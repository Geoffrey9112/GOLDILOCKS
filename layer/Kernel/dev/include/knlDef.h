/*******************************************************************************
 * knlDef.h
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


#ifndef _KNLDEF_H_
#define _KNLDEF_H_ 1

/**
 * @file knlDef.h
 * @brief Kernel Layer Definitions
 */

/**
 * @addtogroup knExternal
 * @{
 */

/**
 * @defgroup knlLogicalAddr Logical Address
 * @{
 */

/**
 * @brief 논리적 주소
 */
typedef stlUInt64 knlLogicalAddr;

#define KNL_LOGICAL_ADDR_NULL      STL_UINT64_C(0xFFFFFFFFFFFFFFFF)

/**
 * @brief 공유 메모리 인덱스 위치 비트
 */
#define KNL_SHM_INDEX_PRECISION    16

/**
 * @brief 공유 메모리 OFFSET 위치 비트
 */
#define KNL_OFFSET_PRECISION       48

/**
 * @brief 논리적 주소에서 공유 메모리 인덱스 위치 (상위 16비트 사용)
 */
#define KNL_SHM_INDEX_MASK         STL_UINT64_C(0xFFFF000000000000)

/**
 * @brief 논리적 주소에서 공유 메모리 OFFSET 위치 (하위 48비트 사용)
 */
#define KNL_OFFSET_MASK            STL_UINT64_C(0x0000FFFFFFFFFFFF)

/**
 * @brief 논리적 주소에서 공유 메모리 인덱스를 얻는다.
 */
#define KNL_GET_SHM_INDEX( aPointer )           \
    ( ( aPointer & KNL_SHM_INDEX_MASK )         \
      >> KNL_OFFSET_PRECISION )                 \

/**
 * @brief 논리적 주소에서 공유 메모리 offset을 얻는다.
 */
#define KNL_GET_OFFSET( aPointer )              \
    ( aPointer & KNL_OFFSET_MASK )

/**
 * @brief 논리적 주소를 만든다.
 */
#define KNL_MAKE_LOGICAL_ADDR( aShmIndex, aOffset )                     \
    ( aOffset + ( (stlUInt64)aShmIndex << KNL_OFFSET_PRECISION ) )

/**
 * @brief 프로퍼티 최대 개수
 */

#define KNL_PROPERTY_MAX_COUNT    (512)

/** @} */

/**
 * @defgroup knlError Error
 * @{
 */

/**
 * @brief initialize error
 */
#define KNL_ERRCODE_INITIALIZE             STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 0 )

/**
 * @brief initialized yet error
 */
#define KNL_ERRCODE_INITIALIZED_YET        STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 1 )

/**
 * @brief invalid argument error
 */
#define KNL_ERRCODE_INVALID_ARGUMENT       STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 2 )

/**
 * @brief invalid shared memory type
 */
#define KNL_ERRCODE_INVALID_SHM_TYPE       STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 3 )

/**
 * @brief invalid static shared memory
 */
#define KNL_ERRCODE_INVALID_STATIC_SHM     STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 4 )

/**
 * @brief exceeded maximum number of shared memory segment num
 */
#define KNL_ERRCODE_EXCEEDED_SHM_SEGMENT_COUNT  STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 5 )

/**
 * @brief internal error
 */
#define KNL_ERRCODE_INTERNAL               STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 6 )

/**
 * @brief identifier is too long
 */
#define KNL_ERRCODE_TOO_LONG_IDENTIFIER    STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 7 )

/**
 * @brief comment is too long
 */
#define KNL_ERRCODE_TOO_LONG_COMMENT       STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 8 ) 

/**
 * @brief fixed table does not exist
 */
#define KNL_ERRCODE_FXTABLE_NOT_EXIST      STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 9 ) 

/**
 * @brief Insufficient static area 
 */
#define KNL_ERRCODE_INSUFFICIENT_STATIC_AREA  STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 10 ) 

/**
 * @brief Unable to extend memory: [MAX: %ld, TOTAL: %ld, ALLOC: %ld] DESC: %s
 */
#define KNL_ERRCODE_UNABLE_EXTEND_MEMORY  STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 11 ) 

/**
 * @brief too many open files
 */
#define KNL_ERRCODE_FILE_DESCRIPTOR_FULL   STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 12 ) 

/**
 * @brief Property does not exist
 */
#define KNL_ERRCODE_PROPERTY_NOT_EXIST    STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 13 )

/**
 * @brief Invalid Startup Phase
 */
#define KNL_ERRCODE_PROPERTY_INVALID_STARTUP_PHASE    STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 14 )

/**
 * @brief Property does not modifiable
 */
#define KNL_ERRCODE_PROPERTY_NOT_MODIFIABLE  STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 15 )

/**
 * @brief Invalid Property Value
 */
#define KNL_ERRCODE_PROPERTY_INVALID_VALUE   STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 16 )

/**
 * @brief Don't modifiable Property using ALTER SESSION.
 */
#define KNL_ERRCODE_PROPERTY_INVALID_SES_MODIFY  STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 17 )

/**
 * @brief Invalid Property SCOPE.
 */
#define KNL_ERRCODE_PROPERTY_INVALID_SCOPE   STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 18 )

/**
 * @brief Invalid Propert MODE
 */
#define KNL_ERRCODE_PROPERTY_INVALID_MODE     STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 19 )

/**
 * @brief Invalid Home Directory on System Environment variable
 */
#define KNL_ERRCODE_PROPERTY_INVALID_HOME_DIR   STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 20 )

/**
 * @brief Invalid SID on System Environment variable
 */
#define KNL_ERRCODE_PROPERTY_INVALID_SID      STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 21 )

/**
 * @brief Invalid Property Data type.
 */
#define KNL_ERRCODE_PROPERTY_INVALID_DATATYPE   STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 22 )

/**
 * @brief Invalid Property ID.
 */
#define KNL_ERRCODE_PROPERTY_INVALID_ID        STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 23 )

/**
 * @brief exceeded maximum number of processes
 */
#define KNL_ERRCODE_EXCEEDED_PROCESS_COUNT  STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 24 )

/**
 * @brief Property File Not Exist
 */
#define KNL_ERRCODE_PROPERTY_FILE_NOT_EXIST  STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 25 )

/**
 * @brief Property File Exist
 */
#define KNL_ERRCODE_PROPERTY_FILE_EXIST      STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 26 )

/**
 * @brief Property File Process Error
 */
#define KNL_ERRCODE_PROPERTY_INVALID_FILE_PROCESS   STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 27 )

/**
 * @brief Property is not Read Only
 */
#define KNL_ERRCODE_PROPERTY_NOT_READONLY     STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 28 )

/**
 * @brief Property exist readonly file.
 */
#define KNL_ERRCODE_PROPERTY_EXIST_READONLY_PROPERTY    STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 29 )

/**
 * @brief Property File Parsing Error
 */
#define KNL_ERRCODE_PROPERTY_FILE_PARSING_ERROR    STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 30 )

/**
 * @brief Filter could not support this return type.
 */
#define KNL_ERRCODE_FILTER_NOT_SUPPORT_RETURN_TYPE_CLASS    STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 31 )

/**
 * @brief Unsupported function is used in making filter.
 */
#define KNL_ERRCODE_FILTER_NOT_SUPPORT_FUNCTION    STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 32 )

/**
 * @brief It can not make range.
 */
#define KNL_ERRCODE_FILTER_CANNOT_MAKE_RANGE    STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 33 )

/**
 * @brief Service is not available
 */
#define KNL_ERRCODE_SERVICE_NOT_AVAILABLE    STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 34 )

/**
 * @brief Invalid Break Point Name
 */
#define KNL_ERRCODE_INVALID_BREAKPOINT_NAME    STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 35 )

/**
 * @brief Break Point General Abort Error
 */
#define KNL_ERRCODE_BREAKPOINT_GENERAL_ABORT      STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 36 )

/**
 * @brief Invalid Session Name Error
 */
#define KNL_ERRCODE_BREAKPOINT_INVALID_SESS_NAME  STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 37 )

/**
 * @brief exceeded maximum idle time
 */
#define KNL_ERRCODE_EXCEEDED_IDLE_TIMEOUT         STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 38 )

/**
 * @brief exceeded maximum query time
 */
#define KNL_ERRCODE_EXCEEDED_QUERY_TIMEOUT        STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 39 )

/**
 * @brief Communication link failure
 */
#define KNL_ERRCODE_COMMUNICATION_LINK_FAILURE    STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 40 )

/**
 * @brief User session ID does not exist
 */
#define KNL_ERRCODE_USER_SESSION_ID_DOSE_NOT_EXIST  STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 41 )

/**
 * @brief Server rejected the connection
 */
#define KNL_ERRCODE_SERVER_REJECTED_THE_CONNECTION  STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 42 )

/**
 * @brief operation canceled
 */
#define KNL_ERRCODE_OPERATION_CANCELED   STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 43 )

/**
 * @brief invalid process identifer
 */
#define KNL_ERRCODE_INVALID_PROCESS_ID   STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 44 )

/**
 * @brief mismatched binary version
 */
#define KNL_ERRCODE_MISMATCH_BINARY_VERSION   STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 45)

/**
 * @brief session cannot be killed now
 */
#define KNL_ERRCODE_SESSION_CANNOT_BE_KILLED_NOW  STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 46 )

/**
 * @brief data is outside the range of the data type to which the number is being converted
 */
#define KNL_ERRCODE_C_NUMERIC_DATA_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED  STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 47 )

/**
 * @brief one or more fields of data value were truncated during conversion
 */
#define KNL_ERRCODE_C_INTERVAL_SOME_FILEDS_TRUNCATED_CONVERSION  STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 48 )

/**
 * @brief not implemented feature, in a function
 */
#define KNL_ERRCODE_NOT_IMPLEMENTED  STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 49 )

/**
 * @brief insufficient plan cache memory
 */
#define KNL_ERRCODE_INSUFFICIENT_PLAN_CACHE_MEMORY STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 50 )

/**
 * @brief There is no information File. unable to attach the shm.
 */
#define KNL_ERRCODE_FAIL_ATTACH_FILE_NOT_EXIST     STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 51 )

/**
 * @brief invalid dump option string
 */
#define KNL_ERRCODE_INVALID_DUMP_OPTION_STRING     STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 52 )

/**
 * @brief dump object modified by concurrent statement
 */
#define KNL_ERRCODE_MODIFIED_DUMP_OBJECT           STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 53 )

/**
 * @brief invalid numa map (%s)
 */
#define STL_ERRCODE_INVALID_NUMA_MAP               STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 54 )

/**
 * @brief AS ADMIN must be executed in dedicated session
 */
#define KNL_ERRCODE_ADMIN_MUST_DEDICATED           STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 55 )

/**
 * @brief unable to create file '%s' - already in use
 */
#define KNL_ERRCODE_FILE_ALREADY_IN_USE            STL_MAKE_ERRCODE( STL_ERROR_MODULE_KERNEL, 56 )

#define KNL_MAX_ERROR     57


/**
 * @brief KERNEL ASSERT 매크로
 * @remarks aParams 전체는 반드시 ()안에 들어 있어야 한다. <BR>
 * @par example
 * KNL_ASSERT( aLsn < 10, aEnv, ("LSN(%d)\n", aLsn) );
 */
#define KNL_ASSERT( aExpression, aEnv, aParams )        \
    do                                                  \
    {                                                   \
        if( !(aExpression) )                            \
        {                                               \
            knlLogMsg( NULL,                            \
                       KNL_ENV(aEnv),                   \
                       KNL_LOG_LEVEL_FATAL,             \
                       "ASSERT(%s)\n"                   \
                       "failed in file %s line %d\n",   \
                       #aExpression,                    \
                       __FILE__,                        \
                       __LINE__ );                      \
                                                        \
            stlAssert aParams;                          \
        }                                               \
    } while( 0 )

#if defined( STL_DEBUG ) || defined( DOXYGEN )
#define KNL_SYSTEM_FATAL_DASSERT( aExpression, aEnv )   \
    {                                                   \
        KNL_ENTER_CRITICAL_SECTION( aEnv );             \
        STL_ASSERT( aExpression );                      \
        KNL_LEAVE_CRITICAL_SECTION( aEnv );             \
    }
#else
#define KNL_SYSTEM_FATAL_DASSERT( aExpression, aEnv )
#endif

/** @} */

/**
 * @addtogroup knlRing
 * @{
 */

/**
 * @brief 연결할 객체 안에 링크 구조체를 선언해 사용한다.
 * @code
 *     struct MyElement
 *     {
 *         knlRingEntry   mLink;
 *         stlInt32       mFoo;
 *         stlChar      * mBar;
 *     }
 * @endcode
 */
typedef struct knlRingEntry
{
    knlLogicalAddr volatile mNext;
    knlLogicalAddr volatile mPrev;
} knlRingEntry;

/**
 * @brief Ring 관리를 위한 Ring Head
 */
typedef knlRingEntry knlRingHead;

/** @} */

/**
 * @addtogroup knlCondVar
 * @{
 */

/**
 * @brief Env 세마포어를 사용하는 Conditional Variable 객체
 */
typedef struct knlCondVar
{
    stlUInt32   mSpinLock;     /**< Spin Lock용 메모리 영역 */
    stlChar     mPadding[4];   /**< padding */
    stlRingHead mWaiters;      /**< Signal에 대기중인 Process/Thread의 대기열 */
} knlCondVar;

/** @} */

/**
 * @addtogroup knlLatch
 * @{
 */

/**
 * @todo knlLatch를 설계해야 함. V$에서 Latch들을 어떻게 보여줄 것인가?
 */

/**
 * @brief 래치 모드
 */
/**
 * @defgroup knlLatchMode Latch Mode
 * @ingroup knlLatch
 * @{
 */
#define  KNL_LATCH_MODE_INIT       0   /**< 래치의 초기 상태 */
#define  KNL_LATCH_MODE_SHARED     1   /**< Shrared 모드 */
#define  KNL_LATCH_MODE_EXCLUSIVE  2   /**< Exclusive 모드 */
/** @} */

/**
 * @defgroup knlLatchPriority Latch Priority
 * @ingroup knlLatch
 * @{
 */
#define  KNL_LATCH_PRIORITY_NORMAL      1   /**< priority NORMAL */
#define  KNL_LATCH_PRIORITY_HIGH        2   /**< priority HIGH */
/** @} */

/**
 * @brief 래치에 대기하는 각 프로세스(쓰레드)가 제공하는 노드
 */
typedef struct knlLatchWaiter
{
    stlRingEntry mRing;        /**< Prev, Next를 가지는 physical pointer circular 리스트 */
    stlSemaphore mSem;         /**< 대기를 위한 세마포어 */
    stlUInt8     mReqMode;     /**< 프로세스(쓰레드)가 요청한 latch 모드 */
    stlBool      mForLogFlush; /**< Log Flush를 위한 대기인지 여부 */
    stlInt8      mLostSemValue;/**< lost semaphore count */
    stlInt8      mWaitState;   /**< waiting state */
    stlSpinLock  mSpinLock;    /**< knlSuspend / knlResume의 lost signal을 방어하기 위한 spin lock */
    stlInt64     mFlushSbsn;   /**< SBSN to be flushed */
    stlInt64     mFlushLsn;    /**< LSN to be flushed */
} knlLatchWaiter;

/**
 * @brief 래치 구조체
 * @note Cache Miss를 고려해 반드시 64 바이트 Align을 맞춰야 한다.
 */
typedef struct knlLatch
{
    knlLogicalAddr    mLogicalAddr;      /**< Logical Address */
    stlUInt32         mCasMissCnt;       /** CAS miss count */
    stlUInt8          mMissHighArray;    /** Miss 계수들의 상위 1바이트 배열 */
    stlChar           mPadding[2];       /** Padding */
    stlUInt8          mCurMode;          /**< 현재 래치 모드 */
    stlSpinLock       mSpinLock;         /**< 래치용 메모리 영역 (STL_SPINLOCK_STATUS_INIT/
                                          *                       STL_SPINLOCK_STATUS_LOCKED)
                                          */
    stlUInt16         mRefCount;         /**< 래치에 Grant된 Process(Thread)의 개수 */
    stlUInt16         mWaitCount;        /**< wait count */
    stlRingHead       mWaiters;          /**< 래치에 대기중인 Process/Thread의 대기열(FIFO) */
    stlThreadHandle * mExclOwner;        /**< Exclusive 래치를 획득한 소유자 핸들 */
    stlUInt64         mExclLockSeq;      /**< Double Checking을 위한 Lock Sequence */
} knlLatch;


/**
 * @brief knlTryLatch 호출시 Spinlock try count
 */

#define KNL_TRY_SPIN_COUNT  1

/** @} */

/**
 * @addtogroup knlStaticHash
 * @{
 */

/**
 * @brief Static Hash 동시성 제어 방법
 */
#define KNL_STATICHASH_MODE_NOLATCH  0     /**< 동시성 제어 안함 */
#define KNL_STATICHASH_MODE_HASHLATCH  1   /**< Hash 전체를 하나의 latch로 동시성 제어함 */
#define KNL_STATICHASH_MODE_BUCKETLATCH  2 /**< 각 Bucket 별로 latch를 할당받아 동시성 제어함 */

/**
 * @brief Static Hash 관리를 위한 Header
 * 바로 뒤에 mBucketCount 만큼의 knlStaticHashBucket의 배열이 있다
 */
typedef struct knlStaticHash
{
    stlUInt16       mBucketCount;
    stlChar         mLatchMode;
    stlBool         mIsOnShm;
    stlUInt16       mLinkOffset;
    stlUInt16       mKeyOffset;
    knlLatch      * mHashLatch;
} knlStaticHash;

typedef struct knlStaticHashBucket
{
    knlLatch    * mLatch;
    stlRingHead   mList;
} knlStaticHashBucket;

typedef struct knlHashModulus
{
    stlUInt32  mPrimeNumber;
    stlUInt32  mConstant;
    stlUInt32  mShiftBit;
    stlUInt32  mAdjustConstant;
} knlHashModulus;

/**
 * @brief Modulus 연산없는 Modulus
 * @param[in]  aB    Bucket Count
 * @param[in]  aK    Hash Key
 * @param[in]  aC    Modulus Constant
 * @param[in]  aS    Modulus Shift Bit
 * @param[in]  aA    Adjust Constant
 */

extern knlHashModulus gKnlHashModulus[];

#define KNL_HASH_BUCKET_IDX( aB, aK, aC, aS, aA )                            \
    ( (aK) - (((((((stlUInt64)(aK)) * (aC)) >> 31) + (aA)) >> 1) >> (aS)) * (aB) )

typedef stlUInt32 (*knlStaticHashHashingFunc)( void * aKey, stlUInt32 aBucketCount );

typedef stlInt32 (*knlStaticHashCompareFunc)( void * aKeyA, void * aKeyB );

#define KNL_STATICHASH_GET_NTH_BUCKET(hashPtr,Seq)  \
    (knlStaticHashBucket*)(((knlStaticHash*)hashPtr) + 1) + Seq

/** @} */

/**
 * @addtogroup knlTraceLogger
 * @{
 */

/**
 * @brief trace 로그 기본 크기
 */
#define KNL_DEFAULT_TRACE_LOG_FILE_SIZE (10 * 1024 * 1024)

/**
 * @brief trace 로그 seperator
 */
#define KNL_TRACE_LOG_FILE_NAME_SEPERATOR "_"

#define KNL_SYSTEM_TRACE_LOGGER  ( &(gKnEntryPoint->mSystemLogger) )

/**
 * @brief trace 로그의 레벨
 */
typedef enum
{
    KNL_LOG_LEVEL_FATAL = 0, /**< FATAL LOG (운영될 수 없는 수준의 LOG) */
    KNL_LOG_LEVEL_ABORT,     /**< ABORT LOG (롤백 가능한 수준의 LOG) */
    KNL_LOG_LEVEL_WARNING,   /**< WARNING LOG (경고 수준의 LOG) */
    KNL_LOG_LEVEL_INFO       /**< INFOMATION (정보 수준의 LOG) */
} knlLogLevel;

/**
 * @brief trace 로그 관리자를 초기화한다.
 * @remark 실제 사용은 knlCreateLogger() 함수를 호출해야 사용할 수 있다.
 * @see knlCreateLogger()
 */
#define KNL_INIT_TRACE_LOGGER( aTraceLogger )                   \
    (aTraceLogger)->mMaxSize        = 0;                        \
    (aTraceLogger)->mLastFileNo     = 0;                        \
    (aTraceLogger)->mName[0]        = 0;                        \
    (aTraceLogger)->mCallstackLevel = KNL_LOG_LEVEL_FATAL

/**
 * @brief trace 로그 관리자
 */
typedef struct knlTraceLogger
{
    stlUInt64 mMaxSize;                        /**< 로그 파일 하나의 최대 크기 */
    stlUInt32 mLastFileNo;                     /**< 마지막에 저장된 로그 파일의 번호 */
    stlUInt32 mCallstackLevel;                 /**< 콜스택이 출력될 로그 레벨  */
    knlLatch  mLatch;                          /**< 동기화 객체 */
    stlChar   mName[STL_MAX_FILE_PATH_LENGTH]; /**< 로그 파일 생성 이름 */
} knlTraceLogger;

/** @} */

/**
 * @addtogroup knlEvent
 * @{
 */

/**
 * @brief no wait event
 */
#define  KNL_EVENT_WAIT_NO                 ( 0x00000000 )
/**
 * @brief blocked wait event
 */
#define  KNL_EVENT_WAIT_BLOCKED            ( 0x00000001 )
/**
 * @brief polling wait event
 */
#define  KNL_EVENT_WAIT_POLLING            ( 0x00000002 )
/**
 * @brief event wait mask
 */
#define  KNL_EVENT_WAIT_MASK               ( 0x00000003 )

/**
 * @brief stop when event failed
 */
#define KNL_EVENT_FAIL_BEHAVIOR_STOP       ( 0x00000000 )
/**
 * @brief continue next event when event failed
 */
#define KNL_EVENT_FAIL_BEHAVIOR_CONTINUE   ( 0x00000010 )
/**
 * @brief event wait mask
 */
#define KNL_EVENT_FAIL_BEHAVIOR_MASK       ( 0x00000010 )

/**
 * @brief 이벤트 Handler Prototype
 * @param[in] aData Event Private Information
 * @param[in] aDataSize Information Size
 * @param[out] aDone Event 성공 여부
 * @param[in,out] Evironment 포인터
 */
typedef stlStatus (*knlEventHandler) ( void      * aData,
                                       stlUInt32   aDataSize,
                                       stlBool   * aDone,
                                       void      * aEnv );

/**
 * @brief EVENT Description 크기
 */
#define KNL_EVENT_MAX_DESC_SIZE (32)

/**
 * @brief 에러 레이어 마스크(상위 10비트 사용)
 */
#define KNL_EVENT_LAYER_MASK         0xFFC00000
/**
 * @brief 에러 순차값 마스크(하위 22비트 사용)
 */
#define KNL_EVENT_VALUE_MASK         0x003FFFFF
/**
 * @brief EVENT 순차값 정밀도
 */
#define KNL_EVENT_VALUE_PRECISION    22
/**
 * @brief Event 코드를 구성한다.
 * Event 코드는 32비트 정수를 사용하며,
 * 상위 10비트의 레이어와 하위 22비트의 Event 순차값으로 구성된다.
 */
#define KNL_MAKE_EVENT_ID( aEventLayer, aEventValue )     \
    ( aEventValue + ( aEventLayer << KNL_EVENT_VALUE_PRECISION ) )
/**
 * @brief Event 코드로부터 Layer를 구한다.
 */
#define KNL_GET_EVENT_LAYER( aEventId )                   \
    ( (aEventId & KNL_EVENT_LAYER_MASK) >> KNL_EVENT_VALUE_PRECISION )
/**
 * @brief Event 코드로부터 Event Value를 구한다.
 */
#define KNL_GET_EVENT_VALUE( aEventId )  ( aEventId & KNL_EVENT_VALUE_MASK )

/**
 * @brief Shared Memory Attach Event
 */
#define KNL_EVENT_ATTACH_SHM   KNL_MAKE_EVENT_ID( STL_LAYER_KERNEL, 0 )

/**
 * @brief Shared Memory Detach Event
 */
#define KNL_EVENT_DETACH_SHM   KNL_MAKE_EVENT_ID( STL_LAYER_KERNEL, 1 )

/**
 * @brief Startup event
 */
#define KNL_EVENT_STARTUP      KNL_MAKE_EVENT_ID( STL_LAYER_KERNEL, 2 )

/**
 * @brief Shutdown event
 */
#define KNL_EVENT_SHUTDOWN     KNL_MAKE_EVENT_ID( STL_LAYER_KERNEL, 3 )

/**
 * @brief Shutdown event
 */
#define KNL_EVENT_CLEANUP      KNL_MAKE_EVENT_ID( STL_LAYER_KERNEL, 4 )

/**
 * @brief Kernel Maximum Event Count
 */
#define KNL_MAX_EVENT_COUNT (5)

/**
 * @brief 이벤트 Listener 구조체
 */
typedef struct knlEventListener
{
    stlSemaphore  mSem;      /**< 세마포어 */
} knlEventListener;

/**
 * @brief 이벤트 구조체
 */
