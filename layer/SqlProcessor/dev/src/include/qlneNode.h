/*******************************************************************************
 * qlneNode.h
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

#ifndef _QLNENODE_H_
#define _QLNENODE_H_ 1

/**
 * @file qlneNode.h
 * @brief SQL Processor Layer Explain Node
 */

#include <qlDef.h>

/**
 * @addtogroup qlne
 * @{
 */


#define QLNE_BUILT_IN_FUNC_NAME_MAX_SIZE  (32)


typedef stlStatus (*qlneExplainNode) ( qllOptimizationNode   * aOptNode,
                                       qllDataArea           * aDataArea,
                                       knlRegionMem          * aRegionMem,
                                       stlUInt32               aDepth,
                                       qllExplainNodeText   ** aPlanText,
                                       qllEnv                * aEnv );

extern qlneExplainNode qlneExplainNodeList[ QLL_PLAN_NODE_TYPE_MAX ];

extern const stlChar qlneBuiltInFuncName[ KNL_BUILTIN_FUNC_MAX ][ QLNE_BUILT_IN_FUNC_NAME_MAX_SIZE ];

extern const stlChar qlneAggrFuncName[ KNL_BUILTIN_AGGREGATION_UNARY_MAX ][ QLNE_BUILT_IN_FUNC_NAME_MAX_SIZE ];

extern const stlChar qlneJoinTypeName[ QLV_JOIN_TYPE_MAX ][ QLNE_BUILT_IN_FUNC_NAME_MAX_SIZE ];


stlStatus qlneExplainSelectStmt( qllOptimizationNode   * aOptNode,
                                 qllDataArea           * aDataArea,
                                 knlRegionMem          * aRegionMem,
                                 stlUInt32               aDepth,
                                 qllExplainNodeText   ** aPlanText,
                                 qllEnv                * aEnv );

stlStatus qlneExplainInsertStmt( qllOptimizationNode   * aOptNode,
                                 qllDataArea           * aDataArea,
                                 knlRegionMem          * aRegionMem,
                                 stlUInt32               aDepth,
                                 qllExplainNodeText   ** aPlanText,
                                 qllEnv                * aEnv );

stlStatus qlneExplainUpdateStmt( qllOptimizationNode   * aOptNode,
                                 qllDataArea           * aDataArea,
                                 knlRegionMem          * aRegionMem,
                                 stlUInt32               aDepth,
                                 qllExplainNodeText   ** aPlanText,
                                 qllEnv                * aEnv );

stlStatus qlneExplainDeleteStmt( qllOptimizationNode   * aOptNode,
                                 qllDataArea           * aDataArea,
                                 knlRegionMem          * aRegionMem,
                                 stlUInt32               aDepth,
                                 qllExplainNodeText   ** aPlanText,
                                 qllEnv                * aEnv );

stlStatus qlneExplainQuerySpec( qllOptimizationNode   * aOptNode,
                                qllDataArea           * aDataArea,
                                knlRegionMem          * aRegionMem,
                                stlUInt32               aDepth,
                                qllExplainNodeText   ** aPlanText,
                                qllEnv                * aEnv );

stlStatus qlneExplainQuerySet( qllOptimizationNode   * aOptNode,
                               qllDataArea           * aDataArea,
                               knlRegionMem          * aRegionMem,
                               stlUInt32               aDepth,
                               qllExplainNodeText   ** aPlanText,
                               qllEnv                * aEnv );

stlStatus qlneExplainSetOP( qllOptimizationNode   * aOptNode,
                            qllDataArea           * aDataArea,
                            knlRegionMem          * aRegionMem,
                            stlUInt32               aDepth,
                            qllExplainNodeText   ** aPlanText,
                            qllEnv                * aEnv );

stlStatus qlneExplainSubQuery( qllOptimizationNode   * aOptNode,
                               qllDataArea           * aDataArea,
                               knlRegionMem          * aRegionMem,
                               stlUInt32               aDepth,
                               qllExplainNodeText   ** aPlanText,
                               qllEnv                * aEnv );

stlStatus qlneExplainSubQueryList( qllOptimizationNode   * aOptNode,
                                   qllDataArea           * aDataArea,
                                   knlRegionMem          * aRegionMem,
                                   stlUInt32               aDepth,
                                   qllExplainNodeText   ** aPlanText,
                                   qllEnv                * aEnv );

stlStatus qlneExplainSubQueryFunc( qllOptimizationNode   * aOptNode,
                                   qllDataArea           * aDataArea,
                                   knlRegionMem          * aRegionMem,
                                   stlUInt32               aDepth,
                                   qllExplainNodeText   ** aPlanText,
                                   qllEnv                * aEnv );

stlStatus qlneExplainSubQueryFilter( qllOptimizationNode   * aOptNode,
                                     qllDataArea           * aDataArea,
                                     knlRegionMem          * aRegionMem,
                                     stlUInt32               aDepth,
                                     qllExplainNodeText   ** aPlanText,
                                     qllEnv                * aEnv );

stlStatus qlneExplainConcat( qllOptimizationNode   * aOptNode,
                             qllDataArea           * aDataArea,
                             knlRegionMem          * aRegionMem,
                             stlUInt32               aDepth,
                             qllExplainNodeText   ** aPlanText,
                             qllEnv                * aEnv );

stlStatus qlneExplainNestedLoopsJoin( qllOptimizationNode   * aOptNode,
                                      qllDataArea           * aDataArea,
                                      knlRegionMem          * aRegionMem,
                                      stlUInt32               aDepth,
                                      qllExplainNodeText   ** aPlanText,
                                      qllEnv                * aEnv );

