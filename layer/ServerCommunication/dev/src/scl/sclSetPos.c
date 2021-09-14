/*******************************************************************************
 * cmlSetPos.c
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
 * @file sclSetPos.c
 * @brief Communication Layer Cancel Protocol Routines
 */

/**
 * @addtogroup cmlProtocol
 * @{
 */


/**
 * @brief SetPos Command Protocol을 읽는다.
 * @param[in]  aHandle       communication handle
 * @param[out] aControl      Protocol Control
 * @param[out] aStatementId  statement identifier
 * @param[out] aRowNum       row number
 * @param[out] aOperation    operation type
 * @param[in]  aEnv          env
 *
 * @par protocol sequence:
 * @code
 *
 * - command (t)ype      : command type            var_int bytes
 * - (p)eriod            : protocol period         1 byte
 * - statement (i)d      : statement identifier    var_int,var_int bytes
 * - row (n)umber        ; row number              var_int bytes
 * - (o)peration type    ; operation type          var_int bytes
 * 
 * field sequence : t-p-i-n-o
 *
 * @endcode
 */
stlStatus sclReadSetPosCommand( sclHandle      * aHandle,
                                stlInt8        * aControl,
                                stlInt64       * aStatementId,
                                stlInt64       * aRowNum,
                                stlInt16       * aOperation,
                                sclEnv         * aEnv )
{
    scpCursor   sCursor;
    stlInt16    sResultType;

    STL_TRY( scpBeginReading( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS );

    SCP_READ_VAR_INT16( aHandle, &sCursor, &sResultType, aEnv );
    STL_DASSERT( sResultType == CML_COMMAND_SETPOS );
    SCP_READ_INT8( aHandle, &sCursor, aControl, aEnv );
    SCP_READ_STATEMENT_ID( aHandle, &sCursor, aStatementId, aEnv );
    SCP_READ_VAR_INT64( aHandle, &sCursor, aRowNum, aEnv );
    SCP_READ_VAR_INT16( aHandle, &sCursor, aOperation, aEnv );
    
    STL_TRY( scpEndReading( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief SetPos Result Protocol을 기록한다.
 * @param[in] aHandle        Communication Handle
 * @param[in] aStatementId   statement identifier
 * @param[in] aErrorStack    기록할 Error Stack
 * @param[in] aEnv           env
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
 * field sequence : if r == 0 then t-r-i
 *                  else if r == 1 then t-r-c-[s-a-m]^c-i
 *                  else t-r-c-[s-a-m]^c
 * 
 * @endcode
 */
stlStatus sclWriteSetPosResult( sclHandle      * aHandle,
                                stlInt64         aStatementId,
                                stlErrorStack  * aErrorStack,
                                sclEnv         * aEnv )
{
    scpCursor sCursor;
    stlInt16  sResultType = CML_COMMAND_SETPOS;

    STL_TRY( scpBeginWriting( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS );

    SCP_WRITE_VAR_INT16( aHandle, &sCursor, &sResultType, aEnv );

    STL_TRY( scpWriteErrorResult( aHandle,
                                  &sCursor,
                                  aErrorStack,
                                  aEnv ) == STL_SUCCESS );

    SCP_WRITE_STATEMENT_ID( aHandle, &sCursor, &aStatementId, aEnv );

    STL_TRY( scpEndWriting( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
