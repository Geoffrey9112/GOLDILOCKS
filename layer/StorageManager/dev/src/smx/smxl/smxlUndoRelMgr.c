/*******************************************************************************
 * smxlUndoRelMgr.c
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

#include <stl.h>
#include <dtl.h>
#include <knl.h>
#include <scl.h>
#include <smlDef.h>
#include <smDef.h>
#include <smxlDef.h>
#include <smp.h>
#include <sms.h>
#include <smg.h>
#include <smf.h>
#include <sma.h>
#include <smxl.h>
#include <smxm.h>
#include <smrDef.h>
#include <smr.h>
#include <smxlUndoRelMgr.h>

/**
 * @file smxlUndoRelMgr.c
 * @brief Storage Manager Layer Local Transactional Undo Relation Manager Internal Routines
 */

extern smxlWarmupEntry  * gSmxlWarmupEntry;

/**
 * @addtogroup smxl
 * @{
 */
                               
stlStatus smxlAttachUndoRelations( smlEnv * aEnv )
{
    stlInt32       sState = 0;
    smlPid         sSegHdrPid;
    void         * sSegMapHandle;
    void         * sSegHandle;
    void         * sSegIterator;
    stlInt32       sRelSeqId = 0;
    smlRid         sSegRid;
    void         * sRelHandle;
    smlStatement * sStatement;

    sSegMapHandle = smsGetSegmentMapHandle();

    STL_TRY( smaAllocStatement( SML_INVALID_TRANSID,
                                SML_INVALID_TRANSID,
                                NULL,
                                SML_STMT_ATTR_READONLY,
                                SML_LOCK_TIMEOUT_INVALID,
                                STL_FALSE,
                                & sStatement,
                                aEnv ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( smsAllocSegIterator( sStatement,
                                  sSegMapHandle,
                                  &sSegIterator,
                                  aEnv ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( smsGetFirstPage( sSegMapHandle,
                              sSegIterator,
                              &sSegHdrPid,
                              aEnv ) == STL_SUCCESS );

    while( sSegHdrPid != SMP_NULL_PID )
    {
        SML_MAKE_RID( &sSegRid,
                      smsGetTbsId( sSegMapHandle ),
                      sSegHdrPid,
                      0 );
        
        STL_TRY( smsGetSegmentHandle( sSegRid,
                                      &sSegHandle,
                                      aEnv ) == STL_SUCCESS );

        if( sSegHandle != NULL )
        {
            if( smsGetSegType( sSegHandle ) == SML_SEG_TYPE_MEMORY_CIRCULAR )
            {
                STL_TRY( smxlGetRelationSeqId( &sSegRid,
                                               sSegHandle,
                                               &sRelSeqId,
                                               aEnv ) == STL_SUCCESS );
                
                if( sRelSeqId < SMXL_UNDO_REL_COUNT )
                {
                    STL_TRY( smxlBuildRelCache( sSegHandle,
                                                sRelSeqId,
                                                &sRelHandle,
                                                aEnv ) == STL_SUCCESS );
            
                    STL_TRY( smxlInitUndoRelEntry( sRelSeqId,
                                                   sSegRid,
                                                   sRelHandle,
                                                   aEnv ) == STL_SUCCESS );
                }
                else
                {
                    /**
                     * UNDO RELATION 프로퍼티 조정후 Startup 실패시
                     * 정해진 개수보다 많은 세그먼트가 존재할수 있다.
                     */
                }
            }
        }

        STL_TRY( smsGetNextPage( sSegMapHandle,
                                 sSegIterator,
                                 &sSegHdrPid,
                                 aEnv ) == STL_SUCCESS );
    }
    
    sState = 1;
    STL_TRY( smsFreeSegIterator( sSegMapHandle,
                                 sSegIterator,
                                 aEnv ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( smaFreeStatement( sStatement,
                               aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)smaFreeStatement( sStatement, aEnv );
        case 1:
            (void)smsFreeSegIterator( sSegMapHandle, sSegIterator, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus smxlCreateUndoRelations( smxlTransId   aTransId,
                                   smlEnv      * aEnv )
{
    stlInt32     i;
    smlRid       sSegRid;
    void       * sSegHandle;
    void       * sRelHandle;
    smxmTrans    sMiniTrans;
    stlInt32     sState = 0;
    stlUInt32    sAttr = SMXM_ATTR_REDO;
    
    for( i = 0; i < SMXL_UNDO_REL_COUNT; i++ )
    {
        STL_TRY( smxmBegin( &sMiniTrans,
                            aTransId,
                            SML_INVALID_RID,
                            sAttr,
                            aEnv ) == STL_SUCCESS );
        sState = 1;

        STL_TRY( smsAllocSegHdr( &sMiniTrans,
                                 SML_MEMORY_UNDO_SYSTEM_TBS_ID,
                                 SML_SEG_TYPE_MEMORY_CIRCULAR,
                                 NULL,
                                 STL_FALSE,
                                 &sSegRid,
                                 &sSegHandle,
                                 aEnv ) == STL_SUCCESS );

        smxmSetSegmentRid( &sMiniTrans, sSegRid );
        
        sState = 0;
        STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

        STL_TRY( smsCreate( aTransId,
                            SML_MEMORY_UNDO_SYSTEM_TBS_ID,
                            SML_SEG_TYPE_MEMORY_CIRCULAR,
                            sSegHandle,
                            0,  /* aInitialExtents(optimal) */
                            aEnv ) == STL_SUCCESS );

        STL_TRY( smxlInitRelationHeader( aTransId,
                                         i,  /* aRelSeqId */
                                         &sSegRid,
                                         sSegHandle,
                                         aEnv )
                 == STL_SUCCESS );
        
        STL_TRY( smxlBuildRelCache( sSegHandle,
                                    i,  /* aRelSeqId */
                                    &sRelHandle,
                                    aEnv ) == STL_SUCCESS );
            
        STL_TRY( smxlInitUndoRelEntry( i,
                                       sSegRid,
                                       sRelHandle,
                                       aEnv ) == STL_SUCCESS );
    }
                        
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)smxmRollback( &sMiniTrans, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus smxlResetUndoRelations( smlEnv * aEnv )
{
    return STL_SUCCESS;
}

stlStatus smxlAllocUndoRelationInternal( smxlTransId   aTransId,
                                         smlEnv      * aEnv )
{
    stlInt32           i;
    stlBool            sIsSuccess;
    void             * sUndoRelEntry = NULL;
    smxlTrans        * sTrans;
    stlInt32           sLatchState = 0;
    stlUInt32          sHintSlotId;
    smxlUndoRelEntry * sUndoEntry;
    stlInt32           sUndoRelCount;

    sTrans = SMXL_TRANS_SLOT( aTransId );
    STL_ASSERT( sTrans->mState != SMXL_STATE_IDLE );
    STL_ASSERT( sTrans->mUndoRelEntry == NULL );

    STL_TRY( knlGetSessionEnvId( KNL_SESS_ENV(aEnv),
                                 &sHintSlotId,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sUndoRelCount = SMXL_UNDO_REL_COUNT;
    sHintSlotId = (sHintSlotId % sUndoRelCount);
    
    while( 1 )
    {
        for( i = sHintSlotId; i < sUndoRelCount; i++ )
        {
            sUndoEntry = &(SMXL_UNDO_REL_ENTRY(i));
            
            if( sUndoEntry->mIsFree == STL_FALSE )
            {
                continue;
            }

            STL_TRY( knlTryLatch( &sUndoEntry->mLatch,
                                  KNL_LATCH_MODE_EXCLUSIVE,
                                  &sIsSuccess,
                                  KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            if( sIsSuccess == STL_FALSE )
            {
                continue;
            }
            sLatchState = 1;

            if( sUndoEntry->mIsFree == STL_TRUE )
            {
                sUndoEntry->mIsFree = STL_FALSE;

                smxlInitUndoRelCache( aTransId,
                                      (smxlUndoRelCache*)sUndoEntry->mRelHandle );
                
                sLatchState = 0;
                STL_TRY( knlReleaseLatch( &sUndoEntry->mLatch,
                                          (knlEnv*)aEnv )
                         == STL_SUCCESS );

                sUndoRelEntry = (void*)sUndoEntry;
                
                break;
            }

            sLatchState = 0;
            STL_TRY( knlReleaseLatch( &sUndoEntry->mLatch,
                                      (knlEnv*)aEnv )
                     == STL_SUCCESS );
        }

        if( sUndoRelEntry != NULL )
        {
            break;
        }
        
        for( i = 0; i < sHintSlotId; i++ )
        {
            sUndoEntry = &(SMXL_UNDO_REL_ENTRY(i));
            
            if( sUndoEntry->mIsFree == STL_FALSE )
            {
                continue;
            }

            STL_TRY( knlTryLatch( &sUndoEntry->mLatch,
                                  KNL_LATCH_MODE_EXCLUSIVE,
                                  &sIsSuccess,
                                  KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            if( sIsSuccess == STL_FALSE )
            {
                continue;
            }
            sLatchState = 1;

            if( sUndoEntry->mIsFree == STL_TRUE )
            {
                sUndoEntry->mIsFree = STL_FALSE;

                smxlInitUndoRelCache( aTransId,
                                      (smxlUndoRelCache*)sUndoEntry->mRelHandle );
                
                sLatchState = 0;
                STL_TRY( knlReleaseLatch( &sUndoEntry->mLatch,
                                          (knlEnv*)aEnv )
                         == STL_SUCCESS );

                sUndoRelEntry = (void*)sUndoEntry;
                
                break;
            }

            sLatchState = 0;
            STL_TRY( knlReleaseLatch( &sUndoEntry->mLatch,
                                      (knlEnv*)aEnv )
                     == STL_SUCCESS );
        }

        if( sUndoRelEntry != NULL )
        {
            break;
        }
        
        STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );
        
        /*
         * Mock-up에서는 1ms를 기다린다.
         */
        stlSleep( 1 );
    }

    sTrans->mUndoRelEntry = sUndoRelEntry;

    /*
     * Mock-up에서는 일단 막는다.
     */
    STL_ASSERT( sTrans->mUndoRelEntry != NULL );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sLatchState == 1 )
    {
        (void)knlReleaseLatch( &sUndoEntry->mLatch, (knlEnv*)aEnv );
    }

    return STL_FAILURE;
}

stlStatus smxlBindUndoRelation( smxlTransId   aTransId,
                                smlEnv      * aEnv )
{
    stlInt32    i;
    smxlTrans * sTrans;

    sTrans = SMXL_TRANS_SLOT( aTransId );
    STL_ASSERT( sTrans->mState != SMXL_STATE_IDLE );
    STL_ASSERT( SML_IS_INVALID_RID( sTrans->mUndoSegRid ) == STL_FALSE );
    STL_ASSERT( sTrans->mUndoRelEntry == NULL );

    for( i = 0; i < SMXL_UNDO_REL_COUNT; i++ )
    {
        if( SML_IS_EQUAL_RID( SMXL_UNDO_REL_ENTRY(i).mSegRid,
                              sTrans->mUndoSegRid ) )
        {
            SMXL_UNDO_REL_ENTRY(i).mIsFree = STL_FALSE;
            sTrans->mUndoRelEntry = (void*)&SMXL_UNDO_REL_ENTRY(i);
            break;
        }
    }

    STL_ASSERT( sTrans->mUndoRelEntry != NULL );
    
    return STL_SUCCESS;
}

stlStatus smxlFreeUndoRelation( smxlTransId   aTransId,
                                smlEnv      * aEnv )
{
    smxlUndoRelEntry * sUndoRelEntry;
    smxlTrans        * sTrans;

    sTrans = SMXL_TRANS_SLOT( aTransId );

    sUndoRelEntry = (smxlUndoRelEntry*)sTrans->mUndoRelEntry;
    sUndoRelEntry->mIsFree = STL_TRUE;

    sTrans->mUndoRelEntry = NULL;

    return STL_SUCCESS;
}

inline void smxlSetUndoSegRid( smxlTransId aTransId,
                               smlRid      aUndoSegRid )
{
    smxlTrans * sTrans;

    sTrans = SMXL_TRANS_SLOT( aTransId );
    STL_ASSERT( sTrans->mState != SMXL_STATE_IDLE );
    STL_ASSERT( sTrans->mTransId == aTransId );
    
    sTrans->mUndoSegRid = aUndoSegRid;
}

inline void smxlSetUndoSegRid4MediaRecovery( smxlTransId   aTransId,
                                             smlRid        aUndoSegRid,
                                             smlEnv      * aEnv )
{
    smxlTrans * sTrans;

    sTrans = SMXL_MEDIA_RECOVERY_TRANS_SLOT( aEnv, aTransId );
    STL_ASSERT( sTrans->mState != SMXL_STATE_IDLE );
    STL_ASSERT( sTrans->mTransId == aTransId );
    
    sTrans->mUndoSegRid = aUndoSegRid;
}

void * smxlGetUndoSegHandleByUndoRelId( stlInt32 aUndoRelId )
{
    void * sUndoRelHandle = NULL;
    void * sUndoSegHandle = NULL;
    
    if( aUndoRelId < SMXL_UNDO_REL_COUNT )
    {
        sUndoRelHandle = smxlGetUndoRelHandle( &SMXL_UNDO_REL_ENTRY(aUndoRelId) );

        if( sUndoRelHandle != NULL )
        {
            sUndoSegHandle = ((smxlUndoRelCache*)sUndoRelHandle)->mSegHandle;
        }
    }
    
    return sUndoSegHandle;
}

/** @} */

/**
 * @addtogroup smxlUndoRelMgr
 * @{
 */

stlStatus smxlInitUndoRelEntry( stlInt16   aRelId,
                                smlRid     aSegRid,
                                void     * aRelHandle,
                                smlEnv   * aEnv )
{
    SMXL_UNDO_REL_ENTRY(aRelId).mRelId = aRelId;
    SMXL_UNDO_REL_ENTRY(aRelId).mSegRid = aSegRid;
    SMXL_UNDO_REL_ENTRY(aRelId).mRelHandle = aRelHandle;
    SMXL_UNDO_REL_ENTRY(aRelId).mIsFree = STL_TRUE;

    STL_TRY( knlInitLatch( &SMXL_UNDO_REL_ENTRY(aRelId).mLatch,
                           STL_TRUE,
                           (knlEnv*)aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

void smxlInitUndoRelCache( smxlTransId        aTransId,
                           smxlUndoRelCache * aUndoRelCache )
{
    aUndoRelCache->mTransId = aTransId;
    aUndoRelCache->mTransLstUndoRid = SML_INVALID_RID;
}

void * smxlGetUndoRelHandle( smxlUndoRelEntry * aUndoRelEntry )
{
    return aUndoRelEntry->mRelHandle;
}

stlStatus smxlBuildRelCache( void      * aSegHandle,
                             stlInt32    aRelSeqId,
                             void     ** aRelHandle,
                             smlEnv    * aEnv )
{
    smxlUndoRelCache * sCache;
    
    STL_TRY( smgAllocShmMem4Open( STL_SIZEOF( smxlUndoRelCache ),
                                  aRelHandle,
                                  aEnv ) == STL_SUCCESS );

    sCache = (smxlUndoRelCache*)(*aRelHandle);
    sCache->mSegHandle = aSegHandle;
    sCache->mTransId = SML_INVALID_TRANSID;
    sCache->mRelSeqId = aRelSeqId;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

inline void * smxlGetUndoRelEntry( smxlTransId aTransId )
{
    smxlTrans * sTrans;
    void      * sUndoRelEntry;

    if( aTransId == SML_INVALID_TRANSID )
    {
        sUndoRelEntry = NULL;
    }
    else
    {
        sTrans = SMXL_TRANS_SLOT( aTransId );
        STL_ASSERT( sTrans->mState != SMXL_STATE_IDLE );
        sUndoRelEntry = sTrans->mUndoRelEntry;
    }

    return sUndoRelEntry;
}

stlStatus smxlRestructureUndoRelation( smlEnv * aEnv )
{
    stlInt64          sNewUndoRelCount = 0;
    stlInt64          sOldUndoRelCount = 0;
    smxlWarmupEntry * sWarmupEntry;
    smxlTransId       sTransId;
    stlUInt64         sTransSeq;
    stlInt32          i;
    stlInt32          sState = 0;
    smxmTrans         sMiniTrans;
    smlRid            sSegRid;
    smlRid            sUndoRid;
    void            * sSegHandle;
    void            * sRelHandle;

    sWarmupEntry = gSmxlWarmupEntry;
    
    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_UNDO_RELATION_COUNT,
                                      &sNewUndoRelCount,
                                      KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    sOldUndoRelCount = smfGetUndoRelCount();

    if( sOldUndoRelCount == 0 )
    {
        /**
         * CREATE DATABASE의 경우는 Control 파일이 존재하지 않기 때문에 0 일수 있다.
         */
        sWarmupEntry->mUndoRelCount = sNewUndoRelCount;
        smfSetUndoRelCount( sNewUndoRelCount );
    
        STL_THROW( RAMP_SUCCESS );
    }
    
    STL_TRY_THROW( sNewUndoRelCount != sOldUndoRelCount, RAMP_SUCCESS );

    STL_TRY( smxlAllocTrans( SML_TIL_READ_COMMITTED,
                             STL_FALSE, /* aIsGlobalTrans */
                             &sTransId,
                             &sTransSeq,
                             aEnv ) == STL_SUCCESS );
    sState = 1;

    smxlSetTransAttr( sTransId, SMXL_ATTR_UPDATABLE );
    
    if( sNewUndoRelCount > sOldUndoRelCount )
    {
        STL_TRY( smxlInsertTransRecord( sTransId, aEnv ) == STL_SUCCESS );

        for( i = sOldUndoRelCount; i < sNewUndoRelCount; i++ )
        {
            STL_TRY( smxmBegin( &sMiniTrans,
                                sTransId,
                                SML_INVALID_RID,
                                SMXM_ATTR_REDO,
                                aEnv ) == STL_SUCCESS );
            sState = 2;

            STL_TRY( smsAllocSegHdr( &sMiniTrans,
                                     SML_MEMORY_UNDO_SYSTEM_TBS_ID,
                                     SML_SEG_TYPE_MEMORY_CIRCULAR,
                                     NULL,
                                     STL_FALSE,
                                     &sSegRid,
                                     &sSegHandle,
                                     aEnv ) == STL_SUCCESS );

            smxmSetSegmentRid( &sMiniTrans, sSegRid );
        
            STL_TRY( smxlInsertUndoRecord( &sMiniTrans,
                                           SMG_COMPONENT_TRANSACTION,
                                           SMXL_UNDO_LOG_MEMORY_CIRCULAR_CREATE,
                                           NULL,
                                           0,
                                           sSegRid,
                                           &sUndoRid,
                                           aEnv ) == STL_SUCCESS );
            
            sState = 1;
            STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

            STL_TRY( smsCreate( sTransId,
                                SML_MEMORY_UNDO_SYSTEM_TBS_ID,
                                SML_SEG_TYPE_MEMORY_CIRCULAR,
                                sSegHandle,
                                0,  /* aInitialExtents(optimal) */
                                aEnv ) == STL_SUCCESS );

            STL_TRY( smxlInitRelationHeader( sTransId,
                                             i,  /* aRelSeqId */
                                             &sSegRid,
                                             sSegHandle,
                                             aEnv )
                     == STL_SUCCESS );
        
            STL_TRY( smxlBuildRelCache( sSegHandle,
                                        i,  /* aRelSeqId */
                                        &sRelHandle,
                                        aEnv ) == STL_SUCCESS );
            
            STL_TRY( smxlInitUndoRelEntry( i,
                                           sSegRid,
                                           sRelHandle,
                                           aEnv ) == STL_SUCCESS );
        }

        KNL_BREAKPOINT( KNL_BREAKPOINT_SMXLRESTRUCTUREUNDORELATION_AFTER_EXPAND_UNDO_RELATION,
                        KNL_ENV(aEnv) );
    }
    else
    {
        for( i = sNewUndoRelCount; i < sOldUndoRelCount; i++ )
        {
            sSegHandle = smxlGetUndoSegHandleByUndoRelId( i );

            /**
             * UNDO shinking 이후 비정상 종료시 Segment Handle이 NULL로 반환될수 있다.
             */
            if( sSegHandle != NULL )
            {
                /**
                 * 에러가 발생할수 없다.
                 */
                STL_TRY_THROW( smsDrop( sTransId,
                                        sSegHandle,
                                        STL_FALSE,
                                        aEnv ) == STL_SUCCESS,
                               RAMP_ERR_INTERNAL );
            }
        }
    }

    sWarmupEntry->mUndoRelCount = sNewUndoRelCount;
    
    sState = 0;
    STL_TRY( smxlCommit( sTransId,
                         NULL, /* aComment */
                         SML_TRANSACTION_CWM_WAIT,
                         aEnv ) == STL_SUCCESS );
    STL_TRY( smxlFreeTrans( sTransId,
                            SML_PEND_ACTION_COMMIT,
                            STL_FALSE, /* aCleanup */
                            SML_TRANSACTION_CWM_WAIT,
                            aEnv ) == STL_SUCCESS );
    
    smfSetUndoRelCount( sNewUndoRelCount );
    
    KNL_BREAKPOINT( KNL_BREAKPOINT_SMXLRESTRUCTUREUNDORELATION_AFTER_COMMIT,
                    KNL_ENV(aEnv) );
    
    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV(aEnv),
                        KNL_LOG_LEVEL_WARNING,
                        "[STARTUP] restructure undo relations - old(%d), new(%d)",
                        sOldUndoRelCount,
                        sNewUndoRelCount )
             == STL_SUCCESS );
    
    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INTERNAL,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      "smxlRestructureUndoRelation()",
                      0 );
        
        KNL_SYSTEM_FATAL_DASSERT( STL_FALSE, KNL_ENV(aEnv) );
    }

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) smxmCommit( &sMiniTrans, aEnv );
        case 1:
            (void) smxlRollback( sTransId,
                                 SML_TRANSACTION_CWM_WAIT,
                                 STL_FALSE,
                                 aEnv );
            (void) smxlFreeTrans( sTransId,
                                  SML_PEND_ACTION_ROLLBACK,
                                  STL_FALSE, /* aCleanup */
                                  SML_TRANSACTION_CWM_WAIT,
                                  aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus smxlInitRelationHeader( smxlTransId   aTransId,
                                  stlInt32      aRelSeqId,
                                  smlRid      * aSegmentRid,
                                  void        * aSegmentHandle,
                                  smlEnv      * aEnv )
{
    smxlRelHdr    sRelHeader;
    smxmTrans     sMiniTrans;
    stlUInt32     sAttr = SMXM_ATTR_REDO;
    smpHandle     sPageHandle;
    stlChar     * sPtr;
    stlUInt32     sState = 0;

    stlMemset( &sRelHeader, 0x00, STL_SIZEOF(smxlRelHdr) );
    
    sRelHeader.mRelSeqId = aRelSeqId;

    STL_TRY( smxmBegin( &sMiniTrans,
                        aTransId,
                        *aSegmentRid,
                        sAttr,
                        aEnv ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( smpAcquire( &sMiniTrans,
                         aSegmentRid->mTbsId,
                         aSegmentRid->mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

    sPtr = smpGetBody(SMP_FRAME(&sPageHandle)) + smsGetHeaderSize( aSegmentHandle);

    stlMemcpy( sPtr, &sRelHeader, STL_SIZEOF(smxlRelHdr) );

    STL_TRY( smxmWriteLog( &sMiniTrans,
                           SMG_COMPONENT_RELATION,
                           SMR_LOG_INIT_REL_HEADER,
                           SMR_REDO_TARGET_PAGE,
                           sPtr,
                           STL_SIZEOF(smxlRelHdr),
                           smpGetTbsId( &sPageHandle ),
                           smpGetPageId( &sPageHandle ),
                           sPtr - (stlChar*)SMP_FRAME(&sPageHandle),
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 1 :
            (void) smxmCommit( &sMiniTrans, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus smxlGetRelationSeqId( smlRid   * aSegmentRid,
                                void     * aSegmentHandle,
                                stlInt32 * aRelSeqId,
                                smlEnv   * aEnv )
{
    smxlRelHdr  * sRelHeader;
    smpHandle     sPageHandle;
    stlUInt32     sState = 0;
    
    STL_TRY( smpFix( aSegmentRid->mTbsId,
                     aSegmentRid->mPageId,
                     &sPageHandle,
                     aEnv ) == STL_SUCCESS );
    sState = 0;

    sRelHeader = (smxlRelHdr*)(smpGetBody(SMP_FRAME(&sPageHandle)) +
                               smsGetHeaderSize( aSegmentHandle));

    *aRelSeqId = sRelHeader->mRelSeqId;

    sState = 0;
    STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void) smpUnfix( &sPageHandle, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}
                                  
/** @} */
