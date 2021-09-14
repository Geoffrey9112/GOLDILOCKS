/*******************************************************************************
 * qltxCommit.c
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
 * @file qltxCommit.c
 * @brief COMMMIT 처리 루틴 
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @defgroup qltxCommit COMMIT 
 * @ingroup qltx
 * @{
 */


/**
 * @brief COMMIT 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltxValidateCommit( smlTransId      aTransID,
                              qllDBCStmt    * aDBCStmt,
                              qllStatement  * aSQLStmt,
                              stlChar       * aSQLString,
                              qllNode       * aParseTree,
                              qllEnv        * aEnv )
{
    qltxInitCommit * sInitPlan = NULL;
    qlpCommit      * sParseTree = NULL;
    stlXid           sXid;
    knlXaContext   * sXaContext;
    stlBool          sFound = STL_TRUE;
    stlInt32         sHashValue;
    dtlCharacterSet  sDBCharacterSet;
    stlInt64         sPos = 0;
    stlChar        * sComment;
    stlInt8          sMbLength;
    stlSize          sCommentLength;

    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_COMMIT_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_COMMIT_FORCE_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aParseTree->mType == QLL_STMT_COMMIT_TYPE) ||
                        (aParseTree->mType == QLL_STMT_COMMIT_FORCE_TYPE),
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */
    
    sParseTree = (qlpCommit *) aParseTree;
    
    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qltxInitCommit),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qltxInitCommit) );

    if( sParseTree->mForce != NULL )
    {
        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                    stlStrlen( QLP_GET_PTR_VALUE( sParseTree->mForce ) ) + 1,
                                    (void **) & sInitPlan->mXidString,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlStrcpy( sInitPlan->mXidString, QLP_GET_PTR_VALUE( sParseTree->mForce ) );

        STL_TRY_THROW( dtlStringToXid( sInitPlan->mXidString,
                                       &sXid,
                                       KNL_ERROR_STACK( aEnv ) )
                       == STL_SUCCESS,
                       RAMP_ERR_INVALID_XID_STRING );
        
        sHashValue = knlGetXidHashValue( &sXid );
        
        STL_TRY( knlFindXaContext( &sXid,
                                   sHashValue,
                                   &sXaContext,
                                   &sFound,
                                   KNL_ENV( aEnv ) ) == STL_SUCCESS );

        STL_TRY_THROW( sFound == STL_TRUE, RAMP_ERR_XA_NOTA );
        STL_TRY_THROW( sXaContext->mAssociateState == KNL_XA_ASSOCIATE_STATE_FALSE,
                       RAMP_ERR_INVALID_XA_STATE );
    }
    else
    {
        sInitPlan->mXidString = NULL;
    }

    if( sParseTree->mComment != NULL )
    {
        sDBCharacterSet = ellGetDbCharacterSet();
        
        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                    stlStrlen( QLP_GET_PTR_VALUE( sParseTree->mComment ) ) + 1,
                                    (void **) & sInitPlan->mComment,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sComment = QLP_GET_PTR_VALUE( sParseTree->mComment );
        sCommentLength = stlStrlen( sComment );
        
        while( 1 )
        {
            if( sPos == sCommentLength )
            {
                break;
            }
            
            STL_TRY( dtlMbStrVerify( sDBCharacterSet,
                                     sComment + sPos,
                                     sCommentLength - sPos,
                                     &sMbLength,
                                     QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS );

            if( sPos + sMbLength >= SML_MAX_COMMIT_COMMENT_SIZE )
            {
                break;
            }

            sPos += sMbLength;
        }

        stlStrncpy( sInitPlan->mComment, sComment, sPos + 1 );
    }
    else
    {
        sInitPlan->mComment = NULL;
    }

    sInitPlan->mWriteMode = sParseTree->mWriteMode;

    /**
     * 권한 검사 없음
     */
    
    /**
     * Init Plan 연결 
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;
        
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_XA_NOTA )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_XA_NOTA,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mForce->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_INVALID_XA_STATE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_XA_STATE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mForce->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_INVALID_XID_STRING )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_XID_STRING,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mForce->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief COMMIT 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltxOptCodeCommit( smlTransId      aTransID,
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
 * @brief COMMIT 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltxOptDataCommit( smlTransId      aTransID,
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
 * @brief COMMIT 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qltxExecuteCommit( smlTransId      aTransID,
                             smlStatement  * aStmt,
                             qllDBCStmt    * aDBCStmt,
                             qllStatement  * aSQLStmt,
                             qllEnv        * aEnv )
{
    qltxInitCommit * sInitPlan = NULL;
    smlTransId       sTransId;
    stlInt32         sState = 0;
    stlXid           sXid;
    smlStatement   * sStatement = NULL; 
    knlXaContext   * sXaContext;
    stlBool          sFound = STL_TRUE;
    stlBool          sIsTimedOut;
    stlInt32         sHashValue;

    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */
    
    sInitPlan = (qltxInitCommit *) aSQLStmt->mInitPlan;
    
    /**
     * COMMIT 을 수행함
     */

    if( aSQLStmt->mStmtType == QLL_STMT_COMMIT_TYPE )
    {
        /**
         * Commit local transaction
         */
        
        STL_TRY( qllCommit( aTransID,
                            sInitPlan->mComment,
                            sInitPlan->mWriteMode,
                            NULL, /* aXaContext */
                            aEnv )
                 == STL_SUCCESS );

        aSQLStmt->mDCLContext.mType = QLL_DCL_TYPE_END_TRANSACTION;
    }
    else
    {
        /**
         * Commit global transaction
         * - local transaction 정보는 수정하지 않는다.
         */

        STL_TRY( dtlStringToXid( sInitPlan->mXidString,
                                 &sXid,
                                 KNL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );
        
        sHashValue = knlGetXidHashValue( &sXid );
        
        STL_TRY( knlFindXaContext( &sXid,
                                   sHashValue,
                                   &sXaContext,
                                   &sFound,
                                   KNL_ENV( aEnv ) ) == STL_SUCCESS );

        STL_TRY_THROW( sFound == STL_TRUE, RAMP_ERR_XA_NOTA );
        
        STL_TRY( knlAcquireLatch( &sXaContext->mLatch,
                                  KNL_LATCH_MODE_EXCLUSIVE,
                                  KNL_LATCH_PRIORITY_NORMAL,
                                  STL_INFINITE_TIME,
                                  &sIsTimedOut,
                                  KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
        sState = 1;
        
        /**
         * latch 획득 이후에 다시 검사한다.
         */
        
        STL_TRY_THROW( knlIsEqualXid( &sXid,
                                      sHashValue,
                                      sXaContext ) == STL_TRUE,
                       RAMP_ERR_XA_NOTA );
        
        STL_TRY_THROW( sXaContext->mAssociateState == KNL_XA_ASSOCIATE_STATE_FALSE,
                       RAMP_ERR_INVALID_XA_STATE );

        if( sXaContext->mState != KNL_XA_STATE_HEURISTIC_COMPLETED )
        {
            STL_TRY_THROW( knlGetTransScope( QLL_SESS_ENV(aEnv) ) != KNL_TRANS_SCOPE_LOCAL,
                           RAMP_ERR_XA_OUTSIDE );

            STL_TRY_THROW( smlHasActiveStatement( SML_ENV(aEnv) ) == STL_FALSE,
                           RAMP_ERR_XA_OUTSIDE );
    
            STL_TRY( smlBeginTrans( SML_TIL_READ_COMMITTED,
                                    STL_TRUE,  /* aIsGlobalTrans */
                                    &sTransId,
                                    SML_ENV( aEnv ) )
                     == STL_SUCCESS );
            sState = 2;

            STL_TRY( smlAllocStatement( sTransId,
                                        NULL, /* aUpdateRelHandle */
                                        SML_STMT_ATTR_UPDATABLE | SML_STMT_ATTR_LOCKABLE,
                                        SML_LOCK_TIMEOUT_INVALID,
                                        STL_FALSE, /* aNeedSnapshotIterator */
                                        & sStatement,
                                        SML_ENV( aEnv ) )
                     == STL_SUCCESS );
            sState = 3;

            STL_TRY( ellInsertPendingTrans( sTransId,
                                            sStatement,
                                            sInitPlan->mXidString,
                                            sXaContext->mTransId,
                                            sInitPlan->mComment,
                                            STL_TRUE,  /* aHeuristicDecision */
                                            ELL_ENV( aEnv ) )
                     == STL_SUCCESS );

            sState = 2;
            STL_TRY( smlFreeStatement( sStatement,
                                       SML_ENV( aEnv ) )
                     == STL_SUCCESS );
            
            STL_TRY( smlCommitInDoubtTransaction( sTransId,
                                                  sXaContext->mTransId,
                                                  sInitPlan->mComment,
                                                  SML_ENV( aEnv ) )
                     == STL_SUCCESS );
            
            sState = 1;
            STL_TRY( smlEndTrans( sTransId,
                                  SML_PEND_ACTION_COMMIT,
                                  SML_TRANSACTION_CWM_WAIT,
                                  SML_ENV( aEnv ) )
                     == STL_SUCCESS );
                
            STL_TRY( smlEndTrans( sXaContext->mTransId,
                                  SML_PEND_ACTION_COMMIT,
                                  SML_TRANSACTION_CWM_WAIT,
                                  SML_ENV( aEnv ) )
                     == STL_SUCCESS );

            sXaContext->mState = KNL_XA_STATE_HEURISTIC_COMPLETED;
            sXaContext->mHeuristicDecision = KNL_XA_HEURISTIC_DECISION_COMMITTED;
        }
        
        sState = 0;
        STL_TRY( knlReleaseLatch( &sXaContext->mLatch,
                                  KNL_ENV( aEnv ) ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_XA_NOTA )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_XA_NOTA,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_INVALID_XA_STATE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_XA_STATE,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_XA_OUTSIDE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_XA_OUTSIDE,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    switch( sState )
    {
        case 3:
            (void) smlFreeStatement( sStatement, SML_ENV( aEnv ) );
        case 2:
            (void) smlRollback( sTransId,
                                SML_TRANSACTION_CWM_NO_WAIT,
                                SML_ENV( aEnv ) );
            (void) smlEndTrans( sTransId,
                                SML_PEND_ACTION_ROLLBACK,
                                SML_TRANSACTION_CWM_NO_WAIT,
                                SML_ENV( aEnv ) );
        case 1:
            (void) knlReleaseLatch( &sXaContext->mLatch, KNL_ENV(aEnv) );
        default:
            break;
    }

    return STL_FAILURE;
}



/** @} qltxCommit */
