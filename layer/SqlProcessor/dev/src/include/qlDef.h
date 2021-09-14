/*******************************************************************************
 * qlDef.h
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


#ifndef _QLDEF_H_
#define _QLDEF_H_ 1

/**
 * @file qlDef.h
 * @brief SQL Processor Layer Internal Definitions
 */

#include <qll.h>


/*******************************************************
 * for DML Statement
 *******************************************************/

/* parser */
#include <qlpParserLex.h>

/* node definitions */
#include <qlpNode.h>
#include <qlpNodeExecutor.h>
#include <qlpNodePlan.h>
#include <qlpNodePlanState.h>
#include <qlpNodePrimitive.h>
#include <qlpNodeExprState.h>
#include <qlpNodePlanner.h>
#include <qlpNodeMemory.h>
#include <qlpNodeValue.h>
#include <qlpNodeList.h>
#include <qlpNodeStatement.h>
#include <qlpNodeParseTree.h>
#include <qlpNodeMisc.h>
#include <qlpNodeAttrInfo.h>


/* validation */

#include <qlgStmtInfo.h>
#include <qlgCallPhase.h>

#define QLV_BASE_TABLE_TARGET_IDX   (-1)

typedef enum qlvExprType                  qlvExprType;
typedef enum qlvSubQueryCategory          qlvSubQueryCategory;
typedef enum qlvSubQueryType              qlvSubQueryType;
typedef enum qlvListFuncType              qlvListFuncType;
typedef enum qlvValueType                 qlvValueType;
typedef enum qlvHintType                  qlvHintType;
typedef enum qlvIterationTime             qlvIterationTime;

typedef enum qlvNodeType                  qlvNodeType;
typedef enum qlvSetType                   qlvSetType;
typedef enum qlvSetOption                 qlvSetOption;
typedef enum qlvJoinType                  qlvJoinType;
typedef enum qlvJoinOperType              qlvJoinOperType;
typedef enum qlvQueryTransformType        qlvQueryTransformType;
typedef enum qlvSubQueryUnnestType        qlvSubQueryUnnestType;
typedef enum qlvUnnestOperType            qlvUnnestOperType;

typedef struct qlvStmtInfo                qlvStmtInfo;
typedef struct qlvInitStmtExprList        qlvInitStmtExprList;
typedef struct qlvInitExprList            qlvInitExprList;

typedef struct qlvInitNode                qlvInitNode;

typedef struct qlvIndexScanItem           qlvIndexScanItem;
typedef struct qlvIndexScanList           qlvIndexScanList;
typedef struct qlvAccessPathHint          qlvAccessPathHint;
typedef struct qlvJoinOrderHint           qlvJoinOrderHint;
typedef struct qlvJoinOperItem            qlvJoinOperItem;
typedef struct qlvJoinOperList            qlvJoinOperList;
typedef struct qlvQueryTransformHint      qlvQueryTransformHint;
typedef struct qlvOtherHint               qlvOtherHint;
typedef struct qlvHintInfo                qlvHintInfo;

typedef struct qlvInitPlan                qlvInitPlan;
typedef struct qlvInitSelect              qlvInitSelect;

typedef struct qlvInitQuerySetNode        qlvInitQuerySetNode;
typedef struct qlvInitQuerySpecNode       qlvInitQuerySpecNode;

typedef struct qlvInitBaseTableNode       qlvInitBaseTableNode;
typedef struct qlvInitSubTableNode        qlvInitSubTableNode;
typedef struct qlvInitJoinSpec            qlvInitJoinSpec;
typedef struct qlvInitJoinTableNode       qlvInitJoinTableNode;
typedef struct qlvInitInstantNode         qlvInitInstantNode;
typedef struct qlvRelationName            qlvRelationName;

typedef struct qlvInstantColDesc          qlvInstantColDesc;

typedef struct qlvInitParamContext        qlvInitParamContext;
typedef struct qlvInitParamContextList    qlvInitParamContextList;

typedef struct qlvInitDataTypeInfo        qlvInitDataTypeInfo;
typedef struct qlvInitTarget              qlvInitTarget;
typedef struct qlvInitTargetList          qlvInitTargetList;

typedef struct qlvInitExpression          qlvInitExpression;
typedef struct qlvInitValue               qlvInitValue;
typedef struct qlvInitColumn              qlvInitColumn;
typedef struct qlvInitInnerColumn         qlvInitInnerColumn;
typedef struct qlvInitOuterColumn         qlvInitOuterColumn;
typedef struct qlvInitBindParam           qlvInitBindParam;
typedef struct qlvInitBindColumn          qlvInitBindColumn;
typedef struct qlvInitFunction            qlvInitFunction;
typedef struct qlvInitPseudoCol           qlvInitPseudoCol;
typedef struct qlvInitAggregation         qlvInitAggregation;
typedef struct qlvInitCaseExpr            qlvInitCaseExpr;
typedef struct qlvInitListFunc            qlvInitListFunc;
typedef struct qlvInitListCol             qlvInitListCol;
typedef struct qlvInitRowExpr             qlvInitRowExpr;
typedef struct qlvInitTypeDef             qlvInitTypeDef;
typedef struct qlvInitTypeCast            qlvInitTypeCast;
typedef struct qlvInitConstExpr           qlvInitConstExpr;
typedef struct qlvInitRefExpr             qlvInitRefExpr;
typedef struct qlvInitSubQuery            qlvInitSubQuery;
typedef struct qlvInitRowIdColumn         qlvInitRowIdColumn;


typedef struct qlvInitInsertTarget        qlvInitInsertTarget;
typedef struct qlvInitSingleRow           qlvInitSingleRow;
typedef struct qlvInitInsertValues        qlvInitInsertValues;
typedef struct qlvInitInsertSelect        qlvInitInsertSelect;
typedef struct qlvInitInsertReturnInto    qlvInitInsertReturnInto;
typedef struct qlvInitInsertRelObject     qlvInitInsertRelObject;
typedef struct qlvInitDelete              qlvInitDelete;
typedef struct qlvInitDeleteRelObject     qlvInitDeleteRelObject;
typedef struct qlvInitUpdate              qlvInitUpdate;
typedef struct qlvInitUpdateRelObject     qlvInitUpdateRelObject;


typedef struct qlvRefExprItem             qlvRefExprItem;
typedef struct qlvRefExprList             qlvRefExprList;
typedef struct qlvRefColListItem          qlvRefColListItem;
typedef struct qlvRefColExprList          qlvRefColExprList;
typedef struct qlvRefTableColItem         qlvRefTableColItem;
typedef struct qlvRefTableColList         qlvRefTableColList;
typedef struct qlvNamedColumnItem         qlvNamedColumnItem;
typedef struct qlvNamedColumnList         qlvNamedColumnList;
typedef struct qlvRefTableItem            qlvRefTableItem;
typedef struct qlvRefTableList            qlvRefTableList;
typedef struct qlvRefTableListLink        qlvRefTableListLink;
typedef struct qlvInitRootExprList        qlvInitRootExprList;
typedef struct qlvGroupBy                 qlvGroupBy;
typedef struct qlvOrderBy                 qlvOrderBy;
typedef struct qlvAggregation             qlvAggregation;

