/*******************************************************************************
 * dtlFuncIsNotUnknown.c
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
 * @file dtlFuncIsNotUnknown.c
 * @brief Data Type Layer IS NOT UNKNOWN 논리연산 
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtfOperLogical.h>

/**
 * @addtogroup dtlFuncIs IS 논리연산
 * @ingroup dtlOperLogical
 * @{
 */

/**
 * @brief IS NOT UNKNOWN function에 대한 정보 얻기
 * @param[in]  aDataTypeArrayCount         aDataTypeArray의 갯수
 * @param[in]  aIsConstantData             constant value인지 여부
 * @param[in]  aDataTypeArray              function을 수행할 data type
 * @param[in]  aDataTypeDefInfoArray       aDataTypeArray에 대응되는 dtlDataTypeDefInfo정보 
 * @param[in]  aFuncArgDataTypeArrayCount  function의 input argument count
 * @param[out] aFuncArgDataTypeArray       function 수행시 수행 argument type
 * @param[out] aFuncArgDataTypeDefInfoArray aFuncArgDataTypeArray에 대응되는 dtlDataTypeDefInfo정보
 * @param[out] aFuncResultDataType         function 수행시 return type
 * @param[out] aFuncResultDataTypeDefInfo  aFuncResultDataType에 대응하는 dtlDataTypeDefInfo정보
 * @param[out] aFuncPtrIdx                 함수 수행을 위한 function pointer 얻기 위한 식별 정보
 * @param[in]  aVectorFunc                 Function Vector
 * @param[in]  aVectorArg                  Vector Argument
 * @param[out] aErrorStack                 에러 스택
 */
stlStatus dtlGetFuncInfoIsNotUnknown( stlUInt16               aDataTypeArrayCount,
                                      stlBool               * aIsConstantData,
                                      dtlDataType           * aDataTypeArray,
                                      dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                      stlUInt16               aFuncArgDataTypeArrayCount,
                                      dtlDataType           * aFuncArgDataTypeArray,
                                      dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                      dtlDataType           * aFuncResultDataType,
                                      dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                      stlUInt32             * aFuncPtrIdx,
                                      dtlFuncVector         * aVectorFunc,
                                      void                  * aVectorArg,                                     
                                      stlErrorStack         * aErrorStack )
{
    STL_PARAM_VALIDATE( aDataTypeArrayCount == 1, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 1, aErrorStack );

    
    /**
     * output 설정
     */
    
    aFuncArgDataTypeArray[0] = DTL_TYPE_BOOLEAN;

    aFuncArgDataTypeDefInfoArray[0].mArgNum1 =
        gResultDataTypeDef[aFuncArgDataTypeArray[0]].mArgNum1;
    aFuncArgDataTypeDefInfoArray[0].mArgNum2 =
        gResultDataTypeDef[aFuncArgDataTypeArray[0]].mArgNum2;
    aFuncArgDataTypeDefInfoArray[0].mStringLengthUnit =
        gResultDataTypeDef[aFuncArgDataTypeArray[0]].mStringLengthUnit;
    aFuncArgDataTypeDefInfoArray[0].mIntervalIndicator =
        gResultDataTypeDef[aFuncArgDataTypeArray[0]].mIntervalIndicator;
    
    *aFuncResultDataType = DTL_TYPE_BOOLEAN;

    aFuncResultDataTypeDefInfo->mArgNum1 = gResultDataTypeDef[DTL_TYPE_BOOLEAN].mArgNum1;
    aFuncResultDataTypeDefInfo->mArgNum2 = gResultDataTypeDef[DTL_TYPE_BOOLEAN].mArgNum2;
    aFuncResultDataTypeDefInfo->mStringLengthUnit =
        gResultDataTypeDef[DTL_TYPE_BOOLEAN].mStringLengthUnit;
    aFuncResultDataTypeDefInfo->mIntervalIndicator =
        gResultDataTypeDef[DTL_TYPE_BOOLEAN].mIntervalIndicator;
    
    *aFuncPtrIdx = 1;   /* dtlCompOperArg1FuncList */
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief IS NOT UNKNOWN function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlGetFuncPtrIsNotUnknown( stlUInt32             aFuncPtrIdx,
                                     dtlBuiltInFuncPtr   * aFuncPtr,
                                     stlErrorStack       * aErrorStack )
{
    /**
     * output 설정
     */

    *aFuncPtr = dtlOperIsNotNull;
    
    return STL_SUCCESS;
}


/** @} dtlFuncIsNotUnknown */
