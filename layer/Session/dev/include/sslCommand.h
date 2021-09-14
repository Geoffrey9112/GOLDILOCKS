/*******************************************************************************
 * sslCommand.h
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

#ifndef _SSLCOMMAND_H_
#define _SSLCOMMAND_H_ 1

/**
 * @file sslCommand.h
 * @brief Session Command
 */

stlStatus sslConnect( stlChar           * aUserName,
                      stlChar           * aEncryptedPassword,
                      stlChar           * aRoleName,
                      stlChar           * aOldPassword,
                      stlChar           * aNewPassword,
                      stlProc             aProc,
                      stlChar           * aProgram,
                      stlChar           * aAddress,
                      stlInt64            aPort,
                      dtlCharacterSet     aCharacterSet,
                      stlInt32            aTimezone,
                      dtlCharacterSet   * aDBCharacterSet,
                      dtlCharacterSet   * aDBNCharCharacterSet,
                      stlInt32          * aDBTimezone,
                      sslEnv            * aEnv );

stlStatus sslDisconnect( sslEnv * aEnv );

stlStatus sslCommit( sslEnv * aEnv );

stlStatus sslRollback( sslEnv * aEnv );

stlStatus sslSetSessionAttr( sslConnectionAttr   aSessionAttr,
                             stlInt32            aIntValue,
                             stlChar           * aStrValue,
                             stlInt32            aStrLength,
                             sslEnv            * aEnv );

stlStatus sslGetSessionAttr( sslConnectionAttr   aSessionAttr,
                             stlInt32          * aIntValue,
                             stlChar           * aStrValue,
                             stlInt32            aBufferLength,
                             stlInt32          * aStrLength,
                             sslEnv            * aEnv );

stlStatus sslAllocStatement( stlInt64  * aStatementId,
                             sslEnv    * aEnv );

stlStatus sslFreeStatement( stlInt64            aStatementId,
                            sslFreeStmtOption   aOption,
                            sslEnv            * aEnv );

stlStatus sslCreateParameter( stlInt64                aStatementId,
                              stlInt32                aParamId,
                              sslParamType            aBindType,
                              dtlDataType             aDtlDataType,
                              stlInt64                aArg1,
                              stlInt64                aArg2,
                              dtlStringLengthUnit     aStringLengthUnit,
                              dtlIntervalIndicator    aIntervalIndicator,
                              knlValueBlockList    ** aINValueBlock,
                              knlValueBlockList    ** aOUTValueBlock,
                              sslEnv                * aEnv );

stlStatus sslCreateParameter4Imp( stlInt64             aStatementId,
                                  stlInt32             aParamCount,
                                  dtlValueBlockList ** aValueBlock,
                                  sslEnv             * aEnv );

stlStatus sslSetParameterData4Imp( stlInt64             aStatementId,
                                   stlInt32             aParamCount,
                                   dtlValueBlockList ** aValueBlock,
                                   sslEnv             * aEnv );

stlStatus sslGetParameterData( stlInt64         aStatementId,
                               stlInt32         aParamId,
                               dtlDataValue  ** aINBindValue,
                               dtlDataValue  ** aOUTBindValue,
                               sslEnv         * aEnv );

stlStatus sslReallocLongVaryingMem( dtlDataValue  * aBindValue,
                                    sslEnv        * aEnv );

stlStatus sslPrepare( stlChar              * aSQLTextString,
                      stlInt64               aStatementId,
                      stlBool              * aHasTransaction,
                      stlInt32             * aStatementType,
                      stlBool              * aResultSet,
                      stlBool              * aWithoutHoldCursor,
                      stlBool              * aIsUpdatable,
                      ellCursorSensitivity * aSensitivity,
                      sslEnv               * aEnv );

stlStatus sslExecute( stlInt64         aStatementId,
                      stlBool          aIsFirst,
                      stlBool        * aHasTransaction,
                      stlBool        * aIsRecompile,
                      stlBool        * aResultSet,
                      stlInt64       * aAffectedRowCount,
                      sslDCLContext  * aDCLContext,
                      sslEnv         * aEnv );

