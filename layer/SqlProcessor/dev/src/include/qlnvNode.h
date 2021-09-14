/*******************************************************************************
 * qlnvNode.h
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

#ifndef _QLNVNODE_H_
#define _QLNVNODE_H_ 1

/**
 * @file qlnvNode.h
 * @brief SQL Processor Layer Validation Node
 */

#include <qlDef.h>

/**
 * @addtogroup qlnv
 * @{
 */


/*******************************************************************************
 * DEFINITIONS
 ******************************************************************************/


/**
 * @brief Query Block Name Size
 */

#define QLV_QUERY_BLOCK_NAME_MAX_SIZE   32      /**< Query Block Name의 최대 길이 */
#define QLV_QUERY_BLOCK_NAME_PREFIX_INS "@INS_" /**< Insert Stmt의 Query Block Name Prefix */
#define QLV_QUERY_BLOCK_NAME_PREFIX_DEL "@DEL_" /**< Delete Stmt의 Query Block Name Prefix */
#define QLV_QUERY_BLOCK_NAME_PREFIX_UPD "@UPD_" /**< Update Stmt의 Query Block Name Prefix */
#define QLV_QUERY_BLOCK_NAME_PREFIX_SEL "@SEL_" /**< Select Stmt의 Query Block Name Prefix */


/**
 * @brief Internal Bind Idx
 */
#define QLV_INTERNAL_BIND_VARIABLE_IDX_NA  (0)   /**< qlvInitValue를 이용 : stlInt32 */


/**
 * @brief expression type
 */
enum qlvExprType
{
    QLV_EXPR_TYPE_VALUE = 0,
    QLV_EXPR_TYPE_BIND_PARAM,
    QLV_EXPR_TYPE_COLUMN,
    QLV_EXPR_TYPE_FUNCTION,
    QLV_EXPR_TYPE_CAST,
    QLV_EXPR_TYPE_PSEUDO_COLUMN,
    QLV_EXPR_TYPE_PSEUDO_ARGUMENT,
    QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT,
    QLV_EXPR_TYPE_REFERENCE,
    QLV_EXPR_TYPE_SUB_QUERY,
    QLV_EXPR_TYPE_INNER_COLUMN,
    QLV_EXPR_TYPE_OUTER_COLUMN,
    QLV_EXPR_TYPE_ROWID_COLUMN,
    QLV_EXPR_TYPE_AGGREGATION,
    QLV_EXPR_TYPE_CASE_EXPR,
    QLV_EXPR_TYPE_LIST_FUNCTION,
    QLV_EXPR_TYPE_LIST_COLUMN,
    QLV_EXPR_TYPE_ROW_EXPR,
    QLV_EXPR_TYPE_MAX
};


/**
 * @brief sub-query category
 */
enum qlvSubQueryCategory
{
    QLV_SUB_QUERY_TYPE_SCALAR,
    QLV_SUB_QUERY_TYPE_ROW,
    QLV_SUB_QUERY_TYPE_RELATION
};


/**
 * @brief sub-query type
 */
enum qlvSubQueryType
{
    QLV_SUB_QUERY_N_TYPE,
    QLV_SUB_QUERY_A_TYPE,
    QLV_SUB_QUERY_J_TYPE,
    QLV_SUB_QUERY_AJ_TYPE
};

/**
 * @brief List Function type
 */
enum qlvListFuncType
{
    QLV_LIST_FUNC_NESTED_TYPE,
    QLV_LIST_FUNC_CORRELATED_TYPE
};


/**
 * @brief expression value type
 * @remark Value Type 수정시 qlncExpr.h의 Value Type도 같이 수정해야 함
 */
enum qlvValueType
{
    QLV_VALUE_TYPE_INVALID = 0,
    QLV_VALUE_TYPE_BOOLEAN,
    QLV_VALUE_TYPE_BINARY_INTEGER,
    QLV_VALUE_TYPE_BINARY_REAL,
    QLV_VALUE_TYPE_BINARY_DOUBLE,
    QLV_VALUE_TYPE_NUMERIC,
    QLV_VALUE_TYPE_STRING_FIXED,
    QLV_VALUE_TYPE_STRING_VARYING,
    QLV_VALUE_TYPE_BITSTRING_FIXED,
    QLV_VALUE_TYPE_BITSTRING_VARYING,
    QLV_VALUE_TYPE_DATE_LITERAL_STRING,
    QLV_VALUE_TYPE_TIME_LITERAL_STRING,
    QLV_VALUE_TYPE_TIME_TZ_LITERAL_STRING,
    QLV_VALUE_TYPE_TIMESTAMP_LITERAL_STRING,
    QLV_VALUE_TYPE_TIMESTAMP_TZ_LITERAL_STRING,
    QLV_VALUE_TYPE_INTERVAL_YEAR_TO_MONTH_STRING,
    QLV_VALUE_TYPE_INTERVAL_DAY_TO_SECOND_STRING,
    QLV_VALUE_TYPE_ROWID_STRING,
    QLV_VALUE_TYPE_NULL
};


/**
 * @brief hint type
 */
enum qlvHintType
{
    QLV_HINT_TYPE_INVALID = 0,
    QLV_HINT_TYPE_INDEX_SCAN,
};


/**
 * @brief Query Element Type
 */
enum qlvNodeType
{
    QLV_NODE_TYPE_QUERY_SET = 0,      /**< Set Expression Node */
    QLV_NODE_TYPE_QUERY_SPEC,         /**< Query Specification Node */
    QLV_NODE_TYPE_BASE_TABLE,         /**< Base Table Node */
    QLV_NODE_TYPE_SUB_TABLE,          /**< Sub Table Node */
    QLV_NODE_TYPE_JOIN_TABLE,         /**< Join Table Node */
    QLV_NODE_TYPE_FLAT_INSTANT,       /**< Flat Instant Node */
    QLV_NODE_TYPE_SORT_INSTANT,       /**< Sort Instant Node */
    QLV_NODE_TYPE_HASH_INSTANT,       /**< Hash Instant Node */
    QLV_NODE_TYPE_GROUP_HASH_INSTANT, /**< Group Hash Instant Node */
    QLV_NODE_TYPE_GROUP_SORT_INSTANT, /**< Group Sort Instant Node */
    QLV_NODE_TYPE_SORT_JOIN_INSTANT,  /**< Sort Join Instant Node */
    QLV_NODE_TYPE_HASH_JOIN_INSTANT,  /**< Hash Instant Node */
    QLV_NODE_TYPE_MAX
};


/**
 * @brief Set Type
 */

enum qlvSetType
{
    QLV_SET_TYPE_NA         = 0,    /**< N/A */
    QLV_SET_TYPE_UNION,             /**< UNION */
    QLV_SET_TYPE_EXCEPT,            /**< EXCEPT */
    QLV_SET_TYPE_INTERSECT          /**< INTERSECT */
};


/**
 * @brief Set Option
 */

enum qlvSetOption
{
    QLV_SET_OPTION_NA       = 0,    /**< N/A */
    QLV_SET_OPTION_ALL,             /**< set ALL */
    QLV_SET_OPTION_DISTINCT         /**< set DISTINCT */
};


/**
 * @brief Join Type
 */

enum qlvJoinType
{
    QLV_JOIN_TYPE_NONE          = 0,        /**< Comma List Join */
    QLV_JOIN_TYPE_CROSS,                    /**< Cross Join */
    QLV_JOIN_TYPE_INNER,                    /**< Inner Join */

    QLV_JOIN_TYPE_LEFT_OUTER,               /**< Left Outer Join */
    QLV_JOIN_TYPE_RIGHT_OUTER,              /**< Right Outer Join */
    QLV_JOIN_TYPE_FULL_OUTER,               /**< Full Outer Join */

    QLV_JOIN_TYPE_LEFT_SEMI,                /**< Left Semi Join */
    QLV_JOIN_TYPE_RIGHT_SEMI,               /**< Right Semi Join */

    QLV_JOIN_TYPE_LEFT_ANTI_SEMI,           /**< Left Anti-Semi Join */
    QLV_JOIN_TYPE_RIGHT_ANTI_SEMI,          /**< Right Anti-Semi Join */

    QLV_JOIN_TYPE_LEFT_ANTI_SEMI_NA,        /**< Left Null Aware Anti-Semi Join */
    QLV_JOIN_TYPE_RIGHT_ANTI_SEMI_NA,       /**< Right Null Aware Anti-Semi Join */

    QLV_JOIN_TYPE_INVERTED_LEFT_SEMI,       /**< Inverted Left Semi Join */
    QLV_JOIN_TYPE_INVERTED_RIGHT_SEMI,      /**< Inverted Right Semi Join */

    QLV_JOIN_TYPE_MAX
};


/**
 * @brief Hint 관련
 */

typedef stlUInt8 qlvHintFlag;

#define QLV_HINT_MASK       (qlvHintFlag)0x03   /**< Hint Mask */
#define QLV_HINT_DEFAULT    (qlvHintFlag)0x00   /**< Default */
#define QLV_HINT_YES        (qlvHintFlag)0x01   /**< Yes */
#define QLV_HINT_NO         (qlvHintFlag)0x02   /**< No */

#define QLV_HINT_SCAN_METHOD_DEFAULT        (qlvHintFlag)0x00   /**< Default */
#define QLV_HINT_SCAN_METHOD_TABLE          (qlvHintFlag)0x01   /**< Table */
#define QLV_HINT_SCAN_METHOD_USE_INDEX      (qlvHintFlag)0x02   /**< Use Index */
#define QLV_HINT_SCAN_METHOD_NO_USE_INDEX   (qlvHintFlag)0x04   /**< No Use Index */
#define QLV_HINT_SCAN_METHOD_ROWID          (qlvHintFlag)0x08   /**< Rowid */
#define QLV_HINT_SCAN_METHOD_INDEX_COMBINE  (qlvHintFlag)0x10   /**< INDEX COMBINE */
#define QLV_HINT_SCAN_METHOD_IN_KEY_RANGE   (qlvHintFlag)0x20   /**< In Key Range */

enum qlvJoinOperType
{
    QLV_HINT_JOIN_OPER_TYPE_DEFAULT = 0,    /**< Default */
    QLV_HINT_JOIN_OPER_TYPE_USE_NL,         /**< USE_NL */
    QLV_HINT_JOIN_OPER_TYPE_NO_USE_NL,      /**< NO_USE_NL */
    QLV_HINT_JOIN_OPER_TYPE_USE_INL,        /**< USE_INL */
    QLV_HINT_JOIN_OPER_TYPE_NO_USE_INL,     /**< NO_USE_INL */
    QLV_HINT_JOIN_OPER_TYPE_USE_MERGE,      /**< USE_MERGE */
    QLV_HINT_JOIN_OPER_TYPE_NO_USE_MERGE,   /**< NO_USE_MERGE */
    QLV_HINT_JOIN_OPER_TYPE_USE_HASH,       /**< USE_HASH */
    QLV_HINT_JOIN_OPER_TYPE_NO_USE_HASH,    /**< NO_USE_HASH */
    QLV_HINT_JOIN_OPER_TYPE_MAX
};

enum qlvQueryTransformType
{
    QLV_HINT_QUERY_TRANSFORM_TYPE_DEFAULT = 0,  /**< DEFAULT */
    QLV_HINT_QUERY_TRANSFORM_TYPE_ALLOW,        /**< NO HINT */
    QLV_HINT_QUERY_TRANSFORM_TYPE_FORBID        /**< NO_QUERY_TRANSFORMATION */
};

