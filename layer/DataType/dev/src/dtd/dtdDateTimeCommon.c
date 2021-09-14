/*******************************************************************************
 * dtdDateTimeCommon.c
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
 * @file dtdDateTimeCommon.c
 * @brief DataType Layer 데이타 중 DATE/TIME/TIMESTAMP/INTERVAL 타입관련 공통 함수 정의
 */

#include <dtl.h>

#include <dtDef.h>

/**
 * @addtogroup dtdDataType
 * @{
 */

static const stlInt64 dtlTimeTypeScale[DTL_TIME_MAX_PRECISION + 1] =
{
    STL_INT64_C(1000000),
    STL_INT64_C(100000),
    STL_INT64_C(10000),
    STL_INT64_C(1000),
    STL_INT64_C(100),
    STL_INT64_C(10),
    STL_INT64_C(1)
};
    
static const stlInt64 dtlTimeTypeOffset[DTL_TIME_MAX_PRECISION + 1] =
{
    STL_INT64_C(500000),
    STL_INT64_C(50000),
    STL_INT64_C(5000),
    STL_INT64_C(500),
    STL_INT64_C(50),
    STL_INT64_C(5),
    STL_INT64_C(0)
};

static const stlInt64 dtlTimestampTypeScale[DTL_TIMESTAMP_MAX_PRECISION + 1] =
{
    STL_INT64_C(1000000),
    STL_INT64_C(100000),
    STL_INT64_C(10000),
    STL_INT64_C(1000),
    STL_INT64_C(100),
    STL_INT64_C(10),
    STL_INT64_C(1)
};
    
static const stlInt64 dtlTimestampTypeOffset[DTL_TIMESTAMP_MAX_PRECISION + 1] =
{
    STL_INT64_C(500000),
    STL_INT64_C(50000),
    STL_INT64_C(5000),
    STL_INT64_C(500),
    STL_INT64_C(50),
    STL_INT64_C(5),
    STL_INT64_C(0)
};

static const stlInt64 dtlIntervalSecondScale[DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION + 1] =
{
    STL_INT64_C(1000000),
    STL_INT64_C(100000),
    STL_INT64_C(10000),
    STL_INT64_C(1000),
    STL_INT64_C(100),
    STL_INT64_C(10),
    STL_INT64_C(1)
};

static const stlInt64 dtlIntervalSecondOffset[DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION + 1] =
{
    STL_INT64_C(500000),
    STL_INT64_C(50000),
    STL_INT64_C(5000),
    STL_INT64_C(500),
    STL_INT64_C(50),
    STL_INT64_C(5),
    STL_INT64_C(0)
};


/**
 * @brief Calendar time을 Julian date로 변환.
 * @param[in] aYear     년
 * @param[in] aMonth    월 
 * @param[in] aDay      일
 * @return Julian Date
 */
stlInt32 dtdDate2JulianDate( stlInt32         aYear,
                             stlInt32         aMonth,
                             stlInt32         aDay )
{
     /**
     * Gregorlan Calendar -> Julian Day
     *  YYYY-MM-DD -> 1234567890
     *
     *  Gliese에서는 Proleptic Gregorian calendar를 사용하며,
     *  따라서, Julian Day는 November 24, 4714 BC 부터를 시작일로 한다. 
     *
     * < Gregorlan Calendar >
     * 현 Gliese는 Proleptic Gregorian calendar를 따르고 있음.
     *  . 이 방식은 SQL 표준, DB2, POSTGRES ... 등에서 사용.
     *  . SQL 표준에 기술된 아래의 문구를 근거로.
     *    Within the definition of a datetime literal, the datetime values are constrained by the natural rules
     *    for dates and times according to the Gregorian calendar.
     *    => 사실상 존재하지 않는 '1582-10-05' ~ '1582-10-14'도 Valid한 날짜로 계산한다.
     *
     * < Julian Day [Number] >   
     * proleptic Gregorian calendar에서의 시작일은 November 24, 4714 BC 부터이며,
     * Gliese는 이 날짜부터 Julian Day를 구한다.
     *
     * 
     * 
     * < 기타 참조 > 자세한 사항은 관련문서 참조.
     *   . Julian Calendar (BC 46년)
     *     태양력의 길이를 맞추기 위해 평년을 365일로 하되, 4년 마다 윤년(1년 366일)을 두고 1태양력을
     *     365.25일로 칭함.
     *   . Gregorlan Calendar (1582년)
     *     BC 46년 1월 1일부터 사용되어 온 율리우스 력의 오차가 누적되어 서기 1582년에 이르러 11일의 차이가 생김.
     *     이를 바로잡기 위해 1582년 10월 4일 다음날을 15일로 하고 1년을 365.2425일로 칭함.
     *   . Julian Day [Number]
     *     proleptic Gregorian calendar에서의 시작일은 November 24, 4714 BC 부터이고,
     *     proleptic Julian calendar에서의 시작일은 January 1, 4713 BC 부터이다. 
     *     
     * 
     *
     * -- Gregorian calendar date to julian day 계산 공식 ------------------------------------------
     *                         [참조] wikipedia Julian day  http://en.wikipedia.org/wiki/Julian_day
     *
     *    The algorithm is valid for all Gregorian calendar dates
     *    starting on March 1, 4801 BC (astronomical year -4800) at noon UT.[14]
     *
     *    a = ( 14 - month ) / 12
     *    y = year + 4800 - a
     *    m = month + 12a - 3
     *
     *    JDN = day + ( ( 153m + 2 )/ 5 ) + 365y + ( y / 4 ) - ( y / 100 ) + ( y / 400 ) - 32045
     *
     *    NOTE: When doing the divisions, the fractional parts of the quotients must be dropped.
     *    All years in the BC era must be converted to a negative value then incremented
     *    toward zero to be passed as an astronomical year, so that 1 BC will be passed as y=0.
     *
     * ---------------------------------------------------------------------------------------------
     **/
    
    stlInt32   sJulian;
    stlInt32   sCentury;
    stlInt32   sYear;
    stlInt32   sMonth;
    stlInt32   sDay;
    
    sYear = aYear;
    sMonth = aMonth;
    sDay = aDay;

    /**
     * 율리우스는 3월을 첫 달의 시작으로 1월, 2월은 전달의 13월, 14월로 계산한다.
     * 그러므로 if문으로 1월과 2월을 따로 처리한다.
     *
     * Year에 4800를 더하는 이유는 기원전을 계산해 주기 위함이며 BC 4801, 3, 1 부터의 일수를 계산하기 때문이다.
     *
     * - 왜 sYear에 +4800과 +4799로 나뉘는 가?
     *   1월과 2월은 율리우스력으로 전년도 13월, 14월이기 때문에 한 년을 뺀 +4799를 해준다.
     * 
     * - 왜 sMonth에 +1 또는 +13을 더해주는 가? 
     *   Julian Day로 계산시 1월은 전년달의 13월, 2월은 전년달의 14월이기 때문이다.
     *   월별로 Month 계산을 수행할 때 30일과 31일의 반복규칙이 당월+1로 계산해야 올바르게 된다.
     *  
     **/
    
    if( sMonth > 2 )                   
    {
        sMonth += 1;
        sYear += 4800;        
    }
    else
    {
        sMonth += 13;
        sYear += 4799;
    }

    /**
     *  * 율리우스력 Year의 계산
     *
     *  - 한 년의 일수는 365이므로 sJulian += sYear * 365를 해준다.
     *  
     *  - 32167을 빼주는 이유?
     *   율리우스의 시작일은 그레고리력과 같이 계산하여 표시했을 때 BC 4714-11-24부터 시작한다.
     *   그러므로 기원전 BC 4801-03-01에서 BC 4714-11-24까지의 일수 32045를 빼주게 된다.
     *
     *    365.2425는 그레고리력의 1년 윤달포함일
     *   
     *    BC 4801-03-01 - BC 4714-11-24
     *    (( 4800 - 4713 ) * 365.2425 ) + 11월24일(245+24) = 32045
     *
     *    32045 + 122 = 32167
     *
     *   그럼 +122이 많은 32167을 빼주는 것인가?
     *    이것은 율리우스력 Month계산에서 첫 달 3월의 일수를 122로 시작하여 
     *    계산하였기 때문에 계산하지 않아도 되는 달 수인 122만큼을 더 빼준 것이다.
     *   
     *  
     *  - 윤달 계산  
     *  sCentury = sYear / 100
     *  sJulian += sYear / 4 - sCentury + sCentury / 4 를 풀게되면
     *
     *  sJulian +=      sYear        sYear         sYear
     *                 -------  -   -------   +   -------
     *                    4           100           400 
     *
     *  율리우스력은 윤달을 year/4 를 했을 때 나눠지는 달을 윤달로 결정하였다. 하지만 오랜기간이 지날수록
     *  그 오차는 커졌고 결국 그레고리력부터 year/4 로 나눠지며 year/400으로 나눠져야지만 윤달로 계산했다.
     *  
     **/
    
    sCentury = sYear / 100;
    sJulian = sYear * 365 - 32167;
    sJulian += sYear / 4 - sCentury + sCentury / 4;

    /**
     * 
     *  * 율리우스력 Month 의 계산      --------------------------------------------------------
     *                                 *  basic Month  M(Julian Month )   Month Date   day    *    
     *     7834 * M                    --------------------------------------------------------
     *   ------------ = MONTH DATE     *       3             4             122          0     *           
     *       256                       *       4             5             153          31    *
     *                                 *       5             6             183          30    *
     *                                 *       6             7             214          31    *
     *                                 *       7             8             244          30    *
     *                                 *       8             9             275          31    *
     *                                 *       9            10             306          31    *
     *                                 *      10            11             336          30    *
     *                                 *      11            12             367          31    *
     *                                 *      12            13             397          30    *
     *                                 *       1            14             428          31    *
     *                                 *       2            15             439          31    *
     *                                 ---------------------------------------------------------
     *                                 
     *  위 MONTH은 율리우스력에 실질적으로 계산되는 MONTH의 DAY 계산 법이다.    
     *
     *  basic Month는 그레고리력의 달을 의미하며 (현재 사용하는 월일 )
     *
     *  M(Julian Month)는 basic Month에 if문을 통해 +1또는 +13을 해준 율리우스력 월이다.
     *
     *  Month Date는 실제로 계산되는 일수이며 
     *  Month의 day는 3월일 경우 첫 달이므로 Month를 계산해 주지 않아도 되고
     *  4월일 경우 전 달보다 31일이 경과되었으므로 이전달의 +31을 해준 값을 더하게 된다.
     *  5월인 경우는 경과된 날 31일 + 30일 만큼을 더한 값으로 계산한다.
     *  
     *  -Month Date가 +122 씩을 더하는 이유?
     *  122는 위 년도 계산시 -32045 + (-122) = -32167로 총 일수에서 빼주게 된다.
     *  그러므로 실질적으로는 +0인 셈이다. (다른 달도 마찬가지)
     *
     *  
     *  * 율리우스력 Day 의 계산
     *     남은 일수는  +Day 해주면 된다. 
     **/

    sJulian += 7834 * sMonth / 256 + sDay;

    return sJulian;
}

/**
 * @brief Julian date를 Calendar time으로 변환.
 * @param[in]  aJulianDate  JulianDate
 * @param[out] aYear        년
 * @param[out] aMonth       월 
 * @param[out] aDay         일 
 * @param[out] aErrorStack  에러 스택 
 */
stlStatus dtdJulianDate2Date ( stlInt32         aJulianDate,
                               stlInt32       * aYear,
                               stlInt32       * aMonth,
                               stlInt32       * aDay,
                               stlErrorStack  * aErrorStack )
{
    /**
     *
     *   -- Gregorian calendar from julian day 계산 공식 
     *                         [참조] wikipedia Julian day  http://en.wikipedia.org/wiki/Julian_day
     *
     *   * Let J be the Julian day number from which we want to compute the date components.
     *
     *   * From J, compute a relative Julian day number j from a Gregorian epoch starting on
     *     March 1, −4800 (i.e. March 1, 4801 BC in the proleptic Gregorian Calendar),
     *     the beginning of the Gregorian quadricentennial 32,044 days before the epoch of the Julian Period.
     *
     *   * From j, compute the number g of Gregorian quadricentennial cycles elapsed
     *     (there are exactly 146,097 days per cycle) since the epoch;
     *     subtract the days for this number of cycles, it leaves dg days since the beginning of the current cycle.
     *
     *   * From dg, compute the number c (from 0 to 4) of Gregorian centennial cycles
     *     (there are exactly 36,524 days per Gregorian centennial cycle) elapsed since
     *     the beginning of the current Gregorian quadricentennial cycle, number reduced to a maximum of 3
     *     (this reduction occurs for the last day of a leap centennial year where c would be 4 if it were not
     *     reduced); subtract the number of days for this number of Gregorian centennial cycles,
     *     it leaves dc days since the beginning of a Gregorian century.
     *
     *   * From dc, compute the number b (from 0 to 24) of Julian quadrennial cycles
     *     (there are exactly 1,461 days in 4 years, except for the last cycle which may be incomplete by 1 day)
     *     since the beginning of the Gregorian century;
     *     subtract the number of days for this number of Julian cycles, it leaves db days in the Gregorian century
     *
     *   * From db, compute the number a (from 0 to 4) of Roman annual cycles
     *     (there are exactly 365 days per Roman annual cycle) since the beginning of the Julian quadrennial cycle,
     *     number reduced to a maximum of 3 (this reduction occurs for the leap day,
     *     if any, where a would be 4 if it were not reduced);
     *     subtract the number of days for this number of annual cycles,
     *     it leaves da days in the Julian year (that begins on March 1).
     *
     *   * Convert the four components g, c, b, a into the number y of years since the epoch,
     *     by summing their values weighted by the number of years that each component represents
     *     (respectively 400 years, 100 years, 4 years, and 1 year).
     *
     *   * With da, compute the number m (from 0 to 11) of months since March
     *     (there are exactly 153 days per 5-month cycle; however, these 5-month cycles are offset by 2 months
     *     within the year, i.e. the cycles start in May, and so the year starts with an initial fixed number
     *     of days on March 1, the month can be computed from this cycle by a Euclidian division by 5);
     *     subtract the number of days for this number of months (using the formula above),
     *     it leaves d days past since the beginning of the month.
     *
     *   * The Gregorian date (Y, M, D) can then be deduced by simple shifts from (y, m, d).
     *
     *   The calculations below (which use integer division [div] and modulo [mod] with positive numbers only)
     *   are valid for the whole range of dates since −4800.
     *   For dates before 1582, the resulting date components are valid only in the Gregorian proleptic calendar.
     *   This is based on the Gregorian calendar but extended to cover dates before its introduction,
     *   including the pre-Christian era. For dates in that era (before year AD 1),
     *   astronomical year numbering is used. This includes a year zero, which immediately precedes AD 1.
     *   Astronomical year zero is 1 BC in the proleptic Gregorian calendar and,
     *   in general, proleptic Gregorian year (n BC) = astronomical year (Y = 1 − n).
     *   For astronomical year Y (Y < 1), the proleptic Gregorian year is (1 − Y) BC.
     *
     *   J = JD + 0.5 
     *  (note: this shifts the epoch back by one half day, to start it at 00:00UTC, instead of 12:00 UTC);
     *  * 0.5 : 시간상의 차이를 소수점으로 표시 0.0는 00:00시 0.5는 12:00시이다.
     *
     *  * 32044 : his shifts the epoch back to astronomical year -4800 instead of
     *            the start of the Christian era in year AD 1 of the proleptic Gregorian calendar
     *  
     *           365.2425는 그레고리력의 윤달포함일
     *           BC 4801-03-01 - BC 4714-11-24
     *           (( 4800 - 4713 ) * 365.2425 ) + 11월24일(245+24) = 32045
     *
     *           Gregorian date에서 Julian date를 구할 때에는 Julian에 -32045가 적용된 경우이므로
     *           올바른 수식을 위해서는 32045를 더해주게 된다.
     *           
     *   j = J + 32044
     * 
     *
     *  * 윤년일 : 율리우스력에서의 1년은 365.25일이다.
     *             그레고리력에서의 1년은 365.2425일로 세가지 조건으로 윤년을 정한다.
     *             1) 서기 년도가 4로 나누어지는 해는 윤년.
     *             2) 그 중 100으로 나누어지는 해는 평년.
     *             3) 그 중 400으로 나누어지는 해는 윤년.
     *            
     *    아래는 윤년포함 일 계산이다.
     *            
     *  g = j div 146097;  dg = j mod 146097;
     *   * 146097 : 4 * 36524 + 1  (400년)
     *
     *  c = (dg div 36524 + 1) × 3 div 4; dc = dg − c × 36524;
     *   * 36524 : 25 * 1460 - 1  (100년)
     *    100년 단위일 경우, 400년 또는 4년으로 정확히 나눠지는 윤년일을 알 수 없기 때문에
     *    정확한 계산을 위해서는 100년으로 나눈 값에 1을 더해 3년으로 다시 계산하여 윤년단위인 4로 나눈다.
     *    예를들면,
     *
     *             36524 + 36524 + 36524 + 36524 = 146096 ( 윤년 하루가 빠진 400년이므로 4로 나눠지면 안된다. )
     *             146096 / 36524 = 4
     *             그러므로 ( 146096 / 36524 + 1 ) * 3 div 4 = 3.75 ( 300년하고 4/3 윤년일 )
     *
     *  b = dc div 1461;  db = dc mod 1461;
     *   * 1461 : 3 * 365 + 366  (4년)
     *
     *  a = (db div 365 + 1) × 3 div 4;  da = db − a × 365;
     *   * 1년 단위로 계산할 시, 윤년 일을 알 수 없기 때문에 정확한 계산을 위해서는 +1 을 한 뒤에 다시
     *     3년으로 곱해 4년으로 나눈다.
     *     예를들면,
     *
     *              365 * 365 * 365 * 365 = 1460 ( 윤년 하루가 빠졌으므로 4년이 되면 안된다. ) 
     *              1460 / 365 = 4
     *              그러므로, ( 1460 / 365 + 1 ) * 3 div 4 = 3.75 ( 올바른 값 3년하고 4/3 윤년일 )
     *
     *  y = g × 400 + c × 100 + b × 4 + a;
     *  (note: this is the integer number of full years elapsed since March 1, 4801 BC at 00:00 UTC);
     *
     *  * 153 : 그레고리력으로 표시되는 율리우스력은 매년 3월을 첫 달로 계산한다. 3월을 첫 달의 기준으로
     *         5달씩 묶어 일수로 계산하면 153( 31 + 30 + 31 + 30 + 31 )이 반복된다.
     *
     *        ------------------------------------------------------------------
     *          1    2  |  3    4  |  5    6    7    8    9  |  10    11    12
     *        ----------|----------|-------------------------|------------------
     *                  |  0   30  |          153            |            153
     * 
     *  m = (da × 5 + 308) div 153 − 2;
     *  (note: this is the integer number of full months elapsed since the last March 1 at 00:00 UTC);
     *   
     *  d = da − (m + 4) × 153 div 5 + 122;
     *  (note: this is the number of days elapsed since day 1 of the month at 00:00 UTC, including fractions of one day);
     *  
     *  Y = y − 4800 + (m + 2) div 12; let M = (m + 2) mod 12 + 1; let D = d + 1;
     *   * 4800 : 율리우스력의 시작 년인 기원전 BC 4800  
     *
     *  return astronomical Gregorian date (Y, M, D).
     *  
     * 
     *
     **/
    
    stlUInt32 sJulian;
    stlUInt32 sQuad;
    stlUInt32 sExtra;
    stlInt32  sYear;
    stlInt32  sMonth;
    stlInt32  sDay;

    sJulian = aJulianDate;
    sJulian += 32044;
    sQuad = sJulian / 146097;
    sExtra = ( sJulian - sQuad * 146097 ) * 4 + 3;
    sJulian += 60 + sQuad * 3 + sExtra / 146097;
    sQuad = sJulian / 1461;
    sJulian -= sQuad * 1461;
    sYear = sJulian * 4 / 1461;
    sJulian = ( (sYear != 0) ? ((sJulian + 305) % 365) : ((sJulian + 306) % 366));
    sJulian += 123;

    sYear += sQuad * 4;
    sYear -= 4800;
    sQuad = sJulian * 2141 / 65536;
    sDay = sJulian - 7834 * sQuad / 256;
    sMonth = ( sQuad + 10 ) % 12 + 1;

    *aYear = sYear;
    *aMonth = sMonth;
    *aDay = sDay;
    
    return STL_SUCCESS;
}

/**
 * @brief timestamp type을 Time(Hour, Minute, Second)으로 변환.
 * @param[in] aTimestamp          Timestamp Value
 * @param[out] aHour              시 
 * @param[out] aMinute            분 
 * @param[out] aSecond            초 
 * @param[out] aFractionalSecond  fractional Second
 */
void dtdTimestamp2Time( dtlTimestampType      aTimestamp,
                        stlInt32            * aHour,
                        stlInt32            * aMinute,
                        stlInt32            * aSecond,
                        dtlFractionalSecond * aFractionalSecond )
{
    dtlTimeOffset       sTimeOffset;
    stlInt32            sHour;
    stlInt32            sMinute;
    stlInt32            sSecond;
    dtlFractionalSecond sFractionalSecond;

    sTimeOffset = aTimestamp;

    sHour = sTimeOffset / DTL_USECS_PER_HOUR;
    sTimeOffset -= sHour * DTL_USECS_PER_HOUR;
    sMinute = sTimeOffset / DTL_USECS_PER_MINUTE;
    sTimeOffset -= sMinute * DTL_USECS_PER_MINUTE;
    sSecond = sTimeOffset / DTL_USECS_PER_SEC;
    sFractionalSecond = sTimeOffset - ( sSecond * DTL_USECS_PER_SEC );

    *aHour = sHour;
    *aMinute = sMinute;
    *aSecond = sSecond;
    *aFractionalSecond = sFractionalSecond;
}

/**
 * @brief date type, timestamp type을 dtlExpTime으로 변환.
 * @param[in]  aDateTime
 * @param[out] aDtlExpTime        
 * @param[out] aFractionalSecond
 * @param[out] aErrorStack
 */
stlStatus dtdDateTime2dtlExpTime( dtlTimeOffset           aDateTime,
                                  dtlExpTime            * aDtlExpTime,
                                  dtlFractionalSecond   * aFractionalSecond,
                                  stlErrorStack         * aErrorStack )
{
    dtlTimestampType   sDate;
    dtlTimestampType   sTime;

    sTime = aDateTime;

    DTL_TIMESTAMP_MODULO( sTime, sDate, DTL_USECS_PER_DAY );

    if( sTime < STL_INT64_C(0) )
    {
        sTime += DTL_USECS_PER_DAY;
        sDate -= 1;
    }
    else
    {
        /* Do Nothing */
    }

    sDate += DTL_EPOCH_JDATE;

    STL_TRY_THROW( (sDate >= 0) && (sDate <= (dtlTimeOffset)STL_INT32_MAX),
                   ERROR_OUT_OF_RANGE );

    STL_TRY( dtdJulianDate2Date ( (stlInt32)sDate,
                                  & aDtlExpTime->mYear,
                                  & aDtlExpTime->mMonth,
                                  & aDtlExpTime->mDay,
                                  aErrorStack )
             == STL_SUCCESS );

    dtdTimestamp2Time( sTime,
                       & (aDtlExpTime->mHour),
                       & (aDtlExpTime->mMinute),
                       & (aDtlExpTime->mSecond),
                       aFractionalSecond );

//    aDtlExpTime->mIsDaylightSaving = -1;
    aDtlExpTime->mIsDaylightSaving = 0;
    aDtlExpTime->mTimeGmtOff = 0;
    aDtlExpTime->mTimeZone = NULL;
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_TIMESTAMP_NOT_CONTAIN_VALID_TIMESTAMP,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;   
}

/**
 * @brief timestampTz type을 dtlExpTime으로 변환.
 * @param[in]  aTimestampTzType     
 * @param[out] aTimeZone          
 * @param[out] aDtlExpTime        
 * @param[out] aFractionalSecond
 * @param[out] aErrorStack
 */
