/*******************************************************************************
 * qlssAlterSystemCleanupPlan.h
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

#ifndef _QLSS_ALTER_SYSTEM_CLEANUP_PLAN_H_
#define _QLSS_ALTER_SYSTEM_CLEANUP_PLAN_H_ 1

/**
 * @file qlssAlterSystemCleanupPlan.h
 * @brief System Control Language
 */

/**
 * @brief ALTER SYSTEM CLEANUP PLAN Init Plan
 */
typedef struct qlssInitSystemCleanupPlan
{
    stlInt64   mDummy;      /**< prepare/execution 의 유효성 검사를 위한 dummy plan */
} qlssInitSystemCleanupPlan;

stlStatus qlssValidateSystemCleanupPlan( smlTransId      aTransID,
                                         qllDBCStmt    * aDBCStmt,
                                         qllStatement  * aSQLStmt,
                                         stlChar       * aSQLString,
                                         qllNode       * aParseTree,
                                         qllEnv        * aEnv );

stlStatus qlssOptCodeSystemCleanupPlan( smlTransId      aTransID,
                                        qllDBCStmt    * aDBCStmt,
                                        qllStatement  * aSQLStmt,
                                        qllEnv        * aEnv );

stlStatus qlssOptDataSystemCleanupPlan( smlTransId      aTransID,
                                        qllDBCStmt    * aDBCStmt,
                                        qllStatement  * aSQLStmt,
                                        qllEnv        * aEnv );

stlStatus qlssExecuteSystemCleanupPlan( smlTransId      aTransID,
                                        smlStatement  * aStmt,
                                        qllDBCStmt    * aDBCStmt,
                                        qllStatement  * aSQLStmt,
                                        qllEnv        * aEnv );



#endif /* _QLSS_ALTER_SYSTEM_CLEANUP_PLAN_H_ */

