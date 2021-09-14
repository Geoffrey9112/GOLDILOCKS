/*******************************************************************************
 * stlError.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file stlError.c
 * @brief Standard Layer Error Routines
 */

#include <stc.h>
#include <stlDef.h>
#include <stl.h>
#include <stlError.h>
#include <ste.h>

#if defined( STC_HAVE_STDIO_H )
#include <stdio.h>
#endif
#if defined( STC_HAVE_STDARG_H )
#include <stdarg.h>
#endif
#if defined( STC_HAVE_STRING_H )
#include <string.h>
#endif

/**
 * @addtogroup stlError
 * @{
 */

/**
 * @brief Standard Layer 에러 테이블.
 * 내부에러에 대응되는 외부에러와 에러 메세지로 구성됨.
 */
stlErrorRecord gStandardErrorTable[STL_MAX_ERROR + 1] =
{
    {/* STL_ERRCODE_INVALID_ARGUMENT */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Invalid argument"
    },
    {/* STL_ERRCODE_INCOMPLETE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "The operation was incomplete although some processing was performed "
        "and the results are partially valid"
    },
    {/* STL_ERRCODE_ACCESS */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Permission denied '%s'"
    },
    {/* STL_ERRCODE_EOF */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Standard Layer has encountered the end of the file"
    },
    {/* STL_ERRCODE_FILE_DUP */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to duplicate a file"
    },
    {/* STL_ERRCODE_FILE_LOCK */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "File Lock Error"
    },
    {/* STL_ERRCODE_FILE_UNLOCK */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "File Unlock Error"
    },
    {/* STL_ERRCODE_FILE_OPEN */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to open the file '%s'"
    },
    {/* STL_ERRCODE_FILE_CLOSE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to close the file"
    },
    {/* STL_ERRCODE_FILE_REMOVE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to remove the file"
    },
    {/* STL_ERRCODE_FILE_LINK */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to link the file"
    },
    {/* STL_ERRCODE_FILE_READ */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to read the file"
    },
    {/* STL_ERRCODE_FILE_WRITE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to write the file"
    },
    {/* STL_ERRCODE_FILE_SYNC */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to sync the file"
    },
    {/* STL_ERRCODE_FILE_SEEK */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to seek the file"
    },
    {/* STL_ERRCODE_FILE_TRUNCATE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to truncate the file"
    },
    {/* STL_ERRCODE_FILE_STAT */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to get the status of the file"
    },
    {/* STL_ERRCODE_PIPE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to create a pipe"
    },
    {/* STL_ERRCODE_PROC_WAIT */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to wait for the given process"
    },
    {/* STL_ERRCODE_INSUFFICIENT_RESOURCE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Insufficient resource"
    },
    {/* STL_ERRCODE_SIGNAL_KILL */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to send signal to given process"
    },
    {/* STL_ERRCODE_SIGNAL_SET_HANDLER */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to set a signal handler"
    },
    {/* STL_ERRCODE_SIGNAL_BLOCK */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to block a signal"
    },
    {/* STL_ERRCODE_THREAD_ATTR */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to set thread attribute"
    },
    {/* STL_ERRCODE_THREAD_CREATE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to create a thread"
    },
    {/* STL_ERRCODE_THREAD_JOIN */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to join with a terminated thread"
    },
    {/* STL_ERRCODE_THREAD_ONCE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to set thread once code"
    },
    {/* STL_ERRCODE_STRING_TRUNCATION */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Truncation occurred on I/O operation"
    },
    {/* STL_ERRCODE_NAMETOOLONG */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "The given name(%s) is too long"
    },
    {/* STL_ERRCODE_SHM_EXIST */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "shared memory segment exists"
    },
    {/* STL_ERRCODE_SHM_CREATE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to allocate a shared memory segment"
    },
    {/* STL_ERRCODE_SHM_ATTACH */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to attach the shared memory segment"
    },
    {/* STL_ERRCODE_SHM_DETACH */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to detach the shared memory segment"
    },
    {/* STL_ERRCODE_SHM_DESTROY */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to destroy the shared memory segment"
    },
    {/* STL_ERRCODE_SEM_CREATE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to create a semaphore"
    },
    {/* STL_ERRCODE_SEM_ACQUIRE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to acquire the given semaphore"
    },
    {/* STL_ERRCODE_SEM_RELEASE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to release the given semaphore"
    },
    {/* STL_ERRCODE_SEM_DESTROY */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to destory the given semaphore"
    },
    {/* STL_ERRCODE_BUSY */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "The given object was busy"
    },
    {/* STL_ERRCODE_TIMEDOUT */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Operation timed out"
    },
    {/* STL_ERRCODE_NO_ENTRY */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "No such object (%s)"
    },
    {/* STL_ERRCODE_MISMATCH */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Two objects do not match"
    },
    {/* STL_ERRCODE_OUT_OF_MEMORY */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Not enough memory"
    },
    {/* STL_ERRCODE_DISK_FULL */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "File system is full"
    },
    {/* STL_ERRCODE_PROC_EXECUTE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to execute the program"
    },
    {/* STL_ERRCODE_NOTSUPPORT */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "not supported function"
    },
    {/* STL_ERRCODE_NOSPACE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "given buffer does'nt have enough space"
    },
    {/* STL_ERRCODE_INET_PTON_INVAL */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "invalid network address"
    },
    {/* STL_ERRCODE_SOCKET_SENDTO */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "sendto() system call returned failure"
    },
    {/* STL_ERRCODE_SOCKET_RECVFROM */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "recvfrom() system call returned failure"
    },
    {/* STL_ERRCODE_GETSOCKNAME */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "fail to get address from socket with getsockname()"
    },
    {/* STL_ERRCODE_GETPEERNAME */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "fail to get peer address from socket with getpeername()"
    },
    {/* STL_ERRCODE_FCNTL */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "fail to manipulate file descriptor with fcntl()"
    },
    {/* STL_ERRCODE_SETSOCKOPT */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "fail to manipulate socket descriptor with setsockopt()"
    },
    {/* STL_ERRCODE_NOTIMPL */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "not implemented feature"
    },
    {/* STL_ERRCODE_IOCTL */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "fail to manipulate device parameters of special file with ioctl()"
    },
    {/* STL_ERRCODE_GETHOSTNAME */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "fail to get hostname"
    },
    {/* STL_ERRCODE_SOCKET_CLOSE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "fail to close a socket"
    },
    {/* STL_ERRCODE_SOCKET_SHUTDOWN */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "fail to shutdown a socket"
    },
    {/* STL_ERRCODE_SOCKET */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "fail to create a socket"
    },
    {/* STL_ERRCODE_GETFD */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "fail to get property of a file descriptor"
    },
    {/* STL_ERRCODE_SETFD */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "fail to set property of a file descriptor"
    },
    {/* STL_ERRCODE_BIND */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "fail to bind a socket to address"
    },
    {/* STL_ERRCODE_LISTEN */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "fail to listen on a socket"
    },
    {/* STL_ERRCODE_ACCEPT */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "fail to accept a connection request with a socket"
    },
    {/* STL_ERRCODE_INTR */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "system call was interrupted by a signal"
    },
    {/* STL_ERRCODE_GETSOCKOPT */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "fail to get options of a socket"
    },
    {/* STL_ERRCODE_CONNECT */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "fail to connect to an host with a socket"
    },
    {/* STL_ERRCODE_POLL */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "fail to wait for some file descriptors with poll()"
    },
    {/* STL_ERRCODE_INTERNAL */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "internal error"
    },
    {/* STL_ERRCODE_SENDMSG */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "sendmsg() system call returned failure"
    },
    {/* STL_ERRCODE_RECVMSG */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "recvmsg() system call returned failure"
    },
    {/* STL_ERRCODE_SOCKETPAIR */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "socketpair() system call returned failure"
    },
    {/* STL_ERRCODE_OUT_OF_RANGE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "converted value is out of range (overflow or underflow)"
    },
    {/* STL_ERRCODE_STRING_IS_NOT_NUMBER */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "given string is not number"
    },
    {/* STL_ERRCODE_FILE_BAD */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "not a valid file desciptor"
    },
    {/* STL_ERRCODE_AGAIN */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "resource temporarily unavailable"
    },
    {/* STL_ERRCODE_ADDR_INUSE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "given address is already in use"
    },
    {/* STL_ERRCODE_INITIALIZE_CONTEXT */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "fail to initialize communication context"
    },
    {/* STL_ERRCODE_FINALIZE_CONTEXT */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "fail to finalize communication context"
    },
    {/* STL_ERRCODE_CONNECT_CONTEXT */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "fail to connect communication context"
    },
    {/* STL_ERRCODE_POLL_CONTEXT */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "fail to poll communication context"
    },
    {/* STL_ERRCODE_WRITE_PACKET */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "fail to write packet through communication context"
    },
    {/* STL_ERRCODE_READ_PACKET */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "fail to read packet through communication context"
    },
    {/* STL_ERRCODE_SEND_CONTEXT */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "fail to send descriptor through communication context"
    },
    {/* STL_ERRCODE_RECV_CONTEXT */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "fail to receive descriptor through communication context"
    },
    {/* STL_ERRCODE_RECV_CONTEXT */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "fail to get current file path"
    },
    {/* STL_ERRCODE_OPEN_LIBRARY */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "fail to open library (%s)"
    },
    {/* STL_ERRCODE_CLOSE_LIBRARY */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "fail to close library"
    },
    {/* STL_ERRCODE_GET_SYMBOL */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "fail to get symbol address (%s)"
    },
    {/* STL_ERRCODE_PARSER_NOTSUPPORT */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "not supported query"
    },
    {/* STL_ERRCODE_FILE_EXIST */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "fail to check file (%s)"
    },
    {/* STL_ERRCODE_PARSER_INPUT_TOO_LONG */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "input is too long"
    },
    {/* STL_ERRCODE_PARSER_SYSTEM_CALL_ERROR */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "system call error (function : %s, error no : %d)"
    },
    {/* STL_ERRCODE_PARSER_DIVIDE_ZERO */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "divide zero"
    },
    {/* STL_ERRCODE_SEM_OPEN */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to open a semaphore"
    },
    {/* STL_ERRCODE_SEM_CLOSE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to close the given semaphore"
    },
    {/* STL_ERRCODE_SEM_UNLINK */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to unlink the given semaphore"
    },
    {/* STL_ERRCODE_ACCESS_FILE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "permission denied - '%s'"
    },
    {/* STL_ERRCODE_OS_NOTSUPPORT */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "not supported os"
    },
    {/* STL_ERRCODE_UNDEFINED_ENV_VARIABLE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Environment Variable \"%s\" is not defined."
    },
    {/* STL_ERRCODE_NOT_EXIST_LICENSE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "License file(%s) is not exist"
    },
    {/* STL_ERRCODE_LICENSE_OUT_OF_DATE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "License is out of date. License is valid after %4d-%02d-%02d"
    },
    {/* STL_ERRCODE_EXPIRED_LICENSE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "License date is expired. License is expired in %4d-%02d-%02d"
    },
    {/* STL_ERRCODE_MISMATCH_CORECOUNT */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "License core count mismatch."
    },
    {/* STL_ERRCODE_INVALID_LICENSE_KEY */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "License key is corrupted."
    },
    {/* STL_ERRCODE_MISMATCH_HOSTNAME */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "License host name mismatch."
    },
    {/* STL_ERRCODE_SET_THREAD_AFFINITY */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "failed to set thread affinity."
    },
    {/* STL_ERRCODE_CREATE_TIMER */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "failed to create timer."
    },
    {/* STL_ERRCODE_SET_TIMER */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "failed to set timer."
    },
    {/* STL_ERRCODE_DESTROY_TIMER */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "failed to destroy timer."
    },
    {/* STL_ERRCODE_ADDR_NOTGIVEN */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "address is not given."
    },
    {/* STL_ERRCODE_MSGQ_ACCESS */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to access the message queue."
    },
    {/* STL_ERRCODE_MSGQ_EXIST */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "A message queue exists."
    },
    {/* STL_ERRCODE_MSGQ_CREATE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to create a message queue."
    },
    {/* STL_ERRCODE_MSGQ_REMOVED */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "The message queue was removed."
    },
    {/* STL_ERRCODE_MSGQ_DESTROY */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to destroy the message queue."
    },
    {/* STL_ERRCODE_MSGQ_SEND */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to send message."
    },
    {/* STL_ERRCODE_MSGQ_RECV */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to receive message."
    },
    {/* STL_ERRCODE_MSGQ_AGAIN */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "No message was available in the message queue."
    },
    {/* STL_ERRCODE_GETFL */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "fail to get property of a file status"
    },
    {/* STL_ERRCODE_SETFL */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "fail to set property of a file status"
    },
    {/* STL_ERRCODE_IBV_VERBS_FAILURE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "infiniband ibv/rdma interface returned error"
    },
    {/* STL_ERRCODE_NO_IB_DEVICE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "no infiniband device was found"
    },
    {/* STL_ERRCODE_CANNOT_FIND_IB_DEVICE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "cannot find specified infiniband device"
    },
    {/* STL_ERRCODE_CANNOT_FIND_MEMORY_REGION */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "cannot find suitable Memory Region for specified ptr and size"
    },
    {/* STL_ERRCODE_SEM_GET_VALUE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "fail to get semaphore value"
    },
    {/* STL_ERRCODE_POLLSET_CTRL */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "fail to control pollset"
    },
    {/* STL_ERRCODE_POLLSET_CREATE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "fail to create pollset"
    },
    {/* STL_ERRCODE_OVERFLOW_FD */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "overflow fd"
    },
    {/* STL_ERRCODE_INVALID_INI_FILE_FORMAT */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "invalid INI file format"
    },
    {
        0,
        NULL
    }
};

