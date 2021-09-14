/*******************************************************************************
 * dtfAddMonths.c
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
 * @file 
 * @brief Add_months Function DataType Layer
 */

#include <dtl.h>
#include <dtlDef.h>
#include <dtDef.h>


/**
 * @ingroup dtf
 * @internal
 * @{
 */


/*******************************************************************************
 * DATE
 *   ADD_MONTHS( DATE, integer )
 *     add_months( DATE, integer ) => [ O ]
 * ex) add_months( DATE, integer )
 *******************************************************************************/

/**
 * @brief add_months
 *        <BR> DATE에 주어진 integer month를 더한 날짜를 반환한다.
 *        <BR> DATE
 *        <BR> add_months( DATE, integer )
 *        <BR>   add_months( DATE, integer ) => [ O ]
 *        <BR> ex) add_months( DATE, integer )
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> last_day(DATE, NUMERIC)
 * @param[out] aResultValue      결과 (결과타입 DATE)
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[in]  aEnv              environment (stlErrorStack)
 */
stlStatus dtfDateAddMonths( stlUInt16        aInputArgumentCnt,
                            dtlValueEntry  * aInputArgument,
                            void           * aResultValue,
                            dtlFuncVector  * aVectorFunc,
                            void           * aVectorArg,
                            void           * aEnv )                  
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;    
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;

    dtlDateType     sDate   = 0;    
    stlInt64        sMonths = 0;
    stlBool         sIsTruncated = STL_FALSE;
    dtlExpTime      sDtlExpTime;
    dtlFractionalSecond sFractionalSecond = 0;    

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;    
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_DATE, sErrorStack );
    DTF_CHECK_DTL_VALUE2( sValue2, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );    

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_DATE, sErrorStack );
    
    sDate = DTF_DATE(sValue1);

    DTL_INIT_DTLEXPTIME( sDtlExpTime );

    STL_TRY( dtdDateTime2dtlExpTime( sDate,
                                     & sDtlExpTime,
                                     & sFractionalSecond,
                                     sErrorStack )
             == STL_SUCCESS );    
    
    /*
     * integer 월 구하기 ( 소수점은 버림 )
     * 예) add_months( to_date( '2013-01-01' ), 1.7 ) => 2013-02-01
     */

    STL_TRY( dtlNumericToInt64( sValue2,
                                & sMonths,
                                & sIsTruncated,
                                sErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( (sMonths >= -DTL_JULIAN_MAX_TOTAL_MONTH) &&
                   (sMonths <= DTL_JULIAN_MAX_TOTAL_MONTH),
                   ERROR_OUT_OF_RANGE );
    
    /*
     * Add months
     */
    
    DTF_DATE_ADD_MONTHS( sDtlExpTime, sMonths );
    
    STL_TRY( dtdDtlExpTime2DateTimeOffset( & sDtlExpTime,
                                           0, /* fractional second */
                                           & sDate,
                                           sErrorStack )
             == STL_SUCCESS );
    
    DTF_DATE(sResultValue) = sDate;
    sResultValue->mLength = DTL_DATE_SIZE;
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATETIME_FIELD_OVERFLOW,
                      NULL,
                      sErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;   
}


/**
 * @brief add_months
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> last_day(TIMESTAMP, NUMERIC)
 * @param[out] aResultValue      결과 (결과타입 DATE)
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[in]  aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimestampAddMonths( stlUInt16        aInputArgumentCnt,
                                 dtlValueEntry  * aInputArgument,
                                 void           * aResultValue,
                                 dtlFuncVector  * aVectorFunc,
                                 void           * aVectorArg,
                                 void           * aEnv )                  
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;    
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;

    dtlDateType         sDate   = 0;    
    stlInt64            sMonths = 0;
    stlBool             sIsTruncated = STL_FALSE;
    dtlFractionalSecond sFractionalSecond;            
    dtlExpTime          sDtlExpTime;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;    
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_TIMESTAMP, sErrorStack );
    DTF_CHECK_DTL_VALUE2( sValue2, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );    

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_DATE, sErrorStack );
    
    DTL_INIT_DTLEXPTIME( sDtlExpTime );    
    
    STL_TRY( dtdDateTime2dtlExpTime( *((dtlTimestampType *)(sValue1->mValue)),
                                     & sDtlExpTime,
                                     & sFractionalSecond,
                                     sErrorStack )
             == STL_SUCCESS );

    /*
     * integer 월 구하기 ( 소수점은 버림 )
     * 예) add_months( to_date( '2013-01-01' ), 1.7 ) => 2013-02-01
     */

    STL_TRY( dtlNumericToInt64( sValue2,
                                & sMonths,
                                & sIsTruncated,
                                sErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( (sMonths >= -DTL_JULIAN_MAX_TOTAL_MONTH) &&
                   (sMonths <= DTL_JULIAN_MAX_TOTAL_MONTH),
                   ERROR_OUT_OF_RANGE );
    
    /*
     * Add months
     */
    
    DTF_DATE_ADD_MONTHS( sDtlExpTime, sMonths );
    
    STL_TRY( dtdDtlExpTime2DateTimeOffset( & sDtlExpTime,
                                           0, /* fractional second */
                                           & sDate,
                                           sErrorStack )
             == STL_SUCCESS );
    
    DTF_DATE(sResultValue) = sDate;
    sResultValue->mLength = DTL_DATE_SIZE;
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATETIME_FIELD_OVERFLOW,
                      NULL,
                      sErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;   
}


/**
 * @brief add_months
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> last_day(TIMESTAMP WITH TIME ZONE, NUMERIC)
 * @param[out] aResultValue      결과 (결과타입 DATE)
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[in]  aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimestampWithTimeZoneAddMonths( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv )                  
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;    
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;

    dtlDateType         sDate   = 0;    
    stlInt64            sMonths = 0;
    stlBool             sIsTruncated = STL_FALSE;
    dtlFractionalSecond sFractionalSecond;
    stlInt32            sTimeZone;        
    dtlExpTime          sDtlExpTime;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;    
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, sErrorStack );
    DTF_CHECK_DTL_VALUE2( sValue2, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_DATE, sErrorStack );
    
    DTL_INIT_DTLEXPTIME( sDtlExpTime );

    STL_TRY( dtdTimestampTz2dtlExpTime( *((dtlTimestampTzType *)(sValue1->mValue)),
                                        & sTimeZone,
                                        & sDtlExpTime,
                                        & sFractionalSecond,
                                        sErrorStack )
             == STL_SUCCESS );

    /*
     * integer 월 구하기 ( 소수점은 버림 )
     * 예) add_months( to_date( '2013-01-01' ), 1.7 ) => 2013-02-01
     */

    STL_TRY( dtlNumericToInt64( sValue2,
                                & sMonths,
                                & sIsTruncated,
                                sErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( (sMonths >= -DTL_JULIAN_MAX_TOTAL_MONTH) &&
                   (sMonths <= DTL_JULIAN_MAX_TOTAL_MONTH),
                   ERROR_OUT_OF_RANGE );
    
    /*
     * Add months
     */
    
    DTF_DATE_ADD_MONTHS( sDtlExpTime, sMonths );
    
    STL_TRY( dtdDtlExpTime2DateTimeOffset( & sDtlExpTime,
                                           0, /* fractional second */
                                           & sDate,
                                           sErrorStack )
             == STL_SUCCESS );
    
    DTF_DATE(sResultValue) = sDate;
    sResultValue->mLength = DTL_DATE_SIZE;
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATETIME_FIELD_OVERFLOW,
                      NULL,
                      sErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;   
}


/** @} */
