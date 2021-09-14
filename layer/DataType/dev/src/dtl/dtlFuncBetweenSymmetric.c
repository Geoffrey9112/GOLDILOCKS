/*******************************************************************************
 * dtlFuncBetweenSymmetric.c
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
 * @file dtlFuncBetweenSymmetric.c
 * @brief Data Type Layer BetweenSymmetric 함수 
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtfBetween.h>


/**
 * @addtogroup dtlFuncBetweenSymmetric
 * @{
 */

stlStatus dtlBetweenSymmetric( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aInfo,
                               dtlFuncVector  * aVectorFunc,
                               void           * aVectorArg,
                               void           * aEnv )
{
    return dtfBetweenSymmetric( aInputArgumentCnt,
                                aInputArgument,
                                aResultValue,
                                aInfo,
                                aVectorFunc,
                                aVectorArg,
                                aEnv );
}

stlStatus dtlNotBetweenSymmetric( stlUInt16        aInputArgumentCnt,
                                  dtlValueEntry  * aInputArgument,
                                  void           * aResultValue,
                                  void           * aInfo,
                                  dtlFuncVector  * aVectorFunc,
                                  void           * aVectorArg,
                                  void           * aEnv )
{
    return dtfNotBetweenSymmetric( aInputArgumentCnt,
                                   aInputArgument,
                                   aResultValue,
                                   aInfo,
                                   aVectorFunc,
                                   aVectorArg,
                                   aEnv );
}

