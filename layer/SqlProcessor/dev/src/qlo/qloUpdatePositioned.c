/*******************************************************************************
 * qloUpdatePositioned.c
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
 * @file qloUpdatePositioned.c
 * @brief SQL Processor Layer : Optimization of UPDATE CURRENT OF statement
 */

#include <qll.h>
#include <qlDef.h>


/*******************************************************
 * CODE AREA
 *******************************************************/

/**
 * @addtogroup qlocUpdatePositioned
 * @{
 */


/**
 * @brief UPDATE CURRENT OF 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement 객체
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qloCodeOptimizeUpdatePositioned( smlTransId     aTransID,
                                           qllDBCStmt   * aDBCStmt,
                                           qllStatement * aSQLStmt,
                                           qllEnv       * aEnv )
{
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_UPDATE_WHERE_CURRENT_OF_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mInitPlan != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 UPDATE Code Optimization
     */
    
    STL_TRY( qloCodeOptimizeUpdate( aTransID,
                                    aDBCStmt,
                                    aSQLStmt,
                                    aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} qlocUpdatePositioned */




/*******************************************************
 * DATA AREA
 *******************************************************/

/**
 * @addtogroup qlodUpdatePositioned
 * @{
 */


/**
 * @brief UPDATE CURRENT OF 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement 객체
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qloDataOptimizeUpdatePositioned( smlTransId     aTransID,
                                           qllDBCStmt   * aDBCStmt,
                                           qllStatement * aSQLStmt,
                                           qllEnv       * aEnv )
{
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_UPDATE_WHERE_CURRENT_OF_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mCodePlan != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 UPDATE Data Optimization
     */
    
    STL_TRY( qloDataOptimizeUpdate( aTransID,
                                    aDBCStmt,
                                    aSQLStmt,
                                    aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} qlodUpdatePositioned */


