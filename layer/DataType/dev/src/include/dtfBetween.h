/*******************************************************************************
 * dtfBetween.h
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


#ifndef _DTF_BETWEEN_H_
#define _DTF_BETWEEN_H_ 1

/**
 * @file dtfBetween.h
 * @brief Between For DataType Layer 
 */

/**
 * @addtogroup dtfBetween Between
 * @ingroup dtf
 * @{
 */

stlStatus dtfBetweenSymmetric( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aInfo,
                               dtlFuncVector  * aVectorFunc,
                               void           * aVectorArg,
                               void           * aEnv );

stlStatus dtfNotBetweenSymmetric( stlUInt16        aInputArgumentCnt,
                                  dtlValueEntry  * aInputArgument,
                                  void           * aResultValue,
                                  void           * aInfo,
                                  dtlFuncVector  * aVectorFunc,
                                  void           * aVectorArg,
                                  void           * aEnv );
/** @} */

#endif /* _DTF_BETWEEN_H_ */
