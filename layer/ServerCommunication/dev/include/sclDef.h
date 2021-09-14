/*******************************************************************************
 * scl.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: sclDef.h 9428 2013-08-23 09:03:41Z mkkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/

#ifndef SCLDEF_H_
#define SCLDEF_H_

/**
 * @file sclDef.h
 * @brief Server Communication Layer Definition
 */


/**
 * @defgroup sclError Error
 * @{
 */
#define SCL_ERRCODE_COMMUNICATION_LINK_FAILURE      STL_MAKE_ERRCODE( STL_ERROR_MODULE_SERVER_COMMUNICATION, 0 )
#define SCL_ERRCODE_INVALID_COMMUNICATION_PROTOCOL  STL_MAKE_ERRCODE( STL_ERROR_MODULE_SERVER_COMMUNICATION, 1 )
#define SCL_ERRCODE_OUT_OF_CM_UNIT_MEMORY           STL_MAKE_ERRCODE( STL_ERROR_MODULE_SERVER_COMMUNICATION, 2 )

#define SCL_MAX_ERROR                               (3)

/** @} */


/**
 * @defgroup sclProcEnv Process Environment
 * @ingroup sclEnvironment
 * @{
 */

/**
 * @brief Server Communication 의 Process Environment 구조체
 */
typedef struct sclEnv
{
    knlEnv                mKnlEnv;              /**< 하위 Layer 의 Process Environment */
} sclEnv;

/**
 * @brief Server Communication Environment
 */
#define SCL_ENV( aEnv )      ( (sclEnv *)aEnv )

/**
 * @brief Server Communication Environment에서 ErrorStack을 얻는 매크로
 */
#define SCL_ERROR_STACK( aEnv ) &((KNL_ENV(aEnv))->mErrorStack)

#define SCL_IS_HEAD( aControl ) \
    ( (((aControl) & CML_CONTROL_HEAD) == CML_CONTROL_HEAD) ? STL_TRUE : STL_FALSE )

#define SCL_IS_TAIL( aControl ) \
    ( (((aControl) & CML_CONTROL_TAIL) == CML_CONTROL_TAIL) ? STL_TRUE : STL_FALSE )


#define SCL_HANDLE_SENT_MARK( aHandle, aMark ) \
    ( (aMark) = (aHandle)->mSentPacketCount )

#define SCL_HANDLE_HAS_SENT( aHandle, aMark ) \
    ( (aHandle)->mSentPacketCount > (aMark) ? STL_TRUE : STL_FALSE )


/**
 * @brief Server Communication의 Session Environment 구조체
 */
typedef struct sclSessionEnv
{
    knlSessionEnv        mKnlSessionEnv; /**< 하위 Layer 의 Session Environment */
} sclSessionEnv;


/**
 * @brief queue로 전송되는 command
 * @remark client와 상관없이 dispatcher와 shared-server간에 보내는 command
 */
typedef enum sclQueueCommand
{
    SCL_QCMD_NORMAL_PERIOD_CONTINUE,
    SCL_QCMD_NORMAL_PERIOD_END,
    SCL_QCMD_SERVER_FAILURE      /**< shared-server에서 command처리하다 문제 발생한경우 */
} sclQueueCommand;



/**
 * @brief sclQueue에서 첫번째 item을 가져오는 macro
 */
#define SCL_GET_FIRST_ITEM_FROM_QUEUE( aCmlQueue )       ((stlChar *)((stlChar *)aCmlQueue + STL_SIZEOF(sclQueue)))

/**
 * @brief array메모리를 사용하는 queue 구조체
 * @remark sclQueue 구조체 다음에 item들이 위치함. item 크기, 개수는 runtime시 정해짐
 */
typedef struct sclQueue
{
    stlUInt64           mRear;
    stlUInt64           mFront;
    stlInt64            mItemCount;         /**< queue에 저장가능한 최대 item 개수 */
    stlInt64            mItemSize;          /**< item하나의 크기 */
    stlSemaphore        mEventSemaphore;    /**< event전달용 */
    stlSemaphore        mLockForPush;
    stlInt64            mQueued;            /** for fixed table */
    stlInt64            mWait;              /** for fixed table */
    stlInt64            mTotalQueue;        /** for fixed table */
} sclQueue;


/**
 * @brief sclIpcPacket의 body부분을 가져온다.
 * @remark sclIpcPacket의 body는 sclIpcPacket 구조체 바로 다음에 위치한다.
 */
