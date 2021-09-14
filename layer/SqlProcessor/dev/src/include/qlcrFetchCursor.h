/*******************************************************************************
 * qlcrFetchCursor.h
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

#ifndef _QLCR_FETCH_CURSOR_H_
#define _QLCR_FETCH_CURSOR_H_ 1

/**
 * @file qlcrFetchCursor.h
 * @brief FETCH CURSOR 처리 함수 
 */


/**
 * @defgroup qlcrFetchCursor FETCH cursor
 * @ingroup qlcrCursor
 * @{
 */

/**
 * @brief FETCH CURSOR init plan
 */
typedef struct qlcrInitFetchCursor
{
    qlvInitPlan           mInitPlan;
    
    /******************************
     * CURSOR 정보 
     ******************************/
    
    ellCursorInstDesc   * mInstDesc;

    /******************************
     * FETCH ORIENTATION
     ******************************/
    
    qllFetchOrientation   mFetchOrient;
    qlvInitExpression   * mFetchPosition;
    stlInt64              mPositionValue; 
    
    /******************************
     * INTO 절 
     ******************************/
    
    stlInt32              mIntoTargetCount;
    qlvInitExpression   * mIntoTargetArray;
    
} qlcrInitFetchCursor;


/**
 * @brief FETCH CURSOR data plan
 */
typedef struct qlcrDataFetchCursor
{
    smlScn         mCursorSCN;
    
    /******************************
     * INTO 절 
     ******************************/
    
    qloDataOutParam        * mIntoTargetArray;

    /******************************
     * Fetch Position Expression Cast 정보
     ******************************/
    
    knlCastFuncInfo        * mCastFuncInfo;

    /******************************
     * Execution Time 정보 
     ******************************/

    stlBool                  mHasData;
    stlInt32                 mValidBlockIdx;
    
} qlcrDataFetchCursor;


    
stlStatus qlcrValidateFetchCursor( smlTransId      aTransID,
                                   qllDBCStmt    * aDBCStmt,
                                   qllStatement  * aSQLStmt,
                                   stlChar       * aSQLString,
                                   qllNode       * aParseTree,
                                   qllEnv        * aEnv );

stlStatus qlcrCodeOptimizeFetchCursor( smlTransId      aTransID,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv );

stlStatus qlcrDataOptimizeFetchCursor( smlTransId      aTransID,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv );

stlStatus qlcrExecuteFuncFetchCursor( smlTransId      aTransID,
                                      smlStatement  * aStmt,
                                      qllDBCStmt    * aDBCStmt,
                                      qllStatement  * aSQLStmt,
                                      qllEnv        * aEnv );


/** @} qlcrFetchCursor */


#endif /* _QLCR_FETCH_CURSOR_H_ */
