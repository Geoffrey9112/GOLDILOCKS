/*******************************************************************************
 * ztpDef.h
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


#ifndef _ZTPDEF_H_
#define _ZTPDEF_H_ 1

#include <dtl.h>
#include <zlplDef.h>

/**
 * @file ztpDef.h
 * @brief Gliese SQL Definition
 */

/**
 * @defgroup ztp Gliese SQL
 * @{
 */

/**
 * @defgroup ztpGeneral General
 * @{
 */

#define ZTP_OPTION_NA                     (0)
#define ZTP_OPTION_UNSAFE_NULL            (-1)
#define ZTP_OPTION_NO_LINEINFO            (-2)

#define ZTP_IS_DIR_SEP(aCh)               ((aCh) == STL_PATH_SEPARATOR[0])

#define ZTP_REGION_INIT_SIZE              (1024*1024)  /* 1M */
#define ZTP_REGION_NEXT_SIZE              (8192)       /* 8K */
#define ZTP_MAX_TABLE_NAME                (128)
#define ZTP_MAX_COLUMN_NAME               (128)

#define ZTP_LINE_BUF_SIZE                 (8192)

#define ZTP_MACROHASH_BUCKET_COUNT        (127)

//#define ZTP_OUTPUT_HEADER_FILE            "ztpOutputHeader.txt"

#define ZTP_INIT_TEMP_BUFFER_SIZE         (1024*1024)  /* 1M */
#define ZTP_MACRO_REPLACE_BUFFER_SIZE     (1024*1024)  /* 1M */

typedef struct ztpTempBuffer
{
    stlChar         *mBuffer;
    stlInt32         mOffset;
    stlInt32         mBufferLength;
} ztpTempBuffer;

#define ZTP_INIT_TEMP_BUFFER( aTempBuffer )                       \
{                                                                 \
    (aTempBuffer)->mBuffer[0] = '\0';                             \
    (aTempBuffer)->mOffset = 0;                                   \
}

typedef enum
{
    ZTP_PARSING_LEVEL_MINIMUM,
    ZTP_PARSING_LEVEL_MEDIUM,
    ZTP_PARSING_LEVEL_MAXIMUM
} ztpParsingLevel;

/** @} */

/**
 * @addtogroup ztpHostVariable
 * @{
 */

#define ZTP_MAX_HOST_VALUE_SIZE  (4096)

typedef enum
{
    ZTP_HOSTVAR_DESC_NONE,
    ZTP_HOSTVAR_DESC_ID,
    ZTP_HOSTVAR_DESC_PERIOD,
    ZTP_HOSTVAR_DESC_PTR,
    ZTP_HOSTVAR_DESC_ARRAY,
    ZTP_HOSTVAR_DESC_MAX,
} ztpHostVarDesc;

typedef struct ztpHostVariable ztpHostVariable;
struct ztpHostVariable
{
    ztpHostVarDesc          mHostVarDesc;
    stlChar                *mValue;
    stlChar                *mHostVarString;
    struct ztpHostVariable *mNext;
    struct ztpHostVariable *mIndicator;
};

/** @} */

/**
 * @defgroup ztpParser Parser
 * @{
 */

#define ZTP_KEY_WORD_MAX_LENGTH      ( 64 )
#define ZTP_KEY_WORD_COUNT( list )   ( STL_SIZEOF( list ) / STL_SIZEOF( list[0] ) )

/**
 * @brief keyword category
 */

typedef enum ztpKeywordCategory
{
    ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE = 0,
    ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE
} ztpKeywordCategory;

/**
 * @brief keyword 정보
 */
typedef struct ztpKeyWord
{
    const stlChar       *mName;
    stlInt16             mTokenCode;
    ztpKeywordCategory   mCategory;
} ztpKeyWord;

/**
 * @brief keyword list
 */
extern const ztpKeyWord   gZtpKeyWordList[];


/**
 * @brief keyword count
 */
extern const stlInt16     ztpKeyWordCount;

/*
 * ztpParseTreeType 내용이 변경되면
 * gCvtSQLFunc 정의 내용도 맞춰줘야 함
 */
typedef enum
{
    ZTP_PARSETREE_TYPE_SQL_TRANSACTION,
    ZTP_PARSETREE_TYPE_SQL_BYPASS,
    ZTP_PARSETREE_TYPE_SET_AUTOCOMMIT,
    ZTP_PARSETREE_TYPE_DECLARE_CURSOR,
    ZTP_PARSETREE_TYPE_OPEN_CURSOR,
    ZTP_PARSETREE_TYPE_FETCH_CURSOR,
    ZTP_PARSETREE_TYPE_CLOSE_CURSOR,
    ZTP_PARSETREE_TYPE_POSITIONED_CURSOR_DML,
    ZTP_PARSETREE_TYPE_EXEC_SQL_INCLUDE,
    ZTP_PARSETREE_TYPE_SQL_EXCEPTION,
    ZTP_PARSETREE_TYPE_SQL_CONNECT,
    ZTP_PARSETREE_TYPE_SQL_DISCONNECT,
    /*
     * Context
     */
    ZTP_PARSETREE_TYPE_CONTEXT_ALLOCATE,
    ZTP_PARSETREE_TYPE_CONTEXT_FREE,
    ZTP_PARSETREE_TYPE_CONTEXT_USE,
    ZTP_PARSETREE_TYPE_CONTEXT_USE_DEFAULT,
    /*
     * Dynamic SQL
     */
    ZTP_PARSETREE_TYPE_DYNAMIC_SQL_EXECUTE_IMMEDIATE,
    ZTP_PARSETREE_TYPE_DYNAMIC_SQL_PREPARE,
    ZTP_PARSETREE_TYPE_DYNAMIC_SQL_EXECUTE,
    /*
     * For Compatibility
     */
    ZTP_PARSETREE_TYPE_IGNORE,

    ZTP_PARSETREE_TYPE_MAX
} ztpParseTreeType;

