/*******************************************************************************
 * qlxUpdate.h
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

#ifndef _QLX_UPDATE_H_
#define _QLX_UPDATE_H_ 1

/**
 * @file qlxUpdate.h
 * @brief SQL Processor Layer Execution UPDATE Statement
 */

#include <qlDef.h>


/**
 * @addtogroup qlxUpdate
 * @{
 */

stlStatus qlxUpdateGetValidPlan( smlTransId              aTransID,
                                 qllDBCStmt            * aDBCStmt,
                                 qllStatement          * aSQLStmt,
                                 qllQueryPhase           aQueryPhase,
                                 qlvInitUpdate        ** aInitPlan,
                                 qllOptimizationNode  ** aCodePlan,
                                 qllDataArea          ** aDataPlan,
                                 qllEnv                * aEnv );

stlStatus qlxUpdateGetConstraintCheckTime(  qlvInitUpdateRelObject * aInitRelObject,
                                            qllOptimizationNode    * aOptNode,
                                            qllDataArea            * aDataArea,
                                            qllEnv                 * aEnv );


stlStatus qlxUpdateResetConstraintCollision( qlvInitUpdateRelObject * aInitRelObject,
                                             qllOptimizationNode    * aOptNode,
                                             qllDataArea            * aDataArea,
                                             qllEnv                 * aEnv );

stlStatus qlxUpdateValidBlockRow( smlTransId             aTransID,
                                  smlStatement         * aStmt,
                                  qllDBCStmt           * aDBCStmt,
                                  qllStatement         * aSQLStmt,
                                  qllDataArea          * aDataArea,
                                  qllExecutionNode     * aExecNode,
                                  qllEnv               * aEnv );

stlStatus qlxUpdateIntegrityCheckConst( smlTransId          aTransID,
                                        qllStatement      * aSQLStmt,
                                        qllDataArea       * aDataArea,
                                        qllExecutionNode  * aExecNode,
                                        qllEnv            * aEnv );

stlStatus qlxUpdateIntegrityKeyConstIndexKey( smlTransId          aTransID,
                                              smlStatement      * aStmt,
                                              qllDataArea       * aDataArea,
                                              qllExecutionNode  * aExecNode,
                                              qllEnv            * aEnv );

stlStatus qlxUpdateCheckStmtCollision( smlTransId         aTransID,
                                       smlStatement     * aStmt,
                                       qllStatement     * aSQLStmt,
                                       qllDataArea      * aDataArea,
                                       qllExecutionNode * aExecNode,
                                       qllEnv           * aEnv );


/** @} qlxUpdate */

#endif /* _QLX_UPDATE_H_ */
