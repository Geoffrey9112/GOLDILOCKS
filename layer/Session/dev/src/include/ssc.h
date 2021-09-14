/*******************************************************************************
 * ssc.h
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

#ifndef _SSC_H_
#define _SSC_H_ 1

/**
 * @file ssc.h
 * @brief Session internal Command
 */

stlStatus sscAuthenticateUser( stlChar * aUserName,
                               stlChar * aEncryptedPasswd,
                               stlChar * aRoleName,
                               stlChar * aOldPassword,
                               stlChar * aNewPassword,
                               sslEnv  * aEnv );

stlStatus sscPrepare( stlChar              * aSQLTextString,
                      sslStatement         * aStatement,
                      stlBool              * aHasTransaction,
                      qllNodeType          * aStatementType,
                      stlBool              * aResultSet,
                      stlBool              * aWithoutHoldCursor,
                      stlBool              * aIsUpdatable,
                      ellCursorSensitivity * aSensitivity,
                      sslEnv               * aEnv );

stlStatus sscExecute( sslStatement   * aStatement,
                      stlBool          aIsFirst,
                      stlBool        * aHasTransaction,
                      stlBool        * aIsRecompile,
                      stlBool        * aHasResultSet,
                      stlInt64       * aAffectedRowCount,
                      sslDCLContext  * aDCLContext,
                      sslEnv         * aEnv );

stlStatus sscExecDirect( stlChar              * aSQLTextString,
                         sslStatement         * aStatement,
                         stlBool                aIsFirst,
                         stlBool              * aHasTransaction,
                         stlBool              * aIsRecompile,
                         qllNodeType          * aStatementType,
                         stlBool              * aHasResultSet,
                         stlBool              * aWithoutHoldCursor,
                         stlBool              * aIsUpdatable,
                         ellCursorSensitivity * aSensitivity,
                         stlInt64             * aAffectedRowCount,
                         sslDCLContext        * aDCLContext,
                         sslEnv               * aEnv );

stlStatus sscNumParams( sslStatement * aStatement,
                        stlInt16     * aParameterCount,
                        sslEnv       * aEnv );

stlStatus sscNumResultCols( sslStatement * aStatement,
                            stlInt16     * aColumnCount,
                            sslEnv       * aEnv );

stlStatus sscDescribeCol( sslStatement  * aStatement,
                          qllTarget    ** aTargetInfo,
                          sslEnv        * aEnv );

stlStatus sscDescribeParam( sslStatement * aStatement,
                            stlInt32       aParamIdx,
                            stlInt16     * aInputOutputType,
                            sslEnv       * aEnv );

stlStatus sscFetch( sslStatement       * aStatement,
                    stlInt64             aOffset,
                    knlValueBlockList ** aTargetBuffer,
                    stlBool            * aIsEndOfScan,
                    stlBool            * aIsBlockRead,
                    stlInt32           * aOneRowIdx,
                    qllCursorRowStatus * aOneRowStatus,
                    stlInt64           * aAbsIdxOfFirstRow,
                    stlInt64           * aKnownLastRowIdx,
                    sslEnv             * aEnv );

stlStatus sscCloseCursor( sslStatement * aStatement,
                          sslEnv       * aEnv );

stlStatus sscCloseWithoutHoldCursors( sslEnv * aEnv );

stlStatus sscSetPos(sslStatement  * aStatement,
                    stlInt64        aRowNumber,
                    stlInt16        aOperation,
                    sslEnv        * aEnv );

stlStatus sscSetDCL( sslStatement  * aStatement,
                     sslDCLContext * aDCLContext,
                     sslEnv        * aEnv );


stlStatus sscSetCursorName( sslStatement * aStatement,
                            stlChar      * aCursorName,
                            sslEnv       * aEnv );

stlStatus sscGetCursorName(sslStatement * aStatement,
                           stlChar      * aCursorName,
                           sslEnv       * aEnv );

#endif /* _SSC_H_ */
