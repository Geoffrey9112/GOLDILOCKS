/*******************************************************************************
 * qlxInsertSelect.h
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

#ifndef _QLX_INSERT_SELECT_H_
#define _QLX_INSERT_SELECT_H_ 1

/**
 * @file qlxInsertSelect.h
 * @brief SQL Processor Layer Execution INSERT .. SELECT Statement
 */

#include <qlDef.h>


/**
 * @addtogroup qlxInsertSelect
 * @{
 */

stlStatus qlxInsertSelectGetValidPlan( smlTransId             aTransID,
                                       qllDBCStmt           * aDBCStmt,
                                       qllStatement         * aSQLStmt,
                                       qllQueryPhase          aQueryPhase,
                                       qlvInitInsertSelect ** aInitPlan,
                                       qllOptimizationNode ** aCodePlan,
                                       qllDataArea         ** aDataPlan,
                                       qllEnv               * aEnv );
                                 

/** @} qlxInsertSelect */

#endif /* _QLX_INSERT_SELECT_H_ */
