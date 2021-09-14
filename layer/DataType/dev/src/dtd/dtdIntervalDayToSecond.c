/*******************************************************************************
 * dtdIntervalDayToSecond.c
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
 * @file dtdIntervalDayToSecond.c
 * @brief DataType Layer dtdIntervalDayToSecond 정의
 */

#include <dtl.h>

#include <dtdDef.h>
#include <dtdDataType.h>

/**
 * @addtogroup dtdIntervalDayToSecond
 * @{
 */

/**
 * @brief dtdIntervalDayToSecond 타입의 length값을 얻음
 * @param[in]  aPrecision
 * @param[in]  aStringUnit string length unit (dtlStringLengthUnit 참조) 
 * @param[out] aLength     dtdIntervalDayToSecond의 길이 값
 * @param[in]  aVectorFunc Function Vector
 * @param[in]  aVectorArg  Vector Argument
 * @param[out] aErrorStack 에러 스택
 */
stlStatus dtdIntervalDayToSecondGetLength( stlInt64              aPrecision,
                                           dtlStringLengthUnit   aStringUnit,
                                           stlInt64            * aLength,
                                           dtlFuncVector       * aVectorFunc,
                                           void                * aVectorArg,
                                           stlErrorStack       * aErrorStack )
{
    /* DTL_PARAM_VALIDATE( (aPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) && */
    /*                     (aPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION), */
    /*                     aErrorStack ); */
    
    *aLength = DTL_INTERVAL_DAY_TO_SECOND_SIZE;
    
    return STL_SUCCESS;

    /* STL_FINISH; */

    /* return STL_FAILURE; */
}


/**
 * @brief dtdIntervalDayToSecond 타입의 length값을 얻음 (입력 문자열에 영향을 받지 않음)
 * @param[in]  aPrecision  
 * @param[in]  aScale
 * @param[in]  aStringUnit   string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aString       입력 문자열
 * @param[in]  aStringLength aString의 길이    
 * @param[out] aLength       dtdIntervalDayToSecond의 길이 값
 * @param[in]  aVectorFunc   Function Vector
 * @param[in]  aVectorArg    Vector Argument
 * @param[out] aErrorStack   에러 스택
 */
