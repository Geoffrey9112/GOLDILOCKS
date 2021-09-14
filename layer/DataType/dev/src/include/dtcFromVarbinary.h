/*******************************************************************************
 * dtcFromVarbinary.h
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


#ifndef _DTC_VARBINARY_H_
#define _DTC_VARBINARY_H_ 1

/**
 * @file dtcFromVarbinary.h
 * @brief Varbinary로부터 다른 타입으로의 변환 
 */

/**
 * @defgroup dtcFromVarbinary Varbinary로부터 다른 타입으로의 변환 
 * @ingroup dtcCast
 * @{
 */

stlStatus dtcCastVarbinaryToBinary( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    void           * aInfo,
                                    dtlFuncVector  * aVectorFunc,
                                    void           * aVectorArg,
                                    void           * aEnv );

stlStatus dtcCastVarbinaryToVarbinary( stlUInt16        aInputArgumentCnt,
                                       dtlValueEntry  * aInputArgument,
                                       void           * aResultValue,
                                       void           * aInfo,
                                       dtlFuncVector  * aVectorFunc,
                                       void           * aVectorArg,
                                       void           * aEnv );

stlStatus dtcCastVarbinaryToLongvarbinary( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv );

/** @} */

#endif /* _DTC_VARBINARY_H_ */
