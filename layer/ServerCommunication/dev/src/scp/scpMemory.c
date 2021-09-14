/*******************************************************************************
 * scpMemory.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: scpMemory.c 6676 2012-12-13 08:39:46Z egon $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stl.h>
#include <dtl.h>
#include <cml.h>
#include <knl.h>
#include <sclDef.h>
#include <scDef.h>
#include <cmlGeneral.h>
#include <sccCommunication.h>

/**
 * @file scpMemory.c
 * @brief Communication Layer Memory Component Routines
 */

/**
 * @addtogroup scpMemory
 * @{
 */

/**
 * @brief Operation Memory 로 부터 주어진 크기만큼 버퍼를 할당한다.
 * @param[in]  aHandle          handle
 * @param[in]  aSize            할닫 크기
 * @param[out] aMemory          할당한 버퍼
 * @param[in]  aEnv             env
 */
stlStatus scpAllocOperationMem( sclHandle          * aHandle,
                                stlInt64             aSize,
                                void              ** aMemory,
                                sclEnv             * aEnv )
{
    sclMemoryManager       * sMemoryMgr = aHandle->mProtocolSentence->mMemoryMgr;

    STL_DASSERT( sMemoryMgr->mOperationBufferSize != 0 );

    if( sMemoryMgr->mOperationBufferSize < aSize )
    {
        STL_TRY( knlFreeUnmarkedRegionMem( &sMemoryMgr->mAllocator,
                                           &sMemoryMgr->mOperationMark,
                                           STL_TRUE,  /* aIsFree */
                                           KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        STL_TRY( knlAllocRegionMem( &sMemoryMgr->mAllocator,
                                    aSize,
                                    (void**)&(sMemoryMgr->mOperationBuffer),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sMemoryMgr->mOperationBufferSize = aSize;
    }

    *aMemory = sMemoryMgr->mOperationBuffer;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} */

