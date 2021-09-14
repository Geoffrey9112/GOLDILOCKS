/*******************************************************************************
 * ztis.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztis.h 12157 2014-04-23 03:50:31Z lym999 $
 *
 * NOTES
 *
 *
 ******************************************************************************/


#ifndef _ZTIS_H_
#define _ZTIS_H_ 1

/**
 * @file ztis.h
 * @brief Gliese send thread
 */

/**
 * @defgroup ztiSend dispatcher send routines
 * @{
 */

void * STL_THREAD_FUNC ztisSendResponse( stlThread *aThread, void *aArg );

stlStatus ztisProcessResponse( sllHandle         * aHandle,
                               sclQueueCommand     aCommand,
                               ztiDispatcher     * aDispatcher,
                               stlInt64            aCmUnitCount,
                               sllEnv            * aEnv );

stlStatus ztisRetryResponse( ztiDispatcher     * aDispatcher,
                             sllEnv            * aEnv );

stlStatus ztisResponse( ztiDispatcher     * aDispatcher,
                        ztiContext        * aContext,
                        sllEnv            * aEnv );
/** @} */


#endif /* _ZTIS_H_ */
