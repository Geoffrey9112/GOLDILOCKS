/*******************************************************************************
 * cmlXaClose.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: cmlXaClose.c 6676 2012-12-13 08:39:46Z egon $
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
 * @file sclXaClose.c
 * @brief Communication Layer XA Close Protocol Routines
 */


/**
 * @brief XA Close Command Protocol을 읽는다.
 * @param[in]  aHandle        Communication Handle
 * @param[in]  aEnv           env
 * 
 * @par protocol sequence:
 * @code
 * 
 * - command (t)ype    : command type         var_int bytes
 *
 * field sequence : t
 * 
 * @endcode
 */
stlStatus sclReadXaCloseCommand( sclHandle      * aHandle,
                                 sclEnv         * aEnv )
{
    scpCursor sCursor;
    stlInt16  sCommandType;
    stlInt8   sControl;

    STL_TRY( scpBeginReading( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS );

    SCP_READ_VAR_INT16( aHandle, &sCursor, &sCommandType, aEnv );
    SCP_READ_INT8( aHandle, &sCursor, &sControl, aEnv );

    STL_DASSERT( SCL_IS_HEAD(sControl) == STL_TRUE );
    STL_DASSERT( SCL_IS_TAIL(sControl) == STL_TRUE );
    STL_DASSERT( sCommandType == CML_COMMAND_XA_CLOSE );

    STL_TRY( scpEndReading( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