stlStatus dtdTimestampTz2dtlExpTime( dtlTimestampTzType      aTimestampTzType,
                                     stlInt32              * aTimeZone,
                                     dtlExpTime            * aDtlExpTime,
                                     dtlFractionalSecond   * aFractionalSecond,
                                     stlErrorStack         * aErrorStack )
{
    dtlTimestampType   sTime;
    dtlTimeT           sUnixEpochTime;

    sTime = aTimestampTzType.mTimestamp - (aTimestampTzType.mTimeZone * DTL_USECS_PER_SEC);

    STL_TRY( dtdDateTime2dtlExpTime( sTime,
                                     aDtlExpTime,
                                     aFractionalSecond,
                                     aErrorStack )
             == STL_SUCCESS );

    aTimestampTzType.mTimestamp = (aTimestampTzType.mTimestamp - *aFractionalSecond)
        / DTL_USECS_PER_SEC + (DTL_EPOCH_JDATE - DTL_UNIX_EPOCH_JDATE) * DTL_SECS_PER_DAY;

    sUnixEpochTime = (dtlTimeT)aTimestampTzType.mTimestamp;

    if( ((dtlTimestampType)sUnixEpochTime) == aTimestampTzType.mTimestamp )
    {
        aDtlExpTime->mTimeGmtOff = aTimestampTzType.mTimeZone;
        
        *aTimeZone = (stlInt32)(aDtlExpTime->mTimeGmtOff);
    }
    else
    {
        /*
         * dtlTimeT out of range 
         */
        *aTimeZone = 0;
        aDtlExpTime->mIsDaylightSaving = -1;
        aDtlExpTime->mTimeGmtOff = 0;
        aDtlExpTime->mTimeZone = NULL;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;   
}

/**
 * @brief date/time을 string으로 변환시, fractionalSecond의 precision에 채워져 있는 0 제거
 * @param[in]     aStr      string으로 변환된 date/time
 * @param[in]     aLength   문자열의 길이
 * @return 변환된 문자열의 길이
 */
stlInt32 dtdTrimTrailingZeros( stlChar  * aStr,
                               stlInt32   aLength )
{
    stlInt32  sLen;
    
    sLen = aLength;
    
    while( (sLen > 1) && (*(aStr + sLen - 1) == '0' ) && (*(aStr + sLen - 2) != '.') )
    {
        sLen--;
    }

    return sLen;
}

/**
 * @brief date/time을 string으로 변환시, second부분에 대한 변환수행후 string에 추가.
 * @param[out] aStr               date/time을 string으로 변환후 저장할 공간
 * @param[out] aResultStrSize     변환된 문자열의 길이
 * @param[in]  aSecond            second
 * @param[in]  aFractionalSecond  fractionalSecond
 * @param[in]  aPrecision         fractionalSecond Precision
 * @param[in]  aFillZeros         fractionalSecond Precision만큼 0을 채울지의 여부
 * @param[out] aErrorStack        stlErrorStack
 */
stlStatus dtdAppendSeconds( stlChar             * aStr,
                            stlInt32            * aResultStrSize,
                            stlInt32              aSecond,
                            dtlFractionalSecond   aFractionalSecond,
                            stlInt32              aPrecision,
                            stlBool               aFillZeros,
                            stlErrorStack       * aErrorStack )  
{
    stlInt32            sSize = 0;
    stlInt32            sUintFormatStrSize = 0;
    stlInt32            sSecond;
    dtlFractionalSecond sFractionalSecond;

    sSecond = (aSecond < 0) ? -aSecond : aSecond;
    sFractionalSecond = (aFractionalSecond < 0) ? -aFractionalSecond : aFractionalSecond;
    
    if( aFractionalSecond == 0 )
    {
        STL_TRY( dtdIntegerToLeadingZeroFormatString( sSecond,
                                                      2,
                                                      & sUintFormatStrSize,
                                                      aStr + sSize,
                                                      aErrorStack )
                 == STL_SUCCESS );
        sSize += sUintFormatStrSize;
    }
    else
    {
        STL_TRY( dtdIntegerToLeadingZeroFormatString( sSecond,
                                                      2,
                                                      & sUintFormatStrSize,
                                                      aStr + sSize,
                                                      aErrorStack )
                 == STL_SUCCESS );
        sSize += sUintFormatStrSize;

        *(aStr + sSize) = '.';
        sSize++;
        
        STL_TRY( dtdIntegerToLeadingZeroFormatString( sFractionalSecond,
                                                      6,
                                                      & sUintFormatStrSize,
                                                      aStr + sSize,
                                                      aErrorStack )
                 == STL_SUCCESS );
        sSize += sUintFormatStrSize;
        
        sSize = dtdTrimTrailingZeros( aStr, sSize );
    }
    
    *aResultStrSize = sSize;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;       
}

/* /\** */
/*  * @brief date/time을 string으로 변환시, second부분에 대한 변환수행후 string에 추가. */
/*  * @param[out] aStr               date/time을 string으로 변환후 저장할 공간 */
/*  * @param[in]  aSecond            second */
/*  * @param[in]  aFractionalSecond  fractionalSecond */
/*  * @param[in]  aPrecision         fractionalSecond Precision */
/*  * @param[in]  aFillZeros         fractionalSecond Precision만큼 0을 채울지의 여부 */
/*  * @return 변환된 문자열의 길이 */
/*  *\/ */
/* stlInt32 dtdAppendSecondsForWithTimeZone( stlChar             * aStr, */
/*                                           stlInt32              aSecond, */
/*                                           dtlFractionalSecond   aFractionalSecond, */
/*                                           stlInt32              aPrecision, */
/*                                           stlBool               aFillZeros )   */
/* { */
/*     /\* */
/*      * @todo 임시코드임. */
/*      * gsql의 datetime insert를 format에 맞게 처리하기 위한 임시코드임. */
/*      * ODBC, CDC 작업을 위해 새로 작성되어야 함. */
/*      *\/     */
    
/*     stlInt32            sSize = 0; */
/*     stlInt32            sUintFormatStrSize = 0; */
/*     stlInt32            sSecond; */
/*     dtlFractionalSecond sFractionalSecond; */

/* //    stlInt32            sLen = 0; */

/*     sSecond = (aSecond < 0) ? -aSecond : aSecond; */
/*     sFractionalSecond = (aFractionalSecond < 0) ? -aFractionalSecond : aFractionalSecond; */
    
/*     if( aFractionalSecond == 0 ) */
/*     { */
/*         STL_TRY( dtdIntegerToLeadingZeroFormatString( sSecond, */
/*                                                       2, */
/*                                                       & sUintFormatStrSize, */
/*                                                       aStr + sSize ) */
/*                  == STL_SUCCESS ); */
/*         sSize += sUintFormatStrSize; */

/*         *(aStr + sSize) = '.'; */
/*         sSize++; */
        
/*         *(aStr + sSize) = '0'; */
/*         sSize++; */
/*     } */
/*     else */
/*     { */
/*         STL_TRY( dtdIntegerToLeadingZeroFormatString( sSecond, */
/*                                                       2, */
/*                                                       & sUintFormatStrSize, */
/*                                                       aStr + sSize ) */
/*                  == STL_SUCCESS ); */
/*         sSize += sUintFormatStrSize; */
        
/*         *(aStr + sSize) = '.'; */
/*         sSize++; */
        
/*         STL_TRY( dtdIntegerToLeadingZeroFormatString( sFractionalSecond, */
/*                                                       6, */
/*                                                       & sUintFormatStrSize, */
/*                                                       aStr + sSize ) */
/*                  == STL_SUCCESS ); */
/*         sSize += sUintFormatStrSize; */
        
/*         sSize = dtdTrimTrailingZeros( aStr, sSize ); */

/*         /\* sLen = sSize; *\/ */

/*         /\* while( (sLen > 1) && (*(aStr + sLen - 1) == '0' ) && (*(aStr + sLen - 2) != '.') ) *\/ */
/*         /\* { *\/ */
/*         /\*     sLen--; *\/ */
/*         /\* } *\/ */

/*         /\* sSize = sSize + sLen;         *\/ */
/*     } */

/*     return sSize; */
/* } */

/* /\** */
/*  * @brief dtlExpTime(date/time)을 string으로 변환. */
/*  * @param[in]  aDtlExpTime       dtlExpTime   */
/*  * @param[in]  aFractionalSecond fractionalSecond */
/*  * @param[in]  aTimeZone         TimeZone */
/*  * @param[in]  aStyle            date/time 출력 style */
/*  * @param[in]  aDateOrder        date 출력 order */
/*  *                          <BR> DTL_DATE_ORDER_YMD, DTL_DATE_ORDER_DMY, DTL_DATE_ORDER_MDY */
/*  * @param[out] aStr              string으로 변환후 저장할 공간 */
/*  * @param[out] aLength            변환된 문자열의 길이 */
/*  * @param[out] aErrorStack       에러 스택 */
/*  *\/ */
/* stlStatus dtdEncodeDateTime( dtlExpTime           * aDtlExpTime, */
/*                              dtlFractionalSecond    aFractionalSecond, */
/*                              stlInt32             * aTimeZone, */
/*                              stlInt32               aStyle, */
/*                              stlInt32               aDateOrder, */
/*                              stlChar              * aStr, */
/*                              stlInt32             * aLength, */
/*                              stlErrorStack        * aErrorStack ) */
/* { */
/*     stlInt32   sYear; */
/*     stlInt32   sSize = 0; */

/*     STL_PARAM_VALIDATE( */
/*         (aDtlExpTime->mMonth >= 1) && (aDtlExpTime->mMonth <= DTL_MONTHS_PER_YEAR), */
/*         aErrorStack ); */

/*     if( aDtlExpTime->mYear > 0 ) */
/*     { */
/*         sYear = aDtlExpTime->mYear; */
/*     } */
/*     else */
/*     { */
/*         sYear = -(aDtlExpTime->mYear - 1); */
/*     } */

/*     if( aStyle == DTL_DATES_STYLE_ISO ) */
/*     { */
/*         sSize += dtdIntegerToLeadingZeroFormatString( sYear, */
/*                                                       4, */
/*                                                       aStr + sSize ); */
        
/*         *(aStr + sSize) = '-'; */
/*         sSize++; */
        
/*         sSize += dtdIntegerToLeadingZeroFormatString( aDtlExpTime->mMonth, */
/*                                                       2, */
/*                                                       aStr + sSize ); */
        
/*         *(aStr + sSize) = '-'; */
/*         sSize++; */
        
/*         sSize += dtdIntegerToLeadingZeroFormatString( aDtlExpTime->mDay, */
/*                                                       2, */
/*                                                       aStr + sSize ); */
        
/*         *(aStr + sSize) = ' '; */
/*         sSize++; */

/*         sSize += dtdIntegerToLeadingZeroFormatString( aDtlExpTime->mHour, */
/*                                                       2, */
/*                                                       aStr + sSize ); */
        
/*         *(aStr + sSize) = ':'; */
/*         sSize++; */
        
/*         sSize += dtdIntegerToLeadingZeroFormatString( aDtlExpTime->mMinute, */
/*                                                       2, */
/*                                                       aStr + sSize ); */
        
/*         *(aStr + sSize) = ':'; */
/*         sSize++; */
        
/*         sSize += dtdAppendSecondsForWithTimeZone( aStr + sSize, */
/*                                                   aDtlExpTime->mSecond, */
/*                                                   aFractionalSecond, */
/*                                                   DTL_TIMESTAMP_MAX_PRECISION, */
/*                                                   STL_TRUE ); */
        
/*         /\* */
/*          * aTimeZone == NULL 이면, output string으로 time zone 정보를 출력하지 않음을 의미. */
/*          * aDtlExpTime->mIsDaylightSaving != -1 이면, 올바른 time zone 정보를 가짐을 의미. */
/*          *\/  */

/*         if( (aTimeZone != NULL) && (aDtlExpTime->mIsDaylightSaving >= 0) ) */
/*         { */
/*             sSize += dtdEncodeTimeZone( aStr + sSize, */
/*                                         *aTimeZone );             */
/*         } */
/*         else */
/*         { */
/*             // Do Nothing */
/*         } */

/*         if( aDtlExpTime->mYear <= 0 ) */
/*         { */
/*             *(aStr + sSize) = ' '; */
/*             sSize++; */
/*             *(aStr + sSize) = 'B'; */
/*             sSize++; */
/*             *(aStr + sSize) = 'C'; */
/*             sSize++; */
/*         } */
/*         else */
/*         { */
/*             /\* Do Nothing *\/ */
/*         } */
/*     } */
/*     else if( aStyle == DTL_DATES_STYLE_SQL ) */
/*     { */
/*         if( aDateOrder == DTL_DATE_ORDER_DMY ) */
/*         { */
/*             sSize += dtdIntegerToLeadingZeroFormatString( aDtlExpTime->mDay, */
/*                                                           2, */
/*                                                           aStr + sSize ); */
            
/*             *(aStr + sSize) = '/'; */
/*             sSize++; */
            
/*             sSize += dtdIntegerToLeadingZeroFormatString( aDtlExpTime->mMonth, */
/*                                                           2, */
/*                                                           aStr + sSize ); */
/*         } */
/*         else */
/*         { */
/*             sSize += dtdIntegerToLeadingZeroFormatString( aDtlExpTime->mMonth, */
/*                                                           2, */
/*                                                           aStr + sSize ); */
            
/*             *(aStr + sSize) = '/'; */
/*             sSize++; */
            
/*             sSize += dtdIntegerToLeadingZeroFormatString( aDtlExpTime->mDay, */
/*                                                           2, */
/*                                                           aStr + sSize ); */
/*         } */

/*         *(aStr + sSize) = '/'; */
/*         sSize++; */

/*         sSize += dtdIntegerToLeadingZeroFormatString( sYear, */
/*                                                       4, */
/*                                                       aStr + sSize ); */

/*         *(aStr + sSize) = ' '; */
/*         sSize++; */

/*         sSize += dtdIntegerToLeadingZeroFormatString( aDtlExpTime->mHour, */
/*                                                       2, */
/*                                                       aStr + sSize ); */
        
/*         *(aStr + sSize) = ':'; */
/*         sSize++; */
        
/*         sSize += dtdIntegerToLeadingZeroFormatString( aDtlExpTime->mMinute, */
/*                                                       2, */
/*                                                       aStr + sSize ); */
        
/*         *(aStr + sSize) = ':'; */
/*         sSize++; */
        
/*         sSize += dtdAppendSeconds( aStr + sSize, */
/*                                    aDtlExpTime->mSecond, */
/*                                    aFractionalSecond, */
/*                                    DTL_TIMESTAMP_MAX_PRECISION, */
/*                                    STL_TRUE ); */
        
/*         if( (aTimeZone != NULL) && (aDtlExpTime->mIsDaylightSaving >= 0) ) */
/*         { */
/*             sSize += dtdEncodeTimeZone( aStr + sSize, */
/*                                         *aTimeZone );             */
/*         } */
/*         else */
/*         { */
/*             // Do Nothing */
/*         } */

/*         if( aDtlExpTime->mYear <= 0 ) */
/*         { */
/*             *(aStr + sSize) = ' '; */
/*             sSize++; */
            
/*             *(aStr + sSize) = 'B'; */
/*             sSize++; */
            
/*             *(aStr + sSize) = 'C'; */
/*             sSize++; */
/*         } */
/*         else */
/*         { */
/*             /\* Do Nothing *\/ */
/*         } */
/*     } */
/*     else */
/*     { */
/*         STL_THROW( ERROR_INTERNAL_ERROR ); */
/*     } */

/*     *aLength = sSize; */

/*     return STL_SUCCESS; */

/*     STL_CATCH( ERROR_INTERNAL_ERROR ) */
/*     { */
/*         stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                       DTL_ERRCODE_NOT_IMPLEMENTED, */
/*                       NULL, */
/*                       aErrorStack, */
/*                       "dtdEncodeDateTime()"); */
/*     }     */

/*     STL_FINISH; */

/*     return STL_FAILURE; */
/* } */

/**
 * @brief input string을 date/time문법에 맞는 token 단위로 분류
 * @param[in]  aTimeStr       input date/time string
 * @param[in]  aTimeStrLength aString의 길이 
 * @param[in]  aDataType      data type
 * @param[in]  aBuffer        field string 저장을 위한 작업공간
 * @param[in]  aBufferLen     aBuffer의 size
 * @param[out] aField         field string의 pointer를 저장하는 배열 
 * @param[out] aFieldType     field type들의 배열 
 * @param[in]  aMaxFields     aField, aFieldType의 크기
 * @param[out] aNumFields     aField, aFieldType에 저장된 실제 수 
 * @param[out] aErrorStack    에러 스택
 * @remark
 *   Several field types are assigned:
 *	<BR> DTL_DTK_NUMBER - digits and (possibly) a decimal point
 *	<BR> DTL_DTK_DATE - digits and two delimiters, or digits and text
 *	<BR> DTL_DTK_TIME - digits, colon delimiters, and possibly a decimal point
 *	<BR> DTL_DTK_STRING - text (no digits or punctuation)
 *	<BR> DTL_DTK_SPECIAL - leading "+" or "-" followed by text
 *	<BR> DTL_DTK_TZ - leading "+" or "-" followed by digits (also eats ':', '.', '-')
 */
stlStatus dtdParseDateTime( const stlChar  * aTimeStr,
                            stlInt64         aTimeStrLength,
                            dtlDataType      aDataType,
                            stlChar        * aBuffer,
                            stlSize          aBufferLen,
                            stlChar       ** aField,
                            stlInt32       * aFieldType,
                            stlInt32         aMaxFields,
                            stlInt32       * aNumFields,
                            stlErrorStack  * aErrorStack )
{
    stlInt32        sCurrentField;
    const stlChar * sTimeStr;
    const stlChar * sTimeStrEnd;
    stlChar       * sBuffer;
    const stlChar * sBufferEnd;
    stlChar         sDelimit;
    stlBool         sIsDate;
    stlBool         sIsSkip = STL_FALSE;
    stlInt32        sValue;
    stlInt32        sValueType;
    stlBool         sIsJulian;
        
    sCurrentField = 0;
    sTimeStr = aTimeStr;
    sTimeStrEnd = aTimeStr + aTimeStrLength;
    sBuffer = aBuffer;
    sBufferEnd = aBuffer + aBufferLen;
    
    /* field간의 공백 무시 */
    while( stlIsspace(*sTimeStr) == STL_TRUE )
    {
        sTimeStr++;
    }

    while( sTimeStr < sTimeStrEnd )
    {
        /* current field의 string 저장 */
        
        STL_TRY_THROW( sCurrentField < aMaxFields, RAMP_ERROR_BAD_FORMAT );

        aField[sCurrentField] = sBuffer;

        /*
         * leading digit이면 date or time
         */
        if( stlIsdigit(*sTimeStr) == STL_TRUE )
        {
            aFieldType[sCurrentField] = DTL_DTK_NUMBER;
            
            DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
            sTimeStr++;
            STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

            while( stlIsdigit(*sTimeStr) == STL_TRUE )
            {
                DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                sTimeStr++;                
                STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
            }

            /* trunc space */
            while( stlIsspace(*sTimeStr) == STL_TRUE )
            {
                sTimeStr++;
                STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
            }
            
            if( *sTimeStr == ':' )
            {
                /*
                 * time field
                 */
                aFieldType[sCurrentField] = DTL_DTK_TIME;
                DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                sTimeStr++;
                STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                
                /* trunc space */
                while( stlIsspace(*sTimeStr) == STL_TRUE )
                {
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }

                while( (stlIsdigit(*sTimeStr) == STL_TRUE) ||
                       (*sTimeStr == '.') )
                {
                    DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }

                /* trunc space */
                while(stlIsspace(*sTimeStr) == STL_TRUE )
                {
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }

                if( *sTimeStr == ':' )
                {
                    DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

                    /* trunc space */
                    while(stlIsspace(*sTimeStr) == STL_TRUE )
                    {
                        sTimeStr++;
                        STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                    }

                    while( (stlIsdigit(*sTimeStr) == STL_TRUE) ||
                           (*sTimeStr == '.') )
                    {
                        DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                        sTimeStr++;
                        STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                    }
                }
                else
                {
                    /* Do Nothing */
                }
                
                /* trunc space */
                while( stlIsspace(*sTimeStr) == STL_TRUE )
                {
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }
            }
            else if( (*sTimeStr == '-') || (*sTimeStr == '/') || (*sTimeStr == '.') )
            {
                /*
                 * date field
                 * allow embedded text month
                 * 
                 * ex) '-'
                 *     1999-01-08  (ISO 8601; January 8 in any mode (recommended format))
                 *     1999-Jan-08 (January 8 in any mode)
                 * ex) '/'
                 *     1/8/1999	   (January 8 in MDY mode; August 1 in DMY mode)
                 *     08-Jan-1999 (January 8, except error in YMD mode) ???
                 * ex) '.'
                 *     1999.008 (year and day of year) 
                 */
                sDelimit = *sTimeStr;
                sIsSkip = (*sTimeStr == '-') || (*sTimeStr == '/');

                DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                sTimeStr++;
                STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

                /* trunc space */
                while( ( sIsSkip == STL_TRUE ) && ( stlIsspace(*sTimeStr) == STL_TRUE ) )
                {
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }

                if( sTimeStr == sTimeStrEnd )
                {
                    aFieldType[sCurrentField] = ((sDelimit == '.') ? DTL_DTK_NUMBER : DTL_DTK_DATE);
                    STL_THROW( RAMP_READ_FIELD );
                }            

                if( stlIsdigit(*sTimeStr) == STL_TRUE )
                {
                    /* second filed is all digit? then no embedded text month
                     * ex) 1999-01-08, 1/8/1999 */
                    
                    aFieldType[sCurrentField] = ((sDelimit == '.') ? DTL_DTK_NUMBER : DTL_DTK_DATE);

                    while( stlIsdigit(*sTimeStr) == STL_TRUE )
                    {
                        DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                        sTimeStr++;
                        STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                    }

                    /* trunc space */
                    while( ( sIsSkip == STL_TRUE ) && ( stlIsspace(*sTimeStr) == STL_TRUE ) )
                    {
                        sTimeStr++;
                        STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                    }

                    /* delimit를 비교해서 date의 three-field date를 얻는다. */
                    if( *sTimeStr == sDelimit )
                    {
                        aFieldType[sCurrentField] = DTL_DTK_DATE;
                        DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                        sTimeStr++;
                        STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

                        /* trunc space */
                        while( ( sIsSkip == STL_TRUE ) && ( stlIsspace(*sTimeStr) == STL_TRUE ) )
                        {
                            sTimeStr++;
                            STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                        }

                        while( (stlIsdigit(*sTimeStr) == STL_TRUE) ||
                               (*sTimeStr == sDelimit) )
                        {
                            DTD_APPEND_CHAR(sBuffer, sBufferEnd, *sTimeStr );
                            sTimeStr++;
                            STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                        }

                        /* trunc space */
                        while( ( sIsSkip == STL_TRUE ) && ( stlIsspace(*sTimeStr) == STL_TRUE ) )
                        {
                            sTimeStr++;
                            STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                        }
                    }
                    else
                    {
                        // Do Nothing
                    }
                }
                else
                {
                    /* ex) 1999-Jan-08 */
                    
                    aFieldType[sCurrentField] = DTL_DTK_DATE;

                    while( (stlIsalnum(*sTimeStr) == STL_TRUE) ||
                           (*sTimeStr == sDelimit) )
                    {
                        /**
                         * @todo DTD_APPEND_CHAR( .., .., stlTolower(*sTimeStr))
                         *       의 stlTolower적용 확인해야 함( multi-byte character set 관련 )
                         *       postgres에서는 pg_tolower()함수를 내부적으로 생성.적용.
                         * 
                         * 
                         */
                        DTD_APPEND_CHAR( sBuffer, sBufferEnd, stlTolower(*sTimeStr) );
                        sTimeStr++;
                        STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                    }

                    /* trunc space */
                    while( stlIsspace(*sTimeStr) == STL_TRUE )
                    {
                        sTimeStr++;
                        STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                    }
                }
            }
            else
            {
                /* ex) 19990108, 990108
                 * 연결된 field들을 이후에 결정한다. */
                
                aFieldType[sCurrentField] = DTL_DTK_NUMBER;
            }
        }
        else if( *sTimeStr == '.' )
        {
            /*
             * leading decimal point이면, fractional seconds
             */
            aFieldType[sCurrentField] = DTL_DTK_NUMBER;
            
            DTD_APPEND_CHAR(sBuffer, sBufferEnd, *sTimeStr );
            sTimeStr++;
            STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

            while( stlIsdigit(*sTimeStr) == STL_TRUE )
            {
                DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                sTimeStr++;
                STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
            }

            /* trunc space */
            while( stlIsspace(*sTimeStr) == STL_TRUE )
            {
                sTimeStr++;
                STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
            }
        }
        else if( stlIsalpha(*sTimeStr) == STL_TRUE )
        {
            /*
             * leading text이면,
             * date string, month, day of week, special, or timezone
             */

            /**
             * @todo DTD_APPEND_CHAR( .., .., stlTolower(*sTimeStr))
             *       의 stlTolower적용 확인해야 함( multi-byte character set 관련 )
             *       postgres에서는 pg_tolower()함수를 내부적으로 생성.적용.
             */
            aFieldType[sCurrentField] = DTL_DTK_STRING;

            sIsJulian = ( *sTimeStr == 'J' ) || (*sTimeStr == 'j' );
            
            DTD_APPEND_CHAR( sBuffer, sBufferEnd, stlTolower(*sTimeStr) );
            sTimeStr++;
            STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

            while( stlIsalpha(*sTimeStr) == STL_TRUE )
            {
                DTD_APPEND_CHAR( sBuffer, sBufferEnd, stlTolower(*sTimeStr) );
                sTimeStr++;
                STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                sIsJulian = STL_FALSE;
            }

            /* date는     '-', '/', '.'를 포함할 수 있고,
             * timezone도 '/', '+', '-', '_', ':'를
             * 포함('_',':'는 first punctuation이 될수없다) 할수 있다.
             * date인지 timezone인지를 구별할 수 있어야 한다.
             */
            
            /* trunc space */
            while( ( sIsJulian == STL_FALSE ) && ( stlIsspace(*sTimeStr) == STL_TRUE ) )
            {
                sTimeStr++;
                STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
            }

            sIsDate = STL_FALSE;
            sIsSkip = (*sTimeStr == '-') || (*sTimeStr == '/') || (*sTimeStr == '+');

            if( (*sTimeStr == '/') || (*sTimeStr == '.') )
            {
                sIsDate  = STL_TRUE;
            }
            else if( (*sTimeStr == '-') || (*sTimeStr == '+') || (stlIsdigit(*sTimeStr) == STL_TRUE) )
            {
                /* null-terminate current field value */
                *sBuffer = '\0';
                
                /* non-timezone keyword인지를 확인 */
                if( dtdDateBinarySearch( aField[sCurrentField],
                                         dtlDateTokenTbl,
                                         dtlDateTokenTblSize )
                    == NULL )
                {
                    sIsDate = STL_TRUE;
                }
                else
                {
                    STL_TRY( dtdDecodeSpecial( sCurrentField,
                                               aField[sCurrentField],
                                               & sValue,
                                               & sValueType,
                                               aErrorStack )
                             == STL_SUCCESS );

                    switch( sValueType )
                    {
                        case DTL_RESERV:
                        case DTL_DTZMOD:
                        case DTL_DTZ:
                        case DTL_TZ:
                        case DTL_AMPM:
                        case DTL_ADBC:
                        case DTL_UNITS:
                        case DTL_ISOTIME:
                        case DTL_UNKNOWN_FIELD:
                            {
                                STL_THROW( RAMP_READ_FIELD );
                            }
                        default:
                            {
                                break;
                            }
                    }
                    
                    if(*sTimeStr == '-')
                    {
                        sIsDate = STL_TRUE;
                    }
                    else
                    {
                        /* Do Nothing */
                    }   
                }
            }
            else
            {
                // Do Nothing
            }

            if( sIsDate == STL_TRUE )
            {
                aFieldType[sCurrentField] = DTL_DTK_DATE;

                /**
                 * @todo DTD_APPEND_CHAR( .., .., stlTolower(*sTimeStr))
                 *       의 stlTolower적용 확인해야 함( multi-byte character set 관련 )
                 *       postgres에서는 pg_tolower()함수를 내부적으로 생성.적용.
                 */
                DTD_APPEND_CHAR( sBuffer, sBufferEnd, stlTolower(*sTimeStr) );
                sTimeStr++;
                STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

                /* trunc space */
                while( ( sIsSkip == STL_TRUE ) && ( stlIsspace(*sTimeStr) == STL_TRUE ) )
                {
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }

                sIsSkip = STL_TRUE;
                
                while( (*sTimeStr == '+') || (*sTimeStr == '-') ||
                       (*sTimeStr == '/') || (*sTimeStr == '_') ||
                       (*sTimeStr == '.') || (*sTimeStr == ':') ||
                       (stlIsalnum(*sTimeStr) == STL_TRUE) )
                {
                    /**
                     * @todo DTD_APPEND_CHAR( .., .., stlTolower(*sTimeStr))
                     *       의 stlTolower적용 확인해야 함( multi-byte character set 관련 )
                     *       postgres에서는 pg_tolower()함수를 내부적으로 생성.적용.
                     */
                    DTD_APPEND_CHAR( sBuffer, sBufferEnd, stlTolower(*sTimeStr) );
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

                    sIsSkip = !(*sTimeStr == '.');
                }

                /* trunc space */
                while( ( sIsSkip == STL_TRUE ) && ( stlIsspace(*sTimeStr) == STL_TRUE ) )
                {
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }

                sIsSkip = STL_TRUE;
                
                while( (*sTimeStr == '+') || (*sTimeStr == '-') ||
                       (*sTimeStr == '/') || (*sTimeStr == '_') ||
                       (*sTimeStr == '.') || (*sTimeStr == ':') ||
                       (stlIsalnum(*sTimeStr) == STL_TRUE) )
                {
                    /**
                     * @todo DTD_APPEND_CHAR( .., .., stlTolower(*sTimeStr))
                     *       의 stlTolower적용 확인해야 함( multi-byte character set 관련 )
                     *       postgres에서는 pg_tolower()함수를 내부적으로 생성.적용.
                     */
                    DTD_APPEND_CHAR( sBuffer, sBufferEnd, stlTolower(*sTimeStr) );
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

                    sIsSkip = !(*sTimeStr == '.');
                }
            }
            else
            {
                // Do Nothing
            }
        }
        else if( (*sTimeStr == '+') || (*sTimeStr == '-') )
        {
            /*
             * leading sign이면, special or numeric timezone
             */

            aFieldType[sCurrentField] = DTL_DTK_TZ;

            DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
            sTimeStr++;
            STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

            while( stlIsspace(*sTimeStr) == STL_TRUE )
            {
                sTimeStr++;
                STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
            }

            if( stlIsdigit(*sTimeStr) == STL_TRUE )
            {
                /*
                 * numeric timezone
                 * 
                 * DTL_DTK_TZ - leading "+" or "-" followed by digits (also eats ':', '.', '-')
                 * DTL_DTK_TZ는 signed float 또는 yyyy-mm 도 가능
                 */
                aFieldType[sCurrentField] = DTL_DTK_TZ;

                while( ( stlIsdigit(*sTimeStr) == STL_TRUE ) ||
                       (*sTimeStr == '.') )
                {
                    DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }

                /* trunc space */
                while( stlIsspace(*sTimeStr) == STL_TRUE )
                {
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }

                if( (*sTimeStr == ':') || (*sTimeStr == '.') || (*sTimeStr == '-') )
                {
                    sIsSkip = !(*sTimeStr == '.');
                    
                    DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

                    /* trunc space */
                    while( ( sIsSkip == STL_TRUE ) && ( stlIsspace(*sTimeStr) == STL_TRUE ) )
                    {
                        sTimeStr++;
                        STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                    }

                    while( ( stlIsdigit(*sTimeStr) == STL_TRUE ) ||
                           (*sTimeStr == '.') )
                    {
                        DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                        sTimeStr++;
                        STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                    }
                }

                /* trunc space */
                while( stlIsspace(*sTimeStr) == STL_TRUE )
                {
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }

                if( (*sTimeStr == ':') || (*sTimeStr == '.') || (*sTimeStr == '-') )
                {
                    sIsSkip = !(*sTimeStr == '.');
                    
                    DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

                    /* trunc space */
                    while( ( sIsSkip == STL_TRUE ) && ( stlIsspace(*sTimeStr) == STL_TRUE ) )
                    {
                        sTimeStr++;
                        STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                    }

                    while( ( stlIsdigit(*sTimeStr) == STL_TRUE ) ||
                           (*sTimeStr == '.') )
                    {
                        DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                        sTimeStr++;
                        STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                    }
                }

                /* trunc space */
                while( ( sIsSkip == STL_TRUE ) && ( stlIsspace(*sTimeStr) == STL_TRUE ) )
                {
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }
            }
            else if( stlIsalpha(*sTimeStr) == STL_TRUE )
            {
                /*
                 * special
                 */
                aFieldType[sCurrentField] = DTL_DTK_SPECIAL;

                /**
                 * @todo DTD_APPEND_CHAR( .., .., stlTolower(*sTimeStr))
                 *       의 stlTolower적용 확인해야 함( multi-byte character set 관련 )
                 *       postgres에서는 pg_tolower()함수를 내부적으로 생성.적용.
                 */
                DTD_APPEND_CHAR( sBuffer, sBufferEnd, stlTolower(*sTimeStr) );
                sTimeStr++;
                STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

                while( stlIsalpha(*sTimeStr) == STL_TRUE )
                {
                    /**
                     * @todo DTD_APPEND_CHAR( .., .., stlTolower(*sTimeStr))
                     *       의 stlTolower적용 확인해야 함( multi-byte character set 관련 )
                     *       postgres에서는 pg_tolower()함수를 내부적으로 생성.적용.
                     */
                    DTD_APPEND_CHAR( sBuffer, sBufferEnd, stlTolower(*sTimeStr) );
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }

                while( stlIsspace(*sTimeStr) == STL_TRUE )
                {
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }
            }
            else
            {
                STL_THROW( RAMP_ERROR_BAD_FORMAT );
            }
        }
        else if( stlIspunct(*sTimeStr) == STL_TRUE )
        {
            /*
             * 이외의 기호는 무시. but 구분자로 사용.
             */ 
            sTimeStr++;
            continue;
        }
        else
        {
            /*
             * invalid
             */ 
            STL_THROW( RAMP_ERROR_BAD_FORMAT );
        }

        STL_RAMP( RAMP_READ_FIELD );

        *sBuffer = '\0';

        sBuffer++;
        sCurrentField++;
    }

    *aNumFields = sCurrentField;
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERROR_BAD_FORMAT )
    {
        if( (aDataType == DTL_TYPE_INTERVAL_YEAR_TO_MONTH) ||
            (aDataType == DTL_TYPE_INTERVAL_DAY_TO_SECOND) )
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          DTL_ERRCODE_C_STRING_NOT_VALID_INTERVAL_LITERAL,
                          NULL,
                          aErrorStack );            
        }
        else
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          DTL_ERRCODE_C_STRING_NOT_VALID_DATE_OR_TIME_OR_TIMESTAMP_LITERAL,
                          NULL,
                          aErrorStack );
        }
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief utf16 input string을 date/time문법에 맞는 token 단위로 분류
 * @param[in]  aTimeStr          input date/time string
 * @param[in]  aTimeStrLength    aString의 길이 
 * @param[in]  aDataType         data type
 * @param[in]  aBuffer           field string 저장을 위한 작업공간
 * @param[in]  aBufferLen        aBuffer의 size
 * @param[out] aField            field string의 pointer를 저장하는 배열 
 * @param[out] aFieldType        field type들의 배열 
 * @param[in]  aMaxFields        aField, aFieldType의 크기
 * @param[out] aNumFields        aField, aFieldType에 저장된 실제 수 
 * @param[out] aErrorStack       에러 스택
 * @remark
 *   Several field types are assigned:
 *	<BR> DTL_DTK_NUMBER - digits and (possibly) a decimal point
 *	<BR> DTL_DTK_DATE - digits and two delimiters, or digits and text
 *	<BR> DTL_DTK_TIME - digits, colon delimiters, and possibly a decimal point
 *	<BR> DTL_DTK_STRING - text (no digits or punctuation)
 *	<BR> DTL_DTK_SPECIAL - leading "+" or "-" followed by text
 *	<BR> DTL_DTK_TZ - leading "+" or "-" followed by digits (also eats ':', '.', '-')
 */
stlStatus dtdParseDateTimeFromUtf16WChar( const stlUInt16    * aTimeStr,
                                          stlInt64             aTimeStrLength,
                                          dtlDataType          aDataType,
                                          stlChar            * aBuffer,
                                          stlSize              aBufferLen,
                                          stlChar           ** aField,
                                          stlInt32           * aFieldType,
                                          stlInt32             aMaxFields,
                                          stlInt32           * aNumFields,
                                          stlErrorStack      * aErrorStack )
{
    stlInt32          sCurrentField;
    const stlUInt16 * sTimeStr;
    const stlUInt16 * sTimeStrEnd;
    stlChar         * sBuffer;
    const stlChar   * sBufferEnd;
    stlChar           sDelimit;
    stlBool           sIsDate;
    stlBool           sIsSkip = STL_FALSE;
    stlInt32          sValue;
    stlInt32          sValueType;
    stlBool           sIsJulian;
        
    sCurrentField = 0;
    sTimeStr = aTimeStr;
    sTimeStrEnd = aTimeStr + (aTimeStrLength / STL_UINT16_SIZE);
    sBuffer = aBuffer;
    sBufferEnd = aBuffer + aBufferLen;

    /*
     * utf16 string check
     */

    while( sTimeStr < sTimeStrEnd )
    {
        if( *sTimeStr < 0x80 )
        {
            /* Do Nothing */
        }
        else
        {
            STL_THROW( RAMP_ERROR_BAD_FORMAT );
        }

        sTimeStr++;
    }

    sTimeStr = aTimeStr;
    
    /* field간의 공백 무시 */
    while( stlIsspace(*sTimeStr) == STL_TRUE )
    {
        sTimeStr++;
    }

    while( sTimeStr < sTimeStrEnd )
    {
        /* current field의 string 저장 */
        
        STL_TRY_THROW( sCurrentField < aMaxFields, RAMP_ERROR_BAD_FORMAT );

        aField[sCurrentField] = sBuffer;

        /*
         * leading digit이면 date or time
         */
        if( stlIsdigit(*sTimeStr) == STL_TRUE )
        {
            aFieldType[sCurrentField] = DTL_DTK_NUMBER;
            
            DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
            sTimeStr++;
            STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

            while( stlIsdigit(*sTimeStr) == STL_TRUE )
            {
                DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                sTimeStr++;                
                STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
            }

            /* trunc space */
            while( stlIsspace(*sTimeStr) == STL_TRUE )
            {
                sTimeStr++;
                STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
            }
            
            if( *sTimeStr == ':' )
            {
                /*
                 * time field
                 */
                aFieldType[sCurrentField] = DTL_DTK_TIME;
                DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                sTimeStr++;
                STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                
                /* trunc space */
                while( stlIsspace(*sTimeStr) == STL_TRUE )
                {
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }

                while( (stlIsdigit(*sTimeStr) == STL_TRUE) ||
                       (*sTimeStr == '.') )
                {
                    DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }

                /* trunc space */
                while(stlIsspace(*sTimeStr) == STL_TRUE )
                {
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }

                if( *sTimeStr == ':' )
                {
                    DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

                    /* trunc space */
                    while(stlIsspace(*sTimeStr) == STL_TRUE )
                    {
                        sTimeStr++;
                        STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                    }

                    while( (stlIsdigit(*sTimeStr) == STL_TRUE) ||
                           (*sTimeStr == '.') )
                    {
                        DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                        sTimeStr++;
                        STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                    }
                }
                else
                {
                    /* Do Nothing */
                }
                
                /* trunc space */
                while( stlIsspace(*sTimeStr) == STL_TRUE )
                {
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }
            }
            else if( (*sTimeStr == '-') || (*sTimeStr == '/') || (*sTimeStr == '.') )
            {
                /*
                 * date field
                 * allow embedded text month
                 * 
                 * ex) '-'
                 *     1999-01-08  (ISO 8601; January 8 in any mode (recommended format))
                 *     1999-Jan-08 (January 8 in any mode)
                 * ex) '/'
                 *     1/8/1999	   (January 8 in MDY mode; August 1 in DMY mode)
                 *     08-Jan-1999 (January 8, except error in YMD mode) ???
                 * ex) '.'
                 *     1999.008 (year and day of year) 
                 */
                sDelimit = *sTimeStr;
                sIsSkip = (*sTimeStr == '-') || (*sTimeStr == '/');

                DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                sTimeStr++;
                STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

                /* trunc space */
                while( ( sIsSkip == STL_TRUE ) && ( stlIsspace(*sTimeStr) == STL_TRUE ) )
                {
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }

                if( sTimeStr == sTimeStrEnd )
                {
                    aFieldType[sCurrentField] = ((sDelimit == '.') ? DTL_DTK_NUMBER : DTL_DTK_DATE);
                    STL_THROW( RAMP_READ_FIELD );
                }            

                if( stlIsdigit(*sTimeStr) == STL_TRUE )
                {
                    /* second filed is all digit? then no embedded text month
                     * ex) 1999-01-08, 1/8/1999 */
                    
                    aFieldType[sCurrentField] = ((sDelimit == '.') ? DTL_DTK_NUMBER : DTL_DTK_DATE);

                    while( stlIsdigit(*sTimeStr) == STL_TRUE )
                    {
                        DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                        sTimeStr++;
                        STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                    }

                    /* trunc space */
                    while( ( sIsSkip == STL_TRUE ) && ( stlIsspace(*sTimeStr) == STL_TRUE ) )
                    {
                        sTimeStr++;
                        STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                    }

                    /* delimit를 비교해서 date의 three-field date를 얻는다. */
                    if( *sTimeStr == sDelimit )
                    {
                        aFieldType[sCurrentField] = DTL_DTK_DATE;
                        DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                        sTimeStr++;
                        STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

                        /* trunc space */
                        while( ( sIsSkip == STL_TRUE ) && ( stlIsspace(*sTimeStr) == STL_TRUE ) )
                        {
                            sTimeStr++;
                            STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                        }

                        while( (stlIsdigit(*sTimeStr) == STL_TRUE) ||
                               (*sTimeStr == sDelimit) )
                        {
                            DTD_APPEND_CHAR(sBuffer, sBufferEnd, *sTimeStr );
                            sTimeStr++;
                            STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                        }

                        /* trunc space */
                        while( ( sIsSkip == STL_TRUE ) && ( stlIsspace(*sTimeStr) == STL_TRUE ) )
                        {
                            sTimeStr++;
                            STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                        }
                    }
                    else
                    {
                        // Do Nothing
                    }
                }
                else
                {
                    /* ex) 1999-Jan-08 */
                    
                    aFieldType[sCurrentField] = DTL_DTK_DATE;

                    while( (stlIsalnum(*sTimeStr) == STL_TRUE) ||
                           (*sTimeStr == sDelimit) )
                    {
                        /**
                         * @todo DTD_APPEND_CHAR( .., .., stlTolower(*sTimeStr))
                         *       의 stlTolower적용 확인해야 함( multi-byte character set 관련 )
                         *       postgres에서는 pg_tolower()함수를 내부적으로 생성.적용.
                         * 
                         * 
                         */
                        DTD_APPEND_CHAR( sBuffer, sBufferEnd, stlTolower(*sTimeStr) );
                        sTimeStr++;
                        STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                    }

                    /* trunc space */
                    while( stlIsspace(*sTimeStr) == STL_TRUE )
                    {
                        sTimeStr++;
                        STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                    }
                }
            }
            else
            {
                /* ex) 19990108, 990108
                 * 연결된 field들을 이후에 결정한다. */
                
                aFieldType[sCurrentField] = DTL_DTK_NUMBER;
            }
        }
        else if( *sTimeStr == '.' )
        {
            /*
             * leading decimal point이면, fractional seconds
             */
            aFieldType[sCurrentField] = DTL_DTK_NUMBER;
            
            DTD_APPEND_CHAR(sBuffer, sBufferEnd, *sTimeStr );
            sTimeStr++;
            STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

            while( stlIsdigit(*sTimeStr) == STL_TRUE )
            {
                DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                sTimeStr++;
                STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
            }

            /* trunc space */
            while( stlIsspace(*sTimeStr) == STL_TRUE )
            {
                sTimeStr++;
                STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
            }
        }
        else if( stlIsalpha(*sTimeStr) == STL_TRUE )
        {
            /*
             * leading text이면,
             * date string, month, day of week, special, or timezone
             */

            /**
             * @todo DTD_APPEND_CHAR( .., .., stlTolower(*sTimeStr))
             *       의 stlTolower적용 확인해야 함( multi-byte character set 관련 )
             *       postgres에서는 pg_tolower()함수를 내부적으로 생성.적용.
             */
            aFieldType[sCurrentField] = DTL_DTK_STRING;

            sIsJulian = ( *sTimeStr == 'J' ) || (*sTimeStr == 'j' );
            
            DTD_APPEND_CHAR( sBuffer, sBufferEnd, stlTolower(*sTimeStr) );
            sTimeStr++;
            STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

            while( stlIsalpha(*sTimeStr) == STL_TRUE )
            {
                DTD_APPEND_CHAR( sBuffer, sBufferEnd, stlTolower(*sTimeStr) );
                sTimeStr++;
                STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                sIsJulian = STL_FALSE;
            }

            /* date는     '-', '/', '.'를 포함할 수 있고,
             * timezone도 '/', '+', '-', '_', ':'를
             * 포함('_',':'는 first punctuation이 될수없다) 할수 있다.
             * date인지 timezone인지를 구별할 수 있어야 한다.
             */
            
            /* trunc space */
            while( ( sIsJulian == STL_FALSE ) && ( stlIsspace(*sTimeStr) == STL_TRUE ) )
            {
                sTimeStr++;
                STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
            }

            sIsDate = STL_FALSE;
            sIsSkip = (*sTimeStr == '-') || (*sTimeStr == '/') || (*sTimeStr == '+');

            if( (*sTimeStr == '/') || (*sTimeStr == '.') )
            {
                sIsDate  = STL_TRUE;
            }
            else if( (*sTimeStr == '-') || (*sTimeStr == '+') || (stlIsdigit(*sTimeStr) == STL_TRUE) )
            {
                /* null-terminate current field value */
                *sBuffer = '\0';
                
                /* non-timezone keyword인지를 확인 */
                if( dtdDateBinarySearch( aField[sCurrentField],
                                         dtlDateTokenTbl,
                                         dtlDateTokenTblSize )
                    == NULL )
                {
                    sIsDate = STL_TRUE;
                }
                else
                {
                    STL_TRY( dtdDecodeSpecial( sCurrentField,
                                               aField[sCurrentField],
                                               & sValue,
                                               & sValueType,
                                               aErrorStack )
                             == STL_SUCCESS );

                    switch( sValueType )
                    {
                        case DTL_RESERV:
                        case DTL_DTZMOD:
                        case DTL_DTZ:
                        case DTL_TZ:
                        case DTL_AMPM:
                        case DTL_ADBC:
                        case DTL_UNITS:
                        case DTL_ISOTIME:
                        case DTL_UNKNOWN_FIELD:
                            {
                                STL_THROW( RAMP_READ_FIELD );
                            }
                        default:
                            {
                                break;
                            }
                    }
                    
                    if(*sTimeStr == '-')
                    {
                        sIsDate = STL_TRUE;
                    }
                    else
                    {
                        /* Do Nothing */
                    }   
                }
            }
            else
            {
                // Do Nothing
            }

            if( sIsDate == STL_TRUE )
            {
                aFieldType[sCurrentField] = DTL_DTK_DATE;

                /**
                 * @todo DTD_APPEND_CHAR( .., .., stlTolower(*sTimeStr))
                 *       의 stlTolower적용 확인해야 함( multi-byte character set 관련 )
                 *       postgres에서는 pg_tolower()함수를 내부적으로 생성.적용.
                 */
                DTD_APPEND_CHAR( sBuffer, sBufferEnd, stlTolower(*sTimeStr) );
                sTimeStr++;
                STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

                /* trunc space */
                while( ( sIsSkip == STL_TRUE ) && ( stlIsspace(*sTimeStr) == STL_TRUE ) )
                {
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }

                sIsSkip = STL_TRUE;
                
                while( (*sTimeStr == '+') || (*sTimeStr == '-') ||
                       (*sTimeStr == '/') || (*sTimeStr == '_') ||
                       (*sTimeStr == '.') || (*sTimeStr == ':') ||
                       (stlIsalnum(*sTimeStr) == STL_TRUE) )
                {
                    /**
                     * @todo DTD_APPEND_CHAR( .., .., stlTolower(*sTimeStr))
                     *       의 stlTolower적용 확인해야 함( multi-byte character set 관련 )
                     *       postgres에서는 pg_tolower()함수를 내부적으로 생성.적용.
                     */
                    DTD_APPEND_CHAR( sBuffer, sBufferEnd, stlTolower(*sTimeStr) );
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

                    sIsSkip = !(*sTimeStr == '.');
                }

                /* trunc space */
                while( ( sIsSkip == STL_TRUE ) && ( stlIsspace(*sTimeStr) == STL_TRUE ) )
                {
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }

                sIsSkip = STL_TRUE;
                
                while( (*sTimeStr == '+') || (*sTimeStr == '-') ||
                       (*sTimeStr == '/') || (*sTimeStr == '_') ||
                       (*sTimeStr == '.') || (*sTimeStr == ':') ||
                       (stlIsalnum(*sTimeStr) == STL_TRUE) )
                {
                    /**
                     * @todo DTD_APPEND_CHAR( .., .., stlTolower(*sTimeStr))
                     *       의 stlTolower적용 확인해야 함( multi-byte character set 관련 )
                     *       postgres에서는 pg_tolower()함수를 내부적으로 생성.적용.
                     */
                    DTD_APPEND_CHAR( sBuffer, sBufferEnd, stlTolower(*sTimeStr) );
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

                    sIsSkip = !(*sTimeStr == '.');
                }
            }
            else
            {
                // Do Nothing
            }
        }
        else if( (*sTimeStr == '+') || (*sTimeStr == '-') )
        {
            /*
             * leading sign이면, special or numeric timezone
             */

            aFieldType[sCurrentField] = DTL_DTK_TZ;

            DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
            sTimeStr++;
            STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

            while( stlIsspace(*sTimeStr) == STL_TRUE )
            {
                sTimeStr++;
                STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
            }

            if( stlIsdigit(*sTimeStr) == STL_TRUE )
            {
                /*
                 * numeric timezone
                 * 
                 * DTL_DTK_TZ - leading "+" or "-" followed by digits (also eats ':', '.', '-')
                 * DTL_DTK_TZ는 signed float 또는 yyyy-mm 도 가능
                 */
                aFieldType[sCurrentField] = DTL_DTK_TZ;

                while( ( stlIsdigit(*sTimeStr) == STL_TRUE ) ||
                       (*sTimeStr == '.') )
                {
                    DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }

                /* trunc space */
                while( stlIsspace(*sTimeStr) == STL_TRUE )
                {
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }

                if( (*sTimeStr == ':') || (*sTimeStr == '.') || (*sTimeStr == '-') )
                {
                    sIsSkip = !(*sTimeStr == '.');
                    
                    DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

                    /* trunc space */
                    while( ( sIsSkip == STL_TRUE ) && ( stlIsspace(*sTimeStr) == STL_TRUE ) )
                    {
                        sTimeStr++;
                        STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                    }

                    while( ( stlIsdigit(*sTimeStr) == STL_TRUE ) ||
                           (*sTimeStr == '.') )
                    {
                        DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                        sTimeStr++;
                        STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                    }
                }

                /* trunc space */
                while( stlIsspace(*sTimeStr) == STL_TRUE )
                {
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }

                if( (*sTimeStr == ':') || (*sTimeStr == '.') || (*sTimeStr == '-') )
                {
                    sIsSkip = !(*sTimeStr == '.');
                    
                    DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

                    /* trunc space */
                    while( ( sIsSkip == STL_TRUE ) && ( stlIsspace(*sTimeStr) == STL_TRUE ) )
                    {
                        sTimeStr++;
                        STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                    }

                    while( ( stlIsdigit(*sTimeStr) == STL_TRUE ) ||
                           (*sTimeStr == '.') )
                    {
                        DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                        sTimeStr++;
                        STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                    }
                }

                /* trunc space */
                while( ( sIsSkip == STL_TRUE ) && ( stlIsspace(*sTimeStr) == STL_TRUE ) )
                {
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }
            }
            else if( stlIsalpha(*sTimeStr) == STL_TRUE )
            {
                /*
                 * special
                 */
                aFieldType[sCurrentField] = DTL_DTK_SPECIAL;

                /**
                 * @todo DTD_APPEND_CHAR( .., .., stlTolower(*sTimeStr))
                 *       의 stlTolower적용 확인해야 함( multi-byte character set 관련 )
                 *       postgres에서는 pg_tolower()함수를 내부적으로 생성.적용.
                 */
                DTD_APPEND_CHAR( sBuffer, sBufferEnd, stlTolower(*sTimeStr) );
                sTimeStr++;
                STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

                while( stlIsalpha(*sTimeStr) == STL_TRUE )
                {
                    /**
                     * @todo DTD_APPEND_CHAR( .., .., stlTolower(*sTimeStr))
                     *       의 stlTolower적용 확인해야 함( multi-byte character set 관련 )
                     *       postgres에서는 pg_tolower()함수를 내부적으로 생성.적용.
                     */
                    DTD_APPEND_CHAR( sBuffer, sBufferEnd, stlTolower(*sTimeStr) );
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }

                while( stlIsspace(*sTimeStr) == STL_TRUE )
                {
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }
            }
            else
            {
                STL_THROW( RAMP_ERROR_BAD_FORMAT );
            }
        }
        else if( stlIspunct(*sTimeStr) == STL_TRUE )
        {
            /*
             * 이외의 기호는 무시. but 구분자로 사용.
             */ 
            sTimeStr++;
            continue;
        }
        else
        {
            /*
             * invalid
             */ 
            STL_THROW( RAMP_ERROR_BAD_FORMAT );
        }

        STL_RAMP( RAMP_READ_FIELD );

        *sBuffer = '\0';

        sBuffer++;
        sCurrentField++;
    }

    *aNumFields = sCurrentField;
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERROR_BAD_FORMAT )
    {
        if( (aDataType == DTL_TYPE_INTERVAL_YEAR_TO_MONTH) ||
            (aDataType == DTL_TYPE_INTERVAL_DAY_TO_SECOND) )
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          DTL_ERRCODE_C_STRING_NOT_VALID_INTERVAL_LITERAL,
                          NULL,
                          aErrorStack );            
        }
        else
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          DTL_ERRCODE_C_STRING_NOT_VALID_DATE_OR_TIME_OR_TIMESTAMP_LITERAL,
                          NULL,
                          aErrorStack );
        }
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief utf32 input string을 date/time문법에 맞는 token 단위로 분류
 * @param[in]  aTimeStr          input date/time string
 * @param[in]  aTimeStrLength    aString의 길이 
 * @param[in]  aDataType         data type
 * @param[in]  aBuffer           field string 저장을 위한 작업공간
 * @param[in]  aBufferLen        aBuffer의 size
 * @param[out] aField            field string의 pointer를 저장하는 배열 
 * @param[out] aFieldType        field type들의 배열 
 * @param[in]  aMaxFields        aField, aFieldType의 크기
 * @param[out] aNumFields        aField, aFieldType에 저장된 실제 수 
 * @param[out] aErrorStack       에러 스택
 * @remark
 *   Several field types are assigned:
 *	<BR> DTL_DTK_NUMBER - digits and (possibly) a decimal point
 *	<BR> DTL_DTK_DATE - digits and two delimiters, or digits and text
 *	<BR> DTL_DTK_TIME - digits, colon delimiters, and possibly a decimal point
 *	<BR> DTL_DTK_STRING - text (no digits or punctuation)
 *	<BR> DTL_DTK_SPECIAL - leading "+" or "-" followed by text
 *	<BR> DTL_DTK_TZ - leading "+" or "-" followed by digits (also eats ':', '.', '-')
 */
