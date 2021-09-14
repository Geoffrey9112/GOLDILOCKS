/*******************************************************************************
 * smaStmt.c
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

#include <dtl.h>
#include <sml.h>
#include <smDef.h>
#include <sma.h>
#include <sme.h>
#include <sms.h>
#include <smn.h>
#include <smg.h>
#include <smf.h>
#include <smxl.h>
#include <smkl.h>
#include <smaStmt.h>
#include <smlTable.h>

/**
 * @file smaStmt.c
 * @brief Storage Manager Layer Statement Internal Routines
 */

/**
 * @addtogroup sma
 * @{
 */

/**
 * @brief Statement를 할당 받는다.
 * @param[in] aTransId Transaction Identifier
 * @param[in] aSmlTransId SML Transaction Identifier
 * @param[in] aUpdateRelHandle Updatable Relation Handle (nullable)
 * @param[in] aAttribute Statement 속성
 * @param[in] aLockTimeout Lock Timeout 시간
 * @param[in] aNeedSnapshotIterator Snaphot Iterator가 필요한 Statement 인지 여부
 * @param[out] aStatement 생성된 Statement Handle
 * @param[in,out] aEnv Environment Pointer
 * @see @a aAttribute : smlStatementAttr
 * @remarks @a aUpdateRelHandle은 DML(INSERT/DELETE/UPDATE) Updatable Statement에만 유효하며,
 * <BR> Update 하고자 하는 대상 relation을 의미한다.
 * <BR> aUpdateRelHandle은 Mutating 검사와 Statement Level Visibility 검사 여부를 위해서 사용된다.
 * <BR> 따라서, Mutating 검사와 Statement Level Visibility 검사 여부가 필요없는 DDL이나 SELECT는
 * <BR> aUpdateRelHandle이 NULL일수 있다.
 */
