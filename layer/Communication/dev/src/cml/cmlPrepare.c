/*******************************************************************************
 * cmlPrepare.c
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
 * @file cmlPrepare.c
 * @brief Communication Layer Prepare Protocol Routines
 */

/**
 * @addtogroup cmlProtocol
 * @{
 */

/**
 * @brief Prepare Command Protocol을 기록한다.
 * @param[in] aHandle        Communication Handle
 * @param[in] aControl       Protocol Control
 * @param[in] aStatementId   Statement Identifier
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
 * - s(q)l text        : attribute data type  variable
 *
 * field sequence : t-p-i-q
 * 
 * @endcode
 */
stlStatus cmlWritePrepareCommand( cmlHandle      * aHandle,
                                  stlInt8          aControl,
                                  stlInt64         aStatementId,
                                  stlChar        * aSqlText,
                                  stlInt64         aSqlTextSize,
                                  stlErrorStack  * aErrorStack )
{
    stlInt16    sCommandType = CML_COMMAND_PREPARE;
    cmpCursor   sCursor;

    STL_TRY( cmpBeginWriting( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_WRITE_VAR_INT16( aHandle, &sCursor, &sCommandType, aErrorStack );
    CMP_WRITE_INT8( aHandle, &sCursor, &aControl, aErrorStack );
    CMP_WRITE_STATEMENT_ID_ENDIAN( aHandle, &sCursor, &aStatementId, aErrorStack );

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
 * @brief Prepare Result Protocol을 읽는다.
 * @param[in] aHandle               Communication Handle
 * @param[out] aIgnored             Ignore protocol 여부
 * @param[out] aStatementId         Statement Identifier
 * @param[out] aStatementType       statement type
 * @param[out] aHasTrans            Has Transaction
 * @param[out] aResultSet           result set
 * @param[out] aIsWithoutHoldCursor without hold cursor
 * @param[out] aIsUpdatable         updatable
 * @param[out] aSensitivity         sensitivity
 * @param[in] aErrorStack           Error Stack Pointer
 * 
 * @par protocol sequence:
 * @code
 * 
 * - result (t)ype         : result type           var_int bytes
 * - e(r)ror level         : error level           1 byte
 * - statement (i)d        : statement identifier  var_int,var_int bytes
 * - statement t(y)pe      : statement type        var_int bytes
 * - (h)as transaction     : has transaction       1 byte
 * - res(u)lt set          : result set            1 byte
 * - (w)ithout hold cursor : without hold cursor   1 byte
 * - updatab(l)e           : updatable             1 byte
 * - se(n)sitivity         : sensitivity           1 byte
 * - error (c)ount         : error count           1 byte
 * - (s)ql state           : sql state             4 bytes
 * - n(a)tive error        : native error          4 bytes
 * - (m)essage text        : message text          variable
 *
 * field sequence : if r == 0 then t-r-i-y-h-u-w-l-n
 *                  else if r == 1 then t-r-c-[s-a-m]^c-i-y-h-u-w-l-n
 *                  else t-r-c-[s-a-m]^c
 * 
 * @endcode
 */
stlStatus cmlReadPrepareResult( cmlHandle      * aHandle,
                                stlBool        * aIgnored,
                                stlInt64       * aStatementId,
                                stlInt32       * aStatementType,
                                stlBool        * aHasTrans,
                                stlBool        * aResultSet,
                                stlBool        * aIsWithoutHoldCursor,
                                stlBool        * aIsUpdatable,
                                stlInt8        * aSensitivity,
                                stlErrorStack  * aErrorStack )
{
    cmpCursor    sCursor;
    stlInt16     sResultType;

    STL_TRY( cmpBeginReading( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_READ_VAR_INT16( aHandle, &sCursor, &sResultType, aErrorStack );
    STL_DASSERT( sResultType == CML_COMMAND_PREPARE );

    STL_TRY( cmpReadErrorResult( aHandle,
                                 aIgnored,
                                 &sCursor,
                                 aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( *aIgnored == STL_FALSE, RAMP_IGNORE );

    CMP_READ_STATEMENT_ID_ENDIAN( aHandle, &sCursor, aStatementId, aErrorStack );
    CMP_READ_VAR_INT32( aHandle, &sCursor, aStatementType, aErrorStack );
    CMP_READ_INT8( aHandle, &sCursor, aHasTrans, aErrorStack );
    CMP_READ_INT8( aHandle, &sCursor, aResultSet, aErrorStack );
    CMP_READ_INT8( aHandle, &sCursor, aIsWithoutHoldCursor, aErrorStack );
    CMP_READ_INT8( aHandle, &sCursor, aIsUpdatable, aErrorStack );
    CMP_READ_INT8( aHandle, &sCursor, aSensitivity, aErrorStack );
    
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