/**
 * @brief 레이어별 에러 테이블 배열.
 * 각 레이어별 에러 테이블 주소가 저장된다.
 */
stlErrorRecord * gLayerErrorTables[STL_ERROR_MODULE_MAX];

/**
 * @brief 에러 테이블을 등록한다.
 * @param[in] aErrorModule 등록할 에러 테이블을 소유한 레이어 클래스
 * @param[in] aErrorTable 등록할 에러 테이블
 */
void stlRegisterErrorTable( stlErrorModule   aErrorModule,
                            stlErrorRecord * aErrorTable )
{
    gLayerErrorTables[aErrorModule] = aErrorTable;
}

/**
 * @brief 에러를 에러 스택에 푸쉬한다.
 * @param[in] aErrorLevel 푸쉬될 에러의 에러레벨
 * @param[in] aErrorCode 푸쉬될 에러의 에러코드
 * @param[in] aDetailErrorMessage 푸쉬될 에러의 자세한 에러 메세지
 * @param[in] aErrorStack 푸쉬될 에러스택 포인터
 * @param[in] ... stlErrorData.mErrorMessage 포맷에 사용될 파라미터 리스트
 * @remark 만약 스택 Top이 정해진 크기를 넘어서는 경우는 최신에러를 보존하기 위해서
 *         기존 에러를 POP 한다.
 */
