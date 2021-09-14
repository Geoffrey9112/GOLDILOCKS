/*******************************************************************************
 * ztrsListener.c
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
 * @file ztrsListener.c
 * @brief GlieseTool LogMirror Sender Listen Routines
 */

#include <goldilocks.h>
#include <ztr.h>

extern stlBool       gRunState;
extern ztrMasterMgr  gMasterMgr;

stlStatus ztrsCreateListener( stlErrorStack * aErrorStack )
{
    stlInt32         sState  = 0;
    stlInt32         sPort;
    stlContext       sNewContext;
    
    STL_TRY( ztrmGetPort( &sPort,
                          aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlAllocHeap( (void**)&gMasterMgr.mSenderHandle,
                           STL_SIZEOF( stlContext ),
                           aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( stlAllocHeap( (void**)&gMasterMgr.mListenHandle,
                           STL_SIZEOF( stlContext ),
                           aErrorStack ) == STL_SUCCESS );
    sState = 2;
    
    STL_TRY( stlInitializeContext( (stlContext*)gMasterMgr.mListenHandle,
                                   STL_AF_INET,
                                   STL_SOCK_STREAM,
                                   0,
                                   STL_TRUE,
                                   "0.0.0.0",
                                   sPort,
                                   aErrorStack ) == STL_SUCCESS );
    sState = 3;
    
    STL_TRY( ztrmLogMsg( aErrorStack,
                         "[LISTENER] START.\n" ) == STL_SUCCESS );
    
    
    while( 1 )
    {
        STL_TRY( gRunState == STL_TRUE ); 
        
        if( stlPollContext( (stlContext*)gMasterMgr.mListenHandle,
                            &sNewContext,
                            STL_SET_SEC_TIME( 1 ),
                            aErrorStack ) == STL_SUCCESS )
        {
            stlMemcpy( gMasterMgr.mSenderHandle,
                       &sNewContext, 
                       STL_SIZEOF( stlContext ) );
            
            /**
             * Socket Option을 Setting한다.
            - NoDelay 설정시 성능 차이 테스트해봐야 함
            STL_TRY( stlSetSocketOption( STL_SOCKET_IN_CONTEXT(*(stlContext*)gMasterMgr.mSenderHandle),
                                         STL_SOPT_TCP_NODELAY,
                                         STL_NO,
                                         aErrorStack ) == STL_SUCCESS );
             */
            STL_TRY( stlSetSocketOption( STL_SOCKET_IN_CONTEXT(*(stlContext*)gMasterMgr.mSenderHandle),
                                         STL_SOPT_SO_SNDBUF,
                                         ZTR_TCP_SOCKET_BUFF_SIZE,
                                         aErrorStack ) == STL_SUCCESS );
    
            STL_TRY( stlSetSocketOption( STL_SOCKET_IN_CONTEXT(*(stlContext*)gMasterMgr.mSenderHandle),
                                         STL_SOPT_SO_RCVBUF,
                                         ZTR_TCP_SOCKET_BUFF_SIZE,
                                         aErrorStack ) == STL_SUCCESS );
                
            STL_TRY( ztrmLogMsg( aErrorStack,
                                 "[SENDER]Slave Connected.\n" ) == STL_SUCCESS );
            
            break;
        }
        else
        {
            STL_TRY( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_TIMEDOUT );
            (void)stlPopError( aErrorStack );
        }
    }
    
    sState = 2;
    STL_TRY( stlFinalizeContext( (stlContext*)gMasterMgr.mListenHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;
    stlFreeHeap( gMasterMgr.mListenHandle );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    switch( sState )
    {
        case 3:
            (void)stlFinalizeContext( (stlContext*)gMasterMgr.mListenHandle,
                                      aErrorStack );
        case 2:
            stlFreeHeap( gMasterMgr.mListenHandle );
        case 1:
            stlFreeHeap( gMasterMgr.mSenderHandle );
        default:
            break;
    }
    
    return STL_FAILURE;
}

stlStatus ztrsCreateListener4IB( stlErrorStack * aErrorStack )
{
    stlInt32     sIbPort  = 0;
    
    STL_TRY( ztrmGetPort( &sIbPort,
                            aErrorStack ) == STL_SUCCESS );

    STL_TRY( ztrmLogMsg( aErrorStack,
                         "[LISTENER] START as Infiniband.\n" ) == STL_SUCCESS );
    
    while( 1 )
    {
        if ( stlRcCreateForServer( &(gMasterMgr.mSenderHandle),
                                   "0.0.0.0",               
                                   sIbPort,                 
                                   ZTR_INFINIBAND_BUFFER_SIZE,
                                   ZTR_INFINIBAND_BUFFER_COUNT_MASTER,
                                   ZTR_INFINIBAND_BUFFER_SIZE,
                                   ZTR_INFINIBAND_BUFFER_COUNT_SLAVE,
                                   STL_SET_SEC_TIME( 1 ),   /* timeout  */
                                   aErrorStack ) == STL_SUCCESS )
        {
            STL_TRY( ztrmLogMsg( aErrorStack,
                                 "[SENDER]Slave Connected.\n" ) == STL_SUCCESS );
            break;
        }
        else
        {
            STL_TRY( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_TIMEDOUT );
            (void)stlPopError( aErrorStack );
        }

        STL_TRY( gRunState == STL_TRUE );
    }
    
    STL_TRY( stlRcSetSendTimeout( gMasterMgr.mSenderHandle,
                                  STL_SET_MSEC_TIME( 100 ), /* Timeout */
                                  STL_SET_MSEC_TIME( 0 ),   /* Busywait Timeout */
                                  aErrorStack ) == STL_SUCCESS );
 
    STL_TRY( stlRcSetRecvTimeout( gMasterMgr.mSenderHandle,
                                  STL_SET_MSEC_TIME( 100 ), /* Timeout */
                                  STL_SET_MSEC_TIME( 0 ),   /* Busywait Timeout */
                                  aErrorStack ) == STL_SUCCESS );
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/** @} */
