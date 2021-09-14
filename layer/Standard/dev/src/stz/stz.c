/*******************************************************************************
 * stz.c
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

#include <stlDef.h>
#include <stlError.h>
#include <stlMemorys.h>
#include <stlAtomic.h>
#include <stlStrings.h>
#include <stlSemaphore.h>
#include <stlSpinLock.h>
#include <stz.h>


stlStatus stzAllocChunk( stlInt64         aSize,
                         void          ** aChunk,
                         stlErrorStack  * aErrorStack )
{
    void           * sAddr = NULL;
    stzChunkHeader * sChunkHeader = NULL;

    STL_TRY( stlAllocHeap( &sAddr,
                           aSize + STL_SIZEOF(stlUInt64),
                           aErrorStack )
             == STL_SUCCESS );

    sChunkHeader = (stzChunkHeader*)STL_ALIGN_DEFAULT((stlVarInt)sAddr);

    STZ_INIT_CHUNK( sChunkHeader, aSize, sAddr );

    *aChunk = sChunkHeader;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stzAllocRegionMem( stlAllocator  * aAllocator,
                             void         ** aAddr,
                             stlInt32        aAllocSize,
                             stlErrorStack * aErrorStack )
{
    stzChunkHeader * sCurChunkHeader = NULL;
    stzChunkHeader * sNxtChunkHeader = NULL;
    stlInt64         sAllocSize;
    
    sCurChunkHeader = (stzChunkHeader*)aAllocator->mCurChunk;

    while( STZ_CAN_ALLOC_MEM( sCurChunkHeader, aAllocSize ) == STL_FALSE )
    {
        sNxtChunkHeader = (stzChunkHeader*)sCurChunkHeader->mNextChunk;

        if( sNxtChunkHeader == NULL )
        {
            sAllocSize = STL_MAX( aAllocSize + STL_SIZEOF(stzChunkHeader),
                                  aAllocator->mNextSize );
            
            STL_TRY( stzAllocChunk( sAllocSize,
                                    &sCurChunkHeader->mNextChunk,
                                    aErrorStack )
                     == STL_SUCCESS );

            aAllocator->mTotalChunkCount += 1;
            aAllocator->mUsedChunkCount += 1;
            aAllocator->mLastChunk = sCurChunkHeader->mNextChunk;
            aAllocator->mCurChunk = sCurChunkHeader->mNextChunk;
            
            sCurChunkHeader = (stzChunkHeader*)sCurChunkHeader->mNextChunk;
        }
        else
        {
            sCurChunkHeader = sNxtChunkHeader;
        }
    }

    *aAddr = (stlChar*)sCurChunkHeader + sCurChunkHeader->mAllocHwm;
    sCurChunkHeader->mAllocHwm += aAllocSize;

    STL_ASSERT( sCurChunkHeader->mAllocHwm <= sCurChunkHeader->mTotalSize ); 
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stzClearRegionMem( stlAllocator  * aAllocator,
                             stlErrorStack * aErrorStack )
{
    stzChunkHeader * sChunk = NULL;
    stzChunkHeader * sNextChunk = NULL;
    
    sChunk = (stzChunkHeader*)aAllocator->mFirstChunk;
    sChunk = sChunk->mNextChunk;
    
    while( sChunk != NULL )
    {
        sNextChunk = sChunk->mNextChunk;
        stlFreeHeap( sChunk->mAddr );
        sChunk = sNextChunk;
    }

    aAllocator->mCurChunk = aAllocator->mFirstChunk;
    aAllocator->mLastChunk = aAllocator->mFirstChunk;
    aAllocator->mTotalChunkCount = 1;
    aAllocator->mUsedChunkCount = 1;

    STZ_INIT_CHUNK( (stzChunkHeader*)aAllocator->mFirstChunk,
                    ((stzChunkHeader*)aAllocator->mFirstChunk)->mTotalSize,
                    ((stzChunkHeader*)aAllocator->mFirstChunk)->mAddr);

    return STL_SUCCESS;
}

stlStatus stzMarkRegionMem( stlAllocator   * aAllocator,
                            stlRegionMark  * aMark,
                            stlErrorStack  * aErrorStack )
{
    aMark->mChunk = aAllocator->mCurChunk;
    aMark->mAllocHwm = ((stzChunkHeader*)aAllocator->mCurChunk)->mAllocHwm;
    aMark->mUsedChunkCount = aAllocator->mUsedChunkCount;
    
    return STL_SUCCESS;
}

stlStatus stzRestoreRegionMem( stlAllocator   * aAllocator,
                               stlRegionMark  * aMark,
                               stlBool          aFree,
                               stlErrorStack  * aErrorStack )
{
    stzChunkHeader * sMarkedChunk = NULL;
    stzChunkHeader * sChunk = NULL;
    stzChunkHeader * sNextChunk = NULL;
    
    sMarkedChunk = ((stzChunkHeader*)aMark->mChunk);

    sMarkedChunk->mAllocHwm = aMark->mAllocHwm;
    aAllocator->mUsedChunkCount = aMark->mUsedChunkCount;
    aAllocator->mCurChunk = sMarkedChunk;

    if( aFree == STL_FALSE )
    {
        /**
         * Mark된 Chunk를 제외한 이후 Chunk의 Alloc HWM를 초기화한다.
         */
        sChunk = sMarkedChunk->mNextChunk;
        
        while( sChunk != NULL )
        {
            sNextChunk = sChunk->mNextChunk;
            sChunk->mAllocHwm = STL_SIZEOF(stzChunkHeader);
            sChunk = sNextChunk;
        }
    }
    else
    {
        /**
         * Mark된 Chunk를 제외한 이후 Chunk를 시스템으로 반환한다.
         */
        sChunk = sMarkedChunk->mNextChunk;
        
        while( sChunk != NULL )
        {
            sNextChunk = sChunk->mNextChunk;
            stlFreeHeap( sChunk->mAddr );
            sChunk = sNextChunk;
        }

        sMarkedChunk->mNextChunk = NULL;
        aAllocator->mTotalChunkCount = aMark->mUsedChunkCount;
        aAllocator->mLastChunk = sMarkedChunk;
    }
    
    return STL_SUCCESS;
}

