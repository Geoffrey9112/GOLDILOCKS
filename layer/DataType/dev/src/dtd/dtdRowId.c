/*******************************************************************************
 * dtdRowId.c
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
 * @file dtdRowId.c
 * @brief DataType Layer RowID type 정의
 */

#include <dtl.h>
#include <dtdDataType.h>
#include <dtdDef.h>

/**
 * @addtogroup dtlRowId
 * @{
 */

/**
 * @brief RowId 타입의 length값을 얻음
 * @param[in]  aPrecision  사용하지 않음
 * @param[in]  aStringUnit string length unit (dtlStringLengthUnit 참조) 
 * @param[out] aLength     Integer의 길이 값
 * @param[in]  aVectorFunc Function Vector
 * @param[in]  aVectorArg  Vector Argument
 * @param[out] aErrorStack 에러 스택
 */
stlStatus dtdRowIdGetLength( stlInt64              aPrecision,
                             dtlStringLengthUnit   aStringUnit,
                             stlInt64            * aLength,
                             dtlFuncVector       * aVectorFunc,
                             void                * aVectorArg,
                             stlErrorStack       * aErrorStack )
{
    *aLength = DTL_ROWID_SIZE;
    
    return STL_SUCCESS;
}


/**
 * @brief RowId 타입의 length값을 얻음 (입력 문자열에 영향을 받지 않음)
 * @param[in]  aPrecision    사용하지 않음
 * @param[in]  aScale        사용하지 않음
 * @param[in]  aStringUnit   string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aString       입력 문자열
 * @param[in]  aStringLength aString의 길이    
 * @param[out] aLength       Integer의 길이 값
 * @param[in]  aVectorFunc   Function Vector
 * @param[in]  aVectorArg    Vector Argument
 * @param[out] aErrorStack   에러 스택
 */
stlStatus dtdRowIdGetLengthFromString( stlInt64              aPrecision,
                                       stlInt64              aScale,
                                       dtlStringLengthUnit   aStringUnit,
                                       stlChar             * aString,
                                       stlInt64              aStringLength,
                                       stlInt64            * aLength,
                                       dtlFuncVector       * aVectorFunc,
                                       void                * aVectorArg,
                                       stlErrorStack       * aErrorStack )
{
    *aLength = DTL_ROWID_SIZE;
    
    return STL_SUCCESS;
}


/* /\** */
/*  * @brief RowId 타입의 length값을 얻음 (입력 숫자에 영향을 받지 않음) */
/*  * @param[in]  aPrecision  사용하지 않음 */
/*  * @param[in]  aScale      사용하지 않음 */
/*  * @param[in]  aInteger    입력 숫자 */
/*  * @param[out] aLength     Integer의 길이 값 */
/*  * @param[out] aErrorStack 에러 스택 */
/*  *\/ */
/* stlStatus dtdRowIdGetLengthFromInteger( stlInt64          aPrecision, */
/*                                         stlInt64          aScale, */
/*                                         stlInt64          aInteger, */
/*                                         stlInt64        * aLength, */
/*                                         stlErrorStack   * aErrorStack ) */
/* { */
/*     stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                   DTL_ERRCODE_NOT_IMPLEMENTED, */
/*                   NULL, */
/*                   aErrorStack, */
/*                   "dtdRowIdGetLengthFromInteger()" ); */

/*     return STL_FAILURE; */
/* } */


/* /\** */
/*  * @brief RowId 타입의 length값을 얻음 (입력 숫자에 영향을 받지 않음) */
/*  * @param[in]  aPrecision  사용하지 않음 */
/*  * @param[in]  aScale      사용하지 않음 */
/*  * @param[in]  aReal       입력 숫자 */
/*  * @param[out] aLength     Integer의 길이 값 */
/*  * @param[out] aErrorStack 에러 스택 */
/*  *\/ */
/* stlStatus dtdRowIdGetLengthFromReal( stlInt64          aPrecision, */
/*                                      stlInt64          aScale, */
/*                                      stlFloat64        aReal, */
/*                                      stlInt64        * aLength, */
/*                                      stlErrorStack   * aErrorStack ) */
/* { */
/*     stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                   DTL_ERRCODE_NOT_IMPLEMENTED, */
/*                   NULL, */
/*                   aErrorStack, */
/*                   "dtdRowIdGetLengthFromReal()" ); */

