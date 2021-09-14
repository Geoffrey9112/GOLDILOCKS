/*******************************************************************************
 * qlpNodeStatement.h
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

#ifndef _QLPNODESTATEMENT_H_
#define _QLPNODESTATEMENT_H_ 1

/**
 * @file qlpNodeStatement.h
 * @brief SQL Processor Layer Statement Nodes
 */


/**
 * @defgroup qlpNodeStatement Statement Nodes 
 * @ingroup qlpNode
 * @{
 */


/*******************************************************************************
 * DEFINITIONS 
 ******************************************************************************/

/**
 * @brief ALTER USER Action
 */
typedef enum qlpAlterUserAction
{
    QLP_ALTER_USER_ACTION_NA = 0,         /**< N/A */

    QLP_ALTER_USER_ACTION_PASSWORD,         /**< ALTER USER PASSWORD */
    QLP_ALTER_USER_ACTION_ALTER_PROFILE,    /**< ALTER USER PROFILE */
    QLP_ALTER_USER_ACTION_PASSWORD_EXPIRE,  /**< ALTER USER PASSWORD EXPIRE */
    QLP_ALTER_USER_ACTION_ACCOUNT_LOCK,     /**< ALTER USER ACCOUNT LOCK */
    QLP_ALTER_USER_ACTION_ACCOUNT_UNLOCK,   /**< ALTER USER ACCOUNT UNLOCK */
    QLP_ALTER_USER_ACTION_DEFAULT_SPACE,    /**< ALTER USER DEFAULT TABLESPACE */
    QLP_ALTER_USER_ACTION_TEMPORARY_SPACE,  /**< ALTER USER TEMPORARY TABLESPACE */
    QLP_ALTER_USER_ACTION_SCHEMA_PATH,      /**< ALTER USER SCHEMA PATH */
    
    QLP_ALTER_USER_ACTION_MAX
    
} qlpAlterUserAction;

/**
 * @brief CURRENT PATH string
 */
#define QLP_ALTER_USER_CURRENT_PATH    ( "$$CURRENT_PATH$$" )

/**
 * @brief tablespace 의 media 유형
 */
typedef enum qlpSpaceMediaType
{
    QLP_SPACE_MEDIA_TYPE_NA = 0,      /**< N/A */
    
    QLP_SPACE_MEDIA_TYPE_DISK,        /**< DISK Tablespace File   */
    QLP_SPACE_MEDIA_TYPE_MEMORY,      /**< MEMORY Tablespace File */
    QLP_SPACE_MEDIA_TYPE_SSD,         /**< Solid-State Disk Tablespace File */

    QLP_SPACE_MEDIA_TYPE_MAX
} qlpSpaceMediaType;


/**
 * @brief tablespace usage 유형
 */
typedef enum qlpSpaceUsageType
{
    QLP_SPACE_USAGE_TYPE_NA = 0,           /**< N/A */
    
    QLP_SPACE_USAGE_TYPE_DICT,             /**< UNDO TABLESPACE */
    QLP_SPACE_USAGE_TYPE_UNDO,             /**< UNDO TABLESPACE */
    QLP_SPACE_USAGE_TYPE_DATA,             /**< DATA TABLESPACE */
    QLP_SPACE_USAGE_TYPE_TEMP,             /**< TEMP TABLESPACE */

    QLP_SPACE_USAGE_TYPE_MAX
} qlpSpaceUsageType;


/**
 * @brief object attribute type for table/index definition
 */
typedef enum qlpObjectAttrType
{
    QLP_OBJECT_ATTR_INVALID = 0,

    QLP_OBJECT_ATTR_SEGMENT_CLAUSE,
    QLP_OBJECT_ATTR_SEGMENT_INITIAL,
    QLP_OBJECT_ATTR_SEGMENT_NEXT,
    QLP_OBJECT_ATTR_SEGMENT_MINSIZE,
    QLP_OBJECT_ATTR_SEGMENT_MAXSIZE,

    QLP_OBJECT_ATTR_PHYSICAL_PCTFREE,
    QLP_OBJECT_ATTR_PHYSICAL_PCTUSED,
    QLP_OBJECT_ATTR_PHYSICAL_INITRANS,
    QLP_OBJECT_ATTR_PHYSICAL_MAXTRANS,

    QLP_OBJECT_ATTR_LOGGING,
    QLP_OBJECT_ATTR_PARALLEL,
    QLP_OBJECT_ATTR_TABLESPACE,

    QLP_OBJECT_ATTR_COLUMNAR_OPTION,

    QLP_OBJECT_ATTR_MAX
} qlpObjectAttrType;


typedef enum qlpIdentityRestart
{
    QLP_IDENTITY_RESTART_OPTION_INVALID = 0,

    QLP_IDENTITY_RESTART_OPTION_CONTINUE,
    QLP_IDENTITY_RESTART_OPTION_RESTART
    
} qlpIdentityRestart;

typedef enum qlpDropStorageOption
{
    QLP_DROP_STORAGE_INVALID = 0,

    QLP_DROP_STORAGE_DROP,
    QLP_DROP_STORAGE_DROP_ALL,
    QLP_DROP_STORAGE_REUSE
    
} qlpDropStorageOption;


/**
 * @brief drop behavior
 */
typedef enum qlpDropDatafiles
{
    QLP_DROP_DATAFILES_INVALID = 0,
    QLP_DROP_DATAFILES_KEEP,
    QLP_DROP_DATAFILES_AND
} qlpDropDatafiles;


/**
 * @brief lock mode
 */
typedef enum qlpLockMode
{
    QLP_LOCK_MODE_INVALID             = SML_LOCK_NONE,
    QLP_LOCK_MODE_ROW_SHARE           = SML_LOCK_IS,
    QLP_LOCK_MODE_ROW_EXCLUSIVE       = SML_LOCK_IX,
    QLP_LOCK_MODE_SHARE               = SML_LOCK_S,
    QLP_LOCK_MODE_EXCLUSIVE           = SML_LOCK_X,
    QLP_LOCK_MODE_SHARE_ROW_EXCLUSIVE = SML_LOCK_SIX
} qlpLockMode;


/**
 * @brief set operation
 */
typedef enum qlpSetOperation
{
    QLP_SET_OP_INVALID = 0,
    QLP_SET_OP_NONE,
    QLP_SET_OP_UNION,
    QLP_SET_OP_EXCEPT,
    QLP_SET_OP_INTERSECT
} qlpSetOperation;


/**
 * @brief transaction attribute
 */
typedef enum qlpTransactionAttr
{
    QLP_TRANSACTION_ATTR_ACCESS = 0,
    QLP_TRANSACTION_ATTR_ISOLATION_LEVEL,
    QLP_TRANSACTION_ATTR_UNIQUE_INTEGRITY,
    
    QLP_TRANSACTION_ATTR_MAX,
} qlpTransactionAttr;

typedef enum qlpAgerBehavior
{
    QLP_AGER_START = 1,
    QLP_AGER_STOP,
    QLP_AGER_LOOPBACK
} qlpAgerBehavior;

typedef enum qlpFlushLogBehavior
{
    QLP_START_FLUSH_LOGS = 1,
    QLP_STOP_FLUSH_LOGS,
    QLP_FLUSH_LOGS
} qlpFlushLogBehavior;

/**
 * @brief REVOKE behavior
 */
typedef enum qlpRevokeBehavior
{
    QLP_REVOKE_NA = 0,

    QLP_REVOKE_RESTRICT,              /**< RESTRICT */
    QLP_REVOKE_CASCADE,               /**< CASCADE */
    QLP_REVOKE_CASCADE_CONSTRAINTS,   /**< CASCADE CONSTRAINTS */

    QLP_REVOKE_MAX
    
} qlpRevokeBehavior;

