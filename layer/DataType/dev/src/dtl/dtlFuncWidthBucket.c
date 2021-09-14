/*******************************************************************************
 * dtlFuncWidthBucket.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtlFuncWidthBucket.c 1841 2011-09-07 07:14:06Z jhkim $
 *
 * NOTES
 *
 ******************************************************************************/

/**
 * @file dtlFuncWidthBucket.c
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
const dtlFuncInfo dtlFuncWidthBucketInfo =
{
    { STL_TRUE, STL_TRUE, STL_TRUE, STL_FALSE, }      /* 함수를 결정하는데 판단이 되는 argument index list */
};


/*
 * 각 함수별 arg type, result type을 정의한다.
 * 주의: 빠르게 처리 가능한 함수를 먼저 정의해야 한다.
 */
const dtlFuncArgInfo dtlFuncWidthBucketList[] =
{
     { dtlBigIntWidthBucket,
       { DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NUMBER },
       DTL_TYPE_NATIVE_INTEGER
     },
     { dtlNumericWidthBucket,
       { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER, DTL_TYPE_NUMBER, DTL_TYPE_NUMBER },
       DTL_TYPE_NATIVE_INTEGER
     },
     { dtlDoubleWidthBucket,
       { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NUMBER },
       DTL_TYPE_NATIVE_INTEGER
     },

     
     { NULL,
       { DTL_TYPE_NA, },
       DTL_TYPE_NA
     }
};



stlStatus dtlBigIntWidthBucket( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                void           * aInfo,
                                dtlFuncVector  * aVectorFunc,
                                void           * aVectorArg,
                                void           * aEnv )
{
    return dtfBigIntWidthBucket( aInputArgumentCnt,
                                 aInputArgument,
                                 aResultValue,
                                 aVectorFunc,
                                 aVectorArg,
                                 aEnv );
}

stlStatus dtlNumericWidthBucket( stlUInt16        aInputArgumentCnt,
                                 dtlValueEntry  * aInputArgument,
                                 void           * aResultValue,
                                 void           * aInfo,
                                 dtlFuncVector  * aVectorFunc,
                                 void           * aVectorArg,
                                 void           * aEnv )
{
    return dtfNumericWidthBucket( aInputArgumentCnt,
                                  aInputArgument,
                                  aResultValue,
                                  aVectorFunc,
                                  aVectorArg,
                                  aEnv );
}

stlStatus dtlDoubleWidthBucket( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                void           * aInfo,
                                dtlFuncVector  * aVectorFunc,
                                void           * aVectorArg,
                                void           * aEnv )
{
    return dtfDoubleWidthBucket( aInputArgumentCnt,
                                 aInputArgument,
                                 aResultValue,
                                 aVectorFunc,
                                 aVectorArg,
                                 aEnv );
}


/**
 * @brief WidthBucket function에 대한 정보 얻기
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
stlStatus dtlGetFuncInfoWidthBucket( stlUInt16               aDataTypeArrayCount,
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
    STL_TRY( dtlGetFuncInfoCommon( &dtlFuncWidthBucketInfo,
                                   dtlFuncWidthBucketList,
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

     return STL_SUCCESS;

     STL_FINISH;

     return STL_FAILURE;
}

/**
 * @brief WidthBucket function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlGetFuncPtrWidthBucket( stlUInt32             aFuncPtrIdx,
                                    dtlBuiltInFuncPtr   * aFuncPtr,
                                    stlErrorStack       * aErrorStack )
{
    /**
     * output 설정
     */

    *aFuncPtr = dtlFuncWidthBucketList[aFuncPtrIdx].mFuncPtr;

    return STL_SUCCESS;
}



/** @} */