/*     return STL_FAILURE; */
/* } */


/**
 * @brief string value로부터 RowId 타입의 value 구성
 * @param[in]  aString            value에 저장될 입력 문자열
 * @param[in]  aStringLength      aString의 길이   
 * @param[in]  aPrecision         사용하지 않음
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
stlStatus dtdRowIdSetValueFromString( stlChar              * aString,
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
    dtlRowIdType  * sRowIdType;
    stlChar       * sString;
    stlInt32        i;
    stlInt64        sValue = 0;
    
    DTL_PARAM_VALIDATE( aAvailableSize >= DTL_ROWID_SIZE, aErrorStack );
    DTL_CHECK_TYPE( DTL_TYPE_ROWID, aValue, aErrorStack );

    STL_PARAM_VALIDATE( aString != NULL, aErrorStack );

    STL_TRY_THROW( aStringLength == DTL_ROWID_TO_STRING_LENGTH, ERROR_INVALID_ROWID );

    *aSuccessWithInfo = STL_FALSE;
    
    sRowIdType = (dtlRowIdType *)(aValue->mValue);
    sString = aString;

    /*
     * 입력으로 들어온 string의 각 문자가 BASE64문자 범위에 속하는지 검사.
     */
    
    for( i = 0; i < DTL_ROWID_TO_STRING_LENGTH; i++ )
    {
        /*
         * dtdBase64lookup[256] 내에서 반환되는 문자가 BASE64 범위의 문자인지 검사.
         */
        STL_TRY_THROW( dtdBase64lookup[(stlUInt8)(sString[i])] != -1,
                       ERROR_INVALID_ROWID );
    }

    /*
     * OBJECT ID
     */

    sValue = (stlInt64)( (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[0]])) << 60) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[1]])) << 54) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[2]])) << 48) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[3]])) << 42) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[4]])) << 36) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[5]])) << 30) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[6]])) << 24) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[7]])) << 18) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[8]])) << 12) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[9]])) << 6)  |
                         ((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[10]])) );
    
    DTD_OBJECT_ID_TO_ROWID_DIGIT( sValue, sRowIdType );
    
    /*
     * TABLESPACE ID
     */

    sValue = (stlInt64)( (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[11]])) << 12) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[12]])) << 6)  |
                         ((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[13]])) );
    

    DTD_TABLESPACE_ID_TO_ROWID_DIGIT( (stlUInt16)sValue, sRowIdType );

    /*
     * PAGE ID
     */

    sValue = (stlInt64)( (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[14]])) << 30) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[15]])) << 24) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[16]])) << 18) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[17]])) << 12) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[18]])) << 6)  |
                         ((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[19]])) );
    
    DTD_PAGE_ID_TO_ROWID_DIGIT( (stlUInt32)sValue, sRowIdType );    
    
    /*
     * PAGE OFFSET
     */

    sValue = (stlInt64)( (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[20]])) << 12) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[21]])) << 6)  |
                         ((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[22]])) );
    
    DTD_ROW_NUMBER_TO_ROWID_DIGIT( (stlInt16)sValue, sRowIdType );

    aValue->mLength = DTL_ROWID_SIZE;

    return STL_SUCCESS;

    STL_CATCH( ERROR_INVALID_ROWID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_ROWID,
                      NULL,
                      aErrorStack );
    };

    STL_FINISH;

    return STL_FAILURE;    
}


/* /\** */
/*  * @brief integer value로부터 RowId 타입의 value 구성 */
/*  * @param[in]  aInteger           value에 저장될 입력 숫자 */
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
/* stlStatus dtdRowIdSetValueFromInteger( stlInt64               aInteger, */
/*                                        stlInt64               aPrecision, */
/*                                        stlInt64               aScale, */
/*                                        dtlStringLengthUnit    aStringUnit, */
/*                                        dtlIntervalIndicator   aIntervalIndicator, */
/*                                        stlInt64               aAvailableSize, */
/*                                        dtlDataValue         * aValue, */
/*                                        stlBool              * aSuccessWithInfo, */
/*                                        dtlFuncVector        * aVectorFunc, */
/*                                        void                 * aVectorArg, */
/*                                        stlErrorStack        * aErrorStack ) */
/* { */
/*     stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                   DTL_ERRCODE_NOT_IMPLEMENTED, */
/*                   NULL, */
/*                   aErrorStack, */
/*                   "dtdRowIdSetValueFromInteger()" ); */

