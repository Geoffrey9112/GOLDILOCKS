/*******************************************************************************
 * ztqDef.h
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


#ifndef _ZTQDEF_H_
#define _ZTQDEF_H_ 1

#include <dtl.h>
#include <xa.h>

/**
 * @file ztqDef.h
 * @brief Gliese SQL Definition
 */

/**
 * @defgroup ztq Gliese SQL
 * @{
 */

/**
 * @defgroup ztqGeneral General
 * @{
 */



typedef enum
{
    ZTQ_COMMENT_NA = 0,  /* N/A */
    
    ZTQ_COMMENT_NO_COMMENT,
    ZTQ_COMMENT_SINGLE_QUOTE,
    ZTQ_COMMENT_DOUBLE_QUOTE,
    ZTQ_COMMENT_LINE_COMMENT,
    ZTQ_COMMENT_BRACKET_COMMENT,
    ZTQ_COMMENT_IS_BEGIN_MARK,
    ZTQ_COMMENT_IS_END_MARK,
    
    ZTQ_COMMENT_MAX
} ztqCommentMark;


#define ZTQ_MIN_LINE_SIZE                 (1)
#define ZTQ_MIN_PAGE_SIZE                 (1)
#define ZTQ_MIN_COL_SIZE                  (1)
#define ZTQ_MIN_NUM_SIZE                  (1)
#define ZTQ_MIN_DDL_SIZE                  (1)
#define ZTQ_MIN_HISTORY_BUFFER_SIZE       (0)

#define ZTQ_MAX_LINE_SIZE                 (10000)
#define ZTQ_MAX_PAGE_SIZE                 (10000)
#define ZTQ_MAX_COL_SIZE                  (100*1024*1024)
#define ZTQ_MAX_NUM_SIZE                  (50)
#define ZTQ_MAX_DDL_SIZE                  (10*1024*1024) 
#define ZTQ_MAX_HISTORY_BUFFER_SIZE       (100000)

#define ZTQ_DEFAULT_LINE_SIZE             (80)
#define ZTQ_DEFAULT_PAGE_SIZE             (20)
#define ZTQ_DEFAULT_COL_SIZE              (8192)
#define ZTQ_DEFAULT_NUM_SIZE              (20)
#define ZTQ_DEFAULT_DDL_SIZE              (10000)
#define ZTQ_DEFAULT_HISTORY_BUFFER_SIZE   (128)
#define ZTQ_DEFAULT_SPOOL_BUFFER_SIZE     (8192)

#define ZTQ_MAX_COMMAND_BUFFER_SIZE       (8192)
#define ZTQ_MAX_TABLE_NAME                (128)
#define ZTQ_MAX_COLUMN_NAME               (128)
#define ZTQ_NUMBER_STRING_MAX_SIZE        ZTQ_MAX_NUM_SIZE

#define ZTQ_REGION_INIT_SIZE              (1024*1024)  /* 1M */
#define ZTQ_REGION_NEXT_SIZE              (8192)       /* 8K */

typedef struct ztqCommandBuffer
{
    stlChar * mBuffer;
    stlInt32  mLength;
    stlInt32  mPosition;
} ztqCommandBuffer;

#define ZTQ_INIT_COMMAND_BUFFER( aCommandBuffer, aBuffer, aLength )  \
{                                                                    \
    (aCommandBuffer)->mBuffer = aBuffer;                             \
    (aCommandBuffer)->mBuffer[0] = '\0';                             \
    (aCommandBuffer)->mLength = aLength;                             \
    (aCommandBuffer)->mPosition = 0;                                 \
}

#define ZTQ_GET_BUFFER( aCommandBuffer )  ( (aCommandBuffer)->mBuffer )
#define ZTQ_SET_BUFFER( aCommandBuffer )  ( (aCommandBuffer)->mBuffer )

typedef enum ztqOperationMode
{
    ZTQ_OPERATION_MODE_DA = 0,
    ZTQ_OPERATION_MODE_CS
} ztqOperationMode;

extern ztqOperationMode gZtqOperationMode;

/** @} */

/**
 * @addtogroup ztqHostVariable
 * @{
 */

#define ZTQ_MAX_HOST_VALUE_SIZE  (DTL_VARCHAR_MAX_CHAR_STRING_SIZE)

/**
 * @brief Host Variable 의 Bind Type
 */
typedef enum ztqHostBindType
{
    ZTQ_HOST_BIND_TYPE_NA = 0,   /**< N/A */
    ZTQ_HOST_BIND_TYPE_IN,       /**< IN Parameter */
    ZTQ_HOST_BIND_TYPE_OUT,      /**< OUT Parameter */
    ZTQ_HOST_BIND_TYPE_INDICATOR /**< Indicator */
} ztqHostBindType;
    
typedef struct ztqHostVariable
{
    stlChar             mName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    ztqHostBindType     mBindType;
    SQLLEN              mIndicator;
    stlInt32            mDataType;
    stlInt32            mPrecision;
    stlInt32            mScale;
    dtlStringLengthUnit mStringLengthUnit;
    stlChar             mValue[ZTQ_MAX_HOST_VALUE_SIZE];
    void              * mIndicatorVariable;
    void              * mNext;
} ztqHostVariable;

