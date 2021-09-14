/*******************************************************************************
 * qlncTrace.h
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

#ifndef _QLNCTRACE_H_
#define _QLNCTRACE_H_ 1

/**
 * @file qlncTrace.h
 * @brief SQL Processor Layer Optimizer Trace
 */

#include <qlDef.h>

/**
 * @addtogroup qlnc
 * @{
 */


/*******************************************************************************
 * DEFINITIONS
 ******************************************************************************/

#define QLNC_TRACE_DEFAULT_STRING_BLOCK_SIZE    ( 4 * 1024 )
#define QLNC_TRACE_INCREASE_STRING_BLOCK_SIZE   ( 4 * 1024 )

#define QLNC_TRACE_TREE_MAX_PREFIX_COUNT    32  /**< Tree 형태의 Trace에서 Prefix 문자들의 최대 개수 */
#define QLNC_TRACE_SUBQUERY_NAME_MAX_SIZE   64  /**< SubQuery Expression Name의 최대 길이 */

#define QLNC_TRACE_SUBQUERY_NAME_PREFIX     "@SUBQUERY-"    /**< SubQuery Expression Name의 Prefix */


#define QLNC_TRACE_GET_DATA_TYPE_NAME( _aDBType )   \
    dtlDataTypeName[ (_aDBType) ]

#define QLNC_TRACE_GET_STRING_LENGTH_UNIT_NAME( _aStringLengthUnit )    \
    gDtlLengthUnitString[ (_aStringLengthUnit) ]

#define QLNC_TRACE_GET_INTERVAL_INDICATOR_NAME( _aIntervalIndicator )   \
    gDtlIntervalIndicatorString[ sTypeCast->mTypeDef.mIntervalIndicator ]

#define QLNC_TRACE_GET_BUILTIN_FUNC_NAME( _aFuncId )    \
    qlneBuiltInFuncName[ (_aFuncId) ]

#define QLNC_TRACE_GET_AGGREGATION_FUNC_NAME( _aAggrId )    \
    qlneAggrFuncName[ (_aAggrId) ]

#define QLNC_TRACE_RESET_STRING_BLOCK( _aStringBlock )  \
{                                                       \
    (_aStringBlock)->mString[0] = '\0';                 \
    (_aStringBlock)->mCurSize = 0;                      \
}

#define QLNC_TRACE_INIT_EXPR_DEFAULT_INFO( _aStringBlock, _aExpr, _aLevel, _aEnv )  \
{                                                                                   \
    STL_TRY( qlncTraceAppendStringToStringBlock(                                    \
                 (_aStringBlock),                                                   \
                 (_aEnv),                                                           \
                 "%s# TYP:%s PTR: 0x%p ITR:%s OFF:%d SET:%c NAM:%s",                \
                 qlncTraceTreePrefix[(_aLevel)],                                    \
                 qlncExprTypeName[(_aExpr)->mType],                                 \
                 (_aExpr),                                                          \
                 qlncIterationTimeName[(_aExpr)->mIterationTime],                   \
                 (_aExpr)->mOffset,                                                 \
                 ((_aExpr)->mIsSetOffset == STL_TRUE ? 'Y' : 'N'),                  \
                 ((_aExpr)->mColumnName == NULL ? "NULL" : (_aExpr)->mColumnName) ) \
             == STL_SUCCESS );                                                      \
}

#define QLNC_TRACE_APPEND_FILTER_INFO( _aStringBlock, _aPrefix, _aHeader, _aAndFilter, _aEnv )  \
{                                                                                               \
    qlncTraceSubQueryExprList     _sSubQueryExprList;                                           \
    STL_TRY( qlncTraceAppendStringToStringBlock(                                                \
                 (_aStringBlock),                                                               \
                 (_aEnv),                                                                       \
                 "%s%s: ",                                                                      \
                 (_aPrefix),                                                                    \
                 (_aHeader) )                                                                   \
             == STL_SUCCESS );                                                                  \
                                                                                                \
    _sSubQueryExprList.mRegionMem = (_aStringBlock)->mRegionMem;                                \
    _sSubQueryExprList.mSubQueryID = 0;                                                         \
    _sSubQueryExprList.mCount = 0;                                                              \
    _sSubQueryExprList.mHead = NULL;                                                            \
    _sSubQueryExprList.mTail = NULL;                                                            \
                                                                                                \
    STL_TRY( qlncTraceTreeFromCandExprAndFilter(                                                \
                 (_aStringBlock),                                                               \
                 (qlncExprCommon*)(_aAndFilter),                                                \
                 &_sSubQueryExprList,                                                           \
                 (_aEnv) )                                                                      \
             == STL_SUCCESS );                                                                  \
                                                                                                \
    STL_TRY( qlncTraceAppendStringToStringBlock(                                                \
                 (_aStringBlock),                                                               \
                 (_aEnv),                                                                       \
                 "\n" )                                                                         \
             == STL_SUCCESS );                                                                  \
}

#define QLNC_TRACE_APPEND_COST_INFO( _aStringBlock, _aPrefix, _aCostCommon, _aEnv )         \
{                                                                                           \
    STL_TRY( qlncTraceAppendStringToStringBlock(                                            \
                 (_aStringBlock),                                                           \
                 (_aEnv),                                                                   \
                 "%sCost: %.8lf   #Filter: %.8lf"                                           \
                 "   #Build: %.8lf   #ReadIO: %.8lf"                                        \
                 "   #OutputIO: %.8lf   #Rows: %.8lf\n",                                    \
                 (_aPrefix),                                                                \
                 (_aCostCommon)->mTotalCost,                                                \
                 ( (_aCostCommon)->mFirstFilterCost + (_aCostCommon)->mNextFilterCost ),    \
                 (_aCostCommon)->mBuildCost,                                                \
                 (_aCostCommon)->mReadIOCost,                                               \
                 (_aCostCommon)->mOutputIOCost,                                             \
                 (_aCostCommon)->mResultRowCount )                                          \
             == STL_SUCCESS );                                                              \
}


/*******************************************************************************
 * STRUCTURES
 ******************************************************************************/

extern const stlChar * qlncTraceTreePrefix[QLNC_TRACE_TREE_MAX_PREFIX_COUNT];
extern const stlChar * qlncExprTypeName[QLV_EXPR_TYPE_MAX];
extern const stlChar * qlncIterationTimeName[DTL_ITERATION_TIME_MAX];
extern const stlChar * qlncInitNodeTypeName[QLV_NODE_TYPE_MAX];
extern const stlChar * qlncJoinTypeName[QLV_JOIN_TYPE_MAX];

/**
 * @brief SQL Query String Block
 */

struct qlncStringBlock
{
    knlRegionMem    * mRegionMem;
    stlChar         * mString;
    stlInt32          mMaxSize;
    stlInt32          mCurSize;
};


/**
 * @brief SubQuery Expression management Item
 */
struct qlncTraceSubQueryExprItem
{
    stlChar                       mSubQueryName[QLNC_TRACE_SUBQUERY_NAME_MAX_SIZE];
    qlncExprSubQuery            * mSubQueryExpr;
    qlncTraceSubQueryExprItem   * mNext;
};


/**
 * @brief SubQuery Expression management List
 */
struct qlncTraceSubQueryExprList
{
    knlRegionMem                * mRegionMem;
    stlInt32                      mSubQueryID;
    stlInt32                      mCount;
    qlncTraceSubQueryExprItem   * mHead;
    qlncTraceSubQueryExprItem   * mTail;
};


/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

/* Trace Query From Candidate Expression */
typedef stlStatus (*qlncTraceQueryFromCandExprFuncPtr) ( qlncStringBlock    * aStringBlock,
                                                         qlncExprCommon     * aCandExpr,
                                                         qllEnv             * aEnv );

extern const qlncTraceQueryFromCandExprFuncPtr qlncTraceQueryFromCandExprFuncList[ QLNC_EXPR_TYPE_MAX ];

stlStatus qlncTraceQueryFromCandExprValue( qlncStringBlock  * aStringBlock,
                                           qlncExprCommon   * aCandExpr,
                                           qllEnv           * aEnv );

stlStatus qlncTraceQueryFromCandExprBindParam( qlncStringBlock  * aStringBlock,
                                               qlncExprCommon   * aCandExpr,
                                               qllEnv           * aEnv );

stlStatus qlncTraceQueryFromCandExprColumn( qlncStringBlock * aStringBlock,
                                            qlncExprCommon  * aCandExpr,
                                            qllEnv          * aEnv );

stlStatus qlncTraceQueryFromCandExprFunction( qlncStringBlock   * aStringBlock,
                                              qlncExprCommon    * aCandExpr,
                                              qllEnv            * aEnv );

stlStatus qlncTraceQueryFromCandExprCast( qlncStringBlock   * aStringBlock,
                                          qlncExprCommon    * aCandExpr,
                                          qllEnv            * aEnv );

stlStatus qlncTraceQueryFromCandExprPseudoColumn( qlncStringBlock   * aStringBlock,
                                                  qlncExprCommon    * aCandExpr,
                                                  qllEnv            * aEnv );

stlStatus qlncTraceQueryFromCandExprPseudoArgument( qlncStringBlock * aStringBlock,
                                                    qlncExprCommon  * aCandExpr,
                                                    qllEnv          * aEnv );

stlStatus qlncTraceQueryFromCandExprConstantExprResult( qlncStringBlock * aStringBlock,
                                                        qlncExprCommon  * aCandExpr,
                                                        qllEnv          * aEnv );

stlStatus qlncTraceQueryFromCandExprSubQuery( qlncStringBlock   * aStringBlock,
                                              qlncExprCommon    * aCandExpr,
                                              qllEnv            * aEnv );

stlStatus qlncTraceQueryFromCandExprReference( qlncStringBlock  * aStringBlock,
                                               qlncExprCommon   * aCandExpr,
                                               qllEnv           * aEnv );

stlStatus qlncTraceQueryFromCandExprInnerColumn( qlncStringBlock    * aStringBlock,
                                                 qlncExprCommon     * aCandExpr,
                                                 qllEnv             * aEnv );

stlStatus qlncTraceQueryFromCandExprOuterColumn( qlncStringBlock    * aStringBlock,
                                                 qlncExprCommon     * aCandExpr,
                                                 qllEnv             * aEnv );

stlStatus qlncTraceQueryFromCandExprRowidColumn( qlncStringBlock    * aStringBlock,
                                                 qlncExprCommon     * aCandExpr,
                                                 qllEnv             * aEnv );

stlStatus qlncTraceQueryFromCandExprAggregation( qlncStringBlock    * aStringBlock,
                                                 qlncExprCommon     * aCandExpr,
                                                 qllEnv             * aEnv );

stlStatus qlncTraceQueryFromCandExprCaseExpr( qlncStringBlock   * aStringBlock,
                                              qlncExprCommon    * aCandExpr,
                                              qllEnv            * aEnv );

stlStatus qlncTraceQueryFromCandExprListFunction( qlncStringBlock   * aStringBlock,
                                                  qlncExprCommon    * aCandExpr,
                                                  qllEnv            * aEnv );

stlStatus qlncTraceQueryFromCandExprListColumn( qlncStringBlock * aStringBlock,
                                                qlncExprCommon  * aCandExpr,
                                                qllEnv          * aEnv );

stlStatus qlncTraceQueryFromCandExprRowExpr( qlncStringBlock    * aStringBlock,
                                             qlncExprCommon     * aCandExpr,
                                             qllEnv             * aEnv );

stlStatus qlncTraceQueryFromCandExprAndFilter( qlncStringBlock  * aStringBlock,
                                               qlncExprCommon   * aCandExpr,
                                               qllEnv           * aEnv );

stlStatus qlncTraceQueryFromCandExprOrFilter( qlncStringBlock   * aStringBlock,
                                              qlncExprCommon    * aCandExpr,
                                              qllEnv            * aEnv );

stlStatus qlncTraceQueryFromCandExprBooleanFilter( qlncStringBlock  * aStringBlock,
                                                   qlncExprCommon   * aCandExpr,
                                                   qllEnv           * aEnv );

