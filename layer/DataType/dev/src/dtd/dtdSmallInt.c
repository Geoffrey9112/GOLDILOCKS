/*******************************************************************************
 * dtdSmallInt.c
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
 * @file dtdSmallInt.c
 * @brief DataType Layer SmallInt type 정의
 */

#include <dtl.h>

#include <dtdDef.h>
#include <dtdDataType.h>

/**
 * @addtogroup dtlSmallInt
 * @{
 */

/**
 * @brief SmallInt 타입의 length값을 얻음
 * @param[in]  aPrecision  사용하지 않음
 * @param[in]  aStringUnit string length unit (dtlStringLengthUnit 참조) 
 * @param[out] aLength     SmallInt의 길이 값
 * @param[in]  aVectorFunc Function Vector
 * @param[in]  aVectorArg  Vector Argument
 * @param[out] aErrorStack 에러 스택
 */
stlStatus dtdSmallIntGetLength( stlInt64              aPrecision,
                                dtlStringLengthUnit   aStringUnit,
                                stlInt64            * aLength,
                                dtlFuncVector       * aVectorFunc,
                                void                * aVectorArg,
                                stlErrorStack       * aErrorStack )
{
    *aLength = DTL_NATIVE_SMALLINT_SIZE;
    
    return STL_SUCCESS;
}


/**
 * @brief SmallInt 타입의 length값을 얻음 (입력 문자열에 영향을 받지 않음)
 * @param[in]  aPrecision    사용하지 않음
 * @param[in]  aScale        사용하지 않음
 * @param[in]  aStringUnit   string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aString       입력 문자열
 * @param[in]  aStringLength aString의 길이    
 * @param[out] aLength       SmallInt의 길이 값
 * @param[in]  aVectorFunc   Function Vector
 * @param[in]  aVectorArg    Vector Argument
 * @param[out] aErrorStack   에러 스택
 */
