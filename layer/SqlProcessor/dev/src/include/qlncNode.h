/*******************************************************************************
 * qlncNode.h
 *
 * Copyright (c) 2013, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *
 *
 ******************************************************************************/

#ifndef _QLNCNODE_H_
#define _QLNCNODE_H_ 1

/**
 * @file qlncNode.h
 * @brief SQL Processor Layer Candidate Optimization Node
 */

#include <qlDef.h>

/**
 * @addtogroup qlnc
 * @{
 */


/*******************************************************************************
 * DEFINITIONS
 ******************************************************************************/

/**
 * @brief Candidate Node Type
 */
enum qlncNodeType
{
    /* Create Node에서 생성되는 노드 */
    QLNC_NODE_TYPE_QUERY_SET = 0,           /**< Set Expression Node */
    QLNC_NODE_TYPE_SET_OP,                  /**< Set Operation Node */
    QLNC_NODE_TYPE_QUERY_SPEC,              /**< Query Specification Node */
    QLNC_NODE_TYPE_BASE_TABLE,              /**< Base Table Node */
    QLNC_NODE_TYPE_SUB_TABLE,               /**< Sub Table Node */
    QLNC_NODE_TYPE_JOIN_TABLE,              /**< Join Table Node */
    QLNC_NODE_TYPE_FLAT_INSTANT,            /**< Flat Instant Node */
    QLNC_NODE_TYPE_SORT_INSTANT,            /**< Sort Instant Node */
    QLNC_NODE_TYPE_HASH_INSTANT,            /**< Hash Instant Node */

    QLNC_NODE_TYPE_CREATE_NODE_MAX,

    /* Cost Based Optimizer에서 생성되는 노드 */
    QLNC_NODE_TYPE_INNER_JOIN_TABLE,        /**< Inner Join Table Node */
    QLNC_NODE_TYPE_OUTER_JOIN_TABLE,        /**< Outer Join Table Node */
    QLNC_NODE_TYPE_SEMI_JOIN_TABLE,         /**< Outer Join Table Node */
    QLNC_NODE_TYPE_ANTI_SEMI_JOIN_TABLE,    /**< Outer Join Table Node */

    QLNC_NODE_TYPE_MAX
};


/**
 * @brief Instant Type
 */
enum qlncInstantType
{
    QLNC_INSTANT_TYPE_NORMAL = 0,   /**< Normal */
    QLNC_INSTANT_TYPE_ORDER,        /**< Order */
    QLNC_INSTANT_TYPE_GROUP,        /**< Group */
    QLNC_INSTANT_TYPE_JOIN,         /**< Join */
    QLNC_INSTANT_TYPE_DISTINCT      /**< Distinct (for Distinct, Alias Group) */
};


/**
 * @brief Set Type
 */
enum qlncSetType
{
    QLNC_SET_TYPE_UNION = 0,        /**< UNION */
    QLNC_SET_TYPE_EXCEPT,           /**< EXCEPT */
    QLNC_SET_TYPE_INTERSECT         /**< INTERSECT */
};


/**
 * @brief Set Option
 */
enum qlncSetOption
{
    QLNC_SET_OPTION_ALL = 0,        /**< ALL */
    QLNC_SET_OPTION_DISTINCT        /**< DISTINCT */
};


/**
 * @brief Join Type
 */
enum qlncJoinType
{
    QLNC_JOIN_TYPE_INVALID = 0,     /**< Invalid (default) */
    QLNC_JOIN_TYPE_INNER,           /**< Inner Join */
    QLNC_JOIN_TYPE_OUTER,           /**< Outer Join */
    QLNC_JOIN_TYPE_SEMI,            /**< Semi Join */
    QLNC_JOIN_TYPE_ANTI_SEMI,       /**< Anti Semi Join */
    QLNC_JOIN_TYPE_ANTI_SEMI_NA     /**< Null Aware Anti Semi Join */
};


/**
 * @brief Join Direct
 */
enum qlncJoinDirect
{
    QLNC_JOIN_DIRECT_NONE = 0,  /**< None */
    QLNC_JOIN_DIRECT_LEFT,      /**< Left */
    QLNC_JOIN_DIRECT_RIGHT,     /**< Right */
    QLNC_JOIN_DIRECT_FULL,      /**< Full */
};


/**
 * @brief Group By Type
 */
enum qlncGroupByType
{
    QLNC_GROUP_BY_TYPE_NA = 0,
    QLNC_GROUP_BY_TYPE_ORDINARY,
    QLNC_GROUP_BY_TYPE_GROUPING_SET,
    QLNC_GROUP_BY_TYPE_EMPTY,
    QLNC_GROUP_BY_TYPE_ROLL_UP,
    QLNC_GROUP_BY_TYPE_CUBE
};


/**
 * @brief Index Type
 */
enum qlncIndexType
{
    QLNC_INDEX_TYPE_PRIMARY_KEY = 0,    /**< Primary Key */
    QLNC_INDEX_TYPE_UNIQUE_KEY,         /**< Unique Key */
    QLNC_INDEX_TYPE_FOREIGN_KEY,        /**< Foreign Key */
    QLNC_INDEX_TYPE_UNIQUE_INDEX,       /**< Unique Index */
    QLNC_INDEX_TYPE_NON_UNIQUE_INDEX    /**< Non Unique Index */
};


/**
 * @brief Best Optimizer Type
 */
enum qlncBestOptType
{
    QLNC_BEST_OPT_TYPE_NA = 0,          /**< N/A */

    QLNC_BEST_OPT_TYPE_QUERY_SET,       /**< Query Set */
    QLNC_BEST_OPT_TYPE_SET_OP,          /**< Set OP */
    QLNC_BEST_OPT_TYPE_QUERY_SPEC,      /**< Query Spec */

    /* Access Path */
    QLNC_BEST_OPT_TYPE_TABLE_SCAN,      /**< Table Scan */
    QLNC_BEST_OPT_TYPE_INDEX_SCAN,      /**< Index Scan */
    QLNC_BEST_OPT_TYPE_INDEX_COMBINE,   /**< Index Combine */
    QLNC_BEST_OPT_TYPE_ROWID_SCAN,      /**< Rowid Scan */

    /* Sub Table */
    QLNC_BEST_OPT_TYPE_FILTER,          /**< Filter */

    /* Join */
    QLNC_BEST_OPT_TYPE_INNER_JOIN,      /**< Inner Join */
    QLNC_BEST_OPT_TYPE_OUTER_JOIN,      /**< Outer Join */
    QLNC_BEST_OPT_TYPE_SEMI_JOIN,       /**< Semi Join */
    QLNC_BEST_OPT_TYPE_ANTI_SEMI_JOIN,  /**< Anti Semi Join */

    /* Join Operation */
    QLNC_BEST_OPT_TYPE_NESTED_LOOPS,    /**< Nested Loops Join */
    QLNC_BEST_OPT_TYPE_MERGE,           /**< Merge Join */
    QLNC_BEST_OPT_TYPE_HASH,            /**< Hash Join */
    QLNC_BEST_OPT_TYPE_JOIN_COMBINE,    /**< Join Combine */