/*******************************************************************************
 * STRUCTURES 
 ******************************************************************************/

/**
 * @brief create user
 */
struct qlpUserDef
{
    qllNodeType             mType;               /**< node type */ 
    stlInt32                mNodePos;            /**< node position */

    qlpValue              * mUserName;           /**< user name */
    qlpValue              * mPassword;           /**< password */

    qlpValue              * mDefaultSpace;       /**< default tablespace */
    qlpValue              * mTempSpace;          /**< temporary tablespace */

    qlpValue              * mSchemaName;         /**< Schema Name */

    qlpValue              * mProfileName;        /**< PROFILE name */
    stlBool                 mPasswordExpire;     /**< PASSWORD EXPIRE */
    stlBool                 mAccountLock;        /**< ACCOUNT LOCK */
};



/**
 * @brief drop user
 */
struct qlpDropUser
{
    qllNodeType         mType;             /**< node type */
    stlInt32            mNodePos;          /**< node position */

    stlBool             mIfExists;         /**< IF EXISTS */
    
    qlpValue          * mName;             /**< user name */

    stlBool             mIsCascade;        /**< CASCADE  */
};

/**
 * @brief alter user
 */
struct qlpAlterUser
{
    qllNodeType         mType;             /**< node type */
    stlInt32            mNodePos;          /**< node position */

    qlpAlterUserAction  mAlterAction;      /**< ALTER USER action */

    qlpValue          * mUserName;         /**< user name */

    /*
     * PASSWORD
     */
    
    qlpValue          * mNewPassword;      /**< new password */
    qlpValue          * mOrgPassword;      /**< original password */

    /*
     * TABLESPACE
     */
    
    qlpValue          * mSpaceName;        /**< tablespace name */

    /*
     * SCHEMA PATH
     */
    
    qlpList           * mSchemaList;       /**< schema name list */

    /*
     * PROFILE
     */
    
    qlpValue          * mProfileName;      /**< profile name */
};

/**
 * @brief password parameter
 */
struct qlpPasswordParameter
{
    qllNodeType              mType;                   /**< node type */
    stlInt32                 mNodePos;                /**< node position */
    
    ellProfileParam          mParameterType;          /**< parameter type */
    
    stlBool                  mIsDEFAULT;              /**< is DEFAULT */
    stlBool                  mIsUNLIMITED;            /**< is UNLIMITED */
    qlpValue               * mValue;                  /**< value */
    stlInt32                 mValueStrLen;            /**< value string length */
};


/**
 * @brief create profile
 */
struct qlpProfileDef
{
    qllNodeType              mType;                  /**< node type */ 
    stlInt32                 mNodePos;               /**< node position */

    qlpValue               * mProfileName;           /**< profile name */

    /*
     * PASSWORD PARAMETER
     */

    qlpList                * mPasswordParameters;    /**< Password Parameters */
};

/**
 * @brief drop profile
 */
struct qlpDropProfile
{
    qllNodeType         mType;             /**< node type */
    stlInt32            mNodePos;          /**< node position */

    stlBool             mIfExists;         /**< IF EXISTS */
    
    qlpValue          * mProfileName;      /**< profile name */

    stlBool             mIsCascade;        /**< CASCADE  */
};

/**
 * @brief alter profile
 */
struct qlpAlterProfile
{
    qllNodeType              mType;                  /**< node type */ 
    stlInt32                 mNodePos;               /**< node position */

    qlpValue               * mProfileName;           /**< profile name */

    /*
     * PASSWORD PARAMETER
     */

    qlpList                * mPasswordParameters;    /**< Password Parameters */
};


/**
 * @brief privilege action
 */
struct qlpPrivAction
{
    qllNodeType         mType;                   /**< node type */
    stlInt32            mNodePos;                /**< node position */
    
    ellPrivObject       mPrivObject;             /**< Privilege Object */
    stlInt32            mPrivAction;             /**< Privilege Action */
    qlpList           * mColumnList;             /**< Column Name List */
};

/**
 * @brief privilege object
 */
struct qlpPrivObject
{
    qllNodeType         mType;                   /**< node type */
    stlInt32            mNodePos;                /**< node position */

    ellPrivObject       mPrivObjType;            /**< Privilege Object */
    qlpValue          * mNonSchemaObjectName;    /**< Non Schema Object Name */
    qlpObjectName     * mSchemaObjectName;       /**< Schema Object Name */
};

/**
 * @brief privilege
 */
struct qlpPrivilege
{
    qllNodeType         mType;                   /**< node type */
    stlInt32            mNodePos;                /**< node position */
    
    qlpPrivObject     * mPrivObject;             /**< Privilege Object */
    qlpList           * mPrivActionList;         /**< Priivlege Action List */
};
    
/**
 * @brief grant privilege
 */
struct qlpGrantPriv
{
    qllNodeType         mType;                   /**< node type */
    stlInt32            mNodePos;                /**< node position */

    qlpPrivilege      * mPrivilege;              /**< Privilege */
    
    qlpList           * mGranteeList;            /**< Grantee List */
    
    stlBool             mWithGrant;              /**< WITH GRANT OPTION */
};

/**
 * @brief revoke privilege
 */
struct qlpRevokePriv
{
    qllNodeType         mType;                   /**< node type */
    stlInt32            mNodePos;                /**< node position */

    qlpPrivilege      * mPrivilege;              /**< Privilege */
    
    qlpList           * mRevokeeList;            /**< Revokee List */
    
    stlBool             mGrantOption;            /**< GRANT OPTION FOR */
    qlpRevokeBehavior   mRevokeBehavior;         /**< revoke behavior */
};


/**
 * @brief schema definition 
 */
struct qlpSchemaDef
{
    qllNodeType             mType;               /**< node type */ 
    stlInt32                mNodePos;            /**< node position */
    
    qlpValue              * mSchemaName;         /**< schema name */
    qlpValue              * mOwnerName;          /**< owner name */

    qlpList               * mSchemaElemList;       /**< schema element list */
    qlpList               * mRefSchemaObjectList;  /**< 구문내에 존재하는 Schema Object List */
};


/**
 * @brief drop schema
 */
struct qlpDropSchema
{
    qllNodeType         mType;             /**< node type */
    stlInt32            mNodePos;          /**< node position */

    stlBool             mIfExists;         /**< IF EXISTS */
    
    qlpValue          * mName;             /**< schema name */

    stlBool             mIsCascade;        /**< CASCADE  */
};


typedef enum
{
    QLP_TABLE_DEFINITION_TYPE_NONE,
    QLP_TABLE_DEFINITION_TYPE_ROW,
    QLP_TABLE_DEFINITION_TYPE_COLUMNAR,
    QLP_TABLE_DEFINITION_TYPE_MAX
} qlpTableDefType;

/**
 * @brief table definition : QLL_TABLE_DEF_TYPE
 */
struct qlpTableDef
{
    qllNodeType             mType;               /**< node type */ 
    stlInt32                mNodePos;            /**< node position */

    qlpTableDefType         mTableDefType;       /**< table type */
    qlpRelInfo            * mRelation;           /**< relation info (primitive) */ 
    qlpList               * mTableElems;         /**< column definitions (qllNode)
                                                  *     : list of qlpColumnDef or qlpConstraint */
    qlpValue              * mTablespaceName;     /**< tablespace name */

    /* segment attribute values */
    qlpSize               * mSegInitialSize;
    qlpSize               * mSegNextSize;
    qlpSize               * mSegMinSize;
    qlpSize               * mSegMaxSize;

    /* permanent attribute values */
    qlpValue              * mPermanent;

    /* physical attribute values */
    qlpValue              * mPctFree;
    qlpValue              * mPctUsed;
    qlpValue              * mIniTrans;
    qlpValue              * mMaxTrans;

