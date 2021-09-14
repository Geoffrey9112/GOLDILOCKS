/*******************************************************************************
 * smlLock.c
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
#include <smkl.h>
#include <sme.h>
#include <sma.h>
#include <smf.h>
#include <sms.h>
#include <smd.h>
#include <smxl.h>
#include <smdmphDef.h>
#include <smdmph.h>

/**
 * @file smlLock.c
 * @brief Storage Manager Layer Lock Routines
 */

/**
 * @addtogroup smlLock
 * @{
 */

/**
 * @brief Relation에 락을 획득한다.
 * @param[in]     aTransId          트랜잭션 아이디
 * @param[in]     aLockMode         락 모드 ( IS, IX, S, X, SIX )
 * @param[in]     aRelationHandle   락을 획득할 릴레이션 핸들
 * @param[in]     aTimeInterval     락 타임아웃 시간( sec단위 )
 * @param[out]    aTableExist       Table 존재 여부
 * @param[in,out] aEnv              Environment 포인터
 * @see @a aLockMode : smlLockMode
 * @see @a aTimeInterval smlLockTimeout
 */
stlStatus smlLockTable( smlTransId   aTransId,
                        stlInt32     aLockMode,
                        void       * aRelationHandle,
                        stlInt64     aTimeInterval,
                        stlBool    * aTableExist,
                        smlEnv     * aEnv )
{
    stlInt64   sTimeInterval;

    STL_PARAM_VALIDATE( aTableExist != NULL, KNL_ERROR_STACK(aEnv) );

    STL_DASSERT( aTimeInterval != SML_LOCK_TIMEOUT_INVALID );
    
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
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

    /**
     * Lock과 Allocation Undo Relation과의 Deadlock을 피하기 위함.
     */
    STL_TRY( smxlAllocUndoRelation( SMXL_TO_TRANS_ID( aTransId ),
                                    aEnv )
             == STL_SUCCESS );

    STL_TRY( smklLock( SMXL_TO_TRANS_ID( aTransId ),
                       aLockMode,
                       aRelationHandle,
                       sTimeInterval,
                       aTableExist,
                       aEnv ) == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 특정 레코드에 락을 획득한다.
 * @param[in] aStatement      사용하는 statement
 * @param[in] aRelationHandle lock을 걸 row가 존재하는 relation의 핸들
 * @param[in] aLockMode       락 모드 ( S, X )
 * @param[in] aRowId          락을 획득할 레코드의 아이디
 * @param[out] aRecordExist   레코드가 삭제되었는지 여부 
 * @param[in,out] aEnv        Environment 포인터
 * @see @a aLockMode : smlLockMode
 * @note 해당 인터페이스는 DDL구문을 위한 함수이다.
 */
stlStatus smlLockRecordForDdl( smlStatement * aStatement,
                               void         * aRelationHandle,
                               stlInt32       aLockMode,
                               smlRid         aRowId,
                               stlBool      * aRecordExist,
                               smlEnv       * aEnv )
{
    stlInt64 sTimeInterval;
    stlInt32 sState = 0;
    smlScn   sRowScn;
    stlBool  sDeleted = STL_FALSE;
    stlBool  sIsMatched = STL_FALSE;

    smlFetchRecordInfo  sFetchRecordInfo;
    
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    *aRecordExist = STL_TRUE;
    
    STL_PARAM_VALIDATE( (aLockMode == SML_LOCK_S) ||
                        (aLockMode == SML_LOCK_X), KNL_ERROR_STACK( aEnv ) );

    /**
     * Lock과 Allocation Undo Relation과의 Deadlock을 피하기 위함.
     */
    STL_TRY( smxlAllocUndoRelation( SMA_GET_TRANS_ID(aStatement),
                                    aEnv )
             == STL_SUCCESS );

    /**
     * Timeout 값은 프로퍼티를 따른다
     */
    sTimeInterval = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DDL_LOCK_TIMEOUT,
                                                   KNL_ENV( aEnv ) );

    STL_TRY( smklLockRecord( SMA_GET_TRANS_ID(aStatement),
                             aLockMode,
                             aRowId,
                             sTimeInterval,
                             aEnv ) == STL_SUCCESS );
    sState = 1;

    /**
     * 레코드 삭제 여부 확인
     */
    
    sFetchRecordInfo.mRelationHandle  = aRelationHandle;
    sFetchRecordInfo.mColList         = NULL;
    sFetchRecordInfo.mRowIdColList    = NULL;
    sFetchRecordInfo.mPhysicalFilter  = NULL;
    sFetchRecordInfo.mTransReadMode   = SML_TRM_COMMITTED;
    sFetchRecordInfo.mStmtReadMode    = SML_SRM_RECENT;
    sFetchRecordInfo.mScnBlock        = & sRowScn;
    sFetchRecordInfo.mLogicalFilter   = NULL;
    sFetchRecordInfo.mLogicalFilterEvalInfo = NULL;
        
    STL_TRY( smdmphFetchRecord( aStatement,
                                & sFetchRecordInfo,
                                & aRowId,
                                0,
                                & sIsMatched,
                                & sDeleted,
                                aEnv ) == STL_SUCCESS );

    /**
     * 이미 삭제된 레코드라면 락을 유지할 이유가 없다.
     */
    if( sDeleted == STL_TRUE )
    {
        STL_TRY( smklUnlockRecord( SMA_GET_TRANS_ID(aStatement),
                                   aRowId,
                                   aEnv )
                 == STL_SUCCESS );

        *aRecordExist = STL_FALSE;
    }
    else
    {
        STL_DASSERT( sIsMatched == STL_TRUE );
        *aRecordExist = STL_TRUE;
    }

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smklUnlockRecord( SMA_GET_TRANS_ID(aStatement), aRowId, aEnv );
    }

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 특정 레코드에 Exclusive 락을 획득한다.
 * @param[in] aStatement 사용하는 statement
 * @param[in] aRelationHandle lock을 걸 row가 존재하는 relation의 핸들
 * @param[in] aRowId 락을 획득할 레코드의 아이디
 * @param[in] aRowScn 레코드 검색 당시의 SCN
 * @param[in] aTimeInterval 락 타임아웃 시간( sec단위 )
 * @param[out] aVersionConflict Lock 시도 도중에 version conflict 상황을 만났는지 여부
 * @param[in,out] aEnv Environment 포인터
 * @note 해당 인터페이스는 FOR UPDATE 구문을 위한 함수이다.
 */
stlStatus smlLockRecordForUpdate( smlStatement * aStatement,
                                  void         * aRelationHandle,
                                  smlRid         aRowId,
                                  smlScn         aRowScn,
                                  stlInt64       aTimeInterval,
                                  stlBool      * aVersionConflict,
                                  smlEnv       * aEnv )
{
    smlRelationType   sRelType = SME_GET_RELATION_TYPE( aRelationHandle );
    smdTableModule  * sModule = smdGetTableModule(SML_GET_TABLE_TYPE(sRelType));

    STL_DASSERT( aTimeInterval != SML_LOCK_TIMEOUT_PROPERTY );
    STL_DASSERT( aTimeInterval != SML_LOCK_TIMEOUT_INVALID );
    
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    /**
     * Lock과 Allocation Undo Relation과의 Deadlock을 피하기 위함.
     */
    STL_TRY( smxlAllocUndoRelation( SMA_GET_TRANS_ID(aStatement),
                                    aEnv )
             == STL_SUCCESS );

    if( smxlIsGlobalTrans( SMA_GET_TRANS_ID(aStatement) ) == STL_TRUE )
    {
        /**
         * global transaction은 prepare 과정으로 인하여 transaction record를
         * 기록해야 한다.
         */
        STL_TRY( smxlInsertTransRecord( SMA_GET_TRANS_ID(aStatement),
                                        aEnv )
                 == STL_SUCCESS );
    }

    /**
     * @todo 이미 Lock이 잡혀있는 경우면 Lock Row를 수행할 필요가 없다.
     * Record에 Lock Bit 설정
     */
    STL_TRY( sModule->mLockRow( aStatement,
                                aRelationHandle,
                                &aRowId,
                                aRowScn,
                                SML_LOCK_X,
                                aTimeInterval,
                                aVersionConflict,
                                aEnv )
             == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief Request Group의 대기 정보에서 Transaction Edge를 추가한다.
 * @brief Request Group의 대기 정보를 설정한다.
 * @param[in]     aTransId Wait     Transaction Identifier
 * @param[in]     aRequestGroupId   대기하는 Requst Group
 */
void smlAddEdgeIntoWaitGraph( smlTransId   aTransId,
                              stlInt32     aRequestGroupId )
{
    smklAddEdgeIntoWaitGraph( SMXL_TRANS_SLOT_ID( aTransId ),
                              aRequestGroupId );
}

/**
 * @brief Request Group의 대기 정보에서 Transaction Edge를 삭제한다.
 * @param[in]     aTransId Wait     Transaction Identifier
 * @param[in]     aRequestGroupId   대기하는 Requst Group
 */
void smlRemoveEdgeFromWaitGraph( smlTransId   aTransId,
                                 stlInt32     aRequestGroupId )
{
    smklRemoveEdgeFromWaitGraph( SMXL_TRANS_SLOT_ID( aTransId ),
                                 aRequestGroupId );
}

/** @} */
