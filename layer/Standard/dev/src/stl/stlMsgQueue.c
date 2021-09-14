/*******************************************************************************
 * stlMsgQueue.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stlMsgQueue.c 9323 2013-08-09 03:26:54Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file stlMsgQueue.c
 * @brief Standard Layer Message Queue Routines
 */

#include <stlDef.h>
#include <stl.h>
#include <stq.h>
#include <stlError.h>

/**
 * @addtogroup stlMsgQueue
 * @{
 */

/**
 * @brief Message Queue를 생성한다.
 * @param[out] aMsgQueue     생성된 Message Queue
 * @param[in]  aMsgKey       Message Queue Key
 * @param[in]  aErrorStack   푸쉬될 에러스택 포인터
 * @par Error Codes :
 * @code
 * [STL_ERRCODE_INSUFFICIENT_RESOURCE]
 *     프로세스나 시스템에서 할당받을수 있는 Message Queue 제한을 넘어선 경우에 발생함.
 * [STL_ERRCODE_MSGQ_ACCESS]
 *     Message Queue에 접근 권한이 없는 경우에 발생함.
 * [STL_ERRCODE_OUT_OF_MEMORY]
 *     커널 메모리를 할당받을수 없는 경우에 발생함.
 * [STL_ERRCODE_MSGQ_EXIST]
 *     입력받은 key로 이미 Message Queue가 존재함.
 * [STL_ERRCODE_MSGQ_CREATE]
 *     msgget()이 실패했음. 시스템 에러를 확인해야 한다.
 * @endcode
 */
stlStatus stlCreateMsgQueue( stlMsgQueue   * aMsgQueue,
                             stlInt32        aMsgKey,
                             stlErrorStack * aErrorStack )
{
    return stqOpen( aMsgQueue,
                    aMsgKey,
                    STL_TRUE,
                    aErrorStack );
}


/**
 * @brief 생성된 Message Queue를 얻는다.
 * @param[out] aMsgQueue     Message Queue
 * @param[in]  aMsgKey       Message Queue Key
 * @param[in]  aErrorStack   푸쉬될 에러스택 포인터
 * @par Error Codes :
 * @code
 * [STL_ERRCODE_INSUFFICIENT_RESOURCE]
 *     프로세스나 시스템에서 할당받을수 있는 Message Queue 제한을 넘어선 경우에 발생함.
 * [STL_ERRCODE_MSGQ_ACCESS]
 *     Message Queue에 접근 권한이 없는 경우에 발생함.
 * [STL_ERRCODE_OUT_OF_MEMORY]
 *     커널 메모리를 할당받을수 없는 경우에 발생함.
 * [STL_ERRCODE_MSGQ_EXIST]
 *     입력받은 key로 이미 Message Queue가 존재함.
 * [STL_ERRCODE_MSGQ_CREATE]
 *     msgget()이 실패했음. 시스템 에러를 확인해야 한다.
 * @endcode
 */
stlStatus stlGetMsgQueue( stlMsgQueue   * aMsgQueue,
                          stlInt32        aMsgKey,
                          stlErrorStack * aErrorStack )
{
    return stqOpen( aMsgQueue,
                    aMsgKey,
                    STL_FALSE,
                    aErrorStack );
}


/**
 * @brief Message Queue를 삭제한다.
 * @param[in]  aMsgQueue     Message Queue
 * @param[in]  aErrorStack   푸쉬될 에러스택 포인터
 * @par Error Codes :
 * @code
 * [STL_ERRCODE_MSGQ_REMOVED]
 *     Message Queue가 이미 삭제된 경우
 * [STL_ERRCODE_MSGQ_ACCESS]
 *     Message Queue에 접근 권한이 없는 경우에 발생함.
 * [STL_ERRCODE_INVALID_ARGUMENT]
 *     잘못된 인자를 사용하는 경우에 발생함.
 * [STL_ERRCODE_MSGQ_DESTROY]
 *     msgctl()이 실패했음. 시스템 에러를 확인해야 한다.
 * @endcode
 */
