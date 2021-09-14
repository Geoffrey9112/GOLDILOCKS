/*******************************************************************************
 * dtlFuncBitwiseNot.c
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
 * @file dtlFuncBitwiseNot.c
 * @brief DataType Layer Function관련 함수 
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtlFuncCommon.h>
#include <dtfBitwise.h>

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
const dtlFuncInfo dtlFuncBitwiseNotInfo =
{
    { STL_TRUE, }      /* 함수를 결정하는데 판단이 되는 argument index list */
};


/*
 * 각 함수별 arg type, result type을 정의한다.
 * 주의: 빠르게 처리 가능한 함수를 먼저 정의해야 한다.
 */
const dtlFuncArgInfo dtlFuncBitwiseNotList[] =
{
    { dtlBitwiseNot,
      { DTL_TYPE_NATIVE_BIGINT },
      DTL_TYPE_NATIVE_BIGINT
    },

    { dtlBitwiseNot,
      { DTL_TYPE_NUMBER },
      DTL_TYPE_NATIVE_BIGINT
    },

    { dtlBitwiseNot,
      { DTL_TYPE_NATIVE_DOUBLE },
      DTL_TYPE_NATIVE_BIGINT
    },
    

    { NULL,
      { DTL_TYPE_NA, },
      DTL_TYPE_NA
    }
};


/**
 * @brief bitwise Not function에 대한 정보 얻기
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
stlStatus dtlGetFuncInfoBitwiseNot( stlUInt16               aDataTypeArrayCount,
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
    
    STL_TRY( dtlGetFuncInfoCommon( &dtlFuncBitwiseNotInfo,
                                   dtlFuncBitwiseNotList,
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
     * function의 인자가 NATIVE_DOUBLE로 판단된 경우, NUMBER로 변경한다.
     */
    
    if( aFuncArgDataTypeArray[0] == DTL_TYPE_NATIVE_DOUBLE )
    {
        aFuncArgDataTypeArray[0] = DTL_TYPE_NUMBER;
        DTL_COPY_DATA_TYPE_DEF_INFO( & aFuncArgDataTypeDefInfoArray[0],
                                     & gResultDataTypeDef[ aFuncArgDataTypeArray[0] ] );         
    }
    else
    {
        /*
         * 인자로 number, binary integer 숫자타입이 오는 경우,
         * 함수인자의 타입은 INPUT 타입으로 지정한다.
         *
         * 또, native integer 계열의 타입은 함수인자의 타입과 결과타입은 동일하다.
         */
        
        sGroup = dtlDataTypeGroup[ aDataTypeArray[0] ];
        
        if( ( sGroup == DTL_GROUP_BINARY_INTEGER ) ||
            ( sGroup == DTL_GROUP_NUMBER ) )
        {
            aFuncArgDataTypeArray[0] = aDataTypeArray[0];
            DTL_COPY_DATA_TYPE_DEF_INFO( & aFuncArgDataTypeDefInfoArray[0],
                                         & aDataTypeDefInfoArray[0] );        
            
            if( sGroup == DTL_GROUP_BINARY_INTEGER )
            {
                *aFuncResultDataType = aDataTypeArray[0];
                DTL_COPY_DATA_TYPE_DEF_INFO( aFuncResultDataTypeDefInfo,
                                             & aDataTypeDefInfoArray[0] );
            }
            else
            {
                /* Do Nothing */
            }
        }
        else
        {
            /* Do Nothing */
        }
    }
    
    STL_DASSERT( gDtfBitwiseNotFunc[ aFuncArgDataTypeArray[0] ] != NULL );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief bitwise Not function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlGetFuncPtrBitwiseNot( stlUInt32             aFuncPtrIdx,
                                   dtlBuiltInFuncPtr   * aFuncPtr,
                                   stlErrorStack       * aErrorStack )
{
    /**
     * output 설정
     */

    *aFuncPtr = dtlBitwiseNot;

    return STL_SUCCESS;
}


stlStatus dtlBitwiseNot( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv )
{
    STL_DASSERT( gDtfBitwiseNotFunc[ aInputArgument[0].mValue.mDataValue->mType ] != NULL );
    
    STL_TRY( gDtfBitwiseNotFunc[ aInputArgument[0].mValue.mDataValue->mType ](
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