stlStatus dtdIntervalDayToSecondGetLengthFromString( stlInt64              aPrecision,
                                                     stlInt64              aScale,
                                                     dtlStringLengthUnit   aStringUnit,
                                                     stlChar             * aString,
                                                     stlInt64              aStringLength,
                                                     stlInt64            * aLength,
                                                     dtlFuncVector       * aVectorFunc,
                                                     void                * aVectorArg,
                                                     stlErrorStack       * aErrorStack )
{
    DTL_PARAM_VALIDATE( (aPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                        (aPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION),
                        aErrorStack );
    DTL_PARAM_VALIDATE( ( (aScale >= DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION) &&
                          (aScale <= DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION) ) ||
                        ( aScale == DTL_SCALE_NA ),
                        aErrorStack );

    *aLength = DTL_INTERVAL_DAY_TO_SECOND_SIZE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief dtdIntervalDayToSecond 타입의 length값을 얻음 (입력 숫자에 영향을 받지 않음)
 * @param[in]  aPrecision  
 * @param[in]  aScale      
 * @param[in]  aInteger    입력 숫자
 * @param[out] aLength     dtdIntervalDayToSecond의 길이 값
 * @param[out] aErrorStack 에러 스택
 */
stlStatus dtdIntervalDayToSecondGetLengthFromInteger( stlInt64          aPrecision,
                                                      stlInt64          aScale,
                                                      stlInt64          aInteger,
                                                      stlInt64        * aLength,
                                                      stlErrorStack   * aErrorStack )
{
    DTL_PARAM_VALIDATE( (aPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                        (aPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION),
                        aErrorStack );
    DTL_PARAM_VALIDATE( ( (aScale >= DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION) &&
                          (aScale <= DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION) ) ||
                        ( aScale == DTL_SCALE_NA ),
                        aErrorStack );

    *aLength = DTL_INTERVAL_DAY_TO_SECOND_SIZE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/* /\** */
/*  * @brief dtdIntervalDayToSecond 타입의 length값을 얻음 (입력 숫자에 영향을 받지 않음) */
/*  * @param[in]  aPrecision   */
/*  * @param[in]  aScale       */
/*  * @param[in]  aReal       입력 숫자 */
/*  * @param[out] aLength     Date의 길이 값 */
/*  * @param[out] aErrorStack 에러 스택 */
/*  *\/ */
/* stlStatus dtdIntervalDayToSecondGetLengthFromReal( stlInt64          aPrecision, */
/*                                                    stlInt64          aScale, */
/*                                                    stlFloat64        aReal, */
/*                                                    stlInt64        * aLength, */
/*                                                    stlErrorStack   * aErrorStack ) */
/* { */
/*     stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                   DTL_ERRCODE_NOT_IMPLEMENTED, */
/*                   NULL, */
/*                   aErrorStack, */
/*                   "dtdIntervalDayToSecondGetLengthFromReal()" ); */

/*     return STL_FAILURE; */
/* } */

/**
 * @brief string value로부터 dtdIntervalDayToSecond 타입의 value 구성
 * @param[in]  aString            value에 저장될 입력 문자열
 * @param[in]  aStringLength      aString의 길이  
 * @param[in]  aPrecision         precision
 *                           <BR> interval type에서는 leading field precision 정보 
 * @param[in]  aScale             scale
 *                           <BR> interval type에서는 fractional second precision 정보
 * @param[in]  aStringUnit      string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aIntervalIndicator INTERVAL 타입의 Indicator
 *                           <BR> dtlIntervalIndicator 참조 
 * @param[in]  aAvailableSize     mValue에 사용할 수 있는 메모리 공간의 크기  
 * @param[out] aValue             dtlDataValue의 주소
 * @param[out] aSuccessWithInfo   warning 발생 여부
 * @param[in]  aSourceVectorFunc  Source Function Vector
 * @param[in]  aSourceVectorArg   Source Vector Argument
 * @param[in]  aDestVectorFunc    Dest Function Vector
 * @param[in]  aDestVectorArg     Dest Vector Argument
 * @param[out] aErrorStack        에러 스택
 *
 * @remark
 * DAY, HOUR, MINUTE, SECOND,
 * DAY TO HOUR, DAY TO MINUTE, DAY TO SECOND,
 * HOUR TO MINUTE, HOUR TO SECOND,
 * MINUTE TO SECOND interval type에 string으로 입력 가능한 포멧은 다음과 같다.
 * @code
 *
 * <unquoted interval string> ::=
 *      [ <sign> ] { <year-month literal> | <day-time literal> }
 * 
 * <day-time literal> ::=
 *      <day-time interval>
 *      | <time interval>
 * 
 * <day-time interval> ::=
 *      <days value> [ <space> <hours value> [ <colon> <minutes value> [ <colon> <seconds value> ] ] ]
 * 
 * <time interval> ::=
 *      <hours value> [ <colon> <minutes value> [ <colon> <seconds value> ] ]
 *      | <minutes value> [ <colon> <seconds value> ]
 *      | <seconds value>
 * 
 *  (1) day hour:minute:second.fractionalsecond
 *      '3 10:11:12.123456' ( 3 day 10 hour 11 minute 12.123456 second )
 *  (2) time
 *      '10:11:12.123456'
 *  (3) number
 *      number는 single field에만 해당됨.
 *      ( INTERVAL DAY, INTERVAL HOUR, INTERVAL MINUTE, INTERVAL SECOND )
 *      예1) INTERVAL DAY에 '3' 입력시,
 *          '3'을 DAY의 값으로 인식해서 +3 00:00:00으로 데이타가 들어감.
 *  (4) time zone시간대
 *      '+09:00'
 * @endcode
 */
stlStatus dtdIntervalDayToSecondSetValueFromString( stlChar              * aString,
                                                    stlInt64               aStringLength,
                                                    stlInt64               aPrecision,
                                                    stlInt64               aScale,
                                                    dtlStringLengthUnit    aStringUnit,
                                                    dtlIntervalIndicator   aIntervalIndicator,
                                                    stlInt64               aAvailableSize,
                                                    dtlDataValue         * aValue,
                                                    stlBool              * aSuccessWithInfo,
                                                    dtlFuncVector        * aSourceVectorFunc,
                                                    void                 * aSourceVectorArg,
                                                    dtlFuncVector        * aDestVectorFunc,
                                                    void                 * aDestVectorArg,
                                                    stlErrorStack        * aErrorStack )
{
    dtlIntervalDayToSecondType  sIntervalDayToSecondType;
    dtlIntervalDayToSecondType  sTempIntervalDayToSecondType;
    dtlExpTime                  sDtlExpTime;
    dtlFractionalSecond         sFractionalSecond;
    stlChar                     sBuffer[256];
    stlChar                   * sField[DTL_MAX_DATE_FIELDS];
    stlInt32                    sFieldType[DTL_MAX_DATE_FIELDS];
    stlInt32                    sNumFields;
    stlInt32                    sDateType;

    DTL_PARAM_VALIDATE( (aPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                        (aPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION),
                        aErrorStack );
    DTL_PARAM_VALIDATE( ( (aScale >= DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION) &&
                          (aScale <= DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION) ) ||
                        ( aScale == DTL_SCALE_NA ),
                        aErrorStack );

    DTL_PARAM_VALIDATE( ( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_HOUR) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_SECOND) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND) )
                        == STL_TRUE,
                        aErrorStack );

    DTL_PARAM_VALIDATE( ( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_HOUR) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE) )
                        ||
                        ( ( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND) ||
                            (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_SECOND) ||
                            (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND) ||
                            (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND) ) &&
                          ( (aScale >= DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION) &&
                            (aScale <= DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION) ) ),
                        aErrorStack );
    
    DTL_PARAM_VALIDATE( aAvailableSize >= DTL_INTERVAL_DAY_TO_SECOND_SIZE, aErrorStack );
    DTL_CHECK_TYPE( DTL_TYPE_INTERVAL_DAY_TO_SECOND, aValue, aErrorStack );

    DTL_PARAM_VALIDATE( aString != NULL, aErrorStack );

    *aSuccessWithInfo = STL_FALSE;
    
    sIntervalDayToSecondType.mIndicator = aIntervalIndicator;

    DTL_INIT_DTLEXPTIME( sDtlExpTime );

    /*
     * INTERVAL의 aPrecision, aScale은 다음과 같은 용도로 사용된다.
     * aPrecision => interval leading field precision
     * aScale     => interval fractional second precision
     * 예)interval year[(year precision = aPrecision)] to month
     * 예)interval day[(day precision = aPrecision)] to second[(fraction_second_precision = aScale)]
     */

    STL_TRY( dtdParseDateTime( aString,
                               aStringLength,
                               DTL_TYPE_INTERVAL_DAY_TO_SECOND,
                               sBuffer,
                               STL_SIZEOF(sBuffer),
                               sField,
                               sFieldType,
                               DTL_MAX_DATE_FIELDS,
                               & sNumFields,
                               aErrorStack )
             == STL_SUCCESS );

    /*
     * input string에 대한 체크
     * DAY, HOUR, MINUTE, SECOND,
     * DAY TO HOUR, DAY TO MINUTE, DAY TO SECOND,
     * HOUR TO MINUTE, HOUR TO SECOND,
     * MINUTE TO SECOND interval type이외의 불필요한 정보가 있는 경우 에러처리한다.
     * 예) '3 11:22:33.123456' ( O )
     *     '1-2 3 11:22:33.12345' ( X )
     */

    /*
     * DAY, HOUR, MINUTE, SECOND,
     * DAY TO HOUR, DAY TO MINUTE, DAY TO SECOND,
     * HOUR TO MINUTE, HOUR TO SECOND,
     * MINUTE TO SECOND interval type에 string으로 입력 가능한 포멧은 다음과 같다.
     *
     * <unquoted interval string> ::=
     *      [ <sign> ] { <year-month literal> | <day-time literal> }
     * 
     * <day-time literal> ::=
     *      <day-time interval>
     *      | <time interval>
     * 
     * <day-time interval> ::=
     *      <days value> [ <space> <hours value> [ <colon> <minutes value> [ <colon> <seconds value> ] ] ]
     * 
     * <time interval> ::=
     *      <hours value> [ <colon> <minutes value> [ <colon> <seconds value> ] ]
     *      | <minutes value> [ <colon> <seconds value> ]
     *      | <seconds value>
     * 
     *  (1) day hour:minute:second.fractionalsecond
     *      '3 10:11:12.123456' ( 3 day 10 hour 11 minute 12.123456 second )
     *  (2) time
     *      '10:11:12.123456'
     *  (3) number
     *      number는 single field에만 해당됨.
     *      ( INTERVAL DAY, INTERVAL HOUR, INTERVAL MINUTE, INTERVAL SECOND )
     *      예1) INTERVAL DAY에 '3' 입력시,
     *          '3'을 DAY의 값으로 인식해서 +3 00:00:00으로 데이타가 들어감.
     *  (4) time zone시간대
     *      '+09:00'
     */
    switch( sNumFields )
    {
        case 2:
            {
                STL_TRY_THROW( (sFieldType[1] == DTL_DTK_TIME) ||
                               (sFieldType[1] == DTL_DTK_NUMBER),
                               RAMP_ERROR_BAD_FORMAT );
                
                STL_TRY_THROW( (sFieldType[0] == DTL_DTK_NUMBER) ||
                               (sFieldType[0] == DTL_DTK_TZ),
                               RAMP_ERROR_BAD_FORMAT );

                break;
            }
        case 1:
            {
                STL_TRY_THROW( (sFieldType[0] == DTL_DTK_TIME) ||
                               (sFieldType[0] == DTL_DTK_NUMBER) ||
                               (sFieldType[0] == DTL_DTK_TZ),
                               RAMP_ERROR_BAD_FORMAT );

                if( sFieldType[0] == DTL_DTK_TZ )
                {
                    if( stlStrchr((sField[0] + 1), ':') != NULL )
                    {
                        // Do Nothing
                    }
                    else
                    {
                        sFieldType[0] = DTL_DTK_NUMBER;
                    }
                }
                else
                {
                    // Do Nothing
                }
                
                if( sFieldType[0] == DTL_DTK_NUMBER )
                {
                    if( stlStrchr((sField[0] + 1), '.') == NULL )
                    {
                        STL_TRY_THROW( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY)    ||
                                       (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR)   ||
                                       (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE) ||
                                       (aIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND),
                                       RAMP_ERROR_BAD_FORMAT );
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }
                else
                {
                    // Do Nothing
                }
                
                break;
            }
        default:
            {
                STL_THROW( RAMP_ERROR_BAD_FORMAT );
                break;
            }
    }
    
    STL_TRY( dtdDecodeInterval( sField,
                                sFieldType,
                                sNumFields,
                                aIntervalIndicator,
                                & sDateType,
                                & sDtlExpTime,
                                & sFractionalSecond,
                                aErrorStack )
             == STL_SUCCESS );

    /*
     * value truncated check
     */
    STL_TRY( dtdIntervalValueTruncatedCheck( & sDtlExpTime,
                                             aIntervalIndicator,
                                             & sFractionalSecond,
                                             aErrorStack )
             == STL_SUCCESS );
    
    switch( sDateType )
    {
        case DTL_DTK_DELTA:
            {
                dtdDtlExpTime2IntervalDayToSecond( & sDtlExpTime,
                                                   sFractionalSecond,
                                                   aIntervalIndicator,
                                                   & sIntervalDayToSecondType );
            }
            break;
        default:
            STL_THROW( RAMP_ERROR_BAD_FORMAT );
            break;
    }

    sTempIntervalDayToSecondType.mIndicator = sIntervalDayToSecondType.mIndicator;
    sTempIntervalDayToSecondType.mDay       = sIntervalDayToSecondType.mDay;
    sTempIntervalDayToSecondType.mTime      = sIntervalDayToSecondType.mTime;
    
    STL_TRY( dtdAdjustIntervalDayToSecond( & sIntervalDayToSecondType,
                                           aIntervalIndicator,
                                           aPrecision,
                                           aScale,
                                           aErrorStack )
             == STL_SUCCESS );    
    
    (((dtlIntervalDayToSecondType *)(aValue->mValue))->mIndicator) =
        sIntervalDayToSecondType.mIndicator;

    ((dtlIntervalDayToSecondType *)(aValue->mValue))->mDay = sIntervalDayToSecondType.mDay;
    ((dtlIntervalDayToSecondType *)(aValue->mValue))->mTime = sIntervalDayToSecondType.mTime;

    aValue->mLength = DTL_INTERVAL_DAY_TO_SECOND_SIZE;

    if( sIntervalDayToSecondType.mTime != sTempIntervalDayToSecondType.mTime )
    {
        *aSuccessWithInfo = STL_TRUE;
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

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief integer value로부터 dtdIntervalDayToSecond 타입의 value 구성
 *  <BR>  interval single field에 대해서만 지원.
 *        ( interval DAY, HOUR, MINUTE, SECOND ) 
 * @param[in]  aInteger           value에 저장될 입력 숫자
 * @param[in]  aPrecision      
 * @param[in]  aScale
 * @param[in]  aStringUnit        string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aIntervalIndicator INTERVAL 타입의 Indicator
 *                           <BR> dtlIntervalIndicator 참조 
 * @param[in]  aAvailableSize     mValue에 사용할 수 있는 메모리 공간의 크기  
 * @param[out] aValue             dtlDataValue의 주소
 * @param[out] aSuccessWithInfo   warning 발생 여부
 * @param[in]  aVectorFunc        Function Vector
 * @param[in]  aVectorArg         Vector Argument
 * @param[out] aErrorStack        에러 스택
 */
stlStatus dtdIntervalDayToSecondSetValueFromInteger( stlInt64               aInteger,
                                                     stlInt64               aPrecision,
                                                     stlInt64               aScale,
                                                     dtlStringLengthUnit    aStringUnit,
                                                     dtlIntervalIndicator   aIntervalIndicator,
                                                     stlInt64               aAvailableSize,
                                                     dtlDataValue         * aValue,
                                                     stlBool              * aSuccessWithInfo,
                                                     dtlFuncVector        * aVectorFunc,
                                                     void                 * aVectorArg,
                                                     stlErrorStack        * aErrorStack )
{
    dtlIntervalDayToSecondType  sIntervalDayToSecondType;
    dtlExpTime                  sDtlExpTime;

    DTL_PARAM_VALIDATE( aAvailableSize >= DTL_INTERVAL_DAY_TO_SECOND_SIZE, aErrorStack );
    DTL_CHECK_TYPE( DTL_TYPE_INTERVAL_DAY_TO_SECOND, aValue, aErrorStack );

    DTL_PARAM_VALIDATE( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY)    ||
                        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR)   ||
                        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE) ||
                        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND),
                        aErrorStack );

    DTL_PARAM_VALIDATE( (aPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                        (aPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION),
                        aErrorStack );
    DTL_PARAM_VALIDATE( ( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE) )
                        ||
                        ( ( aIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND ) &&
                          ( (aScale >= DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION) &&
                            (aScale <= DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION) ) ),
                        aErrorStack );

    /*
     * DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION (6)을 넘는지 체크.
     */ 
    STL_TRY_THROW( (aInteger >= DTD_INTERVAL_MAX_PRECISION_MIN_VALUE) &&
                   (aInteger <= DTD_INTERVAL_MAX_PRECISION_MAX_VALUE),
                   ERROR_OUT_OF_PRECISION );
    
    *aSuccessWithInfo = STL_FALSE;
    
    DTL_INIT_DTLEXPTIME( sDtlExpTime );
    
    switch( aIntervalIndicator )
    {
        case DTL_INTERVAL_INDICATOR_DAY:
            sDtlExpTime.mDay = aInteger;
            break;
        case DTL_INTERVAL_INDICATOR_HOUR:
            sDtlExpTime.mHour = aInteger;
            break;
        case DTL_INTERVAL_INDICATOR_MINUTE:
            sDtlExpTime.mMinute = aInteger;
            break;
        case DTL_INTERVAL_INDICATOR_SECOND:
            sDtlExpTime.mSecond = aInteger;
            break;
        default:
            STL_DASSERT( 1 == 0 );
            STL_THROW( RAMP_ERR_INTERVAL_INDICATOR_INVALID );
            break;
    }

    dtdDtlExpTime2IntervalDayToSecond( & sDtlExpTime,
                                       0, /* aFractionalSecond */
                                       aIntervalIndicator,
                                       & sIntervalDayToSecondType );

    STL_TRY( dtdAdjustIntervalDayToSecond( & sIntervalDayToSecondType,
                                           aIntervalIndicator,
                                           aPrecision,
                                           0, /* aFractionalSecondPrecision */
                                           aErrorStack )
             == STL_SUCCESS );
    
    ((dtlIntervalDayToSecondType *)(aValue->mValue))->mIndicator
        = sIntervalDayToSecondType.mIndicator;
    ((dtlIntervalDayToSecondType *)(aValue->mValue))->mDay = sIntervalDayToSecondType.mDay;
    ((dtlIntervalDayToSecondType *)(aValue->mValue))->mTime = sIntervalDayToSecondType.mTime;

    aValue->mLength = DTL_INTERVAL_DAY_TO_SECOND_SIZE;

    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_PRECISION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_DT_INTERVAL_SOME_FILEDS_TRUNCATED_CONVERSION,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INTERVAL_INDICATOR_INVALID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INTERVAL_INDICATOR_INVALID,
                      NULL,
                      aErrorStack,
                      dtlDataTypeName[ aValue->mType ],
                      aIntervalIndicator,
                      gDataTypeInfoDomain[ aValue->mType ].mMinIntervalIndicator,
                      gDataTypeInfoDomain[ aValue->mType ].mMaxIntervalIndicator );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/* /\** */
