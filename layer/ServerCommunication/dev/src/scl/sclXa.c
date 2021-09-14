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
#include <knl.h>
#include <sclDef.h>
#include <scDef.h>
#include <scpProtocol.h>
#include <sclGeneral.h>
#include <sccCommunication.h>

/**
 * @file sclXa.c
 * @brief Communication Layer XA Protocol Routines
 */

/**
 * @addtogroup cmlProtocol
 * @{
 */


/**
 * @brief XA Command Protocol을 읽는다.
 * @param[in]  aHandle        Communication Handle
 * @param[out] aCommandType   Command Type
 * @param[out] aXid           XID pointer
 * @param[out] aRmId          Resource Manager Identifier
 * @param[out] aFlags         XA Flags
 * @param[in]  aEnv           env
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
stlStatus sclReadXaCommand( sclHandle      * aHandle,
                            stlInt16       * aCommandType,
                            stlXid         * aXid,
                            stlInt32       * aRmId,
                            stlInt64       * aFlags,
                            sclEnv         * aEnv )
{
    scpCursor   sCursor;
    stlInt32    sLen;
    stlInt8     sControl;

    STL_TRY( scpBeginReading( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS );

    SCP_READ_VAR_INT16( aHandle, &sCursor, aCommandType, aEnv );
    SCP_READ_INT8( aHandle, &sCursor, &sControl, aEnv );
    SCP_READ_INT64( aHandle, &sCursor, &aXid->mFormatId, aEnv );
    SCP_READ_INT64( aHandle, &sCursor, &aXid->mTransLength, aEnv );
    SCP_READ_INT64( aHandle, &sCursor, &aXid->mBranchLength, aEnv );
    SCP_READ_N_VAR( aHandle, &sCursor, aXid->mData, sLen, aEnv );
    SCP_READ_INT32( aHandle, &sCursor, aRmId, aEnv );
    SCP_READ_INT64( aHandle, &sCursor, aFlags, aEnv );

    STL_DASSERT( SCL_IS_HEAD(sControl) == STL_TRUE );
    STL_DASSERT( SCL_IS_TAIL(sControl) == STL_TRUE );
    STL_DASSERT( sLen == aXid->mTransLength + aXid->mBranchLength );

    STL_TRY( scpEndReading( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Xa Result Protocol을 기록한다.
 * @param[in] aHandle            communication handle
 * @param[in] aCommandType       command type
 * @param[in] aXaError           XA Error
 * @param[in] aErrorStack       기록할 Error Stack
 * @param[in] aEnv              env
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
stlStatus sclWriteXaResult( sclHandle      * aHandle,
                            stlInt16         aCommandType,
                            stlInt32         aXaError,
                            stlErrorStack  * aErrorStack,
                            sclEnv         * aEnv )
{
    scpCursor  sCursor;

    STL_TRY( scpBeginWriting( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS );

    SCP_WRITE_VAR_INT16( aHandle, &sCursor, &aCommandType, aEnv );

    STL_TRY( scpWriteXaErrorResult( aHandle,
                                    &sCursor,
                                    aXaError,
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