/**
 * @brief include expression 최대 개수 validation
 */
#define QLV_VALIDATE_INCLUDE_EXPR_CNT( _aCount, _aEnv )                         \
    {                                                                           \
        if( ( (_aCount) > KNL_EXPR_MAX_ENTRY_CNT ) || ( (_aCount) < 0 ) )       \
        {                                                                       \
            stlPushError( STL_ERROR_LEVEL_ABORT,                                \
                          QLL_ERRCODE_TOO_MANY_EXPRESSIONS,                     \
                          NULL,                                                 \
                          QLL_ERROR_STACK((_aEnv)) );                           \
            STL_TRY( STL_FALSE );                                               \
        }                                                                       \
    }

/**
 * @brief qlvRefExprItem pointer로 부터 qlvRelationName의 mTable 얻기
 */
#define QLV_GET_TABLE_NAME( aColExpr )                                          \
    ( ( aColExpr->mRelationNode->mType == QLV_NODE_TYPE_BASE_TABLE              \
        ? ( (qlvInitBaseTableNode*) aColExpr->mRelationNode)->mRelationName     \
        : ( aColExpr->mRelationNode->mType == QLV_NODE_TYPE_SUB_TABLE           \
            ? ( (qlvInitSubTableNode*) aColExpr->mRelationNode)->mRelationName  \
            : NULL ) ) )                                                        \

/**
 * @brief qlvRefExprItem pointer로 부터 qlvInitColumn pointer 얻기
 */
#define QLV_GET_COLUMN_INFO( aRefColItem ) ( aRefColItem->mExprPtr->mExpr.mColumn )

/**
 * @brief Expression 이 사용되는 Phrase 유형
 */
typedef enum qlvExprPhraseType
{
    QLV_EXPR_PHRASE_NA = 0,       /**< N/A */
    
    QLV_EXPR_PHRASE_TARGET,       /**< SELECT target, VALUES target, SET target */
    QLV_EXPR_PHRASE_FROM,         /**< FROM expression (for JOIN) */
    QLV_EXPR_PHRASE_CONDITION,    /**< Condition expression */
    QLV_EXPR_PHRASE_HAVING,       /**< HAVING expression */
    QLV_EXPR_PHRASE_GROUPBY,      /**< GROUPBY expression */
    QLV_EXPR_PHRASE_ORDERBY,      /**< ORDERBY expression */
    QLV_EXPR_PHRASE_OFFSETLIMIT,  /**< OFFSET/LIMIT expression */
    QLV_EXPR_PHRASE_RETURN,       /**< RESULT expression */
    QLV_EXPR_PHRASE_DEFAULT,      /**< DEFAULT expression */

    QLV_EXPR_PHRASE_MAX
} qlvExprPhraseType;

/**
 * @brief Conflict Serializability 를 위한 Version Conflict Resolution Action
 */
typedef enum qlvSerialAction
{
    QLV_SERIAL_NA = 0,                    /**< N/A */

                                          /*   ------------------------------------- */
                                          /*   Statement Retry,     Partial Rollback */
                                          /*   ------------------------------------- */
                                          
    QLV_SERIAL_NO_RETRY,                  /**< NO RETRY             NO ROLLBACK */
    QLV_SERIAL_RETRY_ONLY,                /**< RETRY                NO ROLLBACK */
    QLV_SERIAL_ROLLBACK_RETRY,            /**< RETRY                ROLLBACK    */

    QLV_SERIAL_MAX
    
} qlvSerialAction;


#include <qlnvNode.h>

#include <qlvValidation.h>

#include <qlvSelect.h>


/* bind */



/* optimization */

typedef struct qloInitStmtExprList        qloInitStmtExprList;
typedef struct qloInitExprList            qloInitExprList;

typedef struct qloCodeTarget              qloCodeTarget;
typedef struct qloCodeSelect              qloCodeSelect;
typedef struct qloDataSelect              qloDataSelect;

typedef struct qloDataOptExprInfo         qloDataOptExprInfo;

typedef struct qloDBType                  qloDBType;
typedef struct qloFunctionInfo            qloFunctionInfo;
typedef struct qloExprInfo                qloExprInfo;
typedef struct qloDataOutParam            qloDataOutParam;
typedef struct qloListFuncInfo            qloListFuncInfo;

typedef struct qloValidTblStatItem        qloValidTblStatItem;
typedef struct qloValidTblStatList        qloValidTblStatList;

typedef struct qloCodeOptParamInfo        qloCodeOptParamInfo;

/* Node Common */

/**
 * @brief SQL Iterator Info
 */
typedef struct qlnIteratorInfo
{
    void                      * mPhysicalHandle;
    smlTransReadMode            mTransReadMode;
    smlStmtReadMode             mStmtReadMode;
    smlIteratorScanDirection    mScanDirection;
    smlIteratorProperty         mIteratorProperty;
} qlnIteratorInfo;


/**
 * @brief Instant Table Info
 */
typedef struct qlnInstantTable
{
    stlBool               mIsNeedBuild;          /**< Instant Table 구성 필요 여부 */
    stlInt64              mRecCnt;               /**< Instant Table에 삽입된 Record Count */
    stlInt64              mCurIdx;               /**< Instant Table에서 Fetch 대상 레코드의 Index */
    
    smlStatement        * mStmt;                 /**< Instant Table과 관련된 Statement 정보 */
    void                * mTableHandle;          /**< Instant Table Handle */
    void                * mIndexHandle;          /**< Instant Index Handle */
    void                * mIterator;             /**< Instant Table Iterator */
    smlIteratorProperty   mIteratorProperty;     /**< Instant Table Iterator Property */
} qlnInstantTable;


/* SQL Function */
typedef struct qlnfAggrFuncInfo         qlnfAggrFuncInfo;
typedef struct qlnfAggrOptInfo          qlnfAggrOptInfo;
typedef struct qlnfAggrExecInfo         qlnfAggrExecInfo;
typedef struct qlnfAggrDistExecInfo     qlnfAggrDistExecInfo;

#include <qlnfNode.h>


/*
 * Optimization
 */

/* candidate nodes */

