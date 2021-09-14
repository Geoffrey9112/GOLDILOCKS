/*******************************************************************************
 * smlTrans.c
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
#include <smg.h>
#include <smxl.h>
#include <smkl.h>

extern knlStartupPhase   gSmWarmupPhase;
extern smxlTrans       * gSmxlTransTable;

/**
 * @file smlTrans.c
 * @brief Storage Manager Layer Transaction Routines
 */

/**
 * @addtogroup smlTrans
 * @{
 */

/**
 * @brief 트랜잭션을 시작한다.
 * @param[in] aIsolationLevel 트랜잭션 독립성 레벨
 * @param[in] aIsGlobalTrans  Global Transaction 여부
 * @param[out] aTransId 할당된 트랜잭션 아이디
 * @param[in,out] aEnv Environment 포인터 
 */
stlStatus smlBeginTrans( smlIsolationLevel   aIsolationLevel,
                         stlBool             aIsGlobalTrans,
                         smlTransId        * aTransId,
                         smlEnv            * aEnv )
{
    smxlTransId sTransId = SML_INVALID_TRANSID;
    stlUInt64   sTransSeq = 0;
    
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );

    STL_TRY( smxlAllocTrans( aIsolationLevel,
                             aIsGlobalTrans,
                             &sTransId,
                             &sTransSeq,
                             aEnv ) == STL_SUCCESS );

    *aTransId = SMXL_TO_SML_TRANS_ID( sTransId, sTransSeq );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 트랜잭션을 종료한다.
 * @param[in] aTransId 종료할 트랜잭션 아이디
 * @param[in] aPendActionType Pending Action Type
 * @param[in] aWriteMode Transaction Commit Write Mode
 * @param[in,out] aEnv Environment 포인터 
 * @see @a aPendActionType : smlPendActionFlags
 * @see @a aWriteMode : smlTransactionWriteMode
 */
