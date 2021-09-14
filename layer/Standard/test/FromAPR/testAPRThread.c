#include <stdio.h>
#include <stl.h>

stlSemaphore    gSemaphore;
stlThreadOnce   gThreadOnce;
stlInt32        gCount = 0;
stlInt32        gValue = 0;

stlThread gThread1;
stlThread gThread2;
stlThread gThread3;
stlThread gThread4;

/* just some made up number to check on later */
stlStatus gExitRetVal = 123;

#define PRINTERR                                            \
    do                                                      \
    {                                                       \
        stlPrintf( "ERRCODE: %d, ERRSTRING: %s\n",          \
                   stlGetLastErrorCode( &sErrStack ),       \
                   stlGetLastErrorMessage( &sErrStack ) );  \
    } while(0)

/*
 * Thread Init 함수
 */ 
void InitFunc()
{
    gValue++;
}

/**
 * @brief stlSetThreadOnce() 함수 테스트
 */
void * STL_THREAD_FUNC ThreadFunc1(stlThread *aThread, void *aData)
{
    stlInt32 sIdx;

    stlErrorStack sErrStack;

    STL_TRY(stlInitialize() == STL_SUCCESS);
    
    STL_INIT_ERROR_STACK( &sErrStack );
    
    STL_TRY( stlSetThreadOnce( &gThreadOnce,
                               &InitFunc,
                               &sErrStack )
             == STL_SUCCESS );

    for (sIdx = 0; sIdx < 10000; sIdx++)
    {
        STL_TRY( stlAcquireSemaphore( &gSemaphore, &sErrStack ) == STL_SUCCESS );
        gCount++;
        STL_TRY( stlReleaseSemaphore( &gSemaphore, &sErrStack ) == STL_SUCCESS );
    }

    STL_TRY( stlExitThread( aThread,
                            gExitRetVal,
                            &sErrStack )
             == STL_SUCCESS );

    return NULL;

    STL_FINISH;

    PRINTERR;
    
    return NULL;
} 

/**
 * @brief stlInitThreadOnce() 함수 테스트
 */
stlBool TestThreadInit()
{
    stlErrorStack sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );

    STL_TRY( stlInitThreadOnce( &gThreadOnce,
                                &sErrStack )
             == STL_SUCCESS);

    STL_TRY( stlCreateSemaphore( &gSemaphore,
                                 "SEMATET",
                                 1,
                                 &sErrStack )
             == STL_SUCCESS);

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

/**
 * @brief stlCreateThread() 함수 테스트
 */
stlBool TestCreateThreads()
{
    stlErrorStack sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );
    
    STL_TRY( stlCreateThread( &gThread1,
                              NULL,
                              ThreadFunc1,
                              NULL,
                              &sErrStack )
             == STL_SUCCESS );
    
    STL_TRY( stlCreateThread( &gThread2,
                              NULL,
                              ThreadFunc1,
                              NULL,
                              &sErrStack )
             == STL_SUCCESS );
    
    STL_TRY( stlCreateThread( &gThread3,
                              NULL,
                              ThreadFunc1,
                              NULL,
                              &sErrStack )
             == STL_SUCCESS );
    
    STL_TRY( stlCreateThread( &gThread4,
                              NULL,
                              ThreadFunc1,
                              NULL,
                              &sErrStack )
             == STL_SUCCESS );
    
    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

/**
 * @brief stlJoinThread() 함수 테스트
 */
stlBool TestJoinThreads()
{
    stlStatus     sRetStatus;
    stlErrorStack sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );

    STL_TRY( stlJoinThread( &gThread1,
                            &sRetStatus,
                            &sErrStack )
             == STL_SUCCESS );
    STL_TRY(sRetStatus == gExitRetVal);
    
    STL_TRY( stlJoinThread( &gThread2,
                            &sRetStatus,
                            &sErrStack )
             == STL_SUCCESS );
    STL_TRY(sRetStatus == gExitRetVal);
    
    STL_TRY( stlJoinThread( &gThread3,
                            &sRetStatus,
                            &sErrStack )
             == STL_SUCCESS );
    STL_TRY(sRetStatus == gExitRetVal);
    
    STL_TRY( stlJoinThread( &gThread4,
                            &sRetStatus,
                            &sErrStack )
             == STL_SUCCESS );
    STL_TRY(sRetStatus == gExitRetVal);
    
    return STL_TRUE;

    STL_FINISH;

    PRINTERR;

    return STL_FALSE;
}

/*
 * Lock 테스트 검증
 */
stlBool TestCheckLocks()
{
    STL_TRY(40000 == gCount);

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/*
 * Thread Once 값 검증
 */
stlBool TestCheckThreadOnce()
{
    STL_TRY(1 == gValue);

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

stlInt32 main(stlInt32 aArgc, stlChar** aArgv)
{
    stlInt32 sState = 0;

    STL_TRY(stlInitialize() == STL_SUCCESS);

    /*
     * Test ThreadInit
     */
    sState = 1;
    STL_TRY(TestThreadInit() == STL_TRUE);

    /*
     * Test TestCreateThreads
     */
    sState = 2;
    STL_TRY(TestCreateThreads() == STL_TRUE);

    /*
     * Test JoinThreads
     */
    sState = 3;
    STL_TRY(TestJoinThreads() == STL_TRUE);

    /*
     * Test CheckLocks
     */
    sState = 4;
    STL_TRY(TestCheckLocks() == STL_TRUE);

    /*
     * Test CheckThreadOnce
     */
    sState = 5;
    STL_TRY(TestCheckThreadOnce() == STL_TRUE);

    stlPrintf("PASS");

    return (0);

    STL_FINISH;

    stlPrintf("FAILURE %d", sState);

    return (-1);
}
