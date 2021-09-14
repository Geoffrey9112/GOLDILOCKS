/*******************************************************************************
 * stmCommMgrUnix.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
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
    stlInt16  sAf;

    sResult = stnCloseSocket( STL_SOCKET_IN_CONTEXT(*aContext), aErrorStack );
    if(sResult != STL_SUCCESS)
    {
        STL_TRY(errno == EBADF);
    }

    sAf = ((struct sockaddr*)(&aContext->mSocketAddr))->sa_family;

    if( sAf == STL_AF_UNIX )
    {
        if( stlRemoveFile( aContext->mSocketAddr.mSa.mUn.sun_path,
                           aErrorStack ) != STL_SUCCESS )
        {
            STL_TRY( stlGetLastErrorCode(aErrorStack) == STL_ERRCODE_NO_ENTRY );
            (void)stlPopError( aErrorStack );
        }
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

