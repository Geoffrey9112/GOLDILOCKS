/*******************************************************************************
 * qlncCostBasedQueryTransformation.h
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

#ifndef _QLNCCOSTBASEDQUERYTRANSFORMATION_H_
#define _QLNCCOSTBASEDQUERYTRANSFORMATION_H_ 1

/**
 * @file qlncCostBasedQueryTransformation.h
 * @brief SQL Processor Layer Cost Based Query Transformation
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

stlStatus qlncCostBasedQueryTransformation( qlncParamInfo   * aParamInfo,
                                            qlncNodeCommon  * aNode,
                                            qllEnv          * aEnv );

#if 0

/* In Relation SubQuery Rewrite 함수들 */
stlStatus qlncRewriteInRelationSubQuery( qlncParamInfo  * aParamInfo,
                                         qlncNodeCommon * aNode,
                                         qllEnv         * aEnv );

stlStatus qlncRewriteInRelationSubQueryFilter( qlncParamInfo    * aParamInfo,
                                               qlncAndFilter    * aAndFilter,
                                               qllEnv           * aEnv );

stlStatus qlncRewriteInRelationSubQueryNonFilter( qlncParamInfo     * aParamInfo,
                                                  qlncExprCommon    * aNonFilterExpr,
                                                  qlncExprCommon   ** aOutExpr,
                                                  qllEnv            * aEnv );

#endif

// stlStatus qlncRewriteInRelationSubQueryCore( qlncParamInfo      * aParamInfo,
//                                              qlncExprCommon     * aExpr,
//                                              qlncExprCommon    ** aOutExpr,
//                                              qllEnv             * aEnv );

// stlStatus qlncRewriteInRelationSubQueryIn( qlncParamInfo      * aParamInfo,
//                                            qlncExprCommon     * aExpr,
//                                            qlncExprCommon    ** aOutExpr,
//                                            qllEnv             * aEnv );

// stlStatus qlncRewriteInRelationSubQueryExists( qlncParamInfo      * aParamInfo,
//                                                qlncExprCommon     * aExpr,
//                                                qlncExprCommon    ** aOutExpr,
//                                                qllEnv             * aEnv );

// stlStatus qlncRewriteInRelationSubQueryAny( qlncParamInfo      * aParamInfo,
//                                             qlncExprCommon     * aExpr,
//                                             qlncExprCommon    ** aOutExpr,
//                                             qllEnv             * aEnv );

// stlStatus qlncRewriteInRelationSubQueryAll( qlncParamInfo      * aParamInfo,
//                                             qlncExprCommon     * aExpr,
//                                             qlncExprCommon    ** aOutExpr,
//                                             qllEnv             * aEnv );

//stlStatus qlncRewriteInRelationSubQueryRowComp( qlncParamInfo      * aParamInfo,
//                                                qlncExprCommon     * aExpr,
//                                                qlncExprCommon    ** aOutExpr,
//                                                qllEnv             * aEnv );

/* SubQuery에 대한 Unnest 함수들 */
//stlStatus qlncSubQueryUnnest( qlncParamInfo     * aParamInfo,
//                              qlncNodeCommon    * aNode,
//                              qlncNodeCommon   ** aOutNode,
//                              qllEnv            * aEnv );

//stlStatus qlncSQUTableFilter( qlncParamInfo     * aParamInfo,
//                              qlncNodeCommon    * aNode,
//                              qlncNodeCommon   ** aOutNode,
//                              qllEnv            * aEnv );

//stlStatus qlncSQUExtractAndFilter( qlncParamInfo    * aParamInfo,
//                                   qlncNodeCommon   * aNode,
//                                   qlncAndFilter   ** aAndFilter,
//                                   qlncAndFilter  *** aAndFilterPtr,
//                                   qllEnv           * aEnv );

//void qlncSQUGetExprInfoFromNode( qlncNodeCommon * aNode,
//                                 qlncNodeArray  * aNodeArray,
//                                 stlInt32       * aInnerTableColumnCount,
//                                 stlInt32       * aOuterTableColumnCount,
//                                 stlBool        * aHasAggregation );

//void qlncSQUGetExprInfoFromExpr( qlncExprCommon * aExpr,
//                                 qlncNodeArray  * aNodeArray,
//                                 stlInt32       * aInnerTableColumnCount,
//                                 stlInt32       * aOuterTableColumnCount,
//                                 stlBool        * aHasAggregation );

//stlStatus qlncSQUMakeSemiJoinAndFilter( qlncParamInfo       * aParamInfo,
//                                        qlncExprListFunc    * aListFunc,
//                                        qlncExprCommon      * aLeftExpr,
//                                        stlInt32              aRightTargetCount,
//                                        qlncTarget          * aRightTargetArr,
//                                        knlBuiltInFunc        aSetFuncId,
//                                        qlncAndFilter       * aOuterAndFilter,
//                                        qlncAndFilter      ** aSemiJoinAndFilter,
//                                        qllEnv              * aEnv );

/** @} qlnc */

#endif /* _QLNCCOSTBASEDQUERYTRANSFORMATION_H_ */