/*     return STL_FAILURE; */
/* } */


/* /\** */
/*  * @brief real value로부터 RowId 타입의 value 구성 */
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
/* stlStatus dtdRowIdSetValueFromReal( stlFloat64             aReal, */
/*                                     stlInt64               aPrecision, */
/*                                     stlInt64               aScale, */
/*                                     dtlStringLengthUnit    aStringUnit, */
/*                                     dtlIntervalIndicator   aIntervalIndicator, */
/*                                     stlInt64               aAvailableSize, */
/*                                     dtlDataValue         * aValue, */
/*                                     stlBool              * aSuccessWithInfo, */
/*                                     dtlFuncVector        * aVectorFunc, */
/*                                     void                 * aVectorArg, */
/*                                     stlErrorStack        * aErrorStack ) */
/* { */
/*     stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                   DTL_ERRCODE_NOT_IMPLEMENTED, */
/*                   NULL, */
/*                   aErrorStack, */
/*                   "dtdRowIdSetValueFromReal()" ); */

/*     return STL_FAILURE; */
/* } */


/**
 * @brief Byte Order 변경 (redirect 함수 아님, 내부 함수)
 * @param[in]  aValue          byte swapping 대상 및 결과 
 * @param[in]  aIsSameEndian   byte swapping 필요 여부
 * @param[out] aErrorStack     에러 스택
 */
stlStatus dtdRowIdReverseByteOrder( void            * aValue,
                                    stlBool           aIsSameEndian,
                                    stlErrorStack   * aErrorStack )
{
    /* Do Nothing */
    
    return STL_SUCCESS;
}


