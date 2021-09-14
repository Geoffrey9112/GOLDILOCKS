/*******************************************************************************
 * dtdVarchar.c
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
 * @file dtdVarchar.c
 * @brief DataType Layer Varchar type 정의
 */

#include <dtl.h>

#include <dtDef.h>
#include <dtdDataType.h>
#include <dtsCharacterSet.h>

/**
 * @addtogroup dtlVarchar
 * @{
 */

/**
 * @brief Varchar 타입의 byte length 얻어오기 (dtlDataValue의 mValue의 크기) 
 * @param[in]  aPrecision  각 데이타 타입의 precision
 * @param[in]  aStringUnit string length unit (dtlStringLengthUnit 참조) 
 * @param[out] aLength     value의 byte length
 * @param[in]  aVectorFunc Function Vector
 * @param[in]  aVectorArg  Vector Argument
 * @param[out] aErrorStack 에러 스택
 */
stlStatus dtdVarcharGetLength( stlInt64              aPrecision,
                               dtlStringLengthUnit   aStringUnit,
                               stlInt64            * aLength,
                               dtlFuncVector       * aVectorFunc,
                               void                * aVectorArg,
                               stlErrorStack       * aErrorStack )
{
    DTL_PARAM_VALIDATE( (aPrecision >= DTL_VARCHAR_MIN_PRECISION) &&
                        (aPrecision <= DTL_VARCHAR_MAX_PRECISION),
                        aErrorStack );
    DTL_PARAM_VALIDATE( (aStringUnit > DTL_STRING_LENGTH_UNIT_NA ) &&
                        (aStringUnit < DTL_STRING_LENGTH_UNIT_MAX),
                        aErrorStack );

    if( aStringUnit == DTL_STRING_LENGTH_UNIT_CHARACTERS )
    {
        /*
         * 예) varchar(10 characters) 로 선언된 경우 
         */ 
    
        /**
         *  byte length = aPrecision * (각 characterset의 한 바이트에 대한 최대 size)
         */
        
        *aLength = DTL_CHAR_STRING_COLUMN_SIZE( aPrecision, aVectorFunc, aVectorArg );
    }
    else if( aStringUnit == DTL_STRING_LENGTH_UNIT_OCTETS )
    {
        /*
         * 예) varchar(10 octets) 로 선언된 경우 
         */
        
        *aLength = aPrecision;
    }    
    else
    {
        STL_DASSERT( STL_FALSE );
        STL_THROW( RAMP_ERR_CHARACTER_LENGTH_UNIT_INVALID );
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CHARACTER_LENGTH_UNIT_INVALID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_CHARACTER_LENGTH_UNIT_INVALID,
                      NULL,
                      aErrorStack,
                      dtlDataTypeName[ DTL_TYPE_VARCHAR ],
                      aStringUnit,
                      gDataTypeInfoDomain[ DTL_TYPE_VARCHAR ].mMinStringLengthUnit,
                      gDataTypeInfoDomain[ DTL_TYPE_VARCHAR ].mMaxStringLengthUnit );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Varchar 타입의 byte length 얻어오기
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
stlStatus dtdVarcharGetLengthFromString( stlInt64              aPrecision,
                                         stlInt64              aScale,
                                         dtlStringLengthUnit   aStringUnit,
                                         stlChar             * aString,
                                         stlInt64              aStringLength,
                                         stlInt64            * aLength,
                                         dtlFuncVector       * aVectorFunc,
                                         void                * aVectorArg,
                                         stlErrorStack       * aErrorStack )
{
    stlInt64             sByteLength;
    stlInt64             sSourcePrecisionForDestStringUnitLength = 0;    

    DTL_PARAM_VALIDATE( (aPrecision >= DTL_VARCHAR_MIN_PRECISION) &&
                        (aPrecision <= DTL_VARCHAR_MAX_PRECISION),
                        aErrorStack );
    DTL_PARAM_VALIDATE( (aStringUnit > DTL_STRING_LENGTH_UNIT_NA ) &&
                        (aStringUnit < DTL_STRING_LENGTH_UNIT_MAX),
                        aErrorStack );
    STL_PARAM_VALIDATE( aString != NULL, aErrorStack );
    
    /**
     *  (1) aString의 byte length를 얻는다. <BR>
     */
    sByteLength = aStringLength;

    /**
     * (2) aString의 character length를 얻는다.<BR>
     *     (character length를 문자 갯수 또는 value의 precision이라고도 표현할 수 있다.)<BR>
     */    
    STL_TRY( dtdGetPrecisionForDestStringUnitLength( DTL_TYPE_VARCHAR,
                                                     aString,
                                                     sByteLength,
                                                     aStringUnit,
                                                     & sSourcePrecisionForDestStringUnitLength,
                                                     aVectorFunc,
                                                     aVectorArg,
                                                     aErrorStack )
             == STL_SUCCESS );
    
    /** 
     *  (3) aString의 precision > aPrecision 이면, 에러상황 <BR>
     */
    STL_TRY_THROW( (aPrecision >= sSourcePrecisionForDestStringUnitLength),
                   ERRCODE_STRING_DATA_RIGHT_TRUNCATION );    
    
    *aLength = sByteLength;
    
    return STL_SUCCESS;
    
    STL_CATCH( ERRCODE_STRING_DATA_RIGHT_TRUNCATION )
    {
        if ( aStringUnit == DTL_STRING_LENGTH_UNIT_CHARACTERS )
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          DTL_ERRCODE_C_STRING_CHARACTER_LENGTH_GREATER_THAN_COLUMN_LENGTH,
                          NULL,
                          aErrorStack );
        }
        else
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          DTL_ERRCODE_C_STRING_BYTE_LENGTH_GREATER_THAN_COLUMN_LENGTH,
                          NULL,
                          aErrorStack );
        }
    }

    STL_FINISH;

    return STL_FAILURE;
}


