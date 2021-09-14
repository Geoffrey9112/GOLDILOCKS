/*******************************************************************************
 * dtcFromLongvarbinary.h
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


#ifndef _DTC_LONGVARBINARY_H_
#define _DTC_LONGVARBINARY_H_ 1

/**
 * @file dtcFromLongvarbinary.h
 * @brief Longvarbinary로부터 다른 타입으로의 변환 
 */

/**
 * @defgroup dtcFromLongvarbinary Longvarbinary로부터 다른 타입으로의 변환 
 * @ingroup dtcCast
 * @{
 */

stlStatus dtcCastLongvarbinaryToBinary( stlUInt16        aInputArgumentCnt,
                                        dtlValueEntry  * aInputArgument,
                                        void           * aResultValue,
                                        void           * aInfo,
                                        dtlFuncVector  * aVectorFunc,
                                        void           * aVectorArg,
                                        void           * aEnv );

stlStatus dtcCastLongvarbinaryToVarbinary( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv );

stlStatus dtcCastLongvarbinaryToLongvarbinary( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv );
/** @} */

#endif /* _DTC_LONGVARBINARY_H_ */
