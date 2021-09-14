/*******************************************************************************
 * cmlExecute.c
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
#include <cmlDef.h>
#include <cmDef.h>
#include <cmlProtocol.h>
#include <cmlGeneral.h>
#include <cmpCursor.h>
#include <cmpProtocol.h>
#include <cmgMemory.h>

/**
 * @file cmlExecute.c
 * @brief Communication Layer Execute Protocol Routines
 */

/**
 * @addtogroup cmlProtocol
 * @{
 */

/**
 * @brief Execute Command Protocol을 기록한다.
 * @param[in] aHandle        Communication Handle
 * @param[in] aControl       Protocol Control
 * @param[in] aStatementId   Statement Identifier
 * @param[in] aIsFirst       First Flag
 * @param[in] aErrorStack    Error Stack Pointer
 * 
 * @par protocol sequence:
 * @code
 * 
 * - command (t)ype    : command type         var_int bytes
 * - (p)eriod          : protocol period      1 byte
 * - statement (i)d    : statement identifier var_int,var_int bytes
 * - (f)irst           : first flag           1 byte
 *
 * field sequence : t-p-i-f
 * 
 * @endcode
 */
stlStatus cmlWriteExecuteCommand( cmlHandle      * aHandle,
                                  stlInt8          aControl,
                                  stlInt64         aStatementId,
                                  stlBool          aIsFirst,
                                  stlErrorStack  * aErrorStack )
{
    stlInt16    sCommandType = CML_COMMAND_EXECUTE;
    cmpCursor   sCursor;

    STL_TRY( cmpBeginWriting( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_WRITE_VAR_INT16( aHandle, &sCursor, &sCommandType, aErrorStack );
    CMP_WRITE_INT8( aHandle, &sCursor, &aControl, aErrorStack );
    CMP_WRITE_STATEMENT_ID_ENDIAN( aHandle, &sCursor, &aStatementId, aErrorStack );
    CMP_WRITE_INT8( aHandle, &sCursor, &aIsFirst, aErrorStack );

    STL_TRY( cmpEndWriting( aHandle,
                            &sCursor,
                            aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Execute Result Protocol을 읽는다.
 * @param[in]  aHandle            communication handle
 * @param[out] aIgnored           Ignore protocol 여부
 * @param[out] aAffectedRowCount  affected row count
 * @param[out] aRecompile         recompile
 * @param[out] aResultSet         result set
 * @param[out] aHasTrans          has transaction
 * @param[out] aDclAttrCount      DCL attribute count
 * @param[out] aDclAttrType       DCL attribute type
 * @param[out] aDclAttrDataType   DCL attribute data type
 * @param[out] aDclAttrValue      DCL attribute value
 * @param[out] aDclAttrValueSize  DCL attribute value size
 * @param[in]  aErrorStack        error stack pointer
 * 
 * @par protocol sequence:
 * @code
 * 
 * @par protocol sequence:
 * @code
 * 
 * - result (t)ype             : result type             var_int bytes
 * - e(r)ror level             : error level             1 byte
 * - a(f)fected row count      : affected row count      var_int bytes
 * - r(e)compile               : recompile               1 byte
 * - res(u)lt set              : result set              1 byte
 * - (h)as transaction         : has transaction         1 byte
 * - dcl attri(b)ute count     : dcl attribute count     1 byte
 * - dcl attribute ty(p)e      : dcl attribute type      var_int byte
 * - dcl attribute (d)ata type : dcl attribute data type 1 byte
 * - dcl attribute (v)alue     : dcl attribute value     variable
 * - error (c)ount             : error count             1 byte
 * - (s)ql state               : sql state               4 bytes
 * - n(a)tive error            : native error            4 bytes
 * - (m)essage text            : message text            variable
 *
 * field sequence : if r == 0 then
 *                      if b == 0 then t-r-f-e-u-h-b
 *                      else t-r-f-e-u-h-b-p-d-v
 *                  else if r == 1 then
 *                      if b == 0 then t-r-c-[s-a-m]^c-f-e-u-h-b
 *                      else t-r-c-[s-a-m]^c-f-e-u-h-b-p-d-v
 *                  else t-r-c-[s-a-m]^c
 * 
 * @endcode
 */
stlStatus cmlReadExecuteResult( cmlHandle      * aHandle,
                                stlBool        * aIgnored,
                                stlInt64       * aAffectedRowCount,
                                stlBool        * aRecompile,
                                stlBool        * aResultSet,
                                stlBool        * aHasTrans,
                                stlInt8        * aDclAttrCount,
                                stlInt32       * aDclAttrType,
                                dtlDataType    * aDclAttrDataType,
                                stlChar        * aDclAttrValue,
                                stlInt32       * aDclAttrValueSize,
                                stlErrorStack  * aErrorStack )
{
    cmpCursor    sCursor;
    stlInt16     sResultType;
    stlInt8      sDclAttrDataType;
    stlInt32     sDclAttrValueSize;

    STL_TRY( cmpBeginReading( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_READ_VAR_INT16( aHandle, &sCursor, &sResultType, aErrorStack );
    STL_DASSERT( sResultType == CML_COMMAND_EXECUTE );

    STL_TRY( cmpReadErrorResult( aHandle,
                                 aIgnored,
                                 &sCursor,
                                 aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( *aIgnored == STL_FALSE, RAMP_IGNORE );

    CMP_READ_VAR_INT64( aHandle, &sCursor, aAffectedRowCount, aErrorStack );
    CMP_READ_INT8( aHandle, &sCursor, aRecompile, aErrorStack );
    CMP_READ_INT8( aHandle, &sCursor, aResultSet, aErrorStack );
    CMP_READ_INT8( aHandle, &sCursor, aHasTrans, aErrorStack );
    CMP_READ_INT8( aHandle, &sCursor, aDclAttrCount, aErrorStack );

    if( *aDclAttrCount > 0 )
    {
        CMP_READ_VAR_INT32( aHandle, &sCursor, aDclAttrType, aErrorStack );
        CMP_READ_INT8( aHandle, &sCursor, &sDclAttrDataType, aErrorStack );

        if( sDclAttrDataType == DTL_TYPE_NATIVE_INTEGER )
        {
            CMP_READ_VAR_INT32( aHandle, &sCursor, (stlInt32*)aDclAttrValue, aErrorStack );
            sDclAttrValueSize = 4;
        }
        else
        {
            CMP_READ_N_VAR_ENDIAN( aHandle, &sCursor, aDclAttrValue, sDclAttrValueSize, aErrorStack );
        }
        
        *aDclAttrDataType = sDclAttrDataType;
        *aDclAttrValueSize = sDclAttrValueSize;
    }

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
