/*******************************************************************************
 * ztcsListener.c
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
 * @file ztcsListener.c
 * @brief GlieseTool Cyclone Listener Routines
 */

#include <goldilocks.h>
#include <ztc.h>

extern ztcMonitorInfo  gMonitorInfo;

stlStatus ztcsCreateListener( stlErrorStack * aErrorStack )
{
    stlInt32         sState  = 0;
    stlInt32         sPort   = 0;
    ztcMasterCMMgr * sCMMgr  = &gMasterMgr->mCMMgr;
    stlContext       sNewContext;
    ztcProtocolHdr   sSndHeader;
    stlChar          sClientType;
    stlStatus        sReturnStatus;

    STL_TRY( ztcmGetPort( &sPort, aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlInitializeContext( &(sCMMgr->mListenContext),
                                   STL_AF_INET,
                                   STL_SOCK_STREAM,
                                   0,
                                   STL_TRUE,        //Listening
                                   "0.0.0.0",       //Localhost가 막혀있을수 있으므로 데모시에는 IP로 수정하도록 한다.
                                   sPort,
                                   aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    gMonitorInfo.mMasterState = ZTC_MASTER_STATE_RUNNING;
    gMonitorInfo.mSlaveState  = ZTC_SLAVE_STATE_RUNNING;
    stlStrcpy( gMonitorInfo.mSlaveIp, "N/A" );

    while( 1 )
    {
        STL_TRY( gRunState == STL_TRUE ); 
        
        if( stlPollContext( &(sCMMgr->mListenContext),
                            &sNewContext,
                            STL_SET_SEC_TIME( 1 ),
                            aErrorStack ) == STL_SUCCESS )
        {
            if( ztcsInitProtocol( &sNewContext,
                                  &sClientType,
                                  aErrorStack ) != STL_SUCCESS )
            {
                STL_TRY( ztcmLogMsg( aErrorStack,
                                     "[LISTENER] Protocol error.\n" ) == STL_SUCCESS );

                (void)stlPopError( aErrorStack );

                STL_TRY( stlFinalizeContext( &sNewContext,
                                             aErrorStack ) == STL_SUCCESS );
                sNewContext.mPollFd.mSocketHandle = -1;
                continue;
            }
            
            switch( sClientType )
            {
                case 's':       //Slave
                case 'S':       //Slave : sync mode
                {
//                    sCMMgr->mSenderStatus = STL_TRUE;

                    /**
                     * Monitor 정보 Update
                     */
//                    gMonitorInfo.mMasterState = ZTC_MASTER_STATE_RUNNING;
//                    gMonitorInfo.mSlaveState  = ZTC_SLAVE_STATE_RUNNING;

//                    (void) ztcmGetPeerIPAddr( &sNewContext,
//                                              gMonitorInfo.mSlaveIp,
//                                              STL_SIZEOF( gMonitorInfo.mSlaveIp ),
//                                              aErrorStack );

                    stlMemcpy( &gMasterMgr->mCMMgr.mSenderContext,
                               &sNewContext,
                               STL_SIZEOF( stlContext ) );

                    /**
                     * Socket Option을 Setting한다.
                    - NoDelay 설정시 성능 차이 테스트해봐야 함
                    STL_TRY( stlSetSocketOption( STL_SOCKET_IN_CONTEXT(gMasterMgr->mCMMgr.mSenderContext),
                                                 STL_SOPT_TCP_NODELAY,
                                                 STL_NO,
                                                 aErrorStack ) == STL_SUCCESS );
                     */
                    STL_TRY( stlSetSocketOption( STL_SOCKET_IN_CONTEXT(gMasterMgr->mCMMgr.mSenderContext),
                                                 STL_SOPT_SO_SNDBUF,
                                                 ZTC_TCP_SOCKET_BUFF_SIZE,
                                                 aErrorStack ) == STL_SUCCESS );

                    STL_TRY( stlSetSocketOption( STL_SOCKET_IN_CONTEXT(gMasterMgr->mCMMgr.mSenderContext),
                                                 STL_SOPT_SO_RCVBUF,
                                                 ZTC_TCP_SOCKET_BUFF_SIZE,
                                                 aErrorStack ) == STL_SUCCESS );

                    if( ztcsSendLogByteOrder( aErrorStack ) != STL_SUCCESS )
                    {
                        (void)stlPopError( aErrorStack );
                        STL_TRY( stlFinalizeContext( &sNewContext,
                                                     aErrorStack ) == STL_SUCCESS );
                        sNewContext.mPollFd.mSocketHandle = -1;
                        break;
                    }

                    /**
                     * Client에게 결과를 돌려줘야 한다.
                     */
                    sSndHeader.mCategory = ZTC_CATEGORY_INIT;
                    sSndHeader.mType     = ZTC_INIT_CLIENT_OK;
                    sSndHeader.mLength   = 0;

                    if( ztcmSendProtocolHdrNData( &gMasterMgr->mCMMgr.mSenderContext,
                                                  &sSndHeader,
                                                  NULL,
                                                  aErrorStack ) != STL_SUCCESS )
                    {
                        (void)stlPopError( aErrorStack );
                        STL_TRY( stlFinalizeContext( &sNewContext,
                                                     aErrorStack ) == STL_SUCCESS );
                        sNewContext.mPollFd.mSocketHandle = -1;
                        break;
                    }

                    STL_TRY( ztcmLogMsg( aErrorStack,
                                         "[LISTENER] Slave connected.\n" ) == STL_SUCCESS );

                    // 별도로 진행
                    STL_TRY( ztctCreateSenderThread( sClientType,
                                                     aErrorStack )
                             == STL_SUCCESS );

                    /**
                     * gMasterMgr->mCMMgr.mSenderContext 사용함으로 동시 사용 안됨.
                     * 여기서 join하고 기다려야 함.
                     */
                    STL_TRY( stlJoinThread( &(sCMMgr->mSenderThread),
                                            &sReturnStatus,
                                            aErrorStack ) == STL_SUCCESS );

                    (void)stlPopError( aErrorStack );

                    STL_TRY( stlFinalizeContext( &sNewContext,
                                                 aErrorStack ) == STL_SUCCESS );
                    sNewContext.mPollFd.mSocketHandle = -1;

                    break;
                }
                case 'm':       //Monitor
                {
                    if( sCMMgr->mMonitorStatus != STL_TRUE )
                    {
                        sCMMgr->mMonitorStatus = STL_TRUE;
                        
                        stlMemcpy( &gMasterMgr->mCMMgr.mMonitorContext, 
                                   &sNewContext, 
                                   STL_SIZEOF( stlContext ) );
                        
                        /**
                         * Client에게 결과를 돌려줘야 한다.
                         */
                        sSndHeader.mCategory = ZTC_CATEGORY_INIT;
                        sSndHeader.mType     = ZTC_INIT_CLIENT_OK;
                        sSndHeader.mLength   = 0;
            
                        if( ztcmSendProtocolHdrNData( &gMasterMgr->mCMMgr.mMonitorContext,
                                                      &sSndHeader,
                                                      NULL,
                                                      aErrorStack ) != STL_SUCCESS )
                        {
                            (void)stlPopError( aErrorStack );
                            STL_TRY( stlFinalizeContext( &sNewContext,
                                                         aErrorStack ) == STL_SUCCESS );
                            sNewContext.mPollFd.mSocketHandle = -1;
                            break;
                        }
                        
                        STL_TRY( ztcmLogMsg( aErrorStack,
                                             "[LISTENER] Monitor connected.\n" ) == STL_SUCCESS );
                 
                        
                        STL_TRY( ztctCreateMonitorThread( aErrorStack ) == STL_SUCCESS );
                    }
                    else
                    {
                        STL_TRY( ztcmLogMsg( aErrorStack,
                                             "[LISTENER] Monitor connected but already exist.\n" ) == STL_SUCCESS );
                        
                        sSndHeader.mCategory = ZTC_CATEGORY_ERROR;
                        sSndHeader.mType     = ZTC_ERROR_SLAVE_ALREADY_CONNECTED;
                        sSndHeader.mLength   = 0;
            
                        if( ztcmSendProtocolHdrNData( &sNewContext,
                                                      &sSndHeader,
                                                      NULL,
                                                      aErrorStack ) != STL_SUCCESS )
                        {
                            (void)stlPopError( aErrorStack );
                        }

                        STL_TRY( stlFinalizeContext( &sNewContext,
                                                     aErrorStack ) == STL_SUCCESS );
                        sNewContext.mPollFd.mSocketHandle = -1;
                    }
                    
                    break;
                }
                default:
                {
                    STL_THROW( RAMP_ERR_INVALID_CLIENT_TYPE );
                }
            }
        }            
        else
        {
            STL_TRY( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_TIMEDOUT );
            (void)stlPopError( aErrorStack );

//            printf("%ld tps, %ld msgs \n", gCommitIdx, gMsgCnt );
        }
    }
    
    sState = 0;
    STL_TRY( stlFinalizeContext( &(gMasterMgr->mCMMgr.mListenContext),
                                 aErrorStack ) == STL_SUCCESS );
    gMasterMgr->mCMMgr.mListenContext.mPollFd.mSocketHandle = -1;
    
    return STL_SUCCESS;   
    
    STL_CATCH( RAMP_ERR_INVALID_CLIENT_TYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_PROTOCOL,
                      NULL,
                      aErrorStack,
                      "Invalid Client Type" );
    }
    STL_FINISH;
    
    switch( sState )
    {
        case 1:
            (void)stlFinalizeContext( &(gMasterMgr->mCMMgr.mListenContext),
                                      aErrorStack );
            gMasterMgr->mCMMgr.mListenContext.mPollFd.mSocketHandle = -1;

        default:
            break;
    }
    
    return STL_FAILURE;
}

/** @} */
