#ifndef  __STN_IB_DEF_H__
#define  __STN_IB_DEF_H__

#include <stlTypes.h>


/* structure of system resources */ 
typedef struct stnRcResource 
{
    /* rdma resource info */
    struct rdma_cm_id * mCmId;

    struct ibv_mr     * mSendMr;
    struct ibv_mr     * mRecvMr;

    stlUInt32           mMaxInlineSize;

    /* for send */
    stlUInt32           mSendWindowSize;
    stlUInt32           mSendWindowsCount;
    stlInterval         mSendBusyWaitUsec;
    stlInterval         mSendTimeoutUsec;
    stlUInt32           mSendWindowPos;
    stlUInt32           mSendWindowUsedCount;

    /* for receive */
    stlUInt32           mRecvWindowSize;
    stlUInt32           mRecvWindowsCount;
    stlInterval         mRecvBusyWaitUsec;
    stlInterval         mRecvTimeoutUsec;

} stnRcResource; 

typedef stlInt32 (*stnFuncPollCq)( struct ibv_cq * aCq,
                                   stlInt32        aNumEntries,
                                   struct ibv_wc * aWc );

typedef stlInt32 (*stnFuncPostSend)( struct ibv_qp       * aQp,
                                     struct ibv_send_wr  * aWr,
                                     struct ibv_send_wr ** aBadWr );

typedef stlInt32 (*stnFuncPostRecv)( struct ibv_qp       * aQp,
                                     struct ibv_recv_wr  * aWr,
                                     struct ibv_recv_wr ** aBadWr );

typedef struct ibv_device ** (*stnFuncGetDeviceList)( stlInt32 * aNumDevices );

typedef void (*stnFuncFreeDeviceList)( struct ibv_device ** aList );

typedef const stlChar *(*stnFuncGetDeviceName)( struct ibv_device * aDevice );

typedef struct ibv_context *(*stnFuncOpenDevice)( struct ibv_device * aDevice );

typedef stlInt32 (*stnFuncCloseDevice)( struct ibv_context * aContext );

typedef stlInt32 (*stnFuncQueryPort)( struct ibv_context   * aContext,
                                      stlUInt8               aPortNum,
                                      struct ibv_port_attr * aPortAttr );

typedef struct ibv_pd *(*stnFuncAllocPd)( struct ibv_context * aContext );

typedef stlInt32 (*stnFuncDeallocPd)( struct ibv_pd * aPd );

typedef struct ibv_cq *(*stnFuncCreateCq)( struct ibv_context      * aContext,
                                           stlInt32                  aCqe,
                                           void                    * aCqContext,
                                           struct ibv_comp_channel * aChannel,
                                           stlInt32                  aCompVector);

typedef stlInt32 (*stnFuncDestroyCq)( struct ibv_cq * aCq );

typedef struct ibv_mr *(*stnFuncRegMr)( struct ibv_pd * aPd,
                                        void          * aAddr,
                                        stlSize         aLength, 
                                        stlInt32        aAccess );

typedef stlInt32 (*stnFuncDeregMr)( struct ibv_mr * aMr );

typedef struct ibv_qp *(*stnFuncCreateQp)( struct ibv_pd           * aPd,
                                           struct ibv_qp_init_attr * aQpInitAttr );

typedef stlInt32 (*stnFuncDestroyQp)( struct ibv_qp * aQp );

typedef stlInt32 (*stnFuncModifyQp)( struct ibv_qp      * aQp,
                                     struct ibv_qp_attr * aAttr,
                                     stlInt32             aAttrMask );
typedef stlInt32 (*stnFuncQueryGid)( struct ibv_context * aContext,
                                     stlUInt8             aPortNum,
                                     stlInt32             aIndex,
                                     union ibv_gid      * aGid );

typedef stlInt32 (*stnFuncIbvReqNotifyCq)( struct ibv_cq  * aCq,  
                                           stlInt32         aSolicitedOnly );

typedef stlInt32 (*stnFuncIbvGetCqEvent)( struct ibv_comp_channel  * aChannel,
                                          struct ibv_cq           ** aCq,
                                          void                    ** aCqContext );

typedef void (*stnFuncIbvAckCqEvents)( struct ibv_cq  * aCq, 
                                       stlUInt32        aNEvents );

typedef const stlChar * (*stnFuncIbvWcStatusStr)( enum ibv_wc_status aStatus );

typedef struct rdma_event_channel *(*stnFuncCmCreateEventChannel)( );

typedef void (*stnFuncCmDestroyEventChannel)( struct rdma_event_channel * aChannel );

typedef stlInt32 (*stnFuncCmCreateId)( struct rdma_event_channel   * aChannel,
                                       struct rdma_cm_id          ** aCmId, 
                                       void                        * aContext,
                                       enum rdma_port_space          aPs );

typedef stlInt32 (*stnFuncCmDestroyId)( struct rdma_cm_id      * aCmId );

typedef stlInt32 (*stnFuncCmBindAddr)( struct rdma_cm_id       * aCmId, 
                                       stlSockAddr             * aAddr );

typedef stlInt32 (*stnFuncCmResolveAddr)( struct rdma_cm_id    * aCmId, 
                                          stlSockAddr          * aSrcAddr,
                                          stlSockAddr          * aDstAddr, 
                                          stlInt32               aTimeoutMs );

typedef stlInt32 (*stnFuncCmResolveRoute)( struct rdma_cm_id   * aCmId, 
                                           stlInt32              aTimeoutMs );

typedef stlInt32 (*stnFuncCmCreateQp)( struct rdma_cm_id       * aCmId, 
                                       struct ibv_pd           * aPd,
                                       struct ibv_qp_init_attr * aQpInitAttr );