void stlPushError( stlErrorLevel     aErrorLevel,
                   stlUInt32         aErrorCode,
                   stlChar         * aDetailErrorMessage,
                   stlErrorStack   * aErrorStack,
                   ... )
{
    stlErrorData   * sErrorData;
    va_list          sVaList;
    stlUInt32        sErrorModule;
    stlUInt32        sErrorValue;
    stlErrorRecord * sErrorTable;
    stlChar        * sErrorMessage;
    stlInt32         i;

    if( (aErrorStack->mTop + 1) == STL_MAX_ERROR_STACK_DEPTH )
    {
        for( i = 1; i <= aErrorStack->mTop; i++ )
        {
            stlMemcpy( &aErrorStack->mErrorData[i-1],
                       &aErrorStack->mErrorData[i],
                       STL_SIZEOF( stlErrorData ) );
        }

        aErrorStack->mTop--;
    }
    
    sErrorData = &aErrorStack->mErrorData[aErrorStack->mTop + 1];

    sErrorData->mErrorLevel = aErrorLevel;
    sErrorData->mExternalErrorCode = stlToExternalErrorCode( aErrorCode );
    sErrorData->mErrorCode = aErrorCode;
    sErrorData->mSystemErrorCode = 0;
    sErrorData->mDetailErrorMessage[0] = '\0';

    if( aDetailErrorMessage != NULL )
    {
        (void)stlStrncpy( sErrorData->mDetailErrorMessage,
                          aDetailErrorMessage,
                          STL_MAX_ERROR_MESSAGE );
    }

    sErrorModule = STL_GET_ERROR_LAYER( aErrorCode );
    sErrorValue  = STL_GET_ERROR_VALUE( aErrorCode );

    sErrorTable = gLayerErrorTables[sErrorModule];
    sErrorMessage = sErrorTable[sErrorValue].mErrorMessage;
    
    va_start( sVaList, aErrorStack );
    stlVsnprintf( sErrorData->mErrorMessage,
                  STL_MAX_ERROR_MESSAGE,
                  sErrorMessage,
                  sVaList );
    va_end(sVaList);

    if( (aErrorStack->mTop + 1) == 0 )
    {
#ifdef STL_DEBUG
        aErrorStack->mCallStackFrameCount = stlBacktrace( aErrorStack->mErrorCallStack,
                                                          STL_MAX_ERROR_CALLSTACK_DEPTH,
                                                          NULL,
                                                          NULL );
#endif
    }
    
    aErrorStack->mTop++;
}

