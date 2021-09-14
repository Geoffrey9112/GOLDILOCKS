/*******************************************************************************
 * cmlGetConnAttr.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: cmlGetConnAttr.c 6676 2012-12-13 08:39:46Z egon $
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
 * @file sclGetConnAttr.c
 * @brief Communication Layer GetConnAttr Protocol Routines
 */

/**
 * @addtogroup cmlProtocol
 * @{
 */


/**
 * @brief GetConnAttr Command Protocol을 읽는다.
 * @param[in]  aHandle       Communication Handle
 * @param[out] aControl      Protocol Control
 * @param[out] aAttrType     Attribute Type
 * @param[in]  aEnv          env
 * 
 * @par protocol sequence:
 * @code
 * 
 * - command (t)ype   : command type         var_int bytes
 * - (p)eriod         : protocol period      1 byte
 * - (a)ttribute type : attribute type       var_int bytes
 *
 * field sequence : t-p-a
 * 
 * @endcode
 */
stlStatus sclReadGetConnAttrCommand( sclHandle      * aHandle,
                                     stlInt8        * aControl,
                                     stlInt32       * aAttrType,
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
    SCP_READ_VAR_INT32( aHandle, &sCursor, aAttrType, aEnv );

    STL_DASSERT( sResultType == CML_COMMAND_GETCONNATTR );
    
    STL_TRY( scpEndReading( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief GetConnAttr Result Protocol을 기록한다.
 * @param[in] aHandle        Communication Handle
 * @param[in] aAttrType      Attribute Type
 * @param[in] aAttrDataType  Attribute DataType
 * @param[in] aAttrValue     Attribute Value
 * @param[in] aAttrValueSize Attribute Value Size
 * @param[in] aErrorStack    기록할 Error Stack
 * @param[in] aEnv           env
 * @remark aAttrDataType은 NATIVE_INTEGER나 DTL_TYPE_BINARY이여야 한다.
 * 
 * @par protocol sequence:
 * @code
 * 
 * - result (t)ype         : result type         var_int bytes
 * - e(r)ror level         : error level         1 byte
 * - attribute t(y)pe      : attribute type      var_int bytes
 * - attribute (d)ata type : attribute data type 1 byte
 * - attribute value(i)    : integer attribute value var_int
 * - attribute value(v)    : attribute value     variable
 * - error (c)ount         : error count         1 byte
 * - (s)ql state           : sql state           4 bytes
 * - n(a)tive error        : native error        4 bytes
 * - (m)essage text        : message text        variable
 *
 * field sequence : if r == 0 then t-r-y-d-i or t-r-y-d-v
 *                  else if r == 1 then t-r-c-[s-a-m]^c-y-d-i or t-r-c-[s-a-m]^c-y-d-v
 *                  else t-r-c-[s-a-m]^c
 * 
 * @endcode
 */
stlStatus sclWriteGetConnAttrResult( sclHandle         * aHandle,
                                     stlInt32            aAttrType,
                                     dtlDataType         aAttrDataType,
                                     void              * aAttrValue,
                                     stlInt16            aAttrValueSize,
                                     stlErrorStack     * aErrorStack,
                                     sclEnv            * aEnv )
{
    scpCursor    sCursor;
    stlInt16     sResultType = CML_COMMAND_GETCONNATTR;
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

    SCP_WRITE_VAR_INT32( aHandle, &sCursor, &aAttrType, aEnv );
    SCP_WRITE_INT8( aHandle, &sCursor, &sAttrDataType, aEnv );
    
    if( aAttrDataType == DTL_TYPE_NATIVE_INTEGER )
    {
        SCP_WRITE_VAR_INT32( aHandle, &sCursor, (stlInt32*)aAttrValue, aEnv );
    }
    else
    {
        SCP_WRITE_N_VAR16( aHandle, &sCursor, aAttrValue, aAttrValueSize, aEnv );
    }

    STL_TRY( scpEndWriting( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} */