stlStatus sslExecDirect( stlChar              * aSQLTextString,
                         stlInt64               aStatementId,
                         stlBool                aIsFirst,
                         stlBool              * aHasTransaction,
                         stlBool              * aIsRecompile,
                         stlInt32             * aStatementType,
                         stlBool              * aResultSet,
                         stlBool              * aWithoutHoldCursor,
                         stlBool              * aIsUpdatable,
                         ellCursorSensitivity * aSensitivity,
                         stlInt64             * aAffectedRowCount,
                         sslDCLContext        * aDCLContext,
                         sslEnv               * aEnv );

stlStatus sslNumParams( stlInt64        aStatementId,
                        stlInt16      * aParameterCount,
                        sslEnv        * aEnv );

stlStatus sslNumResultCols( stlInt64    aStatementId,
                            stlInt16  * aColumnCount,
                            sslEnv    * aEnv );

stlStatus sslDescribeCol( stlInt64     aStatementId,
                          qllTarget ** aTargetInfo,
                          sslEnv     * aEnv );

stlStatus sslGetParameterType( stlInt64    aStatementId,
                               stlInt32    aParamIdx,
                               stlInt16  * aInputOutputType,
                               sslEnv    * aEnv );

stlStatus sslGetTargetType( stlInt64             aStatementId,
                            stlBool            * aChanged,
                            knlValueBlockList ** aTargetBlockList,
                            sslEnv             * aEnv );

stlStatus sslSetFetchPosition( stlInt64    aStatementId,
                               stlInt64    aRowsetFirstRow,
                               stlInt64    aRowsetCurrentPos,
                               stlInt64    aRowsetLastPos,
                               sslEnv    * aEnv );

stlStatus sslGetFetchPosition( stlInt64                  aStatementId,
                               stlInt64                * aRowsetFirstRow,
                               stlInt64                * aRowsetCurrentPos,
                               stlInt64                * aRowsetLastPos,
                               stlInt64                * aKnownLastRowIdx,
                               ellCursorSensitivity    * aSensitivity,
                               ellCursorUpdatability   * aUpdatability,
                               ellCursorScrollability  * aScrollability,
                               knlValueBlockList      ** aTargetBlock,
                               sslEnv                  * aEnv );

stlStatus sslFetch( stlInt64             aStatementId,
                    stlInt64             aOffset,
                    knlValueBlockList ** aTargetBuffer,
                    stlBool            * aIsEndOfScan,
                    stlBool            * aIsBlockRead,
                    stlInt32           * aOneRowIdx,
                    qllCursorRowStatus * aOneRowStatus,
                    stlInt64           * aAbsIdxOfFirstRow,
                    stlInt64           * aKnownLastRowIdx,
                    sslEnv             * aEnv );

stlStatus sslCloseCursor( stlInt64    aStatementId,
                          sslEnv    * aEnv );

stlStatus sslSetCursorName( stlInt64    aStatementId,
                            stlChar   * aCursorName,
                            sslEnv    * aEnv );

stlStatus sslSetStmtAttr( stlInt64           aStatementId,
                          sslStatementAttr   aAttribute,
                          stlInt64           aIntValue,
                          stlChar          * aStrValue,
                          stlInt32           aStrLength,
                          sslEnv           * aEnv );

stlStatus sslGetCursorName( stlInt64    aStatementId,
                            stlChar   * aCursorName,
                            sslEnv    * aEnv );

stlStatus sslGetStmtAttr( stlInt64           aStatementId,
                          sslStatementAttr   aAttribute,
                          stlInt64         * aIntValue,
                          stlChar          * aStrValue,
                          stlInt32           aBufferLength, 
                          stlInt32         * aStrLength,
                          sslEnv           * aEnv );

stlStatus sslSetPos( stlInt64    aStatementId,
                     stlInt64    aRowNumber,
                     stlInt16    aOperation,
                     sslEnv    * aEnv );

stlStatus sslGetProperty( knlPropertyID   aPropertyID,
                          void          * aValue,
                          sslEnv        * aEnv );

void sslSetSessionProcessInfo( stlProc        * aProcessId,
                               stlChar        * aProcessName,
                               sslSessionEnv  * aSessionEnv );

stlStatus sslEnterSession( stlUInt32       aSessionId,
                           stlInt64        aSessionSeq,
                           stlErrorStack * aErrorStack );

void sslLeaveSession( stlUInt32  aSessionId,
                      stlInt64   aSessionSeq );

void sslSetCallStack( sslEnv * aEnv );

#endif /* _SSLCOMMAND_H_ */
