/*******************************************************************************
 * knlTimer.h
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


#ifndef _KNLTIMER_H_
#define _KNLTIMER_H_ 1

/**
 * @file knlTimer.h
 * @brief Kernel Layer System Timer Routines
 */

/**
 * @defgroup knlTimer System Timer
 * @ingroup knExternal
 * @{
 */

stlStatus knlUpdateSystemTimer( knlEnv * aEnv );
stlTime knlGetSystemTime();
stlStatus knlRegisterSystemTimer( knlEnv * aEnv );
stlStatus knlUnregisterSystemTimer( knlEnv * aEnv );

/** @} */
    
#endif /* _KNLTIMER_H_ */
