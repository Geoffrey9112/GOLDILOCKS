/*******************************************************************************
 * qlncMakeCandPlan.h
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

#ifndef _QLNCMAKECANDPLAN_H_
#define _QLNCMAKECANDPLAN_H_ 1

/**
 * @file qlncMakeCandPlan.h
 * @brief SQL Processor Layer Make Candidate Plan
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
 * @brief Plan Type
 */
enum qlncPlanType
{
    QLNC_PLAN_TYPE_QUERY_SET = 0,       /**< Query Set */
    QLNC_PLAN_TYPE_SET_OP,              /**< Set OP */
    QLNC_PLAN_TYPE_QUERY_SPEC,          /**< Query Spec */

    QLNC_PLAN_TYPE_TABLE_SCAN,          /**< Table Scan */
    QLNC_PLAN_TYPE_INDEX_SCAN,          /**< Index Scan */
    QLNC_PLAN_TYPE_INDEX_COMBINE,       /**< Index Combine */
    QLNC_PLAN_TYPE_ROWID_SCAN,          /**< Rowid Scan */

    QLNC_PLAN_TYPE_SUB_TABLE,           /**< Sub Table */

    QLNC_PLAN_TYPE_NESTED_JOIN,         /**< Nested Join */
    QLNC_PLAN_TYPE_MERGE_JOIN,          /**< Merge Join */
    QLNC_PLAN_TYPE_HASH_JOIN,           /**< Hash Join */
    QLNC_PLAN_TYPE_JOIN_COMBINE,        /**< Join Combine */

    QLNC_PLAN_TYPE_SORT_INSTANT,        /**< Sort Instant */
    QLNC_PLAN_TYPE_SORT_GROUP_INSTANT,  /**< Sort Group Instant */
    QLNC_PLAN_TYPE_SORT_JOIN_INSTANT,   /**< Sort Join Instant */

    QLNC_PLAN_TYPE_HASH_INSTANT,        /**< Hash Instant */
    QLNC_PLAN_TYPE_HASH_GROUP_INSTANT,  /**< Hash Group Instant */
    QLNC_PLAN_TYPE_HASH_JOIN_INSTANT,   /**< Hash Join Instant */

    QLNC_PLAN_TYPE_SUB_QUERY_LIST,      /**< Sub Query List */
    QLNC_PLAN_TYPE_SUB_QUERY_FUNCTION,  /**< Sub Query Function */
    QLNC_PLAN_TYPE_SUB_QUERY_FILTER,    /**< Sub Query Filter */

    QLNC_PLAN_TYPE_GROUP,               /**< Group */

    QLNC_PLAN_TYPE_MAX
};


#define QLNC_PLAN_NODE_MAP_ARRAY_DEFAULT_SIZE   8   /**< Plan Node Map Dynamic Array의 기본 Array 개수 */
#define QLNC_PLAN_NODE_MAP_ARRAY_INCREMENT_SIZE 8   /**< Plan Node Map Dynamic Array의 증가 Array 개수 */


#define QLNC_MAKE_CAND_PLAN_BASIC_PARAM_VALIDATE( _aPlanBasicParamInfo, _aEnv )     \
{                                                                                   \
    STL_PARAM_VALIDATE( (_aPlanBasicParamInfo) != NULL, QLL_ERROR_STACK(_aEnv) );   \
    STL_PARAM_VALIDATE( (_aPlanBasicParamInfo)->mRegionMem != NULL,                 \
                        QLL_ERROR_STACK(_aEnv) );                                   \
    STL_PARAM_VALIDATE( (_aPlanBasicParamInfo)->mPlanNodeMapDynArr != NULL,         \
                        QLL_ERROR_STACK(_aEnv) );                                   \
}


#define QLNC_MAKE_CAND_PLAN_DETAIL_PARAM_VALIDATE( _aPlanDetailParamInfo, _aEnv )       \
{                                                                                       \
    STL_PARAM_VALIDATE( (_aPlanDetailParamInfo) != NULL, QLL_ERROR_STACK(_aEnv) );      \
    STL_PARAM_VALIDATE( (_aPlanDetailParamInfo)->mParamInfo.mDBCStmt != NULL,           \
                        QLL_ERROR_STACK(_aEnv) );                                       \
    STL_PARAM_VALIDATE( (_aPlanDetailParamInfo)->mParamInfo.mSQLStmt != NULL,           \
                        QLL_ERROR_STACK(_aEnv) );                                       \
    STL_PARAM_VALIDATE( (_aPlanDetailParamInfo)->mRegionMem != NULL,                    \
                        QLL_ERROR_STACK(_aEnv) );                                       \
    STL_PARAM_VALIDATE( (_aPlanDetailParamInfo)->mPlanNodeMapDynArr != NULL,            \
                        QLL_ERROR_STACK(_aEnv) );                                       \
    STL_PARAM_VALIDATE( (_aPlanDetailParamInfo)->mInitExprList != NULL,                 \
                        QLL_ERROR_STACK(_aEnv) );                                       \
    STL_PARAM_VALIDATE( (_aPlanDetailParamInfo)->mInitExprList->mStmtExprList != NULL,  \
                        QLL_ERROR_STACK(_aEnv) );                                       \
}


#define QLNC_MAKE_CAND_PLAN_NODE_PARAM_VALIDATE( _aNode, _aNodeType, _aBestOptType, _aEnv )     \
{                                                                                               \
    STL_PARAM_VALIDATE( (_aNode) != NULL, QLL_ERROR_STACK(_aEnv) );                             \
    STL_PARAM_VALIDATE( (_aNode)->mNodeType == (_aNodeType), QLL_ERROR_STACK(_aEnv) );          \
    STL_PARAM_VALIDATE( (_aNode)->mIsAnalyzedCBO == STL_TRUE, QLL_ERROR_STACK(_aEnv) );         \
    STL_PARAM_VALIDATE( (_aNode)->mBestOptType == (_aBestOptType), QLL_ERROR_STACK(_aEnv) );    \
    STL_PARAM_VALIDATE( (_aNode)->mBestCost != NULL, QLL_ERROR_STACK(_aEnv) );                  \
}


#define QLNC_SET_PLAN_COMMON( _aPlanCommon, _aPlanType, _aCandNode, _aInitNode, _aNeedRowBlock )\
{                                                                                               \
    (_aPlanCommon)->mPlanType = (_aPlanType);                                                   \
    (_aPlanCommon)->mCandNode = (_aCandNode);                                                   \
    (_aPlanCommon)->mInitNode = (_aInitNode);                                                   \
    (_aPlanCommon)->mNeedRowBlock = (_aNeedRowBlock);                                           \
    (_aPlanCommon)->mAggrExprList.mCount = 0;                                                   \
    (_aPlanCommon)->mAggrExprList.mHead = NULL;                                                 \
    (_aPlanCommon)->mAggrExprList.mTail = NULL;                                                 \
    (_aPlanCommon)->mNestedAggrExprList.mCount = 0;                                             \
    (_aPlanCommon)->mNestedAggrExprList.mHead = NULL;                                           \
    (_aPlanCommon)->mNestedAggrExprList.mTail = NULL;                                           \
}


#define QLNC_ALLOC_AND_INIT_MEMORY( _aRegionMem, _aVar, _aSize, _aEnv ) \
{                                                                       \
    STL_TRY( knlAllocRegionMem( (_aRegionMem),                          \
                                (_aSize),                               \
                                (void**) &(_aVar),                      \
                                KNL_ENV(_aEnv) )                        \
             == STL_SUCCESS );                                          \
    stlMemset( (_aVar), 0x00, (_aSize) );                               \
}


