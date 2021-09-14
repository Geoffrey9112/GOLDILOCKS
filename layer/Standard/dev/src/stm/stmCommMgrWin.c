/*******************************************************************************
 * stmCommMgrWin.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stmCommMgrUnix.c 13562 2014-09-12 08:31:43Z mkkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/

#include <stc.h>
#include <ste.h>
#include <stn.h>
#include <stl.h>
#include <stm.h>
#include <stlError.h>

stlStatus stmFinalizeContext( stlContext     * aContext,
                              stlErrorStack  * aErrorStack )
{
    stlInt32  sResult;
    stlChar   sMsg[STM_MSG_LEN];

    sResult = stnCloseSocket( STL_SOCKET_IN_CONTEXT(*aContext), aErrorStack );
    if(sResult != STL_SUCCESS)
    {
        STL_TRY(stlGetLastSystemErrorCode(aErrorStack) == WSAENOTSOCK);
    }

    return STL_SUCCESS;

    STL_FINISH;

    stlSnprintf( sMsg, STM_MSG_LEN,
                 "fail to finalize communication context" );
    
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  STL_ERRCODE_FINALIZE_CONTEXT,
                  sMsg,
                  aErrorStack );

    return STL_FAILURE;
}