stlStatus dtdSmallIntGetLengthFromString( stlInt64              aPrecision,
                                          stlInt64              aScale,
                                          dtlStringLengthUnit   aStringUnit,
                                          stlChar             * aString,
                                          stlInt64              aStringLength,
                                          stlInt64            * aLength,
                                          dtlFuncVector       * aVectorFunc,
                                          void                * aVectorArg,
                                          stlErrorStack       * aErrorStack )
{
    stlInt64           sConvertedValue;

    DTL_PARAM_VALIDATE( aString != NULL, aErrorStack );
    
    /* string value로부터 SmallInt 타입의 value 구성 */
    STL_TRY( dtdToInt64FromString( (stlChar *) aString,
                                   aStringLength,
                                   & sConvertedValue,
                                   aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( (sConvertedValue >= DTL_NATIVE_SMALLINT_MIN) &&
                   (sConvertedValue <= DTL_NATIVE_SMALLINT_MAX),
                   ERROR_OUT_OF_RANGE );

    *aLength = DTL_NATIVE_SMALLINT_SIZE;

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
 * @brief SmallInt 타입의 length값을 얻음 (입력 숫자에 영향을 받지 않음)
 * @param[in]  aPrecision  사용하지 않음
 * @param[in]  aScale      사용하지 않음
 * @param[in]  aInteger    입력 숫자
 * @param[out] aLength     SmallInt의 길이 값
 * @param[out] aErrorStack 에러 스택
 */
stlStatus dtdSmallIntGetLengthFromInteger( stlInt64          aPrecision,
                                           stlInt64          aScale,
                                           stlInt64          aInteger,
                                           stlInt64        * aLength,
                                           stlErrorStack   * aErrorStack )
{
    /* integer value로부터 SmallInt 타입의 value 구성 */
    STL_TRY_THROW( (aInteger >= DTL_NATIVE_SMALLINT_MIN) &&
                   (aInteger <= DTL_NATIVE_SMALLINT_MAX),
                   ERROR_OUT_OF_RANGE );

    *aLength = DTL_NATIVE_SMALLINT_SIZE;
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_NUMERIC_DATA_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief SmallInt 타입의 length값을 얻음 (입력 숫자에 영향을 받지 않음)
 * @param[in]  aPrecision  사용하지 않음
 * @param[in]  aScale      사용하지 않음
 * @param[in]  aReal       입력 숫자
 * @param[out] aLength     SmallInt의 길이 값
 * @param[out] aErrorStack 에러 스택
 */
stlStatus dtdSmallIntGetLengthFromReal( stlInt64          aPrecision,
                                        stlInt64          aScale,
                                        stlFloat64        aReal,
                                        stlInt64        * aLength,
                                        stlErrorStack   * aErrorStack )
{
    /* real value로부터 SmallInt 타입의 value 구성 */
    STL_TRY_THROW( (aReal >= DTL_NATIVE_SMALLINT_MIN) &&
                   (aReal <= DTL_NATIVE_SMALLINT_MAX),
                   ERROR_OUT_OF_RANGE );

    *aLength = DTL_NATIVE_SMALLINT_SIZE;
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_NUMERIC_DATA_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      aErrorStack );
    }
                   
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief string value로부터 SmallInt 타입의 value 구성
 * @param[in]  aString          value에 저장될 입력 문자열
 * @param[in]  aStringLength    aString의 길이   
 * @param[in]  aPrecision       사용하지 않음
 * @param[in]  aScale           사용하지 않음
 * @param[in]  aStringUnit      string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aIntervalIndicator 사용하지 않음.
 *                           <BR> INTERVAL 타입의 Indicator dtlIntervalIndicator 참조 
 * @param[in]  aAvailableSize   mValue에 사용할 수 있는 메모리 공간의 크기  
 * @param[out] aValue           dtlDataValue의 주소
 * @param[out] aSuccessWithInfo warning 발생 여부
 * @param[in]  aSourceVectorFunc  Source Function Vector
 * @param[in]  aSourceVectorArg   Source Vector Argument
 * @param[in]  aDestVectorFunc    Dest Function Vector
 * @param[in]  aDestVectorArg     Dest Vector Argument
 * @param[out] aErrorStack      에러 스택
 */
stlStatus dtdSmallIntSetValueFromString( stlChar              * aString,
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
    stlInt64           sConvertedValue;
    dtlSmallIntType  * sDstValue;

    DTL_CHECK_TYPE( DTL_TYPE_NATIVE_SMALLINT, aValue, aErrorStack );
    DTL_PARAM_VALIDATE( aString != NULL, aErrorStack );
    DTL_PARAM_VALIDATE( aAvailableSize >= DTL_NATIVE_SMALLINT_SIZE, aErrorStack );

    *aSuccessWithInfo = STL_FALSE;
    
    /* string value로부터 SmallInt 타입의 value 구성 */
    STL_TRY( dtdToInt64FromString( (stlChar *) aString,
                                   aStringLength,
                                   & sConvertedValue,
                                   aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( (sConvertedValue >= DTL_NATIVE_SMALLINT_MIN) &&
                   (sConvertedValue <= DTL_NATIVE_SMALLINT_MAX),
                   ERROR_OUT_OF_RANGE );

    sDstValue  = (dtlSmallIntType *) aValue->mValue;
    *sDstValue = (dtlSmallIntType) sConvertedValue;

    aValue->mLength = DTL_NATIVE_SMALLINT_SIZE;

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
 * @brief integer value로부터 SmallInt 타입의 value 구성
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
stlStatus dtdSmallIntSetValueFromInteger( stlInt64               aInteger,
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
    /* integer value로부터 SmallInt 타입의 value 구성 */
    dtlSmallIntType  * sDstValue;

    DTL_CHECK_TYPE( DTL_TYPE_NATIVE_SMALLINT, aValue, aErrorStack );
    DTL_PARAM_VALIDATE( aAvailableSize >= DTL_NATIVE_SMALLINT_SIZE, aErrorStack );
    
    /* string value로부터 SmallInt 타입의 value 구성 */
    STL_TRY_THROW( (aInteger >= DTL_NATIVE_SMALLINT_MIN) &&
                   (aInteger <= DTL_NATIVE_SMALLINT_MAX),
                   ERROR_OUT_OF_RANGE );

    *aSuccessWithInfo = STL_FALSE;

    sDstValue  = (dtlSmallIntType *) aValue->mValue;
    *sDstValue = (dtlSmallIntType) aInteger;

    aValue->mLength = DTL_NATIVE_SMALLINT_SIZE;
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_NUMERIC_DATA_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief real value로부터 SmallInt 타입의 value 구성
 * @param[in]  aReal              value에 저장될 입력 숫자
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
stlStatus dtdSmallIntSetValueFromReal( stlFloat64             aReal,
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
    /* real value로부터 SmallInt 타입의 value 구성 */
    dtlSmallIntType  * sDstValue;

    DTL_CHECK_TYPE( DTL_TYPE_NATIVE_SMALLINT, aValue, aErrorStack );
    DTL_PARAM_VALIDATE( aAvailableSize >= DTL_NATIVE_SMALLINT_SIZE, aErrorStack );
    
    /* string value로부터 SmallInt 타입의 value 구성 */
    STL_TRY_THROW( (aReal >= DTL_NATIVE_SMALLINT_MIN) &&
                   (aReal <= DTL_NATIVE_SMALLINT_MAX),
                   ERROR_OUT_OF_RANGE );

    *aSuccessWithInfo = STL_FALSE;

    sDstValue  = (dtlSmallIntType *) aValue->mValue;
    
    if( aReal >= 0 )
    {
        *sDstValue = (dtlSmallIntType) ( aReal + 0.5 );
    }
    else
    {
        *sDstValue = (dtlSmallIntType) ( aReal - 0.5 );
    }

    aValue->mLength = DTL_NATIVE_SMALLINT_SIZE;
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_NUMERIC_DATA_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
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
stlStatus dtdSmallIntReverseByteOrder( void            * aValue,
                                       stlBool           aIsSameEndian,
                                       stlErrorStack   * aErrorStack )
{
    DTL_PARAM_VALIDATE( aValue != NULL, aErrorStack );
    
    if( aIsSameEndian == STL_FALSE )
    {
        dtdReverseByteOrder( (stlChar*) aValue,
                             DTL_NATIVE_SMALLINT_SIZE,
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
 * @brief SmallInt value를 문자열로 변환
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
stlStatus dtdSmallIntToString( dtlDataValue    * aValue,
                               stlInt64          aPrecision,
                               stlInt64          aScale,
                               stlInt64          aAvailableSize,
                               void            * aBuffer,
                               stlInt64        * aLength,
                               dtlFuncVector   * aVectorFunc,
                               void            * aVectorArg,
                               stlErrorStack   * aErrorStack )
{
    dtlSmallIntType    sValue;
    stlInt64           sLength;
    stlChar            sBuffer[DTL_INT64_STRING_SIZE];

    DTL_CHECK_TYPE( DTL_TYPE_NATIVE_SMALLINT, aValue, aErrorStack );
    DTL_PARAM_VALIDATE( aBuffer != NULL, aErrorStack );
    
    sValue = *((dtlSmallIntType*) aValue->mValue);

    dtdInt64ToString( (stlInt64) sValue,
                      sBuffer,
                      & sLength );

    DTL_PARAM_VALIDATE( aAvailableSize > sLength, aErrorStack );

    stlMemcpy( aBuffer, sBuffer, sLength );

    *aLength = sLength;
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SmallInt value를 사용가능한 버퍼사이즈에 맞게 잘라서 문자열로 변환한다.
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
stlStatus dtdSmallIntToStringForAvailableSize( dtlDataValue    * aValue,
                                               stlInt64          aPrecision,
                                               stlInt64          aScale,
                                               stlInt64          aAvailableSize,
                                               void            * aBuffer,
                                               stlInt64        * aLength,
                                               dtlFuncVector   * aVectorFunc,
                                               void            * aVectorArg,
                                               stlErrorStack   * aErrorStack )
{
    dtlSmallIntType    sValue;
    stlInt64           sLength = 0;
    stlChar            sBuffer[DTL_INT64_STRING_SIZE];

    DTL_CHECK_TYPE( DTL_TYPE_NATIVE_SMALLINT, aValue, aErrorStack );
    DTL_PARAM_VALIDATE( aBuffer != NULL, aErrorStack );
    
    sValue = *((dtlSmallIntType*) aValue->mValue);

    dtdInt64ToString( (stlInt64) sValue,
                      sBuffer,
                      & sLength );

    sLength = ( sLength >= aAvailableSize ) ? aAvailableSize : sLength;
    
    stlMemcpy( aBuffer, sBuffer, sLength );
    
    *aLength = sLength;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} */
