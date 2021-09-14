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
#include <cmlDef.h>
#include <cmDef.h>
#include <cmlProtocol.h>
#include <cmlGeneral.h>
#include <cmpCursor.h>
#include <cmpProtocol.h>
#include <cmgMemory.h>

/**
 * @file cmlXaClose.c
 * @brief Communication Layer XA Close Protocol Routines
 */

/**
 * @brief XA Close Command Protocol을 기록한다.
 * @param[in] aHandle        Communication Handle
 * @param[in] aErrorStack    Error Stack Pointer
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
stlStatus cmlWriteXaCloseCommand( cmlHandle      * aHandle,
                                  stlErrorStack  * aErrorStack )
{
    cmpCursor sCursor;
    stlInt16  sCommandType = CML_COMMAND_XA_CLOSE;
    stlInt8   sControl = CML_CONTROL_HEAD | CML_CONTROL_TAIL;

    STL_TRY( cmpBeginWriting( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_WRITE_VAR_INT16( aHandle, &sCursor, &sCommandType, aErrorStack );
    CMP_WRITE_INT8( aHandle, &sCursor, &sControl, aErrorStack );
   
    STL_TRY( cmpEndWriting( aHandle,
                            &sCursor,
                            aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
