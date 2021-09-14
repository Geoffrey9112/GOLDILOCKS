/*******************************************************************************
 * sclProtocol.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: sclProtocol.h 6534 2012-12-04 00:37:52Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _SCLPROTOCOL_H_
#define _SCLPROTOCOL_H_ 1

/**
 * @file sclProtocol.h
 * @brief Communication Layer Protocol Routines
 */

/**
 * @defgroup sclProtocol Protocol
 * @ingroup cmExternal
 * @{
 */

stlStatus sclInitializeHandle( sclHandle        * aHandle,
                               stlContext       * aContext,
                               sclIpc           * aIpc,
                               stlInt16           aMajorVersion,
                               stlInt16           aMinorVersion,
                               stlInt16           aPatchVersion,
                               sclEnv           * aEnv );

stlStatus sclFinalizeHandle( sclHandle     * aHandle,
                             sclEnv        * aEnv );

stlStatus sclInitializeProtocolSentence( sclHandle               * aHandle,
                                         sclProtocolSentence     * aProtocolSentence,
                                         sclMemoryManager        * aMemoryMgr,
                                         sclEnv                  * aEnv );

stlStatus sclFinalizeProtocolSentence( sclProtocolSentence      * aProtocolSentence,
                                       sclEnv                   * aEnv );

stlStatus sclInitializeMemoryManager( sclMemoryManager  * aMemoryMgr,
                                      stlInt64            aPacketBufferSize,
                                      stlInt64            aOperationBufferSize,
                                      stlInt64            aPollingBufferSize,
                                      sclEnv            * aEnv );

stlStatus sclFinalizeMemoryManager( sclMemoryManager  * aMemoryMgr,
                                    sclEnv            * aEnv );

stlStatus sclMoveNextProtocol( sclHandle       * aHandle,
                               stlTime           aTimeout,
                               stlBool         * aIsTimeout,
                               cmlCommandType  * aCommandType,
                               stlErrorStack   * aErrorStack );

stlStatus sclParseCommand( sclProtocolSentence  * aProtocolSentence,
                           cmlCommandType * aCommandType,
                           stlErrorStack  * aErrorStack );

stlStatus sclGetCommand( sclHandle      * aHandle,
                         cmlCommandType * aCommandType,
                         sclEnv         * aEnv );

stlStatus sclRecvPacket( sclHandle     * aHandle,
                         sclEnv        * aEnv );

stlStatus sclSendPacket( sclHandle     * aHandle,
                         sclEnv        * aEnv );

/*
 * ERROR
 */

stlStatus sclWriteErrorResult( sclHandle       * aHandle,
                               cmlCommandType    aResultType,
                               stlErrorStack   * aErrorStack,
                               sclEnv          * aEnv );

stlStatus sclWriteXaErrorResult( sclHandle       * aHandle,
                                 cmlCommandType    aResultType,
                                 stlInt32          aXaError,
                                 stlErrorStack   * aErrorStack,
                                 sclEnv          * aEnv );

stlStatus sclWriteIgnoreResult( sclHandle       * aHandle,
                                cmlCommandType    aResultType,
                                sclEnv          * aEnv );


/*
 * HANDSHAKE PROTOCOL
 */

stlStatus sclHandshakingForServer( sclHandle     * aHandle,
                                   stlChar       * aRoleName,
                                   sclEnv        * aEnv );

stlStatus sclReadHandshakeCommand( sclHandle      * aHandle,
                                   stlChar       * aRoleName,
                                   sclEnv         * aEnv );

stlStatus sclWriteHandshakeResult( sclHandle      * aHandle,
                                   stlInt32         aBufferSize,
                                   stlErrorStack  * aErrorStack,
                                   sclEnv         * aEnv );


/*
 * CONNECT PROTOCOL
 */

stlStatus sclReadConnectCommand( sclHandle       * aHandle,
                                 stlInt8         * aControl,
                                 stlChar         * aUserName,
                                 stlChar         * aPassword,
                                 stlChar         * aOldPassword,
                                 stlChar         * aNewPassword,
                                 stlChar         * aProgramName,
                                 stlInt32        * aProcessId,
                                 stlInt16        * aCharacterset,
                                 stlInt32        * aTimezone,
                                 cmlDbcType      * aDbcType,
                                 sclEnv          * aEnv );


stlStatus sclWriteConnectResult( sclHandle       * aHandle,
                                 stlUInt32         aSessionId,
                                 stlInt64          aSessionSeq,
                                 dtlCharacterSet   aCharacterset,
                                 dtlCharacterSet   aNCharCharacterset,
                                 stlInt32          aDBTimezone,
                                 stlErrorStack   * aErrorStack,
                                 sclEnv          * aEnv );

