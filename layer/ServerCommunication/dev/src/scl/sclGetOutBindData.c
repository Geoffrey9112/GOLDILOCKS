/*******************************************************************************
 * cmlGetOutBindData.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: cmlSetConnAttr.c 6676 2012-12-13 08:39:46Z egon $
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
 * @file sclGetOutBindData.c
 * @brief Communication Layer GetOutBindData Protocol Routines
 */

/**
 * @addtogroup cmlProtocol
 * @{
 */


/**
 * @brief GetOutBindData Command Protocol을 읽는다.
 * @param[in] aHandle        Communication Handle
 * @param[in] aControl       Protocol Control
 * @param[in] aStatementId   Statement Identifier
 * @param[in] aColumnNumber  Column Number
 * @param[in] aEnv           env
 * 
 * @par protocol sequence:
 * @code
 * 
 * - command (t)ype    : command type         var_int bytes
 * - (p)eriod          : protocol period      1 byte
 * - statement (i)d    : statement identifier var_int,var_int bytes
 * - column (n)umber   : column number        var_int byte
 *
 * field sequence : t-p-i-n
 * 
 * @endcode
 */
stlStatus sclReadGetOutBindDataCommand( sclHandle      * aHandle,
                                        stlInt8        * aControl,
                                        stlInt64       * aStatementId,
                                        stlInt32       * aColumnNumber,
                                        sclEnv         * aEnv )
{
    stlInt16    sCommandType;
    scpCursor   sCursor;

    STL_TRY( scpBeginReading( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS );

    SCP_READ_VAR_INT16( aHandle, &sCursor, &sCommandType, aEnv );
    STL_DASSERT( sCommandType == CML_COMMAND_GETOUTBINDDATA );
    
    SCP_READ_INT8( aHandle, &sCursor, aControl, aEnv );
    SCP_READ_STATEMENT_ID( aHandle, &sCursor, aStatementId, aEnv );
    SCP_READ_VAR_INT32( aHandle, &sCursor, aColumnNumber, aEnv );
    
    STL_TRY( scpEndReading( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief GetOutBindData Result Protocol을 기록한다.
 * @param[in] aHandle            communication handle
 * @param[in] aBindValue         data value pointer
 * @param[in] aErrorStack        기록할 Error Stack
 * @param[in] aEnv               env
 * 
 * @par protocol sequence:
 * @code
 * 
 * - result (t)ype             : result type             var_int bytes
 * - e(r)ror level             : error level             1 byte
 * - column t(y)pe             : column type             1 byte
 * - column (v)alue            : column value            variable
 * - error (c)ount             : error count             1 byte
 * - (s)ql state               : sql state               4 bytes
 * - n(a)tive error            : native error            4 bytes
 * - (m)essage text            : message text            variable
 *
 * field sequence : if r == 0 then t-r-y-v
 *                  else if r == 1 then t-r-c-[s-a-m]^c-y-v
 *                  else t-r-c-[s-a-m]^c
 * 
 * @endcode
 */
stlStatus sclWriteGetOutBindDataResult( sclHandle      * aHandle,
                                        dtlDataValue   * aBindValue,
                                        stlErrorStack  * aErrorStack,
                                        sclEnv         * aEnv )
{
    scpCursor    sCursor;
    stlInt16     sResultType = CML_COMMAND_GETOUTBINDDATA;
    stlInt8      sBindType = aBindValue->mType;

    STL_TRY( scpBeginWriting( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS );

    SCP_WRITE_VAR_INT16( aHandle, &sCursor, &sResultType, aEnv );

    STL_TRY( scpWriteErrorResult( aHandle,
                                  &sCursor,
                                  aErrorStack,
                                  aEnv ) == STL_SUCCESS );

    SCP_WRITE_INT8( aHandle, &sCursor, &sBindType, aEnv );
    
    SCP_WRITE_N_VAR( aHandle, &sCursor, aBindValue->mValue, aBindValue->mLength, aEnv );

    STL_TRY( scpEndWriting( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
