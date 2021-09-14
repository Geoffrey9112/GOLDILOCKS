/*******************************************************************************
 * qlssAlterSystemSwitchLogfile.h
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

#ifndef _QLSS_ALTER_SYSTEM_SWITCH_LOGFILE_STMT_H_
#define _QLSS_ALTER_SYSTEM_SWITCH_LOGFILE_STMT_H_ 1

/**
 * @file qlssAlterSystemSwitchLogfile.h
 * @brief ALTER SYSTEM SWITCH LOGFILE Statement
 */

/**
 * @brief Init Plan of ALTER SYSTEM SWITCH LOGFILE Statement
 */
/**
 * @brief ALTER TABLESPACE SWITCH LOGFILE 를 위한 Init Plan
 */
typedef struct qlssInitSwitchLogfile
{
    stlInt64   mDummy;      /**< prepare/execution 의 유효성 검사를 위한 dummy plan */
} qlssInitSwitchLogfile;


stlStatus qlssValidateAlterSystemSwitchLogfile( smlTransId      aTransID,
                                                qllDBCStmt    * aDBCStmt,
                                                qllStatement  * aSQLStmt,
                                                stlChar       * aSQLString,
                                                qllNode       * aParseTree,
                                                qllEnv        * aEnv );

stlStatus qlssOptCodeAlterSystemSwitchLogfile( smlTransId      aTransID,
                                               qllDBCStmt    * aDBCStmt,
                                               qllStatement  * aSQLStmt,
                                               qllEnv        * aEnv );

stlStatus qlssOptDataAlterSystemSwitchLogfile( smlTransId      aTransID,
                                               qllDBCStmt    * aDBCStmt,
                                               qllStatement  * aSQLStmt,
                                               qllEnv        * aEnv );

stlStatus qlssExecuteAlterSystemSwitchLogfile( smlTransId      aTransID,
                                               smlStatement  * aStmt,
                                               qllDBCStmt    * aDBCStmt,
                                               qllStatement  * aSQLStmt,
                                               qllEnv        * aEnv );

#endif /* _QLSS_ALTER_SYSTEM_SWITCH_LOGFILE_STMT_H_ */


