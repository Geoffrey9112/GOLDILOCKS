/*******************************************************************************
 * qldAlterDatabaseDropLogfileGroup.c
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
 * @file qldAlterDatabaseDropLogfileGroup.c
 * @brief ALTER DATABASE DROP LOGFILE GROUP 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>

/**
 * @defgroup qldAlterDatabaseDropLogfileGroup ALTER DATABASE DROP LOGFILE GROUP
 * @ingroup qldAlterDatabase
 * @{
 */




/**
 * @brief ALTER DATABASE DROP LOGFILE GROUP 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldValidateAlterDatabaseDropLogfileGroup( smlTransId      aTransID,
                                                    qllDBCStmt    * aDBCStmt,
                                                    qllStatement  * aSQLStmt,
                                                    stlChar       * aSQLString,
                                                    qllNode       * aParseTree,
                                                    qllEnv        * aEnv )
{
    qlpAlterDatabaseDropLogfileGroup * sParseTree;
    qldInitDropLogfileGroup  * sInitPlan = NULL;
    stlChar                  * sEndPtr;
    stlInt64                   sValue64;
    smlLogGroupAttr          * sGroupAttr;
    stlBool                    sFound = STL_FALSE;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_DROP_LOGFILE_GROUP_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_DATABASE_DROP_LOGFILE_GROUP_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sParseTree = (qlpAlterDatabaseDropLogfileGroup *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qldInitDropLogfileGroup),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qldInitDropLogfileGroup) );

    /**
     * 유효성 검사
     */
    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(smlLogGroupAttr),
                                (void **) & sGroupAttr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sGroupAttr, 0x00, STL_SIZEOF(smlLogGroupAttr) );

    STL_TRY( stlStrToInt64( QLP_GET_PTR_VALUE( sParseTree->mLogGroupId ),
                            STL_NTS,
                            &sEndPtr,
                            10,
                            &sValue64,
                            KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    STL_TRY( smlCheckLogGroupId( (stlInt32)sValue64,
                                 &sFound,
                                 SML_ENV(aEnv) ) == STL_SUCCESS );

    STL_TRY_THROW( (sValue64 <= STL_INT32_MAX) && (sFound == STL_TRUE),
                   RAMP_ERR_INVALID_LOGGROUP_NUMBER );

    sGroupAttr->mLogGroupId = sValue64;
    sInitPlan->mLogStreamAttr.mLogGroupAttr = sGroupAttr;

    /**
     * ALTER DATABASE ON DATABASE 권한 검사
     */

    STL_TRY( qlaCheckDatabasePriv( aTransID,
                                   aDBCStmt,
                                   aSQLStmt,
                                   ELL_DB_PRIV_ACTION_ALTER_DATABASE,
                                   aEnv )
             == STL_SUCCESS );
    
    /**
     * Init Plan 연결 
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;
 
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_LOGGROUP_NUMBER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INVALID_LOGGROUP_NUMBER,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mLogGroupId->mNodePos,
                                           aEnv ),
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief ALTER DATABASE DROP LOGFILE GROUP 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldOptCodeAlterDatabaseDropLogfileGroup( smlTransId      aTransID,
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
 * @brief ALTER DATABASE DROP LOGFILE GROUP 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldOptDataAlterDatabaseDropLogfileGroup( smlTransId      aTransID,
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
 * @brief ALTER DATABASE DROP LOGFILE GROUP 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qldExecuteAlterDatabaseDropLogfileGroup( smlTransId      aTransID,
                                                   smlStatement  * aStmt,
                                                   qllDBCStmt    * aDBCStmt,
                                                   qllStatement  * aSQLStmt,
                                                   qllEnv        * aEnv )
{
    qldInitDropLogfileGroup  * sInitPlan = (qldInitDropLogfileGroup*)aSQLStmt->mInitPlan;
    stlChar                  * sBuf;
    stlInt32                   sArgSize;

    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_DROP_LOGFILE_GROUP_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    sArgSize = STL_ALIGN(STL_SIZEOF(smlLogStreamAttr), 8) + STL_ALIGN(STL_SIZEOF(smlLogGroupAttr), 8);

    STL_TRY( knlAllocRegionMem( & QLL_EXEC_DDL_MEM(aEnv),
                                sArgSize,
                                (void **) & sBuf,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sBuf, 0x00, sArgSize );
    stlMemcpy( sBuf, &(sInitPlan->mLogStreamAttr), STL_SIZEOF(smlLogStreamAttr) );
    stlMemcpy( &(sBuf[STL_ALIGN(STL_SIZEOF(smlLogStreamAttr), 8)]),
               sInitPlan->mLogStreamAttr.mLogGroupAttr, STL_SIZEOF(smlLogGroupAttr) );

    STL_TRY( knlAddEnvEvent( SML_EVENT_DROP_LOGFILE_GROUP,
                             NULL,            /* aEventMem */
                             (void*)sBuf,     /* aData */
                             sArgSize,        /* aDataSize */
                             0, /* startup env id */
                             KNL_EVENT_WAIT_POLLING,
                             STL_FALSE, /* aForceSuccess */
                             KNL_ENV( aEnv ) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} qldAlterDatabaseDropLogfileGroup */
