/*******************************************************************************
 * knaLatch.c
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
 * @file knaLatch.c
 * @brief Kernel Layer Latch implementation routines
 */

#include <stl.h>
#include <dtl.h>
#include <knlDef.h>
#include <knDef.h>
#include <knl.h>
#include <knaLatch.h>


knlLatchScanCallback  gLatchScanCallback[KNL_LATCH_MAX_CLASS_COUNT];
stlUInt16             gCurLatchClassCount = 0;

stlChar * knaGetLatchModeString( stlUInt16 aMode )
{
    stlChar * sString = NULL;

    switch( aMode )
    {
        case KNL_LATCH_MODE_INIT :
            sString = "INITIAL";
            break;
        case KNL_LATCH_MODE_SHARED :
            sString = "SHARED";
            break;
        case KNL_LATCH_MODE_EXCLUSIVE :
            sString = "EXCLUSIVE";
            break;
        default :
            STL_ASSERT( STL_FALSE );
            break;
    }

    return sString;
}


stlStatus knaInitLatch( knlLatch      * aLatch,
                        stlBool         aIsInShm,
                        knlLogicalAddr  aLogicalAddr,
                        knlEnv        * aEnv )
{
    STL_PARAM_VALIDATE( aLatch != NULL, KNL_ERROR_STACK(aEnv) );

    KNA_INIT_CAS_MISS_CNT( aLatch );
    STL_INIT_SPIN_LOCK( aLatch->mSpinLock );

    aLatch->mExclOwner    = NULL;
    aLatch->mCurMode      = KNL_LATCH_MODE_INIT;
    aLatch->mRefCount     = 0;
    aLatch->mWaitCount    = 0;
    aLatch->mExclLockSeq  = 0;
    aLatch->mLogicalAddr  = aLogicalAddr;

    STL_RING_INIT_HEADLINK( &aLatch->mWaiters, STL_OFFSETOF(knlLatchWaiter, mRing) );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

void knaFinLatch( knlLatch      * aLatch )
{
    STL_ASSERT( (aLatch->mSpinLock == STL_SPINLOCK_STATUS_INIT) &&
                (aLatch->mCurMode  == KNL_LATCH_MODE_INIT) &&
                (aLatch->mRefCount == 0) );
}

stlStatus knaAcquireLatchInternal( knlLatch    * aLatch,
                                   stlUInt16     aMode,
                                   stlInt32      aPriority,
                                   stlInterval   aTimeout,
                                   stlInt64      aRetryCount,
                                   stlBool     * aIsTimedOut,
                                   stlBool     * aIsSuccess,
                                   knlEnv      * aEnv )
{
    stlBool           sWaitFlag = STL_FALSE;
    knlLatchWaiter  * sWaiter;
    knlLatchWaiter  * sMyWaiter = KNL_WAITITEM(aEnv);
    stlBool           sFound;
    stlInt64          sRetryCount = 0;
    stlBool           sIsSpinSuccess = STL_TRUE;
    stlUInt64         sCasMissCnt = 0;
    stlThreadHandle * sMyThread;

    *aIsSuccess = STL_FALSE;
    
    if( KNL_IS_SHARED_ENV( aEnv ) == STL_TRUE )
    {
        sMyThread = &(aEnv->mThread);
    }
    else
    {
        sMyThread = NULL;
    }
    
    KNL_ENTER_NON_CANCELLABLE_SECTION( aEnv );
    KNL_ENTER_CRITICAL_SECTION( aEnv );
    
    STL_RAMP( RAMP_RETRY );

    /**
     * Exclusive Mode로 Try Latch를 시도하는 경우
     * Spin Locking 중간에 interleaving 된다면 dead time이 발생할수 있다.
     * 이를 해결하기 위해서는 Spin Lock의 Try방식을 지원해야 한다.
     * 결국 한번이라도 Spin Locking이 실패하면 Latch 획득 실패로 간주한다.
     */
    if( aTimeout == 0 )
    {
        if( aMode == KNL_LATCH_MODE_EXCLUSIVE )
        {
            knaTrySpinLock4EnterLatch( aLatch,
                                       aMode,
                                       &sIsSpinSuccess,
                                       aEnv );

            if( sIsSpinSuccess == STL_FALSE )
            {
                *aIsSuccess = STL_FALSE;
                
                KNL_LEAVE_CRITICAL_SECTION( aEnv );
                KNL_LEAVE_NON_CANCELLABLE_SECTION( aEnv );
                
                STL_THROW( RAMP_FINISH );
            }
        }
        else
        {
            /**
             * 테스트 과정을 거쳐서 래치를 잡을수 있다고 판단되면
             * 재시도한다.
             */
            
            if( aLatch->mCurMode == KNL_LATCH_MODE_EXCLUSIVE )
            {
                *aIsSuccess = STL_FALSE;
                
                KNL_LEAVE_CRITICAL_SECTION( aEnv );
                KNL_LEAVE_NON_CANCELLABLE_SECTION( aEnv );
                
                STL_THROW( RAMP_FINISH );
            }
            
            stlAcquireSpinLock( &aLatch->mSpinLock,
                                &sCasMissCnt );
            KNA_ADD_CAS_MISS_CNT( aLatch, sCasMissCnt );
        }
    }
    else
    {
        stlAcquireSpinLock( &aLatch->mSpinLock,
                            &sCasMissCnt );
        KNA_ADD_CAS_MISS_CNT( aLatch, sCasMissCnt );
    }
    
    if( aMode == KNL_LATCH_MODE_SHARED )
    {
        if( (aLatch->mCurMode == KNL_LATCH_MODE_SHARED) ||
            (aLatch->mCurMode == KNL_LATCH_MODE_INIT) )
        {
            /* 호환됨 */
            aLatch->mCurMode = KNL_LATCH_MODE_SHARED;
            aLatch->mRefCount++;

            stlReleaseSpinLock( &aLatch->mSpinLock );
            
            KNL_PUSH_LATCH( aEnv, aLatch );
            sWaitFlag = STL_FALSE;
        }
        else
        {
            /* 호환되지 않음 */
            sWaitFlag = STL_TRUE;
        }
    }
    else if( aMode == KNL_LATCH_MODE_EXCLUSIVE )
    {
        if( aLatch->mCurMode == KNL_LATCH_MODE_INIT)
        {
            /* 호환됨 */
            aLatch->mCurMode = KNL_LATCH_MODE_EXCLUSIVE;
            STL_ASSERT( aLatch->mRefCount == 0 );
            aLatch->mRefCount++;

            aLatch->mExclLockSeq++;
            aLatch->mExclOwner = sMyThread;
            
            stlReleaseSpinLock( &aLatch->mSpinLock );
            
            KNL_PUSH_LATCH( aEnv, aLatch );
            sWaitFlag = STL_FALSE;
        }
        else
        {
            if( aLatch->mCurMode == KNL_LATCH_MODE_EXCLUSIVE )
            {
                if( aLatch->mExclOwner != NULL )
                {
                    if( aLatch->mExclOwner == &(aEnv->mThread) )
                    {
                        /* 호환됨 */
                        aLatch->mRefCount++;

                        stlReleaseSpinLock( &aLatch->mSpinLock );
            
                        KNL_PUSH_LATCH( aEnv, aLatch );
                        sWaitFlag = STL_FALSE;
                    }
                    else
                    {
                        /* 호환되지 않음 */
                        sWaitFlag = STL_TRUE;
                    }
                }
                else
                {
                    /* 호환되지 않음 */
                    sWaitFlag = STL_TRUE;
                }
            }
            else
            {
                /* 호환되지 않음 */
                sWaitFlag = STL_TRUE;
            }
        }
    }
    else
    {
        /* 프로그램 버그 상황 */
        STL_ASSERT( STL_FALSE );
    }

    if( sWaitFlag == STL_TRUE )
    {
        if( (aRetryCount < STL_INT64_MAX) && (aRetryCount == sRetryCount) )
        {
            *aIsSuccess = STL_FALSE;
            stlReleaseSpinLock( &aLatch->mSpinLock );
            
            KNL_LEAVE_CRITICAL_SECTION( aEnv );
            KNL_LEAVE_NON_CANCELLABLE_SECTION( aEnv );
            STL_THROW( RAMP_FINISH );
        }

        if( aTimeout == 0 )
        {
            /* TryLatch */
            stlReleaseSpinLock( &aLatch->mSpinLock );
            
            KNL_LEAVE_CRITICAL_SECTION( aEnv );
            KNL_LEAVE_NON_CANCELLABLE_SECTION( aEnv );
            *aIsSuccess = STL_FALSE;
        }
        else
        {
            sMyWaiter->mReqMode = aMode;

            if( aPriority == KNL_LATCH_PRIORITY_HIGH )
            {
                STL_RING_ADD_FIRST( &aLatch->mWaiters,
                                    sMyWaiter );
            }
            else
            {
                STL_RING_ADD_LAST( &aLatch->mWaiters,
                                   sMyWaiter );
            }
            
            ++(aLatch->mWaitCount);
            stlReleaseSpinLock( &aLatch->mSpinLock );
            
            KNL_LEAVE_CRITICAL_SECTION( aEnv );
            
            STL_TRY( stlTimedAcquireSemaphore( &sMyWaiter->mSem,
                                               aTimeout,
                                               aIsTimedOut,
                                               KNL_ERROR_STACK(aEnv) )
                     == STL_SUCCESS );

            KNL_ENTER_CRITICAL_SECTION( aEnv );
            
            /*
             * Timeout이 발생하지 않았다면 Holder가 Wait List에서 자신을 제거했음을
             * 의미하기 때문에 Hold List 변경없이 다시 수행한다.
             */
            if( *aIsTimedOut == STL_FALSE )
            {
#ifdef STL_DEBUG
                stlAcquireSpinLock( &aLatch->mSpinLock,
                                    &sCasMissCnt );

                STL_RING_FOREACH_ENTRY( &aLatch->mWaiters, sWaiter )
                {
                    STL_DASSERT( sWaiter != sMyWaiter );
                }
                
                stlReleaseSpinLock( &aLatch->mSpinLock );
#endif
                sRetryCount++;
                STL_THROW( RAMP_RETRY );
            }
            
            /*
             * Timeout이 발생한 경우는 Wait List에서 자신의 존재여부에 따라 아래와 같이
             * 처리된다.
             * 1) 자신이 List상에 존재하는 경우
             *    Holder가 자신을 Release하지 않았음을 의미하며, 이러한 경우에는
             *    Timeout을 설정하고 함수를 종료한다.
             * 2) 자신이 List상에 존재하지 않는 경우
             *    Holder가 자신을 Release했음을 의미하며, 이러한 경우에는 자신의
             *    Semaphore count가 1로 증가되었기 때문에 Semaphore count를 감소시키고
             *    함수를 종료한다.
             *    (이러한 경우는 함수를 재시작 할수도 있지만 Timeout 스펙을 지키기 위해
             *    Timeout으로 처리함)
             */
            stlAcquireSpinLock( &aLatch->mSpinLock,
                                &sCasMissCnt );
            KNA_ADD_CAS_MISS_CNT( aLatch, sCasMissCnt );

            sFound = STL_FALSE;
            
            if( aLatch->mWaitCount > 0 )
            {
                STL_RING_FOREACH_ENTRY( &aLatch->mWaiters, sWaiter )
                {
                    if( sWaiter == sMyWaiter )
                    {
                        sFound = STL_TRUE;
                        break;
                    }
                }
            }

            if( sFound == STL_TRUE )
            {
                STL_RING_UNLINK( &aLatch->mWaiters, sMyWaiter );
                --(aLatch->mWaitCount);
                
                stlReleaseSpinLock( &aLatch->mSpinLock );
                
                KNL_LEAVE_CRITICAL_SECTION( aEnv );
                KNL_LEAVE_NON_CANCELLABLE_SECTION( aEnv );
            }
            else
            {
                stlReleaseSpinLock( &aLatch->mSpinLock );
            
                STL_TRY( stlAcquireSemaphore( &sMyWaiter->mSem,
                                              KNL_ERROR_STACK(aEnv) )
                         == STL_SUCCESS );

                KNL_LEAVE_CRITICAL_SECTION( aEnv );
                KNL_LEAVE_NON_CANCELLABLE_SECTION( aEnv );
            }
            
            *aIsSuccess = STL_FALSE;
        }
    }
    else
    {
        *aIsSuccess = STL_TRUE;
    }

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus knaEscalateLatch( knlLatch    * aLatch,
                            stlBool     * aIsSuccess,
                            knlEnv      * aEnv )
{
    stlUInt64 sCasMissCnt = 0;
    
    *aIsSuccess = STL_FALSE;
    
    stlAcquireSpinLock( &aLatch->mSpinLock,
                        &sCasMissCnt );
    KNA_ADD_CAS_MISS_CNT( aLatch, sCasMissCnt );

    STL_ASSERT( aLatch->mCurMode != KNL_LATCH_MODE_INIT );
    
    if( aLatch->mCurMode == KNL_LATCH_MODE_SHARED )
    {
        /**
         * Shared로 획득된 상태에서 RefCount가 1인 경우는 자신만이 락을
         * 획득하고 있는 상태이기 때문에 Lock 상승이 가능하다.
         */
        if( aLatch->mRefCount == 1 )
        {
            if( KNL_IS_SHARED_ENV( aEnv ) == STL_TRUE )
            {
                aLatch->mExclOwner = &(aEnv->mThread);
            }
            aLatch->mCurMode = KNL_LATCH_MODE_EXCLUSIVE;
            aLatch->mExclLockSeq++;
            *aIsSuccess = STL_TRUE;
        }
    }
    else
    {
        /**
         * Exclusive로 획득된 상태에서는 자신만이 락을 획득하고 있음을
         * 보장할수 있기 때문에 Lock 상승이 가능하다.
         */
        if( aLatch->mExclOwner != NULL )
        {
            STL_ASSERT( aLatch->mExclOwner == &(aEnv->mThread) );
        }
        *aIsSuccess = STL_TRUE;
    }
    
    stlReleaseSpinLock( &aLatch->mSpinLock );
    
    return STL_SUCCESS;
}


stlStatus knaReleaseLatch( knlLatch * aLatch,
                           stlInt32   aExclWakeupCount,
                           knlEnv   * aEnv )
{
    knlLatchWaiter * sCurWaiter;
    knlLatchWaiter * sNextWaiter;
    stlUInt32        sWaiterMode = KNL_LATCH_MODE_INIT;
    stlUInt32        sRepresentMode = KNL_LATCH_MODE_INIT;
    stlUInt32        sState = 0;
    stlUInt64        sCasMissCnt = 0;
    stlInt32         sWakeupCount;

    STL_ASSERT( aLatch->mRefCount > 0 );
    STL_ASSERT( aExclWakeupCount > 0 );

    stlAcquireSpinLock( &aLatch->mSpinLock, &sCasMissCnt );
    sState = 1;
    KNA_ADD_CAS_MISS_CNT( aLatch, sCasMissCnt );

    STL_ASSERT( aLatch->mRefCount > 0 );
    STL_ASSERT( (aLatch->mCurMode == KNL_LATCH_MODE_SHARED) || 
                (aLatch->mCurMode == KNL_LATCH_MODE_EXCLUSIVE) );

    aLatch->mRefCount--;
    
    if( aLatch->mCurMode == KNL_LATCH_MODE_EXCLUSIVE )
    {
        if( aLatch->mRefCount == 0 )
        {
            aLatch->mExclLockSeq++;
            aLatch->mExclOwner = NULL;
        }
    }
    
    if( aLatch->mRefCount == 0 )
    {
        aLatch->mCurMode = KNL_LATCH_MODE_INIT;
        
        if( aLatch->mWaitCount > 0 )
        {
            /**
             * 첫번째 대기자의 request mode를 보고 깨울 대기자의
             * 개수를 산정한다.
             */
            
            sCurWaiter = STL_RING_GET_FIRST_DATA( &aLatch->mWaiters );

            if( sCurWaiter->mReqMode == KNL_LATCH_MODE_EXCLUSIVE )
            {
                /**
                 * exclusive mode라면 파라미터로 결정
                 */
                sWakeupCount = aExclWakeupCount;
            }
            else
            {
                /**
                 * @todo shared mode라면 core개수의 반으로 결정
                 */
                sWakeupCount = KNA_MAX_LATCH_WAKEUP_COUNT;
            }

            STL_RING_FOREACH_ENTRY_SAFE( &aLatch->mWaiters, 
                                         sCurWaiter,
                                         sNextWaiter )
            {
                sWakeupCount--;
                sWaiterMode = sCurWaiter->mReqMode;

                if( sRepresentMode == KNL_LATCH_MODE_SHARED )
                {
                    if( sWaiterMode == KNL_LATCH_MODE_EXCLUSIVE )
                    {
                        break;
                    }
                }
                else
                {
                    if( sRepresentMode == KNL_LATCH_MODE_EXCLUSIVE )
                    {
                        if( sWaiterMode == KNL_LATCH_MODE_SHARED )
                        {
                            break;
                        }
                    }
                }
                    
                STL_RING_UNLINK( &aLatch->mWaiters,
                                 sCurWaiter );
                --(aLatch->mWaitCount);
                
                STL_TRY( stlReleaseSemaphore( &sCurWaiter->mSem, KNL_ERROR_STACK(aEnv) )
                         == STL_SUCCESS );
                    
                if( sWakeupCount == 0 )
                {
                    break;
                }

                sRepresentMode = sWaiterMode;
            }
        }
    }

    sState = 0;
    stlReleaseSpinLock( &aLatch->mSpinLock );

    KNL_POP_LATCH( aEnv, aLatch );
    
    KNL_LEAVE_CRITICAL_SECTION( aEnv );
    KNL_LEAVE_NON_CANCELLABLE_SECTION( aEnv );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        stlReleaseSpinLock( &aLatch->mSpinLock );
    }
    
    return STL_FAILURE;
}

void knaTrySpinLock4EnterLatch( knlLatch  * aLatch,
                                stlUInt16   aMode,
                                stlBool   * aIsSuccess,
                                knlEnv    * aEnv )
{
    stlUInt32  sSpinCounter;
    stlUInt32  sRetVal;

    *aIsSuccess = STL_TRUE;
    
    sSpinCounter = KNL_TRY_SPIN_COUNT;
    do
    {
        if( aMode == KNL_LATCH_MODE_EXCLUSIVE )
        {
            if( aLatch->mCurMode == KNL_LATCH_MODE_SHARED )
            {
                *aIsSuccess = STL_FALSE;
                break;
            }
            else
            {
                if( aLatch->mCurMode == KNL_LATCH_MODE_EXCLUSIVE )
                {
                    if( aLatch->mExclOwner == NULL )
                    {
                        *aIsSuccess = STL_FALSE;
                        break;
                    }
                    else
                    {
                        if( aLatch->mExclOwner != &(aEnv->mThread) )
                        {
                            *aIsSuccess = STL_FALSE;
                            break;
                        }
                    }
                }
            }
        }
        else
        {
            if( aLatch->mCurMode == KNL_LATCH_MODE_EXCLUSIVE )
            {
                *aIsSuccess = STL_FALSE;
                break;
            }
        }
            
        sRetVal = stlAtomicCas32( &aLatch->mSpinLock,
                                  STL_SPINLOCK_STATUS_LOCKED,
                                  STL_SPINLOCK_STATUS_INIT );
        if( sRetVal == STL_SPINLOCK_STATUS_INIT )
        {
            break;
        }

        sSpinCounter--;
        if( sSpinCounter == 0 )
        {
            *aIsSuccess = STL_FALSE;
            break;
        }
    } while( STL_TRUE );
}

stlStatus knaSuspend( stlInterval   aTimeout,
                      stlBool     * aIsTimedOut,
                      knlEnv      * aEnv )
{
    knlLatchWaiter * sWaiter = KNL_WAITITEM( aEnv );
    stlInt32         sSemValue;
    stlInt32         i;
    
    KNL_ENTER_CRITICAL_SECTION( aEnv );
    
    stlAcquireSpinLock( &sWaiter->mSpinLock,
                        NULL /* Miss count */ );

    /**
     * lost semaphore가 있다면 suspend 없이 성공한다.
     * - timeout 이후 resume이 발생하고 이후 다시 suspend를 호출한 경우
     * - 상위레이어에서 timeout 이후 연산이 종료되는 경우에는 연산이 종료하기 전에
     *   반드시 knaResetLostSemValue()을 수행해야 한다.
     */
    if( sWaiter->mLostSemValue > 0 )
    {
        STL_DASSERT( sWaiter->mLostSemValue == 1 );
        
        *aIsTimedOut = STL_FALSE;
        sWaiter->mLostSemValue -= 1;

        STL_THROW( RAMP_FINISH );
    }
    
    sWaiter->mWaitState = KNL_WAITITEM_STATE_SUSPENDED;
    stlReleaseSpinLock( &sWaiter->mSpinLock );
    
    STL_TRY( stlTimedAcquireSemaphore( &sWaiter->mSem,
                                       aTimeout,
                                       aIsTimedOut,
                                       KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );
    
    stlAcquireSpinLock( &sWaiter->mSpinLock,
                        NULL /* Miss count */ );
    
    STL_TRY( stlGetSemaphoreValue( &sWaiter->mSem,
                                   &sSemValue,
                                   KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    /**
     * semaphore count가 0보다 큰 경우는 timeout이 발생한 이후에
     * 다른 프로세스가 resume을 시킨 경우이다.
     */
    if( sSemValue > 0 )
    {
        STL_DASSERT( *aIsTimedOut == STL_TRUE );
        
        for( i = 0; i < sSemValue; i++ )
        {
            STL_TRY( stlAcquireSemaphore( &sWaiter->mSem,
                                          KNL_ERROR_STACK(aEnv) )
                     == STL_SUCCESS );
        }

        /**
         * Timeout이 아닌 상태를 만든다.
         * - Timeout으로 처리하면 Semaphore Leak으로 인한 hang이 발생할수 있다.
         */
        *aIsTimedOut = STL_FALSE;
    }
    
    sWaiter->mWaitState = KNL_WAITITEM_STATE_RESUMED;

    STL_RAMP( RAMP_FINISH );
    
    stlReleaseSpinLock( &sWaiter->mSpinLock );

    KNL_LEAVE_CRITICAL_SECTION( aEnv );
    
    return STL_SUCCESS;

    STL_FINISH;

    KNL_LEAVE_CRITICAL_SECTION( aEnv );
    
    return STL_FAILURE;
}

stlStatus knaResume( knlEnv * aTargetEnv,
                     knlEnv * aEnv )
{
    knlLatchWaiter * sWaiter = KNL_WAITITEM( aTargetEnv );

    stlAcquireSpinLock( &sWaiter->mSpinLock,
                        NULL /* Miss count */ );

    /**
     * suspend 상태일 경우에만 resume을 시킨다.
     * - session event나 lock timeout에 의해서 suspended process가
     *   스스로 깨워날수도 있다.
     * - 이러한 경우는 semaphore를 풀지 않는다.
     */
    if( sWaiter->mWaitState == KNL_WAITITEM_STATE_SUSPENDED )
    {
        STL_TRY( stlReleaseSemaphore( &sWaiter->mSem,
                                      KNL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );
    }
    else
    {
        sWaiter->mLostSemValue += 1;
        STL_DASSERT( sWaiter->mLostSemValue <= 1 );
    }

    stlReleaseSpinLock( &sWaiter->mSpinLock );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus knaResetLostSemValue( knlEnv * aEnv )
{
    knlLatchWaiter * sWaiter = KNL_WAITITEM( aEnv );

    stlAcquireSpinLock( &sWaiter->mSpinLock,
                        NULL /* Miss Count */ );

    STL_DASSERT( sWaiter->mLostSemValue <= 1 );
    sWaiter->mLostSemValue = 0;

    stlReleaseSpinLock( &sWaiter->mSpinLock );
    
    return STL_SUCCESS;
}

stlStatus knaRegisterLatchScanCallback( knlLatchScanCallback * aCallback,
                                        knlEnv               * aEnv )
{
    STL_PARAM_VALIDATE( aCallback != NULL, KNL_ERROR_STACK(aEnv) );
    STL_ASSERT( gCurLatchClassCount < KNL_LATCH_MAX_CLASS_COUNT );

    gLatchScanCallback[gCurLatchClassCount] = *aCallback;
    gCurLatchClassCount ++;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * X$LATCH Fixed Table 정의
 */

knlFxColumnDesc gLatchColumnDesc[] =
{
    {
        "DESCRIPTION",
        "latch description",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( knlLatchFxRecord, mDesc ),
        KNL_LATCH_MAX_DESC_BUF_SIZE,
        STL_TRUE  /* nullable */
    },
    {
        "REF_COUNT",
        "current reference count",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( knlLatchFxRecord, mRefCount ),
        STL_SIZEOF( stlUInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "SPIN_LOCK",
        "spin lock",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( knlLatchFxRecord, mSpinLock ),
        STL_SIZEOF( stlUInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "WAIT_COUNT",
        "wait count",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( knlLatchFxRecord, mWaitCount ),
        STL_SIZEOF( stlUInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "X_LOCK_SEQ",
        "exclusive lock sequence",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knlLatchFxRecord, mExclLockSeq ),
        STL_SIZEOF( stlUInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "CURRENT_MODE",
        "current latch mode",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( knlLatchFxRecord, mCurMode ),
        10,
        STL_FALSE  /* nullable */
    },
    {
        "CAS_MISS_COUNT",
        "Cas miss count",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knlLatchFxRecord, mCasMissCnt ),
        STL_SIZEOF( stlUInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        NULL,
        NULL,
        0,
        0,
        0,
        STL_FALSE  /* nullable */
    }
};

stlStatus openFxLatchCallback( void   * aStmt,
                               void   * aDumpObjHandle,
                               void   * aPathCtrl,
                               knlEnv * aEnv )
{
    // 최초 읽을 레코드 위치
    knlLatchPathControl * sPCtl = (knlLatchPathControl*)aPathCtrl;

    sPCtl->mCurLatchClass = 0;
    sPCtl->mPrevLatch = NULL;
    sPCtl->mPosInfo[0] = '\0';

    if( gCurLatchClassCount > 0 )
    {
        STL_TRY( gLatchScanCallback[sPCtl->mCurLatchClass].mOpen()
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus closeFxLatchCallback( void   * aDumpObjHandle,
                                void   * aPathCtrl,
                                knlEnv * aEnv )
{
    knlLatchPathControl * sPCtl = (knlLatchPathControl*)aPathCtrl;

    if( (gCurLatchClassCount > 0) && (sPCtl->mCurLatchClass < gCurLatchClassCount) )
    {
        STL_TRY( gLatchScanCallback[sPCtl->mCurLatchClass].mClose()
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus buildRecordFxLatchCallback( void              * aDumpObjHandle,
                                      void              * aPathCtrl,
                                      knlValueBlockList * aValueList,
                                      stlInt32            aBlockIdx,
                                      stlBool           * aTupleExist,
                                      knlEnv            * aEnv )
{
    knlLatchPathControl * sPCtl = (knlLatchPathControl*)aPathCtrl;
    knlLatchFxRecord      sLatchRec;

    *aTupleExist = STL_FALSE;

    while( sPCtl->mCurLatchClass < gCurLatchClassCount )
    {
        STL_TRY( gLatchScanCallback[sPCtl->mCurLatchClass].mGetNext( &sPCtl->mPrevLatch,
                                                                     sLatchRec.mDesc,
                                                                     sPCtl->mPosInfo,
                                                                     aEnv )
                 == STL_SUCCESS );
        if( sPCtl->mPrevLatch == NULL )
        {
            STL_TRY( gLatchScanCallback[sPCtl->mCurLatchClass].mClose()
                     == STL_SUCCESS );
            sPCtl->mCurLatchClass++;
            sPCtl->mPosInfo[0] = '\0';
            if( sPCtl->mCurLatchClass >= gCurLatchClassCount )
            {
                *aTupleExist = STL_FALSE;
                break;
            }
                
            STL_TRY( gLatchScanCallback[sPCtl->mCurLatchClass].mOpen()
                     == STL_SUCCESS );
            STL_TRY( gLatchScanCallback[sPCtl->mCurLatchClass].mGetNext( &sPCtl->mPrevLatch,
                                                                          sLatchRec.mDesc,
                                                                          sPCtl->mPosInfo,
                                                                          aEnv )
                     == STL_SUCCESS );
        }

        switch( sPCtl->mPrevLatch->mCurMode )
        {
            case KNL_LATCH_MODE_INIT:
            {
                stlStrcpy(sLatchRec.mCurMode, "INITIAL");
                break;
            }
            case KNL_LATCH_MODE_SHARED:
            {
                stlStrcpy(sLatchRec.mCurMode, "SHARED");
                break;
            }
            case KNL_LATCH_MODE_EXCLUSIVE:
            {
                stlStrcpy(sLatchRec.mCurMode, "EXCLUSIVE");
                break;
            }
            default:
            {
                STL_ASSERT( STL_FALSE );
            }
        }

        sLatchRec.mExclLockSeq = sPCtl->mPrevLatch->mExclLockSeq;
        sLatchRec.mSpinLock = sPCtl->mPrevLatch->mSpinLock;
        sLatchRec.mRefCount = sPCtl->mPrevLatch->mRefCount;
        sLatchRec.mWaitCount = sPCtl->mPrevLatch->mWaitCount;
        sLatchRec.mCasMissCnt = KNA_GET_CAS_MISS_CNT( sPCtl->mPrevLatch );

        STL_TRY( knlBuildFxRecord( gLatchColumnDesc,
                                   &sLatchRec,
                                   aValueList,
                                   aBlockIdx,
                                   aEnv )
                 == STL_SUCCESS );
        *aTupleExist = STL_TRUE;
        break;
    }

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


knlFxTableDesc gLatchTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    NULL,
    openFxLatchCallback,
    closeFxLatchCallback,
    buildRecordFxLatchCallback,
    STL_SIZEOF( knlLatchPathControl ),
    "X$LATCH",
    "latch information fixed table",
    gLatchColumnDesc
};



/** @} */