stlStatus qlncTraceQueryFromCandExprConstBooleanFilter( qlncStringBlock * aStringBlock,
                                                        qlncExprCommon  * aCandExpr,
                                                        qllEnv          * aEnv );

/* Trace Query From Candidate Node */
typedef stlStatus (*qlncTraceQueryFromCandNodeFuncPtr) ( qlncStringBlock    * aStringBlock,
                                                         qlncNodeCommon     * aCandNode,
                                                         qllEnv             * aEnv );

extern const qlncTraceQueryFromCandNodeFuncPtr qlncTraceQueryFromCandNodeFuncList[ QLNC_NODE_TYPE_MAX ];

stlStatus qlncTraceQueryFromCandNodeQuerySet( qlncStringBlock   * aStringBlock,
                                              qlncNodeCommon    * aCandNode,
                                              qllEnv            * aEnv );

stlStatus qlncTraceQueryFromCandNodeSetOP( qlncStringBlock  * aStringBlock,
                                           qlncNodeCommon   * aCandNode,
                                           qllEnv           * aEnv );

stlStatus qlncTraceQueryFromCandNodeQuerySpec( qlncStringBlock  * aStringBlock,
                                               qlncNodeCommon   * aCandNode,
                                               qllEnv           * aEnv );

stlStatus qlncTraceQueryFromCandNodeBaseTable( qlncStringBlock  * aStringBlock,
                                               qlncNodeCommon   * aCandNode,
                                               qllEnv           * aEnv );

stlStatus qlncTraceQueryFromCandNodeSubTable( qlncStringBlock   * aStringBlock,
                                              qlncNodeCommon    * aCandNode,
                                              qllEnv            * aEnv );

stlStatus qlncTraceQueryFromCandNodeJoinTable( qlncStringBlock  * aStringBlock,
                                               qlncNodeCommon   * aCandNode,
                                               qllEnv           * aEnv );

stlStatus qlncTraceQueryFromCandNodeFlatInstant( qlncStringBlock    * aStringBlock,
                                                 qlncNodeCommon     * aCandNode,
                                                 qllEnv             * aEnv );

stlStatus qlncTraceQueryFromCandNodeSortInstant( qlncStringBlock    * aStringBlock,
                                                 qlncNodeCommon     * aCandNode,
                                                 qllEnv             * aEnv );

stlStatus qlncTraceQueryFromCandNodeHashInstant( qlncStringBlock    * aStringBlock,
                                                 qlncNodeCommon     * aCandNode,
                                                 qllEnv             * aEnv );

stlStatus qlncTraceQueryFromCandNodeInnerJoinTable( qlncStringBlock * aStringBlock,
                                                    qlncNodeCommon  * aCandNode,
                                                    qllEnv          * aEnv );

stlStatus qlncTraceQueryFromCandNodeOuterJoinTable( qlncStringBlock * aStringBlock,
                                                    qlncNodeCommon  * aCandNode,
                                                    qllEnv          * aEnv );

stlStatus qlncTraceQueryFromCandNodeSemiJoinTable( qlncStringBlock  * aStringBlock,
                                                   qlncNodeCommon   * aCandNode,
                                                   qllEnv           * aEnv );

stlStatus qlncTraceQueryFromCandNodeAntiSemiJoinTable( qlncStringBlock  * aStringBlock,
                                                       qlncNodeCommon   * aCandNode,
                                                       qllEnv           * aEnv );

/* Trace Tree From Candidate Expression */
typedef stlStatus (*qlncTraceTreeFromCandExprFuncPtr) ( qlncStringBlock             * aStringBlock,
                                                        qlncExprCommon              * aCandExpr,
                                                        qlncTraceSubQueryExprList   * aSubQueryExprList,
                                                        qllEnv                      * aEnv );

extern const qlncTraceTreeFromCandExprFuncPtr qlncTraceTreeFromCandExprFuncList[ QLNC_EXPR_TYPE_MAX ];

stlStatus qlncTraceTreeFromCandExprValue( qlncStringBlock       * aStringBlock,
                                          qlncExprCommon            * aCandExpr,
                                          qlncTraceSubQueryExprList * aSubQueryExprList,
                                          qllEnv                    * aEnv );

stlStatus qlncTraceTreeFromCandExprBindParam( qlncStringBlock           * aStringBlock,
                                              qlncExprCommon            * aCandExpr,
                                              qlncTraceSubQueryExprList * aSubQueryExprList,
                                              qllEnv                    * aEnv );

stlStatus qlncTraceTreeFromCandExprColumn( qlncStringBlock              * aStringBlock,
                                           qlncExprCommon               * aCandExpr,
                                           qlncTraceSubQueryExprList    * aSubQueryExprList,
                                           qllEnv                       * aEnv );

stlStatus qlncTraceTreeFromCandExprFunction( qlncStringBlock            * aStringBlock,
                                             qlncExprCommon             * aCandExpr,
                                             qlncTraceSubQueryExprList  * aSubQueryExprList,
                                             qllEnv                     * aEnv );

stlStatus qlncTraceTreeFromCandExprCast( qlncStringBlock            * aStringBlock,
                                         qlncExprCommon             * aCandExpr,
                                         qlncTraceSubQueryExprList  * aSubQueryExprList,
                                         qllEnv                     * aEnv );

stlStatus qlncTraceTreeFromCandExprPseudoColumn( qlncStringBlock            * aStringBlock,
                                                 qlncExprCommon             * aCandExpr,
                                                 qlncTraceSubQueryExprList  * aSubQueryExprList,
                                                 qllEnv                     * aEnv );

stlStatus qlncTraceTreeFromCandExprPseudoArgument( qlncStringBlock              * aStringBlock,
                                                   qlncExprCommon               * aCandExpr,
                                                   qlncTraceSubQueryExprList    * aSubQueryExprList,
                                                   qllEnv                       * aEnv );

stlStatus qlncTraceTreeFromCandExprConstantExprResult( qlncStringBlock              * aStringBlock,
                                                       qlncExprCommon               * aCandExpr,
                                                       qlncTraceSubQueryExprList    * aSubQueryExprList,
                                                       qllEnv                       * aEnv );

