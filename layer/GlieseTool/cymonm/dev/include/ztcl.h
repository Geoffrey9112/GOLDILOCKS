/*******************************************************************************
 * ztcl.h
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

#ifndef __ZTCL_H__
#define __ZTCL_H__ 1

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/**
 * @file ztcl.h
 * @brief GlieseTool Cyclone library interface
 */

/**
 * @defgroup ztcl Cyclone library interface
 * @{
 */

/**
 * API구조가 변경되면 버전을 올려야 함.
 * 잘못된 버전의 library가 link되지 않도록 check를 위함
 */
#define ZTCL_API_VERSION                "20151126"


#define ZTCL_LIB_LAST_VALUE_QUEUE       "0"

/**
 * ZTCL_MONITOR_STATE_SIZE가 512가 아니면 고려해야 할 사항이 많아짐.
 * disk i/o가 최소 512 단위임으로 512가 넘어가면 write/read 동기화 관련 고민을 해야 함.
 */
#define ZTCL_MONITOR_STATE_SIZE         (512)
#define ZTCL_MAX_MONITOR_QUEUE_CNT      (32)

typedef enum ztclStatus
{
    ZTCL_SUCCESS = 0,
    ZTCL_FAILURE,
    ZTCL_ERROR_MALLOC,
    ZTCL_ERROR_QUEUE_FULL,
    ZTCL_ERROR_BUFFER_OVERFLOW,
    ZTCL_ERROR_LIBRARY_CONFIG,
    ZTCL_ERROR_LIBRARY_PARSE_CONFIG,
    ZTCL_ERROR_LIBRARY_CONFIG_OVER_FLOW,
    ZTCL_ERROR_LIBRARY_CONFIG_MAX_SECTION,
    ZTCL_ERROR_LIBRARY_CONFIG_MAX_ITEM,
    ZTCL_ERROR_GET_HOST_NAME,
    ZTCL_ERROR_UNLINK
} ztclStatus;

typedef enum ztclEventType
{
    ZTCL_EVENT_QUEUE_SPOOL_FULL,
    ZTCL_EVENT_SEND_NACK,
    ZTCL_EVENT_RECV_MSG_ERROR,
    ZTCL_EVENT_ERROR,
    ZTCL_EVENT_MAX
} ztclEventType;


typedef struct ztclRecvData
{
    char      * mBufArray[128];
    int         mSizeArray[128];
    uint64_t    mMsgId[128];
    int         mMsgCnt;
    int         mDataSize;
} ztclRecvData;


typedef void ztclPublisher;
typedef void ztclSubscriber;
typedef void ztclMonitor;

typedef char * (*ztclFuncGetVersion)();

typedef void (*ztclFuncLogMsg)( const char  * aLog );

typedef ztclStatus (*ztclFuncRecvCallback)( ztclSubscriber * aContext,
                                            ztclRecvData   * aRecvData,
                                            const char     * aDataType,
                                            void           * aUser );

typedef ztclStatus (*ztclFuncEventCallback)( ztclEventType   aEventType,
                                             void          * aData,
                                             void          * aUser );


typedef ztclStatus (*ztclFuncInitializeSender)( char                 * aConfig,
                                                char                 * aGroupName,
                                                char                   aReset,
                                                ztclFuncLogMsg         aFuncLogMsg,
                                                ztclFuncEventCallback  aEventCallback,
                                                void                 * aEventUser,
                                                ztclPublisher       ** aContext );

typedef ztclStatus (*ztclFuncPubData)( ztclPublisher     * aContext,
                                       char              * aData,
                                       size_t              aLen,
                                       int                 aTrSeq,
                                       int                 aEnd );

typedef ztclStatus (*ztclFuncFinalizeSender)( ztclPublisher  * aContext );

typedef ztclStatus (*ztclFuncSaveData)( ztclPublisher     * aContext,
                                        void              * aData,
                                        size_t              aLen );

typedef ztclStatus (*ztclFuncLoadData)( ztclPublisher     * aContext,
                                        void              * aData,
                                        size_t            * aLen );

typedef ztclStatus (*ztclFuncDeleteSenderQueue)( ztclPublisher   * aContext,
                                                 char            * aGroupName );

typedef ztclStatus (*ztclFuncInitializeReceiver)( char                 * aConfigFile,
                                                  char                 * aGroupName,
                                                  char                   aReset,
                                                  ztclFuncLogMsg         aFuncLogMsg,
                                                  ztclFuncRecvCallback   aRecvCallback,
                                                  void                 * aRecvUser,
                                                  ztclFuncEventCallback  aEventCallback,
                                                  void                 * aEventUser,
                                                  char                   aStartState,    /* start state : 0 이면 pause 이다 */
                                                  ztclSubscriber      ** aContext );

typedef ztclStatus (*ztclFuncAckData)( ztclSubscriber * aContext,
                                       uint64_t         aMsgId );

typedef ztclStatus (*ztclFuncPauseReceiver)( ztclSubscriber     * aContext );

typedef ztclStatus (*ztclFuncResumeReceiver)( ztclSubscriber     * aContext );

typedef ztclStatus (*ztclFuncFinalizeReceiver)( ztclSubscriber     * aContext );


typedef ztclStatus (*ztclFuncDeleteReceiverQueue)( ztclSubscriber   * aContext,
                                                   char             * aGroupName );


typedef ztclStatus (*ztclFuncInitializeMonitor)( char              * aConfigFile,
                                                 ztclFuncLogMsg      aFuncLogMsg,
                                                 char              * aGroupList[ZTCL_MAX_MONITOR_QUEUE_CNT],
                                                 int                 aGroupCnt,
                                                 ztclMonitor      ** aContext );


typedef ztclStatus (*ztclFuncFinalizeMonitor)( ztclMonitor   * aContext );


typedef ztclStatus (*ztclFuncGetState)( ztclMonitor    * aContext,
                                        char             aState[ZTCL_MONITOR_STATE_SIZE] );


/** @} */

/** @} */

#endif