#define QLNC_ALLOC_AND_COPY_RELATION_NAME( _aRegionMem, _aCandRName, _aValidateRName, _aEnv )   \
{                                                                                               \
    STL_TRY( knlAllocRegionMem( (_aRegionMem),                                                  \
                                STL_SIZEOF( qlvRelationName ),                                  \
                                (void**) &(_aValidateRName),                                    \
                                KNL_ENV(aEnv) )                                                 \
             == STL_SUCCESS );                                                                  \
    QLNC_ALLOC_AND_COPY_STRING( (_aValidateRName)->mCatalog,                                    \
                                (_aCandRName)->mCatalog,                                        \
                                (_aRegionMem),                                                  \
                                (_aEnv) );                                                      \
    QLNC_ALLOC_AND_COPY_STRING( (_aValidateRName)->mSchema,                                     \
                                (_aCandRName)->mSchema,                                         \
                                (_aRegionMem),                                                  \
                                (_aEnv) );                                                      \
    QLNC_ALLOC_AND_COPY_STRING( (_aValidateRName)->mTable,                                      \
                                (_aCandRName)->mTable,                                          \
                                (_aRegionMem),                                                  \
                                (_aEnv) );                                                      \
}


#define QLNC_MAKE_INIT_EXPRESSION( _aRegionMem,                                 \
                                   _aExprType,                                  \
                                   _aRelatedFuncId,                             \
                                   _aIterationTime,                             \
                                   _aPosition,                                  \
                                   _aExprPhraseFlag,                            \
                                   _aChildExprCnt,                              \
                                   _aColumnName,                                \
                                   _aExprValue,                                 \
                                   _aInitExpr,                                  \
                                   _aEnv )                                      \
    {                                                                           \
        STL_TRY( knlAllocRegionMem( (_aRegionMem),                              \
                                    STL_SIZEOF( qlvInitExpression ),            \
                                    (void**) &(_aInitExpr),                     \
                                    KNL_ENV(_aEnv) )                            \
                 == STL_SUCCESS );                                              \
                                                                                \
        (_aInitExpr)->mType = (_aExprType);                                     \
        if( (_aExprPhraseFlag) & QLNC_EXPR_PHRASE_TARGET )                      \
        {                                                                       \
            (_aInitExpr)->mPhraseType = QLV_EXPR_PHRASE_TARGET;                 \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            (_aInitExpr)->mPhraseType = QLV_EXPR_PHRASE_CONDITION;              \
        }                                                                       \
        (_aInitExpr)->mRelatedFuncId = (_aRelatedFuncId);                       \
        (_aInitExpr)->mIterationTime = (_aIterationTime);                       \
                                                                                \
        QLV_VALIDATE_INCLUDE_EXPR_CNT( (_aChildExprCnt) + 1,                    \
                                       (_aEnv) );                               \
        (_aInitExpr)->mIncludeExprCnt = (_aChildExprCnt) + 1/* 자기자신 */;     \
        (_aInitExpr)->mPosition = (_aPosition);                                 \
        (_aInitExpr)->mOffset = 0;                                              \
        (_aInitExpr)->mIsSetOffset = STL_FALSE;                                 \
        (_aInitExpr)->mExpr.mExprValue = (void*)(_aExprValue);                  \
        QLNC_ALLOC_AND_COPY_STRING( (_aInitExpr)->mColumnName,                  \
                                    (_aColumnName),                             \
                                    (_aRegionMem),                              \
                                    (_aEnv) );                                  \
    }


#define QLNC_ADD_READ_COLUMN_MAP( _aRegionMem, _aReadColMapList, _aCandExpr, _aInitExpr, _aEnv )    \
{                                                                                                   \
    qlncReadColMap  * _sReadColMap  = NULL;                                                         \
                                                                                                    \
    STL_TRY( knlAllocRegionMem( (_aRegionMem),                                                      \
                                STL_SIZEOF( qlncReadColMap ),                                       \
                                (void**) &_sReadColMap,                                             \
                                KNL_ENV(_aEnv) )                                                    \
             == STL_SUCCESS );                                                                      \
    _sReadColMap->mCandExpr = (_aCandExpr);                                                         \
    _sReadColMap->mInitExpr = (_aInitExpr);                                                         \
    _sReadColMap->mNext = NULL;                                                                     \
                                                                                                    \
    if( (_aReadColMapList)->mCount == 0 )                                                           \
    {                                                                                               \
        (_aReadColMapList)->mHead = _sReadColMap;                                                   \
        (_aReadColMapList)->mTail = _sReadColMap;                                                   \
    }                                                                                               \
    else                                                                                            \
    {                                                                                               \
        (_aReadColMapList)->mTail->mNext = _sReadColMap;                                            \
        (_aReadColMapList)->mTail = _sReadColMap;                                                   \
    }                                                                                               \
    (_aReadColMapList)->mCount++;                                                                   \
}


#define QLNC_MAKE_INIT_REFERENCE( _aRegionMem, _aOrgExpr, _aInitExpr, _aEnv )   \
{                                                                               \
    qlvInitRefExpr  * _sInitRefExpr = NULL;                                     \
                                                                                \
    STL_TRY( knlAllocRegionMem( (_aRegionMem),                                  \
                                STL_SIZEOF( qlvInitRefExpr ),                   \
                                (void**) &_sInitRefExpr,                        \
                                KNL_ENV(_aEnv) )                                \
             == STL_SUCCESS );                                                  \
    _sInitRefExpr->mOrgExpr = (_aOrgExpr);                                      \
                                                                                \
    QLV_VALIDATE_INCLUDE_EXPR_CNT( (_aOrgExpr)->mIncludeExprCnt,                \
                                   (_aEnv) );                                   \
                                                                                \
    QLNC_MAKE_INIT_EXPRESSION( (_aRegionMem),                                   \
                               QLV_EXPR_TYPE_REFERENCE,                         \
                               KNL_BUILTIN_FUNC_INVALID,                        \
                               (_aOrgExpr)->mIterationTime,                     \
                               (_aOrgExpr)->mPosition,                          \
                               QLV_EXPR_PHRASE_CONDITION,                       \
                               (_aOrgExpr)->mIncludeExprCnt,                    \
                               (_aOrgExpr)->mColumnName,                        \
                               _sInitRefExpr,                                   \
                               (_aInitExpr),                                    \
                               (_aEnv) );                                       \
}


#define QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO( _aConvExprParamInfo,                        \
                                                _aParamInfo,                                \
                                                _aRegionMem,                                \
                                                _aCandidateMem,                             \
                                                _aInitExprList,                             \
                                                _aTableStatList,                            \
                                                _aJoinLeftOuterInfoCount,                   \
                                                _aJoinLeftOuterInfoHead,                    \
                                                _aJoinRightOuterInfoCount,                  \
                                                _aJoinRightOuterInfoHead,                   \
                                                _aCurPlan,                                  \
                                                _aPlanNodeMapDynArr,                        \
                                                _aSubQueryExprList,                         \
                                                _aIsRefReadCol,                             \
                                                _aCandExpr )                                \
{                                                                                           \
    (_aConvExprParamInfo)->mParamInfo               = (_aParamInfo);                        \
    (_aConvExprParamInfo)->mRegionMem               = (_aRegionMem);                        \
    (_aConvExprParamInfo)->mCandidateMem            = (_aCandidateMem);                     \
    (_aConvExprParamInfo)->mInitExprList            = (_aInitExprList);                     \
    (_aConvExprParamInfo)->mTableStatList           = (_aTableStatList);                    \
    (_aConvExprParamInfo)->mJoinLeftOuterInfoList.mCount    = (_aJoinLeftOuterInfoCount);   \
    (_aConvExprParamInfo)->mJoinLeftOuterInfoList.mHead     = (_aJoinLeftOuterInfoHead);    \
    (_aConvExprParamInfo)->mJoinRightOuterInfoList.mCount   = (_aJoinRightOuterInfoCount);  \
    (_aConvExprParamInfo)->mJoinRightOuterInfoList.mHead    = (_aJoinRightOuterInfoHead);   \
    (_aConvExprParamInfo)->mCurPlan                 = (_aCurPlan);                          \
    (_aConvExprParamInfo)->mCandExpr                = (_aCandExpr);                         \
    (_aConvExprParamInfo)->mInitExpr                = NULL;                                 \
    (_aConvExprParamInfo)->mSubQueryExprList        = (_aSubQueryExprList);                 \
    (_aConvExprParamInfo)->mPlanNodeMapDynArr       = (_aPlanNodeMapDynArr);                \
    (_aConvExprParamInfo)->mSubQueryAndFilter       = NULL;                                 \
    (_aConvExprParamInfo)->mIsRefReadCol            = (_aIsRefReadCol);                     \
}


