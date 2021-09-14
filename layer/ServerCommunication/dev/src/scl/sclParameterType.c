/*******************************************************************************
 * cmlParameterType.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
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
 * @file sclParameterType.c
 * @brief Communication Layer ParameterType Protocol Routines
 */

/**
 * @addtogroup cmlProtocol
 * @{
 */

/**
 * @brief ParameterType Command Protocol을 읽는다.
 * @param[in]  aHandle          communication handle
 * @param[out] aControl         Protocol Control
 * @param[out] aStatementId     statement identifier
 * @param[out] aParameterNumber parameter number
 * @param[in]  aEnv             env
 * 
 * @par protocol sequence:
 * @code
 * 
 * - command (t)ype      : command type            var_int bytes
 * - (p)eriod            : protocol period         1 byte
 * - statement (i)d      : statement identifier    var_int,var_int byte
 * - parameter (n)umber  : parameter number        var_int byte
 *
 * field sequence : t-p-s-n
 * 
 * @endcode
 */
stlStatus sclReadParameterTypeCommand( sclHandle     * aHandle,
                                       stlInt8       * aControl,
                                       stlInt64      * aStatementId,
                                       stlInt32      * aParameterNumber,
                                       sclEnv        * aEnv )
{
    stlInt16  sResultType;
    scpCursor sCursor;

    STL_TRY( scpBeginReading( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS );

    SCP_READ_VAR_INT16( aHandle, &sCursor, &sResultType, aEnv );
    SCP_READ_INT8( aHandle, &sCursor, aControl, aEnv );
    SCP_READ_STATEMENT_ID( aHandle, &sCursor, aStatementId, aEnv );
    SCP_READ_VAR_INT32( aHandle, &sCursor, aParameterNumber, aEnv );

    STL_DASSERT( sResultType == CML_COMMAND_PARAMETERTYPE );
    
    STL_TRY( scpEndReading( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ParameterType Result Protocol을 기록한다.
 * @param[in] aHandle           communication handle
 * @param[in] aInputOutputType  parameter type
 * @param[in] aErrorStack       기록할 Error Stack
 * @param[in] aEnv              env
 *
 * @par protocol sequence:
 * @code
 * 
 * - result (t)ype         : result type         var_int bytes
 * - e(r)ror level         : error level         1 byte
 * - p(a)rameter type      : parameter type      var_int byte
 * - error (c)ount         : error count         1 byte
 * - (s)ql state           : sql state           4 bytes
 * - n(a)tive error        : native error        4 bytes
 * - (m)essage text        : message text        variable
 *
 * field sequence : if r == 0 then t-r-a
 *                  else if r == 1 then t-r-c-[s-a-m]^c-a
 *                  else t-r-c-[s-a-m]^c
 * 
 * @endcode
 */
stlStatus sclWriteParameterTypeResult( sclHandle     * aHandle,
                                       stlInt16        aInputOutputType,
                                       stlErrorStack * aErrorStack,
                                       sclEnv        * aEnv )
{
    scpCursor sCursor;
    stlInt16  sResultType = CML_COMMAND_PARAMETERTYPE;

    STL_TRY( scpBeginWriting( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS );

    SCP_WRITE_VAR_INT16( aHandle, &sCursor, &sResultType, aEnv );

    STL_TRY( scpWriteErrorResult( aHandle,
                                  &sCursor,
                                  aErrorStack,
                                  aEnv ) == STL_SUCCESS );

    SCP_WRITE_VAR_INT16( aHandle, &sCursor, &aInputOutputType, aEnv );

    STL_TRY( scpEndWriting( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} */