/**
 * @brief Fatal Error Callstack을 저장한다.
 * @param[in] aSigInfo Callstack을 추출하기 위한 Signal 정보
 * @param[in] aContext Callstack을 추출하기 위한 Context 정보
 * @param[in] aErrorStack 푸쉬될 에러스택 포인터
 * @remark 만약 스택 Top이 정해진 크기를 넘어서는 경우는 최신에러를 보존하기 위해서
 *         기존 에러를 POP 한다.
 */
void stlPushFatalErrorCallstack( void            * aSigInfo,
                                 void            * aContext,
                                 stlErrorStack   * aErrorStack )
{
    stlErrorData   * sErrorData;
    stlUInt32        sErrorModule;
    stlUInt32        sErrorValue;
    stlErrorRecord * sErrorTable;
    stlChar        * sErrorMessage;
    stlInt32         i;

    if( (aErrorStack->mTop + 1) == STL_MAX_ERROR_STACK_DEPTH )
    {
        for( i = 1; i <= aErrorStack->mTop; i++ )
        {
            stlMemcpy( &aErrorStack->mErrorData[i-1],
                       &aErrorStack->mErrorData[i],
                       STL_SIZEOF( stlErrorData ) );
        }

        aErrorStack->mTop--;
    }

    sErrorData = &aErrorStack->mErrorData[aErrorStack->mTop + 1];

    sErrorData->mErrorLevel = STL_ERROR_LEVEL_FATAL;
    sErrorData->mErrorCode = STL_ERRCODE_INTERNAL;
    sErrorData->mExternalErrorCode = stlToExternalErrorCode( STL_ERRCODE_INTERNAL );
    sErrorData->mSystemErrorCode = 0;
    sErrorData->mDetailErrorMessage[0] = '\0';

    sErrorModule = STL_GET_ERROR_LAYER( STL_ERRCODE_INTERNAL );
    sErrorValue  = STL_GET_ERROR_VALUE( STL_ERRCODE_INTERNAL );

    sErrorTable = gLayerErrorTables[sErrorModule];
    sErrorMessage = sErrorTable[sErrorValue].mErrorMessage;
    
    stlStrncpy( sErrorData->mErrorMessage,
                sErrorMessage,
                STL_MAX_ERROR_MESSAGE );

    if( (aErrorStack->mTop + 1) == 0 )
    {
        aErrorStack->mCallStackFrameCount = stlBacktrace( aErrorStack->mErrorCallStack,
                                                          STL_MAX_ERROR_CALLSTACK_DEPTH,
                                                          aSigInfo,
                                                          aContext );
    }
    
    aErrorStack->mTop++;
}

