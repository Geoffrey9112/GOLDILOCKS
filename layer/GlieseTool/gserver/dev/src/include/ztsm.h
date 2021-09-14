/*******************************************************************************
 * ztsm.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztsm.h 12157 2014-04-23 03:50:31Z lym999 $
 *
 * NOTES
 *
 *
 ******************************************************************************/


#ifndef _ZTSM_H_
#define _ZTSM_H_ 1

/**
 * @file ztsm.h
 * @brief Gliese Server main routine
 */

extern stlGetOptOption gZtsmOptOption[];

/**
 * @brief Gliese Server Main functions
 */
stlStatus ztsmDaemonize( stlBool         aDaemonize,
                         stlErrorStack * aErrorStack );

stlStatus ztsmDedicate( stlContext    * aContext,
                       stlInt64        aBufferSize,
                       stlErrorStack * aErrorStack );

stlStatus ztsmStartup( stlBool         aDoDaemonize,
                       cmlSessionType  aSessionMode,
                       stlErrorStack * aErrorStack );

stlStatus ztsmShutdown( stlErrorStack * aErrorStack );

stlStatus ztsmWarmup( stlErrorStack * aErrorStack,
                      cmlSessionType  aSessionMode );

stlStatus ztsmCooldown( stlErrorStack * aErrorStack );

void ztsmFatalHandler( const stlChar * aCauseString,
                       void          * aSigInfo,
                       void          * aContext );

/** @} */

/** @} */

#endif /* _ZTSM_H_ */
