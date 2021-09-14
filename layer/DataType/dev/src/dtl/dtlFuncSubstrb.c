/*******************************************************************************
 * dtlFuncSubstrb.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file dtlFuncSubstrb.c
 * @brief DataType Layer Function관련 함수 
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtlFuncCommon.h>
#include <dtfString.h>

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
const dtlFuncInfo dtlFuncSubstrbInfo =
{
    { STL_TRUE, } /* 함수를 결정하는데 판단이 되는 argument cnt */
};


/*
 * 각 함수별 arg type, result type을 정의한다.
 * 주의: 빠르게 처리 가능한 함수를 먼저 정의해야 한다.
 */
const dtlFuncArgInfo dtlFuncSubstrbList[] =
{
     { dtlStringSubstrb,
       { DTL_TYPE_VARCHAR, DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NATIVE_BIGINT },
       DTL_TYPE_VARCHAR
     },
     { dtlLongvarcharStringSubstrb,
       { DTL_TYPE_LONGVARCHAR, DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NATIVE_BIGINT },
       DTL_TYPE_LONGVARCHAR
     },
     
     { dtlBinaryStringSubstrb,
       { DTL_TYPE_VARBINARY, DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NATIVE_BIGINT },
       DTL_TYPE_VARBINARY
     },
     { dtlLongvarbinaryStringSubstrb,
       { DTL_TYPE_LONGVARBINARY, DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NATIVE_BIGINT },
       DTL_TYPE_LONGVARBINARY
     },
     
     
     { NULL,
       { DTL_TYPE_NA, },
       DTL_TYPE_NA
     }
};


stlStatus dtlStringSubstrb( stlUInt16        aInputArgumentCnt,
                            dtlValueEntry  * aInputArgument,
                            void           * aResultValue,
                            void           * aInfo,
                            dtlFuncVector  * aVectorFunc,
                            void           * aVectorArg,
                            void           * aEnv )
{
    return dtfStringSubstrb( aInputArgumentCnt,
                             aInputArgument,
                             aResultValue,
                             aVectorFunc,
                             aVectorArg,
                             aEnv );
}


stlStatus dtlLongvarcharStringSubstrb( stlUInt16        aInputArgumentCnt,
                                       dtlValueEntry  * aInputArgument,
                                       void           * aResultValue,
                                       void           * aInfo,
                                       dtlFuncVector  * aVectorFunc,
                                       void           * aVectorArg,
                                       void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    if( sValue1->mLength > sResultValue->mBufferSize )
    {
        STL_TRY( aVectorFunc->mReallocLongVaryingMemFunc( aVectorArg,
                                                          sValue1->mLength,
                                                          &sResultValue->mValue,
                                                          (stlErrorStack *)aEnv )
                 == STL_SUCCESS );
        
        sResultValue->mBufferSize = sValue1->mLength;
    }
    
    STL_TRY( dtfStringSubstrb( aInputArgumentCnt,
                               aInputArgument,
                               aResultValue,
                               aVectorFunc,
                               aVectorArg,
                               aEnv )
             == STL_SUCCESS );
    
    STL_DASSERT( sResultValue->mLength <= sResultValue->mBufferSize );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus dtlBinaryStringSubstrb( stlUInt16        aInputArgumentCnt,
                                  dtlValueEntry  * aInputArgument,
                                  void           * aResultValue,
                                  void           * aInfo,
                                  dtlFuncVector  * aVectorFunc,
                                  void           * aVectorArg,
                                  void           * aEnv )
{
    return dtfBinaryStringSubstrb( aInputArgumentCnt,
                                   aInputArgument,
                                   aResultValue,
                                   aVectorFunc,
                                   aVectorArg,
                                   aEnv );
}


stlStatus dtlLongvarbinaryStringSubstrb( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    
    if( sValue1->mLength > sResultValue->mBufferSize )
    {
        STL_TRY( aVectorFunc->mReallocLongVaryingMemFunc( aVectorArg,
                                                          sValue1->mLength,
                                                          &sResultValue->mValue,
                                                          (stlErrorStack *)aEnv )
                 == STL_SUCCESS );
        
        sResultValue->mBufferSize = sValue1->mLength;
    }
    
    STL_TRY( dtfBinaryStringSubstrb( aInputArgumentCnt,
                                     aInputArgument,
                                     aResultValue,
                                     aVectorFunc,
                                     aVectorArg,
                                     aEnv )
             == STL_SUCCESS );
    
    STL_DASSERT( sResultValue->mLength <= sResultValue->mBufferSize );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Substrb function에 대한 정보 얻기
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
stlStatus dtlGetFuncInfoSubstrb( stlUInt16               aDataTypeArrayCount,
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
    STL_TRY( dtlGetFuncInfoCommon( &dtlFuncSubstrbInfo,
                                   dtlFuncSubstrbList,
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
     * result type 에 대한 info 정보 수정.
     */
    
    /*
     * SUBSTRB( str , position [, substring_length ] )
     *          ^^^
     *          
     * result type의 최대 precision 은 str 의 precision 을 넘지 않는다.
     */
    
    aFuncResultDataTypeDefInfo->mArgNum1 =
        aFuncArgDataTypeDefInfoArray[0].mArgNum1;
    aFuncResultDataTypeDefInfo->mArgNum2 =
        aFuncArgDataTypeDefInfoArray[0].mArgNum2;
    aFuncResultDataTypeDefInfo->mStringLengthUnit =
        aFuncArgDataTypeDefInfoArray[0].mStringLengthUnit;
    aFuncResultDataTypeDefInfo->mIntervalIndicator =
        aFuncArgDataTypeDefInfoArray[0].mIntervalIndicator;

    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Substrb function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlGetFuncPtrSubstrb( stlUInt32             aFuncPtrIdx,
                                dtlBuiltInFuncPtr   * aFuncPtr,
                                stlErrorStack       * aErrorStack )
{
    /**
     * output 설정
     */

    /*
     * dtlSubstrb가 n차원 배열임으로 1차원 배열로 typecast해서
     * aFuncPtrIdx를 적용한다.
     */
    *aFuncPtr = dtlFuncSubstrbList[aFuncPtrIdx].mFuncPtr;

    return STL_SUCCESS;
}



/** @} */