#define ZTQ_INIT_HOST_VARIABLE( aHostVariable )                     \
    {                                                               \
        (aHostVariable)->mName[0] = '\0';                           \
        (aHostVariable)->mIndicator = SQL_NULL_DATA;                \
        stlMemset( (void*)((aHostVariable)->mValue),                \
                   0x00,                                            \
                   ZTQ_MAX_HOST_VALUE_SIZE );                       \
        (aHostVariable)->mIndicatorVariable = NULL;                 \
        (aHostVariable)->mNext = NULL;                              \
        (aHostVariable)->mDataType = SQL_UNKNOWN_TYPE;              \
        (aHostVariable)->mPrecision = -1;                           \
        (aHostVariable)->mScale = -1;                               \
    }

#define ZTQ_SYS_VAR_ELAPSED_TIME  "VAR_ELAPSED_TIME__"


/** @} */



/**
 * @addtogroup ztqNamedCursor
 * @{
 */

typedef struct ztqNamedCursor
{
    stlChar           mCursorName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    ztqHostVariable * mHostVarList;

    struct ztqNamedCursor   * mNext;
} ztqNamedCursor;

/** @} */



/**
 * @defgroup ztqParser Parser
 * @{
 */

/******************************************************************************* 
 * KEY WORD
 ******************************************************************************/

#define ZTQ_CMD_KEY_WORD_COUNT( list )   ( STL_SIZEOF( list ) / STL_SIZEOF( list[0] ) )


/**
 * @brief keyword 정보
 */
typedef struct ztqCmdKeyWord
{
    const stlChar * mName;       /**< keyword name : in upper case */
    stlInt16        mTokenCode;  /**< token code for keyword*/
} ztqCmdKeyWord;

typedef enum
{
    ZTQ_COMMAND_TYPE_SQL_TRANSACTION,
    ZTQ_COMMAND_TYPE_SQL_BYPASS,
    ZTQ_COMMAND_TYPE_CMD_EXEC_SQL,
    ZTQ_COMMAND_TYPE_CMD_PREPARE_SQL,
    ZTQ_COMMAND_TYPE_CMD_DYNAMIC_SQL,
    ZTQ_COMMAND_TYPE_CMD_EXEC_PREPARED,
    ZTQ_COMMAND_TYPE_CMD_EXEC_HISTORY,
    ZTQ_COMMAND_TYPE_CMD_EXEC_ASSIGN,
    
    ZTQ_COMMAND_TYPE_CMD_SET_CALLSTACK,
    ZTQ_COMMAND_TYPE_CMD_SET_VERTICAL,
    ZTQ_COMMAND_TYPE_CMD_SET_TIMING,
    ZTQ_COMMAND_TYPE_CMD_SET_VERBOSE,
    ZTQ_COMMAND_TYPE_CMD_SET_LINESIZE,
    ZTQ_COMMAND_TYPE_CMD_SET_PAGESIZE,
    ZTQ_COMMAND_TYPE_CMD_SET_COLSIZE,
    ZTQ_COMMAND_TYPE_CMD_SET_NUMSIZE,
    ZTQ_COMMAND_TYPE_CMD_SET_DDLSIZE,
    ZTQ_COMMAND_TYPE_CMD_SET_AUTOCOMMIT,
    ZTQ_COMMAND_TYPE_CMD_SET_RESULT,
    ZTQ_COMMAND_TYPE_CMD_SET_HISTORY,
    ZTQ_COMMAND_TYPE_CMD_SET_COLOR,
    ZTQ_COMMAND_TYPE_CMD_SET_ERROR,
    
    ZTQ_COMMAND_TYPE_CMD_QUIT,
    ZTQ_COMMAND_TYPE_CMD_HELP,
    ZTQ_COMMAND_TYPE_CMD_PRINT,
    ZTQ_COMMAND_TYPE_CMD_DESC,
    ZTQ_COMMAND_TYPE_CMD_IDESC,
    ZTQ_COMMAND_TYPE_CMD_VAR,
    ZTQ_COMMAND_TYPE_CMD_IMPORT,
    ZTQ_COMMAND_TYPE_CMD_PRINT_HISTORY,
    ZTQ_COMMAND_TYPE_CMD_EXPLAIN_PLAN,
    ZTQ_COMMAND_TYPE_CMD_ALLOC_STMT,
    ZTQ_COMMAND_TYPE_CMD_FREE_STMT,
    ZTQ_COMMAND_TYPE_CMD_USE_STMT_EXEC_SQL,
    ZTQ_COMMAND_TYPE_CMD_USE_STMT_PREPARE_SQL,
    ZTQ_COMMAND_TYPE_CMD_USE_STMT_EXEC_PREPARED,
    
    ZTQ_COMMAND_TYPE_CMD_STARTUP,
    ZTQ_COMMAND_TYPE_CMD_SHUTDOWN,
    ZTQ_COMMAND_TYPE_CMD_CONNECT,
    
    ZTQ_COMMAND_TYPE_CMD_XA_OPEN,
    ZTQ_COMMAND_TYPE_CMD_XA_CLOSE,
    ZTQ_COMMAND_TYPE_CMD_XA_START,
    ZTQ_COMMAND_TYPE_CMD_XA_END,
    ZTQ_COMMAND_TYPE_CMD_XA_COMMIT,
    ZTQ_COMMAND_TYPE_CMD_XA_ROLLBACK,
    ZTQ_COMMAND_TYPE_CMD_XA_PREPARE,
    ZTQ_COMMAND_TYPE_CMD_XA_RECOVER,
    ZTQ_COMMAND_TYPE_CMD_XA_FORGET,
    
    ZTQ_COMMAND_TYPE_CMD_DDL_DB,
    ZTQ_COMMAND_TYPE_CMD_DDL_SPACE,
    ZTQ_COMMAND_TYPE_CMD_DDL_PROFILE,
    ZTQ_COMMAND_TYPE_CMD_DDL_AUTH,
    ZTQ_COMMAND_TYPE_CMD_DDL_SCHEMA,
    ZTQ_COMMAND_TYPE_CMD_DDL_PUBLIC_SYNONYM,
    ZTQ_COMMAND_TYPE_CMD_DDL_TABLE,
    ZTQ_COMMAND_TYPE_CMD_DDL_VIEW,
    ZTQ_COMMAND_TYPE_CMD_DDL_SEQUENCE,
    ZTQ_COMMAND_TYPE_CMD_DDL_INDEX,
    ZTQ_COMMAND_TYPE_CMD_DDL_CONSTRAINT,
    ZTQ_COMMAND_TYPE_CMD_DDL_SYNONYM,

    ZTQ_COMMAND_TYPE_CMD_EDIT,
    ZTQ_COMMAND_TYPE_CMD_SPOOL,
    
    ZTQ_COMMAND_TYPE_MAX
} ztqCommandType;

