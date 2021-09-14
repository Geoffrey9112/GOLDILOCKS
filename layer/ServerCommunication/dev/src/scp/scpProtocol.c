/*******************************************************************************
 * scpProtocol.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: scpProtocol.c 6676 2012-12-13 08:39:46Z egon $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stl.h>
#include <dtl.h>
#include <cml.h>
#include <knl.h>
#include <sclDef.h>
#include <scDef.h>
#include <cmlGeneral.h>
#include <sccCommunication.h>
#include <scpProtocol.h>

/**
 * @file scpProtocol.c
 * @brief Communication Layer Protocol Component Routines
 */

/**
 * @addtogroup scpProtocol
 * @{
 */


stlStatus scpBeginReading( sclHandle       * aHandle,
                           scpCursor       * aCursor,
                           sclEnv          * aEnv )
{
    sclProtocolSentence * sProtocolSentence = aHandle->mProtocolSentence;

    aCursor->mCurPos = sProtocolSentence->mReadProtocolPos;
    aCursor->mEndPos = sProtocolSentence->mReadEndPos;

    return STL_SUCCESS;
}

stlStatus scpEndReading( sclHandle       * aHandle,
                         scpCursor       * aCursor,
                         sclEnv          * aEnv )
{
    sclProtocolSentence * sProtocolSentence = aHandle->mProtocolSentence;

    sProtocolSentence->mReadProtocolPos = aCursor->mCurPos;

    return STL_SUCCESS;
}

stlStatus scpBeginWriting( sclHandle       * aHandle,
                           scpCursor       * aCursor,
                           sclEnv          * aEnv )
{
    sclProtocolSentence * sProtocolSentence = aHandle->mProtocolSentence;

    aCursor->mCurPos = sProtocolSentence->mWriteProtocolPos;
    aCursor->mEndPos = sProtocolSentence->mWriteStartPos + sProtocolSentence->mMemoryMgr->mPacketBufferSize;

    return STL_SUCCESS;
}

stlStatus scpEndWriting( sclHandle       * aHandle,
                         scpCursor       * aCursor,
                         sclEnv          * aEnv )
{
    sclProtocolSentence * sProtocolSentence = aHandle->mProtocolSentence;

    sProtocolSentence->mWriteProtocolPos = aCursor->mCurPos;

    return STL_SUCCESS;
}


