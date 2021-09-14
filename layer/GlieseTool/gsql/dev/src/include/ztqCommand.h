/*******************************************************************************
 * ztqCommand.h
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


#ifndef _ZTQCOMMAND_H_
#define _ZTQCOMMAND_H_ 1

/**
 * @file ztqCommand.h
 * @brief Command Definition
 */

/**
 * @defgroup ztqCommand Command
 * @ingroup ztq
 * @{
 */

stlStatus ztqCmdQuit( ztqParseTree  * aParseTree,
                      stlChar       * aCmdStr,
                      stlAllocator  * aAllocator,
                      stlErrorStack * aErrorStack );

stlStatus ztqCmdHelp( ztqParseTree  * aParseTree,
                      stlChar       * aCmdStr,
                      stlAllocator  * aAllocator,
                      stlErrorStack * aErrorStack );

stlStatus ztqCmdDesc( ztqParseTree  * aParseTree,
                      stlChar       * aCmdStr,
                      stlAllocator  * aAllocator,
                      stlErrorStack * aErrorStack );

stlStatus ztqCmdIdesc( ztqParseTree  * aParseTree,
                       stlChar       * aCmdStr,
                       stlAllocator  * aAllocator,
                       stlErrorStack * aErrorStack );

stlStatus ztqCmdSetCallstack( ztqParseTree  * aParseTree,
                              stlChar       * aCmdStr,
                              stlAllocator  * aAllocator,
                              stlErrorStack * aErrorStack );

stlStatus ztqCmdSetVertical( ztqParseTree  * aParseTree,
                             stlChar       * aCmdStr,
                             stlAllocator  * aAllocator,
                             stlErrorStack * aErrorStack );

stlStatus ztqCmdSetTiming( ztqParseTree  * aParseTree,
                           stlChar       * aCmdStr,
                           stlAllocator  * aAllocator,
                           stlErrorStack * aErrorStack );

stlStatus ztqCmdSetVerbose( ztqParseTree  * aParseTree,
                            stlChar       * aCmdStr,
                            stlAllocator  * aAllocator,
                            stlErrorStack * aErrorStack );

stlStatus ztqCmdSetAutocommit( ztqParseTree  * aParseTree,
                               stlChar       * aCmdStr,
                               stlAllocator  * aAllocator,
                               stlErrorStack * aErrorStack );

stlStatus ztqCmdSetResult( ztqParseTree  * aParseTree,
                           stlChar       * aCmdStr,
                           stlAllocator  * aAllocator,
                           stlErrorStack * aErrorStack );

stlStatus ztqCmdSetColor( ztqParseTree  * aParseTree,
                          stlChar       * aCmdStr,
                          stlAllocator  * aAllocator,
                          stlErrorStack * aErrorStack );

stlStatus ztqCmdSetError( ztqParseTree  * aParseTree,
                          stlChar       * aCmdStr,
                          stlAllocator  * aAllocator,
                          stlErrorStack * aErrorStack );

stlStatus ztqCmdSetLinesize( ztqParseTree  * aParseTree,
                             stlChar       * aCmdStr,
                             stlAllocator  * aAllocator,
                             stlErrorStack * aErrorStack );

stlStatus ztqCmdSetPagesize( ztqParseTree  * aParseTree,
                             stlChar       * aCmdStr,
                             stlAllocator  * aAllocator,
                             stlErrorStack * aErrorStack );

stlStatus ztqCmdSetColsize( ztqParseTree  * aParseTree,
                            stlChar       * aCmdStr,
                            stlAllocator  * aAllocator,
                            stlErrorStack * aErrorStack );

stlStatus ztqCmdSetNumsize( ztqParseTree  * aParseTree,
                            stlChar       * aCmdStr,
                            stlAllocator  * aAllocator,
                            stlErrorStack * aErrorStack );

stlStatus ztqCmdSetDdlsize( ztqParseTree  * aParseTree,
                            stlChar       * aCmdStr,
                            stlAllocator  * aAllocator,
                            stlErrorStack * aErrorStack );

stlStatus ztqCmdSetHistory( ztqParseTree  * aParseTree,
                            stlChar       * aCmdStr,
                            stlAllocator  * aAllocator,
                            stlErrorStack * aErrorStack );

stlStatus ztqCmdVar( ztqParseTree  * aParseTree,
                     stlChar       * aCmdStr,
                     stlAllocator  * aAllocator,
                     stlErrorStack * aErrorStack );

