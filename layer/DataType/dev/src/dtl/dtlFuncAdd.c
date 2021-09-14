/*******************************************************************************
 * dtlFuncAdd.c
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
 * @file dtlFuncAdd.c
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
const dtlFuncInfo dtlFuncAddInfo =
{
    { STL_TRUE, STL_TRUE, }      /* 함수를 결정하는데 판단이 되는 argument index list */
};


/*
 * 각 함수별 arg type, result type을 정의한다.
 * 주의: 빠르게 처리 가능한 함수를 먼저 정의해야 한다.
 */
const dtlFuncArgInfo dtlFuncAddList[] =
{
    /* NATIVE_BIGINT + NATIVE_BIGINT */
    { dtlBigIntAdd,
      { DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NATIVE_BIGINT },
      DTL_TYPE_NATIVE_BIGINT
    },
    /* NUMERIC + NUMERIC */
    { dtlNumericAdd,
      { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER },
      DTL_TYPE_NUMBER
    },
    /* NATIVE_DOUBLE + NATIVE_DOUBLE */
    { dtlDoubleAdd,
      { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NATIVE_DOUBLE },
      DTL_TYPE_NATIVE_DOUBLE
    },

    
    /* DATE + NATIVE_INTEGER */
    { dtlDateAddInteger,
      { DTL_TYPE_DATE, DTL_TYPE_NATIVE_INTEGER },
      DTL_TYPE_DATE,
    },
    /* NATIVE_INTEGER + DATE */
    { dtlIntegerAddDate,
      { DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_DATE },
      DTL_TYPE_DATE,
    },

    
    /* DATE + NUMERIC */
    { dtlDateAddNumeric,
      { DTL_TYPE_DATE, DTL_TYPE_NUMBER },
      DTL_TYPE_DATE,
    },
    /* NUMERIC + DATE */
    { dtlNumericAddDate,
      { DTL_TYPE_NUMBER, DTL_TYPE_DATE },
      DTL_TYPE_DATE,
    },

    
    /* DATE + INTERVAL YEAR TO MONTH */
    { dtlDateAddIntervalYearToMonth,
      { DTL_TYPE_DATE, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
      DTL_TYPE_DATE
    },
    /* DATE + INTERVAL DAY TO SECOND */
    { dtlDateAddIntervalDay,
      { DTL_TYPE_DATE, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
      DTL_TYPE_DATE                     
    },
    /* DATE + INTERVAL DAY TO SECOND */
    { dtlDateAddIntervalDayToSecond,
      { DTL_TYPE_DATE, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
      DTL_TYPE_TIMESTAMP                
    },

    
    /* INTERVAL YEAR TO MONTH + DATE */
    { dtlIntervalYearToMonthAddDate,
      { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_DATE },
      DTL_TYPE_DATE
    },
    /* INTERVAL DAY TO SECOND + DATE */
    { dtlIntervalDayAddDate,
      { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_DATE },
      DTL_TYPE_DATE   
    },
    /* INTERVAL DAY TO SECOND + DATE */
    { dtlIntervalDayToSecondAddDate,
      { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_DATE },
      DTL_TYPE_TIMESTAMP   
    },

    
    { dtlIntervalYearToMonthAddIntervalYearToMonth,
      { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
      DTL_TYPE_INTERVAL_YEAR_TO_MONTH
    },    
    { dtlIntervalDayToSecondAddIntervalDayToSecond,
      { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
      DTL_TYPE_INTERVAL_DAY_TO_SECOND
    },

    
    { dtlTimestampAddInteger,
      { DTL_TYPE_TIMESTAMP, DTL_TYPE_NATIVE_INTEGER },
      DTL_TYPE_TIMESTAMP
    },
    { dtlIntegerAddTimestamp,
      { DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_TIMESTAMP },
      DTL_TYPE_TIMESTAMP
    },

    
    { dtlTimestampAddNumeric,
      { DTL_TYPE_TIMESTAMP, DTL_TYPE_NUMBER },
      DTL_TYPE_TIMESTAMP
    },
    { dtlNumericAddTimestamp,
      { DTL_TYPE_NUMBER, DTL_TYPE_TIMESTAMP },
      DTL_TYPE_TIMESTAMP
    },

    
    { dtlTimestampAddIntervalYearToMonth,
      { DTL_TYPE_TIMESTAMP, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
      DTL_TYPE_TIMESTAMP
    },    
    { dtlTimestampAddIntervalDayToSecond,
      { DTL_TYPE_TIMESTAMP, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
      DTL_TYPE_TIMESTAMP
    },

    
    { dtlIntervalYearToMonthAddTimestamp,
      { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_TIMESTAMP },
      DTL_TYPE_TIMESTAMP
    },    
    { dtlIntervalDayToSecondAddTimestamp,
      { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_TIMESTAMP },
      DTL_TYPE_TIMESTAMP
    },

    
    { dtlTimestampTzAddInteger,
      { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_NATIVE_INTEGER },
      DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE
    },
    { dtlIntegerAddTimestampTz,
      { DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE },
      DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE
    },

    
    { dtlTimestampTzAddNumeric,
      { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_NUMBER },
      DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE
    },
    { dtlNumericAddTimestampTz,
      { DTL_TYPE_NUMBER, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE },
      DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE
    },

    
    { dtlTimestampTzAddIntervalYearToMonth,
      { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
      DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE
    },    
    { dtlTimestampTzAddIntervalDayToSecond,
      { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
      DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE
    },

    
    { dtlIntervalYearToMonthAddTimestampTz,
      { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE },
      DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE
    },    
    { dtlIntervalDayToSecondAddTimestampTz,
      { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE },
      DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE
    },


    { dtlTimeAddInteger,
      { DTL_TYPE_TIME, DTL_TYPE_NATIVE_INTEGER },
      DTL_TYPE_TIME
    },
    { dtlIntegerAddTime,
      { DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_TIME },
      DTL_TYPE_TIME
    },


    { dtlTimeAddNumeric,
      { DTL_TYPE_TIME, DTL_TYPE_NUMBER },
      DTL_TYPE_TIME
    },
    { dtlNumericAddTime,
      { DTL_TYPE_NUMBER, DTL_TYPE_TIME },
      DTL_TYPE_TIME
    },
    
    
    { dtlTimeAddIntervalYearToMonth,
      { DTL_TYPE_TIME, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
      DTL_TYPE_TIME,
    },
    { dtlIntervalYearToMonthAddTime,
      { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_TIME },
      DTL_TYPE_TIME,
    },

    
    { dtlTimeAddIntervalDayToSecond,
      { DTL_TYPE_TIME, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
      DTL_TYPE_TIME,
    },
    { dtlIntervalDayToSecondAddTime,
      { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_TIME },
      DTL_TYPE_TIME,
    },


    { dtlTimeTzAddInteger,
      { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_NATIVE_INTEGER },
      DTL_TYPE_TIME_WITH_TIME_ZONE
    },
    { dtlIntegerAddTimeTz,
      { DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_TIME_WITH_TIME_ZONE },
      DTL_TYPE_TIME_WITH_TIME_ZONE
    },


    { dtlTimeTzAddNumeric,
      { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_NUMBER },
      DTL_TYPE_TIME_WITH_TIME_ZONE
    },
    { dtlNumericAddTimeTz,
      { DTL_TYPE_NUMBER, DTL_TYPE_TIME_WITH_TIME_ZONE },
      DTL_TYPE_TIME_WITH_TIME_ZONE
    },

    
    { dtlTimeTzAddIntervalYearToMonth,
      { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
      DTL_TYPE_TIME_WITH_TIME_ZONE,
    },
    { dtlIntervalYearToMonthAddTimeTz,
      { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_TIME_WITH_TIME_ZONE },
      DTL_TYPE_TIME_WITH_TIME_ZONE,
    },

    
    { dtlTimeTzAddIntervalDayToSecond,
      { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
      DTL_TYPE_TIME_WITH_TIME_ZONE,
    },  
    { dtlIntervalDayToSecondAddTimeTz,
      { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_TIME_WITH_TIME_ZONE },
      DTL_TYPE_TIME_WITH_TIME_ZONE,
    },  

    
    { NULL,
      { DTL_TYPE_NA, },
      DTL_TYPE_NA
    }
};


stlStatus dtlBigIntAdd( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aInfo,
                        dtlFuncVector  * aVectorFunc,
                        void           * aVectorArg,
                        void           * aEnv )
{
    return dtfBigIntAdd( aInputArgumentCnt,
                         aInputArgument,
                         aResultValue,
                         aEnv );
}

stlStatus dtlDoubleAdd( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aInfo,
                        dtlFuncVector  * aVectorFunc,
                        void           * aVectorArg,
                        void           * aEnv )
{
    return dtfDoubleAdd( aInputArgumentCnt,
                         aInputArgument,
                         aResultValue,
                         aEnv );
}

stlStatus dtlNumericAdd( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv )
{
    return dtfNumericAdd( aInputArgumentCnt,
                          aInputArgument,
                          aResultValue,
                          aEnv );
}

stlStatus dtlDateAddInteger( stlUInt16        aInputArgumentCnt,
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

stlStatus dtlIntegerAddDate( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aInfo,
                             dtlFuncVector  * aVectorFunc,
                             void           * aVectorArg,
                             void           * aEnv )
{
    return dtfIntegerAddDate( aInputArgumentCnt,
                              aInputArgument,
                              aResultValue,
                              aEnv );
}

stlStatus dtlDateAddNumeric( stlUInt16        aInputArgumentCnt,
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

stlStatus dtlNumericAddDate( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aInfo,
                             dtlFuncVector  * aVectorFunc,
                             void           * aVectorArg,
                             void           * aEnv )
{
    return dtfNumericAddDate( aInputArgumentCnt,
                              aInputArgument,
                              aResultValue,
                              aEnv );
}

stlStatus dtlDateAddIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
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

stlStatus dtlIntervalYearToMonthAddDate( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv )
{
    return dtfIntervalYearToMonthAddDate( aInputArgumentCnt,
                                          aInputArgument,
                                          aResultValue,
                                          aEnv );
}

stlStatus dtlDateAddIntervalDay( stlUInt16        aInputArgumentCnt,
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
    
stlStatus dtlDateAddIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
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

stlStatus dtlIntervalDayAddDate( stlUInt16        aInputArgumentCnt,
                                 dtlValueEntry  * aInputArgument,
                                 void           * aResultValue,
                                 void           * aInfo,
                                 dtlFuncVector  * aVectorFunc,
                                 void           * aVectorArg,
                                 void           * aEnv )
{
    return dtfIntervalDayAddDate( aInputArgumentCnt,
                                  aInputArgument,
                                  aResultValue,
                                  aVectorFunc,
                                  aVectorArg,
                                  aEnv );
}

stlStatus dtlIntervalDayToSecondAddDate( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv )
{
    return dtfIntervalDayToSecondAddDate( aInputArgumentCnt,
                                          aInputArgument,
                                          aResultValue,
                                          aVectorFunc,
                                          aVectorArg,
                                          aEnv );
}

stlStatus dtlTimeAddInteger( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aInfo,
                             dtlFuncVector  * aVectorFunc,
                             void           * aVectorArg,
                             void           * aEnv )
{
    return dtfTimeAddInteger( aInputArgumentCnt,
                              aInputArgument,
                              aResultValue,
                              aEnv );
}

stlStatus dtlIntegerAddTime( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aInfo,
                             dtlFuncVector  * aVectorFunc,
                             void           * aVectorArg,
                             void           * aEnv )
{
    return dtfIntegerAddTime( aInputArgumentCnt,
                              aInputArgument,
                              aResultValue,
                              aEnv );
}

stlStatus dtlTimeAddNumeric( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aInfo,
                             dtlFuncVector  * aVectorFunc,
                             void           * aVectorArg,
                             void           * aEnv )
{
    return dtfTimeAddNumeric( aInputArgumentCnt,
                              aInputArgument,
                              aResultValue,
                              aEnv );
}

stlStatus dtlNumericAddTime( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aInfo,
                             dtlFuncVector  * aVectorFunc,
                             void           * aVectorArg,
                             void           * aEnv )
{
    return dtfNumericAddTime( aInputArgumentCnt,
                              aInputArgument,
                              aResultValue,
                              aEnv );
}

stlStatus dtlTimeAddIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv )
{
    return dtfTimeAddIntervalYearToMonth( aInputArgumentCnt,
                                          aInputArgument,
                                          aResultValue,
                                          aEnv );
}

stlStatus dtlIntervalYearToMonthAddTime( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv )
{
    return dtfIntervalYearToMonthAddTime( aInputArgumentCnt,
                                          aInputArgument,
                                          aResultValue,
                                          aEnv );
}

stlStatus dtlTimeAddIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv )
{
    return dtfTimeAddIntervalDayToSecond( aInputArgumentCnt,
                                          aInputArgument,
                                          aResultValue,
                                          aEnv );
}

stlStatus dtlIntervalDayToSecondAddTime( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv )
{
    return dtfIntervalDayToSecondAddTime( aInputArgumentCnt,
                                          aInputArgument,
                                          aResultValue,
                                          aEnv );
}

stlStatus dtlTimeTzAddInteger( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aInfo,
                               dtlFuncVector  * aVectorFunc,
                               void           * aVectorArg,
                               void           * aEnv )
{
    return dtfTimeTzAddInteger( aInputArgumentCnt,
                                aInputArgument,
                                aResultValue,
                                aEnv );
}

stlStatus dtlIntegerAddTimeTz( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aInfo,
                               dtlFuncVector  * aVectorFunc,
                               void           * aVectorArg,
                               void           * aEnv )
{
    return dtfIntegerAddTimeTz( aInputArgumentCnt,
                                aInputArgument,
                                aResultValue,
                                aEnv );
}

stlStatus dtlTimeTzAddNumeric( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aInfo,
                               dtlFuncVector  * aVectorFunc,
                               void           * aVectorArg,
                               void           * aEnv )
{
    return dtfTimeTzAddNumeric( aInputArgumentCnt,
                                aInputArgument,
                                aResultValue,
                                aEnv );
}

stlStatus dtlNumericAddTimeTz( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aInfo,
                               dtlFuncVector  * aVectorFunc,
                               void           * aVectorArg,
                               void           * aEnv )
{
    return dtfNumericAddTimeTz( aInputArgumentCnt,
                                aInputArgument,
                                aResultValue,
                                aEnv );
}

stlStatus dtlTimeTzAddIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv )
{
    return dtfTimeTzAddIntervalYearToMonth( aInputArgumentCnt,
                                            aInputArgument,
                                            aResultValue,
                                            aEnv );
}

stlStatus dtlIntervalYearToMonthAddTimeTz( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv )
{
    return dtfIntervalYearToMonthAddTimeTz( aInputArgumentCnt,
                                            aInputArgument,
                                            aResultValue,
                                            aEnv );
}

stlStatus dtlTimeTzAddIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv )
{
    return dtfTimeTzAddIntervalDayToSecond( aInputArgumentCnt,
                                            aInputArgument,
                                            aResultValue,
                                            aEnv );
}

stlStatus dtlIntervalDayToSecondAddTimeTz( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv )
{
    return dtfIntervalDayToSecondAddTimeTz( aInputArgumentCnt,
                                            aInputArgument,
                                            aResultValue,
                                            aEnv );
}

stlStatus dtlIntervalYearToMonthAddIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                                        dtlValueEntry  * aInputArgument,
                                                        void           * aResultValue,
                                                        void           * aInfo,
                                                        dtlFuncVector  * aVectorFunc,
                                                        void           * aVectorArg,
                                                        void           * aEnv )
{
    return dtfIntervalYearToMonthAddIntervalYearToMonth( aInputArgumentCnt,
                                                         aInputArgument,
                                                         aResultValue,
                                                         aEnv );
}

stlStatus dtlIntervalDayToSecondAddIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                                        dtlValueEntry  * aInputArgument,
                                                        void           * aResultValue,
                                                        void           * aInfo,
                                                        dtlFuncVector  * aVectorFunc,
                                                        void           * aVectorArg,
                                                        void           * aEnv )
{
    return dtfIntervalDayToSecondAddIntervalDayToSecond( aInputArgumentCnt,
                                                         aInputArgument,
                                                         aResultValue,
                                                         aEnv );
}

stlStatus dtlTimestampAddInteger( stlUInt16        aInputArgumentCnt,
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

stlStatus dtlIntegerAddTimestamp( stlUInt16        aInputArgumentCnt,
                                  dtlValueEntry  * aInputArgument,
                                  void           * aResultValue,
                                  void           * aInfo,
                                  dtlFuncVector  * aVectorFunc,
                                  void           * aVectorArg,
                                  void           * aEnv )
{
    return dtfIntegerAddTimestamp( aInputArgumentCnt,
                                   aInputArgument,
                                   aResultValue,
                                   aEnv );
}

stlStatus dtlTimestampAddNumeric( stlUInt16        aInputArgumentCnt,
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

stlStatus dtlNumericAddTimestamp( stlUInt16        aInputArgumentCnt,
                                  dtlValueEntry  * aInputArgument,
                                  void           * aResultValue,
                                  void           * aInfo,
                                  dtlFuncVector  * aVectorFunc,
                                  void           * aVectorArg,
                                  void           * aEnv )
{
    return dtfNumericAddTimestamp( aInputArgumentCnt,
                                   aInputArgument,
                                   aResultValue,
                                   aEnv );
}

stlStatus dtlTimestampAddIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
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

stlStatus dtlIntervalYearToMonthAddTimestamp( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv )
{
    return dtfIntervalYearToMonthAddTimestamp( aInputArgumentCnt,
                                               aInputArgument,
                                               aResultValue,
                                               aEnv );
}

stlStatus dtlTimestampAddIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
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

stlStatus dtlIntervalDayToSecondAddTimestamp( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv )
{
    return dtfIntervalDayToSecondAddTimestamp( aInputArgumentCnt,
                                               aInputArgument,
                                               aResultValue,
                                               aEnv );
}

stlStatus dtlTimestampTzAddInteger( stlUInt16        aInputArgumentCnt,
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

stlStatus dtlIntegerAddTimestampTz( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    void           * aInfo,
                                    dtlFuncVector  * aVectorFunc,
                                    void           * aVectorArg,
                                    void           * aEnv )
{
    return dtfIntegerAddTimestampTz( aInputArgumentCnt,
                                     aInputArgument,
                                     aResultValue,
                                     aEnv );
}

stlStatus dtlTimestampTzAddNumeric( stlUInt16        aInputArgumentCnt,
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

stlStatus dtlNumericAddTimestampTz( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    void           * aInfo,
                                    dtlFuncVector  * aVectorFunc,
                                    void           * aVectorArg,
                                    void           * aEnv )
{
    return dtfNumericAddTimestampTz( aInputArgumentCnt,
                                     aInputArgument,
                                     aResultValue,
                                     aEnv );
}

stlStatus dtlTimestampTzAddIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
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

stlStatus dtlIntervalYearToMonthAddTimestampTz( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv )
{
    return dtfIntervalYearToMonthAddTimestampTz( aInputArgumentCnt,
                                                 aInputArgument,
                                                 aResultValue,
                                                 aEnv );
}

stlStatus dtlTimestampTzAddIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
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

stlStatus dtlIntervalDayToSecondAddTimestampTz( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv )
{
    return dtfIntervalDayToSecondAddTimestampTz( aInputArgumentCnt,
                                                 aInputArgument,
                                                 aResultValue,
                                                 aEnv );
}

/**
 * @brief add function에 대한 정보 얻기
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
stlStatus dtlGetFuncInfoAdd( stlUInt16               aDataTypeArrayCount,
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
    STL_TRY( dtlGetFuncInfoCommon( &dtlFuncAddInfo,
                                   dtlFuncAddList,
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
            
            *aFuncResultDataType = dtlFuncAddList[*aFuncPtrIdx].mResultType;
            
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
    else if( ( aFuncArgDataTypeArray[0] == DTL_TYPE_INTERVAL_DAY_TO_SECOND ) &&
             ( aFuncArgDataTypeArray[1] == DTL_TYPE_DATE ) )
    {
        if( aDataTypeDefInfoArray[0].mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY )
        {
            aFuncArgDataTypeDefInfoArray[0].mIntervalIndicator = aDataTypeDefInfoArray[0].mIntervalIndicator;
        }
        else
        {
            STL_DASSERT(
                (aDataTypeDefInfoArray[0].mIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR) ||
                (aDataTypeDefInfoArray[0].mIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE) ||
                (aDataTypeDefInfoArray[0].mIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND) ||
                (aDataTypeDefInfoArray[0].mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_HOUR) ||
                (aDataTypeDefInfoArray[0].mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE) ||
                (aDataTypeDefInfoArray[0].mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_SECOND) ||
                (aDataTypeDefInfoArray[0].mIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE) ||
                (aDataTypeDefInfoArray[0].mIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND) ||
                (aDataTypeDefInfoArray[0].mIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND) );
            
            (*aFuncPtrIdx)++;   
            
            *aFuncResultDataType = dtlFuncAddList[*aFuncPtrIdx].mResultType;
            
            aFuncResultDataTypeDefInfo->mArgNum1 =
                gResultDataTypeDef[*aFuncResultDataType].mArgNum1;
            aFuncResultDataTypeDefInfo->mArgNum2 =
                gResultDataTypeDef[*aFuncResultDataType].mArgNum2;
            aFuncResultDataTypeDefInfo->mStringLengthUnit =
                gResultDataTypeDef[*aFuncResultDataType].mStringLengthUnit;
            aFuncResultDataTypeDefInfo->mIntervalIndicator =
                aDataTypeDefInfoArray[0].mIntervalIndicator;
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
 * @brief add function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlGetFuncPtrAdd( stlUInt32             aFuncPtrIdx,
                            dtlBuiltInFuncPtr   * aFuncPtr,
                            stlErrorStack       * aErrorStack )
{
    /**
     * output 설정
     */

    /*
     * dtlAdd가 n차원 배열임으로 1차원 배열로 typecast해서
     * aFuncPtrIdx를 적용한다.
     */
    *aFuncPtr = dtlFuncAddList[aFuncPtrIdx].mFuncPtr;

    return STL_SUCCESS;
}



/** @} */
