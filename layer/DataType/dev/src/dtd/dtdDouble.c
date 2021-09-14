/*******************************************************************************
 * dtdDouble.c
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
 * @file dtdDouble.c
 * @brief DataType Layer Double type 정의
 */

#include <dtl.h>

#include <dtdDef.h>
#include <dtdDataType.h>

/**
 * @addtogroup dtlDouble
 * @{
 */

/**
 * @brief Double 타입의 length값을 얻음
 * @param[in]  aPrecision  사용하지 않음
 * @param[in]  aStringUnit string length unit (dtlStringLengthUnit 참조) 
 * @param[out] aLength     Double의 길이 값2w
 * @param[in]  aVectorFunc Function Vector
 * @param[in]  aVectorArg  Vector Argument
 * @param[out] aErrorStack 에러 스택
 */
stlStatus dtdDoubleGetLength( stlInt64              aPrecision,
                              dtlStringLengthUnit   aStringUnit,
                              stlInt64            * aLength,
                              dtlFuncVector       * aVectorFunc,
                              void                * aVectorArg,
                              stlErrorStack       * aErrorStack )
{
    *aLength = DTL_NATIVE_DOUBLE_SIZE;
    
    return STL_SUCCESS;
}


/**
 * @brief Double 타입의 length값을 얻음 (입력 문자열에 영향을 받지 않음)
 * @param[in]  aPrecision    사용하지 않음
 * @param[in]  aScale        사용하지 않음
 * @param[in]  aStringUnit   string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aString       입력 문자열
 * @param[in]  aStringLength aString의 길이    
 * @param[out] aLength       Double의 길이 값
 * @param[in]  aVectorFunc   Function Vector
 * @param[in]  aVectorArg    Vector Argument
 * @param[out] aErrorStack   에러 스택
 */
stlStatus dtdDoubleGetLengthFromString( stlInt64              aPrecision,
                                        stlInt64              aScale,
                                        dtlStringLengthUnit   aStringUnit,
                                        stlChar             * aString,
                                        stlInt64              aStringLength,
                                        stlInt64            * aLength,
                                        dtlFuncVector       * aVectorFunc,
                                        void                * aVectorArg,
                                        stlErrorStack       * aErrorStack )
{
    stlFloat64         sConvertedValue;

    STL_PARAM_VALIDATE( aString != NULL, aErrorStack );

    /* string value로부터 Double 타입의 value 구성 */
    STL_TRY( dtdToFloat64FromString( (stlChar *) aString,
                                     aStringLength,
                                     & sConvertedValue,
                                     aErrorStack )
             == STL_SUCCESS );
        
    *aLength = DTL_NATIVE_DOUBLE_SIZE;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Double 타입의 length값을 얻음 (입력 숫자에 영향을 받지 않음)
 * @param[in]  aPrecision  사용하지 않음
 * @param[in]  aScale      사용하지 않음
 * @param[in]  aInteger    입력 숫자
 * @param[out] aLength     Double의 길이 값
 * @param[out] aErrorStack 에러 스택
 */
stlStatus dtdDoubleGetLengthFromInteger( stlInt64          aPrecision,
                                         stlInt64          aScale,
                                         stlInt64          aInteger,
                                         stlInt64        * aLength,
                                         stlErrorStack   * aErrorStack )
{
    /* integer value로부터 Double 타입의 value 구성 */
//    dtlDoubleType    sSrcValue;   /* asign만 하고 사용하지 않는 변수 주석처리 */

//    sSrcValue = (dtlDoubleType) aInteger;
    
    *aLength = DTL_NATIVE_DOUBLE_SIZE;
    
    return STL_SUCCESS;
}

/**
 * @brief Double 타입의 length값을 얻음 (입력 숫자에 영향을 받지 않음)
 * @param[in]  aPrecision  사용하지 않음
 * @param[in]  aScale      사용하지 않음
 * @param[in]  aReal       입력 숫자
 * @param[out] aLength     Double의 길이 값
 * @param[out] aErrorStack 에러 스택
 */
stlStatus dtdDoubleGetLengthFromReal( stlInt64          aPrecision,
                                      stlInt64          aScale,
                                      stlFloat64        aReal,
                                      stlInt64        * aLength,
                                      stlErrorStack   * aErrorStack )
{
    /* real value로부터 Double 타입의 value 구성 */
    STL_TRY_THROW( stlIsfinite( aReal ) != STL_IS_FINITE_FALSE,
                   ERROR_OUT_OF_RANGE );
    
    *aLength = DTL_NATIVE_DOUBLE_SIZE;
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_DATA_OUTSIDE_RANGE_NUMBER_CONVERTED,
                      NULL,
                      aErrorStack );
    } 
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief string value로부터 Double 타입의 value 구성
 * @param[in]  aString            value에 저장될 입력 문자열
 * @param[in]  aStringLength      aString의 길이   
 * @param[in]  aPrecision         사용하지 않음
 * @param[in]  aScale             사용하지 않음
 * @param[in]  aStringUnit        string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aIntervalIndicator 사용하지 않음.
 *                           <BR> INTERVAL 타입의 Indicator dtlIntervalIndicator 참조 
 * @param[in]  aAvailableSize     mValue에 사용할 수 있는 메모리 공간의 크기  
 * @param[out] aValue             dtlDataValue의 주소
 * @param[out] aSuccessWithInfo   warning 발생 여부
 * @param[in]  aSourceVectorFunc  Source Function Vector
 * @param[in]  aSourceVectorArg   Source Vector Argument
 * @param[in]  aDestVectorFunc    Dest Function Vector
 * @param[in]  aDestVectorArg     Dest Vector Argument
 * @param[out] aErrorStack        에러 스택
 */
