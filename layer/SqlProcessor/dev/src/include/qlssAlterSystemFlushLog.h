/*******************************************************************************
 * qlssAlterSystemFlushLog.h
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

#ifndef _QLSS_ALTER_SYSTEM_FLUSH_LOG_H_
#define _QLSS_ALTER_SYSTEM_FLUSH_LOG_H_ 1

/**
 * @file qlssAlterSystemFlushLog.h
 * @brief System Control Language
 */

/**
 * @brief ALTER SYSTEM FLUSH LOG Init Plan
 */
typedef struct qlssInitSystemFlushLog
{
    qlpFlushLogBehavior   mBehavior;  /**< FLUSH LOGS, START/STOP FLUSH LOGS */
} qlssInitSystemFlushLog;

stlStatus qlssValidateSystemFlushLog( smlTransId      aTransID,
                                      qllDBCStmt    * aDBCStmt,
                                      qllStatement  * aSQLStmt,
                                      stlChar       * aSQLString,
                                      qllNode       * aParseTree,
                                      qllEnv        * aEnv );

stlStatus qlssOptCodeSystemFlushLog( smlTransId      aTransID,
                                     qllDBCStmt    * aDBCStmt,
                                     qllStatement  * aSQLStmt,
                                     qllEnv        * aEnv );

stlStatus qlssOptDataSystemFlushLog( smlTransId      aTransID,
                                     qllDBCStmt    * aDBCStmt,
                                     qllStatement  * aSQLStmt,
                                     qllEnv        * aEnv );

stlStatus qlssExecuteSystemFlushLog( smlTransId      aTransID,
                                     smlStatement  * aStmt,
                                     qllDBCStmt    * aDBCStmt,
                                     qllStatement  * aSQLStmt,
                                     qllEnv        * aEnv );



#endif /* _QLSS_ALTER_SYSTEM_FLUSH_LOG_H_ */