stlStatus stlDestroyMsgQueue( stlMsgQueue   * aMsgQueue,
                              stlErrorStack * aErrorStack )
{
    return stqClose( aMsgQueue,
                     aErrorStack );
}


/**
 * @brief Message Queue로 데이터를 전송한다.
 * @param[in]  aMsgQueue     Message Queue
 * @param[in]  aMsgBuffer    전송할 Message가 저장되어 있는 Buffer
 * @param[in]  aMsgSize      전송할 Message 크기
 * @param[in]  aErrorStack   푸쉬될 에러스택 포인터
 * @par Error Codes :
 * @code
 * [STL_ERRCODE_MSGQ_ACCESS]
 *     Message Queue에 접근 권한이 없는 경우에 발생함.
 * [STL_ERRCODE_OUT_OF_MEMORY]
 *     커널 메모리를 할당받을수 없는 경우에 발생함.
 * [STL_ERRCODE_INVALID_ARGUMENT]
 *     잘못된 인자를 사용하는 경우에 발생함.
 * [STL_ERRCODE_MSGQ_REMOVED]
 *     Message Queue가 이미 삭제된 경우
 * [STL_ERRCODE_MSGQ_SEND]
 *     msgsnd()이 실패했음. 시스템 에러를 확인해야 한다.
 * @endcode
 */
stlStatus stlSendMsgQueue( stlMsgQueue   * aMsgQueue,
                           stlMsgBuffer  * aMsgBuffer,
                           stlInt64        aMsgSize,
                           stlErrorStack * aErrorStack )
{
    return stqSend( aMsgQueue,
                    aMsgBuffer,
                    aMsgSize,
                    aErrorStack );
}


/**
 * @brief Message Queue로부터 데이터를 받는다.
 * @param[in]  aMsgQueue     Message Queue
 * @param[in]  aMsgBuffer    Message가 저장될 Buffer
 * @param[in]  aMsgSize      받을 Message Size
 * @param[in]  aMsgType      받을 Message Type
 * @param[in]  aMsgFlag      Message Flags( STL_MSGQ_FLAG_NOWAIT or 0 )
 * @param[out] aReceivedSize 받은 data length
 * @param[in]  aErrorStack   푸쉬될 에러스택 포인터
 * @see @a aMsgType : stlMsgQueueType
 * @par Error Codes :
 * @code
 * [STL_ERRCODE_MSGQ_AGAIN]
 *     메세지가 없는 경우에 발생함.
 * [STL_ERRCODE_MSGQ_ACCESS]
 *     Message Queue에 접근 권한이 없는 경우에 발생함.
 * [STL_ERRCODE_INVALID_ARGUMENT]
 *     잘못된 인자를 사용하는 경우에 발생함.
 * [STL_ERRCODE_MSGQ_REMOVED]
 *     Message Queue가 이미 삭제된 경우
 * [STL_ERRCODE_MSGQ_RECV]
 *     msgrcv()이 실패했음. 시스템 에러를 확인해야 한다.
 * @endcode
 */
stlStatus stlRecvMsgQueue( stlMsgQueue   * aMsgQueue,
                           stlMsgBuffer  * aMsgBuffer,
                           stlInt32        aMsgSize,
                           stlInt64        aMsgType,
                           stlInt32        aMsgFlag,
                           stlSize       * aReceivedSize,
                           stlErrorStack * aErrorStack )
{
    return stqRecv( aMsgQueue,
                    aMsgBuffer,
                    aMsgSize,
                    aMsgType,
                    aMsgFlag,
                    aReceivedSize,
                    aErrorStack );
}


/**
 * @brief Message Buffer를 할당한다.
 * @param[in]  aMsgSize      할당할 Message 크기
 * @param[out] aMsgBuffer    할당된 Message Buffer
 * @param[in]  aErrorStack   푸쉬될 에러스택 포인터
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_OUT_OF_MEMORY]
 *     커널 메모리를 할당받을수 없는 경우에 발생함.
 * @endcode
 */