/* /\** */
/*  * @brief Varchar 타입의 byte length값을 얻음.  */
/*  * @param[in]  aPrecision  각 데이타 타입의 precision( 사용하지 않음 ) */
/*  * @param[in]  aScale      scale 정보(사용하지 않음) */
/*  * @param[in]  aInteger    입력 숫자( 사용하지 않음 ) */
/*  * @param[out] aLength     Varchar의 byte length */
/*  * @param[out] aErrorStack 에러 스택 */
/*  *\/ */
/* stlStatus dtdVarcharGetLengthFromInteger( stlInt64          aPrecision, */
/*                                           stlInt64          aScale, */
/*                                           stlInt64          aInteger,  */
/*                                           stlInt64        * aLength, */
/*                                           stlErrorStack   * aErrorStack ) */
/* { */
/*     /\** */
/*      * Varchar 타입의 경우, Integer로부터 Varchar length를 구하는 경우는 없음. */
/*      * 이전에 이미 function이나 casting을 통해 string으로 처리되는 과정을 거쳤을것임. */
/*      *\/ */
    
/*     stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                   DTL_ERRCODE_NOT_IMPLEMENTED, */
/*                   NULL, */
/*                   aErrorStack, */
/*                   "dtdVarcharGetLengthFromInteger()" ); */

/*     return STL_FAILURE; */
/* } */

/* /\** */
/*  * @brief Varchar 타입의 byte length값을 얻음.  */
/*  * @param[in]  aPrecision  각 데이타 타입의 precision( 사용하지 않음 ) */
/*  * @param[in]  aScale      scale 정보(사용하지 않음) */
/*  * @param[in]  aReal       입력 숫자( 사용하지 않음 ) */
/*  * @param[out] aLength     Varchar의 byte length */
/*  * @param[out] aErrorStack 에러 스택 */
/*  *\/ */
/* stlStatus dtdVarcharGetLengthFromReal( stlInt64          aPrecision, */
/*                                        stlInt64          aScale, */
/*                                        stlFloat64        aReal,  */
/*                                        stlInt64        * aLength, */
/*                                        stlErrorStack   * aErrorStack ) */
/* { */
/*     /\** */
/*      * Varchar 타입의 경우, Real로부터 Varchar length를 구하는 경우는 없음. */
/*      * 이전에 이미 function이나 casting을 통해 string으로 처리되는 과정을 거쳤을것임. */
/*      *\/ */
    
/*     stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                   DTL_ERRCODE_NOT_IMPLEMENTED, */
/*                   NULL, */
/*                   aErrorStack, */
/*                   "dtdVarcharGetLengthFromReal()" ); */

/*     return STL_FAILURE; */
/* } */

