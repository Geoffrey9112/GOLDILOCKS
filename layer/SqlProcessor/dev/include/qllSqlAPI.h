/*******************************************************************************
 * qllSqlAPI.h
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

#ifndef _QLL_SQL_API_H_
#define _QLL_SQL_API_H_ 1

/**
 * @file qllSqlAPI.h
 * @brief SQL API
 */

/*
 * Parsing
 */

stlStatus qllParseSQL( qllDBCStmt    * aDBCStmt,
                       qllStatement  * aSQLStmt,
                       stlChar       * aSQLString,
                       qllNode      ** aParseTree,
                       qllEnv        * aEnv );

/*
 * Validation
 */

stlStatus qllValidateSQL( smlTransId      aTransID,
                          qllDBCStmt    * aDBCStmt,
                          qllStatement  * aSQLStmt,
                          stlChar       * aSQLString,
                          qllNode       * aParseTree,
                          qllEnv        * aEnv );

/*
 * Binding
 */

void qllSetBindContext( qllStatement   * aSQLStmt,
                        knlBindContext * aBindContext,
                        qllEnv         * aEnv );

/*
 * Optimization
 */


stlStatus qllOptimizeCodeSQL( smlTransId      aTransID,
                              qllDBCStmt    * aDBCStmt,
                              qllStatement  * aSQLStmt,
                              qllEnv        * aEnv );

stlStatus qllOptimizeDataSQL( smlTransId      aTransID,
                              qllDBCStmt    * aDBCStmt,
                              qllStatement  * aSQLStmt,
                              qllEnv        * aEnv );

                                     
    
/*
 * Execution
 */

stlBool qllHasPlan( qllStatement  * aSQLStmt );

stlStatus qllRecheckPrivSQL( smlTransId      aTransID,
                             smlStatement  * aStmt,
                             qllDBCStmt    * aDBCStmt,
                             qllStatement  * aSQLStmt,
                             qllEnv        * aEnv );

stlStatus qllExecuteSQL( smlTransId      aTransID,
                         smlStatement  * aStmt,
                         qllDBCStmt    * aDBCStmt,
                         qllStatement  * aSQLStmt,
                         qllEnv        * aEnv );

stlStatus qllGetAffectedRowCnt( qllStatement  * aSQLStmt,
                                stlInt64      * aRowCnt,
                                qllEnv        * aEnv );

/*
 * Recompile
 */

stlStatus qllRecompileSQL( smlTransId      aTransID,
                           qllDBCStmt    * aDBCStmt,
                           qllStatement  * aSQLStmt,
                           qllQueryPhase   aQueryPhase,
                           qllEnv        * aEnv );

stlBool qllIsRecompileAndReset( qllStatement * aSQLStmt );


/*
 * Explain Plan
 */

stlStatus qllExplainSQL( smlTransId      aTransID,
                         qllDBCStmt    * aDBCStmt,
                         qllStatement  * aSQLStmt,
                         stlBool         aIsVerbose,
                         qllEnv        * aEnv );

stlStatus qllTraceExplainSQL( smlTransId      aTransID,
                              qllDBCStmt    * aDBCStmt,
                              qllStatement  * aSQLStmt,
                              qllEnv        * aEnv );

stlStatus qllTraceExplainErrorSQL( smlTransId     aTransID,
                                   qllDBCStmt   * aDBCStmt,
                                   qllStatement * aSQLStmt,
                                   qllEnv       * aEnv );

stlStatus qllTraceExplainSQL_Internal( smlTransId     aTransID,
                                       qllDBCStmt   * aDBCStmt,
                                       qllStatement * aSQLStmt,
                                       stlBool        aIsErrorSQL,
                                       qllEnv       * aEnv );

#endif /* _QLL_SQL_API_H_ */