stlStatus stzDestroyRegionMem( stlAllocator  * aAllocator,
                               stlErrorStack * aErrorStack )
{
    stzChunkHeader * sChunk = NULL;
    stzChunkHeader * sNextChunk = NULL;
    
    sChunk = (stzChunkHeader*)(aAllocator->mFirstChunk);
    
    while( sChunk != NULL )
    {
        sNextChunk = sChunk->mNextChunk;
        stlFreeHeap( sChunk->mAddr );
        sChunk = sNextChunk;
    }

    aAllocator->mFirstChunk = NULL;
    aAllocator->mLastChunk = NULL;
    aAllocator->mCurChunk = NULL;
    aAllocator->mTotalChunkCount = 0;
    aAllocator->mUsedChunkCount = 0;

    return STL_SUCCESS;
}


stlStatus stzInitializeArrayAllocator( stlArrayAllocator    * aAllocator,
                                       stlInt64               aElementBodySize,
                                       stlInt64               aElementCount,
                                       stlInt64             * aTotalUsedBytes,
                                       stlSize                aAlignment,
                                       stlErrorStack        * aErrorStack )
{
    stlInt32            sIdx;
    stlArrayElement   * sElement;
    stlArrayElement   * sOldElement;

#ifdef __MEMORY_TEST__
    stlPrintf("memory test code enable..\n");
    aAllocator->mTestMem = 0;
#endif

    /* cache line padding */
    aAllocator->mElementSize = STL_ALIGN(aElementBodySize + STL_SIZEOF(stlArrayElement), aAlignment);
    aAllocator->mBodySize = aAllocator->mElementSize - STL_SIZEOF(stlArrayElement);

    aAllocator->mTotalElementCount = aElementCount;
    aAllocator->mUsedElementCount = 0;

    /* alloc, free시 loop을 안하도록 mNextFreeElement 설정 */
    sOldElement = NULL;
    sElement = STL_ARRAY_GET_FIRST_ELEMENT(aAllocator);

    for( sIdx = 0; sIdx < aAllocator->mTotalElementCount; sIdx++ )
    {
        /* mNextNotUsed은 이전 항목을 가르키도록 한다.*/
        sElement->mNextFreeElement = sOldElement;

        sOldElement = sElement;
        sElement = STL_ARRAY_GET_NEXT_ELEMENT( aAllocator, sElement );
    }

    /* mFirstFreeElement는 마지막 item을 가르키도록 한다 */
    aAllocator->mFirstFreeElement = sOldElement;

    /* allocator에서 사용하는 총 메모리 byte 구한다 */
    aAllocator->mTotalUsedBytes = ( STL_SIZEOF(stlArrayAllocator) +
                                    aAllocator->mElementSize *
                                    aAllocator->mTotalElementCount );

    *aTotalUsedBytes = aAllocator->mTotalUsedBytes;

    stlInitSpinLock( &aAllocator->mLock );

    return STL_SUCCESS;
}