enum qlvSubQueryUnnestType
{
    QLV_HINT_SUBQUERY_UNNEST_TYPE_DEFAULT = 0,  /**< DEFAULT */
    QLV_HINT_SUBQUERY_UNNEST_TYPE_ALLOW,        /**< UNNEST */
    QLV_HINT_SUBQUERY_UNNEST_TYPE_FORBID        /**< NO_UNNEST */
};

enum qlvUnnestOperType
{
    QLV_HINT_UNNEST_OPER_TYPE_DEFAULT = 0,      /**< DEFAULT */
    QLV_HINT_UNNEST_OPER_TYPE_NL_ISJ,           /**< NL_ISJ */
    QLV_HINT_UNNEST_OPER_TYPE_NL_SJ,            /**< NL_SJ */
    QLV_HINT_UNNEST_OPER_TYPE_NL_AJ,            /**< NL_AJ */
    QLV_HINT_UNNEST_OPER_TYPE_MERGE_SJ,         /**< MERGE_SJ */
    QLV_HINT_UNNEST_OPER_TYPE_MERGE_AJ,         /**< MERGE_AJ */
    QLV_HINT_UNNEST_OPER_TYPE_HASH_ISJ,         /**< HASH_ISJ */
    QLV_HINT_UNNEST_OPER_TYPE_HASH_SJ,          /**< HASH_SJ */
    QLV_HINT_UNNEST_OPER_TYPE_HASH_AJ           /**< HASH_AJ */
};


////////////////////////////////////////////////////////////

/****************************************************
 * QUERY EXPRESSION Nodes
 ****************************************************/ 

/**
 * @brief Validation Node
 */

struct qlvInitNode
{
    qlvNodeType     mType;
};


/**
 * @brief statement expression list
 */
struct qlvInitStmtExprList
{
    qlvRefExprList       * mPseudoColExprList;   /**< Pseudo Column 목록 */
    qlvRefExprList       * mConstExprList;       /**< Constant Expression 목록 */
    stlBool                mHasSubQuery;         /**< Stetement 내 SubQuery 존재 여부 */
};


/**
 * @brief expression list
 */
struct qlvInitExprList
{
    qlvInitStmtExprList  * mStmtExprList;        /**< Statement 단위 Expression List */
    qlvRefExprList       * mPseudoColExprList;   /**< Pseudo Column 목록 */
    qlvRefExprList       * mAggrExprList;        /**< Aggregation 목록 */
    qlvRefExprList       * mNestedAggrExprList;  /**< Aggregation 목록 */
};


/**
 * @brief statement information for validation
 */
struct qlvStmtInfo
{
    smlTransId         mTransID;
    stlBool            mHasHintError;
    stlInt32         * mQBIndex;
    stlChar            mQBPrefix[QLV_QUERY_BLOCK_NAME_MAX_SIZE];
    qllDBCStmt       * mDBCStmt;
    qllStatement     * mSQLStmt;
    stlChar          * mSQLString;
    qlvInitExprList  * mQueryExprList;
};


/**
 * @brief Index Scan Item
 */
struct qlvIndexScanItem
{
    ellDictHandle       mIndexHandle;
    qlvHintFlag         mAsc;
    qlvIndexScanItem  * mNext;
};


/**
 * @brief Index Scan List
 */
struct qlvIndexScanList
{
    stlInt32            mCount;
    qlvIndexScanItem  * mHead;
    qlvIndexScanItem  * mTail;
};


/**
 * @brief access path hint
 */
struct qlvAccessPathHint
{
    qlvHintFlag         mScanMethod;            /**< Scan Method */
    qlvIndexScanList    mUseIndexScanList;      /**< Use Index Scan List */
    qlvIndexScanList    mNoUseIndexScanList;    /**< No Use Index Scan List */
};


/**
 * @brief Join Order Hint
 */
struct qlvJoinOrderHint
{
    stlInt32            mJoinOrderIdx;      /**< Join Order Index */
    stlBool             mIsFixedOrder;      /**< Is Fixed Order */
    stlBool             mIsFixedPosition;   /**< Is Fixed Position */
    stlBool             mIsLeft;            /**< Is Left */
};


/**
 * @brief Join Operation Item
 */
struct qlvJoinOperItem
{
    qlvJoinOperType   mJoinOperType;    /**< Join Operation Type */
    stlInt32          mJoinTableCount;  /**< Join Operation Table Count */
    stlInt32        * mJoinTableArr;    /**< Join Table Array */
    qlvJoinOperItem * mNext;
};


/**
 * @brief Join Operation List
 */
struct qlvJoinOperList
{
    stlInt32          mCount;
    qlvJoinOperItem * mHead;
    qlvJoinOperItem * mTail;
};


/**
 * @brief Query Transformation Hint
 */
struct qlvQueryTransformHint
{
    qlvQueryTransformType     mQueryTransformType;
    qlvSubQueryUnnestType     mSubQueryUnnestType;
    qlvUnnestOperType         mUnnestOperType;
};


/**
 * @brief Other Hint
 */
struct qlvOtherHint
{
    qlvHintFlag       mPushSubq;
    qlvHintFlag       mUseDistinctHash;
    qlvHintFlag       mUseGroupHash;
};


/**
 * @brief hint information for validation
 */
struct qlvHintInfo
{
    smlTransId                mTransID;             /**< Transaction Id */
    stlInt32                  mTableCount;          /**< Base Table Count */
    stlBool                   mHasHintError;        /**< Hint Error 포함 여부 */
    qlvInitNode            ** mTableNodePtrArr;     /**< Base Table Node Pointer Array */

    qlvAccessPathHint       * mAccPathHintArr;      /**< Access Path Hint Array */
    stlInt32                  mJoinOrderCount;      /**< Join Order Count */
    qlvJoinOrderHint        * mJoinOrderArr;        /**< Join Order Array */
    qlvJoinOperList           mJoinOperList;        /**< Join Operatioin Hint List */
    qlvQueryTransformHint     mQueryTransformHint;  /**< Query Transformation Hint */
    qlvOtherHint              mOtherHint;           /**< Other Hint */
};


/****************************************************
 * QUERY EXPRESSION Nodes
 ****************************************************/ 

/**
 * @brief Set Expression Node
 */

struct qlvInitQuerySetNode
{
    qlvNodeType           mType;                /**< Node Type */

    stlChar               mQBName[QLV_QUERY_BLOCK_NAME_MAX_SIZE];   /**< Query Block Name */
    stlInt32              mQBID;                /**< Query Block Identifier */

    stlBool               mIsRootSet;           /**< Root Query Set 여부 */
    stlBool               mIsPureSet;           /**< Pure Query Set 여부 */

    qlvSetType            mSetType;             /**< Set Expression Type */
    qlvSetOption          mSetOption;           /**< Set Expression Option */
    
    stlInt32              mSameSetChildCnt;     /**< 동일한 set 연산자에 포함된 Subquery 개수 */
    
    /* expression list */
    qlvInitExprList     * mQueryExprList;   /**< Query 단위 Expression 정보 */

    stlInt32              mSetTargetCount;      /**< Set Expression Target Count */
    qlvInitTarget       * mSetTargets;          /**< Set Expression Target List : array */
    qlvInitTarget       * mSetColumns;          /**< Set Relation Columns : array */
    qlvInitNode         * mLeftQueryNode;       /**< Left Query Node */
    qlvInitNode         * mRightQueryNode;      /**< Right Query Node */
    /* @todo corresponding specification */
    void                * mCorrespondingSpec;   /**< Corresponding Specification */

    void                * mWithExpr;            /**< With Clause */
    qlvOrderBy          * mOrderBy;             /**< Order By Clause */
    qlvInitExpression   * mResultSkip;          /**< Result Skip Clause */
    qlvInitExpression   * mResultLimit;         /**< Result Limit Clause */
    stlInt64              mSkipCnt;             /**< Result Skip Count */
    stlInt64              mFetchCnt;            /**< Result Fetch Count */
};


/**
 * @brief Query Specification Node
 */

struct qlvInitQuerySpecNode
{
    qlvNodeType           mType;            /**< Node Type */
    qlvInitNode         * mTableNode;       /**< Table Node */
    stlInt32              mTableCount;      /**< Leaf Table Node Count */

    stlChar               mQBName[QLV_QUERY_BLOCK_NAME_MAX_SIZE];   /**< Query Block Name */
    stlInt32              mQBID;                /**< Query Block Identifier */

    /* expression list */
    qlvInitExprList     * mQueryExprList;   /**< Query 단위 Expression 정보 */

    /* Hint Info */
    qlvHintInfo         * mHintInfo;        /**< Hint Info */

    /* targets */
    stlInt32              mTargetCount;     /**< Target Count */
    qlvInitTarget       * mTargets;         /**< Target List : array */
    qlvInitInstantNode  * mDistinct;        /**< Target절의 Distinct */
    stlBool               mIsDistinct;      /**< Target절의 Distinct 여부 */
    
    stlBool               mHasAggrDistinct;        /**< Distinct 조건이 포함된 Aggregation이 존재하는지 여부 */
    stlBool               mHasNestedAggrDistinct;  /**< Distinct 조건이 포함된 Nested Aggregation이 존재하는지 여부 */

    /* where */
    qlvInitExpression   * mWhereExpr;       /**< Where Clause */

    /* group by */
    qlvGroupBy          * mGroupBy;         /**< Group By Clause */

    stlBool               mHasGroupOper;    /**< Group Operation의 존재여부 */

    /* order by */
    qlvOrderBy          * mOrderBy;         /**< Order By Clause */

    /* @todo window */
    void                * mWindowExpr;      /**< Window Clause */

    void                * mWithExpr;        /**< With Clause */
    qlvInitExpression   * mResultSkip;      /**< Result Skip Clause */
    qlvInitExpression   * mResultLimit;     /**< Result Limit Clause */
    stlInt64              mSkipCnt;         /**< Result Skip Count */
    stlInt64              mFetchCnt;        /**< Result Fetch Count */
};


/**
 * @brief Base Table Node
 */

struct qlvInitBaseTableNode
{
    qlvNodeType           mType;            /**< Node Type */
    stlBool               mIsIndexScanAsc;  /**< Index Scan Ascending 여부 */
    ellDictHandle         mSchemaHandle;    /**< Schema Handle of the table */
    ellDictHandle         mTableHandle;     /**< Table Handle */
    void                * mTablePhyHandle;  /**< Table Physical Handle */
    ellDictHandle       * mIndexHandle;     /**< Index Handle */
    void                * mIndexPhyHandle;  /**< Index Physical Handle */

    qlvRelationName     * mRelationName;    /**< Table Name */
    stlChar             * mDumpOption;      /**< Dump Option */
    qlvRefExprList      * mRefColumnList;   /**< Column Reference List */
    qlvRefExprList      * mOuterColumnList; /**< Outer Column List */
    qlvInitExpression   * mRefRowIdColumn;  /**< RowId Column Reference */    
};

/**
 * @brief Sub Table Node
 */

struct qlvInitSubTableNode
{
    qlvNodeType           mType;            /**< Node Type */
    
    stlBool               mIsView;          /**< View or Subquery */

    stlBool               mHasHandle;       /**< Viewed Table or Inline View */
    ellDictHandle         mSchemaHandle;    /**< Schema Handle of the view */
    ellDictHandle         mViewHandle;      /**< Viewed Table 에만 유효함 */
    
    qlvInitNode         * mQueryNode;       /**< Query Expression */
    qlvRelationName     * mRelationName;    /**< Table Name */
    qlvRefExprList      * mRefColumnList;   /**< Column Reference List */
    qlvRefExprList      * mOuterColumnList; /**< Outer Column List */

