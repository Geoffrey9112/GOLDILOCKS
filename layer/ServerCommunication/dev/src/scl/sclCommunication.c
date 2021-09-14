/*******************************************************************************
 * scpProtocol.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: scpProtocol.c 6676 2012-12-13 08:39:46Z egon $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stl.h>
#include <dtl.h>
#include <knl.h>
#include <sclDef.h>
#include <scDef.h>
#include <scpProtocol.h>
#include <sclGeneral.h>
#include <sclProtocol.h>
#include <sccCommunication.h>

/**
 * @file scpProtocol.c
 * @brief Server Communication Layer Protocol Routines
 */

/**
 * @addtogroup scpProtocol
 * @{
 */

/**
 * @brief array를 이용한 queue를 초기화 한다.
 * @param[in]  aQueue       Queue Handle
 * @param[in]  aItemSize    Queue의 item 한개의 크기
 * @param[in]  aItemCount   Queue에 저장되는 item 개수
 * @param[out] aUsedMemory  Queue에서 총 사용하는 메모리 byte
 * @param[in]  aEnv         env
 */
stlStatus sclInitializeQueue( sclQueue        * aQueue,
                              stlInt64          aItemSize,
                              stlInt64          aItemCount,
                              stlInt64        * aUsedMemory,
                              sclEnv          * aEnv )
{
    return sccInitializeQueue( aQueue,
                               aItemSize,
                               aItemCount,
                               aUsedMemory,
                               aEnv );
}

/**
 * @brief array를 이용한 process간 queue를 종료 한다.
 * @param[in]  aQueue       Queue Handle
 * @param[in]  aEnv         env
 */
stlStatus sclFinalizeQueue( sclQueue      * aQueue,
                            sclEnv        * aEnv )
{
    return sccFinalizeQueue( aQueue,
                             aEnv );
}


/**
 * @brief array를 이용한 process간 queue에 data를 넣는다.
 * @param[in]  aQueue       Queue Handle
 * @param[in]  aItem        넣을 data
 * @param[in]  aEnv         env
 */
stlStatus sclEnqueue( sclQueue      * aQueue,
                      void          * aItem,
                      sclEnv        * aEnv )
{
    return sccEnqueue( aQueue,
                       aItem,
                       aEnv );
}

/**
 * @brief array를 이용한 process간 queue에서 data를 꺼낸다.
 * @param[in]  aQueue       Queue Handle
 * @param[in]  aItem        꺼낸 data 저장할 주소
 * @param[in]  aTimeout     대기 시간 (usec)
 * @param[in]  aEnv         env
 */
stlStatus sclDequeue( sclQueue      * aQueue,
                      void          * aItem,
                      stlInterval     aTimeout,
                      sclEnv        * aEnv )
{
    return sccDequeue( aQueue,
                       aItem,
                       aTimeout,
                       aEnv );
}


/**
 * @brief pointer와 command를 전달할수 있는 queue를 초기화 한다.
 * @param[in]  aQueue       Queue Handle
 * @param[in]  aItemCount   Queue에 저장되는 item 개수
 * @param[out] aUsedMemory  Queue에서 총 사용하는 메모리 byte
 * @param[in]  aEnv         env
 * @remark 전송되는 data는 session pointer와 command이며
 *         item size는 자동으로 설정된다.
 */
stlStatus sclInitializeRequestQueue( sclQueue        * aQueue,
                                     stlInt64          aItemCount,
                                     stlInt64        * aUsedMemory,
                                     sclEnv          * aEnv )
{
    return sclInitializeQueue( aQueue,
                               STL_SIZEOF(sclHandle *),
                               aItemCount,
                               aUsedMemory,
                               aEnv );
}

/**
 * @brief pointer와 command를 전달할수 있는 queue를 종료 한다.
 * @param[in]  aQueue       Queue Handle
 * @param[in]  aEnv         env
 */
stlStatus sclFinalizeRequestQueue( sclQueue      * aQueue,
                                   sclEnv        * aEnv )
{
    return sclFinalizeQueue( aQueue,
                             aEnv );
}


/**
 * @brief pointer와 command를 전달할수 있는 queue에 enqueue한다.
 * @param[in]  aQueue           Queue Handle
 * @param[in]  aSclHandle       넣을 SharedHandle
 * @param[in]  aEnv             env
 */
