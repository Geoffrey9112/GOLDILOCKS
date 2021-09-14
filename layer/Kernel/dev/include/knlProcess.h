/*******************************************************************************
 * knlProcess.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: knlProcess.h $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _KNLPROCESS_H_
#define _KNLPROCESS_H_ 1

/**
 * @file knlProcess.h
 * @brief Kernel Layer Pproccess Routines
 */

/**
 * @defgroup knlProcess Process 
 * @ingroup knExternal
 * @{
 */

stlStatus knlKillProcesses4Syncher( void      * aEntryPoint,
                                    stlBool   * aIsInactive,
                                    stlBool     aSilent,
                                    stlLogger * aLogger,
                                    knlEnv    * aEnv );

/** @} */

#endif /* _KNLPROCESS_H_ */
