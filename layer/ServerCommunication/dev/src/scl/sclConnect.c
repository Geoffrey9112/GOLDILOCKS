/*******************************************************************************
 * cmlConnect.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: cmlConnect.c 6676 2012-12-13 08:39:46Z egon $
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
#include <sccCommunication.h>

/**
 * @file sclConnect.c
 * @brief Communication Layer Connect Protocol Routines
 */

/**
 * @addtogroup cmlProtocol
 * @{
 */

/**
 * @brief Connect Command Protocol을 읽는다.
 * @param[in]  aHandle            Communication Handle
 * @param[out] aControl           Control
 * @param[out] aUserName          User Name
 * @param[out] aEncryptedPassword Encrypted Password
 * @param[out] aOldPassword       Old Password
 * @param[out] aNewPassword       New Password
 * @param[out] aProgramName       Program Name
 * @param[out] aCharacterset      Client Characterset
 * @param[out] aTimezone          Client Timezone
 * @param[out] aProcessId         Process Identifier
 * @param[out] aDbcType           DBC Type
 * @param[in]  aEnv               env
 *
 * @par protocol sequence:
 * @code
 *
 * - command (t)ype : command type        var_int bytes
 * - (p)eriod       : protocol period     1 byte
 * - (u)ser name    : user name           variable
 * - pass(w)ord     : password            variable
 * - (o)ld password : old password        variable
 * - new p(a)ssword : new password        variable
 * - program (n)ame : program name        variable
 * - proce(s)s id   : process identifier  4 bytes
 * - (c)haracterset : client characterset 2 bytes
 * - time(z)one     : client timezone     4 bytes
 * - s(e)ssion type : session type        1 byte
 * - (d)bc type     : dbc type            1 byte
 *
 * field sequence : t-p-u-w-n-s-c-z-e-d
 *
 * @endcode
 */
stlStatus sclReadConnectCommand( sclHandle       * aHandle,
                                 stlInt8         * aControl,
                                 stlChar         * aUserName,
                                 stlChar         * aEncryptedPassword,
                                 stlChar         * aOldPassword,
                                 stlChar         * aNewPassword,
                                 stlChar         * aProgramName,
                                 stlInt32        * aProcessId,
                                 stlInt16        * aCharacterset,
                                 stlInt32        * aTimezone,
                                 cmlDbcType      * aDbcType,
                                 sclEnv          * aEnv )
{
    scpReadConnectCommandFunc   scpReadConnectCommand[CML_PROTOCOL_MINOR_VERSION] =
        {
            scpReadConnectCommandV1,
            scpReadConnectCommandV2
            /**
             * 여기에 version추가에 따른 별도 함수 설정
             */
        };


    return scpReadConnectCommand[aHandle->mMinorVersion - 1]( aHandle,
                                                              aControl,
                                                              aUserName,
                                                              aEncryptedPassword,
                                                              aOldPassword,
                                                              aNewPassword,
                                                              aProgramName,
                                                              aProcessId,
                                                              aCharacterset,
                                                              aTimezone,
                                                              aDbcType,
                                                              aEnv );
}

/**
 * @brief Connect Result Protocol을 기록한다.
 * @param[in] aHandle              Communication Handle
 * @param[in] aSessionId           Session Identifier
 * @param[in] aSessionSeq          Session Sequence
 * @param[in] aCharacterset        Character set
 * @param[in] aNCharCharacterset   NCHAR Character set
 * @param[in] aDBTimezone          DB Timezone
 * @param[in] aErrorStack          기록할 Error Stack
 * @param[in] aEnv                 env
 * 
 * @par protocol sequence:
 * @code
 * 
 * - result (t)ype          : result type         var_int bytes
 * - e(r)ror level          : error level         1 byte
 * - session (i)d           : session identifier  var_int bytes
 * - session se(q)          : session sequence    var_int bytes
 * - c(h)aracter set        : character set       2 bytes
 * - (n)char character set  : character set       2 bytes
 * - database time(z)one    : database timezone   4 bytes
 * - error (c)ount          : error count         1 bytes
 * - (s)ql state            : sql state           4 bytes
 * - n(a)tive error         : native error        4 bytes
 * - (m)essage text         : message text        variable
 *
 * field sequence : if r == 0 then t-r-i-q-h-n-z
 *                  else if r == 1 then t-r-c-[s-a-m]^c-i-q-h-n-z
 *                  else t-r-c-[s-a-m]^c
 * 
 * @endcode
 */
stlStatus sclWriteConnectResult( sclHandle       * aHandle,
                                 stlUInt32         aSessionId,
                                 stlInt64          aSessionSeq,
                                 dtlCharacterSet   aCharacterset,
                                 dtlCharacterSet   aNCharCharacterset,
                                 stlInt32          aDBTimezone,
                                 stlErrorStack   * aErrorStack,
                                 sclEnv          * aEnv )
{
    stlInt16    sResultType = CML_COMMAND_CONNECT;
    scpCursor   sCursor;
    stlInt16    sCharacterset;
    stlInt16    sNCharCharacterset;

    STL_TRY( scpBeginWriting( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS);

    SCP_WRITE_VAR_INT16( aHandle, &sCursor, &sResultType, aEnv );
    
    STL_TRY( scpWriteErrorResult( aHandle,
                                  &sCursor,
                                  aErrorStack,
                                  aEnv ) == STL_SUCCESS );

    SCP_WRITE_VAR_UINT32( aHandle, &sCursor, &aSessionId, aEnv );
    SCP_WRITE_VAR_INT64( aHandle, &sCursor, &aSessionSeq, aEnv );

    sCharacterset = aCharacterset;
    SCP_WRITE_INT16( aHandle, &sCursor, &sCharacterset, aEnv );
    sNCharCharacterset = aNCharCharacterset;
    SCP_WRITE_INT16( aHandle, &sCursor, &sNCharCharacterset, aEnv );

    SCP_WRITE_INT32( aHandle, &sCursor, &aDBTimezone, aEnv );

    STL_TRY( scpEndWriting( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