#define QLNC_CONVERT_EXPRESSION_DEFAULT_PARAM_VALIDATE( _aConvExprParamInfo, _aExprType, _aEnv )    \
{                                                                                                   \
    STL_PARAM_VALIDATE( (_aConvExprParamInfo) != NULL, QLL_ERROR_STACK(_aEnv) );                    \
    STL_PARAM_VALIDATE( (_aConvExprParamInfo)->mRegionMem != NULL, QLL_ERROR_STACK(_aEnv) );        \
    STL_PARAM_VALIDATE( (_aConvExprParamInfo)->mInitExprList != NULL, QLL_ERROR_STACK(_aEnv) );     \
    STL_PARAM_VALIDATE( (_aConvExprParamInfo)->mInitExprList->mStmtExprList != NULL,                \
                        QLL_ERROR_STACK(_aEnv) );                                                   \
    STL_PARAM_VALIDATE( (_aConvExprParamInfo)->mCurPlan != NULL, QLL_ERROR_STACK(_aEnv) );          \
    STL_PARAM_VALIDATE( (_aConvExprParamInfo)->mCandExpr != NULL, QLL_ERROR_STACK(_aEnv) );         \
    STL_PARAM_VALIDATE( (_aConvExprParamInfo)->mCandExpr->mType == (_aExprType),                    \
                        QLL_ERROR_STACK(_aEnv) );                                                   \
}


#define QLNC_ALLOC_AND_COPY_DICT_HANDLE( _aRegionMem, _aSrcHdl, _aDestHdl, _aEnv )      \
    {                                                                                   \
        if( (_aSrcHdl) == NULL )                                                        \
        {                                                                               \
            (_aDestHdl) = NULL;                                                         \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            STL_TRY( knlAllocRegionMem( (_aRegionMem),                                  \
                                        STL_SIZEOF( ellDictHandle ),                    \
                                        (void**) &(_aDestHdl),                          \
                                        KNL_ENV(aEnv) )                                 \
                     == STL_SUCCESS );                                                  \
                                                                                        \
            stlMemcpy( (_aDestHdl),                                                     \
                       (_aSrcHdl),                                                      \
                       STL_SIZEOF( ellDictHandle ) );                                   \
        }                                                                               \
    }


/*******************************************************************************
 * STRUCTURES
 ******************************************************************************/

/**
 * @brief Plan Node Map Dynamic Array
 */
struct qlncPlanNodeMapDynArray
{
    knlRegionMem    * mRegionMem;
    stlInt32          mCurCnt;
    stlInt32          mMaxCnt;
    qlncPlanCommon ** mPlanNodeArr;
};


/**
 * @brief Read Column Map
 */
struct qlncReadColMap
{
    qlncExprCommon      * mCandExpr;
    qlvInitExpression   * mInitExpr;
    qlncReadColMap      * mNext;
};


/**
 * @brief Read Column Map List
 */
struct qlncReadColMapList
{
    stlInt32          mCount;
    qlncReadColMap  * mHead;
    qlncReadColMap  * mTail;
};


/**
 * @brief Join Outer Info Item
 */
struct qlncJoinOuterInfoItem
{
    qlncPlanCommon          * mOuterPlanNode;       /**< Join 등에서 Outer Column 설정이 필요한 Plan Node */
    qlvRefExprList          * mOuterColumnList;     /**< Outer Plan Node에 의해 Outer Column으로 변경된 Expression List */
    qlncJoinOuterInfoItem   * mNext;
};


/**
 * @brief Join Outer Info List
 */
struct qlncJoinOuterInfoList
{
    stlInt32                  mCount;
    qlncJoinOuterInfoItem   * mHead;
};


/**
 * @brief Plan Node Map Dynamic Array
 */
struct qlncSubQueryAndFilter
{
    qlvInitExpression  ** mInitSubQueryFilter;   /**< SubQuery And Filter : Init Expr */
    qlvRefExprList     ** mSubQueryExprList;     /**< SubQuery List In SubQuery And Filter */
    stlInt32              mAndFilterCnt;         /**< SubQuery And Filter Count */
};


/**
 * @brief Candidate Plan Basic Parameter Info
 */
struct qlncPlanBasicParamInfo
{
    knlRegionMem            * mRegionMem;           /**< Region Mem (상위에서 사용할 메모리 결정( Candidate Mem / Code Mem ) */
    knlRegionMem            * mCandidateMem;        /**< Candidate Region Mem  Mem */
    qlncPlanNodeMapDynArray * mPlanNodeMapDynArr;   /**< Plan Node Map Dynamic Array */
    qlncPlanQuerySet        * mRootQuerySetPlanNode;/**< Root Query Set Plan Node (for Query Set) */
    qlvInitNode             * mInitQuerySetNode;    /**< Init Query Set Node (for Set OP Only) */
    stlBool                   mNeedRowBlock;        /**< Row Block이 필요한지 여부 */
};


/**
 * @brief Candidate Plan Detail Parameter Info
 */
struct qlncPlanDetailParamInfo
{
    qlncParamInfo             mParamInfo;           /**< Basic Parameter Info */
    knlRegionMem            * mRegionMem;           /**< Region Mem (상위에서 사용할 메모리 결정( Candidate Mem / Code Mem ) */
    knlRegionMem            * mCandidateMem;       /**< Candidate Region Mem  Mem */
    qlncPlanNodeMapDynArray * mPlanNodeMapDynArr;   /**< Plan Node Map Dynamic Array */
    qlvInitExprList         * mInitExprList;        /**< Init Expression List */
    qloValidTblStatList     * mTableStatList;       /**< Table Stat List */

    qlncJoinOuterInfoList     mJoinOuterInfoList;   /**< Join의 Outer Column을 위한 정보 리스트 */
};


/**
 * @brief Convert Expr Parameter Info
 */
struct qlncConvExprParamInfo
{
    qlncParamInfo           * mParamInfo;           /**< Basic Parameter Info */
    knlRegionMem            * mRegionMem;           /**< Region Mem (상위에서 사용할 메모리 결정( Candidate Mem / Code Mem ) */
    knlRegionMem            * mCandidateMem;        /**< Candidate Region Mem  Mem */
    qlvInitExprList         * mInitExprList;        /**< Init Expression List */
    qloValidTblStatList     * mTableStatList;       /**< Table Stat List */

    qlncJoinOuterInfoList     mJoinLeftOuterInfoList;   /**< Join의 Outer Column을 위한 정보 리스트 */
    qlncJoinOuterInfoList     mJoinRightOuterInfoList;  /**< Join의 Outer Column을 위한 정보 리스트 */

    qlncPlanCommon          * mCurPlan;                 /**< Current Plan Node */
    qlncExprCommon          * mCandExpr;                /**< Candidate Expression (Input) */
    qlvInitExpression       * mInitExpr;                /**< Init Expression (Output) */

    qlvRefExprList          * mSubQueryExprList;        /**< SubQuery List (for SubQuery Only) (Input/Output) */
    qlncPlanNodeMapDynArray * mPlanNodeMapDynArr;       /**< Plan Node Map Dynamic Array (for SubQuery Only) */
    qlncSubQueryAndFilter   * mSubQueryAndFilter;       /**< SubQuery를 갖는 And Filter (Output) */

    stlBool                   mIsRefReadCol;            /**< Instant 참조시 Read Column으로부터 Expression을 참조할지 여부 (for Instant Only) */
};


/**
 * @brief Insert Statement Parameter Info
 */
struct qlncPlanInsertStmtParamInfo
{
    qlncNodeCommon      * mWriteTableNode;
    qlncRefExprList     * mWriteColumnList;
    stlInt32              mMultiValueCount;
    qlncExprCommon     ** mRowDefaultExprArr;
    stlInt32              mInsertColumnCount;
    qlncRefExprList     * mInsertColExprList;
    qlncExprCommon    *** mRowValueArray;
    stlInt32              mReturnTargetCount;
    qlncTarget          * mReturnTargetArray;
    qlncRefExprList     * mRowSubQueryExprList;
    qlncRefExprList     * mReturnSubQueryExprList;

