/*******************************************************************************
 * knDef.h
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


#ifndef _KNDEF_H_
#define _KNDEF_H_ 1

/**
 * @file knDef.h
 * @brief Kernel Layer Internal Definitions
 */

/**
 * @defgroup knInternal Kernel Layer Internal Definitions
 * @internal
 * @{
 */

/**
 * @addtogroup knaLatch
 * @{
 */

#define KNA_ADD_CAS_MISS_CNT( aLatch, aMissCnt )                                        \
    {                                                                                   \
        if( ((aMissCnt) + (stlUInt64)(aLatch)->mCasMissCnt) > STL_UINT32_MAX )          \
        {                                                                               \
            (aLatch)->mMissHighArray += 1;                                              \
            (aLatch)->mCasMissCnt = (stlUInt32)(((aMissCnt) +                           \
                                                 (stlUInt64)(aLatch)->mCasMissCnt) -    \
                                                STL_UINT32_MAX);                        \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            aLatch->mCasMissCnt += (aMissCnt);                                          \
        }                                                                               \
    }

#define KNA_GET_CAS_MISS_CNT( aLatch )                                                      \
    ( (stlUInt64)((aLatch)->mMissHighArray) << 32 | (stlUInt64)((aLatch)->mCasMissCnt) )    \

#define KNA_INIT_CAS_MISS_CNT( aLatch )         \
    {                                           \
        (aLatch)->mMissHighArray = 0;           \
        (aLatch)->mCasMissCnt = 0;              \
    }

#define KNA_MAX_LATCH_WAKEUP_COUNT ( 16 )

/** @} */

/**
 * @addtogroup knsSegment
 * @{
 */

/**
 * @brief 유효하지 않은 공유 메모리 키
 */
#define KNS_INVALID_SHM_KEY (-1)

/**
 * @brief X$SHM_SEGMENT 용 Path Controler
 */
typedef struct knsFxPathControl
{
    stlInt32    mSegmentID; /**< 읽어야 할 세그먼트의 ID */
} knsFxPathControl;

/**
 * @brief X$SHM_SEGMENT에서 하나의 segment에 의한 레코드
 */
typedef struct knsFxRecord
{
    stlChar   mName[STL_MAX_SHM_NAME+1]; /**< 공유 메모리 세그먼트의 이름 */
    stlInt16  mId;                       /**< 공유 메모리 세그먼트의 아이디 */
    stlUInt64 mSize;                     /**< 공유 메모리 세그먼트의 크기 */
    stlUInt64 mKey;                      /**< 공유 메모리 세그먼트의 키 */
    stlUInt64 mSeq;                      /**< 공유 메모리 세그먼트의 재사용 계수 */
    stlChar   mAddr[20];                 /**< 공유 메모리 세그먼트의 키 */
} knsFxRecord;

#define KNS_ADDR_TABLE_STATIC_AREA  ( 0 )
/** @} */

/**
 * @defgroup knmMemory Memory Management
 * @internal
 * @{
 */

/**
 * @addtogroup knmDynamic
 * @{
 */

/**
 * @brief Maximum Dynamic Alloator
 */
#define KNM_MAX_DYNAMIC_ALLOCATOR_COUNT   ( 100 )

/**
 * @brief 동적 메모리 chunk의 헤더 
 */
typedef struct knmChunkHeader
{
    stlRingHead  mAllocRing;      /**< 할당된 block들의 ring */
    stlRingHead  mFreeRing;       /**< 반환된 block들의 ring*/
    stlRingEntry mLink;           /**< 할당된 chunk간의 연결 */
    stlUInt64    mSize;           /**< 할당된 chunk의 크기 */
    stlUInt64    mFreeBlockSize;  /**< 반환된 block들의 크기 */
    stlUInt32    mFreeBlockCount; /**< 반환된 block 갯수 */
    stlUInt32    mAlign;          /**< 8byte align을 위한 padding */
} knmChunkHeader;

/**
 * @brief 동적 메모리 block의 헤더 
 */
