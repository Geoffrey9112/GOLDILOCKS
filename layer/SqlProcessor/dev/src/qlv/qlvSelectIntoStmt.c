/*******************************************************************************
 * qlvSelectIntoStmt.c
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
 * @file qlvSelectIntoStmt.c
 * @brief SQL Processor Layer : Validation of SELECT INTO statement
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlvSelectInto
 * @{
 */


/**
 * @brief SELECT INTO 구문을 Validation 한다.
 * @param[in]      aTransID      Transaction ID
 * @param[in]      aDBCStmt      DBC Statement
 * @param[in,out]  aSQLStmt      SQL Statement
 * @param[in]      aSQLString    SQL String 
 * @param[in]      aParseTree    Parse Tree
 * @param[in]      aEnv          Environment
 */
stlStatus qlvValidateSelectInto( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 stlChar       * aSQLString,
                                 qllNode       * aParseTree,
                                 qllEnv        * aEnv )
{
    qlpSelect      * sParseTree = NULL;
    qlvInitSelect  * sInitPlan  = NULL;
    qlvStmtInfo      sStmtInfo;
    qlvInitExprList  sQueryExprList;
    stlInt32         sQBIndex;
    
    
    /**********************************************************
     * Parameter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_SELECT_INTO_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_SELECT_INTO_FOR_UPDATE_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aParseTree->mType == QLL_STMT_SELECT_INTO_TYPE) ||
                        (aParseTree->mType == QLL_STMT_SELECT_INTO_FOR_UPDATE_TYPE),
                        QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * 준비 작업
     **********************************************************/

    /**
     * select parse tree 포인터 매핑
     */

    sParseTree  = (qlpSelect *) aParseTree;


    /**********************************************************
     * Validate SELECT INTO statement
     **********************************************************/

    /**
     * Validate SELECT statement
     */
    
    STL_TRY( qlvValidateSelect( aTransID,
                                aDBCStmt,
                                aSQLStmt,
                                aSQLString,
                                aParseTree,
                                aEnv )
             == STL_SUCCESS );


    /**********************************************************
     * Into Clause Validation
     **********************************************************/
    
    /**
     * Target 개수만큼 INTO 절의 Out Paramter 공간 확보
     */

    sInitPlan = (qlvInitSelect *) aSQLStmt->mInitPlan;
    STL_DASSERT( sParseTree->mInto != NULL );

    /* QLV_NODE_TYPE_QUERY_SET 을 하위로 하는 SELECT INTO 는 지원하지 않는다. */
    
    STL_DASSERT( sInitPlan->mQueryNode->mType == QLV_NODE_TYPE_QUERY_SPEC );

    sInitPlan->mIntoTargetCnt  =
        ((qlvInitQuerySpecNode*) sInitPlan->mQueryNode)->mTargetCount;

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlvInitExpression) * sInitPlan->mIntoTargetCnt,
                                (void **) & sInitPlan->mIntoTargetArray,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    stlMemset( sInitPlan->mIntoTargetArray,
               0x00,
               STL_SIZEOF(qlvInitExpression) * sInitPlan->mIntoTargetCnt );

        
    /**
     * 임시 Query 단위 Expression List 설정
     */

    sQueryExprList.mStmtExprList       = sInitPlan->mStmtExprList;
    sQueryExprList.mPseudoColExprList  = NULL;
    sQueryExprList.mAggrExprList       = NULL;
    sQueryExprList.mNestedAggrExprList = NULL;


    /**
     * statement information 설정
     */

    sQBIndex = 0;

    sStmtInfo.mTransID       = aTransID;
    sStmtInfo.mHasHintError  = sParseTree->mHasHintErr;
    sStmtInfo.mQBIndex       = &sQBIndex;
    stlStrcpy( sStmtInfo.mQBPrefix, QLV_QUERY_BLOCK_NAME_PREFIX_SEL );
    sStmtInfo.mDBCStmt       = aDBCStmt;
    sStmtInfo.mSQLStmt       = aSQLStmt;
    sStmtInfo.mSQLString     = aSQLString;
    sStmtInfo.mQueryExprList = & sQueryExprList;

    
    /**
     * INTO phrase validation
     */
    
    STL_TRY( qlvValidateIntoPhrase( & sStmtInfo,
                                    sInitPlan->mIntoTargetCnt,
                                    sParseTree->mInto,
                                    sInitPlan->mIntoTargetArray,
                                    aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}



/** @} qlvSelectInto */


