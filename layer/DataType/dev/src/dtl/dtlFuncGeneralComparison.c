/*******************************************************************************
 * dtlFuncGeneralComparison.c
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
 * @file dtlFuncGeneralComparison.c
 * @brief DataType Layer Function관련 함수 
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtfGeneralComparison.h>
#include <dtlFuncCommon.h>
#include <dtlFuncGeneralComparison.h>


/**
 * @ingroup dtlFunction Function관련 함수
 * @{
 */

/*******************************************************************************
 * GENERAL COMPARISON COMMON
 *******************************************************************************/

stlStatus dtlGetFuncInfoGeneralComparison( stlUInt16               aDataTypeArrayCount,
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
    stlInt32                sIndex           = 0;
    dtlDataType             sFuncArgDataType = DTL_TYPE_MAX;

    
    /**
     * check, maximum number of arguments
     */
    STL_TRY_THROW( ( aDataTypeArrayCount >= DTL_GENERAL_COMPARISON_FUN_INPUT_MIN_ARG_CNT ) &&
                   ( aDataTypeArrayCount <= DTL_GENERAL_COMPARISON_FUN_INPUT_MAX_ARG_CNT ),
                   RAMP_ERR_MAXIMUM_NUMBER_OF_ARGUMENTS_EXCEEDED );
    
    
    /**
     * get result type
     */
    STL_TRY( dtlGetResultTypeForComparisonRule( aDataTypeArrayCount,
                                                aDataTypeArray,
                                                aDataTypeDefInfoArray,
                                                aFuncResultDataType,
                                                aFuncResultDataTypeDefInfo,
                                                aErrorStack )
             == STL_SUCCESS );
    
    /**
     * copy to function arguemnts's data type
     */
    for( sIndex = 0; sIndex < aDataTypeArrayCount; sIndex++ )
    {
        sFuncArgDataType =
            dtlComparisonFuncArgType[ *aFuncResultDataType ][ aDataTypeArray[ sIndex ] ];
        
        /* dassert */
        STL_DASSERT( sFuncArgDataType != DTL_TYPE_NA );

        /* 변환 타입과 현재 타입이 같은 경우 */
        aFuncArgDataTypeArray[ sIndex ] = sFuncArgDataType;
        if( sFuncArgDataType == aDataTypeArray[ sIndex ])
        {
            DTL_COPY_DATA_TYPE_DEF_INFO(
                & aFuncArgDataTypeDefInfoArray[ sIndex ],
                & aDataTypeDefInfoArray[ sIndex ] );
        }
        else
        {
            /* 변환 타입과 현재 타입이 다른 경우 */
            switch( sFuncArgDataType )
            {
                case DTL_TYPE_BOOLEAN:
                case DTL_TYPE_NATIVE_SMALLINT:
                case DTL_TYPE_NATIVE_INTEGER:
                case DTL_TYPE_NATIVE_BIGINT:
                case DTL_TYPE_NATIVE_REAL:
                case DTL_TYPE_NATIVE_DOUBLE:
                case DTL_TYPE_FLOAT:
                case DTL_TYPE_NUMBER:
                case DTL_TYPE_LONGVARCHAR:    
                case DTL_TYPE_LONGVARBINARY:
                case DTL_TYPE_DATE:
                case DTL_TYPE_ROWID:
                    /* precison, scale, fraction 등을 고려하지 않는 result type */
                    DTL_COPY_DATA_TYPE_DEF_INFO(
                        & aFuncArgDataTypeDefInfoArray[ sIndex ],
                        & gResultDataTypeDef[ sFuncArgDataType ] );
                    break;
                    
                case DTL_TYPE_CHAR:
                case DTL_TYPE_VARCHAR:
                case DTL_TYPE_BINARY:
                case DTL_TYPE_VARBINARY:
                case DTL_TYPE_TIME:
                case DTL_TYPE_TIMESTAMP:
                case DTL_TYPE_TIME_WITH_TIME_ZONE:
                case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
                case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
                case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
                    DTL_COPY_DATA_TYPE_DEF_INFO(
                        & aFuncArgDataTypeDefInfoArray[ sIndex ],
                        aFuncResultDataTypeDefInfo );
                    break;
                

                default:
                    STL_DASSERT(0);
                    break;
            }
        }
    }

    return STL_SUCCESS;


    STL_CATCH( RAMP_ERR_MAXIMUM_NUMBER_OF_ARGUMENTS_EXCEEDED )
    {
        /* "maximum number of arguments exceeded" */
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_MAXIMUM_NUMBER_OF_ARGUMENTS_EXCEEDED,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;
               
    return STL_FAILURE;    
}





/*******************************************************************************
 * GREATEST
 *******************************************************************************/

stlStatus dtlGreatest( stlUInt16        aInputArgumentCnt,
                       dtlValueEntry  * aInputArgument,
                       void           * aResultValue,
                       void           * aInfo,
                       dtlFuncVector  * aVectorFunc,
                       void           * aVectorArg,
                       void           * aEnv )
{
    return dtfGeneralComparison( aInputArgumentCnt,
                                 aInputArgument,
                                 aResultValue,
                                 DTL_COMPARISON_GREATER,
                                 aInfo,
                                 aVectorFunc,
                                 aVectorArg,
                                 aEnv );
}


/**
 * @brief Greatest function에 대한 정보 얻기
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
stlStatus dtlGetFuncInfoGreatest( stlUInt16               aDataTypeArrayCount,
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
    STL_TRY( dtlGetFuncInfoGeneralComparison( aDataTypeArrayCount,
                                              aIsConstantData,
                                              aDataTypeArray,
                                              aDataTypeDefInfoArray,
                                              aFuncArgDataTypeArrayCount,
                                              aFuncArgDataTypeArray,
                                              aFuncArgDataTypeDefInfoArray,
                                              aFuncResultDataType,
                                              aFuncResultDataTypeDefInfo,
                                              aFuncPtrIdx,
                                              aVectorFunc,
                                              aVectorArg,
                                              aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Greatest function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlGetFuncPtrGreatest( stlUInt32             aFuncPtrIdx,
                                 dtlBuiltInFuncPtr   * aFuncPtr,
                                 stlErrorStack       * aErrorStack )
{
    /**
     * output 설정
     */

    *aFuncPtr = dtlGreatest;

    return STL_SUCCESS;
}


