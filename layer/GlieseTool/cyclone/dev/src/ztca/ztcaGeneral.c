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


extern stlBool        gRunState;
extern ztcSlaveMgr  * gSlaveMgr;

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
     * Applier Chunk Memeory
     */
    STL_TRY( stlCreateRegionAllocator( &(aApplierMgr->mRegionMem),
                                       ZTC_REGION_INIT_SIZE,
                                       ZTC_REGION_NEXT_SIZE,
                                       aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztcdCreateConnection( &aApplierMgr->mDbcHandle,
                                   gSlaveMgr->mEnvHandle,
                                   STL_FALSE,
                                   aErrorStack ) == STL_SUCCESS );
    sState = 2;
    
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
    
    if( sPropagateMode == STL_FALSE )
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
    sState = 3;
    
    STL_TRY( ztcaInitializeTableInfo( aApplierMgr,
                                      aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcaInitializeStatement( aApplierMgr,
                                      aErrorStack ) == STL_SUCCESS );
    sState = 4;
    
    STL_TRY( ztcaInitializeBuffer( aApplierMgr,
                                   aErrorStack ) == STL_SUCCESS );
    sState = 5;
    
    STL_TRY( ztcaPrepareNBindParam4Insert( aApplierMgr,
                                           aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcaPrepareNBindParam4Delete( aApplierMgr,
                                           aErrorStack ) == STL_SUCCESS );

    STL_RING_INIT_HEADLINK( &aApplierMgr->mRecordList,
                            STL_OFFSETOF( ztcApplierRecItem, mLink ) );
    
    /**
     * Applier에서 사용할 Statement Handle 할당
     */
    STL_TRY( ztcdSQLAllocHandle( aApplierMgr->mDbcHandle,
                                 &aApplierMgr->mStateStmtHandle,
                                 aErrorStack ) == STL_SUCCESS );
    
    /**
     * 초기화가 되었으니 Distributor가 사용할 수 있도록
     */
    STL_TRY( stlReleaseSemaphore( &(gSlaveMgr->mDistbtStartSem),
                                  aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    switch( sState )
    {
        case 5:
            (void)ztcaFinalizeBuffer( aApplierMgr,
                                      aErrorStack );
        case 4:
            (void)ztcaFinalizeStatement( aApplierMgr,
                                         aErrorStack );
        case 3:
            (void)stlDestroyDynamicAllocator( &(aApplierMgr->mLongVaryingMem),
                                              aErrorStack );
        case 2:
            (void)ztcdDestroyConnection( aApplierMgr->mDbcHandle,
                                         aErrorStack );
        case 1:
            (void)stlDestroyRegionAllocator( &(aApplierMgr->mRegionMem),
                                             aErrorStack );
            break;
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztcaFinalizeApplier( ztcApplierMgr * aApplierMgr,
                               stlErrorStack * aErrorStack )
{
    stlInt32 sState = 5;
    
    STL_TRY( SQLFreeHandle( SQL_HANDLE_STMT,
                            aApplierMgr->mStateStmtHandle ) == SQL_SUCCESS );
    
    sState = 4;
    STL_TRY( ztcaFinalizeBuffer( aApplierMgr,
                                 aErrorStack ) == STL_SUCCESS );
    
    sState = 3;
    STL_TRY( ztcaFinalizeStatement( aApplierMgr,
                                    aErrorStack ) == STL_SUCCESS );
    
    sState = 2;
    STL_TRY( stlDestroyDynamicAllocator( &(aApplierMgr->mLongVaryingMem),
                                         aErrorStack ) == STL_SUCCESS );
    
    sState = 1;
    STL_TRY( ztcdDestroyConnection( aApplierMgr->mDbcHandle,
                                    aErrorStack ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( stlDestroyRegionAllocator( &(aApplierMgr->mRegionMem),
                                        aErrorStack ) == STL_SUCCESS );
    
    ztcmFinSpinLock( &(aApplierMgr->mSubDistSpinLock) );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    switch( sState )
    {
        case 5:
            (void) ztcaFinalizeBuffer( aApplierMgr,
                                       aErrorStack );
        case 4:
            (void) ztcaFinalizeStatement( aApplierMgr,
                                          aErrorStack );
        case 3:
            (void) stlDestroyDynamicAllocator( &(aApplierMgr->mLongVaryingMem),
                                               aErrorStack );
        case 2:
            (void) ztcdDestroyConnection( aApplierMgr->mDbcHandle,
                                          aErrorStack );
        case 1:
            (void) stlDestroyRegionAllocator( &(aApplierMgr->mRegionMem),
                                              aErrorStack );
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
                if( ztcmGetUpdateApplyMode() >= 1 )
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
                if( ztcmGetUpdateApplyMode() == 2 )
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
                
                if( ztcmGetUpdateApplyMode() >= 1 )
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
                
                if( ztcmGetUpdateApplyMode() == 2 )
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
                    STL_ASSERT( 0 );
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
        
        STL_ASSERT( sStmtInfo != NULL );
        
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
