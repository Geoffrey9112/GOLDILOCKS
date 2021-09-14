/*******************************************************************************
 * cmlExecDirect.c
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
 * @file cmlExecDirect.c
 * @brief Communication Layer ExecDirect Protocol Routines
 */

/**
 * @addtogroup cmlProtocol
 * @{
 */

/**
 * @brief ExecDirect Command Protocol을 기록한다.
 * @param[in] aHandle        Communication Handle
 * @param[in] aControl       Protocol Control
 * @param[in] aStatementId   Statement Identifier
 * @param[in] aIsFirst       First Flag
 * @param[in] aSqlText       Sql Text
 * @param[in] aSqlTextSize   Sql Text Size
 * @param[in] aErrorStack    Error Stack Pointer
 * 
 * @par protocol sequence:
 * @code
 * 
 * - command (t)ype    : command type         var_int bytes
 * - (p)eriod          : protocol period      1 byte
 * - statement (i)d    : statement identifier var_int,var_int bytes
 * - (f)irst           : first flag           1 byte
 * - s(q)l text        : attribute data type  variable
 *
 * field sequence : t-p-i-f-q
 * 
 * @endcode
 */
stlStatus cmlWriteExecDirectCommand( cmlHandle      * aHandle,
                                     stlInt8          aControl,
                                     stlInt64         aStatementId,
                                     stlBool          aIsFirst,
                                     stlChar        * aSqlText,
                                     stlInt64         aSqlTextSize,
                                     stlErrorStack  * aErrorStack )
{
    stlInt16    sCommandType = CML_COMMAND_EXECDIRECT;
    cmpCursor   sCursor;

    STL_TRY( cmpBeginWriting( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_WRITE_VAR_INT16( aHandle, &sCursor, &sCommandType, aErrorStack );
    CMP_WRITE_INT8( aHandle, &sCursor, &aControl, aErrorStack );
    CMP_WRITE_STATEMENT_ID_ENDIAN( aHandle, &sCursor, &aStatementId, aErrorStack );
    CMP_WRITE_INT8( aHandle, &sCursor, &aIsFirst, aErrorStack );

    CMP_WRITE_N_VAR_ENDIAN( aHandle, &sCursor, aSqlText, aSqlTextSize, aErrorStack );

    STL_TRY( cmpEndWriting( aHandle,
                            &sCursor,
                            aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ExecDirect Result Protocol을 읽는다.
 * @param[in]  aHandle              communication handle
 * @param[out] aIgnored           Ignore protocol 여부
 * @param[out] aStatementId         statement identifier
 * @param[out] aStatementType       statement type
 * @param[out] aAffectedRowCount    affected row count
 * @param[out] aRecompile           recompile
 * @param[out] aResultSet           result set
 * @param[out] aIsWithoutHoldCursor without hold cursor
 * @param[out] aIsUpdatable         updatable
 * @param[out] aSensitivity         sensitivity
 * @param[out] aHasTrans            has transaction
 * @param[out] aDclAttrCount        DCL attribute count
 * @param[out] aDclAttrType         DCL attribute type
 * @param[out] aDclAttrDataType     DCL attribute data type
 * @param[out] aDclAttrValue        DCL attribute value
 * @param[out] aDclAttrValueSize    DCL attribute value size
 * @param[in]  aErrorStack          error stack pointer
 * 
 * @par protocol sequence:
 * @code
 * 
 * - result (t)ype             : result type             var_int bytes
 * - e(r)ror level             : error level             1 byte
 * - statement (i)d            : statement identifier    var_int,var_int bytes
 * - statement t(y)pe          : statement type          var_int bytes
 * - a(f)fected row count      : affected row count      var_int bytes
 * - r(e)compile               : recompile               1 byte
 * - res(u)lt set              : result set              1 byte
 * - (w)ithout hold cursor     : without hold cursor     1 byte
 * - updatab(l)e               : updatable               1 byte
 * - se(n)sitivity             : sensitivity             1 byte
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
 *                      if b == 0 then t-r-i-y-f-e-u-w-l-n-h-b
 *                      else t-r-i-y-f-e-u-w-l-n-h-b-p-d-v
 *                  else if r == 1 then
 *                      if b == 0 then t-r-c-[s-a-m]^c-i-y-f-e-u-w-l-n-h-b
 *                      else t-r-c-[s-a-m]^c-i-y-f-e-u-w-l-n-h-b-p-d-v
 *                  else t-r-c-[s-a-m]^c
 * 
 * @endcode
 */
stlStatus cmlReadExecDirectResult( cmlHandle      * aHandle,
                                   stlBool        * aIgnored,
                                   stlInt64       * aStatementId,
                                   stlInt32       * aStatementType,
                                   stlInt64       * aAffectedRowCount,
                                   stlBool        * aRecompile,
                                   stlBool        * aResultSet,
                                   stlBool        * aIsWithoutHoldCursor,
                                   stlBool        * aIsUpdatable,
                                   stlInt8        * aSensitivity,
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
    STL_DASSERT( sResultType == CML_COMMAND_EXECDIRECT );

    STL_TRY( cmpReadErrorResult( aHandle,
                                 aIgnored,
                                 &sCursor,
                                 aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( *aIgnored == STL_FALSE, RAMP_IGNORE );

    CMP_READ_STATEMENT_ID_ENDIAN( aHandle, &sCursor, aStatementId, aErrorStack );
    CMP_READ_VAR_INT32( aHandle, &sCursor, aStatementType, aErrorStack );
    CMP_READ_VAR_INT64( aHandle, &sCursor, aAffectedRowCount, aErrorStack );
    CMP_READ_INT8( aHandle, &sCursor, aRecompile, aErrorStack );
    CMP_READ_INT8( aHandle, &sCursor, aResultSet, aErrorStack );
    CMP_READ_INT8( aHandle, &sCursor, aIsWithoutHoldCursor, aErrorStack );
    CMP_READ_INT8( aHandle, &sCursor, aIsUpdatable, aErrorStack );
    CMP_READ_INT8( aHandle, &sCursor, aSensitivity, aErrorStack );
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