typedef struct knlEvent
{
    stlUInt32          mSpinLock;            /**< Spin Lock */
    stlUInt32          mEventCount;          /**< 추가된 Event의 총 개수 */
    stlInt64           mCurEsn;              /**< Current Event Sequence Number */
    stlInt64           mCompletedEsn;        /**< Completed Event Sequence Number */
    stlRingHead        mEventHead;           /**< Event List Header */
    knlEventListener * mEventListener;       /**< Event Listener */
} knlEvent;

/**
 * @brief 이벤트 Table 구조체
 */
typedef struct knlEventTable
{
    stlChar         mEventDesc[KNL_EVENT_MAX_DESC_SIZE];  /**< Event Description */
    knlEventHandler mEventHandler;                        /**< Event를 처리할 Handler */
} knlEventTable;

/**
 * @brief initialize shutdown event arguments
 */
#define KNL_INIT_SHUTDOWN_EVENT_ARG( aArg )                 \
    {                                                       \
        (aArg)->mShutdownMode = KNL_SHUTDOWN_MODE_NONE;     \
        (aArg)->mShutdownPhase = KNL_SHUTDOWN_PHASE_NONE;   \
    }

typedef struct knlShutdownEventArg
{
    stlInt32   mShutdownMode;      /**< Shutdown mode ( abort, normal, ... ) */
    stlInt32   mShutdownPhase;     /**< Shutdown phase */
} knlShutdownEventArg;

/** @} */

/**
 * @addtogroup knlShmManager
 * @{
 */

/**
 * @brief Static Area의 현재 정보를 저장
 */
typedef struct knlStaticAreaMark
{
    stlUInt64   mFixedOffset;
    stlUInt64   mInitSize;
    stlUInt64   mChunkSize;
    void      * mLastBlock;
} knlStaticAreaMark;

/**
 * @brief Static Area 메모리 정보를 저장하는 구조체를 초기화한다.
 * @remark 실제 사용은 knlMarkStaticArea() 함수를 호출해야 사용할 수 있다.
 * @see knlMarkStaticArea()
 */
#define KNL_INIT_STATIC_AREA_MARK( aMark )      \
    (aMark)->mFixedOffset = 0;                  \
    (aMark)->mInitSize    = 0;                  \
    (aMark)->mChunkSize   = 0;                  \
    (aMark)->mLastBlock   = NULL

/** @} */

/**
 * @defgroup knlMemManager Memory Management
 * @{
 */

/**
 * @brief allocator type
 */
typedef enum
{
    KNL_ALLOCATOR_TYPE_NONE,
    KNL_ALLOCATOR_TYPE_REGION,   /**< region allocator */
    KNL_ALLOCATOR_TYPE_DYNAMIC   /**< dynamic allocator */
} knlAllocatorType;

/**
 * @defgroup knlMemType Memory Type
 * @ingroup knlMemManager
 * @{
 */
#define KNL_MEM_STORAGE_TYPE_NONE       (0x00000000)  /**< none */
#define KNL_MEM_STORAGE_TYPE_HEAP       (0x00000001)  /**< heap memory */
#define KNL_MEM_STORAGE_TYPE_SHM        (0x00000002)  /**< shared memory */
#define KNL_MEM_STORAGE_TYPE_PLAN       (0x00000004)  /**< plan shared memory ( only dynamic allocator ) */
#define KNL_MEM_STORAGE_TYPE_MAX        (0x0000000F)  /**< max type */
#define KNL_MEM_STORAGE_TYPE_MASK       (0x0000000F)  /**< memory type mask */
/** @} */

/**
 * @defgroup knlMemAttr Allocator Attribute
 * @ingroup knlMemManager
 * @{
 */
#define KNL_MEM_ATTR_NONE              (0x00000000)  /**< none */
#define KNL_MEM_ATTR_KEEP_EMPTY_CHUNK  (0x00000001)  /**< keep empty chunk */
#define KNL_MEM_ATTR_MASK              (0x00000001)  /**< memory attr mask */
/** @} */

/**
 * @defgroup knlTermSequence Termination Sequence
 * @ingroup knlMemManager
 * @{
 */
#define KNL_UNMARK_SEQ_ORDERED   (0x00000000)  /**< has ordered unmark sequence */
#define KNL_UNMARK_SEQ_RANDOM    (0x00000010)  /**< has random unmark sequence */
#define KNL_UNMARK_SEQ_MAX       (0x00000020)  /**< max type */
#define KNL_UNMARK_SEQ_MASK      (0x00000010)  /**< unmark sequence mask */
/** @} */

/**
 * @defgroup knlAllocatorLevel Allocator Level
 * @ingroup knlMemManager
 * @{
 */
#define KNL_ALLOCATOR_LEVEL_INTERNAL   (0x00000000)  /**< internal allocator */
#define KNL_ALLOCATOR_LEVEL_ROOT       (0x00000100)  /**< root allocator */
#define KNL_ALLOCATOR_LEVEL_MASK       (0x00000100)  /**< allocator type mask */
/** @} */

/**
 * 할당 가능한 크기는 프로퍼티에 종속적이다.
 * @remark KNL_ALLOCATOR_TYPE_LEAF에만 적용된다.
 */
#define KNL_MEMORY_TOTAL_SIZE_DEPEND_ON_PROPERTY   (STL_INT64_MAX - 1)

/**
 * @brief 메모리 사용 카테고리
 */
typedef enum
{
    KNL_MEM_CATEGORY_NONE = 0,    /**< 분류 없음 */
    KNL_MEM_CATEGORY_OPERATION,   /**< 연산에서 사용하는 메모리 */
    KNL_MEM_CATEGORY_STATEMENT,   /**< statement에서 사용하는 메모리 */
    KNL_MEM_CATEGORY_TRANSACTION, /**< 트랜잭션에서 사용하는 메모리 */
    KNL_MEM_CATEGORY_SESSION,     /**< 세션에서 사용하는 메모리 */
    KNL_MEM_CATEGORY_DATABASE,    /**< 데이터베이스에서 사용하는 메모리 */
    KNL_MEM_CATEGORY_MAX
} knlMemCategory;

/**
 * @brief 메모리 관리자의 정보의 최대 길이
 */
#define KNL_ALLOCATOR_MAX_DESC_SIZE  ( 64 )

/**
 * @brief description structure for allocators
 */
typedef struct knlAllocatorDesc
{
    stlInt64         mAllocatorId;       /**< allocator identifier */
    knlAllocatorType mAllocatorType;     /**< allocator type(dynamic,region) */
    stlUInt32        mStorageType;       /**< storage type(heap,shm,plan) */
    stlUInt32        mMemAttr;           /**< attribute(keep empty chunk or not) */
    stlUInt32        mUnmarkType;        /**< unmark type(ordered, random) */
    stlUInt32        mAllocatorLevel;    /**< level(root, internal) */
    knlMemCategory   mAllocatorCategory; /**< category(database,session,transaction, ...) */
    stlUInt32        mMinFragSize;       /**< minimum fragment size for dynamic allocator */
    stlLayerClass    mLayer;             /**< layer(standard,kernel, ...) */
    stlChar        * mDescription;       /**< description */
} knlAllocatorDesc;

/**
 * @brief invalid allocator identifier
 */
#define KNL_INVALID_ALLOCATOR_ID ( -1 )

/**
 * @brief allocator identifier
 */
typedef enum knlAllocatorId
{
    /*
     * dynamic allocator for kernel layer
     */

    KNL_ALLOCATOR_KERNEL_SHARED_VARIABLE_STATIC_AREA,
    KNL_ALLOCATOR_KERNEL_PRIVATE_STATIC_AREA,
    KNL_ALLOCATOR_KERNEL_EVENT,
    KNL_ALLOCATOR_KERNEL_SQL_FIXING_BLOCK,
    KNL_ALLOCATOR_KERNEL_BIND_PARAMETER,
    KNL_ALLOCATOR_KERNEL_LONG_VARYING,
    KNL_ALLOCATOR_KERNEL_PLAN_CACHE,
    KNL_ALLOCATOR_KERNEL_FLANGE,
    KNL_ALLOCATOR_KERNEL_OPERERATION,
    KNL_ALLOCATOR_KERNEL_FILE_MANAGER,

    /*
     * region allocator for kernel layer
     */

    KNL_ALLOCATOR_KERNEL_VARCHAR_PROPERTY,
    KNL_ALLOCATOR_KERNEL_PROPERTY_MANAGER,
    KNL_ALLOCATOR_KERNEL_FIXED_TABLE,

    /*
     * region allocator for Server Communication layer
     */

    KNL_ALLOCATOR_SERVER_COMMUNICATION_REGION_SESSION,
    KNL_ALLOCATOR_SERVER_COMMUNICATION_REGION_SEND_POLLING,

    /*
     * dynamic allocator for storage manager layer
     */

    KNL_ALLOCATOR_STORAGE_MANAGER_MOUNT,
    KNL_ALLOCATOR_STORAGE_MANAGER_OPEN,
    KNL_ALLOCATOR_STORAGE_MANAGER_SESSION_DYNAMIC,
    KNL_ALLOCATOR_STORAGE_MANAGER_PARENT_TRANSACTION,
    KNL_ALLOCATOR_STORAGE_MANAGER_TRANSACTION,
    KNL_ALLOCATOR_STORAGE_MANAGER_PARENT_LOCK_TABLE,
    KNL_ALLOCATOR_STORAGE_MANAGER_LOCK_TABLE,
    KNL_ALLOCATOR_STORAGE_MANAGER_DYNAMIC_OPERATION,
    KNL_ALLOCATOR_STORAGE_MANAGER_PARENT_STATEMENT,

    /*
     * region allocator for storage manager layer
     */
    
    KNL_ALLOCATOR_STORAGE_MANAGER_REGION_OPERATION,
    KNL_ALLOCATOR_STORAGE_MANAGER_LOG_OPERATION,
    KNL_ALLOCATOR_STORAGE_MANAGER_SESSION_REGION,
    KNL_ALLOCATOR_STORAGE_MANAGER_STATEMENT,
    
    /*
     * dynamic allocator for execution library layer
     */
    
    KNL_ALLOCATOR_EXECUTION_LIBRARY_NOMOUNT_DICT_CACHE,
    KNL_ALLOCATOR_EXECUTION_LIBRARY_NOMOUNT_HASH_ELEMENT,
    KNL_ALLOCATOR_EXECUTION_LIBRARY_NOMOUNT_HASH_RELATED_OBJECT,
    KNL_ALLOCATOR_EXECUTION_LIBRARY_NOMOUNT_PENDING_OPERATION,
    KNL_ALLOCATOR_EXECUTION_LIBRARY_MOUNT_DICT_CACHE,
    KNL_ALLOCATOR_EXECUTION_LIBRARY_MOUNT_HASH_ELEMENT,
    KNL_ALLOCATOR_EXECUTION_LIBRARY_MOUNT_HASH_RELATED_OBJECT,
    KNL_ALLOCATOR_EXECUTION_LIBRARY_MOUNT_PENDING_OPERATION,
    KNL_ALLOCATOR_EXECUTION_LIBRARY_DICT_CACHE,
    KNL_ALLOCATOR_EXECUTION_LIBRARY_HASH_ELEMENT,
    KNL_ALLOCATOR_EXECUTION_LIBRARY_HASH_RELATED_OBJECT,
    KNL_ALLOCATOR_EXECUTION_LIBRARY_DICT_RELATED_TRANSACTION,
    KNL_ALLOCATOR_EXECUTION_LIBRARY_PENDING_OPERATION,
    KNL_ALLOCATOR_EXECUTION_LIBRARY_SESSION_OBJECT,
    KNL_ALLOCATOR_EXECUTION_LIBRARY_CURSOR_SLOT,

    /*
     * region allocator for execution library layer
     */
    
    KNL_ALLOCATOR_EXECUTION_LIBRARY_DICT_OPERATION,
    KNL_ALLOCATOR_EXECUTION_LIBRARY_SESSION_HASH,

    /*
     * dynamic allocator for sql processor layer
     */

    KNL_ALLOCATOR_SQL_PROCESSOR_XA_CONTEXT,
    
    KNL_ALLOCATOR_SQL_PROCESSOR_COLLISION,
    KNL_ALLOCATOR_SQL_PROCESSOR_NLS,
    KNL_ALLOCATOR_SQL_PROCESSOR_PARENT_DBC,

    /*
     * region allocator for sql processor layer
     */
    
    KNL_ALLOCATOR_SQL_PROCESSOR_SHARE_SYNTAX_SQL,
    KNL_ALLOCATOR_SQL_PROCESSOR_SHARE_INIT_PLAN,
    KNL_ALLOCATOR_SQL_PROCESSOR_NON_PLAN_CACHE,
    KNL_ALLOCATOR_SQL_PROCESSOR_PLAN_CACHE_FOR_ADMIN_SESSION,
    KNL_ALLOCATOR_SQL_PROCESSOR_PLAN_CACHE,
    KNL_ALLOCATOR_SQL_PROCESSOR_SHARE_DATA_PLAN,
    KNL_ALLOCATOR_SQL_PROCESSOR_SHARE_STATEMENT,
    KNL_ALLOCATOR_SQL_PROCESSOR_PARSING,
    KNL_ALLOCATOR_SQL_PROCESSOR_RUNNING,
    
    /*
     * dynamic allocator for session layer
     */
    
    KNL_ALLOCATOR_SESSION_SHARED_MEMORY,
    KNL_ALLOCATOR_SESSION_DYNAMIC_STATEMENT,

    /*
     * region allocator for session layer
     */
    
    KNL_ALLOCATOR_SESSION_REGION_STATEMENT_PARAM,
    KNL_ALLOCATOR_SESSION_REGION_STATEMENT_COLUMN,
    KNL_ALLOCATOR_SESSION_OPERATION,

    /*
     * dynamic allocator for server library layer
     */
    
    KNL_ALLOCATOR_SERVER_LIBRARY_SHARED_HANDLE,

    /*
     * dynamic allocator for gliese tool layer
     */
    
    KNL_ALLOCATOR_DISPATCHER_DYNAMIC,
    KNL_ALLOCATOR_GBALANCER,
    
    /*
     * invalid max allocator
     */
    
    KNL_ALLOCATOR_MAX
} knlAllocatorId;

/**
 * @addtogroup knlMemController
 * @{
 */

/**
 * @brief 다수개의 Dynamic Memory 관리자의 메모리 크기를 제한한다.
 */
typedef struct knlMemController
{
    knlLatch           mLatch;        /**< 동기화 객체 */
    stlUInt64          mMaxSize;      /**< 할당될수 있는 최대 메모리 크기 */
    stlUInt64          mTotalSize;    /**< Controller를 사용하는 모든 Dynamic Memory에
                                       * 할당된 모든 chunk 메모리의 크기 */
} knlMemController;

/** @} */

/**
 * @addtogroup knlDynamicMem
 * @{
 */

/**
 * @brief 할당할 기본 chunk의 크기
 */
#define KNL_DEFAULT_CHUNK_SIZE (1024)

/**
 * @brief 동적 메모리 관리자를 초기화한다.
 * @remark 실제 사용은 knlCreateDynamicMem() 함수를 호출해야 사용할 수 있다.
 * @see knlCreateDynamicMem()
 */
#define KNL_INIT_DYNAMIC_MEMORY( aDynamicMem )                  \
    (aDynamicMem)->mInitSize   = 0;                             \
    (aDynamicMem)->mNextSize   = 0;                             \
    (aDynamicMem)->mTotalSize  = 0;                             \
    (aDynamicMem)->mMaxSize    = 0;                             \
    (aDynamicMem)->mType       = KNL_MEM_STORAGE_TYPE_MAX;      \
    (aDynamicMem)->mController = NULL;                          \
    (aDynamicMem)->mCategory   = KNL_MEM_CATEGORY_MAX

/**
 * @brief 동적 메모리 관리자가 생성되었는지 묻는다.
 */
#define KNL_DYNAMIC_MEMORY_IS_CREATED( aDynamicMem )    \
    ( (aDynamicMem)->mType == KNL_MEM_STORAGE_TYPE_MAX ? STL_FALSE : STL_TRUE )
/**
 * @brief 동적 메모리 관리자의 MemType을 얻는다.
 */
#define KNL_DYNAMIC_MEMORY_TYPE( aDynamicMem )    \
    ( (aDynamicMem)->mType )

typedef struct knlDynamicMem  knlDynamicMem;

/**
 * @brief 동적으로 alloc, free가 가능한 메모리 관리자
 */
struct knlDynamicMem
{
    stlRingHead        mChunkRing;     /**< 할당된 chunk들의 ring */
    knlLatch           mLatch;         /**< 동기화 객체 */
    stlUInt32          mAttr;          /**< Allocator Attribute */
    stlInt32           mAllocatorId;   /**< Allocator Identifier */
    stlUInt64          mInitSize;      /**< 미리 할당할 메모리의 최소 크기 */
    stlUInt64          mNextSize;      /**< 추가적으로 메모리 할당시 할당할 메모리의 크기 */
    stlUInt64          mMaxSize;       /**< 할당될수 있는 최대 메모리 크기 */
    stlUInt64          mTotalSize;     /**< 할당된 모든 chunk 메모리의 크기 */
    stlUInt16          mCategory;      /**< 메모리 카테고리 */
    stlUInt16          mMinFragSize;   /**< Minimum Fragemnt Size */
    stlUInt16          mType;          /**< 메모리 타입 */
    stlUInt16          mAllocatorLevel; /**< Allocator Level */
    knlDynamicMem    * mParentMem;     /**< Parent Memory Allocator */
    knlMemController * mController;    /**< 동기화 객체 */
};

#if defined( STL_DEBUG ) || defined( DOXYGEN )
#define KNL_CHECK_DYNAMIC_MEMORY_BOUNDARY( aDynamicMem, aEnv )                                      \
    do                                                                                              \
    {                                                                                               \
        if( gKnEntryPoint != NULL )                                                                 \
        {                                                                                           \
            switch( (aDynamicMem)->mCategory )                                                      \
            {                                                                                       \
                case KNL_MEM_CATEGORY_NONE :                                                        \
                case KNL_MEM_CATEGORY_DATABASE :                                                    \
                    break;                                                                          \
                case KNL_MEM_CATEGORY_SESSION :                                                     \
                case KNL_MEM_CATEGORY_TRANSACTION :                                                 \
                case KNL_MEM_CATEGORY_STATEMENT :                                                   \
                case KNL_MEM_CATEGORY_OPERATION :                                                   \
                    STL_ASSERT( gKnEntryPoint->mShmManager != NULL );                               \
                                                                                                    \
                    if( ( (void*)gKnEntryPoint <= (void*)(aDynamicMem) ) &&                         \
                        ( (void*)(aDynamicMem) <= (void*)gKnEntryPoint + gKnEntryPoint->mShmManager->mSegment[0].mSize ) && \
                        ( (aDynamicMem)->mType == KNL_MEM_STORAGE_TYPE_HEAP ) )                             \
                    {                                                                               \
                        STL_ASSERT( KNL_IS_SHARED_ENV( (aEnv) ) == STL_TRUE );                      \
                    }                                                                               \
                    break;                                                                          \
                default :                                                                           \
                    STL_ASSERT( STL_FALSE );                                                        \
                    break;                                                                          \
            }                                                                                       \
        }                                                                                           \
    } while( 0 )
#else
#define KNL_CHECK_DYNAMIC_MEMORY_BOUNDARY( aDynamicMem, aEnv )
#endif

/** @} */

/**
 * @addtogroup knlRegionMem
 * @{
 */

/**
 * @brief 할당할 기본 region의 크기
 */
#define KNL_DEFAULT_REGION_SIZE               (128)
/**
 * @brief 할당할 기본 large region의 크기
 */
#define KNL_DEFAULT_LARGE_REGION_SIZE         (8 * 1024)
/**
 * @brief 할당할 기본 private static area의 크기
 */
#define KNL_DEFAULT_PRIVATE_STATIC_AREA_SIZE (10 * 1024 * 1024)

/**
 * @brief 영역 기반 메모리 관리자를 초기화한다.
 * @remark 실제 사용은 knlCreateRegionMem() 함수를 호출해야 사용할 수 있다.
 * @see knlCreateRegionMem()
 */
#define KNL_INIT_REGION_MEMORY( aRegionMem )                    \
    (aRegionMem)->mInitSize    = 0;                             \
    (aRegionMem)->mNextSize    = 0;                             \
    (aRegionMem)->mTotalSize   = 0;                             \
    (aRegionMem)->mMemType     = KNL_MEM_STORAGE_TYPE_MAX;      \
    (aRegionMem)->mTermSeqType = KNL_UNMARK_SEQ_MAX;            \
    (aRegionMem)->mCategory    = KNL_MEM_CATEGORY_MAX

/**
 * @brief 영역 기반 메모리 관리자가 생성되었는지 묻는다.
 */
#define KNL_REGION_MEMORY_IS_CREATED( aRegionMem )    \
    ( (aRegionMem)->mMemType == KNL_MEM_STORAGE_TYPE_MAX ? STL_FALSE : STL_TRUE )
/**
 * @brief 영역 기반 메모리 관리자의 MemType을 얻는다.
 */
#define KNL_REGION_MEMORY_TYPE( aRegionMem )    \
    ( (aRegionMem)->mMemType )

/**
 * @brief 영역 기반 메모리 정보를 저장하는 구조체를 초기화한다.
 * @remark 실제 사용은 knlMarkRegionMem() 함수를 호출해야 사용할 수 있다.
 * @see knlMarkRegionMem()
 */
#define KNL_INIT_REGION_MARK( aMark )           \
    (aMark)->mOffset = 0;                       \
    (aMark)->mArena = NULL

/**
 * @brief 영역 기반 메모리 정보를 저장하는 구조체가 valid 한지 확인한다.
 */
#define KNL_IS_VALID_REGION_MARK( aMark )       \
    ( (aMark)->mArena != NULL )

/**
 * @brief 특정 크기의 메모리 영역안에서 메모리를 alloc 받으며,
 *        해당 메모리 영역을 한번에 free 하는 메모리 관리자
 */
typedef struct knlRegionMem
{
    stlRingHead     mAllocRing;    /**< 할당된 arena 들의 ring */
    stlRingHead     mFreeRing;     /**< 반환된 arena 들의 ring */
    stlInt32        mAllocatorId;  /**< Allocator Identifier */
    stlUInt64       mInitSize;     /**< 미리 할당할 메모리의 최소 크기 */
    stlUInt64       mNextSize;     /**< 추가적으로 메모리 할당시 할당할 메모리의 크기 */
    stlUInt64       mTotalSize;    /**< 할당된 모든 chunk 메모리의 크기 */
    stlUInt32       mCategory;     /**< 메모리 카테고리 */
    stlUInt32       mMemType;      /**< 메모리 타입 */
    stlUInt32       mTermSeqType;  /**< termination sequence */
    stlBool         mEnableAlloc;  /**< 사용 가능 상태 표시 */
    stlChar         mPadding[3];   /**< padding */
    stlRingHead     mMarkList;
    knlDynamicMem * mParentMem;    /**< parent dynamic memory manager */
} knlRegionMem;

/**
 * @brief Region Memory의 현재 정보를 저장
 */
typedef struct knlRegionMark
{
    stlUInt64   mOffset; /**< 마지막으로 할당된 메모리의 offset */
    void      * mArena;  /**< 현재 사용중인 area */
} knlRegionMark;

typedef struct knlRegionMarkEx
{
    knlRegionMark mRegionMark;
    stlRingEntry  mMarkLink;
    stlBool       mIsFree;
} knlRegionMarkEx;

#if defined( STL_DEBUG ) || defined( DOXYGEN )
#define KNL_CHECK_REGION_MEMORY_BOUNDARY( aRegionMem, aEnv )                                        \
    do                                                                                              \
    {                                                                                               \
        if( gKnEntryPoint != NULL )                                                                 \
        {                                                                                           \
            switch( (aRegionMem)->mCategory )                                                       \
            {                                                                                       \
                case KNL_MEM_CATEGORY_NONE :                                                        \
                case KNL_MEM_CATEGORY_DATABASE :                                                    \
                    break;                                                                          \
                case KNL_MEM_CATEGORY_SESSION :                                                     \
                case KNL_MEM_CATEGORY_TRANSACTION :                                                 \
                case KNL_MEM_CATEGORY_STATEMENT :                                                   \
                case KNL_MEM_CATEGORY_OPERATION :                                                   \
                    STL_ASSERT( gKnEntryPoint->mShmManager != NULL );                               \
                                                                                                    \
                    if( ( (void*)gKnEntryPoint <= (void*)(aRegionMem) ) &&                          \
                        ( (void*)(aRegionMem) <= (void*)gKnEntryPoint + gKnEntryPoint->mShmManager->mSegment[0].mSize ) && \
                        ( (aRegionMem)->mMemType == KNL_MEM_STORAGE_TYPE_HEAP ) )                           \
                    {                                                                               \
                        STL_ASSERT( KNL_IS_SHARED_ENV( (aEnv) ) == STL_TRUE );                      \
                    }                                                                               \
                    break;                                                                          \
                default :                                                                           \
                    STL_ASSERT( STL_FALSE );                                                        \
                    break;                                                                          \
            }                                                                                       \
        }                                                                                           \
    } while( 0 )
#else
#define KNL_CHECK_REGION_MEMORY_BOUNDARY( aRegionMem, aEnv )
#endif

/** @} */

/** @} */

/**
 * @addtogroup knlProperty
 * @{
 */

