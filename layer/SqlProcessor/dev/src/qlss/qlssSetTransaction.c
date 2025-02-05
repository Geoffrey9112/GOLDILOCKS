/*******************************************************************************
 * qlssSetTransaction.c
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
 * @file qlssSetTransaction.c
 * @brief SET TRANSACTION 처리 루틴 
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @defgroup qlssSessionSetTransaction SET TRANSACTION
 * @ingroup qlssSession
 * @{
 */

/**
 * @brief SET TRANSACTION 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssValidateSetTransaction( smlTransId      aTransID,
                                      qllDBCStmt    * aDBCStmt,
                                      qllStatement  * aSQLStmt,
                                      stlChar       * aSQLString,
                                      qllNode       * aParseTree,
                                      qllEnv        * aEnv )
{
    qlssInitSetTransactionMode * sInitPlan   = NULL;
    qlpSetTransaction          * sParseTree  = NULL;

    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_SET_TRANSACTION_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_SET_TRANSACTION_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */
    
    sParseTree = (qlpSetTransaction *) aParseTree;

    STL_PARAM_VALIDATE( ( sParseTree->mAttr >= QLP_TRANSACTION_ATTR_ACCESS ) &&
                        ( sParseTree->mAttr < QLP_TRANSACTION_ATTR_MAX ),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sParseTree->mValue != NULL,
                        QLL_ERROR_STACK(aEnv) );
    
    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlssInitSetTransactionMode),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlssInitSetTransactionMode) );


    /**
     * Init Plan 구성
     */

    sInitPlan->mMode = sParseTree->mAttr;

    switch( sInitPlan->mMode )
    {
        case QLP_TRANSACTION_ATTR_ACCESS :
            sInitPlan->mAccessMode = QLP_GET_INT_VALUE( sParseTree->mValue );
            break;
        case QLP_TRANSACTION_ATTR_ISOLATION_LEVEL :
            sInitPlan->mIsolationLevel = QLP_GET_INT_VALUE( sParseTree->mValue );
            break;
        case QLP_TRANSACTION_ATTR_UNIQUE_INTEGRITY:
            sInitPlan->mUniqueMode = QLP_GET_INT_VALUE( sParseTree->mValue );
            break;
        default:
            STL_DASSERT(0);
            break;
    }

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
 * @brief SET TRANSACTION 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssOptCodeSetTransaction( smlTransId      aTransID,
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
 * @brief SET TRANSACTION 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssOptDataSetTransaction( smlTransId      aTransID,
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
 * @brief SET TRANSACTION 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qlssExecuteSetTransaction( smlTransId      aTransID,
                                     smlStatement  * aStmt,
                                     qllDBCStmt    * aDBCStmt,
                                     qllStatement  * aSQLStmt,
                                     qllEnv        * aEnv )
{
    qlssInitSetTransactionMode * sInitPlan = NULL;

    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    
    /**
     * Init Plan 획득 
     */

    sInitPlan = (qlssInitSetTransactionMode *) aSQLStmt->mInitPlan;

    switch( sInitPlan->mMode )
    {
        case QLP_TRANSACTION_ATTR_ACCESS :
            aSQLStmt->mDCLContext.mType       = QLL_DCL_TYPE_SET_TRANSACTION_ACCESS_MODE;
            aSQLStmt->mDCLContext.mAccessMode = sInitPlan->mAccessMode;
            break;
        case QLP_TRANSACTION_ATTR_ISOLATION_LEVEL :
            aSQLStmt->mDCLContext.mType           = QLL_DCL_TYPE_SET_TRANSACTION_ISOLATION_LEVEL;
            aSQLStmt->mDCLContext.mIsolationLevel = sInitPlan->mIsolationLevel;
            break;
        case QLP_TRANSACTION_ATTR_UNIQUE_INTEGRITY:
            aSQLStmt->mDCLContext.mType       = QLL_DCL_TYPE_SET_TRANSACTION_UNIQUE_INTEGRITY;
            aSQLStmt->mDCLContext.mUniqueMode = sInitPlan->mUniqueMode;
            break;
        default:
            STL_DASSERT(0);
            break;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlssSessionSetTransaction */
