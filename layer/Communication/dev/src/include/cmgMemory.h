/*******************************************************************************
 * cmgMemory.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: cmgMemory.h 6534 2012-12-04 00:37:52Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _CMGMEMORY_H_
#define _CMGMEMORY_H_ 1

/**
 * @file cmgMemory.h
 * @brief Communication Layer Memory Component Routines
 */

/**
 * @defgroup cmgMemory Memory
 * @ingroup cmExternal
 * @{
 */

stlStatus cmgAllocOperationMem( cmlHandle          * aHandle,
                                stlInt64             aSize,
                                void              ** aMemory,
                                stlErrorStack      * aErrorStack );


/** @} */

#endif /* _CMGMEMORY_H_ */
