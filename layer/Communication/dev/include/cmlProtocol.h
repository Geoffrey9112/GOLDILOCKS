/*******************************************************************************
 * cmlProtocol.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: cmlProtocol.h 6534 2012-12-04 00:37:52Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _CMLPROTOCOL_H_
#define _CMLPROTOCOL_H_ 1

/**
 * @file cmlProtocol.h
 * @brief Communication Layer Protocol Routines
 */

/**
 * @defgroup cmlProtocol Protocol
 * @ingroup cmExternal
 * @{
 */

/*
 * HANDSHAKE PROTOCOL
 */

stlStatus cmlHandshakingForClient( cmlHandle     * aHandle,
                                   stlInt32      * aBufferSize,
                                   stlErrorStack * aErrorStack );

stlStatus cmlWriteVersion( cmlHandle      * aHandle,
                           stlErrorStack  * aErrorStack );

stlStatus cmlReadVersion( cmlHandle      * aHandle,
                          stlInt16       * aMajorVersion,
                          stlInt16       * aMinorVersion,
                          stlInt16       * aPatchVersion,
                          stlErrorStack  * aErrorStack );

stlStatus cmlWriteHandshakeCommand( cmlHandle      * aHandle,
                                    stlChar        * aRoleName,
                                    stlErrorStack  * aErrorStack );

stlStatus cmlReadHandshakeResult( cmlHandle      * aHandle,
                                  stlInt32       * aBufferSize,
                                  stlErrorStack  * aErrorStack );


stlStatus cmlPreHandshakeForClient( cmlHandle       * aHandle,
                                    cmlSessionType    aServerMode,
                                    stlErrorStack   * aErrorStack );

stlStatus cmlPreHandshakeForServer( stlContext    * aContext,
                                    stlErrorStack * aErrorStack );

stlStatus cmlWritePreHandshakeCommand( cmlHandle      * aHandle,
                                       cmlSessionType   aServerMode,
                                       stlErrorStack  * aErrorStack );

stlStatus cmlReadPreHandshake( cmlHandle      * aHandle,
                               cmlSessionType * aServerMode,
                               stlErrorStack  * aErrorStack );

stlStatus cmlWritePreHandshakeResult( cmlHandle     * aHandle,
                                      stlErrorStack * aErrorStack );

stlStatus cmlReadPreHandshakeResult( cmlHandle      * aHandle,
                                     stlErrorStack  * aErrorStack );





/*
 * CONNECT PROTOCOL
 */

stlStatus cmlWriteConnectCommand( cmlHandle       * aHandle,
                                  stlInt8           aControl,
                                  stlChar         * aUserName,
                                  stlChar         * aEncryptedPassword,
                                  stlChar         * aOldPassword,
                                  stlChar         * aNewPassword,
                                  stlChar         * aProgramName,
                                  stlInt32          aProcessId,
                                  stlInt16          aCharacterset,
                                  stlInt32          aTimezone,
                                  cmlDbcType        aDbcType,
                                  stlErrorStack   * aErrorStack );

stlStatus cmlReadConnectResult( cmlHandle       * aHandle,
                                stlUInt32       * aSessionId,
                                stlInt64        * aSessionSeq,
                                dtlCharacterSet * aCharacterset,
                                dtlCharacterSet * aNCharCharacterset,
                                stlInt32        * aDBTimezone,
                                stlErrorStack   * aErrorStack );


/*
 * DISCONNECT PROTOCOL
 */

stlStatus cmlWriteDisconnectCommand( cmlHandle       * aHandle,
                                     stlErrorStack   * aErrorStack );

stlStatus cmlReadDisconnectResult( cmlHandle      * aHandle,
                                   stlErrorStack  * aErrorStack );

/*
 * FETCHSCROLL PROTOCOL
 */

stlStatus cmlWriteFetchCommand( cmlHandle     * aHandle,
                                stlInt8         aControl,
                                stlInt64        aStatementId,
                                stlInt64        aOffset,
                                stlInt32        aRowCount,
                                stlErrorStack * aErrorStack );

stlStatus cmlReadFetchResult( cmlHandle                        * aHandle,
                              stlBool                          * aIgnored,
                              dtlValueBlockList                * aValueBlockList,
                              stlInt64                         * aFirstRowIdxOfRowset,
                              stlInt64                         * aLastRowIdx,
                              stlInt8                          * aStatus,
                              stlBool                          * aCursorClosed,
                              cmlReallocLongVaryingMemCallback   aAllocMemCallback,
                              void                             * aCallbackContext,
                              stlErrorStack                    * aErrorStack );


