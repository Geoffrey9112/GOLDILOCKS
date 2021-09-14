/*******************************************************************************
 * stnIbIntf.c
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



stlInt32 stnIbvPollCq( struct ibv_cq * aCq,
                       stlInt32        aNumEntries,
                       struct ibv_wc * aWc )
{
    return ibv_poll_cq( aCq,
                        aNumEntries,
                        aWc );
}

stlInt32 stnIbvPostSend( struct ibv_qp       * aQp,
                         struct ibv_send_wr  * aWr,
                         struct ibv_send_wr ** aBadWr )
{
    return ibv_post_send( aQp,
                          aWr,
                          aBadWr );
}

stlInt32 stnIbvPostRecv( struct ibv_qp       * aQp,
                         struct ibv_recv_wr  * aWr,
                         struct ibv_recv_wr ** aBadWr )
{
    return ibv_post_recv( aQp,
                          aWr,
                          aBadWr );
}

struct ibv_device ** stnIbvGetDeviceList( stlInt32 * aNumDevices )
{
    return ibv_get_device_list( aNumDevices );
}

void stnIbvFreeDeviceList( struct ibv_device ** aList )
{
    return ibv_free_device_list( aList );
}

const stlChar *stnIbvGetDeviceName( struct ibv_device * aDevice )
{
    return ibv_get_device_name( aDevice );
}

struct ibv_context *stnIbvOpenDevice( struct ibv_device * aDevice )
{
    return ibv_open_device( aDevice );
}

stlInt32 stnIbvCloseDevice( struct ibv_context * aContext )
{
    return ibv_close_device( aContext );
}

stlInt32 stnIbvQueryPort( struct ibv_context   * aContext,
                          stlUInt8               aPortNum,
                          struct ibv_port_attr * aPortAttr )
{
    return ibv_query_port( aContext,
                           aPortNum,
                           aPortAttr);
}

struct ibv_pd *stnIbvAllocPd( struct ibv_context * aContext )
{
    return ibv_alloc_pd( aContext );
}

stlInt32 stnIbvDeallocPd( struct ibv_pd * aPd )
{
    return ibv_dealloc_pd( aPd );
}

struct ibv_cq *stnIbvCreateCq( struct ibv_context      * aContext,
                               stlInt32                  aCqe,
                               void                    * aCqContext,
                               struct ibv_comp_channel * aChannel,
                               stlInt32                  aCompVector)
{
    return ibv_create_cq( aContext,
                          aCqe,
                          aCqContext,
                          aChannel,
                          aCompVector );
}

stlInt32 stnIbvDestroyCq( struct ibv_cq * aCq )
{
    return ibv_destroy_cq( aCq );
}

struct ibv_mr *stnIbvRegMr( struct ibv_pd * aPd,
                            void          * aAddr,
                            stlSize         aLength, 
                            stlInt32        aAccess )
{
    return ibv_reg_mr( aPd ,
                       aAddr,
                       aLength,
                       aAccess );
}

stlInt32 stnIbvDeregMr( struct ibv_mr * aMr )
{
    return ibv_dereg_mr( aMr );
}

struct ibv_qp *stnIbvCreateQp( struct ibv_pd           * aPd,
                               struct ibv_qp_init_attr * aQpInitAttr )
{
    return ibv_create_qp( aPd,
                          aQpInitAttr );
}

stlInt32 stnIbvDestroyQp( struct ibv_qp * aQp )
{
    return ibv_destroy_qp( aQp );
}

stlInt32 stnIbvModifyQp( struct ibv_qp      * aQp,
                         struct ibv_qp_attr * aAttr,
                         stlInt32             aAttrMask )
{
    return ibv_modify_qp( aQp,
                          aAttr,
                          aAttrMask);
}

stlInt32 stnIbvQueryGid( struct ibv_context * aContext,
                         stlUInt8             aPortNum,
                         stlInt32             aIndex,
                         union ibv_gid      * aGid )
{
    return ibv_query_gid( aContext,
                          aPortNum,
                          aIndex,
                          aGid);
}

stlInt32 stnIbvReqNotifyCq( struct ibv_cq  * aCq, 
                            stlInt32         aSolicitedOnly )
{
    return ibv_req_notify_cq( aCq, aSolicitedOnly );
}

stlInt32 stnIbvGetCqEvent( struct ibv_comp_channel  * aChannel,
                           struct ibv_cq           ** aCq, 
                           void                    ** aCqContext )
{
    return ibv_get_cq_event( aChannel, aCq, aCqContext );
}

void stnIbvAckCqEvents( struct ibv_cq  * aCq, 
                        stlUInt32        aNEvents )
{
    return ibv_ack_cq_events( aCq, aNEvents );
}

const stlChar * stnIbvWcStatusStr( enum ibv_wc_status aStatus )
{
    return ibv_wc_status_str( aStatus );
}

/* rdma */
struct rdma_event_channel * stnRdmaCmCreateEventChannel()
{
    return rdma_create_event_channel();
}

void  stnRdmaCmDestroyEventChannel( struct rdma_event_channel * aChannel )
{
    rdma_destroy_event_channel( aChannel );
}

