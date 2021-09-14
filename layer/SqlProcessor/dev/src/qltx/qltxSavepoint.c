/*******************************************************************************
 * qltxSavepoint.c
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
 * @file qltxSavepoint.c
 * @brief SAVEPOINT 처리 루틴 
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @defgroup qltxSavepoint SAVEPOINT
 * @ingroup qltx
 * @{
 */


/**
 * @brief SAVEPOINT 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltxValidateSavepoint( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 stlChar       * aSQLString,
                                 qllNode       * aParseTree,
                                 qllEnv        * aEnv )
{
    qltxInitSavepoint * sInitPlan = NULL;
    qlpSavepoint      * sParseTree = NULL;

    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_SAVEPOINT_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_SAVEPOINT_TYPE,
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
                                STL_SIZEOF(qltxInitSavepoint),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qltxInitSavepoint) );

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

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief SAVEPOINT 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltxOptCodeSavepoint( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv )
{
    return STL_SUCCESS;
}


/**
 * @brief SAVEPOINT 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltxOptDataSavepoint( smlTransId      aTransID,
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
 * @brief SAVEPOINT 를 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qltxExecuteSavepoint( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv )
{
    qltxInitSavepoint * sInitPlan = NULL;

    stlInt32  sState = 0;
    stlInt64  sSaveTimestamp = 0;

    /**
     * Init Plan 획득 
     */

    sInitPlan = (qltxInitSavepoint *) aSQLStmt->mInitPlan;

    /**
     * Savepoint 수행
     */
    
    STL_TRY( smlSavepoint( aTransID,
                           sInitPlan->mSavepoint,
                           SML_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 1;

    /**
     * Deferred Constraint 정보에 추가
     */

    if ( smlExistSavepoint( aTransID,
                            sInitPlan->mSavepoint,
                            & sSaveTimestamp,
                            SML_ENV(aEnv) )
         == STL_TRUE )
    {
        /* ok */
    }
    else
    {
        STL_DASSERT(0);
    }
    
    STL_TRY ( qlrcAddSavepointIntoDeferConstraint( sSaveTimestamp, aEnv ) == STL_SUCCESS );
    sState = 2;

    return STL_SUCCESS;

    STL_FINISH;

    switch ( sState )
    {
        case 1:
            (void) smlReleaseSavepoint( aTransID, sInitPlan->mSavepoint, SML_ENV(aEnv) );
            break;
        default:
            break;
    }
    
    return STL_FAILURE;
}



/** @} qltxSavepoint */
