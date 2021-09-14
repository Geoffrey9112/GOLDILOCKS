/*******************************************************************************
 * dtdBoolean.c
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
 * @file dtdBoolean.c
 * @brief DataType Layer Boolean type 정의
 */

#include <dtl.h>

#include <dtdDef.h>
#include <dtdDataType.h>

/**
 * @addtogroup dtlBoolean
 * @{
 */

/**
 * @brief Boolean 타입의 length값을 얻음
 * @param[in]  aPrecision  사용하지 않음
 * @param[in]  aStringUnit string length unit (dtlStringLengthUnit 참조) 
 * @param[out] aLength     Boolean의 길이 값
 * @param[in]  aVectorFunc Function Vector
 * @param[in]  aVectorArg  Vector Argument
 * @param[out] aErrorStack 에러 스택
 */
stlStatus dtdBooleanGetLength( stlInt64              aPrecision,
                               dtlStringLengthUnit   aStringUnit,
                               stlInt64            * aLength,
                               dtlFuncVector       * aVectorFunc,
                               void                * aVectorArg,
                               stlErrorStack       * aErrorStack )
{
    *aLength = DTL_BOOLEAN_SIZE;
    
    return STL_SUCCESS;
}


/**
 * @brief Boolean 타입의 length값을 얻음 (입력 문자열에 영향을 받지 않음)
 * @param[in]  aPrecision    사용하지 않음
 * @param[in]  aScale        사용하지 않음
 * @param[in]  aStringUnit   string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aString       입력 문자열
 * @param[in]  aStringLength aString의 길이    
 * @param[out] aLength       Boolean의 길이 값
 * @param[in]  aVectorFunc   Function Vector
 * @param[in]  aVectorArg    Vector Argument
 * @param[out] aErrorStack   에러 스택
 */
