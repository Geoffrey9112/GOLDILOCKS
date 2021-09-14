/*******************************************************************************
 * dtfAddMonths.h
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

#ifndef _DTF_ADD_MONTHS_H_
#define _DTF_ADD_MONTHS_H_ 1

/**
 * @file dtfAddMonths.h
 * @brief Add_months Functions for DataType Layer
 */

/**
 * @addtogroup dtfAddMonth Add_month Function
 * @ingroup dtf
 * @internal
 * @{
 */

stlStatus dtfDateAddMonths( stlUInt16        aInputArgumentCnt,
                            dtlValueEntry  * aInputArgument,
                            void           * aResultValue,
                            dtlFuncVector  * aVectorFunc,
                            void           * aVectorArg,
                            void           * aEnv );

stlStatus dtfTimestampAddMonths( stlUInt16        aInputArgumentCnt,
                                 dtlValueEntry  * aInputArgument,
                                 void           * aResultValue,
                                 dtlFuncVector  * aVectorFunc,
                                 void           * aVectorArg,
                                 void           * aEnv );

stlStatus dtfTimestampWithTimeZoneAddMonths( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv );

/** @} */
    
#endif /* _DTF_ADD_MONTHS_H_ */
