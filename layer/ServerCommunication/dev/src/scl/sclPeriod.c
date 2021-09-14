/*******************************************************************************
 * cmlPeriod.c
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
 * @file sclPeriod.c
 * @brief Communication Layer Cancel Protocol Routines
 */

/**
 * @addtogroup cmlProtocol
 * @{
 */

/**
 * @brief Period 처리를 한다.
 * @param[in]  aHandle       communication handle
 * @param[in]  aEnv          env
 *
 * @par protocol sequence:
 * @code
 *
 * - command (t)ype      : command type            2 bytes
 * - (p)eriod            : protocol period         1 byte
 * - statement (i)d      : statement identifier    8 bytes
 * - row (n)umber        ; row number              8 bytes
 * - (o)peration type    ; operation type          2 bytes
 * 
 * field sequence : t-p-i-n-o
 *
 * @endcode
 */
stlStatus sclPeriodEnd( sclHandle      * aHandle,
                        sclEnv         * aEnv )
{
    /* 한 패킷 안에서만 유효한 cached statement id를 무효화 한다.*/
    aHandle->mProtocolSentence->mCachedStatementId = CML_STATEMENT_ID_UNDEFINED;

    STL_TRY( sccSendPacket( aHandle, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
