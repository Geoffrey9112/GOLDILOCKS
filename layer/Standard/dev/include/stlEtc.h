/****************************************
 * stlEtc.h
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
 ***************************************/


#ifndef _STLETC_H_
#define _STLETC_H_ 1

/**
 * @file stlEtc.h
 * @brief Standard Layer ETC
 */

STL_BEGIN_DECLS


/**
 * @defgroup stlEtc ETC Routines
 * @ingroup STL 
 * @{
 */

void stlSRand( stlUInt32  aSeed );

stlUInt32 stlRand( );

void * stlBSearch( const void  * aKey,
                   const void  * aBase,
                   stlSize       aBaseArrayCount,
                   stlSize       aSize,
                   stlInt32      (*aCompareFunc)(const void *, const void *) );

inline stlInt32 stlIgnorePrintf( const stlChar *aFormat, ...);

/* @} */
    
STL_END_DECLS

#endif /* _STLETC_H_ */
