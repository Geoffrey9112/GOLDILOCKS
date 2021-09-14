/*******************************************************************************
 * qlssAlterSystemBreakpoint.c
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
 * @file qlssAlterSystemBreakpoint.c
 * @brief ALTER SYSTEM BREAKPOINT 처리 루틴 
 */

#include <qll.h>
#include <qlDef.h>



/**
 * @defgroup qlssSystemAlterSystemBreakpoint ALTER SYSTEM BREAKPOINT
 * @ingroup qlssSystem
 * @{
 */


/**
 * @brief ALTER SYSTEM BREAKPOINT 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssValidateSystemBreakpoint( smlTransId      aTransID,
                                        qllDBCStmt    * aDBCStmt,
                                        qllStatement  * aSQLStmt,
                                        stlChar       * aSQLString,
                                        qllNode       * aParseTree,
                                        qllEnv        * aEnv )
{
    qlssInitSystemBreakpoint  * sInitPlan = NULL;
    qlpAlterSystemBreakpoint  * sParseTree = NULL;

    stlInt64          sValue64;
    stlInt32          sPos = 0;
    stlChar         * sEndPtr;
    stlInt64          sDatabaseTestOption = 0;

    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_SYSTEM_BREAKPOINT_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_SYSTEM_BREAKPOINT_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */
    
    sParseTree = (qlpAlterSystemBreakpoint *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlssInitSystemBreakpoint),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlssInitSystemBreakpoint) );

    /**
     * 유효성 검사
     */

    if( sParseTree->mBreakpointName == NULL )
    {
        /* ALTER SYSTEM CLEAR BREAKPOINT */
        STL_PARAM_VALIDATE( sParseTree->mOwnerSessName == NULL,
                            QLL_ERROR_STACK(aEnv) );
        STL_PARAM_VALIDATE( sParseTree->mOption == NULL,
                            QLL_ERROR_STACK(aEnv) );

        sInitPlan->mIsRegister     = STL_FALSE;
        sInitPlan->mBreakpointId   = KNL_BREAKPOINT_MAX;
        sInitPlan->mSessType       = KNL_BREAKPOINT_SESSION_TYPE_NONE;
        sInitPlan->mOwnerSessId    = -1;
        sInitPlan->mSkipCount      = -1;
        sInitPlan->mActionType     = KNL_BREAKPOINT_ACTION_WAKEUP;
        sInitPlan->mPostActionType = KNL_BREAKPOINT_POST_ACTION_MAX;
        sInitPlan->mArgument       = -1;

    }
    else
    {
        sDatabaseTestOption = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_TEST_OPTION,
                                                             KNL_ENV(aEnv) );

        STL_TRY_THROW( sDatabaseTestOption < 1, RAMP_ERR_DISALLOW_STATEMENT );
    
        /* ALTER SYSTEM REGISTER BREAKPOINT */
        STL_PARAM_VALIDATE( sParseTree->mOwnerSessName != NULL,
                            QLL_ERROR_STACK(aEnv) );
        STL_PARAM_VALIDATE( sParseTree->mOption != NULL,
                            QLL_ERROR_STACK(aEnv) );

        sInitPlan->mIsRegister = STL_TRUE;

        sPos = sParseTree->mBreakpointName->mNodePos;
        knlFindBreakpointIdFromName( QLP_GET_PTR_VALUE( sParseTree->mBreakpointName ),
                                     &sInitPlan->mBreakpointId );

        STL_TRY_THROW( sInitPlan->mBreakpointId < KNL_BREAKPOINT_MAX, RAMP_ERR_INVALID_BREAKPOINT_NAME );

        sInitPlan->mSessType = sParseTree->mSessType;

        sPos = sParseTree->mOwnerSessName->mNodePos;
        knlFindProcIdFromName( QLP_GET_PTR_VALUE( sParseTree->mOwnerSessName ),
                               &sInitPlan->mOwnerSessId );

        STL_TRY_THROW( (sInitPlan->mOwnerSessId > -1) || (knlGetProcCount() > KNL_MAX_BP_PROCESS_COUNT),
                       RAMP_ERR_INVALID_SESS_NAME );

        if( sParseTree->mOption->mSkipCount != NULL )
        {
            sPos = sParseTree->mOption->mSkipCount->mNodePos;
            STL_TRY( stlStrToInt64( QLP_GET_PTR_VALUE( sParseTree->mOption->mSkipCount ),
                                    STL_NTS,
                                    &sEndPtr,
                                    10,
                                    &sValue64,
                                    KNL_ERROR_STACK(aEnv) )
                     == STL_SUCCESS );

            sInitPlan->mSkipCount = (stlInt32)sValue64;
        }
        else
        {
            sInitPlan->mSkipCount = 0;
        }

        sInitPlan->mActionType = sParseTree->mOption->mAction->mActionType;
        if( sParseTree->mOption->mAction->mActionType == KNL_BREAKPOINT_ACTION_WAKEUP )
        {
            sPos = sParseTree->mOption->mAction->mArgument->mNodePos;
            knlFindProcIdFromName( QLP_GET_PTR_VALUE( sParseTree->mOption->mAction->mArgument ),
                                   &sInitPlan->mArgument );

            STL_TRY_THROW( sInitPlan->mArgument > -1, RAMP_ERR_INVALID_SESS_NAME );

            sInitPlan->mPostActionType = sParseTree->mOption->mAction->mPostActionType;
        }
        else if( sParseTree->mOption->mAction->mActionType == KNL_BREAKPOINT_ACTION_SLEEP )
        {
            sPos = sParseTree->mOption->mAction->mArgument->mNodePos;
            STL_TRY( stlStrToInt64( QLP_GET_PTR_VALUE( sParseTree->mOption->mAction->mArgument ),
                                    STL_NTS,
                                    &sEndPtr,
                                    10,
                                    &sValue64,
                                    KNL_ERROR_STACK(aEnv) )
                     == STL_SUCCESS );

            sInitPlan->mArgument = (stlInt32)sValue64;
        }
        else
        {
            sInitPlan->mArgument = -1;
        }
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

    STL_CATCH( RAMP_ERR_INVALID_BREAKPOINT_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_INVALID_BREAKPOINT_NAME,
                      qlgMakeErrPosString( aSQLString,
                                           sPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    STL_CATCH( RAMP_ERR_INVALID_SESS_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_BREAKPOINT_INVALID_SESS_NAME,
                      qlgMakeErrPosString( aSQLString,
                                           sPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

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
 * @brief ALTER SYSTEM BREAKPOINT 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssOptCodeSystemBreakpoint( smlTransId      aTransID,
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
 * @brief ALTER SYSTEM BREAKPOINT 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssOptDataSystemBreakpoint( smlTransId      aTransID,
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
 * @brief ALTER SYSTEM BREAKPOINT 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qlssExecuteSystemBreakpoint( smlTransId      aTransID,
                                       smlStatement  * aStmt,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv )
{
    knlSessionEnv             * sSessionEnv;
    qlssInitSystemBreakpoint  * sInitPlan = NULL;

    /**
     * Init Plan 획득 
     */

    /* shared 모드이면 breakpoint 실패 처리 */
    sSessionEnv = KNL_SESS_ENV( aEnv );
    STL_TRY_THROW( sSessionEnv->mSessionType != KNL_SESSION_TYPE_SHARED, RAMP_ERR_BREAKPOINT_MUST_DEDICATED );

    sInitPlan = (qlssInitSystemBreakpoint *) aSQLStmt->mInitPlan;

    if( sInitPlan->mIsRegister == STL_TRUE )
    {
        STL_TRY( knlRegisterAction( sInitPlan->mBreakpointId,
                                    sInitPlan->mSessType,
                                    sInitPlan->mOwnerSessId,
                                    sInitPlan->mSkipCount,
                                    sInitPlan->mActionType,
                                    sInitPlan->mPostActionType,
                                    sInitPlan->mArgument,
                                    KNL_ENV(aEnv) ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( knlClearActionInfo( KNL_ENV(aEnv) ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_BREAKPOINT_MUST_DEDICATED )
    {
        /**
         * shared session에서 break point 사용 못함.
         * 에러는 일부러 break point error가 나타나지 않도록 하였음.
         */
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DISALLOW_STATEMENT,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    STL_FINISH;

    return STL_FAILURE;
}



/** @} qlssSystemAlterSystemBreakpoint */


