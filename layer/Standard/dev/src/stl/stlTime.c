/*******************************************************************************
 * stlTime.c
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
 * @file stlAtomic.c
 * @brief Standard Layer Atomic Routines
 */

#include <stlDef.h>
#include <stl.h>
#include <stt.h>
#include <ste.h>

/**
 * @addtogroup stlTime
 * @{
 */

/**
 * @brief stlAnsiTime을 stlTime으로 변환한다.
 * @param[in] aResult 변환된 stlTime
 * @param[in] aInput 변환할 stlAnsiTime
 * @param[in,out] aErrorStack 에러 스택
 */
stlStatus stlPutAnsiTime( stlTime        * aResult,
                          stlAnsiTime      aInput,
                          stlErrorStack  * aErrorStack )
{
    STL_TRY( sttPutAnsiTime( aResult, aInput ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    stlPushError( STL_ERROR_LEVEL_ABORT,
                  STL_ERRCODE_PARSER_SYSTEM_CALL_ERROR,
                  NULL,
                  aErrorStack,
                  "stlPutAnsiTime()",
                  0 );

    return STL_FAILURE;
}

/**
 * @brief 현재 시간을 반환한다.
 */
stlTime stlNow()
{
    return sttNow();
}

/**
 * @brief stlTime을 Coordinated Universal Time (UTC)을 적용해
 *        사람이 읽을 수 있는 stlExpTime으로 변환한다.
 * @param[in] aExpTime 변환된 stlExpTime
 * @param[in] aTime 변환활 stlTime
 */
void stlMakeExpTimeByGMT( stlExpTime * aExpTime,
                          stlTime      aTime)
{
    return sttExplode( aExpTime, aTime, 0 );
}

/**
 * @brief stlTime을 사용자의 Timezone을 적용해
 *        사람이 읽을 수 있는 stlExpTime으로 변환한다.
 * @param[in] aExpTime 변환된 stlExpTime
 * @param[in] aTime 변환활 stlTime
 */
void stlMakeExpTimeByLocalTz( stlExpTime * aExpTime,
                              stlTime      aTime)
{
    return sttExplode( aExpTime, aTime, 1 );
}

/**
 * @brief 특정 마이크로초만큼 sleep 한다.
 * @param[in] aInterval sleep할 시간
 */
void stlSleep( stlInterval aInterval )
{
    return sttSleep( aInterval );
}

/**
 * @brief 프로세스를 Busy Wait한다.
 */
void stlBusyWait( void )
{
#ifndef WIN32
    /*
     * 1 nanosec 정도는 1/HZ 보다 작은 값이기 때문에 대부분의 OS에서는
     * Busy Wait 한다.
     */
    struct timespec sTimeVal;
    struct timespec sRemainTime;
    
    sTimeVal.tv_nsec = 1;
    sTimeVal.tv_sec = 0;

    (void) nanosleep( &sTimeVal, &sRemainTime );
#else
    /**
     * @todo 구현해야 한다.
     */
    stlInt32 i;

    for( i = 0; i < 10000; i++ )
    {
    }
#endif
}

/**
 * @brief Timer를 생성한다.
 * @param[in]     aSigNo         Timer의 signal number
 * @param[in]     aNanoSecond    Timer Precision
 * @param[out]    aTimerId       생성된 Timer Identifier
 * @param[in,out] aErrorStack    에러 스택
 */
stlStatus stlCreateTimer( stlInt32        aSigNo,
                          stlInt64        aNanoSecond,
                          stlTimer      * aTimerId,
                          stlErrorStack * aErrorStack )
{
    return sttCreateTimer( aSigNo, aNanoSecond, aTimerId, aErrorStack );
}

/**
 * @brief Timer를 삭제한다.
 * @param[in]     aTimerId       Timer Identifier
 * @param[in,out] aErrorStack    에러 스택
 */
stlStatus stlDestroyTimer( stlTimer        aTimerId,
                           stlErrorStack * aErrorStack )
{
    return sttDestroyTimer( aTimerId, aErrorStack );
}

/** @} */