typedef struct knmBlockHeader
{
    stlRingEntry   mAllocLink;           /**< 할당된 block들의 연결 */
    stlRingEntry   mFreeLink;            /**< 반환된 block들의 연결 */
    stlUInt32      mSize;                /**< 할당된 block의 크기 */
    stlUInt32      mIsUsed;              /**< 해당 block의 사용 여부 */
    stlInt32       mRequestorId;         /**< identifier of request allocator */
    stlUInt32      mRequestorSessionId;  /**< session identifier of requestor */
    void         * mRequestorAddress;    /**< address of request allocator */
} knmBlockHeader;

/**
 * @brief Dynamic Allocator에서 Fragment가 발생할수 있는 최소 크기
 */
#define KNM_DYNAMIC_DEFAULT_MINIMUM_FRAGMENT_SIZE      ( 16 )
/**
 * @brief Shared Static Allocator에서 Fragment가 발생할수 있는 최소 크기
 */
#define KNM_DYNAMIC_SSA_MINIMUM_FRAGMENT_SIZE          ( 1024 )
/**
 * @brief Private Static Allocator에서 Fragment가 발생할수 있는 최소 크기
 */
#define KNM_DYNAMIC_PSA_MINIMUM_FRAGMENT_SIZE          ( 1024 )

/**
 * @brief Event memory 초기 사이즈
 */
#define KNM_EVENT_MEM_INIT_SIZE          ( 1 * 1024 * 1024 )
/**
 * @brief Event memory Next 사이즈
 * - event memory의 공간 확보를 원활히 하기 위해서
 */
#define KNM_EVENT_MEM_NEXT_SIZE          ( KNM_DYNAMIC_SSA_MINIMUM_FRAGMENT_SIZE )

typedef struct knmFxDynamicMem
{
    stlInt64       mSeqId;         /**< sequence id of database level allocators */
    stlInt64       mAllocatorId;   /**< allocator identifier */
    stlUInt64      mAddr;          /**< allocator address */
    stlUInt64      mInitSize;      /**< 미리 할당할 메모리의 최소 크기 */
    stlUInt64      mNextSize;      /**< 추가적으로 메모리 할당시 할당할 메모리의 크기 */
    stlUInt64      mTotalSize;     /**< 할당된 모든 chunk 메모리의 크기 */
    stlUInt64      mMinFragSize;   /**< Minimum Fragemnt Size */
    stlUInt64      mUsedSize;      /**< Total Used Size */
    stlChar        mDesc[KNL_ALLOCATOR_MAX_DESC_SIZE + 1]; /**< 메모리 정보 */
} knmFxDynamicMem;

typedef struct knmFxAllocatorDesc
{
    stlInt64         mId;                    /**< allocator identifier */
    knlAllocatorType mAllocatorType;         /**< allocator type(dynamic,region) */
    stlUInt32        mStorageType;           /**< storage type(heap,shm,plan) */
    stlUInt32        mMemAttr;               /**< attribute(keep empty chunk or not) */
    stlUInt32        mUnmarkType;            /**< unmark type(ordered, random) */
    stlUInt32        mAllocatorLevel;        /**< level(root, internal) */
    knlMemCategory   mAllocatorCategory;     /**< category(database,session,transaction, ...) */
    stlUInt32        mMinFragSize;           /**< minimum fragment size for dynamic allocator */
    stlLayerClass    mLayer;                 /**< layer(standard,kernel, ...) */
    stlChar          mDesc[KNL_ALLOCATOR_MAX_DESC_SIZE + 1]; /**< description */
} knmFxAllocatorDesc;

#define KNM_CALLSTCK_STRING_SIZE   ( 8192 )
#define KNM_ADDR_LENGTH            ( 18 )

typedef struct knmFxDumpDynamicMem
{
    stlUInt64    mRequestorAddress;    /**< address of requestor */
    stlUInt32    mRequestorId;         /**< identifier of requestor */
    stlUInt32    mRequestorSessionId;  /**< session identifier of requestor */
    stlUInt64    mRequestorTotalSize;  /**< current total size of requestor */
    stlUInt64    mChunkId;             /**< block Id ( actually address ) */
    stlUInt64    mBlockId;             /**< block Id ( actually address ) */
    stlUInt64    mSize;                /**< block size */
    stlBool      mIsUsed;              /**< Used 여부 */
    stlChar      mBlockAddr[KNM_ADDR_LENGTH];            /**< block addres */
    stlChar      mCallstack[KNM_CALLSTCK_STRING_SIZE];   /**< callstack string */
} knmFxDumpDynamicMem;