/*
 * GETCONNATTR PROTOCOL
 */

stlStatus cmlWriteGetConnAttrCommand( cmlHandle      * aHandle,
                                      stlInt8          aControl,
                                      stlInt32         aAttrType,
                                      stlErrorStack  * aErrorStack );

stlStatus cmlReadGetConnAttrResult( cmlHandle      * aHandle,
                                    stlBool        * aIgnored,
                                    stlInt32       * aAttrType,
                                    dtlDataType    * aAttrDataType,
                                    void           * aAttrValue,
                                    stlInt16       * aAttrValueSize,
                                    stlErrorStack  * aErrorStack );


/*
 * SETCONNATTR PROTOCOL
 */

stlStatus cmlWriteSetConnAttrCommand( cmlHandle      * aHandle,
                                      stlInt8          aControl,
                                      stlInt32         aAttrType,
                                      dtlDataType      aAttrDataType,
                                      void           * aAttrValue,
                                      stlInt32         aAttrValueSize,
                                      stlErrorStack  * aErrorStack );

stlStatus cmlReadSetConnAttrResult( cmlHandle      * aHandle,
                                    stlBool        * aIgnored,
                                    stlErrorStack  * aErrorStack );


/*
 * DESCRIBECOL PROTOCOL
 */

stlStatus cmlWriteDescribeColCommand( cmlHandle      * aHandle,
                                      stlInt8          aControl,
                                      stlInt64         aStatementId,
                                      stlInt32         aColumnNumber,
                                      stlErrorStack  * aErrorStack );

stlStatus cmlReadDescribeColResult( cmlHandle           * aHandle,
                                    stlBool             * aIgnored,
                                    stlChar             * aCatalogName,
                                    stlChar             * aSchemaName,
                                    stlChar             * aTableName,
                                    stlChar             * aBaseTableName,
                                    stlChar             * aColumnAliasName,
                                    stlChar             * aColumnLabel,
                                    stlChar             * aBaseColumnName,
                                    stlChar             * aLiteralPrefix,
                                    stlChar             * aLiteralSuffix,
                                    dtlDataType         * aDbType,
                                    stlInt16            * aIntervalCode,
                                    stlInt16            * aNumPrecRadix,
                                    stlInt16            * aDatetimeIntervPrec,
                                    stlInt64            * aDisplaySize,
                                    dtlStringLengthUnit * aStringLengthUnit,
                                    stlInt64            * aCharacterLength,
                                    stlInt64            * aOctetLength,
                                    stlInt32            * aPrecision,
                                    stlInt32            * aScale,
                                    stlBool             * aNullable,
                                    stlBool             * aAliasUnnamed,
                                    stlBool             * aCaseSensitive,
                                    stlBool             * aUnsigned,
                                    stlBool             * aFixedPrecScale,
                                    stlBool             * aUpdatable,
                                    stlBool             * aAutoUnique,
                                    stlBool             * aRowVersion,
                                    stlBool             * aAbleLike,
                                    stlErrorStack       * aErrorStack );

/*
 * PARAMETERTYPE PROTOCOL
 */
stlStatus cmlWriteParameterTypeCommand( cmlHandle     * aHandle,
                                        stlInt8         aControl,
                                        stlInt64        aStatementId,
                                        stlInt32        aParameterNumber,
                                        stlErrorStack * aErrorStack );

stlStatus cmlReadParameterTypeResult( cmlHandle     * aHandle,
                                      stlBool       * aIgnored,
                                      stlInt16      * aInputOutputType,
                                      stlErrorStack * aErrorStack );

/*
 * PREPARE PROTOCOL
 */

stlStatus cmlWritePrepareCommand( cmlHandle      * aHandle,
                                  stlInt8          aControl,
                                  stlInt64         aStatementId,
                                  stlChar        * aSqlText,
                                  stlInt64         aSqlTextSize,
                                  stlErrorStack  * aErrorStack );

stlStatus cmlReadPrepareResult( cmlHandle      * aHandle,
                                stlBool        * aIgnored,
                                stlInt64       * aStatementId,
                                stlInt32       * aStatementType,
                                stlBool        * aHasTrans,
                                stlBool        * aResultSet,
                                stlBool        * aIsWithoutHoldCursor,
                                stlBool        * aIsUpdatable,
                                stlInt8        * aSensitivity,
                                stlErrorStack  * aErrorStack );


