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

stlStatus ztnmSetUserIdToGlobalArea( stlChar       * aUserId,
                                     stlErrorStack * aErrorStack );

stlStatus ztnmSetUserIdToGroupArea( stlChar       * aUserId,
                                    stlErrorStack * aErrorStack );

stlStatus ztnmGetUserId( stlChar       * aUserId,
                         stlErrorStack * aErrorStack );

stlStatus ztnmSetUserPwToGlobalArea( stlChar       * aUserPw,
                                     stlErrorStack * aErrorStack );

stlStatus ztnmSetUserPwToGroupArea( stlChar       * aUserPw,
                                    stlErrorStack * aErrorStack );

stlStatus ztnmGetUserPw( stlChar       * aUserPw,
                         stlErrorStack * aErrorStack );

stlStatus ztnmSetUserEncryptPwToGlobalArea( stlChar       * aUserEncryptPw,
                                            stlErrorStack * aErrorStack );

stlStatus ztnmSetUserEncryptPwToGroupArea( stlChar       * aUserEncryptPw,
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

stlInt64  ztnmGetMonitorCycle();

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
 * Dbc
 */

stlStatus ztnmCreateConnection( SQLHDBC       * aDbcHandle,
                                SQLHENV         aEnvHandle,
                                stlErrorStack * aErrorStack );

stlStatus ztnmDestroyConnection( SQLHDBC         aDbcHandle,
                                 stlErrorStack * aErrorStack );

stlStatus ztnmAllocEnvHandle( SQLHENV       * aEnvHandle,
                              stlErrorStack * aErrorStack );

stlStatus ztnmFreeEnvHandle( SQLHENV         aEnvHandle,
                             stlErrorStack * aErrorStack );

void ztnmWriteDiagnostic( SQLSMALLINT     aHandleType,
                          SQLHANDLE       aHandle,
                          stlErrorStack * aErrorStack );

stlStatus ztnmGetDiagnosticMsg( SQLSMALLINT     aHandleType,
                                SQLHANDLE       aHandle,
                                SQLSMALLINT     aRecNumber,
                                stlChar       * aMsg,
                                stlErrorStack * aErrorStack );

stlStatus ztnmPrintDiagnostic( SQLSMALLINT     aHandleType,
                               SQLHANDLE       aHandle,
                               stlErrorStack * aErrorStack );

stlStatus ztnmGetLogFileCount( SQLHDBC         aDbcHandle,
                               stlInt32      * aCount,
                               stlErrorStack * aErrorStack );

stlStatus ztnmBuildLogFileInfo( SQLHDBC         aDbcHandle,
                                stlErrorStack * aErrorStack );

stlStatus ztnmGetDbStatus( SQLHDBC         aDbcHandle,
                           stlInt16      * aGroupId,
                           stlInt64      * aFileSeqNo,
                           stlInt32      * aBlockSeq,
                           stlErrorStack * aErrorStack );

stlStatus ztnmInsertBlankMonitorInfo( SQLHDBC         aDbcHandle,
                                      stlErrorStack * aErrorStack );

stlStatus ztnmUpdateMonitorInfo( SQLHDBC         aDbcHandle,
                                 SQLHSTMT        aStmtHandle,
                                 ztnParameter  * aParameter,
                                 stlInt16        aGroupId,
                                 stlInt64        aFileSeqNo,
                                 stlInt32        aBlockSeq,
                                 stlErrorStack * aErrorStack );

stlStatus ztnmInitUpdateMonitorStmt( SQLHDBC         aDbcHandle,
                                     SQLHSTMT        aStmtHandle,
                                     ztnParameter  * aParameter,
                                     stlErrorStack * aErrorStack );

stlStatus ztnmCheckTableExist( SQLHDBC         aDbcHandle,
                               stlErrorStack * aErrorStack );

stlStatus ztnmSQLAllocStmt( SQLHANDLE       aInputHandle,
                            SQLHANDLE     * aOutputHandlePtr,
                            stlErrorStack * aErrorStack );

stlStatus ztnmSQLFreeStmt( SQLHANDLE       aHandle,
                           stlErrorStack * aErrorStack );

stlStatus ztnmSQLPrepare( SQLHSTMT        aStmtHandle,
                          SQLCHAR       * aStmtText,
                          SQLINTEGER      aTextLength,
                          stlErrorStack * aErrorStack );

stlStatus ztnmSQLBindCol( SQLHSTMT        aStmtHandle,
                          SQLUSMALLINT    aColumnNumber,
                          SQLSMALLINT     aTargetType,
                          SQLPOINTER      aTargetValuePtr,
                          SQLLEN          aBufferLength,
                          SQLLEN        * aIndicator,
                          stlErrorStack * aErrorStack );

stlStatus ztnmSQLBindParameter( SQLHSTMT        aStmtHandle,
                                SQLUSMALLINT    aParameterNumber,
                                SQLSMALLINT     aInputOutputType,
                                SQLSMALLINT     aValueType,
                                SQLSMALLINT     aParameterType,
                                SQLULEN         aColumnSize,
                                SQLSMALLINT     aDecimalDigits,
                                SQLPOINTER      aParameterValuePtr,
                                SQLLEN          aBufferLength,
                                SQLLEN        * aIndicator,
                                stlErrorStack * aErrorStack );
                                
stlStatus ztnmSQLExecute( SQLHSTMT        aStmtHandle,
                          SQLRETURN     * aReturn,
                          stlErrorStack * aErrorStack );
                          
stlStatus ztnmSQLExecDirect( SQLHSTMT        aStmtHandle,
                             SQLCHAR       * aStmtText,
                             SQLINTEGER      aTextLength,
                             SQLRETURN     * aReturn,
                             stlErrorStack * aErrorStack );
                             
stlStatus ztnmSQLFetch( SQLHSTMT        aStmtHandle,
                        SQLRETURN     * aReturn,
                        stlErrorStack * aErrorStack );


/**
 * Communicate
 */

stlStatus ztnmConnectToCyclone( ztnGroupItem  * aGroup,
                                stlErrorStack * aErrorStack );

stlStatus ztnmSendProtocolHdrNData( stlContext     * aContext,
                                    ztnProtocolHdr   aHeader,
                                    stlChar        * aData,
                                    stlErrorStack  * aErrorStack );

stlStatus ztnmReceiveProtocolHdrWithPoll( stlContext     * aContext,
                                          ztnProtocolHdr * aHeader,
                                          stlErrorStack  * aErrorStack );

stlStatus ztnmReceiveProtocolHdr( stlContext     * aContext,
                                  ztnProtocolHdr * aHeader,
                                  stlErrorStack  * aErrorStack );

stlStatus ztnmSendData( stlContext    * aContext,
                        stlChar       * aData,
                        stlInt32        aSize,
                        stlErrorStack * aErrorStack );

stlStatus ztnmReceiveData( stlContext    * aContext,
                           stlChar       * aData,
                           stlInt32        aSize,     
                           stlErrorStack * aErrorStack );

stlStatus ztnmDoProtocol( ztnGroupItem  * aGroup,
                          stlErrorStack * aErrorStack );
                          
/**
 * Protocol
 */

stlStatus ztnmInitProtocol( ztnGroupItem  * aGroup,
                            stlErrorStack * aErrorStack );

stlStatus ztnmMonitorProtocol( ztnGroupItem  * aGroup,
                               stlErrorStack * aErrorStack );



/**
 * Encrypt
 */

stlStatus ztnmPrintCipherText( stlChar          * aPlainText,
                               stlErrorStack    * aErrorStack);


stlStatus ztnmEncrypt( stlChar          * aPlainText,
                       stlSize            aPlainTextLen,
                       stlChar            aKey[16],
                       stlChar          * aCipherText,
                       stlSize          * aCipherTextLen,
                       stlErrorStack    * aErrorStack );

stlStatus ztnmDecrypt( stlChar          * aCipherText,
                       stlSize            aCipherTextLen,
                       stlChar            aKey[16],
                       stlChar          * aPlainText,
                       stlSize          * aPlainTextLen,
                       stlErrorStack    * aErrorStack );

stlStatus ztnmEncrypt8byte( stlUInt32       * aValue,
                            stlUInt32       * aKey,
                            stlErrorStack   * aErrorStack );

stlStatus ztnmDecrypt8byte( stlUInt32       * aValue,
                            stlUInt32       * aKey,
                            stlErrorStack   * aErrorStack );


stlStatus ztnmBase64Encode( stlChar       * aInput,
                            stlSize         aInputLen,
                            stlChar       * aBuf,
                            stlSize         aBufSize,
                            stlErrorStack * aErrorStack );

stlStatus ztnmBase64Decode( stlChar         * aInput,
                            stlUInt8        * aBuf,
                            stlSize         * aBufSize,
                            stlErrorStack   * aErrorStack );

/** @} */

#endif /* _ZTNM_H_ */