    /* Output */
    qlvInitTarget       * mOutReturnTargetArray;        /**< Return Target Array */
    qlvRefExprList      * mOutInsertOuterColumnList;    /**< Insert Outer Column List */
    qlvRefExprList      * mOutReturnOuterColumnList;    /**< Return Outer Column List */
    qlvInitExpression  ** mOutRowDefaultExprArr;        /**< Row Default Expression Array */
    qlvInitSingleRow    * mOutRowList;                  /**< Insert Value Row List */
    qlvRefExprList      * mOutInsertColExprList;        /**< Insert Column Expression List */
    qlncPlanCommon      * mOutPlanWriteTable;           /**< Write Table Candidate Plan Node */
    qlncPlanCommon      * mOutPlanRowSubQueryList;      /**< Row SubQuery List Candidate Plan Node */
    qlncPlanCommon      * mOutPlanReturnSubQueryList;   /**< Return SubQuery List Candidate Plan Node */
};


/**
 * @brief Update Statement Parameter Info
 */
struct qlncPlanUpdateStmtParamInfo
{
    qlncNodeCommon      * mReadTableNode;
    qlncNodeCommon      * mWriteTableNode;
    qlncRefExprList     * mReadColumnList;
    qlncRefExprList     * mWriteColumnList;
    stlInt32              mSetColumnCount;
    qlncRefExprList     * mSetColExprList;
    qlncExprCommon     ** mSetValueArray;
    stlInt32              mReturnTargetCount;
    qlncTarget          * mReturnTargetArray;
    qlncRefExprList     * mShareReadColumnList;
    qlncRefExprList     * mShareWriteColumnList;
    qlncRefExprList     * mSetSubQueryExprList;
    qlncRefExprList     * mReturnSubQueryExprList;
    stlBool               mIsReturnNew;

    /* Output */
    qlvInitTarget       * mOutReturnTargetArray;        /**< Return Target Array */
    qlvRefExprList      * mOutSetOuterColumnList;       /**< Set Outer Column List */
    qlvRefExprList      * mOutReturnOuterColumnList;    /**< Return Outer Column List */
    qlvInitExpression  ** mOutSetExprArray;             /**< Set Column Value Expression List */
    qlvRefExprList      * mOutSetColExprList;           /**< Set Column Expression List */
    qlvRefExprList      * mOutShareReadColList;         /**< Share Read Column List */
    qlvRefExprList      * mOutShareWriteColList;        /**< Share Write Column List */
    qlncPlanCommon      * mOutPlanRootNode;             /**< Root Plan Node */
    qlncPlanCommon      * mOutPlanReadTable;            /**< Read Table Candidate Plan Node */
    qlncPlanCommon      * mOutPlanWriteTable;           /**< Write Table Candidate Plan Node */
    qlncPlanCommon      * mOutPlanSetSubQueryList;      /**< Set SubQuery List Candidate Plan Node */
    qlncPlanCommon      * mOutPlanReturnSubQueryList;   /**< Return SubQuery List Candidate Plan Node */
};


/**
 * @brief Candidate Plan Common
 */
struct qlncPlanCommon
{
    qlncPlanType          mPlanType;            /**< Plan Type */

    stlBool               mNeedRowBlock;        /**< Row Block이 필요한지 여부 */

    qlncNodeCommon      * mCandNode;            /**< Candidate Node */
    qlvInitNode         * mInitNode;            /**< Validation Node */

    qlvRefExprList        mAggrExprList;        /**< Aggregation Expression List */
    qlvRefExprList        mNestedAggrExprList;  /**< Nested Aggregation Expression List */
};


/**
 * @brief Query Set Candidate Plan
 */
struct qlncPlanQuerySet
{
    qlncPlanCommon        mCommon;          /**< Candidate Plan Common */

    stlBool               mIsRootSet;       /**< Root Set 여부 */
    stlBool               mNeedRebuild;     /**< Rebuild가 필요한지 여부 */

    stlInt32              mTargetCount;     /**< Target Count */
    qlvRefExprList      * mTargetList;      /**< Target List */
    qlvRefExprList      * mColumnList;      /**< Column List */
    qlvInitTarget       * mSetTargets;      /**< Set Expression Target List : array */

    qlncPlanCommon      * mChildPlanNode;   /**< Child Candidate Plan Node */

    qlvInitExpression   * mOffset;          /**< Offset(Skip) Expression */
    qlvInitExpression   * mLimit;           /**< Limit(Fetch) Expression */

    qlncPlanQuerySet    * mRootQuerySet;    /**< Root Query Set (for Target, Column) */
    qlncReadColMapList  * mReadColMapList;
    qlncReadColMapList  * mInternalReadColMapList;
};


/**
 * @brief Set OP Candidate Plan
 */
struct qlncPlanSetOP
{
    qlncPlanCommon        mCommon;          /**< Candidate Plan Common */

    stlBool               mNeedRebuild;     /**< Rebuild가 필요한지 여부 */
    
    qlvSetType            mSetType;         /**< Set Operator Type */
    qlvSetOption          mSetOption;       /**< Set Option */

    stlInt32              mChildCount;      /**< Child Count */
    qlncPlanCommon     ** mChildPlanNodeArr;/**< Child Plan Node Array */

    qlncPlanQuerySet    * mRootQuerySet;    /**< Root Query Set (for Target, Column) */
    qlncReadColMapList  * mReadColMapList;
};


/**
 * @brief Query Spec Candidate Plan
 */
struct qlncPlanQuerySpec
{
    qlncPlanCommon        mCommon;          /**< Candidate Plan Common */

    stlBool               mIsDistinct;      /**< Distinct 여부 */

    /* Child Node */
    qlncPlanCommon      * mChildPlanNode;   /**< Child Candidate Plan Node */

    /* Target 관련 */
    stlInt32              mTargetCount;     /**< Target Count */
    qlvRefExprList      * mTargetColList;   /**< Target Column List */

    /* Offset/Limit */
    qlvInitExpression   * mOffset;          /**< Offset(Skip) Expression */
    qlvInitExpression   * mLimit;           /**< Limit(Fetch) Expression */

    qlvInitExprList     * mInitExprList;

    qlncReadColMapList  * mReadColMapList;
};


/**
 * @brief Table Scan Candidate Plan
 */
struct qlncPlanTableScan
{
    qlncPlanCommon        mCommon;          /**< Candidate Plan Common */

    ellDictHandle       * mTableHandle;
    void                * mPhysicalHandle;
    stlChar             * mDumpOption;

    qlvRefExprList      * mReadColList;
    qlvInitExpression   * mRowidColumn;

    qlvRefExprList      * mOuterColumnList;

    qlvInitExpression   * mFilterExpr;

    qlncReadColMapList  * mReadColMapList;
};


/**
 * @brief Index Scan Candidate Plan
 */
struct qlncPlanIndexScan
{
    qlncPlanCommon        mCommon;          /**< Candidate Plan Common */

    ellDictHandle       * mTableHandle;
    void                * mTablePhysicalHandle;

    ellDictHandle       * mIndexHandle;
    void                * mIndexPhysicalHandle;
    stlBool               mIsIndexScanAsc;

    stlInt32              mListColMapCount; /**< List Column Expression Count for IN Key Range */
    qlvInitExpression  ** mValueExpr;       /**< Value Expression Array for IN Key Range */
    qlvInitExpression  ** mListColExpr;     /**< List Column Expression Array for IN Key Range */

    qlvRefExprList      * mReadColList;
    qlvInitExpression   * mRowidColumn;

    qlvRefExprList      * mOuterColumnList;

    qlvInitExpression   * mFilterExpr;

    qlncReadColMapList  * mReadColMapList;
};


/**
 * @brief Rowid Scan Candidate Plan
 */
struct qlncPlanRowidScan
{
    qlncPlanCommon        mCommon;          /**< Candidate Plan Common */

    ellDictHandle       * mTableHandle;
    void                * mPhysicalHandle;

    qlvRefExprList      * mReadColList;
    qlvInitExpression   * mRowidColumn;