/*
 * EXECDIRECT PROTOCOL
 */

stlStatus cmlWriteExecDirectCommand( cmlHandle      * aHandle,
                                     stlInt8          aControl,
                                     stlInt64         aStatementId,
                                     stlBool          aIsFirst,
                                     stlChar        * aSqlText,
                                     stlInt64         aSqlTextSize,
                                     stlErrorStack  * aErrorStack );

stlStatus cmlReadExecDirectResult( cmlHandle      * aHandle,
                                   stlBool        * aIgnored,
                                   stlInt64       * aStatementId,
                                   stlInt32       * aStatementType,
                                   stlInt64       * aAffectedRowCount,
                                   stlBool        * aRecompile,
                                   stlBool        * aResultSet,
                                   stlBool        * aIsWithoutHoldCursor,
                                   stlBool        * aIsUpdatable,
                                   stlInt8        * aSensitivity,
                                   stlBool        * aHasTrans,
                                   stlInt8        * aDclAttrCount,
                                   stlInt32       * aDclAttrType,
                                   dtlDataType    * aDclAttrDataType,
                                   stlChar        * aDclAttrValue,
                                   stlInt32       * aDclAttrValueSize,
                                   stlErrorStack  * aErrorStack );


/*
 * EXECUTE PROTOCOL
 */

stlStatus cmlWriteExecuteCommand( cmlHandle      * aHandle,
                                  stlInt8          aControl,
                                  stlInt64         aStatementId,
                                  stlBool          aIsFirst,
                                  stlErrorStack  * aErrorStack );

stlStatus cmlReadExecuteResult( cmlHandle      * aHandle,
                                stlBool        * aIgnored,
                                stlInt64       * aAffectedRowCount,
                                stlBool        * aRecompile,
                                stlBool        * aResultSet,
                                stlBool        * aHasTrans,
                                stlInt8        * aDclAttrCount,
                                stlInt32       * aDclAttrType,
                                dtlDataType    * aDclAttrDataType,
                                stlChar        * aDclAttrValue,
                                stlInt32       * aDclAttrValueSize,
                                stlErrorStack  * aErrorStack );


/*
 * GETOUTBINDDATA PROTOCOL
 */

stlStatus cmlWriteGetOutBindDataCommand( cmlHandle      * aHandle,
                                         stlInt8          aControl,
                                         stlInt64         aStatementId,
                                         stlInt32         aColumnNumber,
                                         stlErrorStack  * aErrorStack );

stlStatus cmlReadGetOutBindDataResult( cmlHandle                        * aHandle,
                                       stlBool                          * aIgnored,
                                       dtlDataValue                     * aBindValue,
                                       cmlReallocLongVaryingMemCallback   aReallocMemCallback,
                                       void                             * aCallbackContext,
                                       stlErrorStack                    * aErrorStack );


/*
 * FREESTMT PROTOCOL
 */

stlStatus cmlWriteFreeStmtCommand( cmlHandle      * aHandle,
                                   stlInt8          aControl,
                                   stlInt64         aStatementId,
                                   stlInt16         aOption,
                                   stlErrorStack  * aErrorStack );

stlStatus cmlReadFreeStmtResult( cmlHandle      * aHandle,
                                 stlBool        * aIgnored,
                                 stlErrorStack  * aErrorStack );


/*
 * TRANSACTION PROTOCOL
 */

stlStatus cmlWriteTransactionCommand( cmlHandle      * aHandle,
                                      stlInt8          aControl,
                                      stlInt16         aOption,
                                      stlErrorStack  * aErrorStack );

stlStatus cmlReadTransactionResult( cmlHandle      * aHandle,
                                    stlErrorStack  * aErrorStack );


/*
 * TARGETTYPE PROTOCOL
 */

stlStatus cmlWriteTargetTypeCommand( cmlHandle      * aHandle,
                                     stlInt8          aControl,
                                     stlInt64         aStatementId,
                                     stlErrorStack  * aErrorStack );

stlStatus cmlReadTargetTypeResult( cmlHandle        * aHandle,
                                   stlBool          * aIgnored,
                                   stlBool          * aChanged,
                                   stlInt32         * aTargetCount,
                                   cmlTargetType   ** aTargetTypes,
                                   stlErrorStack    * aErrorStack );


