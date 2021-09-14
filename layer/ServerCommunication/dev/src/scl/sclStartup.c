/*******************************************************************************
 * cmlStartup.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
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
 * @file sclStartup.c
 * @brief Communication Layer Startup Protocol Routines
 */

/**
 * @addtogroup cmlProtocol
 * @{
 */

/**
 * @brief Startup Command Protocol을 읽는다.
 * @param[in]  aHandle       Communication Handle
 * @param[out] aControl      Protocol Control
 * @param[in]  aEnv          env
 *
 * @par protocol sequence:
 * @code
 *
 * - command (t)ype : command type        var_int bytes
 * - (p)eriod       : protocol period     1 byte
 *
 * field sequence : t-p
 *
 * @endcode
 */
stlStatus sclReadStartupCommand( sclHandle       * aHandle,
                                 stlInt8         * aControl,
                                 sclEnv          * aEnv )
{
    stlInt16    sCommandType;
    scpCursor   sCursor;

    STL_TRY( scpBeginReading( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS);

    SCP_READ_VAR_INT16( aHandle, &sCursor, &sCommandType, aEnv );
    SCP_READ_INT8( aHandle, &sCursor, aControl, aEnv );

    STL_DASSERT( sCommandType == CML_COMMAND_STARTUP );

    STL_TRY( scpEndReading( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Startup Result Protocol을 기록한다.
 * @param[in] aHandle       Communication Handle
 * @param[in] aErrorStack   기록할 Error Stack
 * @param[in] aEnv          env
 *
 * @par protocol sequence:
 * @code
 *
 * - result (t)ype  : result type         var_int bytes
 * - e(r)ror level  : error level         1 byte
 * - error (c)ount  : error count         1 bytes
 * - (s)ql state    : sql state           4 bytes
 * - n(a)tive error : native error        4 bytes
 * - (m)essage text : message text        variable
 *
 * field sequence : if r == 0 then t-r
 *                  else if r == 1 then t-r-c-[s-a-m]^c
 *                  else t-r-c-[s-a-m]^c
 *
 * @endcode
 */
stlStatus sclWriteStartupResult( sclHandle       * aHandle,
                                 stlErrorStack   * aErrorStack,
                                 sclEnv          * aEnv )
{
    stlInt16    sResultType = CML_COMMAND_STARTUP;
    scpCursor   sCursor;

    STL_TRY( scpBeginWriting( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS);

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
