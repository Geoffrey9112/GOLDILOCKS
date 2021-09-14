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
#include <cmlDef.h>
#include <cmDef.h>
#include <cmlProtocol.h>
#include <cmlGeneral.h>
#include <cmpCursor.h>
#include <cmpProtocol.h>

/**
 * @file cmlGetConnAttr.c
 * @brief Communication Layer GetConnAttr Protocol Routines
 */

/**
 * @addtogroup cmlProtocol
 * @{
 */

/**
 * @brief GetConnAttr Command Protocol을 기록한다.
 * @param[in] aHandle       Communication Handle
 * @param[in] aControl      Protocol Control
 * @param[in] aAttrType     Attribute Type
 * @param[in] aErrorStack   Error Stack Pointer
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
stlStatus cmlWriteGetConnAttrCommand( cmlHandle      * aHandle,
                                      stlInt8          aControl,
                                      stlInt32         aAttrType,
                                      stlErrorStack  * aErrorStack )
{
    stlInt16    sCommandType = CML_COMMAND_GETCONNATTR;
    cmpCursor   sCursor;

    STL_TRY( cmpBeginWriting( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_WRITE_VAR_INT16( aHandle, &sCursor, &sCommandType, aErrorStack );
    CMP_WRITE_INT8( aHandle, &sCursor, &aControl, aErrorStack );
    CMP_WRITE_VAR_INT32( aHandle, &sCursor, &aAttrType, aErrorStack );

    STL_TRY( cmpEndWriting( aHandle,
                            &sCursor,
                            aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief GetConnAttr Result Protocol을 읽는다.
 * @param[in]  aHandle         Communication Handle
 * @param[out] aIgnored        Ignore protocol 여부
 * @param[out] aAttrType       Attribute Type
 * @param[out] aAttrDataType   Attribute DataType
 * @param[out] aAttrValue      Attribute Value
 * @param[out] aAttrValueSize  Attribute Value Size
 * @param[in]  aErrorStack     Error Stack Pointer
 * @remark aAttrDataType은 NATIVE_INTEGER나 DTL_TYPE_BINARY이여야 한다.
 * 
 * @par protocol sequence:
 * @code
 * 
 * - result (t)ype         : result type         var_int bytes
 * - e(r)ror level         : error level         1 byte
 * - attribute t(y)pe      : attribute type      var_int bytes
 * - attribute (v)alue     : attribute value     variable
 * - attribute (d)ata type : attribute data type 1 byte
 * - error (c)ount         : error count         1 byte
 * - (s)ql state           : sql state           4 bytes
 * - n(a)tive error        : native error        4 bytes
 * - (m)essage text        : message text        variable
 *
 * field sequence : if r == 0 then t-r-y-d-v
 *                  else if r == 1 then t-r-c-[s-a-m]^c-y-d-v
 *                  else t-r-c-[s-a-m]^c
 * 
 * @endcode
 */
stlStatus cmlReadGetConnAttrResult( cmlHandle      * aHandle,
                                    stlBool        * aIgnored,
                                    stlInt32       * aAttrType,
                                    dtlDataType    * aAttrDataType,
                                    void           * aAttrValue,
                                    stlInt16       * aAttrValueSize,
                                    stlErrorStack  * aErrorStack )
{
    cmpCursor    sCursor;
    stlInt16     sResultType;
    stlInt8      sAttrDataType;
    stlInt16     sAttrValueSize;

    STL_TRY( cmpBeginReading( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_READ_VAR_INT16( aHandle, &sCursor, &sResultType, aErrorStack );
    STL_DASSERT( sResultType == CML_COMMAND_GETCONNATTR );

    STL_TRY( cmpReadErrorResult( aHandle,
                                 aIgnored,
                                 &sCursor,
                                 aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( *aIgnored == STL_FALSE, RAMP_IGNORE );

    CMP_READ_VAR_INT32( aHandle, &sCursor, aAttrType, aErrorStack );
    CMP_READ_INT8( aHandle, &sCursor, &sAttrDataType, aErrorStack );

    if( sAttrDataType == DTL_TYPE_NATIVE_INTEGER )
    {
        CMP_READ_VAR_INT32( aHandle, &sCursor, (stlInt32*)aAttrValue, aErrorStack );
        sAttrValueSize = 4;
    }
    else
    {
        CMP_READ_N_VAR_ENDIAN( aHandle, &sCursor, aAttrValue, sAttrValueSize, aErrorStack );
    }

    *aAttrValueSize = sAttrValueSize;
    *aAttrDataType = sAttrDataType;
    
    STL_TRY( cmpEndReading( aHandle,
                            &sCursor,
                            aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_IGNORE )
    {
        STL_ASSERT( cmpEndReading( aHandle,
                                   &sCursor,
                                   aErrorStack ) == STL_SUCCESS );
        return STL_SUCCESS;
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