typedef struct qlncParamInfo                qlncParamInfo;
typedef struct qlncCreateNodeParamInfo      qlncCreateNodeParamInfo;
typedef struct qlncCBOptParamInfo           qlncCBOptParamInfo;
typedef struct qlncSelectStmtParamInfo      qlncSelectStmtParamInfo;
typedef struct qlncInsertStmtParamInfo      qlncInsertStmtParamInfo;
typedef struct qlncUpdateStmtParamInfo      qlncUpdateStmtParamInfo;
typedef struct qlncDeleteStmtParamInfo      qlncDeleteStmtParamInfo;


typedef enum qlncExprType                   qlncExprType;
typedef enum qlncValueType                  qlncValueType;

typedef struct qlncRefExprItem              qlncRefExprItem;
typedef struct qlncRefExprList              qlncRefExprList;
typedef struct qlncRefColumnItem            qlncRefColumnItem;
typedef struct qlncRefColumnList            qlncRefColumnList;
typedef struct qlncRefNodeItem              qlncRefNodeItem;
typedef struct qlncRefNodeList              qlncRefNodeList;
typedef struct qlncNamedColumnItem          qlncNamedColumnItem;
typedef struct qlncNamedColumnList          qlncNamedColumnList;


typedef struct qlncExprCommon               qlncExprCommon;
typedef struct qlncExprValue                qlncExprValue;
typedef struct qlncExprBindParam            qlncExprBindParam;
typedef struct qlncExprColumn               qlncExprColumn;
typedef struct qlncExprFunction             qlncExprFunction;
typedef struct qlncExprTypeDef              qlncExprTypeDef;
typedef struct qlncExprTypeCast             qlncExprTypeCast;
typedef struct qlncExprPseudoCol            qlncExprPseudoCol;
typedef struct qlncExprConstExpr            qlncExprConstExpr;
typedef struct qlncExprSubQuery             qlncExprSubQuery;
typedef struct qlncExprReference            qlncExprReference;
typedef struct qlncExprInnerColumn          qlncExprInnerColumn;
typedef struct qlncExprOuterColumn          qlncExprOuterColumn;
typedef struct qlncExprRowidColumn          qlncExprRowidColumn;
typedef struct qlncExprAggregation          qlncExprAggregation;
typedef struct qlncExprCaseExpr             qlncExprCaseExpr;
typedef struct qlncExprListFunc             qlncExprListFunc;
typedef struct qlncExprListCol              qlncExprListCol;
typedef struct qlncExprRowExpr              qlncExprRowExpr;
typedef struct qlncExprPseudoArg            qlncExprPseudoArg;
typedef struct qlncTarget                   qlncTarget;

typedef struct qlncListColMap               qlncListColMap;
typedef struct qlncListColMapList           qlncListColMapList;
typedef struct qlncAndFilter                qlncAndFilter;
typedef struct qlncOrFilter                 qlncOrFilter;
typedef struct qlncConstBooleanFilter       qlncConstBooleanFilter;
typedef struct qlncBooleanFilter            qlncBooleanFilter;

typedef struct qlncConvertExprParamInfo     qlncConvertExprParamInfo;


typedef enum qlncIndexType                  qlncIndexType;

typedef struct qlncColumnStat               qlncColumnStat;
typedef struct qlncTableStat                qlncTableStat;
typedef struct qlncIndexStat                qlncIndexStat;


typedef enum qlncNodeType                   qlncNodeType;
typedef enum qlncInstantType                qlncInstantType;
typedef enum qlncSetType                    qlncSetType;
typedef enum qlncSetOption                  qlncSetOption;
typedef enum qlncJoinType                   qlncJoinType;
typedef enum qlncJoinDirect                 qlncJoinDirect;
typedef enum qlncGroupByType                qlncGroupByType;
typedef enum qlncBestOptType                qlncBestOptType;
typedef enum qlncJoinOperType               qlncJoinOperType;
typedef enum qlncUnnestOperType             qlncUnnestOperType;
typedef enum qlncPushSubqType               qlncPushSubqType;
typedef enum qlncDistinctOperType           qlncDistinctOperType;
typedef enum qlncGroupOperType              qlncGroupOperType;

typedef struct qlncRelationName             qlncRelationName;
typedef struct qlncTableInfo                qlncTableInfo;
typedef struct qlncNodeArray                qlncNodeArray;
typedef struct qlncTableMap                 qlncTableMap;
typedef struct qlncTableMapArray            qlncTableMapArray;
typedef struct qlncQBMap                    qlncQBMap;
typedef struct qlncQBMapArray               qlncQBMapArray;

typedef struct qlncNodeCommon               qlncNodeCommon;

typedef struct qlncSelectStmt               qlncSelectStmt;

typedef struct qlncQuerySet                 qlncQuerySet;
typedef struct qlncSetOP                    qlncSetOP;
typedef struct qlncQuerySpec                qlncQuerySpec;

typedef struct qlncBaseTableNode            qlncBaseTableNode;
typedef struct qlncSubTableNode             qlncSubTableNode;
typedef struct qlncJoinTableNode            qlncJoinTableNode;
typedef struct qlncInnerJoinTableNode       qlncInnerJoinTableNode;
typedef struct qlncOuterJoinTableNode       qlncOuterJoinTableNode;
typedef struct qlncSemiJoinTableNode        qlncSemiJoinTableNode;
typedef struct qlncAntiSemiJoinTableNode    qlncAntiSemiJoinTableNode;

typedef struct qlncSortInstantNode          qlncSortInstantNode;
typedef struct qlncHashInstantNode          qlncHashInstantNode;

typedef struct qlncIndexScanInfo            qlncIndexScanInfo;
typedef struct qlncAccPathHint              qlncAccPathHint;
typedef struct qlncJoinHint                 qlncJoinHint;
typedef struct qlncQueryTransformHint       qlncQueryTransformHint;
typedef struct qlncOtherHint                qlncOtherHint;


typedef struct qlncInnerJoinNodeInfo        qlncInnerJoinNodeInfo;
typedef struct qlncCostCommon               qlncCostCommon;
typedef struct qlncQuerySpecCost            qlncQuerySpecCost;
typedef struct qlncTableScanCost            qlncTableScanCost;
typedef struct qlncIndexScanCost            qlncIndexScanCost;
typedef struct qlncIndexCombineCost         qlncIndexCombineCost;
typedef struct qlncRowidScanCost            qlncRowidScanCost;
typedef struct qlncFilterCost               qlncFilterCost;
typedef struct qlncNestedJoinCost           qlncNestedJoinCost;
typedef struct qlncMergeJoinCost            qlncMergeJoinCost;
typedef struct qlncHashJoinCost             qlncHashJoinCost;
typedef struct qlncJoinCombineCost          qlncJoinCombineCost;
typedef struct qlncFlatInstantCost          qlncFlatInstantCost;
typedef struct qlncSortInstantCost          qlncSortInstantCost;
typedef struct qlncHashInstantCost          qlncHashInstantCost;
typedef struct qlncGroupCost                qlncGroupCost;