    qlvRefExprList      * mOuterColumnList;

    qlvInitExpression   * mFilterExpr;

    qlncReadColMapList  * mReadColMapList;
};


/**
 * @brief Index Combine Candidate Plan
 */
struct qlncPlanIndexCombine
{
    qlncPlanCommon        mCommon;          /**< Candidate Plan Common */

    ellDictHandle       * mTableHandle;

    stlInt32              mChildCount;
    qlncPlanCommon     ** mChildPlanArr;

    qlvRefExprList      * mReadColList;
    qlvInitExpression   * mRowidColumn;

    qlvRefExprList      * mOuterColumnList;

    qlncReadColMapList  * mReadColMapList;
};


/**
 * @brief Sub Table Candidate Plan
 */
struct qlncPlanSubTable
{
    qlncPlanCommon        mCommon;          /**< Candidate Plan Common */

    stlBool               mHasHandle;
    ellDictHandle         mViewHandle;

    qlncPlanCommon      * mChildPlanNode;

    qlvRefExprList      * mReadColList;

    qlvRefExprList      * mOuterColumnList;

    qlvInitExpression   * mFilterExpr;

    qlncReadColMapList  * mReadColMapList;
};


/**
 * @brief Nested Join Candidate Plan
 */
struct qlncPlanNestedJoin
{
    qlncPlanCommon        mCommon;              /**< Candidate Plan Common */

    qlncPlanCommon      * mLeftChildPlanNode;
    qlncPlanCommon      * mRightChildPlanNode;

    qlvJoinType           mJoinType;

    qlvRefExprList      * mReadColList;

    qlvRefExprList      * mJoinLeftOuterColumnList;
    qlvRefExprList      * mJoinRightOuterColumnList;
    qlvRefExprList      * mWhereFilterOuterColumnList;
    qlvRefExprList      * mOuterColumnList;

    qlvInitExpression   * mJoinFilterExpr;
    qlvInitExpression   * mSubQueryFilterExpr;  /* 실제 사용 안함 */
    qlvInitExpression   * mWhereFilterExpr;

    /* Full Outer Join에서 Join Condition에 존재하는 SubQuery Filter를 처리하기 위한 변수들 */
    qlncPlanCommon      * mStmtSubQueryPlanNode;

    qlncPlanCommon         ** mSubQueryPlanNodes;
    qlncSubQueryAndFilter   * mSubQueryAndFilter;

    qlncReadColMapList  * mReadColMapList;
};


/**
 * @brief Merge Join Candidate Plan
 */
struct qlncPlanMergeJoin
{
    qlncPlanCommon        mCommon;              /**< Candidate Plan Common */

    qlncPlanCommon      * mLeftChildPlanNode;
    qlncPlanCommon      * mRightChildPlanNode;

    qlvJoinType           mJoinType;

    qlvRefExprList      * mLeftSortKeyList;
    qlvRefExprList      * mRightSortKeyList;

    qlvRefExprList      * mReadColList;

    qlvRefExprList      * mJoinLeftOuterColumnList;
    qlvRefExprList      * mJoinRightOuterColumnList;
    qlvRefExprList      * mOuterColumnList;

    qlvInitExpression   * mJoinFilterExpr;
    qlvInitExpression   * mWhereFilterExpr;

    qlncReadColMapList  * mReadColMapList;
};


/**
 * @brief Hash Join Candidate Plan
 */
struct qlncPlanHashJoin
{
    qlncPlanCommon        mCommon;              /**< Candidate Plan Common */

    qlncPlanCommon      * mLeftChildPlanNode;
    qlncPlanCommon      * mRightChildPlanNode;

    qlvJoinType           mJoinType;

    qlvRefExprList      * mLeftHashKeyList;

    qlvRefExprList      * mReadColList;

    qlvRefExprList      * mJoinLeftOuterColumnList;
    qlvRefExprList      * mJoinRightOuterColumnList;
    qlvRefExprList      * mOuterColumnList;

    qlvInitExpression   * mJoinFilterExpr;
    qlvInitExpression   * mWhereFilterExpr;

    qlncReadColMapList  * mReadColMapList;
};


/**
 * @brief Join Combine Candidate Plan
 */
struct qlncPlanJoinCombine
{
    qlncPlanCommon        mCommon;          /**< Candidate Plan Common */

    stlInt32              mChildCount;
    qlncPlanCommon     ** mChildPlanArr;

    qlvRefExprList      * mReadColList;

    qlvRefExprList      * mOuterColumnList;

    qlncReadColMapList  * mReadColMapList;
};


/**
 * @brief Sort Instant Candidate Plan
 */
struct qlncPlanSortInstant
{
    qlncPlanCommon        mCommon;              /**< Candidate Plan Common */

    qlncPlanCommon      * mChildPlanNode;

    stlBool               mNeedRebuild;

    smlSortTableAttr      mSortTableAttr;
    stlBool               mScrollable;

    qlvRefExprList      * mKeyColList;
    qlvRefExprList      * mRecColList;
    qlvRefExprList      * mReadColList;

    stlUInt8            * mKeyFlags;

    qlvRefExprList      * mOuterColumnList;

    qlvInitExpression   * mFilterExpr;

    qlncReadColMapList  * mKeyColMapList;
    qlncReadColMapList  * mRecColMapList;
    qlncReadColMapList  * mReadColMapList;
};


/**
 * @brief Hash Instant Candidate Plan
 */
struct qlncPlanHashInstant
{
    qlncPlanCommon        mCommon;              /**< Candidate Plan Common */

    qlncPlanCommon      * mChildPlanNode;

    stlBool               mNeedRebuild;

    smlIndexAttr          mHashTableAttr;
    stlBool               mScrollable;

    qlvRefExprList      * mKeyColList;
    qlvRefExprList      * mRecColList;
    qlvRefExprList      * mReadColList;

    qlvRefExprList      * mOuterColumnList;

    qlvInitExpression   * mHashKeyFilterExpr;
    qlvInitExpression   * mNonHashKeyFilterExpr;

    qlncReadColMapList  * mKeyColMapList;
    qlncReadColMapList  * mRecColMapList;
    qlncReadColMapList  * mReadColMapList;
};


/**
 * @brief SubQuery List Candidate Plan
 */
struct qlncPlanSubQueryList
{
    qlncPlanCommon        mCommon;              /**< Candidate Plan Common */

    qlncPlanCommon      * mChildPlanNode;

    stlInt32              mSubQueryCnt;
    qlncPlanCommon     ** mSubQueryPlanNodes;

    qlvRefExprList      * mSubQueryExprList;

    qlvRefExprList      * mOuterColumnList;
};


/**
 * @brief SubQuery Function Candidate Plan
 */
struct qlncPlanSubQueryFunc
{
    qlncPlanCommon        mCommon;                /**< Candidate Plan Common */

    qlncPlanCommon      * mRightOperandPlanNode;

    qlvInitExpression   * mFuncExpr;
    qlvInitExpression   * mResultExpr;
};


/**
 * @brief SubQuery Filter Candidate Plan
 */
struct qlncPlanSubQueryFilter
{
    qlncPlanCommon            mCommon;              /**< Candidate Plan Common */

    qlncPlanCommon         ** mChildPlanNodes;

    qlncSubQueryAndFilter   * mAndFilter;
};


/**
 * @brief Group Candidate Plan
 */
struct qlncPlanGroup
{
    qlncPlanCommon        mCommon;              /**< Candidate Plan Common */

    qlncPlanCommon      * mChildPlanNode;

    qlvRefExprList      * mKeyColList;
    qlvRefExprList      * mRecColList;
    qlvRefExprList      * mReadColList;

    qlvRefExprList      * mOuterColumnList;

    qlvInitExpression   * mGroupKeyFilterExpr;
    qlvInitExpression   * mNonGroupKeyFilterExpr;

    qlncReadColMapList  * mKeyColMapList;
    qlncReadColMapList  * mRecColMapList;
    qlncReadColMapList  * mReadColMapList;
};


/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