stlStatus scpSkipNBytes( sclHandle       * aHandle,
                         scpCursor       * aCursor,
                         stlInt32          aLength,
                         sclEnv          * aEnv )
{
    stlInt32 sLen;

    while( aLength > 0 )
    {
        sLen = STL_MIN( SCP_READABLE_SIZE( aCursor ), aLength );

        aLength -= sLen;
        aCursor->mCurPos += sLen;

        if( aLength == 0 )
        {
            break;
        }

        STL_TRY( scpEndReading( aHandle, aCursor, aEnv ) == STL_SUCCESS );
        STL_TRY( sccRecvPacket( aHandle, aEnv ) == STL_SUCCESS );
        STL_TRY( scpBeginReading( aHandle, aCursor, aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus scpReadNBytes( sclHandle       * aHandle,
                         scpCursor       * aCursor,
                         stlChar         * aValue,
                         stlInt32          aLength,
                         sclEnv          * aEnv )
{
    stlInt32 sLen;

    while( aLength > 0 )
    {
        sLen = STL_MIN( SCP_READABLE_SIZE( aCursor ), aLength );

        STL_WRITE_BYTES( aValue, aCursor->mCurPos, sLen );

        aLength -= sLen;
        aCursor->mCurPos += sLen;
        aValue += sLen;

        if( aLength == 0 )
        {
            break;
        }

        STL_TRY( scpEndReading( aHandle, aCursor, aEnv ) == STL_SUCCESS );
        STL_TRY( sccRecvPacket( aHandle, aEnv ) == STL_SUCCESS );
        STL_TRY( scpBeginReading( aHandle, aCursor, aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus scpWriteNBytes( sclHandle       * aHandle,
                          scpCursor       * aCursor,
                          stlChar         * aValue,
                          stlInt32          aLength,
                          sclEnv          * aEnv )
{
    stlInt32 sLen;

    while( aLength > 0 )
    {
        sLen = STL_MIN( SCP_WRITABLE_SIZE( aCursor ), aLength );

        STL_WRITE_BYTES( aCursor->mCurPos, aValue, sLen );

        aLength -= sLen;
        aCursor->mCurPos += sLen;
        aValue += sLen;

        if( aLength == 0 )
        {
            break;
        }

        STL_TRY( scpEndWriting( aHandle, aCursor, aEnv ) == STL_SUCCESS );
        STL_TRY( sccFlushPacket( aHandle, aEnv ) == STL_SUCCESS );
        STL_TRY( scpBeginWriting( aHandle, aCursor, aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 이 함수는 preHandshake에서도 불릴수 있음으로
 *        endian 고려해야 해서 SCP_WRITE_xxx_ENDIAN 사용함.
 */
stlStatus scpWriteErrorResult( sclHandle       * aHandle,
                               scpCursor       * aCursor,
                               stlErrorStack   * aErrorStack,
                               sclEnv          * aEnv )
{
    stlErrorData * sErrorData;
    stlInt8        sErrorCount;
    stlInt8        sErrorLevel = CML_RESULT_SUCCESS;
    stlInt16       sErrorIdx = -1;
    stlInt32       sMessageLen;
    stlInt32       sDetailMessageLen;

    if( STL_HAS_ERROR( aErrorStack ) == STL_FALSE )
    {
        SCP_WRITE_INT8( aHandle, aCursor, &sErrorLevel, aEnv );
    }
    else
    {
        sErrorCount = stlGetErrorStackDepth( aErrorStack );
        sErrorData  = stlGetLastErrorData( aErrorStack );

        sErrorLevel = CML_RESULT_WARNING;
        STL_DASSERT( sErrorData->mErrorLevel == STL_ERROR_LEVEL_WARNING );
        
        SCP_WRITE_INT8( aHandle, aCursor, &sErrorLevel, aEnv );
        SCP_WRITE_INT8( aHandle, aCursor, &sErrorCount, aEnv );

        sErrorData = stlGetFirstErrorData( aErrorStack, &sErrorIdx );

        while( sErrorData != NULL )
        {
            sMessageLen = stlStrlen( sErrorData->mErrorMessage );
            sDetailMessageLen = stlStrlen( sErrorData->mDetailErrorMessage );
            
            SCP_WRITE_INT32_ENDIAN( aHandle, aCursor, &sErrorData->mExternalErrorCode, aEnv );
            SCP_WRITE_INT32_ENDIAN( aHandle, aCursor, &sErrorData->mErrorCode, aEnv );
            SCP_WRITE_N_VAR_ENDIAN( aHandle, aCursor, sErrorData->mErrorMessage, sMessageLen, aEnv );
            SCP_WRITE_N_VAR_ENDIAN( aHandle, aCursor, sErrorData->mDetailErrorMessage, sDetailMessageLen, aEnv );

            sErrorData = stlGetNextErrorData( aErrorStack, &sErrorIdx );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus scpWriteXaErrorResult( sclHandle       * aHandle,
                                 scpCursor       * aCursor,
                                 stlInt32          aXaError,
                                 stlErrorStack   * aErrorStack,
                                 sclEnv          * aEnv )
{
    stlErrorData * sErrorData;
    stlInt8        sErrorCount;
    stlInt8        sErrorLevel = CML_RESULT_SUCCESS;
    stlInt16       sErrorIdx = -1;
    stlInt32       sMessageLen;
    stlInt32       sDetailMessageLen;

    if( STL_HAS_ERROR( aErrorStack ) == STL_FALSE )
    {
        SCP_WRITE_INT8( aHandle, aCursor, &sErrorLevel, aEnv );
        SCP_WRITE_INT32( aHandle, aCursor, &aXaError, aEnv );
    }
    else
    {
        sErrorCount = stlGetErrorStackDepth( aErrorStack );
        sErrorData  = stlGetLastErrorData( aErrorStack );

        sErrorLevel = CML_RESULT_WARNING;
        STL_DASSERT( sErrorData->mErrorLevel == STL_ERROR_LEVEL_WARNING );
        
        SCP_WRITE_INT8( aHandle, aCursor, &sErrorLevel, aEnv );
        SCP_WRITE_INT32( aHandle, aCursor, &aXaError, aEnv );
        SCP_WRITE_INT8( aHandle, aCursor, &sErrorCount, aEnv );

        sErrorData = stlGetFirstErrorData( aErrorStack, &sErrorIdx );

        while( sErrorData != NULL )
        {
            sMessageLen = stlStrlen( sErrorData->mErrorMessage );
            sDetailMessageLen = stlStrlen( sErrorData->mDetailErrorMessage );
            
            SCP_WRITE_INT32( aHandle, aCursor, &sErrorData->mExternalErrorCode, aEnv );
            SCP_WRITE_INT32( aHandle, aCursor, &sErrorData->mErrorCode, aEnv );
            SCP_WRITE_N_VAR( aHandle, aCursor, sErrorData->mErrorMessage, sMessageLen, aEnv );
            SCP_WRITE_N_VAR( aHandle, aCursor, sErrorData->mDetailErrorMessage, sDetailMessageLen, aEnv );

            sErrorData = stlGetNextErrorData( aErrorStack, &sErrorIdx );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Socket Handle로 부터 Send pooling buffer chunk를 할당한다.
 * @param[in]  aHandle     Communication Handle
 * @param[in]  aEnv        env
 */
stlStatus scpAllocSendPoolingBuf( sclHandle     * aHandle,
                                  sclEnv        * aEnv )
{
    sclSendPoolingBufChunk * sChunk;
    sclProtocolSentence    * sProtocolSentence = aHandle->mProtocolSentence;
    sclMemoryManager       * sMemoryMgr = sProtocolSentence->mMemoryMgr;

    STL_TRY( knlAllocRegionMem( &sMemoryMgr->mSendPoolingBufAllocator,
                                sMemoryMgr->mPollingBufferSize + STL_SIZEOF( sclSendPoolingBufChunk ),
                                (void**)&sChunk,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sChunk->mBuf = ((stlChar*)sChunk) + STL_SIZEOF( sclSendPoolingBufChunk );
    sChunk->mNext = NULL;

    if( sProtocolSentence->mSendPoolingBufHead == NULL )
    {
        sProtocolSentence->mSendPoolingBufHead = sChunk;
    }
    else
    {
        sProtocolSentence->mSendPoolingBufCur->mNext = sChunk;
    }
    sProtocolSentence->mSendPoolingBufCur = sChunk;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Socket Handle의 Send pooling buffer chunk를 모두 무효화한다.
 * @param[in]  aHandle     Communication Handle
 * @param[in]  aEnv        env
 */
stlStatus scpRestoreSendPoolingBuf( sclHandle     * aHandle,
                                    sclEnv        * aEnv )
{
    sclProtocolSentence    * sProtocolSentence = aHandle->mProtocolSentence;
    sclMemoryManager       * sMemoryMgr = sProtocolSentence->mMemoryMgr;

    STL_TRY( knlFreeUnmarkedRegionMem( &sMemoryMgr->mSendPoolingBufAllocator,
                                       &sMemoryMgr->mSendPoolingBufMark,
                                       STL_FALSE,  /* aIsFree */
                                       KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sProtocolSentence->mSendPoolingBufHead = NULL;
    sProtocolSentence->mSendPoolingBufCur  = NULL;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */

