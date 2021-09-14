/*******************************************************************************
 * stwUnix.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stwUnix.c 641 2011-04-28 11:27:33Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stc.h>
#include <stlDef.h>
#include <stlError.h>
#include <ste.h>

/**
 * @brief Message Queue를 생성한다.
 * @param[out] aMsgQueue     생성된 Message Queue
 * @param[in]  aMsgKey       Message Queue Key
 * @param[in]  aExcl         기존에 Message Queue가 이미 있다면 배제적으로 할지 여부
 * @param[in]  aErrorStack   푸쉬될 에러스택 포인터
 * @par Error Codes :
 * @code
 * [STL_ERRCODE_INSUFFICIENT_RESOURCE]
 *     주어진 timeout 동안 이벤트가 발생하지 않았다.
 * [STL_ERRCODE_MSGQ_ACCESS]
 *     주어진 timeout 동안 이벤트가 발생하지 않았다.
 * [STL_ERRCODE_OUT_OF_MEMORY]
 *     커널 메모리를 할당받을수 없는 경우에 발생함.
 * [STL_ERRCODE_MSGQ_CREATE]
 *     msgget()이 실패했음. 시스템 에러를 확인해야 한다.
 * @endcode
 */
stlStatus stqOpen( stlMsgQueue   * aMsgQueue,
                   stlInt32        aMsgKey,
                   stlBool         aExcl,
                   stlErrorStack * aErrorStack )
{
    stlInt32 sFlag;

    sFlag = IPC_CREAT;
    
    if( aExcl == STL_TRUE )
    {
        sFlag |= IPC_EXCL;
    }
    
    aMsgQueue->mId = msgget( aMsgKey, sFlag | 0666 );

    STL_TRY_THROW( aMsgQueue->mId != -1, RAMP_ERR_MSGGET );

    aMsgQueue->mKey = aMsgKey;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_MSGGET )
    {
        switch( errno )
        {
            case ENOMEM:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_OUT_OF_MEMORY,
                              NULL,
                              aErrorStack );
                break;
            case EACCES:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_MSGQ_ACCESS,
                              NULL,
                              aErrorStack );
                break;
            case ENOSPC:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_INSUFFICIENT_RESOURCE,
                              NULL,
                              aErrorStack );
                break;
            case EEXIST:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_MSGQ_EXIST,
                              NULL,
                              aErrorStack );
                break;
            default:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_MSGQ_CREATE,
                              NULL,
                              aErrorStack );
                break;
        }
        steSetSystemError( errno, aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
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
stlStatus stqClose( stlMsgQueue   * aMsgQueue,
                    stlErrorStack * aErrorStack )
{
    STL_TRY_THROW( msgctl( aMsgQueue->mId,
                           IPC_RMID,
                           NULL ) != -1, RAMP_ERR_MSGCTL );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_MSGCTL )
    {
        switch( errno )
        {
            case EIDRM:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_MSGQ_REMOVED,
                              NULL,
                              aErrorStack );
                break;
            case EACCES:
            case EPERM:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_MSGQ_ACCESS,
                              NULL,
                              aErrorStack );
                break;
            case EINVAL:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_INVALID_ARGUMENT,
                              NULL,
                              aErrorStack );
                break;
            default:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_MSGQ_DESTROY,
                              NULL,
                              aErrorStack );
                break;
        }
        steSetSystemError( errno, aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
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
stlStatus stqSend( stlMsgQueue   * aMsgQueue,
                   stlMsgBuffer  * aMsgBuffer,
                   stlInt64        aMsgSize,
                   stlErrorStack * aErrorStack )
{
    STL_TRY_THROW( msgsnd( aMsgQueue->mId,
                           (void*)aMsgBuffer,
                           aMsgSize,
                           0 ) != -1, RAMP_ERR_MSGSND );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_MSGSND )
    {
        switch( errno )
        {
            case ENOMEM:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_OUT_OF_MEMORY,
                              NULL,
                              aErrorStack );
                break;
            case EACCES:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_MSGQ_ACCESS,
                              NULL,
                              aErrorStack );
                break;
            case EINVAL:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_INVALID_ARGUMENT,
                              NULL,
                              aErrorStack );
                break;
            case EIDRM:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_MSGQ_REMOVED,
                              NULL,
                              aErrorStack );
                break;
            default:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_MSGQ_SEND,
                              NULL,
                              aErrorStack );
                break;
        }
        steSetSystemError( errno, aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
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
stlStatus stqRecv( stlMsgQueue   * aMsgQueue,
                   stlMsgBuffer  * aMsgBuffer,
                   stlInt32        aMsgSize,
                   stlInt64        aMsgType,
                   stlInt32        aMsgFlag,
                   stlSize       * aReceivedSize,
                   stlErrorStack * aErrorStack )
{
    stlSize sSize;

    sSize = msgrcv( aMsgQueue->mId,
                    (void*)aMsgBuffer,
                    aMsgSize,
                    aMsgType,
                    aMsgFlag );

    STL_TRY_THROW( sSize != -1, RAMP_ERR_MSGRCV );

    *aReceivedSize = sSize;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_MSGRCV )
    {
        switch( errno )
        {
            case EAGAIN:
            case ENOMSG:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_MSGQ_AGAIN,
                              NULL,
                              aErrorStack );
                break;
            case EACCES:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_MSGQ_ACCESS,
                              NULL,
                              aErrorStack );
                break;
            case EINVAL:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_INVALID_ARGUMENT,
                              NULL,
                              aErrorStack );
                break;
            case EIDRM:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_MSGQ_REMOVED,
                              NULL,
                              aErrorStack );
                break;
            default:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_MSGQ_RECV,
                              NULL,
                              aErrorStack );
                break;
        }
        steSetSystemError( errno, aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}


