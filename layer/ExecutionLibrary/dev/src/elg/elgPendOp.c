/*******************************************************************************
 * elgPendOp.c
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
 * @file elgPendOp.c
 * @brief Execution Library Layer Pending Operation
 */


#include <ell.h>
#include <eldt.h>
#include <eldc.h>

#include <eldDictionary.h>
#include <elgPendOp.h>

/**
 * @addtogroup elgPendOp
 * @{
 */

elgPendOpFunc  gElgPendOpFunc[ELG_PEND_MAX];


void elgSetPendOp( elgPendOpID    aPendOpID,
                   elgPendOpFunc  aPendFunc )
{
    gElgPendOpFunc[aPendOpID] = aPendFunc;
}

/**
 * @brief Pending Operation 들을 설정한다.
 * @remarks
 */
void elgRegisterPendOperations()
{
    elgSetPendOp(ELG_PRE_ROLLBACK_SET_FLAG, eldcPreRollbackSetFlag);
    
    elgSetPendOp(ELG_PEND_FREE_DICT_MEM, eldcPendRollbackFreeDictMem);
    
    elgSetPendOp(ELG_PEND_ROLLBACK_INSERT_HASH_ELEMENT, eldcPendRollbackInsertHashElement );
    elgSetPendOp(ELG_PEND_ROLLBACK_DELETE_HASH_ELEMENT, eldcPendRollbackDeleteHashElement );

    elgSetPendOp(ELG_PEND_ROLLBACK_ADD_HASH_RELATED, eldcPendRollbackAddHashRelated );
    elgSetPendOp(ELG_PEND_ROLLBACK_DEL_HASH_RELATED, eldcPendRollbackDelHashRelated );
    
    elgSetPendOp(ELG_PEND_COMMIT_INSERT_HASH_ELEMENT, eldcPendCommitInsertHashElement );
    elgSetPendOp(ELG_PEND_COMMIT_DELETE_HASH_ELEMENT, eldcPendCommitDeleteHashElement );
    
    elgSetPendOp(ELG_PEND_COMMIT_ADD_HASH_RELATED, eldcPendCommitAddHashRelated );
    elgSetPendOp(ELG_PEND_COMMIT_DEL_HASH_RELATED, eldcPendCommitDelHashRelated );
}


/**
 * @brief Pre-Rollback 시 수행할 Pending Operation 을 등록한다.
 * @param[in]  aPendOpID       Pending Operation ID
 * @param[in]  aPendOP         Pending Operation Buffer
 * @param[in]  aArgs           Pending Function Arguments
 * @param[in]  aArgsByteSize   aArgs 의 Byte 크기
 * @param[in]  aEnv            Environment
 * @remarks
 */
void elgAddPreRollbackAction( elgPendOpID       aPendOpID,
                              elgPendOP       * aPendOP,
                              void            * aArgs,
                              stlUInt32         aArgsByteSize,
                              ellEnv          * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);

    /**
     * Parameter Validation
     */

    STL_DASSERT( (aPendOpID > ELG_PEND_NA) && (aPendOpID < ELG_PEND_MAX) );
    STL_DASSERT( aPendOP != NULL );
    STL_DASSERT( aArgs != NULL );
    STL_DASSERT( aArgsByteSize > 0 );

    /**
     * Pending Operation 초기화 
     */

    aPendOP->mPendOpID = aPendOpID;
    aPendOP->mStmtTimestamp = sSessEnv->mStmtTimestamp;
    aPendOP->mActionType = ELG_PEND_ACT_ROLLBACK;
    aPendOP->mArgs = (void *) ( (stlChar *)aPendOP + STL_ALIGN_DEFAULT(STL_SIZEOF(elgPendOP)) );
    stlMemcpy( aPendOP->mArgs, aArgs, aArgsByteSize );
    aPendOP->mAgingEventBuffer = NULL;
    aPendOP->mNext = NULL;

    /**
     * Pre- Rollback Pending Operation 연결
     * 연산 순서의 역순으로 수행될 수 있도록 앞에 연결한다.
     */

    if ( sSessEnv->mStmtPreRollbackActionList == NULL )
    {
        sSessEnv->mStmtPreRollbackActionList = (void *) aPendOP;
    }
    else
    {
        aPendOP->mNext = (elgPendOP *) sSessEnv->mStmtPreRollbackActionList;
        sSessEnv->mStmtPreRollbackActionList = (void *) aPendOP;
    }
}


/**
 * @brief Statement Rollback 시 수행할 Pending Operation 을 등록한다.
 * @param[in]  aPendOpID       Pending Operation ID
 * @param[in]  aPendOP         Pending Operation Buffer
 * @param[in]  aArgs           Pending Function Arguments
 * @param[in]  aArgsByteSize   aArgs 의 Byte 크기
 * @param[in]  aEnv            Environment
 * @remarks
 * - Statement Rollback 시에 수행될 Pending Action 을 등록한다.
 *   연산 순서와 반대로 수행할 수 있도록 앞에 연결한다.
 * - Statement 가 정상 수행될 경우, Transaction Rollback Pending List 로 이동한다.
 */