/*  * @brief real value로부터 dtdIntervalDayToSecond 타입의 value 구성 */
/*  * @param[in]  aReal              value에 저장될 입력 숫자 */
/*  * @param[in]  aPrecision       */
/*  * @param[in]  aScale */
/*  * @param[in]  aStringUnit        string length unit (dtlStringLengthUnit 참조)  */
/*  * @param[in]  aIntervalIndicator INTERVAL 타입의 Indicator */
/*  *                           <BR> dtlIntervalIndicator 참조  */
/*  * @param[in]  aAvailableSize     mValue에 사용할 수 있는 메모리 공간의 크기   */
/*  * @param[out] aValue             dtlDataValue의 주소 */
/*  * @param[out] aSuccessWithInfo   warning 발생 여부 */
/*  * @param[in]  aVectorFunc        Function Vector */
/*  * @param[in]  aVectorArg         Vector Argument */
/*  * @param[out] aErrorStack        에러 스택 */
/*  *\/ */
/* stlStatus dtdIntervalDayToSecondSetValueFromReal( stlFloat64             aReal, */
/*                                                   stlInt64               aPrecision, */
/*                                                   stlInt64               aScale, */
/*                                                   dtlStringLengthUnit    aStringUnit, */
/*                                                   dtlIntervalIndicator   aIntervalIndicator, */
/*                                                   stlInt64               aAvailableSize, */
/*                                                   dtlDataValue         * aValue, */
/*                                                   stlBool              * aSuccessWithInfo, */
/*                                                   dtlFuncVector        * aVectorFunc, */
/*                                                   void                 * aVectorArg, */
/*                                                   stlErrorStack        * aErrorStack ) */
/* { */
/*     stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                   DTL_ERRCODE_CONVERSION_NOT_APPLICABLE, */
/*                   NULL, */
/*                   aErrorStack, */
/*                   "approximate numeric data types(FLOAT or DOUBLE)", */
/*                   "INTERVAL" ); */
    
/*     return STL_FAILURE; */
/* } */

/**
 * @brief Byte Order 변경 (redirect 함수 아님, 내부 함수)
 * @param[in]  aValue          byte swapping 대상 및 결과 
 * @param[in]  aIsSameEndian   byte swapping 필요 여부
 * @param[out] aErrorStack     에러 스택
 */
