/*******************************************************************************
 * ztlm.h
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


#ifndef _ZTLM_H_
#define _ZTLM_H_ 1

/**
 * @file ztlm.h
 * @brief Gliese Listener Routines
 */

#include <stl.h>
/**
 * @defgroup ztlm Gliese Listener Main Routines
 * @{
 */

/*
 * CONF
 */
stlStatus ztlmGetConfFilePath( stlChar       * aPath,
                               stlSize         aSize,
                               stlErrorStack * aErrorStack );

stlStatus ztlmGetConf( stlErrorStack * aErrorStack );

stlStatus ztlmSetNetworkFilter( stlErrorStack * aErrorStack );

stlStatus ztlmGetValueFromEnv( stlChar       * aName,
                               stlChar       * aValue,
                               stlSize         aValueLen,
                               stlBool       * aIsFound,
                               stlErrorStack * aErrorStack );

stlStatus ztlmCheckClientIpAddress( ztlcContext   * aContext,
                                    stlBool       * aIsAccess,
                                    stlErrorStack * aErrorStack );

stlStatus ztlmSetValueIni( stlIniFormat  * aFormat,
                           stlChar       * aValue,
                           stlErrorStack * aErrorStack );

stlStatus ztlmParseIni( stlChar       * aFileName,
                        stlIniFormat  * aIniFormat,
                        stlErrorStack * aErrorStack );
/*
 * Main
 */


void ztlmPrintErrorStack( stlErrorStack * aErrorStack );

stlStatus ztlStartDispatcher( stlInt32        aIdx,
                              stlErrorStack * aErrorStack);

stlStatus ztlStartSharedServer( stlInt32        aIdx,
                                stlErrorStack * aErrorStack );

stlInt32 ztlmPrintf( const stlChar *aFormat, ... );
stlStatus ztlmMakeSynchronizeFilePath( stlChar       *aPath,
                                       stlInt64       aPort,
                                       stlErrorStack *aErrorStack );

stlStatus ztlmInitSynchronizeFile( stlChar       *aPath,
                                   stlErrorStack *aErrorStack );
stlStatus ztlmWaitChildReady( stlChar       *aPath,
                              stlErrorStack *aErrorStack );
stlStatus ztlmMarkChildReady( stlChar       *aPath,
                              stlErrorStack *aErrorStack );

/** @} */

/** @} */

#endif /* _ZTLM_H_ */