    /* for CREATE TABLE AS SELECT */
    qlpList               * mColumnNameList;
    qlpQueryExpr          * mSubquery;
    stlBool                 mWithData;
/*
  check_constraint
  table_type
  with_option
  on_commit
  inherit_relation
*/  
};


/**
 * @brief drop table : QLL_DROP_TABLE_TYPE
 */
struct qlpDropTable
{
    qllNodeType       mType;            /**< node type */
    stlInt32          mNodePos;         /**< node position */

    stlBool           mIfExists;        /**< IF EXISTS */

    qlpObjectName   * mName;            /**< table name */

    /*
     * Drop Table Option
     */

    stlBool           mIsCascade;       /**< CASCADE CONSTRAINTS */
};


/**
 * @brief truncate table : QLL_TRUNCATE_TABLE_TYPE
 */
struct qlpTruncateTable
{
    qllNodeType       mType;               /**< node type */
    stlInt32          mNodePos;            /**< node position */

    qlpObjectName   * mName;               /**< table name */

    /*
     * Truncate Table Options
     */
    
    qlpValue        * mIdentityRestart;
    qlpValue        * mDropStorage;
};


struct qlpAlterTableAddColumn
{
    qllNodeType             mType;               /**< node type */ 
    stlInt32                mNodePos;            /**< node position */

    qlpObjectName         * mTableName;          /**< table name */

    /*
     * Column Definition List
     */

    qlpList               * mColumnDefList;       /**< column definitions (qllNode)
                                                   * - list of qlpColumnDef */
};


typedef enum qlpDropColumnAction
{
    QLP_DROP_COLUMN_ACTION_NA = 0,                   /**< N/A */

    QLP_DROP_COLUMN_ACTION_SET_UNUSED_COLUMN,        /**< SET UNUSED COLUMN */
    QLP_DROP_COLUMN_ACTION_DROP_COLUMN,              /**< DROP COLUMN */
    QLP_DROP_COLUMN_ACTION_DROP_UNUSED_COLUMNS,      /**< DROP UNUSED COLUMNS */

    QLP_DROP_COLUMN_ACTION_MAX
    
} qlpDropColumnAction;


struct qlpAlterTableDropColumn
{
    qllNodeType             mType;               /**< node type */ 
    stlInt32                mNodePos;            /**< node position */

    qlpObjectName         * mTableName;          /**< table name */

    /*
     * Drop Action
     */

    qlpDropColumnAction     mDropAction;         /**< Drop Column 유형 */
    stlInt32                mDropActionPos;      /**< Drop Column 유형 Position */
    
    /*
     * Column Name List
     */

    qlpList               * mColumnNameList;       /**< list of string */

    stlBool                 mIsCascade;            /**< CASCADE CONSTRAINTS */
};


typedef enum qlpAlterColumnAction
{
    QLP_ALTER_COLUMN_ACTION_NA = 0,              /**< N/A */

    QLP_ALTER_COLUMN_SET_DEFAULT,                /**< SET DEFAULT */
    QLP_ALTER_COLUMN_DROP_DEFAULT,               /**< DROP DEFAULT */
    QLP_ALTER_COLUMN_SET_NOT_NULL,               /**< SET NOT NULL */
    QLP_ALTER_COLUMN_DROP_NOT_NULL,              /**< DROP NOT NULL */
    QLP_ALTER_COLUMN_SET_DATA_TYPE,              /**< SET DATA TYPE */
    QLP_ALTER_COLUMN_ALTER_IDENTITY_SPEC,        /**< GENERATED { } AS IDENTITY */
    QLP_ALTER_COLUMN_DROP_IDENTITY,              /**< DROP IDENTITY */

    QLP_ALTER_COLUMN_ACTION_MAX
    
} qlpAlterColumnAction;



struct qlpAlterTableAlterColumn
{
    qllNodeType             mType;               /**< node type */ 
    stlInt32                mNodePos;            /**< node position */

    qlpObjectName         * mTableName;          /**< table name */
    qlpValue              * mColumnName;         /**< column name */

    /*
     * Alter Column Action
     */

    qlpAlterColumnAction    mAlterColumnAction;  /**< Alter Column 유형 */
    stlInt32                mAlterActionPos;     /**< Alter Column 유형 Position */

    /*
     * Alter Column Option
     */

    qlpObjectName         * mNotNullName;        /**< SET [CONSTRAINT name] NOT NULL */
    qlpConstraintAttr     * mNotNullConstAttr;   /**< SET NOT NULL init_deferred / deferrable / enforce */
    qllNode               * mDefault;            /**< SET DEFAULT default_string */
    qllNode               * mDataType;           /**< SET DATA TYPE data type */
    qllNode               * mIdentitySpec;       /**< GENERATED { } AS IDENTITY { } */
};

struct qlpAlterTableAddConstraint
{
    qllNodeType             mType;               /**< node type */ 
    stlInt32                mNodePos;            /**< node position */

    qlpObjectName         * mTableName;          /**< table name */

    qlpList               * mConstDefinition;         /**< Constraint Definition */
};

struct qlpAlterTableDropConstraint
{
    qllNodeType             mType;               /**< node type */ 
    stlInt32                mNodePos;            /**< node position */

    qlpObjectName         * mTableName;          /**< table name */

    /*
     * Constraint Object
     */

    qlpDropConstObject    * mConstObject;       /**< Constraint Object */
    
    /*
     * drop option
     */

    stlBool                 mKeepIndex;         /**< KEEP INDEX or DROP INDEX */
    stlBool                 mIsCascade;         /**< CASCADE CONSTRAINTS */
};


struct qlpAlterTableAlterConstraint
{
    qllNodeType             mType;               /**< node type */ 
    stlInt32                mNodePos;            /**< node position */

    qlpObjectName         * mTableName;          /**< table name */

    /*
     * Alter Constraint 
     */

    qlpDropConstObject    * mConstObject;        /**< Constraint Object */
    qlpConstraintAttr     * mConstAttr;          /**< init_deferred / deferrable / enforce */
};

struct qlpAlterTablePhysicalAttr
{
    qllNodeType             mType;               /**< node type */ 
    stlInt32                mNodePos;            /**< node position */

    qlpObjectName         * mTableName;          /**< table name */

    /* segment attribute values */
    qlpSize               * mSegInitialSize;
    qlpSize               * mSegNextSize;
    qlpSize               * mSegMinSize;
    qlpSize               * mSegMaxSize;

    /* physical attribute values */
    qlpValue              * mPctFree;
    qlpValue              * mPctUsed;
    qlpValue              * mIniTrans;
    qlpValue              * mMaxTrans;
};
    
struct qlpAlterTableRenameColumn
{
    qllNodeType             mType;               /**< node type */ 
    stlInt32                mNodePos;            /**< node position */

    qlpObjectName         * mTableName;          /**< table name */

    qlpValue              * mOrgColName;         /**< original column name */
    qlpValue              * mNewColName;         /**< new column name */
};


struct qlpAlterTableSuppLog
{
    qllNodeType             mType;               /**< node type */ 
    stlInt32                mNodePos;            /**< node position */

    qlpObjectName         * mTableName;          /**< table name */
};

struct qlpAlterTableRenameTable
{
    qllNodeType             mType;               /**< node type */ 
    stlInt32                mNodePos;            /**< node position */

    qlpObjectName         * mOrgName;            /**< original table name */
    qlpValue              * mNewName;            /**< new table name */
};


/**
 * @brief alter index .. storage .. : QLL_STMT_ALTER_INDEX_PHYSICAL_ATTR_TYPE
 */
struct qlpAlterIndexPhysicalAttr
{
    qllNodeType             mType;               /**< node type */ 
    stlInt32                mNodePos;            /**< node position */

