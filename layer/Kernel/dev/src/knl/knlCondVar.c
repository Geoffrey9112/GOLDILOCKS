/*******************************************************************************
 * knlCondVar.c
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

/**
 * @file knlCondVar.c
 * @brief Kernel Layer conditional variable wrapper routines
 */

#include <stl.h>
#include <dtl.h>
#include <knlDef.h>
#include <knnCondVar.h>

/**
 * @brief 주어진 CondVar 객체의 멤버변수들을 초기화 한다
 * @param[in] aCondVar 초기화할 CondVar 객체 포인터
 * @param[in,out] aEnv environment 정보
 */
stlStatus knlInitCondVar( knlCondVar * aCondVar,
                          knlEnv     * aEnv )
{
    return knnInitCondVar( aCondVar, aEnv );
}

/**
 * @brief CondVar 객체의 사용을 종료한다.
 * @param[in] aCondVar CondVar 객체 포인터
 */
void knlFinCondVar( knlCondVar * aCondVar )
{
    return knnFinCondVar( aCondVar );
}

/**
 * @brief 주어진 CondVar 객체에 Signal이 전달될때까지 기다린다.
 * @param[in] aCondVar CondVar 객체 포인터
 * @param[in] aTimeout 대기할 최대 시간(microsecond)
 * @param[out] aIsTimedOut Timeout 발생 여부
 * @param[in,out] aEnv environment 정보
 */
stlStatus knlCondWait( knlCondVar   * aCondVar,
                       stlInterval    aTimeout,
                       stlBool      * aIsTimedOut,
                       knlEnv       * aEnv )
{
    return knnCondWait( aCondVar,
                        aTimeout,
                        aIsTimedOut,
                        aEnv );
}

/**
 * @brief 주어진 CondVar 객체에 대기하고 있는 모든 쓰레드/프로세스를 깨운다.
 * @param[in] aCondVar CondVar 객체 포인터
 * @param[in,out] aEnv environment 정보
 */
stlStatus knlCondBroadcast( knlCondVar  * aCondVar,
                            knlEnv      * aEnv )
{
    return knnCondBroadcast( aCondVar, aEnv );
}

/** @} */
