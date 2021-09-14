/*******************************************************************************
 * qlvDeleteReturnInto.c
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
 * @file qlvDeleteReturnInto.c
 * @brief SQL Processor Layer : Validation of DELELE RETURNING INTO statement
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlvDeleteReturnInto
 * @{
 */


/**
 * @brief DELETE RETURNING INTO 구문을 Validation 한다.
 * @param[in]      aTransID      Transaction ID
 * @param[in]      aDBCStmt      DBC Statement
 * @param[in,out]  aSQLStmt      SQL Statement
 * @param[in]      aSQLString    SQL String 
 * @param[in]      aParseTree    Parse Tree
 * @param[in]      aEnv          Environment
 */
stlStatus qlvValidateDeleteReturnInto( smlTransId      aTransID,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       stlChar       * aSQLString,
                                       qllNode       * aParseTree,
                                       qllEnv        * aEnv )
{
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_DELETE_RETURNING_INTO_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_DELETE_RETURNING_INTO_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    STL_TRY( qlvValidateDelete( aTransID,
                                aDBCStmt,
                                aSQLStmt,
                                aSQLString,
                                aParseTree,
                                aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} qlvDeleteReturnInto */