stlStatus qlncTraceTreeFromCandExprSubQuery( qlncStringBlock            * aStringBlock,
                                             qlncExprCommon             * aCandExpr,
                                             qlncTraceSubQueryExprList  * aSubQueryExprList,
                                             qllEnv                     * aEnv );

stlStatus qlncTraceTreeFromCandExprReference( qlncStringBlock           * aStringBlock,
                                              qlncExprCommon            * aCandExpr,
                                              qlncTraceSubQueryExprList * aSubQueryExprList,
                                              qllEnv                    * aEnv );

stlStatus qlncTraceTreeFromCandExprInnerColumn( qlncStringBlock             * aStringBlock,
                                                qlncExprCommon              * aCandExpr,
                                                qlncTraceSubQueryExprList   * aSubQueryExprList,
                                                qllEnv                      * aEnv );

stlStatus qlncTraceTreeFromCandExprOuterColumn( qlncStringBlock             * aStringBlock,
                                                qlncExprCommon              * aCandExpr,
                                                qlncTraceSubQueryExprList   * aSubQueryExprList,
                                                qllEnv                      * aEnv );

stlStatus qlncTraceTreeFromCandExprRowidColumn( qlncStringBlock             * aStringBlock,
                                                qlncExprCommon              * aCandExpr,
                                                qlncTraceSubQueryExprList   * aSubQueryExprList,
                                                qllEnv                      * aEnv );

stlStatus qlncTraceTreeFromCandExprAggregation( qlncStringBlock             * aStringBlock,
                                                qlncExprCommon              * aCandExpr,
                                                qlncTraceSubQueryExprList   * aSubQueryExprList,
                                                qllEnv                      * aEnv );

stlStatus qlncTraceTreeFromCandExprCaseExpr( qlncStringBlock            * aStringBlock,
                                             qlncExprCommon             * aCandExpr,
                                             qlncTraceSubQueryExprList  * aSubQueryExprList,
                                             qllEnv                     * aEnv );

stlStatus qlncTraceTreeFromCandExprListFunction( qlncStringBlock            * aStringBlock,
                                                 qlncExprCommon             * aCandExpr,
                                                 qlncTraceSubQueryExprList  * aSubQueryExprList,
                                                 qllEnv                     * aEnv );

stlStatus qlncTraceTreeFromCandExprListColumn( qlncStringBlock              * aStringBlock,
                                               qlncExprCommon               * aCandExpr,
                                               qlncTraceSubQueryExprList    * aSubQueryExprList,
                                               qllEnv                       * aEnv );

stlStatus qlncTraceTreeFromCandExprRowExpr( qlncStringBlock             * aStringBlock,
                                            qlncExprCommon              * aCandExpr,
                                            qlncTraceSubQueryExprList   * aSubQueryExprList,
                                            qllEnv                      * aEnv );

stlStatus qlncTraceTreeFromCandExprAndFilter( qlncStringBlock           * aStringBlock,
                                              qlncExprCommon            * aCandExpr,
                                              qlncTraceSubQueryExprList * aSubQueryExprList,
                                              qllEnv                    * aEnv );

stlStatus qlncTraceTreeFromCandExprOrFilter( qlncStringBlock            * aStringBlock,
                                             qlncExprCommon             * aCandExpr,
                                             qlncTraceSubQueryExprList  * aSubQueryExprList,
                                             qllEnv                     * aEnv );

stlStatus qlncTraceTreeFromCandExprBooleanFilter( qlncStringBlock           * aStringBlock,
                                                  qlncExprCommon            * aCandExpr,
                                                  qlncTraceSubQueryExprList * aSubQueryExprList,
                                                  qllEnv                    * aEnv );

stlStatus qlncTraceTreeFromCandExprConstBooleanFilter( qlncStringBlock              * aStringBlock,
                                                       qlncExprCommon               * aCandExpr,
                                                       qlncTraceSubQueryExprList    * aSubQueryExprList,
                                                       qllEnv                       * aEnv );

/* Trace Tree From Candidate Node */
typedef stlStatus (*qlncTraceTreeFromCandNodeFuncPtr) ( qlncStringBlock             * aStringBlock,
                                                        qlncNodeCommon              * aCandNode,
                                                        qlncTraceSubQueryExprList   * aSubQueryExprList,
                                                        stlInt32                      aLevel,
                                                        qllEnv                      * aEnv );

extern const qlncTraceTreeFromCandNodeFuncPtr qlncTraceTreeFromCandNodeFuncList[ QLNC_NODE_TYPE_MAX ];

stlStatus qlncTraceTreeFromCandNodeQuerySet( qlncStringBlock            * aStringBlock,
                                             qlncNodeCommon             * aCandNode,
                                             qlncTraceSubQueryExprList  * aSubQueryExprList,
                                             stlInt32                     aLevel,
                                             qllEnv                     * aEnv );

stlStatus qlncTraceTreeFromCandNodeSetOP( qlncStringBlock           * aStringBlock,
                                          qlncNodeCommon            * aCandNode,
                                          qlncTraceSubQueryExprList * aSubQueryExprList,
                                          stlInt32                    aLevel,
                                          qllEnv                    * aEnv );

stlStatus qlncTraceTreeFromCandNodeQuerySpec( qlncStringBlock           * aStringBlock,
                                              qlncNodeCommon            * aCandNode,
                                              qlncTraceSubQueryExprList * aSubQueryExprList,
                                              stlInt32                    aLevel,
                                              qllEnv                    * aEnv );

stlStatus qlncTraceTreeFromCandNodeBaseTable( qlncStringBlock           * aStringBlock,
                                              qlncNodeCommon            * aCandNode,
                                              qlncTraceSubQueryExprList * aSubQueryExprList,
                                              stlInt32                    aLevel,
                                              qllEnv                    * aEnv );

stlStatus qlncTraceTreeFromCandNodeSubTable( qlncStringBlock            * aStringBlock,
                                             qlncNodeCommon             * aCandNode,
                                             qlncTraceSubQueryExprList  * aSubQueryExprList,
                                             stlInt32                     aLevel,
                                             qllEnv                     * aEnv );