typedef struct knmFxDumpDynamicPathCtrl
{
    stlUInt64         mCurrChunkId;
    stlUInt64         mCurrBlockId;
    knmChunkHeader  * mCurrChunk;
    knmBlockHeader  * mCurrBlock;
    stlUInt64         mValidSeq;
} knmFxDumpDynamicPathCtrl;

/** @} */

/**
 * @addtogroup knmDynamic
 * @{
 */

/**
 * @brief Callstack Depth where memory allocation
 */
#define KNM_MAX_CALLSTACK_DEPTH   (16)

/**
 * @brief Memory Head Fence
 */
typedef struct knmHeadFence
{
    void     * mCallstack[KNM_MAX_CALLSTACK_DEPTH];  /** callstack */
    stlInt64   mCallstackCount;                      /** callstack frame count */
    stlInt64   mSize;                                /** allocated memory size */
    stlInt64   mMagicNum;                            /** magic number */
} knmHeadFence;

/**
 * @brief Memory Tail Fence
 */
typedef struct knmTailFence
{
    stlInt64   mMagicNum;      /** magic number */
} knmTailFence;

/**
 * @brief Magic number for memory fence
 */
#define KNM_FENCE_MAGIC_NUMBER     STL_INT64_C(0xDEADBEEFDEADBEEF)
/**
 * @brief Intialial value for Dynamic/Region Memory
 */
#define KNM_MEMORY_INIT_VALUE      (0xDD)

/**
 * @brief Memory Boundary 검사는 Database Validation모드에서만 동작한다.
 */
#if defined(STL_VALIDATE_DATABASE)
#define KNM_CHECK_MEMORY_BOUNDARY( aEnv ) STL_TRUE
#else
#define KNM_CHECK_MEMORY_BOUNDARY( aEnv ) STL_FALSE
#endif

/** @} */

/**
 * @addtogroup knmRegion
 * @{
 */

/**
 * @brief Region-based 메모리 arena의 헤더
 */
typedef struct knmArenaHeader
{
    stlRingEntry   mLink;        /**< 할당된 arena간의 연결 */
    stlUInt64      mAllocOffset; /**< 마지막으로 할당된 메모리의 offset */
    stlUInt64      mSize;        /**< 할당된 arena의 크기 */
    void         * mAddr;        /**< 실제 할당된 메모리 주소 */
} knmArenaHeader;

/** @} */

/** @} */

/**
 * @defgroup kneEvnet Event
 * @{
 */

/**
 * @brief 이벤트 구조체
 */
typedef struct kneEventData
{
    stlUInt32      mEventId;       /**< Event Identifier */
    stlUInt32      mDataSize;      /**< aData의 바이트단위 크기 */
    stlBool        mCanceled;      /**< Event 실행 전에 취소 여부 */
    stlInt8        mWaitMode;
    stlChar        mPadding[2];
    stlUInt32      mFailBehavior;  /**< fail behavior ( stop / continue ) */
    stlInt64       mEsn;           /**< Event Sequence Number */
    void         * mData;          /**< 사용자 정의 Context */
    void         * mWaitEnv;       /**< Event가 종료되기를 기다리는 Environment의 포인터 */
    stlRingEntry   mEventLink;     /**< 다음 Event Link */
} kneEventData;

/**
 * @brief 이벤트 구조체 초기화
 */
#define KNE_INIT_EVENT_DATA( aEventData, aEventId, aData, aDataSize, aWaitEnv, aWaitMode, aEsn, aFailBehavior ) \
    {                                                                                               \
        (aEventData)->mEventId = (aEventId);                                                        \
        (aEventData)->mDataSize = (aDataSize);                                                      \
        (aEventData)->mCanceled = STL_FALSE;                                                        \
        (aEventData)->mWaitEnv = (aWaitEnv);                                                        \
        (aEventData)->mWaitMode = (aWaitMode);                                                      \
        (aEventData)->mEsn = (aEsn);                                                                \
        (aEventData)->mFailBehavior = (aFailBehavior);                                              \
        STL_RING_INIT_DATALINK( (aEventData),                                                       \
                                STL_OFFSETOF( kneEventData, mEventLink ) );                         \
        if( aData != NULL )                                                                         \
        {                                                                                           \
            (aEventData)->mData = ((stlChar*)(aEventData) + STL_SIZEOF( kneEventData ));            \
            stlMemcpy( (aEventData)->mData, aData, aDataSize );                                     \
        }                                                                                           \
        else                                                                                        \
        {                                                                                           \
            (aEventData)->mData = NULL;                                                             \
        }                                                                                           \
    }

