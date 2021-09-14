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
#include <knl.h>
#include <sclDef.h>
#include <scDef.h>
#include <scpProtocol.h>
#include <sclGeneral.h>
#include <sccCommunication.h>

/**
 * @file sclTargetType.c
 * @brief Communication Layer TargetType Protocol Routines
 */

/**
 * @addtogroup cmlProtocol
 * @{
 */


/**
 * @brief TargetType Command Protocol을 읽는다.
 * @param[in]  aHandle       communication handle
 * @param[out] aControl      Protocol Control
 * @param[out] aStatementId  statement identifier
 * @param[in]  aEnv          env
 * 
 * @par protocol sequence:
 * @code
 * 
 * - command (t)ype   : command type            var_int bytes
 * - (p)eriod         : protocol period         1 byte
 * - statement (i)d   : statement identifier    var_int byte
 *
 * field sequence : t-p-s
 * 
 * @endcode
 */
stlStatus sclReadTargetTypeCommand( sclHandle      * aHandle,
                                    stlInt8        * aControl,
                                    stlInt64       * aStatementId,
                                    sclEnv         * aEnv )
{
    stlInt16    sResultType;
    scpCursor   sCursor;

    STL_TRY( scpBeginReading( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS );

    SCP_READ_VAR_INT16( aHandle, &sCursor, &sResultType, aEnv );
    SCP_READ_INT8( aHandle, &sCursor, aControl, aEnv );
    SCP_READ_STATEMENT_ID( aHandle, &sCursor, aStatementId, aEnv );

    STL_DASSERT( sResultType == CML_COMMAND_TARGETTYPE );
    
    STL_TRY( scpEndReading( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief TargetType Result Protocol을 기록한다.
 * @param[in] aHandle           communication handle
 * @param[in] aChanged          이전 target type 전송 후 변경 여부
 * @param[in] aTargetBlockList  target block list
 * @param[in] aCallback         target type callback function
 * @param[in] aErrorStack       기록할 Error Stack
 * @param[in] aEnv              env
 * 
 * @par protocol sequence:
 * @code
 * 
 * - result (t)ype         : result type         var_int bytes
 * - e(r)ror level         : error level         1 byte
 * - c(h)anged             : changed flag        1 byte
 * - target c(o)unt        : target count        var_int bytes
 * - tar(g)et type         : target type         1 byte
 * - arg(u)ment1           : argument1           var_int bytes
 * - argume(n)t1           : argument2           var_int bytes
 * - (s)tring length unit  : string length unit  1 byte
 * - interval in(d)icator  : interval indicator  1 byte
 * - error (c)ount         : error count         1 byte
 * - (s)ql state           : sql state           4 bytes
 * - n(a)tive error        : native error        4 bytes
 * - (m)essage text        : message text        variable
 *
 * field sequence : if r == 0 then t-r-h-o-[g-u-n-s-d]^o
 *                  else if r == 1 then t-r-c-[s-a-m]^c-h-o-[g-u-n-s-d]^o
 *                  else t-r-c-[s-a-m]^c
 * 
 * @endcode
 */
stlStatus sclWriteTargetTypeResult( sclHandle                * aHandle,
                                    stlBool                    aChanged,
                                    dtlValueBlockList        * aTargetBlockList,
                                    sclGetTargetTypeCallback   aCallback,
                                    stlErrorStack            * aErrorStack,
                                    sclEnv                   * aEnv )
{
    scpCursor           sCursor;
    stlInt16            sResultType = CML_COMMAND_TARGETTYPE;
    stlInt32            sTargetCount = 0;
    dtlValueBlockList * sTarget;
    stlInt8             sDataType;
    stlInt8             sStringLengthUnit;
    stlInt8             sIntervalIndicator;
    stlInt64            sArgNum1;
    stlInt64            sArgNum2;

    sTarget = aTargetBlockList;
    while( sTarget != NULL )
    {
        sTargetCount++;
        sTarget = sTarget->mNext;
    }
    
    STL_TRY( scpBeginWriting( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS );

    SCP_WRITE_VAR_INT16( aHandle, &sCursor, &sResultType, aEnv );

    STL_TRY( scpWriteErrorResult( aHandle,
                                  &sCursor,
                                  aErrorStack,
                                  aEnv ) == STL_SUCCESS );

    SCP_WRITE_INT8( aHandle, &sCursor, &aChanged, aEnv );
    SCP_WRITE_VAR_INT32( aHandle, &sCursor, &sTargetCount, aEnv );

    sTarget = aTargetBlockList;

    while( sTarget != NULL )
    {
        STL_TRY( aCallback( aHandle,
                            sTarget,
                            &sDataType,
                            &sStringLengthUnit,
                            &sIntervalIndicator,
                            &sArgNum1,
                            &sArgNum2,
                            aEnv )
                 == STL_SUCCESS );
                            
        SCP_WRITE_INT8( aHandle, &sCursor, &sDataType, aEnv );
        SCP_WRITE_VAR_INT64( aHandle, &sCursor, &sArgNum1, aEnv );
        SCP_WRITE_VAR_INT64( aHandle, &sCursor, &sArgNum2, aEnv );
        SCP_WRITE_INT8( aHandle, &sCursor, &sStringLengthUnit, aEnv );
        SCP_WRITE_INT8( aHandle, &sCursor, &sIntervalIndicator, aEnv );

        sTarget = sTarget->mNext;
    }

    STL_TRY( scpEndWriting( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} */