#define SCL_IPC_GET_BODY_FROM_HEADER( aIpcPacket )       ((stlChar *)((stlChar *)(aIpcPacket) + STL_SIZEOF(sclIpcPacket)))

/**
 * @brief sclIpcPacket의 Header부분을 가져온다.
 * @remark sclIpcPacket의 body는 sclIpcPacket 구조체 바로 다음에 위치한다.
 */
#define SCL_IPC_GET_HEADER_FROM_BODY( aBody )       ((sclIpcPacket *)((stlChar *)(aBody) - STL_SIZEOF(sclIpcPacket)))

/**
 * @brief IPC packet하나를 위한 구조체
 * @remark body는 구조체 다음에 위치함. body size는 runtime시 정해짐
 */
typedef struct sclIpcPacket sclIpcPacket;
struct sclIpcPacket
{
    sclIpcPacket  * mNext;             /**< 다음 packet의 pointer */
    stlInt32        mSize;             /**< 쓰여진 body size */
    stlInt32        mCacheIdx;         /* cache 아니면 -1 */
};


/**
 * @brief sclIpcPacket의 cache
 */
#define SCL_IPC_CACHE_COUNT    2
typedef struct sclIpcCache
{
    stlBool                 mIsUsed;
    stlChar                 mPadding[7];
    sclIpcPacket          * mPacket;
} sclIpcCache;

/**
 * @brief 단방향 ipc의 구조체이다.
 */
typedef struct sclIpcSimplex
{
    sclIpcPacket          * mReadPosition;          /**< 읽을 위치 */
    sclIpcPacket          * mWritePosition;         /**< 저장할 위치 */
    stlInt64                mAllocatedCmUnitCount;  /**< 할당된 cm unit count. 동시성 문제가 있음으로 mFreedCmUnitCount 와 분리 */
    stlInt64                mFreedCmUnitCount;      /**< 해제된 cm unit count */
    sclIpcCache             mCache[SCL_IPC_CACHE_COUNT];
} sclIpcSimplex;


/**
 * @brief IPC통신을 위한 구조체
 * @remark shared-server에서 사용하는데 최적화 되어 있음.\n
 *      IPC통신은 하나의 dispatcher와 여러 shared-server가 사용함.\n
 *      (shared-server에서 client는 dispatcher가 됨)\n
 */
typedef struct sclIpc
{
    stlArrayAllocator     * mArrayAllocator;        /**< shared-memory를 관리하는 allocator */

    sclIpcSimplex           mRequestIpc;            /**< disapatcher -> shared-server */

    sclIpcSimplex           mResponseIpc;           /**< shared-server -> disapatcher */
    sclQueue              * mResponseEvent;         /**< queue는 여러 ipc가 같이 사용함으로 포인터로 가지고 있음 */
} sclIpc;



/**
 * @brief 통신 type
 */
typedef enum sclCommunicationType
{
    SCL_COMMUNICATION_TCP = 1,
    SCL_COMMUNICATION_IPC
} sclCommunicationType;

/**
 *  @brief socket or IPC 통신을 위한 구조체
 */
typedef struct sclCommunication
{
    sclCommunicationType    mType;
    stlContext            * mContext;
    sclIpc                * mIpc;
} sclCommunication;


/**
 * @brief Command처리시 메모리 관리를 위한 구조체
 * @remark life-cycle은 process이다.
 */
typedef struct sclMemoryManager
{
    knlRegionMem             mAllocator;
    stlInt64                 mPacketBufferSize;
    stlChar                * mWriteBuffer;
    stlChar                * mReadBuffer;

    knlRegionMark            mOperationMark;
    stlInt64                 mOperationBufferSize;
    stlChar                * mOperationBuffer;

    knlRegionMem             mSendPoolingBufAllocator;
    stlInt64                 mPollingBufferSize;
    knlRegionMark            mSendPoolingBufMark;
} sclMemoryManager;


typedef struct sclSendPoolingBufChunk sclSendPoolingBufChunk;

struct sclSendPoolingBufChunk
{
    stlChar                * mBuf;
    sclSendPoolingBufChunk * mNext;
    stlInt64                 mSize;  /* 8 bytes align을 맞추기 위해 long을 사용 */
};

