/*******************************************************************************
 * qlxUpdatePositioned.c
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
 * @file qlxUpdatePositioned.c
 * @brief SQL Processor Layer : Execution of UPDATE CURRENT OF statement
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlxUpdatePositioned
 * @{
 */


/**
 * @brief UPDATE CURRENT OF 구문을 실행한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement 객체
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlxExecuteUpdatePositioned( smlTransId      aTransID,
                                      smlStatement  * aStmt,
                                      qllDBCStmt    * aDBCStmt,
                                      qllStatement  * aSQLStmt,
                                      qllEnv        * aEnv )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_UPDATE_WHERE_CURRENT_OF_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mDataPlan != NULL, QLL_ERROR_STACK(aEnv) );

    /*****************************************************************
     * 기본 UPDATE 구문 수행 
     *****************************************************************/

    STL_TRY( qlxExecuteUpdate( aTransID,
                               aStmt,
                               aDBCStmt,
                               aSQLStmt,
                               aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} qlxUpdatePositioned */


