/*******************************************************************************
 * dtfToChar.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id:$
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file dtfToChar.c
 * @brief Formatting Function DataType Library Layer
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtfFormatting.h>

/**
 * @addtogroup dtfToChar  Formatting
 * @ingroup dtf
 * @internal
 * @{
 */


dtlBuiltInFuncPtr  gDtfToCharFunc[ DTL_TYPE_MAX ] = 
{
    NULL,                          /**< BOOLEAN */
    NULL,                          /**< NATIVE_SMALLINT */
    NULL,                          /**< NATIVE_INTEGER */
    NULL,                          /**< NATIVE_BIGINT */

    dtfRealToChar,                 /**< NATIVE_REAL */
    dtfDoubleToChar,               /**< NATIVE_DOUBLE */

    dtfNumberToChar,               /**< FLOAT */
    dtfNumberToChar,               /**< NUMBER */
    NULL,                          /**< unsupported feature, DECIMAL */
    
    dtfCharStringToChar,           /**< CHARACTER */
    dtfCharStringToChar,           /**< CHARACTER VARYING */
    dtfLongvarcharStringToChar,    /**< CHARACTER LONG VARYING */
    NULL,                          /**< unsupported feature, CHARACTER LARGE OBJECT */
    
    NULL,                          /**< BINARY */
    NULL,                          /**< BINARY VARYING */
    NULL,                          /**< BINARY LONG VARYING */
    NULL,                          /**< unsupported feature, BINARY LARGE OBJECT */

    dtfDateToChar,                 /**< DATE */
    dtfTimeToChar,                 /**< TIME WITHOUT TIME ZONE */
    dtfTimestampToChar,            /**< TIMESTAMP WITHOUT TIME ZONE */
    dtfTimeTzToChar,               /**< TIME WITH TIME ZONE */
    dtfTimestampTzToChar,          /**< TIMESTAMP WITH TIME ZONE */

    dtfIntervalYearToMonthToChar,  /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
    dtfIntervalDayToSecondToChar,  /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */

    NULL                           /**< DTL_TYPE_ROWID */
};


/*******************************************************************************
 * DATE
 *   to_char( DATE, fmt )
 *     to_char( DATE, fmt ) => [ P, O ]
 * ex) to_char(DATE, 'YYYY-MM-DD')
 *******************************************************************************/

/**
 * @brief to_char
 *        <BR> DATE를 format string에 맞게 string으로 변환
 *        <BR> DATE
 *        <BR> to_char( DATE, fmt )
 *        <BR>   to_char( DATE, fmt ) => [ P, O ]
 *        <BR> ex) to_char(DATE, 'YYYY-MM-DD')
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> to_char(DATE, VARCHAR)
 * @param[out] aResultValue      결과 (결과타입 VARCHAR)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[in]  aEnv              environment (stlErrorStack)
 */