typedef enum
{
    ZTQ_STMT_SELECT,
    ZTQ_STMT_SELECT_INTO,
    ZTQ_STMT_QUERY_EXPR,
    ZTQ_STMT_QUERY_EXPR_INTO,
    ZTQ_STMT_INSERT,
    ZTQ_STMT_UPDATE,
    ZTQ_STMT_DELETE,
    ZTQ_STMT_CREATE,
    ZTQ_STMT_DROP,
    ZTQ_STMT_ALTER,
    ZTQ_STMT_TRUNCATE,
    ZTQ_STMT_COMMIT,
    ZTQ_STMT_ROLLBACK,
    ZTQ_STMT_RELEASE,
    ZTQ_STMT_GRANT,
    ZTQ_STMT_REVOKE,
    ZTQ_STMT_SET,
    ZTQ_STMT_DECLARE_CURSOR,
    ZTQ_STMT_OPEN_CURSOR,
    ZTQ_STMT_CLOSE_CURSOR,
    ZTQ_STMT_FETCH_CURSOR,
    ZTQ_STMT_COPY,
    ZTQ_STMT_LOCK,
    ZTQ_STMT_COMMENT,
    ZTQ_STMT_CALL,
    ZTQ_STMT_MAX
} ztqStatementType;

typedef enum
{
    ZTQ_OBJECT_DATABASE,
    ZTQ_OBJECT_INDEX,
    ZTQ_OBJECT_SEQUENCE,
    ZTQ_OBJECT_SESSION,
    ZTQ_OBJECT_SYSTEM,
    ZTQ_OBJECT_SCHEMA,
    ZTQ_OBJECT_PROCEDURE,
    ZTQ_OBJECT_FUNCTION,
    ZTQ_OBJECT_TABLESPACE,
    ZTQ_OBJECT_TABLE,
    ZTQ_OBJECT_USER,
    ZTQ_OBJECT_VIEW,
    ZTQ_OBJECT_ROLE,
    ZTQ_OBJECT_CONSTRAINTS,
    ZTQ_OBJECT_SAVEPOINT,
    ZTQ_OBJECT_TRANSACTION,
    ZTQ_OBJECT_SYNONYM,
    ZTQ_OBJECT_PROFILE,
    ZTQ_OBJECT_UNKNOWN,
    ZTQ_OBJECT_MAX
} ztqObjectType;

typedef enum
{
    ZTQ_STARTUP_NOMOUNT,
    ZTQ_STARTUP_MOUNT,
    ZTQ_STARTUP_OPEN
} ztqStartupPhase;

typedef enum
{
    ZTQ_SHUTDOWN_ABORT,
    ZTQ_SHUTDOWN_OPERATIONAL,
    ZTQ_SHUTDOWN_IMMEDIATE,
    ZTQ_SHUTDOWN_TRANSACTIONAL,
    ZTQ_SHUTDOWN_NORMAL
} ztqShutdownMode;

typedef enum
{
    ZTQ_CONNECT_AS_USER = 1,
    ZTQ_CONNECT_AS_SYSDBA,
    ZTQ_CONNECT_AS_ADMIN
} ztqConnectAsUser;

