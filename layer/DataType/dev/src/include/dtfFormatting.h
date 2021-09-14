/*******************************************************************************
 * dtfFormatting.h
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


#ifndef _DTF_FORMATTING_H_
#define _DTF_FORMATTING_H_ 1

/**
 * @file dtfFormatting.h
 * @brief DataType Layer Internal Definitions
 */



/**
 * @defgroup dtInternal DataType Layer Internal Definitions
 * @internal
 * @{
 */


/**
 * @defgroup dtf Function
 * @ingroup dtInternal
 * @{
 */

typedef struct dtfDateSetInfo
{
    stlBool        mDay;
    stlBool        mMonth;
    stlBool        mYear;
    stlBool        mTimeZoneHour;
    stlBool        mTimeZoneMinute;
} dtfDateSetInfo;

/**
 * dtfDateSetInfo struct init
 */
#define DTF_INIT_DATE_SET_INFO( aDateSetInfo )                \
    {                                                         \
        (aDateSetInfo).mDay            = STL_FALSE;           \
        (aDateSetInfo).mMonth          = STL_FALSE;           \
        (aDateSetInfo).mYear           = STL_FALSE;           \
        (aDateSetInfo).mTimeZoneHour   = STL_FALSE;           \
        (aDateSetInfo).mTimeZoneMinute = STL_FALSE;           \
    }


/**
 * dtlToDateTimeFormatElementSetInfo struct init
 */
#define DTF_INIT_TO_DATETIME_FORMAT_ELEMENT_SET_INFO( aDtlToDateTimeFormatElementSetInfo )  \
    {                                                                                       \
        (aDtlToDateTimeFormatElementSetInfo)->mFractionalSecond = STL_FALSE;                \
        (aDtlToDateTimeFormatElementSetInfo)->mSecond      = STL_FALSE;              \
        (aDtlToDateTimeFormatElementSetInfo)->mMinute      = STL_FALSE;              \
        (aDtlToDateTimeFormatElementSetInfo)->mHour        = STL_FALSE;              \
        (aDtlToDateTimeFormatElementSetInfo)->mDay         = STL_FALSE;              \
        (aDtlToDateTimeFormatElementSetInfo)->mMonth       = STL_FALSE;              \
        (aDtlToDateTimeFormatElementSetInfo)->mYear        = STL_FALSE;              \
        (aDtlToDateTimeFormatElementSetInfo)->mWeekDay     = STL_FALSE;              \
        (aDtlToDateTimeFormatElementSetInfo)->mYearDay     = STL_FALSE;              \
        (aDtlToDateTimeFormatElementSetInfo)->mSssss       = STL_FALSE;              \
        (aDtlToDateTimeFormatElementSetInfo)->mJulianDay   = STL_FALSE;              \
        (aDtlToDateTimeFormatElementSetInfo)->mBc          = STL_FALSE;              \
        (aDtlToDateTimeFormatElementSetInfo)->mAmPm        = STL_FALSE;              \
        (aDtlToDateTimeFormatElementSetInfo)->mTimeZoneHour   = STL_FALSE;           \
        (aDtlToDateTimeFormatElementSetInfo)->mTimeZoneMinute = STL_FALSE;           \
    }

typedef struct dtfDateTimeFmtStr
{
    const stlChar  * mStr;
    const stlUInt8   mStrLen;    
    const stlUInt8   mMaxLen;
    const stlUInt8   mDateInfo;
} dtfDateTimeFmtStr;

/***************
 * format node의 구분
 ***************/

#define DTF_FORMAT_NODE_TYPE_END        1
#define DTF_FORMAT_NODE_TYPE_KEYWORD   	2
#define DTF_FORMAT_NODE_TYPE_CHAR       3

/***************
 * DATETIME 의 format element
 ***************/

typedef enum dtfDateTimeFormatElement
{
    DTF_DATETIME_FMT_A_D,
    DTF_DATETIME_FMT_A_M,
    DTF_DATETIME_FMT_AD,
    DTF_DATETIME_FMT_AM,
    DTF_DATETIME_FMT_Am,
    DTF_DATETIME_FMT_B_C,
    DTF_DATETIME_FMT_BC,
    DTF_DATETIME_FMT_CC,
    DTF_DATETIME_FMT_DAY,
    DTF_DATETIME_FMT_Day,
    DTF_DATETIME_FMT_DDD,
    DTF_DATETIME_FMT_DD,
    DTF_DATETIME_FMT_DY,
    DTF_DATETIME_FMT_Dy,   
    DTF_DATETIME_FMT_D,    
    DTF_DATETIME_FMT_FF1,
    DTF_DATETIME_FMT_FF2,
    DTF_DATETIME_FMT_FF3,
    DTF_DATETIME_FMT_FF4,
    DTF_DATETIME_FMT_FF5,
    DTF_DATETIME_FMT_FF6,
    DTF_DATETIME_FMT_FF,
    DTF_DATETIME_FMT_HH24,
    DTF_DATETIME_FMT_HH12,
    DTF_DATETIME_FMT_HH,
    DTF_DATETIME_FMT_J,
    DTF_DATETIME_FMT_MI,
    DTF_DATETIME_FMT_MM,
    DTF_DATETIME_FMT_MONTH,
    DTF_DATETIME_FMT_Month,
    DTF_DATETIME_FMT_MON,
    DTF_DATETIME_FMT_Mon,
    DTF_DATETIME_FMT_P_M,
    DTF_DATETIME_FMT_PM,
    DTF_DATETIME_FMT_Pm,
    DTF_DATETIME_FMT_Q,
    DTF_DATETIME_FMT_RM,
    DTF_DATETIME_FMT_Rm,
    DTF_DATETIME_FMT_RRRR,
    DTF_DATETIME_FMT_RR,
    DTF_DATETIME_FMT_SSSSS,    
    DTF_DATETIME_FMT_SYYYY,
    DTF_DATETIME_FMT_SS,
    DTF_DATETIME_FMT_TZH,
    DTF_DATETIME_FMT_TZM,
    DTF_DATETIME_FMT_WW,
    DTF_DATETIME_FMT_W,
    DTF_DATETIME_FMT_Y_YYY,
    DTF_DATETIME_FMT_YYYY,
    DTF_DATETIME_FMT_YYY,
    DTF_DATETIME_FMT_YY,
    DTF_DATETIME_FMT_Y,

    
    DTF_DATETIME_FMT_a_d,
    DTF_DATETIME_FMT_a_m,
    DTF_DATETIME_FMT_ad,
    DTF_DATETIME_FMT_am,    
    DTF_DATETIME_FMT_b_c,
    DTF_DATETIME_FMT_bc,
    DTF_DATETIME_FMT_cc,
    DTF_DATETIME_FMT_day,
    DTF_DATETIME_FMT_ddd,    
    DTF_DATETIME_FMT_dd,
    DTF_DATETIME_FMT_dy,
    DTF_DATETIME_FMT_d,        
    DTF_DATETIME_FMT_ff1,
    DTF_DATETIME_FMT_ff2,
    DTF_DATETIME_FMT_ff3,
    DTF_DATETIME_FMT_ff4,
    DTF_DATETIME_FMT_ff5,
    DTF_DATETIME_FMT_ff6,
    DTF_DATETIME_FMT_ff,
    DTF_DATETIME_FMT_hh24,
    DTF_DATETIME_FMT_hh12,
    DTF_DATETIME_FMT_hh,
    DTF_DATETIME_FMT_j,
    DTF_DATETIME_FMT_mi,
    DTF_DATETIME_FMT_mm,        
    DTF_DATETIME_FMT_month,    
    DTF_DATETIME_FMT_mon,
    DTF_DATETIME_FMT_p_m,    
    DTF_DATETIME_FMT_pm,
    DTF_DATETIME_FMT_q,
    DTF_DATETIME_FMT_rm,
    DTF_DATETIME_FMT_rrrr,
    DTF_DATETIME_FMT_rr,
    DTF_DATETIME_FMT_sssss,    
    DTF_DATETIME_FMT_syyyy,
    DTF_DATETIME_FMT_ss,
    DTF_DATETIME_FMT_tzh,
    DTF_DATETIME_FMT_tzm,
    DTF_DATETIME_FMT_ww,
    DTF_DATETIME_FMT_w,
    DTF_DATETIME_FMT_y_yyy,
    DTF_DATETIME_FMT_yyyy,
    DTF_DATETIME_FMT_yyy,
    DTF_DATETIME_FMT_yy,
    DTF_DATETIME_FMT_y,
    
    
    DTF_DATETIME_FMT_MAX
    
} dtfDateTimeFormatElement;