/* Make Candidate Plan */
typedef enum qlncPlanType                   qlncPlanType;

typedef struct qlncPlanNodeMapDynArray      qlncPlanNodeMapDynArray;

typedef struct qlncReadColMap               qlncReadColMap;
typedef struct qlncReadColMapList           qlncReadColMapList;

typedef struct qlncJoinOuterInfoItem        qlncJoinOuterInfoItem;
typedef struct qlncJoinOuterInfoList        qlncJoinOuterInfoList;
typedef struct qlncSubQueryAndFilter        qlncSubQueryAndFilter;

typedef struct qlncPlanBasicParamInfo       qlncPlanBasicParamInfo;
typedef struct qlncPlanDetailParamInfo      qlncPlanDetailParamInfo;
typedef struct qlncConvExprParamInfo        qlncConvExprParamInfo;
typedef struct qlncPlanInsertStmtParamInfo  qlncPlanInsertStmtParamInfo;
typedef struct qlncPlanUpdateStmtParamInfo  qlncPlanUpdateStmtParamInfo;
typedef struct qlncPlanCommon               qlncPlanCommon;

typedef struct qlncPlanQuerySet             qlncPlanQuerySet;
typedef struct qlncPlanSetOP                qlncPlanSetOP;
typedef struct qlncPlanQuerySpec            qlncPlanQuerySpec;
typedef struct qlncPlanTableScan            qlncPlanTableScan;
typedef struct qlncPlanIndexScan            qlncPlanIndexScan;
typedef struct qlncPlanRowidScan            qlncPlanRowidScan;
typedef struct qlncPlanIndexCombine         qlncPlanIndexCombine;
typedef struct qlncPlanSubTable             qlncPlanSubTable;
typedef struct qlncPlanNestedJoin           qlncPlanNestedJoin;
typedef struct qlncPlanMergeJoin            qlncPlanMergeJoin;
typedef struct qlncPlanHashJoin             qlncPlanHashJoin;
typedef struct qlncPlanJoinCombine          qlncPlanJoinCombine;
typedef struct qlncPlanSortInstant          qlncPlanSortInstant;
typedef struct qlncPlanHashInstant          qlncPlanHashInstant;
typedef struct qlncPlanSubQueryList         qlncPlanSubQueryList;
typedef struct qlncPlanSubQueryFunc         qlncPlanSubQueryFunc;
typedef struct qlncPlanSubQueryFilter       qlncPlanSubQueryFilter;
typedef struct qlncPlanGroup                qlncPlanGroup;


typedef struct qlncStringBlock              qlncStringBlock;
typedef struct qlncTraceSubQueryExprItem    qlncTraceSubQueryExprItem;
typedef struct qlncTraceSubQueryExprList    qlncTraceSubQueryExprList;


#include <qloOptimization.h>

#include <qlncCommon.h>
#include <qlncExpr.h>
#include <qlncNode.h>
#include <qlncHeuristicQueryTransformation.h>
#include <qlncCostBasedQueryTransformation.h>
#include <qlncCostBasedOptimizer.h>
#include <qlncPostOptimizer.h>
#include <qlncMakeCandPlan.h>
#include <qlncTrace.h>


/* optimization nodes */

typedef struct qlnoDmlStmt                    qlnoDmlStmt;
typedef struct qlnoSelectStmt                 qlnoSelectStmt;
typedef struct qlnoInsertStmt                 qlnoInsertStmt;
typedef struct qlnoUpdateStmt                 qlnoUpdateStmt;
typedef struct qlnoDeleteStmt                 qlnoDeleteStmt;

typedef struct qlnoQuerySet                   qlnoQuerySet;
typedef struct qlnoSetOP                      qlnoSetOP;
typedef struct qlnoQuerySpec                  qlnoQuerySpec;
typedef struct qlnoSubQuery                   qlnoSubQuery;
typedef struct qlnoSubQueryList               qlnoSubQueryList;
typedef struct qlnoSubQueryFunc               qlnoSubQueryFunc;
typedef struct qlnoSubQueryFilter             qlnoSubQueryFilter;
typedef struct qlnoView                       qlnoView;
typedef struct qlnoFilter                     qlnoFilter;

typedef struct qlnoHash                       qlnoHash;
typedef struct qlnoAggregation                qlnoAggregation;
typedef struct qlnoGroup                      qlnoGroup;

typedef struct qlnoNestedLoopsJoin            qlnoNestedLoopsJoin;
typedef struct qlnoSortMergeJoin              qlnoSortMergeJoin;
typedef struct qlnoHashJoin                   qlnoHashJoin;
typedef struct qlnoIndexJoin                  qlnoIndexJoin;

typedef struct qlnoConcat                     qlnoConcat;

typedef struct qlnoSetUnion                   qlnoSetUnion;

typedef struct qlnoTableAccess                qlnoTableAccess;
typedef struct qlnoIndexAccess                qlnoIndexAccess;
typedef struct qlnoRowIdAccess                qlnoRowIdAccess;
typedef struct qlnoFastDualAccess             qlnoFastDualAccess;
typedef struct qlnoInstant                    qlnoInstant;


#include <qlnoNode.h>


/* Execution */


/**
 * @brief 주어진 Optimization Node와 관련된 Execution Node를 초기화한다.
 * @param[in]      aTransID      Transaction ID     (smlTransId)
 * @param[in]      aStmt         Statement          (smlStatement*)
 * @param[in]      aDBCStmt      DBC Statement      (qllDBCStmt*)
 * @param[in,out]  aSQLStmt      SQL Statement      (qllStatement*)
 * @param[in]      aOptNode      Optimization Node  (qllOptimizationNode*)
 * @param[in]      aDataArea     Data Area          (qllDataArea*)
 * @param[in]      aEnv          Environment        (qllEnv)
 *
 * @remark Data Optimization 단계에서 수행한다.
 *    <BR> Execution Node를 생성하고, Execution을 위한 정보를 구축한다.
 *    <BR> 공간 할당시 Data Plan 메모리 관리자를 이용한다.
 */