stlStatus dtdParseDateTimeFromUtf32WChar( const stlUInt32    * aTimeStr,
                                          stlInt64             aTimeStrLength,
                                          dtlDataType          aDataType,
                                          stlChar            * aBuffer,
                                          stlSize              aBufferLen,
                                          stlChar           ** aField,
                                          stlInt32           * aFieldType,
                                          stlInt32             aMaxFields,
                                          stlInt32           * aNumFields,
                                          stlErrorStack      * aErrorStack )
{
    stlInt32          sCurrentField;
    const stlUInt32 * sTimeStr;
    const stlUInt32 * sTimeStrEnd;
    stlChar         * sBuffer;
    const stlChar   * sBufferEnd;
    stlChar           sDelimit;
    stlBool           sIsDate;
    stlBool           sIsSkip = STL_FALSE;
    stlInt32          sValue;
    stlInt32          sValueType;
    stlBool           sIsJulian;
        
    sCurrentField = 0;
    sTimeStr = aTimeStr;
    sTimeStrEnd = aTimeStr + (aTimeStrLength / STL_UINT32_SIZE);
    sBuffer = aBuffer;
    sBufferEnd = aBuffer + aBufferLen;

    /*
     * utf16 string check
     */

    while( sTimeStr < sTimeStrEnd )
    {
        if( *sTimeStr < 0x80 )
        {
            /* Do Nothing */
        }
        else
        {
            STL_THROW( RAMP_ERROR_BAD_FORMAT );
        }

        sTimeStr++;
    }

    sTimeStr = aTimeStr;
    
    /* field간의 공백 무시 */
    while( stlIsspace(*sTimeStr) == STL_TRUE )
    {
        sTimeStr++;
    }

    while( sTimeStr < sTimeStrEnd )
    {
        /* current field의 string 저장 */
        
        STL_TRY_THROW( sCurrentField < aMaxFields, RAMP_ERROR_BAD_FORMAT );

        aField[sCurrentField] = sBuffer;

        /*
         * leading digit이면 date or time
         */
        if( stlIsdigit(*sTimeStr) == STL_TRUE )
        {
            aFieldType[sCurrentField] = DTL_DTK_NUMBER;
            
            DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
            sTimeStr++;
            STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

            while( stlIsdigit(*sTimeStr) == STL_TRUE )
            {
                DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                sTimeStr++;                
                STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
            }

            /* trunc space */
            while( stlIsspace(*sTimeStr) == STL_TRUE )
            {
                sTimeStr++;
                STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
            }
            
            if( *sTimeStr == ':' )
            {
                /*
                 * time field
                 */
                aFieldType[sCurrentField] = DTL_DTK_TIME;
                DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                sTimeStr++;
                STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                
                /* trunc space */
                while( stlIsspace(*sTimeStr) == STL_TRUE )
                {
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }

                while( (stlIsdigit(*sTimeStr) == STL_TRUE) ||
                       (*sTimeStr == '.') )
                {
                    DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }

                /* trunc space */
                while(stlIsspace(*sTimeStr) == STL_TRUE )
                {
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }

                if( *sTimeStr == ':' )
                {
                    DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

                    /* trunc space */
                    while(stlIsspace(*sTimeStr) == STL_TRUE )
                    {
                        sTimeStr++;
                        STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                    }

                    while( (stlIsdigit(*sTimeStr) == STL_TRUE) ||
                           (*sTimeStr == '.') )
                    {
                        DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                        sTimeStr++;
                        STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                    }
                }
                else
                {
                    /* Do Nothing */
                }
                
                /* trunc space */
                while( stlIsspace(*sTimeStr) == STL_TRUE )
                {
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }
            }
            else if( (*sTimeStr == '-') || (*sTimeStr == '/') || (*sTimeStr == '.') )
            {
                /*
                 * date field
                 * allow embedded text month
                 * 
                 * ex) '-'
                 *     1999-01-08  (ISO 8601; January 8 in any mode (recommended format))
                 *     1999-Jan-08 (January 8 in any mode)
                 * ex) '/'
                 *     1/8/1999	   (January 8 in MDY mode; August 1 in DMY mode)
                 *     08-Jan-1999 (January 8, except error in YMD mode) ???
                 * ex) '.'
                 *     1999.008 (year and day of year) 
                 */
                sDelimit = *sTimeStr;
                sIsSkip = (*sTimeStr == '-') || (*sTimeStr == '/');

                DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                sTimeStr++;
                STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

                /* trunc space */
                while( ( sIsSkip == STL_TRUE ) && ( stlIsspace(*sTimeStr) == STL_TRUE ) )
                {
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }

                if( sTimeStr == sTimeStrEnd )
                {
                    aFieldType[sCurrentField] = ((sDelimit == '.') ? DTL_DTK_NUMBER : DTL_DTK_DATE);
                    STL_THROW( RAMP_READ_FIELD );
                }            

                if( stlIsdigit(*sTimeStr) == STL_TRUE )
                {
                    /* second filed is all digit? then no embedded text month
                     * ex) 1999-01-08, 1/8/1999 */
                    
                    aFieldType[sCurrentField] = ((sDelimit == '.') ? DTL_DTK_NUMBER : DTL_DTK_DATE);

                    while( stlIsdigit(*sTimeStr) == STL_TRUE )
                    {
                        DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                        sTimeStr++;
                        STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                    }

                    /* trunc space */
                    while( ( sIsSkip == STL_TRUE ) && ( stlIsspace(*sTimeStr) == STL_TRUE ) )
                    {
                        sTimeStr++;
                        STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                    }

                    /* delimit를 비교해서 date의 three-field date를 얻는다. */
                    if( *sTimeStr == sDelimit )
                    {
                        aFieldType[sCurrentField] = DTL_DTK_DATE;
                        DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                        sTimeStr++;
                        STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

                        /* trunc space */
                        while( ( sIsSkip == STL_TRUE ) && ( stlIsspace(*sTimeStr) == STL_TRUE ) )
                        {
                            sTimeStr++;
                            STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                        }

                        while( (stlIsdigit(*sTimeStr) == STL_TRUE) ||
                               (*sTimeStr == sDelimit) )
                        {
                            DTD_APPEND_CHAR(sBuffer, sBufferEnd, *sTimeStr );
                            sTimeStr++;
                            STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                        }

                        /* trunc space */
                        while( ( sIsSkip == STL_TRUE ) && ( stlIsspace(*sTimeStr) == STL_TRUE ) )
                        {
                            sTimeStr++;
                            STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                        }
                    }
                    else
                    {
                        // Do Nothing
                    }
                }
                else
                {
                    /* ex) 1999-Jan-08 */
                    
                    aFieldType[sCurrentField] = DTL_DTK_DATE;

                    while( (stlIsalnum(*sTimeStr) == STL_TRUE) ||
                           (*sTimeStr == sDelimit) )
                    {
                        /**
                         * @todo DTD_APPEND_CHAR( .., .., stlTolower(*sTimeStr))
                         *       의 stlTolower적용 확인해야 함( multi-byte character set 관련 )
                         *       postgres에서는 pg_tolower()함수를 내부적으로 생성.적용.
                         * 
                         * 
                         */
                        DTD_APPEND_CHAR( sBuffer, sBufferEnd, stlTolower(*sTimeStr) );
                        sTimeStr++;
                        STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                    }

                    /* trunc space */
                    while( stlIsspace(*sTimeStr) == STL_TRUE )
                    {
                        sTimeStr++;
                        STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                    }
                }
            }
            else
            {
                /* ex) 19990108, 990108
                 * 연결된 field들을 이후에 결정한다. */
                
                aFieldType[sCurrentField] = DTL_DTK_NUMBER;
            }
        }
        else if( *sTimeStr == '.' )
        {
            /*
             * leading decimal point이면, fractional seconds
             */
            aFieldType[sCurrentField] = DTL_DTK_NUMBER;
            
            DTD_APPEND_CHAR(sBuffer, sBufferEnd, *sTimeStr );
            sTimeStr++;
            STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

            while( stlIsdigit(*sTimeStr) == STL_TRUE )
            {
                DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                sTimeStr++;
                STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
            }

            /* trunc space */
            while( stlIsspace(*sTimeStr) == STL_TRUE )
            {
                sTimeStr++;
                STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
            }
        }
        else if( stlIsalpha(*sTimeStr) == STL_TRUE )
        {
            /*
             * leading text이면,
             * date string, month, day of week, special, or timezone
             */

            /**
             * @todo DTD_APPEND_CHAR( .., .., stlTolower(*sTimeStr))
             *       의 stlTolower적용 확인해야 함( multi-byte character set 관련 )
             *       postgres에서는 pg_tolower()함수를 내부적으로 생성.적용.
             */
            aFieldType[sCurrentField] = DTL_DTK_STRING;

            sIsJulian = ( *sTimeStr == 'J' ) || (*sTimeStr == 'j' );
            
            DTD_APPEND_CHAR( sBuffer, sBufferEnd, stlTolower(*sTimeStr) );
            sTimeStr++;
            STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

            while( stlIsalpha(*sTimeStr) == STL_TRUE )
            {
                DTD_APPEND_CHAR( sBuffer, sBufferEnd, stlTolower(*sTimeStr) );
                sTimeStr++;
                STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                sIsJulian = STL_FALSE;
            }

            /* date는     '-', '/', '.'를 포함할 수 있고,
             * timezone도 '/', '+', '-', '_', ':'를
             * 포함('_',':'는 first punctuation이 될수없다) 할수 있다.
             * date인지 timezone인지를 구별할 수 있어야 한다.
             */
            
            /* trunc space */
            while( ( sIsJulian == STL_FALSE ) && ( stlIsspace(*sTimeStr) == STL_TRUE ) )
            {
                sTimeStr++;
                STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
            }

            sIsDate = STL_FALSE;
            sIsSkip = (*sTimeStr == '-') || (*sTimeStr == '/') || (*sTimeStr == '+');

            if( (*sTimeStr == '/') || (*sTimeStr == '.') )
            {
                sIsDate  = STL_TRUE;
            }
            else if( (*sTimeStr == '-') || (*sTimeStr == '+') || (stlIsdigit(*sTimeStr) == STL_TRUE) )
            {
                /* null-terminate current field value */
                *sBuffer = '\0';
                
                /* non-timezone keyword인지를 확인 */
                if( dtdDateBinarySearch( aField[sCurrentField],
                                         dtlDateTokenTbl,
                                         dtlDateTokenTblSize )
                    == NULL )
                {
                    sIsDate = STL_TRUE;
                }
                else
                {
                    STL_TRY( dtdDecodeSpecial( sCurrentField,
                                               aField[sCurrentField],
                                               & sValue,
                                               & sValueType,
                                               aErrorStack )
                             == STL_SUCCESS );

                    switch( sValueType )
                    {
                        case DTL_RESERV:
                        case DTL_DTZMOD:
                        case DTL_DTZ:
                        case DTL_TZ:
                        case DTL_AMPM:
                        case DTL_ADBC:
                        case DTL_UNITS:
                        case DTL_ISOTIME:
                        case DTL_UNKNOWN_FIELD:
                            {
                                STL_THROW( RAMP_READ_FIELD );
                            }
                        default:
                            {
                                break;
                            }
                    }
                    
                    if(*sTimeStr == '-')
                    {
                        sIsDate = STL_TRUE;
                    }
                    else
                    {
                        /* Do Nothing */
                    }   
                }
            }
            else
            {
                // Do Nothing
            }

            if( sIsDate == STL_TRUE )
            {
                aFieldType[sCurrentField] = DTL_DTK_DATE;

                /**
                 * @todo DTD_APPEND_CHAR( .., .., stlTolower(*sTimeStr))
                 *       의 stlTolower적용 확인해야 함( multi-byte character set 관련 )
                 *       postgres에서는 pg_tolower()함수를 내부적으로 생성.적용.
                 */
                DTD_APPEND_CHAR( sBuffer, sBufferEnd, stlTolower(*sTimeStr) );
                sTimeStr++;
                STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

                /* trunc space */
                while( ( sIsSkip == STL_TRUE ) && ( stlIsspace(*sTimeStr) == STL_TRUE ) )
                {
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }

                sIsSkip = STL_TRUE;
                
                while( (*sTimeStr == '+') || (*sTimeStr == '-') ||
                       (*sTimeStr == '/') || (*sTimeStr == '_') ||
                       (*sTimeStr == '.') || (*sTimeStr == ':') ||
                       (stlIsalnum(*sTimeStr) == STL_TRUE) )
                {
                    /**
                     * @todo DTD_APPEND_CHAR( .., .., stlTolower(*sTimeStr))
                     *       의 stlTolower적용 확인해야 함( multi-byte character set 관련 )
                     *       postgres에서는 pg_tolower()함수를 내부적으로 생성.적용.
                     */
                    DTD_APPEND_CHAR( sBuffer, sBufferEnd, stlTolower(*sTimeStr) );
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

                    sIsSkip = !(*sTimeStr == '.');
                }

                /* trunc space */
                while( ( sIsSkip == STL_TRUE ) && ( stlIsspace(*sTimeStr) == STL_TRUE ) )
                {
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }

                sIsSkip = STL_TRUE;
                
                while( (*sTimeStr == '+') || (*sTimeStr == '-') ||
                       (*sTimeStr == '/') || (*sTimeStr == '_') ||
                       (*sTimeStr == '.') || (*sTimeStr == ':') ||
                       (stlIsalnum(*sTimeStr) == STL_TRUE) )
                {
                    /**
                     * @todo DTD_APPEND_CHAR( .., .., stlTolower(*sTimeStr))
                     *       의 stlTolower적용 확인해야 함( multi-byte character set 관련 )
                     *       postgres에서는 pg_tolower()함수를 내부적으로 생성.적용.
                     */
                    DTD_APPEND_CHAR( sBuffer, sBufferEnd, stlTolower(*sTimeStr) );
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

                    sIsSkip = !(*sTimeStr == '.');
                }
            }
            else
            {
                // Do Nothing
            }
        }
        else if( (*sTimeStr == '+') || (*sTimeStr == '-') )
        {
            /*
             * leading sign이면, special or numeric timezone
             */

            aFieldType[sCurrentField] = DTL_DTK_TZ;

            DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
            sTimeStr++;
            STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

            while( stlIsspace(*sTimeStr) == STL_TRUE )
            {
                sTimeStr++;
                STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
            }

            if( stlIsdigit(*sTimeStr) == STL_TRUE )
            {
                /*
                 * numeric timezone
                 * 
                 * DTL_DTK_TZ - leading "+" or "-" followed by digits (also eats ':', '.', '-')
                 * DTL_DTK_TZ는 signed float 또는 yyyy-mm 도 가능
                 */
                aFieldType[sCurrentField] = DTL_DTK_TZ;

                while( ( stlIsdigit(*sTimeStr) == STL_TRUE ) ||
                       (*sTimeStr == '.') )
                {
                    DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }

                /* trunc space */
                while( stlIsspace(*sTimeStr) == STL_TRUE )
                {
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }

                if( (*sTimeStr == ':') || (*sTimeStr == '.') || (*sTimeStr == '-') )
                {
                    sIsSkip = !(*sTimeStr == '.');
                    
                    DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

                    /* trunc space */
                    while( ( sIsSkip == STL_TRUE ) && ( stlIsspace(*sTimeStr) == STL_TRUE ) )
                    {
                        sTimeStr++;
                        STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                    }

                    while( ( stlIsdigit(*sTimeStr) == STL_TRUE ) ||
                           (*sTimeStr == '.') )
                    {
                        DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                        sTimeStr++;
                        STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                    }
                }

                /* trunc space */
                while( stlIsspace(*sTimeStr) == STL_TRUE )
                {
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }

                if( (*sTimeStr == ':') || (*sTimeStr == '.') || (*sTimeStr == '-') )
                {
                    sIsSkip = !(*sTimeStr == '.');
                    
                    DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

                    /* trunc space */
                    while( ( sIsSkip == STL_TRUE ) && ( stlIsspace(*sTimeStr) == STL_TRUE ) )
                    {
                        sTimeStr++;
                        STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                    }

                    while( ( stlIsdigit(*sTimeStr) == STL_TRUE ) ||
                           (*sTimeStr == '.') )
                    {
                        DTD_APPEND_CHAR( sBuffer, sBufferEnd, *sTimeStr );
                        sTimeStr++;
                        STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                    }
                }

                /* trunc space */
                while( ( sIsSkip == STL_TRUE ) && ( stlIsspace(*sTimeStr) == STL_TRUE ) )
                {
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }
            }
            else if( stlIsalpha(*sTimeStr) == STL_TRUE )
            {
                /*
                 * special
                 */
                aFieldType[sCurrentField] = DTL_DTK_SPECIAL;

                /**
                 * @todo DTD_APPEND_CHAR( .., .., stlTolower(*sTimeStr))
                 *       의 stlTolower적용 확인해야 함( multi-byte character set 관련 )
                 *       postgres에서는 pg_tolower()함수를 내부적으로 생성.적용.
                 */
                DTD_APPEND_CHAR( sBuffer, sBufferEnd, stlTolower(*sTimeStr) );
                sTimeStr++;
                STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );

                while( stlIsalpha(*sTimeStr) == STL_TRUE )
                {
                    /**
                     * @todo DTD_APPEND_CHAR( .., .., stlTolower(*sTimeStr))
                     *       의 stlTolower적용 확인해야 함( multi-byte character set 관련 )
                     *       postgres에서는 pg_tolower()함수를 내부적으로 생성.적용.
                     */
                    DTD_APPEND_CHAR( sBuffer, sBufferEnd, stlTolower(*sTimeStr) );
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }

                while( stlIsspace(*sTimeStr) == STL_TRUE )
                {
                    sTimeStr++;
                    STL_TRY_THROW( sTimeStr < sTimeStrEnd, RAMP_READ_FIELD );
                }
            }
            else
            {
                STL_THROW( RAMP_ERROR_BAD_FORMAT );
            }
        }
        else if( stlIspunct(*sTimeStr) == STL_TRUE )
        {
            /*
             * 이외의 기호는 무시. but 구분자로 사용.
             */ 
            sTimeStr++;
            continue;
        }
        else
        {
            /*
             * invalid
             */ 
            STL_THROW( RAMP_ERROR_BAD_FORMAT );
        }

        STL_RAMP( RAMP_READ_FIELD );

        *sBuffer = '\0';

        sBuffer++;
        sCurrentField++;
    }

    *aNumFields = sCurrentField;
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERROR_BAD_FORMAT )
    {
        if( (aDataType == DTL_TYPE_INTERVAL_YEAR_TO_MONTH) ||
            (aDataType == DTL_TYPE_INTERVAL_DAY_TO_SECOND) )
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          DTL_ERRCODE_C_STRING_NOT_VALID_INTERVAL_LITERAL,
                          NULL,
                          aErrorStack );            
        }
        else
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          DTL_ERRCODE_C_STRING_NOT_VALID_DATE_OR_TIME_OR_TIMESTAMP_LITERAL,
                          NULL,
                          aErrorStack );
        }
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief dtlDateTokentbl에서 aKey와 일치하는 정보를 binary search로 찾는다.
 * @param[in]  aKey        찾고자 하는 값
 * @param[in]  aBase       배열
 * @param[in]  aBaseSize   배열의 크기  
 * @return aKey와 일치하는 엔트리 주소 반환.
 */
const dtlDateToken * dtdDateBinarySearch( const stlChar      * aKey,
                                          const dtlDateToken * aBase,
                                          stlInt32             aBaseSize )
{
    const stlChar      * sKey;
    const dtlDateToken * sBase;
    const dtlDateToken * sLast;
    dtlDateToken       * sPosition = NULL;
    stlInt32             sResult;

    sKey  = aKey;
    sBase = aBase;
    sLast = aBase + aBaseSize - 1;

    while( sLast >= sBase )
    {
        sPosition = (dtlDateToken*)(sBase + ((sLast - sBase) >> 1));
        sResult = sKey[0] - sPosition->mToken[0];

        if( sResult == 0 )
        {
            sResult = stlStrncmp( sKey, sPosition->mToken, DTL_DATE_TOKEN_MAX_LEN );
            if( sResult == 0 )
            {
                break;
            }
            else
            {
                // Do Nothing
            }
        }
        else
        {
            // Do Nothing 
        }
        
        if( sResult < 0 )
        {
            sLast = sPosition - 1;                
        }
        else
        {
            sBase = sPosition + 1;
        }
    }
    
    if( sLast >= sBase )
    {
        // Do Nothing
    }
    else
    {
        sPosition = NULL;        
    }

    return sPosition;
}

/* /\** */
/*  * @brief dtdParseDateTime에서 분류된 field들에 대한 해석. */
/*  * @param[in]  aString            Date/Time input string */
/*  * @param[in]  aDataType          data Type */
/*  * @param[in]  aField             token단위로 분류된 배열  */
/*  * @param[in]  aFieldType         token단위로 분류된 배열의 type */
/*  * @param[in]  aNumberFields      aField와 aFieldType의 수 */
/*  * @param[in]  aDateOrder         date 출력 order */
/*  *                                <BR> DTL_DATE_ORDER_YMD, DTL_DATE_ORDER_DMY, DTL_DATE_ORDER_MDY */
/*  * @param[out] aDateType          date token type */
/*  * @param[out] aDtlExpTime        dtlExpTime */
/*  * @param[out] aFractionalSecond  fractionalSecond */
/*  * @param[out] aTimeZone          timezone */
/*  * @param[in]  aVectorFunc        Function Vector */
/*  * @param[in]  aVectorArg         Vector Argument */
/*  * @param[out] aErrorStack        에러 스택  */
/*  *  */
/*  * @remark */
/*  *  <BR> External format(s): */
/*  *  <BR>     "<weekday> <month>-<day>-<year> <hour>:<minute>:<second>" */
/*  *  <BR>     "Fri Feb-7-1997 15:23:27" */
/*  *  <BR>     "Feb-7-1997 15:23:27" */
/*  *  <BR>     "2-7-1997 15:23:27" */
/*  *  <BR>     "1997-2-7 15:23:27" */
/*  *  <BR>     "1997.038 15:23:27"         (day of year 1-366) */
/*  *  <BR> Also supports input in compact time: */
/*  *  <BR>     "970207 152327" */
/*  *  <BR>     "97038 152327" */
/*  *  <BR>     "20011225T040506.789-07" */
/*  *			 */
/*  * date, timestamp, timestampTz type에서 호출됨.  */
/*  *\/ */
/* stlStatus dtdDecodeDateTime( const stlChar         * aString, */
/*                              dtlDataType             aDataType, */
/*                              stlChar              ** aField, */
/*                              stlInt32              * aFieldType, */
/*                              stlInt32                aNumberFields, */
/*                              stlInt32                aDateOrder, */
/*                              stlInt32              * aDateType, */
/*                              dtlExpTime            * aDtlExpTime, */
/*                              dtlFractionalSecond   * aFractionalSecond, */
/*                              stlInt32              * aTimeZone, */
/*                              dtlFuncVector         * aVectorFunc, */
/*                              void                  * aVectorArg, */
/*                              stlErrorStack         * aErrorStack ) */
/* { */
/*     stlInt32         sFieldMask; */
/*     stlInt32         sTimeMask; */
/*     stlInt32         sType; */
/*     stlInt32         sPrefixType;  */
/*     stlInt32         i; */
/*     stlInt32         sValue; */
/*     stlInt32         sFieldLen; */
/*     stlInt32         sHour24; */
/*     stlInt32         sDateType; */
/*     stlInt64         sConvertedValue; */
/*     stlBool          sIsJulian; */
/*     stlBool          sIsTwoDigits; */
/*     stlBool          sHaveTextMonth; */
/*     stlBool          sIsBC; */
/*     stlInt32         sTimeZone; */
/*     stlUInt32        sErrorCode;     */
/*     stlChar        * sBuffer; */

/*     sIsJulian = STL_FALSE; */
/*     sIsTwoDigits = STL_FALSE; */
/*     sHaveTextMonth = STL_FALSE; */
/*     sFieldMask = 0; */
/*     sPrefixType = 0;  /\* "prefix type" for ISO y2001m02d04 format *\/ */
/*     sHour24 = DTL_HR24; */
/*     sIsBC = STL_FALSE; */

/*     *aDateType = DTL_DTK_DATE;     */
/*     aDtlExpTime->mHour = 0; */
/*     aDtlExpTime->mMinute = 0; */
/*     aDtlExpTime->mSecond = 0; */
/*     aDtlExpTime->mIsDaylightSaving = -1; */
/*     *aFractionalSecond = 0; */

/*     if( aTimeZone != NULL ) */
/*     { */
/*         *aTimeZone = 0; */
/*     } */
/*     else */
/*     { */
/*         // Do Nothing  */
/*     } */

/*     for( i = 0; i < aNumberFields; i++ ) */
/*     { */
/*         switch( aFieldType[i] ) */
/*         { */
/*             case DTL_DTK_DATE: */
/*                 if( sPrefixType == DTL_DTK_JULIAN ) */
/*                 { */
/*                     /\* */
/*                      * time zone을 포함한 julian day */
/*                      *\/  */
/*                     STL_TRY_THROW( aTimeZone != NULL, RAMP_ERROR_BAD_FORMAT ); */

/*                     STL_TRY_THROW( stlStrToInt64( aField[i], */
/*                                                   STL_NTS, */
/*                                                   & sBuffer, */
/*                                                   10, */
/*                                                   & sConvertedValue, */
/*                                                   aErrorStack ) == STL_SUCCESS, */
/*                                    ERROR_CONVERSION ); */
/*                     STL_TRY_THROW( (sConvertedValue >= 0) && (sConvertedValue <= STL_INT32_MAX), */
/*                                    ERROR_FIELD_OVERFLOW ); */
                    
/*                     sValue = (stlInt32)sConvertedValue; */

/*                     STL_TRY( dtdJulianDate2Date( sValue, */
/*                                                  & aDtlExpTime->mYear, */
/*                                                  & aDtlExpTime->mMonth, */
/*                                                  & aDtlExpTime->mDay, */
/*                                                  aErrorStack ) */
/*                              == STL_SUCCESS ); */

/*                     sIsJulian = STL_TRUE; */

/*                     /\* 나머지 string으로 timezone 정보를 얻는다. *\/ */
/*                     STL_TRY( dtdDecodeTimezone( sBuffer, */
/*                                                 aTimeZone, */
/*                                                 aErrorStack ) */
/*                              == STL_SUCCESS ); */

/*                     sTimeMask = DTL_DTK_DATE_M | DTL_DTK_TIME_M | DTL_DTK_M(DTL_TZ); */
/*                     sPrefixType = 0; */

/*                     break; */
/*                 } */
/*                 else if( (sPrefixType != 0) || */
/*                          ((sFieldMask & (DTL_DTK_M(DTL_MONTH) | DTL_DTK_M(DTL_DAY))) */
/*                           == (DTL_DTK_M(DTL_MONTH) | DTL_DTK_M(DTL_DAY))) ) */
/*                 { */
/*                     /\* */
/*                      * ex) America/New_Yor */
/*                      *     hhmmss-zz */
/*                      *\/ */
                    
/*                     STL_TRY_THROW( aTimeZone != NULL, RAMP_ERROR_BAD_FORMAT ); */

/*                     if( (stlIsdigit(*aField[i]) == STL_TRUE ) || */
/*                         (sPrefixType != 0) ) */
/*                     { */
/*                         if( sPrefixType != 0 ) */
/*                         { */
/*                             STL_TRY_THROW( sPrefixType == DTL_DTK_TIME, RAMP_ERROR_BAD_FORMAT ); */
/*                             sPrefixType = 0; */
/*                         } */
/*                         else */
/*                         { */
/*                             // Do Nothing */
/*                         } */

/*                         STL_TRY_THROW( (sFieldMask & DTL_DTK_TIME_M) != DTL_DTK_TIME_M, */
/*                                        RAMP_ERROR_BAD_FORMAT ); */

/*                         STL_TRY_THROW( (sBuffer = stlStrchr(aField[i], '-')) != NULL, */
/*                                        RAMP_ERROR_BAD_FORMAT ); */

/*                         /\* 나머지 string으로 timezone 정보를 얻는다. *\/ */
/*                         STL_TRY( dtdDecodeTimezone( sBuffer, */
/*                                                     aTimeZone, */
/*                                                     aErrorStack ) == STL_SUCCESS ); */

/*                         *sBuffer = '\0'; */

/*                         STL_TRY( dtdDecodeNumberField( stlStrlen(aField[i]), */
/*                                                        aField[i], */
/*                                                        sFieldMask, */
/*                                                        & sTimeMask, */
/*                                                        aDtlExpTime, */
/*                                                        aFractionalSecond, */
/*                                                        & sIsTwoDigits, */
/*                                                        & sDateType, */
/*                                                        aErrorStack ) */
/*                                  == STL_SUCCESS ); */

/*                         sTimeMask |= DTL_DTK_M(DTL_TZ); */
/*                     } */
/*                     else */
/*                     { */
/*                         /\* */
/*                          * time zone string으로 time zone offset정보 등을 얻는다. */
/*                          * 향후 지원예정시 ..... */
/*                          *\/ */
/*                         STL_THROW( RAMP_ERROR_BAD_FORMAT );                         */
/*                     } */
/*                 } */
/*                 else */
/*                 { */
/*                     STL_TRY( dtdDecodeDate( aField[i], */
/*                                             sFieldMask, */
/*                                             aDateOrder, */
/*                                             & sTimeMask, */
/*                                             & sIsTwoDigits, */
/*                                             aDtlExpTime, */
/*                                             aErrorStack ) */
/*                              == STL_SUCCESS ); */
/*                 } */
/*                 break; */
/*             case DTL_DTK_TIME: */
/*                 { */
/*                     STL_TRY( dtdDecodeTime( aField[i], */
/*                                             sFieldMask, */
/*                                             DTL_INTERVAL_INDICATOR_NA, */
/*                                             & sTimeMask, */
/*                                             aDtlExpTime, */
/*                                             aFractionalSecond, */
/*                                             aErrorStack ) */
/*                              == STL_SUCCESS ); */

/*                     /\* limit 체크, > 24:00:00 *\/ */
/*                     if( (aDtlExpTime->mHour > 24) || */
/*                         ((aDtlExpTime->mHour == 24) && */
/*                          ((aDtlExpTime->mMinute > 0) || (aDtlExpTime->mSecond > 0) || */
/*                           (*aFractionalSecond > 0))) ) */
/*                     { */
/*                         STL_THROW( ERROR_FIELD_OVERFLOW ); */
/*                     } */
/*                     else */
/*                     { */
/*                         // Do Nothing  */
/*                     } */
                    
/*                     break; */
/*                 } */
/*             case DTL_DTK_TZ: */
/*                 { */
/*                     STL_TRY_THROW( aTimeZone != NULL, RAMP_ERROR_BAD_FORMAT ); */
/*                     STL_TRY( dtdDecodeTimezone( aField[i], */
/*                                                 & sTimeZone, */
/*                                                 aErrorStack ) */
/*                              == STL_SUCCESS ); */
/*                     *aTimeZone = sTimeZone; */
/*                     sTimeMask = DTL_DTK_M(DTL_TZ); */
                    
/*                     break; */
/*                 } */
/*             case DTL_DTK_NUMBER: */
/*                 { */
/*                     /\* */
/*                      * ex) "y2001m02d04" */
/*                      *\/ */
                    
/*                     if( sPrefixType != 0 ) */
/*                     { */
/*                         STL_TRY_THROW( stlStrToInt64( aField[i], */
/*                                                       STL_NTS, */
/*                                                       & sBuffer, */
/*                                                       10, */
/*                                                       & sConvertedValue, */
/*                                                       aErrorStack ) == STL_SUCCESS, */
/*                                        ERROR_CONVERSION ); */
                        
/*                         STL_TRY_THROW( (sConvertedValue >= STL_INT32_MIN) && */
/*                                        (sConvertedValue <= STL_INT32_MAX), */
/*                                        ERROR_FIELD_OVERFLOW ); */

/*                         sValue = (stlInt32)sConvertedValue; */

/*                         if( *sBuffer == '.' ) */
/*                         { */
/*                             switch( sPrefixType ) */
/*                             { */
/*                                 case DTL_DTK_JULIAN: */
/*                                     /\* FALL_THROUGH *\/ */
/*                                 case DTL_DTK_TIME: */
/*                                     /\* FALL_THROUGH *\/ */
/*                                 case DTL_DTK_SECOND: */
/*                                     break; */
/*                                 default: */
/*                                     STL_THROW( RAMP_ERROR_BAD_FORMAT); */
/*                                     break; */
/*                             } */
/*                         } */
/*                         else if( *sBuffer != '\0' ) */
/*                         { */
/*                             STL_THROW( RAMP_ERROR_BAD_FORMAT ); */
/*                         } */
/*                         else */
/*                         { */
/*                             // Do Nothing */
/*                         } */

/*                         switch( sPrefixType ) */
/*                         { */
/*                             case DTL_DTK_YEAR: */
/*                                 { */
/*                                     aDtlExpTime->mYear = sValue; */
/*                                     sTimeMask = DTL_DTK_M(DTL_YEAR); */
                                    
/*                                     break; */
/*                                 } */
/*                             case DTL_DTK_MONTH: */
/*                                 { */
/*                                     if( ((sFieldMask & DTL_DTK_M(DTL_MONTH)) != 0) && */
/*                                         ((sFieldMask & DTL_DTK_M(DTL_HOUR)) != 0) ) */
/*                                     { */
/*                                         aDtlExpTime->mMinute = sValue; */
/*                                         sTimeMask = DTL_DTK_M(DTL_MINUTE); */
/*                                     } */
/*                                     else */
/*                                     { */
/*                                         aDtlExpTime->mMonth = sValue; */
/*                                         sTimeMask = DTL_DTK_M(DTL_MONTH); */
/*                                     } */
                                    
/*                                     break; */
/*                                 } */
/*                             case DTL_DTK_DAY: */
/*                                 { */
/*                                     aDtlExpTime->mDay = sValue; */
/*                                     sTimeMask = DTL_DTK_M(DTL_DAY); */
                                    
/*                                     break; */
/*                                 } */
/*                             case DTL_DTK_HOUR: */
/*                                 { */
/*                                     aDtlExpTime->mHour = sValue; */
/*                                     sTimeMask = DTL_DTK_M(DTL_HOUR); */
                                    
/*                                     break; */
/*                                 } */
/*                             case DTL_DTK_MINUTE: */
/*                                 { */
/*                                     aDtlExpTime->mMinute = sValue; */
/*                                     sTimeMask = DTL_DTK_M(DTL_MINUTE); */
                                    
/*                                     break; */
/*                                 } */
/*                             case DTL_DTK_SECOND: */
/*                                 { */
/*                                     aDtlExpTime->mSecond = sValue; */
/*                                     sTimeMask = DTL_DTK_M(DTL_SECOND); */

/*                                     if( *sBuffer == '.' ) */
/*                                     { */
/*                                         STL_TRY( dtdParseFractionalSecond( sBuffer, */
/*                                                                            aFractionalSecond, */
/*                                                                            aErrorStack ) */
/*                                                  == STL_SUCCESS ); */
/*                                         sTimeMask = DTL_DTK_ALL_SECS_M; */
/*                                     } */
/*                                     else */
/*                                     { */
/*                                         // Do Nothing */
/*                                     } */

/*                                     break; */
/*                                 } */
/*                             case DTL_DTK_TZ: */
/*                                 { */
/*                                     sTimeMask = DTL_DTK_M(DTL_TZ); */

/*                                     STL_TRY_THROW( aTimeZone != NULL, RAMP_ERROR_BAD_FORMAT ); */
/*                                     STL_TRY( dtdDecodeTimezone( aField[i], */
/*                                                                 aTimeZone, */
/*                                                                 aErrorStack ) */
/*                                              == STL_SUCCESS ); */
                                    
/*                                     break; */
/*                                 } */
/*                             case DTL_DTK_JULIAN: */
/*                                 { */
/*                                     /\* previous field was a label for "julian date" *\/ */
                                    
/*                                     STL_TRY_THROW( sValue >= 0, ERROR_FIELD_OVERFLOW ); */

/*                                     sTimeMask = DTL_DTK_DATE_M; */
/*                                     STL_TRY( dtdJulianDate2Date( sValue, */
/*                                                                  & aDtlExpTime->mYear, */
/*                                                                  & aDtlExpTime->mMonth, */
/*                                                                  & aDtlExpTime->mDay, */
/*                                                                  aErrorStack ) */
/*                                              == STL_SUCCESS ); */
/*                                     sIsJulian = STL_TRUE; */

/*                                     /\* fractional Julian Day *\/ */
/*                                     if( *sBuffer == '.' ) */
/*                                     { */
/*                                         STL_TRY_THROW( stlStrToInt64( sBuffer + 1, */
/*                                                                       STL_NTS, */
/*                                                                       & sBuffer, */
/*                                                                       10, */
/*                                                                       & sConvertedValue, */
/*                                                                       aErrorStack ) */
/*                                                        == STL_SUCCESS, */
/*                                                        RAMP_ERROR_BAD_FORMAT ); */

/*                                         STL_TRY_THROW( *sBuffer == '\0', RAMP_ERROR_BAD_FORMAT ); */

/*                                         sConvertedValue *= DTL_SECS_PER_DAY; */

/*                                         dtdTimestamp2Time( sConvertedValue, */
/*                                                            & aDtlExpTime->mHour, */
/*                                                            & aDtlExpTime->mMinute, */
/*                                                            & aDtlExpTime->mSecond, */
/*                                                            aFractionalSecond ); */

/*                                         sTimeMask |= DTL_DTK_TIME_M; */
/*                                     } */
/*                                     else */
/*                                     { */
/*                                         // Do Nothing */
/*                                     } */
                                    
/*                                     break; */
/*                                 } */
/*                             case DTL_DTK_TIME: */
/*                                 { */
/*                                     /\* previous field was "t" for ISO time *\/ */
                                    