stlStatus qlncTraceTreeFromCandNodeJoinTable( qlncStringBlock           * aStringBlock,
                                              qlncNodeCommon            * aCandNode,
                                              qlncTraceSubQueryExprList * aSubQueryExprList,
                                              stlInt32                    aLevel,
                                              qllEnv                    * aEnv );

stlStatus qlncTraceTreeFromCandNodeFlatInstant( qlncStringBlock             * aStringBlock,
                                                qlncNodeCommon              * aCandNode,
                                                qlncTraceSubQueryExprList   * aSubQueryExprList,
                                                stlInt32                      aLevel,
                                                qllEnv                      * aEnv );

stlStatus qlncTraceTreeFromCandNodeSortInstant( qlncStringBlock             * aStringBlock,
                                                qlncNodeCommon              * aCandNode,
                                                qlncTraceSubQueryExprList   * aSubQueryExprList,
                                                stlInt32                      aLevel,
                                                qllEnv                      * aEnv );

stlStatus qlncTraceTreeFromCandNodeHashInstant( qlncStringBlock             * aStringBlock,
                                                qlncNodeCommon              * aCandNode,
                                                qlncTraceSubQueryExprList   * aSubQueryExprList,
                                                stlInt32                      aLevel,
                                                qllEnv                      * aEnv );

stlStatus qlncTraceTreeFromCandNodeInnerJoinTable( qlncStringBlock              * aStringBlock,
                                                   qlncNodeCommon               * aCandNode,
                                                   qlncTraceSubQueryExprList    * aSubQueryExprList,
                                                   stlInt32                       aLevel,
                                                   qllEnv                       * aEnv );

stlStatus qlncTraceTreeFromCandNodeOuterJoinTable( qlncStringBlock              * aStringBlock,
                                                   qlncNodeCommon               * aCandNode,
                                                   qlncTraceSubQueryExprList    * aSubQueryExprList,
                                                   stlInt32                       aLevel,
                                                   qllEnv                       * aEnv );

stlStatus qlncTraceTreeFromCandNodeSemiJoinTable( qlncStringBlock               * aStringBlock,
                                                  qlncNodeCommon                * aCandNode,
                                                  qlncTraceSubQueryExprList     * aSubQueryExprList,
                                                  stlInt32                        aLevel,
                                                  qllEnv                        * aEnv );

stlStatus qlncTraceTreeFromCandNodeAntiSemiJoinTable( qlncStringBlock               * aStringBlock,
                                                      qlncNodeCommon                * aCandNode,
                                                      qlncTraceSubQueryExprList     * aSubQueryExprList,
                                                      stlInt32                        aLevel,
                                                      qllEnv                        * aEnv );

stlStatus qlncTraceTreeFromCandNodeReadColumnList( qlncStringBlock              * aStringBlock,
                                                   qlncRefColumnList            * aReadColumnList,
                                                   qlncTraceSubQueryExprList    * aSubQueryExprList,
                                                   stlInt32                       aLevel,
                                                   qllEnv                       * aEnv );

stlStatus qlncTraceTreeFromCandNodeInstantColumnList( qlncStringBlock           * aStringBlock,
                                                      stlChar                   * aTitle,
                                                      qlncRefExprList           * aColumnList,
                                                      stlUInt8                  * aKeyFlags,
                                                      qlncTraceSubQueryExprList * aSubQueryExprList,
                                                      stlInt32                    aLevel,
                                                      qllEnv                    * aEnv );


/* 외부 호출함수들 */
stlStatus qlncTraceQueryFromCandNode( knlRegionMem      * aRegionMem,
                                      qlncNodeCommon    * aCandNode,
                                      qllEnv            * aEnv );

stlStatus qlncTraceTreeFromCandNode( knlRegionMem   * aRegionMem,
                                     qlncNodeCommon * aCandNode,
                                     qllEnv         * aEnv );

stlStatus qlncTraceTreeFromSubQueryExpr( knlRegionMem               * aRegionMem,
                                         qlncTraceSubQueryExprList  * aSubQueryExprList,
                                         stlInt32                     aLevel,
                                         qllEnv                     * aEnv );

/* Trace String Block 관련 */
stlStatus qlncTraceCreateStringBlock( knlRegionMem      * aRegionMem,
                                      qlncStringBlock  ** aStringBlock,
                                      qllEnv            * aEnv );

stlStatus qlncTraceAppendStringToStringBlock( qlncStringBlock   * aStringBlock,
                                              qllEnv            * aEnv,
                                              const stlChar     * aFormat,
                                              ... );

////////////////////////////////////////////////////////////////////////

/* Cost Based Optimizer 관련 */
stlStatus qlncTraceCBOptBaseTable( knlRegionMem         * aRegionMem,
                                   qlncBaseTableNode    * aBaseTable,
                                   qllEnv               * aEnv );

stlStatus qlncTraceCBOptBaseStats( qlncStringBlock      * aStringBlock,
                                   qlncBaseTableNode    * aBaseTable,
                                   qllEnv               * aEnv );

stlStatus qlncTraceCBOptBaseCost( qlncStringBlock   * aStringBlock,
                                  qlncBaseTableNode * aBaseTable,
                                  stlBool             aIsCopied,
                                  qllEnv            * aEnv );

stlStatus qlncTraceCBOptTableScanCost( qlncStringBlock      * aStringBlock,
                                       qlncTableScanCost    * aTableScanCost,
                                       stlInt32               aLevel,
                                       qllEnv               * aEnv );

stlStatus qlncTraceCBOptIndexScanCost( qlncStringBlock      * aStringBlock,
                                       qlncIndexScanCost    * aIndexScanCost,
                                       stlInt32               aLevel,
                                       qllEnv               * aEnv );

stlStatus qlncTraceCBOptRowidScanCost( qlncStringBlock      * aStringBlock,
                                       qlncRowidScanCost    * aRowidScanCost,
                                       stlInt32               aLevel,
                                       qllEnv               * aEnv );

stlStatus qlncTraceCBOptIndexCombineCost( qlncStringBlock       * aStringBlock,
                                          qlncIndexCombineCost  * aIndexCombineCost,
                                          stlInt32                aLevel,
                                          qllEnv                * aEnv );