    stlInt32              mColumnCount;     /**< Column Count */
    qlvInitTarget       * mColumns;         /**< Column List : array */

    qlvInitExpression   * mFilterExpr;     /**< Filter Expression */
};


/**
 * @brief Join Specification
 */

struct qlvInitJoinSpec
{
    qlvInitExpression   * mJoinCondition;
    qlvNamedColumnList  * mNamedColumnList;
};


/**
 * @brief Join Table Node
 */

struct qlvInitJoinTableNode
{
    qlvNodeType           mType;            /**< Node Type */
    qlvJoinType           mJoinType;        /**< Join Type */
    qlvInitNode         * mLeftTableNode;   /**< Left Table Node */
    qlvInitNode         * mRightTableNode;  /**< Right Table Node */
    qlvInitJoinSpec     * mJoinSpec;        /**< Join Specification */
    qlvRefExprList      * mRefColumnList;   /**< Column Reference List */
    qlvRefExprList      * mOuterColumnList; /**< Outer Column List */
};



/**
 * @brief Instant Node
 * instant column description
 */
struct qlvInstantColDesc
{
    stlInt32                mIdx;
 
    stlBool                 mIsSortKey;         /**< Sort Key Column 여부 */
    stlUInt8                mKeyColFlags;       /**< Key Column Flags */

    dtlDataType             mType;              /**< Data Type */
    stlInt64                mArgNum1;           /**< 첫번째 숫자 (TYPE 마다 용도가 다름) */
    stlInt64                mArgNum2;           /**< 두번째 숫자 (TYPE 마다 용도가 다름) */
    dtlStringLengthUnit     mStringLengthUnit;  /**< only for STRING 계열 Type */
    dtlIntervalIndicator    mIntervalIndicator; /**< only for INTERVAL Type */

    qlvInitExpression     * mExpr;
};

 
/**
 * @brief Instant Validation Node
 * @remark Code Optimization 단계에서 생성
 */
struct qlvInitInstantNode
{
    /* init에서 구성하는 정보 : FLAT/SORT/HASH/GROUP */
    qlvNodeType           mType;              /**< Node Type */
    
    qlvInitNode         * mTableNode;         /**< Table Node :  candidate plan 이후에 설정된다. */
    qlvRelationName     * mRelationName;      /**< Table Name */

    /* add column에서 구성하는 정보 */
    qlvRefExprList      * mKeyColList;        /**< Key Column Expression List : Index Read Column List (SORT/HASH/GROUP) */
    qlvRefExprList      * mRecColList;        /**< Record Expression List : Table Read Column List (FLAT/SORT/HASH/GROUP) */
    qlvRefExprList      * mReadColList;       /**< Read Column List (FLAT/SORT/HASH/GROUP) */
    stlUInt16             mKeyColCnt;         /**< Key Column 개수 */
    stlUInt16             mRecColCnt;         /**< Record Column 개수 */
    stlUInt16             mReadColCnt;        /**< Read Column 개수 */

    qlvRefExprList      * mOuterColumnList;   /**< Outer Column List */

    /* Sort Key 관련 */
    stlUInt8            * mKeyFlags;          /**< Key Flags (SORT/GROUP) */
};


/**
 * @brief Relation Name
 */
struct qlvRelationName
{
    stlChar     * mCatalog;     /**< Catalog Name */
    stlChar     * mSchema;      /**< Schema Name */
    stlChar     * mTable;       /**< Table Name or Alias */
};


/****************************************************
 * Misc Nodes
 ****************************************************/ 


/**
 * @brief parameter context
 */
struct qlvInitParamContext
{
    dtlDataType     mDataType;
    stlInt32        mCodeBlockID;
};


/**
 * @brief parameter context list
 */
struct qlvInitParamContextList
{
    stlInt32               mCount;
    qlvInitParamContext  * mParam;
};


/**
 * @brief Data Type Info for Expression
 */
struct qlvInitDataTypeInfo
{
    stlBool              mIsBaseColumn;      /**< Base Column 인지 여부 */
    stlBool              mNullable;          /**< Nullable or Not Nullable */
    stlBool              mUpdatable;         /**< Updatable Column 인지 여부 */
    stlBool              mIsIgnore;          /**< Type 결정시 해당 expr 무시할지 여부 */
    
    dtlDataType          mDataType;          /**< Data Type */
    stlInt64             mArgNum1;           /**< 첫번째 숫자 (TYPE 마다 용도가 다름) */
    stlInt64             mArgNum2;           /**< 두번째 숫자 (TYPE 마다 용도가 다름) */
    dtlStringLengthUnit  mStringLengthUnit;  /**< only for STRING 계열 Type */
    dtlIntervalIndicator mIntervalIndicator; /**< only for INTERVAL Type */
};


/**
 * @brief target for SELECT
 */
struct qlvInitTarget
{
    stlChar              * mDisplayName;     /**< Display Name(alias or target column name) */
    stlInt32               mDisplayPos;      /**< Display Name 의 SQL 상에서의 위치 */
    stlChar              * mAliasName;       /**< Alias Name : nullable */
    qlvInitExpression    * mExpr;            /**< expression */
    
    qlvInitDataTypeInfo    mDataTypeInfo;    /**< Data Type Info */
};


/**
 * @brief expression
 */
struct qlvInitExpression
{
    stlUInt8          mType;            /**< expression type : qlvExprType */
    stlUInt8          mPhraseType;      /**< Expr이 사용된 Phrase 유형 : qlvExprPhraseType */
    stlUInt16         mRelatedFuncId;   /**< related function's ID : knlBuiltInFunc */

    stlUInt32         mIncludeExprCnt;  /**< 현재 expr에 포함된 하위 expr 개수 */

    stlUInt32         mPosition;        /**< stmt내 현재 expr 시작 위치 */
    stlUInt8          mIterationTime;   /**< 평가 반복 수행 옵션 : dtlIterationTime */

    /* list 구성시 자동으로 설정 */
    stlBool           mIsSetOffset;     /**< context offset 설정 여부 */
    stlChar           mPadding1[ 2 ];
    stlInt32          mOffset;          /**< context offset */
    stlChar           mPadding2[ 4 ];

    stlChar         * mColumnName;      /**< column name */

    /**
     * expression : value, column, bind parameter, bind column, function
     */
    union
    {
        qlvInitValue          * mValue;
        qlvInitBindParam      * mBindParam;
        qlvInitColumn         * mColumn;
        qlvInitFunction       * mFunction;
        qlvInitTypeCast       * mTypeCast;
        qlvInitPseudoCol      * mPseudoCol;
        qlvInitConstExpr      * mConstExpr;
        qlvInitRefExpr        * mReference;
        qlvInitSubQuery       * mSubQuery;
        qlvInitInnerColumn    * mInnerColumn;
        qlvInitOuterColumn    * mOuterColumn;
        qlvInitRowIdColumn    * mRowIdColumn;
        qlvInitAggregation    * mAggregation;
        qlvInitCaseExpr       * mCaseExpr;
        qlvInitListFunc       * mListFunc;
        qlvInitListCol        * mListCol;
        qlvInitRowExpr        * mRowExpr;
        void                  * mPseudoArg;

        void                  * mExprValue; /**< for Non-Typed Setting */
    } mExpr;
};


/**
 * @brief reference expression item
 */
struct qlvRefExprItem
{
    qlvInitExpression   * mExprPtr;
    qlvRefExprItem      * mNext;
    stlBool               mIsLeft;      /**< Join Table Node에서 Left Table 참조인지 여부 */
};


/**
 * @brief reference expression list
 */
struct qlvRefExprList
{
    stlInt32              mCount;
    qlvRefExprItem      * mHead;
    qlvRefExprItem      * mTail;
};


/**
 * @brief reference column expression list item
 */
struct qlvRefColListItem
{
    qlvInitNode         * mRelationNode;
    qlvRefExprList      * mColExprList;
    qlvRefColListItem   * mNext;
};


/**
 * @brief reference column expression list
 */
struct qlvRefColExprList
{
    qlvRefColListItem  * mHead;
};

/**
 * @brief reference table column expression item
 */
struct qlvRefTableColItem
{
    stlInt32              mOuterJoinOperCount;
    stlInt32              mColumnCount;
    qlvInitNode         * mRelationNode;
    qlvRefTableColItem  * mNext;
};


/**
 * @brief reference table column expression list
 */
struct qlvRefTableColList
{
    stlInt32              mCount;
    stlInt16              mOuterJoinOperColCount;
    stlInt16              mOuterColumnCount;
    qlvRefTableColItem  * mHead;
    qlvRefTableColItem  * mTail;
};


/**
 * @brief named column item
 */
struct qlvNamedColumnItem
{
    stlChar             * mName;
    stlInt32              mPosition;
    qlvInitExpression   * mLeftExpr;
    qlvInitExpression   * mRightExpr;
    qlvNamedColumnItem  * mNext;
};


/**
 * @brief named column list
 */
struct qlvNamedColumnList
{
    stlInt32              mCount;
    qlvNamedColumnItem  * mHead;
    qlvNamedColumnItem  * mTail;
};


/**
 * @brief reference table item
 */
struct qlvRefTableItem
{
    qlvInitNode         * mTableNode;
    stlInt32              mTargetCount;
    qlvInitTarget       * mTargets;
    qlvRefTableItem     * mNext;
};


/**
 * @brief reference table list
 */
struct qlvRefTableList
{
    stlInt32              mCount;
    qlvRefTableItem     * mHead;
};


/**
 * @brief group by list
 */
struct qlvGroupBy
{
    qlpGroupingType        mGroupByType;     /**< Group By Type */    
    qlvInitInstantNode     mInstant;         /**< Instant Table */
    qlvInitExpression    * mHaving;          /**< Having Clause */
};


/**
 * @brief order by list
 */
struct qlvOrderBy
{
    qlvInitInstantNode    mInstant;          /**< Sort Instant Table */
};


/**
 * @brief order by list
 */
struct qlvAggregation
{
    qlvRefExprList  * mAggrExprList;        /**< Aggregation Expression List 정보 */
};


/**
 * @brief value expression
 */
struct qlvInitValue
{
    qlvValueType        mValueType;          /**< literal value type */
    stlInt32            mInternalBindIdx;    /**< internal bind variable index */

    union
    {
        stlInt64    mInteger;                /**< boolean */
        stlChar   * mString;                 /**< float, string, bitstring */
    } mData;                                 /**< null : mData가 없음 */
};


/**
 * @brief column expression
 */
struct qlvInitColumn
{
    qlvInitNode      * mRelationNode;       /**< releation's node */ 
    ellDictHandle    * mColumnHandle;       /**< releation's column handle */
    stlBool            mIsSetOuterMark;     /**< Outer Join Operator (+) 설정 여부 */
    stlChar            mPadding[7];
};


/**
 * @brief outer column expression
 */
struct qlvInitOuterColumn
{
    qlvInitNode         * mRelationNode;    /**< releation's node */ 
    qlvInitExpression   * mOrgExpr;         /**< 원본 expression */
};


/**
 * @brief Inner Column
 */
struct qlvInitInnerColumn
{
    qlvInitNode         * mRelationNode;    /**< releation's node */ 
    stlInt32            * mIdx;             /**< Subquery의 Target Index(base 0), JoinTable의 Index */
    qlvInitExpression   * mOrgExpr;         /**< 원본 expression */
    stlBool               mIsSetOuterMark;  /**< Outer Join Operator (+) 설정 여부 */
    stlChar               mPadding[7];

};

/**
 * @brief RowId column expression
 */
struct qlvInitRowIdColumn
{
    qlvInitNode      * mRelationNode;       /**< releation's node */ 
    stlBool            mIsSetOuterMark;     /**< Outer Join Operator (+) 설정 여부 */
    stlChar            mPadding[7];
};


/**
 * @brief bind parameter expression
 */
struct qlvInitBindParam
{
    knlBindType         mBindType;            /**< bind type */
    stlInt32            mBindParamIdx;        /**< bind parameter index */
    stlChar           * mHostName;            /**< parameter name */
    stlChar           * mIndicatorName;       /**< indicator name */
};


/**
 * @brief function expression
 */
struct qlvInitFunction
{
    knlBuiltInFunc        mFuncId;         /**< function ID */
    stlInt32              mArgCount;       /**< function argument count */
    qlvInitExpression  ** mArgs;           /**< function argument list : array */
};


/**
 * @brief pseudo column expression
 */
struct qlvInitPseudoCol
{
    knlPseudoCol          mPseudoId;       /**< pseudo column ID */
    stlInt32              mArgCount;       /**< pseudo column argument count */
    ellDictHandle       * mSeqDictHandle;  /**< sequence dictionary handle */
    ellPseudoArg        * mArgs;           /**< pseudo column argument list : array */

    /* list 구성시 자동으로 설정 */
    stlInt32              mPseudoOffset;   /**< offset of pseudo column list */
};


/**
 * @brief aggregation expression
 */
struct qlvInitAggregation
{
    knlBuiltInAggrFunc    mAggrId;         /**< aggregation ID */
    stlInt32              mArgCount;       /**< aggregation argument count */
    qlvInitExpression  ** mArgs;           /**< aggregation argument list : array */
    stlBool               mIsDistinct;     /**< distinct 여부 */
    stlBool               mHasNestedAggr;  /**< nested aggregation을 포함하고 있는지 여부 */
    qlvInitExpression   * mFilter;         /**< filter */
};

/**
 * @brief case expression
 */
struct qlvInitCaseExpr
{
    knlBuiltInFunc        mFuncId;         /**< function ID */    
    stlInt32              mCount;          /**< when clause count  */
    qlvInitExpression  ** mWhenArr;        /**< when expr */
    qlvInitExpression  ** mThenArr;        /**< then expr */
    qlvInitExpression   * mDefResult;      /**< default result */
};

/**
 * @brief list function
 */
struct qlvInitListFunc
{
    knlBuiltInFunc        mFuncId;            /**< function ID */
    qlvListFuncType       mListFuncType;      /**< List Function Type : NESTED/CORRELATED */ 
    stlInt32              mArgCount;          /**< List column */
    stlChar               mBlank[4];
    qlvInitExpression  ** mArgs;              /**< Argument list */
};


/**
 * @brief list column
 */
struct qlvInitListCol
{
    dtlListPredicate        mPredicate;      /**< Predicate */
    stlInt32                mArgCount;       /**< List column */
    qlvInitExpression    ** mArgs;           /**< Argument list */
    knlListEntry         ** mEntry;          /**< List Stack Entry */
};

/**
 * @brief row expr
 */
struct qlvInitRowExpr
{
    stlInt32                   mArgCount;          /**< List column */
    stlBool                    mIsRowSubQuery;     /**< Row SubQuery 여부 */
    stlChar                    mBlank[3];
    qlvInitExpression       ** mArgs;              /**< Argument list */
    knlListEntry             * mEntry;             /**< List Stack Entry */
    qlvInitExpression        * mSubQueryExpr;      /**< Relation Sub Query Expression (Optimize에서 구축됨) */
};

/**
 * @brief type definition expression
 */
struct qlvInitTypeDef
{
    dtlDataType          mDBType;            /**< DB Type */
    stlChar            * mUserTypeName;      /**< 사용자가 기술한 DB Type 이름 */
    stlInt64             mArgNum1;           /**< 첫번째 숫자 (TYPE 마다 용도가 다름) */
    stlInt64             mArgNum2;           /**< 두번째 숫자 (TYPE 마다 용도가 다름) */
    dtlStringLengthUnit  mStringLengthUnit;  /**< only for STRING 계열 Type */
    dtlIntervalIndicator mIntervalIndicator; /**< only for INTERVAL Type */
    stlInt64             mNumericRadix;      /**< only for NUMERIC Type */
};


/**
 * @brief cast expression
 */
struct qlvInitTypeCast
{
    qlvInitExpression  ** mArgs;          /**< cast function argument list : array */
    qlvInitTypeDef        mTypeDef;       /**< Target Type */
};


/**
 * @brief constant expression result
 */
struct qlvInitConstExpr
{
    qlvInitExpression   * mOrgExpr;      /**< 원본 expression */
};


/**
 * @brief reference expression
 */
struct qlvInitRefExpr
{
    qlvInitExpression   * mOrgExpr;      /**< 원본 expression */
};


/**
 * @brief sub-query expression
 * @remark qlvInitSubQuery 구조의 생성은 validation 과정에서 이루어지나,
 *    <BR> 정보 구축은 Code Optimization 단계에서 
 */
struct qlvInitSubQuery
{
    /* validation 시에 구축될 정보 */
    qlvInitNode          * mInitNode;         /**< 관련 Validation Node Pointer */
    qlvInitTarget        * mTargets;          /**< Target List : array */
    stlInt32               mTargetCount;      /**< Target Count */

    /* optimization 시에 구축될 정보 */
    stlInt8                mRefIdx;           /**< Sub Query의 Target에서 참조하는 Target Idx (default: 0) */
    stlInt8                mPadding[3];

    knlBuiltInFunc         mRelatedFuncId;    /**< related function's ID : knlBuiltInFunc */
    
    qlvRefExprList       * mTargetList;       /**< Target List : Reference Column List와 1:1 매칭 */
    qlvRefExprList       * mRefColList;       /**< Reference Column List : Reference Expression List */

    qllOptimizationNode  * mOptNode;          /**< 관련 Optimization Node Pointer */

    /* cost opt시에 구축할 정보 */ 
    qlvSubQueryType        mSubQueryType;     /**< Sub-Query Type : N/A/J/AJ */ 
    qlvSubQueryCategory    mCategory;         /**< Sub-Query Category : 
                                               * ( default category = QLV_SUB_QUERY_TYPE_TABLE )
                                               *
                                               *  < Sub-Query category 변경이 가능한 경우들 >
                                               * QLV_SUB_QUERY_TYPE_TABLE  =>  QLV_SUB_QUERY_TYPE_ROW
                                               * QLV_SUB_QUERY_TYPE_TABLE  =>  QLV_SUB_QUERY_TYPE_SCALAR
                                               * QLV_SUB_QUERY_TYPE_ROW    =>  QLV_SUB_QUERY_TYPE_SCALAR
                                               */
    void                 * mCandPlan;         /**< Candidate Plan Node Pointer (Optimize에서 구축됨) */
};


/****************************************************
 * SELECT STATEMENT Nodes
 ****************************************************/ 

/**
 * @brief Query 와 DML 의 공통 Init Plan
 */
struct qlvInitPlan
{
    /********************************************
     * 공통 정보 
     ********************************************/ 

    ellObjectList       * mValidationObjList;   /**< 접근하는 SQL-Object 목록 */

    stlInt32       * mBindNodePos;   /**< Bind Node 의 SQL 문 상에서의 위치 */
    knlBindType    * mBindIOType;    /**< SQL 문에 포함된 Bind 의 I/O type 정보 array (IN or OUT or INOUT) */
    stlInt32         mBindCount;     /**< SQL 문에 포함된 Bind 개수 */
    
    stlInt32         mInternalBindCount;   /**< Plan 구성시 생성된 Internal Bind Parameter 개수 */
    stlBool          mHasHintError;        /**< Hint Error 포함 여부 */

    /********************************************
     * Query 전용 정보 
     ********************************************/ 

    ellCursorProperty       mResultCursorProperty;   /**< Result Set Cursor Property */
    
};

/**
 * @brief Select Statement Init Plan : QLL_STMT_SELECT_TYPE, QLL_STMT_SELECT_INTO_TYPE
 */
struct qlvInitSelect
{
    qlvInitPlan           mInitPlan;           /**< Init Plan 공통 정보 */
    
    qlvInitNode         * mQueryNode;          /**< Query Node */

    /* Expression List */
    qlvInitStmtExprList * mStmtExprList;       /**< Statement 단위 Expression 정보 */

    /* for INTO clause */
    stlInt32              mIntoTargetCnt;      /**< INTO clause target count */
    qlvInitExpression   * mIntoTargetArray;    /**< INTO clause target array */

    /*********************************************
     * Result Set 생성을 위한 Cursor 정보
     *********************************************/
    
    /*
     * for Sensitivity
     */

    qlvRefTableList     * mScanTableList;      /**< scan table list */
    
    /*
     * for Updatability
     */
    
    qlvSerialAction       mSerialAction;       /**< Version Conflict Resolution Action */
    qlpUpdatableMode      mUpdatableMode;      /**< Updatable Mode */
    qllCursorLockWait     mLockWaitMode;       /**< lock wait mode */
    stlInt64              mLockTimeInterval;   /**< lock time interval */
    qlvRefTableList     * mLockTableList;      /**< lock table list */

    /*******************************************************
     * Target Descriptor 관련
     *******************************************************/
    
    qllTarget           * mTargetInfo;         /**< Target Info */
};


/****************************************************
 * INSERT STATEMENT Nodes
 ****************************************************/ 

/**
 * @brief Insert Target Column
 */
struct qlvInitInsertTarget
{
    stlInt32            * mTargetColumnIdx;    /**< Target Column 의 Column Idx */
    stlBool             * mIsDeclareColumn;    /**< Target Column 명시 여부 */
    stlBool             * mIsRowDefault;       /**< 모든 Values 에 Default 값 설정 여부 */
    qlvInitExpression  ** mRowDefaultExpr;     /**< 모든 Values 를 위한 Default Value */
};

/**
 * @brief Single Value List
 */
struct qlvInitSingleRow
{
    qlvInitExpression  ** mValueExpr;          /**< Source Value Expression */
    
    struct qlvInitSingleRow * mNext;
};


/**
 * @brief Insert 에 영향을 받는 객체 정보
 */
struct qlvInitInsertRelObject
{
    /*
     * PRIMARY KEY constraint
     */
    
    stlInt32        mPrimaryKeyCnt;            /**< Primary Key 개수 */
    ellDictHandle * mPrimaryKeyHandle;         /**< Key Handle Array */
    void         ** mPrimaryKeyIndexPhyHandle; /**< Physical Index Handle Array */

    /*
     * UNIQUE constraint
     */
    
    stlInt32        mUniqueKeyCnt;             /**< Unique Key 개수 */
    ellDictHandle * mUniqueKeyHandle;          /**< Key Handle Array */
    void         ** mUniqueKeyIndexPhyHandle;  /**< Physical Index Handle Array */

    /*
     * FOREIGN KEY constraint
     */
    
    stlInt32        mForeignKeyCnt;            /**< Foreign Key 개수 */
    ellDictHandle * mForeignKeyHandle;         /**< Key Handle Array */
    void         ** mForeignKeyIndexPhyHandle; /**< Physical Index Handle Array */

    /*
     * @todo CHILD FOREIGN KEY constraint
     */
    
    
    /*
     * Unique Index
     */
    
    stlInt32        mUniqueIndexCnt;           /**< Unique Index 개수 */
    ellDictHandle * mUniqueIndexHandle;        /**< Index Handle Array */
    void         ** mUniqueIndexPhyHandle;     /**< Physical Index Handle Array */
    
