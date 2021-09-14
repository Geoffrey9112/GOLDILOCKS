/*******************************************************************************
 * knlTimer.c
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
 * @file knlTimer.c
 * @brief Kernel Layer Timer Routines
 */

#include <stl.h>
#include <dtl.h>
#include <knlDef.h>
#include <knDef.h>

extern knsEntryPoint * gKnEntryPoint;

/**
 * @addtogroup knlTimer
 * @{
 */

/**
 * @brief System Timer에 설정되어 있는 시간을 얻는다.
 */
stlTime knlGetSystemTime()
{
    STL_ASSERT( gKnEntryPoint != NULL );
    
    return gKnEntryPoint->mSystemTime;
}

/**
 * @brief System time을 갱신하기 위한 Signal Handler.
 * @param[in] aSigNo   Signal Number
 * @param[in] aSigInfo Signal Information
 * @param[in] aContext 이전 Context 정보
 */
void knlUpdateSystemTimer( stlInt32   aSigNo,
                           void     * aSigInfo,
                           void     * aContext )
{
    if( gKnEntryPoint != NULL )
    {
        gKnEntryPoint->mSystemTime = stlNow();
    }
}

/**
 * @brief System timer를 등록한다.
 * @param[in] aEnv Environment Pointer
 */
stlStatus knlRegisterSystemTimer( knlEnv * aEnv )
{
    STL_ASSERT( gKnEntryPoint != NULL );
    
    STL_TRY( stlSetSignalHandlerEx( STL_SIGNAL_RTMIN,
                                    knlUpdateSystemTimer,
                                    NULL,  /* aOldHandler */
                                    KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    STL_TRY( stlCreateTimer( STL_SIGNAL_RTMIN,
                             KNL_SYSTEM_TIME_PRECISION,
                             &gKnEntryPoint->mSystemTimerId,
                             KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
                             
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 등록된 System timer를 해제한다.
 * @param[in] aEnv Environment Pointer
 */
stlStatus knlUnregisterSystemTimer( knlEnv * aEnv )
{
    STL_ASSERT( gKnEntryPoint != NULL );
    
    STL_TRY( stlDestroyTimer( gKnEntryPoint->mSystemTimerId,
                              KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
                             
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
