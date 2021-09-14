/*******************************************************************************
 * cmlSetStmtAttr.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: cmlSetStmtAttr.c 6676 2012-12-13 08:39:46Z egon $
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
 * @file sclSetStmtAttr.c
 * @brief Communication Layer SetStmtAttr Protocol Routines
 */

/**
 * @addtogroup cmlProtocol
 * @{
 */

/**
 * @brief SetStmtAttr Command Protocol을 읽는다.
 * @param[in]  aHandle        communication handle
 * @param[out] aControl       Protocol Control
 * @param[out] aStatementId   statement identifier
 * @param[out] aAttrType      attribute yype
 * @param[out] aAttrDataType  attribute dataType
 * @param[out] aAttrValue     attribute value
 * @param[out] aAttrValueSize attribute value Size
 * @param[in]  aEnv           env
 * @remark aAttrDataType은 NATIVE_INTEGER나 DTL_TYPE_BINARY이여야 한다.
 * 
 * @par protocol sequence:
 * @code
 * 
 * - command (t)ype        : command type          var_int bytes
 * - (p)eriod              : protocol period       1 byte
 * - statement (i)d        : statement identifier  var_int,var_int byte
 * - (a)ttribute type      : attribute type        var_int bytes
 * - attribute (d)ata type : attribute data type   1 byte
 * - attribute (v)alue     : attribute value       variable
 *
 * field sequence : t-p-i-a-d-v
 * 
 * @endcode
 */
stlStatus sclReadSetStmtAttrCommand( sclHandle      * aHandle,
                                     stlInt8        * aControl,
                                     stlInt64       * aStatementId,
                                     stlInt32       * aAttrType,
                                     dtlDataType    * aAttrDataType,
                                     void           * aAttrValue,
                                     stlInt16       * aAttrValueSize,
                                     sclEnv         * aEnv )
{
    stlInt16    sCommandType;
    scpCursor   sCursor;
    stlInt8     sAttrDataType;
    stlInt16    sAttrValueSize;

    STL_TRY( scpBeginReading( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS );

    SCP_READ_VAR_INT16( aHandle, &sCursor, &sCommandType, aEnv );
    STL_DASSERT( sCommandType == CML_COMMAND_SETSTMTATTR );
    
    SCP_READ_INT8( aHandle, &sCursor, aControl, aEnv );
    SCP_READ_STATEMENT_ID( aHandle, &sCursor, aStatementId, aEnv );
    SCP_READ_VAR_INT32( aHandle, &sCursor, aAttrType, aEnv );
    SCP_READ_INT8( aHandle, &sCursor, &sAttrDataType, aEnv );

    if( sAttrDataType == DTL_TYPE_NATIVE_INTEGER )
    {
        SCP_READ_VAR_INT32( aHandle, &sCursor, (stlInt32*)aAttrValue, aEnv );
        sAttrValueSize = 4;
    }
    else
    {
        SCP_READ_N_VAR( aHandle, &sCursor, aAttrValue, sAttrValueSize, aEnv );
    }

    *aAttrValueSize = sAttrValueSize;
    *aAttrDataType = sAttrDataType;
    
    STL_TRY( scpEndReading( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief SetStmtAttr Result Protocol을 기록한다.
 * @param[in] aHandle        communication handle
 * @param[in] aStatementId   Statement Identifier
 * @param[in] aErrorStack    기록할 Error Stack
 * @param[in] aEnv           env
 * 
 * @par protocol sequence:
 * @code
 * 
 * - result (t)ype         : result type         var_int bytes
 * - e(r)ror level         : error level         1 byte
 * - statement (i)d        : statement identifier  var_int,var_int bytes
 * - error (c)ount         : error count         1 byte
 * - (s)ql state           : sql state           4 bytes
 * - n(a)tive error        : native error        4 bytes
 * - (m)essage text        : message text        variable
 *
 * field sequence : if r == 0 then t-r
 *                  else if r == 1 then t-r-c-[s-a-m]^c
 *                  else t-r-c-[s-a-m]^c
 * 
 * @endcode
 */
stlStatus sclWriteSetStmtAttrResult( sclHandle     * aHandle,
                                     stlInt64        aStatementId,
                                     stlErrorStack * aErrorStack,
                                     sclEnv        * aEnv )
{
    scpCursor    sCursor;
    stlInt16     sResultType = CML_COMMAND_SETSTMTATTR;

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