/**
 * @brief ProtocolSentence별로 관리가 필요한 구조체
 * @remark life-cycle은 job이다. imlProtocolSentence에 포함되어 있다.\n
 *         sclProtocolSentence는 protocolSentence 마다 초기화(sclInitializeProtocolSentence) 되어야 하지만 \n
 *         ODBC에서는 처음에 한번만 호출한다. (ODBC에서는 protocolSentence 개념이 명확히 구현되어 있지 않음)
 *
 */
typedef struct sclProtocolSentence
{
    sclMemoryManager       * mMemoryMgr;
    stlChar                * mWriteStartPos;
    stlChar                * mWriteProtocolPos;
    stlChar                * mWriteEndPos;
    stlChar                * mReadStartPos;
    stlChar                * mReadProtocolPos;
    stlChar                * mReadEndPos;
    sclSendPoolingBufChunk * mSendPoolingBufHead;
    sclSendPoolingBufChunk * mSendPoolingBufCur;
    stlBool                  mPoolingFlag;
    stlUInt8                 mReadPeriod;           /**< 받은 packet의 period값 CML_PERIOD_CONTINUE or CML_PERIOD_END */
    stlInt32                 mReadPayloadSize;      /**< 받은 packet의 payload size */
    stlInt64                 mCachedStatementId;
    stlInt64                 mPendingCmUnitCount;   /**< response대기중인 cm unit count */
} sclProtocolSentence;


/**
 * @brief  protocol을 관리하는 구조체
 * @remark shared mode에서는 이 부분이 shared memory로 관리되어 여러 shared server에서 공유하게 된다.
 *         life-cycle은 session이다. zlhlHandle에 포함되어 있다.
 */
typedef struct sclHandle
{
    sclCommunication         mCommunication;
    stlInt16                 mMajorVersion;      /**< protocol major version */
    stlInt16                 mMinorVersion;      /**< protocol minor version */
    stlInt16                 mPatchVersion;      /**< protocol patch version */
    stlInt8                  mEndian;            /**< protocol에 적용할 endian */
    stlUInt8                 mSendSequence;      /**< send period sequence */
    stlUInt8                 mRecvSequence;      /**< recv period sequence */
    stlBool                  mSocketPeriod;      /**< client socket을 종료를 의미함 */
    stlInt64                 mSentPacketCount;
    /**
     * mSocketAddr은 impConnect에서 1회성으로 사용함으로 여기보다는 packet으로 주는것이 좋을것 같음.
     * 현재 dispatcher와 shared-server 자체 통신이 명확하지 않아 일단 여기에 둠.
     */
    stlSockAddr              mClientAddr;
    sclProtocolSentence    * mProtocolSentence;
} sclHandle;


/**
 * @brief sclHandle을 얻는 매크로
 */
#define SCL_HANDLE( aHandle )         ((sclHandle*)(aHandle))

/**
 * @brief sclProtocolSentence을 얻는 매크로
 */
#define SCL_PROTOCOL_SENTENCE( aHandle )         ((sclProtocolSentence*)(SCL_HANDLE(aHandle)->mProtocolSentence))


/*
 * CALLBACK FUNCTIONS
 */
typedef void (*sclCheckControlCallback) ( void           * aHandle,
                                          stlInt64       * aStatementId,
                                          stlInt8          aControl,
                                          stlBool        * aIgnore,
                                          stlBool        * aProtocolError,
                                          void           * aEnv );

typedef stlStatus (*sclGetBindValueCallback) ( void           * aHandle,
                                               stlInt64         aStatementId,
                                               stlInt32         aColumnNumber,
                                               stlInt8          aControl,
                                               dtlDataValue  ** aBindValue,
                                               void           * aEnv );

typedef stlStatus (*sclReallocLongVaryingMemCallback) ( void          * aHandle,
                                                        dtlDataValue  * aBindValue,
                                                        void          * aCallbackContext,
                                                        void          * aEnv );

typedef stlStatus (*sclGetTargetTypeCallback) ( void               * aHandle,
                                                dtlValueBlockList  * aTargetColumn,
                                                stlInt8            * aDataType,
                                                stlInt8            * aStringLengthUnit,
                                                stlInt8            * aIntervalIndicator,
                                                stlInt64           * aArgNum1,
                                                stlInt64           * aArgNum2,
                                                void               * aEnv );

/** @} */

#endif /* SCLDEF_H_ */
