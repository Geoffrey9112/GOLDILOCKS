/*******************************************************************************
 * smxlScnMgr.c
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
#include <smrDef.h>
#include <smf.h>
#include <smr.h>
#include <smp.h>
#include <sms.h>
#include <smg.h>
#include <smxm.h>
#include <smxl.h>
#include <smxlScnMgr.h>
#include <smxlUndoRelMgr.h>
#include <smlGeneral.h>

/**
 * @file smxlScnMgr.c
 * @brief Storage Manager Layer Local Transactional Scn Manager Internal Routines
 */

extern smxlWarmupEntry * gSmxlWarmupEntry;

/**
 * @addtogroup smxl
 * @{
 */

inline smlScn smxlGetTransViewScn( smxlTransId aTransId )
{
    smxlTrans * sTrans;
    smlScn      sTransViewScn;

    if( aTransId == SML_INVALID_TRANSID )
    {
        sTransViewScn = SML_INFINITE_SCN;
    }
    else
    {
        sTrans = SMXL_TRANS_SLOT( aTransId );
        STL_ASSERT( (sTrans->mState == SMXL_STATE_ACTIVE)    ||
                    (sTrans->mState == SMXL_STATE_PRECOMMIT) ||
                    (sTrans->mState == SMXL_STATE_PREPARE) );
        sTransViewScn = sTrans->mTransViewScn;
    }

    return sTransViewScn;
}

inline void smxlGetViewScn( smxlTransId   aTransId,
                            smlScn      * aScn )
{
    smxlTrans * sTrans;

    if( aTransId == SML_INVALID_TRANSID )
    {
        /**
         * OPEN phase 이전에 statement를 할당하는 경우이거나,
         * transaction 없이 SELECT 하는 경우
         */
        smxlGetAtomicSystemScn( aScn );
    }
    else
    {
        sTrans = SMXL_TRANS_SLOT( aTransId );
        STL_ASSERT( sTrans->mState == SMXL_STATE_ACTIVE );

        if( sTrans->mIsolationLevel == SML_TIL_SERIALIZABLE )
        {
            *aScn = sTrans->mTransViewScn;
        }
        else
        {
            if( sTrans->mIsolationLevel == SML_TIL_READ_COMMITTED )
            {
                smxlGetAtomicSystemScn( aScn );
            }
            else
            {
                /*
                 * Read Uncommitted Mode에서는 Read Only Statement라도
                 * 최신 버전을 봐야 한다.
                 */
                *aScn = SML_INFINITE_SCN;
            }
        }
    }
}

/**
 * @brief tablespace 별 agable scn 을 구한다.
 * @param[in]     aTbsId       Tablespace Identifier
 * @param[in,out] aEnv         Environment 포인터
 * @remark DDL statement의 scn은 agable scn보다 작을수 있기 때문에 <BR>
 *         DICTIONARY TABLESPACE는 특별히 agable stmt scn 을 반환해야 한다.
 */
inline smlScn smxlGetAgableTbsScn( smlTbsId   aTbsId,
                                   smlEnv   * aEnv )
{
    smlScn sAgableScn;

    /**
     * DDL statement는 snapshot iterator를 획득하지 않은 상태에서
     * dictionary tablespace를 조회하기 때문에 scan 대상이 되는
     * 레코드가 aging될수 있다.
     * 따라서 dictionary tablespace는 agable statement scn으로
     * agable 여부를 검사해야 한다.
     */
    
    if( aTbsId == SML_MEMORY_DICT_SYSTEM_TBS_ID )
    {
        sAgableScn = smxlGetAgableStmtScn( aEnv );
    }
    else
    {
        sAgableScn = smxlGetAgableScn( aEnv );
    }

    return sAgableScn;
}

inline smlScn smxlGetAgableScn( smlEnv * aEnv )
{
    smlScn  sAgableScn;
    
    /**
     * recovery 시에 non-active transaction은 무조건 aging가능하다.
     */
    if( (smfGetServerState() == SML_SERVER_STATE_RECOVERING) ||
        (smrProceedMediaRecovery(aEnv) == STL_TRUE) )
    {
        sAgableScn = SML_MAXIMUM_STABLE_SCN;
        STL_THROW( RAMP_SUCCESS );
    }
    
    if( gSmxlWarmupEntry->mAgerState == SML_AGER_STATE_ACTIVE )
    {
        sAgableScn = gSmxlWarmupEntry->mAgableScn;
    }
    else
    {
        /**
         * database creation
         */
        sAgableScn = smxlGetSystemScn();
    }

    STL_RAMP( RAMP_SUCCESS );
    
    return sAgableScn;
}

