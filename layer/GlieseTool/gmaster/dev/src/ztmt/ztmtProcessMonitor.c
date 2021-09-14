/*******************************************************************************
 * ztmtProcessMonitor.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztmtProcessMonitor.c 4391 2012-04-23 09:12:02Z lym $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file ztmtProcessMonitor.c
 * @brief Gliese Master ProcessMonitor Thread Routines
 */

#include <stl.h>
#include <knl.h>
#include <scl.h>
#include <sml.h>
#include <ell.h>
#include <qll.h>
#include <ssl.h>
#include <ztmDef.h>
#include <ztmt.h>

/**
 * process monitoring
 *
 * db상태에 따른 dispatcher, shared-server process실행과 종료 처리
 *
 * - open :
 *   - monitoring thread start :
 *     - process실행후 msg queue로 정상 상태 될때까지 기다림
 *       (msg queue는 각 process에서 send함)
 * - shutdown normal :
 *   - ZTM_STATE_STOP_PROCESS_MONITOR로 thread loop 빠져 나옴.
 *     - gZtmSharedMemory->mIsInit = STL_FALSE로 설정
 *     - stlIsActiveProc로 process종료 기다림.()
 * - shutdown abort  : KNL_CANCEL_THREAD후 stlIsActiveProc로 기다림.
 */


ztmThread                  gZtmProcessMonitorThread;
sllSharedMemory          * gZtmSharedMemory = NULL;
ztmState                   gZtmProcessMonitorState = ZTM_STATE_INIT;

extern stlInt64            gZtmBootdownKey;