/*
 * DISCONNECT PROTOCOL
 */

stlStatus sclReadDisconnectCommand( sclHandle       * aHandle,
                                    sclEnv          * aEnv );

stlStatus sclWriteDisconnectResult( sclHandle         * aHandle,
                                    stlErrorStack     * aErrorStack,
                                    sclEnv            * aEnv );

/*
 * FETCHSCROLL PROTOCOL
 */

stlStatus sclReadFetchCommand( sclHandle     * aHandle,
                               stlInt8       * aControl,
                               stlInt64      * aStatementId,
                               stlInt64      * aOffset,
                               stlInt32      * aRowCount,
                               sclEnv        * aEnv );

stlStatus sclWriteFetchBegin( sclHandle         * aHandle,
                              stlInt64            aFirstRowIdx,
                              dtlValueBlockList * aValueBlockList,
                              stlErrorStack     * aErrorStack,
                              sclEnv            * aEnv );

stlStatus sclWriteFetchResult( sclHandle         * aHandle,
                               stlInt64            aStartIdx,
                               stlInt32            aRowCount,
                               dtlValueBlockList * aValueBlockList,
                               stlInt8             aStatus,
                               sclEnv            * aEnv );

stlStatus sclWriteFetchEnd( sclHandle          * aHandle,
                            stlInt64             aLastRowIdx,
                            stlBool              aCursorClosed,
                            sclEnv             * aEnv );

stlStatus sclWriteFetchEndWithError( sclHandle       * aHandle,
                                     sclEnv          * aEnv );

/*
 * GETCONNATTR PROTOCOL
 */

stlStatus sclReadGetConnAttrCommand( sclHandle      * aHandle,
                                     stlInt8        * aControl,
                                     stlInt32       * aAttrType,
                                     sclEnv         * aEnv );

stlStatus sclWriteGetConnAttrResult( sclHandle         * aHandle,
                                     stlInt32            aAttrType,
                                     dtlDataType         aAttrDataType,
                                     void              * aAttrValue,
                                     stlInt16            aAttrValueSize,
                                     stlErrorStack     * aErrorStack,
                                     sclEnv            * aEnv );

/*
 * SETCONNATTR PROTOCOL
 */

stlStatus sclReadSetConnAttrCommand( sclHandle      * aHandle,
                                     stlInt8        * aControl,
                                     stlInt32       * aAttrType,
                                     dtlDataType    * aAttrDataType,
                                     void           * aAttrValue,
                                     stlInt16       * aAttrValueSize,
                                     sclEnv         * aEnv );

stlStatus sclWriteSetConnAttrResult( sclHandle         * aHandle,
                                     stlErrorStack     * aErrorStack,
                                     sclEnv            * aEnv );

/*
 * DESCRIBECOL PROTOCOL
 */

stlStatus sclReadDescribeColCommand( sclHandle      * aHandle,
                                     stlInt8        * aControl,
                                     stlInt64       * aStatementId,
                                     stlInt32       * aColumnNumber,
                                     sclEnv         * aEnv );

stlStatus sclWriteDescribeColResult( sclHandle           * aHandle,
                                     stlChar             * aCatalogName,
                                     stlChar             * aSchemaName,
                                     stlChar             * aTableName,
                                     stlChar             * aBaseTableName,
                                     stlChar             * aColumnAliasName,
                                     stlChar             * aColumnLabel,
                                     stlChar             * aBaseColumnName,
                                     stlChar             * aLiteralPrefix,
                                     stlChar             * aLiteralSuffix,
                                     dtlDataType           aDbType,
                                     stlInt16              aIntervalCode,
                                     stlInt16              aNumPrecRadix,
                                     stlInt16              aDatetimeIntervPrec,
                                     stlInt64              aDisplaySize,
                                     dtlStringLengthUnit   aStringLengthUnit,
                                     stlInt64              aCharacterLength,
                                     stlInt64              aOctetLength,
                                     stlInt32              aPrecision,
                                     stlInt32              aScale,
                                     stlBool               aNullable,
                                     stlBool               aAliasUnnamed,
                                     stlBool               aCaseSensitive,
                                     stlBool               aUnsigned,
                                     stlBool               aFixedPrecScale,
                                     stlBool               aUpdatable,
                                     stlBool               aAutoUnique,
                                     stlBool               aRowVersion,
                                     stlBool               aAbleLike,
                                     stlErrorStack       * aErrorStack,
                                     sclEnv              * aEnv );

/*
 * PARAMETERTYPE PROTOCOL
 */