inline smlScn smxlGetAgableStmtScn( smlEnv * aEnv )
{
    smlScn sAgableStmtScn;
    
    /**
     * recovery 시에 non-active transaction은 무조건 aging가능하다.
     */
    if( (smfGetServerState() == SML_SERVER_STATE_RECOVERING) ||
        (smrProceedMediaRecovery(aEnv) == STL_TRUE) )
    {
        sAgableStmtScn = SML_MAXIMUM_STABLE_SCN;
        STL_THROW( RAMP_SUCCESS );
    }
    
    if( gSmxlWarmupEntry->mAgerState == SML_AGER_STATE_ACTIVE )
    {
        sAgableStmtScn = gSmxlWarmupEntry->mAgableStmtScn;
    }
    else
    {
        /**
         * database creation
         */
        sAgableStmtScn = smxlGetSystemScn();
    }

    STL_RAMP( RAMP_SUCCESS );
    
    return sAgableStmtScn;
}

inline smlScn smxlGetAgableViewScn( smlEnv * aEnv )
{
    smlScn  sAgableViewScn;

    /**
     * recovery 시에 non-active transaction은 무조건 aging가능하다.
     */
    if( smfGetServerState() == SML_SERVER_STATE_RECOVERING )
    {
        sAgableViewScn = SML_MAXIMUM_STABLE_SCN;
        STL_THROW( RAMP_SUCCESS );
    }
    
    if( gSmxlWarmupEntry->mAgerState == SML_AGER_STATE_ACTIVE )
    {
        sAgableViewScn = gSmxlWarmupEntry->mAgableViewScn;
    }
    else
    {
        /**
         * database creation
         */
        sAgableViewScn = smxlGetSystemScn();
    }

    STL_RAMP( RAMP_SUCCESS );
    
    return sAgableViewScn;
}

inline smlScn smxlGetMinTransViewScn( smlEnv * aEnv )
{
    smlScn  sMinTransViewScn;

    /**
     * recovery 시에 non-active transaction은 무조건 aging가능하다.
     */
    if( smfGetServerState() == SML_SERVER_STATE_RECOVERING )
    {
        sMinTransViewScn = SML_MAXIMUM_STABLE_SCN;
    }
    else
    {
        if( gSmxlWarmupEntry->mAgerState == SML_AGER_STATE_ACTIVE )
        {
            sMinTransViewScn = gSmxlWarmupEntry->mMinTransViewScn;
        }
        else
        {
            /**
             * database creation
             */
            sMinTransViewScn = smxlGetSystemScn();
        }
    }
    
    return sMinTransViewScn;
}

stlStatus smxlGetCommitScn( smxlTransId   aTransId,
                            smlScn        aTransViewScn,
                            smlScn      * aCommitScn,
                            smlEnv      * aEnv )
{
    smpHandle         sPageHandle;
    smxlTransRecord   sTransRecord;
    stlInt32          sState = 0;
    smlRid            sTransRid;
    smlScn            sAgableViewScn;
    smlScn            sCommitScn;
    stlBool           sIsSuccess = STL_TRUE;
    stlInt64        * sRecycleNo;
    stlInt64          sRecycleNoB;
    stlInt16          sUndoRecSize;
    stlInt64          sSpinCount = 0;
    smlScn            sAgableScn;

    STL_ASSERT( SMXL_IS_UNDO_BOUND_TRANS( aTransId ) == STL_TRUE );
    sTransRid = SMXL_TRANS_RID( aTransId );
    
    *aCommitScn = 0;

    STL_TRY( smxlGetSafeCommitScn( aTransId,
                                   aTransViewScn,
                                   &sCommitScn,
                                   &sIsSuccess,
                                   aEnv )
             == STL_SUCCESS );

    if( sIsSuccess == STL_TRUE )
    {
        *aCommitScn = sCommitScn;
        STL_THROW( RAMP_SUCCESS );
    }
        
    sAgableViewScn = smxlGetAgableViewScn( aEnv );

    if( SMXL_IS_AGABLE_VIEW_SCN( sAgableViewScn, aTransViewScn ) == STL_FALSE )
    {
        /**
         * Commit된 Tranaction Record는 SHARED LATCH로 읽는다.
         */
        STL_TRY( smpFix( sTransRid.mTbsId,
                         sTransRid.mPageId,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );
        sState = 1;

        sSpinCount = stlGetMaxSpinCount();

        while( 1 )
        {
            sRecycleNo = (stlInt64*)smpGetVolatileArea( &sPageHandle );
            sRecycleNoB = sRecycleNo[1];
            stlMemBarrier();

            STL_TRY( smxlGetUnsafeUndoRecord( &sPageHandle,
                                              &sTransRid,
                                              SMR_LOG_INSERT_TRANSACTION_RECORD,
                                              SMG_COMPONENT_TRANSACTION,
                                              (stlChar*)&sTransRecord,
                                              STL_SIZEOF(smxlTransRecord),
                                              &sIsSuccess,
                                              &sUndoRecSize ) == STL_SUCCESS );

            STL_TRY_THROW( (sIsSuccess == STL_TRUE) && (sUndoRecSize == STL_SIZEOF(smxlTransRecord)),
                           RAMP_SKIP );

            sAgableScn = smpGetAgableScn( &sPageHandle );

            stlMemBarrier();
            if( sRecycleNoB == sRecycleNo[0] )
            {
                break;
            }

            sSpinCount--;

            if( sSpinCount == 0 )
            {
                stlBusyWait();
                sSpinCount = stlGetMaxSpinCount();
            }
        }

        if( aTransViewScn >= sAgableScn )
        {
            /**
             * 동시성 문제로 Transaction View Scn이 같고, Commit Scn이 다르다고 하더라도
             * 이전 Commit Scn을 자겨가면 문제가 발생하지 않는다.
             */
            if( sTransRecord.mTransViewScn == aTransViewScn )
            {
                *aCommitScn = sTransRecord.mCommitScn;
            }
        }
        
        STL_RAMP( RAMP_SKIP );

        sState = 0;
        STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
    }    

    STL_RAMP( RAMP_SUCCESS );

    STL_DASSERT( (*aCommitScn == SML_INFINITE_SCN) ||
                 (*aCommitScn == SML_MAXIMUM_STABLE_SCN) ||
                 (*aCommitScn <= smxlGetSystemScn()) );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpUnfix( &sPageHandle, aEnv );
    }

    return STL_FAILURE;
}
    
