/*******************************************************************************
 * qlncPostOptimizer.h
 *
 * Copyright (c) 2014, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *
 *
 ******************************************************************************/

#ifndef _QLNCPOSTOPTIMIZER_H_
#define _QLNCPOSTOPTIMIZER_H_ 1

/**
 * @file qlncPostOptimizer.h
 * @brief SQL Processor Layer Post Optimizer
 */

#include <qlDef.h>

/**
 * @addtogroup qlnc
 * @{
 */


/*******************************************************************************
 * DEFINITIONS
 ******************************************************************************/

#define QLNC_UNNEST_AND_FILTER( _aAndFilter )                           \
    {                                                                   \
        if( (_aAndFilter) != NULL )                                     \
        {                                                               \
            STL_TRY( qlncPostOptSQUExpr( aParamInfo,                    \
                                         (qlncExprCommon*)(_aAndFilter),\
                                         aEnv )                         \
                     == STL_SUCCESS );                                  \
        }                                                               \
    }


/*******************************************************************************
 * SRUCTURES
 ******************************************************************************/



/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

stlStatus qlncPostOptimizer( qlncParamInfo  * aParamInfo,
                             qlncNodeCommon * aNode,
                             qllEnv         * aEnv );

stlStatus qlncPostOptimizer_DML( qlncParamInfo      * aParamInfo,
                                 qlncNodeCommon     * aNode,
                                 qlncNodeCommon    ** aOutNode,
                                 qllEnv             * aEnv );

/* SubQuery에 대한 Unnest 함수들 */
stlStatus qlncPostOptSubQueryUnnest( qlncParamInfo      * aParamInfo,
                                     qlncNodeCommon     * aNode,
                                     stlFloat64           aWeightValue,
                                     qlncNodeCommon    ** aOutNode,
                                     qllEnv             * aEnv );

stlStatus qlncPostOptSQUBaseTable( qlncParamInfo    * aParamInfo,
                                   qlncNodeCommon   * aNode,
                                   stlFloat64         aWeightValue,
                                   qlncNodeCommon  ** aOutNode,
                                   qllEnv           * aEnv );

stlStatus qlncPostOptSQUSubTable( qlncParamInfo     * aParamInfo,
                                  qlncNodeCommon    * aNode,
                                  stlFloat64          aWeightValue,
                                  qlncNodeCommon   ** aOutNode,
                                  qllEnv            * aEnv );

stlStatus qlncPostOptSQUJoinTable( qlncParamInfo    * aParamInfo,
                                   qlncNodeCommon   * aNode,
                                   stlFloat64         aWeightValue,
                                   qlncNodeCommon  ** aOutNode,
                                   qllEnv           * aEnv );

stlStatus qlncPostOptSQUExpr( qlncParamInfo     * aParamInfo,
                              qlncExprCommon    * aExpr,
                              qllEnv            * aEnv );

stlStatus qlncPostOptSQUMakeUnnestNode( qlncParamInfo   * aParamInfo,
                                        qlncNodeCommon  * aLeftNode,
                                        stlFloat64        aWeightValue,
                                        qlncExprCommon  * aSubQueryFilter,
                                        qlncAndFilter   * aSubQueryAndFilter,
                                        qlncNodeCommon ** aOutNode,
                                        qllEnv          * aEnv );

stlStatus qlncPostOptExtractSubQueryFilter( qlncParamInfo       * aParamInfo,
                                            qlncNodeCommon      * aCurrNode,
                                            qlncAndFilter       * aAndFilter,
                                            qlncAndFilter      ** aOutAndFilter,
                                            qlncRefExprList     * aSubQueryFilterList,
                                            qlncAndFilter      ** aOutSubQueryAndFilter,
                                            qllEnv              * aEnv );

stlStatus qlncPostOptSQUExtractOuterAndFilter( qlncParamInfo    * aParamInfo,
                                               qlncNodeCommon   * aNode,
                                               qlncNodeArray    * aRefNodeArray,
                                               qlncAndFilter    * aOuterAndFilter,
                                               qllEnv           * aEnv );

stlStatus qlncPostOptSQUMakeSemiJoinAndFilter( qlncParamInfo        * aParamInfo,
                                               qlncExprListFunc     * aListFunc,
                                               qlncExprCommon       * aLeftExpr,
                                               stlInt32               aRightTargetCount,
                                               qlncRefColumnItem    * aRightRefColumnItem,
                                               qlncTarget           * aRightTargetArr,
                                               knlBuiltInFunc         aSetFuncId,
                                               qlncAndFilter        * aOuterAndFilter,
                                               qlncAndFilter       ** aSemiJoinAndFilter,
                                               qllEnv               * aEnv );

stlBool qlncPostOptSQUIsPossibleAntiSemi( qlncExprCommon    * aExpr );

