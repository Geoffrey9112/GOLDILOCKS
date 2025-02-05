/*******************************************************************************
 * dtdLongvarbinary.c
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
 * @file dtdLongvarbinary.c
 * @brief DataType Layer Longvarbinary type 정의
 */

#include <dtl.h>

#include <dtdDef.h>
#include <dtdDataType.h>

/**
 * @addtogroup dtlLongvarbinary
 * @{
 */

/**
 * @brief Longvarbinary 타입의 length값을 얻음
 * @param[in]  aPrecision  Binary Precision
 * @param[in]  aStringUnit string length unit (dtlStringLengthUnit 참조) 
 * @param[out] aLength     Longvarbinary의 길이 값
 * @param[in]  aVectorFunc Function Vector
 * @param[in]  aVectorArg  Vector Argument
 * @param[out] aErrorStack 에러 스택 
 */
stlStatus dtdLongvarbinaryGetLength( stlInt64              aPrecision,
                                     dtlStringLengthUnit   aStringUnit,
                                     stlInt64            * aLength,
                                     dtlFuncVector       * aVectorFunc,
                                     void                * aVectorArg,
                                     stlErrorStack       * aErrorStack )
{
    *aLength = DTL_LONGVARYING_INIT_STRING_SIZE;
    
    return STL_SUCCESS;
}


/**
 * @brief Longvarbinary 타입의 length값을 얻음
 * @param[in]  aPrecision    Binary Precision
 * @param[in]  aScale        사용하지 않음
 * @param[in]  aStringUnit   string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aString       입력 문자열
 * @param[in]  aStringLength aString의 길이    
 * @param[out] aLength       Longvarbinary의 길이 값
 * @param[in]  aVectorFunc   Function Vector
 * @param[in]  aVectorArg    Vector Argument
 * @param[out] aErrorStack   에러 스택
 *
 * Longvarbinary type에 대한 Input String은 16진수로 표현한다.
 *    => {a-f|A-F|0-9}
 *    
 * 단, input string내에 공백은 존재하지 않아야 한다.
 *
 * character string, 2진수, 8진수 등으로 표현되는 입력 값은
 * explicit conversion function 통해 16진수로 변환 후 수행하도록 한다.
 */
