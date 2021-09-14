/*******************************************************************************
 * ztsd.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztsd.h 12157 2014-04-23 03:50:31Z lym999 $
 *
 * NOTES
 *
 *
 ******************************************************************************/


#ifndef _ZTSD_H_
#define _ZTSD_H_ 1

/**
 * @file ztsd.h
 * @brief Gliese Server dedicate routine
 */

stlStatus ztsdGetContext( stlContext    * aContext,
                          stlChar       * aToListenerPath,
                          stlUInt16     * aMajorVersion,
                          stlUInt16     * aMinorVersion,
                          stlUInt16     * aPatchVersion,
                          sllEnv        * aEnv );

stlStatus ztsdProcessDedicate( sllHandle             * aHandle,
                               sclMemoryManager      * aMemoryMgr,
                               sllProtocolSentence   * aProtocolSentence,
                               sllEnv                * aEnv );

stlStatus ztsdInitializeDedicate( sllHandle             * aHandle,
                                  sclMemoryManager      * aMemoryMgr,
                                  sllProtocolSentence   * aProtocolSentence,
                                  stlContext            * aContext,
                                  stlChar               * aToListenerPath,
                                  cmlCommandType        * aCommandType,
                                  stlErrorStack         * aErrorStack );

stlStatus ztsdFinalizeDedicate( sllHandle            * aHandle,
                                sclMemoryManager     * aMemoryMgr,
                                sllProtocolSentence  * aProtocolSentence,
                                stlContext           * aContext,
                                cmlCommandType         aCommandType,
                                stlErrorStack        * aErrorStack );

#endif /* _ZTSD_H_ */
