/*******************************************************************************
 * dtfToTimestamp.c
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
 * @file dtfToTimestamp.c
 * @brief Formatting Function DataType Library Layer
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtfFormatting.h>

/**
 * @addtogroup dtfToTimestamp  Formatting
 * @ingroup dtf
 * @internal
 * @{
 */


/*******************************************************************************
 * TIMESTAMP
 *   to_timestamp( char, fmt )
 *     to_Timestamp( char, fmt ) => [ P, O ]
 * ex) to_Timestamp( '1999-01-01 11:22:33', 'YYYY-MM-DD HH:MI:SS' )
 *******************************************************************************/

/**
 * @brief to_timestamp
 *        <BR> character를 format string에 맞게 TIMESTAMP 으로 변환
 *        <BR> TIMESTAMP
 *        <BR> to_timestamp( char, fmt )
 *        <BR>   to_timestamp( char, fmt ) => [ P, O ]
 *        <BR> ex) to_timestamp('1999-01-01 11:22:33', 'YYYY-MM-DD HH:MI:SS')
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> to_timestamp(VARCHAR, VARBINARY)
 *                               <BR> to_timestamp(LONGVARCHAR, VARBINARY)
 * @param[out] aResultValue      결과 (결과타입 TIMESTAMP)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[in]  aEnv              environment (stlErrorStack)
 */
stlStatus dtfToTimestamp( stlUInt16        aInputArgumentCnt,
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

    dtlTimestampType        * sTimestampType    = NULL;    
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
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIMESTAMP, sErrorStack );

    sToDateTimeFormatInfo = (dtlDateTimeFormatInfo *)(sValue2->mValue);

    DTL_INIT_DTLEXPTIME( sDtlExpTime );
    
    STL_TRY( dtfToDateTimeFromString( DTL_TYPE_TIMESTAMP,
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
     * Timestamp Type Value 만들기
     * yyyy mm dd hh mi ss ff1...ff6
     */
    
    sTimestampType = (dtlTimestampType *)(sResultValue->mValue);
    
    STL_TRY( dtdDtlExpTime2DateTimeOffset( & sDtlExpTime,
                                           sFractionalSecond,
                                           sTimestampType,
                                           sErrorStack )
             == STL_SUCCESS );
    
    sResultValue->mLength = DTL_TIMESTAMP_SIZE;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}



/** @} */





