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
#include <knl.h>
#include <sclDef.h>
#include <scDef.h>
#include <scpProtocol.h>
#include <sclGeneral.h>
#include <sccCommunication.h>

/**
 * @file sclExecute.c
 * @brief Communication Layer Execute Protocol Routines
 */

/**
 * @addtogroup cmlProtocol
 * @{
 */


/**
 * @brief Execute Command Protocol을 읽는다.
 * @param[in] aHandle        Communication Handle
 * @param[in] aControl       Protocol Control
 * @param[in] aStatementId   Statement Identifier
 * @param[in] aIsFirst       First Flag
 * @param[in] aEnv           env
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
stlStatus sclReadExecuteCommand( sclHandle      * aHandle,
                                 stlInt8        * aControl,
                                 stlInt64       * aStatementId,
                                 stlBool        * aIsFirst,
                                 sclEnv         * aEnv )
{
    stlInt16    sCommandType;
    scpCursor   sCursor;

    STL_TRY( scpBeginReading( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS );

    SCP_READ_VAR_INT16( aHandle, &sCursor, &sCommandType, aEnv );
    STL_DASSERT( sCommandType == CML_COMMAND_EXECUTE );
    
    SCP_READ_INT8( aHandle, &sCursor, aControl, aEnv );
    SCP_READ_STATEMENT_ID( aHandle, &sCursor, aStatementId, aEnv );
    SCP_READ_INT8( aHandle, &sCursor, aIsFirst, aEnv );
    
    STL_TRY( scpEndReading( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Execute Result Protocol을 기록한다.
 * @param[in] aHandle            communication handle
 * @param[in] aAffectedRowCount  affected row count
 * @param[in] aRecompile         recompile
 * @param[in] aResultSet         result set
 * @param[in] aHasTrans          has transaction
 * @param[in] aDclAttrCount      DCL attribute count
 * @param[in] aDclAttrType       DCL attribute type
 * @param[in] aDclAttrDataType   DCL attribute data type
 * @param[in] aDclAttrValue      DCL attribute value
 * @param[in] aDclAttrValueSize  DCL attribute value size
 * @param[in] aErrorStack        기록할 Error Stack
 * @param[in] aEnv               env
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
stlStatus sclWriteExecuteResult( sclHandle      * aHandle,
                                 stlInt64         aAffectedRowCount,
                                 stlBool          aRecompile,
                                 stlBool          aResultSet,
                                 stlBool          aHasTrans,
                                 stlInt8          aDclAttrCount,
                                 stlInt32         aDclAttrType,
                                 dtlDataType      aDclAttrDataType,
                                 stlChar        * aDclAttrValue,
                                 stlInt32         aDclAttrValueSize,
                                 stlErrorStack  * aErrorStack,
                                 sclEnv         * aEnv )
{
    scpCursor    sCursor;
    stlInt16     sResultType = CML_COMMAND_EXECUTE;
    stlInt8      sDclAttrDataType = aDclAttrDataType;

    STL_TRY( scpBeginWriting( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS );

    SCP_WRITE_VAR_INT16( aHandle, &sCursor, &sResultType, aEnv );

    STL_TRY( scpWriteErrorResult( aHandle,
                                  &sCursor,
                                  aErrorStack,
                                  aEnv ) == STL_SUCCESS );

    SCP_WRITE_VAR_INT64( aHandle, &sCursor, &aAffectedRowCount, aEnv );
    SCP_WRITE_INT8( aHandle, &sCursor, &aRecompile, aEnv );
    SCP_WRITE_INT8( aHandle, &sCursor, &aResultSet, aEnv );
    SCP_WRITE_INT8( aHandle, &sCursor, &aHasTrans, aEnv );
    SCP_WRITE_INT8( aHandle, &sCursor, &aDclAttrCount, aEnv );

    if( aDclAttrCount > 0 )
    {
        SCP_WRITE_VAR_INT32( aHandle, &sCursor, &aDclAttrType, aEnv );
        SCP_WRITE_INT8( aHandle, &sCursor, &sDclAttrDataType, aEnv );

        if( aDclAttrDataType == DTL_TYPE_NATIVE_INTEGER )
        {
            SCP_WRITE_VAR_INT32( aHandle, &sCursor, (stlInt32*)aDclAttrValue, aEnv );
        }
        else
        {
            SCP_WRITE_N_VAR( aHandle, &sCursor, aDclAttrValue, aDclAttrValueSize, aEnv );
        }
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
