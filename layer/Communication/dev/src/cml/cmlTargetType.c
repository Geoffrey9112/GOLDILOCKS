/*******************************************************************************
 * cmlTargetType.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: cmlTargetType.c 6676 2012-12-13 08:39:46Z egon $
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
 * @file cmlTargetType.c
 * @brief Communication Layer TargetType Protocol Routines
 */

/**
 * @addtogroup cmlProtocol
 * @{
 */

/**
 * @brief TargetType Command Protocol을 기록한다.
 * @param[in] aHandle       communication handle
 * @param[in] aControl      Protocol Control
 * @param[in] aStatementId  statement identifier
 * @param[in] aErrorStack   error stack pointer
 * 
 * @par protocol sequence:
 * @code
 * 
 * - command (t)ype   : command type            var_int bytes
 * - (p)eriod         : protocol period         1 byte
 * - statement (i)d   : statement identifier    var_int,var_int byte
 *
 * field sequence : t-p-s
 * 
 * @endcode
 */
stlStatus cmlWriteTargetTypeCommand( cmlHandle      * aHandle,
                                     stlInt8          aControl,
                                     stlInt64         aStatementId,
                                     stlErrorStack  * aErrorStack )
{
    stlInt16    sCommandType = CML_COMMAND_TARGETTYPE;
    cmpCursor   sCursor;

    STL_TRY( cmpBeginWriting( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_WRITE_VAR_INT16( aHandle, &sCursor, &sCommandType, aErrorStack );
    CMP_WRITE_INT8( aHandle, &sCursor, &aControl, aErrorStack );
    CMP_WRITE_STATEMENT_ID_ENDIAN( aHandle, &sCursor, &aStatementId, aErrorStack );

    STL_TRY( cmpEndWriting( aHandle,
                            &sCursor,
                            aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief TargetType Result Protocol을 읽는다.
 * @param[in]  aHandle        communication handle
 * @param[out] aIgnored       Ignore protocol 여부
 * @param[out] aChanged       changed flag
 * @param[out] aTargetCount   target count
 * @param[out] aTargetTypes   target types
 * @param[in]  aErrorStack    error stack pointer
 * 
 * @par protocol sequence:
 * @code
 * 
 * - result (t)ype         : result type         var_int bytes
 * - e(r)ror level         : error level         1 byte
 * - c(h)anged             : changed flag        1 byte
 * - target c(o)unt        : target count        var_int bytes
 * - tar(g)et type         : target type         1 byte
 * - arg(u)ment1           : argument1           var_int,var_int bytes
 * - argume(n)t1           : argument2           var_int,var_int bytes
 * - (s)tring length unit  : string length unit  1 byte
 * - interval in(d)icator  : interval indicator  1 byte
 * - error (c)ount         : error count         1 byte
 * - (s)ql state           : sql state           4 bytes
 * - n(a)tive error        : native error        4 bytes
 * - (m)essage text        : message text        variable
 *
 * field sequence : if r == 0 then t-r-o-[g-u-n-s-d]^o
 *                  else if r == 1 then t-r-c-[s-a-m]^c-o-[g-u-n-s-d]^o
 *                  else t-r-c-[s-a-m]^c
 * 
 * @endcode
 */
stlStatus cmlReadTargetTypeResult( cmlHandle        * aHandle,
                                   stlBool          * aIgnored,
                                   stlBool          * aChanged,
                                   stlInt32         * aTargetCount,
                                   cmlTargetType   ** aTargetTypes,
                                   stlErrorStack    * aErrorStack )
{
    cmpCursor       sCursor;
    stlInt16        sResultType;
    stlInt32        i;
    cmlTargetType * sTargetTypes;

    STL_TRY( cmpBeginReading( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_READ_VAR_INT16( aHandle, &sCursor, &sResultType, aErrorStack );
    STL_DASSERT( sResultType == CML_COMMAND_TARGETTYPE );

    STL_TRY( cmpReadErrorResult( aHandle,
                                 aIgnored,
                                 &sCursor,
                                 aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( *aIgnored == STL_FALSE, RAMP_IGNORE );

    CMP_READ_INT8( aHandle, &sCursor, aChanged, aErrorStack );
    CMP_READ_VAR_INT32( aHandle, &sCursor, aTargetCount, aErrorStack );

    STL_TRY( cmgAllocOperationMem( aHandle,
                                   STL_SIZEOF(cmlTargetType) * (*aTargetCount),
                                   (void**)&sTargetTypes,
                                   aErrorStack )
             == STL_SUCCESS );

    for( i = 0; i < *aTargetCount; i++ )
    {
        CMP_READ_INT8( aHandle, &sCursor, &sTargetTypes[i].mType, aErrorStack );
        CMP_READ_VAR_INT64( aHandle, &sCursor, &sTargetTypes[i].mArgNum1, aErrorStack );
        CMP_READ_VAR_INT64( aHandle, &sCursor, &sTargetTypes[i].mArgNum2, aErrorStack );
        CMP_READ_INT8( aHandle, &sCursor, &sTargetTypes[i].mStringLengthUnit, aErrorStack );
        CMP_READ_INT8( aHandle, &sCursor, &sTargetTypes[i].mIntervalIndicator, aErrorStack );
    }

    *aTargetTypes = sTargetTypes;

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