typedef void (*stnFuncCmDestroyQp)( struct rdma_cm_id          * aCmId );

typedef stlInt32 (*stnFuncCmConnect)( struct rdma_cm_id         * aCmId, 
                                      struct rdma_conn_param    * aConnParam );

typedef stlInt32 (*stnFuncCmListen)( struct rdma_cm_id          * aCmId, 
                                     stlInt32                     aBackLog );

typedef stlInt32 (*stnFuncCmAccept)( struct rdma_cm_id          * aCmId, 
                                     struct rdma_conn_param     * aConnParam );

typedef stlInt32 (*stnFuncCmReject)( struct rdma_cm_id          * aCmId, 
                                     const void                 * aPrivateData,
                                     stlUInt8                     aPrivateDataLen );

typedef stlInt32 (*stnFuncCmNotify)( struct rdma_cm_id          * aCmId, 
                                     enum ibv_event_type          aEvent );

typedef stlInt32 (*stnFuncCmDisconnect)( struct rdma_cm_id      * aCmId );

typedef stlInt32 (*stnFuncCmJoinMulticast)( struct rdma_cm_id  * aCmId, 
                                            stlSockAddr        * aAddr,
                                            void               * aContext );

typedef stlInt32 (*stnFuncCmLeaveMulticast)( struct rdma_cm_id * aCmId, 
                                             stlSockAddr       * aAddr );

typedef stlInt32 (*stnFuncCmGetCmEvent)( struct rdma_event_channel      * aChannel,
                                          struct rdma_cm_event         ** aEvent );

typedef stlInt32 (*stnFuncCmAckCmEvent)( struct rdma_cm_event * aEvent );

typedef struct ibv_context **(*stnFuncCmGetDevices)( stlInt32 * aNumDevices );

typedef void (*stnFuncCmFreeDevices)( struct ibv_context ** aList );

typedef const char *(*stnFuncCmEventStr)( enum rdma_cm_event_type aEvent );

typedef stlInt32 (*stnFuncCmPostSend)( struct rdma_cm_id     * aCmId, 
                                      void                  * aContext, 
                                      void                  * aAddr, 
                                      stlSize                 aLength, 
                                      struct ibv_mr         * aMr,
                                      stlInt32                aFlags );

typedef stlInt32 (*stnFuncCmPostRecv)( struct rdma_cm_id     * aCmId, 
                                      void                  * aContext, 
                                      void                  * aAddr, 
                                      stlSize                 aLength, 
                                      struct ibv_mr         * aMr );



typedef struct stnIbCallback
{
    stnFuncPollCq                   mIbvPollCq;
    stnFuncPostSend                 mIbvPostSend;
    stnFuncPostRecv                 mIbvPostRecv;
    stnFuncGetDeviceList            mIbvGetDeviceList;
    stnFuncFreeDeviceList           mIbvFreeDeviceList;
    stnFuncGetDeviceName            mIbvGetDeviceName;
    stnFuncOpenDevice               mIbvOpenDevice;
    stnFuncCloseDevice              mIbvCloseDevice;
    stnFuncQueryPort                mIbvQueryPort;
    stnFuncAllocPd                  mIbvAllocPd;
    stnFuncDeallocPd                mIbvDeallocPd;
    stnFuncCreateCq                 mIbvCreateCq;
    stnFuncDestroyCq                mIbvDestroyCq;
    stnFuncRegMr                    mIbvRegMr;
    stnFuncDeregMr                  mIbvDeregMr;
    stnFuncCreateQp                 mIbvCreateQp;
    stnFuncDestroyQp                mIbvDestroyQp;
    stnFuncModifyQp                 mIbvModifyQp;
    stnFuncQueryGid                 mIbvQueryGid;
    stnFuncIbvReqNotifyCq           mIbvReqNotifyCq;
    stnFuncIbvGetCqEvent            mIbvGetCqEvent;
    stnFuncIbvAckCqEvents           mIbvAckCqEvents;
    stnFuncIbvWcStatusStr           mIbvWcStatusStr;

    stnFuncCmCreateEventChannel     mCmCreateEventChannel;
    stnFuncCmDestroyEventChannel    mCmDestroyEventChannel;
    stnFuncCmCreateId               mCmCreateId;
    stnFuncCmDestroyId              mCmDestroyId;
    stnFuncCmBindAddr               mCmBindAddr;
    stnFuncCmResolveAddr            mCmResolveAddr;
    stnFuncCmResolveRoute           mCmResolveRoute;
    stnFuncCmCreateQp               mCmCreateQp;
    stnFuncCmDestroyQp              mCmDestroyQp;
    stnFuncCmConnect                mCmConnect;
    stnFuncCmListen                 mCmListen;
    stnFuncCmAccept                 mCmAccept;
    stnFuncCmReject                 mCmReject;
    stnFuncCmNotify                 mCmNotify;
    stnFuncCmDisconnect             mCmDisconnect;
    stnFuncCmJoinMulticast          mCmJoinMulticast;
    stnFuncCmLeaveMulticast         mCmLeaveMulticast;
    stnFuncCmGetCmEvent             mCmGetCmEvent;
    stnFuncCmAckCmEvent             mCmAckCmEvent;
    stnFuncCmGetDevices             mCmGetDevices;
    stnFuncCmFreeDevices            mCmFreeDevices;
    stnFuncCmEventStr               mCmEventStr;
    stnFuncCmPostSend               mCmPostSend;
    stnFuncCmPostRecv               mCmPostRecv;
} stnIbCallback;


#endif