/*                                     STL_TRY( dtdDecodeNumberField( stlStrlen(aField[i]), */
/*                                                                    aField[i], */
/*                                                                    ( sFieldMask | DTL_DTK_DATE_M ), */
/*                                                                    & sTimeMask, */
/*                                                                    aDtlExpTime, */
/*                                                                    aFractionalSecond, */
/*                                                                    & sIsTwoDigits, */
/*                                                                    & sDateType, */
/*                                                                    aErrorStack ) */
/*                                              == STL_SUCCESS ); */

/*                                     STL_TRY_THROW( sTimeMask == DTL_DTK_TIME_M, RAMP_ERROR_BAD_FORMAT ); */
                                    
/*                                     break; */
/*                                 } */
/*                             default: */
/*                                 { */
/*                                     STL_THROW( RAMP_ERROR_BAD_FORMAT ); */

/*                                     break; */
/*                                 } */
/*                         } */

/*                         sPrefixType = 0; */
/*                         *aDateType = DTL_DTK_DATE; */
/*                     } */
/*                     else */
/*                     { */
/*                         sFieldLen = stlStrlen( aField[i] ); */
/*                         sBuffer = stlStrchr( aField[i], '.' ); */

/*                         if( (sBuffer != NULL) && ((sFieldMask & DTL_DTK_DATE_M) == 0) ) */
/*                         { */
/*                             /\* Embedded decimal and no date yet? *\/ */
                            
/*                             STL_TRY( dtdDecodeDate( aField[i], */
/*                                                     sFieldMask, */
/*                                                     aDateOrder, */
/*                                                     & sTimeMask, */
/*                                                     & sIsTwoDigits, */
/*                                                     aDtlExpTime, */
/*                                                     aErrorStack ) */
/*                                      == STL_SUCCESS ); */
/*                         } */
/*                         else if( (sBuffer != NULL) && ((sFieldLen - stlStrlen(sBuffer)) > 2) ) */
/*                         { */
/*                             /\* embedded decimal and several digits before? *\/ */
/*                             /\* */
/*                              * ex) 20011223 or 040506 */
/*                              *\/  */
/*                             STL_TRY( dtdDecodeNumberField( sFieldLen, */
/*                                                            aField[i], */
/*                                                            sFieldMask, */
/*                                                            & sTimeMask, */
/*                                                            aDtlExpTime, */
/*                                                            aFractionalSecond, */
/*                                                            & sIsTwoDigits, */
/*                                                            & sDateType, */
/*                                                            aErrorStack ) */
/*                                      == STL_SUCCESS ); */
/*                         } */
/*                         else if( sFieldLen > 4 ) */
/*                         { */
/*                             /\* embedded decimal and several digits before? *\/ */
/*                             STL_TRY( dtdDecodeNumberField( sFieldLen, */
/*                                                            aField[i], */
/*                                                            sFieldMask, */
/*                                                            & sTimeMask, */
/*                                                            aDtlExpTime, */
/*                                                            aFractionalSecond, */
/*                                                            & sIsTwoDigits, */
/*                                                            & sDateType, */
/*                                                            aErrorStack ) */
/*                                      == STL_SUCCESS ); */
/*                         } */
/*                         else */
/*                         { */
/*                             /\* single date/time field... *\/ */
                            
/*                             STL_TRY( dtdDecodeNumber( sFieldLen, */
/*                                                       aField[i], */
/*                                                       sHaveTextMonth, */
/*                                                       sFieldMask, */
/*                                                       aDateOrder, */
/*                                                       & sTimeMask, */
/*                                                       aDtlExpTime, */
/*                                                       aFractionalSecond, */
/*                                                       & sIsTwoDigits, */
/*                                                       aErrorStack ) */
/*                                      == STL_SUCCESS ); */
/*                         } */
/*                     } */
                    
/*                     break; */
/*                 } */
/*             case DTL_DTK_STRING: */
/*                 /\* FALL_THROUGH *\/ */
/*             case DTL_DTK_SPECIAL: */
/*                 { */
/*                     STL_TRY( dtdDecodeSpecial( i, */
/*                                                aField[i], */
/*                                                & sValue, */
/*                                                & sType, */
/*                                                aErrorStack ) */
/*                              == STL_SUCCESS ); */
                             
/*                     if( sType == DTL_IGNORE_DTF ) */
/*                     { */
/*                         continue; */
/*                     } */
/*                     else */
/*                     { */
/*                         // Do Nothing */
/*                     } */

/*                     sTimeMask = DTL_DTK_M( sType ); */

/*                     switch( sType ) */
/*                     { */
/*                         case DTL_RESERV: */
/*                             { */
/*                                 switch( sValue ) */
/*                                 { */
/*                                     case DTL_DTK_ZULU: */
/*                                         { */
/*                                             /\* Military abbreviation for UTC *\/ */
                                            
/*                                             sTimeMask = ( DTL_DTK_TIME_M | DTL_DTK_M(DTL_TZ) ); */
/*                                             *aDateType = DTL_DTK_DATE; */

/*                                             aDtlExpTime->mHour = 0; */
/*                                             aDtlExpTime->mMinute = 0; */
/*                                             aDtlExpTime->mSecond = 0; */

/*                                             if( aTimeZone != NULL ) */
/*                                             { */
/*                                                 *aTimeZone = 0; */
/*                                             } */
/*                                             else */
/*                                             { */
/*                                                 // Do Nothing */
/*                                             } */
                                            
/*                                             break; */
/*                                         } */
/*                                     default: */
/*                                         { */
/*                                             *aDateType = sValue; */
                                            
/*                                             break; */
/*                                         } */
/*                                 } */
                                
/*                                 break; */
/*                             } */
/*                         case DTL_MONTH: */
/*                             { */
/*                                 /\* */
/*                                  * already have a (numeric) month? then see if we can */
/*                                  * substitute... */
/*                                  *\/ */
                                
/*                                 if( ((sFieldMask & DTL_DTK_M(DTL_MONTH)) != 0) && */
/*                                     (sHaveTextMonth == STL_FALSE) && */
/*                                     ((sFieldMask & DTL_DTK_M(DTL_DAY)) == 0) && */
/*                                     (aDtlExpTime->mMonth >= 1) && */
/*                                     (aDtlExpTime->mMonth <= 31) ) */
/*                                 { */
/*                                     aDtlExpTime->mDay = aDtlExpTime->mMonth; */
/*                                     sTimeMask = DTL_DTK_M(DTL_DAY); */
/*                                 } */
/*                                 else */
/*                                 { */
/*                                     // Do Nothing */
/*                                 } */

/*                                 sHaveTextMonth = STL_TRUE; */
/*                                 aDtlExpTime->mMonth = sValue; */
                                
/*                                 break; */
/*                             } */
/*                         case DTL_DTZMOD: */
/*                             { */
/*                                 /\* */
/*                                  * daylight savings time modifier (solves "MET DST" */
/*                                  * syntax) */
/*                                  *\/ */
                                
/*                                 sTimeMask |= DTL_DTK_M(DTL_DTZ); */
/*                                 aDtlExpTime->mIsDaylightSaving = 1; */

/*                                 STL_TRY_THROW( aTimeZone != NULL, RAMP_ERROR_BAD_FORMAT ); */

/*                                 *aTimeZone += sValue * DTL_MINS_PER_HOUR; */
                                
/*                                 break; */
/*                             } */
/*                         case DTL_DTZ: */
/*                             { */
/*                                 /\* */
/*                                  * set mask for TZ here _or_ check for DTZ later when */
/*                                  * getting default timezone */
/*                                  *\/ */
                                
/*                                 sTimeMask |= DTL_DTK_M(DTL_TZ); */
/*                                 aDtlExpTime->mIsDaylightSaving = 1; */

/*                                 STL_TRY_THROW( aTimeZone != NULL, RAMP_ERROR_BAD_FORMAT ); */

/*                                 *aTimeZone = sValue * DTL_MINS_PER_HOUR; */
                                
/*                                 break; */
/*                             } */
/*                         case DTL_TZ: */
/*                             { */
/*                                 aDtlExpTime->mIsDaylightSaving = 0; */

/*                                 STL_TRY_THROW( aTimeZone != NULL, RAMP_ERROR_BAD_FORMAT ); */

/*                                 *aTimeZone = sValue * DTL_MINS_PER_HOUR; */
                                
/*                                 break; */
/*                             } */
/*                         case DTL_IGNORE_DTF: */
/*                             { */
/*                                 break; */
/*                             } */
/*                         case DTL_AMPM: */
/*                             { */
/*                                 sHour24 = sValue; */
                                
/*                                 break; */
/*                             } */
/*                         case DTL_ADBC: */
/*                             { */
/*                                 if( sValue == DTL_BC ) */
/*                                 { */
/*                                     sIsBC = STL_TRUE; */
/*                                 } */
/*                                 else */
/*                                 { */
/*                                     sIsBC = STL_FALSE; */
/*                                 } */
                                    
/*                                 break; */
/*                             } */
/*                         case DTL_DOW: */
/*                             { */
/*                                 aDtlExpTime->mWeekDay = sValue; */
                                
/*                                 break; */
/*                             } */
/*                         case DTL_UNITS: */
/*                             { */
/*                                 sTimeMask = 0; */
/*                                 sPrefixType = sValue; */
                                
/*                                 break; */
/*                             } */
/*                         case DTL_ISOTIME: */
/*                             { */
/*                                 /\* T : next field is time *\/ */
                                  
/*                                 sTimeMask = 0; */

/*                                 STL_TRY_THROW( (sFieldMask & DTL_DTK_DATE_M) == DTL_DTK_DATE_M, */
/*                                                RAMP_ERROR_BAD_FORMAT ); */

/*                                 /\* */
/*                                  * DTK_NUMBER : hhmmss.fff */
/*                                  * DTK_TIME   : hh:mm:ss.fff */
/*                                  * DTK_DATE   : hhmmss-zz */
/*                                  *\/  */
/*                                 if( (i >= (aNumberFields - 1)) || */
/*                                     ((aFieldType[i + 1] != DTL_DTK_NUMBER) && */
/*                                      (aFieldType[i + 1] != DTL_DTK_TIME) && */
/*                                      (aFieldType[i + 1] != DTL_DTK_DATE)) ) */
/*                                 { */
/*                                     STL_THROW( RAMP_ERROR_BAD_FORMAT ); */
/*                                 } */
/*                                 else */
/*                                 { */
/*                                     // Do Nothing */
/*                                 } */

/*                                 sPrefixType = sValue; */
                                
/*                                 break; */
/*                             } */
/*                         case DTL_UNKNOWN_FIELD: */
/*                             { */
/*                                 /\* 향후 time zone string 지원시 .... */
/*                                  * unknown field로 에러를 세팅하기전에, */
/*                                  * all-alpha timezone name인지를 체크. *\/ */

/*                                 STL_THROW( RAMP_ERROR_BAD_FORMAT ); */
/*                                 break; */
/*                             } */
/*                         default: */
/*                             { */
/*                                 STL_THROW( RAMP_ERROR_BAD_FORMAT ); */

/*                                 break; */
/*                             } */
/*                     } */
                    
/*                     break; */
/*                 } */
/*             default: */
/*                 { */
/*                     STL_THROW( RAMP_ERROR_BAD_FORMAT ); */
                    
/*                     break; */
/*                 } */
/*         } */

/*         STL_TRY_THROW( (sTimeMask & sFieldMask) == 0, RAMP_ERROR_BAD_FORMAT ); */

/*         sFieldMask |= sTimeMask; */
/*     } */

/*     /\* */
/*      * Y/M/D field check */
/*      *\/   */
/*     STL_TRY( dtdValidateDate( sFieldMask, */
/*                               sIsJulian, */
/*                               sIsTwoDigits, */
/*                               sIsBC, */
/*                               aDtlExpTime, */
/*                               aErrorStack ) */
/*              == STL_SUCCESS ); */

/*     /\* */
/*      * AM/PM */
/*      *\/ */
/*     if( (sHour24 != DTL_HR24) && (aDtlExpTime->mHour > 12) ) */
/*     { */
/*         STL_THROW( ERROR_FIELD_OVERFLOW ); */
/*     } */
/*     else */
/*     { */
/*         // Do Nothing */
/*     } */

/*     if( (sHour24 == DTL_AM) && (aDtlExpTime->mHour == 12) ) */
/*     { */
/*         aDtlExpTime->mHour = 0; */
/*     } */
/*     else if( (sHour24 == DTL_PM) && (aDtlExpTime->mHour != 12) ) */
/*     { */
/*         aDtlExpTime->mHour += 12; */
/*     } */
/*     else */
/*     { */
/*         // Do Nothing */
/*     } */

/*     if( *aDateType == DTL_DTK_DATE ) */
/*     { */
/*         /\* full date specs을 위한 부가적인 체크 *\/ */
        
/*         if( (sFieldMask & DTL_DTK_DATE_M) != DTL_DTK_DATE_M ) */
/*         { */
/*             STL_THROW( RAMP_ERROR_BAD_FORMAT ); */
/*         } */
/*         else */
/*         { */
/*             // Do Nothing */
/*         } */

/*         /\* timezone 이 없다면, local timezone을 찾는다. *\/ */
/*         if( (aTimeZone != NULL) && ((sFieldMask & (DTL_DTK_M(DTL_TZ))) == 0) ) */
/*         { */
/*             STL_TRY_THROW( (sFieldMask & (DTL_DTK_M(DTL_DTZMOD))) == 0, */
/*                            RAMP_ERROR_BAD_FORMAT ); */
            
/*             *aTimeZone = -(aVectorFunc->mGetGMTOffsetFunc(aVectorArg)); */
/*         } */
/*         else */
/*         { */
/*             // Do Nothing */
/*         } */
                
/*     } */
/*     else */
/*     { */
/*         // Do Nothing  */
/*     } */
    
/*     return STL_SUCCESS; */

/*     STL_CATCH( RAMP_ERROR_BAD_FORMAT ) */
/*     { */
/*         stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                       DTL_ERRCODE_C_STRING_NOT_VALID_DATE_OR_TIME_OR_TIMESTAMP_LITERAL, */
/*                       NULL, */
/*                       aErrorStack ); */
/*     } */

/*     STL_CATCH( ERROR_CONVERSION ) */
/*     { */
/*         sErrorCode = stlGetLastErrorCode( aErrorStack ); */

/*         switch( sErrorCode ) */
/*         { */
/*             case STL_ERRCODE_OUT_OF_RANGE: */
/*                 { */
/*                     stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                                   DTL_ERRCODE_C_STRING_NOT_VALID_DATE_OR_TIME_OR_TIMESTAMP_LITERAL, */
/*                                   NULL, */
/*                                   aErrorStack ); */
/*                 } */
/*             default: */
/*                 { */
/*                     break; */
/*                 } */
/*         }         */
/*     } */

/*     STL_CATCH( ERROR_FIELD_OVERFLOW ) */
/*     { */
/*         stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                       DTL_ERRCODE_C_STRING_NOT_VALID_DATE_OR_TIME_OR_TIMESTAMP_LITERAL, */
/*                       NULL, */
/*                       aErrorStack ); */
/*     } */

/*     STL_FINISH; */

/*     return STL_FAILURE;     */
/* } */

/* /\** */
/*  * @brief numeric timezone으로 해석 */
/*  * @param[in]  aStr        해석할 string정보 */
/*  * @param[in]  aTimeZone   timezone */
/*  * @param[out] aErrorStack 에러 스택 */
/*  * @remark  timezone offset 13:59까지 가능.  */
/*  *\/  */
/* stlStatus dtdDecodeTimezone( stlChar       * aStr, */
/*                              stlInt32      * aTimeZone, */
/*                              stlErrorStack * aErrorStack ) */
/* { */
/*     stlInt32    sTimeZone; */
/*     stlInt32    sHour; */
/*     stlInt32    sMinute; */
/*     stlInt64    sConvertedValue; */
/*     stlChar   * sBuffer; */
/*     stlUInt32   sErrorCode; */

/*     /\* leading character는 '+' or '-' *\/ */
/*     STL_TRY_THROW( (*aStr == '+') || (*aStr == '-'), RAMP_ERROR_BAD_FORMAT ); */

/*     STL_TRY_THROW( stlStrToInt64( aStr + 1, */
/*                                   STL_NTS, */
/*                                   & sBuffer, */
/*                                   10, */
/*                                   & sConvertedValue, */
/*                                   aErrorStack ) == STL_SUCCESS, */
/*                    ERROR_CONVERSION ); */
/*     STL_TRY_THROW( (sConvertedValue >= STL_INT32_MIN) && (sConvertedValue <= STL_INT32_MAX), */
/*                    ERROR_TZDISP_OVERFLOW ); */

/*     sHour = (stlInt32)sConvertedValue; */

/*     /\* 구분자 *\/ */
/*     if( *sBuffer == ':' ) */
/*     { */
/*         STL_TRY_THROW( stlStrToInt64( sBuffer + 1, */
/*                                       STL_NTS, */
/*                                       & sBuffer, */
/*                                       10, */
/*                                       & sConvertedValue, */
/*                                       aErrorStack ) == STL_SUCCESS, */
/*                        ERROR_CONVERSION ); */
/*         STL_TRY_THROW( (sConvertedValue >= STL_INT32_MIN) && (sConvertedValue <= STL_INT32_MAX), */
/*                        ERROR_TZDISP_OVERFLOW ); */

/*         sMinute = (stlInt32)sConvertedValue; */

/*         STL_TRY_THROW( *sBuffer == '\0', RAMP_ERROR_BAD_FORMAT ); */
/*     } */
/*     else if( (*sBuffer == '\0') && (stlStrlen(aStr+1) == 4) ) */
/*     { */
/*         sMinute = sHour % 100; */
/*         sHour = sHour / 100; */
/*     } */
/*     else */
/*     { */
/*         STL_THROW( RAMP_ERROR_BAD_FORMAT ); */
/*     } */

/*     STL_TRY_THROW( ((sHour >= 0) && (sHour < 14) && (sMinute >= 0) && (sMinute < 60)) || */
/*                    ((sHour == 14) && (sMinute == 0) && (sMinute == 0)), */
/*                    ERROR_TZDISP_OVERFLOW ); */

/*     sTimeZone = ( sHour * DTL_MINS_PER_HOUR + sMinute ) * DTL_SECS_PER_MINUTE; */

/*     if( *aStr == '-' ) */
/*     { */
/*         sTimeZone = -sTimeZone; */
/*     } */
/*     else */
/*     { */
/*         // Do Nothing  */
/*     } */

/*     *aTimeZone = -sTimeZone; */

/*     STL_TRY_THROW( *sBuffer == '\0', RAMP_ERROR_BAD_FORMAT ); */

/*     return STL_SUCCESS; */

/*     STL_CATCH( RAMP_ERROR_BAD_FORMAT ) */
/*     { */
/*         stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                       DTL_ERRCODE_C_STRING_NOT_VALID_TIME_OR_TIMESTAMP_LITERAL, */
/*                       NULL, */
/*                       aErrorStack ); */
/*     } */

/*     STL_CATCH( ERROR_CONVERSION ) */
/*     { */
/*         sErrorCode = stlGetLastErrorCode( aErrorStack ); */

/*         switch( sErrorCode ) */
/*         { */
/*             case STL_ERRCODE_OUT_OF_RANGE: */
/*                 { */
/*                     stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                                   DTL_ERRCODE_INVALID_TIME_ZONE_DISPLACEMENT_VALUE, */
/*                                   NULL, */
/*                                   aErrorStack ); */
/*                 } */
/*             default: */
/*                 { */
/*                     break; */
/*                 } */
/*         }         */
/*     } */

/*     STL_CATCH( ERROR_TZDISP_OVERFLOW ) */
/*     { */
/*         stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                       DTL_ERRCODE_INVALID_TIME_ZONE_DISPLACEMENT_VALUE, */
/*                       NULL, */
/*                       aErrorStack ); */
/*     } */

/*     STL_FINISH; */

/*     return STL_FAILURE; */
/* } */

/* /\** */
/*  * @brief date or time field의 numeric string 해석 */
/*  * @param[in]  aLen              field 길이 */
/*  * @param[in]  aStr              field string */
/*  * @param[in]  aFieldMask        field mask */
/*  * @param[out] aTimeMask         time mask */
/*  * @param[out] aDtlExpTime       dtlExpTime */
/*  * @param[out] aFractionalSecond fractional second */
/*  * @param[out] aIsTwoDigits      2-digit year */
/*  * @param[out] aDateType         date type */
/*  * @param[out] aErrorStack       에러 스택  */
/*  *\/ */
/* stlStatus dtdDecodeNumberField( stlInt32               aLen, */
/*                                 stlChar              * aStr, */
/*                                 stlInt32               aFieldMask, */
/*                                 stlInt32             * aTimeMask, */
/*                                 dtlExpTime           * aDtlExpTime, */
/*                                 dtlFractionalSecond  * aFractionalSecond, */
/*                                 stlBool              * aIsTwoDigits, */
/*                                 stlInt32             * aDateType, */
/*                                 stlErrorStack        * aErrorStack ) */
/* { */
/*     stlChar              * sBuffer; */
/*     stlChar              * sBufferEndPtr; */
/*     stlInt64               sConvertedValue; */
/*     stlUInt32              sErrorCode; */

/*     if( (sBuffer = stlStrchr(aStr, '.')) != NULL ) */
/*     { */
/*         /\* decimal point를 가지면, date or second field *\/ */
        
/*         STL_TRY_THROW( stlStrToInt64( sBuffer + 1, */
/*                                       STL_NTS, */
/*                                       & sBufferEndPtr, */
/*                                       10, */
/*                                       & sConvertedValue, */
/*                                       aErrorStack ) == STL_SUCCESS, */
/*                        RAMP_ERROR_BAD_FORMAT ); */
/*         STL_TRY_THROW( (sConvertedValue >= STL_INT32_MIN) && (sConvertedValue <= STL_INT32_MAX), */
/*                        RAMP_ERROR_BAD_FORMAT ); */

/*         *aFractionalSecond = (dtlFractionalSecond)sConvertedValue; */

/*         *sBuffer = '\0'; */
/*         aLen = stlStrlen(aStr); */
/*     } */
/*     else if( (aFieldMask & DTL_DTK_DATE_M) != DTL_DTK_DATE_M ) */
/*     { */
/*         /\* decimal point가 없고 date가 아직 완료되지 않은 경우 *\/ */
        
/*         if( aLen == 8 ) */
/*         { */
/*             /\* */
/*              * yyyymmdd */
/*              *\/ */
/*             *aTimeMask = DTL_DTK_DATE_M; */

/*             STL_TRY_THROW( stlStrToInt64( aStr + 6, */
/*                                           STL_NTS, */
/*                                           & sBufferEndPtr, */
/*                                           10, */
/*                                           & sConvertedValue, */
/*                                           aErrorStack ) == STL_SUCCESS, */
/*                            ERROR_CONVERSION ); */

/*             aDtlExpTime->mDay = (stlInt32)sConvertedValue; */
/*             *(aStr + 6) = '\0'; */

/*             STL_TRY_THROW( stlStrToInt64( aStr + 4, */
/*                                           STL_NTS, */
/*                                           & sBufferEndPtr, */
/*                                           10, */
/*                                           & sConvertedValue, */
/*                                           aErrorStack ) == STL_SUCCESS, */
/*                            ERROR_CONVERSION ); */
            
/*             aDtlExpTime->mMonth = (stlInt32)sConvertedValue; */
/*             *(aStr + 4) = '\0'; */

/*             STL_TRY_THROW( stlStrToInt64( aStr + 0, */
/*                                           STL_NTS, */
/*                                           & sBufferEndPtr, */
/*                                           10, */
/*                                           & sConvertedValue, */
/*                                           aErrorStack ) == STL_SUCCESS, */
/*                            ERROR_CONVERSION ); */

/*             aDtlExpTime->mYear = (stlInt32)sConvertedValue; */

/*             *aDateType = DTL_DTK_DATE; */

/*             STL_THROW( RAMP_SUCCESS ); */
/*         } */
/*         else if ( aLen == 6 ) */
/*         { */
/*             /\* */
/*              * yymmdd */
/*              *\/ */
/*             *aTimeMask = DTL_DTK_DATE_M; */

/*             STL_TRY_THROW( stlStrToInt64( aStr + 4, */
/*                                           STL_NTS, */
/*                                           & sBufferEndPtr, */
/*                                           10, */
/*                                           & sConvertedValue, */
/*                                           aErrorStack ) == STL_SUCCESS, */
/*                            ERROR_CONVERSION ); */

/*             aDtlExpTime->mDay = (stlInt32)sConvertedValue; */
/*             *(aStr + 4) = '\0'; */

/*             STL_TRY_THROW( stlStrToInt64( aStr + 2, */
/*                                           STL_NTS, */
/*                                           & sBufferEndPtr, */
/*                                           10, */
/*                                           & sConvertedValue, */
/*                                           aErrorStack ) == STL_SUCCESS, */
/*                            ERROR_CONVERSION ); */
            
/*             aDtlExpTime->mMonth = (stlInt32)sConvertedValue; */
/*             *(aStr + 2) = '\0'; */

/*             STL_TRY_THROW( stlStrToInt64( aStr + 0, */
/*                                           STL_NTS, */
/*                                           & sBufferEndPtr, */
/*                                           10, */
/*                                           & sConvertedValue, */
/*                                           aErrorStack ) == STL_SUCCESS, */
/*                            ERROR_CONVERSION ); */

/*             aDtlExpTime->mYear = (stlInt32)sConvertedValue; */

/*             *aIsTwoDigits = STL_TRUE; */
/*             *aDateType = DTL_DTK_DATE; */

/*             STL_THROW( RAMP_SUCCESS ); */
/*         } */
/*         else */
/*         { */
/*             // Do Nothing */
/*         } */
/*     } */
/*     else */
/*     { */
/*         // Do Nothing */
/*     } */
    
/*     if( (aFieldMask & DTL_DTK_TIME_M) != DTL_DTK_TIME_M ) */
/*     { */
/*         if( aLen == 6 ) */
/*         { */
/*             /\* */
/*              * hhmmss */
/*              *\/ */
/*             *aTimeMask = DTL_DTK_TIME_M; */

/*             STL_TRY_THROW( stlStrToInt64( aStr + 4, */
/*                                           STL_NTS, */
/*                                           & sBufferEndPtr, */
/*                                           10, */
/*                                           & sConvertedValue, */
/*                                           aErrorStack ) == STL_SUCCESS, */
/*                            ERROR_CONVERSION ); */

/*             aDtlExpTime->mSecond = (stlInt32)sConvertedValue; */
/*             *(aStr + 4) = '\0'; */

/*             STL_TRY_THROW( stlStrToInt64( aStr + 2, */
/*                                           STL_NTS, */
/*                                           & sBufferEndPtr, */
/*                                           10, */
/*                                           & sConvertedValue, */
/*                                           aErrorStack ) == STL_SUCCESS, */
/*                            ERROR_CONVERSION ); */
            
/*             aDtlExpTime->mMinute = (stlInt32)sConvertedValue; */
/*             *(aStr + 2) = '\0'; */

/*             STL_TRY_THROW( stlStrToInt64( aStr + 0, */
/*                                           STL_NTS, */
/*                                           & sBufferEndPtr, */
/*                                           10, */
/*                                           & sConvertedValue, */
/*                                           aErrorStack ) == STL_SUCCESS, */
/*                            ERROR_CONVERSION ); */
            
/*             aDtlExpTime->mHour = (stlInt32)sConvertedValue; */

/*             *aDateType = DTL_DTK_TIME; */

/*             STL_THROW( RAMP_SUCCESS ); */
/*         } */
/*         else if( aLen == 4 ) */
/*         { */
/*             /\* */
/*              * hhmm */
/*              *\/ */
/*             *aTimeMask = DTL_DTK_TIME_M; */

/*             aDtlExpTime->mSecond = 0; */

/*             STL_TRY_THROW( stlStrToInt64( aStr + 2, */
/*                                           STL_NTS, */
/*                                           & sBufferEndPtr, */
/*                                           10, */
/*                                           & sConvertedValue, */
/*                                           aErrorStack ) == STL_SUCCESS, */
/*                            ERROR_CONVERSION ); */
            
/*             aDtlExpTime->mMinute = (stlInt32)sConvertedValue; */
/*             *(aStr + 2) = '\0'; */

/*             STL_TRY_THROW( stlStrToInt64( aStr + 0, */
/*                                           STL_NTS, */
/*                                           & sBufferEndPtr, */
/*                                           10, */
/*                                           & sConvertedValue, */
/*                                           aErrorStack ) == STL_SUCCESS, */
/*                            ERROR_CONVERSION ); */
            
/*             aDtlExpTime->mHour = (stlInt32)sConvertedValue; */

/*             *aDateType = DTL_DTK_TIME; */

/*             STL_THROW( RAMP_SUCCESS ); */
/*         } */
/*         else */
/*         { */
/*             // Do Nothing */
/*         } */
/*     } */
/*     else */
/*     { */
/*         // Do Nothing */
/*     } */

/*     STL_THROW( RAMP_ERROR_BAD_FORMAT ); */

/*     STL_RAMP( RAMP_SUCCESS ); */
    
/*     return STL_SUCCESS; */

/*     STL_CATCH( RAMP_ERROR_BAD_FORMAT ) */
/*     { */
/*         stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                       DTL_ERRCODE_C_STRING_NOT_VALID_DATE_OR_TIME_OR_TIMESTAMP_LITERAL, */
/*                       NULL, */
/*                       aErrorStack ); */
/*     } */

/*     STL_CATCH( ERROR_CONVERSION ) */
/*     { */
/*         sErrorCode = stlGetLastErrorCode( aErrorStack ); */

/*         switch( sErrorCode ) */
/*         { */
/*             case STL_ERRCODE_OUT_OF_RANGE: */
/*                 { */
/*                     stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                                   DTL_ERRCODE_C_STRING_NOT_VALID_DATE_OR_TIME_OR_TIMESTAMP_LITERAL, */
/*                                   NULL, */
/*                                   aErrorStack ); */
/*                 } */
/*             default: */
/*                 { */
/*                     break; */
/*                 } */
/*         }         */
/*     } */

/*     STL_FINISH; */

/*     return STL_FAILURE; */
/* } */

/* /\** */
/*  * @brief parsing된 구분자를 포함한 date field에 대한 해석 */
/*  * @param[in]  aStr          parsing된 field string */
/*  * @param[in]  aFieldMask    parsing된 field mask */
/*  * @param[in]  aDateOrder    date 출력 order */
/*  *                           <BR> DTL_DATE_ORDER_YMD, DTL_DATE_ORDER_DMY, DTL_DATE_ORDER_MDY */
/*  * @param[out] aTimeMask     mask */
/*  * @param[out] aIsTwoDigits  2-digit year */
/*  * @param[out] aDtlExpTime   dtlExpTime */
/*  * @param[out] aErrorStack   에러 스택 */
/*  *\/ */
/* stlStatus dtdDecodeDate( stlChar       * aStr, */
/*                          stlInt32        aFieldMask, */
/*                          stlInt32        aDateOrder, */
/*                          stlInt32      * aTimeMask, */
/*                          stlBool       * aIsTwoDigits, */
/*                          dtlExpTime    * aDtlExpTime, */
/*                          stlErrorStack * aErrorStack ) */
/* { */
/*     dtlFractionalSecond  sFractionalSecond; */
/*     stlInt32             sNumFields; */
/*     stlInt32             i; */
/*     stlInt32             sLen; */
/*     stlBool              sHaveTextMonth; */
/*     stlInt32             sType; */
/*     stlInt32             sValue; */
/*     stlInt32             sDateMask; */
/*     stlChar            * sField[DTL_MAX_DATE_FIELDS]; */

/*     sNumFields = 0; */
/*     sHaveTextMonth = STL_FALSE; */
/*     sDateMask = 0; */
/*     *aTimeMask = 0; */

/*     /\* parse string *\/ */
/*     while( (*aStr != '\0') && (sNumFields < DTL_MAX_DATE_FIELDS) ) */
/*     { */
/*         /\* skip field separators *\/ */
/*         while( stlIsalnum(*aStr) != STL_TRUE ) */
/*         { */
/*             aStr++; */
/*         } */

/*         sField[sNumFields] = aStr; */

/*         if( stlIsdigit(*aStr) == STL_TRUE ) */
/*         { */
/*             while( stlIsdigit(*aStr) == STL_TRUE ) */
/*             { */
/*                 aStr++; */
/*             } */
/*         } */
/*         else if( stlIsalpha(*aStr) == STL_TRUE ) */
/*         { */
/*             while( stlIsalpha(*aStr) == STL_TRUE ) */
/*             { */
/*                 aStr++; */
/*             } */
/*         } */
/*         else */
/*         { */
/*             // Do Nothing  */
/*         } */

/*         if( *aStr != '\0' ) */
/*         { */
/*             *aStr = '\0'; */
/*             aStr++; */
/*         } */
/*         else */
/*         { */
/*             // Do Nothing */
/*         } */

/*         sNumFields++; */
/*     } */

/*     /\* text fields *\/ */
/*     for( i = 0; i < sNumFields; i++ ) */
/*     { */
/*         if( stlIsalpha(*sField[i]) == STL_TRUE ) */
/*         { */
/*             STL_TRY( dtdDecodeSpecial( i, */
/*                                        sField[i], */
/*                                        & sValue, */
/*                                        & sType, */
/*                                        aErrorStack ) */
/*                      == STL_SUCCESS ); */

/*             if( sType == DTL_IGNORE_DTF ) */
/*             { */
/*                 continue; */
/*             } */
/*             else */
/*             { */
/*                 // Do Nothing */
/*             } */

/*             sDateMask = DTL_DTK_M( sType ); */

/*             switch( sType ) */
/*             { */
/*                 case DTL_MONTH: */
/*                     aDtlExpTime->mMonth = sValue; */
/*                     sHaveTextMonth = STL_TRUE; */
/*                     break; */
/*                 default: */
/*                     STL_THROW( RAMP_ERROR_BAD_FORMAT ); */
/*                     break; */
/*             } */

/*             STL_TRY_THROW( (aFieldMask & sDateMask ) == 0, RAMP_ERROR_BAD_FORMAT ); */

/*             aFieldMask |= sDateMask; */
/*             *aTimeMask |= sDateMask; */

/*             /\* field가 완료됨을 표시. *\/ */
/*             sField[i] = NULL; */
/*         } */
/*         else */
/*         { */
/*             // Do Nothing  */
/*         } */
/*     } */

/*     /\* numeric fields *\/ */
/*     for( i = 0; i < sNumFields; i++ ) */
/*     { */
/*         if( sField[i] == NULL ) */
/*         { */
/*             /\* 이미 처리가 완료된 field *\/ */
/*             continue; */
/*         } */
/*         else */
/*         { */
/*             // Do Nothing  */
/*         } */

/*         sLen = stlStrlen( sField[i] ); */
/*         STL_TRY_THROW( sLen > 0, RAMP_ERROR_BAD_FORMAT ); */

/*         STL_TRY( dtdDecodeNumber( sLen, */
/*                                   sField[i], */
/*                                   sHaveTextMonth, */
/*                                   aFieldMask, */
/*                                   aDateOrder, */
/*                                   & sDateMask, */
/*                                   aDtlExpTime, */
/*                                   & sFractionalSecond, */
/*                                   aIsTwoDigits, */
/*                                   aErrorStack ) */
/*                  == STL_SUCCESS ); */

/*         STL_TRY_THROW( (aFieldMask & sDateMask) == 0, RAMP_ERROR_BAD_FORMAT ); */

/*         aFieldMask |= sDateMask; */
/*         *aTimeMask |= sDateMask; */
/*     } */

/*     STL_TRY_THROW( (aFieldMask & ~(DTL_DTK_M(DTL_DOY) | DTL_DTK_M(DTL_TZ))) == DTL_DTK_DATE_M, */
/*                    RAMP_ERROR_BAD_FORMAT ); */
        
/*     return STL_SUCCESS; */
    
/*     STL_CATCH( RAMP_ERROR_BAD_FORMAT ) */
/*     { */
/*         stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                       DTL_ERRCODE_C_STRING_NOT_VALID_DATE_OR_TIMESTAMP_LITERAL, */
/*                       NULL, */
/*                       aErrorStack ); */
/*     } */
    
/*     STL_FINISH; */
    
/*     return STL_FAILURE; */
/* } */

/**
 * @brief 검색배열을 이용한 text string 해석
 * @param[in]  aField      field 
 * @param[in]  aLowToken   검색 string
 * @param[out] aValue      배열에서 검색된 value
 * @param[out] aValueType  검색배열에서 검색된 value의 type
 * @param[out] aErrorStack 에러 스택 
 */
stlStatus  dtdDecodeSpecial( stlInt32        aField,
                             stlChar       * aLowToken,
                             stlInt32      * aValue,
                             stlInt32      * aValueType,
                             stlErrorStack * aErrorStack )
{
    stlInt32              sType;
    stlInt32              sValue;
    const dtlDateToken  * sDateToken;

    sDateToken = dtlDateTokenCache[aField];
    
    if( (sDateToken == NULL) ||
        (stlStrncmp(aLowToken, sDateToken->mToken, DTL_DATE_TOKEN_MAX_LEN) != 0) )
    {
        sDateToken = dtdDateBinarySearch( aLowToken,
                                          dtlDateTokenTbl,
                                          dtlDateTokenTblSize );
    }
    else
    {
        // Do Nothing 
    }

    if( sDateToken == NULL )
    {
        sType = DTL_UNKNOWN_FIELD;
        sValue = 0;
    }
    else
    {
        dtlDateTokenCache[aField] = sDateToken;
        sType = sDateToken->mType;

        switch( sType )
        {
            case DTL_TZ:
                /* FALL_THROUGH */
            case DTL_DTZ:
                /* FALL_THROUGH */
            case DTL_DTZMOD:
                sValue = DTD_FROM_VAL(sDateToken);
                break;
            default:
                sValue = sDateToken->mValue;
                break;
        }
    }

    *aValue = sValue;
    *aValueType = sType;

    return STL_SUCCESS;
}

/* /\** */
/*  * @brief numeric date value 해석. */
/*  * @param[in]  aFieldLen          field len */
/*  * @param[in]  aStr               field string */
/*  * @param[in]  aHaveTextMonth     string month를 포함하는지 여부 */
/*  * @param[in]  aFieldMask         field mask */
/*  * @param[in]  aDateOrder         date 출력 order */
/*  *                                <BR> DTL_DATE_ORDER_YMD, DTL_DATE_ORDER_DMY, DTL_DATE_ORDER_MDY */
/*  * @param[in]  aTimeMask          mask                                */
/*  * @param[out] aDtlExpTime        dtlExpTime */
/*  * @param[out] aFractionalSecond  fractional second */
/*  * @param[out] aIsTwoDigits       2-digit year */
/*  * @param[out] aErrorStack        에러 스택 */
/*  *\/ */
/* stlStatus dtdDecodeNumber( stlInt32               aFieldLen, */
/*                            stlChar              * aStr, */
/*                            stlBool                aHaveTextMonth, */
/*                            stlInt32               aFieldMask, */
/*                            stlInt32               aDateOrder, */
/*                            stlInt32             * aTimeMask, */
/*                            dtlExpTime           * aDtlExpTime, */
/*                            dtlFractionalSecond  * aFractionalSecond, */
/*                            stlBool              * aIsTwoDigits, */
/*                            stlErrorStack        * aErrorStack ) */
/* { */
/*     stlInt32    sValue; */
/*     stlInt32    sDateType; */
/*     stlInt64    sConvertedValue; */
/*     stlChar   * sBuffer; */
/*     stlUInt32   sErrorCode; */