stlStatus qlncTraceCBOptJoinOrder( knlRegionMem             * aRegionMem,
                                   stlInt32                   aSeqNum,
                                   stlInt32                   aInnerJoinNodeCount,
                                   qlncInnerJoinNodeInfo    * aInnerJoinNodeInfoArr,
                                   qllEnv                   * aEnv );

stlStatus qlncTraceCBOptJoinTableName( knlRegionMem     * aRegionMem,
                                       stlInt32           aNodeIdx,
                                       qlncNodeCommon   * aCurNode,
                                       qllEnv           * aEnv );

stlStatus qlncTraceCBOptBestJoinCost( knlRegionMem      * aRegionMem,
                                      qlncNodeCommon    * aAnalyzedNode,
                                      qllEnv            * aEnv );

stlStatus qlncTraceCBOptNestedJoinCost( knlRegionMem        * aRegionMem,
                                        qlncNestedJoinCost  * aNestedJoinCost,
                                        qlncNestedJoinCost  * aSwitchNestedJoinCost,
                                        qllEnv              * aEnv );

stlStatus qlncTraceCBOptMergeJoinCost( knlRegionMem         * aRegionMem,
                                       qlncMergeJoinCost    * aMergeJoinCost,
                                       qlncMergeJoinCost    * aSwitchMergeJoinCost,
                                       qllEnv               * aEnv );

stlStatus qlncTraceCBOptHashJoinCost( knlRegionMem      * aRegionMem,
                                      qlncHashJoinCost  * aHashJoinCost,
                                      qlncHashJoinCost  * aSwitchHashJoinCost,
                                      qllEnv            * aEnv );

stlStatus qlncTraceCBOptSortInstantCost( qlncStringBlock        * aStringBlock,
                                         qlncSortInstantNode    * aSortInstantNode,
                                         stlChar                * aCont,
                                         stlInt32                 aLevel,
                                         qllEnv                 * aEnv );

stlStatus qlncTraceCBOptHashInstantCost( qlncStringBlock        * aStringBlock,
                                         qlncHashInstantNode    * aHashInstantNode,
                                         stlChar                * aCont,
                                         stlInt32                 aLevel,
                                         qllEnv                 * aEnv );

stlStatus qlncTraceCBOptJoinTableInfo( qlncStringBlock  * aStringBlock,
                                       stlChar          * aPosName,
                                       qlncNodeCommon   * aNode,
                                       qllEnv           * aEnv );


////////////////////////////////////////////////////////////////////////

/* All Expression List */
stlStatus qlncTraceAllExprList_Internal( qlncStringBlock    * aStringBlock,
                                         qlvRefExprList     * aAllExprList,
                                         qllEnv             * aEnv );

/* Code Expression */
typedef stlStatus (*qlncTraceTreeFromCodeExprFuncPtr) ( qlncStringBlock     * aStringBlock,
                                                        qlvInitExpression   * aExpr,
                                                        stlInt32              aLevel,
                                                        qllEnv              * aEnv );

extern const qlncTraceTreeFromCodeExprFuncPtr qlncTraceTreeFromCodeExprFuncList[ QLV_EXPR_TYPE_MAX ];

stlStatus qlncTraceTreeFromCodeExpr_Value( qlncStringBlock      * aStringBlock,
                                           qlvInitExpression    * aExpr,
                                           stlInt32               aLevel,
                                           qllEnv               * aEnv );

stlStatus qlncTraceTreeFromCodeExpr_BindParam( qlncStringBlock      * aStringBlock,
                                               qlvInitExpression    * aExpr,
                                               stlInt32               aLevel,
                                               qllEnv               * aEnv );

stlStatus qlncTraceTreeFromCodeExpr_Column( qlncStringBlock     * aStringBlock,
                                            qlvInitExpression   * aExpr,
                                            stlInt32              aLevel,
                                            qllEnv              * aEnv );

stlStatus qlncTraceTreeFromCodeExpr_Function( qlncStringBlock   * aStringBlock,
                                              qlvInitExpression * aExpr,
                                              stlInt32            aLevel,
                                              qllEnv            * aEnv );

stlStatus qlncTraceTreeFromCodeExpr_Cast( qlncStringBlock   * aStringBlock,
                                          qlvInitExpression * aExpr,
                                          stlInt32            aLevel,
                                          qllEnv            * aEnv );

stlStatus qlncTraceTreeFromCodeExpr_PseudoColumn( qlncStringBlock   * aStringBlock,
                                                  qlvInitExpression * aExpr,
                                                  stlInt32            aLevel,
                                                  qllEnv            * aEnv );

stlStatus qlncTraceTreeFromCodeExpr_PseudoArgument( qlncStringBlock     * aStringBlock,
                                                    qlvInitExpression   * aExpr,
                                                    stlInt32              aLevel,
                                                    qllEnv              * aEnv );

stlStatus qlncTraceTreeFromCodeExpr_ConstantExprResult( qlncStringBlock     * aStringBlock,
                                                        qlvInitExpression   * aExpr,
                                                        stlInt32              aLevel,
                                                        qllEnv              * aEnv );

stlStatus qlncTraceTreeFromCodeExpr_Reference( qlncStringBlock      * aStringBlock,
                                               qlvInitExpression    * aExpr,
                                               stlInt32               aLevel,
                                               qllEnv               * aEnv );

stlStatus qlncTraceTreeFromCodeExpr_SubQuery( qlncStringBlock   * aStringBlock,
                                              qlvInitExpression * aExpr,
                                              stlInt32            aLevel,
                                              qllEnv            * aEnv );

stlStatus qlncTraceTreeFromCodeExpr_InnerColumn( qlncStringBlock    * aStringBlock,
                                                 qlvInitExpression  * aExpr,
                                                 stlInt32             aLevel,
                                                 qllEnv             * aEnv );

stlStatus qlncTraceTreeFromCodeExpr_OuterColumn( qlncStringBlock    * aStringBlock,
                                                 qlvInitExpression  * aExpr,
                                                 stlInt32             aLevel,
                                                 qllEnv             * aEnv );

