/*******************************************************************************
 * dtdLongvarchar.c
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
 * @file dtdLongvarchar.c
 * @brief DataType Layer Longvarchar type 정의
 */

#include <dtl.h>

#include <dtdDef.h>
#include <dtdDataType.h>
#include <dtdLongvarchar.h>
#include <dtsCharacterSet.h>

/**
 * @addtogroup dtlLongvarchar
 * @{
 */

/**
 * @brief Longvarchar 타입의 byte length 얻어오기 (dtlDataValue의 mValue의 크기) 
 * @param[in]  aPrecision  각 데이타 타입의 precision
 * @param[in]  aStringUnit string length unit (dtlStringLengthUnit 참조) 
 * @param[out] aLength     value의 byte length
 * @param[in]  aVectorFunc Function Vector
 * @param[in]  aVectorArg  Vector Argument
 * @param[out] aErrorStack 에러 스택
 */
stlStatus dtdLongvarcharGetLength( stlInt64              aPrecision,
                                   dtlStringLengthUnit   aStringUnit,
                                   stlInt64            * aLength,
                                   dtlFuncVector       * aVectorFunc,
                                   void                * aVectorArg,
                                   stlErrorStack       * aErrorStack )
{
    STL_DASSERT( aStringUnit == DTL_STRING_LENGTH_UNIT_OCTETS );
    
    *aLength = DTL_LONGVARYING_INIT_STRING_SIZE;
    
    return STL_SUCCESS;
}


/**
 * @brief Longvarchar 타입의 byte length 얻어오기
 *        <BR> 입력문자열(aString)의 byte length를 얻는다.
 * @param[in]  aPrecision    각 데이타 타입의 precision
 * @param[in]  aScale        scale 정보
 * @param[in]  aStringUnit   string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aString       입력 문자열
 * @param[in]  aStringLength aString의 길이    
 * @param[out] aLength       Char의 byte length
 * @param[in]  aVectorFunc   Function Vector
 * @param[in]  aVectorArg    Vector Argument
 * @param[out] aErrorStack 에러 스택
 */
