/*******************************************************************************
 * ztcaGeneral.c
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
 * @file ztcaGeneral.c
 * @brief GlieseTool Cyclone Applier General  Routines
 */

#include <goldilocks.h>
#include <ztc.h>


extern ztcSlaveMgr  * gSlaveMgr;
extern ztcConfigure   gConfigure;

stlStatus ztcaInitializeApplier( ztcApplierMgr * aApplierMgr,
                                 stlErrorStack * aErrorStack )
{
    stlInt32   sState         = 0;
    stlInt32   sIdx           = 0;
    stlBool    sPropagateMode = ztcmGetPropagateMode();
    stlChar  * sPtr           = NULL;

    ztcApplierRecItem * sItem = NULL;
    
    /**
     * Initialize Value
     */
    aApplierMgr->mApplyCount            = 0;
    aApplierMgr->mApplyCountAfterCommit = 0;
    aApplierMgr->mAnalyzeState          = STL_TRUE;
    aApplierMgr->mWaitDistState         = STL_FALSE;
    aApplierMgr->mWaitSubDistState      = ZTC_SUBDIST_USED_FALSE;
    aApplierMgr->mPrevTableInfo         = NULL;
    aApplierMgr->mPrevTransType         = ZTC_CAPTURE_TYPE_NONE;
    aApplierMgr->mArraySize             = ztcmGetApplyArraySize();
    
    aApplierMgr->mRestartInfo.mFileSeqNo      = ZTC_INVALID_FILE_SEQ_NO;
    aApplierMgr->mRestartInfo.mRestartLSN     = ZTC_INVALID_LSN;
    aApplierMgr->mRestartInfo.mLastCommitLSN  = ZTC_INVALID_LSN;
    aApplierMgr->mRestartInfo.mRedoLogGroupId = 0;
    aApplierMgr->mRestartInfo.mBlockSeq       = 0; 
    aApplierMgr->mRestartInfo.mReadLogNo      = 1;
    aApplierMgr->mRestartInfo.mWrapNo         = ZTC_INVALID_WRAP_NO;

    ztcmInitSpinLock( &(aApplierMgr->mSubDistSpinLock) );

    /**
     * MsgId array alloc
     */
    aApplierMgr->mMsgIdBuffCnt = ZTC_DEFAULT_MAX_MSG_ACK;
    aApplierMgr->mMsgIdCnt = 0;
    STL_TRY( stlAllocHeap( (void **)&(aApplierMgr->mMsgIdArray),
                           aApplierMgr->mMsgIdBuffCnt * STL_SIZEOF( stlUInt64 ),
                           aErrorStack ) == STL_SUCCESS );
    sState = 1;

    /**
     * Applier Chunk Memeory
     */
    STL_TRY( stlCreateRegionAllocator( &(aApplierMgr->mRegionMem),
                                       ZTC_REGION_INIT_SIZE,
                                       ZTC_REGION_NEXT_SIZE,
                                       aErrorStack ) == STL_SUCCESS );
    sState = 2;

    if( gConfigure.mApplyToFile == STL_FALSE )
    {
        STL_TRY( ztcdCreateConnection( &aApplierMgr->mDbcHandle,
                                       gSlaveMgr->mEnvHandle,
                                       STL_FALSE,
                                       aErrorStack ) == STL_SUCCESS );
        sState = 3;
    }
    
    STL_TRY( stlAllocRegionMem( &(aApplierMgr->mRegionMem),
                                ( STL_SIZEOF( ztcApplierRecItem ) * ZTC_DISTRIBUTOR_SLOT_SIZE ) + 64,
                                (void**)&sPtr,
                                aErrorStack ) == STL_SUCCESS );
    
    aApplierMgr->mItemArr = (void*)STL_ALIGN( (stlUInt64)sPtr, 64 );

    for( sIdx = 0; sIdx < ZTC_DISTRIBUTOR_SLOT_SIZE; sIdx++ )
    {
        sItem = &aApplierMgr->mItemArr[ sIdx ];
        
        STL_RING_INIT_DATALINK( sItem,
                                STL_OFFSETOF( ztcApplierRecItem, mLink ) );
        sItem->mRefCount = 0;
        sItem->mIdx      = sIdx;
    }
    
    if( (sPropagateMode == STL_FALSE) &&
        (gConfigure.mApplyToFile == STL_FALSE) )
    {
        STL_TRY( ztcdSetPropagateMode( aApplierMgr->mDbcHandle,
                                       aErrorStack ) == STL_SUCCESS );
    }

    STL_TRY( ztcmCreateStaticHash( &(aApplierMgr->mTableHash),
                                   ZTC_TABLE_HASH_BUCKET_SIZE,
                                   STL_OFFSETOF( ztcApplierTableInfo, mLink ),
                                   STL_OFFSETOF( ztcApplierTableInfo, mTableId ),
                                   &(gSlaveMgr->mRegionMem),
                                   aErrorStack ) == STL_SUCCESS );

    STL_TRY( stlCreateDynamicAllocator( &aApplierMgr->mLongVaryingMem,
                                        ZTC_LONG_VARYING_MEMORY_INIT_SIZE,
                                        ZTC_LONG_VARYING_MEMORY_NEXT_SIZE,
                                        aErrorStack ) == STL_SUCCESS );   
    sState = 4;

    STL_TRY( ztcaInitializeTableInfo( aApplierMgr,
                                      aErrorStack ) == STL_SUCCESS );

    if( gConfigure.mApplyToFile == STL_FALSE )
    {
        STL_TRY( ztcaInitializeStatement( aApplierMgr,
                                          aErrorStack ) == STL_SUCCESS );
        sState = 5;
    }
    
    STL_TRY( ztcaInitializeBuffer( aApplierMgr,
                                   aErrorStack ) == STL_SUCCESS );
    sState = 6;

    if( gConfigure.mApplyToFile == STL_FALSE )
    {
        STL_TRY( ztcaPrepareNBindParam4Insert( aApplierMgr,
                                               aErrorStack ) == STL_SUCCESS );

        STL_TRY( ztcaPrepareNBindParam4Delete( aApplierMgr,
                                               aErrorStack ) == STL_SUCCESS );
    }

    STL_RING_INIT_HEADLINK( &aApplierMgr->mRecordList,
                            STL_OFFSETOF( ztcApplierRecItem, mLink ) );
    
    if( gConfigure.mApplyToFile == STL_FALSE )
    {
        /**
         * Applier에서 사용할 Statement Handle 할당
         */
        STL_TRY( ztcdSQLAllocHandle( aApplierMgr->mDbcHandle,
                                     &aApplierMgr->mStateStmtHandle,
                                     aErrorStack ) == STL_SUCCESS );
    }
    
    /**
     * for file
     */
    if( gConfigure.mApplyToFile == STL_TRUE )
    {
        STL_TRY( ztcaInitializeFile( aApplierMgr,
                                     &aApplierMgr->mSqlFile,
                                     aErrorStack )
                 == STL_SUCCESS );
        sState = 7;
    }

    STL_TRY( ztcmLogMsg( aErrorStack,
                         "[APPLIER] Initialize Done.\n" ) == STL_SUCCESS );

    /**
     * 초기화가 되었으니 Distributor가 사용할 수 있도록
     */
    STL_TRY( stlReleaseSemaphore( &(gSlaveMgr->mDistbtStartSem),
                                  aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    switch( sState )
    {
        case 7:
            if( gConfigure.mApplyToFile == STL_FALSE )
            {
                STL_TRY( ztcaFinalizeFile( &aApplierMgr->mSqlFile,
                                           aErrorStack )
                         == STL_SUCCESS );
            }
        case 6:
            (void)ztcaFinalizeBuffer( aApplierMgr,
                                      aErrorStack );
        case 5:
            if( gConfigure.mApplyToFile == STL_FALSE )
            {
                (void)ztcaFinalizeStatement( aApplierMgr,
                                             aErrorStack );
            }
        case 4:
            (void)stlDestroyDynamicAllocator( &(aApplierMgr->mLongVaryingMem),
                                              aErrorStack );
        case 3:
            if( gConfigure.mApplyToFile == STL_FALSE )
            {
                (void)ztcdDestroyConnection( aApplierMgr->mDbcHandle,
                                             aErrorStack );
            }
        case 2:
            (void)stlDestroyRegionAllocator( &(aApplierMgr->mRegionMem),
                                             aErrorStack );
        case 1:
            (void)stlFreeHeap( (void *)aApplierMgr->mMsgIdArray );
            aApplierMgr->mMsgIdBuffCnt = 0;
            aApplierMgr->mMsgIdCnt = 0;
            break;
        default:
            break;
    }
    
    ztcmFinSpinLock( &(aApplierMgr->mSubDistSpinLock) );

    return STL_FAILURE;
}


stlStatus ztcaFinalizeApplier( ztcApplierMgr * aApplierMgr,
                               stlErrorStack * aErrorStack )
{
    stlInt32 sState = 7;
    
    if( gConfigure.mApplyToFile == STL_TRUE )
    {
        STL_TRY( ztcaFinalizeFile( &aApplierMgr->mSqlFile,
                                   aErrorStack )
                 == STL_SUCCESS );
    }

    sState = 6;
    if( gConfigure.mApplyToFile == STL_FALSE )
    {
        STL_TRY( SQLFreeHandle( SQL_HANDLE_STMT,
                                aApplierMgr->mStateStmtHandle ) == SQL_SUCCESS );
    }
    
    sState = 5;
    STL_TRY( ztcaFinalizeBuffer( aApplierMgr,
                                 aErrorStack ) == STL_SUCCESS );
    
    if( gConfigure.mApplyToFile == STL_FALSE )
    {
        sState = 4;
        STL_TRY( ztcaFinalizeStatement( aApplierMgr,
                                        aErrorStack ) == STL_SUCCESS );
    }
    
    sState = 3;
    STL_TRY( stlDestroyDynamicAllocator( &(aApplierMgr->mLongVaryingMem),
                                         aErrorStack ) == STL_SUCCESS );
    
    if( gConfigure.mApplyToFile == STL_FALSE )
    {
        sState = 2;
        STL_TRY( ztcdDestroyConnection( aApplierMgr->mDbcHandle,
                                        aErrorStack ) == STL_SUCCESS );
    }
    
    sState = 1;
    STL_TRY( stlDestroyRegionAllocator( &(aApplierMgr->mRegionMem),
                                        aErrorStack ) == STL_SUCCESS );
    
    sState = 0;
    (void)stlFreeHeap( (void *)aApplierMgr->mMsgIdArray );
    aApplierMgr->mMsgIdBuffCnt = 0;
    aApplierMgr->mMsgIdCnt = 0;

    ztcmFinSpinLock( &(aApplierMgr->mSubDistSpinLock) );

    return STL_SUCCESS;
    
    STL_FINISH;
    
    switch( sState )
    {
        case 7:
            if( gConfigure.mApplyToFile == STL_FALSE )
            {
                (void)SQLFreeHandle( SQL_HANDLE_STMT,
                                     aApplierMgr->mStateStmtHandle );
            }
        case 6:
            (void) ztcaFinalizeBuffer( aApplierMgr,
                                       aErrorStack );
        case 5:
            if( gConfigure.mApplyToFile == STL_FALSE )
            {
                (void) ztcaFinalizeStatement( aApplierMgr,
                                              aErrorStack );
            }
        case 4:
            (void) stlDestroyDynamicAllocator( &(aApplierMgr->mLongVaryingMem),
                                               aErrorStack );
        case 3:
            if( gConfigure.mApplyToFile == STL_FALSE )
            {
                (void) ztcdDestroyConnection( aApplierMgr->mDbcHandle,
                                              aErrorStack );
            }
        case 2:
            (void) stlDestroyRegionAllocator( &(aApplierMgr->mRegionMem),
                                              aErrorStack );
        case 1:
            (void)stlFreeHeap( (void *)aApplierMgr->mMsgIdArray );
            aApplierMgr->mMsgIdBuffCnt = 0;
            aApplierMgr->mMsgIdCnt = 0;
        default:
            break;
    }
    
    ztcmFinSpinLock( &(aApplierMgr->mSubDistSpinLock) );

    return STL_FAILURE;
}


stlStatus ztcaInitializeBuffer( ztcApplierMgr * aApplierMgr,
                                stlErrorStack * aErrorStack )
{
    stlInt32       sState      = 0;
    stlInt32       sIdx        = 0;
    ztcChunkItem * sChunkItem  = NULL;
    
    /**
     * Chunk Initialize
     */
    STL_RING_INIT_HEADLINK( &aApplierMgr->mWaitWriteCkList,
                            STL_OFFSETOF( ztcChunkItem, mLink ) );
    
    STL_RING_INIT_HEADLINK( &aApplierMgr->mWriteCkList,
                            STL_OFFSETOF( ztcChunkItem, mLink ) );
    
    aApplierMgr->mWaitWriteState = STL_FALSE;
    
    for( sIdx = 0; sIdx < ZTC_APPLIER_CHUNK_ITEM_COUNT; sIdx++ )
    {
        STL_TRY( stlAllocRegionMem( &aApplierMgr->mRegionMem,
                                    STL_SIZEOF( ztcChunkItem ),
                                    (void**)&sChunkItem,
                                    aErrorStack ) == STL_SUCCESS );    
        
        STL_TRY( stlAllocRegionMem( &aApplierMgr->mRegionMem,
                                    ZTC_APPLIER_CHUNK_ITEM_SIZE,
                                    (void**)&(sChunkItem->mBuffer),
                                    aErrorStack ) == STL_SUCCESS );    
        
        sChunkItem->mWriteIdx = 0;
        sChunkItem->mReadIdx  = 0;
        
        STL_RING_INIT_DATALINK( sChunkItem,
                                STL_OFFSETOF( ztcChunkItem, mLink ) );
        
        /**
         * 처음에는 Distributor에서 사용하도록 Write ChunkList에 넣는다.
         */
        STL_RING_ADD_LAST( &aApplierMgr->mWriteCkList, 
                           sChunkItem );
    }

    ztcmInitSpinLock( &(aApplierMgr->mWriteSpinLock) );
    
    STL_TRY( stlCreateSemaphore( &(aApplierMgr->mWaitWriteSem),
                                 "AWWSem",
                                 0,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_RING_INIT_HEADLINK( &aApplierMgr->mWaitReadCkList,
                            STL_OFFSETOF( ztcChunkItem, mLink ) );
    
    STL_RING_INIT_HEADLINK( &aApplierMgr->mReadCkList,
                            STL_OFFSETOF( ztcChunkItem, mLink ) );
    
    aApplierMgr->mWaitReadState = STL_FALSE;

    ztcmInitSpinLock( &(aApplierMgr->mReadSpinLock) );
    
    STL_TRY( stlCreateSemaphore( &(aApplierMgr->mWaitReadSem),
                                 "ARWSem",
                                 0,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 2;
    
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    switch( sState )
    {
        case 2:
            (void) stlDestroySemaphore( &(aApplierMgr->mWaitReadSem),
                                        aErrorStack );
            
            ztcmFinSpinLock( &(aApplierMgr->mReadSpinLock) );
        case 1:
            (void) stlDestroySemaphore( &(aApplierMgr->mWaitWriteSem),
                                        aErrorStack );
            
            ztcmFinSpinLock(  &(aApplierMgr->mWriteSpinLock) );
        default:
            break;
    }
    
    return STL_FAILURE;
}

stlStatus ztcaFinalizeBuffer( ztcApplierMgr * aApplierMgr,
                              stlErrorStack * aErrorStack )
{
    STL_TRY( stlDestroySemaphore( &(aApplierMgr->mWaitReadSem),
                                  aErrorStack ) == STL_SUCCESS );
    
    ztcmFinSpinLock( &(aApplierMgr->mReadSpinLock) );

    STL_TRY( stlDestroySemaphore( &(aApplierMgr->mWaitWriteSem),
                                  aErrorStack ) == STL_SUCCESS );
    
    ztcmFinSpinLock(  &(aApplierMgr->mWriteSpinLock) );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcaInitializeTableInfo( ztcApplierMgr * aApplierMgr,
                                   stlErrorStack * aErrorStack )
{
    ztcSlaveTableInfo   * sSlaveTbInfo   = NULL;
    ztcApplierTableInfo * sApplierTbInfo = NULL;
    ztcColumnInfo       * sColumnInfo    = NULL;
    ztcColumnValue      * sColumnValue   = NULL;
    stlUInt32             sBucketSeq     = 0;
    
    STL_TRY( ztcmGetFirstHashItem( gSlaveMgr->mTableHash, 
                                   &sBucketSeq, 
                                   (void**)&sSlaveTbInfo, 
                                   aErrorStack ) == STL_SUCCESS );

    while( sSlaveTbInfo != NULL )
    {
        STL_TRY( stlAllocRegionMem( &(aApplierMgr->mRegionMem),
                                    STL_SIZEOF( ztcApplierTableInfo ),
                                     (void**)&sApplierTbInfo,
                                    aErrorStack ) == STL_SUCCESS );
        
        STL_RING_INIT_DATALINK( sApplierTbInfo,
                                STL_OFFSETOF( ztcApplierTableInfo, mLink ) );
        
        sApplierTbInfo->mInfoPtr               = sSlaveTbInfo;
        sApplierTbInfo->mTableId               = sSlaveTbInfo->mTableId;
        sApplierTbInfo->mNeedUpdatePrepare     = STL_TRUE;
        sApplierTbInfo->mHasLongVariableCol    = STL_FALSE;
        sApplierTbInfo->mUpdateLongVariableCol = STL_FALSE;
        
        STL_RING_INIT_HEADLINK( &sApplierTbInfo->mColumnList,
                                STL_OFFSETOF( ztcColumnValue, mLink ) );
        
        /**
         * Column Value Initialize
         */
        STL_RING_FOREACH_ENTRY( &sSlaveTbInfo->mColumnList, sColumnInfo )
        {
            STL_TRY( stlAllocRegionMem( &(aApplierMgr->mRegionMem),
                                        STL_SIZEOF( ztcColumnValue ),
                                        (void**)&sColumnValue,
                                        aErrorStack ) == STL_SUCCESS );
            
            stlMemset( sColumnValue, 0x00, STL_SIZEOF( ztcColumnValue ) );
            
            STL_RING_INIT_DATALINK( sColumnValue,
                                    STL_OFFSETOF( ztcColumnValue, mLink ) );
            
            sColumnValue->mInfoPtr   = sColumnInfo;
            sColumnValue->mIsUpdated = STL_FALSE;
            
            STL_TRY( stlAllocRegionMem( &(aApplierMgr->mRegionMem),
                                        sColumnInfo->mValueMaxSize * aApplierMgr->mArraySize,
                                        (void**)&sColumnValue->mValue,
                                        aErrorStack ) == STL_SUCCESS );
            
            STL_TRY( stlAllocRegionMem( &(aApplierMgr->mRegionMem),
                                        STL_SIZEOF( stlInt64 ) * aApplierMgr->mArraySize,
                                        (void**)&sColumnValue->mValueSize,
                                        aErrorStack ) == STL_SUCCESS );
            
            /**
             * ValueSize를 0으로 세팅해야 함
             * Continuous Column 구성시 초기값을 읽는다.... (VARCHAR TYPE)
             */
            stlMemset( sColumnValue->mValueSize, 0x00, STL_SIZEOF( stlInt64 ) * aApplierMgr->mArraySize );
            
            /**
             * Long Variable Column에서 Length가 반드시 0 으로 초기화 되어야 하기 때문에 Memset 해줘야 한다.
             */
            stlMemset( sColumnValue->mValue, 0x00, sColumnInfo->mValueMaxSize * aApplierMgr->mArraySize );
            
            STL_TRY( stlAllocRegionMem( &(aApplierMgr->mRegionMem),
                                        STL_SIZEOF( SQLLEN ) * aApplierMgr->mArraySize,
                                        (void**)&sColumnValue->mIndicator,
                                        aErrorStack ) == STL_SUCCESS );
        
            if( sColumnInfo->mIsPrimary == STL_TRUE )
            {
                STL_TRY( stlAllocRegionMem( &(aApplierMgr->mRegionMem),
                                            sColumnInfo->mValueMaxSize * aApplierMgr->mArraySize,
                                            (void**)&sColumnValue->mKeyValue,
                                            aErrorStack ) == STL_SUCCESS );
                
                STL_TRY( stlAllocRegionMem( &(aApplierMgr->mRegionMem),
                                            STL_SIZEOF( stlInt64 ) * aApplierMgr->mArraySize,
                                            (void**)&sColumnValue->mKeyValueSize,
                                            aErrorStack ) == STL_SUCCESS );
                
                STL_TRY( stlAllocRegionMem( &(aApplierMgr->mRegionMem),
                                            STL_SIZEOF( SQLLEN ) * aApplierMgr->mArraySize,
                                            (void**)&sColumnValue->mKeyIndicator,
                                            aErrorStack ) == STL_SUCCESS );
            }
            else
            {
                /**
                 * Key값이 아니고 Before Image를 비교해야할 경우 메모리 할당
                 */
                if( ztcmGetUpdateApplyMode( aErrorStack ) >= 1 )
                {
                    STL_TRY( stlAllocRegionMem( &(aApplierMgr->mRegionMem),
                                                sColumnInfo->mValueMaxSize * aApplierMgr->mArraySize,
                                                (void**)&sColumnValue->mBefValue,
                                                aErrorStack ) == STL_SUCCESS );

                    STL_TRY( stlAllocRegionMem( &(aApplierMgr->mRegionMem),
                                                STL_SIZEOF( stlInt64 ) * aApplierMgr->mArraySize,
                                                (void**)&sColumnValue->mBefValueSize,
                                                aErrorStack ) == STL_SUCCESS );

                    STL_TRY( stlAllocRegionMem( &(aApplierMgr->mRegionMem),
                                                STL_SIZEOF( SQLLEN ) * aApplierMgr->mArraySize,
                                                (void**)&sColumnValue->mBefIndicator,
                                                aErrorStack ) == STL_SUCCESS );

                    stlMemset( sColumnValue->mBefValueSize, 0x00, STL_SIZEOF( stlInt64 ) * aApplierMgr->mArraySize );
                    stlMemset( sColumnValue->mBefValue, 0x00, sColumnInfo->mValueMaxSize * aApplierMgr->mArraySize );
                }

                /**
                 * Returning Value와 Before Image를 비교해야할 경우 메모리 할당
                 */
                if( ztcmGetUpdateApplyMode( aErrorStack ) == 2 )
                {
                    STL_TRY( stlAllocRegionMem( &(aApplierMgr->mRegionMem),
                                                sColumnInfo->mValueMaxSize * aApplierMgr->mArraySize,
                                                (void**)&sColumnValue->mRetValue,
                                                aErrorStack ) == STL_SUCCESS );

                    STL_TRY( stlAllocRegionMem( &(aApplierMgr->mRegionMem),
                                                STL_SIZEOF( stlInt64 ) * aApplierMgr->mArraySize,
                                                (void**)&sColumnValue->mRetValueSize,
                                                aErrorStack ) == STL_SUCCESS );

                    STL_TRY( stlAllocRegionMem( &(aApplierMgr->mRegionMem),
                                                STL_SIZEOF( SQLLEN ) * aApplierMgr->mArraySize,
                                                (void**)&sColumnValue->mRetIndicator,
                                                aErrorStack ) == STL_SUCCESS );

                    stlMemset( sColumnValue->mRetValueSize, 0x00, STL_SIZEOF( stlInt64 ) * aApplierMgr->mArraySize );
                    stlMemset( sColumnValue->mRetValue, 0x00, sColumnInfo->mValueMaxSize * aApplierMgr->mArraySize );
                }

            }

            /**
             * 실제로는 모든 컬럼에 mStrValue를 할당한다.
             * 이 값은 Execute 에러 발생시 해당 값을 스트링 형태로 로그파일에 저장하기 위해서 사용된다.
             * mStrValue는 Insert 시에 사용되며, mKeyStrValue는 Delete, Update시에 사용한다.
             * TODO : Insert 시에도 mKeyStrValue를 사용하도록 하면 메모리 공간을 절약할 수 있다.
             */
            if( sColumnInfo->mStrValueMaxSize > 0 )
            {
                STL_TRY( stlAllocRegionMem( &(aApplierMgr->mRegionMem),
                                            sColumnInfo->mStrValueMaxSize * aApplierMgr->mArraySize,
                                            (void**)&sColumnValue->mStrValue,
                                            aErrorStack ) == STL_SUCCESS );
            
                if( sColumnInfo->mIsPrimary == STL_TRUE)
                {
                    STL_TRY( stlAllocRegionMem( &(aApplierMgr->mRegionMem),
                                                sColumnInfo->mStrValueMaxSize * aApplierMgr->mArraySize,
                                                (void**)&sColumnValue->mKeyStrValue,
                                                aErrorStack ) == STL_SUCCESS );
                }

                if( ztcmGetUpdateApplyMode( aErrorStack ) >= 1 )
                {
                    /**
                     * Update시 Before Image를 비교하기 위한 공간을 할당한다.
                     * TODO: 실제로는 Interval datatype에서만 사용한다. 나중에 변경해야 한다.
                     */
                    STL_TRY( stlAllocRegionMem( &(aApplierMgr->mRegionMem),
                                                sColumnInfo->mStrValueMaxSize * aApplierMgr->mArraySize,
                                                (void**)&sColumnValue->mBefStrValue,
                                                aErrorStack ) == STL_SUCCESS );
                }
                if( ztcmGetUpdateApplyMode( aErrorStack ) == 2 )
                {
                    STL_TRY( stlAllocRegionMem( &(aApplierMgr->mRegionMem),
                                                sColumnInfo->mStrValueMaxSize * aApplierMgr->mArraySize,
                                                (void**)&sColumnValue->mRetStrValue,
                                                aErrorStack ) == STL_SUCCESS );
                }
            }
            
            /**
             * Execute 이후에 LongVariableMem을 초기화하기 위하여 사용된다.
             * 해당 값이 TRUE일 경우에는 LongVariable Column이 포함되어 있다는 뜻이다.
             */
            if( sColumnInfo->mDataTypeId == DTL_TYPE_LONGVARCHAR ||
                sColumnInfo->mDataTypeId == DTL_TYPE_LONGVARBINARY )
            {
                sApplierTbInfo->mHasLongVariableCol = STL_TRUE;
            }
            
            STL_RING_ADD_LAST( &sApplierTbInfo->mColumnList, 
                               sColumnValue );
        }
        
        STL_TRY( ztcmInsertStaticHash( aApplierMgr->mTableHash,
                                       ztcmSlaveTableHashFunc,
                                       sApplierTbInfo,
                                       aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztcmGetNextHashItem( gSlaveMgr->mTableHash, 
                                      &sBucketSeq, 
                                      (void**)&sSlaveTbInfo, 
                                      aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcaPrepareNBindParam4Delete( ztcApplierMgr * aApplierMgr,
                                        stlErrorStack * aErrorStack )
{
    ztcApplierTableInfo  * sTableInfo  = NULL;
    ztcStmtInfo          * sStmtInfo   = NULL;
    ztcColumnValue       * sColumnInfo = NULL;
    stlUInt32              sBucketSeq  = 0;
    stlUInt16              sColumnIdx  = 1; 
    stlChar                sPrepareStr[ ZTC_COMMAND_BUFFER_SIZE ];
    
    STL_TRY( ztcmGetFirstHashItem( aApplierMgr->mTableHash,
                                   &sBucketSeq, 
                                   (void**)&sTableInfo, 
                                   aErrorStack ) == STL_SUCCESS );
    
    while( sTableInfo != NULL )
    {
        sStmtInfo = &sTableInfo->mStmtInfo;
        
        /**
         * Prepare Insert Statement
         */
        stlMemset( sPrepareStr, 0x00, STL_SIZEOF( sPrepareStr ) );
        
        stlSnprintf( sPrepareStr,
                     STL_SIZEOF( sPrepareStr ),
                     "DELETE /*+ INDEX(\"%s\", %s) */ FROM \"%s\".\"%s\" WHERE",
                     sTableInfo->mInfoPtr->mSlaveTable,
                     sTableInfo->mInfoPtr->mPKIndexName,
                     sTableInfo->mInfoPtr->mSlaveSchema, 
                     sTableInfo->mInfoPtr->mSlaveTable );
        
        sColumnIdx = 0;
        
        STL_RING_FOREACH_ENTRY( &sTableInfo->mColumnList, sColumnInfo )
        {
            if( sColumnInfo->mInfoPtr->mIsPrimary == STL_TRUE )
            {
                if( sColumnIdx == 0 )
                {
                    stlSnprintf( sPrepareStr,
                                 STL_SIZEOF( sPrepareStr ),
                                 "%s \"%s\"=?", sPrepareStr, sColumnInfo->mInfoPtr->mColumnName );
                    
                }
                else
                {
                    stlSnprintf( sPrepareStr,
                                 STL_SIZEOF( sPrepareStr ),
                                 "%s AND \"%s\"=?", sPrepareStr, sColumnInfo->mInfoPtr->mColumnName );
                }
                sColumnIdx++;
            }
        }
        
        STL_TRY( ztcdSQLPrepare( sStmtInfo->mDeleteStmt,
                                 (SQLCHAR*)sPrepareStr,
                                 SQL_NTS,
                                 aErrorStack ) == STL_SUCCESS );
        sColumnIdx = 1;
        
        /**
         * BindParamenter
         */ 
        STL_RING_FOREACH_ENTRY( &sTableInfo->mColumnList, sColumnInfo )
        {
            if( sColumnInfo->mInfoPtr->mIsPrimary != STL_TRUE )
            {
                continue;
            }
            
            switch( sColumnInfo->mInfoPtr->mDataTypeId )
            {
                case DTL_TYPE_CHAR:
                case DTL_TYPE_VARCHAR:
                case DTL_TYPE_BINARY:
                case DTL_TYPE_VARBINARY:
                case DTL_TYPE_BOOLEAN:
                case DTL_TYPE_NATIVE_SMALLINT:
                case DTL_TYPE_NATIVE_INTEGER:
                case DTL_TYPE_NATIVE_BIGINT:
                case DTL_TYPE_NATIVE_REAL:
                case DTL_TYPE_NATIVE_DOUBLE:
                case DTL_TYPE_FLOAT:
                case DTL_TYPE_NUMBER:
                case DTL_TYPE_DATE:
                case DTL_TYPE_TIME:
                case DTL_TYPE_TIME_WITH_TIME_ZONE:
                case DTL_TYPE_TIMESTAMP:
                case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
                {
                    STL_TRY( ztcdSQLBindParameter( sStmtInfo->mDeleteStmt,
                                                   sColumnIdx,
                                                   SQL_PARAM_INPUT,
                                                   sColumnInfo->mInfoPtr->mValueType,
                                                   sColumnInfo->mInfoPtr->mParameterType,
                                                   sColumnInfo->mInfoPtr->mStringLengthUnit,
                                                   sColumnInfo->mInfoPtr->mColumnSize,
                                                   sColumnInfo->mInfoPtr->mDecimalDigits,
                                                   sColumnInfo->mKeyValue,
                                                   sColumnInfo->mInfoPtr->mBufferLength,
                                                   sColumnInfo->mKeyIndicator,
                                                   aErrorStack ) == STL_SUCCESS );
                                                       
                    break;
                }
                case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
                case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
                {
                    STL_TRY( ztcdSQLBindParameter( sStmtInfo->mDeleteStmt,
                                                   sColumnIdx,
                                                   SQL_PARAM_INPUT,
                                                   sColumnInfo->mInfoPtr->mValueType,
                                                   sColumnInfo->mInfoPtr->mParameterType,
                                                   sColumnInfo->mInfoPtr->mStringLengthUnit,
                                                   sColumnInfo->mInfoPtr->mColumnSize,
                                                   sColumnInfo->mInfoPtr->mDecimalDigits,
                                                   sColumnInfo->mKeyStrValue,
                                                   sColumnInfo->mInfoPtr->mBufferLength,
                                                   sColumnInfo->mKeyIndicator,
                                                   aErrorStack ) == STL_SUCCESS );
                    break;
                }
                case DTL_TYPE_LONGVARCHAR:
                case DTL_TYPE_LONGVARBINARY:
                {
                    /** Supported But Not Primary key type.*/
                    STL_DASSERT( 0 );
                }
                case DTL_TYPE_DECIMAL:
                case DTL_TYPE_BLOB:
                case DTL_TYPE_CLOB:
                {
                    /** Not Supported Yet.*/
                    STL_DASSERT( 0 );
                    break;
                }
                default:
                    break;
            }            
                
            sColumnIdx++;
        } //end of FOREACH
        
        STL_TRY( ztcmGetNextHashItem( aApplierMgr->mTableHash,
                                      &sBucketSeq, 
                                      (void**)&sTableInfo, 
                                      aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;   
}

stlStatus ztcaPrepareNBindParam4Insert( ztcApplierMgr * aApplierMgr,
                                        stlErrorStack * aErrorStack )
{
    ztcApplierTableInfo  * sTableInfo  = NULL;
    ztcStmtInfo          * sStmtInfo   = NULL;
    ztcColumnValue       * sColumnInfo = NULL;
    stlUInt32              sBucketSeq  = 0;
    stlUInt16              sColumnIdx  = 1; 
    stlInt32               sIdx        = 0;
    stlChar                sPrepareStr[ ZTC_COMMAND_BUFFER_SIZE ] = {0,};
    
    STL_TRY( ztcmGetFirstHashItem( aApplierMgr->mTableHash, 
                                   &sBucketSeq, 
                                   (void**)&sTableInfo, 
                                   aErrorStack ) == STL_SUCCESS );
    
    while( sTableInfo != NULL )
    {
        
        sStmtInfo = &sTableInfo->mStmtInfo;
        
        /**
         * Prepare Insert Statement
         */
        stlMemset( sPrepareStr, 0x00, STL_SIZEOF( sPrepareStr ) );
        
        stlSnprintf( sPrepareStr,
                     ZTC_COMMAND_BUFFER_SIZE,
                     "INSERT INTO \"%s\".\"%s\" VALUES (",
                     sTableInfo->mInfoPtr->mSlaveSchema, 
                     sTableInfo->mInfoPtr->mSlaveTable );
        
        for( sIdx = 0; sIdx < sTableInfo->mInfoPtr->mColumnCount; sIdx++ )
        {
            if( sIdx + 1 == sTableInfo->mInfoPtr->mColumnCount )
            {
                stlStrcat( sPrepareStr, " ? " );
            }
            else
            {
                stlStrcat( sPrepareStr, " ?," );
            }
            
        }
        
        stlStrcat( sPrepareStr, ")" );
        
        STL_TRY( ztcdSQLPrepare( sStmtInfo->mInsertStmt,
                                 (SQLCHAR*)sPrepareStr,
                                 SQL_NTS,
                                 aErrorStack ) == STL_SUCCESS );
        sColumnIdx = 1;
        
        /**
         * BindParamenter
         */ 
        STL_RING_FOREACH_ENTRY( &sTableInfo->mColumnList, sColumnInfo )
        {
            switch( sColumnInfo->mInfoPtr->mDataTypeId )
            {
                case DTL_TYPE_CHAR:
                case DTL_TYPE_VARCHAR:
                case DTL_TYPE_BINARY:
                case DTL_TYPE_VARBINARY:
                case DTL_TYPE_BOOLEAN:
                case DTL_TYPE_NATIVE_SMALLINT:
                case DTL_TYPE_NATIVE_INTEGER:
                case DTL_TYPE_NATIVE_BIGINT:
                case DTL_TYPE_NATIVE_REAL:
                case DTL_TYPE_NATIVE_DOUBLE:
                case DTL_TYPE_FLOAT:
                case DTL_TYPE_NUMBER:
                case DTL_TYPE_LONGVARCHAR:
                case DTL_TYPE_LONGVARBINARY:                    
                case DTL_TYPE_DATE:
                case DTL_TYPE_TIME:
                case DTL_TYPE_TIME_WITH_TIME_ZONE:
                case DTL_TYPE_TIMESTAMP:
                case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
                {
                    STL_TRY( ztcdSQLBindParameter( sStmtInfo->mInsertStmt,
                                                   sColumnIdx,
                                                   SQL_PARAM_INPUT,
                                                   sColumnInfo->mInfoPtr->mValueType,
                                                   sColumnInfo->mInfoPtr->mParameterType,
                                                   sColumnInfo->mInfoPtr->mStringLengthUnit,
                                                   sColumnInfo->mInfoPtr->mColumnSize,
                                                   sColumnInfo->mInfoPtr->mDecimalDigits,
                                                   sColumnInfo->mValue,
                                                   sColumnInfo->mInfoPtr->mBufferLength,
                                                   sColumnInfo->mIndicator,
                                                   aErrorStack ) == STL_SUCCESS );
                    break;
                }
                case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
                case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
                {
                    STL_TRY( ztcdSQLBindParameter( sStmtInfo->mInsertStmt,
                                                   sColumnIdx,
                                                   SQL_PARAM_INPUT,
                                                   sColumnInfo->mInfoPtr->mValueType,
                                                   sColumnInfo->mInfoPtr->mParameterType,
                                                   sColumnInfo->mInfoPtr->mStringLengthUnit,
                                                   sColumnInfo->mInfoPtr->mColumnSize,
                                                   sColumnInfo->mInfoPtr->mDecimalDigits,
                                                   sColumnInfo->mStrValue,
                                                   sColumnInfo->mInfoPtr->mBufferLength,
                                                   sColumnInfo->mIndicator,
                                                   aErrorStack ) == STL_SUCCESS );
                    break;
                }
                case DTL_TYPE_DECIMAL:    
                case DTL_TYPE_BLOB:
                case DTL_TYPE_CLOB:
                {
                    /** Not Supported Yet.*/
                    ztcmLogMsg( aErrorStack, "assert(Not Supported Yet) \n" );
                    STL_DASSERT( 0 );

                    break;
                }
                default:
                    break;
            }            
                
            sColumnIdx++;
        } //end of FOREACH
        
        STL_TRY( ztcmGetNextHashItem( aApplierMgr->mTableHash,
                                      &sBucketSeq, 
                                      (void**)&sTableInfo, 
                                      aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztcaInitializeStatement( ztcApplierMgr * aApplierMgr,
                                   stlErrorStack * aErrorStack )
{
    ztcApplierTableInfo * sTableInfo  = NULL;
    ztcStmtInfo         * sStmtInfo   = NULL;
    stlUInt32             sBucketSeq  = 0;
    
    STL_TRY( ztcmGetFirstHashItem( aApplierMgr->mTableHash, 
                                   &sBucketSeq, 
                                   (void**)&sTableInfo, 
                                   aErrorStack ) == STL_SUCCESS );
    
    while( sTableInfo != NULL )
    {
        sStmtInfo = &sTableInfo->mStmtInfo;

        STL_TRY( ztcdInitializeApplierStatement( aApplierMgr,
                                                 sStmtInfo, 
                                                 aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztcmGetNextHashItem( aApplierMgr->mTableHash,
                                      &sBucketSeq, 
                                      (void**)&sTableInfo, 
                                      aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztcaFinalizeStatement( ztcApplierMgr * aApplierMgr,
                                 stlErrorStack * aErrorStack )
{
    ztcApplierTableInfo * sTableInfo  = NULL;
    ztcStmtInfo         * sStmtInfo   = NULL;
    stlUInt32             sBucketSeq  = 0;
    
    STL_TRY( ztcmGetFirstHashItem( aApplierMgr->mTableHash, 
                                   &sBucketSeq, 
                                   (void**)&sTableInfo, 
                                   aErrorStack ) == STL_SUCCESS );
    
    while( sTableInfo != NULL )
    {
        sStmtInfo = &sTableInfo->mStmtInfo;
        
        STL_DASSERT( sStmtInfo != NULL );
        
        STL_TRY( ztcdFinalizeApplierStatement( sStmtInfo, 
                                               aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztcmGetNextHashItem( aApplierMgr->mTableHash,
                                      &sBucketSeq, 
                                      (void**)&sTableInfo, 
                                      aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztcaFindApplierTableInfo( ztcApplierMgr         * aApplierMgr,
                                    ztcApplierTableInfo  ** aTableInfo,
                                    stlInt32                aId,
                                    stlErrorStack         * aErrorStack )
{
    ztcApplierTableInfo  * sTableInfo = NULL;
    
    STL_TRY( ztcmFindStaticHash( aApplierMgr->mTableHash,
                                 ztcmSlaveTableHashFunc,
                                 ztcmSlaveTableHashCompareFunc,
                                 (void*)&aId,
                                 (void**)&sTableInfo,
                                 aErrorStack ) == STL_SUCCESS );
    
    *aTableInfo = sTableInfo;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztcaChangeValueStr( stlChar       * aValue,
                              stlSize         aValueBufSize,
                              stlSize       * aValueLen,
                              stlErrorStack * aErrorStack )
{
    stlInt32    sIdx = 0;
    stlSize     sLength = 0;

    /**
     * char 계열, binary 계열 이라도 single quotation 붙이지 않는다.
     * 중간에 double quotation이 있는경우 double quotation 두개로 치환한다.
     *
     * data중에 double quotation이 많으면 buffer가 부족할수 있다.
     * 그래서 assert대신 error처리 했음.
     */

    sLength = stlStrlen( aValue );

    /**
     *  double quotation이 있는경우 double quotation 두개로 치환한다.
     */
    for( sIdx = 0; sIdx < sLength; sIdx++ )
    {
        if( aValue[sIdx] == '\"' )
        {
            STL_TRY_THROW( sLength + 1 < aValueBufSize, RAM_ERR_BUFFER_OVERFLOW );

            stlMemmove( &aValue[sIdx + 1], &aValue[sIdx], sLength - sIdx );
            sLength += 1;

            /* 1byte를 추가했음으로 sIdx도 하나 더한다 */
            sIdx += 1;
        }
    }

    *aValueLen = sLength;

    return STL_SUCCESS;

    STL_CATCH( RAM_ERR_BUFFER_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INTERNAL_ERROR,
                      "ztcaChangeValueStr() buffer overflow",
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztcaChangeDataFile( ztcApplierMgr  * aApplierMgr,
                              ztcSqlFile     * aSqlFile,
                              stlErrorStack  * aErrorStack )
{
    stlChar     sFileName[STL_MAX_FILE_PATH_LENGTH];
    stlInt32    sLength;

    aSqlFile->mFileSeq += 1;
    aSqlFile->mFileOffset = 0;

    /**
     * aFileSequence의 파일을 생성한다.
     */
    STL_TRY( ztcmGetSqlFileName( sFileName,
                                 STL_SIZEOF(sFileName),
                                 aErrorStack )
             == STL_SUCCESS );

    sLength = stlStrlen( sFileName );

    sLength += stlSnprintf( sFileName + sLength,
                            STL_SIZEOF(sFileName) - sLength,
                            "_%ld.dat",
                            aSqlFile->mFileSeq );

    STL_DASSERT( sLength + 1 < STL_SIZEOF( sFileName ) );

    STL_TRY( stlCloseFile( &aSqlFile->mDataFile, aErrorStack ) == STL_SUCCESS );

    STL_TRY( stlOpenFile( &aSqlFile->mDataFile,
                          sFileName,
                          STL_FOPEN_CREATE | STL_FOPEN_TRUNCATE | STL_FOPEN_WRITE | STL_FOPEN_BINARY,
                          STL_FPERM_OS_DEFAULT,
                          aErrorStack ) == STL_SUCCESS );

    STL_TRY( ztcaWriteInfoDataFile( aApplierMgr, aSqlFile, aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;


    return STL_FAILURE;
}


stlStatus ztcaWriteInfoDataFile( ztcApplierMgr  * aApplierMgr,
                                 ztcSqlFile     * aSqlFile,
                                 stlErrorStack  * aErrorStack )
{
    stlInt32    sLength;
    stlChar     sLine[256];
    stlChar   * sVer = ZTC_FILE_VERSION;
    stlExpTime  sExpTime;
    stlTime     sTime = stlNow();
    ztcApplierTableInfo  * sTableInfo  = NULL;
    ztcColumnValue       * sColumnInfo = NULL;
    stlUInt32              sBucketSeq  = 0;

    stlMakeExpTimeByLocalTz( &sExpTime, sTime);

    /**
     * version
     */
    sLength = stlSnprintf( sLine,
                           STL_SIZEOF( sLine ),
                           ",Version,%d,%d,%d,%d,%d,%d,%d,%d\n",
                           sVer[0], sVer[1], sVer[2], sVer[3],
                           sVer[4], sVer[5], sVer[6], sVer[7] );

    STL_DASSERT( sLength + 1 < STL_SIZEOF( sLine ) );

    STL_TRY( ztcaWriteDataFile( aApplierMgr,
                                aSqlFile,
                                sLine,
                                aErrorStack ) == STL_SUCCESS );

    /**
     * date, time
     */
    sLength = stlSnprintf( sLine,
                           STL_SIZEOF( sLine ),
                           ",Date,%04d/%02d/%02d %02d:%02d:%02d\n",
                           sExpTime.mYear + 1900, sExpTime.mMonth + 1, sExpTime.mDay,
                           sExpTime.mHour, sExpTime.mMinute, sExpTime.mSecond );

    STL_DASSERT( sLength + 1 < STL_SIZEOF( sLine ) );

    STL_TRY( ztcaWriteDataFile( aApplierMgr,
                                aSqlFile,
                                sLine,
                                aErrorStack ) == STL_SUCCESS );

    /**
     * table info
     */
    STL_TRY( ztcmGetFirstHashItem( aApplierMgr->mTableHash,
                                   &sBucketSeq,
                                   (void**)&sTableInfo,
                                   aErrorStack ) == STL_SUCCESS );

    while( sTableInfo != NULL )
    {
        sLength = stlSnprintf( sLine,
                               STL_SIZEOF( sLine ),
                               "I,Table,%s,%s\n",
                               sTableInfo->mInfoPtr->mSlaveSchema,
                               sTableInfo->mInfoPtr->mSlaveTable );

        STL_DASSERT( sLength + 1 < STL_SIZEOF( sLine ) );

        STL_TRY( ztcaWriteDataFile( aApplierMgr,
                                    aSqlFile,
                                    sLine,
                                    aErrorStack ) == STL_SUCCESS );

        STL_RING_FOREACH_ENTRY( &sTableInfo->mColumnList, sColumnInfo )
        {
            sLength = stlSnprintf( sLine,
                                   STL_SIZEOF( sLine ),
                                   "I,Column,%s,%s\n",
                                   sColumnInfo->mInfoPtr->mColumnName,
                                   sColumnInfo->mInfoPtr->mDataTypeStr );

            STL_DASSERT( sLength + 1 < STL_SIZEOF( sLine ) );

            STL_TRY( ztcaWriteDataFile( aApplierMgr,
                                        aSqlFile,
                                        sLine,
                                        aErrorStack ) == STL_SUCCESS );
        }

        STL_TRY( ztcmGetNextHashItem( aApplierMgr->mTableHash,
                                      &sBucketSeq,
                                      (void**)&sTableInfo,
                                      aErrorStack ) == STL_SUCCESS );
    }

    STL_TRY( ztcaWriteCtrlFile( aSqlFile,
                                aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    /**
     * release에서도 compile warning 안나도록
     */
    if( sLength == 0 )
    {
    }

    return STL_FAILURE;
}



stlStatus ztcaInitializeFile( ztcApplierMgr  * aApplierMgr,
                              ztcSqlFile     * aSqlFile,
                              stlErrorStack  * aErrorStack )
{
    stlInt32    i;
    stlChar     sFileName[STL_MAX_FILE_PATH_LENGTH];
    stlInt32    sLength;
    stlInt32    sState = 0;
    stlBool     sExistCtrlFile = STL_FALSE; /* 기존에 control 파일 하나라도 있었으면 STL_TRUE */

    /**
     * control file open
     */
    for( i = 0; i < ZTC_CONTROL_FILE_COUNT; i++ )
    {
        aSqlFile->mCtrlFile[i].mFileDesc = -1;
    }

    for( i = 0; i < ZTC_CONTROL_FILE_COUNT; i++ )
    {
        STL_TRY( ztcmGetSqlFileName( sFileName,
                                     STL_SIZEOF(sFileName),
                                     aErrorStack )
                 == STL_SUCCESS );

        sLength = stlStrlen( sFileName );

        sLength += stlSnprintf( sFileName + sLength,
                                STL_SIZEOF(sFileName) - sLength,
                                "_%d.ctrl",
                                i );

        STL_DASSERT( sLength + 1 < STL_SIZEOF( sFileName ) );

        if( sExistCtrlFile == STL_FALSE )
        {
            STL_TRY( stlExistFile( sFileName,
                                   &sExistCtrlFile,
                                   aErrorStack ) == STL_SUCCESS );
        }

        /**
         * control file open or create
         */
        STL_TRY( stlOpenFile( &aSqlFile->mCtrlFile[i],
                              sFileName,
                              STL_FOPEN_CREATE | STL_FOPEN_READ | STL_FOPEN_WRITE | STL_FOPEN_BINARY,
                              STL_FPERM_OS_DEFAULT,
                              aErrorStack ) == STL_SUCCESS );
        /**
         * file open하나라도 성공하면 sState = 1
         */
        sState = 1;
    }

    if( sExistCtrlFile == STL_TRUE )
    {
        /**
         * 기존 control file이 하나라도 있으면
         * get data file info
         */
        STL_TRY( ztcaReadCtrlFile( aSqlFile,
                                   aErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        /**
         * control파일 두개를 모두 생성한 경우이면 file seq, offset을 0으로 초기화 한다.
         */
        aSqlFile->mFileSeq = 0;
        aSqlFile->mFileOffset = 0;
    }

    /**
     * data file open
     */
    STL_TRY( ztcmGetSqlFileName( sFileName,
                                 STL_SIZEOF(sFileName),
                                 aErrorStack )
             == STL_SUCCESS );

    sLength = stlStrlen( sFileName );

    sLength += stlSnprintf( sFileName + sLength,
                            STL_SIZEOF(sFileName) - sLength,
                            "_%d.dat",
                            aSqlFile->mFileSeq );

    STL_DASSERT( sLength + 1 < STL_SIZEOF( sFileName ) );

    /**
     * data file open or create
     */
    STL_TRY( stlOpenFile( &aSqlFile->mDataFile,
                          sFileName,
                          STL_FOPEN_CREATE | STL_FOPEN_WRITE | STL_FOPEN_BINARY,
                          STL_FPERM_OS_DEFAULT,
                          aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( stlSeekFile( &aSqlFile->mDataFile,
                          STL_FSEEK_SET,
                          &aSqlFile->mFileOffset,
                          aErrorStack ) == STL_SUCCESS );
    STL_TRY( ztcaWriteInfoDataFile( aApplierMgr, aSqlFile, aErrorStack ) == STL_SUCCESS );

    aSqlFile->mMsgAllocSize = ZTC_TRACE_BUFFER_SIZE * 1024;

    STL_TRY( stlAllocHeap( (void**)&aSqlFile->mMsgBuffer,
                           aSqlFile->mMsgAllocSize,
                           aErrorStack ) == STL_SUCCESS );
    sState = 3;

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)stlCloseFile( &aSqlFile->mDataFile, aErrorStack );
        case 1:
            for( i = 0; i < ZTC_CONTROL_FILE_COUNT; i++ )
            {
                if( aSqlFile->mCtrlFile[i].mFileDesc != -1 )
                {
                    (void)stlCloseFile( &aSqlFile->mCtrlFile[i], aErrorStack );
                }
            }

        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus ztcaFinalizeFile( ztcSqlFile     * aSqlFile,
                            stlErrorStack  * aErrorStack )
{
    stlInt32 i;

    for( i = 0; i < ZTC_CONTROL_FILE_COUNT; i++ )
    {
        (void)stlCloseFile( &aSqlFile->mCtrlFile[i], aErrorStack );
    }

    (void)stlCloseFile( &aSqlFile->mDataFile, aErrorStack );

    return STL_SUCCESS;
}

stlStatus ztcaWriteDataFile( ztcApplierMgr  * aApplierMgr,
                             ztcSqlFile     * aSqlFile,
                             stlChar        * aMsgBuffer,
                             stlErrorStack  * aErrorStack )
{
    stlInt64    sMaxFileSize;

    STL_DASSERT( aSqlFile->mDataFile.mFileDesc != -1 );

    /* M byte를 byte로 변경 */
    sMaxFileSize = ztcmGetSqlFileSize() * 1024 * 1024;

    /**
     * file size가 최대 이면 다음 파일로 변경
     * 파일 초기화
     */
    if( aSqlFile->mFileOffset + stlStrlen( aMsgBuffer ) >= sMaxFileSize )
    {
        STL_TRY( ztcaChangeDataFile( aApplierMgr,
                                     aSqlFile,
                                     aErrorStack )
                         == STL_SUCCESS );
    }

    /**
     * file에 write
     */
    STL_TRY( stlWriteFile( &aSqlFile->mDataFile,
                           aMsgBuffer,
                           stlStrlen( aMsgBuffer ),
                           NULL,
                           aErrorStack ) == STL_SUCCESS );
    aSqlFile->mFileOffset += stlStrlen( aMsgBuffer );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztcaWriteCtrlFile( ztcSqlFile     * aSqlFile,
                             stlErrorStack  * aErrorStack )
{
    stlInt32        sIdx;
    stlChar         sData[32];
    stlUInt32       sCalcCrc32 = 0;
    stlOffset       sStart = 0;
    stlInt32        i;
    stlUInt64       sTempValue;

    sIdx = 0;

    stlMemcpy( &sData[sIdx], ZTC_FILE_VERSION, 8 );
    sIdx += 8;

    /* crc 4byte */
    sIdx += 4;

    /* reserved 4byte */
    stlMemset( &sData[sIdx], 0x00, 4 );
    sIdx += 4;

    STL_DASSERT( STL_SIZEOF(stlOffset) == 8 );

    sTempValue = ZTC_TO_NETWORK_ENDIAN_64( aSqlFile->mFileSeq );
    stlMemcpy( &sData[sIdx], &sTempValue, 8 );
    sIdx += 8;

    sTempValue = ZTC_TO_NETWORK_ENDIAN_64( aSqlFile->mFileOffset );
    stlMemcpy( &sData[sIdx], &sTempValue, 8 );
    sIdx += 8;

    STL_TRY( stlCrc32( sData + 16, sIdx - 16, &sCalcCrc32, aErrorStack ) == STL_SUCCESS );

    sCalcCrc32 = stlToNetLong( sCalcCrc32 );
    stlMemcpy( &sData[8], &sCalcCrc32, 4 );

    for( i = 0; i < ZTC_CONTROL_FILE_COUNT; i++ )
    {
        sStart = 0;

        STL_TRY( stlSeekFile( &aSqlFile->mCtrlFile[i],
                              STL_FSEEK_SET,
                              &sStart,
                              aErrorStack ) == STL_SUCCESS );

        STL_TRY( stlWriteFile( &aSqlFile->mCtrlFile[i],
                               sData,
                               sIdx,
                               NULL,
                               aErrorStack ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztcaReadCtrlFile( ztcSqlFile     * aSqlFile,
                            stlErrorStack  * aErrorStack )
{
    stlInt32        i;
    stlInt32        sIdx;
    stlSize         sLength;
    stlChar         sData[32];
    stlUInt32       sCalcCrc32 = 0;
    stlUInt8        sVersion[8];
    stlUInt32       sRecvCrc32 = 0;
    stlOffset       sStart = 0;
    stlUInt64       sTempValue;

    for( i = 0; i < ZTC_CONTROL_FILE_COUNT; i++ )
    {
        sStart = 0;
        STL_TRY( stlSeekFile( &aSqlFile->mCtrlFile[i],
                              STL_FSEEK_SET,
                              &sStart,
                              aErrorStack )
                 == STL_SUCCESS );

        /** 파일 header write */
        if( stlReadFile( &aSqlFile->mCtrlFile[i],
                         sData,
                         STL_SIZEOF( sData ),
                         &sLength,
                         aErrorStack ) != STL_SUCCESS )
        {
            (void)stlPopError( aErrorStack );

            continue;
        }

        if( sLength != STL_SIZEOF( sData ) )
        {
            continue;
        }

        sIdx = 0;

        stlMemcpy( sVersion, &sData[sIdx], 8 );
        sIdx += 8;
        if( stlMemcmp( sVersion, ZTC_FILE_VERSION, 8 ) != 0 )
        {
            continue;
        }

        stlMemcpy( &sRecvCrc32, &sData[sIdx], 4 );
        sIdx += 4;

        sRecvCrc32 = stlToHostLong( sRecvCrc32 );

        /* reserved 4byte */
        sIdx += 4;

        stlMemcpy( &sTempValue, &sData[sIdx], 8 );
        sIdx += 8;

        aSqlFile->mFileSeq = ZTC_TO_HOST_ENDIAN_64( sTempValue );

        STL_DASSERT( STL_SIZEOF(stlOffset) == 8 );
        stlMemcpy( &sTempValue, &sData[sIdx], 8 );
        sIdx += 8;
        aSqlFile->mFileOffset = ZTC_TO_HOST_ENDIAN_64( sTempValue );

        sCalcCrc32 = 0;
        STL_TRY( stlCrc32( sData + 16, sIdx - 16, &sCalcCrc32, aErrorStack ) == STL_SUCCESS );

        if( sRecvCrc32 != sCalcCrc32 )
        {
            ztcmLogMsg( aErrorStack,
                        "control file [%d] crc error!",
                        i);

            continue;
        }

        break;
    }

    STL_TRY_THROW( i < ZTC_CONTROL_FILE_COUNT, RAM_ERR_FILE_ERROR );

    return STL_SUCCESS;

    STL_CATCH( RAM_ERR_FILE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_CONTROL_FILE,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}



stlStatus ztcaInsertToFile( ztcApplierMgr        * aApplierMgr,
                            ztcApplierTableInfo  * aTableInfo,
                            stlErrorStack        * aErrorStack )
{
    ztcColumnValue * sColumnValue = NULL;
    dtlDataValue     sDataValue;    //for Conversion.
    stlInt64         sLength;
    stlChar        * sMsgBuffer = NULL;
    stlInt64         sMsgAllocSize = ZTC_TRACE_BUFFER_SIZE;
    stlSize          sIndex = 0;
    stlSize          sValueLen;

    STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnValue )
    {
        sMsgAllocSize += sColumnValue->mInfoPtr->mStrValueMaxSize;
    }

    if( sMsgAllocSize > aApplierMgr->mSqlFile.mMsgAllocSize )
    {
        stlFreeHeap( aApplierMgr->mSqlFile.mMsgBuffer );
        aApplierMgr->mSqlFile.mMsgBuffer = NULL;

        aApplierMgr->mSqlFile.mMsgAllocSize = sMsgAllocSize;

        STL_TRY( stlAllocHeap( (void**)&aApplierMgr->mSqlFile.mMsgBuffer,
                               sMsgAllocSize,
                               aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        /**
         * 실제 할당 크기로 설정
         */
        sMsgAllocSize = aApplierMgr->mSqlFile.mMsgAllocSize;
    }

    sMsgBuffer = aApplierMgr->mSqlFile.mMsgBuffer;

    sIndex += stlSnprintf( sMsgBuffer + sIndex,
                           sMsgAllocSize - sIndex,
                           "R,I,%s,",
                           aTableInfo->mInfoPtr->mSlaveTable );

    STL_TRY_THROW( sIndex + 1 < sMsgAllocSize, ERROR_BUFF_OVERFLOW );

    STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnValue )
    {
        /**
         * APPLY_ARRAY_SIZE 속성이 DEPRECATED 이후에 작업한 내용이며, ARRARY를 위한 처리를 안함
         */
        sDataValue.mType       = sColumnValue->mInfoPtr->mDataTypeId;
        sDataValue.mBufferSize = sColumnValue->mInfoPtr->mStrValueMaxSize;      //mBufferLength??

        /**
         * Insert 일 경우에는 PK값이 mValue에 있지만 Update,Delete일 경우에는 mKeyValue에 세팅된다.
         */
        sDataValue.mValue  = sColumnValue->mValue;
        sDataValue.mLength = *sColumnValue->mIndicator;          //mValueSize는 Indicator 세팅 후에 0으로 초기화하므로 mIndicator를 사용해야 한다.
        STL_DASSERT( sDataValue.mBufferSize >= sDataValue.mLength );

        if( (sDataValue.mType == DTL_TYPE_LONGVARCHAR) ||
            (sDataValue.mType == DTL_TYPE_LONGVARBINARY) )
        {
            STL_TRY( ztcmLogMsg( aErrorStack,
                                 "[APPLIER] sql file write : LONG VARCHAR, LONG VARBINARY column not support. column_name [%s]\n",
                                 sColumnValue->mInfoPtr->mColumnName )
                     == STL_SUCCESS );

            sDataValue.mLength = -1;
        }

        if( sDataValue.mLength > 0 )
        {
            sIndex += stlSnprintf( sMsgBuffer + sIndex,
                                   sMsgAllocSize - sIndex,
                                   "\"" );

            STL_TRY_THROW( sIndex + 1 < sMsgAllocSize, ERROR_BUFF_OVERFLOW );

            STL_TRY_THROW( sIndex + sColumnValue->mInfoPtr->mStrValueMaxSize < sMsgAllocSize, ERROR_BUFF_OVERFLOW );

            STL_TRY( dtlToStringDataValue( (dtlDataValue*)&sDataValue,
                                           sColumnValue->mInfoPtr->mLeadingPrecision,
                                           sColumnValue->mInfoPtr->mSecondPrecision,
                                           sMsgAllocSize - sIndex,
                                           sMsgBuffer + sIndex,
                                           &sLength,
                                           &gSlaveMgr->mDTVector,
                                           NULL,
                                           aErrorStack ) == STL_SUCCESS );
            sMsgBuffer[sIndex + sLength] = 0x00;

            STL_TRY( ztcaChangeValueStr( sMsgBuffer + sIndex,
                                         sMsgAllocSize - sIndex,
                                         &sValueLen,
                                         aErrorStack )
                     == STL_SUCCESS );
            sIndex += sValueLen;

            sIndex += stlSnprintf( sMsgBuffer + sIndex,
                                   sMsgAllocSize - sIndex,
                                   "\"" );

            STL_TRY_THROW( sIndex + 1 < sMsgAllocSize, ERROR_BUFF_OVERFLOW );
        }
        else
        {
            sIndex += stlSnprintf( sMsgBuffer + sIndex,
                                   sMsgAllocSize - sIndex,
                                   " " );

            STL_TRY_THROW( sIndex + 1 < sMsgAllocSize, ERROR_BUFF_OVERFLOW );
        }

        //stlPrintf(" [COLUMN_NAME:%s, VALUE:%s] ", sColumnValue->mInfoPtr->mColumnName, sColumnValue->mStrValue );
        sIndex += stlSnprintf( sMsgBuffer + sIndex,
                               sMsgAllocSize - sIndex,
                               "," );
        STL_TRY_THROW( sIndex + 1 < sMsgAllocSize, ERROR_BUFF_OVERFLOW );
    }

    /**
     * 마지막 comma(,) 제거
     * LF 추가
     */
    sIndex -= 1;

    sIndex += stlSnprintf( sMsgBuffer + sIndex,
                           sMsgAllocSize - sIndex,
                           ZTC_SQL_FILE_LINE_END );

    STL_TRY_THROW( sIndex + 1 < sMsgAllocSize, ERROR_BUFF_OVERFLOW );

    STL_TRY( ztcaWriteDataFile( aApplierMgr,
                                &aApplierMgr->mSqlFile,
                                sMsgBuffer,
                                aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( ERROR_BUFF_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INTERNAL_ERROR,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztcaUpdateToFile( ztcApplierMgr        * aApplierMgr,
                            ztcApplierTableInfo  * aTableInfo,
                            stlErrorStack        * aErrorStack )
{
    ztcColumnValue * sColumnValue = NULL;
    dtlDataValue     sDataValue;    //for Conversion.
    stlInt64         sLength;
    stlChar        * sMsgBuffer = NULL;
    stlInt64         sMsgAllocSize = ZTC_TRACE_BUFFER_SIZE;
    stlSize          sIndex = 0;
    stlSize          sValueLen;

    STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnValue )
    {
        sMsgAllocSize += sColumnValue->mInfoPtr->mStrValueMaxSize;
    }

    if( sMsgAllocSize > aApplierMgr->mSqlFile.mMsgAllocSize )
    {
        stlFreeHeap( aApplierMgr->mSqlFile.mMsgBuffer );
        aApplierMgr->mSqlFile.mMsgBuffer = NULL;

        aApplierMgr->mSqlFile.mMsgAllocSize = sMsgAllocSize;

        STL_TRY( stlAllocHeap( (void**)&aApplierMgr->mSqlFile.mMsgBuffer,
                               sMsgAllocSize,
                               aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        /**
         * 실제 할당 크기로 설정
         */
        sMsgAllocSize = aApplierMgr->mSqlFile.mMsgAllocSize;
    }

    sMsgBuffer = aApplierMgr->mSqlFile.mMsgBuffer;


    sIndex += stlSnprintf( sMsgBuffer + sIndex,
                           sMsgAllocSize - sIndex,
                           "R,U,%s,",
                           aTableInfo->mInfoPtr->mSlaveTable );

    STL_TRY_THROW( sIndex + 1 < sMsgAllocSize, ERROR_BUFF_OVERFLOW );

    STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnValue )
    {
        if( sColumnValue->mIsUpdated == STL_TRUE )
        {
            /**
             * APPLY_ARRAY_SIZE 속성이 DEPRECATED 이후에 작업한 내용이며, ARRARY를 위한 처리를 안함
             */
            sDataValue.mType       = sColumnValue->mInfoPtr->mDataTypeId;
            sDataValue.mBufferSize = sColumnValue->mInfoPtr->mStrValueMaxSize;      //mBufferLength??


            /**
             * SET 절
             */

            sDataValue.mValue  = sColumnValue->mValue;
            sDataValue.mLength = *sColumnValue->mIndicator;          //mValueSize는 Indicator 세팅 후에 0으로 초기화하므로 mIndicator를 사용해야 한다.

            if( (sDataValue.mType == DTL_TYPE_LONGVARCHAR) ||
                (sDataValue.mType == DTL_TYPE_LONGVARBINARY) )
            {
                STL_TRY( ztcmLogMsg( aErrorStack,
                                     "[APPLIER] sql file write : LONG VARCHAR, LONG VARBINARY column not support. column_name [%s]\n",
                                     sColumnValue->mInfoPtr->mColumnName )
                         == STL_SUCCESS );

                sDataValue.mLength = -1;
            }

            /* column name */
            sIndex += stlSnprintf( sMsgBuffer + sIndex,
                                   sMsgAllocSize - sIndex,
                                   "%s,",
                                   sColumnValue->mInfoPtr->mColumnName );

            STL_TRY_THROW( sIndex + 1 < sMsgAllocSize, ERROR_BUFF_OVERFLOW );


            if( sDataValue.mLength > 0 )
            {
                sIndex += stlSnprintf( sMsgBuffer + sIndex,
                                       sMsgAllocSize - sIndex,
                                       "\"" );

                STL_TRY_THROW( sIndex + 1 < sMsgAllocSize, ERROR_BUFF_OVERFLOW );

                STL_TRY_THROW( sIndex + sColumnValue->mInfoPtr->mStrValueMaxSize < sMsgAllocSize, ERROR_BUFF_OVERFLOW );

                STL_TRY( dtlToStringDataValue( (dtlDataValue*)&sDataValue,
                                               sColumnValue->mInfoPtr->mLeadingPrecision,
                                               sColumnValue->mInfoPtr->mSecondPrecision,
                                               sMsgAllocSize - sIndex,
                                               sMsgBuffer + sIndex,
                                               &sLength,
                                               &gSlaveMgr->mDTVector,
                                               NULL,
                                               aErrorStack ) == STL_SUCCESS );
                sMsgBuffer[sIndex + sLength] = 0x00;

                STL_TRY( ztcaChangeValueStr( sMsgBuffer + sIndex,
                                             sMsgAllocSize - sIndex,
                                             &sValueLen,
                                             aErrorStack )
                         == STL_SUCCESS );
                sIndex += sValueLen;

                sIndex += stlSnprintf( sMsgBuffer + sIndex,
                                       sMsgAllocSize - sIndex,
                                       "\"" );

                STL_TRY_THROW( sIndex + 1 < sMsgAllocSize, ERROR_BUFF_OVERFLOW );
            }
            else
            {
                sIndex += stlSnprintf( sMsgBuffer + sIndex,
                                       sMsgAllocSize - sIndex,
                                       " " );

                STL_TRY_THROW( sIndex + 1 < sMsgAllocSize, ERROR_BUFF_OVERFLOW );
            }

            //stlPrintf(" [COLUMN_NAME:%s, VALUE:%s] ", sColumnValue->mInfoPtr->mColumnName, sColumnValue->mStrValue );
            sIndex += stlSnprintf( sMsgBuffer + sIndex,
                                   sMsgAllocSize - sIndex,
                                   "," );
            STL_TRY_THROW( sIndex + 1 < sMsgAllocSize, ERROR_BUFF_OVERFLOW );
        }
    }

    sIndex += stlSnprintf( sMsgBuffer + sIndex,
                           sMsgAllocSize - sIndex,
                           " ," );

    STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnValue )
    {
        if( sColumnValue->mInfoPtr->mIsPrimary == STL_TRUE )
        {
            /**
             * APPLY_ARRAY_SIZE 속성이 DEPRECATED 이후에 작업한 내용이며, ARRARY를 위한 처리를 안함
             */
            sDataValue.mType       = sColumnValue->mInfoPtr->mDataTypeId;
            sDataValue.mBufferSize = sColumnValue->mInfoPtr->mStrValueMaxSize;      //mBufferLength??

            /**
             * WHERE 절
             */

            sDataValue.mValue  = sColumnValue->mKeyValue;
            sDataValue.mLength = *sColumnValue->mKeyIndicator;

            if( (sDataValue.mType == DTL_TYPE_LONGVARCHAR) ||
                (sDataValue.mType == DTL_TYPE_LONGVARBINARY) )
            {
                STL_TRY( ztcmLogMsg( aErrorStack,
                                     "[APPLIER] sql file write : LONG VARCHAR, LONG VARBINARY column not support. column_name [%s]\n",
                                     sColumnValue->mInfoPtr->mColumnName )
                         == STL_SUCCESS );

                sDataValue.mLength = -1;
            }


            /* column name */
            sIndex += stlSnprintf( sMsgBuffer + sIndex,
                                   sMsgAllocSize - sIndex,
                                   "%s,",
                                   sColumnValue->mInfoPtr->mColumnName );

            STL_TRY_THROW( sIndex + 1 < sMsgAllocSize, ERROR_BUFF_OVERFLOW );

            if( sDataValue.mLength > 0 )
            {
                sIndex += stlSnprintf( sMsgBuffer + sIndex,
                                       sMsgAllocSize - sIndex,
                                       "\"" );

                STL_TRY_THROW( sIndex + 1 < sMsgAllocSize, ERROR_BUFF_OVERFLOW );

                STL_TRY_THROW( sIndex + sColumnValue->mInfoPtr->mStrValueMaxSize < sMsgAllocSize, ERROR_BUFF_OVERFLOW );

                STL_TRY( dtlToStringDataValue( (dtlDataValue*)&sDataValue,
                                               sColumnValue->mInfoPtr->mLeadingPrecision,
                                               sColumnValue->mInfoPtr->mSecondPrecision,
                                               sMsgAllocSize - sIndex,
                                               sMsgBuffer + sIndex,
                                               &sLength,
                                               &gSlaveMgr->mDTVector,
                                               NULL,
                                               aErrorStack ) == STL_SUCCESS );
                sMsgBuffer[sIndex + sLength] = 0x00;

                STL_TRY_THROW( sIndex + 1 < sMsgAllocSize, ERROR_BUFF_OVERFLOW );

                /* column value */
                STL_TRY( ztcaChangeValueStr( sMsgBuffer + sIndex,
                                             sMsgAllocSize - sIndex,
                                             &sValueLen,
                                             aErrorStack )
                         == STL_SUCCESS );
                sIndex += sValueLen;

                sIndex += stlSnprintf( sMsgBuffer + sIndex,
                                       sMsgAllocSize - sIndex,
                                       "\"" );

                STL_TRY_THROW( sIndex + 1 < sMsgAllocSize, ERROR_BUFF_OVERFLOW );
            }
            else
            {
                sIndex += stlSnprintf( sMsgBuffer + sIndex,
                                       sMsgAllocSize - sIndex,
                                       " " );

                STL_TRY_THROW( sIndex + 1 < sMsgAllocSize, ERROR_BUFF_OVERFLOW );
            }


            //stlPrintf(" [COLUMN_NAME:%s, VALUE:%s] ", sColumnValue->mInfoPtr->mColumnName, sColumnValue->mStrValue );
            sIndex += stlSnprintf( sMsgBuffer + sIndex,
                                   sMsgAllocSize - sIndex,
                                   "," );
            STL_TRY_THROW( sIndex + 1 < sMsgAllocSize, ERROR_BUFF_OVERFLOW );
        }
    }

    /**
     * 마지막 comma(,) 제거
     * LF 추가
     */
    sIndex -= 1;

    sIndex += stlSnprintf( sMsgBuffer + sIndex,
                           sMsgAllocSize - sIndex,
                           ZTC_SQL_FILE_LINE_END );

    STL_TRY_THROW( sIndex + 1 < sMsgAllocSize, ERROR_BUFF_OVERFLOW );

    STL_TRY( ztcaWriteDataFile( aApplierMgr,
                                &aApplierMgr->mSqlFile,
                                sMsgBuffer,
                                aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( ERROR_BUFF_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INTERNAL_ERROR,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}



stlStatus ztcaDeleteToFile( ztcApplierMgr        * aApplierMgr,
                            ztcApplierTableInfo  * aTableInfo,
                            stlErrorStack        * aErrorStack )
{
    ztcColumnValue * sColumnValue = NULL;
    dtlDataValue     sDataValue;    //for Conversion.
    stlInt64         sLength;
    stlChar        * sMsgBuffer = NULL;
    stlInt64         sMsgAllocSize = ZTC_TRACE_BUFFER_SIZE;
    stlSize          sIndex = 0;
    stlSize          sValueLen;

    STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnValue )
    {
        sMsgAllocSize += sColumnValue->mInfoPtr->mStrValueMaxSize;
        sMsgAllocSize += stlStrlen( sColumnValue->mInfoPtr->mColumnName );
    }

    if( sMsgAllocSize > aApplierMgr->mSqlFile.mMsgAllocSize )
    {
        stlFreeHeap( aApplierMgr->mSqlFile.mMsgBuffer );
        aApplierMgr->mSqlFile.mMsgBuffer = NULL;

        aApplierMgr->mSqlFile.mMsgAllocSize = sMsgAllocSize;

        STL_TRY( stlAllocHeap( (void**)&aApplierMgr->mSqlFile.mMsgBuffer,
                               sMsgAllocSize,
                               aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        /**
         * 실제 할당 크기로 설정
         */
        sMsgAllocSize = aApplierMgr->mSqlFile.mMsgAllocSize;
    }

    sMsgBuffer = aApplierMgr->mSqlFile.mMsgBuffer;

    sIndex += stlSnprintf( sMsgBuffer + sIndex,
                           sMsgAllocSize - sIndex,
                           "R,D,%s,",
                           aTableInfo->mInfoPtr->mSlaveTable );

    STL_TRY_THROW( sIndex + 1 < sMsgAllocSize, ERROR_BUFF_OVERFLOW );

    STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnValue )
    {
        if( sColumnValue->mInfoPtr->mIsPrimary == STL_TRUE )
        {
            /**
             * APPLY_ARRAY_SIZE 속성이 DEPRECATED 이후에 작업한 내용이며, ARRARY를 위한 처리를 안함
             */
            sDataValue.mType       = sColumnValue->mInfoPtr->mDataTypeId;
            sDataValue.mBufferSize = sColumnValue->mInfoPtr->mStrValueMaxSize;      //mBufferLength??

            sDataValue.mValue  = sColumnValue->mKeyValue;
            sDataValue.mLength = *sColumnValue->mKeyIndicator;

            if( (sDataValue.mType == DTL_TYPE_LONGVARCHAR) ||
                (sDataValue.mType == DTL_TYPE_LONGVARBINARY) )
            {
                STL_TRY( ztcmLogMsg( aErrorStack,
                                     "[APPLIER] sql file write : LONG VARCHAR, LONG VARBINARY column not support. column_name [%s]\n",
                                     sColumnValue->mInfoPtr->mColumnName )
                         == STL_SUCCESS );

                sDataValue.mLength = -1;
            }

            /* column name */
            sIndex += stlSnprintf( sMsgBuffer + sIndex,
                                   sMsgAllocSize - sIndex,
                                   "%s,",
                                   sColumnValue->mInfoPtr->mColumnName );
            STL_TRY_THROW( sIndex + 1 < sMsgAllocSize, ERROR_BUFF_OVERFLOW );

            if( sDataValue.mLength > 0 )
            {
                sIndex += stlSnprintf( sMsgBuffer + sIndex,
                                       sMsgAllocSize - sIndex,
                                       "\"" );

                STL_TRY_THROW( sIndex + 1 < sMsgAllocSize, ERROR_BUFF_OVERFLOW );

                STL_TRY_THROW( sIndex + sColumnValue->mInfoPtr->mStrValueMaxSize < sMsgAllocSize, ERROR_BUFF_OVERFLOW );

                STL_TRY( dtlToStringDataValue( (dtlDataValue*)&sDataValue,
                                               sColumnValue->mInfoPtr->mLeadingPrecision,
                                               sColumnValue->mInfoPtr->mSecondPrecision,
                                               sMsgAllocSize - sIndex,
                                               sMsgBuffer + sIndex,
                                               &sLength,
                                               &gSlaveMgr->mDTVector,
                                               NULL,
                                               aErrorStack ) == STL_SUCCESS );
                sMsgBuffer[sIndex + sLength] = 0x00;

                STL_TRY( ztcaChangeValueStr( sMsgBuffer + sIndex,
                                             sMsgAllocSize - sIndex,
                                             &sValueLen,
                                             aErrorStack )
                         == STL_SUCCESS );
                sIndex += sValueLen;

                sIndex += stlSnprintf( sMsgBuffer + sIndex,
                                       sMsgAllocSize - sIndex,
                                       "\"" );

                STL_TRY_THROW( sIndex + 1 < sMsgAllocSize, ERROR_BUFF_OVERFLOW );
            }
            else
            {
                sIndex += stlSnprintf( sMsgBuffer + sIndex,
                                       sMsgAllocSize - sIndex,
                                       " " );

                STL_TRY_THROW( sIndex + 1 < sMsgAllocSize, ERROR_BUFF_OVERFLOW );
            }

            //stlPrintf(" [COLUMN_NAME:%s, VALUE:%s] ", sColumnValue->mInfoPtr->mColumnName, sColumnValue->mStrValue );
            sIndex += stlSnprintf( sMsgBuffer + sIndex,
                                   sMsgAllocSize - sIndex,
                                   "," );
            STL_TRY_THROW( sIndex + 1 < sMsgAllocSize, ERROR_BUFF_OVERFLOW );

        }
    }

    /**
     * 마지막 comma(,) 제거
     * LF 추가
     */
    sIndex -= 1;

    sIndex += stlSnprintf( sMsgBuffer + sIndex,
                           sMsgAllocSize - sIndex,
                           ZTC_SQL_FILE_LINE_END );

    STL_TRY_THROW( sIndex + 1 < sMsgAllocSize, ERROR_BUFF_OVERFLOW );

    STL_TRY( ztcaWriteDataFile( aApplierMgr,
                                &aApplierMgr->mSqlFile,
                                sMsgBuffer,
                                aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( ERROR_BUFF_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INTERNAL_ERROR,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztcaTxBeginToFile( ztcApplierMgr        * aApplierMgr,
                             stlChar              * aComment,
                             stlInt32               aCommentLen,
                             stlErrorStack        * aErrorStack )
{
    stlChar        * sMsgBuffer = NULL;
    stlInt64         sMsgAllocSize;
    stlSize          sIndex = 0;
    stlSize          sValueLen;

    sMsgAllocSize = aApplierMgr->mSqlFile.mMsgAllocSize;
    sMsgBuffer = aApplierMgr->mSqlFile.mMsgBuffer;

    sIndex = stlSnprintf( sMsgBuffer,
                          sMsgAllocSize,
                          "R,B,\"" );
    STL_TRY_THROW( sIndex + 1 < sMsgAllocSize, ERROR_BUFF_OVERFLOW );

    STL_TRY_THROW( sIndex + 1 + aCommentLen < sMsgAllocSize, ERROR_BUFF_OVERFLOW );
    stlMemcpy( sMsgBuffer + sIndex, aComment, aCommentLen );
    sMsgBuffer[sIndex + aCommentLen] = 0x00;

    STL_TRY( ztcaChangeValueStr( sMsgBuffer + sIndex,
                                 sMsgAllocSize - sIndex,
                                 &sValueLen,
                                 aErrorStack )
             == STL_SUCCESS );
    sIndex += sValueLen;

    sIndex += stlSnprintf( sMsgBuffer + sIndex,
                           sMsgAllocSize - sIndex,
                           "\"%s",
                           ZTC_SQL_FILE_LINE_END );

    STL_TRY_THROW( sIndex + 1 < sMsgAllocSize, ERROR_BUFF_OVERFLOW );

    STL_TRY( ztcaWriteDataFile( aApplierMgr,
                                &aApplierMgr->mSqlFile,
                                sMsgBuffer,
                                aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( ERROR_BUFF_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INTERNAL_ERROR,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztcaCommitToFile( ztcApplierMgr        * aApplierMgr,
                            ztcCaptureInfo       * aCaptureInfo,
                            stlErrorStack        * aErrorStack )
{
    stlChar        * sMsgBuffer = NULL;
    stlInt64         sMsgAllocSize;
    stlSize          sValueLen;

    sMsgAllocSize = aApplierMgr->mSqlFile.mMsgAllocSize;
    sMsgBuffer = aApplierMgr->mSqlFile.mMsgBuffer;

    sValueLen = stlSnprintf( sMsgBuffer,
                             sMsgAllocSize,
                             "%s"                               /* COMMIT */
                             ",INFO,%ld,%ld,%ld,%d,%d,%d,%d,%s",
                             ZTC_SQL_FILE_COMMIT,
                             aCaptureInfo->mRestartLSN,
                             aCaptureInfo->mLastCommitLSN,
                             aCaptureInfo->mFileSeqNo,
                             aCaptureInfo->mRedoLogGroupId,
                             aCaptureInfo->mBlockSeq,
                             aCaptureInfo->mReadLogNo,
                             aCaptureInfo->mWrapNo,
                             ZTC_SQL_FILE_LINE_END );

    STL_TRY_THROW( sValueLen + 1 < sMsgAllocSize, ERROR_BUFF_OVERFLOW );

    STL_TRY( ztcaWriteDataFile( aApplierMgr,
                                &aApplierMgr->mSqlFile,
                                sMsgBuffer,
                                aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztcaWriteCtrlFile( &aApplierMgr->mSqlFile,
                                aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( ERROR_BUFF_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INTERNAL_ERROR,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztcaStopToFile( ztcApplierMgr        * aApplierMgr,
                          stlErrorStack        * aErrorStack )
{
    stlChar        * sMsgBuffer = NULL;
    stlInt64         sMsgAllocSize;
    stlSize          sValueLen;

    sMsgAllocSize = aApplierMgr->mSqlFile.mMsgAllocSize;
    sMsgBuffer = aApplierMgr->mSqlFile.mMsgBuffer;

    sValueLen = stlSnprintf( sMsgBuffer,
                             sMsgAllocSize,
                             "C,STOP,%s",
                             ZTC_SQL_FILE_LINE_END );

    STL_TRY_THROW( sValueLen + 1 < sMsgAllocSize, ERROR_BUFF_OVERFLOW );

    STL_TRY( ztcaWriteDataFile( aApplierMgr,
                                &aApplierMgr->mSqlFile,
                                sMsgBuffer,
                                aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztcaWriteCtrlFile( &aApplierMgr->mSqlFile,
                                aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( ERROR_BUFF_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INTERNAL_ERROR,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}



stlStatus ztcaWriteErrorValueToLog( stlChar              * aErrMsg,
                                    stlBool                aIsInsert,
                                    ztcApplierTableInfo  * aTableInfo,
                                    stlErrorStack        * aErrorStack )
{
    ztcColumnValue * sColumnValue = NULL;
    dtlDataValue     sDataValue;    //for Conversion.
    stlInt64         sLength;
    stlChar        * sMsgBuffer = NULL;
    stlInt64         sMsgLength = ZTC_TRACE_BUFFER_SIZE;
    stlInt32         sState     = 0;

    STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnValue )
    {
        if( sColumnValue->mInfoPtr->mIsPrimary == STL_TRUE )
        {
            sMsgLength += sColumnValue->mInfoPtr->mStrValueMaxSize;
        }
    }

    STL_TRY( stlAllocHeap( (void**)&sMsgBuffer,
                           sMsgLength,
                           aErrorStack ) == STL_SUCCESS );
    sState = 1;
    stlMemset( sMsgBuffer, 0x00, sMsgLength );

    stlSnprintf( sMsgBuffer,
                 sMsgLength,
                 "%s  [TABLE_NAME : %s.%s]\n",
                 aErrMsg,
                 aTableInfo->mInfoPtr->mSlaveSchema,
                 aTableInfo->mInfoPtr->mSlaveTable );

    STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnValue )
    {
        if( sColumnValue->mInfoPtr->mIsPrimary == STL_TRUE )
        {
            /**
             * APPLY_ARRAY_SIZE 속성이 DEPRECATED 이후에 작업한 내용이며, ARRARY를 위한 처리를 안함
             */
            sDataValue.mType       = sColumnValue->mInfoPtr->mDataTypeId;
            sDataValue.mBufferSize = sColumnValue->mInfoPtr->mStrValueMaxSize;      //mBufferLength??

            /**
             * Insert 일 경우에는 PK값이 mValue에 있지만 Update,Delete일 경우에는 mKeyValue에 세팅된다.
             */
            if( aIsInsert == STL_TRUE )
            {
                sDataValue.mValue  = sColumnValue->mValue;
                sDataValue.mLength = *sColumnValue->mIndicator;          //mValueSize는 Indicator 세팅 후에 0으로 초기화하므로 mIndicator를 사용해야 한다.
            }
            else
            {
                sDataValue.mValue  = sColumnValue->mKeyValue;
                sDataValue.mLength = *sColumnValue->mKeyIndicator;
            }

            STL_TRY( dtlToStringDataValue( (dtlDataValue*)&sDataValue,
                                           sColumnValue->mInfoPtr->mLeadingPrecision,
                                           sColumnValue->mInfoPtr->mSecondPrecision,
                                           sColumnValue->mInfoPtr->mStrValueMaxSize,
                                           sColumnValue->mStrValue,
                                           &sLength,
                                           &gSlaveMgr->mDTVector,
                                           NULL,
                                           aErrorStack ) == STL_SUCCESS );

            sColumnValue->mStrValue[ sLength ] = '\0';

            stlSnprintf( sMsgBuffer,
                         sMsgLength,
                         "%s    [COLUMN_NAME:%s, VALUE:%s] \n",
                         sMsgBuffer,
                         sColumnValue->mInfoPtr->mColumnName,
                         sColumnValue->mStrValue );

            //stlPrintf(" [COLUMN_NAME:%s, VALUE:%s] ", sColumnValue->mInfoPtr->mColumnName, sColumnValue->mStrValue );
        }
    }

    STL_TRY( ztcmLogMsg( aErrorStack,
                         "%s\n",
                         sMsgBuffer ) == STL_SUCCESS );
    sState = 0;
    (void) stlFreeHeap( sMsgBuffer );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void) stlFreeHeap( sMsgBuffer );
        default:
            break;
    }

    return STL_FAILURE;
}


/** @} */
