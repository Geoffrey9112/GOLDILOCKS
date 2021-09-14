/*******************************************************************************
 * dtfRound.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtfRound.c 1296 2011-06-30 07:24:30Z ehpark $
 *
 * NOTES
 *   1. BigInt, Double, Numeric공통.
 *      함수의 피연산자에 NULL값이 오지 않는다고 가정하고 구현되어 있다.
 *   2. Double형 연산 관련.
 *      현재 NaN은 고려되어 있지 않다.
 *      infinite 값이 들어오면 argument out of range 로 처리한다.
 *   3. Numeric형 연산 관련.
 *      numeric은 precision, scale과 연관되어 검증되어야 함으로 상위단에서
 *      세밀하게 검증이 필요함.
 *
 ******************************************************************************/

/**
 * @file dtfRound.c
 * @brief dtfRound Function DataType Library Layer
 */

#include <dtl.h>
#include <dtlDef.h>
#include <dtDef.h>
#include <dtfArithmetic.h>
#include <dtfNumeric.h>
#include <dtfMathematical.h>



/**
 * @addtogroup dtfRound Round
 * @ingroup dtf
 * @internal
 * @{
 */

/* DTF_ROUND_HOUR_VALUE(12) * DTL_USECS_PER_HOUR(3600000000) */
#define DTF_ROUND_HOUR_USECS_VALUE   ( STL_INT64_C(43200000000) )

#define DTF_ROUND_YYYY_VALUE         ( 51 )
#define DTF_ROUND_MONTH_VALUE        ( 7 )
#define DTF_ROUND_DAY_VALUE          ( 16 )
#define DTF_ROUND_HOUR_VALUE         ( 12 )
#define DTF_ROUND_MINUTE_VALUE       ( 30 )
#define DTF_ROUND_SECOND_VALUE       ( 30 )
#define DTF_ROUND_WW_VALUE           ( 4 )
#define DTF_ROUND_W_VALUE            ( 4 )
#define DTF_ROUND_D_VALUE            ( 4 )



/*******************************************************************************
 * round(x) , round(x, y)
 *   round(x)    => [ P, M, O ]
 *   round(x, y) => [ P, M, O ]
 * ex) round(42.4)        =>  42
 * ex) round(42.4382, 2)  =>  42.44
 ******************************************************************************/

