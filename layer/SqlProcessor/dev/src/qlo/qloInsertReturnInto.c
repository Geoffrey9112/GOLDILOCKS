/*******************************************************************************
 * qloInsertReturnInto.c
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
 * @file qloInsertReturnInto.c
 * @brief SQL Processor Layer : Optimization of INSERT RETURNING INTO statement
 */

#include <qll.h>
#include <qlDef.h>


/*******************************************************
 * CODE AREA
 *******************************************************/

/**
 * @addtogroup qlocInsertReturnInto
 * @{
 */


/**
 * @brief INSERT RETURNING INTO 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement 객체
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qloCodeOptimizeInsertReturnInto( smlTransId     aTransID,
                                           qllDBCStmt   * aDBCStmt,
                                           qllStatement * aSQLStmt,
                                           qllEnv       * aEnv )
{
    qlvInitInsertReturnInto  * sInitPlan = NULL;


    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_INTO_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_QUERY_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mInitPlan != NULL, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * 정보 획득
     ****************************************************************/

    /**
     * 기본 정보 획득
     */

    STL_TRY( qlxInsertReturnIntoGetValidPlan( aTransID,
                                              aDBCStmt,
                                              aSQLStmt,
                                              QLL_PHASE_CODE_OPTIMIZE,
                                              & sInitPlan,
                                              NULL,
                                              NULL,
                                              aEnv )
             == STL_SUCCESS );


    /***************************************************
     * Base INSERT 구문 Code Optimization
     ***************************************************/

    if( sInitPlan->mIsInsertValues == STL_TRUE )
    {
        STL_TRY( qloCodeOptimizeInsertValues( aTransID,
                                              aDBCStmt,
                                              aSQLStmt,
                                              aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( qloCodeOptimizeInsertSelect( aTransID,
                                              aDBCStmt,
                                              aSQLStmt,
                                              aEnv )
                 == STL_SUCCESS );
    }
 

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} qlocInsertReturnInto */




/*******************************************************
 * DATA AREA
 *******************************************************/

/**
 * @addtogroup qlodInsertReturnInto
 * @{
 */


/**
 * @brief INSERT RETURNING INTO 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement 객체
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qloDataOptimizeInsertReturnInto( smlTransId     aTransID,
                                           qllDBCStmt   * aDBCStmt,
                                           qllStatement * aSQLStmt,
                                           qllEnv       * aEnv )
{
    qlvInitInsertReturnInto  * sInitPlan = NULL;


    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_INTO_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_QUERY_TYPE),
                        QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * 정보 획득
     ****************************************************************/

    /**
     * 기본 정보 획득
     */

    STL_TRY( qlxInsertReturnIntoGetValidPlan( aTransID,
                                              aDBCStmt,
                                              aSQLStmt,
                                              QLL_PHASE_CODE_OPTIMIZE,
                                              & sInitPlan,
                                              NULL,
                                              NULL,
                                              aEnv )
             == STL_SUCCESS );


    /***************************************************
     * Base INSERT 구문 Code Optimization
     ***************************************************/

    if( sInitPlan->mIsInsertValues == STL_TRUE )
    {
        STL_TRY( qloDataOptimizeInsertValues( aTransID,
                                              aDBCStmt,
                                              aSQLStmt,
                                              aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( qloDataOptimizeInsertSelect( aTransID,
                                              aDBCStmt,
                                              aSQLStmt,
                                              aEnv )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} qlodInsertReturnInto */