    /*
     * Non-Unique Index
     */
    
    stlInt32        mNonUniqueIndexCnt;        /**< Non-Unique Index 개수 */
    ellDictHandle * mNonUniqueIndexHandle;     /**< Index Handle Array */
    void         ** mNonUniqueIndexPhyHandle;  /**< Physical Index Handle Array */

    /*
     * NOT NULL
     */
    
    stlInt32        mCheckNotNullCnt;          /**< Check Not Null 개수 */
    ellDictHandle * mCheckNotNullHandle;       /**< Check Handle Array */
    
    /*
     * CHECK
     */
    
    stlInt32        mCheckClauseCnt;           /**< Check Clause 개수 */
    ellDictHandle * mCheckClauseHandle;        /**< Check Handle Array */

    /*
     * Identity Column
     * - not null 속성을 가지나 constraint 가 아니다.
     */

    stlBool         mHasIdentity;              
    ellDictHandle   mIdentityColumnHandle;
};


/**
 * @brief INSERT VALUES Init Plan
 */
struct qlvInitInsertValues
{
    qlvInitPlan            mInitPlan;           /**< Init Plan 공통 정보 */
    
    /*
     * Write Table 정보
     */
    
    ellDictHandle          mTableHandle;        /**< Table Handle */
    void                 * mTablePhyHandle;     /**< Table Physical Handle */
    stlInt32               mMultiValueCnt;      /**< Multi Value Count */

    /*
     * Expression List
     */
    
    qlvInitStmtExprList  * mStmtExprList;       /**< Statement 단위 Expression 정보 */
    qlvInitExprList      * mQueryExprList;      /**< Query 단위 Expression 정보 */

    /*
     * Table Node 정보
     */

    qlvInitNode          * mWriteTableNode;     /**< Write Table Node */

    /*
     * Column Count 만큼 Array 로 관리됨
     */

    stlInt32               mTotalColumnCnt;     /**< Total Column Count */
    ellDictHandle        * mColumnHandle;       /**< Column Handle */
    qlvRefExprList       * mInsertColExprList;  /**< Insert 할 Column Expression List */

    /*
     * Insert Target
     */
    
    qlvInitInsertTarget    mInsertTarget;       /**< Insert Target Column */
    
    /*
     * Multi Values list
     */
    
    qlvInitSingleRow     * mRowList;            /**< Multi Values */

    /*******************************************************
     * Insert 에 영향을 받는 Object
     *******************************************************/

    stlBool                mHasCheck;
    stlBool                mHasIndex;
    
    qlvInitInsertRelObject mRelObject;
    
    /*******************************************************
     * RETURN ... INTO 관련
     *******************************************************/

    stlInt32               mReturnTargetCnt;    /**< RETURN clause target count */
    qlvInitTarget        * mReturnTargetArray;  /**< RETURN clause target array */

    qlvInitExpression    * mIntoTargetArray;    /**< INTO clause target array */
};


/**
 * @brief INSERT SELECT Init Plan
 */
struct qlvInitInsertSelect
{
    qlvInitPlan           mInitPlan;           /**< Init Plan 공통 정보 */
    
    /*
     * Write Table 정보
     */
    
    ellDictHandle          mTableHandle;        /**< Table Handle */
    void                 * mTablePhyHandle;     /**< Table Physical Handle */

    /*
     * Expression List
     */
    
    qlvInitStmtExprList  * mStmtExprList;       /**< Statement 단위 Expression 정보 */
    qlvInitExprList      * mQueryExprList;      /**< Query 단위 Expression 정보 */

    /*
     * Write Table 정보
     */

    qlvInitNode          * mWriteTableNode;     /**< Write Table Node */

    /*
     * Column Count 만큼 Array 로 관리됨
     */
    
    stlInt32               mTotalColumnCnt;     /**< Total Column Count */
    ellDictHandle        * mColumnHandle;       /**< Column Handle */
    qlvRefExprList       * mInsertColExprList;  /**< Insert 할 Column Expression List */
    
    /*
     * Insert Target
     */
    
    qlvInitInsertTarget    mInsertTarget;       /**< Insert Target Column */

    /*
     * Single Row
     */

    qlvInitSingleRow     * mRowList;            /**< Source Expression */

    /*
     * Subquery
     */

    qlvInitNode          * mSubQueryNode;       /**< SELECT subquery */

    /*******************************************************
     * Insert 에 영향을 받는 Object
     *******************************************************/

    stlBool                mHasCheck;
    stlBool                mHasIndex;
    
    qlvInitInsertRelObject mRelObject;

    /*******************************************************
     * RETURN ... INTO 관련
     *******************************************************/

    stlInt32               mReturnTargetCnt;    /**< RETURN clause target count */
    qlvInitTarget        * mReturnTargetArray;  /**< RETURN clause target array */

    qlvInitExpression    * mIntoTargetArray;    /**< INTO clause target array */
};


/**
 * @brief INSERT RETURNING (INTO or Query) Init Plan
 */
struct qlvInitInsertReturnInto
{
    qlvInitPlan           mInitPlan;           /**< Init Plan 공통 정보 */
    
    /*******************************
     * BASE insert statement
     *******************************/
    
    stlBool               mIsInsertValues;     /**< INSERT VALUES or INSERT SELECT */

    qlvInitInsertValues * mInitInsertValues;   /**< INSERT VALUES init plan */
    qlvInitInsertSelect * mInitInsertSelect;   /**< INSERT SELECT init plan */

    ellDictHandle       * mTableHandle;    

    /*******************************************************
     * Target Descriptor 관련
     *******************************************************/
    
    qllTarget           * mTargetInfo;         /**< Target Info */
};


/****************************************************
 * DELETE STATEMENT Nodes
 ****************************************************/ 

/**
 * @brief Delete 에 영향을 받는 객체 정보
 */
struct qlvInitDeleteRelObject
{
    /*
     * PRIMARY KEY constraint
     */
    
    stlInt32        mPrimaryKeyCnt;            /**< Primary Key 개수 */
    ellDictHandle * mPrimaryKeyHandle;         /**< Key Handle Array */
    void         ** mPrimaryKeyIndexPhyHandle; /**< Physical Index Handle Array */

    /*
     * UNIQUE constraint
     */
    
    stlInt32        mUniqueKeyCnt;             /**< Unique Key 개수 */
    ellDictHandle * mUniqueKeyHandle;          /**< Key Handle Array */
    void         ** mUniqueKeyIndexPhyHandle;  /**< Physical Index Handle Array */

    /*
     * FOREIGN KEY constraint
     */
    
    stlInt32        mForeignKeyCnt;            /**< Foreign Key 개수 */
    ellDictHandle * mForeignKeyHandle;         /**< Key Handle Array */
    void         ** mForeignKeyIndexPhyHandle; /**< Physical Index Handle Array */

    /*
     * CHILD FOREIGN KEY constraint
     */

    stlInt32        mChildForeignKeyCnt;       /**< Child Foreign Key 개수 */
    ellDictHandle * mChildForeignKeyHandle;    /**< Child Foreign Key Handle Array */

    /*
     * Unique Index
     */
    
    stlInt32        mUniqueIndexCnt;           /**< Unique Index 개수 */
    ellDictHandle * mUniqueIndexHandle;        /**< Index Handle Array */
    void         ** mUniqueIndexPhyHandle;     /**< Physical Index Handle Array */
    
    /*
     * Non-Unique Index
     */
    
    stlInt32        mNonUniqueIndexCnt;        /**< Non-Unique Index 개수 */
    ellDictHandle * mNonUniqueIndexHandle;     /**< Index Handle Array */
    void         ** mNonUniqueIndexPhyHandle;  /**< Physical Index Handle Array */

    /*
     * NOT NULL constraint
     * - DEFERRED 로 동작할 경우 해당 컬럼이 NULL 값을 가지고 있는지 확인해야 함
     */
    
    stlInt32        mDeferNotNullCnt;     /**< Deferrable Not Null 개수 */
    ellDictHandle * mDeferNotNullHandle;  /**< Deferrable Not Null Handle Array */
    
    /*
     * @todo CHECK constraint
     * - DEFERRED 로 동작할 경우 해당 컬럼이 제약조건을 위배하고 있었는지 확인해야 함.
     */
    
};


/**
 * @brief DELETE Init Plan 
 */
struct qlvInitDelete
{
    qlvInitPlan            mInitPlan;           /**< Init Plan 공통 정보 */
    
    qlvSerialAction        mSerialAction;       /**< Version Conflict Resolution Action */

    /*
     * Hint 정보
     */

    qlvHintInfo          * mHintInfo;           /**< Hint Info */
    
    /*
     * Write Table 정보
     */
    
    ellDictHandle          mTableHandle;        /**< Table Handle */
    void                 * mTablePhyHandle;     /**< Table Physical Handle */
    
    /*
     * Expression List
     */
    
    qlvInitStmtExprList  * mStmtExprList;       /**< Statement 단위 Expression 정보 */
    qlvInitExprList      * mQueryExprList;      /**< Query 단위 Expression 정보 */

    /*
     * Read Table 정보
     */

    qlvInitNode          * mReadTableNode;      /**< Table Node for Reading */

    /*
     * WHERE expression
     */
    
    qlvInitExpression    * mWhereExpr;          /**< Where Expression */

    /*
     * result offset .. limit
     */
    
    qlvInitExpression    * mResultSkip;
    qlvInitExpression    * mResultLimit;
    stlInt64               mSkipCnt;            /**< Result Skip Count */
    stlInt64               mFetchCnt;           /**< Result Fetch Count */


    /*******************************************************
     * Delete 에 영향을 받는 Object
     *******************************************************/

    stlBool                mHasIndex;
    stlBool                mHasCheck;
    
    qlvInitDeleteRelObject mRelObject;
    
    /*******************************************************
     * RETURN ... INTO 관련
     *******************************************************/

    stlInt32               mReturnTargetCnt;    /**< RETURN clause target count */
    qlvInitTarget        * mReturnTargetArray;  /**< RETURN clause target array */

    qlvInitExpression    * mIntoTargetArray;    /**< INTO clause target array */

    /*******************************************************
     * WHERE CURRENT OF 관련 
     *******************************************************/

    stlChar              * mCursorName;               /**< Cursor Name */
    stlInt64               mCursorProcID;             /**< Origin Procedure ID */
    
    /*******************************************************
     * Target Descriptor 관련
     *******************************************************/
    
    qllTarget            * mTargetInfo;         /**< Target Info */
};


/****************************************************
 * UPDATE STATEMENT Nodes
 ****************************************************/ 

/**
 * @brief Update 에 영향을 받는 객체 정보
 */
struct qlvInitUpdateRelObject
{
    /*
     * PRIMARY KEY constraint
     */
    
    stlInt32        mTotalPrimaryKeyCnt;            /**< Primary Key 개수 */
    ellDictHandle * mTotalPrimaryKeyHandle;         /**< Key Handle Array */

    stlInt32        mAffectPrimaryKeyCnt;            /**< 갱신 컬럼의 영향을 받는 Primary Key 개수 */
    ellDictHandle * mAffectPrimaryKeyHandle;         /**< Key Handle Array */
    void         ** mAffectPrimaryKeyIndexPhyHandle; /**< Physical Index Handle Array */

    /*
     * UNIQUE KEY constraint
     */
    
    stlInt32        mTotalUniqueKeyCnt;             /**< Unique Key 개수 */
    ellDictHandle * mTotalUniqueKeyHandle;          /**< Key Handle Array */