    qlpObjectName         * mIndexName;          /**< index name */

    /* segment attribute values */
    qlpSize               * mSegInitialSize;
    qlpSize               * mSegNextSize;
    qlpSize               * mSegMinSize;
    qlpSize               * mSegMaxSize;

    /* physical attribute values */
    qlpValue              * mPctFree;
    qlpValue              * mIniTrans;
    qlpValue              * mMaxTrans;
};
    
/**
 * @brief alter database add logfile group : QLL_STMT_ALTER_DATABASE_ADD_LOGFILE_GROUP_TYPE
 */
struct qlpAlterDatabaseAddLogfileGroup
{
    qllNodeType          mType;           /**< node type */
    stlInt32             mNodePos;        /**< node position */

    qlpValue           * mLogGroupId;     /**< log group id */
    qlpList            * mLogfileList;    /**< database log file spec : (char *) */
    qlpSize            * mLogfileSize;    /**< logfile size */
    qlpValue           * mLogfileReuse;   /**< reuse logfile : boolean */
};

/**
 * @brief alter database drop logfile group : QLL_STMT_ALTER_DATABASE_DROP_LOGFILE_GROUP_TYPE
 */
struct qlpAlterDatabaseDropLogfileGroup
{
    qllNodeType          mType;           /**< node type */
    stlInt32             mNodePos;        /**< node position */

    qlpValue           * mLogGroupId;     /**< log group id */
};

/**
 * @brief alter database drop logfile member : QLL_STMT_ALTER_DATABASE_DROP_LOGFILE_MEMBER_TYPE
 */
struct qlpAlterDatabaseDropLogfileMember
{
    qllNodeType          mType;           /**< node type */
    stlInt32             mNodePos;        /**< node position */

    qlpList            * mLogFileList;    /**< file list */
};

/**
 * @brief alter database rename logfile : QLL_STMT_ALTER_DATABASE_RENAME_LOGFILE_TYPE
 */
struct qlpAlterDatabaseRenameLogfile
{
    qllNodeType          mType;           /**< node type */
    stlInt32             mNodePos;        /**< node position */

    qlpList            * mFromFileList;   /**< from file list */
    qlpList            * mToFileList;     /**< to file list */
};

/**
 * @brief alter database rename logfile : QLL_STMT_ALTER_DATABASE_ADD_LOGFILE_MEMBER_TYPE
 */
struct qlpAlterDatabaseAddLogfileMember
{
    qllNodeType          mType;           /**< node type */
    stlInt32             mNodePos;        /**< node position */

    qlpList            * mMemberList;     /**< from Member list */
    qlpValue           * mLogGroupId;     /**< log group id */
    qlpValue           * mLogfileReuse;   /**< reuse logfile : boolean */
};

/**
 * @brief alter database archivelog : QLL_STMT_ALTER_DATABASE_ARCHIVELOG_MODE_TYPE
 */
struct qlpAlterDatabaseArchivelogMode
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */
    stlInt32           mMode;
};

/**
 * @brief alter database begin/end backup : QLL_STMT_ALTER_DATABASE_BACKUP_TYPE
 */

struct qlpAlterDatabaseBackup
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */
    qlpValue         * mCommand;        /**< backup BEGIN, END */
    stlUInt8           mBackupType;     /**< FULL / INCREMENTAL */
    qlpValue         * mOption;         /**< DIFFERENTIAL / CUMULATIVE */
};

struct qlpAlterDatabaseDeleteBackupList
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */
    qlpValue         * mTarget;         /**< OBSOLETE / ALL */
    stlBool            mDeleteFile;     /**< INCLUDING BACKUP LIST */
};

struct qlpIncrementalBackupOption
{
    qlpValue         * mLevel;
    qlpValue         * mOption;
};

struct qlpBackupCtrlfile
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */
    qlpValue         * mTarget;
};

/**
 * @brief alter database restore ctrlfile : QLL_STMT_ALTER_DATABASE_RESTORE_TYPE;
 */

struct qlpRestoreCtrlfile
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */
    qlpValue         * mTarget;         /**< target */
};

/**
 * @brief alter database register irrecoverable segment : QLL_STMT_ALTER_DATABASE_IRRECOVERABLE_TYPE
 */
struct qlpAlterDatabaseIrrecoverable
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */
    qlpList          * mSegmentList;
};

/**
 * @brief alter database recover
 * : QLL_STMT_ALTER_DATABASE_RECOVER_DATABASE_TYPE
 */
struct qlpAlterDatabaseRecover
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */
    qlpValue         * mRecoveryOption; /**< incomplete recovery options */
};

struct qlpImrOption
{
    stlUInt8     mImrOption;
    stlUInt8     mImrCondition;
    qlpValue   * mConditionValue;
    stlBool      mBackupCtrlfile;
};

struct qlpInteractiveImrCondition
{
    stlUInt8     mImrCondition;
    qlpValue   * mLogfileName;
};

struct qlpRestoreOption
{
    stlUInt8     mUntilType;
    qlpValue   * mUntilValue;
};

/**
 * @brief alter database recover tablespace : QLL_STMT_ALTER_DATABASE_RECOVER_TABLESPACE_TYPE
 */
struct qlpAlterDatabaseRecoverTablespace
{
    qllNodeType          mType;            /**< node type */
    stlInt32             mNodePos;         /**< node position */

    qlpValue           * mName;            /**< tablespace name */
};

/**
 * @brief alter database recover datafile
 * : QLL_STMT_ALTER_DATABASE_RECOVER_DATAFILE_TYPE
 */
struct qlpAlterDatabaseRecoverDatafile
{
    qllNodeType   mType;            /**< node type */
    stlInt32      mNodePos;         /**< node position */

    qlpList     * mRecoverList;     /**< Datafile List */
};

struct qlpDatafileList
{
    qlpValue * mDatafileName;
    qlpValue * mBackupName;
    stlBool    mUsingBackup;
    stlBool    mForCorruption;
};
    
    
/**
 * @brief alter database restore : QLL_STMT_ALTER_DATABASE_RECOVER_DATABASE_TYPE,
 */
struct qlpAlterDatabaseRestore
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */
    qlpValue         * mRestoreOption;  /**< restore until option */
};

/**
 * @brief alter database restore tablespace : QLL_STMT_ALTER_DATABASE_RECOVER_TABLESPACE_TYPE
 */
struct qlpAlterDatabaseRestoreTablespace
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */
    qlpValue         * mTbsName;        /**< tablespace name */
};

/**
 * @brief ALTER DATABASE CLEAR PASSWORD HISTORY
 */
struct qlpAlterDatabaseClearPassHistory
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */
};

/**
 * @brief tablespace definition : QLL_STMT_CREATE_TABLESPACE_XXXX
 */
struct qlpTablespaceDef
{
    qllNodeType          mType;           /**< node type */
    stlInt32             mNodePos;        /**< node position */

    qlpSpaceMediaType    mMediaType;
    qlpSpaceUsageType    mUsageType;
    qlpValue           * mName;           /**< tablespace name */
    qlpList            * mFileSpecs;      /**< tablespace file spec : qlpDatafile */
    qlpTablespaceAttr  * mTbsAttrs;       /**< tablespace attributes */
};


/**
 * @brief drop tablespace : QLL_DROP_TABLESPACE_TYPE
 */
struct qlpDropTablespace
{
    qllNodeType         mType;             /**< node type */
    stlInt32            mNodePos;          /**< node position */

    stlBool             mIfExists;         /**< IF EXISTS */
    
    qlpValue          * mName;             /**< tablespace name */
    stlInt32            mNamePos;          /**< node position */

    /*
     * Drop Tablespace Option
     */

