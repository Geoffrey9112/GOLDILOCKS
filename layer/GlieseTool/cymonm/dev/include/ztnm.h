/*******************************************************************************
 * ztnmGeneral.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZTNM_H_
#define _ZTNM_H_ 1



/**
 * @file ztnGeneral.h
 * @brief GlieseTool Cyclone Monitor General Routines
 */
extern ztnLibFunc               gZtnLibFunc;

void      ztnmPrintErrorstack( stlErrorStack * aErrorStack );

void      ztnmWriteLastError( stlErrorStack * aErrorStack );

void      ztnmMonitorSignalFunc( stlInt32 aSignalNo );

stlStatus ztnmInitializeEnv( stlErrorStack * aErrorStack );

stlStatus ztnmFinalizeEnv( stlErrorStack * aErrorStack );

stlStatus ztnmStart( stlErrorStack * aErrorStack );

stlStatus ztnmStop( stlErrorStack * aErrorStack );

stlStatus ztnmSetHomeDir( stlErrorStack * aErrorStack );

stlStatus ztnmGetConfigureFileName( stlChar       * aFileName,
                                    stlErrorStack * aErrorStack );

stlStatus ztnmProcessAliveCheck( stlChar       * aFileName,
                                 stlErrorStack * aErrorStack );

stlStatus ztnmGetPidFileName( stlChar       * aFileName,
                              stlErrorStack * aErrorStack );

stlStatus ztnmSetConfigureFileName( const stlChar * aConfigureFile,
                                    stlErrorStack * aErrorStack );

stlStatus ztnmAddGroupItem( stlChar       * aGroupName,
                            stlErrorStack * aErrorStack );

stlStatus ztnmSetPort( stlChar       * aPort,
                       stlErrorStack * aErrorStack );

stlStatus ztnmSetDsnToGlobalArea( stlChar       * aDsn,
                                  stlErrorStack * aErrorStack );

stlStatus ztnmSetDsnToGroupArea( stlChar       * aDsn,
                                 stlErrorStack * aErrorStack );

stlChar  * ztnmGetDsn();

stlStatus ztnmSetHostIpToGlobalArea( stlChar       * aHostIp,
                                     stlErrorStack * aErrorStack );

stlStatus ztnmSetHostIpToGroupArea( stlChar       * aHostIp,
                                    stlErrorStack * aErrorStack );

stlChar  * ztnmGetHostIp();

stlStatus ztnmSetHostPortToGlobalArea( stlChar       * aPort,
                                       stlErrorStack * aErrorStack );

stlStatus ztnmSetHostPortToGroupArea( stlChar       * aPort,
                                      stlErrorStack * aErrorStack );

stlStatus ztnmGetHostPort( stlInt32      * aPort,
                           stlErrorStack * aErrorStack );

stlStatus ztnmSetMonitorCycle( const stlChar * aCycle,
                               stlErrorStack * aErrorStack );

stlStatus ztnmSetMonitorCycleMs( const stlChar * aCycleMs,
                                 stlErrorStack * aErrorStack );

stlStatus ztnmSetLibConfigToGlobalArea( stlChar       * aLibConfig,
                                        stlErrorStack * aErrorStack );

stlStatus ztnmGetLibConfig( stlChar       * aLibConfig,
                            stlInt32        aBuffSize,
                            stlErrorStack * aErrorStack );

stlStatus ztnmSetMonitorFileToGlobalArea( const stlChar * aMonitorFile,
                                          stlErrorStack * aErrorStack );

stlStatus ztnmGetMonitorFile( stlChar       * aMonitorFile,
                              stlInt32        aBuffSize,
                              stlErrorStack * aErrorStack );

stlStatus ztnmSetMonitorCycleMsToGlobalArea( const stlChar * aMonitorCycleMs,
                                             stlErrorStack * aErrorStack );

stlInt64  ztnmGetMonitorCycleMs();

stlStatus ztnmChangePath( stlChar           * aFilePath,
                          stlInt32            aBuffSize,
                          stlErrorStack     * aErrorStack );

stlStatus ztnmLogMsg( stlErrorStack  * aErrorStack,
                      const stlChar  * aFormat,
                      ... );

stlStatus ztnmAddLogMsg( stlChar        * aMsg,
                         stlInt32         aLength,
                         stlErrorStack  * aErrorStack );

stlStatus ztnmWriteTimeStamp( stlFile        * aFile,
                              stlErrorStack  * aErrorStack );

stlStatus ztnmGetIntervalInfo( stlInt16        aGroupId,
                               stlInt64        aRedoLogFileSeq,
                               stlInt32        aRedoLogBlockSeq,
                               stlInt64        aCaptureFileSeq,
                               stlInt32        aCaptureBlockSeq,
                               stlInt64      * aInterval,
                               stlInt64      * aIntervalSize,
                               stlErrorStack * aErrorStack );

stlStatus ztnmPrintStatus( stlErrorStack * aErrorStack );

/**
 * Monitor
 */
stlStatus ztnmDoMonitor( stlErrorStack * aErrorStack );

stlStatus ztnmGetCycloneMonitorInfo( stlErrorStack * aErrorStack );



/**
 * Communicate
 */

                          
/**
 * Protocol
 */

stlStatus ztnmInitProtocol( ztnGroupItem  * aGroup,
                            stlErrorStack * aErrorStack );

stlStatus ztnmMonitorProtocol( ztnGroupItem  * aGroup,
                               stlErrorStack * aErrorStack );

void ztnmLogMsgCallback( const char * aLog );

stlStatus ztnmOpenLibrary( stlDsoHandle   * aLibHandle,
                           stlErrorStack  * aErrorStack );

stlStatus ztnmCloseLibrary( stlDsoHandle    aLibHandle,
                            stlErrorStack * aErrorStack );

/** @} */

#endif /* _ZTNM_H_ */