stlStatus smaAllocStatement( smxlTransId     aTransId,
                             smlTransId      aSmlTransId,
                             void          * aUpdateRelHandle,
                             stlInt32        aAttribute,
                             stlInt64        aLockTimeout,
                             stlBool         aNeedSnapshotIterator,
                             smlStatement ** aStatement,
                             smlEnv        * aEnv )
{
    smlStatement    * sStatement;
    smlSessionEnv   * sSessionEnv = SML_SESS_ENV(aEnv);
    stlInt32          sState = 0;
    stlInt32          i;
    stlBool           sFound = STL_FALSE;
    smlScn            sViewScn = SML_INFINITE_SCN;

    if( smfGetDataAccessMode() == SML_DATA_ACCESS_MODE_READ_ONLY )
    {
        STL_TRY_THROW( (aAttribute & SML_STMT_ATTR_READONLY) ==
                       SML_STMT_ATTR_READONLY,
                       RAMP_ERR_READ_ONLY_DATABASE );
    }

#ifdef STL_DEBUG
    if( aAttribute & SML_STMT_ATTR_UPDATABLE )
    {
        if( knlGetCurrStartupPhase() >= KNL_STARTUP_PHASE_OPEN )
        {
            KNL_ENTER_CRITICAL_SECTION( aEnv );
            STL_DASSERT( aTransId != SML_INVALID_TRANSID );
            KNL_LEAVE_CRITICAL_SECTION( aEnv );
        }
    }
#endif

    for( i = 0; i < SML_CACHED_STMT_MAX_COUNT; i++ )
    {
        if( sSessionEnv->mStatement[i]->mIsUsing == STL_FALSE )
        {
            sFound = STL_TRUE;
            break;
        }
    }

    if( sFound == STL_TRUE )
    {
        sSessionEnv->mStatement[i]->mIsUsing = STL_TRUE;
        sStatement = sSessionEnv->mStatement[i];
    }
    else
    {
        STL_TRY( knlAllocDynamicMem( &sSessionEnv->mStatementMem,
                                     STL_SIZEOF( smlStatement ),
                                     (void**)&sStatement,
                                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        sState = 1;

        STL_TRY( knlCreateRegionMem( &sStatement->mStatementMem,
                                     KNL_ALLOCATOR_STORAGE_MANAGER_STATEMENT,
                                     &sSessionEnv->mStatementMem,
                                     KNL_DYNAMIC_MEMORY_TYPE( &sSessionEnv->mStatementMem ),
                                     SMG_STATEMENT_MEM_CHUNK_SIZE,
                                     SMG_STATEMENT_MEM_CHUNK_SIZE,
                                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        sState = 2;

        sStatement->mIsCached = STL_FALSE;
    }

    sStatement->mTransViewScn = smxlGetTransViewScn( aTransId );
    sStatement->mMinTransViewScn = smxlGetMinTransViewScn( aEnv );

    /**
     * mininum transaction view scn을 설정한다.
     * - 해당 statement가 살아 있는 동안 aging하지 못하는 transaction view scn을 설정함.
     */
    if( sSessionEnv->mMinTransViewScn == SML_INFINITE_SCN )
    {
        sSessionEnv->mMinTransViewScn = sStatement->mMinTransViewScn;
    }
    
    if( (aNeedSnapshotIterator == STL_TRUE) &&
        (sSessionEnv->mMinSnapshotStmtViewScn == SML_INFINITE_SCN) )
    {
        /**
         * snapshot iterator이고, 기존 snapshot iterator가 수행하고 있지 않다면
         * 원자적으로 mMinSnapshotStmtViewScn을 갱신한다.
         * 원자적으로 갱신하지 않으면 AgableScn이 역전되는 상황이 발생한다.
         */

        smxlGetViewScn( aTransId, &sSessionEnv->mMinSnapshotStmtViewScn );
        sViewScn = sSessionEnv->mMinSnapshotStmtViewScn;
        sSessionEnv->mMinSnapshotStmtBeginTime = knlGetSystemTime();
        sStatement->mBeginTime = sSessionEnv->mMinSnapshotStmtBeginTime;
        
        if( sSessionEnv->mMinStmtViewScn == SML_INFINITE_SCN )
        {
            sSessionEnv->mMinStmtViewScn = sViewScn;
        }
    }
    else
    {
        if( sSessionEnv->mMinStmtViewScn == SML_INFINITE_SCN )
        {
            smxlGetViewScn( aTransId, &sSessionEnv->mMinStmtViewScn );
            sViewScn = sSessionEnv->mMinStmtViewScn;
        }
        else
        {
            smxlGetViewScn( aTransId, &sViewScn );
        }
            
        sStatement->mBeginTime = knlGetSystemTime();
    }

    sStatement->mScn = sViewScn;
    sStatement->mTransId = smxlGetTransId( aTransId );
    sStatement->mIsolationLevel = smxlGetIsolationLevel( aTransId );
    sStatement->mState = SML_STMT_STATE_ACTIVE;
    sStatement->mAttr = aAttribute;
    sStatement->mLockTimeout = aLockTimeout;
    sStatement->mUndoRid = smxlGetLstUndoRid( aTransId );
    sStatement->mTimestamp = smxlGetNextTimestamp( aTransId );
    sStatement->mResetCount = 0;
    sStatement->mCommitScn = SML_INFINITE_SCN;
    sStatement->mUpdateRelHandle = aUpdateRelHandle;
    sStatement->mNeedSnapshotIterator = aNeedSnapshotIterator;
    sStatement->mSmlTransId = aSmlTransId;
    sStatement->mActiveInstantCount = 0;

    /**
     * 락 리스트상의 Savepoint를 설정한다.
     */
    STL_TRY( smklSavepoint( smxlGetTransId( aTransId ),
                            &sStatement->mLockSavepoint,
                            aEnv ) == STL_SUCCESS );

    STL_RING_INIT_HEADLINK( &sStatement->mIterators,
                            STL_OFFSETOF( smiIterator, mIteratorLink ) );
    STL_RING_INIT_DATALINK( sStatement, STL_OFFSETOF( smlStatement, mStmtLink ) );

    if( aAttribute & SML_STMT_ATTR_UPDATABLE )
    {
        STL_DASSERT( (knlGetCurrStartupPhase() < KNL_STARTUP_PHASE_OPEN) ||
                     (aTransId != SML_INVALID_TRANSID) );
        
        sStatement->mTcn = smxlGetNextTcn( aTransId );
        STL_RING_ADD_LAST( &sSessionEnv->mUpdatableStmt, sStatement );
        
        smxlSetTransAttr( aTransId, (SMXL_ATTR_UPDATABLE | SMXL_ATTR_LOCKABLE) );
    }
    else
    {
        sStatement->mTcn = smxlGetTcn( aTransId );
        STL_ASSERT( aAttribute & SML_STMT_ATTR_READONLY );
        STL_RING_ADD_LAST( &sSessionEnv->mReadOnlyStmt, sStatement );

        if( (aAttribute & SML_STMT_ATTR_LOCKABLE) == SML_STMT_ATTR_LOCKABLE )
        {
            STL_DASSERT( (knlGetCurrStartupPhase() < KNL_STARTUP_PHASE_OPEN) ||
                         (aTransId != SML_INVALID_TRANSID) );
            smxlSetTransAttr( aTransId, SMXL_ATTR_LOCKABLE );
        }
    }

    *aStatement = sStatement;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_READ_ONLY_DATABASE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_DATABASE_IS_READ_ONLY,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)knlDestroyRegionMem( &sStatement->mStatementMem, KNL_ENV(aEnv) );
        case 1:
            (void)knlFreeDynamicMem( &sSessionEnv->mStatementMem,
                                     sStatement,
                                     KNL_ENV( aEnv ) );
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief Statement를 해제한다.
 * @param[in] aStatement 해제할 aStatement
 * @param[in,out] aEnv Environment Pointer
 * @remarks Statement의 메모리 해제는 해당 세션의 모든 Statement가 완료한 이후에 수행된다.
 */
stlStatus smaFreeStatement( smlStatement * aStatement,
                            smlEnv       * aEnv )
{
    smlSessionEnv * sSessionEnv = SML_SESS_ENV(aEnv);
    smlStatement  * sNextStatement = NULL;
    smlStatement  * sStatement = NULL;
    smlScn          sMinStmtViewScn = SML_INFINITE_SCN;
    smlScn          sMinSnapshotStmtViewScn = SML_INFINITE_SCN;
    stlTime         sMinSnapshotStmtBeginTime = STL_INVALID_STL_TIME;
    smlScn          sMinTransViewScn = SML_INFINITE_SCN;

    STL_DASSERT( STL_RING_IS_EMPTY( &aStatement->mIterators ) == 
                 STL_TRUE );
    
    if( aStatement->mAttr & SML_STMT_ATTR_UPDATABLE )
    {
        STL_RING_UNLINK( &sSessionEnv->mUpdatableStmt, aStatement );
    }
    else
    {
        STL_ASSERT( aStatement->mAttr & SML_STMT_ATTR_READONLY );
        STL_RING_UNLINK( &sSessionEnv->mReadOnlyStmt, aStatement );
    }

    /**
     * 세션의 minimum snapshot statement view scn 갱신한다.
     */
    
    if( aStatement->mNeedSnapshotIterator == STL_TRUE )
    {
        STL_RING_FOREACH_ENTRY( &(SML_SESS_ENV(aEnv)->mReadOnlyStmt), sStatement )
        {
            if( sStatement->mNeedSnapshotIterator == STL_TRUE )
            {
                sMinSnapshotStmtViewScn = sStatement->mScn;
                sMinSnapshotStmtBeginTime = sStatement->mBeginTime;
                break;
            }
        }
        
        STL_RING_FOREACH_ENTRY( &(SML_SESS_ENV(aEnv)->mUpdatableStmt), sStatement )
        {
            if( sStatement->mNeedSnapshotIterator == STL_TRUE )
            {
                if( sMinSnapshotStmtViewScn > sStatement->mScn )
                {
                    sMinSnapshotStmtViewScn = sStatement->mScn;
                    sMinSnapshotStmtBeginTime = sStatement->mBeginTime;
                }
                
                break;
            }
        }
        
        sSessionEnv->mMinSnapshotStmtViewScn = sMinSnapshotStmtViewScn;
        sSessionEnv->mMinSnapshotStmtBeginTime = sMinSnapshotStmtBeginTime;
    }

    /**
     * 세션의 minimum statement view scn과 mininum transaction view scn을 갱신한다.
     */
    
    if( STL_RING_IS_EMPTY( &sSessionEnv->mReadOnlyStmt ) == STL_TRUE )
    {
        sMinStmtViewScn = STL_MIN( sMinStmtViewScn, SML_INFINITE_SCN );
    }
    else
    {
        sNextStatement = (smlStatement*)STL_RING_GET_FIRST_DATA( &sSessionEnv->mReadOnlyStmt );
        sMinStmtViewScn = STL_MIN( sMinStmtViewScn, sNextStatement->mScn );
        sMinTransViewScn = STL_MIN( sMinTransViewScn, sNextStatement->mMinTransViewScn );
    }

    if( STL_RING_IS_EMPTY( &sSessionEnv->mUpdatableStmt ) == STL_TRUE )
    {
        sMinStmtViewScn = STL_MIN( sMinStmtViewScn, SML_INFINITE_SCN );
    }
    else
    {
        sNextStatement = (smlStatement*)STL_RING_GET_FIRST_DATA( &sSessionEnv->mUpdatableStmt );
        sMinStmtViewScn = STL_MIN( sMinStmtViewScn, sNextStatement->mScn );
        sMinTransViewScn = STL_MIN( sMinTransViewScn, sNextStatement->mMinTransViewScn );
    }

    if( aStatement->mIsolationLevel != SML_TIL_SERIALIZABLE )
    {
        sSessionEnv->mMinStmtViewScn = sMinStmtViewScn;
        sSessionEnv->mMinTransViewScn = sMinTransViewScn;
    }
            
    /**
     * active instant relation이 존재하면 안됨
     */
    STL_DASSERT( aStatement->mActiveInstantCount == 0 );

    /**
     * Session Env에 할당된 Statement는 재사용한다.
     */ 
    if( aStatement->mIsCached == STL_FALSE )
    {
        STL_TRY( knlDestroyRegionMem( &aStatement->mStatementMem,
                                      KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY( knlFreeDynamicMem( &sSessionEnv->mStatementMem,
                                    aStatement,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        aStatement->mIsUsing = STL_FALSE;

        STL_TRY( knlClearRegionMem( &aStatement->mStatementMem,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smaUpdateStmtState( smlEnv * aEnv )
{
    smlStatement  * sStatement;
    stlBool         sExist = STL_FALSE;
    smlSessionEnv * sSessionEnv = SML_SESS_ENV(aEnv);
    
    
    STL_RING_FOREACH_ENTRY( &sSessionEnv->mUpdatableStmt, sStatement )
    {
        sStatement->mState = SML_STMT_STATE_COMMITTED;
        sExist = STL_TRUE;
    }

    STL_RING_FOREACH_ENTRY( &sSessionEnv->mReadOnlyStmt, sStatement )
    {
        sStatement->mState = SML_STMT_STATE_COMMITTED;
        sExist = STL_TRUE;
    }

    if( sExist == STL_FALSE )
    {
        sSessionEnv->mMinSnapshotStmtViewScn = SML_INFINITE_SCN;
        sSessionEnv->mMinStmtViewScn = SML_INFINITE_SCN;
        sSessionEnv->mMinSnapshotStmtBeginTime = STL_INVALID_STL_TIME;
    }

    return STL_SUCCESS;
}

/**
 * @brief 현재 세션에 살아있는 모든 Statement들을 정리한다.
 * @param[in,out] aEnv Environment Pointer
 * @remarks 해당 함수는 비정상 종료시 Cleanup Thread에 의해서 호출된다.
 * @note Statement에 필요한 메모리는 Heap에서 할당 받기 때문에 다른 Process에서
 *  <BR> 해당 세션의 Heap에 접근할수 없다. Heap의 메모리 leak이 발생하기는 하지만
 *  <BR> 비정상 종료가 자주 발생되지 않는다는 가정에서 이를 허용한다.
 *  <BR> 다만 Ager의 원할한 수행을 위해 Scn만을 무한대로 변경한다.
 */
stlStatus smaCleanupActiveStatements( smlEnv * aEnv )
{
    SML_SESS_ENV(aEnv)->mMinSnapshotStmtViewScn = SML_INFINITE_SCN;
    SML_SESS_ENV(aEnv)->mMinStmtViewScn = SML_INFINITE_SCN;
    SML_SESS_ENV(aEnv)->mMinSnapshotStmtBeginTime = STL_INVALID_STL_TIME;

    return STL_SUCCESS;
}

/**
 * @brief Statement를 롤백한다.
 * @param[in] aStatement 롤백할 aStatement
 * @param[in] aReleaseLock Lock Release 여부
 * @param[in,out] aEnv Environment Pointer
 */
stlStatus smaRollbackStatement( smlStatement * aStatement,
                                stlBool        aReleaseLock,
                                smlEnv       * aEnv )
{
    STL_PARAM_VALIDATE( aStatement != NULL, KNL_ERROR_STACK( aEnv ) );
    
    STL_TRY_THROW( aStatement->mTransId != SML_INVALID_TRANSID, RAMP_SUCCESS );
    STL_TRY_THROW( (aStatement->mAttr & SML_STMT_ATTR_UPDATABLE) ||
                   (aStatement->mAttr & SML_STMT_ATTR_LOCKABLE), RAMP_SUCCESS );
    
    if( smxlNeedRollback( aStatement->mTransId ) == STL_TRUE )
    {
        STL_TRY( smxlRollbackUndoRecords( aStatement->mTransId,
                                          aStatement->mUndoRid,
                                          STL_FALSE,  /* aRestartRollback */
                                          aEnv ) == STL_SUCCESS );
    }

    /**
     * Segment Aging과 Unlock의 동시성 문제로 인하여 Agable Scn의 증가를 막는다.
     * - Unlock 이후에 Enable시킨다.
     */
    smlSetScnToDisableAging( aStatement->mTransId,
                             SML_SESS_ENV(aEnv) );
    
    STL_TRY( smgExecutePendOp( SML_PEND_ACTION_ROLLBACK,
                               aStatement->mTimestamp,
                               aEnv )
             == STL_SUCCESS );

    /**
     * reset statement에서는 lock을 해제하지 않는다.
     * - reset statement 이후 QP에서 Validation & Lock을 수행하지 않는다.
     */
    
    if( aReleaseLock == STL_TRUE )
    {
        STL_TRY( smklUnlockForSavepoint( aStatement->mTransId,
                                         &aStatement->mLockSavepoint,
                                         aEnv )
                 == STL_SUCCESS );
    }

    /**
     * Lock의 해제로 Segment의 aging을 허락한다.
     */
    smlUnsetScnToEnableAging( SML_SESS_ENV(aEnv) );
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    smlUnsetScnToEnableAging( SML_SESS_ENV(aEnv) );

    return STL_FAILURE;
}

/**
 * @brief Statement를 RESET 한다.
 * @param[in] aStatement 롤백할 aStatement
 * @param[in] aDoRollback Rollback 수행 여부
 * @param[in,out] aEnv Environment Pointer
 */
stlStatus smaResetStatement( smlStatement * aStatement,
                             stlBool        aDoRollback,
                             smlEnv       * aEnv )
{
    STL_PARAM_VALIDATE( aStatement != NULL, KNL_ERROR_STACK( aEnv ) );

    /**
     * Dictionary Handle Validation 과정에서의 Reset Statement는
     * Read Only라도 허용한다.
     */
    
    if( aDoRollback == STL_TRUE )
    {
        /**
         * Reset Statement는 Updatable Statement나 Lockable Statement로 제한한다.
         * - qllExecuteSQL 이후에 호출되는 경우
         */
        STL_DASSERT( (aStatement->mAttr & SML_STMT_ATTR_UPDATABLE) ||
                     (aStatement->mAttr & SML_STMT_ATTR_LOCKABLE) );

        /**
         * Reset Statement는 Snapshot Iterator를 필요로 하는 Statement여야 한다.
         * - UPDATE / DELETE / SELECT FOR UPDATE / SELECT WITH SQL_CONCUR_LOCK
         */
        STL_DASSERT( aStatement->mNeedSnapshotIterator == STL_TRUE );

        STL_TRY( smaRollbackStatement( aStatement,
                                       STL_FALSE, /* aReleaseLock */
                                       aEnv )
                 == STL_SUCCESS );
        
        aStatement->mUndoRid = smxlGetLstUndoRid( aStatement->mTransId );
    }

    aStatement->mResetCount += 1;

    /**
     * reset 시에는 session의 minimum statement scn을 변경해서는 안된다.
     * - smaResetStatement()는 QP validation이후에 수행되기 때문에
     *   참조하고 있는 relation이 지워질수 있다.
     */
    smxlGetViewScn( aStatement->mTransId, &aStatement->mScn );

    STL_DASSERT( STL_RING_IS_EMPTY( &(aStatement->mIterators) ) == STL_TRUE );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Iterator를 할당한다.
 * @param[in] aStatement        Iterator가 속하는 Statement
 * @param[in] aRelationHandle   Iterator를 사용하려고 하는 Relation의 Handle
 * @param[in] aTransReadMode    Transaction간 Read Mode
 * @param[in] aStmtReadMode     aStatement내 Read Mode
 * @param[in] aProperty         Iterator의 동작방식에 관련된 속성들
 * @param[in] aScanDirection    Scan 방향
 * @param[out] aIterator        할당되고 초기화된 Iterator의 포인터
 * @param[in,out] aEnv          Storage Manager Environment
 * @remarks TransReadMode와 StmtReadMode는 Iterator가 읽어야할 대상을 선정하기 위해서 사용된다.
 *    <BR>  - SML_TRM_SNAPSHOT  : Older Transaction이 변경한 내용을 읽는다.
 *    <BR>  - SML_TRM_COMMITTED : Older/Elder Transaction이 Commit한 모든 내용을 읽는다.
 *    <BR>  - SML_SRM_SNAPSHOT  : Older Statement에서 변경한 내용만 읽는다.
 *    <BR>  - SML_SRM_RECENT    : Older와 My Statement에서 변경한 내용을 읽는다.
 *    <BR>                        ( Elder Statement는 같은 테이블을 변경할수 없기 때문에 고려할 필요없음 )
 */
stlStatus smaAllocIterator( smlStatement              * aStatement,
                            void                      * aRelationHandle,
                            smlTransReadMode            aTransReadMode,
                            smlStmtReadMode             aStmtReadMode,
                            smlIteratorProperty       * aProperty,
                            smlIteratorScanDirection    aScanDirection,
                            void                     ** aIterator,
                            smlEnv                    * aEnv )
{
    smiIterator       * sIterator;
    smeIteratorModule * sModule;
    knlRegionMarkEx   * sRegionMarkEx;
    stlInt32            sRelationType = SME_GET_RELATION_TYPE( aRelationHandle );
    stlInt32            sState = 0;

    if( aProperty->mGroupFetch == STL_TRUE )
    {
        sModule = (smeIteratorModule *) smeGetGroupIteratorModule( sRelationType );
    }
    else
    {
        sModule = smeGetIteratorModule( aRelationHandle );
    }

    STL_TRY( smlValidateRelation( aRelationHandle,
                                  SME_VALIDATION_BEHAVIOR_READ,
                                  aEnv )
             == STL_SUCCESS );

    STL_PARAM_VALIDATE( aStatement != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRelationHandle != NULL, KNL_ERROR_STACK(aEnv) );

    /**
     * Snapshot Iterator를 사용하지 않는다고 설정했다면
     * aTransReadMode는 SML_TRM_COMMITTED 여야만 한다.
     * - "Statement without Snapshot Iterator"는 AgableScn 계산에 적용되지 않기 때문에
     *   반드시 SML_TRM_COMMITTED를 보장해야 한다.
     */
    STL_DASSERT( (aStatement->mNeedSnapshotIterator == STL_TRUE) ||
                 (aTransReadMode == SML_TRM_COMMITTED) );
    
    STL_TRY( knlMarkRegionMemEx( &aStatement->mStatementMem,
                                 &sRegionMarkEx,
                                 KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( smaAllocRegionMem( aStatement,
                                sModule->mIteratorSize,
                                (void**)&sIterator,
                                aEnv ) == STL_SUCCESS );

    sIterator->mRegionMarkEx    = sRegionMarkEx;
    sIterator->mRelationHandle  = aRelationHandle;
    sIterator->mIteratorModule  = sModule;
    sIterator->mTransReadMode   = aTransReadMode;
    sIterator->mStmtReadMode    = aStmtReadMode; 
    sIterator->mStatement       = aStatement;
    sIterator->mValidObjectScn  = SME_GET_OBJECT_SCN( aRelationHandle );
    sIterator->mProperty        = *aProperty;

    sIterator->mSlotIterator.mPage = NULL;
    sIterator->mSlotIterator.mCurSlot = NULL;
    sIterator->mSlotIterator.mSlotFence = NULL;
    sIterator->mState = SMI_STATE_ACTIVE;

    if( aTransReadMode == SML_TRM_COMMITTED )
    {
        /**
         * smlFetchNext 마다 ViewScn이 상되어야 한다.
         */
        sIterator->mViewScn = SML_INFINITE_SCN;
    }
    else
    {
        /**
         * 자신의 트랜잭션 뷰에 맞는 버전만 볼수 있어야 한다.
         */
        sIterator->mViewScn = aStatement->mScn;
    }
    
    if( aStmtReadMode == SML_SRM_RECENT )
    {
        /**
         * 이전 또는 자신의 Statement에서 갱신한 최신 버전을 볼수 있어야 한다.
         */
        sIterator->mViewTcn = SML_INFINITE_TCN;
    }
    else
    {
        /**
         * 이전 Statement에서 갱신한 버전만 볼수 있어야 한다.
         * - 자신의 Statement에서 갱신한 버전은 볼수 없다.
         */
        sIterator->mViewTcn = aStatement->mTcn;
    }

    sIterator->mMoveToRowId   = sModule->mMoveToRowId;
    sIterator->mMoveToPosNum  = sModule->mMoveToPosNum;
    sIterator->mResetIterator = sModule->mResetIterator;
    sIterator->mFiniIterator = sModule->mFiniIterator;

    if( aScanDirection == SML_SCAN_FORWARD )
    {
        sIterator->mFetchNext = sModule->mScanFunc.mFetchNext;
        sIterator->mFetchPrev = sModule->mScanFunc.mFetchPrev;
        sIterator->mFetchAggr = sModule->mScanFunc.mFetchAggr;
        sIterator->mFetchSampling = sModule->mScanFunc.mFetchSampling;
    }
    else
    {
        /* SML_SCAN_BACKWARD */
        sIterator->mFetchNext = sModule->mScanFunc.mFetchPrev;
        sIterator->mFetchPrev = sModule->mScanFunc.mFetchNext;
        sIterator->mFetchAggr = sModule->mScanFunc.mFetchAggr;
        sIterator->mFetchSampling = sModule->mScanFunc.mFetchSampling;
    }

    if( aProperty->mGroupFetch == STL_TRUE )
    {
        sIterator->mMoveToNextKey = ((smeGroupIteratorModule *)sModule)->mMoveToNextKey;
        sIterator->mResetGroup = ((smeGroupIteratorModule *)sModule)->mResetGroup;
    }
    else
    {
        sIterator->mMoveToNextKey = NULL;
        sIterator->mResetGroup = NULL;
    }

    STL_RING_INIT_DATALINK( sIterator, STL_OFFSETOF( smiIterator, mIteratorLink ) );

    STL_TRY( sModule->mInitIterator( sIterator, aEnv ) == STL_SUCCESS );

    STL_RING_ADD_LAST( &aStatement->mIterators, sIterator );

    *aIterator = sIterator;

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) knlFreeUnmarkedRegionMemEx( &aStatement->mStatementMem,
                                           sRegionMarkEx,
                                           KNL_ENV( aEnv ) );
    }

    return STL_FAILURE;
}

/**
 * @brief Statement Level Region Memory를 할당한다.
 * @param[in]     aStatement Statement Pointer
 * @param[in]     aSize      할당 크기
 * @param[out]    aAddr      할당된 주소
 * @param[in,out] aEnv        Storage Manager Environment
 */
stlStatus smaAllocRegionMem( smlStatement * aStatement,
                             stlInt32       aSize,
                             void        ** aAddr,
                             smlEnv       * aEnv )
{
    STL_TRY( knlAllocRegionMem( &aStatement->mStatementMem,
                                aSize,
                                aAddr,
                                KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Iterator를 해제한다.
 * @param[in] aIterator Iterator 포인터
 * @param[in,out] aEnv Storage Manager Environment
 */
stlStatus smaFreeIterator( void    * aIterator,
                           smlEnv  * aEnv )
{
    smiIterator * sIterator;

    sIterator = (smiIterator*)aIterator;
    STL_DASSERT( sIterator->mState != SMI_STATE_FREED );
    
#ifdef STL_DEBUG
    
    /**
     * defence double free
     */
    
    smiIterator * sCurIterator = NULL;
    stlBool       sFound = STL_FALSE;
    
    STL_RING_FOREACH_ENTRY( &sIterator->mStatement->mIterators, sCurIterator )
    {
        if( sCurIterator == aIterator )
        {
            sFound = STL_TRUE;
            break;
        }
    }

    STL_DASSERT( sFound == STL_TRUE );
    
#endif
    
    if( sIterator->mFiniIterator != NULL )
    {
        STL_TRY( sIterator->mFiniIterator( sIterator,
                                           aEnv )
                 == STL_SUCCESS );
        sIterator->mFiniIterator = NULL;
    }

    sIterator->mState = SMI_STATE_FREED;
    
    STL_RING_UNLINK( &sIterator->mStatement->mIterators, sIterator );
    
    STL_TRY( knlFreeUnmarkedRegionMemEx( &sIterator->mStatement->mStatementMem,
                                         sIterator->mRegionMarkEx,
                                         KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief aTimestamp이후에 시작한 NON-HOLDABLE Iterator들을 닫는다.
 * @param[in] aTimestamp 종료할 Statement Iterator의 선택 기준
 * @param[in,out] aEnv Storage Manager Environment
 * @note 해당 함수는 Commit이나 Rollback시에만 호출된다.
 */
stlStatus smaCloseNonHoldableIterators( stlInt64   aTimestamp,
                                        smlEnv * aEnv )
{
    smlStatement * sStatement;
    smiIterator  * sIterator;
    
    STL_RING_FOREACH_ENTRY( &(SML_SESS_ENV(aEnv)->mUpdatableStmt), sStatement )
    {
        if( sStatement->mTimestamp < aTimestamp )
        {
            continue;
        }
        
        STL_RING_FOREACH_ENTRY( &sStatement->mIterators, sIterator )
        {
            sIterator->mState = SMI_STATE_CLOSED;
        }
    }

    STL_RING_FOREACH_ENTRY( &(SML_SESS_ENV(aEnv)->mReadOnlyStmt), sStatement )
    {
        if( sStatement->mTimestamp < aTimestamp )
        {
            continue;
        }
        
        if( (sStatement->mAttr & SML_STMT_ATTR_LOCKABLE) == SML_STMT_ATTR_LOCKABLE )
        {
            STL_RING_FOREACH_ENTRY( &sStatement->mIterators, sIterator )
            {
                sIterator->mState = SMI_STATE_CLOSED;
            }
        }
        else
        {
            if( (sStatement->mAttr & SML_STMT_ATTR_HOLDABLE) != SML_STMT_ATTR_HOLDABLE )
            {
                STL_RING_FOREACH_ENTRY( &sStatement->mIterators, sIterator )
                {
                    sIterator->mState = SMI_STATE_CLOSED;
                }
            }
        }
    }

    return STL_SUCCESS;
}

/**
 * @brief Read Only Statement에 Transaction Information을 설정한다.
 * @param[in] aTransId Transaction Identifier
 * @param[in] aCommitScn Transaction Commit Scn
 * @param[in,out] aEnv Storage Manager Environment
 * @note Fetch Across Commit을 지원하기 위하여 Commit시에 Read Only Statement에
 * <BR> Persistent Transaction 정보(Transaction Record가 기록된 위치)를 설정한다.
 */
stlStatus smaUpdateTransInfoOnReadOnlyStmt( smxlTransId   aTransId,
                                            smlScn        aCommitScn,
                                            smlEnv      * aEnv )
{
    smlStatement * sStatement;

    if( STL_RING_IS_EMPTY( &(SML_SESS_ENV(aEnv)->mReadOnlyStmt) ) != STL_TRUE )
    {
        STL_RING_FOREACH_ENTRY( &(SML_SESS_ENV(aEnv)->mReadOnlyStmt), sStatement )
        {
            /**
             * transaction 을 가진 select statement 에 commit scn이 아직 설정되지 않았다면
             * - 유효한 Transaction id를 가진 경우에만 Commit Scn을 설정한다.
             *   유효하지 않은 Transaction id를 가진 경우는 transaction record를 기록하지 않은
             *   update statement(즉, database에 갱신연산을 수행하기 이전 상태)이기 때문에
             *   my transaction에 대한 visibility를 확인할 필요가 없다.
             * - 
             */
        
            if( (sStatement->mTransId != SML_INVALID_TRANSID) &&
                (sStatement->mCommitScn == SML_INFINITE_SCN) &&
                (sStatement->mState != SML_STMT_STATE_COMMITTED) )
            {
                STL_DASSERT( sStatement->mTransId == aTransId );
                sStatement->mCommitScn = aCommitScn;
            }
        }
    }

    return STL_SUCCESS;
}

/**
 * @brief Active Statement에 Transaction Identifier를 설정한다.
 * @param[in] aTransId Transaction Identifier
 * @param[in,out] aEnv Storage Manager Environment
 * @note Transaction Identifier에 Transaction Record 위치를 설정하기 위해서 사용된다.
 */
stlStatus smaUpdateTransIdOnStmt( smxlTransId   aTransId,
                                  smlEnv      * aEnv )
{
    smlStatement * sStatement;
    stlInt64       sTransSeq;

    sTransSeq = smxlGetTransSeq( aTransId );

    STL_RING_FOREACH_ENTRY( &(SML_SESS_ENV(aEnv)->mUpdatableStmt), sStatement )
    {
        STL_DASSERT( sStatement->mState != SML_STMT_STATE_COMMITTED );
        
        if( SMXL_IS_UNDO_BOUND_TRANS(sStatement->mTransId) == STL_FALSE )
        {
            sStatement->mTransId = aTransId;
            sStatement->mSmlTransId = SMXL_TO_SML_TRANS_ID( aTransId, sTransSeq );
        }
    }

    STL_RING_FOREACH_ENTRY( &(SML_SESS_ENV(aEnv)->mReadOnlyStmt), sStatement )
    {
        if( (SMXL_IS_UNDO_BOUND_TRANS(sStatement->mTransId) == STL_FALSE) &&
            (sStatement->mState != SML_STMT_STATE_COMMITTED) )
        {
            sStatement->mTransId = aTransId;
            sStatement->mSmlTransId = SMXL_TO_SML_TRANS_ID( aTransId, sTransSeq );
        }
    }

    return STL_SUCCESS;
}

/**
 * @brief DDL Lock timeout을 사용해야 하는지 여부를 얻는다.
 * @param[in] aStatement Statement Handle
 * @return Lock Timeout을 사용해야 하면 STL_TRUE 반환
 */
inline stlBool smaUseDdlLockTimeout( smlStatement * aStatement )
{
    return ( ((aStatement->mAttr & SML_STMT_ATTR_DDL_LOCK_TIMEOUT) ==
              SML_STMT_ATTR_DDL_LOCK_TIMEOUT) ? STL_TRUE : STL_FALSE );
}

/**
 * @brief Table Mutating 조건의 위배 여부를 검사한다.
 * @param[in] aMyStatement     자신의  Statement 포인터
 * @param[in] aTargetRelHandle 검사할 대상 relation handle
 * @param[in,out] aEnv         Storage Manager Environment
 * @remarks 자신의 트랜잭션 내에 자신을 제외한 Active Statement들중
 * <BR> 같은 Relation을 갱신하고 있는 Statement가 있는지 확인한다.
 */
stlStatus smaCheckMutating( smlStatement * aMyStatement,
                            void         * aTargetRelHandle,
                            smlEnv       * aEnv )
{
    smlStatement * sStatement;
    
    STL_RING_FOREACH_ENTRY( &(SML_SESS_ENV(aEnv)->mUpdatableStmt), sStatement )
    {
        if( sStatement == aMyStatement )
        {
            continue;
        }

        if( sStatement->mUpdateRelHandle != NULL )
        {
            STL_TRY_THROW( sStatement->mUpdateRelHandle == aTargetRelHandle,
                           RAMP_ERR_MUTATING_TABLE );
        }
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_MUTATING_TABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_MUTATING_TABLE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}
                            
/** @} */

/**
 * @addtogroup smaStmt
 * @{
 */

/** @} */