/**
 * @brief 에러 데이터를 에러 스택에 푸쉬한다.
 * @param[in] aErrorData 푸쉬될 에러 데이터 포인터
 * @param[in] aErrorStack 에러스택 포인터
 * @remark 만약 스택 Top이 정해진 크기를 넘어서는 경우는 최신에러를 보존하기 위해서
 *         기존 에러를 POP 한다.
 */
void stlPushErrorData( stlErrorData  * aErrorData,
                       stlErrorStack * aErrorStack )
{
    stlErrorData * sErrorData;

    stlInt32         i;

    if( (aErrorStack->mTop + 1) == STL_MAX_ERROR_STACK_DEPTH )
    {
        for( i = 1; i <= aErrorStack->mTop; i++ )
        {
            stlMemcpy( &aErrorStack->mErrorData[i-1],
                       &aErrorStack->mErrorData[i],
                       STL_SIZEOF( stlErrorData ) );
        }

        aErrorStack->mTop--;
    }
    
    sErrorData = &aErrorStack->mErrorData[aErrorStack->mTop + 1];
    
    stlMemcpy( sErrorData, aErrorData, STL_SIZEOF( stlErrorData ) );

#ifdef STL_DEBUG
    if( (aErrorStack->mTop + 1) == 0 )
    {
        aErrorStack->mCallStackFrameCount = stlBacktrace( aErrorStack->mErrorCallStack,
                                                          STL_MAX_ERROR_CALLSTACK_DEPTH,
                                                          NULL,
                                                          NULL );
    }
#endif
    
    aErrorStack->mTop++;
}

/**
 * @brief 주어진 에러 스택에서 한개의 에러를 팝한다.
 * @param[in] aErrorStack 팝할 에러스택 포인터
 * @return 팝된 에러 포인터
 * @remark 만약 스택이 비어있는 경우 NULL을 리턴한다.
 */
stlErrorData * stlPopError( stlErrorStack * aErrorStack )
{
    stlErrorData * sErrorData;

    if( aErrorStack == NULL )
    {
        sErrorData = NULL;
    }
    else
    {
        if( aErrorStack->mTop < 0 )
        {
            sErrorData = NULL;
        }
        else
        {
            sErrorData = &aErrorStack->mErrorData[aErrorStack->mTop];
            aErrorStack->mTop--;
        }
    }

    return sErrorData;
}

/**
 * @brief 주어진 에러 스택이 Warning으로 처리되어야 하는 Stack인지 반환한다.
 * @param[in] aErrorStack 팝할 에러스택 포인터
 * @return Warning Stack이라면 STL_TRUE, 그렇지 않으면 STL_FALSE
 */
stlBool stlHasWarningError( stlErrorStack * aErrorStack )
{
    stlErrorData * sErrorData;
    
    sErrorData = &aErrorStack->mErrorData[aErrorStack->mTop];
    
    return (sErrorData->mErrorLevel == STL_ERROR_LEVEL_WARNING) ? STL_TRUE : STL_FALSE;
}

/**
 * @brief Source 에러 스택의 에러를 Target 에러 스택에 추가한다.
 * @param[in,out] aDestErrorStack 추가될 target 에러 스택
 * @param[in] aSrcErrorStack 복사할 source 에러 스택
 */
void stlAppendErrors( stlErrorStack * aDestErrorStack,
                      stlErrorStack * aSrcErrorStack )
{
    stlInt32       i;
    stlErrorData * sErrorData;

    if( ( aSrcErrorStack  != NULL ) &&
        ( aDestErrorStack != NULL ) )
    {
        if( aDestErrorStack->mTop == -1 )
        {
            for( i = 0; i <= aSrcErrorStack->mTop; i++ )
            {
                sErrorData = &aSrcErrorStack->mErrorData[i];
                stlPushErrorData( sErrorData, aDestErrorStack );
            }

#ifdef STL_DEBUG
            aDestErrorStack->mCallStackFrameCount = aSrcErrorStack->mCallStackFrameCount;
            stlMemcpy( aDestErrorStack->mErrorCallStack,
                       aSrcErrorStack->mErrorCallStack,
                       STL_MAX_ERROR_CALLSTACK_DEPTH * STL_SIZEOF(void*) );
#endif
        }
        else
        {
            for( i = 0; i <= aSrcErrorStack->mTop; i++ )
            {
                sErrorData = &aSrcErrorStack->mErrorData[i];
                stlPushErrorData( sErrorData, aDestErrorStack );
            }
        }
    }
}

/**
 * @brief 주어진 에러 스택에서 마지막 에러를 반환한다.
 * @param[in] aErrorStack 에러스택 포인터
 * @return 마지막 에러 데이터 포인터
 * @remark 만약 스택이 비어있는 경우 NULL을 리턴한다.
 */
stlErrorData * stlGetLastErrorData( stlErrorStack * aErrorStack )
{
    stlErrorData * sErrorData;

    if( aErrorStack == NULL )
    {
        sErrorData = NULL;
    }
    else
    {
        if( aErrorStack->mTop < 0 )
        {
            sErrorData = NULL;
        }
        else
        {
            sErrorData = &aErrorStack->mErrorData[aErrorStack->mTop];
        }
    }

    return sErrorData;
}