    stlInt32        mAffectUniqueKeyCnt;            /**< 갱신 컬럼의 영향을 받는 Unique Key 개수 */
    ellDictHandle * mAffectUniqueKeyHandle;         /**< Key Handle Array */
    void         ** mAffectUniqueKeyIndexPhyHandle; /**< Physical Index Handle Array */
    
    /*
     * @todo 
     * FOREIGN KEY constraint
     */

    stlInt32        mTotalForeignKeyCnt;            /**< Foreign Key 개수 */
    ellDictHandle * mTotalForeignKeyHandle;         /**< Key Handle Array */

    stlInt32        mAffectForeignKeyCnt;      /**< 갱신 컬럼의 영향을 받는 Foreign Key 개수 */
    
    /*
     * @todo 
     * CHILD FOREIGN KEY constraint
     */

    stlInt32        mTotalChildForeignKeyCnt;       /**< Child Foreign Key 개수 */
    ellDictHandle * mTotalChildForeignKeyHandle;    /**< Child Foreign Key Handle Array */

    stlInt32        mAffectChildForeignKeyCnt;    /**< 갱신 컬럼의 영향을 받는 Child Foreign Key 개수 */
    
    /*
     * Unique Index
     */
    
    stlInt32        mTotalUniqueIndexCnt;           /**< Unique Index 개수 */
    ellDictHandle * mTotalUniqueIndexHandle;        /**< Index Handle Array */

    stlInt32        mAffectUniqueIndexCnt;        /**< 갱신 컬럼의 영향을 받는 Unique Index 개수 */
    ellDictHandle * mAffectUniqueIndexHandle;     /**< Affetcted Index Handle Array */
    void         ** mAffectUniqueIndexPhyHandle;  /**< Physical Index Handle Array */
    
    /*
     * Non-Unique Index
     */
    
    stlInt32        mTotalNonUniqueIndexCnt;        /**< Non-Unique Index 개수 */
    ellDictHandle * mTotalNonUniqueIndexHandle;     /**< Index Handle Array */
    
    stlInt32        mAffectNonUniqueIndexCnt;        /**< 갱신 컬럼의 영향을 받는 Non Unique Index 개수 */
    ellDictHandle * mAffectNonUniqueIndexHandle;     /**< Affected Index Handle Array */
    void         ** mAffectNonUniqueIndexPhyHandle;  /**< Physical Index Handle Array */

    /*
     * NOT NULL constraint
     */
    
    stlInt32        mTotalCheckNotNullCnt;          /**< NOT NULL 개수 */
    ellDictHandle * mTotalCheckNotNullHandle;       /**< NOT NULL Handle Array */
    
    stlInt32        mAffectCheckNotNullCnt;     /**< 갱신 컬럼의 영향을 받는 NOT NULL 개수 */
    ellDictHandle * mAffectCheckNotNullHandle;  /**< Affected NOT NULL Handle Array */
    stlBool       * mAffectCheckNotNullDefer;   /**< Deferrable NOT NULL 인지 여부 */

    /*
     * @todo  
     * CHECK constraint
     */
    
    stlInt32        mTotalCheckClauseCnt;           /**< CHECK 개수 */
    ellDictHandle * mTotalCheckClauseHandle;        /**< CHECK Handle Array */

    stlInt32        mAffectCheckClauseCnt;     /**< 갱신 컬럼의 영향을 받는 CHECK 개수 */
    
    /*
     * Identity Column
     * - not null 속성을 가지나 constraint 가 아니다.
     */

    stlBool         mHasAffectIdentity;       /**< identity column 이 갱신 대상이 되는지 여부 */
    ellDictHandle   mIdentityColumnHandle;
};


/**
 * @brief UPDATE Init Plan 
 */
struct qlvInitUpdate
{
    qlvInitPlan           mInitPlan;            /**< Init Plan 공통 정보 */
    
    qlvSerialAction       mSerialAction;        /**< Version Conflict Resolution Action */

    /*
     * Hint 정보
     */

    qlvHintInfo          * mHintInfo;           /**< Hint Info */

    /*
     * Write Table 정보
     */
    
    ellDictHandle          mTableHandle;        /**< Table Handle */
    void                 * mTablePhyHandle;     /**< Table Physical Handle */

    /*
     * Expression List
     */
    
    qlvInitStmtExprList  * mStmtExprList;       /**< Statement 단위 Expression 정보 */
    qlvInitExprList      * mQueryExprList;      /**< Query 단위 Expression 정보 */

    /*
     * Read Table 정보
     */

    qlvInitNode          * mReadTableNode;      /**< Table Node for Reading */

    /*
     * Write Table 정보
     */

    qlvInitNode          * mWriteTableNode;     /**< Table Node for Writing  */
    
    /*
     * SET Column Count 만큼 Array 로 관리됨
     */
    
    stlInt32               mSetColumnCnt;       /**< SET Column Count */
    ellDictHandle        * mSetColumnHandle;    /**< SET Column Handle */
    qlvInitExpression   ** mSetExpr;            /**< SET Column Value Expression Array */
    qlvRefExprList       * mSetColExprList;     /**< SET Column Expression List */

    /*
     * Table 의 전체 Column Count 만큼 Array 로 관리됨
     */

    stlInt32               mTotalColumnCnt;     /**< Total Column Count */
    stlInt32             * mSetCodeStackIdx;    /**< SetCodeStack의 idx번호 (Used Column 여부 포함) */
    
    /*
     * WHERE expression
     */
    
    qlvInitExpression    * mWhereExpr;          /**< Where Expression */

    /*
     * result offset .. limit
     */
    
    qlvInitExpression    * mResultSkip;         /**< OFFSET clause */
    qlvInitExpression    * mResultLimit;        /**< LIMIT clause */
    stlInt64               mSkipCnt;            /**< Result Skip Count */
    stlInt64               mFetchCnt;           /**< Result Fetch Count */

    
    /*******************************************************
     * Update 에 영향을 받는 Object
     *******************************************************/

    stlBool                mHasCheck;
    stlBool                mHasIndex;
    
    qlvInitUpdateRelObject mRelObject;
    
    /*******************************************************
     * RETURN ... INTO 관련
     *******************************************************/

    stlBool                mIsReturnNew;

    stlInt32               mReturnTargetCnt;    /**< RETURN clause target count */
    qlvInitTarget        * mReturnTargetArray;  /**< RETURN clause target array */

    qlvInitExpression    * mIntoTargetArray;    /**< INTO clause target array */
    qlvRefExprList       * mShareReadColList;   /**< Share Read Column Expression List For RETURN clause */
    qlvRefExprList       * mShareWriteColList;  /**< Share Write Column Expression List For RETURN clause */

    /*******************************************************
     * WHERE CURRENT OF 관련 
     *******************************************************/

    stlChar              * mCursorName;               /**< Cursor Name */
    stlInt64               mCursorProcID;             /**< Origin Procedure ID */

    /*******************************************************
     * Target Descriptor 관련
     *******************************************************/
    
    qllTarget            * mTargetInfo;         /**< Target Info */
};





/****************************************************
 * Functions
 ****************************************************/ 

/* inline functions */
inline stlStatus qlvQueryNodeValidation( qlvStmtInfo          * aStmtInfo,
                                         stlBool                aCalledFromSubQuery,
                                         ellObjectList        * aValidationObjList,
                                         qlvRefTableList      * aRefTableList,
                                         qllNode              * aParseQueryNode,
                                         qlvInitNode         ** aInitQueryNode,
                                         qllEnv               * aEnv );

inline stlStatus qlvTableNodeValidation( qlvStmtInfo          * aStmtInfo,
                                         ellObjectList        * aValidationObjList,
                                         qlvRefTableList      * aRefTableList,
                                         qllNode              * aParseTableNode,
                                         qlvInitNode         ** aTableNode,
                                         stlInt32             * aTableCount,
                                         qllEnv               * aEnv );

inline stlStatus qlvMakeRelationName( knlRegionMem      * aRegionMem,
                                      stlChar           * aCatalog,
                                      stlChar           * aSchema,
                                      stlChar           * aTable,
                                      qlpAlias          * aAlias,
                                      qlvRelationName  ** aRelationName,
                                      qllEnv            * aEnv );


inline stlStatus qlvAddInnerColumnExpr( qlvInitExpression  ** aInitExpr,
                                        qlvInitNode         * aRelationNode,
                                        stlInt32              aIdx,
                                        knlRegionMem        * aRegionMem,
                                        qllEnv              * aEnv );

inline stlStatus qlvAddReferenceExpr( qlvInitExpression  ** aInitExpr,
                                      knlRegionMem        * aRegionMem,
                                      qllEnv              * aEnv );

inline stlStatus qlvCreateRefTableList( knlRegionMem       * aRegionMem,
                                        qlvRefTableList    * aParentRefTableList,
                                        qlvRefTableList   ** aRefTableList,
                                        qllEnv             * aEnv );

inline stlStatus qlvSetNodeToRefTableItem( qlvRefTableList   * aRefTableList,
                                           qlvInitNode       * aTableNode,
                                           qllEnv            * aEnv );

inline stlStatus qlvSetTargetToRefTableItem( qlvRefTableList   * aRefTableList,
                                             stlInt32            aTargetCount,
                                             qlvInitTarget     * aTargets,
                                             qllEnv            * aEnv );


inline stlStatus qlvAddRefColumnListByColumnName( qlvStmtInfo           * aStmtInfo,
                                                  qlvExprPhraseType       aPhraseType,
                                                  knlRegionMem          * aRegionMem,
                                                  stlChar               * aColumnName,
                                                  stlInt32                aColumnPos,
                                                  qlvInitNode           * aTableNode,
                                                  stlBool                 aNeedCopy,
                                                  stlBool               * aIsUpdatable,
                                                  qlvInitExpression    ** aExpr,
                                                  qllEnv                * aEnv );

inline stlStatus qlvFindOrAddColumnOnBaseTable( qlvStmtInfo             * aStmtInfo,
                                                qlvExprPhraseType         aPhraseType,
                                                knlRegionMem            * aRegionMem,
                                                stlChar                 * aColumnName,
                                                stlInt32                  aColumnPos,
                                                qlvInitBaseTableNode    * aBaseTableNode,
                                                stlBool                   aNeedCopy,
                                                stlBool                 * aExist,
                                                stlBool                 * aIsUpdatable,
                                                qlvRefExprItem         ** aRefColumnItem,
                                                qllEnv                  * aEnv );

inline stlStatus qlvFindOrAddColumnOnSubTable( qlvStmtInfo              * aStmtInfo,
                                               qlvExprPhraseType          aPhraseType,
                                               knlRegionMem             * aRegionMem,
                                               stlChar                  * aColumnName,
                                               stlInt32                   aColumnPos,
                                               qlvInitSubTableNode      * aSubTableNode,
                                               stlBool                    aNeedCopy,
                                               stlBool                  * aExist,
                                               stlBool                  * aIsUpdatable,
                                               qlvRefExprItem          ** aRefColumnItem,
                                               qllEnv                   * aEnv );

inline stlStatus qlvFindOrAddColumnOnJoinTable( qlvStmtInfo              * aStmtInfo,
                                                qlvExprPhraseType          aPhraseType,
                                                knlRegionMem             * aRegionMem,
                                                stlChar                  * aColumnName,
                                                stlInt32                   aColumnPos,
                                                qlvInitJoinTableNode     * aJoinTableNode,
                                                stlBool                    aNeedCopy,
                                                stlBool                  * aExist,
                                                stlBool                  * aIsUpdatable,
                                                qlvRefExprItem          ** aRefColumnItem,
                                                qllEnv                   * aEnv );