stlStatus qlncTraceTreeFromCodeExpr_RowidColumn( qlncStringBlock    * aStringBlock,
                                                 qlvInitExpression  * aExpr,
                                                 stlInt32             aLevel,
                                                 qllEnv             * aEnv );

stlStatus qlncTraceTreeFromCodeExpr_Aggregation( qlncStringBlock    * aStringBlock,
                                                 qlvInitExpression  * aExpr,
                                                 stlInt32             aLevel,
                                                 qllEnv             * aEnv );

stlStatus qlncTraceTreeFromCodeExpr_CaseExpr( qlncStringBlock   * aStringBlock,
                                              qlvInitExpression * aExpr,
                                              stlInt32            aLevel,
                                              qllEnv            * aEnv );

stlStatus qlncTraceTreeFromCodeExpr_ListFunction( qlncStringBlock   * aStringBlock,
                                                  qlvInitExpression * aExpr,
                                                  stlInt32            aLevel,
                                                  qllEnv            * aEnv );

stlStatus qlncTraceTreeFromCodeExpr_ListColumn( qlncStringBlock     * aStringBlock,
                                                qlvInitExpression   * aExpr,
                                                stlInt32              aLevel,
                                                qllEnv              * aEnv );

stlStatus qlncTraceTreeFromCodeExpr_RowExpr( qlncStringBlock    * aStringBlock,
                                             qlvInitExpression  * aExpr,
                                             stlInt32             aLevel,
                                             qllEnv             * aEnv );

/* Code Plan */
typedef stlStatus (*qlncTraceTreeFromCodePlanFuncPtr) ( qlncStringBlock     * aStringBlock,
                                                        qllOptimizationNode * aCodePlan,
                                                        stlInt32              aLevel,
                                                        qllEnv              * aEnv );

extern const qlncTraceTreeFromCodePlanFuncPtr qlncTraceTreeFromCodePlanFuncList[ QLL_PLAN_NODE_TYPE_MAX ];

stlStatus qlncTraceTreeFromCodePlan_SelectStmt( qlncStringBlock     * aStringBlock,
                                                qllOptimizationNode * aCodePlan,
                                                stlInt32              aLevel,
                                                qllEnv              * aEnv );

stlStatus qlncTraceTreeFromCodePlan_InsertStmt( qlncStringBlock     * aStringBlock,
                                                qllOptimizationNode * aCodePlan,
                                                stlInt32              aLevel,
                                                qllEnv              * aEnv );

stlStatus qlncTraceTreeFromCodePlan_UpdateStmt( qlncStringBlock     * aStringBlock,
                                                qllOptimizationNode * aCodePlan,
                                                stlInt32              aLevel,
                                                qllEnv              * aEnv );

stlStatus qlncTraceTreeFromCodePlan_DeleteStmt( qlncStringBlock     * aStringBlock,
                                                qllOptimizationNode * aCodePlan,
                                                stlInt32              aLevel,
                                                qllEnv              * aEnv );

stlStatus qlncTraceTreeFromCodePlan_QuerySpec( qlncStringBlock      * aStringBlock,
                                               qllOptimizationNode  * aCodePlan,
                                               stlInt32               aLevel,
                                               qllEnv               * aEnv );

stlStatus qlncTraceTreeFromCodePlan_QuerySet( qlncStringBlock       * aStringBlock,
                                              qllOptimizationNode   * aCodePlan,
                                              stlInt32                aLevel,
                                              qllEnv                * aEnv );

stlStatus qlncTraceTreeFromCodePlan_UnionAll( qlncStringBlock       * aStringBlock,
                                              qllOptimizationNode   * aCodePlan,
                                              stlInt32                aLevel,
                                              qllEnv                * aEnv );

stlStatus qlncTraceTreeFromCodePlan_UnionDistinct( qlncStringBlock      * aStringBlock,
                                                   qllOptimizationNode  * aCodePlan,
                                                   stlInt32               aLevel,
                                                   qllEnv               * aEnv );

stlStatus qlncTraceTreeFromCodePlan_ExceptAll( qlncStringBlock      * aStringBlock,
                                               qllOptimizationNode  * aCodePlan,
                                               stlInt32               aLevel,
                                               qllEnv               * aEnv );

stlStatus qlncTraceTreeFromCodePlan_ExceptDistinct( qlncStringBlock     * aStringBlock,
                                                    qllOptimizationNode * aCodePlan,
                                                    stlInt32              aLevel,
                                                    qllEnv              * aEnv );

stlStatus qlncTraceTreeFromCodePlan_IntersectAll( qlncStringBlock       * aStringBlock,
                                                  qllOptimizationNode   * aCodePlan,
                                                  stlInt32                aLevel,
                                                  qllEnv                * aEnv );

stlStatus qlncTraceTreeFromCodePlan_IntersectDistinct( qlncStringBlock      * aStringBlock,
                                                       qllOptimizationNode  * aCodePlan,
                                                       stlInt32               aLevel,
                                                       qllEnv               * aEnv );

stlStatus qlncTraceTreeFromCodePlan_SubQuery( qlncStringBlock       * aStringBlock,
                                              qllOptimizationNode   * aCodePlan,
                                              stlInt32                aLevel,
                                              qllEnv                * aEnv );

stlStatus qlncTraceTreeFromCodePlan_SubQueryList( qlncStringBlock       * aStringBlock,
                                                  qllOptimizationNode   * aCodePlan,
                                                  stlInt32                aLevel,
                                                  qllEnv                * aEnv );

stlStatus qlncTraceTreeFromCodePlan_SubQueryFunc( qlncStringBlock       * aStringBlock,
                                                  qllOptimizationNode   * aCodePlan,
                                                  stlInt32                aLevel,
                                                  qllEnv                * aEnv );

stlStatus qlncTraceTreeFromCodePlan_SubQueryFilter( qlncStringBlock     * aStringBlock,
                                                    qllOptimizationNode * aCodePlan,
                                                    stlInt32              aLevel,
                                                    qllEnv              * aEnv );

stlStatus qlncTraceTreeFromCodePlan_Concat( qlncStringBlock     * aStringBlock,
                                            qllOptimizationNode * aCodePlan,
                                            stlInt32              aLevel,
                                            qllEnv              * aEnv );