stlStatus smlEndTrans( smlTransId   aTransId,
                       stlInt32     aPendActionType,
                       stlInt64     aWriteMode,
                       smlEnv     * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    STL_DASSERT( smxlValidateTransId( aTransId, aEnv ) == STL_SUCCESS );

    STL_TRY( smxlFreeTrans( SMXL_TO_TRANS_ID( aTransId ),
                            aPendActionType,
                            STL_FALSE, /* aCleanup */
                            aWriteMode,
                            aEnv ) == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 트랜잭션을 커밋한다.
 * @param[in] aTransId 커밋할 트랜잭션 아이디
 * @param[in] aComment Transaction Comment
 * @param[in] aWriteMode Transaction Commit Write Mode
 * @param[in,out] aEnv Environment 포인터 
 */
stlStatus smlCommit( smlTransId   aTransId,
                     stlChar    * aComment,
                     stlInt64     aWriteMode,
                     smlEnv     * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    STL_DASSERT( smxlValidateTransId( aTransId, aEnv ) == STL_SUCCESS );
    
    STL_TRY( smxlCommit( SMXL_TO_TRANS_ID( aTransId ),
                         aComment,
                         aWriteMode,
                         aEnv )
             == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 트랜잭션을 롤백한다.
 * @param[in] aTransId 롤백할 트랜잭션 아이디
 * @param[in] aWriteMode Transaction Commit Write Mode
 * @param[in,out] aEnv Environment 포인터 
 */
stlStatus smlRollback( smlTransId   aTransId,
                       stlInt64     aWriteMode,
                       smlEnv     * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    STL_DASSERT( smxlValidateTransId( aTransId, aEnv ) == STL_SUCCESS );
    
    STL_TRY( smxlRollback( SMXL_TO_TRANS_ID( aTransId ),
                           aWriteMode, 
                           STL_FALSE, /* aCleanup */
                           aEnv ) == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief IN-DOUBT 트랜잭션을 커밋한다.
 * @param[in]     aTransId         트랜잭션 아이디
 * @param[in]     aInDoubtTransId  IN-DOUBT 트랜잭션 아이디
 * @param[in]     aComment         Transaction Comment
 * @param[in,out] aEnv             Environment 포인터
 */
stlStatus smlCommitInDoubtTransaction( smlTransId   aTransId,
                                       smlTransId   aInDoubtTransId,
                                       stlChar    * aComment,
                                       smlEnv     * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    STL_DASSERT( smxlValidateTransId( aTransId, aEnv ) == STL_SUCCESS );
    STL_DASSERT( smxlValidateTransId( aInDoubtTransId, aEnv ) == STL_SUCCESS );
    
    STL_TRY( smxlCommitInDoubtTransaction( SMXL_TO_TRANS_ID( aTransId ),
                                           SMXL_TO_TRANS_ID( aInDoubtTransId ),
                                           aComment,
                                           aEnv ) == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief IN-DOUBT 트랜잭션을 롤백한다.
 * @param[in]     aTransId         트랜잭션 아이디
 * @param[in]     aInDoubtTransId  IN-DOUBT 트랜잭션 아이디
 * @param[in]     aRestartRollback Restart Rollback 여부
 * @param[in,out] aEnv             Environment 포인터
 */
stlStatus smlRollbackInDoubtTransaction( smlTransId   aTransId,
                                         smlTransId   aInDoubtTransId,
                                         stlBool      aRestartRollback,
                                         smlEnv     * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    STL_DASSERT( smxlValidateTransId( aTransId, aEnv ) == STL_SUCCESS );
    STL_DASSERT( smxlValidateTransId( aInDoubtTransId, aEnv ) == STL_SUCCESS );
    
    STL_TRY( smxlRollbackInDoubtTransaction( SMXL_TO_TRANS_ID( aTransId ),
                                             SMXL_TO_TRANS_ID( aInDoubtTransId ),
                                             aRestartRollback,
                                             aEnv ) == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 트랜잭션을 PREPARE한다.
 * @param[in] aXid        Prepare할 글로벌 트랜잭션 아이디
 * @param[in] aTransId    Prepare할 로컬 트랜잭션 아이디
 * @param[in] aWriteMode  Transaction Commit Write Mode
 * @param[in,out] aEnv Environment 포인터 
 */
stlStatus smlPrepare( stlXid     * aXid,
                      smlTransId   aTransId,
                      stlInt64     aWriteMode,
                      smlEnv     * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    STL_DASSERT( smxlValidateTransId( aTransId, aEnv ) == STL_SUCCESS );
    
    STL_TRY( smxlPrepare( aXid,
                          SMXL_TO_TRANS_ID( aTransId ),
                          aWriteMode,
                          aEnv ) == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief PREPARED Transaction을 Fetch한다.
 * @param[in,out]  aTransId      Local Transaction Identifier
 * @param[in]      aRecoverLock  Lock 정보를 이전상태로 원복할지 여부
 * @param[out]     aXid          Global Transaction Identifier
 * @param[in,out]  aEnv          Environment 정보
 * @remarks aTransId가 SML_INVALID_TRANSID 라면 스캔 시작을 의미하고,
 *          aTransId가 SML_INVALID_TRANSID 아니라면 다음 트래잭션을 recover한다.
 *          반환된 aTransId가 SML_INVALID_TRANSID 라면 스캔을 종료해야 한다.
 */
stlStatus smlFetchPreparedTransaction( smlTransId * aTransId,
                                       stlBool      aRecoverLock,
                                       stlXid     * aXid,
                                       smlEnv     * aEnv )
{
    smxlTransId sTransId;
    stlUInt64   sTransSeq;
    
    sTransId = SMXL_TO_TRANS_ID( *aTransId );
    
    STL_TRY( smxlFetchPreparedTransaction( &sTransId,
                                           &sTransSeq,
                                           aRecoverLock,
                                           aXid,
                                           aEnv )
             == STL_SUCCESS );

    *aTransId = SMXL_TO_SML_TRANS_ID( sTransId, sTransSeq );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 재시작 이후 다시 PREPARE 가능한 트랜잭션인지 검사한다.
 * @param[in] aTransId  Local Transaction Identifier
 */
stlBool smlIsRepreparableTransaction( smlTransId aTransId )
{
    return smxlIsRepreparable( SMXL_TO_TRANS_ID( aTransId ) );
}

/**
 * @brief 자신의 트랜잭션 시작 시간을 얻는다.
 * @param[in] aTransId 대상 트랜잭션 아이디
 * @param[in,out] aEnv Environment 포인터
 * @note Thread Safe 하지 않기 때문에 자신의 트랜잭션 시작 시간정보만 얻어야 한다.
 */
stlTime smlBeginTransTime( smlTransId   aTransId,
                           smlEnv     * aEnv )
{
    return smxlBeginTransTime( SMXL_TO_TRANS_ID( aTransId ),
                               aEnv );
}

/**
 * @brief 트랜잭션을 정리한다.
 * @param[in] aTransId 롤백할 트랜잭션 아이디
 * @param[in,out] aEnv Environment 포인터
 * @remarks 해당 함수는 비정상 종료시 Cleanup Thread에 의해서 호출된다.
 * @note 만약 해당 트랜잭션에 Active Statement가 존재한다면 그 Statement들에 의해서
 *  <BR> 할당된 Heap 메모리는 해제되지 않는다. Cleanup Thread는 해당 세션과 다른
 *  <BR> Process이기 때문에 해당 세션의 Heap에 접근할수 없다.
 *  <BR> Heap 메모리 leak이 발생하기는 하지만 비정상 종료가 자주 발생되지 않는다는
 *  <BR> 가정에서 이를 허용한다.
 */
stlStatus smlCleanupTrans( smlTransId   aTransId,
                           smlEnv     * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( smxlRollback( SMXL_TO_TRANS_ID( aTransId ),
                           SML_TRANSACTION_CWM_WAIT,
                           STL_TRUE, /* aCleanup */
                           aEnv ) == STL_SUCCESS );
    
    STL_TRY( smxlFreeTrans( SMXL_TO_TRANS_ID( aTransId ),
                            SML_PEND_ACTION_ROLLBACK,
                            STL_TRUE, /* aCleanup */
                            SML_TRANSACTION_CWM_WAIT,
                            aEnv ) == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

stlStatus smlRollbackTransForCleanup( smlTransId   aTransId,
                                      smlEnv     * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( smxlRollback( SMXL_TO_TRANS_ID( aTransId ),
                           SML_TRANSACTION_CWM_WAIT,
                           STL_TRUE, /* aCleanup */
                           aEnv )
             == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

stlStatus smlEndTransForCleanup( smlTransId   aTransId,
                                 smlEnv     * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( smxlFreeTrans( SMXL_TO_TRANS_ID( aTransId ),
                            SML_PEND_ACTION_ROLLBACK,
                            STL_TRUE, /* aCleanup */
                            SML_TRANSACTION_CWM_WAIT,
                            aEnv )
             == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief Savepoint를 설정한다.
 * @param[in] aTransId Savepoint를 설정할 트랜잭션 아이디
 * @param[in] aSavepointName Savepoint 이름
 * @param[in,out] aEnv Environment 포인터
 * @note @a aSavepintName은 Null Terminate String이여야 한다.
 */
stlStatus smlSavepoint( smlTransId   aTransId,
                        stlChar    * aSavepointName,
                        smlEnv     * aEnv )
{
    return smxlMarkSavepoint( SMXL_TO_TRANS_ID( aTransId ),
                              aSavepointName,
                              aEnv );
}

/**
 * @brief 주어진 Savepoint까지 롤백한다.
 * @param[in] aTransId Savepoint를 소유한 트랜잭션 아이디
 * @param[in] aSavepointName Savepoint 이름
 * @param[in,out] aEnv Environment 포인터 
 * @note @a aSavepintName은 Null Terminate String이여야 한다.
 */
stlStatus smlRollbackToSavepoint( smlTransId   aTransId,
                                  stlChar    * aSavepointName,
                                  smlEnv     * aEnv )
{
    return smxlRollbackToSavepoint( SMXL_TO_TRANS_ID( aTransId ),
                                    aSavepointName,
                                    aEnv );
}

/**
 * @brief 주어진 Savepoint를 포함한 이후의 모든 Savepoint를 삭제한다.
 * @param[in] aTransId Savepoint를 소유한 트랜잭션 아이디
 * @param[in] aSavepointName Savepoint 이름
 * @param[in,out] aEnv Environment 포인터 
 * @note @a aSavepintName은 Null Terminate String이여야 한다.
 */
stlStatus smlReleaseSavepoint( smlTransId   aTransId,
                               stlChar    * aSavepointName,
                               smlEnv     * aEnv )
{
    return smxlReleaseSavepoint( SMXL_TO_TRANS_ID( aTransId ),
                                 aSavepointName,
                                 aEnv );
}

/**
 * @brief Savepoint의 존재 여부를 검사한다.
 * @param[in] aTransId Savepoint를 설정할 트랜잭션 아이디
 * @param[in] aSavepointName Savepoint 이름
 * @param[out] aSavepointTimestamp Savepoint 의 Timestamp
 * @param[in,out] aEnv Environment 포인터
 * @return 존재하면 STL_TRUE, 그렇지 않다면 STL_FALSE
 * @note @a aSavepintName은 Null Terminate String이여야 한다.
 */
stlBool smlExistSavepoint( smlTransId   aTransId,
                           stlChar    * aSavepointName,
                           stlInt64   * aSavepointTimestamp,
                           smlEnv     * aEnv )
{
    return smxlExistSavepoint( SMXL_TO_TRANS_ID( aTransId ),
                               aSavepointName,
                               aSavepointTimestamp,
                               aEnv );
}

/**
 * @brief 트랜잭션 Commit Scn을 구한다.
 * @param[in] aTransId 트랜잭션 아이디
 * @param[out] aCommitScn Commit Scn
 * @param[in,out] aEnv Environment 포인터
 * @note 해당함수는 smlBeginTrans에서부터 smlEndTrans사이에 호출될수 있다.
 *       그 이전 혹은 이후에 호출될 경우는 SML_RECYCLED_TRANS_SCN을 반환한다.
 *       aTransId가 SML_INVALID_TRANSID 라면 system scn을 반환한다.
 */
stlStatus smlGetCommitScn( smlTransId   aTransId,
                           smlScn     * aCommitScn,
                           smlEnv     * aEnv )
{
    smxlTrans * sTrans;
    smlScn      sCommitScn = SML_INFINITE_SCN;

    if( aTransId == SML_INVALID_TRANSID )
    {
        sCommitScn = smxlGetSystemScn();
    }
    else
    {
        sTrans = SMXL_TRANS_SLOT( aTransId );

        if( sTrans->mTransSeq == SMXL_TRANS_SEQ(aTransId) )
        {
            while( 1 )
            {
                if( sTrans->mState == SMXL_STATE_PRECOMMIT )
                {
                    /**
                     * commit scn이 설정될때가지 반복한다.
                     */
                    stlBusyWait();
                }
                else
                {
                    if( (sTrans->mTransId == SML_INVALID_TRANSID) ||
                        (sTrans->mTransSeq != SMXL_TRANS_SEQ(aTransId)) )
                    {
                        /**
                         * 재사용된 경우
                         */
                        sCommitScn = SML_RECYCLED_TRANS_SCN;
                        break;
                    }
                    
                    sCommitScn = sTrans->mCommitScn;
                    stlMemBarrier();

                    /**
                     * double checking
                     * - commit scn의 유효성 검사
                     */
                    if( (sTrans->mTransId == SML_INVALID_TRANSID) ||
                        (sTrans->mTransSeq != SMXL_TRANS_SEQ(aTransId)) )
                    {
                        /**
                         * 재사용된 경우
                         */
                        sCommitScn = SML_RECYCLED_TRANS_SCN;
                    }
                    
                    break;
                }
            }
        }
        else
        {
            /**
             * 재사용된 경우
             */
            sCommitScn = SML_RECYCLED_TRANS_SCN;
        }
    }

    *aCommitScn = sCommitScn;
    
    return STL_SUCCESS;
}

/**
 * @brief System Next Scn을 구한다.
 * @return System Next Scn
 */
smlScn smlGetNextSystemScn()
{
    return smxlGetSystemScn() + 1;
}

/**
 * @brief System Scn을 구한다.
 * @return System Next Scn
 */
smlScn smlGetSystemScn()
{
    return smxlGetSystemScn();
}

/**
 * @brief 트랜잭션 View Scn을 구한다.
 * @param[in] aTransId 트랜잭션 아이디
 * @param[in,out] aEnv Environment 포인터
 * @return Transaction View Scn
 * @note Transaction View Scn은 smlBeginTrans시 결정된다.
 * <BR> 트랜잭션이 시작된 이후에 발생한 Statement에 의해서 절대로 변경되지 않으며,
 * <BR> smlBeginTrans 이후에 언제든지 호출될수 있다.
 */
smlScn smlGetTransViewScn( smlTransId   aTransId,
                           smlEnv     * aEnv )
{
    return smxlGetTransViewScn( SMXL_TO_TRANS_ID( aTransId ) );
}

/**
 * @brief Agable Scn을 최신상태로 갱신한다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smlBuildAgableScn( smlEnv * aEnv )
{
    return smxlBuildAgableScn( aEnv );
}

/**
 * @brief 주어진 aScn이 Agable 한지 검사한다.
 * @param[in] aScn 검사할 Scn
 * @param[in,out] aEnv Environment 정보
 * @return Agable하면 STL_TRUE, 그렇지 않다면 STL_FALSE를 리턴한다.
 */
stlBool smlIsAgableByViewScn( smlScn   aScn,
                              smlEnv * aEnv )
{
    stlBool sAgable = STL_FALSE;
    
    if( aScn <= smxlGetAgableStmtScn( aEnv ) )
    {
        sAgable = STL_TRUE;
    }

    return sAgable;
}

/**
 * @brief Ager의 상태를 설정한다.
 * @param[in] aState 설정할 Ager State
 * @param[in,out] aEnv Environment 정보
 * @see aState : smlAgerState
 */
stlStatus smlSetAgerState( stlInt16   aState,
                           smlEnv   * aEnv )
{
    return smxlSetAgerState( aState,aEnv );
}

/**
 * @brief Data Store Mode를 구한다.
 * @return Data Store Mode
 * @see : smlDataStoreMode
 */
stlInt64 smlGetDataStoreMode()
{
    return smxlGetDataStoreMode();
}

/**
 * @brief 모든 Transaction이 종료될때까지 대기한다.
 * @param[in,out] aEnv Environment 포인터
 */
stlStatus smlWaitAllTransactionEnd( smlEnv * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( smxlWaitAllTrans( aEnv ) == STL_SUCCESS);
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief Transaction이 Read Only인지 검사한다.
 */
stlBool smlIsReadOnlyTrans( smlTransId aTransId )
{
    if( smxlIsTransRecordRecorded( SMXL_TO_TRANS_ID( aTransId ) ) == STL_TRUE )
    {
        return STL_FALSE;
    }
    else
    {
        return STL_TRUE;
    }
}

/** @} */
