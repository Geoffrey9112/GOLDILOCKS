/*******************************************************************************
 * sty.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: sty.h 2998 2011-12-27 09:13:23Z xcom73 $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _STY_H_
#define _STY_H_ 1

STL_BEGIN_DECLS

/**
 * @brief 동적 메모리 chunk의 헤더 
 */
typedef struct styChunkHeader
{
    stlRingHead  mAllocRing;      /**< 할당된 block들의 ring */
    stlRingHead  mFreeRing;       /**< 반환된 block들의 ring*/
    stlRingEntry mLink;           /**< 할당된 chunk간의 연결 */
    stlUInt64    mSize;           /**< 할당된 chunk의 크기 */
    stlUInt64    mFreeBlockSize;  /**< 반환된 block들의 크기 */
    stlUInt32    mFreeBlockCount; /**< 반환된 block 갯수 */
    stlUInt32    mAlign;          /**< 8byte align을 위한 padding */
} styChunkHeader;

/**
 * @brief 동적 메모리 block의 헤더 
 */
typedef struct styBlockHeader
{
    stlRingEntry mAllocLink; /**< 할당된 block들의 연결 */
    stlRingEntry mFreeLink;  /**< 반환된 block들의 연결 */
    stlUInt32    mSize;      /**< 할당된 block의 크기 */
    stlUInt32    mIsUsed;    /**< 해당 block의 사용 여부 */
} styBlockHeader;

stlStatus styAllocChunk( stlDynamicAllocator   * aAllocator,
                         styChunkHeader       ** aChunkHeader,
                         stlInt64                aChunkSize,
                         stlErrorStack         * aErrorStack );

stlStatus styFreeChunk( stlDynamicAllocator  * aAllocator,
                        styChunkHeader       * aChunkHeader,
                        stlErrorStack        * aErrorStack );

stlStatus styFreeAllChunk( stlDynamicAllocator  * aAllocator,
                           stlErrorStack        * aErrorStack );

stlStatus styFindInFreeList( styChunkHeader  * aChunkHeader,
                             stlUInt32         aSize,
                             styBlockHeader ** aFreeBlock,
                             stlErrorStack   * aErrorStack );

stlStatus styAllocInFreeBlock( styChunkHeader  * aChunkHeader,
                               stlUInt32         aSize,
                               styBlockHeader  * aFreeBlock,
                               styBlockHeader ** aAllocBlock,
                               stlErrorStack   * aErrorStack );

stlStatus styAllocInChunk( styChunkHeader  * aChunkHeader,
                           stlUInt32         aSize,
                           styBlockHeader ** aAllocBlock,
                           stlErrorStack   * aErrorStack );

stlStatus styFindFreeSpace( stlDynamicAllocator  * aAllocator,
                            stlUInt32              aSize,
                            styBlockHeader      ** aFreeBlock,
                            stlErrorStack        * aErrorStack );

stlStatus styFindChunk( stlDynamicAllocator  * aAllocator,
                        styBlockHeader       * aBlockHeader,
                        styChunkHeader      ** aChunkHeader,
                        stlBool              * aIsDefaultChunk,
                        stlErrorStack        * aErrorStack );

stlStatus styAlloc( stlDynamicAllocator  * aAllocator,
                    stlUInt32              aSize,
                    void                ** aAddr,
                    stlErrorStack        * aErrorStack );

stlStatus styFree( stlDynamicAllocator  * aAllocator,
                   void                 * aAddr,
                   stlErrorStack        * aErrorStack );

STL_END_DECLS

#endif /* _STY_H_ */
