/*******************************************************************************
 * qloSelectIntoStmt.c
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
 * @file qloSelect.c
 * @brief SQL Processor Layer : Optimization of SELECT INTO statement
 */

#include <qll.h>
#include <qlDef.h>


/*******************************************************
 * CODE AREA
 *******************************************************/

/**
 * @addtogroup qlocSelectInto
 * @{
 */


/**
 * @brief SELECT INTO 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement 객체
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qloCodeOptimizeSelectInto( smlTransId     aTransID,
                                     qllDBCStmt   * aDBCStmt,
                                     qllStatement * aSQLStmt,
                                     qllEnv       * aEnv )
{
    qlvInitSelect         * sInitPlan          = NULL;
    qllOptimizationNode   * sOptNode           = NULL;
    qlnoSelectStmt        * sOptSelectStmt     = NULL;
    knlValueBlockList     * sValueBlock        = NULL;
    knlBindType             sUserBindType      = KNL_BIND_TYPE_INVALID;
    stlInt32                sLoop              = 0;

    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_SELECT_INTO_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_SELECT_INTO_FOR_UPDATE_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mInitPlan != NULL, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Code Optimization : SELECT INTO statement 
     ****************************************************************/

    /**
     * Optimize SELECT optimizaion Node
     */

    aSQLStmt->mCodePlan = sOptNode;
    
    STL_TRY( qloCodeOptimizeSelect( aTransID,
                                    aDBCStmt,
                                    aSQLStmt,
                                    aEnv )
             == STL_SUCCESS );


    /**
     * Plan Cache로부터 Code Plan 획득한 경우는 Code Opt를 생략한다.
     */
    
    if( aSQLStmt->mSqlHandle != NULL )
    {
        STL_DASSERT( aSQLStmt->mInitPlan == NULL );
        STL_DASSERT( aSQLStmt->mCodePlan != NULL );

        STL_THROW( RAMP_FINISH );
    }
    else
    {
        /* Do Nothing */
    }

    
    /****************************************************************
     * Code Optimiztion : INTO clause 
     ****************************************************************/

    /**
     * SELECT STATEMENT Optimization Node 획득
     */

    sInitPlan      = (qlvInitSelect *) QLL_GET_CACHED_INIT_PLAN( aSQLStmt->mCodePlan );
    sOptNode       = (qllOptimizationNode *) aSQLStmt->mCodePlan;
    sOptSelectStmt = (qlnoSelectStmt *) sOptNode->mOptNode;
 

    /**
     * INTO clause target array
     */

    sOptSelectStmt->mIntoTargetArray = sInitPlan->mIntoTargetArray;

    for( sLoop = 0 ; sLoop < sOptSelectStmt->mTargetCnt ; sLoop++ )
    {
        STL_TRY( knlGetBindParamType( aSQLStmt->mBindContext,
                                      sOptSelectStmt->mIntoTargetArray[ sLoop ].mExpr.mBindParam->mBindParamIdx + 1,
                                      & sUserBindType,
                                      KNL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );
        
        STL_TRY_THROW( (sUserBindType == KNL_BIND_TYPE_OUT) || (sUserBindType == KNL_BIND_TYPE_INOUT),
                       RAMP_BINDTYPE_MISMATCH );

        STL_TRY( knlGetParamValueBlock(
                     aSQLStmt->mBindContext,
                     sOptSelectStmt->mIntoTargetArray[ sLoop ].mExpr.mBindParam->mBindParamIdx + 1,
                     NULL,
                     & sValueBlock,
                     KNL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );

        STL_TRY( qloCastExprDBType(
                     sOptSelectStmt->mTargets[ sLoop ].mExpr->mExpr.mInnerColumn->mOrgExpr,
                     sOptSelectStmt->mStmtExprList->mTotalExprInfo,
                     KNL_GET_BLOCK_DB_TYPE( sValueBlock ),
                     KNL_GET_BLOCK_ARG_NUM1( sValueBlock ),
                     KNL_GET_BLOCK_ARG_NUM2( sValueBlock ),
                     KNL_GET_BLOCK_STRING_LENGTH_UNIT( sValueBlock ),
                     KNL_GET_BLOCK_INTERVAL_INDICATOR( sValueBlock ),
                     STL_FALSE,
                     STL_FALSE,
                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( qloSetExprDBType( sOptSelectStmt->mTargets[ sLoop ].mExpr,
                                   sOptSelectStmt->mStmtExprList->mTotalExprInfo,
                                   KNL_GET_BLOCK_DB_TYPE( sValueBlock ),
                                   KNL_GET_BLOCK_ARG_NUM1( sValueBlock ),
                                   KNL_GET_BLOCK_ARG_NUM2( sValueBlock ),
                                   KNL_GET_BLOCK_STRING_LENGTH_UNIT( sValueBlock ),
                                   KNL_GET_BLOCK_INTERVAL_INDICATOR( sValueBlock ),
                                   aEnv )
                 == STL_SUCCESS );
        
    }


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_CATCH( RAMP_BINDTYPE_MISMATCH )
    {
        stlPushError(
            STL_ERROR_LEVEL_ABORT,
            QLL_ERRCODE_BIND_TYPE_MISMATCH,
            qlgMakeErrPosString( aSQLStmt->mRetrySQL,
                                 sOptSelectStmt->mIntoTargetArray[ sLoop ].mPosition,
                                 aEnv ),
            QLL_ERROR_STACK(aEnv),
            sOptSelectStmt->mIntoTargetArray[ sLoop ].mExpr.mBindParam->mBindParamIdx + 1 );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}


/** @} qlocSelectInto */




/*******************************************************
 * DATA AREA
 *******************************************************/

/**
 * @addtogroup qlodSelectInto
 * @{
 */


/**
 * @brief SELECT INTO 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement 객체
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qloDataOptimizeSelectInto( smlTransId     aTransID,
                                     qllDBCStmt   * aDBCStmt,
                                     qllStatement * aSQLStmt,
                                     qllEnv       * aEnv )
{
    qllOptimizationNode  * sOptNode       = NULL;
    qlnoSelectStmt       * sOptSelectStmt = NULL;
    qllDataArea          * sDataArea      = NULL;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_SELECT_INTO_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_SELECT_INTO_FOR_UPDATE_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mCodePlan != NULL, QLL_ERROR_STACK(aEnv) );

    
    /****************************************************************
     * Data Optimization : SELECT INTO statement
     ****************************************************************/

    /**
     * Data Optimize SELECT
     */
    
    STL_TRY( qloDataOptimizeSelect( aTransID,
                                    aDBCStmt,
                                    aSQLStmt,
                                    aEnv )
             == STL_SUCCESS );


    /****************************************************************
     * INTO 절 Data Optimization
     ****************************************************************/

    /**
     * SELECT STATEMENT Optimization Node 획득
     */

    sOptNode       = (qllOptimizationNode *) aSQLStmt->mCodePlan;
    sOptSelectStmt = (qlnoSelectStmt *) sOptNode->mOptNode;


    /**
     * Data Area 획득
     */

    sDataArea      = (qllDataArea *) aSQLStmt->mDataPlan;

    
    /**
     * INTO 절의 Code Optimization : CAST 정보 구축
     * - 별도의 최적화나 정보 구축은 없음.
     * - 단, Bind Type 과 Target Type 이 호환되는 지 검사함.
     */

    
    /* INTO절을 위한 Cast가 유효한지 검사 */
    STL_TRY( qlndDataOptIntoPhrase( aSQLStmt,
                                    sOptSelectStmt->mStmtExprList->mTotalExprInfo,
                                    aSQLStmt->mBindContext,
                                    sOptSelectStmt->mTargetCnt,
                                    sOptSelectStmt->mTargetColList,
                                    sOptSelectStmt->mIntoTargetArray,
                                    sDataArea,
                                    & QLL_DATA_PLAN( aDBCStmt ),
                                    aEnv )
             == STL_SUCCESS );
    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlodSelectIntoStmt */



