/*******************************************************************************
 * dtfToTimeWithTimeZone.c
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
 * @file dtfToTimeWithTimeZone.c
 * @brief Formatting Function DataType Library Layer
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtfFormatting.h>

/**
 * @addtogroup dtfToTimeWithTimeZone  Formatting
 * @ingroup dtf
 * @internal
 * @{
 */

/*******************************************************************************
 * TIME WITH TIME ZONE
 *   to_time_with_time_zone( char, fmt ) or to_time_tz( char, fmt )
 * ex) to_time_with_time_zone( '11:22:33 +09:00', 'HH:MI:SS TZH:TZM' )
 *     to_time_tz( '11:22:33 +09:00', 'HH:MI:SS TZH:TZM' )
 *******************************************************************************/

/**
 * @brief to_time_with_time_zone  or to_time_tz
 *        <BR> character를 format string에 맞게 TIME_WITH_TIME_ZONE 으로 변환
 *        <BR> TIME_WITH_TIME_ZONE
 *        <BR> to_time_with_time_zone( char, fmt )   or  to_time_tz( char, fmt )
 *        <BR> ex) to_time_with_time_zone( '11:22:33 +09:00', 'HH:MI:SS TZH:TZM' )
 *        <BR>     to_time_tz( '11:22:33 +09:00', 'HH:MI:SS TZH:TZM' )
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> to_time_with_time_zone(VARCHAR, VARBINARY)
 *                               <BR> to_time_with_time_zone(LONGVARCHAR, VARBINARY)
 * @param[out] aResultValue      결과 (결과타입 TIME_WITH_TIME_ZONE)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[in]  aEnv              environment (stlErrorStack)
 */
stlStatus dtfToTimeWithTimeZone( stlUInt16        aInputArgumentCnt,
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

    dtlExpTime                sDtlExpTime;
    dtlFractionalSecond       sFractionalSecond = 0;
    stlInt32                  sTimeZone         = 0;
    dtlTimeTzType           * sTimeTzType;

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
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIME_WITH_TIME_ZONE, sErrorStack );

    sToDateTimeFormatInfo = (dtlDateTimeFormatInfo *)(sValue2->mValue);

    DTL_INIT_DTLEXPTIME( sDtlExpTime );
    
    STL_TRY( dtfToDateTimeFromString( DTL_TYPE_TIME_WITH_TIME_ZONE,
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
     * Time with time zone Type Value 만들기
     * hh mi ss tzh tzm
     */

    sTimeTzType = (dtlTimeTzType *)(sResultValue->mValue);
    
    dtdDtlExpTime2TimeTz( & sDtlExpTime,
                          sFractionalSecond,
                          sTimeZone,
                          sTimeTzType );
    
    sResultValue->mLength = DTL_TIMETZ_SIZE;
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}



/** @} */






