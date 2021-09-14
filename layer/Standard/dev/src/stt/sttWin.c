/*******************************************************************************
 * sttWin.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: sttUnix.c 13496 2014-08-29 05:38:43Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stc.h>
#include <stlDef.h>
#include <stlError.h>
#include <ste.h>

#if defined( STC_HAVE_SYS_TIME_H )
#include <sys/time.h>
#endif
#if defined( STC_HAVE_TIME_H )
#include <time.h>
#endif

/* Number of micro-seconds between the beginning of the Windows epoch
 * (Jan. 1, 1601) and the Unix epoch (Jan. 1, 1970)
 */
#define STT_DELTA_EPOCH_IN_USE STL_TIME_C(11644473600000000)

/* Leap year is any year divisible by four, but not by 100 unless also
 * divisible by 400
 */
#define STT_IS_LEAP_YEAR(aYear) ((!(aYear % 4)) ? (((aYear % 400) && !(aYear % 100)) ? 0 : 1) : 0)

TIME_ZONE_INFORMATION gSttTimezoneInfo;
DWORD gSttTimezoneInfoResult;
stlBool gSttInitTimezoneInfo = STL_FALSE;

static DWORD sttGetLocalTimezone(TIME_ZONE_INFORMATION ** aTimezoneInfo)
{
    if (gSttInitTimezoneInfo == STL_FALSE)
    {
        gSttTimezoneInfoResult = GetTimeZoneInformation(&gSttTimezoneInfo);
        gSttInitTimezoneInfo = STL_TRUE;
    }

    *aTimezoneInfo = &gSttTimezoneInfo;

    return gSttTimezoneInfoResult;
}

void sttFileTimeToStlTime(FILETIME * aFileTime, stlTime * aStlTime)
{
    /* Convert FILETIME one 64bit number so we can work with it. */
    *aStlTime = aFileTime->dwHighDateTime;	
    *aStlTime = (*aStlTime) << 32;	
    *aStlTime |= aFileTime->dwLowDateTime;
    *aStlTime /= 10; /* Convert from 100 nano-sec periods to micro-seconds. */
    *aStlTime -= STT_DELTA_EPOCH_IN_USE; /* Convert from Windows epoch to Unix epoch */
}

void sttStlTimeToFileTime(stlTime aStlTime, FILETIME * aFileTime)
{
    LONGLONG sValue;

    aStlTime += STT_DELTA_EPOCH_IN_USE;
    sValue = aStlTime * 10;
    aFileTime->dwLowDateTime = (DWORD)sValue;
    aFileTime->dwHighDateTime = (DWORD)(sValue >> 32);	
}

void sttSystemTileToStlExpTime(SYSTEMTIME * aSystemTime, stlExpTime * aExpTime)
{
    static const stlInt32 sDayOffset[12] = {
        0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };

    aExpTime->mMicroSecond = aSystemTime->wMilliseconds * 1000;
    aExpTime->mSecond = aSystemTime->wSecond;
    aExpTime->mMinute = aSystemTime->wMinute;
    aExpTime->mHour = aSystemTime->wMinute;
    aExpTime->mDay = aSystemTime->wDay;
    aExpTime->mMonth = aSystemTime->wMonth - 1;
    aExpTime->mYear = aSystemTime->wYear - 1900;
    aExpTime->mWeekDay = aSystemTime->wDayOfWeek;
    aExpTime->mYearDay = sDayOffset[aExpTime->mMonth] + (aSystemTime->wDay - 1);
    aExpTime->mIsDaylightSaving = 0;
    aExpTime->mGMTOffset = 0;

    if (STT_IS_LEAP_YEAR(aSystemTime->wYear) && (aExpTime->mYearDay > 58))
    {
        aExpTime->mYearDay++;
    }

}
stlStatus sttPutAnsiTime( stlTime     * aResult,
                          stlAnsiTime   aInput )
{
    *aResult = (stlTime)aInput * STL_USEC_PER_SEC;
    return STL_SUCCESS;
}

stlTime sttNow( void )
{
    stlTime    sTime = 0;
    FILETIME   sFileTime;
    SYSTEMTIME sSysTime;

    GetSystemTime(&sSysTime);
    SystemTimeToFileTime(&sSysTime, &sFileTime);

    sttFileTimeToStlTime(&sFileTime, &sTime);

    return sTime;
}

void sttExplode( stlExpTime * aExpTime,
                 stlTime      aTime,
                 stlInt32     aUseLocaltime )
{
    FILETIME   sFileTime;
    SYSTEMTIME sSysTime;

    FILETIME   sLocalFileTime;
    SYSTEMTIME sLocalSysTime;
    stlTime    sLocalTime;

    TIME_ZONE_INFORMATION * sTimeTzInfo;

    sttStlTimeToFileTime(aTime, &sFileTime);
    FileTimeToSystemTime(&sFileTime, &sSysTime);

    if (aUseLocaltime == 0)
    {
        sttSystemTileToStlExpTime(&sSysTime, aExpTime);
        aExpTime->mMicroSecond = (stlInt32)(aTime % STL_USEC_PER_SEC);
    }
    else
    {
        sttGetLocalTimezone(&sTimeTzInfo);

        SystemTimeToTzSpecificLocalTime(sTimeTzInfo, &sSysTime, &sLocalSysTime);
        sttSystemTileToStlExpTime(&sLocalSysTime, aExpTime);
        aExpTime->mMicroSecond = (stlInt32)(aTime % STL_USEC_PER_SEC);

        SystemTimeToFileTime(&sLocalSysTime, &sLocalFileTime);
        sttFileTimeToStlTime(&sLocalFileTime, &sLocalTime);
        aExpTime->mGMTOffset = (stlInt32)STL_GET_SEC_TIME(sLocalTime) - (stlInt32)STL_GET_SEC_TIME(aTime);

        aExpTime->mIsDaylightSaving = (aExpTime->mGMTOffset / 3600) - (-(sTimeTzInfo->Bias + sTimeTzInfo->StandardBias) / 60);
    }
}

void sttSleep( stlInterval aInterval )
{
    Sleep((DWORD)(aInterval / 1000));
}

stlStatus sttCreateTimer( stlInt32        aSigNo,
                          stlInt64        aNanoSecond,
                          stlTimer      * aTimerId,
                          stlErrorStack * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  STL_ERRCODE_NOTIMPL,
                  NULL,
                  aErrorStack );

    return STL_FAILURE;
}

stlStatus sttDestroyTimer( stlTimer        aTimerId,
                           stlErrorStack * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  STL_ERRCODE_NOTIMPL,
                  NULL,
                  aErrorStack );

    return STL_FAILURE;
}