#define QLNX_INITIALIZE_NODE( aTransID,                                                                 \
                              aStmt,                                                                    \
                              aDBCStmt,                                                                 \
                              aSQLStmt,                                                                 \
                              aOptNode,                                                                 \
                              aDataArea,                                                                \
                              aEnv )                                                                    \
    {                                                                                                   \
        STL_TRY( qlnxNodeFunctions[ (aOptNode)->mType ].mInitialize( (aTransID),                        \
                                                                     (aStmt),                           \
                                                                     (aDBCStmt),                        \
                                                                     (aSQLStmt),                        \
                                                                     (aOptNode),                        \
                                                                     (aDataArea),                       \
                                                                     (aEnv) )                           \
                 == STL_SUCCESS );                                                                      \
                                                                                                        \
        ((qlnxCommonInfo*)(aDataArea->mExecNodeList[aOptNode->mIdx].mExecNode))->mFetchRowCnt = 0;      \
                                                                                                        \
        ((qlnxCommonInfo*)(aDataArea->mExecNodeList[aOptNode->mIdx].mExecNode))->mBuildTime = 0;        \
        ((qlnxCommonInfo*)(aDataArea->mExecNodeList[aOptNode->mIdx].mExecNode))->mFetchTime = 0;        \
                                                                                                        \
        ((qlnxCommonInfo*)(aDataArea->mExecNodeList[aOptNode->mIdx].mExecNode))->mFetchRecordStat.mTotalSize = 0;   \
        ((qlnxCommonInfo*)(aDataArea->mExecNodeList[aOptNode->mIdx].mExecNode))->mFetchRecordStat.mMaxSize = 0;     \
        ((qlnxCommonInfo*)(aDataArea->mExecNodeList[aOptNode->mIdx].mExecNode))->mFetchRecordStat.mMinSize = 0;     \
        ((qlnxCommonInfo*)(aDataArea->mExecNodeList[aOptNode->mIdx].mExecNode))->mFetchRecordStat.mTotalCount = 0;  \
        ((qlnxCommonInfo*)(aDataArea->mExecNodeList[aOptNode->mIdx].mExecNode))->mFetchCallCount = 0;   \
    }


/**
 * @brief 주어진 Optimization Node와 관련된 Execution Node를 Execute한다.
 * @param[in]      aTransID      Transaction ID     (smlTransId)
 * @param[in]      aStmt         Statement          (smlStatement*)
 * @param[in]      aDBCStmt      DBC Statement      (qllDBCStmt*)
 * @param[in,out]  aSQLStmt      SQL Statement      (qllStatement*)
 * @param[in]      aOptNode      Optimization Node  (qllOptimizationNode*)
 * @param[in]      aDataArea     Data Area          (qllDataArea*)
 * @param[in]      aEnv          Environment        (qllEnv)
 */
#define QLNX_EXECUTE_NODE( aTransID,                                            \
                           aStmt,                                               \
                           aDBCStmt,                                            \
                           aSQLStmt,                                            \
                           aOptNode,                                            \
                           aDataArea,                                           \
                           aEnv )                                               \
    {                                                                           \
        STL_TRY( qlnxNodeFunctions[ (aOptNode)->mType ].mExecute( (aTransID),   \
                                                                  (aStmt),      \
                                                                  (aDBCStmt),   \
                                                                  (aSQLStmt),   \
                                                                  (aOptNode),   \
                                                                  (aDataArea),  \
                                                                  (aEnv) )      \
                 == STL_SUCCESS );                                              \
    }



/**
 * @brief 주어진 Optimization Node와 관련된 Execution Node를 Fetch한다.
 * @param[out]     aFetchNodeInfo  Fetch Node Info    (qlnxFetchNodeInfo)
 * @param[in]      aTransID        Transaction ID     (smlTransId)
 * @param[in]      aStmt           Statement          (smlStatement*)
 * @param[in]      aDBCStmt        DBC Statement      (qllDBCStmt*)
 * @param[in]      aSQLStmt        SQL Statement      (qllStatement*)
 * @param[in]      aDataArea       Data Area          (qllDataArea*)
 */
#define QLNX_INIT_FETCH_NODE_INFO( aFetchNodeInfo,      \
                                   aTransID,            \
                                   aStmt,               \
                                   aDBCStmt,            \
                                   aSQLStmt,            \
                                   aDataArea )          \
    {                                                   \
        STL_DASSERT( (aStmt) != NULL );                 \
        STL_DASSERT( (aDBCStmt) != NULL );              \
        STL_DASSERT( (aSQLStmt) != NULL );              \
        STL_DASSERT( (aDataArea) != NULL );             \
                                                        \
        (aFetchNodeInfo)->mTransID  = (aTransID);       \
        (aFetchNodeInfo)->mStmt     = (aStmt);          \
        (aFetchNodeInfo)->mDBCStmt  = (aDBCStmt);       \
        (aFetchNodeInfo)->mSQLStmt  = (aSQLStmt);       \
        (aFetchNodeInfo)->mDataArea = (aDataArea);      \
                                                        \
        (aFetchNodeInfo)->mOptNode  = NULL;             \
        (aFetchNodeInfo)->mStartIdx = 0;                \
        (aFetchNodeInfo)->mSkipCnt  = 0;                \
        (aFetchNodeInfo)->mFetchCnt = 0;                \
    }                                                   \


/**
 * @brief 주어진 Optimization Node와 관련된 Execution Node를 Fetch한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info    (qlnxFetchNodeInfo)
 * @param[in]      aOptNodeIdx     Opt Node Index     (stlInt16)
 * @param[in]      aStartIdx       Start Index        (stlInt32)
 * @param[in]      aSkipCnt        Skip Record Count  (stlInt64)
 * @param[in]      aFetchCnt       Fetch Record Count (stlInt64)
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수  (stlInt64*)
 * @param[out]     aEOF            End Of Fetch 여부 (stlBool*)
 * @param[in]      aEnv            Environment        (qllEnv*)
 */
#define QLNX_FETCH_NODE( aFetchNodeInfo,                                                            \
                         aOptNodeIdx,                                                               \
                         aStartIdx,                                                                 \
                         aSkipCnt,                                                                  \
                         aFetchCnt,                                                                 \
                         aUsedBlockCnt,                                                             \
                         aEOF,                                                                      \
                         aEnv )                                                                     \
    {                                                                                               \
        STL_DASSERT( (aOptNodeIdx) < (aFetchNodeInfo)->mDataArea->mExecNodeCnt );                   \
        STL_DASSERT( (aOptNodeIdx) != QLL_INVALID_NODE_IDX );                                       \
                                                                                                    \
        (aFetchNodeInfo)->mOptNode  =                                                               \
            (aFetchNodeInfo)->mDataArea->mExecNodeList[ (aOptNodeIdx) ].mOptNode;                   \
        (aFetchNodeInfo)->mStartIdx = (aStartIdx);                                                  \
        (aFetchNodeInfo)->mSkipCnt  = (aSkipCnt);                                                   \
        (aFetchNodeInfo)->mFetchCnt = (aFetchCnt);                                                  \
                                                                                                    \
        STL_DASSERT( (aFetchNodeInfo)->mOptNode != NULL );                                          \
                                                                                                    \
        STL_TRY( qlnxNodeFunctions                                                                  \
                 [ (aFetchNodeInfo)->mDataArea->mExecNodeList[ (aOptNodeIdx) ].mNodeType ].mFetch(  \
                     (aFetchNodeInfo),                                                              \
                     (aUsedBlockCnt),                                                               \
                     (aEOF),                                                                        \
                     (aEnv) )                                                                       \
                 == STL_SUCCESS );                                                                  \
                                                                                                    \
        STL_DASSERT( *(aUsedBlockCnt) == 0 ? *(aEOF) == STL_TRUE : STL_TRUE );                      \
        STL_DASSERT( *(aUsedBlockCnt) <= (aFetchNodeInfo)->mDataArea->mBlockAllocCnt );             \
    }

