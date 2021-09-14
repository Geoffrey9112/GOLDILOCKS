/*******************************************************************************
 * dtfLastDay.h
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

#ifndef _DTF_LAST_DAY_H_
#define _DTF_LAST_DAY_H_ 1

/**
 * @file dtfLastDay.h
 * @brief Last_day Functions for DataType Layer
 */

/**
 * @addtogroup dtfLastDay Last day Function
 * @ingroup dtf
 * @internal
 * @{
 */

stlStatus dtfDateLastDay( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv );

stlStatus dtfTimestampLastDay( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               dtlFuncVector  * aVectorFunc,
                               void           * aVectorArg,
                               void           * aEnv );

stlStatus dtfTimestampWithTimeZoneLastDay( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv );


/** @} */
    
#endif /* _DTF_LAST_DAY_H_ */
