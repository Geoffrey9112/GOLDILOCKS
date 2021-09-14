/*******************************************************************************
 * qleExplainPlan.h
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

#ifndef _QLEEXPLAINPLAN_H_
#define _QLEEXPLAINPLAN_H_ 1

/**
 * @file qleExplainPlan.h
 * @brief SQL Processor Layer Explain Plan
 */

#include <qlDef.h>


/**
 * @addtogroup qleExplainPlan
 * @{
 */

stlStatus qleSetExplainSortExprList( knlRegionMem   * aRegionMem,
                                     qllExplainText * aExplainText,
                                     qlvRefExprList * aRefExprList,
                                     stlUInt8       * aKeyColFlags,
                                     stlBool          aIsVerbose,
                                     qllEnv         * aEnv );

stlStatus qleSetExplainExprList( knlRegionMem   * aRegionMem,
                                 qllExplainText * aExplainText,
                                 qlvRefExprList * aRefExprList,
                                 stlChar        * aSepString,
                                 stlBool          aIsVerbose,
                                 qllEnv         * aEnv );

stlStatus qleSetExplainExpr( knlRegionMem       * aRegionMem,
                             qllExplainText     * aExplainText,
                             qlvInitExpression  * aExpr,
                             stlBool              aCalledByFunc,
                             stlBool              aIsVerbose,
                             qllEnv             * aEnv );

stlStatus qleSetExplainFuncExpr( knlRegionMem       * aRegionMem,
                                 qllExplainText     * aExplainText,
                                 qlvInitExpression  * aFuncExpr,
                                 stlBool              aCalledByFunc,
                                 stlBool              aIsVerbose,
                                 qllEnv             * aEnv );

stlStatus qleSetExplainCaseExpr( knlRegionMem       * aRegionMem,
                                 qllExplainText     * aExplainText,
                                 qlvInitExpression  * aInitCaseExpr,
                                 stlBool              aIsVerbose,
                                 qllEnv             * aEnv );

stlStatus qleSetExplainListFuncExpr( knlRegionMem       * aRegionMem,
                                     qllExplainText     * aExplainText,
                                     qlvInitExpression  * aInitListFunc,
                                     stlBool              aCalledByFunc,
                                     stlBool              aIsVerbose,
                                     qllEnv             * aEnv );

stlStatus qleSetExplainRowExpr( knlRegionMem       * aRegionMem,
                                qllExplainText     * aExplainText,
                                qlvInitExpression  * aRowExpr,
                                stlBool              aIsVerbose,
                                qllEnv             * aEnv );

stlStatus qleSetExplainRange( knlRegionMem      * aRegionMem,
                              qllExplainText    * aExplainText,
                              qlvRefExprList   ** aRangeExprList,
                              stlInt32            aKeyColCnt,
                              stlBool             aIsVerbose,
                              qllEnv            * aEnv );

///////////////////////////////////////////////////////////////////////////////

stlStatus qleExplainPlan( smlTransId              aTransID,
                          qllDBCStmt            * aDBCStmt,
                          qllStatement          * aSQLStmt,
                          qllExplainNodeText   ** aPlanText,
                          qllEnv                * aEnv );

stlStatus qleTraceExplainPlan( smlTransId             aTransID,
                               qllDBCStmt           * aDBCStmt,
                               qllStatement         * aSQLStmt,
                               stlInt32               aSqlPlanLevel,
                               qllExplainNodeText  ** aPlanText,
                               qllEnv               * aEnv );

stlStatus qleCreateExplainNodeText( knlRegionMem        * aRegionMem,
                                    stlInt32              aDepth,
                                    qllExplainNodeText  * aParentNodeText,
                                    qllExplainNodeText ** aCurrentNodeText,
                                    qllEnv              * aEnv );

stlStatus qleCreateExplainPredText( knlRegionMem        * aRegionMem,
                                    stlInt32              aDepth,
                                    qllExplainNodeText  * aNodeText,
                                    qllExplainPredText ** aPredText,
                                    qllEnv              * aEnv );

stlStatus qleSetStringToExplainText( knlRegionMem   * aRegionMem,
                                     qllExplainText * aExplainText,
                                     qllEnv         * aEnv,
                                     const stlChar  * aFormat,
                                     ... );

stlStatus qleAppendStringToExplainText( knlRegionMem    * aRegionMem,
                                        qllExplainText  * aExplainText,
                                        qllEnv          * aEnv,
                                        const stlChar   * aFormat,
                                        ... );

/** @} qleExplainPlan */

#endif /* _QLEEXPLAINPLAN_H_ */

