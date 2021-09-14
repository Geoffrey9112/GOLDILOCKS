/*******************************************************************************
 * qloUpdateReturnQuery.c
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
 * @file qloUpdateReturnQuery.c
 * @brief SQL Processor Layer : Optimization of UPDATE RETURNING Query statement
 */

#include <qll.h>
#include <qlDef.h>


/*******************************************************
 * CODE AREA
 *******************************************************/

/**
 * @addtogroup qlocUpdateReturnQuery
 * @{
 */


/**
 * @brief UPDATE RETURNING Query 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement 객체
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qloCodeOptimizeUpdateReturnQuery( smlTransId     aTransID,
                                           qllDBCStmt   * aDBCStmt,
                                           qllStatement * aSQLStmt,
                                           qllEnv       * aEnv )
{
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_UPDATE_RETURNING_QUERY_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mInitPlan != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 UPDATE 구문 Code Optimization
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



/** @} qlocUpdateReturnQuery */




/*******************************************************
 * DATA AREA
 *******************************************************/

/**
 * @addtogroup qlodUpdateReturnQuery
 * @{
 */


/**
 * @brief UPDATE RETURNING Query 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement 객체
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qloDataOptimizeUpdateReturnQuery( smlTransId     aTransID,
                                            qllDBCStmt   * aDBCStmt,
                                            qllStatement * aSQLStmt,
                                            qllEnv       * aEnv )
{
    qllDataArea          * sDataArea     = NULL;

    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_UPDATE_RETURNING_QUERY_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mCodePlan != NULL, QLL_ERROR_STACK(aEnv) );

    /*****************************************************************
     * 기본 UPDATE 구문 Data Optimization
     *****************************************************************/

    /**
     * 기본 UPDATE 구문 Data Optimization
     */

    STL_TRY( qloDataOptimizeUpdate( aTransID,
                                    aDBCStmt,
                                    aSQLStmt,
                                    aEnv )
             == STL_SUCCESS );

    /****************************************************************
     * Result Set 의 Cursor 정보 설정
     ****************************************************************/

    sDataArea = (qllDataArea *) aSQLStmt->mDataPlan;


    /**
     * Result Set Descriptor 의 정보 초기화
     */

    STL_TRY( qlcrDataOptInitResultSetDesc( aSQLStmt,
                                           sDataArea->mTargetBlock,
                                           & QLL_DATA_PLAN( aDBCStmt ),
                                           aEnv )
             == STL_SUCCESS );

    /**
     * For Sensitivity
     */

    qlcrDataOptSetInsensitiveResultSet( aSQLStmt->mResultSetDesc );
    
    /**
     * For Updatabliity
     */
    
    qlcrDataOptSetReadOnlyResultSet( aSQLStmt->mResultSetDesc );

    /****************************************************************
     * Result Set Materialization 정보 설정 
     ****************************************************************/
    
    /**
     * Materialized Result Set 정보를 설정
     */
    
    STL_TRY( qlcrDataOptSetMaterializedResultSet( aDBCStmt,
                                                  aSQLStmt,
                                                  aSQLStmt->mResultSetDesc,
                                                  aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} qlodUpdateReturnQuery */