    /* Instant */
    QLNC_BEST_OPT_TYPE_SORT_INSTANT,    /**< Sort Instant */
    QLNC_BEST_OPT_TYPE_HASH_INSTANT,    /**< Hash Instant */

    /* Group By */
    QLNC_BEST_OPT_TYPE_GROUP,           /**< Pre-ordered를 이용한 Group By */

    QLNC_BEST_OPT_TYPE_MAX
};


/**
 * @brief Join Operation Type
 */
enum qlncJoinOperType
{
    QLNC_JOIN_OPER_TYPE_DEFAULT = 0,            /**< DEFAULT */
    QLNC_JOIN_OPER_TYPE_USE_NESTED,             /**< USE_NL */
    QLNC_JOIN_OPER_TYPE_NO_USE_NESTED,          /**< NO_USE_NL */
    QLNC_JOIN_OPER_TYPE_USE_INDEX_NESTED,       /**< USE_NL */
    QLNC_JOIN_OPER_TYPE_NO_USE_INDEX_NESTED,    /**< NO_USE_NL */
    QLNC_JOIN_OPER_TYPE_USE_MERGE,              /**< USE_MERGE */
    QLNC_JOIN_OPER_TYPE_NO_USE_MERGE,           /**< NO_USE_MERGE */
    QLNC_JOIN_OPER_TYPE_USE_HASH,               /**< USE_HASH */
    QLNC_JOIN_OPER_TYPE_NO_USE_HASH             /**< NO_USE_HASH */
};


/**
 * @brief Unnest Operation Type
 */
enum qlncUnnestOperType
{
    QLNC_UNNEST_OPER_TYPE_DEFAULT = 0,  /**< DEFAULT */
    QLNC_UNNEST_OPER_TYPE_NL_ISJ,       /**< NL_ISJ */
    QLNC_UNNEST_OPER_TYPE_NL_SJ,        /**< NL_SJ */
    QLNC_UNNEST_OPER_TYPE_NL_AJ,        /**< NL_AJ */
    QLNC_UNNEST_OPER_TYPE_MERGE_SJ,     /**< MERGE_SJ */
    QLNC_UNNEST_OPER_TYPE_MERGE_AJ,     /**< MERGE_AJ */
    QLNC_UNNEST_OPER_TYPE_HASH_ISJ,     /**< HASH_ISJ */
    QLNC_UNNEST_OPER_TYPE_HASH_SJ,      /**< HASH_SJ */
    QLNC_UNNEST_OPER_TYPE_HASH_AJ       /**< HASH_AJ */
};


/**
 * @brief Push Subq Type
 */
enum qlncPushSubqType
{
    QLNC_PUSH_SUBQ_TYPE_DEFAULT = 0,    /**< DEFAULT */
    QLNC_PUSH_SUBQ_TYPE_YES,            /**< PUSH SUBQUERY */
    QLNC_PUSH_SUBQ_TYPE_NO              /**< NO PUSH SUBQUERY */
};


/**
 * @brief Distinct Operation Type
 */
enum qlncDistinctOperType
{
    QLNC_DISTINCT_OPER_TYPE_DEFAULT = 0,    /**< DEFAULT */
    QLNC_DISTINCT_OPER_TYPE_HASH            /**< USE HASH INSTANT */
};


/**
 * @brief Group Operation Type
 */
enum qlncGroupOperType
{
    QLNC_GROUP_OPER_TYPE_DEFAULT = 0,    /**< DEFAULT */
    QLNC_GROUP_OPER_TYPE_HASH            /**< USE HASH INSTANT */
};


#define QLNC_INSTANT_COLUMN_TYPE_KEY    (stlInt16)1 /**< Key Column */
#define QLNC_INSTANT_COLUMN_TYPE_RECORD (stlInt16)2 /**< Record Column */
#define QLNC_INSTANT_COLUMN_TYPE_READ   (stlInt16)4 /**< Read Column */

#define QLNC_TABLE_MAP_ARRAY_DEFAULT_SIZE   8       /**< Table Map Array Default Size */
#define QLNC_TABLE_MAP_ARRAY_INCREASE_SIZE  8       /**< Table Map Array Increase Size */

#define QLNC_QUERY_BLOCK_MAP_ARRAY_DEFAULT_SIZE   8 /**< Table Map Array Default Size */
#define QLNC_QUERY_BLOCK_MAP_ARRAY_INCREASE_SIZE  8 /**< Table Map Array Increase Size */


#define QLNC_INIT_NODE_COMMON( _aNodeCommon, _aNodeID, _aNodeType ) \
{                                                                   \
    (_aNodeCommon)->mNodeType       = (_aNodeType);                 \
    (_aNodeCommon)->mNodeID         = (_aNodeID);                   \
    (_aNodeCommon)->mBestOptType    = QLNC_BEST_OPT_TYPE_NA;        \
    (_aNodeCommon)->mBestCost       = NULL;                         \
    (_aNodeCommon)->mIsAnalyzedCBO  = STL_FALSE;                    \
}

#define QLNC_INIT_CREATE_NODE_PARAM_INFO( _aCreateNodeParamInfo,        \
                                          _aParamInfo,                  \
                                          _aInitNode,                   \
                                          _aHintInfo,                   \
                                          _aTableMapArr,                \
                                          _aOuterTableMapArr,           \
                                          _aExprSubQuery,               \
                                          _aQBMapArr )                  \
{                                                                       \
    (_aCreateNodeParamInfo)->mParamInfo         = *(_aParamInfo);       \
    (_aCreateNodeParamInfo)->mInitNode          = (_aInitNode);         \
    (_aCreateNodeParamInfo)->mHintInfo          = (_aHintInfo);         \
    (_aCreateNodeParamInfo)->mCandNode          = NULL;                 \
    (_aCreateNodeParamInfo)->mTableMapArr       = (_aTableMapArr);      \
    (_aCreateNodeParamInfo)->mOuterTableMapArr  = (_aOuterTableMapArr); \
    (_aCreateNodeParamInfo)->mExprSubQuery      = (_aExprSubQuery);     \
    (_aCreateNodeParamInfo)->mQBMapArr          = (_aQBMapArr);         \
}

#define QLNC_SWAP_NODE( _aNode1, _aNode2 )                      \
{                                                               \
    qlncNodeCommon  * _sTmpNode;                                \
    _sTmpNode = (_aNode1);                                      \
    (_aNode1) = (_aNode2);                                      \
    (_aNode2) = (_sTmpNode);                                    \
}

#define QLNC_INIT_ACC_PATH_HINT( _aAccPathHint )                \
{                                                               \
    (_aAccPathHint)->mPossibleIndexScanInfoArr  = NULL;         \
    (_aAccPathHint)->mPossibleIndexScanCount    = 0;            \
    (_aAccPathHint)->mPossibleTableScan         = STL_FALSE;    \
    (_aAccPathHint)->mPossibleIndexScan         = STL_FALSE;    \
    (_aAccPathHint)->mPossibleRowidScan         = STL_FALSE;    \
    (_aAccPathHint)->mPossibleIndexCombine      = STL_FALSE;    \
}

