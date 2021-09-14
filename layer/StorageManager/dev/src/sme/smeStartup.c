/*******************************************************************************
 * smeStartup.c
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

#include <stl.h>
#include <dtl.h>
#include <knl.h>
#include <scl.h>
#include <smlDef.h>
#include <smDef.h>
#include <smrDef.h>
#include <smg.h>
#include <smr.h>
#include <sme.h>
#include <smf.h>
#include <smxl.h>

/**
 * @file smeStartup.c
 * @brief Storage Manager Layer Access Component Startup Internal Routines
 */

extern smrRedoVector gSmeRedoVectors[];

/**
 * @addtogroup sme
 * @{
 */

/**
 * @brief Access Component를 NO MOUNT 모드로 전이한다.
 * @param[out] aWarmupEntry Access Component의 Warmup Entry 포인터 
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smeStartupNoMount( void  ** aWarmupEntry,
                             smlEnv * aEnv )
{
    smrRegisterRedoVectors( SMG_COMPONENT_RELATION,
                            gSmeRedoVectors );

    return STL_SUCCESS;
}

/**
 * @brief Access Component를 OPEN 모드로 전이한다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smeStartupOpen( smlEnv * aEnv )
{
    if( smfGetServerState() != SML_SERVER_STATE_NONE )
    {
        if( knlGetPropertyBigIntValueByID( KNL_PROPERTY_REFINE_RELATION,
                                           KNL_ENV(aEnv) )
            == STL_TRUE )
        {
            STL_TRY( knlLogMsg( NULL,
                                KNL_ENV(aEnv),
                                KNL_LOG_LEVEL_INFO,
                                "[STARTUP-SM] REFINE RELATIONS\n" )
                     == STL_SUCCESS );

            STL_TRY( smeRefineAtStartup( aEnv ) == STL_SUCCESS );
        }
        else
        {
            STL_TRY( knlLogMsg( NULL,
                                KNL_ENV(aEnv),
                                KNL_LOG_LEVEL_INFO,
                                "[STARTUP-SM] skip relation refining\n" )
                     == STL_SUCCESS );
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Access Component를 Warmup 시킨다.
 * @param[in] aWarmupEntry Access Component의 Warmup Entry 포인터 
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smeWarmup( void   * aWarmupEntry,
                     smlEnv * aEnv )
{
    smrRegisterRedoVectors( SMG_COMPONENT_RELATION,
                            gSmeRedoVectors );
    
    return STL_SUCCESS;
}

/**
 * @brief Access Component를 Cooldown 시킨다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smeCooldown( smlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief Access Component를 Bootdown 시킨다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smeBootdown( smlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief Access Component를 Close 시킨다.
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smeShutdownClose( smlEnv * aEnv )
{
    return STL_SUCCESS;
}

/** @} */
