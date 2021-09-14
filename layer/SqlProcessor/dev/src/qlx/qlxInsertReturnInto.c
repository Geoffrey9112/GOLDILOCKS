/*******************************************************************************
 * qlxInsertReturnInto.c
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
 * @file qlxInsertReturnInto.c
 * @brief SQL Processor Layer : Execution of INSERT RETURNING INTO statement
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlxInsertReturnInto
 * @{
 */


/**
 * @brief INSERT RETURNING INTO 구문을 실행한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement 객체
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlxExecuteInsertReturnInto( smlTransId      aTransID,
                                      smlStatement  * aStmt,
                                      qllDBCStmt    * aDBCStmt,
                                      qllStatement  * aSQLStmt,
                                      qllEnv        * aEnv )
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
        STL_TRY( qlxExecuteInsertValues( aTransID,
                                         aStmt,
                                         aDBCStmt,
                                         aSQLStmt,
                                         aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( qlxExecuteInsertSelect( aTransID,
                                         aStmt,
                                         aDBCStmt,
                                         aSQLStmt,
                                         aEnv )
                 == STL_SUCCESS );
    }
 

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief INSERT RETURNING INTO 수행을 위한 Lock 을 잡고 유효한 Plan 을 얻는다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aDBCStmt      DBC Statement
 * @param[in]  aSQLStmt      SQL Statement
 * @param[in]  aQueryPhase   Query Processing Phase
 * @param[out] aInitPlan     INSERT RETURN INTO Init Plan
 * @param[out] aCodePlan     INSERT RETURN INTO Code Plan
 * @param[out] aDataPlan     INSERT RETURN INTO Data Plan
 * @param[in]  aEnv          Environment
 */
stlStatus qlxInsertReturnIntoGetValidPlan( smlTransId                 aTransID,
                                           qllDBCStmt               * aDBCStmt,
                                           qllStatement             * aSQLStmt,
                                           qllQueryPhase              aQueryPhase,
                                           qlvInitInsertReturnInto ** aInitPlan,
                                           qllOptimizationNode     ** aCodePlan,
                                           qllDataArea             ** aDataPlan,
                                           qllEnv                   * aEnv )
{
    qlvInitInsertReturnInto * sInitPlan = NULL;
    qllOptimizationNode     * sCodePlan = NULL;
    qllDataArea             * sDataPlan = NULL;


    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_INTO_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_QUERY_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aQueryPhase > QLL_PHASE_VALIDATION) &&
                        (aQueryPhase <= QLL_PHASE_EXECUTE),
                        QLL_ERROR_STACK(aEnv) );
    

    /**
     * 기본 정보 설정
     */
    
    sInitPlan = (qlvInitInsertReturnInto *) qllGetInitPlan( aSQLStmt );
    sCodePlan = NULL;
    sDataPlan = NULL;


    /**
     * Output 설정
     */

    if ( aInitPlan != NULL )
    {
        *aInitPlan = sInitPlan;
    }
            
    if ( aCodePlan != NULL )
    {
        *aCodePlan = sCodePlan;
    }

    if ( aDataPlan != NULL )
    {
        *aDataPlan = sDataPlan;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlxInsertReturnInto */


