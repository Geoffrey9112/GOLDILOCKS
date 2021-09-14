/*******************************************************************************
 * ssxRecover.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ssxRecover.c 9033 2013-07-17 08:40:27Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file ssxRecover.c
 * @brief XA Recovery Scan Routines
 */

#include <qll.h>
#include <sslDef.h>
#include <sslEnv.h>
#include <ssDef.h>
#include <ssx.h>

extern ssgWarmupEntry  * gSsgWarmupEntry;


stlStatus ssxDeleteRecover( stlXid   * aXid,
                            stlBool  * aDeleted,
                            sslEnv   * aEnv )
{
    stlChar               sBuffer[STL_XID_STRING_SIZE];
    void                * sRelationHandle;
    stlXid                sXid;
    knlRegionMark         sMemMark;
    stlInt32              sState = 0;
    knlValueBlockList   * sDeleteColumn = NULL;
    dtlDataValue        * sDataValue;
    smlStatement        * sStatement;
    smlTransId            sTransId;
    void                * sIterator = NULL;
    smlIteratorProperty   sIteratorProperty;
    smlFetchInfo          sFetchInfo;
    smlRowBlock           sRowBlock;
    smlRid                sRowRid;
    smlScn                sRowScn;
    
    
    *aDeleted = STL_FALSE;
    
    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               KNL_ENV(aEnv) ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( knlInitBlockNoBuffer( &sDeleteColumn,
                                   STL_LAYER_SESSION,
                                   1,
                                   DTL_TYPE_VARCHAR,
                                   &aEnv->mOperationHeapMem,
                                   KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sDataValue = KNL_GET_BLOCK_DATA_VALUE( sDeleteColumn, 0 );
    sDataValue->mBufferSize = STL_XID_STRING_SIZE;
    sDataValue->mLength = 0;
    sDataValue->mValue = sBuffer;

    STL_TRY( smlInitSingleRowBlock( &sRowBlock,
                                    &sRowRid,
                                    &sRowScn,
                                    SML_ENV(aEnv) )
             == STL_SUCCESS );
    
    SML_SET_FETCH_INFO_FOR_TABLE( & sFetchInfo,
                                  NULL,        /* physical filter */
                                  NULL,        /* logical filter */
                                  NULL,        /* read column list */
                                  NULL,        /* rowid column list */
                                  & sRowBlock,
                                  0,           /* skip count */
                                  SML_FETCH_COUNT_MAX,
                                  STL_FALSE,   /* group key fetch */
                                  NULL );      /* filter evaluate info */

    sFetchInfo.mColList = sDeleteColumn;
    
    STL_TRY( smlBeginTrans( SML_TIL_READ_COMMITTED,
                            STL_FALSE,  /* aIsGlobalTrans */
                            & sTransId,
                            SML_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 2;

    STL_TRY( ellGetPhyHandle4PendingTransTable( &sRelationHandle,
                                                SML_MAXIMUM_STABLE_SCN,
                                                ELL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    STL_TRY( smlAllocStatement( sTransId,
                                sRelationHandle, /* aUpdateRelHandle */
                                SML_STMT_ATTR_UPDATABLE | SML_STMT_ATTR_LOCKABLE,
                                SML_LOCK_TIMEOUT_INVALID,
                                STL_FALSE, /* aNeedSnapshotIterator */
                                & sStatement,
                                SML_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 3;

    SML_INIT_ITERATOR_PROP( sIteratorProperty );

    STL_TRY( smlAllocIterator( sStatement,
                               sRelationHandle,
                               SML_TRM_COMMITTED,
                               SML_SRM_SNAPSHOT,
                               & sIteratorProperty,
                               SML_SCAN_FORWARD,
                               & sIterator,
                               SML_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 4;
    
    /**
     * 삭제할 Row 를 찾는다.
     */

    while( 1 )
    {
        /**
         * 레코드를 읽는다.
         */
        STL_TRY( smlFetchNext( sIterator,
                               & sFetchInfo, 
                               SML_ENV( aEnv ) )
                 == STL_SUCCESS );

        if( sFetchInfo.mIsEndOfScan == STL_TRUE )
        {
            break;
        }
        else
        {
            /**
             * 레코드 삭제 
             */

            ((stlChar*)(sDataValue->mValue))[sDataValue->mLength] = '\0';
            STL_TRY( dtlStringToXid( sDataValue->mValue,
                                     &sXid,
                                     KNL_ERROR_STACK( aEnv ) )
                     == STL_SUCCESS );

            if( dtlIsEqualXid( aXid, &sXid ) == STL_TRUE )
            {
                STL_TRY( smlDeleteRecord( sStatement,
                                          sRelationHandle,
                                          & sRowRid,
                                          sRowScn,
                                          0,
                                          NULL,
                                          NULL,
                                          NULL,
                                          SML_ENV( aEnv ) )
                         == STL_SUCCESS );
        
                *aDeleted = STL_TRUE;
            }
        }
    }

    sState = 3;
    STL_TRY( smlFreeIterator( sIterator,
                              SML_ENV( aEnv ) )
             == STL_SUCCESS );
    
    sState = 2;
    STL_TRY( smlFreeStatement( sStatement,
                               SML_ENV( aEnv ) )
             == STL_SUCCESS );

    STL_TRY( smlCommit( sTransId,
                        NULL, /* aComment */
                        SML_TRANSACTION_CWM_WAIT,
                        SML_ENV( aEnv ) )
             == STL_SUCCESS );
            
    sState = 1;
    STL_TRY( smlEndTrans( sTransId,
                          SML_PEND_ACTION_COMMIT,
                          SML_TRANSACTION_CWM_WAIT,
                          SML_ENV( aEnv ) )
             == STL_SUCCESS );
                
    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 4:
            (void) smlFreeIterator( sIterator, SML_ENV( aEnv ) );
        case 3:
            (void) smlFreeStatement( sStatement, SML_ENV( aEnv ) );
        case 2:
            (void) smlRollback( sTransId,
                                SML_TRANSACTION_CWM_NO_WAIT,
                                SML_ENV( aEnv ) );
            (void) smlEndTrans( sTransId,
                                SML_PEND_ACTION_ROLLBACK,
                                SML_TRANSACTION_CWM_NO_WAIT,
                                SML_ENV( aEnv ) );
        case 1:
            (void) knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                             &sMemMark,
                                             STL_FALSE, /* aFreeChunk */
                                             KNL_ENV( aEnv ) );
        default:
            break;
    }

    return STL_FAILURE;
}


stlStatus ssxResetRecover( sslEnv * aEnv )
{
    sslXaRecoverScanInfo  * sScanInfo;

    sScanInfo = & aEnv->mXaRecoverScanInfo;

    if( sScanInfo->mScanState != SSL_XA_SCAN_STATE_NONE )
    {
        if( sScanInfo->mScanState == SSL_XA_SCAN_STATE_DB )
        {
            STL_TRY( smlResetIterator( sScanInfo->mIterator,
                                       SML_ENV( aEnv ) )
                     == STL_SUCCESS );
        }
    
        sScanInfo->mScanState = SSL_XA_SCAN_STATE_CONTEXT;
        sScanInfo->mCurContextArray = NULL;
        sScanInfo->mCurContextIdx = 0;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ssxOpenRecover( sslEnv * aEnv )
{
    void                  * sRelationHandle;
    stlInt32                sState = 0;
    dtlDataValue          * sDataValue;
    smlIteratorProperty     sIteratorProperty;
    sslXaRecoverScanInfo  * sScanInfo;

    sScanInfo = & aEnv->mXaRecoverScanInfo;
    
    KNL_INIT_REGION_MARK(&sScanInfo->mMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sScanInfo->mMemMark,
                               KNL_ENV(aEnv) ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( knlInitBlockNoBuffer( &sScanInfo->mColumnList,
                                   STL_LAYER_SESSION,
                                   1,
                                   DTL_TYPE_VARCHAR,
                                   &aEnv->mOperationHeapMem,
                                   KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sDataValue = KNL_GET_BLOCK_DATA_VALUE( sScanInfo->mColumnList, 0 );
    sDataValue->mBufferSize = STL_XID_STRING_SIZE;
    sDataValue->mLength = 0;
    sDataValue->mValue = sScanInfo->mValue;

    STL_TRY( smlInitRowBlock( &sScanInfo->mRowBlock,
                              1,
                              &aEnv->mOperationHeapMem,
                              SML_ENV(aEnv) )
             == STL_SUCCESS );
    
    SML_SET_FETCH_INFO_FOR_TABLE( & sScanInfo->mFetchInfo,
                                  NULL,        /* physical filter */
                                  NULL,        /* logical filter */
                                  NULL,        /* read column list */
                                  NULL,        /* rowid column list */
                                  & sScanInfo->mRowBlock,
                                  0,           /* skip count */
                                  SML_FETCH_COUNT_MAX,
                                  STL_FALSE,   /* group key fetch */
                                  NULL );      /* filter evaluate info */

    sScanInfo->mFetchInfo.mColList = sScanInfo->mColumnList;
    
    STL_TRY( ellGetPhyHandle4PendingTransTable( &sRelationHandle,
                                                SML_MAXIMUM_STABLE_SCN,
                                                ELL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    STL_TRY( smlAllocStatement( SML_INVALID_TRANSID,
                                sRelationHandle, /* aUpdateRelHandle */
                                SML_STMT_ATTR_READONLY,
                                SML_LOCK_TIMEOUT_INVALID,
                                STL_FALSE, /* aNeedSnapshotIterator */
                                & sScanInfo->mStatement,
                                SML_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 2;

    SML_INIT_ITERATOR_PROP( sIteratorProperty );

    STL_TRY( smlAllocIterator( sScanInfo->mStatement,
                               sRelationHandle,
                               SML_TRM_COMMITTED,
                               SML_SRM_RECENT,
                               & sIteratorProperty,
                               SML_SCAN_FORWARD,
                               & sScanInfo->mIterator,
                               SML_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 3;
    
    sScanInfo->mScanState = SSL_XA_SCAN_STATE_CONTEXT;
    sScanInfo->mCurContextArray = NULL;
    sScanInfo->mCurContextIdx = 0;
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 3:
            (void) smlFreeIterator( sScanInfo->mIterator, SML_ENV( aEnv ) );
        case 2:
            (void) smlFreeStatement( sScanInfo->mStatement, SML_ENV( aEnv ) );
        case 1:
            (void) knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                             &sScanInfo->mMemMark,
                                             STL_FALSE, /* aFreeChunk */
                                             KNL_ENV( aEnv ) );
            sScanInfo->mScanState = SSL_XA_SCAN_STATE_NONE;
        default:
            break;
    }

    return STL_FAILURE;
}


stlBool ssxIsOpenRecover( sslEnv * aEnv )
{
    sslXaRecoverScanInfo  * sScanInfo;

    sScanInfo = & aEnv->mXaRecoverScanInfo;
    
    if( sScanInfo->mScanState == SSL_XA_SCAN_STATE_NONE )
    {
        return STL_FALSE;
    }
    
    return STL_TRUE;
}


stlStatus ssxFetchRecover( stlXid   * aXids,
                           stlInt64   aRequireCount,
                           stlInt64 * aFetchedCount,
                           sslEnv   * aEnv )
{
    knlXaContext            sContext;
    stlInt64                sFetchCount = 0;
    dtlDataValue          * sDataValue;
    sslXaRecoverScanInfo  * sScanInfo;
    stlXid                  sXid;
    ssxContextArray       * sContextArray;
    
    *aFetchedCount = 0;

    sScanInfo = & aEnv->mXaRecoverScanInfo;

    STL_DASSERT( sScanInfo->mScanState != SSL_XA_SCAN_STATE_NONE );

    if( sScanInfo->mScanState == SSL_XA_SCAN_STATE_CONTEXT )
    {
        if( sScanInfo->mCurContextArray == NULL )
        {
            sScanInfo->mCurContextArray = STL_RING_GET_FIRST_DATA(
                &gSsgWarmupEntry->mContextArrList );
            sScanInfo->mCurContextIdx = 0; 
        }
        
        while( 1 )
        {
            if( sScanInfo->mCurContextIdx >= SSX_CONTEXT_ARRAY_SIZE )
            {
                sScanInfo->mCurContextArray = STL_RING_GET_NEXT_DATA(
                    &gSsgWarmupEntry->mContextArrList,
                    sScanInfo->mCurContextArray );
                sScanInfo->mCurContextIdx = 0;
                
                if( sScanInfo->mCurContextArray ==
                    (ssxContextArray*)&gSsgWarmupEntry->mContextArrList )
                {
                    sScanInfo->mScanState = SSL_XA_SCAN_STATE_DB;
                    sScanInfo->mCurContextArray = NULL;
                    sScanInfo->mCurContextIdx = 0; 
                    break;
                }
            }

            sContextArray = sScanInfo->mCurContextArray;

            stlMemcpy( &sContext,
                       &sContextArray->mContext[sScanInfo->mCurContextIdx],
                       STL_SIZEOF(knlXaContext) );

            sScanInfo->mCurContextIdx++;
            
            if( sContext.mState == KNL_XA_STATE_PREPARED )
            {
                stlMemcpy( &aXids[sFetchCount], &sContext.mXid, STL_SIZEOF(stlXid) );
                sFetchCount++;

                STL_TRY_THROW( sFetchCount != aRequireCount, RAMP_FINISH );
            }
        }
    }

    if( sScanInfo->mScanState == SSL_XA_SCAN_STATE_DB )
    {
        while( 1 )
        {
            /**
             * 레코드를 읽는다.
             */
            STL_TRY( smlFetchNext( sScanInfo->mIterator,
                                   & sScanInfo->mFetchInfo, 
                                   SML_ENV( aEnv ) )
                     == STL_SUCCESS );

            if( sScanInfo->mFetchInfo.mIsEndOfScan == STL_TRUE )
            {
                break;
            }
            else
            {
                sDataValue = KNL_GET_BLOCK_DATA_VALUE( sScanInfo->mColumnList, 0 );
                
                ((stlChar*)(sDataValue->mValue))[sDataValue->mLength] = '\0';
                STL_TRY( dtlStringToXid( sDataValue->mValue,
                                         &sXid,
                                         KNL_ERROR_STACK( aEnv ) )
                         == STL_SUCCESS );

                stlMemcpy( &aXids[sFetchCount], &sXid, STL_SIZEOF(stlXid) );
                sFetchCount++;

                STL_TRY_THROW( sFetchCount != aRequireCount, RAMP_FINISH );
            }
        }
        
    }

    STL_RAMP( RAMP_FINISH );

    *aFetchedCount = sFetchCount;
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) ssxCloseRecover( aEnv );

    return STL_FAILURE;
}


stlStatus ssxCloseRecover( sslEnv * aEnv )
{
    sslXaRecoverScanInfo  * sScanInfo;
    stlInt32                sState = 0;

    sScanInfo = & aEnv->mXaRecoverScanInfo;

    if( sScanInfo->mScanState == SSL_XA_SCAN_STATE_DB )
    {
        sState = 2;
        STL_TRY( smlFreeIterator( sScanInfo->mIterator,
                                  SML_ENV( aEnv ) )
                 == STL_SUCCESS );
    
        sState = 1;
        STL_TRY( smlFreeStatement( sScanInfo->mStatement,
                                   SML_ENV( aEnv ) )
                 == STL_SUCCESS );

        sState = 0;
        STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                           &sScanInfo->mMemMark,
                                           STL_FALSE, /* aFreeChunk */
                                           KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    sScanInfo->mScanState = SSL_XA_SCAN_STATE_NONE;
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) smlFreeStatement( sScanInfo->mStatement, SML_ENV( aEnv ) );
        case 1:
            (void) knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                             &sScanInfo->mMemMark,
                                             STL_FALSE, /* aFreeChunk */
                                             KNL_ENV( aEnv ) );
            sScanInfo->mScanState = SSL_XA_SCAN_STATE_NONE;
        default:
            break;
    }
    
    return STL_FAILURE;
}