#define KNE_CANCEL_EVENT_DATA( aEventData )       \
{                                                 \
    (aEventData)->mCanceled = STL_TRUE;           \
}

#define  KNM_DUMP_DATA_LENGTH   (32)

typedef struct knmDumpMemoryFxRecord
{
    stlChar  mStartAddr[KNM_ADDR_LENGTH + 1];
    stlChar  mBinData[(KNM_DUMP_DATA_LENGTH *2) + 1];
    stlChar  mTextData[KNM_DUMP_DATA_LENGTH + 1];
} knmDumpMemoryFxRecord;

typedef struct knmDumpMemoryPathCtrl
{
    void      * mStartAddr;
    stlUInt64   mSize;
    stlChar   * mCurAddr;
    stlUInt64   mRemainSize;
} knmDumpMemoryPathCtrl;

/** @} */

/**
 * @defgroup knnSignal Signal
 * @{
 */
typedef struct knnFxSignal
{
    stlInt64 mSignalNumber;
} knnFxSignal;

/** @} */

/**
 * @defgroup knfFxTable Fixed Table for File Manager
 * @{
 */

#define KNF_MAX_CRASH_BYTES   ( 100 )

typedef struct knfFxCrashFile
{
    stlInt64 mDummy;
} knfFxCrashFile;

typedef struct knfFxPathCtrl
{
    stlChar   mFileName[STL_MAX_FILE_NAME_LENGTH + STL_MAX_FILE_PATH_LENGTH + 1];
    stlOffset mOffset;
    stlChar   mCrashBytes[KNF_MAX_CRASH_BYTES];
    stlInt32  mCrashBytesSize;
    stlInt64  mRepeatCnt;
    stlOffset mRepeatUnit;
} knfFxPathCtrl;

/** @} */

/**
 * @defgroup kneEnvironment Environment
 * @{
 */

/**
 * @brief Environment 헤더 구조체
 */
typedef struct kneHeader
{
    void      * mNextFreeLink; /**< 할당 가능한 다음 Environment의 포인터 */
    stlUInt32   mIdx;          /**< Environment의 번호 */
#if (STL_SIZEOF_VOIDP == 8)
    stlUInt32   mAlign;        /**< 8byte align을 위한 padding */
#endif 
} kneHeader;

/**
 * @brief Environment 관리자 구조체
 */
typedef struct kneManager
{
    knlLatch    mLatch;       /**< 동기화 객체 */
    stlUInt32   mCount;       /**< 생성된 Environment의 갯수 */
    stlUInt32   mSize;        /**< 하나의 Environment 크기 */
    kneHeader * mFreeEnvList; /**< 할당 가능한 Environment의 포인터 */
#if (STL_SIZEOF_VOIDP == 4)
    void      * mAlign;       /**< 8byte align을 위한 padding */
#endif 
} kneManager;

#define KNE_REQ_MODE_LENGTH 15
#define KNE_STATE_LENGTH    30

/**
 * @brief X$KN_PROC_ENV에서 하나의 레코드
 */
typedef struct kneEnvFxRecord
{
    stlUInt32 mID;                                  /**< 해당 Environment의 ID */
    stlUInt32 mEnterCriSecCount;                    /** 임계구간의 입출을 위한 계수 */
    stlInt64  mNextFreeID;                          /**< Next Free Environment ID */
    stlChar   mTopLayer[STL_MAX_LAYER_NAME + 1];    /**< Top Layer 이름 */
    stlUInt32 mLatchCount;                          /**< 해당 Environment가 획득한 latch의 수 */
    stlInt64  mNumaNodeId;                          /**< Numa Node Identifier */
    stlInt64  mOsProcId;                            /**< Process Table에서 자신의 ID */
    stlInt32  mProcessId;                           /**< Process Table에서 자신의 ID */
    stlTime   mCreationTime;                        /**< Env가 사용되기 시작한 시간 */
    stlChar   mState[KNE_STATE_LENGTH+1];           /**< 해당 Environment 사용 여부 */
    stlChar   mReqLatchMode[KNE_REQ_MODE_LENGTH+1]; /**< 해당 Environment에서 요청한 latch 모드 */
} kneEnvFxRecord;

#define KNE_KL_PROC_STAT_ROW_COUNT (2)

typedef struct kneKnProcStatPathCtrl
{
    void      * mCurrEnv;
    stlUInt32   mProcId;
    stlInt32    mIterator;
    stlInt64    mValues[KNE_KL_PROC_STAT_ROW_COUNT];
} kneKnProcStatPathCtrl;


#define KNE_KL_SESS_STAT_ROW_COUNT (4)

typedef struct kneKnSessStatPathCtrl
{
    void      * mSessEnv;
    stlUInt32   mSessId;
    stlInt32    mIterator;
    stlInt64    mValues[KNE_KL_SESS_STAT_ROW_COUNT];
} kneKnSessStatPathCtrl;



/** @} */

/**
 * @defgroup kncSession Session
 * @{
 */

/**
 * @brief Session 헤더 구조체
 */
typedef struct kncHeader
{
    void      * mNextFreeLink; /**< 할당 가능한 다음 Session의 포인터 */
    stlUInt32   mIdx;          /**< Session Env 번호 */
#if (STL_SIZEOF_VOIDP == 8)
    stlUInt32   mAlign;        /**< 8byte align을 위한 padding */
#endif 
} kncHeader;

/**
 * @brief Environment 관리자 구조체
 */
typedef struct kncManager
{
    knlLatch    mLatch;           /**< 동기화 객체 */
    stlUInt32   mCount;           /**< 생성된 Session Env의 갯수 */
    stlUInt32   mSize;            /**< 하나의 Session Env 크기 */
    kncHeader * mFreeList;        /**< 할당 가능한 Session Env의 포인터 */
#if (STL_SIZEOF_VOIDP == 4)
    void      * mAlign;           /**< 8byte align을 위한 padding */
#endif 
} kncManager;

#define KNC_STATE_LENGTH    30

/**
 * @brief X$KN_SESSION에서 하나의 레코드
 */
typedef struct kncSessionFxRecord
{
    stlUInt32 mID;                                  /**< 해당 Session의 ID */
    stlChar   mState[KNE_STATE_LENGTH+1];           /**< 해당 Session 사용 여부 */
} kncSessionFxRecord;


/** @} */

/**
 * @addtogroup kniDual
 * @{
 */

typedef struct kniDualFxRecord
{
    stlChar mDummy[2];
} kniDualFxRecord;

typedef struct kniDualPathControl
{
    stlBool mRowFetched;
} kniDualPathControl;

/** @} */

/**
 * @addtogroup kniProcess
 * @{
 */

#define KNI_SIGNAL_CAS_INIT    (0)
#define KNI_SIGNAL_CAS_LOCKED  (1)

#define KNI_MAX_NUMA_NODE_COUNT  (64)
#define KNI_MAX_CPU_COUNT        (128)
#define KNI_NUMA_MASTER_CPU_ID   (0)
#define KNI_INVALID_NUMA_NODE_ID (-1)

/**
 * @brief Numa Node Information
 */
typedef struct kniNumaNodeInfo
{
    stlInt32 mBoundCount;  
    stlInt16 mCpuCount;
    stlInt16 mCpuArray[KNI_MAX_CPU_COUNT];
} kniNumaNodeInfo;

/**
 * @brief Numa Information
 */
typedef struct kniNumaInfo
{
    stlInt64           mNodeCount;
    stlInt16           mMasterNodeId;
    stlInt16           mNodeSeq[KNI_MAX_NUMA_NODE_COUNT];
    kniNumaNodeInfo    mNodeInfo[KNI_MAX_NUMA_NODE_COUNT];
} kniNumaInfo;

