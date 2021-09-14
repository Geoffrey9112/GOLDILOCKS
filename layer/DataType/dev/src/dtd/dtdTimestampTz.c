/*******************************************************************************
 * dtdTimestampTz.c
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
 * @file dtdTimestampTz.c
 * @brief DataType Layer TimestampTz type 정의
 */

#include <dtl.h>

#include <dtdDef.h>
#include <dtdDataType.h>
#include <dtfFormatting.h>

/**
 * @addtogroup dtlTimestampTz
 * @{
 */

/**
 * @brief TimestampTz 타입의 length값을 얻음
 * @param[in]  aPrecision
 * @param[in]  aStringUnit string length unit (dtlStringLengthUnit 참조) 
 * @param[out] aLength     TimestampTz의 길이 값
 * @param[in]  aVectorFunc Function Vector
 * @param[in]  aVectorArg  Vector Argument
 * @param[out] aErrorStack 에러 스택
 */
stlStatus dtdTimestampTzGetLength( stlInt64              aPrecision,
                                   dtlStringLengthUnit   aStringUnit,
                                   stlInt64            * aLength,
                                   dtlFuncVector       * aVectorFunc,
                                   void                * aVectorArg,
                                   stlErrorStack       * aErrorStack )
{
    /* DTL_PARAM_VALIDATE( (aPrecision >= DTL_TIMESTAMPTZ_MIN_PRECISION) && */
    /*                     (aPrecision <= DTL_TIMESTAMPTZ_MAX_PRECISION), */
    /*                     aErrorStack ); */

    *aLength = DTL_TIMESTAMPTZ_SIZE;
    
    return STL_SUCCESS;

    /* STL_FINISH; */

    /* return STL_FAILURE; */
}


/**
 * @brief TimestampTz 타입의 length값을 얻음 (입력 문자열에 영향을 받지 않음)
 * @param[in]  aPrecision  
 * @param[in]  aScale
 * @param[in]  aStringUnit   string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aString       입력 문자열
 * @param[in]  aStringLength aString의 길이    
 * @param[out] aLength       TimestampTz의 길이 값
 * @param[in]  aVectorFunc   Function Vector
 * @param[in]  aVectorArg    Vector Argument
 * @param[out] aErrorStack   에러 스택
 */
