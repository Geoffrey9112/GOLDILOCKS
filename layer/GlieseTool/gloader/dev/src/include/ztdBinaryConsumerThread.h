/*******************************************************************************
 * ztdExecute.h
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZTDBINARYCONSUMERTHREAD_H_
#define _ZTDBINARYCONSUMERTHREAD_H_ 1

/**
 * @file ztdExecThread.h
 * @brief ExecThread  Definition
 */

/**
 * @defgroup ztdExecThread ExecThread
 * @ingroup ztd
 * @{
 */

/**
 * @addtogroup binary mode
 * @{
 */

void * STL_THREAD_FUNC ztdFetchValueBlockList( stlThread * aThread,
                                               void      * aArg );

void * STL_THREAD_FUNC ztdWriteValueBlockList( stlThread * aThread,
                                               void      * aArg );

void * STL_THREAD_FUNC ztdReadValueBlockList( stlThread * aThread,
                                              void      * aArg );

void * STL_THREAD_FUNC ztdExecuteValueBlockList( stlThread * aThread,
                                                 void      * aArg );

void * STL_THREAD_FUNC ztdWriteBadValueBlockList( stlThread * aThread,
                                                  void      * aArg );

/** @} */

/** @} */

#endif /* _ZTDBINARYCONSUMERTHREAD_H_ */