/**
 * @brief Property DataType
 * @remarks
 * <BR> Property의 값이 갖는 데이터 타입
 * <BR> 테스트를 위하여 여기에 넣지만 테스트가 끝난 이후에는 KNP에 넣어줘야 한다.
 */

typedef enum
{
    KNL_PROPERTY_TYPE_BOOL    = 0,      /**< BOOLEAN형 */
    KNL_PROPERTY_TYPE_BIGINT  = 1,      /**< 숫자형 */
    KNL_PROPERTY_TYPE_VARCHAR = 2,      /**< 문자형 */
    KNL_PROPERTY_TYPE_MAX     = 3       /**< Invalid */
}knlPropertyDataType;


/**
 * @brief SYS_MODIFY MODE
 * @remarks
 * <BR> ALTER SESSION, SYSTEM, DATABASE의 질의를 구분하기 위함
 */

typedef enum
{
    KNL_PROPERTY_MODE_NONE       = 0,  /**< 초기화에 사용 */
    KNL_PROPERTY_MODE_SES        = 1,  /**< ALTER SESSION 질의일 경우에 사용 */
    KNL_PROPERTY_MODE_SYS        = 2,  /**< ALTER SYSTEM 질의일 경우에 사용 */
    KNL_PROPERTY_MODE_DATABASE   = 3,  /**< ALTER DATABASE 질의일 경우에 사용 */
    KNL_PROPERTY_MODE_MAX        = 4   /**< Invalid Value */
}knlPropertyModifyMode;


/**
 * @brief ISSYS_MODIFIABLE TYPE
 * @remarks
 * <BR> ALTER SYSTEM 질의시 적용 시점을 구분
 */

typedef enum
{
    KNL_PROPERTY_SYS_TYPE_IMMEDIATE   = 0,  /**< 현재 접속되어 있는 세션 및 이후 접속되는 세션에 영향을 받음 */
    KNL_PROPERTY_SYS_TYPE_DEFERRED    = 1,  /**< 이후 접속되는 세션부터 변경된 프로퍼티의 영향을 받음 */
    KNL_PROPERTY_SYS_TYPE_FALSE       = 2,  /**< 운용중에는 변경할 수 없으며, ALTER SYSTEM ... SCOPE=FALSE를 사용해서만 변경 가능하며 Restart가 필요 */
    KNL_PROPERTY_SYS_TYPE_NONE        = 3,  /**< READ-ONLY 속성일 경우에는 ALTER SYSTEM이 불가능하다. */
    KNL_PROPERTY_SYS_TYPE_MAX         = 4   /**< Invalid Value */
}knlPropertyModifyType;


/**
 * @brief SYS_MODIFY SCOPE
 * @remarks
 * <BR> ALTER SYSTEM 질의시 적용 범위를 구분
 */
typedef enum
{
    KNL_PROPERTY_SCOPE_MEMORY    = 0,   /**< 해당 프로퍼티 값이 MEMORY에만 반영되며, Restart 시에는 적용되지 않음 */
    KNL_PROPERTY_SCOPE_FILE      = 1,   /**< 해당 프로퍼티의 변경이 Binary 프로퍼티 파일에만 반영되며, Restart 이후부터 적용 */
    KNL_PROPERTY_SCOPE_BOTH      = 2,   /**< 해당 프로퍼티의 값이 MEMORY, Binary 프로퍼티 파일에 반영되며 Restart 이후에도 적용 */
    KNL_PROPERTY_SCOPE_MAX       = 3    /**< Invalid Value */
}knlPropertyModifyScope;

/**
 * @brief DOMAIN
 * @remarks
 * <BR> Property Domain
 */
typedef enum
{
    KNL_PROPERTY_DOMAIN_EXTERNAL   = 0, /**< 사용자에게 보여줘도 되는 프로퍼티 */
    KNL_PROPERTY_DOMAIN_INTERNAL   = 1, /**< 내부적으로 사용되는 프로퍼티 */
    KNL_PROPERTY_DOMAIN_HIDDEN     = 2, /**< 내부적으로 사용되며 외부로 보여지면 안되는 프로퍼티 */
    KNL_PROPERTY_DOMAIN_MAX        = 3  /**< Invalid Value */
}knlPropertyDomain;

/**
 * @brief BIGINT Data Type을 갖는 Property의 Unit Type
 * @remarks
 * <BR> 
 */
typedef enum
{
    KNL_PROPERTY_BIGINT_UNIT_NONE            = 0,   /**< UNIT이 없음 */
    KNL_PROPERTY_BIGINT_UNIT_SIZE            = 1,   /**< SIZE Unit : byte */
    KNL_PROPERTY_BIGINT_UNIT_TIME            = 2,   /**< TIME Unit : Milli-second */
    KNL_PROPERTY_BIGINT_UNIT_TIMEZONE_OFFSET = 3,   /**< TIMEZONE_OFFSET Unit : second */
    KNL_PROPERTY_BIGINT_UNIT_MAX             = 4    /**< Invalid Value */
}knlPropertyBigintUnit;


/**
 * @brief PROPERTY Info in Environment.
 * @remarks
 * <BR> 
 */
typedef struct knlPropertyInfo
{
    stlInt64        mPropertyValue[KNL_PROPERTY_MAX_COUNT];          /**< Property가 들어갈 수 있는 최대 크기만큼 지정한다. */
    stlInt64        mPropertyInitValue[KNL_PROPERTY_MAX_COUNT];      /**< Property가 들어갈 수 있는 최대 크기만큼 지정한다. */
    stlBool         mModified[KNL_PROPERTY_MAX_COUNT];               /**< Property가 ALTER SESSION으로 변경되었는지 여부. */
    stlChar       * mVarcharValue;                                   /**< Property가 Varchar Value가 들어갈 공간 */
} knlPropertyInfo;


/**
 * @brief 프로퍼티 VARCHAR가 저장할 수 있는 최대 길이 : 8byte align을 위해서 8의 배수가 되어야 함
 */

#define KNL_PROPERTY_STRING_MAX_LENGTH   (STL_MAX_FILE_PATH_LENGTH + STL_MAX_FILE_NAME_LENGTH)

/**
 * @brief 프로퍼티 이름의 최대 길이
 */

#define KNL_PROPERTY_NAME_MAX_LENGTH     (128)

/**
 * @brief 프로퍼티 Fixed Table에 사용되는 임시 Buffer Size
 */

#define KNL_PROPERTY_FXTABLE_BUFFER_SIZE      (62)

/**
 * @brief X$PROPERTY 용 Record
 */
typedef struct knlPropertyFxRecord
{
    stlUInt16   mID;
    stlChar     mName[KNL_PROPERTY_NAME_MAX_LENGTH];
    stlChar     mValue[KNL_PROPERTY_STRING_MAX_LENGTH];
    stlChar     mSource[KNL_PROPERTY_FXTABLE_BUFFER_SIZE];
    stlChar     mInitValue[KNL_PROPERTY_STRING_MAX_LENGTH];
    stlChar     mInitSource[KNL_PROPERTY_FXTABLE_BUFFER_SIZE];
    stlInt32    mDataTypeID;  /* Dictionary 와의 논리적 관계를 유지하기 위한 DB Data Type ID */
    stlChar     mDataType[KNL_PROPERTY_FXTABLE_BUFFER_SIZE];
    stlChar     mUnitType[KNL_PROPERTY_FXTABLE_BUFFER_SIZE];
    stlChar     mStartupPhase[KNL_PROPERTY_FXTABLE_BUFFER_SIZE];
    stlChar     mGroup[KNL_PROPERTY_FXTABLE_BUFFER_SIZE];
    stlChar     mDesc[KNL_PROPERTY_STRING_MAX_LENGTH];
    
    stlChar     mWritable[KNL_PROPERTY_FXTABLE_BUFFER_SIZE];
    stlChar     mSesModifiable[KNL_PROPERTY_FXTABLE_BUFFER_SIZE];
    stlChar     mSysModifiable[KNL_PROPERTY_FXTABLE_BUFFER_SIZE];
    stlChar     mModifiable[KNL_PROPERTY_FXTABLE_BUFFER_SIZE];
    stlChar     mInternal[KNL_PROPERTY_FXTABLE_BUFFER_SIZE];
    
    stlInt64    mMin;
    stlInt64    mMax;
} knlPropertyFxRecord;
 

/**
 * @brief PROPERTY ID
 * @remarks
 * <BR> Element의 이름은 KNL_PROPERTY_(PROPERTY NAME)으로 지정한다.
 * <BR> knpPropertyList.c에 함께 추가해야 함
 */
typedef enum
{
    KNL_PROPERTY_SHARED_MEMORY_NAME = 0,
    KNL_PROPERTY_SHARED_MEMORY_KEY,
    KNL_PROPERTY_SHARED_MEMORY_SIZE,
    KNL_PROPERTY_SHARED_MEMORY_ADDRESS,
    KNL_PROPERTY_PRIVATE_STATIC_AREA_SIZE,
    KNL_PROPERTY_PROCESS_MAX_COUNT,
    KNL_PROPERTY_PORT_NO,            /* deprecated */
    KNL_PROPERTY_CLIENT_MAX_COUNT,
    KNL_PROPERTY_DATA_STORE_MODE,
    KNL_PROPERTY_USE_INSERTABLE_PAGE_HINT,
    KNL_PROPERTY_DEFAULT_INDEX_LOGGING,
    KNL_PROPERTY_DEFAULT_TABLE_LOCKING,
    KNL_PROPERTY_DEFAULT_INITRANS,
    KNL_PROPERTY_DEFAULT_MAXTRANS,
    KNL_PROPERTY_DEFAULT_PCTFREE,
    KNL_PROPERTY_DEFAULT_PCTUSED,
    KNL_PROPERTY_LOG_BUFFER_SIZE,
    KNL_PROPERTY_PENDING_LOG_BUFFER_COUNT,
    KNL_PROPERTY_CONTROL_FILE_COUNT,
    KNL_PROPERTY_CONTROL_FILE_0,
    KNL_PROPERTY_CONTROL_FILE_1,
    KNL_PROPERTY_CONTROL_FILE_2,
    KNL_PROPERTY_CONTROL_FILE_3,
    KNL_PROPERTY_CONTROL_FILE_4,
    KNL_PROPERTY_CONTROL_FILE_5,
    KNL_PROPERTY_CONTROL_FILE_6,
    KNL_PROPERTY_CONTROL_FILE_7,
    KNL_PROPERTY_LOG_BLOCK_SIZE,
    KNL_PROPERTY_LOG_FILE_SIZE,
    KNL_PROPERTY_LOG_DIR,
    KNL_PROPERTY_LOG_GROUP_COUNT,
    KNL_PROPERTY_TRANSACTION_TABLE_SIZE,
    KNL_PROPERTY_UNDO_RELATION_COUNT,
    KNL_PROPERTY_MINIMUM_UNDO_PAGE_COUNT,
    KNL_PROPERTY_TRANSACTION_COMMIT_WRITE_MODE,
    KNL_PROPERTY_SYSTEM_TABLESPACE_DIR,
    KNL_PROPERTY_SYSTEM_MEMORY_DICT_TABLESPACE_SIZE,
    KNL_PROPERTY_SYSTEM_MEMORY_UNDO_TABLESPACE_SIZE,
    KNL_PROPERTY_SYSTEM_MEMORY_DATA_TABLESPACE_SIZE,
    KNL_PROPERTY_SYSTEM_MEMORY_TEMP_TABLESPACE_SIZE,
    KNL_PROPERTY_USER_MEMORY_DATA_TABLESPACE_SIZE,
    KNL_PROPERTY_USER_MEMORY_TEMP_TABLESPACE_SIZE,
    KNL_PROPERTY_SYSTEM_LOGGER_DIR,
    KNL_PROPERTY_ARCHIVELOG_MODE,
    KNL_PROPERTY_ARCHIVELOG_DIR,     /* deprecated */
    KNL_PROPERTY_ARCHIVELOG_FILE,
    KNL_PROPERTY_PARALLEL_LOAD_FACTOR,
    KNL_PROPERTY_PARALLEL_IO_FACTOR,
    KNL_PROPERTY_PARALLEL_IO_GROUP_1,
    KNL_PROPERTY_PARALLEL_IO_GROUP_2,
    KNL_PROPERTY_PARALLEL_IO_GROUP_3,
    KNL_PROPERTY_PARALLEL_IO_GROUP_4,
    KNL_PROPERTY_PARALLEL_IO_GROUP_5,
    KNL_PROPERTY_PARALLEL_IO_GROUP_6,
    KNL_PROPERTY_PARALLEL_IO_GROUP_7,
    KNL_PROPERTY_PARALLEL_IO_GROUP_8,
    KNL_PROPERTY_PARALLEL_IO_GROUP_9,
    KNL_PROPERTY_PARALLEL_IO_GROUP_10,
    KNL_PROPERTY_PARALLEL_IO_GROUP_11,
    KNL_PROPERTY_PARALLEL_IO_GROUP_12,
    KNL_PROPERTY_PARALLEL_IO_GROUP_13,
    KNL_PROPERTY_PARALLEL_IO_GROUP_14,
    KNL_PROPERTY_PARALLEL_IO_GROUP_15,
    KNL_PROPERTY_PARALLEL_IO_GROUP_16,
    KNL_PROPERTY_BLOCK_READ_COUNT,
    KNL_PROPERTY_NLS_DATE_FORMAT,
    KNL_PROPERTY_NLS_TIME_FORMAT,
    KNL_PROPERTY_NLS_TIME_WITH_TIME_ZONE_FORMAT,
    KNL_PROPERTY_NLS_TIMESTAMP_FORMAT,
    KNL_PROPERTY_NLS_TIMESTAMP_WITH_TIME_ZONE_FORMAT,
    KNL_PROPERTY_DDL_LOCK_TIMEOUT,
    KNL_PROPERTY_SHOW_CALLSTACK,
    KNL_PROPERTY_AGING_INTERVAL,
    KNL_PROPERTY_MAXIMUM_FLUSH_LOG_BLOCK_COUNT,
    KNL_PROPERTY_MAXIMUM_FLUSH_PAGE_COUNT,
    KNL_PROPERTY_TIMER_INTERVAL,      /* deprecated */
    KNL_PROPERTY_QUERY_TIMEOUT,
    KNL_PROPERTY_IDLE_TIMEOUT,
    KNL_PROPERTY_SPIN_COUNT,
    KNL_PROPERTY_BUSY_WAIT_COUNT,
    KNL_PROPERTY_DATABASE_FILE_IO,
    KNL_PROPERTY_LOG_FILE_IO,
    KNL_PROPERTY_SYSTEM_FILE_IO,
    KNL_PROPERTY_CACHE_ALIGNED_SIZE,
    KNL_PROPERTY_TRACE_DDL,
    KNL_PROPERTY_SUPPLEMENTAL_LOG_DATA_PRIMARY_KEY,
    KNL_PROPERTY_INDEX_BUILD_DIRECTION,
    KNL_PROPERTY_INDEX_BUILD_PARALLEL_FACTOR,
    KNL_PROPERTY_MEMORY_MERGE_RUN_COUNT,
    KNL_PROPERTY_MEMORY_SORT_RUN_SIZE,
    KNL_PROPERTY_DATABASE_ACCESS_MODE,
    KNL_PROPERTY_INST_TABLE_INST_MEM_MAX,
    KNL_PROPERTY_INST_TABLE_SORT_RUN_COUNT,
    KNL_PROPERTY_INST_TABLE_FIXED_COL_MAX_LEN,
    KNL_PROPERTY_INST_TABLE_VAR_COL_PIECE_MIN_LEN,
    KNL_PROPERTY_INST_TABLE_FIXED_ROW_MAX_LEN,
    KNL_PROPERTY_INST_TABLE_NODE_COUNT_PER_BLOCK,
    KNL_PROPERTY_DEVELOPMENT_OPTION,   /* 디버깅을 위해 런타임시 분기를 컨트롤하기 위한 프로퍼티 */
    KNL_PROPERTY_INDEX_ROOT_MIRRORING_DEPTH,
    KNL_PROPERTY_INDEX_ROOT_MAXIMIZE_FANOUT,
    KNL_PROPERTY_USE_LARGE_PAGES,
    KNL_PROPERTY_LOG_SYNC_INTERVAL,
    KNL_PROPERTY_INST_TABLE_SORT_METHOD,
    KNL_PROPERTY_LOCK_HASH_TABLE_SIZE,
    KNL_PROPERTY_INST_TABLE_MAX_KEY_LEN_FOR_LSD_RADIX,
    KNL_PROPERTY_PROPAGATE_REDO_LOG,
    KNL_PROPERTY_MAXIMUM_CONCURRENT_ACTIVITIES,
    KNL_PROPERTY_NET_BUFFER_SIZE,
    KNL_PROPERTY_PROCESS_LOGGER_DIR,
    KNL_PROPERTY_TRACE_LOG_ID,
    KNL_PROPERTY_LOGGER_FILE_ID,
    KNL_PROPERTY_GENERATE_CORE_DUMP,
    KNL_PROPERTY_SESSION_FATAL_BEHAVIOR,
    KNL_PROPERTY_REFINE_RELATION,
    KNL_PROPERTY_MEDIA_RECOVERY_LOG_BUFFER_SIZE,
    KNL_PROPERTY_NUMA,
    KNL_PROPERTY_NUMA_MAP,
    KNL_PROPERTY_IN_DOUBT_DECISION,
    KNL_PROPERTY_TRACE_XA,
    KNL_PROPERTY_BACKUP_DIR,     /* deprecated */
    KNL_PROPERTY_CONTROL_FILE_TEMP_NAME,
    KNL_PROPERTY_DEFAULT_REMOVAL_OBSOLETE_BACKUP_LIST,
    KNL_PROPERTY_DEFAULT_REMOVAL_BACKUP_FILE,
    KNL_PROPERTY_PLAN_CACHE,
    KNL_PROPERTY_PLAN_CACHE_SIZE,
    KNL_PROPERTY_PLAN_CLOCK_COUNT,
    KNL_PROPERTY_LOG_MIRROR_MODE,
    KNL_PROPERTY_LOG_MIRROR_SHARED_MEMORY_SIZE,
    KNL_PROPERTY_PAGE_CHECKSUM_TYPE,
    KNL_PROPERTY_HINT_ERROR,
    KNL_PROPERTY_DATABASE_TEST_OPTION,
    KNL_PROPERTY_MAXIMUM_NAMED_CURSOR_COUNT,
    KNL_PROPERTY_SNAPSHOT_STATEMENT_TIMEOUT,
    KNL_PROPERTY_TIMEZONE,
    KNL_PROPERTY_CHARACTER_SET,
    KNL_PROPERTY_CHAR_LENGTH_UNITS,
    KNL_PROPERTY_DDL_AUTOCOMMIT,
    KNL_PROPERTY_UNDO_SHRINK_THRESHOLD,
    KNL_PROPERTY_READABLE_ARCHIVELOG_DIR_COUNT,
    KNL_PROPERTY_ARCHIVELOG_DIR_1,
    KNL_PROPERTY_ARCHIVELOG_DIR_2,
    KNL_PROPERTY_ARCHIVELOG_DIR_3,
    KNL_PROPERTY_ARCHIVELOG_DIR_4,
    KNL_PROPERTY_ARCHIVELOG_DIR_5,
    KNL_PROPERTY_ARCHIVELOG_DIR_6,
    KNL_PROPERTY_ARCHIVELOG_DIR_7,
    KNL_PROPERTY_ARCHIVELOG_DIR_8,
    KNL_PROPERTY_ARCHIVELOG_DIR_9,
    KNL_PROPERTY_ARCHIVELOG_DIR_10,
    KNL_PROPERTY_READABLE_BACKUP_DIR_COUNT,
    KNL_PROPERTY_BACKUP_DIR_1,
    KNL_PROPERTY_BACKUP_DIR_2,
    KNL_PROPERTY_BACKUP_DIR_3,
    KNL_PROPERTY_BACKUP_DIR_4,
    KNL_PROPERTY_BACKUP_DIR_5,
    KNL_PROPERTY_BACKUP_DIR_6,
    KNL_PROPERTY_BACKUP_DIR_7,
    KNL_PROPERTY_BACKUP_DIR_8,
    KNL_PROPERTY_BACKUP_DIR_9,
    KNL_PROPERTY_BACKUP_DIR_10,
    KNL_PROPERTY_TRACE_LOG_TIME_DETAIL,
    KNL_PROPERTY_LOG_MIRROR_TIMEOUT,
    KNL_PROPERTY_DISPATCHER_CM_BUFFER_SIZE,
    KNL_PROPERTY_DISPATCHER_CM_UNIT_SIZE,
    KNL_PROPERTY_SHARED_SESSION,
    KNL_PROPERTY_DISPATCHERS,
    KNL_PROPERTY_SHARED_SERVERS,
    KNL_PROPERTY_DISPATCHER_QUEUE_SIZE,
    KNL_PROPERTY_DISPATCHER_CONNECTIONS,
    KNL_PROPERTY_SHARED_REQUEST_QUEUE_COUNT,
    KNL_PROPERTY_MAXIMUM_SESSION_CM_BUFFER_SIZE,
    KNL_PROPERTY_BULK_IO_PAGE_COUNT,
    KNL_PROPERTY_DEFAULT_TABLE_TYPE,
    KNL_PROPERTY_COLUMNAR_TABLE_CONTINUOUS_COLUMN_THRESHOLD,
    KNL_PROPERTY_RECOMPILE_PAGE_PERCENT,
    KNL_PROPERTY_RECOMPILE_CHECK_MINIMUM_PAGE_COUNT,
    KNL_PROPERTY_TRANSACTION_MAXIMUM_UNDO_PAGE_COUNT,
    KNL_PROPERTY_TRACE_LOGIN,
    KNL_PROPERTY_MAXIMUM_FLANGE_COUNT,
    KNL_PROPERTY_AGING_PLAN_INTERVAL,
    KNL_PROPERTY_ID_MAX
} knlPropertyID;

/**
 * @brief Property Minimum Value
 */
#define KNL_PROPERTY_MIN( aPropertyId ) ( aPropertyId ## _MIN )

/**
 * @brief Property Maximum Value
 */
#define KNL_PROPERTY_MAX( aPropertyId ) ( aPropertyId ## _MAX )

/**
 * @brief Declare minimum and maximum value of property
 */
#define KNL_DECLARE_PROPERTY_RANGE( aPropertyId, aMinValue, aMaxValue ) \
    enum                                                                \
    {                                                                   \
        aPropertyId ## _MIN = (aMinValue),                              \
        aPropertyId ## _MAX = (aMaxValue)                               \
    }

KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_SHARED_MEMORY_NAME,
                            0,
                            8 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_SHARED_MEMORY_KEY,
                            0,
                            0 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_SHARED_MEMORY_SIZE,
                            STL_INT64_C(104857600),
                            STL_INT64_C(34359738368) );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_SHARED_MEMORY_ADDRESS,
                            0,
                            0 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_PRIVATE_STATIC_AREA_SIZE,
                            STL_INT64_C(104857600),
                            STL_INT64_C(34359738368) );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_PROCESS_MAX_COUNT,
                            12,
                            8192 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_PORT_NO,
                            1024,
                            30000 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_CLIENT_MAX_COUNT,
                            12,
                            8192 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_DATA_STORE_MODE,
                            1,
                            2 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_USE_INSERTABLE_PAGE_HINT,
                            0,
                            1 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_DEFAULT_INDEX_LOGGING,
                            0,
                            1 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_DEFAULT_TABLE_LOCKING,
                            0,
                            1 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_DEFAULT_INITRANS,
                            1,
                            32 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_DEFAULT_MAXTRANS,
                            1,
                            32 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_DEFAULT_PCTFREE,
                            0,
                            99 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_DEFAULT_PCTUSED,
                            0,
                            99 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_LOG_BUFFER_SIZE,
                            STL_INT64_C(1048576),
                            STL_INT64_C(10737418240) );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_PENDING_LOG_BUFFER_COUNT,
                            0,
                            32 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_CONTROL_FILE_COUNT,
                            2,
                            8 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_CONTROL_FILE_0,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_CONTROL_FILE_1,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_CONTROL_FILE_2,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_CONTROL_FILE_3,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_CONTROL_FILE_4,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_CONTROL_FILE_5,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_CONTROL_FILE_6,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_CONTROL_FILE_7,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_LOG_BLOCK_SIZE,
                            512,
                            4096 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_LOG_FILE_SIZE,
                            STL_INT64_C(20971520),
                            STL_INT64_C(10737418240) );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_LOG_DIR,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_LOG_GROUP_COUNT,
                            4,
                            254 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_TRANSACTION_TABLE_SIZE,
                            64,
                            10240 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_UNDO_RELATION_COUNT,
                            8,
                            10240 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_MINIMUM_UNDO_PAGE_COUNT,
                            16,
                            1048576 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_TRANSACTION_COMMIT_WRITE_MODE,
                            0,
                            1 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_SYSTEM_TABLESPACE_DIR,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_SYSTEM_MEMORY_DICT_TABLESPACE_SIZE,
                            STL_INT64_C(134217728),
                            STL_INT64_C(32212254720) );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_SYSTEM_MEMORY_UNDO_TABLESPACE_SIZE,
                            STL_INT64_C(33554432),
                            STL_INT64_C(32212254720) );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_SYSTEM_MEMORY_DATA_TABLESPACE_SIZE,
                            STL_INT64_C(33554432),
                            STL_INT64_C(32212254720) );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_SYSTEM_MEMORY_TEMP_TABLESPACE_SIZE,
                            STL_INT64_C(33554432),
                            STL_INT64_C(32212254720) );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_USER_MEMORY_DATA_TABLESPACE_SIZE,
                            STL_INT64_C(33554432),
                            STL_INT64_C(32212254720) );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_USER_MEMORY_TEMP_TABLESPACE_SIZE,
                            STL_INT64_C(3554432),
                            STL_INT64_C(32212254720) );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_SYSTEM_LOGGER_DIR,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_ARCHIVELOG_MODE,
                            0,
                            1 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_ARCHIVELOG_DIR,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_ARCHIVELOG_FILE,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_PARALLEL_LOAD_FACTOR,
                            1,
                            64 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_PARALLEL_IO_FACTOR,
                            1,
                            16 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_PARALLEL_IO_GROUP_1,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_PARALLEL_IO_GROUP_2,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_PARALLEL_IO_GROUP_3,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_PARALLEL_IO_GROUP_4,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_PARALLEL_IO_GROUP_5,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_PARALLEL_IO_GROUP_6,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_PARALLEL_IO_GROUP_7,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_PARALLEL_IO_GROUP_8,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_PARALLEL_IO_GROUP_9,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_PARALLEL_IO_GROUP_10,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_PARALLEL_IO_GROUP_11,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_PARALLEL_IO_GROUP_12,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_PARALLEL_IO_GROUP_13,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_PARALLEL_IO_GROUP_14,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_PARALLEL_IO_GROUP_15,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_PARALLEL_IO_GROUP_16,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_BLOCK_READ_COUNT,
                            1,
                            65536 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_NLS_DATE_FORMAT,
                            0,
                            DTL_NLS_DATETIME_FORMAT_NULL_TERMINATED_STRING_MAX_SIZE );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_NLS_TIME_FORMAT,
                            0,
                            DTL_NLS_DATETIME_FORMAT_NULL_TERMINATED_STRING_MAX_SIZE );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_NLS_TIME_WITH_TIME_ZONE_FORMAT,
                            0,
                            DTL_NLS_DATETIME_FORMAT_NULL_TERMINATED_STRING_MAX_SIZE );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_NLS_TIMESTAMP_FORMAT,
                            0,
                            DTL_NLS_DATETIME_FORMAT_NULL_TERMINATED_STRING_MAX_SIZE );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_NLS_TIMESTAMP_WITH_TIME_ZONE_FORMAT,
                            0,
                            DTL_NLS_DATETIME_FORMAT_NULL_TERMINATED_STRING_MAX_SIZE );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_DDL_LOCK_TIMEOUT,
                            0,
                            100000000 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_SHOW_CALLSTACK,
                            0,
                            1 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_AGING_INTERVAL,
                            1,
                            100000000 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_MAXIMUM_FLUSH_LOG_BLOCK_COUNT,
                            1000,
                            2000000 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_MAXIMUM_FLUSH_PAGE_COUNT,
                            1,
                            8192 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_TIMER_INTERVAL,
                            1,
                            100000000 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_QUERY_TIMEOUT,
                            0,
                            10000000 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_IDLE_TIMEOUT,
                            0,
                            10000000 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_SPIN_COUNT,
                            1,
                            1000000 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_BUSY_WAIT_COUNT,
                            1,
                            1000000 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_DATABASE_FILE_IO,
                            0,
                            1 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_LOG_FILE_IO,
                            0,
                            1 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_SYSTEM_FILE_IO,
                            0,
                            1 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_CACHE_ALIGNED_SIZE,
                            8,
                            512 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_TRACE_DDL,
                            0,
                            1 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_SUPPLEMENTAL_LOG_DATA_PRIMARY_KEY,
                            0,
                            1 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_INDEX_BUILD_DIRECTION,
                            0,
                            1 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_INDEX_BUILD_PARALLEL_FACTOR,
                            0,
                            64 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_MEMORY_MERGE_RUN_COUNT,
                            2,
                            64 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_MEMORY_SORT_RUN_SIZE,
                            8192,
                            32768 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_DATABASE_ACCESS_MODE,
                            0,
                            1 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_INST_TABLE_INST_MEM_MAX,
                            0,
                            10000000 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_INST_TABLE_SORT_RUN_COUNT,
                            16,
                            10000000 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_INST_TABLE_FIXED_COL_MAX_LEN,
                            0,
                            8000 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_INST_TABLE_VAR_COL_PIECE_MIN_LEN,
                            16,
                            1000 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_INST_TABLE_FIXED_ROW_MAX_LEN,
                            32,
                            260000 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_INST_TABLE_NODE_COUNT_PER_BLOCK,
                            4,
                            256 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_DEVELOPMENT_OPTION,
                            0,
                            STL_INT64_MAX );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_INDEX_ROOT_MIRRORING_DEPTH,
                            2,
                            8 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_INDEX_ROOT_MAXIMIZE_FANOUT,
                            0,
                            1 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_USE_LARGE_PAGES,
                            0,
                            1 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_LOG_SYNC_INTERVAL,
                            1,
                            10000 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_INST_TABLE_SORT_METHOD,
                            0,
                            3 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_LOCK_HASH_TABLE_SIZE,
                            2,
                            1000000 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_INST_TABLE_MAX_KEY_LEN_FOR_LSD_RADIX,
                            0,
                            8000 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_PROPAGATE_REDO_LOG,
                            0,
                            1 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_MAXIMUM_CONCURRENT_ACTIVITIES,
                            1,
                            65535 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_NET_BUFFER_SIZE,
                            1024,
                            1073741824 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_PROCESS_LOGGER_DIR,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_TRACE_LOG_ID,
                            0,
                            100000000 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_LOGGER_FILE_ID,
                            0,
                            65535 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_GENERATE_CORE_DUMP,
                            0,
                            1 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_SESSION_FATAL_BEHAVIOR,
                            0,
                            1 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_REFINE_RELATION,
                            0,
                            1 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_MEDIA_RECOVERY_LOG_BUFFER_SIZE,
                            786432,
                            33554432 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_NUMA,
                            0,
                            1 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_NUMA_MAP,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_IN_DOUBT_DECISION,
                            1,
                            2 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_TRACE_XA,
                            0,
                            1 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_BACKUP_DIR,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_CONTROL_FILE_TEMP_NAME,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_DEFAULT_REMOVAL_OBSOLETE_BACKUP_LIST,
                            0,
                            1 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_DEFAULT_REMOVAL_BACKUP_FILE,
                            0,
                            1 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_PLAN_CACHE,
                            0,
                            1 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_PLAN_CACHE_SIZE,
                            STL_INT64_C(20971520),
                            STL_INT64_C(34359738368) );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_PLAN_CLOCK_COUNT,
                            1,
                            256 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_LOG_MIRROR_MODE,
                            0,
                            1 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_LOG_MIRROR_SHARED_MEMORY_SIZE,
                            STL_INT64_C(10485760),
                            STL_INT64_C(1073741824) );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_PAGE_CHECKSUM_TYPE,
                            0,
                            1 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_HINT_ERROR,
                            0,
                            1 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_DATABASE_TEST_OPTION,
                            0,
                            2 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_MAXIMUM_NAMED_CURSOR_COUNT,
                            1,
                            100000 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_SNAPSHOT_STATEMENT_TIMEOUT,
                            1,
                            22118400 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_TIMEZONE,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_CHARACTER_SET,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_CHAR_LENGTH_UNITS,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_DDL_AUTOCOMMIT,
                            0,
                            1 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_UNDO_SHRINK_THRESHOLD,
                            STL_INT64_C(1048576),
                            STL_INT64_C(107374182400) );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_READABLE_ARCHIVELOG_DIR_COUNT,
                            1,
                            10 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_ARCHIVELOG_DIR_1,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_ARCHIVELOG_DIR_2,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_ARCHIVELOG_DIR_3,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_ARCHIVELOG_DIR_4,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_ARCHIVELOG_DIR_5,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_ARCHIVELOG_DIR_6,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_ARCHIVELOG_DIR_7,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_ARCHIVELOG_DIR_8,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_ARCHIVELOG_DIR_9,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_ARCHIVELOG_DIR_10,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_READABLE_BACKUP_DIR_COUNT,
                            1,
                            10 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_BACKUP_DIR_1,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_BACKUP_DIR_2,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_BACKUP_DIR_3,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_BACKUP_DIR_4,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_BACKUP_DIR_5,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_BACKUP_DIR_6,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_BACKUP_DIR_7,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_BACKUP_DIR_8,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_BACKUP_DIR_9,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_BACKUP_DIR_10,
                            0,
                            KNL_PROPERTY_STRING_MAX_LENGTH );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_TRACE_LOG_TIME_DETAIL,
                            0,
                            1 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_LOG_MIRROR_TIMEOUT,
                            0,
                            100000000 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_DISPATCHER_CM_BUFFER_SIZE,
                            STL_INT64_C(10485760),
                            STL_INT64_C(34359738368) );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_DISPATCHER_CM_UNIT_SIZE,
                            1024,
                            10485760 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_SHARED_SESSION,
                            0,
                            1 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_DISPATCHERS,
                            1,
                            256 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_SHARED_SERVERS,
                            1,
                            256 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_DISPATCHER_QUEUE_SIZE,
                            1024,
                            32768 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_DISPATCHER_CONNECTIONS,
                            10,
                            4096 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_SHARED_REQUEST_QUEUE_COUNT,
                            1,
                            16 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_MAXIMUM_SESSION_CM_BUFFER_SIZE,
                            STL_INT64_C(1048576),
                            STL_INT64_C(1073741824) );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_BULK_IO_PAGE_COUNT,
                            STL_INT64_C(128),        /* 1M */
                            STL_INT64_C(131072) );   /* 1G */
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_DEFAULT_TABLE_TYPE,
                            0,
                            1 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_COLUMNAR_TABLE_CONTINUOUS_COLUMN_THRESHOLD,
                            STL_INT64_C(8),
                            STL_INT64_C(3000) );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_RECOMPILE_PAGE_PERCENT,
                            0,
                            1000 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_RECOMPILE_CHECK_MINIMUM_PAGE_COUNT,
                            1,
                            10000 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_TRANSACTION_MAXIMUM_UNDO_PAGE_COUNT,
                            1,             /* 8K */
                            838860800 );   /* 100G */
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_TRACE_LOGIN,
                            0,
                            1 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_MAXIMUM_FLANGE_COUNT,
                            128,
                            65535 );
KNL_DECLARE_PROPERTY_RANGE( KNL_PROPERTY_AGING_PLAN_INTERVAL,
                            0,
                            31536000 );

/** @} */

/**
 * @defgroup knlFilter Filter
 * @ingroup knExternal
 * @{
 */

/**
 * @brief dtlValueEntry로 구성된 value stack
 */ 
typedef struct knlValueStack
{
    stlUInt32        mMaxEntryCount;    /**< comparison stack의 크기 (단위 : dtlExprEntry 개수) */
    stlUInt32        mValueEntryCount;  /**< comparison stack내에 저장된 entry 개수 */
    dtlValueEntry  * mEntries;          /**< expr entry로 구성된 comparison stack : Array 구조 */
} knlValueStack;

/** @} */

/**
 * @addtogroup knlGeneral
 * @{
 */

#define KNL_IS_USER_LAYER( aTopLayer ) ( ((aTopLayer) > STL_LAYER_SERVER_LIBRARY) ? STL_TRUE : STL_FALSE )

typedef enum
{
    KNL_ENV_HEAP,
    KNL_ENV_SHARED
} knlEnvType;

#define KNL_WAITITEM_STATE_SUSPENDED ( 1 )
#define KNL_WAITITEM_STATE_RESUMED   ( 2 )

/**
 * @brief Kernel Environment 내의 Latch ID Stack의 최대 깊이
 */
#define KNL_ENV_MAX_LATCH_DEPTH   (32)

/**
 * @brief Invalid Process Identifier
 */
#define KNL_INVALID_PROCESS_ID    (0xFFFFFFFF)
/**
 * @brief Invalid Environment Identifier
 */
#define KNL_INVALID_ENV_ID        (0xFFFFFFFF)
/**
 * @brief Broadcast Process Identifier
 */
#define KNL_BROADCAST_PROCESS_ID  (0x8FFFFFFF)
/**
 * @brief Broadcast Environment Identifier
 */
#define KNL_BROADCAST_ENV_ID      (0x8FFFFFFF)
/**
 * @brief time to check thread cancellation ( 100ms )
 */
#define KNL_CHECK_CANCELLATION_TIME  ( 100000 )

/**
 * @brief Thread Cancellation을 검사한다.
 */
#define KNL_CHECK_THREAD_CANCELLATION( aEnv )           \
    {                                                   \
        if( ((aEnv)->mEnterNonCancelSecCount == 0) &&   \
            ((aEnv)->mCancelThread == STL_TRUE) )       \
        {                                               \
            stlNativeExitThread();                      \
        }                                               \
    }

/**
 * @brief thread cancel 정보를 설정한다.
 */
#define KNL_CANCEL_THREAD( aEnv )               \
    {                                           \
        if( (aEnv) != NULL )                    \
        {                                       \
            (aEnv)->mCancelThread = STL_TRUE;   \
        }                                       \
    }

/**
 * @brief Non-Cancellation Section에 진입한다.
 */
#define KNL_ENTER_NON_CANCELLABLE_SECTION( aEnv )                   \
    {                                                               \
        KNL_CHECK_THREAD_CANCELLATION( KNL_ENV(aEnv) );             \
        ((knlEnv*)((void*)aEnv))->mEnterNonCancelSecCount++;        \
    }

/**
 * @brief Non-Cancellation Section을 떠난다.
 */
#define KNL_LEAVE_NON_CANCELLABLE_SECTION( aEnv )                               \
    {                                                                           \
        STL_DASSERT( ((knlEnv*)((void*)aEnv))->mEnterNonCancelSecCount > 0 );   \
        ((knlEnv*)((void*)aEnv))->mEnterNonCancelSecCount--;                    \
    }

/**
 * @brief Non-Cancellation Section 계수를 반환한다.
 */
#define KNL_GET_ENTER_NON_CANCELLABLE_SECTION_COUNT( aEnv ) \
    ( ((knlEnv*)((void*)aEnv))->mEnterNonCancelSecCount )

typedef dtlValueBlockList           knlValueBlockList;
typedef struct knlExprContextInfo   knlExprContextInfo;
typedef struct knlCastFuncInfo      knlCastFuncInfo;
typedef struct knlBlockRelationList knlBlockRelationList;

/**
 * @defgroup knlEnvState Environment State
 * @ingroup knlGeneral
 * @{
 */
#define KNL_ENV_STATE_UNUSED            (0x0001)    /**< Unsed 상태 */
#define KNL_ENV_STATE_USED              (0x0002)    /**< Used 상태 */
#define KNL_ENV_STATE_AGING             (0x0004)    /**< free 대기 상태 */
/** @} */

/**
 * @brief Environment의 상태를 설정한다.
 */
#define KNL_SET_ENV_STATE( aEnv, aState )   { KNL_ENV(aEnv)->mState = aState; }

/**
 * @brief Environment의 상태를 얻는다.
 */
#define KNL_GET_ENV_STATE( aEnv )           ( KNL_ENV(aEnv)->mState )


/**
 * @brief Environment의 FATAL Handling 상태임을 설정한다. 
 */
#define KNL_SET_FATAL_HANDLING( aEnv, aLayer )  { KNL_ENV(aEnv)->mOnFatalHandling[aLayer] = STL_TRUE; }

/**
 * @brief Environment의 FATAL 진행 상태를 얻는다.
 */
#define KNL_GET_FATAL_HANDLING( aEnv, aLayer )  ( KNL_ENV(aEnv)->mOnFatalHandling[aLayer] )

/**
 * @addtogroup knlFileMgr
 * @{
 */

#define KNL_FILE_INVALID_TIMESTAMP       STL_UINT64_MAX

/**
 * @brief database가 사용하는 file의 종류
 */
typedef enum
{
    KNL_DATABASE_FILE_TYPE_CONTROL = 0,   /**< control file */
    KNL_DATABASE_FILE_TYPE_CONFIG,        /**< db config file */
    KNL_DATABASE_FILE_TYPE_DATAFILE,      /**< data file */
    KNL_DATABASE_FILE_TYPE_REDO_LOG,      /**< redo log file */
    KNL_DATABASE_FILE_TYPE_TRACE_LOG,     /**< trace log file */
    KNL_DATABASE_FILE_TYPE_TEMP,          /**< temporary file */
    KNL_DATABASE_FILE_TYPE_MAX
} knlDbFileType;

typedef struct knlFileItem
{
    stlRingEntry        mList;
    knlDbFileType       mFileType;
    stlChar           * mFileName; /* NULL이면 일반파일, 아니면 데이터파일 */
} knlFileItem;

typedef struct knlFileMgr
{
    knlLatch      mFileLatch;
    knlDynamicMem mMemMgr;
    stlRingHead   mFileList;
} knlFileMgr;

/** @} */

/**
 * @brief Kernel Environment 정의
 */
typedef struct knlEnv
{
    stlErrorStack        mErrorStack;
    knlLatchWaiter       mWaitItem;
    stlInt16             mLatchCount;
    knlLogicalAddr       mLatchStack[KNL_ENV_MAX_LATCH_DEPTH + 1];
    stlBool              mOnFatalHandling[STL_LAYER_MAX];  
    stlInt16             mState;
    stlBool              mCancelThread;        /**< Thread cancellation 여부 */
    stlUInt16            mEnterCriSecCount;    /** 임계구간의 입출을 위한 계수 */
    stlUInt16            mEnterNonCancelSecCount;    /** Non-Cancellable section의 입출을 위한 계수 */

    knlDynamicMem        mPrivateStaticArea;   /**< private static area를 위한 allocator */

    dtlFuncVector        mDTVector;            /**< DT attr 획득을 위한 Function Vector */
    
    void               * mSessionEnv;          /**< Session Environment */
    void               * mExecuteEvent;        /**< 실행 중인 Event data */
    stlTime              mCreationTime;        /**< Env가 사용되기 시작한 시간 */
    knlEvent             mEvent;               /**< Event 정보 */
    stlThreadHandle      mThread;              /**< 자신의 Thread Handle */
    stlInt32             mProcessId;           /**< Process Table에서 자신의 ID */
    stlInt32             mRequestGroupId;      /**< Shared Request Group Identifier */
    stlInt64             mProcessSeq;          /**< Process Sequence */
    stlInt16             mTopLayer;            /**< Env를 소유한 Top Layer Identifier */
    stlBool              mIsInShm;
    stlChar              mPadding[1];
    stlInt16             mNumaNodeId;
} knlEnv;

extern const stlChar *const gPhaseString[];

/**
 * @brief Envrionment가 소속되어 있는 Request Group의 Identifier를 얻는다.
 * @param[in] aEnv Environment
 */
#define KNL_REQUEST_GROUP_ID( aEnv )  ( ((knlEnv*)(aEnv))->mRequestGroupId )

/**
 * @brief 입력받은 Envrionment가 Shared Memory상에 위치하는지 여부를 반환한다.
 * @param[in] aEnv Environment
 */
#define KNL_IS_SHARED_ENV( aEnv )  ( ((knlEnv*)(aEnv))->mIsInShm )

/**
 * @defgroup knlSystemAvailableResource System Available Resource
 * @ingroup knlGeneral
 * @remarks 현재 시스템내에서 이용 가능한 리소스를 나타낸다.
 *     <BR> (1) 정상적인 서비스 상황
 *     <BR>     - KNL_SAR_WARMUP | KNL_SAR_TRANSACTION | KNL_SAR_SESSION
 *     <BR>     - New Session과 Database/Warmup이 허용된 상태
 *     <BR> (2) shutdown { normal | immediate } 시
 *     <BR>     - KNL_SAR_TRANSACTION
 *     <BR>     - Database의 접근만 허용함
 *     <BR> (3) system fatal 또는 shutdown abort 시
 *     <BR>     - KNL_SAR_NONE
 *     <BR>     - Warmup/New Session의 접근과 Database의 읽기/쓰기가 불가능한 상태
 *     <BR> (4) Database 생성시
 *     <BR>     - KNL_SAR_TRANSACTION | KNL_SAR_SESSION
 *     <BR>     - Warmup은 불가하고, Internal Session과 Database의 읽기/쓰기가 허용된 상태
 * @{
 */
#define KNL_SAR_MASK        (0x0000000F)  /**< System Available Resource Mask */
#define KNL_SAR_NONE        (0x00000000)  /**< 새로운 Session의 할당과 Database 접근이 불가능한 상태  */
#define KNL_SAR_TRANSACTION (0x00000001)  /**< New Transaction의 이용 가능 여부 */
#define KNL_SAR_SESSION     (0x00000002)  /**< New Session의 이용 가능 여부 */
#define KNL_SAR_WARMUP      (0x00000004)  /**< Warmup 의 이용 가능 여부 */
/** @} */

/**
 * @brief 임계구간에 진입한다.
 * @note 임계구간 진입이후 Fatal 발생한 경우는 System Fatal로 처리된다.
 */
#define KNL_ENTER_CRITICAL_SECTION( aEnv )          \
{                                                   \
    ((knlEnv*)((void*)aEnv))->mEnterCriSecCount++;  \
}

/**
 * @brief 임계구간을 떠난다.
 * @remarks 임계점을 떠난이후 Fatal 발생한 경우는 Session Fatal로 처리된다.
 */
#define KNL_LEAVE_CRITICAL_SECTION( aEnv )          \
{                                                   \
    ((knlEnv*)((void*)aEnv))->mEnterCriSecCount--;  \
}

/**
 * @brief 임계구간 진입 계수를 얻는다.
 */
#define KNL_GET_ENTER_CRITICAL_SECTION_COUNT( aEnv )  \
(                                                     \
    ((knlEnv*)((void*)aEnv))->mEnterCriSecCount       \
)

/**
 * @brief Environment Path Controler
 */
typedef struct knlEnvPathControl
{
    void * mEnv; /**< 읽어야 할 Environment의 주소 */
} knlEnvPathControl;

/**
 * @brief Kernel Environment를 얻는 매크로
 */
#define KNL_ENV( aEnv )         ((knlEnv*)(aEnv))

/**
 * @brief Kernel Environment에서 ErrorStack을 얻는 매크로
 */
#define KNL_ERROR_STACK( aEnv ) (&(KNL_ENV(aEnv))->mErrorStack)

/**
 * @brief Kernel Environment 에서 DT Attribute Fucnction Vector를 얻는 매크로
 */
#define KNL_DT_VECTOR( aEnv )   &((KNL_ENV(aEnv))->mDTVector)

/**
 * @brief Kernel Environment에서 Latch Wait Item을 얻는 매크로
 */
#define KNL_WAITITEM( aEnv ) &((KNL_ENV(aEnv))->mWaitItem)

/**
 * @brief Kernel Environment에서 LatchID Stack을 얻는 매크로
 */
#define KNL_ENV_LATCH_STACK( aEnv ) ((KNL_ENV(aEnv))->mLatchStack)

/**
 * @brief Kernel Environment에서 LatchID의 개수를 얻는 매크로
 */
#define KNL_ENV_LATCH_COUNT( aEnv ) ((KNL_ENV(aEnv))->mLatchCount)

/**
 * @brief Kernel Environment의 result stack의 최대 entry 개수의 기본값
 */
#define KNL_RESULT_STACK_DEFAULT_MAX_ENTRY_COUNT ( STL_UINT16_MAX - 1 )

/**
 * @brief Kernel Environment의 result stack 공간의 크기 (1 MB)
 */
#define KNL_RESULT_STACK_DEFAULT_SIZE                                   \
    ( KNL_RESULT_STACK_DEFAULT_MAX_ENTRY_COUNT * DTL_VALUE_ENTRY_SIZE )

/**
 * @brief Kernel Environment의 result stack을 위한 region memory의 INIT SIZE (1 MB)
 */
#define KNL_RESULT_STACK_MEM_INIT_SIZE    ( KNL_RESULT_STACK_DEFAULT_SIZE )

/**
 * @brief Latch Stack에 새 Latch를 넣는다
 */

#if 1
#define KNL_PUSH_LATCH( aEnv, aLatch )                                      \
    {                                                                       \
        STL_DASSERT((aEnv)->mLatchCount < KNL_ENV_MAX_LATCH_DEPTH);         \
        (aEnv)->mLatchStack[(aEnv)->mLatchCount] = (aLatch)->mLogicalAddr;  \
        (aEnv)->mLatchCount++;                                              \
        STL_DASSERT( (aEnv)->mLatchCount >= 0 );                            \
    }
#else
#define KNL_PUSH_LATCH( aEnv, aLatch )              \
    do                                              \
    {                                               \
        (aEnv)->mLatchCount++;                      \
        STL_DASSERT( (aEnv)->mLatchCount >= 0 );    \
    } while( 0 )
#endif

/**
 * @brief Latch Stack에서 주어진 Latch를 제거한다
 */
#if 1
#define KNL_POP_LATCH( aEnv, aLatch )                               \
    do                                                              \
    {                                                               \
        stlInt32 i;                                                 \
        stlInt32 j;                                                 \
        for( i = 0, j = 0; i < (aEnv)->mLatchCount; i++, j++ )      \
        {                                                           \
            if( (aEnv)->mLatchStack[i] == (aLatch)->mLogicalAddr )  \
            {                                                       \
                j++;                                                \
            }                                                       \
            (aEnv)->mLatchStack[i] = (aEnv)->mLatchStack[j];        \
        }                                                           \
        (aEnv)->mLatchCount--;                                      \
        STL_DASSERT( i >= 0 );                                      \
        STL_DASSERT( (aEnv)->mLatchCount >= 0 );                    \
    } while( 0 )
