/*******************************************************************************
 * qlssAlterSystemAger.c
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
 * @file qlssAlterSystemAger.c
 * @brief ALTER SYSTEM AGER 처리 루틴 
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @defgroup qlssSystemAlterSystemAger ALTER SYSTEM AGER
 * @ingroup qlssSystem
 * @{
 */


/**
 * @brief ALTER SYSTEM AGER 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssValidateSystemAger( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  stlChar       * aSQLString,
                                  qllNode       * aParseTree,
                                  qllEnv        * aEnv )
{
    qlssInitSystemAger  * sInitPlan = NULL;
    qlpAlterSystemAger  * sParseTree = NULL;
    stlInt64              sDatabaseTestOption = 0;

    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_SYSTEM_AGER_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_SYSTEM_AGER_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    sDatabaseTestOption = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_TEST_OPTION,
                                                       KNL_ENV(aEnv) );

    STL_TRY_THROW( sDatabaseTestOption < 1, RAMP_ERR_DISALLOW_STATEMENT );
    
    /**
     * 기본 정보 획득 
     */
    
    sParseTree = (qlpAlterSystemAger *) aParseTree;

    
    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlssInitSystemAger),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlssInitSystemAger) );
    sInitPlan->mBehavior = sParseTree->mBehavior;

    /**
     * Property 유효성 검사
     */

    /**
     * ALTER SYSTEM ON DATABASE 권한 검사
     */

    STL_TRY( qlaCheckDatabasePriv( aTransID,
                                   aDBCStmt,
                                   aSQLStmt,
                                   ELL_DB_PRIV_ACTION_ALTER_SYSTEM,
                                   aEnv )
             == STL_SUCCESS );
    
    /**
     * Init Plan 연결 
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;
        
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_DISALLOW_STATEMENT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DISALLOW_STATEMENT,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ALTER SYSTEM AGER 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssOptCodeSystemAger( smlTransId      aTransID,
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
 * @brief ALTER SYSTEM AGER 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssOptDataSystemAger( smlTransId      aTransID,
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
 * @brief ALTER SYSTEM AGER 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qlssExecuteSystemAger( smlTransId      aTransID,
                                 smlStatement  * aStmt,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv )
{
    qlssInitSystemAger  * sInitPlan = NULL;

    /**
     * Init Plan 획득 
     */

    sInitPlan = (qlssInitSystemAger *) aSQLStmt->mInitPlan;

    if( sInitPlan->mBehavior == QLP_AGER_START )
    {
        STL_TRY( smlActivateAger( SML_ENV(aEnv) ) == STL_SUCCESS );
    }
    else
    {
        if( sInitPlan->mBehavior == QLP_AGER_STOP )
        {
            STL_TRY( smlDeactivateAger( SML_ENV(aEnv) ) == STL_SUCCESS );
        }
        else
        {
            STL_TRY( smlLoopbackAger( STL_TRUE,     /* build agable scn */
                                      SML_ENV(aEnv) ) == STL_SUCCESS );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} qlssSystemAlterSystemAger */