stlStatus stzFinalizeArrayAllocator( stlArrayAllocator    * aAllocator,
                                     stlErrorStack        * aErrorStack )
{
    aAllocator->mTotalElementCount = -1;
    aAllocator->mUsedElementCount = -1;
    aAllocator->mElementSize = -1;
    aAllocator->mFirstFreeElement = NULL;
    aAllocator->mTotalUsedBytes = -1;

    stlFinSpinLock( &aAllocator->mLock );

    return STL_SUCCESS;
}

stlStatus stzAllocElement( stlArrayAllocator   * aAllocator,
                           void               ** aItem,
                           stlErrorStack       * aErrorStack )
{
    stlInt32            sLockState = 0;
    stlArrayElement   * sTemp;

#ifdef __MEMORY_TEST__
    /* 80번째 이후에 shared-server에서 error발생하도록 함 */
    if( aAllocator->mTestMem > 80 && __progname[1] == 's')
    {
        aAllocator->mTestMem = -1;
    }

    if( aAllocator->mTestMem == -1 )
    {
        STL_THROW( RAMP_ERR_OUT_OF_MEMORY );
    }
    aAllocator->mTestMem++;
#endif


    stlAcquireSpinLock( &aAllocator->mLock, NULL );
    sLockState = 1;

    sTemp = aAllocator->mFirstFreeElement;
    STL_TRY_THROW( sTemp != NULL, RAMP_ERR_OUT_OF_MEMORY );

    aAllocator->mFirstFreeElement = sTemp->mNextFreeElement;
    sTemp->mNextFreeElement = NULL;
    *aItem = STL_ARRAY_GET_BODY_FROM_HEADER(sTemp);
    aAllocator->mUsedElementCount++;

    sLockState = 0;
    stlReleaseSpinLock( &aAllocator->mLock );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_OUT_OF_MEMORY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_OUT_OF_MEMORY,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;

    switch( sLockState )
    {
        case 1:
            stlReleaseSpinLock( &aAllocator->mLock );
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * free된 element는 mFirstFreeElement에 넣지 않고 mLastFreeElement에 넣는다.
 * mFirstFreeElement에 넣으면 ABA problem 이 발생한다.
 */
stlStatus stzFreeElement( stlArrayAllocator   * aAllocator,
                          void                * aItem,
                          stlErrorStack       * aErrorStack )
{
    stlArrayElement   * sTemp;

    stlAcquireSpinLock( &aAllocator->mLock, NULL );

    aAllocator->mUsedElementCount--;

    sTemp = STL_ARRAY_GET_HEADER_FROM_BODY( aItem );

    sTemp->mNextFreeElement = aAllocator->mFirstFreeElement;
    aAllocator->mFirstFreeElement = sTemp;

    stlReleaseSpinLock( &aAllocator->mLock );

    return STL_SUCCESS;
}


stlStatus stzGetInfoArray( stlArrayAllocator   * aAllocator,
                           stlInt32            * aTotalUsedBytes,
                           stlInt32            * aTotalElementCount,
                           stlInt32            * aUsedCount,
                           stlInt32            * aElementBodySize,
                           stlInt32            * aElementSize,
                           stlErrorStack       * aErrorStack )
{
    stlInt32            sIdx;
    stlArrayElement   * sElement;
    stlInt32            sUsedElementCounts = 0;

    if( aTotalUsedBytes != NULL )
    {
        *aTotalUsedBytes = aAllocator->mTotalUsedBytes;
    }

    if( aTotalElementCount != NULL )
    {
        *aTotalElementCount = aAllocator->mTotalElementCount;
    }

    /* item size는 Element 크기에서 Element header를 뺀다 */
    if( aElementBodySize != NULL )
    {
        *aElementBodySize = aAllocator->mElementSize - STL_SIZEOF(stlArrayElement);
    }

    if( aElementSize != NULL )
    {
        *aElementSize = aAllocator->mElementSize;
    }

    if( aUsedCount != NULL )
    {
        /**
         * 사용중인 Element 개수 조회는mNextFreeElement를 따라가면 다른 process에서 변경이 가능함으로
         * 순서대로 모든 Element를 체크한다.
         */
        sElement = STL_ARRAY_GET_FIRST_ELEMENT(aAllocator);
        
        for( sIdx = 0; sIdx < aAllocator->mTotalElementCount; sIdx++ )
        {
            if( sElement->mNextFreeElement == NULL )
            {
                /* 사용중인 Element임. */
                sUsedElementCounts++;
            }

            sElement = STL_ARRAY_GET_NEXT_ELEMENT(aAllocator, sElement);
        }

        /* 맨처음 Element는 mNextFreeElement가 NULL로 설정되어 있음 */
        *aUsedCount = sUsedElementCounts - 1;
    }

    return STL_SUCCESS;
}

