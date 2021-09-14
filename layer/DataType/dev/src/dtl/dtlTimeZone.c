/*******************************************************************************
 * dtlTimeZone.c
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
 * @file dtlTimeZone.c
 * @brief DataType Layer TimeZone 관련 redirect 함수 정의
 */

#include <dtl.h>

/**
 * @brief Interval Type 으로 정의된 Time Zone 을 GMT Offset(second) 로 변환한다.
 * @param[in]  aTimeZoneInterval  Time Zone Interval
 * @param[out] aGMTOffset         GMT Offset (second)
 * @param[in]  aErrorStack        Error Stack
 * @remarks
 */
stlStatus dtlTimeZoneToGMTOffset( dtlIntervalDayToSecondType * aTimeZoneInterval,
                                  stlInt32                   * aGMTOffset,
                                  stlErrorStack              * aErrorStack )
{
    stlInt32  sSecond = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aTimeZoneInterval != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aGMTOffset != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aTimeZoneInterval->mIndicator
                        == DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE, aErrorStack );

    /**
     * Second 단위로 환산 
     */
    
    sSecond = aTimeZoneInterval->mTime / DTL_USECS_PER_SEC;

    /**
     * Time Zone Validation 
     */
    STL_TRY_THROW( ( sSecond % DTL_TIMEZONE_WIDTH ) == 0, RAMP_ERR_INVALID_TIME_ZONE );
    STL_TRY_THROW( (sSecond >= DTL_TIMEZONE_MIN_OFFSET) &&
                   (sSecond <= DTL_TIMEZONE_MAX_OFFSET),
                   RAMP_ERR_INVALID_TIME_ZONE );
    
    /**
     * Output 설정
     */

    *aGMTOffset = sSecond;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_TIME_ZONE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_TIME_ZONE_DISPLACEMENT_VALUE,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief GMT Offset(second) 의 유효성 검사 
 * @param[in] aGMTOffset         GMT Offset (second)
 * @param[in] aErrorStack        Error Stack
 * @remarks
 */
stlStatus dtlCheckGMTOffset( stlInt32        aGMTOffset,
                             stlErrorStack * aErrorStack )
{
    STL_TRY_THROW( (aGMTOffset >= DTL_TIMEZONE_MIN_OFFSET) &&
                   (aGMTOffset <= DTL_TIMEZONE_MAX_OFFSET),
                   RAMP_ERR_INVALID_TIME_ZONE );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_TIME_ZONE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_TIME_ZONE_DISPLACEMENT_VALUE,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
