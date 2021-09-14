/*******************************************************************************
 * dtlFuncReplace.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *
 ******************************************************************************/

/**
 * @file dtlFuncReplace.c
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
const dtlFuncInfo dtlFuncReplaceInfo =
{
    { STL_TRUE } /* 함수를 결정하는데 판단이 되는 argument cnt */
};


/*
 * 각 함수별 arg type, result type을 정의한다.
 * 주의: 빠르게 처리 가능한 함수를 먼저 정의해야 한다.
 */
const dtlFuncArgInfo dtlFuncReplaceList[] =
{
     { dtlReplace,
       { DTL_TYPE_VARCHAR, DTL_TYPE_VARCHAR, DTL_TYPE_VARCHAR },
       DTL_TYPE_VARCHAR
     },
     { dtlLongvarcharReplace,
       { DTL_TYPE_LONGVARCHAR, DTL_TYPE_LONGVARCHAR, DTL_TYPE_LONGVARCHAR },
       DTL_TYPE_LONGVARCHAR
     },

     { NULL,
       { DTL_TYPE_NA, },
       DTL_TYPE_NA
     }
};


stlStatus dtlReplace( stlUInt16        aInputArgumentCnt,
                      dtlValueEntry  * aInputArgument,
                      void           * aResultValue,
                      void           * aInfo,
                      dtlFuncVector  * aVectorFunc,
                      void           * aVectorArg,
                      void           * aEnv )
{
    return dtfReplace( aInputArgumentCnt,
                       aInputArgument,
                       aResultValue,
                       aVectorFunc,
                       aVectorArg,
                       aEnv );
}

stlStatus dtlLongvarcharReplace( stlUInt16        aInputArgumentCnt,
                                 dtlValueEntry  * aInputArgument,
                                 void           * aResultValue,
                                 void           * aInfo,
                                 dtlFuncVector  * aVectorFunc,
                                 void           * aVectorArg,
                                 void           * aEnv )
{
    return dtfLongvarcharReplace( aInputArgumentCnt,
                                  aInputArgument,
                                  aResultValue,
                                  aVectorFunc,
                                  aVectorArg,
                                  aEnv );
}

/**
 * @brief Replace function에 대한 정보 얻기
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
stlStatus dtlGetFuncInfoReplace( stlUInt16               aDataTypeArrayCount,
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
    STL_TRY( dtlGetFuncInfoCommon( &dtlFuncReplaceInfo,
                                   dtlFuncReplaceList,
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
     * REPLACE( str, from, to )
     *
     * REPLACE( 'HI GLIESE', 'HI', 'HELLO' ) => HELLO GLIESE
     * 
     * result type의 precision = ( str precision ) * ( to precision )
     * ( Why: from의 precision으로 최소 1이 올 수 있음을 가정하고 최대로 계산한다. 
     *        예) REPLACE( 'AAAAA', 'A', 'EFG' ) => EFGEFGEFGEFGEFG     )
     *        
     *  ex1) REPLACE( CAST( '가가가' AS VARCHAR( 3 CHAR ) ), '가', CAST( '가나다' AS VARCHAR( 3 CHAR ) ) )
     *       => '가나다가나다가나다'
     *       
     *  ex2) REPLACE( CAST( '가가가' AS VARCHAR( 3 CHAR ) ), '가', CAST( '12345' AS VARCHAR( 5 OCTETS ) ) )
     *       => '123451234512345'
     *       
     *  ex3) REPLACE( CAST( '11111' AS VARCHAR( 5 OCTETS ) ), '1', CAST( 'abc' AS VARCHAR( 3 CHAR ) ) )    
     *       => 'abcabcabcabcabc'
     *       REPLACE( CAST( '11111' AS VARCHAR( 5 OCTETS ) ), '1', CAST( '가나다' AS VARCHAR( 3 CHAR ) ) )
     *       => '가나다가나다가나다가나다가나다'
     *
     *  ex4) REPLACE( CAST( '11111' AS VARCHAR( 5 OCTETS ) ), '1', CAST( 'ABC' AS VARCHAR( 5 OCTETS ) ) )
     *       => 'ABCABCABCABCABC'
     */
    
    if( *aFuncResultDataType == DTL_TYPE_VARCHAR )
    {
        if( aDataTypeArrayCount == 2 )
        {
            aFuncResultDataTypeDefInfo->mArgNum1 = aFuncArgDataTypeDefInfoArray[0].mArgNum1;
        }
        else
        {
            aFuncResultDataTypeDefInfo->mArgNum1 =
                aFuncArgDataTypeDefInfoArray[0].mArgNum1 * aFuncArgDataTypeDefInfoArray[2].mArgNum1;
            
            if( ( aFuncArgDataTypeDefInfoArray[0].mStringLengthUnit
                  == DTL_STRING_LENGTH_UNIT_OCTETS ) &&
                ( aFuncArgDataTypeDefInfoArray[2].mStringLengthUnit
                  == DTL_STRING_LENGTH_UNIT_CHARACTERS ) )
            {
                /* 
                 * ex) REPLACE( cast( '11111' as varchar( 5 octets ) ), '1', '가나다' )
                 *     => 가나다가나다가나다가나다가나다
                 */
                aFuncResultDataTypeDefInfo->mArgNum1 =
                    DTL_CHAR_STRING_COLUMN_SIZE( aFuncResultDataTypeDefInfo->mArgNum1,
                                                 aVectorFunc,
                                                 aVectorArg );
            }
            else
            {
                /* Do Nothing */
            }
        }
        
        if( aFuncResultDataTypeDefInfo->mArgNum1 > DTL_VARCHAR_MAX_PRECISION )
        {
            aFuncResultDataTypeDefInfo->mArgNum1 = DTL_VARCHAR_MAX_PRECISION;
        }
        else
        {
            /* Do Nothing */
        }
        
        aFuncResultDataTypeDefInfo->mArgNum2 =
            aFuncArgDataTypeDefInfoArray[0].mArgNum2;
        aFuncResultDataTypeDefInfo->mStringLengthUnit =
            aFuncArgDataTypeDefInfoArray[0].mStringLengthUnit;
        aFuncResultDataTypeDefInfo->mIntervalIndicator =
            aFuncArgDataTypeDefInfoArray[0].mIntervalIndicator;
    }
    else
    {
        /* LONG VARCHAR */
        /* Do Nothing */
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Replace function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlGetFuncPtrReplace( stlUInt32             aFuncPtrIdx,
                                dtlBuiltInFuncPtr   * aFuncPtr,
                                stlErrorStack       * aErrorStack )
{
    /**
     * output 설정
     */

    /*
     * dtlReplace가 n차원 배열임으로 1차원 배열로 typecast해서
     * aFuncPtrIdx를 적용한다.
     */
    *aFuncPtr = dtlFuncReplaceList[aFuncPtrIdx].mFuncPtr;

    return STL_SUCCESS;
}



/** @} */