/**
 * @brief 주어진 에러 스택에서 처음 에러를 반환한다.
 * @param[in] aErrorStack 에러스택 포인터
 * @param[in,out] aErrorIdx 현재 error 위치
 * @remark 만약 스택이 비어있는 경우 NULL을 리턴한다.
 */
stlErrorData * stlGetFirstErrorData( stlErrorStack * aErrorStack,
                                     stlInt16      * aErrorIdx  )
{
    stlErrorData * sErrorData;
    
    *aErrorIdx = -1;

    if( aErrorStack == NULL )
    {
        sErrorData = NULL;
    }
    else
    {
        if( aErrorStack->mTop < 0 )
        {
            sErrorData = NULL;
        }
        else
        {
            sErrorData = &aErrorStack->mErrorData[0];
        }

        *aErrorIdx = 0;
    }

    return sErrorData;
}

/**
 * @brief 주어진 에러 스택에서 Next 에러를 반환한다.
 * @param[in] aErrorStack 에러스택 포인터
 * @param[in,out] aErrorIdx 현재 error 위치
 * @remark 만약 스택이 비어있는 경우 NULL을 리턴한다.
 */
stlErrorData * stlGetNextErrorData( stlErrorStack * aErrorStack,
                                    stlInt16      * aErrorIdx  )
{
    stlErrorData * sErrorData;
    stlInt16       sErrorIdx = -1;

    if( aErrorStack == NULL )
    {
        sErrorData = NULL;
    }
    else
    {
        sErrorIdx = *aErrorIdx + 1;
        
        if( aErrorStack->mTop < sErrorIdx )
        {
            sErrorData = NULL;
        }
        else
        {
            sErrorData = &aErrorStack->mErrorData[sErrorIdx];
        }

        *aErrorIdx = sErrorIdx;
    }

    return sErrorData;
}

/**
 * @brief 주어진 에러 스택의 에러 데이터 개수를 반환한다.
 * @param[in] aErrorStack 에러스택 포인터
 * @return 에러 데이터 개수
 */
stlInt32 stlGetErrorStackDepth( stlErrorStack * aErrorStack )
{
    if( aErrorStack == NULL )
    {
        return 0;
    }

    return aErrorStack->mTop + 1;
}

/**
 * @brief 주어진 Failure를 유발하는 에러 데이터의 개수를 반환한다.
 * @param[in] aErrorStack 에러스택 포인터
 * @return 에러 데이터 개수
 */
stlInt32 stlGetFailureErrorCount( stlErrorStack * aErrorStack )
{
    stlInt32 sErrorCount = 0;
    stlInt32 i;
    
    if( aErrorStack != NULL )
    {
        for( i = 0; i <= aErrorStack->mTop; i++ )
        {
            if( aErrorStack->mErrorData[i].mErrorLevel < STL_ERROR_LEVEL_WARNING )
            {
                sErrorCount++;
            }
        }
    }
    
    return sErrorCount;
}

/**
 * @brief 주어진 내부 에러를 외부 에러로 변한한다.
 * @param[in] aInternalErrorCode 대상 내부 에러 코드
 * @return 외부 에러 코드
 */
stlUInt32 stlToExternalErrorCode( stlUInt32 aInternalErrorCode )
{
    stlUInt32        sErrorModule;
    stlUInt32        sErrorValue;
    stlUInt32        sExternalErrorCode = STL_INVALID_ERROR_CODE;
    stlErrorRecord * sErrorTable;

    if( aInternalErrorCode != STL_INVALID_ERROR_CODE )
    {
        sErrorModule = STL_GET_ERROR_LAYER( aInternalErrorCode );
        sErrorValue  = STL_GET_ERROR_VALUE( aInternalErrorCode );

        sErrorTable = gLayerErrorTables[sErrorModule];
        sExternalErrorCode = sErrorTable[sErrorValue].mExternalErrorCode;
    }
    
    return sExternalErrorCode;
}

/**
 * @brief 주어진 에러 데이터의 외부 에러를 변한한다.
 * @param[in] aErrorData Error Data Pointer
 * @return 외부 에러 코드
 */
stlUInt32 stlGetExternalErrorCode( stlErrorData * aErrorData )
{
    return aErrorData->mExternalErrorCode;
}

/**
 * @brief 주어진 외부 에러 코드에 해당하는 SQLSTATE를 반환한다.
 * @param[in]  aExternalErrorCode  외부 에러 코드
 * @param[out] aSqlState           변환된 SQL STATE
 * @return SQLSTATE
 */
void stlMakeSqlState( stlUInt32   aExternalErrorCode,
                      stlChar   * aSqlState )
{
    stlInt32 i = 0;

    if( aExternalErrorCode != STL_INVALID_ERROR_CODE )
    {
        for( i = 0; i < 5; i++ )
        {
            aSqlState[i] = STL_MAKE_UNSIXBIT( aExternalErrorCode );
            aExternalErrorCode >>= 6;
        }
    }

    aSqlState[i] = '\0';
}

