/*******************************************************************************
 * smklManger.c
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
#include <smxl.h>
#include <smd.h>
#include <smf.h>
#include <smg.h>
#include <smkl.h>
#include <smklDef.h>
#include <smklManager.h>
#include <smklLockNode.h>
#include <smklHashTable.h>
#include <smklWaitGraph.h>

/**
 * @file smklManager.c
 * @brief Storage Manager Layer Local Lock Manager Internal Routines
 */

extern knlLatch        * gSmklDeadlockLatch;

/**
 * @addtogroup smkl
 * @{
 */

stlChar gLockConversionMatrix[6][6] =
{               /* NONE */      /* IS */      /* IX */     /* S */       /* X */     /* SIX */
    /* NONE */ { SML_LOCK_NONE, SML_LOCK_IS,  SML_LOCK_IX,  SML_LOCK_S,   SML_LOCK_X, SML_LOCK_SIX },
    /* IS */   { SML_LOCK_IS,   SML_LOCK_IS,  SML_LOCK_IX,  SML_LOCK_S,   SML_LOCK_X, SML_LOCK_SIX },
    /* IX */   { SML_LOCK_IX,   SML_LOCK_IX,  SML_LOCK_IX,  SML_LOCK_SIX, SML_LOCK_X, SML_LOCK_SIX },
    /* S  */   { SML_LOCK_S,    SML_LOCK_S,   SML_LOCK_SIX, SML_LOCK_S,   SML_LOCK_X, SML_LOCK_SIX },
    /* X  */   { SML_LOCK_X,    SML_LOCK_X,   SML_LOCK_X,   SML_LOCK_X,   SML_LOCK_X, SML_LOCK_X },
    /* SIX  */ { SML_LOCK_SIX,  SML_LOCK_SIX, SML_LOCK_SIX, SML_LOCK_SIX, SML_LOCK_X, SML_LOCK_SIX }
};

stlBool gLockCompatibleMatrix[6][6] =
{               /* NONE */  /* IS */   /* IX */   /* S */     /* X */   /* SIX */
    /* NONE */ { STL_TRUE,  STL_TRUE,  STL_TRUE,  STL_TRUE,  STL_TRUE,  STL_TRUE },
    /* IS */   { STL_TRUE,  STL_TRUE,  STL_TRUE,  STL_TRUE,  STL_FALSE, STL_TRUE },
    /* IX */   { STL_TRUE,  STL_TRUE,  STL_TRUE,  STL_FALSE, STL_FALSE, STL_FALSE },
    /* S  */   { STL_TRUE,  STL_TRUE,  STL_FALSE, STL_TRUE,  STL_FALSE, STL_FALSE },
    /* X  */   { STL_TRUE,  STL_FALSE, STL_FALSE, STL_FALSE, STL_FALSE, STL_FALSE },
    /* SIX  */ { STL_TRUE,  STL_TRUE,  STL_FALSE, STL_FALSE, STL_FALSE, STL_FALSE }
};


/**
 * @brief 특정 객체에 락을 획득한다.
 * @param[in]     aTransId        트랜잭션 아이디
 * @param[in]     aLockMode       획득하고자 하는 락 모드
 * @param[in]     aRelationHandle 관련 Relation Handle
 * @param[in]     aTimeInterval   락 타임아웃 시간( sec단위 )
 * @param[out]    aTableExist     Table 존재 여부
 * @param[in,out] aEnv            Environment 포인터
 * @see @a aLockMode : smlLockMode
 * @see @a aTimeInterval : smlLockTimeout
 */
