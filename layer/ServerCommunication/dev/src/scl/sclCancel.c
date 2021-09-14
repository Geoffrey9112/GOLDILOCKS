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
#include <knl.h>
#include <sclDef.h>
#include <scDef.h>
#include <scpProtocol.h>
#include <sclGeneral.h>
#include <sccCommunication.h>

/**
 * @file sclCancel.c
 * @brief Communication Layer Cancel Protocol Routines
 */

/**
 * @addtogroup cmlProtocol
 * @{
 */

/**
 * @brief Cancel Command Protocol을 읽는다.
 * @param[in]  aHandle       communication handle
 * @param[out] aControl      Protocol Control
 * @param[out] aSessionId    session identifier
 * @param[out] aSessionSeq   session seqeunce
 * @param[out] aStatementId  statement identifier
 * @param[in]  aEnv          env
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
stlStatus sclReadCancelCommand( sclHandle      * aHandle,
                                stlInt8        * aControl,
                                stlInt32       * aSessionId,
                                stlInt64       * aSessionSeq,
                                stlInt64       * aStatementId,
                                sclEnv         * aEnv )
{
    stlInt16   sResultType;
    scpCursor  sCursor;

    STL_TRY( scpBeginReading( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS );

    SCP_READ_VAR_INT16( aHandle, &sCursor, &sResultType, aEnv );
    STL_DASSERT( sResultType == CML_COMMAND_CANCEL );
    SCP_READ_INT8( aHandle, &sCursor, aControl, aEnv );
    SCP_READ_VAR_INT32( aHandle, &sCursor, aSessionId, aEnv );
    SCP_READ_VAR_INT64( aHandle, &sCursor, aSessionSeq, aEnv );
    SCP_READ_STATEMENT_ID( aHandle, &sCursor, aStatementId, aEnv );

    STL_TRY( scpEndReading( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Cancel Result Protocol을 기록한다.
 * @param[in] aHandle        Communication Handle
 * @param[in] aErrorStack    기록할 Error Stack
 * @param[in] aEnv           env
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
stlStatus sclWriteCancelResult( sclHandle      * aHandle,
                                stlErrorStack  * aErrorStack,
                                sclEnv         * aEnv )
{
    scpCursor sCursor;
    stlInt16  sResultType = CML_COMMAND_CANCEL;

    STL_TRY( scpBeginWriting( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS );

    SCP_WRITE_VAR_INT16( aHandle, &sCursor, &sResultType, aEnv );

    STL_TRY( scpWriteErrorResult( aHandle,
                                  &sCursor,
                                  aErrorStack,
                                  aEnv ) == STL_SUCCESS );

    STL_TRY( scpEndWriting( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
}


/** @} */
