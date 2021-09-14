/*******************************************************************************
 * stlAtomic.h
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


#ifndef _STLATOMIC_H_
#define _STLATOMIC_H_ 1

/**
 * @file stlAtomic.h
 * @brief Standard Layer Atomic Routines
 */

#include <stlDef.h>

STL_BEGIN_DECLS


/**
 * @defgroup stlAtomic Atomic
 * @ingroup STL 
 * @{
 */

stlUInt32 stlAtomicRead32( volatile stlUInt32 *aMem );
void stlAtomicSet32( volatile stlUInt32 * aMem,
                     stlUInt32            aVal );
stlUInt32 stlAtomicAdd32( volatile stlUInt32 * aMem,
                          stlUInt32            aVal );
void stlAtomicSub32( volatile stlUInt32 * aMem,
                     stlUInt32            aVal );
stlUInt32 stlAtomicInc32( volatile stlUInt32 * aMem );
stlInt32 stlAtomicDec32( volatile stlUInt32 * aMem );
stlUInt32 stlAtomicXchg32( volatile stlUInt32 * aMem,
                           stlUInt32            aVal );

#if( (STL_SIZEOF_VOIDP == 8) || defined(DOXYGEN) )

stlUInt64 stlAtomicRead64( volatile stlUInt64 * aMem );
void stlAtomicSet64( volatile stlUInt64 * aMem,
                     stlUInt64            aVal );
stlUInt64 stlAtomicAdd64( volatile stlUInt64 * aMem,
                          stlUInt64            aVal );
void stlAtomicSub64( volatile stlUInt64 * aMem,
                     stlUInt64            aVal );
stlUInt64 stlAtomicInc64( volatile stlUInt64 * aMem );
stlInt64 stlAtomicDec64( volatile stlUInt64 * aMem );
stlUInt64 stlAtomicCas64( volatile stlUInt64 * aMem,
                          stlUInt64            aWith,
                          stlUInt64            aCmp );
stlUInt64 stlAtomicXchg64( volatile stlUInt64 * aMem,
                           stlUInt64            aVal );

#endif

/** @} */
    
STL_END_DECLS

#endif /* _STLATOMIC_H_ */