stlStatus dtdLongvarbinaryGetLengthFromString( stlInt64              aPrecision,
                                               stlInt64              aScale,
                                               dtlStringLengthUnit   aStringUnit,
                                               stlChar             * aString,
                                               stlInt64              aStringLength,
                                               stlInt64            * aLength,
                                               dtlFuncVector       * aVectorFunc,
                                               void                * aVectorArg,
                                               stlErrorStack       * aErrorStack )
{
    stlInt64 sLength;

    STL_PARAM_VALIDATE( aString != NULL, aErrorStack);
    /* STL_PARAM_VALIDATE( aPrecision == DTL_LONGVARBINARY_MAX_PRECISION, aErrorStack ); */
    
    STL_TRY( dtdGetLengthOfDecodedHex( aString,
                                       aStringLength,
                                       & sLength,
                                       aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( DTL_LONGVARBINARY_MAX_PRECISION >= sLength, ERROR_OUT_OF_PRECISION );

    *aLength = sLength;
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_PRECISION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_BYTE_LENGTH_GREATER_THAN_COLUMN_LENGTH,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/* /\** */
/*  * @brief Longvarbinary 타입의 length값을 얻음 (입력 숫자에 영향을 받지 않음) */
/*  * @param[in]  aPrecision  Binary Precision */
/*  * @param[in]  aScale      사용하지 않음 */
/*  * @param[in]  aInteger    입력 숫자 */
/*  * @param[out] aLength     Longvarbinary의 길이 값 */
/*  * @param[out] aErrorStack 에러 스택 */
/*  *\/ */
/* stlStatus dtdLongvarbinaryGetLengthFromInteger( stlInt64          aPrecision, */
/*                                                 stlInt64          aScale, */
/*                                                 stlInt64          aInteger, */
/*                                                 stlInt64        * aLength, */
/*                                                 stlErrorStack   * aErrorStack ) */
/* { */
/*     stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                   DTL_ERRCODE_NOT_IMPLEMENTED, */
/*                   NULL, */
/*                   aErrorStack, */
/*                   "dtdLongvarbinaryGetLengthFromInteger()" ); */
    
/*     return STL_FAILURE; */
/* } */

/* /\** */
/*  * @brief Longvarbinary 타입의 byte length값을 얻음.  */
/*  * @param[in]  aPrecision  각 데이타 타입의 precision( 사용하지 않음 ) */
/*  * @param[in]  aScale      scale 정보(사용하지 않음) */
/*  * @param[in]  aReal       입력 숫자( 사용하지 않음 ) */
/*  * @param[out] aLength     Longvarbinary의 byte length */
/*  * @param[out] aErrorStack 에러 스택 */
/*  *\/ */
/* stlStatus dtdLongvarbinaryGetLengthFromReal( stlInt64          aPrecision, */
/*                                              stlInt64          aScale, */
/*                                              stlFloat64        aReal,  */
/*                                              stlInt64        * aLength, */
/*                                              stlErrorStack   * aErrorStack ) */
/* { */
/*     stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                   DTL_ERRCODE_NOT_IMPLEMENTED, */
/*                   NULL, */
/*                   aErrorStack, */
/*                   "dtdLongvarbinaryGetLengthFromReal()" ); */

/*     return STL_FAILURE; */
/* } */

/**
 * @brief string value로부터 Longvarbinary 타입의 value 구성
 * @param[in]  aString            value에 저장될 입력 문자열
 * @param[in]  aStringLength      aString의 길이   
 * @param[in]  aPrecision         Binary Precision
 * @param[in]  aScale             사용하지 않음
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
 */
stlStatus dtdLongvarbinarySetValueFromString( stlChar              * aString,
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
    stlInt64          sLength;
    
    DTL_CHECK_TYPE( DTL_TYPE_LONGVARBINARY, aValue, aErrorStack );
    STL_PARAM_VALIDATE( aString != NULL, aErrorStack );
    /* STL_PARAM_VALIDATE( aPrecision == DTL_LONGVARBINARY_MAX_PRECISION, aErrorStack ); */
    
    STL_TRY_THROW( DTL_LONGVARBINARY_MAX_PRECISION >= aStringLength,
                   ERROR_INVALID_INPUT_DATA_LENGTH );

    *aSuccessWithInfo = STL_FALSE;
    
    /**
     * 메모리 확장이 필요한지 검사한다.
     */

    if( aStringLength > aValue->mBufferSize )
    {
        STL_TRY( aDestVectorFunc->mReallocLongVaryingMemFunc( aDestVectorArg,
                                                              aStringLength,
                                                              &aValue->mValue,
                                                              aErrorStack )
                 == STL_SUCCESS );

        aValue->mBufferSize = aStringLength;
    }

    STL_TRY( dtdToBinaryStringFromString( aString,
                                          aStringLength,
                                          aValue->mBufferSize,
                                          (stlInt8 *) aValue->mValue,
                                          (stlInt64 *) & sLength,
                                          aErrorStack )
             == STL_SUCCESS );

    aValue->mLength = sLength;
                   
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_INVALID_INPUT_DATA_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_BYTE_LENGTH_GREATER_THAN_COLUMN_LENGTH,
                      NULL,
                      aErrorStack );
    }
                   
    STL_FINISH;

    return STL_FAILURE;
}

/* /\** */
/*  * @brief integer value로부터 Longvarbinary 타입의 value 구성 */
/*  * @param[in]  aInteger           value에 저장될 입력 숫자 */
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
/* stlStatus dtdLongvarbinarySetValueFromInteger( stlInt64               aInteger, */
/*                                                stlInt64               aPrecision, */
/*                                                stlInt64               aScale, */
/*                                                dtlStringLengthUnit    aStringUnit, */
/*                                                dtlIntervalIndicator   aIntervalIndicator, */
/*                                                stlInt64               aAvailableSize, */
/*                                                dtlDataValue         * aValue, */
/*                                                stlBool              * aSuccessWithInfo, */
/*                                                dtlFuncVector        * aVectorFunc, */
/*                                                void                 * aVectorArg, */
/*                                                stlErrorStack        * aErrorStack ) */
/* { */
/*     stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                   DTL_ERRCODE_CONVERSION_NOT_APPLICABLE, */
/*                   NULL, */
/*                   aErrorStack, */
/*                   "numeric data types", */
/*                   "LONGVARBINARY" ); */

/*     return STL_FAILURE; */
/* } */

/* /\** */
/*  * @brief real value로부터 Longvarbinary 타입의 value 구성 */
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
/* stlStatus dtdLongvarbinarySetValueFromReal( stlFloat64             aReal, */
/*                                             stlInt64               aPrecision, */
/*                                             stlInt64               aScale, */
/*                                             dtlStringLengthUnit    aStringUnit, */
/*                                             dtlIntervalIndicator   aIntervalIndicator, */
/*                                             stlInt64               aAvailableSize, */
/*                                             dtlDataValue         * aValue, */
/*                                             stlBool              * aSuccessWithInfo, */
/*                                             dtlFuncVector        * aVectorFunc, */
/*                                             void                 * aVectorArg, */
/*                                             stlErrorStack        * aErrorStack ) */
/* { */
/*     stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                   DTL_ERRCODE_CONVERSION_NOT_APPLICABLE, */
/*                   NULL, */
/*                   aErrorStack, */
/*                   "numeric data types", */
/*                   "LONGVARBINARY" ); */

/*     return STL_FAILURE; */
/* } */

/**
 * @brief Byte Order 변경 (redirect 함수 아님, 내부 함수)
 * @param[in]  aValue          byte swapping 대상 및 결과 
 * @param[in]  aIsSameEndian   byte swapping 필요 여부
 * @param[out] aErrorStack     에러 스택
 */
stlStatus dtdLongvarbinaryReverseByteOrder( void            * aValue,
                                            stlBool           aIsSameEndian,
                                            stlErrorStack   * aErrorStack )
{
    /* Do Nothing */
    
    return STL_SUCCESS;
}

/**
 * @brief Longvarbinary value를 문자열로 변환
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
stlStatus dtdLongvarbinaryToString( dtlDataValue    * aValue,
                                    stlInt64          aPrecision,
                                    stlInt64          aScale,
                                    stlInt64          aAvailableSize,
                                    void            * aBuffer,
                                    stlInt64        * aLength,
                                    dtlFuncVector   * aVectorFunc,
                                    void            * aVectorArg,
                                    stlErrorStack   * aErrorStack )
{
    DTL_CHECK_TYPE( DTL_TYPE_LONGVARBINARY, aValue, aErrorStack );
    
    STL_TRY( dtdToStringFromBinaryString( (stlInt8*) aValue->mValue,
                                          aValue->mLength,
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
 * @brief Long varbinary value를 사용가능한 버퍼사이즈에 맞게 잘라서 문자열로 변환한다.
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
stlStatus dtdLongvarbinaryToStringForAvailableSize( dtlDataValue    * aValue,
                                                    stlInt64          aPrecision,
                                                    stlInt64          aScale,
                                                    stlInt64          aAvailableSize,
                                                    void            * aBuffer,
                                                    stlInt64        * aLength,
                                                    dtlFuncVector   * aVectorFunc,
                                                    void            * aVectorArg,
                                                    stlErrorStack   * aErrorStack )
{
    DTL_CHECK_TYPE( DTL_TYPE_LONGVARBINARY, aValue, aErrorStack );

    if( aAvailableSize >= (aValue->mLength * 2) )
    {
        STL_TRY( dtdToStringFromBinaryString( (stlInt8*) aValue->mValue,
                                              aValue->mLength,
                                              aAvailableSize,
                                              aBuffer,
                                              aLength,
                                              aErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( dtdToStringFromBinaryString( (stlInt8*) aValue->mValue,
                                              (aAvailableSize / 2),
                                              aAvailableSize,
                                              aBuffer,
                                              aLength,
                                              aErrorStack )
                 == STL_SUCCESS );        
    }
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief utf16 string value로부터 Longvarbinary 타입의 value 구성
 * @param[in]  aString            value에 저장될 입력 문자열 ( utf16 string )
 * @param[in]  aStringLength      aString의 길이   
 * @param[in]  aPrecision         Binary Precision
 * @param[in]  aScale             사용하지 않음
 * @param[in]  aStringUnit      string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aIntervalIndicator 사용하지 않음.
 *                           <BR> INTERVAL 타입의 Indicator dtlIntervalIndicator 참조 
 * @param[in]  aAvailableSize     mValue에 사용할 수 있는 메모리 공간의 크기  
 * @param[out] aValue             dtlDataValue의 주소
 * @param[out] aSuccessWithInfo   warning 발생 여부
 * @param[in]  aVectorFunc        Function Vector
 * @param[in]  aVectorArg         Vector Argument
 * @param[out] aErrorStack        에러 스택
 */
stlStatus dtdLongvarbinarySetValueFromUtf16WCharString( void                 * aString,
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
    stlInt64          sLength;
    
    DTL_CHECK_TYPE( DTL_TYPE_LONGVARBINARY, aValue, aErrorStack );
    STL_PARAM_VALIDATE( aString != NULL, aErrorStack );
    /* STL_PARAM_VALIDATE( aPrecision == DTL_LONGVARBINARY_MAX_PRECISION, aErrorStack ); */
    
    STL_TRY_THROW( DTL_LONGVARBINARY_MAX_PRECISION >= aStringLength,
                   ERROR_INVALID_INPUT_DATA_LENGTH );

    *aSuccessWithInfo = STL_FALSE;
    
    /**
     * 메모리 확장이 필요한지 검사한다.
     */

    if( aStringLength > aValue->mBufferSize )
    {
        STL_TRY( aVectorFunc->mReallocLongVaryingMemFunc( aVectorArg,
                                                          aStringLength,
                                                          &aValue->mValue,
                                                          aErrorStack )
                 == STL_SUCCESS );

        aValue->mBufferSize = aStringLength;
    }

    STL_TRY( dtdToBinaryStringFromUtf16WCharString( (stlUInt16 *)aString,
                                                    aStringLength,
                                                    aValue->mBufferSize,
                                                    (stlInt8 *) aValue->mValue,
                                                    (stlInt64 *) & sLength,
                                                    aErrorStack )
             == STL_SUCCESS );

    aValue->mLength = sLength;
                   
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_INVALID_INPUT_DATA_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_BYTE_LENGTH_GREATER_THAN_COLUMN_LENGTH,
                      NULL,
                      aErrorStack );
    }
                   
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief utf32 string value로부터 Longvarbinary 타입의 value 구성
 * @param[in]  aString            value에 저장될 입력 문자열 ( utf32 string )
 * @param[in]  aStringLength      aString의 길이   
 * @param[in]  aPrecision         Binary Precision
 * @param[in]  aScale             사용하지 않음
 * @param[in]  aStringUnit      string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aIntervalIndicator 사용하지 않음.
 *                           <BR> INTERVAL 타입의 Indicator dtlIntervalIndicator 참조 
 * @param[in]  aAvailableSize     mValue에 사용할 수 있는 메모리 공간의 크기  
 * @param[out] aValue             dtlDataValue의 주소
 * @param[out] aSuccessWithInfo   warning 발생 여부
 * @param[in]  aVectorFunc        Function Vector
 * @param[in]  aVectorArg         Vector Argument
 * @param[out] aErrorStack        에러 스택
 */
