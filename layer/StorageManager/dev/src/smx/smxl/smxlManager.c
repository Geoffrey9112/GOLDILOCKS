/*******************************************************************************
 * smxlManger.c
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

#include <stl.h>
#include <dtl.h>
#include <knl.h>
#include <scl.h>
#include <smlDef.h>
#include <smDef.h>
#include <smxlDef.h>
#include <smxl.h>
#include <smrDef.h>
#include <smr.h>
#include <smxm.h>
#include <smf.h>
#include <sma.h>
#include <smg.h>
#include <smd.h>
#include <sms.h>
#include <smkl.h>
#include <smxlUndoRelMgr.h>
#include <smxlScnMgr.h>
#include <smklDef.h>
#include <smlGeneral.h>

/**
 * @file smxlManager.c
 * @brief Storage Manager Layer Local Transaction Manager Internal Routines
 */

extern stlInt64             gDataStoreMode;
extern smxlWarmupEntry    * gSmxlWarmupEntry;

/**
 * @addtogroup smxl
 * @{
 */

/**
 * @brief Component별 Undo Function Vector
 */
smxlUndoFunc * gRecoveryUndoFuncs[SMG_COMPONENT_MAX];

void smxlRegisterUndoFuncs( smgComponentClass   aComponentClass,
                            smxlUndoFunc      * aUndoFuncs )
{
    gRecoveryUndoFuncs[aComponentClass] = aUndoFuncs;
}


/**
 * @brief 트랜잭션 슬롯들을 초기화 한다.
 * @param[in]     aTransTableSize Transaction Table Size
 * @param[in]     aIsShared       Shared Transaction Table
 * @param[in,out] aEnv            Environment 포인터
 */
