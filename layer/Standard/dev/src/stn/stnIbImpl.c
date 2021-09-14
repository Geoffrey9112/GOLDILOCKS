/*******************************************************************************
 * stnIbImpl.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id:$
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stl.h>
#include <stn.h>
#include <stnIbDef.h>
#include <stnIbIntf.h>
#include <stnIbImpl.h>

/* infiniband 규격상 2초(2000) 이내에 event가 옴으로 TIMEOUT은 3초면 충분하다고 함 */
#define     STN_IB_TIMEOUT      (3000)

stnIbCallback gIbCallback = { 0, };

/**
 * @brief infiniband 함수를 사용하기 위하여 함수 포인터를 설정한다.
 * @param[out] aErrorStack      에러 스택
 */
stlStatus stnIbInitCallback( stlErrorStack * aErrorStack )
{
    stlDsoHandle sHandle = NULL;
    stlChar      sFuncName[32];

    STL_TRY( stlOpenLibrary( IB_LIBRARY_NAME, &sHandle, aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnIbvPollCq" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mIbvPollCq,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnIbvPostSend" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mIbvPostSend,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnIbvPostRecv" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mIbvPostRecv,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnIbvGetDeviceList" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mIbvGetDeviceList,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnIbvFreeDeviceList" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mIbvFreeDeviceList,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnIbvGetDeviceName" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mIbvGetDeviceName,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnIbvOpenDevice" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mIbvOpenDevice,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnIbvCloseDevice" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mIbvCloseDevice,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnIbvQueryPort" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mIbvQueryPort,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnIbvAllocPd" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mIbvAllocPd,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnIbvDeallocPd" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mIbvDeallocPd,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnIbvCreateCq" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mIbvCreateCq,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnIbvDestroyCq" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mIbvDestroyCq,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnIbvRegMr" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mIbvRegMr,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnIbvDeregMr" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mIbvDeregMr,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnIbvCreateQp" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mIbvCreateQp,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnIbvDestroyQp" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mIbvDestroyQp,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnIbvModifyQp" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mIbvModifyQp,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnIbvQueryGid" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mIbvQueryGid,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnIbvReqNotifyCq" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mIbvReqNotifyCq,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnIbvGetCqEvent" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mIbvGetCqEvent,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnIbvAckCqEvents" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mIbvAckCqEvents,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnIbvWcStatusStr" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mIbvWcStatusStr,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnRdmaCmCreateEventChannel" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mCmCreateEventChannel,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnRdmaCmDestroyEventChannel" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mCmDestroyEventChannel,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnRdmaCmCreateId" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mCmCreateId,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnRdmaCmDestroyId" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mCmDestroyId,
                           aErrorStack ) == STL_SUCCESS );
 
    stlSnprintf( sFuncName, 32, "stnRdmaCmBindAddr" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mCmBindAddr,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnRdmaCmResolveAddr" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mCmResolveAddr,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnRdmaCmResolveRoute" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mCmResolveRoute,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnRdmaCmCreateQp" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mCmCreateQp,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnRdmaCmDestroyQp" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mCmDestroyQp,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnRdmaCmConnect" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mCmConnect,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnRdmaCmListen" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mCmListen,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnRdmaCmAccept" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mCmAccept,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnRdmaCmReject" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mCmReject,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnRdmaCmNotify" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mCmNotify,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnRdmaCmDisconnect" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mCmDisconnect,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnRdmaCmJoinMulticast" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mCmJoinMulticast,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnRdmaCmLeaveMulticast" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mCmLeaveMulticast,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnRdmaCmGetCmEvent" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mCmGetCmEvent,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnRdmaCmAckCmEvent" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mCmAckCmEvent,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnRdmaCmGetDevices" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mCmGetDevices,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnRdmaCmFreeDevices" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mCmFreeDevices,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnRdmaCmEventStr" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mCmEventStr,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnRdmaPostSend" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mCmPostSend,
                           aErrorStack ) == STL_SUCCESS );

    stlSnprintf( sFuncName, 32, "stnRdmaPostRecv" );
    STL_TRY( stlGetSymbol( sHandle,
                           sFuncName,
                           (stlDsoSymHandle*)&gIbCallback.mCmPostRecv,
                           aErrorStack ) == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stnRcCreateForServer( stlRcHandle   * aHandle,
                                stlChar       * aBindAddr,
                                stlUInt16       aBindPort, 
                                stlUInt32       aSendWindowSize,
                                stlUInt32       aSendWindowsCount,
                                stlUInt32       aRecvWindowSize,
                                stlUInt32       aRecvWindowsCount,
                                stlInterval     aTimeoutUsec,
                                stlErrorStack * aErrorStack )
{
    stlChar                     sMsg[128];
    stnRcResource             * sRes = NULL;
    struct rdma_event_channel * sChannel = NULL;
    stlSockAddr                 sBindAddr;
    struct rdma_cm_event      * sEvent = NULL;
    struct rdma_conn_param      sConnParam;
    stlUInt8                  * sWindow = NULL;
    stlInt32                    i; 
    struct rdma_cm_id         * sListenCmId = NULL;
    struct ibv_qp_init_attr     sQpAttr;
    stlPollFd                   sFds[1];
    stlInt32                    sSigFdNum = 0;
    stlUInt32                   sBuff[2];
    stlSize                     sLen;

    STL_TRY_THROW( aSendWindowSize > 100, RAMP_ERR_INVALID_PARAM );
    STL_TRY_THROW( aRecvWindowSize > 100, RAMP_ERR_INVALID_PARAM );

    if( gIbCallback.mIbvPollCq == NULL )
    {
        STL_TRY( stnIbInitCallback( aErrorStack ) == STL_SUCCESS );
    }

    STL_TRY( stlAllocHeap( (void**)&sRes, 
                           STL_SIZEOF(stnRcResource), 
                           aErrorStack ) == STL_SUCCESS );

    stlMemset( sRes, 0x00, STL_SIZEOF(stnRcResource) ); 

    /* window 크기, 개수 설정 */
    sRes->mSendWindowSize = aSendWindowSize;
    sRes->mSendWindowsCount = aSendWindowsCount;
    sRes->mRecvWindowSize = aRecvWindowSize;
    sRes->mRecvWindowsCount = aRecvWindowsCount;

    /* default timeout, busywait time 설정 */
    STL_TRY( stnRcSetSendTimeout( sRes, 
                                  STL_SET_MSEC_TIME( 100 ),
                                  STL_SET_MSEC_TIME( 0 ),
                                  aErrorStack ) == STL_SUCCESS );

    STL_TRY( stnRcSetRecvTimeout( sRes, 
                                  STL_SET_MSEC_TIME( 100 ),
                                  STL_SET_MSEC_TIME( 0 ),
                                  aErrorStack ) == STL_SUCCESS );

    sChannel = gIbCallback.mCmCreateEventChannel();
    STL_TRY_THROW( sChannel != NULL, RAMP_ERR_RDMA_CREATE_CHANNEL );

    STL_TRY_THROW( gIbCallback.mCmCreateId( sChannel, 
                                            &sRes->mCmId, 
                                            NULL, 
                                            RDMA_PS_TCP ) == STL_SUCCESS,
                   RAMP_ERR_CREATE_ID );
    STL_ASSERT( sRes->mCmId != NULL );

    STL_TRY( stlSetSockAddr( &sBindAddr, 
                             AF_INET, 
                             aBindAddr, 
                             aBindPort, 
                             NULL, 
                             aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( gIbCallback.mCmBindAddr( sRes->mCmId, &sBindAddr ) == STL_SUCCESS, 
                   RAMP_ERR_RDMA_BIND );

    STL_TRY_THROW( gIbCallback.mCmListen( sRes->mCmId, 0 ) == STL_SUCCESS, 
                   RAMP_ERR_RDMA_LISTEN );

    sFds[0].mSocketHandle = sRes->mCmId->channel->fd;
    sFds[0].mReqEvents = STL_POLLIN;    /* ib에서 STL_POLLERR 설정 안됨 */
    sFds[0].mRetEvents = 0;

    /* timeout 시 STL_ERRCODE_TIMEDOUT 발생 */
    STL_TRY( stlPoll( sFds, 1, &sSigFdNum, aTimeoutUsec, aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( gIbCallback.mCmGetCmEvent( sRes->mCmId->channel,     
                                              &sEvent ) == STL_SUCCESS, 
                   RAMP_ERR_RDMA_GET_CM_EVENT );
    STL_TRY_THROW( sEvent->event == RDMA_CM_EVENT_CONNECT_REQUEST, 
                   RAMP_ERR_EVENT_FOR_LISTEN );

    /* cmid를 accept cm id로 변경   */
    sListenCmId = sRes->mCmId;
    sRes->mCmId = sEvent->id;

    STL_TRY_THROW( gIbCallback.mCmAckCmEvent( sEvent ) == STL_SUCCESS, RAMP_ERR_RDMA_ACK_EVENT );
    sEvent = NULL;

    /* create qp    */
    stlMemset( &sQpAttr, 0x00, STL_SIZEOF(sQpAttr) ); 
    sQpAttr.qp_context = NULL;
    sQpAttr.sq_sig_all = 0;
                      
    sQpAttr.cap.max_send_wr = aSendWindowsCount;
    sQpAttr.cap.max_recv_wr = aRecvWindowsCount;
    sQpAttr.cap.max_send_sge = 1;
    sQpAttr.cap.max_recv_sge = 1;
    /** latency는 INLINE이 좋고 throughput은 INLINE 사용 안하는것이 좋음.
     *  sQpAttr.cap.max_inline_data = 100;
     */
    sQpAttr.qp_type = IBV_QPT_RC;

    STL_TRY_THROW( gIbCallback.mCmCreateQp( sRes->mCmId, NULL, &sQpAttr ) == STL_SUCCESS, 
                   RAMP_ERR_RDMA_CREATE_QP );

    sRes->mMaxInlineSize = sQpAttr.cap.max_inline_data;

    /* mr for send */
    STL_TRY( stlAllocHeap( (void**)&sWindow, 
                           aSendWindowsCount * aSendWindowSize, 
                           aErrorStack ) == STL_SUCCESS );

    sRes->mSendMr = gIbCallback.mIbvRegMr( sRes->mCmId->pd,
                                           sWindow,
                                           aSendWindowsCount * aSendWindowSize,
                                           IBV_ACCESS_LOCAL_WRITE );
    STL_TRY_THROW( sRes->mSendMr != NULL, RAMP_ERR_RDMA_REG_MR );
    sWindow = NULL;

    /* mr for recv */
    STL_TRY( stlAllocHeap( (void**)&sWindow, 
                           aRecvWindowsCount * aRecvWindowSize, 
                           aErrorStack ) == STL_SUCCESS );

    sRes->mRecvMr = gIbCallback.mIbvRegMr( sRes->mCmId->pd,
                                           sWindow,
                                           aRecvWindowsCount * aRecvWindowSize,
                                           IBV_ACCESS_LOCAL_WRITE );
    STL_TRY_THROW( sRes->mRecvMr != NULL, RAMP_ERR_RDMA_REG_MR );
    sWindow = NULL;

    for( i = 0; i < aRecvWindowsCount; i++ ) 
    {
        STL_TRY_THROW( gIbCallback.mCmPostRecv( sRes->mCmId,
                                                sRes->mRecvMr->addr + ( i * aRecvWindowSize ),
                                                sRes->mRecvMr->addr + ( i * aRecvWindowSize ) ,
                                                aRecvWindowSize,
                                                sRes->mRecvMr ) == STL_SUCCESS, 
                       RAMP_ERR_RDMA_POST_RECV );
    }

    stlMemset( &sConnParam, 0x00, STL_SIZEOF(sConnParam) ); 
    sConnParam.responder_resources = 1;
    sConnParam.initiator_depth = 1;
    sConnParam.retry_count = 7;
    /** 이 값을 작게 하면 buffer 부족시 바로
     *  IBV_WC_RNR_RETRY_EXC_ERR error 발생함
     */
    sConnParam.rnr_retry_count = 7;

    STL_TRY_THROW( gIbCallback.mCmAccept( sRes->mCmId, &sConnParam ) == STL_SUCCESS, 
                   RAMP_ERR_RDMA_ACCEPT );

    STL_TRY_THROW( gIbCallback.mCmGetCmEvent( sRes->mCmId->channel, &sEvent ) == STL_SUCCESS, 
                   RAMP_ERR_RDMA_GET_CM_EVENT );
    STL_TRY_THROW( sEvent->event == RDMA_CM_EVENT_ESTABLISHED, RAMP_ERR_EVENT_FOR_ACCEPT );

    STL_TRY_THROW( gIbCallback.mCmAckCmEvent( sEvent ) == STL_SUCCESS, RAMP_ERR_RDMA_ACK_EVENT );
    sEvent = NULL;

    STL_TRY_THROW( gIbCallback.mCmDestroyId( sListenCmId ) == STL_SUCCESS, RAMP_ERR_RDMA_DESTROY_ID );
    sListenCmId = 0;

    /* windows size 상대방과 확인 */
    sBuff[0] = stlToNetLong( sRes->mSendWindowSize );
    sBuff[1] = stlToNetLong( sRes->mRecvWindowSize );
    sLen = STL_SIZEOF(sBuff);
    
    STL_TRY( stnRcSend( (stlRcHandle *)sRes, 
                        (stlChar *)&sBuff, 
                        &sLen, 
                        aErrorStack ) == STL_SUCCESS );

    sLen = STL_SIZEOF(sBuff);
    STL_TRY( stnRcRecv( (stlRcHandle *)sRes, 
                        (stlChar *)&sBuff, 
                        &sLen, 
                        aErrorStack ) == STL_SUCCESS );

    sBuff[0] = stlToHostLong( sBuff[0] );
    sBuff[1] = stlToHostLong( sBuff[1] );

    /* 상대방의 recv window size와 내 send window size 비교 */
    STL_TRY_THROW( sBuff[1] == sRes->mSendWindowSize, RAMP_ERR_WRONG_WINDOW_SIZE );
     
    /* 상대방의 send window size와 내 recv window size 비교 */
    STL_TRY_THROW( sBuff[0] == sRes->mRecvWindowSize, RAMP_ERR_WRONG_WINDOW_SIZE );

    *aHandle = sRes;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_PARAM )
    {
        stlSnprintf( sMsg, 
                     128, 
                     "rdma invalid param: aSendWindowSize[%u], aRecvWindowSize[%u]", 
                     aSendWindowSize,
                     aRecvWindowSize );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INVALID_ARGUMENT,
                      sMsg,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_RDMA_CREATE_CHANNEL )
    {
        stlSnprintf( sMsg, 128, "rdma_create_event_channel returns errno [%d]", errno );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_IBV_VERBS_FAILURE,
                      sMsg,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_CREATE_ID )
    {
        stlSnprintf( sMsg, 128, "rdma_create_id returns cmid[%p], errno [%d]", sRes->mCmId, errno );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_IBV_VERBS_FAILURE,
                      sMsg,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_RDMA_BIND )
    {
        stlSnprintf( sMsg, 128, "rdma_bind_addr(%s) returns errno[%d]", aBindAddr, errno );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_IBV_VERBS_FAILURE,
                      sMsg,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_RDMA_LISTEN )
    {
        stlSnprintf( sMsg, 128, "rdma_listen returns errno [%d]", errno );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_IBV_VERBS_FAILURE,
                      sMsg,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_RDMA_GET_CM_EVENT )
    {
        stlSnprintf( sMsg, 128, "rdma_get_cm_event returns errno [%d]", errno );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_IBV_VERBS_FAILURE,
                      sMsg,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_EVENT_FOR_LISTEN )
    {
        stlSnprintf( sMsg, 
                     128, 
                     "rdma event: expect RDMA_CM_EVENT_CONNECT_REQUEST, received %s(%d)", 
                     gIbCallback.mCmEventStr( sEvent->event ),
                     sEvent->event );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_IBV_VERBS_FAILURE,
                      sMsg,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_RDMA_ACK_EVENT )
    {
        stlSnprintf( sMsg, 128, "rdma_ack_cm_event returns errno [%d]", errno );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_IBV_VERBS_FAILURE,
                      sMsg,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_RDMA_CREATE_QP )
    {
        stlSnprintf( sMsg, 128, "rdma_create_qp returns errno [%d]", errno );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_IBV_VERBS_FAILURE,
                      sMsg,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_RDMA_ACCEPT )
    {
        stlSnprintf( sMsg, 128, "rdma_accept returns errno [%d]", errno );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_IBV_VERBS_FAILURE,
                      sMsg,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_EVENT_FOR_ACCEPT )
    {
        stlSnprintf( sMsg, 
                     128, 
                     "rdma event: expect RDMA_CM_EVENT_ESTABLISHED, received %s(%d)", 
                     gIbCallback.mCmEventStr( sEvent->event ),
                     sEvent->event );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_IBV_VERBS_FAILURE,
                      sMsg,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_RDMA_POST_RECV )
    {
        stlSnprintf( sMsg, 128, "rdma_post_recv returns errno [%d]", errno );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_IBV_VERBS_FAILURE,
                      sMsg,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_RDMA_REG_MR )
    {
        stlSnprintf( sMsg, 128, "ibv_reg_mr returns errno [%d]", errno );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_IBV_VERBS_FAILURE,
                      sMsg,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_RDMA_DESTROY_ID )
    {
        stlSnprintf( sMsg, 128, "rdma_destroy_id returns errno [%d]", errno );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_IBV_VERBS_FAILURE,
                      sMsg,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_WRONG_WINDOW_SIZE )
    {
        stlSnprintf( sMsg, 
                     128, 
                     "rdma wrong window size: My[%u]->Peer[%u], Peer[%u]->My[%u]", 
                     sRes->mSendWindowSize,
                     sBuff[1],
                     sBuff[0],
                     sRes->mRecvWindowSize );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_MISMATCH,
                      sMsg,
                      aErrorStack );
    }

    STL_FINISH;

    /* ack안된 event있으면 ack한다 */
    if( sEvent != NULL )
    {
        (void)gIbCallback.mCmAckCmEvent( sEvent );
    }
    if( sWindow != NULL )
    {
        stlFreeHeap( sWindow );
    }

    if( sRes != NULL )
    {
        if( sRes->mSendMr != NULL )
        {
            stlFreeHeap( sRes->mSendMr->addr );
            (void)gIbCallback.mIbvDeregMr( sRes->mSendMr );
        }

        if( sRes->mRecvMr != NULL )
        {
            stlFreeHeap( sRes->mRecvMr->addr );
            (void)gIbCallback.mIbvDeregMr( sRes->mRecvMr );
        }

        if( sListenCmId != NULL )
        {
            gIbCallback.mCmDestroyId( sListenCmId );
        }

        if( sRes->mCmId != NULL )
        {
            if( sRes->mCmId->qp != NULL )
            {
                gIbCallback.mCmDestroyQp( sRes->mCmId );
            }
            (void)gIbCallback.mCmDestroyId( sRes->mCmId );
        }

        stlFreeHeap( sRes );
    }

    if( sChannel != NULL )
    {
        gIbCallback.mCmDestroyEventChannel( sChannel );
    }

    return STL_FAILURE;
}

stlStatus stnRcCreateForClient( stlRcHandle   * aHandle,
                                stlChar       * aBindAddr,
                                stlChar       * aTargetAddr,
                                stlUInt16       aTargetPort, 
                                stlUInt32       aSendWindowSize,
                                stlUInt32       aSendWindowsCount,
                                stlUInt32       aRecvWindowSize,
                                stlUInt32       aRecvWindowsCount,
                                stlErrorStack * aErrorStack )
{
    stlChar                     sMsg[128];
    stnRcResource             * sRes = NULL;
    struct rdma_event_channel * sChannel = NULL;
    stlSockAddr                 sBindAddr;
    stlSockAddr                 sTargetAddr;
    struct rdma_cm_event      * sEvent = NULL;
    struct rdma_conn_param      sConnParam;
    stlUInt8                  * sWindow = NULL;
    stlInt32                    i; 
    struct ibv_qp_init_attr     sQpAttr;
    stlUInt32                   sBuff[2];
    stlSize                     sLen;

    STL_TRY_THROW( aSendWindowSize > 100, RAMP_ERR_INVALID_PARAM );
    STL_TRY_THROW( aRecvWindowSize > 100, RAMP_ERR_INVALID_PARAM );

    if( gIbCallback.mIbvPollCq == NULL )
    {
        STL_TRY( stnIbInitCallback( aErrorStack ) == STL_SUCCESS );
    }

    STL_TRY( stlAllocHeap( (void**)&sRes, 
                           STL_SIZEOF(stnRcResource), 
                           aErrorStack ) == STL_SUCCESS );

    stlMemset( sRes, 0x00, STL_SIZEOF(stnRcResource) ); 

    /* window 크기, 개수 설정 */
    sRes->mSendWindowSize = aSendWindowSize;
    sRes->mSendWindowsCount = aSendWindowsCount;
    sRes->mRecvWindowSize = aRecvWindowSize;
    sRes->mRecvWindowsCount = aRecvWindowsCount;

    /* default timeout(100 ms), busywait time(0ms) 설정 */
    STL_TRY( stnRcSetSendTimeout( sRes, 
                                  STL_SET_MSEC_TIME( 100 ),
                                  STL_SET_MSEC_TIME( 0 ),
                                  aErrorStack ) == STL_SUCCESS );

    STL_TRY( stnRcSetRecvTimeout( sRes, 
                                  STL_SET_MSEC_TIME( 100 ),
                                  STL_SET_MSEC_TIME( 0 ),
                                  aErrorStack ) == STL_SUCCESS );

    sChannel = gIbCallback.mCmCreateEventChannel();
    STL_TRY_THROW( sChannel != NULL, RAMP_ERR_RDMA_CREATE_CHANNEL );

    STL_TRY_THROW( gIbCallback.mCmCreateId( sChannel, 
                                            &sRes->mCmId, 
                                            NULL, 
                                            RDMA_PS_TCP ) == STL_SUCCESS, 
                   RAMP_ERR_CREATE_ID );
    STL_ASSERT( sRes->mCmId != NULL );

    STL_TRY( stlSetSockAddr( &sBindAddr, 
                             AF_INET, 
                             aBindAddr, 
                             0, 
                             NULL, 
                             aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( gIbCallback.mCmBindAddr( sRes->mCmId, &sBindAddr ) == STL_SUCCESS, 
                   RAMP_ERR_RDMA_BIND );

    STL_TRY( stlSetSockAddr( &sTargetAddr, 
                             AF_INET, 
                             aTargetAddr, 
                             aTargetPort, 
                             NULL, 
                             aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( gIbCallback.mCmResolveAddr( sRes->mCmId, 
                                               NULL, 
                                               &sTargetAddr, 
                                               STN_IB_TIMEOUT ) == STL_SUCCESS, 
                   RAMP_ERR_RDMA_RESOLVE_ADDR );

    STL_TRY_THROW( gIbCallback.mCmGetCmEvent( sRes->mCmId->channel, &sEvent ) == STL_SUCCESS, 
                   RAMP_ERR_RDMA_GET_CM_EVENT );
    STL_TRY_THROW( sEvent->event == RDMA_CM_EVENT_ADDR_RESOLVED, RAMP_ERR_EVENT_FOR_RESOLVE_ADDR );

    STL_TRY_THROW( gIbCallback.mCmAckCmEvent( sEvent ) == STL_SUCCESS, RAMP_ERR_RDMA_ACK_EVENT );
    sEvent = NULL;

    STL_TRY_THROW( gIbCallback.mCmResolveRoute( sRes->mCmId, STN_IB_TIMEOUT ) == STL_SUCCESS, 
                   RAMP_ERR_RDMA_RESOLVE_ROUTE );

    STL_TRY_THROW( gIbCallback.mCmGetCmEvent( sRes->mCmId->channel, &sEvent ) == STL_SUCCESS, 
                   RAMP_ERR_RDMA_GET_CM_EVENT );
    STL_TRY_THROW( sEvent->event == RDMA_CM_EVENT_ROUTE_RESOLVED, RAMP_ERR_EVENT_FOR_ROUTE );

    STL_TRY_THROW( gIbCallback.mCmAckCmEvent( sEvent ) == STL_SUCCESS, RAMP_ERR_RDMA_ACK_EVENT );
    sEvent = NULL;

    /* create qp    */
    stlMemset(&sQpAttr, 0x00, STL_SIZEOF(sQpAttr)); 
    sQpAttr.qp_context = NULL;
    sQpAttr.sq_sig_all = 0;
                      
    sQpAttr.cap.max_send_wr = aSendWindowsCount;
    sQpAttr.cap.max_recv_wr = aRecvWindowsCount;
    sQpAttr.cap.max_send_sge = 1;
    sQpAttr.cap.max_recv_sge = 1;
    /** latency는 INLINE이 좋고 throughput은 INLINE 사용 안하는것이 좋음.
     *  sQpAttr.cap.max_inline_data = 100;
     */
    sQpAttr.qp_type = IBV_QPT_RC;

    STL_TRY_THROW( gIbCallback.mCmCreateQp( sRes->mCmId, NULL, &sQpAttr ) == STL_SUCCESS, 
                   RAMP_ERR_RDMA_CREATE_QP );

    sRes->mMaxInlineSize = sQpAttr.cap.max_inline_data;

    /* mr for send */
    STL_TRY( stlAllocHeap( (void**)&sWindow, 
                           aSendWindowsCount * aSendWindowSize, 
                           aErrorStack ) == STL_SUCCESS );

    sRes->mSendMr = gIbCallback.mIbvRegMr( sRes->mCmId->pd,
                                           sWindow,
                                           aSendWindowsCount * aSendWindowSize,
                                           IBV_ACCESS_LOCAL_WRITE );
    STL_TRY_THROW( sRes->mSendMr != NULL, RAMP_ERR_RDMA_REG_MR );
    sWindow = NULL;

    /* mr for recv */
    STL_TRY( stlAllocHeap( (void**)&sWindow, 
                           aRecvWindowsCount * aRecvWindowSize, 
                           aErrorStack ) == STL_SUCCESS );

    sRes->mRecvMr = gIbCallback.mIbvRegMr( sRes->mCmId->pd,
                                           sWindow,
                                           aRecvWindowsCount * aRecvWindowSize,
                                           IBV_ACCESS_LOCAL_WRITE );
    STL_TRY_THROW( sRes->mRecvMr != NULL, RAMP_ERR_RDMA_REG_MR );
    sWindow = NULL;

    for( i = 0; i < aRecvWindowsCount; i++ ) 
    {
        STL_TRY_THROW( gIbCallback.mCmPostRecv( sRes->mCmId,
                                                sRes->mRecvMr->addr + ( i * aRecvWindowSize ),
                                                sRes->mRecvMr->addr + ( i * aRecvWindowSize ) ,
                                                aRecvWindowSize,
                                                sRes->mRecvMr ) == STL_SUCCESS, 
                       RAMP_ERR_RDMA_POST_RECV );
    }

    stlMemset( &sConnParam, 0x00, STL_SIZEOF(sConnParam) ); 
    sConnParam.responder_resources = 1;
    sConnParam.initiator_depth = 1;
    sConnParam.retry_count = 7;
    /** 이 값을 작게 하면 buffer 부족시 바로
     *  IBV_WC_RNR_RETRY_EXC_ERR error 발생함
     */
    sConnParam.rnr_retry_count = 7;

    STL_TRY_THROW( gIbCallback.mCmConnect( sRes->mCmId, &sConnParam ) == STL_SUCCESS, 
                   RAMP_ERR_RDMA_CONNECT );

    STL_TRY_THROW( gIbCallback.mCmGetCmEvent( sRes->mCmId->channel, &sEvent ) == STL_SUCCESS, 
                   RAMP_ERR_RDMA_GET_CM_EVENT );
    STL_TRY_THROW( sEvent->event == RDMA_CM_EVENT_ESTABLISHED, RAMP_ERR_EVENT_FOR_CONNECT );

    STL_TRY_THROW( gIbCallback.mCmAckCmEvent( sEvent ) == STL_SUCCESS, RAMP_ERR_RDMA_ACK_EVENT );
    sEvent = NULL;

    /* windows size 상대방과 확인 */
    sBuff[0] = stlToNetLong( sRes->mSendWindowSize );
    sBuff[1] = stlToNetLong( sRes->mRecvWindowSize );
    sLen = STL_SIZEOF(sBuff);
    
    STL_TRY( stnRcSend( (stlRcHandle *)sRes, 
                        (stlChar *)&sBuff, 
                        &sLen, 
                        aErrorStack ) == STL_SUCCESS );
    STL_TRY( sLen == STL_SIZEOF(sBuff) );

    sLen = STL_SIZEOF(sBuff);
    STL_TRY( stnRcRecv( (stlRcHandle *)sRes, 
                        (stlChar *)&sBuff, 
                        &sLen, 
                        aErrorStack ) == STL_SUCCESS );
    STL_TRY( sLen == STL_SIZEOF(sBuff) );

    sBuff[0] = stlToHostLong( sBuff[0] );
    sBuff[1] = stlToHostLong( sBuff[1] );

    /* 상대방의 recv window size와 내 send window size 비교 */
    STL_TRY_THROW( sBuff[1] == sRes->mSendWindowSize, RAMP_ERR_WRONG_WINDOW_SIZE );
     
    /* 상대방의 send window size와 내 recv window size 비교 */
    STL_TRY_THROW( sBuff[0] == sRes->mRecvWindowSize, RAMP_ERR_WRONG_WINDOW_SIZE );

    *aHandle = sRes;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_PARAM )
    {
        stlSnprintf( sMsg, 
                     128, 
                     "rdma invalid param: aSendWindowSize[%u], aRecvWindowSize[%u]", 
                     aSendWindowSize,
                     aRecvWindowSize );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INVALID_ARGUMENT,
                      sMsg,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_RDMA_CREATE_CHANNEL )
    {
        stlSnprintf( sMsg, 128, "rdma_create_event_channel returns errno [%d]", errno );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_IBV_VERBS_FAILURE,
                      sMsg,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_CREATE_ID )
    {
        stlSnprintf( sMsg, 128, "rdma_create_id returns cmid[%p], errno [%d]", sRes->mCmId, errno );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_IBV_VERBS_FAILURE,
                      sMsg,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_RDMA_BIND )
    {
        stlSnprintf( sMsg, 128, "rdma_bind_addr returns errno[%d]", errno );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_IBV_VERBS_FAILURE,
                      sMsg,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_RDMA_GET_CM_EVENT )
    {
        stlSnprintf( sMsg, 128, "rdma_get_cm_event returns errno [%d]", errno );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_IBV_VERBS_FAILURE,
                      sMsg,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_RDMA_CONNECT )
    {
        stlSnprintf( sMsg, 128, "rdma_connect returns errno [%d]", errno );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_IBV_VERBS_FAILURE,
                      sMsg,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_RDMA_ACK_EVENT )
    {
        stlSnprintf( sMsg, 128, "rdma_ack_cm_event returns errno [%d]", errno );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_IBV_VERBS_FAILURE,
                      sMsg,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_EVENT_FOR_CONNECT )
    {
        stlSnprintf( sMsg, 
                     128, 
                     "rdma event: expect RDMA_CM_EVENT_ESTABLISHED, received %s(%d)", 
                     gIbCallback.mCmEventStr( sEvent->event ),
                     sEvent->event );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_CONNECT,
                      sMsg,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_RDMA_CREATE_QP )
    {
        stlSnprintf( sMsg, 128, "rdma_create_qp returns errno [%d]", errno );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_IBV_VERBS_FAILURE,
                      sMsg,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_RDMA_RESOLVE_ADDR )
    {
        stlSnprintf( sMsg, 128, "rdma_resolve_addr returns errno [%d]", errno );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_IBV_VERBS_FAILURE,
                      sMsg,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_EVENT_FOR_RESOLVE_ADDR )
    {
        stlSnprintf( sMsg, 
                     128, 
                     "rdma event: expect RDMA_CM_EVENT_ADDR_RESOLVED, received %s(%d)", 
                     gIbCallback.mCmEventStr( sEvent->event ),
                     sEvent->event );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_IBV_VERBS_FAILURE,
                      sMsg,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_RDMA_RESOLVE_ROUTE )
    {
        stlSnprintf( sMsg, 128, "rdma_resolve_route returns errno [%d]", errno );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_IBV_VERBS_FAILURE,
                      sMsg,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_EVENT_FOR_ROUTE )
    {
        stlSnprintf( sMsg, 
                     128, 
                     "rdma event: expect RDMA_CM_EVENT_ROUTE_RESOLVED, received %s(%d)", 
                     gIbCallback.mCmEventStr( sEvent->event ),
                     sEvent->event );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_IBV_VERBS_FAILURE,
                      sMsg,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_RDMA_POST_RECV )
    {
        stlSnprintf( sMsg, 128, "rdma_post_recv returns errno [%d]", errno );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_IBV_VERBS_FAILURE,
                      sMsg,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_RDMA_REG_MR )
    {
        stlSnprintf( sMsg, 128, "ibv_reg_mr returns errno [%d]", errno );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_IBV_VERBS_FAILURE,
                      sMsg,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_WRONG_WINDOW_SIZE )
    {
        stlSnprintf( sMsg, 
                     128, 
                     "rdma wrong window size: My[%u]->Peer[%u], Peer[%u]->My[%u]", 
                     sRes->mSendWindowSize,
                     sBuff[1],
                     sBuff[0],
                     sRes->mRecvWindowSize );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_MISMATCH,
                      sMsg,
                      aErrorStack );
    }

    STL_FINISH;

    /* ack안된 event있으면 ack한다 */
    if( sEvent != NULL )
    {
        (void)gIbCallback.mCmAckCmEvent( sEvent );
    }
    if( sWindow != NULL )
    {
        stlFreeHeap( sWindow );
    }

    if( sRes != NULL )
    {
        if( sRes->mSendMr != NULL )
        {
            stlFreeHeap( sRes->mSendMr->addr );
            (void)gIbCallback.mIbvDeregMr( sRes->mSendMr );
        }

        if( sRes->mRecvMr != NULL )
        {
            stlFreeHeap( sRes->mRecvMr->addr );
            (void)gIbCallback.mIbvDeregMr( sRes->mRecvMr );
        }

        if( sRes->mCmId != NULL )
        {
            if( sRes->mCmId->qp != NULL )
            {
                gIbCallback.mCmDestroyQp( sRes->mCmId );
            }
            (void)gIbCallback.mCmDestroyId( sRes->mCmId );
        }

        stlFreeHeap( sRes );
    }

    if( sChannel != NULL )
    {
        gIbCallback.mCmDestroyEventChannel( sChannel );
    }

    return STL_FAILURE;
}

stlStatus stnRcSetSendTimeout( stlRcHandle     aHandle,
                               stlInterval     aTimeoutUsec,
                               stlInterval     aBusyWaitUsec,
                               stlErrorStack * aErrorStack ) 
{
    stnRcResource             * sRes = NULL;

    sRes = (stnRcResource *)aHandle;

    /* busywait 시간이 timeout보다 길면 busywait을 timeout 시간으로 변경한다. */
    if( aBusyWaitUsec > aTimeoutUsec )
    {
        aBusyWaitUsec = aTimeoutUsec;
    }

    sRes->mSendBusyWaitUsec = aBusyWaitUsec;
    sRes->mSendTimeoutUsec = aTimeoutUsec;

    return STL_SUCCESS;
}

stlStatus stnRcSetRecvTimeout( stlRcHandle     aHandle,
                               stlInterval     aTimeoutUsec,
                               stlInterval     aBusyWaitUsec,
                               stlErrorStack * aErrorStack ) 
{
    stnRcResource             * sRes = NULL;

    sRes = (stnRcResource *)aHandle;

    /* busywait 시간이 timeout보다 길면 busywait을 timeout 시간으로 변경한다. */
    if( aBusyWaitUsec > aTimeoutUsec )
    {
        aBusyWaitUsec = aTimeoutUsec;
    }

    sRes->mRecvBusyWaitUsec = aBusyWaitUsec;
    sRes->mRecvTimeoutUsec = aTimeoutUsec;

    return STL_SUCCESS;
}

stlStatus stnRcPollCq( stlRcHandle   * aHandle,
                       struct ibv_cq * aCq,
                       stlUInt32     * aCqCount,
                       struct ibv_wc * aWc,
                       stlErrorStack * aErrorStack )
{
    stlChar                     sMsg[128];
    stlUInt32                   sCqCount = 0;

    sCqCount = gIbCallback.mIbvPollCq( aCq, 1, aWc);
    STL_TRY_THROW( sCqCount >= 0, RAMP_ERR_POLL_CQ );
    if( sCqCount > 0 )
    {
        STL_TRY_THROW( aWc->status == IBV_WC_SUCCESS, RAMP_ERR_POLL_CQ_STATUS );
    }

    *aCqCount = sCqCount;

    return STL_SUCCESS; 

    STL_CATCH( RAMP_ERR_POLL_CQ )
    {
        stlSnprintf( sMsg, 128, "ibv_poll_cq returns errno [%d]", errno );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_IBV_VERBS_FAILURE,
                      sMsg,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_POLL_CQ_STATUS )
    {
        stlSnprintf( sMsg, 
                     128, 
                     "ibv_poll_cq returns status [%s]%d", 
                     gIbCallback.mIbvWcStatusStr( aWc->status ),
                     aWc->status );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_IBV_VERBS_FAILURE,
                      sMsg,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus stnRcGainWindow( stlRcHandle             * aHandle,
                           struct ibv_cq           * aCq,
                           struct ibv_comp_channel * aCqChannel,
                           struct ibv_wc           * aWc,
                           stlUInt32               * aCqCount,
                           stlUInt32                 aTimeoutUsec,
                           stlUInt32                 aBusyWaitUsec,
                           stlErrorStack           * aErrorStack )
{
    stlChar                     sMsg[128];
    stlTime                     sTimeStart = 0;
    stlTime                     sTimeEnd = 0;
    stlPollFd                   sFds[1];
    stlInt32                    sSigFdNum = 0;
    struct ibv_cq             * sEventCq;
    void                      * sUser = NULL;

    /* stlNow가 혹시 동일 값이 나올까봐 -1 했음. */
    sTimeStart = stlNow() - 1;

    /* for BusyWait */
    /* event라도 한번은 PollCq하도록 do_while 사용 했음. */
    do {
        STL_TRY( stnRcPollCq( aHandle, aCq, aCqCount, aWc, aErrorStack ) == STL_SUCCESS );

        if( *aCqCount > 0 )
        {
            break;
        }

        sTimeEnd = stlNow();
    } while( sTimeEnd - sTimeStart < aBusyWaitUsec );

    if( *aCqCount == 0 )
    {
        /* timeout check */
        STL_TRY_THROW( aBusyWaitUsec < aTimeoutUsec, RAMP_ERR_TIMEDOUT );

        /** mIbvReqNotifyCq 호출후 
         *  stlPoll에서 timeout걸려서 
         *  mIbvGetCqEvent 호출 안하고 빠져도 문제 없는것 같음.
         *  test 상 문제 없음. 
         */
        /* for event : sRes->mSendTimeoutUsec - sRes->mSendBusyWaitUsec 시간동안 */
        STL_TRY_THROW( gIbCallback.mIbvReqNotifyCq( aCq, 0 ) == STL_SUCCESS, 
                       RAMP_ERR_IBV_REQ_NOTIFY_CQ );

        sFds[0].mSocketHandle = aCqChannel->fd;
        sFds[0].mReqEvents = STL_POLLIN;    /* ib에서 STL_POLLERR 설정 안됨 */
        sFds[0].mRetEvents = 0;

        STL_TRY( stlPoll( sFds, 
                          1, 
                          &sSigFdNum, 
                          aTimeoutUsec - aBusyWaitUsec, 
                          aErrorStack ) == STL_SUCCESS );

        /* poll에서 event감지가 되었으면 ibv_get_cq_event한다 */
        STL_TRY_THROW( gIbCallback.mIbvGetCqEvent( aCqChannel,
                                                   &sEventCq,
                                                   &sUser ) == STL_SUCCESS, 
                       RAMP_ERR_IBV_GET_CQ_EVENT );

        gIbCallback.mIbvAckCqEvents( aCq, 1 );

        STL_TRY( stnRcPollCq( aHandle, aCq, aCqCount, aWc, aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        /* *aCqCount가 0보다 큰 경우 임으로 do nothing */
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_TIMEDOUT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_TIMEDOUT,
                      NULL,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_IBV_REQ_NOTIFY_CQ )
    {
        stlSnprintf( sMsg, 128, "ibv_req_notify_cq returns errno [%d]", errno );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_IBV_VERBS_FAILURE,
                      sMsg,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_IBV_GET_CQ_EVENT )
    {
        stlSnprintf( sMsg, 128, "ibv_get_cq_event returns errno [%d]", errno );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_IBV_VERBS_FAILURE,
                      sMsg,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stnRcSend( stlRcHandle   * aHandle,
                     const stlChar * aBuf,
                     stlSize       * aLen,
                     stlErrorStack * aErrorStack )
{
    stlInt32                    sRc = 0;
    stlChar                     sMsg[128];
    stnRcResource             * sRes = NULL;
    stlChar                   * sWindow = NULL;
    stlSize                     sToBeSendLen = 0;
    stlInt32                    sFlags = 0;
    stlSize                     sSentLen = 0;
    stlUInt32                   sCqCount = 0;
    struct ibv_wc               sWc;

    sRes = (stnRcResource *)aHandle;

    /* data를 다 보낼때까지 loop */
    while( sSentLen < *aLen )
    {

        /* window를 다 사용중인 경우 ibv_poll_cq를 해서 window확보 한다  */
        if( sRes->mSendWindowUsedCount == sRes->mSendWindowsCount )
        {
            sRc = stnRcGainWindow( aHandle,
                                   sRes->mCmId->send_cq,
                                   sRes->mCmId->send_cq_channel,
                                   &sWc,
                                   &sCqCount,
                                   sRes->mSendTimeoutUsec,
                                   sRes->mSendBusyWaitUsec,
                                   aErrorStack );
            if( sRc == STL_FAILURE )
            {
                if( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_TIMEDOUT )
                {
                    /* timeout이고 보낸 data가 있으면 정상 처리 */
                    /* timeout이고 보낸 data가 없으면 TIMEDOUT 처리 */
                    STL_TRY( sSentLen > 0 );
                    (void)stlPopError( aErrorStack );

                    break;
                }
                else
                {
                    /* timeout이 아니면 error처리 */
                    STL_TRY( STL_FALSE );
                }
            }

            STL_ASSERT ( sRes->mSendWindowUsedCount >= sCqCount );
            sRes->mSendWindowUsedCount -= sCqCount;
        }

        /* window position계산  */
        sWindow = sRes->mSendMr->addr + ( sRes->mSendWindowPos * sRes->mSendWindowSize );
        sRes->mSendWindowPos++;
        sRes->mSendWindowUsedCount++;
        if( sRes->mSendWindowPos == sRes->mSendWindowsCount )
        {
            sRes->mSendWindowPos = 0;
        }

        /* 한번에 보낼 length 계산 */
        if( *aLen - sSentLen > sRes->mSendWindowSize )
        {
            sToBeSendLen = sRes->mSendWindowSize;
        }
        else
        {
            sToBeSendLen = *aLen - sSentLen;
        }
        
        STL_ASSERT ( sWindow >= (stlChar *)sRes->mSendMr->addr );
        STL_ASSERT ( sWindow < (stlChar *)sRes->mSendMr->addr + ( sRes->mSendWindowsCount * sRes->mSendWindowSize ) );
        stlMemcpy( sWindow, aBuf + sSentLen, sToBeSendLen );

        sFlags = IBV_SEND_SIGNALED;
        /** IBV_SEND_INLINE 사용 안함.
         *  if(sToBeSendLen <= sRes->mMaxInlineSize)
         *  {
         *      sFlags |= IBV_SEND_INLINE;
         *  }
         */

        STL_TRY_THROW( gIbCallback.mCmPostSend( sRes->mCmId, 
                                                sWindow,
                                                sWindow,
                                                sToBeSendLen, 
                                                sRes->mSendMr, 
                                                sFlags ) == STL_SUCCESS, 
                       RAMP_ERR_RDMA_POST_SEND );
        sSentLen += sToBeSendLen;

        /* 쌓여 있는 cq를 poll한다. */
        while( 1 )
        {
            STL_TRY( stnRcPollCq( aHandle, 
                                  sRes->mCmId->send_cq, 
                                  &sCqCount, 
                                  &sWc, 
                                  aErrorStack ) == STL_SUCCESS );

            if( sCqCount == 0 )
            {
                /* cq가 없으면 break */
                break;
            }

            /* poll cq를 받으면 다시 cq 받기를 시도한다.  */
            sRes->mSendWindowUsedCount -= sCqCount;
        }
    }

    *aLen = sSentLen;

    return STL_SUCCESS; 

    STL_CATCH( RAMP_ERR_RDMA_POST_SEND )
    {
        stlSnprintf( sMsg, 128, "rdma_post_send returns errno [%d]", errno );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_IBV_VERBS_FAILURE,
                      sMsg,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus stnRcRecv( stlRcHandle     aHandle,
                     stlChar       * aBuf,
                     stlSize       * aLen,
                     stlErrorStack * aErrorStack)
{
    stlInt32                    sRc = 0;
    stnRcResource             * sRes = NULL;
    stlChar                     sMsg[128];
    stlSize                     sReceivedLen = 0;
    stlUInt32                   sCqCount = 0;
    struct ibv_wc               sWc;

    sRes = (stnRcResource *)aHandle;

    /* data를 다 받을때까지 loop */
    while( sReceivedLen < *aLen )
    {
        sRc = stnRcGainWindow( aHandle,
                               sRes->mCmId->recv_cq,
                               sRes->mCmId->recv_cq_channel,
                               &sWc,
                               &sCqCount,
                               sRes->mRecvTimeoutUsec,
                               sRes->mRecvBusyWaitUsec,
                               aErrorStack );
        if( sRc == STL_FAILURE )
        {
            if( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_TIMEDOUT )
            {
                /* timeout이고 받은 data가 있으면 정상 처리 */
                STL_TRY( sReceivedLen > 0 );
                (void)stlPopError( aErrorStack );

                break;
            }
            else
            {
                /* timeout이 아니면 error처리 */
                STL_TRY( STL_FALSE );
            }
        }

        /* recv한 data의 length보다 받는 buffer length가 작으면 받을수 없음. */
        STL_TRY_THROW( *aLen >= sReceivedLen + sWc.byte_len, RAMP_ERR_BUFFER_SIZE_WRONG );

        stlMemcpy( aBuf + sReceivedLen, (void *)sWc.wr_id, sWc.byte_len );
        sReceivedLen += sWc.byte_len;

        /* window에 다시 받도록 준비 시킨다. */
        STL_TRY_THROW( gIbCallback.mCmPostRecv( sRes->mCmId, 
                                                (void *)sWc.wr_id,
                                                (void *)sWc.wr_id,
                                                sRes->mRecvWindowSize, 
                                                sRes->mRecvMr ) == STL_SUCCESS, 
                       RAMP_ERR_RDMA_POST_RECV );
    }

    *aLen = sReceivedLen;

    return STL_SUCCESS; 

    STL_CATCH( RAMP_ERR_BUFFER_SIZE_WRONG )
    {
        stlSnprintf( sMsg, 
                     128, 
                     "rdma recv size wrong: param len[%ld] >= received[%ld] + receive[%ld]", 
                     *aLen, 
                     sReceivedLen, 
                     sWc.byte_len );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_IBV_VERBS_FAILURE,
                      sMsg,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_RDMA_POST_RECV )
    {
        stlSnprintf( sMsg, 128, "rdma_post_recv returns errno [%d]", errno );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_IBV_VERBS_FAILURE,
                      sMsg,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stnRcClose( stlRcHandle   * aHandle,
                      stlErrorStack * aErrorStack ) 
{
    stnRcResource             * sRes = NULL;
    struct rdma_event_channel * sChannel = NULL;
    stlChar                     sMsg[128];

    sRes = (stnRcResource *)aHandle;

    stlFreeHeap( sRes->mSendMr->addr );
    sRes->mSendMr->addr = NULL;
    STL_TRY_THROW( gIbCallback.mIbvDeregMr( sRes->mSendMr ) == STL_SUCCESS, 
                   RAMP_ERR_IBV_DEREG_MR );
    sRes->mSendMr = NULL;

    stlFreeHeap( sRes->mRecvMr->addr );
    sRes->mRecvMr->addr = NULL;
    STL_TRY_THROW( gIbCallback.mIbvDeregMr( sRes->mRecvMr ) == STL_SUCCESS, 
                   RAMP_ERR_IBV_DEREG_MR );
    sRes->mRecvMr = NULL;

    sChannel = sRes->mCmId->channel;

    gIbCallback.mCmDestroyQp( sRes->mCmId );    /* return void */

    STL_TRY_THROW( gIbCallback.mCmDestroyId( sRes->mCmId ) == STL_SUCCESS, 
                   RAMP_ERR_RDMA_DESTROY_ID );
    sRes->mCmId = NULL;

    gIbCallback.mCmDestroyEventChannel( sChannel ); /* return void */

    stlFreeHeap( sRes );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_IBV_DEREG_MR )
    {
        stlSnprintf( sMsg, 128, "ibv_dereg_mr returns errno [%d]", errno );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_IBV_VERBS_FAILURE,
                      sMsg,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_RDMA_DESTROY_ID )
    {
        stlSnprintf( sMsg, 128, "rdma_destroy_event_channel returns errno [%d]", errno );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_IBV_VERBS_FAILURE,
                      sMsg,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}
