/*******************************************************************************
 * qlxInsert.h
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

#ifndef _QLX_INSERT_H_
#define _QLX_INSERT_H_ 1

/**
 * @file qlxInsert.h
 * @brief SQL Processor Layer Execution INSERT Statement
 */

#include <qlDef.h>


/**
 * @addtogroup qlxInsert
 * @{
 */

stlStatus qlxInsertGetValidPlan( smlTransId             aTransID,
                                 qllDBCStmt           * aDBCStmt,
                                 qllStatement         * aSQLStmt,
                                 qllQueryPhase          aQueryPhase,
                                 qlvInitInsertValues ** aInitPlan,
                                 qllOptimizationNode ** aCodePlan,
                                 qllDataArea         ** aDataPlan,
                                 qllEnv               * aEnv );

stlStatus qlxInsertGetConstraintCheckTime(  qlvInitInsertRelObject * aInitRelObject,
                                            qllOptimizationNode    * aOptNode,
                                            qllDataArea            * aDataArea,
                                            qllEnv                 * aEnv );


stlStatus qlxInsertIntegrityCheckConst( smlTransId               aTransID,
                                        qllStatement           * aSQLStmt,
                                        ellDictHandle          * aTableHandle,
                                        qlvInitInsertRelObject * aIniRelObject,
                                        qlxExecInsertRelObject * aExeRelObject,
                                        qllEnv                 * aEnv );

stlStatus qlxInsertIntegrityKeyConstIndexKey( smlTransId               aTransID,
                                              smlStatement           * aStmt,
                                              ellDictHandle          * aTableHandle,
                                              qlvInitInsertRelObject * aIniRelObject,
                                              qlxExecInsertRelObject * aExeRelObject,
                                              smlRowBlock            * aRowBlock,
                                              qllEnv                 * aEnv );


stlStatus qlxInsertCheckStmtCollision( smlTransId          aTransID,
                                       smlStatement      * aStmt,
                                       qllDataArea       * aDataArea,
                                       qllExecutionNode  * aExecNode,
                                       qllEnv            * aEnv );

/** @} qlxInsert */

#endif /* _QLX_INSERT_H_ */