typedef enum
{
    ZTQ_PRINT_DDL_NA = 0,

    ZTQ_PRINT_DDL_DB_ALL,
    ZTQ_PRINT_DDL_DB_GRANT,
    ZTQ_PRINT_DDL_DB_COMMENT,
    
    ZTQ_PRINT_DDL_SPACE_CREATE,
    ZTQ_PRINT_DDL_SPACE_ALTER,
    ZTQ_PRINT_DDL_SPACE_TABLE,
    ZTQ_PRINT_DDL_SPACE_CONSTRAINT,
    ZTQ_PRINT_DDL_SPACE_INDEX,
    ZTQ_PRINT_DDL_SPACE_GRANT,
    ZTQ_PRINT_DDL_SPACE_COMMENT,

    ZTQ_PRINT_DDL_PROFILE_CREATE,
    ZTQ_PRINT_DDL_PROFILE_COMMENT,
    
    ZTQ_PRINT_DDL_AUTH_CREATE,
    ZTQ_PRINT_DDL_AUTH_SCHEMA_PATH,
    ZTQ_PRINT_DDL_AUTH_SCHEMA,
    ZTQ_PRINT_DDL_AUTH_TABLE,
    ZTQ_PRINT_DDL_AUTH_CONSTRAINT,
    ZTQ_PRINT_DDL_AUTH_INDEX,
    ZTQ_PRINT_DDL_AUTH_VIEW,
    ZTQ_PRINT_DDL_AUTH_SEQUENCE,
    ZTQ_PRINT_DDL_AUTH_SYNONYM,
    ZTQ_PRINT_DDL_AUTH_COMMENT,
    
    ZTQ_PRINT_DDL_SCHEMA_CREATE,
    ZTQ_PRINT_DDL_SCHEMA_TABLE,
    ZTQ_PRINT_DDL_SCHEMA_CONSTRAINT,
    ZTQ_PRINT_DDL_SCHEMA_INDEX,
    ZTQ_PRINT_DDL_SCHEMA_VIEW,
    ZTQ_PRINT_DDL_SCHEMA_SEQUENCE,
    ZTQ_PRINT_DDL_SCHEMA_SYNONYM,
    ZTQ_PRINT_DDL_SCHEMA_GRANT,
    ZTQ_PRINT_DDL_SCHEMA_COMMENT,

    ZTQ_PRINT_DDL_PUBLIC_SYNONYM_CREATE,

    ZTQ_PRINT_DDL_TABLE_CREATE,
    ZTQ_PRINT_DDL_TABLE_CONSTRAINT,
    ZTQ_PRINT_DDL_TABLE_INDEX,
    ZTQ_PRINT_DDL_TABLE_IDENTITY,
    ZTQ_PRINT_DDL_TABLE_SUPPLEMENTAL,
    ZTQ_PRINT_DDL_TABLE_GRANT,
    ZTQ_PRINT_DDL_TABLE_COMMENT,

    ZTQ_PRINT_DDL_CONSTRAINT_ALTER,
    ZTQ_PRINT_DDL_CONSTRAINT_COMMENT,
    
    ZTQ_PRINT_DDL_INDEX_CREATE,
    ZTQ_PRINT_DDL_INDEX_COMMENT,
    
    ZTQ_PRINT_DDL_VIEW_CREATE,
    ZTQ_PRINT_DDL_VIEW_GRANT,
    ZTQ_PRINT_DDL_VIEW_COMMENT,

    ZTQ_PRINT_DDL_SEQUENCE_CREATE,
    ZTQ_PRINT_DDL_SEQUENCE_RESTART,
    ZTQ_PRINT_DDL_SEQUENCE_GRANT,
    ZTQ_PRINT_DDL_SEQUENCE_COMMENT,

    ZTQ_PRINT_DDL_SYNONYM_CREATE,

    ZTQ_PRINT_DDL_MAX
    
} ztqPrintDDLType;

typedef enum
{
    ZTQ_SPOOL_FLAG_BEGIN_CREATE,
    ZTQ_SPOOL_FLAG_BEGIN_REPLACE,
    ZTQ_SPOOL_FLAG_BEGIN_APPEND,
    ZTQ_SPOOL_FLAG_END,
    ZTQ_SPOOL_FLAG_STATUS,

    ZTQ_SPOOL_FLAG_MAX
} ztqSpoolFlag;

typedef struct ztqParseTree
{
    ztqCommandType mCommandType;
} ztqParseTree;

typedef struct ztqTransactionTree
{
    ztqCommandType mCommandType;
    stlBool        mIsCommit;
} ztqTransactionTree;

typedef struct ztqBypassTree
{
    ztqCommandType     mCommandType;
    ztqStatementType   mStatementType;
    ztqObjectType      mObjectType;
    stlChar          * mObjectName;
    ztqHostVariable  * mHostVarList;
} ztqBypassTree;

typedef struct ztqCmdParseParam
{
    void            * mYyScanner;
    stlAllocator    * mAllocator;
    stlErrorStack   * mErrorStack;
    stlStatus         mErrStatus;
    stlChar         * mErrMsg;
    stlChar         * mBuffer;
    stlInt32          mPosition;
    stlInt32          mLength;
    stlInt32          mLineNo;
    stlInt32          mColumn;
    stlInt32          mColumnLen;
    stlInt32          mCurPosition;
    stlInt32          mSqlPosition;
    ztqParseTree    * mParseTree;
} ztqCmdParseParam;

typedef struct ztqSqlParseParam
{
    stlAllocator    * mAllocator;
    ztqHostVariable * mHostVarList;
} ztqSqlParseParam;

typedef struct ztqDataType
{
    stlInt32            mDataType;
    stlInt32            mPrecision;
    stlInt32            mScale;
    dtlStringLengthUnit mStringLengthUnit;
} ztqDataType;

typedef struct ztqIdentifier
{
    stlChar * mIdentifier;
    void    * mNext;
} ztqIdentifier;

typedef struct ztqCmdExecSqlTree
{
    ztqCommandType   mCommandType;
    stlInt32         mSqlStartPos;
} ztqCmdExecSqlTree;

typedef struct ztqCmdExecPreparedTree
{
    ztqCommandType   mCommandType;
} ztqCmdExecPreparedTree;

typedef struct ztqCmdPrepareSqlTree
{
    ztqCommandType   mCommandType;
    stlInt32         mSqlStartPos;
} ztqCmdPrepareSqlTree;

typedef struct ztqCmdDynamicSqlTree
{
    ztqCommandType    mCommandType;
    stlChar           mVariableName[STL_MAX_SQL_IDENTIFIER_LENGTH];
} ztqCmdDynamicSqlTree;

