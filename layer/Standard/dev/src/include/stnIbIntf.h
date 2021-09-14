#ifndef  __STN_IB_INTF_H__
#define  __STN_IB_INTF_H__

#include <stl.h>


#define IB_LIBRARY_NAME "libstib.so"


stlInt32 stnIbvPollCq( struct ibv_cq * aCq,
                       stlInt32        aNumEntries,
                       struct ibv_wc * aWc );

stlInt32 stnIbvPostSend( struct ibv_qp       * aQp,
                         struct ibv_send_wr  * aWr,
                         struct ibv_send_wr ** aBadWr );

stlInt32 stnIbvPostRecv( struct ibv_qp       * aQp,
                         struct ibv_recv_wr  * aWr,
                         struct ibv_recv_wr ** aBadWr );

struct ibv_device ** stnIbvGetDeviceList( stlInt32 * aNumDevices );

void stnIbvFreeDeviceList( struct ibv_device ** aList );

const stlChar *stnIbvGetDeviceName( struct ibv_device * aDevice );

struct ibv_context *stnIbvOpenDevice( struct ibv_device * aDevice );

stlInt32 stnIbvCloseDevice( struct ibv_context * aContext );

stlInt32 stnIbvQueryPort( struct ibv_context   * aContext,
                          stlUInt8               aPortNum,
                          struct ibv_port_attr * aPortAttr );

struct ibv_pd *stnIbvAllocPd( struct ibv_context * aContext );

stlInt32 stnIbvDeallocPd( struct ibv_pd * aPd );

struct ibv_cq *stnIbvCreateCq( struct ibv_context      * aContext,
                               stlInt32                  aCqe,
                               void                    * aCqContext,
                               struct ibv_comp_channel * aChannel,
                               stlInt32                  aCompVector);

stlInt32 stnIbvDestroyCq( struct ibv_cq * aCq );

struct ibv_mr *stnIbvRegMr( struct ibv_pd * aPd,
                            void          * aAddr,
                            stlSize         aLength, 
                            stlInt32        aAccess );

stlInt32 stnIbvDeregMr( struct ibv_mr * aMr );

struct ibv_qp *stnIbvCreateQp( struct ibv_pd           * aPd,
                               struct ibv_qp_init_attr * aQpInitAttr );

stlInt32 stnIbvDestroyQp( struct ibv_qp * aQp );

stlInt32 stnIbvModifyQp( struct ibv_qp      * aQp,
                         struct ibv_qp_attr * aAttr,
                         stlInt32             aAttrMask );
stlInt32 stnIbvQueryGid(struct ibv_context * aContext,
                        stlUInt8             aPortNum,
                        stlInt32             aIndex,
                        union ibv_gid      * aGid );

stlInt32 stnIbvReqNotifyCq( struct ibv_cq  * aCq, 
                            stlInt32         aSolicitedOnly );

stlInt32 stnIbvGetCqEvent( struct ibv_comp_channel  * aChannel,
                           struct ibv_cq           ** aCq, 
                           void                    ** aCqContext );

void stnIbvAckCqEvents( struct ibv_cq  * aCq, 
                        stlUInt32        aNEvents );

const stlChar * stnIbvWcStatusStr( enum ibv_wc_status aStatus );


struct rdma_event_channel * stnRdmaCmCreateEventChannel();

void  stnRdmaCmDestroyEventChannel( struct rdma_event_channel * aChannel );

stlInt32  stnRdmaCmCreateId( struct rdma_event_channel   * aChannel,
                             struct rdma_cm_id          ** aCmId,
                             void                        * aContext,
                             enum rdma_port_space          aPs );

stlInt32  stnRdmaCmDestroyId( struct rdma_cm_id      * aCmId );

stlInt32  stnRdmaCmBindAddr( struct rdma_cm_id       * aCmId,
                             stlSockAddr             * aAddr );

stlInt32  stnRdmaCmResolveAddr( struct rdma_cm_id    * aCmId,
                                stlSockAddr          * aSrcAddr,
                                stlSockAddr          * aDstAddr,
                                stlInt32               aTimeoutMs );

stlInt32  stnRdmaCmResolveRoute( struct rdma_cm_id   * aCmId,
                                 stlInt32              aTimeoutMs );

stlInt32  stnRdmaCmCreateQp( struct rdma_cm_id       * aCmId,
                             struct ibv_pd           * aPd,
                             struct ibv_qp_init_attr * aQpInitAttr );

void  stnRdmaCmDestroyQp( struct rdma_cm_id          * aCmId );

stlInt32  stnRdmaCmConnect( struct rdma_cm_id         * aCmId,
                            struct rdma_conn_param    * aConnParam );

stlInt32  stnRdmaCmListen( struct rdma_cm_id          * aCmId,
                           stlInt32                     aBackLog );

stlInt32  stnRdmaCmAccept( struct rdma_cm_id          * aCmId,
                           struct rdma_conn_param     * aConnParam );

stlInt32  stnRdmaCmReject( struct rdma_cm_id          * aCmId,
                           const void                 * aPrivateData,
                           stlUInt8                     aPrivateDataLen );

stlInt32  stnRdmaCmNotify( struct rdma_cm_id          * aCmId,
                           enum ibv_event_type          aEvent );

stlInt32  stnRdmaCmDisconnect( struct rdma_cm_id      * aCmId );

stlInt32  stnRdmaCmJoinMulticast( struct rdma_cm_id  * aCmId,
                                  stlSockAddr        * aAddr,
                                  void               * aContext );

stlInt32  stnRdmaCmLeaveMulticast( struct rdma_cm_id * aCmId,
                                   stlSockAddr       * aAddr );

stlInt32  stnRdmaCmGetCmEvent( struct rdma_event_channel     * aChannel,
                               struct rdma_cm_event         ** aEvent );

stlInt32  stnRdmaCmAckCmEvent( struct rdma_cm_event * aEvent );

struct ibv_context ** stnRdmaCmGetDevices( stlInt32 * aNumDevices );


void  stnRdmaCmFreeDevices( struct ibv_context ** aList );

const char * stnRdmaCmEventStr( enum rdma_cm_event_type aEvent );

stlInt32 stnRdmaPostSend( struct rdma_cm_id     * aCmId, 
                          void                  * aContext, 
                          void                  * aAddr, 
                          stlSize                 aLength, 
                          struct ibv_mr         * aMr,
                          stlInt32                aFlags );

stlInt32 stnRdmaPostRecv( struct rdma_cm_id     * aCmId, 
                          void                  * aContext, 
                          void                  * aAddr, 
                          stlSize                 aLength, 
                          struct ibv_mr         * aMr );


#endif

