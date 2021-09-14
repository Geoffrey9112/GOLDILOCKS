/*******************************************************************************
 * sll.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: sllDef.h 9428 2013-08-23 09:03:41Z mkkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/

#ifndef SLLDEF_H_
#define SLLDEF_H_

/**
 * @brief Server Library error code
 */

#define SLL_ERRCODE_NOT_SUPPORTED                STL_MAKE_ERRCODE( STL_ERROR_MODULE_SERVER_LIBRARY, 0 )
#define SLL_ERRCODE_SERVER_IS_NOT_RUNNING        STL_MAKE_ERRCODE( STL_ERROR_MODULE_SERVER_LIBRARY, 1 )
#define SLL_ERRCODE_UNDEFINED_ENV_VARIABLE       STL_MAKE_ERRCODE( STL_ERROR_MODULE_SERVER_LIBRARY, 2 )
#define SLL_ERRCODE_INVALID_PROTOCOL             STL_MAKE_ERRCODE( STL_ERROR_MODULE_SERVER_LIBRARY, 3 )
#define SLL_ERRCODE_STARTUP_MUST_DEDICATED       STL_MAKE_ERRCODE( STL_ERROR_MODULE_SERVER_LIBRARY, 4 )
#define SLL_ERRCODE_SESSION_KILLED               STL_MAKE_ERRCODE( STL_ERROR_MODULE_SERVER_LIBRARY, 5 )


#define SLL_MAX_ERROR       6



#define SLL_FETCH_COUNT_AUTO      ( 0 )
#define SLL_FETCH_COUNT_UNLIMITED ( -1 )

#define SLL_INIT_PROTOCOLSET_STACK( aStack )    \
{                                               \
    (aStack)->mTop = -1;                        \
}

#define SLL_ADD_PROTOCOLSET( aStack )                                       \
{                                                                           \
    STL_DASSERT( ((aStack)->mTop + 1) != CML_MAX_PROTOCOLSET_STACK_DEPTH ); \
                                                                            \
    (aStack)->mTop++;                                                       \
                                                                            \
    (aStack)->mHasError[(aStack)->mTop] = STL_FALSE;                        \
}

#define SLL_REMOVE_PROTOCOLSET( aStack )                    \
{                                                           \
    STL_DASSERT( (aStack)->mTop >= 0 );                     \
                                                            \
    (aStack)->mHasError[(aStack)->mTop] = STL_FALSE;        \
                                                            \
    (aStack)->mTop--;                                       \
}

#define SLL_SET_PROTOCOLSET_ERROR( aStack, aError ) \
{                                                   \
    STL_DASSERT( (aStack)->mTop >= 0 );             \
                                                    \
    (aStack)->mHasError[(aStack)->mTop] = (aError); \
}

#define SLL_GET_PROTOCOLSET_ERROR( aStack ) (aStack)->mHasError[(aStack)->mTop]

/*
 * CML_COMMAND_CONTROLPACKET protocol의 operation 들 정의
 */
#define SLL_CONTROL_PACKET_OP_POOLING    ( 1 )





/**
 * @brief Gliese Environment
 */
typedef struct sllEnv
{
    sslEnv              mEnv;           /**< Process Environment */

    knlDynamicMem       mAllocator;
} sllEnv;

typedef struct sllSessionEnv
{
    sslSessionEnv mEnv;     /**< Process Environment */
} sllSessionEnv;

#define SLL_ENV( aEnv )      ( (sllEnv *)aEnv )

/**
 * @brief  protocol을 관리하는 구조체
 * @remark shared mode에서는 이 부분이 shared memory로 관리되어 여러 shared server에서 공유하게 된다.
 *         life-cycle은 session이다. imlHandle에 포함되어 있다.\n
 *         stlContext의 실제 object는 sllHandle 밖에 존재한다. \n
 *         하지만 sclIpc는 shared-memory에 있어야 함으로 sllHandle에 포함되어 있다.
 *
 */