#else
#define KNL_POP_LATCH( aEnv, aLatch )               \
    do                                              \
    {                                               \
        (aEnv)->mLatchCount--;                      \
        STL_DASSERT( (aEnv)->mLatchCount >= 0 );    \
    } while( 0 )
#endif

#define KNL_SESSION_FATAL_BEHAVIOR_KILL_THREAD   ( KNL_PROPERTY_SESSION_FATAL_BEHAVIOR_MIN )
#define KNL_SESSION_FATAL_BEHAVIOR_KILL_PROCESS  ( KNL_PROPERTY_SESSION_FATAL_BEHAVIOR_MAX )

/**
 * @brief Startup시 parameter
 * Startup Phase     : MOUNT | OPEN
 * Data Access Mode  : Database 접근 모드(READ_ONLY | READ_WRITE)
 * Log Option        : RESETLOGS | NORESETLOGS
 */
typedef struct knlStartupInfo
{
    stlInt32      mStartupPhase;
    stlInt32      mDataAccessMode;
    stlInt8       mLogOption;
    stlChar       mPadding[7];
} knlStartupInfo;

#define KNL_INIT_STARTUP_ARG( aArg )                                    \
    {                                                                   \
        stlMemset( (void *)(aArg), 0x00, STL_SIZEOF(knlStartupInfo) );  \
        (aArg)->mStartupPhase = KNL_STARTUP_PHASE_NONE;                 \
        (aArg)->mDataAccessMode = SML_DATA_ACCESS_MODE_NONE;            \
        (aArg)->mLogOption = SML_STARTUP_LOG_OPTION_NORESETLOGS;        \
    }

typedef struct knlShutdownInfo
{
    stlInt32      mShutdownMode;  /**< Shutdown mode ( abort, normal, ... ) */
} knlShutdownInfo;

/**
 * @brief initialize shutdown arguments
 */
#define KNL_INIT_SHUTDOWN_INFO( aInfo )                     \
    {                                                       \
        (aInfo)->mShutdownMode = KNL_SHUTDOWN_MODE_NONE;    \
    }

/**
 * @brief Boottup 함수 정의
 */
typedef stlStatus (* knlStartupFunc )( knlStartupInfo  * aStartupInfo,
                                       void            * aEnv );

/**
 * @brief Warmup 함수 정의
 */
typedef stlStatus (* knlWarmupFunc )( void * aEnv );

/**
 * @brief Shutdown 함수 정의
 */
typedef stlStatus (* knlShutdownFunc )( knlShutdownInfo * aShutdownInfo,
                                        void            * aEnv );

/**
 * @brief Cooldown 함수 정의
 */
typedef stlStatus (* knlCooldownFunc )( void * aEnv );

/**
 * @brief Clean Heap 함수 정의
 */
typedef stlStatus (* knlCleanupHeapFunc )( void * aEnv );

/**
 * @defgroup knlStartupPhase Startup Phase
 * @ingroup knlGeneral
 * @remarks
 * <BR> 다단계 Startup 메카니즘은 서버 Startup 과정을 단계별로 구분함으로서 사용자가
 *      서버를 자유롭게 제어할수 있는 편의성을 제공한다.
 * <BR> 다단계 Startup 단계는 아래와 같이 총 3단계로 이루어진다.
 * <BR><BR><BR>
 *  <table>
 *     <tr>
 *       <th> 단계 </th>
 *       <th> 설명 </th>
 *       <th> 단계에서 사용 가능한 Action </th>
 *     </tr>
 *     <tr>
 *       <td> KNL_STARTUP_PHASE_NO_MOUNT </td>
 *       <td> 데이터베이스를 Mount하지 않은 상태 </td>
 *       <td> 새로운 데이터베이스 생성 </td>
 *     </tr>
 *     <tr>
 *       <td> KNL_STARTUP_PHASE_MOUNT </td>
 *       <td> 특정 데이터베이스에 Mount된 상태 </td>
 *       <td> 새로운 데이터베이스 Recovery </td>
 *     </tr>
 *     <tr>
 *       <td> KNL_STARTUP_PHASE_OPEN </td>
 *       <td> 서비스가 개방된 상태 </td>
 *       <td> 모든 연산 </td>
 *     </tr>
 *  </table>
 */

/**
 * @brief Startup Phase 정의
 */
typedef enum
{
    KNL_STARTUP_PHASE_NONE = 0,         /**< Startup 과정 초기화 */
    KNL_STARTUP_PHASE_NO_MOUNT,         /**< Startup 과정중 NO MOUNT 단계 */
    KNL_STARTUP_PHASE_MOUNT,            /**< Startup 과정중 MOUNT 단계 */
    KNL_STARTUP_PHASE_PREOPEN,          /**< Startup 과정중 OPEN 전단계 */
    KNL_STARTUP_PHASE_OPEN,             /**< Startup 과정중 OPEN 단계 */
    KNL_STARTUP_PHASE_MAX
} knlStartupPhase;

/**
 * @brief Shutdown Phase 정의
 */
typedef enum
{
    KNL_SHUTDOWN_PHASE_NONE = 0,
    KNL_SHUTDOWN_PHASE_INIT,             /**< Shutdown 과정중 초기 상태 */
    KNL_SHUTDOWN_PHASE_DISMOUNT,         /**< Shutdown 과정중 DISMOUNT 단계 */
    KNL_SHUTDOWN_PHASE_POSTCLOSE,        /**< Shutdown 과정중 POST CLOSE 단계 */
    KNL_SHUTDOWN_PHASE_CLOSE,            /**< Shutdown 과정중 CLOSE 단계 */
    KNL_SHUTDOWN_PHASE_MAX
} knlShutdownPhase;

/**
 * @brief Shutdown Mode 정의
 */
typedef enum
{
    KNL_SHUTDOWN_MODE_NONE = 0,          /**< 초기 상태 */
    KNL_SHUTDOWN_MODE_ABORT,             /**< {(1) 새로운 세션을 막음} 이후에 종료 */
    KNL_SHUTDOWN_MODE_OPERATIONAL,       /**< (1) + {(2) active operation의 종료를 기다림} 이후에 종료 */
    KNL_SHUTDOWN_MODE_IMMEDIATE,         /**< (1) + (2) + {(3) checkpoint 수행} 이후에 종료*/
    KNL_SHUTDOWN_MODE_TRANSACTIONAL,     /**< (1) + (2) + (3) + {(4) active transaction 종료} 이후에 종료 */
    KNL_SHUTDOWN_MODE_NORMAL,            /**< (1) + (2) + (3) + (4) + {(5) active session 종료} 이후에 종료 */
    KNL_SHUTDOWN_MODE_MAX
} knlShutdownMode;

/**
 * @brief Global Transaction Identifier
 */
typedef enum
{
    KNL_TRANS_SCOPE_NONE = 0,  /**< no transaction */
    KNL_TRANS_SCOPE_LOCAL,     /**< local transaction world */
    KNL_TRANS_SCOPE_GLOBAL     /**< global transaction world */
} knlTransScope;

/**
 * @brief Stat Category for Fixed Tables
 */
typedef enum knlStatCategory
{
    KNL_STAT_CATEGORY_NONE          = 0,     /**< none */
    KNL_STAT_CATEGORY_INIT_MEMORY   = 10,    /**< stat related init memory */
    KNL_STAT_CATEGORY_TOTAL_MEMORY,          /**< stat related total memory */
    KNL_STAT_CATEGORY_SQL           = 20     /**< stat related SQL */
} knlStatCategory;

/**
 * @brief Fixed Table Record for System Information
 */
typedef struct knlFxSystemInfo
{
    stlChar         * mName;              /**< name */
    stlInt64          mValue;             /**< stat value */
    knlStatCategory   mCategory;          /**< category */
    stlChar         * mComment;           /**< comment */
} knlFxSystemInfo;

/**
 * @brief Fixed Table Record for Session Information
 */
typedef struct knlFxSessionInfo
{
    stlChar         * mName;             /**< name */
    stlInt32          mSessId;           /**< session identifier */
    knlStatCategory   mCategory;         /**< category */
    stlInt64          mValue;            /**< stat value */
} knlFxSessionInfo;

/**
 * @brief Fixed Table Record for Statement Information
 */
typedef struct knlFxStatementInfo
{
    stlChar         * mName;           /**< name */
    stlInt32          mSessId;         /**< session identifier */
    stlUInt32         mStmtId;         /**< statement identifier */
    knlStatCategory   mCategory;       /**< category */
    stlInt64          mValue;          /**< stat value */
} knlFxStatementInfo;

/**
 * @brief Fixed Table Record for Process Information
 */

typedef struct knlFxProcInfo
{
    stlChar         * mName;          /**< name */
    stlInt32          mEnvId;         /**< environemnt identifier */
    knlStatCategory   mCategory;      /**< category */
    stlInt64          mValue;         /**< stat name */
} knlFxProcInfo;

/**
 * @brief 두 element를 비교하기 위해 제공되는 함수의 원형.
 * <PRE>
 * 1 : aValAPtr > aValBPtr
 * 0 : aValAPtr == aValBPtr
 * -1 : aValAPtr < aValBPtr
 * </PRE>
 * @param[in] aCmpInfo 비교시 필요한 정보들을 담은 임의의 포인터
 * @param[in] aValAPtr 비교할 Element A가 저장된 list에서의 포인터
 * @param[in] aValBPtr 비교할 Element B가 저장된 list에서의 포인터
 */
typedef stlInt32 (*knlCompareFunc)( void * aCmpInfo,
                                    void * aValAPtr,
                                    void * aValBPtr );

/** @} */

/** @} */

/**
 * @addtogroup knlSession
 * @{
 */

/**
 * @brief Kernel Session Type 정의
 */
typedef enum
{
    KNL_SESSION_TYPE_DEDICATE = 1,   /**< dedicated session */
    KNL_SESSION_TYPE_SHARED,         /**< shared session */
    KNL_SESSION_TYPE_MAX
} knlSessionType;

/**
 * @brief Session Env Type
 */
typedef enum
{
    KNL_SESS_ENV_HEAP = 1,
    KNL_SESS_ENV_SHARED
} knlSessEnvType;

/**
 * @brief End Session Option
 */
typedef enum
{
    KNL_END_SESSION_OPTION_NA = 0,
    KNL_END_SESSION_OPTION_POST_TRANSACTION,
    KNL_END_SESSION_OPTION_IMMEDIATE,
    KNL_END_SESSION_OPTION_KILL,
    KNL_END_SESSION_OPTION_MAX
} knlSessEndOption;

/**
 * @brief End Statement Option
 */
typedef enum
{
    KNL_END_STATEMENT_OPTION_NA = 0,
    KNL_END_STATEMENT_OPTION_CANCEL,
    KNL_END_STATEMENT_OPTION_MAX
} knlStmtEndOption;

typedef enum
{
    KNL_SESSION_STATUS_CONNECTED = 0,
    KNL_SESSION_STATUS_DISCONNECTED,
    KNL_SESSION_STATUS_SIGNALED,
    KNL_SESSION_STATUS_SNIPED,
    KNL_SESSION_STATUS_DEAD,   /**< 더이상 사용하지 못하는 세션 */
    KNL_SESSION_STATUS_MAX
} knlSessionStatus;

typedef enum
{
    KNL_CONNECTION_TYPE_NONE = 0,
    KNL_CONNECTION_TYPE_DA,
    KNL_CONNECTION_TYPE_TCP
} knlConnectionType;

#define KNL_SESSION_ACCESS_MASK    STL_UINT64_C(0xFF00000000000000)
#define KNL_SESSION_ACCESS_ENTER   STL_UINT64_C(0x0100000000000000)
#define KNL_SESSION_ACCESS_LEAVE   STL_UINT64_C(0x0200000000000000)

typedef enum
{
    KNL_TERMINATION_NONE = 0,
    KNL_TERMINATION_NORMAL,    /**< disconnect */
    KNL_TERMINATION_ABNORMAL,  /**< kill */
    KNL_TERMINATION_MAX
} knlTermination;

/**
 * @brief System time precision ( 10ms )
 */
#define KNL_SYSTEM_TIME_PRECISION  (10000000)

/**
 * @brief Check timeout
 * @param[in] aInterval Interval time (us)
 * @param[in] aTimeout Timeout (us)
 * @remarks timeout 계산할 때, system time이 오차범위가 KNL_SYSTEM_TIME_PRECISION 만큼 있기 때문에 <BR>
 * 실제로 timeout에 도달하지 않았는데도 timeout이 발생하는 문제가 있다. <BR>
 * 이를 위해 최소한 오차범위보단 커야 timeout이 발생할 수 있도록 <BR>
 * KNL_SYSTEM_TIME_PRECISION만큼 빼준다.
 */
#define KNL_CHECK_TIMEOUT( aInterval, aTimeout )    \
    ( ( (aInterval) < ((aTimeout) + KNL_SYSTEM_TIME_PRECISION/1000) ) ? STL_FALSE : STL_TRUE )

/**
 * @brief Session Event 정의
 */
typedef struct knlSessionEvent
{
    knlSessEndOption  mSessEndEvent;         /**< End Session Event */
    knlStmtEndOption  mStmtEndEvent;         /**< End Statement Event */
    stlTime           mQueryBeginTime;       /**< to detect Query Timeout */
    stlTime           mIdleBeginTime;        /**< to detect Idle Timeout */
    stlInt32          mStmtQueryTimeout;     /**< query timeout for a statement */
    stlBool           mDisableEvent;         /**< to disable Event */
} knlSessionEvent;

/**
 * @brief statement query timeout의 기본값: session query timeout을 따른다.
 */
#define KNL_DEFAULT_STATEMENT_QUERY_TIMEOUT   ( -1 )

/**
 * @brief Sql Fixing Array Size
 */
#define KNL_SQL_FIX_ARRAY_SIZE   ( 32 )

/**
 * @brief SQL Fixing Block
 */
typedef struct knlSqlFixBlock
{
    stlRingEntry   mFixBlockLink;
    stlBool        mIsDefaultBlock;
    void         * mSqlArray[KNL_SQL_FIX_ARRAY_SIZE];
} knlSqlFixBlock;

/**
 * @brief Kernel Session 정의
 */
typedef struct knlSessionEnv
{
    knlPropertyInfo   mPropertyInfo;               /**< Property 값을 저장 */
    void            * mEnv;                        /**< Environment */
    knlSessionEvent   mSessionEvent;               /**< Session Event */
    knlConnectionType mConnectionType;             /**< Session의 Connection Type */
    knlSessionType    mSessionType;                /**< Session Type */
    knlSessEnvType    mSessEnvType;                /**< Session Environment Type */
    stlLayerClass     mTopLayer;                   /**< Session을 소유한 Top Layer Identifier */
    stlUInt64         mAccessKey;                  /**< Access key to enter session */
    stlTime           mLogonTime;                  /**< Session이 사용되기 시작한 시간 */
    stlInt64          mOpenFileCount;              /**< Opened file count in session */
    knlSessionStatus  mStatus;                     /**< Session Status */
    knlTransScope     mTransScope;                 /**< Transaction Scope */
    stlBool           mUsed;                       /**< Used flag */
    stlBool           mDead;                       /**< Dead flag */
    stlBool           mNeedPendingSignal;          /**< need pending signal flag */
    stlBool           mEnableAging;                /**< whether to aging or not */
    knlTermination    mTermination;                /**< Termination flag */
    stlInt32          mPendingSignalNo;            /**< pending signal number */
    knlDynamicMem     mLongVaryingMem;             /**< dynamic allocator for long varying type */
    knlDynamicMem     mParameterMem;               /**< dynamic allocator for parameter */
    knlRegionMem      mPropertyMem;                /**< Region allocator for varchar type Property */
    stlRingHead       mSqlFixList;
    knlSqlFixBlock    mSqlFixBlock;                /**< SQL fixing block */
    stlBool           mInitialized[STL_LAYER_MAX]; /**< Initialized flag */
} knlSessionEnv;

/**
 * @brief Kernel Session Environment를 얻는다
 * @param[in] aEnv Environment 포인터
 * @note aEnv는 Session Environment가 아니고, Process Environment이다.
 */
#define KNL_SESS_ENV( aEnv )  ((knlSessionEnv*)(KNL_ENV(aEnv)->mSessionEnv))
/**
 * @brief 입력받은 Session Envrionment가 Shared Memory상에 위치하는지 여부를 반환한다.
 * @param[in] aSessEnv Session Environment
 */
#define KNL_IS_SHARED_SESS_ENV( aSessEnv )                                                       \
    ( (((knlSessionEnv*)(aSessEnv))->mSessEnvType == KNL_SESS_ENV_SHARED) ? STL_TRUE : STL_FALSE )

#define KNL_LINK_SESS_ENV( aEnv, aSessionEnv )                  \
    KNL_ENV(aEnv)->mSessionEnv            = (void*)aSessionEnv; \
    ((knlSessionEnv*)(aSessionEnv))->mEnv = (void*)aEnv

#define KNL_UNLINK_SESS_ENV( aEnv, aSessionEnv )                \
    KNL_ENV(aEnv)->mSessionEnv            = NULL;               \
    ((knlSessionEnv*)(aSessionEnv))->mEnv = NULL

#define KNL_GET_SESSION_STATUS( aSessEnv ) ( ((knlSessionEnv*)(aSessEnv))->mStatus )
#define KNL_SET_SESSION_STATUS( aSessEnv, aStatus )         \
    {                                                       \
        ((knlSessionEnv*)(aSessEnv))->mStatus = aStatus;    \
        stlMemBarrier();                                    \
    }

#define KNL_GET_SESSION_SEQ( aSessionEnv )                              \
    ( ((knlSessionEnv*)(aSessionEnv))->mAccessKey & ~KNL_SESSION_ACCESS_MASK )

#define KNL_INCREASE_SESSION_SEQ( aSessionEnv )                             \
    {                                                                       \
        stlAtomicInc64( &(((knlSessionEnv*)(aSessionEnv))->mAccessKey) );   \
    }

#define KNL_GET_SESSION_ACCESS_KEY_POINTER( aSessionEnv )   \
    ( &(((knlSessionEnv*)(aSessionEnv))->mAccessKey) )

#define KNL_MAKE_SESSION_ACCESS_KEY( aAccessMask, aSessionSeq ) \
    ( ((stlUInt64)(aAccessMask) | (stlUInt64)(aSessionSeq)) )

#define KNL_SET_SESSION_ACCESS_KEY( aSessionEnv, aAccessMask, aSessionSeq )         \
    {                                                                               \
        stlAtomicSet64( &(((knlSessionEnv*)(aSessionEnv))->mAccessKey),             \
                        ((stlUInt64)(aAccessMask) | (stlUInt64)(aSessionSeq)) );    \
        stlMemBarrier();                                                            \
    }

#define KNL_GET_SESSION_ACCESS_MASK( aSessionEnv )                              \
    ( ((knlSessionEnv*)(aSessionEnv))->mAccessKey & KNL_SESSION_ACCESS_MASK )

#define KNL_ENABLE_SESSION_AGING( aSessionEnv )                         \
    {                                                                   \
        ((knlSessionEnv*)(aSessionEnv))->mEnableAging = STL_TRUE;       \
    }

#define KNL_DISABLE_SESSION_AGING( aSessionEnv )                        \
    {                                                                   \
        ((knlSessionEnv*)(aSessionEnv))->mEnableAging = STL_FALSE;      \
    }

#define KNL_IS_ENABLE_SESSION_AGING( aSessionEnv )      \
    ( ((knlSessionEnv*)(aSessionEnv))->mEnableAging )

#define KNL_HAS_PENDING_SIGNAL( aSessionEnv )  ( ((knlSessionEnv*)(aSessionEnv))->mNeedPendingSignal );
#define KNL_PENDING_SIGNAL_NO( aSessionEnv )  ( ((knlSessionEnv*)(aSessionEnv))->mPendingSignalNo );

/**
 * Varchar Type Property 메모리 크기의 단위
 */
#define KNL_VARCHAR_PROPERTY_MEM_INIT_SIZE  ( 8 * 1024 )
#define KNL_VARCHAR_PROPERTY_MEM_NEXT_SIZE  ( 8 * 1024 )


/**
 * Long Varying Data를 메모리 크기의 단위
 */
#define KNL_LONG_VARYING_MEM_INIT_SIZE  ( 16 * 1024 )
#define KNL_LONG_VARYING_MEM_NEXT_SIZE  ( 16 * 1024 )

#define KNL_GET_LONG_VARYING_DYNAMIC_MEM( aSessionEnv )   ( &(((knlSessionEnv*)(aSessionEnv))->mLongVaryingMem) )

/**
 * Parameter를 메모리 크기의 단위
 */
#define KNL_PARAMETER_MEM_INIT_SIZE  ( 16 * 1024 )
#define KNL_PARAMETER_MEM_NEXT_SIZE  ( 16 * 1024 )

#define KNL_GET_PARAMETER_DYNAMIC_MEM( aSessionEnv )   ( &(((knlSessionEnv*)(aSessionEnv))->mParameterMem) )

/**
 * @brief Session Type을 얻는다
 * @param[in] aSessionEnv Session Environment 포인터
 * @see knlSessionType
 */
#define KNL_SESSION_TYPE( aSessionEnv )  (((knlSessionEnv*)(aSessionEnv))->mSessionType)

/**
 * @brief Connection Type을 얻는다
 * @param[in] aSessionEnv Session Environment 포인터
 * @see knlConnectionType
 */
#define KNL_CONNECTION_TYPE( aSessionEnv )  (((knlSessionEnv*)(aSessionEnv))->mConnectionType)

/**
 * @brief Session Environment Type을 얻는다
 * @param[in] aSessionEnv Session Environment 포인터
 * @see knlSessionType
 */
#define KNL_SESSION_ENV_TYPE( aSessionEnv )  (((knlSessionEnv*)(aSessionEnv))->mSessEnvType)

/**
 * @brief administrator session identifier
 */
#define KNL_ADMIN_SESSION_ID  (0)

/**
 * @brief Kernel Session Environment에서 Process Environment를 얻는다
 * @param[in] aSessionEnv Session Environment 포인터
 */
#define KNL_SESS_LINKED_ENV( aSessionEnv )  ( ((knlSessionEnv*)(aSessionEnv))->mEnv )

/** @} */

/**
 * @defgroup knlValueBlock Block Read
 * @ingroup knExternal
 * @{
 * @brief Block Read 를 위한 Value 관리 구조
 */

/**
 * @brief Block Read 가 아닌 경우의 Block 개수
 */
#define KNL_NO_BLOCK_READ_CNT (1)

/**
 * @brief Table ID 가 지정되지 않은 Table의 대표 ID
 */
#define KNL_ANONYMOUS_TABLE_ID (0)

/**
 * @brief Value Block List를 연결
 */
#define KNL_LINK_BLOCK_LIST          DTL_LINK_BLOCK_LIST

/**
 * @brief Value Block에서 Value 마다 메모리 공간을 확보하는 지 여부를 반환
 */
#define KNL_GET_BLOCK_IS_SEP_BUFF    DTL_GET_BLOCK_IS_SEP_BUFF

/**
 * @brief Value Block List 이 속한 Table의 Table ID
 */
#define KNL_GET_BLOCK_TABLE_ID       DTL_GET_BLOCK_TABLE_ID

 /**
  * @brief Value Block List 의 Table ID를 설정한다.
  */
#define KNL_SET_BLOCK_TABLE_ID       DTL_SET_BLOCK_TABLE_ID

/**
 * @brief Value Block List 의 Table 내 Column Order
 */
#define KNL_GET_BLOCK_COLUMN_ORDER   DTL_GET_BLOCK_COLUMN_ORDER

/**
 * @brief Value Block 의 Column Order 를 설정한다.
 */
#define KNL_SET_BLOCK_COLUMN_ORDER   DTL_SET_BLOCK_COLUMN_ORDER

/**
 * @brief Value Block List 의 할당된 공간의 개수를 획득 
 */
#define KNL_GET_BLOCK_ALLOC_CNT      DTL_GET_BLOCK_ALLOC_CNT

/**
 * @brief Value Block List 의 Data 가 존재하는 공간의 개수를 획득 
 */
#define KNL_GET_BLOCK_USED_CNT       DTL_GET_BLOCK_USED_CNT
    
/**
 * @brief 하나의 Value Block List 에 Data 가 존재하는 개수를 설정
 */
#define KNL_SET_ONE_BLOCK_USED_CNT   DTL_SET_ONE_BLOCK_USED_CNT

/**
 * @brief List 에 연결된 모든 Value Block List 에 Data 가 존재하는 개수를 설정
 * remark : List 에 연결된 모든 Value Block List 의 값을 변경해야 함.
 */
#define KNL_SET_ALL_BLOCK_USED_CNT   DTL_SET_ALL_BLOCK_USED_CNT

/**
 * @brief Value Block List 에서 Skip 할 공간의 개수를 획득 
 */
#define KNL_GET_BLOCK_SKIP_CNT       DTL_GET_BLOCK_SKIP_CNT

/**
 * @brief 하나의 Value Block List 에 Skip 할 개수를 설정
 */
#define KNL_SET_ONE_BLOCK_SKIP_CNT   DTL_SET_ONE_BLOCK_SKIP_CNT