stlStatus dtdBooleanGetLengthFromString( stlInt64              aPrecision,
                                         stlInt64              aScale,
                                         dtlStringLengthUnit   aStringUnit,
                                         stlChar             * aString,
                                         stlInt64              aStringLength,
                                         stlInt64            * aLength,
                                         dtlFuncVector       * aVectorFunc,
                                         void                * aVectorArg,
                                         stlErrorStack       * aErrorStack )
{
    stlChar  * sStartPos;
    stlChar  * sFence;
    stlInt64   sStringLength;
        
    DTL_PARAM_VALIDATE( aString != NULL, aErrorStack );

    /* string value로부터 Boolean 타입의 value 구성 */
    sStringLength = aStringLength;
    
    /* truncate leading space */
    sStartPos = aString;
    while( sStringLength >= 0 )
    {
        if( stlIsspace(*sStartPos) )
        {
            sStartPos++;
            sStringLength--;
        }
        else
        {
            break;
        }
    }
    
    /* truncate trailing space */
    sFence = aString + aStringLength - 1;
    while( sFence > sStartPos )
    {
        if( stlIsspace(*sFence) )
        {
            sFence--;
            sStringLength--;
        }
        else
        {
            break;
        }
    }

    switch( sStringLength )
    {
        case DTL_BOOLEAN_STRING_TRUE_SIZE :
            {
                STL_TRY_THROW( stlStrncasecmp( sStartPos,
                                               DTL_BOOLEAN_STRING_TRUE,
                                               DTL_BOOLEAN_STRING_TRUE_SIZE ) == 0,
                               ERROR_INVALID_INPUT_DATA );
                break;
            }
        case DTL_BOOLEAN_STRING_FALSE_SIZE :
            {
                STL_TRY_THROW( stlStrncasecmp( sStartPos,
                                               DTL_BOOLEAN_STRING_FALSE,
                                               DTL_BOOLEAN_STRING_FALSE_SIZE ) == 0,
                               ERROR_INVALID_INPUT_DATA );
                break;
            }
        default :
            {
                STL_THROW( ERROR_INVALID_INPUT_DATA );
                break;
            }
    }
    
    *aLength = DTL_BOOLEAN_SIZE;

    return STL_SUCCESS;

    STL_CATCH( ERROR_INVALID_INPUT_DATA )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_DATA_VALUE_NOT_BOOLEAN_LITERAL,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Boolean 타입의 length값을 얻음 (입력 숫자에 영향을 받지 않음)
 * @param[in]  aPrecision  사용하지 않음
 * @param[in]  aScale      사용하지 않음
 * @param[in]  aInteger    입력 숫자
 * @param[out] aLength     Boolean의 길이 값
 * @param[out] aErrorStack 에러 스택
 */
stlStatus dtdBooleanGetLengthFromInteger( stlInt64          aPrecision,
                                          stlInt64          aScale,
                                          stlInt64          aInteger,
                                          stlInt64        * aLength,
                                          stlErrorStack   * aErrorStack )
{
    switch( aInteger )
    {
        case STL_TRUE :
        case STL_FALSE :
            {
                *aLength = DTL_BOOLEAN_SIZE;
                break;
            }
        default :
            {
                STL_THROW( ERROR_INVALID_INPUT_DATA );
                break;
            }
    }
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_INVALID_INPUT_DATA )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_NUMERIC_DATA_LESS_0_GREATER_OR_EQUAL_2,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/* /\** */
/*  * @brief Boolean 타입의 length값을 얻음 (입력 숫자에 영향을 받지 않음) */
/*  * @param[in]  aPrecision  사용하지 않음 */
/*  * @param[in]  aScale      사용하지 않음 */
/*  * @param[in]  aReal       입력 숫자 */
/*  * @param[out] aLength     Boolean의 길이 값 */
/*  * @param[out] aErrorStack 에러 스택 */
/*  *\/ */
/* stlStatus dtdBooleanGetLengthFromReal( stlInt64          aPrecision, */
/*                                        stlInt64          aScale, */
/*                                        stlFloat64        aReal, */
/*                                        stlInt64        * aLength, */
/*                                        stlErrorStack   * aErrorStack ) */
/* { */
/*     stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                   DTL_ERRCODE_NOT_IMPLEMENTED, */
/*                   NULL, */
/*                   aErrorStack, */
/*                   "dtdBooleanGetLengthFromReal()" ); */
    
/*     return STL_FAILURE; */
/* } */

/**
 * @brief string value로부터 Boolean 타입의 value 구성
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
stlStatus dtdBooleanSetValueFromString( stlChar              * aString,
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
    dtlBooleanType   * sValue;
    stlChar          * sStartPos;
    stlChar          * sFence;
    stlInt64           sStringLength;

    DTL_CHECK_TYPE( DTL_TYPE_BOOLEAN, aValue, aErrorStack );
    DTL_PARAM_VALIDATE( aString != NULL, aErrorStack );
    DTL_PARAM_VALIDATE( aAvailableSize >= DTL_BOOLEAN_SIZE, aErrorStack );

    *aSuccessWithInfo = STL_FALSE;
    
    /* string value로부터 Boolean 타입의 value 구성 */
    sStringLength = aStringLength;
    
    /* truncate leading space */
    sStartPos = aString;
    while( sStringLength >= 0 )
    {
        if( stlIsspace(*sStartPos) )
        {
            sStartPos++;
            sStringLength--;
        }
        else
        {
            break;
        }
    }
    
    /* truncate trailing space */
    sFence = aString + aStringLength - 1;
    while( sFence > sStartPos )
    {
        if( stlIsspace(*sFence) )
        {
            sFence--;
            sStringLength--;
        }
        else
        {
            break;
        }
    }

    sValue = aValue->mValue;
    
    switch( sStringLength )
    {
        case DTL_BOOLEAN_STRING_TRUE_SIZE :
            {
                STL_TRY_THROW( stlStrncasecmp( sStartPos,
                                               DTL_BOOLEAN_STRING_TRUE,
                                               DTL_BOOLEAN_STRING_TRUE_SIZE ) == 0,
                               ERROR_INVALID_INPUT_DATA );

                *sValue = (dtlBooleanType)STL_TRUE;
                break;
            }
        case DTL_BOOLEAN_STRING_FALSE_SIZE :
            {
                STL_TRY_THROW( stlStrncasecmp( sStartPos,
                                               DTL_BOOLEAN_STRING_FALSE,
                                               DTL_BOOLEAN_STRING_FALSE_SIZE ) == 0,
                               ERROR_INVALID_INPUT_DATA );
                
                *sValue = (dtlBooleanType)STL_FALSE;
                break;
            }
        default :
            {
                STL_THROW( ERROR_INVALID_INPUT_DATA );
                break;
            }
    }

    aValue->mLength = DTL_BOOLEAN_SIZE;

    return STL_SUCCESS;

    STL_CATCH( ERROR_INVALID_INPUT_DATA )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_DATA_VALUE_NOT_BOOLEAN_LITERAL,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief integer value로부터 Boolean 타입의 value 구성
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
stlStatus dtdBooleanSetValueFromInteger( stlInt64               aInteger,
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
    dtlBooleanType   * sValue;

    DTL_CHECK_TYPE( DTL_TYPE_BOOLEAN, aValue, aErrorStack );
    DTL_PARAM_VALIDATE( aAvailableSize >= DTL_BOOLEAN_SIZE, aErrorStack );

    *aSuccessWithInfo = STL_FALSE;
    
    sValue = aValue->mValue;

    /* integer value로부터 Boolean 타입의 value 구성 */
    switch( aInteger )
    {
        case STL_TRUE :
        case STL_FALSE :
            {
                *sValue = (dtlBooleanType) aInteger;
                aValue->mLength = DTL_BOOLEAN_SIZE;
                break;
            }
        default :
            {
                STL_THROW( ERROR_INVALID_INPUT_DATA );
                break;
            }
    }

    return STL_SUCCESS;

    STL_CATCH( ERROR_INVALID_INPUT_DATA )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_NUMERIC_DATA_LESS_0_GREATER_OR_EQUAL_2,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/* /\** */
/*  * @brief real value로부터 Boolean 타입의 value 구성 */
/*  * @param[in]  aReal              value에 저장될 입력 숫자 */
/*  * @param[in]  aPrecision         사용하지 않음 */
/*  * @param[in]  aScale             사용하지 않음 */
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
/* stlStatus dtdBooleanSetValueFromReal( stlFloat64             aReal, */
/*                                       stlInt64               aPrecision, */
/*                                       stlInt64               aScale, */
/*                                       dtlStringLengthUnit    aStringUnit, */
/*                                       dtlIntervalIndicator   aIntervalIndicator, */
/*                                       stlInt64               aAvailableSize, */
/*                                       dtlDataValue         * aValue, */
/*                                       stlBool              * aSuccessWithInfo, */
/*                                       dtlFuncVector        * aVectorFunc, */
/*                                       void                 * aVectorArg, */
/*                                       stlErrorStack        * aErrorStack ) */
/* { */
/*     stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                   DTL_ERRCODE_NOT_IMPLEMENTED, */
/*                   NULL, */
/*                   aErrorStack, */
/*                   "dtdBooleanSetValueFromReal()" ); */
    
/*     return STL_FAILURE; */
/* } */

/**
 * @brief Byte Order 변경 (redirect 함수 아님, 내부 함수)
 * @param[in]  aValue          byte swapping 대상 및 결과 
 * @param[in]  aIsSameEndian   byte swapping 필요 여부
 * @param[out] aErrorStack     에러 스택
 */
stlStatus dtdBooleanReverseByteOrder( void            * aValue,
                                      stlBool           aIsSameEndian,
                                      stlErrorStack   * aErrorStack )
{
    /* Do Nothing */
    
    return STL_SUCCESS;
}

/**
 * @brief Boolean value를 문자열로 변환
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
stlStatus dtdBooleanToString( dtlDataValue    * aValue,
                              stlInt64          aPrecision,
                              stlInt64          aScale,
                              stlInt64          aAvailableSize,
                              void            * aBuffer,
                              stlInt64        * aLength,
                              dtlFuncVector   * aVectorFunc,
                              void            * aVectorArg,
                              stlErrorStack   * aErrorStack )
{
    dtlBooleanType  * sValue;

    DTL_CHECK_TYPE( DTL_TYPE_BOOLEAN, aValue, aErrorStack );
    DTL_PARAM_VALIDATE( aAvailableSize >= DTL_BOOLEAN_STRING_SIZE, aErrorStack );
    DTL_PARAM_VALIDATE( aBuffer != NULL, aErrorStack );
    
    sValue = (dtlBooleanType*) aValue->mValue;
    
    if( *sValue == STL_TRUE )
    {
        stlMemcpy( aBuffer, DTL_BOOLEAN_STRING_TRUE, DTL_BOOLEAN_STRING_TRUE_SIZE );
        *aLength = DTL_BOOLEAN_STRING_TRUE_SIZE;
    }
    else
    {
        STL_DASSERT( *sValue == STL_FALSE );

        stlMemcpy( aBuffer, DTL_BOOLEAN_STRING_FALSE, DTL_BOOLEAN_STRING_FALSE_SIZE );
        *aLength = DTL_BOOLEAN_STRING_FALSE_SIZE;
    }
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Boolean value를 사용가능한 버퍼사이즈에 맞게 잘라서 문자열로 변환한다.
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
stlStatus dtdBooleanToStringForAvailableSize( dtlDataValue    * aValue,
                                              stlInt64          aPrecision,
                                              stlInt64          aScale,
                                              stlInt64          aAvailableSize,
                                              void            * aBuffer,
                                              stlInt64        * aLength,
                                              dtlFuncVector   * aVectorFunc,
                                              void            * aVectorArg,
                                              stlErrorStack   * aErrorStack )
{
    dtlBooleanType  * sValue;
    stlInt64          sLength = 0;
    
    DTL_CHECK_TYPE( DTL_TYPE_BOOLEAN, aValue, aErrorStack );
    DTL_PARAM_VALIDATE( aAvailableSize >= 0, aErrorStack );    
    DTL_PARAM_VALIDATE( aBuffer != NULL, aErrorStack );

    sValue = (dtlBooleanType*) aValue->mValue;

    if( *sValue == STL_TRUE )
    {
        if( aAvailableSize >= DTL_BOOLEAN_STRING_TRUE_SIZE )
        {
            sLength = DTL_BOOLEAN_STRING_TRUE_SIZE;
        }
        else
        {
            sLength = aAvailableSize;
        }
        
        stlMemcpy( aBuffer, DTL_BOOLEAN_STRING_TRUE, sLength );
    }
    else
    {
        STL_DASSERT( *sValue == STL_FALSE );

        if( aAvailableSize >= DTL_BOOLEAN_STRING_FALSE_SIZE )
        {
            sLength = DTL_BOOLEAN_STRING_FALSE_SIZE;
        }
        else
        {
            sLength = aAvailableSize;
        }

        stlMemcpy( aBuffer, DTL_BOOLEAN_STRING_FALSE, sLength );
    }

    *aLength = sLength;
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief utf16 string value로부터 Boolean 타입의 value 구성
 * @param[in]  aString            value에 저장될 입력 문자열 ( utf16 unicode string )
 * @param[in]  aStringLength      aString의 길이   
 * @param[in]  aPrecision         사용하지 않음
 * @param[in]  aScale             사용하지 않음
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
stlStatus dtdBooleanSetValueFromUtf16WCharString( void                 * aString,
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
    dtlBooleanType   * sValue;
    stlUInt16        * sStartPos;
    stlUInt16        * sFence;
    stlInt64           sCharacterLength;
    stlChar          * sBooleanStr;
    stlChar          * sBooleanLowerStr;

    DTL_CHECK_TYPE( DTL_TYPE_BOOLEAN, aValue, aErrorStack );
    DTL_PARAM_VALIDATE( aString != NULL, aErrorStack );
    DTL_PARAM_VALIDATE( aAvailableSize >= DTL_BOOLEAN_SIZE, aErrorStack );
    /* aStringLength 길이 체크 */
    DTL_PARAM_VALIDATE( (aStringLength - (aStringLength / STL_UINT16_SIZE * STL_UINT16_SIZE)) == 0, 
                        aErrorStack );

    *aSuccessWithInfo = STL_FALSE;

    /*
     * Boolean 타입은 ascii 문자만 처리하면 되므로,
     * db cahracter set 으로의 conversion 은 하지 않아도 된다.
     */
    
    /* string value로부터 Boolean 타입의 value 구성 */
    sCharacterLength = aStringLength / STL_UINT16_SIZE;
    sFence = (stlUInt16*)aString + sCharacterLength - 1;
    
    /* truncate leading space */
    sStartPos = (stlUInt16*)aString;
    while( sCharacterLength >= 0 )
    {
        if( stlIsspace(*sStartPos) )
        {
            sStartPos++;
            sCharacterLength--;
        }
        else
        {
            break;
        }
    }
    
    /* truncate trailing space */
    while( sFence > sStartPos )
    {
        if( stlIsspace(*sFence) )
        {
            sFence--;
            sCharacterLength--;
        }
        else
        {
            break;
        }
    }

    sValue = aValue->mValue;

    switch( sCharacterLength )
    {
        case DTL_BOOLEAN_STRING_TRUE_SIZE:
            {
                sBooleanStr = DTL_BOOLEAN_STRING_TRUE;
                sBooleanLowerStr = DTL_BOOLEAN_LOWER_STRING_TRUE;
  
                /*
                 * stlStrncasecmp
                 */
                if( ( (sStartPos[0] == sBooleanStr[0]) || (sStartPos[0] == sBooleanLowerStr[0]) ) && 
                    ( (sStartPos[1] == sBooleanStr[1]) || (sStartPos[1] == sBooleanLowerStr[1]) ) && 
                    ( (sStartPos[2] == sBooleanStr[2]) || (sStartPos[2] == sBooleanLowerStr[2]) ) && 
                    ( (sStartPos[3] == sBooleanStr[3]) || (sStartPos[3] == sBooleanLowerStr[3]) ) ) 
                {
                    /* Do Nothing */
                }
                else
                {
                    STL_THROW( ERROR_INVALID_INPUT_DATA );
                }
                    
                *sValue = (dtlBooleanType)STL_TRUE;
                break;
            }
        case DTL_BOOLEAN_STRING_FALSE_SIZE:
            {
                sBooleanStr = DTL_BOOLEAN_STRING_FALSE;
                sBooleanLowerStr = DTL_BOOLEAN_LOWER_STRING_FALSE;

                /*
                 * stlStrncasecmp
                 */
                if( ( (sStartPos[0] == sBooleanStr[0]) || (sStartPos[0] == sBooleanLowerStr[0]) ) && 
                    ( (sStartPos[1] == sBooleanStr[1]) || (sStartPos[1] == sBooleanLowerStr[1]) ) && 
                    ( (sStartPos[2] == sBooleanStr[2]) || (sStartPos[2] == sBooleanLowerStr[2]) ) && 
                    ( (sStartPos[3] == sBooleanStr[3]) || (sStartPos[3] == sBooleanLowerStr[3]) ) && 
                    ( (sStartPos[4] == sBooleanStr[4]) || (sStartPos[4] == sBooleanLowerStr[4]) ) ) 
                {
                    /* Do Nothing */
                }
                else
                {
                    STL_THROW( ERROR_INVALID_INPUT_DATA );
                }
                
                *sValue = (dtlBooleanType)STL_FALSE;
                break;
            }
        default :
            {
                STL_THROW( ERROR_INVALID_INPUT_DATA );
                break;
            }
    }

    aValue->mLength = DTL_BOOLEAN_SIZE;

    return STL_SUCCESS;

    STL_CATCH( ERROR_INVALID_INPUT_DATA )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_DATA_VALUE_NOT_BOOLEAN_LITERAL,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief utf32 string value로부터 Boolean 타입의 value 구성
 * @param[in]  aString            value에 저장될 입력 문자열 ( utf32 unicode string )
 * @param[in]  aStringLength      aString의 길이   
 * @param[in]  aPrecision         사용하지 않음
 * @param[in]  aScale             사용하지 않음
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
stlStatus dtdBooleanSetValueFromUtf32WCharString( void                 * aString,
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
    dtlBooleanType   * sValue;
    stlUInt32        * sStartPos;
    stlUInt32        * sFence;
    stlInt64           sCharacterLength;
    stlChar          * sBooleanStr;
    stlChar          * sBooleanLowerStr;

    DTL_CHECK_TYPE( DTL_TYPE_BOOLEAN, aValue, aErrorStack );
    DTL_PARAM_VALIDATE( aString != NULL, aErrorStack );
    DTL_PARAM_VALIDATE( aAvailableSize >= DTL_BOOLEAN_SIZE, aErrorStack );
    /* aStringLength 길이 체크 */
    DTL_PARAM_VALIDATE( (aStringLength - (aStringLength / STL_UINT32_SIZE * STL_UINT32_SIZE)) == 0,
                        aErrorStack );

    *aSuccessWithInfo = STL_FALSE;

    /*
     * Boolean 타입은 ascii 문자만 처리하면 되므로,
     * db cahracter set 으로의 conversion 은 하지 않아도 된다.
     */
    
    /* string value로부터 Boolean 타입의 value 구성 */
    sCharacterLength = aStringLength / STL_UINT32_SIZE;
    sFence = (stlUInt32*)aString + sCharacterLength - 1;
    
    /* truncate leading space */
    sStartPos = (stlUInt32*)aString;
    while( sCharacterLength >= 0 )
    {
        if( stlIsspace(*sStartPos) )
        {
            sStartPos++;
            sCharacterLength--;
        }
        else
        {
            break;
        }
    }
    
    /* truncate trailing space */
    while( sFence > sStartPos )
    {
        if( stlIsspace(*sFence) )
        {
            sFence--;
            sCharacterLength--;
        }
        else
        {
            break;
        }
    }

    sValue = aValue->mValue;

    switch( sCharacterLength )
    {
        case DTL_BOOLEAN_STRING_TRUE_SIZE:
            {
                sBooleanStr = DTL_BOOLEAN_STRING_TRUE;
                sBooleanLowerStr = DTL_BOOLEAN_LOWER_STRING_TRUE;
  
                /*
                 * stlStrncasecmp
                 */
                if( ( (sStartPos[0] == sBooleanStr[0]) || (sStartPos[0] == sBooleanLowerStr[0]) ) && 
                    ( (sStartPos[1] == sBooleanStr[1]) || (sStartPos[1] == sBooleanLowerStr[1]) ) && 
                    ( (sStartPos[2] == sBooleanStr[2]) || (sStartPos[2] == sBooleanLowerStr[2]) ) && 
                    ( (sStartPos[3] == sBooleanStr[3]) || (sStartPos[3] == sBooleanLowerStr[3]) ) ) 
                {
                    /* Do Nothing */
                }
                else
                {
                    STL_THROW( ERROR_INVALID_INPUT_DATA );
                }
                    
                *sValue = (dtlBooleanType)STL_TRUE;
                break;
            }
        case DTL_BOOLEAN_STRING_FALSE_SIZE:
            {
                sBooleanStr = DTL_BOOLEAN_STRING_FALSE;
                sBooleanLowerStr = DTL_BOOLEAN_LOWER_STRING_FALSE;

                /*
                 * stlStrncasecmp
                 */
                if( ( (sStartPos[0] == sBooleanStr[0]) || (sStartPos[0] == sBooleanLowerStr[0]) ) && 
                    ( (sStartPos[1] == sBooleanStr[1]) || (sStartPos[1] == sBooleanLowerStr[1]) ) && 
                    ( (sStartPos[2] == sBooleanStr[2]) || (sStartPos[2] == sBooleanLowerStr[2]) ) && 
                    ( (sStartPos[3] == sBooleanStr[3]) || (sStartPos[3] == sBooleanLowerStr[3]) ) && 
                    ( (sStartPos[4] == sBooleanStr[4]) || (sStartPos[4] == sBooleanLowerStr[4]) ) ) 
                {
                    /* Do Nothing */
                }
                else
                {
                    STL_THROW( ERROR_INVALID_INPUT_DATA );
                }
                
                *sValue = (dtlBooleanType)STL_FALSE;
                break;
            }
        default :
            {
                STL_THROW( ERROR_INVALID_INPUT_DATA );
                break;
            }
    }

    aValue->mLength = DTL_BOOLEAN_SIZE;

    return STL_SUCCESS;

    STL_CATCH( ERROR_INVALID_INPUT_DATA )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_DATA_VALUE_NOT_BOOLEAN_LITERAL,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