/***************
 * DATETIME 의 format element의 index
 ***************/

extern const stlInt32 DTF_DATETIME_FORMAT_INDEX[256];

extern const stlInt32 DTF_DATETIME_FORMAT_SPECIAL_CHAR_INDEX[256];

/***************
 * 세기 표시
 ***************/

/**
 * @brief 년도 ( dtlExpTime->mYear의 값을 부호를 제외한 년도로 반환 )
 */
#define DTF_DATETIME_FMT_GET_YEAR( aYear, aIsInterval ) ( (aIsInterval) == STL_TRUE ? (aYear) : (aYear) <= 0 ? -((aYear) - 1) : (aYear) )

#define DTF_DATETIME_FMT_A_D_STR_LEN  ( 4 )
#define DTF_DATETIME_FMT_AD_STR_LEN   ( 2 )

#define DTF_DATETIME_FMT_B_C_STR_LEN  ( 4 )
#define DTF_DATETIME_FMT_BC_STR_LEN   ( 2 )

#define DTF_DATETIME_FMT_A_D_STR     "A.D."
#define DTF_DATETIME_FMT_a_d_STR     "a.d."
#define DTF_DATETIME_FMT_AD_STR      "AD"
#define DTF_DATETIME_FMT_ad_STR      "ad"

#define DTF_DATETIME_FMT_B_C_STR     "B.C."
#define DTF_DATETIME_FMT_b_c_STR     "b.c."
#define DTF_DATETIME_FMT_BC_STR      "BC"
#define DTF_DATETIME_FMT_bc_STR      "bc"


/***************
 * AM, PM 표시
 ***************/

#define DTF_DATETIME_FMT_A_M_STR_LEN ( 4 )
#define DTF_DATETIME_FMT_AM_STR_LEN  ( 2 )

#define DTF_DATETIME_FMT_P_M_STR_LEN ( 4 )
#define DTF_DATETIME_FMT_PM_STR_LEN  ( 2 )

#define DTF_DATETIME_FMT_A_M_STR     "A.M."
#define DTF_DATETIME_FMT_a_m_STR     "a.m."
#define DTF_DATETIME_FMT_AM_STR      "AM"
#define DTF_DATETIME_FMT_Am_STR      "Am"
#define DTF_DATETIME_FMT_am_STR      "am"

#define DTF_DATETIME_FMT_P_M_STR     "P.M."
#define DTF_DATETIME_FMT_p_m_STR     "p.m."
#define DTF_DATETIME_FMT_PM_STR      "PM"
#define DTF_DATETIME_FMT_Pm_STR      "Pm"
#define DTF_DATETIME_FMT_pm_STR      "pm"


/*******************************
 * MON, Mon, mon 표시
 *******************************/

#define DTF_DATETIME_FMT_MON_STR_LEN       ( 3 )
#define DTF_DATETIME_FMT_MON_MIN_STR_LEN   DTF_DATETIME_FMT_MON_STR_LEN
#define DTF_DATETIME_FMT_MON_MAX_STR_LEN   DTF_DATETIME_FMT_MON_STR_LEN

extern dtfDateTimeFmtStr const DTF_DATETIME_FMT_MON_UPPER_STR[];
extern dtfDateTimeFmtStr const DTF_DATETIME_FMT_MON_FIRST_UPPER_STR[];
extern dtfDateTimeFmtStr const DTF_DATETIME_FMT_MON_LOWER_STR[];

/*******************************
 * MONTH, Month, month 표시
 *******************************/

#define DTF_DATETIME_FMT_MONTH_MIN_STR_LEN   ( 3 )
#define DTF_DATETIME_FMT_MONTH_MAX_STR_LEN   ( 9 )

extern dtfDateTimeFmtStr const DTF_DATETIME_FMT_MONTH_UPPER_STR[];
extern dtfDateTimeFmtStr const DTF_DATETIME_FMT_MONTH_FIRST_UPPER_STR[];
extern dtfDateTimeFmtStr const DTF_DATETIME_FMT_MONTH_LOWER_STR[];

/*******************************
 * 월(month)의 로마 숫자 표시
 *******************************/

/* DTF_DATETIME_FMT_RM_MONTH_UPPER_STR 배열 전체크기는 DTF_DATETIME_FMT_RM_MONTH_MAX + 1 이 됨. */
#define DTF_DATETIME_FMT_RM_MONTH_MAX ( 12 )

#define DTF_DATETIME_FMT_RM_MONTH_MIN_STR_LEN   ( 1 )
#define DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN   ( 4 )

extern dtfDateTimeFmtStr const DTF_DATETIME_FMT_RM_MONTH_UPPER_STR[];
extern dtfDateTimeFmtStr const DTF_DATETIME_FMT_RM_MONTH_FIRST_UPPER_STR[];
extern dtfDateTimeFmtStr const DTF_DATETIME_FMT_RM_MONTH_LOWER_STR[];

/*******************************
 * 일(day)의 dayname 표시
 *******************************/

#define DTF_DATETIME_FMT_DAY_MIN_STR_LEN   ( 6 )
#define DTF_DATETIME_FMT_DAY_MAX_STR_LEN   ( 9 )

extern dtfDateTimeFmtStr const DTF_DATETIME_FMT_DAY_UPPER_STR[];
extern dtfDateTimeFmtStr const DTF_DATETIME_FMT_DAY_FIRST_UPPER_STR[];
extern dtfDateTimeFmtStr const DTF_DATETIME_FMT_DAY_LOWER_STR[];

/*******************************
 * 일(dy)의 Abbreviated name of day. 표시
 *******************************/

#define DTF_DATETIME_FMT_DY_STR_LEN       ( 3 )
#define DTF_DATETIME_FMT_DY_MIN_STR_LEN   DTF_DATETIME_FMT_DY_STR_LEN
#define DTF_DATETIME_FMT_DY_MAX_STR_LEN   DTF_DATETIME_FMT_DY_STR_LEN

extern dtfDateTimeFmtStr const DTF_DATETIME_FMT_DY_UPPER_STR[];
extern dtfDateTimeFmtStr const DTF_DATETIME_FMT_DY_FIRST_UPPER_STR[];
extern dtfDateTimeFmtStr const DTF_DATETIME_FMT_DY_LOWER_STR[];

/************************************
 * DATETIME 을 위한 keywords
 ************************************/

extern const dtlDateTimeFormatKeyWord dtfDateTimeFormattingKeyword[];


/************************************
 * 
 ************************************/

