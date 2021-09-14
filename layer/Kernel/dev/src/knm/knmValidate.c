/*******************************************************************************
 * knmValidate.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: knmValidate.c 2239 2011-11-07 11:26:58Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file knmValidate.c
 * @brief Kernel Layer Memory Validate Internal Routines
 */

#include <stl.h>
#include <dtl.h>
#include <knlDef.h>
#include <knDef.h>
#include <knlTraceLogger.h>
#include <knlProperty.h>
#include <knmValidate.h>

/**
 * @addtogroup knmValidate
 * @{
 */

/**
 * @brief Memory Fence와 Memory Body를 초기화한다.
 * @param[in] aHeadFence Memory Head Fence
 * @param[in] aSize 할당할 메모리 크기
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knmInitMemory( knmHeadFence * aHeadFence,
                         stlUInt32      aSize,
                         knlEnv       * aEnv )
{
    knmTailFence * sTailFence;
    stlChar      * sBody;

    sTailFence = (knmTailFence*)((stlChar*)aHeadFence +
                                 STL_SIZEOF(knmHeadFence) +
                                 aSize );
    sBody = (stlChar*)(aHeadFence + 1);

    /**
     * 가비지 값으로 인한 regression 테스트의 어려움을 해결하기 위해서
     * 임의의 고정된 가비값을 설정한다.
     * 
     * 주의) 임의의 값설정은 valgrind error를 hidden시킬수 있다.
     */
#ifndef STL_VALGRIND
    stlMemset( sBody, KNM_MEMORY_INIT_VALUE, aSize );
#endif

    /**
     * shuffle test에 한해서 callstack을 저장한다.
     */
    if( (KNL_IS_SHARED_ENV(aEnv) == STL_TRUE) &&
        (KNL_SESS_ENV(aEnv) != NULL) &&
        (knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_TEST_OPTION, aEnv ) == 1) )
    {
        /**
         * stlBacktrace는 시간이 오래걸리는 함수이기 때문에 활성화시 이를 감안해야 한다.
         */
        aHeadFence->mCallstackCount = stlBacktrace( aHeadFence->mCallstack,
                                                    KNM_MAX_CALLSTACK_DEPTH,
                                                    NULL,
                                                    NULL );
    }
    else
    {
        aHeadFence->mCallstackCount = 0;
    }

    aHeadFence->mSize = aSize;
    aHeadFence->mMagicNum = KNM_FENCE_MAGIC_NUMBER;
    sTailFence->mMagicNum = KNM_FENCE_MAGIC_NUMBER;
    
    return STL_SUCCESS;
}

/**
 * @brief Area 전체에 대해서 Memory Fence의 무결성을 검증한다.
 * @param[in] aArenaHeader Arena Header
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knmValidateArena( knmArenaHeader * aArenaHeader,
                            knlEnv         * aEnv )
{
    stlUInt32      sReadSize = 0;
    knmHeadFence * sHeadFence;
    
    while( sReadSize < aArenaHeader->mAllocOffset )
    {
        sHeadFence = (knmHeadFence*)( (stlChar*)aArenaHeader +
                                      STL_SIZEOF(knmArenaHeader) +
                                      sReadSize );
        
        STL_TRY( knmValidateFence( sHeadFence,
                                   aEnv )
                 == STL_SUCCESS );
        
        sReadSize += ( STL_SIZEOF(knmHeadFence) +
                       STL_SIZEOF(knmTailFence) +
                       sHeadFence->mSize );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Memory Fence의 무결성을 검증한다.
 * @param[in] aHeadFence Memory Head Fence
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knmValidateFence( knmHeadFence * aHeadFence,
                            knlEnv       * aEnv )
{
    knmTailFence * sTailFence;
    stlChar        sBuffer[8192];

    sTailFence = (knmTailFence*)( (stlChar*)aHeadFence +
                                  STL_SIZEOF(knmHeadFence) +
                                  aHeadFence->mSize );
    
    if( (aHeadFence->mMagicNum != KNM_FENCE_MAGIC_NUMBER) ||
        (sTailFence->mMagicNum != KNM_FENCE_MAGIC_NUMBER) )
    {
        stlBacktraceSymbolsToStr( aHeadFence->mCallstack,
                                  sBuffer,
                                  aHeadFence->mCallstackCount,
                                  8192 );
        STL_TRY( knlLogMsg( NULL,
                            aEnv,
                            KNL_LOG_LEVEL_INFO,
                            "%s",
                            sBuffer )
                 == STL_SUCCESS );
                
        STL_ASSERT( 0 );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
