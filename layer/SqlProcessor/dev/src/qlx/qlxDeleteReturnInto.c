/*******************************************************************************
 * qlxDeleteReturnInto.c
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
 * @file qlxDeleteReturnInto.c
 * @brief SQL Processor Layer : Execution of DELETE RETURN INTO statement
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlxDeleteReturnInto
 * @{
 */


/**
 * @brief DELETE RETURN INTO 구문을 실행한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement 객체
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlxExecuteDeleteReturnInto( smlTransId      aTransID,
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
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_DELETE_RETURNING_INTO_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mDataPlan != NULL, QLL_ERROR_STACK(aEnv) );


    STL_TRY( qlxExecuteDelete( aTransID,
                               aStmt,
                               aDBCStmt,
                               aSQLStmt,
                               aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} qlxDeleteReturnInto */