stlStatus smklLock( smxlTransId   aTransId,
                    stlInt32      aLockMode,
                    void        * aRelationHandle,
                    stlInt64      aTimeInterval,
                    stlBool     * aTableExist,
                    smlEnv      * aEnv )
{
    smklItemRelation * sLockItem;
    stlInt16           sLockSlotId;
    smklLockSlot     * sLockSlot;
    stlBool            sIsTimedOut;
    stlInt32           sLocalState = 0;
    stlInt32           sGlobalState = 0;
    stlInt32           sDeadlockState = 0;
    stlInt32           sGroupEdgeState = 0;
    stlInt32           sConvertedLockMode;
    stlInt32           sHoldLockMode;
    stlInt32           i;
    stlInt32           j = 0;
    stlInt32           sLatchIdx = -1;
    stlBool            sSkipLock = STL_FALSE;
    stlUInt32          sErrCode;
    stlBool            sIsDeadlock; 

    STL_PARAM_VALIDATE( aTableExist != NULL, KNL_ERROR_STACK(aEnv) );

    *aTableExist = STL_TRUE;
    
    STL_DASSERT( smxlIsLockable( aTransId ) == STL_TRUE );
    
    sLockItem = (smklItemRelation*)smdGetLockHandle( aRelationHandle );
    STL_TRY_THROW( sLockItem != NULL, RAMP_SKIP_LOCK );
    STL_DASSERT( aTransId != SML_INVALID_TRANSID );
    
    sLockSlotId = smklGetLockSlotId( aTransId );
    sLockSlot = smklGetLockSlot( sLockSlotId );
    sLatchIdx = sLockSlotId % SMKL_RELATION_LATCH_COUNT;

    /*
     * Optimistic Mode에서 호환여부 확인.
     * 대부분의 경우가 이에 해당된다.
     */ 
    STL_TRY( knlAcquireLatch( &sLockItem->mLatch[sLatchIdx],
                              KNL_LATCH_MODE_SHARED,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              (knlEnv*)aEnv )
             == STL_SUCCESS );
    sLocalState = 1;

    /**
     * 지워질 Lock Item은 무시한다.
     */
    
    if( sLockItem->mBeFree == STL_TRUE )
    {
        sLocalState = 0;
        STL_TRY( knlReleaseLatch( &sLockItem->mLatch[sLatchIdx],
                                  (knlEnv*)aEnv )
                 == STL_SUCCESS );
        
        *aTableExist = STL_FALSE;
        STL_THROW( RAMP_SKIP_LOCK );
    }
        
    if( sLockItem->mIsPessimistic == STL_FALSE )
    {
        if( (aLockMode == SML_LOCK_IS) || (aLockMode == SML_LOCK_IX) )
        {
            if( sLockItem->mIntentHoldArray[sLockSlotId] == SML_LOCK_NONE )
            {
                STL_TRY( smklAddRelation( aTransId,
                                          sLockSlot,
                                          aRelationHandle,
                                          aEnv ) == STL_SUCCESS );
            }
        
            sHoldLockMode = sLockItem->mIntentHoldArray[sLockSlotId];
            sConvertedLockMode = gLockConversionMatrix[sHoldLockMode][aLockMode];
            sLockItem->mIntentHoldArray[sLockSlotId] = sConvertedLockMode;
        
            sLocalState = 0;
            STL_TRY( knlReleaseLatch( &sLockItem->mLatch[sLatchIdx],
                                      (knlEnv*)aEnv )
                     == STL_SUCCESS );
            
            STL_THROW( RAMP_OPTIMISTIC_SUCCESS );
        }
    }
    
    sLocalState = 0;
    STL_TRY( knlReleaseLatch( &sLockItem->mLatch[sLatchIdx],
                              (knlEnv*)aEnv )
             == STL_SUCCESS );

    /*
     * Pessimistic Mode로 전환
     */
    for( j = 0; j < SMKL_RELATION_LATCH_COUNT; j++ )
    {
        STL_TRY( knlAcquireLatch( &sLockItem->mLatch[j],
                                  KNL_LATCH_MODE_EXCLUSIVE,
                                  KNL_LATCH_PRIORITY_NORMAL,
                                  STL_INFINITE_TIME,
                                  &sIsTimedOut,
                                  (knlEnv*)aEnv )
                 == STL_SUCCESS );
    }
    sGlobalState = 1;

    /**
     * 지워질 Lock Item은 무시한다.
     */

    if( sLockItem->mBeFree == STL_TRUE )
    {
        *aTableExist = STL_FALSE;
        STL_THROW( RAMP_PESSIMISTIC_SUCCESS );
    }
    
    STL_TRY( smklLockPessimistic( aTransId,
                                  (smklItemCore*)sLockItem,
                                  aLockMode,
                                  aTimeInterval,
                                  sLockItem->mLatch,
                                  SMKL_RELATION_LATCH_COUNT,
                                  &sSkipLock,
                                  aEnv ) == STL_SUCCESS );

    if( sSkipLock == STL_TRUE )
    {
        *aTableExist = STL_FALSE;
        STL_THROW( RAMP_OPTIMISTIC_SUCCESS );
    }
    
    STL_TRY_THROW( (aLockMode != SML_LOCK_IS) && (aLockMode != SML_LOCK_IX),
                   RAMP_PESSIMISTIC_SUCCESS );

    for( i = 0; i < SMKL_LOCK_TABLE_SIZE; i++ )
    {
        if( sLockSlot == smklGetLockSlot( i ) )
        {
            continue;
        }
        
        if( gLockCompatibleMatrix[(stlInt16)sLockItem->mIntentHoldArray[i]][aLockMode] == STL_TRUE )
        {
            continue;
        }
        
        SMKL_ADD_EDGE( sLockSlotId, i );
        
        sGlobalState = 0;
        for( j = 0; j < SMKL_RELATION_LATCH_COUNT; j++ )
        {
            STL_TRY( knlReleaseLatch( &sLockItem->mLatch[j],
                                      (knlEnv*)aEnv )
                     == STL_SUCCESS );
        }

        if( KNL_SESSION_TYPE( KNL_SESS_ENV(aEnv) ) == KNL_SESSION_TYPE_SHARED )
        {
            SMKL_ADD_EDGE( (gSmklLockTableSize + KNL_REQUEST_GROUP_ID(aEnv)), sLockSlotId );
            sGroupEdgeState = 1;
        }

        while( 1 )
        {
            if( smxlSuspendOnTransSlot( aTransId,
                                        i,
                                        aTimeInterval,
                                        &sLockItem->mIntentHoldArray[i],
                                        aEnv )
                != STL_SUCCESS )
            {
                sErrCode = stlGetLastErrorCode( KNL_ERROR_STACK( aEnv ) );
            
                if( sErrCode == SML_ERRCODE_DEADLOCK )
                {
                    STL_TRY( knlAcquireLatch( gSmklDeadlockLatch,
                                              KNL_LATCH_MODE_EXCLUSIVE,
                                              KNL_LATCH_PRIORITY_NORMAL,
                                              STL_INFINITE_TIME,
                                              &sIsTimedOut,
                                              (knlEnv*)aEnv )
                             == STL_SUCCESS );
                    sDeadlockState = 1;

                    STL_TRY( smklCheckDeadlock( aTransId,
                                                &sIsDeadlock,
                                                aEnv ) == STL_SUCCESS );

                    if( sIsDeadlock != STL_TRUE )
                    {
                        (void)stlPopError( KNL_ERROR_STACK( aEnv ) );
                        
                        sDeadlockState = 0;
                        STL_TRY( knlReleaseLatch( gSmklDeadlockLatch, (knlEnv*)aEnv ) == STL_SUCCESS );

                        continue;
                    }
                    else
                    {
                        smklResolveDeadlock( aTransId );
                    }
                }

                if( sDeadlockState == 1 )
                {
                    sDeadlockState = 0;
                    STL_TRY( knlReleaseLatch( gSmklDeadlockLatch,
                                              (knlEnv*)aEnv )
                             == STL_SUCCESS );
                }
            
                SMKL_REMOVE_EDGE( sLockSlotId, i );
                STL_TRY( STL_FALSE );
            }
            else
            {
                break;
            }
        }

        if( sGroupEdgeState == 1 )
        {
            sGroupEdgeState = 0;
            SMKL_REMOVE_EDGE( (gSmklLockTableSize + KNL_REQUEST_GROUP_ID(aEnv)), sLockSlotId );
        }
        
        SMKL_REMOVE_EDGE( sLockSlotId, i );
        
        STL_DASSERT( sLockItem->mIntentHoldArray[i] == SML_LOCK_NONE );

        for( j = 0; j < SMKL_RELATION_LATCH_COUNT; j++ )
        {
            STL_TRY( knlAcquireLatch( &sLockItem->mLatch[j],
                                      KNL_LATCH_MODE_EXCLUSIVE,
                                      KNL_LATCH_PRIORITY_NORMAL,
                                      STL_INFINITE_TIME,
                                      &sIsTimedOut,
                                      (knlEnv*)aEnv )
                     == STL_SUCCESS );
        }
        sGlobalState = 1;
    }

    STL_RAMP( RAMP_PESSIMISTIC_SUCCESS );
    
    sGlobalState = 0;
    for( j = 0; j < SMKL_RELATION_LATCH_COUNT; j++ )
    {
        STL_TRY( knlReleaseLatch( &sLockItem->mLatch[j],
                                  (knlEnv*)aEnv )
                 == STL_SUCCESS );
    }

    STL_RAMP( RAMP_OPTIMISTIC_SUCCESS );

    smxlSetTransState( aTransId, SMXL_STATE_ACTIVE );

    STL_RAMP( RAMP_SKIP_LOCK );
        
    return STL_SUCCESS;

    STL_FINISH;

    if( sGlobalState == 1 )
    {
        for( j = j - 1; j >= 0; j-- )
        {
            (void)knlReleaseLatch( &sLockItem->mLatch[j], (knlEnv*)aEnv );
        }
    }

    if( sLocalState == 1 )
    {
        (void)knlReleaseLatch( &sLockItem->mLatch[sLatchIdx], (knlEnv*)aEnv );
    }
    
    if( sGroupEdgeState == 1 )
    {
        SMKL_REMOVE_EDGE( (gSmklLockTableSize + KNL_REQUEST_GROUP_ID(aEnv)), sLockSlotId );
    }
    
    if( sDeadlockState == 1 )
    {
        (void)knlReleaseLatch( gSmklDeadlockLatch, (knlEnv*)aEnv );
    }
    
    smxlSetTransState( aTransId, SMXL_STATE_ACTIVE );
        
    return STL_FAILURE;
}

/**
 * @brief 특정 레코드에 락을 획득한다.
 * @param[in] aTransId 트랜잭션 아이디
 * @param[in] aLockMode 락 모드 ( S, X )
 * @param[in] aRid 락을 획득할 레코드의 아이디
 * @param[in] aTimeInterval 락 타임아웃 시간( sec단위 )
 * @param[in,out] aEnv Environment 포인터
 * @see @a aLockMode : smlLockMode
 * @see @a aTimeInterval : smlLockTimeout
 * @note 해당 인터페이스는 DDL구문을 위한 함수이다.
 */
