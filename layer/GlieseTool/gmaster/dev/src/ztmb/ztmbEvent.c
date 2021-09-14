/*******************************************************************************
 * ztmbEvent.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztmbEvent.c 5283 2012-08-10 03:01:37Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file ztmbEvent.c
 * @brief Gliese Master Event Routines
 */

#include <stl.h>
#include <knl.h>
#include <scl.h>
#include <sml.h>
#include <ell.h>
#include <qll.h>
#include <ssl.h>

#include <ztmDef.h>
#include <ztmb.h>
#include <ztmt.h>

extern ztmWarmupEntry ** gZtmWarmupEntry;
extern knlStartupFunc  * gZtmStartupLayer[];

stlStatus ztmbStartupEventHandler( void      * aData,
                                   stlUInt32   aDataSize,
                                   stlBool   * aDone,
                                   void      * aEnv )
{
    knlStartupPhase   sCurrStartupPhase;
    knlStartupInfo    sStartupInfo;
    stlBool           sSetRecovery = STL_FALSE;

    *aDone = STL_FALSE;
    
    STL_PARAM_VALIDATE( aData != NULL, KNL_ERROR_STACK(aEnv) );

    stlMemcpy( (void *)&sStartupInfo, aData, aDataSize );

    sCurrStartupPhase = knlGetCurrStartupPhase();

    STL_DASSERT( (sStartupInfo.mStartupPhase >= KNL_STARTUP_PHASE_NO_MOUNT) &&
                 (sStartupInfo.mStartupPhase < KNL_STARTUP_PHASE_MAX) );
    STL_TRY_THROW( sStartupInfo.mStartupPhase > sCurrStartupPhase,
                   RAMP_ERR_INVALID_STARTUP_PHASE );

    if( (sCurrStartupPhase == KNL_STARTUP_PHASE_NO_MOUNT) &&
        (sStartupInfo.mStartupPhase >= KNL_STARTUP_PHASE_MOUNT) )
    {
        STL_TRY( ztmbStartupPhase( KNL_STARTUP_PHASE_MOUNT,
                                   KNL_STARTUP_PHASE_MOUNT,
                                   &sStartupInfo,
                                   aEnv )
                 == STL_SUCCESS );
    }

    sCurrStartupPhase = knlGetCurrStartupPhase();
    
    if( (sCurrStartupPhase == KNL_STARTUP_PHASE_MOUNT) &&
        (sStartupInfo.mStartupPhase >= KNL_STARTUP_PHASE_OPEN) )
    {
        /**
         * MOUNT phase에서 불완전 복구가 수행되고 있을 때
         * Media recovery나 startup이 수행되어서는 안된다.
         */

        STL_TRY( smlSetRecoveryPhaseAtMount( SML_RECOVERY_AT_MOUNT_RECOVERY_BEGIN,
                                             SML_ENV( aEnv ) )
                 == STL_SUCCESS );

        sSetRecovery = STL_TRUE;

        /**
         * Database Access Mode 설정
         */

        if( sStartupInfo.mDataAccessMode == SML_DATA_ACCESS_MODE_NONE )
        {
            sStartupInfo.mDataAccessMode = knlGetPropertyBigIntValueByID(
                KNL_PROPERTY_DATABASE_ACCESS_MODE,
                KNL_ENV( aEnv ) ) + 1;
        }

        STL_TRY( ztmbStartupPhase( KNL_STARTUP_PHASE_PREOPEN,
                                   KNL_STARTUP_PHASE_OPEN,
                                   &sStartupInfo,
                                   aEnv )
                 == STL_SUCCESS );
        
        /**
         * SAR(System Available Resource)에 SESSION 항목을 추가한다.
         * 이 이후로는 실패가 발생하더라도 OPEN상태로 유지된다.
         */
        
        STL_TRY( knlAddSar( KNL_SAR_SESSION,
                            KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    
    *aDone = STL_TRUE;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_STARTUP_PHASE )
    {
        if( sCurrStartupPhase == KNL_STARTUP_PHASE_MOUNT )
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          QLL_ERRCODE_INVALID_MOUNTABLE_PHASE,
                          NULL,
                          KNL_ERROR_STACK(aEnv) );
        }
        else
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          QLL_ERRCODE_INVALID_OPENABLE_PHASE,
                          NULL,
                          KNL_ERROR_STACK(aEnv) );
        }
    }

    STL_FINISH;

    if( sSetRecovery == STL_TRUE )
    {
        (void)smlSetRecoveryPhaseAtMount( SML_RECOVERY_AT_MOUNT_RECOVERY_END,
                                          SML_ENV( aEnv ) );
    }

    return STL_FAILURE;
}

