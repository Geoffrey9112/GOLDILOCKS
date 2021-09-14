/*******************************************************************************
 * cmlGeneral.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: cmlGeneral.c 6676 2012-12-13 08:39:46Z egon $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stl.h>
#include <dtl.h>
#include <cmlDef.h>
#include <cmDef.h>
#include <cmgSocket.h>
#include <cmlGeneral.h>
#include <cmgMemory.h>
#include <cmgIpc.h>

/**
 * @file cmlGeneral.c
 * @brief Communication Layer General Routines
 */



/**
 * @addtogroup cmlGeneral
 * @{
 */

stlStatus cmlInitSockOption( stlContext    * aContext,
                             stlInt32        aBufferSize,
                             stlErrorStack * aErrorStack )
{

#if 0   /* TODO : 성능 test 필요함 */
    /**
     *  shared모드에서는 dispatcher에서 fd받으면 SocketOption 설정함
     *  shared모드인 경우 shared-server는 socket을 가지고 있지 않음.
     */
    if( aHandle->mCommunication.mType == CML_COMMUNICATION_TCP )
    {
        STL_TRY( stlSetSocketOption( STL_SOCKET_IN_CONTEXT(*aHandle->mCommunication.mContext),
                                     STL_SOPT_SO_SNDBUF,
                                     aBufferSize,
                                     aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( stlSetSocketOption( STL_SOCKET_IN_CONTEXT(*aHandle->mCommunication.mContext),
                                     STL_SOPT_SO_RCVBUF,
                                     aBufferSize,
                                     aErrorStack )
                 == STL_SUCCESS );
    }
#endif

    return STL_SUCCESS;
}



/**
 * @brief Communication Layer를 초기화한다.
 */
stlStatus cmlInitialize()
{
    if( stlGetProcessInitialized( STL_LAYER_COMMUNICATION ) == STL_FALSE )
    {
        /**
         * 하위 Layer 초기화
         */

        STL_TRY( dtlInitialize() == STL_SUCCESS );

        /**
         * Kernel Layer 에러 테이블을 등록한다.
         */
    
        stlRegisterErrorTable( STL_ERROR_MODULE_COMMUNICATION, gCommunicationErrorTable );

        /**
         * DataType Layer 에러 테이블을 등록한다.
         */
    
        stlSetProcessInitialized( STL_LAYER_COMMUNICATION );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Communication Layer를 종료한다.
 */
stlStatus cmlTerminate()
{
    /**
     * nothing to do
     */
    
    /**
     * 하위 Layer 종료
     */
    
    STL_TRY( dtlTerminate() == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief allocator을 초기화 한다.
 * @param[in] aMemoryMgr            cmlMemoryMgr
 * @param[in] aPacketBufferSize     packet BufferSize
 * @param[in] aOperationBufferSize  Operation BufferSize
 * @param[in] aErrorStack           Error Stack Pointer
 */
stlStatus cmlInitializeMemoryManager( cmlMemoryManager  * aMemoryMgr,
                                      stlInt64            aPacketBufferSize,
                                      stlInt64            aOperationBufferSize,
                                      stlErrorStack     * aErrorStack )
{
    stlInt32                sState = 0;

    STL_DASSERT( aPacketBufferSize != 0 );

    STL_TRY( stlCreateRegionAllocator( &aMemoryMgr->mAllocator,
                                       aPacketBufferSize * 2,
                                       aOperationBufferSize,
                                       aErrorStack )
             == STL_SUCCESS );
    sState = 1;

    aMemoryMgr->mPacketBufferSize = aPacketBufferSize;

    STL_TRY( stlAllocRegionMem( &aMemoryMgr->mAllocator,
                                aPacketBufferSize,
                                (void**)&aMemoryMgr->mReadBuffer,
                                aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlAllocRegionMem( &aMemoryMgr->mAllocator,
                                aPacketBufferSize,
                                (void**)&aMemoryMgr->mWriteBuffer,
                                aErrorStack )
             == STL_SUCCESS );

    aMemoryMgr->mOperationBufferSize = aOperationBufferSize;

    if( aOperationBufferSize != 0 )
    {
        STL_TRY( stlMarkRegionMem( &aMemoryMgr->mAllocator,
                                   &aMemoryMgr->mOperationMark,
                                   aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( stlAllocRegionMem( &aMemoryMgr->mAllocator,
                                    aOperationBufferSize,
                                    (void**)&aMemoryMgr->mOperationBuffer,
                                    aErrorStack )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)stlDestroyRegionAllocator( &aMemoryMgr->mAllocator,
                                             aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief allocator을 삭제한다.
 * @param[in] aMemoryMgr cmlMemoryMgr
 * @param[in] aErrorStack   Error Stack Pointer
 */
stlStatus cmlFinalizeMemoryManager( cmlMemoryManager  * aMemoryMgr,
                                    stlErrorStack     * aErrorStack )
{
    STL_TRY( stlDestroyRegionAllocator( &aMemoryMgr->mAllocator,
                                        aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Communication Handle을 초기화 한다.
 * @param[in] aHandle        Communication Handle
 * @param[in] aContext       socket Context
 * @param[in] aEndian        Endian(STL_PLATFORM_ENDIAN, STL_BIG_ENDIAN or STL_LITTLE_ENDIAN)
 * @param[in] aErrorStack    Error Stack Pointer
 * @remark : aContext, aIpc는 둘중에 하나만 설정되어야 함.
 */
stlStatus cmlInitializeHandle( cmlHandle        * aHandle,
                               stlContext       * aContext,
                               stlInt32           aEndian,
                               stlErrorStack    * aErrorStack )
{
    aHandle->mCommunication.mContext = aContext;
    aHandle->mSendSequence = 0;
    aHandle->mRecvSequence = 0;

    aHandle->mEndian = aEndian;

    return STL_SUCCESS;
}

/**
 * @brief Communication Handle을 삭제한다.
 * @param[in] aHandle       Communication Handle
 * @param[in] aErrorStack   Error Stack Pointer
 */
stlStatus cmlFinalizeHandle( cmlHandle     * aHandle,
                             stlErrorStack * aErrorStack )
{
    aHandle->mProtocolSentence = NULL;

    return STL_SUCCESS;
}


/**
 * @brief cmlProtocolSentence을 초기화 한다.
 * @param[in] aHandle               Communication Handle
 * @param[in] aProtocolSentence     cmlProtocolSentence
 * @param[in] aMemoryMgr            memory manager
 * @param[in] aErrorStack           Error Stack Pointer
 * @remark : aProtocolSentence, aMemoryMgr, aBufferSize를 그대로 사용할경우 NULL을 입력한다.
 */
stlStatus cmlInitializeProtocolSentence( cmlHandle               * aHandle,
                                         cmlProtocolSentence     * aProtocolSentence,
                                         cmlMemoryManager        * aMemoryMgr,
                                         stlErrorStack           * aErrorStack )
{
    cmlProtocolSentence     * sProtocolSentence = NULL;

    aHandle->mProtocolSentence = aProtocolSentence;
    aProtocolSentence->mMemoryMgr = aMemoryMgr;

    sProtocolSentence = aHandle->mProtocolSentence;


    /**
     * 한 패킷 안에서만 유효한 cached statement id를 무효화 한다.
     * => 여기서 mCachedStatementId = CML_STATEMENT_ID_UNDEFINED 로 설정하는게 좋을것 같음.
     */
    sProtocolSentence->mCachedStatementId = CML_STATEMENT_ID_UNDEFINED;

    /**
     * TCP의 경우는 MemoryMgr에서 할당해 놓은 mWriteBuffer, mReadBuffer를 재사용하여
     *  mWriteStartPos, mReadStartPos에 설정하여 사용한다.
     */
    sProtocolSentence->mWriteStartPos = aMemoryMgr->mWriteBuffer;
    sProtocolSentence->mReadStartPos = aMemoryMgr->mReadBuffer;

    sProtocolSentence->mReadProtocolPos = sProtocolSentence->mReadStartPos + CMG_PACKET_HEADER_SIZE;
    sProtocolSentence->mReadEndPos = sProtocolSentence->mReadStartPos + CMG_PACKET_HEADER_SIZE;

    sProtocolSentence->mWriteProtocolPos = sProtocolSentence->mWriteStartPos + CMG_PACKET_HEADER_SIZE;
    sProtocolSentence->mWriteEndPos = sProtocolSentence->mWriteStartPos + aMemoryMgr->mPacketBufferSize;

    return STL_SUCCESS;
}


/**
 * @brief ProtocolSentence을 삭제한다.
 * @param[in] aProtocolSentence     cmlProtocolSentence
 * @param[in] aErrorStack           Error Stack Pointer
 */
stlStatus cmlFinalizeProtocolSentence( cmlProtocolSentence      * aProtocolSentence,
                                       stlErrorStack            * aErrorStack )
{

    return STL_SUCCESS;
}

/**
 * @brief 버퍼의 내용을 원격으로 전송한다.
 *        (shared 모드인 경우 static area를 이용하여 dispatcher로 전송한다.)
 * @param[in] aHandle       Communication Handle
 * @param[in] aPeriod       aPeriod
 * @param[in] aErrorStack   Error Stack Pointer
 */
stlStatus cmlSendPacketInternal( cmlHandle     * aHandle,
                                 stlUInt8        aPeriod,
                                 stlErrorStack * aErrorStack )
{
    stlInt32                 sTotalSize;
    stlInt32                 sPayloadSize;
    cmlProtocolSentence    * sProtocolSentence = aHandle->mProtocolSentence;
    cmlMemoryManager       * sMemoryMgr = sProtocolSentence->mMemoryMgr;

    sTotalSize = sProtocolSentence->mWriteProtocolPos - sProtocolSentence->mWriteStartPos;

    STL_DASSERT( sTotalSize <= sMemoryMgr->mPacketBufferSize );

    sPayloadSize = sTotalSize - CMG_PACKET_HEADER_SIZE;

    if( sPayloadSize > 0 )
    {
        CMG_WRITE_HEADER( aHandle,
                          &(sPayloadSize),
                          &aPeriod );

        STL_TRY( cmgSend( STL_SOCKET_IN_CONTEXT(*(aHandle->mCommunication.mContext)),
                          sProtocolSentence->mWriteStartPos,
                          sTotalSize,
                          STL_TRUE,     /* send mtu size */
                          aErrorStack )
                 == STL_SUCCESS );

        sProtocolSentence->mWriteProtocolPos = sProtocolSentence->mWriteStartPos + CMG_PACKET_HEADER_SIZE;
        sProtocolSentence->mWriteEndPos = sProtocolSentence->mWriteStartPos + sMemoryMgr->mPacketBufferSize;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Version을 전송한다.
 * @param[in] aHandle       Communication Handle
 * @param[in] aErrorStack   Error Stack Pointer
 */
stlStatus cmlSendVersion( cmlHandle     * aHandle,
                          stlErrorStack * aErrorStack )
{
    stlInt32                 sTotalSize;
    cmlProtocolSentence    * sProtocolSentence = aHandle->mProtocolSentence;
    cmlMemoryManager       * sMemoryMgr = sProtocolSentence->mMemoryMgr;

    sTotalSize = sProtocolSentence->mWriteProtocolPos - sProtocolSentence->mWriteStartPos - CMG_PACKET_HEADER_SIZE;

    STL_DASSERT( sTotalSize == CML_VERSION_SIZE );

    /**
     * versin은 header없이 바로 payload를 보냄으로
     * mWriteStartPos가 아닌 mWriteProtocolPos를 보낸다.
     */
    STL_TRY( cmgSend( STL_SOCKET_IN_CONTEXT(*(aHandle->mCommunication.mContext)),
                      sProtocolSentence->mWriteStartPos + CMG_PACKET_HEADER_SIZE,
                      sTotalSize,
                      STL_FALSE,        /* send mtu size */
                      aErrorStack )
             == STL_SUCCESS );

    sProtocolSentence->mWriteProtocolPos = sProtocolSentence->mWriteStartPos + CMG_PACKET_HEADER_SIZE;
    sProtocolSentence->mWriteEndPos = sProtocolSentence->mWriteStartPos + sMemoryMgr->mPacketBufferSize;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Network 버퍼의 내용을 전송하기 전에 풀링한다.
 * @param[in] aHandle       Communication Handle
 * @param[in] aErrorStack   Error Stack Pointer
 */
stlStatus cmlFlushPacket( cmlHandle     * aHandle,
                          stlErrorStack * aErrorStack )
{
    STL_TRY( cmlSendPacketInternal( aHandle,
                                    CML_PERIOD_CONTINUE,
                                    aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Network 버퍼의 내용을 원격으로 전송한다.
 * @param[in] aHandle       Communication Handle
 * @param[in] aErrorStack   Error Stack Pointer
 */
stlStatus cmlSendPacket( cmlHandle     * aHandle,
                         stlErrorStack * aErrorStack )
{
    /* client용 mCachedStatementId 초기화 필요함 */
    aHandle->mProtocolSentence->mCachedStatementId = CML_STATEMENT_ID_UNDEFINED;

    STL_TRY( cmlSendPacketInternal( aHandle,
                                    CML_PERIOD_END,
                                    aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 원격으로 부터 Packet을 전송받는다.
 * @param[in] aHandle       Communication Handle
 * @param[in] aErrorStack   Error Stack Pointer
 */
stlStatus cmlRecvPacket( cmlHandle     * aHandle,
                         stlErrorStack * aErrorStack )
{
    cmlProtocolSentence * sProtocolSentence = aHandle->mProtocolSentence;

    STL_TRY( cmgRecv( aHandle,
                      aErrorStack )
             == STL_SUCCESS );

    sProtocolSentence->mReadProtocolPos = sProtocolSentence->mReadStartPos + CMG_PACKET_HEADER_SIZE;
    sProtocolSentence->mReadEndPos = sProtocolSentence->mReadProtocolPos + sProtocolSentence->mReadPayloadSize;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief static area 정보를 shmstatic_info 에 저장한다.
 * @param[in]  aShmInfo         shared memory info
 * @param[in]  aErrorStack      에러 스택
 */
stlStatus cmlSaveStaticAreaInfo( cmlShmStaticFileInfo * aShmInfo,
                                 stlErrorStack        * aErrorStack )
{
    stlInt32      sState = 0;
    stlFile       sFile;
    stlBool       sFound = STL_FALSE;
    stlChar       sFileName[STL_MAX_FILE_PATH_LENGTH + STL_MAX_FILE_NAME_LENGTH] = {0};
    stlChar       sHomeDir[STL_MAX_FILE_PATH_LENGTH] = {0};

    /**
     * GOLDILOCKS_DATA를 구성
     */
    STL_TRY( stlGetEnv( sHomeDir,
                        STL_SIZEOF( sHomeDir ),
                        STL_ENV_DB_DATA,
                        &sFound,
                        aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( sFound == STL_TRUE, RAMP_ERR_INVALID_HOMEDIR );

    /**
     * GOLDILOCKS_DATA 끝에 '/'를 제거한다.
     */
    if ( sHomeDir[ stlStrlen( sHomeDir ) - 1 ] == '/' ||
         sHomeDir[ stlStrlen( sHomeDir ) - 1 ] == '\\' )
    {
        sHomeDir[ stlStrlen( sHomeDir ) ] = '\0';
    }

    stlSnprintf( sFileName, STL_SIZEOF( sFileName ),
                 "%s"STL_PATH_SEPARATOR"%s",
                 sHomeDir,
                 CML_STATIC_ADDR_FILE_NAME );

    STL_TRY( stlOpenFile( &sFile,
                          sFileName,
                          STL_FOPEN_WRITE | STL_FOPEN_CREATE | STL_FOPEN_TRUNCATE,
                          STL_FPERM_OS_DEFAULT,
                          aErrorStack ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( stlWriteFile( &sFile,
                           (void*)aShmInfo,
                           STL_SIZEOF( cmlShmStaticFileInfo ),
                           NULL,
                           aErrorStack ) == STL_SUCCESS );

    STL_TRY( stlSyncFile( &sFile, aErrorStack ) == STL_SUCCESS );
    STL_TRY( stlCloseFile( &sFile, aErrorStack ) == STL_SUCCESS );


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HOMEDIR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      CML_ERRCODE_PROPERTY_INVALID_HOME_DIR,
                      NULL,
                      aErrorStack );

    }

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)stlCloseFile( &sFile, aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}


/**
 * @brief shmstatic_info config를 읽어서 static area에 attach한다.
 * @param[in]  aShmInfo         shared memory info
 * @param[in]  aErrorStack      에러 스택
 */
stlStatus cmlLoadStaticAreaInfo( cmlShmStaticFileInfo * aShmInfo,
                                 stlErrorStack        * aErrorStack )
{
    stlInt32      sState = 0;
    stlFile       sFile;
    stlBool       sFileExist;
    stlBool       sFound = STL_FALSE;
    stlChar       sFileName[STL_MAX_FILE_PATH_LENGTH + STL_MAX_FILE_NAME_LENGTH] = {0};
    stlChar       sHomeDir[STL_MAX_FILE_PATH_LENGTH] = {0};

    /**
     * GOLDILOCKS_DATA를 구성
     */
    STL_TRY( stlGetEnv( sHomeDir,
                        STL_SIZEOF( sHomeDir ),
                        STL_ENV_DB_DATA,
                        &sFound,
                        aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( sFound == STL_TRUE, RAMP_ERR_INVALID_HOMEDIR );

    /**
     * GOLDILOCKS_DATA 끝에 '/'를 제거한다.
     */
    if ( sHomeDir[ stlStrlen( sHomeDir ) - 1 ] == '/' ||
         sHomeDir[ stlStrlen( sHomeDir ) - 1 ] == '\\' )
    {
        sHomeDir[ stlStrlen( sHomeDir ) ] = '\0';
    }

    stlSnprintf( sFileName, STL_SIZEOF( sFileName ),
                 "%s"STL_PATH_SEPARATOR"%s",
                 sHomeDir,
                 CML_STATIC_ADDR_FILE_NAME );

    STL_TRY( stlExistFile( sFileName,
                           &sFileExist,
                           aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( sFileExist == STL_TRUE, RAMP_ERR_FILE_NOT_EXIST );

    STL_TRY( stlOpenFile( &sFile,
                          sFileName,
                          STL_FOPEN_READ,
                          STL_FPERM_OS_DEFAULT,
                          aErrorStack ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( stlReadFile( &sFile,
                          aShmInfo,
                          STL_SIZEOF( cmlShmStaticFileInfo ),
                          NULL,
                          aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( stlCloseFile( &sFile, aErrorStack ) == STL_SUCCESS );


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HOMEDIR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      CML_ERRCODE_PROPERTY_INVALID_HOME_DIR,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH(RAMP_ERR_FILE_NOT_EXIST)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_FILE_OPEN,
                      NULL,
                      aErrorStack,
                      sFileName );
    }

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)stlCloseFile( &sFile, aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief 주어진 Socket으로부터 async로 message를 받는다.
 * @param[in] aContext            socket context
 * @param[in] aReadBuffer         recv buffer
 * @param[in] aBufferSize         recv buffer size
 * @param[out] aReadSize          read size
 * @param[out] aPayloadSize       payload size
 * @param[out] aPeriod            period
 * @param[out] aReceived          data를 다 받았는지 여부
 * @param[in] aErrorStack         에러 스택
 * @remark data을 다 받거나 EAGAIN이 나오면 STL_SUCCESS를 리턴한다.
 *         socket에러 발생한 경우만 STL_FAILURE 리턴한다.
 */
stlStatus cmlRecvRequestAsync( stlContext    * aContext,
                               stlChar       * aReadBuffer,
                               stlInt64        aBufferSize,
                               stlInt32      * aReadSize,
                               stlInt32      * aPayloadSize,
                               stlUInt8      * aPeriod,
                               stlBool       * aReceived,
                               stlErrorStack * aErrorStack )
{
    return cmgRecvRequestAsync( aContext,
                                aReadBuffer,
                                aBufferSize,
                                aReadSize,
                                aPayloadSize,
                                aPeriod,
                                aReceived,
                                aErrorStack );
}


/**
 * @brief 주어진 Socket으로 message를 보낸다.
 * @param[in] aSocket             socket context
 * @param[in] aBuffer             전송할 buffer
 * @param[out] aSize              전송할 길이
 * @param[out] aMtuSize           최소 전송을 MTU 크기로 보낼지 여부
 * @param[in] aErrorStack         에러 스택
 */
stlStatus cmlSend( stlSocket       aSocket,
                   stlChar       * aBuffer,
                   stlInt32        aSize,
                   stlBool         aMtuSize,
                   stlErrorStack * aErrorStack )
{
    return cmgSend( aSocket,
                    aBuffer,
                    aSize,
                    aMtuSize,
                    aErrorStack );
}

/** @} */