stlInt32 stnRdmaCmCreateId( struct rdma_event_channel   * aChannel,
                            struct rdma_cm_id          ** aCmId,
                            void                        * aContext,
                            enum rdma_port_space          aPs )
{
    return rdma_create_id( aChannel, aCmId, aContext, aPs );
}

stlInt32 stnRdmaCmDestroyId( struct rdma_cm_id      * aCmId )
{
    return rdma_destroy_id( aCmId );
}

stlInt32 stnRdmaCmBindAddr( struct rdma_cm_id       * aCmId,
                            stlSockAddr             * aAddr )
{
    return rdma_bind_addr( aCmId, &aAddr->mSa.mSockCommon );
}

stlInt32 stnRdmaCmResolveAddr( struct rdma_cm_id    * aCmId,
                               stlSockAddr          * aSrcAddr,
                               stlSockAddr          * aDstAddr,
                               stlInt32               aTimeoutMs )
{
    return rdma_resolve_addr( aCmId, 
                              &aSrcAddr->mSa.mSockCommon, 
                              &aDstAddr->mSa.mSockCommon, 
                              aTimeoutMs );
}

stlInt32 stnRdmaCmResolveRoute( struct rdma_cm_id   * aCmId,
                                stlInt32              aTimeoutMs )
{
    return rdma_resolve_route( aCmId, aTimeoutMs );
}

stlInt32 stnRdmaCmCreateQp( struct rdma_cm_id       * aCmId,
                            struct ibv_pd           * aPd,
                            struct ibv_qp_init_attr * aQpInitAttr )
{
    return rdma_create_qp( aCmId, aPd, aQpInitAttr );
}

void stnRdmaCmDestroyQp( struct rdma_cm_id          * aCmId )
{
    rdma_destroy_qp( aCmId );
}

stlInt32 stnRdmaCmConnect( struct rdma_cm_id         * aCmId,
                           struct rdma_conn_param    * aConnParam )
{
    return rdma_connect( aCmId, aConnParam );
}

stlInt32 stnRdmaCmListen( struct rdma_cm_id          * aCmId,
                          stlInt32                     aBackLog )
{
    return rdma_listen( aCmId, aBackLog );
}

stlInt32 stnRdmaCmAccept( struct rdma_cm_id          * aCmId,
                          struct rdma_conn_param     * aConnParam )
{
    return rdma_accept( aCmId, aConnParam );
}

stlInt32 stnRdmaCmReject( struct rdma_cm_id          * aCmId,
                          const void                 * aPrivateData,
                          stlUInt8                     aPrivateDataLen )
{
    return rdma_reject( aCmId, aPrivateData, aPrivateDataLen );
}

stlInt32 stnRdmaCmNotify( struct rdma_cm_id          * aCmId,
                          enum ibv_event_type          aEvent )
{
    return rdma_notify( aCmId, aEvent );
}

stlInt32 stnRdmaCmDisconnect( struct rdma_cm_id      * aCmId )
{
    return rdma_disconnect( aCmId );
}

stlInt32 stnRdmaCmJoinMulticast( struct rdma_cm_id  * aCmId,
                                 stlSockAddr        * aAddr,
                                 void               * aContext )
{
    return rdma_join_multicast( aCmId, &aAddr->mSa.mSockCommon, aContext );
}

stlInt32 stnRdmaCmLeaveMulticast( struct rdma_cm_id * aCmId,
                                  stlSockAddr       * aAddr )
{
    return rdma_leave_multicast( aCmId, &aAddr->mSa.mSockCommon );
}

stlInt32 stnRdmaCmGetCmEvent( struct rdma_event_channel     * aChannel,
                              struct rdma_cm_event         ** aEvent )
{
    return rdma_get_cm_event( aChannel, aEvent );
}

stlInt32 stnRdmaCmAckCmEvent( struct rdma_cm_event * aEvent )
{
    return rdma_ack_cm_event( aEvent );
}

struct ibv_context ** stnRdmaCmGetDevices( stlInt32 * aNumDevices )
{
    return rdma_get_devices(aNumDevices );
}


void stnRdmaCmFreeDevices( struct ibv_context ** aList )
{
    rdma_free_devices( aList );
}

const stlChar * stnRdmaCmEventStr( enum rdma_cm_event_type aEvent )
{
    return rdma_event_str( aEvent );
}

stlInt32 stnRdmaPostSend( struct rdma_cm_id     * aCmId, 
                          void                  * aContext, 
                          void                  * aAddr, 
                          stlSize                 aLength, 
                          struct ibv_mr         * aMr,
                          stlInt32                aFlags )
{
    return rdma_post_send( aCmId, aContext, aAddr, aLength, aMr, aFlags );
}



stlInt32 stnRdmaPostRecv( struct rdma_cm_id     * aCmId, 
                          void                  * aContext, 
                          void                  * aAddr, 
                          stlSize                 aLength, 
                          struct ibv_mr         * aMr )
{
    return rdma_post_recv( aCmId, aContext, aAddr, aLength, aMr );
}