stlStatus ztqCmdPrint( ztqParseTree  * aParseTree,
                       stlChar       * aCmdStr,
                       stlAllocator  * aAllocator,
                       stlErrorStack * aErrorStack );

stlStatus ztqCmdExecAssign( ztqParseTree  * aParseTree,
                            stlChar       * aCmdStr,
                            stlAllocator  * aAllocator,
                            stlErrorStack * aErrorStack );

stlStatus ztqCmdExecSql( ztqParseTree  * aParseTree,
                         stlChar       * aCmdStr,
                         stlAllocator  * aAllocator,
                         stlErrorStack * aErrorStack );

stlStatus ztqCmdExecPrepared( ztqParseTree  * aParseTree,
                              stlChar       * aCmdStr,
                              stlAllocator  * aAllocator,
                              stlErrorStack * aErrorStack );

stlStatus ztqCmdPrepareSql( ztqParseTree  * aParseTree,
                            stlChar       * aCmdStr,
                            stlAllocator  * aAllocator,
                            stlErrorStack * aErrorStack );

stlStatus ztqCmdDynamicSql( ztqParseTree  * aParseTree,
                            stlChar       * aCmdStr,
                            stlAllocator  * aAllocator,
                            stlErrorStack * aErrorStack );

stlStatus ztqCmdExecHistory( ztqParseTree  * aParseTree,
                             stlChar       * aCmdStr,
                             stlAllocator  * aAllocator,
                             stlErrorStack * aErrorStack );

stlStatus ztqCmdPrintHistory( ztqParseTree  * aParseTree,
                              stlChar       * aCmdStr,
                              stlAllocator  * aAllocator,
                              stlErrorStack * aErrorStack );

stlStatus ztqCmdImport( ztqParseTree  * aParseTree,
                        stlChar       * aCmdStr,
                        stlAllocator  * aAllocator,
                        stlErrorStack * aErrorStack );

stlStatus ztqCmdExplainPlan( ztqParseTree  * aParseTree,
                             stlChar       * aCmdStr,
                             stlAllocator  * aAllocator,
                             stlErrorStack * aErrorStack );

stlStatus ztqCmdAllocStmt( ztqParseTree  * aParseTree,
                           stlChar       * aCmdStr,
                           stlAllocator  * aAllocator,
                           stlErrorStack * aErrorStack );

stlStatus ztqCmdFreeStmt( ztqParseTree  * aParseTree,
                          stlChar       * aCmdStr,
                          stlAllocator  * aAllocator,
                          stlErrorStack * aErrorStack );

stlStatus ztqCmdUseStmtExecSql( ztqParseTree  * aParseTree,
                                stlChar       * aCmdStr,
                                stlAllocator  * aAllocator,
                                stlErrorStack * aErrorStack );

stlStatus ztqCmdUseStmtPrepareSql( ztqParseTree  * aParseTree,
                                   stlChar       * aCmdStr,
                                   stlAllocator  * aAllocator,
                                   stlErrorStack * aErrorStack );


stlStatus ztqCmdUseStmtExecPrepared( ztqParseTree  * aParseTree,
                                     stlChar       * aCmdStr,
                                     stlAllocator  * aAllocator,
                                     stlErrorStack * aErrorStack );

stlStatus ztqCmdStartup( ztqParseTree  * aParseTree,
                         stlChar       * aCmdStr,
                         stlAllocator  * aAllocator,
                         stlErrorStack * aErrorStack );

stlStatus ztqCmdShutdown( ztqParseTree  * aParseTree,
                          stlChar       * aCmdStr,
                          stlAllocator  * aAllocator,
                          stlErrorStack * aErrorStack );

stlStatus ztqCmdConnect( ztqParseTree  * aParseTree,
                         stlChar       * aCmdStr,
                         stlAllocator  * aAllocator,
                         stlErrorStack * aErrorStack );

stlStatus ztqCmdXaOpen( ztqParseTree  * aParseTree,
                        stlChar       * aCmdStr,
                        stlAllocator  * aAllocator,
                        stlErrorStack * aErrorStack );

stlStatus ztqCmdXaClose( ztqParseTree  * aParseTree,
                         stlChar       * aCmdStr,
                         stlAllocator  * aAllocator,
                         stlErrorStack * aErrorStack );

stlStatus ztqCmdXaStart( ztqParseTree  * aParseTree,
                         stlChar       * aCmdStr,
                         stlAllocator  * aAllocator,
                         stlErrorStack * aErrorStack );

stlStatus ztqCmdXaEnd( ztqParseTree  * aParseTree,
                       stlChar       * aCmdStr,
                       stlAllocator  * aAllocator,
                       stlErrorStack * aErrorStack );