void elgAddStmtRollbackPendOP( elgPendOpID       aPendOpID,
                               elgPendOP       * aPendOP,
                               void            * aArgs,
                               stlUInt32         aArgsByteSize,
                               ellEnv          * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);

    /**
     * Parameter Validation
     */

    STL_DASSERT( (aPendOpID > ELG_PEND_NA) && (aPendOpID < ELG_PEND_MAX) );
    STL_DASSERT( aPendOP != NULL );
    STL_DASSERT( aArgs != NULL );
    STL_DASSERT( aArgsByteSize > 0 );

    /**
     * Pending Operation 초기화 
     */

    aPendOP->mPendOpID = aPendOpID;
    aPendOP->mStmtTimestamp = sSessEnv->mStmtTimestamp;
    aPendOP->mActionType = ELG_PEND_ACT_ROLLBACK;
    aPendOP->mArgs = (void *) ( (stlChar *)aPendOP + STL_ALIGN_DEFAULT(STL_SIZEOF(elgPendOP)) );
    stlMemcpy( aPendOP->mArgs, aArgs, aArgsByteSize );
    aPendOP->mAgingEventBuffer = NULL;
    aPendOP->mNext = NULL;

    /**
     * Statement Rollback Pending Operation 연결
     * 연산 순서의 역순으로 수행될 수 있도록 앞에 연결한다.
     */

    if ( sSessEnv->mStmtRollbackPendOpList == NULL )
    {
        sSessEnv->mStmtRollbackPendOpList = (void *) aPendOP;
    }
    else
    {
        aPendOP->mNext = (elgPendOP *) sSessEnv->mStmtRollbackPendOpList;
        sSessEnv->mStmtRollbackPendOpList = (void *) aPendOP;
    }
}



/**
 * @brief Statement 정상 종료시 수행할 Pending Operation 을 등록한다.
 * @param[in]  aPendOpID          Pending Operation ID
 * @param[in]  aPendOP            Pending Operation Buffer
 * @param[in]  aArgs              Pending Function Arguments
 * @param[in]  aArgsByteSize      aArgs 의 Byte 크기
 * @param[in]  aAgingEventBuffer  Aging Event 를 위한 Buffer 공간 (nullable)
 * @param[in]  aEnv               Environment
 * @remarks
 * 수행시점 
 * - Statement Rollback 시에는 수행되지 않음.
 * - Statement 정상 종료 후에 Transaction Commit Pending Operation 에 연결됨
 * - 연산 순서와 동일하게 수행할 수 있도록 맨뒤에 연결한다.
 */
void elgAddStmtSuccessPendOP( elgPendOpID       aPendOpID,
                              elgPendOP       * aPendOP,
                              void            * aArgs,
                              stlUInt32         aArgsByteSize,
                              void            * aAgingEventBuffer,
                              ellEnv          * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);
    elgPendOP     * sLastPendOp = NULL;

    /**
     * Parameter Validation
     */

    STL_DASSERT( (aPendOpID > ELG_PEND_NA) && (aPendOpID < ELG_PEND_MAX) );
    STL_DASSERT( aPendOP != NULL );
    STL_DASSERT( aArgs != NULL );
    STL_DASSERT( aArgsByteSize > 0 );

    /**
     * Pending Operation 초기화 
     */
    
    aPendOP->mPendOpID = aPendOpID;
    aPendOP->mStmtTimestamp = sSessEnv->mStmtTimestamp;
    aPendOP->mActionType = ELG_PEND_ACT_COMMIT;
    aPendOP->mArgs = (void *) ( (stlChar *)aPendOP + STL_ALIGN_DEFAULT(STL_SIZEOF(elgPendOP)) );
    stlMemcpy( aPendOP->mArgs, aArgs, aArgsByteSize );
    aPendOP->mAgingEventBuffer = aAgingEventBuffer;
    aPendOP->mNext = NULL;

    /**
     * Statement Success Pending Operation 연결
     * 연산 순서의 순방향으로 수행될 수 있도록 맨뒤에 연결한다.
     */
    
    if ( sSessEnv->mStmtSuccessPendOpList == NULL )
    {
        sSessEnv->mStmtSuccessPendOpList = (void *) aPendOP;
    }
    else
    {
        sLastPendOp = sSessEnv->mStmtSuccessPendOpList;
        while ( sLastPendOp->mNext != NULL )
        {
            sLastPendOp = sLastPendOp->mNext;
        }

        sLastPendOp->mNext = aPendOP;
    }
}



