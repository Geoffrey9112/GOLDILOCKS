/*******************************************************************************
 * dtlFuncIsNot.c
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
 * @file dtlFuncIsNot.c
 * @brief Data Type Layer IS NOT 논리연산 
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtfOperLogical.h>

/**
 * @addtogroup dtlFuncIsNot IS NOT 논리연산
 * @ingroup dtlOperLogical
 * @{
 */

/**
 * @brief IS NOT function에 대한 정보 얻기
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
stlStatus dtlGetFuncInfoIsNot( stlUInt16               aDataTypeArrayCount,
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
    stlUInt16   i;
    
    STL_PARAM_VALIDATE( aDataTypeArrayCount == 2, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 2, aErrorStack );

    /*
     * output 설정 
     */

    for( i = 0; i < aFuncArgDataTypeArrayCount; i++ )
    {
        aFuncArgDataTypeArray[i] = DTL_TYPE_BOOLEAN;
        
        aFuncArgDataTypeDefInfoArray[i].mArgNum1 =
            gDefaultDataTypeDef[aFuncArgDataTypeArray[i]].mArgNum1;
        aFuncArgDataTypeDefInfoArray[i].mArgNum2 =
            gDefaultDataTypeDef[aFuncArgDataTypeArray[i]].mArgNum2;
        aFuncArgDataTypeDefInfoArray[i].mStringLengthUnit =
            gDefaultDataTypeDef[aFuncArgDataTypeArray[i]].mStringLengthUnit;
        aFuncArgDataTypeDefInfoArray[i].mIntervalIndicator =
            gDefaultDataTypeDef[aFuncArgDataTypeArray[i]].mIntervalIndicator;
    }

    *aFuncResultDataType = DTL_TYPE_BOOLEAN;

    aFuncResultDataTypeDefInfo->mArgNum1 = gResultDataTypeDef[DTL_TYPE_BOOLEAN].mArgNum1;
    aFuncResultDataTypeDefInfo->mArgNum2 = gResultDataTypeDef[DTL_TYPE_BOOLEAN].mArgNum2;
    aFuncResultDataTypeDefInfo->mStringLengthUnit =
        gResultDataTypeDef[DTL_TYPE_BOOLEAN].mStringLengthUnit;
    aFuncResultDataTypeDefInfo->mIntervalIndicator =
        gResultDataTypeDef[DTL_TYPE_BOOLEAN].mIntervalIndicator;

    *aFuncPtrIdx = 0;

    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;   
}

/**
 * @brief IS NOT function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlGetFuncPtrIsNot( stlUInt32             aFuncPtrIdx,
                              dtlBuiltInFuncPtr   * aFuncPtr,
                              stlErrorStack       * aErrorStack )
{
    /**
     * output 설정
     */

    *aFuncPtr = dtlFuncIsNot;
    
    return STL_SUCCESS;
}


stlStatus dtlFuncIsNot( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aInfo,
                        dtlFuncVector  * aVectorFunc,
                        void           * aVectorArg,
                        void           * aEnv )
{
    return dtfIsNot( aInputArgumentCnt,
                     aInputArgument,
                     aResultValue,
                     aEnv );
}


/** @} dtlFuncIsNot */
