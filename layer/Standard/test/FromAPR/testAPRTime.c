#include <stdio.h>
#include <time.h>
#include <stl.h>

#define STR_SIZE 45

/* The time value is used throughout the tests, so just make this a global.
 * Also, we need a single value that we can test for the positive tests, so
 * I chose the number below, it corresponds to:
 *           2002-08-14 12:05:36.186711 -25200 [257 Sat].
 * Which happens to be when I wrote the new tests.
 */
stlTime  gNow = STL_INT64_C(1032030336186711);

/**
 * @brief stlNow() 함수를 테스트한다.
 */

stlBool TestNow()
{
    stlTime sTimedDiff;
    stlTime sCurrent;
    time_t  sOsNow;

    sCurrent = stlNow();
    time(&sOsNow);

    sTimedDiff = sOsNow - (sCurrent / STL_USEC_PER_SEC);

    /*
     * 거의 동시에 수행되고 있으나, 약간의 차이는 수용한다. 
     * Even though these are called so close together, there is the chance
     * that the time will be slightly off, so accept anything between -1 and
     * 1 second.
     */
    // STL_TRY((-2 <= sTimedDiff) && (sTimedDiff <= 2));
    STL_TRY( sTimedDiff <= 2 );

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlPutAnsiTime() 함수를 테스트한다.
 */

stlBool TestPutAnsiTime()
{
    stlTime sNowSec;

    time_t  sOsNow;

    time(&sOsNow);

    STL_TRY( stlPutAnsiTime( & sNowSec, sOsNow ) == STL_SUCCESS );

    /*
     * time_t 를 miscrosecond 단위로 변환했는지 확인
     */
    STL_TRY( (sNowSec / 1000000) == sOsNow );

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}


/**
 * @brief stlMakeExpTimeByGMT() 함수를 테스트한다.
 */

stlBool TestMakeExpTimeByGMT()
{
    stlExpTime sExpTime;

    time_t      sPosixSec;
    struct tm * sPosixExp;
    
    stlMakeExpTimeByGMT( &sExpTime, gNow );

    /*
     * gmtime() 함수의 결과와 비교한다.
     */
 
    sPosixSec = (time_t) (gNow / 1000000);
    sPosixExp = gmtime( & sPosixSec );

    STL_TRY(sPosixExp->tm_sec   == sExpTime.mSecond);
    STL_TRY(sPosixExp->tm_min   == sExpTime.mMinute);
    STL_TRY(sPosixExp->tm_hour  == sExpTime.mHour);
    STL_TRY(sPosixExp->tm_mday  == sExpTime.mDay);
    STL_TRY(sPosixExp->tm_mon   == sExpTime.mMonth);
    STL_TRY(sPosixExp->tm_year  == sExpTime.mYear);
    STL_TRY(sPosixExp->tm_wday  == sExpTime.mWeekDay);
    STL_TRY(sPosixExp->tm_yday  == sExpTime.mYearDay);
    STL_TRY(sPosixExp->tm_isdst == sExpTime.mIsDaylightSaving);
    
    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}


/**
 * @brief stlMakeExpTimeByLocalTz() 함수를 테스트한다.
 */

stlBool TestMakeExpTimeByLocalTz()
{
    stlExpTime sExpTime;

    time_t      sPosixSec;
    struct tm * sPosixExp;
    
    stlMakeExpTimeByLocalTz( &sExpTime, gNow );

    /*
     * localtime() 함수의 결과와 비교한다.
     */
 
    sPosixSec = (time_t) (gNow / 1000000);
    sPosixExp = localtime( & sPosixSec );

    STL_TRY(sPosixExp->tm_sec   == sExpTime.mSecond);
    STL_TRY(sPosixExp->tm_min   == sExpTime.mMinute);
    STL_TRY(sPosixExp->tm_hour  == sExpTime.mHour);
    STL_TRY(sPosixExp->tm_mday  == sExpTime.mDay);
    STL_TRY(sPosixExp->tm_mon   == sExpTime.mMonth);
    STL_TRY(sPosixExp->tm_year  == sExpTime.mYear);
    STL_TRY(sPosixExp->tm_wday  == sExpTime.mWeekDay);
    STL_TRY(sPosixExp->tm_yday  == sExpTime.mYearDay);
    STL_TRY(sPosixExp->tm_isdst == sExpTime.mIsDaylightSaving);
    
    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlSleep() 함수를 테스트한다.
 */

stlBool TestSleep()
{
    stlInterval  sInterval;
    time_t       sBeginSec;
    time_t       sEndSec;

    sInterval = 100;

    time( & sBeginSec );
    stlSleep( sInterval );
    time( & sEndSec );

    /*
     * stlSleep() 이 microsecond 단위의 sleep 을 제공하는지 확인한다.
     * 시간차가 2초를 넘어가지 않음을 확인함.
     */
    STL_TRY( (sEndSec - sBeginSec) <= 2 );
    
    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

stlInt32 main(stlInt32 aArgc, stlChar** aArgv)
{
    stlInt32 sState = 0;

    STL_TRY(stlInitialize() == STL_SUCCESS);

    /*
     * Test stlNow()
     */
    sState = 1;
    STL_TRY(TestNow() == STL_TRUE);

    /*
     * Test stlPutAnsiTime()
     */
    sState = 2;
    STL_TRY( TestPutAnsiTime() == STL_TRUE );

    /*
     * Test stlMakeExpTimeByGMT()
     */
    sState = 3;
    STL_TRY( TestMakeExpTimeByGMT() == STL_TRUE );

    /*
     * Test stlMakeExpTimeByLocalTz()
     */
    sState = 4;
    STL_TRY( TestMakeExpTimeByLocalTz() == STL_TRUE );

    /*
     * Test stlSleep()
     */
    sState = 5;
    STL_TRY( TestSleep() == STL_TRUE );

    stlPrintf("PASS");
    
    return (0);

    STL_FINISH;

    stlPrintf("FAILURE %d", sState);

    return (-1);
}