stlStatus stlAllocMsgBuffer( stlInt64         aMsgSize,
                             stlMsgBuffer  ** aMsgBuffer,
                             stlErrorStack  * aErrorStack )
{
    void * sAddr;
    
    STL_TRY( stlAllocHeap( (void**)&sAddr,
                           STL_SIZEOF(stlInt64) + aMsgSize,
                           aErrorStack )
             == STL_SUCCESS );

    *aMsgBuffer = (stlMsgBuffer*)sAddr;

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Message Buffer를 해제한다.
 * @param[in]  aMsgBuffer   Message Buffer
 * @param[in]  aErrorStack   푸쉬될 에러스택 포인터
 */
stlStatus stlFreeMsgBuffer( stlMsgBuffer  * aMsgBuffer,
                            stlErrorStack * aErrorStack )
{
    STL_PARAM_VALIDATE( aMsgBuffer != NULL, aErrorStack );
    
    stlFreeHeap( aMsgBuffer );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Message를 Buffer에 복사한다.
 * @param[in]  aMsgBuffer    Message Buffer
 * @param[in]  aMsg          복사할 Message
 * @param[in]  aMsgSize      복사할 Message Size
 * @param[in]  aMsgType      복사할 Message Type
 * @param[in]  aErrorStack   푸쉬될 에러스택 포인터
 * @see @a aMsgType : stlMsgQueueType
 */
stlStatus stlCopyToMsgBuffer( stlMsgBuffer  * aMsgBuffer,
                              void          * aMsg,
                              stlInt64        aMsgSize,
                              stlInt64        aMsgType,
                              stlErrorStack * aErrorStack )
{
    STL_PARAM_VALIDATE( aMsgBuffer != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aMsgSize >= 0, aErrorStack );
    STL_PARAM_VALIDATE( aMsgType > 0, aErrorStack );
    
    if( aMsgSize > 0 )
    {
        stlMemcpy( (void*)aMsgBuffer->mText, aMsg, aMsgSize );
    }

    aMsgBuffer->mType = aMsgType;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief STARTUP을 위한 메세지 큐를 생성한다.
 * @param[in]  aMsgQueue     생성된 공유메모리 세그먼트
 * @param[out] aMsgKey       생성된 공유메모리 키
 * @param[out] aErrorStack   에러 스택
 * @par Error Codes :
 * @code
 * [STL_ERRCODE_INSUFFICIENT_RESOURCE]
 *     프로세스나 시스템에서 할당받을수 있는 Message Queue 제한을 넘어선 경우에 발생함.
 * [STL_ERRCODE_MSGQ_ACCESS]
 *     Message Queue에 접근 권한이 없는 경우에 발생함.
 * [STL_ERRCODE_OUT_OF_MEMORY]
 *     커널 메모리를 할당받을수 없는 경우에 발생함.
 * [STL_ERRCODE_MSGQ_EXIST]
 *     입력받은 key로 이미 Message Queue가 존재함.
 * [STL_ERRCODE_MSGQ_CREATE]
 *     msgget()이 실패했음. 시스템 에러를 확인해야 한다.
 * @endcode
 */
stlStatus stlCreateMsgQueue4Startup( stlMsgQueue   * aMsgQueue,
                                     stlInt32      * aMsgKey,
                                     stlErrorStack * aErrorStack )
{
    stlInt32  sMsgKey;
    
    sMsgKey = STL_STARTUP_MSG_KEY;

    /**
     * 할당 가능한 메세지 큐 key를 찾는다.
     */
    
    while(1)
    {
        if( stlCreateMsgQueue( aMsgQueue,
                               sMsgKey,
                               aErrorStack )
            == STL_SUCCESS )
        {
            break;
        }

        sMsgKey++;
        
        STL_TRY( stlGetLastErrorCode(aErrorStack) == STL_ERRCODE_MSGQ_EXIST );
        
        stlPopError( aErrorStack );
    }

    *aMsgKey = sMsgKey;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
