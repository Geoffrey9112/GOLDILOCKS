/*******************************************************************************
 * dtlFuncUpper.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtlFuncUpper.c 1841 2011-09-07 07:14:06Z jhkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file dtlFuncUpper.c
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


/**
 * @todo binary type도 구현해야 함.
 */

/*
 * 대표 함수의 정보를 정의한다.
 * mValidArgList는 함수를 결정하는데 사용되는 arg index의 list이다.
 * 함수를 결정하는데 사용되지 않는 arg는 무조건 함수의 원형 type으로
 * 리턴한다.
 */
const dtlFuncInfo dtlFuncUpperInfo =
{
    { STL_TRUE } /* 함수를 결정하는데 판단이 되는 argument cnt */
};


/*
 * 각 함수별 arg type, result type을 정의한다.
 * 주의: 빠르게 처리 가능한 함수를 먼저 정의해야 한다.
 */
const dtlFuncArgInfo dtlFuncUpperList[] =
{
    { dtlUpper,
      { DTL_TYPE_VARCHAR },
      DTL_TYPE_VARCHAR
    },
    { dtlLongvarcharUpper,
      { DTL_TYPE_LONGVARCHAR },
      DTL_TYPE_LONGVARCHAR
    },

    { NULL,
      { DTL_TYPE_NA, },
      DTL_TYPE_NA
    }
};


stlStatus dtlUpper( stlUInt16        aInputArgumentCnt,
                    dtlValueEntry  * aInputArgument,
                    void           * aResultValue,
                    void           * aInfo,
                    dtlFuncVector  * aVectorFunc,
                    void           * aVectorArg,
                    void           * aEnv )
{
    return dtfUpper( aInputArgumentCnt,
                     aInputArgument,
                     aResultValue,
                     aVectorFunc,
                     aVectorArg,
                     aEnv );
}

stlStatus dtlLongvarcharUpper( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aInfo,
                               dtlFuncVector  * aVectorFunc,
                               void           * aVectorArg,
                               void           * aEnv )
{
    dtlDataValue * sSrcValue;
    dtlDataValue * sResultValue;
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );
    
    sSrcValue = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    if( sSrcValue->mLength > sResultValue->mBufferSize )
    {
        STL_TRY( aVectorFunc->mReallocLongVaryingMemFunc( aVectorArg,
                                                          sSrcValue->mLength,
                                                          &sResultValue->mValue,
                                                          (stlErrorStack *)aEnv )
                 == STL_SUCCESS );
        
        sResultValue->mBufferSize = sSrcValue->mLength;
    }
    
    STL_TRY( dtfUpper( aInputArgumentCnt,
                       aInputArgument,
                       aResultValue,
                       aVectorFunc,
                       aVectorArg,
                       aEnv ) == STL_SUCCESS );

    STL_DASSERT( sResultValue->mBufferSize >= sResultValue->mLength );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Upper function에 대한 정보 얻기
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
stlStatus dtlGetFuncInfoUpper( stlUInt16               aDataTypeArrayCount,
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
    DTL_PARAM_VALIDATE( aDataTypeArrayCount == 1, aErrorStack );
    DTL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 1, aErrorStack );

    STL_TRY( dtlGetFuncInfoCommon( &dtlFuncUpperInfo,
                                   dtlFuncUpperList,
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
     * result type 조정.
     */
    
    if( aFuncArgDataTypeArray[0] == DTL_TYPE_CHAR )
    {
        *aFuncResultDataType = aFuncArgDataTypeArray[0];
    }
    else
    {
        /* Do Nothing */
    }
    
    /*
     * result type 에 대한 info 정보 수정.
     */

    /*
     * UPPER( str )
     *       ^^^^^
     *
     * 결과 타입 정보는 str 의 정보로 설정해준다.       
     * precision은 str 의 precision 을 넘지 않는다.      
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
 * @brief Upper function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlGetFuncPtrUpper( stlUInt32             aFuncPtrIdx,
                              dtlBuiltInFuncPtr   * aFuncPtr,
                              stlErrorStack       * aErrorStack )
{
    /**
     * output 설정
     */

    *aFuncPtr = dtlFuncUpperList[aFuncPtrIdx].mFuncPtr;    
    
    return STL_SUCCESS;
}



/** @} */
