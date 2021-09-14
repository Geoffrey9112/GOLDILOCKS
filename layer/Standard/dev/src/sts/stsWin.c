/*******************************************************************************
 * stsWin.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stsUnix.c 3375 2012-01-26 11:42:41Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stc.h>
#include <stlDef.h>
#include <ste.h>
#include <stlError.h>

stlStatus stsGetCpuInfo( stlCpuInfo    * aCpuInfo,
                         stlErrorStack * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  STL_ERRCODE_NOTIMPL,
                  NULL,
                  aErrorStack );

    return STL_FAILURE;
}

stlStatus stsGetResourceLimit( stlResourceLimitType    aLimitType,
                               stlResourceLimit      * aLimit,
                               stlErrorStack         * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  STL_ERRCODE_NOTIMPL,
                  NULL,
                  aErrorStack );

    return STL_FAILURE;
}


stlStatus stsSetResourceLimit( stlResourceLimitType    aLimitType,
                               stlResourceLimit      * aLimit,
                               stlErrorStack         * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  STL_ERRCODE_NOTIMPL,
                  NULL,
                  aErrorStack );

    return STL_FAILURE;
}

