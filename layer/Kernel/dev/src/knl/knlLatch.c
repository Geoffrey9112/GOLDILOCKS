/*******************************************************************************
 * knlLatch.c
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
 * @file knlLatch.c
 * @brief Kernel Layer Latch wrapper routines
 */

#include <stl.h>
#include <dtl.h>
#include <knlDef.h>
#include <knl.h>
#include <knaLatch.h>

/**
 * @brief 주어진 래치의 멤버변수들을 초기화 한다
 * @param[in] aLatch 초기화 할 래치의 로지컬 포인터
 * @param[in] aIsInShm 래치가 Shared Memory에 생성되었는지 여부
 * @param[out] aEnv environment 정보
 */
stlStatus knlInitLatch( knlLatch      * aLatch,
                        stlBool         aIsInShm,
                        knlEnv        * aEnv )
{
    knlLogicalAddr sLogicalAddr = KNL_LOGICAL_ADDR_NULL;
    
    if( aIsInShm == STL_TRUE )
    {
        STL_TRY( knlGetLogicalAddr( aLatch,
                                    &sLogicalAddr,
                                    aEnv )
                 == STL_SUCCESS );
    }

    STL_TRY( knaInitLatch( aLatch,
                           aIsInShm,
                           sLogicalAddr,
                           aEnv )
             == STL_SUCCESS);

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 래치의 멤버변수들을 초기화 한다
 * @param[in] aLatch 초기화 할 래치의 로지컬 포인터
 * @param[in] aShmIdx 래치의 Logical Shared Segment
 * @param[out] aEnv environment 정보
 */
stlStatus knlInitDbLatch( knlLatch      * aLatch,
                          stlUInt16       aShmIdx,
                          knlEnv        * aEnv )
{
    knlLogicalAddr   sLogicalAddr = 0;
    void           * sStartAddr;

    STL_TRY( knlGetShmSegmentAddrUnsafe( aShmIdx,
                                         &sStartAddr,
                                         aEnv )
             == STL_SUCCESS );

    sLogicalAddr = KNL_MAKE_LOGICAL_ADDR( aShmIdx, (((void*)aLatch) - sStartAddr) );
    
    STL_TRY( knaInitLatch( aLatch,
                           STL_TRUE,  /* aIsInShm */
                           sLogicalAddr,
                           aEnv )
             == STL_SUCCESS);

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 래치 가 종료 가능한 상태인지 멤버변수들을 체크한다 
 * @param[in] aLatch 체크할 래치의 로지컬 포인터
 */
void knlFinLatch( knlLatch      * aLatch )
{
    knaFinLatch( aLatch );
}

/**
 * @brief 주어진 래치의 주어진 모드로 획득한다
 * @param[in]  aLatch        획득할 래치의 로지컬 포인터
 * @param[in]  aMode         획득할 래치 모드
 * @param[in]  aPriority     우선 순위
 * @param[in]  aTimeout      래치 try 실패시 대기할 최대 시간(microsecond)
 * @param[out] aIsTimedOut   래치 획득에 실패하여 대기하다가 timeout이 발생했는지 여부
 * @param[out] aEnv          environment 정보
 * @note 다중 사용자 환경에서는 Heap Environment를 이용한 대기열 조작이 불가능하기 때문에
 *  <BR> Heap Environment는 knlTryLatch를 사용해서 knlAcquireLatch를 흉내낸다.
 *  <BR> knlTryLatch를 이용한 정확한 Timeout은 구현상 불가능하기 때문애,
 *  <BR> aTimeout은 반드시 무한대(STL_UINT64_MAX)로 설정해야 한다.
 */
stlStatus knlAcquireLatch( knlLatch    * aLatch,
                           stlUInt16     aMode,
                           stlInt32      aPriority,
                           stlInterval   aTimeout,
                           stlBool     * aIsTimedOut,
                           knlEnv      * aEnv )
{
    stlBool sIsSuccess = STL_FALSE;
    
    /*
     * 다중 사용자 환경에서는 Heap Environment를 이용한 대기열 조작이
     * 불가능하기 때문에 Heap Environment는 TryLatch를 사용한다.
     * 이때, aTimeout은 반드시 무한대(STL_UINT64_MAX)로 설정되어 있어야 한다.
     * TryLatch를 이용한 정확한 Timeout은 구현상 불가능하다.
     */
    if( KNL_IS_SHARED_ENV( aEnv ) == STL_TRUE )
    {
        *aIsTimedOut = STL_FALSE;
        
        STL_TRY( knaAcquireLatchInternal( aLatch,
                                          aMode,
                                          aPriority,
                                          aTimeout,
                                          STL_INT64_MAX,
                                          aIsTimedOut,
                                          &sIsSuccess,
                                          aEnv )
                 == STL_SUCCESS );

        if( sIsSuccess == STL_FALSE )
        {
            *aIsTimedOut = STL_TRUE;
        }
    }
    else
    {
        while( sIsSuccess == STL_FALSE )
        {
            STL_TRY( knaAcquireLatchInternal( aLatch,
                                              aMode,
                                              aPriority,
                                              0,
                                              STL_INT64_MAX,
                                              aIsTimedOut,
                                              &sIsSuccess,
                                              aEnv )
                     == STL_SUCCESS );
        }

        *aIsTimedOut = STL_FALSE;
    }

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 모드로의 래치 획득을 aRetryCount만큼 재시도한다
 * @param[in]  aLatch        획득할 래치의 로지컬 포인터
 * @param[in]  aMode         획득할 래치 모드
 * @param[in]  aPriority     우선 순위
 * @param[in]  aTimeout      래치 try 실패시 대기할 최대 시간(microsecond)
 * @param[in]  aRetryCount   Retry Count
 * @param[out]  aIsTimedOut  래치 획득에 실패하여 대기하다가 timeout이 발생했는지 여부
 * @param[out]  aIsSuccess   Success 여부
 * @param[out]  aEnv         environment 정보
 * @note 다중 사용자 환경에서는 Heap Environment를 이용한 대기열 조작이 불가능하기 때문에
 *  <BR> Heap Environment는 knlTryLatch를 사용해서 knlAcquireLatch를 흉내낸다.
 *  <BR> knlTryLatch를 이용한 정확한 Timeout은 구현상 불가능하기 때문애,
 *  <BR> aTimeout은 반드시 무한대(STL_UINT64_MAX)로 설정해야 한다.
 */
stlStatus knlAcquireLatchWithInfo( knlLatch    * aLatch,
                                   stlUInt16     aMode,
                                   stlInt32      aPriority,
                                   stlInterval   aTimeout,
                                   stlInt64      aRetryCount,
                                   stlBool     * aIsTimedOut,
                                   stlBool     * aIsSuccess,
                                   knlEnv      * aEnv )
{
    *aIsSuccess = STL_FALSE;
    
    /*
     * 다중 사용자 환경에서는 Heap Environment를 이용한 대기열 조작이
     * 불가능하기 때문에 Heap Environment는 TryLatch를 사용한다.
     * 이때, aTimeout은 반드시 무한대(STL_UINT64_MAX)로 설정되어 있어야 한다.
     * TryLatch를 이용한 정확한 Timeout은 구현상 불가능하다.
     */
    if( KNL_IS_SHARED_ENV( aEnv ) == STL_TRUE )
    {
        *aIsTimedOut = STL_FALSE;
        
        STL_TRY( knaAcquireLatchInternal( aLatch,
                                          aMode,
                                          aPriority,
                                          aTimeout,
                                          aRetryCount,
                                          aIsTimedOut,
                                          aIsSuccess,
                                          aEnv )
                 == STL_SUCCESS );

        if( *aIsSuccess == STL_FALSE )
        {
            *aIsTimedOut = STL_TRUE;
        }
    }
    else
    {
        while( *aIsSuccess == STL_FALSE )
        {
            STL_TRY( knaAcquireLatchInternal( aLatch,
                                              aMode,
                                              aPriority,
                                              0,
                                              aRetryCount,
                                              aIsTimedOut,
                                              aIsSuccess,
                                              aEnv )
                     == STL_SUCCESS );
        }

        *aIsTimedOut = STL_FALSE;
    }

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 래치의 Latch Mode를 Exclusive Mode로 상승시킨다.
 * @param[in] aLatch      대상 래치의 로지컬 포인터
 * @param[out] aIsSuccess 래치 모드 상승의 성공 여부
 * @param[out] aEnv       environment 정보
 */
stlStatus knlEscalateLatch( knlLatch * aLatch,
                            stlBool  * aIsSuccess,
                            knlEnv   * aEnv )
{
    STL_TRY( knaEscalateLatch( aLatch,
                               aIsSuccess,
                               aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 래치의 주어진 모드로 획득한다
 * @param[in] aLatch 획득할 래치의 로지컬 포인터
 * @param[in] aMode 획득할 래치 모드
 * @param[out] aIsSuccess 래치 획득에 성공했는지 여부
 * @param[out] aEnv environment 정보
 */
stlStatus knlTryLatch( knlLatch  * aLatch,
                       stlUInt16   aMode,
                       stlBool   * aIsSuccess,
                       knlEnv    * aEnv )
{
    stlBool sIsTimedOut;
    
    STL_TRY( knaAcquireLatchInternal( aLatch,
                                      aMode,
                                      KNL_LATCH_PRIORITY_NORMAL,
                                      0,
                                      STL_INT64_MAX,
                                      &sIsTimedOut,
                                      aIsSuccess,
                                      aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 래치의 획득을 해제한다.
 * 래치객체는 owner 개념이 없어서 자신이 획득한 래치인지 확인할
 * 방법이 없으므로 사용에 유의하여야 한다
 * @param[in] aLatch 해제할 래치의 로지컬 포인터
 * @param[out] aEnv environment 정보
 */
stlStatus knlReleaseLatch( knlLatch * aLatch,
                           knlEnv   * aEnv )
{
    return knaReleaseLatch( aLatch,
                            1,
                            aEnv );
}

/**
 * @brief 주어진 래치의 획득을 해제한다.
 * 래치객체는 owner 개념이 없어서 자신이 획득한 래치인지 확인할
 * 방법이 없으므로 사용에 유의하여야 한다
 * @param[in] aLatch 해제할 래치의 로지컬 포인터
 * @param[in] aExclWakeupCount Wakeup 시킬 대기자의 개수
 * @param[out] aEnv environment 정보
 * @remarks aWakeupCount는 Exclusive일때만 적용된다.
 */
stlStatus knlReleaseLatchWithInfo( knlLatch * aLatch,
                                   stlInt32   aExclWakeupCount,
                                   knlEnv   * aEnv )
{
    return knaReleaseLatch( aLatch,
                            aExclWakeupCount,
                            aEnv );
}

/**
 * @brief 주어진 래치의 모드를 얻는다.
 * @param[in] aLatch 대상 래치 포인터
 */
inline stlUInt16 knlCurLatchMode( knlLatch * aLatch )
{
    return aLatch->mCurMode;
}

/**
 * @brief 주어진 래치의 Exclusive lock sequence를 얻는다.
 * @param[in] aLatch 대상 래치 포인터
 */
inline stlUInt64 knlGetExclLockSeq( knlLatch * aLatch )
{
    return aLatch->mExclLockSeq;
}

/**
 * @brief 새로운 Class(용도)의 Latch들을 X$LATCHES에 보여줄 Scan 함수들을 등록한다
 * @param[in] aCallback 등록할 Scan Callback 
 * @param[out] aEnv environment 정보
 */
stlStatus knlRegisterLatchScanCallback( knlLatchScanCallback * aCallback,
                                        knlEnv               * aEnv )
{
    return knaRegisterLatchScanCallback( aCallback,
                                         aEnv );
}

/**
 * @brief aEnv를 소유한 Process나 Thread를 대기(block)시킨다.
 * @param[in] aTimeout 대기 시간 (usec)
 * @param[in] aIsTimedOut 대기시간 초과 여부
 * @param[out] aEnv environment 정보
 */
stlStatus knlSuspend( stlInterval   aTimeout,
                      stlBool     * aIsTimedOut,
                      knlEnv      * aEnv )
{
    return knaSuspend( aTimeout, aIsTimedOut, aEnv );
}

/**
 * @brief 대기하고 있는 Process나 Thread를 깨운다(wakeup).
 * @param[in] aTargetEnv 대기상태에서 깨울 Environment 정보
 * @param[out] aEnv environment 정보
 */
stlStatus knlResume( knlEnv * aTargetEnv,
                     knlEnv * aEnv )
{
    return knaResume( aTargetEnv, aEnv );
}

/**
 * @brief Lost Semaphore Signal 정보를 reset한다.
 * @param[out] aEnv environment 정보
 */
stlStatus knlResetLostSemValue( knlEnv * aEnv )
{
    return knaResetLostSemValue( aEnv );
}

/** @} */