#define QLNC_INIT_JOIN_HINT( _aJoinHint )                       \
{                                                               \
    (_aJoinHint)->mFixedOrder = STL_FALSE;                      \
    (_aJoinHint)->mFixedPos = STL_FALSE;                        \
    (_aJoinHint)->mIsLeftPos = STL_FALSE;                       \
    (_aJoinHint)->mIsInverted = STL_FALSE;                      \
    (_aJoinHint)->mJoinOperation = QLNC_JOIN_OPER_TYPE_DEFAULT; \
}

#define QLNC_INIT_QUERY_TRANSFORM_HINT( _aQueryTransformHint )                      \
{                                                                                   \
    (_aQueryTransformHint)->mAllowedQueryTransform  = STL_TRUE;                     \
    (_aQueryTransformHint)->mAllowedUnnest          = STL_TRUE;                     \
    (_aQueryTransformHint)->mUnnestOperType         = QLNC_UNNEST_OPER_TYPE_DEFAULT;\
}

#define QLNC_INIT_OTHER_HINT( _aOtherHint )                         \
{                                                                   \
    (_aOtherHint)->mPushSubqType    = QLNC_PUSH_SUBQ_TYPE_DEFAULT;  \
}

#define QLNC_CONVERT_INIT_SET_TYPE_TO_CAND_SET_TYPE( _aInitSetType, _aCandSetType ) \
{                                                                                   \
    if( (_aInitSetType) == QLV_SET_TYPE_UNION )                                     \
    {                                                                               \
        (_aCandSetType) = QLNC_SET_TYPE_UNION;                                      \
    }                                                                               \
    else if( (_aInitSetType) == QLV_SET_TYPE_EXCEPT )                               \
    {                                                                               \
        (_aCandSetType) = QLNC_SET_TYPE_EXCEPT;                                     \
    }                                                                               \
    else                                                                            \
    {                                                                               \
        STL_DASSERT( (_aInitSetType) == QLV_SET_TYPE_INTERSECT );                   \
        (_aCandSetType) = QLNC_SET_TYPE_INTERSECT;                                  \
    }                                                                               \
}

#define QLNC_CONVERT_INIT_SET_OPTION_TO_CAND_SET_OPTION( _aInitSetOption, _aCandSetOption ) \
{                                                                                           \
    if( (_aInitSetOption) == QLV_SET_OPTION_ALL )                                           \
    {                                                                                       \
        (_aCandSetOption) = QLNC_SET_OPTION_ALL;                                            \
    }                                                                                       \
    else                                                                                    \
    {                                                                                       \
        STL_DASSERT( (_aInitSetOption) == QLV_SET_OPTION_DISTINCT );                        \
        (_aCandSetOption) = QLNC_SET_OPTION_DISTINCT;                                       \
    }                                                                                       \
}

/*******************************************************************************
 * STRUCTURES
 ******************************************************************************/

/**
 * @brief Common Node
 */
struct qlncNodeCommon
{
    qlncNodeType          mNodeType;        /**< Node Type */
    stlInt32              mNodeID;          /**< Global Node 식별자 */

    qlncBestOptType       mBestOptType;     /**< Best Optimized Type */
    qlncCostCommon      * mBestCost;        /**< Best Cost */
    stlBool               mIsAnalyzedCBO;   /**< 이미 CBOpt를 수행했는지 여부 */
};


/**
 * @brief Create Node Parameter Information
 */
struct qlncCreateNodeParamInfo
{
    qlncParamInfo         mParamInfo;
    qlvInitNode         * mInitNode;
    qlvHintInfo         * mHintInfo;
    qlncNodeCommon      * mCandNode;
    qlncTableMapArray   * mTableMapArr;
    qlncTableMapArray   * mOuterTableMapArr;    /**< for SubQuery Expression */
    qlncExprSubQuery    * mExprSubQuery;        /**< for SubQuery Expression */
    qlncQBMapArray      * mQBMapArr;            /**< Query Block Map Array */
};


/**
 * @brief Select Statement Param Info
 */
struct qlncSelectStmtParamInfo
{
    qlvInitNode         * mStmtNode;                    /**< Statement Node */
    qlvInitNode         * mQueryNode;                   /**< Query Node */

    /* Output */
    qlncPlanCommon      * mOutPlanQuery;                /**< Query Candidate Plan Node */
    qlvInitNode         * mOutStmtNode;                 /**< Statement Node */
    qloValidTblStatList * mOutValidTblStatList;         /**< Valid Table Statistic List */
};


/**
 * @brief Insert Statement Param Info
 */
struct qlncInsertStmtParamInfo
{
    qlvInitNode         * mStmtNode;                    /**< Statement Node */
    qlvInitNode         * mWriteTableNode;              /**< Write Table Node */
    stlInt32              mMultiValueCount;             /**< Multi-Value Count */
    stlInt32              mTotalColumnCount;            /**< Total Column Count */
    qlvRefExprList      * mInsertColExprList;           /**< Insert Column Expression List */
    qlvInitInsertTarget * mInsertTarget;                /**< Insert Target Column */
    qlvInitSingleRow    * mRowList;                     /**< Multi Values */
    stlInt32              mReturnTargetCount;           /**< Return Target Count */
    qlvInitTarget       * mReturnTargetArray;           /**< Return Target Array */
    
    /* Output */
    qlvInitTarget       * mOutReturnTargetArray;        /**< Return Target Array */
    qlvRefExprList      * mOutReturnOuterColumnList;    /**< Return Outer Column List */
    qlvInitInsertTarget * mOutInsertTarget;             /**< Insert Target Column */
    qlvInitSingleRow    * mOutRowList;                  /**< Insert Value Row List */
    qlvRefExprList      * mOutInsertColExprList;        /**< Insert Column Expression List */
    qlncPlanCommon      * mOutPlanWriteTable;           /**< Write Table Candidate Plan Node */
    qlncPlanCommon      * mOutPlanRowSubQueryList;      /**< Row SubQuery List Candidate Plan Node */
    qlncPlanCommon      * mOutPlanReturnSubQueryList;   /**< Return SubQuery List Candidate Plan Node */
    qlvInitNode         * mOutStmtNode;                 /**< Statement Node */
    qloValidTblStatList * mOutValidTblStatList;         /**< Valid Table Statistic List */
};


/**
 * @brief Update Statement Param Info
 */
struct qlncUpdateStmtParamInfo
{
    qlvInitNode         * mStmtNode;                    /**< Statement Node */

    /* Output */
    qlvInitTarget       * mOutReturnTargetArray;        /**< Return Target Array */
    qlvRefExprList      * mOutSetOuterColumnList;       /**< Set Outer Column List */
    qlvRefExprList      * mOutReturnOuterColumnList;    /**< Return Outer Column List */
    qlvInitExpression  ** mOutSetExprArray;             /**< Set Column Value Expression List */
    qlvRefExprList      * mOutSetColExprList;           /**< Set Column Expression List */
    qlvRefExprList      * mOutShareReadColList;         /**< Share Read Column List */
    qlvRefExprList      * mOutShareWriteColList;        /**< Share Write Column List */
    qlncPlanCommon      * mOutPlanRootNode;             /**< Read Table Candidate Plan Node */
    qlncPlanCommon      * mOutPlanReadTable;            /**< Read Table Candidate Plan Node */
    qlncPlanCommon      * mOutPlanWriteTable;           /**< Write Table Candidate Plan Node */
    qlncPlanCommon      * mOutPlanSetSubQueryList;      /**< Set SubQuery List Candidate Plan Node */
    qlncPlanCommon      * mOutPlanReturnSubQueryList;   /**< Return SubQuery List Candidate Plan Node */
    qlvInitNode         * mOutStmtNode;                 /**< Statement Node */
    qloValidTblStatList * mOutValidTblStatList;         /**< Valid Table Statistic List */
};


