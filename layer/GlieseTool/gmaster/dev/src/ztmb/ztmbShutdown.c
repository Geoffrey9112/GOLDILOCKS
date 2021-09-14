/*******************************************************************************
 * ztmbShutdown.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztmbShutdown.c 5283 2012-08-10 03:01:37Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file ztmbShutdown.c
 * @brief Gliese Master Shutdown Routines
 */

#include <stl.h>
#include <knl.h>
#include <scl.h>
#include <sml.h>
#include <ell.h>
#include <qll.h>
#include <ssl.h>
#include <sll.h>
#include <ztmDef.h>
#include <ztmb.h>
#include <ztmt.h>

extern ztmEnv              gZtmSystemEnv;
extern ztmWarmupEntry   ** gZtmWarmupEntry;
extern knlEventTable       gZtmbEventTable[];
extern sllSharedMemory   * gZtmSharedMemory;
extern ztmThread           gZtmProcessMonitorThread;

/**
 * @brief Gliese Master의 다단계 shutdown 함수
 */
knlShutdownFunc gShutdownZT[KNL_SHUTDOWN_PHASE_MAX] =
{
    NULL,
    ztmbShutdownInit,
    ztmbShutdownDismount,
    NULL,
    ztmbShutdownClose
};

/**
 * @brief Layer 별 shutdown 함수
 */
knlShutdownFunc * gZtmShutdownLayer[STL_LAYER_MAX] = { NULL,         /* STL_LAYER_NONE */
                                                       NULL,         /* STL_LAYER_STANDARD */
                                                       NULL,         /* STL_LAYER_DATATYPE */
                                                       NULL,         /* STL_LAYER_COMMUNICATION */
                                                       gShutdownKN,  /* STL_LAYER_KERNEL */
                                                       gShutdownSC,  /* STL_LAYER_SERVER_COMMUNICATION */
                                                       gShutdownSM,  /* STL_LAYER_STORAGE_MANAGER */
                                                       gShutdownEL,  /* STL_LAYER_EXECUTION_LIBRARY */
                                                       gShutdownQP,  /* STL_LAYER_SQL_PROCESSOR */
                                                       NULL,         /* STL_LAYER_PSM_MANAGER */
                                                       gShutdownSS,  /* STL_LAYER_SESSION */
                                                       gShutdownSL,  /* STL_LAYER_SERVER_LIBRARY */
                                                       NULL,         /* STL_LAYER_GLIESE_LIBRARY */
                                                       gShutdownZT,  /* STL_LAYER_GLIESE_TOOL */
                                                       NULL,         /* STL_LAYER_GOLDILOCKS_LIBRARY */
                                                       NULL };       /* STL_LAYER_GOLDILOCKS_TOOL */


/**
 * @brief Gliese Master를 INIT 상태로 전환시킨다.
 * @param[in] aShutdownInfo 종료 시 정보
 * @param[out] aEnv Environment 구조체
 */