stlStatus ztqCmdXaCommit( ztqParseTree  * aParseTree,
                          stlChar       * aCmdStr,
                          stlAllocator  * aAllocator,
                          stlErrorStack * aErrorStack );

stlStatus ztqCmdXaRollback( ztqParseTree  * aParseTree,
                            stlChar       * aCmdStr,
                            stlAllocator  * aAllocator,
                            stlErrorStack * aErrorStack );

stlStatus ztqCmdXaPrepare( ztqParseTree  * aParseTree,
                           stlChar       * aCmdStr,
                           stlAllocator  * aAllocator,
                           stlErrorStack * aErrorStack );

stlStatus ztqCmdXaForget( ztqParseTree  * aParseTree,
                          stlChar       * aCmdStr,
                          stlAllocator  * aAllocator,
                          stlErrorStack * aErrorStack );

stlStatus ztqCmdXaRecover( ztqParseTree  * aParseTree,
                           stlChar       * aCmdStr,
                           stlAllocator  * aAllocator,
                           stlErrorStack * aErrorStack );

stlStatus ztqPrintXaRecover( XID           * aXids,
                             stlInt64        aXidCount,
                             stlAllocator  * aAllocator,
                             stlErrorStack * aErrorStack );

stlStatus ztqCmdPrintDatabaseDDL( ztqParseTree  * aParseTree,
                                  stlChar       * aCmdStr,
                                  stlAllocator  * aAllocator,
                                  stlErrorStack * aErrorStack );

stlStatus ztqCmdPrintSpaceDDL( ztqParseTree  * aParseTree,
                               stlChar       * aCmdStr,
                               stlAllocator  * aAllocator,
                               stlErrorStack * aErrorStack );

stlStatus ztqCmdPrintProfileDDL( ztqParseTree  * aParseTree,
                                 stlChar       * aCmdStr,
                                 stlAllocator  * aAllocator,
                                 stlErrorStack * aErrorStack );

stlStatus ztqCmdPrintAuthDDL( ztqParseTree  * aParseTree,
                              stlChar       * aCmdStr,
                              stlAllocator  * aAllocator,
                              stlErrorStack * aErrorStack );

stlStatus ztqCmdPrintSchemaDDL( ztqParseTree  * aParseTree,
                                stlChar       * aCmdStr,
                                stlAllocator  * aAllocator,
                                stlErrorStack * aErrorStack );

stlStatus ztqCmdPrintPublicSynonymDDL( ztqParseTree  * aParseTree,
                                       stlChar       * aCmdStr,
                                       stlAllocator  * aAllocator,
                                       stlErrorStack * aErrorStack );

stlStatus ztqCmdPrintTableDDL( ztqParseTree  * aParseTree,
                               stlChar       * aCmdStr,
                               stlAllocator  * aAllocator,
                               stlErrorStack * aErrorStack );

stlStatus ztqCmdPrintConstraintDDL( ztqParseTree  * aParseTree,
                                    stlChar       * aCmdStr,
                                    stlAllocator  * aAllocator,
                                    stlErrorStack * aErrorStack );

stlStatus ztqCmdPrintIndexDDL( ztqParseTree  * aParseTree,
                               stlChar       * aCmdStr,
                               stlAllocator  * aAllocator,
                               stlErrorStack * aErrorStack );

stlStatus ztqCmdPrintViewDDL( ztqParseTree  * aParseTree,
                              stlChar       * aCmdStr,
                              stlAllocator  * aAllocator,
                              stlErrorStack * aErrorStack );

stlStatus ztqCmdPrintSequenceDDL( ztqParseTree  * aParseTree,
                                  stlChar       * aCmdStr,
                                  stlAllocator  * aAllocator,
                                  stlErrorStack * aErrorStack );

stlStatus ztqCmdPrintSynonymDDL( ztqParseTree  * aParseTree,
                                 stlChar       * aCmdStr,
                                 stlAllocator  * aAllocator,
                                 stlErrorStack * aErrorStack );

stlStatus ztqCmdEdit( ztqParseTree  * aParseTree,
                      stlChar       * aCmdStr,
                      stlAllocator  * aAllocator,
                      stlErrorStack * aErrorStack );

stlStatus ztqCmdSpool( ztqParseTree  * aParseTree,
                       stlChar       * aCmdStr,
                       stlAllocator  * aAllocator,
                       stlErrorStack * aErrorStack );

/** @} */

#endif /* _ZTQCOMMAND_H_ */
