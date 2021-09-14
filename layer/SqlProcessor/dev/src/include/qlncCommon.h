/*******************************************************************************
 * qlncCommon.h
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

#ifndef _QLNCCOMMON_H_
#define _QLNCCOMMON_H_ 1

/**
 * @file qlncCommon.h
 * @brief SQL Processor Layer Candidate Optimization Common
 */

#include <qlDef.h>

/**
 * @addtogroup qlnc
 * @{
 */


/*******************************************************************************
 * DEFINITIONS
 ******************************************************************************/

#define QLNC_PAGE_SIZE              (stlFloat64)8192.0  /**< 1 Page의 Size */
#define QLNC_DEFAULT_PAGE_COUNT     (stlFloat64)1.0     /**< Page Count를 측정할 수 없는 경우 기본값 */

/**
 * @brief Rowid Column Index Number
 */
#define QLNC_ROWID_COLUMN_IDX           (stlInt32)-1    /**< Rowid Column의 Column Position */

#define QLNC_ALLOC_AND_COPY_STRING( _aDstStr, _aSrcStr, _aMem, _aEnv )  \
{                                                                       \
    if( (_aSrcStr) == NULL )                                            \
    {                                                                   \
        (_aDstStr) = NULL;                                              \
    }                                                                   \
    else                                                                \
    {                                                                   \
        stlInt32 _sLen = stlStrlen( (_aSrcStr) );                       \
        STL_TRY( knlAllocRegionMem( (_aMem),                            \
                                    _sLen + 1,                          \
                                    (void**)&(_aDstStr),                \
                                    KNL_ENV(_aEnv) )                    \
                 == STL_SUCCESS );                                      \
        stlMemcpy( (_aDstStr), (_aSrcStr), _sLen );                     \
        (_aDstStr)[_sLen] = '\0';                                       \
    }                                                                   \
}

#define QLNC_LESS_THAN_DOUBLE( _aLeft, _aRight )                                \
    ( ( stlAbsDouble( (_aLeft) - (_aRight) ) >= (stlFloat64)0.00000001 ) &&     \
      ( (_aLeft) < (_aRight) ) )


#define QLNC_NODE_ARRAY_DEFAULT_COUNT   8   /**< Default Node Array Count */


/*******************************************************************************
 * STRUCTURES
 ******************************************************************************/

/**
 * @brief Common Parameter Information
 */
struct qlncParamInfo
{
    smlTransId        mTransID;
    qllDBCStmt      * mDBCStmt;
    qllStatement    * mSQLStmt;
    stlInt32        * mGlobalNodeID;
    stlInt32        * mGlobalInternalBindIdx;
};


/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

/* Common */
stlBool qlncIsUsableOrFilterOnTableNode( qlncNodeCommon * aTableNode,
                                         qlncOrFilter   * aOrFilter );

stlBool qlncIsUsableFilterOnTableNode( qlncNodeCommon    * aTableNode,
                                       qlncExprCommon    * aFilter );

stlBool qlncIsExistNode( qlncNodeCommon     * aSrcNode,
                         qlncNodeCommon     * aNode );

stlBool qlncIsExistCandNodeOnPlanNode( qlncPlanCommon   * aPlanNode,
                                       qlncNodeCommon   * aCandNode );

stlBool qlncIsExistOuterColumnOnExpr( qlncExprCommon    * aExpr,
                                      qlncNodeArray     * aNodeArray );

stlBool qlncIsExistOuterColumnOnCandNode( qlncNodeCommon    * aNode,
                                          qlncNodeArray     * aNodeArray );

stlBool qlncIsExistOuterColumnByRefNodeOnExpr( qlncExprCommon   * aExpr,
                                               qlncNodeArray    * aRefNodeArray );

stlBool qlncIsExistOuterColumnByRefNodeOnCandNode( qlncNodeCommon   * aNode,
                                                   qlncNodeArray    * aRefNodeArray );

stlStatus qlncMakeNodeArrayOnExpr( qlncNodeArray    * aNodeArray,
                                   qlncExprCommon   * aExpr,
                                   qllEnv           * aEnv );

stlStatus qlncMakeNodeArrayOnCurrNode( qlncNodeArray    * aNodeArray,
                                       qlncNodeCommon   * aNode,
                                       qllEnv           * aEnv );

#if 0
stlStatus qlncExtractNodeForSemiJoinFromExpr( qlncExprCommon    * aExpr,
                                              qlncNodeArray     * aOrgNodeArray,
                                              qlncNodeArray     * aExceptNodeArray,
                                              qlncNodeArray     * aSemiJoinNodeArray,
                                              qllEnv            * aEnv );
#endif


stlBool qlncIsUpdatableNode( qlncNodeCommon * aNode );

stlBool qlncHasAggregationOnExpr( qlncExprCommon    * aExpr,
                                  stlInt32            aSkipSubTableNodeID,
                                  stlBool             aCheckNested );

stlStatus qlncRemoveOuterTableNodeFromRefNodeList( qlncParamInfo    * aParamInfo,
                                                   qlncExprCommon   * aExpr,
                                                   qlncNodeCommon   * aInnerNode,
                                                   qlncExprCommon  ** aOutExpr,
                                                   qllEnv           * aEnv );

stlStatus qlncFindAddSubQueryExprToSubQueryExprList( qlncParamInfo      * aParamInfo,
                                                     qlncExprCommon     * aExpr,
                                                     qlncRefExprList    * aSubQueryExprList,
                                                     qllEnv             * aEnv );

/** @} qlnc */

#endif /* _QLNCCOMMON_H_ */
