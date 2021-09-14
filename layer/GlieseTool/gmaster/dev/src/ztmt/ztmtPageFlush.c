/*******************************************************************************
 * ztmtPageFlush.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztmtPageFlush.c 1519 2011-07-25 04:47:51Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file ztmtPageFlush.c
 * @brief Gliese Master Page Flush Routines
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

ztmThread gZtmPageFlushThread;

extern ztmEnv            gZtmSystemEnv;
extern ztmWarmupEntry ** gZtmWarmupEntry;

void * STL_THREAD_FUNC ztmtPageFlushThread( stlThread * aThread, void * aArg )
{
    stlErrorStack    * sErrorStack = &((ztmThreadArgs *)aArg)->mErrorStack;
    stlInt32           sState     = 0;
    ztmEnv           * sEnv;
    ztmSessionEnv    * sSessionEnv;
    knlEventListener * sEventListener;
    stlBool            sIsTimedOut;
    stlBool            sIsSuccess;
    stlBool            sIsCanceled;
    stlInt32           sEventId;
    stlProc            sProc;
    stlChar            sProgram[STL_PATH_MAX];

    STL_INIT_ERROR_STACK( sErrorStack );

    STL_TRY( stlBlockRealtimeThreadSignals( sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlGetCurrentProc( &sProc,
                                sErrorStack ) == STL_SUCCESS );

    STL_TRY( stlGetProcName( &sProc,
                             sProgram,
                             STL_PATH_MAX,
                             sErrorStack ) == STL_SUCCESS );
    
    sEventListener = &(*gZtmWarmupEntry)->mPageFlushEvent;
    
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
                 ZTM_THREAD_NAME_PAGE_FLUSH );

    knlRegisterEventListener( sEventListener, KNL_ENV(sEnv) );
    smlRegisterPageFlusherEnv( SML_ENV(sEnv) );
    
    ZTM_SET_THREAD_STATE( &gZtmPageFlushThread, ZTM_THREAD_STATE_RUN );
    ZTM_SET_ENV( &gZtmPageFlushThread, sEnv );
    
    KNL_ENTER_CRITICAL_SECTION( sEnv );
        
    while(1)
    {
        STL_TRY( knlWaitEvent( sEventListener,
                               STL_INFINITE_TIME,
                               &sIsTimedOut,
                               KNL_ENV( sEnv ) ) == STL_SUCCESS );

        if( (*gZtmWarmupEntry)->mState == ZTM_STATE_STOP_PAGE_FLUSH )
        {
            break;
        }
        
        /*
         * 이벤트 확인
         */
        STL_TRY( knlExecuteEnvEvent( KNL_ENV( sEnv ),
                                     &sEventId,
                                     &sIsCanceled,
                                     &sIsSuccess ) == STL_SUCCESS );
        
        if( (*gZtmWarmupEntry)->mState == ZTM_STATE_STOP_PAGE_FLUSH )
        {
            break;
        }
    }

    KNL_LEAVE_CRITICAL_SECTION( sEnv );
        
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

    ZTM_SET_THREAD_STATE( &gZtmPageFlushThread, ZTM_THREAD_STATE_INIT );
    
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
                       "[PAGE FLUSHER] abnormally terminated\n" );
        case 2 :
            sslFinalizeEnv( SSL_ENV( sEnv ) );
        case 1 :
            stlAppendErrors( sErrorStack, KNL_ERROR_STACK(sEnv) );

            knlFreeEnv( (void*)sEnv,
                        sErrorStack );
    }

    ZTM_SET_THREAD_STATE( &gZtmPageFlushThread, ZTM_THREAD_STATE_FAIL );
    
    stlExitThread( aThread, STL_FAILURE, sErrorStack );
    
    return NULL;
}

stlStatus ztmtCreatePageFlushThread( ztmEnv * aSystemEnv )
{
    STL_TRY( stlCreateThread( &gZtmPageFlushThread.mThread,
                              NULL,
                              ztmtPageFlushThread,
                              (void *)&gZtmPageFlushThread.mArgs,
                              KNL_ERROR_STACK( aSystemEnv ) ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztmtStopPageFlushThread( ztmEnv * aSystemEnv )
{
    stlStatus sReturnStatus;

    (*gZtmWarmupEntry)->mState = ZTM_STATE_STOP_PAGE_FLUSH;

    STL_TRY( knlWakeupEventListener( &(*gZtmWarmupEntry)->mPageFlushEvent,
                                     KNL_ENV( &gZtmSystemEnv ) ) == STL_SUCCESS );

    if( ZTM_IS_STARTED( &gZtmPageFlushThread ) == STL_TRUE )
    {
        STL_TRY( stlJoinThread( &gZtmPageFlushThread.mThread,
                                &sReturnStatus,
                                KNL_ERROR_STACK( aSystemEnv ) ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