stlStatus smxlInitTrans( stlInt64   aTransTableSize,
                         stlBool    aIsShared,
                         smlEnv   * aEnv )
{
    stlUInt32  i;
    
    for( i = 0; i < aTransTableSize; i++ )
    {
        SMXL_INIT_TRANS( &SMXL_TRANS_TABLE(i), i, aIsShared, aEnv );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 트랜잭션 슬롯들을 재초기화 한다.
 * @param[in,out] aEnv  Environment 포인터
 */
stlStatus smxlResetTrans( smlEnv * aEnv )
{
    stlUInt32  i;
    
    for( i = 0; i < SMXL_TRANS_TABLE_SIZE; i++ )
    {
        SMXL_RESET_TRANS( &SMXL_TRANS_TABLE(i), aEnv, STL_FALSE );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 트랜잭션 슬롯들을 삭제한다.
 * @param[in,out] aEnv Environment 포인터
 */
stlStatus smxlFinTrans( smlEnv * aEnv )
{
    stlUInt32  i;
    
    for( i = 0; i < SMXL_TRANS_TABLE_SIZE; i++ )
    {
        STL_TRY( knlDestroyRegionMem( &SMXL_TRANS_TABLE(i).mShmMem,
                                      (knlEnv*)aEnv ) == STL_SUCCESS );
        KNL_INIT_REGION_MEMORY( &SMXL_TRANS_TABLE(i).mShmMem );

        STL_TRY( smklFinLock( i, aEnv ) == STL_SUCCESS );
        knlFinLatch( &SMXL_TRANS_TABLE(i).mLatch );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Transaction Ideintifier의 유효성을 검사한다.
 * @param[in]     aTransId   Transaction Ideintifier
 * @param[in,out] aEnv       Environment 포인터
 */
stlStatus smxlValidateTransId( smlTransId   aTransId,
                               smlEnv     * aEnv )
{
    if( aTransId != SML_INVALID_TRANSID )
    {
        STL_DASSERT( smxlGetTransSeq( aTransId ) == SMXL_TRANS_SEQ( aTransId ) );
    }
    
    return STL_SUCCESS;
}

/**
 * @brief Global Transaction 여부
 * @param[in]     aTransId   Transaction Ideintifier
 */
stlBool smxlIsGlobalTrans( smlTransId aTransId )
{
    smxlTrans * sTrans;
    
    if( aTransId == SML_INVALID_TRANSID )
    {
        return STL_FALSE;
    }

    sTrans = SMXL_TRANS_SLOT( aTransId );
    return sTrans->mIsGlobal;
}

/**
 * @brief 트랜잭션 슬롯을 할당받는다.
 * @param[in] aIsolationLevel 트랜잭션 독립성 레벨
 * @param[in] aIsGlobalTrans  Global Transaction 여부
 * @param[out] aTransId 할당된 트랜잭션 아이디
 * @param[out] aTransSeq 할당된 트랜잭션의 Sequence Number
 * @param[in,out] aEnv Environment 포인터
 */
stlStatus smxlAllocTrans( smlIsolationLevel   aIsolationLevel,
                          stlBool             aIsGlobalTrans,
                          smxlTransId       * aTransId,
                          stlUInt64         * aTransSeq,
                          smlEnv            * aEnv )
{
    stlUInt32     i;
    smxlTransId   sTransId = SML_INVALID_TRANSID;
    stlBool       sIsSuccess;
    stlUInt32     sHintSlotId;
    stlInt32      sTransTableSize;
    smxlTrans   * sTrans;

    STL_PARAM_VALIDATE( aTransId != NULL, KNL_ERROR_STACK(aEnv) );

    sTransId = SML_INVALID_TRANSID;

    STL_TRY( knlValidateSar( KNL_SAR_TRANSACTION,
                             KNL_ENV(aEnv)->mTopLayer,
                             KNL_ENV(aEnv) ) == STL_SUCCESS );

    /**
     * 가장 간단한 방법으로 Transaction Id가 Invalid가 아닐 동안
     * looping 한다.
     */
    STL_TRY( knlGetSessionEnvId( KNL_SESS_ENV(aEnv),
                                 &sHintSlotId,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sTransTableSize = SMXL_TRANS_TABLE_SIZE;
    sHintSlotId = (sHintSlotId % sTransTableSize);
    
    while( 1 )
    {
        for( i = sHintSlotId; i < sTransTableSize; i++ )
        {
            sTrans = &(SMXL_TRANS_TABLE(i));
            
            if( sTrans->mState != SMXL_STATE_IDLE )
            {
                continue;
            }

            STL_TRY( knlTryLatch( &sTrans->mLatch,
                                  KNL_LATCH_MODE_EXCLUSIVE,
                                  &sIsSuccess,
                                  KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            if( sIsSuccess == STL_FALSE )
            {
                continue;
            }
                              
            if( sTrans->mState == SMXL_STATE_IDLE )
            {
                smxlGetAtomicSystemScn( &SML_SESS_ENV(aEnv)->mTransViewScn );

                sTrans->mIsolationLevel = aIsolationLevel;
                sTrans->mIsGlobal = aIsGlobalTrans;
                sTrans->mTransId = SMXL_MAKE_TRANS_ID( 0, SML_INVALID_RID_OFFSET, i );
                sTrans->mTransViewScn = SML_SESS_ENV(aEnv)->mTransViewScn;
                sTrans->mBeginTime = knlGetSystemTime();
                SML_SESS_ENV(aEnv)->mTransId = sTrans->mTransId;
                SML_SESS_ENV(aEnv)->mDisabledViewScn = SML_INFINITE_SCN;

                sTrans->mState = SMXL_STATE_ACTIVE;
                sTransId = sTrans->mTransId;
            
                *aTransSeq = sTrans->mTransSeq;
                
                STL_TRY( knlReleaseLatch( &sTrans->mLatch,
                                          (knlEnv*)aEnv )
                         == STL_SUCCESS );

                break;
            }

            STL_TRY( knlReleaseLatch( &sTrans->mLatch,
                                      (knlEnv*)aEnv )
                     == STL_SUCCESS );
        }

        if( sTransId != SML_INVALID_TRANSID )
        {
            /**
             * Transaction Begin 시 CDC를 위해 session의 Log propagate 정보를 설정한다.
             */
            sTrans->mPropagateLog =
                knlGetPropertyBoolValueByID( KNL_PROPERTY_PROPAGATE_REDO_LOG,
                                             KNL_ENV( aEnv ) );

            break;
        }
        
        for( i = 0; i < sHintSlotId; i++ )
        {
            sTrans = &(SMXL_TRANS_TABLE(i));
            
            if( sTrans->mState != SMXL_STATE_IDLE )
            {
                continue;
            }

            STL_TRY( knlTryLatch( &sTrans->mLatch,
                                  KNL_LATCH_MODE_EXCLUSIVE,
                                  &sIsSuccess,
                                  KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            if( sIsSuccess == STL_FALSE )
            {
                continue;
            }
                              
            if( sTrans->mState == SMXL_STATE_IDLE )
            {
                smxlGetAtomicSystemScn( &SML_SESS_ENV(aEnv)->mTransViewScn );
                
                sTrans->mIsolationLevel = aIsolationLevel;
                sTrans->mIsGlobal = aIsGlobalTrans;
                sTrans->mTransId = SMXL_MAKE_TRANS_ID( 0, SML_INVALID_RID_OFFSET, i );
                sTrans->mTransViewScn = SML_SESS_ENV(aEnv)->mTransViewScn;
                sTrans->mBeginTime = knlGetSystemTime();
                SML_SESS_ENV(aEnv)->mTransId = sTrans->mTransId;
                SML_SESS_ENV(aEnv)->mDisabledViewScn = SML_INFINITE_SCN;

                sTrans->mState = SMXL_STATE_ACTIVE;
                sTransId = sTrans->mTransId;
            
                *aTransSeq = sTrans->mTransSeq;
                
                STL_TRY( knlReleaseLatch( &sTrans->mLatch,
                                          (knlEnv*)aEnv )
                         == STL_SUCCESS );

                break;
            }

            STL_TRY( knlReleaseLatch( &sTrans->mLatch,
                                      (knlEnv*)aEnv )
                     == STL_SUCCESS );
        }

        if( sTransId != SML_INVALID_TRANSID )
        {
            /**
             * Transaction Begin 시 CDC를 위해 session의 Log propagate 정보를 설정한다.
             */
            sTrans->mPropagateLog =
                knlGetPropertyBoolValueByID( KNL_PROPERTY_PROPAGATE_REDO_LOG,
                                             KNL_ENV( aEnv ) );
            break;
        }

        STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );
        
        /*
         * Mock-up에서는 1ms를 기다린다.
         */
        stlSleep( 1 );
    }

    *aTransId = sTransId;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 기존 트랜잭션 슬롯에 바인드한다.
 * @param[in] aTransId      바인드될 트랜잭션 아이디
 */
void smxlBindTrans( smxlTransId aTransId )
{
    smxlTrans * sTrans;

    STL_ASSERT( aTransId != SML_INVALID_TRANSID );
    sTrans = SMXL_TRANS_SLOT( aTransId );

    if( sTrans->mState == SMXL_STATE_ACTIVE )
    {
        sTrans->mTransId = aTransId;
        return;
    }
    
    if( sTrans->mState == SMXL_STATE_IDLE )
    {
        sTrans->mState = SMXL_STATE_ACTIVE;
        sTrans->mTransId = aTransId;
        sTrans->mTransViewScn = SML_INFINITE_SCN;
    }
}

/**
 * @brief Media Recovery를 위해 트랜잭션 슬롯에 바인드한다.
 * @param[in] aTransId      바인드될 트랜잭션 아이디
 * @param[in,out] aEnv Environment 포인터
 */
void smxlBindTrans4MediaRecovery( smxlTransId   aTransId,
                                  smlEnv      * aEnv )
{
    smxlTrans * sTrans;

    STL_ASSERT( aTransId != SML_INVALID_TRANSID );
    sTrans = SMXL_MEDIA_RECOVERY_TRANS_SLOT( aEnv, aTransId );

    if( sTrans->mState == SMXL_STATE_ACTIVE )
    {
        sTrans->mTransId = aTransId;
        return;
    }
    
    if( sTrans->mState == SMXL_STATE_IDLE )
    {
        sTrans->mState = SMXL_STATE_ACTIVE;
        sTrans->mTransId = aTransId;
        sTrans->mTransViewScn = SML_INFINITE_SCN;
    }
}

/**
 * @brief 기존 트랜잭션 슬롯의 View Scn을 설정한다.
 * @param[in] aTransId      바인드될 트랜잭션 아이디
 * @param[in] aTransViewScn 바인드될 트랜잭션의 View Scn
 */
void smxlSetTransViewScn( smxlTransId aTransId,
                          smlScn      aTransViewScn )
{
    smxlTrans * sTrans;

    sTrans = SMXL_TRANS_SLOT( aTransId );
    STL_ASSERT( sTrans->mState != SMXL_STATE_IDLE );
    STL_ASSERT( sTrans->mTransId == aTransId );
    
    sTrans->mTransViewScn = aTransViewScn;
}

/**
 * @brief Media Recovery를 위해 기존 트랜잭션 슬롯의 View Scn을 설정한다.
 * @param[in] aTransId      트랜잭션 아이디
 * @param[in] aTransViewScn 트랜잭션의 View Scn
 * @param[in,out] aEnv Environment 포인터
 */
void smxlSetTransViewScn4MediaRecovery( smxlTransId   aTransId,
                                        smlScn        aTransViewScn,
                                        smlEnv      * aEnv )
{
    smxlTrans * sTrans;

    sTrans = SMXL_MEDIA_RECOVERY_TRANS_SLOT( aEnv, aTransId );

    STL_ASSERT( sTrans->mState != SMXL_STATE_IDLE );
    STL_ASSERT( sTrans->mTransId == aTransId );
    
    sTrans->mTransViewScn = aTransViewScn;
}

#include <smklDef.h>
#include <smklManager.h>

/**
 * @brief 트랜잭션 슬롯을 유휴상태로 전환시킨다.
 * @param[in] aTransId 대상 트랜잭션 아이디
 * @param[in] aPendActionType Pending Action Type
 * @param[in] aCleanup 트랜잭션 Cleanup Action 여부
 * @param[in] aWriteMode Transaction Commit Write Mode
 * @param[in,out] aEnv Environment 포인터
 * @see @a aPendActionType : smlPendActionFlags
 * @see @a aWriteMode : smlTransactionWriteMode
 */
stlStatus smxlFreeTrans( smxlTransId   aTransId,
                         stlInt32      aPendActionType,
                         stlBool       aCleanup,
                         stlInt64      aWriteMode,
                         smlEnv      * aEnv )
{
    stlBool     sIsSuccess;
    smxlTrans * sTrans  = NULL;

    KNL_ENTER_CRITICAL_SECTION( aEnv );

    if( aTransId == SML_INVALID_TRANSID )
    {
        STL_TRY( smaUpdateStmtState( aEnv ) == STL_SUCCESS );
        STL_THROW( RAMP_SUCCESS );
    }

    sTrans = SMXL_TRANS_SLOT( aTransId );

    if( smxlIsLockable( aTransId ) == STL_TRUE )
    {
        /**
         * Pending Operation과 Dictionary Versioning의 동시성 문제로 인하여 Unlock시점을
         * FreeTrans에서 한다.
         */
        STL_TRY( smklUnlock( sTrans->mTransId, aEnv ) == STL_SUCCESS );
        STL_TRY( smklWakeupWaitTrans( sTrans->mTransId, aEnv ) == STL_SUCCESS );
    }
    else
    {
#ifdef STL_DEBUG
        smklLockSlot  * sLockSlot;
        sLockSlot = smklGetLockSlot( smklGetLockSlotId( aTransId ) );
        STL_ASSERT( STL_RING_IS_EMPTY( &sLockSlot->mLockNodeList ) == STL_TRUE );
#endif
    }
    
    /**
     * Lock의 해제로 Segment의 aging을 허락한다.
     */
    smlUnsetScnToEnableAging( SML_SESS_ENV(aEnv) );
    
    /*
     * Cleanup Thread에서의 요청이라면 Heap Memory에 접근할수 없다.
     * Heap Memory의 접근없이 Active Statement를 정리할수 있는 함수를 호출한다.
     */
    if( aCleanup == STL_FALSE )
    {
        STL_TRY( smaCloseNonHoldableIterators( 0,  /* aTimestamp */
                                               aEnv )
                 == STL_SUCCESS );
    
        if( smxlIsUpdatable( sTrans->mTransId ) == STL_TRUE )
        {
            STL_TRY( smaUpdateTransInfoOnReadOnlyStmt( sTrans->mTransId,
                                                       sTrans->mCommitScn,
                                                       aEnv ) == STL_SUCCESS );
        }
    }
    else
    {
        STL_TRY( smaCleanupActiveStatements( aEnv ) == STL_SUCCESS );
    }

    STL_TRY( smaUpdateStmtState( aEnv ) == STL_SUCCESS );
    
    if( sTrans->mCommitLsn != SMR_INVALID_LSN )
    {
        STL_TRY( smrFlushLog( sTrans->mCommitLsn,
                              sTrans->mCommitSbsn,
                              STL_FALSE,   /* aSwitchBlock */
                              aWriteMode,
                              aEnv ) == STL_SUCCESS );
    }
    
    STL_TRY( knlAcquireLatch( &sTrans->mLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsSuccess,
                              (knlEnv*)aEnv )
             == STL_SUCCESS );
    
    SMXL_RESET_TRANS( sTrans, aEnv, aCleanup );
    
    /*
     * Session Environment의 정보 리셋
     */
    
    if( sTrans->mIsGlobal == STL_FALSE )
    {
        SML_SESS_ENV(aEnv)->mTransId = SML_INVALID_TRANSID;
        SML_SESS_ENV(aEnv)->mTransViewScn = SML_INFINITE_SCN;
    }
    else
    {
        STL_DASSERT( STL_RING_IS_EMPTY( &SML_SESS_ENV(aEnv)->mUpdatableStmt ) == STL_TRUE );
        STL_DASSERT( STL_RING_IS_EMPTY( &SML_SESS_ENV(aEnv)->mReadOnlyStmt ) == STL_TRUE );
    }

    STL_TRY( knlReleaseLatch( &sTrans->mLatch,
                              (knlEnv*)aEnv )
             == STL_SUCCESS );
    
    STL_RAMP( RAMP_SUCCESS );
    
    KNL_LEAVE_CRITICAL_SECTION( aEnv );
    
    return STL_SUCCESS;

    STL_FINISH;

    KNL_ASSERT( STL_FALSE, aEnv, SMXL_FORMAT_TRANS_SLOT( sTrans ) );

    return STL_FAILURE;
}

/**
 * @brief Media Recovery용 트랜잭션 슬롯을 유휴상태로 전환시킨다.
 * @param[in] aTransId 대상 트랜잭션 아이디
 * @param[in,out] aEnv Environment 포인터
 */
stlStatus smxlFreeTrans4MediaRecovery( smxlTransId   aTransId,
                                       smlEnv      * aEnv )
{
    smxlTrans * sTrans;

    KNL_ENTER_CRITICAL_SECTION( aEnv );
    
    STL_TRY_THROW( aTransId != SML_INVALID_TRANSID, RAMP_SUCCESS );

    sTrans = SMXL_MEDIA_RECOVERY_TRANS_SLOT( aEnv, aTransId );

    STL_TRY( smgExecutePendOp( SML_PEND_ACTION_ROLLBACK,
                               0, /* aStatementTcn */
                               aEnv ) == STL_SUCCESS );

    /**
     * Segment의 aging을 허락한다.
     */
    smlUnsetScnToEnableAging( SML_SESS_ENV(aEnv) );
    
    /*
     * Cleanup Thread에서의 요청이라면 Heap Memory에 접근할수 없다.
     * Heap Memory의 접근없이 Active Statement를 정리할수 있는 함수를 호출한다.
     */
    STL_TRY( smaCloseNonHoldableIterators( 0,  /* aTimestamp */
                                           aEnv )
             == STL_SUCCESS );
    
    STL_TRY( smaUpdateTransInfoOnReadOnlyStmt( SMXL_MEDIA_RECOVERY_TRANS_ID( sTrans->mTransId ),
                                               sTrans->mCommitScn,
                                               aEnv ) == STL_SUCCESS );

    STL_TRY( smaUpdateStmtState( aEnv ) == STL_SUCCESS );

    SMXL_RESET_TRANS( sTrans, aEnv, STL_FALSE );
    
    /*
     * Session Environment의 정보 리셋
     */
    SML_SESS_ENV(aEnv)->mTransId = SML_INVALID_TRANSID;
    SML_SESS_ENV(aEnv)->mTransViewScn = SML_INFINITE_SCN; 
    
    STL_RAMP( RAMP_SUCCESS );
    
    KNL_LEAVE_CRITICAL_SECTION( aEnv );
    
    return STL_SUCCESS;

    STL_FINISH;

    KNL_ASSERT( STL_FALSE, aEnv, SMXL_FORMAT_TRANS_SLOT( sTrans ) );

    return STL_FAILURE;
}

/**
 * @brief A 트랜잭션이 B 트랜잭션에 대기한다.
 * @param[in] aTransIdA 대기할 Transaction의 아이디
 * @param[in] aTransIdB 락을 획득하고 있는 Transaction의 아이디
 * @param[in] aTimeInterval 락 타임아웃 시간( sec단위 )
 * @param[in,out] aEnv Environment 포인터
 * @see @a aTimeInterval : smlLockTimeout
 * @note 레코드락으로 인해 대기해야할 경우에 사용된다.
 * <BR> 내부적으로 Shared 모드의 Latch를 요구해야 한다.
 */
stlStatus smxlWaitTrans( smxlTransId   aTransIdA,
                         smxlTransId   aTransIdB,
                         stlInt64      aTimeInterval,
                         smlEnv      * aEnv )
{
    stlInt64 sTimeInterval;
    
    STL_PARAM_VALIDATE( aTransIdB != SML_INVALID_TRANSID,
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( SMXL_IS_UNDO_BOUND_TRANS( aTransIdB ) == STL_TRUE,
                        KNL_ERROR_STACK( aEnv ) );

    STL_DASSERT( aTimeInterval != SML_LOCK_TIMEOUT_INVALID );
    
    /**
     * Invalid timeout 값은 프로퍼티를 따른다
     */
    if( aTimeInterval == SML_LOCK_TIMEOUT_PROPERTY )
    {
        sTimeInterval = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DDL_LOCK_TIMEOUT,
                                                       KNL_ENV( aEnv ) );
    }
    else
    {
        sTimeInterval = aTimeInterval;
    }

    STL_TRY( smklWaitTransLock( aTransIdA,
                                aTransIdB,
                                sTimeInterval,
                                aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 모든 Transaction이 종료될때까지 대기한다.
 * @param[in,out] aEnv Environment 포인터
 */
stlStatus smxlWaitAllTrans( smlEnv * aEnv )
{
    stlBool  sExistActiveTrans = STL_FALSE;
    stlInt32 i;

    while( 1 )
    {
        sExistActiveTrans = STL_FALSE;
        
        for( i = 0; i < SMXL_TRANS_TABLE_SIZE; i++ )
        {
            if( SMXL_TRANS_TABLE(i).mState == SMXL_STATE_PREPARE )
            {
                continue;
            }
            
            if( SMXL_TRANS_TABLE(i).mState != SMXL_STATE_IDLE )
            {
                sExistActiveTrans = STL_TRUE;
                break;
            }
        }

        if( sExistActiveTrans == STL_FALSE )
        {
            break;
        }
        
        STL_TRY( knlCheckEndSession( KNL_SESS_ENV(aEnv),
                                     KNL_ENV(aEnv) ) == STL_SUCCESS );
        
        stlSleep( 100000 );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Active Transaction의 개수를 반환한다.
 * @note 동시성 제어 없음
 */
stlInt32 smxlGetActiveTransCount()
{
    stlInt32 sActiveTransCount = 0;
    stlInt32 i;

    for( i = 0; i < SMXL_TRANS_TABLE_SIZE; i++ )
    {
        if( SMXL_TRANS_TABLE(i).mState == SMXL_STATE_PREPARE )
        {
            continue;
        }
            
        if( SMXL_TRANS_TABLE(i).mState != SMXL_STATE_IDLE )
        {
            sActiveTransCount++;
        }
    }

    return sActiveTransCount;
}

/**
 * @brief 트랜잭션 슬롯에 대기한다.( 잠금 관리자 전용 함수 )
 * @param[in] aMyTransId 대기하는 Transaction의 아이디
 * @param[in] aTargetTransSlotId 락을 획득하고 있는 Transaction Slot의 아이디
 * @param[in] aTimeInterval 락 타임아웃 시간( sec단위 )
 * @param[in] aValidationLockMode 락 유효섬 검사를 위한 메모리 주소
 * @param[in,out] aEnv Environment 포인터
 * @note 레코드락에 대기해야 한다면 Shared 모드를 요구해야 하며,
 * <BR> 테이블락을 요청하는 경우에는 Exclusive를 설정해야 한다.
 */
stlStatus smxlSuspendOnTransSlot( smxlTransId    aMyTransId,
                                  stlInt16       aTargetTransSlotId,
                                  stlInt64       aTimeInterval,
                                  stlChar      * aValidationLockMode,
                                  smlEnv       * aEnv )
{
    stlBool     sIsTimedOut;
    stlBool     sIsDeadlock;
    stlInt64    sWaitTime;  /* 단위 : usec */
    stlInt64    sOldTransSeq;
    stlInt64    sNewTransSeq;
    
    sWaitTime = (stlInt64)(aTimeInterval * 1000 * 1000);
    sOldTransSeq = smxlGetTransSeq( (smxlTransId)aTargetTransSlotId );

    /**
     * 대기하기 전에 deadlock을 먼저 검사한다( performance issue ).
     */
    STL_TRY( smklCheckDeadlock( aMyTransId,
                                &sIsDeadlock,
                                aEnv ) == STL_SUCCESS );

    STL_TRY_THROW( sIsDeadlock == STL_FALSE, RAMP_ERR_DEADLOCK );
    
    while( 1 )
    {
        /**
         * Query Timeout 검사
         */
        STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
        STL_TRY_THROW( sWaitTime > 0, RAMP_ERR_TIMEOUT_EXPIRED );
        
        STL_TRY( knlAcquireLatch( &SMXL_TRANS_TABLE(aTargetTransSlotId).mLatch,
                                  KNL_LATCH_MODE_EXCLUSIVE,
                                  KNL_LATCH_PRIORITY_NORMAL,
                                  STL_INFINITE_TIME,
                                  &sIsTimedOut,
                                  (knlEnv*)aEnv )
                 == STL_SUCCESS );

        sNewTransSeq = smxlGetTransSeq( (smxlTransId)aTargetTransSlotId );

        if( (sNewTransSeq == sOldTransSeq) &&
            ((stlInt32)*aValidationLockMode != SML_LOCK_NONE) )
        {
            STL_TRY( knlReleaseLatch( &SMXL_TRANS_TABLE(aTargetTransSlotId).mLatch,
                                      (knlEnv*)aEnv )
                     == STL_SUCCESS );

            STL_TRY( knlSuspend( SMKL_DEADLOCK_CHECK_INTERVAL,
                                 &sIsTimedOut,
                                 (knlEnv*)aEnv ) == STL_SUCCESS );
        
            if( sIsTimedOut == STL_TRUE )
            {
                STL_TRY( smklCheckDeadlock( aMyTransId,
                                            &sIsDeadlock,
                                            aEnv ) == STL_SUCCESS );

                STL_TRY_THROW( sIsDeadlock == STL_FALSE, RAMP_ERR_DEADLOCK );
                STL_TRY( knlCheckEndSession( KNL_SESS_ENV(aEnv),
                                             KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                sWaitTime -= SMKL_DEADLOCK_CHECK_INTERVAL;
            }
        }
        else
        {
            /**
             * Transaction Slot이 재사용되었거나 락이 해제된 경우라면
             */
        
            STL_TRY( knlReleaseLatch( &SMXL_TRANS_TABLE(aTargetTransSlotId).mLatch,
                                      (knlEnv*)aEnv )
                     == STL_SUCCESS );
            break;
        }
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_DEADLOCK )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_DEADLOCK,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_TIMEOUT_EXPIRED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_LOCK_TIMEOUT,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 트랜잭션을 대기(block)시킨다.
 * @param[in] aTransId 대기할 Transaction의 아이디
 * @param[in] aTimeInterval 락 타임아웃 시간( sec단위 )
 * @param[in,out] aEnv Environment 포인터
 */
stlStatus smxlSuspend( smxlTransId   aTransId,
                       stlInt64      aTimeInterval,
                       smlEnv      * aEnv )
{
    smxlTrans * sTrans;
    stlBool     sIsDeadlock;
    stlBool     sIsTimedOut;
    stlInt64    sWaitTime;  /* 단위 : usec */

    STL_ASSERT( aTransId != SML_INVALID_TRANSID );
    sTrans = SMXL_TRANS_SLOT( aTransId );
    sTrans->mState = SMXL_STATE_BLOCK;

    sWaitTime = (stlInt64)(aTimeInterval * 1000 * 1000);

    STL_TRY( smklCheckDeadlock( sTrans->mTransId,
                                &sIsDeadlock,
                                aEnv ) == STL_SUCCESS );

    STL_TRY_THROW( sIsDeadlock == STL_FALSE, RAMP_ERR_DEADLOCK );
    
    while( 1 )
    {
        /**
         * Query Timeout 검사
         */
        STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
        STL_TRY_THROW( sWaitTime > 0, RAMP_ERR_TIMEOUT_EXPIRED );
        
        STL_TRY( knlSuspend( SMKL_DEADLOCK_CHECK_INTERVAL,
                             &sIsTimedOut,
                             (knlEnv*)aEnv ) == STL_SUCCESS );
        
        if( sIsTimedOut == STL_TRUE )
        {
            STL_TRY( smklCheckDeadlock( sTrans->mTransId,
                                        &sIsDeadlock,
                                        aEnv ) == STL_SUCCESS );

            STL_TRY_THROW( sIsDeadlock == STL_FALSE, RAMP_ERR_DEADLOCK );

            sWaitTime -= SMKL_DEADLOCK_CHECK_INTERVAL;
        }
        else
        {
            break;
        }
    }
    
    sTrans->mState = SMXL_STATE_ACTIVE;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_DEADLOCK )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_DEADLOCK,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_TIMEOUT_EXPIRED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_LOCK_TIMEOUT,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 대기상태의 트랜잭션을 Wakeup 시킨다.
 * @param[in] aTransId Wakeup 시키는 Transaction의 아이디
 * @param[in] aTargetEnv Wakeup할 트랜잭션이 소유한 Environment 포인터
 * @param[in,out] aEnv Environment 포인터
 */
stlStatus smxlResume( smxlTransId   aTransId,
                      smlEnv      * aTargetEnv,
                      smlEnv      * aEnv )
{
    STL_ASSERT( aTransId != SML_INVALID_TRANSID );
    STL_TRY( knlResume( (knlEnv*)aTargetEnv,
                        (knlEnv*)aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 트랜잭션을 커밋한다.
 * @param[in] aTransId 대상 트랜잭션 아이디
 * @param[in] aComment Transaction Comment
 * @param[in] aWriteMode Transaction Commit Write Mode
 * @param[in,out] aEnv Environment 포인터
 */
stlStatus smxlCommit( smxlTransId   aTransId,
                      stlChar     * aComment,
                      stlInt64      aWriteMode,
                      smlEnv      * aEnv )
{
    stlBool     sIsTimedOut;
    smxlTrans * sTrans = NULL;
    smlScn      sCommitScn;

    KNL_ENTER_CRITICAL_SECTION( aEnv );
    
    STL_TRY_THROW( aTransId != SML_INVALID_TRANSID, RAMP_SUCCESS );
    
    sTrans = SMXL_TRANS_SLOT( aTransId );

    STL_PARAM_VALIDATE( (sTrans->mState == SMXL_STATE_ACTIVE) ||
                        (sTrans->mState == SMXL_STATE_PREPARE),
                        KNL_ERROR_STACK(aEnv) );

    if( smxlIsUpdatable( aTransId ) == STL_TRUE )
    {
        sTrans->mState = SMXL_STATE_PRECOMMIT;
        stlMemBarrier();
        sCommitScn = smxlGetNextSystemScn();

        STL_TRY( smxlWriteCommitLog( sTrans->mTransId,
                                     sCommitScn,
                                     STL_TRUE,
                                     aComment,
                                     aWriteMode,
                                     &sTrans->mCommitSbsn,
                                     &sTrans->mCommitLsn,
                                     aEnv ) == STL_SUCCESS );

        STL_TRY( knlAcquireLatch( &sTrans->mLatch,
                                  KNL_LATCH_MODE_EXCLUSIVE,
                                  KNL_LATCH_PRIORITY_NORMAL,
                                  STL_INFINITE_TIME,
                                  &sIsTimedOut,
                                  (knlEnv*)aEnv )
                 == STL_SUCCESS );

        /**
         * Segment Aging과 Unlock의 동시성 문제로 인하여 Agable Scn의 증가를 막는다.
         * - Unlock 이후에 Enable시킨다.
         */
        smlSetScnToDisableAging( aTransId, SML_SESS_ENV(aEnv) );
    
        sTrans->mState = SMXL_STATE_COMMIT;
        sTrans->mCommitScn = sCommitScn;

        if( sTrans->mUndoRelEntry != NULL )
        {
            STL_TRY( smxlFreeUndoRelation( aTransId,
                                           aEnv ) == STL_SUCCESS );
        }
    
        STL_TRY( knlReleaseLatch( &sTrans->mLatch,
                                  (knlEnv*)aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        if( smxlIsLockable( aTransId ) == STL_TRUE )
        {
            if( sTrans->mUndoRelEntry != NULL )
            {
                STL_TRY( smxlFreeUndoRelation( aTransId,
                                               aEnv ) == STL_SUCCESS );
            }
        }
        
        sTrans->mState = SMXL_STATE_COMMIT;
        sTrans->mCommitScn = smxlGetSystemScn();
    }

    /**
     * Execution Library Pending 이전에 Storage Manager Pending이 수행되어야 한다.
     * - CREATE DDL은 Dictionary Cache의 작업이 종료되는 즉시 visibility가 TRUR가
     *   되기 때문에 SM에서 Create의 Pending 작업이 진행중일때 object가 삭제될수도 있다.
     */
    STL_TRY( smgExecutePendOp( SML_PEND_ACTION_COMMIT,
                               0, /* aStatementTcn */
                               aEnv ) == STL_SUCCESS );

    STL_RAMP( RAMP_SUCCESS );

    KNL_LEAVE_CRITICAL_SECTION( aEnv );
    
    return STL_SUCCESS;

    STL_FINISH;

    KNL_ASSERT( STL_FALSE, aEnv, SMXL_FORMAT_TRANS_SLOT( sTrans ) );

    return STL_FAILURE;
}

/**
 * @brief 트랜잭션을 롤백한다.
 * @param[in] aTransId 대상 트랜잭션 아이디
 * @param[in] aWriteMode Transaction Commit Write Mode
 * @param[in] aCleanup 트랜잭션 Cleanup Action 여부
 * @param[in,out] aEnv Environment 포인터
 */
stlStatus smxlRollback( smxlTransId   aTransId,
                        stlInt64      aWriteMode,
                        stlBool       aCleanup,
                        smlEnv      * aEnv )
{
    stlBool     sIsTimedOut;
    smxlTrans * sTrans = NULL;
    smlScn      sCommitScn;

    KNL_ENTER_CRITICAL_SECTION( aEnv );
    
    STL_TRY_THROW( aTransId != SML_INVALID_TRANSID, RAMP_SUCCESS );
    
    sTrans = SMXL_TRANS_SLOT( aTransId );

    STL_PARAM_VALIDATE( (sTrans->mState == SMXL_STATE_ACTIVE) ||
                        (sTrans->mState == SMXL_STATE_PREPARE),
                        KNL_ERROR_STACK(aEnv) );
    
    if( smxlIsUpdatable( aTransId ) == STL_TRUE )
    {
        if( smxlNeedRollback( aTransId ) == STL_TRUE )
        {
            STL_TRY( smxlRollbackUndoRecords( sTrans->mTransId,
                                              SML_INVALID_RID,
                                              STL_FALSE,    /* aRestartRollback */
                                              aEnv ) == STL_SUCCESS );
        }

        sTrans->mState = SMXL_STATE_PRECOMMIT;
        stlMemBarrier();
        sCommitScn = smxlGetNextSystemScn();

        STL_TRY( smxlWriteCommitLog( sTrans->mTransId,
                                     sCommitScn,
                                     STL_FALSE,
                                     NULL, /* aComment */
                                     aWriteMode,
                                     &sTrans->mCommitSbsn,
                                     &sTrans->mCommitLsn,
                                     aEnv ) == STL_SUCCESS );
    
        STL_TRY( knlAcquireLatch( &sTrans->mLatch,
                                  KNL_LATCH_MODE_EXCLUSIVE,
                                  KNL_LATCH_PRIORITY_NORMAL,
                                  STL_INFINITE_TIME,
                                  &sIsTimedOut,
                                  (knlEnv*)aEnv )
                 == STL_SUCCESS );

        /**
         * Segment Aging과 Unlock의 동시성 문제로 인하여 Agable Scn의 증가를 막는다.
         * - Unlock 이후에 Enable시킨다.
         */
        smlSetScnToDisableAging( aTransId, SML_SESS_ENV(aEnv) );
    
        sTrans->mState = SMXL_STATE_ROLLBACK;
        sTrans->mCommitScn = sCommitScn;

        if( sTrans->mUndoRelEntry != NULL )
        {
            STL_TRY( smxlFreeUndoRelation( aTransId,
                                           aEnv ) == STL_SUCCESS );
        }
    
        STL_TRY( knlReleaseLatch( &sTrans->mLatch,
                                  (knlEnv*)aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        if( smxlIsLockable( aTransId ) == STL_TRUE )
        {
            if( sTrans->mUndoRelEntry != NULL )
            {
                STL_TRY( smxlFreeUndoRelation( aTransId,
                                               aEnv ) == STL_SUCCESS );
            }
        }
        
        sTrans->mState = SMXL_STATE_ROLLBACK;
        sTrans->mCommitScn = smxlGetSystemScn();
    }

    /**
     * Execution Library Pending 이전에 Storage Manager Pending이 수행되어야 한다.
     * - CREATE DDL은 Dictionary Cache의 작업이 종료되는 즉시 visibility가 TRUR가
     *   되기 때문에 SM에서 Create의 Pending 작업이 진행중일때 object가 삭제될수도 있다.
     */
    STL_TRY( smgExecutePendOp( SML_PEND_ACTION_ROLLBACK,
                               0, /* aStatementTcn */
                               aEnv ) == STL_SUCCESS );

    STL_RAMP( RAMP_SUCCESS );
    
    KNL_LEAVE_CRITICAL_SECTION( aEnv );
    
    return STL_SUCCESS;

    STL_FINISH;

    KNL_ASSERT( STL_FALSE, aEnv, SMXL_FORMAT_TRANS_SLOT( sTrans ) );

    return STL_FAILURE;
}

/**
 * @brief IN-DOUBT 트랜잭션을 커밋한다.
 * @param[in]     aTransId         트랜잭션 아이디
 * @param[in]     aInDoubtTransId  IN-DOUBT 트랜잭션 아이디
 * @param[in]     aComment         Transaction Comment
 * @param[in,out] aEnv             Environment 포인터
 */
stlStatus smxlCommitInDoubtTransaction( smxlTransId   aTransId,
                                        smxlTransId   aInDoubtTransId,
                                        stlChar     * aComment,
                                        smlEnv      * aEnv )
{
    stlBool     sIsTimedOut;
    smxlTrans * sTrans = NULL;
    smxlTrans * sInDoubtTrans = NULL;
    smlScn      sCommitScn;
    smlScn      sInDoubtCommitScn;

    KNL_ENTER_CRITICAL_SECTION( aEnv );
    
    STL_TRY_THROW( aTransId != SML_INVALID_TRANSID, RAMP_SUCCESS );
    
    sTrans = SMXL_TRANS_SLOT( aTransId );
    sInDoubtTrans = SMXL_TRANS_SLOT( aInDoubtTransId );

    if( sInDoubtTrans->mWrittenTransRecord == STL_TRUE )
    {
        sTrans->mState = SMXL_STATE_PRECOMMIT;
        sInDoubtTrans->mState = SMXL_STATE_PRECOMMIT;
        stlMemBarrier();
        sCommitScn = smxlGetNextSystemScn();
        sInDoubtCommitScn = smxlGetNextSystemScn();

        STL_TRY( smxlWriteInDoubtCommitLog( sTrans->mTransId,
                                            sInDoubtTrans->mTransId,
                                            sCommitScn,
                                            sInDoubtCommitScn,
                                            STL_TRUE,  /* aIsInDoubtCommit */
                                            aComment,
                                            &sTrans->mCommitSbsn,
                                            &sTrans->mCommitLsn,
                                            &sInDoubtTrans->mCommitSbsn,
                                            &sInDoubtTrans->mCommitLsn,
                                            aEnv ) == STL_SUCCESS );

        /**
         *  processing LOCAL Transaction COMMIT
         */
    
        STL_TRY( knlAcquireLatch( &sTrans->mLatch,
                                  KNL_LATCH_MODE_EXCLUSIVE,
                                  KNL_LATCH_PRIORITY_NORMAL,
                                  STL_INFINITE_TIME,
                                  &sIsTimedOut,
                                  (knlEnv*)aEnv )
                 == STL_SUCCESS );

        /**
         * Segment Aging과 Unlock의 동시성 문제로 인하여 Agable Scn의 증가를 막는다.
         * - Unlock 이후에 Enable시킨다.
         */
        smlSetScnToDisableAging( aTransId, SML_SESS_ENV(aEnv) );
    
        sTrans->mState = SMXL_STATE_COMMIT;
        sTrans->mCommitScn = sCommitScn;

        if( sTrans->mUndoRelEntry != NULL )
        {
            STL_TRY( smxlFreeUndoRelation( aTransId,
                                           aEnv ) == STL_SUCCESS );
        }
    
        STL_TRY( knlReleaseLatch( &sTrans->mLatch,
                                  (knlEnv*)aEnv )
                 == STL_SUCCESS );

        /**
         *  processing IN_DOUBT Transaction COMMIT
         */
    
        STL_TRY( knlAcquireLatch( &sInDoubtTrans->mLatch,
                                  KNL_LATCH_MODE_EXCLUSIVE,
                                  KNL_LATCH_PRIORITY_NORMAL,
                                  STL_INFINITE_TIME,
                                  &sIsTimedOut,
                                  (knlEnv*)aEnv )
                 == STL_SUCCESS );

        sInDoubtTrans->mState = SMXL_STATE_COMMIT;
        sInDoubtTrans->mCommitScn = sInDoubtCommitScn;

        if( sInDoubtTrans->mUndoRelEntry != NULL )
        {
            STL_TRY( smxlFreeUndoRelation( aInDoubtTransId,
                                           aEnv ) == STL_SUCCESS );
        }
    
        STL_TRY( knlReleaseLatch( &sInDoubtTrans->mLatch,
                                  (knlEnv*)aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( smxlCommit( aTransId,
                             NULL, /* aComment */
                             SML_TRANSACTION_CWM_WAIT,
                             aEnv )
                 == STL_SUCCESS );
        
        if( smxlIsLockable( sInDoubtTrans->mTransId ) == STL_TRUE )
        {
            if( sInDoubtTrans->mUndoRelEntry != NULL )
            {
                STL_TRY( smxlFreeUndoRelation( aInDoubtTransId,
                                               aEnv ) == STL_SUCCESS );
            }
        }
        
        sInDoubtTrans->mState = SMXL_STATE_COMMIT;
        sInDoubtTrans->mCommitScn = SML_MAXIMUM_STABLE_SCN;
    }

    /**
     * global transaction commit시 local transaction은 active 상태일수 없다.
     */
    STL_DASSERT( STL_RING_IS_EMPTY( &SML_SESS_ENV(aEnv)->mPendOp ) == STL_TRUE );
    
    STL_RAMP( RAMP_SUCCESS );

    KNL_LEAVE_CRITICAL_SECTION( aEnv );
    
    return STL_SUCCESS;

    STL_FINISH;

    KNL_ASSERT( STL_FALSE, aEnv, SMXL_FORMAT_TRANS_SLOT( sTrans ) );

    return STL_FAILURE;
}

/**
 * @brief IN-DOUBT 트랜잭션을 롤백한다.
 * @param[in]     aTransId         트랜잭션 아이디
 * @param[in]     aInDoubtTransId  IN-DOUBT 트랜잭션 아이디
 * @param[in]     aRestartRollback Restart Rollback 여부
 * @param[in,out] aEnv             Environment 포인터
 */
stlStatus smxlRollbackInDoubtTransaction( smxlTransId   aTransId,
                                          smxlTransId   aInDoubtTransId,
                                          stlBool       aRestartRollback,
                                          smlEnv      * aEnv )
{
    stlBool     sIsTimedOut;
    smxlTrans * sTrans = NULL;
    smxlTrans * sInDoubtTrans = NULL;
    smlScn      sCommitScn;
    smlScn      sInDoubtCommitScn;

    KNL_ENTER_CRITICAL_SECTION( aEnv );
    
    STL_TRY_THROW( aTransId != SML_INVALID_TRANSID, RAMP_SUCCESS );
    
    sTrans = SMXL_TRANS_SLOT( aTransId );
    sInDoubtTrans = SMXL_TRANS_SLOT( aInDoubtTransId );

    if( sInDoubtTrans->mWrittenTransRecord == STL_TRUE )
    {
        STL_TRY( smxlRollbackUndoRecords( sInDoubtTrans->mTransId,
                                          SML_INVALID_RID,
                                          aRestartRollback,
                                          aEnv ) == STL_SUCCESS );

        sTrans->mState = SMXL_STATE_PRECOMMIT;
        sInDoubtTrans->mState = SMXL_STATE_PRECOMMIT;
        stlMemBarrier();
        sCommitScn = smxlGetNextSystemScn();
        sInDoubtCommitScn = smxlGetNextSystemScn();

        STL_TRY( smxlWriteInDoubtCommitLog( sTrans->mTransId,
                                            sInDoubtTrans->mTransId,
                                            sCommitScn,
                                            sInDoubtCommitScn,
                                            STL_FALSE,  /* aIsInDoubtCommit */
                                            NULL,       /* aComment */
                                            &sTrans->mCommitSbsn,
                                            &sTrans->mCommitLsn,
                                            &sInDoubtTrans->mCommitSbsn,
                                            &sInDoubtTrans->mCommitLsn,
                                            aEnv ) == STL_SUCCESS );
    
        /**
         *  processing LOCAL Transaction COMMIT
         */
    
        STL_TRY( knlAcquireLatch( &sTrans->mLatch,
                                  KNL_LATCH_MODE_EXCLUSIVE,
                                  KNL_LATCH_PRIORITY_NORMAL,
                                  STL_INFINITE_TIME,
                                  &sIsTimedOut,
                                  (knlEnv*)aEnv )
                 == STL_SUCCESS );

        /**
         * Segment Aging과 Unlock의 동시성 문제로 인하여 Agable Scn의 증가를 막는다.
         * - Unlock 이후에 Enable시킨다.
         */
        smlSetScnToDisableAging( aTransId, SML_SESS_ENV(aEnv) );
    
        sTrans->mState = SMXL_STATE_COMMIT;
        sTrans->mCommitScn = sCommitScn;

        if( sTrans->mUndoRelEntry != NULL )
        {
            STL_TRY( smxlFreeUndoRelation( aTransId,
                                           aEnv ) == STL_SUCCESS );
        }
    
        STL_TRY( knlReleaseLatch( &sTrans->mLatch,
                                  (knlEnv*)aEnv )
                 == STL_SUCCESS );
    
        /**
         *  processing IN_DOUBT Transaction ROLLBACK
         */
    
        STL_TRY( knlAcquireLatch( &sInDoubtTrans->mLatch,
                                  KNL_LATCH_MODE_EXCLUSIVE,
                                  KNL_LATCH_PRIORITY_NORMAL,
                                  STL_INFINITE_TIME,
                                  &sIsTimedOut,
                                  (knlEnv*)aEnv )
                 == STL_SUCCESS );

        sInDoubtTrans->mState = SMXL_STATE_ROLLBACK;
        sInDoubtTrans->mCommitScn = sInDoubtCommitScn;

        if( sInDoubtTrans->mUndoRelEntry != NULL )
        {
            STL_TRY( smxlFreeUndoRelation( aInDoubtTransId,
                                           aEnv ) == STL_SUCCESS );
        }
    
        STL_TRY( knlReleaseLatch( &sInDoubtTrans->mLatch,
                                  (knlEnv*)aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( smxlCommit( aTransId,
                             NULL, /* aComment */
                             SML_TRANSACTION_CWM_WAIT,
                             aEnv )
                 == STL_SUCCESS );
        
        if( smxlIsLockable( sInDoubtTrans->mTransId ) == STL_TRUE )
        {
            if( sInDoubtTrans->mUndoRelEntry != NULL )
            {
                STL_TRY( smxlFreeUndoRelation( aInDoubtTransId,
                                               aEnv ) == STL_SUCCESS );
            }
        }
        
        sInDoubtTrans->mState = SMXL_STATE_ROLLBACK;
        sInDoubtTrans->mCommitScn = SML_MAXIMUM_STABLE_SCN;
    }
    
    /**
     * global transaction rollback시 local transaction은 active 상태일수 없다.
     */
    STL_DASSERT( STL_RING_IS_EMPTY( &SML_SESS_ENV(aEnv)->mPendOp ) == STL_TRUE );
    
    STL_RAMP( RAMP_SUCCESS );
    
    KNL_LEAVE_CRITICAL_SECTION( aEnv );
    
    return STL_SUCCESS;

    STL_FINISH;

    KNL_ASSERT( STL_FALSE, aEnv, SMXL_FORMAT_TRANS_SLOT( sTrans ) );

    return STL_FAILURE;
}

/**
 * @brief 트랜잭션을 PREPARE한다.
 * @param[in] aXid        Prepare할 글로벌 트랜잭션 아이디
 * @param[in] aTransId    Prepare할 로컬 트랜잭션 아이디
 * @param[in] aWriteMode  Transaction Commit Write Mode
 * @param[in,out] aEnv Environment 포인터 
 */
stlStatus smxlPrepare( stlXid      * aXid,
                       smxlTransId   aTransId,
                       stlInt64      aWriteMode,
                       smlEnv      * aEnv )
{
    smxlTrans        * sTrans = NULL;
    smklLockSlot     * sLockSlot;
    stlInt16           sLockSlotId;
    smklNode         * sLockNode;
    smklItemCore     * sLockItem;
    smklItemRelation * sLockItemRelation;
    smklRelation     * sRelation;
    smklItemLog      * sItemLog;
    smklLockLog        sLockLog;
    stlUInt64          sLockItemCount = 0;
    smlRid             sUndoRid;
    smxmTrans          sMiniTrans;
    stlInt32           sState = 0;
    smlRid             sTransRid;
    smrLogPieceHdr     sLogPieceHdr;
    stlChar            sLogBody[SMR_LOGPIECE_HDR_SIZE + STL_SIZEOF(smxlTransRecord)];
    smxlTransRecord    sTransRecord;
    smxlTransId        sTransId;
    

    STL_TRY_THROW( gDataStoreMode == SML_DATA_STORE_MODE_TDS, RAMP_SUCCESS );
    STL_TRY_THROW( aTransId != SML_INVALID_TRANSID, RAMP_SUCCESS );
    
    sTrans = SMXL_TRANS_SLOT( aTransId );

    STL_PARAM_VALIDATE( sTrans->mState == SMXL_STATE_ACTIVE,
                        KNL_ERROR_STACK(aEnv) );

    sTransId  = sTrans->mTransId;
    sLockSlotId = smklGetLockSlotId( sTransId );
    sLockSlot = smklGetLockSlot( sLockSlotId );

    STL_DASSERT( SMXL_IS_UNDO_BOUND_TRANS( sTransId ) == STL_TRUE );
    STL_DASSERT( sTrans->mIsGlobal == STL_TRUE );

    stlMemcpy( (stlChar*)&sLockLog.mXid, aXid, STL_SIZEOF(stlXid) );
    
    sItemLog = sLockLog.mLockItemArray;
    
    STL_RING_FOREACH_ENTRY( &sLockSlot->mLockNodeList, sLockNode )
    {
        sLockItem = sLockNode->mLockItem;

        /*
         * Deadlock이 발생한 경우에 Lock Item이 Null일수 있다.
         */
        if( sLockItem == NULL )
        {
            continue;
        }

        sItemLog[sLockItemCount].mGrantedMode = sLockItem->mGrantedMode;
        sItemLog[sLockItemCount].mItemType    = sLockItem->mItemType;

        if( sLockItem->mItemType == SMKL_ITEM_RELATION )
        {
            sItemLog[sLockItemCount].mRid = smsGetSegRid(
                SME_GET_SEGMENT_HANDLE( ((smklItemRelation*)sLockItem)->mRelationHandle) );
        }
        else
        {
            if( ((smklItemRecord*)sLockItem)->mUsageType == SMKL_LOCK_USAGE_DML )
            {
                continue;
            }

            sItemLog[sLockItemCount].mRid = ((smklItemRecord*)sLockItem)->mRid;
        }

        sLockItemCount++;

        if( sLockItemCount >= SMKL_MAX_LOCKITEM_COUNT_IN_LOG )
        {
            STL_TRY( smxmBegin( &sMiniTrans,
                                sTransId,
                                SML_INVALID_RID,
                                SMXM_ATTR_REDO | SMXM_ATTR_NO_VALIDATE_PAGE,
                                aEnv )
                     == STL_SUCCESS );
            sState = 1;
                                
            sLockLog.mLockCount = sLockItemCount;
            
            STL_TRY( smxlInsertUndoRecord( &sMiniTrans,
                                           SMG_COMPONENT_TRANSACTION,
                                           SMXL_UNDO_LOG_LOCK_FOR_PREPARE,
                                           (stlChar*)&sLockLog,
                                           ( STL_SIZEOF(stlXid) +
                                             STL_SIZEOF(stlInt64) +
                                             (STL_SIZEOF(smklItemLog) * sLockItemCount) ),
                                           SML_INVALID_RID,
                                           &sUndoRid,
                                           aEnv )
                     == STL_SUCCESS );

            sState = 0;
            STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

            sLockItemCount = 0;
        }
    }

    STL_RING_FOREACH_ENTRY( &sLockSlot->mRelationList, sRelation )
    {
        sLockItemRelation = (smklItemRelation*)smdGetLockHandle( sRelation->mRelationHandle );
        
        sItemLog[sLockItemCount].mRid = smsGetSegRid( 
            SME_GET_SEGMENT_HANDLE( sRelation->mRelationHandle) );
        
        sItemLog[sLockItemCount].mItemType    = SMKL_ITEM_RELATION;
        sItemLog[sLockItemCount].mGrantedMode = sLockItemRelation->mIntentHoldArray[sLockSlotId];
        
        sLockItemCount++;

        if( sLockItemCount >= SMKL_MAX_LOCKITEM_COUNT_IN_LOG )
        {
            STL_TRY( smxmBegin( &sMiniTrans,
                                sTransId,
                                SML_INVALID_RID,
                                SMXM_ATTR_REDO | SMXM_ATTR_NO_VALIDATE_PAGE,
                                aEnv )
                     == STL_SUCCESS );
            sState = 1;
                                
            sLockLog.mLockCount = sLockItemCount;
            
            STL_TRY( smxlInsertUndoRecord( &sMiniTrans,
                                           SMG_COMPONENT_TRANSACTION,
                                           SMXL_UNDO_LOG_LOCK_FOR_PREPARE,
                                           (stlChar*)&sLockLog,
                                           ( STL_SIZEOF(stlXid) +
                                             STL_SIZEOF(stlInt64) +
                                             (STL_SIZEOF(smklItemLog) * sLockItemCount) ),
                                           SML_INVALID_RID,
                                           &sUndoRid,
                                           aEnv )
                     == STL_SUCCESS );

            sState = 0;
            STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

            sLockItemCount = 0;
        }
    }
    
    if( sLockItemCount > 0 )
    {
        STL_TRY( smxmBegin( &sMiniTrans,
                            sTransId,
                            SML_INVALID_RID,
                            SMXM_ATTR_REDO | SMXM_ATTR_NO_VALIDATE_PAGE,
                            aEnv )
                 == STL_SUCCESS );
        sState = 1;
                                
        sLockLog.mLockCount = sLockItemCount;
            
        STL_TRY( smxlInsertUndoRecord( &sMiniTrans,
                                       SMG_COMPONENT_TRANSACTION,
                                       SMXL_UNDO_LOG_LOCK_FOR_PREPARE,
                                       (stlChar*)&sLockLog,
                                       ( STL_SIZEOF(stlXid) +
                                         STL_SIZEOF(stlInt64) +
                                         (STL_SIZEOF(smklItemLog) * sLockItemCount) ),
                                       SML_INVALID_RID,
                                       &sUndoRid,
                                       aEnv )
                 == STL_SUCCESS );

        sState = 0;
        STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

        sLockItemCount = 0;
    }

    sTransRid = SMXL_TRANS_RID( sTransId );
    
    sLogPieceHdr.mType = SMR_LOG_PREPARE;
    sLogPieceHdr.mComponentClass = SMG_COMPONENT_TRANSACTION;
    sLogPieceHdr.mRedoTargetType = SMR_REDO_TARGET_TRANSACTION;
    SMG_WRITE_RID( &sLogPieceHdr.mDataRid, &sTransRid );
    sLogPieceHdr.mSize = STL_SIZEOF( smxlTransRecord );
    sLogPieceHdr.mPropagateLog = STL_FALSE;
    sLogPieceHdr.mRepreparable = smxlIsRepreparable( aTransId );
        
    sTransRecord.mTransId = sTransId;
    sTransRecord.mCommitScn = 0;
    sTransRecord.mTransViewScn = sTrans->mTransViewScn;
    sTransRecord.mGlobalTrans = sTrans->mIsGlobal;
    sTransRecord.mCommitTime = 0;
    sTransRecord.mCommentSize = 0;
    stlMemset( sTransRecord.mPadding, 0x00, 5 );

    SMR_WRITE_LOGPIECE_HDR( (stlChar*)sLogBody, &sLogPieceHdr );
    STL_WRITE_BYTES( (stlChar*)sLogBody + SMR_LOGPIECE_HDR_SIZE,
                     &sTransRecord,
                     STL_SIZEOF( smxlTransRecord ) );
        
    STL_TRY( smrWriteLog( sTransId,
                          sLogBody,
                          STL_SIZEOF( smxlTransRecord ) + SMR_LOGPIECE_HDR_SIZE,
                          1,
                          SML_INVALID_RID,
                          ((aWriteMode == SML_TRANSACTION_CWM_WAIT) ? STL_TRUE : STL_FALSE),
                          NULL,
                          NULL,
                          aEnv ) == STL_SUCCESS );

    smxlSetTransState( sTransId, SMXL_STATE_PREPARE );

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) smxmRollback( &sMiniTrans, aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief 트랜잭션을 PREPARED TRANSACTION으로 준비한다.
 * @param[in] aTransId    Prepared 로컬 트랜잭션 아이디
 * @param[in,out] aEnv Environment 포인터 
 */
stlStatus smxlPreparePreparedTransaction( smxlTransId   aTransId,
                                          smlEnv      * aEnv )
{
    smxlTrans  * sTrans;

    sTrans = SMXL_TRANS_SLOT( aTransId );
    
    /**
     * Bind undo relation
     */
    
    STL_TRY( smxlBindUndoRelation( aTransId, aEnv ) == STL_SUCCESS );

    /**
     * Configure prepared transaction
     */
    
    sTrans->mAttr |= SMXL_ATTR_UPDATABLE | SMXL_ATTR_LOCKABLE;
    sTrans->mTryLogCount = 1;   /* 기록된것 처럼 설정 */
    sTrans->mWrittenTransRecord = STL_TRUE;
    sTrans->mPropagateLog = STL_FALSE;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief PREPARED Transaction을 Fetch 한다.
 * @param[in,out]  aTransId      Local Transaction Identifier
 * @param[out]     aTransSeq     Transaction Sequence Number
 * @param[in]      aRecoverLock  Lock 정보를 이전상태로 원복할지 여부
 * @param[out]     aXid          Recovered Global Transaction Identifier
 * @param[in,out]  aEnv Environment 정보
 * @remarks aTransId가 SML_INVALID_TRANSID 라면 스캔 시작을 의미하고,
 *          aTransId가 SML_INVALID_TRANSID 아니라면 다음 트래잭션을 recover한다.
 *          반환된 aTransId가 SML_INVALID_TRANSID 라면 스캔을 종료해야 한다.
 */
stlStatus smxlFetchPreparedTransaction( smxlTransId * aTransId,
                                        stlUInt64   * aTransSeq,
                                        stlBool       aRecoverLock,
                                        stlXid      * aXid,
                                        smlEnv      * aEnv )
{
    smxlTransId         sTransId;
    smxlTrans         * sTrans;
    smxlTransSnapshot   sTransSnapshot;
    
    sTransId = *aTransId;

    if( sTransId == SML_INVALID_TRANSID )
    {
        STL_TRY( smxlGetFirstActiveTrans( &sTransId,
                                          &sTransSnapshot,
                                          aEnv )
                 == STL_SUCCESS );
        
        if( sTransId != SML_INVALID_TRANSID )
        {
            if( smxlGetTransState( sTransId ) == SMXL_STATE_PREPARE )
            {
                STL_TRY( smxlRecoverLock4PreparedTransaction( sTransId,
                                                              aRecoverLock,
                                                              aXid,
                                                              aEnv )
                         == STL_SUCCESS );

                sTrans = SMXL_TRANS_SLOT( sTransId );
                *aTransSeq = sTrans->mTransSeq;
                
                STL_THROW( RAMP_FINISH );
            }
        }
    }
    
    while( sTransId != SML_INVALID_TRANSID )
    {
        STL_TRY( smxlGetNextActiveTrans( &sTransId,
                                         &sTransSnapshot,
                                         aEnv )
                 == STL_SUCCESS );

        if( sTransId != SML_INVALID_TRANSID )
        {
            if( smxlGetTransState( sTransId ) == SMXL_STATE_PREPARE )
            {
                STL_TRY( smxlRecoverLock4PreparedTransaction( sTransId,
                                                              aRecoverLock,
                                                              aXid,
                                                              aEnv )
                         == STL_SUCCESS );
                                                              
                sTrans = SMXL_TRANS_SLOT( sTransId );
                *aTransSeq = sTrans->mTransSeq;
                
                break;
            }
            else
            {
                /**
                 * Open 이전에서 ACTIVE transaction인데
                 * PREPARE가 아닌 상태가 있을수 없다.
                 */
                
                STL_DASSERT( STL_FALSE );
            }
        }
    }

    STL_RAMP( RAMP_FINISH );
    
    *aTransId = sTransId;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 자신의 트랜잭션 시작 시간을 얻는다.
 * @param[in] aTransId 대상 트랜잭션 아이디
 * @param[in,out] aEnv Environment 포인터
 * @note Thread Safe 하지 않기 때문에 자신의 트랜잭션 시작 시간정보만 얻어야 한다.
 */
stlTime smxlBeginTransTime( smxlTransId   aTransId,
                            smlEnv      * aEnv )
{
    smxlTrans * sTrans;
    stlTime     sBeginTime;

    /**
     * DS mode 에서는 System Time을 반환한다.
     */
    if( aTransId == SML_INVALID_TRANSID )
    {
        sBeginTime = knlGetSystemTime();
    }
    else
    {
        sTrans = SMXL_TRANS_SLOT( aTransId );
        sBeginTime = sTrans->mBeginTime;
    }

    return sBeginTime;
}

/**
 * @brief Re-preparable 트랜잭션인지 검사한다.
 * @param[in] aTransId  Local Transaction Identifier
 */
stlBool smxlIsRepreparable( smxlTransId aTransId )
{
    smxlTrans * sTrans;
    stlBool     sPreparable;

    /**
     * DS mode 에서는 System Time을 반환한다.
     */
    if( aTransId == SML_INVALID_TRANSID )
    {
        sPreparable = STL_FALSE;
    }
    else
    {
        sTrans = SMXL_TRANS_SLOT( aTransId );
        sPreparable = sTrans->mRepreparable;
    }

    return sPreparable;
}

/**
 * @brief Re-preparable 여부를 설정한다.
 * @param[in] aTransId        Local Transaction Identifier
 * @param[in] aRepreparable   Re-preparable 여부
 */
void smxlSetRepreparable( smxlTransId aTransId,
                          stlBool     aRepreparable )
{
    smxlTrans * sTrans;

    if( aTransId != SML_INVALID_TRANSID )
    {
        sTrans = SMXL_TRANS_SLOT( aTransId );
        STL_ASSERT( (sTrans->mState == SMXL_STATE_ACTIVE) ||
                    (sTrans->mState == SMXL_STATE_PREPARE) );

        sTrans->mRepreparable = aRepreparable;
    }
}

/**
 * @brief Restart시에 트랜잭션을 롤백한다.
 * @param[in] aTransId 대상 트랜잭션 아이디
 * @param[in,out] aEnv Environment 포인터
 * @note Restart시에만 사용될수 있다.
 */
stlStatus smxlRestartRollback( smxlTransId   aTransId,
                               smlEnv      * aEnv )
{
    smxlTrans * sTrans;

    STL_ASSERT( aTransId != SML_INVALID_TRANSID );
    sTrans = SMXL_TRANS_SLOT( aTransId );

    STL_PARAM_VALIDATE( (sTrans->mState == SMXL_STATE_ACTIVE) ||
                        (sTrans->mState == SMXL_STATE_PREPARE),
                        KNL_ERROR_STACK(aEnv) );
    
    sTrans->mCommitScn = smxlGetNextSystemScn();

    /**
     * Undo Record를 갖는 트랜잭션이라면
     */
    if( SML_IS_INVALID_RID(sTrans->mUndoSegRid) != STL_TRUE )
    {
        STL_TRY( smxlBindUndoRelation( sTrans->mTransId, aEnv ) == STL_SUCCESS );
    }
    else
    {
        STL_DASSERT( sTrans->mUndoRelEntry == NULL );
    }
    
    if( sTrans->mUndoRelEntry != NULL )
    {
        STL_TRY( knlLogMsg( NULL,
                            KNL_ENV( aEnv ),
                            KNL_LOG_LEVEL_INFO,
                            "[ROLLBACK] user transaction - id(%ld), scn(%ld), undo segment rid(%d,%d,%d)\n",
                            sTrans->mTransId,
                            sTrans->mCommitScn,
                            sTrans->mUndoSegRid.mTbsId,
                            sTrans->mUndoSegRid.mPageId,
                            sTrans->mUndoSegRid.mOffset )
                 == STL_SUCCESS );
        
        STL_TRY( smxlRollbackUndoRecords( sTrans->mTransId,
                                          SML_INVALID_RID,
                                          STL_TRUE,   /* aRestartRollback */
                                          aEnv ) == STL_SUCCESS );
        
        STL_TRY( smxlWriteCommitLog( sTrans->mTransId,
                                     sTrans->mCommitScn,
                                     STL_FALSE,
                                     NULL, /* aComment */
                                     SML_TRANSACTION_CWM_WAIT,
                                     &sTrans->mCommitSbsn,
                                     &sTrans->mCommitLsn,
                                     aEnv ) == STL_SUCCESS );
        
        STL_TRY( smxlFreeUndoRelation( aTransId,
                                       aEnv ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( knlLogMsg( NULL,
                            KNL_ENV( aEnv ),
                            KNL_LOG_LEVEL_INFO,
                            "[ROLLBACK] system transaction - id(%ld), scn(%ld)\n",
                            sTrans->mTransId,
                            sTrans->mCommitScn )
                 == STL_SUCCESS );
        
        STL_TRY( smxlWriteCommitLog( sTrans->mTransId,
                                     sTrans->mCommitScn,
                                     STL_FALSE,
                                     NULL, /* aComment */
                                     SML_TRANSACTION_CWM_WAIT,
                                     &sTrans->mCommitSbsn,
                                     &sTrans->mCommitLsn,
                                     aEnv ) == STL_SUCCESS );
    }
        
    sTrans->mState = SMXL_STATE_ROLLBACK;
    
    STL_TRY( smklUnlock( sTrans->mTransId, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 첫번째 Active Transaction을 얻는다.
 * @param[out]    aTransId         대상 트랜잭션 아이디
 * @param[out]    aTransSnapshot   대상 트랜잭션의 snapshot image
 * @param[in,out] aEnv             Environment 포인터
 * @note Active Transaction이 없는 경우는 aTransId에 SMXL_INVALID_TRANSID가 설정된다.
 */
stlStatus smxlGetFirstActiveTrans( smxlTransId       * aTransId,
                                   smxlTransSnapshot * aTransSnapshot,
                                   smlEnv            * aEnv )
{
    STL_PARAM_VALIDATE( aTransId != NULL, KNL_ERROR_STACK(aEnv) );

    *aTransId = SMXL_MAKE_TRANS_ID( 0, 0, -1 );
    
    STL_TRY( smxlGetNextActiveTrans( aTransId,
                                     aTransSnapshot,
                                     aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 다음 Active Transaction을 얻는다.
 * @param[in,out] aTransId         대상 트랜잭션 아이디
 * @param[out]    aTransSnapshot   대상 트랜잭션의 snapshot image
 * @param[in,out] aEnv             Environment 포인터
 * @note Active Transaction이 없는 경우는 aTransId에 SMXL_INVALID_TRANSID가 설정된다.
 */
stlStatus smxlGetNextActiveTrans( smxlTransId * aTransId,
                                  smxlTransSnapshot * aTransSnapshot,
                                  smlEnv            * aEnv )
{
    stlUInt32          i;
    smxlTransId        sTransId;
    stlBool            sIsTimedOut;
    smlRid             sUndoSegRid;
    stlInt32           sTransState;
    smlScn             sTransViewScn;
    stlBool            sRepreparable;
    stlBool            sUpdatable;
    smlIsolationLevel  sIsolationLevel;

    STL_PARAM_VALIDATE( aTransId != NULL, KNL_ERROR_STACK(aEnv) );

    sTransId = SML_INVALID_TRANSID;
    sUndoSegRid = SML_INVALID_RID;
    sTransViewScn = SML_INFINITE_SCN;
    sTransState = SMXL_STATE_IDLE;
    sRepreparable = STL_TRUE;
    sUpdatable = STL_FALSE;
    sIsolationLevel = SML_TIL_READ_UNCOMMITTED;

    for( i = SMXL_GET_NEXT_TRANS_SLOT_ID( *aTransId ); i < SMXL_TRANS_TABLE_SIZE; i++ )
    {
        sTransState = SMXL_TRANS_TABLE(i).mState;
        
        if( (sTransState == SMXL_STATE_IDLE)   ||
            (sTransState == SMXL_STATE_COMMIT) ||
            (sTransState == SMXL_STATE_ROLLBACK) )
        {
            continue;
        }

        STL_TRY( knlAcquireLatch( &SMXL_TRANS_TABLE(i).mLatch,
                                  KNL_LATCH_MODE_EXCLUSIVE,
                                  KNL_LATCH_PRIORITY_NORMAL,
                                  STL_INFINITE_TIME,
                                  &sIsTimedOut,
                                  (knlEnv*)aEnv )
                 == STL_SUCCESS );

        sTransState = SMXL_TRANS_TABLE(i).mState;
        
        if( (sTransState == SMXL_STATE_IDLE)    ||
            (sTransState == SMXL_STATE_COMMIT)  ||
            (sTransState == SMXL_STATE_ROLLBACK) )
        {
            STL_TRY( knlReleaseLatch( &SMXL_TRANS_TABLE(i).mLatch,
                                      (knlEnv*)aEnv )
                     == STL_SUCCESS );
            continue;
        }

        sTransId = SMXL_TRANS_TABLE(i).mTransId;
        sTransViewScn = SMXL_TRANS_TABLE(i).mTransViewScn;
        sRepreparable = SMXL_TRANS_TABLE(i).mRepreparable;
        sUpdatable = smxlIsUpdatable( sTransId );
        sIsolationLevel = SMXL_TRANS_TABLE(i).mIsolationLevel;

        /**
         * Undo Relation이 할당되어 있고, Transaction Record가 기록되어 있는 경우가
         * Rollback이 필요한 경우이다.
         */
        if( smxlNeedRollback( sTransId ) == STL_TRUE )
        {
            sUndoSegRid = ((smxlUndoRelEntry*)SMXL_TRANS_TABLE(i).mUndoRelEntry)->mSegRid;
        }
        else
        {
            sTransId = SMXL_MAKE_TRANS_ID( 0, SML_INVALID_RID_OFFSET, i );
        }
        
        STL_TRY( knlReleaseLatch( &SMXL_TRANS_TABLE(i).mLatch,
                                  (knlEnv*)aEnv )
                 == STL_SUCCESS );
        break;
    }

    *aTransId = sTransId;
    aTransSnapshot->mUndoSegRid = sUndoSegRid;
    aTransSnapshot->mTransViewScn = sTransViewScn;
    aTransSnapshot->mState = sTransState;
    aTransSnapshot->mRepreparable = sRepreparable;
    aTransSnapshot->mUpdatable = sUpdatable;
    aTransSnapshot->mIsolationLevel = sIsolationLevel;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Transaction subsystem을 사용자가 설정한 프로퍼티에 따라서 restructure 시킨다.
 * @param[in,out] aEnv  Environment 포인터
 */
stlStatus smxlRestructure( smlEnv * aEnv )
{
    STL_TRY( smxlRestructureTransTable( aEnv ) == STL_SUCCESS );

    STL_TRY( smxlRestructureUndoRelation( aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smxlRestructureTransTable( smlEnv * aEnv )
{
    stlInt64          sNewTransTableSize = 0;
    stlInt64          sOldTransTableSize = 0;
    smxlWarmupEntry * sWarmupEntry;
    stlInt32          i;

    sWarmupEntry = gSmxlWarmupEntry;
    
    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_TRANSACTION_TABLE_SIZE,
                                      &sNewTransTableSize,
                                      KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    sOldTransTableSize = smfGetTransTableSize();

    STL_TRY_THROW( sNewTransTableSize != sOldTransTableSize, RAMP_SUCCESS );
    
    for( i = sNewTransTableSize; i < sOldTransTableSize; i++ )
    {
        STL_TRY( knlDestroyRegionMem( &(sWarmupEntry->mTransTable[i].mShmMem),
                                      KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
            
    sWarmupEntry->mTransTableSize = sNewTransTableSize;
    
    smfSetTransTableSize( sNewTransTableSize );
    
    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV(aEnv),
                        KNL_LOG_LEVEL_WARNING,
                        "[STARTUP] restructure transaction table - old(%d), new(%d)",
                        sOldTransTableSize,
                        sNewTransTableSize )
             == STL_SUCCESS );
    
    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

inline stlStatus smxlAllocShmMem( smxlTransId    aTransId,
                                  stlInt32       aSize,
                                  void        ** aAddr,
                                  smlEnv       * aEnv )
{
    stlInt16 sTransSlotId;

    STL_ASSERT( aTransId != SML_INVALID_TRANSID );
    sTransSlotId = SMXL_TRANS_SLOT_ID( aTransId );
    STL_DASSERT( (SMXL_TRANS_TABLE(sTransSlotId).mState == SMXL_STATE_ACTIVE) ||
                 (SMXL_TRANS_TABLE(sTransSlotId).mState == SMXL_STATE_PREPARE) );

    STL_TRY( knlAllocRegionMem( &SMXL_TRANS_TABLE(sTransSlotId).mShmMem,
                                aSize,
                                aAddr,
                                (knlEnv*)aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

inline void smxlUpdateTransId( smxlTransId   aTransId,
                               smlEnv      * aEnv )
{
    stlInt16 sTransSlotId;

    STL_ASSERT( aTransId != SML_INVALID_TRANSID );
    sTransSlotId = SMXL_TRANS_SLOT_ID( aTransId );
    STL_ASSERT( SMXL_TRANS_TABLE(sTransSlotId).mState == SMXL_STATE_ACTIVE );

    SMXL_TRANS_TABLE(sTransSlotId).mTransId = aTransId;
    SML_SESS_ENV(aEnv)->mTransId = aTransId;
}

inline void smxlUpdateBeginLsn( smxlTransId   aTransId,
                                smlEnv      * aEnv )
{
    stlInt16 sTransSlotId;

    STL_ASSERT( aTransId != SML_INVALID_TRANSID );
    sTransSlotId = SMXL_TRANS_SLOT_ID( aTransId );
    STL_ASSERT( SMXL_TRANS_TABLE(sTransSlotId).mState == SMXL_STATE_ACTIVE );

    SMXL_TRANS_TABLE(sTransSlotId).mBeginLsn = smrGetSystemLsn();
} 

inline smlIsolationLevel smxlGetIsolationLevel( smxlTransId aTransId )
{
    smxlTrans        * sTrans;
    smlIsolationLevel  sIsolationLevel;

    if( aTransId == SML_INVALID_TRANSID )
    {
        sIsolationLevel = SML_TIL_READ_COMMITTED;
    }
    else
    {
        sTrans = SMXL_TRANS_SLOT( aTransId );
        STL_ASSERT( (sTrans->mState == SMXL_STATE_ACTIVE) ||
                    (sTrans->mState == SMXL_STATE_PREPARE) );
        sIsolationLevel = sTrans->mIsolationLevel;
    }

    return sIsolationLevel;
}

inline smxlTransId smxlGetTransId( smxlTransId aTransId )
{
    smxlTrans   * sTrans;
    smxlTransId   sTransId;

    if( aTransId == SML_INVALID_TRANSID )
    {
        sTransId = SML_INVALID_TRANSID;
    }
    else
    {
        sTrans = SMXL_TRANS_SLOT( aTransId );
        STL_ASSERT( (sTrans->mState == SMXL_STATE_ACTIVE)    ||
                    (sTrans->mState == SMXL_STATE_PRECOMMIT) ||
                    (sTrans->mState == SMXL_STATE_PREPARE) );
        sTransId = sTrans->mTransId;
    }

    return sTransId;
}

inline smxlTransId smxlGetUnsafeTransId( smxlTransId aTransId )
{
    smxlTrans   * sTrans;
    smxlTransId   sTransId;

    if( aTransId == SML_INVALID_TRANSID )
    {
        sTransId = SML_INVALID_TRANSID;
    }
    else
    {
        sTrans = SMXL_TRANS_SLOT( aTransId );
        sTransId = sTrans->mTransId;
    }

    return sTransId;
}

inline smxlTransId smxlGetUnsafeTransId4MediaRecovery( smxlTransId   aTransId,
                                                       smlEnv      * aEnv )
{
    smxlTrans   * sTrans;
    smxlTransId   sTransId;

    if( aTransId == SML_INVALID_TRANSID )
    {
        sTransId = SML_INVALID_TRANSID;
    }
    else
    {
        sTrans = SMXL_MEDIA_RECOVERY_TRANS_SLOT( aEnv, aTransId );
        sTransId = sTrans->mTransId;
    }

    return sTransId;
}

inline void smxlAddTryLogCount( smxlTransId aTransId,
                                stlInt32    aTryLogCount )
{
    stlInt16 sTransSlotId;

    STL_ASSERT( aTransId != SML_INVALID_TRANSID );

    /**
     * System Transaction은 Redo Only이며 특정 트랜잭션에서 기록된
     * System Log라 하더라도 Commit 로그를 기록할 필요없다.
     * - Sequence에서 System Transaction log를 기록한다.
     */
    if( SMXL_IS_SYSTEM_TRANS( aTransId ) == STL_FALSE )
    {
        sTransSlotId = SMXL_TRANS_SLOT_ID( aTransId );
        STL_ASSERT( (SMXL_TRANS_TABLE(sTransSlotId).mState == SMXL_STATE_ACTIVE) ||
                    (SMXL_TRANS_TABLE(sTransSlotId).mState == SMXL_STATE_PREPARE) );

        SMXL_TRANS_TABLE(sTransSlotId).mTryLogCount += aTryLogCount;
    }
}

inline stlStatus smxlGetCommitScnFromCache( smxlTransId   aTransId,
                                            smlScn      * aCommitScn,
                                            smlEnv      * aEnv )
{
    smxlTrans * sTrans;

    if( aTransId == SML_INVALID_TRANSID )
    {
        *aCommitScn = 0;
    }
    else
    {
        sTrans = SMXL_TRANS_SLOT( aTransId );
        *aCommitScn = sTrans->mCommitScn;
    }
    
    return STL_SUCCESS;
}

inline smlTcn smxlGetNextTcn( smxlTransId aTransId )
{
    smxlTrans * sTrans;
    smlTcn      sTcn;

    if( aTransId == SML_INVALID_TRANSID )
    {
        /**
         * OPEN phase 이전에는 visibility를 검증할 필요가 없다.
         */
        sTcn = 0;
    }
    else
    {
        sTrans = SMXL_TRANS_SLOT( aTransId );
        STL_ASSERT( sTrans->mState == SMXL_STATE_ACTIVE );

        sTcn = sTrans->mTcn;
        sTrans->mTcn++;
    }
    
    return sTcn;
}

inline smlTcn smxlGetTcn( smxlTransId aTransId )
{
    smxlTrans * sTrans;
    smlTcn      sTcn;

    if( aTransId == SML_INVALID_TRANSID )
    {
        sTcn = 0;
    }
    else
    {
        sTrans = SMXL_TRANS_SLOT( aTransId );
        STL_ASSERT( sTrans->mState == SMXL_STATE_ACTIVE );

        sTcn = sTrans->mTcn;
    }
    
    return sTcn;
}

inline stlInt64 smxlGetTransTableSize( )
{
    return SMXL_TRANS_TABLE_SIZE;
}

inline void smxlSetTransState( smxlTransId aTransId,
                               stlInt32    aState )
{
    STL_ASSERT( aTransId != SML_INVALID_TRANSID );
    SMXL_TRANS_TABLE(SMXL_TRANS_SLOT_ID(aTransId)).mState = aState;
}

inline stlInt32 smxlGetTransState( smxlTransId aTransId )
{
    STL_ASSERT( aTransId != SML_INVALID_TRANSID );
    return SMXL_TRANS_TABLE(SMXL_TRANS_SLOT_ID(aTransId)).mState;
}

stlStatus smxlGetSafeCommitScn( smxlTransId   aTransId,
                                smlScn        aTransViewScn,
                                smlScn      * aCommitScn,
                                stlBool     * aIsSuccess,
                                smlEnv      * aEnv )
{
    stlInt32        sTransState = 0;
    smlScn          sCommitScn = SML_INFINITE_SCN;
    smxlTrans     * sTrans;
    smxlTransId     sTransId;
    smlScn          sTransViewScn;
    smlServerState  sServerState;

    STL_PARAM_VALIDATE( aTransId != SML_INVALID_TRANSID, KNL_ERROR_STACK( aEnv ) );

    *aIsSuccess = STL_TRUE;

    sServerState = smfGetServerState();

    /**
     * Online Media Recovery 중에 IDLE 상태의 트랜잭션 CommitScn은 Upper Bound Scn을 갖는다.
     * : online media recovery를 위한 전용의 transaction table을 이용
     */
    if( smrProceedOnlineMediaRecovery( aEnv ) == STL_TRUE )
    {
        sTrans = SMXL_MEDIA_RECOVERY_TRANS_SLOT( aEnv, aTransId );

        *aIsSuccess = STL_TRUE;
        if( (sTrans->mState == SMXL_STATE_IDLE) ||
            (SMXL_MEDIA_RECOVERY_TRANS_ID(aTransId) !=
             SMXL_MEDIA_RECOVERY_TRANS_ID(sTrans->mTransId)) ||
            (sTrans->mTransViewScn > aTransViewScn) )
        {
            *aCommitScn = 0;
        }
        else
        {
            *aCommitScn = SML_INFINITE_SCN;
        }

        STL_THROW( RAMP_SUCCESS );
    }

    sTrans = SMXL_TRANS_SLOT( aTransId );
    sTransId = sTrans->mTransId;

    /**
     * Restart Recovery 중에 IDLE 상태의 트랜잭션 CommitScn은 Upper Bound Scn을 갖는다.
     * : MOUNT 단계에서의 Media Recovery는 Restart Recovery와 동일하다.
     */
    if( (sServerState == SML_SERVER_STATE_RECOVERING) ||
        (smrProceedMediaRecovery(aEnv) == STL_TRUE) )
    {
        *aIsSuccess = STL_TRUE;
        if( (sTrans->mState == SMXL_STATE_IDLE) ||
            (aTransId != sTransId) ||
            (sTrans->mTransViewScn > aTransViewScn) )
        {
            *aCommitScn = 0;
        }
        else
        {
            *aCommitScn = SML_INFINITE_SCN;
        }

        STL_THROW( RAMP_SUCCESS );
    }
    
    if( aTransId != sTransId )
    {
        *aIsSuccess = STL_FALSE;
        STL_THROW( RAMP_SUCCESS );
    }

    /**
     * Memmory Barrier 비용을 제거하기 위해서 AgableViewScn으로 검사한다.
     */
    if( SMXL_IS_AGABLE_VIEW_SCN( smxlGetAgableViewScn(aEnv), aTransViewScn ) )
    {
        *aIsSuccess = STL_TRUE;
        *aCommitScn = 0;
        STL_THROW( RAMP_SUCCESS );
    }
    
    sTransViewScn = sTrans->mTransViewScn;
    sCommitScn = sTrans->mCommitScn;
    sTransState = sTrans->mState;
    stlMemBarrier();

    /**
     * sCommitScn의 유효성을 판단한다.
     */
    if( aTransId != sTrans->mTransId )
    {
        *aIsSuccess = STL_FALSE;
        STL_THROW( RAMP_SUCCESS );
    }

    if( (sTransState == SMXL_STATE_ACTIVE) ||
        (sTransState == SMXL_STATE_BLOCK)  ||
        (sTransState == SMXL_STATE_PREPARE) )
    {
        if( sTransViewScn == aTransViewScn )
        {
            sCommitScn = SML_INFINITE_SCN;
        }
        else
        {
            if( aTransId == sTrans->mTransId )
            {
                /**
                 * Transaction Undo Record가 재사용되었다.
                 */
                sCommitScn = 0;
            }
            else
            {
                /**
                 * Transaction Slot이 재사용되었다.
                 */
                *aIsSuccess = STL_FALSE;
                STL_THROW( RAMP_SUCCESS );
            }
        }
    }

    if( (sTransState == SMXL_STATE_COMMIT)    ||
        (sTransState == SMXL_STATE_PRECOMMIT) ||
        (sTransState == SMXL_STATE_ROLLBACK) )
    {
        while( 1 )
        {
            sTransId = sTrans->mTransId;
            sTransViewScn = sTrans->mTransViewScn;
            stlMemBarrier();
            sCommitScn = sTrans->mCommitScn;

            if( aTransId != sTransId )
            {
                *aIsSuccess = STL_FALSE;
                STL_THROW( RAMP_SUCCESS );
            }

            if( sCommitScn != SML_INFINITE_SCN )
            {
                if( sTransViewScn != aTransViewScn )
                {
                    if( aTransId == sTrans->mTransId )
                    {
                        /**
                         * Transaction Undo Record가 재사용되었다.
                         */
                        sCommitScn = 0;
                    }
                    else
                    {
                        /**
                         * Transaction Slot이 재사용되었다.
                         */
                        *aIsSuccess = STL_FALSE;
                        STL_THROW( RAMP_SUCCESS );
                    }
                }
                break;
            }

            stlBusyWait();
        }
    }

    *aCommitScn = sCommitScn;
    
    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

inline stlInt64 smxlGetNextTimestamp( smxlTransId aTransId )
{
    smxlTrans  * sTrans;
    stlInt64     sTimestamp;

    if( aTransId == SML_INVALID_TRANSID )
    {
        /*
         * Invalid transaction의 경우는 어떠한 timestamp를 넘겨도 무방하다.
         */
        sTimestamp = 0;
    }
    else
    {
        sTrans = SMXL_TRANS_SLOT( aTransId );
        STL_ASSERT( sTrans->mState == SMXL_STATE_ACTIVE );

        sTimestamp = sTrans->mTimestamp;
        sTrans->mTimestamp++;
    }
    
    return sTimestamp;
}

inline stlInt64 smxlGetCurTimestamp( smxlTransId aTransId )
{
    smxlTrans  * sTrans;

    STL_ASSERT( aTransId != SML_INVALID_TRANSID );
    sTrans = SMXL_TRANS_SLOT( aTransId );
    STL_ASSERT( sTrans->mState == SMXL_STATE_ACTIVE );

    return sTrans->mTimestamp;
}

inline stlBool smxlIsTransRecordRecorded( smxlTransId aTransId )
{
    smxlTrans  * sTrans;

    STL_ASSERT( aTransId != SML_INVALID_TRANSID );
    sTrans = SMXL_TRANS_SLOT( aTransId );

    return sTrans->mWrittenTransRecord;
}

/**
 * @brief Transaction 속성을 설정한다.
 * @param[in] aTransId Transaction Identifier
 * @param[in] aAttr Transaction Attribute
 * @see smxlAttr
 */
inline void smxlSetTransAttr( smxlTransId aTransId,
                              stlInt32   aAttr )
{
    smxlTrans  * sTrans;
    
    if( aTransId != SML_INVALID_TRANSID )
    {
        sTrans = SMXL_TRANS_SLOT( aTransId );
        STL_DASSERT( (sTrans->mState == SMXL_STATE_ACTIVE) ||
                     (sTrans->mState == SMXL_STATE_PREPARE) );
        sTrans->mAttr |= aAttr;
    }
}

/**
 * @brief Updatable 여부를 얻는다.
 * @param[in] aTransId Transaction Identifier
 * @return Updatable 여부
 */
inline stlBool smxlIsUpdatable( smxlTransId aTransId )
{
    stlBool      sUpdatable = STL_FALSE;
    smxlTrans  * sTrans;
    
    if( aTransId != SML_INVALID_TRANSID )
    {
        sTrans = SMXL_TRANS_SLOT( aTransId );
        
        if( (sTrans->mAttr & SMXL_ATTR_UPDATABLE) == SMXL_ATTR_UPDATABLE )
        {
            sUpdatable = STL_TRUE;
        }
    }

    return sUpdatable;
}

/**
 * @brief Lockable 여부를 얻는다.
 * @param[in] aTransId Transaction Identifier
 * @return Lockable 여부
 */
inline stlBool smxlIsLockable( smxlTransId aTransId )
{
    stlBool      sLockable = STL_FALSE;
    smxlTrans  * sTrans;
    
    if( aTransId != SML_INVALID_TRANSID )
    {
        sTrans = SMXL_TRANS_SLOT( aTransId );

        STL_DASSERT( (sTrans->mState == SMXL_STATE_ACTIVE)    ||
                     (sTrans->mState == SMXL_STATE_PREPARE)   ||
                     (sTrans->mState == SMXL_STATE_COMMIT)    ||
                     (sTrans->mState == SMXL_STATE_PRECOMMIT) ||
                     (sTrans->mState == SMXL_STATE_ROLLBACK) );

        if( (sTrans->mAttr & SMXL_ATTR_LOCKABLE) == SMXL_ATTR_LOCKABLE )
        {
            sLockable = STL_TRUE;
        }
    }

    return sLockable;
}


inline void smxlSetTransRecordRecorded( smxlTransId aTransId )
{
    smxlTrans  * sTrans;

    STL_ASSERT( aTransId != SML_INVALID_TRANSID );
    sTrans = SMXL_TRANS_SLOT( aTransId );

    sTrans->mWrittenTransRecord = STL_TRUE;
}

inline void smxlIncUndoPageCount( smxlTransId aTransId )
{
    smxlTrans  * sTrans;

    STL_ASSERT( aTransId != SML_INVALID_TRANSID );
    sTrans = SMXL_TRANS_SLOT( aTransId );

    sTrans->mUsedUndoPageCount += 1;
}

inline stlInt64 smxlGetUndoPageCount( smxlTransId aTransId )
{
    smxlTrans  * sTrans;

    STL_ASSERT( aTransId != SML_INVALID_TRANSID );
    sTrans = SMXL_TRANS_SLOT( aTransId );

    return sTrans->mUsedUndoPageCount;
}

inline void smxlSetTransRecordRecorded4MediaRecovery( smxlTransId   aTransId,
                                                      smlEnv      * aEnv )
{
    smxlTrans  * sTrans;

    STL_ASSERT( aTransId != SML_INVALID_TRANSID );
    sTrans = SMXL_MEDIA_RECOVERY_TRANS_SLOT( aEnv, aTransId );

    sTrans->mWrittenTransRecord = STL_TRUE;
}

inline stlBool smxlNeedRollback( smxlTransId aTransId )
{
    smxlTrans  * sTrans;
    stlBool      sNeedRollback = STL_FALSE;

    STL_ASSERT( aTransId != SML_INVALID_TRANSID );
    sTrans = SMXL_TRANS_SLOT( aTransId );

    if( (sTrans->mUndoRelEntry != NULL) &&
        (sTrans->mWrittenTransRecord == STL_TRUE) )
    {
        sNeedRollback = STL_TRUE;
    }
    
    return sNeedRollback;
}

inline stlInt64 smxlGetTransSeq( smxlTransId aTransId )
{
    smxlTrans  * sTrans;

    STL_DASSERT( aTransId != SML_INVALID_TRANSID );
    
    sTrans = SMXL_TRANS_SLOT( aTransId );
    return sTrans->mTransSeq;
}
                
/**
 * @brief Undo Segment의 크기를 줄인다.
 * @param[in]     aTransId     Transaction Identifier
 * @param[in,out] aEnv         Environment 포인터
 */
stlStatus smxlShrinkTo( smxlTransId     aTransId,
                        smlEnv        * aEnv )
{
    stlBool            sTimeout  = STL_FALSE;
    smxlUndoRelCache * sUndoRelCache;
    smxlUndoRelEntry * sUndoEntry;
    stlInt32           sState = 0;
    smlSegmentAttr     sSegmentAttr;
    stlInt64           sAllocPageCount;
    stlInt32           i;

    stlMemset( &sSegmentAttr, 0x00, STL_SIZEOF(smlSegmentAttr) );

    sSegmentAttr.mValidFlags = SML_SEGMENT_SHRINKTOSIZE_YES;
    sSegmentAttr.mShrinkToSize = knlGetPropertyBigIntValueByID( KNL_PROPERTY_UNDO_SHRINK_THRESHOLD,
                                                                KNL_ENV(aEnv) );

    for( i = 0; i < SMXL_UNDO_REL_COUNT; i++ )
    {
        sUndoEntry = &(SMXL_UNDO_REL_ENTRY( i ));
        sUndoRelCache = (smxlUndoRelCache*)(sUndoEntry->mRelHandle);

        sAllocPageCount = smsGetAllocPageCount( sUndoRelCache->mSegHandle );

        if( (sAllocPageCount * SMP_PAGE_SIZE) > sSegmentAttr.mShrinkToSize )
        {
            STL_TRY( knlAcquireLatch( &sUndoEntry->mLatch,
                                      KNL_LATCH_MODE_EXCLUSIVE,
                                      KNL_LATCH_PRIORITY_NORMAL,
                                      STL_INFINITE_TIME,   /* interval */
                                      &sTimeout,
                                      KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            sState = 1;

            if( sUndoEntry->mIsFree == STL_FALSE )
            {
                sState = 0;
                STL_TRY( knlReleaseLatch( &sUndoEntry->mLatch,
                                          KNL_ENV(aEnv) )
                         == STL_SUCCESS );
        
                continue;
            }

            sUndoEntry->mIsFree = STL_FALSE;
    
            sState = 0;
            STL_TRY( knlReleaseLatch( &sUndoEntry->mLatch,
                                      KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        
            STL_TRY( smsShrinkTo( aTransId,
                                  sUndoRelCache->mSegHandle,
                                  &sSegmentAttr,
                                  aEnv )
                     == STL_SUCCESS );
        
            sUndoEntry->mIsFree = STL_TRUE;
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) knlReleaseLatch( &sUndoEntry->mLatch, KNL_ENV(aEnv) );
    }

    return STL_FAILURE;
}

/** @} */