/*
typedef enum
{
    ZTP_STMT_SELECT,
    ZTP_STMT_SELECT_INTO,
    ZTP_STMT_QUERY_EXPR,
    ZTP_STMT_QUERY_EXPR_INTO,
    ZTP_STMT_INSERT,
    ZTP_STMT_UPDATE,
    ZTP_STMT_DELETE,
    ZTP_STMT_CREATE,
    ZTP_STMT_DROP,
    ZTP_STMT_ALTER,
    ZTP_STMT_TRUNCATE,
    ZTP_STMT_COMMIT,
    ZTP_STMT_ROLLBACK,
    ZTP_STMT_RELEASE,
    ZTP_STMT_GRANT,
    ZTP_STMT_REVOKE,
    ZTP_STMT_SET,
    ZTP_STMT_DECLARE_CURSOR,
    ZTP_STMT_OPEN_CURSOR,
    ZTP_STMT_CLOSE_CURSOR,
    ZTP_STMT_FETCH_CURSOR,
    ZTP_STMT_COPY,
    ZTP_STMT_LOCK,
    ZTP_STMT_COMMENT,
    ZTP_STMT_CALL,
    ZTP_STMT_MAX
} ztpStatementType;
*/

typedef enum
{
    ZTP_OBJECT_DATABASE,
    ZTP_OBJECT_INDEX,
    ZTP_OBJECT_SEQUENCE,
    ZTP_OBJECT_SESSION,
    ZTP_OBJECT_SYSTEM,
    ZTP_OBJECT_SCHEMA,
    ZTP_OBJECT_PROCEDURE,
    ZTP_OBJECT_FUNCTION,
    ZTP_OBJECT_TABLESPACE,
    ZTP_OBJECT_TABLE,
    ZTP_OBJECT_USER,
    ZTP_OBJECT_VIEW,
    ZTP_OBJECT_ROLE,
    ZTP_OBJECT_CONSTRAINTS,
    ZTP_OBJECT_SAVEPOINT,
    ZTP_OBJECT_TRANSACTION,
    ZTP_OBJECT_SYNONYM,
    ZTP_OBJECT_PROFILE,
    ZTP_OBJECT_UNKNOWN,
    ZTP_OBJECT_MAX
} ztpObjectType;

typedef enum
{
    ZTP_EXCEPTION_COND_SQLSTATE,
    ZTP_EXCEPTION_COND_NOT_FOUND,
    ZTP_EXCEPTION_COND_SQLERROR,
    ZTP_EXCEPTION_COND_SQLWARNING,
    ZTP_EXCEPTION_COND_MAX
} ztpExceptionConditionType;

typedef enum
{
    ZTP_EXCEPTION_ACT_GOTO,
    ZTP_EXCEPTION_ACT_CONTINUE,
    ZTP_EXCEPTION_ACT_STOP,
    ZTP_EXCEPTION_ACT_DO,
    ZTP_EXCEPTION_ACT_MAX
} ztpExceptionActionType;

typedef enum
{
    ZTP_NAMETYPE_DQ_LITERAL,
    ZTP_NAMETYPE_LITERAL,
    ZTP_NAMETYPE_HOSTVAR
} ztpNameType;

typedef enum
{
    ZTP_CONTEXT_ALLOCATE,
    ZTP_CONTEXT_FREE,
    ZTP_CONTEXT_USE,
    ZTP_CONTEXT_USE_DEFAULT,
    ZTP_CONTEXT_MAX
} ztpContextActionType;

typedef struct ztpCIdentifierList ztpCIdentifierList;
typedef struct ztpCParameterList ztpCParameterList;
typedef struct ztpCDeclarator ztpCDeclarator;
typedef struct ztpCDirectDeclarator ztpCDirectDeclarator;
typedef struct ztpCDeclaratorList ztpCDeclaratorList;
typedef struct ztpCDeclarationList ztpCDeclarationList;
//typedef struct ztpDeclSpec ztpDeclSpec;
typedef struct ztpCVariable ztpCVariable;

typedef struct ztpParseTree
{
    ztpParseTreeType mParseTreeType;
} ztpParseTree;

typedef struct ztpCIdentifier
{
    stlInt32   mLength;
    stlChar   *mName;
} ztpCIdentifier;

struct ztpCIdentifierList
{
    ztpCIdentifier     *mCIdentifier;
    ztpCIdentifierList *mNext;
};

typedef struct ztpCParameterDeclaration
{
    stlInt64            mDeclarationSpecifiers;
    zlplCDataType       mCDataType;
    ztpCDeclarator     *mCDeclarator;
} ztpCParameterDeclaration;

struct ztpCParameterList
{
    ztpCParameterDeclaration  *mCParameterDeclaration;
    ztpCParameterList         *mNext;
};

struct ztpCDirectDeclarator
{
    ztpCIdentifier              *mCIdentifier;
    ztpCDeclarator              *mCDeclarator;
    ztpCDirectDeclarator        *mCDirectDeclarator;
    stlChar                     *mArrayValue;
    ztpCIdentifierList          *mCIdentifierList;
    ztpCParameterList           *mCParameterList;
};

struct ztpCDeclarator
{
    ztpCDirectDeclarator  *mCDirectDeclarator;
    stlInt32               mAddrDepth;
};

struct ztpCDeclaratorList
{
    ztpCDeclarator       *mCDeclarator;
    ztpCDeclaratorList   *mNext;
};

typedef struct ztpCDeclaration
{
    stlInt32                mIndent;
    stlInt32                mStartPos;
    stlInt32                mEndPos;
    stlInt32                mLineNo;
    ztpCVariable           *mCVariable;
    ztpCDeclaratorList     *mDeclaratorList;
    stlChar                *mPrecision;
    stlChar                *mScale;
    zlplCDataType           mDataType;
} ztpCDeclaration;

struct ztpCDeclarationList
{
    ztpCDeclaration      *mCDeclaration;
    ztpCDeclarationList  *mNext;
};

typedef struct ztpCStructSpecifier
{
    ztpCIdentifier       *mTagName;
    ztpCDeclarationList  *mCDeclarationList;
} ztpCStructSpecifier;

