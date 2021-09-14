/*******************************************************************************
 * ztrm.h
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


#ifndef _ZTRM_H_
#define _ZTRM_H_ 1

/**
 * @file ztrm.h
 * @brief GlieseTool LogMirror Main Routines
 */


/**
 * @defgroup ztrm LogMirror Main Routines
 * @{
 */


void      ztrmPrintHelp();

void      ztrmMasterSignalFunc( stlInt32 aSignalNo );

void      ztrmSlaveSignalFunc( stlInt32 aSignalNo );

stlStatus ztrmProcessAliveCheck( stlChar       * aFileName,
                                 stlErrorStack * aErrorStack );

stlStatus ztrmStart( stlBool         aIsMaster,
                     stlBool         aDoReset,
                     stlErrorStack * aErrorStack );
                    
stlStatus ztrmStop( stlBool         aIsMaster,
                    stlErrorStack * aErrorStack );

stlStatus ztrmInitializeEnv4Master( stlErrorStack * aErrorStack );

stlStatus ztrmFinalizeEnv4Master( stlErrorStack * aErrorStack );

stlStatus ztrmInitializeEnv4Slave( stlErrorStack * aErrorStack );

stlStatus ztrmFinalizeEnv4Slave( stlErrorStack * aErrorStack );

stlStatus ztrmSetUserId( stlChar       * aUserId,
                         stlErrorStack * aErrorStack );

stlStatus ztrmGetUserId( stlChar       * aUserId,
                         stlErrorStack * aErrorStack );

stlStatus ztrmSetUserPw( stlChar       * aUserPw,
                         stlErrorStack * aErrorStack );

stlStatus ztrmGetUserPw( stlChar       * aUserPw,
                         stlErrorStack * aErrorStack );

stlStatus ztrmSetLogPath( stlChar       * aLogPath,
                          stlErrorStack * aErrorStack );

stlStatus ztrmGetLogPath( stlChar       * aLogPath,
                          stlErrorStack * aErrorStack );

stlStatus ztrmSetPort( stlChar       * aPort,
                       stlErrorStack * aErrorStack );

stlStatus ztrmGetPort( stlInt32      * aPort,
                       stlErrorStack * aErrorStack );

stlStatus ztrmSetMasterIp( stlChar       * aMasterIp,
                           stlErrorStack * aErrorStack );

stlChar * ztrmGetMasterIp();

stlStatus ztrmSetDsn( stlChar       * aDsn,
                      stlErrorStack * aErrorStack );

stlChar * ztrmGetDsn();

stlStatus ztrmSetHostIp( stlChar       * aHostIp,
                         stlErrorStack * aErrorStack );

stlChar * ztrmGetHostIp();

stlStatus ztrmSetHostPort( stlChar       * aPort,
                           stlErrorStack * aErrorStack );

stlStatus ztrmGetHostPort( stlInt32      * aPort,
                           stlErrorStack * aErrorStack );

stlStatus ztrmSetConfigureFile( const stlChar * aConfigureFile,
                                stlErrorStack * aErrorStack );

stlStatus ztrmGetConfigureFileName( stlChar       * aFileName,
                                    stlErrorStack * aErrorStack );

stlStatus ztrmGetLogFileName( stlChar       * aFileName,
                              stlInt64        aFileSeq,
                              stlErrorStack * aErrorStack );

stlStatus ztrmGetControlFileName( stlChar       * aFileName,
                                  stlErrorStack * aErrorStack );

stlStatus ztrmSetLogMirrorShmKeyFromDB( stlErrorStack * aErrorStack );

stlStatus ztrmGetPidFileName( stlChar       * aFileName,
                              stlBool         aIsMaster,
                              stlErrorStack * aErrorStack );

stlBool   ztrmGetIBMode();

void      ztrmSetCommunicateCallback( ztrCommunicateCallback * aCallback );

stlStatus ztrmSetHomeDir( stlErrorStack * aErrorStack );

/**
 * Threads
 */
stlStatus ztrmCreateThreads( stlErrorStack * aErrorStack );

stlStatus ztrmJoinThreads( stlBool          aIsMaster,
                           stlErrorStack  * aErrorStack );

/**
 * Dbc
 */
stlStatus ztrmAllocEnvHandle( SQLHENV       * aEnvHandle,
                              stlErrorStack * aErrorStack );

stlStatus ztrmFreeEnvHandle( SQLHENV         aEnvHandle,
                             stlErrorStack * aErrorStack );

stlStatus ztrmSQLAllocStmt( SQLHANDLE       aInputHandle,
                            SQLHANDLE     * aOutputHandlePtr,
                            stlErrorStack * aErrorStack );

stlStatus ztrmSQLFreeStmt( SQLHANDLE       aHandle,
                           stlErrorStack * aErrorStack );

stlStatus ztrmCreateConnection( SQLHDBC       * aDbcHandle,
                                SQLHENV         aEnvHandle,
                                stlErrorStack * aErrorStack );

stlStatus ztrmDestroyConnection( SQLHDBC         aDbcHandle,
                                 stlErrorStack * aErrorStack );

stlStatus ztrmSQLPrepare( SQLHSTMT        aStmtHandle,
                          SQLCHAR       * aStmtText,
                          SQLINTEGER      aTextLength,
                          stlErrorStack * aErrorStack );

