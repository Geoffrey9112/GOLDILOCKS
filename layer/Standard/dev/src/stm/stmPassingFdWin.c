/*******************************************************************************
 * stmPassingFdWin.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stmPassingFdUnix.c 13984 2014-12-03 11:24:50Z lym999 $
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

stlStatus stmRecvContext( stlContext     * aContext,
                          stlContext     * aNewContext,
                          stlChar        * aBuf,
                          stlSize        * aLen,
                          stlErrorStack  * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  STL_ERRCODE_NOTIMPL,
                  NULL,
                  aErrorStack );

    return STL_FAILURE;
}

stlStatus stmSendContext( stlContext     * aContext,
                          stlSockAddr    * aAddr,
                          stlContext     * aPassingContext,
                          const stlChar  * aBuf,
                          stlSize          aLen,
                          stlErrorStack  * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  STL_ERRCODE_NOTIMPL,
                  NULL,
                  aErrorStack );

    return STL_FAILURE;
}

stlStatus stmCreateContextPair( stlInt32          aDomain,
                                stlInt32          aType,
                                stlInt32          aProtocol,
                                stlContext        aContexts[2],
                                stlErrorStack    *aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  STL_ERRCODE_NOTIMPL,
                  NULL,
                  aErrorStack );

    return STL_FAILURE;
}

