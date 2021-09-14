/*******************************************************************************
 * cmlXaRecover.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: cmlXaRecover.c 6676 2012-12-13 08:39:46Z egon $
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
 * @file cmlXaRecover.c
 * @brief Communication Layer XA Recover Protocol Routines
 */

/**
 * @brief XA Recover Command Protocol을 기록한다.
 * @param[in] aHandle        Communication Handle
 * @param[in] aCount         XID Array Size
 * @param[in] aRmId          Resource Manager Identifier
 * @param[in] aFlags         XA Flags
 * @param[in] aErrorStack    Error Stack Pointer
 * 
 * @par protocol sequence:
 * @code
 * 
 * - command (t)ype    : command type         var_int bytes
 * - (c)ount           : XID array size       8 bytes
 * - (r)m id           : resource manager id  4 byte
 * - (f)lags           : xa flags             8 byte
 *
 * field sequence : t-c-r-f
 * 
 * @endcode
 */
stlStatus cmlWriteXaRecoverCommand( cmlHandle      * aHandle,
                                    stlInt64         aCount,
                                    stlInt32         aRmId,
                                    stlInt64         aFlags,
                                    stlErrorStack  * aErrorStack )
{
    cmpCursor   sCursor;
    stlInt16    sCommandType = CML_COMMAND_XA_RECOVER;
    stlInt8     sControl = CML_CONTROL_HEAD | CML_CONTROL_TAIL;

    STL_TRY( cmpBeginWriting( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_WRITE_VAR_INT16( aHandle, &sCursor, &sCommandType, aErrorStack );
    CMP_WRITE_INT8( aHandle, &sCursor, &sControl, aErrorStack );
    CMP_WRITE_INT64_ENDIAN( aHandle, &sCursor, &aCount, aErrorStack );
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
 * @param[out] aCount             XID Array Size
 * @param[out] aXids              XID Array
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
 * - x(i)d count               : xid count               8 bytes
 * - xid f(o)rmat              : xid format              8 byte
 * - (x)id trans               : xid trans length        8 byte
 * - xid (b)ranch              : xid branch length       8 byte
 * - xid (d)ata                : xid data                128 byte
 *
 * field sequence : if c == 0 then t-r-e-i-[o-x-b-d]^i
 *                  else t-r-e-c-[s-a-m]^c
 *                  
 * @endcode
 */
stlStatus cmlReadXaRecoverResult( cmlHandle      * aHandle,
                                  stlInt64       * aCount,
                                  stlXid         * aXids,
                                  stlInt32       * aXaError,
                                  stlErrorStack  * aErrorStack )
{
    cmpCursor sCursor;
    stlInt16  sResultType;
    stlInt64  i;
    stlInt32  sLen;

    STL_TRY( cmpBeginReading( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_READ_VAR_INT16( aHandle, &sCursor, &sResultType, aErrorStack );
    STL_DASSERT( sResultType == CML_COMMAND_XA_RECOVER );

    STL_TRY( cmpReadXaErrorResult( aHandle,
                                   NULL,    /* aIgnored */
                                   &sCursor,
                                   aXaError,
                                   aErrorStack )
             == STL_SUCCESS );

    CMP_READ_INT64_ENDIAN( aHandle, &sCursor, aCount, aErrorStack );

    for( i = 0; i < *aCount; i++ )
    {
        CMP_READ_INT64_ENDIAN( aHandle, &sCursor, &aXids[i].mFormatId, aErrorStack );
        CMP_READ_INT64_ENDIAN( aHandle, &sCursor, &aXids[i].mTransLength, aErrorStack );
        CMP_READ_INT64_ENDIAN( aHandle, &sCursor, &aXids[i].mBranchLength, aErrorStack );
        CMP_READ_N_VAR_ENDIAN( aHandle, &sCursor, aXids[i].mData, sLen, aErrorStack );

        STL_DASSERT( sLen == aXids[i].mTransLength + aXids[i].mBranchLength );
    }

    STL_TRY( cmpEndReading( aHandle,
                            &sCursor,
                            aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