inline stlStatus qlvGetJoinNamedColumnCount( qlvInitNode    * aTableNode,
                                             stlInt32       * aCount,
                                             qllEnv         * aEnv );

inline stlStatus qlvMakeFuncAnd2Arg( knlRegionMem         * aRegionMem,
                                     stlInt32               aPosition,
                                     qlvInitExpression    * aLeftExpr,
                                     qlvInitExpression    * aRightExpr,
                                     qlvInitExpression   ** aFuncExpr,
                                     qllEnv               * aEnv );

inline stlStatus qlvMakeFuncAndNArg( knlRegionMem         * aRegionMem,
                                     stlInt32               aPosition,
                                     stlInt32               aExprCnt,
                                     qlvInitExpression   ** aExprArr,
                                     qlvInitExpression   ** aFuncExpr,
                                     qllEnv               * aEnv );

inline stlStatus qlvMakeFuncOrNArg( knlRegionMem         * aRegionMem,
                                    stlInt32               aPosition,
                                    stlInt32               aExprCnt,
                                    qlvInitExpression   ** aExprArr,
                                    qlvInitExpression   ** aFuncExpr,
                                    qllEnv               * aEnv );

inline stlStatus qlvMakeFuncEqual( knlRegionMem         * aRegionMem,
                                   stlInt32               aPosition,
                                   qlvInitExpression    * aLeftExpr,
                                   qlvInitExpression    * aRightExpr,
                                   qlvInitExpression   ** aFuncExpr,
                                   qllEnv               * aEnv );

inline stlStatus qlvMakeFuncAndFromExprList( knlRegionMem         * aRegionMem,
                                             stlInt32               aPosition,
                                             qlvRefExprList       * aAggrExprList,
                                             qlvInitExpression   ** aFuncExpr,
                                             qllEnv               * aEnv );

inline stlBool qlvHasANSIJoin( qlvInitNode  * aTableNode );

inline stlStatus qlvValidateOuterJoinOperator( qlvStmtInfo          * aStmtInfo,
                                               qlvInitExpression    * aWhereExpr,
                                               qlvInitNode          * aTableNode,
                                               qlvInitExpression   ** aOutWhereExpr,
                                               qlvInitNode         ** aOutTableNode,
                                               qllEnv               * aEnv );

inline stlStatus qlvGetRefTableColList( knlRegionMem        * aRegionMem,
                                        qlvInitExpression   * aExpr,
                                        qlvRefTableColList  * aTableColList,
                                        qllEnv              * aEnv );

inline void qlvGetTableCount( qlvInitNode   * aTableNode,
                              stlInt32      * aTableCount );

inline void qlvSetTableNodeArray( qlvInitNode   * aTableNode,
                                  qlvInitNode  ** aTableNodeArray,
                                  stlInt32      * aCurNodeIdx );

inline stlBool qlvIsExistBaseNodeInTableNode( qlvInitNode   * aBaseTableNode,
                                              qlvInitNode   * aSrcTableNode );

inline stlStatus qlvAdjustFilterByTableNode( knlRegionMem       * aRegionMem,
                                             qlvInitNode        * aTableNode,
                                             qlvInitExpression  * aExpr,
                                             qlvInitExpression ** aOutExpr,
                                             qllEnv             * aEnv );

inline stlStatus qlvAdjustColumnByTableNode( knlRegionMem       * aRegionMem,
                                             qlvInitNode        * aTableNode,
                                             qlvInitExpression  * aExpr,
                                             qlvInitExpression ** aOutExpr,
                                             qllEnv             * aEnv );

inline stlStatus qlvAddColumnInJoinTableNode( knlRegionMem          * aRegionMem,
                                              qlvInitNode           * aTableNode,
                                              qlvInitNode           * aNodeOfExpr,
                                              stlInt32                aColumnPos,
                                              qlvInitExpression     * aExpr,
                                              qlvInitExpression    ** aOutExpr,
                                              qllEnv                * aEnv );

/* query node */
stlStatus qlvValidateQuerySetNode( qlvStmtInfo        * aStmtInfo,
                                   stlBool              aCalledFromSubQuery,
                                   ellObjectList      * aValidationObjList,
                                   qlvRefTableList    * aRefTableList,
                                   qlpQuerySetNode    * aParseQuerySetNode,
                                   qlvInitNode       ** aQueryNode,
                                   qllEnv             * aEnv );

stlStatus qlvValidateQuerySetInfo( qlvStmtInfo          * aStmtInfo,
                                   stlBool                aCalledFromSubQuery,
                                   ellObjectList        * aValidationObjList,
                                   qlvRefTableList      * aRefTableList,
                                   stlBool                aIsRootSet,
                                   stlBool                aIsLeftSet,
                                   qlvInitNode          * aLeftMostQuerySpec,
                                   stlInt32               aSetColumnCount,
                                   qlvInitTarget        * aSetColumns,
                                   qlpQuerySetNode      * aParseQuerySetNode,
                                   qlvInitQuerySetNode  * aQuerySetNode,
                                   qllEnv               * aEnv );


stlStatus qlvValidateQuerySpecNode( qlvStmtInfo         * aStmtInfo,
                                    stlBool               aCalledFromSubQuery,
                                    ellObjectList       * aValidationObjList,
                                    qlvRefTableList     * aRefTableList,
                                    qlpQuerySpecNode    * aParseQuerySpecNode,
                                    qlvInitNode        ** aQueryNode,
                                    qllEnv              * aEnv );

/* table node */

stlStatus qlvValidateLeafTableNode( qlvStmtInfo        * aStmtInfo,
                                    ellObjectList      * aValidationObjList,
                                    qlvRefTableList    * aRefTableList,
                                    qlpBaseTableNode   * aParseBaseTableNode,
                                    qlvInitNode       ** aTableNode,
                                    stlInt32           * aTableCount,
                                    qllEnv             * aEnv );

stlStatus qlvValidatePhysicalTable( qlvStmtInfo        * aStmtInfo,
                                    ellObjectList      * aValidationObjList,
                                    qlpBaseTableNode   * aParseBaseTableNode,
                                    ellDictHandle      * aTableHandle,
                                    qlvInitNode       ** aTableNode,
                                    qllEnv             * aEnv );

stlStatus qlvValidateViewedTable( qlvStmtInfo         * aStmtInfo,
                                  ellObjectList       * aValidationObjList,
                                  qlvRefTableList     * aRefTableList,
                                  qlpBaseTableNode    * aParseBaseTableNode,
                                  ellDictHandle       * aViewHandle,
                                  qlvInitNode        ** aTableNode,
                                  qllEnv              * aEnv );

stlStatus qlvValidateSubTableNode( qlvStmtInfo          * aStmtInfo,
                                   ellObjectList        * aValidationObjList,
                                   qlvRefTableList      * aRefTableList,
                                   qlpSubTableNode      * aParseSubTableNode,
                                   qlvInitNode         ** aTableNode,
                                   stlInt32             * aTableCount,
                                   qllEnv               * aEnv );

stlStatus qlvValidateJoinTableNode( qlvStmtInfo         * aStmtInfo,
                                    ellObjectList       * aValidationObjList,
                                    qlvRefTableList     * aRefTableList,
                                    qlpJoinTableNode    * aParseJoinTableNode,
                                    qlvInitNode        ** aTableNode,
                                    stlInt32            * aTableCount,
                                    qllEnv              * aEnv );

stlStatus qlvValidateJoinSpecNamedColumns( qlvStmtInfo          * aStmtInfo,
                                           qlvExprPhraseType      aPhraseType,
                                           knlRegionMem         * aRegionMem,
                                           qlvInitNode          * aTableNode,
                                           qlpList              * aColumnList,
                                           qlvNamedColumnList  ** aNamedColumnList,
                                           qllEnv               * aEnv );

stlStatus qlvValidateJoinSpecNamedColumnInternal( qlvStmtInfo          * aStmtInfo,
                                                  qlvExprPhraseType      aPhraseType,
                                                  knlRegionMem         * aRegionMem,
                                                  qlvInitNode          * aTableNode,
                                                  stlChar              * aColumnName,
                                                  stlInt32               aNodePos,
                                                  stlBool                aIsSetOuterMark,
                                                  qlvNamedColumnList   * aNamedColumnList,
                                                  qllEnv               * aEnv );

stlStatus qlvMakeSetRelationColumns( knlRegionMem          * aRegionMem,
                                     qlvInitQuerySetNode   * aQuerySetNode,
                                     qlvInitQuerySpecNode  * aLeftMostSpecNode,
                                     stlInt32              * aTargetCount,
                                     qlvInitTarget        ** aSetColumns,
                                     qllEnv                * aEnv );

stlStatus qlvMakeSetTargets( knlRegionMem        * aRegionMem,
                             stlBool               aIsRootSet,
                             qlvInitQuerySetNode * aQuerySetNode,
                             stlInt32              aTargetCount,
                             qlvInitTarget       * aSetColumns,
                             qlvInitTarget      ** aSetTargets,
                             qllEnv              * aEnv );

stlBool qlvIsSameTable( qlvRelationName   * aRelationName,
                        stlChar           * aCatalog,
                        stlChar           * aSchema,
                        stlChar           * aTable );

stlBool qlvIsSameBaseTable( qlvInitNode     * aNode1,
                            qlvInitNode     * aNode2 );


stlStatus qlvGetTableNodeForAsterisk( qlvStmtInfo       * aStmtInfo,
                                      qlvInitNode       * aSourceTableNode,
                                      qlpObjectName     * aObjectName,
                                      qlvInitNode      ** aTableNode,
                                      stlInt32          * aTableNodeCount,
                                      qllEnv            * aEnv );

stlStatus qlvGetTableNodeWithColumn( qlvStmtInfo        * aStmtInfo,
                                     qlvExprPhraseType    aPhraseType,
                                     qlvRefTableList    * aRefTableList,
                                     qlpColumnName      * aColumnNameSource,
                                     stlBool              aIsSetOuterMark,
                                     qlvInitExpression ** aColumnExpr,
                                     stlBool            * aIsColumn,
                                     stlBool            * aisUpdatable,
                                     knlBuiltInFunc       aRelatedFuncId,
                                     dtlIterationTime     aIterationTime,
                                     knlRegionMem       * aRegionMem,
                                     qllEnv             * aEnv );

stlStatus qlvGetTableNodeWithColumnInternal( qlvStmtInfo        * aStmtInfo,
                                             qlvExprPhraseType    aPhraseType,
                                             qlvInitNode        * aTableNode,
                                             qlpColumnName      * aColumnNameSource,
                                             stlBool              aNeedCopy,
                                             stlBool              aIsRootTableNode,
                                             qlvInitExpression ** aColumnExpr,
                                             stlBool            * aExist,
                                             stlBool            * aIsUpdatable,
                                             knlRegionMem       * aRegionMem,
                                             qllEnv             * aEnv );

stlStatus qlvGetAsteriskTargetCountMultiTables( qlvStmtInfo         * aStmtInfo,
                                                qlvInitNode         * aTableNode,
                                                stlInt32              aExceptNameCount,
                                                stlChar            ** aExceptNameArr,
                                                stlInt32            * aTargetCount,
                                                qllEnv              * aEnv );

stlStatus qlvGetTargetCountFromTargetList( qlvStmtInfo      * aStmtInfo,
                                           qlpList          * aTargetSource,
                                           qlvInitNode      * aTableNode,
                                           stlInt32           aExceptNameCount,
                                           stlChar         ** aExceptNameArr,
                                           stlInt32         * aTargetCount,
                                           qllEnv           * aEnv );

