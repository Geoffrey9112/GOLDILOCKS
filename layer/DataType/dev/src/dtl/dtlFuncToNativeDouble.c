

/**
 * @file dtlFuncToNativeDouble.c
 * @brief Data Type Layer To_Native_Double 함수 
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtfFormatting.h>

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
const dtlFuncInfo dtlFuncToNativeDoubleInfo =
{
    { STL_TRUE,  }      /* 함수를 결정하는데 판단이 되는 argument index list */
};


/*
 * 각 함수별 arg type, result type을 정의한다.
 * 주의: 빠르게 처리 가능한 함수를 먼저 정의해야 한다.
 */
const dtlFuncArgInfo dtlFuncToNativeDoubleList[] =
{
    { dtlToNativeDouble,
      { DTL_TYPE_VARCHAR, DTL_TYPE_VARBINARY },
      DTL_TYPE_NATIVE_DOUBLE
    },
    { dtlToNativeDouble,
      { DTL_TYPE_LONGVARCHAR, DTL_TYPE_VARBINARY },
      DTL_TYPE_NATIVE_DOUBLE
    },

    { NULL,
      { DTL_TYPE_NA, },
      DTL_TYPE_NA
    }
};    
    

stlStatus dtlToNativeDouble( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aInfo,
                             dtlFuncVector  * aVectorFunc,
                             void           * aVectorArg,
                             void           * aEnv )
{
    return dtfToNativeDouble( aInputArgumentCnt,
                              aInputArgument,
                              aResultValue,
                              aInfo,
                              aVectorFunc,
                              aVectorArg,
                              aEnv );            
}


/**
 * @brief To_Native_Double  function에 대한 정보 얻기
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
stlStatus dtlGetFuncInfoToNativeDouble( stlUInt16               aDataTypeArrayCount,
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
    STL_TRY( dtlGetFuncInfoCommon( &dtlFuncToNativeDoubleInfo,
                                   dtlFuncToNativeDoubleList,
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
 * @brief To_Native_Double function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlGetFuncPtrToNativeDouble( stlUInt32             aFuncPtrIdx,
                                       dtlBuiltInFuncPtr   * aFuncPtr,
                                       stlErrorStack       * aErrorStack )
{
    /**
     * output 설정
     */

    *aFuncPtr = dtlFuncToNativeDoubleList[aFuncPtrIdx].mFuncPtr;
    
    return STL_SUCCESS;
}

/** @} */