/**
 * @brief Statement Rollback 발생시 Cache 메모리 반납을 위한 Pending Operation 을 등록한다. 
 * @param[in]  aPendOP            Pending Operation Buffer
 * @param[in]  aArgs              Pending Function Arguments
 * @param[in]  aArgsByteSize      aArgs 의 Byte 크기
 * @param[in]  aAgingEventBuffer  Aging Event 를 위한 Buffer 공간 
 * @param[in]  aEnv               Environment
 * @remarks
 * - Statement/Transaction Rollback 시 반납할 Cache 메모리를 등록한다.
 * - cf) Transaction Commit 시에 회수될 Cache 메모리는
 *       Commit Pending Operation 에 의해 Ager 에 등록됨.
 * - cf) Statement/Transaction Rollback 시에 회수될 Cache 메모리는
 *       Rollback Pending Operation 에 의해 해제됨
 * - 연산 순서의 역순으로 수행될 수 있도록 앞에 연결한다.
 */
void elgAddStmtRollbackFreeCacheMemPendOP( elgPendOP       * aPendOP,
                                           void            * aArgs,
                                           stlUInt32         aArgsByteSize,
                                           void            * aAgingEventBuffer,
                                           ellEnv          * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);
    
    /**
     * Parameter Validation
     */

    STL_DASSERT( aPendOP != NULL );
    STL_DASSERT( aArgs != NULL );
    STL_DASSERT( aArgsByteSize > 0 );
    STL_DASSERT( aAgingEventBuffer != NULL );

    /**
     * Pending Operation 초기화 
     */
    
    aPendOP->mPendOpID = ELG_PEND_FREE_DICT_MEM;
    aPendOP->mStmtTimestamp = sSessEnv->mStmtTimestamp;
    aPendOP->mActionType = ELG_PEND_ACT_ROLLBACK;
    aPendOP->mArgs = (void *) ( (stlChar *)aPendOP + STL_ALIGN_DEFAULT(STL_SIZEOF(elgPendOP)) );
    stlMemcpy( aPendOP->mArgs, aArgs, aArgsByteSize );
    aPendOP->mAgingEventBuffer = aAgingEventBuffer;
    aPendOP->mNext = NULL;

    /**
     * Statement Rollback Pending Operation 연결
     * 연산 순서의 역순으로 수행될 수 있도록 앞에 연결한다.
     */
    
    if ( sSessEnv->mStmtRollbackPendOpList == NULL )
    {
        sSessEnv->mStmtRollbackPendOpList = (void *) aPendOP;
    }
    else
    {
        aPendOP->mNext = (elgPendOP *) sSessEnv->mStmtRollbackPendOpList;
        sSessEnv->mStmtRollbackPendOpList = (void *) aPendOP;
    }
}


/**
 * @brief Statement 의 ROLLBACK Pending Action 을 수행한다.
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aEnv            Environment
 * @remarks
 */
