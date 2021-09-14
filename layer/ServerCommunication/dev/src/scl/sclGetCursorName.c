/*******************************************************************************
 * cmlGetCursorName.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *
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
 * @file sclGetCursorName.c
 * @brief Communication Layer Cancel Protocol Routines
 */

/**
 * @addtogroup cmlProtocol
 * @{
 */


/**
 * @brief GetCursorName Command Protocol을 읽는다.
 * @param[in]  aHandle       communication handle
 * @param[out] aControl      Protocol Control
 * @param[out] aStatementId  statement identifier
 * @param[in]  aEnv          env
 *
 * @par protocol sequence:
 * @code
 *
 * - command (t)ype      : command type            var_int bytes
 * - (p)eriod            : protocol period         1 byte
 * - statement (i)d      : statement identifier    var_int,var_int bytes
 *
 * field sequence : t-p-i
 *
 * @endcode
 */
stlStatus sclReadGetCursorNameCommand( sclHandle      * aHandle,
                                       stlInt8        * aControl,
                                       stlInt64       * aStatementId,
                                       sclEnv  		  * aEnv )
{
    stlInt16   sResultType;
    scpCursor  sCursor;

    STL_TRY( scpBeginReading( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS );

    SCP_READ_VAR_INT16( aHandle, &sCursor, &sResultType, aEnv );
    STL_DASSERT( sResultType == CML_COMMAND_GETCURSORNAME );
    SCP_READ_INT8( aHandle, &sCursor, aControl, aEnv );
    SCP_READ_STATEMENT_ID( aHandle, &sCursor, aStatementId, aEnv );

    STL_TRY( scpEndReading( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief GetCursorName Result Protocol을 기록한다.
 * @param[in] aHandle        Communication Handle
 * @param[in] aStatementId   Statement Identifier
 * @param[in] aCursorName    Cursor Name
 * @param[in] aErrorStack    기록할 Error Stack
 * @param[in] aEnv           env
 * 
 * @par protocol sequence:
 * @code
 * 
 * - result (t)ype         : result type           2 bytes
 * - e(r)ror level         : error level           1 byte
 * - statement (i)d        : statement identifier  8 bytes
 * - c(u)rsor name         : cursor name           variable
 * - error (c)ount         : error count           1 byte
 * - (s)ql state           : sql state             4 bytes
 * - n(a)tive error        : native error          4 bytes
 * - (m)essage text        : message text          variable
 *
 * field sequence : if r == 0 then t-r-i-u
 *                  else if r == 1 then t-r-c-[s-a-m]^c-i-u
 *                  else t-r-c-[s-a-m]^c
 * 
 * @endcode
 */
stlStatus sclWriteGetCursorNameResult( sclHandle      * aHandle,
                                       stlInt64         aStatementId,
                                       stlChar        * aCursorName,
                                       stlErrorStack  * aErrorStack,
                                       sclEnv         * aEnv )
{
    scpCursor  sCursor;
    stlInt16   sResultType = CML_COMMAND_GETCURSORNAME;
    stlInt32   sLength;

    STL_TRY( scpBeginWriting( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS );

    SCP_WRITE_VAR_INT16( aHandle, &sCursor, &sResultType, aEnv );

    STL_TRY( scpWriteErrorResult( aHandle,
                                  &sCursor,
                                  aErrorStack,
                                  aEnv ) == STL_SUCCESS );

    SCP_WRITE_STATEMENT_ID( aHandle, &sCursor, &aStatementId, aEnv );

    sLength = stlStrlen( aCursorName );
    SCP_WRITE_N_VAR( aHandle, &sCursor, aCursorName, sLength, aEnv );

    STL_TRY( scpEndWriting( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
