/*******************************************************************************
 * qlaAlterUser.c
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
 * @file qlaAlterUser.c
 * @brief ALTER USER 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qlaAlterUser ALTER USER
 * @ingroup qla
 * @{
 */


/**
 * @brief ALTER USER 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlaValidateAlterUser( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                stlChar       * aSQLString,
                                qllNode       * aParseTree,
                                qllEnv        * aEnv )
{
    ellDictHandle    * sAuthHandle = NULL;
    
    qlaInitAlterUser  * sInitPlan = NULL;
    qlpAlterUser      * sParseTree = NULL;
    
    stlBool             sObjectExist = STL_FALSE;
    stlInt64            sDatabaseTestOption = 0;

    stlBool    sCheckPriv = STL_FALSE;

    stlInt32 i = 0;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_USER_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_USER_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sParseTree = (qlpAlterUser *) aParseTree;
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlaInitAlterUser),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlaInitAlterUser) );

    STL_TRY( qlvSetInitPlan( aDBCStmt,
                             aSQLStmt,
                             & sInitPlan->mCommonInit,
                             aEnv )
             == STL_SUCCESS );
    
    /*************************************************
     * User name 검증 
     *************************************************/

    /**
     * User 존재 여부
     */

    STL_TRY( ellGetAuthDictHandle( aTransID,
                                   aSQLStmt->mViewSCN,
                                   QLP_GET_PTR_VALUE( sParseTree->mUserName ),
                                   & sInitPlan->mUserHandle,
                                   & sObjectExist,
                                   ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_USER_NOT_EXIST );

    /*************************************************
     * Alter User Action 검증 
     *************************************************/

    sInitPlan->mAlterAction = sParseTree->mAlterAction;

    /**
     * Authorization 이 적절한 ALTER_ACTION 을 수행할 수 있는지 검사
     */
    
    if ( sInitPlan->mAlterAction == QLP_ALTER_USER_ACTION_SCHEMA_PATH )
    {
        STL_TRY_THROW( (ellGetAuthType( & sInitPlan->mUserHandle ) == ELL_AUTHORIZATION_TYPE_USER) ||
                       (ellGetAuthType( & sInitPlan->mUserHandle ) == ELL_AUTHORIZATION_TYPE_ALL_USER),
                       RAMP_NOT_USER );
    }
    else
    {
        STL_TRY_THROW( ellGetAuthType( & sInitPlan->mUserHandle ) == ELL_AUTHORIZATION_TYPE_USER,
                       RAMP_NOT_USER );
    }
    
    /**
     * Shuffle test에서는 허용하지 않는다.
     */
    sDatabaseTestOption = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_TEST_OPTION,
                                                         KNL_ENV(aEnv) );

    STL_TRY_THROW( sDatabaseTestOption < 1, RAMP_ERR_DISALLOW_STATEMENT );
    
    /**
     * ALTER USER ACTION 별 Validation
     */
    
    sCheckPriv = STL_TRUE;
    switch ( sInitPlan->mAlterAction )
    {
        case QLP_ALTER_USER_ACTION_PASSWORD:
            {
                if ( ellGetAuthID( & sInitPlan->mUserHandle ) == ellGetAuthID( sAuthHandle ) &&
                     (sParseTree->mOrgPassword != NULL) )
                {
                    /**
                     * 자신의 Password 를 변경하는 경우
                     */
                    
                    sCheckPriv = STL_FALSE;
                }
                else
                {
                    sCheckPriv = STL_TRUE;
                }

                STL_TRY( qlaValidateAlterPassword( aTransID,
                                                   aDBCStmt,
                                                   aSQLStmt,
                                                   aSQLString,
                                                   sParseTree,
                                                   sInitPlan,
                                                   aEnv )
                         == STL_SUCCESS );
                break;
            }
        case QLP_ALTER_USER_ACTION_ALTER_PROFILE:
            {
                STL_TRY( qlaValidateAlterUserProfile( aTransID,
                                                      aSQLStmt,
                                                      aSQLString,
                                                      sParseTree,
                                                      sInitPlan,
                                                      aEnv )
                         == STL_SUCCESS );
                break;
            }
        case QLP_ALTER_USER_ACTION_PASSWORD_EXPIRE:
            {
                /* nothing to do */
                break;
            }
        case QLP_ALTER_USER_ACTION_ACCOUNT_LOCK:
            {
                STL_TRY_THROW( ellGetAuthID( & sInitPlan->mUserHandle ) != ellGetAuthIdSYS(),
                               RAMP_ERR_CANNOT_LOCK_SYS );
                    
                break;
            }
        case QLP_ALTER_USER_ACTION_ACCOUNT_UNLOCK:
            {
                /* nothing to do */
                break;
            }
        case QLP_ALTER_USER_ACTION_DEFAULT_SPACE:
        case QLP_ALTER_USER_ACTION_TEMPORARY_SPACE:
            {
                STL_TRY( qlaValidateAlterSpace( aTransID,
                                                aDBCStmt,
                                                aSQLStmt,
                                                aSQLString,
                                                sParseTree,
                                                sInitPlan,
                                                aEnv )
                         == STL_SUCCESS );
                break;
            }
        case QLP_ALTER_USER_ACTION_SCHEMA_PATH:
            {
                STL_TRY( qlaValidateAlterSchemaPath( aTransID,
                                                     aDBCStmt,
                                                     aSQLStmt,
                                                     aSQLString,
                                                     sParseTree,
                                                     sInitPlan,
                                                     aEnv )
                         == STL_SUCCESS );
                break;
            }
        default:
            {
                STL_ASSERT(0);
                break;
            }
    }

    /**
     * ALTER USER ON DATABASE 권한 검사 
     */
    
    if ( sCheckPriv == STL_TRUE )
    {
        STL_TRY( qlaCheckDatabasePriv( aTransID,
                                       aDBCStmt,
                                       aSQLStmt,
                                       ELL_DB_PRIV_ACTION_ALTER_USER,
                                       aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /* nothing to do */
    }

    /********************************************************
     * Validation Object 추가 
     ********************************************************/

    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                  & sInitPlan->mUserHandle,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * ALTER ACTION 별 Object Handle 추가 
     */

    switch ( sInitPlan->mAlterAction )
    {
        case QLP_ALTER_USER_ACTION_PASSWORD:
        case QLP_ALTER_USER_ACTION_PASSWORD_EXPIRE:
        case QLP_ALTER_USER_ACTION_ACCOUNT_LOCK:
        case QLP_ALTER_USER_ACTION_ACCOUNT_UNLOCK:
            {
                /*
                 * nothing to do
                 */
                break;
            }
        case QLP_ALTER_USER_ACTION_ALTER_PROFILE:
            {
                if ( sInitPlan->mHasProfile == STL_TRUE )
                {
                    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                                  & sInitPlan->mProfileHandle,
                                                  QLL_INIT_PLAN(aDBCStmt),
                                                  ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                }
                else
                {
                    /* nothing to do */
                }
                
                break;
            }
        case QLP_ALTER_USER_ACTION_DEFAULT_SPACE:
        case QLP_ALTER_USER_ACTION_TEMPORARY_SPACE:
            {
                STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                              & sInitPlan->mSpaceHandle,
                                              QLL_INIT_PLAN(aDBCStmt),
                                              ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case QLP_ALTER_USER_ACTION_SCHEMA_PATH:
            {
                for ( i = 0; i < sInitPlan->mSchemaCnt; i++ )
                {
                    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                                  & sInitPlan->mSchemaHandleArray[i],
                                                  QLL_INIT_PLAN(aDBCStmt),
                                                  ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                }
                break;
            }
        default:
            {
                STL_ASSERT(0);
                break;
            }
    }
    
    /*************************************************
     * Output 설정 
     *************************************************/

    /**
     * Init Plan 연결 
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_USER_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_USER_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mUserName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      QLP_GET_PTR_VALUE( sParseTree->mUserName ) );
    }

    STL_CATCH( RAMP_NOT_USER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_USER_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mUserName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      QLP_GET_PTR_VALUE( sParseTree->mUserName ) );
    }
    
    STL_CATCH( RAMP_ERR_DISALLOW_STATEMENT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DISALLOW_STATEMENT,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_CANNOT_LOCK_SYS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CANNOT_LOCK_SYS_ACCOUNT,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    /* STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED ) */
    /* { */
    /*     stlPushError( STL_ERROR_LEVEL_ABORT, */
    /*                   QLL_ERRCODE_NOT_IMPLEMENTED, */
    /*                   NULL, */
    /*                   QLL_ERROR_STACK(aEnv), */
    /*                   "qlaValidateAlterUser()" ); */
    /* } */
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ALTER USER 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlaOptCodeAlterUser( smlTransId      aTransID,
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
 * @brief ALTER USER 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlaOptDataAlterUser( smlTransId      aTransID,
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
 * @brief ALTER USER 구문을 Execute 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlaExecuteAlterUser( smlTransId      aTransID,
                               smlStatement  * aStmt,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv )
{
    qlaInitAlterUser  * sInitPlan = NULL;

    stlTime  sTime = 0;

    stlBool   sLocked = STL_TRUE;

    stlInt32  i = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_USER_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mInitPlan != NULL, QLL_ERROR_STACK(aEnv) );

    /***************************************************************
     * 기본 정보 설정 
     ***************************************************************/
    
    /**
     * DDL 수행 시간 설정
     */

    sTime = stlNow();
    STL_TRY( ellBeginStmtDDL( aStmt, sTime, ELL_ENV(aEnv) ) == STL_SUCCESS );

    /***************************************************************
     *  Run-Time Validation
     ***************************************************************/

    STL_RAMP( RAMP_RETRY );
    STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    /**
     * Valid Plan 획득
     */

    STL_TRY( qlgGetValidPlan4DDL( aTransID,
                                  aDBCStmt,
                                  aSQLStmt,
                                  QLL_PHASE_EXECUTE,
                                  aEnv )
             == STL_SUCCESS );

    sInitPlan = (qlaInitAlterUser *) aSQLStmt->mInitPlan;
    
    /**
     * DDL Lock 획득 
     */

    STL_TRY( ellLock4AlterUser( aTransID,
                                aStmt,
                                & sInitPlan->mUserHandle,
                                & sLocked,
                                ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );

    /**
     * 관련 객체에 대한 Lock 획득
     */

    switch ( sInitPlan->mAlterAction )
    {
        case QLP_ALTER_USER_ACTION_PASSWORD:
            {
                sLocked = STL_TRUE;
                break;
            }
        case QLP_ALTER_USER_ACTION_ALTER_PROFILE:
            {
                if ( sInitPlan->mHasProfile == STL_TRUE )
                {
                    STL_TRY( ellLogicalObjectLock( aTransID,
                                                   aStmt,
                                                   & sInitPlan->mProfileHandle,
                                                   SML_LOCK_S,
                                                   & sLocked,
                                                   ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                }
                else
                {
                    sLocked = STL_TRUE;
                }
                break;
            }
        case QLP_ALTER_USER_ACTION_PASSWORD_EXPIRE:
            {
                sLocked = STL_TRUE;
                break;
            }
        case QLP_ALTER_USER_ACTION_ACCOUNT_LOCK:
        case QLP_ALTER_USER_ACTION_ACCOUNT_UNLOCK:
            {
                sLocked = STL_TRUE;
                break;
            }
        case QLP_ALTER_USER_ACTION_DEFAULT_SPACE:
        case QLP_ALTER_USER_ACTION_TEMPORARY_SPACE:
            {
                STL_TRY( ellLogicalObjectLock( aTransID,
                                               aStmt,
                                               & sInitPlan->mSpaceHandle,
                                               SML_LOCK_S,
                                               & sLocked,
                                               ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case QLP_ALTER_USER_ACTION_SCHEMA_PATH:
            {
                for ( i = 0; i < sInitPlan->mSchemaCnt; i++ )
                {
                    STL_TRY( ellLogicalObjectLock( aTransID,
                                                   aStmt,
                                                   & sInitPlan->mSchemaHandleArray[i],
                                                   SML_LOCK_S,
                                                   & sLocked,
                                                   ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                    
                    if ( sLocked == STL_FALSE )
                    {
                        break;
                    }
                }
                break;
            }
        default:
            {
                STL_ASSERT(0);
                break;
            }
    }

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    /**
     * Valid Plan 을 다시 획득
     */

    STL_TRY( qlgGetValidPlan4DDL( aTransID,
                                  aDBCStmt,
                                  aSQLStmt,
                                  QLL_PHASE_EXECUTE,
                                  aEnv )
             == STL_SUCCESS );

    sInitPlan = (qlaInitAlterUser *) aSQLStmt->mInitPlan;

    /***************************************************************
     * Dictionary Record 변경
     ***************************************************************/
    
    /**
     * ALTER USER ACTION 별 Dictionary Record 변경
     */
    
    switch ( sInitPlan->mAlterAction )
    {
        case QLP_ALTER_USER_ACTION_PASSWORD:
            {
                STL_TRY( qlaExecuteAlterPassword( aTransID,
                                                  aStmt,
                                                  aSQLStmt,
                                                  sInitPlan,
                                                  sTime,
                                                  aEnv )
                         == STL_SUCCESS );
                break;
            }
        case QLP_ALTER_USER_ACTION_ALTER_PROFILE:
            {
                STL_TRY( qlaExecuteAlterUserProfile( aTransID,
                                                     aStmt,
                                                     aSQLStmt,
                                                     sInitPlan,
                                                     sTime,
                                                     aEnv )
                         == STL_SUCCESS );
                
                break;
            }
        case QLP_ALTER_USER_ACTION_PASSWORD_EXPIRE:
            {
                STL_TRY( qlaExecuteAlterPasswordExpire( aTransID,
                                                        aStmt,
                                                        sInitPlan,
                                                        sTime,
                                                        aEnv )
                         == STL_SUCCESS );
                break;
            }
        case QLP_ALTER_USER_ACTION_ACCOUNT_LOCK:
        case QLP_ALTER_USER_ACTION_ACCOUNT_UNLOCK:
            {
                STL_TRY( qlaExecuteAlterAccountLocked( aTransID,
                                                       aStmt,
                                                       sInitPlan,
                                                       sTime,
                                                       aEnv )
                         == STL_SUCCESS );
                break;
            }
        case QLP_ALTER_USER_ACTION_DEFAULT_SPACE:
        case QLP_ALTER_USER_ACTION_TEMPORARY_SPACE:
            {
                STL_TRY( qlaExecuteAlterSpace( aTransID,
                                               aStmt,
                                               aDBCStmt,
                                               aSQLStmt,
                                               sInitPlan,
                                               aEnv )
                         == STL_SUCCESS );
                break;
            }
        case QLP_ALTER_USER_ACTION_SCHEMA_PATH:
            {
                STL_TRY( qlaExecuteAlterSchemaPath( aTransID,
                                                    aStmt,
                                                    aDBCStmt,
                                                    aSQLStmt,
                                                    sInitPlan,
                                                    aEnv )
                         == STL_SUCCESS );
                break;
            }
        default:
            {
                STL_ASSERT(0);
                break;
            }
    }

    /**
     * Authorization 변경 시간 설정
     */

    STL_TRY( ellSetTimeAlterAuth( aTransID,
                                  aStmt,
                                  ellGetAuthID( & sInitPlan->mUserHandle ),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /***************************************************************
     * Dictionary Cache 재구성 
     ***************************************************************/

    STL_TRY( ellRefineCache4AlterUser( aTransID,
                                       aStmt,
                                       & sInitPlan->mUserHandle,
                                       ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    /* STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED ) */
    /* { */
    /*     stlPushError( STL_ERROR_LEVEL_ABORT, */
    /*                   QLL_ERRCODE_NOT_IMPLEMENTED, */
    /*                   NULL, */
    /*                   QLL_ERROR_STACK(aEnv), */
    /*                   "qlaExecuteAlterUser()" ); */
    /* } */
    
    STL_FINISH;

    return STL_FAILURE;
}




/**
 * @brief ALTER PASSWORD 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree ALTER USER Parse Tree
 * @param[in] aInitPlan  ALTER USER Init Plan
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlaValidateAlterPassword( smlTransId         aTransID,
                                    qllDBCStmt       * aDBCStmt,
                                    qllStatement     * aSQLStmt,
                                    stlChar          * aSQLString,
                                    qlpAlterUser     * aParseTree,
                                    qlaInitAlterUser * aInitPlan,
                                    qllEnv           * aEnv )
{
    stlChar * sPassword = NULL;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_USER_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_USER_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan->mAlterAction == QLP_ALTER_USER_ACTION_PASSWORD, QLL_ERROR_STACK(aEnv) );

    /*****************************************************
     * Old Password 정보 
     *****************************************************/

    if ( aParseTree->mOrgPassword == NULL )
    {
        /* nothing to do */
        aInitPlan->mHasOrgPassword = STL_FALSE;
        aInitPlan->mPosOrgPassword = 0;
    }
    else
    {
        aInitPlan->mHasOrgPassword = STL_TRUE;
        aInitPlan->mPosOrgPassword = aParseTree->mOrgPassword->mNodePos;
        
        sPassword = QLP_GET_PTR_VALUE(aParseTree->mOrgPassword);
        stlStrcpy( aInitPlan->mPlainOrgPassword, sPassword );
    }

    /*****************************************************
     * New Password Encryption
     *****************************************************/

    /**
     * Password Encryption
     */
    
    sPassword = QLP_GET_PTR_VALUE(aParseTree->mNewPassword);
    stlStrcpy( aInitPlan->mPlainNewPassword, sPassword );
    
    stlMemset( aInitPlan->mEncryptPassword, 0x00, STL_MAX_SQL_IDENTIFIER_LENGTH );
    STL_TRY( stlMakeEncryptedPassword( sPassword,
                                       stlStrlen( sPassword ),
                                       aInitPlan->mEncryptPassword,
                                       STL_MAX_SQL_IDENTIFIER_LENGTH,
                                       QLL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief ALTER PROFILE 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree ALTER USER Parse Tree
 * @param[in] aInitPlan  ALTER USER Init Plan
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlaValidateAlterUserProfile( smlTransId         aTransID,
                                       qllStatement     * aSQLStmt,
                                       stlChar          * aSQLString,
                                       qlpAlterUser     * aParseTree,
                                       qlaInitAlterUser * aInitPlan,
                                       qllEnv           * aEnv )
{
    stlChar * sProfileName = NULL;
    stlBool   sObjectExist = STL_FALSE;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_USER_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_USER_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan->mAlterAction == QLP_ALTER_USER_ACTION_ALTER_PROFILE, QLL_ERROR_STACK(aEnv) );

    /*****************************************************
     * Profile 정보 획득
     *****************************************************/

    if ( aParseTree->mProfileName == NULL )
    {
        aInitPlan->mHasProfile = STL_FALSE;
        ellInitDictHandle( & aInitPlan->mProfileHandle );
    }
    else
    {
        aInitPlan->mHasProfile = STL_TRUE;
        sProfileName = QLP_GET_PTR_VALUE( aParseTree->mProfileName );

        STL_TRY( ellGetProfileDictHandleByName( aTransID,
                                                aSQLStmt->mViewSCN,
                                                sProfileName,
                                                & aInitPlan->mProfileHandle,
                                                & sObjectExist,
                                                ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_PROFILE_NOT_EXISTS );

        STL_TRY_THROW( ellGetAuthID( & aInitPlan->mUserHandle ) != ellGetAuthIdSYS(),
                       RAMP_ERR_CANNOT_PROFILE_SYS );
    }
        

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_PROFILE_NOT_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_PROFILE_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           aParseTree->mProfileName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sProfileName );
    }

    STL_CATCH( RAMP_ERR_CANNOT_PROFILE_SYS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CANNOT_ASSIGN_PROFILE_SYS_ACCOUNT,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief ALTER SPACE 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree ALTER USER Parse Tree
 * @param[in] aInitPlan  ALTER USER Init Plan
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlaValidateAlterSpace( smlTransId         aTransID,
                                 qllDBCStmt       * aDBCStmt,
                                 qllStatement     * aSQLStmt,
                                 stlChar          * aSQLString,
                                 qlpAlterUser     * aParseTree,
                                 qlaInitAlterUser * aInitPlan,
                                 qllEnv           * aEnv )
{
    stlChar * sSpaceName = NULL;
    stlBool   sObjectExist = STL_FALSE;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_USER_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_USER_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aInitPlan->mAlterAction == QLP_ALTER_USER_ACTION_DEFAULT_SPACE) ||
                        (aInitPlan->mAlterAction == QLP_ALTER_USER_ACTION_TEMPORARY_SPACE),
                        QLL_ERROR_STACK(aEnv) );

    /*****************************************************
     * Space Handle 정보 획득
     *****************************************************/

    sSpaceName = QLP_GET_PTR_VALUE(aParseTree->mSpaceName);
    
    STL_TRY( ellGetTablespaceDictHandle( aTransID,
                                         aSQLStmt->mViewSCN,
                                         sSpaceName,
                                         & aInitPlan->mSpaceHandle,
                                         & sObjectExist,
                                         ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SPACE_NOT_EXIST );

    if ( aInitPlan->mAlterAction == QLP_ALTER_USER_ACTION_DEFAULT_SPACE )
    {
        STL_TRY_THROW( ellGetTablespaceUsageType( & aInitPlan->mSpaceHandle ) == ELL_SPACE_USAGE_TYPE_DATA,
                       RAMP_ERR_NOT_DATA_SPACE );
    }
    else
    {
        STL_TRY_THROW( ellGetTablespaceUsageType( & aInitPlan->mSpaceHandle ) == ELL_SPACE_USAGE_TYPE_TEMPORARY,
                       RAMP_ERR_NOT_TEMP_SPACE );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SPACE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLESPACE_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           aParseTree->mSpaceName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sSpaceName );
    }

    STL_CATCH( RAMP_ERR_NOT_DATA_SPACE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLESPACE_NOT_DATA_TABLESPACE,
                      qlgMakeErrPosString( aSQLString,
                                           aParseTree->mSpaceName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sSpaceName );
    }

    STL_CATCH( RAMP_ERR_NOT_TEMP_SPACE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLESPACE_NOT_TEMPORARY_TABLESPACE,
                      qlgMakeErrPosString( aSQLString,
                                           aParseTree->mSpaceName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sSpaceName );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief ALTER SCHEMA PATH 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree ALTER USER Parse Tree
 * @param[in] aInitPlan  ALTER USER Init Plan
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlaValidateAlterSchemaPath( smlTransId         aTransID,
                                      qllDBCStmt       * aDBCStmt,
                                      qllStatement     * aSQLStmt,
                                      stlChar          * aSQLString,
                                      qlpAlterUser     * aParseTree,
                                      qlaInitAlterUser * aInitPlan,
                                      qllEnv           * aEnv )
{
    qlpListElement * sListElement = NULL;
    qlpValue       * sSchema = NULL;
    
    stlChar       * sSchemaName = NULL;
    ellDictHandle   sSchemaHandle;
    stlInt64        sSchemaID = ELL_DICT_OBJECT_ID_NA;
    stlBool         sObjectExist = STL_FALSE;

    stlInt32   sSchemaPathCnt = 0;
    stlInt64 * sSchemaPathArray = NULL;

    stlInt32  i = 0;
    stlInt32  j = 0;
    stlBool   sHasSchema = STL_FALSE;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_USER_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_USER_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan->mAlterAction == QLP_ALTER_USER_ACTION_SCHEMA_PATH,
                        QLL_ERROR_STACK(aEnv) );

    /*****************************************************
     * 최대 Schema 개수 계산 
     *****************************************************/

    sSchemaPathCnt = ellGetAuthSchemaPathCnt( & aInitPlan->mUserHandle );
    sSchemaPathArray = ellGetAuthSchemaPathArray( & aInitPlan->mUserHandle );

    aInitPlan->mSchemaMaxCnt = sSchemaPathCnt + QLP_LIST_GET_COUNT( aParseTree->mSchemaList );

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(ellDictHandle) * aInitPlan->mSchemaMaxCnt,
                                (void **) & aInitPlan->mSchemaHandleArray,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /*****************************************************
     * Schema List Validation
     *****************************************************/

    /**
     * Schema Name List 를 순회하며 Schema Handle 정보를 획득
     */

    aInitPlan->mSchemaCnt = 0;
    
    QLP_LIST_FOR_EACH( aParseTree->mSchemaList, sListElement )
    {
        sSchema = (qlpValue *) QLP_LIST_GET_POINTER_VALUE( sListElement );
        sSchemaName = QLP_GET_PTR_VALUE( sSchema );

        if ( stlStrcmp( sSchemaName, QLP_ALTER_USER_CURRENT_PATH ) == 0 )
        {
            /**
             * CURRENT PATH 인 경우
             */

            for ( i = 0; i < sSchemaPathCnt; i++ )
            {
                /**
                 * 이미 존재하는 지 검사
                 */

                sSchemaID = sSchemaPathArray[i];
                
                sHasSchema = STL_FALSE;
                for ( j = 0; j < aInitPlan->mSchemaCnt; j++ )
                {
                    if ( ellGetSchemaID( & aInitPlan->mSchemaHandleArray[j] ) == sSchemaID )
                    {
                        sHasSchema = STL_TRUE;
                        break;
                    }
                }

                if ( sHasSchema == STL_TRUE )
                {
                    /**
                     * 추가할 필요가 없음
                     */
                }
                else
                {
                    /**
                     * Schema Handle 획득
                     */
                    
                    STL_TRY( ellGetSchemaDictHandleByID( aTransID,
                                                         aSQLStmt->mViewSCN,
                                                         sSchemaPathArray[i],
                                                         & sSchemaHandle,
                                                         & sObjectExist,
                                                         ELL_ENV(aEnv) )
                             == STL_SUCCESS );

                    STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_USER_SCHEMA_DROPPED );
                    
                    /**
                     * 추가해야 함
                     */

                    stlMemcpy( & aInitPlan->mSchemaHandleArray[aInitPlan->mSchemaCnt],
                               & sSchemaHandle,
                               STL_SIZEOF( ellDictHandle ) );

                    aInitPlan->mSchemaCnt++;
                }
            }
        }
        else
        {
            /**
             * Schema Name 인 경우 
             */

            STL_TRY( ellGetSchemaDictHandle( aTransID,
                                             aSQLStmt->mViewSCN,
                                             sSchemaName,
                                             & sSchemaHandle,
                                             & sObjectExist,
                                             ELL_ENV(aEnv) )
                     == STL_SUCCESS );

            STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SCHEMA_NOT_EXIST );

            /**
             * 이미 존재하는 Schema 인지 검사
             */
            
            sSchemaID = ellGetSchemaID( & sSchemaHandle );
                
            sHasSchema = STL_FALSE;
            for ( j = 0; j < aInitPlan->mSchemaCnt; j++ )
            {
                if ( ellGetSchemaID( & aInitPlan->mSchemaHandleArray[j] ) == sSchemaID )
                {
                    sHasSchema = STL_TRUE;
                    break;
                }
            }

            if ( sHasSchema == STL_TRUE )
            {
                /**
                 * 추가할 필요가 없음
                 */
            }
            else
            {
                /**
                 * 추가해야 함
                 */
                
                stlMemcpy( & aInitPlan->mSchemaHandleArray[aInitPlan->mSchemaCnt],
                           & sSchemaHandle,
                           STL_SIZEOF( ellDictHandle ) );
                
                aInitPlan->mSchemaCnt++;
            }
        }
    }    
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_USER_SCHEMA_DROPPED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_USER_DEFAULT_SCHEMA_DROPPED,
                      qlgMakeErrPosString( aSQLString,
                                           sSchema->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_SCHEMA_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sSchema->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sSchemaName );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief ALTER PASSWORD 구문을 Execution 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aModifyTime  Modify Time
 * @param[in] aInitPlan  ALTER USER Init Plan
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlaExecuteAlterPassword( smlTransId         aTransID,
                                   smlStatement     * aStmt,
                                   qllStatement     * aSQLStmt,
                                   qlaInitAlterUser * aInitPlan,
                                   stlTime            aModifyTime,
                                   qllEnv           * aEnv )
{
    stlBool   sHasProfile = STL_FALSE;
    stlInt64  sProfileID = ELL_DICT_OBJECT_ID_NA;
    ellDictHandle sProfileHandle;
    stlBool       sObjectExist = STL_FALSE;

    ellUserProfileParam sProfileParam;
    ellAuthenInfo       sUserAuthenInfo;

    stlChar   sOrgEncrypt[STL_MAX_SQL_IDENTIFIER_LENGTH] = {0,};
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_USER_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan->mAlterAction == QLP_ALTER_USER_ACTION_PASSWORD, QLL_ERROR_STACK(aEnv) );

    /**
     * User 의 Authentication Information 획득
     */

    STL_TRY( ellGetUserAuthenticationInfo( aTransID,
                                           aStmt,
                                           & aInitPlan->mUserHandle,
                                           & sUserAuthenInfo,
                                           ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * User 와 관계된 Profile Parameter 획득
     */
    
    sProfileID = ellGetUserProfileID( & aInitPlan->mUserHandle );

    if ( sProfileID == ELL_DICT_OBJECT_ID_NA )
    {
        sHasProfile = STL_FALSE;
        
        ellInitDictHandle( & sProfileHandle );

        STL_TRY( ellGetUserProfileParam( aTransID,
                                         aStmt,
                                         NULL,
                                         & sProfileParam,
                                         ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        sHasProfile = STL_TRUE;
        
        STL_TRY( ellGetProfileDictHandleByID( aTransID,
                                              aStmt->mScn,
                                              sProfileID,
                                              & sProfileHandle,
                                              & sObjectExist,
                                              ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        STL_DASSERT( sObjectExist == STL_TRUE );

        STL_TRY( ellGetUserProfileParam( aTransID,
                                         aStmt,
                                         & sProfileHandle,
                                         & sProfileParam,
                                         ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    /**
     * Org Password 검증
     */

    if ( aInitPlan->mHasOrgPassword == STL_TRUE )
    {
        stlMemset( sOrgEncrypt, 0x00, STL_MAX_SQL_IDENTIFIER_LENGTH );
        STL_TRY( stlMakeEncryptedPassword( aInitPlan->mPlainOrgPassword,
                                           stlStrlen( aInitPlan->mPlainOrgPassword ),
                                           sOrgEncrypt,
                                           STL_MAX_SQL_IDENTIFIER_LENGTH,
                                           QLL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );

        STL_TRY_THROW( stlStrcmp( sOrgEncrypt, sUserAuthenInfo.mEncryptPassword ) == 0,
                       RAMP_ERR_INVALID_OLD_PASSWORD );
    }
    
    /**
     * New Password 검증
     */
    
    if ( sHasProfile == STL_TRUE )
    {
        /**
         * Password Verify Function 검증
         */

        if ( aInitPlan->mHasOrgPassword == STL_TRUE )
        {
            STL_TRY( qlaVerifyPassword( ellGetAuthName( & aInitPlan->mUserHandle ),
                                        aInitPlan->mPlainNewPassword,
                                        aInitPlan->mPlainOrgPassword,
                                        sProfileParam.mVerifyFuncID,
                                        aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            STL_TRY( qlaVerifyPassword( ellGetAuthName( & aInitPlan->mUserHandle ),
                                        aInitPlan->mPlainNewPassword,
                                        NULL,
                                        sProfileParam.mVerifyFuncID,
                                        aEnv )
                     == STL_SUCCESS );
        }

        /**
         * Password Reuse 검증
         */

        if ( ( sProfileParam.mReuseMax == ELL_PROFILE_LIMIT_UNLIMITED ) &&
             ( DTL_IS_NULL(sProfileParam.mReuseTime) == STL_TRUE ) )
        {
            /**
             * PASSWORD_REUSE_MAX UNLIMITED
             * PASSWORD_REUSE_TIME UNLIMITED
             */

            /* password reuse 를 검사할 필요가 없음 */
        }
        else
        {
            STL_TRY( qlaCheckPasswordReuse( aTransID,
                                            aStmt,
                                            & aInitPlan->mUserHandle,
                                            & sUserAuthenInfo,
                                            & sProfileParam,
                                            aInitPlan->mPlainNewPassword,
                                            aModifyTime,
                                            aEnv )
                     == STL_SUCCESS );
        }
    }
    else
    {
        /**
         * Profile 이 없는 경우
         */
    }
        
    /**
     * PASSWORD EXPIRY 관련 정보 변경
     */

    sUserAuthenInfo.mExpiryStatus = ELL_USER_EXPIRY_STATUS_OPEN;
        
    if ( DTL_IS_NULL( sProfileParam.mLifeTime ) == STL_TRUE )
    {
        sUserAuthenInfo.mExpiryTime = ELL_DICT_STLTIME_NA;
    }
    else
    {
        STL_TRY( dtlFuncStlTimeAddNumber( aModifyTime,
                                          sProfileParam.mLifeTime,
                                          & sUserAuthenInfo.mExpiryTime,
                                          KNL_DT_VECTOR(aEnv),
                                          aEnv,
                                          QLL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );
    }

    stlStrcpy( sUserAuthenInfo.mEncryptPassword, aInitPlan->mEncryptPassword );
    sUserAuthenInfo.mPassChangeCount++;
    
    /**
     * Password Dictionary Record 변경
     */

    STL_TRY( ellModifyUserAuthentication4DDL( aTransID,
                                              aStmt,
                                              & aInitPlan->mUserHandle,
                                              & sUserAuthenInfo,
                                              ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_OLD_PASSWORD )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_OLD_PASSWORD,
                      qlgMakeErrPosString( aSQLStmt->mRetrySQL,
                                           aInitPlan->mPosOrgPassword, 
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief ALTER PROFILE 구문을 Execution 한다.
 * @param[in] aTransID     Transaction ID
 * @param[in] aStmt        Statement
 * @param[in] aSQLStmt     SQL Statement
 * @param[in] aModifyTime  Modify Time
 * @param[in] aInitPlan    ALTER USER Init Plan
 * @param[in] aEnv         Environment
 * @remarks
 */
stlStatus qlaExecuteAlterUserProfile( smlTransId         aTransID,
                                      smlStatement     * aStmt,
                                      qllStatement     * aSQLStmt,
                                      qlaInitAlterUser * aInitPlan,
                                      stlTime            aModifyTime,
                                      qllEnv           * aEnv )
{
    stlInt64  sProfileID = ELL_DICT_OBJECT_ID_NA;

    ellUserProfileParam sProfileParam;
    ellAuthenInfo       sUserAuthenInfo;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_USER_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan->mAlterAction == QLP_ALTER_USER_ACTION_ALTER_PROFILE, QLL_ERROR_STACK(aEnv) );

    /**
     * User 의 Authentication Information 획득
     */

    STL_TRY( ellGetUserAuthenticationInfo( aTransID,
                                           aStmt,
                                           & aInitPlan->mUserHandle,
                                           & sUserAuthenInfo,
                                           ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * 새로운 Profile 와 관계된 Profile Parameter 획득
     */

    if ( aInitPlan->mHasProfile == STL_TRUE )
    {
        sProfileID = ellGetProfileID( & aInitPlan->mProfileHandle );

        STL_TRY( ellGetUserProfileParam( aTransID,
                                         aStmt,
                                         & aInitPlan->mProfileHandle,
                                         & sProfileParam,
                                         ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        sProfileID = ELL_DICT_OBJECT_ID_NA;
        STL_TRY( ellGetUserProfileParam( aTransID,
                                         aStmt,
                                         NULL,
                                         & sProfileParam,
                                         ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    /**
     * PASSWORD EXPIRY 관련 정보 변경
     */

    if ( sUserAuthenInfo.mExpiryStatus == ELL_USER_EXPIRY_STATUS_OPEN )
    {
        if ( DTL_IS_NULL( sProfileParam.mLifeTime ) == STL_TRUE )
        {
            sUserAuthenInfo.mExpiryTime = ELL_DICT_STLTIME_NA;
        }
        else
        {
            STL_TRY( dtlFuncStlTimeAddNumber( aModifyTime,
                                              sProfileParam.mLifeTime,
                                              & sUserAuthenInfo.mExpiryTime,
                                              KNL_DT_VECTOR(aEnv),
                                              aEnv,
                                              QLL_ERROR_STACK(aEnv) )
                     == STL_SUCCESS );
        }
    }
    else
    {
        /**
         * ACCOUNT Locked 상태 유지
         * PASSWORD Expiry 상태 유지
         * PASSWORD Verify 상태 유지
         */
    }

    /**
     * User Profile 변경
     */

    STL_TRY( ellModifyUserProfile( aTransID,
                                   aStmt,
                                   & aInitPlan->mUserHandle,
                                   sProfileID,
                                   & sUserAuthenInfo,
                                   ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief PASSWORD EXPIRE 구문을 Execution 한다.
 * @param[in] aTransID      Transaction ID
 * @param[in] aStmt         Statement
 * @param[in] aInitPlan     ALTER USER Init Plan
 * @param[in] aExpiryTime   Expiry Time 
 * @param[in] aEnv          Environment
 * @remarks
 */
stlStatus qlaExecuteAlterPasswordExpire( smlTransId         aTransID,
                                         smlStatement     * aStmt,
                                         qlaInitAlterUser * aInitPlan,
                                         stlTime            aExpiryTime,
                                         qllEnv           * aEnv )
{
    ellAuthenInfo  sAuthenInfo;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan->mAlterAction == QLP_ALTER_USER_ACTION_PASSWORD_EXPIRE, QLL_ERROR_STACK(aEnv) );

    /**
     * User Authentication 정보 획득
     */

    STL_TRY( ellGetUserAuthenticationInfo( aTransID,
                                           aStmt,
                                           & aInitPlan->mUserHandle,
                                           & sAuthenInfo,
                                           ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Expiry 상태 변경
     */

    sAuthenInfo.mExpiryStatus = ELL_USER_EXPIRY_STATUS_EXPIRED;
    sAuthenInfo.mExpiryTime   = aExpiryTime;
    
    /**
     * User Authentication 정보 변경
     */

    STL_TRY( ellModifyUserAuthentication4DDL( aTransID,
                                              aStmt,
                                              & aInitPlan->mUserHandle,
                                              & sAuthenInfo,
                                              ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief ACCOUNT LOCK | UNLOCK 구문을 Execution 한다.
 * @param[in] aTransID      Transaction ID
 * @param[in] aStmt         Statement
 * @param[in] aInitPlan     ALTER USER Init Plan
 * @param[in] aLockedTime   Locked Time 
 * @param[in] aEnv          Environment
 * @remarks
 */
stlStatus qlaExecuteAlterAccountLocked( smlTransId         aTransID,
                                        smlStatement     * aStmt,
                                        qlaInitAlterUser * aInitPlan,
                                        stlTime            aLockedTime,
                                        qllEnv           * aEnv )
{
    ellAuthenInfo  sAuthenInfo;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aInitPlan->mAlterAction == QLP_ALTER_USER_ACTION_ACCOUNT_LOCK) ||
                        (aInitPlan->mAlterAction == QLP_ALTER_USER_ACTION_ACCOUNT_UNLOCK)
                        , QLL_ERROR_STACK(aEnv) );

    /**
     * User Authentication 정보 획득
     */

    STL_TRY( ellGetUserAuthenticationInfo( aTransID,
                                           aStmt,
                                           & aInitPlan->mUserHandle,
                                           & sAuthenInfo,
                                           ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Locked 상태 변경
     */

    if ( aInitPlan->mAlterAction == QLP_ALTER_USER_ACTION_ACCOUNT_LOCK )
    {
        sAuthenInfo.mLockedStatus = ELL_USER_LOCKED_STATUS_LOCKED;
        sAuthenInfo.mLockedTime   = aLockedTime;
    }
    else
    {
        sAuthenInfo.mLockedStatus = ELL_USER_LOCKED_STATUS_OPEN;
        sAuthenInfo.mLockedTime   = ELL_DICT_STLTIME_NA;
        sAuthenInfo.mFailedLoginAttempt = 0;
    }
    
    /**
     * User Authentication 정보 변경
     */

    STL_TRY( ellModifyUserAuthentication4DDL( aTransID,
                                              aStmt,
                                              & aInitPlan->mUserHandle,
                                              & sAuthenInfo,
                                              ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief ALTER SPACE 구문을 Execution 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aInitPlan  ALTER USER Init Plan
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlaExecuteAlterSpace( smlTransId         aTransID,
                                smlStatement     * aStmt,
                                qllDBCStmt       * aDBCStmt,
                                qllStatement     * aSQLStmt,
                                qlaInitAlterUser * aInitPlan,
                                qllEnv           * aEnv )
{
    stlInt64  sSpaceID = ELL_DICT_OBJECT_ID_NA;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_USER_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aInitPlan->mAlterAction == QLP_ALTER_USER_ACTION_DEFAULT_SPACE) ||
                        (aInitPlan->mAlterAction == QLP_ALTER_USER_ACTION_TEMPORARY_SPACE),
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sSpaceID = ellGetTablespaceID( & aInitPlan->mSpaceHandle );
    
    /**
     * User Dictionary Record 변경
     */

    if (aInitPlan->mAlterAction == QLP_ALTER_USER_ACTION_DEFAULT_SPACE)
    {
        STL_TRY( ellModifyUserDataSpace( aTransID,
                                         aStmt,
                                         & aInitPlan->mUserHandle,
                                         sSpaceID,
                                         ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( ellModifyUserTempSpace( aTransID,
                                         aStmt,
                                         & aInitPlan->mUserHandle,
                                         sSpaceID,
                                         ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief ALTER SCHEMA PATH 구문을 Execution 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aInitPlan  ALTER USER Init Plan
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlaExecuteAlterSchemaPath( smlTransId         aTransID,
                                     smlStatement     * aStmt,
                                     qllDBCStmt       * aDBCStmt,
                                     qllStatement     * aSQLStmt,
                                     qlaInitAlterUser * aInitPlan,
                                     qllEnv           * aEnv )
{
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_USER_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan->mAlterAction == QLP_ALTER_USER_ACTION_SCHEMA_PATH, QLL_ERROR_STACK(aEnv) );

    /**
     * Auth Dictionary Record 변경
     */

    STL_TRY( ellModifyAuthSchemaPath( aTransID,
                                      aStmt,
                                      & aInitPlan->mUserHandle,
                                      aInitPlan->mSchemaCnt,
                                      aInitPlan->mSchemaHandleArray,
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} qlaAlterUser */