stlStatus elgExecStmtRollbackPendOP( smlTransId     aTransID,
                                     ellEnv       * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);
    
    elgPendOP   * sRollbackPendOP = NULL;
    elgPendOP   * sNextRollbackPendOP = NULL;

    elgPendOP   * sSuccessPendOP = NULL;
    elgPendOP   * sNextSuccessPendOP = NULL;
    
    KNL_ENTER_CRITICAL_SECTION( aEnv );
    
    /**
     * Parameter Validation
     */

    /**
     * 기본 정보 획득
     */

    sRollbackPendOP = (elgPendOP *) sSessEnv->mStmtRollbackPendOpList;
    sSuccessPendOP  = (elgPendOP *) sSessEnv->mStmtSuccessPendOpList;

    /**
     * Statement Rollback Pending Operation 들을 수행
     */
    
    while( sRollbackPendOP != NULL )
    {
        sNextRollbackPendOP = sRollbackPendOP->mNext;
        
        STL_TRY( gElgPendOpFunc[sRollbackPendOP->mPendOpID]( aTransID,
                                                             sRollbackPendOP->mArgs,
                                                             sRollbackPendOP->mAgingEventBuffer,
                                                             aEnv )
                 == STL_SUCCESS );
        
        STL_TRY( eldClearPendingMem( sRollbackPendOP, aEnv ) == STL_SUCCESS );
        sRollbackPendOP = sNextRollbackPendOP;
    }

    /**
     * Statement Commit Pending Operation 들은 수행하지 않고 메모리 반납만 한다.
     */
    
    while( sSuccessPendOP != NULL )
    {
        sNextSuccessPendOP = sSuccessPendOP->mNext;
        if ( sSuccessPendOP->mAgingEventBuffer != NULL )
        {
            /* STL_TRY( knlLogMsg( NULL, */
            /*                     KNL_ENV(aEnv), */
            /*                     KNL_LOG_LEVEL_INFO, */
            /*                     "[EL Stmt-Rollback] [Free Event Mem]  %p", */
            /*                     sSuccessPendOP->mAgingEventBuffer ) */
            /*          == STL_SUCCESS ); */
            
            STL_TRY( knlFreeEnvEventMem( sSuccessPendOP->mAgingEventBuffer, KNL_ENV(aEnv) ) == STL_SUCCESS );
            sSuccessPendOP->mAgingEventBuffer = NULL;
        }
        STL_TRY( eldClearPendingMem( sSuccessPendOP, aEnv ) == STL_SUCCESS );
        sSuccessPendOP = sNextSuccessPendOP;
    }

    
    /**
     * Statement Pending Operation List 초기화 
     */
    
    sSessEnv->mStmtRollbackPendOpList = NULL;
    sSessEnv->mStmtSuccessPendOpList = NULL;

    KNL_LEAVE_CRITICAL_SECTION( aEnv );
    
    return STL_SUCCESS;

    STL_FINISH;

    /**
     * 실패가 발생하더라도 Pending Operation들을 모두 수행한다.
     */

    if ( sNextRollbackPendOP != NULL )
    {
        sRollbackPendOP = sNextRollbackPendOP;
        while( sRollbackPendOP != NULL )
        {
            sNextRollbackPendOP = sRollbackPendOP->mNext;
            (void) gElgPendOpFunc[sRollbackPendOP->mPendOpID]( aTransID,
                                                               sRollbackPendOP->mArgs,
                                                               sRollbackPendOP->mAgingEventBuffer,
                                                               aEnv );
            
            (void) eldClearPendingMem( sRollbackPendOP, aEnv );
            sRollbackPendOP = sNextRollbackPendOP;
        }
    }

    if ( sNextSuccessPendOP != NULL )
    {
        sSuccessPendOP = sNextSuccessPendOP;
        while( sSuccessPendOP != NULL )
        {
            sNextSuccessPendOP = sSuccessPendOP->mNext;
            if ( sSuccessPendOP->mAgingEventBuffer != NULL )
            {
                /* STL_TRY( knlLogMsg( NULL, */
                /*                     KNL_ENV(aEnv), */
                /*                     KNL_LOG_LEVEL_INFO, */
                /*                     "[EL Stmt-Rollback Error] [Free Event Mem]  %p", */
                /*                     sSuccessPendOP->mAgingEventBuffer ) */
                /*          == STL_SUCCESS ); */
                
                (void) knlFreeEnvEventMem( sSuccessPendOP->mAgingEventBuffer, KNL_ENV(aEnv) );
                sSuccessPendOP->mAgingEventBuffer = NULL;
            }
            (void) eldClearPendingMem( sSuccessPendOP, aEnv );
            sSuccessPendOP = sSuccessPendOP->mNext;
        }
    }

    sSessEnv->mStmtRollbackPendOpList = NULL;
    sSessEnv->mStmtSuccessPendOpList = NULL;

    KNL_LEAVE_CRITICAL_SECTION( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief DDL Statement의 정상적 종료에 대한 Pending Operation 을 수행한다.
 * @param[in] aEnv       Environment
 * @remarks
 * Statement 가 정상 종료하므로
 * - Stmt Rollback Pending 은 Transaction Rollback Pending 으로 연결 
 *  - 연산 순서의 역순으로 수행될 수 있도록 맨앞에 연결한다.
 * - Stmt Commit Pending 은 Transaction Commit Pending 으로 연결
 *  - 연산 순서의 순방향으로 수행될 수 있도록 맨뒤에 연결한다.
 */
void elgExecStmtSuccessPendOP( ellEnv * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);
    elgPendOP     * sPendOP = NULL;

    KNL_ENTER_CRITICAL_SECTION( aEnv );
    
    /**
     * Statement Rollback Pending Operation 들을
     * Transaction Rollback을 위한 Pending Operation 으로 변경
     * - 연산 순서의 역순으로 수행될 수 있도록 맨앞에 연결 
     */

    if ( sSessEnv->mTxRollbackPendOpList == NULL )
    {
        sSessEnv->mTxRollbackPendOpList = sSessEnv->mStmtRollbackPendOpList;
    }
    else
    {
        if ( sSessEnv->mStmtRollbackPendOpList == NULL )
        {
            /*
             * nothing to do
             */
        }
        else
        {
            sPendOP = (elgPendOP *) sSessEnv->mStmtRollbackPendOpList;
            
            while ( sPendOP->mNext != NULL )
            {
                sPendOP = sPendOP->mNext;
            }
            
            sPendOP->mNext = (elgPendOP *) sSessEnv->mTxRollbackPendOpList;
            sSessEnv->mTxRollbackPendOpList = (void *) sSessEnv->mStmtRollbackPendOpList;
        }
    }

    /**
     * Statement Commit Pending Operation 들을
     * Transaction Rollback을 위한 Pending Operation 으로 변경
     * - 연산 순서의 순방향으로 수행될 수 있도록 맨뒤에 연결 
     */

    if ( sSessEnv->mTxCommitPendOpList == NULL )
    {
        sSessEnv->mTxCommitPendOpList = sSessEnv->mStmtSuccessPendOpList;
    }
    else
    {
        sPendOP = (elgPendOP *) sSessEnv->mTxCommitPendOpList;
    
        while ( sPendOP->mNext != NULL )
        {
            sPendOP = sPendOP->mNext;
        }

        sPendOP->mNext = sSessEnv->mStmtSuccessPendOpList;
    }
    
    /**
     * Statement Pending List 를 초기화
     */

    sSessEnv->mStmtRollbackPendOpList = NULL;
    sSessEnv->mStmtSuccessPendOpList = NULL;

    KNL_LEAVE_CRITICAL_SECTION( aEnv );
}




