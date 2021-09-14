/*******************************************************************************
 * ztccMemManager.c
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
 * @file ztccMemManager.c
 * @brief GlieseTool Cyclone Capture Chunk Management Routines
 */

#include <goldilocks.h>
#include <ztc.h>




stlStatus ztccWriteCommittedTransToChunk( ztcTransSlotHdr * aTransSlotHdr,
                                          stlUInt64         aCommitSCN,
                                          stlInt32          aCommentLength,
                                          stlChar         * aComment,
                                          stlErrorStack   * aErrorStack )
{
    ztcTransData   * sTransData     = NULL;
    stlChar        * sDataPtr       = NULL;
    stlBool          sExist         = STL_FALSE;
    stlSize          sFileReadSize  = 0;
    stlInt32         sState         = 0;
    stlInt64         sIdx           = 0;
    stlInt32         sCommentLength = STL_ALIGN( STL_SIZEOF( stlInt32 ) + aCommentLength, ZTC_MEM_ALIGN );

    stlFile     sFile;
    stlChar     sFileName[ STL_MAX_FILE_PATH_LENGTH ] = {0, };
    stlChar   * sLogBufferPtr = NULL;
    stlChar   * sLogBuffer    = NULL;
    stlInt32    sTrSeq = 0;
    
    ztcCaptureTableInfo * sTableInfo = NULL;
    ztcTransHdr         * sTransHdr  = NULL;

    STL_TRY( stlAllocHeap( (void**)&sLogBufferPtr,
                           ( ZTC_LOG_PIECE_MAX_SIZE + STL_SIZEOF(ztcTransHdr) + ZTC_MEM_ALIGN ),
                           aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    sLogBuffer = (void*)STL_ALIGN( (stlUInt64)sLogBufferPtr, ZTC_MEM_ALIGN );
    sTransHdr  = (ztcTransHdr*)sLogBuffer;
    
    if( aTransSlotHdr->mFileLogCount > 0 )
    {
        STL_TRY( ztcmGetTransFileName( ZTC_TRANS_SLOT_ID( aTransSlotHdr->mTransId ),
                                       sFileName,
                                       aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( stlExistFile( sFileName,
                               &sExist,
                               aErrorStack ) == STL_SUCCESS );
    
        STL_TRY_THROW( sExist == STL_TRUE, RAMP_ERR_NOT_EXIST_FILE );
    
        STL_TRY( stlOpenFile( &sFile,
                              sFileName,
                              STL_FOPEN_READ | STL_FOPEN_BINARY,
                              STL_FPERM_OS_DEFAULT,
                              aErrorStack ) == STL_SUCCESS );
        sState = 2;
    }
    
    /**
     * 하나의 Chunk에 모두 넣을 수 있는 경우.
     * 1. File에 쓰여진 내용이 있으면 File 부터 읽어서 보낸다.
     * 2. Memory에 있는 내용을 보낸다.
     */
    if( aTransSlotHdr->mFileLogCount > 0 )
    {
        /**
         * 성능 튜닝해야함!!!
         */
        for( sIdx = 0; sIdx < aTransSlotHdr->mFileLogCount; sIdx++ )
        {
            // TransHeader 읽기
            STL_TRY( stlReadFile( &sFile,
                                  sTransHdr,
                                  STL_SIZEOF( ztcTransHdr ),
                                  &sFileReadSize,
                                  aErrorStack ) == STL_SUCCESS );

            STL_TRY_THROW( STL_SIZEOF( ztcTransHdr ) == sFileReadSize, RAMP_ERROR_FILE_READ );
            STL_TRY_THROW( sTransHdr->mSize <= ZTC_LOG_PIECE_MAX_SIZE, RAMP_ERROR_FILE_READ );
            
            // TransBody 읽기
            STL_TRY( stlReadFile( &sFile,
                                  &sLogBuffer[ STL_SIZEOF(ztcTransHdr) ],
                                  sTransHdr->mSize,
                                  &sFileReadSize,
                                  aErrorStack ) == STL_SUCCESS );

            STL_TRY_THROW( sTransHdr->mSize == sFileReadSize, RAMP_ERROR_FILE_READ );
            
            if( sTransHdr->mTablePId != ZTC_INVALID_TABLE_PHYSICAL_ID )
            {
                STL_TRY( ztcmFindCaptureTableInfo( &sTableInfo,
                                                   sTransHdr->mTablePId,
                                                   aErrorStack ) == STL_SUCCESS );

                STL_DASSERT( sTableInfo != NULL );

                /**
                 * Transaction에 포함된 Table의 CommitScn이 현재 Commit된 SCN 보다 크거나 같을 경우는
                 * 이미 Slave에 반영된것 이기 때문에 Slave에 보내지 않는다. (SYNC 기능추가에 따른 구현인데 성능에 영향을 미칠수 있는 부분임..나중에 좀더 생각해 봐야겠음)
                 */
                if( sTableInfo->mCommitSCN >= aCommitSCN &&
                    sTableInfo->mCommitSCN != ZTC_INVALID_SCN )
                {
                    continue;
                }
            }

            /**
             * Commit Comment 가 있을 경우 Transaction Begin에 넣어서 처리하도록 함
             */
            if( sTransHdr->mLogType == ZTC_CAPTURE_TYPE_BEGIN )
            {
                sTransHdr->mSize = sCommentLength;
                STL_WRITE_INT32( &sLogBuffer[ STL_SIZEOF(ztcTransHdr) ], &aCommentLength );

                if( aCommentLength > 0 )
                {
                    stlMemcpy( &sLogBuffer[ STL_SIZEOF(ztcTransHdr) + STL_SIZEOF( stlInt32 ) ], aComment, aCommentLength );
                }
            }

            STL_TRY( ztcmPubData( &gMasterMgr->mCMMgr,
                                  gMasterMgr->mCMMgr.mPubContext,
                                  (stlChar*)sLogBuffer,
                                  STL_SIZEOF(ztcTransHdr) + sTransHdr->mSize,
                                  &sTrSeq,
                                  aErrorStack ) == STL_SUCCESS );

        }   //end of for loop
    }
        
    sIdx = 0;
    STL_RING_FOREACH_ENTRY( &aTransSlotHdr->mTransDataList, sTransData )
    {
        sDataPtr = (stlChar*)&sTransData->mTransHdr;

        if( sTransData->mTransHdr.mTablePId != ZTC_INVALID_TABLE_PHYSICAL_ID )
        {
            STL_TRY( ztcmFindCaptureTableInfo( &sTableInfo,
                                               sTransData->mTransHdr.mTablePId,
                                               aErrorStack ) == STL_SUCCESS );

            STL_DASSERT( sTableInfo != NULL );

            /**
             * Transaction에 포함된 Table의 CommitScn이 현재 Commit된 SCN 보다 크거나 같을 경우는
             * 이미 Slave에 반영된것 이기 때문에 Slave에 보내지 않는다. (SYNC 기능추가에 따른 구현인데 성능에 영향을 미칠수 있는 부분임..나중에 좀더
생각해 봐야겠음)
             */
            if( sTableInfo->mCommitSCN >= aCommitSCN &&
                sTableInfo->mCommitSCN != ZTC_INVALID_SCN )
            {
                continue;
            }
        }

        /**
          * Commit Comment 가 있을 경우 Transaction Begin에 넣어서 처리하도록 함
          */
        if( sTransData->mTransHdr.mLogType == ZTC_CAPTURE_TYPE_BEGIN )
        {
            sTransData->mTransHdr.mSize = sCommentLength;
            stlMemcpy( sLogBuffer, sDataPtr, STL_SIZEOF(ztcTransHdr) );
            STL_WRITE_INT32( &sLogBuffer[ STL_SIZEOF(ztcTransHdr) ], &aCommentLength );

            if( aCommentLength > 0 )
            {
                stlMemcpy( &sLogBuffer[ STL_SIZEOF(ztcTransHdr) + STL_SIZEOF( stlInt32 ) ], aComment, aCommentLength );
            }

            STL_TRY( ztcmPubData( &gMasterMgr->mCMMgr,
                                  gMasterMgr->mCMMgr.mPubContext,
                                  sLogBuffer,
                                  STL_SIZEOF(ztcTransHdr) + sCommentLength,
                                  &sTrSeq,
                                  aErrorStack ) == STL_SUCCESS );
        }
        else
        {
            STL_TRY( ztcmPubData( &gMasterMgr->mCMMgr,
                                  gMasterMgr->mCMMgr.mPubContext,
                                  sDataPtr,
                                  sTransData->mBufferSize,
                                  &sTrSeq,
                                  aErrorStack ) == STL_SUCCESS );
        }

        sIdx++;

//        if( sIdx % 10 == 0 )
//        {
//            STL_TRY( ztcmPubData( &gMasterMgr->mCMMgr,
//                                  gMasterMgr->mCMMgr.mPubContext,
//                                  NULL,
//                                  0,
//                                  ZTC_SEND_NOW,
//                                  "d",
//                                  aErrorStack ) == STL_SUCCESS );
//        }
    }

//    STL_TRY( ztcmPubData( &gMasterMgr->mCMMgr,
//                          gMasterMgr->mCMMgr.mPubContext,
//                          NULL,
//                          0,
//                          ZTC_SEND_NOW,
//                          "e",
//                          aErrorStack ) == STL_SUCCESS );

    stlAtomicInc64( &gCommitIdx );

    if( aTransSlotHdr->mFileLogCount > 0 )
    {    
        sState = 1;
        STL_TRY( stlCloseFile( &sFile,
                               aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( stlRemoveFile( sFileName,
                                aErrorStack ) == STL_SUCCESS );
        
    }
    
    sState = 0;
    stlFreeHeap( sLogBufferPtr );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_NOT_EXIST_FILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_FILE_NAME,
                      NULL,
                      aErrorStack,
                      sFileName );
    }
    STL_CATCH( RAMP_ERROR_FILE_READ )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_TEMP_FILE,
                      NULL,
                      aErrorStack,
                      sFileName );
    }
    STL_FINISH;
    
    switch( sState )
    {
        case 2:
            (void) stlCloseFile( &sFile,
                             aErrorStack );
        case 1:
            stlFreeHeap( sLogBufferPtr );
        default:
            break;
    }
        
    return STL_FAILURE;
}



/** @} */

