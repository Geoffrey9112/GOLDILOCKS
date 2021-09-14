/*******************************************************************************
 * dtfToDate.c
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
 * @file dtfToDate.c
 * @brief Formatting Function DataType Library Layer
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtfFormatting.h>

/**
 * @addtogroup dtfToDate  Formatting
 * @ingroup dtf
 * @internal
 * @{
 */


/*******************************************************************************
 * DATE
 *   to_date( char, fmt )
 *     to_date( char, fmt ) => [ P, O ]
 * ex) to_date( '1999-01-01', 'YYYY-MM-DD' )
 *******************************************************************************/

/**
 * @brief to_date
 *        <BR> character를 format string에 맞게 DATE으로 변환
 *        <BR> DATE
 *        <BR> to_date( char, fmt )
 *        <BR>   to_date( char, fmt ) => [ P, O ]
 *        <BR> ex) to_date('1999-01-01', 'YYYY-MM-DD')
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> to_date(VARCHAR, VARBINARY)
 *                               <BR> to_date(LONGVARCHAR, VARBINARY)
 * @param[out] aResultValue      결과 (결과타입 DATE)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[in]  aEnv              environment (stlErrorStack)
 */
stlStatus dtfToDate( stlUInt16        aInputArgumentCnt,
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

    dtlDateType             * sDateType         = NULL;    
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
    DTF_CHECK_DTL_RESULT2( sResultValue,
                           DTL_TYPE_DATE,
                           DTL_TYPE_TIMESTAMP,
                           sErrorStack );    
    
    sToDateTimeFormatInfo = (dtlDateTimeFormatInfo *)(sValue2->mValue);
    
    DTL_INIT_DTLEXPTIME( sDtlExpTime );
    
    STL_TRY( dtfToDateTimeFromString( DTL_TYPE_DATE,
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
     * Date Type Value 만들기
     * yyyy mm dd 
     */
    
    sDateType = (dtlDateType *)(sResultValue->mValue);
    
    STL_TRY( dtdDtlExpTime2DateTimeOffset( & sDtlExpTime,
                                           0, /* fractional second */
                                           sDateType,
                                           sErrorStack )
             == STL_SUCCESS );
    
    sResultValue->mLength = DTL_DATE_SIZE;    
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}



/** @} */





