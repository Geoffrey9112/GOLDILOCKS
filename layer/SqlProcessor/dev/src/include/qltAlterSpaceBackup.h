/*******************************************************************************
 * qltAlterSpaceBackup.h
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

#ifndef _QLT_ALTER_SPACE_BACKUP_H_
#define _QLT_ALTER_SPACE_BACKUP_H_ 1

/**
 * @file qltAlterSpaceBackup.h
 * @brief ALTER TABLESPACE .. BACKUP
 */

/**
 * @brief ALTER TABLESPACE BACKUP 를 위한 Init Plan
 */
typedef struct qltInitAlterSpaceBackup
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */
    
    ellDictHandle     mSpaceHandle;     /**< Tablespace Handle */
    stlChar           mBackupType;      /**< FULL / INCREMENTAL */
    stlChar           mCommand;         /**< Backup Type (BEGIN/END) */
    stlChar           mOption;          /**< DIFFERENTIAL / CUMULATIVE */
    stlInt16          mLevel;           /**< Incremental Backup Level */
} qltInitAlterSpaceBackup;

stlStatus qltValidateAlterSpaceBackup( smlTransId      aTransID,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       stlChar       * aSQLString,
                                       qllNode       * aParseTree,
                                       qllEnv        * aEnv );


stlStatus qltOptCodeAlterSpaceBackup( smlTransId      aTransID,
                                      qllDBCStmt    * aDBCStmt,
                                      qllStatement  * aSQLStmt,
                                      qllEnv        * aEnv );

stlStatus qltOptDataAlterSpaceBackup( smlTransId      aTransID,
                                      qllDBCStmt    * aDBCStmt,
                                      qllStatement  * aSQLStmt,
                                      qllEnv        * aEnv );

stlStatus qltExecuteAlterSpaceBackup( smlTransId      aTransID,
                                      smlStatement  * aStmt,
                                      qllDBCStmt    * aDBCStmt,
                                      qllStatement  * aSQLStmt,
                                      qllEnv        * aEnv );

#endif /* _QLT_ALTER_SPACE_BACKUP_H_ */
