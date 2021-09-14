/*******************************************************************************
 * knlTraceLogger.h
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


#ifndef _KNLTRACELOGGER_H_
#define _KNLTRACELOGGER_H_ 1

/**
 * @file knlTraceLogger.h
 * @brief Kernel Layer Trace Logger Routines
 */

/**
 * @defgroup knlTraceLogger Trace Logger
 * @ingroup knExternal
 * @{
 */

stlStatus knlCreateLogger( knlTraceLogger * aLogger,
                           stlChar        * aName,
                           stlUInt64        aMaxSize,
                           knlLogLevel      aCallstackLevel,
                           knlEnv         * aEnv );

stlStatus knlDestroyLogger( knlTraceLogger * aLogger,
                            knlEnv         * aEnv );

stlStatus knlCreateProcessLogger( knlTraceLogger  * aLogger,
                                  stlChar         * aPrefix,
                                  knlEnv          * aEnv );

stlStatus knlDestroyProcessLogger( knlTraceLogger * aLogger,
                                   knlEnv         * aEnv );

stlStatus knlLogMsg( knlTraceLogger * aLogger,
                     knlEnv         * aEnv,
                     knlLogLevel      aLevel,
                     const stlChar  * aFormat,
                     ... );

stlStatus knlLogMsgWithoutTimestamp( knlTraceLogger * aLogger,
                                     knlEnv         * aEnv,
                                     const stlChar  * aFormat,
                                     ... );

stlStatus knlSimpleLogMsgWithoutTimestamp( knlTraceLogger   * aLogger,
                                           stlChar          * aLogMsg,
                                           knlEnv           * aEnv );

stlStatus knlLogMsgUnsafe( knlTraceLogger * aLogger,
                           knlEnv         * aEnv,
                           knlLogLevel      aLevel,
                           const stlChar  * aFormat,
                           ... );

stlStatus knlVarLogMsg( knlTraceLogger * aLogger,
                        knlEnv         * aEnv,
                        knlLogLevel      aLevel,
                        stlInt32         aMsgSize,
                        const stlChar * aFormat,
                        stlVarList      aVarArgList );

stlStatus knlLogErrorCallStack( knlTraceLogger * aLogger,
                                knlEnv         * aEnv );

stlStatus knlLogCallStack( knlTraceLogger * aLogger,
                           knlEnv         * aEnv );

stlStatus knlLogCallStackUnsafe( knlTraceLogger * aLogger,
                                 knlEnv         * aEnv );

stlStatus knlLogErrorStack( knlTraceLogger * aLogger,
                            stlErrorStack  * aErrorStack,
                            knlEnv         * aEnv );

stlStatus knlLogErrorStackUnsafe( knlTraceLogger * aLogger,
                                  stlErrorStack  * aErrorStack,
                                  knlEnv         * aEnv );

knlTraceLogger * knlGetSystemTraceLogger();

/** @} */
    
#endif /* _KNLTRACELOGGER_H_ */