    qlpValue          * mIsIncluding;     /**< INCLUDING CONTENTS */
    qlpValue          * mDropDatafiles;   /**< drop datafiles option, qlpDropDatafiles */
    qlpValue          * mIsCascade;       /**< CASCADE CONSTRAINTS */
};

/**
 * @brief alter tablespace add file : QLL_STMT_ALTER_TABLESPACE_ADD_TYPE
 */
struct qlpAlterSpaceAddFile
{
    qllNodeType          mType;           /**< node type */
    stlInt32             mNodePos;        /**< node position */

    qlpValue           * mName;           /**< tablespace name */

    qlpSpaceMediaType    mMediaType;
    stlInt32             mUsageTypePos;   /**< usage type pos */
    qlpSpaceUsageType    mUsageType;      /**< usage type */
    qlpList            * mFileSpecs;      /**< tablespace file spec : qlpDatafile */
};

/**
 * @brief alter tablespace backup :
 * QLL_STMT_ALTER_TABLESPACE_BACKUP_TYPE
 */

struct qlpAlterTablespaceBackup
{
    qllNodeType          mType;           /**< node type */
    stlInt32             mNodePos;        /**< node position */
    qlpValue           * mName;           /**< tablespace name */
    qlpValue           * mCommand;        /**< backup BEGIN, END */
    stlUInt8             mBackupType;     /**< FULL / INCREMENTAL */
    qlpValue           * mOption;         /**< DIFFERENTIAL / CUMULATIVE */
};

/**
 * @brief alter tablespace drop file : QLL_STMT_ALTER_TABLESPACE_DROP_TYPE
 */
struct qlpAlterSpaceDropFile
{
    qllNodeType          mType;           /**< node type */
    stlInt32             mNodePos;        /**< node position */

    qlpValue           * mName;           /**< tablespace name */

    qlpSpaceMediaType    mMediaType;      /**< media type */
    stlInt32             mUsageTypePos;   /**< usage type pos */
    qlpSpaceUsageType    mUsageType;      /**< usage type */
    qlpValue           * mFileName;       /**< file name */
};


/**
 * @brief alter tablespace drop file : QLL_STMT_ALTER_TABLESPACE_ONLINE_TYPE
 */
struct qlpAlterSpaceOnline
{
    qllNodeType          mType;            /**< node type */
    stlInt32             mNodePos;         /**< node position */

    qlpValue           * mName;            /**< tablespace name */
};


/**
 * @brief alter tablespace drop file : QLL_STMT_ALTER_TABLESPACE_OFFLINE_TYPE
 */
struct qlpAlterSpaceOffline
{
    qllNodeType          mType;            /**< node type */
    stlInt32             mNodePos;         /**< node position */

    qlpValue           * mName;            /**< tablespace name */
    qlpValue           * mOfflineBehavior; /**< offline hehavior */
};


/**
 * @brief alter tablespace rename file : QLL_STMT_ALTER_TABLESPACE_RENAME_FILE_TYPE
 */
struct qlpAlterSpaceRenameFile
{
    qllNodeType          mType;           /**< node type */
    stlInt32             mNodePos;        /**< node position */

    qlpValue           * mName;           /**< tablespace name */

    qlpSpaceMediaType    mMediaType;      /**< media type */
    stlInt32             mUsageTypePos;   /**< usage type pos */
    qlpSpaceUsageType    mUsageType;      /**< usage type */
    qlpList            * mFromFileList;   /**< from file list */
    qlpList            * mToFileList;     /**< to file list */
};


/**
 * @brief alter tablespace rename to : QLL_STMT_ALTER_TABLESPACE_RENAME_TYPE
 */
struct qlpAlterSpaceRename
{
    qllNodeType          mType;           /**< node type */
    stlInt32             mNodePos;        /**< node position */

    qlpValue           * mName;           /**< tablespace name */
    qlpValue           * mNewName;         /**< new tablespace name */
};



/**
 * @brief drop sequence : QLL_DROP_SEQUENCE_TYPE
 */
struct qlpDropSequence
{
    qllNodeType       mType;            /**< node type */
    stlInt32          mNodePos;         /**< node position */

    stlBool           mIfExists;        /**< IF EXISTS */

    qlpObjectName   * mName;            /**< sequence name */
};


/**
 * @brief view definition
 */
struct qlpViewDef
{
    qllNodeType          mType;             /**< node type */
    stlInt32             mNodePos;          /**< node position */

    stlBool              mOrReplace;        /**< OR REPLACE option */
    stlBool              mIsForce;          /**< FORCE option */
    
    qlpObjectName      * mViewName;         /**< view name */
    qlpList            * mColumnList;       /**< list of string */

    qlpQueryExpr       * mViewQuery;        /**< Query Expression */

    stlChar            * mViewColumnString;  /**< View Column String */
    stlChar            * mViewQueryString;   /**< View Query String */
};

/**
 * @brief drop view 
 */
struct qlpDropView
{
    qllNodeType       mType;            /**< node type */
    stlInt32          mNodePos;         /**< node position */

    stlBool           mIfExists;        /**< IF EXISTS */

    qlpObjectName   * mViewName;        /**< view name */
};


/**
 * @brief ALTER VIEW action
 */
typedef enum qlpAlterViewAction
{
    QLP_ALTER_VIEW_ACTION_NA = 0,       /**< N/A */

    QLP_ALTER_VIEW_ACTION_COMPILE,      /**< COMPILE */

    QLP_ALTER_VIEW_ACTION_MAX
} qlpAlterViewAction;


/**
 * @brief Alter view 
 */
struct qlpAlterView
{
    qllNodeType          mType;            /**< node type */
    stlInt32             mNodePos;         /**< node position */

    qlpAlterViewAction   mAlterAction;     /**< Alter Action */

    qlpObjectName      * mViewName;        /**< view name */
};



/**
 * @brief sequence definition : QLL_SEQUENCE_DEF_TYPE
 */
struct qlpSequenceDef
{
    qllNodeType          mType;             /**< node type */
    stlInt32             mNodePos;          /**< node position */

    qlpObjectName      * mName;             /**< sequence name */
    qlpSequenceOption  * mOptions;          /**< sequence options */
};


/**
 * @brief ALTER SEQUENCE
 */
struct qlpAlterSequence
{
    qllNodeType          mType;             /**< node type */
    stlInt32             mNodePos;          /**< node position */

    qlpObjectName      * mName;             /**< sequence name */
    qlpSequenceOption  * mOptions;          /**< sequence options */
};

/**
 * @brief synonym definition : QLL_STMT_CREATE_SYNONYM_TYPE
 */
struct qlpSynonymDef
{
    qllNodeType          mType;             /**< node type */
    stlInt32             mNodePos;          /**< node position */

    stlBool              mOrReplace;        /**< OR REPLACE option */
    stlBool              mIsPublic;         /**< is PUBLIC */

    qlpObjectName      * mSynonymName;      /**< synonym name */
    qlpObjectName      * mObjectName;       /**< object name */
};

/**
 * @brief synonym definition : QLL_STMT_DROP_SYNONYM_TYPE
 */
struct qlpDropSynonym
{
    qllNodeType          mType;             /**< node type */
    stlInt32             mNodePos;          /**< node position */
    
    stlBool              mIsPublic;         /**< is PUBLIC */
    stlBool              mIfExists;         /**< IF EXISTS */

    qlpObjectName      * mSynonymName;      /**< synonym name */
};


/**
 * @brief index definition : QLL_INDEX_DEF_TYPE
 */
struct qlpIndexDef
{
    qllNodeType     mType;             /**< node type */
    stlInt32        mNodePos;          /**< node position */

    stlInt32        mIsUniquePos;         /**< position : is index on UK */
    stlInt32        mIsPrimaryPos;        /**< position : is index on PK */
    stlInt32        mIsFromConstraintPos; /**< position : is it from a CONSTRAINT clause ? */
    
