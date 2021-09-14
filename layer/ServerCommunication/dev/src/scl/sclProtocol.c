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
#include <knl.h>
#include <sclDef.h>
#include <scDef.h>
#include <scpProtocol.h>
#include <sclGeneral.h>
#include <sclProtocol.h>
#include <sccCommunication.h>

/**
 * @file scpProtocol.c
 * @brief Server Communication Layer Protocol Routines
 */

/**
 * @addtogroup scpProtocol
 * @{
 */

/**
 * @brief Server Communication Handle을 초기화 한다.
 * @param[in] aHandle        Server Communication Handle
 * @param[in] aContext       socket Context
 * @param[in] aIpc           ipc
 * @param[in] aMajorVersion  protocol major version
 * @param[in] aMinorVersion  protocol minor version
 * @param[in] aPatchVersion  protocol patch version
 * @param[in] aEnv           env
 * @remark : aContext, aIpc는 둘중에 하나만 설정되어야 함.
 */
stlStatus sclInitializeHandle( sclHandle        * aHandle,
                               stlContext       * aContext,
                               sclIpc           * aIpc,
                               stlInt16           aMajorVersion,
                               stlInt16           aMinorVersion,
                               stlInt16           aPatchVersion,
                               sclEnv           * aEnv )
{
    if( aContext != NULL )
    {
        STL_PARAM_VALIDATE( aIpc == NULL, KNL_ERROR_STACK(aEnv) );

        aHandle->mCommunication.mType = SCL_COMMUNICATION_TCP;
        aHandle->mCommunication.mContext = aContext;
        aHandle->mSentPacketCount = 0;
    }
    else if( aIpc != NULL )
    {
        STL_PARAM_VALIDATE( aContext == NULL, KNL_ERROR_STACK(aEnv) );

        aHandle->mCommunication.mType = SCL_COMMUNICATION_IPC;
        aHandle->mCommunication.mIpc = aIpc;
        aHandle->mSentPacketCount = 0;
    }
    else
    {
        STL_PARAM_VALIDATE( STL_FALSE, KNL_ERROR_STACK(aEnv) );
    }

    aHandle->mEndian = STL_PLATFORM_ENDIAN;
    aHandle->mSocketPeriod = STL_FALSE;
    aHandle->mMajorVersion = aMajorVersion;
    aHandle->mMinorVersion = aMinorVersion;
    aHandle->mPatchVersion = aPatchVersion;
    aHandle->mProtocolSentence = NULL;

    /**
     * listener에서 sequence증가됨으로 여기서는 1로 설정한다.
     */
    aHandle->mSendSequence = 1;
    aHandle->mRecvSequence = 1;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Server Communication Handle을 삭제한다.
 * @param[in] aHandle       Server Communication Handle
 * @param[in] aEnv          env
 */
stlStatus sclFinalizeHandle( sclHandle     * aHandle,
                             sclEnv        * aEnv )
{
    aHandle->mProtocolSentence = NULL;

    return STL_SUCCESS;
}


/**
 * @brief sclProtocolSentence을 초기화 한다.
 * @param[in] aHandle               Communication Handle
 * @param[in] aProtocolSentence     sclProtocolSentence
 * @param[in] aMemoryMgr            memory manager
 * @param[in] aEnv                  env
 * @remark : aProtocolSentence, aMemoryMgr, aBufferSize를 그대로 사용할경우 NULL을 입력한다.
 */
stlStatus sclInitializeProtocolSentence( sclHandle               * aHandle,
                                         sclProtocolSentence     * aProtocolSentence,
                                         sclMemoryManager        * aMemoryMgr,
                                         sclEnv                  * aEnv )
{
    sclProtocolSentence     * sProtocolSentence = NULL;

    STL_DASSERT( aHandle->mProtocolSentence == NULL );

    aHandle->mProtocolSentence = aProtocolSentence;
    aProtocolSentence->mMemoryMgr = aMemoryMgr;

    sProtocolSentence = aHandle->mProtocolSentence;
    sProtocolSentence->mPendingCmUnitCount = 0;

    /**
     * 한 패킷 안에서만 유효한 cached statement id를 무효화 한다.
     * => 여기서 mCachedStatementId = CML_STATEMENT_ID_UNDEFINED 로 설정하는게 좋을것 같음.
     */
    sProtocolSentence->mCachedStatementId = CML_STATEMENT_ID_UNDEFINED;
    sProtocolSentence->mPoolingFlag = STL_FALSE;


    sProtocolSentence->mSendPoolingBufHead = NULL;
    sProtocolSentence->mSendPoolingBufCur  = NULL;

    if( aHandle->mCommunication.mType == SCL_COMMUNICATION_TCP )
    {
        /**
         * TCP의 경우는 MemoryMgr에서 할당해 놓은 mWriteBuffer, mReadBuffer를 재사용하여
         *  mWriteStartPos, mReadStartPos에 설정하여 사용한다.
         */
        sProtocolSentence->mWriteStartPos = aMemoryMgr->mWriteBuffer;
        sProtocolSentence->mReadStartPos = aMemoryMgr->mReadBuffer;
    }
    else
    {
        /**
         * zerocopy의 경우 받을 버퍼를 가지고 data를 받는것이 아니라
         * sclRecvPacket시에 data가 받아져 있는 곳을 설정해야 함으로
         * 여기서 buffer를 설정하지 않고 sclRecvPacket시에 mReadStartPos를 설정한다.
         * 현재 IPC는 zerocopy 방식을 사용한다.
         */
        sProtocolSentence->mReadStartPos = NULL;

        /* ipc의 경우 responseIpc로 부터 send할 버퍼를 가져와서 mWriteStartPos에 설정한다. */
        STL_TRY( sccGetSendBuffer( &aHandle->mCommunication.mIpc->mResponseIpc,
                                   aHandle->mCommunication.mIpc->mArrayAllocator,
                                   &sProtocolSentence->mWriteStartPos,
                                   aEnv )
                 == STL_SUCCESS );
    }

    sProtocolSentence->mReadProtocolPos = sProtocolSentence->mReadStartPos + CML_PACKET_HEADER_SIZE;
    sProtocolSentence->mReadEndPos = sProtocolSentence->mReadStartPos + CML_PACKET_HEADER_SIZE;

    sProtocolSentence->mWriteProtocolPos = sProtocolSentence->mWriteStartPos + CML_PACKET_HEADER_SIZE;
    sProtocolSentence->mWriteEndPos = sProtocolSentence->mWriteStartPos + aMemoryMgr->mPacketBufferSize;


    return STL_SUCCESS;

    STL_FINISH;

    /**
     * 이 함수에서 sccGetSendBuffer는 cache에서 buffer를 가져옴으로 실패할수 없다.
     */
    STL_DASSERT( STL_FALSE );

    return STL_FAILURE;
}


/**
 * @brief ProtocolSentence을 삭제한다.
 * @param[in] aProtocolSentence     sclProtocolSentence
 * @param[in] aEnv                  env
 */
stlStatus sclFinalizeProtocolSentence( sclProtocolSentence      * aProtocolSentence,
                                       sclEnv                   * aEnv )
{

    return STL_SUCCESS;
}


/**
 * @brief allocator을 초기화 한다.
 * @param[in] aMemoryMgr            sclMemoryMgr
 * @param[in] aPacketBufferSize     packet BufferSize
 * @param[in] aOperationBufferSize  Operation BufferSize
 * @param[in] aPollingBufferSize    Polling BufferSize
 * @param[in] aEnv                  env
 */
stlStatus sclInitializeMemoryManager( sclMemoryManager  * aMemoryMgr,
                                      stlInt64            aPacketBufferSize,
                                      stlInt64            aOperationBufferSize,
                                      stlInt64            aPollingBufferSize,
                                      sclEnv            * aEnv )
{
    stlInt32                sState = 0;

    STL_DASSERT( aPacketBufferSize != 0 );

    STL_TRY( knlCreateRegionMem( &aMemoryMgr->mAllocator,
                                 KNL_ALLOCATOR_SERVER_COMMUNICATION_REGION_SESSION,
                                 NULL,  /* aParentMem */
                                 KNL_MEM_STORAGE_TYPE_HEAP,
                                 aPacketBufferSize * 2,
                                 aOperationBufferSize,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;

    aMemoryMgr->mPacketBufferSize = aPacketBufferSize;

    STL_TRY( knlAllocRegionMem( &aMemoryMgr->mAllocator,
                                aPacketBufferSize,
                                (void**)&aMemoryMgr->mReadBuffer,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlAllocRegionMem( &aMemoryMgr->mAllocator,
                                aPacketBufferSize,
                                (void**)&aMemoryMgr->mWriteBuffer,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    aMemoryMgr->mOperationBufferSize = aOperationBufferSize;

    KNL_INIT_REGION_MARK( &(aMemoryMgr->mOperationMark) );

    STL_TRY( knlMarkRegionMem( &aMemoryMgr->mAllocator,
                               &aMemoryMgr->mOperationMark,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlAllocRegionMem( &aMemoryMgr->mAllocator,
                                aOperationBufferSize,
                                (void**)&aMemoryMgr->mOperationBuffer,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    aMemoryMgr->mPollingBufferSize = aPollingBufferSize;

    if( aPollingBufferSize != 0 )
    {
        STL_TRY( knlCreateRegionMem( &aMemoryMgr->mSendPoolingBufAllocator,
                                     KNL_ALLOCATOR_SERVER_COMMUNICATION_REGION_SEND_POLLING,
                                     NULL,  /* aParentMem */
                                     KNL_MEM_STORAGE_TYPE_HEAP,
                                     aPollingBufferSize + STL_SIZEOF( sclSendPoolingBufChunk ),
                                     aPollingBufferSize + STL_SIZEOF( sclSendPoolingBufChunk ),
                                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        sState = 2;

        KNL_INIT_REGION_MARK( &(aMemoryMgr->mSendPoolingBufMark) );

        STL_TRY( knlMarkRegionMem( &aMemoryMgr->mSendPoolingBufAllocator,
                                   &aMemoryMgr->mSendPoolingBufMark,
                                   KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)knlDestroyRegionMem( &aMemoryMgr->mSendPoolingBufAllocator,
                                       KNL_ENV(aEnv) );
        case 1:
            (void)knlDestroyRegionMem( &aMemoryMgr->mAllocator,
                                       KNL_ENV(aEnv) );
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief allocator을 삭제한다.
 * @param[in] aMemoryMgr sclMemoryMgr
 * @param[in] aEnv       env
 */
stlStatus sclFinalizeMemoryManager( sclMemoryManager  * aMemoryMgr,
                                    sclEnv            * aEnv )
{
    if( aMemoryMgr->mPollingBufferSize != 0 )
    {
        STL_TRY( knlDestroyRegionMem( &aMemoryMgr->mSendPoolingBufAllocator,
                                      KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    STL_TRY( knlDestroyRegionMem( &aMemoryMgr->mAllocator,
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief data에서 command를 가져온다
 * @param[in] aHandle           scl handle
 * @param[in] aTimeout          timeout
 * @param[out] aIsTimeout       timeout여부
 * @param[out] aCommandType     command type
 * @param[in] aErrorStack       error stack
 */
stlStatus sclMoveNextProtocol( sclHandle       * aHandle,
                               stlTime           aTimeout,
                               stlBool         * aIsTimeout,
                               cmlCommandType  * aCommandType,
                               stlErrorStack   * aErrorStack )
{
    stlInt32                sSigFdNum;
    sclProtocolSentence   * sProtocolSentence = aHandle->mProtocolSentence;

    *aIsTimeout = STL_FALSE;

    if( sProtocolSentence->mReadEndPos <= sProtocolSentence->mReadProtocolPos )
    {
        /* 읽을 data가 없고 이전에 CML_PERIOD_CONTINUE 였으면 packet을 다시 읽는다 */
        if( sProtocolSentence->mReadPeriod == CML_PERIOD_CONTINUE )
        {
            if( stlPoll( &aHandle->mCommunication.mContext->mPollFd,
                         1,
                         &sSigFdNum,
                         aTimeout,
                         aErrorStack ) == STL_FAILURE )
            {
                STL_TRY( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_TIMEDOUT );

                (void)stlPopError( aErrorStack );
                *aIsTimeout = STL_TRUE;

                STL_THROW( RAMP_SUCCESS );
            }

            /**
             * read next packet
             */

            STL_TRY( sccRecvFromSocket( aHandle, aErrorStack ) == STL_SUCCESS );

            sProtocolSentence->mReadProtocolPos = sProtocolSentence->mReadStartPos + CML_PACKET_HEADER_SIZE;
            sProtocolSentence->mReadEndPos = sProtocolSentence->mReadProtocolPos + sProtocolSentence->mReadPayloadSize;
        }
        /* 읽을 data가 없고 이전 packet이 CML_PERIOD_END 였으면 command를 CML_COMMAND_PERIOD로 설정한다 */
        else
        {
            *aCommandType = CML_COMMAND_PERIOD;

            /*
             * 다음 패킷을 받기 위해 period continue 상태로 돌린다.
             */
            sProtocolSentence->mReadPeriod = CML_PERIOD_CONTINUE;
            STL_THROW( RAMP_SUCCESS );
        }
    }

    STL_TRY( sclParseCommand( sProtocolSentence,
                              aCommandType,
                              aErrorStack )
             == STL_SUCCESS );

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief data에서 command를 가져온다
 * @param[in] aProtocolSentence protocol sentence
 * @param[out] aCommandType     command type
 * @param[in] aErrorStack       error stack
 */
stlStatus sclParseCommand( sclProtocolSentence  * aProtocolSentence,
                           cmlCommandType       * aCommandType,
                           stlErrorStack        * aErrorStack )
{
    stlInt8         sCommandType8;
    stlInt16        sCommandType16;
    stlChar       * sData = aProtocolSentence->mReadProtocolPos;

    STL_WRITE_INT8( &sCommandType8, sData );

    if( sCommandType8 == SCP_VAR_INT_2BYTE_FLAG )
    {
        STL_WRITE_INT16( &sCommandType16, sData + 1 );
        *aCommandType = (cmlCommandType)sCommandType16;
    }
    else
    {
        *aCommandType = sCommandType8;
    }

    return STL_SUCCESS;
}


/**
 * @brief packet에서 command를 하나 꺼낸다.
 * @param[in] aHandle             communication handle
 * @param[out] aCommandType       command type
 * @param[in] aEnv                env
 */
stlStatus sclGetCommand( sclHandle      * aHandle,
                         cmlCommandType * aCommandType,
                         sclEnv         * aEnv )
{
    sclProtocolSentence * sProtocolSentence = aHandle->mProtocolSentence;

    STL_DASSERT( aHandle->mCommunication.mType == SCL_COMMUNICATION_IPC );

    if( sProtocolSentence->mReadEndPos == sProtocolSentence->mReadProtocolPos )
    {
        /* buffer에 읽을 data가 없음 */

        /* CML_PERIOD_END 이면 CML_COMMAND_PERIOD command로 설정한다. */
        if( sProtocolSentence->mReadPeriod == CML_PERIOD_END )
        {
            STL_TRY( sccCompleteRecvBuffer( &aHandle->mCommunication.mIpc->mRequestIpc,
                                            aHandle->mCommunication.mIpc->mArrayAllocator,
                                            sProtocolSentence->mReadStartPos,
                                            aEnv )
                     == STL_SUCCESS );

            *aCommandType = CML_COMMAND_PERIOD;

            sProtocolSentence->mReadProtocolPos = NULL;
            sProtocolSentence->mReadEndPos = NULL;

            STL_THROW( RAMP_SUCCESS );
        }

        /* 다음 버퍼를 읽는다. */
        STL_TRY( sccRecvPacket( aHandle,
                                aEnv ) == STL_SUCCESS );
    }

    STL_ASSERT( sProtocolSentence->mReadEndPos > sProtocolSentence->mReadProtocolPos );

    /* buffer에 읽을 data가 있음 */
    STL_TRY( sclParseCommand( sProtocolSentence,
                              aCommandType,
                              KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    STL_RAMP( RAMP_SUCCESS );

    STL_ASSERT( *aCommandType < CML_COMMAND_MAX );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief packet을 recv하여 ProtocolSentence의 buffer에 저장한다.
 * @param[in] aHandle       Communication Handle
 * @param[in] aEnv          env
 */
stlStatus sclRecvPacket( sclHandle     * aHandle,
                         sclEnv        * aEnv )
{
    return sccRecvPacket( aHandle,
                          aEnv );
}

/**
 * @brief ProtocolSentence의 buffer로 packet send한다.
 * @param[in] aHandle       Communication Handle
 * @param[in] aEnv          env
 */
stlStatus sclSendPacket( sclHandle     * aHandle,
                         sclEnv        * aEnv )
{
    return sccSendPacket( aHandle,
                          aEnv );
}


/**
 * @brief Error Result Protocol을 기록한다.
 * @param[in] aHandle          Communication Handle
 * @param[in] aResultType      Result Type
 * @param[in] aErrorStack      기록될 error stack
 * @param[in] aEnv             env
 *
 * @par protocol sequence:
 * @code
 *
 * - result (t)ype  : result type         2 bytes
 * - erro(r) level  : error level         1 byte
 * - error (c)ount  : session identifier  1 bytes
 * - (s)ql state    : sql state           4 bytes
 * - n(a)tive error : native error        4 bytes
 * - (m)essage text : message text        variable
 *
 * field sequence : t-r-c-[s-a-m]^c
 *
 * @endcode
 */
stlStatus sclWriteErrorResult( sclHandle       * aHandle,
                               cmlCommandType    aResultType,
                               stlErrorStack   * aErrorStack,
                               sclEnv          * aEnv )
{
    scpCursor      sCursor;
    stlErrorData * sErrorData;
    stlInt8        sErrorCount;
    stlInt8        sErrorLevel;
    stlInt16       sResultType = aResultType;
    stlInt32       sMessageLen;
    stlInt32       sDetailMessageLen;
    stlInt32       i;

    STL_TRY( scpBeginWriting( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS);

    SCP_WRITE_VAR_INT16( aHandle, &sCursor, &sResultType, aEnv );

    sErrorCount = stlGetErrorStackDepth( aErrorStack );

    sErrorLevel = CML_RESULT_ABORT;

    SCP_WRITE_INT8( aHandle, &sCursor, &sErrorLevel, aEnv );
    SCP_WRITE_INT8( aHandle, &sCursor, &sErrorCount, aEnv );

    for( i = 0; i < sErrorCount; i++ )
    {
        sErrorData = &(aErrorStack->mErrorData[i]);
        sMessageLen = stlStrlen( sErrorData->mErrorMessage );
        sDetailMessageLen = stlStrlen( sErrorData->mDetailErrorMessage );

        SCP_WRITE_INT32( aHandle, &sCursor, &sErrorData->mExternalErrorCode, aEnv );
        SCP_WRITE_INT32( aHandle, &sCursor, &sErrorData->mErrorCode, aEnv );
        SCP_WRITE_N_VAR( aHandle, &sCursor, sErrorData->mErrorMessage, sMessageLen, aEnv );
        SCP_WRITE_N_VAR( aHandle, &sCursor, sErrorData->mDetailErrorMessage, sDetailMessageLen, aEnv );
    }

    STL_TRY( scpEndWriting( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief XA Error Result Protocol을 기록한다.
 * @param[in] aHandle          Communication Handle
 * @param[in] aResultType      Result Type
 * @param[in] aXaError         XA Error Number
 * @param[in] aErrorStack      기록될 error stack
 * @param[in] aEnv             env
 *
 * @par protocol sequence:
 * @code
 *
 * - result (t)ype  : result type         2 bytes
 * - erro(r) level  : error level         1 byte
 * - (x)a error     : xa error number     4 bytes
 * - error (c)ount  : error count         1 bytes
 * - (s)ql state    : sql state           4 bytes
 * - n(a)tive error : native error        4 bytes
 * - (m)essage text : message text        variable
 *
 * field sequence : t-r-x-c-[s-a-m]^c
 *
 * @endcode
 */
stlStatus sclWriteXaErrorResult( sclHandle       * aHandle,
                                 cmlCommandType    aResultType,
                                 stlInt32          aXaError,
                                 stlErrorStack   * aErrorStack,
                                 sclEnv          * aEnv )
{
    scpCursor      sCursor;
    stlErrorData * sErrorData;
    stlInt8        sErrorCount;
    stlInt8        sErrorLevel;
    stlInt16       sResultType = aResultType;
    stlInt32       sMessageLen;
    stlInt32       sDetailMessageLen;
    stlInt32       i;

    STL_TRY( scpBeginWriting( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS);

    SCP_WRITE_VAR_INT16( aHandle, &sCursor, &sResultType, aEnv );

    sErrorCount = stlGetErrorStackDepth( aErrorStack );

    sErrorLevel = CML_RESULT_ABORT;

    SCP_WRITE_INT8( aHandle, &sCursor, &sErrorLevel, aEnv );
    SCP_WRITE_INT32( aHandle, &sCursor, &aXaError, aEnv );
    SCP_WRITE_INT8( aHandle, &sCursor, &sErrorCount, aEnv );

    for( i = 0; i < sErrorCount; i++ )
    {
        sErrorData = &(aErrorStack->mErrorData[i]);
        sMessageLen = stlStrlen( sErrorData->mErrorMessage );
        sDetailMessageLen = stlStrlen( sErrorData->mDetailErrorMessage );

        SCP_WRITE_INT32( aHandle, &sCursor, &sErrorData->mExternalErrorCode, aEnv );
        SCP_WRITE_INT32( aHandle, &sCursor, &sErrorData->mErrorCode, aEnv );
        SCP_WRITE_N_VAR( aHandle, &sCursor, sErrorData->mErrorMessage, sMessageLen, aEnv );
        SCP_WRITE_N_VAR( aHandle, &sCursor, sErrorData->mDetailErrorMessage, sDetailMessageLen, aEnv );
    }

    STL_TRY( scpEndWriting( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Ignore Result Protocol을 기록한다.
 * @param[in] aHandle          Communication Handle
 * @param[in] aResultType      Result Type
 * @param[in] aEnv      env
 *
 * @par protocol sequence:
 * @code
 *
 * - result (t)ype  : result type         2 bytes
 * - erro(r) level  : error level         1 byte
 *
 * field sequence : t-r
 *
 * @endcode
 */
stlStatus sclWriteIgnoreResult( sclHandle       * aHandle,
                                cmlCommandType    aResultType,
                                sclEnv          * aEnv )
{
    scpCursor      sCursor;
    stlInt8        sErrorLevel;
    stlInt16       sResultType = aResultType;

    STL_TRY( scpBeginWriting( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS);

    SCP_WRITE_VAR_INT16( aHandle, &sCursor, &sResultType, aEnv );
    sErrorLevel = CML_RESULT_IGNORE;
    SCP_WRITE_INT8( aHandle, &sCursor, &sErrorLevel, aEnv );

    STL_TRY( scpEndWriting( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