typedef struct ztqCmdExecAssignTree
{
    ztqCommandType    mCommandType;
    ztqHostVariable * mHostVariable;
    stlInt32          mSqlStartPos;
} ztqCmdExecAssignTree;

typedef struct ztqCmdPrintTree
{
    ztqCommandType mCommandType;
    stlChar        mName[STL_MAX_SQL_IDENTIFIER_LENGTH];
} ztqCmdPrintTree;
    
typedef struct ztqCmdDeclareVarTree
{
    ztqCommandType   mCommandType;
    stlChar          mName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    ztqDataType    * mDataType;
} ztqCmdDeclareVarTree;
    
typedef struct ztqCmdSetBooleanTree
{
    ztqCommandType mCommandType;
    stlBool        mOn;
} ztqCmdSetBooleanTree;

typedef ztqCmdSetBooleanTree ztqCmdSetCallstackTree;
typedef ztqCmdSetBooleanTree ztqCmdSetVerticalTree;
typedef ztqCmdSetBooleanTree ztqCmdSetAutocommitTree;
typedef ztqCmdSetBooleanTree ztqCmdSetResultTree;
typedef ztqCmdSetBooleanTree ztqCmdSetTimingTree;
typedef ztqCmdSetBooleanTree ztqCmdSetVerboseTree;
typedef ztqCmdSetBooleanTree ztqCmdSetColorTree;
typedef ztqCmdSetBooleanTree ztqCmdSetErrorTree;

typedef struct ztqCmdSetLinesizeTree
{
    ztqCommandType mCommandType;
    stlInt64       mLineSize;
} ztqCmdSetLinesizeTree;

typedef struct ztqCmdSetColsizeTree
{
    ztqCommandType mCommandType;
    stlInt64       mColSize;
} ztqCmdSetColsizeTree;

typedef struct ztqCmdSetNumsizeTree
{
    ztqCommandType mCommandType;
    stlInt64       mNumSize;
} ztqCmdSetNumsizeTree;

typedef struct ztqCmdSetDdlsizeTree
{
    ztqCommandType mCommandType;
    stlInt64       mDdlSize;
} ztqCmdSetDdlsizeTree;

typedef struct ztqCmdSetPagesizeTree
{
    ztqCommandType mCommandType;
    stlInt64       mPageSize;
} ztqCmdSetPagesizeTree;

typedef struct ztqCmdSetHistoryTree
{
    ztqCommandType mCommandType;
    stlInt64       mHistorySize;
} ztqCmdSetHistoryTree;

typedef struct ztqCmdImportTree
{
    ztqCommandType mCommandType;
    stlChar        mImportFile[STL_MAX_FILE_PATH_LENGTH];
} ztqCmdImportTree;

typedef struct ztqCmdExecHistoryTree
{
    ztqCommandType mCommandType;
    stlInt64       mHistoryNum;
} ztqCmdExecHistoryTree;

typedef struct ztqCmdDescTree
{
    ztqCommandType   mCommandType;
    ztqIdentifier  * mIdentifierList;
} ztqCmdDescTree;

typedef ztqCmdDescTree ztqCmdIdescTree;

typedef struct ztqCmdExplainPlanTree
{
    ztqCommandType   mCommandType;
    stlUInt16        mExplainPlanType;
    stlInt32         mSqlStartPos;
} ztqCmdExplainPlanTree;

/**
 * @brief \\ALLOC stmt
 */
typedef struct ztqCmdAllocStmtTree
{
    ztqCommandType   mCommandType;
    stlChar          mStmtName[STL_MAX_SQL_IDENTIFIER_LENGTH];
} ztqCmdAllocStmtTree;


/**
 * @brief \\FREE stmt
 */
typedef struct ztqCmdFreeStmtTree
{
    ztqCommandType   mCommandType;
    stlChar          mStmtName[STL_MAX_SQL_IDENTIFIER_LENGTH];
} ztqCmdFreeStmtTree;


/**
 * @brief \\USE stmt EXEC SQL
 */
typedef struct ztqCmdUseStmtExecSqlTree
{
    ztqCommandType   mCommandType;
    stlChar          mStmtName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlInt32         mSqlStartPos;
} ztqCmdUseStmtExecSqlTree;

/**
 * @brief \\USE stmt PREPARE SQL
 */
typedef struct ztqCmdUseStmtPrepareSqlTree
{
    ztqCommandType   mCommandType;
    stlChar          mStmtName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlInt32         mSqlStartPos;
} ztqCmdUseStmtPrepareSqlTree;

/**
 * @brief \\USE stmt EXEC
 */
typedef struct ztqCmdUseStmtExecPreparedTree
{
    ztqCommandType   mCommandType;
    stlChar          mStmtName[STL_MAX_SQL_IDENTIFIER_LENGTH];
} ztqCmdUseStmtExecPreparedTree;

/**
 * @brief \\STARTUP startup_phase
 */
typedef struct ztqCmdStartupTree
{
    ztqCommandType   mCommandType;
    ztqStartupPhase  mStartupPhase;
} ztqCmdStartupTree;

/**
 * @brief \\SHUTDOWN
 */
typedef struct ztqCmdShutdownTree
{
    ztqCommandType   mCommandType;
    ztqShutdownMode  mShutdownMode;
} ztqCmdShutdownTree;

/**
 * @brief \\CONNECT [userid password] [AS SYSDBA]
 */
