/*******************************************************************************
 * qloInsertReturnQuery.c
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
 * @file qloInsertReturnQuery.c
 * @brief SQL Processor Layer : Optimization of DELELE RETURN Query statement
 */

#include <qll.h>
#include <qlDef.h>


/*******************************************************
 * CODE AREA
 *******************************************************/

/**
 * @addtogroup qlocInsertReturnQuery
 * @{
 */


/**
 * @brief INSERT RETURN Query 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement 객체
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qloCodeOptimizeInsertReturnQuery( smlTransId     aTransID,
                                            qllDBCStmt   * aDBCStmt,
                                            qllStatement * aSQLStmt,
                                            qllEnv       * aEnv )
{
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_QUERY_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mInitPlan != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 INSERT RETURN INTO Code Optimization
     */
    
    STL_TRY( qloCodeOptimizeInsertReturnInto( aTransID,
                                              aDBCStmt,
                                              aSQLStmt,
                                              aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} qlocInsertReturnQuery */




/*******************************************************
 * DATA AREA
 *******************************************************/

/**
 * @addtogroup qlodInsertReturnQuery
 * @{
 */


/**
 * @brief INSERT RETURN Query 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement 객체
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qloDataOptimizeInsertReturnQuery( smlTransId     aTransID,
                                            qllDBCStmt   * aDBCStmt,
                                            qllStatement * aSQLStmt,
                                            qllEnv       * aEnv )
{
    qllDataArea             * sDataArea     = NULL;

    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_QUERY_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mCodePlan != NULL, QLL_ERROR_STACK(aEnv) );

    /*****************************************************************
     * 기본 INSERT Data Optimization
     *****************************************************************/

    /**
     * 기본 INSERT RETURN Data Optimization
     */
    

    STL_TRY( qloDataOptimizeInsertReturnInto( aTransID,
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



/** @} qlodInsertReturnQuery */


