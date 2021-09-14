/*******************************************************************************
 * ztccTransMemManager.c
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
 * @file ztccTransMemManager.c
 * @brief GlieseTool Cyclone Transaction Chunk Management Routines
 */

#include <goldilocks.h>
#include <ztc.h>

extern ztcMasterMgr  * gMasterMgr;
extern stlBool         gRunState;

stlStatus ztccWriteTransDataToChunk( ztcTransSlotHdr * aTransSlotHdr,
                                     ztcTransHdr     * aTransHdr,
                                     stlChar         * aData,
                                     stlUInt16         aSize,
                                     stlErrorStack   * aErrorStack )
{
    ztcChunkItem  * sChunkItem   = NULL;
    ztcCaptureMgr * sCaptureMgr  = &(gMasterMgr->mCaptureMgr);
    ztcTransData  * sTransData   = NULL;

    if( STL_RING_IS_EMPTY( &aTransSlotHdr->mTransChunkList ) == STL_FALSE )
    {
        sChunkItem = STL_RING_GET_LAST_DATA( &aTransSlotHdr->mTransChunkList );

        if( (sChunkItem->mWriteIdx) + aSize + STL_SIZEOF( ztcTransData ) > ZTC_TRANS_CHUNK_ITEM_SIZE )
        {
            sChunkItem = NULL;
        }
    }

    if( sChunkItem == NULL )
    {
        if( STL_RING_IS_EMPTY( &sCaptureMgr->mTransChunkFreeList ) == STL_TRUE )
        {
            /**
             * 더이상 사용할 수 있는 Chunk가 없을 경우
             * Memory를 가장 많이 사용하는 Transation을 Disk로 내림
             */
            STL_TRY( ztccWriteTransDataToFile( aErrorStack ) == STL_SUCCESS );
        }
        
        STL_DASSERT( STL_RING_IS_EMPTY( &sCaptureMgr->mTransChunkFreeList ) == STL_FALSE );
        
        sChunkItem = STL_RING_GET_FIRST_DATA( &sCaptureMgr->mTransChunkFreeList );

        STL_RING_UNLINK( &sCaptureMgr->mTransChunkFreeList, sChunkItem );

        STL_RING_ADD_LAST( &aTransSlotHdr->mTransChunkList,
                           sChunkItem );
    }

    STL_ASSERT( sChunkItem != NULL );

    sTransData = (ztcTransData*)&sChunkItem->mBuffer[ sChunkItem->mWriteIdx ];

    STL_RING_INIT_DATALINK( sTransData,
                            STL_OFFSETOF( ztcTransData, mLink ) );

    sTransData->mBufferSize = STL_SIZEOF( ztcTransHdr ) + aSize;
    
    stlMemcpy( &sTransData->mTransHdr, (stlChar*)aTransHdr, STL_SIZEOF( ztcTransHdr ) );
    
    STL_RING_ADD_LAST( &aTransSlotHdr->mTransDataList,
                       sTransData );
    
    sChunkItem->mWriteIdx += STL_SIZEOF( ztcTransData );

    if( aData != NULL )
    {
        stlMemcpy( &sChunkItem->mBuffer[ sChunkItem->mWriteIdx ],
                   aData,
                   aSize );
    }

    sChunkItem->mWriteIdx += aSize;
    
    aTransSlotHdr->mLogCount++;
    aTransSlotHdr->mSize += STL_SIZEOF( ztcTransHdr ) + aSize;
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztccRemoveTransDataFromChunk( ztcTransSlotHdr * aTransSlotHdr,
                                        ztcTransData    * aTransData,
                                        stlErrorStack   * aErrorStack )
{
    ztcChunkItem  * sChunkItem   = NULL;
    ztcCaptureMgr * sCaptureMgr  = &(gMasterMgr->mCaptureMgr);
    stlUInt32       sSize;

    sSize = STL_SIZEOF( ztcTransData ) + aTransData->mTransHdr.mSize;
    
    STL_ASSERT( STL_RING_IS_EMPTY( &aTransSlotHdr->mTransChunkList ) == STL_FALSE );
        
    STL_RING_UNLINK( &aTransSlotHdr->mTransDataList, aTransData );
    
    aTransSlotHdr->mLogCount--;
    aTransSlotHdr->mSize -= aTransData->mBufferSize;
    
    sChunkItem = STL_RING_GET_LAST_DATA( &aTransSlotHdr->mTransChunkList );
    
    sChunkItem->mWriteIdx -= sSize;
    
    STL_ASSERT( sChunkItem->mWriteIdx >= 0 );
    
    if( sChunkItem->mWriteIdx == 0 )
    {
        STL_RING_UNLINK( &aTransSlotHdr->mTransChunkList, 
                         sChunkItem );
        
        sChunkItem->mWriteIdx = 0;
        sChunkItem->mReadIdx  = 0;
        
        STL_RING_ADD_LAST( &sCaptureMgr->mTransChunkFreeList, 
                           sChunkItem );
    }
    
    return STL_SUCCESS;
}


stlStatus ztccWriteTransDataToFile( stlErrorStack * aErrorStack )
{
    stlInt32   sIdx       = 0;
    stlInt32   sState     = 0;
    stlInt32   sMaxIdx    = 0;
    stlInt64   sMaxSize   = 0;
    stlSize    sWriteSize = 0;
    stlSize    sTotalWriteSize = 0;
    stlBool    sExist     = STL_FALSE;
    stlChar  * sDataPtr   = NULL;
    stlFile    sFile;
    stlChar    sFileName[ STL_MAX_FILE_PATH_LENGTH ] = {0, };
    
    ztcCaptureMgr   * sCaptureMgr  = &(gMasterMgr->mCaptureMgr);
    ztcTransSlotHdr * sTransSlotHdr  = NULL;
    ztcTransData    * sTransData   = NULL;
    
    for( sIdx = 0; sIdx < sCaptureMgr->mTransTableMax; sIdx++ )
    {
        sTransSlotHdr = &(sCaptureMgr->mTransSlotHdr[ sIdx ]);
        
        if( sTransSlotHdr->mIsRebuild == STL_TRUE ||
            sTransSlotHdr->mSize == 0 )
        {
            /**
             * Transaction Slot이 Rebuild 중이면 해당 Slot을 처리해서는 안된다.
             */
            continue;   
        }
        
        if( sTransSlotHdr->mSize > sMaxSize )
        {
            sMaxIdx  = sIdx;
            sMaxSize = sTransSlotHdr->mSize;
        }
    }
    
    sTransSlotHdr = &(sCaptureMgr->mTransSlotHdr[ sMaxIdx ]);
    
    STL_TRY( ztcmGetTransFileName( sMaxIdx,
                                   sFileName,
                                   aErrorStack ) == STL_SUCCESS );
    
    if( sTransSlotHdr->mFileLogCount > 0 )
    {
        STL_TRY( stlExistFile( sFileName,
                               &sExist,
                               aErrorStack ) == STL_SUCCESS );
        
        STL_TRY_THROW( sExist == STL_TRUE, RAMP_ERR_NOT_EXIST_FILE );
        
        STL_TRY( stlOpenFile( &sFile,
                              sFileName,
                              STL_FOPEN_WRITE | STL_FOPEN_APPEND | STL_FOPEN_BINARY,
                              STL_FPERM_OS_DEFAULT,
                              aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( stlOpenFile( &sFile,
                              sFileName,
                              STL_FOPEN_CREATE | STL_FOPEN_WRITE | STL_FOPEN_BINARY | STL_FOPEN_TRUNCATE,
                              STL_FPERM_OS_DEFAULT,
                              aErrorStack ) == STL_SUCCESS );
    }
    sState = 1;
    
    STL_RING_FOREACH_ENTRY( &sTransSlotHdr->mTransDataList, sTransData )
    {
        sDataPtr = (stlChar*)&sTransData->mTransHdr;
        
        STL_TRY( stlWriteFile( &sFile,
                               sDataPtr,
                               sTransData->mBufferSize,
                               &sWriteSize,
                               aErrorStack ) == STL_SUCCESS );
        
        sTotalWriteSize += sWriteSize;
        
        STL_TRY_THROW( sTransData->mBufferSize == sWriteSize, RAMP_ERR_INVALID_FILE_WRITE );
        
        sTransSlotHdr->mFileLogCount++;
        sTransSlotHdr->mLogCount--;
    }
    
    STL_TRY_THROW( sTotalWriteSize == sTransSlotHdr->mSize, RAMP_ERR_INVALID_FILE_WRITE );
    STL_ASSERT( sTransSlotHdr->mLogCount == 0 );
    
    sTransSlotHdr->mFileSize += sTransSlotHdr->mSize;
    sTransSlotHdr->mSize      = 0;
    
    sState = 0;
    STL_TRY( stlCloseFile( &sFile,
                           aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztccRemoveListAtSlot( sTransSlotHdr,
                                   aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_NOT_EXIST_FILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_FILE_NAME,
                      NULL,
                      aErrorStack,
                      sFileName );
    }
    STL_CATCH( RAMP_ERR_INVALID_FILE_WRITE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_FAIL_WRITE_FILE,
                      NULL,
                      aErrorStack,
                      sFileName );
    }
    STL_FINISH;
    
    if( sState > 0 )
    {
        (void) stlCloseFile( &sFile,
                             aErrorStack );
    }
    
    return STL_FAILURE;
}

/** @} */

