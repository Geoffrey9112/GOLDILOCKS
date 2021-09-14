/*******************************************************************************
 * dtcFromIntervalYearToMonth.h
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


#ifndef _DTC_INTERVAL_YEARTOMONTH_H_
#define _DTC_INTERVAL_YEARTOMONTH_H_ 1

/**
 * @file dtcFromIntervalYearToMonth.h
 * @brief IntervalYearToMonth로부터 다른 타입으로의 변환 
 */

/**
 * @defgroup dtcFromIntervalYearToMonth IntervalYearToMonth로부터 다른 타입으로의 변환 
 * @ingroup dtcCast
 * @{
 */

stlStatus dtcCastIntervalYearToMonthToSmallInt( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv );

stlStatus dtcCastIntervalYearToMonthToInteger( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv );

stlStatus dtcCastIntervalYearToMonthToBigInt( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv );

stlStatus dtcCastIntervalYearToMonthToNumeric( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv );

stlStatus dtcCastIntervalYearToMonthToChar( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv );

stlStatus dtcCastIntervalYearToMonthToVarchar( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv );

stlStatus dtcCastIntervalYearToMonthToLongvarchar( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv );

stlStatus dtcCastIntervalYearToMonthToIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                                           dtlValueEntry  * aInputArgument,
                                                           void           * aResultValue,
                                                           void           * aInfo,
                                                           dtlFuncVector  * aVectorFunc,
                                                           void           * aVectorArg,
                                                           void           * aEnv );

/** @} */

#endif /* _DTC_INTERVAL_YEARTOMONTH_H_ */
