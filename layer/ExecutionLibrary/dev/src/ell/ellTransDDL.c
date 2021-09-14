/*******************************************************************************
 * ellTransDDL.c
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
 * @file ellTransDDL.c
 * @brief Execution Layer 의 DDL Transaction 관리 공통 모듈 
 */

#include <ell.h>
#include <elDef.h>

#include <eldt.h>
#include <eldc.h>

#include <eldDictionary.h>
#include <elgPendOp.h>

/**
 * @addtogroup ellTransDDL
 * @{
 */

stlBool gEllIsMetaBuilding = STL_FALSE;

/**
 * @brief DDL Statement 를 시작
 * @param[in] aStmt      Statement
 * @param[in] aTimeDDL   DDL이 시작한 시간
 *                  <BR> 객체가 생성된 시간, 변경된 시간 정보를 관리하기 위해 사용한다.
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus ellBeginStmtDDL( smlStatement * aStmt, stlTime aTimeDDL, ellEnv * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * DDL Statement 시작
     */

    sSessEnv->mTimeDDL = aTimeDDL;
    sSessEnv->mStmtTimestamp = smlGetStatementTimestamp( aStmt );
    
    /**
     * DDL 이 존재함을 설정 
     */
    
    sSessEnv->mIncludeDDL = STL_TRUE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief DDL Statement를 정상적으로 종료
 * @param[in] aStmt      Statement
 * @param[in] aEnv       Environment
 * @remarks
 * Statement 가 정상 종료하므로 Transaction Rollback 을 위한 Pending Operation으로 변경
 * 연산 순서의 역순으로 수행될 수 있도록 맨앞에 연결한다.
 */