struct ztpCVariable
{
    stlChar           *mName;
    stlChar           *mParentStructName;
    stlInt32           mNamePos;
    stlBool            mIsArray;
    stlBool            mIsStructMember;
    stlBool            mIsAvailHostVar;
    zlplParamIOType    mParamIOType;
    zlplCDataType      mDataType;
    stlInt32           mAddrDepth;
    stlChar           *mArrayValue;
    stlChar           *mCharLength;
    stlChar           *mPrecision;
    stlChar           *mScale;
    ztpCVariable      *mIndicator;
    ztpCVariable      *mNext;
    ztpCVariable      *mStructMember;
};

#define ZTP_INIT_C_VARIABLE( aCVariable )                       \
    {                                                           \
        (aCVariable)->mName = NULL;                             \
        (aCVariable)->mParentStructName = NULL;                 \
        (aCVariable)->mNamePos = 0;                             \
        (aCVariable)->mIsArray = STL_FALSE;                     \
        (aCVariable)->mIsStructMember = STL_FALSE;              \
        (aCVariable)->mIsAvailHostVar = STL_FALSE;              \
        (aCVariable)->mParamIOType = ZLPL_PARAM_IO_TYPE_MAX;    \
        (aCVariable)->mDataType = SQL_UNKNOWN_TYPE;             \
        (aCVariable)->mAddrDepth = 0;                           \
        (aCVariable)->mArrayValue = NULL;                       \
        (aCVariable)->mCharLength = NULL;                       \
        (aCVariable)->mPrecision = NULL;                        \
        (aCVariable)->mScale = NULL;                            \
        (aCVariable)->mIndicator = NULL;                        \
        (aCVariable)->mNext = NULL;                             \
        (aCVariable)->mStructMember = NULL;                     \
    }

typedef struct ztpTypeDefSymbol
{
    stlChar                 *mName;
    stlChar                  mSymbol[1];
    struct ztpTypeDefSymbol *mNext;
} ztpTypeDefSymbol;

#define ZTP_INIT_TYPEDEF_SYMBOL( aTypeDefSymbol )   \
    {                                               \
        (aTypeDefSymbol)->mName = NULL;             \
        (aTypeDefSymbol)->mSymbol[0] = '\0';        \
        (aTypeDefSymbol)->mNext = NULL;             \
    }

typedef struct ztpIncludePath
{
    struct ztpIncludePath *mNext;
    stlChar                mIncludePath[STL_MAX_FILE_PATH_LENGTH + 1];
} ztpIncludePath;

typedef struct ztpConvertContext
{
    stlFile         *mInFile;
    stlFile         *mOutFile;
    stlChar         *mInFileName;
    stlChar         *mOutFileName;
    stlAllocator    *mAllocator;
    stlErrorStack   *mErrorStack;
    stlChar         *mBuffer;
    stlSize          mLength;
    stlBool          mNeedPrint;
    ztpParseTree    *mParseTree;
} ztpConvertContext;

typedef struct ztpTransactionTree
{
    ztpParseTreeType   mParseTreeType;
    zlplStatementType  mStatementType;
    stlBool            mIsCommit;
    stlBool            mIsRelease;
} ztpTransactionTree;

typedef struct ztpBypassTree
{
    ztpParseTreeType   mParseTreeType;
    zlplStatementType  mStatementType;
    ztpObjectType      mObjectType;
    ztpCVariable     * mHostVarList;
    stlChar          * mSqlString;
} ztpBypassTree;

typedef struct ztpSqlParseParam
{
    stlAllocator    * mAllocator;
    ztpCVariable    * mHostVarList;
} ztpSqlParseParam;

typedef struct ztpDataType
{
    stlInt32    mDataType;
    stlInt32    mPrecision;
    stlInt32    mScale;
} ztpDataType;

typedef struct ztpExecSqlInclude
{
    ztpParseTreeType   mParseTreeType;
    stlChar            mFileName[STL_MAX_FILE_NAME_LENGTH];
} ztpExecSqlInclude;

typedef struct ztpExceptionCondition
{
    ztpExceptionConditionType  mType;
    stlChar                    mValue[6];
} ztpExceptionCondition;

typedef struct ztpExceptionAction
{
    ztpExceptionActionType     mType;
    stlChar                   *mValue;
} ztpExceptionAction;

typedef struct ztpExecSqlException
{
    ztpParseTreeType        mParseTreeType;
    ztpExceptionCondition  *mCondition;
    ztpExceptionAction     *mAction;
} ztpExecSqlException;

typedef struct ztpNameTag
{
    ztpNameType  mType;
    stlChar      mName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlInt32     mNamePos;
} ztpNameTag;

typedef struct ztpIdentifier
{
    stlChar      mName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlInt32     mNamePos;
} ztpIdentifier;

typedef struct ztpTokenStream
{
    stlInt32     mStartPos;
    stlInt32     mEndPos;
} ztpTokenStream;

#define ZTP_INIT_TOKEN_STREAM(aTokenStream)     \
    {                                           \
        aTokenStream.mStartPos = STL_INT32_MAX; \
        aTokenStream.mEndPos = 0;               \
    }

typedef struct ztpDeclCursor
{
    ztpParseTreeType            mParseTreeType;
    zlplStatementType           mType;

    stlChar                   * mCursorName;
    stlInt32                    mCursorNamePos;

    stlInt32                    mCursorOriginOffset;
    stlChar                   * mCursorOrigin;   /**< cursor query or dynamic statement name */
    zlplCursorOriginType        mOriginType;

    /*
     * Cursor property
     */
    zlplCursorStandardType      mStandardType;

    zlplCursorSensitivity       mSensitivity;
    zlplCursorScrollability     mScrollability;
    zlplCursorHoldability       mHoldability;
    zlplCursorUpdatability      mUpdatability;
    zlplCursorReturnability     mReturnability;

    ztpCVariable               *mHostVarList;
    struct ztpDeclCursor       *mNext;
} ztpDeclCursor;