/* Expression */
typedef stlStatus (*qlncConvertCandExprToInitExprFuncPtr) ( qlncConvExprParamInfo   * aConvExprParamInfo, 
                                                            qllEnv                  * aEnv );

extern const qlncConvertCandExprToInitExprFuncPtr qlncConvertCandExprToInitExprFuncList[ QLNC_EXPR_TYPE_MAX ];

stlStatus qlncConvertCandExprToInitExprValue( qlncConvExprParamInfo * aConvExprParamInfo,
                                              qllEnv                * aEnv );

stlStatus qlncConvertCandExprToInitExprBindParam( qlncConvExprParamInfo * aConvExprParamInfo,
                                                  qllEnv                * aEnv );

stlStatus qlncConvertCandExprToInitExprColumn( qlncConvExprParamInfo    * aConvExprParamInfo,
                                               qllEnv                   * aEnv );

stlStatus qlncConvertCandExprToInitExprFunction( qlncConvExprParamInfo  * aConvExprParamInfo,
                                                 qllEnv                 * aEnv );

stlStatus qlncConvertCandExprToInitExprCast( qlncConvExprParamInfo  * aConvExprParamInfo,
                                             qllEnv                 * aEnv );

stlStatus qlncConvertCandExprToInitExprPseudoColumn( qlncConvExprParamInfo  * aConvExprParamInfo,
                                                     qllEnv                 * aEnv );

stlStatus qlncConvertCandExprToInitExprPseudoArgument( qlncConvExprParamInfo    * aConvExprParamInfo,
                                                       qllEnv                   * aEnv );

stlStatus qlncConvertCandExprToInitExprConstantExprResult( qlncConvExprParamInfo    * aConvExprParamInfo,
                                                           qllEnv                   * aEnv );

stlStatus qlncConvertCandExprToInitExprSubQuery( qlncConvExprParamInfo  * aConvExprParamInfo,
                                                 qllEnv                 * aEnv );

stlStatus qlncConvertCandExprToInitExprReference( qlncConvExprParamInfo * aConvExprParamInfo,
                                                  qllEnv                * aEnv );

stlStatus qlncConvertCandExprToInitExprInnerColumn( qlncConvExprParamInfo   * aConvExprParamInfo,
                                                    qllEnv                  * aEnv );

stlStatus qlncConvertCandExprToInitExprOuterColumn( qlncConvExprParamInfo   * aConvExprParamInfo,
                                                    qllEnv                  * aEnv );

stlStatus qlncConvertCandExprToInitExprRowidColumn( qlncConvExprParamInfo   * aConvExprParamInfo,
                                                    qllEnv                  * aEnv );

stlStatus qlncConvertCandExprToInitExprAggregation( qlncConvExprParamInfo   * aConvExprParamInfo,
                                                    qllEnv                  * aEnv );

stlStatus qlncConvertCandExprToInitExprCaseExpr( qlncConvExprParamInfo  * aConvExprParamInfo,
                                                 qllEnv                 * aEnv );

stlStatus qlncConvertCandExprToInitExprListFunction( qlncConvExprParamInfo  * aConvExprParamInfo,
                                                     qllEnv                 * aEnv );

stlStatus qlncConvertCandExprToInitExprListColumn( qlncConvExprParamInfo    * aConvExprParamInfo,
                                                   qllEnv                   * aEnv );

stlStatus qlncConvertCandExprToInitExprRowExpr( qlncConvExprParamInfo   * aConvExprParamInfo,
                                                qllEnv                  * aEnv );

stlStatus qlncConvertCandExprToInitExprAndFilter( qlncConvExprParamInfo * aConvExprParamInfo,
                                                  qllEnv                * aEnv );

stlStatus qlncConvertCandExprToInitExprOrFilter( qlncConvExprParamInfo  * aConvExprParamInfo,
                                                 qllEnv                 * aEnv );

stlStatus qlncConvertCandExprToInitExprBooleanFilter( qlncConvExprParamInfo * aConvExprParamInfo,
                                                      qllEnv                * aEnv );

stlStatus qlncConvertCandExprToInitExprConstBooleanFilter( qlncConvExprParamInfo * aConvExprParamInfo,
                                                           qllEnv               * aEnv );

stlStatus qlncConvertCandExprToInitExprRootAndFilter( qlncConvExprParamInfo * aConvExprParamInfo,
                                                      qllEnv                * aEnv );

stlStatus qlncAddReadColumnOnCurPlan( qlncConvExprParamInfo * aConvExprParamInfo,
                                      qllEnv                * aEnv );

stlStatus qlncAddReadColumnOnCurPlan_SetOP( qlncConvExprParamInfo   * aConvExprParamInfo,
                                            qllEnv                  * aEnv );

stlStatus qlncAddReadColumnOnCurPlan_BaseTable( qlncConvExprParamInfo   * aConvExprParamInfo,
                                                qllEnv                  * aEnv );

stlStatus qlncAddReadColumnOnCurPlan_IndexCombine( qlncConvExprParamInfo    * aConvExprParamInfo,
                                                   qllEnv                   * aEnv );

stlStatus qlncAddReadColumnOnCurPlan_SubTable( qlncConvExprParamInfo    * aConvExprParamInfo,
                                               qllEnv                   * aEnv );

stlStatus qlncAddReadColumnOnCurPlan_JoinCombine( qlncConvExprParamInfo * aConvExprParamInfo,
                                                  qllEnv                * aEnv );

stlStatus qlncAddReadColumnOnCurPlan_JoinTable( qlncConvExprParamInfo   * aConvExprParamInfo,
                                                qllEnv                  * aEnv );

stlStatus qlncAddReadColumnOnCurPlan_Instant( qlncConvExprParamInfo * aConvExprParamInfo,
                                              qllEnv                * aEnv );

stlStatus qlncAddReadColumnOnCurPlan_InstantForSelfRef( qlncConvExprParamInfo   * aConvExprParamInfo,
                                                        qllEnv                  * aEnv );

stlStatus qlncAddReadColumnOnCurPlan_ByPass( qlncConvExprParamInfo  * aConvExprParamInfo,
                                             qllEnv                 * aEnv );


/* Basic Plan Info */
typedef stlStatus (*qlncMakeCandPlanBasicFuncPtr) ( qlncPlanBasicParamInfo  * aPlanBasicParamInfo,
                                                    qlncNodeCommon          * aCandNode,
                                                    qlncPlanCommon         ** aPlanNode,
                                                    qllEnv                  * aEnv );

extern const qlncMakeCandPlanBasicFuncPtr qlncMakeCandPlanBasicFuncList[ QLNC_BEST_OPT_TYPE_MAX ];

stlStatus qlncMakeCandPlanBasicQuerySet( qlncPlanBasicParamInfo     * aPlanBasicParamInfo,
                                         qlncNodeCommon             * aCandNode,
                                         qlncPlanCommon            ** aPlanNode,
                                         qllEnv                     * aEnv );

stlStatus qlncMakeCandPlanBasicSetOP( qlncPlanBasicParamInfo    * aPlanBasicParamInfo,
                                      qlncNodeCommon            * aCandNode,
                                      qlncPlanCommon           ** aPlanNode,
                                      qllEnv                    * aEnv );

stlStatus qlncMakeCandPlanBasicQuerySpec( qlncPlanBasicParamInfo    * aPlanBasicParamInfo,
                                          qlncNodeCommon            * aCandNode,
                                          qlncPlanCommon           ** aPlanNode,
                                          qllEnv                    * aEnv );

stlStatus qlncMakeCandPlanBasicTableScan( qlncPlanBasicParamInfo    * aPlanBasicParamInfo,
                                          qlncNodeCommon            * aCandNode,
                                          qlncPlanCommon           ** aPlanNode,
                                          qllEnv                    * aEnv );

stlStatus qlncMakeCandPlanBasicIndexScan( qlncPlanBasicParamInfo    * aPlanBasicParamInfo,
                                          qlncNodeCommon            * aCandNode,
                                          qlncPlanCommon           ** aPlanNode,
                                          qllEnv                    * aEnv );

stlStatus qlncMakeCandPlanBasicIndexCombine( qlncPlanBasicParamInfo     * aPlanBasicParamInfo,
                                             qlncNodeCommon             * aCandNode,
                                             qlncPlanCommon            ** aPlanNode,
                                             qllEnv                     * aEnv );

