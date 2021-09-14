/*******************************************************************************
 * ztmtArchivelog.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztmtArchivelog.c 1303 2011-07-01 06:20:38Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file ztmtArchivelog.c
 * @brief Gliese Master Archive Log Routines
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

ztmThread         gZtmArchivelogThread;
ztmEnv          * gZtmArchivelogEnv;
ztmSessionEnv   * gZtmArchivelogSessionEnv;

extern ztmEnv            gZtmSystemEnv;
extern ztmWarmupEntry ** gZtmWarmupEntry;

void * STL_THREAD_FUNC ztmtArchivelogThread( stlThread * aThread, void * aArg )
{
    stlErrorStack    * sErrorStack = &((ztmThreadArgs *)aArg)->mErrorStack;
    stlInt32           sState     = 0;
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
    
    sEventListener = &(*gZtmWarmupEntry)->mArchivelogEvent;
    
    STL_TRY( knlAllocEnv( (void**)&gZtmArchivelogEnv,
                          sErrorStack ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( sslInitializeEnv( SSL_ENV( gZtmArchivelogEnv ),
                               KNL_ENV_SHARED ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( knlAllocSessionEnv( (void**)&gZtmArchivelogSessionEnv,
                                 KNL_ENV( gZtmArchivelogEnv ) ) == STL_SUCCESS );
    sState = 3;

    STL_TRY( sslInitSessionEnv( (sslSessionEnv*)gZtmArchivelogSessionEnv,
                                KNL_CONNECTION_TYPE_DA,
                                KNL_SESSION_TYPE_DEDICATE,
                                KNL_SESS_ENV_SHARED,
                                SSL_ENV( gZtmArchivelogEnv ) ) == STL_SUCCESS );
    sState = 4;

    KNL_LINK_SESS_ENV( gZtmArchivelogEnv, gZtmArchivelogSessionEnv );

    ((sslSessionEnv*)gZtmArchivelogSessionEnv)->mClientProcId = sProc;
    stlSnprintf( ((sslSessionEnv*)gZtmArchivelogSessionEnv)->mProgram,
                 SSL_PROGRAM_NAME,
                 "%s: %s",
                 sProgram,
                 ZTM_THREAD_NAME_ARCHIVELOG );

    knlRegisterEventListener( sEventListener, KNL_ENV(gZtmArchivelogEnv) );
    smlRegisterArchivelogEnv( SML_ENV(gZtmArchivelogEnv) );

    ZTM_SET_THREAD_STATE( &gZtmArchivelogThread, ZTM_THREAD_STATE_RUN );
    ZTM_SET_ENV( &gZtmArchivelogThread, gZtmArchivelogEnv );
    
    KNL_ENTER_CRITICAL_SECTION( gZtmArchivelogEnv );
        
    while(1)
    {
        STL_TRY( knlWaitEvent( sEventListener,
                               STL_INFINITE_TIME,
                               &sIsTimedOut,
                               KNL_ENV( gZtmArchivelogEnv ) ) == STL_SUCCESS );

        if( (*gZtmWarmupEntry)->mState == ZTM_STATE_STOP_ARCHIVELOG )
        {
            break;
        }

        /*
         * do archive log
         */

        STL_TRY( knlExecuteEnvEvent( KNL_ENV( gZtmArchivelogEnv ),
                                     &sEventId,
                                     &sIsCanceled,
                                     &sIsSuccess )
                 == STL_SUCCESS );

        if( (*gZtmWarmupEntry)->mState == ZTM_STATE_STOP_ARCHIVELOG )
        {
            break;
        }
    }

    KNL_LEAVE_CRITICAL_SECTION( gZtmArchivelogEnv );
        
    sState = 3;
    STL_TRY( sslFiniSessionEnv( (sslSessionEnv*)gZtmArchivelogSessionEnv,
                                SSL_ENV( gZtmArchivelogEnv ) ) == STL_SUCCESS );

    sState = 2;
    STL_TRY( knlFreeSessionEnv( (void*)gZtmArchivelogSessionEnv,
                                KNL_ENV( gZtmArchivelogEnv ) ) == STL_SUCCESS );

    sState = 1;
    STL_TRY( sslFinalizeEnv( SSL_ENV( gZtmArchivelogEnv ) ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( knlFreeEnv( (void*)gZtmArchivelogEnv,
                         sErrorStack ) == STL_SUCCESS );

    ZTM_SET_THREAD_STATE( &gZtmArchivelogThread, ZTM_THREAD_STATE_INIT );

    return NULL;

    STL_FINISH;
    
    switch( sState )
    {
        case 4 :
            (void)sslFiniSessionEnv( (sslSessionEnv*)gZtmArchivelogSessionEnv,
                                     SSL_ENV( gZtmArchivelogEnv ) );
        case 3 :
            (void)knlFreeSessionEnv( (void*)gZtmArchivelogSessionEnv,
                                     KNL_ENV( gZtmArchivelogEnv ) );
            knlLogErrorCallStack( NULL, KNL_ENV(gZtmArchivelogEnv) ); 
            knlLogMsg( NULL,
                       KNL_ENV(gZtmArchivelogEnv),
                       KNL_LOG_LEVEL_FATAL,
                       "[ARCHIVER] abnormally terminated\n" );
        case 2 :
            sslFinalizeEnv( SSL_ENV( gZtmArchivelogEnv ) );
        case 1 :
            stlAppendErrors( sErrorStack, KNL_ERROR_STACK(gZtmArchivelogEnv) );

            knlFreeEnv( (void*)gZtmArchivelogEnv,
                        sErrorStack );
    }

    ZTM_SET_THREAD_STATE( &gZtmArchivelogThread, ZTM_THREAD_STATE_FAIL );

    stlExitThread( aThread, STL_FAILURE, sErrorStack );
    
    return NULL;
}

stlStatus ztmtCreateArchivelogThread( ztmEnv * aSystemEnv )
{
    STL_TRY( stlCreateThread( &gZtmArchivelogThread.mThread,
                              NULL,
                              ztmtArchivelogThread,
                              (void *)&gZtmArchivelogThread.mArgs,
                              KNL_ERROR_STACK( aSystemEnv ) ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztmtStopArchivelogThread( ztmEnv * aSystemEnv )
{
    stlStatus sReturnStatus;

    (*gZtmWarmupEntry)->mState = ZTM_STATE_STOP_ARCHIVELOG;

    STL_TRY( knlWakeupEventListener( &(*gZtmWarmupEntry)->mArchivelogEvent,
                                     KNL_ENV( &gZtmSystemEnv ) ) == STL_SUCCESS );

    if( ZTM_IS_STARTED( &gZtmArchivelogThread ) == STL_TRUE )
    {
        STL_TRY( stlJoinThread( &gZtmArchivelogThread.mThread,
                                &sReturnStatus,
                                KNL_ERROR_STACK( aSystemEnv ) ) == STL_SUCCESS );
    }
                            
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}