/**
 * @brief 스택에 푸쉬된 에러중 마지막 에러의 에러 코드를 얻는다.
 * @param[in] aErrorStack 대상 에러스택 포인터
 * @return 마지막 에러의 에러 코드
 * @remark 만약 스택이 비어있는 경우 STL_INVALID_ERROR_CODE를 리턴한다.
 */
stlUInt32 stlGetLastErrorCode( stlErrorStack * aErrorStack )
{
    stlErrorData * sErrorData;
    stlUInt32      sErrorCode;

    if( aErrorStack == NULL )
    {
        sErrorCode = STL_INVALID_ERROR_CODE;
    }
    else
    {
        if( aErrorStack->mTop < 0 )
        {
            sErrorCode = STL_INVALID_ERROR_CODE;
        }
        else
        {
            sErrorData = &aErrorStack->mErrorData[aErrorStack->mTop];
            sErrorCode = sErrorData->mErrorCode;
        }
    }

    return sErrorCode;
}

/**
 * @brief 스택에 푸쉬된 에러중 마지막 에러의 시스템 에러 코드를 얻는다.
 * @param[in] aErrorStack 대상 에러스택 포인터
 * @return 마지막 에러의 에러 코드
 * @remark 만약 스택이 비어있는 경우 STL_INVALID_ERROR_CODE를 리턴한다.
 */
stlUInt32 stlGetLastSystemErrorCode( stlErrorStack * aErrorStack )
{
    stlErrorData * sErrorData;
    stlUInt32      sErrorCode;

    if( aErrorStack == NULL )
    {
        sErrorCode = STL_INVALID_ERROR_CODE;
    }
    else
    {
        if( aErrorStack->mTop < 0 )
        {
            sErrorCode = STL_INVALID_ERROR_CODE;
        }
        else
        {
            sErrorData = &aErrorStack->mErrorData[aErrorStack->mTop];
            sErrorCode = sErrorData->mSystemErrorCode;
        }
    }

    return sErrorCode;
}

/**
 * @brief 스택에 푸쉬된 에러중 마지막 에러의 에러 메세지를 얻는다.
 * @param[in] aErrorStack 대상 에러스택 포인터
 * @return 마지막 에러의 에러 메세지
 * @remark 만약 스택이 비어있는 경우 NULL을 리턴한다.
 */
stlChar * stlGetLastErrorMessage( stlErrorStack * aErrorStack )
{
    stlErrorData * sErrorData;
    stlChar      * sErrorMessage;

    if( aErrorStack == NULL )
    {
        sErrorMessage = NULL;
    }
    else
    {
        if( aErrorStack->mTop < 0 )
        {
            sErrorMessage = NULL;
        }
        else
        {
            sErrorData = &aErrorStack->mErrorData[aErrorStack->mTop];
            sErrorMessage = sErrorData->mErrorMessage;
        }
    }

    return sErrorMessage;
}

/**
 * @brief 스택에 푸쉬된 에러중 마지막 에러의 자세한 에러 메세지를 얻는다.
 * @param[in] aErrorStack 대상 에러스택 포인터
 * @return 마지막 에러의 자세한 에러 메세지
 * @remark 만약 스택이 비어있는 경우 NULL을 리턴한다.
 */
stlChar * stlGetLastDetailErrorMessage( stlErrorStack * aErrorStack )
{
    stlErrorData * sErrorData;
    stlChar      * sErrorMessage;

    if( aErrorStack == NULL )
    {
        sErrorMessage = NULL;
    }
    else
    {
        if( aErrorStack->mTop < 0 )
        {
            sErrorMessage = NULL;
        }
        else
        {
            sErrorData = &aErrorStack->mErrorData[aErrorStack->mTop];
            sErrorMessage = sErrorData->mDetailErrorMessage;
        }
    }

    return sErrorMessage;
}

/**
 * @brief 스택에 푸쉬된 에러중 마지막 에러에 대해 자세한 에러 메세지를 설정한다.
 * @param[in] aErrorStack 대상 에러스택 포인터
 * @param[in] aDetailErrorMessage Error Message
 * @remark 만약 스택이 비어있다면 아무런 처리를 하지 않는다.
 */
void stlSetLastDetailErrorMessage( stlErrorStack   * aErrorStack,
                                   stlChar         * aDetailErrorMessage )
{
    stlErrorData * sErrorData;

    if( aErrorStack == NULL )
    {
        /* nothing to do */
    }
    else
    {
        if( aErrorStack->mTop < 0 )
        {
            /* nothing to do */
        }
        else
        {
            sErrorData = &aErrorStack->mErrorData[aErrorStack->mTop];
            sErrorData->mDetailErrorMessage[0] = '\0';
            
            if( aDetailErrorMessage != NULL )
            {
                (void)stlStrncpy( sErrorData->mDetailErrorMessage,
                                  aDetailErrorMessage,
                                  STL_MAX_ERROR_MESSAGE );
            }
        }
    }
}

