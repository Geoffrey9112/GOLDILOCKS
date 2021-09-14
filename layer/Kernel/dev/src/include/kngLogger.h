/*******************************************************************************
 * kngLogger.h
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


#ifndef _KNGLOGGER_H_
#define _KNGLOGGER_H_ 1

/**
 * @file kngLogger.h
 * @brief Kernel Layer Logger Internal Routines
 */

/**
 * @defgroup kngLogger Logger
 * @ingroup knInternal
 * @internal
 * @{
 */

stlStatus kngPrintTimestamp( stlFile     * aFile,
                             knlLogLevel   aLevel,
                             knlEnv      * aEnv);

stlStatus kngPrintHeader( stlFile * aFile,
                          knlEnv  * aEnv);

stlStatus kngPrintStartMsg( knlTraceLogger * aLogger,
                            knlEnv         * aEnv );

stlStatus kngPrintCallStack( stlFile * aFile,
                             knlEnv  * aEnv );

stlStatus kngCreateDefaultFile( knlTraceLogger * aLogger,
                                knlEnv         * aEnv );

stlStatus kngCreateNewFile( knlTraceLogger * aLogger,
                            knlEnv         * aEnv );

stlStatus kngOpenDefaultFile( knlTraceLogger * aLogger,
                                  stlSize          aSize,
                                  knlEnv         * aEnv );

stlStatus kngCloseDefaultFile( knlTraceLogger * aLogger,
                                   knlEnv         * aEnv );

stlStatus kngAddLogMsg( knlTraceLogger * aLogger,
                        knlLogLevel      aLevel,
                        stlChar        * aMsg,
                        stlInt32         aLength,
                        knlEnv         * aEnv );

stlStatus kngAddLogMsgWithoutTimestamp( knlTraceLogger  * aLogger,
                                        stlChar         * aMsg,
                                        stlInt32          aLength,
                                        knlEnv          * aEnv );

stlStatus kngAddLogError( knlTraceLogger * aLogger,
                          stlErrorData   * aErrorData,
                          knlEnv         * aEnv );

stlStatus kngAddErrorCallStack( knlTraceLogger  * aLogger,
                                stlBool           aFrameInErrorStack,
                                knlEnv          * aEnv );

/** @} */
    
#endif /* _KNGLOGGER_H_ */
