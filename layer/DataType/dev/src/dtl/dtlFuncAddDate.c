/*******************************************************************************
 * dtlFuncAddDate.c
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
 * @file dtlFuncAddDate.c
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
const dtlFuncInfo dtlFuncAddDateInfo =
{
    { STL_TRUE, STL_TRUE, }      /* 함수를 결정하는데 판단이 되는 argument index list */
};


/*
 * 각 함수별 arg type, result type을 정의한다.
 * 주의: 빠르게 처리 가능한 함수를 먼저 정의해야 한다.
 */
const dtlFuncArgInfo dtlFuncAddDateList[] =
{
    { dtlDateAddDateInteger,
      { DTL_TYPE_DATE, DTL_TYPE_NATIVE_INTEGER },
      DTL_TYPE_DATE
    },
    { dtlDateAddDateNumeric,
      { DTL_TYPE_DATE, DTL_TYPE_NUMBER },
      DTL_TYPE_DATE
    },
    { dtlDateAddDateIntervalYearToMonth,
      { DTL_TYPE_DATE, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
      DTL_TYPE_DATE
    },
    { dtlDateAddDateIntervalDay,
      { DTL_TYPE_DATE, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
      DTL_TYPE_DATE
    },
    { dtlDateAddDateIntervalDayToSecond,
      { DTL_TYPE_DATE, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
      DTL_TYPE_TIMESTAMP
    },

    
    { dtlTimestampAddDateInteger,
      { DTL_TYPE_TIMESTAMP, DTL_TYPE_NATIVE_INTEGER },
      DTL_TYPE_TIMESTAMP
    },
    { dtlTimestampAddDateNumeric,
      { DTL_TYPE_TIMESTAMP, DTL_TYPE_NUMBER },
      DTL_TYPE_TIMESTAMP
    },
    { dtlTimestampAddDateIntervalYearToMonth,
      { DTL_TYPE_TIMESTAMP, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
      DTL_TYPE_TIMESTAMP
    },
    { dtlTimestampAddDateIntervalDayToSecond,
      { DTL_TYPE_TIMESTAMP, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
      DTL_TYPE_TIMESTAMP
    },

    
    { dtlTimestampTzAddDateInteger,
      { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_NATIVE_INTEGER },
      DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE
    },
    { dtlTimestampTzAddDateNumeric,
      { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_NUMBER },
      DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE
    },
    { dtlTimestampTzAddDateIntervalYearToMonth,
      { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
      DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE
    },
    { dtlTimestampTzAddDateIntervalDayToSecond,
      { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
      DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE
    },        

    
    { NULL,
      { DTL_TYPE_NA, },
      DTL_TYPE_NA
    }
};


stlStatus dtlDateAddDateInteger( stlUInt16        aInputArgumentCnt,
                                 dtlValueEntry  * aInputArgument,
                                 void           * aResultValue,
                                 void           * aInfo,
                                 dtlFuncVector  * aVectorFunc,
                                 void           * aVectorArg,
                                 void           * aEnv )
{
    return dtfDateAddInteger( aInputArgumentCnt,
                              aInputArgument,
                              aResultValue,
                              aEnv );
}

stlStatus dtlDateAddDateNumeric( stlUInt16        aInputArgumentCnt,
                                 dtlValueEntry  * aInputArgument,
                                 void           * aResultValue,
                                 void           * aInfo,
                                 dtlFuncVector  * aVectorFunc,
                                 void           * aVectorArg,
                                 void           * aEnv )
{
    return dtfDateAddNumeric( aInputArgumentCnt,
                              aInputArgument,
                              aResultValue,
                              aEnv );
}

stlStatus dtlDateAddDateIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv )
{
    return dtfDateAddIntervalYearToMonth( aInputArgumentCnt,
                                          aInputArgument,
                                          aResultValue,
                                          aEnv );
}

stlStatus dtlDateAddDateIntervalDay( stlUInt16        aInputArgumentCnt,
                                     dtlValueEntry  * aInputArgument,
                                     void           * aResultValue,
                                     void           * aInfo,
                                     dtlFuncVector  * aVectorFunc,
                                     void           * aVectorArg,
                                     void           * aEnv )
{
    return dtfDateAddIntervalDay( aInputArgumentCnt,
                                  aInputArgument,
                                  aResultValue,
                                  aVectorFunc,
                                  aVectorArg,
                                  aEnv );
}

stlStatus dtlDateAddDateIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv )
{
    return dtfDateAddIntervalDayToSecond( aInputArgumentCnt,
                                          aInputArgument,
                                          aResultValue,
                                          aVectorFunc,
                                          aVectorArg,
                                          aEnv );
}

stlStatus dtlTimestampAddDateInteger( stlUInt16        aInputArgumentCnt,
                                      dtlValueEntry  * aInputArgument,
                                      void           * aResultValue,
                                      void           * aInfo,
                                      dtlFuncVector  * aVectorFunc,
                                      void           * aVectorArg,
                                      void           * aEnv )
{
    return dtfTimestampAddInteger( aInputArgumentCnt,
                                   aInputArgument,
                                   aResultValue,
                                   aEnv );
}

stlStatus dtlTimestampAddDateNumeric( stlUInt16        aInputArgumentCnt,
                                      dtlValueEntry  * aInputArgument,
                                      void           * aResultValue,
                                      void           * aInfo,
                                      dtlFuncVector  * aVectorFunc,
                                      void           * aVectorArg,
                                      void           * aEnv )
{
    return dtfTimestampAddNumeric( aInputArgumentCnt,
                                   aInputArgument,
                                   aResultValue,
                                   aEnv );
}

stlStatus dtlTimestampAddDateIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv )
{
    return dtfTimestampAddIntervalYearToMonth( aInputArgumentCnt,
                                               aInputArgument,
                                               aResultValue,
                                               aEnv );
}

stlStatus dtlTimestampAddDateIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv )
{
    return dtfTimestampAddIntervalDayToSecond( aInputArgumentCnt,
                                               aInputArgument,
                                               aResultValue,
                                               aEnv );
}

stlStatus dtlTimestampTzAddDateInteger( stlUInt16        aInputArgumentCnt,
                                        dtlValueEntry  * aInputArgument,
                                        void           * aResultValue,
                                        void           * aInfo,
                                        dtlFuncVector  * aVectorFunc,
                                        void           * aVectorArg,
                                        void           * aEnv )
{
    return dtfTimestampTzAddInteger( aInputArgumentCnt,
                                     aInputArgument,
                                     aResultValue,
                                     aEnv );
}

stlStatus dtlTimestampTzAddDateNumeric( stlUInt16        aInputArgumentCnt,
                                        dtlValueEntry  * aInputArgument,
                                        void           * aResultValue,
                                        void           * aInfo,
                                        dtlFuncVector  * aVectorFunc,
                                        void           * aVectorArg,
                                        void           * aEnv )
{
    return dtfTimestampTzAddNumeric( aInputArgumentCnt,
                                     aInputArgument,
                                     aResultValue,
                                     aEnv );
}


stlStatus dtlTimestampTzAddDateIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                                    dtlValueEntry  * aInputArgument,
                                                    void           * aResultValue,
                                                    void           * aInfo,
                                                    dtlFuncVector  * aVectorFunc,
                                                    void           * aVectorArg,
                                                    void           * aEnv )
{
    return dtfTimestampTzAddIntervalYearToMonth( aInputArgumentCnt,
                                                 aInputArgument,
                                                 aResultValue,
                                                 aEnv );
}

stlStatus dtlTimestampTzAddDateIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                                    dtlValueEntry  * aInputArgument,
                                                    void           * aResultValue,
                                                    void           * aInfo,
                                                    dtlFuncVector  * aVectorFunc,
                                                    void           * aVectorArg,
                                                    void           * aEnv )
{
    return dtfTimestampTzAddIntervalDayToSecond( aInputArgumentCnt,
                                                 aInputArgument,
                                                 aResultValue,
                                                 aEnv );
}

/**
 * @brief adddate function에 대한 정보 얻기
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
stlStatus dtlGetFuncInfoAddDate( stlUInt16               aDataTypeArrayCount,
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
    STL_TRY( dtlGetFuncInfoCommon( &dtlFuncAddDateInfo,
                                   dtlFuncAddDateList,
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

    if( ( aFuncArgDataTypeArray[0] == DTL_TYPE_DATE ) &&
        ( aFuncArgDataTypeArray[1] == DTL_TYPE_INTERVAL_DAY_TO_SECOND ) )
    {
        if( aDataTypeDefInfoArray[1].mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY )
        {
            aFuncArgDataTypeDefInfoArray[1].mIntervalIndicator = aDataTypeDefInfoArray[1].mIntervalIndicator;
        }
        else
        {
            STL_DASSERT(
                (aDataTypeDefInfoArray[1].mIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR) ||
                (aDataTypeDefInfoArray[1].mIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE) ||
                (aDataTypeDefInfoArray[1].mIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND) ||
                (aDataTypeDefInfoArray[1].mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_HOUR) ||
                (aDataTypeDefInfoArray[1].mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE) ||
                (aDataTypeDefInfoArray[1].mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_SECOND) ||
                (aDataTypeDefInfoArray[1].mIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE) ||
                (aDataTypeDefInfoArray[1].mIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND) ||
                (aDataTypeDefInfoArray[1].mIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND) );
            
            (*aFuncPtrIdx)++;   
            
            *aFuncResultDataType = dtlFuncAddDateList[*aFuncPtrIdx].mResultType;
            
            aFuncResultDataTypeDefInfo->mArgNum1 =
                gResultDataTypeDef[*aFuncResultDataType].mArgNum1;
            aFuncResultDataTypeDefInfo->mArgNum2 =
                gResultDataTypeDef[*aFuncResultDataType].mArgNum2;
            aFuncResultDataTypeDefInfo->mStringLengthUnit =
                gResultDataTypeDef[*aFuncResultDataType].mStringLengthUnit;
            aFuncResultDataTypeDefInfo->mIntervalIndicator =
                aDataTypeDefInfoArray[1].mIntervalIndicator;
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
 * @brief adddate function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlGetFuncPtrAddDate( stlUInt32             aFuncPtrIdx,
                                dtlBuiltInFuncPtr   * aFuncPtr,
                                stlErrorStack       * aErrorStack )
{
    /**
     * output 설정
     */

    /*
     * dtlAddDate가 n차원 배열임으로 1차원 배열로 typecast해서
     * aFuncPtrIdx를 적용한다.
     */
    *aFuncPtr = dtlFuncAddDateList[aFuncPtrIdx].mFuncPtr;

    return STL_SUCCESS;
}



/** @} */