/**
 * @brief Process ID를 초기화한다.
 * @param[in,out] aProc stlProc 타입의 프로세스 구조체
 */
#define KNI_INIT_PROCESS_ID( aProc ) (aProc)->mProcID = KNL_INVALID_PROCESS_ID

/**
 * @brief 프로세스 관리를 위한 구조체
 */
typedef struct kniProcessManager
{
    knlLatch mLatch;            /**< 동기화 객체 */
    stlInt32 mFirstFreeProcess; /**< 사용 가능한 프로세스 구조체 번호 */
    stlInt32 mMaxProcessCount;  /**< 최대 프로세스의 갯수 */
} kniProcessManager;

/**
 * @brief 프로세스 정보 구조체
 * @todo 프로세스의 메모리 사용량도 관리할 수 있어야 한다.
 */
typedef struct kniProcessInfo
{
    stlInt32 mNextFreeProcess; /**< 사용 가능한 프로세스 구조체 번호 */
    stlInt32 mAttachCount;     /**< Attach된 session의 개수 */
    stlProc  mProc;            /**< 프로세스 ID */
    knlEvent mEvent;           /**< 이벤트 */
    stlTime  mCreationTime;    /**< Process Info가 할당된 시간 */
    stlInt64 mProcessSeq;      /**< Process 의 uniqueness를 보장하기 위한 sequence */
} kniProcessInfo;

/**
 * @brief X$PROCESS_INFO에서 하나의 레코드
 */
typedef struct kniProcessInfoFxRecord
{
    stlInt32 mID;           /**< 프로세스 테이블에서의 ID */
    stlInt32 mAttachCount;  /**< attached environment count */
    stlProc  mBackendPid;   /**< OS에서의 ID */
    stlTime  mCreationTime; /**< Process info의 생성 시점 */
    stlInt64 mProcessSeq;   /**< Process 의 uniqueness를 보장하기 위한 sequence */
} kniProcessInfoFxRecord;

/** @} */

/**
 * @addtogroup kniStartup
 * @{
 */

#define KNI_SYSTEM_LOGGER_NAME     "system.trc"
#define KNI_ALERT_LOGGER_NAME      "alert.trc"
#define KNI_SYSTEM_LOGGER_MAXSIZE  (10*1024*1024)

#define KNI_INSTANCE_VERSION_LENGTH 64
#define KNI_INSTANCE_STATUS_LENGTH  16

/**
 * @brief X$INSTANCE에서 하나의 레코드
 */
typedef struct kniInstanceFxRecord
{
    stlTime mStartupTime;                          /**< Time when the instance was started */
    stlChar mVersion[KNI_INSTANCE_VERSION_LENGTH]; /**< Database version */
    stlChar mStatus[KNI_INSTANCE_STATUS_LENGTH];   /**< Status of the instanc */
} kniInstanceFxRecord;

#define KNI_SYSTEM_INFO_ROW_COUNT  (16)

/**
 * @brief X$KN_SYSTEM_INFO를 위한 자료구조
 */
typedef struct kniFxSystemInfoPathCtrl
{
    stlInt32 mIterator;
    stlInt64 mValues[KNI_SYSTEM_INFO_ROW_COUNT];
} kniFxSystemInfoPathCtrl;

/** @} */

/**
 * @defgroup knbBreakPoint Break Point
 * @{
 */

typedef struct knbAction
{
    knlBreakPointId           mBpId;
    knlBreakPointSessType     mSessType;
    stlInt32                  mProcId;
    stlInt32                  mSkipCount;
    knlBreakPointAction       mActionType;
    knlBreakPointPostAction   mPostActionType;
    stlInt32                  mValue; /* sleep sec or waiter id */
} knbAction;

typedef struct knbProcInfo
{
    stlChar         mProcName[KNL_MAX_PROC_NAME_LEN];
    knlEnv        * mProcEnv;
    knlProcStatus   mStatus;
} knbProcInfo;

typedef struct knbBreakPointInfo
{
    stlInt32        mActionCount;
    stlInt32        mCurActionSeq;
    stlInt32        mProcCount;
    knbAction       mArray[KNL_MAX_ACTION_COUNT];
    knbProcInfo     mProcList[KNL_MAX_BP_PROCESS_COUNT];
} knbBreakPointInfo;

