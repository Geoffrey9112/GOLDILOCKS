/*******************************************************************************
 * slp.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: slp.h 13675 2014-10-06 14:17:27Z lym999 $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _SLP_H_
#define _SLP_H_ 1

/**
 * @file slp.h
 * @brief Server Library protocol description
 */

/**
 * @defgroup slp Server Library protocol description
 * @{
 */

/** @} */

stlStatus slpCmdNone( sllCommandArgs * aArgs,
                      sllEnv         * aEnv );
stlStatus slpCmdConnect( sllCommandArgs * aArgs,
                         sllEnv         * aEnv );
stlStatus slpCmdDisconnect( sllCommandArgs * aArgs,
                            sllEnv         * aEnv );
stlStatus slpCmdGetConnAttr( sllCommandArgs * aArgs,
                             sllEnv         * aEnv );
stlStatus slpCmdSetConnAttr( sllCommandArgs * aArgs,
                             sllEnv         * aEnv );
stlStatus slpCmdFreeStmt( sllCommandArgs * aArgs,
                          sllEnv         * aEnv );
stlStatus slpCmdCancel( sllCommandArgs * aArgs,
                        sllEnv         * aEnv );
stlStatus slpCmdDescribeCol( sllCommandArgs * aArgs,
                             sllEnv         * aEnv );
stlStatus slpCmdParameterType( sllCommandArgs * aArgs,
                               sllEnv         * aEnv );
stlStatus slpCmdTransaction( sllCommandArgs * aArgs,
                             sllEnv         * aEnv );
stlStatus slpCmdPrepare( sllCommandArgs * aArgs,
                         sllEnv         * aEnv );
stlStatus slpCmdExecDirect( sllCommandArgs * aArgs,
                            sllEnv         * aEnv );
stlStatus slpCmdExecute( sllCommandArgs * aArgs,
                         sllEnv         * aEnv );
stlStatus slpCmdGetOutBindData( sllCommandArgs * aArgs,
                                sllEnv         * aEnv );
stlStatus slpCmdFetch( sllCommandArgs * aArgs,
                       sllEnv         * aEnv );
stlStatus slpCmdTargetType( sllCommandArgs * aArgs,
                            sllEnv         * aEnv );
stlStatus slpCmdBindType( sllCommandArgs * aArgs,
                          sllEnv         * aEnv );
stlStatus slpCmdBindData( sllCommandArgs * aArgs,
                          sllEnv         * aEnv );
stlStatus slpCmdGetStmtAttr( sllCommandArgs * aArgs,
                             sllEnv         * aEnv );
stlStatus slpCmdSetStmtAttr( sllCommandArgs * aArgs,
                             sllEnv         * aEnv );
stlStatus slpCmdNumParams( sllCommandArgs * aArgs,
                           sllEnv         * aEnv );
stlStatus slpCmdNumResultCols( sllCommandArgs * aArgs,
                               sllEnv         * aEnv );
stlStatus slpCmdGetCursorName( sllCommandArgs * aArgs,
                               sllEnv         * aEnv );
stlStatus slpCmdSetCursorName( sllCommandArgs * aArgs,
                               sllEnv         * aEnv );
stlStatus slpCmdSetPos( sllCommandArgs * aArgs,
                        sllEnv         * aEnv );
stlStatus slpCmdCloseCursor( sllCommandArgs * aArgs,
                             sllEnv         * aEnv );
stlStatus slpCmdStartUp( sllCommandArgs * aArgs,
                         sllEnv         * aEnv );
stlStatus slpCmdXaClose( sllCommandArgs * aArgs,
                         sllEnv         * aEnv );
stlStatus slpCmdXaCommit( sllCommandArgs * aArgs,
                          sllEnv         * aEnv );
stlStatus slpCmdXaRollback( sllCommandArgs * aArgs,
                            sllEnv         * aEnv );
stlStatus slpCmdXaPrepare( sllCommandArgs * aArgs,
                           sllEnv         * aEnv );
stlStatus slpCmdXaStart( sllCommandArgs * aArgs,
                         sllEnv         * aEnv );
stlStatus slpCmdXaEnd( sllCommandArgs * aArgs,
                       sllEnv         * aEnv );
stlStatus slpCmdXaForget( sllCommandArgs * aArgs,
                          sllEnv         * aEnv );
stlStatus slpCmdXaRecover( sllCommandArgs * aArgs,
                           sllEnv         * aEnv );
stlStatus slpCmdPeriod( sllCommandArgs * aArgs,
                        sllEnv         * aEnv );
stlStatus slpCmdControlPacket( sllCommandArgs * aArgs,
                               sllEnv         * aEnv );

/**
 * CALLBACK FUNCTIONS
 */

void slpCheckControlCallback( void           * aHandle,
                              stlInt64       * aStatementId,
                              stlInt8          aControl,
                              stlBool        * aIgnore,
                              stlBool        * aProtocolError,
                              void           * aEnv );

stlStatus slpGetBindValueCallback( void           * aHandle,
                                   stlInt64         aStatementId,
                                   stlInt32         aColumnNumber,
                                   stlInt8          aControl,
                                   dtlDataValue  ** aBindValue,
                                   void           * aEnv );

stlStatus slpReallocLongVaryingMemCallback( void           * aHandle,
                                            dtlDataValue   * aBindValue,
                                            void           * aCallbackContext,
                                            void           * aEnv );

stlStatus slpGetTargetType( void               * aHandle,
                            dtlValueBlockList  * aTargetColumn,
                            stlInt8            * aDataType,
                            stlInt8            * aStringLengthUnit,
                            stlInt8            * aIntervalIndicator,
                            stlInt64           * aArgNum1,
                            stlInt64           * aArgNum2,
                            void               * aEnv );
/** @} */

#endif /* _SLP_H_ */
