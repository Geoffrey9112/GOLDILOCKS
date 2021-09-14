/*******************************************************************************
 * dtcFromRowId.h
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


#ifndef _DTC_ROWID_H_
#define _DTC_ROWID_H_ 1

/**
 * @file dtcFromRowId.h
 * @brief RowId로부터 다른 타입으로의 변환 
 */

/**
 * @defgroup dtcFromRowId RowId로부터 다른 타입으로의 변환 
 * @ingroup dtcCast
 * @{
 */

stlStatus dtcCastRowIdToChar( stlUInt16        aInputArgumentCnt,
                              dtlValueEntry  * aInputArgument,
                              void           * aResultValue,
                              void           * aInfo,
                              dtlFuncVector  * aVectorFunc,
                              void           * aVectorArg,
                              void           * aEnv );

stlStatus dtcCastRowIdToVarchar( stlUInt16        aInputArgumentCnt,
                                 dtlValueEntry  * aInputArgument,
                                 void           * aResultValue,
                                 void           * aInfo,
                                 dtlFuncVector  * aVectorFunc,
                                 void           * aVectorArg,
                                 void           * aEnv );

stlStatus dtcCastRowIdToLongvarchar( stlUInt16        aInputArgumentCnt,
                                     dtlValueEntry  * aInputArgument,
                                     void           * aResultValue,
                                     void           * aInfo,
                                     dtlFuncVector  * aVectorFunc,
                                     void           * aVectorArg,
                                     void           * aEnv );

stlStatus dtcCastRowIdToRowId( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aInfo,
                               dtlFuncVector  * aVectorFunc,
                               void           * aVectorArg,
                               void           * aEnv );

/** @} */

#endif /* _DTC_ROWID_H_ */