/**
 * @brief 주어진 Optimization Node와 관련된 Execution Node를 종료한다.
 * @param[in]      aOptNode      Optimization Node  (qllOptimizationNode*)
 * @param[in]      aDataArea     Data Area          (qllDataArea*)
 * @param[in]      aEnv          Environment        (qllEnv)
 */
#define QLNX_FINALIZE_NODE( aOptNode,                                                           \
                            aDataArea,                                                          \
                            aEnv )                                                              \
    {                                                                                           \
        if( ( (aOptNode) != NULL ) && ( (aDataArea) != NULL ) )                                 \
        {                                                                                       \
            if( QLL_GET_EXECUTION_NODE( (aDataArea), QLL_GET_OPT_NODE_IDX( (aOptNode) ) )       \
                != NULL )                                                                       \
            {                                                                                   \
                STL_TRY( qlnxNodeFunctions[ (aOptNode)->mType ].mFinalize(                      \
                             (aOptNode),                                                        \
                             (aDataArea),                                                       \
                             (aEnv) )                                                           \
                         == STL_SUCCESS );                                                      \
            }                                                                                   \
        }                                                                                       \
    }

#define QLNX_FINALIZE_NODE_IGNORE_FAILURE( aOptNode,                                            \
                                           aDataArea,                                           \
                                           aEnv )                                               \
    {                                                                                           \
        knlDisableSessionEvent( KNL_ENV( (aEnv) ) );                                            \
        if( ( (aOptNode) != NULL ) && ( (aDataArea) != NULL ) )                                 \
        {                                                                                       \
            if( QLL_GET_EXECUTION_NODE( (aDataArea), QLL_GET_OPT_NODE_IDX( (aOptNode) ) )       \
                != NULL )                                                                       \
            {                                                                                   \
                qlnxNodeFunctions[ (aOptNode)->mType ].mFinalize(                               \
                    (aOptNode),                                                                 \
                    (aDataArea),                                                                \
                    (aEnv) );                                                                   \
            }                                                                                   \
        }                                                                                       \
        knlEnableSessionEvent( KNL_ENV( (aEnv) ) );                                             \
    }


typedef struct qlxExecInsertRelObject         qlxExecInsertRelObject;
typedef struct qlxExecUpdateRelObject         qlxExecUpdateRelObject;
typedef struct qlxExecDeleteRelObject         qlxExecDeleteRelObject;

#include <qlrcDeferrableConstraint.h>
#include <qlrcNotNull.h>

#include <qlxExecution.h>

#include <qlxExecute.h>
#include <qlxSelect.h>
#include <qlxInsert.h>
#include <qlxInsertSelect.h>
#include <qlxInsertReturnInto.h>
#include <qlxDelete.h>
#include <qlxUpdate.h>


/* execution nodes */

typedef struct qlnxRecordStat                 qlnxRecordStat;

typedef struct qlnxCommonInfo                 qlnxCommonInfo;
typedef struct qlnxSelectStmt                 qlnxSelectStmt;
typedef struct qlnxInsertStmt                 qlnxInsertStmt;
typedef struct qlnxUpdateStmt                 qlnxUpdateStmt;
typedef struct qlnxDeleteStmt                 qlnxDeleteStmt;

typedef struct qlnxQueryExpr                  qlnxQueryExpr;
typedef struct qlnxQueryExprInternal          qlnxQueryExprInternal;
typedef struct qlnxQuerySpec                  qlnxQuerySpec;
typedef struct qlnxSubQuery                   qlnxSubQuery;
typedef struct qlnxSubQueryList               qlnxSubQueryList;
typedef struct qlnxSubQueryFunc               qlnxSubQueryFunc;
typedef struct qlnxSubQueryFilter             qlnxSubQueryFilter;
typedef struct qlnxView                       qlnxView;

typedef struct qlnxQuerySet                   qlnxQuerySet;
typedef struct qlnxSetOP                      qlnxSetOP;

typedef struct qlnxHash                       qlnxHash;
typedef struct qlnxAggregation                qlnxAggregation;
typedef struct qlnxGroup                      qlnxGroup;

typedef struct qlnxCacheBlockInfo             qlnxCacheBlockInfo;
typedef struct qlnxRowBlockItem               qlnxRowBlockItem;
typedef struct qlnxRowBlockList               qlnxRowBlockList;
typedef struct qlnxCastInfo                   qlnxCastInfo;
typedef struct qlnxValueBlockItem             qlnxValueBlockItem;
typedef struct qlnxValueBlockList             qlnxValueBlockList;
typedef struct qlnxOuterColumnBlockItem       qlnxOuterColumnBlockItem;
typedef struct qlnxOuterColumnBlockList       qlnxOuterColumnBlockList;
typedef struct qlnxNestedLoopsJoin            qlnxNestedLoopsJoin;
typedef struct qlnxSortKeyCompareItem         qlnxSortKeyCompareItem;
typedef struct qlnxSortKeyCompareList         qlnxSortKeyCompareList;
typedef struct qlnxSortMergeJoin              qlnxSortMergeJoin;
typedef struct qlnxHashJoin                   qlnxHashJoin;

typedef struct qlnxSetUnion                   qlnxSetUnion;
typedef struct qlnxConcat                     qlnxConcat;

typedef struct qlnxTableAccess                qlnxTableAccess;
typedef struct qlnxIndexAccess                qlnxIndexAccess;
typedef struct qlnxRowIdAccess                qlnxRowIdAccess;
typedef struct qlnxFastDualAccess             qlnxFastDualAccess;
typedef struct qlnxInstant                    qlnxInstant;

typedef struct qlnxConflictCheckInfo          qlnxConflictCheckInfo;
typedef struct qlnxListFuncInstantFetchInfo   qlnxListFuncInstantFetchInfo;
typedef struct qlnxListFuncArrayFetchInfo     qlnxListFuncArrayFetchInfo;
typedef struct qlnxInKeyRangeFetchInfo        qlnxInKeyRangeFetchInfo;

typedef struct qlnxFunctionPtr                qlnxFunctionPtr;

typedef struct qlnxFetchNodeInfo              qlnxFetchNodeInfo;