stlStatus dtdDoubleSetValueFromString( stlChar              * aString,
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
    stlFloat64         sConvertedValue;
    dtlDoubleType    * sDstValue;
    
    DTL_CHECK_TYPE( DTL_TYPE_NATIVE_DOUBLE, aValue, aErrorStack );
    STL_PARAM_VALIDATE( aString != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aAvailableSize >= DTL_NATIVE_DOUBLE_SIZE, aErrorStack );

    *aSuccessWithInfo = STL_FALSE;

    /* string value로부터 Double 타입의 value 구성 */
    STL_TRY( dtdToFloat64FromString( (stlChar *) aString,
                                     aStringLength,
                                     & sConvertedValue,
                                     aErrorStack )
             == STL_SUCCESS );
        
    sDstValue  = (dtlDoubleType *) aValue->mValue;
    *sDstValue = (dtlDoubleType) sConvertedValue;
    
    aValue->mLength = DTL_NATIVE_DOUBLE_SIZE;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief integer value로부터 Double 타입의 value 구성
 * @param[in]  aInteger           value에 저장될 입력 숫자
 * @param[in]  aPrecision         사용하지 않음
 * @param[in]  aScale             사용하지 않음
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
stlStatus dtdDoubleSetValueFromInteger( stlInt64               aInteger,
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
    /* integer value로부터 Double 타입의 value 구성 */
    dtlDoubleType  * sDstValue;

    DTL_CHECK_TYPE( DTL_TYPE_NATIVE_DOUBLE, aValue, aErrorStack );
    STL_PARAM_VALIDATE( aAvailableSize >= DTL_NATIVE_DOUBLE_SIZE, aErrorStack );

    *aSuccessWithInfo = STL_FALSE;
    
    /* string value로부터 Double 타입의 value 구성 */
    sDstValue  = (dtlDoubleType *) aValue->mValue;
    *sDstValue = (dtlDoubleType) aInteger;

    aValue->mLength = DTL_NATIVE_DOUBLE_SIZE;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief real value로부터 Double 타입의 value 구성
 * @param[in]  aReal              value에 저장될 입력 숫자
 * @param[in]  aPrecision         사용하지 않음
 * @param[in]  aScale             사용하지 않음
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
stlStatus dtdDoubleSetValueFromReal( stlFloat64             aReal,
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
    /* real value로부터 Double 타입의 value 구성 */
    dtlDoubleType    sSrcValue;
    dtlDoubleType  * sDstValue;

    DTL_CHECK_TYPE( DTL_TYPE_NATIVE_DOUBLE, aValue, aErrorStack );
    STL_PARAM_VALIDATE( aAvailableSize >= DTL_NATIVE_DOUBLE_SIZE, aErrorStack );

    *aSuccessWithInfo = STL_FALSE;
    
    /* string value로부터 Double 타입의 value 구성 */
    sSrcValue = (dtlDoubleType) aReal;
    
    STL_TRY_THROW( stlIsfinite( sSrcValue ) != STL_IS_FINITE_FALSE,
                   ERROR_OUT_OF_RANGE );
    
    sDstValue  = (dtlDoubleType *) aValue->mValue;
    *sDstValue = sSrcValue;

    aValue->mLength = DTL_NATIVE_DOUBLE_SIZE;

    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_DATA_OUTSIDE_RANGE_NUMBER_CONVERTED,
                      NULL,
                      aErrorStack );
    } 
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Byte Order 변경 (redirect 함수 아님, 내부 함수)
 * @param[in]  aValue          byte swapping 대상 및 결과 
 * @param[in]  aIsSameEndian   byte swapping 필요 여부
 * @param[out] aErrorStack     에러 스택
 */
stlStatus dtdDoubleReverseByteOrder( void            * aValue,
                                     stlBool           aIsSameEndian,
                                     stlErrorStack   * aErrorStack )
{
    STL_PARAM_VALIDATE( aValue != NULL, aErrorStack );
    
    if( aIsSameEndian == STL_FALSE )
    {
        dtdReverseByteOrder( (stlChar*) aValue,
                             DTL_NATIVE_DOUBLE_SIZE,
                             aValue );
    }
    else
    {
        /* Do Nothing */
    }

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Float64 value로부터 변환된 문자열의 길이 
 * @param[in]  aFloat64             Float64 value  
 * @param[out] aIntegralLength      정수부분 문자열의 길이
 * @param[out] aEntirePrintLength   전체 숫자를 그대로 표현할때 문자열의 길이
 * @param[out] aExpPrintLength      전체 숫자를 exponent 형태로 표현할때  문자열의 길이
 * @param[out] aErrorStack          에러 스택
 */
stlStatus dtdGetStringLengthFromFloat64( stlFloat64      * aFloat64,
                                         stlInt64        * aIntegralLength,
                                         stlInt64        * aEntirePrintLength,
                                         stlInt64        * aExpPrintLength,
                                         stlErrorStack   * aErrorStack )
{
    stlFloat64      sValue;
    stlFloat64      sPow;
    stlBool         sIsNegative;
    stlInt64        sInteger;
    stlBool         sIsNegativeExp;
    stlInt32        sExponent;
    stlInt32        sExponentForExpExpression;
    stlInt64        sTotDigitCount;
    stlInt64        sIntDigitCount;
    stlInt32        sIntegralLength;
    stlInt32        sExpLength;
    stlInt32        sNonExpLength;

    STL_PARAM_VALIDATE( aFloat64 != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aIntegralLength != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aEntirePrintLength != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aExpPrintLength != NULL, aErrorStack );
    
    sValue = *aFloat64;

    if( stlIsfinite(sValue) == STL_IS_FINITE_FALSE )
    {
        *aIntegralLength = DTL_BINARY_REAL_INFINITY_SIZE;
        *aEntirePrintLength = DTL_BINARY_REAL_INFINITY_SIZE;
        *aExpPrintLength = DTL_BINARY_REAL_INFINITY_SIZE;
        STL_THROW( RAMP_SUCCESS );
    }

    if( sValue == 0 )
    {
        *aIntegralLength = 1;
        *aEntirePrintLength = 1;
        *aExpPrintLength = 1;
        STL_THROW( RAMP_SUCCESS );
    }


    /* STL_DBL_DIG만큼의 유효숫자(trailing zero는 제거한 상태)와 
     * exponent의 형태로 만든다. */
    if( sValue < 0.0 )
    {
        sIsNegative = STL_TRUE;
        sValue = -sValue;
    }
    else
    {
        sIsNegative = STL_FALSE;
    }

    sTotDigitCount = stlLog10( sValue );
    if( sValue < 1 )
    {
        DTL_IS_VALID( stlGet10Pow( sTotDigitCount, &sPow, aErrorStack ) );
        if( sValue == sPow )
        {
            sTotDigitCount++;
        }
        else
        {
            /* Do Nothing */
        }
    }
    else
    {
        sTotDigitCount++;
    }

    if( sTotDigitCount > 0 )
    {
        sIntegralLength = sTotDigitCount + (sIsNegative == STL_TRUE ? 1 : 0);
    }
    else
    {
        sIntegralLength = (sIsNegative == STL_TRUE ? 2 : 1);
    }

    sExponent = sTotDigitCount - STL_DBL_DIG;
    if( sExponent > STL_DBL_MAX_10_EXP )
    {
        DTL_IS_VALID( stlGet10Pow( STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sValue = sValue / sPow;
        DTL_IS_VALID( stlGet10Pow( sExponent - STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sInteger = (stlInt64)(sValue / sPow + 0.5);
    }
    if( sExponent < -STL_DBL_MAX_10_EXP )
    {
        DTL_IS_VALID( stlGet10Pow( -STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sValue = sValue / sPow;
        DTL_IS_VALID( stlGet10Pow( sExponent + STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sInteger = (stlInt64)(sValue / sPow + 0.5);
    }
    else
    {
        DTL_IS_VALID( stlGet10Pow( sExponent, &sPow, aErrorStack ) );
        sInteger = (stlInt64)(sValue / sPow + 0.5);
    }
    
    DTL_IS_VALID( stlGet10Pow( STL_DBL_DIG - 1, &sPow, aErrorStack ) );
    while( sInteger < (stlInt64)sPow )
    {
        sInteger *= 10;
        sExponent--;
    }

    sIntDigitCount = STL_DBL_DIG;
    while( sInteger % 10 == 0 )
    {
        sExponent++;
        sIntDigitCount--;
        sInteger /= 10;
    }

    if( sIntDigitCount == 0 )
    {
        /* 반올림으로 인해 자리수가 증가한 경우 */
        sIntDigitCount = 1;
    }
    else
    {
        /* Do Nothing */
    }

    if( sExponent < 0 )
    {
        sIsNegativeExp = STL_TRUE;
        sExponent = -sExponent;
    }
    else
    {
        sIsNegativeExp = STL_FALSE;
    }

    /* 지금까지 처리된 결과는 아래와 같다.
     * sIsNegative      sValue의 음수여부
     * sInteger         sValue의 STL_DBL_DIG만큼의 유효숫자에서 
     *                  trailing zero를 제거한 수
     * sIntDigitCount   sInteger의 digit count
     * sIsNegativeExp   exponent의 음수여부
     * sExponent        exponent의 값

     * */

    if( sIsNegativeExp == STL_TRUE )
    {
        if( sExponent <= sIntDigitCount )
        {
            sNonExpLength = sIntDigitCount + 1/* point */ + (sIsNegative == STL_TRUE ? 1 : 0);

            sExponentForExpExpression = (sIntDigitCount == sExponent ? 1 : sIntDigitCount - sExponent - 1);
            sExpLength = sIntDigitCount +
                (sIntDigitCount > 1 ? 1 : 0) /* point */ +
                (sIsNegative == STL_TRUE ? 1 : 0);
            sExpLength += 2; /* exponent + sign */
            sExpLength += (sExponentForExpExpression < 10 ? 1 : (sExponentForExpExpression < 100 ? 2 : 3));
        }
        else
        {
            sNonExpLength = sIntDigitCount + 
                1/* point */ + 
                (sIsNegative == STL_TRUE ? 1 : 0) +
                (sExponent - sIntDigitCount) /* leading zero */;

            sExponentForExpExpression = sExponent - sIntDigitCount + 1;
            sExpLength = sIntDigitCount +
                (sIntDigitCount > 1 ? 1 : 0) /* point */ +
                (sIsNegative == STL_TRUE ? 1 : 0);
            sExpLength += 2; /* exponent + sign */
            sExpLength += (sExponentForExpExpression < 10 ? 1 : (sExponentForExpExpression < 100 ? 2 : 3));
        }
    }
    else
    {
        sNonExpLength = sIntDigitCount + sExponent + (sIsNegative == STL_TRUE ? 1 : 0);

        sExponentForExpExpression = sExponent + sIntDigitCount - 1;
        sExpLength = sIntDigitCount + 
            (sIntDigitCount > 1 ? 1 : 0) /* point */ + 
            (sIsNegative == STL_TRUE ? 1 : 0);
        sExpLength += 2; /* exponent + sign */
        sExpLength += (sExponentForExpExpression < 10 ? 1 : (sExponentForExpExpression < 100 ? 2 : 3));
    }

    *aIntegralLength = sIntegralLength;
    *aEntirePrintLength = sNonExpLength;
    *aExpPrintLength = sExpLength;

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Float64 value로부터 유효한 숫자의 길이 
 * @param[in]  aFloat64             Float64 value
 * @param[out] aValidNumberLength   유효한 숫자의 길의 ( 소수점 및 trailing zero 제외 )
 * @param[out] aErrorStack          에러 스택
 * @remark value 의 유효숫자중 소수점에 해당하는 부분을 제외한 부분을 유효한 숫자로 정의 하며 <br>
 *         trailing zero 있는 ( 정수타입 값 ) 경우 trailing zero 를 제외한 값
 */
stlStatus dtdGetValidNumberLengthFromFloat64( stlFloat64      * aFloat64,
                                              stlInt64        * aValidNumberLength,
                                              stlErrorStack   * aErrorStack )
{
    stlFloat64      sValue;
    stlFloat64      sPow;
    stlInt64        sInteger;
    stlBool         sIsNegative;
    stlBool         sIsNegativeExp;
    stlInt32        sExponent;
    stlInt64        sTotDigitCount;
    stlInt64        sIntDigitCount;

    STL_PARAM_VALIDATE( aFloat64 != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aValidNumberLength != NULL, aErrorStack );
    
    sValue = *aFloat64;

    if( stlIsfinite(sValue) == STL_IS_FINITE_FALSE )
    {
        *aValidNumberLength = DTL_BINARY_REAL_INFINITY_SIZE;
        STL_THROW( RAMP_SUCCESS );
    }

    if( sValue == 0 )
    {
        *aValidNumberLength = 1;
        STL_THROW( RAMP_SUCCESS );
    }


    /* STL_DBL_DIG만큼의 유효숫자(trailing zero는 제거한 상태)와 
     * exponent의 형태로 만든다. */
    if( sValue < 0.0 )
    {
        sValue = -sValue;
        sIsNegative = STL_TRUE;
    }
    else
    {
        sIsNegative = STL_FALSE;
    }

    sTotDigitCount = stlLog10( sValue );
    if( sValue < 1 )
    {
        DTL_IS_VALID( stlGet10Pow( sTotDigitCount, &sPow, aErrorStack ) );
        if( sValue == sPow )
        {
            sTotDigitCount++;
        }
        else
        {
            /* Do Nothing */
        }
    }
    else
    {
        sTotDigitCount++;
    }

    sExponent = sTotDigitCount - STL_DBL_DIG;
    if( sExponent > STL_DBL_MAX_10_EXP )
    {
        DTL_IS_VALID( stlGet10Pow( STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sValue = sValue / sPow;
        DTL_IS_VALID( stlGet10Pow( sExponent - STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sInteger = (stlInt64)(sValue / sPow + 0.5);
    }
    if( sExponent < -STL_DBL_MAX_10_EXP )
    {
        DTL_IS_VALID( stlGet10Pow( -STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sValue = sValue / sPow;
        DTL_IS_VALID( stlGet10Pow( sExponent + STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sInteger = (stlInt64)(sValue / sPow + 0.5);
    }
    else
    {
        DTL_IS_VALID( stlGet10Pow( sExponent, &sPow, aErrorStack ) );
        sInteger = (stlInt64)(sValue / sPow + 0.5);
    }

    DTL_IS_VALID( stlGet10Pow( STL_DBL_DIG - 1, &sPow, aErrorStack ) );
    while( sInteger < (stlInt64)sPow )
    {
        sInteger *= 10;
        sExponent--;
    }

    sIntDigitCount = STL_DBL_DIG;
    while( sInteger % 10 == 0 )
    {
        sExponent++;
        sIntDigitCount--;
        sInteger /= 10;
    }

    if( sIntDigitCount == 0 )
    {
        /* 반올림으로 인해 자리수가 증가한 경우 */
        sIntDigitCount = 1;
    }
    else
    {
        /* Do Nothing */
    }

    if( sExponent < 0 )
    {
        sIsNegativeExp = STL_TRUE;
        sExponent = -sExponent;
    }
    else
    {
        sIsNegativeExp = STL_FALSE;
    }

    /* 지금까지 처리된 결과는 아래와 같다.
     * sIsNegative      sValue의 음수여부
     * sInteger         sValue의 STL_DBL_DIG만큼의 유효숫자에서 
     *                  trailing zero를 제거한 수
     * sIntDigitCount   sInteger의 digit count
     * sIsNegativeExp   exponent의 음수여부
     * sExponent        exponent의 값
     * */

    if( sIsNegativeExp == STL_TRUE )
    {
        if( sExponent <= sIntDigitCount )
        {
            /*
             * 123.456
             * .123456
             */
            *aValidNumberLength = sIntDigitCount - sExponent;
            *aValidNumberLength = ( sIsNegative == STL_TRUE ? 1 : 0 ) +
                                  ( *aValidNumberLength == 0 ? 1 : *aValidNumberLength );
        }
        else
        {
            /*
             * .000123456
             */
            *aValidNumberLength = ( sIsNegative == STL_TRUE ? 1 : 0 ) + 1;
        }
    }
    else
    {
        /*
         * 123456
         * 123456000
         */
        *aValidNumberLength = ( sIsNegative == STL_TRUE ? 1 : 0 ) + sIntDigitCount;
    }

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Double value를 문자열로 변환
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
stlStatus dtdDoubleToString( dtlDataValue    * aValue,
                             stlInt64          aPrecision,
                             stlInt64          aScale,
                             stlInt64          aAvailableSize,
                             void            * aBuffer,
                             stlInt64        * aLength,
                             dtlFuncVector   * aVectorFunc,
                             void            * aVectorArg,
                             stlErrorStack   * aErrorStack )
{
    dtlDoubleType   sValue;
    stlFloat64      sPow;
    stlBool         sIsNegative;
    stlInt64        sInteger;
    stlBool         sIsNegativeExp;
    stlInt32        sExponent;
    stlInt32        sExponentForExpExpression;
    stlInt64        sTotDigitCount;
    stlInt64        sIntDigitCount;
    stlInt32        sIntegralLength;
    stlInt32        sExpLength;
    stlInt32        sNonExpLength;
    stlInt32        sMinExpLength;
    stlInt32        sLength;
    stlInt64        sTempValue;
    stlChar       * sTempBuffer;

    sValue = *((dtlDoubleType*) aValue->mValue);

    if( stlIsfinite(sValue) == STL_IS_FINITE_FALSE )
    {
        STL_TRY_THROW( aAvailableSize >= DTL_BINARY_REAL_INFINITY_SIZE, ERROR_NOT_ENOUGH_BUFFER );
        stlMemcpy( (stlChar*)aBuffer, DTL_BINARY_REAL_INFINITY, DTL_BINARY_REAL_INFINITY_SIZE );
        *aLength = DTL_BINARY_REAL_INFINITY_SIZE;
        STL_THROW( RAMP_SUCCESS );
    }

    STL_TRY_THROW( aAvailableSize > 0, ERROR_NOT_ENOUGH_BUFFER );

    if( sValue == 0 )
    {
        *(stlChar*)aBuffer = '0';
        *aLength = 1;
        STL_THROW( RAMP_SUCCESS );
    }

    /* STL_DBL_DIG만큼의 유효숫자(trailing zero는 제거한 상태)와 
     * exponent의 형태로 만든다. */
    if( sValue < 0.0 )
    {
        sIsNegative = STL_TRUE;
        sValue = -sValue;
    }
    else
    {
        sIsNegative = STL_FALSE;
    }

    sTotDigitCount = stlLog10( sValue );
    if( sValue < 1 )
    {
        DTL_IS_VALID( stlGet10Pow( sTotDigitCount, &sPow, aErrorStack ) );
        if( sValue == sPow )
        {
            sTotDigitCount++;
        }
        else
        {
            /* Do Nothing */
        }
    }
    else
    {
        sTotDigitCount++;
    }

    sIntegralLength = sTotDigitCount + (sIsNegative == STL_TRUE ? 1 : 0);

    sExponent = sTotDigitCount - STL_DBL_DIG;
    if( sExponent > STL_DBL_MAX_10_EXP )
    {
        DTL_IS_VALID( stlGet10Pow( STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sValue = sValue / sPow;
        DTL_IS_VALID( stlGet10Pow( sExponent - STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sInteger = (stlInt64)(sValue / sPow + 0.5);
    }
    else if( sExponent < -STL_DBL_MAX_10_EXP )
    {
        DTL_IS_VALID( stlGet10Pow( -STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sValue = sValue / sPow;
        DTL_IS_VALID( stlGet10Pow( sExponent + STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sInteger = (stlInt64)(sValue / sPow + 0.5);
    }
    else
    {
        DTL_IS_VALID( stlGet10Pow( sExponent, &sPow, aErrorStack ) );
        sInteger = (stlInt64)(sValue / sPow + 0.5);
    }

    DTL_IS_VALID( stlGet10Pow( STL_DBL_DIG - 1, &sPow, aErrorStack ) );
    while( sInteger < (stlInt64)sPow )
    {
        sInteger *= 10;
        sExponent--;
    }

    sIntDigitCount = STL_DBL_DIG;
    while( sInteger % 10 == 0 )
    {
        sExponent++;
        sIntDigitCount--;
        sInteger /= 10;
    }

    if( sIntDigitCount == 0 )
    {
        /* 반올림으로 인해 자리수가 증가한 경우 */
        sIntDigitCount = 1;
    }
    else
    {
        /* Do Nothing */
    }

    if( sExponent < 0 )
    {
        sIsNegativeExp = STL_TRUE;
        sExponent = -sExponent;
    }
    else
    {
        sIsNegativeExp = STL_FALSE;
    }

    /* 지금까지 처리된 결과는 아래와 같다.
     * sIsNegative      sValue의 음수여부
     * sInteger         sValue의 STL_DBL_DIG만큼의 유효숫자에서 
     *                  trailing zero를 제거한 수
     * sIntDigitCount   sInteger의 digit count
     * sIsNegativeExp   exponent의 음수여부
     * sExponent        exponent의 값
     * */

    if( sIsNegativeExp == STL_TRUE )
    {
        if( sExponent <= sIntDigitCount )
        {
            sNonExpLength = sIntDigitCount + 1/* point */ + (sIsNegative == STL_TRUE ? 1 : 0);

            sExponentForExpExpression = (sIntDigitCount == sExponent ? 1 : sIntDigitCount - sExponent - 1);
            sExpLength = sIntDigitCount +
                (sIntDigitCount > 1 ? 1 : 0) /* point */ +
                (sIsNegative == STL_TRUE ? 1 : 0);
            sExpLength += 2; /* exponent + sign */
            sExpLength += (sExponentForExpExpression < 10 ? 1 : (sExponentForExpExpression < 100 ? 2 : 3));
        }
        else
        {
            sNonExpLength = sIntDigitCount + 
                1/* point */ + 
                (sIsNegative == STL_TRUE ? 1 : 0) +
                (sExponent - sIntDigitCount) /* leading zero */;

            sExponentForExpExpression = sExponent - sIntDigitCount + 1;
            sExpLength = sIntDigitCount +
                (sIntDigitCount > 1 ? 1 : 0) /* point */ +
                (sIsNegative == STL_TRUE ? 1 : 0);
            sExpLength += 2; /* exponent + sign */
            sExpLength += (sExponentForExpExpression < 10 ? 1 : (sExponentForExpExpression < 100 ? 2 : 3));
        }
    }
    else
    {
        sNonExpLength = sIntDigitCount + sExponent + (sIsNegative == STL_TRUE ? 1 : 0);

        sExponentForExpExpression = sExponent + sIntDigitCount - 1;
        sExpLength = sIntDigitCount + 
            (sIntDigitCount > 1 ? 1 : 0) /* point */ + 
            (sIsNegative == STL_TRUE ? 1 : 0);
        sExpLength += 2; /* exponent + sign */
        sExpLength += (sExponentForExpExpression < 10 ? 1 : (sExponentForExpExpression < 100 ? 2 : 3));
    }

    sMinExpLength = 3/* 1digit + 'E' + exp_sign */ +
        (sIsNegative == STL_TRUE ? 1 : 0) +
        (sExponentForExpExpression < 10 ? 1 : (sExponentForExpExpression < 100 ? 2 : 3));


    if( sNonExpLength <= aAvailableSize )
    {
        /* digit로만 표현 */
        sTempBuffer = (stlChar*)aBuffer;
        if( sIsNegative == STL_TRUE )
        {
            *sTempBuffer = '-';
            sTempBuffer++;
        }
        else
        {
            /* Do Nothing */
        }

        if( sIsNegativeExp == STL_TRUE )
        {
            if( sExponent <= sIntDigitCount )
            {
                sTempBuffer += (sIntDigitCount + 1);
                while( sExponent > 0 )
                {
                    sTempBuffer--;
                    sTempValue = sInteger / 10;
                    *sTempBuffer = sInteger - (sTempValue * 10) + '0';
                    sInteger = sTempValue;
                    sExponent--;
                }
                sTempBuffer--;
                *sTempBuffer = '.';
                while( sInteger > 0 )
                {
                    sTempBuffer--;
                    sTempValue = sInteger / 10;
                    *sTempBuffer = sInteger - (sTempValue * 10) + '0';
                    sInteger = sTempValue;
                }
                sTempBuffer += (sIntDigitCount + 1);
            }
            else
            {
                *sTempBuffer = '.';
                sTempBuffer++;
                sExponent -= sIntDigitCount;
                while( sExponent > 0 )
                {
                    *sTempBuffer = '0';
                    sTempBuffer++;
                    sExponent--;
                }

                sTempBuffer += sIntDigitCount;
                while( sInteger > 0 )
                {
                    sTempBuffer--;
                    sTempValue = sInteger / 10;
                    *sTempBuffer = sInteger - (sTempValue * 10) + '0';
                    sInteger = sTempValue;
                }
                sTempBuffer += sIntDigitCount;
            }
        }
        else
        {
            sTempBuffer += sIntDigitCount;
            while( sInteger > 0 )
            {
                sTempBuffer--;
                sTempValue = sInteger / 10;
                *sTempBuffer = sInteger - (sTempValue * 10) + '0';
                sInteger = sTempValue;
            }
            sTempBuffer += sIntDigitCount;
            while( sExponent > 0 )
            {
                *sTempBuffer = '0';
                sTempBuffer++;
                sExponent--;
            }
        }

        *aLength = sNonExpLength;
    }
    else if( sExpLength <= aAvailableSize )
    {
        /* exponent를 이용한 표현 */
        sTempBuffer = (stlChar*)aBuffer;
        if( sIsNegative == STL_TRUE )
        {
            *sTempBuffer = '-';
            sTempBuffer++;
        }
        else
        {
            /* Do Nothing */
        }

        if( sInteger > 9 )
        {
            sTempBuffer += (sIntDigitCount + 1);
            while( sInteger > 9 )
            {
                sTempBuffer--;
                sTempValue = sInteger / 10;
                *sTempBuffer = sInteger - (sTempValue * 10) + '0';
                sInteger = sTempValue;
            }
            sTempBuffer--;
            *sTempBuffer = '.';
            sTempBuffer--;
            *sTempBuffer = sInteger + '0';
            sTempBuffer += (sIntDigitCount + 1);
        }
        else
        {
            *sTempBuffer = sInteger + '0';
            sTempBuffer++;
        }

        *sTempBuffer = 'E';
        sTempBuffer++;
        *sTempBuffer = (sIsNegativeExp == STL_TRUE ? '-' : '+');
        sTempBuffer++;
        if( sExponentForExpExpression < 10 )
        {
            *sTempBuffer = sExponentForExpExpression + '0';
            sTempBuffer++;
        }
        else if( sExponentForExpExpression < 100 )
        {
            sTempValue = sExponentForExpExpression / 10;
            *sTempBuffer = sTempValue + '0';
            sTempBuffer++;
            *sTempBuffer = sExponentForExpExpression - (sTempValue * 10) + '0';
            sTempBuffer++;
        }
        else
        {
            sTempValue = sExponentForExpExpression / 100;
            *sTempBuffer = sTempValue + '0';
            sTempBuffer++;
            sExponentForExpExpression -= (sTempValue * 100);
            sTempValue = sExponentForExpExpression / 10;
            *sTempBuffer = sTempValue + '0';
            sTempBuffer++;
            *sTempBuffer = sExponentForExpExpression - (sTempValue * 10) + '0';
            sTempBuffer++;
        }

        *aLength = sExpLength;
    }
    else if( (sIntegralLength <= aAvailableSize) &&
             ( (sIntegralLength > 0) ||
               ((sExponent - sIntDigitCount + 1) <= (sMinExpLength - 1)) ) )
    {
        /* digit로 표현하고 availablesize 이후는 truncate */
        sTempBuffer = (stlChar*)aBuffer;
        sLength = aAvailableSize;
        if( sIsNegative == STL_TRUE )
        {
            *sTempBuffer = '-';
            sTempBuffer++;
            sLength--;
        }
        else
        {
            /* Do Nothing */
        }

        if( sIsNegativeExp == STL_TRUE )
        {
            if( sExponent < sIntDigitCount )
            {
                if( sLength < sIntDigitCount )
                {
                    /* 소수 부분의 숫자를 버린다. */
                    if( sLength > (sIntDigitCount - sExponent) )
                    {
                        sInteger /= gPreDefinedPow[sIntDigitCount-sLength+1];
                        sExponent -= (sIntDigitCount - sLength + 1);
                        sIntDigitCount = sLength - 1;
                    }
                    else
                    {
                        sInteger /= gPreDefinedPow[sIntDigitCount-sLength];
                        sExponent -= (sIntDigitCount - sLength);
                        sIntDigitCount = sLength;
                    }
                }

                sTempBuffer += (sIntDigitCount + 1);

                while( sExponent > 0 )
                {
                    sTempBuffer--;
                    sTempValue = sInteger / 10;
                    *sTempBuffer = sInteger - (sTempValue * 10) + '0';
                    sInteger = sTempValue;
                    sExponent--;
                }
                    sTempBuffer--;
                *sTempBuffer = '.';
                while( sInteger > 0 )
                {
                    sTempBuffer--;
                    sTempValue = sInteger / 10;
                    *sTempBuffer = sInteger - (sTempValue * 10) + '0';
                    sInteger = sTempValue;
                }

                *aLength = aAvailableSize;
                STL_THROW( RAMP_RTRUNCATED_FINISH );
            }
            else
            {
                if( sLength <= (sExponent - sIntDigitCount + 1) )
                {
                    /* sLength 이후에나 0이 아닌 숫자가 나온다. */
                    ((stlChar*)aBuffer)[0] = '0';
                    *aLength = 1;

                    STL_THROW( RAMP_RTRUNCATED_FINISH );
                }

                sExponent -= sIntDigitCount;
                sLength -= (sExponent + 1);

                *sTempBuffer = '.';
                sTempBuffer++;
                while( sExponent > 0 )
                {
                    *sTempBuffer = '0';
                    sTempBuffer++;
                    sExponent--;
                }

                if( sLength < sIntDigitCount )
                {
                    sInteger /= gPreDefinedPow[sIntDigitCount-sLength];
                    sIntDigitCount = sLength;
                }

                sTempBuffer += sIntDigitCount;
                while( sInteger > 0 )
                {
                    sTempBuffer--;
                    sTempValue = sInteger / 10;
                    *sTempBuffer = sInteger - (sTempValue * 10) + '0';
                    sInteger = sTempValue;
                }

                *aLength = aAvailableSize;
            }
        }
        else
        {
            /* 모든 자리수가 integral part 부분이므로
             * 여기에 들어올일은 없을 것이다.*/
            STL_ASSERT(0);
        }

        STL_RAMP( RAMP_RTRUNCATED_FINISH );
    }
    else if( sMinExpLength <= aAvailableSize )
    {
        sTempBuffer = (stlChar*)aBuffer;

        if( aAvailableSize > sMinExpLength + 1 )
        {
            sInteger /= gPreDefinedPow[sIntDigitCount - (aAvailableSize - sMinExpLength)];
            sIntDigitCount = (aAvailableSize - sMinExpLength);
        }
        else
        {
            sInteger /= gPreDefinedPow[sIntDigitCount - 1];
            sIntDigitCount = 1;
        }

        if( sIsNegative == STL_TRUE )
        {
            *sTempBuffer = '-';
            sTempBuffer++;
        }
        else
        {
            /* Do Nothing */
        }

        if( sInteger > 9 )
        {
            sTempBuffer += (sIntDigitCount + 1);
            while( sInteger > 9 )
            {
                sTempBuffer--;
                sTempValue = sInteger / 10;
                *sTempBuffer = sInteger - (sTempValue * 10) + '0';
                sInteger = sTempValue;
            }
            sTempBuffer--;
            *sTempBuffer = '.';
            sTempBuffer--;
            *sTempBuffer = sInteger + '0';
            sTempBuffer += (sIntDigitCount + 1);
        }
        else
        {
            *sTempBuffer = sInteger + '0';
            sTempBuffer++;
        }

        *sTempBuffer = 'E';
        sTempBuffer++;
        *sTempBuffer = (sIsNegativeExp == STL_TRUE ? '-' : '+');
        sTempBuffer++;
        if( sExponentForExpExpression < 10 )
        {
            *sTempBuffer = sExponentForExpExpression + '0';
            sTempBuffer++;
        }
        else if( sExponentForExpExpression < 100 )
        {
            sTempValue = sExponentForExpExpression / 10;
            *sTempBuffer = sTempValue + '0';
            sTempBuffer++;
            *sTempBuffer = sExponentForExpExpression - (sTempValue * 10) + '0';
            sTempBuffer++;
        }
        else
        {
            sTempValue = sExponentForExpExpression / 100;
            *sTempBuffer = sTempValue + '0';
            sTempBuffer++;
            sExponentForExpExpression -= (sTempValue * 100);
            sTempValue = sExponentForExpExpression / 10;
            *sTempBuffer = sTempValue + '0';
            sTempBuffer++;
            *sTempBuffer = sExponentForExpExpression - (sTempValue * 10) + '0';
            sTempBuffer++;
        }

        *aLength = aAvailableSize;
    }
    else
    {
        /* 버퍼 크기가 작음 */
        STL_THROW( ERROR_NOT_ENOUGH_BUFFER );
    }

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;
    
    STL_CATCH( ERROR_NOT_ENOUGH_BUFFER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_NUMBER_WHOLE_DIGITS_GREATER_BUFFER_LENGTH,
                      NULL,
                      aErrorStack );
    } 

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Double value를 사용가능한 버퍼사이즈에 맞게 잘라서 문자열로 변환한다.
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
stlStatus dtdDoubleToStringForAvailableSize( dtlDataValue    * aValue,
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
    stlChar           sDoubleBuffer[DTL_FLOAT64_STRING_SIZE];

    DTL_CHECK_TYPE( DTL_TYPE_NATIVE_DOUBLE, aValue, aErrorStack );
    DTL_PARAM_VALIDATE( aBuffer != NULL, aErrorStack );
    
    if( aAvailableSize >= DTL_FLOAT64_STRING_SIZE )
    {
        STL_TRY( dtdDoubleToString( aValue,
                                    aPrecision,
                                    aScale,
                                    aAvailableSize,
                                    aBuffer,
                                    &sLength,
                                    aVectorFunc,
                                    aVectorArg,
                                    aErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( dtdDoubleToString( aValue,
                                    aPrecision,
                                    aScale,
                                    DTL_FLOAT64_STRING_SIZE,
                                    sDoubleBuffer,
                                    &sLength,
                                    aVectorFunc,
                                    aVectorArg,
                                    aErrorStack )
                 == STL_SUCCESS );
        
        sLength = ( sLength >= aAvailableSize ) ? aAvailableSize : sLength;
        stlMemcpy( aBuffer, sDoubleBuffer, sLength );        
    }
    
    *aLength = sLength;
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Double value를 Exponent 문자열로 변환
 * @param[in]  aValue           dtlDataValue
 * @param[in]  aAvailableSize   aBuffer에 사용할 수 있는 메모리 공간의 크기
 * @param[out] aBuffer          문자열로 변환한 결과가 저장될 메모리 공간
 * @param[out] aLength          변환된 문자열의 길이
 * @param[in]  aVectorFunc      Function Vector
 * @param[in]  aVectorArg       Vector Argument
 * @param[out] aSuccessWithInfo warning 발생 여부 
 * @param[out] aErrorStack      에러 스택
 */
stlStatus dtdDoubleToExponentString( dtlDataValue    * aValue,
                                     stlInt64          aAvailableSize,
                                     void            * aBuffer,
                                     stlInt64        * aLength,
                                     dtlFuncVector   * aVectorFunc,
                                     void            * aVectorArg,
                                     stlBool         * aSuccessWithInfo,
                                     stlErrorStack   * aErrorStack )
{
    dtlDoubleType   sValue;
    stlFloat64      sPow;
    stlBool         sIsNegative;
    stlInt64        sInteger;
    stlInt64        sRemainder;
    stlBool         sIsNegativeExp;
    stlBool         sIsNegativeSignForExpExpression;
    stlInt32        sExponent;
    stlInt32        sExponentForExpExpression;
    stlInt64        sTotDigitCount;
    stlInt64        sIntDigitCount;
    stlInt32        sExpLength;
    stlInt32        sMinExpLength;
    stlInt64        sTempValue;
    stlChar       * sTempBuffer;

    *aSuccessWithInfo = STL_FALSE;

    sValue = *((dtlDoubleType*) aValue->mValue);

    if( stlIsfinite(sValue) == STL_IS_FINITE_FALSE )
    {
        STL_TRY_THROW( aAvailableSize >= DTL_BINARY_REAL_INFINITY_SIZE, ERROR_NOT_ENOUGH_BUFFER );
        stlMemcpy( (stlChar*)aBuffer, DTL_BINARY_REAL_INFINITY, DTL_BINARY_REAL_INFINITY_SIZE );
        *aLength = DTL_BINARY_REAL_INFINITY_SIZE;
        STL_THROW( RAMP_SUCCESS );
    }

    STL_TRY_THROW( aAvailableSize > 0, ERROR_NOT_ENOUGH_BUFFER );

    if( sValue == 0 )
    {
        *(stlChar*)aBuffer = '0';
        *aLength = 1;
        STL_THROW( RAMP_SUCCESS );
    }

    /* STL_DBL_DIG만큼의 유효숫자(trailing zero는 제거한 상태)와 
     * exponent의 형태로 만든다. */
    if( sValue < 0.0 )
    {
        sIsNegative = STL_TRUE;
        sValue = -sValue;
    }
    else
    {
        sIsNegative = STL_FALSE;
    }

    sTotDigitCount = stlLog10( sValue );
    if( sValue < 1 )
    {
        DTL_IS_VALID( stlGet10Pow( sTotDigitCount, &sPow, aErrorStack ) );
        if( sValue == sPow )
        {
            sTotDigitCount++;
        }
        else
        {
            /* Do Nothing */
        }
    }
    else
    {
        sTotDigitCount++;
    }

    //sIntegralLength = sTotDigitCount + (sIsNegative == STL_TRUE ? 1 : 0);

    sExponent = sTotDigitCount - STL_DBL_DIG;
    if( sExponent > STL_DBL_MAX_10_EXP )
    {
        DTL_IS_VALID( stlGet10Pow( STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sValue = sValue / sPow;
        DTL_IS_VALID( stlGet10Pow( sExponent - STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sInteger = (stlInt64)(sValue / sPow + 0.5);
    }
    else if( sExponent < -STL_DBL_MAX_10_EXP )
    {
        DTL_IS_VALID( stlGet10Pow( -STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sValue = sValue / sPow;
        DTL_IS_VALID( stlGet10Pow( sExponent + STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sInteger = (stlInt64)(sValue / sPow + 0.5);
    }
    else
    {
        DTL_IS_VALID( stlGet10Pow( sExponent, &sPow, aErrorStack ) );
        sInteger = (stlInt64)(sValue / sPow + 0.5);
    }

    DTL_IS_VALID( stlGet10Pow( STL_DBL_DIG - 1, &sPow, aErrorStack ) );
    while( sInteger < (stlInt64)sPow )
    {
        sInteger *= 10;
        sExponent--;
    }

    sIntDigitCount = STL_DBL_DIG;
    while( sInteger % 10 == 0 )
    {
        sExponent++;
        sIntDigitCount--;
        sInteger /= 10;
    }

    if( sIntDigitCount == 0 )
    {
        /* 반올림으로 인해 자리수가 증가한 경우 */
        sIntDigitCount = 1;
    }
    else
    {
        /* Do Nothing */
    }

    if( sExponent < 0 )
    {
        sIsNegativeExp = STL_TRUE;
        sExponent = -sExponent;
    }
    else
    {
        sIsNegativeExp = STL_FALSE;
    }

    /* 지금까지 처리된 결과는 아래와 같다.
     * sIsNegative      sValue의 음수여부
     * sInteger         sValue의 STL_DBL_DIG만큼의 유효숫자에서 
     *                  trailing zero를 제거한 수
     * sIntDigitCount   sInteger의 digit count
     * sIsNegativeExp   exponent의 음수여부
     * sExponent        exponent의 값
     * */

    if( sIsNegativeExp == STL_TRUE )
    {
        if( sExponent <= sIntDigitCount )
        {
            sExponentForExpExpression = (sIntDigitCount == sExponent ? 1 : sIntDigitCount - sExponent - 1);
            sExpLength = sIntDigitCount +
                (sIntDigitCount > 1 ? 1 : 0) /* point */ +
                (sIsNegative == STL_TRUE ? 1 : 0);
            sExpLength += 2; /* exponent + sign */
            sExpLength += (sExponentForExpExpression < 10 ? 1 : (sExponentForExpExpression < 100 ? 2 : 3));
        }
        else
        {
            sExponentForExpExpression = sExponent - sIntDigitCount + 1;
            sExpLength = sIntDigitCount +
                (sIntDigitCount > 1 ? 1 : 0) /* point */ +
                (sIsNegative == STL_TRUE ? 1 : 0);
            sExpLength += 2; /* exponent + sign */
            sExpLength += (sExponentForExpExpression < 10 ? 1 : (sExponentForExpExpression < 100 ? 2 : 3));
        }
    }
    else
    {
        sExponentForExpExpression = sExponent + sIntDigitCount - 1;
        sExpLength = sIntDigitCount + 
            (sIntDigitCount > 1 ? 1 : 0) /* point */ + 
            (sIsNegative == STL_TRUE ? 1 : 0);
        sExpLength += 2; /* exponent + sign */
        sExpLength += (sExponentForExpExpression < 10 ? 1 : (sExponentForExpExpression < 100 ? 2 : 3));
    }

    sMinExpLength = 3/* 1digit + 'E' + exp_sign */ +
        (sIsNegative == STL_TRUE ? 1 : 0) +
        (sExponentForExpExpression < 10 ? 1 : (sExponentForExpExpression < 100 ? 2 : 3));

    if( sIntDigitCount <= sExponent && sIsNegativeExp == STL_TRUE )
    {
        sIsNegativeSignForExpExpression = STL_TRUE;
    }
    else
    {
        sIsNegativeSignForExpExpression = STL_FALSE;
    }

    if( sExpLength <= aAvailableSize )
    {
        /* exponent를 이용한 표현 */
        sTempBuffer = (stlChar*)aBuffer;
        if( sIsNegative == STL_TRUE )
        {
            *sTempBuffer = '-';
            sTempBuffer++;
        }
        else
        {
            /* Do Nothing */
        }

        if( sInteger > 9 )
        {
            sTempBuffer += (sIntDigitCount + 1);
            while( sInteger > 9 )
            {
                sTempBuffer--;
                sTempValue = sInteger / 10;
                *sTempBuffer = sInteger - (sTempValue * 10) + '0';
                sInteger = sTempValue;
            }
            sTempBuffer--;
            *sTempBuffer = '.';
            sTempBuffer--;
            *sTempBuffer = sInteger + '0';
            sTempBuffer += (sIntDigitCount + 1);
        }
        else
        {
            *sTempBuffer = sInteger + '0';
            sTempBuffer++;
        }

        *sTempBuffer = 'E';
        sTempBuffer++;
        *sTempBuffer = (sIsNegativeSignForExpExpression == STL_TRUE ? '-' : '+');
        sTempBuffer++;
        if( sExponentForExpExpression < 10 )
        {
            *sTempBuffer = sExponentForExpExpression + '0';
            sTempBuffer++;
        }
        else if( sExponentForExpExpression < 100 )
        {
            sTempValue = sExponentForExpExpression / 10;
            *sTempBuffer = sTempValue + '0';
            sTempBuffer++;
            *sTempBuffer = sExponentForExpExpression - (sTempValue * 10) + '0';
            sTempBuffer++;
        }
        else
        {
            sTempValue = sExponentForExpExpression / 100;
            *sTempBuffer = sTempValue + '0';
            sTempBuffer++;
            sExponentForExpExpression -= (sTempValue * 100);
            sTempValue = sExponentForExpExpression / 10;
            *sTempBuffer = sTempValue + '0';
            sTempBuffer++;
            *sTempBuffer = sExponentForExpExpression - (sTempValue * 10) + '0';
            sTempBuffer++;
        }

        *aLength = sExpLength;
    }
    else if( sMinExpLength + 1 < aAvailableSize )
    {
        sTempBuffer = (stlChar *)aBuffer;
        
        sRemainder = sInteger % gPreDefinedPow[sIntDigitCount - (aAvailableSize - sMinExpLength)];
        sInteger /= gPreDefinedPow[sIntDigitCount - (aAvailableSize - sMinExpLength)];
        sInteger += ( sRemainder / gPreDefinedPow[sIntDigitCount - (aAvailableSize - sMinExpLength) -1 ] > 4 ? 1 : 0 );
        sIntDigitCount = (aAvailableSize - sMinExpLength);

        if( sInteger == gPreDefinedPow[ sIntDigitCount ] )
        {
            sInteger /= 10;
            if( sIsNegativeSignForExpExpression == STL_TRUE )
            {
                sExponentForExpExpression--;
            }
            else
            {
                sExponentForExpExpression++;
            }

            if( sExponentForExpExpression == 0 )
            {
                sIsNegativeSignForExpExpression = STL_FALSE;
            }
            else
            {
                /* Do Nothing */
            }

            sMinExpLength = 3/* 1digit + 'E' + exp_sign */ +
                (sIsNegative == STL_TRUE ? 1 : 0) +
                (sExponentForExpExpression < 10 ? 1 : (sExponentForExpExpression < 100 ? 2 : 3));

            STL_TRY_THROW( sMinExpLength <= aAvailableSize ,  ERROR_NOT_ENOUGH_BUFFER );
        }
        *aLength = aAvailableSize;

        if( sIsNegative == STL_TRUE )
        {
            *sTempBuffer = '-';
            sTempBuffer++;
        }
        else
        {
            /* Do Nothing */
        }

        if( sInteger > 9 )
        {
            sTempBuffer += (sIntDigitCount + 1);
            while( sInteger > 9 )
            {
                sTempBuffer--;
                sTempValue = sInteger / 10;
                *sTempBuffer = sInteger - (sTempValue * 10) + '0';
                sInteger = sTempValue;
            }
            sTempBuffer--;
            *sTempBuffer = '.';
            sTempBuffer--;
            *sTempBuffer = sInteger + '0';
            sTempBuffer += (sIntDigitCount);

            
            while( *sTempBuffer == '0' || *sTempBuffer == '.' )
            {
                sTempBuffer--;
                (*aLength)--;
                
            }
            sTempBuffer++;
        }
        else
        {
            *sTempBuffer = sInteger + '0';
            sTempBuffer++;
        }

        *sTempBuffer = 'E';
        sTempBuffer++;
        *sTempBuffer = (sIsNegativeSignForExpExpression == STL_TRUE ? '-' : '+');
        sTempBuffer++;
        if( sExponentForExpExpression < 10 )
        {
            *sTempBuffer = sExponentForExpExpression + '0';
            sTempBuffer++;
        }
        else if( sExponentForExpExpression < 100 )
        {
            sTempValue = sExponentForExpExpression / 10;
            *sTempBuffer = sTempValue + '0';
            sTempBuffer++;
            *sTempBuffer = sExponentForExpExpression - (sTempValue * 10) + '0';
            sTempBuffer++;
        }
        else
        {
            sTempValue = sExponentForExpExpression / 100;
            *sTempBuffer = sTempValue + '0';
            sTempBuffer++;
            sExponentForExpExpression -= (sTempValue * 100);
            sTempValue = sExponentForExpExpression / 10;
            *sTempBuffer = sTempValue + '0';
            sTempBuffer++;
            *sTempBuffer = sExponentForExpExpression - (sTempValue * 10) + '0';
            sTempBuffer++;
        }

        STL_THROW( RAMP_RTRUNCATED_FINISH );
    }
    else
    {
        STL_TRY_THROW( sMinExpLength <= aAvailableSize ,  ERROR_NOT_ENOUGH_BUFFER );
        
        sTempBuffer = (stlChar*)aBuffer;

        sRemainder = sInteger % gPreDefinedPow[ sIntDigitCount - 1 ];
        sInteger /= gPreDefinedPow[ sIntDigitCount - 1];
        sInteger += ( sRemainder / gPreDefinedPow[ sIntDigitCount -2 ] > 4 ? 1 : 0 );
        sIntDigitCount = 1;

        if( sInteger == gPreDefinedPow[ sIntDigitCount ] )
        {
            sInteger /= 10;
            if( sIsNegativeSignForExpExpression == STL_TRUE )
            {
                sExponentForExpExpression--;
            }
            else
            {
                sExponentForExpExpression++;
            }

            if( sExponentForExpExpression == 0 )
            {
                sIsNegativeSignForExpExpression = STL_FALSE;
            }
            else
            {
                /* Do Nothing */
            }

            sMinExpLength = 3/* 1digit + 'E' + exp_sign */ +
                (sIsNegative == STL_TRUE ? 1 : 0) +
                (sExponentForExpExpression < 10 ? 1 : (sExponentForExpExpression < 100 ? 2 : 3));

            STL_TRY_THROW( sMinExpLength <= aAvailableSize ,  ERROR_NOT_ENOUGH_BUFFER );
        }
        *aLength = sMinExpLength;

        if( sIsNegative == STL_TRUE )
        {
            *sTempBuffer = '-';
            sTempBuffer++;
        }
        else
        {
            /* Do Nothing */
        }

        if( sInteger > 9 )
        {
            sTempBuffer += (sIntDigitCount + 1);
            while( sInteger > 9 )
            {
                sTempBuffer--;
                sTempValue = sInteger / 10;
                *sTempBuffer = sInteger - (sTempValue * 10) + '0';
                sInteger = sTempValue;
            }
            sTempBuffer--;
            *sTempBuffer = '.';
            sTempBuffer--;
            *sTempBuffer = sInteger + '0';
            sTempBuffer += (sIntDigitCount);

            
            while( *sTempBuffer == '0' || *sTempBuffer == '.' )
            {
                sTempBuffer--;
                (*aLength)--;
                
            }
            sTempBuffer++;
        }
        else
        {
            *sTempBuffer = sInteger + '0';
            sTempBuffer++;
        }

        *sTempBuffer = 'E';
        sTempBuffer++;
        *sTempBuffer = (sIsNegativeSignForExpExpression == STL_TRUE ? '-' : '+');
        sTempBuffer++;
        if( sExponentForExpExpression < 10 )
        {
            *sTempBuffer = sExponentForExpExpression + '0';
            sTempBuffer++;
        }
        else if( sExponentForExpExpression < 100 )
        {
            sTempValue = sExponentForExpExpression / 10;
            *sTempBuffer = sTempValue + '0';
            sTempBuffer++;
            *sTempBuffer = sExponentForExpExpression - (sTempValue * 10) + '0';
            sTempBuffer++;
        }
        else
        {
            sTempValue = sExponentForExpExpression / 100;
            *sTempBuffer = sTempValue + '0';
            sTempBuffer++;
            sExponentForExpExpression -= (sTempValue * 100);
            sTempValue = sExponentForExpExpression / 10;
            *sTempBuffer = sTempValue + '0';
            sTempBuffer++;
            *sTempBuffer = sExponentForExpExpression - (sTempValue * 10) + '0';
            sTempBuffer++;
        }

        STL_THROW( RAMP_RTRUNCATED_FINISH );
    }

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_RAMP( RAMP_RTRUNCATED_FINISH );

    *aSuccessWithInfo = STL_TRUE;

    return STL_SUCCESS;
    
    STL_CATCH( ERROR_NOT_ENOUGH_BUFFER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_NUMBER_WHOLE_DIGITS_GREATER_BUFFER_LENGTH,
                      NULL,
                      aErrorStack );
    } 

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Double value를 Non Truncate Exponent 문자열로 변환
 * @param[in]  aValue           dtlDataValue
 * @param[in]  aAvailableSize   aBuffer에 사용할 수 있는 메모리 공간의 크기
 * @param[out] aBuffer          문자열로 변환한 결과가 저장될 메모리 공간
 * @param[out] aLength          변환된 문자열의 길이
 * @param[in]  aVectorFunc      Function Vector
 * @param[in]  aVectorArg       Vector Argument
 * @param[out] aErrorStack      에러 스택
 */
stlStatus dtdDoubleToNonTruncateExponentString( dtlDataValue    * aValue,
                                                stlInt64          aAvailableSize,
                                                void            * aBuffer,
                                                stlInt64        * aLength,
                                                dtlFuncVector   * aVectorFunc,
                                                void            * aVectorArg,
                                                stlErrorStack   * aErrorStack )
{
    dtlDoubleType   sValue;
    stlFloat64      sPow;
    stlBool         sIsNegative;
    stlInt64        sInteger;
    stlBool         sIsNegativeExp;
    stlBool         sIsNegativeSignForExpExpression;
    stlInt32        sExponent;
    stlInt32        sExponentForExpExpression;
    stlInt64        sTotDigitCount;
    stlInt64        sIntDigitCount;
    stlInt32        sExpLength;
    stlInt64        sTempValue;
    stlChar       * sTempBuffer;

    sValue = *((dtlDoubleType*) aValue->mValue);

    if( stlIsfinite(sValue) == STL_IS_FINITE_FALSE )
    {
        STL_TRY_THROW( aAvailableSize >= DTL_BINARY_REAL_INFINITY_SIZE, ERROR_NOT_ENOUGH_BUFFER );
        stlMemcpy( (stlChar*)aBuffer, DTL_BINARY_REAL_INFINITY, DTL_BINARY_REAL_INFINITY_SIZE );
        *aLength = DTL_BINARY_REAL_INFINITY_SIZE;
        STL_THROW( RAMP_SUCCESS );
    }

    STL_TRY_THROW( aAvailableSize > 0, ERROR_NOT_ENOUGH_BUFFER );

    if( sValue == 0 )
    {
        *(stlChar*)aBuffer = '0';
        *aLength = 1;
        STL_THROW( RAMP_SUCCESS );
    }

    /* STL_DBL_DIG만큼의 유효숫자(trailing zero는 제거한 상태)와 
     * exponent의 형태로 만든다. */
    if( sValue < 0.0 )
    {
        sIsNegative = STL_TRUE;
        sValue = -sValue;
    }
    else
    {
        sIsNegative = STL_FALSE;
    }

    sTotDigitCount = stlLog10( sValue );
    if( sValue < 1 )
    {
        DTL_IS_VALID( stlGet10Pow( sTotDigitCount, &sPow, aErrorStack ) );
        if( sValue == sPow )
        {
            sTotDigitCount++;
        }
        else
        {
            /* Do Nothing */
        }
    }
    else
    {
        sTotDigitCount++;
    }

    //sIntegralLength = sTotDigitCount + (sIsNegative == STL_TRUE ? 1 : 0);

    sExponent = sTotDigitCount - STL_DBL_DIG;
    if( sExponent > STL_DBL_MAX_10_EXP )
    {
        DTL_IS_VALID( stlGet10Pow( STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sValue = sValue / sPow;
        DTL_IS_VALID( stlGet10Pow( sExponent - STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sInteger = (stlInt64)(sValue / sPow + 0.5);
    }
    else if( sExponent < -STL_DBL_MAX_10_EXP )
    {
        DTL_IS_VALID( stlGet10Pow( -STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sValue = sValue / sPow;
        DTL_IS_VALID( stlGet10Pow( sExponent + STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sInteger = (stlInt64)(sValue / sPow + 0.5);
    }
    else
    {
        DTL_IS_VALID( stlGet10Pow( sExponent, &sPow, aErrorStack ) );
        sInteger = (stlInt64)(sValue / sPow + 0.5);
    }

    DTL_IS_VALID( stlGet10Pow( STL_DBL_DIG - 1, &sPow, aErrorStack ) );
    while( sInteger < (stlInt64)sPow )
    {
        sInteger *= 10;
        sExponent--;
    }

    sIntDigitCount = STL_DBL_DIG;
    while( sInteger % 10 == 0 )
    {
        sExponent++;
        sIntDigitCount--;
        sInteger /= 10;
    }

    if( sIntDigitCount == 0 )
    {
        /* 반올림으로 인해 자리수가 증가한 경우 */
        sIntDigitCount = 1;
    }
    else
    {
        /* Do Nothing */
    }

    if( sExponent < 0 )
    {
        sIsNegativeExp = STL_TRUE;
        sExponent = -sExponent;
    }
    else
    {
        sIsNegativeExp = STL_FALSE;
    }

    /* 지금까지 처리된 결과는 아래와 같다.
     * sIsNegative      sValue의 음수여부
     * sInteger         sValue의 STL_DBL_DIG만큼의 유효숫자에서 
     *                  trailing zero를 제거한 수
     * sIntDigitCount   sInteger의 digit count
     * sIsNegativeExp   exponent의 음수여부
     * sExponent        exponent의 값
     * */

    if( sIsNegativeExp == STL_TRUE )
    {
        if( sExponent <= sIntDigitCount )
        {
            sExponentForExpExpression = (sIntDigitCount == sExponent ? 1 : sIntDigitCount - sExponent - 1);
            sExpLength = sIntDigitCount +
                (sIntDigitCount > 1 ? 1 : 0) /* point */ +
                (sIsNegative == STL_TRUE ? 1 : 0);
            sExpLength += 2; /* exponent + sign */
            sExpLength += (sExponentForExpExpression < 10 ? 1 : (sExponentForExpExpression < 100 ? 2 : 3));
        }
        else
        {
            sExponentForExpExpression = sExponent - sIntDigitCount + 1;
            sExpLength = sIntDigitCount +
                (sIntDigitCount > 1 ? 1 : 0) /* point */ +
                (sIsNegative == STL_TRUE ? 1 : 0);
            sExpLength += 2; /* exponent + sign */
            sExpLength += (sExponentForExpExpression < 10 ? 1 : (sExponentForExpExpression < 100 ? 2 : 3));
        }
    }
    else
    {
        sExponentForExpExpression = sExponent + sIntDigitCount - 1;
        sExpLength = sIntDigitCount + 
            (sIntDigitCount > 1 ? 1 : 0) /* point */ + 
            (sIsNegative == STL_TRUE ? 1 : 0);
        sExpLength += 2; /* exponent + sign */
        sExpLength += (sExponentForExpExpression < 10 ? 1 : (sExponentForExpExpression < 100 ? 2 : 3));
    }

    if( sIntDigitCount <= sExponent && sIsNegativeExp == STL_TRUE )
    {
        sIsNegativeSignForExpExpression = STL_TRUE;
    }
    else
    {
        sIsNegativeSignForExpExpression = STL_FALSE;
    }


    if( sExpLength <= aAvailableSize )
    {
        /* exponent를 이용한 표현 */
        sTempBuffer = (stlChar*)aBuffer;
        if( sIsNegative == STL_TRUE )
        {
            *sTempBuffer = '-';
            sTempBuffer++;
        }
        else
        {
            /* Do Nothing */
        }

        if( sInteger > 9 )
        {
            sTempBuffer += (sIntDigitCount + 1);
            while( sInteger > 9 )
            {
                sTempBuffer--;
                sTempValue = sInteger / 10;
                *sTempBuffer = sInteger - (sTempValue * 10) + '0';
                sInteger = sTempValue;
            }
            sTempBuffer--;
            *sTempBuffer = '.';
            sTempBuffer--;
            *sTempBuffer = sInteger + '0';
            sTempBuffer += (sIntDigitCount + 1);
        }
        else
        {
            *sTempBuffer = sInteger + '0';
            sTempBuffer++;
        }

        *sTempBuffer = 'E';
        sTempBuffer++;
        *sTempBuffer = (sIsNegativeSignForExpExpression == STL_TRUE ? '-' : '+');
        sTempBuffer++;
        if( sExponentForExpExpression < 10 )
        {
            *sTempBuffer = sExponentForExpExpression + '0';
            sTempBuffer++;
        }
        else if( sExponentForExpExpression < 100 )
        {
            sTempValue = sExponentForExpExpression / 10;
            *sTempBuffer = sTempValue + '0';
            sTempBuffer++;
            *sTempBuffer = sExponentForExpExpression - (sTempValue * 10) + '0';
            sTempBuffer++;
        }
        else
        {
            sTempValue = sExponentForExpExpression / 100;
            *sTempBuffer = sTempValue + '0';
            sTempBuffer++;
            sExponentForExpExpression -= (sTempValue * 100);
            sTempValue = sExponentForExpExpression / 10;
            *sTempBuffer = sTempValue + '0';
            sTempBuffer++;
            *sTempBuffer = sExponentForExpExpression - (sTempValue * 10) + '0';
            sTempBuffer++;
        }

        *aLength = sExpLength;
    }
    else
    {
        /* 버퍼 크기가 작음 */
        STL_THROW( ERROR_NOT_ENOUGH_BUFFER );
    }

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;
    
    STL_CATCH( ERROR_NOT_ENOUGH_BUFFER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_NUMBER_WHOLE_DIGITS_GREATER_BUFFER_LENGTH,
                      NULL,
                      aErrorStack );
    } 

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
