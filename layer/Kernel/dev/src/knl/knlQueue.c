/*******************************************************************************
 * knlQueue.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: knlQueue.c 767 2011-05-18 08:22:01Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file knlQueue.c
 * @brief Kernel Layer Queue wrapper routines
 */

#include <stl.h>
#include <dtl.h>
#include <knlDef.h>
#include <knDef.h>
#include <knlDynamicMem.h>

/**
 * @addtogroup knlQueue
 * @{
 */

#define KNL_QUEUE_FRONT_POS( aQueueInfo )                                               \
    ( (aQueueInfo)->mArray +                                                            \
      (((aQueueInfo)->mFrontSn % (aQueueInfo)->mArraySize) * (aQueueInfo)->mItemSize ) )

#define KNL_QUEUE_REAR_POS( aQueueInfo )                                                \
    ( (aQueueInfo)->mArray +                                                            \
      (((aQueueInfo)->mRearSn % (aQueueInfo)->mArraySize) * (aQueueInfo)->mItemSize ) )

#define KNL_QUEUE_BUFF_POS( aArray, aArraySize, aSn, aItemSize )    \
    ( (aArray) + (((aSn) % (aArraySize)) * (aItemSize)) )

/**
 * @brief Queue를 초기화 한다.
 * @param[in] aQueueInfo Queue 구조체
 * @param[in] aInitArraySize 초기 배열 크기
 * @param[in] aItemSize Queue에 저장될 item의 크기 
 * @param[in] aDynamicMem Array가 부족할 경우 사용될 Allocator
 * @param[in,out] aEnv environment 정보
 */
stlStatus knlInitQueue( knlQueueInfo  * aQueueInfo,
                        stlInt64        aInitArraySize,
                        stlInt64        aItemSize,
                        knlDynamicMem * aDynamicMem,
                        knlEnv        * aEnv )
{
    aQueueInfo->mDynamicMem = aDynamicMem;
    aQueueInfo->mRearSn = 0;
    aQueueInfo->mFrontSn = 0;
    aQueueInfo->mArraySize = aInitArraySize;
    aQueueInfo->mItemSize = aItemSize;
    
    STL_TRY( knlAllocDynamicMem( aDynamicMem,
                                 aInitArraySize * aItemSize,
                                 (void**)&aQueueInfo->mArray,
                                 aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Queue를 종료시킨다.
 * @param[in] aQueueInfo Queue 구조체
 * @param[in,out] aEnv environment 정보
 */
stlStatus knlFiniQueue( knlQueueInfo  * aQueueInfo,
                        knlEnv        * aEnv )
{
    STL_TRY( knlFreeDynamicMem( aQueueInfo->mDynamicMem,
                                aQueueInfo->mArray,
                                aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Queue Front에 Item을 삽입한다.
 * @param[in] aQueueInfo Queue 구조체
 * @param[in] aItem 저장될 Item 주소
 * @param[in,out] aEnv environment 정보
 */
stlStatus knlEnqueue( knlQueueInfo * aQueueInfo,
                      void         * aItem,
                      knlEnv       * aEnv )
{
    stlChar  * sOrgArray;
    stlInt64   sOrgArraySize;
    stlInt64   sOrgFrontSn;
    stlInt64   sOrgRearSn;
    stlInt64   sOrgItemSize;

    if( (aQueueInfo->mFrontSn - aQueueInfo->mRearSn) >= aQueueInfo->mArraySize )
    {
        sOrgArray = aQueueInfo->mArray;
        sOrgArraySize = aQueueInfo->mArraySize;
        sOrgItemSize = aQueueInfo->mItemSize;
        
        aQueueInfo->mArraySize *= 2;
        
        STL_TRY( knlAllocDynamicMem( aQueueInfo->mDynamicMem,
                                     aQueueInfo->mArraySize * aQueueInfo->mItemSize,
                                     (void**)&aQueueInfo->mArray,
                                     aEnv )
                 == STL_SUCCESS );

        sOrgFrontSn = aQueueInfo->mFrontSn;
        sOrgRearSn  = aQueueInfo->mRearSn;

        aQueueInfo->mRearSn = 0;
        aQueueInfo->mFrontSn = 0;

        while( sOrgFrontSn != sOrgRearSn )
        {
            STL_TRY( knlEnqueue( aQueueInfo,
                                 KNL_QUEUE_BUFF_POS( sOrgArray,
                                                     sOrgArraySize,
                                                     sOrgRearSn,
                                                     sOrgItemSize ),
                                 aEnv )
                     == STL_SUCCESS );

            sOrgRearSn++;
        }
        
        STL_TRY( knlFreeDynamicMem( aQueueInfo->mDynamicMem,
                                    sOrgArray,
                                    aEnv )
                 == STL_SUCCESS );
    }

    stlMemcpy( KNL_QUEUE_FRONT_POS( aQueueInfo ),
               aItem,
               aQueueInfo->mItemSize );
    aQueueInfo->mFrontSn++;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Queue에서 Rear Item을 반환한다.
 * @param[in] aQueueInfo Queue 구조체
 * @param[out] aItem 반환될 Item 주소
 * @param[out] aIsEmpty Queue가 비었는지 여부
 * @param[in,out] aEnv environment 정보
 */
stlStatus knlDequeue( knlQueueInfo * aQueueInfo,
                      void         * aItem,
                      stlBool      * aIsEmpty,
                      knlEnv       * aEnv )
{
    *aIsEmpty = STL_FALSE;
    
    if( aQueueInfo->mRearSn != aQueueInfo->mFrontSn )
    {
        stlMemcpy( aItem,
                   KNL_QUEUE_REAR_POS( aQueueInfo ),
                   aQueueInfo->mItemSize );
        aQueueInfo->mRearSn++;
    }
    else
    {
        *aIsEmpty = STL_TRUE;
    }

    return STL_SUCCESS;
}

/**
 * @brief 현재 Queue에 저장되어 있는 Item의 개수를 반환한다.
 * @param[in] aQueueInfo Queue 구조체
 */
stlInt64 knlGetQueueSize( knlQueueInfo * aQueueInfo )
{
    return (aQueueInfo->mFrontSn - aQueueInfo->mRearSn);
}

/** @} */