stlStatus dtdTimestampTzGetLengthFromString( stlInt64              aPrecision,
                                             stlInt64              aScale,
                                             dtlStringLengthUnit   aStringUnit,
                                             stlChar             * aString,
                                             stlInt64              aStringLength,
                                             stlInt64            * aLength,
                                             dtlFuncVector       * aVectorFunc,
                                             void                * aVectorArg,
                                             stlErrorStack       * aErrorStack )
{
    DTL_PARAM_VALIDATE( (aPrecision >= DTL_TIMESTAMPTZ_MIN_PRECISION) &&
                        (aPrecision <= DTL_TIMESTAMPTZ_MAX_PRECISION),
                        aErrorStack );

    *aLength = DTL_TIMESTAMPTZ_SIZE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/* /\** */
/*  * @brief TimestampTz 타입의 length값을 얻음 (입력 숫자에 영향을 받지 않음) */
/*  * @param[in]  aPrecision   */
/*  * @param[in]  aScale       */
/*  * @param[in]  aInteger    입력 숫자 */
/*  * @param[out] aLength     TimestampTz의 길이 값 */
/*  * @param[out] aErrorStack 에러 스택 */
/*  *\/ */
/* stlStatus dtdTimestampTzGetLengthFromInteger( stlInt64          aPrecision, */
/*                                               stlInt64          aScale, */
/*                                               stlInt64          aInteger, */
/*                                               stlInt64        * aLength, */
/*                                               stlErrorStack   * aErrorStack ) */
/* { */
/*     /\** */
/*      * TIMESTAMPTZ 타입의 경우, Integer로부터 length를 구하는 경우는 없음. */
/*      * 이전에 이미 function이나 casting을 통해 string으로 처리되는 과정을 거쳤을것임. */
/*      *\/ */
    
/*     stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                   DTL_ERRCODE_NOT_IMPLEMENTED, */
/*                   NULL, */
/*                   aErrorStack, */
/*                   "dtdTimestampTzGetLengthFromInteger()" ); */

/*     return STL_FAILURE; */
/* } */

/* /\** */
/*  * @brief TimestampTz 타입의 length값을 얻음 (입력 숫자에 영향을 받지 않음) */
/*  * @param[in]  aPrecision   */
/*  * @param[in]  aScale       */
/*  * @param[in]  aReal       입력 숫자 */
/*  * @param[out] aLength     Date의 길이 값 */
/*  * @param[out] aErrorStack 에러 스택 */
/*  *\/ */
/* stlStatus dtdTimestampTzGetLengthFromReal( stlInt64          aPrecision, */
/*                                            stlInt64          aScale, */
/*                                            stlFloat64        aReal, */
/*                                            stlInt64        * aLength, */
/*                                            stlErrorStack   * aErrorStack ) */
/* { */
/*     stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                   DTL_ERRCODE_NOT_IMPLEMENTED, */
/*                   NULL, */
/*                   aErrorStack, */
/*                   "dtdTimestampTzGetLengthFromReal()" ); */

/*     return STL_FAILURE; */
/* } */


/* /\** */
/*  * @brief integer value로부터 TimestampTz 타입의 value 구성 */
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
/* stlStatus dtdTimestampTzSetValueFromInteger( stlInt64               aInteger, */
/*                                              stlInt64               aPrecision, */
/*                                              stlInt64               aScale, */
/*                                              dtlStringLengthUnit    aStringUnit, */
/*                                              dtlIntervalIndicator   aIntervalIndicator, */
/*                                              stlInt64               aAvailableSize, */
/*                                              dtlDataValue         * aValue, */
/*                                              stlBool              * aSuccessWithInfo, */
/*                                              dtlFuncVector        * aVectorFunc, */
/*                                              void                 * aVectorArg, */
/*                                              stlErrorStack        * aErrorStack ) */
/* { */
/*     stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                   DTL_ERRCODE_NOT_IMPLEMENTED, */
/*                   NULL, */
/*                   aErrorStack, */
/*                   "dtdTimestampTzSetValueFromInteger()" ); */

/*     return STL_FAILURE; */
/* } */

/* /\** */
/*  * @brief real value로부터 Date 타입의 value 구성 */
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
/* stlStatus dtdTimestampTzSetValueFromReal( stlFloat64             aReal, */
/*                                           stlInt64               aPrecision, */
/*                                           stlInt64               aScale, */
/*                                           dtlStringLengthUnit    aStringUnit, */
/*                                           dtlIntervalIndicator   aIntervalIndicator, */
/*                                           stlInt64               aAvailableSize, */
/*                                           dtlDataValue         * aValue, */
/*                                           stlBool              * aSuccessWithInfo, */
/*                                           dtlFuncVector        * aVectorFunc, */
/*                                           void                 * aVectorArg, */
/*                                           stlErrorStack        * aErrorStack ) */
/* { */
/*     stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                   DTL_ERRCODE_NOT_IMPLEMENTED, */
/*                   NULL, */
/*                   aErrorStack, */
/*                   "dtdTimestampTzSetValueFromReal()" ); */

/*     return STL_FAILURE; */
/* } */

/**
 * @brief Byte Order 변경 (redirect 함수 아님, 내부 함수)
 * @param[in]  aValue          byte swapping 대상 및 결과 
 * @param[in]  aIsSameEndian   byte swapping 필요 여부
 * @param[out] aErrorStack     에러 스택
 */
stlStatus dtdTimestampTzReverseByteOrder( void            * aValue,
                                          stlBool           aIsSameEndian,
                                          stlErrorStack   * aErrorStack )
{
    DTL_PARAM_VALIDATE( aValue != NULL, aErrorStack );

    if( aIsSameEndian == STL_FALSE )
    {
        dtdReverseByteOrder( (stlChar*)&(((dtlTimestampTzType*)aValue)->mTimestamp),
                             STL_SIZEOF((((dtlTimestampTzType*)aValue)->mTimestamp)),
                             aValue );

        dtdReverseByteOrder(
            (stlChar*)&(((dtlTimestampTzType*)aValue)->mTimeZone),
            STL_SIZEOF((((dtlTimestampTzType*)aValue)->mTimeZone)),
            (stlChar*)aValue + STL_SIZEOF((((dtlTimestampTzType*)aValue)->mTimestamp)) );
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
 * @brief TimestampTz value를 문자열로 변환
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
stlStatus dtdTimestampTzToString( dtlDataValue    * aValue,
                                  stlInt64          aPrecision,
                                  stlInt64          aScale,
                                  stlInt64          aAvailableSize,
                                  void            * aBuffer,
                                  stlInt64        * aLength,
                                  dtlFuncVector   * aVectorFunc,
                                  void            * aVectorArg,
                                  stlErrorStack   * aErrorStack )
{
    dtlTimestampTzType       sTimestampTzType;
    dtlExpTime               sDtlExpTime;
    dtlFractionalSecond      sFractionalSecond;
    stlInt32                 sTimeZone;
    dtlDateTimeFormatInfo  * sToCharFormatInfo;    

    DTL_CHECK_TYPE( DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, aValue, aErrorStack );
    STL_PARAM_VALIDATE( aBuffer != NULL, aErrorStack );

    sToCharFormatInfo = aVectorFunc->mGetTimestampWithTimeZoneFormatInfoFunc( aVectorArg );
    
    DTL_INIT_DTLEXPTIME( sDtlExpTime );

    sTimestampTzType.mTimestamp = ((dtlTimestampTzType *)(aValue->mValue))->mTimestamp;
    sTimestampTzType.mTimeZone = ((dtlTimestampTzType *)(aValue->mValue))->mTimeZone;

    STL_TRY( dtdTimestampTz2dtlExpTime( sTimestampTzType,
                                        & sTimeZone,
                                        & sDtlExpTime,
                                        & sFractionalSecond,
                                        aErrorStack )
             == STL_SUCCESS );

    STL_TRY( dtfDateTimeToChar( sToCharFormatInfo,
                                & sDtlExpTime,
                                sFractionalSecond, 
                                & sTimeZone,
                                aAvailableSize,
                                (stlChar *)aBuffer,
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
 * @brief TimestampTz value를 사용가능한 버퍼사이즈에 맞게 잘라서 문자열로 변환한다.
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
stlStatus dtdTimestampTzToStringForAvailableSize( dtlDataValue    * aValue,
                                                  stlInt64          aPrecision,
                                                  stlInt64          aScale,
                                                  stlInt64          aAvailableSize,
                                                  void            * aBuffer,
                                                  stlInt64        * aLength,
                                                  dtlFuncVector   * aVectorFunc,
                                                  void            * aVectorArg,
                                                  stlErrorStack   * aErrorStack )
{
    DTL_CHECK_TYPE( DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, aValue, aErrorStack);
    STL_PARAM_VALIDATE( aBuffer != NULL, aErrorStack );

    STL_TRY( dtdDateTimeToStringForAvailableSize( aValue,
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
 * @brief session format 정보를 이용하여 String으로부터 Timestamp with time zone 타입의 value 구성
 * @param[in]  aString            value에 저장될 입력 문자열
 * @param[in]  aStringLength      aString의 길이   
 * @param[in]  aPrecision         timestampTz type의 fractional second precision    
 * @param[in]  aScale             사용하지 않음.
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
 * @remark
 * TIMESTAMP WITH TIME ZONE type에 string으로 입력 가능한 포멧은 SESSION FORMAT 정보이다.
 * ODBC, CDC, GLOADER 는 지정된 default format 정보로 처리된다.
 */
stlStatus dtdTimestampTzSetValueFromString( stlChar              * aString,
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
    dtlDateTimeFormatInfo   * sToDateTimeFormatInfo;
    dtlTimestampTzType      * sTimestampTzType  = NULL;    
    dtlExpTime                sDtlExpTime;
    dtlFractionalSecond       sFractionalSecond = 0;
    stlInt32                  sTimeZone         = 0;   
    
    DTL_PARAM_VALIDATE( aAvailableSize >= DTL_TIMESTAMPTZ_SIZE, aErrorStack );
    DTL_CHECK_TYPE( DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, aValue, aErrorStack );

    STL_PARAM_VALIDATE( aString != NULL, aErrorStack );
    STL_PARAM_VALIDATE( (aPrecision >= DTL_TIMESTAMPTZ_MIN_PRECISION) &&
                        (aPrecision <= DTL_TIMESTAMPTZ_MAX_PRECISION),
                        aErrorStack );

    *aSuccessWithInfo = STL_FALSE;
    
    sToDateTimeFormatInfo =
        aSourceVectorFunc->mGetTimestampWithTimeZoneFormatInfoFunc( aSourceVectorArg );

    DTL_INIT_DTLEXPTIME( sDtlExpTime );
    
    STL_TRY( dtfToDateTimeFromString( DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,
                                      aString,
                                      aStringLength,
                                      sToDateTimeFormatInfo,
                                      & sDtlExpTime,
                                      & sFractionalSecond,
                                      & sTimeZone,
                                      aSourceVectorFunc,
                                      aSourceVectorArg,
                                      aErrorStack )
             == STL_SUCCESS );
    
    /**
     * Timestamp with time zone Type Value 만들기
     */
    
    sTimestampTzType = (dtlTimestampTzType *)(aValue->mValue);
    
    STL_TRY( dtdDtlExpTime2TimestampTz( NULL,
                                        & sDtlExpTime,
                                        sFractionalSecond,
                                        & sTimeZone,
                                        sTimestampTzType,
                                        aErrorStack )
             == STL_SUCCESS );

    STL_TRY( dtdAdjustTimestamp( & (sTimestampTzType->mTimestamp),
                                 aPrecision,
                                 aErrorStack )
             == STL_SUCCESS );
    
    aValue->mLength = DTL_TIMESTAMPTZ_SIZE;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief timestamp 의 typed literal 입력 문자열을 통해 mValue를 구성
 *    <BR> TIMESTAMP'2000-12-15 11:22:33.999999 +09:00'
 *    <BR> TIMESTAMP WITH TIME ZONE'2000-12-15 11:22:33.999999 +09:00'
 *    <BR> format 형식은 SYYYY-MM-DD HH24:MI:SS[.[FF6]] {+|-}TZH:TZM 이 적용된다.
 * @param[in]  aString            value에 저장될 입력 문자열
 * @param[in]  aStringLength      aString의 길이   
 * @param[in]  aAvailableSize     mValue에 사용할 수 있는 메모리 공간의 크기  
 * @param[out] aValue             dtlDataValue의 주소
 * @param[in]  aVectorFunc        Function Vector
 * @param[in]  aVectorArg         Vector Argument
 * @param[out] aErrorStack        에러 스택
 */
stlStatus dtdTimestampTzSetValueFromTypedLiteral( stlChar             * aString,
                                                  stlInt64              aStringLength,
                                                  stlInt64              aAvailableSize,
                                                  dtlDataValue        * aValue,
                                                  dtlFuncVector       * aVectorFunc,
                                                  void                * aVectorArg,
                                                  stlErrorStack       * aErrorStack )
{
    dtlTimestampTzType      * sTimestampTzType  = NULL;    
    dtlExpTime                sDtlExpTime;
    dtlFractionalSecond       sFractionalSecond = 0;
    stlInt32                  sTimeZone         = 0;
    
    stlChar                 * sDateTimeStr      = aString;
    stlInt64                  sDateTimeStrLen   = aStringLength;
    
    DTL_PARAM_VALIDATE( aAvailableSize >= DTL_TIMESTAMPTZ_SIZE, aErrorStack );
    DTL_CHECK_TYPE( DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, aValue, aErrorStack );

    STL_PARAM_VALIDATE( aString != NULL, aErrorStack );

    DTL_INIT_DTLEXPTIME( sDtlExpTime );

    /*
     * timestamp  typed literal 처리
     * ex) TIMESTAMP'2000-12-15 11:22:33.999999 +09:00'
     *     TIMESTAMP WITH TIME ZONE'2000-12-15 11:22:33.999999 +09:00'
     *     format 형식은 SYYYY-MM-DD HH24:MI:SS[.[FF6]] {+|-}TZH:TZM 이 적용된다.
     */

    /* SYYYY-MM-DD 정보를 얻는다. */
    STL_TRY( dtfToDateInfoFromTypedLiteral( DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,
                                            sDateTimeStr,
                                            sDateTimeStrLen,
                                            & sDtlExpTime,
                                            & sDateTimeStr,
                                            & sDateTimeStrLen,
                                            aVectorFunc,
                                            aVectorArg,
                                            aErrorStack )
             == STL_SUCCESS );

    /* HH24:MI:SS[.[FF6]] 정보를 얻는다. */
    STL_TRY( dtfToTimeInfoFromTypedLiteral( DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,
                                            sDateTimeStr,
                                            sDateTimeStrLen,
                                            & sDtlExpTime,
                                            & sFractionalSecond,
                                            & sDateTimeStr,
                                            & sDateTimeStrLen,
                                            aVectorFunc,
                                            aVectorArg,
                                            aErrorStack )
             == STL_SUCCESS );
    
    /* {+|-}TZH:TZM 정보를 얻는다. */
    STL_TRY( dtfToTimeZoneInfoFromTypedLiteral( DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,
                                                sDateTimeStr,
                                                sDateTimeStrLen,
                                                & sTimeZone,
                                                aVectorFunc,
                                                aVectorArg,
                                                aErrorStack )
             == STL_SUCCESS );
    
    /**
     * Timestamp with time zone Type Value 만들기
     */
    
    sTimestampTzType = (dtlTimestampTzType *)(aValue->mValue);
    
    STL_TRY( dtdDtlExpTime2TimestampTz( NULL,
                                        & sDtlExpTime,
                                        sFractionalSecond,
                                        & sTimeZone,
                                        sTimestampTzType,
                                        aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( dtdAdjustTimestamp( & (sTimestampTzType->mTimestamp),
                                 DTL_TIMESTAMPTZ_MAX_PRECISION,
                                 aErrorStack )
             == STL_SUCCESS );
    
    aValue->mLength = DTL_TIMESTAMPTZ_SIZE;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief session format 정보를 이용하여 Utf16 String으로부터 Timestamp with time zone 타입의 value 구성
 * @param[in]  aString            value에 저장될 입력 문자열
 * @param[in]  aStringLength      aString의 길이   
 * @param[in]  aPrecision         timestampTz type의 fractional second precision    
 * @param[in]  aScale             사용하지 않음.
 * @param[in]  aStringUnit      string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aIntervalIndicator 사용하지 않음.
 *                           <BR> INTERVAL 타입의 Indicator dtlIntervalIndicator 참조 
 * @param[in]  aAvailableSize     mValue에 사용할 수 있는 메모리 공간의 크기  
 * @param[out] aValue             dtlDataValue의 주소
 * @param[out] aSuccessWithInfo   warning 발생 여부
 * @param[in]  aVectorFunc    Dest Function Vector
 * @param[in]  aVectorArg     Dest Vector Argument
 * @param[out] aErrorStack        에러 스택
 *
 * @remark
 * TIMESTAMP WITH TIME ZONE type에 string으로 입력 가능한 포멧은 SESSION FORMAT 정보이다.
 * ODBC, CDC, GLOADER 는 지정된 default format 정보로 처리된다.
 */
stlStatus dtdTimestampTzSetValueFromUtf16WCharString( void                 * aString,
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
    dtlDateTimeFormatInfo   * sToDateTimeFormatInfo;
    dtlTimestampTzType      * sTimestampTzType  = NULL;    
    dtlExpTime                sDtlExpTime;
    dtlFractionalSecond       sFractionalSecond = 0;
    stlInt32                  sTimeZone         = 0;   
    
    DTL_PARAM_VALIDATE( aAvailableSize >= DTL_TIMESTAMPTZ_SIZE, aErrorStack );
    DTL_CHECK_TYPE( DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, aValue, aErrorStack );

    STL_PARAM_VALIDATE( aString != NULL, aErrorStack );
    STL_PARAM_VALIDATE( (aPrecision >= DTL_TIMESTAMPTZ_MIN_PRECISION) &&
                        (aPrecision <= DTL_TIMESTAMPTZ_MAX_PRECISION),
                        aErrorStack );

    *aSuccessWithInfo = STL_FALSE;
    
    sToDateTimeFormatInfo = aVectorFunc->mGetTimestampWithTimeZoneFormatInfoFunc( aVectorArg );

    DTL_INIT_DTLEXPTIME( sDtlExpTime );
    
    STL_TRY( dtfToDateTimeFromWCharString( DTL_UNICODE_UTF16,
                                           DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,
                                           aString,
                                           aStringLength,
                                           sToDateTimeFormatInfo,
                                           & sDtlExpTime,
                                           & sFractionalSecond,
                                           & sTimeZone,
                                           aVectorFunc,
                                           aVectorArg,
                                           aErrorStack )
             == STL_SUCCESS );
    
    /**
     * Timestamp with time zone Type Value 만들기
     */
    
    sTimestampTzType = (dtlTimestampTzType *)(aValue->mValue);
    
    STL_TRY( dtdDtlExpTime2TimestampTz( NULL,
                                        & sDtlExpTime,
                                        sFractionalSecond,
                                        & sTimeZone,
                                        sTimestampTzType,
                                        aErrorStack )
             == STL_SUCCESS );

    STL_TRY( dtdAdjustTimestamp( & (sTimestampTzType->mTimestamp),
                                 aPrecision,
                                 aErrorStack )
             == STL_SUCCESS );
    
    aValue->mLength = DTL_TIMESTAMPTZ_SIZE;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief session format 정보를 이용하여 Utf32 String으로부터 Timestamp with time zone 타입의 value 구성
 * @param[in]  aString            value에 저장될 입력 문자열
 * @param[in]  aStringLength      aString의 길이   
 * @param[in]  aPrecision         timestampTz type의 fractional second precision    
 * @param[in]  aScale             사용하지 않음.
 * @param[in]  aStringUnit      string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aIntervalIndicator 사용하지 않음.
 *                           <BR> INTERVAL 타입의 Indicator dtlIntervalIndicator 참조 
 * @param[in]  aAvailableSize     mValue에 사용할 수 있는 메모리 공간의 크기  
 * @param[out] aValue             dtlDataValue의 주소
 * @param[out] aSuccessWithInfo   warning 발생 여부
 * @param[in]  aVectorFunc    Dest Function Vector
 * @param[in]  aVectorArg     Dest Vector Argument
 * @param[out] aErrorStack        에러 스택
 *
 * @remark
 * TIMESTAMP WITH TIME ZONE type에 string으로 입력 가능한 포멧은 SESSION FORMAT 정보이다.
 * ODBC, CDC, GLOADER 는 지정된 default format 정보로 처리된다.
 */
stlStatus dtdTimestampTzSetValueFromUtf32WCharString( void                 * aString,
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
    dtlDateTimeFormatInfo   * sToDateTimeFormatInfo;
    dtlTimestampTzType      * sTimestampTzType  = NULL;    
    dtlExpTime                sDtlExpTime;
    dtlFractionalSecond       sFractionalSecond = 0;
    stlInt32                  sTimeZone         = 0;   
    
    DTL_PARAM_VALIDATE( aAvailableSize >= DTL_TIMESTAMPTZ_SIZE, aErrorStack );
    DTL_CHECK_TYPE( DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, aValue, aErrorStack );

    STL_PARAM_VALIDATE( aString != NULL, aErrorStack );
    STL_PARAM_VALIDATE( (aPrecision >= DTL_TIMESTAMPTZ_MIN_PRECISION) &&
                        (aPrecision <= DTL_TIMESTAMPTZ_MAX_PRECISION),
                        aErrorStack );

    *aSuccessWithInfo = STL_FALSE;
    
    sToDateTimeFormatInfo = aVectorFunc->mGetTimestampWithTimeZoneFormatInfoFunc( aVectorArg );

    DTL_INIT_DTLEXPTIME( sDtlExpTime );
    
    STL_TRY( dtfToDateTimeFromWCharString( DTL_UNICODE_UTF32,
                                           DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,
                                           aString,
                                           aStringLength,
                                           sToDateTimeFormatInfo,
                                           & sDtlExpTime,
                                           & sFractionalSecond,
                                           & sTimeZone,
                                           aVectorFunc,
                                           aVectorArg,
                                           aErrorStack )
             == STL_SUCCESS );
    
    /**
     * Timestamp with time zone Type Value 만들기
     */
    
    sTimestampTzType = (dtlTimestampTzType *)(aValue->mValue);
    
    STL_TRY( dtdDtlExpTime2TimestampTz( NULL,
                                        & sDtlExpTime,
                                        sFractionalSecond,
                                        & sTimeZone,
                                        sTimestampTzType,
                                        aErrorStack )
             == STL_SUCCESS );

    STL_TRY( dtdAdjustTimestamp( & (sTimestampTzType->mTimestamp),
                                 aPrecision,
                                 aErrorStack )
             == STL_SUCCESS );
    
    aValue->mLength = DTL_TIMESTAMPTZ_SIZE;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief TimestampTz value를 utf16 문자열로 변환
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
stlStatus dtdTimestampTzToUtf16WCharString( dtlDataValue    * aValue,
                                            stlInt64          aPrecision,
                                            stlInt64          aScale,
                                            stlInt64          aAvailableSize,
                                            void            * aBuffer,
                                            stlInt64        * aLength,
                                            dtlFuncVector   * aVectorFunc,
                                            void            * aVectorArg,
                                            stlErrorStack   * aErrorStack )
{
    dtlTimestampTzType       sTimestampTzType;
    dtlExpTime               sDtlExpTime;
    dtlFractionalSecond      sFractionalSecond;
    stlInt32                 sTimeZone;
    dtlDateTimeFormatInfo  * sToCharFormatInfo;    

    DTL_CHECK_TYPE( DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, aValue, aErrorStack );
    STL_PARAM_VALIDATE( aBuffer != NULL, aErrorStack );

    sToCharFormatInfo = aVectorFunc->mGetTimestampWithTimeZoneFormatInfoFunc( aVectorArg );
    
    DTL_INIT_DTLEXPTIME( sDtlExpTime );

    sTimestampTzType.mTimestamp = ((dtlTimestampTzType *)(aValue->mValue))->mTimestamp;
    sTimestampTzType.mTimeZone = ((dtlTimestampTzType *)(aValue->mValue))->mTimeZone;

    STL_TRY( dtdTimestampTz2dtlExpTime( sTimestampTzType,
                                        & sTimeZone,
                                        & sDtlExpTime,
                                        & sFractionalSecond,
                                        aErrorStack )
             == STL_SUCCESS );

    STL_TRY( dtfDateTimeToWCharString( DTL_UNICODE_UTF16,
                                       sToCharFormatInfo,
                                       & sDtlExpTime,
                                       sFractionalSecond, 
                                       & sTimeZone,
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
 * @brief TimestampTz value를 utf32 문자열로 변환
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
stlStatus dtdTimestampTzToUtf32WCharString( dtlDataValue    * aValue,
                                            stlInt64          aPrecision,
                                            stlInt64          aScale,
                                            stlInt64          aAvailableSize,
                                            void            * aBuffer,
                                            stlInt64        * aLength,
                                            dtlFuncVector   * aVectorFunc,
                                            void            * aVectorArg,
                                            stlErrorStack   * aErrorStack )
{
    dtlTimestampTzType       sTimestampTzType;
    dtlExpTime               sDtlExpTime;
    dtlFractionalSecond      sFractionalSecond;
    stlInt32                 sTimeZone;
    dtlDateTimeFormatInfo  * sToCharFormatInfo;    

    DTL_CHECK_TYPE( DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, aValue, aErrorStack );
    STL_PARAM_VALIDATE( aBuffer != NULL, aErrorStack );

    sToCharFormatInfo = aVectorFunc->mGetTimestampWithTimeZoneFormatInfoFunc( aVectorArg );
    
    DTL_INIT_DTLEXPTIME( sDtlExpTime );

    sTimestampTzType.mTimestamp = ((dtlTimestampTzType *)(aValue->mValue))->mTimestamp;
    sTimestampTzType.mTimeZone = ((dtlTimestampTzType *)(aValue->mValue))->mTimeZone;

    STL_TRY( dtdTimestampTz2dtlExpTime( sTimestampTzType,
                                        & sTimeZone,
                                        & sDtlExpTime,
                                        & sFractionalSecond,
                                        aErrorStack )
             == STL_SUCCESS );

    STL_TRY( dtfDateTimeToWCharString( DTL_UNICODE_UTF32,
                                       sToCharFormatInfo,
                                       & sDtlExpTime,
                                       sFractionalSecond, 
                                       & sTimeZone,
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



/** @} */

