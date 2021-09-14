/*******************************************************************************
 * qlxDelete.h
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

#ifndef _QLX_DELETE_H_
#define _QLX_DELETE_H_ 1

/**
 * @file qlxDelete.h
 * @brief SQL Processor Layer Execution DELETE Statement
 */

#include <qlDef.h>


/**
 * @addtogroup qlxDelete
 * @{
 */

stlStatus qlxDeleteGetValidPlan( smlTransId              aTransID,
                                 qllDBCStmt            * aDBCStmt,
                                 qllStatement          * aSQLStmt,
                                 qllQueryPhase           aQueryPhase,
                                 qlvInitDelete        ** aInitPlan,
                                 qllOptimizationNode  ** aCodePlan,
                                 qllDataArea          ** aDataPlan,
                                 qllEnv                * aEnv );

stlStatus qlxDeleteGetConstraintCheckTime( qlvInitDeleteRelObject * aInitRelObject,
                                           qllOptimizationNode    * aOptNode,
                                           qllDataArea            * aDataArea,
                                           qllEnv                 * aEnv );

stlStatus qlxDeleteResetConstraintCollision( qlvInitDeleteRelObject * aInitRelObject,
                                             qllOptimizationNode    * aOptNode,
                                             qllDataArea            * aDataArea,
                                             qllEnv                 * aEnv );
                                        
stlStatus qlxDeleteValidBlockRow( smlTransId          aTransID,
                                  smlStatement      * aStmt,
                                  qllDBCStmt        * aDBCStmt,
                                  qllStatement      * aSQLStmt,
                                  qllDataArea       * aDataArea,
                                  qllExecutionNode  * aExecNode,
                                  qllEnv            * aEnv );

stlStatus qlxDeleteIntegrityCheckConstraint( smlTransId         aTransID,
                                             smlStatement     * aStmt,
                                             qllDataArea      * aDataArea,
                                             qllExecutionNode * aExecNode,
                                             qllEnv           * aEnv );

stlStatus qlxDeleteIntegrityKeyConstIndexKey( smlTransId          aTransID,
                                              smlStatement      * aStmt,
                                              qllDataArea       * aDataArea,
                                              qllExecutionNode  * aExecNode,
                                              qllEnv            * aEnv );

stlStatus qlxDeleteCheckStmtCollision( smlTransId          aTransID,
                                       smlStatement      * aStmt,
                                       qllDataArea       * aDataArea,
                                       qllExecutionNode  * aExecNode,
                                       qllEnv            * aEnv );


/** @} qlxDelete */

#endif /* _QLX_DELETE_H_ */