stlStatus ztmbShutdownEventHandler( void      * aData,
                                    stlUInt32   aDataSize,
                                    stlBool   * aDone,
                                    void      * aEnv )
{
    knlShutdownEventArg sShutdownEventArg;
    knlShutdownInfo     sShutdownInfo;
    knlStartupPhase     sCurrStartupPhase;
    knlShutdownPhase    sShutdownPhase;
    
    *aDone = STL_FALSE;
    
    STL_PARAM_VALIDATE( aData != NULL, KNL_ERROR_STACK(aEnv) );

    stlMemcpy( (void *)&sShutdownEventArg, aData, aDataSize );

    sShutdownPhase = sShutdownEventArg.mShutdownPhase;
    sCurrStartupPhase = knlGetCurrStartupPhase();

    KNL_INIT_SHUTDOWN_INFO( &sShutdownInfo );
    sShutdownInfo.mShutdownMode = sShutdownEventArg.mShutdownMode;

    /**
     * OPEN Phase에서 shtudown close 혹은
     * Media recovery 수행 후 shutdown close
     * OPEN Phase에서 shtudown close
     */
    if( sCurrStartupPhase == KNL_STARTUP_PHASE_OPEN )
    {
        if( sShutdownPhase <= KNL_SHUTDOWN_PHASE_CLOSE )
        {
            STL_TRY( ztmbShutdownPhase( KNL_SHUTDOWN_PHASE_CLOSE,
                                        KNL_SHUTDOWN_PHASE_POSTCLOSE,
                                        &sShutdownInfo,
                                        aEnv )
                     == STL_SUCCESS );
        }
    }

    sCurrStartupPhase = knlGetCurrStartupPhase();
    
    /**
     * MOUNT Phase에서 shtudown dismount
     */
    if( sCurrStartupPhase == KNL_STARTUP_PHASE_MOUNT )
    {
        if( sShutdownPhase <= KNL_SHUTDOWN_PHASE_DISMOUNT )
        {
            STL_TRY( ztmbShutdownPhase( KNL_SHUTDOWN_PHASE_DISMOUNT,
                                        KNL_SHUTDOWN_PHASE_DISMOUNT,
                                        &sShutdownInfo,
                                        aEnv )
                     == STL_SUCCESS );
        }
    }

    /**
     * shtudown init
     */
    if( sShutdownPhase <= KNL_SHUTDOWN_PHASE_INIT )
    {
        if( sShutdownEventArg.mShutdownMode == KNL_SHUTDOWN_MODE_ABORT )
        {
            (*gZtmWarmupEntry)->mServerStatus = ZTM_SERVER_STATUS_SHUTDOWN_ABORT;
        }
        else
        {
            (*gZtmWarmupEntry)->mServerStatus = ZTM_SERVER_STATUS_SHUTDOWN;
        }
    }

    *aDone = STL_TRUE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztmbCleanupEventHandler( void      * aData,
                                   stlUInt32   aDataSize,
                                   stlBool   * aDone,
                                   void      * aEnv )
{
    *aDone = STL_FALSE;
    
    STL_TRY( ztmtCleanupDeadEnv( aEnv ) == STL_SUCCESS );

    *aDone = STL_TRUE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

