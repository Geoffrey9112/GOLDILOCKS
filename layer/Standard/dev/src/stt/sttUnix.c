/*******************************************************************************
 * sttUnix.c
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

#include <stc.h>
#include <stlDef.h>
#include <stlError.h>
#include <ste.h>
#include <stlMemorys.h>
#if defined( STC_HAVE_SYS_TIME_H )
#include <sys/time.h>
#endif
#if defined( STC_HAVE_TIME_H )
#include <time.h>
#endif

static stlInt32 sttGetGMTOffset( struct tm * aTime )
{
    stlInt32    sGMTOffset;

#if defined( STC_STRUCT_TM_TM_GMTOFF )
    sGMTOffset = aTime->tm_gmtoff;
#elif defined( STC_STRUCT_TM___TM_GMTOFF )
    sGMTOffset = aTime->__tm_gmtoff;
#else
    sttTimeVal  sNow;
    stlAnsiTime sTime1;
    stlAnsiTime sTime2;
    struct tm   sGMTTime;

    gettimeofday( &sNow, NULL );

    sTime1 = sNow.tv_sec;
    sTime2 = 0;

#if defined( STC_HAVE_GMTIME_R )
    gmtime_r( &sTime1, &sGMTTime );
#else
    sGMTTime = *gmtime( &sTime1 );
#endif
    sGMTTime.tm_isdst = 0;

    sTime2 = mktime( &sGMTTime );

    sGMTOffset = (stlInt32)difftime( sTime1, sTime2 );

    if( aTime->tm_isdst != 0 )
    {
        sGMTOffset += 3600;
    }

#endif

    return sGMTOffset;
}

stlStatus sttPutAnsiTime( stlTime     * aResult,
                          stlAnsiTime   aInput )
{
    *aResult = (stlTime)aInput * STL_USEC_PER_SEC;
    return STL_SUCCESS;
}

stlTime sttNow( void )
{
    sttTimeVal sTimeVal;
    
    gettimeofday( &sTimeVal, NULL );
    
    return (sTimeVal.tv_sec * STL_USEC_PER_SEC + sTimeVal.tv_usec);
}

void sttExplode( stlExpTime * aExpTime,
                 stlTime      aTime,
                 stlInt32     aUseLocaltime )
{
    struct tm   sTime;
    stlAnsiTime sAnsiTime;

    sAnsiTime = (aTime / STL_USEC_PER_SEC);

    aExpTime->mMicroSecond = aTime % STL_USEC_PER_SEC;

    if( aUseLocaltime )
    {
#if defined( STC_HAVE_LOCALTIME_R )
        localtime_r( &sAnsiTime, &sTime );
#else
        sTime = *localtime( &sAnsiTime );
#endif
    }
    else
    {
#if defined( STC_HAVE_GMTIME_R )
        gmtime_r( &sAnsiTime, &sTime );
#else
        sTime = *gmtime( &sAnsiTime );
#endif
    }

    aExpTime->mSecond           = sTime.tm_sec;
    aExpTime->mMinute           = sTime.tm_min;
    aExpTime->mHour             = sTime.tm_hour;
    aExpTime->mDay              = sTime.tm_mday;
    aExpTime->mMonth            = sTime.tm_mon;
    aExpTime->mYear             = sTime.tm_year;
    aExpTime->mWeekDay          = sTime.tm_wday;
    aExpTime->mYearDay          = sTime.tm_yday;
    aExpTime->mIsDaylightSaving = sTime.tm_isdst;
    aExpTime->mGMTOffset        = sttGetGMTOffset( &sTime );
}

void sttSleep( stlInterval aInterval )
{
#ifdef OS2
    DosSleep( aInterval / 1000 );
#elif defined(BEOS)
    snooze( aInterval );
#elif defined(NETWARE)
    delay( aInterval / 1000 );
#else
    sttTimeVal sTimeVal;
    
    sTimeVal.tv_usec = aInterval % STL_USEC_PER_SEC;
    sTimeVal.tv_sec = aInterval / STL_USEC_PER_SEC;
    
    (void) select(0, NULL, NULL, NULL, &sTimeVal);
#endif
}

stlStatus sttCreateTimer( stlInt32        aSigNo,
                          stlInt64        aNanoSecond,
                          stlTimer      * aTimerId,
                          stlErrorStack * aErrorStack )
{
    stlSigEvent  sSigEvent;
    stlTimerSpec sTimerSpec;

    stlMemset( &sSigEvent, 0x00, STL_SIZEOF(stlSigEvent) );
    stlMemset( &sTimerSpec, 0x00, STL_SIZEOF(stlTimerSpec) );
    
    sSigEvent.sigev_notify = SIGEV_SIGNAL;
    sSigEvent.sigev_signo = aSigNo;
    sSigEvent.sigev_value.sival_ptr = aTimerId;

    STL_TRY_THROW( timer_create( CLOCK_REALTIME,
                                 &sSigEvent,
                                 aTimerId ) == 0,
                   RAMP_ERR_CREATE_TIMER );
        
    sTimerSpec.it_value.tv_sec = aNanoSecond / 1000000000;
    sTimerSpec.it_value.tv_nsec = aNanoSecond % 1000000000;
    sTimerSpec.it_interval.tv_sec = sTimerSpec.it_value.tv_sec;
    sTimerSpec.it_interval.tv_nsec = sTimerSpec.it_value.tv_nsec;

    STL_TRY_THROW( timer_settime( *aTimerId,
                                  0,
                                  &sTimerSpec,
                                  NULL ) == 0,
                   RAMP_ERR_SET_TIMER );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CREATE_TIMER )
    {
        switch( errno )
        {
            case ENOMEM:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_OUT_OF_MEMORY,
                              NULL,
                              aErrorStack );
                break;
            default:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_CREATE_TIMER,
                              NULL,
                              aErrorStack );
                break;
        }
        steSetSystemError( errno, aErrorStack );
    }
    
    STL_CATCH( RAMP_ERR_SET_TIMER )
    {
        switch( errno )
        {
            case EINVAL:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_INVALID_ARGUMENT,
                              NULL,
                              aErrorStack );
                break;
            default:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_SET_TIMER,
                              NULL,
                              aErrorStack );
                break;
        }
        steSetSystemError( errno, aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus sttDestroyTimer( stlTimer        aTimerId,
                           stlErrorStack * aErrorStack )
{
    STL_TRY_THROW( timer_delete( aTimerId ) == 0,
                   RAMP_ERR_DESTROY_TIMER );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_DESTROY_TIMER )
    {
        switch( errno )
        {
            case EINVAL:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_INVALID_ARGUMENT,
                              NULL,
                              aErrorStack );
                break;
            default:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_DESTROY_TIMER,
                              NULL,
                              aErrorStack );
                break;
        }
        steSetSystemError( errno, aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;    
}
