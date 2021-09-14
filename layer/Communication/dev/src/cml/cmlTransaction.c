/*******************************************************************************
 * cmlTransaction.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: cmlTransaction.c 6676 2012-12-13 08:39:46Z egon $
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
 * @file cmlTransaction.c
 * @brief Communication Layer Transaction Protocol Routines
 */

/**
 * @addtogroup cmlProtocol
 * @{
 */

/**
 * @brief Transaction Command Protocol을 기록한다.
 * @param[in] aHandle       communication handle
 * @param[in] aControl      Protocol Control
 * @param[in] aOption       option
 * @param[in] aErrorStack   error stack pointer
 * 
 * @par protocol sequence:
 * @code
 * 
 * - command (t)ype   : command type            var_int bytes
 * - (p)eriod         : protocol period         1 byte
 * - (o)ption         : option                  var_int bytes
 *
 * field sequence : t-p-o
 * 
 * @endcode
 */
stlStatus cmlWriteTransactionCommand( cmlHandle      * aHandle,
                                      stlInt8          aControl,
                                      stlInt16         aOption,
                                      stlErrorStack  * aErrorStack )
{
    stlInt16    sCommandType = CML_COMMAND_TRANSACTION;
    cmpCursor   sCursor;

    STL_TRY( cmpBeginWriting( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_WRITE_VAR_INT16( aHandle, &sCursor, &sCommandType, aErrorStack );
    CMP_WRITE_INT8( aHandle, &sCursor, &aControl, aErrorStack );
    CMP_WRITE_VAR_INT16( aHandle, &sCursor, &aOption, aErrorStack );

    STL_TRY( cmpEndWriting( aHandle,
                            &sCursor,
                            aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Transaction Result Protocol을 읽는다.
 * @param[in]  aHandle         Communication Handle
 * @param[in]  aErrorStack     Error Stack Pointer
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
stlStatus cmlReadTransactionResult( cmlHandle      * aHandle,
                                    stlErrorStack  * aErrorStack )
{
    cmpCursor    sCursor;
    stlInt16     sResultType;

    STL_TRY( cmpBeginReading( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_READ_VAR_INT16( aHandle, &sCursor, &sResultType, aErrorStack );
    STL_DASSERT( sResultType == CML_COMMAND_TRANSACTION );

    STL_TRY( cmpReadErrorResult( aHandle,
                                 NULL, /* aIgnored */
                                 &sCursor,
                                 aErrorStack )
             == STL_SUCCESS );

    STL_TRY( cmpEndReading( aHandle,
                            &sCursor,
                            aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