stlStatus qlncTraceTreeFromCodePlan_NestedLoops( qlncStringBlock        * aStringBlock,
                                                 qllOptimizationNode    * aCodePlan,
                                                 stlInt32                 aLevel,
                                                 qllEnv                 * aEnv );

stlStatus qlncTraceTreeFromCodePlan_MergeJoin( qlncStringBlock      * aStringBlock,
                                               qllOptimizationNode  * aCodePlan,
                                               stlInt32               aLevel,
                                               qllEnv               * aEnv );

stlStatus qlncTraceTreeFromCodePlan_HashJoin( qlncStringBlock       * aStringBlock,
                                              qllOptimizationNode   * aCodePlan,
                                              stlInt32                aLevel,
                                              qllEnv                * aEnv );

stlStatus qlncTraceTreeFromCodePlan_HashAggregation( qlncStringBlock        * aStringBlock,
                                                     qllOptimizationNode    * aCodePlan,
                                                     stlInt32                 aLevel,
                                                     qllEnv                 * aEnv );

stlStatus qlncTraceTreeFromCodePlan_Group( qlncStringBlock      * aStringBlock,
                                           qllOptimizationNode  * aCodePlan,
                                           stlInt32               aLevel,
                                           qllEnv               * aEnv );

stlStatus qlncTraceTreeFromCodePlan_TableAccess( qlncStringBlock        * aStringBlock,
                                                 qllOptimizationNode    * aCodePlan,
                                                 stlInt32                 aLevel,
                                                 qllEnv                 * aEnv );

stlStatus qlncTraceTreeFromCodePlan_IndexAccess( qlncStringBlock        * aStringBlock,
                                                 qllOptimizationNode    * aCodePlan,
                                                 stlInt32                 aLevel,
                                                 qllEnv                 * aEnv );

stlStatus qlncTraceTreeFromCodePlan_RowidAccess( qlncStringBlock        * aStringBlock,
                                                 qllOptimizationNode    * aCodePlan,
                                                 stlInt32                 aLevel,
                                                 qllEnv                 * aEnv );

stlStatus qlncTraceTreeFromCodePlan_FastDualAccess( qlncStringBlock     * aStringBlock,
                                                    qllOptimizationNode * aCodePlan,
                                                    stlInt32              aLevel,
                                                    qllEnv              * aEnv );

stlStatus qlncTraceTreeFromCodePlan_FlatInstantAccess( qlncStringBlock      * aStringBlock,
                                                       qllOptimizationNode  * aCodePlan,
                                                       stlInt32               aLevel,
                                                       qllEnv               * aEnv );

stlStatus qlncTraceTreeFromCodePlan_SortInstantAccess( qlncStringBlock      * aStringBlock,
                                                       qllOptimizationNode  * aCodePlan,
                                                       stlInt32               aLevel,
                                                       qllEnv               * aEnv );

stlStatus qlncTraceTreeFromCodePlan_HashInstantAccess( qlncStringBlock      * aStringBlock,
                                                       qllOptimizationNode  * aCodePlan,
                                                       stlInt32               aLevel,
                                                       qllEnv               * aEnv );

stlStatus qlncTraceTreeFromCodePlan_GroupHashInstantAccess( qlncStringBlock     * aStringBlock,
                                                            qllOptimizationNode * aCodePlan,
                                                            stlInt32              aLevel,
                                                            qllEnv              * aEnv );

stlStatus qlncTraceTreeFromCodePlan_GroupSortInstantAccess( qlncStringBlock     * aStringBlock,
                                                            qllOptimizationNode * aCodePlan,
                                                            stlInt32              aLevel,
                                                            qllEnv              * aEnv );

stlStatus qlncTraceTreeFromCodePlan_SortJoinInstantAccess( qlncStringBlock      * aStringBlock,
                                                           qllOptimizationNode  * aCodePlan,
                                                           stlInt32               aLevel,
                                                           qllEnv               * aEnv );

stlStatus qlncTraceTreeFromCodePlan_HashJoinInstantAccess( qlncStringBlock      * aStringBlock,
                                                           qllOptimizationNode  * aCodePlan,
                                                           stlInt32               aLevel,
                                                           qllEnv               * aEnv );

stlStatus qlncTraceTreeFromCodePlan_Expr( qlncStringBlock   * aStringBlock,
                                          stlChar           * aTitle,
                                          qlvInitExpression * aExpr,
                                          stlInt32            aLevel,
                                          qllEnv            * aEnv );

stlStatus qlncTraceTreeFromCodePlan_RefExprList( qlncStringBlock    * aStringBlock,
                                                 stlChar            * aTitle,
                                                 qlvRefExprList     * aRefExprList,
                                                 stlInt32             aLevel,
                                                 qllEnv             * aEnv );

stlStatus qlncTraceTreeFromCodePlan_IndexScanInfo( qlncStringBlock  * aStringBlock,
                                                   qloIndexScanInfo * aIndexScanInfo,
                                                   stlInt32           aLevel,
                                                   qllEnv           * aEnv );

stlStatus qlncTraceTreeFromCodePlan_HashScanInfo( qlncStringBlock   * aStringBlock,
                                                  qloHashScanInfo   * aHashScanInfo,
                                                  stlInt32            aLevel,
                                                  qllEnv            * aEnv );

stlStatus qlncTraceTreeFromCodePlan_Instant( qlncStringBlock      * aStringBlock,
                                             qllOptimizationNode  * aCodePlan,
                                             stlInt32               aLevel,
                                             qllEnv               * aEnv );

/* 외부 호출 함수들 */

stlStatus qlncTraceAllExprList( knlRegionMem    * aRegionMem,
                                qlvRefExprList  * aAllExprList,
                                qllEnv          * aEnv );

stlStatus qlncTraceTreeFromCodePlan( knlRegionMem           * aRegionMem,
                                     qllOptimizationNode    * aCodePlan,
                                     qllEnv                 * aEnv );

stlStatus qlncTraceExplainPlan( qllExplainNodeText  * aPlanText,
                                qllEnv              * aEnv );

/** @} qlnc */

#endif /* _QLNCNODE_H_ */
