/*******************************************************************************
 * dtfLastDay.c
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
 * @brief Last_day Function DataType Layer
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
 *   last_day( DATE )
 *     last_day( DATE ) => [ O ]
 * ex) last_day( DATE )
 *******************************************************************************/

/**
 * @brief last_day
 *        <BR> DATE를 포함한 달의 마지막 날짜를 구한다.
 *        <BR> DATE
 *        <BR> last_day( DATE )
 *        <BR>   last_day( DATE ) => [ O ]
 *        <BR> ex) last_day( DATE )
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> last_day(DATE)
 * @param[out] aResultValue      결과 (결과타입 DATE)
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[in]  aEnv              environment (stlErrorStack)
 */
stlStatus dtfDateLastDay( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv )                  
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;

    dtlDateType     sDate = 0;
    dtlExpTime      sDtlExpTime;
    dtlFractionalSecond sFractionalSecond = 0;    

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_DATE, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_DATE, sErrorStack );

    sDate = *(dtlDateType *)(sValue1->mValue);

    DTL_INIT_DTLEXPTIME( sDtlExpTime );

    STL_TRY( dtdDateTime2dtlExpTime( sDate,
                                     & sDtlExpTime,
                                     & sFractionalSecond,
                                     sErrorStack )
             == STL_SUCCESS );

    /*
     * DATE를 포함한 달의 마지막 날짜를 구한다.
     */
    
    sDtlExpTime.mDay = dtlDayTab[DTL_IS_LEAP(sDtlExpTime.mYear)][sDtlExpTime.mMonth - 1];

    STL_TRY( dtdDtlExpTime2DateTimeOffset( & sDtlExpTime,
                                           0, /* fractional second */
                                           & sDate,
                                           sErrorStack )
             == STL_SUCCESS );
    
    *(dtlDateType *)(sResultValue->mValue) = sDate;
    sResultValue->mLength = DTL_DATE_SIZE;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;   
}


/**
 * @brief last_day
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> last_day(TIMESTAMP)
 * @param[out] aResultValue      결과 (결과타입 DATE)
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[in]  aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimestampLastDay( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               dtlFuncVector  * aVectorFunc,
                               void           * aVectorArg,
                               void           * aEnv )                  
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;

    dtlDateType         sDate = 0;
    dtlFractionalSecond sFractionalSecond;        
    dtlExpTime          sDtlExpTime;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_TIMESTAMP, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_DATE, sErrorStack );

    DTL_INIT_DTLEXPTIME( sDtlExpTime );    

    STL_TRY( dtdDateTime2dtlExpTime( *((dtlTimestampType *)(sValue1->mValue)),
                                     & sDtlExpTime,
                                     & sFractionalSecond,
                                     sErrorStack )
             == STL_SUCCESS );

    /*
     * DATE를 포함한 달의 마지막 날짜를 구한다.
     */
    
    sDtlExpTime.mDay = dtlDayTab[DTL_IS_LEAP(sDtlExpTime.mYear)][sDtlExpTime.mMonth - 1];

    STL_TRY( dtdDtlExpTime2DateTimeOffset( & sDtlExpTime,
                                           0, /* fractional second */
                                           & sDate,
                                           sErrorStack )
             == STL_SUCCESS );
    
    *(dtlDateType *)(sResultValue->mValue) = sDate;
    sResultValue->mLength = DTL_DATE_SIZE;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;   
}


/**
 * @brief last_day
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> last_day(TIMESTAMP WITH TIME ZONE)
 * @param[out] aResultValue      결과 (결과타입 DATE)
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[in]  aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimestampWithTimeZoneLastDay( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv )                  
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;

    dtlDateType         sDate = 0;
    dtlFractionalSecond sFractionalSecond;
    stlInt32            sTimeZone;    
    dtlExpTime          sDtlExpTime;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, sErrorStack );

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
     * DATE를 포함한 달의 마지막 날짜를 구한다.
     */
    
    sDtlExpTime.mDay = dtlDayTab[DTL_IS_LEAP(sDtlExpTime.mYear)][sDtlExpTime.mMonth - 1];

    STL_TRY( dtdDtlExpTime2DateTimeOffset( & sDtlExpTime,
                                           0, /* fractional second */
                                           & sDate,
                                           sErrorStack )
             == STL_SUCCESS );
    
    *(dtlDateType *)(sResultValue->mValue) = sDate;
    sResultValue->mLength = DTL_DATE_SIZE;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;   
}


/** @} */