/*
 * BINDTYPE PROTOCOL
 */

stlStatus cmlWriteBindTypeCommand( cmlHandle      * aHandle,
                                   stlInt8          aControl,
                                   stlInt64         aStatementId,
                                   stlInt32         aColumnNumber,
                                   stlInt8          aBindType,
                                   stlInt8          aDataType,
                                   stlInt64         aArg1,
                                   stlInt64         aArg2,
                                   stlInt8          aStringLengthUnit,
                                   stlInt8          aIntervalIndicator,
                                   stlErrorStack  * aErrorStack );

stlStatus cmlReadBindTypeResult( cmlHandle      * aHandle,
                                 stlBool        * aIgnored,
                                 stlInt64       * aStatementId,
                                 stlErrorStack  * aErrorStack );

/*
 * BINDDATA PROTOCOL
 */

stlStatus cmlWriteBindDataBeginCommand( cmlHandle      * aHandle,
                                        stlInt8          aControl,
                                        stlInt64         aStatementId,
                                        stlInt32         aColumnCount,
                                        stlInt64         aArraySize,
                                        stlErrorStack  * aErrorStack );

stlStatus cmlWriteBindDataCommand( cmlHandle      * aHandle,
                                   stlBool          aOutBind,
                                   stlInt64         aArraySize,
                                   dtlDataValue   * aBindValue,
                                   stlErrorStack  * aErrorStack );

stlStatus cmlReadBindDataResult( cmlHandle      * aHandle,
                                 stlBool        * aIgnored,
                                 stlErrorStack  * aErrorStack );


/*
 * GETSTMTATTR PROTOCOL
 */

stlStatus cmlWriteGetStmtAttrCommand( cmlHandle      * aHandle,
                                      stlInt8          aControl,
                                      stlInt64         aStatementId,
                                      stlInt32         aAttrType,
                                      stlInt32         aStringBufferLength,
                                      stlErrorStack  * aErrorStack );

stlStatus cmlReadGetStmtAttrResult( cmlHandle      * aHandle,
                                    stlBool        * aIgnored,
                                    stlInt64       * aStatementId,
                                    stlInt32       * aAttrType,
                                    dtlDataType    * aAttrDataType,
                                    void           * aAttrValue,
                                    stlInt32       * aAttrValueSize,
                                    stlInt32       * aStringLength,
                                    stlErrorStack  * aErrorStack );


/*
 * SETSTMTATTR PROTOCOL
 */

stlStatus cmlWriteSetStmtAttrCommand( cmlHandle      * aHandle,
                                      stlInt8          aControl,
                                      stlInt64         aStatementId,
                                      stlInt32         aAttrType,
                                      dtlDataType      aAttrDataType,
                                      void           * aAttrValue,
                                      stlInt32         aAttrValueSize,
                                      stlErrorStack  * aErrorStack );

stlStatus cmlReadSetStmtAttrResult( cmlHandle     * aHandle,
                                    stlBool       * aIgnored,
                                    stlInt64      * aStatementId,
                                    stlErrorStack * aErrorStack );


/*
 * CLOSECURSOR PROTOCOL
 */

stlStatus cmlWriteCloseCursorCommand( cmlHandle      * aHandle,
                                      stlInt8          aControl,
                                      stlInt64         aStatementId,
                                      stlErrorStack  * aErrorStack );

stlStatus cmlReadCloseCursorResult( cmlHandle      * aHandle,
                                    stlBool        * aIgnored,
                                    stlErrorStack  * aErrorStack );


/*
 * CANCEL PROTOCOL
 */

stlStatus cmlWriteCancelCommand( cmlHandle      * aHandle,
                                 stlInt8          aControl,
                                 stlInt32         aSessionId,
                                 stlInt64         aSessionSeq,
                                 stlInt64         aStatementId,
                                 stlErrorStack  * aErrorStack );

stlStatus cmlReadCancelResult( cmlHandle      * aHandle,
                               stlBool        * aIgnored,
                               stlErrorStack  * aErrorStack );

/*
 * NUMPARAMS PROTOCOL
 */

stlStatus cmlWriteNumParamsCommand( cmlHandle      * aHandle,
                                    stlInt8          aControl,
                                    stlInt64         aStatementId,
                                    stlErrorStack  * aErrorStack );

stlStatus cmlReadNumParamsResult( cmlHandle      * aHandle,
                                  stlBool        * aIgnored,
                                  stlInt32       * aParamCnt,
                                  stlErrorStack  * aErrorStack );