smlScn smxlGetSystemScn()
{
    return gSmxlWarmupEntry->mSystemScn;
}

void smxlGetAtomicSystemScn( smlScn * aScn )
{
    while( 1 )
    {
        *aScn = gSmxlWarmupEntry->mSystemScn;
        stlMemBarrier();

        if( *aScn == gSmxlWarmupEntry->mSystemScn )
        {
            break;
        }
    }
}

smlScn smxlGetNextSystemScn()
{
    return stlAtomicInc64( &gSmxlWarmupEntry->mSystemScn ) + 1;
}

void smxlSetSystemScn( smlScn aSystemScn )
{
    if( gSmxlWarmupEntry->mSystemScn < aSystemScn )
    {
        gSmxlWarmupEntry->mSystemScn = aSystemScn;
    }
}

void smxlGetSystemInfo( smxlSystemInfo * aSystemInfo )
{
    aSystemInfo->mSystemScn = gSmxlWarmupEntry->mSystemScn;
    aSystemInfo->mMinTransViewScn = gSmxlWarmupEntry->mMinTransViewScn;
    aSystemInfo->mAgableScn = gSmxlWarmupEntry->mAgableScn;
    aSystemInfo->mAgableStmtScn = gSmxlWarmupEntry->mAgableStmtScn;
    aSystemInfo->mAgableViewScn = gSmxlWarmupEntry->mAgableViewScn;
    aSystemInfo->mTryStealUndoCount = gSmxlWarmupEntry->mTryStealUndoCount;
    aSystemInfo->mDataStoreMode = smxlGetDataStoreMode();
    aSystemInfo->mTransTableSize = gSmxlWarmupEntry->mTransTableSize;
    aSystemInfo->mUndoRelCount = gSmxlWarmupEntry->mUndoRelCount;
}

smlScn smxlGetCommitScnFromLog( stlChar * aCommitLog )
{
    smxlTransRecord sTransRecord;

    STL_WRITE_BYTES( &sTransRecord,
                     aCommitLog,
                     STL_SIZEOF( smxlTransRecord ) );

    return sTransRecord.mCommitScn;
}

smxlTransId smxlGetTransIdFromLog( stlChar * aCommitLog )
{
    smxlTransRecord sTransRecord;

    STL_WRITE_BYTES( &sTransRecord,
                     aCommitLog,
                     STL_SIZEOF( smxlTransRecord ) );

    return sTransRecord.mTransId;
}

smlScn smxlGetTransViewScnFromLog( stlChar * aCommitLog )
{
    smxlTransRecord sTransRecord;

    STL_WRITE_BYTES( &sTransRecord,
                     aCommitLog,
                     STL_SIZEOF( smxlTransRecord ) );

    return sTransRecord.mTransViewScn;
}

stlStatus smxlSetAgerState( stlInt16   aState,
                            smlEnv   * aEnv )
{
    gSmxlWarmupEntry->mAgerState = aState;
    return STL_SUCCESS;
}

