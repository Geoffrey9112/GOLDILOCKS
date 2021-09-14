/*******************************************************************************
 * qlvUpdateReturnQuery.c
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
 * @file qlvUpdateReturnQuery.c
 * @brief SQL Processor Layer : Validation of UPDATE RETURNING Query statement
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlvUpdateReturnQuery
 * @{
 */


/**
 * @brief UPDATE RETURNING Query 구문을 Validation 한다.
 * @param[in]      aTransID      Transaction ID
 * @param[in]      aDBCStmt      DBC Statement
 * @param[in,out]  aSQLStmt      SQL Statement
 * @param[in]      aSQLString    SQL String 
 * @param[in]      aParseTree    Parse Tree
 * @param[in]      aEnv          Environment
 */
stlStatus qlvValidateUpdateReturnQuery( smlTransId      aTransID,
                                        qllDBCStmt    * aDBCStmt,
                                        qllStatement  * aSQLStmt,
                                        stlChar       * aSQLString,
                                        qllNode       * aParseTree,
                                        qllEnv        * aEnv )
{
    qlvStmtInfo     sStmtInfo;
    stlInt32        sQBIndex;

    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_UPDATE_RETURNING_QUERY_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_UPDATE_RETURNING_QUERY_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 UPDATE 구문 Validation
     */
    
    STL_TRY( qlvValidateUpdate( aTransID,
                                aDBCStmt,
                                aSQLStmt,
                                aSQLString,
                                aParseTree,
                                aEnv )
             == STL_SUCCESS );

    /**********************************************************
     * Result Set Property 설정
     **********************************************************/

    /**
     * statement information 설정
     */

    sQBIndex = 0;

    sStmtInfo.mTransID       = aTransID;
    sStmtInfo.mHasHintError  = STL_FALSE;
    sStmtInfo.mQBIndex       = &sQBIndex;
    stlStrcpy( sStmtInfo.mQBPrefix, QLV_QUERY_BLOCK_NAME_PREFIX_UPD );
    sStmtInfo.mDBCStmt       = aDBCStmt;
    sStmtInfo.mSQLStmt       = aSQLStmt;
    sStmtInfo.mSQLString     = aSQLString;
    sStmtInfo.mQueryExprList = NULL;

    /**
     * UPDATE RETURNING Query 의 Result Set Property 결정
     */
    
    STL_TRY( qlcrValidateDMLQueryResultCursorProperty( & sStmtInfo, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} qlvUpdateReturnQuery */