#define DTF_DATE_MIN_YEAR_STRING          "4714 BC"
#define DTF_DATE_MAX_YEAR_STRING          "9999 AD"
#define DTF_DATE_DAY_OF_WEEK_STRING       "day of week"
#define DTF_DATE_DAY_OF_MONTH_STRING      "day of month"
#define DTF_DATE_JULIAN_DATE_STRING       "Julian date"
#define DTF_DATE_YEAR_STRING              "year"
#define DTF_DATE_MONTH_STRING             "month"
#define DTF_DATE_HOUR_STRING              "hour"
#define DTF_DATE_FRACTIONAL_SECOND_STRING "fractional second"
#define DTF_DATE_BC_AD_STRING             "BC/AD"
#define DTF_DATE_LONG_BC_AD_STRING        "B.C./A.D."
#define DTF_DATE_AM_PM_STRING             "AM/PM"
#define DTF_DATE_LONG_AM_PM_STRING        "A.M./P.M."
#define DTF_DATE_SECONDS_IN_DAY_STRING    "seconds in day"
#define DTF_DATE_MINUTES_OF_HOUR_STRING   "minutes of hour"
#define DTF_DATE_SECONDS_OF_MINUTE_STRING "seconds of minute"

#define DTF_FIRST_DAY_OF_MONTH      ( 1 )
#define DTF_MAX_LAST_DAYS_PER_MONTH ( 31 )

#define DTF_FIRST_MONTH_OF_YEAR     ( 1 )

#define DTF_FIRST_DAY_OF_YEAR       ( 1 )
#define DTF_LAST_DAYS_OF_YEAR       ( 365 )
#define DTF_LAST_DAYS_OF_LEAP_YEAR  ( 366 )

#define DTF_ONE_THOUSAND_YEAR       ( 1000 )
#define DTF_ONE_HUNDRED_YEAR        ( 100 )
#define DTF_TEN_YEAR                ( 10 )
#define DTF_TWO_DIGIT               ( 2 )

#define DTF_HH24_MIN                ( 0 )
#define DTF_HH24_MAX                ( 23 )

#define DTF_HH12_MIN                ( 1 )
#define DTF_HH12_MAX                ( 12 )

#define DTF_MINUTE_MIN              ( 0 )
#define DTF_MINUTE_MAX              ( 59 )

#define DTF_SECOND_MIN              ( 0 )
#define DTF_SECOND_MAX              ( 59 )

#define DTF_FRACTIONAL_SECOND_MIN     ( 0 )
#define DTF_FRACTIONAL_SECOND_FF1_MAX ( 9 )
#define DTF_FRACTIONAL_SECOND_FF2_MAX ( 99 )
#define DTF_FRACTIONAL_SECOND_FF3_MAX ( 999 )
#define DTF_FRACTIONAL_SECOND_FF4_MAX ( 9999 )
#define DTF_FRACTIONAL_SECOND_FF5_MAX ( 99999 )
#define DTF_FRACTIONAL_SECOND_FF6_MAX ( 999999 )


/************************************
 * NUMBER 의 format element
 ************************************/

typedef enum dtfNumberFormatElement
{
    DTF_NUMBER_FMT_DOLLAR,
    DTF_NUMBER_FMT_COMMA,
    DTF_NUMBER_FMT_PERIOD,
    
    DTF_NUMBER_FMT_0,    
    DTF_NUMBER_FMT_9,
    DTF_NUMBER_FMT_B,
    DTF_NUMBER_FMT_EEEE,
    DTF_NUMBER_FMT_FM,    
    DTF_NUMBER_FMT_MI,
    DTF_NUMBER_FMT_PR,
    DTF_NUMBER_FMT_RN,
    DTF_NUMBER_FMT_S,
    DTF_NUMBER_FMT_V,
    DTF_NUMBER_FMT_X,
    
    DTF_NUMBER_FMT_b,
    DTF_NUMBER_FMT_eeee,
    DTF_NUMBER_FMT_fm,        
    DTF_NUMBER_FMT_mi,
    DTF_NUMBER_FMT_pr,
    DTF_NUMBER_FMT_rn,
    DTF_NUMBER_FMT_s,
    DTF_NUMBER_FMT_v,
    DTF_NUMBER_FMT_x,
    
    DTF_NUMBER_FMT_MAX
} dtfNumberFormatElement;

/***************************************
 * NUMBER 를 위한 keywords
 ***************************************/

typedef struct dtfNumberFormatKeyWord
{
    const stlChar      * mName;
    stlUInt8             mLen;
    stlInt32             mId;
} dtfNumberFormatKeyWord;

//const dtfNumberFormatKeyWord * dtfNumberFormattingKeyword[];

typedef struct dtfRomanNumeral
{
    const stlChar    * mStr;            /**< 로만문자열 포인터 */
    stlUInt8           mStrLen;         /**< 로만문자열 길이 */
    stlUInt16          mDecimalValue;   /**< 10진수 값 */
} dtfRomanNumeral;

#define DTF_NUMBER_FMT_LOWER_IDX   ( 0 )
#define DTF_NUMBER_FMT_UPPER_IDX   ( 1 )

/***************************************
 * NUMBER 를 위한 definition
 ***************************************/

/* dtfNumberFormatInfo.mFlag */

#define DTF_NUMBER_FMT_FLAG_PERIOD      ( 1 << 1 )   // '.'
#define DTF_NUMBER_FMT_FLAG_ZERO        ( 1 << 2 )   // '0'
#define DTF_NUMBER_FMT_FLAG_BLANK       ( 1 << 3 )   // 'B'
#define DTF_NUMBER_FMT_FLAG_SIGN        ( 1 << 4 )   // 'S'
#define DTF_NUMBER_FMT_FLAG_MINUS       ( 1 << 5 )   // 'MI'
#define DTF_NUMBER_FMT_FLAG_BRACKET     ( 1 << 6 )   // 'PR'
#define DTF_NUMBER_FMT_FLAG_DOLLAR      ( 1 << 7 )   // '$'
#define DTF_NUMBER_FMT_FLAG_EEEE        ( 1 << 8 )   // 'EEEE'
#define DTF_NUMBER_FMT_FLAG_V           ( 1 << 9 )   // 'V'
#define DTF_NUMBER_FMT_FLAG_RN          ( 1 << 10 )  // 'RN'
#define DTF_NUMBER_FMT_FLAG_X           ( 1 << 11 )  // 'X'
#define DTF_NUMBER_FMT_FLAG_FM          ( 1 << 12 )  // 'FM'


/* dtfNumberFormatInfo.mSignLocate */

#define DTF_NUMBER_SIGN_LOCATE_PRE   ( -1 )
#define DTF_NUMBER_SIGN_LOCATE_POST  ( 1 )
#define DTF_NUMBER_SIGN_LOCATE_NONE  ( 0 )

#define DTF_NUMBER_DIGIT_COUNT_FOR_X_FORMAT ( 76 )
#define DTF_NUMBER_MAX_DISPLAY_COUNT_FOR_X_FORMAT   ( 63 )


#define DTF_NUMBER_FMT_FLAG_IS_PERIOD( aFormat )                                        \
    ( ((aFormat)->mFlag & DTF_NUMBER_FMT_FLAG_PERIOD) > 0 ? STL_TRUE : STL_FALSE )
#define DTF_NUMBER_FMT_FLAG_IS_ZERO( aFormat )                                          \
    ( ((aFormat)->mFlag & DTF_NUMBER_FMT_FLAG_ZERO) > 0 ? STL_TRUE : STL_FALSE )
#define DTF_NUMBER_FMT_FLAG_IS_BLANK( aFormat )                                         \
    ( ((aFormat)->mFlag & DTF_NUMBER_FMT_FLAG_BLANK) > 0 ? STL_TRUE : STL_FALSE )
#define DTF_NUMBER_FMT_FLAG_IS_SIGN( aFormat )                                          \
    ( ((aFormat)->mFlag & DTF_NUMBER_FMT_FLAG_SIGN) > 0 ? STL_TRUE : STL_FALSE )