/**
 * @brief List 에 연결된 모든 Value Block List 에 Skip 개수를 설정
 * remark : List 에 연결된 모든 Value Block List 의 값을 변경해야 함.
 */
#define KNL_SET_ALL_BLOCK_SKIP_CNT   DTL_SET_ALL_BLOCK_SKIP_CNT

/**
 * @brief List 에 연결된 모든 Value Block List 에 Skip과 데이터가 존재하는 개수를 설정
 * remark : List 에 연결된 모든 Value Block List 의 값을 변경해야 함.
 */
#define KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT DTL_SET_ALL_BLOCK_SKIP_AND_USED_CNT

/**
 * @brief Value Block List 의 DB StringLengthUnit
 */
#define KNL_GET_BLOCK_STRING_LENGTH_UNIT    DTL_GET_BLOCK_STRING_LENGTH_UNIT

/**
 * @brief Value Block List 의 DB IntervalIndicator
 */
#define KNL_GET_BLOCK_INTERVAL_INDICATOR    DTL_GET_BLOCK_INTERVAL_INDICATOR

/**
 * @brief Value Block List 의 Data Buffer 의 할당된 공간의 크기
 */
#define KNL_GET_BLOCK_DATA_BUFFER_SIZE      DTL_GET_BLOCK_DATA_BUFFER_SIZE

/**
 * @brief Value Block List 의 첫번째 Data Value 를 획득
 */
#define KNL_GET_BLOCK_FIRST_DATA_VALUE      DTL_GET_BLOCK_FIRST_DATA_VALUE

/**
 * @brief Value Block List 의 DB Data Type
 */
#define KNL_GET_BLOCK_DB_TYPE           DTL_GET_BLOCK_DB_TYPE

/**
 * @brief Value Block List를 할당한 Layer를 반환한다.
 */
#define KNL_GET_BLOCK_ALLOC_LAYER   DTL_GET_BLOCK_ALLOC_LAYER

/**
 * @brief Value Block List 의 DB Precision
 */
#define KNL_GET_BLOCK_ARG_NUM1          DTL_GET_BLOCK_ARG_NUM1

/**
 * @brief Value Block List 의 DB Scale
 */
#define KNL_GET_BLOCK_ARG_NUM2          DTL_GET_BLOCK_ARG_NUM2

/**
 * @brief Value Block List 의 N 번째 Data 의 실제 길이를 획득  
 * remark : aBlockIdx는 Block Idx (0-base)
 */
#define KNL_GET_BLOCK_DATA_LENGTH       DTL_GET_BLOCK_DATA_LENGTH

/**
 * @brief Value Block List 의 N 번째 Data 의 실제 길이를 설정 
 * remark : aBlockIdx는 Block Idx (0-base)
 *          aDataLength는 Data 의 실제 길이 
 */
#define KNL_SET_BLOCK_DATA_LENGTH       DTL_SET_BLOCK_DATA_LENGTH

/**
 * @brief Value Block List 의 N 번째 Data Value 를 획득
 * remark : 범위를 벗어난 Idx 인 경우, NULL
 */
#define KNL_GET_BLOCK_DATA_VALUE        DTL_GET_BLOCK_DATA_VALUE

/**
 * @brief mIsSepBuff가 True인 Value Block List 의 N 번째 Data Value 를 획득
 * return : Data Value Pointer   ( dtlDataValue * )
 */
#define KNL_GET_BLOCK_SEP_DATA_VALUE   DTL_GET_BLOCK_SEP_DATA_VALUE

/**
 * @brief Value Block의 N 번째 Data Value 를 획득
 * remark : 범위를 벗어난 Idx 인 경우, NULL
 *   Kernel Layer 에서만 사용함.
 */
#define KNL_GET_VALUE_BLOCK_DATA_VALUE  DTL_GET_VALUE_BLOCK_DATA_VALUE

/**
 * @brief Value Block을 할당한 Layer를 반환한다.
 */
#define KNL_GET_VALUE_BLOCK_ALLOC_LAYER  DTL_GET_VALUE_BLOCK_ALLOC_LAYER

/**
 * @brief Value Block List의 N 번째 Data 의 Buffer Pointer
 * return : Data Value 의 mValue Buffer Pointer   ( void * )
 */
#define KNL_GET_BLOCK_DATA_PTR          DTL_GET_BLOCK_DATA_PTR

/**
 * @brief Value Block List의 N 번째 Data Buffer 공간을 변경한다.
 * remark : aBlockIdx는 Block Idx (0-base)
 */
#define KNL_SET_BLOCK_DATA_PTR          DTL_SET_BLOCK_DATA_PTR

/**
 * @brief (SCAN 시) 현재 읽고 있는 row내 해당 column value 정보
 */
#define KNL_GET_BLOCK_COLUMN_IN_ROW     DTL_GET_BLOCK_COLUMN_IN_ROW

#if defined( STL_DEBUG )
#define KNL_VALIDATE_DATA_VALUE( aBlockList, aBlockIdx, aEnv )                                      \
    {                                                                                               \
        if( (aBlockList)->mValueBlock->mIsSepBuff == STL_TRUE )                                     \
        {                                                                                           \
            if( ((aBlockList)->mValueBlock->mDataValue[(aBlockIdx)].mType != DTL_TYPE_LONGVARCHAR) && \
                ((aBlockList)->mValueBlock->mDataValue[(aBlockIdx)].mType != DTL_TYPE_LONGVARBINARY) ) \
            {                                                                                       \
                STL_ASSERT( (aBlockList)->mValueBlock->mDataValue[(aBlockIdx)].mLength <=           \
                            (aBlockList)->mValueBlock->mAllocBufferSize );                          \
                STL_ASSERT( (aBlockList)->mValueBlock->mDataValue[(aBlockIdx)].mLength <=           \
                            (aBlockList)->mValueBlock->mDataValue[(aBlockIdx)].mBufferSize );       \
            }                                                                                       \
        }                                                                                           \
        else                                                                                        \
        {                                                                                           \
            if( ((aBlockList)->mValueBlock->mDataValue[(aBlockIdx)].mType != DTL_TYPE_LONGVARCHAR) && \
                ((aBlockList)->mValueBlock->mDataValue[(aBlockIdx)].mType != DTL_TYPE_LONGVARBINARY) ) \
            {                                                                                       \
                STL_ASSERT( (aBlockList)->mValueBlock->mDataValue[0].mLength <=                     \
                            (aBlockList)->mValueBlock->mAllocBufferSize );                          \
                STL_ASSERT( (aBlockList)->mValueBlock->mDataValue[0].mLength <=                     \
                            (aBlockList)->mValueBlock->mDataValue[0].mBufferSize );                 \
            }                                                                                       \
        }                                                                                           \
    }
#else
#define KNL_VALIDATE_DATA_VALUE( aBlockList, aBlockIdx, aEnv )
#endif


/**
 * @brief Value Block 의 Header 정보로
 * Table Read, Filter, Index Key 등의 처리를 위해 공유 대상이 되는 자료구조임. 
 */
typedef dtlValueBlock knlValueBlock;


/**
 * @brief SM에서 SCAN하고 있는 현재 row에 대한 column value 정보 (align이 고려되지 않은 value)
 * @remarks SM에서 SCAN시 사용하기 위한 구조체
 */
typedef dtlColumnInReadRow knlColumnInReadRow;


/** @} knlValueBlock */

/**
 * @defgroup knlBindContext Bind Context
 * @ingroup knExternal
 * @{
 * @brief Data Binding을 위한 구조
 */

/**
 * @brief Bind Type
 */
typedef enum
{
    KNL_BIND_TYPE_IN,         /**< IN bind type */
    KNL_BIND_TYPE_OUT,        /**< OUT bind type */
    KNL_BIND_TYPE_INOUT,      /**< IN/OUT bind type */
    KNL_BIND_TYPE_INVALID     /**< Invalid bind type */
} knlBindType;

/**
 * @brief Bind Parameter Structure
 */
typedef struct knlBindParam
{
    knlBindType         mBindType;              /**< Bind Type */
    knlValueBlockList * mINValueBlock;          /**< Bind된 Parameter의 SQL 타입의 Input Value Block */
    knlValueBlockList * mOUTValueBlock;         /**< Bind된 Parameter의 SQL 타입의 Output Value Block */
    knlValueBlockList   mExecuteValueBlockList; /**< 수행될 Parameter의 SQL 타입의 Value Block */
    knlValueBlock       mExecuteValueBlock;
} knlBindParam;
    
typedef struct knlBindContext
{
    stlBool         mHasServerParameter;   /**< 바인딩된 파라미터중 서버 생성 존재 여부 */
    stlInt64        mInitSize;             /**< Execute를 위한 Array 초기 Size */
    stlInt64        mExecuteSize;          /**< Execute를 위한 Array Size */
    stlInt64        mExecutePos;           /**< 현재 Execute하고 있는 파라미터의 value 위치 */
    stlInt32        mSize;                 /**< Bind Context에서 관리할수 있는 Parameter들의 최대 개수 */
    stlInt32        mMapSize;              /**< Parameter 포인터 맵의 크기 */
    knlBindParam ** mMap;                  /**< Parameter 포인터 맵 */
} knlBindContext;

/**
 * @brief 바인딩된 파라미터중 서버 생성 존재 여부
 */
#define KNL_BIND_CONTEXT_HAS_SERVER_PARAMETER( aBindContext )  ( (aBindContext)->mHasServerParameter )

/** @} */


/**
 * @addtogroup knlFixedTable
 * @{
 */

/**
 * @brief Fixed Table 용 주석을 위한 최대 길이
 */
#define KNL_MAX_FIXEDTABLE_COMMENTS_LENGTH    (1024)

/**
 * @brief Fixed Table용 Data Type
 */
typedef enum knlFxTableDataType
{
    KNL_FXTABLE_DATATYPE_VARCHAR = 0,   /**<  SQL Type: CHARACTER VARYING(n),
                                         * <BR> C Type: stlChar[n]    */
    KNL_FXTABLE_DATATYPE_VARCHAR_POINTER,
                                    /**<  SQL Type: CHARACTER VARYING(n),
                                     * <BR> C Type: stlChar*
                                     * <BR>
                                     * <BR> CHAR, VARCHAR와 VARCHAR POINTER의 차이점
                                     * <PRE>
                                     * typedef struct myType
                                     * {
                                     *        stlInt32  mInteger;
                                     *        stlChar   mChar[LENGTH];
                                     *        stlChar   mVarchar[LENGTH];
                                     *        stlChar * mVarcharPtr;
                                     * } myType;
                                     * </PRE>
                                     * 문자열 관련 Data Type은 Source String이
                                     * Null Termination되어 있음을 가정하고 있다.
                                     * - CHAR와 VARCHAR
                                     *  - CHAR    : Value 생성시 Space Padding 함.  	
                                     *  - VARCHAR : Value 생성시 Space Padding 하지 않음
                                     * - CHAR와 VARCHAR
                                     *  - CHAR    : Value 가 모두 꽉 차 있음.
                                     *  - VARCHAR : Value 가 Null-Terminated 되어 있음
                                     * - VARCHAR와 VARCHAR_POINTER
                                     *  - VARCHAR         : Data가 자료구조 내부에 있음.
                                     *  - VARCHAR_POINTER : Data가 자료구조 외부에 있음.
                                     */
    KNL_FXTABLE_DATATYPE_LONGVARCHAR,   /**<  SQL Type: LONG CHARACTER VARYING(n),
                                         * <BR> C Type: stlChar[n]    */
    KNL_FXTABLE_DATATYPE_LONGVARCHAR_POINTER,
                                    /**<  SQL Type: LONG CHARACTER VARYING(n),
                                     * <BR> C Type: stlChar*
                                     */
    KNL_FXTABLE_DATATYPE_SMALLINT,  /**<  SQL Type: SMALLINT,
                                     * <BR> C Type: stlInt16    */
    KNL_FXTABLE_DATATYPE_INTEGER,   /**<  SQL Type: INTEGER,
                                     * <BR> C Type: stlInt32    */
    KNL_FXTABLE_DATATYPE_BIGINT,    /**<  SQL Type: BIGINT,
                                     * <BR> C Type: stlInt64    */
    KNL_FXTABLE_DATATYPE_REAL,      /**<  SQL Type: REAL,
                                     * <BR> C Type: stlFloat32  */
    KNL_FXTABLE_DATATYPE_DOUBLE,    /**<  SQL Type: DOUBLE,
                                     * <BR> C Type: stlFloat64  */
    KNL_FXTABLE_DATATYPE_BOOL,      /**<  SQL Type: BOOLEAN,
                                     * <BR> C Type: stlBool     */
    KNL_FXTABLE_DATATYPE_TIMESTAMP, /**<  SQL Type: TIMESTAMP WITHOUT TIMEZONE,
                                     * <BR> C Type: stlTime */
    KNL_FXTABLE_DATATYPE_MAX
} knlFxTableDataType;

extern const stlChar * const gKnlFxTableDataTypeString[KNL_FXTABLE_DATATYPE_MAX];

/**
 * @brief Fixed Table의 Column 정의 자료구조
 * @remarks
 * <BR> Fixed Table을 구성하는 Column 의 자료구조를 정의한다.
 */
typedef struct knlFxColumnDesc
{
    stlChar            * mColumnName;     /**< 컬럼 이름
                                           * <BR> 조회 : X$COLUMNS.COLUMN_NAME */
    stlChar            * mColumnComment;  /**< 컬럼에 대한 주석
                                           * <BR> 조회 : X$COLUMNS.COMMENTS */
    knlFxTableDataType   mDataType;       /**< Column 의 Data Type
                                           * <BR> 조회 : X$COLUMNS.DTD_IDENTIFIER */
    stlInt32             mOffset;         /**< Structure에서의 Member 의 위치 */
    stlInt32             mLength;         /**< Structure Member의 크기 */
    stlBool              mNullable;       /**< column 의 nullable 여부 */
} knlFxColumnDesc;

/**
 * @brief Fixed Table의 사용 방식
 * @remarks
 * <BR> Fixed Table : 테이블 이름으로 식별이 가능한 자료구조 
 * <BR> Dump Table : 테이블 이름으로 식별이 되지 않아 별도의 Identifier가 필요한 자료구조
 */
typedef enum knlFxTableUsageType
{
    KNL_FXTABLE_USAGE_FIXED_TABLE = 0, /**< Basic Fixed Table */
    KNL_FXTABLE_USAGE_DUMP_TABLE,      /**< Dump Fixed Table */
    KNL_FXTABLE_USAGE_MAX
} knlFxTableUsageType;

extern const stlChar * const gKnlFxTableUsageTypeString[KNL_FXTABLE_USAGE_MAX];

/**
 * @brief IDENTIFIER Name 에 해당하는 Dump Option String 을 해석하기 위한 보조 함수
 * @param[in]   aTransID      Transaction ID
 * @param[in]   aStmt         Statement
 * @param[in]   aSchamaName   Schema Name
 *                     - <BR> Tablespace 인 경우 NULL
 *                     - <BR> if NULL, Schema 가 필요한 객체인 경우 "PUBLIC"
 * @param[in]   aObjectName   Object Name (Table, Index, Sequence, Tablespace)
 * @param[in]   aOptionName   Option Name
 * @param[out]  aObjectHandle Object Handle
 * @param[out]  aExist        존재 여부
 * @param[in]   aEnv          Environment
 * @remarks
 * Exmaple
 * - Identifier 가 1개인 객체
 *  - Tablespace (TBS1)
 *   - SchemaName = NULL
 *   - ObjectName = "TBS1"
 *   - OptionName = NULL
 * - Identifier 가 2개인 객체 
 *  - Table (S1.T1)
 *   - SchemaName = "S1"
 *   - ObjectName = "T1"
 *   - OptionName = NULL
 *  - Index (S1.IDX1)
 *   - SchemaName = "S1"
 *   - ObjectName = "IDX1"
 * - Identifier 가 3개인 객체
 *  - Partition (S1.T1.P1)
 *   - SchemaName = "S1"
 *   - ObjectName = "T1"
 *   - OptionName = "P1"
 */
typedef stlStatus (* knlDumpNameCallback) ( stlInt64   aTransID,
                                            stlInt64   aViewSCN,
                                            stlChar  * aSchemaName,
                                            stlChar  * aObjectName,
                                            stlChar  * aOptionName,
                                            void    ** aObjectHandle,
                                            stlBool  * aExist,
                                            knlEnv   * aEnv );

/**
 * @brief Dump Option String 해석을 위해 Name Identifier 를 얻기 위한 Callback 함수 모음
 * @remarks
 * Dictionary Table 을 관리하는 Execution Library Layer 에서 Callback을 등록한다.
 */
typedef struct knlDumpNameHelper
{
    knlDumpNameCallback    mGetBaseTableHandle; /**< Base Table 의 Handle 을 얻는 함수 */
    knlDumpNameCallback    mGetIndexHandle;     /**< Index Handle 을 얻는 함수 */
    knlDumpNameCallback    mGetSequenceHandle;  /**< Sequence Handle 을 얻는 함수 */
    knlDumpNameCallback    mGetSpaceHandle;     /**< Tablespace ID 를 얻는 함수 */
} knlDumpNameHelper;

/**
 * Dump Helper Global 변수
 */
extern knlDumpNameHelper  gKnlDumpNameHelper;

/**
 * @brief Dump Table일 경우 Dump Object의 Handle을 얻기 위한 함수 포인터
 * @param[in]     aTransID          Transaction ID (Dump Name Callback을 위해서만 사용됨)
 * @param[in]     aStmt             Statement (Dump Name Callback을 위해서만 사용됨)
 * @param[in]     aDumpOption       Dump Object 를 얻기 위한 Option String 
 * @param[out]    aDumpObjHandle    Dump Object 의 Handle
 * @param[in,out] aKnlEnv           Kernel Environment Handle
 * @remarks
 * <BR> Dump Object의 handle을 얻기 위한 함수를 정의한다.
 * <BR> D$MEM_PAGE( table_name ) 과 같은 경우
 * table_name 에 해당하는 handle을 얻을 방법을 정의한다.
 */
typedef stlStatus (* knlGetDumpObjectCallback) ( stlInt64  aTransID,
                                                 void    * aStmt,
                                                 stlChar * aDumpOption,
                                                 void   ** aDumpObjHandle,
                                                 knlEnv  * aKnlEnv );

/**
 * @brief 정의한 Fixed Table의  시작를 위한 함수 포인터
 * @param[in]     aStmt          Statement 포인터 (SM 상위 레이어에서만 사용 가능)
 * @param[in]     aDumpObjHandle 원하는 유형의 handle
 * <BR> 사용방식에 따라 유연하게 사용할 수 있으나, 일반적으로 다음과 같은 의미를 갖는다.
 * <BR> - Fixed Table : 자료구조에 대응하는 객체가 하나로 NULL 값
 * <BR> - Dump Table  : 자료구조에 대응하는 객체가 다양하여 이를 구분하기 위한 Dump Object
 * @param[out]    aPathCtrl      첫번째 레코드를 얻기 위한 정보를 저장할 메모리 공간
 * @param[in,out] aKnlEnv        Kernel Environment Handle
 * @remarks
 * 본 함수 포인터에는 다음과 같은 작업들이 포함된다.
 * - 첫번째 레코드를 구성하기 위한 위치 정보를 Path Finder에 기록
 * - 동시성 제어가 필요가 경우 latch의 획득
 */
typedef stlStatus (* knlOpenFxTableCallback) ( void   * aStmt,
                                               void   * aDumpObjHandle,
                                               void   * aPathCtrl,
                                               knlEnv * aKnlEnv );

/**
 * @brief 정의한 Fixed Table의 종료를 위한 함수 포인터
 * @param[in]     aDumpObjHandle 원하는 유형의 handle
 * <BR> 사용방식에 따라 유연하게 사용할 수 있으나, 일반적으로 다음과 같은 의미를 갖는다.
 * <BR> - Fixed Table : 자료구조에 대응하는 객체가 하나이므로, 일반적으로 NULL 값
 * <BR> - Dump Table  : 자료구조에 대응하는 객체가 다양하여 이를 구분하기 위한 Dump Object
 * @param[in ]    aPathCtrl      종료시점의 경로 정보
 * @param[in,out] aKnlEnv        Kernel Environment Handle
 * @remarks
 * 본 함수 포인터에는 다음과 같은 작업들이 포함된다.
 * - 동시성 제어를 위해 latch를 획득한 경우 latch의 해제
 */
typedef stlStatus (* knlCloseFxTableCallback) ( void   * aDumpObjHandle,
                                                void   * aPathCtrl,
                                                knlEnv * aKnlEnv );

/**
 * @brief 정의한 Fixed Table에서 하나의 레코드를 생성하기 위한 함수 포인터
 * @param[in]     aDumpObjHandle 원하는 유형의 handle
 * <BR> 사용방식에 따라 유연하게 사용할 수 있으나, 일반적으로 다음과 같은 의미를 갖는다.
 * <BR> - Fixed Table : 자료구조에 대응하는 객체가 하나이므로, 일반적으로 NULL 값
 * <BR> - Dump Table  : 자료구조에 대응하는 객체가 다양하여 이를 구분하기 위한 Dump Object
 * @param[in,out] aPathCtrl   레코드를 구성할 경로 정보
 * <BR> in : 현재 레코드를 생성할 자료구조 위치를 포함한 정보
 * <BR> out: 다음 레코드를 생성할 자료구조 위치를 포함한 정보
 * @param[out]    aValueList  생성할 레코드의 메모리 공간
 * @param[in]     aBlockIdx   Block Idx
 * @param[out]    aIsExist    생성한 레코드의 존재 여부 
 * @param[in,out] aKnlEnv     Kernel Environment Handle
 */
typedef stlStatus (* knlBuildFxRecordCallback) ( void              * aDumpObjHandle,
                                                 void              * aPathCtrl,
                                                 knlValueBlockList * aValueList,
                                                 stlInt32            aBlockIdx,
                                                 stlBool           * aIsExist,
                                                 knlEnv            * aKnlEnv );

/**
 * @brief Fixed Table 의 Table 정의 자료구조
 * @remarks
 * <BR> Materialized Fixed Table과 Pipelined Fixed Table이 공통으로 사용하는 Table 정의 자료구조
 */
typedef struct knlFxTableDesc
{
    knlFxTableUsageType          mUsageType;       /**< Fixed Table 의 사용 방식 */
    knlStartupPhase              mStartupPhase;    /**< 사용가능한 Startup 단계 */

    knlGetDumpObjectCallback     mGetDumpObjectFunc; /**< Dump Object의 Handle 획득 함수 */
    knlOpenFxTableCallback       mOpenFunc;        /**< Fixed Table 의 열기 함수 */
    knlCloseFxTableCallback      mCloseFunc;       /**< Fixed Table 의 닫기 함수 */
    knlBuildFxRecordCallback     mBuildRecordFunc; /**< 레코드를 생성하는 함수 */

    stlInt32                mPathCtrlSize; /**< 사용자가 정의한 경로 관리 정보의 크기 */
    stlChar               * mTableName;    /**< Fixed Table 이름으로 다음과 같은 Prefix를 갖는다.
                                            * <BR> Fixed Table : X$table_name
                                            * <BR> Dump Table  : D$table_name
                                            * <BR> 조회 : X$TABLES.TABLE_NAME */
    
    stlChar               * mTableComment; /**< Fixed Table 에 대한 주석 
                                            * <BR> 조회 : X$TABLES.COMMENTS */
    knlFxColumnDesc       * mColumnDesc;   /**< 컬럼 정의 정보 Array */
} knlFxTableDesc;

/**
 * @brief X$TABLES의 컬럼 Order
 * @remarks Dictionary Cache 를 위해 Execution Library layer 와 약속한 Enumeration
 */
typedef enum knlFxTableColumnOrder
{
    KNL_FXTABLE_ORDER_TABLE_ID = 0,
    KNL_FXTABLE_ORDER_TABLE_NAME,
    KNL_FXTABLE_ORDER_TABLE_COMMENTS,
    KNL_FXTABLE_ORDER_COLUMN_COUNT,
    KNL_FXTABLE_ORDER_USAGE_TYPE_VALUE,
    KNL_FXTABLE_ORDER_USAGE_TYPE_NAME,
    KNL_FXTABLE_ORDER_STARTUP_PHASE_VALUE,
    KNL_FXTABLE_ORDER_STARTUP_PHASE_NAME,
    
    KNL_FXTABLE_ORDER_MAX
} knlFxTableColumnOrder;

/**
 * @brief X$COLUMNS의 컬럼 Order
 * @remarks Dictionary Cache 를 위해 Execution Library layer 와 약속한 Enumeration
 */
typedef enum knlFxColumnColumnOrder
{
    KNL_FXCOLUMN_ORDER_TABLE_ID = 0,
    KNL_FXCOLUMN_ORDER_COLUMN_ID,
    KNL_FXCOLUMN_ORDER_TABLE_NAME,
    KNL_FXCOLUMN_ORDER_COLUMN_NAME,
    KNL_FXCOLUMN_ORDER_ORDINAL_POSITION,
    KNL_FXCOLUMN_ORDER_DATA_TYPE_ID,
    KNL_FXCOLUMN_ORDER_DATA_TYPE_NAME,
    KNL_FXCOLUMN_ORDER_COLUMN_LENGTH,
    KNL_FXCOLUMN_ORDER_IS_NULLABLE,
    KNL_FXCOLUMN_ORDER_COLUMN_COMMENTS,
    
    KNL_FXCOLUMN_COLUMN_ORDER_MAX
} knlFxColumnColumnOrder;


