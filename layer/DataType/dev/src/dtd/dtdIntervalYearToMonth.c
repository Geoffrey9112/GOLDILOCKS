/*******************************************************************************
 * dtdIntervalYearToMonth.c
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
 * @file dtdIntervalYearToMonth.c
 * @brief DataType Layer dtdIntervalYearToMonth 정의
 */

#include <dtl.h>

#include <dtdDef.h>
#include <dtdDataType.h>

/**
 * @addtogroup dtdIntervalYearToMonth
 * @{
 */

/**
 * @brief dtdIntervalYearToMonth 타입의 length값을 얻음
 * @param[in]  aPrecision
 * @param[in]  aStringUnit string length unit (dtlStringLengthUnit 참조) 
 * @param[out] aLength     dtdIntervalYearToMonth의 길이 값
 * @param[in]  aVectorFunc Function Vector
 * @param[in]  aVectorArg  Vector Argument
 * @param[out] aErrorStack 에러 스택
 */
stlStatus dtdIntervalYearToMonthGetLength( stlInt64              aPrecision,
                                           dtlStringLengthUnit   aStringUnit,
                                           stlInt64            * aLength,
                                           dtlFuncVector       * aVectorFunc,
                                           void                * aVectorArg,
                                           stlErrorStack       * aErrorStack )
{
    /* DTL_PARAM_VALIDATE( (aPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) && */
    /*                     (aPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION), */
    /*                     aErrorStack ); */

    *aLength = DTL_INTERVAL_YEAR_TO_MONTH_SIZE;
    
    return STL_SUCCESS;

    /* STL_FINISH; */

    /* return STL_FAILURE; */
}


/**
 * @brief dtdIntervalYearToMonth 타입의 length값을 얻음 (입력 문자열에 영향을 받지 않음)
 * @param[in]  aPrecision  
 * @param[in]  aScale
 * @param[in]  aStringUnit   string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aString       입력 문자열
 * @param[in]  aStringLength aString의 길이    
 * @param[out] aLength       dtdIntervalYearToMonth의 길이 값
 * @param[in]  aVectorFunc   Function Vector
 * @param[in]  aVectorArg    Vector Argument
 * @param[out] aErrorStack   에러 스택
 */
