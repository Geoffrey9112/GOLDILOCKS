/*******************************************************************************
 * ztrrGeneral.c
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
 * @file ztrrGeneral.c
 * @brief GlieseTool LogMirror Receiver Routines
 */

#include <goldilocks.h>
#include <ztr.h>

extern stlBool      gRunState;
extern ztrSlaveMgr  gSlaveMgr;
extern stlBool      gZtrFlusherState;

extern ztrCommunicateCallback gZtrCommCallback;

stlStatus ztrrInitializeReceiver( stlErrorStack * aErrorStack )
{
    STL_TRY( gZtrCommCallback.mCreateReceiverFunc( aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS; 
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztrrFinalizeReceiver( stlErrorStack * aErrorStack )
{
    STL_TRY( gZtrCommCallback.mDestroyHandleFunc( gSlaveMgr.mRecvHandle,
                                                  aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztrrReceiverMain( stlErrorStack * aErrorStack )
{
    stlInt32 sState = 0;
    
    STL_TRY( ztrrInitializeReceiver( aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    /**
     * Protocol을 수행한다.
     */
    STL_TRY( ztrrDoProtocol( aErrorStack ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( ztrrFinalizeReceiver( aErrorStack ) == STL_SUCCESS );
    
    /**
     * Flusher가 종료할 수 있도록 한다.
     * Flusher는 Buffer를 모두 Flush한 뒤에 종료해야 한다.
     */
    gZtrFlusherState = STL_FALSE;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    /**
     * Flusher가 종료할 수 있도록 한다.
     * Flusher는 Buffer를 모두 Flush한 뒤에 종료해야 한다.
     */
    gZtrFlusherState = STL_FALSE;
    
    switch( sState )
    {
        case 1:
            (void) ztrrFinalizeReceiver( aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztrrCreateReceiver( stlErrorStack * aErrorStack )
{
    stlInt32   sPort;
    stlInt32   sState    = 0;
    stlChar  * sMasterIp = ztrmGetMasterIp();
    
    STL_TRY( stlAllocHeap( (void**)&gSlaveMgr.mRecvHandle,
                           STL_SIZEOF( stlContext ),
                           aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztrmGetPort( &sPort,
                          aErrorStack ) == STL_SUCCESS );
    
    while( 1 )
    {
        STL_TRY( stlInitializeContext( (stlContext*)gSlaveMgr.mRecvHandle,
                                       STL_AF_INET,
                                       STL_SOCK_STREAM,
                                       0,
                                       STL_FALSE,
                                       sMasterIp,
                                       sPort,
                                       aErrorStack ) == STL_SUCCESS );
        sState = 2;
        
        /**
         * Socket Option을 Setting한다.
        - NoDelay 설정시 성능 차이 테스트해봐야 함
        STL_TRY( stlSetSocketOption( STL_SOCKET_IN_CONTEXT(*(stlContext*)gSlaveMgr.mRecvHandle),
                                     STL_SOPT_TCP_NODELAY,
                                     STL_NO,
                                     aErrorStack ) == STL_SUCCESS );
         */
        STL_TRY( stlSetSocketOption( STL_SOCKET_IN_CONTEXT(*(stlContext*)gSlaveMgr.mRecvHandle),
                                     STL_SOPT_SO_SNDBUF,
                                     ZTR_TCP_SOCKET_BUFF_SIZE,
                                     aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( stlSetSocketOption( STL_SOCKET_IN_CONTEXT(*(stlContext*)gSlaveMgr.mRecvHandle),
                                     STL_SOPT_SO_RCVBUF,
                                     ZTR_TCP_SOCKET_BUFF_SIZE,
                                     aErrorStack ) == STL_SUCCESS );
        
        ((stlContext*)(gSlaveMgr.mRecvHandle))->mPollFd.mReqEvents = STL_POLLIN | STL_POLLERR;
    
        if ( stlConnectContext( (stlContext*)gSlaveMgr.mRecvHandle,
                                STL_SET_SEC_TIME( 1 ),
                                aErrorStack ) == STL_SUCCESS )
        {
            STL_TRY( ztrmLogMsg( aErrorStack,
                                 "[RECEIVER] Connect success.\n" ) == STL_SUCCESS );
            break;
        }

        STL_TRY( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_CONNECT ||
                 stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_TIMEDOUT );
        
        (void)stlPopError( aErrorStack );

        //stlSleep( STL_SET_SEC_TIME( 1 ) );
        
        STL_TRY( gRunState == STL_TRUE );
        
        sState = 1;
        STL_TRY( stlFinalizeContext( (stlContext*)gSlaveMgr.mRecvHandle,
                                     aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    if( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_READ_PACKET )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_MASTER_DISCONNT,
                      NULL,
                      aErrorStack );
    }
    
    switch( sState )
    {
        case 2:
            (void)stlFinalizeContext( (stlContext*)gSlaveMgr.mRecvHandle,
                                      aErrorStack );
        case 1:
            stlFreeHeap( gSlaveMgr.mRecvHandle );
        default:
            break;
    }
    
    return STL_FAILURE;
}

stlStatus ztrrCreateReceiver4IB( stlErrorStack * aErrorStack )
{
    stlInt32     sIbPort   = 0;
    stlChar    * sMasterIp = ztrmGetMasterIp();
    
    STL_TRY( ztrmGetPort( &sIbPort,
                          aErrorStack ) == STL_SUCCESS );
    
    while( 1 )
    {
        if( stlRcCreateForClient( &(gSlaveMgr.mRecvHandle),
                                  "0.0.0.0",
                                  sMasterIp,
                                  sIbPort,
                                  ZTR_INFINIBAND_BUFFER_SIZE,
                                  ZTR_INFINIBAND_BUFFER_COUNT_SLAVE,
                                  ZTR_INFINIBAND_BUFFER_SIZE,
                                  ZTR_INFINIBAND_BUFFER_COUNT_MASTER,
                                  aErrorStack ) == STL_SUCCESS )
        {
            STL_TRY( ztrmLogMsg( aErrorStack,
                                 "[RECEIVER] Connect success as Infiniband.\n" ) == STL_SUCCESS );
            break;
        }
        else
        {
            STL_TRY( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_CONNECT );
        
            (void)stlPopError( aErrorStack );
            
            STL_TRY( gRunState == STL_TRUE );
            
            /**
             * 접속 실패시 1초 대기후 다시 시도해 본다.
             */
            stlSleep( STL_SET_SEC_TIME( 1 ) );
            
        }
    }

    STL_TRY( stlRcSetSendTimeout( gSlaveMgr.mRecvHandle,
                                  STL_SET_MSEC_TIME( 100 ), /* Timeout */
                                  STL_SET_MSEC_TIME( 0 ),   /* Busywait Timeout */
                                  aErrorStack ) == STL_SUCCESS );
 
    STL_TRY( stlRcSetRecvTimeout( gSlaveMgr.mRecvHandle,
                                  STL_SET_MSEC_TIME( 100 ), /* Timeout */
                                  STL_SET_MSEC_TIME( 0 ),   /* Busywait Timeout */
                                  aErrorStack ) == STL_SUCCESS );
   
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/** @} */