#define DTF_NUMBER_FMT_FLAG_IS_MINUS( aFormat )                                         \
    ( ((aFormat)->mFlag & DTF_NUMBER_FMT_FLAG_MINUS ) > 0 ? STL_TRUE : STL_FALSE )
#define DTF_NUMBER_FMT_FLAG_IS_BRACKET( aFormat )                                       \
    ( ((aFormat)->mFlag & DTF_NUMBER_FMT_FLAG_BRACKET ) > 0 ? STL_TRUE : STL_FALSE )
#define DTF_NUMBER_FMT_FLAG_IS_DOLLAR( aFormat )                                        \
    ( ((aFormat)->mFlag & DTF_NUMBER_FMT_FLAG_DOLLAR ) > 0 ? STL_TRUE : STL_FALSE )
#define DTF_NUMBER_FMT_FLAG_IS_EEEE( aFormat )                                          \
    ( ((aFormat)->mFlag & DTF_NUMBER_FMT_FLAG_EEEE ) > 0 ? STL_TRUE : STL_FALSE )
#define DTF_NUMBER_FMT_FLAG_IS_V( aFormat )                                             \
    ( ((aFormat)->mFlag & DTF_NUMBER_FMT_FLAG_V ) > 0 ? STL_TRUE : STL_FALSE )
#define DTF_NUMBER_FMT_FLAG_IS_RN( aFormat )                                            \
    ( ((aFormat)->mFlag & DTF_NUMBER_FMT_FLAG_RN ) > 0 ? STL_TRUE : STL_FALSE )
#define DTF_NUMBER_FMT_FLAG_IS_X( aFormat )                                             \
    ( ((aFormat)->mFlag & DTF_NUMBER_FMT_FLAG_X ) > 0 ? STL_TRUE : STL_FALSE )
#define DTF_NUMBER_FMT_FLAG_IS_FM( aFormat )                                            \
    ( ((aFormat)->mFlag & DTF_NUMBER_FMT_FLAG_FM ) > 0 ? STL_TRUE : STL_FALSE )


#define DTF_NUMBER_FMT_PERIOD_STR       "decimal point"
#define DTF_NUMBER_FMT_SIGN_STR         "S"
#define DTF_NUMBER_FMT_MI_STR           "MI"
#define DTF_NUMBER_FMT_PR_STR           "PR"
#define DTF_NUMBER_FMT_DOLLAR_STR       "$"
#define DTF_NUMBER_FMT_V_STR            "V"


#define DTF_NUMBER_FMT_SIGN_STR_LEN( aFormat )                          \
    ( (DTF_NUMBER_FMT_FLAG_IS_BRACKET(aFormat) == STL_FALSE) ? 1 : 2 )

#define DTF_NUMBER_FMT_EEEE_STR_LEN  ( 5 )

#define DTF_NUMBER_FMT_LAST_SPACE_USED( aFormatInfo , sLastSpaceUsed )  \
{                                                                       \
    sLastSpaceUsed = STL_FALSE;                                         \
                                                                        \
    if( DTF_NUMBER_FMT_FLAG_IS_BRACKET( (aFormatInfo) ) == STL_FALSE )  \
    {                                                                   \
        /* POST 'S' , 'MI' */                                           \
        if( aFormatInfo->mSignLocate == DTF_NUMBER_SIGN_LOCATE_POST )   \
        {                                                               \
            sLastSpaceUsed = STL_TRUE;                                  \
        }                                                               \
        else                                                            \
        {                                                               \
            sLastSpaceUsed = STL_FALSE;                                 \
        }                                                               \
    }                                                                   \
    else                                                                \
    {                                                                   \
        /* PR 에 대한 부호 표기 */                                      \
        sLastSpaceUsed = STL_TRUE;                                      \
    }                                                                   \
}                                                                


#define DTF_SKIP_LEADING_SPACE( _aStrPtr, _aStrEndPtr )                 \
    {                                                                   \
        while( (_aStrPtr) < (_aStrEndPtr) )                             \
        {                                                               \
            if( *(_aStrPtr) == ' ' )                                    \
            {                                                           \
                (_aStrPtr)++;                                           \
            }                                                           \
            else                                                        \
            {                                                           \
                break;                                                  \
            }                                                           \
        }                                                               \
    }

#define DTF_SKIP_LEADING_ZERO_CHARACTERS( _aStrPtr, _aStrEndPtr, _aZeroCount )   \
    {                                                                            \
        (_aZeroCount) = 0;                                                       \
        while( (_aStrPtr) < (_aStrEndPtr) )                                      \
        {                                                                        \
            if( *(_aStrPtr) == '0' )                                             \
            {                                                                    \
                (_aZeroCount)++;                                                 \
                (_aStrPtr)++;                                                    \
            }                                                                    \
            else                                                                 \
            {                                                                    \
                break;                                                           \
            }                                                                    \
        }                                                                        \
    }

#define DTF_SKIP_TRAILING_ZERO_EXCEPT_AFTER_DECIMAL_ONE_ZERO_CHARACTERS( _aStrPtr, _aStrEndPtr, _aZeroCount ) \
    {                                                                   \
        stlChar * sStrLast = (_aStrEndPtr) - 1;                         \
        (_aZeroCount) = 0;                                              \
        while( (_aStrPtr) <= sStrLast )                                 \
        {                                                               \
            if( *sStrLast == '0' )                                      \
            {                                                           \
                (_aZeroCount)++;                                        \
                sStrLast--;                                             \
            }                                                           \
            else                                                        \
            {                                                           \
                if( (*sStrLast == '.') && ((_aZeroCount) > 0) )         \
                {                                                       \
                    sStrLast++;                                         \
                    (_aZeroCount)--;                                    \
                }                                                       \
                break;                                                  \
            }                                                           \
        }                                                               \
        (_aStrEndPtr) = sStrLast + 1;                                   \
    }


/***************************************
 * NUMBER FormatInfo struct
 ***************************************/

#define DTF_NLS_NUMBER_FORMAT_STRING_MAX_SIZE ( 64 )
#define DTF_NLS_NUMBER_FORMAT_NULL_TERMINATED_STRING_MAX_SIZE ( DTF_NLS_NUMBER_FORMAT_STRING_MAX_SIZE + 1 )

#define DTF_NUMBER_TO_CHAR_RESULT_STRING_MAX_SIZE ( DTF_NLS_NUMBER_FORMAT_STRING_MAX_SIZE + 2 )
#define DTF_NUMBER_TO_CHAR_RESULT_ROMAN_NUMERAL_STRING_SIZE ( 15 )

typedef struct dtfNumberFormatInfo  dtfNumberFormatInfo;

/**
 * @brief 실행될 format function
 */
typedef stlStatus (*dtfNumberFormatFuncPtr) ( dtlDataValue         * aDataValue,
                                              dtfNumberFormatInfo  * aFormatInfo,
                                              stlInt64               aAvailableSize,
                                              stlChar              * aResult,
                                              stlInt64             * aLength,
                                              dtlFuncVector        * aVectorFunc,
                                              void                 * aVectorArg,
                                              stlErrorStack        * aErrorStack );



/**
 * @brief number format info 구조체
 */
