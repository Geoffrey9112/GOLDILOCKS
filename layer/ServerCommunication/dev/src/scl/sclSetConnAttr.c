/*******************************************************************************
 * cmlSetConnAttr.c
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
 * @file sclSetConnAttr.c
 * @brief Communication Layer SetConnAttr Protocol Routines
 */

/**
 * @addtogroup cmlProtocol
 * @{
 */


/**
 * @brief SetConnAttr Command Protocol을 읽는다.
 * @param[in] aHandle        Communication Handle
 * @param[in] aControl       Protocol Control
 * @param[in] aAttrType      Attribute Type
 * @param[in] aAttrDataType  Attribute DataType
 * @param[in] aAttrValue     Attribute Value
 * @param[in] aAttrValueSize Attribute Value Size
 * @param[in] aEnv           env
 * @remark aAttrDataType은 NATIVE_INTEGER나 DTL_TYPE_BINARY이여야 한다.
 * 
 * @par protocol sequence:
 * @code
 * 
 * - command (t)ype        : command type         var_int bytes
 * - (p)eriod              : protocol period      1 byte
 * - (a)ttribute type      : attribute type       var_int bytes
 * - attribute (d)ata type : attribute data type  1 byte
 * - attribute (v)alue     : attribute value      variable
 *
 * field sequence : t-p-a-d-v
 * 
 * @endcode
 */
stlStatus sclReadSetConnAttrCommand( sclHandle      * aHandle,
                                     stlInt8        * aControl,
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
    STL_DASSERT( sCommandType == CML_COMMAND_SETCONNATTR );
    
    SCP_READ_INT8( aHandle, &sCursor, aControl, aEnv );
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
 * @brief SetConnAttr Result Protocol을 기록한다.
 * @param[in] aHandle        Communication Handle
 * @param[in] aErrorStack    기록할 Error Stack
 * @param[in] aEnv           env
 * 
 * @par protocol sequence:
 * @code
 * 
 * - result (t)ype         : result type         var_int bytes
 * - e(r)ror level         : error level         1 byte
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
stlStatus sclWriteSetConnAttrResult( sclHandle         * aHandle,
                                     stlErrorStack     * aErrorStack,
                                     sclEnv            * aEnv )
{
    scpCursor    sCursor;
    stlInt16     sResultType = CML_COMMAND_SETCONNATTR;

    STL_TRY( scpBeginWriting( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS );

    SCP_WRITE_VAR_INT16( aHandle, &sCursor, &sResultType, aEnv );

    STL_TRY( scpWriteErrorResult( aHandle,
                                  &sCursor,
                                  aErrorStack,
                                  aEnv ) == STL_SUCCESS );

    STL_TRY( scpEndWriting( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