stlStatus ztrmSQLBindCol( SQLHSTMT        aStmtHandle,
                          SQLUSMALLINT    aColumnNumber,
                          SQLSMALLINT     aTargetType,
                          SQLPOINTER      aTargetValuePtr,
                          SQLLEN          aBufferLength,
                          SQLLEN        * aIndicator,
                          stlErrorStack * aErrorStack );

stlStatus ztrmSQLExecDirect( SQLHSTMT        aStmtHandle,
                             SQLCHAR       * aStmtText,
                             SQLINTEGER      aTextLength,
                             SQLRETURN     * aReturn,
                             stlErrorStack * aErrorStack );

stlStatus ztrmSQLExecute( SQLHSTMT        aStmtHandle,
                          SQLRETURN     * aReturn,
                          stlErrorStack * aErrorStack );

stlStatus ztrmSQLFetch( SQLHSTMT        aStmtHandle,
                        SQLRETURN     * aReturn,
                        stlErrorStack * aErrorStack );

stlStatus ztrmPrintDiagnostic( SQLSMALLINT     aHandleType,
                               SQLHANDLE       aHandle,
                               stlErrorStack * aErrorStack );

void ztrmWriteDiagnostic( SQLSMALLINT     aHandleType,
                          SQLHANDLE       aHandle,
                          stlErrorStack * aErrorStack );

stlStatus ztrmGetDiagnosticMsg( SQLSMALLINT     aHandleType,
                                SQLHANDLE       aHandle,
                                SQLSMALLINT     aRecNumber,
                                stlChar       * aMsg,
                                stlErrorStack * aErrorStack );


/**
 * Logging
 */
void      ztrmPrintErrorstack( stlErrorStack * aErrorStack );

stlStatus ztrmLogMsg( stlErrorStack  * aErrorStack,
                      const stlChar  * aFormat,
                      ... );
                      
void      ztrmWriteLastError( stlErrorStack * aErrorStack );

stlStatus ztrmWriteTimeStamp( stlFile        * aFile,
                              stlErrorStack  * aErrorStack );

stlStatus ztrmAddLogMsg( stlChar        * aMsg,
                         stlInt32         aLength,
                         stlErrorStack  * aErrorStack );

/**
 * SpinLock
 */

void      ztrmInitSpinLock( ztrSpinLock * aSpinLock );

void      ztrmFinSpinLock( ztrSpinLock * aSpinLock );

void      ztrmAcquireSpinLock( ztrSpinLock * aSpinLock );

void      ztrmTrySpinLock( ztrSpinLock * aSpinLock,
                           stlBool     * aIsSuccess );
                           
void      ztrmReleaseSpinLock( ztrSpinLock * aSpinLock );


/**
 * Communication
 */

stlStatus ztrmSendHdrNData( void            * aHandle,
                            ztrLogMirrorHdr * aHeader,
                            stlChar         * aData,
                            stlErrorStack   * aErrorStack );

stlStatus ztrmSendHdrNData4IB( void            * aHandle,
                               ztrLogMirrorHdr * aHeader,
                               stlChar         * aData,
                               stlErrorStack   * aErrorStack );

stlStatus ztrmRecvHdrNData( void            * aHandle,
                            ztrLogMirrorHdr * aHeader,
                            stlChar         * aData,
                            stlErrorStack   * aErrorStack );

stlStatus ztrmRecvHdrNData4IB( void            * aHandle,
                               ztrLogMirrorHdr * aHeader,
                               stlChar         * aData,
                               stlErrorStack   * aErrorStack );

stlStatus ztrmRecvHdrNDataWithPoll( void            * aHandle,
                                    ztrLogMirrorHdr * aHeader,
                                    stlChar         * aData,
                                    stlBool         * aIsTimeout,
                                    stlErrorStack   * aErrorStack );

stlStatus ztrmRecvHdrNDataWithPoll4IB( void            * aHandle,
                                       ztrLogMirrorHdr * aHeader,
                                       stlChar         * aData,
                                       stlBool         * aIsTimeout,
                                       stlErrorStack   * aErrorStack );

stlStatus ztrmSendData( stlContext    * aContext,
                        stlChar       * aData,
                        stlInt64        aSize,
                        stlErrorStack * aErrorStack );

stlStatus ztrmSendData4IB( stlRcHandle     aHandle,
                           stlChar       * aData,
                           stlInt64        aSize,
                           stlErrorStack * aErrorStack );

stlStatus ztrmRecvData( stlContext    * aContext,
                        stlChar       * aData,
                        stlInt64        aSize,     
                        stlErrorStack * aErrorStack );

stlStatus ztrmRecvData4IB( stlRcHandle     aHandle,
                           stlChar       * aData,
                           stlInt64        aSize,
                           stlBool         aIsPolling,
                           stlBool       * aIsTimeout,
                           stlErrorStack * aErrorStack );

stlStatus ztrmDestroyHandle( void          * aHandle,
                             stlErrorStack * aErrorStack );

stlStatus ztrmDestroyHandle4IB( void          * aHandle,
                                stlErrorStack * aErrorStack );


/** @} */


#endif /* _ZTCM_H_ */
