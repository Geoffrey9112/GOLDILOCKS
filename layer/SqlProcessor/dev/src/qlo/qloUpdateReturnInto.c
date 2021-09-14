/*******************************************************************************
 * qloUpdateReturnInto.c
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
 * @file qloUpdateReturnInto.c
 * @brief SQL Processor Layer : Optimization of UPDATE RETURNING INTO statement
 */

#include <qll.h>
#include <qlDef.h>


/*******************************************************
 * CODE AREA
 *******************************************************/

/**
 * @addtogroup qlocUpdateReturnInto
 * @{
 */


/**
 * @brief UPDATE RETURNING INTO 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement 객체
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qloCodeOptimizeUpdateReturnInto( smlTransId     aTransID,
                                           qllDBCStmt   * aDBCStmt,
                                           qllStatement * aSQLStmt,
                                           qllEnv       * aEnv )
{
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_UPDATE_RETURNING_INTO_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mInitPlan != NULL, QLL_ERROR_STACK(aEnv) );


    STL_TRY( qloCodeOptimizeUpdate( aTransID,
                                    aDBCStmt,
                                    aSQLStmt,
                                    aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} qlocUpdateReturnInto */




/*******************************************************
 * DATA AREA
 *******************************************************/

/**
 * @addtogroup qlodUpdateReturnInto
 * @{
 */


/**
 * @brief UPDATE RETURNING INTO 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement 객체
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qloDataOptimizeUpdateReturnInto( smlTransId     aTransID,
                                            qllDBCStmt   * aDBCStmt,
                                            qllStatement * aSQLStmt,
                                            qllEnv       * aEnv )
{
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_UPDATE_RETURNING_INTO_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mCodePlan != NULL, QLL_ERROR_STACK(aEnv) );


    STL_TRY( qloDataOptimizeUpdate( aTransID,
                                    aDBCStmt,
                                    aSQLStmt,
                                    aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} qlodUpdateReturnInto */


