/*******************************************************************************
 * qlfSelect.c
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

/**
 * @file qlfSelect.c
 * @brief SQL Processor Layer Fetch SELECT Statement 
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlfSelect
 * @{
 */


/**
 * @brief SELECT 구문을 Fetch 한다.
 * @param[in]  aTransID   Transaction ID
 * @param[in]  aStmt      Statement
 * @param[in]  aDBCStmt   DBC Statement
 * @param[in]  aSQLStmt   SQL Statement 객체
 * @param[out] aEOF       End Of Fetch 여부
 * @param[in]  aEnv       Environment
 * @remarks
 */
stlStatus qlfFetchSelect( smlTransId      aTransID,
                          smlStatement  * aStmt,
                          qllDBCStmt    * aDBCStmt,
                          qllStatement  * aSQLStmt,
                          stlBool       * aEOF,
                          qllEnv        * aEnv )
{
    qllOptimizationNode  * sOptNode      = NULL;
    qllDataArea          * sDataArea     = NULL;
    stlInt64               sReadCnt      = 0;
    qlnxFetchNodeInfo      sFetchNodeInfo;

    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aSQLStmt->mStmtType == QLL_STMT_SELECT_TYPE ) ||
                        ( aSQLStmt->mStmtType == QLL_STMT_SELECT_FOR_UPDATE_TYPE ),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mDataPlan != NULL, QLL_ERROR_STACK(aEnv) );


    /******************************************************
     * 정보 획득 
     ******************************************************/
    
    /**
     * 검증된 Plan 정보 획득 
     */

    sOptNode  = aSQLStmt->mCodePlan;
    sDataArea = aSQLStmt->mDataPlan;

    
    /******************************************************
     * Fetch 수행
     ******************************************************/

    /**
     * Fetch 수행
     */
    
    /* fetch records*/
    QLNX_INIT_FETCH_NODE_INFO( & sFetchNodeInfo,
                               aTransID,
                               aStmt,
                               aDBCStmt,
                               aSQLStmt,
                               sDataArea );
        
    QLNX_FETCH_NODE( & sFetchNodeInfo,
                     QLL_GET_OPT_NODE_IDX( sOptNode ),
                     0, /* Start Idx */
                     0, /* Skip Count */
                     QLL_FETCH_COUNT_MAX, /* Fetch Count */
                     & sReadCnt,
                     aEOF,
                     aEnv );

    
    return STL_SUCCESS;

    STL_FINISH;
    
    /* Execution Node 종료 */
    QLNX_FINALIZE_NODE_IGNORE_FAILURE( sOptNode,
                                       sDataArea,
                                       aEnv );

    return STL_FAILURE;
}


/**
 * @brief Key Range 가 항상 FALSE 가 되는 지의 여부
 * @param[in] aMinRangeList  Min Range List
 * @param[in] aMaxRangeList  Max Range List
 * @return
 * - STL_TRUE : 항상 FALSE 가 되므로 결과가 존재할 수 없음 
 * - STL_FALSE: 결과가 존재할 수 있음.
 * @remarks
 */
stlBool qlfIsAlwaysFalseRange( knlCompareList * aMinRangeList,
                               knlCompareList * aMaxRangeList )
{
    knlCompareList * sRange = NULL;

    /**
     * @todo 항상 TRUE 또는 FALSE 값을 가지는 Range 인지 판단되어야 한다.
     *  <BR> 
     *  <BR> Bind Value에 의해 모든 Range Value가 정해진 경우,
     *  <BR> 각 key column에 대한 MIN / MAX 값의 비교로 
     *  <BR> Range의 적합성 여부를 판단하는 기능을 추가하여야 한다.
     */


    /**
     * Min Range 검사
     */
    
    for ( sRange = aMinRangeList; sRange != NULL;  sRange = sRange->mNext )
    {
        if ( ( sRange->mCompNullStop == STL_TRUE ) && ( sRange->mRangeLen == 0 ) )
        {
            /**
             * NULL 에 대한 Range 일 경우임.
             */
            return STL_TRUE;
        }
        else
        {
            /* nothing to do */
        }
    }

    /**
     * Max Range 검사
     */
    
    for ( sRange = aMaxRangeList; sRange != NULL;  sRange = sRange->mNext )
    {
        if ( ( sRange->mCompNullStop == STL_TRUE ) && ( sRange->mRangeLen == 0 ) )
        {
            /**
             * NULL 에 대한 Range 일 경우임.
             */
            return STL_TRUE;
        }
        else
        {
            /* nothing to do */
        }
    }

    return STL_FALSE;
}


/**
 * @brief Filter 가 항상 FALSE 가 되는 지의 여부
 * @param[in]      aFilterExpr   Filter Expression
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @return
 * - STL_TRUE : 항상 FALSE 가 되므로 결과가 존재할 수 없음 
 * - STL_FALSE: 결과가 존재할 수 있음.
 * @remarks
 *  - constant value 가 단독으로 오는 경우만 false 필터를 체크한다.
 */
stlBool qlfHasFalseFilter( qlvInitExpression  * aFilterExpr,
                           qllDataArea        * aDataArea )
{
    knlValueBlock  * sValueBlock = NULL;
    
    if( aFilterExpr == NULL )
    {
        return STL_FALSE;
    }
    else
    {
        if( aFilterExpr->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT )
        {
            sValueBlock = 
                aDataArea->mExprDataContext->mContexts[ aFilterExpr->mOffset ].mInfo.mValueInfo;

            STL_DASSERT( sValueBlock->mDataValue->mType == DTL_TYPE_BOOLEAN );

            return ( DTL_BOOLEAN_IS_FALSE( sValueBlock->mDataValue ) );
        }
        else
        {
            return STL_FALSE;
        }
    }
}


/** @} qlfSelect */
