/*******************************************************************************
 * dtlFuncMul.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtlFuncMul.c 1841 2011-09-07 07:14:06Z jhkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file dtlFuncMul.c
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
const dtlFuncInfo dtlFuncMulInfo =
{
    { STL_TRUE, STL_TRUE, }      /* 함수를 결정하는데 판단이 되는 argument index list */
};


/*
 * 각 함수별 arg type, result type을 정의한다.
 * 주의: 빠르게 처리 가능한 함수를 먼저 정의해야 한다.
 */
const dtlFuncArgInfo dtlFuncMulList[] =
{
     { dtlBigIntMul,
       { DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NATIVE_BIGINT },
       DTL_TYPE_NATIVE_BIGINT
     },
     { dtlNumericMul,
       { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER },
       DTL_TYPE_NUMBER
     },
     { dtlDoubleMul,
       { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NATIVE_DOUBLE },
       DTL_TYPE_NATIVE_DOUBLE
     },
     { dtlIntervalYearToMonthMulBigInt,
       { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_NATIVE_BIGINT },
       DTL_TYPE_INTERVAL_YEAR_TO_MONTH
     },
     { dtlBigIntMulIntervalYearToMonth,
       { DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
       DTL_TYPE_INTERVAL_YEAR_TO_MONTH
     },
     { dtlIntervalYearToMonthMulNumeric,
       { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_NUMBER },
       DTL_TYPE_INTERVAL_YEAR_TO_MONTH
     },
     { dtlNumericMulIntervalYearToMonth,
       { DTL_TYPE_NUMBER, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
       DTL_TYPE_INTERVAL_YEAR_TO_MONTH
     },
     { dtlIntervalDayToSecondMulBigInt,
       { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_NATIVE_BIGINT },
       DTL_TYPE_INTERVAL_DAY_TO_SECOND
     },
     { dtlBigIntMulIntervalDayToSecond,
       { DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
       DTL_TYPE_INTERVAL_DAY_TO_SECOND
     },
     { dtlIntervalDayToSecondMulNumeric,
       { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_NUMBER },
       DTL_TYPE_INTERVAL_DAY_TO_SECOND
     },
     { dtlNumericMulIntervalDayToSecond,
       { DTL_TYPE_NUMBER, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
       DTL_TYPE_INTERVAL_DAY_TO_SECOND
     },
     
     { NULL,
       { DTL_TYPE_NA, },
       DTL_TYPE_NA
     }
};


stlStatus dtlBigIntMul( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aInfo,
                        dtlFuncVector  * aVectorFunc,
                        void           * aVectorArg,
                        void           * aEnv )
{
    return dtfBigIntMul( aInputArgumentCnt,
                         aInputArgument,
                         aResultValue,
                         aEnv );
}

stlStatus dtlNumericMul( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv )
{
    return dtfNumericMul( aInputArgumentCnt,
                          aInputArgument,
                          aResultValue,
                          aEnv );
}

stlStatus dtlDoubleMul( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aInfo,
                        dtlFuncVector  * aVectorFunc,
                        void           * aVectorArg,
                        void           * aEnv )
{
    return dtfDoubleMul( aInputArgumentCnt,
                         aInputArgument,
                         aResultValue,
                         aEnv );
}


stlStatus dtlIntervalYearToMonthMulBigInt( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv )
{
    return dtfIntervalYearToMonthMulBigInt( aInputArgumentCnt,
                                            aInputArgument,
                                            aResultValue,
                                            aEnv );
}

stlStatus dtlBigIntMulIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv )
{
    return dtfBigIntMulIntervalYearToMonth( aInputArgumentCnt,
                                            aInputArgument,
                                            aResultValue,
                                            aEnv );
}

stlStatus dtlIntervalYearToMonthMulNumeric( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv )
{
    return dtfIntervalYearToMonthMulNumeric( aInputArgumentCnt,
                                             aInputArgument,
                                             aResultValue,
                                             aEnv );
}

stlStatus dtlNumericMulIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv )
{
    return dtfNumericMulIntervalYearToMonth( aInputArgumentCnt,
                                             aInputArgument,
                                             aResultValue,
                                             aEnv );
}

stlStatus dtlIntervalDayToSecondMulBigInt( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv )
{
    return dtfIntervalDayToSecondMulBigInt( aInputArgumentCnt,
                                            aInputArgument,
                                            aResultValue,
                                            aEnv );
}

stlStatus dtlBigIntMulIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv )
{
    return dtfBigIntMulIntervalDayToSecond( aInputArgumentCnt,
                                            aInputArgument,
                                            aResultValue,
                                            aEnv );
}

stlStatus dtlIntervalDayToSecondMulNumeric( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv )
{
    return dtfIntervalDayToSecondMulNumeric( aInputArgumentCnt,
                                             aInputArgument,
                                             aResultValue,
                                             aEnv );
}

stlStatus dtlNumericMulIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv )
{
    return dtfNumericMulIntervalDayToSecond( aInputArgumentCnt,
                                             aInputArgument,
                                             aResultValue,
                                             aEnv );
}

/**
 * @brief Mul function에 대한 정보 얻기
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
stlStatus dtlGetFuncInfoMul( stlUInt16               aDataTypeArrayCount,
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
    STL_TRY( dtlGetFuncInfoCommon( &dtlFuncMulInfo,
                                   dtlFuncMulList,
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

    /* result data type이 interval 계열인 경우 indicator를
     * input data type들 중에서 interval 계열인 것을 찾아
     * 해당 indicator로 셋팅해준다. */
    if( (*aFuncResultDataType == DTL_TYPE_INTERVAL_YEAR_TO_MONTH) ||
        (*aFuncResultDataType == DTL_TYPE_INTERVAL_DAY_TO_SECOND) )
    {
        if( (aDataTypeArray[0] == DTL_TYPE_INTERVAL_YEAR_TO_MONTH) ||
            (aDataTypeArray[0] == DTL_TYPE_INTERVAL_DAY_TO_SECOND) )
        {
            aFuncResultDataTypeDefInfo->mIntervalIndicator
                = aFuncArgDataTypeDefInfoArray[0].mIntervalIndicator;
        }
        else
        {
            aFuncResultDataTypeDefInfo->mIntervalIndicator
                = aFuncArgDataTypeDefInfoArray[1].mIntervalIndicator;
        }
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
 * @brief Mul function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlGetFuncPtrMul( stlUInt32             aFuncPtrIdx,
                            dtlBuiltInFuncPtr   * aFuncPtr,
                            stlErrorStack       * aErrorStack )
{
    /**
     * output 설정
     */

    /*
     * dtlMul가 n차원 배열임으로 1차원 배열로 typecast해서
     * aFuncPtrIdx를 적용한다.
     */
    *aFuncPtr = dtlFuncMulList[aFuncPtrIdx].mFuncPtr;

    return STL_SUCCESS;
}



/** @} */
