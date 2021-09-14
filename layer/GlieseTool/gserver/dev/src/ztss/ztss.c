/*******************************************************************************
 * ztss.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztss.c 11483 2014-02-28 03:18:44Z lym $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file ztss.c
 * @brief Gliese gserver shared Routines
 */

#include <stl.h>
#include <cml.h>
#include <knl.h>
#include <scl.h>
#include <sml.h>
#include <ell.h>
#include <qll.h>
#include <ssl.h>
#include <sll.h>
#include <ztsDef.h>
#include <ztsm.h>
#include <sll.h>

sllHandle       * gZtsCurrentSllHandle = NULL;
sllSessionEnv   * gZtsSessionEnv = NULL;
stlBool           gZtsJoinGroupWorker = STL_FALSE;
sllSharedMemory * gZtsSharedMemory = NULL;
stlInt32          gZtsSharedServerId = 0;

stlStatus ztssProcessHandshake( sllSharedMemory   * aSharedMemory,
                                sllHandle         * aSllHandle,
                                sllEnv            * aEnv )
{
    sclHandle             * sSclHandle = SCL_HANDLE(aSllHandle);

    if( sclHandshakingForServer( sSclHandle,
                                 aSllHandle->mRoleName,
                                 SCL_ENV(aEnv) ) == STL_FAILURE )
    {
        /* handshake실패해도 무시 */
        (void)stlPopError( KNL_ERROR_STACK(aEnv) );

        STL_TRY( sclEnqueueResponse( aSllHandle->mIpc.mResponseEvent,
                                     sSclHandle,
                                     SCL_QCMD_SERVER_FAILURE,
                                     0,                   /* CmUnitCount */
                                     SCL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztssProcessRequest( sllSharedMemory    * aSharedMemory,
                              stlInt32             aServerId,
                              sllHandle          * aSllHandle,
                              sllSessionEnv      * aSessionEnv,
                              sllEnv             * aEnv )
{
    stlStatus           sRtn;
    cmlCommandType      sCommandType = CML_COMMAND_NONE;
    sclHandle         * sSclHandle = SCL_HANDLE(aSllHandle);
    sllCommandArgs      sCommandArgs;
    sllSessionEnv     * sUserSessionEnv = NULL;
    stlInt32            sState = 0;
    stlInt32            sEnterState = 0;

    STL_TRY( sclParseCommand( sSclHandle->mProtocolSentence,
                              &sCommandType,
                              KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    (void) sllRemoveEdgeFromWaitGraph( aSllHandle,
                                       SLL_REQUEST_GROUP_ID( aSharedMemory,
                                                             aServerId ),
                                       KNL_ERROR_STACK(aEnv) );

    /**
     * CML_CONN_STATUS_CONNECTED는 CML_COMMAND_CONNECT command이후 상태임.
     */
    if( aSllHandle->mSessionId != SSL_INVALID_SESSION_ID )
    {
        STL_TRY_THROW( sslEnterSession( aSllHandle->mSessionId,
                                        aSllHandle->mSessionSeq,
                                        KNL_ERROR_STACK(aEnv) )
                       == STL_SUCCESS, RAMP_ERR_FAILURE );
        sEnterState = 1;

        sUserSessionEnv = knlGetSessionEnv( aSllHandle->mSessionId );
        KNL_LINK_SESS_ENV( aEnv, sUserSessionEnv );
        sState = 3;
    }
    else
    {
        STL_DASSERT( (sCommandType == CML_COMMAND_CONNECT) ||
                     (sCommandType == CML_COMMAND_STARTUP) );

        /**
         * shared session을 할당할수 있는 상태인지 검사한다.
         */
        STL_TRY_THROW( knlValidateSar( KNL_SAR_SESSION,
                                       STL_LAYER_GLIESE_LIBRARY,
                                       KNL_ENV(aEnv) )
                       == STL_SUCCESS, RAMP_ERR_ALLOC_OPERATION );
        
        STL_TRY_THROW( sslAllocSession( aSllHandle->mRoleName,
                                        (sslSessionEnv**)&sUserSessionEnv,
                                        SSL_ENV(aEnv) )
                       == STL_SUCCESS, RAMP_ERR_ALLOC_OPERATION );
        sState = 1;

        KNL_LINK_SESS_ENV( aEnv, sUserSessionEnv );

        STL_TRY_THROW( sllInitializeSessionEnv( sUserSessionEnv,
                                                KNL_CONNECTION_TYPE_TCP,
                                                KNL_SESSION_TYPE_SHARED,
                                                KNL_SESS_ENV_SHARED,
                                                aEnv )
                       == STL_SUCCESS, RAMP_ERR_ALLOC_OPERATION );
        sState = 2;

        knlSetSessionTopLayer( (knlSessionEnv*)sUserSessionEnv, STL_LAYER_GLIESE_LIBRARY );
    
        STL_TRY_THROW( knlGetSessionEnvId( sUserSessionEnv,
                                           &aSllHandle->mSessionId,
                                           KNL_ENV(aEnv) )
                       == STL_SUCCESS, RAMP_ERR_ALLOC_OPERATION );

        aSllHandle->mSessionSeq = KNL_GET_SESSION_SEQ( sUserSessionEnv );

        KNL_ENABLE_SESSION_AGING( sUserSessionEnv );
        sState = 3;
        
        STL_TRY_THROW( sslEnterSession( aSllHandle->mSessionId,
                                        aSllHandle->mSessionSeq,
                                        KNL_ERROR_STACK(aEnv) )
                       == STL_SUCCESS, RAMP_ERR_FAILURE );
        sEnterState = 1;
    }

    while( STL_TRUE )
    {
        KNL_CHECK_THREAD_CANCELLATION( KNL_ENV(aEnv) );

        STL_INIT_ERROR_STACK( KNL_ERROR_STACK(aEnv) );

        /* packet에서 command를 가져온다 */
        if( sclGetCommand( sSclHandle,
                           &sCommandType,
                           SCL_ENV(aEnv) ) == STL_FAILURE )
        {
            (void) knlLogMsg( NULL,
                              KNL_ENV(aEnv),
                              KNL_LOG_LEVEL_WARNING,
                              "[SHARED_SERVER-%d] received invalid protocol\n",
                              aServerId );

            (void)sllKillSession( aSllHandle, aEnv );

            if( sEnterState == 1 )
            {
                sEnterState = 0;
                sslLeaveSession( aSllHandle->mSessionId, aSllHandle->mSessionSeq );
            }
            
            sSclHandle->mProtocolSentence = NULL;

            STL_TRY( sclEnqueueResponse( aSllHandle->mIpc.mResponseEvent,
                                         sSclHandle,
                                         SCL_QCMD_SERVER_FAILURE,
                                         0,                   /* CmUnitCount */
                                         SCL_ENV(aEnv) )
                     == STL_SUCCESS );
            break;
        }

        if( sCommandType == CML_COMMAND_PERIOD )
        {
            if( sEnterState == 1 )
            {
                sEnterState = 0;
                sslLeaveSession( aSllHandle->mSessionId, aSllHandle->mSessionSeq );
            }

            KNL_LINK_SESS_ENV( aEnv, aSessionEnv );
        }
        
        SLL_MARSHALLING_COMMAND_ARGS( &sCommandArgs, aSllHandle, NULL );
        STL_DASSERT( aSllHandle->mSclHandle.mProtocolSentence->mMemoryMgr != NULL );

        sRtn = gSllCommandFunc[sCommandType]( &sCommandArgs,
                                              aEnv );

        if( sRtn == STL_FAILURE )
        {
            if( stlGetLastErrorCode( KNL_ERROR_STACK(aEnv) ) != SCL_ERRCODE_COMMUNICATION_LINK_FAILURE )
            {
                (void) knlLogMsg( NULL,
                                  KNL_ENV(aEnv),
                                  KNL_LOG_LEVEL_WARNING,
                                  "[SHARED_SERVER-%d] failed to process command - command_type(%d), session_id(%d) \n",
                                  aServerId,
                                  sCommandType,
                                  aSllHandle->mSessionId );
            }

            (void)sllKillSession( aSllHandle, aEnv );

            if( sEnterState == 1 )
            {
                sEnterState = 0;
                sslLeaveSession( aSllHandle->mSessionId, aSllHandle->mSessionSeq );
            }
            
            sSclHandle->mProtocolSentence = NULL;

            STL_TRY( sclEnqueueResponse( aSllHandle->mIpc.mResponseEvent,
                                         sSclHandle,
                                         SCL_QCMD_SERVER_FAILURE,
                                         0,                   /* CmUnitCount */
                                         SCL_ENV(aEnv) )
                     == STL_SUCCESS );

            aSllHandle = NULL;

            /* 나머지 command는 버리기 위해 break */
            break;
        }
        else
        {
            if( sCommandType == CML_COMMAND_DISCONNECT )
            {
                (void) sllFinalizeSessionEnv( sUserSessionEnv, aEnv );
                (void) knlDisconnectSession( sUserSessionEnv );
            }
            else
            {
                if( (sCommandType == CML_COMMAND_CONNECT) &&
                    (sSclHandle->mSocketPeriod == STL_FALSE) )
                {
                    sUserSessionEnv = knlGetSessionEnv( aSllHandle->mSessionId );
                    aSllHandle->mConnectStatus = CML_CONN_STATUS_CONNECTED;
                }
            }
        }

        if( sCommandType == CML_COMMAND_PERIOD )
        {
            /**
             * PERIOD 이후에는 SllHandle은 사용하면 안된다.
             * - Client의 응답으로 인해 다른 Shared Server에서 SllHandle을
             *   사용하고 있을수도 있다.
             */
            aSllHandle = NULL;
            break;
        }

    }

    STL_DASSERT( sEnterState == 0 );

    KNL_LINK_SESS_ENV( aEnv, aSessionEnv );
    
    STL_INIT_ERROR_STACK( KNL_ERROR_STACK(aEnv) );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_ALLOC_OPERATION )
    {
        switch( sState )
        {
            case 2:
                (void) sllFinalizeSessionEnv( sUserSessionEnv, aEnv );
            case 1:
                (void) sslFreeSession( (sslSessionEnv*)sUserSessionEnv, SSL_ENV(aEnv) );
            default:
                break;
        }
        
        KNL_LINK_SESS_ENV( aEnv, aSessionEnv );

        /**
         * Environment Allocation시 실패가 발생하면 trailing protocol을 무시하고,
         * 에러를 클라이언트로 전송한다.
         */
        STL_TRY_THROW( sclWriteErrorResult( sSclHandle,
                                            sCommandType,
                                            KNL_ERROR_STACK(aEnv),
                                            SCL_ENV(aEnv) )
                       == STL_SUCCESS, RAMP_ERR_FAILURE );
        
        /**
         * client로 부터 받은 data를 소비하지 않고 client로 data를 전송함으로
         * 이후에는 socket을 끊어야 함.
         */
        sSclHandle->mSocketPeriod = STL_TRUE;

        STL_TRY_THROW( sclPeriodEnd( sSclHandle,
                                     SCL_ENV(aEnv) )
                       == STL_SUCCESS, RAMP_ERR_FAILURE );
        
        (void) knlLogMsg( NULL,
                          KNL_ENV(aEnv),
                          KNL_LOG_LEVEL_WARNING,
                          "[SHARED_SERVER-%d] fail to allocate session env - last command_type(%d), last session_id(%d) \n",
                          aServerId,
                          sCommandType,
                          aSllHandle->mSessionId );
        
        STL_INIT_ERROR_STACK( KNL_ERROR_STACK(aEnv) );
        
        return STL_SUCCESS;
    }

    STL_CATCH( RAMP_ERR_FAILURE )
    {
        (void)sllKillSession( aSllHandle, aEnv );
        
        if( sEnterState == 1 )
        {
            sEnterState = 0;
            sslLeaveSession( aSllHandle->mSessionId, aSllHandle->mSessionSeq );
        }
        
        KNL_LINK_SESS_ENV( aEnv, aSessionEnv );
        
        sSclHandle->mProtocolSentence = NULL;

        STL_TRY( sclEnqueueResponse( aSllHandle->mIpc.mResponseEvent,
                                     sSclHandle,
                                     SCL_QCMD_SERVER_FAILURE,
                                     0,                   /* CmUnitCount */
                                     SCL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        STL_INIT_ERROR_STACK( KNL_ERROR_STACK(aEnv) );
        
        return STL_SUCCESS;
    }

    STL_FINISH;

    (void) knlLogMsg( NULL,
                      KNL_ENV(aEnv),
                      KNL_LOG_LEVEL_WARNING,
                      "[SHARED_SERVER-%d] abnormally terminated - last command_type(%d), last session_id(%d) \n",
                      aServerId,
                      sCommandType,
                      aSllHandle->mSessionId );
    
    if( sState == 3 )
    {
        knlKillSession( sUserSessionEnv );
    }
    
    if( sEnterState == 1 )
    {
        sslLeaveSession( aSllHandle->mSessionId, aSllHandle->mSessionSeq );
    }

    KNL_LINK_SESS_ENV( aEnv, aSessionEnv );
    STL_INIT_ERROR_STACK( KNL_ERROR_STACK(aEnv) );
    
    return STL_FAILURE;
}


stlStatus ztssProcessShared( stlInt64  * aMsgKey,
                             stlInt32    aIndex,
                             sllEnv    * aEnv )
{
    stlInt32                sState = 0;
    sclMemoryManager        sMemoryMgr;
    sllSharedServer       * sSharedServer = NULL;
    sllSharedMemory       * sSharedMemory;
    sllHandle             * sSllHandle = NULL;
    sllProtocolSentence     sSllProtocolSentence;
    sclHandle             * sSclHandle = NULL;
    sllSessionEnv         * sSessionEnv = NULL;
    stlInt32                sEnterState = 0;
    stlUInt32               sSessionId;
    stlInt64                sSessionSeq = -1;
    stlTime                 sIdleStart;
    stlTime                 sIdleEnd;

    stlMemset( &sSllProtocolSentence, 0x00, STL_SIZEOF(sllProtocolSentence) );

    STL_TRY( sllGetSharedMemory( &sSharedMemory,
                                 KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );
    gZtsSharedMemory = sSharedMemory;
    gZtsSharedServerId = aIndex;

    sSharedServer = &sSharedMemory->mSharedServer[aIndex];

    sSessionEnv = (sllSessionEnv*)SSL_SESS_ENV(aEnv);
    gZtsSessionEnv = sSessionEnv;

    STL_TRY( knlGetSessionEnvId( sSessionEnv,
                                 &sSessionId,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    sSessionSeq = KNL_GET_SESSION_SEQ( sSessionEnv );

    /**
     * Session의 진입을 설정한다.
     */
    STL_TRY( sslEnterSession( sSessionId,
                              sSessionSeq,
                              KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );
    sEnterState = 1;

    STL_TRY( stlThreadSelf( (stlThreadHandle *)&sSharedServer->mThread,
                            KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    (void) sslSetSessionProcessInfo( &sSharedServer->mThread.mProc,
                                     "shared server",
                                     (sslSessionEnv*)sSessionEnv );

    /**
     * aHandle->mMemoryMgr, mOperationBuffer는 재사용 함으로
     * 여기서 할당한다.
     */
    STL_TRY( sclInitializeMemoryManager( &sMemoryMgr,
                                         sSharedMemory->mUnitSize,          /* PacketBufferSize */
                                         CML_BUFFER_SIZE,                   /* OperationBufferSize */
                                         0,                                 /* PollingBufferSize */
                                         SCL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;

    /* gmaster에서 control하기 위해서 env, thread정보를 shared-memory에 설정 */
    sSharedServer->mEnv = aEnv;

    /**
     * Deadlock Detection을 위한 worker 정보를 설정한다.
     */
    smlIncRequestGroupWorker( SLL_REQUEST_GROUP_ID( sSharedMemory, aIndex ) );
    gZtsJoinGroupWorker = STL_TRUE;
    sState = 2;

    /**
     * Kernel에 request group의 identifier를 기록한다.
     * - Trasaction이 대기상태가 되면 Request Group에 대기 정보를 설정하기 위함.
     */
    knlSetRequestGroupId( SLL_REQUEST_GROUP_ID( sSharedMemory, aIndex ),
                          KNL_ENV(aEnv) );
    
    STL_TRY( stlWakeupExecProc( *aMsgKey,
                                KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    /**
     * stlSendErrorStackMsgQueue를 한번 호출하고 나면 aMsgKey를 0으로 설정한다.
     */
    *aMsgKey = 0;
    sIdleEnd = knlGetSystemTime();

    while( sSharedMemory->mExitFlag == STL_FALSE )
    {
        STL_INIT_ERROR_STACK( KNL_ERROR_STACK(aEnv) );

        KNL_CHECK_THREAD_CANCELLATION( KNL_ENV(aEnv) );

        sSharedServer->mStatus = SLL_STATUS_WAIT;

        /**
         * sIdleEnd 부터 sIdleStart까지는 BUSY time추가
         */
        sIdleStart = knlGetSystemTime();
        sSharedServer->mBusy += sIdleStart - sIdleEnd;

        if( sclDequeueRequest( SLL_REQUEST_QUEUE(sSharedMemory, aIndex),
                               (sclHandle **)&sSllHandle,
                               STL_SET_SEC_TIME(1),
                               SCL_ENV(aEnv) ) == STL_FAILURE )
        {
            STL_TRY( stlGetLastErrorCode( KNL_ERROR_STACK(aEnv) ) == STL_ERRCODE_AGAIN );
            (void)stlPopError( KNL_ERROR_STACK(aEnv) );

            sEnterState = 0;
            sslLeaveSession( sSessionId, sSessionSeq );

            STL_TRY( sslEnterSession( sSessionId,
                                      sSessionSeq,
                                      KNL_ERROR_STACK(aEnv) )
                     == STL_SUCCESS );
            sEnterState = 1;

            STL_TRY( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS );

            /**
             * sIdleStart 부터 sIdleEnd까지는 IDLE time추가
             */
            sIdleEnd = knlGetSystemTime();
            sSharedServer->mIdle += sIdleEnd - sIdleStart;

            continue;
        }

        /**
         * sIdleStart 부터 sIdleEnd까지는 IDLE time추가
         */
        sIdleEnd = knlGetSystemTime();
        sSharedServer->mIdle += sIdleEnd - sIdleStart;

        sSharedServer->mStatus = SLL_STATUS_RUN;

        /**
         * 현재 프로세스가 처리하고 있는 Handle을 설정함.
         * - signal handler에서 server failure메세지를 구성하기 위해 사용됨.
         */
        gZtsCurrentSllHandle = sSllHandle;

        sSclHandle = SCL_HANDLE(sSllHandle);

        /*
         * sHandle에서 mMemoryMgr는 재사용하고
         * 나머지는 여기서 session에 맞게 재설정 한다
         * 이 함수 이후로 gZtssHandle.mSession에 값이 설정되어 STL_SIGNAL_SEGV시 cleanup이 된다
         */
        STL_TRY( sllInitializeProtocolSentence( sSllHandle,
                                                &sSllProtocolSentence,
                                                &sMemoryMgr,
                                                aEnv )
                 == STL_SUCCESS );

        STL_DASSERT( sSllHandle->mSclHandle.mProtocolSentence->mMemoryMgr != NULL );

        STL_TRY( sclRecvPacket( sSclHandle,
                                SCL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* for fixed table */
        sSharedServer->mJobCount++;

        /* packet recv */
        if( sSllHandle->mDoneHandshake == STL_FALSE )
        {
            sSllHandle->mDoneHandshake = STL_TRUE;

            STL_TRY( ztssProcessHandshake( sSharedMemory,
                                           sSllHandle,
                                           aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            STL_TRY( ztssProcessRequest( sSharedMemory,
                                         aIndex,
                                         sSllHandle,
                                         sSessionEnv,
                                         aEnv )
                     == STL_SUCCESS );
        }

        /**
         * SllHandle은 사용하면 안된다.
         * - Client의 응답으로 인해 다른 Shared Server에서 SllHandle을
         *   사용하고 있을수도 있다.
         */
        sSllHandle = NULL;

        gZtsCurrentSllHandle = NULL;
        
        STL_TRY( sllFinalizeProtocolSentence( &sSllProtocolSentence,
                                              aEnv )
                 == STL_SUCCESS );
        
        sEnterState = 0;
        sslLeaveSession( sSessionId, sSessionSeq );

        STL_TRY( sslEnterSession( sSessionId,
                                  sSessionSeq,
                                  KNL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );
        sEnterState = 1;
    }


    sState = 1;
    gZtsJoinGroupWorker = STL_FALSE;
    smlDecRequestGroupWorker( SLL_REQUEST_GROUP_ID( sSharedMemory, aIndex ) );
    
    sState = 0;
    (void)sclFinalizeMemoryManager( &sMemoryMgr, SCL_ENV(aEnv) );

    sEnterState = 0;
    sslLeaveSession( sSessionId, sSessionSeq );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            smlDecRequestGroupWorker( SLL_REQUEST_GROUP_ID( sSharedMemory, aIndex ) );
        case 1:
            (void)sclFinalizeMemoryManager( &sMemoryMgr, SCL_ENV(aEnv) );
        default:
            break;
    }

    if( sEnterState == 1 )
    {
        sslLeaveSession( sSessionId, sSessionSeq );
    }


    return STL_FAILURE;
}