/**
 * @brief RowId value를 문자열로 변환
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
stlStatus dtdRowIdToString( dtlDataValue    * aValue,
                            stlInt64          aPrecision,
                            stlInt64          aScale,
                            stlInt64          aAvailableSize,
                            void            * aBuffer,
                            stlInt64        * aLength,
                            dtlFuncVector   * aVectorFunc,
                            void            * aVectorArg,
                            stlErrorStack   * aErrorStack )
{
    STL_TRY( dtdToStringFromDtlRowId( (dtlRowIdType *)(aValue->mValue),
                                      aAvailableSize,
                                      (stlChar *)aBuffer,
                                      aLength,
                                      aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief RowId value를 사용가능한 버퍼사이즈에 맞게 잘라서 문자열로 변환한다.
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
stlStatus dtdRowIdToStringForAvailableSize( dtlDataValue    * aValue,
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
    stlChar    sRowIdBuffer[DTL_ROWID_TO_STRING_LENGTH];

    DTL_CHECK_TYPE( DTL_TYPE_ROWID, aValue, aErrorStack );
    STL_PARAM_VALIDATE( aBuffer != NULL, aErrorStack );

    if( aAvailableSize >= DTL_ROWID_TO_STRING_LENGTH )
    {
        STL_TRY( dtdToStringFromDtlRowId( (dtlRowIdType *)(aValue->mValue),
                                          aAvailableSize,
                                          (stlChar *)aBuffer,
                                          &sLength,
                                          aErrorStack )
             == STL_SUCCESS );
    }
    else
    {
        STL_TRY( dtdToStringFromDtlRowId( (dtlRowIdType *)(aValue->mValue),
                                          DTL_ROWID_TO_STRING_LENGTH,
                                          sRowIdBuffer,
                                          &sLength,
                                          aErrorStack )
             == STL_SUCCESS );
        
        sLength = ( sLength >= aAvailableSize ) ? aAvailableSize : sLength;
        stlMemcpy( aBuffer, sRowIdBuffer, sLength );        
    }

    *aLength = sLength;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief utf16 string value로부터 RowId 타입의 value 구성
 * @param[in]  aString            value에 저장될 입력 문자열
 * @param[in]  aStringLength      aString의 길이   
 * @param[in]  aPrecision         사용하지 않음
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
stlStatus dtdRowIdSetValueFromUtf16WCharString( void                 * aString,
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
    dtlRowIdType  * sRowIdType;
    stlUInt16     * sString;
    stlInt32        i;
    stlInt64        sValue = 0;
    
    DTL_PARAM_VALIDATE( aAvailableSize >= DTL_ROWID_SIZE, aErrorStack );
    DTL_CHECK_TYPE( DTL_TYPE_ROWID, aValue, aErrorStack );

    STL_PARAM_VALIDATE( aString != NULL, aErrorStack );

    STL_TRY_THROW( (aStringLength / STL_UINT16_SIZE) == DTL_ROWID_TO_STRING_LENGTH, 
                   ERROR_INVALID_ROWID );

    *aSuccessWithInfo = STL_FALSE;
    
    sRowIdType = (dtlRowIdType *)(aValue->mValue);
    sString = (stlUInt16*)aString;

    /*
     * 입력으로 들어온 string의 각 문자가 BASE64문자 범위에 속하는지 검사.
     */
    
    for( i = 0; i < DTL_ROWID_TO_STRING_LENGTH; i++ )
    {
        /*
         * dtdBase64lookup[256] 내에서 반환되는 문자가 BASE64 범위의 문자인지 검사.
         */

        STL_TRY_THROW( sString[i] < 0x80, ERROR_INVALID_ROWID );

        STL_TRY_THROW( dtdBase64lookup[(stlUInt8)(sString[i])] != -1,
                       ERROR_INVALID_ROWID );
    }

    /*
     * OBJECT ID
     */

    sValue = (stlInt64)( (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[0]])) << 60) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[1]])) << 54) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[2]])) << 48) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[3]])) << 42) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[4]])) << 36) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[5]])) << 30) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[6]])) << 24) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[7]])) << 18) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[8]])) << 12) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[9]])) << 6)  |
                         ((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[10]])) );
    
    DTD_OBJECT_ID_TO_ROWID_DIGIT( sValue, sRowIdType );
    
    /*
     * TABLESPACE ID
     */

    sValue = (stlInt64)( (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[11]])) << 12) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[12]])) << 6)  |
                         ((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[13]])) );
    

    DTD_TABLESPACE_ID_TO_ROWID_DIGIT( (stlUInt16)sValue, sRowIdType );

    /*
     * PAGE ID
     */

    sValue = (stlInt64)( (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[14]])) << 30) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[15]])) << 24) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[16]])) << 18) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[17]])) << 12) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[18]])) << 6)  |
                         ((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[19]])) );
    
    DTD_PAGE_ID_TO_ROWID_DIGIT( (stlUInt32)sValue, sRowIdType );    
    
    /*
     * PAGE OFFSET
     */

    sValue = (stlInt64)( (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[20]])) << 12) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[21]])) << 6)  |
                         ((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[22]])) );
    
    DTD_ROW_NUMBER_TO_ROWID_DIGIT( (stlInt16)sValue, sRowIdType );

    aValue->mLength = DTL_ROWID_SIZE;

    return STL_SUCCESS;

    STL_CATCH( ERROR_INVALID_ROWID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_ROWID,
                      NULL,
                      aErrorStack );
    };

    STL_FINISH;

    return STL_FAILURE;    
}