/**
 * @brief Transaction 종료 직전 수행할 Pending Action 을 수행한다.
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aActionType     COMMIT or ROLLABCK
 * @param[in]  aEnv            Environment
 * @remarks
 */
stlStatus elgExecPreActionTransEnd( smlTransId       aTransID,
                                    elgPendActType   aActionType,
                                    ellEnv         * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);

    elgPendOP   * sPendAction = NULL;
    elgPendOP   * sNextAction = NULL;

    KNL_ENTER_CRITICAL_SECTION( aEnv );
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( (aActionType == ELG_PEND_ACT_COMMIT) ||
                        (aActionType == ELG_PEND_ACT_ROLLBACK),
                        ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sPendAction  = (elgPendOP *) sSessEnv->mTxPreRollbackActionList;
    
    if ( aActionType == ELG_PEND_ACT_COMMIT )
    {
        /**
         * Pre-Rollback Action 메모리를 모두 제거
         */
        
        while( sPendAction != NULL )
        {
            sNextAction = sPendAction->mNext;
            
            /* Pend OP 의 Aging Event 메모리를 해제한다.*/
            if ( sPendAction->mAgingEventBuffer != NULL )
            {
                STL_TRY( knlFreeEnvEventMem( sPendAction->mAgingEventBuffer, KNL_ENV(aEnv) ) == STL_SUCCESS );
            }
            
            sPendAction->mAgingEventBuffer = NULL;
            
            STL_TRY( eldClearPendingMem( sPendAction, aEnv ) == STL_SUCCESS );
            sPendAction = sNextAction;
        }
    }
    else
    {
        /**
         * Pre-Rollback Action 메모리를 모두 제거
         */
        
        while( sPendAction != NULL )
        {
            sNextAction = sPendAction->mNext;
            
            STL_TRY( gElgPendOpFunc[sPendAction->mPendOpID]( aTransID,
                                                             sPendAction->mArgs,
                                                             sPendAction->mAgingEventBuffer,
                                                             aEnv )
                     == STL_SUCCESS );
            
            STL_TRY( eldClearPendingMem( sPendAction, aEnv ) == STL_SUCCESS );
            sPendAction = sNextAction;
        }
    }
    
    /**
     * Transaction Pre-Rollback Action List 초기화
     */

    sSessEnv->mTxPreRollbackActionList = NULL;    

    KNL_LEAVE_CRITICAL_SECTION( aEnv );
    
    return STL_SUCCESS;

    STL_FINISH;

    /**
     * Transaction 종료는 실패하지 않아야 함
     */

    KNL_ASSERT( STL_FALSE, aEnv, ("elgExecPreActionTransEnd()") );
    
    return STL_FAILURE;
}


/**
 * @brief Stmt 종료 직전 수행할 Pending Action 을 수행한다.
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aIsSuccess      Statement의 성공여부
 * @param[in]  aEnv            Environment
 * @remarks
 */
