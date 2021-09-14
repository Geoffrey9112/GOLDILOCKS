/*******************************************************************************
 * knlHeapQueue.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: knlHeapQueue.c 767 2011-05-18 08:22:01Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file knlHeapQueue.c
 * @brief Kernel Layer Heap Queue wrapper routines
 */

#include <stl.h>
#include <dtl.h>
#include <knlDef.h>

/**
 * @addtogroup knlHeapQueue
 * @{
 */

/**
 * @brief Heap Queue를 구성한다.
 * @param[in] aHeapQueueInfo Heap Queue 구조체
 * @param[in] aContext Compare 방법과 Iterating등을 기술한 정보
 * @param[in] aRunArray Sorting할 Run을 담은 구조체
 * @param[in] aArraySize aRunArray의 배열 개수
 * @param[in] aRunCount Run의 개수 
 * @param[in,out] aEnv environment 정보
 */
stlStatus knlHeapify( knlHeapQueueInfo    * aHeapQueueInfo,
                      knlHeapQueueContext * aContext,
                      knlHeapQueueEntry   * aRunArray,
                      stlInt64              aArraySize,
                      stlInt64              aRunCount,
                      knlEnv              * aEnv )
{
    stlInt64 i;
    stlInt64 j;
    stlInt64 sNeedArraySize;
    stlInt64 sLevelArraySize = 1;
    stlInt64 sWinner;
    stlInt32 sResult;
    stlInt32 sHeight = 0;

    /**
     * Queue Info 초기화
     */
    
    while( 1 )
    {
        sHeight++;
        if( sLevelArraySize >= aRunCount )
        {
            break;
        }
        sLevelArraySize <<= 1;
    }
    
    sNeedArraySize = ( sLevelArraySize * 2 );

    aHeapQueueInfo->mContext   = aContext;
    aHeapQueueInfo->mRunArray  = aRunArray;
    aHeapQueueInfo->mRunCount  = aRunCount;
    aHeapQueueInfo->mArraySize = sNeedArraySize;
    aHeapQueueInfo->mHeight    = sHeight;
    
    STL_ASSERT( sNeedArraySize <= aArraySize );

    for( i = 1; i < sNeedArraySize; i++ )
    {
        aRunArray[i].mItem = NULL;
        aRunArray[i].mIteratorIdx = -1;
    }

    /**
     * leaf 초기화
     */
    
    for( i = (sNeedArraySize >> 1), j = 0; j < aRunCount; i++, j++ )
    {
        STL_TRY( (aContext->mFetchNext)( aContext->mIterator,
                                         j,
                                         &aRunArray[i].mItem,
                                         aEnv )
                 == STL_SUCCESS );
        
        if( aRunArray[i].mItem == NULL )
        {
            aRunArray[i].mIteratorIdx = -1;
        }
        else
        {
            aRunArray[i].mIteratorIdx = j;
        }
    }

    /**
     * Internal 초기화
     */

    for( i = (sNeedArraySize >> 1); i > 1; i >>= 1 )
    {
        sLevelArraySize = (i << 1);
        
        for( j = i; j < sLevelArraySize; j += 2 )
        {
            if( aRunArray[j].mIteratorIdx != -1 )
            {
                if( aRunArray[j+1].mIteratorIdx != -1 )
                {
                    sResult = (aContext->mCompare)( aContext->mCompareInfo,
                                                    aRunArray[j].mItem,
                                                    aRunArray[j+1].mItem );
                    if( sResult < 0 )
                    {
                        sWinner = j;
                    }
                    else
                    {
                        sWinner = j + 1;
                    }
                }
                else
                {
                    sWinner = j;
                }
            }
            else
            {
                if( aRunArray[j+1].mIteratorIdx != -1 )
                {
                    sWinner = j + 1;
                }
                else
                {
                    /**
                     * 2개가 모두 없는 경우는 아무거나 올려준다.
                     */
                    sWinner = j;
                }
            }

            aRunArray[sWinner >> 1] = aRunArray[sWinner];
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Heap Queue에서 정렬된 Item을 얻는다.
 * @param[in] aHeapQueueInfo Heap Queue 구조체
 * @param[out] aItem 반환될 Item 주소
 * @param[out] aIteratorIdx 반환되는 heap의 idx
 * @param[in,out] aEnv environment 정보
 */
stlStatus knlGetMinHeap( knlHeapQueueInfo  * aHeapQueueInfo,
                         void             ** aItem,
                         stlInt64          * aIteratorIdx,
                         knlEnv            * aEnv )
{
    stlInt64              sLeft;
    stlInt64              sRight;
    stlInt64              i;
    stlInt64              sLeafIdx;
    void                * sItem;
    stlInt32              sIteratorIdx = -1;
    knlHeapQueueContext * sContext;
    knlHeapQueueEntry   * sRunArray;
    stlInt64              sWinner;
    stlInt32              sResult;
    
    sContext  = aHeapQueueInfo->mContext;
    sRunArray = aHeapQueueInfo->mRunArray;

    sItem = sRunArray[1].mItem;

    STL_TRY_THROW( sItem != NULL, RAMP_FINISH );
    
    sIteratorIdx = sRunArray[1].mIteratorIdx;
    sLeafIdx = (aHeapQueueInfo->mArraySize >> 1) + sIteratorIdx;

    STL_TRY( (sContext->mFetchNext)( sContext->mIterator,
                                     sIteratorIdx,
                                     &sRunArray[sLeafIdx].mItem,
                                     aEnv )
             == STL_SUCCESS );

    if( sRunArray[sLeafIdx].mItem == NULL )
    {
        sRunArray[sLeafIdx].mIteratorIdx = -1;
    }

    sLeft = sLeafIdx;

    /**
     * Propagate Item
     */
    
    for( i = 0; i < aHeapQueueInfo->mHeight - 1; i++ )
    {
        if( sLeft & STL_INT64_C(0x0000000000000001) )
        {
            sLeft = sLeft - 1;
        }
        
        sRight = sLeft + 1;
    
        if( sRunArray[sLeft].mIteratorIdx != -1 )
        {
            if( sRunArray[sRight].mIteratorIdx != -1 )
            {
                sResult = (sContext->mCompare)( sContext->mCompareInfo,
                                                sRunArray[sLeft].mItem,
                                                sRunArray[sRight].mItem );
                if( sResult < 0 )
                {
                    sWinner = sLeft;
                }
                else
                {
                    sWinner = sRight;
                }
            }
            else
            {
                sWinner = sLeft;
            }
        }
        else
        {
            if( sRunArray[sRight].mIteratorIdx != -1 )
            {
                sWinner = sRight;
            }
            else
            {
                /**
                 * 2개가 모두 없는 경우는 아무거나 올려준다.
                 */
                sWinner = sLeft;
            }
        }

        sRunArray[sWinner >> 1] = sRunArray[sWinner];
        sLeft = sLeft >> 1;
    }

    STL_RAMP( RAMP_FINISH );
    
    *aItem = sItem;

    if( aIteratorIdx != NULL )
    {
        *aIteratorIdx = sIteratorIdx;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Heap Queue에서 Top Entry를 얻는다.
 * @param[in] aHeapQueueInfo Heap Queue 구조체
 * @return top entry pointer
 */
knlHeapQueueEntry * knlGetMinQueueEntry( knlHeapQueueInfo * aHeapQueueInfo )
{
    return &(aHeapQueueInfo->mRunArray[1]);
}

/** @} */
