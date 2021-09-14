/*******************************************************************************
 * dtfToTimestampWithTimeZone.c
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
 * @file dtfToTimestampWithTimeZone.c
 * @brief Formatting Function DataType Library Layer
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtfFormatting.h>

/**
 * @addtogroup dtfToTimestampWithTimeZone  Formatting
 * @ingroup dtf
 * @internal
 * @{
 */


/*******************************************************************************
 * TIMESTAMP_WITH_TIME_ZONE
 *   to_timestamp_With_Time_Zone( char, fmt )
 *     to_Timestamp_With_Time_Zone( char, fmt ) => [ P, O ]
 * ex) to_Timestamp_With_Time_Zone( '1999-01-01 11:22:33 +09:00', 'YYYY-MM-DD HH:MI:SS TZH:TZM' )
 *******************************************************************************/

/**
 * @brief to_timestamp_with_time_zone
 *        <BR> character를 format string에 맞게 TIMESTAMP_WITH_TIME_ZONE 으로 변환
 *        <BR> TIMESTAMP_WITH_TIME_ZONE
 *        <BR> to_timestamp_with_time_zone( char, fmt )
 *        <BR>   to_timestamp_with_time_zone( char, fmt ) => [ P, O ]
 *        <BR> ex) to_timestamp_with_time_zone('1999-01-01 11:22:33 +09:00', 'YYYY-MM-DD HH:MI:SS TZH:TZM')
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> to_date(VARCHAR, VARBINARY)
 *                               <BR> to_date(LONGVARCHAR, VARBINARY)
 * @param[out] aResultValue      결과 (결과타입 TIMESTAMP_WITH_TIME_ZONE)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[in]  aEnv              environment (stlErrorStack)
 */
stlStatus dtfToTimestampWithTimeZone( stlUInt16        aInputArgumentCnt,
                                      dtlValueEntry  * aInputArgument,
                                      void           * aResultValue,
                                      void           * aInfo,
                                      dtlFuncVector  * aVectorFunc,
                                      void           * aVectorArg,
                                      void           * aEnv )                  
{
    dtlDataValue            * sValue1;
    dtlDataValue            * sValue2;
    dtlDateTimeFormatInfo   * sToDateTimeFormatInfo;
    dtlDataValue            * sResultValue;
    stlErrorStack           * sErrorStack;

    dtlTimestampTzType      * sTimestampTzType  = NULL;    
    dtlExpTime                sDtlExpTime;
    dtlFractionalSecond       sFractionalSecond = 0;
    stlInt32                  sTimeZone         = 0;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_GROUP2( sValue1,
                          DTL_GROUP_CHAR_STRING,
                          DTL_GROUP_LONGVARCHAR_STRING,
                          sErrorStack );    
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_VARBINARY, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, sErrorStack );

    sToDateTimeFormatInfo = (dtlDateTimeFormatInfo *)(sValue2->mValue);

    DTL_INIT_DTLEXPTIME( sDtlExpTime );
    
    STL_TRY( dtfToDateTimeFromString( DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,
                                      (stlChar *)(sValue1->mValue),
                                      sValue1->mLength,
                                      sToDateTimeFormatInfo,
                                      & sDtlExpTime,
                                      & sFractionalSecond,
                                      & sTimeZone,
                                      aVectorFunc,
                                      aVectorArg,
                                      sErrorStack )
             == STL_SUCCESS );
    
    /**
     * Timestamp with time zone Type Value 만들기
     * yyyy mm dd hh mi ss ff1...ff6
     */
    
    sTimestampTzType = (dtlTimestampTzType *)(sResultValue->mValue);
    
    STL_TRY( dtdDtlExpTime2TimestampTz( NULL,
                                        & sDtlExpTime,
                                        sFractionalSecond,
                                        & sTimeZone,
                                        sTimestampTzType,
                                        sErrorStack )
             == STL_SUCCESS );
    
    sResultValue->mLength = DTL_TIMESTAMPTZ_SIZE;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}



/** @} */