/*     *aTimeMask = 0; */

/*     STL_TRY_THROW( stlStrToInt64( aStr, */
/*                                   STL_NTS, */
/*                                   & sBuffer, */
/*                                   10, */
/*                                   & sConvertedValue, */
/*                                   aErrorStack ) == STL_SUCCESS, */
/*                    ERROR_CONVERSION ); */
/*     STL_TRY_THROW( (sConvertedValue >= STL_INT32_MIN) && (sConvertedValue <= STL_INT32_MAX), */
/*                    ERROR_FIELD_OVERFLOW ); */

/*     sValue = (stlInt32)sConvertedValue; */

/*     STL_TRY_THROW( sBuffer != aStr, RAMP_ERROR_BAD_FORMAT ); */

/*     if( *sBuffer == '.' ) */
/*     { */
/*         if( (sBuffer - aStr) > 2 ) */
/*         { */
/*             /\* decimal point앞에 2-digit 초과의 숫자가 있다면, date or run-together time */
/*              * 2001.360 20011225 040506.789 */
/*              *\/  */
/*             STL_TRY( dtdDecodeNumberField( aFieldLen, */
/*                                            aStr, */
/*                                            (aFieldMask | DTL_DTK_DATE_M), */
/*                                            aTimeMask, */
/*                                            aDtlExpTime, */
/*                                            aFractionalSecond, */
/*                                            aIsTwoDigits, */
/*                                            & sDateType, */
/*                                            aErrorStack ) */
/*                      == STL_SUCCESS ); */

/*             STL_THROW( RAMP_SUCCESS ); */
/*         } */
/*         else */
/*         { */
/*             // Do Nothing  */
/*         } */

/*         STL_TRY( dtdParseFractionalSecond( sBuffer, */
/*                                            aFractionalSecond, */
/*                                            aErrorStack ) */
/*                  == STL_SUCCESS ); */
/*     } */
/*     else if( *sBuffer != '\0' ) */
/*     { */
/*         STL_THROW( RAMP_ERROR_BAD_FORMAT ); */
/*     } */
/*     else */
/*     { */
/*         // Do Nothing  */
/*     } */

/*     /\* Special case for day of year *\/ */
/*     if( (aFieldLen == 3) && */
/*         ((aFieldMask & DTL_DTK_DATE_M) == DTL_DTK_M(DTL_YEAR)) && */
/*         (sValue >= 1) && */
/*         (sValue <= 366) ) */
/*     { */
/*         *aTimeMask = (DTL_DTK_M(DTL_DOY) | DTL_DTK_M(DTL_MONTH) | DTL_DTK_M(DTL_DAY)); */
/*         aDtlExpTime->mYearDay = sValue; */

/*         STL_THROW( RAMP_SUCCESS ); */
/*     } */
/*     else */
/*     { */
/*         // Do Nothing */
/*     } */

/*     switch( aFieldMask & DTL_DTK_DATE_M ) */
/*     { */
/*         case 0: */
/*             /\* */
/*              * YYYY-MM-DD  or field order는 DateOrder */
/*              *\/ */
/*             { */
/*                 if( (aFieldLen >= 3) || (aDateOrder == DTL_DATE_ORDER_YMD) ) */
/*                 { */
/*                     *aTimeMask = DTL_DTK_M(DTL_YEAR); */
/*                     aDtlExpTime->mYear = sValue; */
/*                 } */
/*                 else if( aDateOrder == DTL_DATE_ORDER_DMY ) */
/*                 { */
/*                     *aTimeMask = DTL_DTK_M(DTL_DAY); */
/*                     aDtlExpTime->mDay = sValue; */
/*                 } */
/*                 else /\* DTL_DATE_ORDER_MDY *\/ */
/*                 { */
/*                     *aTimeMask = DTL_DTK_M(DTL_MONTH); */
/*                     aDtlExpTime->mMonth = sValue; */
/*                 } */
                
/*                 break; */
/*             } */
/*         case ( DTL_DTK_M(DTL_YEAR) ): */
/*             { */
/*                 /\* */
/*                  * YY-MM-DD의 second field  */
/*                  *\/ */
/*                 *aTimeMask = DTL_DTK_M(DTL_MONTH); */
/*                 aDtlExpTime->mMonth = sValue; */
/*                 break; */
/*             } */
/*         case ( DTL_DTK_M(DTL_MONTH) ): */
/*             { */
/*                 if( aHaveTextMonth == STL_TRUE ) */
/*                 { */
/*                     /\* textual month name을 포함한 date의 first numeric field */
/*                      * MON-DD-YYYY, DD-MON-YYYY, and YYYY-MON-DD */
/*                      * MON-DD-YY or DD-MON-YY ( DTL_DATE_ORDER_DMY or DTL_DATE_ORDER_MDY ) */
/*                      * YY-MON-DD ( DTL_DATE_ORDER_YMD ) */
/*                      *\/  */
/*                     if( (aFieldLen >= 3) || (aDateOrder == DTL_DATE_ORDER_YMD) ) */
/*                     { */
/*                         *aTimeMask = DTL_DTK_M(DTL_YEAR); */
/*                         aDtlExpTime->mYear = sValue; */
/*                     } */
/*                     else */
/*                     { */
/*                         *aTimeMask = DTL_DTK_M(DTL_DAY); */
/*                         aDtlExpTime->mDay = sValue; */
/*                     } */
/*                 } */
/*                 else */
/*                 { */
/*                     /\* */
/*                      * MM-DD-YY의 second field */
/*                      *\/ */
/*                     *aTimeMask = DTL_DTK_M(DTL_DAY); */
/*                     aDtlExpTime->mDay = sValue; */
/*                 } */
                
/*                 break; */
/*             } */
/*         case ( DTL_DTK_M(DTL_YEAR) | DTL_DTK_M(DTL_MONTH) ): */
/*             { */
/*                 if( aHaveTextMonth == STL_TRUE ) */
/*                 { */
/*                     /\* */
/*                      * DD-MON-YYYY (DTL_DATE_ORDER_YMD) */
/*                      *\/  */
/*                     if( (aFieldLen >= 3) && (*aIsTwoDigits == STL_TRUE) ) */
/*                     { */
/*                         *aTimeMask = DTL_DTK_M(DTL_DAY); */
/*                         aDtlExpTime->mDay = aDtlExpTime->mYear; */
/*                         aDtlExpTime->mYear = sValue; */
/*                         *aIsTwoDigits = STL_FALSE; */
/*                     } */
/*                     else */
/*                     { */
/*                         *aTimeMask = DTL_DTK_M(DTL_DAY); */
/*                         aDtlExpTime->mDay = sValue; */
/*                     } */
/*                 } */
/*                 else */
/*                 { */
/*                     /\* */
/*                      * YY-MM-DD */
/*                      *\/ */
/*                     *aTimeMask = DTL_DTK_M(DTL_DAY); */
/*                     aDtlExpTime->mDay = sValue; */
/*                 } */
                
/*                 break; */
/*             } */
/*         case ( DTL_DTK_M(DTL_DAY) ): */
/*             { */
/*                 /\* */
/*                  * DD-MM-YY의 second field */
/*                  *\/ */
/*                 *aTimeMask = DTL_DTK_M(DTL_MONTH); */
/*                 aDtlExpTime->mMonth = sValue; */
                
/*                 break; */
/*             } */
/*         case ( DTL_DTK_M(DTL_MONTH) | DTL_DTK_M(DTL_DAY) ): */
/*             { */
/*                 /\* */
/*                  * DD-MM-YY or MM-DD-YY의 thried field */
/*                  *\/ */
/*                 *aTimeMask = DTL_DTK_M(DTL_YEAR); */
/*                 aDtlExpTime->mYear = sValue; */
                
/*                 break; */
/*             } */
/*         case ( DTL_DTK_M(DTL_YEAR) | DTL_DTK_M(DTL_MONTH) | DTL_DTK_M(DTL_DAY) ): */
/*             { */
/*                 /\* date field는 모두 처리되었고, time field에 대한 처리 *\/ */
/*                 STL_TRY( dtdDecodeNumberField( aFieldLen, */
/*                                                aStr, */
/*                                                aFieldMask, */
/*                                                aTimeMask, */
/*                                                aDtlExpTime, */
/*                                                aFractionalSecond, */
/*                                                aIsTwoDigits, */
/*                                                & sDateType, */
/*                                                aErrorStack ) */
/*                          == STL_SUCCESS ); */

/*                 STL_THROW( RAMP_SUCCESS ); */
                
/*                 break; */
/*             } */
/*         default: */
/*             { */
/*                 STL_THROW( RAMP_ERROR_BAD_FORMAT ); */
            
/*                 break; */
/*             } */
/*     } */

/*     if( *aTimeMask == DTL_DTK_M(DTL_YEAR) ) */
/*     { */
/*         *aIsTwoDigits = (aFieldLen <= 2); */
/*     } */
/*     else */
/*     { */
/*         // Do Nothing */
/*     } */

/*     STL_RAMP( RAMP_SUCCESS ); */
    
/*     return STL_SUCCESS; */

/*     STL_CATCH( RAMP_ERROR_BAD_FORMAT ) */
/*     { */
/*         stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                       DTL_ERRCODE_C_STRING_NOT_VALID_DATE_OR_TIME_OR_TIMESTAMP_LITERAL, */
/*                       NULL, */
/*                       aErrorStack ); */
/*     } */

/*     STL_CATCH( ERROR_CONVERSION ) */
/*     { */
/*         sErrorCode = stlGetLastErrorCode( aErrorStack ); */

/*         switch( sErrorCode ) */
/*         { */
/*             case STL_ERRCODE_OUT_OF_RANGE: */
/*                 { */
/*                     stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                                   DTL_ERRCODE_C_STRING_NOT_VALID_DATE_OR_TIME_OR_TIMESTAMP_LITERAL, */
/*                                   NULL, */
/*                                   aErrorStack ); */
/*                 } */
/*             default: */
/*                 { */
/*                     break; */
/*                 } */
/*         }         */
/*     } */

/*     STL_CATCH( ERROR_FIELD_OVERFLOW ) */
/*     { */
/*         stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                       DTL_ERRCODE_C_STRING_NOT_VALID_DATE_OR_TIME_OR_TIMESTAMP_LITERAL, */
/*                       NULL, */
/*                       aErrorStack ); */
/*     } */

/*     STL_FINISH; */

/*     return STL_FAILURE;     */
/* } */

/**
 * @brief parse fractional second value
 * @param[in]  aBuffer           string
 * @param[out] aFractionalSecond fractional second
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtdParseFractionalSecond( stlChar             * aBuffer,
                                    dtlFractionalSecond * aFractionalSecond,
                                    stlErrorStack       * aErrorStack )
{
    stlFloat64   sConvertedValue;

    STL_TRY_THROW( *aBuffer == '.', RAMP_ERROR_INTERNAL_ERROR );
    
    if( *(aBuffer + 1) == '\0' )
    {
        sConvertedValue = 0;
    }
    else
    {
        STL_TRY_THROW( stlStrToFloat64( aBuffer,
                                        STL_NTS,
                                        & aBuffer,
                                        & sConvertedValue,
                                        aErrorStack ) == STL_SUCCESS,
                       RAMP_ERROR_BAD_FORMAT );
        
        STL_TRY_THROW( *aBuffer == '\0', RAMP_ERROR_BAD_FORMAT );
    }
    
    *aFractionalSecond = (dtlFractionalSecond)stlRint(sConvertedValue * 1000000);

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERROR_BAD_FORMAT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_NOT_VALID_TIME_OR_TIMESTAMP_LITERAL,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERROR_INTERNAL_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_NOT_VALID_TIME_OR_TIMESTAMP_LITERAL,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 구분자를 포함한 time string 해석.
 * @param[in]  aStr               field string
 * @param[in]  aFieldMask         field mask
 * @param[in]  aRange             interval range
 * @param[out] aTimeMask          mask
 * @param[out] aDtlExpTime        dtlExpTime
 * @param[out] aFractionalSecond  fractional second
 * @param[out] aErrorStack        에러 스택
 */
stlStatus dtdDecodeTime( stlChar              * aStr,
                         stlInt32               aFieldMask,
                         dtlIntervalIndicator   aRange,
                         stlInt32             * aTimeMask,
                         dtlExpTime           * aDtlExpTime,
                         dtlFractionalSecond  * aFractionalSecond,
                         stlErrorStack        * aErrorStack )
{
    stlChar  * sBuffer;
    stlInt64   sConvertedValue;
    stlUInt32  sErrorCode;
    stlBool    sIsMinuteToSecondStr = STL_FALSE;

    *aTimeMask = DTL_DTK_TIME_M;

    STL_TRY_THROW( stlStrToInt64( aStr,
                                  STL_NTS,
                                  & sBuffer,
                                  10,
                                  & sConvertedValue,
                                  aErrorStack ) == STL_SUCCESS,
                   ERROR_CONVERSION );
    STL_TRY_THROW( (sConvertedValue >= STL_INT32_MIN) && (sConvertedValue <= STL_INT32_MAX),
                   ERROR_FIELD_OVERFLOW );

    aDtlExpTime->mHour = (stlInt32)sConvertedValue;

    STL_TRY_THROW( *sBuffer == ':', RAMP_ERROR_BAD_FORMAT );

    STL_TRY_THROW( stlStrToInt64( sBuffer + 1,
                                  STL_NTS,
                                  & sBuffer,
                                  10,
                                  & sConvertedValue,
                                  aErrorStack ) == STL_SUCCESS,
                   ERROR_CONVERSION );
    STL_TRY_THROW( (sConvertedValue >= STL_INT32_MIN) && (sConvertedValue <= STL_INT32_MAX),
                   ERROR_FIELD_OVERFLOW );

    aDtlExpTime->mMinute = (stlInt32)sConvertedValue;
    
    if( *sBuffer == '\0' )
    {
        /* ex) interval'99:59'minute to second */
        sIsMinuteToSecondStr = STL_TRUE;
        
        aDtlExpTime->mSecond = 0;
        *aFractionalSecond = 0;

        /*
         * MINUTE TO SECOND interval, mm:ss
         */ 
        if( aRange == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND )
        {
            aDtlExpTime->mSecond = aDtlExpTime->mMinute;
            aDtlExpTime->mMinute = aDtlExpTime->mHour;
            aDtlExpTime->mHour = 0;
        }
        else
        {
            // Do Nothing
        }
    }
    else if( *sBuffer == '.' )
    {
        /* ex) interval'99:59.999999'minute to second */
        sIsMinuteToSecondStr = STL_TRUE;
        
        /* mm:ss.sss MINUTE TO SECOND */
        STL_TRY( dtdParseFractionalSecond( sBuffer,
                                           aFractionalSecond,
                                           aErrorStack )
                 == STL_SUCCESS );

        aDtlExpTime->mSecond = aDtlExpTime->mMinute;
        aDtlExpTime->mMinute = aDtlExpTime->mHour;
        aDtlExpTime->mHour = 0;        
    }
    else if( *sBuffer == ':' )
    {
        /* ex) interval'23:59:59.999999' ...  */
        sIsMinuteToSecondStr = STL_FALSE;
        
        STL_TRY_THROW( stlStrToInt64( sBuffer + 1,
                                      STL_NTS,
                                      & sBuffer,
                                      10,
                                      & sConvertedValue,
                                      aErrorStack ) == STL_SUCCESS,
                       ERROR_CONVERSION );
        STL_TRY_THROW( (sConvertedValue >= STL_INT32_MIN) && (sConvertedValue <= STL_INT32_MAX),
                       ERROR_FIELD_OVERFLOW );

        aDtlExpTime->mSecond = (stlInt32)sConvertedValue;

        if( *sBuffer == '\0' )
        {
            *aFractionalSecond = 0;
        }
        else if( *sBuffer == '.' )
        {
            STL_TRY( dtdParseFractionalSecond( sBuffer,
                                               aFractionalSecond,
                                               aErrorStack )
                     == STL_SUCCESS );
        }
        else
        {
            STL_THROW( RAMP_ERROR_BAD_FORMAT );
        }
    }
    else
    {
        STL_THROW( RAMP_ERROR_BAD_FORMAT );
    }

    if( aRange == DTL_INTERVAL_INDICATOR_DAY_TO_SECOND )
    {
        /* Do Nothing */
    }
    else
    {
        switch( aRange )
        {
            case DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND:
                if( sIsMinuteToSecondStr == STL_TRUE )
                {
                    aDtlExpTime->mHour += aDtlExpTime->mMinute / DTL_MINS_PER_HOUR;
                    aDtlExpTime->mMinute =
                        aDtlExpTime->mMinute - (aDtlExpTime->mMinute / DTL_MINS_PER_HOUR * DTL_MINS_PER_HOUR);
                    
                    aDtlExpTime->mDay = aDtlExpTime->mHour / DTL_HOURS_PER_DAY;
                    aDtlExpTime->mHour =
                        aDtlExpTime->mHour - (aDtlExpTime->mHour / DTL_HOURS_PER_DAY * DTL_HOURS_PER_DAY);
                }
                break;
            case DTL_INTERVAL_INDICATOR_HOUR:
            case DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE:
            case DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND:
                if( aDtlExpTime->mHour >= DTL_HOURS_PER_DAY )
                {
                    aDtlExpTime->mDay = aDtlExpTime->mHour / DTL_HOURS_PER_DAY;
                    aDtlExpTime->mHour =
                        aDtlExpTime->mHour - (aDtlExpTime->mHour / DTL_HOURS_PER_DAY * DTL_HOURS_PER_DAY);
                }
                break;
            default:
                /* Do Nothing */
                break;
        }
    }
    
    STL_TRY_THROW(
        ((aDtlExpTime->mHour >= 0) && (aDtlExpTime->mHour < DTL_HOURS_PER_DAY) &&
         (aDtlExpTime->mMinute >= 0) && (aDtlExpTime->mMinute < DTL_MINS_PER_HOUR) &&
         (aDtlExpTime->mSecond >= 0) && (aDtlExpTime->mSecond < DTL_SECS_PER_MINUTE) &&
         (*aFractionalSecond >= STL_INT64_C(0)) && (*aFractionalSecond <= DTL_USECS_PER_SEC))
        == STL_TRUE,
        ERROR_FIELD_OVERFLOW );

    return STL_SUCCESS;

    STL_CATCH( ERROR_CONVERSION )
    {
        sErrorCode = stlGetLastErrorCode( aErrorStack );

        switch( sErrorCode )
        {
            case STL_ERRCODE_OUT_OF_RANGE:
                {
                    stlPushError( STL_ERROR_LEVEL_ABORT,
                                  DTL_ERRCODE_C_STRING_NOT_VALID_TIME_OR_TIMESTAMP_LITERAL,
                                  NULL,
                                  aErrorStack );
                }
            default:
                {
                    break;
                }
        }        
    }

    STL_CATCH( ERROR_FIELD_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_NOT_VALID_TIME_OR_TIMESTAMP_LITERAL,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERROR_BAD_FORMAT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_NOT_VALID_TIME_OR_TIMESTAMP_LITERAL,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;    
}

/* /\** */
/*  * @brief year/month/day 검사, BC and DOY(day of year) */
/*  * @param[in]  aFieldMask    field mask */
/*  * @param[in]  aIsJulian     julian date 여부 */
/*  * @param[in]  aIsTwoDigits  2-digit year 여부 */
/*  * @param[in]  aIsBc         BC 여부  */
/*  * @param[out] aDtlExpTime   dtlExpTime */
/*  * @param[out] aErrorStack   에러 스택 */
/*  *\/ */
/* stlStatus dtdValidateDate( stlInt32         aFieldMask, */
/*                            stlBool          aIsJulian, */
/*                            stlBool          aIsTwoDigits, */
/*                            stlBool          aIsBc, */
/*                            dtlExpTime     * aDtlExpTime, */
/*                            stlErrorStack  * aErrorStack ) */
/* { */
/*     stlInt32   sJulianDate; */
    
/*     if( aFieldMask & DTL_DTK_M(DTL_YEAR) ) */
/*     { */
/*         if( aIsJulian == STL_TRUE ) */
/*         { */
/*             // aDtlExpTime->mYear는 유효한 값임. */
/*             // Do Nothing */
/*         } */
/*         else if( aIsBc == STL_TRUE ) */
/*         { */
/*             /\* AD/BC 표기의 year는 zero가 아님. *\/ */
/*             STL_TRY_THROW( aDtlExpTime->mYear > 0, ERROR_FIELD_OVERFLOW ); */

/*             /\* internally, we represent 1 BC as year zero, 2 BC as -1, etc *\/ */
/*             aDtlExpTime->mYear = -(aDtlExpTime->mYear - 1); */
/*         } */
/*         else if( aIsTwoDigits == STL_TRUE ) */
/*         { */
/*             /\* process 1 or 2-digit input as 1970-2069 AD, allow '0' and '00' *\/ */

/*             STL_TRY_THROW( aDtlExpTime->mYear >= 0, ERROR_FIELD_OVERFLOW ); */

/*             if( aDtlExpTime->mYear < 70 ) */
/*             { */
/*                 aDtlExpTime->mYear += 2000; */
/*             } */
/*             else if( aDtlExpTime->mYear < 100 ) */
/*             { */
/*                 aDtlExpTime->mYear += 1900; */
/*             } */
/*             else */
/*             { */
/*                 // Do Nothing  */
/*             } */
/*         } */
/*         else */
/*         { */
/*             /\* AD/BC 표기의 year는 zero가 아님. *\/ */
/*             STL_TRY_THROW( aDtlExpTime->mYear > 0, ERROR_FIELD_OVERFLOW ); */
/*         } */
/*     } */
/*     else */
/*     { */
/*         // Do Nothing */
/*     } */

/*     /\* decode DOY(day of year) *\/ */
/*     if( (aFieldMask & DTL_DTK_M(DTL_DOY)) != 0 ) */
/*     { */
/*         sJulianDate = dtdDate2JulianDate( aDtlExpTime->mYear, */
/*                                           1, */
/*                                           1 ); */

/*         STL_TRY( dtdJulianDate2Date ( sJulianDate + aDtlExpTime->mYearDay - 1, */
/*                                       & aDtlExpTime->mYear, */
/*                                       & aDtlExpTime->mMonth, */
/*                                       & aDtlExpTime->mDay, */
/*                                       aErrorStack ) */
/*                  == STL_SUCCESS ); */
                 
/*     } */
/*     else */
/*     { */
/*         // Do Nothing */
/*     } */

/*     /\* check valid year *\/ */
/*     if( (aFieldMask & DTL_DTK_M(DTL_YEAR)) != 0 ) */
/*     { */
/*         STL_TRY_THROW( (aDtlExpTime->mYear >= -9999) && */
/*                        (aDtlExpTime->mYear <= 9999), */
/*                        ERROR_MD_FIELD_OVERFLOW ); */
/*     } */
/*     else */
/*     { */
/*         // Do Nothing */
/*     } */

/*     /\* check valid month *\/ */
/*     if( (aFieldMask & DTL_DTK_M(DTL_MONTH)) != 0 ) */
/*     { */
/*         STL_TRY_THROW( (aDtlExpTime->mMonth >= 1) && (aDtlExpTime->mMonth <= DTL_MONTHS_PER_YEAR), */
/*                        ERROR_MD_FIELD_OVERFLOW ); */
/*     } */
/*     else */
/*     { */
/*         // Do Nothing */
/*     } */

/*     /\* check valid day *\/ */
/*     if( (aFieldMask & DTL_DTK_M(DTL_DAY)) != 0 ) */
/*     { */
/*         STL_TRY_THROW( (aDtlExpTime->mDay >= 1) && (aDtlExpTime->mDay <= 31), */
/*                        ERROR_MD_FIELD_OVERFLOW ); */
/*     } */
/*     else */
/*     { */
/*         // Do Nothing */
/*     } */

/*     if( (aFieldMask & DTL_DTK_DATE_M) == DTL_DTK_DATE_M ) */
/*     { */
/*         STL_TRY_THROW( */
/*             (aDtlExpTime->mDay <= */
/*              dtlDayTab[DTL_IS_LEAP(aDtlExpTime->mYear)][aDtlExpTime->mMonth - 1]), */
/*             ERROR_FIELD_OVERFLOW ); */
/*     } */
/*     else */
/*     { */
/*         // Do Nothing */
/*     } */

/*     return STL_SUCCESS; */

/*     STL_CATCH( ERROR_FIELD_OVERFLOW ) */
/*     { */
/*         stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                       DTL_ERRCODE_C_STRING_NOT_VALID_DATE_OR_TIMESTAMP_LITERAL, */
/*                       NULL, */
/*                       aErrorStack ); */
/*     } */

/*     STL_CATCH( ERROR_MD_FIELD_OVERFLOW ) */
/*     { */
/*         stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                       DTL_ERRCODE_C_STRING_NOT_VALID_DATE_OR_TIMESTAMP_LITERAL, */
/*                       NULL, */
/*                       aErrorStack ); */
/*     } */
    
/*     STL_FINISH; */

/*     return STL_FAILURE;     */
/* } */

/* /\** */
/*  * @brief Date value를 string으로 변환. */
/*  * @param[in]  aDtlExpTime  dtlExpTime */
/*  * @param[in]  aDateStyle   date style */
/*  *                          <BR> DTL_DATES_STYLE_ISO, DTL_DATES_STYLE_SQL */
/*  * @param[in]  aDateOrder   date 출력 order */
/*  *                          <BR> DTL_DATE_ORDER_YMD, DTL_DATE_ORDER_DMY, DTL_DATE_ORDER_MDY */
/*  * @param[out] aStr         변환된 string을 저장할 공간 */
/*  * @param[out] aLength      변환된 문자열의 길이 */
/*  * @param[out] aErrorStack  에러 스택 */
/*  *\/ */
/* stlStatus dtdEncodeDateOnly( dtlExpTime    * aDtlExpTime, */
/*                              stlInt32        aDateStyle, */
/*                              stlInt32        aDateOrder, */
/*                              stlChar       * aStr, */
/*                              stlInt32      * aLength, */
/*                              stlErrorStack * aErrorStack ) */
/* { */
/*     stlInt32  sSize = 0; */
    
/*     STL_TRY_THROW( (aDtlExpTime->mMonth >= 1) && (aDtlExpTime->mMonth <= DTL_MONTHS_PER_YEAR), */
/*                    ERROR_INVALID_ARGUMENT ); */

/*     switch( aDateStyle ) */
/*     { */
/*         case DTL_DATES_STYLE_SQL: */
/*             { */
/*                 if( aDateOrder == DTL_DATE_ORDER_DMY ) */
/*                 { */
/*                     sSize += dtdIntegerToLeadingZeroFormatString( aDtlExpTime->mDay, */
/*                                                                   2, */
/*                                                                   aStr + sSize ); */
                    
/*                     *(aStr + sSize) = '/'; */
/*                     sSize++; */
                    
/*                     sSize += dtdIntegerToLeadingZeroFormatString( aDtlExpTime->mMonth, */
/*                                                                   2, */
/*                                                                   aStr + sSize ); */
/*                 } */
/*                 else */
/*                 { */
/*                     sSize += dtdIntegerToLeadingZeroFormatString( aDtlExpTime->mMonth, */
/*                                                                   2, */
/*                                                                   aStr + sSize ); */
                    
/*                     *(aStr + sSize) = '/'; */
/*                     sSize++; */
                    
/*                     sSize += dtdIntegerToLeadingZeroFormatString( aDtlExpTime->mDay, */
/*                                                                   2, */
/*                                                                   aStr + sSize ); */
/*                 } */
                
/*                 if( aDtlExpTime->mYear > 0 ) */
/*                 { */
/*                     *(aStr + sSize) = '/'; */
/*                     sSize++; */
                    
/*                     sSize += dtdIntegerToLeadingZeroFormatString( aDtlExpTime->mYear, */
/*                                                                   4, */
/*                                                                   aStr + sSize ); */
/*                 } */
/*                 else */
/*                 { */
/*                     *(aStr + sSize) = '/'; */
/*                     sSize++; */
                    
/*                     sSize += dtdIntegerToLeadingZeroFormatString( -(aDtlExpTime->mYear - 1), */
/*                                                                   4, */
/*                                                                   aStr + sSize ); */
/*                     *(aStr + sSize) = ' '; */
/*                     sSize++; */
                    
/*                     *(aStr + sSize) = 'B'; */
/*                     sSize++; */
                    
/*                     *(aStr + sSize) = 'C'; */
/*                     sSize++; */
/*                 } */
                
/*                 break; */
/*             } */
/*         case DTL_DATES_STYLE_ISO: */
/*             /\* FALL_THROUGH *\/ */
/*         default: */
/*             { */
/*                 if( aDtlExpTime->mYear > 0 ) */
/*                 { */
/*                     sSize += dtdIntegerToLeadingZeroFormatString( aDtlExpTime->mYear, */
/*                                                                   4, */
/*                                                                   aStr + sSize); */
/*                     *(aStr + sSize) = '-'; */
/*                     sSize++; */

/*                     sSize += dtdIntegerToLeadingZeroFormatString( aDtlExpTime->mMonth, */
/*                                                                   2, */
/*                                                                   aStr + sSize); */

/*                     *(aStr + sSize) = '-'; */
/*                     sSize++; */

/*                     sSize += dtdIntegerToLeadingZeroFormatString( aDtlExpTime->mDay, */
/*                                                                   2, */
/*                                                                   aStr + sSize ); */
/*                 } */
/*                 else */
/*                 { */
/*                     sSize += dtdIntegerToLeadingZeroFormatString( -(aDtlExpTime->mYear - 1), */
/*                                                                   4, */
/*                                                                   aStr + sSize ); */
/*                     *(aStr + sSize) = '-'; */
/*                     sSize++; */

/*                     sSize += dtdIntegerToLeadingZeroFormatString( aDtlExpTime->mMonth, */
/*                                                                   2, */
/*                                                                   aStr + sSize ); */

/*                     *(aStr + sSize) = '-'; */
/*                     sSize++; */

/*                     sSize += dtdIntegerToLeadingZeroFormatString( aDtlExpTime->mDay, */
/*                                                                   2, */
/*                                                                   aStr + sSize ); */

/*                     *(aStr + sSize) = ' '; */
/*                     sSize++; */

/*                     *(aStr + sSize) = 'B'; */
/*                     sSize++; */
                    
/*                     *(aStr + sSize) = 'C'; */
/*                     sSize++; */
/*                 } */
                
/*                 break; */
/*             } */
/*     } */

/*     *aLength = sSize; */

/*     return STL_SUCCESS; */

/*     STL_CATCH( ERROR_INVALID_ARGUMENT ) */
/*     { */
/*         stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                       DTL_ERRCODE_C_DATE_NOT_VALID_DATE, */
/*                       NULL, */
/*                       aErrorStack ); */
/*     } */

/*     STL_FINISH; */

/*     return STL_FAILURE;     */
/* } */

/* /\** */
/*  * @brief time field string에 대한 해석 */
/*  * @param[in]  aTimeStr           Date/Time input string */
/*  * @param[in]  aDataType          data type */
/*  * @param[in]  aField             token단위로 분류된 배열 */
/*  * @param[in]  aFieldType         token단위로 분류된 배열의 type */
/*  * @param[in]  aNumFields         aField와 aFieldType의 수 */
/*  * @param[in]  aDateOrder         date 출력 order */
/*  *                           <BR> DTL_DATE_ORDER_YMD, DTL_DATE_ORDER_DMY, DTL_DATE_ORDER_MDY */
/*  * @param[out] aDateType          type */
/*  * @param[out] aDtlExpTime        dtlExpTime */
/*  * @param[out] aFractionalSecond  fractional second */
/*  * @param[out] aTimeZone          time zone */
/*  * @param[in]  aVectorFunc        Function Vector */
/*  * @param[in]  aVectorArg         Vector Argument */
/*  * @param[out] aErrorStack        에러 스택  */
/*  *\/ */
/* stlStatus dtdDecodeTimeOnly( const stlChar       * aTimeStr, */
/*                              dtlDataType           aDataType, */
/*                              stlChar            ** aField, */
/*                              stlInt32            * aFieldType, */
/*                              stlInt32              aNumFields, */
/*                              stlInt32              aDateOrder, */
/*                              stlInt32            * aDateType, */
/*                              dtlExpTime          * aDtlExpTime, */
/*                              dtlFractionalSecond * aFractionalSecond, */
/*                              stlInt32            * aTimeZone, */
/*                              dtlFuncVector       * aVectorFunc, */
/*                              void                * aVectorArg, */
/*                              stlErrorStack       * aErrorStack ) */
/* { */
/*     stlInt32       sFieldMask; */
/*     stlInt32       sTimeMask; */
/*     stlInt32       sType; */
/*     stlInt32       sPrefixType;  /\* "prefix type" for ISO h04mm05s06 format *\/ */
/*     stlInt32       i; */
/*     stlInt32       sValue; */
/*     stlInt64       sConvertedValue; */
/*     stlFloat64     sConvertedTime; */
/*     stlBool        sIsJulian; */
/*     stlBool        sIsTwoDigits; */
/*     stlBool        sIsBC; */
/*     stlInt32       sHour24; */
/*     stlInt32       sTimeZone; */
/*     stlInt32       sFieldLen; */
/*     stlChar      * sBuffer;     */
/*     stlUInt32      sErrorCode; */

/*     sFieldMask = 0; */
/*     sPrefixType = 0; */
/*     sIsJulian = STL_FALSE; */
/*     sIsTwoDigits = STL_FALSE; */
/*     sIsBC = STL_FALSE; */
/*     sHour24 = DTL_HR24; */

/*     *aDateType = DTL_DTK_TIME; */
/*     aDtlExpTime->mHour = 0; */
/*     aDtlExpTime->mMinute = 0; */
/*     aDtlExpTime->mSecond = 0; */
/*     aDtlExpTime->mIsDaylightSaving = -1; */
/*     *aFractionalSecond = 0; */

/*     if( aTimeZone != NULL ) */
/*     { */
/*         *aTimeZone = 0; */
/*     } */
/*     else */
/*     { */
/*         // Do Nothing */
/*     } */

/*     for( i = 0; i < aNumFields; i++ ) */
/*     { */
/*         switch( aFieldType[i] ) */
/*         { */
/*             case DTL_DTK_DATE:                 */
/*                 { */
/*                     STL_TRY_THROW( aTimeZone != NULL, RAMP_ERROR_BAD_FORMAT ); */

/*                     /\* Under limited circumstances, we will accept a date... ????? *\/ */
/*                     if( (i == 0) && */
/*                         (aNumFields >= 2) && */
/*                         ((aFieldType[aNumFields - 1] == DTL_DTK_DATE) || */
/*                          (aFieldType[1] == DTL_DTK_TIME)) ) */
/*                     { */
/*                         STL_TRY( dtdDecodeDate( aField[i], */
/*                                                 sFieldMask, */
/*                                                 aDateOrder, */
/*                                                 & sTimeMask, */
/*                                                 & sIsTwoDigits, */
/*                                                 aDtlExpTime, */
/*                                                 aErrorStack ) */
/*                                  == STL_SUCCESS ); */
/*                     } */
/*                     else */
/*                     { */
/*                         /\* time and/or time zone *\/ */
                        
/*                         if( stlIsdigit(*aField[i]) == STL_TRUE ) */
/*                         { */
/*                             STL_TRY_THROW( (sFieldMask & DTL_DTK_TIME_M) != DTL_DTK_TIME_M, */
/*                                            RAMP_ERROR_BAD_FORMAT ); */
                            
/*                             sBuffer = stlStrchr(aField[i], '-'); */
/*                             STL_TRY_THROW( sBuffer != NULL, RAMP_ERROR_BAD_FORMAT ); */

/*                             /\* time zone을 얻는다. *\/ */
/*                             STL_TRY( dtdDecodeTimezone( sBuffer, */
/*                                                         aTimeZone, */
/*                                                         aErrorStack ) */
/*                                      == STL_SUCCESS ); */
                            
/*                             *sBuffer = '\0'; */

/*                             /\* string의 나머지 time 처리 *\/ */
/*                             STL_TRY( dtdDecodeNumberField( stlStrlen(aField[i]), */
/*                                                            aField[i], */
/*                                                            (sFieldMask | DTL_DTK_DATE_M ), */
/*                                                            & sTimeMask, */
/*                                                            aDtlExpTime, */
/*                                                            aFractionalSecond, */
/*                                                            & sIsTwoDigits, */
/*                                                            & aFieldType[i], */
/*                                                            aErrorStack ) */
/*                                      == STL_SUCCESS ); */
                            
/*                             sTimeMask |= DTL_DTK_M(DTL_TZ); */
/*                         } */
/*                         else */
/*                         { */
/*                             STL_THROW( RAMP_ERROR_BAD_FORMAT ); */
/*                         } */
/*                     } */
                    
/*                     break; */
/*                 } */
/*             case DTL_DTK_TIME: */
/*                 { */
/*                     STL_TRY( dtdDecodeTime( aField[i], */
/*                                             (sFieldMask | DTL_DTK_DATE_M), */
/*                                             DTL_INTERVAL_INDICATOR_NA, */
/*                                             & sTimeMask, */
/*                                             aDtlExpTime, */
/*                                             aFractionalSecond, */
/*                                             aErrorStack ) */
/*                              == STL_SUCCESS ); */
/*                 } */

/*                 break; */
/*             case DTL_DTK_TZ: */
/*                 { */
/*                     STL_TRY_THROW( aTimeZone != NULL, RAMP_ERROR_BAD_FORMAT ); */

/*                     STL_TRY( dtdDecodeTimezone( aField[i], */
/*                                                 & sTimeZone, */
/*                                                 aErrorStack ) */
/*                              == STL_SUCCESS ); */

/*                     *aTimeZone = sTimeZone; */

/*                     sTimeMask = DTL_DTK_M(DTL_TZ); */
/*                 } */

/*                 break; */
/*             case DTL_DTK_NUMBER: */
/*                 { */
/*                     /\* field label을 포함한 ISO time */
/*                      * ex) "h04m05s06" *\/ */
                    
/*                     if( sPrefixType != 0 ) */
/*                     { */
/*                         /\* Only accept a date under limited circumstances ??? *\/ */
/*                         switch( sPrefixType ) */
/*                         { */
/*                             case DTL_DTK_JULIAN: */
/*                                 /\* FALL_THROUGH *\/ */
/*                             case DTL_DTK_YEAR: */
/*                                 /\* FALL_THROUGH *\/ */
/*                             case DTL_DTK_MONTH: */
/*                                 /\* FALL_THROUGH *\/ */
/*                             case DTL_DTK_DAY: */
/*                                 STL_TRY_THROW( aTimeZone != NULL, RAMP_ERROR_BAD_FORMAT ); */
/*                                 break; */
/*                             default: */
/*                                 break; */
/*                         } */

/*                         STL_TRY( stlStrToInt64( aField[i], */
/*                                                 STL_NTS, */
/*                                                 & sBuffer, */
/*                                                 10, */
/*                                                 & sConvertedValue, */
/*                                                 aErrorStack ) */
/*                                  == STL_SUCCESS ); */

/*                         STL_TRY_THROW( (sConvertedValue >= STL_INT32_MIN) && */
/*                                        (sConvertedValue <= STL_INT32_MAX), */
/*                                        ERROR_CONVERSION ); */

/*                         sValue = (stlInt32)sConvertedValue; */

/*                         if( *sBuffer == '.' ) */
/*                         { */
/*                             switch( sPrefixType ) */
/*                             { */
/*                                 case DTL_DTK_JULIAN: */
/*                                     /\* FALL_THROUGH *\/ */
/*                                 case DTL_DTK_TIME: */
/*                                     /\* FALL_THROUGH *\/ */
/*                                 case DTL_DTK_SECOND: */
/*                                     break; */
/*                                 default: */
/*                                     STL_THROW( RAMP_ERROR_BAD_FORMAT ); */
/*                                     break; */
/*                             } */
/*                         } */
/*                         else if( *sBuffer != '\0' ) */
/*                         { */
/*                             STL_THROW( RAMP_ERROR_BAD_FORMAT ); */
/*                         } */
/*                         else */
/*                         { */
/*                             // Do Nothing */
/*                         } */

/*                         switch( sPrefixType ) */
/*                         { */
/*                             case DTL_DTK_YEAR: */
/*                                 { */
/*                                     aDtlExpTime->mYear = sValue; */
/*                                     sTimeMask = DTL_DTK_M(DTL_YEAR); */
/*                                 } */

/*                                 break; */
/*                             case DTL_DTK_MONTH: */
/*                                 { */
/*                                     if( ((sFieldMask & DTL_DTK_M(DTL_MONTH)) != 0) && */
/*                                         ((sFieldMask & DTL_DTK_M(DTL_HOUR)) != 0) ) */
/*                                     { */
/*                                         /\* month and hour가 이미 처리되었다면, minute*\/ */
/*                                         aDtlExpTime->mMinute = sValue; */
/*                                         sTimeMask = DTL_DTK_M(DTL_MINUTE); */
/*                                     } */
/*                                     else */
/*                                     { */
/*                                         aDtlExpTime->mMonth = sValue; */
/*                                         sTimeMask = DTL_DTK_M(DTL_MONTH); */
/*                                     } */
/*                                 } */

/*                                 break; */
/*                             case DTL_DTK_DAY: */
/*                                 { */
/*                                     aDtlExpTime->mDay = sValue; */
/*                                     sTimeMask = DTL_DTK_M(DTL_DAY); */
/*                                 } */

/*                                 break; */
/*                             case DTL_DTK_HOUR: */
/*                                 { */
/*                                     aDtlExpTime->mHour = sValue; */
/*                                     sTimeMask = DTL_DTK_M(DTL_HOUR); */
/*                                 } */

/*                                 break; */
/*                             case DTL_DTK_MINUTE: */
/*                                 { */
/*                                     aDtlExpTime->mMinute = sValue; */
/*                                     sTimeMask = DTL_DTK_M(DTL_MINUTE); */
/*                                 } */

/*                                 break; */
/*                             case DTL_DTK_SECOND: */
/*                                 { */
/*                                     aDtlExpTime->mSecond = sValue; */
/*                                     sTimeMask = DTL_DTK_M(DTL_SECOND); */

/*                                     if( *sBuffer == '.' ) */
/*                                     { */
/*                                         STL_TRY( dtdParseFractionalSecond( sBuffer, */
/*                                                                            aFractionalSecond, */
/*                                                                            aErrorStack ) */
/*                                                  == STL_SUCCESS ); */

/*                                         sTimeMask = DTL_DTK_ALL_SECS_M; */
/*                                     } */
/*                                     else */
/*                                     { */
/*                                         // Do Nothing */
/*                                     } */
/*                                 } */

/*                                 break; */
/*                             case DTL_DTK_TZ: */
/*                                 { */
/*                                     sTimeMask = DTL_DTK_M(DTL_TZ); */

/*                                     STL_TRY( dtdDecodeTimezone( aField[i], */
/*                                                                 aTimeZone, */
/*                                                                 aErrorStack ) */
/*                                              == STL_SUCCESS ); */
/*                                 } */

/*                                 break; */
/*                             case DTL_DTK_JULIAN: */
/*                                 { */
/*                                     STL_TRY_THROW( sValue >= 0, ERROR_FIELD_OVERFLOW ); */

/*                                     sTimeMask = DTL_DTK_DATE_M; */

/*                                     STL_TRY( dtdJulianDate2Date ( sValue, */
/*                                                                   & aDtlExpTime->mYear, */
/*                                                                   & aDtlExpTime->mMonth, */
/*                                                                   & aDtlExpTime->mDay, */
/*                                                                   aErrorStack ) */
/*                                              == STL_SUCCESS ); */

/*                                     sIsJulian = STL_TRUE; */

/*                                     if( *sBuffer == '.' ) */
/*                                     { */
/*                                         STL_TRY_THROW( stlStrToInt64( sBuffer + 1, */
/*                                                                       STL_NTS, */
/*                                                                       & sBuffer, */
/*                                                                       10, */
/*                                                                       & sConvertedValue, */
/*                                                                       aErrorStack ) */
/*                                                        == STL_SUCCESS, */
/*                                                        RAMP_ERROR_BAD_FORMAT ); */
                                        
/*                                         STL_TRY_THROW( *sBuffer == '\0', RAMP_ERROR_BAD_FORMAT ); */
                                        
/*                                         sConvertedTime *= DTL_SECS_PER_DAY; */
                                        
/*                                         dtdTimestamp2Time( sConvertedValue, */
/*                                                            & aDtlExpTime->mHour, */
/*                                                            & aDtlExpTime->mMinute, */
/*                                                            & aDtlExpTime->mSecond, */
/*                                                            aFractionalSecond ); */
                                        
/*                                         sTimeMask |= DTL_DTK_TIME_M; */
/*                                     } */
/*                                     else */
/*                                     { */
/*                                         // Do Nothing  */
/*                                     } */
/*                                 } */

/*                                 break; */
/*                             case DTL_DTK_TIME: */
/*                                 { */
/*                                     /\* 이전 field : ISO time 't'*\/ */
/*                                     STL_TRY( dtdDecodeNumberField( stlStrlen(aField[i]), */
/*                                                                    aField[i], */
/*                                                                    (sFieldMask | DTL_DTK_DATE_M), */
/*                                                                    & sTimeMask, */
/*                                                                    aDtlExpTime, */
/*                                                                    aFractionalSecond, */
/*                                                                    & sIsTwoDigits, */
/*                                                                    & aFieldType[i], */
/*                                                                    aErrorStack ) */
/*                                              == STL_SUCCESS ); */

/*                                     STL_TRY_THROW( sTimeMask == DTL_DTK_TIME_M, RAMP_ERROR_BAD_FORMAT ); */
/*                                 } */
                                
/*                                 break; */
/*                             default: */
/*                                 { */
/*                                     STL_THROW( RAMP_ERROR_BAD_FORMAT ); */
/*                                 } */
                                
/*                                 break; */
/*                         } */

/*                         sPrefixType = 0; */
/*                         *aDateType = DTL_DTK_DATE; */
/*                     } */
/*                     else */
/*                     { */
/*                         sFieldLen = stlStrlen( aField[i] ); */
/*                         sBuffer = stlStrchr( aField[i], '.' ); */

/*                         if( sBuffer != NULL ) */
/*                         { */
/*                             if( (i == 0) && */
/*                                 (aNumFields >= 2) && */
/*                                 (aFieldType[aNumFields - 1] == DTL_DTK_DATE) ) */
/*                             { */
/*                                 /\* Under limited circumstances *\/ */
/*                                 STL_TRY( dtdDecodeDate( aField[i], */
/*                                                         sFieldMask, */
/*                                                         aDateOrder, */
/*                                                         & sTimeMask, */
/*                                                         & sIsTwoDigits, */
/*                                                         aDtlExpTime, */
/*                                                         aErrorStack ) */
/*                                          == STL_SUCCESS ); */
/*                             } */
/*                             else if( (sFieldLen - stlStrlen(sBuffer)) > 2 ) */
/*                             { */
/*                                 /\* decimal point전 numeric value 존재 */
/*                                  * ex) 2001.360 or 20011225 040506.789 or 20011223 or 040506 *\/ */
/*                                 STL_TRY( dtdDecodeNumberField( sFieldLen, */
/*                                                                aField[i], */
/*                                                                (sFieldMask | DTL_DTK_DATE_M), */
/*                                                                & sTimeMask, */
/*                                                                aDtlExpTime, */
/*                                                                aFractionalSecond, */
/*                                                                & sIsTwoDigits, */
/*                                                                & aFieldType[i], */
/*                                                                aErrorStack ) */
/*                                          == STL_SUCCESS ); */
/*                             } */
/*                             else */
/*                             { */
/*                                 STL_THROW( RAMP_ERROR_BAD_FORMAT ); */
/*                             } */
/*                         } */
/*                         else if( sFieldLen > 4 ) */
/*                         { */
/*                             STL_TRY( dtdDecodeNumberField( sFieldLen, */
/*                                                            aField[i], */
/*                                                            (sFieldMask | DTL_DTK_DATE_M), */
/*                                                            & sTimeMask, */
/*                                                            aDtlExpTime, */
/*                                                            aFractionalSecond, */
/*                                                            & sIsTwoDigits, */
/*                                                            & aFieldType[i], */
/*                                                            aErrorStack ) */
/*                                      == STL_SUCCESS ); */
/*                         } */
/*                         else */
/*                         { */
/*                             /\* single date/time field *\/ */
/*                             STL_TRY( dtdDecodeNumber( sFieldLen, */
/*                                                       aField[i], */
/*                                                       STL_FALSE, /\* aHaveTextMonth *\/ */
/*                                                       (sFieldMask | DTL_DTK_DATE_M), */
/*                                                       aDateOrder, */
/*                                                       & sTimeMask, */
/*                                                       aDtlExpTime, */
/*                                                       aFractionalSecond, */
/*                                                       & sIsTwoDigits, */
/*                                                       aErrorStack ) */
/*                                      == STL_SUCCESS ); */
/*                         } */
/*                     } */
/*                 } */

/*                 break; */
/*             case DTL_DTK_STRING: */
/*                 /\* FALL_THROUGH *\/ */
/*             case DTL_DTK_SPECIAL: */
/*                 { */
/*                     STL_TRY( dtdDecodeSpecial( i, */
/*                                                aField[i], */
/*                                                & sValue, */
/*                                                & sType, */
/*                                                aErrorStack ) */
/*                              == STL_SUCCESS ); */

/*                     if( sType == DTL_IGNORE_DTF ) */
/*                     { */
/*                         continue; */
/*                     } */
/*                     else */
/*                     { */
/*                         // Do Nothing */
/*                     } */

/*                     sTimeMask = DTL_DTK_M(sType); */

/*                     switch( sType ) */
/*                     { */
/*                         case DTL_RESERV: */
/*                             { */
/*                                 switch( sValue ) */
/*                                 { */
/*                                     case DTL_DTK_ZULU: */
/*                                         { */
/*                                             sTimeMask = (DTL_DTK_TIME_M | DTL_DTK_M(DTL_TZ)); */
/*                                             *aDateType = DTL_DTK_TIME; */
/*                                             aDtlExpTime->mHour = 0; */
/*                                             aDtlExpTime->mMinute = 0; */
/*                                             aDtlExpTime->mSecond = 0; */
/*                                             aDtlExpTime->mIsDaylightSaving = 0; */
/*                                         } */

/*                                         break; */
/*                                     default: */
/*                                         STL_THROW( RAMP_ERROR_BAD_FORMAT ); */
/*                                         break; */
/*                                 } */
/*                             } */

/*                             break; */
/*                         case DTL_DTZMOD: */
/*                             { */
/*                                 /\* daylight savings time modifier (solves "MET DST" syntax) *\/ */
/*                                 sTimeMask |= DTL_DTK_M(DTL_DTZ); */
/*                                 aDtlExpTime->mIsDaylightSaving = 1; */

/*                                 STL_TRY_THROW( aTimeZone != NULL, RAMP_ERROR_BAD_FORMAT ); */

/*                                 *aTimeZone += sValue * DTL_MINS_PER_HOUR; */
/*                             } */

/*                             break; */
/*                         case DTL_DTZ: */
/*                             { */
/*                                 sTimeMask |= DTL_DTK_M(DTL_TZ); */
/*                                 aDtlExpTime->mIsDaylightSaving = 1; */

/*                                 STL_TRY_THROW( aTimeZone != NULL, RAMP_ERROR_BAD_FORMAT ); */

/*                                 *aTimeZone = sValue * DTL_MINS_PER_HOUR; */

/*                                 aFieldType[i] = DTL_DTK_TZ; */
/*                             } */

/*                             break; */
/*                         case DTL_TZ: */
/*                             { */
/*                                 aDtlExpTime->mIsDaylightSaving = 0; */

/*                                 STL_TRY_THROW( aTimeZone != NULL, RAMP_ERROR_BAD_FORMAT ); */

/*                                 *aTimeZone = sValue * DTL_MINS_PER_HOUR; */

/*                                 aFieldType[i] = DTL_DTK_TZ; */
/*                             } */

/*                             break; */
/*                         case DTL_IGNORE_DTF: */
/*                             break; */
/*                         case DTL_AMPM: */
/*                             { */
/*                                 sHour24 = sValue; */
/*                             } */
                            
/*                             break; */
/*                         case DTL_ADBC: */
/*                             { */
/*                                 if( sValue == DTL_BC ) */
/*                                 { */
/*                                     sIsBC = STL_TRUE; */
/*                                 } */
/*                                 else */
/*                                 { */
/*                                     // Do Nothing */
/*                                 } */
/*                             } */

/*                             break; */
/*                         case DTL_UNITS: */
/*                             { */
/*                                 sTimeMask = 0; */
/*                                 sPrefixType = sValue; */
/*                             } */

/*                             break; */
/*                         case DTL_ISOTIME: */
/*                             { */
/*                                 sTimeMask = 0; */

/*                                 /\* */
/*                                  * DTL_DTK_NUMBER => hhmmss.fff */
/*                                  * DTL_DTK_TIME   => hh:mm:ss.fff */
/*                                  * DTL_DTK_DATE   => hhmmss-zz */
/*                                  *\/ */

/*                                 if( (i >= (aNumFields - 1)) || */
/*                                     ((aFieldType[i + 1] != DTL_DTK_NUMBER) && */
/*                                      (aFieldType[i + 1] != DTL_DTK_TIME) && */
/*                                      (aFieldType[i + 1] != DTL_DTK_DATE)) ) */
/*                                 { */
/*                                     STL_THROW( RAMP_ERROR_BAD_FORMAT ); */
/*                                 } */
/*                                 else */
/*                                 { */
/*                                     // Do Nothing */
/*                                 } */

/*                                 sPrefixType = sValue; */
/*                             } */

/*                             break; */
/*                         case DTL_UNKNOWN_FIELD: */
/*                             {                                 */
/*                                 /\* all-alpha timezone name인지 검사 *\/ */
/*                                 STL_THROW( RAMP_ERROR_BAD_FORMAT ); */
/*                             } */

/*                             break; */
/*                         default: */
/*                             STL_THROW( RAMP_ERROR_BAD_FORMAT ); */
/*                             break; */
/*                     } */
/*                 } */

/*                 break; */
/*             default: */
/*                 STL_THROW( RAMP_ERROR_BAD_FORMAT ); */
/*                 break; */
/*         } */

/*         STL_TRY_THROW( (sTimeMask & sFieldMask) == 0, RAMP_ERROR_BAD_FORMAT ); */

/*         sFieldMask |= sTimeMask; */
/*     } */

/*     /\* Y/M/D field *\/ */
/*     STL_TRY( dtdValidateDate( sFieldMask, */
/*                               sIsJulian, */
/*                               sIsTwoDigits, */
/*                               sIsBC, */
/*                               aDtlExpTime, */
/*                               aErrorStack ) */
/*              == STL_SUCCESS ); */

/*     /\* AM/PM *\/ */
/*     if( (sHour24 != DTL_HR24) && (aDtlExpTime->mHour > 12) ) */
/*     { */
/*         STL_THROW( ERROR_FIELD_OVERFLOW ); */
/*     } */
/*     else */
/*     { */
/*         // Do Nothing */
/*     } */

/*     if( (sHour24 == DTL_AM) && (aDtlExpTime->mHour == 12) ) */
/*     { */
/*         aDtlExpTime->mHour = 0; */
/*     } */
/*     else if( (sHour24 == DTL_PM) && (aDtlExpTime->mHour != 12) ) */
/*     { */
/*         aDtlExpTime->mHour += 12;         */
/*     } */
/*     else */
/*     { */
/*         // Do Nothing */
/*     } */

/*     if( (aDtlExpTime->mHour < 0)   || */
/*         (aDtlExpTime->mMinute < 0) || (aDtlExpTime->mMinute > 59) || */
/*         (aDtlExpTime->mSecond < 0) || (aDtlExpTime->mSecond > 59) || */
/*         (aDtlExpTime->mHour > 23)  || */
/*         (*aFractionalSecond < 0) || (*aFractionalSecond > DTL_USECS_PER_SEC) ) */
/*     { */
/*         STL_THROW( ERROR_FIELD_OVERFLOW ); */
/*     } */
/*     else */
/*     { */
/*         // Do Nothing */
/*     } */

/*     STL_TRY_THROW( (sFieldMask & DTL_DTK_TIME_M) == DTL_DTK_TIME_M, RAMP_ERROR_BAD_FORMAT ); */

/*     /\* timezone이 명시되지 않았다면, local timezone을 찾는다. *\/ */
/*     if( (aTimeZone != NULL) && ((sFieldMask & DTL_DTK_M(DTL_TZ)) == 0) ) */
/*     {         */
/*         /\* daylight savings time modifier but no standard timezone? then error ??? *\/ */
/*         STL_TRY_THROW( (sFieldMask & (DTL_DTK_M(DTL_DTZMOD))) == 0, RAMP_ERROR_BAD_FORMAT ); */

/*         *aTimeZone = -(aVectorFunc->mGetGMTOffsetFunc(aVectorArg));         */
/*     } */
/*     else */
/*     { */
/*         // Do Nothing */
/*     } */

/*     return STL_SUCCESS; */

/*     STL_CATCH( RAMP_ERROR_BAD_FORMAT ) */
/*     { */
/*         stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                       DTL_ERRCODE_C_STRING_NOT_VALID_TIME_OR_TIMESTAMP_LITERAL, */
/*                       NULL, */
/*                       aErrorStack ); */
/*     } */

/*     STL_CATCH( ERROR_CONVERSION ) */
/*     { */
/*         sErrorCode = stlGetLastErrorCode( aErrorStack ); */

/*         switch( sErrorCode ) */
/*         { */
/*             case STL_ERRCODE_OUT_OF_RANGE: */
/*                 { */
/*                     stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                                   DTL_ERRCODE_C_STRING_NOT_VALID_TIME_OR_TIMESTAMP_LITERAL, */
/*                                   NULL, */
/*                                   aErrorStack ); */
/*                 } */
/*             default: */
/*                 { */
/*                     break; */
/*                 } */
/*         }         */
/*     } */
    
/*     STL_CATCH( ERROR_FIELD_OVERFLOW ) */
/*     { */
/*         stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                       DTL_ERRCODE_C_STRING_NOT_VALID_TIME_OR_TIMESTAMP_LITERAL, */
/*                       NULL, */
/*                       aErrorStack ); */
/*     } */
    
/*     STL_FINISH; */

/*     return STL_FAILURE; */
/* } */

/**
 * @brief dtlExpTime을 dtlTimeType으로 변환
 * @param[in]  aDtlExpTime        dtlExpTime
 * @param[in]  aFractionalSecond  fractional second
 * @param[out] aTimeType          dtlTimeType
 */
void dtdDtlExpTime2Time( dtlExpTime          * aDtlExpTime,
                         dtlFractionalSecond   aFractionalSecond,
                         dtlTimeType         * aTimeType )
{
    *aTimeType = ((((aDtlExpTime->mHour * DTL_MINS_PER_HOUR + aDtlExpTime->mMinute)
                    * DTL_SECS_PER_MINUTE)
                   + aDtlExpTime->mSecond)
                  * DTL_USECS_PER_SEC) + aFractionalSecond;
}

/**
 * @brief dtlTimeType을 precision(fractional second precision)에 맞게 조정.
 * @param[in,out] aTimeType       dtlTimeType value
 * @param[in]     aTimePrecision  dtlTimeType의 precision(fractional second precision)
 * @param[out]    aErrorStack     에러스택 
 */
stlStatus dtdAdjustTime( dtlTimeType   * aTimeType,
                         stlInt32        aTimePrecision,
                         stlErrorStack * aErrorStack )
{
    STL_TRY_THROW( (aTimePrecision >= DTL_TIME_MIN_PRECISION) &&
                   (aTimePrecision <= DTL_TIME_MAX_PRECISION),
                   ERROR_INVALID_PARAMETER_VALUE );
    
    if( *aTimeType >= STL_INT64_C(0) )
    {
        *aTimeType = ((((*aTimeType) + dtlTimeTypeOffset[aTimePrecision])
                       / dtlTimeTypeScale[aTimePrecision])
                      * dtlTimeTypeScale[aTimePrecision]);
    }
    else
    {
        *aTimeType = -((((-*aTimeType) + dtlTimeTypeOffset[aTimePrecision])
                        / dtlTimeTypeScale[aTimePrecision])
                       * dtlTimeTypeScale[aTimePrecision]);
    }

    return STL_SUCCESS;

    STL_CATCH( ERROR_INVALID_PARAMETER_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_TIME_NOT_VALID_TIME_FORMAT,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief dtlTimeType을 dtlExpTime으로 변환
 * @param[in]  aTimeType          dtlTimeType value
 * @param[out] aDtlExpTime        dtlExpTime
 * @param[out] aFractionalSecond  fractional second
 */
void dtdTime2dtlExpTime( dtlTimeType           aTimeType,
                         dtlExpTime          * aDtlExpTime,
                         dtlFractionalSecond * aFractionalSecond )
{
    aDtlExpTime->mHour = aTimeType / DTL_USECS_PER_HOUR;
    aTimeType -= aDtlExpTime->mHour * DTL_USECS_PER_HOUR;
    
    aDtlExpTime->mMinute = aTimeType / DTL_USECS_PER_MINUTE;
    aTimeType -= aDtlExpTime->mMinute * DTL_USECS_PER_MINUTE;
    
    aDtlExpTime->mSecond = aTimeType / DTL_USECS_PER_SEC;
    aTimeType -= aDtlExpTime->mSecond * DTL_USECS_PER_SEC;
    
    *aFractionalSecond = aTimeType;
}

/* /\** */
/*  * @brief Time을 string으로 변환. */
/*  * @param[in]  aDtlExpTime        dtlExpTime */
/*  * @param[in]  aFractionalSecond  fractional second */
/*  * @param[in]  aTimeZone          timezone */
/*  * @param[in]  aPrecision         precision ( fractional precision ) */
/*  * @param[in]  aFillZeros         fractional second에 precision만큼 0으로 채울지의 여부  */
/*  * @param[out] aStr               변환된 string을 저장할 공간 */
/*  * @param[out] aLength            변환된 문자열의 길이 */
/*  * @param[out] aErrorStack        에러 스택  */
/*  *  */
/*  *\/ */
/* stlStatus dtdEncodeTimeOnly( dtlExpTime          * aDtlExpTime, */
/*                              dtlFractionalSecond   aFractionalSecond, */
/*                              stlInt32            * aTimeZone, */
/*                              stlInt32              aPrecision, */
/*                              stlBool               aFillZeros, */
/*                              stlChar             * aStr, */
/*                              stlInt32            * aLength, */
/*                              stlErrorStack       * aErrorStack ) */
/* { */
/*     stlInt32  sSize = 0; */
    
/*     sSize += dtdIntegerToLeadingZeroFormatString( aDtlExpTime->mHour, */
/*                                                   2, */
/*                                                   aStr + sSize ); */
    
/*     *(aStr + sSize) = ':'; */
/*     sSize++; */
    
/*     sSize += dtdIntegerToLeadingZeroFormatString( aDtlExpTime->mMinute, */
/*                                                   2, */
/*                                                   aStr + sSize ); */
    
/*     *(aStr + sSize) = ':'; */
/*     sSize++; */

/*     sSize += dtdAppendSecondsForWithTimeZone( aStr + sSize, */
/*                                               aDtlExpTime->mSecond, */
/*                                               aFractionalSecond, */
/*                                               aPrecision,                                */
/*                                               aFillZeros ); */
    
/*     if( aTimeZone != NULL ) */
/*     { */
/*         sSize += dtdEncodeTimeZone( aStr + sSize, */
/*                                     *aTimeZone ); */
/*     } */
/*     else */
/*     { */
/*         // Do Nothing */
/*     } */

/*     *aLength = sSize; */

/*     return STL_SUCCESS; */

/* //    STL_FINISH; */

/* //    return STL_FAILURE; */
/* } */

/* /\** */
/*  * @brief numeric timezone offset을 string으로 변환 */
/*  * @param[out]  aStr       변환된 string을 저장할 공간 */
/*  * @param[in]   aTimeZone  timezone offset */
/*  * @return 변환된 문자열의 길이 */
/*  *\/ */
/* stlInt32 dtdEncodeTimeZone( stlChar  * aStr, */
/*                             stlInt32   aTimeZone ) */
/* {     */
/*     stlInt32  sHour; */
/*     stlInt32  sMinute; */
/*     stlInt32  sSecond; */
/*     stlInt32  sSize = 0; */
    
/*     sSecond = (aTimeZone < 0) ? -aTimeZone : aTimeZone; */
/*     sMinute = sSecond / DTL_SECS_PER_MINUTE;     */
/*     sSecond -= sMinute * DTL_SECS_PER_MINUTE; */
/*     sHour = sMinute / DTL_MINS_PER_HOUR; */
/*     sMinute -= sHour * DTL_MINS_PER_HOUR; */

/*     /\* TZ is negated compared to sign we wish to display ... *\/ */
    
/*     *(aStr + sSize) = ' '; */
/*     sSize++; */
    
/*     if( aTimeZone <= 0 ) */
/*     { */
/*         *(aStr + sSize) = '+'; */
/*         sSize++; */
/*     } */
/*     else */
/*     { */
/*         *(aStr + sSize) = '-'; */
/*         sSize++; */
/*     } */

/*     sSize += dtdIntegerToLeadingZeroFormatString( sHour, */
/*                                                   2, */
/*                                                   aStr + sSize ); */

/*     *(aStr + sSize) = ':'; */
/*     sSize++; */

/*     sSize += dtdIntegerToLeadingZeroFormatString( sMinute, */
/*                                                   2, */
/*                                                   aStr + sSize ); */
    
/*     return sSize; */
/* } */

/**
 * @brief dtlExpTime을 dtlTimeTz type으로 변환
 * @param[in]  aDtlExpTime        dtlExpTime
 * @param[in]  aFractionalSecond  fractional second
 * @param[in]  aTimeZone          timezone
 * @param[out] aTimeTzType        dtlTimeTzType value
 */
void dtdDtlExpTime2TimeTz( dtlExpTime          * aDtlExpTime,
                           dtlFractionalSecond   aFractionalSecond,
                           stlInt32              aTimeZone,
                           dtlTimeTzType       * aTimeTzType )
{
    aTimeTzType->mTime = ((((aDtlExpTime->mHour * DTL_MINS_PER_HOUR + aDtlExpTime->mMinute)
                            * DTL_SECS_PER_MINUTE)
                           + aDtlExpTime->mSecond)
                          * DTL_USECS_PER_SEC) + aFractionalSecond;
    
    /*
     * UTC time으로 dtlTimeTz.mTime를 저장한다.
     * 예) '09:00:00 +09:00'은 
     *     mTimestamp에 '00:00:00' ( 09:00를 뺀 UTC time으로 저장됨 )이 
     *     mTimeZone 에 '+09:00'이 저장된다.
     */ 
    dtdTime2Local( aTimeTzType->mTime,
                   -aTimeZone,
                   & (aTimeTzType->mTime) );
    
    aTimeTzType->mTimeZone = aTimeZone;
}

/**
 * @brief dtlTimeTzType을 dtlExpTime으로 변환
 * @param[in]  aTimeTzType        dtlTimeTzType value
 * @param[out] aDtlExpTime        dtlExpTime
 * @param[out] aFractionalSecond  fractional second
 * @param[out] aTimeZone          timezone offset
 */
void dtdTimeTz2dtlExpTime( dtlTimeTzType        * aTimeTzType,
                           dtlExpTime           * aDtlExpTime,
                           dtlFractionalSecond  * aFractionalSecond,
                           stlInt32             * aTimeZone )
{
    dtlTimeOffset  sTimeOffset;
    
    sTimeOffset = aTimeTzType->mTime - (aTimeTzType->mTimeZone * DTL_USECS_PER_SEC);

    aDtlExpTime->mHour = sTimeOffset / DTL_USECS_PER_HOUR;
    sTimeOffset -= aDtlExpTime->mHour * DTL_USECS_PER_HOUR;
    aDtlExpTime->mMinute = sTimeOffset / DTL_USECS_PER_MINUTE;
    sTimeOffset -= aDtlExpTime->mMinute * DTL_USECS_PER_MINUTE;
    aDtlExpTime->mSecond = sTimeOffset / DTL_USECS_PER_SEC;

    *aFractionalSecond = sTimeOffset - aDtlExpTime->mSecond * DTL_USECS_PER_SEC;

    if( aTimeZone != NULL )
    {
        *aTimeZone = aTimeTzType->mTimeZone;
    }
    else
    {
        // Do Nothing
    }
}


/**
 * @brief dtlExpTime을 datetimeoffset( stlInt64 ) value로 변환 ( DATE, TIMESTAMP type으로 변환시 호출됨. ) 
 * @param[in]  aDtlExpTime        dtlExpTime
 * @param[in]  aFractionalSecond  fractional second
 * @param[out] aDateTimeOffset    dtlDateType or dtlTimestamp value
 * @param[out] aErrorStack        에러 스택 
 */
stlStatus dtdDtlExpTime2DateTimeOffset( dtlExpTime          * aDtlExpTime,
                                        dtlFractionalSecond   aFractionalSecond,
                                        dtlTimeOffset       * aDateTimeOffset,
                                        stlErrorStack       * aErrorStack )
{
    dtlTimeOffset   sDate;
    dtlTimeOffset   sTime;

    STL_TRY_THROW( DTL_IS_VALID_JULIAN(aDtlExpTime->mYear,
                                       aDtlExpTime->mMonth,
                                       aDtlExpTime->mDay) == STL_TRUE,
                   ERROR_OUT_OF_RANGE );

    sDate = dtdDate2JulianDate( aDtlExpTime->mYear,
                                aDtlExpTime->mMonth,
                                aDtlExpTime->mDay );
    sDate = sDate - DTL_EPOCH_JDATE;

    dtdTime2TimeOffset( aDtlExpTime->mHour,
                        aDtlExpTime->mMinute,
                        aDtlExpTime->mSecond,
                        aFractionalSecond,
                        & sTime );

    *aDateTimeOffset = sDate * DTL_USECS_PER_DAY + sTime;
    
    /* check overflow */
    STL_TRY_THROW( ((*aDateTimeOffset - sTime) / DTL_USECS_PER_DAY) == sDate,
                   ERROR_OUT_OF_RANGE );
    
    if( ((*aDateTimeOffset < 0) && (sDate >= 0)) ||
        ((*aDateTimeOffset >= 0) && (sDate < 0)) )
    {
        STL_THROW( ERROR_OUT_OF_RANGE );
    }
    else
    {
        // Do Nothing
    }

    return STL_SUCCESS;
    
    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_NOT_VALID_DATE_OR_TIMESTAMP_LITERAL,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief dtlExpTime을 dtlTimestampTzType value로 변환
 * @param[in]  aStr               date/time input string
 * @param[in]  aDtlExpTime        dtlExpTime
 * @param[in]  aFractionalSecond  fractional second
 * @param[in]  aTimeZone          timezone
 * @param[out] aTimestampTzType   dtlTimestampTzType value
 * @param[out] aErrorStack        에러 스택 
 */
stlStatus dtdDtlExpTime2TimestampTz( const stlChar       * aStr,
                                     dtlExpTime          * aDtlExpTime,
                                     dtlFractionalSecond   aFractionalSecond,
                                     stlInt32            * aTimeZone,
                                     dtlTimestampTzType  * aTimestampTzType,
                                     stlErrorStack       * aErrorStack )
{
    /*
     * dtlTimestampTz.mTimestamp 
     */ 
    STL_TRY( dtdDtlExpTime2DateTimeOffset( aDtlExpTime,
                                           aFractionalSecond,
                                           & (aTimestampTzType->mTimestamp),
                                           aErrorStack )
             == STL_SUCCESS );
    
    /*
     * UTC time으로 dtlTimestampTz.mTimestamp를 저장한다.
     * 예) '1999-01-01 09:00:00 +09:00'은 
     *     mTimestamp에 '1999-01-01 00:00:00' ( 09:00를 뺀 UTC time으로 저장됨 )이 
     *     mTimeZone 에 '+09:00'이 저장된다.
     */ 
    if( aTimeZone != NULL )
    {
        dtdTimestamp2Local( aTimestampTzType->mTimestamp,
                            -(*aTimeZone),
                            & (aTimestampTzType->mTimestamp) );
        /*
         * dtlTimestamp.mTimeZone
         */ 
        aTimestampTzType->mTimeZone = *aTimeZone;
    }
    else
    {
        aTimestampTzType->mTimeZone = 0;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief time value를 dtlTimeOffset으로 변환
 * @param[in]  aHour              hour
 * @param[in]  aMinute            minute
 * @param[in]  aSecond            second
 * @param[in]  aFractionalSecond  fractional second
 * @param[out] aTimeOffset        dtlTimeOffset
 */
void dtdTime2TimeOffset( stlInt32              aHour,
                         stlInt32              aMinute,
                         stlInt32              aSecond,
                         dtlFractionalSecond   aFractionalSecond,
                         dtlTimeOffset       * aTimeOffset )
{
    *aTimeOffset = (((((aHour * DTL_MINS_PER_HOUR)
                       + aMinute)
                      * DTL_SECS_PER_MINUTE)
                     + aSecond)
                    * DTL_USECS_PER_SEC) + aFractionalSecond;
}

/**
 * @brief dtlTimestampType value를 UTC timestamptype value로 변환.
 * @param[in]   aTimestampType  dtlTimestampType value
 * @param[in]   aTimeZone       timezone
 * @param[out]  aLocalTime      local time으로 변환한 dtlTimestampType value
 */
void dtdTimestamp2Local( dtlTimestampType   aTimestampType,
                         stlInt32           aTimeZone,
                         dtlTimestampType * aLocalTime )
{
    aTimestampType -= (aTimeZone * DTL_USECS_PER_SEC);

    *aLocalTime = aTimestampType;
}

/**
 * @brief dtlTimeType value를 UTC timetype value로 변환.
 * @param[in]   aTimeType       dtlTimeType value
 * @param[in]   aTimeZone       timezone
 * @param[out]  aLocalTime      local time으로 변환한 dtlTimeType value
 */
void dtdTime2Local( dtlTimeType        aTimeType,
                    stlInt32           aTimeZone,
                    dtlTimeType      * aLocalTime )
{
    aTimeType -= (aTimeZone * DTL_USECS_PER_SEC);

    *aLocalTime = aTimeType;
}


/**
 * @brief dtlDateType value를 precision(0)에 맞추어 fractional second를 TRUNCATE하여 조정.
 * @param[in,out] aDateType       dtlDateType
 * @param[out]    aErrorStack     에러 스택 
 */
stlStatus dtdAdjustDate( dtlDateType      * aDateType,
                         stlErrorStack    * aErrorStack )
{
    dtlDateType sDate = 0;
    dtlDateType sEpochJdate = 0;
    
    /*
     * datetype은 timestamp(0)과 같이 저장하므로,
     * fractional second를 truncate한다.
     *
     * 예) 연산결과가 
     *     '1999-01-01 11:22:33.500000' 인 경우,
     *     '1999-01-01 11:22:33' 로 truncate하여 datetype의 value를 만든다.
     */

    sDate = *aDateType;
    if( sDate >= STL_INT64_C(0) )
    {
        sDate = (((sDate)
                  / dtlTimestampTypeScale[0])
                 * dtlTimestampTypeScale[0]);
    }
    else
    {
        sEpochJdate = (DTL_EPOCH_JDATE * DTL_USECS_PER_DAY);
        sDate = sDate + sEpochJdate;
        
        sDate = (((sDate)
                  / dtlTimestampTypeScale[0])
                 * dtlTimestampTypeScale[0]);        

        sDate = sDate - sEpochJdate;
    }

    *aDateType = sDate;
    
    return STL_SUCCESS;
    
    /* STL_FINISH; */
    
    /* return STL_FAILURE; */
}


/**
 * @brief dtlTimestampType value를 precision(fractional second precision)에 맞게 조정.
 * @param[in,out] aTimestampType       dtlTimestampType
 * @param[in]     aTimestampPrecision  dtlTimestampType의 precision(fractional second precision)
 * @param[out]    aErrorStack          에러 스택 
 */
stlStatus dtdAdjustTimestamp( dtlTimestampType * aTimestampType,
                              stlInt32           aTimestampPrecision,
                              stlErrorStack    * aErrorStack )
{
    if( (aTimestampPrecision >= DTL_TIMESTAMP_MIN_PRECISION) &&
        (aTimestampPrecision <= DTL_TIMESTAMP_MAX_PRECISION) )
    {
        // Do Nothing
    }
    else
    {
        STL_THROW( ERROR_INVALID_PARAMETER_VALUE );
    }
    
    if( *aTimestampType >= STL_INT64_C(0) )
    {
        *aTimestampType = (((*aTimestampType + dtlTimestampTypeOffset[aTimestampPrecision])
                            / dtlTimestampTypeScale[aTimestampPrecision])
                           * dtlTimestampTypeScale[aTimestampPrecision]);
    }
    else
    {
        *aTimestampType = -((((-*aTimestampType) + dtlTimestampTypeOffset[aTimestampPrecision])
                             / dtlTimestampTypeScale[aTimestampPrecision])
                            * dtlTimestampTypeScale[aTimestampPrecision]);
        
    }
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_INVALID_PARAMETER_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_TIMESTAMP_NOT_CONTAIN_VALID_TIMESTAMP,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief dtlIntervalYearToMonthType value를 dtlExpTime으로 변환.
 * @param[in]  aIntervalYearToMonthType  dtlIntervalYearToMonthType value
 * @param[out] aDtlExpTime               dtlExpTime
 */
void dtdIntervalYearToMonth2DtlExpTime( dtlIntervalYearToMonthType   aIntervalYearToMonthType,
                                        dtlExpTime                 * aDtlExpTime )
{
    aDtlExpTime->mYear = aIntervalYearToMonthType.mMonth / DTL_MONTHS_PER_YEAR;
    aDtlExpTime->mMonth = aIntervalYearToMonthType.mMonth % DTL_MONTHS_PER_YEAR;
}

/**
 * @brief dtlIntervalDayToSecondType value를 dtlExpTime으로 변환.
 * @param[in]  aIntervalDayToSecondType  dtlIntervalDayToSecondType value
 * @param[out] aDtlExpTime               dtlExpTime
 * @param[out] aFractionalSecond         fractional second
 */
void dtdIntervalDayToSecond2DtlExpTime( dtlIntervalDayToSecondType   aIntervalDayToSecondType,
                                        dtlExpTime                 * aDtlExpTime,
                                        dtlFractionalSecond        * aFractionalSecond )
{
    dtlTimeOffset  sTime;
    dtlTimeOffset  sFraction;

    aDtlExpTime->mDay = aIntervalDayToSecondType.mDay;
    
    sTime = aIntervalDayToSecondType.mTime;
    
    sFraction = sTime / DTL_USECS_PER_HOUR;
    sTime -= sFraction * DTL_USECS_PER_HOUR;
    aDtlExpTime->mHour = sFraction;
    sFraction = sTime / DTL_USECS_PER_MINUTE;
    sTime -= sFraction * DTL_USECS_PER_MINUTE;
    aDtlExpTime->mMinute = sFraction;
    sFraction = sTime / DTL_USECS_PER_SEC;
    *aFractionalSecond = sTime - (sFraction * DTL_USECS_PER_SEC);
    aDtlExpTime->mSecond = sFraction;
}


/**
 * @brief token별로 parsing된 field를 해석
 * @param[in]  aField             field string
 * @param[in]  aFieldType         field type
 * @param[in]  aNumField          aField와 aFieldType의 배열수 
 * @param[in]  aIntervalIndicator interval indicator
 *                           <BR> dtlIntervalIndicator 참조.
 * @param[out] aDateType          type 
 * @param[out] aDtlExpTime        dtlExpTime
 * @param[out] aFractionalSecond  fractional second
 * @param[out] aErrorStack        에러 스택 
 */
stlStatus dtdDecodeInterval( stlChar             ** aField,
                             stlInt32             * aFieldType,
                             stlInt32               aNumField,
                             dtlIntervalIndicator   aIntervalIndicator,
                             stlInt32             * aDateType,
                             dtlExpTime           * aDtlExpTime,
                             dtlFractionalSecond  * aFractionalSecond,
                             stlErrorStack        * aErrorStack )
{
    stlBool    sIsBefore;
    stlChar  * sBuffer;
    stlInt32   sType;
    stlInt32   i;
    stlInt32   sFieldMask;
    stlInt32   sTimeMask;
    stlInt64   sValue;
    stlInt64   sValueMonth;
    stlFloat64 sValueFractional;
    stlInt32   sDecodeUnit;
    stlInt32   sSecond;

    stlUInt32  sErrorCode;
    dtlIntervalIndicator   sIntervalIndicator;    

    *aDateType = DTL_DTK_DELTA;
    sType = DTL_IGNORE_DTF;
    sIsBefore = STL_FALSE;
    sFieldMask = 0;

    aDtlExpTime->mYear = 0;
    aDtlExpTime->mMonth = 0;
    aDtlExpTime->mDay = 0;
    aDtlExpTime->mHour = 0;
    aDtlExpTime->mMinute = 0;
    aDtlExpTime->mSecond = 0;
    *aFractionalSecond = 0;    

    for( i = (aNumField - 1); i >= 0; i-- )
    {
        switch( aFieldType[i] )
        {
            case DTL_DTK_TIME:
                {
                    if( aNumField == 1 )
                    {
                        /* <time interval> ::=
                         *      <hours value> [ <colon> <minutes value> [ <colon> <seconds value> ] ]
                         *      | <minutes value> [ <colon> <seconds value> ]
                         *      | <seconds value>
                         */
                        sIntervalIndicator = aIntervalIndicator;
                    }
                    else
                    {
                        /* <day-time interval> ::=
                         *      <days value> [ <space> <hours value> [ <colon> <minutes value>
                         *      [ <colon> <seconds value> ] ] ]                        
                         */
                        sIntervalIndicator = DTL_INTERVAL_INDICATOR_DAY_TO_SECOND;
                    }
                    
                    STL_TRY( dtdDecodeTime( aField[i],
                                            sFieldMask,
                                            sIntervalIndicator,
                                            & sTimeMask,
                                            aDtlExpTime,
                                            aFractionalSecond,
                                            aErrorStack )
                             == STL_SUCCESS );
                    
                    sType = DTL_DTK_DAY;
                }
                break;
            case DTL_DTK_TZ:
                {
                    /*
                     * Timezone is a token with a leading sign character and at
                     * least one digit; there could be ':', '.', '-' embedded in
                     * it as well.
                     * ?????
                     */
                    STL_TRY_THROW( (*aField[i] == '-') || (*aField[i] == '+'),
                                   RAMP_ERROR_BAD_FORMAT );

                    if( aNumField == 1 )
                    {
                        /* <time interval> ::=
                         *      <hours value> [ <colon> <minutes value> [ <colon> <seconds value> ] ]
                         *      | <minutes value> [ <colon> <seconds value> ]
                         *      | <seconds value>
                         */
                        sIntervalIndicator = aIntervalIndicator;
                    }
                    else
                    {
                        /* <day-time interval> ::=
                         *      <days value> [ <space> <hours value> [ <colon> <minutes value>
                         *      [ <colon> <seconds value> ] ] ]                        
                         */
                        sIntervalIndicator = DTL_INTERVAL_INDICATOR_DAY_TO_SECOND;
                    }

                    /*
                     * Try for hh:mm or hh:mm:ss.
                     * If not, fall through to DTK_NUMBER case,
                     * which can handle signed float numbers and signed year-month values.
                     */
                    if( stlStrchr((aField[i] + 1), ':') != NULL )
                    {
                        STL_TRY( dtdDecodeTime( aField[i] + 1,
                                                sFieldMask,
                                                sIntervalIndicator,
                                                & sTimeMask,
                                                aDtlExpTime,
                                                aFractionalSecond,
                                                aErrorStack )
                                 == STL_SUCCESS );

                        if( *aField[i] == '-' )
                        {
                            aDtlExpTime->mHour = -aDtlExpTime->mHour;
                            aDtlExpTime->mMinute = -aDtlExpTime->mMinute;
                            aDtlExpTime->mSecond = -aDtlExpTime->mSecond;
                            *aFractionalSecond = -(*aFractionalSecond);
                        }
                        else
                        {
                            // Do Nothing
                        }

                        /*
                         * Set the next type to be a day, if units are not specified.
                         * This handles the case of '1 +02:03' since we are reading right to left.
                         * ???
                         */
                        sType = DTL_DTK_DAY;
                        sTimeMask = DTL_DTK_M(DTL_TZ);

                        break;
                    }
                    else
                    {
                        // Do Nothing
                    }
                        
                }
                /* FALL_THROUGH */
            case DTL_DTK_DATE:
                /* FALL_THROUGH */
            case DTL_DTK_NUMBER:
                {
                    if( sType == DTL_IGNORE_DTF )
                    {
                        if( aNumField == 1 )
                        {
                            switch( aIntervalIndicator )
                            {
                                case DTL_INTERVAL_INDICATOR_YEAR:
                                    sType = DTL_DTK_YEAR;
                                    break;
                                case DTL_INTERVAL_INDICATOR_MONTH:
                                    /* FALL_THROUGH */
                                case DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH:
                                    sType = DTL_DTK_MONTH;
                                    break;
                                case DTL_INTERVAL_INDICATOR_DAY:
                                    sType = DTL_DTK_DAY;
                                    break;
                                case DTL_INTERVAL_INDICATOR_HOUR:
                                    /* FALL_THROUGH */
                                case DTL_INTERVAL_INDICATOR_DAY_TO_HOUR:
                                    sType = DTL_DTK_HOUR;
                                    break;
                                case DTL_INTERVAL_INDICATOR_MINUTE:
                                    /* FALL_THROUGH */
                                case DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE:
                                    /* FALL_THROUGH */
                                case DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE:
                                    sType = DTL_DTK_MINUTE;
                                    break;
                                case DTL_INTERVAL_INDICATOR_SECOND:
                                    /* FALL_THROUGH */
                                case DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND:
                                    /* FALL_THROUGH */
                                case DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND:
                                    /* FALL_THROUGH */
                                case DTL_INTERVAL_INDICATOR_DAY_TO_SECOND:
                                    sType = DTL_DTK_SECOND;
                                    break;
                                default:
                                    sType = DTL_DTK_SECOND;
                                    break;
                            }
                        }
                        else
                        {
                            sType = DTL_DTK_HOUR;
                        }
                    }
                    else
                    {
                        // Do Nothing
                    }

                    STL_TRY_THROW( stlStrToInt64( aField[i],
                                                  STL_NTS,
                                                  & sBuffer,
                                                  10,
                                                  & sValue,
                                                  aErrorStack ) == STL_SUCCESS,
                                   ERROR_CONVERSION );
                    STL_TRY_THROW( (sValue >= STL_INT32_MIN) && (sValue <= STL_INT32_MAX),
                                   ERROR_FIELD_OVERFLOW );

                    if( ( aNumField > 1 ) && ( sType == DTL_DTK_HOUR ) )
                    {
                        STL_TRY_THROW( (sValue >= 0) && (sValue < DTL_HOURS_PER_DAY),
                                       ERROR_FIELD_OVERFLOW );
                    }
                    else
                    {
                        /* Do Nothing */
                    }

                    if( *sBuffer == '-' )
                    {
                        /*
                         * year-months 
                         */
                        STL_TRY_THROW( stlStrToInt64( sBuffer + 1,
                                                      STL_NTS,
                                                      & sBuffer,
                                                      10,
                                                      & sValueMonth,
                                                      aErrorStack ) == STL_SUCCESS,
                                       ERROR_CONVERSION );
                        
                        STL_TRY_THROW( (sValueMonth >= 0) && (sValueMonth < DTL_MONTHS_PER_YEAR),
                                       ERROR_FIELD_OVERFLOW );

                        STL_TRY_THROW( *sBuffer == '\0', RAMP_ERROR_BAD_FORMAT );

                        sType = DTL_DTK_MONTH;

                        if( *aField[i] == '-' )
                        {
                            sValueMonth = -sValueMonth;
                        }
                        else
                        {
                            // Do Nothing
                        }

                        sValue = sValue * DTL_MONTHS_PER_YEAR + sValueMonth;
                        sValueFractional = 0;
                    }
                    else if( *sBuffer == '.' )
                    {
                        /*
                         * INTERVAL SECOND 인 경우,
                         * 12.34, +12.34, -12.34 ... 와 같이 fractional second를 포함한 표현식허용.
                         */
                        
                        STL_TRY_THROW( aIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND,
                                       RAMP_ERROR_BAD_FORMAT );
                        
                        STL_TRY_THROW( stlStrToFloat64( sBuffer,
                                                        STL_NTS,
                                                        & sBuffer,
                                                        & sValueFractional,
                                                        aErrorStack ) == STL_SUCCESS,
                                       RAMP_ERROR_BAD_FORMAT );
                        
                        STL_TRY_THROW( *sBuffer == '\0', RAMP_ERROR_BAD_FORMAT );
                        
                        if( *aField[i] == '-' )
                        {
                            sValueFractional = -sValueFractional;
                        }
                        else
                        {
                            // Do Nothing
                        }
                    }
                    else if( *sBuffer == '\0' )
                    {
                        sValueFractional = 0;
                    }
                    else
                    {
                        STL_THROW( RAMP_ERROR_BAD_FORMAT );
                    }

                    sTimeMask = 0;

                    switch( sType )
                    {
                        case DTL_DTK_MICROSEC:
                            {
                                *aFractionalSecond += stlRint(sValue + sValueFractional);
                                sTimeMask = DTL_DTK_M(DTL_MICROSECOND);
                            }
                            break;
                        case DTL_DTK_MILLISEC:
                            {
                                aDtlExpTime->mSecond += sValue / 1000;
                                sValue -= (sValue / 1000) * 1000;

                                *aFractionalSecond += stlRint(sValue + sValueFractional) * 1000;

                                sTimeMask = DTL_DTK_M(DTL_MILLISECOND);
                            }
                            break;
                        case DTL_DTK_SECOND:
                            {
                                aDtlExpTime->mSecond += sValue;
                                *aFractionalSecond += stlRint(sValueFractional * 1000000);

                                if( sValueFractional == 0 )
                                {
                                    sTimeMask = DTL_DTK_M(DTL_SECOND);
                                }
                                else
                                {
                                    sTimeMask = DTL_DTK_ALL_SECS_M;
                                }
                            }
                            break;
                        case DTL_DTK_MINUTE:
                            {
                                aDtlExpTime->mMinute += sValue;

                                dtdAdjustFractSecond( sValueFractional,
                                                      aDtlExpTime,
                                                      aFractionalSecond,
                                                      DTL_SECS_PER_MINUTE );

                                sTimeMask = DTL_DTK_M(DTL_MINUTE);                                
                            }
                            break;
                        case DTL_DTK_HOUR:
                            {
                                aDtlExpTime->mHour += sValue;

                                dtdAdjustFractSecond( sValueFractional,
                                                      aDtlExpTime,
                                                      aFractionalSecond,
                                                      DTL_SECS_PER_HOUR );
                                
                                sTimeMask = DTL_DTK_M(DTL_HOUR);
                                sType = DTL_DTK_DAY;
                            }
                            break;
                        case DTL_DTK_DAY:
                            {
                                aDtlExpTime->mDay += sValue;

                                dtdAdjustFractSecond( sValueFractional,
                                                      aDtlExpTime,
                                                      aFractionalSecond,
                                                      DTL_SECS_PER_DAY );
                                
                                sTimeMask = DTL_DTK_M(DTL_DAY);                                
                            }
                            break;
                        case DTL_DTK_WEEK:
                            {
                                aDtlExpTime->mDay += sValue * 7;

                                dtdAdjustFractDays( sValueFractional,
                                                    aDtlExpTime,
                                                    aFractionalSecond,
                                                    7 );

                                sTimeMask = DTL_DTK_M(DTL_WEEK);                                
                            }
                            break;
                        case DTL_DTK_MONTH:
                            {
                                aDtlExpTime->mMonth += sValue;

                                dtdAdjustFractDays( sValueFractional,
                                                    aDtlExpTime,
                                                    aFractionalSecond,
                                                    DTL_DAYS_PER_MONTH );

                                sTimeMask = DTL_DTK_M(DTL_MONTH);                                
                            }
                            break;
                        case DTL_DTK_YEAR:
                            {
                                aDtlExpTime->mYear += sValue;

                                if( sValueFractional != 0 )
                                {
                                    aDtlExpTime->mMonth += sValueFractional * DTL_MONTHS_PER_YEAR;
                                }
                                else
                                {
                                    // Do Nothing
                                }

                                sTimeMask = DTL_DTK_M(DTL_YEAR);
                            }
                            break;
                        case DTL_DTK_DECADE:
                            {
                                aDtlExpTime->mYear += sValue * 10;

                                if( sValueFractional != 0 )
                                {
                                    aDtlExpTime->mMonth +=
                                        sValueFractional * DTL_MONTHS_PER_YEAR * 10;
                                }
                                else
                                {
                                    // Do Nothing
                                }

                                sTimeMask = DTL_DTK_M(DTL_DECADE);
                            }
                            break;
                        case DTL_DTK_CENTURY:
                            {
                                aDtlExpTime->mYear += sValue * 100;

                                if( sValueFractional != 0 )
                                {
                                    aDtlExpTime->mMonth +=
                                        sValueFractional * DTL_MONTHS_PER_YEAR * 100;
                                }
                                else
                                {
                                    // Do Nothing
                                }

                                sTimeMask = DTL_DTK_M(DTL_CENTURY);
                            }
                            break;
                        case DTL_DTK_MILLENNIUM:
                            {
                                aDtlExpTime->mYear += sValue * 1000;

                                if( sValueFractional != 0 )
                                {
                                    aDtlExpTime->mMonth +=
                                        sValueFractional * DTL_MONTHS_PER_YEAR * 1000;
                                }
                                else
                                {
                                    // Do Nothing
                                }

                                sTimeMask = DTL_DTK_M(DTL_MILLENNIUM);
                            }
                            break;
                        default:
                            STL_THROW( RAMP_ERROR_BAD_FORMAT );
                            break;
                    }
                }
                break;
            case DTL_DTK_STRING:
                /* FALL_THROUGH */
            case DTL_DTK_SPECIAL:
                {
                    sType = dtdDecodeUnits( i,
                                            aField[i],
                                            & sDecodeUnit );

                    if( sType == DTL_IGNORE_DTF )
                    {
                        continue;
                    }
                    else
                    {
                        // Do Nothing
                    }

                    sTimeMask = 0;

                    switch( sType )
                    {
                        case DTL_UNITS:
                            sType = sDecodeUnit;
                            break;
                        case DTL_AGO:
                            sIsBefore = STL_TRUE;
                            sType = sDecodeUnit;
                            break;
                        case DTL_RESERV:
                            sTimeMask = (DTL_DTK_DATE_M|DTL_DTK_TIME_M);
                            *aDateType = sDecodeUnit;
                            break;
                        default:
                            STL_THROW( RAMP_ERROR_BAD_FORMAT );
                            break;
                    }
                }
                break;
            default:
                STL_THROW( RAMP_ERROR_BAD_FORMAT );
                break;
        }

        STL_TRY_THROW( (sTimeMask & sFieldMask) == 0, RAMP_ERROR_BAD_FORMAT );

        sFieldMask |= sTimeMask;
    }

    STL_TRY_THROW( sFieldMask != 0, RAMP_ERROR_BAD_FORMAT );

    if( *aFractionalSecond != 0 )
    {
        sSecond = *aFractionalSecond / DTL_USECS_PER_SEC;
        *aFractionalSecond -= sSecond * DTL_USECS_PER_SEC;
        
        aDtlExpTime->mSecond += sSecond;
    }
    else
    {
        // Do Nothing
    }
    
    /*
     * SQL standard interval literal
     * 부호는 맨 앞에 하나만 올 수 있다.
     * '-1 1:00:00' => negative 1 day and negative 1 hours
     */

    for( i = 1; i < aNumField; i++ )
    {
        if( (*aField[i] == '-') ||
            (*aField[i] == '+') )
        {
            STL_THROW( RAMP_ERROR_BAD_FORMAT );
            break;
        }
        else
        {
            // Do Nothing
        }
    }        
        
    if( *aField[0] == '-' )
    {
        *aFractionalSecond =
            (*aFractionalSecond > 0) ? -(*aFractionalSecond) : *aFractionalSecond;
        
        aDtlExpTime->mSecond =
            (aDtlExpTime->mSecond > 0) ? -(aDtlExpTime->mSecond) : aDtlExpTime->mSecond;
        
        aDtlExpTime->mMinute =
            (aDtlExpTime->mMinute > 0) ? -(aDtlExpTime->mMinute) : aDtlExpTime->mMinute;
        
        aDtlExpTime->mHour =
            (aDtlExpTime->mHour > 0) ? -(aDtlExpTime->mHour) : aDtlExpTime->mHour;
        
        aDtlExpTime->mDay =
            (aDtlExpTime->mDay > 0) ? -(aDtlExpTime->mDay) : aDtlExpTime->mDay;
        
        aDtlExpTime->mMonth =
            (aDtlExpTime->mMonth > 0) ? -(aDtlExpTime->mMonth) : aDtlExpTime->mMonth;
        
        aDtlExpTime->mYear =
            (aDtlExpTime->mYear > 0) ? -(aDtlExpTime->mYear) : aDtlExpTime->mYear;
    }
    else
    {
        // Do Nothing
    }

    /* AGO는 모든것을 무효화시킨다. */
    if( sIsBefore == STL_TRUE )
    {
        *aFractionalSecond = -(*aFractionalSecond);

        aDtlExpTime->mSecond = -(aDtlExpTime->mSecond);
        aDtlExpTime->mMinute = -(aDtlExpTime->mMinute);
        aDtlExpTime->mHour = -(aDtlExpTime->mHour);
        aDtlExpTime->mDay = -(aDtlExpTime->mDay);
        aDtlExpTime->mMonth = -(aDtlExpTime->mMonth);
        aDtlExpTime->mYear = -(aDtlExpTime->mYear);
    }
    else
    {
        // Do Nothing
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERROR_BAD_FORMAT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_NOT_VALID_INTERVAL_LITERAL,
                      NULL,
                      aErrorStack );
    };

    STL_CATCH( ERROR_CONVERSION )
    {
        sErrorCode = stlGetLastErrorCode( aErrorStack );

        switch( sErrorCode )
        {
            case STL_ERRCODE_OUT_OF_RANGE:
                {
                    stlPushError( STL_ERROR_LEVEL_ABORT,
                                  DTL_ERRCODE_C_STRING_NOT_VALID_INTERVAL_LITERAL,
                                  NULL,
                                  aErrorStack );
                }
            default:
                {
                    break;
                }
        }        
    }

    STL_CATCH( ERROR_FIELD_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_NOT_VALID_INTERVAL_LITERAL,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;    
}

/**
 * @brief interval value truncated 검사 
 * @param[in]  aDtlExpTime         dtlExpTime
 * @param[in]  aIntervalIndicator  interval indicator
 * @param[in]  aFractionalSecond   fractional second
 * @param[out] aErrorStack         에러 스택 
 */
stlStatus dtdIntervalValueTruncatedCheck( dtlExpTime           * aDtlExpTime,
                                          dtlIntervalIndicator   aIntervalIndicator,
                                          dtlFractionalSecond  * aFractionalSecond,
                                          stlErrorStack        * aErrorStack )
{
    /*
     * value validate check
     */ 
    switch( aIntervalIndicator )
    {
        case DTL_INTERVAL_INDICATOR_YEAR:
            {
                STL_TRY_THROW( (aDtlExpTime->mMonth % DTL_MONTHS_PER_YEAR == 0),
                               RAMP_ERROR_BAD_FORMAT );
            }
            break;
        case DTL_INTERVAL_INDICATOR_MONTH:
        case DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH:            
            {
                // Do Nothing
            }
            break;
        case DTL_INTERVAL_INDICATOR_DAY:
            {
                STL_TRY_THROW( (aDtlExpTime->mYear == 0) && (aDtlExpTime->mMonth == 0) &&
                               (aDtlExpTime->mHour % DTL_HOURS_PER_DAY == 0) &&
                               (aDtlExpTime->mMinute % (DTL_HOURS_PER_DAY * DTL_MINS_PER_HOUR) == 0) &&
                               (aDtlExpTime->mSecond % DTL_SECS_PER_DAY == 0) &&
                               (*aFractionalSecond == 0),
                               RAMP_ERROR_BAD_FORMAT );
            }
            break;
        case DTL_INTERVAL_INDICATOR_HOUR:
        case DTL_INTERVAL_INDICATOR_DAY_TO_HOUR:
            {
                STL_TRY_THROW( (aDtlExpTime->mYear == 0) && (aDtlExpTime->mMonth == 0) &&
                               (aDtlExpTime->mMinute % DTL_MINS_PER_HOUR == 0) &&
                               (aDtlExpTime->mSecond % DTL_SECS_PER_HOUR == 0) &&
                               (*aFractionalSecond == 0),
                               RAMP_ERROR_BAD_FORMAT );
            }
        case DTL_INTERVAL_INDICATOR_MINUTE:
        case DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE:
        case DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE:
            {
                STL_TRY_THROW( (aDtlExpTime->mYear == 0) && (aDtlExpTime->mMonth == 0) &&
                               (aDtlExpTime->mSecond % DTL_SECS_PER_MINUTE == 0) &&
                               (*aFractionalSecond == 0),
                               RAMP_ERROR_BAD_FORMAT );
            }
            break;
        case DTL_INTERVAL_INDICATOR_SECOND:
        case DTL_INTERVAL_INDICATOR_DAY_TO_SECOND:
        case DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND:
        case DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND:
            {
                STL_TRY_THROW( (aDtlExpTime->mYear == 0) && (aDtlExpTime->mMonth == 0),
                               RAMP_ERROR_BAD_FORMAT );
            }
            break;
        default:
            STL_DASSERT( 1 == 0 );
            break;
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERROR_BAD_FORMAT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_INTERVAL_FIELD_TRUNCATED,
                      NULL,
                      aErrorStack );
    };

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief aFrac*aScale에서 second와 aFractionalSecond를 구한다.
 * @param[in]  aFrac              value fractional
 * @param[out] aDtlExpTime        dtlExpTime
 * @param[out] aFractionalSecond  fractional second
 * @param[in]  aScale             second를 위한 scale
 */
void dtdAdjustFractSecond( stlFloat64            aFrac,
                           dtlExpTime          * aDtlExpTime,
                           dtlFractionalSecond * aFractionalSecond,
                           stlInt32              aScale )
{
    stlInt32 sSecond;

    if( aFrac == 0 )
    {
        // Do Nothing
    }
    else
    {
        aFrac *= aScale;
        sSecond = (stlInt32)aFrac;
        aDtlExpTime->mSecond += sSecond;
        aFrac -= sSecond;

        *aFractionalSecond += stlRint(aFrac * 1000000);
    }
}

/**
 * @brief aFrac*aScale에서 day & second & fractional second를 구한다.
 * @param[in]  aFrac              value fractional
 * @param[out] aDtlExpTime        dtlExpTime
 * @param[out] aFractionalSecond  fractional second
 * @param[in]  aScale             day를 위한 scale
 */
void dtdAdjustFractDays( stlFloat64            aFrac,
                         dtlExpTime          * aDtlExpTime,
                         dtlFractionalSecond * aFractionalSecond,
                         stlInt32              aScale )
{
    stlInt32  sExtraDays;

    if( aFrac == 0 )
    {
        // Do Nothing
    }
    else
    {
        aFrac *= aScale;
        sExtraDays = (stlInt32)aFrac;
        aDtlExpTime->mDay += sExtraDays;
        aFrac -= sExtraDays;
        dtdAdjustFractSecond( aFrac,
                              aDtlExpTime,
                              aFractionalSecond,
                              DTL_SECS_PER_DAY );
    }
}

/**
 * @brief 검색배열을 이용해 test string을 해석
 * @param[in]  aField     token으로 분류된 field
 * @param[in]  aLowToken  검색 string
 * @param[out] aValue     배열에서 검색된 value
 * @return 검색배열에서 검색된 value의 type
 */
stlInt32 dtdDecodeUnits( stlInt32   aField,
                         stlChar  * aLowToken,
                         stlInt32 * aValue )
{
    stlInt32             sType;
    const dtlDateToken * sTokenPtr;

    sTokenPtr = dtlDateDeltaCache[aField];

    if( (sTokenPtr == NULL) ||
        (stlStrncmp(aLowToken, sTokenPtr->mToken, DTL_DATE_TOKEN_MAX_LEN) != 0) )
    {
        sTokenPtr = dtdDateBinarySearch( aLowToken,
                                         dtlDateDeltaTokenTbl,
                                         dtlDateDeltaTokenTblSize );
                                         
    }
    else
    {
        // Do Nothing
    }

    if( sTokenPtr == NULL )
    {
        sType = DTL_UNKNOWN_FIELD;
        *aValue = 0;
    }
    else
    {
        dtlDateDeltaCache[aField] = sTokenPtr;
        sType = sTokenPtr->mType;

        if( (sType == DTL_TZ) || (sType == DTL_DTZ) )
        {
            *aValue = DTD_FROM_VAL(sTokenPtr);
        }
        else
        {
            *aValue = sTokenPtr->mValue;
        }
    }

    return sType;
}

/* /\** */
/*  * @brief ISO8601Interval value의 decimal value 해석 */
/*  * @param[in]  aStr               input string */
/*  * @param[out] aDateType          type */
/*  * @param[out] aDtlExpTime        dtlExpTime */
/*  * @param[out] aFractionalSecond  fractional second */
/*  * @param[out] aErrorStack        에러 스택 */
/*  *  */
/*  *  <BR> Examples:  P1D	for 1 day */
/*  *  <BR>            PT1H for 1 hour */
/*  *  <BR>            P2Y6M7DT1H30M for 2 years, 6 months, 7 days 1 hour 30 min */
/*  *  <BR>            P0002-06-07T01:30:00 the same value in alternative format */
/*  *\/ */
/* stlStatus dtdDecodeISO8601Interval( stlChar             * aStr, */
/*                                     stlInt32            * aDateType, */
/*                                     dtlExpTime          * aDtlExpTime, */
/*                                     dtlFractionalSecond * aFractionalSecond, */
/*                                     stlErrorStack       * aErrorStack ) */
/* { */
/*     stlBool    sIsDatePart; */
/*     stlBool    sIsHaveField; */
/*     stlChar  * sFieldStart; */
/*     stlInt32   sValue; */
/*     stlFloat64 sValueFractional; */
/*     stlChar    sUnit; */
    
/*     sIsDatePart = STL_TRUE; */
/*     sIsHaveField = STL_FALSE; */

/*     *aDateType = DTL_DTK_DELTA; */
/*     aDtlExpTime->mYear = 0; */
/*     aDtlExpTime->mMonth = 0; */
/*     aDtlExpTime->mDay = 0; */
/*     aDtlExpTime->mHour = 0; */
/*     aDtlExpTime->mMinute = 0; */
/*     aDtlExpTime->mSecond = 0; */
/*     *aFractionalSecond = 0; */

/*     if( (stlStrlen(aStr) < 2) || (aStr[0] != 'P') ) */
/*     { */
/*         STL_THROW( RAMP_ERROR_BAD_FORMAT ); */
/*     } */
/*     else */
/*     { */
/*         // Do Nothing */
/*     } */

/*     aStr++; */

/*     while( *aStr != '\0' ) */
/*     { */
/*         /\* */
/*          * Time part */
/*          *\/   */
/*         if( *aStr == 'T' ) */
/*         { */
/*             sIsDatePart = STL_FALSE; */
/*             sIsHaveField = STL_FALSE; */
/*             aStr++; */
/*             continue; */
/*         } */
/*         else */
/*         { */
/*             // Do Nothing */
/*         } */

/*         sFieldStart = aStr; */
        
/*         STL_TRY( dtdParseISO8601Number( aStr, */
/*                                         & aStr, */
/*                                         & sValue, */
/*                                         & sValueFractional, */
/*                                         aErrorStack ) */
/*                  == STL_SUCCESS ); */

/*         sUnit = *aStr; */
/*         aStr++; */

/*         if( sIsDatePart == STL_TRUE ) */
/*         { */
/*             switch( sUnit ) */
/*             { */
/*                 /\* before T: Y M W D *\/ */
/*                 case 'Y': */
/*                     { */
/*                         aDtlExpTime->mYear += sValue; */
/*                         aDtlExpTime->mMonth += (sValueFractional * 12); */
/*                     } */
/*                     break; */
/*                 case 'M': */
/*                     { */
/*                         aDtlExpTime->mMonth += sValue; */
/*                         dtdAdjustFractDays( sValueFractional, */
/*                                             aDtlExpTime, */
/*                                             aFractionalSecond, */
/*                                             DTL_DAYS_PER_MONTH ); */
/*                     } */
/*                     break; */
/*                 case 'W': */
/*                     { */
/*                         aDtlExpTime->mDay += sValue * 7; */
/*                         dtdAdjustFractDays( sValueFractional, */
/*                                             aDtlExpTime, */
/*                                             aFractionalSecond, */
/*                                             7 ); */
/*                     } */
/*                     break; */
/*                 case 'D': */
/*                     { */
/*                         aDtlExpTime->mDay += sValue; */
/*                         dtdAdjustFractSecond( sValueFractional, */
/*                                               aDtlExpTime, */
/*                                               aFractionalSecond, */
/*                                               DTL_SECS_PER_DAY ); */
/*                     } */
/*                     break; */
/*                 case 'T': */
/*                     /\* FALL_THROUGH *\/ */
/*                 case '\0': */
/*                     { */
/*                         if( (dtdISO8601IntegerWidth(sFieldStart) == 8) && */
/*                             (sIsHaveField != STL_TRUE) ) */
/*                         { */
/*                             aDtlExpTime->mYear += sValue / 10000; */
/*                             aDtlExpTime->mMonth += (sValue / 100) % 100; */
/*                             aDtlExpTime->mDay += sValue % 100; */
/*                             dtdAdjustFractSecond( sValueFractional, */
/*                                                   aDtlExpTime, */
/*                                                   aFractionalSecond, */
/*                                                   DTL_SECS_PER_DAY ); */
/*                             if( sUnit == '\0' ) */
/*                             { */
/*                                 STL_THROW( RAMP_SUCCESS ); */
/*                             } */
/*                             else */
/*                             { */
/*                                 // Do Nothing */
/*                             } */

/*                             sIsDatePart = STL_FALSE; */
/*                             sIsHaveField = STL_FALSE; */
/*                             continue; */
/*                         } */
/*                         else */
/*                         { */
/*                             // Do Nothing */
/*                         } */
/*                     } */
/*                     /\* FALL_THROUGH *\/ */
/*                 case '-': */
/*                     { */
/*                         STL_TRY_THROW( sIsHaveField != STL_TRUE, RAMP_ERROR_BAD_FORMAT ); */

/*                         aDtlExpTime->mYear += sValue; */
/*                         aDtlExpTime->mMonth += (sValueFractional * 12); */

/*                         if( sUnit == '\0' ) */
/*                         { */
/*                             STL_THROW( RAMP_SUCCESS ); */
/*                         } */
/*                         else */
/*                         { */
/*                             // Do Nothing */
/*                         } */

/*                         if( sUnit == 'T' ) */
/*                         { */
/*                             sIsDatePart = STL_FALSE; */
/*                             sIsHaveField = STL_FALSE; */
/*                             continue; */
/*                         } */
/*                         else */
/*                         { */
/*                             // Do Nothing */
/*                         } */

/*                         STL_TRY( dtdParseISO8601Number( aStr, */
/*                                                         & aStr, */
/*                                                         & sValue, */
/*                                                         & sValueFractional, */
/*                                                         aErrorStack ) */
/*                                  == STL_SUCCESS ); */

/*                         aDtlExpTime->mMonth += sValue; */

/*                         dtdAdjustFractDays( sValueFractional, */
/*                                             aDtlExpTime, */
/*                                             aFractionalSecond, */
/*                                             DTL_DAYS_PER_MONTH ); */

/*                         if( *aStr == '\0' ) */
/*                         { */
/*                             STL_THROW( RAMP_SUCCESS ); */
/*                         } */
/*                         else */
/*                         { */
/*                             // Do Nothing */
/*                         } */

/*                         if( *aStr == 'T' ) */
/*                         { */
/*                             sIsDatePart = STL_FALSE; */
/*                             sIsHaveField = STL_FALSE; */
/*                             continue; */
/*                         } */
/*                         else */
/*                         { */
/*                             // Do Nothing */
/*                         } */

/*                         STL_TRY_THROW( *aStr == '-', RAMP_ERROR_BAD_FORMAT ); */

/*                         aStr++; */

/*                         STL_TRY( dtdParseISO8601Number( aStr, */
/*                                                         & aStr, */
/*                                                         & sValue, */
/*                                                         & sValueFractional, */
/*                                                         aErrorStack ) */
/*                                  == STL_SUCCESS ); */

/*                         aDtlExpTime->mDay += sValue; */

/*                         dtdAdjustFractSecond( sValueFractional, */
/*                                               aDtlExpTime, */
/*                                               aFractionalSecond, */
/*                                               DTL_SECS_PER_DAY ); */

/*                         if( *aStr == '\0' ) */
/*                         { */
/*                             STL_THROW( RAMP_SUCCESS ); */
/*                         } */
/*                         else */
/*                         { */
/*                             // Do Nothing */
/*                         } */

/*                         if( *aStr == 'T' ) */
/*                         { */
/*                             sIsDatePart = STL_FALSE; */
/*                             sIsHaveField = STL_FALSE; */
/*                             continue; */
/*                         } */
/*                         else */
/*                         { */
/*                             // Do Nothing */
/*                         } */

/*                         STL_THROW( RAMP_ERROR_BAD_FORMAT ); */
/*                     } */
/*                 default: */
/*                     STL_THROW( RAMP_ERROR_BAD_FORMAT ); */
/*                     break; */
/*             } */
/*         } */
/*         else */
/*         { */
/*             /\* after T : H M S *\/ */
/*             switch( sUnit ) */
/*             { */
/*                 case 'H': */
/*                     { */
/*                         aDtlExpTime->mHour += sValue; */
/*                         dtdAdjustFractSecond( sValueFractional, */
/*                                               aDtlExpTime, */
/*                                               aFractionalSecond, */
/*                                               DTL_SECS_PER_HOUR ); */
/*                     } */
/*                     break; */
/*                 case 'M': */
/*                     { */
/*                         aDtlExpTime->mMinute += sValue; */
/*                         dtdAdjustFractSecond( sValueFractional, */
/*                                               aDtlExpTime, */
/*                                               aFractionalSecond, */
/*                                               DTL_SECS_PER_MINUTE ); */
/*                     } */
/*                     break; */
/*                 case 'S': */
/*                     { */
/*                         aDtlExpTime->mSecond += sValue; */
/*                         dtdAdjustFractSecond( sValueFractional, */
/*                                               aDtlExpTime, */
/*                                               aFractionalSecond, */
/*                                               1 ); */
/*                     } */
/*                     break; */
/*                 case '\0': */
/*                     { */
/*                         if( (dtdISO8601IntegerWidth(sFieldStart) == 6) && */
/*                             (sIsHaveField != STL_TRUE) ) */
/*                         { */
/*                             aDtlExpTime->mHour += sValue / 10000; */
/*                             aDtlExpTime->mMinute += (sValue / 100) % 100; */
/*                             aDtlExpTime->mSecond += sValue % 100; */
/*                             dtdAdjustFractSecond( sValueFractional, */
/*                                                   aDtlExpTime, */
/*                                                   aFractionalSecond, */
/*                                                   1 ); */
                            
/*                             STL_THROW( RAMP_SUCCESS ); */
/*                         } */
/*                         else */
/*                         { */
/*                             // Do Nothing */
/*                         } */
/*                     } */
/*                     break; */
/*                 case ':': */
/*                     { */
/*                         STL_TRY_THROW( sIsHaveField != STL_TRUE, RAMP_ERROR_BAD_FORMAT ); */

/*                         aDtlExpTime->mHour += sValue; */
/*                         dtdAdjustFractSecond( sValueFractional, */
/*                                               aDtlExpTime, */
/*                                               aFractionalSecond, */
/*                                               DTL_SECS_PER_HOUR ); */

/*                         if( sUnit == '\0' ) */
/*                         { */
/*                             STL_THROW( RAMP_SUCCESS ); */
/*                         } */
/*                         else */
/*                         { */
/*                             // Do Nothing */
/*                         } */

/*                         STL_TRY( dtdParseISO8601Number( aStr, */
/*                                                         & aStr, */
/*                                                         & sValue, */
/*                                                         & sValueFractional, */
/*                                                         aErrorStack ) */
/*                                  == STL_SUCCESS ); */

/*                         aDtlExpTime->mMinute += sValue; */

/*                         dtdAdjustFractSecond( sValueFractional, */
/*                                               aDtlExpTime, */
/*                                               aFractionalSecond, */
/*                                               DTL_SECS_PER_MINUTE ); */

/*                         if( *aStr == '\0' ) */
/*                         { */
/*                             STL_THROW( RAMP_SUCCESS ); */
/*                         } */
/*                         else */
/*                         { */
/*                             // Do Nothing */
/*                         } */

/*                         STL_TRY_THROW( *aStr == ':', RAMP_ERROR_BAD_FORMAT ); */

/*                         aStr++; */

/*                         STL_TRY( dtdParseISO8601Number( aStr, */
/*                                                         & aStr, */
/*                                                         & sValue, */
/*                                                         & sValueFractional, */
/*                                                         aErrorStack ) */
/*                                  == STL_SUCCESS ); */

/*                         aDtlExpTime->mSecond += sValue; */

/*                         dtdAdjustFractSecond( sValueFractional, */
/*                                               aDtlExpTime, */
/*                                               aFractionalSecond, */
/*                                               1 ); */

/*                         if( *aStr == '\0' ) */
/*                         { */
/*                             STL_THROW( RAMP_SUCCESS ); */
/*                         } */
/*                         else */
/*                         { */
/*                             // Do Nothing */
/*                         } */

/*                         STL_THROW( RAMP_ERROR_BAD_FORMAT ); */
/*                     } */
/*                     break; */
/*                 default: */
/*                     STL_THROW( RAMP_ERROR_BAD_FORMAT ); */
/*                     break;                         */
/*             } */
/*         } */
        
/*         sIsHaveField = STL_TRUE; */
/*     } */

/*     STL_RAMP( RAMP_SUCCESS ); */

/*     return STL_SUCCESS; */
    
/*     STL_CATCH( RAMP_ERROR_BAD_FORMAT ) */
/*     { */
/*         stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                       DTL_ERRCODE_C_STRING_NOT_VALID_INTERVAL_LITERAL, */
/*                       NULL, */
/*                       aErrorStack ); */
/*     }; */

/*     STL_FINISH; */

/*     return STL_FAILURE;     */
/* } */

/* /\** */
/*  * @brief parse deciaml value  */
/*  * @param[in]  aStr            string             */
/*  * @param[out] aEndPtr         해석가능한 마지막 문자에 대한 포인터   */
/*  * @param[out] aIntegerPart    integer part   */
/*  * @param[out] aFractionalPart fractional part */
/*  * @param[out] aErrorStack     에러 스택 */
/*  *\/ */
/* stlStatus dtdParseISO8601Number( stlChar        * aStr, */
/*                                  stlChar       ** aEndPtr, */
/*                                  stlInt32       * aIntegerPart, */
/*                                  stlFloat64     * aFractionalPart, */
/*                                  stlErrorStack  * aErrorStack ) */
/* { */
/*     stlFloat64  sValue; */

/*     if( (stlIsdigit(*aStr) != STL_TRUE) || */
/*         (*aStr == '-') || (*aStr == '.') ) */
/*     { */
/*         STL_THROW( RAMP_ERROR_BAD_FORMAT ); */
/*     } */
/*     else */
/*     { */
/*         // Do Nothing */
/*     } */

/*     STL_TRY_THROW( stlStrToFloat64( aStr, */
/*                                     STL_NTS, */
/*                                     aEndPtr, */
/*                                     & sValue, */
/*                                     aErrorStack ) == STL_SUCCESS, */
/*                    RAMP_ERROR_BAD_FORMAT ); */

/*     STL_TRY_THROW( *aEndPtr != aStr, RAMP_ERROR_BAD_FORMAT ); */

/*     STL_TRY_THROW( (sValue >= STL_INT32_MIN) && (sValue <= STL_INT32_MAX), */
/*                    ERROR_FIELD_OVERFLOW ); */

/*     if( sValue >= 0 ) */
/*     { */
/*         *aIntegerPart = (stlInt32)stlFloor( sValue ); */
/*     } */
/*     else */
/*     { */
/*         *aIntegerPart = (stlInt32)(-stlFloor(sValue)); */
/*     } */

/*     *aFractionalPart = sValue - *aIntegerPart; */

/*     return STL_SUCCESS; */

/*     STL_CATCH( RAMP_ERROR_BAD_FORMAT ) */
/*     { */
/*         stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                       DTL_ERRCODE_C_STRING_NOT_VALID_INTERVAL_LITERAL, */
/*                       NULL, */
/*                       aErrorStack ); */
/*     }; */

/*     STL_CATCH( ERROR_FIELD_OVERFLOW ) */
/*     { */
/*         stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                       DTL_ERRCODE_C_STRING_INTERVAL_FIELD_TRUNCATED, */
/*                       NULL, */
/*                       aErrorStack ); */
/*     };     */

/*     STL_FINISH; */

/*     return STL_FAILURE; */
/* } */

/* /\** */
/*  * @brief string에서 숫자로 구성된 초기 문자열의 길이 반환  */
/*  * @param  aFieldStart  string */
/*  * @return 숫자로 구성된 초기 문자열의 길이 반환 */
/*  *\/ */
/* stlInt32 dtdISO8601IntegerWidth( stlChar * aFieldStart ) */
/* { */
/*     if( *aFieldStart == '-' ) */
/*     { */
/*         aFieldStart++; */
/*     } */
/*     else */
/*     { */
/*         // Do Nothing */
/*     } */

/*     return stlStrspn( aFieldStart, "0123456789" ); */
/* } */

/**
 * @brief dtlExpTime을 dtlIntervalYearToMonthType 으로 변환
 * @param[in]  aDtlExpTime              dtlExpTime
 * @param[in]  aIntervalIndicator       interval indicator (dtlIntervalIndicater 참조)
 * @param[out] aIntervalYearToMonthType dtlIntervalYearToMonthType value
 */
void dtdDtlExpTime2IntervalYearToMonth( dtlExpTime                 * aDtlExpTime,
                                        dtlIntervalIndicator         aIntervalIndicator,
                                        dtlIntervalYearToMonthType * aIntervalYearToMonthType )
{
    aIntervalYearToMonthType->mIndicator = aIntervalIndicator;

    aIntervalYearToMonthType->mMonth =
        aDtlExpTime->mYear * DTL_MONTHS_PER_YEAR + aDtlExpTime->mMonth;
}

/**
 * @brief dtlExpTime을 dtlIntervalDayToSecondType 으로 변환
 * @param[in]  aDtlExpTime              dtlExpTime
 * @param[in]  aFractionalSecond        fractional second
 * @param[in]  aIntervalIndicator       interval indicator (dtlIntervalIndicater 참조)
 * @param[out] aIntervalDayToSecondType aIntervalDayToSecondType value
 */
void dtdDtlExpTime2IntervalDayToSecond( dtlExpTime                 * aDtlExpTime,
                                        dtlFractionalSecond          aFractionalSecond,
                                        dtlIntervalIndicator         aIntervalIndicator,
                                        dtlIntervalDayToSecondType * aIntervalDayToSecondType )
{
    aIntervalDayToSecondType->mIndicator = aIntervalIndicator;
    
    aIntervalDayToSecondType->mDay = aDtlExpTime->mDay;
    
    aIntervalDayToSecondType->mDay += aDtlExpTime->mHour / DTL_HOURS_PER_DAY;

    aIntervalDayToSecondType->mTime =
        ((((((aDtlExpTime->mHour % DTL_HOURS_PER_DAY) * STL_INT64_C(60)) +
            aDtlExpTime->mMinute) *
           STL_INT64_C(60)) +
          aDtlExpTime->mSecond) *
         DTL_USECS_PER_SEC) + aFractionalSecond;

    aIntervalDayToSecondType->mDay += aIntervalDayToSecondType->mTime / DTL_USECS_PER_DAY;
    aIntervalDayToSecondType->mTime = aIntervalDayToSecondType->mTime % DTL_USECS_PER_DAY;
}

/**
 * @brief dtlIntervalYearToMonthType  precision 검사 
 * @param[in,out] aIntervalYearToMonthType    dtlIntervalYearToMonthType
 * @param[in]     aIntervalIndicator          dtlIntervalIndicator
 * @param[in]     aLeadingPrecision           leading precision
 * @param[out]    aErrorStack                 에러 스택
 */
stlStatus dtdAdjustIntervalYearToMonth( dtlIntervalYearToMonthType * aIntervalYearToMonthType,
                                        dtlIntervalIndicator         aIntervalIndicator,
                                        stlInt32                     aLeadingPrecision,
                                        stlErrorStack              * aErrorStack )
{
    stlInt64   sDigitValue = 0;
    stlInt64   sCmpValue;
    
    /*
     * interval leading precision check
     */
    
    switch( aIntervalIndicator )
    {
        case DTL_INTERVAL_INDICATOR_YEAR:
            /* FALL_THROUGH */            
        case DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH:
            {
                sDigitValue = (aIntervalYearToMonthType->mMonth == 0) ?
                    0 : (aIntervalYearToMonthType->mMonth / DTL_MONTHS_PER_YEAR);
                break;
            }
        case DTL_INTERVAL_INDICATOR_MONTH:
            {
                sDigitValue = aIntervalYearToMonthType->mMonth;
                break;
            }
        default:
            {
                STL_DASSERT( 1 == 0 );
                break;
            }
    }

    sDigitValue = (sDigitValue < 0 ? -sDigitValue : sDigitValue);
    sCmpValue = ( aLeadingPrecision == 2 ? 100 : 
                 (aLeadingPrecision == 3 ? 1000 : 
                 (aLeadingPrecision == 4 ? 10000 : 
                 (aLeadingPrecision == 5 ? 100000 : 
                                           1000000 ) ) ) );
    
    STL_TRY_THROW( sDigitValue < sCmpValue, ERROR_OUT_OF_PRECISION );

    return STL_SUCCESS;
    
    STL_CATCH( ERROR_OUT_OF_PRECISION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_YM_INTERVAL_SOME_FILEDS_TRUNCATED_CONVERSION,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;   
}

/**
 * @brief dtlIntervalDayToSecondType precision 체크 및 fractional second에 맞게 조정.
 * @param[in,out] aIntervalDayToSecondType   aIntervalDayToSecondType
 * @param[in]     aIntervalIndicator         dtlIntervalIndicator
 * @param[in]     aLeadingPrecision          leading precision
 * @param[in]     aFractionalSecondPrecision fractional second
 * @param[out]    aErrorStack                에러 스택
 */
stlStatus dtdAdjustIntervalDayToSecond( dtlIntervalDayToSecondType * aIntervalDayToSecondType,
                                        dtlIntervalIndicator         aIntervalIndicator,
                                        stlInt32                     aLeadingPrecision,
                                        stlInt32                     aFractionalSecondPrecision,
                                        stlErrorStack              * aErrorStack )
{
    stlInt64   sDigitValue;
    stlInt64   sCmpValue;
    
    /*
     * interval second precision에 대한 처리
     */

    switch( aIntervalIndicator )
    {
        case DTL_INTERVAL_INDICATOR_SECOND :
        case DTL_INTERVAL_INDICATOR_DAY_TO_SECOND :
        case DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND :
        case DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND :
            STL_TRY_THROW(
                (aFractionalSecondPrecision >= DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION) &&
                (aFractionalSecondPrecision <= DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION),
                ERROR_INVALID_PARAMETER_VALUE );
    
            if( aIntervalDayToSecondType->mTime >= STL_INT64_C(0) )
            {
                aIntervalDayToSecondType->mTime =
                    ((aIntervalDayToSecondType->mTime +
                      dtlIntervalSecondOffset[aFractionalSecondPrecision]) /
                     dtlIntervalSecondScale[aFractionalSecondPrecision]) *
                    dtlIntervalSecondScale[aFractionalSecondPrecision];
            }
            else
            {
                aIntervalDayToSecondType->mTime =
                    -(((-aIntervalDayToSecondType->mTime +
                        dtlIntervalSecondOffset[aFractionalSecondPrecision]) /
                       dtlIntervalSecondScale[aFractionalSecondPrecision]) *
                      dtlIntervalSecondScale[aFractionalSecondPrecision]);
            }
            break;
        default :
            break;
    }

    /*
     * interval leading precision check
     */

    switch( aIntervalIndicator )
    {
        case DTL_INTERVAL_INDICATOR_DAY:
            /* FALL_THROUGH */            
        case DTL_INTERVAL_INDICATOR_DAY_TO_HOUR:
            /* FALL_THROUGH */                        
        case DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE:
            /* FALL_THROUGH */
        case DTL_INTERVAL_INDICATOR_DAY_TO_SECOND:
            {
                /* usec time -> day 로 계산 */
                sDigitValue = (aIntervalDayToSecondType->mTime == 0) ?
                    0 : (aIntervalDayToSecondType->mTime / DTL_USECS_PER_DAY);
                sDigitValue = sDigitValue + aIntervalDayToSecondType->mDay;

                break;
            }
        case DTL_INTERVAL_INDICATOR_HOUR:
            /* FALL_THROUGH */            
        case DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE:            
            /* FALL_THROUGH */
        case DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND:            
            {
                /* usec time->hour으로 계산 */
                sDigitValue = (aIntervalDayToSecondType->mTime == 0) ?
                    0 : (aIntervalDayToSecondType->mTime / DTL_USECS_PER_HOUR);

                /* day->hour로 계산 */
                sDigitValue = sDigitValue +
                    (aIntervalDayToSecondType->mDay * DTL_HOURS_PER_DAY);
                
                break;
            }
        case DTL_INTERVAL_INDICATOR_MINUTE:
            /* FALL_THROUGH */
        case DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND:
            {
                /* usec time->minute으로 계산 */
                sDigitValue = (aIntervalDayToSecondType->mTime == 0) ?
                    0 : (aIntervalDayToSecondType->mTime / DTL_USECS_PER_MINUTE);

                /* day->minute로 계산 */
                sDigitValue = sDigitValue +
                    (aIntervalDayToSecondType->mDay * DTL_HOURS_PER_DAY * DTL_MINS_PER_HOUR);

                break;
            }
        case DTL_INTERVAL_INDICATOR_SECOND:
            {
                /* usec time -> second로 계산 */
                sDigitValue = aIntervalDayToSecondType->mTime / DTL_USECS_PER_SEC;

                /* day->second로 계산 */
                sDigitValue = sDigitValue +
                    (aIntervalDayToSecondType->mDay * DTL_SECS_PER_DAY);

                break;
            }
        default:
            {
                STL_THROW( RAMP_ERROR_BAD_FORMAT );
                break;
            }
    }

    sDigitValue = (sDigitValue < 0 ? -sDigitValue : sDigitValue);
    sCmpValue = ( aLeadingPrecision == 2 ? 100 : 
                 (aLeadingPrecision == 3 ? 1000 : 
                 (aLeadingPrecision == 4 ? 10000 : 
                 (aLeadingPrecision == 5 ? 100000 : 
                                           1000000 ) ) ) );
    STL_TRY_THROW( sDigitValue < sCmpValue, ERROR_OUT_OF_PRECISION );

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERROR_BAD_FORMAT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_DT_INTERVAL_NOT_VALID_INTERVAL_LITERAL,
                      NULL,
                      aErrorStack );
    };
    
    STL_CATCH( ERROR_INVALID_PARAMETER_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_DT_INTERVAL_NOT_VALID_INTERVAL_LITERAL,
                      NULL,
                      aErrorStack );
    };    
    
    STL_CATCH( ERROR_OUT_OF_PRECISION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_DT_INTERVAL_SOME_FILEDS_TRUNCATED_CONVERSION,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;   
}

/* stlBool dtdCheckDateTokenTable( const dtlDateToken * aBase, */
/*                                 stlInt32             aNum ) */
/* { */
/*     stlBool   sOK; */
/*     stlInt32  i; */

/*     sOK = STL_TRUE; */

/*     for( i = 1; i < aNum; i++ ) */
/*     { */
/*         if( stlStrncmp(aBase[i - 1].mToken, aBase[i].mToken, DTL_DATE_TOKEN_MAX_LEN) >= 0 ) */
/*         { */
/*             sOK = STL_FALSE; */
/*         } */
/*         else */
/*         { */
/*             // Do Nothing */
/*         } */
/*     } */

/*     return sOK; */
/* } */

/**
 * @brief timestamp type -> timestampTz type으로 변환.
 * @param[in]  aTimestampType   timestamp type
 * @param[out] aTimestampTzType timestampTz type
 * @param[in]  aVectorFunc Function Vector
 * @param[in]  aVectorArg  Vector Argument
 * @param[out] aErrorStack      에러스택
 */
stlStatus dtdTimestamp2TimestampTz( dtlTimestampType     aTimestampType,
                                    dtlTimestampTzType * aTimestampTzType,
                                    dtlFuncVector      * aVectorFunc,
                                    void               * aVectorArg,
                                    stlErrorStack      * aErrorStack )
{
    dtlTimestampTzType  sTimestampTzType;
    dtlExpTime          sDtlExpTime;
    dtlFractionalSecond sFractionalSecond;
    stlInt32            sTimeZone;

    sTimeZone = -(aVectorFunc->mGetGMTOffsetFunc(aVectorArg));
    
    STL_TRY( dtdDateTime2dtlExpTime( aTimestampType,
                                     & sDtlExpTime,
                                     & sFractionalSecond,
                                     aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( dtdDtlExpTime2TimestampTz( NULL,
                                        & sDtlExpTime,
                                        sFractionalSecond,
                                        & sTimeZone,
                                        & sTimestampTzType,
                                        aErrorStack )
             == STL_SUCCESS );
    
    aTimestampTzType->mTimestamp = sTimestampTzType.mTimestamp;
    aTimestampTzType->mTimeZone  = sTimestampTzType.mTimeZone;
    
    return STL_SUCCESS;
    
    STL_FINISH;    
    
    return STL_FAILURE;    
}

/**
 * @brief date type -> timestamp without time zone type으로 변환.
 * @param[in]  aDateType       date type
 * @param[out] aTimestampType  timestamp type
 * @param[in]  aVectorFunc     Function Vector
 * @param[in]  aVectorArg      Vector Argument
 * @param[out] aErrorStack     에러스택
 */
stlStatus dtdDateToTimestamp( dtlDateType        aDateType,
                              dtlTimestampType * aTimestampType,
                              dtlFuncVector    * aVectorFunc,
                              void             * aVectorArg,
                              stlErrorStack    * aErrorStack )
{
    *(aTimestampType) = aDateType;
    
    return STL_SUCCESS;
    
    /* STL_FINISH; */
    
    /* return STL_FAILURE; */
}

/**
 * @brief date type -> timestamp with time zone type으로 변환.
 * @param[in]  aDateType         date type
 * @param[out] aTimestampTzType  timestampTz type
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aErrorStack       에러스택
 */
stlStatus dtdDateToTimestampTz( dtlDateType          aDateType,
                                dtlTimestampTzType * aTimestampTzType,
                                dtlFuncVector      * aVectorFunc,
                                void               * aVectorArg,
                                stlErrorStack      * aErrorStack )
{
    dtlTimestampType     sTimestampValue;
    stlInt32             sTimeZone;

    sTimeZone = -(aVectorFunc->mGetGMTOffsetFunc(aVectorArg));

    /**
     * @todo time zone 정리할때 아래 코드 확인하기.
     */ 
    
//        STL_TRY( dtdGetPropertyTimeZone( & gSessionTimeZone,
//                                         sErrorStack )
//                 == STL_SUCCESS );
//        
//        sTimeZone = dtdDetermineTimeZoneOffset( & sDtlExpTime,
//                                                & gSessionTimeZone,
//                                                & gSessionTimeZone );
    
    sTimestampValue = aDateType + (sTimeZone * DTL_USECS_PER_SEC);    
    
    STL_TRY_THROW( (sTimestampValue - (sTimeZone * DTL_USECS_PER_SEC))
                   == aDateType,
                   ERROR_OUT_OF_PRECISION );
    
    aTimestampTzType->mTimestamp = sTimestampValue;
    aTimestampTzType->mTimeZone  = sTimeZone;
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_OUT_OF_PRECISION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_DATE_NOT_VALID_DATE,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;    

    return STL_FAILURE;    
}

/**
 * @brief 숫자값을 주어진 size의 leading zero padding한 format의 string으로 변환한다.
 *   <BR> 예2) 12 => '0012' (%04d) ...
 * @param[in]  aValue             string으로 변환하고자 하는 integer value
 * @param[in]  aSize              출력할 문자열의 길이
 * @param[out] aResultStrSize     변환된 문자열의 길이
 * @param[out] aStr               변환된 string이 저장될 주소
 * @param[out] aErrorStack        stlErrorStack
 */
stlStatus dtdIntegerToLeadingZeroFormatString( stlInt32         aValue,
                                               stlInt32         aSize,
                                               stlInt32       * aResultStrSize,
                                               stlChar        * aStr,
                                               stlErrorStack  * aErrorStack )
{
    stlUInt32   sUValue;
    stlUInt32   sUNewValue;
    stlChar   * sValuePtr = aStr;
    stlChar   * sBufferPtr;
    stlInt32    sLength = 0;
    stlInt32    i;
    stlInt32    sLeadingZeroCnt = 0;
    stlInt32    sSize;

    if( aSize > 0 )
    {
        if( aValue < 0 )
        {
            sSize = aSize + 1;
        
            sUValue = -aValue;
            sBufferPtr = sValuePtr + sSize;
        
            *sValuePtr = '-';
            sValuePtr++;
        }
        else
        {
            sSize = aSize;
        
            sUValue = aValue;
            sBufferPtr = sValuePtr + sSize;
        }

        if( sUValue == 0 )
        {
            // Do Nothing
        }
        else
        {
            do
            {
                sBufferPtr--;
                STL_DASSERT( sValuePtr <= sBufferPtr );

                sLength++;

                sUNewValue = sUValue / 10;
                *sBufferPtr = ( sUValue - ( sUNewValue * 10)) + '0';
                sUValue = sUNewValue;
            }while( (sUValue != 0) && (sValuePtr < sBufferPtr) );
        }

        STL_TRY_THROW( ( sUValue == 0 ) && ( sValuePtr <= sBufferPtr ),
                       ERROR_NOT_ENOUGH_BUFFER );

        sLeadingZeroCnt = ( aSize - sLength );
        STL_DASSERT( ( sLeadingZeroCnt >= 0 ) &&
                     ( (sBufferPtr - sValuePtr) == sLeadingZeroCnt ) );
        
        for( i = 0; i < sLeadingZeroCnt; i++ )
        {
            *sValuePtr = 0 + '0';
            sValuePtr++;
        }
    }
    else
    {
        sSize = 0;
    }
    
    *aResultStrSize = sSize;

    return STL_SUCCESS;

    STL_CATCH( ERROR_NOT_ENOUGH_BUFFER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_NUMBER_BYTE_LENGTH_GREATER_BUFFER_LENGTH,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief 숫자값을 주어진 size의 leading zero padding한 format의 utf16 string으로 변환한다.
 *   <BR> 예2) 12 => '0012' (%04d) ...
 * @param[in]  aValue             string으로 변환하고자 하는 integer value
 * @param[in]  aSize              출력할 문자열의 길이
 * @param[out] aResultStrSize     변환된 문자열의 길이
 * @param[out] aStr               변환된 string이 저장될 주소
 * @param[out] aErrorStack        stlErrorStack
 */
stlStatus dtdIntegerToLeadingZeroFormatUtf16String( stlInt32         aValue,
                                                    stlInt32         aSize,
                                                    stlInt32       * aResultStrSize,
                                                    stlUInt16      * aStr,
                                                    stlErrorStack  * aErrorStack )
{
    stlUInt32   sUValue;
    stlUInt32   sUNewValue;
    stlUInt16 * sValuePtr = aStr;
    stlUInt16 * sBufferPtr;
    stlInt32    sLength = 0;
    stlInt32    i;
    stlInt32    sLeadingZeroCnt = 0;
    stlInt32    sSize;

    if( aSize > 0 )
    {
        if( aValue < 0 )
        {
            sSize = aSize;
        
            sUValue = -aValue;
            sBufferPtr = sValuePtr + sSize;
        
            *sValuePtr = '-';
            sValuePtr++;
        }
        else
        {
            sSize = aSize;
        
            sUValue = aValue;
            sBufferPtr = sValuePtr + sSize;
        }

        if( sUValue == 0 )
        {
            // Do Nothing
        }
        else
        {
            do
            {
                sBufferPtr--;
                STL_DASSERT( sValuePtr <= sBufferPtr );

                sLength++;

                sUNewValue = sUValue / 10;
                *sBufferPtr = ( sUValue - ( sUNewValue * 10)) + '0';
                sUValue = sUNewValue;
            }while( (sUValue != 0) && (sValuePtr < sBufferPtr) );
        }

        STL_TRY_THROW( ( sUValue == 0 ) && ( sValuePtr <= sBufferPtr ),
                       ERROR_NOT_ENOUGH_BUFFER );
        
        sLeadingZeroCnt = ( aSize - sLength );
        STL_DASSERT( ( sLeadingZeroCnt >= 0 ) &&
                     ( (sBufferPtr - sValuePtr) == sLeadingZeroCnt ) );

        for( i = 0; i < sLeadingZeroCnt; i++ )
        {
            *sValuePtr = 0 + '0';
            sValuePtr++;
        }
    }
    else
    {
        sSize = 0;
    }
    
    *aResultStrSize = sSize * STL_UINT16_SIZE;

    return STL_SUCCESS;

    STL_CATCH( ERROR_NOT_ENOUGH_BUFFER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_NUMBER_BYTE_LENGTH_GREATER_BUFFER_LENGTH,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief 숫자값을 주어진 size의 leading zero padding한 format의 utf32 string으로 변환한다.
 *   <BR> 예2) 12 => '0012' (%04d) ...
 * @param[in]  aValue             string으로 변환하고자 하는 integer value
 * @param[in]  aSize              출력할 문자열의 길이
 * @param[out] aResultStrSize     변환된 문자열의 길이
 * @param[out] aStr               변환된 string이 저장될 주소
 * @param[out] aErrorStack        stlErrorStack
 */
stlStatus dtdIntegerToLeadingZeroFormatUtf32String( stlInt32         aValue,
                                                    stlInt32         aSize,
                                                    stlInt32       * aResultStrSize,
                                                    stlUInt32      * aStr,
                                                    stlErrorStack  * aErrorStack )
{
    stlUInt32   sUValue;
    stlUInt32   sUNewValue;
    stlUInt32 * sValuePtr = aStr;
    stlUInt32 * sBufferPtr;
    stlInt32    sLength = 0;
    stlInt32    i;
    stlInt32    sLeadingZeroCnt = 0;
    stlInt32    sSize;

    if( aSize > 0 )
    {
        if( aValue < 0 )
        {
            sSize = aSize;
        
            sUValue = -aValue;
            sBufferPtr = sValuePtr + sSize;
        
            *sValuePtr = '-';
            sValuePtr++;
        }
        else
        {
            sSize = aSize;
        
            sUValue = aValue;
            sBufferPtr = sValuePtr + sSize;
        }

        if( sUValue == 0 )
        {
            // Do Nothing
        }
        else
        {
            do
            {
                sBufferPtr--;
                STL_DASSERT( sValuePtr <= sBufferPtr );

                sLength++;

                sUNewValue = sUValue / 10;
                *sBufferPtr = ( sUValue - ( sUNewValue * 10)) + '0';
                sUValue = sUNewValue;
            }while( (sUValue != 0) && (sValuePtr < sBufferPtr) );
        }

        STL_TRY_THROW( ( sUValue == 0 ) && ( sValuePtr <= sBufferPtr ),
                       ERROR_NOT_ENOUGH_BUFFER );
        
        sLeadingZeroCnt = ( aSize - sLength );
        STL_DASSERT( ( sLeadingZeroCnt >= 0 ) &&
                     ( (sBufferPtr - sValuePtr) == sLeadingZeroCnt ) );

        for( i = 0; i < sLeadingZeroCnt; i++ )
        {
            *sValuePtr = 0 + '0';
            sValuePtr++;
        }
    }
    else
    {
        sSize = 0;
    }
    
    *aResultStrSize = sSize * STL_UINT32_SIZE;

    return STL_SUCCESS;

    STL_CATCH( ERROR_NOT_ENOUGH_BUFFER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_NUMBER_BYTE_LENGTH_GREATER_BUFFER_LENGTH,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief stlTime을 지정된 time zone으로 DATE타입의 value 구성
 *   <BR> stlTime(stlNow()) -> date 로 변환.
 * @param[in]  aStlTime         value에 저장될 입력 숫자
 * @param[in]  aTimeZoneOffset  value에 적용할 time zone offset
 * @param[out] aValue           dtlDataValue의 주소
 * @param[out] aErrorStack      에러 스택
 */
stlStatus dtdDateSetValueFromStlTimeAndTimeZone( stlTime           aStlTime,
                                                 stlInt32          aTimeZoneOffset,
                                                 dtlDataValue    * aValue,
                                                 stlErrorStack   * aErrorStack )
{
    dtlDateType    sTimestampUnixTZ;
    dtlDateType    sTimestampJulian;
    dtlDateType    sTimestampSec;

    stlInt32       sGMTOffset = 0;

    /**
     * Parameter Validation
     */

    DTL_CHECK_TYPE( DTL_TYPE_DATE, aValue, aErrorStack );

    /**
     * 지정된 TimeZone 값으로 변경 
     */

    sGMTOffset = aTimeZoneOffset;
    sTimestampUnixTZ = aStlTime + (sGMTOffset * DTL_USECS_PER_SEC);
    
    /**
     * Unix Time 을 Julian Time 으로 변경
     */
    
    DTL_TIMESTAMP_MODULO( sTimestampUnixTZ, sTimestampSec, DTL_USECS_PER_SEC );
    
    sTimestampSec =
        sTimestampSec - ((DTL_EPOCH_JDATE - DTL_UNIX_EPOCH_JDATE) * DTL_SECS_PER_DAY);
    sTimestampJulian = (sTimestampSec * DTL_USECS_PER_SEC) + sTimestampUnixTZ;

    /**
     * Precision 보정
     */
    
    STL_TRY( dtdAdjustDate( & sTimestampJulian,
                            aErrorStack )
             == STL_SUCCESS );
    
    /**
     * Result 설정
     */
    
    *(dtlDateType *)(aValue->mValue) = sTimestampJulian;
    aValue->mLength = DTL_DATE_SIZE;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief stlTime을 지정된 time zone으로 TIME WITH TIME ZONE 타입의 value 구성
 *   <BR> 예) stlTime(stlNow()) -> timetz 로 변환.
 * @param[in]  aStlTime         value에 저장될 입력 숫자
 * @param[in]  aTimeZoneOffset  value에 적용할 time zone offset
 * @param[in]  aPrecision       precision
 * @param[out] aValue           dtlDataValue의 주소
 * @param[out] aErrorStack      에러 스택
 */
stlStatus dtdTimeTzSetValueFromStlTimeAndTimeZone( stlTime           aStlTime,
                                                   stlInt32          aTimeZoneOffset,
                                                   stlInt64          aPrecision,
                                                   dtlDataValue    * aValue,
                                                   stlErrorStack   * aErrorStack )
{
    dtlTimestampType    sTimestampUnixTZ;

    dtlTimeTzType       sTimeTzValue;

    dtlTimeOffset       sTimeOffset;
    
    stlInt32 sGMTOffset = 0;

    /**
     * Parameter Validation
     */

    DTL_CHECK_TYPE( DTL_TYPE_TIME_WITH_TIME_ZONE, aValue, aErrorStack );
    DTL_PARAM_VALIDATE( (aPrecision >= DTL_TIMETZ_MIN_PRECISION) &&
                        (aPrecision <= DTL_TIMETZ_MAX_PRECISION),
                        aErrorStack );
    
    /**
     * 지정된 TimeZone 값으로 변경 
     */

    sGMTOffset = aTimeZoneOffset;
    sTimestampUnixTZ = aStlTime;

    /**
     * Timestamp TZ 를 Time TZ 로 변경
     */
    
    /*
     * time의 범위 체크
     * '00:00:00.000000' ~ '23:59:59.999999'
     */
    
    sTimeOffset = sTimestampUnixTZ + (sGMTOffset * DTL_USECS_PER_SEC);
    sTimeOffset = sTimeOffset % DTL_USECS_PER_DAY;
    sTimeOffset -= (sGMTOffset * DTL_USECS_PER_SEC);

    sTimeTzValue.mTime = sTimeOffset;
    sTimeTzValue.mTimeZone = -sGMTOffset;
    
    
    /**
     * Precision 보정
     */
    STL_TRY( dtdAdjustTime( &(sTimeTzValue.mTime),
                            aPrecision,
                            aErrorStack )
             == STL_SUCCESS );    
    
    /**
     * Result 설정
     */

    stlMemcpy( aValue->mValue,
               & sTimeTzValue,
               DTL_TIMETZ_SIZE );
    
    aValue->mLength = DTL_TIMETZ_SIZE;
                          
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief stlTime을 지정된 time zone으로 TIMESTAMP WITH TIME ZONE 타입의 value 구성
 *   <BR> 예) stlTime(stlNow()) -> timestamptz 로 변환.
 * @param[in]  aStlTime         value에 저장될 입력 숫자
 * @param[in]  aTimeZoneOffset  value에 적용할 time zone offset
 * @param[in]  aPrecision      
 * @param[out] aValue           dtlDataValue의 주소
 * @param[out] aErrorStack      에러 스택
 */
stlStatus dtdTimestampTzSetValueFromStlTimeAndTimeZone( stlTime           aStlTime,
                                                        stlInt32          aTimeZoneOffset,
                                                        stlInt64          aPrecision,
                                                        dtlDataValue    * aValue,
                                                        stlErrorStack   * aErrorStack )
{
    dtlTimestampType    sTimestampUnixTZ;
    dtlTimestampType    sTimestampJulian;
    dtlTimestampType    sTimestampSec;

    dtlTimestampTzType  sValueTimestampTZ;
    
    stlInt32   sGMTOffset;

    /**
     * Parameter Validation
     */

    DTL_CHECK_TYPE( DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, aValue, aErrorStack );
    STL_PARAM_VALIDATE( (aPrecision >= DTL_TIMESTAMPTZ_MIN_PRECISION) &&
                        (aPrecision <= DTL_TIMESTAMPTZ_MAX_PRECISION),
                        aErrorStack );

    /**
     * 지정된 DB TimeZone 값으로 변경 
     */

    sGMTOffset = aTimeZoneOffset;

    sTimestampUnixTZ = aStlTime;
    
    /**
     * Unix Time 을 Julian Time 으로 변경
     */
    
    DTL_TIMESTAMP_MODULO( sTimestampUnixTZ, sTimestampSec, DTL_USECS_PER_SEC );
    
    sTimestampSec =
        sTimestampSec - ((DTL_EPOCH_JDATE - DTL_UNIX_EPOCH_JDATE) * DTL_SECS_PER_DAY);
    sTimestampJulian = (sTimestampSec * DTL_USECS_PER_SEC) + sTimestampUnixTZ;

    sValueTimestampTZ.mTimestamp = sTimestampJulian;
    sValueTimestampTZ.mTimeZone  = -sGMTOffset;
    
    /**
     * Precision 보정
     */
    
    STL_TRY( dtdAdjustTimestamp( & (sValueTimestampTZ.mTimestamp),
                                 aPrecision,
                                 aErrorStack )
             == STL_SUCCESS );

    /**
     * Result 설정
     */

    ((dtlTimestampTzType *)(aValue->mValue))->mTimestamp = sValueTimestampTZ.mTimestamp;
    ((dtlTimestampTzType *)(aValue->mValue))->mTimeZone = sValueTimestampTZ.mTimeZone;
    aValue->mLength = DTL_TIMESTAMPTZ_SIZE;

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief timestamp with time zone type 값을 date type의 값으로 반환한다.
 * @param[in]  aTimestampTz   timestamp with time zone type value
 * @param[out] aDate          date type value
 * @param[out] aErrorStack    에러스택
 */
stlStatus dtdTimestampTzToDate( dtlTimestampTzType  * aTimestampTz,
                                dtlDateType         * aDate,
                                stlErrorStack       * aErrorStack )
{
    dtlDateType  sDate = 0;
    
    DTL_GET_LOCAL_TIMESTAMP_FROM_TIMESTAMPTZ( aTimestampTz,
                                              sDate );
    
    STL_TRY( dtdAdjustDate( & sDate,
                            aErrorStack )
             == STL_SUCCESS );
    
    *aDate = sDate;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief timestamp with time zone type 값을 time with time zone type의 값으로 반환한다.
 * @param[in]  aTimestampTz   timestamp with time zone type value
 * @param[in]  aPrecision     precision( time with time zone type )
 * @param[out] aTimeTz        time with time zone type value
 * @param[out] aErrorStack    에러스택
 */
stlStatus dtdTimestampTzToTimeTz( dtlTimestampTzType  * aTimestampTz,
                                  stlInt64              aPrecision,
                                  dtlTimeTzType       * aTimeTz,
                                  stlErrorStack       * aErrorStack )
{
    dtlTimeType  sTime = 0;
    dtlDateType  sDate = 0;
    
    DTL_GET_LOCAL_TIMESTAMP_FROM_TIMESTAMPTZ( aTimestampTz,
                                              sTime );

    DTL_TIMESTAMP_MODULO( sTime, sDate, DTL_USECS_PER_DAY );
    
    STL_TRY( dtdAdjustTime( & sTime,
                            aPrecision,
                            aErrorStack )
             == STL_SUCCESS );
    
    sTime -= sTime / DTL_USECS_PER_DAY * DTL_USECS_PER_DAY;
    
    if( sTime < 0 )
    {
        sTime += DTL_USECS_PER_DAY;
    }
    else
    {
        // Do Nothing
    }

    aTimeTz->mTime = sTime + (aTimestampTz->mTimeZone * DTL_USECS_PER_SEC);
    aTimeTz->mTimeZone = aTimestampTz->mTimeZone;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief timestamp with time zone type 값을 time without time zone type의 값으로 반환한다.
 * @param[in]  aTimestampTz   timestamp with time zone type value
 * @param[in]  aPrecision     precision( time without time zone type )
 * @param[out] aTime          time without time zone type value
 * @param[out] aErrorStack    에러스택
 */
stlStatus dtdTimestampTzToTime( dtlTimestampTzType  * aTimestampTz,
                                stlInt64              aPrecision,
                                dtlTimeType         * aTime,
                                stlErrorStack       * aErrorStack )
{
    dtlTimeType  sTime = 0;
    dtlDateType  sDate = 0;
    
    DTL_GET_LOCAL_TIMESTAMP_FROM_TIMESTAMPTZ( aTimestampTz,
                                              sTime );

    DTL_TIMESTAMP_MODULO( sTime, sDate, DTL_USECS_PER_DAY );
    
    STL_TRY( dtdAdjustTime( & sTime,
                            aPrecision,
                            aErrorStack )
             == STL_SUCCESS );
    
    sTime -= sTime / DTL_USECS_PER_DAY * DTL_USECS_PER_DAY;
    
    if( sTime < 0 )
    {
        sTime += DTL_USECS_PER_DAY;
    }
    else
    {
        // Do Nothing
    }

    *aTime = sTime;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief timestamp with time zone type 값을 timestamp without time zone type의 값으로 반환한다.
 * @param[in]  aTimestampTz   timestamp with time zone type value
 * @param[in]  aPrecision     precision( timestamp without time zone type )
 * @param[out] aTimestamp     timestamp without time zone type value
 * @param[out] aErrorStack    에러스택
 */
stlStatus dtdTimestampTzToTimestamp( dtlTimestampTzType  * aTimestampTz,
                                     stlInt64              aPrecision,
                                     dtlTimestampType    * aTimestamp,
                                     stlErrorStack       * aErrorStack )
{
    dtlTimestampType  sTimestamp = 0;
    stlBool           sIsValid   = STL_FALSE;
    
    DTL_GET_LOCAL_TIMESTAMP_FROM_TIMESTAMPTZ( aTimestampTz,
                                              sTimestamp );
    
    STL_TRY( dtdAdjustTimestamp( & sTimestamp,
                                 aPrecision,
                                 aErrorStack )
             == STL_SUCCESS );
    
    *aTimestamp = sTimestamp;

    DTF_DATETIME_VALUE_IS_VALID( sTimestamp, sIsValid );
    STL_TRY_THROW( sIsValid == STL_TRUE, ERROR_OUT_OF_RANGE );
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATETIME_FIELD_OVERFLOW,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/** @} */
