/*******************************************************************************
 * cmlBindType.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: cmlBindType.c 6676 2012-12-13 08:39:46Z egon $
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
 * @file sclBindType.c
 * @brief Communication Layer BindType Protocol Routines
 */

/**
 * @addtogroup cmlProtocol
 * @{
 */


/**
 * @brief BindType Command Protocol을 읽는다.
 * @param[in]  aHandle            communication handle
 * @param[out] aControl           Protocol Control
 * @param[out] aStatementId       statement identifier
 * @param[out] aColumnNumber      column number array
 * @param[out] aBindType          bind type array
 * @param[out] aDataType          data type
 * @param[out] aArg1              argument 1
 * @param[out] aArg2              argument 2
 * @param[out] aStringLengthUnit  string length unit
 * @param[out] aIntervalIndicator interval indicator
 * @param[in]  aEnv               env
 * 
 * @par protocol sequence:
 * @code
 * 
 * - command (t)ype        : command type            var_int bytes
 * - (p)eriod              : protocol period         1 byte
 * - (h)eading             : protocol heading        1 byte
 * - statement (i)d        : statement identifier    var_int,var_int bytes
 * - (c)olumn number       : column number           var_int bytes
 * - (b)ind type           : bind type               1 bytes
 * - (d)ata type           : data type               1 bytes
 * - (a)rgument1           : argument 1              var_int bytes
 * - a(r)gument2           : argument 2              var_int bytes
 * - (s)tring length unit  : string length unit      1 bytes
 * - i(n)terval indicator  : interval indicator      1 bytes
 *
 * field sequence : t-p-i-c-b-d-a-r-s-n
 * 
 * @endcode
 */
stlStatus sclReadBindTypeCommand( sclHandle      * aHandle,
                                  stlInt8        * aControl,
                                  stlInt64       * aStatementId,
                                  stlInt32       * aColumnNumber,  
                                  stlInt8        * aBindType,  
                                  stlInt8        * aDataType,
                                  stlInt64       * aArg1,
                                  stlInt64       * aArg2,
                                  stlInt8        * aStringLengthUnit,
                                  stlInt8        * aIntervalIndicator,
                                  sclEnv         * aEnv )
{
    stlInt16    sResultType;
    scpCursor   sCursor;
    
    STL_TRY( scpBeginReading( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS );

    SCP_READ_VAR_INT16( aHandle, &sCursor, &sResultType, aEnv );
    STL_DASSERT( sResultType == CML_COMMAND_BINDTYPE );
    SCP_READ_INT8( aHandle, &sCursor, aControl, aEnv );
    SCP_READ_STATEMENT_ID( aHandle, &sCursor, aStatementId, aEnv );
    SCP_READ_VAR_INT32( aHandle, &sCursor, aColumnNumber, aEnv );
    SCP_READ_INT8( aHandle, &sCursor, aBindType, aEnv );
    SCP_READ_INT8( aHandle, &sCursor, aDataType, aEnv );
    SCP_READ_VAR_INT64( aHandle, &sCursor, aArg1, aEnv );
    SCP_READ_VAR_INT64( aHandle, &sCursor, aArg2, aEnv );
    SCP_READ_INT8( aHandle, &sCursor, aStringLengthUnit, aEnv );
    SCP_READ_INT8( aHandle, &sCursor, aIntervalIndicator, aEnv );

    STL_TRY( scpEndReading( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief BindType Result Protocol을 기록한다.
 * @param[in] aHandle        Communication Handle
 * @param[in] aStatementId   statement identifier
 * @param[in] aErrorStack    기록할 Error Stack
 * @param[in] aEnv           env
 * 
 * @par protocol sequence:
 * @code
 * 
 * - result (t)ype         : result type          var_int bytes
 * - e(r)ror level         : error level          1 byte
 * - statement (i)d        : statement identifier var_int,var_int bytes
 * - error (c)ount         : error count          1 byte
 * - (s)ql state           : sql state            4 bytes
 * - n(a)tive error        : native error         4 bytes
 * - (m)essage text        : message text         variable
 *
 * field sequence : if r == 0 then t-r-i
 *                  else if r == 1 then t-r-c-[s-a-m]^c-i
 *                  else t-r-c-[s-a-m]^c
 * 
 * @endcode
 */
stlStatus sclWriteBindTypeResult( sclHandle         * aHandle,
                                  stlInt64            aStatementId,
                                  stlErrorStack     * aErrorStack,
                                  sclEnv            * aEnv )
{
    scpCursor    sCursor;
    stlInt16     sResultType = CML_COMMAND_BINDTYPE;

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

    STL_TRY( scpEndWriting( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
