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
#include <knl.h>
#include <sclDef.h>
#include <scDef.h>
#include <scpProtocol.h>
#include <sclGeneral.h>
#include <sccCommunication.h>

/**
 * @file sclXaRecover.c
 * @brief Communication Layer XA Recover Protocol Routines
 */


/**
 * @brief XA Recover Command Protocol을 읽는다.
 * @param[in]  aHandle        Communication Handle
 * @param[out] aCount         XID Array Size
 * @param[out] aRmId          Resource Manager Identifier
 * @param[out] aFlags         XA Flags
 * @param[in]  aEnv           env
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
stlStatus sclReadXaRecoverCommand( sclHandle      * aHandle,
                                   stlInt64       * aCount,
                                   stlInt32       * aRmId,
                                   stlInt64       * aFlags,
                                   sclEnv         * aEnv )
{
    scpCursor   sCursor;
    stlInt16    sCommandType;
    stlInt8     sControl;

    STL_TRY( scpBeginReading( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS );

    SCP_READ_VAR_INT16( aHandle, &sCursor, &sCommandType, aEnv );
    SCP_READ_INT8( aHandle, &sCursor, &sControl, aEnv );

    STL_DASSERT( SCL_IS_HEAD(sControl) == STL_TRUE );
    STL_DASSERT( SCL_IS_TAIL(sControl) == STL_TRUE );
    STL_DASSERT( sCommandType == CML_COMMAND_XA_RECOVER );
    
    SCP_READ_INT64_ENDIAN( aHandle, &sCursor, aCount, aEnv );
    SCP_READ_INT32_ENDIAN( aHandle, &sCursor, aRmId, aEnv );
    SCP_READ_INT64_ENDIAN( aHandle, &sCursor, aFlags, aEnv );

    STL_TRY( scpEndReading( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Xa Recover Result Protocol을 기록한다.
 * @param[in] aHandle            communication handle
 * @param[in] aCount             XID Array Size
 * @param[in] aXids              XID Array
 * @param[in] aXaError           XA Error
 * @param[in] aErrorStack        기록할 Error Stack
 * @param[in] aEnv               env
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
stlStatus sclWriteXaRecoverResult( sclHandle      * aHandle,
                                   stlInt64         aCount,
                                   stlXid         * aXids,
                                   stlInt32         aXaError,
                                   stlErrorStack  * aErrorStack,
                                   sclEnv         * aEnv )
{
    scpCursor  sCursor;
    stlInt16   sResultType = CML_COMMAND_XA_RECOVER;
    stlInt64   i;

    STL_TRY( scpBeginWriting( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS );

    SCP_WRITE_VAR_INT16( aHandle, &sCursor, &sResultType, aEnv );
    
    STL_TRY( scpWriteXaErrorResult( aHandle,
                                    &sCursor,
                                    aXaError,
                                    aErrorStack,
                                    aEnv ) == STL_SUCCESS );

    SCP_WRITE_INT64( aHandle, &sCursor, &aCount, aEnv );

    for( i = 0; i < aCount; i++ )
    {
        SCP_WRITE_INT64( aHandle, &sCursor, &aXids[i].mFormatId, aEnv );
        SCP_WRITE_INT64( aHandle, &sCursor, &aXids[i].mTransLength, aEnv );
        SCP_WRITE_INT64( aHandle, &sCursor, &aXids[i].mBranchLength, aEnv );
        SCP_WRITE_N_VAR( aHandle, &sCursor, aXids[i].mData, aXids[i].mTransLength + aXids[i].mBranchLength, aEnv );
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