stlStatus sclReadParameterTypeCommand( sclHandle     * aHandle,
                                       stlInt8       * aControl,
                                       stlInt64      * aStatementId,
                                       stlInt32      * aParameterNumber,
                                       sclEnv        * aEnv );

stlStatus sclWriteParameterTypeResult( sclHandle     * aHandle,
                                       stlInt16        aInputOutputType,
                                       stlErrorStack * aErrorStack,
                                       sclEnv        * aEnv );

/*
 * PREPARE PROTOCOL
 */

stlStatus sclReadPrepareCommand( sclHandle      * aHandle,
                                 stlInt8        * aControl,
                                 stlInt64       * aStatementId,
                                 stlChar       ** aSqlText,
                                 stlInt64       * aSqlTextSize,
                                 sclEnv         * aEnv );

stlStatus sclWritePrepareResult( sclHandle      * aHandle,
                                 stlInt64         aStatementId,
                                 stlInt32         aStatementType,
                                 stlBool          aHasTrans,
                                 stlBool          aResultSet,
                                 stlBool          aIsWithoutHoldCursor,
                                 stlBool          aIsUpdatable,
                                 stlInt8          aSensitivity,
                                 stlErrorStack  * aErrorStack,
                                 sclEnv         * aEnv );

/*
 * EXECDIRECT PROTOCOL
 */

stlStatus sclReadExecDirectCommand( sclHandle      * aHandle,
                                    stlInt8        * aControl,
                                    stlInt64       * aStatementId,
                                    stlBool        * aIsFirst,
                                    stlChar       ** aSqlText,
                                    stlInt64       * aSqlTextSize,
                                    sclEnv         * aEnv );

stlStatus sclWriteExecDirectResult( sclHandle      * aHandle,
                                    stlInt64         aStatementId,
                                    stlInt32         aStatementType,
                                    stlInt64         aAffectedRowCount,
                                    stlBool          aRecompile,
                                    stlBool          aResultSet,
                                    stlBool          aIsWithoutHoldCursor,
                                    stlBool          aIsUpdatable,
                                    stlInt8          aSensitivity,
                                    stlBool          aHasTrans,
                                    stlInt8          aDclAttrCount,
                                    stlInt32         aDclAttrType,
                                    dtlDataType      aDclAttrDataType,
                                    stlChar        * aDclAttrValue,
                                    stlInt32         aDclAttrValueSize,
                                    stlErrorStack  * aErrorStack,
                                    sclEnv         * aEnv );

/*
 * EXECUTE PROTOCOL
 */

stlStatus sclReadExecuteCommand( sclHandle      * aHandle,
                                 stlInt8        * aControl,
                                 stlInt64       * aStatementId,
                                 stlBool        * aIsFirst,
                                 sclEnv         * aEnv );

stlStatus sclWriteExecuteResult( sclHandle      * aHandle,
                                 stlInt64         aAffectedRowCount,
                                 stlBool          aRecompile,
                                 stlBool          aResultSet,
                                 stlBool          aHasTrans,
                                 stlInt8          aDclAttrCount,
                                 stlInt32         aDclAttrType,
                                 dtlDataType      aDclAttrDataType,
                                 stlChar        * aDclAttrValue,
                                 stlInt32         aDclAttrValueSize,
                                 stlErrorStack  * aErrorStack,
                                 sclEnv         * aEnv );

/*
 * GETOUTBINDDATA PROTOCOL
 */

stlStatus sclReadGetOutBindDataCommand( sclHandle      * aHandle,
                                        stlInt8        * aControl,
                                        stlInt64       * aStatementId,
                                        stlInt32       * aColumnNumber,
                                        sclEnv         * aEnv );

stlStatus sclWriteGetOutBindDataResult( sclHandle      * aHandle,
                                        dtlDataValue   * aBindValue,
                                        stlErrorStack  * aErrorStack,
                                        sclEnv         * aEnv );


/*
 * FREESTMT PROTOCOL
 */

stlStatus sclReadFreeStmtCommand( sclHandle      * aHandle,
                                  stlInt8        * aControl,
                                  stlInt64       * aStatementId,
                                  stlInt16       * aOption,
                                  sclEnv         * aEnv );

stlStatus sclWriteFreeStmtResult( sclHandle         * aHandle,
                                  stlErrorStack     * aErrorStack,
                                  sclEnv            * aEnv );

/*
 * TRANSACTION PROTOCOL
 */

stlStatus sclReadTransactionCommand( sclHandle      * aHandle,
                                     stlInt8        * aControl,
                                     stlInt16       * aOption,
                                     sclEnv         * aEnv );

