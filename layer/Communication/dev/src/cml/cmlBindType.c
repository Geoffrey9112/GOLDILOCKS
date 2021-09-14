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
#include <cmlDef.h>
#include <cmDef.h>
#include <cmlProtocol.h>
#include <cmlGeneral.h>
#include <cmpCursor.h>
#include <cmpProtocol.h>
#include <cmgMemory.h>

/**
 * @file cmlBindType.c
 * @brief Communication Layer BindType Protocol Routines
 */

/**
 * @addtogroup cmlProtocol
 * @{
 */

/**
 * @brief BindType Command Protocol을 기록한다.
 * @param[in] aHandle            communication handle
 * @param[in] aControl           Protocol Control
 * @param[in] aStatementId       statement identifier
 * @param[in] aColumnNumber      column number
 * @param[in] aBindType          bind type array
 * @param[in] aDataType          data type
 * @param[in] aArg1              argument 1
 * @param[in] aArg2              argument 2
 * @param[in] aStringLengthUnit  string length unit
 * @param[in] aIntervalIndicator interval indicator
 * @param[in] aErrorStack        error stack pointer
 * 
 * @par protocol sequence:
 * @code
 * 
 * - command (t)ype        : command type            var_int bytes
 * - (p)eriod              : protocol period         1 byte
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
stlStatus cmlWriteBindTypeCommand( cmlHandle      * aHandle,
                                   stlInt8          aControl,
                                   stlInt64         aStatementId,
                                   stlInt32         aColumnNumber,
                                   stlInt8          aBindType,
                                   stlInt8          aDataType,
                                   stlInt64         aArg1,
                                   stlInt64         aArg2,
                                   stlInt8          aStringLengthUnit,
                                   stlInt8          aIntervalIndicator,
                                   stlErrorStack  * aErrorStack )
{
    stlInt16    sCommandType = CML_COMMAND_BINDTYPE;
    cmpCursor   sCursor;

    STL_TRY( cmpBeginWriting( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_WRITE_VAR_INT16( aHandle, &sCursor, &sCommandType, aErrorStack );
    CMP_WRITE_INT8( aHandle, &sCursor, &aControl, aErrorStack );
    CMP_WRITE_STATEMENT_ID_ENDIAN( aHandle, &sCursor, &aStatementId, aErrorStack );
    CMP_WRITE_VAR_INT32( aHandle, &sCursor, &aColumnNumber, aErrorStack );
    CMP_WRITE_INT8( aHandle, &sCursor, &aBindType, aErrorStack );
    CMP_WRITE_INT8( aHandle, &sCursor, &aDataType, aErrorStack );
    CMP_WRITE_VAR_INT64( aHandle, &sCursor, &aArg1, aErrorStack );
    CMP_WRITE_VAR_INT64( aHandle, &sCursor, &aArg2, aErrorStack );
    CMP_WRITE_INT8( aHandle, &sCursor, &aStringLengthUnit, aErrorStack );
    CMP_WRITE_INT8( aHandle, &sCursor, &aIntervalIndicator, aErrorStack );

    STL_TRY( cmpEndWriting( aHandle,
                            &sCursor,
                            aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief BindType Result Protocol을 읽는다.
 * @param[in]  aHandle         Communication Handle
 * @param[out] aIgnored        Ignore protocol 여부
 * @param[out] aStatementId    statement identifier
 * @param[in]  aErrorStack     Error Stack Pointer
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
 * field sequence : if r == 0 then t-r
 *                  else if r == 1 then t-r-c-[s-a-m]^c
 *                  else t-r-c-[s-a-m]^c
 * 
 * @endcode
 */
stlStatus cmlReadBindTypeResult( cmlHandle      * aHandle,
                                 stlBool        * aIgnored,
                                 stlInt64       * aStatementId,
                                 stlErrorStack  * aErrorStack )
{
    cmpCursor    sCursor;
    stlInt16     sResultType;

    STL_TRY( cmpBeginReading( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_READ_VAR_INT16( aHandle, &sCursor, &sResultType, aErrorStack );
    STL_DASSERT( sResultType == CML_COMMAND_BINDTYPE );

    STL_TRY( cmpReadErrorResult( aHandle,
                                 aIgnored,
                                 &sCursor,
                                 aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( *aIgnored == STL_FALSE, RAMP_IGNORE );

    CMP_READ_STATEMENT_ID_ENDIAN( aHandle, &sCursor, aStatementId, aErrorStack );

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