stlStatus sclEnqueueRequest( sclQueue         * aQueue,
                             sclHandle        * aSclHandle,
                             sclEnv           * aEnv )
{
    stlChar     sData[STL_SIZEOF(sclHandle *)];

    stlMemcpy( sData, &aSclHandle, STL_SIZEOF(sclHandle *) );

    return sclEnqueue( aQueue,
                       sData,
                       aEnv );
}

/**
 * @brief shared-server <-> dispatcher간 queue에서 data를 꺼낸다.
 * @param[in]  aQueue           Queue Handle
 * @param[out]  aSclHandle      가져올 SharedHandle
 * @param[in]  aTimeout         대기 시간 (usec)
 * @param[in]  aEnv             env
 */
stlStatus sclDequeueRequest( sclQueue          * aQueue,
                             sclHandle        ** aSclHandle,
                             stlInterval         aTimeout,
                             sclEnv            * aEnv )
{
    stlChar     sData[STL_SIZEOF(sclHandle *) + STL_SIZEOF(sclQueueCommand)];

    STL_TRY( sclDequeue( aQueue,
                         sData,
                         aTimeout,
                         aEnv )
             == STL_SUCCESS );

    stlMemcpy( aSclHandle, sData, STL_SIZEOF(sclHandle *) );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief pointer와 command를 전달할수 있는 queue를 초기화 한다.
 * @param[in]  aQueue       Queue Handle
 * @param[in]  aItemCount   Queue에 저장되는 item 개수
 * @param[out] aUsedMemory  Queue에서 총 사용하는 메모리 byte
 * @param[in]  aEnv         env
 * @remark 전송되는 data는 session pointer와 command이며
 *         item size는 자동으로 설정된다.
 */
stlStatus sclInitializeResponseQueue( sclQueue        * aQueue,
                                      stlInt64          aItemCount,
                                      stlInt64        * aUsedMemory,
                                      sclEnv          * aEnv )
{
    return sclInitializeQueue( aQueue,
                               STL_SIZEOF(sclHandle *) + STL_SIZEOF(sclQueueCommand) + STL_SIZEOF(stlInt64),
                               aItemCount,
                               aUsedMemory,
                               aEnv );
}

/**
 * @brief pointer와 command를 전달할수 있는 queue를 종료 한다.
 * @param[in]  aQueue       Queue Handle
 * @param[in]  aEnv         env
 */
stlStatus sclFinalizeResponseQueue( sclQueue      * aQueue,
                                    sclEnv        * aEnv )
{
    return sclFinalizeQueue( aQueue,
                             aEnv );
}


/**
 * @brief pointer와 command를 전달할수 있는 queue에 enqueue한다.
 * @param[in]  aQueue           Queue Handle
 * @param[in]  aSclHandle       넣을 SharedHandle
 * @param[in]  aCommand         넣을 command
 * @param[in]  aCmUnitCount     data가 저정되어 있는 cm unit count
 * @param[in]  aEnv             env
 */
stlStatus sclEnqueueResponse( sclQueue         * aQueue,
                              sclHandle        * aSclHandle,
                              sclQueueCommand    aCommand,
                              stlInt64           aCmUnitCount,
                              sclEnv           * aEnv )
{
    stlChar     sData[STL_SIZEOF(sclHandle *) + STL_SIZEOF(sclQueueCommand) + STL_SIZEOF(stlInt64)];

    stlMemcpy( sData, &aSclHandle, STL_SIZEOF(sclHandle *) );
    stlMemcpy( sData + STL_SIZEOF(sclHandle *), &aCommand, STL_SIZEOF(sclQueueCommand) );
    stlMemcpy( sData + STL_SIZEOF(sclHandle *) + STL_SIZEOF(sclQueueCommand), &aCmUnitCount, STL_SIZEOF(stlInt64) );

    return sclEnqueue( aQueue,
                       sData,
                       aEnv );
}

/**
 * @brief shared-server <-> dispatcher간 queue에서 data를 꺼낸다.
 * @param[in]  aQueue           Queue Handle
 * @param[out]  aSclHandle      가져올 SharedHandle
 * @param[out]  aCommand        가져올 command
 * @param[out]  aCmUnitCount    가져올 cm unit count
 * @param[in]  aTimeout         대기 시간 (usec)
 * @param[in]  aEnv             env
 */
stlStatus sclDequeueResponse( sclQueue          * aQueue,
                              sclHandle        ** aSclHandle,
                              sclQueueCommand   * aCommand,
                              stlInt64          * aCmUnitCount,
                              stlInterval         aTimeout,
                              sclEnv            * aEnv )
{
    stlChar     sData[STL_SIZEOF(sclHandle *) + STL_SIZEOF(sclQueueCommand) + STL_SIZEOF(stlInt64)];

    STL_TRY( sclDequeue( aQueue,
                         sData,
                         aTimeout,
                         aEnv )
             == STL_SUCCESS );

    stlMemcpy( aSclHandle, sData, STL_SIZEOF(sclHandle *) );
    stlMemcpy( aCommand, sData + STL_SIZEOF(sclHandle *), STL_SIZEOF(sclQueueCommand) );
    stlMemcpy( aCmUnitCount, sData + STL_SIZEOF(sclHandle *) + STL_SIZEOF(sclQueueCommand), STL_SIZEOF(stlInt64) );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief sclIpc를 초기화 한다.
 * @param[in]  aIpc             sclIpc
 * @param[in]  aArrayAllocator  array allocator handle
 * @param[in]  aResponseEvent   response event
 * @param[in]  aEnv             env
 */
stlStatus sclInitializeIpc( sclIpc              * aIpc,
                            stlArrayAllocator   * aArrayAllocator,
                            sclQueue            * aResponseEvent,
                            sclEnv              * aEnv )
{
    stlInt32    sState = 0;

    aIpc->mArrayAllocator = aArrayAllocator;

    STL_TRY( sccInitializeIpcSimplex( &aIpc->mRequestIpc,
                                      aArrayAllocator,
                                      aEnv )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( sccInitializeIpcSimplex( &aIpc->mResponseIpc,
                                      aArrayAllocator,
                                      aEnv )
             == STL_SUCCESS );
    sState = 2;

    aIpc->mResponseEvent = aResponseEvent;

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)sccFinalizeIpcSimplex( &aIpc->mResponseIpc,
                                         aArrayAllocator,
                                         aEnv );
        case 1:
            (void)sccFinalizeIpcSimplex( &aIpc->mRequestIpc,
                                         aArrayAllocator,
                                         aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}


/**
 * @brief sclIpc를 종료 한다.
 * @param[in]  aIpc             sclIpc
 * @param[in]  aArrayAllocator  array allocator handle
 * @param[in]  aEnv             env
 */
stlStatus sclFinalizeIpc( sclIpc              * aIpc,
                          stlArrayAllocator   * aArrayAllocator,
                          sclEnv              * aEnv )
{

    STL_TRY( sccFinalizeIpcSimplex( &aIpc->mRequestIpc,
                                    aArrayAllocator,
                                    aEnv )
             == STL_SUCCESS );

    STL_TRY( sccFinalizeIpcSimplex( &aIpc->mResponseIpc,
                                    aArrayAllocator,
                                    aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief sclIpcSimplex를 초기화 한다.
 * @param[in]  aIpcSimplex      ipcSimplex handle
 * @param[in]  aArrayAllocator  array allocator handle
 * @param[in]  aEnv             env
 */
stlStatus sclInitializeIpcSimplex( sclIpcSimplex       * aIpcSimplex,
                                   stlArrayAllocator   * aArrayAllocator,
                                   sclEnv              * aEnv )
{
    return sccInitializeIpcSimplex( aIpcSimplex,
                                    aArrayAllocator,
                                    aEnv );
}

/**
 * @brief sclIpcSimplex를 종료 한다.
 * @param[in]  aIpcSimplex      ipcSimplex handle
 * @param[in]  aArrayAllocator  array allocator handle
 * @param[in]  aEnv             env
 */
stlStatus sclFinalizeIpcSimplex( sclIpcSimplex       * aIpcSimplex,
                                 stlArrayAllocator   * aArrayAllocator,
                                 sclEnv              * aEnv )
{
    return sccFinalizeIpcSimplex( aIpcSimplex,
                                  aArrayAllocator,
                                  aEnv );
}

/**
 * @brief sclIpcSimplex에서 send를 위한 buffer를 가져온다.
 * @param[in]  aIpcSimplex      ipcSimplex handle
 * @param[in]  aArrayAllocator  array allocator handle
 * @param[out] aBuf             send를 위한 buffer
 * @param[in]  aEnv             env
 * @remark zerocopy를 위한 api로 sclGetSendBuffer하여 send할 data를 설정하고
 * sclCompleteSendBuffer를 이용해서 완료 처리한다.
 */
stlStatus sclGetSendBuffer( sclIpcSimplex       * aIpcSimplex,
                            stlArrayAllocator   * aArrayAllocator,
                            stlChar            ** aBuf,
                            sclEnv              * aEnv )
{
    return sccGetSendBuffer( aIpcSimplex,
                             aArrayAllocator,
                             aBuf,
                             aEnv );
}

/**
 * @brief sclIpcSimplex에서 send를 완료한다.
 * @param[in]  aIpcSimplex      ipcSimplex handle
 * @param[in]  aArrayAllocator  array allocator handle
 * @param[in]  aBuffer          sclGetSendBuffer로 얻은 buffer
 * @param[in]  aLen             send할 length
 * @param[in]  aEnv             env
 * @remark sclGetSendBuffer로 가져온 buffer의 사용을 완료 했다는 의미임.
 */
stlStatus sclCompleteSendBuffer( sclIpcSimplex      * aIpcSimplex,
                                 stlArrayAllocator  * aArrayAllocator,
                                 stlChar            * aBuffer,
                                 stlSize              aLen,
                                 sclEnv             * aEnv )
{
    return sccCompleteSendBuffer( aIpcSimplex,
                                  aArrayAllocator,
                                  aBuffer,
                                  aLen,
                                  aEnv );
}

/**
 * @brief sclIpcSimplex의 recv buffer를 가져온다.
 * @param[in]  aIpcSimplex      ipcSimplex handle
 * @param[in]  aArrayAllocator  allocator
 * @param[out] aBuf             recv할 buffer
 * @param[out] aSize            buffer size
 * @param[in]  aEnv             env
 * @remark zerocopy를 위한 api로 sclGetRecvBuffer하여 recv한 data를 처리하고
 * sclCompleteRecvBuffer를 이용해서 완료 처리한다.
 */
stlStatus sclGetRecvBuffer( sclIpcSimplex       * aIpcSimplex,
                            stlArrayAllocator   * aArrayAllocator,
                            stlChar            ** aBuf,
                            stlInt32            * aSize,
                            sclEnv              * aEnv )
{
    return sccGetRecvBuffer( aIpcSimplex,
                             aArrayAllocator,
                             aBuf,
                             aSize,
                             aEnv );
}

/**
 * @brief sclIpcSimplex의 recv 를 완료한다.
 * @param[in]  aIpcSimplex      ipcSimplex handle
 * @param[in]  aArrayAllocator  allocator
 * @param[in]  aBuffer          sclGetRecvBuffer로 얻은 buffer
 * @param[in]  aEnv             env
 * @remark sclGetRecvBuffer로 가져온 buffer의 사용을 완료 했다는 의미임.
 */
stlStatus sclCompleteRecvBuffer( sclIpcSimplex      * aIpcSimplex,
                                 stlArrayAllocator  * aArrayAllocator,
                                 stlChar            * aBuffer,
                                 sclEnv             * aEnv )
{
    return sccCompleteRecvBuffer( aIpcSimplex,
                                  aArrayAllocator,
                                  aBuffer,
                                  aEnv );
}

/**
 * @brief 버퍼의 내용을 원격으로 전송한다.
 * @param[in] aContext      socket Context
 * @param[in] aData         전송할 data
 * @param[in] aDataLen      전송할 data length
 * @param[in,out] aSentLen  전송한 data length
 * @param[in] aEnv          env
 * @remark dispatcher에서 사용됨.
 */
stlStatus sclSendToSocket( stlContext    * aContext,
                           stlChar       * aData,
                           stlInt32        aDataLen,
                           stlSize       * aSentLen,
                           sclEnv        * aEnv )
{
    return sccSendToSocket( aContext,
                            aData,
                            aDataLen,
                            aSentLen,
                            aEnv );
}


/** @} */