typedef stlStatus (*qlnxSubQueryFuncPtr) ( smlTransId              aTransID,
                                           smlStatement          * aStmt,
                                           qllDBCStmt            * aDBCStmt,
                                           qllStatement          * aSQLStmt,
                                           qllOptimizationNode   * aOptNode,
                                           qllDataArea           * aDataArea,
                                           stlInt32                aIdx,
                                           qllEnv                * aEnv );

#include <qlndNode.h>
#include <qlnxNode.h>

/* Fetch */

#include <qlfFetch.h>
#include <qlfTarget.h>


/* Explain Plan */

#include <qlneNode.h>
#include <qleExplainPlan.h>


/*******************************************************
 * for special DML Statement
 *******************************************************/

#include <qlcrDeclareCursor.h>
#include <qlcrOpenCursor.h>
#include <qlcrCloseCursor.h>
#include <qlcrResultSet.h>
#include <qlcrFetchCursor.h>
#include <qlcrFetchInternal.h>

/*******************************************************
 * for DDL Statement
 *******************************************************/

#include <qlrCreateTable.h>
#include <qlrCreateTableAsSelect.h>
#include <qlrDropTable.h>
#include <qlrTruncateTable.h>
#include <qlrAlterTableAddColumn.h>
#include <qlrAlterTableDropColumn.h>
#include <qlrAlterTableAlterColumn.h>
#include <qlrAlterTableRenameColumn.h>
#include <qlrAlterTableAddConst.h>
#include <qlrAlterTableDropConst.h>
#include <qlrAlterTableAlterConst.h>
#include <qlrAlterTablePhysicalAttr.h>
#include <qlrAlterTableAddSuppLog.h>
#include <qlrAlterTableDropSuppLog.h>
#include <qlrAlterTableRenameTable.h>

#include <qlrCreateView.h>
#include <qlrDropView.h>
#include <qlrAlterView.h>

#include <qlaCreateUser.h>
#include <qlaDropUser.h>
#include <qlaAlterUser.h>
#include <qlaCreateProfile.h>
#include <qlaDropProfile.h>
#include <qlaAlterProfile.h>
#include <qlaGrantPriv.h>
#include <qlaRevokePriv.h>
#include <qlaPrivCheck.h>

#include <qltCreateSpace.h>
#include <qltDropSpace.h>
#include <qltAlterSpaceAddFile.h>
#include <qltAlterSpaceDropFile.h>
#include <qltAlterSpaceOnline.h>
#include <qltAlterSpaceOffline.h>
#include <qltAlterSpaceRenameFile.h>
#include <qltAlterSpaceRename.h>
#include <qltAlterSpaceBackup.h>

#include <qlsCreateSchema.h>
#include <qlsDropSchema.h>

#include <qlqCreateSequence.h>
#include <qlqDropSequence.h>
#include <qlqAlterSequence.h>

#include <qlyCreateSynonym.h>
#include <qlyDropSynonym.h>

#include <qliCreateIndex.h>
#include <qliDropIndex.h>
#include <qliAlterIndexPhysicalAttr.h>

#include <qldCommentStmt.h>
#include <qldAlterDatabaseAddLogfileGroup.h>
#include <qldAlterDatabaseAddLogfileMember.h>
#include <qldAlterDatabaseDropLogfileGroup.h>
#include <qldAlterDatabaseDropLogfileMember.h>
#include <qldAlterDatabaseRenameLogfile.h>

#include <qldAlterDatabaseArchivelogMode.h>
#include <qldAlterDatabaseBackup.h>
#include <qldAlterDatabaseDeleteBackupList.h>
#include <qldAlterDatabaseBackupCtrlfile.h>
#include <qldAlterDatabaseRestoreCtrlfile.h>
#include <qldAlterDatabaseIrrecoverable.h>
#include <qldAlterDatabaseRecover.h>
#include <qldAlterDatabaseIncompleteRecover.h>
#include <qldAlterDatabaseRecoverTablespace.h>
#include <qldAlterDatabaseRecoverDatafile.h>
#include <qldAlterDatabaseRestore.h>
#include <qldAlterDatabaseRestoreTablespace.h>
#include <qldAlterDatabaseClearPasswordHistory.h>

/*******************************************************
 * for Control Statement
 *******************************************************/

/*
 * for Transaction Control Statement
 */

#include <qltxCommit.h>
#include <qltxRollback.h>
#include <qltxSavepoint.h>
#include <qltxReleaseSavepoint.h>
#include <qltxLockTable.h>
#include <qltxSetConstraint.h>

/*
 * for Session Control Statement
 */

#include <qlssAlterSessionSet.h>
#include <qlssSetTransaction.h>
#include <qlssSetSessionCharacteristics.h>
#include <qlssSetSessionAuth.h>
#include <qlssSetTimeZone.h>


/*
 * for System Control Statement
 */

#include <qlssAlterSystemSet.h>
#include <qlssAlterSystemReset.h>
#include <qlssAlterSystemCheckpoint.h>
#include <qlssAlterSystemFlushLog.h>
#include <qlssAlterSystemCleanupPlan.h>
#include <qlssAlterSystemCleanupSession.h>
#include <qlssAlterSystemAger.h>
#include <qlssAlterSystemStartupDatabase.h>
#include <qlssAlterSystemShutdownDatabase.h>
#include <qlssAlterSystemDisconnectSession.h>
#include <qlssAlterSystemKillSession.h>
#include <qlssAlterSystemBreakpoint.h>
#include <qlssAlterSystemSessName.h>
#include <qlssAlterSystemSwitchLogfile.h>


/*******************************************************
 * for Fixed Table
 *******************************************************/

#include <qlgFixedTable.h>

/**
 * @defgroup qlInternal SQL Processor Layer Internal Definitions
 * @internal
 * @{
 */





/** @} qlInternal */


/**
 * @defgroup qlgStartup SQL Processor Layer Startup
 * @ingroup qlInternal
 * @internal
 * @{
 */

/** @} qlgStartup */

/**
 * @defgroup qlgStmtTable SQL Statement Table
 * @ingroup qlInternal
 * @internal
 * @{
 */

/**
 * @brief SQL Statement 종류별 정보 Table
 */
typedef struct qlgStmtTable
{
    qllNodeType        mStmtType;        /**< SQL Statement 의 소분류 */
    stlChar          * mStmtTypeKeyword; /**< SQL Type 의 Keyword String */ 
    stlInt32           mStmtAttr;        /**< SQL Statement 의 속성 @see qllStatementAttr  */
    knlStartupPhase    mExecutableFrom;  /**< 구문 수행이 가능한 FROM 단계 */
    knlStartupPhase    mExecutableUntil; /**< 구문 수행이 가능한 UNTIL 단계 */
    qllValidateFunc    mValidFunc;       /**< Validation Function Pointer */
    qllCodeOptFunc     mCodeOptFunc;     /**< Code Area Optimize Function Pointer */
    qllDataOptFunc     mDataOptFunc;     /**< Data Area Optimize Function Pointer */
    qllExecuteFunc     mExecuteFunc;     /**< Execution Function Pointer */
    
} qlgStmtTable;

