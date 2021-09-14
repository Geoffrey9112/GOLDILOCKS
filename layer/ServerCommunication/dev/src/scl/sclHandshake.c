/*******************************************************************************
 * sclHandshake.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: sclHandshake.c 6676 2012-12-13 08:39:46Z egon $
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
#include <sclProtocol.h>
#include <sclGeneral.h>
#include <sccCommunication.h>

/**
 * @file sclHandshake.c
 * @brief Communication Layer Handshake Protocol Routines
 */

/**
 * @addtogroup sclProtocol
 * @{
 */


/**
 * @brief Server를 위한 Protocol Handshaking
 * @param[in] aHandle          Communication Handle
 * @param[in] aRoleName        role name
 * @param[in] aEnv             env
 */
stlStatus sclHandshakingForServer( sclHandle     * aHandle,
                                   stlChar       * aRoleName,
                                   sclEnv        * aEnv )
{
    sclProtocolSentence   * sProtocolSentence = aHandle->mProtocolSentence;

    aHandle->mEndian = STL_PLATFORM_ENDIAN;

    STL_TRY( sclReadHandshakeCommand( aHandle,
                                      aRoleName,
                                      aEnv )
             == STL_SUCCESS );

    if( aHandle->mCommunication.mType == SCL_COMMUNICATION_IPC )
    {
        /*
         * aHandle->mImSession->mReadPacketStart을 읽어서
         * handshake를 처리했음으로 mReadPacketStart는 free하고 next로 옮긴다.
         */
        STL_TRY( sccCompleteRecvBuffer( &aHandle->mCommunication.mIpc->mRequestIpc,
                                        aHandle->mCommunication.mIpc->mArrayAllocator,
                                        sProtocolSentence->mReadStartPos,
                                        aEnv )
                 == STL_SUCCESS );
    }

    /* client의 버전이 높으면 error 처리 */
    STL_TRY_THROW( aHandle->mMajorVersion == CML_PROTOCOL_MAJOR_VERSION,  RAMP_ERR_INVALID_PROTOCOL_VERSION );
    STL_TRY_THROW( aHandle->mMinorVersion <= CML_PROTOCOL_MINOR_VERSION,  RAMP_ERR_INVALID_PROTOCOL_VERSION );
    STL_TRY_THROW( aHandle->mPatchVersion <= CML_PROTOCOL_PATCH_VERSION,  RAMP_ERR_INVALID_PROTOCOL_VERSION );

    STL_TRY( sclWriteHandshakeResult( aHandle,
                                      sProtocolSentence->mMemoryMgr->mPacketBufferSize,
                                      KNL_ERROR_STACK(aEnv),
                                      aEnv )
             == STL_SUCCESS );

    if( aHandle->mCommunication.mType == SCL_COMMUNICATION_TCP )
    {
        aHandle->mCommunication.mContext->mPollFd.mReqEvents = STL_POLLIN | STL_POLLERR;
    }

    STL_DASSERT( aHandle->mProtocolSentence->mMemoryMgr != NULL );

    STL_TRY( sccSendPacket( aHandle,
                            aEnv )
             == STL_SUCCESS );

    aHandle = NULL;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_PROTOCOL_VERSION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SCL_ERRCODE_INVALID_COMMUNICATION_PROTOCOL,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );

        STL_TRY( sclWriteErrorResult( aHandle,
                                      CML_COMMAND_HANDSHAKE,
                                      KNL_ERROR_STACK(aEnv),
                                      aEnv )
                 == STL_SUCCESS );

        STL_TRY( sccSendPacket( aHandle,
                                aEnv )
                 == STL_SUCCESS );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Handshake Command Protocol을 읽는다.
 * @param[in]  aHandle       Communication Handle
 * @param[in]  aRoleName     role name
 * @param[in]  aEnv          env
 * 
 * @par protocol sequence:
 * @code
 * 
 * - command (t)ype     : command type        var_int bytes
 * - (p)eriod           : protocol control    1 byte
 * - (r)ole name        : role name           variable
 *
 * field sequence : t-p-r
 * 
 * @endcode
 */
stlStatus sclReadHandshakeCommand( sclHandle      * aHandle,
                                   stlChar        * aRoleName,
                                   sclEnv         * aEnv )
{
    stlInt16    sCommandType;
    stlInt8     sControl;
    scpCursor   sCursor;
    stlInt32    sLength;

    STL_TRY( scpBeginReading( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS );

    SCP_READ_VAR_INT16( aHandle, &sCursor, &sCommandType, aEnv );
    SCP_READ_INT8( aHandle, &sCursor, &sControl, aEnv );
    SCP_READ_N_VAR( aHandle, &sCursor, aRoleName, sLength, aEnv );

    STL_DASSERT( sCommandType == CML_COMMAND_HANDSHAKE );

    STL_TRY( scpEndReading( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Handshake Result Protocol을 기록한다.
 * @param[in] aHandle      Communication Handle
 * @param[in] aBufferSize  Buffer Size
 * @param[in] aErrorStack  기록할 Error Stack
 * @param[in] aEnv         env
 * 
 * @par protocol sequence:
 * @code
 * 
 * - command (t)ype : command type        var_int bytes
 * - e(r)ror level  : error level         1 byte
 * - (b)uffer size  : buffer size         var_int bytes
 * - error (c)ount  : error count         1 bytes
 * - (s)ql state    : sql state           4 bytes
 * - n(a)tive error : native error        4 bytes
 * - (m)essage text : message text        variable
 *
 * field sequence : if r == 0 then t-r-b
 *                  else if r == 1 then t-r-c-[s-a-m]^c-b
 *                  else t-r-c-[s-a-m]^c
 * @endcode
 */
stlStatus sclWriteHandshakeResult( sclHandle      * aHandle,
                                   stlInt32         aBufferSize,
                                   stlErrorStack  * aErrorStack,
                                   sclEnv         * aEnv )
{
    stlInt16    sResultType = CML_COMMAND_HANDSHAKE;
    scpCursor   sCursor;

    STL_TRY( scpBeginWriting( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS );

    SCP_WRITE_VAR_INT16( aHandle, &sCursor, &sResultType, aEnv );

    STL_TRY( scpWriteErrorResult( aHandle,
                                  &sCursor,
                                  aErrorStack,
                                  aEnv ) == STL_SUCCESS );

    SCP_WRITE_VAR_INT32( aHandle, &sCursor, &aBufferSize, aEnv );

    STL_TRY( scpEndWriting( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} */
