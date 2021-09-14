/*******************************************************************************
 * dtfTrunc.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtfTrunc.c 1296 2011-06-30 07:24:30Z ehpark $
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
 * @file dtfTrunc.c
 * @brief dtfTrunc Function DataType Library Layer
 */

#include <dtl.h>
#include <dtlDef.h>
#include <dtDef.h>
#include <dtfArithmetic.h>
#include <dtfNumeric.h>
#include <dtfMathematical.h>



/**
 * @addtogroup dtfTrunc Trunc
 * @ingroup dtf
 * @internal
 * @{
 */


/*******************************************************************************
 * trunc(x) ,  trunc(x, y)
 *   trunc(x)    => [ P, O ]
 *   trunc(x, y) => [ P, M, O ]
 * ex) trunc(42.8)        =>  42
 * ex) trunc(42.4382, 2)  => 42.43
 ******************************************************************************/

/**
 * @brief trunc(x) ,  trunc(x, y) 함수
 *        <BR> trunc(x) ,  trunc(x, y)
 *        <BR>   trunc(x)    => [ P, O ]
 *        <BR>   trunc(x, y) => [ P, M, O ]
 *        <BR> ex) trunc(42.8)        =>  42
 *        <BR> ex) trunc(42.4382, 2)  => 42.43
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> trunc(BIGINT), trunc(BIGINT, INTEGER)
 * @param[out] aResultValue      연산 결과 (결과타입 BIGINT)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfBigIntTrunc( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv )
{
    /**
     * aValue2가 NULL인 경우는 ( trunc(x) )
     * aValue2가 NULL이 아닌 경우는 ( trunc(x, y) )
     */

    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;
    dtlDataValue  * sResultValue;
    stlInt64        sTruncValue;

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


        /*
         * trunc 연산을 한다.
         * 변수가 두개인 경우는 postgres에 없음.
         */
        if( DTF_INTEGER( sValue2 ) < 0 )
        {
            if( DTF_INTEGER( sValue2 ) < -19 )
            {
                DTF_BIGINT( sResultValue ) = 0;
            }
            else
            {
                sTruncValue = (stlInt64)gPreDefinedPow[ -DTF_INTEGER( sValue2 ) ];
                DTF_BIGINT( sResultValue ) =
                    (stlInt64)( ( DTF_BIGINT( sValue1 ) / sTruncValue ) * sTruncValue );
            }
        }
        else
        {
            DTF_BIGINT( sResultValue ) = DTF_BIGINT( sValue1 );
        }
    }

    sResultValue->mLength = DTL_NATIVE_BIGINT_SIZE;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief trunc(x) ,  trunc(x, y) 함수
 *        <BR> trunc(x) ,  trunc(x, y)
 *        <BR>   trunc(x)    => [ P, O ]
 *        <BR>   trunc(x, y) => [ P, M, O ]
 *        <BR> ex) trunc(42.8)        =>  42
 *        <BR> ex) trunc(42.4382, 2)  => 42.43
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> trunc(NUMERIC), trunc(NUMERIC, INTEGER)
 * @param[out] aResultValue      연산 결과 (결과타입 NUMERIC)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfNumericTrunc( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sResultValue;
    stlInt32          sScale;

    dtlNumericType  * sNumeric;
    dtlNumericType  * sResult;

    stlBool       sIsPositive;
    stlInt32      sDigitCount;
    stlInt32      sExponent;
    stlInt32      sTruncPos;
    stlUInt8    * sDigitPtr;
    stlUInt8      sFirstValueSize;
    stlUInt8      sTmpValue;
    stlUInt8      sTmpPos;

    stlUInt8    * sResultDigitPtr;


    DTL_PARAM_VALIDATE( ( aInputArgumentCnt == 1 ) || ( aInputArgumentCnt == 2 ) ,
                        (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;
    sValue1 = aInputArgument[0].mValue.mDataValue;
    DTF_CHECK_DTL_VALUE2( sValue1, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, (stlErrorStack *)aEnv );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, (stlErrorStack *)aEnv );

    if( aInputArgumentCnt == 2 )
    {
        sValue2 = aInputArgument[1].mValue.mDataValue;
        DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_NATIVE_INTEGER, (stlErrorStack *)aEnv );
    }
    else
    {
        sValue2 = NULL;
    }

    /* trunc 연산을 한다. */
    if ( sValue2 == NULL )
    {
        sScale = 0;
    }
    else
    {
        sScale = DTF_INTEGER( sValue2 );
    }

    sNumeric = DTF_NUMERIC( sValue1 );
    sResult = DTF_NUMERIC( sResultValue );

    
    if( DTL_NUMERIC_IS_ZERO( sNumeric, sValue1->mLength ) == STL_TRUE )
    {
        DTL_NUMERIC_SET_ZERO( sResult, sResultValue->mLength );
        STL_THROW( RAMP_FINISH );
    }

    if( DTL_NUMERIC_IS_POSITIVE( sNumeric ) == STL_TRUE )
    {
        sIsPositive = STL_TRUE;
        sExponent = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( sNumeric );
        sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( sValue1->mLength );
        sDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );
        sFirstValueSize = ( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigitPtr ) < 10
                            ? 1 : 2 );
    }
    else
    {
        sIsPositive = STL_FALSE;
        sExponent = DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( sNumeric );
        sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( sValue1->mLength );
        sDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );
        sFirstValueSize = ( DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigitPtr ) < 10
                            ? 1 : 2 );
    }

    sTruncPos = sExponent * 2 + sScale + sFirstValueSize;

    if( sTruncPos <= 0 )
    {
        DTL_NUMERIC_SET_ZERO( sResult, sResultValue->mLength );
    }
    else if( sTruncPos >= ((sDigitCount-1) * 2 + sFirstValueSize) )
    {
        DTL_COPY_DATA_VALUE( sResultValue->mBufferSize, sValue1, sResultValue );
    }
    else
    {
        if( sFirstValueSize == 1 )
        {
            sTruncPos++;
        }

        sTmpPos = (sTruncPos / 2);

        if( sTruncPos & ((stlInt32)1) )
        {
            if( sTmpPos == 0 )
            {
                sTmpValue = DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sIsPositive, sDigitPtr[0] );
                if( sTmpValue < 10 )
                {
                    DTL_NUMERIC_SET_ZERO( sResult, sResultValue->mLength );
                    STL_THROW( RAMP_FINISH );
                }
                else
                {
                    sResultValue->mLength = 2;
                    DTL_NUMERIC_SET_SIGN_AND_EXPONENT( sResult, sIsPositive, sExponent );
                    sTmpValue = (sTmpValue / 10) * 10;
                    sResultDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sResult );
                    *sResultDigitPtr = DTL_NUMERIC_GET_DIGIT_FROM_VALUE( sIsPositive, sTmpValue );
                    STL_THROW( RAMP_FINISH );
                }
            }

            sTmpValue = DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sIsPositive, sDigitPtr[sTmpPos] );
            sTmpValue = (sTmpValue / 10) * 10;
            if( sTmpValue == 0 )
            {
                sTmpPos--;
                sTmpValue = DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sIsPositive, sDigitPtr[sTmpPos] );
            }

            sResultValue->mLength = sTmpPos + 2;
            DTL_NUMERIC_SET_SIGN_AND_EXPONENT( sResult, sIsPositive, sExponent );

            sResultDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sResult );
            sResultDigitPtr[sTmpPos] = DTL_NUMERIC_GET_DIGIT_FROM_VALUE( sIsPositive, sTmpValue );

            while( sTmpPos > 0 )
            {
                sTmpPos--;
                sResultDigitPtr[sTmpPos] = sDigitPtr[sTmpPos];
            }
        }
        else
        {
            sTmpPos--;

            sResultValue->mLength = sTmpPos + 2;
            DTL_NUMERIC_SET_SIGN_AND_EXPONENT( sResult, sIsPositive, sExponent );

            sResultDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sResult );
            sResultDigitPtr[sTmpPos] = sDigitPtr[sTmpPos];
            while( sTmpPos > 0 )
            {
                sTmpPos--;
                sResultDigitPtr[sTmpPos] = sDigitPtr[sTmpPos];
            }

        }
    }

    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief trunc(x) ,  trunc(x, y) 함수
 *        <BR> trunc(x) ,  trunc(x, y)
 *        <BR>   trunc(x)    => [ P, O ]
 *        <BR>   trunc(x, y) => [ P, M, O ]
 *        <BR> ex) trunc(42.8)        =>  42
 *        <BR> ex) trunc(42.4382, 2)  => 42.43
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> trunc(DOUBLE), trunc(DOUBLE, INTEGER)
 * @param[out] aResultValue      연산 결과 (결과타입 DOUBLE)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfDoubleTrunc( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv )
{
    /**
     * aValue2가 NULL인 경우는 ( trunc(x) )
     * aValue2가 NULL이 아닌 경우는 ( trunc(x, y) )
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

    /* Double의 경우는 input이 finite인지 확인한다. */
    STL_DASSERT( stlIsinfinite( DTF_DOUBLE( sValue1 ) )
                 == STL_IS_INFINITE_FALSE );

    if( aInputArgumentCnt == 1 )
    {
        if( DTF_DOUBLE( sValue1 ) >= 0 )
        {
            DTF_DOUBLE( sResultValue ) = stlFloor( DTF_DOUBLE( sValue1 ) );
        }
        else
        {
            DTF_DOUBLE( sResultValue ) = -stlFloor( -DTF_DOUBLE( sValue1 ) );
        }
    }
    else
    {
        sValue2 = aInputArgument[1].mValue.mDataValue;
        DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_NATIVE_INTEGER, sErrorStack );

        /*
         * trunk 연산을 한다.
         * 변수가 두개인 경우는 postgres에 없음.
         */
        STL_TRY( dtfDoubleRoundWithScale( DTF_DOUBLE( sValue1 ),
                                          DTF_INTEGER( sValue2 ),
                                          STL_TRUE,
                                          &DTF_DOUBLE( sResultValue),
                                          sErrorStack )
                 == STL_SUCCESS );
    }

    sResultValue->mLength = DTL_NATIVE_DOUBLE_SIZE;

    
    /*
     * trunk 연산후 overflow또는 underflows 나지 않음.
     * postgres에도 체크하지 않음.
     */

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief TRUNC( date [, 'fmt' ] )
 *        <BR> date를 'fmt' 단위로 버림한 결과를 반환한다.
 *        <BR> TRUNC( date [, 'fmt' ] )
 *        <BR>   EX1) TO_CHAR( TRUNC( TO_DATE( '2014-05-21 14:25:33', 'YYYY-MM-DD HH24:MI:SS' ) ),
 *        <BR>                 'YYYY-MM-DD HH24:MI:SS' )
 *        <BR>        => '2014-05-21 00:00:00'
 *        <BR>   EX2) TO_CHAR( TRUNC( TO_DATE( '2014-05-21 14:25:33', 'YYYY-MM-DD HH24:MI:SS' ), 'YYYY' ),
 *        <BR>                 'YYYY-MM-DD HH24:MI:SS' )
 *        <BR>        => '2014-01-01 00:00:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> TRUNC( date [, 'fmt' ] )
 * @param[out] aResultValue      결과 (결과타입 DATE)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[in]  aEnv              environment (stlErrorStack)
 */