stlStatus dtdIntervalDayToSecondReverseByteOrder( void            * aValue,
                                                  stlBool           aIsSameEndian,
                                                  stlErrorStack   * aErrorStack )
{
    dtlIntervalDayToSecondType  * sIntervalDayToSecondType;

    DTL_PARAM_VALIDATE( aValue != NULL, aErrorStack );

    sIntervalDayToSecondType = (dtlIntervalDayToSecondType *)(aValue);

    if( aIsSameEndian == STL_FALSE )
    {
        dtdReverseByteOrder( (stlChar*) & sIntervalDayToSecondType->mIndicator,
                             STL_SIZEOF(sIntervalDayToSecondType->mIndicator),
                             (stlChar*) & sIntervalDayToSecondType->mIndicator );

        dtdReverseByteOrder( (stlChar*) & sIntervalDayToSecondType->mDay,
                             STL_SIZEOF(sIntervalDayToSecondType->mDay),
                             (stlChar*) & sIntervalDayToSecondType->mDay );
        
        dtdReverseByteOrder( (stlChar*) & sIntervalDayToSecondType->mTime,
                             STL_SIZEOF(sIntervalDayToSecondType->mTime),
                             (stlChar*) & sIntervalDayToSecondType->mTime );
    }
    else
    {
        // Do Nothing
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief dtdIntervalDayToSecond value를 leading precision / fractional second precision 자리수에 맞게 문자열로 변환
 * @param[in]  aValue                       dtlDataValue
 * @param[in]  aLeadingPrecision            leading field precision
 * @param[in]  aFractionalSecondPrecision   fractional second precision
 * @param[in]  aAvailableSize               aBuffer에 사용할 수 있는 메모리 공간의 크기
 * @param[out] aBuffer                      문자열로 변환한 결과가 저장될 메모리 공간
 * @param[out] aLength                      변환된 문자열의 길이
 * @param[in]  aVectorFunc                  Function Vector
 * @param[in]  aVectorArg                   Vector Argument
 * @param[out] aErrorStack                  에러 스택
 */
stlStatus dtdIntervalDayToSecondToFormatString( dtlDataValue    * aValue,
                                                stlInt64          aLeadingPrecision,
                                                stlInt64          aFractionalSecondPrecision,
                                                stlInt64          aAvailableSize,
                                                void            * aBuffer,
                                                stlInt64        * aLength,
                                                dtlFuncVector   * aVectorFunc,
                                                void            * aVectorArg,
                                                stlErrorStack   * aErrorStack )
{
    dtlIntervalDayToSecondType  sIntervalDayToSecondType;
    dtlExpTime                  sDtlExpTime;
    dtlFractionalSecond         sFractionalSecond;

    /*
     * dtdIntervalDayToSecondToFormatString() 호출시,
     * precision, scale 정보를 구할 수 없는 경우가 있어
     * DTL_PRECISION_NA, DTL_SCALE_NA 로 넘어올 수 있다.
     * 이런 경우, dtlIntervalDayToSecondToFormatString() 함수내에서
     * DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION, 
     * DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION 로 처리된다.
     */

    DTL_PARAM_VALIDATE( ( (aLeadingPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                          (aLeadingPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION) ) ||
                        ( aLeadingPrecision == DTL_PRECISION_NA ),
                        aErrorStack );
    DTL_PARAM_VALIDATE( ( (aFractionalSecondPrecision >= DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION) &&
                          (aFractionalSecondPrecision <= DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION) ) ||
                        ( aFractionalSecondPrecision == DTL_SCALE_NA ),
                        aErrorStack );
    DTL_CHECK_TYPE( DTL_TYPE_INTERVAL_DAY_TO_SECOND, aValue, aErrorStack );
    DTL_PARAM_VALIDATE( aAvailableSize >= 
                        DTL_INTERVAL_DAY_TO_SECOND_DISPLAY_SIZE( 
                            aLeadingPrecision, 
                            aFractionalSecondPrecision,
                            ((dtlIntervalDayToSecondType*)(aValue->mValue))->mIndicator ),
                        aErrorStack );
    DTL_PARAM_VALIDATE( aBuffer != NULL, aErrorStack );

    sIntervalDayToSecondType.mIndicator =
        ((dtlIntervalDayToSecondType*)(aValue->mValue))->mIndicator;
    sIntervalDayToSecondType.mDay =
        ((dtlIntervalDayToSecondType*)(aValue->mValue))->mDay;
    sIntervalDayToSecondType.mTime =
        ((dtlIntervalDayToSecondType*)(aValue->mValue))->mTime;

    DTL_INIT_DTLEXPTIME( sDtlExpTime );    

    dtdIntervalDayToSecond2DtlExpTime( sIntervalDayToSecondType,
                                       & sDtlExpTime,
                                       & sFractionalSecond );

    STL_TRY( dtlIntervalDayToSecondToFormatString( sIntervalDayToSecondType.mIndicator,
                                                   & sDtlExpTime,
                                                   sFractionalSecond,
                                                   aLeadingPrecision,
                                                   aFractionalSecondPrecision,
                                                   aBuffer,
                                                   aLength,
                                                   aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
}


/**
 * @brief dtdIntervalDayToSecond value를 문자열로 변환
 * @param[in]  aValue          dtlDataValue
 * @param[in]  aPrecision      precision
 * @param[in]  aScale          scale
 * @param[in]  aAvailableSize  aBuffer에 사용할 수 있는 메모리 공간의 크기
 * @param[out] aBuffer         문자열로 변환한 결과가 저장될 메모리 공간
 * @param[out] aLength         변환된 문자열의 길이
 * @param[in]  aVectorFunc     Function Vector
 * @param[in]  aVectorArg      Vector Argument
 * @param[out] aErrorStack     에러 스택
 */
stlStatus dtdIntervalDayToSecondToString( dtlDataValue    * aValue,
                                          stlInt64          aPrecision,
                                          stlInt64          aScale,
                                          stlInt64          aAvailableSize,
                                          void            * aBuffer,
                                          stlInt64        * aLength,
                                          dtlFuncVector   * aVectorFunc,
                                          void            * aVectorArg,
                                          stlErrorStack   * aErrorStack )
{
    STL_TRY( dtdIntervalDayToSecondToFormatString( aValue,
                                                   aPrecision,
                                                   aScale,
                                                   aAvailableSize,
                                                   aBuffer,
                                                   aLength,
                                                   aVectorFunc,
                                                   aVectorArg,
                                                   aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;    
}

/**
 * @brief dtdIntervalDayToSecond value를 사용가능한 버퍼사이즈에 맞게 잘라서 문자열로 변환한다.
 * @param[in]  aValue          dtlDataValue
 * @param[in]  aPrecision      precision
 * @param[in]  aScale          scale
 * @param[in]  aAvailableSize  aBuffer에 사용할 수 있는 메모리 공간의 크기
 * @param[out] aBuffer         문자열로 변환한 결과가 저장될 메모리 공간
 * @param[out] aLength         변환된 문자열의 길이
 * @param[in]  aVectorFunc     Function Vector
 * @param[in]  aVectorArg      Vector Argument
 * @param[out] aErrorStack     에러 스택
 */
stlStatus dtdIntervalDayToSecondToStringForAvailableSize( dtlDataValue    * aValue,
                                                          stlInt64          aPrecision,
                                                          stlInt64          aScale,
                                                          stlInt64          aAvailableSize,
                                                          void            * aBuffer,
                                                          stlInt64        * aLength,
                                                          dtlFuncVector   * aVectorFunc,
                                                          void            * aVectorArg,
                                                          stlErrorStack   * aErrorStack )
{
    stlInt64          sLength = 0;
    stlChar           sIntervalBuffer[DTL_INTERVAL_DAY_TO_SECOND_MAX_STRING_SIZE];
    
    DTL_CHECK_TYPE( DTL_TYPE_INTERVAL_DAY_TO_SECOND, aValue, aErrorStack );
    DTL_PARAM_VALIDATE( aBuffer != NULL, aErrorStack );

    if( aAvailableSize >= 
        DTL_INTERVAL_DAY_TO_SECOND_DISPLAY_SIZE(aPrecision,
                                                aScale,
                                                ((dtlIntervalDayToSecondType*)(aValue->mValue))->mIndicator) )
    {
        STL_TRY( dtdIntervalDayToSecondToString( aValue,
                                                 aPrecision,
                                                 aScale,
                                                 aAvailableSize,
                                                 aBuffer,
                                                 & sLength,
                                                 aVectorFunc,
                                                 aVectorArg,
                                                 aErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( dtdIntervalDayToSecondToString( aValue,
                                                 aPrecision,
                                                 aScale,
                                                 DTL_INTERVAL_DAY_TO_SECOND_MAX_STRING_SIZE,
                                                 sIntervalBuffer,
                                                 & sLength,
                                                 aVectorFunc,
                                                 aVectorArg,
                                                 aErrorStack )
                 == STL_SUCCESS );

        sLength = ( sLength >= aAvailableSize ) ? aAvailableSize : sLength;
        stlMemcpy( aBuffer, sIntervalBuffer, sLength );
    }
        
    *aLength = sLength;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
}


/**
 * @brief utf16 string value로부터 dtdIntervalDayToSecond 타입의 value 구성
 * @param[in]  aString            value에 저장될 입력 문자열
 * @param[in]  aStringLength      aString의 길이  
 * @param[in]  aPrecision         precision
 *                           <BR> interval type에서는 leading field precision 정보 
 * @param[in]  aScale             scale
 *                           <BR> interval type에서는 fractional second precision 정보
 * @param[in]  aStringUnit      string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aIntervalIndicator INTERVAL 타입의 Indicator
 *                           <BR> dtlIntervalIndicator 참조 
 * @param[in]  aAvailableSize     mValue에 사용할 수 있는 메모리 공간의 크기  
 * @param[out] aValue             dtlDataValue의 주소
 * @param[out] aSuccessWithInfo   warning 발생 여부
 * @param[in]  aVectorFunc        Function Vector
 * @param[in]  aVectorArg         Vector Argument
 * @param[out] aErrorStack        에러 스택
 *
 * @remark
 * DAY, HOUR, MINUTE, SECOND,
 * DAY TO HOUR, DAY TO MINUTE, DAY TO SECOND,
 * HOUR TO MINUTE, HOUR TO SECOND,
 * MINUTE TO SECOND interval type에 string으로 입력 가능한 포멧은 다음과 같다.
 * @code
 *
 * <unquoted interval string> ::=
 *      [ <sign> ] { <year-month literal> | <day-time literal> }
 * 
 * <day-time literal> ::=
 *      <day-time interval>
 *      | <time interval>
 * 
 * <day-time interval> ::=
 *      <days value> [ <space> <hours value> [ <colon> <minutes value> [ <colon> <seconds value> ] ] ]
 * 
 * <time interval> ::=
 *      <hours value> [ <colon> <minutes value> [ <colon> <seconds value> ] ]
 *      | <minutes value> [ <colon> <seconds value> ]
 *      | <seconds value>
 * 
 *  (1) day hour:minute:second.fractionalsecond
 *      '3 10:11:12.123456' ( 3 day 10 hour 11 minute 12.123456 second )
 *  (2) time
 *      '10:11:12.123456'
 *  (3) number
 *      number는 single field에만 해당됨.
 *      ( INTERVAL DAY, INTERVAL HOUR, INTERVAL MINUTE, INTERVAL SECOND )
 *      예1) INTERVAL DAY에 '3' 입력시,
 *          '3'을 DAY의 값으로 인식해서 +3 00:00:00으로 데이타가 들어감.
 *  (4) time zone시간대
 *      '+09:00'
 * @endcode
 */
stlStatus dtdIntervalDayToSecondSetValueFromUtf16WCharString( void                 * aString,
                                                              stlInt64               aStringLength,
                                                              stlInt64               aPrecision,
                                                              stlInt64               aScale,
                                                              dtlStringLengthUnit    aStringUnit,
                                                              dtlIntervalIndicator   aIntervalIndicator,
                                                              stlInt64               aAvailableSize,
                                                              dtlDataValue         * aValue,
                                                              stlBool              * aSuccessWithInfo,
                                                              dtlFuncVector        * aVectorFunc,
                                                              void                 * aVectorArg,
                                                              stlErrorStack        * aErrorStack )
{
    dtlIntervalDayToSecondType  sIntervalDayToSecondType;
    dtlIntervalDayToSecondType  sTempIntervalDayToSecondType;
    dtlExpTime                  sDtlExpTime;
    dtlFractionalSecond         sFractionalSecond;
    stlChar                     sBuffer[256];
    stlChar                   * sField[DTL_MAX_DATE_FIELDS];
    stlInt32                    sFieldType[DTL_MAX_DATE_FIELDS];
    stlInt32                    sNumFields;
    stlInt32                    sDateType;

    DTL_PARAM_VALIDATE( (aPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                        (aPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION),
                        aErrorStack );
    DTL_PARAM_VALIDATE( ( (aScale >= DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION) &&
                          (aScale <= DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION) ) ||
                        ( aScale == DTL_SCALE_NA ),
                        aErrorStack );

    DTL_PARAM_VALIDATE( ( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_HOUR) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_SECOND) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND) )
                        == STL_TRUE,
                        aErrorStack );

    DTL_PARAM_VALIDATE( ( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_HOUR) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE) )
                        ||
                        ( ( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND) ||
                            (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_SECOND) ||
                            (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND) ||
                            (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND) ) &&
                          ( (aScale >= DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION) &&
                            (aScale <= DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION) ) ),
                        aErrorStack );

    DTL_PARAM_VALIDATE( aAvailableSize >= DTL_INTERVAL_DAY_TO_SECOND_SIZE, aErrorStack );
    DTL_CHECK_TYPE( DTL_TYPE_INTERVAL_DAY_TO_SECOND, aValue, aErrorStack );

    DTL_PARAM_VALIDATE( aString != NULL, aErrorStack );

    *aSuccessWithInfo = STL_FALSE;
    
    sIntervalDayToSecondType.mIndicator = aIntervalIndicator;

    DTL_INIT_DTLEXPTIME( sDtlExpTime );

    /*
     * INTERVAL의 aPrecision, aScale은 다음과 같은 용도로 사용된다.
     * aPrecision => interval leading field precision
     * aScale     => interval fractional second precision
     * 예)interval year[(year precision = aPrecision)] to month
     * 예)interval day[(day precision = aPrecision)] to second[(fraction_second_precision = aScale)]
     */

    STL_TRY( dtdParseDateTimeFromUtf16WChar( (stlUInt16 *)aString,
                                             aStringLength,
                                             DTL_TYPE_INTERVAL_DAY_TO_SECOND,
                                             sBuffer,
                                             STL_SIZEOF(sBuffer),
                                             sField,
                                             sFieldType,
                                             DTL_MAX_DATE_FIELDS,
                                             & sNumFields,
                                             aErrorStack )
             == STL_SUCCESS );

    /*
     * input string에 대한 체크
     * DAY, HOUR, MINUTE, SECOND,
     * DAY TO HOUR, DAY TO MINUTE, DAY TO SECOND,
     * HOUR TO MINUTE, HOUR TO SECOND,
     * MINUTE TO SECOND interval type이외의 불필요한 정보가 있는 경우 에러처리한다.
     * 예) '3 11:22:33.123456' ( O )
     *     '1-2 3 11:22:33.12345' ( X )
     */

    /*
     * DAY, HOUR, MINUTE, SECOND,
     * DAY TO HOUR, DAY TO MINUTE, DAY TO SECOND,
     * HOUR TO MINUTE, HOUR TO SECOND,
     * MINUTE TO SECOND interval type에 string으로 입력 가능한 포멧은 다음과 같다.
     *
     * <unquoted interval string> ::=
     *      [ <sign> ] { <year-month literal> | <day-time literal> }
     * 
     * <day-time literal> ::=
     *      <day-time interval>
     *      | <time interval>
     * 
     * <day-time interval> ::=
     *      <days value> [ <space> <hours value> [ <colon> <minutes value> [ <colon> <seconds value> ] ] ]
     * 
     * <time interval> ::=
     *      <hours value> [ <colon> <minutes value> [ <colon> <seconds value> ] ]
     *      | <minutes value> [ <colon> <seconds value> ]
     *      | <seconds value>
     * 
     *  (1) day hour:minute:second.fractionalsecond
     *      '3 10:11:12.123456' ( 3 day 10 hour 11 minute 12.123456 second )
     *  (2) time
     *      '10:11:12.123456'
     *  (3) number
     *      number는 single field에만 해당됨.
     *      ( INTERVAL DAY, INTERVAL HOUR, INTERVAL MINUTE, INTERVAL SECOND )
     *      예1) INTERVAL DAY에 '3' 입력시,
     *          '3'을 DAY의 값으로 인식해서 +3 00:00:00으로 데이타가 들어감.
     *  (4) time zone시간대
     *      '+09:00'
     */
    switch( sNumFields )
    {
        case 2:
            {
                STL_TRY_THROW( (sFieldType[1] == DTL_DTK_TIME) ||
                               (sFieldType[1] == DTL_DTK_NUMBER),
                               RAMP_ERROR_BAD_FORMAT );
                
                STL_TRY_THROW( (sFieldType[0] == DTL_DTK_NUMBER) ||
                               (sFieldType[0] == DTL_DTK_TZ),
                               RAMP_ERROR_BAD_FORMAT );

                break;
            }
        case 1:
            {
                STL_TRY_THROW( (sFieldType[0] == DTL_DTK_TIME) ||
                               (sFieldType[0] == DTL_DTK_NUMBER) ||
                               (sFieldType[0] == DTL_DTK_TZ),
                               RAMP_ERROR_BAD_FORMAT );

                if( sFieldType[0] == DTL_DTK_TZ )
                {
                    if( stlStrchr((sField[0] + 1), ':') != NULL )
                    {
                        // Do Nothing
                    }
                    else
                    {
                        sFieldType[0] = DTL_DTK_NUMBER;
                    }
                }
                else
                {
                    // Do Nothing
                }
                
                if( sFieldType[0] == DTL_DTK_NUMBER )
                {
                    if( stlStrchr((sField[0] + 1), '.') == NULL )
                    {
                        STL_TRY_THROW( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY)    ||
                                       (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR)   ||
                                       (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE) ||
                                       (aIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND),
                                       RAMP_ERROR_BAD_FORMAT );
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }
                else
                {
                    // Do Nothing
                }
                
                break;
            }
        default:
            {
                STL_THROW( RAMP_ERROR_BAD_FORMAT );
                break;
            }
    }
    
    STL_TRY( dtdDecodeInterval( sField,
                                sFieldType,
                                sNumFields,
                                aIntervalIndicator,
                                & sDateType,
                                & sDtlExpTime,
                                & sFractionalSecond,
                                aErrorStack )
             == STL_SUCCESS );

    /*
     * value truncated check
     */
    STL_TRY( dtdIntervalValueTruncatedCheck( & sDtlExpTime,
                                             aIntervalIndicator,
                                             & sFractionalSecond,
                                             aErrorStack )
             == STL_SUCCESS );
    
    switch( sDateType )
    {
        case DTL_DTK_DELTA:
            {
                dtdDtlExpTime2IntervalDayToSecond( & sDtlExpTime,
                                                   sFractionalSecond,
                                                   aIntervalIndicator,
                                                   & sIntervalDayToSecondType );
            }
            break;
        default:
            STL_THROW( RAMP_ERROR_BAD_FORMAT );
            break;
    }

    sTempIntervalDayToSecondType.mIndicator = sIntervalDayToSecondType.mIndicator;
    sTempIntervalDayToSecondType.mDay       = sIntervalDayToSecondType.mDay;
    sTempIntervalDayToSecondType.mTime      = sIntervalDayToSecondType.mTime;
    
    STL_TRY( dtdAdjustIntervalDayToSecond( & sIntervalDayToSecondType,
                                           aIntervalIndicator,
                                           aPrecision,
                                           aScale,
                                           aErrorStack )
             == STL_SUCCESS );    
    
    (((dtlIntervalDayToSecondType *)(aValue->mValue))->mIndicator) =
        sIntervalDayToSecondType.mIndicator;

    ((dtlIntervalDayToSecondType *)(aValue->mValue))->mDay = sIntervalDayToSecondType.mDay;
    ((dtlIntervalDayToSecondType *)(aValue->mValue))->mTime = sIntervalDayToSecondType.mTime;

    aValue->mLength = DTL_INTERVAL_DAY_TO_SECOND_SIZE;

    if( sIntervalDayToSecondType.mTime != sTempIntervalDayToSecondType.mTime )
    {
        *aSuccessWithInfo = STL_TRUE;
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

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief utf32 string value로부터 dtdIntervalDayToSecond 타입의 value 구성
 * @param[in]  aString            value에 저장될 입력 문자열
 * @param[in]  aStringLength      aString의 길이  
 * @param[in]  aPrecision         precision
 *                           <BR> interval type에서는 leading field precision 정보 
 * @param[in]  aScale             scale
 *                           <BR> interval type에서는 fractional second precision 정보
 * @param[in]  aStringUnit      string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aIntervalIndicator INTERVAL 타입의 Indicator
 *                           <BR> dtlIntervalIndicator 참조 
 * @param[in]  aAvailableSize     mValue에 사용할 수 있는 메모리 공간의 크기  
 * @param[out] aValue             dtlDataValue의 주소
 * @param[out] aSuccessWithInfo   warning 발생 여부
 * @param[in]  aVectorFunc        Function Vector
 * @param[in]  aVectorArg         Vector Argument
 * @param[out] aErrorStack        에러 스택
 *
 * @remark
 * DAY, HOUR, MINUTE, SECOND,
 * DAY TO HOUR, DAY TO MINUTE, DAY TO SECOND,
 * HOUR TO MINUTE, HOUR TO SECOND,
 * MINUTE TO SECOND interval type에 string으로 입력 가능한 포멧은 다음과 같다.
 * @code
 *
 * <unquoted interval string> ::=
 *      [ <sign> ] { <year-month literal> | <day-time literal> }
 * 
 * <day-time literal> ::=
 *      <day-time interval>
 *      | <time interval>
 * 
 * <day-time interval> ::=
 *      <days value> [ <space> <hours value> [ <colon> <minutes value> [ <colon> <seconds value> ] ] ]
 * 
 * <time interval> ::=
 *      <hours value> [ <colon> <minutes value> [ <colon> <seconds value> ] ]
 *      | <minutes value> [ <colon> <seconds value> ]
 *      | <seconds value>
 * 
 *  (1) day hour:minute:second.fractionalsecond
 *      '3 10:11:12.123456' ( 3 day 10 hour 11 minute 12.123456 second )
 *  (2) time
 *      '10:11:12.123456'
 *  (3) number
 *      number는 single field에만 해당됨.
 *      ( INTERVAL DAY, INTERVAL HOUR, INTERVAL MINUTE, INTERVAL SECOND )
 *      예1) INTERVAL DAY에 '3' 입력시,
 *          '3'을 DAY의 값으로 인식해서 +3 00:00:00으로 데이타가 들어감.
 *  (4) time zone시간대
 *      '+09:00'
 * @endcode
 */
stlStatus dtdIntervalDayToSecondSetValueFromUtf32WCharString( void                 * aString,
                                                              stlInt64               aStringLength,
                                                              stlInt64               aPrecision,
                                                              stlInt64               aScale,
                                                              dtlStringLengthUnit    aStringUnit,
                                                              dtlIntervalIndicator   aIntervalIndicator,
                                                              stlInt64               aAvailableSize,
                                                              dtlDataValue         * aValue,
                                                              stlBool              * aSuccessWithInfo,
                                                              dtlFuncVector        * aVectorFunc,
                                                              void                 * aVectorArg,
                                                              stlErrorStack        * aErrorStack )
{
    dtlIntervalDayToSecondType  sIntervalDayToSecondType;
    dtlIntervalDayToSecondType  sTempIntervalDayToSecondType;
    dtlExpTime                  sDtlExpTime;
    dtlFractionalSecond         sFractionalSecond;
    stlChar                     sBuffer[256];
    stlChar                   * sField[DTL_MAX_DATE_FIELDS];
    stlInt32                    sFieldType[DTL_MAX_DATE_FIELDS];
    stlInt32                    sNumFields;
    stlInt32                    sDateType;

    DTL_PARAM_VALIDATE( (aPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                        (aPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION),
                        aErrorStack );
    DTL_PARAM_VALIDATE( ( (aScale >= DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION) &&
                          (aScale <= DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION) ) ||
                        ( aScale == DTL_SCALE_NA ),
                        aErrorStack );

    DTL_PARAM_VALIDATE( ( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_HOUR) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_SECOND) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND) )
                        == STL_TRUE,
                        aErrorStack );

    DTL_PARAM_VALIDATE( ( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_HOUR) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE) )
                        ||
                        ( ( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND) ||
                            (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_SECOND) ||
                            (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND) ||
                            (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND) ) &&
                          ( (aScale >= DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION) &&
                            (aScale <= DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION) ) ),
                        aErrorStack );

    DTL_PARAM_VALIDATE( aAvailableSize >= DTL_INTERVAL_DAY_TO_SECOND_SIZE, aErrorStack );
    DTL_CHECK_TYPE( DTL_TYPE_INTERVAL_DAY_TO_SECOND, aValue, aErrorStack );

    DTL_PARAM_VALIDATE( aString != NULL, aErrorStack );

    *aSuccessWithInfo = STL_FALSE;
    
    sIntervalDayToSecondType.mIndicator = aIntervalIndicator;

    DTL_INIT_DTLEXPTIME( sDtlExpTime );

    /*
     * INTERVAL의 aPrecision, aScale은 다음과 같은 용도로 사용된다.
     * aPrecision => interval leading field precision
     * aScale     => interval fractional second precision
     * 예)interval year[(year precision = aPrecision)] to month
     * 예)interval day[(day precision = aPrecision)] to second[(fraction_second_precision = aScale)]
     */

    STL_TRY( dtdParseDateTimeFromUtf32WChar( (stlUInt32 *)aString,
                                             aStringLength,
                                             DTL_TYPE_INTERVAL_DAY_TO_SECOND,
                                             sBuffer,
                                             STL_SIZEOF(sBuffer),
                                             sField,
                                             sFieldType,
                                             DTL_MAX_DATE_FIELDS,
                                             & sNumFields,
                                             aErrorStack )
             == STL_SUCCESS );

    /*
     * input string에 대한 체크
     * DAY, HOUR, MINUTE, SECOND,
     * DAY TO HOUR, DAY TO MINUTE, DAY TO SECOND,
     * HOUR TO MINUTE, HOUR TO SECOND,
     * MINUTE TO SECOND interval type이외의 불필요한 정보가 있는 경우 에러처리한다.
     * 예) '3 11:22:33.123456' ( O )
     *     '1-2 3 11:22:33.12345' ( X )
     */

    /*
     * DAY, HOUR, MINUTE, SECOND,
     * DAY TO HOUR, DAY TO MINUTE, DAY TO SECOND,
     * HOUR TO MINUTE, HOUR TO SECOND,
     * MINUTE TO SECOND interval type에 string으로 입력 가능한 포멧은 다음과 같다.
     *
     * <unquoted interval string> ::=
     *      [ <sign> ] { <year-month literal> | <day-time literal> }
     * 
     * <day-time literal> ::=
     *      <day-time interval>
     *      | <time interval>
     * 
     * <day-time interval> ::=
     *      <days value> [ <space> <hours value> [ <colon> <minutes value> [ <colon> <seconds value> ] ] ]
     * 
     * <time interval> ::=
     *      <hours value> [ <colon> <minutes value> [ <colon> <seconds value> ] ]
     *      | <minutes value> [ <colon> <seconds value> ]
     *      | <seconds value>
     * 
     *  (1) day hour:minute:second.fractionalsecond
     *      '3 10:11:12.123456' ( 3 day 10 hour 11 minute 12.123456 second )
     *  (2) time
     *      '10:11:12.123456'
     *  (3) number
     *      number는 single field에만 해당됨.
     *      ( INTERVAL DAY, INTERVAL HOUR, INTERVAL MINUTE, INTERVAL SECOND )
     *      예1) INTERVAL DAY에 '3' 입력시,
     *          '3'을 DAY의 값으로 인식해서 +3 00:00:00으로 데이타가 들어감.
     *  (4) time zone시간대
     *      '+09:00'
     */
    switch( sNumFields )
    {
        case 2:
            {
                STL_TRY_THROW( (sFieldType[1] == DTL_DTK_TIME) ||
                               (sFieldType[1] == DTL_DTK_NUMBER),
                               RAMP_ERROR_BAD_FORMAT );
                
                STL_TRY_THROW( (sFieldType[0] == DTL_DTK_NUMBER) ||
                               (sFieldType[0] == DTL_DTK_TZ),
                               RAMP_ERROR_BAD_FORMAT );

                break;
            }
        case 1:
            {
                STL_TRY_THROW( (sFieldType[0] == DTL_DTK_TIME) ||
                               (sFieldType[0] == DTL_DTK_NUMBER) ||
                               (sFieldType[0] == DTL_DTK_TZ),
                               RAMP_ERROR_BAD_FORMAT );

                if( sFieldType[0] == DTL_DTK_TZ )
                {
                    if( stlStrchr((sField[0] + 1), ':') != NULL )
                    {
                        // Do Nothing
                    }
                    else
                    {
                        sFieldType[0] = DTL_DTK_NUMBER;
                    }
                }
                else
                {
                    // Do Nothing
                }
                
                if( sFieldType[0] == DTL_DTK_NUMBER )
                {
                    if( stlStrchr((sField[0] + 1), '.') == NULL )
                    {
                        STL_TRY_THROW( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY)    ||
                                       (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR)   ||
                                       (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE) ||
                                       (aIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND),
                                       RAMP_ERROR_BAD_FORMAT );
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }
                else
                {
                    // Do Nothing
                }
                
                break;
            }
        default:
            {
                STL_THROW( RAMP_ERROR_BAD_FORMAT );
                break;
            }
    }
    
    STL_TRY( dtdDecodeInterval( sField,
                                sFieldType,
                                sNumFields,
                                aIntervalIndicator,
                                & sDateType,
                                & sDtlExpTime,
                                & sFractionalSecond,
                                aErrorStack )
             == STL_SUCCESS );

    /*
     * value truncated check
     */
    STL_TRY( dtdIntervalValueTruncatedCheck( & sDtlExpTime,
                                             aIntervalIndicator,
                                             & sFractionalSecond,
                                             aErrorStack )
             == STL_SUCCESS );
    
    switch( sDateType )
    {
        case DTL_DTK_DELTA:
            {
                dtdDtlExpTime2IntervalDayToSecond( & sDtlExpTime,
                                                   sFractionalSecond,
                                                   aIntervalIndicator,
                                                   & sIntervalDayToSecondType );
            }
            break;
        default:
            STL_THROW( RAMP_ERROR_BAD_FORMAT );
            break;
    }

    sTempIntervalDayToSecondType.mIndicator = sIntervalDayToSecondType.mIndicator;
    sTempIntervalDayToSecondType.mDay       = sIntervalDayToSecondType.mDay;
    sTempIntervalDayToSecondType.mTime      = sIntervalDayToSecondType.mTime;
    
    STL_TRY( dtdAdjustIntervalDayToSecond( & sIntervalDayToSecondType,
                                           aIntervalIndicator,
                                           aPrecision,
                                           aScale,
                                           aErrorStack )
             == STL_SUCCESS );    
    
    (((dtlIntervalDayToSecondType *)(aValue->mValue))->mIndicator) =
        sIntervalDayToSecondType.mIndicator;

    ((dtlIntervalDayToSecondType *)(aValue->mValue))->mDay = sIntervalDayToSecondType.mDay;
    ((dtlIntervalDayToSecondType *)(aValue->mValue))->mTime = sIntervalDayToSecondType.mTime;

    aValue->mLength = DTL_INTERVAL_DAY_TO_SECOND_SIZE;

    if( sIntervalDayToSecondType.mTime != sTempIntervalDayToSecondType.mTime )
    {
        *aSuccessWithInfo = STL_TRUE;
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

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief dtdIntervalDayToSecond value를 utf16 문자열로 변환
 * @param[in]  aValue          dtlDataValue
 * @param[in]  aPrecision      precision
 * @param[in]  aScale          scale
 * @param[in]  aAvailableSize  aBuffer에 사용할 수 있는 메모리 공간의 크기
 * @param[out] aBuffer         문자열로 변환한 결과가 저장될 메모리 공간
 * @param[out] aLength         변환된 문자열의 길이
 * @param[in]  aVectorFunc     Function Vector
 * @param[in]  aVectorArg      Vector Argument
 * @param[out] aErrorStack     에러 스택
 */
stlStatus dtdIntervalDayToSecondToUtf16WCharString( dtlDataValue    * aValue,
                                                    stlInt64          aPrecision,
                                                    stlInt64          aScale,
                                                    stlInt64          aAvailableSize,
                                                    void            * aBuffer,
                                                    stlInt64        * aLength,
                                                    dtlFuncVector   * aVectorFunc,
                                                    void            * aVectorArg,
                                                    stlErrorStack   * aErrorStack )
{
    dtlIntervalDayToSecondType  sIntervalDayToSecondType;
    dtlExpTime                  sDtlExpTime;
    dtlFractionalSecond         sFractionalSecond;

    /* /\* */
    /*  * dtdIntervalDayToSecondToFormatString() 호출시, */
    /*  * precision, scale 정보를 구할 수 없는 경우가 있어 */
    /*  * DTL_PRECISION_NA, DTL_SCALE_NA 로 넘어올 수 있다. */
    /*  * 이런 경우, dtlIntervalDayToSecondToFormatString() 함수내에서 */
    /*  * DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION,  */
    /*  * DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION 로 처리된다. */
    /*  *\/ */

    /* DTL_PARAM_VALIDATE( ( (aPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) && */
    /*                       (aPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION) ) || */
    /*                     ( aPrecision == DTL_PRECISION_NA ), */
    /*                     aErrorStack ); */
    /* DTL_PARAM_VALIDATE( ( (aScale >= DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION) && */
    /*                       (aScale <= DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION) ) || */
    /*                     ( aScale == DTL_SCALE_NA ), */
    /*                     aErrorStack ); */

    DTL_PARAM_VALIDATE( (aPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                        (aPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION),
                        aErrorStack );
    DTL_PARAM_VALIDATE( (aScale >= DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION) &&
                        (aScale <= DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION),
                        aErrorStack );

    DTL_CHECK_TYPE( DTL_TYPE_INTERVAL_DAY_TO_SECOND, aValue, aErrorStack );
    DTL_PARAM_VALIDATE( aAvailableSize >= 
                        ( DTL_INTERVAL_DAY_TO_SECOND_DISPLAY_SIZE( 
                            aPrecision, 
                            aScale,
                            ((dtlIntervalDayToSecondType*)(aValue->mValue))->mIndicator )
                          * STL_UINT16_SIZE ),
                        aErrorStack );
    DTL_PARAM_VALIDATE( aBuffer != NULL, aErrorStack );

    sIntervalDayToSecondType.mIndicator =
        ((dtlIntervalDayToSecondType*)(aValue->mValue))->mIndicator;
    sIntervalDayToSecondType.mDay =
        ((dtlIntervalDayToSecondType*)(aValue->mValue))->mDay;
    sIntervalDayToSecondType.mTime =
        ((dtlIntervalDayToSecondType*)(aValue->mValue))->mTime;

    DTL_INIT_DTLEXPTIME( sDtlExpTime );    

    dtdIntervalDayToSecond2DtlExpTime( sIntervalDayToSecondType,
                                       & sDtlExpTime,
                                       & sFractionalSecond );

    STL_TRY( dtlIntervalDayToSecondToFormatUtf16String( sIntervalDayToSecondType.mIndicator,
                                                        & sDtlExpTime,
                                                        sFractionalSecond,
                                                        aPrecision,
                                                        aScale,
                                                        aBuffer,
                                                        aLength,
                                                        aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;    
}


/**
 * @brief dtdIntervalDayToSecond value를 utf32 문자열로 변환
 * @param[in]  aValue          dtlDataValue
 * @param[in]  aPrecision      precision
 * @param[in]  aScale          scale
 * @param[in]  aAvailableSize  aBuffer에 사용할 수 있는 메모리 공간의 크기
 * @param[out] aBuffer         문자열로 변환한 결과가 저장될 메모리 공간
 * @param[out] aLength         변환된 문자열의 길이
 * @param[in]  aVectorFunc     Function Vector
 * @param[in]  aVectorArg      Vector Argument
 * @param[out] aErrorStack     에러 스택
 */
stlStatus dtdIntervalDayToSecondToUtf32WCharString( dtlDataValue    * aValue,
                                                    stlInt64          aPrecision,
                                                    stlInt64          aScale,
                                                    stlInt64          aAvailableSize,
                                                    void            * aBuffer,
                                                    stlInt64        * aLength,
                                                    dtlFuncVector   * aVectorFunc,
                                                    void            * aVectorArg,
                                                    stlErrorStack   * aErrorStack )
{
    dtlIntervalDayToSecondType  sIntervalDayToSecondType;
    dtlExpTime                  sDtlExpTime;
    dtlFractionalSecond         sFractionalSecond;

    /* /\* */
    /*  * dtdIntervalDayToSecondToFormatString() 호출시, */
    /*  * precision, scale 정보를 구할 수 없는 경우가 있어 */
    /*  * DTL_PRECISION_NA, DTL_SCALE_NA 로 넘어올 수 있다. */
    /*  * 이런 경우, dtlIntervalDayToSecondToFormatString() 함수내에서 */
    /*  * DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION,  */
    /*  * DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION 로 처리된다. */
    /*  *\/ */

    /* DTL_PARAM_VALIDATE( ( (aPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) && */
    /*                       (aPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION) ) || */
    /*                     ( aPrecision == DTL_PRECISION_NA ), */
    /*                     aErrorStack ); */
    /* DTL_PARAM_VALIDATE( ( (aScale >= DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION) && */
    /*                       (aScale <= DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION) ) || */
    /*                     ( aScale == DTL_SCALE_NA ), */
    /*                     aErrorStack ); */

    DTL_PARAM_VALIDATE( (aPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                        (aPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION),
                        aErrorStack );
    DTL_PARAM_VALIDATE( (aScale >= DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION) &&
                        (aScale <= DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION),
                        aErrorStack );

    DTL_CHECK_TYPE( DTL_TYPE_INTERVAL_DAY_TO_SECOND, aValue, aErrorStack );
    DTL_PARAM_VALIDATE( aAvailableSize >= 
                        ( DTL_INTERVAL_DAY_TO_SECOND_DISPLAY_SIZE( 
                            aPrecision, 
                            aScale,
                            ((dtlIntervalDayToSecondType*)(aValue->mValue))->mIndicator )
                          * STL_UINT32_SIZE ),
                        aErrorStack );
    DTL_PARAM_VALIDATE( aBuffer != NULL, aErrorStack );

    sIntervalDayToSecondType.mIndicator =
        ((dtlIntervalDayToSecondType*)(aValue->mValue))->mIndicator;
    sIntervalDayToSecondType.mDay =
        ((dtlIntervalDayToSecondType*)(aValue->mValue))->mDay;
    sIntervalDayToSecondType.mTime =
        ((dtlIntervalDayToSecondType*)(aValue->mValue))->mTime;

    DTL_INIT_DTLEXPTIME( sDtlExpTime );    

    dtdIntervalDayToSecond2DtlExpTime( sIntervalDayToSecondType,
                                       & sDtlExpTime,
                                       & sFractionalSecond );

    STL_TRY( dtlIntervalDayToSecondToFormatUtf32String( sIntervalDayToSecondType.mIndicator,
                                                        & sDtlExpTime,
                                                        sFractionalSecond,
                                                        aPrecision,
                                                        aScale,
                                                        aBuffer,
                                                        aLength,
                                                        aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;    
}


/** @} */


