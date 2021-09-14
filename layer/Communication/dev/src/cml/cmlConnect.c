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
#include <cmlDef.h>
#include <cmDef.h>
#include <cmlProtocol.h>
#include <cmlGeneral.h>
#include <cmpCursor.h>
#include <cmpProtocol.h>

/**
 * @file cmlConnect.c
 * @brief Communication Layer Connect Protocol Routines
 */

/**
 * @addtogroup cmlProtocol
 * @{
 */

/**
 * @brief Connect Command Protocol을 기록한다.
 * @param[in] aHandle            Communication Handle
 * @param[in] aControl           Control
 * @param[in] aUserName          User Name
 * @param[in] aEncryptedPassword Encrypted Password
 * @param[in] aOldPassword       Old Password
 * @param[in] aNewPassword       New Password
 * @param[in] aProgramName       Program Name
 * @param[in] aProcessId         Process Identifier
 * @param[in] aCharacterset      Clinet Characterset
 * @param[in] aTimezone          Clinet Timezone
 * @param[in] aDbcType           DBC Type
 * @param[in] aErrorStack        Error Stack Pointer
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
 * - pass(w)ord     : password            variable
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
stlStatus cmlWriteConnectCommand( cmlHandle       * aHandle,
                                  stlInt8           aControl,
                                  stlChar         * aUserName,
                                  stlChar         * aEncryptedPassword,
                                  stlChar         * aOldPassword,
                                  stlChar         * aNewPassword,
                                  stlChar         * aProgramName,
                                  stlInt32          aProcessId,
                                  stlInt16          aCharacterset,
                                  stlInt32          aTimezone,
                                  cmlDbcType        aDbcType,
                                  stlErrorStack   * aErrorStack )
{
    stlInt16    sCommandType = CML_COMMAND_CONNECT;
    cmpCursor   sCursor;
    stlInt8     sInt8;
    stlInt32    sLength;
    stlChar     sEmpty[] = "";

    STL_TRY( cmpBeginWriting( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS);

    CMP_WRITE_VAR_INT16( aHandle, &sCursor, &sCommandType, aErrorStack );
    CMP_WRITE_INT8( aHandle, &sCursor, &aControl, aErrorStack );

    sLength = stlStrlen( aUserName );
    CMP_WRITE_N_VAR_ENDIAN( aHandle, &sCursor, aUserName, sLength, aErrorStack );
    sLength = stlStrlen( aEncryptedPassword );
    CMP_WRITE_N_VAR_ENDIAN( aHandle, &sCursor, aEncryptedPassword, sLength, aErrorStack );
    if( aOldPassword == NULL )
    {
        sLength = 0;
        CMP_WRITE_N_VAR_ENDIAN( aHandle, &sCursor, sEmpty, sLength, aErrorStack );
    }
    else
    {
        sLength = stlStrlen( aOldPassword );
        CMP_WRITE_N_VAR_ENDIAN( aHandle, &sCursor, aOldPassword, sLength, aErrorStack );
    }
    if( aNewPassword == NULL )
    {
        sLength = 0;
        CMP_WRITE_N_VAR_ENDIAN( aHandle, &sCursor, sEmpty, sLength, aErrorStack );
    }
    else
    {
        sLength = stlStrlen( aNewPassword );
        CMP_WRITE_N_VAR_ENDIAN( aHandle, &sCursor, aNewPassword, sLength, aErrorStack );
    }
    sLength = stlStrlen( aProgramName );
    CMP_WRITE_N_VAR_ENDIAN( aHandle, &sCursor, aProgramName, sLength, aErrorStack );
    
    CMP_WRITE_INT32_ENDIAN( aHandle, &sCursor, &aProcessId, aErrorStack );
    CMP_WRITE_INT16_ENDIAN( aHandle, &sCursor, &aCharacterset, aErrorStack );
    CMP_WRITE_INT32_ENDIAN( aHandle, &sCursor, &aTimezone, aErrorStack );

    sInt8 = aDbcType;
    CMP_WRITE_INT8( aHandle, &sCursor, &sInt8, aErrorStack );

    STL_TRY( cmpEndWriting( aHandle,
                            &sCursor,
                            aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Connect Result Protocol을 읽는다.
 * @param[in]  aHandle              Communication Handle
 * @param[out] aSessionId           Session Identifier
 * @param[out] aSessionSeq          Session Sequence
 * @param[out] aCharacterset        Character set
 * @param[out] aNCharCharacterset   NCHAR Character set
 * @param[out] aDBTimezone          DB Timezone
 * @param[in]  aErrorStack          Error Stack Pointer
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
stlStatus cmlReadConnectResult( cmlHandle       * aHandle,
                                stlUInt32       * aSessionId,
                                stlInt64        * aSessionSeq,
                                dtlCharacterSet * aCharacterset,
                                dtlCharacterSet * aNCharCharacterset,
                                stlInt32        * aDBTimezone,
                                stlErrorStack   * aErrorStack )
{
    stlInt16   sResultType;
    cmpCursor  sCursor;
    stlInt16   sCharacterset;
    stlInt16   sNCharCharacterset;

    STL_TRY( cmpBeginReading( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS);

    CMP_READ_VAR_INT16( aHandle, &sCursor, &sResultType, aErrorStack );
    STL_DASSERT( sResultType == CML_COMMAND_CONNECT );

    STL_TRY( cmpReadErrorResult( aHandle,
                                 NULL, /* aIgnored */
                                 &sCursor,
                                 aErrorStack )
             == STL_SUCCESS );
    
    CMP_READ_VAR_INT32( aHandle, &sCursor, aSessionId, aErrorStack );
    CMP_READ_VAR_INT64( aHandle, &sCursor, aSessionSeq, aErrorStack );

    CMP_READ_INT16_ENDIAN( aHandle, &sCursor, &sCharacterset, aErrorStack );
    *aCharacterset = sCharacterset;

    CMP_READ_INT16_ENDIAN( aHandle, &sCursor, &sNCharCharacterset, aErrorStack );
    *aNCharCharacterset = sNCharCharacterset;

    CMP_READ_INT32_ENDIAN( aHandle, &sCursor, aDBTimezone, aErrorStack );

    STL_TRY( cmpEndReading( aHandle,
                            &sCursor,
                            aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
