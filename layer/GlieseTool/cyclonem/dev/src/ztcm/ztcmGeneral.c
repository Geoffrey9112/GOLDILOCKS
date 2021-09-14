/*******************************************************************************
 * ztcmGeneral.c
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
 * @file ztcmGeneral.c
 * @brief GlieseTool Cyclone General Routines
 */

#include <goldilocks.h>
#include <ztc.h>

ztcMasterMgr      * gMasterMgr = NULL;
ztcSlaveMgr       * gSlaveMgr  = NULL;
ztcSyncMgr        * gSyncMgr   = NULL;

ztcMonitorInfo      gMonitorInfo;
ztcConfigure        gConfigure;

stlUInt32 ztcmCaptureTableHashFunc( void      * aKey, 
                                    stlUInt32   aBucketCount )
{
    /**
     * @todo 더 좋은 방법으로 수정할 필요가 있음..
     */
    
    stlInt64 sValue = *(stlInt64*)(aKey);
    
    STL_DASSERT( sValue > 0 );
    
    return sValue % aBucketCount;
}


stlInt32  ztcmCaptureTableHashCompareFunc( void * aKeyA, 
                                           void * aKeyB )
{
    stlInt32 sRet = 0;
    
    if ( *(stlInt64*)aKeyA == *(stlInt64*)aKeyB )
    {
        sRet = 0;
    }
    else
    {
        sRet = -1;
    }

    return sRet;
}

stlUInt32 ztcmSlaveTableHashFunc( void      * aKey, 
                                  stlUInt32   aBucketCount )
{
    stlInt32 sValue = *(stlInt32*)(aKey);
    
    STL_DASSERT( sValue > 0 );
       
    return sValue % aBucketCount;
}

stlInt32  ztcmSlaveTableHashCompareFunc( void * aKeyA, 
                                         void * aKeyB )
{
    stlInt32 sRet = 0;
    
    if ( *(stlInt32*)aKeyA == *(stlInt32*)aKeyB )
    {
        sRet = 0;
    }
    else
    {
        sRet = -1;
    }

    return sRet;
}