stlStatus smxlBuildAgableScn( smlEnv * aEnv )
{
    smlSessionEnv     * sSessionEnv;
    stlBool             sUsed;
    stlBool             sInitialized;
    smxlTransSnapshot   sTransSnapshot;
    
    smlScn              sDisabledMinViewScn;
    smlScn              sTransViewScn;
    smlScn              sSystemScn;
    smxlTransId         sTransId;
    
    smlScn              sMinSnapshotStmtViewScn;
    smlScn              sMinStmtViewScn;
    smlScn              sMinTransViewScn;
    
    smlScn              sGlobalMinTransViewScn;
    smlScn              sGlobalAgableViewScn;
    smlScn              sGlobalAgableScn;
    smlScn              sGlobalAgableStmtScn;
    stlInt64            sActiveTransCount = 0;
    stlInt32            sState = 0;
    stlBool             sTimeout = STL_FALSE;

    STL_TRY( knlAcquireLatch( &(gSmxlWarmupEntry->mLatch),
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,   /* interval */
                              & sTimeout,
                              KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;
    
    sSystemScn = smxlGetSystemScn();
    sGlobalAgableScn = sSystemScn;
    sGlobalAgableStmtScn = sSystemScn;
    sGlobalMinTransViewScn = sSystemScn;
    sGlobalAgableViewScn = sSystemScn;

    sSessionEnv = (smlSessionEnv*)knlGetFirstSessionEnv();

    while( sSessionEnv != NULL )
    {
        sUsed = knlIsUsedSessionEnv( (knlSessionEnv*)sSessionEnv );
        sInitialized = knlGetInitializedFlag( (knlSessionEnv*)sSessionEnv,
                                              STL_LAYER_STORAGE_MANAGER );
        
        if( (sUsed == STL_TRUE) && (sInitialized == STL_TRUE) )
        {
            sMinSnapshotStmtViewScn = sSessionEnv->mMinSnapshotStmtViewScn;
            sMinStmtViewScn = sSessionEnv->mMinStmtViewScn;
            sDisabledMinViewScn = sSessionEnv->mDisabledViewScn;
            sMinTransViewScn = sSessionEnv->mMinTransViewScn;
            sTransViewScn = sSessionEnv->mTransViewScn; 

            /**
             * Minimum Snapshot Statement View Scn
             */
            sMinSnapshotStmtViewScn = STL_MIN( sDisabledMinViewScn, sMinSnapshotStmtViewScn );

            /**
             * Agable Scn 갱신
             */
            sGlobalAgableScn = STL_MIN( sGlobalAgableScn, sMinSnapshotStmtViewScn );
            STL_DASSERT( gSmxlWarmupEntry->mAgableScn <= sGlobalAgableScn );

            /**
             * Minimum Statement View Scn
             */
            sMinStmtViewScn = STL_MIN( sDisabledMinViewScn, sMinStmtViewScn );

            /**
             * Minimum Transaction View Scn
             */
            sGlobalMinTransViewScn = STL_MIN( sGlobalMinTransViewScn, sTransViewScn ); 

            /**
             * Agable Stmt Scn 갱신
             */
            sGlobalAgableStmtScn = STL_MIN( sGlobalAgableStmtScn, sMinStmtViewScn );
            STL_DASSERT( gSmxlWarmupEntry->mAgableStmtScn <= sGlobalAgableStmtScn );

            /**
             * Minimum of Minimum Transaction View Scn
             * - 동시성 문제로 Session에서 설정된 MinTransViewScn이 현재
             *   AgableViewScn 보다 작게 설정될수 있다. 이러한 경우는
             *   무시한다.
             */
            if( gSmxlWarmupEntry->mAgableViewScn <= sMinTransViewScn )
            {
                sGlobalAgableViewScn = STL_MIN( sGlobalAgableViewScn, sMinTransViewScn );
            }
        }
        
        sSessionEnv = (smlSessionEnv*)knlGetNextSessionEnv( (knlSessionEnv*)sSessionEnv );
    }

    /**
     * Global transaction을 고려해 Transaction table을 검사한다.
     */
    
    if( (knlGetCurrStartupPhase() == KNL_STARTUP_PHASE_OPEN) ||
        (smrGetRecoveryPhase() == SMR_RECOVERY_PHASE_DONE) )
    {
        STL_TRY( smxlGetFirstActiveTrans( &sTransId,
                                          &sTransSnapshot,
                                          aEnv ) == STL_SUCCESS );

        while( sTransId != SML_INVALID_TRANSID )
        {
            sActiveTransCount++;
            
            /**
             * Transaction들의 Minimum Transaction View Scn을 구한다.
             * - Free Page의 Agable 여부를 판단하기 위해서 사용된다.
             */
            
            sGlobalMinTransViewScn = STL_MIN( sGlobalMinTransViewScn, sTransSnapshot.mTransViewScn );
            STL_DASSERT( gSmxlWarmupEntry->mMinTransViewScn <= sGlobalMinTransViewScn );

            /**
             * serializable transaction이 참조하는 레코드가 삭제되어서는 안된다.
             */
            
            if( sTransSnapshot.mIsolationLevel == SML_TIL_SERIALIZABLE )
            {
                sGlobalAgableScn = STL_MIN( sGlobalAgableScn, sTransSnapshot.mTransViewScn );
                sGlobalAgableStmtScn = STL_MIN( sGlobalAgableStmtScn, sTransSnapshot.mTransViewScn );
            }
            
            STL_TRY( smxlGetNextActiveTrans( &sTransId,
                                             &sTransSnapshot,
                                             aEnv ) == STL_SUCCESS );
        }

        STL_DASSERT( gSmxlWarmupEntry->mAgableScn <= sGlobalAgableScn );
        STL_DASSERT( gSmxlWarmupEntry->mAgableStmtScn <= sGlobalAgableStmtScn );
        STL_DASSERT( gSmxlWarmupEntry->mMinTransViewScn <= sGlobalMinTransViewScn );
        STL_DASSERT( gSmxlWarmupEntry->mAgableViewScn <= gSmxlWarmupEntry->mMinTransViewScn );
        STL_DASSERT( gSmxlWarmupEntry->mAgableViewScn <=
                     STL_MIN( sGlobalMinTransViewScn, sGlobalAgableViewScn ) );
    }
    else
    {
        sGlobalMinTransViewScn = SMXL_INIT_SYSTEM_SCN;
    }
    
    gSmxlWarmupEntry->mAgableScn = sGlobalAgableScn;
    gSmxlWarmupEntry->mAgableStmtScn = STL_MIN( gSmxlWarmupEntry->mAgableScn,
                                                sGlobalAgableStmtScn );
    
    gSmxlWarmupEntry->mMinTransViewScn = STL_MIN( gSmxlWarmupEntry->mAgableStmtScn,
                                                  sGlobalMinTransViewScn );
    
    /**
     * restarted prepare transaction은 session에 MinTransViewScn을 유지할수 없다.
     * 트랜잭션으로부터 정보를 얻어야 한다.
     */
    
    gSmxlWarmupEntry->mAgableViewScn = STL_MIN( gSmxlWarmupEntry->mMinTransViewScn,
                                                sGlobalAgableViewScn );

    sState = 0;
    STL_TRY( knlReleaseLatch( &(gSmxlWarmupEntry->mLatch),
                              KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) knlReleaseLatch( &(gSmxlWarmupEntry->mLatch), KNL_ENV(aEnv) );
    }

    STL_ASSERT( STL_FALSE );

    return STL_SUCCESS;
}

stlStatus smxlBuildAgableScn4Restart( smlEnv * aEnv )
{
    /**
     * 강제적으로 AgableViewScn을 갱신하기 위해서는 System scn을 증가시킬 필요가 있다.
     */
    (void)smxlGetNextSystemScn();
    STL_TRY( smlLoopbackAger( STL_TRUE,    /* build agable scn */
                              aEnv ) == STL_SUCCESS );
    
    /**
     * 강제적으로 AgableViewScn을 갱신하기 위해서는 System scn을 증가시킬 필요가 있다.
     */
    (void)smxlGetNextSystemScn();
    STL_TRY( smlLoopbackAger( STL_TRUE,    /* build agable scn */
                              aEnv ) == STL_SUCCESS );
    
    STL_ASSERT( gSmxlWarmupEntry->mAgableViewScn > 1 );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */

/**
 * @addtogroup smxlScnMgr
 * @{
 */

stlStatus smxlWriteCommitLog( smxlTransId   aTransId,
                              smlScn        aCommitScn,
                              stlBool       aIsCommit,
                              stlChar     * aComment,
                              stlInt64      aWriteMode,
                              smrSbsn     * aCommitSbsn,
                              smrLsn      * aCommitLsn,
                              smlEnv      * aEnv )
{
    smxlUndoRelCache * sUndoRelCache = NULL;
    smxlUndoRelEntry * sUndoRelEntry;
    smlRid             sUndoSegRid;
    smxlTransRecord    sTransRecord;
    stlInt16           sLogType;
    smlRid             sTransRid;
    smrLogPieceHdr     sLogPieceHdr;
    stlChar            sLogBody[SMR_LOGPIECE_HDR_SIZE + STL_SIZEOF(smxlTransRecord) + SML_MAX_COMMIT_COMMENT_SIZE];
    smxlTrans        * sTrans;
    smrSbsn            sCommitSbsn = SMR_INVALID_SBSN;
    smrLsn             sCommitLsn = SMR_INVALID_LSN;
    smpHandle          sPageHandle;
    stlInt32           sState = 0;
    stlChar          * sUndoRecord;
    stlInt32           sOffset = 0;
    stlInt16           sCommentSize = 0;

    sTrans = SMXL_TRANS_SLOT( aTransId );
    sUndoRelEntry = (smxlUndoRelEntry*)smxlGetUndoRelEntry( aTransId );

    /**
     * 한번이라도 로깅을 시도한적이 있다면 Transaction Record에 Commit Scn을
     * 설정해야 한다.
     */
    STL_TRY_THROW( sTrans->mTryLogCount > 0, RAMP_SUCCESS );
    
    if( aIsCommit == STL_TRUE )
    {
        sLogType = SMR_LOG_COMMIT;
    }
    else
    {
        sLogType = SMR_LOG_ROLLBACK;
    }
    
    if( smxlNeedRollback( aTransId ) == STL_FALSE )
    {
        sTransRid = SML_INVALID_RID;
        sUndoSegRid = SML_INVALID_RID;
    }
    else
    {
        sUndoRelCache = (smxlUndoRelCache*)smxlGetUndoRelHandle( sUndoRelEntry );
        sUndoSegRid = smsGetSegRid( sUndoRelCache->mSegHandle );

        STL_ASSERT( SMXL_IS_UNDO_BOUND_TRANS( aTransId ) == STL_TRUE );
        sTransRid = SMXL_TRANS_RID( aTransId );
        
        STL_TRY( smpAcquire( NULL,
                             sTransRid.mTbsId,
                             sTransRid.mPageId,
                             KNL_LATCH_MODE_EXCLUSIVE,
                             &sPageHandle,
                             aEnv ) == STL_SUCCESS );
        sState = 1;
    }

    /**
     * Data Store Mode가 TDS 라면 트랜잭션 Commit/rollback 로그를 기록한다.
     * - CDS 는 REDO 로그를 기록할 필요가 없다.
     * - READ ONLY DATABASE는 로그를 기록할 필요없다.
     * - 불완전 복구 시에는 로그를 기록하지 않는다.
     */
    if( (smxlGetDataStoreMode() == SML_DATA_STORE_MODE_TDS) &&
        (smfGetDataAccessMode() == SML_DATA_ACCESS_MODE_READ_WRITE) &&
        (smrProceedIncompleteMediaRecovery(aEnv) == STL_FALSE) )
    {
        if( aComment != NULL )
        {
            sCommentSize = stlStrlen( aComment );
        }
        
        sLogPieceHdr.mType = sLogType;
        sLogPieceHdr.mComponentClass = SMG_COMPONENT_TRANSACTION;
        sLogPieceHdr.mRedoTargetType = SMR_REDO_TARGET_TRANSACTION;
        SMG_WRITE_RID( &sLogPieceHdr.mDataRid, &sTransRid );
        sLogPieceHdr.mSize = STL_SIZEOF( smxlTransRecord ) + sCommentSize;
        sLogPieceHdr.mPropagateLog = SMXL_TRANS_PROPAGATE_LOG( aTransId );
        sLogPieceHdr.mRepreparable = STL_TRUE;
        
        sTransRecord.mTransId = sTrans->mTransId;
        sTransRecord.mCommitScn = aCommitScn;
        sTransRecord.mTransViewScn = sTrans->mTransViewScn;
        sTransRecord.mGlobalTrans = sTrans->mIsGlobal;
        sTransRecord.mCommitTime = knlGetSystemTime();
        sTransRecord.mCommentSize = sCommentSize;
        stlMemset( sTransRecord.mPadding, 0x00, 5 );

        SMR_WRITE_MOVE_LOGPIECE_HDR( (stlChar*)sLogBody, &sLogPieceHdr, sOffset );
        STL_WRITE_MOVE_BYTES( (stlChar*)sLogBody,
                              &sTransRecord,
                              STL_SIZEOF( smxlTransRecord ),
                              sOffset );
        
        if( aComment != NULL )
        {
            STL_WRITE_MOVE_BYTES( (stlChar*)sLogBody, aComment, sCommentSize, sOffset );
        }

        /**
         * Page의 CommitScn이 기록되지 이전에 로깅되어야 한다.
         */
        
        STL_TRY( smrWriteLog( aTransId,
                              sLogBody,
                              sOffset,
                              1,
                              sUndoSegRid,
                              ((aWriteMode == SML_TRANSACTION_CWM_WAIT) ? STL_TRUE : STL_FALSE),
                              &sCommitSbsn,
                              &sCommitLsn,
                              aEnv ) == STL_SUCCESS );
    }
    else
    {
        sCommitLsn = smrGetSystemLsn();
    }

    if( sUndoRelCache != NULL )
    {
        STL_DASSERT( sCommitLsn != SMR_INVALID_LSN );

        /**
         * Transaction record의 CommitScn 은 Logging 기록 이후에 설정되어야 한다.
         * - Log가 기록되지 않은 Transaction이 Commit되었다고 판단될수 있으며,
         *   이러한 경우는 restart recovery시 공간 할당에 문제를 발생시킬 소지가 있다.
         */

        sUndoRecord = smpGetNthRowItem( &sPageHandle, sTransRid.mOffset );

        STL_WRITE_BYTES( ( sUndoRecord +
                           SMXL_UNDO_REC_HDR_SIZE +
                           STL_OFFSETOF( smxlTransRecord, mCommitScn ) ),
                         &aCommitScn,
                         STL_SIZEOF( smlScn ) );
        
        smpSetRecoveryLsn( &sPageHandle,
                           sCommitSbsn,
                           sCommitLsn,
                           SMR_MAX_LPSN );
        STL_TRY( smpSetDirty( &sPageHandle, aEnv ) == STL_SUCCESS );

        sState = 0;
        STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
    }

    if( aCommitSbsn != NULL )
    {
        *aCommitSbsn = sCommitSbsn;
    }

    if( aCommitLsn != NULL )
    {
        *aCommitLsn = sCommitLsn;
    }

    if( smrIsLogFlusherActive() == STL_TRUE )
    {
        if( aWriteMode == SML_TRANSACTION_CWM_WAIT )
        {
            STL_TRY( smrWakeupFlusher( aEnv ) == STL_SUCCESS );
        }
    }
        
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) smpRelease( &sPageHandle, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smxlWriteInDoubtCommitLog( smxlTransId   aTransId,
                                     smxlTransId   aInDoubtTransId,
                                     smlScn        aCommitScn,
                                     smlScn        aInDoubtCommitScn,
                                     stlBool       aIsInDoubtCommit,
                                     stlChar     * aComment,
                                     smrSbsn     * aCommitSbsn,
                                     smrLsn      * aCommitLsn,
                                     smrSbsn     * aInDoubtCommitSbsn,
                                     smrLsn      * aInDoubtCommitLsn,
                                     smlEnv      * aEnv )
{
    smxlUndoRelCache * sUndoRelCache;
    smxlUndoRelEntry * sUndoRelEntry;
    smlRid             sUndoSegRid;
    smxlTransRecord    sTransRecord;
    stlInt16           sLogType;
    smlRid             sTransRid;
    stlInt32           sState = 0;
    smlRid             sInDoubtTransRid;
    smpHandle          sPageHandle;
    smpHandle          sInDoubtPageHandle;
    smrLogPieceHdr     sLogPieceHdr;
    stlChar            sLogBody[(SMR_LOGPIECE_HDR_SIZE + STL_SIZEOF(smxlTransRecord) + SML_MAX_COMMIT_COMMENT_SIZE)*2];
    smxlTrans        * sTrans;
    smxlTrans        * sInDoubtTrans;
    smrSbsn            sCommitSbsn = SMR_INVALID_SBSN;
    smrLsn             sCommitLsn = SMR_INVALID_LSN;
    stlInt64           sLogOffset = 0;
    stlChar          * sUndoRecord;
    stlInt16           sCommentSize = 0;
    
    sTrans = SMXL_TRANS_SLOT( aTransId );
    sInDoubtTrans = SMXL_TRANS_SLOT( aInDoubtTransId );
    sUndoRelEntry = (smxlUndoRelEntry*)smxlGetUndoRelEntry( aTransId );

    STL_DASSERT( sTrans->mTryLogCount > 0 );
    
    if( aIsInDoubtCommit == STL_TRUE )
    {
        sLogType = SMR_LOG_COMMIT;
    }
    else
    {
        sLogType = SMR_LOG_ROLLBACK;
    }
    
    sUndoRelCache = (smxlUndoRelCache*)smxlGetUndoRelHandle( sUndoRelEntry );
    STL_DASSERT( sUndoRelCache != NULL );
    sUndoSegRid = smsGetSegRid( sUndoRelCache->mSegHandle );

    STL_ASSERT( SMXL_IS_UNDO_BOUND_TRANS( aTransId ) == STL_TRUE );
    STL_ASSERT( SMXL_IS_UNDO_BOUND_TRANS( aInDoubtTransId ) == STL_TRUE );
    
    sTransRid = SMXL_TRANS_RID( aTransId );
    sInDoubtTransRid = SMXL_TRANS_RID( aInDoubtTransId );

    STL_TRY( smpAcquire( NULL,
                         sTransRid.mTbsId,
                         sTransRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( smpAcquire( NULL,
                         sInDoubtTransRid.mTbsId,
                         sInDoubtTransRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sInDoubtPageHandle,
                         aEnv ) == STL_SUCCESS );
    sState = 2;

    sUndoRecord = smpGetNthRowItem( &sPageHandle, sTransRid.mOffset );

    STL_WRITE_BYTES( ( sUndoRecord +
                       SMXL_UNDO_REC_HDR_SIZE +
                       STL_OFFSETOF( smxlTransRecord, mCommitScn ) ),
                     &aCommitScn,
                     STL_SIZEOF( smlScn ) );
    
    sUndoRecord = smpGetNthRowItem( &sInDoubtPageHandle, sInDoubtTransRid.mOffset );

    STL_WRITE_BYTES( ( sUndoRecord +
                       SMXL_UNDO_REC_HDR_SIZE +
                       STL_OFFSETOF( smxlTransRecord, mCommitScn ) ),
                     &aInDoubtCommitScn,
                     STL_SIZEOF( smlScn ) );
    
    /**
     * Data Store Mode가 TDS 라면 트랜잭션 Commit/rollback 로그를 기록한다.
     * - CDS / DS 는 REDO 로그를 기록할 필요가 없다.
     * - READ ONLY DATABASE는 로그를 기록할 필요없다.
     */
    if( (smxlGetDataStoreMode() == SML_DATA_STORE_MODE_TDS) &&
        (smfGetDataAccessMode() == SML_DATA_ACCESS_MODE_READ_WRITE) )
    {
        sLogPieceHdr.mType = SMR_LOG_COMMIT;
        sLogPieceHdr.mComponentClass = SMG_COMPONENT_TRANSACTION;
        sLogPieceHdr.mRedoTargetType = SMR_REDO_TARGET_TRANSACTION;
        SMG_WRITE_RID( &sLogPieceHdr.mDataRid, &sTransRid );
        sLogPieceHdr.mSize = STL_SIZEOF( smxlTransRecord );
        sLogPieceHdr.mPropagateLog = SMXL_TRANS_PROPAGATE_LOG( aTransId );
        sLogPieceHdr.mRepreparable = STL_TRUE;
        
        sTransRecord.mTransId = sTrans->mTransId;
        sTransRecord.mCommitScn = aCommitScn;
        sTransRecord.mTransViewScn = sTrans->mTransViewScn;
        sTransRecord.mGlobalTrans = sTrans->mIsGlobal;
        sTransRecord.mCommitTime = knlGetSystemTime();
        sTransRecord.mCommentSize = 0;
        stlMemset( sTransRecord.mPadding, 0x00, 5 );

        SMR_WRITE_MOVE_LOGPIECE_HDR( (stlChar*)sLogBody, &sLogPieceHdr, sLogOffset );
        STL_WRITE_MOVE_BYTES( (stlChar*)sLogBody,
                              &sTransRecord,
                              STL_SIZEOF( smxlTransRecord ),
                              sLogOffset );
        
        if( aComment != NULL )
        {
            sCommentSize = stlStrlen( aComment );
        }
        
        sLogPieceHdr.mType = sLogType;
        sLogPieceHdr.mComponentClass = SMG_COMPONENT_TRANSACTION;
        sLogPieceHdr.mRedoTargetType = SMR_REDO_TARGET_TRANSACTION;
        SMG_WRITE_RID( &sLogPieceHdr.mDataRid, &sInDoubtTransRid );
        sLogPieceHdr.mSize = STL_SIZEOF( smxlTransRecord ) + sCommentSize;
        sLogPieceHdr.mPropagateLog = SMXL_TRANS_PROPAGATE_LOG( aInDoubtTransId );
        sLogPieceHdr.mRepreparable = STL_TRUE;
        
        sTransRecord.mTransId = sInDoubtTrans->mTransId;
        sTransRecord.mCommitScn = aInDoubtCommitScn;
        sTransRecord.mTransViewScn = sInDoubtTrans->mTransViewScn;
        sTransRecord.mGlobalTrans = sInDoubtTrans->mIsGlobal;
        sTransRecord.mCommentSize = sCommentSize;
        sTransRecord.mCommitTime = knlGetSystemTime();
        stlMemset( sTransRecord.mPadding, 0x00, 5 );

        SMR_WRITE_MOVE_LOGPIECE_HDR( (stlChar*)sLogBody, &sLogPieceHdr, sLogOffset );
        STL_WRITE_MOVE_BYTES( (stlChar*)sLogBody,
                              &sTransRecord,
                              STL_SIZEOF( smxlTransRecord ),
                              sLogOffset );
        
        if( aComment != NULL )
        {
            STL_WRITE_MOVE_BYTES( (stlChar*)sLogBody, aComment, sCommentSize, sLogOffset );
        }

        STL_TRY( smrWriteLog( aTransId,
                              sLogBody,
                              sLogOffset,
                              2,
                              sUndoSegRid,
                              STL_TRUE,   /* aSwitchBlock */
                              &sCommitSbsn,
                              &sCommitLsn,
                              aEnv ) == STL_SUCCESS );
    }
    else
    {
        sCommitLsn = smrGetSystemLsn();
    }

    smpSetRecoveryLsn( &sInDoubtPageHandle,
                       sCommitSbsn,
                       sCommitLsn,
                       SMR_MAX_LPSN );
    smpSetRecoveryLsn( &sPageHandle,
                       sCommitSbsn,
                       sCommitLsn,
                       SMR_MAX_LPSN );
    
    STL_TRY( smpSetDirty( &sInDoubtPageHandle, aEnv ) == STL_SUCCESS );
    STL_TRY( smpSetDirty( &sPageHandle, aEnv ) == STL_SUCCESS );

    sState = 1;
    STL_TRY( smpRelease( &sInDoubtPageHandle, aEnv ) == STL_SUCCESS );
    sState = 0;
    STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
    
    if( aCommitSbsn != NULL )
    {
        *aCommitSbsn = sCommitSbsn;
    }

    if( aCommitLsn != NULL )
    {
        *aCommitLsn = sCommitLsn;
    }

    if( aInDoubtCommitSbsn != NULL )
    {
        *aInDoubtCommitSbsn = sCommitSbsn;
    }

    if( aInDoubtCommitLsn != NULL )
    {
        *aInDoubtCommitLsn = sCommitLsn;
    }

    if( smrIsLogFlusherActive() == STL_TRUE )
    {
        STL_TRY( smrWakeupFlusher( aEnv ) == STL_SUCCESS );
    }
        
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) smpRelease( &sInDoubtPageHandle, aEnv );
        case 1:
            (void) smpRelease( &sInDoubtPageHandle, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}

/** @} */
