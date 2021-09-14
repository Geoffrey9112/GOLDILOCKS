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
#include <cmlDef.h>
#include <cmDef.h>
#include <cmlProtocol.h>
#include <cmlGeneral.h>
#include <cmpCursor.h>
#include <cmpProtocol.h>
#include <cmgMemory.h>

/**
 * @file cmlPeriod.c
 * @brief Communication Layer Cancel Protocol Routines
 */

/**
 * @addtogroup cmlProtocol
 * @{
 */

/**
 * @brief Period 처리를 한다.
 * @param[in]  aHandle       communication handle
 * @param[in]  aErrorStack   error stack pointer
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
stlStatus cmlPeriodEnd( cmlHandle      * aHandle,
                        stlErrorStack  * aErrorStack )
{
    /* 한 패킷 안에서만 유효한 cached statement id를 무효화 한다.*/
    aHandle->mProtocolSentence->mCachedStatementId = CML_STATEMENT_ID_UNDEFINED;

    return STL_SUCCESS;
}

/** @} */