stlStatus ztcmInitializeEnv4Master( stlErrorStack * aErrorStack )
{
    stlInt32       sState          = 0;
    stlInt32       sIdx            = 0;
    stlInt32       sChunkCount     = 0;
    ztcChunkItem * sChunkItem      = NULL;
    stlInt64       sGiveupInterval = 0;
    stlInt32       sPort           = 0;
    stlChar        sLibConfig[STL_MAX_FILE_PATH_LENGTH];

    STL_TRY( stlAllocHeap( (void**)&gMasterMgr,
                           STL_SIZEOF( ztcMasterMgr ),
                           aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    stlMemset( gMasterMgr, 0x00, STL_SIZEOF( ztcMasterMgr ) );
    stlMemset( &gMonitorInfo, 0x00, STL_SIZEOF( ztcMonitorInfo ) );
    
    STL_TRY( stlCreateRegionAllocator( &(gMasterMgr->mRegionMem),
                                       ZTC_REGION_INIT_SIZE,
                                       ZTC_REGION_NEXT_SIZE,
                                       aErrorStack ) == STL_SUCCESS );
    sState = 2;
    
    STL_TRY( ztcdAllocEnvHandle( &gMasterMgr->mEnvHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 3;
    
    if( ztcdCreateConnection( &gMasterMgr->mDbcHandle,
                              gMasterMgr->mEnvHandle,
                              STL_TRUE,
                              aErrorStack ) == STL_SUCCESS )
    {
        gMasterMgr->mDbConnected = STL_TRUE;
    }
    else
    {
        STL_TRY( stlGetLastErrorCode( aErrorStack ) != ZTC_ERRCODE_INVALID_CONN_INFO );
        gMasterMgr->mDbConnected = STL_FALSE;
    }
    sState = 4;
    
    /**
     * Connection(Attach) 이후에 SignalHandler를 등록해야 한다.
     */
    STL_TRY( stlSetSignalHandler( ZTC_SIGNAL_DEFINE,
                                  ztcmMasterSignalFunc,
                                  &gConfigure.mOldSignalFunc,
                                  aErrorStack ) == STL_SUCCESS );
    
    /**
     * 기타 Sginal을 무시한다.
     */
    STL_TRY( stlSetSignalHandler( STL_SIGNAL_USR1,
                                  STL_SIGNAL_IGNORE,
                                  NULL,
                                  aErrorStack ) == STL_SUCCESS );

    STL_TRY( stlSetSignalHandler( STL_SIGNAL_HUP,
                                  STL_SIGNAL_IGNORE,
                                  NULL,
                                  aErrorStack ) == STL_SUCCESS );

#ifndef STL_DEBUG
    STL_TRY( stlSetSignalHandler( STL_SIGNAL_TERM,
                                  STL_SIGNAL_IGNORE,
                                  NULL,
                                  aErrorStack ) == STL_SUCCESS );

    STL_TRY( stlSetSignalHandler( STL_SIGNAL_QUIT,
                                  STL_SIGNAL_IGNORE,
                                  NULL, 
                                  aErrorStack ) == STL_SUCCESS );
#endif

    STL_TRY( stlSetSignalHandler( STL_SIGNAL_INT,
                                  STL_SIGNAL_IGNORE,
                                  NULL,
                                  aErrorStack )== STL_SUCCESS );

    STL_TRY( stlSetSignalHandler( STL_SIGNAL_CLD,
                                  STL_SIGNAL_IGNORE,
                                  NULL,
                                  aErrorStack ) == STL_SUCCESS );
    
    sState = 5;
    
    STL_TRY( ztcmCreateStaticHash( &(gMasterMgr->mTableHash),
                                   ZTC_TABLE_HASH_BUCKET_SIZE,
                                   STL_OFFSETOF(ztcCaptureTableInfo, mLink),
                                   STL_OFFSETOF(ztcCaptureTableInfo, mPhysicalId),
                                   &(gMasterMgr->mRegionMem),
                                   aErrorStack ) == STL_SUCCESS );

    /**
     * Capture ChunkList 초기화
     */
    sState = 6;
    
    /**
     * Sender ChunkList 초기화
     */
    
    sState = 7;
    
    /**
     * Capture START 대기 Semaphore
     */
    STL_TRY( stlCreateSemaphore( &(gMasterMgr->mCaptureStartSem),
                                 "csSem",
                                 0,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 8;
    
    if( gConfigure.mDoReset == STL_TRUE )
    {
        /**
         * Meta File 삭제
         */
        STL_TRY( ztcmRemoveMasterMetaFile( aErrorStack ) == STL_SUCCESS );
    }

    /**
     * Table Meta List 초기화
     */
    STL_RING_INIT_HEADLINK( &gMasterMgr->mTableMetaList,
                            STL_OFFSETOF( ztcMasterTableMeta, mLink ) );
    
    /**
     * Meta 구성
     */
    STL_TRY( ztcmBuildMasterMeta( aErrorStack ) == STL_SUCCESS );

    /**
     * Configure File 읽기
     */
    STL_TRY( ztcfReadConfigure( aErrorStack ) == STL_SUCCESS );

    /**
     * initialize sender(publisher)
     */
    STL_TRY( ztcmGetLibConfig( sLibConfig,
                               STL_SIZEOF( sLibConfig ),
                               aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( gZtcLibFunc.mInitializeSender( sLibConfig,
                                                  gConfigure.mCurrentGroupPtr->mName,
                                                  gConfigure.mDoReset,
                                                  ztcmLogMsgCallback,
                                                  ztcrOnEventCallback,
                                                  NULL,
                                                  &(gMasterMgr->mCMMgr.mPubContext) )
                   == ZTCL_SUCCESS,
                   RAMP_ERR_MIDDLEWARE );

    /**
     * Trc Log에 기록 
     */
    STL_TRY( ztcmLogConfigureMsgMaster( aErrorStack ) == STL_SUCCESS );
             
    /**
     * Validate value
     */
    STL_TRY( ztcmGetPort( &sPort,
                          aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( stlStrlen( gConfigure.mCurrentGroupPtr->mName ) > 0, RAMP_ERR_INVALID_GROUP_NAME );
    
    if( gMasterMgr->mDbConnected == STL_FALSE )
    {
        sGiveupInterval = ztcmGetGiveupInterval();
        
        if( sGiveupInterval > 0 )
        {
            STL_TRY( ztcmLogMsg( aErrorStack,
                                 "[CAPTURE]GIVEUP_INTERVAL is disabled because GOLDILOCKS is not on-line.\n" ) == STL_SUCCESS );
            /**
             * DB가 연결되어있지 않으므로 Giveup Interval Check는 하지 않는다.
             */
            gConfigure.mGiveupInterval = 0;
        }
    }
    else
    {
        /**
         * Transaction Table Size를 얻어온다.
         */
       STL_TRY( ztcdSetTransTableSize( gMasterMgr->mDbcHandle,
                                       aErrorStack ) == STL_SUCCESS );
                                       
    }
    sChunkCount = ztcmGetCommChunkCount();
    
    for( sIdx = 0; sIdx < sChunkCount; sIdx++ )
    {
        STL_TRY( stlAllocRegionMem( &(gMasterMgr->mRegionMem),
                                    STL_SIZEOF( ztcChunkItem ),
                                    (void**)&sChunkItem,
                                    aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( stlAllocRegionMem( &(gMasterMgr->mRegionMem),
                                    ZTC_CHUNK_ITEM_SIZE,
                                    (void**)&sChunkItem->mBuffer,
                                    aErrorStack ) == STL_SUCCESS );
        
        sChunkItem->mWriteIdx = 0;
        sChunkItem->mReadIdx  = 0;
        
        STL_RING_INIT_DATALINK( sChunkItem,
                                STL_OFFSETOF( ztcChunkItem, mLink ) );
        
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_GROUP_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_GROUP_NAME,
                      NULL,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_MIDDLEWARE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_MIDDLEWARE_ERROR,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    switch( sState )
    {
        case 8:
            (void)stlDestroySemaphore( &(gMasterMgr->mCaptureStartSem),
                                       aErrorStack );
        case 7:
        case 6:
        case 5:
            (void) stlSetSignalHandler( ZTC_SIGNAL_DEFINE,
                                        gConfigure.mOldSignalFunc,
                                        NULL,
                                        aErrorStack );
        case 4:
            (void)ztcdDestroyConnection( gMasterMgr->mDbcHandle,
                                         aErrorStack );
        case 3:
            (void)ztcdFreeEnvHandle( gMasterMgr->mEnvHandle, 
                                     aErrorStack );
        case 2:
            (void)stlDestroyRegionAllocator( &(gMasterMgr->mRegionMem),
                                             aErrorStack );
        case 1:
            (void)stlFreeHeap( gMasterMgr );
            break;
        default:
            break;
    }

    return STL_FAILURE;   
}

stlStatus ztcmFinalizeEnv4Master( stlErrorStack * aErrorStack )
{
    STL_TRY( stlDestroySemaphore( &(gMasterMgr->mCaptureStartSem),
                                  aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlSetSignalHandler( ZTC_SIGNAL_DEFINE,
                                  gConfigure.mOldSignalFunc,
                                  NULL,
                                  aErrorStack ) == STL_SUCCESS );
    
    if( gMasterMgr->mDbcHandle != NULL )
    {
        STL_TRY( ztcdDestroyConnection( gMasterMgr->mDbcHandle,
                                        aErrorStack ) == STL_SUCCESS );
    }

    if( gMasterMgr->mEnvHandle != NULL )
    {
        STL_TRY( ztcdFreeEnvHandle( gMasterMgr->mEnvHandle,
                                    aErrorStack ) == STL_SUCCESS );
    }
    
    STL_TRY( stlDestroyRegionAllocator( &(gMasterMgr->mRegionMem),
                                        aErrorStack ) == STL_SUCCESS );
    
    stlFreeHeap( gMasterMgr );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;   

}

stlStatus ztcmInitializeEnv4Slave( stlErrorStack * aErrorStack )
{
    stlInt32       sState      = 0;
    stlInt32       sIdx        = 0;
    stlInt32       sChunkCount = 0;
    stlInt32       sPort       = 0;
    ztcChunkItem * sChunkItem  = NULL;
    stlChar        sLibConfig[STL_MAX_FILE_PATH_LENGTH];

    STL_TRY( stlAllocHeap( (void**)&gSlaveMgr,
                           STL_SIZEOF( ztcSlaveMgr ),
                           aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    stlMemset( gSlaveMgr, 0x00, STL_SIZEOF( ztcSlaveMgr ) );

    STL_TRY( stlCreateRegionAllocator( &(gSlaveMgr->mRegionMem),
                                       ZTC_REGION_INIT_SIZE,
                                       ZTC_REGION_NEXT_SIZE,
                                       aErrorStack ) == STL_SUCCESS );
    sState = 2;
    
    if( gConfigure.mApplyToFile == STL_FALSE )
    {
        STL_TRY( ztcdAllocEnvHandle( &gSlaveMgr->mEnvHandle,
                                     aErrorStack ) == STL_SUCCESS );
        sState = 3;

        STL_TRY( ztcdCreateConnection( &gSlaveMgr->mDbcHandle,
                                       gSlaveMgr->mEnvHandle,
                                       STL_FALSE,
                                       aErrorStack ) == STL_SUCCESS );
        sState = 4;
    }
    
    STL_TRY( stlSetSignalHandler( ZTC_SIGNAL_DEFINE,
                                  ztcmSlaveSignalFunc,
                                  &gConfigure.mOldSignalFunc,
                                  aErrorStack ) == STL_SUCCESS );
    
    /**
     * 기타 Sginal을 무시한다.
     */
    STL_TRY( stlSetSignalHandler( STL_SIGNAL_HUP,
                                  STL_SIGNAL_IGNORE,
                                  NULL,
                                  aErrorStack ) == STL_SUCCESS );

#ifndef STL_DEBUG
    STL_TRY( stlSetSignalHandler( STL_SIGNAL_TERM,
                                  STL_SIGNAL_IGNORE,
                                  NULL,
                                  aErrorStack ) == STL_SUCCESS );

    STL_TRY( stlSetSignalHandler( STL_SIGNAL_QUIT,
                                  STL_SIGNAL_IGNORE,
                                  NULL, 
                                  aErrorStack ) == STL_SUCCESS );
#endif
    STL_TRY( stlSetSignalHandler( STL_SIGNAL_INT,
                                  STL_SIGNAL_IGNORE,
                                  NULL,
                                  aErrorStack )== STL_SUCCESS );

    STL_TRY( stlSetSignalHandler( STL_SIGNAL_CLD,
                                  STL_SIGNAL_IGNORE,
                                  NULL,
                                  aErrorStack ) == STL_SUCCESS );
    sState = 5;
    
    if( gConfigure.mApplyToFile == STL_FALSE )
    {
        STL_TRY( ztcdSetDTFuncValue( gSlaveMgr->mDbcHandle,
                                     &gSlaveMgr->mCharacterSet,
                                     &gSlaveMgr->mTimezone,
                                     aErrorStack ) == STL_SUCCESS );
    }
    
    STL_TRY( ztcmSetDTFuncVector() == STL_SUCCESS );
    
    STL_RING_INIT_HEADLINK( &gSlaveMgr->mTableList,
                            STL_OFFSETOF( ztcSlaveTableInfo, mLink ) );
    
    STL_TRY( ztcmCreateStaticHash( &(gSlaveMgr->mTableHash),
                                   ZTC_TABLE_HASH_BUCKET_SIZE,
                                   STL_OFFSETOF(ztcSlaveTableInfo, mLink),
                                   STL_OFFSETOF(ztcSlaveTableInfo, mTableId),
                                   &(gSlaveMgr->mRegionMem),
                                   aErrorStack ) == STL_SUCCESS );
    
    gSlaveMgr->mTableCount = 0;
    gSlaveMgr->mRecvReady = 0;

    /**
     * Chunk Initialize
     */
    STL_RING_INIT_HEADLINK( &gSlaveMgr->mWaitWriteCkList,
                            STL_OFFSETOF( ztcChunkItem, mLink ) );
    
    STL_RING_INIT_HEADLINK( &gSlaveMgr->mWriteCkList,
                            STL_OFFSETOF( ztcChunkItem, mLink ) );
            
    gSlaveMgr->mWaitReadState = STL_FALSE;
    
    ztcmInitSpinLock( &(gSlaveMgr->mWriteSpinLock) );
    
    STL_TRY( stlCreateSemaphore( &(gSlaveMgr->mWaitWriteSem),
                                 "RcvWSem",
                                 0,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 6;
    
    STL_RING_INIT_HEADLINK( &gSlaveMgr->mWaitReadCkList,
                            STL_OFFSETOF( ztcChunkItem, mLink ) );
    
    STL_RING_INIT_HEADLINK( &gSlaveMgr->mReadCkList,
                            STL_OFFSETOF( ztcChunkItem, mLink ) );
    
    gSlaveMgr->mWaitWriteState = STL_FALSE;

    ztcmInitSpinLock( &(gSlaveMgr->mReadSpinLock) );
    
    STL_TRY( stlCreateSemaphore( &(gSlaveMgr->mWaitReadSem),
                                 "DstWSem",
                                 0,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 7;
    
    /**
     * DISTRIBUTOR/Appliers START 대기 Semaphore
     */
    STL_TRY( stlCreateSemaphore( &(gSlaveMgr->mDistbtStartSem),
                                 "DsSSem",
                                 0,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 8;
    
    STL_TRY( stlCreateSemaphore( &(gSlaveMgr->mApplierStartSem),
                                 "ApSSem",
                                 0,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 9;
    
    gSlaveMgr->mCurApplierIdx = 0;
    
    if( gConfigure.mDoReset == STL_TRUE )
    {
        if( gConfigure.mApplyToFile == STL_FALSE )
        {
            STL_TRY( ztcdResetSlaveMeta( gSlaveMgr->mDbcHandle,
                                         aErrorStack ) == STL_SUCCESS );
        }
    }

    STL_TRY( ztcfReadConfigure( aErrorStack ) == STL_SUCCESS );

    /**
     * initialize receiver(subscriber)
     */
    STL_TRY( ztcmGetLibConfig( sLibConfig,
                               STL_SIZEOF( sLibConfig ),
                               aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( gZtcLibFunc.mInitializeReceiver( sLibConfig,
                                                    gConfigure.mCurrentGroupPtr->mName,
                                                    gConfigure.mDoReset,
                                                    ztcmLogMsgCallback,
                                                    ztcrOnRecvCallback,
                                                    NULL,
                                                    ztcrOnEventCallback,
                                                    NULL,
                                                    0, /* start state : 0 이면 pause 이다 */
                                                    &gSlaveMgr->mCMMgr.mSubContext )
                   == ZTCL_SUCCESS,
                   RAMP_ERR_MIDDLEWARE );

    /**
     * Trc Log에 기록 
     */
    STL_TRY( ztcmLogConfigureMsgSlave( aErrorStack ) == STL_SUCCESS );
    
    /**
     * Validate value
     */
    STL_TRY_THROW( stlStrlen( gConfigure.mMasterIp ) > 0, RAMP_ERR_INVALID_MASTER_IP );
    
    STL_TRY( ztcmGetPort( &sPort,
                          aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( stlStrlen( gConfigure.mCurrentGroupPtr->mName ) > 0, RAMP_ERR_INVALID_GROUP_NAME );
    
    /**
     * SYNC Mode로 동작할 경우에는 반드시 ListenPort에 대한 정보를 갖어야 한다.
     */ 
    if( ztcmGetSyncMode() == STL_TRUE )
    {
        STL_TRY( ztcmGetGoldilocksListenPort( &gSlaveMgr->mGoldilocksListenPort,
                                         aErrorStack ) == STL_SUCCESS );

        STL_TRY( ztcmLogMsg( aErrorStack,
                             "GOLDILOCKS Listen Port %d\n",
                             gSlaveMgr->mGoldilocksListenPort )
                 == STL_SUCCESS );
    }
    
    gSlaveMgr->mApplierCount = ztcmGetApplierCount( aErrorStack );
    sChunkCount              = ztcmGetCommChunkCount();
    
    /**
     * Applier Manager를 할당한다.
     */
    STL_TRY( stlAllocRegionMem( &gSlaveMgr->mRegionMem,
                                STL_SIZEOF( ztcApplierMgr ) * gSlaveMgr->mApplierCount,
                                (void**)&gSlaveMgr->mApplierMgr,
                                aErrorStack ) == STL_SUCCESS );

    
    for( sIdx = 0; sIdx < sChunkCount; sIdx++ )
    {
        STL_TRY( stlAllocRegionMem( &gSlaveMgr->mRegionMem,
                                    STL_SIZEOF( ztcChunkItem ),
                                    (void**)&sChunkItem,
                                    aErrorStack ) == STL_SUCCESS );    
        
        STL_TRY( stlAllocRegionMem( &gSlaveMgr->mRegionMem,
                                    ZTC_CHUNK_ITEM_SIZE,
                                    (void**)&(sChunkItem->mBuffer),
                                    aErrorStack ) == STL_SUCCESS );    
        
        sChunkItem->mWriteIdx = 0;
        sChunkItem->mReadIdx  = 0;
        
        STL_RING_INIT_DATALINK( sChunkItem,
                                STL_OFFSETOF( ztcChunkItem, mLink ) );
        
        /**
         * 처음에는 Rcv ChunkList에 넣는다.
         */
        STL_RING_ADD_LAST( &gSlaveMgr->mWriteCkList, 
                           sChunkItem );
    }

    /**
     * Active Rid Slot Manager를 초기화 한다.
     * 종료시에 Distributor / Applier(s) 어느 것이 먼저 종료될지 모르기 때문에 
     * Distributor Thread를 생성시에 초기화하는 것이 아니라 여기서 초기화한다.
     */
    STL_TRY( ztcbInitializeSlotMgr( aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_MASTER_IP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_MASTER_IP,
                      NULL,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_INVALID_GROUP_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_GROUP_NAME,
                      NULL,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_MIDDLEWARE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_MIDDLEWARE_ERROR,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    switch( sState )
    {
        case 9:
            (void)stlDestroySemaphore( &(gSlaveMgr->mApplierStartSem),
                                       aErrorStack );
        case 8:
            (void)stlDestroySemaphore( &(gSlaveMgr->mDistbtStartSem),
                                       aErrorStack );
        case 7:
            (void)stlDestroySemaphore( &(gSlaveMgr->mWaitReadSem),
                                       aErrorStack );
            
            ztcmFinSpinLock( &(gSlaveMgr->mReadSpinLock) );
        case 6:
            (void)stlDestroySemaphore( &(gSlaveMgr->mWaitWriteSem),
                                       aErrorStack );
            
            ztcmFinSpinLock( &(gSlaveMgr->mWriteSpinLock) );
        case 5:
            (void) stlSetSignalHandler( ZTC_SIGNAL_DEFINE,
                                        gConfigure.mOldSignalFunc,
                                        NULL,
                                        aErrorStack );
        case 4:
            if( gConfigure.mApplyToFile == STL_FALSE )
            {
                (void)ztcdDestroyConnection( gSlaveMgr->mDbcHandle,
                                             aErrorStack );
            }
        case 3:
            if( gConfigure.mApplyToFile == STL_FALSE )
            {
                (void)ztcdFreeEnvHandle( gSlaveMgr->mEnvHandle,
                                         aErrorStack );
            }
        case 2:
            (void)stlDestroyRegionAllocator( &(gSlaveMgr->mRegionMem),
                                             aErrorStack );
        case 1:
            (void)stlFreeHeap( gSlaveMgr );
            break;
        default:
            break;
    }
    
    return STL_FAILURE;  
}


stlStatus ztcmFinalizeEnv4Slave( stlErrorStack * aErrorStack )
{
    STL_TRY( gZtcLibFunc.mFinalizeReceiver( gSlaveMgr->mCMMgr.mSubContext ) == ZTCL_SUCCESS );

    STL_TRY( ztcbFinalizeSlotMgr( aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlDestroySemaphore( &(gSlaveMgr->mApplierStartSem),
                                  aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlDestroySemaphore( &(gSlaveMgr->mDistbtStartSem),
                                  aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlDestroySemaphore( &(gSlaveMgr->mWaitReadSem),
                                  aErrorStack ) == STL_SUCCESS );
    
    ztcmFinSpinLock( &(gSlaveMgr->mReadSpinLock) );
    
    STL_TRY( stlDestroySemaphore( &(gSlaveMgr->mWaitWriteSem),
                                  aErrorStack ) == STL_SUCCESS );
    
    ztcmFinSpinLock( &(gSlaveMgr->mWriteSpinLock) );
    
    STL_TRY( stlSetSignalHandler( ZTC_SIGNAL_DEFINE,
                                  gConfigure.mOldSignalFunc,
                                  NULL,
                                  aErrorStack ) == STL_SUCCESS );
    if( gConfigure.mApplyToFile == STL_FALSE )
    {
        STL_TRY( ztcdDestroyConnection( gSlaveMgr->mDbcHandle,
                                        aErrorStack ) == STL_SUCCESS );

        STL_TRY( ztcdFreeEnvHandle( gSlaveMgr->mEnvHandle,
                                    aErrorStack ) == STL_SUCCESS );
    }
    STL_TRY( stlDestroyRegionAllocator( &(gSlaveMgr->mRegionMem),
                                        aErrorStack ) == STL_SUCCESS );
    
    (void)stlFreeHeap( gSlaveMgr );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcmFindCaptureTableInfo( ztcCaptureTableInfo  ** aTableInfo,
                                    stlInt64                aId,
                                    stlErrorStack         * aErrorStack )
{
    ztcCaptureTableInfo  * sTableInfo = NULL;
    
    STL_TRY( ztcmFindStaticHash( gMasterMgr->mTableHash,
                                 ztcmCaptureTableHashFunc,
                                 ztcmCaptureTableHashCompareFunc,
                                 (void*)&aId,
                                 (void**)&sTableInfo,
                                 aErrorStack ) == STL_SUCCESS );
    
    *aTableInfo = sTableInfo;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztcmFindSlaveTableInfo( ztcSlaveTableInfo  ** aTableInfo,
                                  stlInt32              aTableId,
                                  void                * aErrorStack )
{
    ztcSlaveTableInfo  * sTableInfo = NULL;
    
    STL_TRY( ztcmFindStaticHash( gSlaveMgr->mTableHash,
                                 ztcmSlaveTableHashFunc,
                                 ztcmSlaveTableHashCompareFunc,
                                 (void*)&aTableId,
                                 (void**)&sTableInfo,
                                 aErrorStack ) == STL_SUCCESS );
    
    *aTableInfo = sTableInfo;   
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;    
}


stlStatus ztcmAddTableInfo( stlChar       * aSchemaName,
                            stlChar       * aTableName,
                            stlErrorStack * aErrorStack )
{
    stlChar      sSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar      sTableName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlInt64     sPhysicalId;
    stlBool      sIsSupp  = STL_FALSE;
    
    ztcCaptureTableInfo  * sTableInfo   = NULL;
    ztcMasterTableMeta   * sTableMeta   = NULL;
    
    STL_TRY_THROW( gConfigure.mIsMaster == STL_TRUE, RAMP_SKIP );
    
    if ( aSchemaName != NULL )
    {
        if( *aSchemaName == '"' )
        {
            stlStrncpy( sSchemaName, aSchemaName + 1, stlStrlen( aSchemaName ) - 1 );
        }
        else
        {
            stlToupperString( aSchemaName, sSchemaName );
        }
    }
    else
    {
        /**
         * Schema Name이 없을 경우, PUBLIC으로 처리함
         */ 
        stlStrncpy( sSchemaName, ZTC_DEFAULT_SCHEMANAME, stlStrlen( ZTC_DEFAULT_SCHEMANAME ) + 1 );
    }
    
    if( *aTableName == '"' )
    {
        stlStrncpy( sTableName, aTableName + 1, stlStrlen( aTableName ) - 1 );
    }
    else
    {
        stlToupperString( aTableName, sTableName );
    }
    
    STL_TRY( ztcmFindTableMeta( &sTableMeta,
                                sSchemaName,
                                sTableName,
                                aErrorStack ) == STL_SUCCESS );
    
    /**
     * Master_META에 해당 Table에 없을 경우 FileWrite를 수행한다.
     */
    if( sTableMeta == NULL )
    {
        STL_TRY_THROW( ztcmGetDbConnected() == STL_TRUE, RAMP_ERR_INVALID_ACCESS );
        
        STL_TRY( ztcdGetTablePhysicalIDFromDB( gMasterMgr->mDbcHandle, 
                                               sSchemaName,
                                               sTableName,
                                               &sPhysicalId,
                                               aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztcdGetSupplLogTableValue( gMasterMgr->mDbcHandle, 
                                            sSchemaName,
                                            sTableName,
                                            &sIsSupp,
                                            aErrorStack ) == STL_SUCCESS );
            
        STL_TRY_THROW( sIsSupp == STL_TRUE, RAMP_ERR_TABLE_SUPPLEMENTAL );

        STL_TRY( stlAllocRegionMem( &(gMasterMgr->mRegionMem),
                                    STL_SIZEOF( ztcCaptureTableInfo ),
                                    (void**)&sTableInfo,
                                    aErrorStack ) == STL_SUCCESS );

        stlMemset( sTableInfo, 0x00, STL_SIZEOF( ztcCaptureTableInfo ) );
        
        stlStrncpy( sTableInfo->mSchemaName, sSchemaName, stlStrlen( sSchemaName ) + 1 );
        stlStrncpy( sTableInfo->mTableName,  sTableName,  stlStrlen( sTableName ) + 1 );
        sTableInfo->mPhysicalId    = sPhysicalId;
        sTableInfo->mTableId       = ++gMasterMgr->mMaxTableId;
        sTableInfo->mDdlLSN        = ZTC_INVALID_LSN;
        sTableInfo->mGiveUpTransId = ZTC_INVALID_TRANSID;
        sTableInfo->mCommitSCN     = ZTC_INVALID_SCN;
          
        STL_TRY( ztcmInsertStaticHash( gMasterMgr->mTableHash,
                                       ztcmCaptureTableHashFunc,
                                       sTableInfo,
                                       aErrorStack ) == STL_SUCCESS );
        
        gMasterMgr->mTableCount++;
        
        /**
         * Table Meta구성
         */
        STL_TRY( stlAllocRegionMem( &(gMasterMgr->mRegionMem),
                                    STL_SIZEOF( ztcMasterTableMeta ),
                                    (void**)&sTableMeta,
                                    aErrorStack ) == STL_SUCCESS );
        
        stlMemset( sTableMeta, 0x00, STL_SIZEOF( ztcMasterTableMeta ) );
        
        STL_RING_INIT_HEADLINK( &(sTableMeta->mColumnMetaList),
                                STL_OFFSETOF( ztcColumnInfo, mLink ) );
        
        stlStrncpy( sTableMeta->mSchemaName, sSchemaName, stlStrlen( sSchemaName ) + 1 );
        stlStrncpy( sTableMeta->mTableName,  sTableName,  stlStrlen( sTableName ) + 1 );
        sTableMeta->mPhysicalId  = sTableInfo->mPhysicalId;
        sTableMeta->mTableId     = sTableInfo->mTableId;
        sTableMeta->mDdlLSN      = sTableInfo->mDdlLSN;
        sTableMeta->mCommitSCN   = sTableInfo->mCommitSCN;
        sTableMeta->mColumnCount = 0;
        sTableMeta->mFileOffset  = 0;  //Write 시에 세팅해야 함!!
        
        STL_TRY( ztcdBuildTableInfoMeta( gMasterMgr->mDbcHandle, 
                                         &gMasterMgr->mRegionMem,
                                         sTableMeta,
                                         aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztcmWriteMasterTableMeta( sTableMeta,
                                           aErrorStack ) == STL_SUCCESS );
        
        STL_RING_ADD_LAST( &(gMasterMgr->mTableMetaList),
                           sTableMeta );

    }
    else
    {
        /**
         * 이미 Hash에 추가되었으면 처리하지 않는다.
         */
        STL_TRY( ztcmFindCaptureTableInfo( &sTableInfo,
                                           sTableMeta->mPhysicalId,
                                           aErrorStack ) == STL_SUCCESS );
        
        STL_TRY_THROW( sTableInfo == NULL, RAMP_ERR_DUPLICATE_CAPTURE_TABLE );
        
        /**
         * Capture에서 사용할 Hash Insert..
         */
        STL_TRY( stlAllocRegionMem( &(gMasterMgr->mRegionMem),
                                    STL_SIZEOF( ztcCaptureTableInfo ),
                                    (void**)&sTableInfo,
                                    aErrorStack ) == STL_SUCCESS );

        stlMemset( sTableInfo, 0x00, STL_SIZEOF( ztcCaptureTableInfo ) );
    
        stlStrncpy( sTableInfo->mSchemaName, sTableMeta->mSchemaName, stlStrlen( sTableMeta->mSchemaName ) + 1 );
        stlStrncpy( sTableInfo->mTableName,  sTableMeta->mTableName,  stlStrlen( sTableMeta->mTableName ) + 1 );
        sTableInfo->mPhysicalId    = sTableMeta->mPhysicalId;
        sTableInfo->mTableId       = sTableMeta->mTableId;
        sTableInfo->mDdlLSN        = sTableMeta->mDdlLSN;
        sTableInfo->mGiveUpTransId = ZTC_INVALID_TRANSID;
        sTableInfo->mCommitSCN     = sTableMeta->mCommitSCN;
      
        STL_TRY( ztcmInsertStaticHash( gMasterMgr->mTableHash,
                                       ztcmCaptureTableHashFunc,
                                       sTableInfo,
                                       aErrorStack ) == STL_SUCCESS );
    
        gMasterMgr->mTableCount++;
    }
    
    STL_RAMP( RAMP_SKIP );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_TABLE_SUPPLEMENTAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_SUPPLEMENTAL_LOG,
                      NULL,
                      aErrorStack,
                      sSchemaName, 
                      sTableName );
    }
    STL_CATCH( RAMP_ERR_INVALID_ACCESS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_FAIL_ADD_TABLE,
                      NULL,
                      aErrorStack,
                      sSchemaName, 
                      sTableName );
    }
    STL_CATCH( RAMP_ERR_DUPLICATE_CAPTURE_TABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_DUPLICATE_OBJECT,
                      NULL,
                      aErrorStack,
                      sSchemaName, 
                      sTableName );
    }
    STL_FINISH;
    
    return STL_FAILURE;       
}


stlStatus ztcmAddApplyInfo( stlChar       * aMasterSchema,
                            stlChar       * aMasterTable,
                            stlChar       * aSlaveSchema,
                            stlChar       * aSlaveTable,
                            stlErrorStack * aErrorStack )
{
    stlChar      sMasterSchema[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar      sMasterTable[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar      sSlaveSchema[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar      sSlaveTable[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlBool      sFound   = STL_FALSE;
    stlInt32     sTableId;

    ztcColumnInfo     * sColumnInfo = NULL;
    ztcSlaveTableInfo * sTableInfo  = NULL;
    
    STL_TRY_THROW( gConfigure.mIsMaster == STL_FALSE, RAMP_SUCCESS );
    
    if ( aMasterSchema != NULL )
    {
        if( *aMasterSchema == '"' )
        {
            stlStrncpy( sMasterSchema, aMasterSchema + 1, stlStrlen( aMasterSchema ) - 1 );
        }
        else
        {
            stlToupperString( aMasterSchema, sMasterSchema );
        }
    }
    else
    {
        /**
         * Schema Name이 없을 경우, PUBLIC으로 처리함
         */ 
        stlStrncpy( sMasterSchema, ZTC_DEFAULT_SCHEMANAME, stlStrlen( ZTC_DEFAULT_SCHEMANAME ) + 1 );
    }
    
    if( *aMasterTable == '"' )
    {
        stlStrncpy( sMasterTable, aMasterTable + 1, stlStrlen( aMasterTable ) - 1 );
    }
    else
    {
        stlToupperString( aMasterTable, sMasterTable );
    }
    
    if ( aSlaveSchema != NULL )
    {
        if( *aSlaveSchema == '"' )
        {
            stlStrncpy( sSlaveSchema, aSlaveSchema + 1, stlStrlen( aSlaveSchema ) - 1 );
        }
        else
        {
            stlToupperString( aSlaveSchema, sSlaveSchema );
        }
    }
    else
    {
        /**
         * Schema Name이 없을 경우, PUBLIC으로 처리함
         */ 
        stlStrncpy( sSlaveSchema, ZTC_DEFAULT_SCHEMANAME, stlStrlen( ZTC_DEFAULT_SCHEMANAME ) + 1 );
    }
    
    if( *aSlaveTable == '"' )
    {
        stlStrncpy( sSlaveTable, aSlaveTable + 1, stlStrlen( aSlaveTable ) - 1 );
    }
    else
    {
        stlToupperString( aSlaveTable, sSlaveTable );
    }
    
    if( gConfigure.mApplyToFile == STL_FALSE )
    {
        /**
         * Flow에는 Table을 구분하기 위해서 TableId(stlInt32)를 사용한다.
         * Master의 Table이 Drop & Create 되더라도 같은 SchemaName, TableName을 갖는다면 동일한 TableId를 갖게된다.
         * Slave는 Master와의 최초 접속시 TableId를 받고 이를 Slave의 Meta Table에 저장한다.
         * 만약 새로운 Table이 추가되었을 경우에, TableId는 Slave에서 받은적이 없으므로,
         * INVALID로 처리되고 Protocol 과정에서 Master에게 해당 TableId를 받는다.
         * 즉, Hash에서는 Flow에서 읽은 TableId를 사용해서 Slave의 SchemaName, TableName을 얻을 수 있어야 한다. !!!!
         */
        STL_TRY( ztcdGetTableIDFromSlaveMeta( gSlaveMgr->mDbcHandle,
                                              sSlaveSchema,
                                              sSlaveTable,
                                              &sTableId,
                                              &sFound,
                                              aErrorStack ) == STL_SUCCESS );

        if( sFound == STL_TRUE )
        {
            /**
             * Duplicate 되었을 경우 Fail로 처리하지 않는다.
             * Master의 두개의 Table이 Slave의 하나의 Table로 데이터를 입력할 수 있으므로...
             */
            STL_TRY( ztcmFindSlaveTableInfo( &sTableInfo,
                                             sTableId,
                                             aErrorStack ) == STL_SUCCESS );

            STL_TRY_THROW( sTableInfo == NULL, RAMP_SUCCESS );
        }
    }
    
    STL_TRY( stlAllocRegionMem( &gSlaveMgr->mRegionMem,
                                STL_SIZEOF( ztcSlaveTableInfo ),
                                (void**)&sTableInfo,
                                aErrorStack ) == STL_SUCCESS );    
    
    STL_RING_INIT_DATALINK( sTableInfo,
                            STL_OFFSETOF( ztcSlaveTableInfo, mLink ) );
    
    stlStrncpy( sTableInfo->mMasterSchema, sMasterSchema, stlStrlen( sMasterSchema ) + 1 );
    stlStrncpy( sTableInfo->mMasterTable,  sMasterTable,  stlStrlen( sMasterTable ) + 1 );
    stlStrncpy( sTableInfo->mSlaveSchema,  sSlaveSchema,  stlStrlen( sSlaveSchema ) + 1 );
    stlStrncpy( sTableInfo->mSlaveTable,   sSlaveTable,   stlStrlen( sSlaveTable ) + 1 );
    
    STL_RING_INIT_HEADLINK( &sTableInfo->mColumnList,
                            STL_OFFSETOF( ztcColumnInfo, mLink ) );
    
    sTableInfo->mColumnCount = 0;
    sTableInfo->mTableId     = sTableId;
    
    if( gConfigure.mApplyToFile == STL_FALSE )
    {
        STL_TRY( ztcdGetSlaveColumnInfo( gSlaveMgr->mDbcHandle,
                                         sTableInfo,
                                         aErrorStack ) == STL_SUCCESS );


        STL_RING_FOREACH_ENTRY( &sTableInfo->mColumnList, sColumnInfo )
        {
            STL_TRY( ztcdGetColumnParamInfo( gSlaveMgr->mDbcHandle,
                                             sTableInfo->mTableIdAtDB,
                                             &gSlaveMgr->mDTVector,
                                             sColumnInfo,
                                             aErrorStack ) == STL_SUCCESS);
        }
    }

    
    if( sFound == STL_TRUE )
    {
        /**
         * Meta에서 찾았으면.. Hash Insert
         */
        STL_TRY( ztcmInsertStaticHash( gSlaveMgr->mTableHash,
                                       ztcmSlaveTableHashFunc,
                                       sTableInfo,
                                       aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        sTableInfo->mTableId = ZTC_INVALID_TABLE_LOGICAL_ID;
        STL_RING_ADD_LAST( &gSlaveMgr->mTableList, 
                           sTableInfo );
    }
    
    gSlaveMgr->mTableCount++;
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcmSetLogFileInfo( ztcCaptureInfo * aCaptureInfo,
                              stlErrorStack  * aErrorStack )
{
    ztcLogFileInfo * sLogFileInfo = NULL;
    stlBool          sFound       = STL_FALSE;
    
    STL_RING_FOREACH_ENTRY( &gMasterMgr->mLogFileList, sLogFileInfo )
    {
        if( sLogFileInfo->mCaptureInfo.mRedoLogGroupId == aCaptureInfo->mRedoLogGroupId )
        {
            sFound                = STL_TRUE;
            sLogFileInfo->mActive = STL_TRUE;

            sLogFileInfo->mCaptureInfo.mFileSeqNo     = aCaptureInfo->mFileSeqNo;
            sLogFileInfo->mCaptureInfo.mBlockSeq      = aCaptureInfo->mBlockSeq;
            sLogFileInfo->mCaptureInfo.mReadLogNo     = aCaptureInfo->mReadLogNo;
            sLogFileInfo->mCaptureInfo.mWrapNo        = aCaptureInfo->mWrapNo;
            sLogFileInfo->mCaptureInfo.mRestartLSN    = aCaptureInfo->mRestartLSN;
            sLogFileInfo->mCaptureInfo.mLastCommitLSN = aCaptureInfo->mLastCommitLSN;
        }
        else
        {
            sLogFileInfo->mActive = STL_FALSE;

            sLogFileInfo->mCaptureInfo.mFileSeqNo     = ZTC_INVALID_FILE_SEQ_NO;
            sLogFileInfo->mCaptureInfo.mBlockSeq      = 0;
            sLogFileInfo->mCaptureInfo.mReadLogNo     = 1;
            sLogFileInfo->mCaptureInfo.mWrapNo        = ZTC_INVALID_WRAP_NO;
            sLogFileInfo->mCaptureInfo.mRestartLSN    = ZTC_INVALID_LSN;
            sLogFileInfo->mCaptureInfo.mLastCommitLSN = ZTC_INVALID_LSN;
        }
    }

    STL_TRY_THROW( sFound == STL_TRUE, RAMP_ERR_INVALID_CAPTURE_INFO );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_CAPTURE_INFO )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_CAPTURE_INFO,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcmAddLogFileInfo( stlChar       * aLogFileName,
                              stlInt16        aGroupId,
                              stlInt64        aFileSize,
                              stlErrorStack * aErrorStack )
{
    stlFileInfo       sFileInfo;
    ztcLogFileInfo  * sLogFileInfo = NULL;
    
    STL_TRY( stlAllocRegionMem( &(gMasterMgr->mRegionMem),
                                STL_SIZEOF( ztcLogFileInfo ),
                                (void**)&sLogFileInfo,
                                aErrorStack ) == STL_SUCCESS );
    
    stlMemset( sLogFileInfo, 0x00, STL_SIZEOF( ztcLogFileInfo ) );
    
    stlStrncpy( sLogFileInfo->mName, aLogFileName, stlStrlen( aLogFileName ) + 1 );
    
    STL_TRY( stlGetFileStatByName( &sFileInfo,
                                   sLogFileInfo->mName,
                                   STL_FINFO_MIN,
                                   aErrorStack ) == STL_SUCCESS );
    
    sLogFileInfo->mLastModifiedTime = sFileInfo.mLastModifiedTime;
    sLogFileInfo->mActive           = STL_FALSE;
    sLogFileInfo->mFileSize         = aFileSize;
    sLogFileInfo->mGroupId          = aGroupId;
    
    if( aFileSize != 0 )
    {
        sLogFileInfo->mBlockCount = aFileSize / ZTC_DEFAULT_BLOCK_SIZE;   
    }
    else
    {
        sLogFileInfo->mBlockCount = 0;
    }
    
    sLogFileInfo->mCaptureInfo.mRedoLogGroupId = aGroupId;
    sLogFileInfo->mCaptureInfo.mBlockSeq       = 0;
    sLogFileInfo->mCaptureInfo.mReadLogNo      = 1;
    sLogFileInfo->mCaptureInfo.mFileSeqNo      = ZTC_INVALID_FILE_SEQ_NO;
    sLogFileInfo->mCaptureInfo.mRestartLSN     = ZTC_INVALID_LSN;
    sLogFileInfo->mCaptureInfo.mLastCommitLSN  = ZTC_INVALID_LSN;
    sLogFileInfo->mCaptureInfo.mWrapNo         = ZTC_INVALID_WRAP_NO;
    
    STL_RING_INIT_DATALINK( sLogFileInfo,
                            STL_OFFSETOF( ztcLogFileInfo, mLink ) );
    
    STL_RING_ADD_LAST( &gMasterMgr->mLogFileList, 
                       sLogFileInfo );
    
    gMasterMgr->mLogFileCount++;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;       
}


stlStatus ztcmCreateThreads( stlErrorStack  * aErrorStack )
{
    stlInt32 sState          = 0;
    stlInt64 sGiveupInterval = 0;
    
    if( gConfigure.mIsMaster == STL_TRUE )
    {
        STL_TRY( ztcmInitializeEnv4Master( aErrorStack ) == STL_SUCCESS );
        sState = 1;
        
        /**
         * Capture Thread 생성
         */
        STL_TRY( ztctCreateCaptureThread( aErrorStack ) == STL_SUCCESS );
        
        /**
         * Master 관련 Communication Thread 생성(Listener & Sender)
         */
        STL_TRY( ztctCreateThreadCM4Master( aErrorStack ) == STL_SUCCESS );

        /**
         * GIVEUP_INTERVAL이 설정되어있을 경우
         */
        sGiveupInterval= ztcmGetGiveupInterval();
        
        if( sGiveupInterval > 0 )
        {
            STL_TRY( ztctCreateStatusThread( aErrorStack ) == STL_SUCCESS );
        }
    }
    else
    {
        STL_TRY( ztcmInitializeEnv4Slave( aErrorStack ) == STL_SUCCESS );
        sState = 1;

        /**
         * Sub Distributor Thread 생성
         */
        STL_TRY( ztctCreateSubDistbtThread( aErrorStack ) == STL_SUCCESS );
        
        while( (gRunState == STL_TRUE) &&
               (gSlaveMgr->mStartState == 0) )
        {
            stlSleep( STL_SET_MSEC_TIME( 100 ) );
        }

        /**
         * Receiver Thread 생성
         */
        STL_TRY( ztctCreateThreadCM4Slave( aErrorStack ) == STL_SUCCESS );

        /**
         * Distributor Thread 생성
         */
        STL_TRY( ztctCreateDistbtThread( aErrorStack ) == STL_SUCCESS );
        
        
        /**
         * Applier Thread 생성
         */
        STL_TRY( ztctCreateApplierThread( aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    if( sState == 1 )
    {
        if( gConfigure.mIsMaster == STL_TRUE )
        {
            (void) ztcmFinalizeEnv4Master( aErrorStack );
        }
        else
        {
            (void) ztcmFinalizeEnv4Slave( aErrorStack );
        }
    }
    
    return STL_FAILURE;
}


stlStatus ztcmJoinThreads( stlBool          aIsMaster,
                           stlErrorStack  * aErrorStack )
{
    if( aIsMaster == STL_TRUE )
    {
        STL_TRY( ztctJoinCaptureThread( aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztctJoinThreadCM4Master( aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztcmFinalizeEnv4Master( aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( ztctJoinThreadCM4Slave( aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztctJoinDistbtThread( aErrorStack ) == STL_SUCCESS );

        STL_TRY( ztctJoinSubDistbtThread( aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztctJoinApplierThread( aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztcmFinalizeEnv4Slave( aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;   
}

stlStatus ztcmStart( stlBool         aIsMaster,
                     stlBool         aDoReset,
                     stlErrorStack * aErrorStack )
{
    stlProc   sProc;
    stlBool   sIsChild = STL_FALSE;
    
    stlFile   sFile;
    stlChar   sFileName[STL_MAX_FILE_PATH_LENGTH];
    stlChar   sWriteBuff[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlSize   sWriteSize = 0;
    
    stlBool   sExist = STL_FALSE;
    stlInt32  sState = 0;
    
    ztcGroupItem * sGroup = NULL;
    
    gConfigure.mIsMaster      = aIsMaster;
    gConfigure.mDoReset       = aDoReset;
    gConfigure.mPort          = 0;
    gConfigure.mPropagateMode = STL_FALSE;      //Propagate 하지 않는것이 Default 값임

    STL_TRY( stlCreateRegionAllocator( &gConfigure.mAllocator,
                                       ZTC_REGION_INIT_SIZE,
                                       ZTC_REGION_NEXT_SIZE,
                                       aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_RING_INIT_HEADLINK( &gConfigure.mGroupList,
                            STL_OFFSETOF( ztcGroupItem, mLink ) );
    
    /**
     * Group만 Parsing한다.
     */
    STL_TRY( ztcfIdentifyGroup( aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( STL_RING_IS_EMPTY( &gConfigure.mGroupList ) != STL_TRUE, RAMP_ERR_INVALID_GROUP_COUNT );
    
    STL_RING_FOREACH_ENTRY( &gConfigure.mGroupList, sGroup )
    {
        /**
         * Child Process에 수행해야할 Group을 설정한다.
         */
        gConfigure.mCurrentGroupPtr = sGroup;
        
        if( aIsMaster == STL_TRUE )
        {
            /**
             * Nothing To Do.
             */
        }
        else
        {
            if( gConfigure.mApplyToFile == STL_FALSE )
            {
                //Slave Meta Table 생성
                STL_TRY( ztcdCreateMetaTable4Slave( aErrorStack ) == STL_SUCCESS );
            }
        }

        /**
         * Group별로 daemon을 생성한다.
         */

        STL_TRY( stlForkProc( &sProc,
                              &sIsChild,
                              aErrorStack ) == STL_SUCCESS );
            
        if( sIsChild == STL_TRUE )
        {
            break;
        }
    }

    
    if( sIsChild == STL_FALSE )
    {
        /**
         * PId 관련 처리해야 함
         */
    }
    else
    {
        STL_TRY( stlGetCurrentProc( &sProc, aErrorStack ) == STL_SUCCESS );

        STL_TRY( ztcmGetPidFileName( sFileName, 
                                     aIsMaster,
                                     aErrorStack ) == STL_SUCCESS );
    
        STL_TRY( stlExistFile( sFileName,
                               &sExist,
                               aErrorStack ) == STL_SUCCESS );

        if ( sExist == STL_TRUE )
        {
            STL_TRY( ztcmProcessAliveCheck( sFileName, aErrorStack ) == STL_SUCCESS );
        }

        /**
         * AIX에서는 fork 후에 Semaphore를 child가 정상적으로 인식하지 못한다.
         * 따라서, 반드시 fork 이후에 Semaphore를 생성해야 한다!!!!!
         */
        STL_TRY( stlCreateSemaphore( &(gConfigure.mLogSem),
                                     "LogSem",
                                     1,
                                     aErrorStack ) == STL_SUCCESS );
        sState = 2;

        STL_TRY( stlOpenFile( &sFile,
                              sFileName,
                              STL_FOPEN_CREATE | STL_FOPEN_WRITE | STL_FOPEN_APPEND | STL_FOPEN_BINARY,
                              STL_FPERM_OS_DEFAULT,
                              aErrorStack ) == STL_SUCCESS );
        sState = 3;
        
        stlSnprintf( sWriteBuff,
                     STL_SIZEOF( sWriteBuff ),
                     "%s:%d\n",
                     gConfigure.mCurrentGroupPtr->mName, sProc.mProcID );
        
        STL_TRY( stlLockFile( &sFile, 
                              STL_FLOCK_EXCLUSIVE,
                              aErrorStack ) == STL_SUCCESS );
        sState = 4;
                           
        STL_TRY( stlWriteFile( &sFile,
                               sWriteBuff,
                               stlStrlen( sWriteBuff ),
                               &sWriteSize,
                               aErrorStack ) == STL_SUCCESS );

        sState = 3;
        STL_TRY( stlUnlockFile( &sFile, 
                                aErrorStack ) == STL_SUCCESS );

        sState = 2;
        STL_TRY( stlCloseFile( &sFile,
                               aErrorStack ) == STL_SUCCESS );


        /**
         * Thread 생성 및 Wait
         */
        
        STL_TRY( ztcmCreateThreads( aErrorStack ) == STL_SUCCESS );

        STL_TRY( ztcmLogMsg( aErrorStack,
                             "STARTUP Done.\n" ) == STL_SUCCESS );

        if( gConfigure.mIsSilent == STL_FALSE )
        {
            if( gConfigure.mIsMaster == STL_TRUE )
            {
                stlPrintf( "[%s] Startup done as Master.\n", gConfigure.mCurrentGroupPtr->mName );
            }
            else
            {
                stlPrintf( "[%s] Startup done as Slave.\n", gConfigure.mCurrentGroupPtr->mName );
            }
        }

        STL_TRY( ztcmJoinThreads( aIsMaster, aErrorStack ) == STL_SUCCESS );

        if( gConfigure.mIsSilent == STL_FALSE )
        {
            stlPrintf( "[%s] stop done.\n", gConfigure.mCurrentGroupPtr->mName );
        }

        STL_TRY( ztcmLogMsg( aErrorStack,
                             "STOP Done.\n" ) == STL_SUCCESS );

        sState = 1;
        STL_TRY( stlDestroySemaphore( &(gConfigure.mLogSem),
                                      aErrorStack ) == STL_SUCCESS );
    }
    
    sState = 0;
    STL_TRY( stlDestroyRegionAllocator( &gConfigure.mAllocator,
                                        aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_GROUP_COUNT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_GROUP_COUNT,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    switch( sState )
    {
        case 4:
            (void)stlUnlockFile( &sFile, 
                                 aErrorStack );
        case 3:
            (void)stlCloseFile( &sFile,
                                aErrorStack );
        case 2:
            (void)stlDestroySemaphore( &(gConfigure.mLogSem),
                                       aErrorStack );
        case 1:
            (void)stlDestroyRegionAllocator( &gConfigure.mAllocator,
                                             aErrorStack );
        default:
            break;   
    }
    
    return STL_FAILURE;
}


stlStatus ztcmProcessAliveCheck( stlChar       * aFileName,
                                 stlErrorStack * aErrorStack )
{
    stlFile    sFile;
    stlChar    sLineBuff[STL_MAX_SQL_IDENTIFIER_LENGTH * 2] = {0,};
    stlChar    sGroupName[STL_MAX_SQL_IDENTIFIER_LENGTH] = {0,};
    stlChar    sProcessId[STL_MAX_SQL_IDENTIFIER_LENGTH] = {0,};
    stlChar  * sColonMark = NULL;
    stlChar  * sWriteBuff = NULL;
    stlChar  * sEndPtr    = NULL;
    stlBool    sFound     = STL_FALSE;
    stlInt32   sState     = 0;
    stlSize    sWriteSize = 0;
    stlOffset  sOffset    = 0;
    stlOffset  sFileSize  = 0;
    stlProc    sCheckProc;
    stlInt64   sCheckProcId;
    
    STL_TRY( stlOpenFile( &sFile,
                          aFileName,
                          STL_FOPEN_READ | STL_FOPEN_WRITE | STL_FOPEN_BINARY,
                          STL_FPERM_OS_DEFAULT,
                          aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( stlLockFile( &sFile, 
                          STL_FLOCK_EXCLUSIVE,
                          aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_END,
                          &sFileSize,
                          aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( sFileSize > 0, RAMP_SUCCESS );
    
    /**
     * Group:ProcessId가 Invalid할 경우에 제거하기 위한 Buffer
     */
    STL_TRY( stlAllocHeap( (void**)&sWriteBuff,
                           sFileSize,
                           aErrorStack ) == STL_SUCCESS );
    sState = 3;
    
    stlMemset( sWriteBuff, 0x00, sFileSize );

    sOffset = 0;
    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          aErrorStack ) == STL_SUCCESS );

    while( 1 )
    {
        STL_TRY( gRunState == STL_TRUE );

        if( stlGetStringFile( sLineBuff,
                              STL_SIZEOF( sLineBuff ),
                              &sFile,
                              aErrorStack ) == STL_SUCCESS )
        {
            sColonMark = stlStrchr( sLineBuff, ':' );
            
            if( sColonMark != NULL )
            {
                if( sFound == STL_FALSE )
                {
                    sColonMark++;
                
                    /**
                     * TODO : Windows에서는 개행문자가 다르기 때문에 아래의 처리가 변경되어야 함!!
                     */
                    stlStrncpy( sProcessId, 
                                sColonMark, 
                                stlStrlen( sColonMark ) );
                    
                    stlStrncpy( sGroupName, 
                                sLineBuff, 
                                stlStrlen( sLineBuff ) - stlStrlen( sProcessId ) - 1 );
                    
                    if( stlStrcmp( sGroupName, gConfigure.mCurrentGroupPtr->mName ) == 0 )
                    {
                        sFound = STL_TRUE;
                    }
                    else
                    {
                        stlSnprintf( sWriteBuff,
                                     sFileSize,
                                     "%s%s",
                                     sWriteBuff,
                                     sLineBuff );
                    }
                }
                else
                {
                    stlSnprintf( sWriteBuff,
                                 sFileSize,
                                 "%s%s",
                                 sWriteBuff,
                                 sLineBuff );    
                }
            }
        }
        else
        {
            STL_TRY( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_EOF );
            (void)stlPopError( aErrorStack );
            break;
        }
    }

    if( sFound == STL_TRUE )
    {
        STL_TRY( stlStrToInt64( sProcessId,
                                STL_NTS,
                                &sEndPtr,
                                10,
                                (stlInt64*)&sCheckProcId, 
                                aErrorStack ) == STL_SUCCESS );
        
        sCheckProc.mProcID = sCheckProcId;

        /** 입력된 String이 정상적으로 변환이 되었을 경우에는 EndPtr는 0 된다. */
        STL_TRY_THROW( *sEndPtr == 0, RAMP_ERR_INVALID_VALUE );
        
        /**
         * PROCESS가 살아있는지 확인
         */
        STL_TRY_THROW( stlKillProc( &sCheckProc,
                                    0,           //NULL Signal, Error Check
                                    aErrorStack ) != STL_SUCCESS, RAMP_ERR_ALREADY_RUNNING );

        /**
         * PID관련 파일은 있지만 Process가 존재하지 않을 경우에...
         */
        (void)stlPopError( aErrorStack );
        
        /**
         * PID에서 Group:ProcessId 제거
         */
        STL_TRY( stlTruncateFile( &sFile,
                                  0,
                                  aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( stlWriteFile( &sFile,
                               sWriteBuff,
                               stlStrlen( sWriteBuff ),
                               &sWriteSize,
                               aErrorStack ) == STL_SUCCESS );
    }
    
    sState = 2;
    stlFreeHeap( sWriteBuff );
    
    STL_RAMP( RAMP_SUCCESS );
    
    sState = 1;
    STL_TRY( stlUnlockFile( &sFile, 
                            aErrorStack ) == STL_SUCCESS );
    sState = 0;
    STL_TRY( stlCloseFile( &sFile,
                           aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_ALREADY_RUNNING )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_ALREADY_RUNNING,
                      NULL,
                      aErrorStack,
                      gConfigure.mCurrentGroupPtr->mName );
    }
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INTERNAL_ERROR,
                      NULL,
                      aErrorStack,
                      "invalid process id" );
    }
    STL_FINISH;
    
    switch( sState )
    {
        case 3:
            (void) stlFreeHeap( sWriteBuff );
        case 2:
            (void) stlUnlockFile( &sFile, 
                                  aErrorStack );
        case 1:
            (void) stlCloseFile( &sFile,
                                 aErrorStack );
        default:
            break; 
    }
    
    return STL_FAILURE;
}

stlStatus ztcmStop( stlBool         aIsMaster,
                    stlErrorStack * aErrorStack )
{
    stlProc    sProc;
    stlInt64   sProcId;
    stlFile    sFile;
    stlChar    sFileName[STL_MAX_FILE_PATH_LENGTH];
    stlChar    sLineBuff[STL_MAX_SQL_IDENTIFIER_LENGTH * 2]  = {0,};
    stlChar    sGroupName[STL_MAX_SQL_IDENTIFIER_LENGTH] = {0,};
    stlChar    sProcessId[STL_MAX_SQL_IDENTIFIER_LENGTH] = {0,};
    stlChar  * sWriteBuff = NULL;
    stlChar  * sColonMark = NULL;
    stlChar  * sEndPtr    = NULL;
    stlBool    sExist     = STL_FALSE;
    stlBool    sFound     = STL_FALSE;
    stlInt32   sState     = 0;
    stlSize    sWriteSize = 0;
    stlOffset  sOffset    = 0;
    stlOffset  sFileSize  = 0;
    
    STL_TRY( ztcmGetPidFileName( sFileName,
                                 aIsMaster,
                                 aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlExistFile( sFileName,
                           &sExist,
                           aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( sExist == STL_TRUE, RAMP_ERR_NOT_FILE_EXIST );
    
    STL_TRY( stlOpenFile( &sFile,
                          sFileName,
                          STL_FOPEN_READ | STL_FOPEN_WRITE | STL_FOPEN_BINARY,
                          STL_FPERM_OS_DEFAULT,
                          aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( stlLockFile( &sFile, 
                          STL_FLOCK_EXCLUSIVE,
                          aErrorStack ) == STL_SUCCESS );
    sState = 2;
    
    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_END,
                          &sFileSize,
                          aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlAllocHeap( (void**)&sWriteBuff,
                           sFileSize,
                           aErrorStack ) == STL_SUCCESS );
    sState = 3;
            
    stlMemset( sWriteBuff, 0x00, sFileSize );

    sOffset = 0;
    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          aErrorStack ) == STL_SUCCESS );
    
    sFound = STL_FALSE;
    
    while( 1 )
    {
        STL_TRY( gRunState == STL_TRUE );

        if( stlGetStringFile( sLineBuff,
                              STL_SIZEOF( sLineBuff ),
                              &sFile,
                              aErrorStack ) == STL_SUCCESS )
        {
            sColonMark = stlStrchr( sLineBuff, ':' );
            
            if( sColonMark != NULL )
            {
                sColonMark++;
                
                stlStrncpy( sProcessId, 
                            sColonMark, 
                            stlStrlen( sColonMark ) );
                
                stlStrncpy( sGroupName, 
                            sLineBuff, 
                            stlStrlen( sLineBuff ) - stlStrlen( sProcessId ) - 1 );
                
                if( stlStrlen( gConfigure.mSetGroupName ) > 0 )
                {
                    /**
                     * 특정 Group Process만 Kill
                     */
                    if( sFound == STL_FALSE )
                    {
                        if( stlStrcasecmp( sGroupName, gConfigure.mSetGroupName ) == 0 )
                        {
                            sFound = STL_TRUE;
                            
                            STL_TRY( stlStrToInt64( sProcessId,
                                                    STL_NTS,
                                                    &sEndPtr,
                                                    10,
                                                    (stlInt64*)&sProcId, 
                                                    aErrorStack ) == STL_SUCCESS );
                            
                            sProc.mProcID = sProcId;

                            /** 입력된 String이 정상적으로 변환이 되었을 경우에는 EndPtr는 0 된다. */
                            STL_TRY_THROW( *sEndPtr == 0, RAMP_ERR_INVALID_VALUE );
        
                            if( stlKillProc( &sProc,
                                             ZTC_SIGNAL_DEFINE,
                                             aErrorStack ) != STL_SUCCESS ) 
                            {
                                //Nothing To Do.
                            }
                        }
                        else
                        {
                            stlSnprintf( sWriteBuff,
                                         sFileSize,
                                         "%s%s",
                                         sWriteBuff,
                                         sLineBuff );
                        }
                    }
                    else
                    {
                        stlSnprintf( sWriteBuff,
                                     sFileSize,
                                     "%s%s",
                                     sWriteBuff,
                                     sLineBuff );    
                    }
                }
                else
                {
                    /**
                     * 모든 Group Process Kill
                     */
                    STL_TRY( stlStrToInt64( sProcessId,
                                            STL_NTS,
                                            &sEndPtr,
                                            10,
                                            (stlInt64*)&sProcId, 
                                            aErrorStack ) == STL_SUCCESS );
                    
                    sProc.mProcID = sProcId;

                    /** 입력된 String이 정상적으로 변환이 되었을 경우에는 EndPtr는 0 된다. */
                    STL_TRY_THROW( *sEndPtr == 0, RAMP_ERR_INVALID_VALUE );
        
                    if( stlKillProc( &sProc,
                                     ZTC_SIGNAL_DEFINE,
                                     aErrorStack ) != STL_SUCCESS ) 
                    {
                        //Nothing To Do.
                    }
                }
            }
        }
        else
        {
            STL_TRY( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_EOF );
            (void)stlPopError( aErrorStack );
            break;
        }
    }

    if( stlStrlen( gConfigure.mSetGroupName ) > 0 )
    {
        STL_TRY_THROW( sFound == STL_TRUE, RAMP_ERR_NOT_FOUND_GROUP );
    }

    /**
     * 특정 Group만 종료시킬 경우, 나머지 Group 내용을 다시 저장한다.
     */
    if( stlStrlen( sWriteBuff ) > 0 && sFound == STL_TRUE )
    {
        STL_TRY( stlTruncateFile( &sFile,
                                  0,
                                  aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( stlWriteFile( &sFile,
                               sWriteBuff,
                               stlStrlen( sWriteBuff ),
                               &sWriteSize,
                               aErrorStack ) == STL_SUCCESS );   
    }
            
    sState = 2;
    stlFreeHeap( sWriteBuff );
    
    sState = 1;
    STL_TRY( stlUnlockFile( &sFile, 
                            aErrorStack ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( stlCloseFile( &sFile,
                           aErrorStack ) == STL_SUCCESS );
    
    /**
     * 모든 Process를 kill 하거나, 특정 Process를 Kill 했는데 하나만 남아있는 경우
     * pid 파일을 제거한다.
     */
    if( stlStrlen( gConfigure.mSetGroupName ) == 0 || 
        ( sWriteSize == 0 && sFound == STL_TRUE ) )
    {
        STL_TRY( stlRemoveFile( sFileName, 
                                aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_FOUND_GROUP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_NOT_RUNNING,
                      NULL,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_NOT_FILE_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_NOT_RUNNING,
                      NULL,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INTERNAL_ERROR,
                      NULL,
                      aErrorStack,
                      "invalid process id" );
    }
    STL_FINISH;
    
    switch( sState )
    {
        case 3:
            stlFreeHeap( sWriteBuff );
        case 2:
            (void) stlUnlockFile( &sFile, 
                                  aErrorStack );
        case 1:
            (void)stlCloseFile( &sFile,
                                aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
    
}

stlStatus ztcmGetPidFileName( stlChar       * aFileName,
                              stlBool         aIsMaster,
                              stlErrorStack * aErrorStack )
{
    stlChar   sFileName[STL_MAX_FILE_PATH_LENGTH];
    
    if( aIsMaster == STL_TRUE )
    {
        stlSnprintf( sFileName, 
                     STL_SIZEOF( sFileName ),
                     "%sconf"STL_PATH_SEPARATOR"%s",
                     gConfigure.mHomeDir,
                     ZTC_DEFAULT_MASTER_PID_FILE );
    }
    else
    {
        stlSnprintf( sFileName, 
                     STL_SIZEOF( sFileName ),
                     "%sconf"STL_PATH_SEPARATOR"%s",
                     gConfigure.mHomeDir,
                     ZTC_DEFAULT_SLAVE_PID_FILE );
    }
    
    stlStrncpy( aFileName, 
                sFileName, 
                stlStrlen( sFileName ) + 1 );
    
    return STL_SUCCESS;
}

stlStatus ztcmSetConfigureFile( const stlChar * aConfigureFile,
                                stlErrorStack * aErrorStack )
{
    STL_TRY_THROW( stlStrlen( aConfigureFile ) < STL_MAX_FILE_PATH_LENGTH, RAMP_ERR_INVALID_FILE_NAME );
    
    stlStrncpy( gConfigure.mUserConfigure, 
                aConfigureFile,
                stlStrlen( aConfigureFile ) + 1 );
    
    return STL_SUCCESS;   
    
    STL_CATCH( RAMP_ERR_INVALID_FILE_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_FILE_NAME,
                      NULL,
                      aErrorStack,
                      aConfigureFile );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcmSetGroupName( const stlChar * aGroupName,
                            stlErrorStack * aErrorStack )
{
    STL_TRY_THROW( stlStrlen( aGroupName ) < STL_MAX_SQL_IDENTIFIER_LENGTH, RAMP_ERR_INVALID_VALUE );
    
    stlStrncpy( gConfigure.mSetGroupName, 
                aGroupName,
                stlStrlen( aGroupName ) + 1 );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_GROUP_NAME,
                      NULL,
                      aErrorStack,
                      aGroupName );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcmGetConfigureFileName( stlChar       * aFileName,
                                    stlErrorStack * aErrorStack )
{
    stlChar   sFileName[STL_MAX_FILE_PATH_LENGTH];
    stlChar * sAbsolute = NULL;
    stlBool   sExist    = STL_FALSE;
    
    
    /**
     * Configure File 경로 검색
     * - Configure 옵션을 설정했을 경우
     *   1. 현재 Directory에서 검색
     *   2. GOLDILOCKS_DATA/conf에서 검색
     *   3. 절대 경로일 경우 해당 Path에서만 검색
     * - Configure 옵션을 설정하지 않았을 경우
     *   1. master : cyclone.master.conf
     *   2. slave  : cyclone.slave.conf
     */
    
    if( stlStrlen( gConfigure.mUserConfigure ) > 0 )
    {
        sAbsolute = stlStrchr( gConfigure.mUserConfigure, '/' );
        
        STL_TRY( stlExistFile( gConfigure.mUserConfigure,
                               &sExist,
                               aErrorStack ) == STL_SUCCESS );
        
        if( sExist == STL_FALSE )
        {
            if( sAbsolute == gConfigure.mUserConfigure )
            {
                stlStrncpy( sFileName, 
                            gConfigure.mUserConfigure, 
                            stlStrlen( gConfigure.mUserConfigure ) + 1 );
                
                STL_TRY_THROW( sExist == STL_TRUE, RAMP_ERR_FILE_NOT_EXIST );
            }
            
            stlSnprintf( sFileName, 
                         STL_SIZEOF( sFileName ), 
                         "%sconf"STL_PATH_SEPARATOR"%s", 
                         gConfigure.mHomeDir, 
                         gConfigure.mUserConfigure );
            
            STL_TRY( stlExistFile( sFileName,
                                   &sExist,
                                   aErrorStack ) == STL_SUCCESS );
            
            STL_TRY_THROW( sExist == STL_TRUE, RAMP_ERR_FILE_NOT_EXIST );
            
            stlStrncpy( aFileName, 
                        sFileName, 
                        stlStrlen( sFileName ) + 1 );
        }
        else
        {
            stlStrncpy( aFileName, 
                        gConfigure.mUserConfigure, 
                        stlStrlen( gConfigure.mUserConfigure ) + 1 );
        }
    }
    else
    {
        if( gConfigure.mIsMaster == STL_TRUE )
        {
            stlSnprintf( sFileName, 
                         STL_SIZEOF( sFileName ), 
                         "%sconf"STL_PATH_SEPARATOR"%s", 
                         gConfigure.mHomeDir, 
                         ZTC_DEFAULT_MASTER_CONFIGURE_FILE );
        }
        else
        {
            stlSnprintf( sFileName, 
                         STL_SIZEOF( sFileName ), 
                         "%sconf"STL_PATH_SEPARATOR"%s", 
                         gConfigure.mHomeDir, 
                         ZTC_DEFAULT_SLAVE_CONFIGURE_FILE );
        }
        
        STL_TRY( stlExistFile( sFileName,
                               &sExist,
                               aErrorStack ) == STL_SUCCESS );
            
        STL_TRY_THROW( sExist == STL_TRUE, RAMP_ERR_FILE_NOT_EXIST );
        
        stlStrncpy( aFileName, 
                    sFileName, 
                    stlStrlen( sFileName ) + 1 );
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_FILE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_FILE_NAME,
                      NULL,
                      aErrorStack,
                      sFileName );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztcmGetCurrentLogFileInfo( ztcLogFileInfo ** aLogFileInfo,
                                     stlErrorStack   * aErrorStack )
{
    ztcLogFileInfo * sLogFileInfo = NULL;
    stlBool          sFound       = STL_FALSE;
    stlBool          sExist       = STL_FALSE;
    stlInt16         sWrapNo      = ZTC_INVALID_WRAP_NO;
    stlChar        * sLogFilePath = NULL;
    
    STL_RING_FOREACH_ENTRY( &gMasterMgr->mLogFileList, sLogFileInfo )
    {
        if ( sLogFileInfo->mActive == STL_TRUE )
        {
            sFound = STL_TRUE;
            break;
        }
    }
    
    STL_TRY_THROW( sFound == STL_TRUE, RAMP_ERR_INVALID_ACCESS );
    
    /**
     * LogMirror에서 생성한 LogFile을 읽을 경우에는 해당 File이름을 변경해야 한다.
     */
    if( ztcmGetUserLogFilePathStatus() == STL_TRUE )
    {
        sLogFilePath = ztcmGetUserLogFilePath();
        
        stlSnprintf( sLogFileInfo->mName,
                     STL_MAX_FILE_PATH_LENGTH,
                     "%s"STL_PATH_SEPARATOR"%s%ld.log",
                     sLogFilePath,
                     ZTC_LOG_FILE_PREFIX,
                     sLogFileInfo->mCaptureInfo.mFileSeqNo );
    }
    
    /**
     * 최초 실행시(Slave에 Restart 정보가 없을 경우), WrapNo는 INVALID한 상태이다.
     * 이 경우에는 WrapNo를 File에서 직접 얻어온다.
     * 현재 Online Redo 로그를 읽는 로직이므로, 이 시점에서 ArchiveLog를 읽어야 한다면 오류가 발생할 것이다.
     */
    if( sLogFileInfo->mCaptureInfo.mWrapNo == ZTC_INVALID_WRAP_NO )
    {
        /**
         * 최초 실행시 파일이 없다면...
         */
        STL_TRY( stlExistFile( sLogFileInfo->mName,
                               &sExist,
                               aErrorStack ) == STL_SUCCESS );
        
        STL_TRY_THROW( sExist == STL_TRUE, RAMP_ERR_INVALID_FILE );
        
        STL_TRY( ztccGetLogFileWrapNo( sLogFileInfo->mName,
                                       &sWrapNo,
                                       aErrorStack ) == STL_SUCCESS );
    
        sLogFileInfo->mCaptureInfo.mWrapNo = sWrapNo;
    }
    
    *aLogFileInfo = sLogFileInfo;
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_ACCESS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_NO_ACTIVE_LOG_FILE,
                      NULL,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_INVALID_FILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_MUST_EXIST_LOG_FILE,
                      NULL,
                      aErrorStack,
                      sLogFileInfo->mName );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztcmGetNextLogFileInfo( ztcLogFileInfo ** aLogFileInfo,
                                  stlErrorStack   * aErrorStack )
{
    ztcLogFileInfo * sLogFileInfoBef = *aLogFileInfo;
    ztcLogFileInfo * sLogFileInfoAft = NULL;
    stlChar        * sLogFilePath    = NULL;
    stlInt64         sFileSeqNo;
    stlInt16         sWrapNo;
    
    sFileSeqNo = sLogFileInfoBef->mCaptureInfo.mFileSeqNo;
    sWrapNo    = sLogFileInfoBef->mCaptureInfo.mWrapNo;
    sLogFileInfoBef->mActive = STL_FALSE;
            
    sLogFileInfoAft = STL_RING_GET_NEXT_DATA( &gMasterMgr->mLogFileList, sLogFileInfoBef );
    
    /**
     * Ring의 맨끝이라면 처음 데이터를 얻어온다.
     */
    if ( (void*)sLogFileInfoAft == (void*)&gMasterMgr->mLogFileList )
    {
        sLogFileInfoAft = STL_RING_GET_FIRST_DATA( &gMasterMgr->mLogFileList );
    }
    
    /**
     * re-Initialize 
     */
    sLogFileInfoAft->mActive                     = STL_TRUE;
    sLogFileInfoAft->mCaptureInfo.mFileSeqNo     = sFileSeqNo + 1;
    sLogFileInfoAft->mCaptureInfo.mBlockSeq      = 0;
    sLogFileInfoAft->mCaptureInfo.mReadLogNo     = 1;
    sLogFileInfoAft->mCaptureInfo.mRestartLSN    = ZTC_INVALID_LSN;
    sLogFileInfoAft->mCaptureInfo.mLastCommitLSN = ZTC_INVALID_LSN;
    
    /**
     * LogMirror에서 생성한 LogFile을 읽을 경우에는 해당 File이름을 변경해야 한다.
     */
    if( ztcmGetUserLogFilePathStatus() == STL_TRUE )
    {
        sLogFilePath = ztcmGetUserLogFilePath();
        
        stlSnprintf( sLogFileInfoAft->mName,
                     STL_MAX_FILE_PATH_LENGTH,
                     "%s"STL_PATH_SEPARATOR"%s%ld.log",
                     sLogFilePath,
                     ZTC_LOG_FILE_PREFIX,
                     sLogFileInfoAft->mCaptureInfo.mFileSeqNo );
    }
    
    if( sWrapNo + 1 > ZTC_WRAP_NO_MAX ) //sWrapNo == ZTC_WRAP_NO_MAX
    {
        sLogFileInfoAft->mCaptureInfo.mWrapNo = 0;
    }
    else
    {
        sLogFileInfoAft->mCaptureInfo.mWrapNo = sWrapNo + 1;
    }
    
    /**
     * Next LogFileInfo.
     */
    *aLogFileInfo = sLogFileInfoAft;
    
    return STL_SUCCESS;
}


stlStatus ztcmSetUserIdToGlobalArea( stlChar       * aUserId,
                                     stlErrorStack * aErrorStack )
{
    stlInt32   sLen = 0;
    
    sLen = stlStrlen( aUserId );
    
    STL_TRY_THROW( sLen < STL_MAX_SQL_IDENTIFIER_LENGTH, RAMP_ERR_TOO_LONG_USER_ID );
    
    stlStrncpy( gConfigure.mUserId, aUserId, sLen + 1 );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_TOO_LONG_USER_ID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "User id",
                      0 );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcmSetUserIdToGroupArea( stlChar       * aUserId,
                                    stlErrorStack * aErrorStack )
{
    stlInt32       sLen   = 0;
    ztcGroupItem * sGroup = NULL;
    
    sLen = stlStrlen( aUserId );
    
    STL_TRY_THROW( sLen < STL_MAX_SQL_IDENTIFIER_LENGTH, RAMP_ERR_TOO_LONG_USER_ID );
    
    STL_TRY_THROW( STL_RING_IS_EMPTY( &gConfigure.mGroupList ) != STL_TRUE, RAMP_ERR_INVALID_GROUP );
    
    sGroup = STL_RING_GET_LAST_DATA( &gConfigure.mGroupList );
    
    stlStrncpy( sGroup->mUserId, aUserId, sLen + 1 );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_TOO_LONG_USER_ID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "User id",
                      0 );
    }
    STL_CATCH( RAMP_ERR_INVALID_GROUP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_GROUP_NAME,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcmGetUserId( stlChar       * aUserId,
                         stlErrorStack * aErrorStack )
{
    stlInt32 sLen = 0;
    
    sLen = stlStrlen( gConfigure.mCurrentGroupPtr->mUserId );
    
    /**
     * Group에서 UserID를 세팅한 경우에는 Group 정보를 가져오고, 
     * 아니면 gConfigure에서 가져온다.
     */
    if( sLen > 0 )
    {
        stlStrncpy( aUserId, gConfigure.mCurrentGroupPtr->mUserId, sLen + 1 );
    }
    else
    {
        STL_TRY_THROW( stlStrlen( gConfigure.mUserId ) > 0, RAMP_ERR_INVALID_VALUE );
        
        stlStrncpy( aUserId, gConfigure.mUserId, stlStrlen( gConfigure.mUserId ) + 1 );
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_CONN_INFO,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}
                        
stlStatus ztcmSetUserPwToGlobalArea( stlChar       * aUserPw,
                                     stlErrorStack * aErrorStack )
{
    stlInt32   sLen = 0;

    STL_TRY_THROW( stlStrlen( gConfigure.mUserPw ) == 0, RAMP_ERR_ALREADY_EXIST_USER_PW );

    sLen = stlStrlen( aUserPw );
    
    STL_TRY_THROW( sLen < STL_MAX_SQL_IDENTIFIER_LENGTH, RAMP_ERR_TOO_LONG_USER_PW );
    
    stlStrncpy( gConfigure.mUserPw, aUserPw, sLen + 1 );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_ALREADY_EXIST_USER_PW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_PASSWD_ALREADY_EXISTS,
                      NULL,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_TOO_LONG_USER_PW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "User pw",
                      0 );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcmSetUserPwToGroupArea( stlChar       * aUserPw,
                                    stlErrorStack * aErrorStack )
{
    stlInt32       sLen   = 0;
    ztcGroupItem * sGroup = NULL;
    
    sLen = stlStrlen( aUserPw );
    
    STL_TRY_THROW( sLen < STL_MAX_SQL_IDENTIFIER_LENGTH, RAMP_ERR_TOO_LONG_USER_PW );
    
    STL_TRY_THROW( STL_RING_IS_EMPTY( &gConfigure.mGroupList ) != STL_TRUE, RAMP_ERR_INVALID_GROUP );
    
    sGroup = STL_RING_GET_LAST_DATA( &gConfigure.mGroupList );

    STL_TRY_THROW( stlStrlen( sGroup->mUserPw ) == 0, RAMP_ERR_ALREADY_EXIST_USER_PW );

    stlStrncpy( sGroup->mUserPw, aUserPw, sLen + 1 );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_ALREADY_EXIST_USER_PW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_PASSWD_ALREADY_EXISTS,
                      NULL,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_TOO_LONG_USER_PW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "User pw",
                      0 );
    }
    STL_CATCH( RAMP_ERR_INVALID_GROUP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_GROUP_NAME,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcmGetUserPw( stlChar       * aUserPw,
                         stlErrorStack * aErrorStack )
{
    stlInt32 sLen = 0;

    sLen = stlStrlen( gConfigure.mCurrentGroupPtr->mUserPw );

    /**
     * Group에서 UserPw를 세팅한 경우에는 Group 정보를 가져오고,
     * 아니면 gConfigure에서 가져온다.
     */
    if( sLen > 0 )
    {
        stlStrncpy( aUserPw, gConfigure.mCurrentGroupPtr->mUserPw, sLen + 1 );
    }
    else
    {
        STL_TRY_THROW( stlStrlen( gConfigure.mUserPw ) > 0, RAMP_ERR_INVALID_VALUE );

        stlStrncpy( aUserPw, gConfigure.mUserPw, stlStrlen( gConfigure.mUserPw ) + 1 );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_CONN_INFO,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztcmSetUserEncryptPwToGlobalArea( stlChar       * aUserEncryptPw,
                                            stlErrorStack * aErrorStack )
{
    stlChar     sPlainText[ZTC_MAX_CIPHER_SIZE];
    stlChar     sCipherText[ZTC_MAX_CIPHER_SIZE];
    stlSize     sCipherTextLen;
    stlSize     sPlainTextLen;

    STL_TRY_THROW( stlStrlen( gConfigure.mUserPw ) == 0, RAMP_ERR_ALREADY_EXIST_USER_PW );

    STL_TRY_THROW( stlStrlen( gConfigure.mKey ) != 0, RAMP_NO_KEY );

    sCipherTextLen = STL_SIZEOF( sCipherText );

    STL_TRY( ztcmBase64Decode( aUserEncryptPw, (stlUInt8 *)sCipherText, &sCipherTextLen, aErrorStack )
             == STL_SUCCESS );

    sPlainTextLen = STL_SIZEOF( sPlainText );
    STL_TRY( ztcmDecrypt( sCipherText, sCipherTextLen, gConfigure.mKey, sPlainText, &sPlainTextLen, aErrorStack )
             == STL_SUCCESS );

    sPlainText[sPlainTextLen] = 0x00;

    STL_TRY( ztcmSetUserPwToGlobalArea( sPlainText, aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_ALREADY_EXIST_USER_PW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_PASSWD_ALREADY_EXISTS,
                      NULL,
                      aErrorStack );
    }
    STL_CATCH( RAMP_NO_KEY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_NO_KEY_PARAMETER,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztcmSetUserEncryptPwToGroupArea( stlChar       * aUserEncryptPw,
                                           stlErrorStack * aErrorStack )
{
    stlChar        sPlainText[ZTC_MAX_CIPHER_SIZE];
    stlChar        sCipherText[ZTC_MAX_CIPHER_SIZE];
    stlSize        sCipherTextLen;
    stlSize        sPlainTextLen;
    ztcGroupItem * sGroup = NULL;

    STL_TRY_THROW( STL_RING_IS_EMPTY( &gConfigure.mGroupList ) != STL_TRUE, RAMP_ERR_INVALID_GROUP );

    sGroup = STL_RING_GET_LAST_DATA( &gConfigure.mGroupList );

    STL_TRY_THROW( stlStrlen( sGroup->mUserPw ) == 0, RAMP_ERR_ALREADY_EXIST_USER_PW );

    STL_TRY_THROW( stlStrlen( gConfigure.mKey ) != 0, RAMP_NO_KEY );

    sCipherTextLen = STL_SIZEOF( sCipherText );

    STL_TRY( ztcmBase64Decode( aUserEncryptPw, (stlUInt8 *)sCipherText, &sCipherTextLen, aErrorStack )
             == STL_SUCCESS );

    sPlainTextLen = STL_SIZEOF( sPlainText );
    STL_TRY( ztcmDecrypt( sCipherText, sCipherTextLen, gConfigure.mKey, sPlainText, &sPlainTextLen, aErrorStack )
             == STL_SUCCESS );

    sPlainText[sPlainTextLen] = 0x00;

    STL_TRY( ztcmSetUserPwToGroupArea( sPlainText, aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_ALREADY_EXIST_USER_PW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_PASSWD_ALREADY_EXISTS,
                      NULL,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_INVALID_GROUP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_GROUP_NAME,
                      NULL,
                      aErrorStack );
    }
    STL_CATCH( RAMP_NO_KEY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_NO_KEY_PARAMETER,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztcmSetPort( stlChar       * aPort,
                       stlErrorStack * aErrorStack )
{
    stlChar    * sEndPtr;
    stlInt64     sPort = 0;
    
    STL_TRY( stlStrToInt64( aPort,
                            STL_NTS,
                            &sEndPtr,
                            10,
                            &sPort, 
                            aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( *sEndPtr == 0, RAMP_ERR_INVALID_VALUE );
    
    STL_TRY_THROW( (sPort >= 1024) && (sPort <= 49151), RAMP_ERR_INVALID_VALUE );
    
    gConfigure.mPort = (stlInt32) sPort;
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_PORT,
                      NULL,
                      aErrorStack,
                      aPort);
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcmGetPort( stlInt32      * aPort,
                       stlErrorStack * aErrorStack )
{
    stlInt64    sPort       = 0;
    stlBool     sFound      = STL_FALSE;
    stlChar   * sEndPtr     = NULL;
    stlChar     sTmpStr[16] = {0,};
    
    STL_TRY( stlGetEnv( (stlChar*)sTmpStr, 
                        STL_SIZEOF( sTmpStr ),
                        "CYCLONEM_PORT",
                        &sFound,
                        aErrorStack ) == STL_SUCCESS );

    if( sFound == STL_TRUE )
    {
        STL_TRY( stlStrToInt64( sTmpStr,
                                STL_NTS,
                                &sEndPtr,
                                10,
                                &sPort, 
                                aErrorStack ) == STL_SUCCESS );
    
        /** 입력된 String이 정상적으로 변환이 되었을 경우에는 EndPtr는 0 된다. */
        STL_TRY_THROW( *sEndPtr == 0, RAMP_ERR_INVALID_VALUE );
    }
    else
    {
        sPort = ZTC_DEFAULT_PORT;
    }
    
    if( gConfigure.mPort != 0 )
    {
        sPort = gConfigure.mPort;
    }
    
    STL_TRY_THROW( sPort < 65536 && 
                   sPort != 0, RAMP_ERR_INVALID_VALUE );
    
    *aPort = (stlInt32) sPort;
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_PORT,
                      NULL,
                      aErrorStack,
                      "PORT");
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcmSetGoldilocksListenPort( stlChar       * aPort,
                                  stlErrorStack * aErrorStack )
{
    stlChar    * sEndPtr;
    stlInt64     sPort = 0;
    
    STL_TRY( stlStrToInt64( aPort,
                            STL_NTS,
                            &sEndPtr,
                            10,
                            &sPort, 
                            aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( *sEndPtr == 0, RAMP_ERR_INVALID_VALUE );
    
    STL_TRY_THROW( sPort < 65536, RAMP_ERR_INVALID_VALUE );
    
    gConfigure.mGoldilocksListenPort = (stlInt32) sPort;
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_PORT,
                      NULL,
                      aErrorStack,
                      "GOLDILOCKS_LISTEN_PORT");
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcmGetGoldilocksListenPort( stlInt32      * aPort,
                                  stlErrorStack * aErrorStack )
{
    stlInt64    sPort       = 0;
    stlBool     sFound      = STL_FALSE;
    stlChar   * sEndPtr     = NULL;
    stlChar     sTmpStr[16] = {0,};
    
    /**
     * 환경파일의 GOLDILOCKS_LISTEN_PORT -> 환경 파일의 HOST_PORT -> 환경 변수 GOLDILOCKS_LISTEN_PORT 
     * 순서로 읽는다
     */
    
    if( gConfigure.mGoldilocksListenPort > 0 )
    {
        sPort = gConfigure.mGoldilocksListenPort;
    }
    else if( gConfigure.mCurrentGroupPtr->mHostPort > 0 )
    {
        sPort = gConfigure.mCurrentGroupPtr->mHostPort;
    }
    else if( gConfigure.mHostPort > 0 )
    {
        sPort = gConfigure.mHostPort;
    }
    else
    {
        STL_TRY( stlGetEnv( (stlChar*)sTmpStr, 
                            STL_SIZEOF( sTmpStr ),
                            ZTC_ENV_LISTEN_PORT,
                            &sFound,
                            aErrorStack ) == STL_SUCCESS );
        
        STL_TRY_THROW( sFound == STL_TRUE, RAMP_ERR_INVALID_VALUE );
        
        STL_TRY( stlStrToInt64( sTmpStr,
                                STL_NTS,
                                &sEndPtr,
                                10,
                                &sPort, 
                                aErrorStack ) == STL_SUCCESS );
    
        /** 입력된 String이 정상적으로 변환이 되었을 경우에는 EndPtr는 0 된다. */
        STL_TRY_THROW( *sEndPtr == 0, RAMP_ERR_INVALID_VALUE );
    }
    
    STL_TRY_THROW( sPort < 65536 && 
                   sPort != 0, RAMP_ERR_INVALID_VALUE );
    
    *aPort = (stlInt32) sPort;
    
    return STL_SUCCESS;   
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_PORT,
                      NULL,
                      aErrorStack,
                      "GOLDILOCKS_LISTEN_PORT or HOST_PORT or GOLDILOCKS_LISTEN_PORT(env)");
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcmSetHostPortToGlobalArea( stlChar       * aPort,
                                       stlErrorStack * aErrorStack )
{
    stlChar    * sEndPtr;
    stlInt64     sPort = 0;
    
    STL_TRY( stlStrToInt64( aPort,
                            STL_NTS,
                            &sEndPtr,
                            10,
                            &sPort, 
                            aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( *sEndPtr == 0, RAMP_ERR_INVALID_VALUE );
    
    STL_TRY_THROW( sPort < 65536, RAMP_ERR_INVALID_VALUE );
    
    gConfigure.mHostPort = (stlInt32) sPort;
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_PORT,
                      NULL,
                      aErrorStack,
                      "HOST_PORT" );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}
                                     
stlStatus ztcmSetHostPortToGroupArea( stlChar       * aPort,
                                      stlErrorStack * aErrorStack )
{
    stlChar      * sEndPtr;
    stlInt64       sPort = 0;
    ztcGroupItem * sGroup = NULL;
    
    STL_TRY( stlStrToInt64( aPort,
                            STL_NTS,
                            &sEndPtr,
                            10,
                            &sPort, 
                            aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( *sEndPtr == 0, RAMP_ERR_INVALID_VALUE );
    
    STL_TRY_THROW( sPort < 65536, RAMP_ERR_INVALID_VALUE );
    
    STL_TRY_THROW( STL_RING_IS_EMPTY( &gConfigure.mGroupList ) != STL_TRUE, RAMP_ERR_INVALID_GROUP );
    
    sGroup = STL_RING_GET_LAST_DATA( &gConfigure.mGroupList );
    
    sGroup->mHostPort = (stlInt32) sPort;
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_PORT,
                      NULL,
                      aErrorStack,
                      "HOST_PORT");
    }
    STL_CATCH( RAMP_ERR_INVALID_GROUP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_GROUP_NAME,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcmGetHostPort( stlInt32      * aPort,
                           stlErrorStack * aErrorStack )
{
    stlInt32 sPort = 0;
    
    if( gConfigure.mCurrentGroupPtr->mHostPort > 0 )
    {
        sPort = gConfigure.mCurrentGroupPtr->mHostPort;
    }
    else
    {
        sPort = gConfigure.mHostPort;
    }
    
    STL_TRY_THROW( sPort < 65536 && 
                   sPort != 0, RAMP_ERR_INVALID_VALUE );
    
    *aPort = sPort;
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_PORT,
                      NULL,
                      aErrorStack,
                      "HOST_PORT");
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcmSetHostIpToGlobalArea( stlChar       * aHostIp,
                                     stlErrorStack * aErrorStack )
{
    stlInt32   sLen = 0;
    
    sLen = stlStrlen( aHostIp );
    
    STL_TRY_THROW( sLen < 16, RAMP_ERR_TOO_LONG_IP );
    
    stlStrncpy( gConfigure.mHostIp, aHostIp, sLen + 1 );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_TOO_LONG_IP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "Host IP",
                      0 );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}
                                     
stlStatus ztcmSetHostIpToGroupArea( stlChar       * aHostIp,
                                    stlErrorStack * aErrorStack )
{
    stlInt32       sLen   = 0;
    ztcGroupItem * sGroup = NULL;
    
    sLen = stlStrlen( aHostIp );
    
    STL_TRY_THROW( sLen < 16, RAMP_ERR_TOO_LONG_IP );
    
    STL_TRY_THROW( STL_RING_IS_EMPTY( &gConfigure.mGroupList ) != STL_TRUE, RAMP_ERR_INVALID_GROUP );
    
    sGroup = STL_RING_GET_LAST_DATA( &gConfigure.mGroupList );
    
    stlStrncpy( sGroup->mHostIp, aHostIp, sLen + 1 );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_TOO_LONG_IP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "Host IP",
                      0 );
    }
    STL_CATCH( RAMP_ERR_INVALID_GROUP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_GROUP_NAME,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlChar * ztcmGetHostIp()
{
    stlChar  * ret = NULL;
    
    if( stlStrlen( gConfigure.mCurrentGroupPtr->mHostIp ) > 0 )
    {
        ret = gConfigure.mCurrentGroupPtr->mHostIp;
    }
    else if( stlStrlen( gConfigure.mHostIp ) > 0 )
    {
        ret = gConfigure.mHostIp;
    }
    
    return ret;
}

stlStatus ztcmSetDsnToGlobalArea( stlChar       * aDsn,
                                  stlErrorStack * aErrorStack )
{
    stlInt32   sLen = 0;
    
    sLen = stlStrlen( aDsn );
    
    STL_TRY_THROW( sLen < STL_MAX_SQL_IDENTIFIER_LENGTH, RAMP_ERR_TOO_LONG_VALUE );
    
    stlStrncpy( gConfigure.mDsn, aDsn, sLen + 1 );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_TOO_LONG_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "DSN",
                      0 );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}
                                     
stlStatus ztcmSetDsnToGroupArea( stlChar       * aDsn,
                                 stlErrorStack * aErrorStack )
{
    stlInt32       sLen   = 0;
    ztcGroupItem * sGroup = NULL;
    
    sLen = stlStrlen( aDsn );
    
    STL_TRY_THROW( sLen < STL_MAX_SQL_IDENTIFIER_LENGTH, RAMP_ERR_TOO_LONG_VALUE );
    
    STL_TRY_THROW( STL_RING_IS_EMPTY( &gConfigure.mGroupList ) != STL_TRUE, RAMP_ERR_INVALID_GROUP );
    
    sGroup = STL_RING_GET_LAST_DATA( &gConfigure.mGroupList );
    
    stlStrncpy( sGroup->mDsn, aDsn, sLen + 1 );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_TOO_LONG_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "DSN",
                      0 );
    }
    STL_CATCH( RAMP_ERR_INVALID_GROUP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_GROUP_NAME,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlChar * ztcmGetDsn()
{
    stlChar  * ret = NULL;
    
    if( stlStrlen( gConfigure.mCurrentGroupPtr->mDsn ) > 0 )
    {
        ret = gConfigure.mCurrentGroupPtr->mDsn;
    }
    else if( stlStrlen( gConfigure.mDsn ) > 0 )
    {
        ret = gConfigure.mDsn;
    }
    
    return ret;
}


stlStatus ztcmSetMasterIp( stlChar       * aMasterIp,
                           stlErrorStack * aErrorStack )
{
    stlInt32   sLen = 0;
    
    sLen = stlStrlen( aMasterIp );
    
    STL_TRY_THROW( sLen < 16, RAMP_ERR_TOO_LONG_IP );
    
    stlStrncpy( gConfigure.mMasterIp, aMasterIp, sLen + 1 );
    
    return STL_SUCCESS;
       
    STL_CATCH( RAMP_ERR_TOO_LONG_IP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "Master IP",
                      0 );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlChar * ztcmGetMasterIp()
{
    stlInt32   sLen = 0;
    stlChar  * ret = NULL;
    
    sLen = stlStrlen( gConfigure.mMasterIp );
    
    if( sLen > 0 )
    {
        ret = gConfigure.mMasterIp;
    }
    
    return ret;
}


void ztcmPrintErrorstack( stlErrorStack * aErrorStack )
{
    stlInt32 i;
    stlInt32 sErrorCode;
    stlInt32 sExtErrCode;
    stlChar  sSqlState[6];

    for( i = aErrorStack->mTop; i >= 0; i-- )
    {
        sErrorCode = aErrorStack->mErrorData[i].mErrorCode;
        sExtErrCode = stlGetExternalErrorCode( &aErrorStack->mErrorData[i] );
        stlMakeSqlState( sExtErrCode, sSqlState );
 
        stlPrintf( "ERR-%s(%d): %s %s\n",
                   sSqlState,
                   sErrorCode,
                   aErrorStack->mErrorData[i].mErrorMessage,
                   aErrorStack->mErrorData[i].mDetailErrorMessage );
        
    }
}

void ztcmWriteLastError( stlErrorStack * aErrorStack )
{
    stlInt32 sCount = aErrorStack->mTop;
    stlInt32 i = 0;
    stlInt32 sErrorCode;
    stlInt32 sExtErrCode;
    stlChar  sSqlState[6];

    for( i = sCount;i >= 0; i-- )
    {
        sErrorCode = aErrorStack->mErrorData[i].mErrorCode;
        sExtErrCode = stlGetExternalErrorCode( &aErrorStack->mErrorData[i] );
        stlMakeSqlState( sExtErrCode, sSqlState );
        
        (void) ztcmLogMsg( aErrorStack,
                           "ERR-%s(%d): %s %s\n",
                           sSqlState,
                           sErrorCode,
                           aErrorStack->mErrorData[i].mErrorMessage,
                           aErrorStack->mErrorData[i].mDetailErrorMessage );
    }
}

stlStatus ztcmWriteTimeStamp( stlFile        * aFile,
                              stlErrorStack  * aErrorStack )
{
    stlExpTime sExpTime;
    stlChar    sBuf[1024];
    
    stlMakeExpTimeByLocalTz(&sExpTime, stlNow() );
    
    stlSnprintf( sBuf,
                 1024,
                 "\n[%04d-%02d-%02d %02d:%02d:%02d.%06d]\n",
                 sExpTime.mYear + 1900,
                 sExpTime.mMonth + 1,
                 sExpTime.mDay,
                 sExpTime.mHour,
                 sExpTime.mMinute,
                 sExpTime.mSecond,
                 sExpTime.mMicroSecond );
    
    STL_TRY( stlPutStringFile( sBuf,
                               aFile,
                               aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztcmLogMsg( stlErrorStack  * aErrorStack,
                      const stlChar  * aFormat,
                      ... )
{
    va_list          sVarArgList;
    stlInt32         sLogSize;
    stlChar        * sLogMsg = NULL;
    stlInt32         sState  = 0;
    
    va_start(sVarArgList, aFormat);
    sLogSize = stlGetVprintfBufferSize( aFormat, sVarArgList );
    va_end(sVarArgList);
    
    STL_TRY( stlAllocHeap( (void**)&sLogMsg,
                           sLogSize + 1,
                           aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    va_start(sVarArgList, aFormat);
    stlVsnprintf( sLogMsg,
                  sLogSize + 1,
                  aFormat,
                  sVarArgList );
    va_end(sVarArgList);
    
    STL_TRY( stlAcquireSemaphore( &(gConfigure.mLogSem),
                                  aErrorStack ) == STL_SUCCESS );
    sState = 2;
    
    STL_TRY( ztcmAddLogMsg( sLogMsg,
                            sLogSize,
                            aErrorStack ) == STL_SUCCESS );
    sState = 1;
    STL_TRY( stlReleaseSemaphore( &(gConfigure.mLogSem),
                                  aErrorStack ) == STL_SUCCESS );
    
    sState = 0;
    stlDebug(sLogMsg);

    stlFreeHeap( sLogMsg );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)stlReleaseSemaphore( &(gConfigure.mLogSem),
                                       aErrorStack ) ;
        case 1:
            (void)stlFreeHeap( sLogMsg );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztcmAddLogMsg( stlChar        * aMsg,
                         stlInt32         aLength,
                         stlErrorStack  * aErrorStack )
{
    stlFile   sFile;
    stlChar   sFileName[STL_MAX_FILE_PATH_LENGTH];
    stlSize   sWrittenBytes;
    stlInt32  sState = 0;
    
    if( gConfigure.mIsMaster == STL_TRUE )
    {
        stlSnprintf( sFileName,
                     STL_MAX_FILE_PATH_LENGTH,
                     "%strc"STL_PATH_SEPARATOR"cyclonem_master_%s.trc",
                     gConfigure.mHomeDir,
                     gConfigure.mCurrentGroupPtr->mName );
    }
    else
    {
        stlSnprintf( sFileName,
                     STL_MAX_FILE_PATH_LENGTH,
                     "%strc"STL_PATH_SEPARATOR"cyclonem_slave_%s.trc",
                     gConfigure.mHomeDir,
                     gConfigure.mCurrentGroupPtr->mName );
    }

    STL_TRY( stlOpenFile( &sFile,
                          sFileName,
                          STL_FOPEN_CREATE |
                          STL_FOPEN_WRITE  |
                          STL_FOPEN_APPEND,
                          STL_FPERM_OS_DEFAULT,
                          aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztcmWriteTimeStamp( &sFile,
                                 aErrorStack ) == STL_SUCCESS );

    STL_TRY( stlWriteFile( &sFile,
                           (void*)aMsg,
                           aLength,
                           &sWrittenBytes,
                           aErrorStack ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( stlCloseFile( &sFile,
                           aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;
    
    switch( sState )
    {
        case 1:
            (void) stlCloseFile( &sFile,
                                 aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus ztcmAddGroupItem( stlChar       * aGroupName,
                            stlErrorStack * aErrorStack )
{
    ztcGroupItem * sGroup        = NULL;
    ztcGroupItem * sCompareGroup = NULL;
    
    if( STL_RING_IS_EMPTY( &gConfigure.mGroupList ) != STL_TRUE )
    {
        STL_RING_FOREACH_ENTRY( &gConfigure.mGroupList, sCompareGroup )
        {
            STL_TRY_THROW( stlStrcasecmp( sCompareGroup->mName, aGroupName ) != 0, RAMP_ERR_DUPLICATE_GROUP );
        }
    }
        
    STL_TRY( stlAllocRegionMem( &gConfigure.mAllocator,
                                STL_SIZEOF( ztcGroupItem ),
                                (void **)&sGroup,
                                aErrorStack ) == STL_SUCCESS );

    STL_RING_INIT_DATALINK( sGroup,
                            STL_OFFSETOF( ztcGroupItem, mLink ) );
    
    stlMemset( sGroup, 0x00, STL_SIZEOF( ztcGroupItem ) );
    
    stlStrncpy( sGroup->mName, 
                aGroupName, 
                stlStrlen( aGroupName ) + 1 );
        
    stlToupperString( sGroup->mName, sGroup->mName );
    gConfigure.mGroupCount++;
    
    STL_RING_ADD_LAST( &gConfigure.mGroupList, 
                       sGroup );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_DUPLICATE_GROUP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_DUPLICATE_GROUP_NAME,
                      NULL,
                      aErrorStack,
                      aGroupName );
        
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztcmSetTransSortAreaSize( stlChar       * aSize,
                                    stlErrorStack * aErrorStack )
{
    stlChar   * sEndPtr;
    stlInt64    sSize = 0;
    
    STL_TRY( stlStrToInt64( aSize,
                            STL_NTS,
                            &sEndPtr,
                            10,
                            &sSize, 
                            aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( *sEndPtr == 0, RAMP_ERR_INVALID_VALUE );
    
    STL_TRY_THROW( sSize >= 50, RAMP_ERR_TOO_LOW_VALUE );
    
    gConfigure.mTransSortAreaSize = sSize * 1024 * 1024;
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "trans_sort_area_size",
                      aSize );
    }
    STL_CATCH( RAMP_ERR_TOO_LOW_VALUE )
    {
         stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "trans_sort_area_size (min:50)",
                      sSize );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlInt64 ztcmGetTransSortAreaSize()
{
    stlInt64  sSize = 0;
    
    sSize = ZTC_TRANSACTION_SORT_AREA_SIZE;
    
    if( gConfigure.mTransSortAreaSize != 0 )
    {
        sSize = gConfigure.mTransSortAreaSize;
    }
    
    return sSize;
}

stlStatus ztcmSetApplierCount( stlChar       * aSize,
                               stlErrorStack * aErrorStack )
{
    stlChar   * sEndPtr;
    stlInt64    sSize = 0;
    
    STL_TRY( stlStrToInt64( aSize,
                            STL_NTS,
                            &sEndPtr,
                            10,
                            &sSize, 
                            aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( *sEndPtr == 0, RAMP_ERR_INVALID_VALUE );
    
    STL_TRY_THROW( sSize >= 1, RAMP_ERR_TOO_LOW_VALUE );
    
    gConfigure.mApplierCount = sSize;
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "applier_count",
                      aSize );
    }
    STL_CATCH( RAMP_ERR_TOO_LOW_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "applier_count (min:1)",
                      sSize );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlInt64 ztcmGetApplierCount( stlErrorStack * aErrorStack )
{
    stlInt64  sSize = 0;

    /**
     * file에 write는 applier를 1개만 사용 한다.
     */
    if( gConfigure.mApplyToFile == STL_TRUE )
    {
        ztcmLogMsg( aErrorStack, "used --file option => set APPLIER_COUNT = 1\n" );

        sSize = 1;
    }
    else
    {
        sSize = ZTC_APPLIER_SESSION_COUNT;

        if( gConfigure.mApplierCount != 0 )
        {
            sSize = gConfigure.mApplierCount;
        }
    }

    return sSize;
}

stlStatus ztcmSetApplyCommitSize( stlChar       * aSize,
                                  stlErrorStack * aErrorStack )
{
    stlChar   * sEndPtr;
    stlInt64    sSize = 0;
    
    STL_TRY( stlStrToInt64( aSize,
                            STL_NTS,
                            &sEndPtr,
                            10,
                            &sSize, 
                            aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( *sEndPtr == 0, RAMP_ERR_INVALID_VALUE );
    
    STL_TRY_THROW( sSize >= 100, RAMP_ERR_TOO_LOW_VALUE );
    
//    ztcmLogMsg( aErrorStack, "test => set mApplierCommitSize = 1\n" );
    sSize = 1;

    gConfigure.mApplierCommitSize = sSize;
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "apply_commit_size",
                      aSize );
    }
    STL_CATCH( RAMP_ERR_TOO_LOW_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "apply_commit_size (min:100)",
                      sSize );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlInt64 ztcmGetApplyCommitSize()
{
    stlInt64  sSize = 0;

    /**
     * file에 write는 CommitSize를 1개만 사용 한다.
     * TODO lym : solace사용시 1 아니면 안됨.
     * ex) tr3보내는중에 master가 죽은 경우 master는 tr3부터 보내는데
     *     slave에서 commitsize가 1이 아니면 tr1, tr2, tr3까지 commit안하고 있다가
     *     tr3 begin다시 받으면 tr1, tr2까지 commit해야 하는데 가능할지 모르겠음.
     */
    if( gConfigure.mApplyToFile == STL_TRUE )
    {
        sSize = 1;
    }
    else
    {
        sSize = ZTC_APPLIER_TRANS_COMMIT_MAX_SIZE;

        if( gConfigure.mApplierCommitSize != 0 )
        {
            sSize = gConfigure.mApplierCommitSize;
        }
    }

    sSize = 0;

    return sSize;
}

stlStatus ztcmSetApplyArraySize( stlChar       * aSize,
                                 stlErrorStack * aErrorStack )
{
    STL_TRY( ztcmLogMsg( aErrorStack,
                         "'APPLY_ARRAY_SIZE' Property deprecated.\n" ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlInt64 ztcmGetApplyArraySize()
{
    stlInt64  sSize = 0;
    
    /**
     * file에 write는 ApplyArraySize를 1로 사용 한다.
     */
    if( gConfigure.mApplyToFile == STL_TRUE )
    {
        sSize = 1;
    }
    else
    {
        sSize = ZTC_APPLIER_ARRAY_SIZE;

        if( gConfigure.mApplierArraySize != 0 )
        {
            sSize = gConfigure.mApplierArraySize;
        }
    }
    
    return sSize;
}


stlStatus ztcmSetReadLogBlockCount( stlChar       * aCount,
                                    stlErrorStack * aErrorStack )
{
    stlChar   * sEndPtr;
    stlInt64    sCount = 0;
    
    STL_TRY( stlStrToInt64( aCount,
                            STL_NTS,
                            &sEndPtr,
                            10,
                            &sCount,
                            aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( *sEndPtr == 0, RAMP_ERR_INVALID_VALUE );
    
    STL_TRY_THROW( sCount >= 100, RAMP_ERR_TOO_LOW_VALUE );
    
    gConfigure.mReadLogBlockCount = sCount;
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "read_log_block_count",
                      aCount );
    }
    STL_CATCH( RAMP_ERR_TOO_LOW_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "read_log_block_count (min:100)",
                      sCount );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlInt64 ztcmGetReadLogBlockCount()
{
    stlInt64  sCount = 0;
    
    sCount = ZTC_READ_BUFFER_BLOCK_COUNT;
    
    if( gConfigure.mReadLogBlockCount != 0 )
    {
        sCount = gConfigure.mReadLogBlockCount;
    }
    
    return sCount;
}

stlStatus ztcmSetCommChunkCount( stlChar       * aCount,
                                 stlErrorStack * aErrorStack )
{
    stlChar   * sEndPtr;
    stlInt64    sCount = 0;
    
    STL_TRY( stlStrToInt64( aCount,
                            STL_NTS,
                            &sEndPtr,
                            10,
                            &sCount, 
                            aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( *sEndPtr == 0, RAMP_ERR_INVALID_VALUE );
    
    STL_TRY_THROW( sCount >= 10, RAMP_ERR_TOO_LOW_VALUE );
    
    gConfigure.mCommChunkCount = (stlInt32) sCount;
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "comm_chunk_count",
                      aCount );
    }
    STL_CATCH( RAMP_ERR_TOO_LOW_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "comm_chunk_count (min:10)",
                      sCount );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}
                                
stlInt32 ztcmGetCommChunkCount()
{
    stlInt32  sCount = 0;
    
    sCount = ZTC_CHUNK_ITEM_COUNT;
    
    if( gConfigure.mCommChunkCount!= 0 )
    {
        sCount = gConfigure.mCommChunkCount;
    }
    
    return sCount;
}

stlStatus ztcmSetPropagateMode( stlChar       * aMode,
                                stlErrorStack * aErrorStack )
{
    stlChar   * sEndPtr;
    stlInt64    sMode = 0;
    
    STL_TRY( stlStrToInt64( aMode,
                            STL_NTS,
                            &sEndPtr,
                            10,
                            &sMode, 
                            aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( *sEndPtr == 0, RAMP_ERR_INVALID_VALUE );
    
    STL_TRY_THROW( sMode == 0 || sMode == 1, RAMP_ERR_INVALID_VALUE );
    
    if( sMode == 1 )
    {
        gConfigure.mPropagateMode = STL_TRUE;
    }
    else
    {
        gConfigure.mPropagateMode = STL_FALSE;
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "propagate_mode (enable:1, disable:0)",
                      aMode );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlBool ztcmGetPropagateMode()
{
    return gConfigure.mPropagateMode;
}

stlStatus ztcmSetUserLogFilePath( stlChar       * aLogPath,
                                  stlErrorStack * aErrorStack )
{
    stlInt32   sLength  = 0;
    stlChar    sLogPath[ STL_MAX_FILE_PATH_LENGTH ];
    
    sLength = stlStrlen( aLogPath );
    
    STL_TRY_THROW( sLength < STL_MAX_FILE_PATH_LENGTH &&
                   sLength > 2, RAMP_ERR_INVALID_LOG_PATH );
    
    /**
     * 앞뒤의 ' 를 제거한다.
     */
    stlMemset( sLogPath, 0x00, STL_MAX_FILE_PATH_LENGTH );
    
    stlMemcpy( sLogPath,
               &aLogPath[1],
               sLength - 2 );
    
    sLength = stlStrlen( sLogPath );
    
    /**
     * Directory가 '/', '\'로 끝날 경우에 제거한다.
     */
    if ( sLogPath[ sLength - 1 ] == '/' ||
         sLogPath[ sLength - 1 ] == '\\' )
    {
        sLength = sLength - 1;
    }
    else
    {
        sLength = sLength;
    }
    
    stlStrncpy( gConfigure.mUserLogFilePath, 
                sLogPath,
                sLength + 1 );
    
    return STL_SUCCESS;   
    
    STL_CATCH( RAMP_ERR_INVALID_LOG_PATH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "LOG_PATH",
                      aLogPath );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlChar * ztcmGetUserLogFilePath()
{
    stlInt32   sLen = 0;
    stlChar  * ret = NULL;
    
    sLen = stlStrlen( gConfigure.mUserLogFilePath );
    
    if( sLen > 0 )
    {
        ret = gConfigure.mUserLogFilePath;
    }
    
    return ret;
}

stlBool ztcmGetUserLogFilePathStatus()
{
    stlBool   sRet = STL_FALSE;
    stlInt32  sLen = 0;
    
    sLen = stlStrlen( gConfigure.mUserLogFilePath );
    
    if( sLen > 0 )
    {
        sRet = STL_TRUE;
    }
    
    return sRet;
}

stlStatus ztcmSetTransFilePath( stlChar       * aPath,
                                stlErrorStack * aErrorStack )
{
    stlInt32   sLength  = 0;
    stlChar    sPath[ STL_MAX_FILE_PATH_LENGTH ];
    
    sLength = stlStrlen( aPath );
    
    STL_TRY_THROW( sLength < STL_MAX_FILE_PATH_LENGTH &&
                   sLength > 2, RAMP_ERR_INVALID_LOG_PATH );
    
    /**
     * 앞뒤의 ' 를 제거한다.
     */
    stlMemset( sPath, 0x00, STL_MAX_FILE_PATH_LENGTH );
    
    stlMemcpy( sPath,
               &aPath[1],
               sLength - 2 );
    
    sLength = stlStrlen( sPath );
    
    /**
     * Directory가 '/', '\'로 끝날 경우에 제거한다.
     */
    if ( sPath[ sLength - 1 ] == '/' ||
         sPath[ sLength - 1 ] == '\\' )
    {
        sLength = sLength - 1;
    }
    else
    {
        sLength = sLength;
    }
    
    stlStrncpy( gConfigure.mTransFilePath, 
                sPath,
                sLength + 1 );
    
    return STL_SUCCESS;   
    
    STL_CATCH( RAMP_ERR_INVALID_LOG_PATH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "TRANS_FILE_PATH",
                      aPath );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlChar * ztcmGetTransFilePath()
{
    stlInt32   sLen = 0;
    stlChar  * ret = NULL;
    
    sLen = stlStrlen( gConfigure.mTransFilePath );
    
    if( sLen > 0 )
    {
        ret = gConfigure.mTransFilePath;
    }
    
    return ret;
}


stlStatus ztcmSetGiveupInterval( stlChar       * aInterval,
                                 stlErrorStack * aErrorStack )
{
    stlChar   * sEndPtr;
    stlInt64    sInterval = 0;
    
    STL_TRY( stlStrToInt64( aInterval,
                            STL_NTS,
                            &sEndPtr,
                            10,
                            &sInterval,
                            aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( *sEndPtr == 0, RAMP_ERR_INVALID_VALUE );
    
    STL_TRY_THROW( sInterval > 0, RAMP_ERR_TOO_LOW_VALUE );
    
    gConfigure.mGiveupInterval = sInterval;
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "giveup_interval",
                      aInterval );
    }
    STL_CATCH( RAMP_ERR_TOO_LOW_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "giveup_interval (min:0)",
                      sInterval );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlInt64  ztcmGetGiveupInterval()
{
    stlInt64 sRet = 0;
    
    if( gConfigure.mGiveupInterval > 0 )
    {
        sRet = gConfigure.mGiveupInterval;
    }
    
    return sRet;
}


stlInt64  ztcmGetTransTableSize()
{
    stlInt64 sRet = ZTC_TRANSACTION_TABLE_SIZE;
    
    if( gConfigure.mTransTableSize > 0 )
    {
        sRet = gConfigure.mTransTableSize;
    }
    
    return sRet;
}


void ztcmInitSpinLock( ztcSpinLock * aSpinLock )
{
    ZTC_INIT_SPIN_LOCK( *aSpinLock );
}

void ztcmFinSpinLock( ztcSpinLock * aSpinLock )
{
    STL_DASSERT( *aSpinLock == ZTC_SPINLOCK_STATUS_INIT );
}


void ztcmAcquireSpinLock( ztcSpinLock * aSpinLock )
{
    stlUInt32  sRetVal;
    stlUInt32  sMaxRetryCnt = 1000;
    stlUInt32  sSpinCnt     = 0;

    sSpinCnt = sMaxRetryCnt;
    do
    {
        sRetVal = stlAtomicCas32( aSpinLock,
                                  ZTC_SPINLOCK_STATUS_LOCKED,
                                  ZTC_SPINLOCK_STATUS_INIT );

        if( sRetVal == ZTC_SPINLOCK_STATUS_INIT )
        {
            break;
        }
        
        sSpinCnt--;
        
        if( sSpinCnt == 0 )
        {
            stlBusyWait();
            sSpinCnt = sMaxRetryCnt;
        }
    } while( STL_TRUE );
}

void ztcmTrySpinLock( ztcSpinLock * aSpinLock,
                      stlBool     * aIsSuccess )
{
    stlUInt32  sRetVal;

    *aIsSuccess = STL_TRUE;

    sRetVal = stlAtomicCas32( aSpinLock,
                              ZTC_SPINLOCK_STATUS_LOCKED,
                              ZTC_SPINLOCK_STATUS_INIT );

    if(sRetVal != ZTC_SPINLOCK_STATUS_INIT)
    {
        *aIsSuccess = STL_FALSE;
    }
}

void ztcmReleaseSpinLock( ztcSpinLock * aSpinLock )
{
    stlUInt32 sRetVal;

    sRetVal = stlAtomicCas32( aSpinLock,
                              ZTC_SPINLOCK_STATUS_INIT,
                              ZTC_SPINLOCK_STATUS_LOCKED );

    STL_DASSERT( sRetVal == ZTC_SPINLOCK_STATUS_LOCKED );

    /**
     * release에서도 compile warning 안나도록
     */
    if( sRetVal != ZTC_SPINLOCK_STATUS_LOCKED )
    {
    }
}



dtlCharacterSet ztcmGetCharSetIDFunc( void * aArgs )
{
    STL_DASSERT( gSlaveMgr != NULL );

    return gSlaveMgr->mCharacterSet;
}

dtlCharacterSet ztcmGetSyncCharSetIDFunc( void * aArgs )
{
    STL_DASSERT( gSyncMgr != NULL );
    
    return gSyncMgr->mCharacterSet;
}

stlInt32 ztcmGetGMTOffsetFunc( void * aArgs )
{
    STL_DASSERT( gSlaveMgr != NULL );
    
    return gSlaveMgr->mTimezone;
}

stlInt32 ztcmGetSyncGMTOffsetFunc( void * aArgs )
{
    STL_DASSERT( gSyncMgr != NULL );
    
    return gSyncMgr->mTimezone;
}

stlStatus ztcmReallocLongVaryingMemFunc( void           * aArgs,
                                         stlInt32         aAllocSize,
                                         void          ** aAddr,
                                         stlErrorStack  * aErrorStack )
{
    STL_DASSERT( STL_FALSE );

    return STL_SUCCESS;
}

          
stlStatus ztcmReallocAndMoveLongVaryingMemFunc( void           * aArgs,
                                                dtlDataValue   * aDataValue,
                                                stlInt32         aAllocSize,
                                                stlErrorStack  * aErrorStack )
{
    STL_DASSERT( STL_FALSE );

    return STL_SUCCESS;
}

          
static stlChar * ztcmGetDateFormatStringFunc( void * aArgs )
{
    return DTL_DATE_FORMAT_FOR_ODBC;
}

dtlDateTimeFormatInfo * ztcmGetDateFormatInfoFunc( void * aArgs )
{
    return gDtlNLSDateFormatInfoForODBC;    
}

static stlChar * ztcmGetTimeFormatStringFunc( void * aArgs )
{
    return DTL_TIME_FORMAT_FOR_ODBC;    
}

dtlDateTimeFormatInfo * ztcmGetTimeFormatInfoFunc( void * aArgs )
{
    return gDtlNLSTimeFormatInfoForODBC;
}

static stlChar * ztcmGetTimeWithTimeZoneFormatStringFunc( void * aArgs )
{
    return DTL_TIME_WITH_TIME_ZONE_FORMAT_FOR_ODBC;    
}

dtlDateTimeFormatInfo * ztcmGetTimeWithTimeZoneFormatInfoFunc( void * aArgs )
{
    return gDtlNLSTimeWithTimeZoneFormatInfoForODBC;    
}

static stlChar * ztcmGetTimestampFormatStringFunc( void * aArgs )
{
    return DTL_TIMESTAMP_FORMAT_FOR_ODBC;    
}

dtlDateTimeFormatInfo * ztcmGetTimestampFormatInfoFunc( void * aArgs )
{
    return gDtlNLSTimestampFormatInfoForODBC;    
}

static stlChar * ztcmGetTimestampWithTimeZoneFormatStringFunc( void * aArgs )
{
    return DTL_TIMESTAMP_WITH_TIME_ZONE_FORMAT_FOR_ODBC;    
}

dtlDateTimeFormatInfo * ztcmGetTimestampWithTimeZoneFormatInfoFunc( void * aArgs )
{
    return gDtlNLSTimestampWithTimeZoneFormatInfoForODBC;    
}

stlStatus ztcmSetDTFuncVector()
{   
    dtlFuncVector * sVector = &gSlaveMgr->mDTVector;

    sVector->mGetCharSetIDFunc  = ztcmGetCharSetIDFunc;
    sVector->mGetGMTOffsetFunc  = ztcmGetGMTOffsetFunc;
    sVector->mReallocLongVaryingMemFunc = ztcmReallocLongVaryingMemFunc;
    sVector->mReallocAndMoveLongVaryingMemFunc = ztcmReallocAndMoveLongVaryingMemFunc;
    
    sVector->mGetDateFormatStringFunc = ztcmGetDateFormatStringFunc;
    sVector->mGetDateFormatInfoFunc = ztcmGetDateFormatInfoFunc;
    
    sVector->mGetTimeFormatStringFunc = ztcmGetTimeFormatStringFunc;
    sVector->mGetTimeFormatInfoFunc = ztcmGetTimeFormatInfoFunc;

    sVector->mGetTimeWithTimeZoneFormatStringFunc = ztcmGetTimeWithTimeZoneFormatStringFunc;
    sVector->mGetTimeWithTimeZoneFormatInfoFunc = ztcmGetTimeWithTimeZoneFormatInfoFunc;

    sVector->mGetTimestampFormatStringFunc = ztcmGetTimestampFormatStringFunc;
    sVector->mGetTimestampFormatInfoFunc = ztcmGetTimestampFormatInfoFunc;

    sVector->mGetTimestampWithTimeZoneFormatStringFunc = ztcmGetTimestampWithTimeZoneFormatStringFunc;
    sVector->mGetTimestampWithTimeZoneFormatInfoFunc = ztcmGetTimestampWithTimeZoneFormatInfoFunc;

    return STL_SUCCESS;
}

stlStatus ztcmSetSyncDTFuncVector( dtlFuncVector * aVectorFunc )
{   
    aVectorFunc->mGetCharSetIDFunc  = ztcmGetSyncCharSetIDFunc;
    aVectorFunc->mGetGMTOffsetFunc  = ztcmGetSyncGMTOffsetFunc;
    aVectorFunc->mReallocLongVaryingMemFunc = ztcmReallocLongVaryingMemFunc;
    aVectorFunc->mReallocAndMoveLongVaryingMemFunc = ztcmReallocAndMoveLongVaryingMemFunc;
    
    aVectorFunc->mGetDateFormatStringFunc = ztcmGetDateFormatStringFunc;
    aVectorFunc->mGetDateFormatInfoFunc = ztcmGetDateFormatInfoFunc;
    
    aVectorFunc->mGetTimeFormatStringFunc = ztcmGetTimeFormatStringFunc;
    aVectorFunc->mGetTimeFormatInfoFunc = ztcmGetTimeFormatInfoFunc;

    aVectorFunc->mGetTimeWithTimeZoneFormatStringFunc = ztcmGetTimeWithTimeZoneFormatStringFunc;
    aVectorFunc->mGetTimeWithTimeZoneFormatInfoFunc = ztcmGetTimeWithTimeZoneFormatInfoFunc;

    aVectorFunc->mGetTimestampFormatStringFunc = ztcmGetTimestampFormatStringFunc;
    aVectorFunc->mGetTimestampFormatInfoFunc = ztcmGetTimestampFormatInfoFunc;

    aVectorFunc->mGetTimestampWithTimeZoneFormatStringFunc = ztcmGetTimestampWithTimeZoneFormatStringFunc;
    aVectorFunc->mGetTimestampWithTimeZoneFormatInfoFunc = ztcmGetTimestampWithTimeZoneFormatInfoFunc;

    return STL_SUCCESS;
}

stlStatus ztcmGetTransFileName( stlInt32        aIdx,
                                stlChar       * aName,
                                stlErrorStack * aErrorStack )
{
    stlChar * sPath = ztcmGetTransFilePath();
    
    if( sPath == NULL )
    {
        stlSnprintf( aName, 
                     STL_MAX_FILE_PATH_LENGTH, 
                     STL_PATH_SEPARATOR"tmp"STL_PATH_SEPARATOR"trans_%s_%d.tmp", 
                     gConfigure.mCurrentGroupPtr->mName, aIdx );
    }
    else
    {
        stlSnprintf( aName, 
                     STL_MAX_FILE_PATH_LENGTH, 
                     "%s"STL_PATH_SEPARATOR"trans_%s_%d.tmp", 
                     sPath, gConfigure.mCurrentGroupPtr->mName, aIdx );
    }
    
    return STL_SUCCESS;
}

stlBool ztcmGetArchiveLogMode()
{
    return gConfigure.mArchiveMode;
}

stlStatus ztcmGetArchiveLogFile( stlChar       * aFileName,
                                 stlInt64        aFileSeqNo,
                                 stlErrorStack * aErrorStack )
{
    stlSnprintf( aFileName, 
                 STL_MAX_FILE_PATH_LENGTH, 
                 "%s"STL_PATH_SEPARATOR"%s_%ld.log", 
                 gConfigure.mArchivePath, gConfigure.mArchiveFilePrefix, aFileSeqNo );
    
    return STL_SUCCESS;
}


stlStatus ztcmReadMasterTableMeta( stlErrorStack * aErrorStack )
{
    stlFile     sFile;
    stlChar     sFileName[STL_MAX_FILE_PATH_LENGTH];
    stlInt32    sState       = 0;
    stlInt32    sIdx         = 0;
    stlBool     sExist       = STL_FALSE;
    stlSize     sReadSize    = 0;
    stlOffset   sFileSize    = 0;
    stlOffset   sFileStart   = 0;
    stlInt64    sTotReadSize = 0;
    
    ztcMasterTableMeta  * sTableMeta  = NULL;
    ztcColumnInfo       * sColumnInfo = NULL;
    
    STL_TRY( ztcmGetTableMetaFileName( sFileName ) == STL_SUCCESS );
    
    STL_TRY( stlExistFile( sFileName,
                           &sExist,
                           aErrorStack ) == STL_SUCCESS );
    
    if( sExist == STL_TRUE )
    {
        STL_TRY( stlOpenFile( &sFile,
                              sFileName,
                              STL_FOPEN_CREATE | STL_FOPEN_READ | STL_FOPEN_BINARY,
                              STL_FPERM_OS_DEFAULT,
                              aErrorStack ) == STL_SUCCESS );
        sState = 1;
        
        STL_TRY( stlSeekFile( &sFile,
                              STL_FSEEK_END,
                              &sFileSize,
                              aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( stlSeekFile( &sFile,
                              STL_FSEEK_SET,
                              &sFileStart,
                              aErrorStack ) == STL_SUCCESS );
        
        while( 1 )
        {
            STL_TRY( gRunState == STL_TRUE );
            /**
             * Protocol에 사용될 Table 정보 생성
             */
            STL_TRY( stlAllocRegionMem( &(gMasterMgr->mRegionMem),
                                        STL_SIZEOF( ztcMasterTableMeta ),
                                        (void**)&sTableMeta,
                                        aErrorStack ) == STL_SUCCESS );
            
            stlMemset( sTableMeta, 0x00, STL_SIZEOF( ztcMasterTableMeta ) );
            
            STL_RING_INIT_DATALINK( sTableMeta,
                                    STL_OFFSETOF( ztcMasterTableMeta, mLink ) );
            
            STL_RING_INIT_HEADLINK( &(sTableMeta->mColumnMetaList),
                                    STL_OFFSETOF( ztcColumnInfo, mLink ) );
            
            STL_TRY( stlReadFile( &sFile,
                                  &sTableMeta->mSchemaName,
                                  STL_SIZEOF(ztcMasterTableMeta) - STL_SIZEOF(stlRingEntry) - STL_SIZEOF(stlRingHead),
                                  &sReadSize,
                                  aErrorStack ) == STL_SUCCESS );
            
            STL_DASSERT( sReadSize == STL_SIZEOF(ztcMasterTableMeta) - STL_SIZEOF(stlRingEntry) - STL_SIZEOF(stlRingHead) );
            
            sTotReadSize += sReadSize;
            
            /**
             * MaxTableId를 조정한다.
             */
            if( sTableMeta->mTableId > gMasterMgr->mMaxTableId )
            {
                gMasterMgr->mMaxTableId = sTableMeta->mTableId;
            }
            /*
            stlPrintf("================================================\n");
            stlPrintf(" SchemaName  : [%s]\n", sTableMeta->mSchemaName );
            stlPrintf(" TableName   : [%s]\n", sTableMeta->mTableName );
            stlPrintf(" FileOffset  : [%ld]\n", sTableMeta->mFileOffset );
            stlPrintf(" PhysicalId  : [%ld]\n", sTableMeta->mPhysicalId );
            stlPrintf(" DdlLSN      : [%ld]\n", sTableMeta->mDdlLSN );
            stlPrintf(" TableId     : [%d]\n", sTableMeta->mTableId );
            stlPrintf(" ColumnCnt   : [%d]\n", sTableMeta->mColumnCount );
            */
            STL_RING_ADD_LAST( &(gMasterMgr->mTableMetaList),
                               sTableMeta );
            
            /**
             * Protocol에 사용될 Column정보 생성
             */
            for( sIdx = 0; sIdx < sTableMeta->mColumnCount; sIdx++ )
            {
                STL_TRY( stlAllocRegionMem( &(gMasterMgr->mRegionMem),
                                            STL_SIZEOF( ztcColumnInfo ),
                                            (void**)&sColumnInfo,
                                            aErrorStack ) == STL_SUCCESS );
                
                STL_RING_INIT_DATALINK( sColumnInfo,
                                        STL_OFFSETOF( ztcColumnInfo, mLink ) );
                
                STL_TRY( stlReadFile( &sFile,
                                      &(sColumnInfo->mColumnId),
                                      STL_SIZEOF(ztcColumnInfo) - STL_SIZEOF(stlRingEntry),
                                      &sReadSize,
                                      aErrorStack ) == STL_SUCCESS );
                
                STL_DASSERT( sReadSize == STL_SIZEOF(ztcColumnInfo) - STL_SIZEOF(stlRingEntry) );
                
                sTotReadSize += sReadSize;
                /*
                stlPrintf("---------------- ColumnInfo[%d] ---\n", sIdx + 1);
                stlPrintf("---- ColumnName : [%s]\n", sColumnMeta->mColumnName );
                stlPrintf("---- DataType   : [%s]\n", sColumnMeta->mDataType );
                stlPrintf("---- PrimaryKey : [%d]\n", sColumnMeta->mIsPrimary );
                stlPrintf("---- Unique     : [%d]\n", sColumnMeta->mIsUnique );
                stlPrintf("---- Nullable   : [%d]\n", sColumnMeta->mIsNullable );
                */
                STL_RING_ADD_LAST( &(sTableMeta->mColumnMetaList),
                                   sColumnInfo );
            }
            
            if( sTotReadSize == sFileSize )
            {
                break;
            }
        }
        
        sState = 0;
        STL_TRY( stlCloseFile( &sFile,
                               aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztcmLogMsg( aErrorStack,
                             "Read Master TableMeta File[%s].\n",
                             sFileName ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    switch( sState )
    {
        case 1:
            (void) stlCloseFile( &sFile,
                                 aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}

stlStatus ztcmGetTableMetaFileName( stlChar * aFileName )
{
    stlSnprintf( aFileName, 
                 STL_MAX_FILE_PATH_LENGTH, 
                 "%sconf"STL_PATH_SEPARATOR"%s.%s", 
                 gConfigure.mHomeDir, 
                 ZTC_DEFAULT_MASTER_TABLE_META_FILE, 
                 gConfigure.mCurrentGroupPtr->mName );
    
    return STL_SUCCESS;
}


stlStatus ztcmGetMetaFileName( stlChar * aFileName )
{
    stlSnprintf( aFileName, 
                 STL_MAX_FILE_PATH_LENGTH, 
                 "%sconf"STL_PATH_SEPARATOR"%s.%s", 
                 gConfigure.mHomeDir, 
                 ZTC_DEFAULT_MASTER_META_FILE, 
                 gConfigure.mCurrentGroupPtr->mName );
    
    return STL_SUCCESS;
}



stlStatus ztcmWriteMasterTableMeta( ztcMasterTableMeta * aTableMeta,
                                    stlErrorStack      * aErrorStack )
{
    stlFile   sFile;
    stlChar   sFileName[STL_MAX_FILE_PATH_LENGTH];
    stlInt32  sState     = 0;
    stlInt32  sIdx       = 0;
    stlSize   sWriteSize = 0;
    stlOffset sOffset    = 0;
    
    ztcColumnInfo * sColumnInfo = NULL;
    
    STL_TRY( ztcmGetTableMetaFileName( sFileName ) == STL_SUCCESS );
    
    STL_TRY( stlOpenFile( &sFile,
                          sFileName,
                          STL_FOPEN_CREATE | STL_FOPEN_WRITE | STL_FOPEN_APPEND | STL_FOPEN_BINARY,
                          STL_FPERM_OS_DEFAULT,
                          aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_END,
                          &sOffset,
                          aErrorStack ) == STL_SUCCESS );
    
    /**
     * TableMeta의 FileOffset을 세팅한다.
     */
    aTableMeta->mFileOffset = sOffset;
    
    STL_TRY( stlWriteFile( &sFile,
                           &aTableMeta->mSchemaName,
                           STL_SIZEOF(ztcMasterTableMeta) - STL_SIZEOF(stlRingEntry) - STL_SIZEOF(stlRingHead),
                           &sWriteSize,
                           aErrorStack ) == STL_SUCCESS );
    sState = 2;
    
    STL_RING_FOREACH_ENTRY( &aTableMeta->mColumnMetaList, sColumnInfo )
    {
        STL_TRY( stlWriteFile( &sFile,
                               &sColumnInfo->mColumnId,
                               STL_SIZEOF(ztcColumnInfo) - STL_SIZEOF(stlRingEntry),
                               &sWriteSize,
                               aErrorStack ) == STL_SUCCESS );
        sIdx++;
    }
    
    STL_TRY( aTableMeta->mColumnCount == sIdx );
    
    sState = 0;
    STL_TRY( stlCloseFile( &sFile,
                           aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    switch( sState )
    {
        case 2:
            /**
             * 부분적으로 씌여진 파일을 복구한다.
             */
            (void) stlTruncateFile( &sFile,
                                    sOffset,
                                    aErrorStack );
            
        case 1:
            (void) stlCloseFile( &sFile,
                                 aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}

stlStatus ztcmFindTableMeta( ztcMasterTableMeta ** aTableMeta,
                             stlChar             * aSchemaName,
                             stlChar             * aTableName,
                             stlErrorStack       * aErrorStack )
{
    ztcMasterTableMeta  * sTableMeta  = NULL;
    ztcMasterTableMeta  * sMatchMeta  = NULL;
    
    STL_RING_FOREACH_ENTRY( &gMasterMgr->mTableMetaList, sTableMeta )
    {
        if( stlStrcmp( sTableMeta->mSchemaName, aSchemaName ) == 0 )
        {
            if( stlStrcmp( sTableMeta->mTableName, aTableName ) == 0 )
            {
                sMatchMeta = sTableMeta;
                break;
            }
        }
    }
    
    *aTableMeta = sMatchMeta;
    
    return STL_SUCCESS;
}

stlStatus ztcmBuildMasterMeta( stlErrorStack * aErrorStack )
{
    stlChar     sFileName[STL_MAX_FILE_PATH_LENGTH];
    stlBool     sExist = STL_FALSE;
    
    /**
     * Table Meta File 확인
     */
    STL_TRY( ztcmGetTableMetaFileName( sFileName ) == STL_SUCCESS );
    
    STL_TRY( stlExistFile( sFileName,
                           &sExist,
                           aErrorStack ) == STL_SUCCESS );
    
    if( sExist == STL_FALSE )
    {
        STL_TRY_THROW( gMasterMgr->mDbConnected == STL_TRUE, RAMP_ERR_DB_CONNECT_AT_FIRST );
    }
    else
    {
        /**
         * Table Meta 구성
         */
        STL_TRY( ztcmReadMasterTableMeta( aErrorStack ) == STL_SUCCESS );
    }
    
    /**
     * Meta File 확인
     */
    STL_TRY( ztcmGetMetaFileName( sFileName ) == STL_SUCCESS );
    
    STL_TRY( stlExistFile( sFileName,
                           &sExist,
                           aErrorStack ) == STL_SUCCESS );
    
    if( sExist == STL_FALSE )
    {
        STL_TRY_THROW( gMasterMgr->mDbConnected == STL_TRUE, RAMP_ERR_DB_CONNECT_AT_FIRST );
    }
    
    /**
     * Meta 구성
     */
    if( gMasterMgr->mDbConnected == STL_TRUE )
    {
        /**
         * Capture를 수행할 Redo Log File 정보 세팅
         */
        STL_RING_INIT_HEADLINK( &gMasterMgr->mLogFileList,
                                STL_OFFSETOF( ztcLogFileInfo, mLink ) );
    
        STL_TRY( ztcdGetArchiveLogInfo( gMasterMgr->mDbcHandle,
                                        gConfigure.mArchivePath,
                                        gConfigure.mArchiveFilePrefix,
                                        &gConfigure.mArchiveMode,
                                        aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztcdBuildLogFileInfo( gMasterMgr->mDbcHandle, 
                                       aErrorStack ) == STL_SUCCESS );
        
        /**
         * 최신정보로 Meta를 Update한다.
         */
        STL_TRY( ztcmWriteMasterMeta( aErrorStack ) == STL_SUCCESS );    
    }
    else
    {
       /**
        * Meta file에서 읽어옴
        */
       STL_TRY( ztcmReadMasterMeta( aErrorStack ) == STL_SUCCESS );
    }

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_DB_CONNECT_AT_FIRST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_MUST_CONNECT_DB,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcmWriteMasterMeta( stlErrorStack * aErrorStack )
{
    stlFile          sFile;
    stlChar          sFileName[STL_MAX_FILE_PATH_LENGTH];
    stlInt32         sState        = 0;
    stlInt32         sLogFileCount = 0;
    ztcLogFileInfo * sLogFileInfo  = NULL;
    
    STL_TRY( ztcmGetMetaFileName( sFileName ) == STL_SUCCESS );
    
    STL_TRY( stlOpenFile( &sFile,
                          sFileName,
                          STL_FOPEN_CREATE | STL_FOPEN_WRITE | STL_FOPEN_BINARY,
                          STL_FPERM_OS_DEFAULT,
                          aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    /**
     * 1. Redo Log Count : 4Bytes 
     */
    sLogFileCount = 0;
    STL_RING_FOREACH_ENTRY( &gMasterMgr->mLogFileList, sLogFileInfo )
    {
        sLogFileCount++;   
    }
    
    STL_TRY( stlWriteFile( &sFile,
                           &sLogFileCount,
                           STL_SIZEOF( stlInt32 ),
                           NULL,
                           aErrorStack ) == STL_SUCCESS );
    
    /**
     * 2. Redo Log Info (FileName, GroupId): 4Bytes 
     */
    STL_RING_FOREACH_ENTRY( &gMasterMgr->mLogFileList, sLogFileInfo )
    {
        STL_TRY( stlWriteFile( &sFile,
                               sLogFileInfo->mName,
                               STL_MAX_FILE_PATH_LENGTH,
                               NULL,
                               aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( stlWriteFile( &sFile,
                               &sLogFileInfo->mCaptureInfo.mRedoLogGroupId,
                               STL_SIZEOF( stlInt32 ),
                               NULL,
                               aErrorStack ) == STL_SUCCESS );
    }
    
    if( gConfigure.mArchiveMode == STL_TRUE )
    {
        /**
         * 3. Archive Log file path
         */
        STL_TRY( stlWriteFile( &sFile,
                               gConfigure.mArchivePath,
                               STL_MAX_FILE_PATH_LENGTH,
                               NULL,
                               aErrorStack ) == STL_SUCCESS );
        
        /**
         * 4. Archive Log Prefix
         */ 
        STL_TRY( stlWriteFile( &sFile,
                               gConfigure.mArchiveFilePrefix,
                               STL_MAX_FILE_NAME_LENGTH,
                               NULL,
                               aErrorStack ) == STL_SUCCESS );
    }
    
    sState = 0;
    STL_TRY( stlCloseFile( &sFile,
                           aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    switch( sState )
    {
        case 1:
            (void) stlCloseFile( &sFile,
                                 aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}

stlStatus ztcmReadMasterMeta( stlErrorStack * aErrorStack )
{
    stlFile       sFile;
    stlChar       sFileName[STL_MAX_FILE_PATH_LENGTH];
    stlChar       sLogFileName[STL_MAX_FILE_PATH_LENGTH];
    stlInt32      sGroupId      = 0;
    stlInt32      sState        = 0;
    stlInt32      sLogFileCount = 0;
    stlInt32      sIdx          = 0;
    
    STL_TRY( ztcmGetMetaFileName( sFileName ) == STL_SUCCESS );
    
    STL_TRY( stlOpenFile( &sFile,
                          sFileName,
                          STL_FOPEN_CREATE | STL_FOPEN_READ | STL_FOPEN_BINARY,
                          STL_FPERM_OS_DEFAULT,
                          aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    /**
     * 1. Redo Log Count : 4Bytes 
     */
    STL_TRY( stlReadFile( &sFile,
                          &sLogFileCount,
                          STL_SIZEOF( stlInt32 ),
                          NULL,
                          aErrorStack ) == STL_SUCCESS );
    
    /**
     * 2. Redo Log Info (FileName, GroupId): 4Bytes 
     */
    for( sIdx = 0; sIdx < sLogFileCount; sIdx++ )
    {
        STL_TRY( stlReadFile( &sFile,
                              sLogFileName,
                              STL_MAX_FILE_PATH_LENGTH,
                              NULL,
                              aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( stlReadFile( &sFile,
                              &sGroupId,
                              STL_SIZEOF( stlInt32 ),
                              NULL,
                              aErrorStack ) == STL_SUCCESS );
        
        /**
         * DB가 연결되어 있지 않은 상태이므로 FileSize는 0으로 세팅한다.
         * 어차피 사용되지 않을테니깐....
         */
        STL_TRY( ztcmAddLogFileInfo( sLogFileName, 
                                     sGroupId,
                                     0,
                                     aErrorStack ) == STL_SUCCESS );
        /*
        stlPrintf("----------------------------------\n");
        stlPrintf("[%d][%s]\n", sGroupId, sFileName );
        */
    }
    
    /**
     * 3. Archive Log file path
     */
    if( stlReadFile( &sFile,
                     gConfigure.mArchivePath,
                     STL_MAX_FILE_PATH_LENGTH,
                     NULL,
                     aErrorStack ) != STL_SUCCESS )
    {
        STL_TRY_THROW( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_EOF, RAMP_ERR_INVLIAD_META_FILE );
        
        (void)stlPopError( aErrorStack );
        
        gConfigure.mArchiveMode = STL_FALSE;
        
        STL_THROW( RAMP_SUCCESS );
    }
    /*
    stlPrintf("----------------------------------\n");
    stlPrintf("ARCHIVEPATH[%s]\n", gConfigure.mArchivePath );
    */
    gConfigure.mArchiveMode = STL_TRUE;
    
    /**
     * 4. Archive Log Prefix
     */ 
    STL_TRY_THROW( stlReadFile( &sFile,
                                gConfigure.mArchiveFilePrefix,
                                STL_MAX_FILE_NAME_LENGTH,
                                NULL,
                                aErrorStack ) == STL_SUCCESS, RAMP_ERR_INVLIAD_META_FILE );
    /*
    stlPrintf("----------------------------------\n");
    stlPrintf("ARCHIVEPREFIX[%s]\n", gConfigure.mArchiveFilePrefix );
    */
    STL_RAMP( RAMP_SUCCESS );
    
    STL_TRY( ztcmLogMsg( aErrorStack,
                         "Could not establish connection to Database. so, Read Master Meta File[%s].\n",
                         sFileName ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( stlCloseFile( &sFile,
                           aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVLIAD_META_FILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_META_FILE,
                      NULL,
                      aErrorStack,
                      sFileName );
    }
    STL_FINISH;
    
    switch( sState )
    {
        case 1:
            (void) stlCloseFile( &sFile,
                                 aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}

stlBool ztcmGetDbConnected()
{
    stlBool sConn = STL_FALSE;
    
    if( gMasterMgr->mDbConnected == STL_TRUE )
    {
        sConn = STL_TRUE;   
    }
    
    return sConn;
}

stlStatus ztcmRemoveMasterMetaFile( stlErrorStack * aErrorStack )
{
    stlChar sFileName[STL_MAX_FILE_PATH_LENGTH];
    stlBool sExist = STL_FALSE;
    
    STL_TRY( ztcmGetTableMetaFileName( sFileName ) == STL_SUCCESS );
    
    STL_TRY( stlExistFile( sFileName,
                           &sExist,
                           aErrorStack ) == STL_SUCCESS );
    
    if( sExist == STL_TRUE )
    {
        STL_TRY( stlRemoveFile( sFileName, 
                                aErrorStack ) == STL_SUCCESS );
    }
    
    STL_TRY( ztcmGetMetaFileName( sFileName ) == STL_SUCCESS );
    
    STL_TRY( stlExistFile( sFileName,
                           &sExist,
                           aErrorStack ) == STL_SUCCESS );
    
    if( sExist == STL_TRUE )
    {
        STL_TRY( stlRemoveFile( sFileName, 
                                aErrorStack ) == STL_SUCCESS );
    }
        
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztcmUpdateMasterTableMeta( ztcMasterTableMeta  * aTableMeta,
                                     stlErrorStack       * aErrorStack )
{
    stlFile     sFile;
    stlChar     sFileName[STL_MAX_FILE_PATH_LENGTH];
    stlBool     sExist     = STL_FALSE;
    stlInt32    sState     = 0;
    stlOffset   sOffset    = 0;
    stlSize     sWriteSize = 0;
    
    STL_TRY( ztcmGetTableMetaFileName( sFileName ) == STL_SUCCESS );
    
    STL_TRY( stlExistFile( sFileName,
                           &sExist,
                           aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( sExist == STL_TRUE, RAMP_ERR_FILE_NOT_EXIST );
    
    STL_TRY( stlOpenFile( &sFile,
                          sFileName,
                          STL_FOPEN_CREATE | STL_FOPEN_WRITE | STL_FOPEN_BINARY,
                          STL_FPERM_OS_DEFAULT,
                          aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    sOffset = aTableMeta->mFileOffset;
    
    STL_TRY( stlSeekFile( &sFile,
                          SEEK_SET,
                          &sOffset,
                          aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlWriteFile( &sFile,
                           &aTableMeta->mSchemaName,
                           STL_SIZEOF(ztcMasterTableMeta) - STL_SIZEOF(stlRingEntry) - STL_SIZEOF(stlRingHead),
                           &sWriteSize,
                           aErrorStack ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( stlCloseFile( &sFile,
                           aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_FILE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_FILE_NAME,
                      NULL,
                      aErrorStack,
                      sFileName );
    }
    STL_FINISH;
    
    switch( sState )
    {
        case 1:
            (void) stlCloseFile( &sFile,
                                 aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;   
}


stlStatus ztcmSetLogFileInfoUsingLSN( stlInt64        aBeginLsn,
                                      stlErrorStack * aErrorStack )
{
    stlInt16         sGroupId       = 0;
    stlInt32         sBlockSeq      = 0;
    stlInt64         sFileSeqNo     = ZTC_INVALID_FILE_SEQ_NO;
    stlInt16         sRetGroupId    = 0;
    stlInt64         sRetFileSeqNo  = ZTC_INVALID_FILE_SEQ_NO;
    stlInt16         sWrapNo        = ZTC_INVALID_WRAP_NO;
    
    stlChar        * sLogFilePath   = NULL;
    stlBool          sIsArchiveMode = ztcmGetArchiveLogMode();
    stlBool          sFound         = STL_FALSE;
    stlBool          sFirst         = STL_TRUE;
    stlChar          sLogFileName[ STL_MAX_FILE_PATH_LENGTH ] = {0,};
    
    ztcCaptureStatus sCaptureStatus   = ZTC_CAPTURE_STATUS_MAX;
    ztcLogFileInfo * sLogFileInfo     = NULL;
    ztcLogFileInfo * sPrevLogFileInfo = NULL;
    
    STL_TRY( ztcdGetCurrentLogBufferInfo( gMasterMgr->mDbcHandle,
                                          &sGroupId,
                                          &sBlockSeq,
                                          &sFileSeqNo,
                                          aErrorStack ) == STL_SUCCESS );
    
    /**
     * 0. LogMirror 연동시 해당 파일을 읽는다. 
     * 1. Online 로그파일을 읽어본다.
     * 2. Archive 로그 파일을 읽어본다.
     */
    STL_RING_FOREACH_ENTRY( &gMasterMgr->mLogFileList, sLogFileInfo )
    {
        if ( sLogFileInfo->mGroupId == sGroupId )
        {
            sFound = STL_TRUE;
            break;   
        }
    }
    
    STL_TRY_THROW( sFound == STL_TRUE, RAMP_ERR_INTERNAL );
    
    /**
     * FileSeqNo, BlockSeq, Active, WrapNo 를 세팅해야 한다.
     */
    while( 1 )
    {
        STL_TRY( gRunState == STL_TRUE );
        /**
         * LogMirror에서 생성한 LogFile을 읽을 경우에는 해당 File이름을 변경해야 한다.
         */
        if( ztcmGetUserLogFilePathStatus() == STL_TRUE )
        {
            sLogFilePath = ztcmGetUserLogFilePath();
        
            stlSnprintf( sLogFileInfo->mName,
                         STL_MAX_FILE_PATH_LENGTH,
                         "%s"STL_PATH_SEPARATOR"%s%ld.log",
                         sLogFilePath,
                         ZTC_LOG_FILE_PREFIX,
                         sFileSeqNo );
        }
        
        stlMemcpy( sLogFileName, sLogFileInfo->mName, STL_MAX_FILE_PATH_LENGTH );
        
        STL_TRY( ztccGetLogFileSeqNoNGroupId( sLogFileName,
                                              &sRetFileSeqNo,
                                              &sRetGroupId,
                                              aErrorStack ) == STL_SUCCESS );
        
        /**
         * CaptureInfo를 세팅한다. 
         * LSN을 이용해서 BlockSeq를 찾는것이므로 임시정보로만 세팅해놓는다. 
         * Redo Log 파일은 Top->down 방식으로 읽어야 한다.
         */
        sLogFileInfo->mCaptureInfo.mFileSeqNo     = sFileSeqNo;
        sLogFileInfo->mCaptureInfo.mBlockSeq      = 0;
        sLogFileInfo->mCaptureInfo.mReadLogNo     = 1;
        sLogFileInfo->mCaptureInfo.mRestartLSN    = ZTC_INVALID_LSN;
        sLogFileInfo->mCaptureInfo.mLastCommitLSN = ZTC_INVALID_LSN;
        sLogFileInfo->mActive                     = STL_FALSE;
        
        /**
         * Online Redo Log 파일이 해당 파일이 아닐 경우, Archive 되어있는 파일을 읽어야 한다.
         */
        if( sRetFileSeqNo != sFileSeqNo )
        {
            STL_TRY_THROW( ztcmGetUserLogFilePathStatus() == STL_FALSE, RAMP_ERR_INVALID_LOGFILE );
            
            /**
             * ArchiveLog에서 읽어와 함
             */
            STL_TRY_THROW( sIsArchiveMode == STL_TRUE, RAMP_ERR_INVALID_LOGFILE );
            
            STL_TRY( ztcmGetArchiveLogFile( sLogFileName,
                                            sFileSeqNo,
                                            aErrorStack ) == STL_SUCCESS );
            
            STL_TRY( ztccValidateArchiveLogFile( sLogFileName,
                                                 sLogFileInfo,
                                                 aErrorStack ) == STL_SUCCESS );
        }
        
        /**
         * 첫 파일에서 WrapNo를 얻어와야 한다.
         */
        if( sFirst == STL_TRUE )
        {
            STL_TRY( ztccGetLogFileWrapNo( sLogFileName,
                                           &sWrapNo,
                                           aErrorStack ) == STL_SUCCESS );
            sFirst = STL_FALSE;
        }
        
        /**
         * LSN을 이용해 BlockSeq를 얻는다.
         */
        STL_TRY( ztccGetLogFileBlockSeq( sLogFileName,
                                         &sLogFileInfo->mCaptureInfo,
                                         aBeginLsn,
                                         sWrapNo,
                                         &sFound,
                                         &sCaptureStatus,
                                         aErrorStack ) == STL_SUCCESS );
        
        /**
         * 찾았을 경우....
         */
        if( sFound == STL_TRUE )
        {
            /**
             * BlockSeq와 FileSeqNo는 미리 세팅되어서 넘어온다.
             */
            sLogFileInfo->mCaptureInfo.mWrapNo = sWrapNo;
            sLogFileInfo->mActive = STL_TRUE;
            break;
        }
        
        if( sCaptureStatus == ZTC_CAPTURE_STATUS_READ_DONE ||
            sCaptureStatus == ZTC_CAPTURE_STATUS_INVALID_WRAP_NO )
        {
            /**
             * LogFile이 Reuse되거나, LogFile이 아직 생성되지 않은 상태이므로 다시 읽도록 해야한다.
             */
            continue;
        }
        else if( sCaptureStatus == ZTC_CAPTURE_STATUS_EOF )
        {
            /**
             * Do Next.... Nothing To do..
             */
        }
        else
        {
            STL_DASSERT( STL_FALSE );
        }
        
        /**
         * 이전 Redo Log File로 세팅한다.
         * LogFileInfo, FileSeqNo, GroupId, WrapNo
         */
        sFileSeqNo--;
        STL_TRY_THROW( sFileSeqNo >=0, RAMP_ERR_NO_LOGFILE );
        
        sPrevLogFileInfo = STL_RING_GET_PREV_DATA( &gMasterMgr->mLogFileList, sLogFileInfo );
        if( STL_RING_IS_HEADLINK( &gMasterMgr->mLogFileList, 
                                   sPrevLogFileInfo ) == STL_TRUE )
        {
            sLogFileInfo = STL_RING_GET_LAST_DATA( &gMasterMgr->mLogFileList );
        }
        else
        {
            sLogFileInfo = sPrevLogFileInfo;
        }
        
        sGroupId = sLogFileInfo->mGroupId;
        
        if( sWrapNo == 0 )
        {
            sWrapNo = ZTC_WRAP_NO_MAX;
        }
        else
        {
            sWrapNo--;   
        }
    }
    
    return STL_SUCCESS;   
    
    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INTERNAL_ERROR,
                      NULL,
                      aErrorStack,
                      "Set LogFile Info Using LSN" );
    }
    STL_CATCH( RAMP_ERR_INVALID_LOGFILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_LOG_FILE_REUSED,
                      NULL,
                      aErrorStack,
                      sLogFileName );
    }
    STL_CATCH( RAMP_ERR_NO_LOGFILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INTERNAL_ERROR,
                      NULL,
                      aErrorStack,
                      "Invalid LogFileSeqNo" );
    }  
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcmSetCurrentLogFileInfo( stlErrorStack * aErrorStack )
{
    ztcLogFileInfo * sLogFileInfo = NULL;
    stlInt16         sGroupId     = 0;
    stlInt32         sBlockSeq    = 0;
    stlInt64         sFileSeqNo   = ZTC_INVALID_FILE_SEQ_NO;
    stlBool          sFound       = STL_FALSE;
    
    STL_TRY( ztcdGetCurrentLogBufferInfo( gMasterMgr->mDbcHandle,
                                          &sGroupId,
                                          &sBlockSeq,
                                          &sFileSeqNo,
                                          aErrorStack ) == STL_SUCCESS );
    
    STL_RING_FOREACH_ENTRY( &gMasterMgr->mLogFileList, sLogFileInfo )
    {
        if ( sLogFileInfo->mCaptureInfo.mRedoLogGroupId == sGroupId )
        {
            sFound = STL_TRUE;
            break;   
        }
    }
    
    STL_TRY_THROW( sFound == STL_TRUE, RAMP_ERR_INTERNAL );
    
    /**
     * LogHeader를 Skip 해야 되기 때문에 +1 ... 아니다.. REAR_LID_BLOCK_SEQ_NO는 LogHeader를 포함한 값이므로 우선 +1 을 하지 않도록 한다.
     * Log를 쓴 다음 부터 읽어야 하므로 +1 을 해준다. 총 +2가 된다.
     * TRANSACTION_COMMIT_WRITE_MODE 에 따라서 달라져야 하는가?
     * Todo !!! 정리해야 함!!
     */
    sLogFileInfo->mCaptureInfo.mBlockSeq  = sBlockSeq; //sBlockSeq + 1;
    sLogFileInfo->mCaptureInfo.mFileSeqNo = sFileSeqNo;
    sLogFileInfo->mActive                 = STL_TRUE;

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INTERNAL_ERROR,
                      NULL,
                      aErrorStack,
                      "Set LogFile Info" );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztcmSetHomeDir( stlErrorStack * aErrorStack )
{
    stlChar   sPath[STL_MAX_FILE_PATH_LENGTH] = {0,};
    stlBool   sFound = STL_FALSE;
        
    STL_TRY( stlGetEnv( sPath, 
                        STL_SIZEOF(sPath),
                        STL_ENV_DB_DATA,
                        &sFound,
                        aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( sFound == STL_TRUE, RAMP_ERR_INVALID_HOMEDIR );
    
    /**
     * GOLDILOCKS_DATA 끝에 '/'를 제거한다.
     */
    if ( sPath[ stlStrlen( sPath ) - 1 ] == '/' ||
         sPath[ stlStrlen( sPath ) - 1 ] == '\\' )
    {
        sPath[ stlStrlen( sPath ) ] = '\0';
    }
    
    stlSnprintf( gConfigure.mHomeDir, 
                 STL_MAX_FILE_PATH_LENGTH, 
                 "%s"STL_PATH_SEPARATOR, 
                 sPath );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_HOMEDIR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_HOME,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztcmPrintStatus( stlBool         aIsMaster,
                           stlErrorStack * aErrorStack )
{
    stlChar     sFileName[STL_MAX_FILE_PATH_LENGTH];
    stlChar     sLineBuff[STL_MAX_SQL_IDENTIFIER_LENGTH * 2] = {0,};
    stlChar     sProcessId[STL_MAX_SQL_IDENTIFIER_LENGTH] = {0,};
    stlChar     sGroupName[STL_MAX_SQL_IDENTIFIER_LENGTH] = {0,};
    stlFile     sFile;
    stlInt32    sState     = 0;
    stlBool     sExist     = STL_FALSE;
    stlBool     sInfo      = STL_FALSE;
    stlOffset   sFileSize  = 0;
    stlOffset   sOffset    = 0;
    stlChar   * sColonMark = NULL;
    stlChar   * sEndPtr    = NULL;
    stlInt64    sCheckProcId;
    stlProc     sCheckProc;
    stlChar     sCfgFileName[STL_MAX_FILE_PATH_LENGTH];

    ztcGroupItem * sGroup = NULL;

    gConfigure.mIsMaster      = aIsMaster;

    STL_TRY( stlCreateRegionAllocator( &gConfigure.mAllocator,
                                       ZTC_REGION_INIT_SIZE,
                                       ZTC_REGION_NEXT_SIZE,
                                       aErrorStack ) == STL_SUCCESS );
    sState = 1;

    STL_RING_INIT_HEADLINK( &gConfigure.mGroupList,
                            STL_OFFSETOF( ztcGroupItem, mLink ) );

    if( gConfigure.mUserConfigure[0] != 0x00 )
    {
        STL_TRY( ztcmGetConfigureFileName( sCfgFileName,
                                           aErrorStack ) == STL_SUCCESS );

        stlPrintf("\n config file : %s\n\n", sCfgFileName);

        /**
         * Group만 Parsing한다.
         */
        if( ztcfIdentifyGroup( aErrorStack ) == STL_SUCCESS )
        {
            STL_TRY_THROW( STL_RING_IS_EMPTY( &gConfigure.mGroupList ) != STL_TRUE, RAMP_ERR_INVALID_GROUP_COUNT );
        }
        else
        {
            (void)stlPopError( aErrorStack );
        }
    }


    STL_TRY( ztcmGetPidFileName( sFileName, 
                                 aIsMaster,
                                 aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlExistFile( sFileName,
                           &sExist,
                           aErrorStack ) == STL_SUCCESS );    
    
    stlPrintf("======================================\n");
    if( aIsMaster == STL_TRUE )
    {
        stlPrintf("|       CYCLONEM STATUS - MASTER     |\n");
    }
    else
    {
        stlPrintf("        CYCLONEM STATUS - SLAVE       \n");
    }
    stlPrintf("======================================\n");
    
    STL_TRY_THROW( sExist == STL_TRUE, RAMP_SUCCESS );

    STL_TRY( stlOpenFile( &sFile,
                          sFileName,
                          STL_FOPEN_READ | STL_FOPEN_WRITE | STL_FOPEN_BINARY,
                          STL_FPERM_OS_DEFAULT,
                          aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( stlLockFile( &sFile,
                          STL_FLOCK_EXCLUSIVE,
                          aErrorStack ) == STL_SUCCESS );
    sState = 3;
    
    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_END,
                          &sFileSize,
                          aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( sFileSize > 0, RAMP_SUCCESS );

    sInfo = STL_TRUE;

    sOffset = 0;
    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          aErrorStack ) == STL_SUCCESS );

    while( 1 )
    {
        if( stlGetStringFile( sLineBuff,
                              STL_SIZEOF( sLineBuff ),
                              &sFile,
                              aErrorStack ) == STL_SUCCESS )
        {
            sColonMark = stlStrchr( sLineBuff, ':' );

            if( sColonMark != NULL )
            {
                sColonMark++;

                stlStrncpy( sProcessId,
                            sColonMark,
                            stlStrlen( sColonMark ) );

                stlStrncpy( sGroupName,
                            sLineBuff,
                            stlStrlen( sLineBuff ) - stlStrlen( sProcessId ) - 1 );

                STL_TRY( stlStrToInt64( sProcessId,
                                        STL_NTS,
                                        &sEndPtr,
                                        10,
                                        (stlInt64*)&sCheckProcId,
                                        aErrorStack ) == STL_SUCCESS );

                sCheckProc.mProcID = sCheckProcId;

                STL_TRY_THROW( *sEndPtr == 0, RAMP_ERR_INVALID_VALUE );

                sGroup = NULL;
                if( STL_RING_IS_EMPTY( &gConfigure.mGroupList ) != STL_TRUE )
                {
                    sGroup = STL_RING_GET_FIRST_DATA( &gConfigure.mGroupList );

                    while( sGroup != NULL )
                    {
                        if( stlStrcmp( sGroup->mName, sGroupName ) == 0 )
                        {
                            break;
                        }

                        if( sGroup == STL_RING_GET_LAST_DATA( &gConfigure.mGroupList ) )
                        {
                            sGroup = NULL;
                            break;
                        }
                        sGroup = STL_RING_GET_NEXT_DATA( &gConfigure.mGroupList, sGroup );
                    }
                }

                /**
                 * PROCESS가 살아있는지 확인
                 */
                if( stlKillProc( &sCheckProc,
                                 0,           //NULL Signal, Error Check
                                 aErrorStack ) == STL_SUCCESS )
                {
                    if( sGroup != NULL )
                    {
                        sGroup->mAlive = STL_TRUE;
                    }
                    else
                    {
                        stlPrintf(" %s Running...\n", sGroupName );
                    }
                }
                else
                {
                    (void)stlPopError( aErrorStack );

                    if( sGroup != NULL )
                    {
                        sGroup->mAlive = STL_FALSE;
                    }
                    else
                    {
                        stlPrintf(" %s Stopped...\n", sGroupName );
                    }
                }
            }

        }
        else
        {
            STL_TRY( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_EOF );
            (void)stlPopError( aErrorStack );

            break;
        }
    }

    if( STL_RING_IS_EMPTY( &gConfigure.mGroupList ) != STL_TRUE )
    {
        sGroup = STL_RING_GET_FIRST_DATA( &gConfigure.mGroupList );

        while( sGroup != NULL )
        {
            if( sGroup->mAlive == STL_TRUE )
            {
                stlPrintf(" %s Running...\n", sGroup->mName );
            }
            else
            {
                stlPrintf(" %s Stopped...\n", sGroup->mName );
            }

            if( sGroup == STL_RING_GET_LAST_DATA( &gConfigure.mGroupList ) )
            {
                sGroup = NULL;
                break;
            }
            sGroup = STL_RING_GET_NEXT_DATA( &gConfigure.mGroupList, sGroup );
        }
    }

    STL_RAMP( RAMP_SUCCESS );


    if( sGroup != NULL )
    {
        sInfo = STL_TRUE;

        while( 1 )
        {
            stlPrintf(" %s Stopped...\n", sGroup->mName );

            if( sGroup == STL_RING_GET_LAST_DATA( &gConfigure.mGroupList ) )
            {
                break;
            }

            sGroup = STL_RING_GET_NEXT_DATA( &gConfigure.mGroupList, sGroup );
        }
    }


    if ( sInfo == STL_FALSE )
    {
        stlPrintf("|      Service is not running...     |\n");
    }
    
    stlPrintf("--------------------------------------\n");
    
    switch( sState )
    {
        case 3:
            (void) stlUnlockFile( &sFile,
                                  aErrorStack );
        case 2:
            (void) stlCloseFile( &sFile,
                                 aErrorStack );
        case 1:
            (void)stlDestroyRegionAllocator( &gConfigure.mAllocator,
                                             aErrorStack );
        default:
            break;
    }

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_GROUP_COUNT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_GROUP_COUNT,
                      NULL,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INTERNAL_ERROR,
                      NULL,
                      aErrorStack,
                      "invalid process id" );
    }
    STL_FINISH;
    
    switch( sState )
    {
        case 3:
            (void) stlUnlockFile( &sFile, 
                                  aErrorStack );
        case 2:
            (void) stlCloseFile( &sFile,
                                 aErrorStack );
        case 1:
            (void)stlDestroyRegionAllocator( &gConfigure.mAllocator,
                                             aErrorStack );
        default:
            break; 
    }
    
    return STL_FAILURE;
}

stlUInt32 ztcmMurmurHash( const void * aKey, stlInt32 aLength )
{
    const stlUInt32   m = 0x5bd1e995;
    const stlInt32    r = 24;
    const stlUInt8  * sData = aKey;
   
    stlInt32  sLength = aLength;
    stlInt32  sKey;
    stlUInt32 h = ZTC_DISTRIBUTOR_SLOT_SIZE ^ aLength;

    while( sLength >= 4 )
    {
        sKey = *(stlUInt32*)sData;

        sKey *= m;
        sKey ^= sKey >> r;
        sKey *= m;

        h *= m;
        h ^= sKey;

        sData += 4;
        sLength -= 4;
    }

    switch( sLength )
    {
        case 3: 
            h ^= sData[2] << 16;
        case 2: 
            h ^= sData[1] << 8;
        case 1: 
            h ^= sData[0];
            h *= m;
    }
        
    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}


stlBool ztcmGetSyncMode()
{
    return gConfigure.mIsSync;
}


void ztcmSetSyncMode( stlBool aMode )
{
    gConfigure.mIsSync = aMode;
}

stlStatus ztcmGetSlaveIp( stlChar       * aSlaveIp,
                          stlErrorStack * aErrorStack )
{
    stlInt32 sLen = stlStrlen( gMasterMgr->mSlaveIp );
    
    STL_TRY_THROW( sLen > 0 && sLen < 32, 
                   RAMP_ERR_INVALID_SLAVE_IP );
    
    stlStrncpy( aSlaveIp, gMasterMgr->mSlaveIp, sLen + 1 );
    
    return STL_SUCCESS;   
    
    STL_CATCH( RAMP_ERR_INVALID_SLAVE_IP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_SLAVE_CONN_INFO,
                      NULL,
                      aErrorStack,
                      "Slave IP Address");
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztcmGetSlaveUserId( stlChar       * aUserId,
                              stlErrorStack * aErrorStack )
{
    stlInt32 sLen = stlStrlen( gMasterMgr->mSlaveUserId );
    
    STL_TRY_THROW( sLen > 0 && sLen < STL_MAX_SQL_IDENTIFIER_LENGTH, 
                   RAMP_ERR_INVALID_SLAVE_USER_ID );
    
    stlStrncpy( aUserId, gMasterMgr->mSlaveUserId, sLen + 1 );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_SLAVE_USER_ID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_SLAVE_CONN_INFO,
                      NULL,
                      aErrorStack,
                      "Slave UserId");
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcmGetSlaveUserPw( stlChar       * aUserPw,
                              stlErrorStack * aErrorStack )
{
    stlInt32 sLen = stlStrlen( gMasterMgr->mSlaveUserPw );
    
    STL_TRY_THROW( sLen > 0 && sLen < STL_MAX_SQL_IDENTIFIER_LENGTH, 
                   RAMP_ERR_INVALID_SLAVE_USER_PW );
    
    stlStrncpy( aUserPw, gMasterMgr->mSlaveUserPw, sLen + 1 );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_SLAVE_USER_PW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_SLAVE_CONN_INFO,
                      NULL,
                      aErrorStack,
                      "Slave UserPw");
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcmGetSlaveGoldilocksListenPort( stlInt32      * aPort,
                                       stlErrorStack * aErrorStack )
{
    STL_TRY_THROW( gMasterMgr->mSlaveGoldilocksListenPort < 65536 &&
                   gMasterMgr->mSlaveGoldilocksListenPort != 0, RAMP_ERR_INVALID_SLAVE_PORT );
    
    *aPort = gMasterMgr->mSlaveGoldilocksListenPort;
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_SLAVE_PORT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_SLAVE_CONN_INFO,
                      NULL,
                      aErrorStack,
                      "Slave Goldilocks Listen Port");
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztcmSetSyncherCount( stlChar       * aCount,
                               stlErrorStack * aErrorStack )
{
    stlChar   * sEndPtr;
    stlInt64    sCount = 0;
    
    STL_TRY( stlStrToInt64( aCount,
                            STL_NTS,
                            &sEndPtr,
                            10,
                            &sCount, 
                            aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( *sEndPtr == 0, RAMP_ERR_INVALID_VALUE );
    
    STL_TRY_THROW( sCount >= 1 && sCount <=20, RAMP_ERR_TOO_LOW_VALUE );
    
    gConfigure.mSyncherCount = sCount;
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "syncher_count",
                      sCount );
    }
    STL_CATCH( RAMP_ERR_TOO_LOW_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "syncher_count (min:1, max:20)",
                      sCount );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlInt32 ztcmGetSyncherCount()
{
    stlInt32 sCount = 0;
    
    if( gConfigure.mSyncherCount > 0 )
    {
        sCount = gConfigure.mSyncherCount;
    }
    else
    {
        sCount = ZTC_DEFAULT_SYNCHER_COUNT;
    }
    
    return sCount;
}

stlStatus ztcmSetSyncArraySize( stlChar       * aSize,
                                stlErrorStack * aErrorStack )
{
    stlChar   * sEndPtr;
    stlInt64    sSize = 0;
    
    STL_TRY( stlStrToInt64( aSize,
                            STL_NTS,
                            &sEndPtr,
                            10,
                            &sSize, 
                            aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( *sEndPtr == 0, RAMP_ERR_INVALID_VALUE );
    
    STL_TRY_THROW( sSize >= 1, RAMP_ERR_TOO_LOW_VALUE );
    
    gConfigure.mSyncArraySize = sSize;
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "sync_array_size",
                      sSize );
    }
    STL_CATCH( RAMP_ERR_TOO_LOW_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "sync_array_size (min:1)",
                      sSize );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlInt32 ztcmGetSyncArraySize()
{
    stlInt32 sSize = 0;
    
    if( gConfigure.mSyncArraySize > 0 )
    {
        sSize = gConfigure.mSyncArraySize;
    }
    else
    {
        sSize = ZTC_DEFAULT_SYNC_ARRAY_SIZE;
    }
    
    return sSize;
}


stlStatus ztcmSetUpdateApplyMode( stlChar       * aMode,
                                  stlErrorStack * aErrorStack )
{
    stlChar   * sEndPtr;
    stlInt64    sMode = 0;

    STL_TRY( stlStrToInt64( aMode,
                            STL_NTS,
                            &sEndPtr,
                            10,
                            &sMode,
                            aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( *sEndPtr == 0, RAMP_ERR_INVALID_VALUE );

    STL_TRY_THROW( (sMode >= 0) && (sMode <= 2), RAMP_ERR_ACCEPTABLE_VALUE );

    gConfigure.mUpdateApplyMode = sMode;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "update_apply_mode",
                      sMode );
    }
    STL_CATCH( RAMP_ERR_ACCEPTABLE_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "update_apply_mode (0:Compare with primary key only, 1: Compare with before value and primary key, 2: Compare with primary key but check before value)",
                      sMode );
    }
    STL_FINISH;

    return STL_FAILURE;
}


stlInt32 ztcmGetUpdateApplyMode( stlErrorStack * aErrorStack )
{
    stlInt32 sMode = 0;

    if( gConfigure.mUpdateApplyMode > 0 )
    {
        sMode = gConfigure.mUpdateApplyMode;
    }

    /**
     * file에 write는 updateApplyMode 0으로 설정함
     */
    if( gConfigure.mApplyToFile == STL_TRUE )
    {
        sMode = 0;
    }

    return sMode;
}

stlStatus ztcmLogConfigureMsgMaster( stlErrorStack * aErrorStack )
{
    stlChar  sHostIp[16]                                = { 0, };
    stlChar  sDsn[STL_MAX_SQL_IDENTIFIER_LENGTH]        = { 0, };
    stlChar  sUserLogFilePath[STL_MAX_FILE_PATH_LENGTH] = { 0, };
    stlChar  sTransFilePath[STL_MAX_FILE_PATH_LENGTH]   = { 0, };
    stlInt32 sCommChunkCount;
    stlInt64 sReadLogBlockCount;
    stlInt64 sTransSortAreaSize;
    stlInt32 sSyncherCount;
    stlInt32 sSyncArraySize;
    
    if( stlStrlen( gConfigure.mHostIp ) == 0 )
    {
        stlStrcpy( sHostIp, "NULL" );
    }
    else
    {
        stlStrcpy( sHostIp, gConfigure.mHostIp );
    }

    if( stlStrlen( gConfigure.mDsn ) == 0 )
    {
        stlStrcpy( sDsn, "NULL" );
    }
    else
    {
        stlStrcpy( sDsn, gConfigure.mDsn );
    }

    if( stlStrlen( gConfigure.mTransFilePath ) == 0 )
    {
        stlStrcpy( sTransFilePath, "NULL" );
    }
    else
    {
        stlStrcpy( sTransFilePath, gConfigure.mTransFilePath );
    }

    if( stlStrlen( gConfigure.mUserLogFilePath ) == 0 )
    {
        stlStrcpy( sUserLogFilePath, "NULL" );
    }
    else
    {
        stlStrcpy( sUserLogFilePath, gConfigure.mUserLogFilePath );
    }
    
    sCommChunkCount    = gConfigure.mCommChunkCount == 0 ?
        ZTC_CHUNK_ITEM_COUNT : gConfigure.mCommChunkCount;

    sReadLogBlockCount = gConfigure.mReadLogBlockCount == 0 ?
        ZTC_READ_BUFFER_BLOCK_COUNT : gConfigure.mReadLogBlockCount;
    
    sTransSortAreaSize = gConfigure.mTransSortAreaSize == 0 ?
        ZTC_TRANSACTION_SORT_AREA_SIZE : gConfigure.mTransSortAreaSize;

    if( gConfigure.mIsSync == STL_TRUE )
    {
        sSyncherCount = gConfigure.mSyncherCount == 0 ?
            ZTC_DEFAULT_SYNCHER_COUNT : gConfigure.mSyncherCount;

        sSyncArraySize = gConfigure.mSyncArraySize == 0 ?
            ZTC_DEFAULT_SYNC_ARRAY_SIZE : gConfigure.mSyncArraySize;
    }
    else
    {
        sSyncherCount  = gConfigure.mSyncherCount;
        sSyncArraySize = gConfigure.mSyncArraySize;
    }
    
    STL_TRY( ztcmLogMsg( aErrorStack,
                         "Master Configure\n"
                         "    Host Ip                    : %s\n"
                         "    Host Port                  : %d\n"
                         "    Port                       : %d\n"
                         "    DSN                        : %s\n"
                         "    Group Count                : %d\n"
                         "    Communication Chunk Count  : %d\n"
                         "    Transaction File Path      : %s\n"
                         "    Log Path                   : %s\n"
                         "    Read Log Block Count       : %ld\n"
                         "    Transaction Sort Area Size : %ld\n"
                         "    Giveup Interval            : %ld\n"
                         "    Syncher Count              : %d\n"
                         "    Sync Array Size            : %d\n",
                         sHostIp,
                         gConfigure.mHostPort,
                         gConfigure.mPort,
                         sDsn,
                         gConfigure.mGroupCount,
                         sCommChunkCount,
                         sTransFilePath,
                         sUserLogFilePath,
                         sReadLogBlockCount,
                         sTransSortAreaSize,
                         gConfigure.mGiveupInterval,
                         sSyncherCount,
                         sSyncArraySize ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztcmLogConfigureMsgSlave( stlErrorStack * aErrorStack )
{
    stlChar  sPropageModeStr[6]                  = { 0, };
    stlChar  sHostIp[16]                         = { 0, };
    stlChar  sMasterIp[16]                       = { 0, };
    stlChar  sDsn[STL_MAX_SQL_IDENTIFIER_LENGTH] = { 0, };
    stlInt32 sCommChunkCount;
    stlInt32 sUpdateApplyMode;
    stlInt64 sApplierCount;
    stlInt64 sApplierCommitSize;
    stlInt64 sApplierArraySize;

    if( stlStrlen( gConfigure.mHostIp ) == 0 )
    {
        stlStrcpy( sHostIp, "NULL" );
    }
    else
    {
        stlStrcpy( sHostIp, gConfigure.mHostIp );
    }

    if( stlStrlen( gConfigure.mMasterIp ) == 0 )
    {
        stlStrcpy( sMasterIp, "NULL" );
    }
    else
    {
        stlStrcpy( sMasterIp, gConfigure.mMasterIp );
    }

    if( stlStrlen( gConfigure.mDsn ) == 0 )
    {
        stlStrcpy( sDsn, "NULL" );
    }
    else
    {
        stlStrcpy( sDsn, gConfigure.mDsn );
    }
    
    if( gConfigure.mPropagateMode == STL_TRUE )
    {
        stlStrcpy( sPropageModeStr, "TRUE" );
    }
    else
    {
        stlStrcpy( sPropageModeStr, "FALSE" );
    }

    sCommChunkCount    = ztcmGetCommChunkCount();

    sApplierCount      = ztcmGetApplierCount( aErrorStack );

    sApplierCommitSize = ztcmGetApplyCommitSize();

    sApplierArraySize  = ztcmGetApplyArraySize();
    
    sUpdateApplyMode = ztcmGetUpdateApplyMode( aErrorStack );

    STL_TRY( ztcmLogMsg( aErrorStack,
                         "Slave Configure\n"
                         "    Host Ip                   : %s\n"
                         "    Master Ip                 : %s\n"
                         "    Host Port                 : %d\n"
                         "    Port                      : %d\n"
                         "    Goldilocks Listen Port         : %d\n"
                         "    DSN                       : %s\n"
                         "    Group Count               : %d\n"
                         "    Communication Chunk Count : %d\n"
                         "    Update Apply Mode         : %d\n"
                         "    Applier Count             : %ld\n"
                         "    Apply Array Size          : %ld\n"
                         "    Apply Commit Size         : %ld\n"
                         "    Propagate Mode            : %s\n",
                         sHostIp,
                         sMasterIp,
                         gConfigure.mHostPort,
                         gConfigure.mPort,
                         gConfigure.mGoldilocksListenPort,
                         sDsn,
                         gConfigure.mGroupCount,
                         sCommChunkCount,
                         sUpdateApplyMode,
                         sApplierCount,
                         sApplierArraySize,
                         sApplierCommitSize,
                         sPropageModeStr ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztcmSetLibConfig( stlChar       * aLibConfig,
                            stlErrorStack * aErrorStack )
{
    stlInt32       sLen   = 0;

    sLen = stlStrlen( aLibConfig );

    STL_TRY_THROW( sLen < STL_SIZEOF( gConfigure.mLibConfig ), RAMP_ERR_VALUE_TOO_LONG );

    stlStrcpy( gConfigure.mLibConfig, aLibConfig );

    STL_TRY( ztcmChangePath( gConfigure.mLibConfig,
                             STL_SIZEOF( gConfigure.mLibConfig ),
                             aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_VALUE_TOO_LONG )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_VALUE_TOO_LONG,
                      NULL,
                      aErrorStack,
                      "LIB_CONFIG" );
    }
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztcmGetLibConfig( stlChar       * aLibConfig,
                            stlInt32        aBuffSize,
                            stlErrorStack * aErrorStack )
{
    stlInt32       sLen   = 0;
    stlChar      * sPos = NULL;

    sLen = stlStrlen( gConfigure.mLibConfig );

    if( sLen > 0 )
    {
        sPos = gConfigure.mLibConfig;

        /**
         * ['] 제거
         */
        if( sPos[0] == '\'' )
        {
            sPos++;
            sLen--;
        }

        sLen = stlSnprintf( aLibConfig,
                            aBuffSize,
                            "%s",
                            sPos );

        if( aLibConfig[ sLen - 1 ] == '\'' )
        {
            aLibConfig[ sLen - 1 ] = 0x00;
            sLen--;
        }
    }
    else
    {
        sLen = stlSnprintf( aLibConfig,
                            aBuffSize,
                            "%sconf"STL_PATH_SEPARATOR"%s",
                            gConfigure.mHomeDir,
                            ZTC_DEFAULT_LIB_CONFIG );
    }
    STL_TRY_THROW( sLen + 1 < aBuffSize, RAMP_ERR_VALUE_TOO_LONG );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_VALUE_TOO_LONG )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_VALUE_TOO_LONG,
                      NULL,
                      aErrorStack,
                      "LIB_CONFIG" );
    }
    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztcmSetSqlFileName( stlChar       * aFileName,
                              stlErrorStack * aErrorStack )
{
    stlInt32       sLen   = 0;

    sLen = stlStrlen( aFileName );

    STL_TRY_THROW( sLen < STL_SIZEOF( gConfigure.mSqlFileName ), RAMP_ERR_VALUE_TOO_LONG );

    stlStrcpy( gConfigure.mSqlFileName, aFileName );

    STL_TRY( ztcmChangePath( gConfigure.mSqlFileName,
                             STL_SIZEOF( gConfigure.mSqlFileName ),
                             aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_VALUE_TOO_LONG )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_VALUE_TOO_LONG,
                      NULL,
                      aErrorStack,
                      aFileName,
                      "SQL_FILE_NAME" );
    }
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztcmGetSqlFileName( stlChar       * aFileName,
                              stlInt32        aBuffSize,
                              stlErrorStack * aErrorStack )
{
    stlInt32       sLen   = 0;
    stlChar      * sPos = NULL;

    sLen = stlStrlen( gConfigure.mSqlFileName );

    if( sLen > 0 )
    {
        sPos = gConfigure.mSqlFileName;

        /**
         * ['] 제거
         */
        if( sPos[0] == '\'' )
        {
            sPos++;
            sLen--;
        }

        sLen = stlSnprintf( aFileName,
                            aBuffSize,
                            "%s",
                            sPos );

        if( aFileName[ sLen - 1 ] == '\'' )
        {
            aFileName[ sLen - 1 ] = 0x00;
            sLen--;
        }
    }
    else
    {
        sLen = stlSnprintf( aFileName,
                            aBuffSize,
                            "%strc"STL_PATH_SEPARATOR"%s",
                            gConfigure.mHomeDir,
                            ZTC_DEFAULT_SQL_FILE );
    }

    STL_TRY_THROW( sLen + 1 < aBuffSize, RAMP_ERR_VALUE_TOO_LONG );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_VALUE_TOO_LONG )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_VALUE_TOO_LONG,
                      NULL,
                      aErrorStack,
                      "SQL_FILE_NAME" );
    }
    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztcmSetSqlFileSize( stlChar       * aFileSize,
                              stlErrorStack * aErrorStack )
{
    stlChar   * sEndPtr;
    stlInt64    sSize = 0;

    STL_TRY( stlStrToInt64( aFileSize,
                            STL_NTS,
                            &sEndPtr,
                            10,
                            &sSize,
                            aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( *sEndPtr == 0, RAMP_ERR_INVALID_VALUE );

    STL_TRY_THROW( sSize >= 1, RAMP_ERR_TOO_LOW_VALUE );

    gConfigure.mSqlFileSize = sSize;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "SQL_FILE_SIZE",
                      sSize );
    }
    STL_CATCH( RAMP_ERR_TOO_LOW_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "SQL_FILE_SIZE (min:1)",
                      sSize );
    }
    STL_FINISH;

    return STL_FAILURE;
}

stlInt64 ztcmGetSqlFileSize()
{
    stlInt32 sSize = 0;

    if( gConfigure.mSqlFileSize > 0 )
    {
        sSize = gConfigure.mSqlFileSize;
    }
    else
    {
        sSize = ZTC_DEFAULT_MAX_SQL_FILE_SIZE;
    }

    return sSize;
}


stlStatus ztcmChangePath( stlChar           * aFilePath,
                          stlInt32            aBuffSize,
                          stlErrorStack     * aErrorStack )
{
    stlInt32   sLen = stlStrlen( aFilePath );
    stlChar  * sPosStart;
    stlChar  * sPosEnd;
    stlInt64   sEnvKeyLen;
    stlChar    sEnvKey[STL_MAX_FILE_PATH_LENGTH];
    stlChar    sEnvValue[STL_MAX_FILE_PATH_LENGTH];
    stlBool    sIsFound;

    sPosStart = stlStrchr( aFilePath, '$' );

    if( sPosStart != NULL )
    {
        sPosEnd = stlStrchr( sPosStart, '/' );
        if( sPosEnd == NULL )
        {
            sPosEnd = &aFilePath[sLen];
        }
        else
        {
            sPosEnd = sPosEnd;
        }

        sEnvKeyLen = sPosEnd - sPosStart;

        STL_TRY_THROW( sEnvKeyLen < STL_MAX_FILE_PATH_LENGTH, RAMP_ERR_OVERFLOW );

        /**
         * sEnvKey에 '$' 는 빼고 저장.
         */
        stlMemcpy( sEnvKey, sPosStart + 1, sEnvKeyLen - 1 );

        STL_TRY( stlGetEnv( sEnvValue,
                            STL_SIZEOF(sEnvValue),
                            sEnvKey,
                            &sIsFound,
                            aErrorStack ) == STL_SUCCESS );

        STL_TRY_THROW( sIsFound == STL_TRUE, RAMP_ERR_NOT_FOUND_ENV );

        STL_TRY_THROW( sLen - sEnvKeyLen + stlStrlen(sEnvValue) < STL_MAX_FILE_PATH_LENGTH,
                       RAMP_ERR_OVERFLOW );

        /**
         * sPosStart ~ sPosEnd까지를 env 값으로 변경한다.
         */
        stlMemmove( sPosStart + stlStrlen(sEnvValue), sPosEnd, stlStrlen(sPosEnd) );
        stlMemcpy( sPosStart, sEnvValue, stlStrlen(sEnvValue) );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "buffer overflow" );
    }
    STL_CATCH( RAMP_ERR_NOT_FOUND_ENV )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "[%s] env not found",
                      sEnvValue );
    }
    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
