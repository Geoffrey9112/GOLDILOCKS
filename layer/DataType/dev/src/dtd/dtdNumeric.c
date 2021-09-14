/*******************************************************************************
 * dtdNumeric.c
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
 * @file dtdNumeric.c
 * @brief DataType Layer Numeric type 정의
 */

#include <dtl.h>

#include <dtDef.h>
#include <dtfNumeric.h>

/**
 * @addtogroup dtlNumeric
 * @{
 */

/**
 * @brief Numeric 타입의 length값을 얻음
 * @param[in]  aPrecision  Numeric Precision
 * @param[in]  aStringUnit string length unit (dtlStringLengthUnit 참조) 
 * @param[out] aLength     Numeric의 길이 값
 * @param[in]  aVectorFunc Function Vector
 * @param[in]  aVectorArg  Vector Argument
 * @param[out] aErrorStack 에러 스택 
 */
stlStatus dtdNumericGetLength( stlInt64              aPrecision,
                               dtlStringLengthUnit   aStringUnit,
                               stlInt64            * aLength,
                               dtlFuncVector       * aVectorFunc,
                               void                * aVectorArg,
                               stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( ((aPrecision >= DTL_NUMERIC_MIN_PRECISION) &&
                         (aPrecision <= DTL_NUMERIC_MAX_PRECISION)),
                        aErrorStack );
    
    *aLength = DTL_NUMERIC_SIZE( aPrecision );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Float 타입의 length값을 얻음
 * @param[in]  aPrecision  Numeric Precision
 * @param[in]  aStringUnit string length unit (dtlStringLengthUnit 참조) 
 * @param[out] aLength     Numeric의 길이 값
 * @param[in]  aVectorFunc Function Vector
 * @param[in]  aVectorArg  Vector Argument
 * @param[out] aErrorStack 에러 스택 
 */
stlStatus dtdFloatGetLength( stlInt64              aPrecision,
                             dtlStringLengthUnit   aStringUnit,
                             stlInt64            * aLength,
                             dtlFuncVector       * aVectorFunc,
                             void                * aVectorArg,
                             stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( ((aPrecision >= DTL_FLOAT_MIN_PRECISION) &&
                         (aPrecision <= DTL_FLOAT_MAX_PRECISION)),
                        aErrorStack );
    
    *aLength = DTL_NUMERIC_SIZE( dtlBinaryToDecimalPrecision[ aPrecision ] );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Numeric 타입의 length값을 얻음
 * @param[in]  aPrecision    Numeric Precision
 * @param[in]  aScale        Numeric Scale
 * @param[in]  aStringUnit   string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aString       입력 문자열
 * @param[in]  aStringLength aString의 길이    
 * @param[out] aLength       Numeric의 길이 값
 * @param[in]  aVectorFunc   Function Vector
 * @param[in]  aVectorArg    Vector Argument
 * @param[out] aErrorStack   에러 스택
 */
stlStatus dtdNumericGetLengthFromString( stlInt64              aPrecision,
                                         stlInt64              aScale,
                                         dtlStringLengthUnit   aStringUnit,
                                         stlChar             * aString,
                                         stlInt64              aStringLength,
                                         stlInt64            * aLength,
                                         dtlFuncVector       * aVectorFunc,
                                         void                * aVectorArg,
                                         stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( aString != NULL, aErrorStack);
    STL_PARAM_VALIDATE( ((aPrecision >= DTL_NUMERIC_MIN_PRECISION) &&
                         (aPrecision <= DTL_NUMERIC_MAX_PRECISION)),
                        aErrorStack );
    
    STL_TRY( dtdValidateNumericFromString( aString,
                                           aStringLength,
                                           aPrecision,
                                           aErrorStack )
             == STL_SUCCESS );
    
    *aLength = DTL_NUMERIC_SIZE( aPrecision );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Float 타입의 length값을 얻음
 * @param[in]  aPrecision    Numeric Precision
 * @param[in]  aScale        Numeric Scale
 * @param[in]  aStringUnit   string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aString       입력 문자열
 * @param[in]  aStringLength aString의 길이    
 * @param[out] aLength       Numeric의 길이 값
 * @param[in]  aVectorFunc   Function Vector
 * @param[in]  aVectorArg    Vector Argument
 * @param[out] aErrorStack   에러 스택
 */
stlStatus dtdFloatGetLengthFromString( stlInt64              aPrecision,
                                       stlInt64              aScale,
                                       dtlStringLengthUnit   aStringUnit,
                                       stlChar             * aString,
                                       stlInt64              aStringLength,
                                       stlInt64            * aLength,
                                       dtlFuncVector       * aVectorFunc,
                                       void                * aVectorArg,
                                       stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( aString != NULL, aErrorStack);
    STL_PARAM_VALIDATE( ((aPrecision >= DTL_FLOAT_MIN_PRECISION) &&
                         (aPrecision <= DTL_FLOAT_MAX_PRECISION)),
                        aErrorStack );

    STL_TRY( dtdValidateNumericFromString( aString,
                                           aStringLength,
                                           dtlBinaryToDecimalPrecision[ aPrecision ],
                                           aErrorStack )
             == STL_SUCCESS );
    
    *aLength = DTL_NUMERIC_SIZE( dtlBinaryToDecimalPrecision[ aPrecision ] );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Numeric 타입의 length값을 얻음 (입력 숫자에 영향을 받지 않음)
 * @param[in]  aPrecision  Numeric Precision
 * @param[in]  aScale      Numeric Scale
 * @param[in]  aInteger    입력 숫자
 * @param[out] aLength     Numeric의 길이 값
 * @param[out] aErrorStack 에러 스택
 */
stlStatus dtdNumericGetLengthFromInteger( stlInt64          aPrecision,
                                          stlInt64          aScale,
                                          stlInt64          aInteger,
                                          stlInt64        * aLength,
                                          stlErrorStack   * aErrorStack )
{
    STL_PARAM_VALIDATE( ((aPrecision >= DTL_NUMERIC_MIN_PRECISION) &&
                         (aPrecision <= DTL_NUMERIC_MAX_PRECISION)),
                        aErrorStack );
    
    *aLength = DTL_NUMERIC_SIZE( aPrecision );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Float 타입의 length값을 얻음 (입력 숫자에 영향을 받지 않음)
 * @param[in]  aPrecision  Numeric Precision
 * @param[in]  aScale      Numeric Scale
 * @param[in]  aInteger    입력 숫자
 * @param[out] aLength     Numeric의 길이 값
 * @param[out] aErrorStack 에러 스택
 */
stlStatus dtdFloatGetLengthFromInteger( stlInt64          aPrecision,
                                        stlInt64          aScale,
                                        stlInt64          aInteger,
                                        stlInt64        * aLength,
                                        stlErrorStack   * aErrorStack )
{
    STL_PARAM_VALIDATE( ((aPrecision >= DTL_FLOAT_MIN_PRECISION) &&
                         (aPrecision <= DTL_FLOAT_MAX_PRECISION)),
                        aErrorStack );
    
    *aLength = DTL_NUMERIC_SIZE( dtlBinaryToDecimalPrecision[ aPrecision ] );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Numeric 타입의 length값을 얻음 (입력 숫자에 영향을 받지 않음)
 * @param[in]  aPrecision  Numeric Precision
 * @param[in]  aScale      Numeric Scale
 * @param[in]  aReal       입력 숫자
 * @param[out] aLength     Numeric의 길이 값
 * @param[out] aErrorStack 에러 스택
 */
stlStatus dtdNumericGetLengthFromReal( stlInt64          aPrecision,
                                       stlInt64          aScale,
                                       stlFloat64        aReal,
                                       stlInt64        * aLength,
                                       stlErrorStack   * aErrorStack )
{
    STL_PARAM_VALIDATE( ((aPrecision >= DTL_NUMERIC_MIN_PRECISION) &&
                         (aPrecision <= DTL_NUMERIC_MAX_PRECISION)),
                        aErrorStack );
    
    *aLength = DTL_NUMERIC_SIZE( aPrecision );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Float 타입의 length값을 얻음 (입력 숫자에 영향을 받지 않음)
 * @param[in]  aPrecision  Numeric Precision
 * @param[in]  aScale      Numeric Scale
 * @param[in]  aReal       입력 숫자
 * @param[out] aLength     Numeric의 길이 값
 * @param[out] aErrorStack 에러 스택
 */
stlStatus dtdFloatGetLengthFromReal( stlInt64          aPrecision,
                                     stlInt64          aScale,
                                     stlFloat64        aReal,
                                     stlInt64        * aLength,
                                     stlErrorStack   * aErrorStack )
{
    STL_PARAM_VALIDATE( ((aPrecision >= DTL_FLOAT_MIN_PRECISION) &&
                         (aPrecision <= DTL_FLOAT_MAX_PRECISION)),
                        aErrorStack );
    
    *aLength = DTL_NUMERIC_SIZE( dtlBinaryToDecimalPrecision[ aPrecision ] );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief string value로부터 Numeric 타입의 value 구성
 * @param[in]  aString            value에 저장될 입력 문자열
 * @param[in]  aStringLength      aString의 길이   
 * @param[in]  aPrecision         precision
 * @param[in]  aScale             scale
 * @param[in]  aStringUnit      string length unit (dtlStringLengthUnit 참조) 
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
 *
 * string value로부터 채워지지 않은 precision 영역은 0x00으로 채운다.
 */
stlStatus dtdNumericSetValueFromString( stlChar              * aString,
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
    DTL_CHECK_TYPE2( DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, aValue, aErrorStack );
    STL_PARAM_VALIDATE( aString != NULL, aErrorStack);
    STL_PARAM_VALIDATE( ((aPrecision >= DTL_NUMERIC_MIN_PRECISION) &&
                         (aPrecision <= DTL_NUMERIC_MAX_PRECISION)),
                        aErrorStack );
    STL_PARAM_VALIDATE( aAvailableSize >= DTL_NUMERIC_SIZE( aPrecision ),
                        aErrorStack );

    *aSuccessWithInfo = STL_FALSE;
   
    STL_TRY( dtdToNumericFromString( aString,
                                     aStringLength,
                                     aPrecision,
                                     aScale,
                                     aValue,
                                     aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief string value로부터 Numeric 타입의 value 구성
 * @param[in]  aString            value에 저장될 입력 문자열
 * @param[in]  aStringLength      aString의 길이   
 * @param[in]  aPrecision         precision
 * @param[in]  aScale             scale
 * @param[in]  aStringUnit      string length unit (dtlStringLengthUnit 참조) 
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
 *
 * string value로부터 채워지지 않은 precision 영역은 0x00으로 채운다.
 */
stlStatus dtdFloatSetValueFromString( stlChar              * aString,
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
    DTL_CHECK_TYPE( DTL_TYPE_FLOAT, aValue, aErrorStack );
    STL_PARAM_VALIDATE( aString != NULL, aErrorStack);
    STL_PARAM_VALIDATE( ((aPrecision >= DTL_FLOAT_MIN_PRECISION) &&
                         (aPrecision <= DTL_FLOAT_MAX_PRECISION)),
                        aErrorStack );
    STL_PARAM_VALIDATE( aScale == DTL_SCALE_NA, aErrorStack );  
    STL_PARAM_VALIDATE( aAvailableSize >= DTL_NUMERIC_SIZE( dtlBinaryToDecimalPrecision[ aPrecision ] ),
                        aErrorStack );

    *aSuccessWithInfo = STL_FALSE;
   
    STL_TRY( dtdToNumericFromString( aString,
                                     aStringLength,
                                     dtlBinaryToDecimalPrecision[ aPrecision ],
                                     aScale,
                                     aValue,
                                     aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief integer value로부터 Numeric 타입의 value 구성
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
stlStatus dtdNumericSetValueFromInteger( stlInt64               aInteger,
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
    DTL_CHECK_TYPE2( DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, aValue, aErrorStack );
    STL_PARAM_VALIDATE( ((aPrecision >= DTL_NUMERIC_MIN_PRECISION) &&
                         (aPrecision <= DTL_NUMERIC_MAX_PRECISION)),
                        aErrorStack );    

    STL_PARAM_VALIDATE( aAvailableSize >= DTL_NUMERIC_SIZE( aPrecision ),
                        aErrorStack );

    *aSuccessWithInfo = STL_FALSE;

    STL_TRY( dtdToNumericFromInt64( aInteger,
                                    aPrecision,
                                    aScale,
                                    aValue,
                                    aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief integer value로부터 Float 타입의 value 구성
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
stlStatus dtdFloatSetValueFromInteger( stlInt64               aInteger,
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
    DTL_CHECK_TYPE( DTL_TYPE_FLOAT, aValue, aErrorStack );
    STL_PARAM_VALIDATE( ((aPrecision >= DTL_FLOAT_MIN_PRECISION) &&
                         (aPrecision <= DTL_FLOAT_MAX_PRECISION)),
                        aErrorStack );
    STL_PARAM_VALIDATE( aScale == DTL_SCALE_NA, aErrorStack );   
    STL_PARAM_VALIDATE( aAvailableSize >= DTL_NUMERIC_SIZE( dtlBinaryToDecimalPrecision[ aPrecision ] ),
                        aErrorStack );

    *aSuccessWithInfo = STL_FALSE;

    STL_TRY( dtdToNumericFromInt64( aInteger,
                                    dtlBinaryToDecimalPrecision[ aPrecision ],
                                    aScale,
                                    aValue,
                                    aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Real value로부터 Numeric 타입의 value 구성
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
stlStatus dtdNumericSetValueFromReal( stlFloat64             aReal,
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
    * aSuccessWithInfo = STL_FALSE;
    
    return dtdNumericSetValueFromFloat64( aReal, aPrecision, aScale,
                                          aStringUnit, aIntervalIndicator, aAvailableSize,
                                          aValue, aSuccessWithInfo, aVectorFunc,
                                          aVectorArg, aErrorStack );
}




/**
 * @brief Real value로부터 Float 타입의 value 구성
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
stlStatus dtdFloatSetValueFromReal( stlFloat64             aReal,
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
    * aSuccessWithInfo = STL_FALSE;
    
    return dtdFloatSetValueFromFloat64( aReal, aPrecision, aScale,
                                        aStringUnit, aIntervalIndicator, aAvailableSize,
                                        aValue, aSuccessWithInfo, aVectorFunc,
                                        aVectorArg, aErrorStack );
}


/**
 * @brief float32 value로부터 Numeric 타입의 value 구성
 * @param[in]  aFloat32           value에 저장될 입력 숫자
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
stlStatus dtdNumericSetValueFromFloat32( stlFloat32             aFloat32,
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
    DTL_CHECK_TYPE2( DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, aValue, aErrorStack );
    STL_PARAM_VALIDATE( ((aPrecision >= DTL_NUMERIC_MIN_PRECISION) &&
                         (aPrecision <= DTL_NUMERIC_MAX_PRECISION)),
                        aErrorStack );
    STL_PARAM_VALIDATE( aAvailableSize >= DTL_NUMERIC_SIZE( aPrecision ),
                        aErrorStack );

    *aSuccessWithInfo = STL_FALSE;
   
    STL_TRY( dtdToNumericFromFloat32( aFloat32,
                                      aPrecision,
                                      aScale,
                                      aValue,
                                      aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief float32 value로부터 Float 타입의 value 구성
 * @param[in]  aFloat32           value에 저장될 입력 숫자
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
stlStatus dtdFloatSetValueFromFloat32( stlFloat32             aFloat32,
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
    DTL_CHECK_TYPE( DTL_TYPE_FLOAT, aValue, aErrorStack );
    STL_PARAM_VALIDATE( ((aPrecision >= DTL_FLOAT_MIN_PRECISION) &&
                         (aPrecision <= DTL_FLOAT_MAX_PRECISION)),
                        aErrorStack );
    STL_PARAM_VALIDATE( aScale == DTL_SCALE_NA, aErrorStack );    
    STL_PARAM_VALIDATE( aAvailableSize >= DTL_NUMERIC_SIZE( dtlBinaryToDecimalPrecision[ aPrecision ] ),
                        aErrorStack );

    *aSuccessWithInfo = STL_FALSE;
    
    STL_TRY( dtdToNumericFromFloat32( aFloat32,
                                      dtlBinaryToDecimalPrecision[ aPrecision ],
                                      aScale,
                                      aValue,
                                      aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief float64 value로부터 Numeric 타입의 value 구성
 * @param[in]  aFloat64           value에 저장될 입력 숫자
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
stlStatus dtdNumericSetValueFromFloat64( stlFloat64             aFloat64,
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
    DTL_CHECK_TYPE2( DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, aValue, aErrorStack );
    STL_PARAM_VALIDATE( ((aPrecision >= DTL_NUMERIC_MIN_PRECISION) &&
                         (aPrecision <= DTL_NUMERIC_MAX_PRECISION)),
                        aErrorStack );
    STL_PARAM_VALIDATE( aAvailableSize >= DTL_NUMERIC_SIZE( aPrecision ),
                        aErrorStack );

    *aSuccessWithInfo = STL_FALSE;
   
    STL_TRY( dtdToNumericFromFloat64( aFloat64,
                                      aPrecision,
                                      aScale,
                                      aValue,
                                      aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief float64 value로부터 Float 타입의 value 구성
 * @param[in]  aFloat64           value에 저장될 입력 숫자
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
stlStatus dtdFloatSetValueFromFloat64( stlFloat64             aFloat64,
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
    DTL_CHECK_TYPE( DTL_TYPE_FLOAT, aValue, aErrorStack );
    STL_PARAM_VALIDATE( ((aPrecision >= DTL_FLOAT_MIN_PRECISION) &&
                         (aPrecision <= DTL_FLOAT_MAX_PRECISION)),
                        aErrorStack );
    STL_PARAM_VALIDATE( aScale == DTL_SCALE_NA, aErrorStack );    
    STL_PARAM_VALIDATE( aAvailableSize >= DTL_NUMERIC_SIZE( dtlBinaryToDecimalPrecision[ aPrecision ] ),
                        aErrorStack );

    *aSuccessWithInfo = STL_FALSE;
   
    STL_TRY( dtdToNumericFromFloat64( aFloat64,
                                      dtlBinaryToDecimalPrecision[ aPrecision ],
                                      aScale,
                                      aValue,
                                      aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Byte Order 변경 (redirect 함수 아님, 내부 함수)
 * @param[in]  aValue          byte swapping 대상 및 결과 
 * @param[in]  aIsSameEndian   byte swapping 필요 여부
 * @param[out] aErrorStack     에러 스택
 */
stlStatus dtdNumericReverseByteOrder( void            * aValue,
                                      stlBool           aIsSameEndian,
                                      stlErrorStack   * aErrorStack )
{
    return STL_SUCCESS;
}

/**
 * @brief Numeric value를 문자열로 변환
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
stlStatus dtdNumericToString( dtlDataValue    * aValue,
                              stlInt64          aPrecision,
                              stlInt64          aScale,
                              stlInt64          aAvailableSize,
                              void            * aBuffer,
                              stlInt64        * aLength,
                              dtlFuncVector   * aVectorFunc,
                              void            * aVectorArg,
                              stlErrorStack   * aErrorStack )
{
    DTL_CHECK_TYPE2( DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, aValue, aErrorStack );
    STL_PARAM_VALIDATE( aBuffer != NULL, aErrorStack );
    
    STL_TRY( dtdToStringFromNumeric( aValue,
                                     aAvailableSize,
                                     aBuffer,
                                     aLength,
                                     aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Numeric value를 사용가능한 버퍼사이즈에 맞게 잘라서 문자열로 변환한다.
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
stlStatus dtdNumericToStringForAvailableSize( dtlDataValue    * aValue,
                                              stlInt64          aPrecision,
                                              stlInt64          aScale,
                                              stlInt64          aAvailableSize,
                                              void            * aBuffer,
                                              stlInt64        * aLength,
                                              dtlFuncVector   * aVectorFunc,
                                              void            * aVectorArg,
                                              stlErrorStack   * aErrorStack )
{
    stlInt64   sLength = 0;
    stlChar    sNumericBuffer[DTL_NUMERIC_STRING_MAX_SIZE];
    
    DTL_CHECK_TYPE2( DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, aValue, aErrorStack );
    STL_PARAM_VALIDATE( aBuffer != NULL, aErrorStack );

    if( aAvailableSize >= DTL_NUMERIC_STRING_MAX_SIZE )
    {
        STL_TRY( dtdToStringFromNumeric( aValue,
                                         aAvailableSize,
                                         aBuffer,
                                         &sLength,
                                         aErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( dtdToStringFromNumeric( aValue,
                                         DTL_NUMERIC_STRING_MAX_SIZE,
                                         sNumericBuffer,
                                         &sLength,
                                         aErrorStack )
                 == STL_SUCCESS );
        
        sLength = ( sLength >= aAvailableSize ) ? aAvailableSize : sLength;
        stlMemcpy( aBuffer, sNumericBuffer, sLength );        
    }

    *aLength = sLength;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief utf16 string value로부터 Numeric 타입의 value 구성
 * @param[in]  aString            value에 저장될 입력 문자열
 * @param[in]  aStringLength      aString의 길이   
 * @param[in]  aPrecision         precision
 * @param[in]  aScale             scale
 * @param[in]  aStringUnit      string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aIntervalIndicator 사용하지 않음.
 *                           <BR> INTERVAL 타입의 Indicator dtlIntervalIndicator 참조 
 * @param[in]  aAvailableSize     mValue에 사용할 수 있는 메모리 공간의 크기  
 * @param[out] aValue             dtlDataValue의 주소
 * @param[out] aSuccessWithInfo   warning 발생 여부
 * @param[in]  aVectorFunc        Function Vector
 * @param[in]  aVectorArg         Vector Argument
 * @param[out] aErrorStack        에러 스택
 *
 * string value로부터 채워지지 않은 precision 영역은 0x00으로 채운다.
 */
stlStatus dtdNumericSetValueFromUtf16WCharString( void                 * aString,
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
    DTL_CHECK_TYPE2( DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, aValue, aErrorStack );
    STL_PARAM_VALIDATE( aString != NULL, aErrorStack);
    STL_PARAM_VALIDATE( ((aPrecision >= DTL_NUMERIC_MIN_PRECISION) &&
                         (aPrecision <= DTL_NUMERIC_MAX_PRECISION)),
                        aErrorStack );
    STL_PARAM_VALIDATE( aAvailableSize >= DTL_NUMERIC_SIZE( aPrecision ),
                        aErrorStack );

    *aSuccessWithInfo = STL_FALSE;
   
    STL_TRY( dtdToNumericFromUtf16WCharString( (stlUInt16 *)aString,
                                               aStringLength,
                                               aPrecision,
                                               aScale,
                                               aValue,
                                               aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief utf32 string value로부터 Numeric 타입의 value 구성
 * @param[in]  aString            value에 저장될 입력 문자열
 * @param[in]  aStringLength      aString의 길이   
 * @param[in]  aPrecision         precision
 * @param[in]  aScale             scale
 * @param[in]  aStringUnit      string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aIntervalIndicator 사용하지 않음.
 *                           <BR> INTERVAL 타입의 Indicator dtlIntervalIndicator 참조 
 * @param[in]  aAvailableSize     mValue에 사용할 수 있는 메모리 공간의 크기  
 * @param[out] aValue             dtlDataValue의 주소
 * @param[out] aSuccessWithInfo   warning 발생 여부
 * @param[in]  aVectorFunc        Function Vector
 * @param[in]  aVectorArg         Vector Argument
 * @param[out] aErrorStack        에러 스택
 *
 * string value로부터 채워지지 않은 precision 영역은 0x00으로 채운다.
 */
stlStatus dtdNumericSetValueFromUtf32WCharString( void                 * aString,
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
    DTL_CHECK_TYPE2( DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, aValue, aErrorStack );
    STL_PARAM_VALIDATE( aString != NULL, aErrorStack);
    STL_PARAM_VALIDATE( ((aPrecision >= DTL_NUMERIC_MIN_PRECISION) &&
                         (aPrecision <= DTL_NUMERIC_MAX_PRECISION)),
                        aErrorStack );
    STL_PARAM_VALIDATE( aAvailableSize >= DTL_NUMERIC_SIZE( aPrecision ),
                        aErrorStack );

    *aSuccessWithInfo = STL_FALSE;
   
    STL_TRY( dtdToNumericFromUtf32WCharString( (stlUInt32 *)aString,
                                               aStringLength,
                                               aPrecision,
                                               aScale,
                                               aValue,
                                               aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief utf16 string value로부터 Numeric 타입의 value 구성
 * @param[in]  aString            value에 저장될 입력 문자열
 * @param[in]  aStringLength      aString의 길이   
 * @param[in]  aPrecision         precision
 * @param[in]  aScale             scale
 * @param[in]  aStringUnit      string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aIntervalIndicator 사용하지 않음.
 *                           <BR> INTERVAL 타입의 Indicator dtlIntervalIndicator 참조 
 * @param[in]  aAvailableSize     mValue에 사용할 수 있는 메모리 공간의 크기  
 * @param[out] aValue             dtlDataValue의 주소
 * @param[out] aSuccessWithInfo   warning 발생 여부
 * @param[in]  aVectorFunc        Function Vector
 * @param[in]  aVectorArg         Vector Argument
 * @param[out] aErrorStack        에러 스택
 *
 * string value로부터 채워지지 않은 precision 영역은 0x00으로 채운다.
 */
stlStatus dtdFloatSetValueFromUtf16WCharString( void                 * aString,
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
    DTL_CHECK_TYPE( DTL_TYPE_FLOAT, aValue, aErrorStack );
    STL_PARAM_VALIDATE( aString != NULL, aErrorStack);
    STL_PARAM_VALIDATE( ((aPrecision >= DTL_FLOAT_MIN_PRECISION) &&
                         (aPrecision <= DTL_FLOAT_MAX_PRECISION)),
                        aErrorStack );
    STL_PARAM_VALIDATE( aScale == DTL_SCALE_NA, aErrorStack );  
    STL_PARAM_VALIDATE( aAvailableSize >= DTL_NUMERIC_SIZE( dtlBinaryToDecimalPrecision[ aPrecision ] ),
                        aErrorStack );

    *aSuccessWithInfo = STL_FALSE;
   
    STL_TRY( dtdToNumericFromUtf16WCharString( (stlUInt16 *)aString,
                                               aStringLength,
                                               dtlBinaryToDecimalPrecision[ aPrecision ],
                                               aScale,
                                               aValue,
                                               aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief utf32 string value로부터 Numeric 타입의 value 구성
 * @param[in]  aString            value에 저장될 입력 문자열
 * @param[in]  aStringLength      aString의 길이   
 * @param[in]  aPrecision         precision
 * @param[in]  aScale             scale
 * @param[in]  aStringUnit      string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aIntervalIndicator 사용하지 않음.
 *                           <BR> INTERVAL 타입의 Indicator dtlIntervalIndicator 참조 
 * @param[in]  aAvailableSize     mValue에 사용할 수 있는 메모리 공간의 크기  
 * @param[out] aValue             dtlDataValue의 주소
 * @param[out] aSuccessWithInfo   warning 발생 여부
 * @param[in]  aVectorFunc        Function Vector
 * @param[in]  aVectorArg         Vector Argument
 * @param[out] aErrorStack        에러 스택
 *
 * string value로부터 채워지지 않은 precision 영역은 0x00으로 채운다.
 */
stlStatus dtdFloatSetValueFromUtf32WCharString( void                 * aString,
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
    DTL_CHECK_TYPE( DTL_TYPE_FLOAT, aValue, aErrorStack );
    STL_PARAM_VALIDATE( aString != NULL, aErrorStack);
    STL_PARAM_VALIDATE( ((aPrecision >= DTL_FLOAT_MIN_PRECISION) &&
                         (aPrecision <= DTL_FLOAT_MAX_PRECISION)),
                        aErrorStack );  
    STL_PARAM_VALIDATE( aScale == DTL_SCALE_NA, aErrorStack );  
    STL_PARAM_VALIDATE( aAvailableSize >= DTL_NUMERIC_SIZE( dtlBinaryToDecimalPrecision[ aPrecision ] ),
                        aErrorStack );

    *aSuccessWithInfo = STL_FALSE;
   
    STL_TRY( dtdToNumericFromUtf32WCharString( (stlUInt32 *)aString,
                                               aStringLength,
                                               dtlBinaryToDecimalPrecision[ aPrecision ],
                                               aScale,
                                               aValue,
                                               aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */



