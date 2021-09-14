/*******************************************************************************
 * ztmtAger.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztmtAger.c 4391 2012-04-23 09:12:02Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file ztmtAger.c
 * @brief Gliese Master Ager Thread Routines
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

ztmThread gZtmAgerThread;

extern ztmEnv            gZtmSystemEnv;
extern ztmWarmupEntry ** gZtmWarmupEntry;

void * STL_THREAD_FUNC ztmtAgerThread( stlThread * aThread, void * aArg )
{
    stlErrorStack * sErrorStack = &((ztmThreadArgs *)aArg)->mErrorStack;
    stlInt32        sState     = 0;
    ztmEnv        * sEnv;
    ztmSessionEnv * sSessionEnv;
    stlBool         sIsSuccess;
    stlBool         sIsCanceled;
    stlInt32        sEventId;
    stlInt64        sIntervalTime;
    stlProc         sProc;
    stlChar         sProgram[STL_PATH_MAX];
    stlTime         sLastShrinkTime;

    STL_INIT_ERROR_STACK( sErrorStack );

    STL_TRY( stlBlockRealtimeThreadSignals( sErrorStack ) == STL_SUCCESS );
    
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

    stlSnprintf( sProgram,
                 SSL_PROGRAM_NAME,
                 "%s: %s",
                 sProgram,
                 ZTM_THREAD_NAME_AGER );

    (void) sslSetSessionProcessInfo( &sProc,
                                     sProgram,
                                     (sslSessionEnv*)sSessionEnv );
    
    smlRegisterAgerEnv( SML_ENV( sEnv ) );

    ZTM_SET_THREAD_STATE( &gZtmAgerThread, ZTM_THREAD_STATE_RUN );
    ZTM_SET_ENV( &gZtmAgerThread, sEnv );

    /**
     * Startup시 최초 한번은 Agable Scn을 빌드한다.
     * - AGER INTERVAL TIME이 무한대일 경우 Agable Scn이 무한대로 남아 있을수 있다.
     * - Agable Scn이 무한대가 되면 모든 변경사항은 그 즉시 Againg될수 있다.
     */
    STL_TRY( smlBuildAgableScn( SML_ENV( sEnv ) ) == STL_SUCCESS );
    
    KNL_ENTER_CRITICAL_SECTION( sEnv );

    sLastShrinkTime = knlGetSystemTime();
        
    while(1)
    {
        sIsSuccess = STL_TRUE;
        
        sIntervalTime = knlGetPropertyBigIntValueByID( KNL_PROPERTY_AGING_INTERVAL,
                                                       KNL_ENV(sEnv) );

        while( sIntervalTime > 0 )
        {
            if( sIntervalTime < ZTM_AGER_SLEEP_TIME )
            {
                stlSleep( sIntervalTime * 1000 );
                break;
            }
            else
            {
                stlSleep( ZTM_AGER_SLEEP_TIME * 1000 );      /* 10 ms */
            }
            
            if( (*gZtmWarmupEntry)->mState == ZTM_STATE_STOP_AGER )
            {
                STL_THROW( RAMP_EXIT );
            }

            sIntervalTime = sIntervalTime - ZTM_AGER_SLEEP_TIME;
        }
        
        KNL_CHECK_THREAD_CANCELLATION( KNL_ENV( sEnv ) );
        
        if( smlGetAgerState() == SML_AGER_STATE_ACTIVE )
        {
            /**
             *  Undo segment를 aging 한다.
             */
            if( STL_GET_AS_MSEC_TIME(knlGetSystemTime() - sLastShrinkTime) >
                ZTM_SHRINK_UNDO_INTERVAL_TIME )
            {
                STL_TRY( smlShrinkUndoSegments( SML_ENV(sEnv) ) == STL_SUCCESS );
                sLastShrinkTime = knlGetSystemTime();
            }
            
            /**
             *  Agable Scn을 구축한다.
             */
            STL_TRY( smlBuildAgableScn( SML_ENV( sEnv ) ) == STL_SUCCESS );
            
            /**
             *  Aging Garbage Data
             */
            while( sIsSuccess == STL_TRUE )
            {
                STL_TRY( knlExecuteEnvEvent( KNL_ENV( sEnv ),
                                             &sEventId,
                                             &sIsCanceled,
                                             &sIsSuccess )
                         == STL_SUCCESS );
                
                if( (*gZtmWarmupEntry)->mState == ZTM_STATE_STOP_AGER )
                {
                    STL_THROW( RAMP_EXIT );
                }

                if( smlGetAgerState() == SML_AGER_STATE_ACTIVE )
                {
                    STL_TRY( smlBuildAgableScn( SML_ENV( sEnv ) ) == STL_SUCCESS );
                }
                
                KNL_CHECK_THREAD_CANCELLATION( KNL_ENV( sEnv ) );
            }
        }
    }

    STL_RAMP( RAMP_EXIT );

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

    ZTM_SET_THREAD_STATE( &gZtmAgerThread, ZTM_THREAD_STATE_INIT );
    
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
                       "[AGER] abnormally terminated\n" );
        case 2 :
            sslFinalizeEnv( SSL_ENV( sEnv ) );
        case 1 :
            stlAppendErrors( sErrorStack, KNL_ERROR_STACK(sEnv) );

            knlFreeEnv( (void*)sEnv,
                        sErrorStack );
    }

    ZTM_SET_THREAD_STATE( &gZtmAgerThread, ZTM_THREAD_STATE_FAIL );

    stlExitThread( aThread, STL_FAILURE, sErrorStack );
    
    return NULL;
}

stlStatus ztmtCreateAgerThread( ztmEnv * aSystemEnv )
{
    STL_TRY( stlCreateThread( &gZtmAgerThread.mThread,
                              NULL,
                              ztmtAgerThread,
                              (void *)&gZtmAgerThread.mArgs,
                              KNL_ERROR_STACK( aSystemEnv ) ) == STL_SUCCESS );
    
    STL_TRY( smlSetAgerState( SML_AGER_STATE_ACTIVE,
                              SML_ENV( aSystemEnv ) ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztmtStopAgerThread( ztmEnv * aSystemEnv )
{
    stlStatus sReturnStatus;

    (*gZtmWarmupEntry)->mState = ZTM_STATE_STOP_AGER;

    if( ZTM_IS_STARTED( &gZtmAgerThread ) == STL_TRUE )
    {
        STL_TRY( stlJoinThread( &gZtmAgerThread.mThread,
                                &sReturnStatus,
                                KNL_ERROR_STACK( aSystemEnv ) ) == STL_SUCCESS );
    }
    
    STL_TRY( smlSetAgerState( SML_AGER_STATE_INACTIVE,
                              SML_ENV( aSystemEnv ) ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