struct dtfNumberFormatInfo
{
    stlChar                  mStr[ DTF_NLS_NUMBER_FORMAT_STRING_MAX_SIZE ];  /**< format string */
    stlUInt8                 mStrLen;                    /**< format string length */
    stlUInt8                 mDigitCntBeforeDecimal;     /**< 소수점 이전에 나타나는 숫자의 갯수 */
    stlUInt8                 mDigitCntAfterDecimal;      /**< 소수점 이후에 나타나는 숫자의 갯수 */
    stlUInt8                 mDigitCntAfterV;            /**< V format element 이후에 나타나는 숫자의 갯수 */
    stlUInt8                 mCommaCnt;                  /**< format string에 나타나는 comma 의 갯수 */
    stlUInt8                 mXCnt;                      /**< format string에 나타나는 X element의 갯수 */    
    stlInt8                  mSignLocate;                /**< 부호가 표기될 위치 정보 */
    stlInt32                 mFlag;                      /**< format element들의 정보 */
    
    
    stlChar                * mFirstZeroOrNine;           /**< format string에서 '0' 또는 '9' 가 처음으로 나타나는 위치정보 */
    stlUInt8                 mZeroStart;                 /**< mDigitCntBeforeDecimal에서 '0' 가 처음으로 나타나는 위치정보 */
    stlUInt8                 mZeroEndAfterDecimal;       /**< mDigitCntAfterDecimal에서 '0' 이 마지막으로 나타나는 위치정보 */    
    dtfNumberFormatFuncPtr   mFormatFunc;                /**< execute시 실행될 함수 정보 */    
    
    /**
     * TO_CHAR 함수 수행을 위해 필요한 멤버변수들
     */
    
    stlBool                  mIsOddDigitCntAfterDecimal; /**< mDigitCntAfterDecimal 의 홀수/짝수 여부 */
    stlBool                  mIsOddDigitCntAfterV;       /**< mDigitCntAfterV 의 홀수/짝수 여부 */
    stlUInt8                 mToCharResultLen;           /**< to_char로 변환했을때의 결과 길이 */
    stlUInt8                 mLowerUpperIdx;             /**< 로만 문자열 리스트 또는
                                                          * 16진수 문자열 리스트에 대한 대소문자 구분
                                                          * 0 : 소문자 ,  1 : 대문자 */
};



/**
 * @brief number를 TO_CHAR로 변환했을때의 결과 길이를 구한다. <BR>
 *        TO_CHAR( number, fmt ) => 이때 fmt는 EEEE, RN, X format element가 포함되지 않는 경우이다.
 */
#define DTF_SET_NUMBER_TO_CHAR_RESULT_LEN( _aFormatInfo )                              \
    {                                                                                  \
        STL_DASSERT( ( DTF_NUMBER_FMT_FLAG_IS_EEEE( (_aFormatInfo) ) == STL_FALSE ) && \
                     ( DTF_NUMBER_FMT_FLAG_IS_RN( (_aFormatInfo) ) == STL_FALSE ) &&   \
                     ( DTF_NUMBER_FMT_FLAG_IS_X( (_aFormatInfo) ) == STL_FALSE ) );    \
                                                                                       \
        (_aFormatInfo)->mToCharResultLen =                                             \
            DTF_NUMBER_FMT_SIGN_STR_LEN( (_aFormatInfo) ) +                            \
            (_aFormatInfo)->mDigitCntBeforeDecimal +                                   \
            (_aFormatInfo)->mDigitCntAfterDecimal +                                    \
            (_aFormatInfo)->mCommaCnt +                                                \
            (DTF_NUMBER_FMT_FLAG_IS_PERIOD( (_aFormatInfo) ) == STL_TRUE ? 1 : 0) +    \
            (DTF_NUMBER_FMT_FLAG_IS_DOLLAR( (_aFormatInfo) ) == STL_TRUE ? 1 : 0);    \
    }

/**
 * @brief number를 EEEE format element를 포함한 경우 TO_CHAR로 변환했을때의 결과 길이를 구한다. <BR>
 *        TO_CHAR( number, fmt ) => 이때 fmt는 EEEE element가 포함된 경우이다. <BR>
 *                                 ( RN, X format element는 포함되지 않는 경우이다. )
 */
#define DTF_SET_NUMBER_EEEE_TO_CHAR_RESULT_LEN( _aFormatInfo )                         \
    {                                                                                  \
        STL_DASSERT( ( DTF_NUMBER_FMT_FLAG_IS_RN( (_aFormatInfo) ) == STL_FALSE ) &&   \
                     ( DTF_NUMBER_FMT_FLAG_IS_X( (_aFormatInfo) ) == STL_FALSE )  &&   \
                     ( (_aFormatInfo)->mDigitCntBeforeDecimal > 0 ) );                 \
                                                                                       \
        (_aFormatInfo)->mToCharResultLen =                                             \
            DTF_NUMBER_FMT_SIGN_STR_LEN( (_aFormatInfo) ) +                            \
            1 +  /* digit count before decimal */                                      \
            (_aFormatInfo)->mDigitCntAfterDecimal +                                    \
            (_aFormatInfo)->mDigitCntAfterV +                                          \
            (DTF_NUMBER_FMT_FLAG_IS_PERIOD( (_aFormatInfo) ) == STL_TRUE ? 1 : 0) +    \
            (DTF_NUMBER_FMT_FLAG_IS_DOLLAR( (_aFormatInfo) ) == STL_TRUE ? 1 : 0) +    \
            DTF_NUMBER_FMT_EEEE_STR_LEN;                                               \
    }


#define DTF_INIT_NUMBER_FORMAT_INFO( aFormatInfo )              \
    {                                                           \
        (aFormatInfo)->mStr[0] = '\0';                          \
        (aFormatInfo)->mStrLen = 0;                             \
        (aFormatInfo)->mDigitCntBeforeDecimal = 0;              \
        (aFormatInfo)->mDigitCntAfterDecimal  = 0;              \
        (aFormatInfo)->mDigitCntAfterV        = 0;              \
        (aFormatInfo)->mCommaCnt              = 0;              \
        (aFormatInfo)->mXCnt                  = 0;              \
        (aFormatInfo)->mSignLocate            = 0;              \
        (aFormatInfo)->mFlag                  = 0;              \
        (aFormatInfo)->mFirstZeroOrNine       = NULL;           \
        (aFormatInfo)->mZeroStart             = 0;              \
        (aFormatInfo)->mZeroEndAfterDecimal   = 0;              \
        (aFormatInfo)->mFormatFunc            = NULL;           \
        (aFormatInfo)->mIsOddDigitCntAfterDecimal = STL_FALSE;  \
        (aFormatInfo)->mIsOddDigitCntAfterV       = STL_FALSE;  \
        (aFormatInfo)->mToCharResultLen       = 0;              \
        (aFormatInfo)->mLowerUpperIdx         = 1;              \
    }


#define DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( aFmtStr )        \
    {                                                                      \
        while( ( *(aFmtStr) == 'B' ) ||                                    \
               ( *(aFmtStr) == 'b' ) ||                                    \
               ( *(aFmtStr) == '$' ) )                                     \
        {                                                                  \
            (aFmtStr)++;                                                   \
        }                                                                  \
    }

#define DTF_NUMBER_FMT_SKIP_FM_SIGN_NEXT_FORMAT_STR( aFmtStr )             \
    {                                                                      \
        if( ( *(aFmtStr) == 'S' ) || ( *(aFmtStr) == 's' ) )               \
        {                                                                  \
            (aFmtStr)++;                                                   \
        }                                                                  \
        else if( ( ( *(aFmtStr) == 'F' ) || ( *(aFmtStr) == 'f' ) ) &&     \
                 ( ( *(aFmtStr+1) == 'M' ) || ( *(aFmtStr+1) == 'm' ) ) )  \
        {                                                                  \
            (aFmtStr) += 2;                                                \
        }                                                                  \
        else                                                               \
        {                                                                  \
            /* Do Nothing */                                               \
        }                                                                  \
                                                                           \
        if( ( *(aFmtStr) == 'S' ) || ( *(aFmtStr) == 's' ) )               \
        {                                                                  \
            (aFmtStr)++;                                                   \
        }                                                                  \
        else if( ( ( *(aFmtStr) == 'F' ) || ( *(aFmtStr) == 'f' ) ) &&     \
                 ( ( *(aFmtStr+1) == 'M' ) || ( *(aFmtStr+1) == 'm' ) ) )  \
        {                                                                  \
            (aFmtStr) += 2;                                                \
        }                                                                  \
        else                                                               \
        {                                                                  \
            /* Do Nothing */                                               \
        }                                                                  \
    }

