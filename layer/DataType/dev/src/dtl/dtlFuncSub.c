/*******************************************************************************
 * dtlFuncSub.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtlFuncSub.c 1841 2011-09-07 07:14:06Z jhkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file dtlFuncSub.c
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
const dtlFuncInfo dtlFuncSubInfo =
{
    { STL_TRUE, STL_TRUE, } /* 함수를 결정하는데 판단이 되는 argument cnt */
};


/*
 * 각 함수별 arg type, result type을 정의한다.
 * 주의: 빠르게 처리 가능한 함수를 먼저 정의해야 한다.
 */
const dtlFuncArgInfo dtlFuncSubList[] =
{
    { dtlBigIntSub,
      { DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NATIVE_BIGINT },
      DTL_TYPE_NATIVE_BIGINT
    },
    { dtlNumericSub,
      { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER },
      DTL_TYPE_NUMBER
    },
    { dtlDoubleSub,
      { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NATIVE_DOUBLE },
      DTL_TYPE_NATIVE_DOUBLE
    },
    
    
    { dtlDateSubDate,
      { DTL_TYPE_DATE, DTL_TYPE_DATE },
      DTL_TYPE_NUMBER,
    },
    { dtlDateSubInteger,
      { DTL_TYPE_DATE, DTL_TYPE_NATIVE_INTEGER },
      DTL_TYPE_DATE,
    },
    { dtlDateSubNumeric,
      { DTL_TYPE_DATE, DTL_TYPE_NUMBER },
      DTL_TYPE_DATE,
    },
    { dtlDateSubIntervalYearToMonth,
      { DTL_TYPE_DATE, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
      DTL_TYPE_DATE,
    },
    { dtlDateSubIntervalDay,
      { DTL_TYPE_DATE, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
      DTL_TYPE_DATE,
    },    
    { dtlDateSubIntervalDayToSecond,
      { DTL_TYPE_DATE, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
      DTL_TYPE_TIMESTAMP,
    },

    
    { dtlTimeSubTime,
      { DTL_TYPE_TIME, DTL_TYPE_TIME },
      DTL_TYPE_INTERVAL_DAY_TO_SECOND,
    },
    { dtlTimeSubInteger,
      { DTL_TYPE_TIME, DTL_TYPE_NATIVE_INTEGER },
      DTL_TYPE_TIME,
    },
    { dtlTimeSubNumeric,
      { DTL_TYPE_TIME, DTL_TYPE_NUMBER },
      DTL_TYPE_TIME,
    },    
    { dtlTimeSubIntervalYearToMonth,
      { DTL_TYPE_TIME, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
      DTL_TYPE_TIME,
    },
    { dtlTimeSubIntervalDayToSecond,
      { DTL_TYPE_TIME, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
      DTL_TYPE_TIME,
    },


    { dtlTimeTzSubInteger,
      { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_NATIVE_INTEGER },
      DTL_TYPE_TIME_WITH_TIME_ZONE,
    },
    { dtlTimeTzSubNumeric,
      { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_NUMBER },
      DTL_TYPE_TIME_WITH_TIME_ZONE,
    },    
    { dtlTimeTzSubIntervalYearToMonth,
      { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
      DTL_TYPE_TIME_WITH_TIME_ZONE,
    },
    { dtlTimeTzSubIntervalDayToSecond,
      { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
      DTL_TYPE_TIME_WITH_TIME_ZONE,
    },

    
    { dtlTimestampSubTimestamp,
      { DTL_TYPE_TIMESTAMP, DTL_TYPE_TIMESTAMP },
      DTL_TYPE_INTERVAL_DAY_TO_SECOND,
    },   
    { dtlTimestampSubInteger,
      { DTL_TYPE_TIMESTAMP, DTL_TYPE_NATIVE_INTEGER },
      DTL_TYPE_TIMESTAMP,
    },
    { dtlTimestampSubNumeric,
      { DTL_TYPE_TIMESTAMP, DTL_TYPE_NUMBER },
      DTL_TYPE_TIMESTAMP,
    },
    { dtlTimestampSubIntervalYearToMonth,
      { DTL_TYPE_TIMESTAMP, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
      DTL_TYPE_TIMESTAMP,
    },
    { dtlTimestampSubIntervalDayToSecond,
      { DTL_TYPE_TIMESTAMP, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
      DTL_TYPE_TIMESTAMP,
    },

    { dtlTimestampTzSubTimestampTz,
      { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE },
      DTL_TYPE_INTERVAL_DAY_TO_SECOND,
    },       
    { dtlTimestampTzSubInteger,
      { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_NATIVE_INTEGER },
      DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,
    },
    { dtlTimestampTzSubNumeric,
      { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_NUMBER },
      DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,
    },
    { dtlTimestampTzSubIntervalYearToMonth,
      { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
      DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,
    },
    { dtlTimestampTzSubIntervalDayToSecond,
      { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
      DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,
    },

    
    { dtlIntervalYearToMonthSubIntervalYearToMonth,
      { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
      DTL_TYPE_INTERVAL_YEAR_TO_MONTH,
    },
    { dtlIntervalDayToSecondSubIntervalDayToSecond,
      { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
      DTL_TYPE_INTERVAL_DAY_TO_SECOND,
    },
     
    { NULL,
      { DTL_TYPE_NA, },
      DTL_TYPE_NA
    }
};


stlStatus dtlBigIntSub( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aInfo,
                        dtlFuncVector  * aVectorFunc,
                        void           * aVectorArg,
                        void           * aEnv )
{
    return dtfBigIntSub( aInputArgumentCnt,
                         aInputArgument,
                         aResultValue,
                         aEnv );
}

stlStatus dtlDoubleSub( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aInfo,
                        dtlFuncVector  * aVectorFunc,
                        void           * aVectorArg,
                        void           * aEnv )
{
    return dtfDoubleSub( aInputArgumentCnt,
                         aInputArgument,
                         aResultValue,
                         aEnv );
}

stlStatus dtlNumericSub( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv )
{
    return dtfNumericSub( aInputArgumentCnt,
                          aInputArgument,
                          aResultValue,
                          aEnv );
}

stlStatus dtlDateSubDate( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv )
{
    return dtfDateSubDate( aInputArgumentCnt,
                           aInputArgument,
                           aResultValue,
                           aEnv );
}

stlStatus dtlDateSubInteger( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aInfo,
                             dtlFuncVector  * aVectorFunc,
                             void           * aVectorArg,
                             void           * aEnv )
{
    return dtfDateSubInteger( aInputArgumentCnt,
                              aInputArgument,
                              aResultValue,
                              aEnv );
}

stlStatus dtlDateSubNumeric( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aInfo,
                             dtlFuncVector  * aVectorFunc,
                             void           * aVectorArg,
                             void           * aEnv )
{
    return dtfDateSubNumeric( aInputArgumentCnt,
                              aInputArgument,
                              aResultValue,
                              aEnv );
}

stlStatus dtlDateSubIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv )
{
    return dtfDateSubIntervalYearToMonth( aInputArgumentCnt,
                                          aInputArgument,
                                          aResultValue,
                                          aEnv );
}

stlStatus dtlDateSubIntervalDay( stlUInt16        aInputArgumentCnt,
                                 dtlValueEntry  * aInputArgument,
                                 void           * aResultValue,
                                 void           * aInfo,
                                 dtlFuncVector  * aVectorFunc,
                                 void           * aVectorArg,
                                 void           * aEnv )
{
    return dtfDateSubIntervalDay( aInputArgumentCnt,
                                  aInputArgument,
                                  aResultValue,
                                  aVectorFunc,
                                  aVectorArg,
                                  aEnv );
}

stlStatus dtlDateSubIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv )
{
    return dtfDateSubIntervalDayToSecond( aInputArgumentCnt,
                                          aInputArgument,
                                          aResultValue,
                                          aVectorFunc,
                                          aVectorArg,
                                          aEnv );
}

stlStatus dtlTimeSubTime( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv )
{
    return dtfTimeSubTime( aInputArgumentCnt,
                           aInputArgument,
                           aResultValue,
                           aEnv );
}

stlStatus dtlTimeSubInteger( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aInfo,
                             dtlFuncVector  * aVectorFunc,
                             void           * aVectorArg,
                             void           * aEnv )
{
    return dtfTimeSubInteger( aInputArgumentCnt,
                              aInputArgument,
                              aResultValue,
                              aEnv );
}

stlStatus dtlTimeSubNumeric( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aInfo,
                             dtlFuncVector  * aVectorFunc,
                             void           * aVectorArg,
                             void           * aEnv )
{
    return dtfTimeSubNumeric( aInputArgumentCnt,
                              aInputArgument,
                              aResultValue,
                              aEnv );
}

stlStatus dtlTimeSubIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv )
{
    return dtfTimeSubIntervalYearToMonth( aInputArgumentCnt,
                                          aInputArgument,
                                          aResultValue,
                                          aEnv );
}

stlStatus dtlTimeSubIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv )
{
    return dtfTimeSubIntervalDayToSecond( aInputArgumentCnt,
                                          aInputArgument,
                                          aResultValue,
                                          aEnv );
}

stlStatus dtlTimeTzSubInteger( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aInfo,
                               dtlFuncVector  * aVectorFunc,
                               void           * aVectorArg,
                               void           * aEnv )
{
    return dtfTimeTzSubInteger( aInputArgumentCnt,
                                aInputArgument,
                                aResultValue,
                                aEnv );
}

stlStatus dtlTimeTzSubNumeric( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aInfo,
                               dtlFuncVector  * aVectorFunc,
                               void           * aVectorArg,
                               void           * aEnv )
{
    return dtfTimeTzSubNumeric( aInputArgumentCnt,
                                aInputArgument,
                                aResultValue,
                                aEnv );
}

stlStatus dtlTimeTzSubIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv )
{
    return dtfTimeTzSubIntervalYearToMonth( aInputArgumentCnt,
                                            aInputArgument,
                                            aResultValue,
                                            aEnv );
}

stlStatus dtlTimeTzSubIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv )
{
    return dtfTimeTzSubIntervalDayToSecond( aInputArgumentCnt,
                                            aInputArgument,
                                            aResultValue,
                                            aEnv );
}

stlStatus dtlTimestampSubTimestamp( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    void           * aInfo,
                                    dtlFuncVector  * aVectorFunc,
                                    void           * aVectorArg,
                                    void           * aEnv )
{
    return dtfTimestampSubTimestamp( aInputArgumentCnt,
                                     aInputArgument,
                                     aResultValue,
                                     aEnv );
}

stlStatus dtlTimestampSubInteger( stlUInt16        aInputArgumentCnt,
                                  dtlValueEntry  * aInputArgument,
                                  void           * aResultValue,
                                  void           * aInfo,
                                  dtlFuncVector  * aVectorFunc,
                                  void           * aVectorArg,
                                  void           * aEnv )
{
    return dtfTimestampSubInteger( aInputArgumentCnt,
                                   aInputArgument,
                                   aResultValue,
                                   aEnv );
}

stlStatus dtlTimestampSubNumeric( stlUInt16        aInputArgumentCnt,
                                  dtlValueEntry  * aInputArgument,
                                  void           * aResultValue,
                                  void           * aInfo,
                                  dtlFuncVector  * aVectorFunc,
                                  void           * aVectorArg,
                                  void           * aEnv )
{
    return dtfTimestampSubNumeric( aInputArgumentCnt,
                                   aInputArgument,
                                   aResultValue,
                                   aEnv );
}

stlStatus dtlTimestampSubIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv )
{
    return dtfTimestampSubIntervalYearToMonth( aInputArgumentCnt,
                                               aInputArgument,
                                               aResultValue,
                                               aEnv );
}

stlStatus dtlTimestampSubIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv )
{
    return dtfTimestampSubIntervalDayToSecond( aInputArgumentCnt,
                                               aInputArgument,
                                               aResultValue,
                                               aEnv );
}

stlStatus dtlTimestampTzSubTimestampTz( stlUInt16        aInputArgumentCnt,
                                        dtlValueEntry  * aInputArgument,
                                        void           * aResultValue,
                                        void           * aInfo,
                                        dtlFuncVector  * aVectorFunc,
                                        void           * aVectorArg,
                                        void           * aEnv )
{
    return dtfTimestampTzSubTimestampTz( aInputArgumentCnt,
                                         aInputArgument,
                                         aResultValue,
                                         aEnv );
}

stlStatus dtlTimestampTzSubInteger( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    void           * aInfo,
                                    dtlFuncVector  * aVectorFunc,
                                    void           * aVectorArg,
                                    void           * aEnv )
{
    return dtfTimestampTzSubInteger( aInputArgumentCnt,
                                     aInputArgument,
                                     aResultValue,
                                     aEnv );
}

stlStatus dtlTimestampTzSubNumeric( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    void           * aInfo,
                                    dtlFuncVector  * aVectorFunc,
                                    void           * aVectorArg,
                                    void           * aEnv )
{
    return dtfTimestampTzSubNumeric( aInputArgumentCnt,
                                     aInputArgument,
                                     aResultValue,
                                     aEnv );
}

stlStatus dtlTimestampTzSubIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv )
{
    return dtfTimestampTzSubIntervalYearToMonth( aInputArgumentCnt,
                                                 aInputArgument,
                                                 aResultValue,
                                                 aEnv );
}

stlStatus dtlTimestampTzSubIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv )
{
    return dtfTimestampTzSubIntervalDayToSecond( aInputArgumentCnt,
                                                 aInputArgument,
                                                 aResultValue,
                                                 aEnv );
}

stlStatus dtlIntervalYearToMonthSubIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                                        dtlValueEntry  * aInputArgument,
                                                        void           * aResultValue,
                                                        void           * aInfo,
                                                        dtlFuncVector  * aVectorFunc,
                                                        void           * aVectorArg,
                                                        void           * aEnv )
{
    return dtfIntervalYearToMonthSubIntervalYearToMonth( aInputArgumentCnt,
                                                         aInputArgument,
                                                         aResultValue,
                                                         aEnv );
}

stlStatus dtlIntervalDayToSecondSubIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                                        dtlValueEntry  * aInputArgument,
                                                        void           * aResultValue,
                                                        void           * aInfo,
                                                        dtlFuncVector  * aVectorFunc,
                                                        void           * aVectorArg,
                                                        void           * aEnv )
{
    return dtfIntervalDayToSecondSubIntervalDayToSecond( aInputArgumentCnt,
                                                         aInputArgument,
                                                         aResultValue,
                                                         aEnv );
}



/**
 * @brief Sub function에 대한 정보 얻기
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
stlStatus dtlGetFuncInfoSub( stlUInt16               aDataTypeArrayCount,
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
    STL_TRY( dtlGetFuncInfoCommon( &dtlFuncSubInfo,
                                   dtlFuncSubList,
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
            
            *aFuncResultDataType = dtlFuncSubList[*aFuncPtrIdx].mResultType;
            
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
        /*
         * function의 result type이 interval type인 경우,
         * interval Indicator를 계산해서 지정한다.
         */
        
        if( ( (*aFuncResultDataType == DTL_TYPE_INTERVAL_YEAR_TO_MONTH) &&
              (aFuncArgDataTypeArray[0] == DTL_TYPE_INTERVAL_YEAR_TO_MONTH) &&
              (aFuncArgDataTypeArray[1] == DTL_TYPE_INTERVAL_YEAR_TO_MONTH) )
            ||
            ( (*aFuncResultDataType == DTL_TYPE_INTERVAL_DAY_TO_SECOND) &&
              (aFuncArgDataTypeArray[0] == DTL_TYPE_INTERVAL_DAY_TO_SECOND) &&
              (aFuncArgDataTypeArray[1] == DTL_TYPE_INTERVAL_DAY_TO_SECOND) ) )
        {
            aFuncResultDataTypeDefInfo->mIntervalIndicator =
                dtlIntervalIndicatorContainArgIndicator
                [aFuncArgDataTypeDefInfoArray[0].mIntervalIndicator]
                [aFuncArgDataTypeDefInfoArray[1].mIntervalIndicator];
            
            STL_DASSERT( ( (*aFuncResultDataType == DTL_TYPE_INTERVAL_YEAR_TO_MONTH) &&
                           ((aFuncResultDataTypeDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_YEAR) ||
                            (aFuncResultDataTypeDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_MONTH) ||
                            (aFuncResultDataTypeDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH)) )
                         ||
                         ( (*aFuncResultDataType == DTL_TYPE_INTERVAL_DAY_TO_SECOND) &&
                           ((aFuncResultDataTypeDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY) ||
                            (aFuncResultDataTypeDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR) ||
                            (aFuncResultDataTypeDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE) ||
                            (aFuncResultDataTypeDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND) ||
                            (aFuncResultDataTypeDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_HOUR) ||
                            (aFuncResultDataTypeDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE) ||
                            (aFuncResultDataTypeDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_SECOND) ||
                            (aFuncResultDataTypeDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE) ||
                            (aFuncResultDataTypeDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND) ||
                            (aFuncResultDataTypeDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND)) ) );
        }
        else
        {
            /* Do Nothing */
        }
    }

    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Sub function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlGetFuncPtrSub( stlUInt32             aFuncPtrIdx,
                            dtlBuiltInFuncPtr   * aFuncPtr,
                            stlErrorStack       * aErrorStack )
{
    /**
     * output 설정
     */

    /*
     * dtlSub가 n차원 배열임으로 1차원 배열로 typecast해서
     * aFuncPtrIdx를 적용한다.
     */
    *aFuncPtr = dtlFuncSubList[aFuncPtrIdx].mFuncPtr;

    return STL_SUCCESS;
}



/** @} */
