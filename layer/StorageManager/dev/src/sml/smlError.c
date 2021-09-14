/*******************************************************************************
 * smlError.c
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

#include <stl.h>

/**
 * @file smlError.c
 * @brief Storage Manager Layer Error Routines
 */

/**
 * @addtogroup smlError
 * @{
 */

/**
 * @brief Storage Manager Layer 에러 테이블.
 * 내부에러에 대응되는 외부에러와 에러 메세지로 구성됨.
 */
stlErrorRecord gStorageManagerErrorTable[] =
{
    {/* SML_ERRCODE_INITIALIZE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Failed to initialize storage manager layer"
    },
    {/* SML_ERRCODE_INITIALIZED_YET */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "storage manager layer is not initialized yet"
    },
    {/* SML_ERRCODE_EXCEED_DATAFILE_LIMIT */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Tablespace(%d) exceed datafile limit"
    },
    {/* SML_ERRCODE_TABLESPACE_FULL */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Tablespace(%d) is full"
    },
    {/* SML_ERRCODE_ALREADY_DROPPED_TBS */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Tablespace(%d) is already dropped"
    },
    {/* SML_ERRCODE_EXISTS_SYSTEM_TBS */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "System Tablespace is exists"
    },
    {/* SML_ERRCODE_MUTATING_TABLE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "Table is mutating, trigger/function may not see it"
    },
    {/* SML_ERRCODE_DEADLOCK */
        STL_EXT_ERRCODE_DATABASE_RESOURCE_EXCEPTION_NO_SUBCLASS,
        "deadlock detected"
    },
    {/* SML_ERRCODE_FETCH_SEQUENCE */
        STL_EXT_ERRCODE_INVALID_CURSOR_STATE_NO_SUBCLASS,
        "fetch out of sequence"
    },
    {/* SML_ERRCODE_SEQUENCE_LIMIT_EXCEEDED */
        STL_EXT_ERRCODE_DATA_EXCEPTION_SEQUENCE_GENERATOR_LIMIT_EXCEEDED,
        "sequence generator limit exceeded"
    },
    {/* SML_ERRCODE_SEQUENCE_MAXVALUE_BELOW_CURRENT_VALUE */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_PARAMETER_VALUE,
        "MAXVALUE cannot be made to be less than the current value"
    },
    {/* SML_ERRCODE_SEQUENCE_MINVALUE_EXCEED_CURRENT_VALUE */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_PARAMETER_VALUE,
        "MINVALUE cannot be made to exceed the current value"
    },
    {/* SML_ERRCODE_SEQUENCE_SETVALUE_BELOW_CURRENT_VALUE */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_PARAMETER_VALUE,
        "cannot be made to be less than the current value"
    },
    {/* SML_ERRCODE_SEQUENCE_SETVALUE_EXCEED_CURRENT_VALUE */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_PARAMETER_VALUE,
        "cannot be made to exceed the current value"
    },
    {/* SML_ERRCODE_DATAFILE_EXTEND */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Fail to extend datafile(%u) of tablespace(%u) in Transaction(%ld)"
    },
    {/* SML_ERRCODE_NO_MORE_EXTENDIBLE_DATAFILE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "there is no extendible datafile in tablespace '%s'"
    },
    {/* SML_ERRCODE_INDEX_UNIQUENESS_VIOLATION             */
        STL_EXT_ERRCODE_INTEGRITY_CONSTRAINT_VIOLATION_RESTRICT_VIOLATION, 
        "some rows of base table violate uniqueness of index"
    },
    {   /* SML_ERRCODE_INVALID_OPERATION_ON_FIXED_TABLE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS, 
        "invalid operation on fixed table or dump table"
    },
    {   /* SML_ERRCODE_INVALID_STARTUP_PHASE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS, 
        "the operation is disallowed before %s startup phase"
    },
    {/* SML_ERRCODE_INVALID_LOGSTREAM_NUMBER */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "log stream number string is invalid"
    },
    {/* SML_ERRCODE_EXISTS_LOGSTREAM */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "log stream %d already exists"
    },
    {/* SML_ERRCODE_INVALID_LOGGROUP_NUMBER */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "log group number string is invalid"
    },
    {/* SML_ERRCODE_INVALID_LOGGROUP_STATE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "log group state must be INACTIVE or UNUSED"
    },
    {/* SML_ERRCODE_EXISTS_LOGGROUP */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "log group %d already exists"
    },
    {/* SML_ERRCODE_EXISTS_LOGMEMBER */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "log member %s already exists"
    },
    {/* SML_ERRCODE_INVALID_DUMP_OPTION_STRING */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "invalid dump option string: %s"
    },
    {/* SML_ERRCODE_LOCK_TIMEOUT */
        STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_TIMEOUT_EXPIRED,
        "resource busy or timeout expired"
    },
    {/* SML_ERRCODE_ALREADY_DROPPED_DATAFILE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "datafile(%d,%d) is already dropped"
    },
    {/* SML_ERRCODE_ALREADY_EXIST_FILE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "file '%s' is already exist"
    },
    {/* SML_ERRCODE_SEQUENCE_NOT_YET_DEFINED */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "sequence is not yet defined"
    },
    {/* SML_ERRCODE_EXCEEDED_MAX_INDEX_DEPTH */
        STL_EXT_ERRCODE_DATABASE_RESOURCE_EXCEPTION_NO_SUBCLASS,
        "maximum index depth(%d) exceeded"
    },
    {/* SML_ERRCODE_EXCEEDED_MAX_KEY_SIZE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "maximum key length(%d) exceeds"
    },
    {/* SML_ERRCODE_SERIALIZE_TRANSACTION */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "can't serialize access for this transaction"
    },
    {/* SML_ERRCODE_UNABLE_CHECKPOINT_IN_CDS_MODE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "unable to checkpoint in cds mode"
    },
    {/* SML_ERRCODE_UNABLE_RECOVER_IN_CDS_MODE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "unable to recover database in cds mode"
    },
    {/* SML_ERRCODE_NOT_NULL_CONSTRAINT_VIOLATION */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "some rows of base table violate not null constraint"
    },
    {/* SML_ERRCODE_INVALID_ROWID */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "invalid ROWID"
    },
    {/* SML_ERRCODE_DATABASE_IS_READ_ONLY */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "Database is READ ONLY"
    },
    {/* SML_ERRCODE_UNABLE_RECOVER_IN_READ_ONLY_MODE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "unable to recover database in READ ONLY mode"
    },
    {/* SML_ERRCODE_EXCEED_SEGMENT_MAXSIZE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "exceed segment MAXSIZE(%ld)"
    },
    {/* SML_ERRCODE_MUTATING_DUMP_OBJECT */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Target object of dump table is mutating"
    },
    {/* SML_ERRCODE_ACCESS_OFFLINE_TBS */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "cannot access the OFFLINE tablespace '%s' "
    },
    {/* SML_ERRCODE_NO_INST_ROW */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "cannot find the row (%ld)"
    },
    {/* SML_ERRCODE_FILE_ALREADY_USED */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "file is already used in database '%s'"
    },
    {/* SML_ERRCODE_NON_EMPTY_DATAFILE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "datafile not empty"
    },
    {/* SML_ERRCODE_DROP_OFFLINE_DATAFILE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "cannot drop datafile of OFFLINE tablespace '%s'"
    },
    {/* SML_ERRCODE_FILE_NOT_EXIST */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "file does not exist - '%s'"
    },
    {/* SML_ERRCODE_FILE_IS_READ_ONLY */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "file is READ ONLY - '%s'"
    },
    {/* SML_ERRCODE_INVALID_DATAFILE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "file is invalid or old version - '%s'"
    },
    {/* SML_ERRCODE_NOT_ONLINE_TABLESPACE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "tablespace is not online - '%s'"
    },
    {/* SML_ERRCODE_NOT_OFFLINE_TABLESPACE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "tablespace is not offline - '%s'"
    },
    {/* SML_ERRCODE_NEED_MEDIA_RECOVERY */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "media recovery required - '%s'"
    },
    {/* SML_ERRCODE_ACCESS_UNUSABLE_SEGMENT */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "segment is unusable ( physical id : %ld )"
    },
    {/* SML_ERRCODE_SORT_OVERFLOW */
        STL_EXT_ERRCODE_DATABASE_RESOURCE_EXCEPTION_NO_SUBCLASS,
        "insufficient resource for sort"
    },
    {/* SML_ERRCODE_INTERNAL */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "internal error ( %s, %d )"
    },
    {/* SML_ERRCODE_SEGMENT_CORRUPTED */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "segment is corrupted ( physical id : %ld )"
    },
    {/* SML_ERRCODE_FAILED_TO_REDO */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "failed to apply redo log ( lsn : %ld, lpsn : %d, physical id : %ld )"
    },
    {/* SML_ERRCODE_MINIMUM_LOGGROUP_COUNT */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "dropping log group makes total log group count less than minimum log group count(%d)"
    },
    {/* SML_ERRCODE_LOG_MEMBER_NOT_EXIST */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "log member is not exist - '%s'"
    },
    {/* SML_ERRCODE_ALREADY_USED_LOG_MEMBER */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "log member is already used - '%s'"
    },
    {/* SML_ERRCODE_EMPTY_LOGFILE_GROUP */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "CURRENT logfile is empty - '%s'"
    },
    {/* SML_ERRCODE_NEXT_LOGGROUP_ACTIVE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "next logfile to switch should be INACTIVE or UNUSED - '%s'"
    },
    {/* SML_ERRCODE_TRANS_RANGE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "INITRANS must be less equal than MAXTRANS"
    },
    {/* SML_ERRCODE_EXCEED_MAX_LOGMEMBER_COUNT */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "cannot be added log member to exceed maximum log member count(%d)"
    },
    {/* SML_ERRCODE_NOT_APPROPRIATE_PHASE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS, 
        "the operation is disallowed at %s phase"
    },
    {/* SML_ERRCODE_INSTANT_TABLE_ROW_TOO_LARGE */
        STL_EXT_ERRCODE_DATABASE_RESOURCE_EXCEPTION_NO_SUBCLASS,
        "the instant table row length is too large"
    },
    {/* SML_ERRCODE_SORT_TABLE_KEY_ROW_TOO_LARGE */
        STL_EXT_ERRCODE_DATABASE_RESOURCE_EXCEPTION_NO_SUBCLASS,
        "the sort instant table key row length is too large"
    },
    {/* SML_ERRCODE_NOT_ARCHIVELOG_MODE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "cannot media recover; archivelog mode not enabled"
    },
    {/* SML_ERRCODE_LOGFILE_NOT_EXIST */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "logfile does not exist - '%s'"
    },
    {/* SML_ERRCODE_UNABLE_TO_ARCHIVELOG_IN_CDS_MODE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "cannot support ARCHIVELOG in cds mode"
    },
    {/* SML_ERRCODE_OLD_CONTROL_FILE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "control file is not recent; do media recovery"
    },
    {/* SML_ERRCODE_CANNOT_BACKUP_OFFLINE_TABLESPACE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "cannot BACKUP; tablespace(%s) is offline"
    },
    {/* SML_ERRCODE_CANNOT_BACKUP_TEMPORARY_TABLESPACE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "cannot BACKUP; tablespace(%s) is temporary"
    },
    {/* SML_ERRCODE_CANNOT_BACKUP_DROPPED_TABLESPACE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "cannot BACKUP; tablespace is dropped"
    },
    {/* SML_ERRCODE_CANNOT_BEGIN_BACKUP */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "cannot BACKUP; tablespace(%s) is already in backup"
    },
    {/* SML_ERRCODE_BACKUP_IN_PROGRESS */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "cannot execute; tablespace(%s) is proceeding backup"
    },
    {/* SML_ERRCODE_CANNOT_END_BACKUP */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "cannot END BACKUP; tablespace(%s) is not in backup"
    },
    {/* SML_ERRCODE_ALL_TBS_NOT_IN_BACKUP */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "cannot END BACKUP; none of the tablespaces are in backup"
    },
    {/* SML_ERRCODE_CANNOT_BACKUP_BUSY_TABLESPACE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "cannot BACKUP; tablespace(%s) is proceeding other operation"
    },
    {/* SML_ERRCODE_CANNOT_BEGIN_BACKUP_DATABASE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "cannot BACKUP; database is already in backup"
    },
    {/* SML_ERRCODE_CANNOT_END_BACKUP_DATABASE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "cannot END BACKUP; database is not in backup"
    },
    {/* SML_ERRCODE_DATABASE_IN_SHUTDOWN */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "cannot BACKUP; database is going shutdown"
    },
    {/* SML_ERRCODE_NOT_APPROPRIATE_LOGFILE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "(%s) is not an appropriate logfile to recover"
    },
    {/* SML_ERRCODE_MUST_OPEN_RESETLOGS */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "must use RESETLOGS option for database open"
    },
    {/* SML_ERRCODE_CANNOT_OPEN_RESETLOGS */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "must use NORESETLOGS option for database open"
    },
    {/* SML_ERRCODE_NOT_SUFFICIENT_OLD_BACKUP */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "(%s) was not a sufficiently old backup"
    },
    {/* SML_ERRCODE_MEDIA_RECOVERY_CANCELED */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "media recovery canceled"
    },
    {/* SML_ERRCODE_CANNOT_PREVENT_BACKUP */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "cannot SHUTDOWN; backup in progress"
    },
    {/* SML_ERRCODE_CORRUPTED_CONTROLFILE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "control file is corrupted"
    },
    {/* SML_ERRCODE_NOT_EXIST_LEVEL_0_BACKUP */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "LEVEL 0 INCREMENTAL BACKUP does not exist"
    },
    {/* SML_ERRCODE_DISABLED_LOG_FLUSHING */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "cannot flush logs; disabled log flushing"
    },
    {/* SML_ERRCODE_LOGFILE_NOT_VALID */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "(%s) is not valid logfile"
    },
    {/* SML_ERRCODE_NOT_EXIST_BACKUP */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "not exist incremental backup"
    },
    {/* SML_ERRCODE_NOT_EXIST_VALID_INCREMENTAL_BACKUP */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "not exist valid incremental backup for tablespace(%s)"
    },
    {/* SML_ERRCODE_DATAFILE_REQUIRED_RECOVERY */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "datafile recovery required - datafile(%s) of tablespace(%s) corrupted"
    },
    {/* SML_ERRCODE_BACKUP_IS_CORRUPTED */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "backup file(%s) is corrupted"
    },
    {/* SML_ERRCODE_INDEX_REBUILD             */
        STL_EXT_ERRCODE_INTEGRITY_CONSTRAINT_VIOLATION_RESTRICT_VIOLATION, 
        "fail to build index ( %ld )"
    },
    {/* SML_ERRCODE_CTRLFILE_CORRUPTED */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "control file is corrupted - '%s'"
    },
    {/* SML_ERRCODE_EXCEEDED_MAX_INSTANT_ROW_SIZE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "maximum record length(%d) exceeds"
    },
    {/* SML_ERRCODE_INVALID_LOGMEMBER_STATE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "log member state must be UNUSED or INACTIVE"
    },
    {/* SML_ERRCODE_MINIMUM_LOGMEMBER_COUNT */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "dropping log member makes log member count of group(%d) less than minimum log member count(%d)"
    },
    {/* SML_ERRCODE_NOT_EXIST_LOGGROUP_ID */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "log group id does not exist - '%d'"
    },
    {/* SML_ERRCODE_RECOVERY_IN_PROGRESS */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "cannot execute; recovery is in progress"
    },
    {/* SML_ERRCODE_WARNING_NEED_SPECIFIC_ARCHIVED_LOGFILE */
        STL_EXT_ERRCODE_WARNING_NO_SUBCLASS,
        "Warning: media recovery needs a logfile including log (Lsn %ld)"
    },
    {/* SML_ERRCODE_WARNING_SUGGESTED_ARCHIVED_LOGFILE */
        STL_EXT_ERRCODE_WARNING_NO_SUBCLASS,
        "Warning: suggestion '%s'"
    },
    {/* SML_ERRCODE_INCONSISTENT_CONTROL_FILE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "control file '%s' is inconsistent with primary"
    },
    {/* SML_ERRCODE_TABLESPACE_OFFLINE_ABNORMALLY */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "tablespace (%s) is taken offline as the result of a write error"
    },
    {/* SML_ERRCODE_UNSUITABLE_LOG_BLOCK_SIZE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "given LOG_BLOCK_SIZE(%d) is not a suitable value for a size of log block; LOG_BLOCK_SIZE should be one of 512, 1024, 2048 or 4096."
    },
    {/* SML_ERRCODE_CHANGE_NOARCHIVELOG_MODE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "cannot disable archivelog - exist tablespace needed media recovery."
    },
    {/* SML_ERRCODE_TABLESPACE_OFFLINE_NORMAL */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "OFFLINE NORMAL disallowed; cannot be taken offline consistently."
    },
    {/* SML_ERRCODE_TABLESPACE_OFFLINE_IMMEDIATE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "OFFLINE IMMEDIATE disallowed; noarchivelog mode"
    },
    {/* SML_ERRCODE_MIN_ROW_VIOLATION */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "cannot create COLUMNAR TABLE; given MIN ROW COUNT is too big to put into one page"
    },
    {/* SML_ERRCODE_COLUMNAR_TABLE_INVALID_ROW_SIZE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "record size is too large for columnar table"
    },
    {/* SML_ERRCODE_DATAFILE_IS_CORRUPTED  */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "datafile '%s' is corrupted"
    },
    {/* SML_ERRCODE_DATAFILE_RECENT_THAN_REDO_LOGFILE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "datafile '%s' is more recent than redo logfile"
    },
    {/* SML_ERRCODE_EXCEED_TRANSACTION_UNDO */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "transaction undo limit exceeded"
    },
    {
        0,
        NULL
    }
};

/** @} */