extern qlgStmtTable  gSQLStmtTable[QLL_STMT_MAX - QLL_STMT_NA];

/** @} qlgStmtTable */


/**
 * @defgroup qlpParser Parsing
 * @ingroup qlInternal
 * @{
 */

/** @} */


/**
 * @defgroup qlv Validation
 * @ingroup qlInternal
 * @{
 */

#define QLV_PREDEFINE_NULL_STRING   ("")


/** @} qlv */



/**
 * @defgroup qlo Optimization
 * @ingroup qlInternal
 * @{
 */

/** @} qlo */


/**
 * @defgroup qloc Code Optimization
 * @ingroup qlo
 * @{
 */

/** @} qloc */


/**
 * @defgroup qlod Data Optimization
 * @ingroup qlo
 * @{
 */

/** @} qlod */


/**
 * @defgroup qlop Candidate Plan
 * @ingroup qlo
 * @{
 */

/** @} qlop */


/**
 * @defgroup qlb Binding
 * @ingroup qlInternal
 * @{
 */

/** @} qlb */


/**
 * @defgroup qlbt Type Binding
 * @ingroup qlb
 * @{
 */

/** @} qlbt */


/**
 * @defgroup qlbv Value Binding
 * @ingroup qlb
 * @{
 */

/** @} qlbv */


/**
 * @defgroup qlx Execution
 * @ingroup qlInternal
 * @{
 */

/** @} qlx */


/**
 * @defgroup qlf Fetch
 * @ingroup qlInternal
 * @{
 */

/** @} qlf */


/**
 * @defgroup qlObject SQL-Object DDL
 * @ingroup qlInternal
 * @{
 */

/** @} qlObject */



/**
 * @defgroup qld Database DDL
 * @ingroup qlObject
 * @{
 */

/** @} qld */


/**
 * @defgroup qldAlterDatabase ALTER DATABASE
 * @ingroup qld
 * @{
 */

/** @} qldAlterDatabase */



/**
 * @defgroup qla User DDL
 * @ingroup qlObject
 * @{
 */

/** @} qla */


/**
 * @defgroup qlt Tablespace DDL
 * @ingroup qlObject
 * @{
 */

/** @} qlt */



/**
 * @defgroup qltAlterSpace ALTER TABLESPACE
 * @ingroup qlt
 * @{
 */

/** @} qltAlterSpace */



/**
 * @defgroup qls Schema DDL
 * @ingroup qlObject
 * @{
 */

/** @} qls */





/**
 * @defgroup qlr Table DDL
 * @ingroup qlObject
 * @{
 */

/** @} qlr */




/**
 * @defgroup qlrAlterTable ALTER TABLE
 * @ingroup qlr
 * @{
 */

/** @} qlrAlterTable */








/**
 * @defgroup qlq Sequence DDL
 * @ingroup qlObject
 * @{
 */

/** @} qlq */







/**
 * @defgroup qli Index DDL
 * @ingroup qlObject
 * @{
 */

/** @} qli */

/**
 * @defgroup qliAlterIndex ALTER INDEX
 * @ingroup qli
 * @{
 */

/** @} qliAlterIndex */









/**
 * @defgroup qlrc Constraint DDL
 * @ingroup qlObject
 * @{
 */

/** @} qlrc */





/**
 * @defgroup qlcrCursor Cursor
 * @ingroup qlObject
 * @{
 */

/** @} qlcrCursor */






/**
 * @defgroup qlCtrl  Control Lanaguage
 * @ingroup qlInternal
 * @{
 */

/** @} qlCtrl */






/**
 * @defgroup qlssSession Session Control Language
 * @ingroup qlCtrl
 * @{
 */

/** @} qlssSession */








/**
 * @defgroup qlssSystem System Contorl Language
 * @ingroup qlCtrl
 * @{
 */

/** @} qlssSystem */









/**
 * @defgroup qltx Transaction Control Language
 * @ingroup qlCtrl
 * @{
 */

/** @} qltx */





/**
 * @defgroup qle Explain Plan
 * @ingroup qlInternal
 * @{
 */

/** @} qle */




/**
 * @defgroup qln Node
 * @ingroup qlInternal
 * @{
 */

/**
 * @brief SQL Iterator Type
 */
typedef enum
{
    QLN_ITERATOR_TYPE_BASE_TABLE = 0,      /**< Base Table에 대한 SQL Iterator */
    QLN_ITERATOR_TYPE_BASE_INDEX,          /**< Base Index에 대한 SQL Iterator */
    QLN_ITERATOR_TYPE_INSTANCE_TABLE,      /**< Instance Table에 대한 SQL Iterator */
    QLN_ITERATOR_TYPE_INSTANCE_INDEX,      /**< Instance Index에 대한 SQL Iterator */
    QLN_ITERATOR_TYPE_MAX                  /**< read only statement 사용 */
} qlnIteratorType;


/**
 * @brief SQL Iterator Store Data Type
 */
typedef enum
{
    QLN_ITERATOR_DATA_TYPE_VALUE = 0,      /**< Data Value : 실제 데이타를 저장 */
    QLN_ITERATOR_DATA_TYPE_POINTER,        /**< Data Value Pointer : memory 상에 상주하고 있는 Value의 주소 */
    QLN_ITERATOR_DATA_TYPE_RID,            /**< RID : 다른 Iterator를 통한 접근 */
    QLN_ITERATOR_DATA_TYPE_MAX             
} qlnIteratorDataType;


/** @} qln */




/**
 * @defgroup qlnv Validation Node
 * @ingroup qln
 * @{
 */

/** @} qlnv */


/**
 * @defgroup qlno Code Optimization Node
 * @ingroup qln
 * @{
 */

/** @} qlno */


/**
 * @defgroup qlnd Data Optimization Node
 * @ingroup qln
 * @{
 */

/** @} qlnd */


/**
 * @defgroup qlnx Execution Node
 * @ingroup qln
 * @{
 */

/** @} qlnx */




/**
 * @defgroup qlnf SQL Function
 * @ingroup qln
 * @{
 */

/** @} qlnf */




/**
 * @defgroup qlnfAggr Aggregation Function
 * @ingroup qlnf
 * @{
 */


/** @} qlnfAggr */



/**
 * @defgroup qlne Node Expanation
 * @ingroup qln
 * @{
 */

/** @} qlne */



#endif /* _QLDEF_H_ */