stlStatus elgExecStmtPreRollbackAction( smlTransId   aTransID,
                                        stlBool      aIsSuccess,
                                        ellEnv     * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);

    elgPendOP   * sPendAction = NULL;
    elgPendOP   * sNextAction = NULL;

    KNL_ENTER_CRITICAL_SECTION( aEnv );
    
    /**
     * 기본 정보 획득
     */

    if( aIsSuccess == STL_TRUE )
    {
        if ( sSessEnv->mTxPreRollbackActionList != NULL )
        {
            sPendAction = (elgPendOP *) sSessEnv->mStmtPreRollbackActionList;
            
            if( sPendAction != NULL )
            {
                while( sPendAction->mNext != NULL )
                {
                    sPendAction = sPendAction->mNext;
                }

                sPendAction->mNext = sSessEnv->mTxPreRollbackActionList;
                sSessEnv->mTxPreRollbackActionList = sSessEnv->mStmtPreRollbackActionList;
            }
        }
        else
        {
            sSessEnv->mTxPreRollbackActionList = sSessEnv->mStmtPreRollbackActionList;
        }
    }
    else
    {
        sPendAction = (elgPendOP *) sSessEnv->mStmtPreRollbackActionList;
        
        while( sPendAction != NULL )
        {
            sNextAction = sPendAction->mNext;
        
            STL_TRY( gElgPendOpFunc[sPendAction->mPendOpID]( aTransID,
                                                             sPendAction->mArgs,
                                                             sPendAction->mAgingEventBuffer,
                                                             aEnv )
                     == STL_SUCCESS );
        
            STL_TRY( eldClearPendingMem( sPendAction, aEnv ) == STL_SUCCESS );
            sPendAction = sNextAction;
        }
    }

    sSessEnv->mStmtPreRollbackActionList = NULL;

    KNL_LEAVE_CRITICAL_SECTION( aEnv );
    
    return STL_SUCCESS;

    STL_FINISH;

    while( sPendAction != NULL )
    {
        sNextAction = sPendAction->mNext;
        
        (void) gElgPendOpFunc[sPendAction->mPendOpID]( aTransID,
                                                       sPendAction->mArgs,
                                                       sPendAction->mAgingEventBuffer,
                                                       aEnv );
        
        (void) eldClearPendingMem( sPendAction, aEnv );
        sPendAction = sNextAction;
    }
    
    sSessEnv->mStmtPreRollbackActionList = NULL;

    KNL_LEAVE_CRITICAL_SECTION( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief Transaction 종료를 위한 Pending Action 을 수행한다.
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aActionType     COMMIT or ROLLABCK
 * @param[in]  aEnv            Environment
 * @remarks
 */
stlStatus elgExecTransPendOP( smlTransId       aTransID,
                              elgPendActType   aActionType,
                              ellEnv         * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);

    elgPendOP   * sRunPendOP = NULL;
    elgPendOP   * sNextRunPendOP = NULL;

    elgPendOP   * sSkipPendOP = NULL;
    elgPendOP   * sNextSkipPendOP = NULL;

    KNL_ENTER_CRITICAL_SECTION( aEnv );
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( (aActionType == ELG_PEND_ACT_COMMIT) ||
                        (aActionType == ELG_PEND_ACT_ROLLBACK),
                        ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    if ( aActionType == ELG_PEND_ACT_COMMIT )
    {
        /**
         * Commit Pending Operation 을 수행
         */
        
        sRunPendOP  = (elgPendOP *) sSessEnv->mTxCommitPendOpList;
        sSkipPendOP = (elgPendOP *) sSessEnv->mTxRollbackPendOpList;
    }
    else
    {
        /**
         * Rollback Pending Operation 을 수행
         */
        
        sRunPendOP  = (elgPendOP *) sSessEnv->mTxRollbackPendOpList;
        sSkipPendOP = (elgPendOP *) sSessEnv->mTxCommitPendOpList;
    }
    
    /**
     * Run(commit or rollback) Pending Operation 들을 수행
     */
    
    while( sRunPendOP != NULL )
    {
        STL_DASSERT( aActionType == sRunPendOP->mActionType );
        
        sNextRunPendOP = sRunPendOP->mNext;
        
        STL_TRY( gElgPendOpFunc[sRunPendOP->mPendOpID]( aTransID,
                                                        sRunPendOP->mArgs,
                                                        sRunPendOP->mAgingEventBuffer,
                                                        aEnv )
                 == STL_SUCCESS );

        STL_TRY( eldClearPendingMem( sRunPendOP, aEnv ) == STL_SUCCESS );
        
        sRunPendOP = sNextRunPendOP;
    }

    /**
     * Skip Pending Operation 들을 제거 
     */
    
    while( sSkipPendOP != NULL )
    {
        STL_DASSERT( aActionType != sSkipPendOP->mActionType );

        sNextSkipPendOP = sSkipPendOP->mNext;

        /* Pend OP 의 Aging Event 메모리를 해제한다.*/
        if ( sSkipPendOP->mAgingEventBuffer != NULL )
        {
            /* STL_TRY( knlLogMsg( NULL, */
            /*                     KNL_ENV(aEnv), */
            /*                     KNL_LOG_LEVEL_INFO, */
            /*                     "[EL COMMIT] [Free Event Mem]  %p", */
            /*                     sSkipPendOP->mAgingEventBuffer ) */
            /*          == STL_SUCCESS ); */
            
            STL_TRY( knlFreeEnvEventMem( sSkipPendOP->mAgingEventBuffer, KNL_ENV(aEnv) ) == STL_SUCCESS );
        }
            
        sSkipPendOP->mAgingEventBuffer = NULL;
        
        STL_TRY( eldClearPendingMem( sSkipPendOP, aEnv ) == STL_SUCCESS );
        sSkipPendOP = sNextSkipPendOP;
    }

    /**
     * Transaction Pending Operation List 초기화
     */
    
    sSessEnv->mTxRollbackPendOpList = NULL;
    sSessEnv->mTxCommitPendOpList = NULL;

    KNL_LEAVE_CRITICAL_SECTION( aEnv );
    
    return STL_SUCCESS;

    STL_FINISH;

    /**
     * Transaction 종료는 실패하지 않아야 함
     */

    KNL_ASSERT( STL_FALSE, aEnv, ("elgExecTransPendOP()") );
    
    return STL_FAILURE;
}


stlStatus elgExecRollbackToSavepointPendOP( smlTransId       aTransID,
                                            stlInt64         aSavepointTimestamp,
                                            ellEnv         * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);

    elgPendOP   * sRollbackPendOP = NULL;
    elgPendOP   * sNextRollbackPendOP = NULL;
    elgPendOP   * sLastRollbackPendOP = NULL;

    elgPendOP   * sCommitPendOP = NULL;
    elgPendOP   * sNextCommitPendOP = NULL;
    elgPendOP   * sLastCommitPendOP = NULL;

    elgPendOP   * sPendAction = NULL;
    elgPendOP   * sNextAction = NULL;

    KNL_ENTER_CRITICAL_SECTION( aEnv );
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSavepointTimestamp >= 0, ELL_ERROR_STACK(aEnv) );
    
    /**
     * 기본 정보 획득
     */
      
    sRollbackPendOP = (elgPendOP *) sSessEnv->mTxRollbackPendOpList;
    sCommitPendOP   = (elgPendOP *) sSessEnv->mTxCommitPendOpList;
    sPendAction = (elgPendOP *) sSessEnv->mTxPreRollbackActionList;
        
    /**
     * Savepoint 이후의 Pre-Rollback Action 을 수행한다.
     * - Pre-Rollback Action 은 시간 역순으로 정렬되어 있다.
     */

    while( sPendAction != NULL )
    {
        sNextAction = sPendAction->mNext;
        
        if ( sPendAction->mStmtTimestamp >= aSavepointTimestamp )
        {
            STL_TRY( gElgPendOpFunc[sPendAction->mPendOpID]( aTransID,
                                                             sPendAction->mArgs,
                                                             sPendAction->mAgingEventBuffer,
                                                             aEnv )
                     == STL_SUCCESS );
        
            STL_TRY( eldClearPendingMem( sPendAction, aEnv ) == STL_SUCCESS );
        }
        else
        {
            break;
        }
        
        sPendAction = sNextAction;
    }

    sSessEnv->mTxPreRollbackActionList = sPendAction;
    
    /**
     * Savepoint 이후의 Commit Pending Operation 을 제거한다.
     * - Commit Pending Operation 은 시간 순서대로 정렬되어 있다.
     */

    sLastCommitPendOP = NULL;
    while( sCommitPendOP != NULL )
    {
        STL_DASSERT( sCommitPendOP->mActionType == ELG_PEND_ACT_COMMIT );
        
        if ( sCommitPendOP->mStmtTimestamp >= aSavepointTimestamp )
        {
            /**
             * 여기서부터 제거해야 함.
             */
            break;
        }
        else
        {
            sLastCommitPendOP = sCommitPendOP;
        }

        sCommitPendOP = sCommitPendOP->mNext;
    }
    
    while( sCommitPendOP != NULL )
    {
        STL_DASSERT( sCommitPendOP->mActionType == ELG_PEND_ACT_COMMIT );
        STL_DASSERT( sCommitPendOP->mStmtTimestamp >= aSavepointTimestamp );

        sNextCommitPendOP = sCommitPendOP->mNext;
        
        /* Commit Pend OP 의 Aging Event 메모리를 해제한다.*/
        if ( sCommitPendOP->mAgingEventBuffer != NULL )
        {
            /* STL_TRY( knlLogMsg( NULL, */
            /*                     KNL_ENV(aEnv), */
            /*                     KNL_LOG_LEVEL_INFO, */
            /*                     "[EL Rollback-To-Savepoint] [Free Event Mem]  %p", */
            /*                     sCommitPendOP->mAgingEventBuffer ) */
            /*          == STL_SUCCESS ); */
            
            STL_TRY( knlFreeEnvEventMem( sCommitPendOP->mAgingEventBuffer, KNL_ENV(aEnv) ) == STL_SUCCESS );
        }
        sCommitPendOP->mAgingEventBuffer = NULL;
        
        STL_TRY( eldClearPendingMem( sCommitPendOP, aEnv ) == STL_SUCCESS );
        
        sCommitPendOP = sNextCommitPendOP;
    }
    
    
    /**
     * Savepoint 이전의 Rollback Pending Operation 을 수행한다.
     * - Rollback Pending Operation 은 시간 역순으로 정렬되어 있다.
     */

    sLastRollbackPendOP = NULL;
    while( sRollbackPendOP != NULL )
    {
        STL_DASSERT( sRollbackPendOP->mActionType == ELG_PEND_ACT_ROLLBACK );
        
        if ( sRollbackPendOP->mStmtTimestamp >= aSavepointTimestamp )
        {
            sNextRollbackPendOP = sRollbackPendOP->mNext;
            STL_TRY( gElgPendOpFunc[sRollbackPendOP->mPendOpID]( aTransID,
                                                                 sRollbackPendOP->mArgs,
                                                                 sRollbackPendOP->mAgingEventBuffer,
                                                                 aEnv )
                     == STL_SUCCESS );

            /* rollback pend op 에는 aging event buffer 가 없다 */
            
            STL_TRY( eldClearPendingMem( sRollbackPendOP, aEnv ) == STL_SUCCESS );
            sRollbackPendOP = sNextRollbackPendOP;
        }
        else
        {
            /**
             * 여기부터 남겨두어야 함.
             */
            sLastRollbackPendOP = sRollbackPendOP;
            break;
        }
    }
    
    /**
     * Transaction Pending Operation List 정보 변경
     */
    
    sSessEnv->mTxRollbackPendOpList = sLastRollbackPendOP;

    if ( sLastCommitPendOP == NULL )
    {
        sSessEnv->mTxCommitPendOpList = NULL;
    }
    else
    {
        sLastCommitPendOP->mNext = NULL;
    }

    KNL_LEAVE_CRITICAL_SECTION( aEnv );
    
    return STL_SUCCESS;

    STL_FINISH;

    /**
     * 에러가 발생할 경우 복구할 방법이 없다.
     */

    KNL_ASSERT( STL_FALSE, aEnv, ("elgExecRollbackToSavepointPendOP()") );
    
    return STL_FAILURE;
}



/**
 * @brief OPEN 단계가 아닌 start-up 단계에서 발생한 Pending Operation 은 수행 대상이 아님
 * @param[in]  aEnv  Environment
 */
stlStatus elgResetSessionPendOp4NotOpenPhase( ellEnv * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);

    elgPendOP * sPendOP = NULL;
    elgPendOP * sNextPendOP = NULL;

    /**
     * Fixed Table Cache 를 위해 사용된 Pending Memory 를 해제한다.
     */

    sPendOP  = (elgPendOP *) sSessEnv->mTxCommitPendOpList;
    while( sPendOP != NULL )
    {
        sNextPendOP = sPendOP->mNext;
        
        STL_TRY( eldClearPendingMem( sPendOP, aEnv ) == STL_SUCCESS );
        
        sPendOP = sNextPendOP;
    }
    sSessEnv->mTxCommitPendOpList = NULL;
    
    sPendOP  = (elgPendOP *) sSessEnv->mTxRollbackPendOpList;
    while( sPendOP != NULL )
    {
        sNextPendOP = sPendOP->mNext;
        
        STL_TRY( eldClearPendingMem( sPendOP, aEnv ) == STL_SUCCESS );
        
        sPendOP = sNextPendOP;
    }
    sSessEnv->mTxRollbackPendOpList = NULL;

    sPendOP  = (elgPendOP *) sSessEnv->mTxPreRollbackActionList;
    while( sPendOP != NULL )
    {
        sNextPendOP = sPendOP->mNext;
        
        STL_TRY( eldClearPendingMem( sPendOP, aEnv ) == STL_SUCCESS );
        
        sPendOP = sNextPendOP;
    }
    sSessEnv->mTxPreRollbackActionList = NULL;
    
    sPendOP  = (elgPendOP *) sSessEnv->mStmtSuccessPendOpList;
    while( sPendOP != NULL )
    {
        sNextPendOP = sPendOP->mNext;
        
        STL_TRY( eldClearPendingMem( sPendOP, aEnv ) == STL_SUCCESS );
        
        sPendOP = sNextPendOP;
    }
    sSessEnv->mStmtSuccessPendOpList = NULL;

    sPendOP  = (elgPendOP *) sSessEnv->mStmtRollbackPendOpList;
    while( sPendOP != NULL )
    {
        sNextPendOP = sPendOP->mNext;
        
        STL_TRY( eldClearPendingMem( sPendOP, aEnv ) == STL_SUCCESS );
        
        sPendOP = sNextPendOP;
    }
    sSessEnv->mStmtRollbackPendOpList = NULL;
    
    sPendOP  = (elgPendOP *) sSessEnv->mStmtPreRollbackActionList;
    while( sPendOP != NULL )
    {
        sNextPendOP = sPendOP->mNext;
        
        STL_TRY( eldClearPendingMem( sPendOP, aEnv ) == STL_SUCCESS );
        
        sPendOP = sNextPendOP;
    }
    sSessEnv->mStmtPreRollbackActionList = NULL;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} elgPendOp */
