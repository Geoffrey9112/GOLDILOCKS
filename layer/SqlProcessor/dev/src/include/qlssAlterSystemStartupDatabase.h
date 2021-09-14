/*******************************************************************************
 * qlssAlterSystemStartupDatabase.h
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

#ifndef _QLSS_ALTER_SYSTEM_STARTUP_DATABASE_H_
#define _QLSS_ALTER_SYSTEM_STARTUP_DATABASE_H_ 1

/**
 * @file qlssAlterSystemStartupDatabase.h
 * @brief SystemStartup Control Language
 */


/**
 * @brief ALTER SYSTEM {MOUNT|OPEN} DATABASE Init Plan
 */
typedef struct qlssInitSystemStartupDatabase
{
    knlStartupPhase    mStartupPhase;       /**< MOUNT/OPEN */
    smlDataAccessMode  mOpenOption;         /**< NONE/READ_ONLY/READ_WRITE */
    stlChar            mLogOption;          /**< NORESETLOGS/RESETLOGS */
} qlssInitSystemStartupDatabase;



stlStatus qlssValidateSystemStartupDatabase( smlTransId      aTransID,
                                             qllDBCStmt    * aDBCStmt,
                                             qllStatement  * aSQLStmt,
                                             stlChar       * aSQLString,
                                             qllNode       * aParseTree,
                                             qllEnv        * aEnv );

stlStatus qlssOptCodeSystemStartupDatabase( smlTransId      aTransID,
                                            qllDBCStmt    * aDBCStmt,
                                            qllStatement  * aSQLStmt,
                                            qllEnv        * aEnv );

stlStatus qlssOptDataSystemStartupDatabase( smlTransId      aTransID,
                                            qllDBCStmt    * aDBCStmt,
                                            qllStatement  * aSQLStmt,
                                            qllEnv        * aEnv );

stlStatus qlssExecuteSystemStartupDatabase( smlTransId      aTransID,
                                            smlStatement  * aStmt,
                                            qllDBCStmt    * aDBCStmt,
                                            qllStatement  * aSQLStmt,
                                            qllEnv        * aEnv );

#endif /* _QLSS_ALTER_SYSTEM_STARTUP_DATABASE_H_ */

