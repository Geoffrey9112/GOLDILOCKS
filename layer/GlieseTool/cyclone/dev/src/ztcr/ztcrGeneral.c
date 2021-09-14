/*******************************************************************************
 * ztcrGeneral.c
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
 * @file ztcrGeneral.c
 * @brief GlieseTool Cyclone Slave/Receiver General Routines
 */

#include <goldilocks.h>
#include <ztc.h>

extern ztcSlaveMgr * gSlaveMgr;
extern stlBool       gRunState;

stlStatus ztcrInitializeCM4Slave( stlErrorStack * aErrorStack )
{

    return STL_SUCCESS;
}

stlStatus ztcrFinalizeCM4Slave( stlErrorStack * aErrorStack )
{

    return STL_SUCCESS;
}


stlStatus ztcrCreateReceiver( stlErrorStack * aErrorStack )
{
    stlInt32   sState = 0;
    stlInt32   sPort  = 0;
    stlChar  * sIp    = ztcmGetMasterIp();
    
    STL_TRY( ztcmGetPort( &sPort, 
                          aErrorStack ) == STL_SUCCESS );
    
    while( 1 )
    {
        //Create Socket
        STL_TRY( stlInitializeContext( &(gSlaveMgr->mCMMgr.mContext),
                                       STL_AF_INET,
                                       STL_SOCK_STREAM,
                                       0,
                                       STL_FALSE,
                                       sIp,
                                       sPort,
                                       aErrorStack ) == STL_SUCCESS );
        sState = 1;
        
        /**
         * Socket Option을 Setting한다.
        - NoDelay 설정시 성능 차이 테스트해봐야 함
        STL_TRY( stlSetSocketOption( STL_SOCKET_IN_CONTEXT(gSlaveMgr->mCMMgr.mContext),
                                     STL_SOPT_TCP_NODELAY,
                                     STL_NO,
                                     aErrorStack ) == STL_SUCCESS );
         */
        STL_TRY( stlSetSocketOption( STL_SOCKET_IN_CONTEXT(gSlaveMgr->mCMMgr.mContext),
                                     STL_SOPT_SO_SNDBUF,
                                     ZTC_TCP_SOCKET_BUFF_SIZE,
                                     aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( stlSetSocketOption( STL_SOCKET_IN_CONTEXT(gSlaveMgr->mCMMgr.mContext),
                                     STL_SOPT_SO_RCVBUF,
                                     ZTC_TCP_SOCKET_BUFF_SIZE,
                                     aErrorStack ) == STL_SUCCESS );
        
        gSlaveMgr->mCMMgr.mContext.mPollFd.mReqEvents = STL_POLLIN | STL_POLLERR;
        
        if ( stlConnectContext( &(gSlaveMgr->mCMMgr.mContext),
                                STL_SET_SEC_TIME( 1 ),      //Polling Time.
                                aErrorStack ) == STL_SUCCESS )
        {
            STL_TRY( ztcmLogMsg( aErrorStack,
                                 "[RECEIVER] Connect success.\n" ) == STL_SUCCESS );
            break;
        }
        
        STL_TRY( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_CONNECT ||
                 stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_TIMEDOUT );

        (void)stlPopError( aErrorStack );
      
        /**
         * TODO: Retry Count Property를 추가하고 해당 시간동안만 접속하도록 해야함??
         * 지금은 계속 접속 시도하도록 함!!
         */
        stlSleep( STL_SET_SEC_TIME( 1 ) );
        
        STL_TRY_THROW( gRunState == STL_TRUE, RAMP_SUCCESS );
        
        sState = 0;
        STL_TRY( stlFinalizeContext( &(gSlaveMgr->mCMMgr.mContext),
                                     aErrorStack ) == STL_SUCCESS );
    
    }
    
    STL_TRY( ztcrDoProtocol( aErrorStack ) == STL_SUCCESS ); 
    
    STL_RAMP( RAMP_SUCCESS );
    
    sState = 0;
    STL_TRY( stlFinalizeContext( &(gSlaveMgr->mCMMgr.mContext),
                                 aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcmLogMsg( aErrorStack,
                         "[RECEIVER] Finalize Done.\n" ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    if( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_READ_PACKET )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_MASTER_DISCONNT,
                      NULL,
                      aErrorStack );
    }
    
    switch( sState )
    {
        case 1:
            (void)stlFinalizeContext( &(gSlaveMgr->mCMMgr.mContext),
                                      aErrorStack );
            break;
        default:
            break;
        
    }
    
    
    return STL_FAILURE;
}


/** @} */
