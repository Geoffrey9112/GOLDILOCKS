/*******************************************************************************
 * qlssAlterSystemSessName.c
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
 * @file qlssAlterSystemSessName.c
 * @brief ALTER SYSTEM SESSION NAME 처리 루틴 
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @defgroup qlssSystemAlterSystemSessName ALTER SYSTEM SESSION NAME
 * @ingroup qlssSystem
 * @{
 */


/**
 * @brief ALTER SYSTEM SESSION NAME 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssValidateSystemSessName( smlTransId      aTransID,
                                      qllDBCStmt    * aDBCStmt,
                                      qllStatement  * aSQLStmt,
                                      stlChar       * aSQLString,
                                      qllNode       * aParseTree,
                                      qllEnv        * aEnv )
{
    qlssInitSystemSessName  * sInitPlan = NULL;
    qlpAlterSystemSessName  * sParseTree = NULL;

    stlInt32   sPos = 0;
    stlInt32   sNameLen;

    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_SYSTEM_SESS_NAME_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_SYSTEM_SESS_NAME_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sParseTree = (qlpAlterSystemSessName *) aParseTree;


    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlssInitSystemSessName),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlssInitSystemSessName) );

    /**
     * 유효성 검사
     */

    if( sParseTree->mSessName != NULL )
    {
        /* ALTER SYSTEM REGISTER SESSION NAME */
        sInitPlan->mIsRegister = STL_TRUE;
        sPos = sParseTree->mSessName->mNodePos;
        sNameLen = stlStrlen( QLP_GET_PTR_VALUE( sParseTree->mSessName ) );
        STL_TRY_THROW( (sNameLen > 0) && (sNameLen <= STL_MAX_SQL_IDENTIFIER_LENGTH),
                       RAMP_ERR_INVALID_SESS_NAME );

        stlStrncpy( sInitPlan->mSessName,
                    QLP_GET_PTR_VALUE( sParseTree->mSessName ),
                    STL_MAX_SQL_IDENTIFIER_LENGTH );
        sInitPlan->mSessName[sNameLen] = '\0';
    }
    else
    {
        /* ALTER SYSTEM CLEAR SESSION NAME */
        sInitPlan->mIsRegister = STL_FALSE;
    }

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

    STL_CATCH( RAMP_ERR_INVALID_SESS_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_BREAKPOINT_INVALID_SESS_NAME,
                      qlgMakeErrPosString( aSQLString,
                                           sPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ALTER SYSTEM SESSION NAME 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssOptCodeSystemSessName( smlTransId      aTransID,
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
 * @brief ALTER SYSTEM SESSION NAME 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssOptDataSystemSessName( smlTransId      aTransID,
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
 * @brief ALTER SYSTEM SESSION NAME 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qlssExecuteSystemSessName( smlTransId      aTransID,
                                     smlStatement  * aStmt,
                                     qllDBCStmt    * aDBCStmt,
                                     qllStatement  * aSQLStmt,
                                     qllEnv        * aEnv )
{
    qlssInitSystemSessName  * sInitPlan = NULL;

    /**
     * Init Plan 획득 
     */

    sInitPlan = (qlssInitSystemSessName *) aSQLStmt->mInitPlan;

    if( sInitPlan->mIsRegister == STL_TRUE )
    {
        STL_TRY( knlRegisterProcess( sInitPlan->mSessName,
                                     KNL_ENV(aEnv) ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( knlClearProcessInfo( KNL_ENV(aEnv) ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} qlssSystemAlterSystemSessName */


