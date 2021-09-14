/*******************************************************************************
 * dtlFuncAcos.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtlFuncAcos.c 1841 2011-09-07 07:14:06Z jhkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file dtlFuncAcos.c
 * @brief DataType Layer Function관련 함수 
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtlFuncCommon.h>
#include <dtfMathematical.h>

/**
 * @ingroup dtlFunction Function관련 함수
 * @{
 */


/*
 * 대표 함수의 정보를 정의한다.
 * mValidArgList는 함수를 결정하는데 사용되는 arg index의 list이다.
 * 함수를 결정하는데 사용되지 않는 arg는 무조건 함수의 원형 type으로
 * 리턴한다.
 */
const dtlFuncInfo dtlFuncAcosInfo =
{
    { STL_TRUE, }      /* 함수를 결정하는데 판단이 되는 argument index list */
};


/*
 * 각 함수별 arg type, result type을 정의한다.
 * 주의: 빠르게 처리 가능한 함수를 먼저 정의해야 한다.
 */
const dtlFuncArgInfo dtlFuncAcosList[] =
{
     { dtlNumericAcos,
       { DTL_TYPE_NUMBER },
       DTL_TYPE_NUMBER
     },
     { dtlDoubleAcos,
       { DTL_TYPE_NATIVE_DOUBLE },
       DTL_TYPE_NATIVE_DOUBLE
     },
     
     { NULL,
       { DTL_TYPE_NA, },
       DTL_TYPE_NA
     }
};


/* stlStatus dtlBigIntAcos( stlUInt16        aInputArgumentCnt, */
/*                          dtlValueEntry  * aInputArgument, */
/*                          void           * aResultValue, */
/*                          void           * aInfo, */
/*                          dtlFuncVector  * aVectorFunc, */
/*                          void           * aVectorArg, */
/*                          void           * aEnv ) */
/* { */
/*     return dtfBigIntAcos( aInputArgumentCnt, */
/*                           aInputArgument, */
/*                           aResultValue, */
/*                           aEnv ); */
/* } */


stlStatus dtlDoubleAcos( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv )
{
    return dtfDoubleAcos( aInputArgumentCnt,
                          aInputArgument,
                          aResultValue,
                          aEnv );
}


stlStatus dtlNumericAcos( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv )
{
    return dtfNumericAcos( aInputArgumentCnt,
                           aInputArgument,
                           aResultValue,
                           aEnv );
}


/**
 * @brief Acos function에 대한 정보 얻기
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
stlStatus dtlGetFuncInfoAcos( stlUInt16               aDataTypeArrayCount,
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
    stlInt32   i;
    
    STL_TRY( dtlGetFuncInfoCommon( &dtlFuncAcosInfo,
                                   dtlFuncAcosList,
                                   aDataTypeArrayCount,
                                   aIsConstantData,
                                   aDataTypeArray,
                                   aDataTypeDefInfoArray,
                                   aFuncArgDataTypeArrayCount,
                                   aFuncArgDataTypeArray,
                                   aFuncArgDataTypeDefInfoArray,
                                   aFuncResultDataType,
                                   aFuncResultDataTypeDefInfo,
                                   aFuncPtrIdx,
                                   aErrorStack )
             == STL_SUCCESS );

    if( dtlDataTypeGroup[ aDataTypeArray[0] ] == DTL_GROUP_BINARY_INTEGER )
    {
        /* function argument 조정 */
        aFuncArgDataTypeArray[0] = DTL_TYPE_NATIVE_DOUBLE;
        
        aFuncArgDataTypeDefInfoArray[0].mArgNum1 =
            gResultDataTypeDef[aFuncArgDataTypeArray[0]].mArgNum1;
        aFuncArgDataTypeDefInfoArray[0].mArgNum2 =
            gResultDataTypeDef[aFuncArgDataTypeArray[0]].mArgNum2;
        aFuncArgDataTypeDefInfoArray[0].mStringLengthUnit =
            gResultDataTypeDef[aFuncArgDataTypeArray[0]].mStringLengthUnit;                    
        aFuncArgDataTypeDefInfoArray[0].mIntervalIndicator =
            gResultDataTypeDef[aFuncArgDataTypeArray[0]].mIntervalIndicator;
        
        /* result type 조정 */
        *aFuncResultDataType = DTL_TYPE_NATIVE_DOUBLE;
        
        aFuncResultDataTypeDefInfo->mArgNum1 = gResultDataTypeDef[*aFuncResultDataType].mArgNum1;
        aFuncResultDataTypeDefInfo->mArgNum2 = gResultDataTypeDef[*aFuncResultDataType].mArgNum2;
        aFuncResultDataTypeDefInfo->mStringLengthUnit =
            gResultDataTypeDef[*aFuncResultDataType].mStringLengthUnit;
        aFuncResultDataTypeDefInfo->mIntervalIndicator =
            gResultDataTypeDef[*aFuncResultDataType].mIntervalIndicator;
        
        /* function ptr idx 조정 */
        i = 0;
        while( dtlFuncAcosList[i].mFuncPtr != NULL )
        {
            if( dtlFuncAcosList[i].mFuncPtr == dtlDoubleAcos )
            {
                break;
            }
            else
            {
                i++;
            }
        }
        
        STL_DASSERT( dtlFuncAcosList[i].mFuncPtr == dtlDoubleAcos );
        *aFuncPtrIdx = i;
    }
    else
    {
        /* Do Nothing */
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Acos function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlGetFuncPtrAcos( stlUInt32             aFuncPtrIdx,
                             dtlBuiltInFuncPtr   * aFuncPtr,
                             stlErrorStack       * aErrorStack )
{
    /**
     * output 설정
     */

    /*
     * dtlAcos가 n차원 배열임으로 1차원 배열로 typecast해서
     * aFuncPtrIdx를 적용한다.
     */
    *aFuncPtr = dtlFuncAcosList[aFuncPtrIdx].mFuncPtr;

    return STL_SUCCESS;
}



/** @} */