/*******************************************************************************
 * LEAST
 *******************************************************************************/

stlStatus dtlLeast( stlUInt16        aInputArgumentCnt,
                    dtlValueEntry  * aInputArgument,
                    void           * aResultValue,
                    void           * aInfo,
                    dtlFuncVector  * aVectorFunc,
                    void           * aVectorArg,
                    void           * aEnv )
{
    return dtfGeneralComparison( aInputArgumentCnt,
                                 aInputArgument,
                                 aResultValue,
                                 DTL_COMPARISON_LESS,
                                 aInfo,
                                 aVectorFunc,
                                 aVectorArg,
                                 aEnv );
}


/**
 * @brief Least function에 대한 정보 얻기
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
stlStatus dtlGetFuncInfoLeast( stlUInt16               aDataTypeArrayCount,
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
    STL_TRY( dtlGetFuncInfoGeneralComparison( aDataTypeArrayCount,
                                              aIsConstantData,
                                              aDataTypeArray,
                                              aDataTypeDefInfoArray,
                                              aFuncArgDataTypeArrayCount,
                                              aFuncArgDataTypeArray,
                                              aFuncArgDataTypeDefInfoArray,
                                              aFuncResultDataType,
                                              aFuncResultDataTypeDefInfo,
                                              aFuncPtrIdx,
                                              aVectorFunc,
                                              aVectorArg,
                                              aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Least function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlGetFuncPtrLeast( stlUInt32             aFuncPtrIdx,
                              dtlBuiltInFuncPtr   * aFuncPtr,
                              stlErrorStack       * aErrorStack )
{
    /**
     * output 설정
     */

    *aFuncPtr = dtlLeast;

    return STL_SUCCESS;
}

/** @} */