stlStatus qlvValidateAsteriskMultiTables( qlvStmtInfo       * aStmtInfo,
                                          qlvExprPhraseType   aPhraseType,
                                          qlvInitNode       * aTableNode,
                                          qlpTarget         * aTargetSource,
                                          stlInt32            aExceptNameCount,
                                          stlChar          ** aExceptNameArr,
                                          qlvInitTarget     * aTargets,
                                          stlInt32          * aTargetCount,
                                          qllEnv            * aEnv );

stlStatus qlvValidateAsteriskBaseTable( qlvStmtInfo         * aStmtInfo,
                                        qlvExprPhraseType     aPhraseType,
                                        qlvInitNode         * aTableNode,
                                        qlpTarget           * aTargetSource,
                                        stlInt32              aExceptNameCount,
                                        stlChar            ** aExceptNameArr,
                                        qlvInitTarget       * aTargets,
                                        stlInt32            * aTargetCount,
                                        qllEnv              * aEnv );

stlStatus qlvValidateAsteriskSubTable( qlvStmtInfo         * aStmtInfo,
                                       qlvExprPhraseType     aPhraseType,
                                       qlvInitNode         * aTableNode,
                                       qlpTarget           * aTargetSource,
                                       stlInt32              aExceptNameCount,
                                       stlChar            ** aExceptNameArr,
                                       qlvInitTarget       * aTargets,
                                       stlInt32            * aTargetCount,
                                       qllEnv              * aEnv );

stlStatus qlvValidateAsteriskJoinTable( qlvStmtInfo         * aStmtInfo,
                                        qlvExprPhraseType     aPhraseType,
                                        qlvInitNode         * aTableNode,
                                        qlpTarget           * aTargetSource,
                                        stlInt32              aExceptNameCount,
                                        stlChar            ** aExceptNameArr,
                                        qlvInitTarget       * aTargets,
                                        stlInt32            * aTargetCount,
                                        qllEnv              * aEnv );

stlStatus qlvAddRefColumnItemOnBaseTable( knlRegionMem          * aRegionMem,
                                          qlvExprPhraseType       aPhraseType,
                                          stlInt32                aNodePos,
                                          qlvInitBaseTableNode  * aBaseTableNode,
                                          ellDictHandle         * aColumnHandle,
                                          stlBool                 aNeedCopy,
                                          qlvRefExprItem       ** aRefColumnItem,
                                          qllEnv                * aEnv );

stlStatus qlvAddRefColumnItemOnSubTable( knlRegionMem           * aRegionMem,
                                         qlvExprPhraseType        aPhraseType,
                                         stlInt32                 aNodePos,
                                         qlvInitSubTableNode    * aSubTableNode,
                                         stlInt32                 aTargetIdx,
                                         qlvInitExpression      * aExpr,
                                         stlBool                  aNeedCopy,
                                         qlvRefExprItem        ** aRefColumnItem,
                                         qllEnv                 * aEnv );

stlStatus qlvAddRefColumnItemOnJoinTable( knlRegionMem           * aRegionMem,
                                          qlvExprPhraseType        aPhraseType,
                                          stlInt32                 aNodePos,
                                          qlvInitJoinTableNode   * aJoinTableNode,
                                          stlBool                  aIsLeft,
                                          qlvInitExpression      * aExpr,
                                          stlBool                  aNeedCopy,
                                          qlvRefExprItem        ** aRefColumnItem,
                                          qllEnv                 * aEnv );

stlStatus qlvGetNamedColumnListForNaturalJoin( qlvStmtInfo          * aStmtInfo,
                                               qlvInitJoinTableNode * aJoinTableNode,
                                               qlvNamedColumnList   * aNamedColumnList,
                                               qllEnv               * aEnv );

stlStatus qlvGetAllColumnNamesFromTableNode( qlvInitNode    * aTableNode,
                                             stlInt32         aExceptNameCount,
                                             stlChar       ** aExceptNames,
                                             stlChar       ** aColumnNames,
                                             stlInt32       * aColumnNameCount,
                                             qllEnv         * aEnv );

stlStatus qlvMakeAndAddRowIdColumnOnBaseTable( qlvStmtInfo             * aStmtInfo,
                                               qlvExprPhraseType         aPhraseType,
                                               knlRegionMem            * aRegionMem,
                                               stlInt32                  aColumnPos,
                                               qlvInitBaseTableNode    * aBaseTableNode,
                                               stlBool                   aNeedCopy,
                                               stlBool                 * aExist,
                                               qlvInitExpression      ** aRefRowIdColExpr,
                                               qllEnv                  * aEnv );

stlStatus qlvSetTargetName( qlvStmtInfo   * aStmtInfo,
                            qlpTarget     * aTargetSource,
                            qlvInitTarget * aTargetCode,
                            qllEnv        * aEnv );

stlStatus qlvValidateOrderBy( qlvStmtInfo       * aStmtInfo,
                              ellObjectList     * aValidationObjList,
                              qlvRefTableList   * aRefTableList,
                              stlInt32            aTargetCount,
                              qlvInitTarget     * aTargets,
                              stlInt32            aAllowedAggrDepth,
                              qlpList           * aOrderBySource,
                              qlvOrderBy       ** aOrderBy,
                              qllEnv            * aEnv );

stlStatus qlvValidateGroupBy( qlvStmtInfo       * aStmtInfo,
                              ellObjectList     * aValidationObjList,
                              qlvRefTableList   * aRefTableList,
                              qlpGroupBy        * aGroupBySource,
                              qlpHaving         * aHavingSource,
                              qlvGroupBy        * aGroupByList,
                              stlBool           * aHasAggrDistinct,
                              qllEnv            * aEnv );

stlStatus qlvValidateValueExprByGroup( qlvStmtInfo          * aStmtInfo,
                                       qlvInitInstantNode   * aInstantTable,
                                       qlvInitExpression    * aExpr,
                                       stlBool                aHasGroup,
                                       stlInt32               aAggrDepth,
                                       qlvInitExpression   ** aResultExpr,
                                       qllEnv               * aEnv );

stlStatus qlvCheckExprByGroup( qlvStmtInfo          * aStmtInfo,
                               qlvInitInstantNode   * aInstantTable,
                               qlvInitExpression    * aExpr,
                               stlBool                aHasNestedAggr,
                               stlInt32               aAggrDepth,
                               qlvInitExpression   ** aResultExpr,
                               qllEnv               * aEnv );

stlStatus qlvValidateValueExprWithoutGroup( qlvStmtInfo          * aStmtInfo,
                                            qlvInitExpression    * aExpr,
                                            qllEnv               * aEnv );

stlStatus qlvValidateValueExprByDistinct( qlvStmtInfo          * aStmtInfo,
                                          qlvInitInstantNode  ** aDistinct,
                                          qlvGroupBy           * aGroupBy,
                                          stlInt32               aTargetCnt,
                                          qlvInitTarget        * aTargetExprList,
                                          qllEnv               * aEnv );

stlStatus qlvSetNestedAggr( qlvStmtInfo          * aStmtInfo,
                            qlvInitExprList      * aInitExprList,
                            qlvInitExpression    * aExpr,
                            qllEnv               * aEnv );

stlStatus qlnvHasDistinctAggr( qlvRefExprList  * aAggrExprList,
                               stlBool         * aHasDistinct,
                               qllEnv          * aEnv );

/* INSERT STATEMENT */
stlStatus qlvValidateInsertTarget( qlvStmtInfo         * aStmtInfo,
                                   ellObjectList       * aValidationObjList,
                                   ellDictHandle       * aTableHandle,
                                   stlInt32              aTableColumnCnt,
                                   ellDictHandle       * aTableColumnHandle,
                                   qlpInsertSource     * aInsertSource,
                                   qlvInitInsertTarget * aInsertTarget,
                                   qllEnv              * aEnv );

stlStatus qlvValidateMultiValues( qlvStmtInfo         * aStmtInfo,
                                  qlvInitInsertValues * aInitPlan,
                                  qlpInsertSource     * aInsertSource,
                                  qllEnv              * aEnv );

stlStatus qlvValidateSingleValues( qlvStmtInfo         * aStmtInfo,
                                   qlvInitInsertValues * aInitPlan,
                                   qlpInsertSource     * aInsertSource,
                                   qlpList             * aValuesExpr,
                                   qlvInitSingleRow   ** aSingleRow,
                                   qllEnv              * aEnv );

stlStatus qlvGetRelObject4Insert( qlvStmtInfo            * aStmtInfo,
                                  ellDictHandle          * aTableHandle,
                                  qlvInitInsertRelObject * aRelObject,
                                  qllEnv                 * aEnv );


/* DELETE STATEMENT */
stlStatus qlvGetRelObject4Delete( qlvStmtInfo    * aStmtInfo,
                                  qlvInitDelete  * aInitPlan,
                                  qllEnv         * aEnv );


/* UPDATE STATEMENT */
stlStatus qlvValidateUpdateSet( qlvStmtInfo      * aStmtInfo,
                                qlvInitUpdate    * aInitPlan,
                                qlvRefTableList  * aReadTableList,
                                qlvRefTableList  * aWriteTableList,
                                stlChar          * aTableAliasName,                                
                                qlpList          * aUpdateSet,
                                qllEnv           * aEnv );

stlStatus qlvGetRelObject4Update( qlvStmtInfo    * aStmtInfo,
                                  qlvInitUpdate  * aInitPlan,
                                  qllEnv         * aEnv );

stlStatus qlvSetRefColumnListForUpdate( qlvStmtInfo             * aStmtInfo,
                                        qlvInitNode             * aReadTableNode,
                                        qlvInitNode             * aWriteTableNode,
                                        qlvInitUpdate           * aUpdateNode,
                                        qlvInitUpdateRelObject  * aRelObject,
                                        qllEnv                  * aEnv );

stlStatus qlvChangeRelationNodeForRowIdCol( qlvInitNode        * aSrcTableNode,
                                            qlvInitNode        * aDstTableNode,
                                            qlvInitExpression  * aInitExprCode,
                                            qllEnv             * aEnv );

stlStatus qlvSetRefColumnListForDelete( qlvStmtInfo             * aStmtInfo,
                                        qlvInitNode             * aReadTableNode,
                                        qlvInitDelete           * aDeleteNode,
                                        qlvInitDeleteRelObject  * aRelObject,
                                        qllEnv                  * aEnv );


/* Get Type Info */
stlStatus qlvGetExprDataTypeInfo( stlChar              * aSQLString,
                                  knlRegionMem         * aRegionMem,
                                  qlvInitExpression    * aTargetExpr,
                                  knlBindContext       * aBindContext,
                                  qlvInitDataTypeInfo  * aDataTypeInfo,
                                  qllEnv               * aEnv );

stlBool qlvCheckNullable( qlvInitNode        * aTableNode,
                          qlvInitExpression  * aInitExpr );

stlStatus qlvGetResultTypeCombiantion( stlChar              * aSQLString,
                                       knlBindContext       * aBindContext,
                                       stlInt32               aExprPos,
                                       stlBool                aUseCompRule,
                                       qlvInitDataTypeInfo  * aLeftDataTypeInfo,
                                       qlvInitDataTypeInfo  * aRightDataTypeInfo,
                                       qlvInitDataTypeInfo  * aResDataTypeInfo,
                                       qllEnv               * aEnv );


/** @} qlnv */

#endif /* _QLNVNODE_H_ */