#define DTF_NUMBER_FMT_SET_COMMA( aResult, aFmtStr )                       \
    {                                                                      \
        while( *(aFmtStr) == ',' )                                         \
        {                                                                  \
            *(aResult) = ',';                                              \
            (aResult)++;                                                   \
            (aFmtStr)++;                                                   \
            DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( (aFmtStr) ); \
        }                                                                  \
    }

#define DTF_NUMBER_FMT_REWIND_V_FORMAT_STR( aFmtStr )  \
    {                                                                      \
        while( ( *(aFmtStr) == 'V' ) ||                                    \
               ( *(aFmtStr) == 'v' ) )                                     \
        {                                                                  \
            (aFmtStr)--;                                                   \
        }                                                                  \
    }

#define DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( aFmtStr )  \
    {                                                                      \
        while( ( *(aFmtStr) == 'B' ) ||                                    \
               ( *(aFmtStr) == 'b' ) ||                                    \
               ( *(aFmtStr) == '$' ) )                                     \
        {                                                                  \
            (aFmtStr)--;                                                   \
        }                                                                  \
    }

#define DTF_NUMBER_FMT_SET_COMMA_AND_REWIND( aResult, aFmtStr )                  \
    {                                                                            \
        while( *(aFmtStr) == ',' )                                               \
        {                                                                        \
            *(aResult) = ',';                                                    \
            (aResult)--;                                                         \
            (aFmtStr)--;                                                         \
            DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( (aFmtStr) ); \
        }                                                                        \
    }



/**
 * @brief format string을 변환하는 과정 중 $를 표기한다.
 * 
 */
#define DTF_NUMBER_FMT_SET_DOLLAR( aFormatInfo, aCurResultPtr )            \
    {                                                                      \
        if( DTF_NUMBER_FMT_FLAG_IS_DOLLAR( (aFormatInfo) ) == STL_TRUE )   \
        {                                                                  \
            *(aCurResultPtr) = '$';                                        \
            (aCurResultPtr)++;                                             \
        }                                                                  \
    }


/**
 * @brief charactor를 length만큼 assign하고, assign이 수행되면 _aStrPtr의 pointer가 이동된다.
 */
#define DTF_ASSIGN_CHAR( _aStrPtr, _aChar, _aLength )   \
    {                                                   \
        while( (_aLength) > 0 )                         \
        {                                               \
            *(_aStrPtr) = (_aChar);                     \
                                                        \
            (_aLength)--;                               \
            (_aStrPtr)++;                               \
        }                                               \
    }



/******************************************************************************
 * FORMATTING 공통함수
 ******************************************************************************/

const dtlDateTimeFormatKeyWord * dtfGetDateTimeFormatKeyWord( stlChar  * aFormatString );

stlStatus dtfDateTimeToChar( dtlDateTimeFormatInfo * aFormatInfo,
                             dtlExpTime            * aDtlExpTime,
                             dtlFractionalSecond     aFractionalSecond,
                             stlInt32              * aTimeZone,
                             stlInt64                aAvailableSize,
                             stlChar               * aResult,
                             stlInt64              * aLength,
                             dtlFuncVector         * aVectorFunc,
                             void                  * aVectorArg,
                             stlErrorStack         * aErrorStack );

stlStatus dtfToDateTimeFromString( dtlDataType                aDateTimeType,
                                   stlChar                  * aDateTimeString,
                                   stlInt64                   aDateTimeStringLength,
                                   dtlDateTimeFormatInfo    * aToDateTimeFormatInfo,
                                   dtlExpTime               * aDtlExpTime,
                                   dtlFractionalSecond      * aFractionalSecond,
                                   stlInt32                 * aTimeZone,
                                   dtlFuncVector            * aVectorFunc,
                                   void                     * aVectorArg,
                                   stlErrorStack            * aErrorStack );

stlStatus dtfToDateTimeGetIntValueFromParseString( dtlDateTimeFormatStrInfo  * aFormatStrInfo,
                                                   stlChar                   * aDateTimeStr,
                                                   stlChar                   * aDateTimeStrEnd,
                                                   dtlDateTimeFormatNode     * aFormatNode,
                                                   stlInt32                  * aIntValue,
                                                   stlInt32                  * aIntValueDigitCnt,
                                                   stlInt32                  * aLeadingZeroDigitCnt,
                                                   stlInt32                  * aElementStrLen,
                                                   dtlFuncVector             * aVectorFunc,
                                                   void                      * aVectorArg,
                                                   stlErrorStack             * aErrorStack );

const dtfNumberFormatKeyWord * dtfGetNumberFormatKeyWord( stlChar  * aFormatString );

stlStatus dtfGetNumberFormatInfoForToChar( dtlDataType      aDataType,
                                           stlChar        * aFormatString,
                                           stlInt64         aFormatStringLen,
                                           void           * aToCharFormatInfoBuffer,
                                           stlInt32         aToCharFormatInfoBufferSize,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           stlErrorStack  * aErrorStack );

stlStatus dtfSetNumberFormatInfo( stlChar                       * aFormatStr,
                                  const dtfNumberFormatKeyWord  * aKeyWord,
                                  dtfNumberFormatInfo           * aNumberFormatInfo,
                                  dtlFuncVector                 * aVectorFunc,
                                  void                          * aVectorArg,
                                  stlErrorStack                 * aErrorStack );

stlStatus dtfToCharNumberToFormatString( dtlDataValue         * aNumericDataValue,
                                         dtfNumberFormatInfo  * aFormatInfo,
                                         stlInt64               aAvailableSize,
                                         stlChar              * aResult,
                                         stlInt64             * aLength,
                                         dtlFuncVector        * aVectorFunc,
                                         void                 * aVectorArg,
                                         stlErrorStack        * aErrorStack );

stlStatus dtfToCharNumberToVFormatString( dtlDataValue         * aNumericDataValue,
                                          dtfNumberFormatInfo  * aFormatInfo,
                                          stlInt64               aAvailableSize,
                                          stlChar              * aResult,
                                          stlInt64             * aLength,
                                          dtlFuncVector        * aVectorFunc,
                                          void                 * aVectorArg,
                                          stlErrorStack        * aErrorStack );

stlStatus dtfToCharNumberToXFormatString( dtlDataValue         * aNumericDataValue,
                                          dtfNumberFormatInfo  * aFormatInfo,
                                          stlInt64               aAvailableSize,
                                          stlChar              * aResult,
                                          stlInt64             * aLength,
                                          dtlFuncVector        * aVectorFunc,
                                          void                 * aVectorArg,
                                          stlErrorStack        * aErrorStack );

stlStatus dtfToCharNumberToEEEEFormatString( dtlDataValue         * aNumericDataValue,
                                             dtfNumberFormatInfo  * aFormatInfo,
                                             stlInt64               aAvailableSize,
                                             stlChar              * aResult,
                                             stlInt64             * aLength,
                                             dtlFuncVector        * aVectorFunc,
                                             void                 * aVectorArg,
                                             stlErrorStack        * aErrorStack );

stlStatus dtfToCharNumberToRNFormatString( dtlDataValue         * aNumericDataValue,
                                           dtfNumberFormatInfo  * aFormatInfo,
                                           stlInt64               aAvailableSize,
                                           stlChar              * aResult,
                                           stlInt64             * aLength,
                                           dtlFuncVector        * aVectorFunc,
                                           void                 * aVectorArg,
                                           stlErrorStack        * aErrorStack );

