/*******************************************************************************
 * dtlFuncNegative.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtlFuncNegative.c 1841 2011-09-07 07:14:06Z jhkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file dtlFuncNegative.c
 * @brief DataType Layer Function관련 함수 
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtlFuncCommon.h>
#include <dtfArithmetic.h>

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
const dtlFuncInfo dtlFuncNegativeInfo =
{
    { STL_TRUE, } /* 함수를 결정하는데 판단이 되는 argument cnt */
};


/*
 * 각 함수별 arg type, result type을 정의한다.
 * 주의: 빠르게 처리 가능한 함수를 먼저 정의해야 한다.
 */
const dtlFuncArgInfo dtlFuncNegativeList[] =
{
     { dtlNegative,
       { DTL_TYPE_NATIVE_BIGINT },
       DTL_TYPE_NATIVE_BIGINT
     },
     { dtlNegative,
       { DTL_TYPE_NUMBER },
       DTL_TYPE_NUMBER
     },    
     { dtlNegative,
       { DTL_TYPE_NATIVE_DOUBLE },
       DTL_TYPE_NATIVE_DOUBLE
     },
     
    
     { NULL,
       { DTL_TYPE_NA, },
       DTL_TYPE_NA
     }
};


/**
 * @brief Negative function에 대한 정보 얻기
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
stlStatus dtlGetFuncInfoNegative( stlUInt16               aDataTypeArrayCount,
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
    dtlGroup            sGroup = DTL_GROUP_MAX;
    
    STL_TRY( dtlGetFuncInfoCommon( &dtlFuncNegativeInfo,
                                   dtlFuncNegativeList,
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

    /*
     * 함수의 인자로 숫자타입이 오는 경우,
     * 함수인자의 타입과 결과타입은 동일하다.
     */
    
    sGroup = dtlDataTypeGroup[ aDataTypeArray[0] ];
    
    if( ( sGroup == DTL_GROUP_BINARY_INTEGER ) ||
        ( sGroup == DTL_GROUP_NUMBER ) ||
        ( sGroup == DTL_GROUP_BINARY_REAL ) )
    {
        aFuncArgDataTypeArray[0] = aDataTypeArray[0];
        DTL_COPY_DATA_TYPE_DEF_INFO( & aFuncArgDataTypeDefInfoArray[0],
                                     & aDataTypeDefInfoArray[0] );        
        
        *aFuncResultDataType = aDataTypeArray[0];
        DTL_COPY_DATA_TYPE_DEF_INFO( aFuncResultDataTypeDefInfo,
                                     & aDataTypeDefInfoArray[0] );
    }
    else
    {
        /* Do Nothing */
    }
    
    STL_DASSERT( gDtfNegativeFunc[ *aFuncResultDataType ] != NULL );

    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Negative function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlGetFuncPtrNegative( stlUInt32             aFuncPtrIdx,
                                 dtlBuiltInFuncPtr   * aFuncPtr,
                                 stlErrorStack       * aErrorStack )
{
    /**
     * output 설정
     */
    
    *aFuncPtr = dtlNegative;
    
    return STL_SUCCESS;
}


stlStatus dtlNegative( stlUInt16        aInputArgumentCnt,
                       dtlValueEntry  * aInputArgument,
                       void           * aResultValue,
                       void           * aInfo,
                       dtlFuncVector  * aVectorFunc,
                       void           * aVectorArg,
                       void           * aEnv )
{
    STL_DASSERT( gDtfNegativeFunc[ aInputArgument[0].mValue.mDataValue->mType ] != NULL );
    
    STL_TRY( gDtfNegativeFunc[ aInputArgument[0].mValue.mDataValue->mType ](
                 aInputArgumentCnt,
                 aInputArgument,
                 aResultValue,
                 aInfo,
                 aVectorFunc,
                 aVectorArg,
                 (stlErrorStack *)aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/** @} */