/** @} */


/**
 * @defgroup kntHash XA Hash
 * @{
 */

/**
 * @brief XA context hash bucket
 */
typedef struct kntHashBucket
{
    knlLatch      mLatch;          /**< latch */
    stlRingHead   mContextList;    /**< linked list for contexts */
} kntHashBucket;

#define KNT_XA_HASH_SIZE    ( 3181 )

#define KNT_XID_HASH_VALUE( aXid )                                                  \
    dtlHash32Any( (stlUInt8*)(aXid), (aXid)->mTransLength + (aXid)->mBranchLength )

/** @} */


/**
 * @defgroup knsEntryPoint Entry Point
 * @{
 */

/**
 * @brief 커널의 각 모듈들에 대한 entry point
 */
typedef struct knsEntryPoint
{
    stlUInt64           mStartupPhase;      /**< 현재까지 완료된 Bootup 단계 */
    stlUInt32           mSar;               /**< System에서 할당 가능한 리소스 목록 */
    stlBool             mEnableDiskIo;      /**< Enable Disk IO */
    stlChar             mPadding[3];        /**< Padding */
    stlTimer            mSystemTimerId;     /**< System Time Identifier */
    stlTime             mSystemTime;        /**< System Time */
    stlTime             mStartupTime;       /**< 인스턴스가 시작할 때의 시간 */
    stlUInt64           mFixedOffset;       /**< 현재까지 할당된 fixed static area의 offset */
    knlShmManager     * mShmManager;        /**< 공유 메모리 관리자의 주소 */
    knlDynamicMem     * mDynamicMem;        /**< variable static memory 관리 */
    kneManager        * mEnvManager;        /**< Environment 관리자의 주소 */
    kncManager        * mSessionEnvManager; /**< Session 관리자의 주소 */
    kniProcessManager * mProcessManager;    /**< 프로세스 관리자의 주소 */
    knlDynamicMem       mEventMem;          /**< Event 저장을 위한 Dynamic Memory */
    void              * mPropertyMem;       /**< Property 리스트의 주소 */
    stlInt64            mXaHashSize;        /**< Hash table size for XA */
    kntHashBucket     * mXaHashTable;       /**< Hash table for XA */
    knlFileMgr          mFileMgr;           /**< File manager */
    knlTraceLogger      mSystemLogger;      /**< System Loggger */
    knlStaticAreaMark   mStaticMark4Mount;
    knlStaticAreaMark   mStaticMark4Open;
    knbBreakPointInfo * mBreakPointInfo;    /**< 설정된 break point들을 저장하는 구조체 */
    stlInt64            mCacheAlignedSize;  /**< static area에서 메모리 할당시 할당된 메모리 주소의 align size */
    stlInt64            mCpuSetSize;        /**< maximum number of CPUs */
    void              * mFxRelationInfo;    /**< Fixed Table Relation Header Informations */
    stlVersionInfo      mVersionInfo;

    /*
     * allocator
     */

    stlInt32           mDynamicArrayCount;
    knlDynamicMem    * mDynamicMemArray[KNM_MAX_DYNAMIC_ALLOCATOR_COUNT];
    
    /*
     * for Plan Cache
     */

    void             * mPlanHash;
    stlInt64           mPlanHashSize;
    void             * mClock;
    stlInt64           mClockCount;
    knlMemController   mMemController;
    knlDynamicMem      mSqlFixMem;
    stlInt64           mClockAgingCount;
    
    /*
     * numa maps
     */

    kniNumaInfo        mNumaInfo;
    
    /*
     * barrier
     * - member 추가/삭제에 따른 성능저하 현상이 발생하고 있습니다.
     *   원인 파악이 될때까지 member를 추가하실때 mPadding2의
     *   공간을 사용해야 합니다.
     */
    stlChar            mBarrier[64];
} knsEntryPoint;

/** @} */

/**
 * @defgroup knpc Plan Cache
 * @{
 */

