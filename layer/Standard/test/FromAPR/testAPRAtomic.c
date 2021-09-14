#include <stdio.h>
#include <stl.h>


/**
 * @brief stlAtomicSet32() 함수 테스트
 */
stlBool TestSet32()
{
    stlUInt32 sInt32;

    stlAtomicSet32(&sInt32, 2);

    STL_TRY(2 == sInt32);

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlAtomicRead32() 함수 테스트
 */
stlBool TestRead32()
{
    stlUInt32 sInt32;

    stlAtomicSet32(&sInt32, 2);

    STL_TRY(2 == stlAtomicRead32(&sInt32));

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlAtomicDec32() 함수 테스트
 */
stlBool TestDec32()
{
    stlUInt32 sInt32;
    stlUInt32 sRet;

    stlAtomicSet32(&sInt32, 2);

    sRet = stlAtomicDec32(&sInt32);
    STL_TRY(1 == sInt32);
    STL_TRY(sRet != 0);
    
    sRet = stlAtomicDec32(&sInt32);
    STL_TRY(0 == sInt32);
    STL_TRY(sRet == 0);
    
    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlAtomicXchg32() 함수 테스트
 */
stlBool TestXchg32()
{
    stlUInt32 sInt32;
    stlUInt32 sOldVal;

    stlAtomicSet32(&sInt32, 100);
    sOldVal = stlAtomicXchg32(&sInt32, 50);

    STL_TRY(100 == sOldVal);
    STL_TRY(50 == sInt32);
    
    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlAtomicCas32() 함수 테스트
 */
stlBool TestCasEqual()
{
    stlUInt32 sCasVal = 0;
    stlUInt32 sOldVal;

    sOldVal = stlAtomicCas32(&sCasVal, 12, 0);
    STL_TRY(0 == sOldVal);
    STL_TRY(12 == sCasVal);
    
    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlAtomicAdd32() 함수 테스트
 */
stlBool TestAdd32()
{
    stlUInt32 sInt32;
    stlUInt32 sOldVal;

    stlAtomicSet32(&sInt32, 23);
    sOldVal = stlAtomicAdd32(&sInt32, 4);
    STL_TRY(23 == sOldVal);
    STL_TRY(27 == sInt32);
    
    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlAtomicInc32() 함수 테스트
 */
stlBool TestInc32()
{
    stlUInt32 sInt32;
    stlUInt32 sOldVal;

    stlAtomicSet32(&sInt32, 23);
    sOldVal = stlAtomicInc32(&sInt32);
    STL_TRY(23 == sOldVal);
    STL_TRY(24 == sInt32);
    
    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlAtomicSub32() 함수 테스트
 */
stlBool TestSetAddIncSub()
{
    stlUInt32 sInt32;

    stlAtomicSet32(&sInt32, 0);
    stlAtomicAdd32(&sInt32, 20);
    stlAtomicInc32(&sInt32);
    stlAtomicSub32(&sInt32, 10);

    STL_TRY(11 == sInt32);
    
    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlAtomicDec32() 함수 테스트
 */
stlBool TestWrapZero()
{
    stlUInt32 sInt32;
    stlUInt32 sRet;
    stlUInt32 sMinus1 = -1;

    stlAtomicSet32(&sInt32, 0);
    sRet = stlAtomicDec32(&sInt32);

    STL_TRY(0 != sRet);
    STL_TRY(sMinus1 == sInt32);
    
    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlAtomicInc32() 함수 테스트
 */
stlBool TestIncNeg1()
{
    stlUInt32 sInt32 = -1;
    stlUInt32 sRet;
    stlUInt32 sMinus1 = -1;

    sRet = stlAtomicInc32(&sInt32);

    STL_TRY(sMinus1 == sRet);
    STL_TRY(0 == sInt32);
    
    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

void * STL_THREAD_FUNC MutexThreadFunc( stlThread *aThread, void *aData );
void * STL_THREAD_FUNC AtomicThreadFunc( stlThread *aThread, void *aData );

stlSemaphore       gSemaphore;
stlChar          * gSemaName   = "TESTSEM";
volatile stlUInt32 gLockItem   = 0;
volatile stlUInt32 gAtomicOp   = 0;
stlStatus          gExitRetVal = 123; /* just some made up number to check on later */

#define NUM_THREADS    40
#define NUM_ITERATIONS 20000

#define PRINTERR                                            \
    do                                                      \
    {                                                       \
        stlPrintf( "ERRCODE: %d, ERRSTRING: %s\n",          \
                   stlGetLastErrorCode( &sErrStack ),       \
                   stlGetLastErrorMessage( &sErrStack ) );  \
    } while(0)

/*
 * gLockItem++
 */ 
void * STL_THREAD_FUNC MutexThreadFunc(stlThread *aThread, void *aData)
{
    stlInt32 sIdx;
    stlErrorStack sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );

    for (sIdx = 0; sIdx < NUM_ITERATIONS; sIdx++)
    {
        STL_TRY( stlAcquireSemaphore( &gSemaphore, &sErrStack ) == STL_SUCCESS );
        gLockItem++;
        STL_TRY( stlReleaseSemaphore( &gSemaphore, &sErrStack ) == STL_SUCCESS );
    }

    STL_TRY( stlExitThread( aThread, gExitRetVal, &sErrStack ) == STL_SUCCESS );

    return NULL;

    STL_FINISH;

    PRINTERR;
    
    return NULL;
}

/*
 * gAtomicOp++
 */ 
void * STL_THREAD_FUNC AtomicThreadFunc(stlThread *aThread, void *aData)
{
    stlInt32 sIdx;
    stlErrorStack sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );

    for (sIdx = 0; sIdx < NUM_ITERATIONS ; sIdx++)
    {
        stlAtomicInc32(&gAtomicOp);
        stlAtomicAdd32(&gAtomicOp, 2);
        stlAtomicDec32(&gAtomicOp);
        stlAtomicDec32(&gAtomicOp);
    }

    STL_TRY( stlExitThread( aThread, gExitRetVal, &sErrStack ) == STL_SUCCESS );

    return NULL;
    
    STL_FINISH;

    PRINTERR;
    
    return NULL;
}

/**
 * @brief 다수의 쓰레드간 Atomic Action의 보장 여부
 */
stlBool TestAtomicsThreaded()
{
    stlThread  sThread1[NUM_THREADS];
    stlThread  sThread2[NUM_THREADS];
    stlInt32   sIdx;
    stlStatus  sStatus1;
    stlStatus  sStatus2;

    stlErrorStack sErrStack;

    gLockItem = 0;
    gAtomicOp = 0;
    
    STL_INIT_ERROR_STACK( &sErrStack );
    
    STL_TRY( stlCreateSemaphore( &gSemaphore,
                                 gSemaName,
                                 1,
                                 &sErrStack )
             == STL_SUCCESS);

    /*
     * 다수의 쓰레드 생성
     */
    for (sIdx = 0; sIdx < NUM_THREADS; sIdx++)
    {
        STL_TRY( stlCreateThread( &sThread1[sIdx],
                                  NULL,
                                  MutexThreadFunc,
                                  NULL,
                                  &sErrStack )
                 == STL_SUCCESS);
        
        STL_TRY( stlCreateThread( &sThread2[sIdx],
                                  NULL,
                                  AtomicThreadFunc,
                                  NULL,
                                  &sErrStack )
                 == STL_SUCCESS);
    }

    /*
     * 다수의 쓰레드 종료 대기
     */
    for (sIdx = 0; sIdx < NUM_THREADS; sIdx++)
    {
        STL_TRY( stlJoinThread( &sThread1[sIdx],
                                &sStatus1,
                                &sErrStack )
                 == STL_SUCCESS);
        
        STL_TRY( stlJoinThread( &sThread2[sIdx],
                                &sStatus2,
                                &sErrStack )
                 == STL_SUCCESS);
        
        STL_TRY( sStatus1 == gExitRetVal );
        STL_TRY( sStatus2 == gExitRetVal );
    }

    /*
     * Atomic Action 검증
     */
    STL_TRY( (NUM_THREADS * NUM_ITERATIONS) == gLockItem );
    STL_TRY( (NUM_THREADS * NUM_ITERATIONS) == stlAtomicRead32(&gAtomicOp) );
    
    STL_TRY( stlDestroySemaphore( &gSemaphore, &sErrStack ) == STL_SUCCESS);

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

#undef  NUM_THREADS
#define NUM_THREADS 7

typedef struct testBox testBox;

struct testBox
{
    stlUInt32  *mMem;
    stlUInt32   mPreVal;
    stlUInt32   mPostVal;
    stlUInt32   mLoop;
    stlBool   (*mFunc)(testBox *aBox);
};

/*
 * Busy Loop Atomic Read
 */
stlBool BusyLoopRead32(testBox *aBox)
{
    stlUInt32 sVal;

    do
    {
        sVal = stlAtomicRead32(aBox->mMem);

        if (sVal != aBox->mPreVal)
        {
            stlYieldThread();
        }
        else
        {
            break;
        }
    } while (1);

    return STL_TRUE;
}

/*
 * Busy Loop Atomic Set
 */
stlBool BusyLoopSet32(testBox *aBox)
{
    do
    {
        STL_TRY( BusyLoopRead32(aBox) == STL_TRUE );
        stlAtomicSet32(aBox->mMem, aBox->mPostVal);
        aBox->mLoop--;
    } while (aBox->mLoop);

    return STL_TRUE;

    STL_FINISH;

    // PRINTERR;

    return STL_FALSE;
}

/*
 * Busy Loop Atomic Add
 */
stlBool BusyLoopAdd32(testBox *aBox)
{
    stlUInt32 sVal;

    stlErrorStack sErrStack;

    STL_INIT_ERROR_STACK( &sErrStack );
    
    do
    {
        STL_TRY( BusyLoopRead32(aBox) == STL_TRUE );
        sVal = stlAtomicAdd32(aBox->mMem, aBox->mPostVal);

        STL_TRY( stlAcquireSemaphore( &gSemaphore, &sErrStack ) == STL_SUCCESS );
        STL_TRY( sVal == aBox->mPreVal );
        STL_TRY( stlReleaseSemaphore( &gSemaphore, &sErrStack ) == STL_SUCCESS );
    } while (--aBox->mLoop);

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

/*
 * Busy Loop Atomic Subtraction
 */
stlBool BusyLoopSub32(testBox *aBox)
{
    do
    {
        STL_TRY( BusyLoopRead32(aBox) == STL_TRUE );
        stlAtomicSub32(aBox->mMem, aBox->mPostVal);
        aBox->mLoop--;
    } while (aBox->mLoop);

    return STL_TRUE;

    STL_FINISH;

    // PRINTERR;

    return STL_FALSE;
}

/*
 * Busy Loop Atomic Increasement
 */
stlBool BusyLoopInc32(testBox *aBox)
{
    stlUInt32 sVal;

    stlErrorStack sErrStack;

    STL_INIT_ERROR_STACK( &sErrStack );

    do
    {
        STL_TRY( BusyLoopRead32(aBox) == STL_TRUE );
        sVal = stlAtomicInc32(aBox->mMem);

        STL_TRY( stlAcquireSemaphore( &gSemaphore, &sErrStack ) == STL_SUCCESS );
        STL_TRY( sVal == aBox->mPreVal );
        STL_TRY( stlReleaseSemaphore( &gSemaphore, &sErrStack ) == STL_SUCCESS );
        aBox->mLoop--;
    } while (aBox->mLoop);

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

/*
 * Busy Loop Atomic Decreasement
 */
stlBool BusyLoopDec32(testBox *aBox)
{
    stlUInt32 sVal;

    stlErrorStack sErrStack;

    STL_INIT_ERROR_STACK( &sErrStack );
    
    do
    {
        STL_TRY( BusyLoopRead32(aBox) == STL_TRUE );
        sVal = stlAtomicDec32(aBox->mMem);

        STL_TRY( stlAcquireSemaphore( &gSemaphore, &sErrStack ) == STL_SUCCESS );
        STL_TRY( sVal != 0);
        STL_TRY( stlReleaseSemaphore( &gSemaphore, &sErrStack ) == STL_SUCCESS );
        aBox->mLoop--;
    } while (aBox->mLoop);

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;

    return STL_FALSE;
}

/*
 * Busy Loop Atomic Compare And Swap
 */
stlBool BusyLoopCas32(testBox *aBox)
{
    stlUInt32 sVal;

    do
    {
        do
        {
            sVal = stlAtomicCas32( aBox->mMem,
                                   aBox->mPostVal,
                                   aBox->mPreVal );

            if (sVal != aBox->mPreVal)
            {
                stlYieldThread();
            }
            else
            {
                break;
            }
        } while (1);
        aBox->mLoop--;
    } while (aBox->mLoop);

    return STL_TRUE;
}

/*
 * Busy Loop Atomic Exchange
 */
stlBool BusyLoopXchg32(testBox *aBox)
{
    stlUInt32 sVal;

    stlErrorStack sErrStack;

    STL_INIT_ERROR_STACK( &sErrStack );
    
    do
    {
        STL_TRY( BusyLoopRead32(aBox) == STL_TRUE );
        sVal = stlAtomicXchg32(aBox->mMem, aBox->mPostVal);

        STL_TRY( stlAcquireSemaphore( &gSemaphore, &sErrStack ) == STL_SUCCESS );
        STL_TRY( sVal == aBox->mPreVal);
        STL_TRY( stlReleaseSemaphore( &gSemaphore, &sErrStack ) == STL_SUCCESS );
        aBox->mLoop--;
    } while (aBox->mLoop);

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

/*
 * Busy Loop Thread
 */
void * STL_THREAD_FUNC BusyLoopThreadFunc(stlThread *aThread, void *aData)
{
    stlBool   sRet;
    stlStatus sStatus;
    testBox  *sBox = aData;

    stlErrorStack sErrStack;

    STL_INIT_ERROR_STACK( &sErrStack );
    
    sRet = sBox->mFunc(sBox);

    if (sRet == STL_TRUE)
    {
        sStatus = STL_SUCCESS;
    }
    else
    {
        sStatus = -1;
    }

    STL_TRY( stlExitThread( aThread, sStatus, &sErrStack ) == STL_SUCCESS );

    return NULL;

    STL_FINISH;

    PRINTERR;

    return NULL;
}

/*
 * Atomic Busy Loop Thread
 */
stlBool TestAtomicsBusyLoopThreaded()
{
    stlUInt32   sIdx;
    stlUInt32   sCount = 0;
    testBox     sBox[NUM_THREADS];
    stlThread   sThread[NUM_THREADS];
    stlStatus   sRetVal;

    stlErrorStack sErrStack;

    STL_INIT_ERROR_STACK( &sErrStack );
    
    STL_TRY( stlCreateSemaphore( &gSemaphore,
                                 gSemaName,
                                 1,
                                 &sErrStack )
             == STL_SUCCESS);

    /* get ready */
    for (sIdx = 0; sIdx < NUM_THREADS; sIdx++)
    {
        sBox[sIdx].mMem = &sCount;
        sBox[sIdx].mLoop = 50;
    }

    sBox[0].mPreVal = 98;
    sBox[0].mPostVal = 3891;
    sBox[0].mFunc = BusyLoopAdd32;

    sBox[1].mPreVal = 3989;
    sBox[1].mPostVal = 1010;
    sBox[1].mFunc = BusyLoopSub32;

    sBox[2].mPreVal = 2979;
    sBox[2].mPostVal = 0; /* not used */
    sBox[2].mFunc = BusyLoopInc32;

    sBox[3].mPreVal = 2980;
    sBox[3].mPostVal = 16384;
    sBox[3].mFunc = BusyLoopSet32;

    sBox[4].mPreVal = 16384;
    sBox[4].mPostVal = 0; /* not used */
    sBox[4].mFunc = BusyLoopDec32;

    sBox[5].mPreVal = 16383;
    sBox[5].mPostVal = 1048576;
    sBox[5].mFunc = BusyLoopCas32;

    sBox[6].mPreVal = 1048576;
    sBox[6].mPostVal = 98; /* goto sBox[0] */
    sBox[6].mFunc = BusyLoopXchg32;

    /* get set */
    for (sIdx = 0; sIdx < NUM_THREADS; sIdx++)
    {
        STL_TRY( stlCreateThread( &sThread[sIdx],
                                  NULL,
                                  BusyLoopThreadFunc,
                                  &sBox[sIdx],
                                  &sErrStack )
                 == STL_SUCCESS);
    }

    /* go! */
    stlAtomicSet32(sBox->mMem, 98);

    for (sIdx = 0; sIdx < NUM_THREADS; sIdx++)
    {
        STL_TRY( stlJoinThread( &sThread[sIdx],
                                &sRetVal,
                                &sErrStack )
                 == STL_SUCCESS);

        STL_TRY(sRetVal == STL_SUCCESS);
    }

    STL_TRY(98 == sCount);

    STL_TRY( stlDestroySemaphore( &gSemaphore, &sErrStack ) == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

#if (STL_SIZEOF_VOIDP == 8)

/**
 * @brief stlAtomicSet64() 함수 테스트
 */
stlBool TestSet64()
{
    stlUInt64 sInt64;

    stlAtomicSet64( &sInt64, STL_UINT64_C(STL_UINT32_MAX + 1) );

    STL_TRY( STL_UINT64_C(STL_UINT32_MAX + 1) == sInt64 );

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlAtomicRead64() 함수 테스트
 */
stlBool TestRead64()
{
    stlUInt64 sInt64;

    stlAtomicSet64( &sInt64, STL_UINT64_C(STL_UINT32_MAX + 1) );

    STL_TRY( STL_UINT64_C(STL_UINT32_MAX + 1) == stlAtomicRead64(&sInt64) );

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlAtomicDec64() 함수 테스트
 */
stlBool TestDec64()
{
    stlUInt64 sInt64;
    stlUInt64 sRet;

    stlAtomicSet64( &sInt64, STL_UINT64_C(STL_UINT32_MAX + 2) );

    sRet = stlAtomicDec64( &sInt64 );
    STL_TRY( STL_UINT64_C(STL_UINT32_MAX + 1) == sInt64 );
    STL_TRY( sRet != 0 );
    
    sRet = stlAtomicDec64(&sInt64);
    STL_TRY( STL_UINT32_MAX == sInt64 );
    STL_TRY( sRet != 0 );
    
    stlAtomicSet64(&sInt64, 1);

    /*
     * 명확히 0 이 리턴되어야 함
     */
    sRet = stlAtomicDec64(&sInt64);
    STL_TRY(0 == sInt64);
    STL_TRY(sRet == 0);
    
    return STL_TRUE;

    STL_FINISH;

    // PRINTERR;

    return STL_FALSE;
}

/**
 * @brief stlAtomicXchg64() 함수 테스트
 */
stlBool TestXchg64()
{
    stlUInt64 sInt64;
    stlUInt64 sOldVal;

    stlAtomicSet64( &sInt64, STL_UINT32_MAX );
    sOldVal = stlAtomicXchg64( &sInt64, STL_UINT64_C(STL_UINT32_MAX * 2) );

    STL_TRY( STL_UINT32_MAX == sOldVal );
    STL_TRY( STL_UINT64_C(STL_UINT32_MAX * 2) == sInt64 );
    
    return STL_TRUE;

    STL_FINISH;

    // PRINTERR;
    
    return STL_FALSE;
}

/**
 * @brief stlAtomicCas64() 함수 테스트
 */
stlBool TestCasEqual64()
{
    stlUInt64 sCasVal = STL_UINT64_C(STL_UINT32_MAX + 1);
    stlUInt64 sOldVal;

    sOldVal = stlAtomicCas64( &sCasVal,
                              STL_UINT64_C(STL_UINT32_MAX * 2),
                              STL_UINT64_C(STL_UINT32_MAX + 1) );
    STL_TRY( STL_UINT64_C(STL_UINT32_MAX + 1) == sOldVal );
    STL_TRY( STL_UINT64_C(STL_UINT32_MAX * 2) == sCasVal );
    
    return STL_TRUE;

    STL_FINISH;

    // PRINTERR;

    return STL_FALSE;
}

/**
 * @brief stlAtomicAdd64() 함수 테스트
 */
stlBool TestAdd64()
{
    stlUInt64 sInt64;
    stlUInt64 sOldVal;

    stlAtomicSet64( &sInt64, STL_UINT64_C(STL_UINT32_MAX + 1) );
    sOldVal = stlAtomicAdd64( &sInt64, 4);
    STL_TRY( STL_UINT64_C(STL_UINT32_MAX + 1) == sOldVal );
    STL_TRY( STL_UINT64_C(STL_UINT32_MAX + 5) == sInt64 );
    
    stlAtomicSet64( &sInt64, STL_UINT64_C(STL_UINT32_MAX + 1) );
    sOldVal = stlAtomicAdd64( &sInt64, STL_UINT64_C(STL_UINT32_MAX + 1) );
    STL_TRY( STL_UINT64_C(STL_UINT32_MAX + 1) == sOldVal );
    STL_TRY( (STL_UINT64_C(STL_UINT32_MAX * 2) + 2) == sInt64 );
    
    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlAtomicAdd64() 함수 테스트
 */
stlBool TestInc64()
{
    stlUInt64 sInt64;
    stlUInt64 sOldVal;

    stlAtomicSet64( &sInt64, STL_UINT64_C(STL_UINT32_MAX + 1) );
    sOldVal = stlAtomicInc64(&sInt64);
    STL_TRY( STL_UINT64_C(STL_UINT32_MAX + 1) == sOldVal );
    STL_TRY( STL_UINT64_C(STL_UINT32_MAX + 2) == sInt64 );
    
    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlAtomicSub64() 함수 테스트
 */
stlBool TestSetAddIncSub64()
{
    stlUInt64 sInt64;

    stlAtomicSet64( &sInt64, STL_UINT32_MAX );
    stlAtomicAdd64( &sInt64, STL_UINT64_C(STL_UINT32_MAX + 1) );
    stlAtomicInc64( &sInt64 );
    stlAtomicSub64( &sInt64, STL_UINT32_MAX );

    STL_TRY( STL_UINT64_C(STL_UINT32_MAX + 2) == sInt64 );
    
    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlAtomicSub64() 함수 테스트
 */
stlBool TestWrapZero64()
{
    stlUInt64 sInt64;
    stlUInt64 sRet;
    stlUInt64 sMinus1 = -1;

    stlAtomicSet64( &sInt64, 0 );
    sRet = stlAtomicDec64(&sInt64);

    STL_TRY( 0 != sRet );
    STL_TRY( sMinus1 == sInt64 );
    
    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlAtomicInc64() 함수 테스트
 */
stlBool TestInc64Neg1()
{
    stlUInt64 sInt64 = -1;
    stlUInt64 sRet;
    stlUInt64 sMinus1 = -1;

    sRet = stlAtomicInc64( &sInt64 );

    STL_TRY( sMinus1 == sRet );
    STL_TRY( 0 == sInt64 );
    
    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

#endif


stlInt32 main(stlInt32 aArgc, stlChar** aArgv)
{
    stlInt32 sState = 0;

    STL_TRY( stlInitialize() == STL_SUCCESS );
    
    /*
     * Test Set32
     */
    sState = 1;
    STL_TRY( TestSet32() == STL_TRUE );

    /*
     * Test Read32
     */
    sState = 2;
    STL_TRY(TestRead32() == STL_TRUE);

    /*
     * Test Dec32
     */
    sState = 3;
    STL_TRY(TestDec32() == STL_TRUE);

    /*
     * Test Xchg32
     */
    sState = 4;
    STL_TRY(TestXchg32() == STL_TRUE);

    /*
     * Test CasEqual
     */
    sState = 6;
    STL_TRY(TestCasEqual() == STL_TRUE);

    /*
     * Test Add32
     */
    sState = 10;
    STL_TRY(TestAdd32() == STL_TRUE);

    /*
     * Test Inc32
     */
    sState = 11;
    STL_TRY(TestInc32() == STL_TRUE);

    /*
     * Test SetAddIncSub
     */
    sState = 12;
    STL_TRY(TestSetAddIncSub() == STL_TRUE);

    /*
     * Test WrapZero
     */
    sState = 13;
    STL_TRY(TestWrapZero() == STL_TRUE);

    /*
     * Test IncNeg1
     */
    sState = 14;
    STL_TRY(TestIncNeg1() == STL_TRUE);

    /*
     * Test AtomicsThreaded
     */
    sState = 15;
    STL_TRY(TestAtomicsThreaded() == STL_TRUE);

    /*
     * Test AtomicsBusyLoopThreaded
     */
    sState = 16;
    STL_TRY(TestAtomicsBusyLoopThreaded() == STL_TRUE);

#if (STL_SIZEOF_VOIDP == 8)

    /*
     * Test Set64
     */
    sState = 17;
    STL_TRY(TestSet64() == STL_TRUE);

    /*
     * Test Read64
     */
    sState = 18;
    STL_TRY(TestRead64() == STL_TRUE);

    /*
     * Test Dec64
     */
    sState = 19;
    STL_TRY(TestDec64() == STL_TRUE);

    /*
     * Test Xchg64
     */
    sState = 20;
    STL_TRY(TestXchg64() == STL_TRUE);

    /*
     * Test CasEqual64
     */
    sState = 21;
    STL_TRY(TestCasEqual64() == STL_TRUE);

    /*
     * Test Add64
     */
    sState = 23;
    STL_TRY(TestAdd64() == STL_TRUE);

    /*
     * Test Inc64
     */
    sState = 24;
    STL_TRY(TestInc64() == STL_TRUE);

    /*
     * Test SetAddIncSub64
     */
    sState = 25;
    STL_TRY(TestSetAddIncSub64() == STL_TRUE);

    /*
     * Test WrapZero64
     */
    sState = 26;
    STL_TRY(TestWrapZero64() == STL_TRUE);

    /*
     * Test Inc64Neg1
     */
    sState = 27;
    STL_TRY(TestInc64Neg1() == STL_TRUE);

#endif

    stlPrintf("PASS");

    return (0);

    STL_FINISH;

    stlPrintf("FAILURE %d", sState);
    
    return (-1);
}
