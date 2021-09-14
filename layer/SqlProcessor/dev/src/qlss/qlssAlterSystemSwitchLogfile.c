/*******************************************************************************
 * qlssAlterSystemSwitchLogFile.c
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
 * @file qlssAlterSystemSwitchLogfile.c
 * @brief ALTER SYSTEM SWITCH LOGFILE 처리 루틴 
 */

#include <sml.h>
#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qlssAlterSystemSwitchLogFile ALTER SYSTEM SWITCH LOGFILE
 * @ingroup qlssSystem
 * @{
 */


/**
 * @brief ALTER SYSTEM SWITCH LOGFILE 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssValidateAlterSystemSwitchLogfile( smlTransId      aTransID,
                                                qllDBCStmt    * aDBCStmt,
                                                qllStatement  * aSQLStmt,
                                                stlChar       * aSQLString,
                                                qllNode       * aParseTree,
                                                qllEnv        * aEnv )
{
    qlssInitSwitchLogfile * sInitPlan   = NULL;
    
    /**
     * nothing to do
     */

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlssInitSwitchLogfile),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlssInitSwitchLogfile) );

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

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ALTER SYSTEM SWITCH LOGFILE 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssOptCodeAlterSystemSwitchLogfile( smlTransId      aTransID,
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
 * @brief ALTER SYSTEM SWITCH LOGFILE 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssOptDataAlterSystemSwitchLogfile( smlTransId      aTransID,
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
 * @brief ALTER SYSTEM SWITCH LOGFILE 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qlssExecuteAlterSystemSwitchLogfile( smlTransId      aTransID,
                                               smlStatement  * aStmt,
                                               qllDBCStmt    * aDBCStmt,
                                               qllStatement  * aSQLStmt,
                                               qllEnv        * aEnv )
{
    if( knlGetCurrStartupPhase() >= KNL_STARTUP_PHASE_OPEN )
    {
        STL_TRY( smlSwitchLogGroup( SML_ENV(aEnv) ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( knlAddEnvEvent( SML_EVENT_SWITCH_LOGFILE_GROUP,
                                 NULL,     /* aEventMem */
                                 NULL,     /* aData */
                                 0,        /* aDataSize */
                                 0,        /* startup env id */
                                 KNL_EVENT_WAIT_POLLING,
                                 STL_FALSE, /* aForceSuccess */
                                 KNL_ENV( aEnv ) ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} qlssAlterSystemSwitchLogFile */
