/*******************************************************************************
 * cmlGetStmtAttr.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: cmlGetStmtAttr.c 6676 2012-12-13 08:39:46Z egon $
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
 * @file sclGetStmtAttr.c
 * @brief Communication Layer GetStmtAttr Protocol Routines
 */

/**
 * @addtogroup cmlProtocol
 * @{
 */

/**
 * @brief GetStmtAttr Command Protocol을 읽는다.
 * @param[in]  aHandle                Communication Handle
 * @param[out] aControl                Protocol Control
 * @param[out] aStatementId            Statement Identifier
 * @param[out] aAttrType               Attribute Type
 * @param[out] aStringBufferLength     String buffer length
 * @param[in]  aEnv                    env
 * 
 * @par protocol sequence:
 * @code
 * 
 * - command (t)ype   : command type          var_int bytes
 * - (p)eriod         : protocol period       1 byte
 * - statement (i)d   : statement identifier  var_int,var_int byte
 * - (a)ttribute type : attribute type        var_int bytes
 * - (b)uffer length  : string buffer legnth  var_int bytes
 *
 * field sequence : t-p-i-a-b
 * 
 * @endcode
 */
stlStatus sclReadGetStmtAttrCommand( sclHandle      * aHandle,
                                     stlInt8        * aControl,
                                     stlInt64       * aStatementId,
                                     stlInt32       * aAttrType,
                                     stlInt32       * aStringBufferLength,
                                     sclEnv         * aEnv )
{
    stlInt16    sResultType;
    scpCursor   sCursor;

    STL_TRY( scpBeginReading( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS );

    SCP_READ_VAR_INT16( aHandle, &sCursor, &sResultType, aEnv );
    SCP_READ_INT8( aHandle, &sCursor, aControl, aEnv );
    SCP_READ_STATEMENT_ID( aHandle, &sCursor, aStatementId, aEnv );
    SCP_READ_VAR_INT32( aHandle, &sCursor, aAttrType, aEnv );
    SCP_READ_VAR_INT32( aHandle, &sCursor, aStringBufferLength, aEnv );

    STL_DASSERT( sResultType == CML_COMMAND_GETSTMTATTR );
    
    STL_TRY( scpEndReading( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief GetStmtAttr Result Protocol을 기록한다.
 * @param[in] aHandle        Communication Handle
 * @param[in] aStatementId   statement identifier
 * @param[in] aAttrType      Attribute Type
 * @param[in] aAttrDataType  Attribute DataType
 * @param[in] aAttrValue     Attribute Value
 * @param[in] aAttrValueSize Attribute Value Size
 * @param[in] aStringLength  @a aAttrType 이 string일 경우 value의 전체 길이
 * @param[in] aErrorStack    기록할 Error Stack
 * @param[in] aEnv           env
 * @remark aAttrDataType은 NATIVE_INTEGER나 DTL_TYPE_BINARY이여야 한다.
 * 
 * @par protocol sequence:
 * @code
 * 
 * - result (t)ype         : result type          var_int bytes
 * - e(r)ror level         : error level          1 byte
 * - statement (i)d        : statement identifier var_int,var_int bytes
 * - attribute t(y)pe      : attribute type       var_int bytes
 * - attribute (d)ata type : attribute data type  1 byte
 * - attribute (v)alue     : attribute value      variable
 * - string (l)egnth       : string legnth        var_int bytes
 * - error (c)ount         : error count          1 byte
 * - (s)ql state           : sql state            4 bytes
 * - n(a)tive error        : native error         4 bytes
 * - (m)essage text        : message text         variable
 *
 * field sequence : if r == 0 then t-r-i-y-d-v-l
 *                  else if r == 1 then t-r-c-[s-a-m]^c-i-y-d-v-l
 *                  else t-r-c-[s-a-m]^c
 * 
 * @endcode
 */
stlStatus sclWriteGetStmtAttrResult( sclHandle         * aHandle,
                                     stlInt64            aStatementId,
                                     stlInt32            aAttrType,
                                     dtlDataType         aAttrDataType,
                                     void              * aAttrValue,
                                     stlInt32            aAttrValueSize,
                                     stlInt32            aStringLength,
                                     stlErrorStack     * aErrorStack,
                                     sclEnv            * aEnv )
{
    scpCursor    sCursor;
    stlInt16     sResultType = CML_COMMAND_GETSTMTATTR;
    stlInt8      sAttrDataType = aAttrDataType;

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
    SCP_WRITE_VAR_INT32( aHandle, &sCursor, &aAttrType, aEnv );
    SCP_WRITE_INT8( aHandle, &sCursor, &sAttrDataType, aEnv );
    
    if( aAttrDataType == DTL_TYPE_NATIVE_INTEGER )
    {
        SCP_WRITE_VAR_INT32( aHandle, &sCursor, (stlInt32*)aAttrValue, aEnv );
    }
    else
    {
        SCP_WRITE_N_VAR( aHandle, &sCursor, aAttrValue, aAttrValueSize, aEnv );
    }

    SCP_WRITE_VAR_INT32( aHandle, &sCursor, &aStringLength, aEnv );

    STL_TRY( scpEndWriting( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
