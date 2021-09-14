/*******************************************************************************
 * cmlSetCursorName.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *
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
 * @file cmlSetCursorName.c
 * @brief Communication Layer Cancel Protocol Routines
 */

/**
 * @addtogroup cmlProtocol
 * @{
 */

/**
 * @brief SetCursorName Command Protocol을 기록한다.
 * @param[in] aHandle       communication handle
 * @param[in] aControl      Protocol Control
 * @param[in] aStatementId  statement identifier
 * @param[in] aCursorName   cursor name
 * @param[in] aErrorStack   error stack pointer
 *
 * @par protocol sequence:
 * @code
 *
 * - command (t)ype      : command type            var_int bytes
 * - (p)eriod            : protocol period         1 byte
 * - statement (i)d      : statement identifier    var_int,var_int bytes
 * - (c)ursor name       : cursor name             variable
 * 
 * field sequence : t-p-i-c
 *
 * @endcode
 */
stlStatus cmlWriteSetCursorNameCommand( cmlHandle      * aHandle,
                                        stlInt8          aControl,
                                        stlInt64         aStatementId,
                                        stlChar        * aCursorName,
                                        stlErrorStack  * aErrorStack )
{
    stlInt16   sCommandType = CML_COMMAND_SETCURSORNAME;
    cmpCursor  sCursor;
    stlInt32   sLength;
    
    STL_TRY( cmpBeginWriting( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_WRITE_VAR_INT16( aHandle, &sCursor, &sCommandType, aErrorStack );
    CMP_WRITE_INT8( aHandle, &sCursor, &aControl, aErrorStack );
    CMP_WRITE_STATEMENT_ID_ENDIAN( aHandle, &sCursor, &aStatementId, aErrorStack );
    sLength = stlStrlen( aCursorName );
    CMP_WRITE_N_VAR_ENDIAN( aHandle, &sCursor, aCursorName, sLength, aErrorStack );
    
    STL_TRY( cmpEndWriting( aHandle,
                            &sCursor,
                            aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SetCursorName Result Protocol을 읽는다.
 * @param[in] aHandle        Communication Handle
 * @param[out] aIgnored      Ignore protocol 여부
 * @param[in] aStatementId   Statement Identifier
 * @param[in] aErrorStack    Error Stack Pointer
 * 
 * @par protocol sequence:
 * @code
 * 
 * - result (t)ype         : result type           var_int bytes
 * - e(r)ror level         : error level           1 byte
 * - statement (i)d        : statement identifier  var_int,var_int bytes
 * - error (c)ount         : error count           1 byte
 * - (s)ql state           : sql state             4 bytes
 * - n(a)tive error        : native error          4 bytes
 * - (m)essage text        : message text          variable
 *
 * field sequence : if r == 0 then t-r-i
 *                  else if r == 1 then t-r-c-[s-a-m]^c-i
 *                  else t-r-c-[s-a-m]^c
 * 
 * @endcode
 */
stlStatus cmlReadSetCursorNameResult( cmlHandle      * aHandle,
                                      stlBool        * aIgnored,
                                      stlInt64       * aStatementId,
                                      stlErrorStack  * aErrorStack )
{
    cmpCursor   sCursor;
    stlInt16    sResultType;

    STL_TRY( cmpBeginReading( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_READ_VAR_INT16( aHandle, &sCursor, &sResultType, aErrorStack );
    STL_DASSERT( sResultType == CML_COMMAND_SETCURSORNAME );

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
