/*******************************************************************************
 * cmlXa.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: cmlSetConnAttr.c 6676 2012-12-13 08:39:46Z egon $
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
 * @file cmlXa.c
 * @brief Communication Layer XA Protocol Routines
 */

/**
 * @addtogroup cmlProtocol
 * @{
 */

/**
 * @brief XA Command Protocol을 기록한다.
 * @param[in] aHandle        Communication Handle
 * @param[in] aCommandType   Command Type
 * @param[in] aXid           XID pointer
 * @param[in] aRmId          Resource Manager Identifier
 * @param[in] aFlags         XA Flags
 * @param[in] aErrorStack    Error Stack Pointer
 * 
 * @par protocol sequence:
 * @code
 * 
 * - command (t)ype    : command type         var_int bytes
 * - xid for(m)at      : xid format           8 byte
 * - (x)id trans       : xid trans length     8 byte
 * - xid (b)ranch      : xid branch length    8 byte
 * - xid (d)ata        : xid data             128 byte
 * - (r)m id           : resource manager id  4 byte
 * - (f)lags           : xa flags             8 byte
 *
 * field sequence : t-m-x-b-d-r-f
 * 
 * @endcode
 */
stlStatus cmlWriteXaCommand( cmlHandle      * aHandle,
                             stlInt16         aCommandType,
                             stlXid         * aXid,
                             stlInt32         aRmId,
                             stlInt64         aFlags,
                             stlErrorStack  * aErrorStack )
{
    cmpCursor   sCursor;
    stlInt8     sControl = CML_CONTROL_HEAD | CML_CONTROL_TAIL;

    STL_TRY( cmpBeginWriting( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_WRITE_VAR_INT16( aHandle, &sCursor, &aCommandType, aErrorStack );
    CMP_WRITE_INT8( aHandle, &sCursor, &sControl, aErrorStack );
    
    CMP_WRITE_INT64_ENDIAN( aHandle, &sCursor, &aXid->mFormatId, aErrorStack );
    CMP_WRITE_INT64_ENDIAN( aHandle, &sCursor, &aXid->mTransLength, aErrorStack );
    CMP_WRITE_INT64_ENDIAN( aHandle, &sCursor, &aXid->mBranchLength, aErrorStack );
    CMP_WRITE_N_VAR_ENDIAN( aHandle, &sCursor, aXid->mData, aXid->mTransLength + aXid->mBranchLength, aErrorStack );
    CMP_WRITE_INT32_ENDIAN( aHandle, &sCursor, &aRmId, aErrorStack );
    CMP_WRITE_INT64_ENDIAN( aHandle, &sCursor, &aFlags, aErrorStack );

    STL_TRY( cmpEndWriting( aHandle,
                            &sCursor,
                            aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief XA Result Protocol을 읽는다.
 * @param[in]  aHandle            communication handle
 * @param[out] aResultType        Result type
 * @param[out] aXaError           XA Error
 * @param[in]  aErrorStack        error stack pointer
 * 
 * @par protocol sequence:
 * @code
 * 
 * - result (t)ype             : result type             var_int bytes
 * - e(r)ror level             : error level             1 byte
 * - xa (e)rror                : xa error                4 byte
 * - error (c)ount             : error count             1 byte
 * - (s)ql state               : sql state               4 bytes
 * - n(a)tive error            : native error            4 bytes
 * - (m)essage text            : message text            variable
 *
 * field sequence : if c == 0 then t-r-e
 *                  else t-r-e-c-[s-a-m]^c
 * 
 * @endcode
 */
stlStatus cmlReadXaResult( cmlHandle      * aHandle,
                           stlInt16       * aResultType,
                           stlInt32       * aXaError,
                           stlErrorStack  * aErrorStack )
{
    cmpCursor sCursor;

    STL_TRY( cmpBeginReading( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_READ_VAR_INT16( aHandle, &sCursor, aResultType, aErrorStack );

    STL_TRY( cmpReadXaErrorResult( aHandle,
                                   NULL,    /* aIgnored */
                                   &sCursor,
                                   aXaError,
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
