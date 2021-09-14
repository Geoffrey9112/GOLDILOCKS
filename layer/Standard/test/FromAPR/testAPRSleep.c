#include <stdio.h>
#include <time.h>
#include <stl.h>

#define SLEEP_INTERVAL (2)

#define PRINTERR                                            \
    do                                                      \
    {                                                       \
        stlPrintf( "ERRCODE: %d, ERRSTRING: %s\n",          \
                   stlGetLastErrorCode( &sErrStack ),       \
                   stlGetLastErrorMessage( &sErrStack ) );  \
    } while(0)

/**
 * @brief stlSleep() 테스트
 */
stlBool TestSleepOne()
{
    time_t sPreTime = time(NULL);
    time_t sPostTime;
    time_t sTimeDiff;

    /* stlSleep(usec) */
    stlSleep(SLEEP_INTERVAL * 1000000);
    sPostTime = time(NULL);

    /* normalize the timediff.  We should have slept for SLEEP_INTERVAL, so
     * we should just subtract that out.
     */
    sTimeDiff = sPostTime - sPreTime - SLEEP_INTERVAL;

    STL_TRY(sTimeDiff >= 0);
    STL_TRY(sTimeDiff <= 1);

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

stlInt32 main(stlInt32 aArgc, stlChar** aArgv)
{
    stlInt32 sState = 0;

    STL_TRY(stlInitialize() == STL_SUCCESS);

    /*
     * Test SleepOne
     */
    sState = 1;
    STL_TRY(TestSleepOne() == STL_TRUE);

    stlPrintf("PASS");

    return (0);

    STL_FINISH;

    stlPrintf("FAILURE %d", sState);

    return (-1);
}