stlStatus smklLockRecord( smxlTransId   aTransId,
                          stlInt32      aLockMode,
                          smlRid        aRid,
                          stlInt64      aTimeInterval,
                          smlEnv      * aEnv )
{
    smklHashBucket * sHashBucket;
    smklItemRecord * sLockItem;
    stlBool          sIsTimedOut;
    stlInt32         sState = 0;

    STL_TRY_THROW( aTransId != SML_INVALID_TRANSID, RAMP_SKIP_LOCK );
    STL_DASSERT( smxlIsLockable( aTransId ) == STL_TRUE );
    
    sHashBucket = smklGetHashBucket( aRid );

    STL_TRY( knlAcquireLatch( &sHashBucket->mLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              (knlEnv*)aEnv )
             == STL_SUCCESS );
    sState = 1;

    sLockItem = smklFindHashItem( sHashBucket, aRid );

    if( sLockItem == NULL )
    {
        STL_TRY( knlAllocDynamicMem( &sHashBucket->mShmMem,
                                     STL_SIZEOF( smklItemRecord ),
                                     (void**)&sLockItem,
                                     (knlEnv*)aEnv ) == STL_SUCCESS );

        sLockItem->mItemType = SMKL_ITEM_RECORD;
        sLockItem->mGrantedMode = SML_LOCK_NONE;
        sLockItem->mIsPessimistic = STL_FALSE;
        sLockItem->mRid = aRid;
        sLockItem->mBeFree = STL_FALSE;
        sLockItem->mUsageType = SMKL_LOCK_USAGE_DDL;
        
        STL_RING_INIT_HEADLINK( &sLockItem->mHoldList,
                                STL_OFFSETOF( smklNode, mNodeLink ) );
        STL_RING_INIT_HEADLINK( &sLockItem->mRequestList,
                                STL_OFFSETOF( smklNode, mNodeLink ) );
        STL_RING_INIT_DATALINK( sLockItem,
                                STL_OFFSETOF( smklItemRecord, mItemLink ) );

        smklAddHashItem( sHashBucket, sLockItem );
    }

    STL_TRY( smklLockPessimistic( aTransId,
                                  (smklItemCore*)sLockItem,
                                  aLockMode,
                                  aTimeInterval,
                                  &sHashBucket->mLatch,
                                  1,    /* aLatchCount */
                                  NULL, /* aSkipLock */
                                  aEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( knlReleaseLatch( &sHashBucket->mLatch, (knlEnv*)aEnv ) == STL_SUCCESS );

    smxlSetTransState( aTransId, SMXL_STATE_ACTIVE );

    STL_RAMP( RAMP_SKIP_LOCK );
        
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlReleaseLatch( &sHashBucket->mLatch, (knlEnv*)aEnv );
    }

    smxlSetTransState( aTransId, SMXL_STATE_ACTIVE );
        
    return STL_FAILURE;
}


/**
 * @brief 특정 레코드 락에 대기한다.
 * @param[in] aTransId 트랜잭션 아이디
 * @param[in] aLockMode 락 모드 ( S, X )
 * @param[in] aRid 대기할 레코드의 아이디
 * @param[in] aTimeInterval 락 타임아웃 시간( sec단위 )
 * @param[in,out] aEnv Environment 포인터
 * @see @a aLockMode : smlLockMode
 * @see @a aTimeInterval : smlLockTimeout
 */
stlStatus smklWaitRecord( smxlTransId   aTransId,
                          stlInt32      aLockMode,
                          smlRid        aRid,
                          stlInt64      aTimeInterval,
                          smlEnv      * aEnv )
{
    smklHashBucket * sHashBucket;
    smklItemRecord * sLockItem;
    stlBool          sIsTimedOut;
    stlInt32         sState = 0;

    STL_TRY_THROW( aTransId != SML_INVALID_TRANSID, RAMP_SKIP_LOCK );
    
    sHashBucket = smklGetHashBucket( aRid );

    STL_TRY( knlAcquireLatch( &sHashBucket->mLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              (knlEnv*)aEnv )
             == STL_SUCCESS );
    sState = 1;

    sLockItem = smklFindHashItem( sHashBucket, aRid );

    /**
     * 기존에 락이 잡혀 있지 않다면 대기하지 않는다.
     */
    if( sLockItem == NULL )
    {
        sState = 0;
        STL_TRY( knlReleaseLatch( &sHashBucket->mLatch,
                                  (knlEnv*)aEnv )
                 == STL_SUCCESS );

        STL_THROW( RAMP_SKIP_LOCK );
    }

    /**
     * 일단 락을 획득하기 위해서 대기한다.
     */
    STL_TRY( smklLockPessimistic( aTransId,
                                  (smklItemCore*)sLockItem,
                                  aLockMode,
                                  aTimeInterval,
                                  &sHashBucket->mLatch,
                                  1,    /* aLatchCount */
                                  NULL, /* aSkipLock */
                                  aEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( knlReleaseLatch( &sHashBucket->mLatch,
                              (knlEnv*)aEnv )
             == STL_SUCCESS );

    smxlSetTransState( aTransId, SMXL_STATE_ACTIVE );
        
    STL_RAMP( RAMP_SKIP_LOCK );
        
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlReleaseLatch( &sHashBucket->mLatch, (knlEnv*)aEnv );
    }

    smxlSetTransState( aTransId, SMXL_STATE_ACTIVE );
        
    return STL_FAILURE;
}

/**
 * @brief 특정 레코드 락이 Active 상태인지 확인한다.
 * @param[in] aTransId 트랜잭션 아이디
 * @param[in] aRid 검사할 레코드의 아이디
 * @param[in] aLockItem Lock Item 포인터
 * @param[in] aIsMyTrans My Transaction이 락을 소유하고 있는지 여부
 * @param[in,out] aEnv Environment 포인터
 * @remarks 자신이 락을 획득하고 있는 경우는 aLockItem이 NULL 이다.
 */
stlStatus smklIsActiveLockRecord( smxlTransId    aTransId,
                                  smlRid         aRid,
                                  void        ** aLockItem,
                                  stlBool      * aIsMyTrans,
                                  smlEnv       * aEnv )
{
    smklHashBucket * sHashBucket;
    smklItemRecord * sLockItem;
    stlBool          sIsTimedOut;
    stlInt32         sState = 0;
    smklNode       * sLockNode;

    *aIsMyTrans = STL_FALSE;
    *aLockItem = NULL;
    
    STL_TRY_THROW( aTransId != SML_INVALID_TRANSID, RAMP_SKIP_LOCK );
    
    sHashBucket = smklGetHashBucket( aRid );

    STL_TRY( knlAcquireLatch( &sHashBucket->mLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              (knlEnv*)aEnv )
             == STL_SUCCESS );
    sState = 1;

    sLockItem = smklFindHashItem( sHashBucket, aRid );

    /**
     * 기존에 락이 잡혀 있지 않다면 대기하지 않는다.
     */
    if( sLockItem != NULL )
    {
        sLockNode = smklFindLockNode( aTransId, (smklItemCore*)sLockItem );

        /**
         * 자신이 획득하고 있지 않은 경우에만 aIsActive가 STL_TRUE이다.
         */
        if( sLockNode != NULL )
        {
            *aIsMyTrans = STL_TRUE;
        }
    }

    sState = 0;
    STL_TRY( knlReleaseLatch( &sHashBucket->mLatch,
                              (knlEnv*)aEnv )
             == STL_SUCCESS );

    *aLockItem = sLockItem;
    
    STL_RAMP( RAMP_SKIP_LOCK );
        
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlReleaseLatch( &sHashBucket->mLatch, (knlEnv*)aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief 트랜잭션에서 획득한 모든 락을 해제한다.
 * @param[in] aTransId 트랜잭션 아이디 
 * @param[in,out] aEnv Environment 포인터
 * @note 해당 함수는 Transaction Latch에 의해서 보호된다.
 */
stlStatus smklUnlock( smxlTransId   aTransId,
                      smlEnv      * aEnv )
{
    smklLockSlot     * sLockSlot;
    stlInt16           sLockSlotId;
    smklNode         * sLockNode;
    smklItemCore     * sLockItem;
    smklItemRelation * sLockItemRelation;
    smklRelation     * sRelation;

    STL_TRY_THROW( aTransId != SML_INVALID_TRANSID, RAMP_SKIP_UNLOCK );
    
    sLockSlotId = smklGetLockSlotId( aTransId );
    sLockSlot = smklGetLockSlot( sLockSlotId );

    /*
     * 1. Transaction Lock Node List에서 획득한 락 제거
     */
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

        STL_ASSERT( SMXL_TRANS_SLOT_ID(sLockNode->mTransId) ==
                    SMXL_TRANS_SLOT_ID(aTransId) );
        STL_TRY( smklUnlockNode( sLockNode, aEnv ) == STL_SUCCESS );
    }
    
    STL_RING_INIT_HEADLINK( &sLockSlot->mLockNodeList,
                            STL_OFFSETOF( smklNode, mTransLink ) );
    
    /*
     * 2. Relation List에서 Intention Lock 제거
     * 
     * Hold Array는 트랜잭션 별로 독립된 공간을 차지하기 때문에
     * No Latch로 가능하다.
     */
    smklResetWaitSlot( aTransId );

    STL_RING_FOREACH_ENTRY( &sLockSlot->mRelationList, sRelation )
    {
        sLockItemRelation = (smklItemRelation*)smdGetLockHandle( sRelation->mRelationHandle );
        sLockItemRelation->mIntentHoldArray[sLockSlotId] = SML_LOCK_NONE;
    }

    STL_RING_INIT_HEADLINK( &sLockSlot->mRelationList,
                            STL_OFFSETOF( smklRelation, mRelationLink ) );

    STL_RAMP( RAMP_SKIP_UNLOCK );
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Lock Savepoint를 설정한다.
 * @param[in]     aTransId   트랜잭션 아이디 
 * @param[out]    aSavepoint Lock Savepoint
 * @param[in,out] aEnv       Environment 포인터
 */
stlStatus smklSavepoint( smxlTransId        aTransId,
                         smlLockSavepoint * aSavepoint,
                         smlEnv           * aEnv )
{
    smklLockSlot  * sLockSlot;
    stlInt16        sLockSlotId;

    STL_TRY_THROW( aTransId != SML_INVALID_TRANSID, RAMP_SKIP_SAVEPOINT );
    
    sLockSlotId = smklGetLockSlotId( aTransId );
    sLockSlot = smklGetLockSlot( sLockSlotId );

    aSavepoint->mLockNode = STL_RING_GET_LAST_DATA( &sLockSlot->mLockNodeList );
    aSavepoint->mRelation = STL_RING_GET_LAST_DATA( &sLockSlot->mRelationList );

    STL_RAMP( RAMP_SKIP_SAVEPOINT );
    
    return STL_SUCCESS;
}


/**
 * @brief 트랜잭션에서 획득한 모든 락을 해제한다.
 * @param[in] aTransId   트랜잭션 아이디 
 * @param[in] aSavepoint Lock Savepoint
 * @param[in,out] aEnv Environment 포인터
 * @note 해당 함수는 Transaction Latch에 의해서 보호된다.
 */
stlStatus smklUnlockForSavepoint( smxlTransId        aTransId,
                                  smlLockSavepoint * aSavepoint,
                                  smlEnv           * aEnv )
{
    smklLockSlot     * sLockSlot;
    stlInt16           sLockSlotId;
    smklNode         * sLockNode;
    smklNode         * sNextLockNode;
    smklItemCore     * sLockItem;
    smklItemRelation * sLockItemRelation;
    smklRelation     * sRelation;
    smklRelation     * sNextRelation;

    STL_TRY_THROW( aTransId != SML_INVALID_TRANSID, RAMP_SKIP_UNLOCK );
    
    sLockSlotId = smklGetLockSlotId( aTransId );
    sLockSlot = smklGetLockSlot( sLockSlotId );

    sLockNode = (smklNode*)aSavepoint->mLockNode;
    sRelation = (smklRelation*)aSavepoint->mRelation;

    /**
     * 1. Transaction Lock Node List에서 획득한 락 제거
     * - Savepoint에 설정된 지점 이후부터 Unlock 한다.
     */
    sLockNode = STL_RING_GET_NEXT_DATA( &sLockSlot->mLockNodeList,
                                        sLockNode );

    if( STL_RING_IS_HEADLINK( &sLockSlot->mLockNodeList,
                              sLockNode )
        == STL_FALSE )
    {
        STL_RING_AT_FOREACH_ENTRY_SAFE( &sLockSlot->mLockNodeList,
                                        sLockNode,
                                        sLockNode,
                                        sNextLockNode )
        {
            sLockItem = sLockNode->mLockItem;

            /*
             * Deadlock이 발생한 경우에 Lock Item이 Null일수 있다.
             */
            if( sLockItem == NULL )
            {
                continue;
            }

            STL_ASSERT( SMXL_TRANS_SLOT_ID(sLockNode->mTransId) ==
                        SMXL_TRANS_SLOT_ID(aTransId) );
            STL_TRY( smklUnlockNode( sLockNode, aEnv ) == STL_SUCCESS );

            STL_RING_UNLINK( &sLockSlot->mLockNodeList, sLockNode );
        }
    }
    
    /**
     * 2. Relation List에서 Intention Lock 제거
     *  - Savepoint에 설정된 지점 이후부터 Unlock 한다.
     */
    sRelation = STL_RING_GET_NEXT_DATA( &sLockSlot->mRelationList,
                                        sRelation );

    if( STL_RING_IS_HEADLINK( &sLockSlot->mRelationList,
                              sRelation )
        == STL_FALSE )
    {
        STL_RING_AT_FOREACH_ENTRY_SAFE( &sLockSlot->mRelationList,
                                        sRelation,
                                        sRelation,
                                        sNextRelation )
        {
            sLockItemRelation = (smklItemRelation*)smdGetLockHandle( sRelation->mRelationHandle );
            sLockItemRelation->mIntentHoldArray[sLockSlotId] = SML_LOCK_NONE;
            
            STL_RING_UNLINK( &sLockSlot->mRelationList, sRelation );
        }
    }
    
    STL_RAMP( RAMP_SKIP_UNLOCK );
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 트랜잭션에서 획득한 모든 락을 해제한다.
 * @param[in] aTransId 트랜잭션 아이디 
 * @param[in] aRid 해제하고자 하는 레코드의 아이디
 * @param[in,out] aEnv Environment 포인터
 * @note 해당 함수는 Transaction Latch에 의해서 보호된다.
 */
stlStatus smklUnlockRecord( smxlTransId   aTransId,
                            smlRid        aRid,
                            smlEnv      * aEnv )
{
    smklItemRecord * sLockItem;
    smklHashBucket * sHashBucket;
    smklNode       * sLockNode = NULL;
    smklNode       * sLockNodeIter;
    stlBool          sIsTimedOut;
    stlInt32         sState = 0;
    
    STL_TRY_THROW( aTransId != SML_INVALID_TRANSID, RAMP_SKIP_UNLOCK );
    
    sHashBucket = smklGetHashBucket( aRid );

    STL_TRY( knlAcquireLatch( &sHashBucket->mLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              (knlEnv*)aEnv )
             == STL_SUCCESS );
    sState = 1;
    
    sLockItem = smklFindHashItem( sHashBucket, aRid );

    STL_RING_FOREACH_ENTRY( &sLockItem->mHoldList, sLockNodeIter )
    {
        if( SMXL_TRANS_SLOT_ID(sLockNodeIter->mTransId) == SMXL_TRANS_SLOT_ID(aTransId) )
        {
            sLockNode = sLockNodeIter;
            break;
        }
    }
    
    STL_ASSERT( sLockNode != NULL );
    
    sState = 0;
    STL_TRY( knlReleaseLatch( &sHashBucket->mLatch, (knlEnv*)aEnv ) == STL_SUCCESS );
    
    STL_TRY( smklUnlockNode( sLockNode, aEnv ) == STL_SUCCESS );

    sLockNode->mLockItem = NULL;
    
    STL_RAMP( RAMP_SKIP_UNLOCK );
        
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlReleaseLatch( &sHashBucket->mLatch, (knlEnv*)aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief A 트랜잭션이 B 트랜잭션의 종료를 대기한다.
 * @param[in] aTransIdA 대기할 트랜잭션 아이디
 * @param[in] aTransIdB 대상 트랜잭션 아이디
 * @param[in] aTimeInterval 락 타임아웃 시간( sec단위 )
 * @param[in,out] aEnv Environment 포인터
 * @see @a aTimeInterval : smlLockTimeout
 */
stlStatus smklWaitTransLock( smxlTransId   aTransIdA,
                             smxlTransId   aTransIdB,
                             stlInt64      aTimeInterval,
                             smlEnv      * aEnv )
{
    stlInt16       sLockSlotIdA;
    stlInt16       sLockSlotIdB;
    smklLockSlot * sLockSlot;
    stlBool        sIsTimedOut;
    stlBool        sIsDeadlock;
    stlInt32       sDeadlockState = 0;
    stlInt32       sGroupEdgeState = 0;
    stlInt32       sState = 0;
    stlInt64       sWaitTime;  /* 단위 : usec */
    
    sLockSlotIdA = smklGetLockSlotId( aTransIdA );
    sLockSlotIdB = smklGetLockSlotId( aTransIdB );
    sLockSlot = smklGetLockSlot( sLockSlotIdB );

    if( KNL_SESSION_TYPE( KNL_SESS_ENV(aEnv) ) == KNL_SESSION_TYPE_SHARED )
    {
        SMKL_ADD_EDGE( (gSmklLockTableSize + KNL_REQUEST_GROUP_ID(aEnv)), sLockSlotIdA );
        sGroupEdgeState = 1;
    }

    SMKL_ADD_EDGE( sLockSlotIdA, sLockSlotIdB );
    sState = 1;
    
    sWaitTime = (stlInt64)(aTimeInterval * 1000 * 1000);

    while( 1 )
    {
        /**
         * Query Timeout 검사
         */
        STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );
        
        /**
         * Timeout 발생
         */
        STL_TRY_THROW( sWaitTime > 0, RAMP_ERR_TIMEOUT_EXPIRED );
        
        /*
         * 주어진 aTransIdB가 실제 Transaction Identifier와 같지 않다면
         * 다음과 같은 이유로 재사용되었다고 판단할수 있다.
         * 1. aTransIdB는 Persisient Transaction Identifier이다.
         * 2. Persisient Transaction Identifier는 Transaction Record가 재사용되지 않는한
         *    유일함을 보장할수 있고, aTransIdA가 살아있는한 aTransIdB의 Transaction Record는
         *    절대로 Aging될수 없다.
         */
        if( smxlGetUnsafeTransId( aTransIdB ) != aTransIdB )
        {
            break;
        }

        if( (smxlGetTransState( aTransIdB ) == SMXL_STATE_IDLE)      ||
            (smxlGetTransState( aTransIdB ) == SMXL_STATE_COMMIT)    ||
            (smxlGetTransState( aTransIdB ) == SMXL_STATE_ROLLBACK) )
        {
            break;
        }

        STL_TRY( smklCheckDeadlock( aTransIdA,
                                    &sIsDeadlock,
                                    aEnv ) == STL_SUCCESS );

        if( sIsDeadlock == STL_FALSE )
        {
            STL_TRY( knlCondWait( &sLockSlot->mTransLock,
                                  SMKL_DEADLOCK_CHECK_INTERVAL,
                                  &sIsTimedOut,
                                  KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            if( sIsTimedOut == STL_FALSE )
            {
                break;
            }
        }
        else
        {
            STL_TRY( knlAcquireLatch( gSmklDeadlockLatch,
                                      KNL_LATCH_MODE_EXCLUSIVE,
                                      KNL_LATCH_PRIORITY_NORMAL,
                                      STL_INFINITE_TIME,
                                      &sIsTimedOut,
                                      KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            sDeadlockState = 1;
                
            STL_TRY( smklCheckDeadlock( aTransIdA,
                                        &sIsDeadlock,
                                        aEnv ) == STL_SUCCESS );

            if( sIsDeadlock == STL_TRUE )
            {
                smklResolveDeadlock( aTransIdA );
            }

            sDeadlockState = 0;
            STL_TRY( knlReleaseLatch( gSmklDeadlockLatch,
                                      KNL_ENV(aEnv) )
                     == STL_SUCCESS );
                
            if( sIsDeadlock == STL_TRUE )
            {
                if( smxlGetUnsafeTransId( aTransIdB ) != aTransIdB )
                {
                    break;
                }
        
                if( (smxlGetTransState( aTransIdB ) == SMXL_STATE_IDLE)      ||
                    (smxlGetTransState( aTransIdB ) == SMXL_STATE_COMMIT)    ||
                    (smxlGetTransState( aTransIdB ) == SMXL_STATE_ROLLBACK) )
                {
                    break;
                }

                STL_THROW( RAMP_ERR_DEADLOCK );
            }
        }

        sWaitTime -= SMKL_DEADLOCK_CHECK_INTERVAL;
    }

    /*
     * aTransIdB의 래치를 잡지 않고 Edge를 변경할수 있다. 
     * aTransIdB가 재사용되었다 하여도 Wait Graph[aTransIdB][aTransIdA]는 다른 트랜잭션에
     * 의해서 재사용될수 없기 때문에 동시성 문제를 보장할수 있다.
     */
    sState = 0;
    SMKL_REMOVE_EDGE( sLockSlotIdA, sLockSlotIdB );
    
    if( sGroupEdgeState == 1 )
    {
        sGroupEdgeState = 0;
        SMKL_REMOVE_EDGE( (gSmklLockTableSize + KNL_REQUEST_GROUP_ID(aEnv)), sLockSlotIdA );
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

    if( sDeadlockState == 1 )
    {
        (void)knlReleaseLatch( gSmklDeadlockLatch, KNL_ENV(aEnv) );
    }

    switch( sState )
    {
        case 1:
            SMKL_REMOVE_EDGE( sLockSlotIdA, sLockSlotIdB );
        default:
            break;
    }

    if( sGroupEdgeState == 1 )
    {
        SMKL_REMOVE_EDGE( (gSmklLockTableSize + KNL_REQUEST_GROUP_ID(aEnv)), sLockSlotIdA );
    }

    return STL_FAILURE;
}

/**
 * @brief 트랜잭션 락을 해제한다.
 * @param[in] aTransId 해제할 트랜잭션 아이디 
 * @param[in,out] aEnv Environment 포인터
 */
stlStatus smklWakeupWaitTrans( smxlTransId   aTransId,
                               smlEnv      * aEnv )
{
    stlInt16       sLockSlotId;
    smklLockSlot * sLockSlot;
    
    sLockSlotId = smklGetLockSlotId( aTransId );
    sLockSlot = smklGetLockSlot( sLockSlotId );

    STL_TRY( knlCondBroadcast( &sLockSlot->mTransLock,
                               (knlEnv*)aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 릴레이션을 위한 Lock Handle을 할당한다.
 * @param[in]  aRelationHandle  Relation Handle
 * @param[out] aLockHandle      할당한 Lock Handle
 * @param[in,out] aEnv Environment 포인터
 */
stlStatus smklAllocLockHandle( void     * aRelationHandle,
                               void    ** aLockHandle,
                               smlEnv   * aEnv )
{
    smklItemRelation * sLockItem;
    stlInt32           i;
    
    STL_TRY( smgAllocShmMem4Open( STL_SIZEOF( smklItemRelation ),
                                  (void**)&sLockItem,
                                  aEnv ) == STL_SUCCESS );

    STL_TRY( smgAllocShmMem4Open( STL_SIZEOF( stlChar ) * SMKL_LOCK_TABLE_SIZE,
                                  (void**)&sLockItem->mIntentHoldArray,
                                  aEnv ) == STL_SUCCESS );

    sLockItem->mItemType = SMKL_ITEM_RELATION;
    sLockItem->mGrantedMode = SML_LOCK_NONE;
    sLockItem->mIsPessimistic = STL_FALSE;
    sLockItem->mBeFree = STL_FALSE;
    sLockItem->mRelationHandle = aRelationHandle;
    stlMemset( sLockItem->mIntentHoldArray,
               0x00,
               STL_SIZEOF( stlChar ) * SMKL_LOCK_TABLE_SIZE );
    STL_RING_INIT_HEADLINK( &sLockItem->mHoldList,
                            STL_OFFSETOF( smklNode, mNodeLink ) );
    STL_RING_INIT_HEADLINK( &sLockItem->mRequestList,
                            STL_OFFSETOF( smklNode, mNodeLink ) );
    for( i = 0; i < SMKL_RELATION_LATCH_COUNT; i++ )
    {
        STL_TRY( knlInitLatch( &sLockItem->mLatch[i],
                               STL_TRUE,
                               (knlEnv*)aEnv ) == STL_SUCCESS );
    }

    *aLockHandle = sLockItem;
                           
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 릴레이션을 위한 Lock Item을 해제다.
 * @param[in] aRelationHandle Relation Handle
 * @param[in,out] aEnv Environment 포인터
 */
stlStatus smklFreeLockHandle( void   * aRelationHandle,
                              smlEnv * aEnv )
{
    smklItemRelation * sLockItem;
    stlInt32           i;
    stlInt32           j = 0;
    stlBool            sIsTimedOut;
    stlInt32           sState = 0;

    sLockItem = (smklItemRelation*)SMD_GET_LOCK_HANDLE(aRelationHandle);
    
    STL_DASSERT( sLockItem != NULL );
    
    for( i = 0; i < SMKL_RELATION_LATCH_COUNT; i++ )
    {
        STL_TRY( knlAcquireLatch( &sLockItem->mLatch[i],
                                  KNL_LATCH_MODE_EXCLUSIVE,
                                  KNL_LATCH_PRIORITY_NORMAL,
                                  STL_INFINITE_TIME,
                                  &sIsTimedOut,
                                  KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    sState = 1;

    STL_TRY( smgFreeShmMem4Open( sLockItem->mIntentHoldArray,
                                 aEnv ) == STL_SUCCESS );

    STL_DASSERT( STL_RING_IS_EMPTY( &sLockItem->mHoldList ) == STL_TRUE );
    STL_DASSERT( STL_RING_IS_EMPTY( &sLockItem->mRequestList ) == STL_TRUE );

    SMD_SET_LOCK_HANDLE( aRelationHandle, NULL );
    
    sState = 0;
    for( i = 0; i < SMKL_RELATION_LATCH_COUNT; i++ )
    {
        STL_TRY( knlReleaseLatch( &sLockItem->mLatch[i],
                                  KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    for( i = 0; i < SMKL_RELATION_LATCH_COUNT; i++ )
    {
        knlFinLatch( &sLockItem->mLatch[i] );
    }
    
    STL_TRY( smgFreeShmMem4Open( sLockItem,
                                 aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        for( i = i - 1; i >= 0; i-- )
        {
            (void)knlReleaseLatch( &sLockItem->mLatch[j], KNL_ENV(aEnv) );
        }
    }
    
    return STL_FAILURE;
}

/**
 * @brief 릴레이션을 위한 Lock Item을 해제다.
 * @param[in] aLockItem Lock Item
 * @param[in,out] aEnv Environment 포인터
 */
stlStatus smklAgingLockHandle( void   * aLockItem,
                               smlEnv * aEnv )
{
    smklItemRelation * sLockItem;

    sLockItem = (smklItemRelation*)aLockItem;
    
    STL_TRY( smgFreeShmMem4Open( sLockItem->mIntentHoldArray,
                                 aEnv ) == STL_SUCCESS );

    STL_DASSERT( STL_RING_IS_EMPTY( &sLockItem->mHoldList ) == STL_TRUE );
    STL_DASSERT( STL_RING_IS_EMPTY( &sLockItem->mRequestList ) == STL_TRUE );

    STL_TRY( smgFreeShmMem4Open( sLockItem,
                                 aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Free될 Lock Item 인지 설정한다.
 * @param[in] aLockHandle 해제할 Lock Handle
 * @param[in,out] aEnv Environment 포인터
 */
stlStatus smklMarkFreeLockHandle( void   * aLockHandle,
                                  smlEnv * aEnv )
{
    smklItemRelation * sLockItem;
    stlInt32           i;
    stlInt32           j = 0;
    stlBool            sIsTimedOut;
    stlInt32           sState = 0;

    sLockItem = (smklItemRelation*)aLockHandle;

    for( i = 0; i < SMKL_RELATION_LATCH_COUNT; i++ )
    {
        STL_TRY( knlAcquireLatch( &sLockItem->mLatch[i],
                                  KNL_LATCH_MODE_EXCLUSIVE,
                                  KNL_LATCH_PRIORITY_NORMAL,
                                  STL_INFINITE_TIME,
                                  &sIsTimedOut,
                                  KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    sState = 1;

    sLockItem->mBeFree = STL_TRUE;

    sState = 0;
    for( i = 0; i < SMKL_RELATION_LATCH_COUNT; i++ )
    {
        STL_TRY( knlReleaseLatch( &sLockItem->mLatch[i],
                                  KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        for( i = i - 1; i >= 0; i-- )
        {
            (void)knlReleaseLatch( &sLockItem->mLatch[j], KNL_ENV(aEnv) );
        }
    }
    
    return STL_FAILURE;
}

stlStatus smklFinLock( stlInt16   aLockSlotId,
                       smlEnv   * aEnv )
{
    knlFinCondVar( &SMKL_LOCK_TABLE(aLockSlotId).mTransLock );
    
    return STL_SUCCESS;
}

/** @} */


/**
 * @addtogroup smklManager
 * @{
 */

inline smklLockSlot * smklGetLockSlot( stlInt16 aLockSlotId )
{
    return ( &SMKL_LOCK_TABLE(aLockSlotId) );
}

inline stlInt16 smklGetLockSlotId( smxlTransId aTransId )
{
    return SMXL_TRANS_SLOT_ID( aTransId );
}

stlStatus smklAddRelation( smxlTransId    aTransId,
                           smklLockSlot * aLockSlot,
                           void         * aRelationHandle,
                           smlEnv       * aEnv )
{
    smklRelation * sRelation;

    STL_DASSERT( aRelationHandle != NULL );
    
    STL_TRY( smxlAllocShmMem( aTransId,
                              STL_SIZEOF( smklRelation ),
                              (void**)&sRelation,
                              aEnv ) == STL_SUCCESS );

    STL_RING_INIT_DATALINK( sRelation,
                            STL_OFFSETOF( smklRelation, mRelationLink ) );
    sRelation->mRelationHandle = aRelationHandle;

    STL_RING_ADD_LAST( &aLockSlot->mRelationList, sRelation );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smklInitLockSlots( stlInt64   aTransTableSize,
                             smlEnv   * aEnv )
{
    stlInt32 i;
    
    for( i = 0; i < aTransTableSize; i++ )
    {
        STL_RING_INIT_HEADLINK( &SMKL_LOCK_TABLE(i).mLockNodeList,
                                STL_OFFSETOF( smklNode, mTransLink ) );
        STL_RING_INIT_HEADLINK( &SMKL_LOCK_TABLE(i).mRelationList,
                                STL_OFFSETOF( smklRelation, mRelationLink ) );
        
        STL_TRY( knlInitCondVar( &SMKL_LOCK_TABLE(i).mTransLock,
                                 (knlEnv*)aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smklUnlockNode( smklNode * aLockNode,
                          smlEnv   * aEnv )
{
    smklItemCore     * sLockItem;
    knlLatch         * sLatch;
    smklHashBucket   * sHashBucket;
    stlInt32           sState = 0;
    stlUInt32          sLockMode;
    smklNode         * sReqLockNode;
    smklNode         * sNxtReqLockNode;
    smklNode         * sHoldLockNode;
    stlBool            sIsTimedOut;
    stlInt32           i;
    stlInt32           sLatchCount;
    stlInt32           sHoldLockCount;

    sLockItem = aLockNode->mLockItem;

    if( sLockItem->mItemType == SMKL_ITEM_RELATION )
    {
        sLatch = ((smklItemRelation*)sLockItem)->mLatch;
        sLatchCount = SMKL_RELATION_LATCH_COUNT;
    }
    else
    {
        sHashBucket = smklGetHashBucket( ((smklItemRecord*)sLockItem)->mRid );
        sLatch = &sHashBucket->mLatch;
        sLatchCount = 1;
    }

    for( i = 0; i < sLatchCount; i++ )
    {
        STL_TRY( knlAcquireLatch( &sLatch[i],
                                  KNL_LATCH_MODE_EXCLUSIVE,
                                  KNL_LATCH_PRIORITY_NORMAL,
                                  STL_INFINITE_TIME,
                                  &sIsTimedOut,
                                  (knlEnv*)aEnv )
                 == STL_SUCCESS );
    }
    sState = 1;

    /**
     * request에 연결되어 있는 상태에서 트랜잭션이 진행되는 경우라도
     * 처리가 가능해야 한다.
     * - 이러한 경우는 발생해서는 안된다.
     */
    STL_RING_FOREACH_ENTRY_SAFE( &sLockItem->mRequestList, sReqLockNode, sNxtReqLockNode )
    {
        if( sReqLockNode == aLockNode )
        {
            STL_RING_UNLINK( &sLockItem->mRequestList, sReqLockNode );
            STL_DASSERT( STL_FALSE );
            STL_THROW( RAMP_FINISH );
        }
    }
    
    STL_RING_UNLINK( &sLockItem->mHoldList, aLockNode );

    sLockMode = SML_LOCK_NONE;
        
    STL_RING_FOREACH_ENTRY( &sLockItem->mRequestList, sReqLockNode )
    {
        SMKL_REMOVE_EDGE( smklGetLockSlotId( sReqLockNode->mTransId ),
                          smklGetLockSlotId( aLockNode->mTransId ) );
    }
    
    if( STL_RING_IS_EMPTY( &sLockItem->mHoldList ) == STL_TRUE )
    {
        STL_RING_FOREACH_ENTRY_SAFE( &sLockItem->mRequestList, sReqLockNode, sNxtReqLockNode )
        {
            if( sLockItem->mBeFree == STL_TRUE )
            {
                STL_RING_UNLINK( &sLockItem->mRequestList, sReqLockNode );
                STL_TRY( smxlResume( sReqLockNode->mTransId,
                                     sReqLockNode->mEnv,
                                     aEnv ) == STL_SUCCESS );
            }
            else
            {
                if( gLockCompatibleMatrix[sLockMode][sReqLockNode->mLockMode] == STL_FALSE )
                {
                    STL_RING_FOREACH_ENTRY( &sLockItem->mHoldList, sHoldLockNode )
                    {
                        SMKL_ADD_EDGE( smklGetLockSlotId( sReqLockNode->mTransId ),
                                       smklGetLockSlotId( sHoldLockNode->mTransId ) );
                    }
                }
                else
                {
                    STL_RING_UNLINK( &sLockItem->mRequestList, sReqLockNode );
                    STL_RING_ADD_LAST( &sLockItem->mHoldList, sReqLockNode );
            
                    sLockMode = gLockConversionMatrix[sLockMode][sReqLockNode->mLockMode];
                }
            }
        }

        STL_RING_FOREACH_ENTRY( &sLockItem->mHoldList, sHoldLockNode )
        {
            STL_TRY( smxlResume( sHoldLockNode->mTransId,
                                 sHoldLockNode->mEnv,
                                 aEnv ) == STL_SUCCESS );
        }
                
        /*
         * Adjust Pessimistic Mode
         */
        sLockItem->mIsPessimistic = STL_FALSE;

        if( STL_RING_IS_EMPTY( &sLockItem->mHoldList ) == STL_FALSE )
        {
            sLockItem->mIsPessimistic = STL_TRUE;
        }
        else
        {
            STL_DASSERT( STL_RING_IS_EMPTY( &sLockItem->mRequestList ) == STL_TRUE );
        }
    }
    else
    {
        sHoldLockCount = 0;
        
        STL_RING_FOREACH_ENTRY( &sLockItem->mHoldList, sHoldLockNode )
        {
            sHoldLockCount += 1;
        }

        /**
         * Lock을 소유하고 있는 트랜잭션이 하나인 경우는 Lock Escalation 실패로 인하여
         * request list에 연결된 lock node를 merge 한다.
         */
        
        if( sHoldLockCount == 1 )
        {
            sHoldLockNode = STL_RING_GET_FIRST_DATA( &sLockItem->mHoldList );

            STL_RING_FOREACH_ENTRY_SAFE( &sLockItem->mRequestList, sReqLockNode, sNxtReqLockNode )
            {
                if( SMXL_TRANS_SLOT_ID(sReqLockNode->mTransId) == SMXL_TRANS_SLOT_ID(sHoldLockNode->mTransId) )
                {
                    STL_RING_UNLINK( &sLockItem->mRequestList, sReqLockNode );
                    sReqLockNode->mLockItem = NULL;
                    sHoldLockNode->mLockMode = gLockConversionMatrix[sReqLockNode->mLockMode][sHoldLockNode->mLockMode];
                    
                    STL_TRY( smxlResume( sReqLockNode->mTransId,
                                         sReqLockNode->mEnv,
                                         aEnv ) == STL_SUCCESS );
                    
                    break;
                }
            }
        }

        /**
         * Granted lock mode를 다시 계산한다.
         */
        
        STL_RING_FOREACH_ENTRY( &sLockItem->mHoldList, sHoldLockNode )
        {
            sLockMode = gLockConversionMatrix[sLockMode][sHoldLockNode->mLockMode];
        }
    }
        
    sLockItem->mGrantedMode = sLockMode;

    STL_DASSERT( smklValidateLockItem( sLockItem ) == STL_TRUE );
    
    if( sLockItem->mItemType == SMKL_ITEM_RECORD )
    {
        STL_DASSERT( (sLockItem->mGrantedMode == SML_LOCK_NONE) ||
                     (sLockItem->mGrantedMode == SML_LOCK_S)    ||
                     (sLockItem->mGrantedMode == SML_LOCK_X) );
                     
        if( STL_RING_IS_EMPTY( &sLockItem->mHoldList ) == STL_TRUE )
        {
            STL_DASSERT( STL_RING_IS_EMPTY( &sLockItem->mRequestList ) == STL_TRUE );
            STL_TRY( smklRemoveHashItem( (smklItemRecord*)sLockItem,
                                         aEnv ) == STL_SUCCESS );
        }
    }

    STL_RAMP( RAMP_FINISH );
        
    sState = 0;
    for( i = 0; i < sLatchCount; i++ )
    {
        STL_TRY( knlReleaseLatch( &sLatch[i],
                                  (knlEnv*)aEnv ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        for( i = 0; i < sLatchCount; i++ )
        {
            (void)knlReleaseLatch( &sLatch[i], (knlEnv*)aEnv );
        }
    }

    return STL_FAILURE;
}

stlStatus smklLockPessimistic( smxlTransId     aTransId,
                               smklItemCore  * aLockItem,
                               stlUInt32       aLockMode,
                               stlInt64        aTimeInterval,
                               knlLatch      * aLatch,
                               stlInt32        aLatchCount,
                               stlBool       * aSkipLock,
                               smlEnv        * aEnv )
{
    stlInt16       sLockSlotId;
    smklLockSlot * sLockSlot;
    stlBool        sIsTimedOut;
    smklNode     * sLockNode;
    smklNode     * sReqLockNode;
    smklNode     * sNextReqLockNode;
    stlInt32       sDeadlockState = 0;
    stlInt32       sGroupEdgeState = 0;
    stlInt32       sState = 1;
    stlBool        sWaitFlag = STL_FALSE;
    stlBool        sIsDeadlock;
    stlUInt32      sErrCode;
    stlInt32       i;

    sLockSlotId = smklGetLockSlotId( aTransId );
    sLockSlot = smklGetLockSlot( sLockSlotId );
    
    aLockItem->mIsPessimistic = STL_TRUE;

    if( aSkipLock != NULL )
    {
        *aSkipLock = STL_FALSE;
    }

    /**
     * 기존에 이미 락을 획득한 경우라면 기존것을 제거하고 다시 삽입한다.
     * 그렇지 않다면 새로운 락노드를 삽입한다.
     */
    sLockNode = smklFindLockNode( aTransId, aLockItem );

    if( sLockNode != NULL )
    {
        /**
         * No Wait 모드에서는 Lock Conflict가 발생할경우 에러를 리턴한다.
         */
        if( aTimeInterval == SML_LOCK_TIMEOUT_NOWAIT )
        {
            STL_TRY( smklCheckLockConflict( aLockItem,
                                            sLockNode,
                                            aLockMode,
                                            aEnv )
                     == STL_SUCCESS );
        }
        
        STL_TRY( smklAddLockNode( aTransId,
                                  sLockSlot,
                                  sLockNode,
                                  aLockMode,
                                  &sLockNode,
                                  &sWaitFlag,
                                  aEnv ) == STL_SUCCESS );
    }
    else
    {
        /**
         * No Wait 모드에서는 Lock Conflict가 발생할경우 에러를 리턴한다.
         */
        if( aTimeInterval == SML_LOCK_TIMEOUT_NOWAIT )
        {
            STL_TRY( smklCheckLockConflict( aLockItem,
                                            NULL,
                                            aLockMode,
                                            aEnv )
                     == STL_SUCCESS );
        }
        
        STL_TRY( smklAddNewLockNode( aTransId,
                                     sLockSlot,
                                     aLockItem,
                                     aLockMode,
                                     &sLockNode,
                                     &sWaitFlag,
                                     aEnv ) == STL_SUCCESS );
    }
    
    /*
     * 자신의 락노드가 Request List에 삽입되었다면 대기해야 한다.
     */
    if( sWaitFlag == STL_TRUE )
    {
        smxlSetTransState( aTransId, SMXL_STATE_BLOCK );
        
        sState = 0;
        for( i = 0; i < aLatchCount; i++ )
        {
            STL_TRY( knlReleaseLatch( &aLatch[i],
                                      (knlEnv*)aEnv )
                     == STL_SUCCESS );
        }

        if( KNL_SESSION_TYPE( KNL_SESS_ENV(aEnv) ) == KNL_SESSION_TYPE_SHARED )
        {
            SMKL_ADD_EDGE( (gSmklLockTableSize + KNL_REQUEST_GROUP_ID(aEnv)), sLockSlotId );
            sGroupEdgeState = 1;
        }

        while( 1 )
        {
            if( smxlSuspend( aTransId, aTimeInterval, aEnv ) != STL_SUCCESS )
            {
                sErrCode = stlGetLastErrorCode( KNL_ERROR_STACK( aEnv ) );
                
                if( sErrCode == SML_ERRCODE_DEADLOCK )
                {
                    STL_TRY( knlAcquireLatch( gSmklDeadlockLatch,
                                              KNL_LATCH_MODE_EXCLUSIVE,
                                              KNL_LATCH_PRIORITY_NORMAL,
                                              STL_INFINITE_TIME,
                                              &sIsTimedOut,
                                              (knlEnv*)aEnv )
                             == STL_SUCCESS );
                    sDeadlockState = 1;

                    STL_TRY( smklCheckDeadlock( aTransId,
                                                &sIsDeadlock,
                                                aEnv ) == STL_SUCCESS );

                    if( sIsDeadlock != STL_TRUE )
                    {
                        (void)stlPopError( KNL_ERROR_STACK( aEnv ) );
                        
                        sDeadlockState = 0;
                        STL_TRY( knlReleaseLatch( gSmklDeadlockLatch, (knlEnv*)aEnv ) == STL_SUCCESS );

                        continue;
                    }
                    else
                    {
                        smklResolveDeadlock( aTransId );
                    }
                }
                
                for( i = 0; i < aLatchCount; i++ )
                {
                    STL_TRY( knlAcquireLatch( &aLatch[i],
                                              KNL_LATCH_MODE_EXCLUSIVE,
                                              KNL_LATCH_PRIORITY_NORMAL,
                                              STL_INFINITE_TIME,
                                              &sIsTimedOut,
                                              (knlEnv*)aEnv )
                             == STL_SUCCESS );
                }
                sState = 1;

                /**
                 * Unlock시 무시하기 위해서 Lock Item을 Null로 초기화 한다.
                 * - request list에 없을 수도 있다.
                 */

                STL_RING_FOREACH_ENTRY_SAFE( &aLockItem->mRequestList,
                                             sReqLockNode,
                                             sNextReqLockNode )
                {
                    if( SMXL_TRANS_SLOT_ID(sReqLockNode->mTransId) ==
                        SMXL_TRANS_SLOT_ID(aTransId) )
                    {
                        STL_RING_UNLINK( &aLockItem->mRequestList, sReqLockNode );
                        sReqLockNode->mLockItem = NULL;
                        break;
                    }
                }

                STL_DASSERT( smklValidateLockItem( aLockItem ) == STL_TRUE );
                
                sState = 0;
                for( i = 0; i < aLatchCount; i++ )
                {
                    STL_TRY( knlReleaseLatch( &aLatch[i],
                                              (knlEnv*)aEnv )
                             == STL_SUCCESS );
                }

                if( sDeadlockState == 1 )
                {
                    sDeadlockState = 0;
                    STL_TRY( knlReleaseLatch( gSmklDeadlockLatch,
                                              (knlEnv*)aEnv )
                             == STL_SUCCESS );
                }

                /**
                 * suspend 이후 resume이 발생했다면 semaphore정보를 reset해야 한다.
                 */
                STL_TRY( knlResetLostSemValue( KNL_ENV(aEnv) ) == STL_SUCCESS );

                STL_TRY( STL_FALSE );
            }
            else
            {
                for( i = 0; i < aLatchCount; i++ )
                {
                    STL_TRY( knlAcquireLatch( &aLatch[i],
                                              KNL_LATCH_MODE_EXCLUSIVE,
                                              KNL_LATCH_PRIORITY_NORMAL,
                                              STL_INFINITE_TIME,
                                              &sIsTimedOut,
                                              (knlEnv*)aEnv )
                             == STL_SUCCESS );
                }
                sState = 1;
                
                /**
                 * 지워질 Lock Item은 무시한다.
                 */
                if( aLockItem->mBeFree == STL_TRUE )
                {
                    sLockNode->mLockItem = NULL;

                    if( aSkipLock != NULL )
                    {
                        *aSkipLock = STL_TRUE;
                    }

                    sState = 0;
                    for( i = 0; i < aLatchCount; i++ )
                    {
                        STL_TRY( knlReleaseLatch( &aLatch[i],
                                                  (knlEnv*)aEnv )
                                 == STL_SUCCESS );
                    }
                }

#ifdef STL_DEBUG
                /**
                 * to detect abnormal semaphore signal
                 */
                STL_RING_FOREACH_ENTRY( &aLockItem->mRequestList, sReqLockNode )
                {
                    STL_DASSERT( SMXL_TRANS_SLOT_ID(sReqLockNode->mTransId) !=
                                 SMXL_TRANS_SLOT_ID(aTransId) );
                }
#endif
                        
                break;
            }
        }

        if( sGroupEdgeState == 1 )
        {
            sGroupEdgeState = 0;
            SMKL_REMOVE_EDGE( (gSmklLockTableSize + KNL_REQUEST_GROUP_ID(aEnv)), sLockSlotId );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 0 )
    {
        for( i = 0; i < aLatchCount; i++ )
        {
            (void)knlAcquireLatch( &aLatch[i],
                                   KNL_LATCH_MODE_EXCLUSIVE,
                                   KNL_LATCH_PRIORITY_NORMAL,
                                   STL_INFINITE_TIME,
                                   &sIsTimedOut,
                                   (knlEnv*)aEnv );
        }
    }

    if( sDeadlockState == 1 )
    {
        (void)knlReleaseLatch( gSmklDeadlockLatch, (knlEnv*)aEnv );
    }

    if( sGroupEdgeState == 1 )
    {
        SMKL_REMOVE_EDGE( (gSmklLockTableSize + KNL_REQUEST_GROUP_ID(aEnv)), sLockSlotId );
    }

    return STL_FAILURE;
}

stlStatus smklRestructure( smlEnv * aEnv )
{
    stlInt64          sNewTransTableSize = 0;
    stlInt64          sOldTransTableSize = 0;
    stlInt64          sSharedRequestQueueCount = 0;
    smklWarmupEntry * sWarmupEntry;

    sWarmupEntry = gSmklWarmupEntry;
    
    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_TRANSACTION_TABLE_SIZE,
                                      &sNewTransTableSize,
                                      KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_SHARED_REQUEST_QUEUE_COUNT,
                                      &sSharedRequestQueueCount,
                                      KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    sOldTransTableSize = smfGetTransTableSize();

    STL_TRY_THROW( sNewTransTableSize != sOldTransTableSize, RAMP_SUCCESS );
    
    sWarmupEntry->mLockTableSize = sNewTransTableSize;
    sWarmupEntry->mLockWaitTableSize = (sNewTransTableSize + sSharedRequestQueueCount);
    
    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV(aEnv),
                        KNL_LOG_LEVEL_WARNING,
                        "[STARTUP] restructure lock table size - old(%d), new(%d)",
                        sOldTransTableSize,
                        sNewTransTableSize )
             == STL_SUCCESS );
    
    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlBool smklValidateLockItem( smklItemCore * aLockItem )
{
    stlBool sResult = STL_TRUE;
    
    if( STL_RING_IS_EMPTY( &aLockItem->mHoldList ) == STL_TRUE )
    {
        if( aLockItem->mGrantedMode != SML_LOCK_NONE )
        {
            sResult = STL_FALSE;
        }
    }

    return sResult;
}

void smklAddEdgeIntoWaitGraph( stlInt32    aTransSlotId,
                               stlInt32    aRequestGroupId )
{
    SMKL_ADD_EDGE( aTransSlotId, (gSmklLockTableSize + aRequestGroupId) );
}

void smklRemoveEdgeFromWaitGraph( stlInt32    aTransSlotId,
                                  stlInt32    aRequestGroupId )
{
    SMKL_REMOVE_EDGE( aTransSlotId, (gSmklLockTableSize + aRequestGroupId) );
}

void smklIncRequestGroupWorker( stlInt32 aRequestGroupId )
{
    gSmklWarmupEntry->mGroupWorkerCount[aRequestGroupId] += 1;
}

void smklDecRequestGroupWorker( stlInt32 aRequestGroupId )
{
    gSmklWarmupEntry->mGroupWorkerCount[aRequestGroupId] -= 1;
}

void smklGetSystemInfo( smklSystemInfo * aSystemInfo )
{
    aSystemInfo->mLockTableSize = gSmklWarmupEntry->mLockTableSize;
    aSystemInfo->mLockWaitTableSize = gSmklWarmupEntry->mLockWaitTableSize;
    aSystemInfo->mLockHashTableSize = gSmklWarmupEntry->mLockHashTableSize;
}

/** @} */
