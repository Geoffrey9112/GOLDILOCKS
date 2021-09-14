/*******************************************************************************
 * ztmtLogFlush.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztmtLogFlush.c 4391 2012-04-23 09:12:02Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file ztmtLogFlush.c
 * @brief Gliese Master Log Flush Routines
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

ztmThread gZtmLogFlushThread;

extern ztmEnv            gZtmSystemEnv;
extern ztmWarmupEntry ** gZtmWarmupEntry;

void * STL_THREAD_FUNC ztmtLogFlushThread( stlThread * aThread, void * aArg )
{
    stlErrorStack    * sErrorStack = &((ztmThreadArgs *)aArg)->mErrorStack;
    stlInt32           sState     = 0;
    ztmEnv           * sEnv;
    ztmSessionEnv    * sSessionEnv;
    knlEventListener * sEventListener;
    stlInt64           sFlushedLsn = 0;
    stlInt64           sFlushedSbsn = 0;
    stlInt64           sLastSyncLsn = 0;
    stlTime            sSyncInterval;
    stlTime            sLastSyncTime;
    stlBool            sIsTimedOut;
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
    
    sEventListener = &(*gZtmWarmupEntry)->mLogFlusherEvent;
    
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

    /**
     * decide Numa Node
     */
    STL_TRY( knlBindNumaMasterNode( KNL_ENV(sEnv) ) == STL_SUCCESS );
    
    ((sslSessionEnv*)sSessionEnv)->mClientProcId = sProc;
    stlSnprintf( ((sslSessionEnv*)sSessionEnv)->mProgram,
                 SSL_PROGRAM_NAME,
                 "%s: %s",
                 sProgram,
                 ZTM_THREAD_NAME_LOG_FLUSH );

    knlRegisterEventListener( sEventListener, KNL_ENV(sEnv) );
    smlRegisterLogFlusherEnv( SML_ENV(sEnv) );
    
    ZTM_SET_THREAD_STATE( &gZtmLogFlushThread, ZTM_THREAD_STATE_RUN );
    ZTM_SET_ENV( &gZtmLogFlushThread, sEnv );

    sLastSyncTime = knlGetSystemTime();
    
    KNL_ENTER_CRITICAL_SECTION( sEnv );
        
    while(1)
    {
        STL_TRY( knlWaitEvent( sEventListener,
                               STL_TIMESLICE_TIME,
                               &sIsTimedOut,
                               KNL_ENV( sEnv ) ) == STL_SUCCESS );

        if( (*gZtmWarmupEntry)->mState == ZTM_STATE_STOP_LOG_FLUSH )
        {
            break;
        }

        /*
         * 이벤트 확인
         */
        STL_TRY( knlExecuteProcessEvent( KNL_ENV( sEnv ) ) == STL_SUCCESS );

        if( smlGetDataStoreMode() != SML_DATA_STORE_MODE_TDS )
        {
            continue;
        }
        
        if( smlGetDataAccessMode() == SML_DATA_ACCESS_MODE_READ_ONLY )
        {
            continue;
        }

        sSyncInterval = knlGetPropertyBigIntValueByID( KNL_PROPERTY_LOG_SYNC_INTERVAL,
                                                       KNL_ENV(sEnv) );

        sSyncInterval = STL_SET_SEC_TIME( sSyncInterval );
        
        /*
         * do log flush
         */

        if( ((sFlushedLsn + 1) < smlGetSystemLsn()) ||
            ((sFlushedSbsn + 1) < smlGetSystemSbsn()) )
        {
            /**
             * 현재 로그가 기록된 것이 있다면
             * - chained log로 인하여 system lsn와 flushed lsn이 같은 값을 가질수 있다.
             *   SBSN으로 한번더 학인해야 한다.
             */
            
            if( (sSyncInterval <= (knlGetSystemTime() - sLastSyncTime)) &&
                (sFlushedLsn == sLastSyncLsn) )
            {
                STL_TRY( smlFlushLogForFlusher( STL_TRUE,  /* aFlushAll */
                                                &sFlushedLsn,
                                                &sFlushedSbsn,
                                                SML_ENV( sEnv ) )
                         == STL_SUCCESS );
            
                sLastSyncTime = knlGetSystemTime();
            }
            else
            {
                STL_TRY( smlFlushLogForFlusher( STL_FALSE,  /* aFlushAll */
                                                &sFlushedLsn,
                                                &sFlushedSbsn,
                                                SML_ENV( sEnv ) )
                         == STL_SUCCESS );
            }

            if( sFlushedLsn != sLastSyncLsn )
            {
                sLastSyncLsn = sFlushedLsn;
                sLastSyncTime = knlGetSystemTime();
            }
        }
        else
        {
            /**
             * 변경 사항이 없다면
             */

            sLastSyncTime = knlGetSystemTime();
        }
    }

    KNL_LEAVE_CRITICAL_SECTION( sEnv );
        
    STL_TRY( knlUnbindNumaNode( STL_FALSE,  /* aIsCleanup */
                                KNL_ENV(sEnv) )
             == STL_SUCCESS );
    
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

    ZTM_SET_THREAD_STATE( &gZtmLogFlushThread, ZTM_THREAD_STATE_INIT );

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
                       "[LOG FLUSHER] abnormally terminated\n" );
        case 2 :
            sslFinalizeEnv( SSL_ENV( sEnv ) );
        case 1 :
            stlAppendErrors( sErrorStack, KNL_ERROR_STACK(sEnv) );

            knlFreeEnv( (void*)sEnv,
                        sErrorStack );
        default:
            break;
    }

    ZTM_SET_THREAD_STATE( &gZtmLogFlushThread, ZTM_THREAD_STATE_FAIL );

    stlExitThread( aThread, STL_FAILURE, sErrorStack );
    
    return NULL;
}

stlStatus ztmtCreateLogFlushThreads( ztmEnv * aSystemEnv )
{
    STL_TRY( stlCreateThread( &gZtmLogFlushThread.mThread,
                              NULL,
                              ztmtLogFlushThread,
                              (void *)&gZtmLogFlushThread.mArgs,
                              KNL_ERROR_STACK( aSystemEnv ) ) == STL_SUCCESS );
    
    STL_TRY( smlSetLogFlusherState( SML_LOG_FLUSHER_STATE_ACTIVE,
                                    SML_ENV( aSystemEnv ) ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztmtStopLogFlushThreads( ztmEnv * aSystemEnv )
{
    stlStatus sReturnStatus;

    (*gZtmWarmupEntry)->mState = ZTM_STATE_STOP_LOG_FLUSH;

    if( ZTM_IS_STARTED( &gZtmLogFlushThread ) == STL_TRUE )
    {
        STL_TRY( stlJoinThread( &gZtmLogFlushThread.mThread,
                                &sReturnStatus,
                                KNL_ERROR_STACK( aSystemEnv ) ) == STL_SUCCESS );
    }
                            
    STL_TRY( smlSetLogFlusherState( SML_LOG_FLUSHER_STATE_INACTIVE,
                                    SML_ENV( aSystemEnv ) ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}
