/*******************************************************************************
 * dtcFromTimestampTz.h
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


#ifndef _DTC_TIMESTAMPTZ_H_
#define _DTC_TIMESTAMPTZ_H_ 1

/**
 * @file dtcFromTimestampTz.h
 * @brief TimestampTz로부터 다른 타입으로의 변환 
 */

/**
 * @defgroup dtcFromTimestampTz TimestampTz로부터 다른 타입으로의 변환 
 * @ingroup dtcCast
 * @{
 */

stlStatus dtcCastTimestampTzToChar( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    void           * aInfo,
                                    dtlFuncVector  * aVectorFunc,
                                    void           * aVectorArg,
                                    void           * aEnv );

stlStatus dtcCastTimestampTzToVarchar( stlUInt16        aInputArgumentCnt,
                                       dtlValueEntry  * aInputArgument,
                                       void           * aResultValue,
                                       void           * aInfo,
                                       dtlFuncVector  * aVectorFunc,
                                       void           * aVectorArg,
                                       void           * aEnv );

stlStatus dtcCastTimestampTzToLongvarchar( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv );

stlStatus dtcCastTimestampTzToDate( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    void           * aInfo,
                                    dtlFuncVector  * aVectorFunc,
                                    void           * aVectorArg,
                                    void           * aEnv );

stlStatus dtcCastTimestampTzToTime( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    void           * aInfo,
                                    dtlFuncVector  * aVectorFunc,
                                    void           * aVectorArg,
                                    void           * aEnv );

stlStatus dtcCastTimestampTzToTimeTz( stlUInt16        aInputArgumentCnt,
                                      dtlValueEntry  * aInputArgument,
                                      void           * aResultValue,
                                      void           * aInfo,
                                      dtlFuncVector  * aVectorFunc,
                                      void           * aVectorArg,
                                      void           * aEnv );

stlStatus dtcCastTimestampTzToTimestamp( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv );

stlStatus dtcCastTimestampTzToTimestampTz( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv );

/** @} */

#endif /* _DTC_TIMESTAMPTZ_H_ */