    stlBool         mIsUnique;         /**< is index on UK */
    stlBool         mIsPrimary;        /**< is index on PK */
    stlBool         mIsFromConstraint; /**< is it from a CONSTRAINT clause ? */
    qlpObjectName * mName;             /**< index name */
    qlpRelInfo    * mRelInfo;          /**< relation info */
    qlpList       * mElems;            /**< index elements : qlpIndexElement */

    qlpIndexAttr    mIndexAttr;        /**< index attribute */
    
    qlpValue      * mTablespaceName;   /**< tablespace name */
};


/**
 * @brief drop index : QLL_DROP_INDEX_TYPE
 */
struct qlpDropIndex
{
    qllNodeType       mType;            /**< node type */
    stlInt32          mNodePos;         /**< node position */

    stlBool           mIfExists;        /**< IF EXISTS */
    
    qlpObjectName   * mName;            /**< index name */
};








/**
 * @brief select : QLL_STMT_SELECT_TYPE, QLL_STMT_SELECT_INTO_TYPE
 */
struct qlpSelect
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */

    stlBool            mHasHintErr;     /**< Has Hint Syntax Error */

    qllNode               * mQueryNode;    /**< Query Node */
    qlpIntoClause         * mInto;         /**< INTO clause */
    qlpUpdatableClause    * mUpdatability; /**< FOR UPDATE or FOR READ ONLY */
};




/**
 * @brief query expr : QLL_STMT_QUERY_EXPR_TYPE, QLL_STMT_QUERY_EXPR_INTO_TYPE
 */
struct qlpQueryExpr
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */

    qlpList          * mHints;          /**< list : qlpHint */
    qlpList          * mTargets;        /**< list : qlpTarget */
    qlpIntoClause    * mInto;           /**< INTO clause */
    qlpRelInfo       * mFrom;           /**< FROM clause */
    qllNode          * mWhere;          /**< WHERE qualification */

    qlpValue         * mDumpOpt;        /**< Dump option string for Dump Table */

    qllNode          * mResultSkip;     /**< OFFSET skip_count */
    qllNode          * mResultLimit;    /**< LIMIT fetch_count */
};


/**
 * @brief insert : QLL_INSERT_TYPE
 */
struct qlpInsert
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */

    stlBool            mHasHintErr;     /**< Has Hint Syntax Error */

    qlpRelInfo       * mRelation;       /**< relation info (primitive) */
    qlpInsertSource  * mSource;         /**< target columns & values */

    /*
     * INSERT .. RETURNING 을 위한 정보
     */
    
    stlInt32           mBaseInsertType; /**< INSERT_VALUES or INSERT_SELECT */

    qlpList          * mReturnTarget;   /**< RETURNING : qlpTarget */
    qlpIntoClause    * mIntoTarget;     /**< INTO  */
};


/**
 * @brief delete : QLL_DELETE_TYPE
 */
struct qlpDelete
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */

    stlBool            mHasHintErr;     /**< Has Hint Syntax Error */

    qlpRelInfo       * mRelation;       /**< relation info (primitive) */
    qlpList          * mHints;          /**< list : qlpHint */
    qllNode          * mWhere;          /**< WHERE qualification */

    qllNode          * mResultSkip;     /**< OFFSET skip_count */
    qllNode          * mResultLimit;    /**< LIMIT fetch_count */

    qlpList          * mReturnTarget;   /**< RETURNING : qlpTarget */
    qlpIntoClause    * mIntoTarget;     /**< INTO  */

    qlpValue         * mCursorName;     /**< WHERE CURRENT OF cursor */
};


/**
 * @brief update : QLL_UPDATE_TYPE
 */
struct qlpUpdate
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */

    stlBool            mHasHintErr;     /**< Has Hint Syntax Error */

    qlpRelInfo       * mRelation;       /**< relation info (primitive) */
    qlpList          * mHints;          /**< list : qlpHint */
    qlpList          * mSet;            /**< qlpTarget : list of target */
    qllNode          * mWhere;          /**< WHERE qualification */

    qllNode          * mResultSkip;     /**< OFFSET skip_count */
    qllNode          * mResultLimit;    /**< LIMIT fetch_count */

    stlBool            mIsReturnNew;    /**< RETURNING NEW or OLD */
    qlpList          * mReturnTarget;   /**< RETURNING : qlpTarget */
    qlpIntoClause    * mIntoTarget;     /**< INTO  */

    qlpValue         * mCursorName;     /**< WHERE CURRENT OF cursor */
};


/**
 * @brief savepoint : QLL_SAVEPOINT_TYPE
 */
struct qlpSavepoint
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */

    stlChar          * mName;           /**< savepoint name */
    stlInt32           mNamePos;        /**< position : savepoint name */
};


/**
 * @brief commit : QLL_COMMIT_TYPE
 */
struct qlpCommit
{
    qllNodeType        mType;          /**< node type */
    stlInt32           mNodePos;       /**< node position */
    
    qlpValue         * mForce;         /**< xid string for force clause */
    qlpValue         * mComment;       /**< comment string */
    stlInt32           mWriteMode;     /**< lock mode */
};


/**
 * @brief rollback : QLL_ROLLBACK_TYPE
 */
struct qlpRollback
{
    qllNodeType        mType;              /**< node type */
    stlInt32           mNodePos;           /**< node position */

    stlInt32           mSavepointNamePos;  /**< position : savepoint name */
    
    stlChar          * mSavepointName;     /**< savepoint name */
    qlpValue         * mForce;         /**< xid string for force clause */
    qlpValue         * mComment;       /**< comment string */
};


/**
 * @brief lock table: QLL_LOCK_TABLE_TYPE
 */
struct qlpLockTable
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */

    stlInt32           mLockModePos;    /**< position of lock mode */

    qlpList          * mRelations;      /**< target relation list */
    qlpLockMode        mLockMode;       /**< lock mode */
    qlpValue         * mWaitTime;       /**< wait time : null means infinity time */
};


/**
 * @brief [DEFERRED] [SCOPE]
 */
struct qlpSetParamAttr
{
    qllNodeType            mType;       /**< node type */
    stlInt32               mNodePos;    /**< node position */

    stlBool                mIsDeferred; /**< DEFERRED */
    knlPropertyModifyScope mScope;      /**< SCOPE */
};
    
/**
 * @brief parameter = value, parameter to default
 */
struct qlpSetParam
{
    qllNodeType            mType;           /**< node type */
    stlInt32               mNodePos;        /**< node position */

    qlpValue             * mPropertyName;   /**< property Name */
    qlpValue             * mSetValue;       /**< property value for SET
                                             *   DEFAULT : mSetValue = NULL */

    stlBool                mIsDeferred;     /**< DEFERRED */
    knlPropertyModifyScope mScope;          /**< SCOPE */
};


/**
 * @brief reset/unset parameter
 */
struct qlpResetParam
{
    qllNodeType            mType;           /**< node type */
    stlInt32               mNodePos;        /**< node position */

    qlpValue             * mPropertyName;   /**< property Name */

    stlBool                mIsDeferred;     /**< DEFERRED */
    knlPropertyModifyScope mScope;          /**< SCOPE */
};


/**
 * @brief breakpoint action
 *   */
struct qlpBreakpointAction
{
    qllNodeType                mType;            /**< node type */
    stlInt32                   mNodePos;         /**< node position */

    knlBreakPointAction        mActionType;      /**< owner session Name */
    knlBreakPointPostAction    mPostActionType;  /**< post action type */
    qlpValue                 * mArgument;        /**< breakpoint option */
};


/**
 * @brief breakpoint option
 *   */