/**
 * @brief string value로부터 Varchar 타입의 value 구성
 *        <BR> value가 null인 경우는 이 함수를 거치지 않고, 외부에서 처리된다.
 * @param[in]  aString            value에 저장될 문자열
 * @param[in]  aStringLength      aString의 길이   
 * @param[in]  aPrecision         precision
 * @param[in]  aScale             scale 정보
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
stlStatus dtdVarcharSetValueFromString( stlChar              * aString,
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
    stlInt64        sSourcePrecisionForDestStringUnitLength = 0;
    stlInt64        sOffset;
    stlInt64        sDestStringLength;
    dtlCharacterSet sSourceCharacterSet;
    dtlCharacterSet sDestCharacterSet;

    DTL_PARAM_VALIDATE( (aPrecision >= DTL_VARCHAR_MIN_PRECISION) &&
                        (aPrecision <= DTL_VARCHAR_MAX_PRECISION),
                        aErrorStack );
    DTL_PARAM_VALIDATE( (aStringUnit > DTL_STRING_LENGTH_UNIT_NA ) &&
                        (aStringUnit < DTL_STRING_LENGTH_UNIT_MAX),
                        aErrorStack );
    DTL_CHECK_TYPE2( DTL_TYPE_CHAR, DTL_TYPE_VARCHAR, aValue, aErrorStack );
    STL_PARAM_VALIDATE( aString != NULL, aErrorStack );

    *aSuccessWithInfo = STL_FALSE;
    
    if ( aStringLength > aPrecision )
    {
        /**
         * (1) aString의 character length를 얻는다.<BR>
         *     (character length를 문자 갯수 또는 value의 precision이라고도 표현할 수 있다.)<BR>
         */
        STL_TRY( dtdGetPrecisionForDestStringUnitLength( aValue->mType,
                                                         aString,
                                                         aStringLength,
                                                         aStringUnit,
                                                         & sSourcePrecisionForDestStringUnitLength,
                                                         aSourceVectorFunc,
                                                         aSourceVectorArg,
                                                         aErrorStack )
                 == STL_SUCCESS );
        
        /** 
         *  (2) (aString의 precision) > aPrecision 이면, 에러상황 <BR>
         */
        STL_TRY_THROW( aPrecision >= sSourcePrecisionForDestStringUnitLength,
                       ERRCODE_STRING_DATA_RIGHT_TRUNCATION );
    }
    else
    {
        /**
         * String Length 가 Precision 보다 작다면 Right Truncation 에러가 발생하지 않는다.
         * - VARCHAR 의 경우 Character Length 를 별도로 구할 필요가 없다.
         */
    }
    
    
    /**
     * (3) aValue->mValue에 aString으로 value 구성.<BR>
     */
    sSourceCharacterSet = aSourceVectorFunc->mGetCharSetIDFunc(aSourceVectorArg);
    sDestCharacterSet   = aDestVectorFunc->mGetCharSetIDFunc(aDestVectorArg);

    if( sSourceCharacterSet == sDestCharacterSet )
    {
        sDestStringLength = aStringLength;
        STL_TRY_THROW( sDestStringLength <= aAvailableSize,
                       ERRCODE_STRING_DATA_RIGHT_TRUNCATION );
        
        stlMemcpy( aValue->mValue, aString, aStringLength );
    }
    else
    {
        STL_TRY( dtlMbConversion( sSourceCharacterSet,
                                  sDestCharacterSet,
                                  aString,
                                  aStringLength,
                                  aValue->mValue,
                                  aAvailableSize,
                                  &sOffset,
                                  &sDestStringLength,
                                  aErrorStack ) == STL_SUCCESS );

        STL_TRY_THROW( sDestStringLength <= aAvailableSize,
                       ERRCODE_STRING_DATA_RIGHT_TRUNCATION );
    }

    /** 
     *  (4) 최종 byte length를 구함.
     */        
    aValue->mLength = sDestStringLength;

    return STL_SUCCESS;

    STL_CATCH( ERRCODE_STRING_DATA_RIGHT_TRUNCATION )
    {
        if ( aStringUnit == DTL_STRING_LENGTH_UNIT_CHARACTERS )
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          DTL_ERRCODE_C_STRING_CHARACTER_LENGTH_GREATER_THAN_COLUMN_LENGTH,
                          NULL,
                          aErrorStack );
        }
        else
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          DTL_ERRCODE_C_STRING_BYTE_LENGTH_GREATER_THAN_COLUMN_LENGTH,
                          NULL,
                          aErrorStack );
        }
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief integer value로부터 Varchar 타입의 value 구성
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
stlStatus dtdVarcharSetValueFromInteger( stlInt64               aInteger,
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

    DTL_PARAM_VALIDATE( (aPrecision >= DTL_VARCHAR_MIN_PRECISION) &&
                        (aPrecision <= DTL_VARCHAR_MAX_PRECISION),
                        aErrorStack );
    DTL_PARAM_VALIDATE( (aStringUnit > DTL_STRING_LENGTH_UNIT_NA ) &&
                        (aStringUnit < DTL_STRING_LENGTH_UNIT_MAX),
                        aErrorStack );
    DTL_CHECK_TYPE2( DTL_TYPE_CHAR, DTL_TYPE_VARCHAR, aValue, aErrorStack );

    *aSuccessWithInfo = STL_FALSE;

    dtdInt64ToString( aInteger,
                      (stlChar *)(aValue->mValue),
                      & sLength );

    STL_TRY_THROW( aPrecision >= sLength, ERRCODE_STRING_DATA_RIGHT_TRUNCATION );

    STL_TRY_THROW( aAvailableSize >= sLength,
                   ERRCODE_STRING_DATA_RIGHT_TRUNCATION );

    aValue->mLength = sLength;

    return STL_SUCCESS;

    STL_CATCH( ERRCODE_STRING_DATA_RIGHT_TRUNCATION )
    {
        if ( aStringUnit == DTL_STRING_LENGTH_UNIT_CHARACTERS )
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          DTL_ERRCODE_C_STRING_CHARACTER_LENGTH_GREATER_THAN_COLUMN_LENGTH,
                          NULL,
                          aErrorStack );
        }
        else
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          DTL_ERRCODE_C_STRING_BYTE_LENGTH_GREATER_THAN_COLUMN_LENGTH,
                          NULL,
                          aErrorStack );
        }
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief real value로부터 Varchar 타입의 value 구성
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
stlStatus dtdVarcharSetValueFromReal( stlFloat64             aReal,
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

    DTL_PARAM_VALIDATE( (aPrecision >= DTL_VARCHAR_MIN_PRECISION) &&
                        (aPrecision <= DTL_VARCHAR_MAX_PRECISION),
                        aErrorStack );
    DTL_PARAM_VALIDATE( (aStringUnit > DTL_STRING_LENGTH_UNIT_NA ) &&
                        (aStringUnit < DTL_STRING_LENGTH_UNIT_MAX),
                        aErrorStack );
    DTL_CHECK_TYPE2( DTL_TYPE_CHAR, DTL_TYPE_VARCHAR, aValue, aErrorStack );

    *aSuccessWithInfo = STL_FALSE;

    sLength = stlSnprintf( sBuffer,
                           DTL_FLOAT64_STRING_SIZE,
                           "%.*G",
                           STL_DBL_DIG,
                           aReal );

    STL_TRY_THROW( aPrecision >= sLength, ERRCODE_STRING_DATA_RIGHT_TRUNCATION );

    STL_TRY_THROW( aAvailableSize >= sLength,
                   ERRCODE_STRING_DATA_RIGHT_TRUNCATION );

    stlMemcpy( aValue->mValue, sBuffer, sLength );

    aValue->mLength = sLength;

    return STL_SUCCESS;

    STL_CATCH( ERRCODE_STRING_DATA_RIGHT_TRUNCATION )
    {
        if ( aStringUnit == DTL_STRING_LENGTH_UNIT_CHARACTERS )
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          DTL_ERRCODE_C_STRING_CHARACTER_LENGTH_GREATER_THAN_COLUMN_LENGTH,
                          NULL,
                          aErrorStack );
        }
        else
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          DTL_ERRCODE_C_STRING_BYTE_LENGTH_GREATER_THAN_COLUMN_LENGTH,
                          NULL,
                          aErrorStack );
        }
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
stlStatus dtdVarcharReverseByteOrder( void            * aValue,
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
 * @brief Varchar value를 문자열로 변환
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
stlStatus dtdVarcharToString( dtlDataValue    * aValue,
                              stlInt64          aPrecision,
                              stlInt64          aScale,
                              stlInt64          aAvailableSize,
                              void            * aBuffer,
                              stlInt64        * aLength,
                              dtlFuncVector   * aVectorFunc,
                              void            * aVectorArg,
                              stlErrorStack   * aErrorStack )
{
    DTL_CHECK_TYPE( DTL_TYPE_VARCHAR, aValue, aErrorStack );
    STL_PARAM_VALIDATE( aAvailableSize >= aValue->mLength, aErrorStack );
    STL_PARAM_VALIDATE( aBuffer != NULL, aErrorStack );
    
    stlMemcpy( aBuffer, aValue->mValue, aValue->mLength );
    *aLength = aValue->mLength;

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;    
}


/**
 * @brief Varchar value를 사용가능한 버퍼사이즈에 맞게 잘라서 문자열로 변환한다.
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
stlStatus dtdVarcharToStringForAvailableSize( dtlDataValue    * aValue,
                                              stlInt64          aPrecision,
                                              stlInt64          aScale,
                                              stlInt64          aAvailableSize,
                                              void            * aBuffer,
                                              stlInt64        * aLength,
                                              dtlFuncVector   * aVectorFunc,
                                              void            * aVectorArg,
                                              stlErrorStack   * aErrorStack )
{    
    DTL_CHECK_TYPE( DTL_TYPE_VARCHAR, aValue, aErrorStack );
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
 * @brief utf16 string value로부터 Varchar 타입의 value 구성
 *        <BR> value가 null인 경우는 이 함수를 거치지 않고, 외부에서 처리된다.
 * @param[in]  aString            value에 저장될 문자열 ( utf16 unicode string )
 * @param[in]  aStringLength      aString의 길이   
 * @param[in]  aPrecision         precision
 * @param[in]  aScale             scale 정보
 * @param[in]  aStringUnit        string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aIntervalIndicator 사용하지 않음.
 *                           <BR> INTERVAL 타입의 Indicator dtlIntervalIndicator 참조 
 * @param[in]  aAvailableSize     mValue에 사용할 수 있는 메모리 공간의 크기
 * @param[out] aValue             dtlDataValue의 주소
 * @param[out] aSuccessWithInfo   warning 발생 여부
 * @param[in]  aVectorFunc        Function Vector
 * @param[in]  aVectorArg         Vector Argument
 * @param[out] aErrorStack        에러 스택
 */
stlStatus dtdVarcharSetValueFromUtf16WCharString( void                 * aString,
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
    STL_TRY( dtdVarcharSetValueFromWCharString( DTL_UNICODE_UTF16,
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
 * @brief utf32 string value로부터 Varchar 타입의 value 구성
 *        <BR> value가 null인 경우는 이 함수를 거치지 않고, 외부에서 처리된다.
 * @param[in]  aString            value에 저장될 문자열 ( utf32 unicode string )
 * @param[in]  aStringLength      aString의 길이   
 * @param[in]  aPrecision         precision
 * @param[in]  aScale             scale 정보
 * @param[in]  aStringUnit        string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aIntervalIndicator 사용하지 않음.
 *                           <BR> INTERVAL 타입의 Indicator dtlIntervalIndicator 참조 
 * @param[in]  aAvailableSize     mValue에 사용할 수 있는 메모리 공간의 크기
 * @param[out] aValue             dtlDataValue의 주소
 * @param[out] aSuccessWithInfo   warning 발생 여부
 * @param[in]  aVectorFunc        Function Vector
 * @param[in]  aVectorArg         Vector Argument
 * @param[out] aErrorStack        에러 스택
 */
stlStatus dtdVarcharSetValueFromUtf32WCharString( void                 * aString,
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
    STL_TRY( dtdVarcharSetValueFromWCharString( DTL_UNICODE_UTF32,
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
 * @brief Unicode ( utf16, utf32 ) string value로부터 Varchar 타입의 value 구성
 *        <BR> value가 null인 경우는 이 함수를 거치지 않고, 외부에서 처리된다.
 * @param[in]  aUniEncoding       unicode encoding ( utf16 또는 utf32 )
 * @param[in]  aString            value에 저장될 문자열 ( utf16 또는 utf32 unicode string )
 * @param[in]  aStringLength      aString의 길이   
 * @param[in]  aPrecision         precision
 * @param[in]  aScale             scale 정보
 * @param[in]  aStringUnit        string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aIntervalIndicator 사용하지 않음.
 *                           <BR> INTERVAL 타입의 Indicator dtlIntervalIndicator 참조 
 * @param[in]  aAvailableSize     mValue에 사용할 수 있는 메모리 공간의 크기
 * @param[out] aValue             dtlDataValue의 주소
 * @param[out] aSuccessWithInfo   warning 발생 여부
 * @param[in]  aVectorFunc        Function Vector
 * @param[in]  aVectorArg         Vector Argument
 * @param[out] aErrorStack        에러 스택
 */
stlStatus dtdVarcharSetValueFromWCharString( dtlUnicodeEncoding     aUniEncoding,
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
    stlInt64          sDestPrecision = 0;
    stlInt64          sOffset;
    stlInt64          sDestStringLength;
    dtlCharacterSet   sDestCharacterSet;
    stlChar         * sDest;

    DTL_PARAM_VALIDATE( (aPrecision >= DTL_VARCHAR_MIN_PRECISION) &&
                        (aPrecision <= DTL_VARCHAR_MAX_PRECISION),
                        aErrorStack );
    DTL_PARAM_VALIDATE( (aStringUnit > DTL_STRING_LENGTH_UNIT_NA ) &&
                        (aStringUnit < DTL_STRING_LENGTH_UNIT_MAX),
                        aErrorStack );
    DTL_CHECK_TYPE2( DTL_TYPE_CHAR, DTL_TYPE_VARCHAR, aValue, aErrorStack );
    STL_PARAM_VALIDATE( aString != NULL, aErrorStack );

    *aSuccessWithInfo = STL_FALSE;

    sDestCharacterSet = aVectorFunc->mGetCharSetIDFunc(aVectorArg);
    sDest = aValue->mValue;

    /**
     * (1) unicode ( utf16, utf32 ) 를 Mb(muti byte) Character set 으로 변경
     */
    STL_TRY( dtlUnicodeToMbConversion( aUniEncoding,
                                       sDestCharacterSet,
                                       aString,
                                       aStringLength,
                                       sDest,
                                       aAvailableSize,
                                       &sOffset,
                                       &sDestStringLength,
                                       aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( sDestStringLength <= aAvailableSize,
                   ERRCODE_STRING_DATA_RIGHT_TRUNCATION );
 
    if ( sDestStringLength > aPrecision )
    {
        /**
         * (2) aString의 character length를 얻는다.<BR>
         *     (character length를 문자 갯수 또는 value의 precision이라고도 표현할 수 있다.)<BR>
         */
        STL_TRY( dtdGetPrecisionForDestStringUnitLength( aValue->mType,
                                                         sDest,
                                                         sDestStringLength,
                                                         aStringUnit,
                                                         & sDestPrecision,
                                                         aVectorFunc,
                                                         aVectorArg,
                                                         aErrorStack )
                 == STL_SUCCESS );
        
        /** 
         *  (3) (aString의 precision) > aPrecision 이면, 에러상황 <BR>
         */
        STL_TRY_THROW( aPrecision >= sDestPrecision,
                       ERRCODE_STRING_DATA_RIGHT_TRUNCATION );
    }
    else
    {
        /**
         * String Length 가 Precision 보다 작다면 Right Truncation 에러가 발생하지 않는다.
         * - VARCHAR 의 경우 Character Length 를 별도로 구할 필요가 없다.
         */
    }
    
    /** 
     *  (4) 최종 byte length를 구함.
     */        
    aValue->mLength = sDestStringLength;

    return STL_SUCCESS;

    STL_CATCH( ERRCODE_STRING_DATA_RIGHT_TRUNCATION )
    {
        if ( aStringUnit == DTL_STRING_LENGTH_UNIT_CHARACTERS )
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          DTL_ERRCODE_C_STRING_CHARACTER_LENGTH_GREATER_THAN_COLUMN_LENGTH,
                          NULL,
                          aErrorStack );
        }
        else
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          DTL_ERRCODE_C_STRING_BYTE_LENGTH_GREATER_THAN_COLUMN_LENGTH,
                          NULL,
                          aErrorStack );
        }
    }

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
