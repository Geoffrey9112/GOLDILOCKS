/*******************************************************************************
 * ztqOdbcBridge.h
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


#ifndef _ZTQODBCBRIDGE_H_
#define _ZTQODBCBRIDGE_H_ 1

/**
 * @file ztqOdbcBridge.h
 * @brief ODBC Bridge  Definition
 */

/**
 * @defgroup ztqOdbcBridge ODBC Bridge
 * @ingroup ztq
 * @{
 */

stlStatus ztqOpenDriver( stlChar        * aDriverStr,
                         void          ** aDriverHandle,
                         stlErrorStack  * aErrorStack );

stlStatus ztqCloseDriver( void          * aDriverHandle,
                          stlErrorStack * aErrorStack );

stlStatus ztqSQLAllocHandle( SQLSMALLINT     aHandleType,
                             SQLHANDLE       aInputHandle,
                             SQLHANDLE     * aOutputHandle,
                             stlErrorStack * aErrorStack );

stlStatus ztqSQLConnect( SQLHDBC         aConnectionHandle,
                         SQLCHAR       * aServerName,
                         SQLSMALLINT     aNameLength1,
                         SQLCHAR       * aUserName,
                         SQLSMALLINT     aNameLength2,
                         SQLCHAR       * aAuthentication,
                         SQLSMALLINT     aNameLength3,
                         stlAllocator  * aAllocator,
                         stlErrorStack * aErrorStack );

stlStatus ztqSQLDriverConnect( SQLHDBC         aConnectionHandle,
                               SQLCHAR       * aConnString,
                               SQLSMALLINT     aConnStringLength,
                               stlAllocator  * aAllocator,
                               stlErrorStack * aErrorStack );

stlStatus ztqSQLSetEnvAttr( SQLHENV         aEnvHandle,
                            SQLINTEGER      aAttribute,
                            SQLPOINTER      aValuePtr,
                            SQLINTEGER      aStringLength,
                            stlErrorStack * aErrorStack  );

stlStatus ztqSQLPrepare( SQLHSTMT        aStmtHandle,
                         SQLCHAR       * aStmtText,
                         SQLINTEGER      aTextLength,
                         stlAllocator  * aAllocator,
                         stlErrorStack * aErrorStack );

stlStatus ztqSQLExecute( SQLHSTMT        aStmtHandle,
                         SQLRETURN     * aReturn,
                         stlErrorStack * aErrorStack );

stlStatus ztqSQLFetch( SQLHSTMT        aStmtHandle,
                       SQLRETURN     * aReturn,
                       stlErrorStack * aErrorStack );

stlStatus ztqSQLNumResultCols( SQLHSTMT        aStmtHandle,
                               SQLSMALLINT   * aColumnCount,
                               stlErrorStack * aErrorStack );

stlStatus ztqSQLDescribeCol( SQLHSTMT        aStmtHandle,
                             SQLUSMALLINT    aColumnNumber,
                             SQLCHAR       * aColumnName,
                             SQLSMALLINT     aBufferLength,
                             SQLSMALLINT   * aNameLengthPtr,
                             SQLSMALLINT   * aDataTypePtr,
                             SQLULEN       * aColumnSizePtr,
                             SQLSMALLINT   * aDecimalDigitsPtr,
                             SQLSMALLINT   * aNullablePtr,
                             stlAllocator  * aAllocator,
                             stlErrorStack * aErrorStack );

stlStatus ztqSQLBindCol( SQLHSTMT        aStmtHandle,
                         SQLUSMALLINT    aColumnNumber,
                         SQLSMALLINT     aTargetType,
                         SQLPOINTER      aTargetValuePtr,
                         SQLLEN          aBufferLength,
                         SQLLEN        * aIndicator,
                         stlErrorStack * aErrorStack );

stlStatus ztqSQLExecDirect( SQLHSTMT        aStmtHandle,
                            SQLCHAR       * aStmtText,
                            SQLINTEGER      aTextLength,
                            SQLRETURN     * aReturn,
                            stlAllocator  * aAllocator,
                            stlErrorStack * aErrorStack );

stlStatus ztqDescPrepare( SQLHSTMT        aStmtHandle,
                          SQLCHAR       * aStmtText,
                          SQLINTEGER      aTextLength,
                          SQLRETURN     * aReturn,
                          stlAllocator  * aAllocator,
                          stlErrorStack * aErrorStack );

stlStatus ztqDescExecDirect( SQLHSTMT        aStmtHandle,
                             SQLCHAR       * aStmtText,
                             SQLINTEGER      aTextLength,
                             SQLRETURN     * aReturn,
                             stlAllocator  * aAllocator,
                             stlErrorStack * aErrorStack );

stlStatus ztqSQLEndTran( SQLSMALLINT     aHandleType,
                         SQLHANDLE       aHandle,
                         SQLSMALLINT     aCompletionType,
                         stlBool         aIgnoreFailure,
                         stlErrorStack * aErrorStack );

stlStatus ztqSQLRowCount( SQLHSTMT        aStmtHandle,
                          SQLLEN        * aRowCountPtr,
                          stlErrorStack * aErrorStack );

stlStatus ztqSQLFreeHandle( SQLSMALLINT     aHandleType,
                            SQLHANDLE       aHandle,
                            stlErrorStack * aErrorStack );

