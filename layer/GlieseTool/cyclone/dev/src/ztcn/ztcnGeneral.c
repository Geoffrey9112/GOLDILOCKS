/*******************************************************************************
 * ztcnGeneral.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file ztcnGeneral.c
 * @brief GlieseTool Cyclone Sync General Routines
 */

#include <goldilocks.h>
#include <ztc.h>

extern stlBool        gRunState;
extern ztcMasterMgr * gMasterMgr;
extern ztcSyncMgr   * gSyncMgr;

/**
 * @brief Sync 기능을 수행한다.
 * @param[in,out] aErrorStack error stack 
 */ 
stlStatus ztcnSync( stlErrorStack * aErrorStack )
{
    ztcSyncMgr          sSyncMgr;
    stlInt32            sState       = 0;
    ztcSyncher        * sSyncher     = NULL;
    
    gSyncMgr               = &sSyncMgr;
    sSyncMgr.mSyncherCount = ztcmGetSyncherCount();
    sSyncMgr.mArraySize    = ztcmGetSyncArraySize();

    /**
     * 1. Initialize:
     *  Create Dynamic Allocator
     *  Connect to Master DB
     *  Alloc & Init Sync Buffer
     */ 
    STL_TRY( ztcnInitializeSyncMgr( &sSyncMgr,
                                    aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztcmLogMsg( aErrorStack,
                         "[SYNC] Initialize SyncManager Done.\n" ) == STL_SUCCESS );

    /**
     * 3. Create Syncher Thread
     */

    STL_RING_FOREACH_ENTRY( &sSyncMgr.mSyncherList, sSyncher )
    {
        STL_TRY( ztctCreateSyncherThread( sSyncher,
                                          aErrorStack ) == STL_SUCCESS );
    }
    sState = 2;

    /**
     * 4. Do Sync Mgr
     */
    STL_TRY( ztcnDoSyncMgr( &sSyncMgr,
                            aErrorStack ) == STL_SUCCESS );
    

    sState = 1;
    STL_RING_FOREACH_ENTRY( &sSyncMgr.mSyncherList, sSyncher )
    {
        STL_TRY( ztctJoinSyncherThread( sSyncher,
                                        aErrorStack ) == STL_SUCCESS );
    }

    sState = 0;
    STL_TRY( ztcnFinalizeSyncMgr( &sSyncMgr,
                                  aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcmLogMsg( aErrorStack,
                         "[SYNC] Finalize SyncManager Done.\n" ) == STL_SUCCESS );

    gSyncMgr = NULL;
    
    return STL_SUCCESS;
    
    STL_FINISH;

    gSyncMgr  = NULL;
    gRunState = STL_FALSE;

    switch( sState )
    {
        case 2:
            STL_RING_FOREACH_ENTRY( &sSyncMgr.mSyncherList, sSyncher )
            {
                (void) ztctJoinSyncherThread( sSyncher,
                                              aErrorStack );
            }
        case 1:
            (void) ztcnFinalizeSyncMgr( &sSyncMgr,
                                        aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}

/**
 * @brief Sync Manager를 수행하기 전에 준비 작업을 한다.
 * @param[in,out] aSyncMgr    sync manager 
 * @param[in,out] aErrorStack error stack 
 */ 
stlStatus ztcnInitializeSyncMgr( ztcSyncMgr    * aSyncMgr,
                                 stlErrorStack * aErrorStack )
{
    stlInt32            sState = 0;
    
    /** 1: Create Dynamic Allocator */
    STL_TRY( stlCreateDynamicAllocator( &aSyncMgr->mDynamicMem,
                                        ZTC_REGION_INIT_SIZE,
                                        ZTC_REGION_NEXT_SIZE,
                                        aErrorStack ) == STL_SUCCESS );

    sState = 1;
    
    /** 2: Alloc Env Handle */
    STL_TRY( ztcdAllocEnvHandle( &aSyncMgr->mEnvHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 2;

    /** 3: Alloc Env Handle for Slave */
    STL_TRY( ztcdAllocEnvHandle( &aSyncMgr->mSlaveEnvHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 3;

    /** 4: Create Connection */
    STL_TRY( ztcdCreateConnection( &aSyncMgr->mDbcHandle,
                                   aSyncMgr->mEnvHandle,
                                   STL_FALSE, /* aIsAutoCommit */
                                   aErrorStack ) == STL_SUCCESS );
    sState = 4;

    /** 5: Create Connection to slave */
    STL_TRY( ztcdCreateConnection4Syncher( &aSyncMgr->mSlaveDbcHandle,
                                           aSyncMgr->mSlaveEnvHandle,
                                           STL_TRUE,
                                           aErrorStack ) == STL_SUCCESS );
    sState = 5;
    
    STL_RING_INIT_HEADLINK( &aSyncMgr->mSyncherList,
                            STL_OFFSETOF( ztcSyncher, mLink ) );

    STL_RING_INIT_HEADLINK( &aSyncMgr->mBufferInfoList,
                            STL_OFFSETOF( ztcSyncBufferInfo, mLink ) );

    STL_RING_INIT_HEADLINK( &aSyncMgr->mTableList,
                            STL_OFFSETOF( ztcSyncTableInfo, mLink ) );

    STL_TRY( ztcdSetDTFuncValue( aSyncMgr->mDbcHandle,
                                 &aSyncMgr->mCharacterSet,
                                 &aSyncMgr->mTimezone,
                                 aErrorStack ) == STL_SUCCESS );

    STL_TRY( ztcmSetSyncDTFuncVector( &aSyncMgr->mDTVector ) == STL_SUCCESS );

    /** 6: alloc row status*/
    STL_TRY( stlAllocDynamicMem( &aSyncMgr->mDynamicMem,
                                 STL_SIZEOF( SQLUSMALLINT ) * aSyncMgr->mArraySize,
                                 (void**)&aSyncMgr->mArrRowStatus,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 6;
    
    aSyncMgr->mRowFetchedCount = 0;

    /** 7: Init Table Info and Init & Get Column Info */
    STL_TRY( ztcnInitializeSyncTableInfo( aSyncMgr,
                                          gMasterMgr,
                                          aErrorStack ) == STL_SUCCESS );
    sState = 7;
    
    /** 8: Alloc Sync Buffer */
    STL_TRY( ztcnInitailzeSyncBufferInfoNSyncher( aSyncMgr,
                                                  aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    switch( sState )
    {
        case 7:
            (void) ztcnFinalizeSyncTableInfo( aSyncMgr,
                                              aErrorStack );
        case 6:
            (void) stlFreeDynamicMem( &aSyncMgr->mDynamicMem,
                                      (void*) aSyncMgr->mArrRowStatus,
                                      aErrorStack );
        case 5:
            (void) ztcdDestroyConnection( aSyncMgr->mSlaveDbcHandle,
                                          aErrorStack );
        case 4:
            (void) ztcdDestroyConnection( aSyncMgr->mDbcHandle,
                                          aErrorStack );
        case 3:
            (void) ztcdFreeEnvHandle( aSyncMgr->mSlaveEnvHandle,
                                      aErrorStack );
        case 2:
            (void) ztcdFreeEnvHandle( aSyncMgr->mEnvHandle,
                                      aErrorStack );
        case 1:
            (void) stlDestroyDynamicAllocator( &aSyncMgr->mDynamicMem,
                                               aErrorStack );
        default:
            break;
        
    }
    
    return STL_FAILURE;
}

/**
 * @brief Sync Manager를 정리한다.
 * @param[in,out] aSyncMgr    sync manager 
 * @param[in,out] aErrorStack error stack 
 */ 
stlStatus ztcnFinalizeSyncMgr( ztcSyncMgr    * aSyncMgr,
                               stlErrorStack * aErrorStack )
{
    stlInt32            sState = 7;

    STL_TRY( ztcnFinalizeSyncBufferInfoNSyncher( aSyncMgr,
                                                 aErrorStack ) == STL_SUCCESS );
    
    sState = 6;
    STL_TRY( ztcnFinalizeSyncTableInfo( aSyncMgr,
                                        aErrorStack ) == STL_SUCCESS );

    sState = 5;
    STL_TRY( stlFreeDynamicMem( &aSyncMgr->mDynamicMem,
                                (void*) aSyncMgr->mArrRowStatus,
                                aErrorStack ) == STL_SUCCESS );

    sState = 4;
    STL_TRY( ztcdDestroyConnection( aSyncMgr->mSlaveDbcHandle,
                                    aErrorStack ) == STL_SUCCESS );
    
    sState = 3;
    STL_TRY( ztcdDestroyConnection( aSyncMgr->mDbcHandle,
                                    aErrorStack ) == STL_SUCCESS );

    sState = 2;
    STL_TRY( ztcdFreeEnvHandle( aSyncMgr->mSlaveEnvHandle,
                                aErrorStack ) == STL_SUCCESS );
    
    sState = 1;
    STL_TRY( ztcdFreeEnvHandle( aSyncMgr->mEnvHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( stlDestroyDynamicAllocator( &aSyncMgr->mDynamicMem,
                                         aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 7:
            (void) ztcnFinalizeSyncTableInfo( aSyncMgr,
                                              aErrorStack );
        case 6:
            (void) stlFreeDynamicMem( &aSyncMgr->mDynamicMem,
                                      (void*) aSyncMgr->mArrRowStatus,
                                      aErrorStack );
        case 5:
            (void) ztcdDestroyConnection( aSyncMgr->mSlaveDbcHandle,
                                          aErrorStack );
        case 4:
            (void) ztcdDestroyConnection( aSyncMgr->mDbcHandle,
                                          aErrorStack );
        case 3:
            (void) ztcdFreeEnvHandle( aSyncMgr->mSlaveEnvHandle,
                                      aErrorStack );
        case 2:
            (void) ztcdFreeEnvHandle( aSyncMgr->mEnvHandle,
                                      aErrorStack );
        case 1:
            (void) stlDestroyDynamicAllocator( &aSyncMgr->mDynamicMem,
                                               aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


/**
 * @brief Syncher를 수행하기 전에 준비한다.
 * @param[in,out] aSyncher    syncher 
 * @param[in,out] aErrorStack error stack 
 */ 
stlStatus ztcnInitializeSyncher( ztcSyncher    * aSyncher,
                                 stlErrorStack * aErrorStack )
{
    stlInt32   sState = 0;

    STL_TRY( stlCreateRegionAllocator( &aSyncher->mRegionMem,
                                       ZTC_REGION_INIT_SIZE,
                                       ZTC_REGION_NEXT_SIZE,
                                       aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztcdAllocEnvHandle( &aSyncher->mEnvHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 2;
    
    STL_TRY( ztcdCreateConnection4Syncher( &aSyncher->mDbcHandle,
                                           aSyncher->mEnvHandle,
                                           STL_TRUE,
                                           aErrorStack ) == STL_SUCCESS );
    sState = 3;

    STL_TRY( stlAllocRegionMem( &aSyncher->mRegionMem,
                                STL_SIZEOF( SQLUSMALLINT ) * aSyncher->mArraySize,
                                (void**)&aSyncher->mArrRecStatus,
                                aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 3:
            (void) ztcdDestroyConnection( aSyncher->mDbcHandle,
                                          aErrorStack );
        case 2:
            (void) ztcdFreeEnvHandle( aSyncher->mEnvHandle,
                                      aErrorStack );
        case 1:
            (void) stlDestroyRegionAllocator( &aSyncher->mRegionMem,
                                              aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}

/**
 * @brief Syncher를 정리한다.
 * @param[in,out] aSyncher    syncher
 * @param[in,out] aErrorStack error stack 
 */ 
stlStatus ztcnFinalizeSyncher( ztcSyncher    * aSyncher,
                               stlErrorStack * aErrorStack )
{
    stlInt32 sState = 3;

    STL_TRY( ztcdDestroyConnection( aSyncher->mDbcHandle,
                                    aErrorStack ) == STL_SUCCESS );

    sState = 2;
    STL_TRY( ztcdFreeEnvHandle( aSyncher->mEnvHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 1;
    STL_TRY( stlDestroyRegionAllocator( &aSyncher->mRegionMem,
                                        aErrorStack ) == STL_SUCCESS );
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) ztcdFreeEnvHandle( aSyncher->mEnvHandle,
                                      aErrorStack );
        case 1:
            (void) stlDestroyRegionAllocator( &aSyncher->mRegionMem,
                                              aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief Sync Manager에서 Syncher와 Buffer Infomation을 할당한다.
 * @param[in,out] aSyncMgr    sync manager 
 * @param[in,out] aErrorStack error stack
 * @remark Syncher에 BufferInfo Pointer 연결을 확실하기 위해 하나의 함수에서 할당한다.
 */ 
stlStatus ztcnInitailzeSyncBufferInfoNSyncher( ztcSyncMgr    * aSyncMgr,
                                               stlErrorStack * aErrorStack )
{
    stlInt32            i;
    ztcSyncBufferInfo * sBufferInfo = NULL;
    ztcSyncher        * sSyncher    = NULL;

    for( i = 0; i < aSyncMgr->mSyncherCount; i++ )
    {
        /** ztcSyncBufferInfo 할당 */
        STL_TRY( stlAllocDynamicMem( &(aSyncMgr->mDynamicMem),
                                     STL_SIZEOF( ztcSyncBufferInfo ),
                                     (void**) &sBufferInfo,
                                     aErrorStack ) == STL_SUCCESS );

        STL_TRY( stlCreateSemaphore( &sBufferInfo->mSyncSem,
                                     "Sync",
                                     0,
                                     aErrorStack ) == STL_SUCCESS );
    
        STL_RING_INIT_HEADLINK( &sBufferInfo->mColumnValueList,
                                STL_OFFSETOF( ztcSyncColumnValue, mLink ) );

        STL_RING_INIT_DATALINK( sBufferInfo,
                                STL_OFFSETOF( ztcSyncBufferInfo, mLink ) );
    
        sBufferInfo->mNextJob       = ZTC_SYNC_NEXT_JOB_NONE;
        sBufferInfo->mIsFetched     = STL_FALSE;
        sBufferInfo->mFetchedCount  = 0;
        sBufferInfo->mErrCount      = 0;
        sBufferInfo->mTableInfoPtr  = NULL;

        /** ztcSyncher 할당 */
        STL_TRY( stlAllocDynamicMem( &(aSyncMgr->mDynamicMem),
                                     STL_SIZEOF( ztcSyncher ),
                                     (void**) &sSyncher,
                                     aErrorStack ) == STL_SUCCESS );

        STL_RING_INIT_DATALINK( sSyncher,
                                STL_OFFSETOF( ztcSyncher, mLink ) );
        sSyncher->mSyncherId     = i;
        sSyncher->mBufferInfoPtr = sBufferInfo;
        sSyncher->mArraySize     = aSyncMgr->mArraySize;
        
        STL_RING_ADD_LAST( &aSyncMgr->mBufferInfoList, sBufferInfo );
        STL_RING_ADD_LAST( &aSyncMgr->mSyncherList, sSyncher );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_RING_FOREACH_ENTRY( &aSyncMgr->mBufferInfoList, sBufferInfo )
    {
        (void) stlDestroySemaphore( &sBufferInfo->mSyncSem,
                                    aErrorStack );
    }

    return STL_FAILURE;
}

/**
 * @brief Sync Manager에서 Buffer Infomation을 정리한다.
 * @param[in]     aSyncMgr         sync manager
 * @param[in]     aSyncBufferInfo  sync buffer info
 * @param[in,out] aErrorStack      error stack 
 */
stlStatus ztcnFinalizeSyncBufferInfoNSyncher( ztcSyncMgr    * aSyncMgr,
                                              stlErrorStack * aErrorStack )
{
    stlInt32           sState = 1;
    ztcSyncBufferInfo * sBufferInfo = NULL;
    ztcSyncBufferInfo * sNextBufferInfo = NULL;
    ztcSyncher        * sSyncher = NULL;
    ztcSyncher        * sNextSyncher = NULL;
    
    STL_RING_FOREACH_ENTRY_SAFE( &aSyncMgr->mBufferInfoList,
                                 sBufferInfo,
                                 sNextBufferInfo )
    {
        STL_RING_UNLINK( &aSyncMgr->mBufferInfoList, sBufferInfo );
        
        STL_TRY( stlDestroySemaphore( &sBufferInfo->mSyncSem,
                                      aErrorStack ) == STL_SUCCESS );

        STL_TRY( stlFreeDynamicMem( &aSyncMgr->mDynamicMem,
                                    (void*) sBufferInfo,
                                    aErrorStack ) == STL_SUCCESS );
    }
    sState = 0;
    
    STL_RING_FOREACH_ENTRY_SAFE( &aSyncMgr->mSyncherList,
                                 sSyncher,
                                 sNextSyncher )
    {
        STL_RING_UNLINK( &aSyncMgr->mSyncherList, sSyncher );

        STL_TRY( stlFreeDynamicMem( &aSyncMgr->mDynamicMem,
                                    (void*) sSyncher,
                                    aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            STL_RING_FOREACH_ENTRY_SAFE( &aSyncMgr->mBufferInfoList,
                                         sBufferInfo,
                                         sNextBufferInfo )
            {
                STL_RING_UNLINK( &aSyncMgr->mBufferInfoList, sBufferInfo );
                
                (void) stlDestroySemaphore( &sBufferInfo->mSyncSem,
                                            aErrorStack );
            }

        default:
            break;
    }
    
    return STL_FAILURE;
}

/**
 * @brief Sync Manager에서 Table Infomation을 할당한다.
 * @param[in,out] aSyncMgr    sync manager
 * @param[in]     aMasterMgr  master manager
 * @param[in,out] aErrorStack error stack 
 */ 
stlStatus ztcnInitializeSyncTableInfo( ztcSyncMgr    * aSyncMgr,
                                       ztcMasterMgr  * aMasterMgr,
                                       stlErrorStack * aErrorStack )
{
    ztcMasterTableMeta  * sTableMeta      = NULL;
    ztcSyncTableInfo    * sSyncTableInfo  = NULL;

    /**
     * @todo Slave Schema/Table Name 설정.
     */ 
    STL_RING_FOREACH_ENTRY( &aMasterMgr->mTableMetaList, sTableMeta )
    {
        STL_TRY( stlAllocDynamicMem( &aSyncMgr->mDynamicMem,
                                     STL_SIZEOF( ztcSyncTableInfo ),
                                     (void**)&sSyncTableInfo,
                                     aErrorStack ) == STL_SUCCESS );
        
        STL_RING_INIT_HEADLINK( &sSyncTableInfo->mColumnInfoList,
                                STL_OFFSETOF( ztcColumnInfo, mLink ) );
        
        STL_RING_INIT_DATALINK( sSyncTableInfo,
                                STL_OFFSETOF( ztcSyncTableInfo, mLink ) );

        stlStrncpy( sSyncTableInfo->mMasterSchemaName,
                    sTableMeta->mSchemaName,
                    stlStrlen( sTableMeta->mSchemaName ) + 1 );
        
        stlStrncpy( sSyncTableInfo->mMasterTableName,
                    sTableMeta->mTableName,
                    stlStrlen( sTableMeta->mTableName ) + 1 );

        stlStrncpy( sSyncTableInfo->mSlaveSchemaName,
                    sTableMeta->mSlaveSchema,
                    stlStrlen( sTableMeta->mSlaveSchema ) + 1 );

        stlStrncpy( sSyncTableInfo->mSlaveTableName,
                    sTableMeta->mSlaveTable,
                    stlStrlen( sTableMeta->mSlaveTable ) + 1 );

        sSyncTableInfo->mColumnCount = sTableMeta->mColumnCount;
        sSyncTableInfo->mPhysicalId  = sTableMeta->mPhysicalId;
                
        STL_TRY( ztcnInitializeSyncColumnInfo( aSyncMgr,
                                               sSyncTableInfo,
                                               sTableMeta,
                                               aErrorStack ) == STL_SUCCESS );
        
        STL_RING_ADD_LAST( &aSyncMgr->mTableList, sSyncTableInfo );
    }

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief Sync Manager에서 Sync Table Infomation을 할당한다.
 * @param[in,out] aSyncMgr    sync manager 
 * @param[in,out] aErrorStack error stack 
 */ 
stlStatus ztcnFinalizeSyncTableInfo( ztcSyncMgr    * aSyncMgr,
                                     stlErrorStack * aErrorStack )
{
    ztcSyncTableInfo * sSyncTableInfo     = NULL;
    ztcSyncTableInfo * sNextSyncTableInfo = NULL;

    STL_RING_FOREACH_ENTRY_SAFE( &aSyncMgr->mTableList,
                                 sSyncTableInfo,
                                 sNextSyncTableInfo )
    {
        STL_RING_UNLINK( &aSyncMgr->mTableList, sSyncTableInfo );
        
        STL_TRY( ztcnFinalizeSyncColumnInfo( &aSyncMgr->mDynamicMem,
                                             sSyncTableInfo,
                                             aErrorStack ) == STL_SUCCESS );

        STL_TRY( stlFreeDynamicMem( &aSyncMgr->mDynamicMem,
                                    sSyncTableInfo,
                                    aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief Sync Table Info에 Column Info를 할당하고 값을 설정한다.
 * @param[in]     aSyncMgr        sync manager
 * @param[in,out] aSyncTableInfo  Sync table info
 * @param[in]     aTableMeta      Master Table Meta
 * @param[in,out] aErrorStack     error stack 
 */ 
stlStatus ztcnInitializeSyncColumnInfo( ztcSyncMgr          * aSyncMgr,
                                        ztcSyncTableInfo    * aSyncTableInfo,
                                        ztcMasterTableMeta  * aTableMeta,
                                        stlErrorStack       * aErrorStack )
{
    ztcColumnInfo       * sColumnInfo;
    ztcMasterColumnMeta * sColumnMeta;

    STL_RING_FOREACH_ENTRY( &aTableMeta->mColumnMetaList, sColumnMeta )
    {
        STL_TRY( stlAllocDynamicMem( &aSyncMgr->mDynamicMem,
                                     STL_SIZEOF( ztcColumnInfo ),
                                     (void**)&sColumnInfo,
                                     aErrorStack ) == STL_SUCCESS );
        
        STL_RING_INIT_DATALINK( sColumnInfo,
                                STL_OFFSETOF( ztcColumnInfo, mLink ) );

        stlStrncpy( sColumnInfo->mColumnName,
                    sColumnMeta->mColumnName,
                    stlStrlen( sColumnMeta->mColumnName ) + 1 );

        STL_TRY( ztcdGetSyncColumnInfo( aSyncMgr->mDbcHandle,
                                        aSyncTableInfo->mMasterSchemaName,
                                        aSyncTableInfo->mMasterTableName,
                                        &aSyncMgr->mDTVector,
                                        sColumnInfo,
                                        aErrorStack ) == STL_SUCCESS );
        
        STL_RING_ADD_LAST( &aSyncTableInfo->mColumnInfoList, sColumnInfo );
    }

    /**
     * @remarks statement_view_scn 데이터 타입이 변경될 경우 수정이 필요.
     * ztcdGetColumnParamInfo를 참고하여 작성.
     * Syncher에서 SQLBindParameter를 할 때에 Statement View Scn을 제외하기 위하여
     * ztcSyncTableInfo에 ColumnCount를 증가하지 않는다.
     */
    STL_TRY( stlAllocDynamicMem( &aSyncMgr->mDynamicMem,
                                 STL_SIZEOF( ztcColumnInfo ),
                                 (void**)&sColumnInfo,
                                 aErrorStack ) == STL_SUCCESS );
        
    STL_RING_INIT_DATALINK( sColumnInfo,
                            STL_OFFSETOF( ztcColumnInfo, mLink ) );

    stlStrncpy( sColumnInfo->mColumnName,
                "statement_view_scn()",
                stlStrlen( "statement_view_scn()" ) + 1 );

    sColumnInfo->mDataTypeId       = DTL_TYPE_NUMBER;
    sColumnInfo->mValueType        = SQL_C_UBIGINT;//SQL_C_BINARY;
    sColumnInfo->mParameterType    = SQL_NUMERIC;

    sColumnInfo->mNumericPrecision = 0;
    sColumnInfo->mNumericScale     = 0;
    
    sColumnInfo->mColumnSize       = sColumnInfo->mNumericPrecision;
    sColumnInfo->mDecimalDigits    = sColumnInfo->mNumericScale;

    sColumnInfo->mBufferLength     = STL_SIZEOF( stlUInt64 );
    sColumnInfo->mValueMaxSize     = STL_SIZEOF( stlUInt64 );
    sColumnInfo->mStrValueMaxSize  = 0;
    sColumnInfo->mStringLengthUnit = SQL_CLU_NONE;
    
    STL_RING_ADD_LAST( &aSyncTableInfo->mColumnInfoList, sColumnInfo );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztcnFinalizeSyncColumnInfo( stlDynamicAllocator * aDynamicMem,
                                      ztcSyncTableInfo    * aSyncTableInfo,
                                      stlErrorStack       * aErrorStack )
{
    ztcColumnInfo * sColumnInfo     = NULL;
    ztcColumnInfo * sNextColumnInfo = NULL;

    STL_RING_FOREACH_ENTRY_SAFE( &aSyncTableInfo->mColumnInfoList,
                                 sColumnInfo,
                                 sNextColumnInfo )
    {
        STL_RING_UNLINK( &aSyncTableInfo->mColumnInfoList, sColumnInfo );
        
        STL_TRY( stlFreeDynamicMem( aDynamicMem,
                                    (void*)sColumnInfo,
                                    aErrorStack ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcnSetSyncStartPosition( stlErrorStack * aErrorStack )
{
    stlInt64 sBeginLsn = ZTC_INVALID_LSN;
    
    STL_TRY( ztcdGetMinBeginLsn( gMasterMgr->mDbcHandle,
                                 &sBeginLsn,
                                 aErrorStack ) == STL_SUCCESS );
    
    /**
     * 현재 진행중인 Transaction이 없을 경우 HandShake 과정에서 세팅한 LogFile 정보를 그냥 사용하면 된다.
     */
    if( sBeginLsn != ZTC_INVALID_LSN )
    {
        STL_TRY( ztcmSetLogFileInfoUsingLSN( sBeginLsn,
                                             aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} */
