/*******************************************************************************
 * dtfOperLogical.h
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


#ifndef _DTF_OPER_LOGICAL_H_
#define _DTF_OPER_LOGICAL_H_ 1

/**
 * @file dtfOperLogical.h
 * @brief logical operator for DataType Layer
 */

/**
 * @addtogroup dtfOperLogical 논리연산
 * @ingroup dtf
 * @{
 */

stlStatus dtfAnd( stlUInt16        aInputArgumentCnt,
                  dtlValueEntry  * aInputArgument,
                  void           * aResultValue,
                  void           * aEnv );

stlStatus dtfOr( stlUInt16        aInputArgumentCnt,
                 dtlValueEntry  * aInputArgument,
                 void           * aResultValue,
                 void           * aEnv );

stlStatus dtfNot( stlUInt16        aInputArgumentCnt,
                  dtlValueEntry  * aInputArgument,
                  void           * aResultValue,
                  void           * aEnv );

stlStatus dtfIs( stlUInt16        aInputArgumentCnt,
                 dtlValueEntry  * aInputArgument,
                 void           * aResultValue,
                 void           * aEnv );

stlStatus dtfIsNot( stlUInt16        aInputArgumentCnt,
                    dtlValueEntry  * aInputArgument,
                    void           * aResultValue,
                    void           * aEnv );

/** @} */
    
#endif /* _DTF_OPER_LOGICAL_H_ */
