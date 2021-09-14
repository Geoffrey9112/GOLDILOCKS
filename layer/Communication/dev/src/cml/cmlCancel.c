/*******************************************************************************
 * cmlCancel.c
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
 * @file cmlCancel.c
 * @brief Communication Layer Cancel Protocol Routines
 */

/**
 * @addtogroup cmlProtocol
 * @{
 */

/**
 * @brief Cancel Command Protocol을 기록한다.
 * @param[in] aHandle       communication handle
 * @param[in] aControl      Protocol Control
 * @param[in] aSessionId    session identifier
 * @param[in] aSessionSeq   session seqeunce
 * @param[in] aStatementId  statement identifier
 * @param[in] aErrorStack   error stack pointer
 *
 * @par protocol sequence:
 * @code
 *
 * - command (t)ype      : command type            var_int bytes
 * - (p)eriod            : protocol period         1 byte
 * - s(e)ssion id        : session identifier      var_int bytes
 * - session (s)equence  : session sequence        var_int bytes
 * - statement (i)d      : statement identifier    var_int,var_int bytes
 *
 * field sequence : t-p-e-s-i
 *
 * @endcode
 */
stlStatus cmlWriteCancelCommand( cmlHandle      * aHandle,
                                 stlInt8          aControl,
                                 stlInt32         aSessionId,
                                 stlInt64         aSessionSeq,
                                 stlInt64         aStatementId,
                                 stlErrorStack  * aErrorStack )
{
    stlInt16   sCommandType = CML_COMMAND_CANCEL;
    cmpCursor  sCursor;

    STL_TRY( cmpBeginWriting( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_WRITE_VAR_INT16( aHandle, &sCursor, &sCommandType, aErrorStack );
    CMP_WRITE_INT8( aHandle, &sCursor, &aControl, aErrorStack );
    CMP_WRITE_VAR_INT32( aHandle, &sCursor, &aSessionId, aErrorStack );
    CMP_WRITE_VAR_INT64( aHandle, &sCursor, &aSessionSeq, aErrorStack );
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
 * @brief Cancel Result Protocol을 읽는다.
 * @param[in] aHandle        Communication Handle
 * @param[out] aIgnored      Ignore protocol 여부
 * @param[in] aErrorStack    Error Stack Pointer
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
stlStatus cmlReadCancelResult( cmlHandle      * aHandle,
                               stlBool        * aIgnored,
                               stlErrorStack  * aErrorStack )
{
    cmpCursor   sCursor;
    stlInt16    sResultType;

    STL_TRY( cmpBeginReading( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_READ_VAR_INT16( aHandle, &sCursor, &sResultType, aErrorStack );
    STL_DASSERT( sResultType == CML_COMMAND_CANCEL );

    STL_TRY( cmpReadErrorResult( aHandle,
                                 aIgnored,
                                 &sCursor,
                                 aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( *aIgnored == STL_FALSE, RAMP_IGNORE );

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
