#include <stl.h>

#define STR_SIZE 45

/* The time value is used throughout the tests, so just make this a global.
 * Also, we need a single value that we can test for the positive tests, so
 * I chose the number below, it corresponds to:
 *           2002-08-14 12:05:36.186711 -25200 [257 Sat].
 * Which happens to be when I wrote the new tests.
 */
stlTime  gNow = STL_UINT64_C(1032030336186711);

stlBool TestNow()
{
    stlInt64 sTimedDiff;
    stlTime sCurrent;
    time_t  sOsNow;

    sCurrent = stlNow();
    time(&sOsNow);

    sTimedDiff = sOsNow - (sCurrent / STL_USEC_PER_SEC);

    /* Even though these are called so close together, there is the chance
     * that the time will be slightly off, so accept anything between -1 and
     * 1 second.
     */
    STL_TRY((sTimedDiff >= -2) && (sTimedDiff <= 2));

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

int main(stlInt32 aArgc, stlChar** aArgv)
{
    stlInt32 sState = 0;
    
    STL_TRY(stlInitialize() == STL_SUCCESS);

    /*
     * Test Now
     */
    sState = 1;
    STL_TRY(TestNow() == STL_TRUE);

    stlPrintf("PASS");

    return 0;
    
    STL_FINISH;

    stlPrintf("FAILURE %d", sState);

    return -1;
}
