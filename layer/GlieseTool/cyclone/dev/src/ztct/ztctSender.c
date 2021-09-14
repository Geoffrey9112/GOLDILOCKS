/*******************************************************************************
 * ztctSender.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file ztctSender.c
 * @brief GlieseTool Cyclone Master/Sender Communicator Routines
 */

#include <goldilocks.h>
#include <ztc.h>

extern stlBool         gRunState;
extern ztcMasterMgr  * gMasterMgr;
extern ztcMonitorInfo  gMonitorInfo;

void * STL_THREAD_FUNC ztctListenThread( stlThread * aThread, void * aArg )
{
    stlErrorStack   sErrorStack;
    stlInt32        sState = 0;
    
    STL_INIT_ERROR_STACK( &sErrorStack );
    
    STL_TRY( dtlInitialize() == STL_SUCCESS );
    sState = 1;

    STL_TRY( ztcmLogMsg( &sErrorStack,
                         "[SENDER] Create Listener.\n" ) == STL_SUCCESS );
    
    /**
     * Listener 생성 및 Client 접속 대기
     */
    if( ztcsCreateListener( &sErrorStack ) != STL_SUCCESS )
    {
        STL_TRY( gRunState == STL_FALSE );   
    }
    
    STL_TRY( ztcmLogMsg( &sErrorStack,
                         "[SENDER] Finalize Listener.\n" ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( dtlTerminate() == STL_SUCCESS );
    
    return NULL;

    STL_FINISH;
    
    (void) ztcmLogMsg( &sErrorStack,
                       "[SENDER] Error Occurred at Listener.\n" );
    
    ztcmWriteLastError( &sErrorStack );
    
    gRunState = STL_FALSE;
    
    switch( sState )
    {
        case 1:
            (void) dtlTerminate();
        default:
            break;
    }
    
    stlExitThread( aThread, STL_FAILURE, &sErrorStack );
    
    return NULL;
    
}


void * STL_THREAD_FUNC ztctSenderThread( stlThread * aThread, void * aArg )
{
    stlErrorStack    sErrorStack;
    stlInt32         sState = 0;
    ztcMasterCMMgr * sCMMgr = &gMasterMgr->mCMMgr;
    
    STL_INIT_ERROR_STACK( &sErrorStack );
    
    STL_TRY( dtlInitialize() == STL_SUCCESS );
    sState = 1;
    
    /**
     * Sender Protocol
     */
    (void) ztcsDoProtocol( &sErrorStack );
    
    ztcmWriteLastError( &sErrorStack );
    
    STL_TRY( ztcmLogMsg( &sErrorStack,
                         "[SENDER] Slave disconnected.\n" ) == STL_SUCCESS );
    
    STL_TRY( stlFinalizeContext( &(sCMMgr->mSenderContext),
                                 &sErrorStack ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( dtlTerminate() == STL_SUCCESS );
    
    /**
     * Capture Restart Flag 세팅
     */
    gMasterMgr->mCaptureRestart = STL_TRUE;
    sCMMgr->mSenderStatus       = STL_FALSE;
    
    /**
     * Monitor 정보 Update
     */
    gMonitorInfo.mMasterState = ZTC_MASTER_STATE_READY;
    gMonitorInfo.mSlaveState  = ZTC_SLAVE_STATE_NA;
    
    return NULL;

    STL_FINISH;
    
    /**
     * 여기로 왔다는 것은 Protocol 상의 문제가 아니라 
     * Initialize 과정에서 문제가 발생한것이므로 ...
     */ 
    (void) ztcmLogMsg( &sErrorStack,
                       "[SENDER] Error Occurred.\n" );
    
    ztcmWriteLastError( &sErrorStack );
    
    gRunState = STL_FALSE;
    
    (void) stlFinalizeContext( &(sCMMgr->mSenderContext),
                               &sErrorStack );
    
    switch( sState )
    {
        case 1:
            (void) dtlTerminate();
            break;
        default:
            break;
    }
    
    stlExitThread( aThread, STL_FAILURE, &sErrorStack );
    
    return NULL;
}


void * STL_THREAD_FUNC ztctMonitorThread( stlThread * aThread, void * aArg )
{
    stlErrorStack    sErrorStack;
    stlInt32         sState = 0;
    ztcMasterCMMgr * sCMMgr = &gMasterMgr->mCMMgr;
    
    STL_INIT_ERROR_STACK( &sErrorStack );
    
    STL_TRY( dtlInitialize() == STL_SUCCESS );
    sState = 1;
    
    /////////////////////////////////////
    
    //Do Receive Monitor Request...
    (void) ztcsMonitorProtocol( &sErrorStack );
    
    /////////////////////////////////////
    
    STL_TRY( ztcmLogMsg( &sErrorStack,
                         "[MONITOR] Monitor disconnected.\n" ) == STL_SUCCESS );
    
    STL_TRY( stlFinalizeContext( &(sCMMgr->mMonitorContext),
                                 &sErrorStack ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( dtlTerminate() == STL_SUCCESS );
    
    sCMMgr->mMonitorStatus = STL_FALSE;
    
    return NULL;

    STL_FINISH;
    
    (void) ztcmLogMsg( &sErrorStack,
                       "[MONITOR] Error Occurred.\n" );
    
    ztcmWriteLastError( &sErrorStack );
    
    gRunState = STL_FALSE;
    
    (void) stlFinalizeContext( &(sCMMgr->mMonitorContext),
                               &sErrorStack );
    
    switch( sState )
    {
        case 1:
            (void) dtlTerminate();
            break;
        default:
            break;
    }
    
    stlExitThread( aThread, STL_FAILURE, &sErrorStack );
    
    return NULL;
}

stlStatus ztctCreateSenderThread( stlErrorStack * aErrorStack )
{
    ztcMasterCMMgr * sCMMgr = &gMasterMgr->mCMMgr;
    
    STL_TRY( stlCreateThread( &(sCMMgr->mSenderThread),
                              NULL,
                              ztctSenderThread,
                              NULL,
                              aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztctCreateThreadCM4Master( stlErrorStack * aErrorStack )
{
    stlInt32  sState = 0;
    
    /**
     * CM Master Initialize
     */
    STL_TRY( ztcsInitializeCM4Master( aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    /**
     * Listener Thread 생성
     */
    STL_TRY( stlCreateThread( &(gMasterMgr->mCMMgr.mListenThread),
                              NULL,
                              ztctListenThread,
                              NULL,
                              aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    switch( sState )
    {
        case 1:
            (void) ztcsFinalizeCM4Master( aErrorStack );
            break;
        default:
            break;
    }

    return STL_FAILURE;
}


stlStatus ztctJoinThreadCM4Master( stlErrorStack * aErrorStack )
{
    stlStatus        sReturnStatus;
    ztcMasterCMMgr * sCMMgr  = &gMasterMgr->mCMMgr;
    
    /**
     * Listen Thread join (First !!!)
     */
    STL_TRY( stlJoinThread( &(sCMMgr->mListenThread),
                            &sReturnStatus,
                            aErrorStack ) == STL_SUCCESS );
    
    if( sCMMgr->mSenderStatus == STL_TRUE )
    {
        STL_TRY( stlJoinThread( &(sCMMgr->mSenderThread),
                                &sReturnStatus,
                                aErrorStack ) == STL_SUCCESS );
    }
    
    if( sCMMgr->mMonitorStatus == STL_TRUE )
    {
        STL_TRY( stlJoinThread( &(sCMMgr->mMonitorThread),
                                &sReturnStatus,
                                aErrorStack ) == STL_SUCCESS );
    }
    
    /**
     * CM Master Finalize
     */
    STL_TRY( ztcsFinalizeCM4Master( aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;    
}


stlStatus ztctCreateMonitorThread( stlErrorStack * aErrorStack )
{
    ztcMasterCMMgr * sCMMgr = &gMasterMgr->mCMMgr;
    
    STL_TRY( stlCreateThread( &(sCMMgr->mMonitorThread),
                              NULL,
                              ztctMonitorThread,
                              NULL,
                              aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/** @} */