/**
 * @brief Delete Statement Param Info
 */
struct qlncDeleteStmtParamInfo
{
    qlvInitNode         * mStmtNode;                    /**< Statement Node */

    /* Output */
    qlvInitTarget       * mOutReturnTargetArray;        /**< Return Target Array */
    qlvRefExprList      * mOutReturnOuterColumnList;    /**< Return Outer Column List */
    qlncPlanCommon      * mOutPlanDelete;               /**< Delete Candidate Plan Node */
    qlncPlanCommon      * mOutPlanReadTable;            /**< Read Table Candidate Plan Node */
    qlncPlanCommon      * mOutPlanReturnSubQueryList;   /**< Return SubQuery List Candidate Plan Node */
    qlvInitNode         * mOutStmtNode;                 /**< Statement Node */
    qloValidTblStatList * mOutValidTblStatList;         /**< Valid Table Statistic List */
};


/**
 * @brief Column Statistic
 */
struct qlncColumnStat
{
    stlBool               mNullable;            /**< Null 허용 여부 */
    stlBool               mIsUnique;            /**< Unique 여부 */
    stlChar               mPadding[6];
    stlFloat64            mColumnCard;          /**< Column Cardinality */
    stlFloat64            mAvgColumnLen;        /**< Column의 평균 길이 */
};

/**
 * @brief Table Statistic
 */
struct qlncTableStat
{
    stlFloat64            mRowCount;            /**< Row Count */
    stlFloat64            mPageCount;           /**< Page Count */
    stlFloat64            mAvgRowLen;           /**< Row의 평균 길이 */
    stlFloat64            mAvgRowCountPerPage;  /**< Page당 평균 Row 개수 */
    stlInt64              mColumnCount;         /**< Column의 개수 */
    qlncColumnStat      * mColumnStat;          /**< Column 통계 정보 */
    stlBool               mIsAnalyzed;          /**< 분석된 통계인지 여부 */
    stlUInt8              mPadding[7];
};


/**
 * @brief Index Statistic
 */
struct qlncIndexStat
{
    qlncIndexType         mIndexType;           /**< Index 유형 */
    ellDictHandle       * mIndexHandle;         /**< Index Handle */
    void                * mIndexPhyHandle;      /**< Index Physical Handle */
    ellIndexKeyDesc     * mIndexKeyDesc;        /**< Index Key Description */
    stlFloat64            mDepth;               /**< Root에서 Leaf Page까지의 Depth */
    stlFloat64            mLeafPageCount;       /**< Leaf Page Count */
    stlFloat64            mAvgKeyCountPerPage;  /**< Page당 평균 Key 개수 */
    stlInt32              mIndexKeyCount;       /**< Index Key Count */
    stlBool               mIsUnique;            /**< Unique 여부 */
    stlBool               mIsAnalyzed;          /**< 분석된 통계인지 여부 */
    stlUInt8              mPadding[2];
};


/**
 * @brief Index Scan Information
 */
struct qlncIndexScanInfo
{
    stlInt32          mOffset;          /**< Index Stat의 Offset */
    stlBool           mIsAsc;           /**< Index Scan 방향 */
    stlUInt8          mPadding[3];
};


/**
 * @brief Access Path Hint Information
 */
struct qlncAccPathHint
{
    qlncIndexScanInfo   * mPossibleIndexScanInfoArr;
    stlInt32              mPossibleIndexScanCount;
    stlBool               mPossibleTableScan;
    stlBool               mPossibleIndexScan;
    stlBool               mPossibleRowidScan;
    stlBool               mPossibleIndexCombine;
    stlBool               mPossibleInKeyRange;
};


/**
 * @brief Join Hint Information
 */
struct qlncJoinHint
{
    /* Join Order */
    stlBool               mFixedOrder;
    stlBool               mFixedPos;
    stlBool               mIsLeftPos;

    /* Inverted 여부 */
    stlBool               mIsInverted;      /**< for ONLY Semi Join( NL 지정 시에만 적용) */

    /* Join Operation */
    qlncJoinOperType      mJoinOperation;
};


/**
 * @brief qlncQueryTransformHint
 */
struct qlncQueryTransformHint
{
    stlBool               mAllowedQueryTransform;
    stlBool               mAllowedUnnest;

    qlncUnnestOperType    mUnnestOperType;
};


/**
 * @brief qlncOtherHint
 */
struct qlncOtherHint
{
    qlncPushSubqType      mPushSubqType;
    qlncDistinctOperType  mDistinctOperType;
    qlncGroupOperType     mGroupOperType;
};


/**
 * @brief Relation Name
 */
struct qlncRelationName
{
    stlChar             * mCatalog;
    stlChar             * mSchema;
    stlChar             * mTable;
};


/**
 * @brief Table Info
 */
struct qlncTableInfo
{
    ellDictHandle       * mSchemaHandle;
    ellDictHandle       * mTableHandle;
    void                * mTablePhyHandle;
    stlChar             * mDumpOption;
    qlncRelationName      mRelationName;
};


/**
 * @brief Node Array
 */
struct qlncNodeArray
{
    knlRegionMem        * mRegionMem;
    stlInt32              mCurCnt;
    stlInt32              mMaxCnt;
    qlncNodeCommon     ** mNodeArr;
};


/**
 * @brief Table Map
 */
struct qlncTableMap
{
    qlvInitNode     * mInitNode;
    qlncNodeCommon  * mCandNode;
};


/**
 * @brief Table Map Array
 */
struct qlncTableMapArray
{
    knlRegionMem    * mRegionMem;
    stlInt32          mCurCount;
    stlInt32          mMaxCount;
    qlncTableMap    * mArray;
};


/**
 * @brief Query Block Map
 */
struct qlncQBMap
{
    stlInt32          mQBID;
    stlChar           mPadding[4];
    qlvInitNode     * mInitNode;
    qlncNodeCommon  * mCandNode;
};


/**
 * @brief Query Block Map Array
 */
struct qlncQBMapArray
{
    knlRegionMem    * mRegionMem;
    stlInt32          mCurCount;
    stlInt32          mMaxCount;
    qlncQBMap       * mArray;
};


/**
 * @brief QUERY SET Candidate Optimization Node
 */
struct qlncQuerySet
{
    qlncNodeCommon        mNodeCommon;              /**< Common Node Info */

    qlncTableMapArray   * mTableMapArr;

    stlBool               mIsRootSet;

    /* Query Block ID */
    stlInt32                  mQBID;

    /* Target */
    stlInt32              mSetTargetCount;
    qlncTarget          * mSetTargetArr;
    qlncTarget          * mSetColumnArr;

    qlncNodeCommon      * mChildNode;

    /* Offset/Limit */
    qlncExprCommon      * mOffset;
    qlncExprCommon      * mLimit;

