/*******************************************************************************
 * knpcClock.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: knpcClock.c 9033 2013-07-17 08:40:27Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file knpcClock.c
 * @brief Plan Clock
 */

#include <knl.h>
#include <knDef.h>
#include <knpc.h>

extern knsEntryPoint * gKnEntryPoint;

/**
 * @brief Session에 해당하는 Clock을 얻는다.
 * @param[out]    aClock   Clock Pointer
 * @param[in,out] aEnv     environment 정보
 */
stlStatus knpcGetClock( knpcClock ** aClock,
                        knlEnv     * aEnv )
{
    knpcClock * sClock;
    stlUInt32   sSessionId;

    STL_TRY( knlGetSessionEnvId( KNL_SESS_ENV(aEnv),
                                 &sSessionId,
                                 aEnv )
             == STL_SUCCESS );
    
    sClock = gKnEntryPoint->mClock;

    *aClock = &sClock[sSessionId % gKnEntryPoint->mClockCount];
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Clock을 초기화 한다.
 * @param[in]     aClock           Clock Pointer
 * @param[in]     aClockId         Clock Identifier
 * @param[in]     aMemController   Clock Identifier
 * @param[in,out] aEnv             environment 정보
 */
stlStatus knpcInitClock( knpcClock        * aClock,
                         stlInt32           aClockId,
                         knlMemController * aMemController,
                         knlEnv           * aEnv )
{
    knpcFlangeBlock * sFlangeBlock;
    stlInt32          i;
    stlInt32          sState = 0;
    
    STL_RING_INIT_HEADLINK( &(aClock->mBlockList),
                            STL_OFFSETOF(knpcFlangeBlock, mBlockLink) );
    
    aClock->mClockId = aClockId;

    STL_TRY( knlCreateDynamicMem( &aClock->mDynamicMem,
                                  NULL, /* aParentMem */
                                  KNL_ALLOCATOR_KERNEL_PLAN_CACHE,
                                  KNL_MEM_STORAGE_TYPE_PLAN,
                                  KNPC_PLAN_INIT_SIZE,
                                  KNPC_PLAN_INIT_SIZE,
                                  aMemController,
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( knlCreateDynamicMem( &aClock->mFlangeDynamicMem,
                                  NULL, /* aParentMem */
                                  KNL_ALLOCATOR_KERNEL_FLANGE,
                                  KNL_MEM_STORAGE_TYPE_PLAN,
                                  KNPC_PLAN_INIT_SIZE,
                                  KNPC_PLAN_INIT_SIZE,
                                  aMemController,
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 2;

    STL_TRY( knlAllocDynamicMem( &aClock->mFlangeDynamicMem,
                                 STL_SIZEOF(knpcFlangeBlock),
                                 (void**)&sFlangeBlock,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_RING_INIT_DATALINK( sFlangeBlock,
                            STL_OFFSETOF(knpcFlangeBlock, mBlockLink) );

    for( i = 0; i < KNPC_MAX_FLANGE_COUNT; i++ )
    {
        knpcInitFlange( &sFlangeBlock->mFlanges[i],
                        0,
                        NULL,
                        0,
                        NULL,
                        NULL,
                        NULL,
                        STL_FALSE );
        
        aClock->mFlangeCount = KNPC_MAX_FLANGE_COUNT;
    }

    STL_RING_ADD_FIRST( &(aClock->mBlockList),
                        sFlangeBlock );

    aClock->mMaxFlangeCount = knlGetPropertyBigIntValueByID( KNL_PROPERTY_MAXIMUM_FLANGE_COUNT,
                                                             aEnv );

    aClock->mFreeHand.mBlockHand = sFlangeBlock;
    aClock->mFreeHand.mFlangeHand = 0;
    aClock->mAgingHand.mBlockHand = sFlangeBlock;
    aClock->mAgingHand.mFlangeHand = 0;

    STL_TRY( knlInitLatch( &aClock->mLatch,
                           STL_TRUE, /* aIsInShm */
                           KNL_ENV(aEnv) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) knlDestroyDynamicMem( &aClock->mFlangeDynamicMem, KNL_ENV(aEnv) );
        case 1:
            (void) knlDestroyDynamicMem( &aClock->mDynamicMem, KNL_ENV(aEnv) );
        default:
            break;
    }

    return STL_FAILURE;
}


/**
 * @brief Clock의 Old Plan들을 Aging한다.
 * @param[in]     aClock                  Clock Pointer
 * @param[in]     aDeadAge                Dead Age
 * @param[in]     aAgingThreshold         Aging Threshold
 * @param[in]     aAgingPlanInterval      Aging Plan Interval(sec)
 * @param[out]    aIsSuccess              성공 여부
 * @param[in,out] aEnv                    environment 정보
 * @remark Aging 단위는 Flange Block이다.
 */
stlStatus knpcAgingClock( knpcClock * aClock,
                          stlInt32    aDeadAge,
                          stlInt32    aAgingThreshold,
                          stlInt64    aAgingPlanInterval,
                          stlBool   * aIsSuccess,
                          knlEnv    * aEnv )
{
    stlBool           sIsTimedOut;
    stlInt32          sState = 0;
    stlInt64          sFreedFlange = 0;
    stlInt32          sFlangeHand;
    knpcFlangeBlock * sStartBlock;
    knpcFlangeBlock * sFlangeBlock;
    knpcFlange      * sFlange;
    knpcClockHand   * sAgingHand;
    stlInt32          sTryAge = aDeadAge;
    knpcHashBucket  * sHashBucket;

    *aIsSuccess = STL_FALSE;

    STL_TRY( knlAcquireLatch( &aClock->mLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 1;

    gKnEntryPoint->mClockAgingCount++;

    sAgingHand = &(aClock->mAgingHand);
    
    sStartBlock = sAgingHand->mBlockHand;
    sFlangeHand = sAgingHand->mFlangeHand;
    sFlangeBlock = sAgingHand->mBlockHand;

    while( 1 )
    {
        while( 1 )
        {
            /**
             * Flange Block 전체를 Aging한다.
             */

            while( sFlangeHand < KNPC_MAX_FLANGE_COUNT )
            {
                sFlange = &(sFlangeBlock->mFlanges[sFlangeHand]);

                /**
                 * compare without lock, then compare with lock
                 */
            
                if( (KNPC_IS_EMPTY_FLANGE( sFlange ) == STL_FALSE) &&
                    (sFlange->mPlanCount > 0) &&
                    (sFlange->mRefCount == 0) )
                {
                    sHashBucket = knpcGetHashBucket( sFlange->mHashValue );
        
                    STL_TRY( knlAcquireLatch( &sHashBucket->mLatch,
                                              KNL_LATCH_MODE_EXCLUSIVE,
                                              KNL_LATCH_PRIORITY_NORMAL,
                                              STL_INFINITE_TIME,
                                              &sIsTimedOut,
                                              KNL_ENV( aEnv ) )
                             == STL_SUCCESS );
                    
                    stlAcquireSpinLock( &sFlange->mSpinLock,
                                        NULL /* Miss count */ );
                    sState = 2;

                    /**
                     * Caching은 되어있지만 사용되지 않는 Flange 라면
                     */
                    
                    if( (KNPC_IS_EMPTY_FLANGE( sFlange ) == STL_FALSE) &&
                        (sFlange->mPlanCount > 0) &&
                        (sFlange->mRefCount == 0) )
                    {
                        /**
                         * Age가 기준치를 넘어서면 재사용한다.
                         * - 강제적으로 삭제된 경우에도 재사용한다.
                         */
                        
                        if( (sFlange->mDropped == STL_TRUE) ||
                            (sFlange->mAge >= sTryAge) )
                        {
                            /**
                             * Hash에서 연결을 해제한다.
                             */
                            
                            STL_RING_UNLINK( &(sHashBucket->mOverflowList), sFlange );
                            
                            STL_TRY( knlReleaseLatch( &sHashBucket->mLatch,
                                                      KNL_ENV( aEnv ) ) == STL_SUCCESS );

                            /**
                             * free flange
                             */
                            
                            STL_TRY( knpcFreeFlange( aClock,
                                                     sFlange,
                                                     aEnv )
                                     == STL_SUCCESS );

                            sFreedFlange++;
                        }
                        else
                        {
                            STL_TRY( knlReleaseLatch( &sHashBucket->mLatch,
                                                      KNL_ENV( aEnv ) ) == STL_SUCCESS );
                        
                            /**
                             * SQL 나이를 올린다.
                             */

                            if( (knlGetSystemTime() - sFlange->mLastFixTime) >=
                                STL_SET_SEC_TIME(aAgingPlanInterval) )
                            {
                                /**
                                 * interval time 보다 오래된 plan의 나이를 올린다.
                                 */
                                sFlange->mAge++;
                            }
                        }
                    }
                    else
                    {
                        STL_TRY( knlReleaseLatch( &sHashBucket->mLatch,
                                                  KNL_ENV( aEnv ) ) == STL_SUCCESS );
                    }
                
                    sState = 1;
                    stlReleaseSpinLock( &sFlange->mSpinLock );
                }
                else
                {
                    if( (KNPC_IS_EMPTY_FLANGE( sFlange ) == STL_TRUE) &&
                        (sFlange->mPlanCount == 0) &&
                        (sFlange->mRefCount == 0) )
                    {
                        sFreedFlange++;
                    }
                }

                sFlangeHand++;
            }

            /**
             * move to next block
             */
        
            sFlangeBlock = STL_RING_GET_NEXT_DATA( &aClock->mBlockList, sFlangeBlock );
        
            if( STL_RING_IS_HEADLINK( &(aClock->mBlockList),
                                      sFlangeBlock ) == STL_TRUE )
            {
                sFlangeBlock = STL_RING_GET_FIRST_DATA( &aClock->mBlockList );
            }
            
            sFlangeHand = 0;
        
            if( sFlangeBlock == sStartBlock )
            {
                break;
            }

            /**
             * Aging 해야할 최대 개수를 넘는다면 멈춘다.
             */
        
            if( sFreedFlange > aAgingThreshold )
            {
                break;
            }
        }

        /**
         * Flange가 하나라도 Free되었다면 멈춘다.
         */
        
        if( sFreedFlange > 0 )
        {
            break;
        }

        /**
         * 더이상 Againg할수 없다면
         */
        
        if( sTryAge <= 0 )
        {
            break;
        }
        
        sTryAge--;
    }

    sAgingHand->mBlockHand = sFlangeBlock;
    sAgingHand->mFlangeHand = sFlangeHand;
    
    sState = 0;
    STL_TRY( knlReleaseLatch( &aClock->mLatch,
                              KNL_ENV( aEnv ) ) == STL_SUCCESS );

    if( sFreedFlange > 0 )
    {
        *aIsSuccess = STL_TRUE;
    }

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            stlReleaseSpinLock( &sFlange->mSpinLock );
        case 1:
            (void)knlReleaseLatch( &aClock->mLatch, KNL_ENV( aEnv ) );
        default:
            break;
    }
    
    return STL_FAILURE;
}


/**
 * @brief Clock상의 Empty Flange를 얻는다.
 * @param[in]     aClock       Clock Pointer
 * @param[out]    aIsSuccess   성공 여부
 * @param[in,out] aEnv       environment 정보
 */
stlStatus knpcExtendClock( knpcClock   * aClock,
                           stlBool     * aIsSuccess,
                           knlEnv      * aEnv )
{
    stlBool           sIsTimedOut;
    stlInt32          sState = 0;
    knpcFlangeBlock * sFlangeBlock;
    stlInt32          i;

    *aIsSuccess = STL_FALSE;

    STL_TRY_THROW( aClock->mMaxFlangeCount > (aClock->mFlangeCount + KNPC_MAX_FLANGE_COUNT),
                   RAMP_FINISH );
    
    if( knlAllocDynamicMem( &aClock->mFlangeDynamicMem,
                            STL_SIZEOF(knpcFlangeBlock),
                            (void**)&sFlangeBlock,
                            KNL_ENV(aEnv) )
        != STL_SUCCESS )
    {
        stlPopError( KNL_ERROR_STACK(aEnv) );
        STL_THROW( RAMP_FINISH );
    }

    STL_RING_INIT_DATALINK( sFlangeBlock,
                            STL_OFFSETOF(knpcFlangeBlock, mBlockLink) );

    for( i = 0; i < KNPC_MAX_FLANGE_COUNT; i++ )
    {
        knpcInitFlange( &sFlangeBlock->mFlanges[i],
                        0,
                        NULL,
                        0,
                        NULL,
                        NULL,
                        NULL,
                        STL_FALSE );
    }

    STL_TRY( knlAcquireLatch( &aClock->mLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 1;

    /**
     * Free Hand 앞에 삽입한다.
     */
    
    STL_RING_INSERT_AFTER( &(aClock->mBlockList),
                           aClock->mFreeHand.mBlockHand,
                           sFlangeBlock );

    aClock->mFlangeCount += KNPC_MAX_FLANGE_COUNT;

    sState = 0;
    STL_TRY( knlReleaseLatch( &aClock->mLatch,
                              KNL_ENV( aEnv ) ) == STL_SUCCESS );

    *aIsSuccess = STL_TRUE;
    
    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlReleaseLatch( &aClock->mLatch, KNL_ENV( aEnv ) );
    }

    return STL_FAILURE;
}