typedef struct sllHandle
{
    sclHandle               mSclHandle;         /**< 이 변수는 제일 위에 있어야 함 */

    stlInt64                mSessionSeq;
    stlUInt32               mSessionId;

    sclIpc                  mIpc;
    void                  * mUserContext;       /**< 상위 app(dispatcher)의 context */
    stlBool                 mDoneHandshake;     /**< handshake 했는지 여부 */
    cmlConnectStatus        mConnectStatus;     /**< CML_CONN_STATUS_CONNECTED 인 경우에 check timeout을 한다 */
    stlChar                 mRoleName[STL_MAX_SQL_IDENTIFIER_LENGTH + 1];
    cmlProtocolSetStack     mProtocolSetStack;
} sllHandle;

/**
 * @brief ProtocolSet별로 관리가 필요한 구조체
 * @remark life-cycle은 job이다.
 */
typedef struct sllProtocolSentence
{
    sclProtocolSentence      mSclProtocolSentence;  /**< 이 변수는 제일 위에 있어야 함 */

    stlBool                  mCloseDatabase;
    stlInt64                 mCachedStmtId;
} sllProtocolSentence;


/**
 * @brief sllHandle을 얻는 매크로
 */
#define SLL_HANDLE( aHandle )                   ((sllHandle*)(aHandle))

/**
 * @brief imlProtocolSentence을 얻는 매크로
 */
#define SLL_PROTOCOL_SENTENCE( aHandle )        ((sllProtocolSentence *)SCL_PROTOCOL_SENTENCE(aHandle))

/**
 * @brief ProtocolSetStack을 얻는 매크로
 */
#define SLL_PROTOCOL_STACK( aHandle )           (&SLL_HANDLE(aHandle)->mProtocolSetStack)


/**
 * performance view를 위한 상태 enum
 */
typedef enum sllStatus
{
    SLL_STATUS_WAIT,
    SLL_STATUS_SEND,
    SLL_STATUS_RECEIVE,
    SLL_STATUS_RUN
} sllStatus;


/**
 * @brief Balancer정보
 */
typedef struct sllBalancer
{
    void          * mEnv;               /**< Balancer 의 env */
    stlThread       mThread;            /**< Balancer의 main thread */
    stlInt64        mCurrentConnections;/**< Balancer의 Context count */
    stlInt64        mMaxConnections;    /**< Balancer의 Max Context count */
    stlInt64        mConnections;       /**< total context(socket) 개수 */
    stlInt64        mConnectionsHighwater;   /**< High water mark context(socket) 개수 */
    sllStatus       mStatus;            /**< status */
} sllBalancer;


/**
 * @brief shared server정보
 */
typedef struct sllSharedServer
{
    void          * mEnv;           /**< shared server 의 env */
    stlThread       mThread;        /**< shared server의 main thread */
    stlInt64        mJobCount;      /**< current job count */
    sllStatus       mStatus;        /**< process status */
    stlInt64        mIdle;          /**< idle time (1/100 second) */
    stlInt64        mBusy;          /**< busy time (1/100 second) */
} sllSharedServer;


/**
 * @brief dispatcher 정보
 * response용 queue는 dispatcher별로 있음.
 */
typedef struct sllDispatcher
{
    void          * mMainThreadEnv;     /**< dispatcher의 main thread env */
    void          * mSendThreadEnv;     /**< dispatcher의 send thread env */
    stlThread       mThread;            /**< dispatcher의 main thread */
    stlBool         mAccept;            /**< fd받을 준비가 되어 있는지 여부 */
    stlTime         mStartTime;         /**< dispatcher 시작 time */
    stlInt32        mSocketCount;       /**< 현재 관리하는 socket 개수 */
    sclQueue      * mResponseEvent;     /**< response를 위한 event queue */
    stlInt64        mConnections;       /**< total context(socket) 개수 */
    stlInt64        mConnectionsHighwater;   /**< High water mark context(socket) 개수 */
    stlInt64        mMaxConnections;    /**< 관리 가능한 최대 context(socket) 개수 */
    sllStatus       mRecvStatus;        /**< recv status */
    stlInt64        mTotalRecvBytes;    /**< total recv bytes */
    stlInt64        mTotalRecvUnits;    /**< total recv units */
    stlInt64        mRecvIdle;          /**< recv idle time (1/100 second) */
    stlInt64        mRecvBusy;          /**< recv busy time (1/100 second) */
    sllStatus       mSendStatus;        /**< send status */
    stlInt64        mTotalSendBytes;    /**< total send bytes */
    stlInt64        mTotalSendUnits;    /**< total send units */
    stlInt64        mSendIdle;          /**< send idle time (1/100 second) */
    stlInt64        mSendBusy;          /**< send busy time (1/100 second) */
} sllDispatcher;