stlStatus dtfDateTrunc( stlUInt16        aInputArgumentCnt,
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
    stlInt32                sWeekOfYear = 0;
    stlInt32                sYearDayForWeekOfYear = 0;
    stlInt32                sLeapYear = 0;
    stlInt32                i = 0;
    stlInt32                sWeekOfMonth = 0;
    stlInt32                sWeekDay = 0;

    
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
        /* TRUNC( DATE, 'DD' ) */
        
        DTL_GET_DATE_TIME_FROM_TIMESTAMP( sDateTypeValue, sDate, DTL_USECS_PER_DAY );
        DTF_DATE( sResultValue ) = sDate * DTL_USECS_PER_DAY;

        /* validate */
        sDate = sDate + DTL_EPOCH_JDATE;
        STL_TRY_THROW( (sDate >= DTL_JULIAN_MINJULDAY) && (sDate <= DTL_JULIAN_MAXJULDAY),
                       ERROR_OUT_OF_RANGE );        
        
        STL_THROW( RAMP_FINISH_TRUNC );
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
            DTF_DATE( sResultValue ) = sDate * DTL_USECS_PER_DAY;

            /* validate */
            sDate = sDate + DTL_EPOCH_JDATE;
            STL_TRY_THROW( (sDate >= DTL_JULIAN_MINJULDAY) && (sDate <= DTL_JULIAN_MAXJULDAY),
                           ERROR_OUT_OF_RANGE );        
            
            STL_THROW( RAMP_FINISH_TRUNC );
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
     * Trunc
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
        sDtlExpTime.mDay    = 1;
        sDtlExpTime.mHour   = 0;
        sDtlExpTime.mMinute = 0;
        sDtlExpTime.mSecond = 0;        
    }
    else if( ( ( sFormatLen == 2 ) && ( stlStrncasecmp( "HH", sFormat, 2 ) == 0 ) )   ||
             ( ( sFormatLen == 4 ) && ( stlStrncasecmp( "HH12", sFormat, 4 ) == 0 ) ) ||
             ( ( sFormatLen == 4 ) && ( stlStrncasecmp( "HH24", sFormat, 4 ) == 0 ) ) )
    {
        sDtlExpTime.mMinute = 0;
        sDtlExpTime.mSecond = 0;                
    }
    else if( ( sFormatLen == 2 ) && ( stlStrncasecmp( "MI", sFormat, 2 ) == 0 ) )
    {
        sDtlExpTime.mSecond = 0;                        
    }
    else if( ( ( sFormatLen == 2 ) && ( stlStrncasecmp( "CC", sFormat, 2 ) == 0 ) )   ||
             ( ( sFormatLen == 3 ) && ( stlStrncasecmp( "SCC", sFormat, 3 ) == 0 ) ) )
    {
        if( sDtlExpTime.mYear > 0 )
        {
            sDtlExpTime.mYear = ( sDtlExpTime.mYear / DTL_YEARS_PER_CENTURY * DTL_YEARS_PER_CENTURY ) + 1;
        }
        else
        {
            sDtlExpTime.mYear =
                ( sDtlExpTime.mYear / DTL_YEARS_PER_CENTURY * DTL_YEARS_PER_CENTURY )
                - ( DTL_YEARS_PER_CENTURY - 1 );
        }
        
        sDtlExpTime.mMonth  = 1;
        sDtlExpTime.mDay    = 1;
        sDtlExpTime.mHour   = 0;
        sDtlExpTime.mMinute = 0;
        sDtlExpTime.mSecond = 0;
    }
    else if( ( sFormatLen == 1 ) && ( stlStrncasecmp( "Q", sFormat, 1 ) == 0 ) )
    {
        sDtlExpTime.mMonth  = ( ( (sDtlExpTime.mMonth - 1) / 3 ) * 3 ) + 1;
        sDtlExpTime.mDay    = 1;
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
        
        sDtlExpTime.mDay = ( ( sWeekOfMonth - 1 ) * DTL_DAYS_PER_WEEK ) + 1;
        
        sDtlExpTime.mHour   = 0;
        sDtlExpTime.mMinute = 0;
        sDtlExpTime.mSecond = 0;
    }
    else if( ( ( sFormatLen == 3 ) && ( stlStrncasecmp( "DAY", sFormat, 3 ) == 0 ) ) ||
             ( ( sFormatLen == 2 ) && ( stlStrncasecmp( "DY", sFormat, 2 ) == 0 ) ) ||
             ( ( sFormatLen == 1 ) && ( stlStrncasecmp( "D", sFormat, 1 ) == 0 ) ) )
    {
        sDate = dtdDate2JulianDate( sDtlExpTime.mYear,
                                    sDtlExpTime.mMonth,
                                    sDtlExpTime.mDay );
        
        sWeekDay = ( sDate + 1 ) % 7;

        sDateTypeValue -= sWeekDay * DTL_USECS_PER_DAY;

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
    
    STL_RAMP( RAMP_FINISH_TRUNC );    
    
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
