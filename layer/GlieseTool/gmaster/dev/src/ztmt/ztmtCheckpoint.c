/*******************************************************************************
 * ztmtCheckPoint.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztmtCheckPoint.c 4391 2012-04-23 09:12:02Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file ztmtCheckPoint.c
 * @brief Gliese Master CheckPoint Routines
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

ztmThread         gZtmCheckPointThread;
ztmEnv          * gZtmCheckPointEnv;
ztmSessionEnv   * gZtmCheckPointSessionEnv;

extern ztmEnv            gZtmSystemEnv;
extern ztmWarmupEntry ** gZtmWarmupEntry;

void * STL_THREAD_FUNC ztmtCheckPointThread( stlThread * aThread, void * aArg )
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
    
    sEventListener = &(*gZtmWarmupEntry)->mCheckpointEvent;
    
    STL_TRY( knlAllocEnv( (void**)&gZtmCheckPointEnv,
                          sErrorStack ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( sslInitializeEnv( SSL_ENV( gZtmCheckPointEnv ),
                               KNL_ENV_SHARED ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( knlAllocSessionEnv( (void**)&gZtmCheckPointSessionEnv,
                                 KNL_ENV( gZtmCheckPointEnv ) ) == STL_SUCCESS );
    sState = 3;

    STL_TRY( sslInitSessionEnv( (sslSessionEnv*)gZtmCheckPointSessionEnv,
                                KNL_CONNECTION_TYPE_DA,
                                KNL_SESSION_TYPE_DEDICATE,
                                KNL_SESS_ENV_SHARED,
                                SSL_ENV( gZtmCheckPointEnv ) ) == STL_SUCCESS );
    sState = 4;

    KNL_LINK_SESS_ENV( gZtmCheckPointEnv, gZtmCheckPointSessionEnv );

    ((sslSessionEnv*)gZtmCheckPointSessionEnv)->mClientProcId = sProc;
    stlSnprintf( ((sslSessionEnv*)gZtmCheckPointSessionEnv)->mProgram,
                 SSL_PROGRAM_NAME,
                 "%s: %s",
                 sProgram,
                 ZTM_THREAD_NAME_CHECK_POINT );

    knlRegisterEventListener( sEventListener, KNL_ENV(gZtmCheckPointEnv) );
    smlRegisterCheckpointEnv( SML_ENV(gZtmCheckPointEnv) );

    ZTM_SET_THREAD_STATE( &gZtmCheckPointThread, ZTM_THREAD_STATE_RUN );
    ZTM_SET_ENV( &gZtmCheckPointThread, gZtmCheckPointEnv );

    KNL_ENTER_CRITICAL_SECTION( gZtmCheckPointEnv );
        
    while(1)
    {
        STL_TRY( knlWaitEvent( sEventListener,
                               STL_INFINITE_TIME,
                               &sIsTimedOut,
                               KNL_ENV( gZtmCheckPointEnv ) ) == STL_SUCCESS );

        if( (*gZtmWarmupEntry)->mState == ZTM_STATE_STOP_CHECK_POINT )
        {
            break;
        }

        if( smlGetDataStoreMode() != SML_DATA_STORE_MODE_TDS )
        {
            continue;
        }
        
        if( smlGetDataAccessMode() == SML_DATA_ACCESS_MODE_READ_ONLY )
        {
            continue;
        }
        
        /*
         * do checkpoint
         */

        STL_TRY( knlExecuteEnvEvent( KNL_ENV( gZtmCheckPointEnv ),
                                     &sEventId,
                                     &sIsCanceled,
                                     &sIsSuccess )
                 == STL_SUCCESS );

        if( (*gZtmWarmupEntry)->mState == ZTM_STATE_STOP_CHECK_POINT )
        {
            break;
        }
    }

    KNL_LEAVE_CRITICAL_SECTION( gZtmCheckPointEnv );
        
    sState = 3;
    STL_TRY( sslFiniSessionEnv( (sslSessionEnv*)gZtmCheckPointSessionEnv,
                                SSL_ENV( gZtmCheckPointEnv ) ) == STL_SUCCESS );

    sState = 2;
    STL_TRY( knlFreeSessionEnv( (void*)gZtmCheckPointSessionEnv,
                                KNL_ENV( gZtmCheckPointEnv ) ) == STL_SUCCESS );

    sState = 1;
    STL_TRY( sslFinalizeEnv( SSL_ENV( gZtmCheckPointEnv ) ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( knlFreeEnv( (void*)gZtmCheckPointEnv,
                         sErrorStack ) == STL_SUCCESS );

    ZTM_SET_THREAD_STATE( &gZtmCheckPointThread, ZTM_THREAD_STATE_INIT );
    
    return NULL;

    STL_FINISH;

    switch( sState )
    {
        case 4 :
            (void)sslFiniSessionEnv( (sslSessionEnv*)gZtmCheckPointSessionEnv,
                                     SSL_ENV( gZtmCheckPointEnv ) );
        case 3 :
            (void)knlFreeSessionEnv( (void*)gZtmCheckPointSessionEnv,
                                     KNL_ENV( gZtmCheckPointEnv ) );
            knlLogErrorCallStack( NULL, KNL_ENV(gZtmCheckPointEnv) ); 
            knlLogMsg( NULL,
                       KNL_ENV(gZtmCheckPointEnv),
                       KNL_LOG_LEVEL_FATAL,
                       "[CHECKPOINTER] abnormally terminated\n" );
        case 2 :
            sslFinalizeEnv( SSL_ENV( gZtmCheckPointEnv ) );
        case 1 :
            stlAppendErrors( sErrorStack, KNL_ERROR_STACK(gZtmCheckPointEnv) );

            knlFreeEnv( (void*)gZtmCheckPointEnv,
                        sErrorStack );
    }

    ZTM_SET_THREAD_STATE( &gZtmCheckPointThread, ZTM_THREAD_STATE_FAIL );

    stlExitThread( aThread, STL_FAILURE, sErrorStack );
    
    return NULL;
}

stlStatus ztmtCreateCheckPointThread( ztmEnv * aSystemEnv )
{
    STL_TRY( stlCreateThread( &gZtmCheckPointThread.mThread,
                              NULL,
                              ztmtCheckPointThread,
                              (void *)&gZtmCheckPointThread.mArgs,
                              KNL_ERROR_STACK( aSystemEnv ) ) == STL_SUCCESS );

    STL_TRY( smlSetCheckpointerState( SML_CHECKPOINTER_STATE_ACTIVE,
                                      SML_ENV( aSystemEnv ) ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztmtStopCheckPointThread( ztmEnv * aSystemEnv )
{
    stlStatus sReturnStatus;

    (*gZtmWarmupEntry)->mState = ZTM_STATE_STOP_CHECK_POINT;

    STL_TRY( knlWakeupEventListener( &(*gZtmWarmupEntry)->mCheckpointEvent,
                                     KNL_ENV( &gZtmSystemEnv ) ) == STL_SUCCESS );

    if( ZTM_IS_STARTED( &gZtmCheckPointThread ) == STL_TRUE )
    {
        STL_TRY( stlJoinThread( &gZtmCheckPointThread.mThread,
                                &sReturnStatus,
                                KNL_ERROR_STACK( aSystemEnv ) ) == STL_SUCCESS );
    }

    STL_TRY( smlSetCheckpointerState( SML_CHECKPOINTER_STATE_INACTIVE,
                                      SML_ENV( aSystemEnv ) ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}