    /* Cost Based Optimizer */
    qlncCostCommon      * mQuerySetCost;    /**< Query Set Cost */
};


/**
 * @brief SET OP Candidate Optimization Node
 */
struct qlncSetOP
{
    qlncNodeCommon        mNodeCommon;              /**< Common Node Info */

    /* Set Operator */
    qlncSetType           mSetType;
    qlncSetOption         mSetOption;

    /* Child Node */
    stlInt32              mChildCount;
    stlInt32              mCurChildIdx;
    qlncNodeCommon     ** mChildNodeArr;

    /* Cost Based Optimizer */
    qlncCostCommon      * mSetOPCost;       /**< Set OP Cost */
};


/**
 * @brief QUERY SPECIFICATION Candidate Optimization Node
 */
struct qlncQuerySpec
{
    qlncNodeCommon            mNodeCommon;              /**< Common Node Info */

    qlncTableMapArray       * mTableMapArr;

    /* Query Block ID */
    stlInt32                  mQBID;

    /* Hint Info */
    qlncQueryTransformHint  * mQueryTransformHint;
    qlncOtherHint           * mOtherHint;

    /* Distinct 여부 */
    stlBool                   mIsDistinct;

    /* Target */
    stlInt32                  mTargetCount;
    qlncTarget              * mTargetArr;

    /* From, Group by, Order By */
    qlncNodeCommon          * mChildNode;

    /* Offset/Limit */
    qlncExprCommon          * mOffset;
    qlncExprCommon          * mLimit;

    /* Non Filter SubQuery Expression List */
    qlncRefExprList         * mNonFilterSubQueryExprList;

    /* Cost Based Optimizer */
    qlncQuerySpecCost       * mQuerySpecCost;   /**< Query Spec Cost */
};


/**
 * @brief BASE TABLE Candidate Optimization Node
 */
struct qlncBaseTableNode
{
    qlncNodeCommon            mNodeCommon;          /**< Common Node Info */
    qlncTableInfo           * mTableInfo;           /**< Table Info */

    /* Base Statistic */
    qlncTableStat           * mTableStat;           /**< Table Statistic */
    stlInt32                  mIndexCount;          /**< Index Count */
    qlncIndexStat           * mIndexStatArr;        /**< Index Statistic Array */

    /* Hint Info */
    qlncAccPathHint         * mAccPathHint;         /**< Access Path Hint */
    qlncJoinHint            * mJoinHint;            /**< Join Hint */

    /* Cost Based Optimizer */
    qlncAndFilter           * mFilter;              /**< Filter */
    qlncRefColumnList         mReadColumnList;      /**< Read Column List */

    qlncTableScanCost       * mTableScanCost;           /**< Table Scan Cost */
    stlInt32                  mIndexScanCostCount;      /**< Index Scan Count */
    stlInt32                  mBestInKeyRangeCostCount; /**< Best In Key Range Count */
    qlncIndexScanCost       * mIndexScanCostArr;        /**< Index Scan Cost */
    qlncIndexScanCost       * mBestInKeyRangeCostArr;   /**< Best In Key Range Cost */
    qlncIndexCombineCost    * mIndexCombineCost;        /**< Index Combine Cost */
    qlncRowidScanCost       * mRowidScanCost;           /**< Rowid Scan Cost */
};


/**
 * @brief SUB TABLE Candidate Optimization Node
 */
struct qlncSubTableNode
{
    qlncNodeCommon            mNodeCommon;          /**< Common Node Info */
    stlBool                   mHasHandle;           /**< Has View Handle */
    ellDictHandle             mSchemaHandle;        /**< Schema Handle of the view */
    ellDictHandle             mViewHandle;          /**< View Handle */
    qlncRelationName        * mRelationName;        /**< Relation Name */
    qlncNodeCommon          * mQueryNode;           /**< Query Node */

    /* Hint Info */
    qlncJoinHint            * mJoinHint;            /**< Join Hint */

    /* Cost Based Optimizer */
    qlncAndFilter           * mFilter;              /**< Filter */
    qlncRefColumnList         mReadColumnList;      /**< Read Column List */

    qlncFilterCost          * mFilterCost;          /**< Filter Cost */
};


/**
 * @brief JOIN TABLE Candidate Optimization Node
 */
struct qlncJoinTableNode
{
    qlncNodeCommon                mNodeCommon;              /**< Common Node Info */
    qlncNodeArray                 mNodeArray;               /**< Table Node Array */
    qlncAndFilter               * mJoinCond;                /**< Join Condition */
    qlncAndFilter               * mFilter;                  /**< Filter */

    qlncJoinType                  mJoinType;                /**< Join Type */
    qlncJoinDirect                mJoinDirect;              /**< Left/Right/Full 여부 */

    qlncNamedColumnList         * mNamedColumnList;         /**< Named Column List */

    /* Hint Info */
    qlncJoinHint                * mSemiJoinHint;            /**< Semi-Join/Anti-Semi-Join Hint */

    qlncInnerJoinTableNode      * mBestInnerJoinTable;      /**< Best Inner Join Table Node */
    qlncOuterJoinTableNode      * mBestOuterJoinTable;      /**< Best Outer Join Table Node */
    qlncSemiJoinTableNode       * mBestSemiJoinTable;       /**< Best Semi Join Table Node */
    qlncAntiSemiJoinTableNode   * mBestAntiSemiJoinTable;   /**< Best Anti Semim Join Table Node */
};


/**
 * @brief INNER JOIN TABLE Candidate Optimization Node
 */
struct qlncInnerJoinTableNode
{
    qlncNodeCommon            mNodeCommon;          /**< Common Node Info */

    qlncNamedColumnList     * mNamedColumnList;     /**< Named Column List */

    /* Cost Based Optimizer */
    qlncNodeCommon          * mLeftNode;            /**< Left Node */
    qlncNodeCommon          * mRightNode;           /**< Right Node */
    qlncAndFilter           * mJoinCondition;       /**< Join Condition */
    qlncAndFilter           * mJoinFilter;          /**< Join Filter */
    qlncRefColumnList         mReadColumnList;      /**< Read Column List */

    qlncNestedJoinCost      * mNestedJoinCost;      /**< Nested Loops Join Cost */
    qlncMergeJoinCost       * mMergeJoinCost;       /**< Merge Join Cost */
    qlncHashJoinCost        * mHashJoinCost;        /**< Hash Join Cost */
    qlncJoinCombineCost     * mJoinCombineCost;     /**< Join Combine Cost */
};


/**
 * @brief OUTER JOIN TABLE Candidate Optimization Node
 */
struct qlncOuterJoinTableNode
{
    qlncNodeCommon            mNodeCommon;          /**< Common Node Info */

    /* Cost Based Optimizer */
    qlncJoinDirect            mJoinDirect;          /**< Join Direct (Left/Right/Full) */
    qlncNodeCommon          * mLeftNode;            /**< Left Node */
    qlncNodeCommon          * mRightNode;           /**< Right Node */
    qlncAndFilter           * mJoinCondition;       /**< Join Condition */
    qlncAndFilter           * mJoinFilter;          /**< Join Filter */
    qlncRefColumnList         mReadColumnList;      /**< Read Column List */