stlStatus dtdLongvarbinarySetValueFromUtf32WCharString( void                 * aString,
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
    stlInt64          sLength;
    
    DTL_CHECK_TYPE( DTL_TYPE_LONGVARBINARY, aValue, aErrorStack );
    STL_PARAM_VALIDATE( aString != NULL, aErrorStack );
    /* STL_PARAM_VALIDATE( aPrecision == DTL_LONGVARBINARY_MAX_PRECISION, aErrorStack ); */
    
    STL_TRY_THROW( DTL_LONGVARBINARY_MAX_PRECISION >= aStringLength,
                   ERROR_INVALID_INPUT_DATA_LENGTH );

    *aSuccessWithInfo = STL_FALSE;
    
    /**
     * 메모리 확장이 필요한지 검사한다.
     */

    if( aStringLength > aValue->mBufferSize )
    {
        STL_TRY( aVectorFunc->mReallocLongVaryingMemFunc( aVectorArg,
                                                          aStringLength,
                                                          &aValue->mValue,
                                                          aErrorStack )
                 == STL_SUCCESS );

        aValue->mBufferSize = aStringLength;
    }

    STL_TRY( dtdToBinaryStringFromUtf32WCharString( (stlUInt32 *)aString,
                                                    aStringLength,
                                                    aValue->mBufferSize,
                                                    (stlInt8 *) aValue->mValue,
                                                    (stlInt64 *) & sLength,
                                                    aErrorStack )
             == STL_SUCCESS );

    aValue->mLength = sLength;
                   
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_INVALID_INPUT_DATA_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_BYTE_LENGTH_GREATER_THAN_COLUMN_LENGTH,
                      NULL,
                      aErrorStack );
    }
                   
    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