stlStatus dtfDateToChar( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv )                  
{
    dtlDataValue         * sValue1;
    dtlDataValue         * sValue2;
    dtlDataValue         * sResultValue;
    stlErrorStack        * sErrorStack;

    dtlDateTimeFormatInfo  * sToCharFormatInfo;
    dtlDateType              sDate = 0;
    dtlExpTime               sDtlExpTime;
    dtlFractionalSecond      sFractionalSecond = 0;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_DATE, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_VARBINARY, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_VARCHAR, sErrorStack );

    sDate = *(dtlDateType *)(sValue1->mValue);
    sToCharFormatInfo = (dtlDateTimeFormatInfo *)(sValue2->mValue);

    DTL_INIT_DTLEXPTIME( sDtlExpTime );

    STL_TRY( dtdDateTime2dtlExpTime( sDate,
                                     & sDtlExpTime,
                                     & sFractionalSecond,
                                     sErrorStack )
             == STL_SUCCESS );    
    
    STL_TRY( dtfDateTimeToChar( sToCharFormatInfo,
                                & sDtlExpTime,
                                0,          // fractional second
                                NULL,       // time zone
                                DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE,
                                (stlChar *)(sResultValue->mValue),
                                & sResultValue->mLength,
                                aVectorFunc,
                                aVectorArg,
                                sErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/*******************************************************************************
 * TIME
 *   to_char( TIME, fmt )
 *     to_char( TIME, fmt ) => [ P ]
 * ex) to_char( TIME, 'HH:MI:SS')
 *******************************************************************************/

/**
 * @brief to_char
 *        <BR> TIME를 format string에 맞게 string으로 변환
 *        <BR> TIME
 *        <BR> to_char( TIME, fmt )
 *        <BR>   to_char( TIME, fmt ) => [ P, O ]
 *        <BR> ex) to_char(TIME, 'HH:MI:SS')
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> to_char(TIME, VARCHAR)
 * @param[out] aResultValue      결과 (결과타입 VARCHAR)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[in]  aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimeToChar( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv )                  
{
    dtlDataValue         * sValue1;
    dtlDataValue         * sValue2;
    dtlDataValue         * sResultValue;
    stlErrorStack        * sErrorStack;

    dtlDateTimeFormatInfo  * sToCharFormatInfo;    
    dtlFractionalSecond      sFractionalSecond;
    dtlExpTime               sDtlExpTime;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_TIME, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_VARBINARY, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_VARCHAR, sErrorStack );
    
    sToCharFormatInfo = (dtlDateTimeFormatInfo *)(sValue2->mValue);
    
    DTL_INIT_DTLEXPTIME( sDtlExpTime );

    dtdTime2dtlExpTime( *(dtlTimeType *)(sValue1->mValue),
                        & sDtlExpTime,
                        & sFractionalSecond );

    STL_TRY( dtfDateTimeToChar( sToCharFormatInfo,
                                & sDtlExpTime,
                                sFractionalSecond, // fractional second
                                NULL,              // time zone
                                DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE,
                                (stlChar *)(sResultValue->mValue),
                                & sResultValue->mLength,
                                aVectorFunc,
                                aVectorArg,
                                sErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/*******************************************************************************
 * TIME WITH TIME ZONE
 *   to_char( TIME WITH TIME ZONE, fmt )
 *     to_char( TIME WITH TIME ZONE, fmt ) => [ P ]
 * ex) to_char( TIME WITH TIME ZONE, 'HH:MI:SS')
 *******************************************************************************/

/**
 * @brief to_char
 *        <BR> TIME WITH TIME ZONE를 format string에 맞게 string으로 변환
 *        <BR> TIME WITH TIME ZONE
 *        <BR> to_char( TIME WITH TIME ZONE, fmt )
 *        <BR>   to_char( TIME WITH TIME ZONE, fmt ) => [ P, O ]
 *        <BR> ex) to_char(TIME WITH TIME ZONE, 'HH:MI:SS')
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> to_char(TIME WITH TIME ZONE, VARCHAR)
 * @param[out] aResultValue      결과 (결과타입 VARCHAR)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[in]  aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimeTzToChar( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aInfo,
                           dtlFuncVector  * aVectorFunc,
                           void           * aVectorArg,
                           void           * aEnv )                  
{
    dtlDataValue         * sValue1;
    dtlDataValue         * sValue2;
    dtlDataValue         * sResultValue;
    stlErrorStack        * sErrorStack;

    dtlDateTimeFormatInfo  * sToCharFormatInfo;        
    dtlFractionalSecond      sFractionalSecond;
    dtlExpTime               sDtlExpTime;
    stlInt32                 sTimeZone;    

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_TIME_WITH_TIME_ZONE, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_VARBINARY, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_VARCHAR, sErrorStack );
    
    sToCharFormatInfo = (dtlDateTimeFormatInfo *)(sValue2->mValue);
    
    DTL_INIT_DTLEXPTIME( sDtlExpTime );

    dtdTimeTz2dtlExpTime( (dtlTimeTzType *)(sValue1->mValue),
                          & sDtlExpTime,
                          & sFractionalSecond,
                          & sTimeZone);

    STL_TRY( dtfDateTimeToChar( sToCharFormatInfo,
                                & sDtlExpTime,
                                sFractionalSecond, // fractional second
                                & sTimeZone,
                                DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE,
                                (stlChar *)(sResultValue->mValue),
                                & sResultValue->mLength,
                                aVectorFunc,
                                aVectorArg,
                                sErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/*******************************************************************************
 * TIMESTAMP
 *   to_char( TIMESTAMP, fmt )
 *     to_char( TIMESTAMP, fmt ) => [ P, O ]
 * ex) to_char( TIMESTAMP, 'HH:MI:SS')
 *******************************************************************************/

/**
 * @brief to_char
 *        <BR> TIMESTAMP를 format string에 맞게 string으로 변환
 *        <BR> TIMESTAMP
 *        <BR> to_char( TIMESTAMP, fmt )
 *        <BR>   to_char( TIMESTAMP, fmt ) => [ P, O ]
 *        <BR> ex) to_char(TIMESTAMP, 'HH:MI:SS')
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> to_char(TIMESTAMP, VARCHAR)
 * @param[out] aResultValue      결과 (결과타입 VARCHAR)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[in]  aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimestampToChar( stlUInt16        aInputArgumentCnt,
                              dtlValueEntry  * aInputArgument,
                              void           * aResultValue,
                              void           * aInfo,
                              dtlFuncVector  * aVectorFunc,
                              void           * aVectorArg,
                              void           * aEnv )                  
{
    dtlDataValue         * sValue1;
    dtlDataValue         * sValue2;
    dtlDataValue         * sResultValue;
    stlErrorStack        * sErrorStack;

    dtlDateTimeFormatInfo  * sToCharFormatInfo;        
    dtlFractionalSecond      sFractionalSecond;
    dtlExpTime               sDtlExpTime;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_TIMESTAMP, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_VARBINARY, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_VARCHAR, sErrorStack );
    
    sToCharFormatInfo = (dtlDateTimeFormatInfo *)(sValue2->mValue);

    DTL_INIT_DTLEXPTIME( sDtlExpTime );

    STL_TRY( dtdDateTime2dtlExpTime( *(dtlTimestampType *)(sValue1->mValue),
                                     & sDtlExpTime,
                                     & sFractionalSecond,
                                     sErrorStack )
             == STL_SUCCESS );

    STL_TRY( dtfDateTimeToChar( sToCharFormatInfo,
                                & sDtlExpTime,
                                sFractionalSecond, // fractional second
                                NULL,              // time zone
                                DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE,
                                (stlChar *)(sResultValue->mValue),
                                & sResultValue->mLength,
                                aVectorFunc,
                                aVectorArg,
                                sErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/*******************************************************************************
 * TIMESTAMP WITH TIME ZONE
 *   to_char( TIMESTAMP WITH TIME ZONE, fmt )
 *     to_char( TIMESTAMP WITH TIME ZONE, fmt ) => [ P, O ]
 * ex) to_char( TIMESTAMP WITH TIME ZONE, 'HH:MI:SS')
 *******************************************************************************/

/**
 * @brief to_char
 *        <BR> TIMESTAMP WITH TIME ZONE를 format string에 맞게 string으로 변환
 *        <BR> TIMESTAMP WITH TIME ZONE
 *        <BR> to_char( TIMESTAMP WITH TIME ZONE, fmt )
 *        <BR>   to_char( TIMESTAMP WITH TIME ZONE, fmt ) => [ P, O ]
 *        <BR> ex) to_char(TIMESTAMP WITH TIME ZONE, 'HH:MI:SS')
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> to_char(TIMESTAMP WITH TIME ZONE, VARCHAR)
 * @param[out] aResultValue      결과 (결과타입 VARCHAR)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[in]  aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimestampTzToChar( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                void           * aInfo,
                                dtlFuncVector  * aVectorFunc,
                                void           * aVectorArg,
                                void           * aEnv )                  
{
    dtlDataValue         * sValue1;
    dtlDataValue         * sValue2;
    dtlDataValue         * sResultValue;
    stlErrorStack        * sErrorStack;

    dtlDateTimeFormatInfo  * sToCharFormatInfo;    
    dtlFractionalSecond      sFractionalSecond;
    dtlExpTime               sDtlExpTime;
    dtlTimestampTzType       sTimestampTzType;    
    stlInt32                 sTimeZone;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_VARBINARY, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_VARCHAR, sErrorStack );
    
    sToCharFormatInfo = (dtlDateTimeFormatInfo *)(sValue2->mValue);
    
    DTL_INIT_DTLEXPTIME( sDtlExpTime );

    sTimestampTzType.mTimestamp = ((dtlTimestampTzType *)(sValue1->mValue))->mTimestamp;
    sTimestampTzType.mTimeZone = ((dtlTimestampTzType *)(sValue1->mValue))->mTimeZone;

    STL_TRY( dtdTimestampTz2dtlExpTime( sTimestampTzType,
                                        & sTimeZone,
                                        & sDtlExpTime,
                                        & sFractionalSecond,
                                        sErrorStack )
             == STL_SUCCESS );

    STL_TRY( dtfDateTimeToChar( sToCharFormatInfo,
                                & sDtlExpTime,
                                sFractionalSecond, 
                                & sTimeZone,
                                DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE,
                                (stlChar *)(sResultValue->mValue),
                                & sResultValue->mLength,
                                aVectorFunc,
                                aVectorArg,
                                sErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/*******************************************************************************
 * INTERVAL YEAR TO MONTH
 *   to_char( INTERVAL YEAR TO MONTH, fmt )
 *     to_char( INTERVAL YEAR TO MONTH, fmt ) => [ P, O ]
 * ex) to_char(INTERVAL YEAR TO MONTH) 
 *******************************************************************************/

/**
 * @brief to_char
 *        <BR> INTERVAL YEAR TO MONTH를 string으로 변환
 *        <BR> INTERVAL YEAR TO MONTH
 *        <BR> to_char( INTERVAL YEAR TO MONTH, fmt )
 *        <BR>   to_char( INTERVAL YEAR TO MONTH, fmt ) => [ P, O ]
 *        <BR> ex) to_char(INTERVAL YEAR TO MONTH)
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> to_char(INTERVAL YEAR TO MONTH, VARCHAR)
 * @param[out] aResultValue      결과 (결과타입 VARCHAR)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[in]  aEnv              environment (stlErrorStack)
 */
stlStatus dtfIntervalYearToMonthToChar( stlUInt16        aInputArgumentCnt,
                                        dtlValueEntry  * aInputArgument,
                                        void           * aResultValue,
                                        void           * aInfo,
                                        dtlFuncVector  * aVectorFunc,
                                        void           * aVectorArg,
                                        void           * aEnv )
{
    dtlDataValue      * sValue1;
    dtlDataValue      * sResultValue;
    stlErrorStack     * sErrorStack;
    dtlDataTypeInfo   * sSrcTypeInfo = NULL;        
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_INTERVAL_YEAR_TO_MONTH, sErrorStack );
    DTF_CHECK_DTL_VALUE( aInputArgument[1].mValue.mDataValue, DTL_TYPE_VARBINARY, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_VARCHAR, sErrorStack );
   
    /**
     * interval year to month인 경우는,
     * fmt는 적용하지 않고 ( 무시하고 ),
     * interval year to month value를 varchar로 변환해서 결과를 낸다.
     */

    sSrcTypeInfo = (dtlDataTypeInfo *)(aInfo);

    STL_DASSERT( sSrcTypeInfo->mArgNum3 >= 0 );
    STL_DASSERT( sSrcTypeInfo->mArgNum3 < DTL_INTERVAL_INDICATOR_MAX );

    STL_TRY( dtdIntervalYearToMonthToFormatString( sValue1,
                                                   sSrcTypeInfo->mArgNum1,
                                                   DTL_INTERVAL_YEAR_TO_MONTH_MAX_DISPLAY_SIZE,
                                                   (void *)(sResultValue->mValue),
                                                   &(sResultValue->mLength),
                                                   aVectorFunc,
                                                   aVectorArg,
                                                   sErrorStack )
             == STL_SUCCESS );    
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/*******************************************************************************
 * INTERVAL DAY TO SECOND
 *   to_char( INTERVAL DAY TO SECOND, fmt )
 *     to_char( INTERVAL DAY TO SECOND, fmt ) => [ P, O ]
 * ex) to_char(INTERVAL DAY TO SECOND) 
 *******************************************************************************/

/**
 * @brief to_char
 *        <BR> INTERVAL DAY TO SECOND를 string으로 변환
 *        <BR> INTERVAL DAY TO SECOND
 *        <BR> to_char( INTERVAL DAY TO SECOND, fmt )
 *        <BR>   to_char( INTERVAL DAY TO SECOND, fmt ) => [ P, O ]
 *        <BR> ex) to_char(INTERVAL DAY TO SECOND)
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> to_char(INTERVAL DAY TO SECOND, VARCHAR)
 * @param[out] aResultValue      결과 (결과타입 VARCHAR)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[in]  aEnv              environment (stlErrorStack)
 */
stlStatus dtfIntervalDayToSecondToChar( stlUInt16        aInputArgumentCnt,
                                        dtlValueEntry  * aInputArgument,
                                        void           * aResultValue,
                                        void           * aInfo,
                                        dtlFuncVector  * aVectorFunc,
                                        void           * aVectorArg,
                                        void           * aEnv )
{
    dtlDataValue      * sValue1;
    dtlDataValue      * sResultValue;
    stlErrorStack     * sErrorStack;
    dtlDataTypeInfo   * sSrcTypeInfo = NULL;        

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_INTERVAL_DAY_TO_SECOND, sErrorStack );
    DTF_CHECK_DTL_VALUE( aInputArgument[1].mValue.mDataValue, DTL_TYPE_VARBINARY, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_VARCHAR, sErrorStack );

    /**
     * interval year to month인 경우는,
     * fmt는 적용하지 않고 ( 무시하고 ),
     * interval year to month value를 varchar로 변환해서 결과를 낸다.
     */

    sSrcTypeInfo = (dtlDataTypeInfo *)(aInfo);

    STL_DASSERT( sSrcTypeInfo->mArgNum3 >= 0 );
    STL_DASSERT( sSrcTypeInfo->mArgNum3 < DTL_INTERVAL_INDICATOR_MAX );

    STL_TRY( dtdIntervalDayToSecondToFormatString( sValue1,
                                                   sSrcTypeInfo->mArgNum1,
                                                   sSrcTypeInfo->mArgNum2,
                                                   DTL_INTERVAL_DAY_TO_SECOND_MAX_DISPLAY_SIZE,
                                                   (void *)(sResultValue->mValue),
                                                   &(sResultValue->mLength),
                                                   aVectorFunc,
                                                   aVectorArg,
                                                   sErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/*******************************************************************************
 * NUMERIC
 *   to_char( NUMERIC, fmt )
 *     to_char( NUMERIC, fmt ) => [ P, O ]
 * ex) to_char( 123.45, 'S999.999' ) => +123.450
 *******************************************************************************/

/**
 * @brief to_char
 *        <BR> NUMBER  string으로 변환
 *        <BR> NUMBER
 *        <BR> to_char( NUMBER, fmt )
 *        <BR>   to_char( NUMBER, fmt ) => [ P, O ]
 *        <BR> ex) to_char( 123.45, 'S999.999' ) => +123.450
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> to_char(NUMBER, VARCHAR) 또는 to_char(FLOAT, VARCHAR)
 * @param[out] aResultValue      결과 (결과타입 VARCHAR)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[in]  aEnv              environment (stlErrorStack)
 */
stlStatus dtfNumberToChar( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aInfo,
                           dtlFuncVector  * aVectorFunc,
                           void           * aVectorArg,
                           void           * aEnv )
{
    dtlDataValue          * sValue1;
    dtlDataValue          * sValue2;
    dtlDataValue          * sResultValue;
    stlErrorStack         * sErrorStack;    

    dtfNumberFormatInfo  * sToCharFormatInfo = NULL;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;
    
    DTF_CHECK_DTL_VALUE2( sValue1, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_VARBINARY, sErrorStack );
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_VARCHAR, sErrorStack );
    
    sToCharFormatInfo = (dtfNumberFormatInfo *)(sValue2->mValue);    
    
    if( sToCharFormatInfo->mStrLen == 0 )
    {
        /*****************************************************
         * to_char( number )
         *****************************************************/   
        
        STL_TRY( dtdCastNumericToString( sValue1,
                                         (DTL_NUMERIC_STRING_MAX_SIZE - 1),
                                         sResultValue->mValue,
                                         &(sResultValue->mLength),
                                         sErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        /*****************************************************
         * to_char( number, fmt )
         * format에 따라 숫자를 문자로 변환.
         *****************************************************/   
        
        STL_DASSERT( sToCharFormatInfo != NULL );
        STL_DASSERT( sResultValue->mValue != NULL );
        
        /*****************************************************
         * format에 따라 숫자를 문자로 변환.
         *****************************************************/

        STL_TRY_THROW( sResultValue->mBufferSize >= sToCharFormatInfo->mToCharResultLen, ERROR_NOT_ENOUGH_BUFFER );
        
        STL_TRY( sToCharFormatInfo->mFormatFunc( sValue1,
                                                 sToCharFormatInfo,
                                                 sResultValue->mBufferSize,
                                                 (stlChar *)(sResultValue->mValue),
                                                 & sResultValue->mLength,
                                                 aVectorFunc,
                                                 aVectorArg,
                                                 sErrorStack )
                 == STL_SUCCESS );
    }
    
    STL_DASSERT( sResultValue->mLength <= sResultValue->mBufferSize );
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_NOT_ENOUGH_BUFFER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_NUMBER_BYTE_LENGTH_GREATER_BUFFER_LENGTH,
                      NULL,
                      sErrorStack );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}

/*
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> to_char(NATIVE_DOUBLE, VARCHAR)
 * @param[out] aResultValue      결과 (결과타입 VARCHAR)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[in]  aEnv              environment (stlErrorStack)
 */
stlStatus dtfDoubleToChar( stlUInt16        aInputArgumentCnt,
                            dtlValueEntry  * aInputArgument,
                            void           * aResultValue,
                            void           * aInfo,
                            dtlFuncVector  * aVectorFunc,
                            void           * aVectorArg,
                            void           * aEnv )
{
    dtlDataValue          * sValue1;
    dtlDataValue          * sValue2;
    dtlDataValue          * sResultValue;
    stlErrorStack         * sErrorStack;    

    dtfNumberFormatInfo  * sToCharFormatInfo = NULL;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;
    
    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_DOUBLE, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_VARBINARY, sErrorStack );
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_VARCHAR, sErrorStack );
    
    sToCharFormatInfo = (dtfNumberFormatInfo *)(sValue2->mValue);    
    
    if( sToCharFormatInfo->mStrLen == 0 )
    {
        STL_TRY( dtdDoubleToString( sValue1,
                                    DTL_PRECISION_NA,
                                    DTL_SCALE_NA,
                                    DTL_NATIVE_DOUBLE_STRING_SIZE -1,
                                    sResultValue->mValue,
                                    &(sResultValue->mLength),
                                    aVectorFunc,
                                    aVectorArg,
                                    sErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        /*****************************************************
         * to_char( native_double, fmt )
         * format에 따라 숫자를 문자로 변환.
         *****************************************************/   
        
        STL_DASSERT( sToCharFormatInfo != NULL );
        STL_DASSERT( sResultValue->mValue != NULL );
        
        /*****************************************************
         * format에 따라 숫자를 문자로 변환.
         *****************************************************/   
        
        STL_TRY_THROW( sResultValue->mBufferSize >= sToCharFormatInfo->mToCharResultLen, ERROR_NOT_ENOUGH_BUFFER );
        
        STL_TRY( sToCharFormatInfo->mFormatFunc( sValue1,
                                                 sToCharFormatInfo,
                                                 sResultValue->mBufferSize,
                                                 (stlChar *)(sResultValue->mValue),
                                                 & sResultValue->mLength,
                                                 aVectorFunc,
                                                 aVectorArg,
                                                 sErrorStack )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_NOT_ENOUGH_BUFFER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_NUMBER_BYTE_LENGTH_GREATER_BUFFER_LENGTH,
                      NULL,
                      sErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}



/*
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> to_char(NATIVE_REAL, VARCHAR)
 * @param[out] aResultValue      결과 (결과타입 VARCHAR)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[in]  aEnv              environment (stlErrorStack)
 */
stlStatus dtfRealToChar( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv )
{
    dtlDataValue          * sValue1;
    dtlDataValue          * sValue2;
    dtlDataValue          * sResultValue;
    stlErrorStack         * sErrorStack;    

    dtfNumberFormatInfo  * sToCharFormatInfo = NULL;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;
    
    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_REAL, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_VARBINARY, sErrorStack );
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_VARCHAR, sErrorStack );
    
    sToCharFormatInfo = (dtfNumberFormatInfo *)(sValue2->mValue);    
    
    if( sToCharFormatInfo->mStrLen == 0 )
    {
        STL_TRY( dtdRealToString( sValue1,
                                  DTL_PRECISION_NA,
                                  DTL_SCALE_NA,
                                  DTL_NATIVE_REAL_STRING_SIZE -1,
                                  sResultValue->mValue,
                                  &(sResultValue->mLength),
                                  aVectorFunc,
                                  aVectorArg,
                                  sErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        /*****************************************************
         * to_char( native_real, fmt )
         * format에 따라 숫자를 문자로 변환.
         *****************************************************/   
        
        STL_DASSERT( sToCharFormatInfo != NULL );
        STL_DASSERT( sResultValue->mValue != NULL );
        
        /*****************************************************
         * format에 따라 숫자를 문자로 변환.
         *****************************************************/   
        
        STL_TRY_THROW( sResultValue->mBufferSize >= sToCharFormatInfo->mToCharResultLen, ERROR_NOT_ENOUGH_BUFFER );
        
        STL_TRY( sToCharFormatInfo->mFormatFunc( sValue1,
                                                 sToCharFormatInfo,
                                                 sResultValue->mBufferSize,
                                                 (stlChar *)(sResultValue->mValue),
                                                 & sResultValue->mLength,
                                                 aVectorFunc,
                                                 aVectorArg,
                                                 sErrorStack )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_NOT_ENOUGH_BUFFER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_NUMBER_BYTE_LENGTH_GREATER_BUFFER_LENGTH,
                      NULL,
                      sErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/*******************************************************************************
 * CHAR, VARCHAR
 *   to_char( CHAR | VARCHAR )
 *     to_char( CHAR ) => [ O ]
 * ex) to_char( 'abc' ) => 'abc'
 *******************************************************************************/

/**
 * @brief to_char
 *        <BR> character string계열을 string으로 변환
 *        <BR> CHAR, VARCHAR
 *        <BR> to_char( CHAR | VARCHAR )
 *        <BR> ex) to_char( 'abc' ) => 'abc'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> to_char( CHAR | VARCHAR  )
 * @param[out] aResultValue      결과 (결과타입 CHAR | VARCHAR)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[in]  aEnv              environment (stlErrorStack)
 */
stlStatus dtfCharStringToChar( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aInfo,
                               dtlFuncVector  * aVectorFunc,
                               void           * aVectorArg,
                               void           * aEnv )
{
    dtlDataValue          * sValue1;
    dtlDataValue          * sValue2;
    dtlDataValue          * sResultValue;
    stlErrorStack         * sErrorStack;
    
    dtfNumberFormatInfo   * sToCharFormatInfo = NULL;

    DTL_PARAM_VALIDATE( ( aInputArgumentCnt == 1 ) || ( aInputArgumentCnt == 2 ),
                        (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;
    
    DTF_CHECK_DTL_VALUE2( sValue1, DTL_TYPE_CHAR, DTL_TYPE_VARCHAR, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_VARBINARY, sErrorStack );
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue,
                           DTL_TYPE_CHAR,
                           DTL_TYPE_VARCHAR,
                           sErrorStack );
    
    sToCharFormatInfo = (dtfNumberFormatInfo *)(sValue2->mValue);
    
    if( sToCharFormatInfo->mStrLen == 0 )
    {
        /*****************************************************
         * to_char( CHAR | VARCHAR )
         *****************************************************/
        
        STL_DASSERT( sResultValue->mBufferSize >= sValue1->mLength );
        
        stlMemcpy( sResultValue->mValue,
                   sValue1->mValue,
                   sValue1->mLength );
        
        sResultValue->mLength = sValue1->mLength;
    }
    else
    {
        /*
         * to_char( CHAR | VARCHAR )의 경우, 인자가 하나만 가능하다.
         */ 
        STL_THROW( ERR_FORMAT_STR );
        
        /* Do Nothing */
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( ERR_FORMAT_STR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_FORMAT_NOT_MATCH_FORMAT_STRING,
                      NULL,
                      sErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/*******************************************************************************
 * LONG VARCHAR
 *   to_char( LONG VARCHAR )
 *     to_char( LONG VARCHAR )
 * ex) to_char( 'abc' ) => 'abc'
 *******************************************************************************/

/**
 * @brief to_char
 *        <BR> character string계열을 string으로 변환
 *        <BR> LONG VARCHAR
 *        <BR> to_char( LONG VARCHAR )
 *        <BR> ex) to_char( 'abc' ) => 'abc'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> to_char( LONG VARCHAR  )
 * @param[out] aResultValue      결과 (결과타입 LONG VARCHAR)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[in]  aEnv              environment (stlErrorStack)
 */
stlStatus dtfLongvarcharStringToChar( stlUInt16        aInputArgumentCnt,
                                      dtlValueEntry  * aInputArgument,
                                      void           * aResultValue,
                                      void           * aInfo,
                                      dtlFuncVector  * aVectorFunc,
                                      void           * aVectorArg,
                                      void           * aEnv )
{
    dtlDataValue          * sValue1;
    dtlDataValue          * sValue2;    
    dtlDataValue          * sResultValue;
    stlErrorStack         * sErrorStack;

    dtfNumberFormatInfo   * sToCharFormatInfo = NULL;

    DTL_PARAM_VALIDATE( ( aInputArgumentCnt == 1 ) || ( aInputArgumentCnt == 2 ),
                        (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;
    
    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_LONGVARCHAR, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_VARBINARY, sErrorStack );
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_LONGVARCHAR, sErrorStack );

    sToCharFormatInfo = (dtfNumberFormatInfo *)(sValue2->mValue);
    
    if( sToCharFormatInfo->mStrLen == 0 )
    {
        /*****************************************************
         * to_char( LONG VARCHAR )
         *****************************************************/

        if( sValue1->mLength > sResultValue->mBufferSize )
        {
            STL_TRY( aVectorFunc->mReallocLongVaryingMemFunc( aVectorArg,
                                                              sValue1->mLength,
                                                              &sResultValue->mValue,
                                                              (stlErrorStack *)aEnv )
                     == STL_SUCCESS );
            
            sResultValue->mBufferSize = sValue1->mLength;
        }

        stlMemcpy( sResultValue->mValue,
                   sValue1->mValue,
                   sValue1->mLength );

        sResultValue->mLength = sValue1->mLength;
    }
    else
    {
        /*
         * to_char( LONG VARCHAR )의 경우, 인자가 하나만 가능하다.
         */
        STL_THROW( ERR_FORMAT_STR );
        
        /* Do Nothing */
    }
    
    return STL_SUCCESS;

    STL_CATCH( ERR_FORMAT_STR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_FORMAT_NOT_MATCH_FORMAT_STRING,
                      NULL,
                      sErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/** @} */