/* @todo 제거예정. */
void dtfNumberFmtSetSign( dtfNumberFormatInfo   * aFormatInfo,
                          stlChar               * aResult,
                          stlInt32                aResultLen,
                          stlInt32                aIsPositive,
                          stlInt32              * aSetSignResultLen,                          
                          stlChar               * aCurResultPtr,
                          stlChar              ** aCurResultPtrAdd );

void dtfNumberFmtSetSignForNormal( dtfNumberFormatInfo   * aFormatInfo,
                                   stlChar               * aResult,
                                   stlInt32                aResultLen,
                                   stlInt32                aIsPositive,
                                   stlChar               * aCurResultPtr,
                                   stlChar              ** aCurResultPtrAdd );

void dtfNumberFmtSetSignForFM( dtfNumberFormatInfo   * aFormatInfo,
                               stlChar               * aResult,
                               stlInt32                aResultLen,
                               stlInt32                aIsPositive,
                               stlBool                 aIsSignLocatePre,
                               stlInt32              * aSetSignResultLen,                          
                               stlChar               * aCurResultPtr,
                               stlChar              ** aCurResultPtrAdd );

void dtfNumberFmtSetEEEEExpForTempNumber( dtfNumberFormatInfo   * aFormatInfo,
                                          stlChar               * aResult,
                                          stlInt32                aResultLen,
                                          stlInt32                aIsPositive,
                                          stlInt32                aExp,
                                          stlUInt8              * aSetEEEEExpLen );

void dtfNumberFmtSetEEEEExp( dtfNumberFormatInfo   * aFormatInfo,
                             stlInt32                aExp,
                             stlChar               * aCurResultPtr,
                             stlUInt8              * aSetEEEEExpLen );

stlStatus dtfToCharDoubleToFormatString( dtlDataValue         * aDataValue,
                                         dtfNumberFormatInfo  * aFormatInfo,
                                         stlInt64               aAvailableSize,
                                         stlChar              * aResult,
                                         stlInt64             * aLength,
                                         dtlFuncVector        * aVectorFunc,
                                         void                 * aVectorArg,
                                         stlErrorStack        * aErrorStack );

stlStatus dtfToCharDoubleToVFormatString( dtlDataValue         * aDataValue,
                                          dtfNumberFormatInfo  * aFormatInfo,
                                          stlInt64               aAvailableSize,
                                          stlChar              * aResult,
                                          stlInt64             * aLength,
                                          dtlFuncVector        * aVectorFunc,
                                          void                 * aVectorArg,
                                          stlErrorStack        * aErrorStack );

stlStatus dtfToCharDoubleToXFormatString( dtlDataValue         * aDataValue,
                                          dtfNumberFormatInfo  * aFormatInfo,
                                          stlInt64               aAvailableSize,
                                          stlChar              * aResult,
                                          stlInt64             * aLength,
                                          dtlFuncVector        * aVectorFunc,
                                          void                 * aVectorArg,
                                          stlErrorStack        * aErrorStack );

stlStatus dtfToCharDoubleToEEEEFormatString( dtlDataValue         * aDataValue,
                                             dtfNumberFormatInfo  * aFormatInfo,
                                             stlInt64               aAvailableSize,
                                             stlChar              * aResult,
                                             stlInt64             * aLength,
                                             dtlFuncVector        * aVectorFunc,
                                             void                 * aVectorArg,
                                             stlErrorStack        * aErrorStack );

stlStatus dtfToCharDoubleToRNFormatString( dtlDataValue         * aDataValue,
                                            dtfNumberFormatInfo  * aFormatInfo,
                                            stlInt64               aAvailableSize,
                                            stlChar              * aResult,
                                            stlInt64             * aLength,
                                            dtlFuncVector        * aVectorFunc,
                                            void                 * aVectorArg,
                                            stlErrorStack        * aErrorStack );

stlStatus dtfToCharRealToFormatString( dtlDataValue         * aDataValue,
                                       dtfNumberFormatInfo  * aFormatInfo,
                                       stlInt64               aAvailableSize,
                                       stlChar              * aResult,
                                       stlInt64             * aLength,
                                       dtlFuncVector        * aVectorFunc,
                                       void                 * aVectorArg,
                                       stlErrorStack        * aErrorStack );

stlStatus dtfToCharRealToVFormatString( dtlDataValue         * aDataValue,
                                        dtfNumberFormatInfo  * aFormatInfo,
                                        stlInt64               aAvailableSize,
                                        stlChar              * aResult,
                                        stlInt64             * aLength,
                                        dtlFuncVector        * aVectorFunc,
                                        void                 * aVectorArg,
                                        stlErrorStack        * aErrorStack );

stlStatus dtfToCharRealToXFormatString( dtlDataValue         * aDataValue,
                                        dtfNumberFormatInfo  * aFormatInfo,
                                        stlInt64               aAvailableSize,
                                        stlChar              * aResult,
                                        stlInt64             * aLength,
                                        dtlFuncVector        * aVectorFunc,
                                        void                 * aVectorArg,
                                        stlErrorStack        * aErrorStack );


stlStatus dtfToCharRealToEEEEFormatString( dtlDataValue         * aDataValue,
                                           dtfNumberFormatInfo  * aFormatInfo,
                                           stlInt64               aAvailableSize,
                                           stlChar              * aResult,
                                           stlInt64             * aLength,
                                           dtlFuncVector        * aVectorFunc,
                                           void                 * aVectorArg,
                                           stlErrorStack        * aErrorStack );

stlStatus dtfToCharRealToRNFormatString( dtlDataValue         * aDataValue,
                                         dtfNumberFormatInfo  * aFormatInfo,
                                         stlInt64               aAvailableSize,
                                         stlChar              * aResult,
                                         stlInt64             * aLength,
                                         dtlFuncVector        * aVectorFunc,
                                         void                 * aVectorArg,
                                         stlErrorStack        * aErrorStack );

stlStatus dtfGetNumberFormatInfoForToNumber( dtlDataType      aDataType,
                                             stlChar        * aFormatString,
                                             stlInt64         aFormatStringLen,
                                             void           * aFormatInfoBuffer,
                                             stlInt32         aFormatInfoBufferSize,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             stlErrorStack  * aErrorStack );

stlStatus dtfGetNumberLiteralFromString( dtlDataValue          * aDataValue,
                                         dtfNumberFormatInfo   * aFormatInfo,
                                         stlChar               * aNumberLiteralBuf,
                                         stlInt64              * aNumberLiteralBufLen,
                                         dtlFuncVector         * aVectorFunc,
                                         void                  * aVectorArg,
                                         stlErrorStack         * aErrorStack );

stlStatus dtfToDateInfoFromTypedLiteral( dtlDataType                aDtlDataType,
                                         stlChar                  * aDateTimeString,
                                         stlInt64                   aDateTimeStringLength,
                                         dtlExpTime               * aDtlExpTime,
                                         stlChar                 ** aRemainDateTimeStr,
                                         stlInt64                 * aRemainDateTimeStrLen,
                                         dtlFuncVector            * aVectorFunc,
                                         void                     * aVectorArg,
                                         stlErrorStack            * aErrorStack );

stlStatus dtfToTimeInfoFromTypedLiteral( dtlDataType                aDtlDataType,
                                         stlChar                  * aDateTimeString,
                                         stlInt64                   aDateTimeStringLength,
                                         dtlExpTime               * aDtlExpTime,
                                         dtlFractionalSecond      * aFractionalSecond,
                                         stlChar                 ** aRemainDateTimeStr,
                                         stlInt64                 * aRemainDateTimeStrLen,
                                         dtlFuncVector            * aVectorFunc,
                                         void                     * aVectorArg,
                                         stlErrorStack            * aErrorStack );

