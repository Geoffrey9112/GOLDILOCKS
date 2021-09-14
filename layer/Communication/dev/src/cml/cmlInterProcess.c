/*******************************************************************************
 * cmlInterProcess.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: cmlInterProcess.c 6676 2012-12-13 08:39:46Z egon $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stl.h>
#include <dtl.h>
#include <cmlDef.h>
#include <cmDef.h>
#include <cmlProtocol.h>
#include <cmlGeneral.h>
#include <cmlInterProcess.h>
#include <cmpCursor.h>
#include <cmgSocket.h>
#include <cmpProtocol.h>

/**
 * @file cmlInterProcess.c
 * @brief Communication Layer Protocol Routines
 */

/**
 * @addtogroup cmlInterProcess
 * @{
 */


/**
 * @brief 주어진 context로 ack를 전송한다.
 * @param[in] aUdsContext       uds context
 * @param[in] aTargetAddr       전송할 address
 * @param[in] aSenderStartTime  송신자의 start time
 * @param[in] aSequence         sequence
 * @param[in] aIdx              index
 * @param[in] aErrorStack       에러 스택
 */
stlStatus cmlSendAckTo( stlContext      * aUdsContext,
                        stlSockAddr     * aTargetAddr,
                        stlTime           aSenderStartTime,
                        stlInt64          aSequence,
                        stlInt64          aIdx,
                        stlErrorStack   * aErrorStack )
{
    stlSize             sSize;
    cmlUdsAck           sAck;

    stlMemset( &sAck, 0x00, STL_SIZEOF(sAck) );

    sAck.mCmdType          = CML_UDS_CMD_ACK;
    sAck.mErrorStack       = *aErrorStack;
    sAck.mSenderStartTime  = aSenderStartTime;
    sAck.mSequence         = aSequence;
    sAck.mIdx              = aIdx;

    STL_TRY( cmlSetInterProcessProtocolVersion( &sAck.mVersion,
                                                aErrorStack )
             == STL_SUCCESS );

    sSize = STL_SIZEOF(cmlUdsAck);

    STL_TRY( stlSendTo( STL_SOCKET_IN_CONTEXT(*aUdsContext),
                        aTargetAddr,
                        0,
                        (stlChar *)&sAck,
                        &sSize,
                        aErrorStack )
             == STL_SUCCESS );

    STL_DASSERT( sSize == STL_SIZEOF(cmlUdsAck) );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 context로 fd 요청을 전송한다.
 * @param[in] aUdsContext       uds context
 * @param[in] aTargetAddr       전송할 address
 * @param[in] aErrorStack       에러 스택
 */
stlStatus cmlSendRequestFdTo( stlContext      * aUdsContext,
                              stlSockAddr     * aTargetAddr,
                              stlErrorStack   * aErrorStack )
{
    stlSize             sSize;
    cmlUdsRequestFd     sRequestFd;

    stlMemset( &sRequestFd, 0x00, STL_SIZEOF(sRequestFd) );
    sRequestFd.mCmdType = CML_UDS_CMD_REQUEST_FD;

    STL_TRY( cmlSetInterProcessProtocolVersion( &sRequestFd.mVersion,
                                                aErrorStack )
             == STL_SUCCESS );

    sSize = STL_SIZEOF(cmlUdsRequestFd);

    STL_TRY( stlSendTo( STL_SOCKET_IN_CONTEXT(*aUdsContext),
                        aTargetAddr,
                        0,
                        (stlChar *)&sRequestFd,
                        &sSize,
                        aErrorStack )
             == STL_SUCCESS );

    STL_DASSERT( sSize == STL_SIZEOF(cmlUdsRequestFd) );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 context로 quit를 요청을 전송한다.
 * @param[in] aUdsContext       uds context
 * @param[in] aTargetAddr       전송할 address
 * @param[in] aErrorStack       에러 스택
 */
stlStatus cmlSendRequestQuitTo( stlContext      * aUdsContext,
                                stlSockAddr     * aTargetAddr,
                                stlErrorStack   * aErrorStack )
{
    stlSize             sSize;
    cmlUdsRequestQuit   sRequestQuit;

    stlMemset( &sRequestQuit, 0x00, STL_SIZEOF(sRequestQuit) );
    sRequestQuit.mCmdType = CML_UDS_CMD_REQUEST_QUIT;

    STL_TRY( cmlSetInterProcessProtocolVersion( &sRequestQuit.mVersion,
                                                aErrorStack )
             == STL_SUCCESS );

    sSize = STL_SIZEOF(cmlUdsRequestQuit);

    STL_TRY( stlSendTo( STL_SOCKET_IN_CONTEXT(*aUdsContext),
                        aTargetAddr,
                        0,
                        (stlChar *)&sRequestQuit,
                        &sSize,
                        aErrorStack )
             == STL_SUCCESS );

    STL_DASSERT( sSize == STL_SIZEOF(cmlUdsRequestQuit) );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 context로 상태 요청을 전송한다.
 * @param[in] aUdsContext       uds context
 * @param[in] aTargetAddr       전송할 address
 * @param[in] aErrorStack       에러 스택
 */
stlStatus cmlSendRequestStatusTo( stlContext      * aUdsContext,
                                  stlSockAddr     * aTargetAddr,
                                  stlErrorStack   * aErrorStack )
{
    stlSize             sSize;
    cmlUdsRequestStatus sRequestStatus;

    stlMemset( &sRequestStatus, 0x00, STL_SIZEOF(sRequestStatus) );
    sRequestStatus.mCmdType = CML_UDS_CMD_REQUEST_STATUS;

    STL_TRY( cmlSetInterProcessProtocolVersion( &sRequestStatus.mVersion,
                                                aErrorStack )
             == STL_SUCCESS );

    sSize = STL_SIZEOF(cmlUdsRequestStatus);

    STL_TRY( stlSendTo( STL_SOCKET_IN_CONTEXT(*aUdsContext),
                        aTargetAddr,
                        0,
                        (stlChar *)&sRequestStatus,
                        &sSize,
                        aErrorStack )
             == STL_SUCCESS );

    STL_DASSERT( sSize == STL_SIZEOF(cmlUdsRequestStatus) );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief 주어진 context로 fd를 전송한다.
 * @param[in] aUdsContext       uds context
 * @param[in] aTargetAddr       전송할 address
 * @param[in] aPassingContext   전달할 fd
 * @param[in] aKey              송신자의 key
 * @param[in] aSenderStartTime  송신자의 start time
 * @param[in] aSequence         fd sequence
 * @param[in] aIdx              fd index
 * @param[in] aMajorVersion     client major version
 * @param[in] aMinorVersion     client minor version
 * @param[in] aPatchVersion     client patch version
 * @param[in] aErrorStack       에러 스택
 */
stlStatus cmlSendFdTo( stlContext      * aUdsContext,
                       stlSockAddr     * aTargetAddr,
                       stlContext      * aPassingContext,
                       stlInt64          aKey,
                       stlTime           aSenderStartTime,
                       stlInt64          aSequence,
                       stlInt64          aIdx,
                       stlUInt16         aMajorVersion,
                       stlUInt16         aMinorVersion,
                       stlUInt16         aPatchVersion,
                       stlErrorStack   * aErrorStack )
{
    cmlUdsForwardFd     sForwardFd;

    stlMemset( &sForwardFd, 0x00, STL_SIZEOF(sForwardFd) );
    sForwardFd.mCmdType         = CML_UDS_CMD_FORWARD_FD;
    sForwardFd.mKey             = aKey;
    sForwardFd.mSenderStartTime = aSenderStartTime;
    sForwardFd.mSequence        = aSequence;
    sForwardFd.mIdx             = aIdx;
    sForwardFd.mMajorVersion    = aMajorVersion;
    sForwardFd.mMinorVersion    = aMinorVersion;
    sForwardFd.mPatchVersion    = aPatchVersion;

    STL_TRY( cmlSetInterProcessProtocolVersion( &sForwardFd.mVersion,
                                                aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlSendContext( aUdsContext,
                             aTargetAddr,
                             aPassingContext,
                             (stlChar *)&sForwardFd,
                             STL_SIZEOF(cmlUdsForwardFd),
                             aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 context로 상태를 전송한다.
 * @param[in] aUdsContext       uds context
 * @param[in] aTargetAddr       전송할 address
 * @param[in] aStatus           상태 문자열
 * @param[in] aErrorStack       에러 스택
 */
stlStatus cmlSendStatusTo( stlContext      * aUdsContext,
                           stlSockAddr     * aTargetAddr,
                           stlChar         * aStatus,
                           stlErrorStack   * aErrorStack )
{
    stlSize                 sSize;
    cmlUdsResponseStatus    sResponseStatus;

    stlMemset( &sResponseStatus, 0x00, STL_SIZEOF(sResponseStatus) );
    sResponseStatus.mCmdType = CML_UDS_CMD_RESPONSE_STATUS;

    STL_TRY( cmlSetInterProcessProtocolVersion( &sResponseStatus.mVersion,
                                                aErrorStack )
             == STL_SUCCESS );

    stlStrncpy( sResponseStatus.mData, aStatus, CML_STATUS_INFO_LENGTH );

    sSize = STL_SIZEOF(cmlUdsResponseStatus);

    STL_TRY(stlSendTo( STL_SOCKET_IN_CONTEXT(*aUdsContext),
                       aTargetAddr,
                       0,
                       (stlChar *)&sResponseStatus,
                       &sSize,
                       aErrorStack )
            == STL_SUCCESS );

    STL_DASSERT( sSize == STL_SIZEOF(cmlUdsResponseStatus) );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 주어진 context로 packet을 받는다.
 * @param[in] aUdsContext       uds context
 * @param[in] aPeerAddr         송신자 address
 * @param[in] aPacket           수신한 packet
 * @param[in] aErrorStack       에러 스택
 */
stlStatus cmlRecvFrom( stlContext      * aUdsContext,
                       stlSockAddr     * aPeerAddr,
                       cmlUdsPacket    * aPacket,
                       stlErrorStack   * aErrorStack )
{
    stlSize             sSize;

    sSize = STL_SIZEOF(cmlUdsPacket);

    STL_TRY( stlRecvFrom( STL_SOCKET_IN_CONTEXT(*aUdsContext),
                          aPeerAddr,
                          0,
                          (stlChar *)aPacket,
                          &sSize,
                          STL_FALSE,
                          aErrorStack )
             == STL_SUCCESS );

    STL_TRY( cmlCheckInterProcessProtocolVersion( &aPacket->mCmd.mCommon.mVersion,
                                                  aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 context로 packet을 받는다.
 * @param[in] aUdsContext       uds context
 * @param[in] aRecvContext      송신자 address
 * @param[in] aForwardFd        수신한 packet
 * @param[in] aErrorStack       에러 스택
 */
stlStatus cmlRecvFdFrom( stlContext      * aUdsContext,
                         stlContext      * aRecvContext,
                         cmlUdsForwardFd * aForwardFd,
                         stlErrorStack   * aErrorStack )
{
    stlSize                 sRecvLen;

    sRecvLen = STL_SIZEOF(cmlUdsForwardFd);

    STL_TRY( stlRecvContext( aUdsContext,
                             aRecvContext,
                             (stlChar *)aForwardFd,
                             &sRecvLen,
                             aErrorStack )
             == STL_SUCCESS );

    STL_DASSERT( sRecvLen == STL_SIZEOF(cmlUdsForwardFd) );

    STL_TRY( cmlCheckInterProcessProtocolVersion( &aForwardFd->mVersion,
                                                  aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief inter-process 통신 버전을 설정한다.
 * @param[in] aVersion          inter-process 통신 버전 structure
 * @param[in] aErrorStack       에러 스택
 */
stlStatus cmlSetInterProcessProtocolVersion( cmlUdsVersion   * aVersion,
                                             stlErrorStack   * aErrorStack)
{
    aVersion->mMajorVersion = CML_INTERPROCESS_PROTOCOL_MAJOR_VERSION;
    aVersion->mMinorVersion = CML_INTERPROCESS_PROTOCOL_MINOR_VERSION;
    aVersion->mPatchVersion = CML_INTERPROCESS_PROTOCOL_PATCH_VERSION;

    return STL_SUCCESS;
}

/**
 * @brief inter-process 통신 버전을 체크한다.
 * @param[in] aVersion          inter-process 통신 버전 structure
 * @param[in] aErrorStack       에러 스택
 */
stlStatus cmlCheckInterProcessProtocolVersion( cmlUdsVersion   * aVersion,
                                               stlErrorStack   * aErrorStack)
{

    STL_TRY_THROW( aVersion->mMajorVersion == CML_INTERPROCESS_PROTOCOL_MAJOR_VERSION,
                   RAMP_INVALID_PROTOCOL );
    STL_TRY_THROW( aVersion->mMinorVersion == CML_INTERPROCESS_PROTOCOL_MINOR_VERSION,
                   RAMP_INVALID_PROTOCOL );
    STL_TRY_THROW( aVersion->mPatchVersion == CML_INTERPROCESS_PROTOCOL_PATCH_VERSION,
                   RAMP_INVALID_PROTOCOL );

    return STL_SUCCESS;

    STL_CATCH( RAMP_INVALID_PROTOCOL )
    {
        stlPushError( STL_ERROR_LEVEL_WARNING,
                      CML_ERRCODE_INVALID_INTERPROCESS_PROTOCOL,
                      NULL,
                      aErrorStack  );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
