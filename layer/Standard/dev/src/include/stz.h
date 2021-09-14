/*******************************************************************************
 * stz.h
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


#ifndef _STZ_H_
#define _STZ_H_ 1

STL_BEGIN_DECLS

typedef struct stzChunkHeader
{
    stlInt32   mTotalSize;
    stlInt32   mAllocHwm;
    void     * mAddr;
    void     * mNextChunk;
} stzChunkHeader;

#define STZ_INIT_CHUNK( aChunkHeader, aChunkSize, aAddr )       \
{                                                               \
    (aChunkHeader)->mTotalSize = aChunkSize;                    \
    (aChunkHeader)->mNextChunk = NULL;                          \
    (aChunkHeader)->mAddr      = aAddr;                         \
    (aChunkHeader)->mAllocHwm  = STL_SIZEOF(stzChunkHeader);    \
}

#define STZ_CAN_ALLOC_MEM( sChunkHeader, aAllocSize )           \
    ((((sChunkHeader)->mAllocHwm + aAllocSize)                  \
      > (sChunkHeader)->mTotalSize ) ? STL_FALSE : STL_TRUE)

stlStatus stzAllocChunk( stlInt64         aSize,
                         void          ** aChunk,
                         stlErrorStack  * aErrorStack );

stlStatus stzAllocRegionMem( stlAllocator  * aAllocator,
                             void         ** aAddr,
                             stlInt32        aAllocSize,
                             stlErrorStack * aErrorStack );

stlStatus stzClearRegionMem( stlAllocator  * aAllocator,
                             stlErrorStack * aErrorStack );

stlStatus stzMarkRegionMem( stlAllocator   * aAllocator,
                            stlRegionMark  * aMark,
                            stlErrorStack  * aErrorStack );

stlStatus stzRestoreRegionMem( stlAllocator   * aAllocator,
                               stlRegionMark  * aMark,
                               stlBool          aFree,
                               stlErrorStack  * aErrorStack );

stlStatus stzDestroyRegionMem( stlAllocator  * aAllocator,
                               stlErrorStack * aErrorStack );


stlStatus stzInitializeArrayAllocator( stlArrayAllocator    * aAllocator,
                                       stlInt64               aElementBodySize,
                                       stlInt64               aElementCount,
                                       stlInt64             * aTotalUsedBytes,
                                       stlSize                aAlignment,
                                       stlErrorStack        * aErrorStack );

stlStatus stzFinalizeArrayAllocator( stlArrayAllocator    * aAllocator,
                                     stlErrorStack        * aErrorStack );

stlStatus stzAllocElement( stlArrayAllocator   * aAllocator,
                           void               ** aItem,
                           stlErrorStack       * aErrorStack );

stlStatus stzFreeElement( stlArrayAllocator   * aAllocator,
                          void                * aItem,
                          stlErrorStack       * aErrorStack );

stlStatus stzGetInfoArray( stlArrayAllocator   * aAllocator,
                           stlInt32            * aTotalUsedBytes,
                           stlInt32            * aTotalElementCount,
                           stlInt32            * aUsedCount,
                           stlInt32            * aElementBodySize,
                           stlInt32            * aElementSize,
                           stlErrorStack       * aErrorStack );


STL_END_DECLS

#endif /* _STZ_H_ */