stlStatus dtfToTimeZoneInfoFromTypedLiteral( dtlDataType                aDtlDataType,
                                             stlChar                  * aDateTimeString,
                                             stlInt64                   aDateTimeStringLength,
                                             stlInt32                 * aTimeZone,
                                             dtlFuncVector            * aVectorFunc,
                                             void                     * aVectorArg,
                                             stlErrorStack            * aErrorStack );

stlStatus dtfToDateTimeGetIntValueFromDateTimeTypedLiteral(
    stlChar                  * aDateTimeString,
    stlChar                  * aDateTimeStringEnd,
    const dtlDateTimeFormatKeyWord * aKeyWord,
    stlInt32                 * aIntValue,
    stlInt32                 * aIntValueDigitCnt,
    stlInt32                 * aLeadingZeroDigitCnt,
    stlInt32                 * aElementStrLen,
    dtlFuncVector            * aVectorFunc,
    void                     * aVectorArg,
    stlErrorStack            * aErrorStack );

stlBool dtfIsExistTimeZoneStr( stlChar     * aDateTimeString,
                               stlInt64      aDateTimeStringLength,
                               stlBool       aIsTimestampStr );


/******************************************************************************
 * internal function ( TO_CHAR )
 ******************************************************************************/

extern dtlBuiltInFuncPtr  gDtfToCharFunc[ DTL_TYPE_MAX ];

stlStatus dtfDateToChar( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv );


stlStatus dtfTimeToChar( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv );

stlStatus dtfTimeTzToChar( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aInfo,
                           dtlFuncVector  * aVectorFunc,
                           void           * aVectorArg,
                           void           * aEnv );

stlStatus dtfTimestampToChar( stlUInt16        aInputArgumentCnt,
                              dtlValueEntry  * aInputArgument,
                              void           * aResultValue,
                              void           * aInfo,
                              dtlFuncVector  * aVectorFunc,
                              void           * aVectorArg,
                              void           * aEnv );

stlStatus dtfTimestampTzToChar( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                void           * aInfo,
                                dtlFuncVector  * aVectorFunc,
                                void           * aVectorArg,
                                void           * aEnv );

stlStatus dtfIntervalYearToMonthToChar( stlUInt16        aInputArgumentCnt,
                                        dtlValueEntry  * aInputArgument,
                                        void           * aResultValue,
                                        void           * aInfo,
                                        dtlFuncVector  * aVectorFunc,
                                        void           * aVectorArg,
                                        void           * aEnv );

stlStatus dtfIntervalDayToSecondToChar( stlUInt16        aInputArgumentCnt,
                                        dtlValueEntry  * aInputArgument,
                                        void           * aResultValue,
                                        void           * aInfo,
                                        dtlFuncVector  * aVectorFunc,
                                        void           * aVectorArg,
                                        void           * aEnv );

stlStatus dtfNumberToChar( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aInfo,
                           dtlFuncVector  * aVectorFunc,
                           void           * aVectorArg,
                           void           * aEnv );

stlStatus dtfDoubleToChar( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aInfo,
                           dtlFuncVector  * aVectorFunc,
                           void           * aVectorArg,
                           void           * aEnv );

stlStatus dtfRealToChar( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv );

stlStatus dtfCharStringToChar( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aInfo,
                               dtlFuncVector  * aVectorFunc,
                               void           * aVectorArg,
                               void           * aEnv );

stlStatus dtfLongvarcharStringToChar( stlUInt16        aInputArgumentCnt,
                                      dtlValueEntry  * aInputArgument,
                                      void           * aResultValue,
                                      void           * aInfo,
                                      dtlFuncVector  * aVectorFunc,
                                      void           * aVectorArg,
                                      void           * aEnv );

stlStatus dtfToDateTimeFromWCharString( dtlUnicodeEncoding         aUniEncoding,
                                        dtlDataType                aDateTimeType,
                                        void                     * aDateTimeString,
                                        stlInt64                   aDateTimeStringLength,
                                        dtlDateTimeFormatInfo    * aToDateTimeFormatInfo,
                                        dtlExpTime               * aDtlExpTime,
                                        dtlFractionalSecond      * aFractionalSecond,
                                        stlInt32                 * aTimeZone,
                                        dtlFuncVector            * aVectorFunc,
                                        void                     * aVectorArg,
                                        stlErrorStack            * aErrorStack );

stlStatus dtfDateTimeToWCharString( dtlUnicodeEncoding      aUniEncoding,
                                    dtlDateTimeFormatInfo * aFormatInfo,
                                    dtlExpTime            * aDtlExpTime,
                                    dtlFractionalSecond     aFractionalSecond,
                                    stlInt32              * aTimeZone,
                                    stlInt64                aAvailableSize,
                                    void                  * aResult,
                                    stlInt64              * aLength,
                                    dtlFuncVector         * aVectorFunc,
                                    void                  * aVectorArg,
                                    stlErrorStack         * aErrorStack );


/******************************************************************************
 * internal function ( TO_DATE )
 ******************************************************************************/

stlStatus dtfToDate( stlUInt16        aInputArgumentCnt,
                     dtlValueEntry  * aInputArgument,
                     void           * aResultValue,
                     void           * aInfo,
                     dtlFuncVector  * aVectorFunc,
                     void           * aVectorArg,
                     void           * aEnv );

/******************************************************************************
 * internal function ( TO_TIME )
 ******************************************************************************/

stlStatus dtfToTime( stlUInt16        aInputArgumentCnt,
                     dtlValueEntry  * aInputArgument,
                     void           * aResultValue,
                     void           * aInfo,
                     dtlFuncVector  * aVectorFunc,
                     void           * aVectorArg,
                     void           * aEnv );

/******************************************************************************
 * internal function ( TO_TIME_WITH_TIME_ZONE )
 ******************************************************************************/

stlStatus dtfToTimeWithTimeZone( stlUInt16        aInputArgumentCnt,
                                 dtlValueEntry  * aInputArgument,
                                 void           * aResultValue,
                                 void           * aInfo,
                                 dtlFuncVector  * aVectorFunc,
                                 void           * aVectorArg,
                                 void           * aEnv );

/******************************************************************************
 * internal function ( TO_TIMESTAMP )
 ******************************************************************************/

stlStatus dtfToTimestamp( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv );

/******************************************************************************
 * internal function ( TO_TIMESTAMP_WITH_TIME_ZONE )
 ******************************************************************************/

stlStatus dtfToTimestampWithTimeZone( stlUInt16        aInputArgumentCnt,
                                      dtlValueEntry  * aInputArgument,
                                      void           * aResultValue,
                                      void           * aInfo,
                                      dtlFuncVector  * aVectorFunc,
                                      void           * aVectorArg,
                                      void           * aEnv );

/******************************************************************************
 * internal function ( TO_NUMBER )
 ******************************************************************************/

stlStatus dtfToNumber( stlUInt16        aInputArgumentCnt,
                       dtlValueEntry  * aInputArgument,
                       void           * aResultValue,
                       void           * aInfo,
                       dtlFuncVector  * aVectorFunc,
                       void           * aVectorArg,
                       void           * aEnv );

/******************************************************************************
 * internal function ( TO_NATIVE_REAL )
 ******************************************************************************/

stlStatus dtfToNativeReal( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aInfo,
                           dtlFuncVector  * aVectorFunc,
                           void           * aVectorArg,
                           void           * aEnv );

/******************************************************************************
 * internal function ( TO_NATIVE_DOUBLE )
 ******************************************************************************/

stlStatus dtfToNativeDouble( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aInfo,
                             dtlFuncVector  * aVectorFunc,
                             void           * aVectorArg,
                             void           * aEnv );


/** @} dtf */

/** @} */

#endif /* _DTF_FORMATTING_H_ */