struct qlpBreakpointOption
{
    qllNodeType            mType;        /**< node type */
    stlInt32               mNodePos;     /**< node position */

    qlpValue             * mSkipCount;   /**< breakpoint Name */
    qlpBreakpointAction  * mAction;      /**< action info */
};


/**
 * @brief alter system set/reset/ager
 * : QLL_STMT_ALTER_SYSTEM_SET_TYPE, QLL_STMT_ALTER_SYSTEM_RESET_TYPE
 */
struct qlpAlterSystem
{
    qllNodeType            mType;           /**< node type */
    stlInt32               mNodePos;        /**< node position */

    qlpValue             * mPropertyName;   /**< property Name */
    qlpValue             * mSetValue;       /**< property value for SET
                                             *   RESET : mSetValue = NULL */

    stlBool                mIsDeferred;     /**< DEFERRED */
    knlPropertyModifyScope mScope;          /**< SCOPE */
};


/**
 * @brief alter system set/reset/ager
 * : QLL_STMT_ALTER_SYSTEM_AGER_TYPE
 */
struct qlpAlterSystemAger
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */

    qlpAgerBehavior    mBehavior;       /**< START,STOP,LOOPBACK */
};


/**
 * @brief Alter System breakpoint
 * : QLL_STMT_ALTER_SYSTEM_BREAKPOINT_TYPE
 */
struct qlpAlterSystemBreakpoint
{
    qllNodeType             mType;             /**< node type */
    stlInt32                mNodePos;          /**< node position */

    knlBreakPointSessType   mSessType;         /**< breakpoint session type */
    qlpValue              * mBreakpointName;   /**< breakpoint Name */
    qlpValue              * mOwnerSessName;    /**< owner session Name */

    qlpBreakpointOption   * mOption;           /**< breakpoint option */
};


/**
 * @brief Alter System session name
 * : QLL_STMT_ALTER_SYSTEM_SESS_NAME_TYPE
 */
struct qlpAlterSystemSessName
{
    qllNodeType            mType;             /**< node type */
    stlInt32               mNodePos;          /**< node position */

    qlpValue             * mSessName;         /**< Session Name */
};


/**
 * @brief alter system mount/open database
 * : QLL_STMT_ALTER_SYSTEM_STARTUP_DATABASE_TYPE
 */
struct qlpAlterSystemStartupDatabase
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */

    knlStartupPhase    mStartupPhase;   /**< MOUNT/RESTRICT/SERVCE */
    qlpValue         * mOpenOption;     /**< NULL/READ_ONLY/READ_WRITE */
    qlpValue         * mLogOption;      /**< NORESETLOGS/RESETLOGS */
};


/**
 * @brief alter system close database
 * : QLL_STMT_ALTER_SYSTEM_SHUTDOWN_DATABASE_TYPE
 */
struct qlpAlterSystemShutdownDatabase
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */

    knlShutdownPhase   mShutdownPhase;  /**< CLOSE */
    qlpValue         * mCloseOption; /**< NULL/NORMAL/TRANSACTIONAL/IMMEDIATE/ABORT/KILL */
};


/**
 * @brief alter system switch logfile : QLL_STMT_ALTER_SYSTEM_SWITCH_LOGFILE_TYPE
 */
struct qlpAlterSystemSwitchLogfile
{
    qllNodeType          mType;           /**< node type */
    stlInt32             mNodePos;        /**< node position */
};


/**
 * @brief alter system checkpoint
 * : QLL_STMT_ALTER_SYSTEM_CHECKPOINT_TYPE
 */
struct qlpAlterSystemChkpt
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */
};


/**
 * @brief alter system flush log
 * : QLL_STMT_ALTER_SYSTEM_FLUSH_LOG_TYPE
 */
struct qlpAlterSystemFlushLog
{
    qllNodeType           mType;           /**< node type */
    stlInt32              mNodePos;        /**< node position */
    qlpFlushLogBehavior   mBehavior;       /**< FLUSH LOGS, START FLUSH LOGS, STOP FLUSH LOGS */
};


/**
 * @brief ALTER SYSTEM CLEANUP PLAN
 * : QLL_STMT_ALTER_SYSTEM_CLEANUP_PLAN
 */
struct qlpAlterSystemCleanupPlan
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */
};


/**
 * @brief ALTER SYSTEM CLEANUP SESSION
 * : QLL_STMT_ALTER_SYSTEM_CLEANUP_SESSION
 */
struct qlpAlterSystemCleanupSession
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */
};


/**
 * @brief alter system disconnect session
 * : QLL_STMT_ALTER_SYSTEM_DISCONNECT_SESSION_TYPE
 */
struct qlpAlterSystemDisconnSession
{
    qllNodeType        mType;              /**< node type */
    stlInt32           mNodePos;           /**< node position */
    qlpValue         * mSessionID;         /**< session ID */
    qlpValue         * mSerialNum;         /**< serial# */
    stlBool            mIsPostTransaction; /**< post transaction */
    qlpValue         * mOption;            /**< option */
};


/**
 * @brief alter system kill session
 * : QLL_STMT_ALTER_SYSTEM_KILL_SESSION_TYPE
 */
struct qlpAlterSystemKillSession
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */
    qlpValue         * mSessionID;      /**< session ID */
    qlpValue         * mSerialNum;      /**< serial# */
};


/**
 * @brief alter session set/reset
 * : QLL_STMT_ALTER_SESSION_SET_TYPE
 */
struct qlpAlterSession
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */

    qlpValue         * mPropertyName;   /**< property Name */
    qlpValue         * mSetValue;       /**< property value for SET
                                         *   RESET : mSetValue = NULL */
};



/**
 * @brief comment on ... is ...
 * : QLL_STMT_COMMENT_ON_TYPE
 */
struct qlpCommentOnStmt
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */

    qlpCommentObject * mCommentObject;  /**< ON object */
    qlpValue         * mCommentString;  /**< IS comment */
};


/**
 * @brief set constraint 
 * : QLL_STMT_SET_CONSTRAINTS_TYPE
 */
struct qlpSetConstraint
{
    qllNodeType            mType;            /**< node type */
    stlInt32               mNodePos;         /**< node position */

    qllSetConstraintMode   mConstMode;       /**< Constraint Mode */
    qlpList              * mConstNameList;   /**< Constraint Name List */
};

/**
 * @brief transaction mode
 * : QLL_SET_TRANSACTION_MODE
 */
struct qlpTransactionMode
{
    qllNodeType          mType;            /**< node type */
    stlInt32             mNodePos;         /**< node position */

    qlpTransactionAttr   mAttr;            /**< transacton attr */
    qlpValue           * mValue;           /**< transacton mode value */
};

/**
 * @brief set [local] transaction
 * : QLL_STMT_SET_TRANSACTION_TYPE
 */
struct qlpSetTransaction
{
    qllNodeType          mType;            /**< node type */
    stlInt32             mNodePos;         /**< node position */

    qlpTransactionAttr   mAttr;            /**< transacton attr */
    qlpValue           * mValue;           /**< transacton value */
};


/**
 * @brief set session authorization
 */
struct qlpSetSessionAuth
{
    qllNodeType          mType;            /**< node type */
    stlInt32             mNodePos;         /**< node position */

    qlpValue           * mUserName;        /**< User Name */
};

/**
 * @brief set session characteristics
 * : QLL_STMT_SET_SESSION_CHARACTERISTICS_TYPE
 */
struct qlpSetSessionCharacteristics
{
    qllNodeType          mType;            /**< node type */
    stlInt32             mNodePos;         /**< node position */

    qlpTransactionAttr   mAttr;            /**< transacton attr */
    qlpValue           * mValue;           /**< transacton value */
};