stlStatus qlncPostOptSQUIsPossibleHashAntiSemi( stlChar         * aSQLString,
                                                qlncExprRowExpr * aLeftRowExpr,
                                                qlncQuerySpec   * aRightQuerySpec,
                                                stlInt32          aExprPos,
                                                stlBool         * aIsPossible,
                                                qllEnv          * aEnv );

/* Post Filter Push Down */
stlStatus qlncPostOptFilterPushDown( qlncParamInfo  * aParamInfo,
                                     qlncNodeCommon * aNode,
                                     qlncJoinType     aJoinType,
                                     qllEnv         * aEnv );

stlStatus qlncPostOptFPDSubQueryExprList( qlncParamInfo     * aParamInfo,
                                          qlncRefExprList   * aSubQueryExprList,
                                          qllEnv            * aEnv );

stlStatus qlncPostOptFPDSubQueryOnAndFilter( qlncParamInfo  * aParamInfo,
                                             qlncAndFilter  * aAndFilter,
                                             qllEnv         * aEnv );


stlStatus qlncPostOptFPDSubTableNode( qlncParamInfo     * aParamInfo,
                                      qlncSubTableNode  * aSubTableNode,
                                      qllEnv            * aEnv );

stlStatus qlncPostOptFPDNestedJoin( qlncParamInfo   * aParamInfo,
                                    qlncNodeCommon  * aNode,
                                    qlncJoinType      aJoinType,
                                    qllEnv          * aEnv );

stlStatus qlncPostOptFPDMergeJoin( qlncParamInfo    * aParamInfo,
                                   qlncNodeCommon   * aNode,
                                   qlncJoinType       aJoinType,
                                   qllEnv           * aEnv );

stlStatus qlncPostOptFPDHashJoin( qlncParamInfo     * aParamInfo,
                                  qlncNodeCommon    * aNode,
                                  qlncJoinType        aJoinType,
                                  qllEnv            * aEnv );

#if 0
stlStatus qlncPostOptFPDApplyToChildNode( qlncParamInfo     * aParamInfo,
                                          qlncNodeCommon    * aChildNode,
                                          qlncOrFilter      * aPushOrFilter,
                                          qllEnv            * aEnv );
#endif

#if 0
stlStatus qlncPostOptFindNodeForPushDownNonSubQueryFilter( qlncNodeCommon   * aCurNode,
                                                           qlncOrFilter     * aOrFilter,
                                                           qlncNodeCommon  ** aFoundNode,
                                                           qllEnv           * aEnv );
#endif

stlStatus qlncPostOptMakeNodeArrayOnExpr( qlncNodeArray     * aNodeArray,
                                          qlncExprCommon    * aExpr,
                                          qllEnv            * aEnv );

stlStatus qlncPostOptMakeNodeArrayOnCurrNode( qlncNodeArray     * aNodeArray,
                                              qlncNodeCommon    * aNode,
                                              qllEnv            * aEnv );

/* Group by Pre-ordered Node */
stlStatus qlncPostOptGroupByPreorderedNode( qlncParamInfo   * aParamInfo,
                                            qlncNodeCommon  * aNode,
                                            qlncOtherHint   * aOtherHint,
                                            qllEnv          * aEnv );

stlStatus qlncPostOptGBPNSubQueryExprList( qlncParamInfo    * aParamInfo,
                                           qlncRefExprList  * aSubQueryExprList,
                                           qllEnv           * aEnv );

stlStatus qlncPostOptGBPNSubQueryOnAndFilter( qlncParamInfo * aParamInfo,
                                              qlncAndFilter * aAndFilter,
                                              qllEnv        * aEnv );

stlStatus qlncPostOptGBPNGroupHashInstant( qlncParamInfo    * aParamInfo,
                                           qlncNodeCommon   * aNode,
                                           qllEnv           * aEnv );

stlStatus qlncPostOptGBPNDistinctWithGroupHashInstant( qlncParamInfo    * aParamInfo,
                                                       qlncNodeCommon   * aNode,
                                                       qllEnv           * aEnv );

stlStatus qlncPostOptGBPNFindUsablePreorderedNode( qlncParamInfo    * aParamInfo,
                                                   qlncNodeCommon   * aNode,
                                                   qlncNodeCommon  ** aUsableNode,
                                                   qllEnv           * aEnv );

stlStatus qlncPostOptGBPNIsUsableIndexForGroupBy( qlncParamInfo     * aParamInfo,
                                                  qlncRefExprList   * aKeyColList,
                                                  stlInt32            aNodeID,
                                                  qlncIndexStat     * aIndexStat,
                                                  stlBool           * aIsUsable,
                                                  qllEnv            * aEnv );

/** @} qlnc */

#endif /* _QLNCPOSTOPTIMIZER_H_ */