stlStatus sclWriteTransactionResult( sclHandle         * aHandle,
                                     stlErrorStack     * aErrorStack,
                                     sclEnv            * aEnv );


/*
 * TARGETTYPE PROTOCOL
 */

stlStatus sclReadTargetTypeCommand( sclHandle      * aHandle,
                                    stlInt8        * aControl,
                                    stlInt64       * aStatementId,
                                    sclEnv         * aEnv );

stlStatus sclWriteTargetTypeResult( sclHandle                * aHandle,
                                    stlBool                    aChanged,
                                    dtlValueBlockList        * aTargetBlockList,
                                    sclGetTargetTypeCallback   aCallback,
                                    stlErrorStack            * aErrorStack,
                                    sclEnv                   * aEnv );

/*
 * BINDTYPE PROTOCOL
 */


stlStatus sclReadBindTypeCommand( sclHandle      * aHandle,
                                  stlInt8        * aControl,
                                  stlInt64       * aStatementId,
                                  stlInt32       * aColumnNumber,  
                                  stlInt8        * aBindType,  
                                  stlInt8        * aDataType,
                                  stlInt64       * aArg1,
                                  stlInt64       * aArg2,
                                  stlInt8        * aStringLengthUnit,
                                  stlInt8        * aIntervalIndicator,
                                  sclEnv         * aEnv );

stlStatus sclWriteBindTypeResult( sclHandle         * aHandle,
                                  stlInt64            aStatementId,
                                  stlErrorStack     * aErrorStack,
                                  sclEnv            * aEnv );

/*
 * BINDDATA PROTOCOL
 */


stlStatus sclReadBindDataCommand( sclHandle                        * aHandle,
                                  stlInt8                          * aControl,
                                  stlBool                          * aProtocolError,
                                  sclCheckControlCallback            aCheckControlCallback,
                                  sclGetBindValueCallback            aGetValueCallback,
                                  sclReallocLongVaryingMemCallback   aReallocMemCallback,
                                  sclEnv                           * aEnv );

stlStatus sclWriteBindDataResult( sclHandle         * aHandle,
                                  stlErrorStack     * aErrorStack,
                                  sclEnv            * aEnv );


/*
 * GETSTMTATTR PROTOCOL
 */

stlStatus sclReadGetStmtAttrCommand( sclHandle      * aHandle,
                                     stlInt8        * aControl,
                                     stlInt64       * aStatementId,
                                     stlInt32       * aAttrType,
                                     stlInt32       * aStringBufferLength,
                                     sclEnv         * aEnv );

stlStatus sclWriteGetStmtAttrResult( sclHandle         * aHandle,
                                     stlInt64            aStatementId,
                                     stlInt32            aAttrType,
                                     dtlDataType         aAttrDataType,
                                     void              * aAttrValue,
                                     stlInt32            aAttrValueSize,
                                     stlInt32            aStringLength,
                                     stlErrorStack     * aErrorStack,
                                     sclEnv            * aEnv );

/*
 * SETSTMTATTR PROTOCOL
 */

stlStatus sclReadSetStmtAttrCommand( sclHandle      * aHandle,
                                     stlInt8        * aControl,
                                     stlInt64       * aStatementId,
                                     stlInt32       * aAttrType,
                                     dtlDataType    * aAttrDataType,
                                     void           * aAttrValue,
                                     stlInt16       * aAttrValueSize,
                                     sclEnv         * aEnv );

stlStatus sclWriteSetStmtAttrResult( sclHandle     * aHandle,
                                     stlInt64        aStatementId,
                                     stlErrorStack * aErrorStack,
                                     sclEnv        * aEnv );

/*
 * CLOSECURSOR PROTOCOL
 */

stlStatus sclReadCloseCursorCommand( sclHandle      * aHandle,
                                     stlInt8        * aControl,
                                     stlInt64       * aStatementId,
                                     sclEnv         * aEnv );

stlStatus sclWriteCloseCursorResult( sclHandle         * aHandle,
                                     stlErrorStack     * aErrorStack,
                                     sclEnv            * aEnv );


/*
 * CANCEL PROTOCOL
 */

stlStatus sclReadCancelCommand( sclHandle      * aHandle,
                                stlInt8        * aControl,
                                stlInt32       * aSessionId,
                                stlInt64       * aSessionSeq,
                                stlInt64       * aStatementId,
                                sclEnv         * aEnv );

stlStatus sclWriteCancelResult( sclHandle      * aHandle,
                                stlErrorStack  * aErrorStack,
                                sclEnv         * aEnv );

