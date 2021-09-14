/*******************************************************************************
 * ztmbStartup.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztmbStartup.c 5283 2012-08-10 03:01:37Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file ztmbStartup.c
 * @brief Gliese Master Startup Routines
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

extern ztmThread            gZtmTimerThread;
extern ztmThread            gZtmCleanupThread;
extern ztmThread            gZtmProcessMonitorThread;
extern ztmThread            gZtmCheckPointThread;
extern ztmThread            gZtmLogFlushThread;
extern ztmThread            gZtmPageFlushThread;
extern ztmThread            gZtmAgerThread;
extern ztmThread            gZtmArchivelogThread;
extern ztmThread            gZtmIoSlaveThread[];
extern sllSharedMemory    * gZtmSharedMemory;

ztmWarmupEntry           ** gZtmWarmupEntry;
ztmEnv                    * gZtmEnv = NULL;
ztmSessionEnv             * gZtmSessEnv = NULL;

ztmEnv                      gZtmSystemEnv;
ztmSessionEnv               gZtmSystemSessionEnv;

stlInt64                    gZtmBootdownKey = 0;
/**
 * @brief Gliese Master의 다단계 startup 함수
 */
knlStartupFunc gStartupZT[KNL_STARTUP_PHASE_MAX] =
{
    NULL,
    ztmbStartupNoMount,
    ztmbStartupMount,
    NULL,
    ztmbStartupOpen
};

/**
 * @brief Layer 별 startup 함수
 */
knlStartupFunc * gZtmStartupLayer[STL_LAYER_MAX] = { NULL,         /* STL_LAYER_NONE */
                                                     NULL,         /* STL_LAYER_STANDARD */
                                                     NULL,         /* STL_LAYER_DATATYPE */
                                                     NULL,         /* STL_LAYER_COMMUNICATION */
                                                     gStartupKN,   /* STL_LAYER_KERNEL */
                                                     gStartupSC,   /* STL_LAYER_SERVER_COMMUNICATION */
                                                     gStartupSM,   /* STL_LAYER_STORAGE_MANAGER */
                                                     gStartupEL,   /* STL_LAYER_EXECUTION_LIBRARY */
                                                     gStartupQP,   /* STL_LAYER_SQL_PROCESSOR */
                                                     NULL,         /* STL_LAYER_PSM_PROCESSOR */
                                                     gStartupSS,   /* STL_LAYER_SESSION */
                                                     gStartupSL,   /* STL_LAYER_SERVER_LIBRARY */
                                                     NULL,         /* STL_LAYER_GLIESE_LIBRARY */
                                                     gStartupZT,   /* STL_LAYER_GLIESE_TOOL */
                                                     NULL,         /* STL_LAYER_GOLDILOCKS_LIBRARY */
                                                     NULL };       /* STL_LAYER_GOLDILOCKS_TOOL */

extern knlShutdownFunc * gZtmShutdownLayer[];

/**
 * @brief Gliese Master를 No Mount 상태로 전환시킨다.
 * @param[in] aStartupInfo Startup 시 정보 (Recovery type(restart or media), Tablespace Id)
 * @param[out] aEnv Environment 구조체
 */
