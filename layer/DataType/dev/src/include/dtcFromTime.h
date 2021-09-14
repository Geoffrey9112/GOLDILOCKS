/*******************************************************************************
 * dtcFromTime.h
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


#ifndef _DTC_TIME_H_
#define _DTC_TIME_H_ 1

/**
 * @file dtcFromTime.h
 * @brief Time로부터 다른 타입으로의 변환 
 */

/**
 * @defgroup dtcFromTime Time로부터 다른 타입으로의 변환 
 * @ingroup dtcCast
 * @{
 */

stlStatus dtcCastTimeToChar( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aInfo,
                             dtlFuncVector  * aVectorFunc,
                             void           * aVectorArg,
                             void           * aEnv );

stlStatus dtcCastTimeToVarchar( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                void           * aInfo,
                                dtlFuncVector  * aVectorFunc,
                                void           * aVectorArg,
                                void           * aEnv );

stlStatus dtcCastTimeToLongvarchar( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    void           * aInfo,
                                    dtlFuncVector  * aVectorFunc,
                                    void           * aVectorArg,
                                    void           * aEnv );

stlStatus dtcCastTimeToTime( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aInfo,
                             dtlFuncVector  * aVectorFunc,
                             void           * aVectorArg,
                             void           * aEnv );

stlStatus dtcCastTimeToTimeTz( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aInfo,
                               dtlFuncVector  * aVectorFunc,
                               void           * aVectorArg,
                               void           * aEnv );
/** @} */

#endif /* _DTC_TIME_H_ */