stlStatus qlneExplainSortMergeJoin( qllOptimizationNode   * aOptNode,
                                    qllDataArea           * aDataArea,
                                    knlRegionMem          * aRegionMem,
                                    stlUInt32               aDepth,
                                    qllExplainNodeText   ** aPlanText,
                                    qllEnv                * aEnv );

stlStatus qlneExplainHashJoin( qllOptimizationNode   * aOptNode,
                               qllDataArea           * aDataArea,
                               knlRegionMem          * aRegionMem,
                               stlUInt32               aDepth,
                               qllExplainNodeText   ** aPlanText,
                               qllEnv                * aEnv );

stlStatus qlneExplainOrderBy( qllOptimizationNode   * aOptNode,
                              qllDataArea           * aDataArea,
                              knlRegionMem          * aRegionMem,
                              stlUInt32               aDepth,
                              qllExplainNodeText   ** aPlanText,
                              qllEnv                * aEnv );

stlStatus qlneExplainTableAccess( qllOptimizationNode   * aOptNode,
                                  qllDataArea           * aDataArea,
                                  knlRegionMem          * aRegionMem,
                                  stlUInt32               aDepth,
                                  qllExplainNodeText   ** aPlanText,
                                  qllEnv                * aEnv );

stlStatus qlneExplainIndexAccess( qllOptimizationNode   * aOptNode,
                                  qllDataArea           * aDataArea,
                                  knlRegionMem          * aRegionMem,
                                  stlUInt32               aDepth,
                                  qllExplainNodeText   ** aPlanText,
                                  qllEnv                * aEnv );

stlStatus qlneExplainRowIdAccess( qllOptimizationNode   * aOptNode,
                                  qllDataArea           * aDataArea,
                                  knlRegionMem          * aRegionMem,
                                  stlUInt32               aDepth,
                                  qllExplainNodeText   ** aPlanText,
                                  qllEnv                * aEnv );

#if 0
stlStatus qlneExplainFastDualAccess( qllOptimizationNode   * aOptNode,
                                     qllDataArea           * aDataArea,
                                     knlRegionMem          * aRegionMem,
                                     stlUInt32               aDepth,
                                     qllExplainNodeText   ** aPlanText,
                                     qllEnv                * aEnv );
#endif

#if 0
stlStatus qlneExplainFlatInstantAccess( qllOptimizationNode   * aOptNode,
                                        qllDataArea           * aDataArea,
                                        knlRegionMem          * aRegionMem,
                                        stlUInt32               aDepth,
                                        qllExplainNodeText   ** aPlanText,
                                        qllEnv                * aEnv );
#endif

stlStatus qlneExplainSortInstantAccess( qllOptimizationNode   * aOptNode,
                                        qllDataArea           * aDataArea,
                                        knlRegionMem          * aRegionMem,
                                        stlUInt32               aDepth,
                                        qllExplainNodeText   ** aPlanText,
                                        qllEnv                * aEnv );

#if 0
stlStatus qlneExplainHashInstantAccess( qllOptimizationNode   * aOptNode,
                                        qllDataArea           * aDataArea,
                                        knlRegionMem          * aRegionMem,
                                        stlUInt32               aDepth,
                                        qllExplainNodeText   ** aPlanText,
                                        qllEnv                * aEnv );
#endif

stlStatus qlneExplainGroupHashInstantAccess( qllOptimizationNode   * aOptNode,
                                             qllDataArea           * aDataArea,
                                             knlRegionMem          * aRegionMem,
                                             stlUInt32               aDepth,
                                             qllExplainNodeText   ** aPlanText,
                                             qllEnv                * aEnv );

#if 0
stlStatus qlneExplainGroupSortInstantAccess( qllOptimizationNode   * aOptNode,
                                             qllDataArea           * aDataArea,
                                             knlRegionMem          * aRegionMem,
                                             stlUInt32               aDepth,
                                             qllExplainNodeText   ** aPlanText,
                                             qllEnv                * aEnv );
#endif

stlStatus qlneExplainHashAggregation( qllOptimizationNode   * aOptNode,
                                      qllDataArea           * aDataArea,
                                      knlRegionMem          * aRegionMem,
                                      stlUInt32               aDepth,
                                      qllExplainNodeText   ** aPlanText,
                                      qllEnv                * aEnv );

stlStatus qlneExplainGroup( qllOptimizationNode   * aOptNode,
                            qllDataArea           * aDataArea,
                            knlRegionMem          * aRegionMem,
                            stlUInt32               aDepth,
                            qllExplainNodeText   ** aPlanText,
                            qllEnv                * aEnv );

stlStatus qlneExplainSortJoinInstantAccess( qllOptimizationNode   * aOptNode,
                                            qllDataArea           * aDataArea,
                                            knlRegionMem          * aRegionMem,
                                            stlUInt32               aDepth,
                                            qllExplainNodeText   ** aPlanText,
                                            qllEnv                * aEnv );

stlStatus qlneExplainHashJoinInstantAccess( qllOptimizationNode   * aOptNode,
                                            qllDataArea           * aDataArea,
                                            knlRegionMem          * aRegionMem,
                                            stlUInt32               aDepth,
                                            qllExplainNodeText   ** aPlanText,
                                            qllEnv                * aEnv );

/** @} qlne */

#endif /* _QLNENODE_H_ */
