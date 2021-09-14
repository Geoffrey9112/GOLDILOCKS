/*******************************************************************************
 * qlxSelect.h
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

#ifndef _QLX_SELECT_H_
#define _QLX_SELECT_H_ 1

/**
 * @file qlxSelect.h
 * @brief SQL Processor Layer Execution SELECT Statement
 */

#include <qlDef.h>


/**
 * @addtogroup qlxSelect
 * @{
 */

stlStatus qlxExecuteSubquery( smlTransId       aTransID,
                              smlStatement   * aStmt,
                              qllDBCStmt     * aDBCStmt,
                              qlvInitSelect  * aInitPlan,
                              qlnoSelectStmt * aCodePlan,
                              qllDataArea    * aDataPlan,
                              qllEnv         * aEnv );

stlStatus qlxSelectGetValidPlan( smlTransId              aTransID,
                                 qllDBCStmt            * aDBCStmt,
                                 qllStatement          * aSQLStmt,
                                 qllQueryPhase           aQueryPhase,
                                 qlvInitSelect        ** aInitPlan,
                                 qllOptimizationNode  ** aCodePlan,
                                 qllDataArea          ** aDataPlan,
                                 qllEnv                * aEnv );

stlStatus qlxBuildMaterialResultSet4ForUpdate( smlTransId                aTransID,
                                               smlStatement            * aStmt,
                                               qllDBCStmt              * aDBCStmt,
                                               qllStatement            * aSQLStmt,
                                               qlvSerialAction           aSerialAction,
                                               qllResultSetDesc        * aResultSetDesc,
                                               qllEnv                  * aEnv );

stlStatus qlxBuildMaterialResultSet4ForReadOnly( smlTransId                aTransID,
                                                 smlStatement            * aStmt,
                                                 qllDBCStmt              * aDBCStmt,
                                                 qllStatement            * aSQLStmt,
                                                 qllResultSetDesc        * aResultSetDesc,
                                                 qllEnv                  * aEnv );


/** @} qlxSelect */

#endif /* _QLX_SELECT_H_ */