/**
 * @brief maximum request queue count
 */
#define SLL_MAX_REQUEST_QUEUE_COUNT  ( KNL_PROPERTY_MAX( KNL_PROPERTY_SHARED_REQUEST_QUEUE_COUNT ) )

/**
 * @brief macro for getting own request queue from request queue pool
 */
#define SLL_REQUEST_GROUP_ID( aSharedMemory, aId )       \
    ( (aId) % (aSharedMemory)->mRequestQueueCount )

/**
 * @brief macro for getting own request queue from request queue pool
 */
#define SLL_REQUEST_QUEUE( aSharedMemory, aId )                           \
    ( (aSharedMemory)->mRequestEvent[SLL_REQUEST_GROUP_ID( aSharedMemory, aId )] )

/**
 * @brief InstanceManager의 WarmupEntry 임.
 * shared 모드를 위해서 listener, dispatcher, shared-server가 참조한다.
 */
typedef struct sllSharedMemory
{
    stlSize               mTotalSize;           /**< InstanceManager의 WarmupEntry 영역 크기 */
    stlSize               mNetBufferSize;       /**< imbStartupNoMount시 goldilocks.property.conf의 KNL_PROPERTY_NET_BUFFER_SIZE 값을 저장 */
    stlSize               mUnitSize;            /**< imbStartupNoMount시 goldilocks.property.conf의 KNL_PROPERTY_DISPATCHER_CM_UNIT_SIZE 값을 저장 */
    stlBool               mExitFlag;            /**< shared관련 process가 종료하도록 설정하는 flag */
    stlChar               mPadding[3];
    stlInt32              mMaxDispatchers;      /**< dispatcher max property  */
    stlInt32              mMaxSharedServers;    /**< shared server max property  */
    stlInt32              mDispatchers;         /**< dispatcher 개수 : property 값이 아닌 실제 동작중인 process 수임 */
    stlInt32              mSharedServers;       /**< shared server 개수 : property 값이 아닌 실제 동작중인 process 수임*/
    stlInt32              mConnections;         /**< dispatcher당 최대 connection 개수 */
    stlInt32              mRequestQueueCount;   /**< Request Queue 개수 */
    sllBalancer           mBalancer;            /**< Balancer 정보 */
    sllSharedServer     * mSharedServer;        /**< shared-server 개수만큼 할당 되는 pointer */
    sllDispatcher       * mDispatcher;          /**< for dispatcher 개수만큼 할당 되는 pointer */
    sclQueue            * mRequestEvent[SLL_MAX_REQUEST_QUEUE_COUNT];     /**< dispatcher들이 요청하는 ipc */
    knlMemController      mMemController;
    knlDynamicMem         mSessionAllocator;    /**< alloc imlHandle */
    stlArrayAllocator   * mPacketAllocator;     /**< for Packet Data */
} sllSharedMemory;


/**
 * @brief Command Argument Structure
 */
typedef struct sllCommandArgs
{
    void * mArg1;   /**< sllHandle - common argument */
    void * mArg2;
} sllCommandArgs;

/**
 * @brief Marshalling Macro for Command Arguments
 */
#define SLL_MARSHALLING_COMMAND_ARGS( aCommandArgs, aArg1, aArg2 )      \
    {                                                                   \
        (aCommandArgs)->mArg1 = (aArg1);                                \
        (aCommandArgs)->mArg2 = (aArg2);                                \
    }

/**
 * @brief Command Argument 1
 * - CML_COMMAND_* : SllHandle
 */
#define SLL_COMMAND_ARG1( aCommandArgs )    ( (aCommandArgs)->mArg1 )
/**
 * @brief Command Argument 2
 * - unused argument
 */
#define SLL_COMMAND_ARG2( aCommandArgs )    ( (aCommandArgs)->mArg2 )


#endif /* SLLDEF_H_ */
