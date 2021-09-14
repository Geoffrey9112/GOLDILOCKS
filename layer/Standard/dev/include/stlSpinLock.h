/*******************************************************************************
 * stlSpinLock.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _STLSPINLOCKL_H_
#define _STLSPINLOCK_H_ 1

/**
 * @file stlPoll.h
 * @brief Standard Layer Poll Routines
 */

#include <stlDef.h>

/**
 * @defgroup stlSpinLock spin lock
 * @ingroup STL 
 * @{
 */

void stlInitSpinLock( stlSpinLock * aSpinLock );

void stlFinSpinLock( stlSpinLock * aSpinLock );

void stlAcquireSpinLock( stlSpinLock * aSpinLock,
                         stlUInt64   * aCasMissCnt );

void stlTrySpinLock( stlSpinLock * aSpinLock,
                     stlBool     * aIsSuccess,
                     stlUInt64   * aCasMissCnt );

void stlReleaseSpinLock( stlSpinLock * aSpinLock );

void stlSetMaxSpinCount( stlUInt32 aSpinCount );

stlInt64 stlGetMaxSpinCount();

void stlSetMaxBusyWaitCount( stlUInt32 aBusyWaitCount );

/** @} */

#endif /* _STLSPINLOCK_H_ */
