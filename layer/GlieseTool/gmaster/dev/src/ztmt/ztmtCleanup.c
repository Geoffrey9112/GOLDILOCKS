/*******************************************************************************
 * ztmtCleanup.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztmtCleanup.c 4391 2012-04-23 09:12:02Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file ztmtCleanup.c
 * @brief Gliese Master Cleanup Thread Routines
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

ztmThread gZtmCleanupThread;

extern stlInt64          gZtmBootdownKey;

extern ztmEnv            gZtmSystemEnv;
extern ztmWarmupEntry ** gZtmWarmupEntry;

void * STL_THREAD_FUNC ztmtCleanupThread( stlThread * aThread, void * aArg )
{
    stlErrorStack * sErrorStack = &((ztmThreadArgs *)aArg)->mErrorStack;
    stlInt32        sState     = 0;
    ztmEnv        * sEnv;
    ztmSessionEnv * sSessionEnv;
    stlProc         sProc;
    stlChar         sProgram[STL_PATH_MAX];
    stlBool         sIsSuccess;
    stlBool         sIsCanceled;
    stlInt32        sEventId;

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

    ((sslSessionEnv*)sSessionEnv)->mClientProcId = sProc;
    stlSnprintf( ((sslSessionEnv*)sSessionEnv)->mProgram,
                 SSL_PROGRAM_NAME,
                 "%s: %s",
                 sProgram,
                 ZTM_THREAD_NAME_CLEANUP );

    smlRegisterCleanupEnv( SML_ENV( sEnv ) );

    ZTM_SET_THREAD_STATE( &gZtmCleanupThread, ZTM_THREAD_STATE_RUN );
    ZTM_SET_ENV( &gZtmCleanupThread, sEnv );

    KNL_ENTER_CRITICAL_SECTION( sEnv );
        
    while(1)
    {
        stlSleep( ZTM_CLEANUP_SLEEP_TIME );      /* 1 초 */
        
        if( (*gZtmWarmupEntry)->mState == ZTM_STATE_STOP_CLEANUP )
        {
            STL_THROW( RAMP_EXIT );
        }

        KNL_CHECK_THREAD_CANCELLATION( KNL_ENV(sEnv) );
        
        STL_TRY( knlExecuteEnvEvent( KNL_ENV( sEnv ),
                                     &sEventId,
                                     &sIsCanceled,
                                     &sIsSuccess )
                 == STL_SUCCESS );

        /**
         * Long statement를 정리한다.
         */
        
        STL_TRY( ztmtCleanupLongStatement( sEnv ) == STL_SUCCESS );
                
        /**
         *  Session Fatal이 발생한 Environment를 정리한다.
         */

        STL_TRY( ztmtCleanupDeadEnv( sEnv ) == STL_SUCCESS );
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

    ZTM_SET_THREAD_STATE( &gZtmCleanupThread, ZTM_THREAD_STATE_INIT );

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
                       "[CLEANUP] abnormally terminated\n" );
        case 2 :
            sslFinalizeEnv( SSL_ENV( sEnv ) );
        case 1 :
            stlAppendErrors( sErrorStack, KNL_ERROR_STACK(sEnv) );

            knlFreeEnv( (void*)sEnv,
                        sErrorStack );
    }

    ZTM_SET_THREAD_STATE( &gZtmCleanupThread, ZTM_THREAD_STATE_FAIL );

    stlExitThread( aThread, STL_FAILURE, sErrorStack );
    
    return NULL;
}

