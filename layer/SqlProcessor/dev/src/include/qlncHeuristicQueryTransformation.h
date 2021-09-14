/*******************************************************************************
 * qlncHeuristicQueryTransformation.h
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

#ifndef _QLNCHEURISTICQUERYTRANSFORMATION_H_
#define _QLNCHEURISTICQUERYTRANSFORMATION_H_ 1

/**
 * @file qlncHeuristicQueryTransformation.h
 * @brief SQL Processor Layer Heuristic Query Transformation
 */

#include <qlDef.h>

/**
 * @addtogroup qlnc
 * @{
 */


/*******************************************************************************
 * DEFINITIONS
 ******************************************************************************/



/*******************************************************************************
 * SRUCTURES
 ******************************************************************************/



/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

stlStatus qlncHeuristicQueryTransformation( qlncParamInfo   * aParamInfo,
                                            qlncNodeCommon  * aNode,
                                            qllEnv          * aEnv );

/* Rewrite Target On Exists */
stlStatus qlncRewriteTargetOnExists( qlncParamInfo  * aParamInfo,
                                     qlncNodeCommon * aNode,
                                     qllEnv         * aEnv );

stlStatus qlncRTEAndFilter( qlncParamInfo   * aParamInfo,
                            qlncAndFilter   * aAndFilter,
                            qllEnv          * aEnv );

qlncExprCommon * qlncRTEGetAggrExpr( qlncExprCommon * aExpr );

/* Transitive Predicate Generation */
stlStatus qlncTransitivePredicateGeneration( qlncParamInfo  * aParamInfo,
                                             qlncNodeCommon * aNode,
                                             qllEnv         * aEnv );

stlStatus qlncTPGExpression( qlncParamInfo  * aParamInfo,
                             qlncAndFilter  * aAndFilter,
                             qllEnv         * aEnv );

/* Filter Push Down */
stlStatus qlncFilterPushDown( qlncParamInfo     * aParamInfo,
                              qlncNodeCommon    * aNode,
                              qllEnv            * aEnv );

stlStatus qlncFPDSubQueryOnAndFilter( qlncParamInfo * aParamInfo,
                                      qlncAndFilter * aAndFilter,
                                      qllEnv        * aEnv );

stlStatus qlncFPDSubQueryExprList( qlncParamInfo    * aParamInfo,
                                   qlncRefExprList  * aSubQueryExprList,
                                   qllEnv           * aEnv );

stlStatus qlncFPDSubTableNode( qlncParamInfo        * aParamInfo,
                               qlncSubTableNode     * aSubTableNode,
                               qllEnv               * aEnv );

stlStatus qlncFPDNonOuterJoinTableNode( qlncParamInfo       * aParamInfo,
                                        qlncJoinTableNode   * aJoinNode,
                                        qllEnv              * aEnv );

stlStatus qlncFPDOuterJoinTableNode( qlncParamInfo          * aParamInfo,
                                     qlncJoinTableNode      * aJoinNode,
                                     qllEnv                 * aEnv );

stlStatus qlncFPDSortInstantNode( qlncParamInfo         * aParamInfo,
                                  qlncSortInstantNode   * aSortInstantNode,
                                  qllEnv                * aEnv );

stlStatus qlncFPDHashInstantNode( qlncParamInfo         * aParamInfo,
                                  qlncHashInstantNode   * aHashInstantNode,
                                  qllEnv                * aEnv );

stlStatus qlncFPDChangeInnerColumnToTargetExpr( qlncParamInfo   * aParamInfo,
                                                stlInt32          aNodeID,
                                                stlInt32          aTargetCount,
                                                qlncTarget      * aTargetArr,
                                                qlncExprCommon  * aExpr,
                                                qlncExprCommon ** aOutExpr,
                                                qllEnv          * aEnv );

stlStatus qlncFPDChangeInnerColumnToOrgExpr( qlncParamInfo      * aParamInfo,
                                             qlncNodeCommon     * aNode,
                                             qlncExprCommon     * aExpr,
                                             qlncExprCommon    ** aOutExpr,
                                             qllEnv             * aEnv );

void qlncFPDSetUsedTargetIdxOnFilter( stlInt32            aNodeID,
                                      stlInt32            aTargetCount,
                                      stlInt32          * aTargetIdxArr,
                                      qlncExprCommon    * aExpr );

stlBool qlncFPDCheckQueryNode( qlncTarget       * aTargetArr,
                               stlInt32         * aTargetIdxArr,
                               qlncNodeCommon   * aNode );

stlStatus qlncFPDPushFilterOnQueryNode( qlncParamInfo       * aParamInfo,
                                        qlncSubTableNode    * aSubTableNode,
                                        qlncNodeCommon      * aNode,
                                        qlncAndFilter       * aPushFilter,
                                        qllEnv              * aEnv );

/* Single Table Min/Max Aggregation Conversion */
stlStatus qlncSingleTableAggregationConversion( qlncParamInfo   * aParamInfo,
                                                qlncNodeCommon  * aNode,
                                                qllEnv          * aEnv );

stlStatus qlncSTACSubQueryOnAndFilter( qlncParamInfo    * aParamInfo,
                                       qlncAndFilter    * aAndFilter,
                                       qllEnv           * aEnv );

stlStatus qlncSTACSubQueryExprList( qlncParamInfo   * aParamInfo,
                                    qlncRefExprList * aSubQueryExprList,
                                    qllEnv          * aEnv );

stlStatus qlncSTACQuerySpec( qlncParamInfo  * aParamInfo,
                             qlncQuerySpec  * aQuerySpec,
                             qllEnv         * aEnv );

/* Quantify Operator Elemination */
stlStatus qlncQuantifyOperatorElemination( qlncParamInfo    * aParamInfo,
                                           qlncNodeCommon   * aNode,
                                           qllEnv           * aEnv );

stlStatus qlncQOEOnAndFilter( qlncParamInfo * aParamInfo,
                              qlncAndFilter * aAndFilter,
                              qllEnv        * aEnv );

stlStatus qlncQOEListFunctionToFunction( qlncParamInfo      * aParamInfo,
                                         qlncExprListFunc   * aListFunc,
                                         qlncExprCommon    ** aFunction,
                                         qllEnv             * aEnv );

stlStatus qlncQOESubQueryExprList( qlncParamInfo    * aParamInfo,
                                   qlncRefExprList  * aSubQueryExprList,
                                   qllEnv           * aEnv );

stlStatus qlncQOEAddAggregationOnHashInstant( qlncParamInfo         * aParamInfo,
                                              qlncHashInstantNode   * aHashInstant,
                                              qlncExprCommon        * aAggregation,
                                              qlncExprCommon       ** aOutExpr,
                                              qllEnv                * aEnv );


/** @} qlnc */

#endif /* _QLNCHEURISTICQUERYTRANSFORMATION_H_ */