stlStatus ztmbStartupNoMount( knlStartupInfo * aStartupInfo,
                              void           * aEnv )
{
    ztmWarmupEntry * sWarmupEntry;
    stlInt32         i;

    STL_TRY( knlGetEntryPoint( STL_LAYER_GLIESE_TOOL,
                               (void**)&gZtmWarmupEntry,
                               (knlEnv*)aEnv ) == STL_SUCCESS );

    STL_TRY( knlAllocFixedStaticArea( STL_SIZEOF(ztmWarmupEntry),
                                      (void**)&sWarmupEntry,
                                      (knlEnv*)aEnv ) == STL_SUCCESS );

    STL_TRY( knlInitEventListener( &sWarmupEntry->mStartupEvent,
                                   KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( knlInitEventListener( &sWarmupEntry->mCheckpointEvent,
                                   KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( knlInitEventListener( &sWarmupEntry->mPageFlushEvent,
                                   KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( knlInitEventListener( &sWarmupEntry->mLogFlusherEvent,
                                   KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( knlInitEventListener( &sWarmupEntry->mArchivelogEvent,
                                   KNL_ENV(aEnv) ) == STL_SUCCESS );


    for( i = 0; i < SML_MAX_PARALLEL_IO_GROUP; i++ )
    {
        STL_TRY( knlInitEventListener( &sWarmupEntry->mIoSlaveEvent[i],
                                       KNL_ENV(aEnv) ) == STL_SUCCESS );
    }
    
    knlRegisterEventHandler( KNL_EVENT_STARTUP, ztmbStartupEventHandler );
    knlRegisterEventHandler( KNL_EVENT_SHUTDOWN, ztmbShutdownEventHandler );
    knlRegisterEventHandler( KNL_EVENT_CLEANUP, ztmbCleanupEventHandler );
    
    *gZtmWarmupEntry = sWarmupEntry;

    sWarmupEntry->mServerStatus = ZTM_SERVER_STATUS_INIT;
    sWarmupEntry->mState = ZTM_STATE_INIT;
    
    knlSetStartupPhase( KNL_STARTUP_PHASE_NO_MOUNT );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 시스템 쓰레드가 정상적으로 시작되었는지 확인한다.
 * @param[in] aThread check할 thread
 * @param[in] aEnv    Environment 구조체
 * @remark thread가 정상 시작되지 않은 경우 error를 errorStack에 추가한다.
 */
stlStatus ztmbCheckStarted( ztmThread * aThread,
                            void      * aEnv )
{
    STL_TRY( ZTM_IS_STARTED( aThread ) == STL_TRUE );

    return STL_SUCCESS;

    STL_FINISH;

    stlAppendErrors( KNL_ERROR_STACK(aEnv), &aThread->mArgs.mErrorStack );

    return STL_FAILURE;
}

/**
 * @brief 시스템 쓰레드들을 생성한다.
 * @param[out] aEnv Environment 구조체
 * @remark CheckPoint, LogFlush 쓰레드를 생성한다.
 */
stlStatus ztmbCreateMountSystemThreads( void * aEnv )
{
    stlInt32 i = 0;
    stlInt32 sState = 0;

    STL_TRY_THROW( ((*gZtmWarmupEntry)->mState != ZTM_STATE_RUN),
                   RAMP_ERR_ALREADY_EXIST_SYSTEM_THREAD );

    /*
     * checkpoint 쓰레드 생성하기
     */

    ZTM_SET_THREAD_STATE( &gZtmCheckPointThread, ZTM_THREAD_STATE_INIT );
    STL_TRY( ztmtCreateCheckPointThread( aEnv ) == STL_SUCCESS );
    ZTM_WAIT_THREAD( &gZtmCheckPointThread );
    STL_TRY_THROW( ztmbCheckStarted( &gZtmCheckPointThread, aEnv ) == STL_SUCCESS,
                   RAMP_ERR_CHECKPOINT_THREAD_CREATION_FAIL );
    sState = 1;

    /*
     * log flush 쓰레드 생성하기
     */

    ZTM_SET_THREAD_STATE( &gZtmLogFlushThread, ZTM_THREAD_STATE_INIT );
    STL_TRY( ztmtCreateLogFlushThreads( aEnv ) == STL_SUCCESS );
    ZTM_WAIT_THREAD( &gZtmLogFlushThread );
    STL_TRY_THROW( ztmbCheckStarted( &gZtmLogFlushThread, aEnv ) == STL_SUCCESS,
                   RAMP_ERR_LOGFLUSH_THREAD_CREATION_FAIL );
    sState = 2;

    /*
     * page flush 쓰레드 생성하기
     */

    ZTM_SET_THREAD_STATE( &gZtmPageFlushThread, ZTM_THREAD_STATE_INIT );
    STL_TRY( ztmtCreatePageFlushThread( aEnv ) == STL_SUCCESS );
    ZTM_WAIT_THREAD( &gZtmPageFlushThread );
    STL_TRY_THROW( ztmbCheckStarted( &gZtmPageFlushThread, aEnv ) == STL_SUCCESS,
                   RAMP_ERR_PAGEFLUSH_THREAD_CREATION_FAIL );
    sState = 3;

    /*
     * io slaves 쓰레드 생성하기
     */

    for( i = 0; i < (*gZtmWarmupEntry)->mIoSlaveCount; i++ )
    {
        ZTM_SET_THREAD_STATE( &gZtmIoSlaveThread[i], ZTM_THREAD_STATE_INIT );
        STL_TRY( ztmtCreateIoSlaveThread( i, aEnv ) == STL_SUCCESS );
        ZTM_WAIT_THREAD( &gZtmIoSlaveThread[i] );
        STL_TRY_THROW( ztmbCheckStarted( &gZtmIoSlaveThread[i], aEnv ) == STL_SUCCESS,
                       RAMP_ERR_IOSLAVE_THREAD_CREATION_FAIL );
        sState = 4;
    }

    /*
     * Ager 쓰레드 생성하기
     */

    ZTM_SET_THREAD_STATE( &gZtmAgerThread, ZTM_THREAD_STATE_INIT );
    STL_TRY( ztmtCreateAgerThread( aEnv ) == STL_SUCCESS );
    ZTM_WAIT_THREAD( &gZtmAgerThread );
    STL_TRY_THROW( ztmbCheckStarted( &gZtmAgerThread, aEnv ) == STL_SUCCESS,
                   RAMP_ERR_AGER_THREAD_CREATION_FAIL );
    sState = 5;

    /*
     * archivelog 쓰레드 생성하기
     */

    ZTM_SET_THREAD_STATE( &gZtmArchivelogThread, ZTM_THREAD_STATE_INIT );
    STL_TRY( ztmtCreateArchivelogThread( aEnv ) == STL_SUCCESS );
    ZTM_WAIT_THREAD( &gZtmArchivelogThread );
    STL_TRY_THROW( ztmbCheckStarted( &gZtmArchivelogThread, aEnv ) == STL_SUCCESS,
                   RAMP_ERR_ARCHIVELOG_THREAD_CREATION_FAIL );
    sState = 6;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_ALREADY_EXIST_SYSTEM_THREAD )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_ALREADY_EXIST_SYSTEM_THREAD,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }


    STL_CATCH( RAMP_ERR_CHECKPOINT_THREAD_CREATION_FAIL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SYSTEM_THREAD_CREATION_FAIL,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      ZTM_THREAD_NAME_CHECK_POINT );
    }
    STL_CATCH( RAMP_ERR_LOGFLUSH_THREAD_CREATION_FAIL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SYSTEM_THREAD_CREATION_FAIL,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      ZTM_THREAD_NAME_LOG_FLUSH );
    }
    STL_CATCH( RAMP_ERR_PAGEFLUSH_THREAD_CREATION_FAIL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SYSTEM_THREAD_CREATION_FAIL,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      ZTM_THREAD_NAME_PAGE_FLUSH );
    }
    STL_CATCH( RAMP_ERR_IOSLAVE_THREAD_CREATION_FAIL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SYSTEM_THREAD_CREATION_FAIL,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      ZTM_THREAD_NAME_IO_SLAVE );
    }
    STL_CATCH( RAMP_ERR_AGER_THREAD_CREATION_FAIL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SYSTEM_THREAD_CREATION_FAIL,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      ZTM_THREAD_NAME_AGER );
    }
    STL_CATCH( RAMP_ERR_ARCHIVELOG_THREAD_CREATION_FAIL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SYSTEM_THREAD_CREATION_FAIL,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      ZTM_THREAD_NAME_ARCHIVELOG );
    }

    STL_FINISH;

    switch( sState )
    {
        case 6:
            (void) ztmtStopArchivelogThread( aEnv );
        case 5:
            (void) ztmtStopAgerThread( aEnv );
        case 4:
            (void) ztmtStopIoSlaveThreads( i, aEnv );
        case 3:
            (void) ztmtStopPageFlushThread( aEnv );
        case 2:
            (void) ztmtStopLogFlushThreads( aEnv );
        case 1:
            (void) ztmtStopCheckPointThread( aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief 시스템 쓰레드들의 종료를 기다린다.
 */
stlStatus ztmbStopMountSystemThreads()
{
    STL_TRY( ztmtStopAgerThread( &gZtmSystemEnv ) == STL_SUCCESS );
    STL_TRY( ztmtStopCheckPointThread( &gZtmSystemEnv ) == STL_SUCCESS );
    STL_TRY( ztmtStopPageFlushThread( &gZtmSystemEnv ) == STL_SUCCESS );
    STL_TRY( ztmtStopLogFlushThreads( &gZtmSystemEnv ) == STL_SUCCESS );
    STL_TRY( ztmtStopArchivelogThread( &gZtmSystemEnv ) == STL_SUCCESS );
    STL_TRY( ztmtStopIoSlaveThreads( (*gZtmWarmupEntry)->mIoSlaveCount,
                                     &gZtmSystemEnv )
             == STL_SUCCESS );
    (*gZtmWarmupEntry)->mState = ZTM_STATE_STOPPED;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 시스템 쓰레드들을 죽인다.
 */
stlStatus ztmbKillSystemThreads( ztmEnv * aEnv )
{
    stlInt32  i;
    stlStatus sReturnStatus;

    KNL_CANCEL_THREAD( KNL_ENV(gZtmTimerThread.mEnv) );
    ZTM_SET_THREAD_STATE( &gZtmTimerThread, ZTM_THREAD_STATE_INIT );
    
    KNL_CANCEL_THREAD( KNL_ENV(gZtmCleanupThread.mEnv) );
    ZTM_SET_THREAD_STATE( &gZtmCleanupThread, ZTM_THREAD_STATE_INIT );

    KNL_CANCEL_THREAD( KNL_ENV(gZtmCheckPointThread.mEnv) );
    ZTM_SET_THREAD_STATE( &gZtmCheckPointThread, ZTM_THREAD_STATE_INIT );

    KNL_CANCEL_THREAD( KNL_ENV(gZtmLogFlushThread.mEnv) );
    ZTM_SET_THREAD_STATE( &gZtmLogFlushThread, ZTM_THREAD_STATE_INIT );

    KNL_CANCEL_THREAD( KNL_ENV(gZtmPageFlushThread.mEnv) );
    ZTM_SET_THREAD_STATE( &gZtmPageFlushThread, ZTM_THREAD_STATE_INIT );

    KNL_CANCEL_THREAD( KNL_ENV(gZtmAgerThread.mEnv) );
    ZTM_SET_THREAD_STATE( &gZtmAgerThread, ZTM_THREAD_STATE_INIT );

    KNL_CANCEL_THREAD( KNL_ENV(gZtmArchivelogThread.mEnv) );
    ZTM_SET_THREAD_STATE( &gZtmArchivelogThread, ZTM_THREAD_STATE_INIT );

    for( i = 0; i < (*gZtmWarmupEntry)->mIoSlaveCount; i++ )
    {
        KNL_CANCEL_THREAD( KNL_ENV(gZtmIoSlaveThread[i].mEnv) );
        ZTM_SET_THREAD_STATE( &gZtmIoSlaveThread[i], ZTM_THREAD_STATE_INIT );
    }

    STL_TRY( stlJoinThread( &gZtmTimerThread.mThread,
                            &sReturnStatus,
                            KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    STL_TRY( stlJoinThread( &gZtmCleanupThread.mThread,
                            &sReturnStatus,
                            KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    STL_TRY( stlJoinThread( &gZtmCheckPointThread.mThread,
                            &sReturnStatus,
                            KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    STL_TRY( stlJoinThread( &gZtmLogFlushThread.mThread,
                            &sReturnStatus,
                            KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    STL_TRY( stlJoinThread( &gZtmPageFlushThread.mThread,
                            &sReturnStatus,
                            KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    STL_TRY( stlJoinThread( &gZtmAgerThread.mThread,
                            &sReturnStatus,
                            KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    STL_TRY( stlJoinThread( &gZtmArchivelogThread.mThread,
                            &sReturnStatus,
                            KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    for( i = 0; i < (*gZtmWarmupEntry)->mIoSlaveCount; i++ )
    {
        STL_TRY( stlJoinThread( &gZtmIoSlaveThread[i].mThread,
                                &sReturnStatus,
                                KNL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );
    }

    (*gZtmWarmupEntry)->mState = ZTM_STATE_STOPPED;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ProcessMonitor 쓰레드를 죽인다.
 */
stlStatus ztmbKillProcessMonitorThreads( ztmEnv * aEnv )
{
    stlStatus sReturnStatus;

    KNL_CANCEL_THREAD( KNL_ENV(gZtmProcessMonitorThread.mEnv) );
    ZTM_SET_THREAD_STATE( &gZtmProcessMonitorThread, ZTM_THREAD_STATE_INIT );

    STL_TRY( stlJoinThread( &gZtmProcessMonitorThread.mThread,
                            &sReturnStatus,
                            KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 시스템 쓰레드들이 종료되어야 함을 설정한다.
 */
stlStatus ztmbCancelIoSlaveThreads()
{
    stlInt32 i;
    
    for( i = 0; i < (*gZtmWarmupEntry)->mIoSlaveCount; i++ )
    {
        STL_TRY( ztmtCancelIoSlaveThread( i, &gZtmSystemEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Gliese Master를 Service 가능한 상태로 전환시킨다.
 * @param[in] aStartupInfo Startup 시 정보 (Recovery type(restart or media), Tablespace Id)
 * @param[out] aEnv Environment 구조체
 */
stlStatus ztmbStartupMount( knlStartupInfo * aStartupInfo,
                            void           * aEnv )
{
    /**
     * System Thread들을 구동시킨다.
     */

    (*gZtmWarmupEntry)->mIoSlaveCount = knlGetPropertyBigIntValueByID( KNL_PROPERTY_PARALLEL_IO_FACTOR,
                                                                       KNL_ENV(aEnv) );

    STL_TRY( ztmbCreateMountSystemThreads( aEnv ) == STL_SUCCESS );
    (*gZtmWarmupEntry)->mState = ZTM_STATE_RUN;

    knlSetStartupPhase( KNL_STARTUP_PHASE_MOUNT );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Gliese Master를 Service 가능한 상태로 전환시킨다.
 * @param[in] aStartupInfo Startup 시 정보 (Recovery type(restart or media), Tablespace Id)
 * @param[out] aEnv Environment 구조체
 */
stlStatus ztmbStartupOpen( knlStartupInfo * aStartupInfo,
                           void           * aEnv )
{
    void             ** sWarmupEntry;
    stlBool             sActiveShared;

    /**
     * SAR(System Available Resource)에 TRANSACTION 항목을 추가한다.
     */

    STL_TRY( knlAddSar( KNL_SAR_TRANSACTION,
                        KNL_ENV(aEnv) )
             == STL_SUCCESS );
        
    knlSetStartupPhase( KNL_STARTUP_PHASE_OPEN );


    STL_TRY( knlGetEntryPoint( STL_LAYER_SERVER_LIBRARY,
                               (void **) &sWarmupEntry,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );

    gZtmSharedMemory = *sWarmupEntry;

    /**
     * ProcessMonitor Thread 구동
     */
    sActiveShared = knlGetPropertyBoolValueByID( KNL_PROPERTY_SHARED_SESSION,
                                                 KNL_ENV(aEnv) );

    if( sActiveShared == STL_TRUE )
    {
        STL_TRY( ztmtCreateProcessMonitorThread( aEnv ) == STL_SUCCESS );

        STL_TRY_THROW( ztmbCheckStarted( &gZtmProcessMonitorThread, aEnv ) == STL_SUCCESS,
                       RAMP_ERR_MONITOR_THREAD_CREATION_FAIL );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_MONITOR_THREAD_CREATION_FAIL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SYSTEM_THREAD_CREATION_FAIL,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      ZTM_THREAD_NAME_PROCESS_MONITOR );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Gliese를 Startup 한다.
 * @param[in] aStartPhase 시작 단계
 * @param[in] aEndPhase 종료 단계
 * @param[in] aStartupInfo Startup 시 정보
 * @param[in,out] aEnv Environment 포인터
 */
stlStatus ztmbStartupPhase( knlStartupPhase   aStartPhase,
                            knlStartupPhase   aEndPhase,
                            knlStartupInfo  * aStartupInfo,
                            ztmEnv          * aEnv )
{
    stlInt32            i;
    knlStartupFunc    * sLayerStartupFunc;
    knlStartupFunc      sPhaseStartupFunc;
    knlStartupPhase     sStartupPhase;
    knlShutdownPhase    sShutdownPhase;
    knlShutdownPhase    sEndShutdownPhase = KNL_SHUTDOWN_PHASE_NONE;
    knlShutdownFunc   * sLayerShutdownFunc;
    knlShutdownFunc     sPhaseShutdownFunc;
    knlShutdownInfo     sShutdownInfo;

    for( sStartupPhase = aStartPhase; sStartupPhase <= aEndPhase; sStartupPhase++ )
    {
        for( i = 0; i < STL_LAYER_MAX; i++ )
        {
            sLayerStartupFunc = gZtmStartupLayer[i];
            if( sLayerStartupFunc == NULL )
            {
                continue;
            }

            sPhaseStartupFunc = sLayerStartupFunc[sStartupPhase];
            if( sPhaseStartupFunc == NULL )
            {
                continue;
            }

            STL_TRY( sPhaseStartupFunc( aStartupInfo, aEnv ) == STL_SUCCESS );
        }
    }

    return STL_SUCCESS;
    
    STL_FINISH;

    sShutdownPhase = knlGetShutdownPhasePair( sStartupPhase );
    
    switch( sStartupPhase )
    {
        case KNL_STARTUP_PHASE_NO_MOUNT:
            (void) knlLogMsg( NULL,
                              KNL_ENV(aEnv),
                              KNL_LOG_LEVEL_WARNING,
                              "[STARTUP] failed to NOMOUNT database\n" );

            sEndShutdownPhase = KNL_SHUTDOWN_PHASE_INIT;
            break;
        case KNL_STARTUP_PHASE_MOUNT:
            (void) knlLogMsg( NULL,
                              KNL_ENV(aEnv),
                              KNL_LOG_LEVEL_WARNING,
                              "[STARTUP] failed to MOUNT database\n" );

            sEndShutdownPhase = KNL_SHUTDOWN_PHASE_DISMOUNT;
            break;
        case KNL_STARTUP_PHASE_PREOPEN:
        case KNL_STARTUP_PHASE_OPEN:
            (void) knlLogMsg( NULL,
                              KNL_ENV(aEnv),
                              KNL_LOG_LEVEL_WARNING,
                              "[STARTUP] failed to OPEN database\n" );

            sEndShutdownPhase = KNL_SHUTDOWN_PHASE_POSTCLOSE;
            break;
        case KNL_STARTUP_PHASE_NONE:
        case KNL_STARTUP_PHASE_MAX:
        default:
            break;
    }

    KNL_INIT_SHUTDOWN_INFO( &sShutdownInfo );
    sShutdownInfo.mShutdownMode = KNL_SHUTDOWN_MODE_NORMAL;
    
    for( ; sShutdownPhase >= sEndShutdownPhase; sShutdownPhase-- )
    {
        for( i--; i >= 0; i-- )
        {
            sLayerShutdownFunc = gZtmShutdownLayer[i];
            if( sLayerShutdownFunc == NULL )
            {
                continue;
            }

            sPhaseShutdownFunc = sLayerShutdownFunc[sShutdownPhase];
            if( sPhaseShutdownFunc == NULL )
            {
                continue;
            }

            (void) sPhaseShutdownFunc( &sShutdownInfo,
                                       aEnv );
        }

        i = STL_LAYER_MAX;
    }

    (void) knlLogErrorCallStack( NULL, KNL_ENV(aEnv) );
    
    return STL_FAILURE;
}


/**
 * @brief Gliese 마스터를 startup 한다.
 * @param[in/out] aMsgKey msg queue key
 * @param[in] aErrorStack 푸쉬될 에러스택 포인터
 */
stlStatus ztmbStartup( stlInt64      * aMsgKey,
                       stlErrorStack * aErrorStack )
{
    stlInt32           sState = 0;
    knlEventListener * sEventListener;
    stlBool            sIsTimedOut;
    stlBool            sIsSuccess;
    stlUInt32          sEnvId;
    stlChar            sSemName[STL_MAX_NAMED_SEM_NAME+1];
    stlNamedSemaphore  sSem;
    stlBool            sOpenSem = STL_FALSE;
    stlBool            sCreateSem = STL_FALSE;

    stlBool            sInitBootupEnv     = STL_FALSE;
    stlBool            sInitBootupSession = STL_FALSE;
    ztmEnv             sBootupEnv;
    ztmSessionEnv      sBootupSession;

    stlBool            sInitChildEnv     = STL_FALSE;
    stlBool            sInitChildSession = STL_FALSE;

    stlProc            sProcess;
    stlChar            sProgram[STL_PATH_MAX];
    knlStartupInfo     sStartupInfo;
    knlShutdownInfo    sShutdownInfo;
    stlInt32           sEventId;
    stlBool            sIsCanceled;
    stlChar          * sEventDesc;

    /*
     * 시스템 Env, Session으로 no mount bootup
     */
    STL_TRY( sslInitializeEnv( SSL_ENV( &sBootupEnv ),
                               KNL_ENV_HEAP ) == STL_SUCCESS );
    sInitBootupEnv = STL_TRUE;

    STL_TRY( sslInitSessionEnv( (sslSessionEnv*)&sBootupSession,
                                KNL_CONNECTION_TYPE_NONE,
                                KNL_SESSION_TYPE_DEDICATE,
                                KNL_SESS_ENV_HEAP,
                                SSL_ENV( &sBootupEnv ) ) == STL_SUCCESS );
    sInitBootupSession = STL_TRUE;
    
    KNL_LINK_SESS_ENV( &sBootupEnv, &sBootupSession );

    KNL_INIT_STARTUP_ARG( &sStartupInfo );
    sStartupInfo.mLogOption = SML_STARTUP_LOG_OPTION_NORESETLOGS;

    STL_TRY( ztmbStartupPhase( KNL_STARTUP_PHASE_NO_MOUNT,
                               KNL_STARTUP_PHASE_NO_MOUNT,
                               &sStartupInfo,
                               &sBootupEnv ) == STL_SUCCESS );
    sState = 1;

    /**
     * SAR(System Available Resource)에 WARMUP 항목을 추가한다.
     */
        
    STL_TRY( knlAddSar( KNL_SAR_WARMUP,
                        KNL_ENV(&sBootupEnv) )
             == STL_SUCCESS );
    
    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_SHARED_MEMORY_KEY,
                                      &gZtmBootdownKey,
                                      KNL_ENV(&sBootupEnv) ) == STL_SUCCESS );

    stlSnprintf( sSemName, STL_MAX_NAMED_SEM_NAME+1, "%d", (stlInt32)gZtmBootdownKey );

    sEventListener = &(*gZtmWarmupEntry)->mStartupEvent;
    
    sInitBootupSession = STL_FALSE;
    STL_TRY( sslFiniSessionEnv( (sslSessionEnv*)&sBootupSession,
                                SSL_ENV( &sBootupEnv ) ) == STL_SUCCESS );
    
    sInitBootupEnv = STL_FALSE;
    STL_TRY( sslFinalizeEnv( SSL_ENV( &sBootupEnv ) ) == STL_SUCCESS );

    STL_TRY( stlBlockRealtimeThreadSignals( KNL_ERROR_STACK(&gZtmSystemEnv) )
             == STL_SUCCESS );

    STL_TRY( stlSetSignalHandler( STL_SIGNAL_HUP,
                                  STL_SIGNAL_IGNORE,
                                  NULL,  /* aOldHandler */
                                  KNL_ERROR_STACK(&gZtmSystemEnv))
             == STL_SUCCESS );

    STL_TRY( stlSetSignalHandler( STL_SIGNAL_TERM,
                                  STL_SIGNAL_IGNORE,
                                  NULL,  /* aOldHandler */
                                  KNL_ERROR_STACK(&gZtmSystemEnv))
             == STL_SUCCESS );

    STL_TRY( stlSetSignalHandler( STL_SIGNAL_QUIT,
                                  STL_SIGNAL_IGNORE,
                                  NULL,  /* aOldHandler */
                                  KNL_ERROR_STACK(&gZtmSystemEnv))
             == STL_SUCCESS );
    
    STL_TRY( stlSetSignalHandler( STL_SIGNAL_INT,
                                  STL_SIGNAL_IGNORE,
                                  NULL,  /* aOldHandler */
                                  KNL_ERROR_STACK(&gZtmSystemEnv))
             == STL_SUCCESS );

    STL_TRY( stlSetSignalHandler( STL_SIGNAL_USR2,
                                  STL_SIGNAL_IGNORE,
                                  NULL,  /* aOldHandler */
                                  KNL_ERROR_STACK(&gZtmSystemEnv))
             == STL_SUCCESS );

    STL_TRY( stlSetSignalHandler( STL_SIGNAL_CHLD,
                                  STL_SIGNAL_IGNORE,
                                  NULL, /* aOldHandler */
                                  KNL_ERROR_STACK(&gZtmSystemEnv))
             == STL_SUCCESS );


    STL_TRY( sslInitializeEnv( SSL_ENV( &gZtmSystemEnv ),
                               KNL_ENV_HEAP ) == STL_SUCCESS );
    sInitChildEnv = STL_TRUE;

    STL_TRY( sslInitSessionEnv( (sslSessionEnv*)&gZtmSystemSessionEnv,
                                KNL_CONNECTION_TYPE_NONE,
                                KNL_SESSION_TYPE_DEDICATE,
                                KNL_SESS_ENV_HEAP,
                                SSL_ENV( &gZtmSystemEnv ) ) == STL_SUCCESS );
    sInitChildSession = STL_TRUE;

    KNL_LINK_SESS_ENV( &gZtmSystemEnv, &gZtmSystemSessionEnv );

    STL_TRY( stlGetCurrentProc( &sProcess,
                                KNL_ERROR_STACK(&gZtmSystemEnv) ) == STL_SUCCESS );

    STL_TRY( stlGetProcName( &sProcess,
                             sProgram,
                             STL_PATH_MAX,
                             KNL_ERROR_STACK(&gZtmSystemEnv) ) == STL_SUCCESS );

    /**
     * Allocate environment
     */
    STL_TRY( knlAllocEnv( (void**)&gZtmEnv,
                          KNL_ERROR_STACK(&gZtmSystemEnv) )
             == STL_SUCCESS );
    sState = 3;

    knlGetEnvId( gZtmEnv, &sEnvId );
    STL_DASSERT( sEnvId == 0 );

    STL_TRY( sslInitializeEnv( SSL_ENV( gZtmEnv ),
                               KNL_ENV_SHARED ) == STL_SUCCESS );
    sState = 4;

    STL_TRY( knlAllocSessionEnv( (void**)&gZtmSessEnv,
                                 KNL_ENV(gZtmEnv) )
             == STL_SUCCESS );
    sState = 5;

    STL_TRY( sslInitSessionEnv( (sslSessionEnv*)gZtmSessEnv,
                                KNL_CONNECTION_TYPE_DA,
                                KNL_SESSION_TYPE_DEDICATE,
                                KNL_SESS_ENV_SHARED,
                                SSL_ENV( gZtmEnv ) ) == STL_SUCCESS );
    sState = 6;

    KNL_LINK_SESS_ENV( gZtmEnv, gZtmSessEnv );

    ((sslSessionEnv*)gZtmSessEnv)->mClientProcId = sProcess;
    stlSnprintf( ((sslSessionEnv*)gZtmSessEnv)->mProgram,
                 SSL_PROGRAM_NAME,
                 "%s",
                 sProgram );

    /**
     * Timer Thread 구동
     */

    STL_TRY( ztmtCreateTimerThread( gZtmEnv ) == STL_SUCCESS );
    STL_TRY_THROW( ztmbCheckStarted( &gZtmTimerThread, gZtmEnv ) == STL_SUCCESS,
                   RAMP_ERR_TIMER_THREAD_CREATION_FAIL );

    /**
     * Cleanup Thread 구동
     */

    STL_TRY( ztmtCreateCleanupThread( gZtmEnv ) == STL_SUCCESS );
    STL_TRY_THROW( ztmbCheckStarted( &gZtmCleanupThread, gZtmEnv ) == STL_SUCCESS,
                   RAMP_ERR_CLEANUP_THREAD_CREATION_FAIL );

    /**
     * Event Listener 등록
     */
    knlRegisterEventListener( sEventListener, KNL_ENV(gZtmEnv) );

    (*gZtmWarmupEntry)->mServerStatus = ZTM_SERVER_STATUS_START;

    STL_TRY( stlWakeupExecProc( *aMsgKey,
                                KNL_ERROR_STACK(gZtmEnv) )
             == STL_SUCCESS );

    /**
     * stlSendErrorStackMsgQueue를 한번 호출하고 나면 aMsgKey를 0으로 설정한다.
     */
    *aMsgKey = 0;

    while( 1 )
    {
        STL_TRY( knlWaitEvent( sEventListener,
                               STL_INFINITE_TIME,
                               &sIsTimedOut,
                               KNL_ENV( gZtmEnv ) )
                 == STL_SUCCESS );

        STL_TRY( knlExecuteEnvEvent( KNL_ENV( gZtmEnv ),
                                     &sEventId,
                                     &sIsCanceled,
                                     &sIsSuccess )
                 == STL_SUCCESS );

        sEventDesc = knlGetEventDesc( sEventId );

        STL_TRY( knlLogMsg( NULL,
                            KNL_ENV( gZtmEnv ),
                            KNL_LOG_LEVEL_INFO,
                            "[EVENT] %s : %s\n",
                            (sEventDesc == NULL)? "UNKNOWN" : sEventDesc,
                            ( (sIsCanceled == STL_TRUE)? "CANCELED" :
                              ((sIsSuccess == STL_TRUE)? "SUCCESS" : "FAIL") ) )
                 == STL_SUCCESS );

        if( ((*gZtmWarmupEntry)->mServerStatus == ZTM_SERVER_STATUS_SHUTDOWN) ||
            ((*gZtmWarmupEntry)->mServerStatus == ZTM_SERVER_STATUS_SHUTDOWN_ABORT) )
        {
            sCreateSem = STL_TRUE;
            break;
        }
    }

    /**
     * Timer/Cleanup Thread 종료 대기
     */

    if( ZTM_IS_STARTED( &gZtmTimerThread ) == STL_TRUE )
    {
        STL_TRY( ztmtStopTimerThread( gZtmEnv ) == STL_SUCCESS );
        STL_TRY( ztmtStopCleanupThread( gZtmEnv ) == STL_SUCCESS );
    }

    /**
     * Deallocate environment
     */

    if( (*gZtmWarmupEntry)->mServerStatus == ZTM_SERVER_STATUS_SHUTDOWN )
    {
        sState = 5;
        STL_TRY( sslFiniSessionEnv( (sslSessionEnv*)gZtmSessEnv,
                                    SSL_ENV( gZtmEnv ) )
                 == STL_SUCCESS );

        sState = 4;
        STL_TRY( knlFreeSessionEnv( (void*)gZtmSessEnv,
                                    KNL_ENV( gZtmEnv ) )
                 == STL_SUCCESS );

        sState = 3;
        STL_TRY( sslFinalizeEnv( SSL_ENV( gZtmEnv ) ) == STL_SUCCESS );

        sState = 2;
        STL_TRY( knlFreeEnv( (void*)gZtmEnv,
                             KNL_ERROR_STACK( &gZtmSystemEnv ) )
                 == STL_SUCCESS );
    }
    else
    {
        /**
         * 정상 Shutdown이 아닌 경우는 ( shutdown abort ) Environemnt를
         * 정리하지 않는다.
         * - gmaster thread가 cancel된 경우 래치를 획득한 상태에서 종료될수
         *   있기 때문에 hang이 발생할수 있다.
         * - Valgrind leak을 막기 위해서 private static area만 정리한다.
         */

        STL_TRY( knlCleanupHeapMem( KNL_ENV( gZtmEnv ) ) == STL_SUCCESS );
    }

    /**
     * Boot down
     */
    if( stlOpenNamedSemaphore( &sSem,
                               sSemName,
                               KNL_ERROR_STACK( &gZtmSystemEnv ) )
        == STL_SUCCESS )
    {
        sOpenSem = STL_TRUE;
    }
    else
    {
        (void) knlLogMsgUnsafe( NULL,
                                KNL_ENV( &gZtmSystemEnv ),
                                KNL_LOG_LEVEL_INFO,
                                "[SHUTDOWN] failed to open semaphore %s (error code: %d, system code: %d)\n",
                                sSemName,
                                stlGetLastErrorCode(KNL_ERROR_STACK(&gZtmSystemEnv)),
                                stlGetLastSystemErrorCode(KNL_ERROR_STACK(&gZtmSystemEnv)) );
    }

    KNL_INIT_SHUTDOWN_INFO( &sShutdownInfo );
    
    if( (*gZtmWarmupEntry)->mServerStatus == ZTM_SERVER_STATUS_SHUTDOWN_ABORT )
    {
        sShutdownInfo.mShutdownMode = KNL_SHUTDOWN_MODE_ABORT;
    }
    else
    {
        sShutdownInfo.mShutdownMode = KNL_SHUTDOWN_MODE_NORMAL;
    }

    (*gZtmWarmupEntry)->mServerStatus = ZTM_SERVER_STATUS_EXIT;

    sState = 0;
    (void) ztmbShutdownPhase( knlGetShutdownPhasePair( knlGetCurrStartupPhase() ),
                              KNL_SHUTDOWN_PHASE_INIT,
                              &sShutdownInfo,
                              &gZtmSystemEnv );

    if( sOpenSem == STL_TRUE )
    {
        STL_TRY( stlReleaseNamedSemaphore( &sSem,
                                           KNL_ERROR_STACK( &gZtmSystemEnv ) )
                 == STL_SUCCESS );

        sOpenSem = STL_FALSE;
        STL_TRY( stlCloseNamedSemaphore( &sSem,
                                         KNL_ERROR_STACK( &gZtmSystemEnv ) )
                 == STL_SUCCESS );
    }

    sInitChildSession = STL_FALSE;
    (void)sslFiniSessionEnv( (sslSessionEnv*)&gZtmSystemSessionEnv,
                              SSL_ENV( &gZtmSystemEnv ) );

    sInitChildEnv = STL_FALSE;
    (void)sslFinalizeEnv( SSL_ENV( &gZtmSystemEnv ) );

    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_TIMER_THREAD_CREATION_FAIL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SYSTEM_THREAD_CREATION_FAIL,
                      NULL,
                      KNL_ERROR_STACK(gZtmEnv),
                      ZTM_THREAD_NAME_TIMER );
    }
    STL_CATCH( RAMP_ERR_CLEANUP_THREAD_CREATION_FAIL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SYSTEM_THREAD_CREATION_FAIL,
                      NULL,
                      KNL_ERROR_STACK(gZtmEnv),
                      ZTM_THREAD_NAME_CLEANUP );
    }

    STL_FINISH;

    switch( sState )
    {
        case 6 :
            (void)sslFiniSessionEnv( (sslSessionEnv*)gZtmSessEnv, SSL_ENV(gZtmEnv) );
        case 5 :
            (void)knlFreeSessionEnv( (void*)gZtmSessEnv, KNL_ENV(gZtmEnv) );
        case 4 :
            stlAppendErrors( aErrorStack, KNL_ERROR_STACK(gZtmEnv) );
            (void)sslFinalizeEnv( SSL_ENV( gZtmEnv ) );
        case 3 :
            (void)knlFreeEnv( (void*)gZtmEnv, KNL_ERROR_STACK(&gZtmSystemEnv) );
        case 2 :
        case 1 :
            if( sCreateSem == STL_TRUE )
            {
                (void)stlOpenNamedSemaphore( &sSem,
                                             sSemName,
                                             KNL_ERROR_STACK( &gZtmSystemEnv ) );
                sOpenSem = STL_TRUE;
            }

            if( sInitChildSession == STL_TRUE )
            {
                (void)sslFiniSessionEnv( (sslSessionEnv*)&gZtmSystemSessionEnv,
                                         SSL_ENV( &gZtmSystemEnv ) );
            }

            stlAppendErrors( aErrorStack, KNL_ERROR_STACK(&gZtmSystemEnv) );
            if( sInitChildEnv == STL_TRUE )
            {
                (void)sslFinalizeEnv( SSL_ENV( &gZtmSystemEnv ) );
            }

        default :
            break;
    }
    
    if( sOpenSem == STL_TRUE )
    {
        (void)stlReleaseNamedSemaphore( &sSem,
                                        KNL_ERROR_STACK( &gZtmSystemEnv ) );
        (void)stlCloseNamedSemaphore( &sSem,
                                      KNL_ERROR_STACK( &gZtmSystemEnv ) );
    }

    if( sInitBootupSession == STL_TRUE )
    {
        (void)sslFiniSessionEnv( (sslSessionEnv*)&sBootupSession,
                                 SSL_ENV( &sBootupEnv ) );
    }

    stlAppendErrors( aErrorStack, KNL_ERROR_STACK(&sBootupEnv) );
    
    if( sInitBootupEnv == STL_TRUE )
    {
        (void)sslFinalizeEnv( SSL_ENV( &sBootupEnv ) );
    }

    
    return STL_FAILURE;
}