stlStatus qlncMakeCandPlanBasicRowidScan( qlncPlanBasicParamInfo    * aPlanBasicParamInfo,
                                          qlncNodeCommon            * aCandNode,
                                          qlncPlanCommon           ** aPlanNode,
                                          qllEnv                    * aEnv );

stlStatus qlncMakeCandPlanBasicSubTable( qlncPlanBasicParamInfo     * aPlanBasicParamInfo,
                                         qlncNodeCommon             * aCandNode,
                                         qlncPlanCommon            ** aPlanNode,
                                         qllEnv                     * aEnv );

stlStatus qlncMakeCandPlanBasicInnerJoin( qlncPlanBasicParamInfo    * aPlanBasicParamInfo,
                                          qlncNodeCommon            * aCandNode,
                                          qlncPlanCommon           ** aPlanNode,
                                          qllEnv                    * aEnv );

stlStatus qlncMakeCandPlanBasicOuterJoin( qlncPlanBasicParamInfo    * aPlanBasicParamInfo,
                                          qlncNodeCommon            * aCandNode,
                                          qlncPlanCommon           ** aPlanNode,
                                          qllEnv                    * aEnv );

stlStatus qlncMakeCandPlanBasicSemiJoin( qlncPlanBasicParamInfo     * aPlanBasicParamInfo,
                                         qlncNodeCommon             * aCandNode,
                                         qlncPlanCommon            ** aPlanNode,
                                         qllEnv                     * aEnv );

stlStatus qlncMakeCandPlanBasicAntiSemiJoin( qlncPlanBasicParamInfo     * aPlanBasicParamInfo,
                                             qlncNodeCommon             * aCandNode,
                                             qlncPlanCommon            ** aPlanNode,
                                             qllEnv                     * aEnv );

stlStatus qlncMakeCandPlanBasicNestedLoops( qlncPlanBasicParamInfo  * aPlanBasicParamInfo,
                                            qlncNodeCommon          * aCandNode,
                                            qlncPlanCommon         ** aPlanNode,
                                            qllEnv                  * aEnv );

stlStatus qlncMakeCandPlanBasicMerge( qlncPlanBasicParamInfo    * aPlanBasicParamInfo,
                                      qlncNodeCommon            * aCandNode,
                                      qlncPlanCommon           ** aPlanNode,
                                      qllEnv                    * aEnv );

stlStatus qlncMakeCandPlanBasicHash( qlncPlanBasicParamInfo     * aPlanBasicParamInfo,
                                     qlncNodeCommon             * aCandNode,
                                     qlncPlanCommon            ** aPlanNode,
                                     qllEnv                     * aEnv );

stlStatus qlncMakeCandPlanBasicJoinCombine( qlncPlanBasicParamInfo  * aPlanBasicParamInfo,
                                            qlncNodeCommon          * aCandNode,
                                            qlncPlanCommon         ** aPlanNode,
                                            qllEnv                  * aEnv );

stlStatus qlncMakeCandPlanBasicSortInstant( qlncPlanBasicParamInfo  * aPlanBasicParamInfo,
                                            qlncNodeCommon          * aCandNode,
                                            qlncPlanCommon         ** aPlanNode,
                                            qllEnv                  * aEnv );

stlStatus qlncMakeCandPlanBasicHashInstant( qlncPlanBasicParamInfo  * aPlanBasicParamInfo,
                                            qlncNodeCommon          * aCandNode,
                                            qlncPlanCommon         ** aPlanNode,
                                            qllEnv                  * aEnv );

stlStatus qlncMakeCandPlanBasicGroup( qlncPlanBasicParamInfo    * aPlanBasicParamInfo,
                                      qlncNodeCommon            * aCandNode,
                                      qlncPlanCommon           ** aPlanNode,
                                      qllEnv                    * aEnv );


/* Detail Plan Info */
typedef stlStatus (*qlncMakeCandPlanDetailFuncPtr) ( qlncPlanDetailParamInfo    * aPlanDetailParamInfo,
                                                     qlncPlanCommon             * aPlanNode,
                                                     qlncPlanCommon            ** aOutPlanNode,
                                                     qllEnv                     * aEnv );

extern const qlncMakeCandPlanDetailFuncPtr qlncMakeCandPlanDetailFuncList[ QLNC_PLAN_TYPE_MAX ];

stlStatus qlncMakeCandPlanDetailQuerySet( qlncPlanDetailParamInfo   * aPlanDetailParamInfo,
                                          qlncPlanCommon            * aPlanNode,
                                          qlncPlanCommon           ** aOutPlanNode,
                                          qllEnv                    * aEnv );

stlStatus qlncMakeCandPlanDetailSetOP( qlncPlanDetailParamInfo      * aPlanDetailParamInfo,
                                       qlncPlanCommon               * aPlanNode,
                                       qlncPlanCommon              ** aOutPlanNode,
                                       qllEnv                       * aEnv );

stlStatus qlncMakeCandPlanDetailQuerySpec( qlncPlanDetailParamInfo  * aPlanDetailParamInfo,
                                           qlncPlanCommon           * aPlanNode,
                                           qlncPlanCommon          ** aOutPlanNode,
                                           qllEnv                   * aEnv );

stlStatus qlncMakeCandPlanDetailTableScan( qlncPlanDetailParamInfo  * aPlanDetailParamInfo,
                                           qlncPlanCommon           * aPlanNode,
                                           qlncPlanCommon          ** aOutPlanNode,
                                           qllEnv                   * aEnv );

stlStatus qlncMakeCandPlanDetailIndexScan( qlncPlanDetailParamInfo  * aPlanDetailParamInfo,
                                           qlncPlanCommon           * aPlanNode,
                                           qlncPlanCommon          ** aOutPlanNode,
                                           qllEnv                   * aEnv );

stlStatus qlncMakeCandPlanDetailIndexCombine( qlncPlanDetailParamInfo   * aPlanDetailParamInfo,
                                              qlncPlanCommon            * aPlanNode,
                                              qlncPlanCommon           ** aOutPlanNode,
                                              qllEnv                    * aEnv );

stlStatus qlncMakeCandPlanDetailRowidScan( qlncPlanDetailParamInfo  * aPlanDetailParamInfo,
                                           qlncPlanCommon           * aPlanNode,
                                           qlncPlanCommon          ** aOutPlanNode,
                                           qllEnv                   * aEnv );

stlStatus qlncMakeCandPlanDetailSubTable( qlncPlanDetailParamInfo   * aPlanDetailParamInfo,
                                          qlncPlanCommon            * aPlanNode,
                                          qlncPlanCommon           ** aOutPlanNode,
                                          qllEnv                    * aEnv );

stlStatus qlncMakeCandPlanDetailNestedJoin( qlncPlanDetailParamInfo * aPlanDetailParamInfo,
                                            qlncPlanCommon          * aPlanNode,
                                            qlncPlanCommon         ** aOutPlanNode,
                                            qllEnv                  * aEnv );

stlStatus qlncMakeCandPlanDetailMergeJoin( qlncPlanDetailParamInfo  * aPlanDetailParamInfo,
                                           qlncPlanCommon           * aPlanNode,
                                           qlncPlanCommon          ** aOutPlanNode,
                                           qllEnv                   * aEnv );

stlStatus qlncMakeCandPlanDetailHashJoin( qlncPlanDetailParamInfo   * aPlanDetailParamInfo,
                                          qlncPlanCommon            * aPlanNode,
                                          qlncPlanCommon           ** aOutPlanNode,
                                          qllEnv                    * aEnv );

stlStatus qlncMakeCandPlanDetailJoinCombine( qlncPlanDetailParamInfo    * aPlanDetailParamInfo,
                                             qlncPlanCommon             * aPlanNode,
                                             qlncPlanCommon            ** aOutPlanNode,
                                             qllEnv                     * aEnv );