/**
 * @brief BetweenSymmetric function에 대한 정보 얻기
 * @param[in]  aDataTypeArrayCount         aDataTypeArrary의 갯수
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
stlStatus dtlGetFuncInfoBetweenSymmetric( stlUInt16               aDataTypeArrayCount,
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
    dtlDataType   sDataType[ 2 ];
    dtlDataType   sReturnDataType[ 2 ];
    
    dtlGroup      sGroup;
    stlUInt8      i;
    
    /* symmetric은 A between symmetric b and c 시
     * A가 b 와 c 각각 cast를 붙여줘야 하므로
     * A.1 A.2를 갖게 된다.
     * 그러므로 argument는 3 이지만 실질적 값은 +1 인 4이다.
     */ 

    STL_PARAM_VALIDATE( ( aDataTypeArrayCount == 3 + 1 ), aErrorStack );
    STL_PARAM_VALIDATE( ( aFuncArgDataTypeArrayCount == 3 + 1 ), aErrorStack );
    
    /**
     * function이 존재하는지 확인 ( Return Type )
     *
     * A between symmetric B and C
     * = (( A >= B ) && ( A <= C ) ) || ( ( A >= C ) && ( A <= B ) )
     *
     *                          이므로, 각각의 Type의 Compare를 확인해 봐야 한다.
     * 
     * A의 경우에는 B와 C를 각 따로 conversion을 시켜줘야 하므로 
     * A는 A.1과 A.2를 갖고 있어 각각 B와 C의 conversion cast를 수행하게 된다.
     *
     * aDataTypeArray[4]
     * aDataTypeArray[0] = A.1, aDataTypeArray[1] = A.2, aDataTypeArray[2] = B, aDataTypeArray[3] = C
     * 
     */

    for( i = 2; i < aDataTypeArrayCount ; i ++ )
    {
        sDataType[ 0 ] = aDataTypeArray[i - 2];
        sDataType[ 1 ] = aDataTypeArray[i];

        STL_PARAM_VALIDATE( ( ( aDataTypeArray[0] >= DTL_TYPE_BOOLEAN ) &&
                              ( aDataTypeArray[0] < DTL_TYPE_MAX ) ) &&
                            ( ( aDataTypeArray[1] >= DTL_TYPE_BOOLEAN ) &&
                              ( aDataTypeArray[1] < DTL_TYPE_MAX ) ),
                            aErrorStack );
        
        sGroup = dtlConversionGroup
            [dtlDataTypeGroup[sDataType[ 0 ]]]
            [dtlDataTypeGroup[sDataType[ 1 ]]];
        
        STL_TRY_THROW( (sGroup != DTL_GROUP_MAX), ERROR_NOT_APPLICABLE );

        STL_TRY( dtlSetFuncArgDataTypeInfo( sDataType,
                                            2,
                                            sGroup,
                                            sReturnDataType,
                                            aErrorStack )
             == STL_SUCCESS );

        aFuncArgDataTypeArray[i - 2] = sReturnDataType[0];
        aFuncArgDataTypeArray[i]     = sReturnDataType[1];

        /**
         * ( A.1 >= B ) && ( A.2 <= C ) || ( A.2 >= C ) && ( A.1 <= B )
         *
         * ( A.1 >= B ) && ( A.1 <= B )
         * 
         * ( A.2 >= C ) && ( A.2 <= C )
         */ 
        STL_TRY_THROW( dtlPhysicalCompareFuncList[ aFuncArgDataTypeArray[i - 2] ][ aFuncArgDataTypeArray[i] ]
                       != NULL,
                       ERROR_NOT_APPLICABLE);
    }

    for( i = 0; i < aFuncArgDataTypeArrayCount; i++ )
    {
        aFuncArgDataTypeDefInfoArray[i].mArgNum1 =
            gResultDataTypeDef[aFuncArgDataTypeArray[i]].mArgNum1;
        aFuncArgDataTypeDefInfoArray[i].mArgNum2 =
            gResultDataTypeDef[aFuncArgDataTypeArray[i]].mArgNum2;
        aFuncArgDataTypeDefInfoArray[i].mStringLengthUnit =
            gResultDataTypeDef[aFuncArgDataTypeArray[i]].mStringLengthUnit;
        aFuncArgDataTypeDefInfoArray[i].mIntervalIndicator =
            gResultDataTypeDef[aFuncArgDataTypeArray[i]].mIntervalIndicator;
    }

    /**
     * @todo INTERVAL Indicator정리
     */
    
    /**
     * INTERVAL TYPE인 경우, Indicator를 설정.
     */
    for( i = 2; i < aFuncArgDataTypeArrayCount; i ++ )
    {
        if( ((aFuncArgDataTypeArray[i - 2] == DTL_TYPE_INTERVAL_YEAR_TO_MONTH) &&
             (aFuncArgDataTypeArray[i] == DTL_TYPE_INTERVAL_YEAR_TO_MONTH))
            ||
            ((aFuncArgDataTypeArray[i - 2] == DTL_TYPE_INTERVAL_DAY_TO_SECOND) &&
             (aFuncArgDataTypeArray[i] == DTL_TYPE_INTERVAL_DAY_TO_SECOND)) )
        {
            if( ((aDataTypeArray[i - 2] == DTL_TYPE_INTERVAL_YEAR_TO_MONTH) &&
                 (aDataTypeArray[i] != DTL_TYPE_INTERVAL_YEAR_TO_MONTH))
                ||
                ((aDataTypeArray[i - 2] == DTL_TYPE_INTERVAL_DAY_TO_SECOND) &&
                 (aDataTypeArray[i] != DTL_TYPE_INTERVAL_DAY_TO_SECOND)) )
            {
                aFuncArgDataTypeDefInfoArray[i - 2].mIntervalIndicator =
                    aDataTypeDefInfoArray[i - 2].mIntervalIndicator;
                aFuncArgDataTypeDefInfoArray[i].mIntervalIndicator =
                    aDataTypeDefInfoArray[i - 2].mIntervalIndicator;
            }
            else if( ((aDataTypeArray[i] == DTL_TYPE_INTERVAL_YEAR_TO_MONTH) &&
                      (aDataTypeArray[i - 2] != DTL_TYPE_INTERVAL_YEAR_TO_MONTH))
                     ||
                     ((aDataTypeArray[i] == DTL_TYPE_INTERVAL_DAY_TO_SECOND) &&
                      (aDataTypeArray[i - 2] != DTL_TYPE_INTERVAL_DAY_TO_SECOND)) )
            {
                aFuncArgDataTypeDefInfoArray[i - 2].mIntervalIndicator =
                    aDataTypeDefInfoArray[i].mIntervalIndicator;
                aFuncArgDataTypeDefInfoArray[i].mIntervalIndicator =
                    aDataTypeDefInfoArray[i].mIntervalIndicator;
            }
            else
            {
                // Do Nothing
            }
        }
    }
         
    /**
     * @todo function의 result data type의 info가 저장되는 부분을 고민해야 함.
     */
    
    *aFuncResultDataType = DTL_TYPE_BOOLEAN;

    aFuncResultDataTypeDefInfo->mArgNum1 = gResultDataTypeDef[DTL_TYPE_BOOLEAN].mArgNum1;
    aFuncResultDataTypeDefInfo->mArgNum2 = gResultDataTypeDef[DTL_TYPE_BOOLEAN].mArgNum2;
    aFuncResultDataTypeDefInfo->mStringLengthUnit =
        gResultDataTypeDef[DTL_TYPE_BOOLEAN].mStringLengthUnit;
    aFuncResultDataTypeDefInfo->mIntervalIndicator =
        gResultDataTypeDef[DTL_TYPE_BOOLEAN].mIntervalIndicator;

    *aFuncPtrIdx = 0;

    return STL_SUCCESS;

    STL_CATCH( ERROR_NOT_APPLICABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_COMPARISON_NOT_APPLICABLE,
                      NULL,
                      aErrorStack,
                      dtlDataTypeName[aDataTypeArray[0]],
                      dtlDataTypeName[aDataTypeArray[i]] );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Between Symmetric function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlGetFuncPtrBetweenSymmetric( stlUInt32             aFuncPtrIdx,
                                         dtlBuiltInFuncPtr   * aFuncPtr,
                                         stlErrorStack       * aErrorStack )
{
    /**
     * output 설정
     */
    
     *aFuncPtr = dtlBetweenSymmetric;
    
    return STL_SUCCESS;
}


/**
 * @brief Not Between Symmetric function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlGetFuncPtrNotBetweenSymmetric( stlUInt32             aFuncPtrIdx,
                                            dtlBuiltInFuncPtr   * aFuncPtr,
                                            stlErrorStack       * aErrorStack )
{
    /**
     * output 설정
     */
    
     *aFuncPtr = dtlNotBetweenSymmetric;
    
    return STL_SUCCESS;
}

    
/** @} */