#define ZTP_INIT_DECL_CURSOR( aDeclCursor )                             \
    {                                                                   \
        (aDeclCursor)->mParseTreeType = ZTP_PARSETREE_TYPE_DECLARE_CURSOR; \
        (aDeclCursor)->mType = ZLPL_STMT_DECLARE_CURSOR;                \
        (aDeclCursor)->mCursorName = NULL;                              \
        (aDeclCursor)->mCursorQuery = NULL;                             \
        (aDeclCursor)->mOriginType = ZLPL_CURSOR_ORIGIN_NA;             \
        (aDeclCursor)->mStandardType = ZLPL_CURSOR_STANDARD_NA;         \
        (aDeclCursor)->mSensitivity = ZLPL_CURSOR_SENSITIVITY_NA;       \
        (aDeclCursor)->mScrollability = ZLPL_CURSOR_SCROLLABILITY_NA;   \
        (aDeclCursor)->mHoldability = ZLPL_CURSOR_HOLDABILITY_NA;       \
        (aDeclCursor)->mUpdatability = ZLPL_CURSOR_UPDATABILITY_NA;     \
        (aDeclCursor)->mReturnability = ZLPL_CURSOR_RETURNABILITY_NA;   \
        (aDeclCursor)->mHostVarList = NULL;                             \
        (aDeclCursor)->mNext = NULL;                                    \
    }

typedef struct ztpDynamicParam
{
    stlBool           mIsDesc;
    ztpCVariable    * mHostVar;
    stlChar         * mDescName;
} ztpDynamicParam;


typedef struct ztpOpenCursor
{
    ztpParseTreeType    mParseTreeType;
    zlplStatementType   mType;

    stlChar           * mCursorName;
    stlInt32            mCursorNamePos;

    ztpDynamicParam   * mDynUsing;       /**< USING host */

} ztpOpenCursor;

typedef struct ztpFetchOrient
{
    ztpNameTag           mFetchPos;
    ztpCVariable       * mFetchOffsetHost;

    zlplFetchOrientation mFetchOrient;
} ztpFetchOrient;

typedef struct ztpFetchCursor
{
    ztpParseTreeType    mParseTreeType;
    zlplStatementType   mType;

    stlChar           * mCursorName;
    stlInt32            mCursorNamePos;

    ztpCVariable      * mHostVarList;
    ztpFetchOrient    * mFetchOrientation;
} ztpFetchCursor;

typedef struct ztpCloseCursor
{
    ztpParseTreeType     mParseTreeType;
    zlplStatementType    mType;

    stlChar            * mCursorName;
    stlInt32             mCursorNamePos;
} ztpCloseCursor;


typedef struct ztpPositionedCursorDML
{
    ztpParseTreeType     mParseTreeType;
    zlplStatementType    mType;

    stlChar            * mCursorName;
    stlInt32             mCursorNamePos;

    stlChar            * mCursorDML;
    ztpCVariable       * mHostVarList;

} ztpPositionedCursorDML;

/**
 * EXECUTE IMMEDIATE
 */
typedef struct ztpExecuteImmediate
{
    ztpParseTreeType    mParseTreeType;
    zlplStatementType   mType;

    ztpCVariable      * mHostVariable;    /**< Host Variable */
    stlChar           * mStringSQL;       /**< SQL string */
} ztpExecuteImmediate;


/**
 * PREPARE
 */
typedef struct ztpPrepare
{
    ztpParseTreeType    mParseTreeType;
    zlplStatementType   mType;

    ztpIdentifier     * mStmtName;        /**< statement name */

    ztpCVariable      * mHostVariable;    /**< Host Variable */
    stlChar           * mStringSQL;       /**< SQL string */
} ztpPrepare;


/**
 * EXECUTE
 */
typedef struct ztpExecute
{
    ztpParseTreeType    mParseTreeType;
    zlplStatementType   mType;

    ztpIdentifier     * mStmtName;        /**< statement name */

    ztpDynamicParam   * mDynUsing;       /**< USING host */
    ztpDynamicParam   * mDynInto;        /**< INTO host */
} ztpExecute;


typedef struct ztpSymTabHeader
{
    stlInt32                 mDepth;
    ztpCVariable           * mCVariableHead;
    ztpDeclCursor          * mCursorListHead;
    ztpCVariable           * mTypedefListHead;
    struct ztpSymTabHeader * mPrevDepth;
    struct ztpSymTabHeader * mNextDepth;
} ztpSymTabHeader;

typedef struct ztpConnectParam
{
    ztpParseTreeType     mParseTreeType;
    ztpNameTag          *mConnName;
    ztpNameTag          *mConnString;
    ztpNameTag          *mUserName;
    ztpNameTag          *mPassword;
} ztpConnectParam;

typedef struct ztpDisconnectParam
{
    ztpParseTreeType     mParseTreeType;
    ztpNameTag          *mConnectionObject;
    stlBool              mIsAll;
} ztpDisconnectParam;

typedef struct ztpSqlContext
{
    ztpParseTreeType      mParseTreeType;
    ztpContextActionType  mContextActionType;
    stlChar               mName[STL_MAX_SQL_IDENTIFIER_LENGTH];
} ztpSqlContext;

typedef struct ztpAutoCommit
{
    ztpParseTreeType      mParseTreeType;
    stlBool               mAutocommitOn;
} ztpAutoCommit;

