/*******************************************************************************
 * stlMemorys.h
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


#ifndef _STLMEMORYS_H_
#define _STLMEMORYS_H_ 1

/**
 * @file stlMemorys.h
 * @brief Standard Layer Memory Routines
 */

#include <stlDef.h>

STL_BEGIN_DECLS

/**
 * @defgroup stlMemorys Memory
 * @ingroup STL 
 * @{
 */

stlStatus stlAllocHeap( void         ** aAddr,
                        stlSize         aSize,
                        stlErrorStack * aErrorStack);

void stlFreeHeap( void * aAddr );

void * stlMemset( void   * aAddr,
                  stlInt32 aConstant,
                  stlSize  aSize );

#if 0
void * stlMemcpy( void *       aDest,
                  const void * aSrc,
                  stlSize      aSize );
#endif

void * stlMemmove( void *       aDest,
                   const void * aSrc,
                   stlSize      aSize );

void * stlMemchr( void *       aSrc,
                  stlInt32     aChar,
                  stlSize      aSize );

stlInt32 stlMemcmp( void *       aValue1,
                    void *       aValue2,
                    stlSize      aSize );


extern void stlCopy0Bytes( void * aDest, const void * aSrc );
extern void stlCopy1Bytes( void * aDest, const void * aSrc );
extern void stlCopy2Bytes( void * aDest, const void * aSrc );
extern void stlCopy3Bytes( void * aDest, const void * aSrc );
extern void stlCopy4Bytes( void * aDest, const void * aSrc );
extern void stlCopy5Bytes( void * aDest, const void * aSrc );
extern void stlCopy6Bytes( void * aDest, const void * aSrc );
extern void stlCopy7Bytes( void * aDest, const void * aSrc );
extern void stlCopy8Bytes( void * aDest, const void * aSrc );

typedef void (*stlCopyFunction)( void * aDest, const void * aSrc );

extern const stlCopyFunction gCopyFunc[9];

#if defined( STL_DEBUG ) || defined( DOXYGEN )
#define stlMemcpy( aDest, aSrc, aSize )                                         \
    {                                                                           \
        stlUInt64 stlMemcpySize = (stlUInt64)(aSize);                           \
        /* aSize가 signed인 경우 양수여야 함 */                                 \
        STL_DASSERT( (aSize) == stlMemcpySize );                                \
        /* aSize를 type casting안하면 hp에서 warning  */                        \
        STL_DASSERT( stlMemcpySize < STL_INT64_C(1073741824) );                 \
        if( (aSize) > 8 )                                                       \
        {                                                                       \
            memcpy( aDest, aSrc, aSize );                                       \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            (void)gCopyFunc[aSize]( aDest, aSrc );                              \
        }                                                                       \
    }
#else
#define stlMemcpy( aDest, aSrc, aSize )                                         \
    {                                                                           \
        if( (aSize) > 8 )                                                       \
        {                                                                       \
            memcpy( aDest, aSrc, aSize );                                       \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            (void)gCopyFunc[aSize]( aDest, aSrc );                              \
        }                                                                       \
    }
#endif

/** @} */
    
STL_END_DECLS

#endif /* _STLATOMIC_H_ */
