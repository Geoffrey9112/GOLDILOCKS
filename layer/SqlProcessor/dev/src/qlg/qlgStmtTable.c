/*******************************************************************************
 * glgStmtTable.c
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

/**
 * @file qlgStmtTable.c
 * @brief SQL Statement Table
 */


#include <qll.h>

#include <qlDef.h>

/**
 * @addtogroup qlgStmtTable
 * @{
 */

    
qlgStmtTable  gSQLStmtTable[QLL_STMT_MAX - QLL_STMT_NA] =
{
    {
        QLL_STMT_NA,                                 /* INVALID Statement */
        "N/A",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_NONE,
        KNL_STARTUP_PHASE_NONE,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_ALTER_DATABASE_ADD_LOGFILE_GROUP_TYPE,    /* ALTER DATABASE ADD LOGFILE GROUP */
        "ALTER DATABASE ADD LOGFILE GROUP",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_NO          |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_YES       |
          QLL_STMT_ATTR_SUPPORT_CDS_NO       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_MOUNT,          
        KNL_STARTUP_PHASE_MOUNT,
        qldValidateAlterDatabaseAddLogfileGroup,
        qldOptCodeAlterDatabaseAddLogfileGroup,
        qldOptDataAlterDatabaseAddLogfileGroup,
        qldExecuteAlterDatabaseAddLogfileGroup
    },
    {
        QLL_STMT_ALTER_DATABASE_ADD_LOGFILE_MEMBER_TYPE, /* ALTER DATABASE ADD LOGFILE MEMBER */
        "ALTER DATABASE ADD LOGFILE MEMBER",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_NO          |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_YES       |
          QLL_STMT_ATTR_SUPPORT_CDS_NO       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_MOUNT,          
        KNL_STARTUP_PHASE_MOUNT,
        qldValidateAlterDatabaseAddLogfileMember,
        qldOptCodeAlterDatabaseAddLogfileMember,
        qldOptDataAlterDatabaseAddLogfileMember,
        qldExecuteAlterDatabaseAddLogfileMember
    },
    {
        QLL_STMT_ALTER_DATABASE_DROP_LOGFILE_GROUP_TYPE,   /* ALTER DATABASE DROP LOGFILE GROUP */
        "ALTER DATABASE DROP LOGFILE GROUP",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_NO          |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_YES       |
          QLL_STMT_ATTR_SUPPORT_CDS_NO       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_MOUNT,          
        KNL_STARTUP_PHASE_MOUNT,
        qldValidateAlterDatabaseDropLogfileGroup,
        qldOptCodeAlterDatabaseDropLogfileGroup,
        qldOptDataAlterDatabaseDropLogfileGroup,
        qldExecuteAlterDatabaseDropLogfileGroup
    },
    {
        QLL_STMT_ALTER_DATABASE_DROP_LOGFILE_MEMBER_TYPE,   /* ALTER DATABASE DROP LOGFILE MEMBER */
        "ALTER DATABASE DROP LOGFILE MEMBER",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_NO          |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_YES       |
          QLL_STMT_ATTR_SUPPORT_CDS_NO       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_MOUNT,          
        KNL_STARTUP_PHASE_MOUNT,
        qldValidateAlterDatabaseDropLogfileMember,
        qldOptCodeAlterDatabaseDropLogfileMember,
        qldOptDataAlterDatabaseDropLogfileMember,
        qldExecuteAlterDatabaseDropLogfileMember
    },
    {
        QLL_STMT_ALTER_DATABASE_RENAME_LOGFILE_TYPE, /* ALTER DATABASE RENAME LOGFILE */        
        "ALTER DATABASE RENAME LOGFILE",
        ( QLL_STMT_ATTR_DB_ACCESS_NONE       |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_NO          |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_YES       |
          QLL_STMT_ATTR_SUPPORT_CDS_NO       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_MOUNT,          
        KNL_STARTUP_PHASE_MOUNT,
        qldValidateAlterDatabaseRenameLogfile,
        qldOptCodeAlterDatabaseRenameLogfile,
        qldOptDataAlterDatabaseRenameLogfile,
        qldExecuteAlterDatabaseRenameLogfile
    },
    {
        QLL_STMT_ALTER_DATABASE_ARCHIVELOG_MODE_TYPE,     /* ALTER DATABASE ARCHIVELOG */        
        "ALTER DATABASE ARCHIVELOG",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_YES       |
          QLL_STMT_ATTR_SUPPORT_CDS_NO       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_MOUNT,          
        KNL_STARTUP_PHASE_MOUNT,
        qldValidateAlterDatabaseArchivelogMode,
        qldOptCodeAlterDatabaseArchivelogMode,
        qldOptDataAlterDatabaseArchivelogMode,
        qldExecuteAlterDatabaseArchivelogMode
    },
    {
        QLL_STMT_ALTER_DATABASE_NOARCHIVELOG_MODE_TYPE,     /* ALTER DATABASE NOARCHIVELOG */        
        "ALTER DATABASE NOARCHIVELOG",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_YES       |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_MOUNT,          
        KNL_STARTUP_PHASE_MOUNT,
        qldValidateAlterDatabaseArchivelogMode,
        qldOptCodeAlterDatabaseArchivelogMode,
        qldOptDataAlterDatabaseArchivelogMode,
        qldExecuteAlterDatabaseArchivelogMode
    },
    {
        QLL_STMT_ALTER_DATABASE_BACKUP_TYPE,   /* ALTER DATABASE BACKUP .. */        
        "ALTER DATABASE BACKUP .. ",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_NO               |
          QLL_STMT_ATTR_LOCKABLE_NO          |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_NO       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,
        KNL_STARTUP_PHASE_OPEN,
        qldValidateAlterDatabaseBackup,
        qldOptCodeAlterDatabaseBackup,
        qldOptDataAlterDatabaseBackup,
        qldExecuteAlterDatabaseBackup
    },
    {
        QLL_STMT_ALTER_DATABASE_DELETE_BACKUP_LIST_TYPE,   /* ALTER DATABASE DELETE BACKUP LIST .. */
        "ALTER DATABASE DELETE BACKUP LIST .. ",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_NO               |
          QLL_STMT_ATTR_LOCKABLE_NO          |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_NO       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_MOUNT,
        KNL_STARTUP_PHASE_OPEN,
        qldValidateAlterDatabaseDeleteBackupList,
        qldOptCodeAlterDatabaseDeleteBackupList,
        qldOptDataAlterDatabaseDeleteBackupList,
        qldExecuteAlterDatabaseDeleteBackupList
    },
    {
        QLL_STMT_ALTER_DATABASE_BACKUP_INCREMENTAL_TYPE,   /* ALTER DATABASE BACKUP INCREMENTAL .. */
        "ALTER DATABASE BACKUP INCREMENTAL .. ",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_NO               |
          QLL_STMT_ATTR_LOCKABLE_NO          |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_NO       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,
        KNL_STARTUP_PHASE_OPEN,
        qldValidateAlterDatabaseBackup,
        qldOptCodeAlterDatabaseBackup,
        qldOptDataAlterDatabaseBackup,
        qldExecuteAlterDatabaseBackup
    },
    {
        QLL_STMT_ALTER_DATABASE_BACKUP_TYPE,   /* ALTER DATABASE BACKUP CONTROLFILE TO */        
        "ALTER DATABASE BACKUP CONTROLFILE TO",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_NO               |
          QLL_STMT_ATTR_LOCKABLE_NO          |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_NO       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,
        KNL_STARTUP_PHASE_OPEN,
        qldValidateAlterDatabaseBackupCtrlfile,
        qldOptCodeAlterDatabaseBackupCtrlfile,
        qldOptDataAlterDatabaseBackupCtrlfile,
        qldExecuteAlterDatabaseBackupCtrlfile
    },
    {
        QLL_STMT_ALTER_DATABASE_IRRECOVERABLE_TYPE,   /* ALTER DATABASE REGISTER IRRECOVERABLE SEGMENT */        
        "ALTER DATABASE REGISTER IRRECOVERABLE SEGMENT",
        ( QLL_STMT_ATTR_DB_ACCESS_NONE       |
          QLL_STMT_ATTR_DDL_NO               |
          QLL_STMT_ATTR_LOCKABLE_NO          |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_NO       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_MOUNT,          
        KNL_STARTUP_PHASE_MOUNT,
        qldValidateAlterDatabaseIrrecoverable,
        qldOptCodeAlterDatabaseIrrecoverable,
        qldOptDataAlterDatabaseIrrecoverable,
        qldExecuteAlterDatabaseIrrecoverable
    },
    {
        QLL_STMT_ALTER_DATABASE_RECOVER_DATABASE_TYPE,       /* ALTER DATABASE RECOVER */
        "ALTER DATABASE RECOVER",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_NO               |
          QLL_STMT_ATTR_LOCKABLE_NO          |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_YES       |
          QLL_STMT_ATTR_SUPPORT_CDS_NO       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_MOUNT,          
        KNL_STARTUP_PHASE_MOUNT,
        qldValidateAlterDatabaseRecover,
        qldOptCodeAlterDatabaseRecover,
        qldOptDataAlterDatabaseRecover,
        qldExecuteAlterDatabaseRecover
    },
    {
        QLL_STMT_ALTER_DATABASE_INCOMPLETE_RECOVERY_TYPE,  /* ALTER DATABASE INCOMPLETE RECOVERY */
        "ALTER DATABASE INCOMPLETE RECOVERY",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_NO               |
          QLL_STMT_ATTR_LOCKABLE_NO          |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_YES       |
          QLL_STMT_ATTR_SUPPORT_CDS_NO       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_MOUNT,          
        KNL_STARTUP_PHASE_MOUNT,
        qldValidateAlterDatabaseIncompleteRecover,
        qldOptCodeAlterDatabaseIncompleteRecover,
        qldOptDataAlterDatabaseIncompleteRecover,
        qldExecuteAlterDatabaseIncompleteRecover
    },
    {
        QLL_STMT_ALTER_DATABASE_RECOVER_TABLESPACE_TYPE,   /* ALTER DATABASE RECOVER TABLESPACE .. */
        "ALTER DATABASE RECOVER TABLESPACE ..",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_NO               |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_YES       |
          QLL_STMT_ATTR_SUPPORT_CDS_NO       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_MOUNT,          
        KNL_STARTUP_PHASE_OPEN,
        qldValidateAlterDatabaseRecoverTablespace,
        qldOptCodeAlterDatabaseRecoverTablespace,
        qldOptDataAlterDatabaseRecoverTablespace,
        qldExecuteAlterDatabaseRecoverTablespace
    },
    {
        QLL_STMT_ALTER_DATABASE_RECOVER_DATAFILE_TYPE, /* ALTER DATABASE RECOVER DATAFILE  */
        "ALTER DATABASE RECOVER DATAFILE ..",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_NO               |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_AUTOCOMMIT_YES       |
          QLL_STMT_ATTR_SUPPORT_CDS_NO       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_MOUNT,          
        KNL_STARTUP_PHASE_OPEN,
        qldValidateAlterDatabaseRecoverDatafile,
        qldOptCodeAlterDatabaseRecoverDatafile,
        qldOptDataAlterDatabaseRecoverDatafile,
        qldExecuteAlterDatabaseRecoverDatafile
    },
    {
        QLL_STMT_ALTER_DATABASE_RESTORE_DATABASE_TYPE,       /* ALTER DATABASE RESTORE */
        "ALTER DATABASE RESTORE",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_NO               |
          QLL_STMT_ATTR_LOCKABLE_NO          |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_NO       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_MOUNT,          
        KNL_STARTUP_PHASE_MOUNT,
        qldValidateAlterDatabaseRestore,
        qldOptCodeAlterDatabaseRestore,
        qldOptDataAlterDatabaseRestore,
        qldExecuteAlterDatabaseRestore
    },
    {
        QLL_STMT_ALTER_DATABASE_RESTORE_TABLESPACE_TYPE,   /* ALTER DATABASE RESTORE TABLESPACE .. */
        "ALTER DATABASE RESTORE TABLESPACE ..",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_NO               |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_YES       |
          QLL_STMT_ATTR_SUPPORT_CDS_NO       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_MOUNT,          
        KNL_STARTUP_PHASE_OPEN,
        qldValidateAlterDatabaseRestoreTablespace,
        qldOptCodeAlterDatabaseRestoreTablespace,
        qldOptDataAlterDatabaseRestoreTablespace,
        qldExecuteAlterDatabaseRestoreTablespace
    },
    {
        QLL_STMT_ALTER_DATABASE_RESTORE_CTRLFILE_TYPE,  /* ALTER DATABASE RESTORE CONTROLFILE FROM */
        "ALTER DATABASE RESTORE CONTROLFILE FROM ..",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_NO               |
          QLL_STMT_ATTR_LOCKABLE_NO          |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_NO       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        qldValidateAlterDatabaseRestoreCtrlfile,
        qldOptCodeAlterDatabaseRestoreCtrlfile,
        qldOptDataAlterDatabaseRestoreCtrlfile,
        qldExecuteAlterDatabaseRestoreCtrlfile
    },
    {
        QLL_STMT_ALTER_DATABASE_CLEAR_PASSWORD_HISTORY_TYPE,  /* ALTER DATABASE CLEAR PASSWORD HISTORY */
        "ALTER DATABASE CLEAR PASSWORD HISTORY",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qldValidateAlterDatabaseClearPasswordHistory,
        qldOptCodeAlterDatabaseClearPasswordHistory,
        qldOptDataAlterDatabaseClearPasswordHistory,
        qldExecuteAlterDatabaseClearPasswordHistory
    },
    {
        QLL_STMT_ALTER_DOMAIN_TYPE,                  /* ALTER DOMAIN */
        "ALTER DOMAIN",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_ALTER_FUNCTION_TYPE,                /* ALTER FUNCTION */
        "ALTER FUNCTION",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_ALTER_INDEX,                        /* ALTER INDEX */
        "ALTER INDEX",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_ALTER_METHOD_TYPE,                  /* ALTER METHOD */
        "ALTER METHOD",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_ALTER_PROCEDURE_TYPE,               /* ALTER PROCEDURE */
        "ALTER PROCEDURE",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_ALTER_PROFILE_TYPE,                 /* ALTER PROFILE */
        "ALTER PROFILE",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlaValidateAlterProfile,
        qlaOptCodeAlterProfile,
        qlaOptDataAlterProfile,
        qlaExecuteAlterProfile
    },
    {
        QLL_STMT_ALTER_ROUTINE_TYPE,                /* ALTER ROUTINE */
        "ALTER ROUTINE",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_ALTER_SEQUENCE_TYPE,                /* ALTER SEQUENCE */
        "ALTER SEQUENCE",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlqValidateAlterSeq,
        qlqOptCodeAlterSeq,
        qlqOptDataAlterSeq,
        qlqExecuteAlterSeq
    },
    {
        QLL_STMT_ALTER_SESSION_SET_TYPE,             /* ALTER SESSION SET */
        "ALTER SESSION SET property",
        ( QLL_STMT_ATTR_SUPPORT_GLOBAL_YES   |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_PLAN_CACHE_NO ),
        KNL_STARTUP_PHASE_MOUNT,
        KNL_STARTUP_PHASE_OPEN,
        qlssValidateSessionSet,
        qlssOptCodeSessionSet,
        qlssOptDataSessionSet,
        qlssExecuteSessionSet
    },
    {
        QLL_STMT_ALTER_SYSTEM_AGER_TYPE,             /* ALTER SYSTEM AGER */
        "ALTER SYSTEM AGER",
        ( QLL_STMT_ATTR_SUPPORT_GLOBAL_YES   |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_PLAN_CACHE_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlssValidateSystemAger,
        qlssOptCodeSystemAger,
        qlssOptDataSystemAger,
        qlssExecuteSystemAger
    },
    {
        QLL_STMT_ALTER_SYSTEM_BREAKPOINT_TYPE,        /* ALTER SYSTEM BEAKPOINT */
        "ALTER SYSTEM BREAKPOINT",
        ( QLL_STMT_ATTR_SUPPORT_GLOBAL_YES   |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_PLAN_CACHE_NO ),
        KNL_STARTUP_PHASE_NO_MOUNT,          
        KNL_STARTUP_PHASE_OPEN,
        qlssValidateSystemBreakpoint,
        qlssOptCodeSystemBreakpoint,
        qlssOptDataSystemBreakpoint,
        qlssExecuteSystemBreakpoint
    },
    {
        QLL_STMT_ALTER_SYSTEM_SESS_NAME_TYPE,        /* ALTER SYSTEM SESSION NAME */
        "ALTER SYSTEM SESSION NAME",
        ( QLL_STMT_ATTR_SUPPORT_GLOBAL_YES   |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_PLAN_CACHE_NO ),
        KNL_STARTUP_PHASE_NO_MOUNT,          
        KNL_STARTUP_PHASE_OPEN,
        qlssValidateSystemSessName,
        qlssOptCodeSystemSessName,
        qlssOptDataSystemSessName,
        qlssExecuteSystemSessName
    },
    {
        QLL_STMT_ALTER_SYSTEM_STARTUP_DATABASE_TYPE,   /* ALTER SYSTEM {MOUNT|OPEN} DATABASE */
        "ALTER SYSTEM { MOUNT | OPEN } DATABASE",
        ( QLL_STMT_ATTR_NOTHING              |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_YES ),
        KNL_STARTUP_PHASE_NO_MOUNT,          
        KNL_STARTUP_PHASE_MOUNT,
        qlssValidateSystemStartupDatabase,
        qlssOptCodeSystemStartupDatabase,
        qlssOptDataSystemStartupDatabase,
        qlssExecuteSystemStartupDatabase
    },
    {
        QLL_STMT_ALTER_SYSTEM_SHUTDOWN_DATABASE_ABORT_TYPE,   /* ALTER SYSTEM CLOSE DATABASE ABORT */
        "ALTER SYSTEM CLOSE DATABASE ABORT",
        ( QLL_STMT_ATTR_NOTHING            |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO  |
          QLL_STMT_ATTR_PLAN_CACHE_NO      |
          QLL_STMT_ATTR_AUTOCOMMIT_NO      |
          QLL_STMT_ATTR_SUPPORT_CDS_YES    |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_YES ),
        KNL_STARTUP_PHASE_NO_MOUNT,          
        KNL_STARTUP_PHASE_OPEN,
        qlssValidateSystemShutdownDatabase,
        qlssOptCodeSystemShutdownDatabase,
        qlssOptDataSystemShutdownDatabase,
        qlssExecuteSystemShutdownDatabase
    },
    {
        QLL_STMT_ALTER_SYSTEM_SHUTDOWN_DATABASE_TYPE,   /* ALTER SYSTEM CLOSE DATABASE */
        "ALTER SYSTEM CLOSE DATABASE",
        ( QLL_STMT_ATTR_NOTHING            |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO  |
          QLL_STMT_ATTR_PLAN_CACHE_NO      |
          QLL_STMT_ATTR_AUTOCOMMIT_YES     |
          QLL_STMT_ATTR_SUPPORT_CDS_YES    |
          QLL_STMT_ATTR_STATE_TRANS_YES ),
        KNL_STARTUP_PHASE_NO_MOUNT,          
        KNL_STARTUP_PHASE_OPEN,
        qlssValidateSystemShutdownDatabase,
        qlssOptCodeSystemShutdownDatabase,
        qlssOptDataSystemShutdownDatabase,
        qlssExecuteSystemShutdownDatabase
    },
    {
        QLL_STMT_ALTER_SYSTEM_SWITCH_LOGFILE_TYPE,      /* ALTER SYSTEM SWITCH LOGFILE */        
        "ALTER SYSTEM SWITCH LOGFILE",
        ( QLL_STMT_ATTR_DB_ACCESS_NONE       |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_NO          |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_YES       |
          QLL_STMT_ATTR_SUPPORT_CDS_NO       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_MOUNT,          
        KNL_STARTUP_PHASE_OPEN,
        qlssValidateAlterSystemSwitchLogfile,
        qlssOptCodeAlterSystemSwitchLogfile,
        qlssOptDataAlterSystemSwitchLogfile,
        qlssExecuteAlterSystemSwitchLogfile
    },
    {
        QLL_STMT_ALTER_SYSTEM_CHECKPOINT_TYPE,       /* ALTER SYSTEM CHECKPOINT */
        "ALTER SYSTEM CHECKPOINT",
        ( QLL_STMT_ATTR_SUPPORT_GLOBAL_YES   |
          QLL_STMT_ATTR_SUPPORT_CDS_NO       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlssValidateSystemChkpt,
        qlssOptCodeSystemChkpt,
        qlssOptDataSystemChkpt,
        qlssExecuteSystemChkpt
    },
    {
        QLL_STMT_ALTER_SYSTEM_DISCONNECT_SESSION_TYPE,        /* ALTER SYSTEM DISCONNECT SESSION */
        "ALTER SYSTEM DISCONNECT SESSION",
        ( QLL_STMT_ATTR_SUPPORT_GLOBAL_YES   |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO ),
        KNL_STARTUP_PHASE_MOUNT,   
        KNL_STARTUP_PHASE_OPEN,
        qlssValidateSystemDisconnectSession,
        qlssOptCodeSystemDisconnectSession,
        qlssOptDataSystemDisconnectSession,
        qlssExecuteSystemDisconnectSession
    },
    {
        QLL_STMT_ALTER_SYSTEM_FLUSH_LOG_TYPE,        /* ALTER SYSTEM FLUSH LOG */
        "ALTER SYSTEM FLUSH LOG",
        ( QLL_STMT_ATTR_SUPPORT_GLOBAL_YES   |
          QLL_STMT_ATTR_SUPPORT_CDS_NO       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlssValidateSystemFlushLog,
        qlssOptCodeSystemFlushLog,
        qlssOptDataSystemFlushLog,
        qlssExecuteSystemFlushLog
    },
    {
        QLL_STMT_ALTER_SYSTEM_CLEANUP_PLAN_TYPE,        /* ALTER SYSTEM CLEANUP PLAN */
        "ALTER SYSTEM CLEANUP PLAN",
        ( QLL_STMT_ATTR_SUPPORT_GLOBAL_YES   |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlssValidateSystemCleanupPlan,
        qlssOptCodeSystemCleanupPlan,
        qlssOptDataSystemCleanupPlan,
        qlssExecuteSystemCleanupPlan
    },
    {
        QLL_STMT_ALTER_SYSTEM_CLEANUP_SESSION_TYPE,        /* ALTER SYSTEM CLEANUP SESSION */
        "ALTER SYSTEM CLEANUP SESSION",
        ( QLL_STMT_ATTR_SUPPORT_GLOBAL_YES   |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlssValidateSystemCleanupSession,
        qlssOptCodeSystemCleanupSession,
        qlssOptDataSystemCleanupSession,
        qlssExecuteSystemCleanupSession
    },
    {
        QLL_STMT_ALTER_SYSTEM_KILL_SESSION_TYPE,     /* ALTER SYSTEM KILL SESSION Statement */
        "ALTER SYSTEM KILL SESSION",
        ( QLL_STMT_ATTR_SUPPORT_GLOBAL_YES   |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO ),
        KNL_STARTUP_PHASE_MOUNT,   
        KNL_STARTUP_PHASE_OPEN,
        qlssValidateSystemKillSession,
        qlssOptCodeSystemKillSession,
        qlssOptDataSystemKillSession,
        qlssExecuteSystemKillSession
    },
    {
        QLL_STMT_ALTER_SYSTEM_RESET_TYPE,            /* ALTER SYSTEM RESET */
        "ALTER SYSTEM RESET",
        ( QLL_STMT_ATTR_SUPPORT_GLOBAL_YES   |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
           QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO ),
        KNL_STARTUP_PHASE_MOUNT,   
        KNL_STARTUP_PHASE_OPEN,
        qlssValidateSystemReset,
        qlssOptCodeSystemReset,
        qlssOptDataSystemReset,
        qlssExecuteSystemReset
    },
    {
        QLL_STMT_ALTER_SYSTEM_SET_TYPE,              /* ALTER SYSTEM SET */
        "ALTER SYSTEM SET property",
        ( QLL_STMT_ATTR_SUPPORT_GLOBAL_YES   |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO ),
        KNL_STARTUP_PHASE_NO_MOUNT,   
        KNL_STARTUP_PHASE_OPEN,
        qlssValidateSystemSet,
        qlssOptCodeSystemSet,
        qlssOptDataSystemSet,
        qlssExecuteSystemSet
    },
    {
        QLL_STMT_ALTER_SYSTEM_SET_UNDO_MEM_TBS_TYPE, /* ALTER SYSTEM SET UNDO_MEMORY_TBS */
        "ALTER SYSTEM SET UNDO_MEMORY_TBS",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_NONE,
        KNL_STARTUP_PHASE_NONE,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_ALTER_TABLE_ADD_COLUMN_TYPE,        /* ALTER TABLE .. ADD COLUMN */
        "ALTER TABLE .. ADD COLUMN",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlrValidateAddColumn,
        qlrOptCodeAddColumn,
        qlrOptDataAddColumn,
        qlrExecuteAddColumn
    },
    {
        QLL_STMT_ALTER_TABLE_SET_UNUSED_COLUMN_TYPE,       /* ALTER TABLE .. SET UNUSED COLUMN */
        "ALTER TABLE .. SET UNUSED COLUMN",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlrValidateDropColumn,
        qlrOptCodeDropColumn,
        qlrOptDataDropColumn,
        qlrExecuteDropColumn
    },
    {
        QLL_STMT_ALTER_TABLE_DROP_COLUMN_TYPE,       /* ALTER TABLE .. DROP COLUMN */
        "ALTER TABLE .. DROP COLUMN",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlrValidateDropColumn,
        qlrOptCodeDropColumn,
        qlrOptDataDropColumn,
        qlrExecuteDropColumn
    },
    {
        QLL_STMT_ALTER_TABLE_DROP_UNUSED_COLUMNS_TYPE,   /* ALTER TABLE .. DROP UNUSED COLUMNS */
        "ALTER TABLE .. DROP UNUSED COLUMNS",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlrValidateDropColumn,
        qlrOptCodeDropColumn,
        qlrOptDataDropColumn,
        qlrExecuteDropColumn
    },
    {
        QLL_STMT_ALTER_TABLE_ALTER_COLUMN_SET_DEFAULT_TYPE,    /* ALTER TABLE .. ALTER COLUMN .. SET DEFAULT */
        "ALTER TABLE .. ALTER COLUMN .. SET DEFAULT",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlrValidateAlterColumn,
        qlrOptCodeAlterColumn,
        qlrOptDataAlterColumn,
        qlrExecuteAlterColumn  
    },
    {
        QLL_STMT_ALTER_TABLE_ALTER_COLUMN_DROP_DEFAULT_TYPE,   /* ALTER TABLE .. ALTER COLUMN .. DROP DEFAULT */
        "ALTER TABLE .. ALTER COLUMN .. DROP DEFAULT",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlrValidateAlterColumn,
        qlrOptCodeAlterColumn,
        qlrOptDataAlterColumn,
        qlrExecuteAlterColumn
    },
    {
        QLL_STMT_ALTER_TABLE_ALTER_COLUMN_SET_NOT_NULL_TYPE,   /* ALTER TABLE .. ALTER COLUMN .. SET NOT NULL */
        "ALTER TABLE .. ALTER COLUMN .. SET NOT NULL",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlrValidateAlterColumn,
        qlrOptCodeAlterColumn,
        qlrOptDataAlterColumn,
        qlrExecuteAlterColumn
    },
    {
        QLL_STMT_ALTER_TABLE_ALTER_COLUMN_SET_NOT_NULL_TYPE,   /* ALTER TABLE .. ALTER COLUMN .. DROP NOT NULL */
        "ALTER TABLE .. ALTER COLUMN .. DROP NOT NULL",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlrValidateAlterColumn,
        qlrOptCodeAlterColumn,
        qlrOptDataAlterColumn,
        qlrExecuteAlterColumn
    },
    {
        /*
         * SET DATA TYPE 은 Auto-Commit DDL 이다.
         * 이는 다음과 같이 fixed => variable 로 변경시 index 의 fix key 와 var key 가 혼재되어 key order 가 잘못되기 때문이다.
         * 1) CHAR(3) : 'A  '
         * 2) DDL : CHAR(3) => VARCHAR(5)
         * 3) DML : INSERT 'A' : varchar 의 key order 는 'A'(RID:10) < 'A  '(RID:5) 이다.
         * 4) Rollback
         * 5) char 의 key order 는 non-aged deleted key 'A' 와 'A  ' 는 동일한 값이기 때문에 'A  '(RID:5) < 'A'(RID:10) 이 되어야 한다.
         * 6) DDL => DML => Rollback 이 발생하면 index 의 key order 가 잘못된다.
         */
        QLL_STMT_ALTER_TABLE_ALTER_COLUMN_SET_DATA_TYPE_TYPE,  /* ALTER TABLE .. ALTER COLUMN .. SET DATA TYPE */
        "ALTER TABLE .. ALTER COLUMN .. SET DATA TYPE",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_YES       |   /* 주석 참조 */
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlrValidateAlterColumn,
        qlrOptCodeAlterColumn,
        qlrOptDataAlterColumn,
        qlrExecuteAlterColumn
    },
    {
        QLL_STMT_ALTER_TABLE_ALTER_COLUMN_ALTER_IDENTITY_TYPE, /* ALTER TABLE .. ALTER COLUMN .. AS IDENTITY */
        "ALTER TABLE .. ALTER COLUMN .. AS IDENTITY",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlrValidateAlterColumn,
        qlrOptCodeAlterColumn,
        qlrOptDataAlterColumn,
        qlrExecuteAlterColumn
    },
    {
        QLL_STMT_ALTER_TABLE_ALTER_COLUMN_DROP_IDENTITY_TYPE,  /* ALTER TABLE .. ALTER COLUMN .. DROP IDENTITY */
        "ALTER TABLE .. ALTER COLUMN .. DROP IDENTITY",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlrValidateAlterColumn,
        qlrOptCodeAlterColumn,
        qlrOptDataAlterColumn,
        qlrExecuteAlterColumn
    },
    {
        QLL_STMT_ALTER_TABLE_RENAME_COLUMN_TYPE,     /* ALTER TABLE .. RENAME COLUMN */
        "ALTER TABLE .. RENAME COLUMN",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlrValidateRenameColumn,
        qlrOptCodeRenameColumn,
        qlrOptDataRenameColumn,
        qlrExecuteRenameColumn
    },
    {
        QLL_STMT_ALTER_TABLE_PHYSICAL_ATTR_TYPE,     /* ALTER TABLE .. STORAGE */
        "ALTER TABLE .. STORAGE",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlrValidateAlterPhysicalAttr,
        qlrOptCodeAlterPhysicalAttr,
        qlrOptDataAlterPhysicalAttr,
        qlrExecuteAlterPhysicalAttr
    },
    {
        QLL_STMT_ALTER_TABLE_ADD_CONSTRAINT_TYPE,    /* ALTER TABLE .. ADD CONSTRAINT */
        "ALTER TABLE .. ADD CONSTRAINT",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlrValidateAddConst,
        qlrOptCodeAddConst,
        qlrOptDataAddConst,
        qlrExecuteAddConst
    },
    {
        QLL_STMT_ALTER_TABLE_ALTER_CONSTRAINT_TYPE,  /* ALTER TABLE .. ALTER CONSTRAINT */
        "ALTER TABLE .. ALTER CONSTRAINT",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlrValidateAlterConst,
        qlrOptCodeAlterConst,
        qlrOptDataAlterConst,
        qlrExecuteAlterConst
    },
    {
        QLL_STMT_ALTER_TABLE_DROP_CONSTRAINT_TYPE,   /* ALTER TABLE .. DROP CONSTRAINT */
        "ALTER TABLE .. DROP CONSTRAINT",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlrValidateDropConst,
        qlrOptCodeDropConst,
        qlrOptDataDropConst,
        qlrExecuteDropConst
    },
    {
        QLL_STMT_ALTER_TABLE_RENAME_CONSTRAINT_TYPE, /* ALTER TABLE .. RENAME CONSTRAINT */
        "ALTER TABLE .. RENAME CONSTRAINT",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_ALTER_TABLE_RENAME_TABLE_TYPE,      /* ALTER TABLE .. RENAME TO .. */
        "ALTER TABLE .. RENAME TO ..",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlrValidateRenameTable,
        qlrOptCodeRenameTable,
        qlrOptDataRenameTable,
        qlrExecuteRenameTable
    },
    {
        QLL_STMT_ALTER_TABLE_TABLESPACE_TYPE,        /* ALTER TABLE .. TABLESPACE */
        "ALTER TABLE .. TABLESPACE",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_ALTER_TABLE_ADD_SUPPLOG_TYPE,   /* ALTER TABLE .. ADD SUPPLEMENTAL LOG */
        "ALTER TABLE .. ADD SUPPLEMENTAL LOG",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_NO       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlrValidateAddSuppLog,
        qlrOptCodeAddSuppLog,
        qlrOptDataAddSuppLog,
        qlrExecuteAddSuppLog
    },
    {
        QLL_STMT_ALTER_TABLE_DROP_SUPPLOG_TYPE,  /* ALTER TABLE .. DROP SUPPLEMENTAL LOG */
        "ALTER TABLE .. DROP SUPPLEMENTAL LOG",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_NO       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlrValidateDropSuppLog,
        qlrOptCodeDropSuppLog,
        qlrOptDataDropSuppLog,
        qlrExecuteDropSuppLog
    },
    {
        QLL_STMT_ALTER_INDEX_PHYSICAL_ATTR_TYPE,     /* ALTER INDEX .. STORAGE */
        "ALTER INDEX .. STORAGE",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qliValidateAlterPhysicalAttr,
        qliOptCodeAlterPhysicalAttr,
        qliOptDataAlterPhysicalAttr,
        qliExecuteAlterPhysicalAttr
    },
    {
        QLL_STMT_ALTER_TABLESPACE_ADD_TYPE,          /* ALTER TABLESPACE .. ADD  */
        "ALTER TABLESPACE .. ADD",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_YES       |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_MOUNT,          
        KNL_STARTUP_PHASE_OPEN,
        qltValidateAlterSpaceAddFile,
        qltOptCodeAlterSpaceAddFile,
        qltOptDataAlterSpaceAddFile,
        qltExecuteAlterSpaceAddFile
    },
    {
        QLL_STMT_ALTER_TABLESPACE_BACKUP_TYPE,       /* ALTER TABLESPACE .. BACKUP  */
        "ALTER TABLESPACE .. BACKUP",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_YES       |
          QLL_STMT_ATTR_SUPPORT_CDS_NO       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qltValidateAlterSpaceBackup,
        qltOptCodeAlterSpaceBackup,
        qltOptDataAlterSpaceBackup,
        qltExecuteAlterSpaceBackup
    },
    {
        QLL_STMT_ALTER_TABLESPACE_BACKUP_INCREMENTAL_TYPE,  /* ALTER TABLESPACE .. BACKUP INCREMENTAL */
        "ALTER TABLESPACE .. BACKUP INCREMENTAL",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_AUTOCOMMIT_YES       |
          QLL_STMT_ATTR_SUPPORT_CDS_NO       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qltValidateAlterSpaceBackup,
        qltOptCodeAlterSpaceBackup,
        qltOptDataAlterSpaceBackup,
        qltExecuteAlterSpaceBackup
    },
    {
        QLL_STMT_ALTER_TABLESPACE_DROP_TYPE,         /* ALTER TABLESPACE .. DROP  */
        "ALTER TABLESPACE .. DROP",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_YES       |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,   
        KNL_STARTUP_PHASE_OPEN,
        qltValidateAlterSpaceDropFile,
        qltOptCodeAlterSpaceDropFile,
        qltOptDataAlterSpaceDropFile,
        qltExecuteAlterSpaceDropFile
    },
    {
        QLL_STMT_ALTER_TABLESPACE_ONLINE_TYPE,       /* ALTER TABLESPACE .. ONLINE  */
        "ALTER TABLESPACE .. ONLINE",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_YES       |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qltValidateAlterSpaceOnline,
        qltOptCodeAlterSpaceOnline,
        qltOptDataAlterSpaceOnline,
        qltExecuteAlterSpaceOnline
    },
    {
        QLL_STMT_ALTER_TABLESPACE_OFFLINE_TYPE,       /* ALTER TABLESPACE .. OFFLINE  */
        "ALTER TABLESPACE .. OFFLINE",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_YES       |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_MOUNT,          
        KNL_STARTUP_PHASE_OPEN,
        qltValidateAlterSpaceOffline,
        qltOptCodeAlterSpaceOffline,
        qltOptDataAlterSpaceOffline,
        qltExecuteAlterSpaceOffline
    },
    {
        QLL_STMT_ALTER_TABLESPACE_RENAME_TYPE,       /* ALTER TABLESPACE .. RENAME  */
        "ALTER TABLESPACE .. RENAME TO",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_YES       |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,   /* recovery 가능해야 하기 때문에 open상태에서만 가능 */
        KNL_STARTUP_PHASE_OPEN,
        qltValidateAlterSpaceRename,
        qltOptCodeAlterSpaceRename,
        qltOptDataAlterSpaceRename,
        qltExecuteAlterSpaceRename
    },
    {
        QLL_STMT_ALTER_TABLESPACE_RENAME_FILE_TYPE,  /* ALTER TABLESPACE .. RENAME FILE */
        "ALTER TABLESPACE .. RENAME { DATAFILE | MEMORY }",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_YES       |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_MOUNT,          
        KNL_STARTUP_PHASE_OPEN,
        qltValidateAlterSpaceRenameFile,
        qltOptCodeAlterSpaceRenameFile,
        qltOptDataAlterSpaceRenameFile,
        qltExecuteAlterSpaceRenameFile
    },
    {
        QLL_STMT_ALTER_TABLESPACE_SHRINK_TYPE,       /* ALTER TABLESPACE .. SHRINK */
        "ALTER TABLESPACE .. SHRINK",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_NONE,
        KNL_STARTUP_PHASE_NONE,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_ALTER_TRANSFORM_FOR_TYPE,           /* ALTER TRANSFORM FOR */
        "ALTER TRANSFORM FOR",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_ALTER_TYPE_TYPE,                    /* ALTER TYPE */
        "ALTER TYPE",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_ALTER_USER_TYPE,                    /* ALTER USER */
        "ALTER USER",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlaValidateAlterUser,
        qlaOptCodeAlterUser,
        qlaOptDataAlterUser,
        qlaExecuteAlterUser
    },
    {
        QLL_STMT_ALTER_VIEW_TYPE,                    /* ALTER VIEW */
        "ALTER VIEW",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlrValidateAlterView,
        qlrOptCodeAlterView,
        qlrOptDataAlterView,
        qlrExecuteAlterView
    },
    {
        QLL_STMT_CALL_TYPE,                          /* CALL routine*/
        "CALL routine",
        QLL_STMT_ATTR_SUPPORT_GLOBAL_YES,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_CLOSE_CURSOR_TYPE,                  /* CLOSE cursor */
        "CLOSE cursor",
        ( QLL_STMT_ATTR_DB_ACCESS_NONE        |      /* OPEN 구문의 stmt 사용함 */
          QLL_STMT_ATTR_DDL_NO                |
          QLL_STMT_ATTR_LOCKABLE_NO           |
          QLL_STMT_ATTR_FETCH_NO              |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_YES |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_YES    |
          QLL_STMT_ATTR_PLAN_CACHE_NO         |
          QLL_STMT_ATTR_AUTOCOMMIT_NO         |
          QLL_STMT_ATTR_SUPPORT_CDS_YES       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO  |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlcrValidateCloseCursor,
        qlcrCodeOptimizeCloseCursor,
        qlcrDataOptimizeCloseCursor,
        qlcrExecuteFuncCloseCursor
    },
    {
        QLL_STMT_COMMENT_ON_TYPE,                    /* COMMENT ON .. IS */
        "COMMENT ON .. IS",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE       |
          QLL_STMT_ATTR_DDL_YES               |
          QLL_STMT_ATTR_LOCKABLE_YES          |
          QLL_STMT_ATTR_FETCH_NO              |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO  |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO     |
          QLL_STMT_ATTR_PLAN_CACHE_NO         |
          QLL_STMT_ATTR_AUTOCOMMIT_NO         |
          QLL_STMT_ATTR_SUPPORT_CDS_YES       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_YES |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qldValidateCommentStmt,
        qldOptCodeCommentStmt,
        qldOptDataCommentStmt,
        qldExecuteCommentStmt
    },
    {
        QLL_STMT_COMMIT_TYPE,                        /* COMMIT */
        "COMMIT",
        ( QLL_STMT_ATTR_DB_ACCESS_NONE       |
          QLL_STMT_ATTR_DDL_NO               |
          QLL_STMT_ATTR_LOCKABLE_NO          |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_YES ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qltxValidateCommit,
        qltxOptCodeCommit,
        qltxOptDataCommit,
        qltxExecuteCommit
    },
    {
        QLL_STMT_COMMIT_FORCE_TYPE,                  /* COMMIT FORCE ... */
        "COMMIT FORCE ...",
        ( QLL_STMT_ATTR_DB_ACCESS_NONE       |
          QLL_STMT_ATTR_DDL_NO               |
          QLL_STMT_ATTR_LOCKABLE_NO          |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qltxValidateCommit,
        qltxOptCodeCommit,
        qltxOptDataCommit,
        qltxExecuteCommit
    },
    {
        QLL_STMT_CONNECT_TO_TYPE,                    /* CONNECT TO server_name */
        "CONNECT TO ..",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_COPY_FROM_TYPE,                     /* COPY .. FROM .. */
        "COPY .. FROM ..",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_COPY_TO_TYPE,                       /* COPY .. TO .. */
        "COPY .. TO ..",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_CREATE_ASSERTION_TYPE,              /* CREATE ASSERTION */
        "CREATE ASSERTION",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_CREATE_CAST_TYPE,                   /* CREATE CAST */
        "CREATE CAST",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_CREATE_CHARACTER_SET_TYPE,          /* CREATE CHARACTER SET */
        "CREATE CHARACTER SET",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_CREATE_COLLATION_TYPE,              /* CREATE COLLATION */
        "CREATE COLLATION",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_CREATE_DATABASE_TYPE,               /* CREATE DATABASE */
        "CREATE DATABASE",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_NO_MOUNT,          
        KNL_STARTUP_PHASE_NO_MOUNT,          
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_CREATE_DOMAIN_TYPE,                 /* CREATE DOMAIN */
        "CREATE DOMAIN",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_CREATE_FUNCTION_TYPE,               /* CREATE FUNCTION */
        "CREATE FUNCTION",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_CREATE_INDEX_TYPE,                  /* CREATE INDEX */
        "CREATE INDEX",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qliValidateCreateIndex,
        qliOptCodeCreateIndex,
        qliOptDataCreateIndex,
        qliExecuteCreateIndex
    },
    {
        QLL_STMT_CREATE_METHOD_TYPE,                 /* CREATE METHOD */
        "CREATE METHOD",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_CREATE_ORDERING_FOR_TYPE,           /* CREATE ORDERING FOR */
        "CREATE ORDERING FOR",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_CREATE_PROCEDURE_TYPE,              /* CREATE PROCEDURE */
        "CREATE PROCEDURE",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_CREATE_PROFILE_TYPE,                   /* CREATE PROFILE */
        "CREATE PROFILE",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlaValidateCreateProfile,
        qlaOptCodeCreateProfile,
        qlaOptDataCreateProfile,
        qlaExecuteCreateProfile
    },
    {
        QLL_STMT_CREATE_ROLE_TYPE,                   /* CREATE ROLE */
        "CREATE ROLE",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_CREATE_SCHEMA_TYPE,                 /* CREATE SCHEMA */
        "CREATE SCHEMA",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlsValidateCreateSchema,
        qlsOptCodeCreateSchema,
        qlsOptDataCreateSchema,
        qlsExecuteCreateSchema
    },
    {
        QLL_STMT_CREATE_SEQUENCE_TYPE,               /* CREATE SEQUENCE */
        "CREATE SEQUENCE",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlqValidateCreateSeq,
        qlqOptCodeCreateSeq,
        qlqOptDataCreateSeq,
        qlqExecuteCreateSeq
    },
    {
        QLL_STMT_CREATE_SYNONYM_TYPE,               /* CREATE SYNONYM */
        "CREATE SYNONYM",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlyValidateCreateSynonym,
        qlyOptCodeCreateSynonym,
        qlyOptDataCreateSynonym,
        qlyExecuteCreateSynonym
    },
    {
        QLL_STMT_CREATE_TABLE_TYPE,                  /* CREATE TABLE */
        "CREATE TABLE",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlrValidateCreateTable,
        qlrOptCodeCreateTable,
        qlrOptDataCreateTable,
        qlrExecuteCreateTable
    },
    {
        QLL_STMT_CREATE_TABLE_AS_SELECT_TYPE,        /* CREATE TABLE ... AS SELECT */
        "CREATE TABLE ... AS SELECT",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE       |
          QLL_STMT_ATTR_DDL_YES               |
          QLL_STMT_ATTR_LOCKABLE_YES          |
          QLL_STMT_ATTR_FETCH_NO              |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO  |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO     |
          QLL_STMT_ATTR_PLAN_CACHE_NO         |
          QLL_STMT_ATTR_AUTOCOMMIT_NO         |
          QLL_STMT_ATTR_SUPPORT_CDS_YES       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlrValidateCreateTableAsSelect,
        qlrOptCodeCreateTableAsSelect,
        qlrOptDataCreateTableAsSelect,
        qlrExecuteCreateTableAsSelect
    },
    {
        QLL_STMT_CREATE_TABLESPACE_TYPE, /* CREATE TABLESPACE */
        "CREATE TABLESPACE",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_YES       |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qltValidateCreateSpace,
        qltOptCodeCreateSpace,
        qltOptDataCreateSpace,
        qltExecuteCreateSpace
    },
    {
        QLL_STMT_CREATE_TRANSFORM_FOR_TYPE,          /* CREATE TRANSFORM_FOR */
        "CREATE TRANSFORM_FOR",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_CREATE_TRANSLATION_TYPE,            /* CREATE TRANSLATION */
        "CREATE TRANSLATION",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_CREATE_TRIGGER_TYPE,                /* CREATE TRIGGER */
        "CREATE TRIGGER",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_CREATE_TYPE_TYPE,                   /* CREATE TYPE */
        "CREATE TYPE",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_CREATE_USER_TYPE,                   /* CREATE USER */
        "CREATE USER",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlaValidateCreateUser,
        qlaOptCodeCreateUser,
        qlaOptDataCreateUser,
        qlaExecuteCreateUser
    },
    {
        QLL_STMT_CREATE_VIEW_TYPE,                   /* CREATE VIEW */
        "CREATE VIEW",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlrValidateCreateView,
        qlrOptCodeCreateView,
        qlrOptDataCreateView,
        qlrExecuteCreateView
    },
    {
        QLL_STMT_DECLARE_CURSOR_TYPE,               /* DECLARE .. CURSOR */
        "DECLARE .. CURSOR",
        ( QLL_STMT_ATTR_DB_ACCESS_READ        |
          QLL_STMT_ATTR_DDL_NO                |
          QLL_STMT_ATTR_LOCKABLE_NO           |
          QLL_STMT_ATTR_FETCH_NO              |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_YES |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_YES    |
          QLL_STMT_ATTR_PLAN_CACHE_NO         |
          QLL_STMT_ATTR_AUTOCOMMIT_NO         |
          QLL_STMT_ATTR_SUPPORT_CDS_YES       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_YES |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlcrValidateDeclareCursor,
        qlcrCodeOptimizeDeclareCursor,
        qlcrDataOptimizeDeclareCursor,
        qlcrExecuteFuncDeclareCursor
    },
    {
        QLL_STMT_DECLARE_LOCAL_TEMPORARY_TABLE_TYPE, /* DECLARE LOCAL TEMPORARY TABLE */
        "DECLARE LOCAL TEMPORARY TABLE",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_DELETE_TYPE,                        /* DELETE FROM */
        "DELETE FROM",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE       |
          QLL_STMT_ATTR_DDL_NO                |
          QLL_STMT_ATTR_LOCKABLE_YES          |
          QLL_STMT_ATTR_FETCH_NO              |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_YES |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_YES    |
          QLL_STMT_ATTR_PLAN_CACHE_YES        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO         |
          QLL_STMT_ATTR_SUPPORT_CDS_YES       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_YES |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlvValidateDelete,
        qloCodeOptimizeDelete,
        qloDataOptimizeDelete,
        qlxExecuteDelete
    },
    {
        QLL_STMT_DELETE_RETURNING_QUERY_TYPE,        /* DELETE FROM .. RETURNING */
        "DELETE FROM .. RETURNING query",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE       |
          QLL_STMT_ATTR_DDL_NO                |
          QLL_STMT_ATTR_LOCKABLE_YES          |
          QLL_STMT_ATTR_FETCH_YES             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_YES |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_YES    |
          QLL_STMT_ATTR_PLAN_CACHE_YES        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO         |
          QLL_STMT_ATTR_SUPPORT_CDS_YES       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_YES |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlvValidateDeleteReturnQuery,
        qloCodeOptimizeDeleteReturnQuery,
        qloDataOptimizeDeleteReturnQuery,
        qlxExecuteDeleteReturnQuery
    },
    {
        QLL_STMT_DELETE_RETURNING_INTO_TYPE,        /* DELETE FROM .. RETURNING .. INTO */
        "DELETE FROM .. RETURNING .. INTO",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE       |
          QLL_STMT_ATTR_DDL_NO                |
          QLL_STMT_ATTR_LOCKABLE_YES          |
          QLL_STMT_ATTR_FETCH_NO              |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_YES |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_YES    |
          QLL_STMT_ATTR_PLAN_CACHE_YES        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO         |
          QLL_STMT_ATTR_SUPPORT_CDS_YES       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_YES |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlvValidateDeleteReturnInto,
        qloCodeOptimizeDeleteReturnInto,
        qloDataOptimizeDeleteReturnInto,
        qlxExecuteDeleteReturnInto
    },
    {
        QLL_STMT_DELETE_WHERE_CURRENT_OF_TYPE,       /* DELETE FROM .. WHERE CURRENT OF cursor*/
        "DELETE FROM .. WHERE CURRENT OF cursor",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE       |
          QLL_STMT_ATTR_DDL_NO                |
          QLL_STMT_ATTR_LOCKABLE_YES          |
          QLL_STMT_ATTR_FETCH_NO              |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_YES |      /* easy to validate table handle */
          QLL_STMT_ATTR_SUPPORT_GLOBAL_YES    |
          QLL_STMT_ATTR_PLAN_CACHE_NO         |
          QLL_STMT_ATTR_AUTOCOMMIT_NO         |
          QLL_STMT_ATTR_SUPPORT_CDS_YES       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_YES |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlvValidateDeletePositioned,
        qloCodeOptimizeDeletePositioned,
        qloDataOptimizeDeletePositioned,
        qlxExecuteDeletePositioned
    },
    {
        QLL_STMT_DISCONNECT_TYPE,                    /* DISCONNECT */
        "DISCONNECT",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_DROP_ASSERTION_TYPE,                /* DROP ASSERTION */
        "DROP ASSERTION",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_DROP_CAST_TYPE,                     /* DROP CAST */
        "DROP CAST",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_DROP_CHARACTER_SET_TYPE,            /* DROP CHARACTER SET */
        "DROP CHARACTER SET",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_DROP_COLLATION_TYPE,                /* DROP COLLATION */
        "DROP COLLATION",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_DROP_DATABASE_TYPE,                 /* DROP DATABASE */
        "DROP DATABASE",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_NO_MOUNT,          
        KNL_STARTUP_PHASE_NO_MOUNT,          
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_DROP_DOMAIN_TYPE,                   /* DROP DOMAIN */
        "DROP DOMAIN",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_DROP_FUNCTION_TYPE,                 /* DROP FUNCTION */
        "DROP FUNCTION",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_DROP_INDEX_TYPE,                    /* DROP INDEX */
        "DROP INDEX",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qliValidateDropIndex,
        qliOptCodeDropIndex,
        qliOptDataDropIndex,
        qliExecuteDropIndex
    },
    {
        QLL_STMT_DROP_METHOD_TYPE,                   /* DROP METHOD */
        "DROP METHOD",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_DROP_ORDERING_FOR_TYPE,             /* DROP ORDERING FOR */
        "DROP ORDERING FOR",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_DROP_PROCEDURE_TYPE,                /* DROP PROCEDURE */
        "DROP PROCEDURE",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_DROP_PROFILE_TYPE,                  /* DROP PROFILE */
        "DROP PROFILE",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlaValidateDropProfile,
        qlaOptCodeDropProfile,
        qlaOptDataDropProfile,
        qlaExecuteDropProfile
    },
    {
        QLL_STMT_DROP_ROLE_TYPE,                     /* DROP ROLE */
        "DROP ROLE",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_DROP_ROUTINE_TYPE,                  /* DROP ROUTINE */
        "DROP ROUTINE",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_DROP_SCHEMA_TYPE,                  /* DROP SCHEMA */
        "DROP SCHEMA",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlsValidateDropSchema,
        qlsOptCodeDropSchema,
        qlsOptDataDropSchema,
        qlsExecuteDropSchema
    },
    {
        QLL_STMT_DROP_SEQUENCE_TYPE,                 /* DROP SEQUENCE */
        "DROP SEQUENCE",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlqValidateDropSeq,
        qlqOptCodeDropSeq,
        qlqOptDataDropSeq,
        qlqExecuteDropSeq
    },
    {
        QLL_STMT_DROP_SYNONYM_TYPE,                 /* DROP SYNONYM */
        "DROP SYNONYM",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlyValidateDropSynonym,
        qlyOptCodeDropSynonym,
        qlyOptDataDropSynonym,
        qlyExecuteDropSynonym,
    },
    {
        QLL_STMT_DROP_TABLE_TYPE,                    /* DROP TABLE */
        "DROP TABLE",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlrValidateDropTable,
        qlrOptCodeDropTable,
        qlrOptDataDropTable,
        qlrExecuteDropTable
    },
    {
        QLL_STMT_DROP_TABLESPACE_TYPE,               /* DROP TABLESPACE */
        "DROP TABLESPACE",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_YES       |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qltValidateDropSpace,
        qltOptCodeDropSpace,
        qltOptDataDropSpace,
        qltExecuteDropSpace
    },
    {
        QLL_STMT_DROP_TRANSFORM_FOR_TYPE,            /* DROP TRANSFORM FOR */
        "DROP TRANSFORM FOR",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_DROP_TRANSLATION_TYPE,              /* DROP TRANSLATION */
        "DROP TRANSLATION",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_DROP_TRIGGER_TYPE,                  /* DROP TRIGGER */
        "DROP TRIGGER",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_DROP_TYPE_TYPE,                     /* DROP TYPE */
        "DROP TYPE",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_DROP_USER_TYPE,                     /* DROP USER */
        "DROP USER",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlaValidateDropUser,
        qlaOptCodeDropUser,
        qlaOptDataDropUser,
        qlaExecuteDropUser
    },
    {
        QLL_STMT_DROP_VIEW_TYPE,                     /* DROP VIEW */
        "DROP VIEW",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlrValidateDropView,
        qlrOptCodeDropView,
        qlrOptDataDropView,
        qlrExecuteDropView
    },
    {
        QLL_STMT_FETCH_CURSOR_TYPE,                  /* FETCH cursor */
        "FETCH cursor",
        ( QLL_STMT_ATTR_DB_ACCESS_NONE        |      /* OPEN 구문의 stmt 사용함 */
          QLL_STMT_ATTR_DDL_NO                |
          QLL_STMT_ATTR_LOCKABLE_NO           |
          QLL_STMT_ATTR_FETCH_NO              |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_YES |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_YES    |
          QLL_STMT_ATTR_PLAN_CACHE_NO         |
          QLL_STMT_ATTR_AUTOCOMMIT_NO         |
          QLL_STMT_ATTR_SUPPORT_CDS_YES       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlcrValidateFetchCursor,
        qlcrCodeOptimizeFetchCursor,
        qlcrDataOptimizeFetchCursor,
        qlcrExecuteFuncFetchCursor
    },
    {
        QLL_STMT_FETCH_LOCATOR_TYPE,                 /* FETCH LOCATOR locator */
        "FETCH LOCATOR locator",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_GET_DIAGNOSTICS_TYPE,               /* GET DIAGNOSTICS */
        "GET DIAGNOSTICS",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_GRANT_DATABASE_TYPE,                /* GRANT .. ON DATABASE */
        "GRANT .. ON DATABASE",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlaValidateGrantPriv,
        qlaOptCodeGrantPriv,
        qlaOptDataGrantPriv,
        qlaExecuteGrantPriv
    },
    {
        QLL_STMT_GRANT_TABLESPACE_TYPE,              /* GRANT .. ON TABLESPACE */
        "GRANT .. ON TABLESPACE",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlaValidateGrantPriv,
        qlaOptCodeGrantPriv,
        qlaOptDataGrantPriv,
        qlaExecuteGrantPriv
    },
    {
        QLL_STMT_GRANT_SCHEMA_TYPE,                  /* GRANT .. ON SCHEMA */
        "GRANT .. ON SCHEMA",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlaValidateGrantPriv,
        qlaOptCodeGrantPriv,
        qlaOptDataGrantPriv,
        qlaExecuteGrantPriv
    },
    {
        QLL_STMT_GRANT_TABLE_TYPE,                   /* GRANT .. ON TABLE */
        "GRANT .. ON TABLE",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE       |
          QLL_STMT_ATTR_DDL_YES               |
          QLL_STMT_ATTR_LOCKABLE_YES          |
          QLL_STMT_ATTR_FETCH_NO              |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO  |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO     |
          QLL_STMT_ATTR_PLAN_CACHE_NO         |
          QLL_STMT_ATTR_AUTOCOMMIT_NO         |
          QLL_STMT_ATTR_SUPPORT_CDS_YES       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_YES |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlaValidateGrantPriv,
        qlaOptCodeGrantPriv,
        qlaOptDataGrantPriv,
        qlaExecuteGrantPriv
    },
    {
        QLL_STMT_GRANT_USAGE_TYPE,                  /* GRANT USAGE ON .. */
        "GRANT USAGE ON .. ",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE       |
          QLL_STMT_ATTR_DDL_YES               |
          QLL_STMT_ATTR_LOCKABLE_YES          |
          QLL_STMT_ATTR_FETCH_NO              |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO  |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO     |
          QLL_STMT_ATTR_PLAN_CACHE_NO         |
          QLL_STMT_ATTR_AUTOCOMMIT_NO         |
          QLL_STMT_ATTR_SUPPORT_CDS_YES       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_YES |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlaValidateGrantPriv,
        qlaOptCodeGrantPriv,
        qlaOptDataGrantPriv,
        qlaExecuteGrantPriv
    },
    {
        QLL_STMT_GRANT_ROLE_TYPE,                    /* GRANT role TO */
        "GRANT role TO",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_HOLD_LOCATOR_TYPE,                  /* HOLD LOCATOR */
        "HOLD LOCATOR",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_INSERT_TYPE,                        /* INSERT INTO */
        "INSERT INTO",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE       |
          QLL_STMT_ATTR_DDL_NO                |
          QLL_STMT_ATTR_LOCKABLE_YES          |
          QLL_STMT_ATTR_FETCH_NO              |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_YES |      /* easy to validate table handle */
          QLL_STMT_ATTR_SUPPORT_GLOBAL_YES    |
          QLL_STMT_ATTR_PLAN_CACHE_YES        |
          QLL_STMT_ATTR_ATOMIC_EXECUTION_YES  |
          QLL_STMT_ATTR_AUTOCOMMIT_NO         |
          QLL_STMT_ATTR_SUPPORT_CDS_YES       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_YES |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlvValidateInsertValues,
        qloCodeOptimizeInsertValues,
        qloDataOptimizeInsertValues,
        qlxExecuteInsertValues
    },
    {
        QLL_STMT_INSERT_RETURNING_QUERY_TYPE,       /* INSERT .. RETURNING */
        "INSERT .. RETURNING query",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE       |
          QLL_STMT_ATTR_DDL_NO                |
          QLL_STMT_ATTR_LOCKABLE_YES          |
          QLL_STMT_ATTR_FETCH_YES             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_YES |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_YES    |
          QLL_STMT_ATTR_PLAN_CACHE_YES        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO         |
          QLL_STMT_ATTR_SUPPORT_CDS_YES       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_YES |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlvValidateInsertReturnQuery,
        qloCodeOptimizeInsertReturnQuery,
        qloDataOptimizeInsertReturnQuery,
        qlxExecuteInsertReturnQuery
    },
    {
        QLL_STMT_INSERT_RETURNING_INTO_TYPE,        /* INSERT .. RETURNING .. INTO */
        "INSERT .. RETURNING .. INTO",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE       |
          QLL_STMT_ATTR_DDL_NO                |
          QLL_STMT_ATTR_LOCKABLE_YES          |
          QLL_STMT_ATTR_FETCH_NO              |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_YES |  /* INSERT SELECT RETURN INTO 와 구분 필요 */
          QLL_STMT_ATTR_SUPPORT_GLOBAL_YES    |
          QLL_STMT_ATTR_PLAN_CACHE_YES        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO         |
          QLL_STMT_ATTR_SUPPORT_CDS_YES       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_YES |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlvValidateInsertReturnInto,
        qloCodeOptimizeInsertReturnInto,
        qloDataOptimizeInsertReturnInto,
        qlxExecuteInsertReturnInto
    },
    {
        QLL_STMT_INSERT_SELECT_TYPE,                 /* INSERT .. SELECT */
        "INSERT .. SELECT",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE       |
          QLL_STMT_ATTR_DDL_NO                |
          QLL_STMT_ATTR_LOCKABLE_YES          |
          QLL_STMT_ATTR_FETCH_NO              |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_YES |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_YES    |
          QLL_STMT_ATTR_PLAN_CACHE_YES        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO         |
          QLL_STMT_ATTR_SUPPORT_CDS_YES       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_YES |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlvValidateInsertSelect,
        qloCodeOptimizeInsertSelect,
        qloDataOptimizeInsertSelect,
        qlxExecuteInsertSelect
    },
    {
        QLL_STMT_MERGE_INTO_TYPE,                    /* MERGE INTO */
        "MERGE INTO",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_MODULE_TYPE,                        /* MODULE module */
        "MODULE module",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_LOCK_TABLE_TYPE,                    /* LOCK TABLE */
        "LOCK TABLE",
        ( QLL_STMT_ATTR_DB_ACCESS_READ        |
          QLL_STMT_ATTR_DDL_NO                |
          QLL_STMT_ATTR_LOCKABLE_YES          |
          QLL_STMT_ATTR_FETCH_NO              |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO  |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_YES    |
          QLL_STMT_ATTR_PLAN_CACHE_NO         |
          QLL_STMT_ATTR_AUTOCOMMIT_NO         |
          QLL_STMT_ATTR_SUPPORT_CDS_YES       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_YES |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qltxValidateLockTable,
        qltxOptCodeLockTable,
        qltxOptDataLockTable,
        qltxExecuteLockTable
    },
    {
        QLL_STMT_OPEN_CURSOR_TYPE,                   /* OPEN cursor */
        "OPEN cursor",
        ( QLL_STMT_ATTR_DB_ACCESS_UNKNOWN     |      /* Read/Write 를 알 수 없음. 내부 SM Stmt 를 유지 관리함 */
          QLL_STMT_ATTR_DDL_NO                |
          QLL_STMT_ATTR_LOCKABLE_NO           |
          QLL_STMT_ATTR_FETCH_NO              |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_YES |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_YES    |
          QLL_STMT_ATTR_PLAN_CACHE_NO         |
          QLL_STMT_ATTR_AUTOCOMMIT_NO         |
          QLL_STMT_ATTR_SUPPORT_CDS_YES       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO  |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlcrValidateOpenCursor,
        qlcrCodeOptimizeOpenCursor,
        qlcrDataOptimizeOpenCursor,
        qlcrExecuteFuncOpenCursor
    },
    {
        QLL_STMT_PROCEDURE_TYPE,                     /* PROCEDURE procedure */
        "PROCEDURE procedure",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_RELEASE_SAVEPOINT_TYPE,             /* RELEASE SAVEPOINT */
        "RELEASE SAVEPOINT savepoint",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_NO               |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qltxValidateReleaseSavepoint,
        qltxOptCodeReleaseSavepoint,
        qltxOptDataReleaseSavepoint,
        qltxExecuteReleaseSavepoint
    },
    {
        QLL_STMT_RETURN_TYPE,                        /* RETURN value */
        "RETURN value",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_REVOKE_DATABASE_TYPE,               /* REVOKE .. ON DATABASE */
        "REVOKE .. ON DATABASE",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlaValidateRevokePriv,
        qlaOptCodeRevokePriv,
        qlaOptDataRevokePriv,
        qlaExecuteRevokePriv
    },
    {
        QLL_STMT_REVOKE_TABLESPACE_TYPE,             /* REVOKE .. ON TABLESPACE */
        "REVOKE .. ON TABLESPACE",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlaValidateRevokePriv,
        qlaOptCodeRevokePriv,
        qlaOptDataRevokePriv,
        qlaExecuteRevokePriv
    },
    {
        QLL_STMT_REVOKE_SCHEMA_TYPE,                 /* REVOKE .. ON SCHEMA */
        "REVOKE .. ON SCHEMA",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlaValidateRevokePriv,
        qlaOptCodeRevokePriv,
        qlaOptDataRevokePriv,
        qlaExecuteRevokePriv
    },
    {
        QLL_STMT_REVOKE_TABLE_TYPE,                  /* REVOKE .. ON TABLE */
        "REVOKE .. ON TABLE",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE       |
          QLL_STMT_ATTR_DDL_YES               |
          QLL_STMT_ATTR_LOCKABLE_YES          |
          QLL_STMT_ATTR_FETCH_NO              |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO  |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO     |
          QLL_STMT_ATTR_PLAN_CACHE_NO         |
          QLL_STMT_ATTR_AUTOCOMMIT_NO         |
          QLL_STMT_ATTR_SUPPORT_CDS_YES       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_YES |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlaValidateRevokePriv,
        qlaOptCodeRevokePriv,
        qlaOptDataRevokePriv,
        qlaExecuteRevokePriv
    },
    {
        QLL_STMT_REVOKE_USAGE_TYPE,                  /* REVOKE USAGE ON .. */
        "REVOKE USAGE ON ..",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE       |
          QLL_STMT_ATTR_DDL_YES               |
          QLL_STMT_ATTR_LOCKABLE_YES          |
          QLL_STMT_ATTR_FETCH_NO              |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO  |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO     |
          QLL_STMT_ATTR_PLAN_CACHE_NO         |
          QLL_STMT_ATTR_AUTOCOMMIT_NO         |
          QLL_STMT_ATTR_SUPPORT_CDS_YES       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_YES |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlaValidateRevokePriv,
        qlaOptCodeRevokePriv,
        qlaOptDataRevokePriv,
        qlaExecuteRevokePriv
    },
    {
        QLL_STMT_REVOKE_ROLE_TYPE,                   /* REVOKE role TO */
        "REVOKE role TO",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_ROLLBACK_TYPE,                      /* ROLLBACK */
        "ROLLBACK",
        ( QLL_STMT_ATTR_DB_ACCESS_NONE       |
          QLL_STMT_ATTR_DDL_NO               |
          QLL_STMT_ATTR_LOCKABLE_NO          |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_YES ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qltxValidateRollback,
        qltxOptCodeRollback,
        qltxOptDataRollback,
        qltxExecuteRollback
    },
    {
        QLL_STMT_ROLLBACK_FORCE_TYPE,                /* ROLLBACK FORCE ...  */
        "ROLLBACK FORCE ...",
        ( QLL_STMT_ATTR_DB_ACCESS_NONE       |
          QLL_STMT_ATTR_DDL_NO               |
          QLL_STMT_ATTR_LOCKABLE_NO          |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qltxValidateRollback,
        qltxOptCodeRollback,
        qltxOptDataRollback,
        qltxExecuteRollback
    },
    {
        QLL_STMT_ROLLBACK_TO_TYPE,                   /* ROLLBACK TO savepoint */
        "ROLLBACK TO savepoint",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_NO               |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qltxValidateRollback,
        qltxOptCodeRollback,
        qltxOptDataRollback,
        qltxExecuteRollback
    },
    {
        QLL_STMT_SAVEPOINT_TYPE,                     /* SAVEPOINT savepoint */
        "SAVEPOINT savepoint",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_NO               |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qltxValidateSavepoint,
        qltxOptCodeSavepoint,
        qltxOptDataSavepoint,
        qltxExecuteSavepoint
    },
    {
        QLL_STMT_SELECT_TYPE,                        /* SELECT */
        "SELECT",
        ( QLL_STMT_ATTR_DB_ACCESS_READ        |
          QLL_STMT_ATTR_DDL_NO                |
          QLL_STMT_ATTR_LOCKABLE_NO           |
          QLL_STMT_ATTR_FETCH_YES             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_YES |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_YES    |
          QLL_STMT_ATTR_PLAN_CACHE_YES        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO         |
          QLL_STMT_ATTR_SUPPORT_CDS_YES       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_YES |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_NO_MOUNT,          
        KNL_STARTUP_PHASE_OPEN,
        qlvValidateSelect,
        qloCodeOptimizeSelect,
        qloDataOptimizeSelect,
        qlxExecuteSelect
    },
    {
        QLL_STMT_SELECT_FOR_UPDATE_TYPE,            /* SELECT .. FOR UPDATE */
        "SELECT .. FOR UPDATE",
        ( QLL_STMT_ATTR_DB_ACCESS_READ        |
          QLL_STMT_ATTR_DDL_NO                |
          QLL_STMT_ATTR_LOCKABLE_YES          |
          QLL_STMT_ATTR_FETCH_YES             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_YES |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_YES    |
          QLL_STMT_ATTR_PLAN_CACHE_YES        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO         |
          QLL_STMT_ATTR_SUPPORT_CDS_YES       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_YES |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlvValidateSelect,
        qloCodeOptimizeSelect,
        qloDataOptimizeSelect,
        qlxExecuteSelect
    },
    {
        QLL_STMT_SELECT_INTO_TYPE,                   /* SELECT .. INTO */
        "SELECT .. INTO",
        ( QLL_STMT_ATTR_DB_ACCESS_READ        |
          QLL_STMT_ATTR_DDL_NO                |
          QLL_STMT_ATTR_LOCKABLE_NO           |
          QLL_STMT_ATTR_FETCH_NO              |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_YES |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_YES    |
          QLL_STMT_ATTR_PLAN_CACHE_YES        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO         |
          QLL_STMT_ATTR_SUPPORT_CDS_YES       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_YES |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_NO_MOUNT,          
        KNL_STARTUP_PHASE_OPEN,
        qlvValidateSelectInto,
        qloCodeOptimizeSelectInto,
        qloDataOptimizeSelectInto,
        qlxExecuteSelectInto
    },
    {
        QLL_STMT_SELECT_INTO_FOR_UPDATE_TYPE,        /* SELECT .. INTO .. FOR UPDATE */
        "SELECT .. INTO .. FOR UPDATE",
        ( QLL_STMT_ATTR_DB_ACCESS_READ        |
          QLL_STMT_ATTR_DDL_NO                |
          QLL_STMT_ATTR_LOCKABLE_YES          |
          QLL_STMT_ATTR_FETCH_NO              |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_YES |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_YES    |
          QLL_STMT_ATTR_PLAN_CACHE_YES        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO         |
          QLL_STMT_ATTR_SUPPORT_CDS_YES       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_YES |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlvValidateSelectInto,
        qloCodeOptimizeSelectInto,
        qloDataOptimizeSelectInto,
        qlxExecuteSelectInto
    },
    {
        QLL_STMT_SET_CATALOG_TYPE,                   /* SET CATALOG */
        "SET CATALOG",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_NONE,      
        KNL_STARTUP_PHASE_NONE,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_SET_COLLATION_TYPE,                 /* SET COLLATION */
        "SET COLLATION",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_SET_CONSTRAINTS_TYPE,               /* SET CONSTRAINTS */
        "SET CONSTRAINTS",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_NO               |
          QLL_STMT_ATTR_LOCKABLE_NO          |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_YES   |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qltxValidateSetConstraint,
        qltxOptCodeSetConstraint,
        qltxOptDataSetConstraint,
        qltxExecuteSetConstraint
    },
    {
        QLL_STMT_SET_NAMES_TYPE,                     /* SET NAMES */
        "SET NAMES",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_SET_PATH_TYPE,                      /* SET PATH */
        "SET PATH",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_SET_ROLE_TYPE,                      /* SET ROLE */
        "SET ROLE",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_SET_SCHEMA_TYPE,                    /* SET SCHEMA */
        "SET SCHEMA",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_SET_SESSION_AUTHORIZATION_TYPE,     /* SET SESSION AUTHORIZATION */
        "SET SESSION AUTHORIZATION",
        ( QLL_STMT_ATTR_DB_ACCESS_NONE       |
          QLL_STMT_ATTR_DDL_NO               |
          QLL_STMT_ATTR_LOCKABLE_NO          |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlssValidateSetSessionAuth,
        qlssOptCodeSetSessionAuth,
        qlssOptDataSetSessionAuth,
        qlssExecuteSetSessionAuth
    },
    {
        QLL_STMT_SET_SESSION_CHARACTERISTICS_TYPE,   /* SET SESSION CHARACTERISTICS */
        "SET SESSION CHARACTERISTICS",
        ( QLL_STMT_ATTR_DB_ACCESS_NONE       |
          QLL_STMT_ATTR_DDL_NO               |
          QLL_STMT_ATTR_LOCKABLE_NO          |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_YES ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlssValidateSetSessionCharacteristics,
        qlssOptCodeSetSessionCharacteristics,
        qlssOptDataSetSessionCharacteristics,
        qlssExecuteSetSessionCharacteristics
    },
    {
        QLL_STMT_SET_TIMEZONE_TYPE,                  /* SET TIME ZONE */
        "SET TIME ZONE",
        ( QLL_STMT_ATTR_DB_ACCESS_NONE       |
          QLL_STMT_ATTR_DDL_NO               |
          QLL_STMT_ATTR_LOCKABLE_NO          |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_YES ),
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNL_STARTUP_PHASE_OPEN,
        qlssValidateSetTimeZone,
        qlssOptCodeSetTimeZone,
        qlssOptDataSetTimeZone,
        qlssExecuteSetTimeZone
    },
    {
        QLL_STMT_SET_TRANSACTION_TYPE,               /* SET TRANSACTION */
        "SET TRANSACTION",
        ( QLL_STMT_ATTR_DB_ACCESS_NONE       |
          QLL_STMT_ATTR_DDL_NO               |
          QLL_STMT_ATTR_LOCKABLE_NO          |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_YES ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlssValidateSetTransaction,
        qlssOptCodeSetTransaction,
        qlssOptDataSetTransaction,
        qlssExecuteSetTransaction
    },
    {
        QLL_STMT_SET_TRANSFORM_GROUP_FOR_TYPE,       /* SET TRANSFORM GROUP FOR */
        "SET TRANSFORM GROUP FOR",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_START_TRANSACTION_TYPE,             /* START TRANSACTION */
        "START TRANSACTION",
        QLL_STMT_ATTR_NA,
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qllValidateFuncNA,
        qllCodeOptimizeFuncNA,
        qllDataOptimizeFuncNA,
        qllExecuteFuncNA
    },
    {
        QLL_STMT_TRUNCATE_TABLE_TYPE,                /* TRUNCATE TABLE */
        "TRUNCATE TABLE",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE      |
          QLL_STMT_ATTR_DDL_YES              |
          QLL_STMT_ATTR_LOCKABLE_YES         |
          QLL_STMT_ATTR_FETCH_NO             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |
          QLL_STMT_ATTR_PLAN_CACHE_NO        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO        |
          QLL_STMT_ATTR_SUPPORT_CDS_YES      |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlrValidateTruncateTable,
        qlrOptCodeTruncateTable,
        qlrOptDataTruncateTable,
        qlrExecuteTruncateTable
    },
    {
        QLL_STMT_UPDATE_TYPE,                        /* UPDATE */
        "UPDATE",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE       |
          QLL_STMT_ATTR_DDL_NO                |
          QLL_STMT_ATTR_LOCKABLE_YES          |
          QLL_STMT_ATTR_FETCH_NO              |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_YES |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_YES    |
          QLL_STMT_ATTR_PLAN_CACHE_YES        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO         |
          QLL_STMT_ATTR_SUPPORT_CDS_YES       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_YES |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,
        KNL_STARTUP_PHASE_OPEN,
        qlvValidateUpdate,
        qloCodeOptimizeUpdate,
        qloDataOptimizeUpdate,
        qlxExecuteUpdate
    },
    {
        QLL_STMT_UPDATE_RETURNING_QUERY_TYPE,        /* UPDATE .. RETURNING */
        "UPDATE .. RETURNING query",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE       |
          QLL_STMT_ATTR_DDL_NO                |
          QLL_STMT_ATTR_LOCKABLE_YES          |
          QLL_STMT_ATTR_FETCH_YES             |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_YES |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_YES    |
          QLL_STMT_ATTR_PLAN_CACHE_YES        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO         |
          QLL_STMT_ATTR_SUPPORT_CDS_YES       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_YES |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlvValidateUpdateReturnQuery,
        qloCodeOptimizeUpdateReturnQuery,
        qloDataOptimizeUpdateReturnQuery,
        qlxExecuteUpdateReturnQuery
    },
    {
        QLL_STMT_UPDATE_RETURNING_INTO_TYPE,         /* UPDATE .. RETURNING .. INTO */
        "UPDATE .. RETURNING .. INTO",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE       |
          QLL_STMT_ATTR_DDL_NO                |
          QLL_STMT_ATTR_LOCKABLE_YES          |
          QLL_STMT_ATTR_FETCH_NO              |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_YES |
          QLL_STMT_ATTR_SUPPORT_GLOBAL_YES    |
          QLL_STMT_ATTR_PLAN_CACHE_YES        |
          QLL_STMT_ATTR_AUTOCOMMIT_NO         |
          QLL_STMT_ATTR_SUPPORT_CDS_YES       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_YES |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlvValidateUpdateReturnInto,
        qloCodeOptimizeUpdateReturnInto,
        qloDataOptimizeUpdateReturnInto,
        qlxExecuteUpdateReturnInto
    },
    {
        QLL_STMT_UPDATE_WHERE_CURRENT_OF_TYPE,       /* UPDATE .. WHERE CURRENT OF cursor */
        "UPDATE .. WHERE CURRENT OF cursor",
        ( QLL_STMT_ATTR_DB_ACCESS_WRITE       |
          QLL_STMT_ATTR_DDL_NO                |
          QLL_STMT_ATTR_LOCKABLE_YES          |
          QLL_STMT_ATTR_FETCH_NO              |
          QLL_STMT_ATTR_SNAPSHOT_ITERATOR_YES |      /* easy to validate table handle */
          QLL_STMT_ATTR_SUPPORT_GLOBAL_YES    |
          QLL_STMT_ATTR_PLAN_CACHE_NO         |
          QLL_STMT_ATTR_AUTOCOMMIT_NO         |
          QLL_STMT_ATTR_SUPPORT_CDS_YES       |
          QLL_STMT_ATTR_CAN_REFER_SYNONYM_YES |
          QLL_STMT_ATTR_STATE_TRANS_NO ),
        KNL_STARTUP_PHASE_OPEN,          
        KNL_STARTUP_PHASE_OPEN,
        qlvValidateUpdatePositioned,
        qloCodeOptimizeUpdatePositioned,
        qloDataOptimizeUpdatePositioned,
        qlxExecuteUpdatePositioned
    }
};




/** @} qlgStmtTable */

