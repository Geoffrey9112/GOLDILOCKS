/*******************************************************************************
 * qlxInsertReturnInto.h
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

#ifndef _QLX_INSERT_RETURN_INTO_H_
#define _QLX_INSERT_RETURN_INTO_H_ 1

/**
 * @file qlxInsertReturnInto.h
 * @brief SQL Processor Layer : Execution of INSERT RETURNING INTO statement
 */

#include <qlDef.h>



/**
 * @addtogroup qlxInsertReturnInto
 * @{
 */

stlStatus qlxInsertReturnIntoGetValidPlan( smlTransId                 aTransID,
                                           qllDBCStmt               * aDBCStmt,
                                           qllStatement             * aSQLStmt,
                                           qllQueryPhase              aQueryPhase,
                                           qlvInitInsertReturnInto ** aInitPlan,
                                           qllOptimizationNode     ** aCodePlan,
                                           qllDataArea             ** aDataPlan,
                                           qllEnv                   * aEnv );

/** @} qlxInsertReturnInto */

#endif /* _QLX_INSERT_RETURN_INTO_H_ */
