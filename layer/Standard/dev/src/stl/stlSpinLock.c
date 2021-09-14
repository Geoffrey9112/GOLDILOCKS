/*******************************************************************************
 * stlSpinLock.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file stlSpinLock.c
 * @brief Standard Layer spin lock
 */

#include <stlDef.h>
#include <stlSpinLock.h>
#include <stlTime.h>
#include <stlThreadProc.h>
#include <stlError.h>


/**
 * @addtogroup stlSpinLock
 * @{
 */

#define STL_TRY_SPIN_COUNT  1

stlUInt32 gStlSpinLockMaxSpinCount     = 1000;
stlUInt32 gStlSpinLockMaxBusyWaitCount = 50;

/**
 * @brief 주어진 SpinLock의 멤버변수들을 초기화 한다
 * @param[in] aSpinLock 초기화 할 SpinLock
 */
void stlInitSpinLock( stlSpinLock * aSpinLock )
{
    STL_INIT_SPIN_LOCK( *aSpinLock );
}

/**
 * @brief 주어진 SpinLock이 종료 가능한 상태인지 멤버변수들을 체크한다 
 * @param[in] aSpinLock 체크할 SpinLock
 */
void stlFinSpinLock( stlSpinLock * aSpinLock )
{
    STL_ASSERT( *aSpinLock == STL_SPINLOCK_STATUS_INIT );
}

/**
 * @brief 주어진 SpinLock을 획득한다.
 * @param[in]  aSpinLock    획득할 SpinLock
 * @param[out] aCasMissCnt  Miss count
 */
void stlAcquireSpinLock( stlSpinLock * aSpinLock,
                         stlUInt64   * aCasMissCnt )
{
    stlUInt32  sSpinCounter;
    stlUInt32  sRetVal;
    stlInt32   sBusyWaitCount = 0;

    if( aCasMissCnt != NULL )
    {
        *aCasMissCnt    = 0;
    }
    else
    {
        /* Do Nothing */
    }
    
    sSpinCounter   = gStlSpinLockMaxSpinCount;
    sBusyWaitCount = gStlSpinLockMaxBusyWaitCount;
    
    do
    {
        sRetVal = stlAtomicCas32( aSpinLock,
                                  STL_SPINLOCK_STATUS_LOCKED,
                                  STL_SPINLOCK_STATUS_INIT );
        if( sRetVal == STL_SPINLOCK_STATUS_INIT )
        {
            break;
        }

        sSpinCounter--;
        if( sSpinCounter == 0 )
        {
            if( aCasMissCnt != NULL )
            {
                *aCasMissCnt += gStlSpinLockMaxSpinCount;
            }
            else
            {
                /* Do Nothing */
            }
            
            if( sBusyWaitCount == 0 )
            {
                stlYieldThread();
                sBusyWaitCount = gStlSpinLockMaxBusyWaitCount;
            }
            else
            {
                stlBusyWait();
                sBusyWaitCount--;
            }
            sSpinCounter = gStlSpinLockMaxSpinCount;
        }
    } while( STL_TRUE );
}

/**
 * @brief 주어진 SpinLock을 획득을 try 한다.
 * @param[in] aSpinLock     획득할 SpinLock
 * @param[out] aIsSuccess   SpinLock 획득에 성공했는지 여부
 * @param[out] aCasMissCnt  Miss count
 */
void stlTrySpinLock( stlSpinLock * aSpinLock,
                     stlBool     * aIsSuccess,
                     stlUInt64   * aCasMissCnt )
{
    stlUInt32  sSpinCounter;
    stlUInt32  sRetVal;

    if( aCasMissCnt != NULL )
    {
        *aCasMissCnt = 0;
    }
    else
    {
        /* Do Nothing */
    }

    *aIsSuccess  = STL_TRUE;
    
    sSpinCounter = STL_TRY_SPIN_COUNT;
    do
    {
        sRetVal = stlAtomicCas32( aSpinLock,
                                  STL_SPINLOCK_STATUS_LOCKED,
                                  STL_SPINLOCK_STATUS_INIT );
        if( sRetVal == STL_SPINLOCK_STATUS_INIT )
        {
            break;
        }

        sSpinCounter--;
        if( sSpinCounter == 0 )
        {
            if( aCasMissCnt != NULL )
            {
                *aCasMissCnt += STL_TRY_SPIN_COUNT;
            }
            else
            {
                /* Do Nothing */
            }

            *aIsSuccess = STL_FALSE;
            
            break;
        }
    } while( STL_TRUE );
}

/**
 * @brief 주어진 SpinLock을 획득을 해제한다.
 * @param[in] aSpinLock 해제할 SpinLock
 */
void stlReleaseSpinLock( stlSpinLock * aSpinLock )
{
    stlUInt32 sRetVal;
    
    sRetVal = stlAtomicCas32( aSpinLock,
                              STL_SPINLOCK_STATUS_INIT,
                              STL_SPINLOCK_STATUS_LOCKED );
    
    STL_ASSERT( sRetVal == STL_SPINLOCK_STATUS_LOCKED );
}

/**
 * @brief 최대 Spin 회수를 반환한다.
 * @param[in] aSpinCount  spin count
 */
void stlSetMaxSpinCount( stlUInt32 aSpinCount )
{
    gStlSpinLockMaxSpinCount = aSpinCount;
}

/**
 * @brief 최대 Spin 회수를 반환한다.
 * @return spin count
 */
stlInt64 stlGetMaxSpinCount()
{
    return gStlSpinLockMaxSpinCount;
}

/**
 * @brief 최대 Spin 회수를 반환한다.
 * @param[in] aBusyWaitCount  busy wait count
 */
void stlSetMaxBusyWaitCount( stlUInt32 aBusyWaitCount )
{
    gStlSpinLockMaxBusyWaitCount = aBusyWaitCount;
}

/** @} */