    qlncNestedJoinCost      * mNestedJoinCost;      /**< Nested Loops Join Cost */
    qlncMergeJoinCost       * mMergeJoinCost;       /**< Merge Join Cost */
    qlncHashJoinCost        * mHashJoinCost;        /**< Hash Join Cost */
};


/**
 * @brief SEMI JOIN TABLE Candidate Optimization Node
 */
struct qlncSemiJoinTableNode
{
    qlncNodeCommon            mNodeCommon;          /**< Common Node Info */

    /* Cost Based Optimizer */
    qlncJoinDirect            mJoinDirect;          /**< Join Direct (Left/Right) */
    qlncNodeCommon          * mLeftNode;            /**< Left Node */
    qlncNodeCommon          * mRightNode;           /**< Right Node */
    qlncAndFilter           * mJoinCondition;       /**< Join Condition */
    qlncAndFilter           * mJoinFilter;          /**< Join Filter */
    qlncRefColumnList         mReadColumnList;      /**< Read Column List */

    qlncNestedJoinCost      * mNestedJoinCost;      /**< Nested Loops Join Cost */
    qlncMergeJoinCost       * mMergeJoinCost;       /**< Merge Join Cost */
    qlncHashJoinCost        * mHashJoinCost;        /**< Hash Join Cost */
};


/**
 * @brief ANTI SEMI JOIN TABLE Candidate Optimization Node
 */
struct qlncAntiSemiJoinTableNode
{
    qlncNodeCommon            mNodeCommon;          /**< Common Node Info */

    /* Cost Based Optimizer */
    qlncJoinDirect            mJoinDirect;          /**< Join Direct (Left/Right) */
    stlBool                   mIsNullAware;         /**< Null Aware 여부 */
    qlncNodeCommon          * mLeftNode;            /**< Left Node */
    qlncNodeCommon          * mRightNode;           /**< Right Node */
    qlncAndFilter           * mJoinCondition;       /**< Join Condition */
    qlncAndFilter           * mJoinFilter;          /**< Join Filter */
    qlncRefColumnList         mReadColumnList;      /**< Read Column List */

    qlncNestedJoinCost      * mNestedJoinCost;      /**< Nested Loops Join Cost */
    qlncMergeJoinCost       * mMergeJoinCost;       /**< Merge Join Cost */
    qlncHashJoinCost        * mHashJoinCost;        /**< Hash Join Cost */
};


/**
 * @brief Sort Instant Node
 */
struct qlncSortInstantNode
{
    qlncNodeCommon            mNodeCommon;          /**< Common Node Info */

    qlncInstantType           mInstantType;         /**< Instant Type (Normal/Group/Join) */
    smlSortTableAttr          mSortTableAttr;       /**< Sort Table Attribute */
    ellCursorScrollability    mScrollable;          /**< Scroll 여부 설정 */

    qlncNodeCommon          * mChildNode;           /**< Child Node */

    qlncRefExprList         * mKeyColList;          /**< Key Column List */
    qlncRefExprList         * mRecColList;          /**< Record Column List */
    qlncRefExprList         * mReadColList;         /**< Read Column List */

    stlUInt8                * mKeyFlags;            /**< Key Flags */

    qlncAndFilter           * mKeyFilter;           /**< Sort Key에 대한 Filter */
    qlncAndFilter           * mRecFilter;           /**< Record에 대한 Filter */
    qlncAndFilter           * mFilter;              /**< Filter (KeyFilter + RecFilter) */

    /* Non Filter SubQuery Expression List */
    qlncRefExprList         * mNonFilterSubQueryExprList;

    /* Hint Info */
    qlncJoinHint            * mSemiJoinHint;        /**< Semi-Join/Anti-Semi-Join Hint */

    /* Cost Based Optimizer */
    qlncSortInstantCost     * mSortInstantCost;     /**< Sort Instant Cost */
};


/**
 * @brief Hash Instant Node
 */
struct qlncHashInstantNode
{
    qlncNodeCommon            mNodeCommon;          /**< Common Node Info */

    qlncInstantType           mInstantType;         /**< Instant Type (Normal/Group/Join) */
    smlIndexAttr              mHashTableAttr;       /**< Hash Table Attribute */
    ellCursorScrollability    mScrollable;          /**< Scroll 여부 설정 */

    qlncNodeCommon          * mChildNode;           /**< Child Node */

    qlncRefExprList         * mKeyColList;          /**< Key Column List */
    qlncRefExprList         * mRecColList;          /**< Record Column List */
    qlncRefExprList         * mReadColList;         /**< Read Column List */

    qlncAndFilter           * mKeyFilter;           /**< Hash Key에 대한 Filter */
    qlncAndFilter           * mRecFilter;           /**< Record에 대한 Filter */
    qlncAndFilter           * mFilter;              /**< Filter (KeyFilter + RecFilter) */

    /* Non Filter SubQuery Expression List */
    qlncRefExprList         * mNonFilterSubQueryExprList;

    /* Hint Info */
    qlncJoinHint            * mSemiJoinHint;        /**< Semi-Join/Anti-Semi-Join Hint */

    /* Cost Based Optimizer */
    qlncHashInstantCost     * mHashInstantCost;     /**< Hash Instant Cost */
    qlncGroupCost           * mGroupCost;           /**< Group Cost */
};


/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

/* Table Node */
stlStatus qlncInitializeNodeArray( knlRegionMem     * aRegionMem,
                                   qlncNodeArray    * aNodeArray,
                                   qllEnv           * aEnv );

stlStatus qlncAddNodeToNodeArray( qlncNodeArray     * aNodeArray,
                                  qlncNodeCommon    * aNode,
                                  qllEnv            * aEnv );

stlStatus qlncCreateTableMapArray( qlncParamInfo        * aParamInfo,
                                   qlncTableMapArray   ** aTableMapArray,
                                   qllEnv               * aEnv );

stlStatus qlncAddTableMapToTableMapArray( qlncTableMapArray    * aTableMapArray,
                                          qlvInitNode          * aInitNode,
                                          qlncNodeCommon       * aCandNode,
                                          qllEnv               * aEnv );

stlStatus qlncMergeTableMapArray( knlRegionMem          * aRegionMem,
                                  qlncTableMapArray     * aSrcTableMapArr1,
                                  qlncTableMapArray     * aSrcTableMapArr2,
                                  qlncTableMapArray    ** aMergedTableMapArr,
                                  qllEnv                * aEnv );

qlncNodeCommon * qlncGetCandNodePtrFromInitNodePtr( qlncTableMapArray   * aTableMapArray,
                                                    qlvInitNode         * aInitNode );

stlBool qlncIsExistCandNodeOnTableMap( qlncTableMapArray    * aTableMapArray,
                                       qlncNodeCommon       * aCandNode );

/* Table Node */
stlStatus qlncCreateQBMapArray( qlncParamInfo   * aParamInfo,
                                qlncQBMapArray ** aQBMapArray,
                                qllEnv          * aEnv );

stlStatus qlncAddQBMapToQBMapArray( qlncQBMapArray  * aQBMapArray,
                                    stlInt32          aQBID,
                                    qlvInitNode     * aInitNode,
                                    qlncNodeCommon  * aCandNode,
                                    qllEnv          * aEnv );