stlStatus dtdIntervalYearToMonthGetLengthFromString( stlInt64              aPrecision,
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

    *aLength = DTL_INTERVAL_YEAR_TO_MONTH_SIZE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief dtdIntervalYearToMonth 타입의 length값을 얻음 (입력 숫자에 영향을 받지 않음)
 * @param[in]  aPrecision  
 * @param[in]  aScale      
 * @param[in]  aInteger    입력 숫자
 * @param[out] aLength     dtdIntervalYearToMonth의 길이 값
 * @param[out] aErrorStack 에러 스택
 */
stlStatus dtdIntervalYearToMonthGetLengthFromInteger( stlInt64          aPrecision,
                                                      stlInt64          aScale,
                                                      stlInt64          aInteger,
                                                      stlInt64        * aLength,
                                                      stlErrorStack   * aErrorStack )
{
    DTL_PARAM_VALIDATE( (aPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                        (aPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION),
                        aErrorStack );

    *aLength = DTL_INTERVAL_YEAR_TO_MONTH_SIZE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/* /\** */
/*  * @brief dtdIntervalYearToMonth 타입의 length값을 얻음 (입력 숫자에 영향을 받지 않음) */
/*  * @param[in]  aPrecision   */
/*  * @param[in]  aScale       */
/*  * @param[in]  aReal       입력 숫자 */
/*  * @param[out] aLength     Date의 길이 값 */
/*  * @param[out] aErrorStack 에러 스택 */
/*  *\/ */
/* stlStatus dtdIntervalYearToMonthGetLengthFromReal( stlInt64          aPrecision, */
/*                                                    stlInt64          aScale, */
/*                                                    stlFloat64        aReal, */
/*                                                    stlInt64        * aLength, */
/*                                                    stlErrorStack   * aErrorStack ) */
/* { */
/*     stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                   DTL_ERRCODE_NOT_IMPLEMENTED, */
/*                   NULL, */
/*                   aErrorStack, */
/*                   "dtdIntervalYearToMonthGetLengthFromReal()" ); */

/*     return STL_FAILURE; */
/* } */

/**
 * @brief string value로부터 dtdIntervalYearToMonth 타입의 value 구성
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
 * YEAR, MONTH, YEAR TO MONTH interval type에 string으로 입력 가능한 포멧은 다음과 같다.
 * @code
 *
 * <unquoted interval string> ::=
 *        [ <sign> ] { <year-month literal> | <day-time literal> }
 *
 * <year-month literal> ::=
 *        <years value> [ <minus sign> <months value> ]
 *        | <months value>
 *        
 *  (1) year-month
 *      '1-2'  =>  ( 1 year 2 month )
 *     예) YEAR  => '1-0' ( O ), '+1-0' ( O ), '-1-1' ( X ),  '1-9' ( X )
 *     예) MONTH => '1-0' ( O ), '0-12' ( X ), '1-1' ( O ), '+1-1' ( O ), '-1-1' ( O )
 *     예) YEAR TO MONTH => '1-0' ( O ), '0-12' ( X ), '1-1' ( O ), '+1-1' ( O ), '-1-1' ( O )
 *  (2) number
 *      number는 single field에만 해당됨. ( INTERVAL YEAR, INTERVAL MONTH )
 *     예1) INTERVAL YEAR에 '3' 입력시,
 *          '3'을 YEAR의 값으로 인식해 +3-00으로 데이타가 들어감.
 * @endcode
 */
stlStatus dtdIntervalYearToMonthSetValueFromString( stlChar              * aString,
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
    dtlIntervalYearToMonthType   sIntervalYearToMonthType;
    dtlExpTime                   sDtlExpTime;
    dtlFractionalSecond          sFractionalSecond;
    stlChar                      sBuffer[256];
    stlChar                    * sField[DTL_MAX_DATE_FIELDS];
    stlInt32                     sFieldType[DTL_MAX_DATE_FIELDS];
    stlInt32                     sNumFields;
    stlInt32                     sDateType;

    DTL_PARAM_VALIDATE( (aPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                        (aPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION),
                        aErrorStack );
    DTL_PARAM_VALIDATE( aAvailableSize >= DTL_INTERVAL_YEAR_TO_MONTH_SIZE, aErrorStack );
    DTL_CHECK_TYPE( DTL_TYPE_INTERVAL_YEAR_TO_MONTH, aValue, aErrorStack );

    DTL_PARAM_VALIDATE( aString != NULL, aErrorStack );

    DTL_PARAM_VALIDATE( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_YEAR)  ||
                        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MONTH) ||
                        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH),
                        aErrorStack );

    *aSuccessWithInfo = STL_FALSE;

    sIntervalYearToMonthType.mIndicator = aIntervalIndicator;

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
                               DTL_TYPE_INTERVAL_YEAR_TO_MONTH,
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
     *  YEAR, MONTH, YEAR TO MONTH interval type이외의 불필요한 정보가 있는 경우 에러처리한다.
     * 예) '1-2' ( O )
     *     '1-2 3 11:11:11.12345' ( X )
     */

    /*
     * YEAR, MONTH, YEAR TO MONTH interval type에 string으로 입력 가능한 포멧은 다음과 같다.
     * 
     * <unquoted interval string> ::=
     *        [ <sign> ] { <year-month literal> | <day-time literal> }
     *
     * <year-month literal> ::=
     *        <years value> [ <minus sign> <months value> ]
     *        | <months value>
     *        
     *  (1) year-month
     *      '1-2'  =>  ( 1 year 2 month )
     *     예) YEAR  => '1-0' ( O ), '+1-0' ( O ), '-1-1' ( X ),  '1-9' ( X )
     *     예) MONTH => '1-0' ( O ), '0-12' ( X ), '1-1' ( O ), '+1-1' ( O ), '-1-1' ( O )
     *     예) YEAR TO MONTH => '1-0' ( O ), '0-12' ( X ), '1-1' ( O ), '+1-1' ( O ), '-1-1' ( O )
     *  (2) number
     *      number는 single field에만 해당됨. ( INTERVAL YEAR, INTERVAL MONTH )
     *     예1) INTERVAL YEAR에 '3' 입력시,
     *          '3'을 YEAR의 값으로 인식해 +3-00으로 데이타가 들어감.
     */
    
    STL_TRY_THROW( (sNumFields == 1) &&
                   ((sFieldType[0] == DTL_DTK_DATE) ||
                    (sFieldType[0] == DTL_DTK_TZ)   ||
                    (sFieldType[0] == DTL_DTK_NUMBER)),
                   RAMP_ERROR_BAD_FORMAT );
    
    if( sFieldType[0] == DTL_DTK_TZ )
    {
        if( stlStrchr((sField[0] + 1), '-') != NULL )
        {
            // Do Nothing
        }
        else if( stlStrchr((sField[0] + 1), ':') != NULL )
        {
            STL_THROW( RAMP_ERROR_BAD_FORMAT );
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
        STL_TRY_THROW( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_YEAR) ||
                       (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MONTH),
                       RAMP_ERROR_BAD_FORMAT );
    }
    else
    {
        // Do Nothing
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
                dtdDtlExpTime2IntervalYearToMonth( & sDtlExpTime,
                                                   aIntervalIndicator,
                                                   & sIntervalYearToMonthType );
            }
            break;
        default:
            STL_THROW( RAMP_ERROR_BAD_FORMAT );
            break;
    }
    
    STL_TRY( dtdAdjustIntervalYearToMonth( & sIntervalYearToMonthType,
                                           aIntervalIndicator,
                                           aPrecision,
                                           aErrorStack )
             == STL_SUCCESS );
    
    ((dtlIntervalYearToMonthType *)(aValue->mValue))->mIndicator =
        sIntervalYearToMonthType.mIndicator;
    ((dtlIntervalYearToMonthType *)(aValue->mValue))->mMonth = sIntervalYearToMonthType.mMonth;

    aValue->mLength = DTL_INTERVAL_YEAR_TO_MONTH_SIZE;

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
 * @brief integer value로부터     dtdIntervalYearToMonth 타입의 value 구성
 *  <BR>  interval single field에 대해서만 지원.
 *        ( interval YEAR, MONTH )
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
stlStatus dtdIntervalYearToMonthSetValueFromInteger( stlInt64               aInteger,
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
    dtlIntervalYearToMonthType  sIntervalYearToMonthType;
    dtlExpTime                  sDtlExpTime;

    DTL_PARAM_VALIDATE( (aPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                        (aPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION),
                        aErrorStack );    
    DTL_PARAM_VALIDATE( aAvailableSize >= DTL_INTERVAL_YEAR_TO_MONTH_SIZE, aErrorStack );
    DTL_CHECK_TYPE( DTL_TYPE_INTERVAL_YEAR_TO_MONTH, aValue, aErrorStack );

    DTL_PARAM_VALIDATE( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_YEAR) ||
                        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MONTH),
                        aErrorStack );

    /*
     * DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION (6)을 넘는지 체크.
     */ 
    STL_TRY_THROW( (aInteger >= DTD_INTERVAL_MAX_PRECISION_MIN_VALUE) &&
                   (aInteger <= DTD_INTERVAL_MAX_PRECISION_MAX_VALUE),
                   ERROR_OUT_OF_PRECISION );

    *aSuccessWithInfo = STL_FALSE;
    
    DTL_INIT_DTLEXPTIME( sDtlExpTime );
    
    if( aIntervalIndicator == DTL_INTERVAL_INDICATOR_YEAR )
    {
        /* INTERVAL YEAR */
        sDtlExpTime.mYear = aInteger;        
    }
    else if( aIntervalIndicator == DTL_INTERVAL_INDICATOR_MONTH )
    {
        /* INTERVAL MONTH */
        sDtlExpTime.mMonth = aInteger;
    }
    else
    {
        STL_DASSERT( STL_FALSE );
        STL_THROW( RAMP_ERR_INTERVAL_INDICATOR_INVALID );
    }
    
    dtdDtlExpTime2IntervalYearToMonth( & sDtlExpTime,
                                       aIntervalIndicator,
                                       & sIntervalYearToMonthType );
    
    STL_TRY( dtdAdjustIntervalYearToMonth( & sIntervalYearToMonthType,
                                           aIntervalIndicator,
                                           aPrecision,
                                           aErrorStack )
             == STL_SUCCESS );

    ((dtlIntervalYearToMonthType*)(aValue->mValue))->mIndicator
        = sIntervalYearToMonthType.mIndicator;
    ((dtlIntervalYearToMonthType*)(aValue->mValue))->mMonth = sIntervalYearToMonthType.mMonth;
    
    aValue->mLength = DTL_INTERVAL_YEAR_TO_MONTH_SIZE;    
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_PRECISION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_YM_INTERVAL_SOME_FILEDS_TRUNCATED_CONVERSION,
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
/*  * @brief real value로부터 dtdIntervalYearToMonth 타입의 value 구성 */
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
/* stlStatus dtdIntervalYearToMonthSetValueFromReal( stlFloat64             aReal, */
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
stlStatus dtdIntervalYearToMonthReverseByteOrder( void            * aValue,
                                                  stlBool           aIsSameEndian,
                                                  stlErrorStack   * aErrorStack )
{
    dtlIntervalYearToMonthType  *sIntervalYearToMonthType;

    DTL_PARAM_VALIDATE( aValue != NULL, aErrorStack );

    sIntervalYearToMonthType = (dtlIntervalYearToMonthType *)(aValue);

    if( aIsSameEndian == STL_FALSE )
    {
        dtdReverseByteOrder( (stlChar*) & sIntervalYearToMonthType->mIndicator,
                             STL_SIZEOF(sIntervalYearToMonthType->mIndicator),
                             (stlChar*) & sIntervalYearToMonthType->mIndicator );
        
        dtdReverseByteOrder( (stlChar*) & sIntervalYearToMonthType->mMonth,
                             STL_SIZEOF(sIntervalYearToMonthType->mMonth),
                             (stlChar*) & sIntervalYearToMonthType->mMonth );
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
 * @brief dtdIntervalYearToMonth value를 leading precision 자리수에 맞게 문자열로 변환
 * @param[in]  aValue             dtlDataValue
 * @param[in]  aLeadingPrecision  leading field precision
 * @param[in]  aAvailableSize     aBuffer에 사용할 수 있는 메모리 공간의 크기
 * @param[out] aBuffer            문자열로 변환한 결과가 저장될 메모리 공간
 * @param[out] aLength            변환된 문자열의 길이
 * @param[in]  aVectorFunc        Function Vector
 * @param[in]  aVectorArg         Vector Argument
 * @param[out] aErrorStack        에러 스택
 */
stlStatus dtdIntervalYearToMonthToFormatString( dtlDataValue    * aValue,
                                                stlInt64          aLeadingPrecision,
                                                stlInt64          aAvailableSize,
                                                void            * aBuffer,
                                                stlInt64        * aLength,
                                                dtlFuncVector   * aVectorFunc,
                                                void            * aVectorArg,
                                                stlErrorStack   * aErrorStack )
{
    dtlIntervalYearToMonthType  sIntervalYearToMonthType;
    dtlExpTime                  sDtlExpTime;

    /*
     * dtdIntervalYearToMonthToFormatString() 호출시,
     * precision 정보를 구할 수 없는 경우가 있어
     * DTL_PRECISION_NA 로 넘어올 수 있다.
     * 이런 경우, dtlIntervalYearToMonthToFormatString() 함수내에서
     * DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION 로 처리된다.
     */
    DTL_PARAM_VALIDATE( ( (aLeadingPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                          (aLeadingPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION) ) ||
                        ( aLeadingPrecision == DTL_PRECISION_NA ),
                        aErrorStack );    
    DTL_CHECK_TYPE( DTL_TYPE_INTERVAL_YEAR_TO_MONTH, aValue, aErrorStack );
    DTL_PARAM_VALIDATE( aAvailableSize >= DTL_INTERVAL_YEAR_TO_MONTH_DISPLAY_SIZE(aLeadingPrecision),
                        aErrorStack );
    DTL_PARAM_VALIDATE( aBuffer != NULL, aErrorStack );

    sIntervalYearToMonthType.mIndicator =
        ((dtlIntervalYearToMonthType *)(aValue->mValue))->mIndicator;
    sIntervalYearToMonthType.mMonth = 
        ((dtlIntervalYearToMonthType*)(aValue->mValue))->mMonth;

    DTL_INIT_DTLEXPTIME( sDtlExpTime );    

    dtdIntervalYearToMonth2DtlExpTime( sIntervalYearToMonthType,
                                       & sDtlExpTime );

    STL_TRY( dtlIntervalYearToMonthToFormatString( & sDtlExpTime,
                                                   aLeadingPrecision,
                                                   aBuffer,
                                                   aLength,
                                                   aErrorStack )
             == STL_SUCCESS );                                                   

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
}


/**
 * @brief dtdIntervalYearToMonth value를 문자열로 변환
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
stlStatus dtdIntervalYearToMonthToString( dtlDataValue    * aValue,
                                          stlInt64          aPrecision,
                                          stlInt64          aScale,
                                          stlInt64          aAvailableSize,
                                          void            * aBuffer,
                                          stlInt64        * aLength,
                                          dtlFuncVector   * aVectorFunc,
                                          void            * aVectorArg,
                                          stlErrorStack   * aErrorStack )
{
    STL_TRY( dtdIntervalYearToMonthToFormatString( aValue,
                                                   aPrecision,
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
 * @brief dtdIntervalYearToMonth value를 사용가능한 버퍼사이즈에 맞게 잘라서 문자열로 변환한다.
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
stlStatus dtdIntervalYearToMonthToStringForAvailableSize( dtlDataValue    * aValue,
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
    stlChar           sIntervalBuffer[DTL_INTERVAL_YEAR_TO_MONTH_MAX_STRING_SIZE];
    
    DTL_CHECK_TYPE( DTL_TYPE_INTERVAL_YEAR_TO_MONTH, aValue, aErrorStack );
    DTL_PARAM_VALIDATE( aBuffer != NULL, aErrorStack );

    if( aAvailableSize >= DTL_INTERVAL_YEAR_TO_MONTH_DISPLAY_SIZE(aPrecision) )
    {
        STL_TRY( dtdIntervalYearToMonthToString( aValue,
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
        STL_TRY( dtdIntervalYearToMonthToString( aValue,
                                                 aPrecision,
                                                 aScale,
                                                 DTL_INTERVAL_YEAR_TO_MONTH_MAX_STRING_SIZE,
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
 * @brief utf16 string value로부터 dtdIntervalYearToMonth 타입의 value 구성
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
 * YEAR, MONTH, YEAR TO MONTH interval type에 string으로 입력 가능한 포멧은 다음과 같다.
 * @code
 *
 * <unquoted interval string> ::=
 *        [ <sign> ] { <year-month literal> | <day-time literal> }
 *
 * <year-month literal> ::=
 *        <years value> [ <minus sign> <months value> ]
 *        | <months value>
 *        
 *  (1) year-month
 *      '1-2'  =>  ( 1 year 2 month )
 *     예) YEAR  => '1-0' ( O ), '+1-0' ( O ), '-1-1' ( X ),  '1-9' ( X )
 *     예) MONTH => '1-0' ( O ), '0-12' ( X ), '1-1' ( O ), '+1-1' ( O ), '-1-1' ( O )
 *     예) YEAR TO MONTH => '1-0' ( O ), '0-12' ( X ), '1-1' ( O ), '+1-1' ( O ), '-1-1' ( O )
 *  (2) number
 *      number는 single field에만 해당됨. ( INTERVAL YEAR, INTERVAL MONTH )
 *     예1) INTERVAL YEAR에 '3' 입력시,
 *          '3'을 YEAR의 값으로 인식해 +3-00으로 데이타가 들어감.
 * @endcode
 */
stlStatus dtdIntervalYearToMonthSetValueFromUtf16WCharString( void                 * aString,
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
    dtlIntervalYearToMonthType   sIntervalYearToMonthType;
    dtlExpTime                   sDtlExpTime;
    dtlFractionalSecond          sFractionalSecond;
    stlChar                      sBuffer[256];
    stlChar                    * sField[DTL_MAX_DATE_FIELDS];
    stlInt32                     sFieldType[DTL_MAX_DATE_FIELDS];
    stlInt32                     sNumFields;
    stlInt32                     sDateType;

    DTL_PARAM_VALIDATE( (aPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                        (aPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION),
                        aErrorStack );
    DTL_PARAM_VALIDATE( aAvailableSize >= DTL_INTERVAL_YEAR_TO_MONTH_SIZE, aErrorStack );
    DTL_CHECK_TYPE( DTL_TYPE_INTERVAL_YEAR_TO_MONTH, aValue, aErrorStack );

    DTL_PARAM_VALIDATE( aString != NULL, aErrorStack );

    DTL_PARAM_VALIDATE( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_YEAR)  ||
                        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MONTH) ||
                        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH),
                        aErrorStack );

    *aSuccessWithInfo = STL_FALSE;

    sIntervalYearToMonthType.mIndicator = aIntervalIndicator;

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
                                             DTL_TYPE_INTERVAL_YEAR_TO_MONTH,
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
     *  YEAR, MONTH, YEAR TO MONTH interval type이외의 불필요한 정보가 있는 경우 에러처리한다.
     * 예) '1-2' ( O )
     *     '1-2 3 11:11:11.12345' ( X )
     */

    /*
     * YEAR, MONTH, YEAR TO MONTH interval type에 string으로 입력 가능한 포멧은 다음과 같다.
     * 
     * <unquoted interval string> ::=
     *        [ <sign> ] { <year-month literal> | <day-time literal> }
     *
     * <year-month literal> ::=
     *        <years value> [ <minus sign> <months value> ]
     *        | <months value>
     *        
     *  (1) year-month
     *      '1-2'  =>  ( 1 year 2 month )
     *     예) YEAR  => '1-0' ( O ), '+1-0' ( O ), '-1-1' ( X ),  '1-9' ( X )
     *     예) MONTH => '1-0' ( O ), '0-12' ( X ), '1-1' ( O ), '+1-1' ( O ), '-1-1' ( O )
     *     예) YEAR TO MONTH => '1-0' ( O ), '0-12' ( X ), '1-1' ( O ), '+1-1' ( O ), '-1-1' ( O )
     *  (2) number
     *      number는 single field에만 해당됨. ( INTERVAL YEAR, INTERVAL MONTH )
     *     예1) INTERVAL YEAR에 '3' 입력시,
     *          '3'을 YEAR의 값으로 인식해 +3-00으로 데이타가 들어감.
     */
    
    STL_TRY_THROW( (sNumFields == 1) &&
                   ((sFieldType[0] == DTL_DTK_DATE) ||
                    (sFieldType[0] == DTL_DTK_TZ)   ||
                    (sFieldType[0] == DTL_DTK_NUMBER)),
                   RAMP_ERROR_BAD_FORMAT );
    
    if( sFieldType[0] == DTL_DTK_TZ )
    {
        if( stlStrchr((sField[0] + 1), '-') != NULL )
        {
            // Do Nothing
        }
        else if( stlStrchr((sField[0] + 1), ':') != NULL )
        {
            STL_THROW( RAMP_ERROR_BAD_FORMAT );
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
        STL_TRY_THROW( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_YEAR) ||
                       (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MONTH),
                       RAMP_ERROR_BAD_FORMAT );
    }
    else
    {
        // Do Nothing
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
                dtdDtlExpTime2IntervalYearToMonth( & sDtlExpTime,
                                                   aIntervalIndicator,
                                                   & sIntervalYearToMonthType );
            }
            break;
        default:
            STL_THROW( RAMP_ERROR_BAD_FORMAT );
            break;
    }
    
    STL_TRY( dtdAdjustIntervalYearToMonth( & sIntervalYearToMonthType,
                                           aIntervalIndicator,
                                           aPrecision,
                                           aErrorStack )
             == STL_SUCCESS );
    
    ((dtlIntervalYearToMonthType *)(aValue->mValue))->mIndicator =
        sIntervalYearToMonthType.mIndicator;
    ((dtlIntervalYearToMonthType *)(aValue->mValue))->mMonth = sIntervalYearToMonthType.mMonth;

    aValue->mLength = DTL_INTERVAL_YEAR_TO_MONTH_SIZE;

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
 * @brief utf32 string value로부터 dtdIntervalYearToMonth 타입의 value 구성
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
 * YEAR, MONTH, YEAR TO MONTH interval type에 string으로 입력 가능한 포멧은 다음과 같다.
 * @code
 *
 * <unquoted interval string> ::=
 *        [ <sign> ] { <year-month literal> | <day-time literal> }
 *
 * <year-month literal> ::=
 *        <years value> [ <minus sign> <months value> ]
 *        | <months value>
 *        
 *  (1) year-month
 *      '1-2'  =>  ( 1 year 2 month )
 *     예) YEAR  => '1-0' ( O ), '+1-0' ( O ), '-1-1' ( X ),  '1-9' ( X )
 *     예) MONTH => '1-0' ( O ), '0-12' ( X ), '1-1' ( O ), '+1-1' ( O ), '-1-1' ( O )
 *     예) YEAR TO MONTH => '1-0' ( O ), '0-12' ( X ), '1-1' ( O ), '+1-1' ( O ), '-1-1' ( O )
 *  (2) number
 *      number는 single field에만 해당됨. ( INTERVAL YEAR, INTERVAL MONTH )
 *     예1) INTERVAL YEAR에 '3' 입력시,
 *          '3'을 YEAR의 값으로 인식해 +3-00으로 데이타가 들어감.
 * @endcode
 */