stlStatus ztmbShutdownInit( knlShutdownInfo * aShutdownInfo,
                            void           * aEnv )
{
    stlInt32  i;
    
    for( i = 0; i < SML_MAX_PARALLEL_IO_GROUP; i++ )
    {
        STL_TRY( knlFiniEventListener( &(*gZtmWarmupEntry)->mIoSlaveEvent[i],
                                       KNL_ENV(aEnv) ) == STL_SUCCESS );
    }
    
    STL_TRY( knlFiniEventListener( &(*gZtmWarmupEntry)->mCheckpointEvent,
                                   KNL_ENV(aEnv) ) == STL_SUCCESS );

    STL_TRY( knlFiniEventListener( &(*gZtmWarmupEntry)->mPageFlushEvent,
                                   KNL_ENV(aEnv) ) == STL_SUCCESS );

    STL_TRY( knlFiniEventListener( &(*gZtmWarmupEntry)->mLogFlusherEvent,
                                   KNL_ENV(aEnv) ) == STL_SUCCESS );

    STL_TRY( knlFiniEventListener( &(*gZtmWarmupEntry)->mArchivelogEvent,
                                   KNL_ENV(aEnv) ) == STL_SUCCESS );

    STL_TRY( knlFiniEventListener( &(*gZtmWarmupEntry)->mStartupEvent,
                                   KNL_ENV(aEnv) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Gliese Master를 DISMOUNT 상태로 전환시킨다.
 * @param[in] aShutdownInfo 종료 시 정보
 * @param[out] aEnv Environment 구조체
 */
stlStatus ztmbShutdownDismount( knlShutdownInfo * aShutdownInfo,
                                void            * aEnv )
{
    /**
     * 동시성 문제로 인하여 Shutdown 진입 초기에 stop시켜야 한다.
     * - smlCheckpoint4Shutdown()은 page flusher가 멈춘후에 진행되야 한다.
     */
        
    STL_DASSERT( (*gZtmWarmupEntry)->mState == ZTM_STATE_RUN );
    
    if( aShutdownInfo->mShutdownMode == KNL_SHUTDOWN_MODE_ABORT )
    {
        STL_TRY( ztmbKillSystemThreads( (ztmEnv*)aEnv ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( ztmbStopMountSystemThreads() == STL_SUCCESS );
    }
            
    if( aShutdownInfo->mShutdownMode == KNL_SHUTDOWN_MODE_ABORT )
    {
        STL_TRY( knlLogMsgUnsafe( NULL,
                                  KNL_ENV(aEnv),
                                  KNL_LOG_LEVEL_INFO,
                                  "[SHUTDOWN] skip DISMOUNT phase\n" )
                 == STL_SUCCESS );

        knlSetStartupPhase( KNL_STARTUP_PHASE_NO_MOUNT );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Gliese Master를 CLOSE 상태로 전환시킨다.
 * @param[in] aShutdownInfo 종료 시 정보
 * @param[out] aEnv Environment 구조체
 */
stlStatus ztmbShutdownClose( knlShutdownInfo * aShutdownInfo,
                             void            * aEnv )
{
    stlBool             sActiveShared;

    if( aShutdownInfo->mShutdownMode == KNL_SHUTDOWN_MODE_ABORT )
    {
        /**
         * 모든 Session의 Disk IO를 막는다.
         */
        knlDisableDiskIo();

        /**
         * Disk IO를 진행하는 Session이 종료되기를 기다린다.
         */
        while( 1 )
        {
            /**
             * System thread가 종료되어야 함을 설정한다.
             */
            STL_TRY( ztmbCancelIoSlaveThreads() == STL_SUCCESS );

            if( knlHasFileOpenedSession() == STL_FALSE )
            {
                break;
            }

            stlSleep( 10000 );
        }
    }

    sActiveShared = knlGetPropertyBoolValueByID( KNL_PROPERTY_SHARED_SESSION,
                                                 KNL_ENV(aEnv) );

    if( gZtmSharedMemory != NULL )
    {
        if( sActiveShared == STL_TRUE )
        {
            if( ZTM_IS_STARTED(&gZtmProcessMonitorThread) == STL_TRUE )
            {
                if( aShutdownInfo->mShutdownMode == KNL_SHUTDOWN_MODE_ABORT )
                {
                    STL_TRY( ztmbKillProcessMonitorThreads( (ztmEnv*)aEnv ) == STL_SUCCESS );
                }
                else
                {
                    // normal이면
                    STL_TRY( ztmtStopProcessMonitorThread( aEnv ) == STL_SUCCESS );
                }
            }

            if( aShutdownInfo->mShutdownMode == KNL_SHUTDOWN_MODE_ABORT )
            {
                STL_TRY( ztmtKillProcess( gZtmSharedMemory,
                                          aEnv )
                         == STL_SUCCESS );
            }
        }
    }

    if( aShutdownInfo->mShutdownMode == KNL_SHUTDOWN_MODE_ABORT )
    {
        STL_TRY( knlLogMsgUnsafe( NULL,
                                  KNL_ENV(aEnv),
                                  KNL_LOG_LEVEL_INFO,
                                  "[SHUTDOWN] skip CLOSE phase\n" )
                 == STL_SUCCESS );

        knlSetStartupPhase( KNL_STARTUP_PHASE_MOUNT );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Gliese를 Shutdown 한다.
 * @param[in] aStartPhase 시작 단계
 * @param[in] aEndPhase 종료 단계
 * @param[in] aShutdownArg 종료 argument
 * @param[in,out] aEnv Environment 포인터
 */
stlStatus ztmbShutdownPhase( knlShutdownPhase   aStartPhase,
                             knlShutdownPhase   aEndPhase,
                             knlShutdownInfo  * aShutdownInfo,
                             ztmEnv           * aEnv )
{
    stlInt32          i;
    stlInt32          j;
    knlShutdownFunc * sLayerShutdownFunc;
    knlShutdownFunc   sPhaseShutdownFunc;

    for( i = aStartPhase; i >= aEndPhase; i-- )
    {
        for( j = (STL_LAYER_MAX-1); j >= 0; j-- )
        {
            sLayerShutdownFunc = gZtmShutdownLayer[j];
            if( sLayerShutdownFunc == NULL )
            {
                continue;
            }
        
            sPhaseShutdownFunc = sLayerShutdownFunc[i];
            if( sPhaseShutdownFunc == NULL )
            {
                continue;
            }

            STL_TRY( sPhaseShutdownFunc( aShutdownInfo,
                                         aEnv )
                     == STL_SUCCESS );
        }
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