stlStatus dtdLongvarcharGetLengthFromString( stlInt64              aPrecision,
                                             stlInt64              aScale,
                                             dtlStringLengthUnit   aStringUnit,
                                             stlChar             * aString,
                                             stlInt64              aStringLength,
                                             stlInt64            * aLength,
                                             dtlFuncVector       * aVectorFunc,
                                             void                * aVectorArg,
                                             stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( aString != NULL, aErrorStack );
    /* DTL_PARAM_VALIDATE( aPrecision == DTL_LONGVARCHAR_MAX_PRECISION, aErrorStack ); */
    STL_PARAM_VALIDATE( aStringUnit == DTL_STRING_LENGTH_UNIT_OCTETS, aErrorStack );
    
    /**
     *  aString의 precision > aPrecision 이면, 에러상황
     */
    STL_TRY_THROW( (DTL_LONGVARCHAR_MAX_PRECISION >= aStringLength),
                   ERRCODE_STRING_DATA_RIGHT_TRUNCATION );

    *aLength = aStringLength;

    return STL_SUCCESS;

    STL_CATCH( ERRCODE_STRING_DATA_RIGHT_TRUNCATION )
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
/*  * @brief Longvarchar 타입의 byte length값을 얻음.  */
/*  * @param[in]  aPrecision  각 데이타 타입의 precision( 사용하지 않음 ) */
/*  * @param[in]  aScale      scale 정보(사용하지 않음) */
/*  * @param[in]  aInteger    입력 숫자( 사용하지 않음 ) */
/*  * @param[out] aLength     Longvarchar의 byte length */
/*  * @param[out] aErrorStack 에러 스택 */
/*  *\/ */
/* stlStatus dtdLongvarcharGetLengthFromInteger(  stlInt64          aPrecision, */
/*                                                stlInt64          aScale, */
/*                                                stlInt64          aInteger,  */
/*                                                stlInt64        * aLength, */
/*                                                stlErrorStack   * aErrorStack ) */
/* { */
/*     /\** */
/*      * Longvarchar 타입의 경우, Integer로부터 Longvarchar length를 구하는 경우는 없음. */
/*      * 이전에 이미 function이나 casting을 통해 string으로 처리되는 과정을 거쳤을것임. */
/*      *\/ */
    
/*     stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                   DTL_ERRCODE_NOT_IMPLEMENTED, */
/*                   NULL, */
/*                   aErrorStack, */
/*                   "dtdLongvarcharGetLengthFromInteger()" ); */

/*     return STL_FAILURE; */
/* } */

/* /\** */
/*  * @brief Longvarchar 타입의 byte length값을 얻음.  */
/*  * @param[in]  aPrecision  각 데이타 타입의 precision( 사용하지 않음 ) */
/*  * @param[in]  aScale      scale 정보(사용하지 않음) */
/*  * @param[in]  aReal       입력 숫자( 사용하지 않음 ) */
/*  * @param[out] aLength     Longvarchar의 byte length */
/*  * @param[out] aErrorStack 에러 스택 */
/*  *\/ */
/* stlStatus dtdLongvarcharGetLengthFromReal( stlInt64          aPrecision, */
/*                                            stlInt64          aScale, */
/*                                            stlFloat64        aReal,  */
/*                                            stlInt64        * aLength, */
/*                                            stlErrorStack   * aErrorStack ) */
/* { */
/*     /\** */
/*      * Longvarchar 타입의 경우, Real로부터 Longvarchar length를 구하는 경우는 없음. */
/*      * 이전에 이미 function이나 casting을 통해 string으로 처리되는 과정을 거쳤을것임. */
/*      *\/ */
    
/*     stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                   DTL_ERRCODE_NOT_IMPLEMENTED, */
/*                   NULL, */
/*                   aErrorStack, */
/*                   "dtdLongvarcharGetLengthFromReal()" ); */

/*     return STL_FAILURE; */
/* } */

/**
 * @brief string value로부터 Longvarchar 타입의 value 구성
 *        <BR> value가 null인 경우는 이 함수를 거치지 않고, 외부에서 처리된다.
 * @param[in]  aString          value에 저장될 문자열
 * @param[in]  aStringLength    aString의 길이   
 * @param[in]  aPrecision       precision
 * @param[in]  aScale           scale 정보
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
stlStatus dtdLongvarcharSetValueFromString( stlChar              * aString,
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
    stlInt64        sOffset;
    stlInt64        sMaxLength = 0;
    stlInt64        sDestStringLength;
    dtlCharacterSet sSourceCharacterSet;
    dtlCharacterSet sDestCharacterSet;

    DTL_CHECK_TYPE( DTL_TYPE_LONGVARCHAR, aValue, aErrorStack );
    STL_PARAM_VALIDATE( aString != NULL, aErrorStack );
    /* DTL_PARAM_VALIDATE( aPrecision == DTL_LONGVARCHAR_MAX_PRECISION, aErrorStack ); */
    STL_PARAM_VALIDATE( aStringUnit == DTL_STRING_LENGTH_UNIT_OCTETS, aErrorStack );

    /**
     * (1) (aString의 precision) > aPrecision 이면, 에러상황
     */
    
    STL_TRY_THROW( DTL_LONGVARCHAR_MAX_PRECISION >= aStringLength,
                   ERRCODE_STRING_DATA_RIGHT_TRUNCATION );

    *aSuccessWithInfo = STL_FALSE;

    sSourceCharacterSet = aSourceVectorFunc->mGetCharSetIDFunc(aSourceVectorArg);
    sDestCharacterSet   = aDestVectorFunc->mGetCharSetIDFunc(aDestVectorArg);
    
    /**
     * (2) 메모리 확장이 필요한지 검사한다.
     */

    /*
     * source code 전체가 ascii 인 경우라면, sMaxLength == aStringLength 이겠지만,
     * multi byte인 경우, 최소단위인 2byte로 구성된 글자로 가정하고 sMaxLength룰 예측한다.
     */
    sMaxLength = ( aStringLength / 2 + 1 ) * ( dtlGetMbMaxLength(sDestCharacterSet) );
    sMaxLength = STL_MIN( sMaxLength, DTL_LONGVARCHAR_MAX_PRECISION );

    if( sMaxLength > aValue->mBufferSize )
    {
        STL_TRY( aDestVectorFunc->mReallocLongVaryingMemFunc( aDestVectorArg,
                                                              sMaxLength,
                                                              &aValue->mValue,
                                                              aErrorStack )
                 == STL_SUCCESS );

        aValue->mBufferSize = sMaxLength;
    }

    /**
     * (3) aValue->mValue에 aString으로 value 구성.
     */
    
    if( sSourceCharacterSet == sDestCharacterSet )
    {
        stlMemcpy( aValue->mValue, aString, aStringLength );
        sDestStringLength = aStringLength;
    }
    else
    {
        STL_TRY( dtlMbConversion( sSourceCharacterSet,
                                  sDestCharacterSet,
                                  aString,
                                  aStringLength,
                                  aValue->mValue,
                                  aValue->mBufferSize,
                                  &sOffset,
                                  &sDestStringLength,
                                  aErrorStack ) == STL_SUCCESS );

        STL_TRY_THROW( sDestStringLength <= aValue->mBufferSize,
                       ERRCODE_STRING_DATA_RIGHT_TRUNCATION );
        STL_TRY_THROW( sDestStringLength <= DTL_LONGVARCHAR_MAX_PRECISION,
                       ERRCODE_STRING_DATA_RIGHT_TRUNCATION );
    }
    
    /**
     *  (4) 최종 byte length를 구함.
     */
    
    aValue->mLength = sDestStringLength;

    return STL_SUCCESS;

    STL_CATCH( ERRCODE_STRING_DATA_RIGHT_TRUNCATION )
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
 * @brief integer value로부터 Longvarchar 타입의 value 구성
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
stlStatus dtdLongvarcharSetValueFromInteger( stlInt64               aInteger,
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
    stlInt64      sLength;

    DTL_CHECK_TYPE( DTL_TYPE_LONGVARCHAR, aValue, aErrorStack );
    /* DTL_PARAM_VALIDATE( aPrecision == DTL_LONGVARCHAR_MAX_PRECISION, aErrorStack ); */
    STL_PARAM_VALIDATE( aStringUnit == DTL_STRING_LENGTH_UNIT_OCTETS, aErrorStack );

    dtdInt64ToString( aInteger,
                      (stlChar *)(aValue->mValue),
                      & sLength );

    STL_TRY_THROW( DTL_LONGVARCHAR_MAX_PRECISION >= sLength,
                   ERRCODE_STRING_DATA_RIGHT_TRUNCATION );

    STL_DASSERT( aValue->mBufferSize >= sLength );

    *aSuccessWithInfo = STL_FALSE;

    aValue->mLength = sLength;

    return STL_SUCCESS;

    STL_CATCH( ERRCODE_STRING_DATA_RIGHT_TRUNCATION )
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
 * @brief real value로부터 Longvarchar 타입의 value 구성
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
stlStatus dtdLongvarcharSetValueFromReal( stlFloat64             aReal,
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
    stlInt32     sLength;
    stlChar      sBuffer[DTL_FLOAT64_STRING_SIZE];

    DTL_CHECK_TYPE( DTL_TYPE_LONGVARCHAR, aValue, aErrorStack );
    /* DTL_PARAM_VALIDATE( aPrecision == DTL_LONGVARCHAR_MAX_PRECISION, aErrorStack ); */
    STL_PARAM_VALIDATE( aStringUnit == DTL_STRING_LENGTH_UNIT_OCTETS, aErrorStack );

    *aSuccessWithInfo = STL_FALSE;

    sLength = stlSnprintf( sBuffer,
                           DTL_FLOAT64_STRING_SIZE,
                           "%.*G",
                           STL_DBL_DIG,
                           aReal );

    STL_TRY_THROW( DTL_LONGVARCHAR_MAX_PRECISION >= sLength,
                   ERRCODE_STRING_DATA_RIGHT_TRUNCATION );

    STL_DASSERT( aValue->mBufferSize >= sLength );

    stlMemcpy( aValue->mValue, sBuffer, sLength );    

    aValue->mLength = sLength;

    return STL_SUCCESS;

    STL_CATCH( ERRCODE_STRING_DATA_RIGHT_TRUNCATION )
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
 * @brief Byte Order 변경 (redirect 함수 아님, 내부 함수)
 * @param[in]  aValue          byte swapping 대상
 * @param[in]  aIsSameEndian   byte swapping 필요 여부
 * @param[out] aErrorStack     에러 스택
 */
stlStatus dtdLongvarcharReverseByteOrder( void            * aValue,
                                          stlBool           aIsSameEndian,
                                          stlErrorStack   * aErrorStack )
{
    /**
     * @todo multi-byte character set인 경우,
     *       Byte Order에 대한 고려가 정말 필요없는지 확인이 필요함. 
     */
    
    /* Do Nothing */
    return STL_SUCCESS;
}

/**
 * @brief Longvarchar value를 문자열로 변환
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
stlStatus dtdLongvarcharToString( dtlDataValue    * aValue,
                                  stlInt64          aPrecision,
                                  stlInt64          aScale,
                                  stlInt64          aAvailableSize,
                                  void            * aBuffer,
                                  stlInt64        * aLength,
                                  dtlFuncVector   * aVectorFunc,
                                  void            * aVectorArg,
                                  stlErrorStack   * aErrorStack )
{
    DTL_CHECK_TYPE( DTL_TYPE_LONGVARCHAR, aValue, aErrorStack );
    STL_PARAM_VALIDATE( aAvailableSize >= aValue->mLength, aErrorStack );
    STL_PARAM_VALIDATE( aBuffer != NULL, aErrorStack );
    
    stlMemcpy( aBuffer, aValue->mValue, aValue->mLength );
    *aLength = aValue->mLength;

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Long varchar value를 사용가능한 버퍼사이즈에 맞게 잘라서 문자열로 변환한다.
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
stlStatus dtdLongvarcharToStringForAvailableSize( dtlDataValue    * aValue,
                                                  stlInt64          aPrecision,
                                                  stlInt64          aScale,
                                                  stlInt64          aAvailableSize,
                                                  void            * aBuffer,
                                                  stlInt64        * aLength,
                                                  dtlFuncVector   * aVectorFunc,
                                                  void            * aVectorArg,
                                                  stlErrorStack   * aErrorStack )
{    
    DTL_CHECK_TYPE( DTL_TYPE_LONGVARCHAR, aValue, aErrorStack );
    STL_PARAM_VALIDATE( aBuffer != NULL, aErrorStack );
    
    STL_TRY( dtdCharStringToStringForAvailableSize( aValue,
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
 * @brief utf16 string value로부터 Longvarchar 타입의 value 구성
 *        <BR> value가 null인 경우는 이 함수를 거치지 않고, 외부에서 처리된다.
 * @param[in]  aString          value에 저장될 문자열 ( utf16 unicode string )
 * @param[in]  aStringLength    aString의 길이   
 * @param[in]  aPrecision       precision
 * @param[in]  aScale           scale 정보
 * @param[in]  aStringUnit      string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aIntervalIndicator 사용하지 않음.
 *                           <BR> INTERVAL 타입의 Indicator dtlIntervalIndicator 참조  
 * @param[in]  aAvailableSize   mValue에 사용할 수 있는 메모리 공간의 크기
 * @param[out] aValue           dtlDataValue의 주소
 * @param[out] aSuccessWithInfo warning 발생 여부
 * @param[in]  aVectorFunc      Function Vector
 * @param[in]  aVectorArg       Vector Argument
 * @param[out] aErrorStack      에러 스택
 */
stlStatus dtdLongvarcharSetValueFromUtf16WCharString( void                 * aString,
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
    STL_TRY( dtdLongvarcharSetValueFromWCharString( DTL_UNICODE_UTF16,
                                                    aString,
                                                    aStringLength,
                                                    aPrecision,
                                                    aScale,
                                                    aStringUnit,
                                                    aIntervalIndicator,
                                                    aAvailableSize,
                                                    aValue,
                                                    aSuccessWithInfo,
                                                    aVectorFunc,
                                                    aVectorArg,
                                                    aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief utf32 string value로부터 Longvarchar 타입의 value 구성
 *        <BR> value가 null인 경우는 이 함수를 거치지 않고, 외부에서 처리된다.
 * @param[in]  aString          value에 저장될 문자열 ( utf32 unicode string )
 * @param[in]  aStringLength    aString의 길이   
 * @param[in]  aPrecision       precision
 * @param[in]  aScale           scale 정보
 * @param[in]  aStringUnit      string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aIntervalIndicator 사용하지 않음.
 *                           <BR> INTERVAL 타입의 Indicator dtlIntervalIndicator 참조  
 * @param[in]  aAvailableSize   mValue에 사용할 수 있는 메모리 공간의 크기
 * @param[out] aValue           dtlDataValue의 주소
 * @param[out] aSuccessWithInfo warning 발생 여부
 * @param[in]  aVectorFunc      Function Vector
 * @param[in]  aVectorArg       Vector Argument
 * @param[out] aErrorStack      에러 스택
 */
stlStatus dtdLongvarcharSetValueFromUtf32WCharString( void                 * aString,
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
    STL_TRY( dtdLongvarcharSetValueFromWCharString( DTL_UNICODE_UTF32,
                                                    aString,
                                                    aStringLength,
                                                    aPrecision,
                                                    aScale,
                                                    aStringUnit,
                                                    aIntervalIndicator,
                                                    aAvailableSize,
                                                    aValue,
                                                    aSuccessWithInfo,
                                                    aVectorFunc,
                                                    aVectorArg,
                                                    aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Unicode ( utf16, utf32 ) string value로부터 Longvarchar 타입의 value 구성
 *        <BR> value가 null인 경우는 이 함수를 거치지 않고, 외부에서 처리된다.
 * @param[in]  aUniEncoding     unicode encoding ( utf16 또는 utf32 )
 * @param[in]  aString          value에 저장될 문자열 ( utf16 또는 utf32 unicode string )
 * @param[in]  aStringLength    aString의 길이   
 * @param[in]  aPrecision       precision
 * @param[in]  aScale           scale 정보
 * @param[in]  aStringUnit      string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aIntervalIndicator 사용하지 않음.
 *                           <BR> INTERVAL 타입의 Indicator dtlIntervalIndicator 참조  
 * @param[in]  aAvailableSize   mValue에 사용할 수 있는 메모리 공간의 크기
 * @param[out] aValue           dtlDataValue의 주소
 * @param[out] aSuccessWithInfo warning 발생 여부
 * @param[in]  aVectorFunc      Function Vector
 * @param[in]  aVectorArg       Vector Argument
 * @param[out] aErrorStack      에러 스택
 */
stlStatus dtdLongvarcharSetValueFromWCharString( dtlUnicodeEncoding     aUniEncoding,
                                                 void                 * aString,
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
    stlInt8           sMbMaxLength;
    stlInt64          sOffset;
    stlInt64          sMaxLength = 0;
    stlInt64          sDestStringLength;
    dtlCharacterSet   sDestCharacterSet;

    DTL_CHECK_TYPE( DTL_TYPE_LONGVARCHAR, aValue, aErrorStack );
    STL_PARAM_VALIDATE( aString != NULL, aErrorStack );
    /* DTL_PARAM_VALIDATE( aPrecision == DTL_LONGVARCHAR_MAX_PRECISION, aErrorStack ); */
    STL_PARAM_VALIDATE( aStringUnit == DTL_STRING_LENGTH_UNIT_OCTETS, aErrorStack );

    /**
     * (1) (aString의 precision) > aPrecision 이면, 에러상황
     */
    
    STL_TRY_THROW( DTL_LONGVARCHAR_MAX_PRECISION >= aStringLength,
                   ERRCODE_STRING_DATA_RIGHT_TRUNCATION );

    *aSuccessWithInfo = STL_FALSE;

    sDestCharacterSet   = aVectorFunc->mGetCharSetIDFunc(aVectorArg);
    
    /**
     * (2) 메모리 확장이 필요한지 검사한다.
     */

    if( aUniEncoding == DTL_UNICODE_UTF16 )
    {
        sMbMaxLength = STL_UINT16_SIZE;
    }
    else if( aUniEncoding == DTL_UNICODE_UTF32 )
    {
        sMbMaxLength = STL_UINT32_SIZE;
    }
    else
    {
        STL_DASSERT( STL_FALSE );
        STL_THROW( ERROR_INTERNAL );
    }

    if( sMbMaxLength >= dtlGetMbMaxLength( sDestCharacterSet ) )
    {
        sMaxLength = aStringLength;
    }
    else
    {
        sMaxLength = aStringLength * (dtlGetMbMaxLength( sDestCharacterSet ) / sMbMaxLength);
    }

    sMaxLength = STL_MIN( sMaxLength, DTL_LONGVARCHAR_MAX_PRECISION );

    if( sMaxLength > aValue->mBufferSize )
    {
        STL_TRY( aVectorFunc->mReallocLongVaryingMemFunc( aVectorArg,
                                                          sMaxLength,
                                                          &aValue->mValue,
                                                          aErrorStack )
                 == STL_SUCCESS );

        aValue->mBufferSize = sMaxLength;
    }

    /**
     * (3) unicode ( utf16, utf32 ) 를 Mb(muti byte) Character set 으로 변경
     */

    STL_TRY( dtlUnicodeToMbConversion( aUniEncoding,
                                       sDestCharacterSet,
                                       aString,
                                       aStringLength,
                                       aValue->mValue,
                                       aValue->mBufferSize,
                                       &sOffset,
                                       &sDestStringLength,
                                       aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( sDestStringLength <= aValue->mBufferSize,
                   ERRCODE_STRING_DATA_RIGHT_TRUNCATION );
    STL_TRY_THROW( sDestStringLength <= DTL_LONGVARCHAR_MAX_PRECISION,
                   ERRCODE_STRING_DATA_RIGHT_TRUNCATION );
    
    /**
     *  (4) 최종 byte length를 구함.
     */
    
    aValue->mLength = sDestStringLength;

    return STL_SUCCESS;

    STL_CATCH( ERRCODE_STRING_DATA_RIGHT_TRUNCATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_BYTE_LENGTH_GREATER_THAN_COLUMN_LENGTH,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INTERNAL,
                      NULL,
                      aErrorStack,
                      "dtdLongvarcharSetValueFromWCharString()" );  
    } 

    STL_FINISH;

    return STL_FAILURE;
}



/** @} */