stlStatus ellEndStmtDDL( smlStatement * aStmt,
                         ellEnv       * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);

    /**
     * Pending Operation들을 Transaction Rollback을 위한 Pending Operation 으로 변경 
     */
    
    if ( sSessEnv->mIncludeDDL == STL_TRUE )
    {
        STL_TRY( elgExecStmtPreRollbackAction( SML_INVALID_TRANSID,
                                               STL_TRUE,  /* aIsSuccess */
                                               aEnv )
                 == STL_SUCCESS );
        
        elgExecStmtSuccessPendOP( aEnv );
    }
    else
    {
        /**
         * nothing to do
         */
        
        STL_DASSERT( sSessEnv->mStmtRollbackPendOpList == NULL );
        STL_DASSERT( sSessEnv->mStmtSuccessPendOpList == NULL );
        STL_DASSERT( sSessEnv->mStmtPreRollbackActionList == NULL );

        STL_DASSERT( sSessEnv->mTxPreRollbackActionList == NULL );
        STL_DASSERT( sSessEnv->mTxRollbackPendOpList == NULL );
        STL_DASSERT( sSessEnv->mTxCommitPendOpList == NULL );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief DDL Statement를 Rollback
 * @param[in] aTransID   Transaction ID
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus ellRollbackStmtDDL( smlTransId     aTransID,
                              ellEnv       * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);
        
    /*
     * Parameter Validation
     */

    /**
     * Pending Operation 을 수행
     */
    
    if( sSessEnv->mIncludeDDL == STL_TRUE )
    {
        STL_TRY( elgExecStmtPreRollbackAction( aTransID,
                                               STL_FALSE,  /* aIsSuccess */
                                               aEnv )
                 == STL_SUCCESS );
        
        STL_TRY( elgExecStmtRollbackPendOP( aTransID, aEnv ) == STL_SUCCESS );
    }
    else
    {
        /* nothing to do */
        
        STL_DASSERT( sSessEnv->mStmtRollbackPendOpList == NULL );
        STL_DASSERT( sSessEnv->mStmtSuccessPendOpList == NULL );
        STL_DASSERT( sSessEnv->mStmtPreRollbackActionList == NULL );

        STL_DASSERT( sSessEnv->mTxPreRollbackActionList == NULL );
        STL_DASSERT( sSessEnv->mTxRollbackPendOpList == NULL );
        STL_DASSERT( sSessEnv->mTxCommitPendOpList == NULL );
    }
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief DDL Commit 전  Pre-Action 을 수행 
 * @param[in] aTransID      Transaction ID
 * @param[in] aEnv          Environment
 * @remarks
 */
stlStatus ellPreActionCommitDDL( smlTransId       aTransID,
                                 ellEnv         * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);
    
    /**
     * Pending Operation 을 수행
     */
    
    if ( sSessEnv->mIncludeDDL == STL_TRUE )
    {
        STL_TRY( elgExecPreActionTransEnd( aTransID,
                                           ELG_PEND_ACT_COMMIT,
                                           aEnv )
                 == STL_SUCCESS );
        
    }
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief DDL Rollback 전  Pre-Action 을 수행 
 * @param[in] aTransID      Transaction ID
 * @param[in] aEnv          Environment
 * @remarks
 */
stlStatus ellPreActionRollbackDDL( smlTransId       aTransID,
                                   ellEnv         * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);
    
    /**
     * Pending Operation 을 수행
     */
    
    if ( sSessEnv->mIncludeDDL == STL_TRUE )
    {
        STL_TRY( elgExecPreActionTransEnd( aTransID,
                                           ELG_PEND_ACT_ROLLBACK,
                                           aEnv )
                 == STL_SUCCESS );
        
    }
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief DDL Transaction 을 Commit
 * @param[in] aTransID   Transaction ID
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus ellCommitTransDDL( smlTransId aTransID,
                             ellEnv   * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);

    /*
     * Parameter Validation
     */

    /* 종료되지 않은 DDL Statement가 없어야 함 */
    STL_PARAM_VALIDATE( sSessEnv->mStmtRollbackPendOpList == NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sSessEnv->mStmtSuccessPendOpList == NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sSessEnv->mStmtPreRollbackActionList == NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sSessEnv->mTxPreRollbackActionList == NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Pending Operation 을 수행
     */

    if ( sSessEnv->mIncludeDDL == STL_TRUE )
    {
        STL_TRY( elgExecTransPendOP( aTransID, ELG_PEND_ACT_COMMIT, aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /* nothing to do */
    }

    /**
     * DDL 이 없음을 설정 
     */
    
    sSessEnv->mIncludeDDL = STL_FALSE;
    sSessEnv->mLocalModifySeq = 0;

    /**
     * 모든 Local Cache 들을 삭제한다.
     */
    
    STL_TRY( eldcFreeLocalCaches( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    sSessEnv->mIncludeDDL = STL_FALSE;
    sSessEnv->mLocalModifySeq = 0;
    
    (void) eldcFreeLocalCaches( aEnv );
    
    return STL_FAILURE;
}
    
/**
 * @brief DDL Transaction 을 Rollback
 * @param[in] aTransID   Transaction ID
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus ellRollbackTransDDL( smlTransId aTransID,
                               ellEnv   * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);

    /*
     * Parameter Validation
     */

    /* 종료되지 않은 DDL Statement가 없어야 함 */
    STL_PARAM_VALIDATE( sSessEnv->mStmtRollbackPendOpList == NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sSessEnv->mStmtSuccessPendOpList == NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sSessEnv->mStmtPreRollbackActionList == NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sSessEnv->mTxPreRollbackActionList == NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Pending Operation 을 수행
     */
    
    if ( sSessEnv->mIncludeDDL == STL_TRUE )
    {
        STL_TRY( elgExecTransPendOP( aTransID,
                                     ELG_PEND_ACT_ROLLBACK,
                                     aEnv )
                 == STL_SUCCESS );
        
    }
    else
    {
        /* nothing to do */

        STL_DASSERT( sSessEnv->mStmtRollbackPendOpList == NULL );
        STL_DASSERT( sSessEnv->mStmtSuccessPendOpList == NULL );
        STL_DASSERT( sSessEnv->mStmtPreRollbackActionList == NULL );
        
        STL_DASSERT( sSessEnv->mTxPreRollbackActionList == NULL );
        STL_DASSERT( sSessEnv->mTxRollbackPendOpList == NULL );
        STL_DASSERT( sSessEnv->mTxCommitPendOpList == NULL );
    }

    /**
     * DDL 이 없음을 설정 
     */
    
    sSessEnv->mIncludeDDL = STL_FALSE;
    sSessEnv->mLocalModifySeq = 0;
    
    /**
     * 모든 Local Cache 들을 삭제한다.
     */
    
    STL_TRY( eldcFreeLocalCaches( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    sSessEnv->mIncludeDDL = STL_FALSE;
    sSessEnv->mLocalModifySeq = 0;

    (void) eldcFreeLocalCaches( aEnv );
    
    return STL_FAILURE;
}

/**
 * @brief Savepoint 까지 DDL 을 Rollback 한다.
 * @param[in] aTransID              Transaction ID
 * @param[in] aSavepointTimestamp   Savepoint Timestamp
 * @param[in] aEnv                  Environment
 * @remarks
 */
stlStatus ellRollbackToSavepointDDL( smlTransId aTransID,
                                     stlInt64   aSavepointTimestamp,
                                     ellEnv   * aEnv )
{
    return elgExecRollbackToSavepointPendOP( aTransID,
                                             aSavepointTimestamp,
                                             aEnv );
}

/**
 * @brief Clean-Up 과정을 수행하기 위해 DDL 관련 Session 정보를 이관한다.
 * @param[out] aDstSessionEnv  destin Session Env
 * @param[in]  aSrcSessionEnv  source Session Env
 */
stlStatus ellCleanupHandoverSession( ellSessionEnv * aDstSessionEnv,
                                     ellSessionEnv * aSrcSessionEnv )
{
    /**
     * DDL 관련 정보 이관
     */

    aDstSessionEnv->mIncludeDDL           = aSrcSessionEnv->mIncludeDDL;
    
    aDstSessionEnv->mStmtRollbackPendOpList    = aSrcSessionEnv->mStmtRollbackPendOpList;
    aDstSessionEnv->mStmtSuccessPendOpList     = aSrcSessionEnv->mStmtSuccessPendOpList;
    aDstSessionEnv->mStmtPreRollbackActionList = aSrcSessionEnv->mStmtPreRollbackActionList;
    
    aDstSessionEnv->mTxPreRollbackActionList = aSrcSessionEnv->mTxPreRollbackActionList;
    aDstSessionEnv->mTxRollbackPendOpList    = aSrcSessionEnv->mTxRollbackPendOpList;
    aDstSessionEnv->mTxCommitPendOpList      = aSrcSessionEnv->mTxCommitPendOpList;

    /**
     * Source DDL 정보 초기화
     */
    aSrcSessionEnv->mIncludeDDL = STL_FALSE;
    
    aSrcSessionEnv->mStmtRollbackPendOpList    = NULL;
    aSrcSessionEnv->mStmtSuccessPendOpList     = NULL;
    aSrcSessionEnv->mStmtPreRollbackActionList = NULL;
    
    aSrcSessionEnv->mTxPreRollbackActionList = NULL;
    aSrcSessionEnv->mTxRollbackPendOpList    = NULL;
    aSrcSessionEnv->mTxCommitPendOpList      = NULL;
    
    return STL_SUCCESS;
}


/**
 * @brief FATAL 상황시 진행중이던 DDL 에 대해 ROLLBACK 수행전 Pre-Rollback 을 수행한다.
 * @param[in]   aTransID   Transaction ID
 * @param[in]   aEnv       Environment
 * @warning
 * Clean Up 과정중에 heap memory 에 대한 alloc/free 가 존재하면 안됨.
 * - 즉, aEnv 나 aSessionEnv 의 memory manager 를 사용하면 안됨.
 */
stlStatus ellCleanupPriorRollbackTransDDL( smlTransId     aTransID,
                                           ellEnv       * aEnv )
{
    /**
     * DDL Statement Pending Rollback 을 수행
     */

    STL_TRY( elgExecStmtRollbackPendOP( aTransID, aEnv ) == STL_SUCCESS );

    /**
     * DDL Pre-Rollback Action 을 수행
     */

    STL_TRY( elgExecStmtPreRollbackAction( aTransID,
                                           STL_FALSE,  /* aIsSuccess */
                                           aEnv )
             == STL_SUCCESS );
    
    STL_TRY( elgExecPreActionTransEnd( aTransID,
                                       ELG_PEND_ACT_ROLLBACK,
                                       aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}



/**
 * @brief FATAL 상황시 진행중이던 DDL 에 대해 ROLLBACK 수행후 Pending Operation 을 수행한다.
 * @param[in]   aTransID   Transaction ID
 * @param[in]   aEnv       Environment
 * @warning
 * Clean Up 과정중에 heap memory 에 대한 alloc/free 가 존재하면 안됨.
 * - 즉, aEnv 나 aSessionEnv 의 memory manager 를 사용하면 안됨.
 */

stlStatus ellCleanupAfterRollbackTransDDL( smlTransId     aTransID,
                                           ellEnv       * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);
    
    /**
     * DDL Transaction Pending Rollback 을 수행
     */
    
    STL_TRY( elgExecTransPendOP( aTransID, ELG_PEND_ACT_ROLLBACK, aEnv ) == STL_SUCCESS );

    /**
     * DDL 이 없음을 설정 
     */
    
    sSessEnv->mIncludeDDL = STL_FALSE;
    
    return STL_SUCCESS;

    STL_FINISH;

    sSessEnv->mIncludeDDL = STL_FALSE;
    
    return STL_FAILURE;
}


/** @} ellTransDDL */
