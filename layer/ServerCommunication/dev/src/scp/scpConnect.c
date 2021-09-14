/*******************************************************************************
 * scpConnect.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: scpConnect.c 6676 2012-12-13 08:39:46Z egon $
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
#include <sclProtocol.h>
#include <sclGeneral.h>
#include <scpProtocol.h>
#include <sccCommunication.h>

/**
 * @file scpConnect.c
 * @brief Communication Layer Connect Protocol Routines
 */

/**
 * @addtogroup scpProtocol
 * @{
 */


stlStatus scpReadConnectCommandV1( sclHandle       * aHandle,
                                   stlInt8         * aControl,
                                   stlChar         * aUserName,
                                   stlChar         * aEncryptedPassword,
                                   stlChar         * aOldPassword,
                                   stlChar         * aNewPassword,
                                   stlChar         * aProgramName,
                                   stlInt32        * aProcessId,
                                   stlInt16        * aCharacterset,
                                   stlInt32        * aTimezone,
                                   cmlDbcType      * aDbcType,
                                   sclEnv          * aEnv )
{
    stlInt16    sCommandType;
    scpCursor   sCursor;
    stlInt8     sInt8;
    stlInt32    sLength;

    STL_TRY( scpBeginReading( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS);

    SCP_READ_VAR_INT16( aHandle, &sCursor, &sCommandType, aEnv );
    SCP_READ_INT8( aHandle, &sCursor, aControl, aEnv );

    STL_DASSERT( sCommandType == CML_COMMAND_CONNECT );

    SCP_READ_N_VAR( aHandle, &sCursor, aUserName, sLength, aEnv );
    SCP_READ_N_VAR( aHandle, &sCursor, aEncryptedPassword, sLength, aEnv );
    SCP_READ_N_VAR( aHandle, &sCursor, aProgramName, sLength, aEnv );
    SCP_READ_INT32( aHandle, &sCursor, aProcessId, aEnv );
    SCP_READ_INT16( aHandle, &sCursor, aCharacterset, aEnv );
    SCP_READ_INT32( aHandle, &sCursor, aTimezone, aEnv );
    SCP_READ_INT8( aHandle, &sCursor, &sInt8, aEnv );
    *aDbcType = sInt8;

    STL_TRY( scpEndReading( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus scpReadConnectCommandV2( sclHandle       * aHandle,
                                   stlInt8         * aControl,
                                   stlChar         * aUserName,
                                   stlChar         * aEncryptedPassword,
                                   stlChar         * aOldPassword,
                                   stlChar         * aNewPassword,
                                   stlChar         * aProgramName,
                                   stlInt32        * aProcessId,
                                   stlInt16        * aCharacterset,
                                   stlInt32        * aTimezone,
                                   cmlDbcType      * aDbcType,
                                   sclEnv          * aEnv )
{
    stlInt16    sCommandType;
    scpCursor   sCursor;
    stlInt8     sInt8;
    stlInt32    sLength;

    STL_TRY( scpBeginReading( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS);

    SCP_READ_VAR_INT16( aHandle, &sCursor, &sCommandType, aEnv );
    SCP_READ_INT8( aHandle, &sCursor, aControl, aEnv );

    STL_DASSERT( sCommandType == CML_COMMAND_CONNECT );

    SCP_READ_N_VAR( aHandle, &sCursor, aUserName, sLength, aEnv );
    SCP_READ_N_VAR( aHandle, &sCursor, aEncryptedPassword, sLength, aEnv );
    SCP_READ_N_VAR( aHandle, &sCursor, aOldPassword, sLength, aEnv );
    SCP_READ_N_VAR( aHandle, &sCursor, aNewPassword, sLength, aEnv );
    SCP_READ_N_VAR( aHandle, &sCursor, aProgramName, sLength, aEnv );
    SCP_READ_INT32( aHandle, &sCursor, aProcessId, aEnv );
    SCP_READ_INT16( aHandle, &sCursor, aCharacterset, aEnv );
    SCP_READ_INT32( aHandle, &sCursor, aTimezone, aEnv );
    SCP_READ_INT8( aHandle, &sCursor, &sInt8, aEnv );
    *aDbcType = sInt8;

    STL_TRY( scpEndReading( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