/**
 * @brief set time zone
 * : QLL_STMT_SET_TIMEZONE_TYPE
 */
struct qlpSetTimeZone
{
    qllNodeType   mType;            /**< node type */
    stlInt32      mNodePos;         /**< node position */

    qlpValue    * mValue;           /**< time zone value */
};

/**
 * @brief declare .. cursor
 */
struct qlpDeclareCursor
{
    qllNodeType          mType;            /**< node type */
    stlInt32             mNodePos;         /**< node position */

    qlpValue           * mCursorName;

    qlpValue           * mStandardType;    /**< Standard Type */
    
    qlpValue           * mSensitivity;     /**< Sensitivity */
    qlpValue           * mScrollability;   /**< Scrollability */
    qlpValue           * mHoldability;     /**< Holdability */

    qllNode            * mCursorQuery;     /**< Cursor Query */
};


/**
 * @brief open cursor
 */
struct qlpOpenCursor
{
    qllNodeType          mType;            /**< node type */
    stlInt32             mNodePos;         /**< node position */

    qlpValue           * mCursorName;
};

/**
 * @brief close cursor
 */
struct qlpCloseCursor
{
    qllNodeType          mType;            /**< node type */
    stlInt32             mNodePos;         /**< node position */

    qlpValue           * mCursorName;
};


/**
 * @brief fetch cursor
 */
struct qlpFetchCursor
{
    qllNodeType           mType;              /**< node type */
    stlInt32              mNodePos;           /**< node position */

    qlpValue            * mCursorName;

    qllFetchOrientation   mFetchOrient;
    qllNode             * mFetchPosition;

    qlpIntoClause       * mIntoList;
};


/*******************************************************************************
 * PHRASE - for syntax replacement 
 ******************************************************************************/

/**
 * @brief phrase viewed table
 */
struct qlpPhraseViewedTable
{
    qllNodeType          mType;             /**< node type */
    stlInt32             mNodePos;          /**< node position */

    qlpObjectName      * mViewName;         /**< view name */
    qlpList            * mColumnList;       /**< list of string */
};

/**
 * @brief phrase default expression
 */
struct qlpPhraseDefaultExpr
{
    qllNodeType          mType;             /**< node type */
    stlInt32             mNodePos;          /**< node position */

    qlpValueExpr       * mDefaultExpr;      /**< DEFAULT expression */
};

/*******************************************************************************
 * FUNCTIONS 
 ******************************************************************************/
    



    // QLL_QUERY_TYPE,        /* not-planned queries */
    // QLL_PLANNEDSTMT_TYPE,


    // QLL_INSERTSTMT_TYPE,
    // QLL_DELETESTMT_TYPE,
    // QLL_UPDATESTMT_TYPE,



    // QLL_UTILITY_TYPE,    이건 없었는데... (commit, alter system...)






    // QLL_ALTERTABLESTMT_TYPE,
    // QLL_ALTERTABLECMD_TYPE,
    // QLL_ALTERDOMAINSTMT_TYPE,
    // QLL_SETOPERATIONSTMT_TYPE,
    // QLL_GRANTSTMT_TYPE,
    // QLL_GRANTROLESTMT_TYPE,
    // QLL_ALTERDEFAULTPRIVILEGESSTMT_TYPE,
    // QLL_CLOSEPORTALSTMT_TYPE,
    // QLL_CLUSTERSTMT_TYPE,
    // QLL_COPYSTMT_TYPE,
    // QLL_DEFINESTMT_TYPE,
    // QLL_COMMENTSTMT_TYPE,
    // QLL_FETCHSTMT_TYPE,

    // QLL_CREATEFUNCTIONSTMT_TYPE,
    // QLL_ALTERFUNCTIONSTMT_TYPE,
    // QLL_REMOVEFUNCSTMT_TYPE,
    // QLL_DOSTMT_TYPE,
    // QLL_RENAMESTMT_TYPE,
    // QLL_RULESTMT_TYPE,
    // QLL_NOTIFYSTMT_TYPE,
    // QLL_LISTENSTMT_TYPE,
    // QLL_UNLISTENSTMT_TYPE,
    // QLL_TRANSACTIONSTMT_TYPE,
    // QLL_VIEWSTMT_TYPE,
    // QLL_LOADSTMT_TYPE,
    // QLL_CREATEDOMAINSTMT_TYPE,
    // QLL_CREATEDBSTMT_TYPE,
    // QLL_DROPDBSTMT_TYPE,
    // QLL_VACUUMSTMT_TYPE,
    // QLL_EXPLAINSTMT_TYPE,
    // QLL_CREATESEQSTMT_TYPE,
    // QLL_ALTERSEQSTMT_TYPE,
    // QLL_VARIABLESETSTMT_TYPE,
    // QLL_VARIABLESHOWSTMT_TYPE,
    // QLL_DISCARDSTMT_TYPE,
    // QLL_CREATETRIGSTMT_TYPE,
    // QLL_DROPPROPERTYSTMT_TYPE,
    // QLL_CREATEPLANGSTMT_TYPE,
    // QLL_DROPPLANGSTMT_TYPE,
    // QLL_CREATEROLESTMT_TYPE,
    // QLL_ALTERROLESTMT_TYPE,
    // QLL_DROPROLESTMT_TYPE,
    // QLL_CONSTRAINTSSETSTMT_TYPE,
    // QLL_REINDEXSTMT_TYPE,
    // QLL_CHECKPOINTSTMT_TYPE,
    // QLL_CREATESCHEMASTMT_TYPE,
    // QLL_ALTERDATABASESTMT_TYPE,
    // QLL_ALTERDATABASESETSTMT_TYPE,
    // QLL_ALTERROLESETSTMT_TYPE,
    // QLL_CREATECONVERSIONSTMT_TYPE,
    // QLL_CREATECASTSTMT_TYPE,
    // QLL_DROPCASTSTMT_TYPE,
    // QLL_CREATEOPCLASSSTMT_TYPE,
    // QLL_CREATEOPFAMILYSTMT_TYPE,
    // QLL_ALTEROPFAMILYSTMT_TYPE,
    // QLL_REMOVEOPCLASSSTMT_TYPE,
    // QLL_REMOVEOPFAMILYSTMT_TYPE,
    // QLL_PREPARESTMT_TYPE,
    // QLL_EXECUTESTMT_TYPE,
    // QLL_DEALLOCATESTMT_TYPE,
    // QLL_DECLARECURSORSTMT_TYPE,
    // QLL_ALTEROBJECTSCHEMASTMT_TYPE,
    // QLL_ALTEROWNERSTMT_TYPE,
    // QLL_DROPOWNEDSTMT_TYPE,
    // QLL_REASSIGNOWNEDSTMT_TYPE,
    // QLL_COMPOSITETYPESTMT_TYPE,
    // QLL_CREATEENUMSTMT_TYPE,
    // QLL_ALTERTSDICTIONARYSTMT_TYPE,
    // QLL_ALTERTSCONFIGURATIONSTMT_TYPE,
    // QLL_CREATEFDWSTMT_TYPE,
    // QLL_ALTERFDWSTMT_TYPE,
    // QLL_DROPFDWSTMT_TYPE,
    // QLL_CREATEFOREIGNSERVERSTMT_TYPE,
    // QLL_ALTERFOREIGNSERVERSTMT_TYPE,
    // QLL_DROPFOREIGNSERVERSTMT_TYPE,
    // QLL_CREATEUSERMAPPINGSTMT_TYPE,
    // QLL_ALTERUSERMAPPINGSTMT_TYPE,
    // QLL_DROPUSERMAPPINGSTMT_TYPE,
    // QLL_ALTERTABLESPACEOPTIONSSTMT_TYPE,


/** @} */

#endif /* _QLPNODESTATEMENT_H_ */



