/*******************************************************************************
 * dtfGeneralComparison.h
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

#ifndef _DTF_FUNC_GENERAL_COMPARISON_H_
#define _DTF_FUNC_GENERAL_COMPARISON_H_ 1

/**
 * @file 
 * @brief dtfGeneralComparison Functions for DataType Layer
 */

/**
 * @addtogroup dtfGeneralComparison Greatest Least
 * @ingroup dtf
 * @internal
 * @{
 */

/**
 * general comparison data type conversion combination array
 */


/*******************************************************************************
 * LEAST    GREATEST  실행함수
 *******************************************************************************/

stlStatus dtfGeneralComparison( stlUInt16          aInputArgumentCnt,
                                dtlValueEntry    * aInputArgument,
                                void             * aResultValue,
                                dtlCompareResult   aCompareResult,
                                void             * aInfo,
                                dtlFuncVector    * aVectorFunc,
                                void             * aVectorArg,
                                void             * aEnv );


/** @} */
    
#endif /* _DTF_FUNC_GENERAL_COMPARISON_H_ */