/**
 * @brief round(x) , round(x, y) 함수
 *        <BR> round(x) , round(x, y)
 *        <BR>   round(x)    => [ P, M, O ]
 *        <BR>   round(x, y) => [ P, M, O ]
 *        <BR> ex) round(42.4)        =>  42
 *        <BR> ex) round(42.4382, 2)  =>  42.44
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> round(BIGINT), round(BIGINT, INTEGER)
 * @param[out] aResultValue      연산 결과 (결과타입 BIGINT)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfBigIntRound( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv )
{
    /**
     * aValue2가 NULL인 경우는 ( round(x) )
     * aValue2가 NULL이 아닌 경우는 ( round(x, y) )
     */
    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;
    dtlDataValue  * sResultValue;

    stlInt64        sTmpValue;
    stlInt64        sRoundDigit;

    DTL_PARAM_VALIDATE( ( aInputArgumentCnt == 1 ) || ( aInputArgumentCnt == 2 ),
                        (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;
    sValue1 = aInputArgument[0].mValue.mDataValue;
    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_BIGINT, (stlErrorStack *)aEnv );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_BIGINT, (stlErrorStack *)aEnv );

    if( aInputArgumentCnt == 1 )
    {
        DTF_BIGINT( sResultValue ) = DTF_BIGINT( sValue1 );
    }
    else
    {
        sValue2 = aInputArgument[1].mValue.mDataValue;
        DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_NATIVE_INTEGER, (stlErrorStack *)aEnv );

        sRoundDigit = (stlInt64)DTF_INTEGER( sValue2 );

        /*
         * round 연산을 한다.
         * postgres의 경우 round함수에 rint를 사용했음.
         * 변수가 두개인 경우는 postgres에 없음.
         */
        if( sRoundDigit >= 0 )
        {
            DTF_BIGINT( sResultValue ) = DTF_BIGINT( sValue1 );
        }
        else
        {
            if( -sRoundDigit >= DTL_INT64_MAX_DIGIT_COUNT )
            {
                DTF_BIGINT( sResultValue ) = 0;
            }
            else
            {
                if( DTF_BIGINT( sValue1 ) < 0 )
                {
                    sTmpValue = (stlInt64)gPreDefinedPow[ -sRoundDigit ];
                    DTF_BIGINT( sResultValue ) =
                        (stlInt64)( ( (DTF_BIGINT( sValue1 ) - (5 * sTmpValue / 10)) / sTmpValue ) *
                                    sTmpValue );
                }
                else
                {
                    sTmpValue = (stlInt64)gPreDefinedPow[ -sRoundDigit ];
                    DTF_BIGINT( sResultValue ) =
                        (stlInt64)( ( (DTF_BIGINT( sValue1 ) + (5 * sTmpValue / 10)) / sTmpValue ) *
                                    sTmpValue );
                }
            }
        }
    }

    sResultValue->mLength = DTL_NATIVE_BIGINT_SIZE;

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief round(x) , round(x, y) 함수
 *        <BR> round(x) , round(x, y)
 *        <BR>   round(x)    => [ P, M, O ]
 *        <BR>   round(x, y) => [ P, M, O ]
 *        <BR> ex) round(42.4)        =>  42
 *        <BR> ex) round(42.4382, 2)  =>  42.44
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> round(NUMERIC), round(NUMERIC, INTEGER)
 * @param[out] aResultValue      연산 결과 (결과타입 NUMERIC)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfNumericRound( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aEnv )
{
    /**
     * aValue2가 NULL인 경우는 ( round(x) )
     * aValue2가 NULL이 아닌 경우는 ( round(x, y) )
     */
    
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;
    stlInt32          sScale;

    DTL_PARAM_VALIDATE( ( aInputArgumentCnt == 1 ) || ( aInputArgumentCnt == 2 ) ,
                        (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;
    sValue1 = aInputArgument[0].mValue.mDataValue;
    DTF_CHECK_DTL_VALUE2( sValue1, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );

    if( aInputArgumentCnt == 2 )
    {
        sValue2 = aInputArgument[1].mValue.mDataValue;
        DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_NATIVE_INTEGER, sErrorStack );
        sScale = DTF_INTEGER( sValue2 );
    }
    else
    {
        sScale = 0;
    }

    /* round 연산을 한다. */
    STL_TRY( dtfNumericRounding( sValue1,
                                 sScale,
                                 sResultValue,
                                 sErrorStack )
             == STL_SUCCESS );

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief round(x) , round(x, y) 함수
 *        <BR> round(x) , round(x, y)
 *        <BR>   round(x)    => [ P, M, O ]
 *        <BR>   round(x, y) => [ P, M, O ]
 *        <BR> ex) round(42.4)        =>  42
 *        <BR> ex) round(42.4382, 2)  =>  42.44
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> round(DOUBLE), round(DOUBLE, INTEGER)
 * @param[out] aResultValue      연산 결과 (결과타입 DOUBLE)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfDoubleRound( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv )
{
    /**
     * aValue2가 NULL인 경우는 ( round(x) )
     * aValue2가 NULL이 아닌 경우는 ( round(x, y) )
     */
    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;

    DTL_PARAM_VALIDATE( ( aInputArgumentCnt == 1 ) || ( aInputArgumentCnt == 2 ),
                        (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;
    sValue1 = aInputArgument[0].mValue.mDataValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_DOUBLE, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_DOUBLE, sErrorStack );
    STL_DASSERT( stlIsinfinite( DTF_DOUBLE( sValue1 ) )
                 == STL_IS_INFINITE_FALSE );

    if( aInputArgumentCnt == 1 )
    {
        DTF_DOUBLE( sResultValue ) = stlRint( DTF_DOUBLE( sValue1 ) );
    }
    else
    {
        sValue2 = aInputArgument[1].mValue.mDataValue;
        DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_NATIVE_INTEGER, sErrorStack );

        /*
         * round 연산을 한다.
         * postgres의 경우 round함수에 rint를 사용했음.
         * 변수가 두개인 경우는 postgres에 없음.
         */
        STL_TRY( dtfDoubleRoundWithScale( DTF_DOUBLE( sValue1 ),
                                          DTF_INTEGER( sValue2 ),
                                          STL_FALSE,
                                          &DTF_DOUBLE( sResultValue),
                                          sErrorStack )
                 == STL_SUCCESS );
    }

    sResultValue->mLength = DTL_NATIVE_DOUBLE_SIZE;


    /*
     *  overflow또는 underflow가 나지 않는다.
     */

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/*
 * @TODO 확인 필요함.
 * round(x,y), trunc(x,y)함수를 위한 dtfDoubleRoundWithScale()시 overflow가 날수 있다.
 * 예) scale조정을 위해 double에서 * 100 을 하는 순간 overflow가 날수 있다.
 */

/**
 * @brief round, trunc를 위한 내부 함수.
 *        scale이 적용된 round, trunc를 연산한다.
 * @param[in]  aValue1          double형식의 피연산자.
 *                              <BR> tan(DOUBLE)
 * @param[in]  aScale           input Scale
 * @param[in]  aIsTrunc         round인 경우 STL_FALSE,
 *                              trunc인 경우 STL_TRUE를 사용한다.
 * @param[out] aResultValue     연산 결과 (결과타입 DOUBLE)
 * @param[out] aErrorStack      stlErrorStack
 */
stlStatus dtfDoubleRoundWithScale( dtlDoubleType    aValue1,
                                   dtlIntegerType   aScale,
                                   stlBool          aIsTrunc,
                                   stlFloat64     * aResultValue,
                                   stlErrorStack  * aErrorStack )
{
    /*
     * c에서 제공하는건 소수점 바로 아래에서 반올림하는 기능만 제공한다.
     * => 소숫점 자릿수를 옮겨서 계산하고 원복 하는 방법을 사용한다.
     * ex) round(42.4382, 2) 인 경우 round(4243.82) => 4244 가 나온다.
     *     42.44 로 원복한다.
     * ex) round(42.4382, -1) 인 경우 round(4.24382) => 4.0 가 나온다.
     *     40.0 로 원복한다.
     */

    stlFloat64  sTemp;              /* 자릿수 이동할 변수 */
    stlBool     sIsNegative;        /* aScale값의 음수 여부 */
    stlFloat64  sDoubleResult;
    stlUInt64   sAbsScale;          /* aScale의 절대값 */
    stlBool     sIsOutOfRange = STL_FALSE;

    if( aScale < 0 ){
        sIsNegative = STL_TRUE;
        sAbsScale = -aScale;
    }
    else
    {
        sIsNegative = STL_FALSE;
        sAbsScale = aScale;
    }

//    errno = 0;
    if( stlPow( 10.0, (stlFloat64)sAbsScale, &sTemp, aErrorStack ) == STL_SUCCESS )
    {
        /* Do Nothing */
    }
    else
    {
        (void)stlPopError( aErrorStack );
        sIsOutOfRange = STL_TRUE;
    }

    /*
     * sScale가 음수고 이동할 자릿수가 무한대면 결과는 0으로 한다.
     * 매우 큰자릿수(무한대)에서 반올림을 함으로 0이다.
     */
    if( ( sIsNegative == STL_TRUE ) &&
        ( ( stlIsinfinite( sTemp ) != STL_IS_INFINITE_FALSE ) ||
          ( sIsOutOfRange == STL_TRUE ) ) )
    {
        sDoubleResult= 0;
    }
    /*
     * sScale가 양수고 자릿수 이동한값이 무한대면 결과는 0으로 한다.
     * 매우 작은 자릿수(소수점 이하 무한대)에서 반올림을 함으로 결과는 sValue1과 같다.
     */
    else if( ( sIsNegative == STL_FALSE ) &&
             ( ( stlIsinfinite( aValue1 * sTemp ) != STL_IS_INFINITE_FALSE ) ||
               ( sIsOutOfRange == STL_TRUE ) ) )
    {
        sDoubleResult= aValue1;
    }
    else {
        /*
         * scale이 적용된 trunc 연산
         */
        if( aIsTrunc == STL_TRUE )
        {
            /*
             * sScale가 음수인 경우 / 하여 자릿수를 옮기고 반올림 하고 * 한다.
             * ex) trunc(42.4382, -1) 인 경우 trunc(4.24382) => 4.0 가 나온다.
             *     40.0 로 원복한다.
             */
            if( aScale < 0 )
            {
                sDoubleResult = stlTrunc( aValue1 / sTemp ) * sTemp;
            }
            /*
             * sScale가 양수인 경우 * 하여 자릿수를 옮기고 반올림 하고 / 한다.
             * ex) trunc(42.4382, 2) 인 경우 trunc(4243.82) => 4243 가 나온다.
             *     42.43 로 원복한다.
             */
            else
            {
                sDoubleResult = stlTrunc( aValue1 * sTemp ) / sTemp;
            }
        }
        /*
         * scale이 적용된 round 연산
         */
        else
        {
            /*
             * sScale가 음수인 경우 / 하여 자릿수를 옮기고 반올림 하고 * 한다.
             * ex) round(42.4382, -1) 인 경우 round(4.24382) => 4.0 가 나온다.
             *     40.0 로 원복한다.
             */
            if( aScale < 0 )
            {
                sDoubleResult = stlRint( aValue1 / sTemp ) * sTemp;
            }
            /*
             * sScale가 양수인 경우 * 하여 자릿수를 옮기고 반올림 하고 / 한다.
             * ex) round(42.4382, 2) 인 경우 round(4243.82) => 4244 가 나온다.
             *     42.44 로 원복한다.
             */
            else
            {
                sDoubleResult = stlRint( aValue1 * sTemp ) / sTemp;
            }
        }
    }

    *aResultValue = sDoubleResult;

    return STL_SUCCESS;
}


/**
 * @brief ROUND( date [, 'fmt' ] )
 *        <BR> date를 'fmt' 단위로 올림한 결과를 반환한다.
 *        <BR> ROUND( date [, 'fmt' ] )
 *        <BR>   EX1) TO_CHAR( ROUND( TO_DATE( '2014-07-21 14:25:33', 'YYYY-MM-DD HH24:MI:SS' ) ),
 *        <BR>                 'YYYY-MM-DD HH24:MI:SS' )
 *        <BR>        => '2014-07-22 00:00:00'
 *        <BR>   EX2) TO_CHAR( ROUND( TO_DATE( '2014-07-21 14:25:33', 'YYYY-MM-DD HH24:MI:SS' ), 'YYYY' ),
 *        <BR>                 'YYYY-MM-DD HH24:MI:SS' )
 *        <BR>        => '2015-01-01 00:00:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> ROUND( date [, 'fmt' ] )
 * @param[out] aResultValue      결과 (결과타입 DATE)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[in]  aEnv              environment (stlErrorStack)
 */
stlStatus dtfDateRound( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aInfo,
                        dtlFuncVector  * aVectorFunc,
                        void           * aVectorArg,
                        void           * aEnv )                  
{
    dtlDataValue   * sValue1 = NULL;
    dtlDataValue   * sValue2 = NULL;
    dtlDataValue   * sResultValue = NULL;
    stlErrorStack  * sErrorStack = NULL;

    stlChar        * sFormat = NULL;
    stlInt64         sFormatLen = 0;        
    dtlFractionalSecond     sFractionalSecond = 0;
    dtlDateType             sDateTypeValue = 0;
    dtlExpTime              sDtlExpTime;
    stlInt32                sDate = 0;
    stlInt32                sDateYearFirstDay = 0;
    stlInt32                sYearDay = 0;
    stlInt32                sMonthDay = 0;
    stlInt32                sWeekOfYear = 0;
    stlInt32                sYearDayForWeekOfYear = 0;
    stlInt32                sLeapYear = 0;
    stlInt32                i = 0;
    stlInt32                sWeekOfMonth = 0;
    stlInt32                sWeekDay = 0;
    stlInt32                sValue = 0;
    stlInt32                sCheckRoundValue = 0;

    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;
    
    DTL_PARAM_VALIDATE( ( aInputArgumentCnt >= 1 ) && ( aInputArgumentCnt <= 2 ),
                        sErrorStack );
    
    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_DATE, sErrorStack );
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_DATE, sErrorStack );    
    
    sDateTypeValue = DTF_DATE( sValue1 );
    
    if( aInputArgumentCnt == 1 )
    {
        /* ROUND( DATE, 'DD' ) */

        DTL_GET_DATE_TIME_FROM_TIMESTAMP( sDateTypeValue, sDate, DTL_USECS_PER_DAY );
        
        if( sDateTypeValue >= DTF_ROUND_HOUR_USECS_VALUE )
        {
            sDate++;
        }
        else
        {
            /* Do Nothing */
        }

        DTF_DATE( sResultValue ) = sDate * DTL_USECS_PER_DAY;

        /* validate */
        sDate = sDate + DTL_EPOCH_JDATE;
        STL_TRY_THROW( (sDate >= DTL_JULIAN_MINJULDAY) && (sDate <= DTL_JULIAN_MAXJULDAY),
                       ERROR_OUT_OF_RANGE );
        
        STL_THROW( RAMP_FINISH_ROUND );
    }
    else
    {
        sValue2 = aInputArgument[1].mValue.mDataValue;
        DTF_CHECK_DTL_GROUP2( sValue2,
                              DTL_GROUP_CHAR_STRING,
                              DTL_GROUP_LONGVARCHAR_STRING,
                              sErrorStack );
        sFormat = DTF_VARCHAR( sValue2 );
        sFormatLen = sValue2->mLength;

        if( ( ( sFormatLen == 3 ) && ( stlStrncasecmp( "DDD", sFormat, 3 ) == 0 ) ) ||
            ( ( sFormatLen == 2 ) && ( stlStrncasecmp( "DD", sFormat, 2 ) == 0 ) )  ||
            ( ( sFormatLen == 1 ) && ( stlStrncasecmp( "J", sFormat, 1 ) == 0 ) ) )
        {
            DTL_GET_DATE_TIME_FROM_TIMESTAMP( sDateTypeValue, sDate, DTL_USECS_PER_DAY );
            
            if( sDateTypeValue >= DTF_ROUND_HOUR_USECS_VALUE )
            {
                sDate++;
            }
            else
            {
                /* Do Nothing */
            }
            
            DTF_DATE( sResultValue ) = sDate * DTL_USECS_PER_DAY;
            
            /* validate */
            sDate = sDate + DTL_EPOCH_JDATE;
            STL_TRY_THROW( (sDate >= DTL_JULIAN_MINJULDAY) && (sDate <= DTL_JULIAN_MAXJULDAY),
                           ERROR_OUT_OF_RANGE );
            
            STL_THROW( RAMP_FINISH_ROUND );            
        }
        else
        {
            /*
             * Do Nothing
             * 아래 코드에서 처리
             */
        }
    }
    
    /*
     * Round
     */
    
    DTL_INIT_DTLEXPTIME( sDtlExpTime );
    STL_TRY( dtdDateTime2dtlExpTime( sDateTypeValue,
                                     & sDtlExpTime,
                                     & sFractionalSecond,
                                     sErrorStack )
             == STL_SUCCESS );
    
    if( ( ( sFormatLen == 4 ) && ( stlStrncasecmp( "YYYY", sFormat, 4 ) == 0 ) )  ||
        ( ( sFormatLen == 4 ) && ( stlStrncasecmp( "YEAR", sFormat, 4 ) == 0 ) )  ||
        ( ( sFormatLen == 5 ) && ( stlStrncasecmp( "SYYYY", sFormat, 5 ) == 0 ) ) ||
        ( ( sFormatLen == 5 ) && ( stlStrncasecmp( "SYEAR", sFormat, 5 ) == 0 ) ) ||
        ( ( sFormatLen == 3 ) && ( stlStrncasecmp( "YYY", sFormat, 3 ) == 0 ) )   ||
        ( ( sFormatLen == 2 ) && ( stlStrncasecmp( "YY", sFormat, 2 ) == 0 ) )    ||
        ( ( sFormatLen == 1 ) && ( stlStrncasecmp( "Y", sFormat, 1 ) == 0 ) ) )
    {
        /* 7월 1일이후로 반올림. */
        if( sDtlExpTime.mMonth >= DTF_ROUND_MONTH_VALUE )
        {
            sDtlExpTime.mYear = sDtlExpTime.mYear + 1;
        }
        else
        {
            /* Do Nothing */
        }
        
        sDtlExpTime.mMonth  = 1;
        sDtlExpTime.mDay    = 1;
        sDtlExpTime.mHour   = 0;
        sDtlExpTime.mMinute = 0;
        sDtlExpTime.mSecond = 0;
    }
    else if( ( ( sFormatLen == 5 ) && ( stlStrncasecmp( "MONTH", sFormat, 5 ) == 0 ) ) ||
             ( ( sFormatLen == 3 ) && ( stlStrncasecmp( "MON", sFormat, 3 ) == 0 ) )   ||
             ( ( sFormatLen == 2 ) && ( stlStrncasecmp( "MM", sFormat, 2 ) == 0 ) )    ||
             ( ( sFormatLen == 2 ) && ( stlStrncasecmp( "RM", sFormat, 2 ) == 0 ) ) )
    {
        /* 16일 이후로 반올림 */
        if( sDtlExpTime.mDay >= DTF_ROUND_DAY_VALUE )
        {
            sDtlExpTime.mMonth = sDtlExpTime.mMonth + 1;

            if( sDtlExpTime.mMonth > DTL_MONTHS_PER_YEAR )
            {
                sDtlExpTime.mYear += (sDtlExpTime.mMonth - 1) / DTL_MONTHS_PER_YEAR;
                sDtlExpTime.mMonth = ((sDtlExpTime.mMonth - 1) % DTL_MONTHS_PER_YEAR) + 1;
            }
            else
            {
                // Do Nothing
                STL_DASSERT( sDtlExpTime.mMonth >= 1 );
            }
        }
        else
        {
            /* Do Nothing */
        }

        sDtlExpTime.mDay    = 1;
        sDtlExpTime.mHour   = 0;
        sDtlExpTime.mMinute = 0;
        sDtlExpTime.mSecond = 0;        
    }
    else if( ( ( sFormatLen == 2 ) && ( stlStrncasecmp( "HH", sFormat, 2 ) == 0 ) )   ||
             ( ( sFormatLen == 4 ) && ( stlStrncasecmp( "HH12", sFormat, 4 ) == 0 ) ) ||
             ( ( sFormatLen == 4 ) && ( stlStrncasecmp( "HH24", sFormat, 4 ) == 0 ) ) )
    {
        if( sDtlExpTime.mMinute >= DTF_ROUND_MINUTE_VALUE )
        {
            sDateTypeValue += DTL_USECS_PER_HOUR;

            DTL_INIT_DTLEXPTIME( sDtlExpTime );
            STL_TRY( dtdDateTime2dtlExpTime( sDateTypeValue,
                                             & sDtlExpTime,
                                             & sFractionalSecond,
                                             sErrorStack )
                     == STL_SUCCESS );                       
        }
        else
        {
            /* Do Nothing */
        }
        
        sDtlExpTime.mMinute = 0;
        sDtlExpTime.mSecond = 0;                
    }
    else if( ( sFormatLen == 2 ) && ( stlStrncasecmp( "MI", sFormat, 2 ) == 0 ) )
    {
        if( sDtlExpTime.mSecond >= DTF_ROUND_SECOND_VALUE )
        {
            sDateTypeValue += DTL_USECS_PER_MINUTE;

            DTL_INIT_DTLEXPTIME( sDtlExpTime );
            STL_TRY( dtdDateTime2dtlExpTime( sDateTypeValue,
                                             & sDtlExpTime,
                                             & sFractionalSecond,
                                             sErrorStack )
                     == STL_SUCCESS );                       
        }
        else
        {
            /* Do Nothing */
        }
        
        sDtlExpTime.mSecond = 0;                        
    }
    else if( ( ( sFormatLen == 2 ) && ( stlStrncasecmp( "CC", sFormat, 2 ) == 0 ) )   ||
             ( ( sFormatLen == 3 ) && ( stlStrncasecmp( "SCC", sFormat, 3 ) == 0 ) ) )
    {
        sValue = ( sDtlExpTime.mYear / DTL_YEARS_PER_CENTURY );
        sCheckRoundValue = sDtlExpTime.mYear - ( sValue * DTL_YEARS_PER_CENTURY );
        
        if( sDtlExpTime.mYear > 0 )
        {
            if( sCheckRoundValue >= DTF_ROUND_YYYY_VALUE )
            {
                sValue = sValue + 1;
            }
            else
            {
                /* Do Nothing */
            }
            
            sDtlExpTime.mYear = ( sValue * DTL_YEARS_PER_CENTURY ) + 1;
        }
        else
        {
            if( sCheckRoundValue > -( DTF_ROUND_YYYY_VALUE - 1) )
            {
                sValue = sValue + 1;
            }
            else
            {
                /* Do Nothing */
            }
            
            sDtlExpTime.mYear = ( sValue * DTL_YEARS_PER_CENTURY ) - ( DTL_YEARS_PER_CENTURY - 1 );
        }
        
        sDtlExpTime.mMonth  = 1;
        sDtlExpTime.mDay    = 1;
        sDtlExpTime.mHour   = 0;
        sDtlExpTime.mMinute = 0;
        sDtlExpTime.mSecond = 0;
    }
    else if( ( sFormatLen == 1 ) && ( stlStrncasecmp( "Q", sFormat, 1 ) == 0 ) )
    {
        sValue = ( ( (sDtlExpTime.mMonth - 1) / DTL_MONTHS_PER_QUARTER ) * DTL_MONTHS_PER_QUARTER ) + 1;
        sCheckRoundValue = sValue + 1;

        /* 해당 분기의 두번째달의 16일부터 올림. */
        if( ( sDtlExpTime.mMonth > sCheckRoundValue )
            ||
            ( ( sDtlExpTime.mMonth == sCheckRoundValue ) &&
              ( sDtlExpTime.mDay >= DTF_ROUND_DAY_VALUE ) ) )
        {
            sDtlExpTime.mMonth = sValue + DTL_MONTHS_PER_QUARTER;
            
            if( sDtlExpTime.mMonth > DTL_MONTHS_PER_YEAR )
            {
                sDtlExpTime.mYear += (sDtlExpTime.mMonth - 1) / DTL_MONTHS_PER_YEAR;
                sDtlExpTime.mMonth = ((sDtlExpTime.mMonth - 1) % DTL_MONTHS_PER_YEAR) + 1;
            }
            else
            {
                // Do Nothing
                STL_DASSERT( sDtlExpTime.mMonth >= 1 );
            }
        }
        else
        {
            sDtlExpTime.mMonth = sValue;
        }
        
        sDtlExpTime.mDay = 1;
        sDtlExpTime.mHour   = 0;
        sDtlExpTime.mMinute = 0;
        sDtlExpTime.mSecond = 0;
    }
    else if( ( sFormatLen == 2 ) && ( stlStrncasecmp( "WW", sFormat, 2 ) == 0 ) )
    {
        sDate = dtdDate2JulianDate( sDtlExpTime.mYear,
                                    sDtlExpTime.mMonth,
                                    sDtlExpTime.mDay );
        
        sDateYearFirstDay = dtdDate2JulianDate( sDtlExpTime.mYear,
                                                1,
                                                1 );
        
        sYearDay = ( sDate - sDateYearFirstDay + 1 );
        sWeekOfYear = ( sYearDay - 1 ) / DTL_DAYS_PER_WEEK + 1;
        STL_DASSERT( ( sWeekOfYear >= 1 ) && ( sWeekOfYear <= 53 ) );
        
        sYearDayForWeekOfYear = ( ( sWeekOfYear - 1 ) * DTL_DAYS_PER_WEEK + 1 );
        sCheckRoundValue = ( sYearDay - sYearDayForWeekOfYear ) + 1;
        
        if( ( sCheckRoundValue > DTF_ROUND_WW_VALUE )
            ||
            ( ( sCheckRoundValue == DTF_ROUND_WW_VALUE ) &&
              ( sDtlExpTime.mHour >= DTF_ROUND_HOUR_VALUE ) ) )
        {
            /*
             * ex) round( to_date( '2014-01-05', 'yyyy-mm-dd' ), 'ww' )
             *     round( to_date( '2014-01-04 12:00:00', 'yyyy-mm-dd hh24:mi:ss' ), 'ww' )
             */ 
            
            sYearDayForWeekOfYear += DTL_DAYS_PER_WEEK;
        }
        else
        {
            /* Do Nothing */
        }
        
        /* 평년인지 윤년인지의 정보를 얻는다. */
        sLeapYear = ( DTL_IS_LEAP(sDtlExpTime.mYear) == STL_FALSE ) ? 0 : 1;
        
        for( i = 1; i <= DTL_MONTHS_PER_YEAR; i++ )
        {
            if( sYearDayForWeekOfYear <= dtlSumDayOfYear[sLeapYear][i] )
            {
                break;
            }
            else
            {
                /* Do Nothing */
            }
        }
        
        sDtlExpTime.mMonth = i;
        sDtlExpTime.mDay   = sYearDayForWeekOfYear - dtlSumDayOfYear[sLeapYear][i-1];
        
        sDtlExpTime.mHour   = 0;
        sDtlExpTime.mMinute = 0;
        sDtlExpTime.mSecond = 0;
    }
    else if( ( sFormatLen == 1 ) && ( stlStrncasecmp( "W", sFormat, 1 ) == 0 ) )
    {
        sWeekOfMonth = ( sDtlExpTime.mDay - 1 ) / DTL_DAYS_PER_WEEK + 1;
        STL_DASSERT( ( sWeekOfMonth >= 1 ) && ( sWeekOfMonth <= 5 ) );
        
        sMonthDay = ( ( sWeekOfMonth - 1 ) * DTL_DAYS_PER_WEEK ) + 1;
        sCheckRoundValue = ( sDtlExpTime.mDay - sMonthDay ) + 1;
        
        sDtlExpTime.mDay = sMonthDay;
        
        if( ( sCheckRoundValue > DTF_ROUND_W_VALUE )
            ||
            ( ( sCheckRoundValue == DTF_ROUND_W_VALUE ) &&
              ( sDtlExpTime.mHour >= DTF_ROUND_HOUR_VALUE ) ) )
        {
            /*
             * ex) round( to_date( '2014-01-05', 'yyyy-mm-dd' ), 'w' )
             *     round( to_date( '2014-01-04 12:00:00', 'yyyy-mm-dd hh24:mi:ss' ), 'w' )
             */
            
            sDtlExpTime.mDay += DTL_DAYS_PER_WEEK;
            
            if( sDtlExpTime.mDay > dtlDayTab[DTL_IS_LEAP(sDtlExpTime.mYear)][sDtlExpTime.mMonth-1] )
            {
                sDtlExpTime.mDay =
                    sDtlExpTime.mDay - dtlDayTab[DTL_IS_LEAP(sDtlExpTime.mYear)][sDtlExpTime.mMonth-1];
                sDtlExpTime.mMonth += 1;
            }
            else
            {
                /* Do Nothing */
            }

            /* 반올림되어 년도가 증가하는 경우는 없다. */
            STL_DASSERT( sDtlExpTime.mMonth <= DTL_MONTHS_PER_YEAR );
        }
        else
        {
            /* Do Nothing */
        }
        
        STL_DASSERT( sDtlExpTime.mDay <= dtlDayTab[DTL_IS_LEAP(sDtlExpTime.mYear)][sDtlExpTime.mMonth-1] );

        sDtlExpTime.mHour   = 0;
        sDtlExpTime.mMinute = 0;
        sDtlExpTime.mSecond = 0;
    }
    else if( ( ( sFormatLen == 3 ) && ( stlStrncasecmp( "DAY", sFormat, 3 ) == 0 ) ) ||
             ( ( sFormatLen == 2 ) && ( stlStrncasecmp( "DY", sFormat, 2 ) == 0 ) )  ||
             ( ( sFormatLen == 1 ) && ( stlStrncasecmp( "D", sFormat, 1 ) == 0 ) ) )
    {
        sDate = dtdDate2JulianDate( sDtlExpTime.mYear,
                                    sDtlExpTime.mMonth,
                                    sDtlExpTime.mDay );
        
        sWeekDay = ( sDate + 1 ) % 7;
        sDateTypeValue -= sWeekDay * DTL_USECS_PER_DAY;

        sCheckRoundValue = sWeekDay + 1;

        if( ( sCheckRoundValue > DTF_ROUND_D_VALUE )
            ||
            ( ( sCheckRoundValue == DTF_ROUND_D_VALUE ) &&
              ( sDtlExpTime.mHour >= DTF_ROUND_HOUR_VALUE ) ) )
        {
            sDateTypeValue += DTL_DAYS_PER_WEEK * DTL_USECS_PER_DAY;
        }
        else
        {
            /* Do Nothing */
        }

        DTL_INIT_DTLEXPTIME( sDtlExpTime );
        STL_TRY( dtdDateTime2dtlExpTime( sDateTypeValue,
                                         & sDtlExpTime,
                                         & sFractionalSecond,
                                         sErrorStack )
                 == STL_SUCCESS );                               
        
        sDtlExpTime.mHour   = 0;
        sDtlExpTime.mMinute = 0;
        sDtlExpTime.mSecond = 0;        
    }        
    else
    {
        STL_THROW( ERROR_FORMAT_STR );
    }
    
    /*
     * Date Value
     */
    
    STL_TRY( dtdDtlExpTime2DateTimeOffset( & sDtlExpTime,
                                           0, /* fractional second */
                                           & DTF_DATE( sResultValue ),
                                           sErrorStack )
             == STL_SUCCESS );

    STL_RAMP( RAMP_FINISH_ROUND );
    
    sResultValue->mLength = DTL_DATE_SIZE;
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_FORMAT_STR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_FORMAT_NOT_RECOGNIZED,
                      NULL,
                      sErrorStack );
    }

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATETIME_FIELD_OVERFLOW,
                      NULL,
                      sErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;    
}


/** @} */