stlStatus dtdIntervalYearToMonthSetValueFromUtf32WCharString( void                 * aString,
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
    dtlIntervalYearToMonthType   sIntervalYearToMonthType;
    dtlExpTime                   sDtlExpTime;
    dtlFractionalSecond          sFractionalSecond;
    stlChar                      sBuffer[256];
    stlChar                    * sField[DTL_MAX_DATE_FIELDS];
    stlInt32                     sFieldType[DTL_MAX_DATE_FIELDS];
    stlInt32                     sNumFields;
    stlInt32                     sDateType;

    DTL_PARAM_VALIDATE( (aPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                        (aPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION),
                        aErrorStack );
    DTL_PARAM_VALIDATE( aAvailableSize >= DTL_INTERVAL_YEAR_TO_MONTH_SIZE, aErrorStack );
    DTL_CHECK_TYPE( DTL_TYPE_INTERVAL_YEAR_TO_MONTH, aValue, aErrorStack );

    DTL_PARAM_VALIDATE( aString != NULL, aErrorStack );

    DTL_PARAM_VALIDATE( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_YEAR)  ||
                        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MONTH) ||
                        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH),
                        aErrorStack );

    *aSuccessWithInfo = STL_FALSE;

    sIntervalYearToMonthType.mIndicator = aIntervalIndicator;

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
                                             DTL_TYPE_INTERVAL_YEAR_TO_MONTH,
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
     *  YEAR, MONTH, YEAR TO MONTH interval type이외의 불필요한 정보가 있는 경우 에러처리한다.
     * 예) '1-2' ( O )
     *     '1-2 3 11:11:11.12345' ( X )
     */

    /*
     * YEAR, MONTH, YEAR TO MONTH interval type에 string으로 입력 가능한 포멧은 다음과 같다.
     * 
     * <unquoted interval string> ::=
     *        [ <sign> ] { <year-month literal> | <day-time literal> }
     *
     * <year-month literal> ::=
     *        <years value> [ <minus sign> <months value> ]
     *        | <months value>
     *        
     *  (1) year-month
     *      '1-2'  =>  ( 1 year 2 month )
     *     예) YEAR  => '1-0' ( O ), '+1-0' ( O ), '-1-1' ( X ),  '1-9' ( X )
     *     예) MONTH => '1-0' ( O ), '0-12' ( X ), '1-1' ( O ), '+1-1' ( O ), '-1-1' ( O )
     *     예) YEAR TO MONTH => '1-0' ( O ), '0-12' ( X ), '1-1' ( O ), '+1-1' ( O ), '-1-1' ( O )
     *  (2) number
     *      number는 single field에만 해당됨. ( INTERVAL YEAR, INTERVAL MONTH )
     *     예1) INTERVAL YEAR에 '3' 입력시,
     *          '3'을 YEAR의 값으로 인식해 +3-00으로 데이타가 들어감.
     */
    
    STL_TRY_THROW( (sNumFields == 1) &&
                   ((sFieldType[0] == DTL_DTK_DATE) ||
                    (sFieldType[0] == DTL_DTK_TZ)   ||
                    (sFieldType[0] == DTL_DTK_NUMBER)),
                   RAMP_ERROR_BAD_FORMAT );
    
    if( sFieldType[0] == DTL_DTK_TZ )
    {
        if( stlStrchr((sField[0] + 1), '-') != NULL )
        {
            // Do Nothing
        }
        else if( stlStrchr((sField[0] + 1), ':') != NULL )
        {
            STL_THROW( RAMP_ERROR_BAD_FORMAT );
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
        STL_TRY_THROW( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_YEAR) ||
                       (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MONTH),
                       RAMP_ERROR_BAD_FORMAT );
    }
    else
    {
        // Do Nothing
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
                dtdDtlExpTime2IntervalYearToMonth( & sDtlExpTime,
                                                   aIntervalIndicator,
                                                   & sIntervalYearToMonthType );
            }
            break;
        default:
            STL_THROW( RAMP_ERROR_BAD_FORMAT );
            break;
    }
    
    STL_TRY( dtdAdjustIntervalYearToMonth( & sIntervalYearToMonthType,
                                           aIntervalIndicator,
                                           aPrecision,
                                           aErrorStack )
             == STL_SUCCESS );
    
    ((dtlIntervalYearToMonthType *)(aValue->mValue))->mIndicator =
        sIntervalYearToMonthType.mIndicator;
    ((dtlIntervalYearToMonthType *)(aValue->mValue))->mMonth = sIntervalYearToMonthType.mMonth;

    aValue->mLength = DTL_INTERVAL_YEAR_TO_MONTH_SIZE;

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
 * @brief dtdIntervalYearToMonth value를 utf16 문자열로 변환
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
stlStatus dtdIntervalYearToMonthToUtf16WCharString( dtlDataValue    * aValue,
                                                    stlInt64          aPrecision,
                                                    stlInt64          aScale,
                                                    stlInt64          aAvailableSize,
                                                    void            * aBuffer,
                                                    stlInt64        * aLength,
                                                    dtlFuncVector   * aVectorFunc,
                                                    void            * aVectorArg,
                                                    stlErrorStack   * aErrorStack )
{
    dtlIntervalYearToMonthType  sIntervalYearToMonthType;
    dtlExpTime                  sDtlExpTime;

    /* /\* */
    /*  * dtdIntervalYearToMonthToFormatString() 호출시, */
    /*  * precision 정보를 구할 수 없는 경우가 있어 */
    /*  * DTL_PRECISION_NA 로 넘어올 수 있다. */
    /*  * 이런 경우, dtlIntervalYearToMonthToFormatString() 함수내에서 */
    /*  * DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION 로 처리된다. */
    /*  *\/ */
    /* DTL_PARAM_VALIDATE( ( (aPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) && */
    /*                       (aPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION) ) || */
    /*                     ( aPrecision == DTL_PRECISION_NA ), */
    /*                     aErrorStack );     */

    DTL_PARAM_VALIDATE( (aPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                        (aPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION),
                        aErrorStack );    
    DTL_CHECK_TYPE( DTL_TYPE_INTERVAL_YEAR_TO_MONTH, aValue, aErrorStack );
    DTL_PARAM_VALIDATE( aAvailableSize >= 
                        ( DTL_INTERVAL_YEAR_TO_MONTH_DISPLAY_SIZE(aPrecision) * STL_UINT16_SIZE ),
                        aErrorStack );
    DTL_PARAM_VALIDATE( aBuffer != NULL, aErrorStack );

    sIntervalYearToMonthType.mIndicator =
        ((dtlIntervalYearToMonthType *)(aValue->mValue))->mIndicator;
    sIntervalYearToMonthType.mMonth = 
        ((dtlIntervalYearToMonthType*)(aValue->mValue))->mMonth;

    DTL_INIT_DTLEXPTIME( sDtlExpTime );    

    dtdIntervalYearToMonth2DtlExpTime( sIntervalYearToMonthType,
                                       & sDtlExpTime );

    STL_TRY( dtlIntervalYearToMonthToFormatUtf16String( & sDtlExpTime,
                                                        aPrecision,
                                                        aBuffer,
                                                        aLength,
                                                        aErrorStack )
             == STL_SUCCESS );                                                   

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
}


/**
 * @brief dtdIntervalYearToMonth value를 utf32 문자열로 변환
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
stlStatus dtdIntervalYearToMonthToUtf32WCharString( dtlDataValue    * aValue,
                                                    stlInt64          aPrecision,
                                                    stlInt64          aScale,
                                                    stlInt64          aAvailableSize,
                                                    void            * aBuffer,
                                                    stlInt64        * aLength,
                                                    dtlFuncVector   * aVectorFunc,
                                                    void            * aVectorArg,
                                                    stlErrorStack   * aErrorStack )
{
    dtlIntervalYearToMonthType  sIntervalYearToMonthType;
    dtlExpTime                  sDtlExpTime;

    /* /\* */
    /*  * dtdIntervalYearToMonthToFormatString() 호출시, */
    /*  * precision 정보를 구할 수 없는 경우가 있어 */
    /*  * DTL_PRECISION_NA 로 넘어올 수 있다. */
    /*  * 이런 경우, dtlIntervalYearToMonthToFormatString() 함수내에서 */
    /*  * DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION 로 처리된다. */
    /*  *\/ */
    /* DTL_PARAM_VALIDATE( ( (aPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) && */
    /*                       (aPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION) ) || */
    /*                     ( aPrecision == DTL_PRECISION_NA ), */
    /*                     aErrorStack );     */

    DTL_PARAM_VALIDATE( (aPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                        (aPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION),
                        aErrorStack );    
    DTL_CHECK_TYPE( DTL_TYPE_INTERVAL_YEAR_TO_MONTH, aValue, aErrorStack );
    DTL_PARAM_VALIDATE( aAvailableSize >= 
                        ( DTL_INTERVAL_YEAR_TO_MONTH_DISPLAY_SIZE(aPrecision) * STL_UINT32_SIZE ),
                        aErrorStack );
    DTL_PARAM_VALIDATE( aBuffer != NULL, aErrorStack );

    sIntervalYearToMonthType.mIndicator =
        ((dtlIntervalYearToMonthType *)(aValue->mValue))->mIndicator;
    sIntervalYearToMonthType.mMonth = 
        ((dtlIntervalYearToMonthType*)(aValue->mValue))->mMonth;

    DTL_INIT_DTLEXPTIME( sDtlExpTime );    

    dtdIntervalYearToMonth2DtlExpTime( sIntervalYearToMonthType,
                                       & sDtlExpTime );

    STL_TRY( dtlIntervalYearToMonthToFormatUtf32String( & sDtlExpTime,
                                                        aPrecision,
                                                        aBuffer,
                                                        aLength,
                                                        aErrorStack )
             == STL_SUCCESS );                                                   

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
}


/** @} */


