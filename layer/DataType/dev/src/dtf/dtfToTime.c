/*******************************************************************************
 * dtfToTime.c
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
 * @file dtfToTime.c
 * @brief Formatting Function DataType Library Layer
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtfFormatting.h>

/**
 * @addtogroup dtfToTime  Formatting
 * @ingroup dtf
 * @internal
 * @{
 */

/*******************************************************************************
 * TIME
 *   to_time( char, fmt )
 *     to_time( char, fmt )
 * ex) to_time( '11:22:33', 'HH:MI:SS' )
 *******************************************************************************/

/**
 * @brief to_time
 *        <BR> character를 format string에 맞게 TIME으로 변환
 *        <BR> TIME
 *        <BR> to_time( char, fmt )
 *        <BR>   to_time( char, fmt )
 *        <BR> ex) to_time( '11:22:33', 'HH:MI:SS' )
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> to_time(VARCHAR, VARBINARY)
 *                               <BR> to_time(LONGVARCHAR, VARBINARY)
 * @param[out] aResultValue      결과 (결과타입 TIME)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[in]  aEnv              environment (stlErrorStack)
 */
stlStatus dtfToTime( stlUInt16        aInputArgumentCnt,
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
    dtlTimeType             * sTimeType;

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
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIME, sErrorStack );

    sToDateTimeFormatInfo = (dtlDateTimeFormatInfo *)(sValue2->mValue);

    DTL_INIT_DTLEXPTIME( sDtlExpTime );
    
    STL_TRY( dtfToDateTimeFromString( DTL_TYPE_TIME,
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
     * Time Type Value 만들기
     * hh mi ss ff1...ff6
     */

    sTimeType = (dtlTimeType *)(sResultValue->mValue);
    
    dtdDtlExpTime2Time( & sDtlExpTime,
                        sFractionalSecond,
                        sTimeType );
    
    sResultValue->mLength = DTL_TIME_SIZE;
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}



/** @} */






