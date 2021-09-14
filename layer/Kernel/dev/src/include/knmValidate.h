/*******************************************************************************
 * knmValidate.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: knmValidate.h 591 2011-04-20 05:25:54Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _KNMVALIDATE_H_
#define _KNMVALIDATE_H_ 1

/**
 * @file knmValidate.h
 * @brief Kernel Layer Memory Validation Internal Routines
 */

/**
 * @defgroup knmValidate Memory Validation
 * @ingroup knmMemory
 * @internal
 * @{
 */

stlStatus knmInitMemory( knmHeadFence * aHeadFence,
                         stlUInt32      aSize,
                         knlEnv       * aEnv );

stlStatus knmValidateArena( knmArenaHeader * aArenaHeader,
                            knlEnv         * aEnv );

stlStatus knmValidateFence( knmHeadFence * aHeadFence,
                            knlEnv       * aEnv );

/** @} */
    
#endif /* _KNMVALIDATE_H_ */