#define KNPC_PLAN_INIT_SIZE         ( 1 * 1024 *1024 )
#define KNPC_PLAN_HASH_SIZE         ( 3181 )
#define KNPC_MAX_FLANGE_COUNT       ( 128 )
#define KNPC_MAX_PLAN_COUNT         ( 8 )
#define KNPC_CLOCK_AGING_THRESHOLD  ( 100 )
#define KNPC_DEAD_AGE               ( 1 )

typedef struct knpcPlan
{
    void         * mPlanMem;
    void         * mCodePlan;
    stlBool        mIsUsed;
    stlChar        mPadding[3];
    stlInt32       mClockId;
} knpcPlan;

typedef struct knpcHashBucket
{
    knlLatch     mLatch;
    stlRingHead  mOverflowList;
    stlInt32     mBucketId;
} knpcHashBucket;

typedef struct knpcFlange
{
    stlRingEntry      mOverflowLink;
    stlUInt64         mRefCount;
    stlInt32          mPlanCount;
    stlSpinLock       mSpinLock;
    stlInt32          mHashBucketId;
    stlInt32          mHashValue;
    stlInt64          mAge;
    stlChar         * mSqlString;
    stlInt64          mSqlLength;
    knlPlanUserInfo   mUserInfo;
    knlPlanCursorInfo mCursorInfo;
    knlPlanSqlInfo    mSqlInfo;
    void            * mBuffer;
    knpcPlan          mPlans[ KNPC_MAX_PLAN_COUNT ];
    stlTime           mLastFixTime;
    stlBool           mDropped;
    stlChar           mPadding[7];   /**< 64 byte cache alignment */
} knpcFlange;

typedef struct knpcFlangeBlock
{
    knpcFlange    mFlanges[ KNPC_MAX_FLANGE_COUNT ];
    stlRingEntry  mBlockLink;
} knpcFlangeBlock;

typedef struct knpcClockHand
{
    knpcFlangeBlock * mBlockHand;
    stlInt32          mFlangeHand;
    stlChar           mPadding[4];
} knpcClockHand;

typedef struct knpcClock
{
    stlRingHead       mBlockList;
    knpcClockHand     mAgingHand;
    knpcClockHand     mFreeHand;
    stlInt32          mClockId;
    stlInt64          mMaxFlangeCount;
    stlInt64          mFlangeCount;
    knlLatch          mLatch;
    knlDynamicMem     mDynamicMem;
    knlDynamicMem     mFlangeDynamicMem;
} knpcClock;

typedef struct knpcFxSqlCache
{
    void         * mSqlHandle;
    stlUInt64      mRefCount;
    stlInt32       mHashValue;
    stlInt32       mPlanCount;
    stlInt64       mClockId;
    stlInt64       mAge;
    stlChar      * mSqlString;
    stlInt64       mSqlLength;
    stlInt64       mUserId;
    stlInt32       mStmtType;
    stlInt32       mBindCount;
    stlBool        mDropped;
    stlTime        mLastAccessTime;
} knpcFxSqlCache;

typedef struct knpcFxPlanClock
{
    stlInt64       mClockId;
    stlInt64       mFlangeAllocSize;
    stlInt64       mFlangeCount;
    stlInt64       mFreeFlangeCount;
    stlInt64       mPlanAllocSize;
    stlInt64       mPlanCount;
} knpcFxPlanClock;

typedef struct knpcFxSqlCachePathCtrl
{
    stlInt64          mClockId;
    knpcFlangeBlock * mFlangeBlock;
    stlInt32          mFlangeId;
} knpcFxSqlCachePathCtrl;

typedef struct knpcFxPlanClockPathCtrl
{
    stlInt64   mClockId;
} knpcFxPlanClockPathCtrl;

typedef struct knfFxDbFilePathCtrl
{
    stlUInt64         mValidSeq;
    knlFileItem     * mCurFile;
} knfFxDbFilePathCtrl;

#define KNF_DB_FILE_TYPE_STRING_LEN     16

typedef struct knfFxDbFile
{
    stlChar           mPath[STL_MAX_FILE_PATH_LENGTH];
    stlChar           mFileType[KNF_DB_FILE_TYPE_STRING_LEN];
} knfFxDbFile;

/** @} knpc */


/** @} */

#endif /* _KNDEF_H_ */