stlStatus ztmtCreateCleanupThread( ztmEnv * aSystemEnv )
{
    ZTM_SET_THREAD_STATE( &gZtmCleanupThread, ZTM_THREAD_STATE_INIT );
    
    STL_TRY( stlCreateThread( &gZtmCleanupThread.mThread,
                              NULL,
                              ztmtCleanupThread,
                              (void *)&gZtmCleanupThread.mArgs,
                              KNL_ERROR_STACK( aSystemEnv ) ) == STL_SUCCESS );
    
    ZTM_WAIT_THREAD( &gZtmCleanupThread );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztmtStopCleanupThread( ztmEnv * aSystemEnv )
{
    stlStatus sReturnStatus;

    (*gZtmWarmupEntry)->mState = ZTM_STATE_STOP_CLEANUP;
    
    STL_TRY( stlJoinThread( &gZtmCleanupThread.mThread,
                            &sReturnStatus,
                            KNL_ERROR_STACK( aSystemEnv ) ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztmtCleanupLongStatement( ztmEnv * aSystemEnv )
{
    knlSessionEnv * sKnlSessionEnv;
    smlSessionEnv * sSmlSessionEnv;
    knlEnv        * sKnlEnv;
    stlInt64        sStatementTimeout;
    stlInt64        sDiffTime;
    stlExpTime      sExpTime;
    stlTime         sStmtBeginTime;
    stlUInt32       sSessionId;

    sStatementTimeout = knlGetPropertyBigIntValueByID( KNL_PROPERTY_SNAPSHOT_STATEMENT_TIMEOUT,
                                                       KNL_ENV(aSystemEnv) );
    
    sKnlSessionEnv = knlGetFirstSessionEnv();

    while( sKnlSessionEnv != NULL )
    {
        sKnlEnv = (knlEnv*)KNL_SESS_LINKED_ENV( sKnlSessionEnv );

        if( sKnlEnv == NULL )
        {
            sKnlSessionEnv = knlGetNextSessionEnv( sKnlSessionEnv );
            continue;
        }
            
        /**
         * 아래 5가지 경우는 Timeout을 검사하지 않는다.
         *
         *  1. 존재하지 않는 Process
         *  2. 사용중이지 않은 Session
         *  3. Aging이 불가능한 세션
         *  4. 종료된 Session
         *  5. 할당은 되어 있지만 아직 Process Env와 연결이 되어 있지 않은 경우
         *  6. Internal session
         *  7. 이미 죽은 세션
         */

        if( (knlIsUsedSessionEnv( sKnlSessionEnv ) == STL_FALSE) ||
            (KNL_IS_ENABLE_SESSION_AGING( sKnlSessionEnv ) == STL_FALSE) )
        {
            sKnlSessionEnv = knlGetNextSessionEnv( sKnlSessionEnv );
            continue;
        }
        
        if( (stlIsActiveProc( &(sKnlEnv->mThread.mProc) ) == STL_FALSE) ||
            (knlIsTerminatedSessionEnv( sKnlSessionEnv ) == STL_TRUE)   ||
            (knlIsDeadSessionEnv( sKnlSessionEnv ) == STL_TRUE)         ||
            (KNL_IS_USER_LAYER( sKnlSessionEnv->mTopLayer ) == STL_FALSE) )
        {
            sKnlSessionEnv = knlGetNextSessionEnv( sKnlSessionEnv );
            continue;
        }

        sSmlSessionEnv = (smlSessionEnv*)sKnlSessionEnv;
        sStmtBeginTime = sSmlSessionEnv->mMinSnapshotStmtBeginTime;
        
        if( sStmtBeginTime != STL_INVALID_STL_TIME )
        {
            sDiffTime = knlGetSystemTime() - sStmtBeginTime;

            if( KNL_CHECK_TIMEOUT( sDiffTime, STL_SET_SEC_TIME(sStatementTimeout) ) == STL_TRUE )
            {
                stlMakeExpTimeByLocalTz(&sExpTime, sStmtBeginTime );

                knlGetSessionEnvId( sKnlSessionEnv, &sSessionId, KNL_ENV(aSystemEnv) );
                
                STL_TRY( knlLogMsg( NULL,
                                    KNL_ENV( aSystemEnv ),
                                    KNL_LOG_LEVEL_WARNING,
                                    "[CLEANUP] long statement timeout - session(%ld), pid(%d), thread(%ld), program(%s), "
                                    "statement start time(%04d-%02d-%02d %02d:%02d:%02d.%06d)\n",
                                    sSessionId,
                                    sKnlEnv->mThread.mProc.mProcID,
                                    sKnlEnv->mThread.mHandle,
                                    ((sslSessionEnv*)sKnlSessionEnv)->mProgram,
                                    sExpTime.mYear + 1900,
                                    sExpTime.mMonth + 1,
                                    sExpTime.mDay,
                                    sExpTime.mHour,
                                    sExpTime.mMinute,
                                    sExpTime.mSecond,
                                    sExpTime.mMicroSecond )
                         == STL_SUCCESS );
                
                knlKillSession( sKnlSessionEnv );
            }
        }
        
        sKnlSessionEnv = knlGetNextSessionEnv( sKnlSessionEnv );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztmtCleanupDeadEnv( ztmEnv * aSystemEnv )
{
    knlSessionEnv * sSessionEnv;
    knlEnv        * sDeadEnv;
    stlUInt32       sDeadEnvId;
    stlUInt32       sDeadSessEnvId;
    smlSessionEnv * sDeadSessEnv;
    smlSessionEnv * sSystemSessEnv;
    stlUInt64       sCompareKey;
    stlUInt64       sWithKey;
    stlUInt64       sOldAccessKey;
    stlUInt64     * sAccessKey;
    stlUInt64       sSessionSeq;
    stlInt32        sTotalDeadSessionCount = 0;
    stlBool         sIsZombieSession;
    knlXaContext  * sXaContext;
    stlChar         sXidString[STL_XID_STRING_SIZE];
    
    sSessionEnv = knlGetFirstSessionEnv();

    while( sSessionEnv != NULL )
    {
        sIsZombieSession = STL_FALSE;

        if( (knlIsUsedSessionEnv( sSessionEnv ) == STL_TRUE) &&
            (sSessionEnv->mSessionType == KNL_SESSION_TYPE_DEDICATE) )
        {
            /**
             * 더이상 사용하지 못하는 세션은 무시한다.
             */
            if( knlIsDeadSessionEnv( sSessionEnv ) == STL_TRUE )
            {
                sSessionEnv = knlGetNextSessionEnv( sSessionEnv );
                continue;
            }
            
            /**
             * Connect되어 있지만 프로세스가 종료된 경우는 강제로 세션을 종료시킨다.
             */
            
            sDeadEnv = (knlEnv*)KNL_SESS_LINKED_ENV( sSessionEnv );

            if( sDeadEnv != NULL )
            {
                if( stlIsActiveProc( &(sDeadEnv->mThread.mProc) ) == STL_TRUE )
                {
                    /**
                     * Process는 살아 있지만 종료된 세션은 cleanup 한다.
                     */
                    if( knlIsTerminatedSessionEnv( sSessionEnv ) == STL_FALSE )
                    {
                        sSessionEnv = knlGetNextSessionEnv( sSessionEnv );
                        continue;
                    }
                }
                else
                {
                    /**
                     * 비정상적으로 Process가 종료된 경우
                     * - Disconnect를 호출하지 않고 종료된 경우
                     * - signal에 의해서 종료된 경우는 이에 해당되지 않는다.
                     */

                    if( knlIsTerminatedSessionEnv( sSessionEnv ) == STL_FALSE )
                    {
                        STL_TRY( knlLogMsg( NULL,
                                            KNL_ENV( aSystemEnv ),
                                            KNL_LOG_LEVEL_WARNING,
                                            "[CLEANUP] snipe at zombie session - pid(%d), thread(%ld), program(%s)\n",
                                            sDeadEnv->mThread.mProc.mProcID,
                                            sDeadEnv->mThread.mHandle,
                                            ((sslSessionEnv*)sSessionEnv)->mProgram )
                                 == STL_SUCCESS );
                    
                        knlKillSession( sSessionEnv );
                    }
                    
                    sIsZombieSession = STL_TRUE;
                }
            }
        }

        /**
         * 아래 3가지 경우는 clean하지 않는다.
         *
         *  1. 사용중이지 않은 Session
         *  2. 죽지 않은 Session
         *  3. 할당은 되어 있지만 Aging이 가능한 상태가 아닌경우
         */
        if( (knlIsUsedSessionEnv( sSessionEnv ) == STL_FALSE)       ||
            (knlIsTerminatedSessionEnv( sSessionEnv ) == STL_FALSE) ||
            (KNL_IS_ENABLE_SESSION_AGING( sSessionEnv ) == STL_FALSE) )
        {
            sSessionEnv = knlGetNextSessionEnv( sSessionEnv );
            continue;
        }

        /**
         * Internal master session은 clean될수 없다.
         */
        STL_ASSERT( sSessionEnv->mTopLayer != STL_LAYER_SESSION );

        /**
         * Session 사용의 종료를 검사한다.
         */

        if( sIsZombieSession == STL_FALSE )
        {
            sSessionSeq = KNL_GET_SESSION_SEQ( sSessionEnv );
            sCompareKey = KNL_MAKE_SESSION_ACCESS_KEY( KNL_SESSION_ACCESS_LEAVE, sSessionSeq );
            sWithKey    = KNL_MAKE_SESSION_ACCESS_KEY( KNL_SESSION_ACCESS_ENTER, sSessionSeq );
            sAccessKey  = KNL_GET_SESSION_ACCESS_KEY_POINTER( sSessionEnv );
    
            sOldAccessKey = stlAtomicCas64( sAccessKey,
                                            sWithKey,
                                            sCompareKey );

            if( sOldAccessKey != sCompareKey )
            {
                sSessionEnv = knlGetNextSessionEnv( sSessionEnv );
                continue;
            }
        }
        else
        {
            sSessionSeq = KNL_GET_SESSION_SEQ( sSessionEnv );
        }

        KNL_SET_SESSION_ACCESS_KEY( sSessionEnv,
                                    KNL_SESSION_ACCESS_LEAVE,
                                    sSessionSeq );

        /**
         * Session 을 정리한다.
         */
        
        sDeadSessEnv = (smlSessionEnv*)sSessionEnv;
        sDeadEnv = (knlEnv*)KNL_SESS_LINKED_ENV( sSessionEnv );

        if( KNL_SESSION_TYPE(sDeadSessEnv) == KNL_SESSION_TYPE_SHARED )
        {
            /**
             * Shared Session은 강제적으로 Env를 Null로 설정해서
             * 사용하지 못하도록 한다.
             */
            sDeadEnv = NULL;
        }

        if( sDeadEnv != NULL )
        {
            STL_TRY( knlGetEnvId( sDeadEnv, &sDeadEnvId ) == STL_SUCCESS );
        }
        else
        {
            sDeadEnvId = -1;
        }

        STL_TRY( knlGetSessionEnvId( sDeadSessEnv,
                                     &sDeadSessEnvId,
                                     KNL_ENV(aSystemEnv) )
                 == STL_SUCCESS );
            
        if( sDeadEnv != NULL )
        {
            /**
             * Dead Session을 Session Fatal로 처리 가능한지 확인한다.
             */
            if( knlResolveSystemFatal( STL_TRUE, sDeadEnv ) == STL_FALSE )
            {
                STL_TRY( knlLogMsg( NULL,
                                    KNL_ENV( aSystemEnv ),
                                    KNL_LOG_LEVEL_WARNING,
                                    "[CLEANUP] failed to cleaning session - server restart required\n"
                                    "...... dead session in critical section - "
                                    "env(%d), session(%d), transaction(%lX), program(%s), pid(%d), thread(%ld)\n",
                                    sDeadEnvId,
                                    sDeadSessEnvId,
                                    ((smlSessionEnv*)sDeadSessEnv)->mTransId,
                                    ((sslSessionEnv*)sDeadSessEnv)->mProgram,
                                    sDeadEnv->mThread.mProc.mProcID,
                                    sDeadEnv->mThread.mHandle )
                         == STL_SUCCESS );

                knlDeadSession( sSessionEnv );
                KNL_SET_SESSION_STATUS( sSessionEnv, KNL_SESSION_STATUS_DEAD );

                sSessionEnv = knlGetNextSessionEnv( sSessionEnv );
                continue;
            }
        }

        switch( knlGetTerminationStatus( sSessionEnv ) )
        {
            case KNL_TERMINATION_NORMAL :
                if( sDeadEnv != NULL )
                {
                    KNL_UNLINK_SESS_ENV( sDeadEnv, sDeadSessEnv );
                }
                else
                {
                    ((knlSessionEnv*)(sDeadSessEnv))->mEnv = NULL;
                }
                stlMemBarrier();

                STL_TRY( knlFreeSessionEnv( sDeadSessEnv,
                                            KNL_ENV( aSystemEnv ) )
                         == STL_SUCCESS );
                
                if( sDeadEnv != NULL )
                {
                    STL_TRY( knlFreeEnv( sDeadEnv,
                                         KNL_ERROR_STACK( aSystemEnv ) )
                             == STL_SUCCESS );
                }
                break;
                
            case KNL_TERMINATION_ABNORMAL :
                if( (sSessionEnv->mSessionType == KNL_SESSION_TYPE_DEDICATE) &&
                    (KNL_IS_USER_LAYER( knlGetSessionTopLayer((knlSessionEnv*)sSessionEnv) ) == STL_TRUE) )
                {
                    STL_TRY( knlUnbindNumaNode( STL_TRUE,  /* aIsCleanup */
                                                KNL_ENV(sDeadEnv) )
                             == STL_SUCCESS );
                }
                
                sTotalDeadSessionCount++;

                if( sDeadEnv != NULL )
                {
                    STL_TRY( knlLogMsg( NULL,
                                        KNL_ENV( aSystemEnv ),
                                        KNL_LOG_LEVEL_WARNING,
                                        "[CLEANUP] cleaning session - "
                                        "env(%d), session(%d), transaction(%lX), program(%s), pid(%d), thread(%ld)",
                                        sDeadEnvId,
                                        sDeadSessEnvId,
                                        ((smlSessionEnv*)sDeadSessEnv)->mTransId,
                                        ((sslSessionEnv*)sDeadSessEnv)->mProgram,
                                        sDeadEnv->mThread.mProc.mProcID,
                                        sDeadEnv->mThread.mHandle )
                             == STL_SUCCESS );
                }
                else
                {
                    STL_TRY( knlLogMsg( NULL,
                                        KNL_ENV( aSystemEnv ),
                                        KNL_LOG_LEVEL_WARNING,
                                        "[CLEANUP] cleaning session - "
                                        "env(%d), session(%d), transaction(%lX), program(%s)",
                                        sDeadEnvId,
                                        sDeadSessEnvId,
                                        ((smlSessionEnv*)sDeadSessEnv)->mTransId,
                                        ((sslSessionEnv*)sDeadSessEnv)->mProgram )
                             == STL_SUCCESS );
                }

                /**
                 * Global Transaction 처리
                 * - XA 상태를 ROLLBACK ONLY로 처리한다.
                 * - 트랜잭션을 롤백하지 않고 다른 세션에서 롤백할수 있도록 처리한다.
                 */
                
                if( knlGetTransScope( sDeadSessEnv ) == KNL_TRANS_SCOPE_GLOBAL )
                {
                    sXaContext = ((sslSessionEnv*)sDeadSessEnv)->mXaContext;
                    
                    if( sXaContext != NULL )
                    {
                        STL_TRY( dtlXidToString( &sXaContext->mXid,
                                                 STL_XID_STRING_SIZE,
                                                 sXidString,
                                                 KNL_ERROR_STACK( aSystemEnv ) )
                                 == STL_SUCCESS );
                        
                        STL_TRY( sslCleanupContext( sXaContext,
                                                    (sslSessionEnv*)sDeadSessEnv,
                                                    SSL_ENV( aSystemEnv ) )
                                 == STL_SUCCESS );
        
                        if( sDeadEnv != NULL )
                        {
                            STL_TRY( knlLogMsg( NULL,
                                                KNL_ENV( aSystemEnv ),
                                                KNL_LOG_LEVEL_WARNING,
                                                "[CLEANUP] dissociate xa context - "
                                                "env(%d), session(%d), transaction(%s), pid(%d), thread(%ld)",
                                                sDeadEnvId,
                                                sDeadSessEnvId,
                                                sXidString,
                                                sDeadEnv->mThread.mProc.mProcID,
                                                sDeadEnv->mThread.mHandle )
                                     == STL_SUCCESS );
                        }
                        else
                        {
                            STL_TRY( knlLogMsg( NULL,
                                                KNL_ENV( aSystemEnv ),
                                                KNL_LOG_LEVEL_WARNING,
                                                "[CLEANUP] cleaning xa context - "
                                                "env(%d), session(%d), transaction(%s)",
                                                sDeadEnvId,
                                                sDeadSessEnvId,
                                                sXidString )
                                     == STL_SUCCESS );
                        }
                    }
                }
                
                sSystemSessEnv = (smlSessionEnv*)KNL_SESS_ENV(aSystemEnv);
                
                /**
                 * @todo 향후 전 Layer에 Clean Up 함수를 추가해야함.
                 * @todo Cleanup을 위한 heap memory는 할당된 기본 메모리를 초과할수 없다.
                 * <BR> Cleanup 도중 추가적인 heap memory의 요구는 Cleanup의 실패를 유발시킬수 있다.
                 * <BR> 향후 이러한 제약조건을 제거해야 한다.
                 */

                STL_TRY( sslCleanupSessionEnv( (sslSessionEnv*)sDeadSessEnv,
                                               SSL_ENV(aSystemEnv) )
                         == STL_SUCCESS );
                
                STL_TRY( qllCleanupSessionEnv( (qllSessionEnv*)sDeadSessEnv,
                                               QLL_ENV(aSystemEnv) )
                         == STL_SUCCESS );

                /**
                 * Storage Manager CLEANUP을 위한 Session 운용 방안
                 * 1. SHUTDOWN ABORT 시 thread cancellation functionality를 만족시키기
                 *    위해서는 System Session을 사용해야 한다. ( rollback은 오래걸리는
                 *    작업일수 있다)
                 * 2. Dead session 내에 cleanup할 대상들을 system env 쪽으로 옮겨야 한다.
                 *    Session에 연결되어 있는 Pending operation을 system으로 옮겨야 한다.
                 */

                STL_TRY( ellCleanupHandoverSession( (ellSessionEnv *) sSystemSessEnv,
                                                    (ellSessionEnv *) sDeadSessEnv )
                         == STL_SUCCESS );
                
                STL_TRY( smlHandoverSession( (smlSessionEnv*)sSystemSessEnv,
                                             (smlSessionEnv*)sDeadSessEnv )
                         == STL_SUCCESS );

                STL_TRY( ellCleanupPriorRollbackTransDDL( ((sslSessionEnv*)sDeadSessEnv)->mTransId,
                                                          ELL_ENV(aSystemEnv) )
                         == STL_SUCCESS );
                
                STL_TRY( smlRollbackTransForCleanup( ((sslSessionEnv*)sDeadSessEnv)->mTransId,
                                                     SML_ENV(aSystemEnv) )
                         == STL_SUCCESS );

                STL_TRY( ellCleanupAfterRollbackTransDDL( ((sslSessionEnv*)sDeadSessEnv)->mTransId,
                                                          ELL_ENV(aSystemEnv) )
                         == STL_SUCCESS );
                         
                STL_TRY( smlEndTransForCleanup( ((sslSessionEnv*)sDeadSessEnv)->mTransId,
                                                SML_ENV(aSystemEnv) )
                         == STL_SUCCESS );

                STL_TRY( ellCleanupSessionEnv( (ellSessionEnv*)sDeadSessEnv,
                                               ELL_ENV(aSystemEnv) )
                         == STL_SUCCESS );
                
                STL_TRY( smlCleanupSessionEnv( sDeadSessEnv,
                                               SML_ENV(aSystemEnv) )
                         == STL_SUCCESS );

                STL_TRY( knlCleanupSessionEnv( (knlSessionEnv*)sDeadSessEnv,
                                               KNL_ENV(aSystemEnv) )
                         == STL_SUCCESS );

                if( sDeadEnv != NULL )
                {
                    KNL_UNLINK_SESS_ENV( sDeadEnv, sDeadSessEnv );
                }
                else
                {
                    ((knlSessionEnv*)(sDeadSessEnv))->mEnv = NULL;
                }
                stlMemBarrier();

                STL_TRY( knlFreeSessionEnv( (void*)sDeadSessEnv,
                                            KNL_ENV( aSystemEnv ) )
                         == STL_SUCCESS );
                
                if( sDeadEnv != NULL )
                {
                    STL_TRY( knlFreeEnv( sDeadEnv,
                                         KNL_ERROR_STACK( aSystemEnv ) )
                             == STL_SUCCESS );
                }
                
                break;
            case KNL_TERMINATION_NONE:
            default :
                STL_ASSERT( STL_FALSE );
                break;
        }
        
        sSessionEnv = knlGetNextSessionEnv( sSessionEnv );
    }

    if( sTotalDeadSessionCount > 0 )
    {
        STL_TRY( knlLogMsg( NULL,
                            KNL_ENV( aSystemEnv ),
                            KNL_LOG_LEVEL_WARNING,
                            "[CLEANUP] cleaning up %d sessions\n",
                            sTotalDeadSessionCount )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