/**
 * @brief 기본 Fatal Handler를 자신의 Handler로 대체한다.
 * @param[in] aNewHandler 새로운 Fatal Handler
 * @param[out] aOldHandler 기존 Fatal Handler
 * @note Hook를 시킨 Layer는 반드시 자신의 Fatal Handling 이후에
 * <BR> Old Fatal Handler를 호출할 책임이 있다.
 * <BR> 이는 Layer마다 고유한 Fatal Hanlder가 존재하고, Fatal 상황에서
 * <BR> 모든 Fatal Handler들이 호출되어야 하기 때문이다.
 * <BR> 결국 Layer마다 자신의 Fatal Handling이후에 Old Handler를 호출해 준다면,
 * <BR> Hooking을 호출한 순서의 역순으로 Fatal Handling이 처리된다.
 */
stlStatus stlHookFatalHandler( stlFatalHandler   aNewHandler,
                               stlFatalHandler * aOldHandler )
{
    *aOldHandler = gStlFatalHandler;
    gStlFatalHandler = aNewHandler;

    return STL_SUCCESS;
}

/**
 * @brief 코드의 ASSERT Validation 처리
 * @param[in] aFormat Assert Format
 */
void stlAssert( const stlChar * aFormat,
                ... )
{
    stlChar sBuffer[STL_MAX_LARGE_ERROR_MESSAGE];
    va_list sVarArgList;

    va_start( sVarArgList, aFormat );
    stlVsnprintf( sBuffer,
                  STL_MAX_LARGE_ERROR_MESSAGE,
                  aFormat,
                  sVarArgList );
    va_end( sVarArgList );

    gStlFatalHandler( sBuffer, NULL, NULL );
}

/**
 * @brief (테스트 용도임) Error Stack의 내용을 출력한다.
 * @param[in] aErrorStack    출력할 에러스택 주소
 * @remarks
 *     테스트 용도이므로 제품내에서 사용되면 안되는 함수임.
 */
void stlTestPrintErrorStack( stlErrorStack * aErrorStack )
{
    stlInt32       i;
    stlErrorData * sErrorData;

    if( aErrorStack != NULL )
    {
        for( i = aErrorStack->mTop; i >= 0; i-- )
        {
            sErrorData = &aErrorStack->mErrorData[i];
            stlPrintf( "------------------------------------------------------\n" );
            stlPrintf( "Error level           : %d\n", sErrorData->mErrorLevel );
            stlPrintf( "Internal error code   : %d\n", sErrorData->mErrorCode );
            stlPrintf( "System error code     : %d\n", sErrorData->mSystemErrorCode );
            stlPrintf( "Primary Error message : %s\n", sErrorData->mErrorMessage );
            stlPrintf( "Detail Error message  : %s\n", sErrorData->mDetailErrorMessage );
            stlPrintf( "------------------------------------------------------\n" );
            stlPrintf( "\n" );
        }
    }
}

/**
 * @brief 최초 발생된 에러의 콜스택을 stdout으로 출력한다.
 * @param[in] aErrorStack 출력할 에러스택 주소
 */
void stlPrintFirstErrorCallstack( stlErrorStack * aErrorStack )
{
    stlFile sOutFile;

    if( aErrorStack != NULL )
    {
        if( aErrorStack->mTop >= 0 )
        {
            stlOpenStdoutFile( &sOutFile, NULL );
            stlBacktraceSymbolsToFile( aErrorStack->mErrorCallStack,
                                       aErrorStack->mCallStackFrameCount,
                                       &sOutFile );
        }
    }
}

/**
 * @brief 최초 발생된 에러의 콜스택을 주어진 aBuffer로 출력한다.
 * @param[in]  aErrorStack 출력할 에러스택 주소
 * @param[out] aBuffer     콜스택을 저장할 버퍼 공간
 * @param[in]  aBufferSize aBuffer의 크기
 */
void stlPrintFirstErrorCallstackToStr( stlErrorStack * aErrorStack,
                                       stlChar       * aBuffer,
                                       stlInt32        aBufferSize )
{
    if( aErrorStack != NULL )
    {
        if( aErrorStack->mTop >= 0 )
        {
            stlBacktraceSymbolsToStr( aErrorStack->mErrorCallStack,
                                      aBuffer,
                                      aErrorStack->mCallStackFrameCount,
                                      aBufferSize );
        }
    }
}


/**
 * @brief 주어진 에러 스택에 입력 에러 코드와 일치하는 에러가 있는지 확인한다. 
 * @param[in] aErrorStack 대상 에러스택 포인터
 * @param[in] aInternalErrorCode 대상 내부 에러 코드
 * @remark 만약 스택이 비어있다면 FALSE를 반환한다.
 */
stlBool stlCheckErrorCode( stlErrorStack  * aErrorStack,
                           stlUInt32        aInternalErrorCode )
{
    stlErrorData  * sErrorData;
    stlInt16        sErrorIdx = 0;

    if( aErrorStack == NULL )
    {
        return STL_FALSE;
    }
    else
    {
        sErrorData = stlGetFirstErrorData( aErrorStack, & sErrorIdx );

        while (sErrorData != NULL )
        {
            if ( sErrorData->mErrorCode == aInternalErrorCode )
            {
                return STL_TRUE;
            }
            else
            {
                sErrorData = stlGetNextErrorData( aErrorStack, & sErrorIdx );
            }
        }
    }

    return STL_FALSE;
}

/** @} */
