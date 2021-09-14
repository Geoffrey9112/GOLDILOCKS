/*******************************************************************************
 * ztmtIoSlave.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztmtIoSlave.c 4391 2012-04-23 09:12:02Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file ztmtIoSlave.c
 * @brief Gliese Master I/O Slave Routines
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

ztmThread gZtmIoSlaveThread[SML_MAX_PARALLEL_IO_GROUP];

extern ztmWarmupEntry ** gZtmWarmupEntry;

void * STL_THREAD_FUNC ztmtIoSlaveThread( stlThread * aThread, void * aArg )
{
    ztmThreadArgs    * sThreadArgs;
    stlErrorStack    * sErrorStack;
    stlInt32           sState     = 0;
    ztmEnv           * sEnv;
    ztmSessionEnv    * sSessionEnv;
    stlInt64           sThreadIdx;
    knlEventListener * sEventListener;
    stlBool            sIsTimedOut;
    stlBool            sIsSuccess;
    stlBool            sIsCanceled;
    stlInt32           sEventId;
    stlProc            sProc;
    stlChar            sProgram[STL_PATH_MAX];

    sThreadArgs = (ztmThreadArgs *)aArg;

    sThreadIdx = sThreadArgs->mThreadIndex;
    sErrorStack = &sThreadArgs->mErrorStack;
    
    STL_INIT_ERROR_STACK( sErrorStack );

    STL_TRY( stlBlockRealtimeThreadSignals( sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlGetCurrentProc( &sProc,
                                sErrorStack ) == STL_SUCCESS );

    STL_TRY( stlGetProcName( &sProc,
                             sProgram,
                             STL_PATH_MAX,
                             sErrorStack ) == STL_SUCCESS );
    
    sEventListener = &(*gZtmWarmupEntry)->mIoSlaveEvent[sThreadIdx];
    
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
                 "%s: %s (%ld)",
                 sProgram,
                 ZTM_THREAD_NAME_IO_SLAVE,
                 sThreadIdx );

    knlRegisterEventListener( sEventListener, KNL_ENV(sEnv) );
    smlRegisterIoSlaveEnv( sThreadIdx, SML_ENV(sEnv) );
    
    ZTM_SET_THREAD_STATE( &gZtmIoSlaveThread[sThreadIdx], ZTM_THREAD_STATE_RUN );
    ZTM_SET_ENV( &gZtmIoSlaveThread[sThreadIdx], sEnv );

    KNL_ENTER_CRITICAL_SECTION( sEnv );
        
    while(1)
    {
        STL_TRY( knlWaitEvent( sEventListener,
                               STL_INFINITE_TIME,
                               &sIsTimedOut,
                               KNL_ENV( sEnv ) ) == STL_SUCCESS );

        if( (*gZtmWarmupEntry)->mState == ZTM_STATE_STOP_IO_SLAVES )
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
        
        if( (*gZtmWarmupEntry)->mState == ZTM_STATE_STOP_IO_SLAVES )
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

    ZTM_SET_THREAD_STATE( &gZtmIoSlaveThread[sThreadIdx], ZTM_THREAD_STATE_INIT );
    
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
                       "[IO SLAVE] abnormally terminated\n" );
        case 2 :
            sslFinalizeEnv( SSL_ENV( sEnv ) );
        case 1 :
            knlFreeEnv( (void*)sEnv,
                        sErrorStack );
        default:
            break;
    }

    ZTM_SET_THREAD_STATE( &gZtmIoSlaveThread[sThreadIdx], ZTM_THREAD_STATE_FAIL );
    
    stlExitThread( aThread, STL_FAILURE, sErrorStack );
    
    return NULL;
}

stlStatus ztmtCreateIoSlaveThread( stlInt64   aThreadIdx,
                                   ztmEnv   * aSystemEnv )
{
    gZtmIoSlaveThread[aThreadIdx].mArgs.mThreadIndex = aThreadIdx;

    STL_TRY( stlCreateThread( &gZtmIoSlaveThread[aThreadIdx].mThread,
                              NULL,
                              ztmtIoSlaveThread,
                              (void *)&gZtmIoSlaveThread[aThreadIdx].mArgs,
                              KNL_ERROR_STACK( aSystemEnv ) ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztmtStopIoSlaveThreads( stlInt32    aIoSlaveCount,
                                  ztmEnv    * aSystemEnv )
{
    stlInt32  i;
    stlStatus sReturnStatus;

    (*gZtmWarmupEntry)->mState = ZTM_STATE_STOP_IO_SLAVES;

    for( i = (aIoSlaveCount - 1); i >= 0; i-- )
    {
        STL_TRY( knlWakeupEventListener( &((*gZtmWarmupEntry)->mIoSlaveEvent[i]),
                                         KNL_ENV( aSystemEnv ) ) == STL_SUCCESS );
    }

    for( i = (aIoSlaveCount - 1); i >= 0; i-- )
    {
        if( ZTM_IS_STARTED( &gZtmIoSlaveThread[i] ) == STL_TRUE )
        {
            STL_TRY( stlJoinThread( &gZtmIoSlaveThread[i].mThread,
                                    &sReturnStatus,
                                    KNL_ERROR_STACK( aSystemEnv ) ) == STL_SUCCESS );
        }
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztmtCancelIoSlaveThread( stlInt64   aThreadIdx,
                                   ztmEnv   * aSystemEnv )
{
    knlSessionEnv * sSessEnv;

    sSessEnv = KNL_SESS_ENV( gZtmIoSlaveThread[aThreadIdx].mEnv );
    
    (void)knlEndStatement( sSessEnv,
                           KNL_END_STATEMENT_OPTION_CANCEL,
                           KNL_ENV(aSystemEnv) );
    
    return STL_SUCCESS;
}