stlStatus qlncMakeCandPlanDetailSortInstant( qlncPlanDetailParamInfo    * aPlanDetailParamInfo,
                                             qlncPlanCommon             * aPlanNode,
                                             qlncPlanCommon            ** aOutPlanNode,
                                             qllEnv                     * aEnv );

stlStatus qlncMakeCandPlanDetailHashInstant( qlncPlanDetailParamInfo    * aPlanDetailParamInfo,
                                             qlncPlanCommon             * aPlanNode,
                                             qlncPlanCommon            ** aOutPlanNode,
                                             qllEnv                     * aEnv );

#if 0
stlStatus qlncMakeCandPlanDetailSubQueryList( qlncPlanDetailParamInfo   * aPlanDetailParamInfo,
                                              qlncPlanCommon            * aPlanNode,
                                              qlncPlanCommon           ** aOutPlanNode,
                                              qllEnv                    * aEnv );

stlStatus qlncMakeCandPlanDetailSubQueryFunc( qlncPlanDetailParamInfo * aPlanDetailParamInfo,
                                              qlncPlanCommon          * aPlanNode,
                                              qlncPlanCommon         ** aOutPlanNode,
                                              qllEnv                  * aEnv );

stlStatus qlncMakeCandPlanDetailSubQueryFilter( qlncPlanDetailParamInfo * aPlanDetailParamInfo,
                                                qlncPlanCommon          * aPlanNode,
                                                qlncPlanCommon         ** aOutPlanNode,
                                                qllEnv                  * aEnv );
#endif

stlStatus qlncMakeCandPlanDetailGroup( qlncPlanDetailParamInfo  * aPlanDetailParamInfo,
                                       qlncPlanCommon           * aPlanNode,
                                       qlncPlanCommon          ** aOutPlanNode,
                                       qllEnv                   * aEnv );


stlStatus qlncMakeCandPlanSubQueryList( knlRegionMem    * aRegionMem,
                                        knlRegionMem    * aCandidateMem,
                                        qlncPlanCommon  * aChildPlanNode,
                                        qlvRefExprList  * aSubQueryExprList,
                                        qlncPlanCommon ** aOutPlanNode,
                                        qllEnv          * aEnv );

stlStatus qlncMakeCandPlanSubQueryFunc( knlRegionMem          * aRegionMem,
                                        knlRegionMem          * aCandidateMem,
                                        qlvInitExpression     * aFuncExpr,
                                        qlncPlanCommon       ** aOutPlanNode,
                                        qllEnv                * aEnv );

stlStatus qlncMakeCandPlanSubQueryFilter( knlRegionMem              * aRegionMem,
                                          knlRegionMem              * aCandidateMem,
                                          qlncPlanCommon            * aChildPlanNode,
                                          qlncSubQueryAndFilter     * aSubQueryFilter,
                                          qlncPlanCommon           ** aOutPlanNode,
                                          qllEnv                    * aEnv );

stlStatus qlncMakeCandPlanSubQueryCondition( knlRegionMem           * aRegionMem,
                                             knlRegionMem           * aCandidateMem,
                                             qlncPlanCommon         * aChildPlanNode,
                                             qlncSubQueryAndFilter  * aSubQueryFilter,
                                             qlncPlanCommon        ** aOutPlanNode,
                                             qllEnv                 * aEnv );

stlStatus qlncGetQuerySetTargetDataTypeInfo( qlncPlanDetailParamInfo   * aPlanDetailParamInfo,
                                             qlncPlanCommon            * aPlanNode,
                                             stlInt32                    aTargetCnt,
                                             qlvInitTarget             * aTarget,
                                             qllEnv                    * aEnv );


////////////////////////////////////////////////////////////////////////////////////

stlStatus qlncCreatePlanNodeMapDynArray( knlRegionMem               * aRegionMem,
                                         qlncPlanNodeMapDynArray   ** aPlanNodeMapDynArr,
                                         qllEnv                     * aEnv );

stlStatus qlncAddTailPlanNodeMapDynArray( qlncPlanNodeMapDynArray   * aPlanNodeMapDynArr,
                                          qlncPlanCommon            * aPlanNode,
                                          qllEnv                    * aEnv );

stlStatus qlncDelTailPlanNodeMapDynArray( qlncPlanNodeMapDynArray   * aPlanNodeMapDynArr,
                                          stlInt32                    aDelCount,
                                          qllEnv                    * aEnv );

stlStatus qlncFindPlanNodeFromPlanNodeMapDynArray( qlncPlanNodeMapDynArray  * aPlanNodeMapDynArr,
                                                   qlncNodeCommon           * aNode,
                                                   qlncPlanCommon          ** aFoundPlanNode,
                                                   qllEnv                   * aEnv );

void qlncFindOuterColumnListFromPlanNode( qlncPlanCommon    * aPlanNode,
                                          qlvRefExprList   ** aOuterColumnList );

////////////////////////////////////////////////////////////////////////////////////

stlStatus qlncMakeCandPlan( smlTransId            aTransID,
                            qllDBCStmt          * aDBCStmt,
                            qllStatement        * aSQLStmt,
                            qlvInitStmtExprList * aInitStmtExprList,
                            qloValidTblStatList * aTableStatList,
                            qlncNodeCommon      * aCandNode,
                            qlncPlanCommon      * aRootPlan,
                            qlncPlanCommon     ** aCandPlan,
                            qllEnv              * aEnv );

stlStatus qlncMakeCandPlanForInsert( smlTransId                       aTransID,
                                     qllDBCStmt                     * aDBCStmt,
                                     qllStatement                   * aSQLStmt,
                                     qloInitExprList                * aQueryExprList,
                                     qloValidTblStatList            * aTableStatList,
                                     qlncPlanInsertStmtParamInfo    * aPlanInsertStmtParamInfo,
                                     qllEnv                         * aEnv );

stlStatus qlncMakeCandPlanForDelete( smlTransId               aTransID,
                                     qllDBCStmt             * aDBCStmt,
                                     qllStatement           * aSQLStmt,
                                     qloInitExprList        * aQueryExprList,
                                     qloValidTblStatList    * aTableStatList,
                                     qlncNodeCommon         * aCandNode,
                                     qlncRefExprList        * aReadColumnList,
                                     stlInt32                 aReturnTargetCount,
                                     qlncTarget             * aReturnTargetArray,
                                     qlvInitTarget         ** aInitReturnTargetArray,
                                     qlvRefExprList        ** aOuterColumnList,
                                     qlncPlanCommon        ** aPlanDelete,
                                     qlncPlanCommon        ** aPlanReadTable,
                                     qlncPlanCommon        ** aPlanSubQueryList,
                                     qllEnv                 * aEnv );

stlStatus qlncMakeCandPlanForUpdate( smlTransId                       aTransID,
                                     qllDBCStmt                     * aDBCStmt,
                                     qllStatement                   * aSQLStmt,
                                     qloInitExprList                * aQueryExprList,
                                     qloValidTblStatList            * aTableStatList,
                                     qlncPlanUpdateStmtParamInfo    * aPlanUpdateStmtParamInfo,
                                     qllEnv                         * aEnv );

stlStatus qlncMakeCandPlanForSubQueryExpr( qlncParamInfo            * aParamInfo,
                                           qlvInitStmtExprList      * aInitStmtExprList,
                                           qloValidTblStatList      * aTableStatList,
                                           qlncNodeCommon           * aCandNode,
                                           qlncJoinOuterInfoList    * aJoinOuterInfoList,
                                           qlncPlanNodeMapDynArray  * aPlanNodeMapDynArr,
                                           qlncPlanCommon          ** aCandPlan,
                                           qllEnv                   * aEnv );

stlStatus qlncMCPSetAggrExprList( qlncPlanCommon    * aCandPlan,
                                  qlvRefExprList   ** aOutAggrExprList,
                                  qlvRefExprList   ** aOutNestedAggrExprList,
                                  qllEnv            * aEnv );

stlStatus qlncMCPAppendTableStat( qloValidTblStatList   * aTableStatList,
                                  qlncPlanCommon        * aPlanNode,
                                  knlRegionMem          * aRegionMem,
                                  qllEnv                * aEnv );


/** @} qlnc */

#endif /* _QLNCMAKECANDPLAN_H_ */
