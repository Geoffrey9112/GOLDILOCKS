/*******************************************************************************
 * knlMemController.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: knlMemController.h 8792 2013-06-25 04:57:44Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _KNLMEMCONTROLLER_H_
#define _KNLMEMCONTROLLER_H_ 1

/**
 * @file knlMemController.h
 * @brief Kernel Layer Memory Controller
 */

/**
 * @defgroup knlMemController Memory Controller
 * @ingroup knlMemManager
 * @{
 */

stlStatus knlInitMemController( knlMemController * aMemController,
                                stlUInt64          aMaxSize,
                                knlEnv           * aEnv );

stlStatus knlFiniMemController( knlMemController * aMemController,
                                knlEnv           * aEnv );

/** @} */
    
#endif /* _KNLMEMCONTROLLER_H_ */