/*
 * NUMRESULTCOLS PROTOCOL
 */

stlStatus cmlWriteNumResultColsCommand( cmlHandle      * aHandle,
                                        stlInt8          aControl,
                                        stlInt64         aStatementId,
                                        stlErrorStack  * aErrorStack );

stlStatus cmlReadNumResultColsResult( cmlHandle      * aHandle,
                                      stlBool        * aIgnored,
                                      stlInt32       * aColumnCnt,
                                      stlErrorStack  * aErrorStack );

/*
 * GETCURSORNAME PROTOCOL
 */

stlStatus cmlWriteGetCursorNameCommand( cmlHandle      * aHandle,
                                        stlInt8          aControl,
                                        stlInt64         aStatementId,
                                        stlErrorStack  * aErrorStack );

stlStatus cmlReadGetCursorNameResult( cmlHandle      * aHandle,
                                      stlBool        * aIgnored,
                                      stlInt64       * aStatementId,
                                      stlChar        * aCursorName,
                                      stlErrorStack  * aErrorStack );


/*
 * SETCURSORNAME PROTOCOL
 */

stlStatus cmlWriteSetCursorNameCommand( cmlHandle      * aHandle,
                                        stlInt8          aControl,
                                        stlInt64         aStatementId,
                                        stlChar        * aCursorName,
                                        stlErrorStack  * aErrorStack );

stlStatus cmlReadSetCursorNameResult( cmlHandle      * aHandle,
                                      stlBool        * aIgnored,
                                      stlInt64       * aStatementId,
                                      stlErrorStack  * aErrorStack );


/*
 * SETPOS PROTOCOL
 */

stlStatus cmlWriteSetPosCommand( cmlHandle      * aHandle,
                                 stlInt8          aControl,
                                 stlInt64         aStatementId,
                                 stlInt64         aRowNum,
                                 stlInt16         aOperation,
                                 stlErrorStack  * aErrorStack );

stlStatus cmlReadSetPosResult( cmlHandle      * aHandle,
                               stlBool        * aIgnored,
                               stlInt64       * aStatementId,
                               stlErrorStack  * aErrorStack );


/*
 * STARTUP PROTOCOL
 */

stlStatus cmlWriteStartupCommand( cmlHandle       * aHandle,
                                  stlInt8           aControl,
                                  stlErrorStack   * aErrorStack );

stlStatus cmlReadStartupResult( cmlHandle       * aHandle,
                                stlErrorStack   * aErrorStack );

/*
 * XA PROTOCOL
 */

stlStatus cmlWriteXaCommand( cmlHandle      * aHandle,
                             stlInt16         aCommandType,
                             stlXid         * aXid,
                             stlInt32         aRmId,
                             stlInt64         aFlags,
                             stlErrorStack  * aErrorStack );

stlStatus cmlWriteXaCloseCommand( cmlHandle      * aHandle,
                                  stlErrorStack  * aErrorStack );

stlStatus cmlReadXaResult( cmlHandle      * aHandle,
                           stlInt16       * aResultType,
                           stlInt32       * aXaError,
                           stlErrorStack  * aErrorStack );

stlStatus cmlWriteXaRecoverCommand( cmlHandle      * aHandle,
                                    stlInt64         aCount,
                                    stlInt32         aRmId,
                                    stlInt64         aFlags,
                                    stlErrorStack  * aErrorStack );

stlStatus cmlReadXaRecoverResult( cmlHandle      * aHandle,
                                  stlInt64       * aCount,
                                  stlXid         * aXids,
                                  stlInt32       * aXaError,
                                  stlErrorStack  * aErrorStack );

stlStatus cmlPeriodEnd( cmlHandle      * aHandle,
                        stlErrorStack  * aErrorStack );

stlStatus cmlWriteControlPacketCommand( cmlHandle      * aHandle,
                                        stlInt8          aControl,
                                        stlInt8          aControlOp,
                                        stlErrorStack  * aErrorStack );

stlStatus cmlReadControlPacketResult( cmlHandle      * aHandle,
                                      stlBool        * aIgnored,
                                      stlErrorStack  * aErrorStack );

stlStatus cmlWriteErrorResult( cmlHandle       * aHandle,
                               cmlCommandType    aResultType,
                               stlErrorStack   * aErrorStack );


/** @} */

#endif /* _CMLPROTOCOL_H_ */