/**
 * @brief X${layer}_STMT_STAT을 위한 표준 컬럼 정의
 */
extern knlFxColumnDesc gKnlStatementInfoColumnDesc[];

/**
 * @brief X${layer}_SESS_{ENV/STAT}을 위한 표준 컬럼 정의
 */
extern knlFxColumnDesc gKnlSessionInfoColumnDesc[];

/**
 * @brief X${layer}_SYSTEM_INFO들을 위한 표준 컬럼 정의
 */
extern knlFxColumnDesc gKnlSystemInfoColumnDesc[];

/**
 * @brief X${layer}_PROC_{ENV/STAT}을 위한 표준 컬럼 정의
 */
extern knlFxColumnDesc gKnlProcInfoColumnDesc[];


/** @} */

/**
 * @addtogroup knlLatch
 * @{
 */

/**
 * @brief latch의 이름을 저장하는 버퍼의 최대 길이
 */
#define KNL_LATCH_MAX_DESC_BUF_SIZE  64
/**
 * @brief 특정 latch의 위치를 저장하는 버퍼의 최대 길이
 */
#define KNL_LATCH_MAX_POSITION_SIZE  64


/**
 * @brief 특정 종류의 래치를 Scan하기 전에 해야 할 작업들을 위한 함수 프로토타입
 */
typedef stlStatus (*knlOpenLatchClassFunc)( );

/**
 * @brief X$LATCH를 위해 해당 종류의 다음 순번의 래치와 래치 이름을 반환하는 function의 프로토타입
 * @param[in,out] aLatch 이전에 반환한 래치 포인터, 이번에 반환할 래치 포인터
 * (NULL이면 맨 처음 Latch를 찾아 반환해야 한다)
 * @param[in] aBuf latch의 Description을 출력할 메모리 공간 ( < KNL_MAX_LATCH_DESC_BUF_SIZE  )
 * @param[in,out] aPrevPosition 지난번에 반환한 latch의 위치 정보 입력, 이번의 위치정보 반환
 *  ( < KNL_MAX_LATCH_POSITION_SIZE )
 * @param[out] aEnv  kernel environment handle
 */
typedef stlStatus (*knlGetNextLatchAndDescFunc)( knlLatch ** aLatch, 
                                                 stlChar   * aBuf,
                                                 void      * aPrevPosition,
                                                 knlEnv    * aEnv );

/**
 * @brief 특정 종류의 래치를 Scan한 후에 해야 할 작업들을 위한 함수 프로토타입
 */
typedef stlStatus (*knlCloseLatchClassFunc)( );


/**
 * @brief 시스템 내의 latch의 종류의 최대 개수
 */
#define KNL_LATCH_MAX_CLASS_COUNT  32

/**
 * @brief 특정 종류의 latch를 scan하는 callback
 */
typedef struct knlLatchScanCallback
{
    knlOpenLatchClassFunc        mOpen;
    knlGetNextLatchAndDescFunc   mGetNext;
    knlCloseLatchClassFunc       mClose;
} knlLatchScanCallback;

/**
 * @brief X$LATCH 용 Path Controler
 */
typedef struct knlLatchPathControl
{
    stlInt64    mPosInfo[KNL_LATCH_MAX_POSITION_SIZE / STL_SIZEOF(stlInt64)];
    knlLatch  * mPrevLatch;
    stlUInt16   mCurLatchClass;
} knlLatchPathControl;

/**
 * @brief X$LATCH에서 하나의 latch에 의한 레코드
 */
typedef struct knlLatchFxRecord
{
    stlChar    mDesc[KNL_LATCH_MAX_DESC_BUF_SIZE];
    stlUInt16  mRefCount;
    stlChar    mCurMode[10];
    stlUInt32  mSpinLock;
    stlUInt16  mWaitCount;
    stlUInt64  mExclLockSeq;
    stlUInt64  mCasMissCnt;
} knlLatchFxRecord;

/** @} */

/**
 * @addtogroup knlQueue
 * @{
 */

typedef struct knlQueueInfo
{
    knlDynamicMem * mDynamicMem;
    stlChar       * mArray;
    stlInt64        mRearSn;
    stlInt64        mFrontSn;
    stlInt64        mArraySize;
    stlInt64        mItemSize;
} knlQueueInfo;

/** @} */

/**
 * @addtogroup knlHeapQueue
 * @{
 */

typedef stlStatus (*knlHeapQueueFetchNextFunc)( void      * aIterator,
                                                stlInt64    aIteratorIdx,
                                                void     ** aItem,
                                                knlEnv    * aEnv );

typedef struct knlHeapQueueContext
{
    knlCompareFunc              mCompare;
    knlHeapQueueFetchNextFunc   mFetchNext;
    void                      * mCompareInfo;
    void                      * mIterator;
} knlHeapQueueContext;

typedef struct knlHeapQueueEntry
{
    void     * mItem;
    stlInt64   mIteratorIdx;
} knlHeapQueueEntry;

typedef struct knlHeapQueueInfo
{
    knlHeapQueueContext  * mContext;
    knlHeapQueueEntry    * mRunArray;
    stlInt64               mRunCount;
    stlInt64               mArraySize;
    stlInt64               mHeight;
} knlHeapQueueInfo;

/** @} */

/**
 * @addtogroup knlQSort
 * @{
 */

/**
 * @brief Quick Sort를 할 list가 담고있는 element의 크기
 */
typedef enum
{
    KNL_QSORT_ELEM_2BYTE,  /**< Page Offset등의 stlUInt16 값들의 배열(stlUInt16*) */
    KNL_QSORT_ELEM_8BYTE   /**< pointer 값들의 배열(void**) */
} knlQSortElemType;

/**
 * @brief 주어진 list를 Quick Sort 할 방법에 대한 정의
 */
typedef struct knlQSortContext
{
    knlQSortElemType    mElemType;
    stlBool             mIsUnique;
    knlCompareFunc      mCompare;
    void              * mCmpInfo; /* compare시에 같이 입력되는 정보 */
} knlQSortContext;

/** @} */

/**
 * @addtogroup knlFilter
 * @{
 */

typedef enum
{
    KNL_BUILTIN_FUNC_INVALID = 0,

    /**
     * assign operations
     */

    KNL_BUILTIN_FUNC_ASSIGN,

    /**
     * unconditional jump operations
     */
    
    KNL_BUILTIN_FUNC_RETURN_EMPTY,
    KNL_BUILTIN_FUNC_RETURN_NULL,
    KNL_BUILTIN_FUNC_RETURN_PASS,

    /**
     * comparision operations
     */ 

    KNL_BUILTIN_FUNC_IS_EQUAL,                   /**< IS EQUAL */
    KNL_BUILTIN_FUNC_IS_NOT_EQUAL,               /**< IS NOT EQUAL */
    KNL_BUILTIN_FUNC_IS_LESS_THAN,               /**< IS LESS THAN */
    KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL,         /**< IS LESS_THAN EQUAL */
    KNL_BUILTIN_FUNC_IS_GREATER_THAN,            /**< IS GREATER THAN */
    KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL,      /**< IS GREATER THAN EQUAL */
    KNL_BUILTIN_FUNC_IS_NULL,                    /**< IS NULL */
    KNL_BUILTIN_FUNC_IS_NOT_NULL,                /**< IS NOT NULL */

    /*
     * logical operations
     */
    
    KNL_BUILTIN_FUNC_AND,            /**< AND */
    KNL_BUILTIN_FUNC_OR,             /**< OR */
    KNL_BUILTIN_FUNC_NOT,            /**< NOT */
    KNL_BUILTIN_FUNC_IS,             /**< IS */
    KNL_BUILTIN_FUNC_IS_NOT,         /**< IS NOT */
    KNL_BUILTIN_FUNC_IS_UNKNOWN,     /**< IS UNKNOWN */
    KNL_BUILTIN_FUNC_IS_NOT_UNKNOWN, /**< IS NOT UNKNOWN */

    /**
     * between
     */
    
    KNL_BUILTIN_FUNC_BETWEEN,             /**< BETWEEN */
    KNL_BUILTIN_FUNC_NOT_BETWEEN,         /**< NOT BETWEEN */
    KNL_BUILTIN_FUNC_BETWEEN_SYMMETRIC,   /**< BETWEEN SYMMETRIC */
    KNL_BUILTIN_FUNC_NOT_BETWEEN_SYMMETRIC,   /**< NOT BETWEEN SYMMETRIC */

    /**
     * like
     */
    KNL_BUILTIN_FUNC_LIKE,                /**< LIKE */
    KNL_BUILTIN_FUNC_NOT_LIKE,            /**< NOT LIKE */
    
    /**
     * like pattern
     */
    KNL_BUILTIN_FUNC_LIKE_PATTERN,        /**< PATTERN; LIKE SUB-FUNCTION */

    /**
     * cast 
     */
    
    KNL_BUILTIN_FUNC_CAST,                /**< CAST */

    /**
     * Mathematical ( with Arithmetic )
     */

    KNL_BUILTIN_FUNC_ADD,                 /**< ADDITION       */
    KNL_BUILTIN_FUNC_SUB,                 /**< SUBTRACTION    */
    KNL_BUILTIN_FUNC_MUL,                 /**< MULTIPLICATION */
    KNL_BUILTIN_FUNC_DIV,                 /**< DIVISION       */
    KNL_BUILTIN_FUNC_MOD,                 /**< MODULUS        */
    KNL_BUILTIN_FUNC_POSITIVE,            /**< POSITIVE       */
    KNL_BUILTIN_FUNC_NEGATIVE,            /**< NEGATIVE       */


    KNL_BUILTIN_FUNC_POWER,               /**< POWER */
    KNL_BUILTIN_FUNC_SQRT,                /**< SQRT  */
    KNL_BUILTIN_FUNC_CBRT,                /**< CBRT */
    KNL_BUILTIN_FUNC_FACTORIAL,           /**< FACTORIAL */
    KNL_BUILTIN_FUNC_ABS,                 /**< ABS */
    KNL_BUILTIN_FUNC_CEIL,                /**< CEIL */
    KNL_BUILTIN_FUNC_DEGREES,             /**< DEGREES */
    KNL_BUILTIN_FUNC_EXP,                 /**< EXP */
    KNL_BUILTIN_FUNC_FLOOR,               /**< FLOOR */
    KNL_BUILTIN_FUNC_LN,                  /**< LN */
    KNL_BUILTIN_FUNC_LOG10,               /**< LOG10 */
    KNL_BUILTIN_FUNC_LOG,                 /**< LOG */
    KNL_BUILTIN_FUNC_PI,                  /**< PI */
    KNL_BUILTIN_FUNC_RADIANS,             /**< RADIANS */
    KNL_BUILTIN_FUNC_RANDOM,              /**< RANDOM */
    KNL_BUILTIN_FUNC_ROUND,               /**< ROUND */
    KNL_BUILTIN_FUNC_SIGN,                /**< SIGN */
    KNL_BUILTIN_FUNC_TRUNC,               /**< TRUNC */
    KNL_BUILTIN_FUNC_WIDTHBUCKET,         /**< WIDTHBUCKET */
    KNL_BUILTIN_FUNC_ACOS,                /**< ACOS */
    KNL_BUILTIN_FUNC_ASIN,                /**< ASIN */
    KNL_BUILTIN_FUNC_ATAN,                /**< ATAN */
    KNL_BUILTIN_FUNC_ATAN2,               /**< ATAN2 */
    KNL_BUILTIN_FUNC_COS,                 /**< COS */
    KNL_BUILTIN_FUNC_COT,                 /**< COT */
    KNL_BUILTIN_FUNC_SIN,                 /**< SIN */
    KNL_BUILTIN_FUNC_TAN,                 /**< TAN */


    /**
     * Bitwise
     */
    KNL_BUILTIN_FUNC_BITWISE_AND,         /**< AND        */
    KNL_BUILTIN_FUNC_BITWISE_OR,          /**< OR         */
    KNL_BUILTIN_FUNC_BITWISE_XOR,         /**< XOR        */
    KNL_BUILTIN_FUNC_BITWISE_NOT,         /**< NOT        */
    KNL_BUILTIN_FUNC_BITWISE_SHIFTLEFT,   /**< SHIFTLEFT  */
    KNL_BUILTIN_FUNC_BITWISE_SHIFTRIGHT,  /**< SHIFTRIGHT */

    /**
     * String
     */

    KNL_BUILTIN_FUNC_CONCATENATE,         /**< CONCATENATE */
    KNL_BUILTIN_FUNC_BITLENGTH,           /**< BITLENGTH */
    KNL_BUILTIN_FUNC_CHARLENGTH,          /**< CHARLENGTH */
    KNL_BUILTIN_FUNC_LOWER,               /**< LOWER */
    KNL_BUILTIN_FUNC_OCTETLENGTH,         /**< OCTETLENGTH */
    KNL_BUILTIN_FUNC_OVERLAY,             /**< OVERLAY */
    KNL_BUILTIN_FUNC_POSITION,            /**< POSITION */
    KNL_BUILTIN_FUNC_INSTR,               /**< INSTR */    
    KNL_BUILTIN_FUNC_SUBSTRING,           /**< SUBSTRING */
    KNL_BUILTIN_FUNC_SUBSTRB,             /**< SUBSTRB */
    KNL_BUILTIN_FUNC_TRIM,                /**< TRIM */
    KNL_BUILTIN_FUNC_LTRIM,               /**< LTRIM */
    KNL_BUILTIN_FUNC_RTRIM,               /**< RTRIM */    
    KNL_BUILTIN_FUNC_UPPER,               /**< UPPER */
    KNL_BUILTIN_FUNC_INITCAP,             /**< INITCAP */
    KNL_BUILTIN_FUNC_LPAD,                /**< LPAD */
    KNL_BUILTIN_FUNC_RPAD,                /**< RPAD */
    KNL_BUILTIN_FUNC_REPEAT,              /**< REPEAT */
    KNL_BUILTIN_FUNC_REPLACE,             /**< REPLACE */
    KNL_BUILTIN_FUNC_SPLIT_PART,          /**< SPLIT_PART */
    KNL_BUILTIN_FUNC_TRANSLATE,           /**< TRANSLATE */
    KNL_BUILTIN_FUNC_CHR,                 /**< CHR */

    /**
     * Date & Time 
     */

    KNL_BUILTIN_FUNC_ADDDATE,        /**< ADDDATE */ 
    KNL_BUILTIN_FUNC_ADDTIME,        /**< ADDTIME */
    KNL_BUILTIN_FUNC_DATEADD,        /**< DATEADD */
    KNL_BUILTIN_FUNC_DATE_ADD,       /**< DATE_ADD */
    KNL_BUILTIN_FUNC_EXTRACT,        /**< EXTRACT */
    KNL_BUILTIN_FUNC_DATE_PART,      /**< DATE_PART */
    KNL_BUILTIN_FUNC_LAST_DAY,       /**< LAST_DAY */
    KNL_BUILTIN_FUNC_ADD_MONTHS,     /**< ADD_MONTHS */
    KNL_BUILTIN_FUNC_FROM_UNIXTIME,  /**< FROM_UNIXTIME */
    KNL_BUILTIN_FUNC_DATEDIFF,       /**< DATEDIFF */


    /**
     * RowId
     */
    
    KNL_BUILTIN_FUNC_ROWID_OBJECT_ID,      /**< ROWID_OBJECT_ID     */
    KNL_BUILTIN_FUNC_ROWID_PAGE_ID,        /**< ROWID_PAGE_ID       */
    KNL_BUILTIN_FUNC_ROWID_ROW_NUMBER,     /**< ROWID_ROW_NUMBER    */
    KNL_BUILTIN_FUNC_ROWID_TABLESPACE_ID,  /**< ROWID_TABLESPACE_ID */

    /**
     * Case
     */

    KNL_BUILTIN_FUNC_CASE,           /**< CASE      */
    KNL_BUILTIN_FUNC_NULLIF,         /**< NULLIF    */
    KNL_BUILTIN_FUNC_COALESCE,       /**< COALESCE */

    KNL_BUILTIN_FUNC_DECODE,         /**< DECODE */
    KNL_BUILTIN_FUNC_CASE2,          /**< CASE2  */
    

    /**
     * Nvl
     */

    KNL_BUILTIN_FUNC_NVL,            /**< NVL    */
    KNL_BUILTIN_FUNC_NVL2,           /**< NVL2    */

    /**
     * To_Char
     */
    
    KNL_BUILTIN_FUNC_TO_CHAR,        /**< TO_CHAR */

    KNL_BUILTIN_FUNC_TO_CHAR_FORMAT, /**< TO_CHAR_FORMAT */


    /**
     * To Date Time
     */

    KNL_BUILTIN_FUNC_TO_DATE,                      /**< TO_DATE */
    KNL_BUILTIN_FUNC_TO_TIME,                      /**< TO_TIME */
    KNL_BUILTIN_FUNC_TO_TIME_WITH_TIME_ZONE,       /**< TO_TIME_WITH_TIME_ZONE */        
    KNL_BUILTIN_FUNC_TO_TIMESTAMP,                 /**< TO_TIMESTAMP */
    KNL_BUILTIN_FUNC_TO_TIMESTAMP_WITH_TIME_ZONE,  /**< TO_TIMESTAMP_WITH_TIME_ZONE */        

    
    KNL_BUILTIN_FUNC_TO_DATETIME_FORMAT,           /**< TO_DATE_FORMAT */

    /**
     * TO_NUMBER
     */

    KNL_BUILTIN_FUNC_TO_NUMBER,                  /**< TO_NUMBER */

    KNL_BUILTIN_FUNC_TO_NUMBER_FORMAT,           /**< TO_NUMBER_FORMAT */

    /**
     * TO_NATIVE_REAL
     */

    KNL_BUILTIN_FUNC_TO_NATIVE_REAL,             /**< TO_NATIVE_REAL */

    /**
     * TO_NATIVE_DOUBLE
     */

    KNL_BUILTIN_FUNC_TO_NATIVE_DOUBLE,           /**< TO_NATIVE_DOUBLE */

    /**
     * general_comparision_function
     */

    KNL_BUILTIN_FUNC_GREATEST,                   /**< GREATEST */
    KNL_BUILTIN_FUNC_LEAST,                      /**< LEAST */

    /**
     * LIST OPERATION
     */
    KNL_BUILTIN_FUNC_IS_EQUAL_ROW,               /**< IS EQUAL ROW */
    KNL_BUILTIN_FUNC_IS_NOT_EQUAL_ROW,           /**< IS NOT EQUAL ROW */
    KNL_BUILTIN_FUNC_IS_LESS_THAN_ROW,           /**< IS LESS THAN ROW */
    KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL_ROW,     /**< IS LESS THAN EQUAL ROW */
    KNL_BUILTIN_FUNC_IS_GREATER_THAN_ROW,        /**< IS GREATER THAN ROW */
    KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL_ROW,  /**< IS GREATER THAN EQUAL ROW */

    /**
     * In
     */
    KNL_BUILTIN_FUNC_IN,                     /**< IN */
    KNL_BUILTIN_FUNC_NOT_IN,                 /**< NOT IN */
    
    /**
     * Exists
     */
    KNL_BUILTIN_FUNC_EXISTS,                 /**< EXISTS */
    KNL_BUILTIN_FUNC_NOT_EXISTS,             /**< NOT EXISTS */

    /**
     * LIST OPERATION ( ANY )
     */
    KNL_BUILTIN_FUNC_EQUAL_ANY,              /**< EQUAL ANY */
    KNL_BUILTIN_FUNC_NOT_EQUAL_ANY,          /**< NOT EQUAL ANY */
    KNL_BUILTIN_FUNC_LESS_THAN_ANY,          /**< LESS THANANY */ 
    KNL_BUILTIN_FUNC_LESS_THAN_EQUAL_ANY,    /**< LESS THAN EQUAL ANY */
    KNL_BUILTIN_FUNC_GREATER_THAN_ANY,       /**< GREATER THAN ANY */
    KNL_BUILTIN_FUNC_GREATER_THAN_EQUAL_ANY, /**< GREATER THAN EQUAL ANY */

    /**
     * LIST OPERATION ( ALL )
     */
    KNL_BUILTIN_FUNC_EQUAL_ALL,              /**< EQUAL ALL */
    KNL_BUILTIN_FUNC_NOT_EQUAL_ALL,          /**< NOT EQUAL ALL */
    KNL_BUILTIN_FUNC_LESS_THAN_ALL,          /**< LESS THANALL */ 
    KNL_BUILTIN_FUNC_LESS_THAN_EQUAL_ALL,    /**< LESS THAN EQUAL ALL */
    KNL_BUILTIN_FUNC_GREATER_THAN_ALL,       /**< GREATER THAN ALL */
    KNL_BUILTIN_FUNC_GREATER_THAN_EQUAL_ALL, /**< GREATER THAN EQUAL ALL */

    /**
     * Dump
     */
    KNL_BUILTIN_FUNC_DUMP,             /**< DUMP    */

    /*
     * System : for SESSION
     */

    KNL_BUILTIN_FUNC_CURRENT_CATALOG,            /**< CATALOG_NAME() */
    KNL_BUILTIN_FUNC_VERSION,                    /**< VERSION() */
    
    /*
     * Session : for SESSION
     */

    KNL_BUILTIN_FUNC_SESSION_ID,                 /**< BUILTIN_ID() */
    KNL_BUILTIN_FUNC_SESSION_SERIAL,             /**< BUILTIN_SERIAL() */
    KNL_BUILTIN_FUNC_USER_ID,                    /**< USER_ID() */
    KNL_BUILTIN_FUNC_CURRENT_USER,               /**< CURRENT_USER() USER() */
    KNL_BUILTIN_FUNC_SESSION_USER,               /**< SESSION_USER()  */
    KNL_BUILTIN_FUNC_LOGON_USER,                 /**< LOGON_USER()  */
    KNL_BUILTIN_FUNC_CURRENT_SCHEMA,             /**< CURRENT_SCHEMA() */

    /*
     * Date & Time : for SESSION
     */

    KNL_BUILTIN_FUNC_CLOCK_DATE,                 /**< CLOCK_DATE() */
    KNL_BUILTIN_FUNC_STATEMENT_DATE,             /**< STATEMENT_DATE(),CURRENT_DATE(),SYSDATE */
    KNL_BUILTIN_FUNC_TRANSACTION_DATE,           /**< TRANSACTION_DATE() */

    KNL_BUILTIN_FUNC_CLOCK_TIME,                 /**< CLOCK_TIME() */
    KNL_BUILTIN_FUNC_STATEMENT_TIME,             /**< STATEMENT_TIME(),CURRENT_TIME(),SYSTIME */
    KNL_BUILTIN_FUNC_TRANSACTION_TIME,           /**< TRANSACTION_TIME() */
    
    KNL_BUILTIN_FUNC_CLOCK_TIMESTAMP,            /**< CLOCK_TIMESTAMP() */
    KNL_BUILTIN_FUNC_STATEMENT_TIMESTAMP,        /**< STATEMENT_TIMESTAMP() */
    KNL_BUILTIN_FUNC_TRANSACTION_TIMESTAMP,      /**< TRANSACTION_TIMESTAMP() */

    KNL_BUILTIN_FUNC_CLOCK_LOCALTIME,            /**< CLOCK_LOCALTIME() */
    KNL_BUILTIN_FUNC_STATEMENT_LOCALTIME,        /**< STATEMENT_LOCALTIME(), LOCALTIME() */
    KNL_BUILTIN_FUNC_TRANSACTION_LOCALTIME,      /**< TRANSACTION_LOCALTIME() */

    KNL_BUILTIN_FUNC_CLOCK_LOCALTIMESTAMP,       /**< CLOCK_LOCALTIMESTAMP() */
    KNL_BUILTIN_FUNC_STATEMENT_LOCALTIMESTAMP,   /**< STATEMENT_LOCALTIMESTAMP() */
    KNL_BUILTIN_FUNC_TRANSACTION_LOCALTIMESTAMP, /**< TRANSACTION_LOCALTIMESTAMP() */

    KNL_BUILTIN_FUNC_SYSDATE,                    /**< SYSDATE */
    KNL_BUILTIN_FUNC_SYSTIME,                    /**< SYSTIME */
    KNL_BUILTIN_FUNC_SYSTIMESTAMP,               /**< SYSTIMESTAMP */     

    /*
     * SCN : for SESSION
     */
    
    KNL_BUILTIN_FUNC_STATEMENT_VIEW_SCN,         /**< STATEMENT_VIEW_SCN() */    

    KNL_BUILTIN_FUNC_MAX
} knlBuiltInFunc;


/**
 * @brief Pseudo Column ID
 * @note
 * 물리적 자원 정보가 Pseudo Column 으로 제공해서는 안된다. (PID, SHMID 등)
 * - Open Source 에서는 물리적 자원 정보를 함수로 제공하는 경우가 종종 있으나,
 * - 상용 DBMS 는 이러한 정보를 함수로 제공하는 경우가 드물다.
 * - 정확한 구분 기준은 정의할 수는 없으나, 이는 동일한 SQL 내에서 서로 다른 결과를 생성할 수 있어,
 *  - Procedure 등에서 사용시 논리적 오류를 유발할 수 있기 때문이다.
 *  - 물리적 자원 정보의 조회는 Fixed Table 로 제공하는 것이 바람직하다. 
 */
