/*******************************************************************************
 * cmgMemory.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: cmlMemory.c 6676 2012-12-13 08:39:46Z egon $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stl.h>
#include <dtl.h>
#include <cmlDef.h>
#include <cmDef.h>

/**
 * @file cmgMemory.c
 * @brief Communication Layer Memory Component Routines
 */

/**
 * @addtogroup cmlGeneral
 * @{
 */

/**
 * @brief Operation Memory 로 부터 주어진 크기만큼 버퍼를 할당한다.
 * @param[in]  aHandle          handle
 * @param[in]  aSize            할닫 크기
 * @param[out] aMemory          할당한 버퍼
 * @param[in]  aErrorStack      에러 스택
 */
stlStatus cmgAllocOperationMem( cmlHandle          * aHandle,
                                stlInt64             aSize,
                                void              ** aMemory,
                                stlErrorStack      * aErrorStack )
{
    cmlMemoryManager       * sMemoryMgr = aHandle->mProtocolSentence->mMemoryMgr;

    STL_DASSERT( sMemoryMgr->mOperationBufferSize != 0 );

    if( sMemoryMgr->mOperationBufferSize < aSize )
    {
        STL_TRY( stlRestoreRegionMem( &sMemoryMgr->mAllocator,
                                      &sMemoryMgr->mOperationMark,
                                      STL_TRUE,  /* aIsFree */
                                      aErrorStack )
                 == STL_SUCCESS );
        
        STL_TRY( stlAllocRegionMem( &sMemoryMgr->mAllocator,
                                    aSize,
                                    (void**)&(sMemoryMgr->mOperationBuffer),
                                    aErrorStack )
                 == STL_SUCCESS );
        
        sMemoryMgr->mOperationBufferSize = aSize;
    }

    *aMemory = sMemoryMgr->mOperationBuffer;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