/**
 * @brief utf32 string value로부터 RowId 타입의 value 구성
 * @param[in]  aString            value에 저장될 입력 문자열
 * @param[in]  aStringLength      aString의 길이   
 * @param[in]  aPrecision         사용하지 않음
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
stlStatus dtdRowIdSetValueFromUtf32WCharString( void                 * aString,
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
    dtlRowIdType  * sRowIdType;
    stlUInt32     * sString;
    stlInt32        i;
    stlInt64        sValue = 0;
    
    DTL_PARAM_VALIDATE( aAvailableSize >= DTL_ROWID_SIZE, aErrorStack );
    DTL_CHECK_TYPE( DTL_TYPE_ROWID, aValue, aErrorStack );

    STL_PARAM_VALIDATE( aString != NULL, aErrorStack );

    STL_TRY_THROW( (aStringLength / STL_UINT32_SIZE) == DTL_ROWID_TO_STRING_LENGTH, 
                   ERROR_INVALID_ROWID );

    *aSuccessWithInfo = STL_FALSE;
    
    sRowIdType = (dtlRowIdType *)(aValue->mValue);
    sString = (stlUInt32*)aString;

    /*
     * 입력으로 들어온 string의 각 문자가 BASE64문자 범위에 속하는지 검사.
     */
    
    for( i = 0; i < DTL_ROWID_TO_STRING_LENGTH; i++ )
    {
        /*
         * dtdBase64lookup[256] 내에서 반환되는 문자가 BASE64 범위의 문자인지 검사.
         */

        STL_TRY_THROW( sString[i] < 0x80, ERROR_INVALID_ROWID );

        STL_TRY_THROW( dtdBase64lookup[(stlUInt8)(sString[i])] != -1,
                       ERROR_INVALID_ROWID );
    }

    /*
     * OBJECT ID
     */

    sValue = (stlInt64)( (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[0]])) << 60) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[1]])) << 54) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[2]])) << 48) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[3]])) << 42) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[4]])) << 36) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[5]])) << 30) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[6]])) << 24) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[7]])) << 18) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[8]])) << 12) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[9]])) << 6)  |
                         ((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[10]])) );
    
    DTD_OBJECT_ID_TO_ROWID_DIGIT( sValue, sRowIdType );
    
    /*
     * TABLESPACE ID
     */

    sValue = (stlInt64)( (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[11]])) << 12) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[12]])) << 6)  |
                         ((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[13]])) );
    

    DTD_TABLESPACE_ID_TO_ROWID_DIGIT( (stlUInt16)sValue, sRowIdType );

    /*
     * PAGE ID
     */

    sValue = (stlInt64)( (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[14]])) << 30) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[15]])) << 24) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[16]])) << 18) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[17]])) << 12) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[18]])) << 6)  |
                         ((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[19]])) );
    
    DTD_PAGE_ID_TO_ROWID_DIGIT( (stlUInt32)sValue, sRowIdType );    
    
    /*
     * PAGE OFFSET
     */

    sValue = (stlInt64)( (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[20]])) << 12) |
                         (((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[21]])) << 6)  |
                         ((stlUInt64)(dtdBase64lookup[(stlUInt8)sString[22]])) );
    
    DTD_ROW_NUMBER_TO_ROWID_DIGIT( (stlInt16)sValue, sRowIdType );

    aValue->mLength = DTL_ROWID_SIZE;

    return STL_SUCCESS;

    STL_CATCH( ERROR_INVALID_ROWID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_ROWID,
                      NULL,
                      aErrorStack );
    };

    STL_FINISH;

    return STL_FAILURE;    
}


/**
 * @brief RowId value를 utf16 문자열로 변환
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
stlStatus dtdRowIdToUtf16WCharString( dtlDataValue    * aValue,
                                      stlInt64          aPrecision,
                                      stlInt64          aScale,
                                      stlInt64          aAvailableSize,
                                      void            * aBuffer,
                                      stlInt64        * aLength,
                                      dtlFuncVector   * aVectorFunc,
                                      void            * aVectorArg,
                                      stlErrorStack   * aErrorStack )
{
    STL_TRY( dtdToUtf16WCharStringFromDtlRowId( (dtlRowIdType *)(aValue->mValue),
                                                aAvailableSize,
                                                (stlUInt16 *)aBuffer,
                                                aLength,
                                                aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief RowId value를 utf32 문자열로 변환
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
stlStatus dtdRowIdToUtf32WCharString( dtlDataValue    * aValue,
                                      stlInt64          aPrecision,
                                      stlInt64          aScale,
                                      stlInt64          aAvailableSize,
                                      void            * aBuffer,
                                      stlInt64        * aLength,
                                      dtlFuncVector   * aVectorFunc,
                                      void            * aVectorArg,
                                      stlErrorStack   * aErrorStack )
{
    STL_TRY( dtdToUtf32WCharStringFromDtlRowId( (dtlRowIdType *)(aValue->mValue),
                                                aAvailableSize,
                                                (stlUInt32 *)aBuffer,
                                                aLength,
                                                aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */

