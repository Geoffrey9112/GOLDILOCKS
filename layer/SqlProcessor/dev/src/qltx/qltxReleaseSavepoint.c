/*******************************************************************************
 * qltxReleaseSavepoint.c
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
 * @file qltxReleaseSavepoint.c
 * @brief RELEASE SAVEPOINT 처리 루틴 
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @defgroup qltxReleaseSavepoint RELEASE SAVEPOINT
 * @ingroup qltx
 * @{
 */


/**
 * @brief RELEASE SAVEPOINT 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltxValidateReleaseSavepoint( smlTransId      aTransID,
                                        qllDBCStmt    * aDBCStmt,
                                        qllStatement  * aSQLStmt,
                                        stlChar       * aSQLString,
                                        qllNode       * aParseTree,
                                        qllEnv        * aEnv )
{
    qltxInitReleaseSavepoint * sInitPlan = NULL;
    qlpSavepoint             * sParseTree = NULL;
    stlInt64                   sSavepointTimestamp = 0;

    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_RELEASE_SAVEPOINT_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_RELEASE_SAVEPOINT_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */
    
    sParseTree = (qlpSavepoint *) aParseTree;
    
    STL_PARAM_VALIDATE( sParseTree->mName != NULL, QLL_ERROR_STACK(aEnv) );
    
    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qltxInitReleaseSavepoint),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qltxInitReleaseSavepoint) );

    /**
     * Savepoint 유효성 검사
     */

    STL_TRY_THROW( aTransID != SML_INVALID_TRANSID, RAMP_ERR_INVALID_SAVEPOINT );
    
    STL_TRY_THROW( smlExistSavepoint( aTransID,
                                      sParseTree->mName,
                                      & sSavepointTimestamp,
                                      SML_ENV( aEnv ) )
                   == STL_TRUE, RAMP_ERR_INVALID_SAVEPOINT );
        
    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                stlStrlen( sParseTree->mName ) + 1,
                                (void **) & sInitPlan->mSavepoint,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
        
    stlStrcpy( sInitPlan->mSavepoint, sParseTree->mName );

    /**
     * 권한 검사 없음
     */
    
    /**
     * Init Plan 연결 
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_SAVEPOINT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SAVEPOINT_EXCEPTION,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mNamePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sParseTree->mName );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief RELEASE SAVEPOINT 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltxOptCodeReleaseSavepoint( smlTransId      aTransID,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv )
{
    return STL_SUCCESS;
}


/**
 * @brief RELEASE SAVEPOINT 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltxOptDataReleaseSavepoint( smlTransId      aTransID,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv )
{
    /**
     * nothing to do
     */
    
    return STL_SUCCESS;
}




/**
 * @brief RELEASE SAVEPOINT 를 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qltxExecuteReleaseSavepoint( smlTransId      aTransID,
                                       smlStatement  * aStmt,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv )
{
    qltxInitReleaseSavepoint * sInitPlan = NULL;

    /**
     * Init Plan 획득 
     */

    sInitPlan = (qltxInitReleaseSavepoint *) aSQLStmt->mInitPlan;

    /**
     * ReleaseSavepoint 수행
     */
    
    STL_TRY( smlReleaseSavepoint( aTransID,
                                  sInitPlan->mSavepoint,
                                  SML_ENV( aEnv ) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} qltxReleaseSavepoint */