typedef struct ztqCmdConnectTree
{
    ztqCommandType   mCommandType;
    stlChar          mUserId[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar          mPassword[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar          mNewPassword[STL_MAX_SQL_IDENTIFIER_LENGTH];
    ztqConnectAsUser mAsUser;
} ztqCmdConnectTree;


/**
 * @brief \\ddl_table, \\ddl_user, ...
 */
typedef struct ztqCmdDDLTree
{
    ztqCommandType   mCommandType;
    ztqIdentifier  * mIdentifierList;
    ztqPrintDDLType  mDDLType;
} ztqCmdDDLTree;

#define ZTQ_MAX_XAINFO_SIZE    ( 256 )

/**
 * @brief \\XA OPEN
 */
typedef struct ztqCmdXaOpenTree
{
    ztqCommandType   mCommandType;
    stlChar          mXaInfo[ZTQ_MAX_XAINFO_SIZE];
    stlInt64         mFlags;
} ztqCmdXaOpenTree;

/**
 * @brief \\XA CLOSE
 */
typedef struct ztqCmdXaCloseTree
{
    ztqCommandType   mCommandType;
    stlChar          mXaInfo[ZTQ_MAX_XAINFO_SIZE];
    stlInt64         mFlags;
} ztqCmdXaCloseTree;

/**
 * @brief \\XA START
 */
typedef struct ztqCmdXaStartTree
{
    ztqCommandType   mCommandType;
    stlInt64         mFlags;
    XID              mXid;
} ztqCmdXaStartTree;

/**
 * @brief \\XA END
 */
typedef struct ztqCmdXaEndTree
{
    ztqCommandType   mCommandType;
    stlInt64         mFlags;
    XID              mXid;
} ztqCmdXaEndTree;

/**
 * @brief \\XA COMMIT
 */
typedef struct ztqCmdXaCommitTree
{
    ztqCommandType   mCommandType;
    stlInt64         mFlags;
    XID              mXid;
} ztqCmdXaCommitTree;

/**
 * @brief \\XA ROLLBACK
 */
typedef struct ztqCmdXaRollbackTree
{
    ztqCommandType   mCommandType;
    stlInt64         mFlags;
    XID              mXid;
} ztqCmdXaRollbackTree;

/**
 * @brief \\XA PREPARE
 */
typedef struct ztqCmdXaPrepareTree
{
    ztqCommandType   mCommandType;
    stlInt64         mFlags;
    XID              mXid;
} ztqCmdXaPrepareTree;

/**
 * @brief \\XA FORGET
 */
typedef struct ztqCmdXaForgetTree
{
    ztqCommandType   mCommandType;
    stlInt64         mFlags;
    XID              mXid;
} ztqCmdXaForgetTree;

/**
 * @brief \\XA RECOVER
 */
typedef struct ztqCmdXaRecoverTree
{
    ztqCommandType   mCommandType;
    XID            * mXids;
    stlInt64         mFlags;
    stlInt64         mCount;
} ztqCmdXaRecoverTree;

typedef struct ztqXaFlag
{
    stlInt64  mFlag;
    void    * mNext;
} ztqXaFlag;

/**
 * @brief \\EDIT
 */
typedef struct ztqCmdEditTree
{
    ztqCommandType   mCommandType;
    stlChar         *mSqlFile;
    stlInt64         mHistoryNum;
} ztqCmdEditTree;

/**
 * @brief \\SPOOL
 */
typedef struct ztqCmdSpoolTree
{
    ztqCommandType   mCommandType;
    stlChar         *mSpoolFile;
    ztqSpoolFlag     mSpoolFlag;
} ztqCmdSpoolTree;

#define ZTQ_EMPTY_PARSE_TREE ((void*)1)

/** @} */


/**
 * @addtogroup ztqQueryHistory
 * @{
 */

typedef struct ztqHistoryQueue
{
    stlInt64  mId;
    stlInt64  mBufSize;
    stlChar  *mCommand;
} ztqHistoryQueue;

/** @} */


/**
 * @addtogroup ztqDisplay
 * @{
 */

#define ZTQ_DEFAULT_DSN                 "GOLDILOCKS"

#define ZTQ_PROMPT_TAIL                 ">"
#define ZTQ_MAX_PROMPT_SIZE             (32)
#define ZTQ_DEFAULT_PROMPT              "gSQL"ZTQ_PROMPT_TAIL
#define ZTQ_NULL_STRING                 "null"

#define ZTQ_MAX_FORMAT                  (64)
#define ZTQ_COLUMN_FORMAT_CLASS_NUMERIC "%*s"
#define ZTQ_COLUMN_FORMAT_CLASS_STRING  "%-*s"
#define ZTQ_MAX_ANSWER_MESSAGE          (256)
#define ZTQ_TAB_SIZE                    (4)

#define ZTQ_PRINT_PROMPT( aDisplayPrompt )                          \
    {                                                               \
        if( aDisplayPrompt == STL_TRUE )                            \
        {                                                           \
            if( gZtqLineNo == 1 )                                   \
            {                                                       \
                ztqEchoPrintf( "%s ", gZtqPrompt );                 \
            }                                                       \
            else                                                    \
            {                                                       \
                ztqEchoPrintf( "%*d ", gZtqPromptSize, gZtqLineNo );    \
            }                                                       \
        }                                                           \
    }

/** @} */

/**
 * @addtogroup ztqCommand
 * @{
 */

#define ZTQ_EDITOR_ENV                 "EDITOR"
#ifdef WIN32
#define ZTQ_DEFAULT_EDITOR             "notepad.exe"
#else
#define ZTQ_DEFAULT_EDITOR             "vi"
#endif
#define ZTQ_EDIT_BUF_FILE              "gsqledt.buf"

/** @} */

/**
 * @defgroup ztqError Error
 * @{
 */

/**
 * @brief syntax error
 */
#define ZTQ_ERRCODE_SYNTAX                         STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 0 )
#define ZTQ_ERRCODE_NOT_SUPPORTED                  STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 1 )
#define ZTQ_ERRCODE_NOT_DECLARED_HOST_VARIABLE     STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 2 )
#define ZTQ_ERRCODE_INVALID_TABLE_NAME             STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 3 )
#define ZTQ_ERRCODE_INVALID_INDEX_NAME             STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 4 )
#define ZTQ_ERRCODE_SERVICE_NOT_AVAILABLE          STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 5 )
#define ZTQ_ERRCODE_NOT_PREPARED                   STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 6 )
#define ZTQ_ERRCODE_NAMED_STMT_DUPLICATE           STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 7 )
#define ZTQ_ERRCODE_NAMED_STMT_NOT_EXIST           STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 8 )
#define ZTQ_ERRCODE_NAMED_STMT_NOT_PREPARED        STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 9 )
#define ZTQ_ERRCODE_INSUFFICIENT_PRIVILEGES        STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 10 )
#define ZTQ_ERRCODE_NOT_SUPPORT_FUNCTION           STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 11)
#define ZTQ_ERRCODE_INVALID_OBJECT_DESCRIBE        STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 12)
#define ZTQ_ERRCODE_MISMATCH_DRIVER                STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 13)

#define ZTQ_ERRCODE_XA_RBROLLBACK                  STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 14)
#define ZTQ_ERRCODE_XA_COMMFAIL                    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 15)
#define ZTQ_ERRCODE_XA_RBDEADLOCK                  STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 16)
#define ZTQ_ERRCODE_XA_RBINTEGRITY                 STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 17)
#define ZTQ_ERRCODE_XA_RBOTHER                     STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 18)
#define ZTQ_ERRCODE_XA_RBPROTO                     STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 19)
#define ZTQ_ERRCODE_XA_RBTIMEOUT                   STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 20)
#define ZTQ_ERRCODE_XA_RBTRANSIENT                 STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 21)
#define ZTQ_ERRCODE_XA_RBEND                       STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 22)
#define ZTQ_ERRCODE_XA_NOMIGRATE                   STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 23)
#define ZTQ_ERRCODE_XA_HEURHAZ                     STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 24)
#define ZTQ_ERRCODE_XA_HEURCOM                     STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 25)
#define ZTQ_ERRCODE_XA_HEURRB                      STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 26)
#define ZTQ_ERRCODE_XA_HEURMIX                     STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 27)
#define ZTQ_ERRCODE_XA_RETRY                       STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 28)
#define ZTQ_ERRCODE_XA_RDONLY                      STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 29)
#define ZTQ_ERRCODE_XA_ASYNC                       STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 30)
#define ZTQ_ERRCODE_XA_RMERR                       STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 31)
#define ZTQ_ERRCODE_XA_NOTA                        STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 32)
#define ZTQ_ERRCODE_XA_INVAL                       STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 33)
#define ZTQ_ERRCODE_XA_PROTO                       STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 34)
#define ZTQ_ERRCODE_XA_RMFAIL                      STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 35)
#define ZTQ_ERRCODE_XA_DUPID                       STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 36)
#define ZTQ_ERRCODE_XA_OUTSIDE                     STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 37)
#define ZTQ_ERRCODE_XA_UNKNOWN                     STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 38)
#define ZTQ_ERRCODE_XA_RBROLLBACK_ROLLBACKONLY     STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 39)
#define ZTQ_ERRCODE_XA_RMERR_OPEN                  STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 40)
#define ZTQ_ERRCODE_XA_RMERR_CLOSE                 STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 41)
#define ZTQ_ERRCODE_NULL_SQL_STRING                STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 42)
#define ZTQ_ERRCODE_INVALID_HOST_TYPE              STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 43)
#define ZTQ_ERRCODE_CONNECTION_DOES_NOT_EXIST      STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 44)
#define ZTQ_ERRCODE_INVALID_NUMSIZE_VALUE          STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 45)
#define ZTQ_ERRCODE_INVALID_PAGESIZE_VALUE         STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 46)
#define ZTQ_ERRCODE_INVALID_LINESIZE_VALUE         STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 47)
#define ZTQ_ERRCODE_INVALID_COLSIZE_VALUE          STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 48)
#define ZTQ_ERRCODE_INVALID_HISTORY_VALUE          STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 49)
#define ZTQ_ERRCODE_INVALID_DDLSIZE_VALUE          STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 50)
#define ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER      STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 51)
#define ZTQ_ERRCODE_NOT_ENOUGH_DDLSIZE             STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 52)
#define ZTQ_ERRCODE_CONFIRMATION_PASSWORD_MISMATCH STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSQL, 53)

#define ZTQ_MAX_ERROR     54

/** @} */

/**
 * @addtogroup ztqExecute
 * @{
 */

#define ZTQ_TIME_NOW( aTime )            \
{                                        \
    if( gZtqTiming == STL_TRUE )         \
    {                                    \
        aTime = stlNow();                \
    }                                    \
}

#define ZTQ_GROUP_STRING  (1)
#define ZTQ_GROUP_NUMERIC (2)

typedef struct ztqIntervalColInfo
{
    SQLINTEGER  mLeadingPrecision;
    SQLSMALLINT mSecondPrecision;
    
} ztqIntervalColInfo;

typedef struct ztqTargetColumn
{
    stlChar            mName[ZTQ_MAX_COLUMN_NAME];
    stlChar            mColumnFormat[ZTQ_MAX_FORMAT];
    SQLULEN            mDataSize;
    stlChar          * mDataValue;
    SQLSMALLINT        mDataType;
    SQLLEN             mIndicator;
    stlInt16           mDataTypeGroup;
    dtlDataType        mDtlDataType;    
    ztqIntervalColInfo mIntervalColInfo;
} ztqTargetColumn;

typedef struct ztqColBuffer
{
    stlChar   * mColName;
    stlInt32    mBufSize;
    stlInt32    mMaxLength;
    stlInt32    mMaxLineCount;
    stlInt32    mActualLineCount;
    stlChar  ** mDataBuf;
} ztqColBuffer;

typedef struct ztqRowBuffer
{
    stlInt32       mColCnt;
    stlInt32       mLineCnt;
    ztqColBuffer * mColBuf;
} ztqRowBuffer;

typedef struct ztqSqlStmt
{
    ztqCommandType     mCommandType;
    ztqStatementType   mStatementType;
    ztqObjectType      mObjectType;
    stlChar            mObjectName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLHSTMT           mOdbcStmt;
    ztqHostVariable  * mHostVarList;
    stlBool            mExplainPlan;
} ztqSqlStmt;

/** @} */


/**
 * @addtogroup ztqNamedStmt
 * @{
 */

/**
 * @brief Named Statement
 * @remarks
 */
typedef struct ztqNamedStmt
{
    stlChar                mStmtName[STL_MAX_SQL_IDENTIFIER_LENGTH]; 
    stlBool                mIsPrepared;       /**< Prepare Execution */
    ztqSqlStmt             mZtqSqlStmt;       /**< GSQL Stmt */
    struct ztqNamedStmt  * mNext;             /**< Next */
} ztqNamedStmt;


extern ztqNamedStmt * gZtqNamedStmt;

/** @} */


/**
 * @addtogroup ztqCommand
 * @{
 */

typedef stlStatus (*ztqCmdFunc)( ztqParseTree  * aParseTree,
                                 stlChar       * aCmdStr,
                                 stlAllocator  * aAllocator,
                                 stlErrorStack * aErrorStack );

typedef stlStatus (*ztqSpoolCmdFunc)( stlChar       *aSpoolFilePath,
                                      ztqSpoolFlag   aSpoolFlag,
                                      stlAllocator  *aAllocator,
                                      stlErrorStack *aErrorStack );

/** @} */

/**
 * @addtogroup ztqOdbcBridge
 * @{
 */

typedef SQLRETURN (SQL_API * ztqOdbcBridgeFunc)();

typedef struct ztqDriverFuncMap
{
    ztqOdbcBridgeFunc   mFunction;
    ztqOdbcBridgeFunc   mGlieseFunction;
    stlChar           * mFunctionName;
    stlChar           * mFunctionNameW;
} ztqDriverFuncMap;

typedef enum
{
    ZTQ_FUNC_ALLOCHANDLE,
    ZTQ_FUNC_ALLOCENV,
    ZTQ_FUNC_ALLOCCONNECT,
    ZTQ_FUNC_ALLOCSTMT,
    ZTQ_FUNC_CONNECT,
    ZTQ_FUNC_DRIVERCONNECT,
    ZTQ_FUNC_SETENVATTR,
    ZTQ_FUNC_PREPARE,
    ZTQ_FUNC_EXECUTE,
    ZTQ_FUNC_FETCH,
    ZTQ_FUNC_NUMRESULTCOLS,
    ZTQ_FUNC_BINDCOL,
    ZTQ_FUNC_DESCRIBECOL,
    ZTQ_FUNC_GETDIAGREC,
    ZTQ_FUNC_ERROR,
    ZTQ_FUNC_EXECDIRECT,
    ZTQ_FUNC_ENDTRAN,
    ZTQ_FUNC_TRANSACT,
    ZTQ_FUNC_ROWCOUNT,
    ZTQ_FUNC_FREEHANDLE,
    ZTQ_FUNC_FREEENV,
    ZTQ_FUNC_FREECONNECT,
    ZTQ_FUNC_FREESTMT,
    ZTQ_FUNC_BINDPARAMETER,
    ZTQ_FUNC_SETCONNECTATTR,
    ZTQ_FUNC_GETCONNECTATTR,
    ZTQ_FUNC_SETCONNECTOPTION,
    ZTQ_FUNC_DISCONNECT,
    ZTQ_FUNC_CLOSECURSOR,
    ZTQ_FUNC_SETSTMTATTR,
    ZTQ_FUNC_GETSTMTATTR,
    ZTQ_FUNC_GETDESCFIELD,
    ZTQ_FUNC_SETDESCFIELD,
    ZTQ_MAX_FUNC
} ztqDriverFuncType;

/** @} */

/** @} */

#endif /* _ZTQDEF_H_ */
