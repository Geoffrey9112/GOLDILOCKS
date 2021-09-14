/*******************************************************************************
 * smxsManger.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <sml.h>
#include <smlDef.h>
#include <smDef.h>
#include <smxl.h>
#include <sma.h>
#include <smg.h>
#include <smkl.h>
#include <smxlDef.h>
#include <smxlUndoRelation.h>

/**
 * @file smxlSavepoint.c
 * @brief Storage Manager Layer Savepoint Internal Routines
 */

/**
 * @addtogroup smxl
 * @{
 */

stlStatus smxlMarkSavepoint( smxlTransId   aTransId,
                             stlChar     * aSavepointName,
                             smlEnv      * aEnv )
{
    smxlSavepoint * sSavepoint;
    smxlTrans     * sTrans;
    stlBool         sFound = STL_FALSE;

    sTrans = SMXL_TRANS_SLOT( aTransId );
    STL_ASSERT( sTrans->mState == SMXL_STATE_ACTIVE );

    STL_RING_FOREACH_ENTRY( &sTrans->mSavepointList, sSavepoint )
    {
        if( stlStrcmp( sSavepoint->mName, aSavepointName ) == 0 )
        {
            sFound = STL_TRUE;
            STL_RING_UNLINK( &sTrans->mSavepointList, sSavepoint );
            break;
        }
    }

    if( sFound == STL_FALSE )
    {
        STL_TRY( smxlAllocShmMem( aTransId,
                                  STL_SIZEOF( smxlSavepoint ),
                                  (void**)&sSavepoint,
                                  aEnv ) == STL_SUCCESS );
    }

    stlStrncpy( sSavepoint->mName,
                aSavepointName,
                STL_MAX_SQL_IDENTIFIER_LENGTH );

    sSavepoint->mLstUndoRid = smxlGetLstUndoRid( aTransId );
    sSavepoint->mTimestamp = smxlGetCurTimestamp( aTransId );
    
    /**
     * 락 리스트상의 Savepoint를 설정한다.
     */
    STL_TRY( smklSavepoint( aTransId,
                            &sSavepoint->mLockSavepoint,
                            aEnv ) == STL_SUCCESS );

    STL_RING_INIT_DATALINK( sSavepoint,
                            STL_OFFSETOF( smxlSavepoint, mSavepointLink ) );

    STL_RING_ADD_LAST( &sTrans->mSavepointList, sSavepoint );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smxlReleaseSavepoint( smxlTransId   aTransId,
                                stlChar     * aSavepointName,
                                smlEnv      * aEnv )
{
    smxlSavepoint * sSavepoint;
    smxlTrans     * sTrans;
    stlBool         sFound = STL_FALSE;
    smxlSavepoint * sNextSavepoint;

    sTrans = SMXL_TRANS_SLOT( aTransId );
    STL_ASSERT( sTrans->mState == SMXL_STATE_ACTIVE );

    STL_RING_FOREACH_ENTRY_SAFE( &sTrans->mSavepointList, sSavepoint, sNextSavepoint )
    {
        if( stlStrcmp( sSavepoint->mName, aSavepointName ) == 0 )
        {
            sFound = STL_TRUE;
        }

        if( sFound == STL_TRUE )
        {
            STL_RING_UNLINK( &sTrans->mSavepointList, sSavepoint );
        }
    }

    STL_ASSERT( sFound == STL_TRUE );
    
    return STL_SUCCESS;
}

stlBool smxlExistSavepoint( smxlTransId   aTransId,
                            stlChar     * aSavepointName,
                            stlInt64    * aSavepointTimestamp,
                            smlEnv      * aEnv )
{
    smxlSavepoint * sSavepoint;
    smxlTrans     * sTrans;
    stlBool         sFound = STL_FALSE;

    sTrans = SMXL_TRANS_SLOT( aTransId );
    STL_ASSERT( sTrans->mState == SMXL_STATE_ACTIVE );

    STL_RING_FOREACH_ENTRY( &sTrans->mSavepointList, sSavepoint )
    {
        if( stlStrcmp( sSavepoint->mName, aSavepointName ) == 0 )
        {
            *aSavepointTimestamp = sSavepoint->mTimestamp;
            sFound = STL_TRUE;
            break;
        }
    }

    return sFound;
}

stlStatus smxlRollbackToSavepoint( smxlTransId   aTransId,
                                   stlChar     * aSavepointName,
                                   smlEnv      * aEnv )
{
    smxlSavepoint * sSavepoint;
    smxlSavepoint * sTargetSavepoint = NULL;
    smxlSavepoint * sPrevSavepoint;
    smxlTrans     * sTrans;
    stlBool         sFound = STL_FALSE;

    STL_ASSERT( aSavepointName != NULL );
    
    sTrans = SMXL_TRANS_SLOT( aTransId );
    STL_ASSERT( sTrans->mState == SMXL_STATE_ACTIVE );
    
    STL_RING_FOREACH_ENTRY_REVERSE_SAFE( &sTrans->mSavepointList, sSavepoint, sPrevSavepoint )
    {
        if( stlStrcmp( sSavepoint->mName, aSavepointName ) == 0 )
        {
            sTargetSavepoint = sSavepoint;
            sFound = STL_TRUE;
            break;
        }

        STL_RING_UNLINK( &sTrans->mSavepointList, sSavepoint );
    }

    STL_ASSERT( sFound == STL_TRUE );

    if( smxlNeedRollback( aTransId ) == STL_TRUE )
    {
        STL_TRY( smxlRollbackUndoRecords( sTrans->mTransId,
                                          sTargetSavepoint->mLstUndoRid,
                                          STL_FALSE,  /* aRestartRollback */
                                          aEnv ) == STL_SUCCESS );
    }

    STL_TRY( smaCloseNonHoldableIterators( sTargetSavepoint->mTimestamp,
                                           aEnv ) == STL_SUCCESS );

    /**
     * Segment Aging과 Unlock의 동시성 문제로 인하여 Agable Scn의 증가를 막는다.
     * - Unlock 이후에 Enable시킨다.
     */
    smlSetScnToDisableAging( aTransId,
                             SML_SESS_ENV(aEnv) );
    
    STL_TRY( smgExecutePendOp( SML_PEND_ACTION_ROLLBACK,
                               sTargetSavepoint->mTimestamp,
                               aEnv )
             == STL_SUCCESS );

    STL_TRY( smklUnlockForSavepoint( aTransId,
                                     &sTargetSavepoint->mLockSavepoint,
                                     aEnv )
             == STL_SUCCESS );
    
    /**
     * Lock의 해제로 Segment의 aging을 허락한다.
     */
    smlUnsetScnToEnableAging( SML_SESS_ENV(aEnv) );
    
    return STL_SUCCESS;

    STL_FINISH;

    smlUnsetScnToEnableAging( SML_SESS_ENV(aEnv) );
    
    return STL_FAILURE;
}

/** @} */
