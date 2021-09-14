/*******************************************************************************
 * dtlTimeZone.h
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
#ifndef _DTL_TIMEZONE_H_
#define _DTL_TIMEZONE_H_ 1

/**
 * @file dtlTimeZone.h
 * @brief DataType Layer TimeZone 관련 redirect 함수 정의 
 */

/**
 * @defgroup dtlTimeZone TimeZone
 * @ingroup dtlGeneral
 * @{
 */

/**
 * @brief Timezone 간격  (15분)
 */
#define DTL_TIMEZONE_WIDTH             (60 * 15)

#define DTL_TIMEZONE_MIN_HOUR          ( -14 )
#define DTL_TIMEZONE_MAX_HOUR          ( 14 )

/**
 * @brief 최소 Time Zone (-14:00)
 */
#define DTL_TIMEZONE_MIN_OFFSET        ( 60*60*DTL_TIMEZONE_MIN_HOUR )

/**
 * @brief 최대 Time Zone (+14:00)
 */
#define DTL_TIMEZONE_MAX_OFFSET        ( 60*60*DTL_TIMEZONE_MAX_HOUR )


stlStatus dtlTimeZoneToGMTOffset( dtlIntervalDayToSecondType * aTimeZoneInterval,
                                  stlInt32                   * aGMTOffset,
                                  stlErrorStack              * aErrorStack );
                                  
stlStatus dtlCheckGMTOffset( stlInt32         aGMTOffset,
                             stlErrorStack  * aErrorStack );

/** @} dtlTimeZone */


#endif /* _DTL_TIMEZONE_H_ */
