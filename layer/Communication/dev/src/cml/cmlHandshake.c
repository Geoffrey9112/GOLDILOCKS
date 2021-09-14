/*******************************************************************************
 * cmlHandshake.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: cmlHandshake.c 6676 2012-12-13 08:39:46Z egon $
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
#include <cmpCursor.h>
#include <cmpProtocol.h>
#include <cmgSocket.h>
#include <cmgMemory.h>

/**
 * @file cmlHandshake.c
 * @brief Communication Layer Handshake Protocol Routines
 */

/**
 * @addtogroup cmlProtocol
 * @{
 */


/**
 * @brief version을 기록한다.
 * @param[in] aHandle       Communication Handle
 * @param[in] aErrorStack   Error Stack Pointer
 */
stlStatus cmlWriteVersion( cmlHandle      * aHandle,
                           stlErrorStack  * aErrorStack )
{
    cmpCursor   sCursor;
    stlInt16    sMajorVersion = CML_PROTOCOL_MAJOR_VERSION;
    stlInt16    sMinorVersion = CML_PROTOCOL_MINOR_VERSION;
    stlInt16    sPatchVersion = CML_PROTOCOL_PATCH_VERSION;

    aHandle->mEndian = STL_LITTLE_ENDIAN;

    STL_TRY( cmpBeginWriting( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_WRITE_INT16_ENDIAN( aHandle, &sCursor, &sMajorVersion, aErrorStack );
    CMP_WRITE_INT16_ENDIAN( aHandle, &sCursor, &sMinorVersion, aErrorStack );
    CMP_WRITE_INT16_ENDIAN( aHandle, &sCursor, &sPatchVersion, aErrorStack );

    STL_TRY( cmpEndWriting( aHandle,
                            &sCursor,
                            aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Handshake Command Protocol을 기록한다.
 * @param[in] aHandle       Communication Handle
 * @param[in] aRoleName     RoleName
 * @param[in] aErrorStack   Error Stack Pointer
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
stlStatus cmlWriteHandshakeCommand( cmlHandle      * aHandle,
                                    stlChar        * aRoleName,
                                    stlErrorStack  * aErrorStack )
{
    stlInt16    sCommandType = CML_COMMAND_HANDSHAKE;
    stlInt8     sControl = CML_CONTROL_HEAD | CML_CONTROL_TAIL;
    cmpCursor   sCursor;
    stlInt32    sLength;

    STL_TRY( cmpBeginWriting( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_WRITE_VAR_INT16( aHandle, &sCursor, &sCommandType, aErrorStack );
    CMP_WRITE_INT8( aHandle, &sCursor, &sControl, aErrorStack );
    sLength = stlStrlen( aRoleName );
    CMP_WRITE_N_VAR_ENDIAN( aHandle, &sCursor, aRoleName, sLength, aErrorStack );

    STL_TRY( cmpEndWriting( aHandle,
                            &sCursor,
                            aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Handshake Result Protocol을 읽는다.
 * @param[in]  aHandle       Communication Handle
 * @param[out] aBufferSize   Buffer Size
 * @param[in]  aErrorStack   Error Stack Pointer
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
 * 
 * @endcode
 */
stlStatus cmlReadHandshakeResult( cmlHandle      * aHandle,
                                  stlInt32       * aBufferSize,
                                  stlErrorStack  * aErrorStack )
{
    stlInt16    sResultType;
    cmpCursor   sCursor;
    
    STL_TRY( cmpBeginReading( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_READ_VAR_INT16( aHandle, &sCursor, &sResultType, aErrorStack );

    STL_TRY( cmpReadErrorResult( aHandle,
                                 NULL, /* aIgnored */
                                 &sCursor,
                                 aErrorStack )
             == STL_SUCCESS );
    
    CMP_READ_VAR_INT32( aHandle, &sCursor, aBufferSize, aErrorStack );

    STL_DASSERT( sResultType == CML_COMMAND_HANDSHAKE );

    STL_TRY( cmpEndReading( aHandle,
                            &sCursor,
                            aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief PreHandshake Command Protocol을 기록한다.
 * @param[in] aHandle       Communication Handle
 * @param[in] aServerMode   server mode
 * @param[in] aErrorStack   Error Stack Pointer
 */
stlStatus cmlWritePreHandshakeCommand( cmlHandle      * aHandle,
                                       cmlSessionType   aServerMode,
                                       stlErrorStack  * aErrorStack )
{
    stlInt16    sCommandType = CML_COMMAND_PRE_HANDSHAKE;
    stlInt8     sControl = CML_CONTROL_HEAD | CML_CONTROL_TAIL;
    cmpCursor   sCursor;
    stlInt32    sServerMode = aServerMode;

    STL_TRY( cmpBeginWriting( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_WRITE_VAR_INT16( aHandle, &sCursor, &sCommandType, aErrorStack );

    CMP_WRITE_INT8( aHandle, &sCursor, &sControl, aErrorStack );
    CMP_WRITE_INT32_ENDIAN( aHandle, &sCursor, &sServerMode, aErrorStack );

    STL_TRY( cmpEndWriting( aHandle,
                            &sCursor,
                            aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief listener에서 gsqlnet로부터 받은 PreHandshake를 읽는다.
 * @param[in] aHandle           aHandle
 * @param[out] aServerMode      serverMode(dedicate or multiplex)
 * @param[in] aErrorStack       Error Stack Pointer
 * @remark 이 함수는 listener가 호출함으로 cmlHandle사용 못함.
 */
stlStatus cmlReadPreHandshake( cmlHandle      * aHandle,
                               cmlSessionType * aServerMode,
                               stlErrorStack  * aErrorStack )
{
    stlInt16                sCommandType = 0;
    stlInt32                sPayloadSize = 0;
    stlUInt8                sPeriod = 0;
    stlInt8                 sReserved = 0;
    stlInt8                 sControl;
    stlInt8                 sServerMode;
    cmpCursor               sCursor;

    /* preHandshake는 little endian 임 */
    aHandle->mEndian = STL_LITTLE_ENDIAN;

    STL_TRY( cmpBeginReading( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    /* header - payload size */
    CMP_READ_INT32_ENDIAN( aHandle, &sCursor, &sPayloadSize, aErrorStack );

    /* listener와 preHandshake에서 payload size는 6임 */
    STL_TRY_THROW( sPayloadSize == 6,
                   RAMP_ERR_INVALID_PROTOCOL );

    /* header - period */
    CMP_READ_PERIOD( aHandle, &sCursor, &sPeriod, aErrorStack );
    STL_TRY_THROW( sPeriod == CML_PERIOD_END,
                   RAMP_ERR_INVALID_PROTOCOL );

    /* reserved 3byte */
    CMP_READ_INT8( aHandle, &sCursor, &sReserved, aErrorStack );
    CMP_READ_INT8( aHandle, &sCursor, &sReserved, aErrorStack );
    CMP_READ_INT8( aHandle, &sCursor, &sReserved, aErrorStack );

    CMP_READ_VAR_INT16( aHandle, &sCursor, &sCommandType, aErrorStack );

    STL_TRY_THROW( sCommandType == CML_COMMAND_PRE_HANDSHAKE,
                   RAMP_ERR_INVALID_PROTOCOL );

    CMP_READ_INT8( aHandle, &sCursor, &sControl, aErrorStack );

    CMP_READ_INT8( aHandle, &sCursor, &sServerMode, aErrorStack );

    *aServerMode = sServerMode;

    STL_TRY( cmpEndReading( aHandle,
                            &sCursor,
                            aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_PROTOCOL )
    {
        stlPushError( STL_ERROR_LEVEL_WARNING,
                      CML_ERRCODE_INVALID_COMMUNICATION_PROTOCOL,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief listener가 client에게 PreHandshake result를 보낸다.
 * @param[in] aHandle       aHandle
 * @param[in] aErrorStack   Error Stack Pointer
 * @remark 이 함수는 listener가 사용하지만 cmlHandle을 사용하도록 하였음.
 *         cmlHandle을 사용 안하면 cmpWriteErrorResult 처리가 복잡하여
 *         listener에서 Handshake result를 위한 cmlHandle하나만 생성하여 사용함.
 */
stlStatus cmlWritePreHandshakeResult( cmlHandle     * aHandle,
                                      stlErrorStack * aErrorStack )
{
    cmpCursor       sCursor;
    stlInt16        sResultType = CML_COMMAND_PRE_HANDSHAKE;
    stlInt8         sServerEndian;

    sServerEndian = STL_PLATFORM_ENDIAN;

    /**
     * preHandshake는 little endian으로 보내야만 한다.
     */
    STL_DASSERT( aHandle->mEndian == STL_LITTLE_ENDIAN );

    STL_TRY( cmpBeginWriting( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_WRITE_VAR_INT16( aHandle, &sCursor, &sResultType, aErrorStack );

    STL_TRY( cmpWriteErrorResult( aHandle,
                                  &sCursor,
                                  aErrorStack )
             == STL_SUCCESS );

    CMP_WRITE_INT8( aHandle, &sCursor, &sServerEndian, aErrorStack );

    STL_TRY( cmpEndWriting( aHandle,
                            &sCursor,
                            aErrorStack )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief PreHandshake Result Protocol을 읽는다.
 * @param[in]  aHandle       Communication Handle
 * @param[in]  aErrorStack   Error Stack Pointer
 */
stlStatus cmlReadPreHandshakeResult( cmlHandle      * aHandle,
                                     stlErrorStack  * aErrorStack )
{
    stlInt16    sResultType;
    cmpCursor   sCursor;
    stlInt8     sEndian;

    STL_TRY( cmpBeginReading( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_READ_VAR_INT16( aHandle, &sCursor, &sResultType, aErrorStack );

    STL_TRY( cmpReadErrorResult( aHandle,
                                 NULL, /* aIgnored */
                                 &sCursor,
                                 aErrorStack )
             == STL_SUCCESS );

    STL_TRY( aErrorStack->mTop == -1 );

    CMP_READ_INT8( aHandle, &sCursor, &sEndian, aErrorStack );

    STL_DASSERT( sResultType == CML_COMMAND_PRE_HANDSHAKE );

    STL_TRY( cmpEndReading( aHandle,
                            &sCursor,
                            aErrorStack )
             == STL_SUCCESS );

    aHandle->mEndian = sEndian;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief version을 읽는다.
 * @param[in] aHandle       Communication Handle
 * @param[in] aMajorVersion Major Version
 * @param[in] aMinorVersion Minor Version
 * @param[in] aPatchVersion Patch Version
 * @param[in] aErrorStack   Error Stack Pointer
 */
stlStatus cmlReadVersion( cmlHandle      * aHandle,
                          stlInt16       * aMajorVersion,
                          stlInt16       * aMinorVersion,
                          stlInt16       * aPatchVersion,
                          stlErrorStack  * aErrorStack )
{
    cmpCursor   sCursor;

    aHandle->mEndian = STL_LITTLE_ENDIAN;

    STL_TRY( cmpBeginReading( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_READ_INT16_ENDIAN( aHandle, &sCursor, aMajorVersion, aErrorStack );
    CMP_READ_INT16_ENDIAN( aHandle, &sCursor, aMinorVersion, aErrorStack );
    CMP_READ_INT16_ENDIAN( aHandle, &sCursor, aPatchVersion, aErrorStack );

    STL_TRY( cmpEndReading( aHandle,
                            &sCursor,
                            aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