stlStatus ztqSQLFreeStmt( SQLHANDLE       aHandle,
                          SQLUSMALLINT    aOption,
                          stlErrorStack * aErrorStack );

stlStatus ztqSQLBindParameter( SQLHSTMT        aStmtHandle,
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

stlStatus ztqSQLSetConnectAttr( SQLHDBC         aConnectionHandle,
                                SQLINTEGER      aAttribute,
                                SQLPOINTER      aValuePtr,
                                SQLINTEGER      aStringLength,
                                stlErrorStack * aErrorStack );

stlStatus ztqSQLGetConnectAttr( SQLHDBC         aConnectionHandle,
                                SQLINTEGER      aAttribute,
                                SQLPOINTER      aValuePtr,
                                SQLINTEGER      aBufferLength,
                                SQLINTEGER    * aStringLengthPtr,
                                stlErrorStack * aErrorStack );

stlStatus ztqSQLDisconnect( SQLHDBC         aConnectionHandle,
                            stlErrorStack * aErrorStack );

stlStatus ztqSQLCloseCursor( SQLHSTMT        aStmtHandle,
                             stlErrorStack * aErrorStack );

stlStatus ztqSQLGetDiagRec( SQLSMALLINT     aHandleType,
                            SQLHANDLE       aHandle,
                            SQLSMALLINT     aRecNumber,
                            SQLCHAR       * aSqlState,
                            SQLINTEGER    * aNativeErrorPtr,
                            SQLCHAR       * aMessageText,
                            SQLSMALLINT     aBufferLength,
                            SQLSMALLINT   * aTextLengthPtr,
                            SQLRETURN     * aReturn,
                            stlErrorStack * aErrorStack );

stlStatus ztqSQLSetStmtAttr( SQLHSTMT        aStmtHandle,
                             SQLINTEGER      aAttribute,
                             SQLPOINTER      aValuePtr,
                             SQLINTEGER      aStringLength,
                             stlErrorStack * aErrorStack );

stlStatus ztqSQLGetStmtAttr( SQLHSTMT        aStmtHandle,
                             SQLINTEGER      aAttribute,
                             SQLPOINTER      aValuePtr,
                             SQLINTEGER      aBufferLength, 
                             SQLINTEGER    * aStringLength,
                             stlErrorStack * aErrorStack );

stlStatus ztqSQLGetDescField( SQLHDESC        aDescriptorHandle,
                              SQLSMALLINT     aRecNumber,
                              SQLSMALLINT     aFieldIdentifier,
                              SQLPOINTER      aValuePtr,
                              SQLINTEGER      aBufferLength,
                              SQLINTEGER    * aStringLengthPtr,
                              stlErrorStack * aErrorStack );

stlStatus ztqSQLSetDescField( SQLHDESC        aDescriptorHandle,
                              SQLSMALLINT     aRecNumber,
                              SQLSMALLINT     aFieldIdentifier,
                              SQLPOINTER      aValuePtr,
                              SQLINTEGER      aBufferLength,
                              stlErrorStack * aErrorStack );

stlStatus ztqPrintError( SQLHANDLE       aHandle,
                         SQLSMALLINT     aHandleType,
                         stlErrorStack * aErrorStack );

stlInt32 ztqGetNativeTopError( SQLHANDLE       aHandle,
                               SQLSMALLINT     aHandleType,
                               stlErrorStack * aErrorStack );

stlInt32 ztqGetNativeErrorByRecNumber(SQLHANDLE       aHandle,
                                      SQLSMALLINT     aHandleType,
                                      SQLSMALLINT     aRecNumber,
                                      stlErrorStack * aErrorStack );

stlStatus ztqGetXaConnectionHandle( SQLHANDLE     * aDbcHandle,
                                    stlErrorStack * aErrorStack );

stlStatus ztqOpenXa( stlChar       * aXaInfo,
                     stlInt64        aFlags,
                     stlErrorStack * aErrorStack );

stlStatus ztqCloseXa( stlChar       * aXaInfo,
                      stlInt64        aFlags,
                      stlErrorStack * aErrorStack );

stlStatus ztqStartXa( XID           * aXid,
                      stlInt64        aFlags,
                      stlErrorStack * aErrorStack );

stlStatus ztqEndXa( XID           * aXid,
                    stlInt64        aFlags,
                    stlErrorStack * aErrorStack );

stlStatus ztqCommitXa( XID           * aXid,
                       stlInt64        aFlags,
                       stlErrorStack * aErrorStack );

stlStatus ztqRollbackXa( XID           * aXid,
                         stlInt64        aFlags,
                         stlErrorStack * aErrorStack );

stlStatus ztqPrepareXa( XID           * aXid,
                        stlInt64        aFlags,
                        stlErrorStack * aErrorStack );

stlStatus ztqForgetXa( XID           * aXid,
                       stlInt64        aFlags,
                       stlErrorStack * aErrorStack );

stlStatus ztqRecoverXa( XID           * aXid,
                        stlInt64        aCount,
                        stlInt64        aFlags,
                        stlInt64      * aFetchCount,
                        stlErrorStack * aErrorStack );

/** @} */

#endif /* _ZTQODBCBRIDGE_H_ */