stlBool qlncFindQueryBlockInQBMapArray( qlncQBMapArray  * aQBMapArray,
                                        qlvInitNode     * aInitNode,
                                        stlInt32        * aQBID,
                                        qlncNodeCommon ** aCandNode );


/* Create Query Candidate Plan Nodes */
stlStatus qlncCreateQueryCandNode( qlncCreateNodeParamInfo  * aCreateNodeParamInfo,
                                   qllEnv                   * aEnv );

stlStatus qlncCreateQuerySetCandNode( qlncCreateNodeParamInfo   * aCreateNodeParamInfo,
                                      qllEnv                    * aEnv );

stlStatus qlncCreateDiffSetOPCandNode( qlncCreateNodeParamInfo  * aCreateNodeParamInfo,
                                       qllEnv                   * aEnv );

stlStatus qlncCreateSameSetOPCandNode( qlncCreateNodeParamInfo  * aCreateNodeParamInfo,
                                       qlncSetOP                * aRootSetOP,
                                       qlncSetOption              aParentSetOption,
                                       qllEnv                   * aEnv );

stlStatus qlncCreateQuerySpecCandNode( qlncCreateNodeParamInfo  * aCreateNodeParamInfo,
                                       qllEnv                   * aEnv );

/* Create Table Candidate Plan Nodes */
stlStatus qlncCreateTableCandNode( qlncCreateNodeParamInfo  * aCreateNodeParamInfo,
                                   qllEnv                   * aEnv );

stlStatus qlncCreateBaseCandNode( qlncCreateNodeParamInfo   * aCreateNodeParamInfo,
                                  qllEnv                    * aEnv );

stlStatus qlncCreateSubCandNode( qlncCreateNodeParamInfo    * aCreateNodeParamInfo,
                                 qllEnv                     * aEnv );

stlStatus qlncCreateInnerJoinCandNode( qlncCreateNodeParamInfo  * aCreateNodeParamInfo,
                                       qllEnv                   * aEnv );

stlStatus qlncCreateOuterJoinCandNode( qlncCreateNodeParamInfo  * aCreateNodeParamInfo,
                                       qllEnv                   * aEnv );

stlStatus qlncCreateSemiJoinCandNode( qlncCreateNodeParamInfo   * aCreateNodeParamInfo,
                                      qllEnv                    * aEnv );

stlStatus qlncCreateAntiSemiJoinCandNode( qlncCreateNodeParamInfo   * aCreateNodeParamInfo,
                                          qllEnv                    * aEnv );

stlStatus qlncCreateChildCandNodeFromInnerJoin( qlncCreateNodeParamInfo * aCreateNodeParamInfo,
                                                stlBool                   aHasNamedColumnList,
                                                qllEnv                  * aEnv );

stlStatus qlncCreateChildCandNodeFromNonInnerJoin( qlncCreateNodeParamInfo  * aCreateNodeParamInfo,
                                                   qllEnv                   * aEnv );

/* Create Instant Candidate Plan Nodes */
stlStatus qlncCreateGroupByCandNode( qlncCreateNodeParamInfo    * aCreateNodeParamInfo,
                                     qlvGroupBy                 * aGroupBy,
                                     qlncNodeCommon             * aChildNode,
                                     qllEnv                     * aEnv );

stlStatus qlncCreateDistinctCandNode( qlncCreateNodeParamInfo   * aCreateNodeParamInfo,
                                      qlvInitInstantNode        * aDistinct,
                                      qlncNodeCommon            * aChildNode,
                                      qllEnv                    * aEnv );

stlStatus qlncCreateOrderByCandNode( qlncCreateNodeParamInfo    * aCreateNodeParamInfo,
                                     qlvOrderBy                 * aOrderBy,
                                     qlncNodeCommon             * aChildNode,
                                     qllEnv                     * aEnv );

stlStatus qlncCreateSortInstantForMergeJoin( qlncParamInfo              * aParamInfo,
                                             qlncInstantType              aInstantType,
                                             smlSortTableAttr           * aSortTableAttr,
                                             ellCursorScrollability       aScrollable,
                                             stlInt32                     aSortKeyCount,
                                             qlncExprCommon            ** aSortKeyArr,
                                             stlUInt8                   * aSortKeyFlags,
                                             qlncNodeCommon             * aChildNode,
                                             qlncNodeCommon            ** aSortInstant,
                                             qllEnv                     * aEnv );

stlStatus qlncCreateHashInstantForHashJoin( qlncParamInfo           * aParamInfo,
                                            qlncInstantType           aInstantType,
                                            smlIndexAttr            * aHashTableAttr,
                                            ellCursorScrollability    aScrollable,
                                            stlInt32                  aHashKeyCount,
                                            qlncExprCommon         ** aHashKeyArr,
                                            qlncNodeCommon          * aChildNode,
                                            qlncNodeCommon         ** aHashInstant,
                                            qllEnv                  * aEnv );

stlStatus qlncCreateSortInstant( qlncParamInfo              * aParamInfo,
                                 qlncInstantType              aInstantType,
                                 smlSortTableAttr           * aSortTableAttr,
                                 ellCursorScrollability       aScrollable,
                                 qlncNodeCommon             * aChildNode,
                                 qlncSortInstantNode       ** aSortInstant,
                                 qllEnv                     * aEnv );

stlStatus qlncAddColumnOnSortInstant( qlncCreateNodeParamInfo   * aCreateNodeParamInfo,
                                      qlvRefExprList            * aColumnList,
                                      stlUInt8                  * aKeyFlags,
                                      stlInt16                    aColumnType,
                                      stlInt32                    aExprPhraseFlag,
                                      qlncSortInstantNode       * aSortInstant,
                                      qllEnv                    * aEnv );

stlStatus qlncCreateHashInstant( qlncParamInfo              * aParamInfo,
                                 qlncInstantType              aInstantType,
                                 smlIndexAttr               * aHashTableAttr,
                                 ellCursorScrollability       aScrollable,
                                 qlncNodeCommon             * aChildNode,
                                 qlncHashInstantNode       ** aHashInstant,
                                 qllEnv                     * aEnv );

stlStatus qlncAddColumnOnHashInstant( qlncCreateNodeParamInfo   * aCreateNodeParamInfo,
                                      qlvRefExprList            * aColumnList,
                                      stlInt16                    aColumnType,
                                      stlInt32                    aExprPhraseFlag,
                                      qlncHashInstantNode       * aHashInstant,
                                      qllEnv                    * aEnv );

void qlncSetFilterOnHashInstant( qlncHashInstantNode    * aHashInstant,
                                 qlncAndFilter          * aFilter );


/* Copy Candidate Plan Nodes */
stlStatus qlncCopyBaseNodeWithoutCBOInfo( qlncParamInfo     * aParamInfo,
                                          qlncNodeCommon    * aOrgNode,
                                          qlncNodeCommon   ** aCopiedNode,
                                          qllEnv            * aEnv );

stlStatus qlncCopySubNodeWithoutCBOInfo( qlncParamInfo      * aParamInfo,
                                         qlncNodeCommon     * aOrgNode,
                                         qlncNodeCommon    ** aCopiedNode,
                                         qllEnv             * aEnv );