/*
 * NUMPARAMS PROTOCOL
 */


stlStatus sclReadNumParamsCommand( sclHandle      * aHandle,
                                   stlInt8        * aControl,
                                   stlInt64       * aStatementId,
                                   sclEnv         * aEnv );

stlStatus sclWriteNumParamsResult( sclHandle      * aHandle,
                                   stlInt32         aParamCnt,
                                   stlErrorStack  * aErrorStack,
                                   sclEnv         * aEnv );


/*
 * NUMRESULTCOLS PROTOCOL
 */

stlStatus sclReadNumResultColsCommand( sclHandle      * aHandle,
                                       stlInt8        * aControl,
                                       stlInt64       * aStatementId,
                                       sclEnv         * aEnv );

stlStatus sclWriteNumResultColsResult( sclHandle      * aHandle,
                                       stlInt32         aColumnCnt,
                                       stlErrorStack  * aErrorStack,
                                       sclEnv         * aEnv );


/*
 * GETCURSORNAME PROTOCOL
 */

stlStatus sclReadGetCursorNameCommand( sclHandle      * aHandle,
                                       stlInt8        * aControl,
                                       stlInt64       * aStatementId,
                                       sclEnv         * aEnv );

stlStatus sclWriteGetCursorNameResult( sclHandle      * aHandle,
                                       stlInt64         aStatementId,
                                       stlChar        * aCursorName,
                                       stlErrorStack  * aErrorStack,
                                       sclEnv         * aEnv );

/*
 * SETCURSORNAME PROTOCOL
 */

stlStatus sclReadSetCursorNameCommand( sclHandle      * aHandle,
                                       stlInt8        * aControl,
                                       stlInt64       * aStatementId,
                                       stlChar        * aCursorName,
                                       sclEnv         * aEnv );

stlStatus sclWriteSetCursorNameResult( sclHandle      * aHandle,
                                       stlInt64         aStatementId,
                                       stlErrorStack  * aErrorStack,
                                       sclEnv         * aEnv );


/*
 * SETPOS PROTOCOL
 */

stlStatus sclReadSetPosCommand( sclHandle      * aHandle,
                                stlInt8        * aControl,
                                stlInt64       * aStatementId,
                                stlInt64       * aRowNum,
                                stlInt16       * aOperation,
                                sclEnv         * aEnv );

stlStatus sclWriteSetPosResult( sclHandle      * aHandle,
                                stlInt64         aStatementId,
                                stlErrorStack  * aErrorStack,
                                sclEnv         * aEnv );

/*
 * STARTUP PROTOCOL
 */

stlStatus sclReadStartupCommand( sclHandle       * aHandle,
                                 stlInt8         * aControl,
                                 sclEnv          * aEnv );

stlStatus sclWriteStartupResult( sclHandle       * aHandle,
                                 stlErrorStack   * aErrorStack,
                                 sclEnv          * aEnv );

/*
 * XA PROTOCOL
 */


stlStatus sclReadXaCommand( sclHandle      * aHandle,
                            stlInt16       * aCommandType,
                            stlXid         * aXid,
                            stlInt32       * aRmId,
                            stlInt64       * aFlags,
                            sclEnv         * aEnv );

stlStatus sclReadXaCloseCommand( sclHandle      * aHandle,
                                 sclEnv         * aEnv );

stlStatus sclWriteXaResult( sclHandle      * aHandle,
                            stlInt16         aCommandType,
                            stlInt32         aXaError,
                            stlErrorStack  * aErrorStack,
                            sclEnv         * aEnv );

stlStatus sclReadXaRecoverCommand( sclHandle      * aHandle,
                                   stlInt64       * aCount,
                                   stlInt32       * aRmId,
                                   stlInt64       * aFlags,
                                   sclEnv         * aEnv );

stlStatus sclWriteXaRecoverResult( sclHandle      * aHandle,
                                   stlInt64         aCount,
                                   stlXid         * aXids,
                                   stlInt32         aXaError,
                                   stlErrorStack  * aErrorStack,
                                   sclEnv         * aEnv );

stlStatus sclPeriodEnd( sclHandle      * aHandle,
                        sclEnv         * aEnv );

stlStatus sclReadControlPacketCommand( sclHandle      * aHandle,
                                       stlInt8        * aControl,
                                       stlInt8        * aControlOp,
                                       sclEnv         * aEnv );

stlStatus sclWriteControlPacketResult( sclHandle      * aHandle,
                                       stlErrorStack  * aErrorStack,
                                       sclEnv         * aEnv );

/** @} */

#endif /* _SCLPROTOCOL_H_ */
