/*******************************************************************************
 * knlMemController.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: knlMemController.c 8991 2013-07-15 03:33:32Z enigma $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file knlMemController.c
 * @brief Kernel Layer Dynamic Memory Controller
 */

#include <stl.h>
#include <dtl.h>
#include <knlDef.h>
#include <knlRing.h>
#include <knlLatch.h>
#include <knDef.h>
#include <knmDynamic.h>
#include <knsSegment.h>
#include <knlSession.h>

extern knsEntryPoint * gKnEntryPoint;

/**
 * @addtogroup knlMemController
 * @{
 */

/**
 * @brief Memory Controller를 초기화 한다.
 * @param[in,out]  aMemController   Memory Controller
 * @param[in]      aMaxSize         할당할수 있는 메모리의 총 크기
 * @param[in,out]  aEnv             커널 Environment
 */
stlStatus knlInitMemController( knlMemController * aMemController,
                                stlUInt64          aMaxSize,
                                knlEnv           * aEnv )
{
    STL_PARAM_VALIDATE( aMemController != NULL, KNL_ERROR_STACK( aEnv ));
    
    aMemController->mMaxSize = aMaxSize;
    aMemController->mTotalSize = 0;

    STL_TRY( knlInitLatch( &(aMemController->mLatch),
                           STL_TRUE,  /* aIsInShm */
                           aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Memory Controller를 종료 한다.
 * @param[in]      aMemController   Memory Controller
 * @param[in,out]  aEnv             커널 Environment
 */
stlStatus knlFiniMemController( knlMemController * aMemController,
                                knlEnv           * aEnv )
{
    knlFinLatch( &(aMemController->mLatch) );

    return STL_SUCCESS;
}

/** @} */
