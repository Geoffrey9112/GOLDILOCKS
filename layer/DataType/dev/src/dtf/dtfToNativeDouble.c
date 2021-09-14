/*******************************************************************************
 * dtfToNativeDouble.c
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
 * @file dtfToNativeDouble.c
 * @brief Formatting Function DataType Library Layer
 */

#include <dtl.h>
#include <dtlDef.h>
#include <dtDef.h>
#include <dtfFormatting.h>

/**
 * @addtogroup dtfNativeDouble  Formatting
 * @ingroup dtf
 * @internal
 * @{
 */


/*******************************************************************************
 * NATIVE_DOUBLE
 *   to_native_double( char, fmt )
 *     to_native_double( char, fmt ) => [ P, O ]
 * ex) to_native_double( '100.00', '9,999.99' )
 *******************************************************************************/

/**
 * @brief to_native_double
 *        <BR> character를 format string에 맞게 NATIVE_DOUBLE 로 변환
 *        <BR> NATIVE_DOUBLE
 *        <BR> to_native_double( char, fmt )
 *        <BR>   to_native_double( char, fmt ) => [ P, O ]
 *        <BR> ex) to_native_double( '100.00', '9,999.99' )
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> to_native_double(VARCHAR, VARBINARY)
 *                               <BR> to_native_double(LONGVARCHAR, VARBINARY)
 * @param[out] aResultValue      결과 (결과타입 NATIVE_DOUBLE)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[in]  aEnv              environment (stlErrorStack)
 */
stlStatus dtfToNativeDouble( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aInfo,
                             dtlFuncVector  * aVectorFunc,
                             void           * aVectorArg,
                             void           * aEnv )                  
{
    dtlDataValue            * sValue1 = NULL;
    dtlDataValue            * sValue2 = NULL;
    dtfNumberFormatInfo     * sToNumberFormatInfo = NULL;
    dtlDataValue            * sResultValue = NULL;
    stlBool                   sSuccessWithInfo = STL_FALSE;
    stlErrorStack           * sErrorStack = NULL;

    /*
     * X format 인 경우 최대 76자리의 number string이 만들어질 수 있다.
     */ 
    
    stlChar     sNumStr[ DTF_NUMBER_DIGIT_COUNT_FOR_X_FORMAT ];
    stlInt64    sNumStrLen = 0;
    
    DTL_PARAM_VALIDATE( ( aInputArgumentCnt == 1 ) || ( aInputArgumentCnt == 2 ),
                        (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;
    
    DTF_CHECK_DTL_GROUP2( sValue1,
                          DTL_GROUP_CHAR_STRING,
                          DTL_GROUP_LONGVARCHAR_STRING,
                          sErrorStack );    

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_DOUBLE, sErrorStack );
    
    if( aInputArgumentCnt == 1 )
    {
        /*****************************************************
         * to_native_double( str )
         *****************************************************/   
        STL_TRY( dtdDoubleSetValueFromString( (stlChar *)(sValue1->mValue),
                                              sValue1->mLength,
                                              DTL_NATIVE_DOUBLE_DEFAULT_PRECISION,
                                              DTL_SCALE_NA,
                                              DTL_STRING_LENGTH_UNIT_OCTETS,
                                              DTL_INTERVAL_INDICATOR_NA,
                                              DTL_NATIVE_DOUBLE_SIZE,
                                              sResultValue,
                                              & sSuccessWithInfo,
                                              aVectorFunc,
                                              aVectorArg,
                                              aVectorFunc,
                                              aVectorArg,
                                              sErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        /*****************************************************
         * to_native_double( str, fmt )
         *****************************************************/   
        
        sValue2 = aInputArgument[1].mValue.mDataValue;
        sToNumberFormatInfo = (dtfNumberFormatInfo *)(sValue2->mValue);        
        
        DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_VARBINARY, sErrorStack );
        
        STL_TRY( dtfGetNumberLiteralFromString( sValue1,
                                                sToNumberFormatInfo,
                                                sNumStr,
                                                & sNumStrLen,
                                                aVectorFunc,
                                                aVectorArg,
                                                sErrorStack )
                 == STL_SUCCESS );

        STL_TRY( dtdDoubleSetValueFromString( sNumStr,
                                              sNumStrLen,
                                              DTL_NATIVE_DOUBLE_DEFAULT_PRECISION,
                                              DTL_SCALE_NA,
                                              DTL_STRING_LENGTH_UNIT_NA,
                                              DTL_INTERVAL_INDICATOR_NA,
                                              DTL_NATIVE_DOUBLE_SIZE,
                                              sResultValue,
                                              & sSuccessWithInfo,
                                              aVectorFunc,
                                              aVectorArg,
                                              aVectorFunc,
                                              aVectorArg,
                                              sErrorStack )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}



/** @} */
