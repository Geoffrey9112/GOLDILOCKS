/*******************************************************************************
 * ztmtTimer.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztmtTimer.c 4391 2012-04-23 09:12:02Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file ztmtTimer.c
 * @brief Gliese Master Timer Thread Routines
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

ztmThread gZtmTimerThread;

extern ztmWarmupEntry ** gZtmWarmupEntry;

void * STL_THREAD_FUNC ztmtTimerThread( stlThread * aThread, void * aArg )
{
    stlErrorStack * sErrorStack = &((ztmThreadArgs *)aArg)->mErrorStack;
    stlInt32        sState     = 0;
    ztmEnv        * sEnv;
    ztmSessionEnv * sSessionEnv;
    stlProc         sProc;
    stlChar         sProgram[STL_PATH_MAX];

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
                 ZTM_THREAD_NAME_TIMER );

    ZTM_SET_THREAD_STATE( &gZtmTimerThread, ZTM_THREAD_STATE_RUN );
    ZTM_SET_ENV( &gZtmTimerThread, sEnv );

    /**
     * System Timer를 설정한다.
     */
        
    STL_TRY( knlRegisterSystemTimer( KNL_ENV(sEnv) ) == STL_SUCCESS );

    STL_TRY( stlUnblockRealtimeThreadSignals( KNL_ERROR_STACK( sEnv ) ) == STL_SUCCESS );
    
    KNL_ENTER_CRITICAL_SECTION( sEnv );
        
    while(1)
    {
        KNL_CHECK_THREAD_CANCELLATION( KNL_ENV(sEnv) );
        
        stlSleep( ZTM_TIMER_SLEEP_TIME );      /* 1 초 */
        
        if( (*gZtmWarmupEntry)->mState == ZTM_STATE_STOP_TIMER )
        {
            STL_THROW( RAMP_EXIT );
        }
    }

    STL_RAMP( RAMP_EXIT );

    KNL_LEAVE_CRITICAL_SECTION( sEnv );
        
    STL_TRY( knlUnregisterSystemTimer( KNL_ENV(sEnv) ) == STL_SUCCESS );
        
    sState = 3;
    STL_TRY( sslFiniSessionEnv( (sslSessionEnv*)sSessionEnv,
                                SSL_ENV( sEnv ) ) == STL_SUCCESS );

    sState = 2;
    STL_TRY( knlFreeSessionEnv( (void*)sSessionEnv,
                                KNL_ENV( sEnv ) ) == STL_SUCCESS );

    sState = 1;
    STL_TRY( sslFinalizeEnv( SSL_ENV( sEnv ) ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( knlFreeEnv( (void*)sEnv,
                         sErrorStack ) == STL_SUCCESS );

    ZTM_SET_THREAD_STATE( &gZtmTimerThread, ZTM_THREAD_STATE_INIT );

    return NULL;

    STL_FINISH;

    switch( sState )
    {
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
                       "[TIMER] abnormally terminated\n" );
        case 2 :
            sslFinalizeEnv( SSL_ENV( sEnv ) );
        case 1 :
            stlAppendErrors( sErrorStack, KNL_ERROR_STACK(sEnv) );

            knlFreeEnv( (void*)sEnv,
                        sErrorStack );
    }

    ZTM_SET_THREAD_STATE( &gZtmTimerThread, ZTM_THREAD_STATE_FAIL );

    stlExitThread( aThread, STL_FAILURE, sErrorStack );
    
    return NULL;
}

stlStatus ztmtCreateTimerThread( ztmEnv * aSystemEnv )
{
    ZTM_SET_THREAD_STATE( &gZtmTimerThread, ZTM_THREAD_STATE_INIT );
    
    STL_TRY( stlCreateThread( &gZtmTimerThread.mThread,
                              NULL,
                              ztmtTimerThread,
                              (void *)&gZtmTimerThread.mArgs,
                              KNL_ERROR_STACK( aSystemEnv ) ) == STL_SUCCESS );
    
    ZTM_WAIT_THREAD( &gZtmTimerThread );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztmtStopTimerThread( ztmEnv * aSystemEnv )
{
    stlStatus sReturnStatus;

    (*gZtmWarmupEntry)->mState = ZTM_STATE_STOP_TIMER;
    
    STL_TRY( stlJoinThread( &gZtmTimerThread.mThread,
                            &sReturnStatus,
                            KNL_ERROR_STACK( aSystemEnv ) ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

