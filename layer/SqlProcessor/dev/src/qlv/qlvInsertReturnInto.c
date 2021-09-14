/*******************************************************************************
 * qlvInsertReturnInto.c
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
 * @file qlvInsertReturnInto.c
 * @brief SQL Processor Layer : Validation of INSERT RETURNING INTO statement
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlvInsertReturnInto
 * @{
 */


/**
 * @brief INSERT RETURNING INTO 구문을 Validation 한다.
 * @param[in]      aTransID      Transaction ID
 * @param[in]      aDBCStmt      DBC Statement
 * @param[in,out]  aSQLStmt      SQL Statement
 * @param[in]      aSQLString    SQL String 
 * @param[in]      aParseTree    Parse Tree
 * @param[in]      aEnv          Environment
 */
stlStatus qlvValidateInsertReturnInto( smlTransId      aTransID,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       stlChar       * aSQLString,
                                       qllNode       * aParseTree,
                                       qllEnv        * aEnv )
{
    qlpInsert               * sParseTree = NULL;
    qlvInitInsertReturnInto * sInitPlan  = NULL;


    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_INTO_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_QUERY_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aParseTree->mType == QLL_STMT_INSERT_RETURNING_INTO_TYPE) ||
                        (aParseTree->mType == QLL_STMT_INSERT_RETURNING_QUERY_TYPE),
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */
    
    sParseTree  = (qlpInsert *) aParseTree;


    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlvInitInsertReturnInto),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlvInitInsertReturnInto) );

    /**
     * Init Plan 정보를 구문 유형에 따라 Validation 후 복사한다.
     */
    
    /**********************************************************
     * BASE insert statement validation
     **********************************************************/

    if ( sParseTree->mBaseInsertType == QLL_STMT_INSERT_TYPE )
    {
        /**
         * validation: INSERT VALUES statement
         */

        STL_TRY( qlvValidateInsertValues( aTransID,
                                          aDBCStmt,
                                          aSQLStmt,
                                          aSQLString,
                                          aParseTree,
                                          aEnv )
                 == STL_SUCCESS );

        sInitPlan->mIsInsertValues   = STL_TRUE;
        sInitPlan->mInitInsertValues = (qlvInitInsertValues *) aSQLStmt->mInitPlan;
        sInitPlan->mInitInsertSelect = NULL;
        sInitPlan->mTableHandle      = &sInitPlan->mInitInsertValues->mTableHandle;

        if( sParseTree->mIntoTarget != NULL )
        {
            /**
             * INSERT VALUES 구문은 single-row 인지 validation 단계에서 판단할 수 있다.
             */
            
            STL_TRY_THROW( sInitPlan->mInitInsertValues->mMultiValueCnt == 1,
                           RAMP_ERR_SINGLE_ROW_EXPRESSION );
        }
        else
        {
            /**
             * INSERT RETURNING Query 임
             */
        }
    }
    else
    {
        /**
         * validation: INSERT SELECT statement
         */

        STL_TRY( qlvValidateInsertSelect( aTransID,
                                          aDBCStmt,
                                          aSQLStmt,
                                          aSQLString,
                                          aParseTree,
                                          aEnv )
                 == STL_SUCCESS );

        sInitPlan->mIsInsertValues   = STL_FALSE;
        sInitPlan->mInitInsertValues = NULL; 
        sInitPlan->mInitInsertSelect = (qlvInitInsertSelect *) aSQLStmt->mInitPlan;
        sInitPlan->mTableHandle      = &sInitPlan->mInitInsertSelect->mTableHandle;
        
        /**
         * INSERT SELECT 구문은 single-row 인지 validation 단계에서 판단할 수 없다.
         */
    }

    /**
     * 공통 Init Plan 정보 설정
     * - 하위 구문 Validation 후 정보를 설정한다.
     */

    if ( sInitPlan->mIsInsertValues == STL_TRUE )
    {
        /**
         * INSERT VALUES 인 경우
         */

        qlvSetInitPlanByInitPlan( & sInitPlan->mInitPlan,
                                  & sInitPlan->mInitInsertValues->mInitPlan );
    }
    else
    {
        /**
         * INSERT SELECT 인 경우
         */

        qlvSetInitPlanByInitPlan( & sInitPlan->mInitPlan,
                                  & sInitPlan->mInitInsertSelect->mInitPlan );
    }
    
    /***************************************************
     * Init Plan 연결
     ***************************************************/
    
    aSQLStmt->mInitPlan = (void *) sInitPlan;

    
    /**
     * Target Info 설정
     */

    if( aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_QUERY_TYPE )
    {
        STL_TRY( qlfMakeQueryTarget( aSQLStmt,
                                     QLL_INIT_PLAN( aDBCStmt ),
                                     & sInitPlan->mTargetInfo,
                                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sInitPlan->mTargetInfo = NULL;
    }

    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SINGLE_ROW_EXPRESSION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INTO_CLAUSE_MULTIPLE_ROWS,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}



/** @} qlvInsertReturnInto */