void * STL_THREAD_FUNC ztmtProcessMonitorThread( stlThread * aThread, void * aArg )
{
    stlErrorStack         * sErrorStack = &((ztmThreadArgs *)aArg)->mErrorStack;
    stlInt32                sState      = 0;
    ztmEnv                * sEnv = NULL;
    ztmSessionEnv         * sSessionEnv = NULL;
    stlProc                 sProc;
    stlChar                 sProgram[STL_PATH_MAX];
    stlInt64                sDispatcherCount;
    stlInt64                sSharedServerCount;

    STL_INIT_ERROR_STACK( sErrorStack );

    STL_TRY( stlGetCurrentProc( &sProc,
                                sErrorStack ) == STL_SUCCESS );

    STL_TRY( stlGetProcName( &sProc,
                             sProgram,
                             STL_PATH_MAX,
                             sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( knlAllocEnv( (void**)&sEnv,
                          sErrorStack ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( sslInitializeEnv( SSL_ENV( sEnv ),
                               KNL_ENV_SHARED ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( knlAllocSessionEnv( (void**)&sSessionEnv,
                                 KNL_ENV( sEnv ) ) == STL_SUCCESS );
    sState = 3;

    STL_TRY( sslInitSessionEnv( (sslSessionEnv*)sSessionEnv,
                                KNL_CONNECTION_TYPE_DA,
                                KNL_SESSION_TYPE_DEDICATE,
                                KNL_SESS_ENV_SHARED,
                                SSL_ENV( sEnv ) ) == STL_SUCCESS );
    sState = 4;

    KNL_LINK_SESS_ENV( sEnv, sSessionEnv );

    ((sslSessionEnv*)sSessionEnv)->mClientProcId = sProc;
    stlSnprintf( ((sslSessionEnv*)sSessionEnv)->mProgram,
                 SSL_PROGRAM_NAME,
                 "%s: %s",
                 sProgram,
                 ZTM_THREAD_NAME_PROCESS_MONITOR );

    STL_TRY( ztmtStartProcess( gZtmSharedMemory,
                               sEnv ) == STL_SUCCESS );
    sState = 5;

    ZTM_SET_THREAD_STATE( &gZtmProcessMonitorThread, ZTM_THREAD_STATE_RUN );
    ZTM_SET_ENV( &gZtmProcessMonitorThread, sEnv );


    KNL_ENTER_CRITICAL_SECTION( sEnv );

    while( STL_TRUE )
    {
        STL_INIT_ERROR_STACK( KNL_ERROR_STACK(sEnv) );

        stlSleep( ZTM_MONITOR_SLEEP_TIME );      /* 1 초 */

        if( gZtmProcessMonitorState == ZTM_STATE_STOP_PROCESS_MONITOR )
        {
            STL_THROW( RAMP_EXIT );
        }

        KNL_CHECK_THREAD_CANCELLATION( KNL_ENV(sEnv) );

        sDispatcherCount = knlGetPropertyBigIntValueAtShmByID( KNL_PROPERTY_DISPATCHERS );

        sSharedServerCount = knlGetPropertyBigIntValueAtShmByID( KNL_PROPERTY_SHARED_SERVERS );

        /**
         * process를 늘이는것만 됨.
         * porcess를 줄이려면 정상 처리 완료후 process가 종료되어야 하는데.
         * 그동안 process에 처리(shutdown등)에 대한 고려가 필요함.
         */
        if( sDispatcherCount < gZtmSharedMemory->mDispatchers )
        {
            sDispatcherCount = gZtmSharedMemory->mDispatchers;
        }
        if( sSharedServerCount < gZtmSharedMemory->mSharedServers )
        {
            sSharedServerCount = gZtmSharedMemory->mSharedServers;
        }

        /*  process 개수 변경 처리 */
        if( gZtmSharedMemory->mDispatchers < sDispatcherCount )
        {
            (void)ztmtStartDispatcher( gZtmSharedMemory->mDispatchers,
                                       sDispatcherCount,
                                       sEnv );

            gZtmSharedMemory->mDispatchers = sDispatcherCount;
        }

        if( gZtmSharedMemory->mSharedServers < sSharedServerCount )
        {
            (void)ztmtStartSharedServer( gZtmSharedMemory->mSharedServers,
                                         sSharedServerCount,
                                         sEnv );

            gZtmSharedMemory->mSharedServers = sSharedServerCount;
        }


        /**
         *  shared Session 관련 process를 관리한다.
         */
        if( ztmtCheckProcess( gZtmSharedMemory,
                              sEnv ) == STL_FAILURE )
        {
            knlLogMsg( NULL,
                       KNL_ENV(sEnv),
                       KNL_LOG_LEVEL_INFO,
                       "[PROCESS MONITOR] failed to check process status \n" );
        }

    }

    STL_RAMP( RAMP_EXIT );

    KNL_LEAVE_CRITICAL_SECTION( sEnv );

    sState = 4;
    (void)ztmtStopProcess( gZtmSharedMemory, sEnv );

    sState = 3;
    (void)sslFiniSessionEnv( (sslSessionEnv*)sSessionEnv, SSL_ENV( sEnv ) );

    sState = 2;
    (void)knlFreeSessionEnv( (void*)sSessionEnv, KNL_ENV( sEnv ) );

    sState = 1;
    (void)sslFinalizeEnv( SSL_ENV( sEnv ) );
    
    sState = 0;
    (void)knlFreeEnv( (void*)sEnv, sErrorStack );

    ZTM_SET_THREAD_STATE( &gZtmProcessMonitorThread, ZTM_THREAD_STATE_INIT );

    return NULL;

    STL_FINISH;

    switch( sState )
    {
        case 5 :
            (void)ztmtStopProcess( gZtmSharedMemory, sEnv );
        case 4 :
            (void)sslFiniSessionEnv( (sslSessionEnv*)sSessionEnv,
                                     SSL_ENV( sEnv ) );
        case 3 :
            (void)knlFreeSessionEnv( (void*)sSessionEnv,
                                     KNL_ENV( sEnv ) );
            knlLogErrorCallStack( NULL, KNL_ENV(sEnv) ); 
            knlLogMsg( NULL,
                       KNL_ENV(sEnv),
                       KNL_LOG_LEVEL_FATAL,
                       "[PROCESS MONITOR] abnormally terminated : %s \n",
                       stlGetLastErrorMessage( KNL_ERROR_STACK( sEnv ) ) );
        case 2 :
            sslFinalizeEnv( SSL_ENV( sEnv ) );
        case 1 :
            stlAppendErrors( sErrorStack, KNL_ERROR_STACK(sEnv) );

            knlFreeEnv( (void*)sEnv,
                        sErrorStack );
        default:
            break;
    }

    ZTM_SET_THREAD_STATE( &gZtmProcessMonitorThread, ZTM_THREAD_STATE_FAIL );

    stlExitThread( aThread, STL_FAILURE, sErrorStack );
    
    return NULL;
}

stlStatus ztmtCreateProcessMonitorThread( ztmEnv * aSystemEnv )
{
    ZTM_SET_THREAD_STATE( &gZtmProcessMonitorThread, ZTM_THREAD_STATE_INIT );
    
    STL_TRY( stlCreateThread( &gZtmProcessMonitorThread.mThread,
                              NULL,
                              ztmtProcessMonitorThread,
                              (void *)&gZtmProcessMonitorThread.mArgs,
                              KNL_ERROR_STACK( aSystemEnv ) ) == STL_SUCCESS );
    
    ZTM_WAIT_THREAD( &gZtmProcessMonitorThread );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztmtStopProcessMonitorThread( ztmEnv * aSystemEnv )
{
    stlStatus sReturnStatus;

    gZtmProcessMonitorState = ZTM_STATE_STOP_PROCESS_MONITOR;
    
    STL_TRY( stlJoinThread( &gZtmProcessMonitorThread.mThread,
                            &sReturnStatus,
                            KNL_ERROR_STACK( aSystemEnv ) ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * process를 실행한다.
 */
stlStatus ztmtStartProcess( sllSharedMemory  * aSharedMemory,
                            ztmEnv           * aSystemEnv )
{
    STL_TRY( ztmtStartBalancer( aSystemEnv ) == STL_SUCCESS );

    STL_TRY( ztmtStartDispatcher( 0,
                                  gZtmSharedMemory->mDispatchers,
                                  aSystemEnv ) == STL_SUCCESS );

    STL_TRY( ztmtStartSharedServer( 0,
                                    gZtmSharedMemory->mSharedServers,
                                    aSystemEnv ) == STL_SUCCESS );
    return STL_SUCCESS;

    STL_FINISH;

    (void)ztmtStopProcess( aSharedMemory, aSystemEnv );

    return STL_FAILURE;
}


/**
 * Balancer를 실행한다.
 */
stlStatus ztmtStartBalancer( ztmEnv           * aSystemEnv )
{
    stlChar           * sArgv[12];

    sArgv[0] = "gbalancer";
    sArgv[1] = NULL;

    if( stlExecuteProcAndWait( sArgv, KNL_ERROR_STACK(aSystemEnv) ) == STL_FAILURE )
    {
        knlLogMsg( NULL,
                   KNL_ENV(aSystemEnv),
                   KNL_LOG_LEVEL_INFO,
                   "[PROCESS MONITOR] failed to create process [%s] - %s \n",
                       sArgv[0],
                       stlGetLastErrorMessage( KNL_ERROR_STACK( aSystemEnv ) ) );

        STL_TRY( STL_FALSE );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * dispatcher를 실행한다.
 */
stlStatus ztmtStartDispatcher( stlInt32           aStartIdx,
                               stlInt32           aEndIdx,
                               ztmEnv           * aSystemEnv )
{
    stlInt32            sIdx;
    stlChar             sStrIndex[10];
    stlChar           * sArgv[12];

    for( sIdx = aStartIdx; sIdx < aEndIdx; sIdx++ )
    {
        KNL_CHECK_THREAD_CANCELLATION( KNL_ENV(aSystemEnv) );

        stlSnprintf( sStrIndex, STL_SIZEOF(sStrIndex),
                     "%d",
                     sIdx );

        sArgv[0] = "gdispatcher";
        sArgv[1] = "--index";
        sArgv[2] = sStrIndex;
        sArgv[3] = NULL;

        if( stlExecuteProcAndWait( sArgv, KNL_ERROR_STACK(aSystemEnv) ) == STL_FAILURE )
        {
            knlLogMsg( NULL,
                       KNL_ENV(aSystemEnv),
                       KNL_LOG_LEVEL_INFO,
                       "[PROCESS MONITOR] failed to create process [%s] - %s \n",
                       sArgv[0],
                       stlGetLastErrorMessage( KNL_ERROR_STACK( aSystemEnv ) ) );

            STL_TRY( STL_FALSE );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * shared-server를 실행한다.
 */
stlStatus ztmtStartSharedServer( stlInt32           aStartIdx,
                                 stlInt32           aEndIdx,
                                 ztmEnv           * aSystemEnv )
{
    stlInt32            sIdx;
    stlChar             sStrIndex[10];
    stlChar           * sArgv[12];

    for( sIdx = aStartIdx; sIdx < aEndIdx; sIdx++ )
    {
        KNL_CHECK_THREAD_CANCELLATION( KNL_ENV(aSystemEnv) );

        stlSnprintf( sStrIndex, STL_SIZEOF(sStrIndex),
                     "%d",
                     sIdx );

        sArgv[0] = "gserver";
        sArgv[1] = "--shared";
        sArgv[2] = sStrIndex;
        sArgv[3] = NULL;

        if( stlExecuteProcAndWait( sArgv, KNL_ERROR_STACK(aSystemEnv) ) == STL_FAILURE )
        {
            knlLogMsg( NULL,
                       KNL_ENV(aSystemEnv),
                       KNL_LOG_LEVEL_INFO,
                       "[PROCESS MONITOR] failed to create process [%s] - %s \n",
                       sArgv[0],
                       stlGetLastErrorMessage( KNL_ERROR_STACK( aSystemEnv ) ) );

            STL_TRY( STL_FALSE );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * balancer, dispatcher, shared-server가 종료할때까지 기다린다
 */
stlStatus ztmtStopProcess( sllSharedMemory  * aSharedMemory,
                           ztmEnv           * aSystemEnv )
{
    stlInt32            sIdx;

    sllBalancer       * sBalancer;
    sllDispatcher     * sDispatcher;
    sllSharedServer   * sSharedServer;

    /* dispatcher, shared-server가 죽도록 mExitFlag을 true로 설정 */
    aSharedMemory->mExitFlag = STL_TRUE;

    sBalancer = &aSharedMemory->mBalancer;
    sDispatcher = aSharedMemory->mDispatcher;
    sSharedServer = aSharedMemory->mSharedServer;

    if( sBalancer->mThread.mProc.mProcID != 0 )
    {
        while( stlIsActiveProc( &sBalancer->mThread.mProc ) == STL_TRUE )
        {
            stlSleep( STL_SET_MSEC_TIME(100) );
        }
    }

    for( sIdx = 0; sIdx < aSharedMemory->mDispatchers; sIdx++ )
    {
        if( sDispatcher[sIdx].mThread.mProc.mProcID == 0 )
        {
            continue;
        }

        while( stlIsActiveProc( &sDispatcher[sIdx].mThread.mProc ) == STL_TRUE )
        {
            stlSleep( STL_SET_MSEC_TIME(100) );
        }
    }

    for( sIdx = 0; sIdx < aSharedMemory->mSharedServers; sIdx++ )
    {
        if( sSharedServer[sIdx].mThread.mProc.mProcID == 0 )
        {
            continue;
        }

        while( stlIsActiveProc( &sSharedServer[sIdx].mThread.mProc ) == STL_TRUE )
        {
            stlSleep( STL_SET_MSEC_TIME(100) );
        }
    }

    return STL_SUCCESS;
}


/**
 * dispatcher, shared-server가 즉시 종료하도록 한다.
 */
stlStatus ztmtKillProcess( sllSharedMemory  * aSharedMemory,
                           ztmEnv           * aSystemEnv )
{
    stlInt32            sIdx;

    sllBalancer       * sBalancer;
    sllDispatcher     * sDispatcher;
    sllSharedServer   * sSharedServer;

    sBalancer = &aSharedMemory->mBalancer;
    sDispatcher = aSharedMemory->mDispatcher;
    sSharedServer = aSharedMemory->mSharedServer;

    KNL_CANCEL_THREAD( KNL_ENV(sBalancer->mEnv) );

    for( sIdx = 0; sIdx < aSharedMemory->mDispatchers; sIdx++ )
    {
        KNL_CANCEL_THREAD( KNL_ENV(sDispatcher[sIdx].mSendThreadEnv) );
        KNL_CANCEL_THREAD( KNL_ENV(sDispatcher[sIdx].mMainThreadEnv) );
    }

    for( sIdx = 0; sIdx < aSharedMemory->mSharedServers; sIdx++ )
    {
        KNL_CANCEL_THREAD( KNL_ENV(sSharedServer[sIdx].mEnv) );
    }

    STL_TRY( ztmtStopProcess( aSharedMemory, aSystemEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * dispatcher, shared-server가 죽었으면 다시 살린다.
 */
stlStatus ztmtCheckProcess( sllSharedMemory  * aSharedMemory,
                            ztmEnv            * aSystemEnv )
{
    stlInt32            sIdx;
    sllBalancer       * sBalancer;
    sllDispatcher     * sDispatcher;
    sllSharedServer   * sSharedServer;

    sBalancer = &aSharedMemory->mBalancer;
    sDispatcher = aSharedMemory->mDispatcher;
    sSharedServer = aSharedMemory->mSharedServer;

    /* process가 죽었으면 다시 실행한다 */
    if( stlIsActiveProc( &sBalancer->mThread.mProc ) == STL_FALSE )
    {
        STL_TRY( ztmtStartBalancer( aSystemEnv )
                 == STL_SUCCESS );
    }

    for( sIdx = 0; sIdx < aSharedMemory->mDispatchers; sIdx++ )
    {
        /* process가 죽었으면 다시 실행한다 */
        if( stlIsActiveProc( &sDispatcher[sIdx].mThread.mProc ) == STL_FALSE )
        {
            STL_TRY( ztmtStartDispatcher( sIdx,
                                          sIdx + 1,
                                          aSystemEnv )
                     == STL_SUCCESS );
        }

        KNL_CHECK_THREAD_CANCELLATION( KNL_ENV(aSystemEnv) );
    }

    for( sIdx = 0; sIdx < aSharedMemory->mSharedServers; sIdx++ )
    {
        /* process가 죽었으면 다시 실행한다 */
        if( stlIsActiveProc( &sSharedServer[sIdx].mThread.mProc ) == STL_FALSE )
        {
            STL_TRY( ztmtStartSharedServer( sIdx,
                                            sIdx + 1,
                                            aSystemEnv )
                     == STL_SUCCESS );
        }

        KNL_CHECK_THREAD_CANCELLATION( KNL_ENV(aSystemEnv) );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