stlStatus qlncCopyJoinNodeWithoutCBOInfo( qlncParamInfo     * aParamInfo,
                                          qlncNodeCommon    * aOrgNode,
                                          qlncNodeCommon   ** aCopiedNode,
                                          qllEnv            * aEnv );

stlStatus qlncCopyAllNodeWithoutCBOInfo( qlncParamInfo      * aParamInfo,
                                         qlncNodeCommon     * aOrgNode,
                                         qlncNodeCommon    ** aCopiedNode,
                                         qllEnv             * aEnv );

stlStatus qlncCopySortInstantNode( qlncParamInfo    * aParamInfo,
                                   qlncNodeCommon   * aOrgNode,
                                   qlncNodeCommon  ** aCopiedNode,
                                   qllEnv           * aEnv );

stlStatus qlncCopyHashInstantNode( qlncParamInfo    * aParamInfo,
                                   qlncNodeCommon   * aOrgNode,
                                   qlncNodeCommon  ** aCopiedNode,
                                   qllEnv           * aEnv );


stlStatus qlncCreateSubTable( qlncParamInfo     * aParamInfo,
                              qlncNodeCommon    * aQueryNode,
                              qlncNodeCommon   ** aOutNode,
                              qllEnv            * aEnv );

/* Query Optimizer */
stlStatus qlncQueryOptimizer( smlTransId              aTransID,
                              qllDBCStmt            * aDBCStmt,
                              qllStatement          * aSQLStmt,
                              qlvInitNode           * aQueryNode,
                              qlvInitStmtExprList   * aInitStmtExprList,
                              qloValidTblStatList   * aTableStatList,
                              stlInt32              * aInternalBindIdx,
                              qlncPlanCommon       ** aCandQueryPlan,
                              qllEnv                * aEnv );

stlStatus qlncSelectOptimizer( smlTransId                 aTransID,
                               qllDBCStmt               * aDBCStmt,
                               qllStatement             * aSQLStmt,
                               qloInitExprList          * aQueryExprList,
                               stlInt32                 * aInternalBindIdx,
                               qlncSelectStmtParamInfo  * aSelectStmtParamInfo,
                               qllEnv                   * aEnv );

stlStatus qlncInsertValuesOptimizer( smlTransId                 aTransID,
                                     qllDBCStmt               * aDBCStmt,
                                     qllStatement             * aSQLStmt,
                                     qloInitExprList          * aQueryExprList,
                                     stlInt32                 * aInternalBindIdx,
                                     qlncInsertStmtParamInfo  * aInsertStmtParamInfo,
                                     qllEnv                   * aEnv );

stlStatus qlncInsertSelectOptimizer( smlTransId                 aTransID,
                                     qllDBCStmt               * aDBCStmt,
                                     qllStatement             * aSQLStmt,
                                     qloInitExprList          * aQueryExprList,
                                     qloValidTblStatList      * aTableStatList,
                                     stlInt32                 * aInternalBindIdx,
                                     qlncInsertStmtParamInfo  * aInsertStmtParamInfo,
                                     qllEnv                   * aEnv );

stlStatus qlncInsertOptimizerInternal( smlTransId                 aTransID,
                                       qllDBCStmt               * aDBCStmt,
                                       qllStatement             * aSQLStmt,
                                       qloInitExprList          * aQueryExprList,
                                       qloValidTblStatList      * aTableStatList,
                                       stlInt32                 * aInternalBindIdx,
                                       qlncInsertStmtParamInfo  * aInsertStmtParamInfo,
                                       qllEnv                   * aEnv );

stlStatus qlncDeleteOptimizer( smlTransId                 aTransID,
                               qllDBCStmt               * aDBCStmt,
                               qllStatement             * aSQLStmt,
                               qloInitExprList          * aQueryExprList,
                               stlInt32                 * aInternalBindIdx,
                               qlncDeleteStmtParamInfo  * aDeleteStmtParamInfo,
                               qllEnv                   * aEnv );

stlStatus qlncUpdateOptimizer( smlTransId                 aTransID,
                               qllDBCStmt               * aDBCStmt,
                               qllStatement             * aSQLStmt,
                               qloInitExprList          * aQueryExprList,
                               stlInt32                 * aInternalBindIdx,
                               qlncUpdateStmtParamInfo  * aUpdateStmtParamInfo,
                               qllEnv                   * aEnv );

stlStatus qlncCopyUpdateRelObject( qlvInitUpdateRelObject  * aSrcRelObject,
                                   qlvInitUpdateRelObject  * aDestRelObject,
                                   knlRegionMem            * aRegionMem,
                                   qllEnv                  * aEnv );

stlStatus qlncCopyDeleteRelObject( qlvInitDeleteRelObject  * aSrcRelObject,
                                   qlvInitDeleteRelObject  * aDestRelObject,
                                   knlRegionMem            * aRegionMem,
                                   qllEnv                  * aEnv );

stlStatus qlncCopyInsertRelObject( qlvInitInsertRelObject  * aSrcRelObject,
                                   qlvInitInsertRelObject  * aDestRelObject,
                                   knlRegionMem            * aRegionMem,
                                   qllEnv                  * aEnv );


/*******************************************************************************
 * OTHER FUNCTIONS
 ******************************************************************************/

/* Statistic */
stlStatus qlncGatherBaseStatistic( qlncCreateNodeParamInfo  * aCreateNodeParamInfo,
                                   qllEnv                   * aEnv );

stlStatus qlncGatherTableStat( qlncCreateNodeParamInfo   * aCreateNodeParamInfo,
                               qllEnv                   * aEnv );

stlStatus qlncGatherIndexStat( qlncCreateNodeParamInfo  * aCreateNodeParamInfo,
                               qlncTableStat            * aTableStat,
                               qllEnv                   * aEnv );

stlStatus qlncGatherColumnStatFromUniqueIndex( qlncCreateNodeParamInfo  * aCreateNodeParamInfo,
                                               qllEnv                   * aEnv );

/* Hint Analyzer */
stlStatus qlncAnalyzeAccessPathHint( qlncCreateNodeParamInfo    * aCreateNodeParamInfo,
                                     qllEnv                     * aEnv );

stlStatus qlncAnalyzeJoinOrderHint( qlncCreateNodeParamInfo     * aCreateNodeParamInfo,
                                    qllEnv                      * aEnv );

stlStatus qlncAnalyzeJoinOperationHint( qlncCreateNodeParamInfo     * aCreateNodeParamInfo,
                                        qllEnv                      * aEnv );

stlStatus qlncAnalyzeQueryTransformHint( qlncCreateNodeParamInfo    * aCreateNodeParamInfo,
                                         qllEnv                     * aEnv );

stlStatus qlncAnalyzeOtherHint( qlncCreateNodeParamInfo * aCreateNodeParamInfo,
                                qllEnv                  * aEnv );

/* Optimizer */
stlStatus qlncQueryNodeOptimizer( qlncParamInfo     * aParamInfo,
                                  qlncNodeCommon    * aNode,
                                  qllEnv            * aEnv );

stlStatus qlncDMLNodeOptimizer( qlncParamInfo   * aParamInfo,
                                qlncNodeCommon  * aNode,
                                qlncNodeCommon ** aOutNode,
                                qllEnv          * aEnv );


/** @} qlnc */

#endif /* _QLNCNODE_H_ */