typedef struct ztpCParseParam
{
    void                 *mYyScanner;
    stlAllocator         *mAllocator;
    stlErrorStack        *mErrorStack;
    stlChar              *mErrorMsg;
    stlChar              *mBuffer;
    stlInt32              mSQLStartPos;
    stlStatus             mErrorStatus;
    stlBool               mHasSQLError;
    stlBool               mNeedPrint;
    stlBool               mInnerDeclSec;
    stlBool               mIsTypedef;
    stlBool               mBypassCToken;
    stlBool               mIsAtomic;
    stlInt32              mPosition;
    stlInt32              mLength;
    stlInt32              mFileLength;
    stlInt32              mLineNo;
    stlInt32              mColumn;
    stlInt32              mColumnLen;
    stlInt32              mCurPosition;
    stlInt32              mCCodeStartLoc;
    stlInt32              mCCodeEndLoc;
    stlInt32              mCurrLoc;
    stlInt32              mNextLoc;
    stlFile              *mOutFile;
    stlChar              *mInFileName;
    stlChar              *mOutFileName;
    ztpParseTree         *mParseTree;
    void                 *mContext;
    stlChar              *mIterationValue;
    stlChar              *mConnStr;
    ztpExecSqlException  *mException[ZTP_EXCEPTION_COND_MAX];

    ztpCDeclarator       *mCDeclarator;

    stlChar              *mTempBuffer;
    ztpTokenStream        mTokenStream;
    stlChar               mCurrContext[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar               mTempString[STL_MAX_SQL_IDENTIFIER_LENGTH];
} ztpCParseParam;

/*
 * Preprocessor Parsing
 */

typedef struct ztpArgNameList
{
    stlChar                *mArgName;
    struct ztpArgNameList  *mNext;
} ztpArgNameList;

typedef struct ztpArgList
{
    stlInt64             mValue;
    struct ztpArgList   *mNext;
} ztpArgList;

typedef struct ztpMacroSymbol
{
    stlChar           *mMacroName;
    stlChar           *mContents;
    ztpArgNameList    *mArgNameList;

    /*
     * mArgCount :
     *  -1   : Object-like macro
     *  >= 0 : Function-like macro argument count
     */
    stlInt32           mArgCount;
    stlBool            mIsVarArgs;
    stlRingEntry       mLink;
} ztpMacroSymbol;

#define ZTP_INIT_MACRO_SYMBOL( aMacroSymbol )                           \
    {                                                                   \
        (aMacroSymbol)->mMacroName = NULL;                              \
        (aMacroSymbol)->mContents = NULL;                               \
        (aMacroSymbol)->mArgNameList = NULL;                            \
        (aMacroSymbol)->mArgCount = -1;                                 \
        (aMacroSymbol)->mIsVarArgs = STL_FALSE;                         \
        STL_RING_INIT_DATALINK( (aMacroSymbol), STL_OFFSETOF(ztpMacroSymbol, mLink) ); \
    }

typedef enum
{
    ZTP_PPOP_NONE,
    ZTP_PPOP_PLUS,
    ZTP_PPOP_MINUS,
    ZTP_PPOP_TILDE,
    ZTP_PPOP_NOT,
    ZTP_PPOP_MUL,
    ZTP_PPOP_DIV,
    ZTP_PPOP_MOD,
    ZTP_PPOP_ADD,
    ZTP_PPOP_SUB,
    ZTP_PPOP_SL,       /* 10 */
    ZTP_PPOP_SR,
    ZTP_PPOP_LT,
    ZTP_PPOP_GT,
    ZTP_PPOP_LE,
    ZTP_PPOP_GE,
    ZTP_PPOP_EQ,
    ZTP_PPOP_NE,
    ZTP_PPOP_BIT_AND,
    ZTP_PPOP_BIT_XOR,
    ZTP_PPOP_BIT_OR,   /* 20 */
    ZTP_PPOP_AND,
    ZTP_PPOP_OR,
    ZTP_PPOP_MAX
} ztpPPOperator;

typedef enum
{
    ZTP_TERMINAL_TYPE_VALUE,
    ZTP_TERMINAL_TYPE_EXPRESSION,
    ZTP_TERMINAL_TYPE_OBJECT_LIKE_MACRO,
    ZTP_TERMINAL_TYPE_FUNCTION_LIKE_MACRO,
    ZTP_TERMINAL_TYPE_MAX
} ztpPPTerminalType;

typedef enum
{
    ZTP_MACRO_TOKEN_ID,
    ZTP_MACRO_TOKEN_ETC,
    ZTP_MACRO_TOKEN_NULL,
    ZTP_MACRO_TOKEN_MAX
} ztpPPMacroTokenId;

typedef struct ztpPPParameterList     ztpPPParameterList;
typedef struct ztpPPPrimaryExpr       ztpPPPrimaryExpr;
typedef struct ztpPPUnaryExpr         ztpPPUnaryExpr;
typedef struct ztpPPMulExpr           ztpPPMulExpr;
typedef struct ztpPPAddExpr           ztpPPAddExpr;
typedef struct ztpPPShiftExpr         ztpPPShiftExpr;
typedef struct ztpPPRelationalExpr    ztpPPRelationalExpr;
typedef struct ztpPPEqualExpr         ztpPPEqualExpr;
typedef struct ztpPPBitAndExpr        ztpPPBitAndExpr;
typedef struct ztpPPBitXorExpr        ztpPPBitXorExpr;
typedef struct ztpPPBitOrExpr         ztpPPBitOrExpr;
typedef struct ztpPPAndExpr           ztpPPAndExpr;
typedef struct ztpPPOrExpr            ztpPPOrExpr;
typedef struct ztpPPCondExpr          ztpPPCondExpr;
typedef struct ztpPPConstExpr         ztpPPConstExpr;

struct ztpPPParameterList
{
    ztpPPConstExpr         *mConstExpr;
    stlInt64                mValue;
    ztpPPParameterList     *mNext;
};

struct ztpPPPrimaryExpr
{
    ztpPPTerminalType       mType;
    stlInt64                mValue;
    ztpPPConstExpr         *mConstExpr;
    stlChar                *mMacroName;
    ztpPPParameterList     *mParameterList;
};

struct ztpPPUnaryExpr
{
    ztpPPPrimaryExpr       *mPrimaryExpr;
    ztpPPUnaryExpr         *mUnaryExpr;
    ztpPPOperator           mOperator;
};

struct ztpPPMulExpr
{
    ztpPPMulExpr           *mMulExpr;
    ztpPPUnaryExpr         *mUnaryExpr;
    ztpPPOperator           mOperator;
};

struct ztpPPAddExpr
{
    ztpPPAddExpr           *mAddExpr;
    ztpPPMulExpr           *mMulExpr;
    ztpPPOperator           mOperator;
};

struct ztpPPShiftExpr
{
    ztpPPShiftExpr         *mShiftExpr;
    ztpPPAddExpr           *mAddExpr;
    ztpPPOperator           mOperator;
};

struct ztpPPRelationalExpr
{
    ztpPPRelationalExpr    *mRelationalExpr;
    ztpPPShiftExpr         *mShiftExpr;
    ztpPPOperator           mOperator;
};

struct ztpPPEqualExpr
{
    ztpPPEqualExpr         *mEqualExpr;
    ztpPPRelationalExpr    *mRelationalExpr;
    ztpPPOperator           mOperator;
};

struct ztpPPBitAndExpr
{
    ztpPPBitAndExpr        *mBitAndExpr;
    ztpPPEqualExpr         *mEqualExpr;
};

struct ztpPPBitXorExpr
{
    ztpPPBitXorExpr        *mBitXorExpr;
    ztpPPBitAndExpr        *mBitAndExpr;
};

struct ztpPPBitOrExpr
{
    ztpPPBitOrExpr         *mBitOrExpr;
    ztpPPBitXorExpr        *mBitXorExpr;
};

struct ztpPPAndExpr
{
    ztpPPAndExpr           *mAndExpr;
    ztpPPBitOrExpr         *mBitOrExpr;
};

struct ztpPPOrExpr
{
    ztpPPOrExpr            *mOrExpr;
    ztpPPAndExpr           *mAndExpr;
};

struct ztpPPCondExpr
{
    ztpPPOrExpr            *mOrExpr;
    ztpPPConstExpr         *mConstExpr;
    ztpPPCondExpr          *mCondExpr;
};

struct ztpPPConstExpr
{
    ztpPPCondExpr          *mCondExpr;
    stlInt64                mResult;
};

typedef struct ztpPPParseParam
{
    void                 *mYyScanner;
    stlAllocator         *mAllocator;
    stlErrorStack        *mErrorStack;
    stlChar              *mErrorMsg;
    stlChar              *mBuffer;
    stlChar              *mMacroReplaceBuffer;
    stlChar              *mMacroTokenBuffer;
    stlStatus             mErrorStatus;
    stlBool               mNeedPrint;

    /*
     * 연속된 #if, #elif, #else 문에서 한번이라도 TRUE 였던
     * condition이 존재했었는지를 check
     * 한번이라도 TRUE 였었다면, 이후에는 #elif 와 #else 는
     * skip 해야만 함
     */
    stlBool               mOccurConditionTrue;
    stlChar               mPad[2];

    /*
     * 현재 #if group의 level을 표시. 중첩된 #if 사용에서
     * 결합된 #elif, #else, #endif 를 식별하기 위해 사용
     * Level 이 증가할때는 현재 group을 expand 중이라는 의미이므로,
     * mOccurConditionTrue == TRUE 이어야만 한다.
     * 따라서, #endif 를 만나서 Level을 감소시킬 때는
     * mOccurConditionTrue = TRUE
     * 로 값을 설정해 주어야만 한다.
     */
    stlInt32              mIfLevel;
    stlInt32              mPosition;      /* Used by Lexer: input position */
    stlInt32              mLength;
    stlInt32              mFileLength;    /* 현재 분석중인 File의 길이 */
    stlInt32              mLineNo;        /* 현재 Token의 Line number */
    stlInt32              mColumn;        /* 현재 Token의 Line내에서의 Column */
    stlInt32              mColumnLen;     /* 현재 Token의 Column의 길이: error msg 출력에 사용 */
    stlInt32              mCCodeStartLoc; /* Bypass할 C Code의 시작 위치 */
    stlInt32              mCCodeEndLoc;   /* Bypsss할 C Code의 마지막 위치 */
    stlInt32              mCurrLoc;       /* 현재 Token의 시작 위치 */
    stlInt32              mNextLoc;       /* 다음 Token의 시작 위치 */
    stlFile              *mOutFile;
    stlChar              *mInFileName;
    stlChar              *mOutFileName;
    ztpParseTree         *mParseTree;
    void                 *mContext;

    stlChar              *mTempBuffer;
    ztpTokenStream        mTokenStream;
} ztpPPParseParam;

/*
 * Preprocessor Calcurator Parsing
 */

typedef struct ztpCalcResult
{
    stlInt64              mValue;
} ztpCalcResult;

typedef struct ztpCalcParseParam
{
    void                 *mYyScanner;
    stlAllocator         *mAllocator;
    stlErrorStack        *mErrorStack;
    stlChar              *mErrorMsg;
    stlChar              *mBuffer;

    /*
     * Expression 계산 결과
     */
    stlInt64              mResult;

    stlStatus             mErrorStatus;

    stlInt32              mPosition;      /* Used by Lexer: input position */
    stlInt32              mLength;
    stlInt32              mExprLength;
    stlInt32              mLineNo;        /* 현재 Token의 Line number */
    stlInt32              mColumn;        /* 현재 Token의 Line내에서의 Column */
    stlInt32              mColumnLen;     /* 현재 Token의 Column의 길이: error msg 출력에 사용 */
    stlInt32              mCurrLoc;       /* 현재 Token의 시작 위치 */
    stlInt32              mNextLoc;       /* 다음 Token의 시작 위치 */

} ztpCalcParseParam;

/*
 * C Host variable parsing
 */

typedef struct ztpCHostVarParseParam
{
    void                 *mYyScanner;
    stlAllocator         *mAllocator;
    stlErrorStack        *mErrorStack;
    stlChar              *mErrorMsg;
    stlChar              *mBuffer;

    /*
     * C Host variable string
     */
    stlChar              *mHostVarStr;

    stlStatus             mErrorStatus;

    stlInt32              mPosition;      /* Used by Lexer: input position */
    stlInt32              mLength;
    stlInt32              mCHostVarLength;
    stlInt32              mLineNo;        /* 현재 Token의 Line number */
    stlInt32              mColumn;        /* 현재 Token의 Line내에서의 Column */
    stlInt32              mColumnLen;     /* 현재 Token의 Column의 길이: error msg 출력에 사용 */
    stlInt32              mCurrLoc;       /* 현재 Token의 시작 위치 */
    stlInt32              mNextLoc;       /* 다음 Token의 시작 위치 */
    ztpHostVariable      *mHostVar;
} ztpCHostVarParseParam;

/*
 * Path description
 */

typedef struct ztpIncludeFileParseParam
{
    void                 *mYyScanner;
    stlAllocator         *mAllocator;
    stlErrorStack        *mErrorStack;
    stlChar              *mErrorMsg;
    stlChar              *mBuffer;

    /*
     * Path description
     */
    stlChar              *mPathDesc;

    stlStatus             mErrorStatus;

    stlInt32              mPosition;      /* Used by Lexer: input position */
    stlInt32              mLength;
    stlInt32              mPathLength;
    stlInt32              mLineNo;        /* 현재 Token의 Line number */
    stlInt32              mColumn;        /* 현재 Token의 Line내에서의 Column */
    stlInt32              mColumnLen;     /* 현재 Token의 Column의 길이: error msg 출력에 사용 */
    stlInt32              mCurrLoc;       /* 현재 Token의 시작 위치 */
    stlInt32              mNextLoc;       /* 다음 Token의 시작 위치 */
} ztpIncludeFileParseParam;

/*
 * Path description list
 */

typedef struct ztpPathParseParam
{
    void                 *mYyScanner;
    stlAllocator         *mAllocator;
    stlErrorStack        *mErrorStack;
    stlChar              *mErrorMsg;
    stlChar              *mBuffer;

    /*
     * Path description list
     */
    ztpIncludePath       *mPathList;

    stlStatus             mErrorStatus;

    stlInt32              mPosition;      /* Used by Lexer: input position */
    stlInt32              mLength;
    stlInt32              mPathLength;
    stlInt32              mLineNo;        /* 현재 Token의 Line number */
    stlInt32              mColumn;        /* 현재 Token의 Line내에서의 Column */
    stlInt32              mColumnLen;     /* 현재 Token의 Column의 길이: error msg 출력에 사용 */
    stlInt32              mCurrLoc;       /* 현재 Token의 시작 위치 */
    stlInt32              mNextLoc;       /* 다음 Token의 시작 위치 */
} ztpPathParseParam;

#define ZTP_EMPTY_PARSE_TREE ((void*)1)

typedef stlStatus (*ztpCvtDTFunc)(ztpCParseParam  *aParam,
                                  ztpCDeclaration *aCDeclaration,
                                  ztpCVariable    *aCVariableListHead);
typedef stlStatus (*ztpdRefineDTFunc)( ztpCVariable     *aCVariable,
                                       ztpCDeclaration  *aCDeclaration );
typedef stlStatus (*ztpdRefineDTHostVarFunc)(ztpCVariable    *aCVariable);
typedef stlStatus (*ztpExceptCondFunc)(ztpCParseParam  *aParam,
                                       stlInt32         aCondType,
                                       stlChar         *aExceptionStr);
typedef stlStatus (*ztpExceptActFunc)(ztpCParseParam  *aParam,
                                      stlInt32         aCondType,
                                      stlChar         *aExceptionStr);

/** @} */

/**
 * @addtogroup Declarator Specifier
 * @{
 */

#define ZTP_SPECIFIER_BIT_TYPEDEF                     0
#define ZTP_SPECIFIER_BIT_CHAR                        1
#define ZTP_SPECIFIER_BIT_SHORT                       2
#define ZTP_SPECIFIER_BIT_INT                         3
#define ZTP_SPECIFIER_BIT_LONG                        4
#define ZTP_SPECIFIER_BIT_LONGLONG                    5
#define ZTP_SPECIFIER_BIT_FLOAT                       6
#define ZTP_SPECIFIER_BIT_DOUBLE                      7
#define ZTP_SPECIFIER_BIT_SIGNED                      8
#define ZTP_SPECIFIER_BIT_UNSIGNED                    9
#define ZTP_SPECIFIER_BIT_DERIVED                     10
#define ZTP_SPECIFIER_BIT_STRUCT                      11
#define ZTP_SPECIFIER_BIT_SQLCONTEXT                  12
#define ZTP_SPECIFIER_BIT_VARCHAR                     13
#define ZTP_SPECIFIER_BIT_LONGVARCHAR                 14
#define ZTP_SPECIFIER_BIT_BINARY                      15
#define ZTP_SPECIFIER_BIT_VARBINARY                   16
#define ZTP_SPECIFIER_BIT_LONGVARBINARY               17
#define ZTP_SPECIFIER_BIT_BIT                         18
#define ZTP_SPECIFIER_BIT_BOOLEAN                     19
#define ZTP_SPECIFIER_BIT_NUMERIC                     20
#define ZTP_SPECIFIER_BIT_DATE                        21
#define ZTP_SPECIFIER_BIT_TIME                        22
#define ZTP_SPECIFIER_BIT_TIMETZ                      23
#define ZTP_SPECIFIER_BIT_TIMESTAMP                   24
#define ZTP_SPECIFIER_BIT_TIMESTAMPTZ                 25
#define ZTP_SPECIFIER_BIT_INTERVAL_YEAR               26
#define ZTP_SPECIFIER_BIT_INTERVAL_MONTH              27
#define ZTP_SPECIFIER_BIT_INTERVAL_DAY                28
#define ZTP_SPECIFIER_BIT_INTERVAL_HOUR               29
#define ZTP_SPECIFIER_BIT_INTERVAL_MINUTE             30
#define ZTP_SPECIFIER_BIT_INTERVAL_SECOND             31
#define ZTP_SPECIFIER_BIT_INTERVAL_YEAR_TO_MONTH      32
#define ZTP_SPECIFIER_BIT_INTERVAL_DAY_TO_HOUR        33
#define ZTP_SPECIFIER_BIT_INTERVAL_DAY_TO_MINUTE      34
#define ZTP_SPECIFIER_BIT_INTERVAL_DAY_TO_SECOND      35
#define ZTP_SPECIFIER_BIT_INTERVAL_HOUR_TO_MINUTE     36
#define ZTP_SPECIFIER_BIT_INTERVAL_HOUR_TO_SECOND     37
#define ZTP_SPECIFIER_BIT_INTERVAL_MINUTE_TO_SECOND   38

#define ZTP_SET_BIT(aFlag, aBit)      ((aFlag) |= (1LL << (aBit)))
#define ZTP_CLEAR_BIT(aFlag, aBit)    ((aFlag) &= ~(1LL << (aBit)))
#define ZTP_IS_SET_BIT(aFlag, aBit)   (((aFlag) & (1LL << (aBit))) ? STL_TRUE : STL_FALSE)

typedef enum
{
    ZTP_SPECIFIER_UNKNOWN,                     /* 0  */
    ZTP_SPECIFIER_CHAR,                        /* 1  */
    ZTP_SPECIFIER_SHORT,                       /* 2  */
    ZTP_SPECIFIER_INT,                         /* 3  */
    ZTP_SPECIFIER_LONG,                        /* 4  */
    ZTP_SPECIFIER_LONGLONG,                    /* 5  */
    ZTP_SPECIFIER_FLOAT,                       /* 6  */
    ZTP_SPECIFIER_DOUBLE,                      /* 7  */
    ZTP_SPECIFIER_SIGNED,                      /* 8  */
    ZTP_SPECIFIER_UNSIGNED,                    /* 9  */
    ZTP_SPECIFIER_DERIVED,                     /* 10 */
    ZTP_SPECIFIER_STRUCT,                      /* 11 */
    ZTP_SPECIFIER_SQLCONTEXT,                  /* 12 */
    ZTP_SPECIFIER_VARCHAR,                     /* 13 */
    ZTP_SPECIFIER_LONGVARCHAR,                 /* 14 */
    ZTP_SPECIFIER_BINARY,                      /* 15 */
    ZTP_SPECIFIER_VARBINARY,                   /* 16 */
    ZTP_SPECIFIER_LONGVARBINARY,               /* 17 */
    ZTP_SPECIFIER_BIT,                         /* 18 */
    ZTP_SPECIFIER_BOOLEAN,                     /* 19 */
    ZTP_SPECIFIER_NUMBER,                      /* 20 */
    ZTP_SPECIFIER_DATE,                        /* 21 */
    ZTP_SPECIFIER_TIME,                        /* 22 */
    ZTP_SPECIFIER_TIMETZ,                      /* 23 */
    ZTP_SPECIFIER_TIMESTAMP,                   /* 24 */
    ZTP_SPECIFIER_TIMESTAMPTZ,                 /* 25 */
    ZTP_SPECIFIER_INTERVAL_YEAR,               /* 26 */
    ZTP_SPECIFIER_INTERVAL_MONTH,              /* 27 */
    ZTP_SPECIFIER_INTERVAL_DAY,                /* 28 */
    ZTP_SPECIFIER_INTERVAL_HOUR,               /* 29 */
    ZTP_SPECIFIER_INTERVAL_MINUTE,             /* 30 */
    ZTP_SPECIFIER_INTERVAL_SECOND,             /* 31 */
    ZTP_SPECIFIER_INTERVAL_YEAR_TO_MONTH,      /* 32 */
    ZTP_SPECIFIER_INTERVAL_DAY_TO_HOUR,        /* 33 */
    ZTP_SPECIFIER_INTERVAL_DAY_TO_MINUTE,      /* 34 */
    ZTP_SPECIFIER_INTERVAL_DAY_TO_SECOND,      /* 35 */
    ZTP_SPECIFIER_INTERVAL_HOUR_TO_MINUTE,     /* 36 */
    ZTP_SPECIFIER_INTERVAL_HOUR_TO_SECOND,     /* 37 */
    ZTP_SPECIFIER_INTERVAL_MINUTE_TO_SECOND,   /* 38 */
    ZTP_SPECIFIER_TYPEDEF,                     /* 39  */
    ZTP_SPECIFIER_MAX
} ztpTypeSpecifier;

typedef struct ztpTypeSpecifierNode
{
    ztpTypeSpecifier  mTypeSpecifier;
    stlChar          *mPrecision;
    stlChar          *mScale;
    ztpCVariable     *mCVariable;
    stlInt32          mPosition;
    stlInt32          mLength;
} ztpTypeSpecifierNode;

typedef struct ztpTypeSpecifierList ztpTypeSpecifierList;
struct ztpTypeSpecifierList
{
    ztpTypeSpecifierNode  *mTypeSpecifierNode;
    ztpTypeSpecifierList  *mNext;
};

/** @} */

/**
 * @defgroup ztphStaticHash Static Hash
 * @{
 */

/**
 * @brief Static Hash 관리를 위한 Header
 * 바로 뒤에 mBucketCount 만큼의 ztphStaticHashBucket의 배열이 있다
 */
typedef struct ztphStaticHash
{
    stlUInt16        mBucketCount;
    stlUInt16        mLinkOffset;
    stlUInt16        mKeyOffset;
    stlChar          mAlign1[2];
#if (STL_SIZEOF_VOIDP == 4 )
    void           * mAlign2;
#endif
    void           * mHintNode;
} ztphStaticHash;

typedef struct ztphStaticHashBucket
{
    stlRingHead      mList;
} ztphStaticHashBucket;

typedef stlUInt32 (*ztphStaticHashHashingFunc)(void * aKey, stlUInt32 aBucketCount);
typedef stlInt32 (*ztphStaticHashCompareFunc)(void * aKeyA, void * aKeyB);

/** @} */

extern stlBool                  gIsPathGivenextern;
extern stlBool                  gUnsafeNull;
extern stlBool                  gNoLineInfo;
extern stlChar                  gInFilePath[STL_MAX_FILE_PATH_LENGTH + 1];
extern stlChar                  gInFileDir[STL_MAX_FILE_PATH_LENGTH + 1];
extern stlChar                  gOutFilePath[STL_MAX_FILE_PATH_LENGTH + 1];
extern ztpIncludePath           gIncludePath;
extern ztpTempBuffer            gTempBuffer;
extern stlChar                  gOutputHeader[];
extern stlSize                  gOutputHeaderSize;
extern ztpCvtDTFunc             gCvtDTFunc[];
extern ztpdRefineDTFunc         gRefineDTFunc[];
extern ztpdRefineDTHostVarFunc  gRefineDTHostVarFunc[];
extern ztpExceptCondFunc        gExceptCondFunc[];
extern ztpExceptActFunc         gExceptActFunc[];
extern ztpSymTabHeader         *gRootSymTab;
extern ztpSymTabHeader         *gCurrSymTab;
extern ztphStaticHash          *gMacroSymTab;
extern ztpParsingLevel          gParsingLevel;
extern stlChar                 *gSqlBuffer;
extern stlInt32                 gSqlBufLen;
extern stlInt32                 gSqlBufferIdx;


/** @} */

#include <ztpErrDef.h>

#endif /* _ZTPDEF_H_ */