typedef enum
{
    KNL_PSEUDO_COL_INVALID = 0,

    /*
     * Sequence
     */
    
    KNL_PSEUDO_COL_CURRVAL,                  /**< CURRVAL(seq) */
    KNL_PSEUDO_COL_NEXTVAL,                  /**< NEXTVAL(seq) */

    /*
     * ROWID
     */

    KNL_PSEUDO_COL_ROWID,                   /**< ROWID */

    /*
     * ROWNUM
     */

    KNL_PSEUDO_COL_ROWNUM,                  /**< ROWNUM */

    
    KNL_PSEUDO_COL_MAX
} knlPseudoCol;

#define KNL_PHYSICAL_VALUE_CONSTANT         ( -1 )
#define KNL_PHYSICAL_LIST_VALUE_CONSTANT    ( -2 )

typedef struct knlPhysicalFilter  knlPhysicalFilter;

/**
 * @brief physical filter
 */
struct knlPhysicalFilter
{
    dtlPhysicalFunc      mFunc;
    stlInt32             mColIdx1;
    stlInt32             mColIdx2;     /**< constant value이면, KNL_PHYSICAL_VALUE_CONSTANT로 설정 */

    knlColumnInReadRow * mColVal1;     /**< sColIdx1이 참조하는 Align되지 않은 Column Value In Row,
                                        *   항상 NULL이 아닌 값이 존재 */
    knlColumnInReadRow * mColVal2;     /**< sColIdx2이 참조하는 Align되지 않은 Column Value In Row,
                                        *   항상 NULL이 아닌 값이 존재 */
    dtlDataValue       * mConstVal;
    
    knlPhysicalFilter  * mNext;
};

extern knlColumnInReadRow  knlEmtpyColumnValue[1];

typedef knlPhysicalFilter  * knlPhysicalFilterPtr;

/**
 * @brief compare list for min/max range 
 */
typedef struct knlCompareList  knlCompareList;

struct knlCompareList
{
    void            * mRangeVal;        /**< Range Value's Value Pointer */
    stlInt64          mRangeLen;        /**< Range Value's Length */

    stlInt32          mColOrder;        /**< Index Column Order */
    
    stlBool           mIsIncludeEqual;  /**< Is Include Equal condition */
    stlBool           mIsAsc;           /**< Is Ascend Order */
    stlBool           mIsNullFirst;     /**< Is Nulls First */
    stlBool           mIsDiffTypeCmp;   /**< Range와 Const Value가 같은 Data Type인지 여부 */
    stlBool           mIsLikeFunc;      /**< Like Function인지 여부 */
    
    stlBool                     mCompNullStop;  /* Key Range 가 NULL 상수와 사용될 경우 STOP */
    dtlDataValue              * mConstVal;  /**< Constant Value Pointer for Range Value */
    dtlPhysicalCompareFuncPtr   mCompFunc;
    knlCompareList  * mNext;
};

typedef struct knlRange  knlRange;

struct knlRange
{
    knlCompareList  * mMinRange;         /**< Min-Range Conditions (IN) */
    knlCompareList  * mMaxRange;         /**< Max-Range Conditions (IN) */
    knlCompareList  * mNext;
};

typedef struct knlDataValueList knlDataValueList;

/**
 * @brief Data Value List
 */
struct knlDataValueList
{
    dtlDataValue        * mConstVal;

    knlDataValueList    * mNext;
};


/**
 * @brief compare list for valid version check 
 */
typedef struct knlKeyCompareList  knlKeyCompareList;

struct knlKeyCompareList
{
    stlInt32             mIndexColCount;     /**< Key Column Count */
    
    /* table column order 순으로 정렬 됨 */
    stlInt32           * mTableColOrder;     /**< Read Table Column Order */
    stlInt32           * mIndexColOffset;    /**< Index Column Value Offset */

    /* 수행시 key column 정보 구성이 필요 */
    void              ** mIndexColVal;       /**< Index Column Value's Value Pointer */
    stlInt64           * mIndexColLen;       /**< Index Column Value's Length */

    /* 수행시 사용할 value buffer */
    knlValueBlockList  * mValueList;         /**< Value Block List for Table Column */

    dtlPhysicalFunc    * mCompFunc;          /**< Compare Function (IS EQUAL) */
};

/** @} knlFilter */



/**
 * @addtogroup knlBreakPoint
 * @{
 */

#define KNL_MAX_BP_NAME_LEN      128
#define KNL_MAX_ACTION_COUNT     128
#define KNL_MAX_BP_PROCESS_COUNT 128
#define KNL_MAX_PROC_NAME_LEN    128

/**
 * @brief break point 관련 작업에 참여하는 각 process들의 상태
 */
typedef enum
{
    KNL_PROC_STATUS_IDLE,
    KNL_PROC_STATUS_WAITING,
    KNL_PROC_STATUS_ACTIVE,
    KNL_PROC_STATUS_TERMINATED
} knlProcStatus;

/**
 * @brief 코드 곳곳에 설정될 breakpoint들의 id 리스트
 */
typedef enum
{
    KNL_BREAKPOINT_QLLFETCHSQL_BEFORE_FETCH,
    KNL_BREAKPOINT_SMNMPBDML_BEFORE_MOVE_TO_LEAF,
    KNL_BREAKPOINT_CHECKPOINT_BEFORE_CHKPT_LOGGING,
    KNL_BREAKPOINT_SMLINSERTRECORD_BEFORE_INSERT,
    KNL_BREAKPOINT_ELDLOCKANDVALIDATETABLE4DDL_AFTER_LOCK,
    KNL_BREAKPOINT_SMNMPBCREATE_AFTER_ALLOCHDR,
    KNL_BREAKPOINT_SMNMPBCREATEFORTRUNCATE_AFTER_ALLOCHDR,
    KNL_BREAKPOINT_SMDMPHCREATE_AFTER_ALLOCHDR,
    KNL_BREAKPOINT_SMDMPHCREATEFORTRUNCATE_AFTER_ALLOCHDR,
    KNL_BREAKPOINT_SMNMPBCHECKUNIQUENESS_BEFORE_FETCH_LEFT,
    KNL_BREAKPOINT_SMNMPBCHECKUNIQUENESS_BEFORE_FETCH_RIGHT1,
    KNL_BREAKPOINT_SMNMPBCHECKUNIQUENESS_BEFORE_FETCH_RIGHT2,
    KNL_BREAKPOINT_QLTEXECUTECREATESPACE_AFTER_CREATE_TBS,
    KNL_BREAKPOINT_QLTEXECUTECREATESPACE_AFTER_REFINE_CACHE,
    KNL_BREAKPOINT_SMFADDDATAFILE_AFTER_LOGGING,
    KNL_BREAKPOINT_SMFVALIDATEDATAFILES_AFTER_VALIDATION,
    KNL_BREAKPOINT_QLTEXECUTEALTERSPACEADDFILE_AFTER_SMLADDDATAFILE,
    KNL_BREAKPOINT_SMFDROPDATAFILE_AFTER_LOGGING,
    KNL_BREAKPOINT_QLTEXECUTEALTERSPACEDROPFILE_AFTER_SMLDROPDATAFILE,
    KNL_BREAKPOINT_QLTEXECUTEALTERSPACERENAME_AFTER_LOCK,
    KNL_BREAKPOINT_QLTEXECUTEALTERSPACERENAME_AFTER_REFINECACHE,
    KNL_BREAKPOINT_SMFONLINETBS_AFTER_PARALLELLOAD,
    KNL_BREAKPOINT_SMNMPBFETCH_PREV_BEFORE_MOVE_PREV_PAGE,
    KNL_BREAKPOINT_QLTEXECUTEALTERSPACEOFFLINE_AFTER_SMLOFFLINETABLESPACE,
    KNL_BREAKPOINT_QLTEXECUTEALTERSPACEONLINE_AFTER_SMLONLINETABLESPACE,
    KNL_BREAKPOINT_QLLCOMMIT_BEFORE_DICTIONARY_PENDING,
    KNL_BREAKPOINT_QLLROLLBACK_BEFORE_DICTIONARY_PENDING,
    KNL_BREAKPOINT_ELDCGETTABLEHANDLEBYNAME_BEFORE_SET_DICT_HANDLE,
    KNL_BREAKPOINT_STARTUPDATABASE_BEFORE_ADDENVEVENT,
    KNL_BREAKPOINT_STARTUPDATABASE_AFTER_ADDENVEVENT,
    KNL_BREAKPOINT_QLTEXECUTEALTERSPACEONLINE_AFTER_SMLFINALIZEONLINETABLESPACE,
    KNL_BREAKPOINT_SMLDELETERECORD_BEFORE_MODULEDELETE,
    KNL_BREAKPOINT_SMLUPDATERECORD_BEFORE_MODULEUPDATE,
    KNL_BREAKPOINT_SMRRECOVER_BEFORE_RECOVER,
    KNL_BREAKPOINT_SMXLRESTRUCTUREUNDORELATION_AFTER_EXPAND_UNDO_RELATION,
    KNL_BREAKPOINT_SMXLRESTRUCTUREUNDORELATION_AFTER_COMMIT,
    KNL_BREAKPOINT_SMDMPHUPDATE_AFTER_COMPACT,
    KNL_BREAKPOINT_SMFBACKUPTBSBEGIN_AFTER_SETTING,
    KNL_BREAKPOINT_BACKUPWAITING_AFTER_SET,
    KNL_BREAKPOINT_BACKUPWAITING_AFTER_RESET,
    KNL_BREAKPOINT_SMRREADONLINEFILEBLOCKS_BEFORE_FIND_LOG_GROUP,
    KNL_BREAKPOINT_SMRREADONLINEFILEBLOCKS_BEFORE_READ,
    KNL_BREAKPOINT_SMRMEDIARECOVER_AFTER_RECOVERY,
    KNL_BREAKPOINT_SMFMWRITELOGCREATE_BEFORE_UNDOLOGGING,
    KNL_BREAKPOINT_SMFMWRITELOGCREATE_AFTER_UNDOLOGGING,
    KNL_BREAKPOINT_SMFWRITELOGDATAFILE_BEFORE_UNDOLOGGING,
    KNL_BREAKPOINT_SMFWRITELOGDATAFILE_AFTER_UNDOLOGGING,
    KNL_BREAKPOINT_SMFWRITELOGTBS_BEFORE_UNDOLOGGING,
    KNL_BREAKPOINT_SMFWRITELOGTBS_AFTER_UNDOLOGGING,
    KNL_BREAKPOINT_SMFWRITELOGRENAMETBS_BEFORE_UNDOLOGGING,
    KNL_BREAKPOINT_SMFWRITELOGRENAMETBS_AFTER_UNDOLOGGING,
    KNL_BREAKPOINT_SHUTDOWNDATABASE_AFTER_PREVENTBACKUP,
    KNL_BREAKPOINT_SMFCREATETBS_BEFORE_ADD_TBSINFO_TO_CONTROLFILE,
    KNL_BREAKPOINT_SMDMPHCREATEUNDO_AFTER_SET_DROPPING,
    KNL_BREAKPOINT_QLDEXECUTERECOVERPAGE_BEFORE_ACQUIRE_LOCK,
    KNL_BREAKPOINT_QLDEXECUTERECOVERPAGE_AFTER_ACQUIRE_LOCK,
    KNL_BREAKPOINT_QLG_VALIDATE_SQL_BEFORE,
    KNL_BREAKPOINT_QLG_VALIDATE_SQL_COMPLETE,
    KNL_BREAKPOINT_QLG_CODE_OPTIMIZE_SQL_BEFORE,
    KNL_BREAKPOINT_QLG_CODE_OPTIMIZE_SQL_COMPLETE,
    KNL_BREAKPOINT_QLG_DATA_OPTIMIZE_SQL_BEFORE,
    KNL_BREAKPOINT_QLG_DATA_OPTIMIZE_SQL_COMPLETE,
    KNL_BREAKPOINT_QLG_EXECUTE_SQL_BEFORE,
    KNL_BREAKPOINT_QLG_EXECUTE_SQL_COMPLETE,
    KNL_BREAKPOINT_QLL_EXECUTE_SQL_BEFORE,
    KNL_BREAKPOINT_QLL_EXECUTE_SQL_COMPLETE,
    KNL_BREAKPOINT_QLNC_QUERY_OPTIMIZER,
    KNL_BREAKPOINT_QLL_OPTIMIZE_CODE_SQL_BEGIN,
    KNL_BREAKPOINT_QLL_OPTIMIZE_CODE_SQL_INTERNAL_BEGIN,
    KNL_BREAKPOINT_QLL_EXPLAIN_SQL_BEGIN,
    KNL_BREAKPOINT_QLG_RECOMPILE_SQL_PHASE_CODE_OPTIMIZE,
    KNL_BREAKPOINT_QLL_OPTIMIZE_DATA_SQL_COMPLETE,
    KNL_BREAKPOINT_QLCR_EXECUTE_OPEN_CURSOR_BEFORE_QUERY_EXECUTE,
    KNL_BREAKPOINT_SMFREWRITEDATAFILEHEADER_BEFORE_WRITE,
    KNL_BREAKPOINT_QLL_VALIDATE_SQL_GET_TABLE_HANDLE,
    KNL_BREAKPOINT_ELDC_PEND_COMMIT_DELETE_HASH_RELATED_COMPLETE,
    KNL_BREAKPOINT_SMFDROPTBS_BEFORE_ADD_COMMIT_PENDING,
    
    KNL_BREAKPOINT_MAX
} knlBreakPointId;

/**
 * @brief 각 breakpoint에서 수행할 행동 종류
 */
typedef enum
{
    KNL_BREAKPOINT_ACTION_WAKEUP,      /* 주어진 process가 진행되도록 하고 대기한다 */
    KNL_BREAKPOINT_ACTION_SLEEP,       /* 주어진 시간만큼 대기한다 */
    KNL_BREAKPOINT_ACTION_ABORT,       /* ABORT error를 발생시킨다 */
    KNL_BREAKPOINT_ACTION_KILL,        /* FATAL error를 발생시킨다 */
    KNL_BREAKPOINT_ACTION_MAX
} knlBreakPointAction;

/**
 * @brief 각 breakpoint에서 수행할 Post Action 종류
 */
typedef enum
{
    KNL_BREAKPOINT_POST_ACTION_NONE,          /* none */
    KNL_BREAKPOINT_POST_ACTION_ABORT,         /* ABORT error를 발생시킨다 */
    KNL_BREAKPOINT_POST_ACTION_SESSION_KILL,  /* SESSION FATAL error를 발생시킨다 */
    KNL_BREAKPOINT_POST_ACTION_SYSTEM_KILL,   /* SYSTEM FATAL error를 발생시킨다 */
    KNL_BREAKPOINT_POST_ACTION_MAX
} knlBreakPointPostAction;

typedef enum
{
    KNL_BREAKPOINT_SESSION_TYPE_NONE,
    KNL_BREAKPOINT_SESSION_TYPE_SYSTEM,
    KNL_BREAKPOINT_SESSION_TYPE_USER,
    KNL_BREAKPOINT_SESSION_TYPE_MAX
} knlBreakPointSessType;

/**
 * @brief code 곳곳에 breakpoint를 선언하는 문장
 */
#define KNL_BREAKPOINT( aBpActionId, aEnv )                                 \
    {                                                                       \
        if( knlGetActionCount() > 0 )                                       \
        {                                                                   \
            STL_TRY( knlDoBreakPoint( aBpActionId, aEnv ) == STL_SUCCESS ); \
        }                                                                   \
    }


/** @} */


/**
 * @defgroup knlAggregation Aggregation
 * @ingroup knExternal
 * @{
 */


/**
 * @brief Aggregation Function Types
 */
typedef enum
{
    /* Unary Aggregation */
    KNL_BUILTIN_AGGREGATION_UNARY_MIN = 0,

    KNL_BUILTIN_AGGREGATION_SUM,
    KNL_BUILTIN_AGGREGATION_COUNT,
    KNL_BUILTIN_AGGREGATION_COUNT_ASTERISK,
    KNL_BUILTIN_AGGREGATION_MIN,
    KNL_BUILTIN_AGGREGATION_MAX,

    KNL_BUILTIN_AGGREGATION_UNARY_MAX,

} knlBuiltInAggrFunc;


/**
 * Invalid Aggregation Function ID
 */
#define KNL_BUILTIN_AGGREGATION_INVALID  ( 0 )

/**
 * Unary Aggregation의 Argument Count
 */
#define KNL_UNARY_AGGREGATION_ARG_COUNT  ( 1 )


typedef stlStatus (*knlAddValueToNumFuncPtr) ( dtlDataValue  * aSrcValue,
                                               dtlDataValue  * aDestValue,
                                               knlEnv        * aEnv );

typedef stlStatus (*knlAddToNumFuncPtr) ( knlValueBlockList  * aSrcValue,
                                          knlValueBlockList  * aDestValue,
                                          stlInt32             aStartBlockIdx,
                                          stlInt32             aBlockCnt,
                                          stlInt32             aResultBlockIdx,
                                          knlEnv             * aEnv );

typedef stlStatus (*knlAddToNumWithConflictInfoFuncPtr) ( knlValueBlockList  * aSrcValue,
                                                          knlValueBlockList  * aDestValue,
                                                          knlValueBlockList  * aConflictInfo,
                                                          stlInt32             aBlockCnt,
                                                          knlEnv             * aEnv );

extern knlAddValueToNumFuncPtr gKnlAddValueToNumFuncPtrArr[ DTL_TYPE_MAX ];

extern knlAddToNumFuncPtr gKnlAddToNumFuncPtrArr[ DTL_TYPE_MAX ];


/** @} */

/**
 * @addtogroup knlLogicalAddr
 * @{
 */

#define KNL_SHM_TABLE_ELEMENT_COUNT (1 << KNL_SHM_INDEX_PRECISION)

/**
 * @brief 할당된 공유메모리 세그먼트 정보를 저장하는 구조체
 */
typedef struct knlShmTableElement
{
    stlChar       mName[STL_MAX_SHM_NAME+1]; /**< 공유 메모리 이름(Null Terminated String) */
    stlSize       mSize;   /**< 할당된 공유 메모리 세그먼트의 크기 */
    stlInt32      mKey;    /**< 공유 메모리 세그먼트의 키 */
#if (STL_SIZEOF_VOIDP == 8)
    stlInt32      mAlign;  /**< 8byte align을 위한 padding */
#endif 
    stlUInt64     mSeq;    /**< 현재 공유 메모리 세그먼트의 재사용 Sequence */
} knlShmTableElement;

/**
 * @brief 공유메모리 세그먼트를 관리한다.
 * @remark 공유 메모리에 존재한다.
 */
typedef struct knlShmManager
{
    /** @brief 동기화 객체 */
    knlLatch  mLatch;
    /** @brief 생성된 공유메모리 세그먼트 수 */
    stlInt32  mSegmentCount;
    /** @brief 유효성 검사를 위한 Magic Number */
    stlInt32  mMagicNumber;
    /** @brief 공유 메모리 세그먼트 생성을 위한 key seed */
    stlInt32  mKeySeed;
    /** @brief 8byte align을 위한 padding */
    stlInt32  mAlign;
    /** @brief 공유메모리 세그먼트들의 정보 */
    knlShmTableElement mSegment[KNL_SHM_TABLE_ELEMENT_COUNT]; 
} knlShmManager;
    
/**
 * @brief 공유 메모리 세그먼트와 프로세스에서의 주소 관리를 위한 구조체
 */
typedef struct knlShmAddrTable
{
    /** @brief local latch */
    knlLatch        mLatch;
    /** @brief static area 정보 */
    stlShm          mStaticArea;
    /** @brief 프로세스에서 유효한 공유메모리 관리자의 주소 */
    knlShmManager * mShmManager;
    /** @brief 공유메모리 세그먼트들의 물리적 주소 */
    void          * mAddr[KNL_SHM_TABLE_ELEMENT_COUNT]; 
    stlUInt64       mSeq[KNL_SHM_TABLE_ELEMENT_COUNT]; 
} knlShmAddrTable;

extern knlShmAddrTable gShmAddrTable;

/**
 * @brief 논리적 주소를 물리적 주소로 변환한다.
 * @param[in] aLogicalAddr 논리적 주소
 */

#define KNL_TO_PHYSICAL_ADDR( aLogicalAddr )                                                \
    (gShmAddrTable.mAddr[KNL_GET_SHM_INDEX(aLogicalAddr)] + KNL_GET_OFFSET(aLogicalAddr))

/** @} */

/**
 * @defgroup knlDataValue Data Value 
 * @ingroup knExternal
 * @{
 * @brief Block Read 를 위한 Data Value 관리 구조
 */


/** @} */


/**
 * @defgroup knlXaHash XA Context Hash
 * @ingroup knExternal
 * @{
 * @brief Hash for Global Transactions
 */

/**
 * @brief XA States
 */

typedef enum
{
    KNL_XA_STATE_NOTR = 0,             /**< no transaction */
    KNL_XA_STATE_ACTIVE,               /**< active */ 
    KNL_XA_STATE_IDLE,                 /**< idle */ 
    KNL_XA_STATE_PREPARED,             /**< prepared */ 
    KNL_XA_STATE_ROLLBACK_ONLY,        /**< rollback only */ 
    KNL_XA_STATE_HEURISTIC_COMPLETED   /**< heuristic completed */ 
} knlXaState;

typedef enum
{
    KNL_XA_HEURISTIC_DECISION_NONE = 0,      /**< waiting decision */
    KNL_XA_HEURISTIC_DECISION_ROLLED_BACK,   /**< heuristic rolled back */
    KNL_XA_HEURISTIC_DECISION_COMMITTED      /**< heuristic committed */
} knlHeuristicDecision;
    
typedef enum
{
    KNL_XA_ASSOCIATE_STATE_FALSE,            /**< not associated */
    KNL_XA_ASSOCIATE_STATE_TRUE,             /**< associated */
    KNL_XA_ASSOCIATE_STATE_SUSPEND,          /**< association suspended */
} knlAssociateState;

/**
 * @brief XA Context Structure
 */
typedef struct knlXaContext
{
    knlXaState            mState;              /**< XA state */
    knlAssociateState     mAssociateState;     /**< association state */
    knlHeuristicDecision  mHeuristicDecision;  /**< heuristic decision state */
    stlInt64              mTransId;            /**< local transaction identifier */
    stlXid                mXid;                /**< global transaction identifier */
    void                * mDeferContext;       /**< deferred constraint context */
    stlUInt32             mSessionId;          /**< session identifier */
    stlInt32              mHashValue;          /**< hash value for XID */
    knlLatch              mLatch;              /**< latch for concurrency contol */
    knlCondVar            mCondVar;            /**< conditional variable */
    stlRingEntry          mContextLink;        /**< link for hash bucket */
    stlRingEntry          mFreeLink;           /**< free context link */
} knlXaContext;


/** @} */


/**
 * @defgroup knlPlanCache Plan Cache
 * @ingroup knExternal
 * @{
 * @brief Plan Cache
 */

typedef void * knlSqlHandle;
typedef void * knlPlanHandle;

typedef struct knlPlanSqlInfo
{
    stlBool       mIsQuery;     /**< Query 인 경우 Cursor Information 도 동일해야 함 */
    stlInt32      mStmtType;
    stlInt32      mBindCount;
    stlInt32      mPrivCount;
    stlInt32      mPrivArraySize;
    void        * mPrivArray;
} knlPlanSqlInfo;

typedef struct knlPlanUserInfo
{
    stlInt64       mUserId;
} knlPlanUserInfo;

typedef struct knlPlanCursorInfo
{
    stlBool    mIsDbcCursor;   /**< ODBC/JDBC cursor 인지 SQL cursor(DECLARE CURSOR) 구문에 의한 설정인지 여부 */

    stlInt32   mStandardType;     /**< Cursor Standard Type */

    stlInt32   mSensitivity;      /**< Cursor Sensitivity */
    stlInt32   mScrollability;    /**< Cursor Scrollability */
    stlInt32   mHoldability;      /**< Cursor Holdability */
    stlInt32   mUpdatability;     /**< Cursor Updatability */
    stlInt32   mReturnability;    /**< Cursor Returnability */
    
    /**
     * @todo Cursor Type 을 Standard Layer 에 정의하자
     */
    
    // ellCursorStandardType   mStandardType;     /**< Cursor Standard Type */

    // ellCursorSensitivity    mSensitivity;      /**< Cursor Sensitivity */
    // ellCursorScrollability  mScrollability;    /**< Cursor Scrollability */
    // ellCursorHoldability    mHoldability;      /**< Cursor Holdability */
    // ellCursorUpdatability   mUpdatability;     /**< Cursor Updatability */
    // ellCursorReturnability  mReturnability;    /**< Cursor Returnability */
} knlPlanCursorInfo;

/**
 * @brief Plan 비교 함수
 */
typedef stlBool (*knlComparePlanFunc) ( knlPlanSqlInfo  * aSqlInfo,
                                        void            * aCodePlanA,
                                        void            * aCodePlanB );

typedef struct knlPlanIterator
{
    stlInt32       mPlanIdx;
    knlSqlHandle * mSqlHandle;
} knlPlanIterator;

typedef struct knlFlangeIterator
{
    stlInt32       mClockId;
    stlInt32       mFlangeId;
    void         * mFlangeBlock;
    knlSqlHandle   mSqlHandle;
} knlFlangeIterator;

/**
 * @brief DEVELOPMENT_OPTION의 키 값 정의
 */
#define KNL_DEVELOPMENT_OPTION_NONE                   ( 0 )         /**< development option이 꺼져 있음. 정상 상태 */

/** @} */

#endif /* _KNLDEF_H_ */

