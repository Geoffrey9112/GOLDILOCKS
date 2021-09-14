/*******************************************************************************
 * smlDef.h
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


#ifndef _SMLDEF_H_
#define _SMLDEF_H_ 1

/**
 * @file smlDef.h
 * @brief Storage Manager Definitions
 */

/**
 * @defgroup smExternal Storage Manager Layer Routines
 * @{
 */

/**
 * @addtogroup smlTrans
 * @{
 */

/**
 * @brief 트랜잭션 아이디
 */
typedef stlInt64 smlTransId;

/**
 * @brief 트랜잭션 독립성 레벨
 */
typedef enum
{
    SML_TIL_READ_UNCOMMITTED = 0, /**< Read Uncommitted Level */
    SML_TIL_READ_COMMITTED,       /**< Read Committed Level */
    SML_TIL_SERIALIZABLE          /**< Serializable Level */ 
} smlIsolationLevel;

/**
 * @brief 데이터베이스 접근 모드
 */
typedef enum
{
    SML_DATA_ACCESS_MODE_NONE = 0,    /**< None */
    SML_DATA_ACCESS_MODE_READ_ONLY,   /**< Read-Only Mode */
    SML_DATA_ACCESS_MODE_READ_WRITE   /**< Read Write Mode */
} smlDataAccessMode;

/**
 * @defgroup smlDataStoreMode Data Store Mode
 * @ingroup smlTrans
 * @{
 */
#define SML_DATA_STORE_MODE_CDS  1  /**< Concurrent Data Store : A(O),C(O),I(O),D(X) */
#define SML_DATA_STORE_MODE_TDS  2  /**< Transactional Data Store : A(O),C(O),I(O),D(O) */
/** @} */

/**
 * @defgroup smlPendActionFlags Pending Action Flags
 * @ingroup smlTrans
 * @{
 */
#define SML_PEND_ACTION_NONE      0x00000000   /**< INVALID */
#define SML_PEND_ACTION_COMMIT    0x00000001   /**< Pending Job을 Commit시 수행 */
#define SML_PEND_ACTION_ROLLBACK  0x00000002   /**< Pending Job을 Rollback시 수행 */
/** @} */

/**
 * @defgroup smlAgerState Ager State
 * @{
 */
#define SML_AGER_STATE_ACTIVE   (1)     /**< Ager가 활동중인 상태 */
#define SML_AGER_STATE_INACTIVE (2)     /**< Ager가 멈춘상태 */
/** @} */

/**
 * @brief invalid 트랜잭션 아이디
 */
#define SML_INVALID_TRANSID  STL_INT64_C( 0xFFFFFFFFFFFFFFFF )

/**
 * @brief invalid 트랜잭션 아이디
 */
#define SML_DUMMY_TRANSID    STL_INT64_C( 0x0 )

#define SML_IN_DOUBT_DECISION_NONE          (0)     /**< preserve in-doubt transaction */
#define SML_IN_DOUBT_DECISION_COMMIT        (1)     /**< commit in-doubt transaction */
#define SML_IN_DOUBT_DECISION_ROLLBACK      (2)     /**< rollback in-doubt transaction */

#define SML_MAX_COMMIT_COMMENT_SIZE         (128)   /**< maximum commit comment size */

/** @} */


/**
 * @addtogroup smlGeneral
 * @{
 */

#define SML_SEGMENT_MAP_RID           ( (smlRid) {0,0,1} )

/**
 * @brief 서버 상태
 */
typedef stlInt32 smlServerState;

#define SML_SERVER_STATE_NONE        (0)   /**< 초기 상태 */
#define SML_SERVER_STATE_RECOVERING  (1)   /**< Recovery가 진행중 상태 */
#define SML_SERVER_STATE_RECOVERED   (2)   /**< Recovery가 완료된 상태 */
#define SML_SERVER_STATE_SERVICE     (3)   /**< 서비스가 가능한 상태 */
#define SML_SERVER_STATE_SHUTDOWN    (4)   /**< 서버가 정상 종료된 상태 */

/**
 * @brief SCN 타입
 */
typedef stlUInt64 smlScn;

/**
 * @brief 무한대 SCN
 */
#define SML_INFINITE_SCN          (STL_UINT64_MAX)
/**
 * @brief 생성 자체를 보이지 않게 만들기 위한 특별한 SCN
 */
#define SML_INVISIBLE_CREATE_SCN  (SML_INFINITE_SCN-1)
/**
 * @brief 최대 Stable SCN
 */
#define SML_MAXIMUM_STABLE_SCN    (SML_INFINITE_SCN-2)
/**
 * @brief Transaction의 SCN을 결정할수 없는 경우 사용
 */
#define SML_RECYCLED_TRANS_SCN    (SML_INFINITE_SCN-3)

/* smp, smd, smn 등에서 page에 view scn을 commit scn과 구별하여 저장할 시에 쓰임 */
#define SML_VIEW_SCN_MASK       STL_UINT64_C(0x8000000000000000)
#define SML_IS_VIEW_SCN(scn)    (((scn) & SML_VIEW_SCN_MASK) == SML_VIEW_SCN_MASK ? STL_TRUE : STL_FALSE )
#define SML_MAKE_VIEW_SCN(scn)  ((scn) | SML_VIEW_SCN_MASK)
#define SML_MAKE_REAL_SCN(scn)  ((scn) & (~SML_VIEW_SCN_MASK))


/**
 * @addtogroup smlTrans
 * @{
 */

/** @} */

#define SML_MAX_MINITRANS_COUNT (8)  /**< 동시에 열수 있는 Mini Transction의 최대 개수 */

/**
 * @brief Storage Manager Environment 구조체
 */
typedef struct smlEnv
{
    sclEnv           mSclEnv;              /**< Server Communication Environment */
    knlRegionMem     mOperationHeapMem;    /**< Operation 레벨 힙 메모리 할당/해제 */
    knlRegionMem     mLogHeapMem;          /**< Oparation 레벨 로그 힙 메모리 할당/해제 */
    knlDynamicMem    mDynamicHeapMem;      /**< Operation 레벨 다이나믹 메모리 할당/해제 */
    
   /**
    * Exclusive Latch를 획득한 Mini-Transaction Stack
    */
    void           * mExclMiniTransStack[SML_MAX_MINITRANS_COUNT];
    stlInt16         mMiniTransStackTop;
    stlInt32         mCurLatchCount;   /**< for validate storage manager */
} smlEnv;

/**
 * @brief Storage Manager Environment를 얻는 매크로
 */
#define SML_ENV( aEnv ) ((smlEnv*)(aEnv))

/**
 * @brief Checkpoint event
 */
#define SML_EVENT_CHECKPOINT          KNL_MAKE_EVENT_ID( STL_LAYER_STORAGE_MANAGER, 0 )
/**
 * @brief Drop Table event
 */
#define SML_EVENT_DROP_TABLE          KNL_MAKE_EVENT_ID( STL_LAYER_STORAGE_MANAGER, 1 )
/**
 * @brief Drop Index event
 */
#define SML_EVENT_DROP_INDEX          KNL_MAKE_EVENT_ID( STL_LAYER_STORAGE_MANAGER, 2 )
/**
 * @brief Archivelog event
 */
#define SML_EVENT_ARCHIVELOG          KNL_MAKE_EVENT_ID( STL_LAYER_STORAGE_MANAGER, 3 )
/**
 * @brief Deactivate ager event
 */
#define SML_EVENT_DEACTIVATE_AGER     KNL_MAKE_EVENT_ID( STL_LAYER_STORAGE_MANAGER, 4 )
/**
 * @brief Drop Tablespace event
 */
#define SML_EVENT_DROP_TBS            KNL_MAKE_EVENT_ID( STL_LAYER_STORAGE_MANAGER, 5 )
/**
 * @brief Drop Datafile event
 */
#define SML_EVENT_DROP_DATAFILE       KNL_MAKE_EVENT_ID( STL_LAYER_STORAGE_MANAGER, 6 )
/**
 * @brief Page flushing event
 */
#define SML_EVENT_PAGE_FLUSH          KNL_MAKE_EVENT_ID( STL_LAYER_STORAGE_MANAGER, 7 )
/**
 * @brief Wakeup Log Flusher event
 */
#define SML_EVENT_WAKEUP_LOG_FLUSHER  KNL_MAKE_EVENT_ID( STL_LAYER_STORAGE_MANAGER, 8 )
/**
 * @brief Page flushing event related to a tablespace
 */
#define SML_EVENT_PAGE_FLUSH_TBS      KNL_MAKE_EVENT_ID( STL_LAYER_STORAGE_MANAGER, 9 )
/**
 * @brief Loopback event
 */
#define SML_EVENT_LOOPBACK            KNL_MAKE_EVENT_ID( STL_LAYER_STORAGE_MANAGER, 10 )
/**
 * @brief datafile IO event
 */
#define SML_EVENT_DATAFILE_LOAD       KNL_MAKE_EVENT_ID( STL_LAYER_STORAGE_MANAGER, 11 )
/**
 * @brief flush datafile event
 */
#define SML_EVENT_IO_SLAVE_FLUSH_DATAFILE  KNL_MAKE_EVENT_ID( STL_LAYER_STORAGE_MANAGER, 12 )
/**
 * @brief add logfile group event
 */
#define SML_EVENT_ADD_LOGFILE_GROUP        KNL_MAKE_EVENT_ID( STL_LAYER_STORAGE_MANAGER, 13 )
/**
 * @brief drop logfile group event
 */
#define SML_EVENT_DROP_LOGFILE_GROUP       KNL_MAKE_EVENT_ID( STL_LAYER_STORAGE_MANAGER, 14 )
/**
 * @brief rename logfile event
 */
#define SML_EVENT_RENAME_LOGFILE           KNL_MAKE_EVENT_ID( STL_LAYER_STORAGE_MANAGER, 15 )
/**
 * @brief switch logfile group event
 */
#define SML_EVENT_SWITCH_LOGFILE_GROUP     KNL_MAKE_EVENT_ID( STL_LAYER_STORAGE_MANAGER, 16 )
/**
 * @brief alter archivelog event
 */
#define SML_EVENT_ALTER_ARCHIVELOG         KNL_MAKE_EVENT_ID( STL_LAYER_STORAGE_MANAGER, 17 )
/**
 * @brief media recover event
 */
#define SML_EVENT_MEDIA_RECOVER            KNL_MAKE_EVENT_ID( STL_LAYER_STORAGE_MANAGER, 18 )
/**
 * @brief escalate warmup phase
 */
#define SML_EVENT_ESCALATE_WARMUP_PHASE    KNL_MAKE_EVENT_ID( STL_LAYER_STORAGE_MANAGER, 19 )
/**
 * @brief restore event
 */
#define SML_EVENT_RESTORE                  KNL_MAKE_EVENT_ID( STL_LAYER_STORAGE_MANAGER, 20 )
/**
 * @brief drop sequence
 */
#define SML_EVENT_DROP_SEQUENCE            KNL_MAKE_EVENT_ID( STL_LAYER_STORAGE_MANAGER, 21 )
/**
 * @brief aging lock item
 */
#define SML_EVENT_AGING_LOCK_ITEM          KNL_MAKE_EVENT_ID( STL_LAYER_STORAGE_MANAGER, 22 )
/**
 * @brief add datafile
 */
#define SML_EVENT_ADD_DATAFILE             KNL_MAKE_EVENT_ID( STL_LAYER_STORAGE_MANAGER, 23 )
/**
 * @brief rename datafile
 */
#define SML_EVENT_RENAME_DATAFILE          KNL_MAKE_EVENT_ID( STL_LAYER_STORAGE_MANAGER, 24 )
/**
 * @brief offline tbs
 */
#define SML_EVENT_OFFLINE_TBS              KNL_MAKE_EVENT_ID( STL_LAYER_STORAGE_MANAGER, 25 )
/**
 * @brief add logfile member
 */
#define SML_EVENT_ADD_LOGFILE_MEMBER       KNL_MAKE_EVENT_ID( STL_LAYER_STORAGE_MANAGER, 26 )
/**
 * @brief drop logfile member
 */
#define SML_EVENT_DROP_LOGFILE_MEMBER      KNL_MAKE_EVENT_ID( STL_LAYER_STORAGE_MANAGER, 27 )

/**
 * @brief Storage Manager maximum event count
 */
#define SML_MAX_EVENT_COUNT (28)

#define SML_CHECKPOINT_ENV_ID               smlGetCheckpointEnvId()
#define SML_LOG_FLUSHER_ENV_ID              smlGetLogFlusherEnvId()
#define SML_PAGE_FLUSHER_ENV_ID             smlGetPageFlusherEnvId()
#define SML_AGER_ENV_ID                     smlGetAgerEnvId()
#define SML_CLEANUP_ENV_ID                  smlGetCleanupEnvId()
#define SML_ARCHIVELOG_ENV_ID               smlGetArchivelogEnvId()
#define SML_IO_SLAVE_ENV_ID( aThreadIdx )   smlGetIoSlaveEnvId( aThreadIdx )

/** @} */

typedef struct smlStatement smlStatement;

#define SML_CACHED_STMT_MAX_COUNT (3)

/**
 * @addtogroup smlSessionEnv
 * @{
 */

typedef struct smlSessionEnv
{
    sclSessionEnv    mSclSessionEnv;       /**< Server Communication Session Environment */
    stlRingHead      mPendOp;              /**< Pending Operation List */
    knlRegionMem     mSessionMem;          /**< Session 레벨 메모리 할당/해제 */
    knlDynamicMem    mStatementMem;        /**< Statement 레벨 메모리 할당/해제 */
    knlDynamicMem    mDynamicMem;          /**< 동적으로 할당/해제 가능한 메모리; ex) for instant table */

    smlStatement   * mStatement[SML_CACHED_STMT_MAX_COUNT]; /**< mStatementMem에서 할당받는 smlStatement */
    void           * mRelHintPool;         /**< RELATION 힌트 배열 포인터 */
    void           * mSegHintPool;         /**< 세그먼트 힌트 배열 포인터 */
    void           * mTbsHintPool;         /**< 테이블스페이스  힌트 배열 포인터 */
    stlRingHead      mReadOnlyStmt;        /**< Read-Only Atomic Statement Execution Context */
    stlRingHead      mUpdatableStmt;       /**< Updatable Atomic Statement Execution Context */
    smlScn           mMinSnapshotStmtViewScn;  /**< Minimum snapshoted statement view scn */
    smlScn           mMinStmtViewScn;      /**< Minimum statement view scn */
    stlInt64         mSegmentRetryCount;   /**< for performance view */
    stlInt64         mRelationRetryCount;  /**< for performance view */
    stlInt64         mFixCount;            /**< for performance view */
    stlInt64         mVersionConflictCount;/**< for performance view */
    smlScn           mDisabledViewScn;     /**< Aging을 막기 위한 View Scn */
    smlScn           mMinTransViewScn;     /**< transaction이 시작할 시점에서 최소 Transaction view scn */
    stlTime          mMinSnapshotStmtBeginTime;    /**< mMinSnapshotStmtViewScn을 가진 Statement의 begin time */
    smlTransId       mTransId;             /**< Environment에 Bind된 Transaction Identifier */
    smlScn           mTransViewScn;        /**< Environment에 Bind 된 Transaction view scn */
    knlStaticHash  * mSeqHash;             /**< Sequence Hash */

    void           * mInstTableList;       /**< Instant table handle list */
    void           * mMediaRecoveryInfo;   /**< Information for Media Recovery */
} smlSessionEnv;

/**
 * @brief Storage Manager Session Environment를 얻는다
 * @param[in] aEnv Environment 포인터
 * @note aEnv는 Session Environment가 아니고, Process Environment이다.
 */
#define SML_SESS_ENV( aEnv )  ((smlSessionEnv*)(KNL_ENV(aEnv)->mSessionEnv))

/** @} */

/**
 * @defgroup smlError Error
 * @{
 */

/**
 * @brief initialize error
 */
#define SML_ERRCODE_INITIALIZE                               STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 0 )
/**
 * @brief initialized yet error
 */
#define SML_ERRCODE_INITIALIZED_YET                          STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 1 )
/**
 * @brief exceed datafile limit error
 */
#define SML_ERRCODE_EXCEED_DATAFILE_LIMIT                    STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 2 )
/**
 * @brief tabasespace full
 */
#define SML_ERRCODE_TABLESPACE_FULL                          STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 3 )
/**
 * @brief dropping already dropped tablespace
 */
#define SML_ERRCODE_ALREADY_DROPPED_TBS                      STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 4 )
/**
 * @brief exists system tablespace
 */
#define SML_ERRCODE_EXISTS_SYSTEM_TBS                        STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 5 )
/**
 * @brief Mutating table error
 */
#define SML_ERRCODE_MUTATING_TABLE                           STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 6 )
/**
 * @brief Deadlock error
 */
#define SML_ERRCODE_DEADLOCK                                 STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 7 )
/**
 * @brief Fetch Out of sequence
 */
#define SML_ERRCODE_FETCH_SEQUENCE                           STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 8 )
/**
 * @brief sequence cannot be instantiated
 */
#define SML_ERRCODE_SEQUENCE_LIMIT_EXCEEDED                  STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 9 )
/**
 * @brief MAXVALUE cannot be made to be less than the current value
 */
#define SML_ERRCODE_SEQUENCE_MAXVALUE_BELOW_CURRENT_VALUE    STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 10 )
/**
 * @brief MINVALUE cannot be made to exceed the current value
 */
#define SML_ERRCODE_SEQUENCE_MINVALUE_EXCEED_CURRENT_VALUE   STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 11 )
/**
 * @brief cannot be made to be less than the current value
 */
#define SML_ERRCODE_SEQUENCE_SETVALUE_BELOW_CURRENT_VALUE    STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 12 )
/**
 * @brief cannot be made to exceed the current value
 */
#define SML_ERRCODE_SEQUENCE_SETVALUE_EXCEED_CURRENT_VALUE   STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 13 )
/**
 * @brief cannot extend datafile
 */
#define SML_ERRCODE_DATAFILE_EXTEND                          STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 14 )
/**
 * @brief there is no extendible datafile in tablespace '%s'
 */
#define SML_ERRCODE_NO_MORE_EXTENDIBLE_DATAFILE              STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 15 )
/**
 * @brief one or more rows of source table violate uniqueness of index
 */
#define SML_ERRCODE_INDEX_UNIQUENESS_VIOLATION               STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 16 )
/**
 * @brief invalid operation on fixed table or dump table
 */
#define SML_ERRCODE_INVALID_OPERATION_ON_FIXED_TABLE         STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 17 )
/**
 * @brief the operation is disallowed before %s startup phase
 */
#define SML_ERRCODE_INVALID_STARTUP_PHASE                    STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 18 )
/**
 * @brief invalid log stream number
 */
#define SML_ERRCODE_INVALID_LOGSTREAM_NUMBER                 STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 19 )
/**
 * @brief already exists log stream
 */
#define SML_ERRCODE_EXISTS_LOGSTREAM                         STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 20 )
/**
 * @brief invalid log group number
 */
#define SML_ERRCODE_INVALID_LOGGROUP_NUMBER                  STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 21 )
/**
 * @brief log group state must be INACTIVE or UNUSED
 */
#define SML_ERRCODE_INVALID_LOGGROUP_STATE                   STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 22 )
/**
 * @brief already exists log group
 */
#define SML_ERRCODE_EXISTS_LOGGROUP                          STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 23 )
/**
 * @brief already exists log member
 */
#define SML_ERRCODE_EXISTS_LOGMEMBER                         STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 24 )
/**
 * @brief invalid dump option string for dump table
 */
#define SML_ERRCODE_INVALID_DUMP_OPTION_STRING               STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 25 )
/**
 * @brief resource busy or timeout expired
 */
#define SML_ERRCODE_LOCK_TIMEOUT                             STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 26 )
/**
 * @brief datafile is already dropped
 */
#define SML_ERRCODE_ALREADY_DROPPED_DATAFILE                 STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 27 )
/**
 * @brief file is already exist
 */
#define SML_ERRCODE_ALREADY_EXIST_FILE                       STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 28 )
/**
 * @brief sequence is not yet defined
 */
#define SML_ERRCODE_SEQUENCE_NOT_YET_DEFINED                 STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 29 )
/**
 * @brief maximum index depth exceeded
 */
#define SML_ERRCODE_EXCEEDED_MAX_INDEX_DEPTH                 STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 30 )
/**
 * @brief maximum index Key Size 
 */
#define SML_ERRCODE_EXCEEDED_MAX_KEY_SIZE                    STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 31 )
/**
 * @brief can't serialize access for this transaction
 */
#define SML_ERRCODE_SERIALIZE_TRANSACTION                    STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 32 )
/**
 * @brief unable to checkpoint in cds mode
 */
#define SML_ERRCODE_UNABLE_CHECKPOINT_IN_CDS_MODE            STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 33 )
/**
 * @brief unable to recover database in cds mode
 */
#define SML_ERRCODE_UNABLE_RECOVER_IN_CDS_MODE               STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 34 )
/**
 * @brief one or more rows of source table violate not null constraint
 */
#define SML_ERRCODE_NOT_NULL_CONSTRAINT_VIOLATION            STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 35 )
/**
 * @brief invalid ROWID
 */
#define SML_ERRCODE_INVALID_ROWID                            STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 36 )
/**
 * @brief Database is READ ONLY
 */
#define SML_ERRCODE_DATABASE_IS_READ_ONLY                    STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 37 )
/**
 * @brief unable to recover database in READ ONLY mode
 */
#define SML_ERRCODE_UNABLE_RECOVER_IN_READ_ONLY_MODE         STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 38 )
/**
 * @brief exceed segment MAXSIZE(%ld)
 */
#define SML_ERRCODE_EXCEED_SEGMENT_MAXSIZE                   STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 39 )
/**
 * @brief target object of dump table is mutating
 */
#define SML_ERRCODE_MUTATING_DUMP_OBJECT                     STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 40 )
/**
 * @brief cannot access offline tablespace
 */
#define SML_ERRCODE_ACCESS_OFFLINE_TBS                       STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 41 )
/**
 * @brief cannot find the row (%ld)
 */
#define SML_ERRCODE_NO_INST_ROW                              STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 42 )
/**
 * @brief file is already used in database - '%s'
 */
#define SML_ERRCODE_FILE_ALREADY_USED                        STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 43 )
/**
 * @brief datafile not empty
 */
#define SML_ERRCODE_NON_EMPTY_DATAFILE                       STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 44 )
/**
 * @brief cannot drop datafile of OFFLINE tablespace '%s'
 */
#define SML_ERRCODE_DROP_OFFLINE_DATAFILE                    STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 45 )
/**
 * @brief file does not exist - '%s'
 */
#define SML_ERRCODE_FILE_NOT_EXIST                           STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 46 )
/**
 * @brief file is READ ONLY - '%s'
 */
#define SML_ERRCODE_FILE_IS_READ_ONLY                        STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 47 )
/**
 * @brief file is invalid or old version - '%s'
 */
#define SML_ERRCODE_INVALID_DATAFILE                         STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 48 )
/**
 * @brief tablespace is not online - '%s'
 */
#define SML_ERRCODE_NOT_ONLINE_TABLESPACE                    STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 49 )
/**
 * @brief tablespace is not offline - '%s'
 */
#define SML_ERRCODE_NOT_OFFLINE_TABLESPACE                   STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 50 )
/**
 * @brief media recovery required - '%s'
 */
#define SML_ERRCODE_NEED_MEDIA_RECOVERY                      STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 51 )
/**
 * @brief segment is unusable ( physical id : %ld )
 */
#define SML_ERRCODE_ACCESS_UNUSABLE_SEGMENT                  STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 52 )
/**
 * @brief insufficient resource for sort
 */
#define SML_ERRCODE_SORT_OVERFLOW                            STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 53 )
/**
 * @brief internal error ( %s, %d )
 */
#define SML_ERRCODE_INTERNAL                                 STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 54 )
/**
 * @brief segment is corrupted ( physical id : %ld )
 */
#define SML_ERRCODE_SEGMENT_CORRUPTED                        STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 55 )
/**
 * @brief failed to apply redo log ( lsn : %ld, lpsn : %d, physical id : %ld )
 */
#define SML_ERRCODE_FAILED_TO_REDO                           STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 56 )
/**
 * @brief minimum log group count violation
 */
#define SML_ERRCODE_MINIMUM_LOGGROUP_COUNT                   STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 57 )
/**
 * @brief log member is not exist '%s'
 */
#define SML_ERRCODE_LOG_MEMBER_NOT_EXIST                     STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 58 )
/**
 * @brief logfile is invalid - '%s'
 */
#define SML_ERRCODE_ALREADY_USED_LOG_MEMBER                  STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 59 )
/**
 * @brief CURRENT logfile is empty - '%s'
 */
#define SML_ERRCODE_EMPTY_LOGFILE_GROUP                      STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 60 )
/**
 * @brief next logfile to switch should be INACTIVE or UNUSED - '%s'
 */
#define SML_ERRCODE_NEXT_LOGGROUP_ACTIVE                     STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 61 )
/**
 * @brief INITRANS must be less equal than MAXTRANS
 */
#define SML_ERRCODE_TRANS_RANGE                              STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 62 )
/**
 * @brief cannot be added log member to exceed maximum log member count(%d)
 */
#define SML_ERRCODE_EXCEED_MAX_LOGMEMBER_COUNT               STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 63 )
/**
 * @brief the operation is disallowed at %s phase
 */
#define SML_ERRCODE_NOT_APPROPRIATE_PHASE                    STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 64 )
/**
 * @brief the instant table row length is too large
 */
#define SML_ERRCODE_INSTANT_TABLE_ROW_TOO_LARGE              STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 65 )
/**
 * @brief the sort instant table key row length is too large
 */
#define SML_ERRCODE_SORT_TABLE_KEY_ROW_TOO_LARGE             STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 66 )
/**
 * @brief cannot media recover; archivelog mode not enabled
 */
#define SML_ERRCODE_NOT_ARCHIVELOG_MODE                      STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 67 )
/**
 * @brief logfile does not exist - '%s'
 */
#define SML_ERRCODE_LOGFILE_NOT_EXIST                        STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 68 )
/**
 * @brief cannot support ARCHIVELOG in cds mode
 */
#define SML_ERRCODE_UNABLE_TO_ARCHIVELOG_IN_CDS_MODE         STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 69 )
/**
 * @brief control file is not recent; do media recovery
 */
#define SML_ERRCODE_OLD_CONTROL_FILE                         STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 70 )
/**
 * @brief cannot BEGIN BACKUP; tablespace(%s) is offline
 */
#define SML_ERRCODE_CANNOT_BACKUP_OFFLINE_TABLESPACE         STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 71 )
/**
 * @brief cannot BACKUP; tablespace(%s) is temporary
 */
#define SML_ERRCODE_CANNOT_BACKUP_TEMPORARY_TABLESPACE       STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 72 )
/**
 * @brief cannot BEGIN BACKUP; tablespace is dropped
 */
#define SML_ERRCODE_CANNOT_BACKUP_DROPPED_TABLESPACE         STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 73 )
/**
 * @brief cannot BACKUP; tablespace(%s) is already in backup
 */
#define SML_ERRCODE_CANNOT_BEGIN_BACKUP                      STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 74 )
/**
 * @brief cannot execute; tablespace(%s) is proceeding backup
 */
#define SML_ERRCODE_BACKUP_IN_PROGRESS                       STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 75 )
/**
 * @brief cannot END BACKUP; tablespace(%s) is not in backup
 */
#define SML_ERRCODE_CANNOT_END_BACKUP                        STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 76 )
/**
 * @brief cannot END BACKUP; none of the tablespaces are in backup
 */
#define SML_ERRCODE_ALL_TBS_NOT_IN_BACKUP                    STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 77 )
/**
 * @brief cannot BEGIN BACKUP; tablespace(%s) is proceeding other operation
 */
#define SML_ERRCODE_CANNOT_BACKUP_BUSY_TABLESPACE            STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 78 )
/**
 * @brief cannot BACKUP; database is already in backup
 */
#define SML_ERRCODE_DATABASE_IN_BACKUP                       STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 79 )
/**
 * @brief cannot END BACKUP; database is not in backup
 */
#define SML_ERRCODE_CANNOT_END_BACKUP_DATABASE               STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 80 )
/**
 * @brief cannot BACKUP; database is going shutdown
 */
#define SML_ERRCODE_DATABASE_IN_SHUTDOWN                     STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 81 )
/**
 * @brief (%s) is not an appropriate logfile to recover
 */
#define SML_ERRCODE_NOT_APPROPRIATE_LOGFILE                  STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 82 )
/**
 * @brief must use RESETLOGS option for database open
 */
#define SML_ERRCODE_MUST_OPEN_RESETLOGS                      STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 83 )
/**
 * @brief must use NORESETLOGS option for database open
 */
#define SML_ERRCODE_CANNOT_OPEN_RESETLOGS                    STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 84 )
/**
 * @brief (%s) was not a sufficiently old backup
 */
#define SML_ERRCODE_NOT_SUFFICIENT_OLD_BACKUP                STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 85 )
/**
 * @brief media recovery canceled
 */
#define SML_ERRCODE_MEDIA_RECOVERY_CANCELED                  STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 86 )
/**
 * @brief cannot SHUTDOWN; backup in progress
 */
#define SML_ERRCODE_CANNOT_PREVENT_BACKUP                    STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 87 )
/**
 * @brief control file is corrupted
 */
#define SML_ERRCODE_CORRUPTED_CONTROLFILE                    STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 88 )
/**
 * @brief LEVEL 0 INCREMENTAL BACKUP does not exist
 */
#define SML_ERRCODE_NOT_EXIST_LEVEL_0_BACKUP                 STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 89 )
/**
 * @brief cannot flush logs; disabled log flushing
 */
#define SML_ERRCODE_DISABLED_LOG_FLUSHING                    STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 90 )
/**
 * @brief (%s) is not valid logfile
 */
#define SML_ERRCODE_LOGFILE_NOT_VALID                        STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 91 )
/**
 * @brief not exist incremental backup
 */ 
#define SML_ERRCODE_NOT_EXIST_BACKUP                         STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 92 )
/**
 * @brief not exist valid incremental backup for tablespace(%s)
 */ 
#define SML_ERRCODE_NOT_EXIST_VALID_INCREMENTAL_BACKUP       STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 93 )
/**
 * @brief tablespace(%s) has corrupted datafile(%s)
 */ 
#define SML_ERRCODE_DATAFILE_REQUIRED_RECOVERY               STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 94 )
/**
 * @brief backup file(%s) is corrupted
 */ 
#define SML_ERRCODE_BACKUP_IS_CORRUPTED                      STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 95 )

/**
 * @brief fail to build index ( %ld )
 */ 
#define SML_ERRCODE_INDEX_REBUILD                            STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 96 )
/**
 * @brief control file is corrupted - '%s'
 */
#define SML_ERRCODE_CTRLFILE_CORRUPTED                       STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 97 )
/**
 * @brief maximum record length(%d) exceeds
 */
#define SML_ERRCODE_EXCEEDED_MAX_INSTANT_ROW_SIZE            STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 98 )
/**
 * @brief log member state must be UNUSED
 */
#define SML_ERRCODE_INVALID_LOGMEMBER_STATE                  STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 99 )
/**
 * @brief minimum log member count violation
 */
#define SML_ERRCODE_MINIMUM_LOGMEMBER_COUNT                  STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 100 )
/**
 * @brief log group id does not exist - '%d'
 */ 
#define SML_ERRCODE_NOT_EXIST_LOGGROUP_ID                    STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 101 )
/**
 * @brief cannot execute; recovery is in progress
 */
#define SML_ERRCODE_RECOVERY_IN_PROGRESS                     STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 102 )
/**
 * @brief Warning: media recovery needs a logfile including log (Lsn %ld)
 */
#define SML_ERRCODE_WARNING_NEED_SPECIFIC_ARCHIVED_LOGFILE   STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 103 )
/**
 * @brief Warning: suggestion '%s'
 */
#define SML_ERRCODE_WARNING_SUGGESTED_ARCHIVED_LOGFILE       STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 104 )
/**
 * @brief control file '%s' is inconsistent with primary
 */
#define SML_ERRCODE_INCONSISTENT_CONTROL_FILE                STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 105 )
/**
 * @brief tablespace (%s) is taken offline as the result of a write error
 */
#define SML_ERRCODE_TABLESPACE_OFFLINE_ABNORMALLY            STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 106 )
/**
 * @brief given LOG_BLOCK_SIZE(%d) is not a suitable value for a size of log block; LOG_BLOCK_SIZE should be one of 512, 1024, 2048 or 4096.
 */
#define SML_ERRCODE_UNSUITABLE_LOG_BLOCK_SIZE                STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 107 )
/**
 * @brief cannot disable archivelog - exist tablespace needeed media recovery.
 */
#define SML_ERRCODE_CHANGE_NOARCHIVELOG_MODE                 STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 108 )
/**
 * @brief OFFLINE NORMAL disallowed; cannot be taken offline consistently.
 */
#define SML_ERRCODE_TABLESPACE_OFFLINE_NORMAL                STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 109 )
/**
 * @brief OFFLINE IMMEDIATE disallowed; noarchivelog mode.
 */
#define SML_ERRCODE_TABLESPACE_OFFLINE_IMMEDIATE             STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 110 )
/**
 * @brief cannot create COLUMNAR TABLE; given MIN ROW COUNT is too big to put into one page.
 */
#define SML_ERRCODE_MIN_ROW_VIOLATION                        STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 111 )
/**
 * @brief record size is too large for columnar table.
 */
#define SML_ERRCODE_COLUMNAR_TABLE_INVALID_ROW_SIZE          STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 112 )
/**
 * @brief datafile '%s' is corrupted
 */
#define SML_ERRCODE_DATAFILE_IS_CORRUPTED                    STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 113 )
/**
 * @brief datafile '%s' is more recent than redo logfile
 */
#define SML_ERRCODE_DATAFILE_RECENT_THAN_REDO_LOGFILE        STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 114 )
/**
 * @brief transaction undo limit exceeded
 */
#define SML_ERRCODE_EXCEED_TRANSACTION_UNDO                  STL_MAKE_ERRCODE( STL_ERROR_MODULE_STORAGE_MANAGER, 115 )

#define SML_MAX_ERROR    116

/** @} */

/**
 * @addtogroup smlLock
 * @{
 */

/**
 * @defgroup smlLockMode Lock Mode
 * @ingroup smlLock
 * @{
 */
#define SML_LOCK_NONE  0     /**< None */
#define SML_LOCK_IS    1     /**< Intention shared */
#define SML_LOCK_IX    2     /**< Intention exclusive */
#define SML_LOCK_S     3     /**< Shared */
#define SML_LOCK_X     4     /**< Exclusive */
#define SML_LOCK_SIX   5     /**< Shared intention exclusive */
/** @} */

/**
 * @defgroup smlLockTimeout Lock Timeout
 * @ingroup smlLock
 * @{
 */
/**
 * @brief 락을 기다리지 않음
 */
#define SML_LOCK_TIMEOUT_NOWAIT      STL_INT64_C( 0x0 )
/**
 * @brief 무한대기
 */
#define SML_LOCK_TIMEOUT_INFINITE    STL_INT64_C( 0x7FFFFFFE )
/**
 * @brief 락 대기 시간은 프로퍼티에 따라 결정됨
 */
#define SML_LOCK_TIMEOUT_PROPERTY    STL_INT64_C( 0x7FFFFFFF )
/**
 * @brief 사용하지 않는 Lock Timeout
 */
#define SML_LOCK_TIMEOUT_INVALID     STL_INT64_C( 0xFFFFFFFFFFFFFFFF )
/** @} */

/** @} */

/**
 * @addtogroup smlSequence
 * @{
 */

/**
 * @defgroup smlSequenceValidMask Sequence Valid Mask
 * @ingroup smlSequence
 * @{
 */
#define SML_SEQ_STARTWITH_MASK      0x00000001     /**< Start With Mask */
#define SML_SEQ_STARTWITH_YES       0x00000001     /**< Attr.mStartWith 설정이 유효함 */
#define SML_SEQ_STARTWITH_NO        0x00000000     /**< Attr.mStartWith 설정이 유효하지 않음 */

#define SML_SEQ_INCREMENTBY_MASK    0x00000002     /**< Increment By Mask */
#define SML_SEQ_INCREMENTBY_YES     0x00000002     /**< Attr.mIncrementBy 설정이 유효함 */
#define SML_SEQ_INCREMENTBY_NO      0x00000000     /**< Attr.mIncrementBy 설정이 유효하지 않음 */

#define SML_SEQ_MAXVALUE_MASK       0x00000004     /**< MaxValue Mask */
#define SML_SEQ_MAXVALUE_YES        0x00000004     /**< Attr.mMaxValue 설정이 유효함 */
#define SML_SEQ_MAXVALUE_NO         0x00000000     /**< Attr.mMaxValue 설정이 유효하지 않음 */

#define SML_SEQ_MINVALUE_MASK       0x00000008     /**< MinValue Mask */
#define SML_SEQ_MINVALUE_YES        0x00000008     /**< Attr.mMinValue 설정이 유효함 */
#define SML_SEQ_MINVALUE_NO         0x00000000     /**< Attr.mMinValue 설정이 유효하지 않음 */

#define SML_SEQ_CACHESIZE_MASK      0x00000010     /**< Cache Size Mask */
#define SML_SEQ_CACHESIZE_YES       0x00000010     /**< Attr.mCacheSize 설정이 유효함 */
#define SML_SEQ_CACHESIZE_NO        0x00000000     /**< Attr.mCacheSize 설정이 유효하지 않음 */

#define SML_SEQ_CYCLE_MASK          0x00000020     /**< Cycle Mask */
#define SML_SEQ_CYCLE_YES           0x00000020     /**< Attr.mCycle 설정이 유효함 */
#define SML_SEQ_CYCLE_NO            0x00000000     /**< Attr.mCycle 설정이 유효하지 않음 */

#define SML_SEQ_NOCACHE_MASK        0x00000040     /**< No Cache Mask */
#define SML_SEQ_NOCACHE_YES         0x00000040     /**< Attr.mNoCache 설정이 유효함 */
#define SML_SEQ_NOCACHE_NO          0x00000000     /**< Attr.mNoCache 설정이 유효하지 않음 */

#define SML_SEQ_NOMAXVALUE_MASK     0x00000080     /**< No MaxValue Mask */
#define SML_SEQ_NOMAXVALUE_YES      0x00000080     /**< Attr.mNoMaxValue 설정이 유효함 */
#define SML_SEQ_NOMAXVALUE_NO       0x00000000     /**< Attr.mNoMaxValue 설정이 유효하지 않음 */

#define SML_SEQ_NOMINVALUE_MASK     0x00000100     /**< No MinValue Mask */
#define SML_SEQ_NOMINVALUE_YES      0x00000100     /**< Attr.mNoMinValue 설정이 유효함 */
#define SML_SEQ_NOMINVALUE_NO       0x00000000     /**< Attr.mNoMinValue 설정이 유효하지 않음 */

#define SML_SEQ_RESTART_MASK        0x00000600     /**< No MinValue Mask */
#define SML_SEQ_RESTART_DEFAULT     0x00000200     /**< Attr.mRestartValue를 Default로 설정 */
#define SML_SEQ_RESTART_VALUE       0x00000400     /**< Attr.mRestartValue를 주어진 value로 설정 */
#define SML_SEQ_RESTART_NO          0x00000000     /**< Attr.mRestartValue 설정이 유효하지 않음 */
/** @} */

/**
 * @brief Sequence DEFAULT Value
 * @{
 */
#define SML_INCREMENTBY_DEFAULT           (1)
#define SML_POSITIVE_MINVALUE_DEFAULT     (1)
#define SML_NEGATIVE_MINVALUE_DEFAULT     (STL_INT64_MIN)
#define SML_POSITIVE_MAXVALUE_DEFAULT     (STL_INT64_MAX)
#define SML_NEGATIVE_MAXVALUE_DEFAULT     (-1)
#define SML_CACHESIZE_DEFAULT             (20)
/** @} */


/**
 * @brief Sequence Atrribute
 */
typedef struct smlSequenceAttr
{
    stlUInt32 mValidFlags;                /**< 멤버중 유효한 변수 @see smlSequenceValidMask */
    stlInt64  mStartWith;                 /**< 시작 정수형 값 */
    stlInt64  mIncrementBy;               /**< 증가 값 */
    stlInt64  mMaxValue;                  /**< 최대 증가값 */
    stlInt64  mMinValue;                  /**< 최소 증가값 */
    stlInt64  mCacheSize;                 /**< 캐시 사이즈 */
    stlInt64  mRestartValue;              /**< 캐시 사이즈 */
    stlBool   mCycle;                     /**< 사이클 허용여부 */
    stlBool   mNoCache;                   /**< 캐시를 허용하지 않는가? */
    stlBool   mNoMaxValue;                /**< 최대값 허용여부 */
    stlBool   mNoMinValue;                /**< 최소값 허용여부 */
} smlSequenceAttr;

/** @} */

/**
 * @addtogroup smlTablespace
 * @{
 */

/**
 * @brief 테이블스페이스 아이디
 */
typedef stlUInt16 smlTbsId;

/**
 * @defgroup smlTablespaceId System Defined Tablespace Identifier
 * @ingroup smlTablespace
 * @{
 */
#define SML_MEMORY_DICT_SYSTEM_TBS_ID  0  /**< Memory Dictionary System Tablespace */
#define SML_MEMORY_UNDO_SYSTEM_TBS_ID  1  /**< Memory Undo System Tablespace */
#define SML_MEMORY_DATA_SYSTEM_TBS_ID  2  /**< Memory Data System Tablespace */
#define SML_MEMORY_TEMP_SYSTEM_TBS_ID  3  /**< Memory Temporary System Tablespace */
#define SML_MAX_SYSTEM_TBS_ID          4

/** @} */

/**
 * @brief Offline Behavior 정의
 */
typedef enum
{
    SML_OFFLINE_BEHAVIOR_INVALID = 0,
    SML_OFFLINE_BEHAVIOR_NORMAL,       /**< offline normal */
    SML_OFFLINE_BEHAVIOR_IMMEDIATE     /**< offline immediate */
} smlOfflineBehavior;

typedef struct smlOfflineTbsArg
{
    smlTbsId            mTbsId;           /**< targe offline tablespace Id */
    stlChar             mPadding[2];
    smlOfflineBehavior  mOfflineBehavior; /**< offline behavior(normal or immediate) */
} smlOfflineTbsArg;

/**
 * @defgroup smlTablespaceName System Defined Tablespace Name
 * @ingroup smlTablespace
 * @{
 */
#define SML_MEMORY_DICT_SYSTEM_TBS_NAME  "DICTIONARY_TBS"  /**< Memory Dictionary System Tablespace */
#define SML_MEMORY_UNDO_SYSTEM_TBS_NAME  "MEM_UNDO_TBS"    /**< Memory Undo System Tablespace */
#define SML_MEMORY_DATA_SYSTEM_TBS_NAME  "MEM_DATA_TBS"    /**< Memory Data System Tablespace */
#define SML_MEMORY_TEMP_SYSTEM_TBS_NAME  "MEM_TEMP_TBS"    /**< Memory Temporary System Tablespace */
/** @} */

/**
 * @brief 데이터파일 아이디
 */
typedef stlUInt16 smlDatafileId;

/**
 * @brief 데이터파일 아이디
 */
typedef stlUInt32 smlPid;

/**
 * @brief 익스텐트 아이디
 */
typedef stlUInt64 smlExtId;

/**
 * @brief Invalid 익스텐트 아이디
 */
#define SML_INVALID_EXTID  STL_INT64_C(0xFFFFFFFFFFFFFFFF)

/**
 * @brief Invalid Datafile Identifier
 */
#define SML_INVALID_DATAFILE_ID (0xFFFF)

/**
 * @defgroup smlDatafileValidMask Datafile Valid Masks
 * @ingroup smlTablesapce
 * @{
 */
#define SML_DATAFILE_AUTOEXTEND_MASK  0x00000001 /**< smlDatafileAttr.mAutoExtend Mask */
#define SML_DATAFILE_AUTOEXTEND_YES   0x00000001 /**< smlDatafileAttr.mAutoExtend 설정이 유효함 */
#define SML_DATAFILE_AUTOEXTEND_NO    0x00000000 /**< smlDatafileAttr.mAutoExtend 설정이 유효함 */

#define SML_DATAFILE_SIZE_MASK        0x00000002 /**< smlDatafileAttr.mSize Mask */
#define SML_DATAFILE_SIZE_YES         0x00000002 /**< smlDatafileAttr.mSize 설정이 유효함 */
#define SML_DATAFILE_SIZE_NO          0x00000000 /**< smlDatafileAttr.mSize 설정이 유효하지 않음 */

#define SML_DATAFILE_NEXTSIZE_MASK    0x00000004 /**< smlDatafileAttr.mNextSize Mask */
#define SML_DATAFILE_NEXTSIZE_YES     0x00000004 /**< smlDatafileAttr.mNextSize 설정이 유효함 */
#define SML_DATAFILE_NEXTSIZE_NO      0x00000000 /**< smlDatafileAttr.mNextSize 설정이 유효하지 않음 */

#define SML_DATAFILE_MAXSIZE_MASK     0x00000008 /**< smlDatafileAttr.mMaxSize Mask */
#define SML_DATAFILE_MAXSIZE_YES      0x00000008 /**< smlDatafileAttr.mMaxSize 설정이 유효함 */
#define SML_DATAFILE_MAXSIZE_NO       0x00000000 /**< smlDatafileAttr.mMaxSize 설정이 유효하지 않음 */

#define SML_DATAFILE_REUSE_MASK       0x00000010 /**< smlDatafileAttr.mReuse Mask */
#define SML_DATAFILE_REUSE_YES        0x00000010 /**< smlDatafileAttr.mReuse 설정이 유효함 */
#define SML_DATAFILE_REUSE_NO         0x00000000 /**< smlDatafileAttr.mReuse 설정이 유효하지 않음 */

#define SML_DATAFILE_NAME_MASK        0x00000020 /**< smlDatafileAttr.mName Mask */
#define SML_DATAFILE_NAME_YES         0x00000020 /**< smlDatafileAttr.mName 설정이 유효함 */
#define SML_DATAFILE_NAME_NO          0x00000000 /**< smlDatafileAttr.mName 설정이 유효하지 않음 */

/** @} */

typedef struct smlDatafileAttr smlDatafileAttr;
/**
 * @brief 데이터파일 속성 구조체
 */
struct smlDatafileAttr
{
    stlUInt32         mValidFlags;                       /**< 멤버중 유효한 변수 */
    stlInt32          mDatafileId;                       /**< 데이터파일 아이디 (SM 내부 변수) */
    stlChar           mName[STL_MAX_FILE_PATH_LENGTH];   /**< 데이터파일 이름(+경로) */
    smlTbsId          mTbsId;                            /**< 데이터파일의 Owner Tablespace Id */
    stlBool           mAutoExtend;                       /**< 자동 확장 여부 */
    stlUInt64         mSize;                             /**< 초기 크기 */
    stlUInt64         mNextSize;                         /**< 다음 확장 크기 */
    stlUInt64         mMaxSize;                          /**< 최대 크기 */
    stlUInt64         mReuse;                            /**< 재사용 여부 */
    smlDatafileAttr * mNext;                             /**< 다음 데이터파일 포인터 */
};

/**
 * @brief 테이블스페이스 속성 구조체
 */
typedef struct smlTbsAttr
{
    stlChar           mName[STL_MAX_SQL_IDENTIFIER_LENGTH]; /**< 테이블스페이스 이름 */
    stlUInt32         mValidFlags;                          /**< 멤버중 유효한 변수 @see smlTablesapceValidMask */
    stlUInt32         mAttr;                                /**< attribute */
    stlInt64          mExtSize;                             /**< 한 Extent의 바이트 단위 크기 */
    stlBool           mIsLogging;                           /**< Logging/Nologging */
    stlBool           mIsOnline;                            /**< Online/Offline */
    stlChar           mPadding[6];
    smlDatafileAttr * mDatafileAttr;                        /**< 데이터파일 리스트 */
} smlTbsAttr;

/**
 * @brief 테이블스페이스 최대 개수
 */
#define SML_TBS_MAX_COUNT 65535

#define SML_INVALID_TBS_ID    (65535)

/**
 * @brief 테이블스페이스당  최대 데이터파일 개수
 */
#define SML_DATAFILE_MAX_COUNT 1024

/**
 * @defgroup smlTablesapceValidMask Tablespace Valid Mask
 * @ingroup smlTablesapce
 * @{
 */

#define SML_TBS_LOGGING_MASK      0x00000001    /**< smlTbsAttr.mIsLogging Mask */
#define SML_TBS_LOGGING_YES       0x00000001    /**< smlTbsAttr.mIsLogging 설정이 유효함 */
#define SML_TBS_LOGGING_NO        0x00000000    /**< smlTbsAttr.mIsLogging 설정이 유효하지 않음 */

#define SML_TBS_ONLINE_MASK       0x00000002     /**< smlTbsAttr.mIsOnline Mask */
#define SML_TBS_ONLINE_YES        0x00000002     /**< smlTbsAttr.mIsOnline 설정이 유효함 */
#define SML_TBS_ONLINE_NO         0x00000000     /**< smlTbsAttr.mIsOnline 설정이 유효하지 않음 */

#define SML_TBS_EXTSIZE_MASK      0x00000004     /**< smlTbsAttr.mExtSize Mask */
#define SML_TBS_EXTSIZE_YES       0x00000004     /**< smlTbsAttr.mExtSize 설정이 유효함 */
#define SML_TBS_EXTSIZE_NO        0x00000000     /**< smlTbsAttr.mExtSize 설정이 유효하지 않음 */

#define SML_TBS_ATTR_MASK         0x00000008     /**< smlTbsAttr.mAttr Mask */
#define SML_TBS_ATTR_YES          0x00000008     /**< smlTbsAttr.mAttr 설정이 유효함 */
#define SML_TBS_ATTR_NO           0x00000000     /**< smlTbsAttr.mAttr 설정이 유효하지 않음 */

/** @} */

/**
 * @defgroup smlTablespaceAttr Tablespace Attribute Flags
 * @ingroup smlTablespace
 * @{
 */
#define SML_TBS_DEVICE_MASK       0x00000001    /**< Device Attribute */
#define SML_TBS_DEVICE_MEMORY     0x00000000    /**< On Memory */
#define SML_TBS_DEVICE_DISK       0x00000001    /**< On Disk */

#define SML_TBS_TEMPORARY_MASK    0x00000002    /**< Temporary Attribute */
#define SML_TBS_TEMPORARY_YES     0x00000002    /**< Temporary */
#define SML_TBS_TEMPORARY_NO      0x00000000    /**< Persistent */

#define SML_TBS_USAGE_MASK        0x000000F0    /**< Usage Attribute */
#define SML_TBS_USAGE_DICT        0x00000000    /**< Dictionary tablespace */
#define SML_TBS_USAGE_UNDO        0x00000010    /**< Undo tablespace */
#define SML_TBS_USAGE_DATA        0x00000020    /**< Data tablespace */
#define SML_TBS_USAGE_TEMPORARY   0x00000040    /**< Temporary tablespace */

#define SML_TBS_DEFAULT_ATTR      (SML_TBS_DEVICE_MEMORY |  \
                                   SML_TBS_TEMPORARY_NO  |  \
                                   SML_TBS_USAGE_DATA)
#define SML_TBS_DEFAULT_EXTSIZE   (SMF_PAGE_COUNT_IN_EXT * SMP_PAGE_SIZE)

/**
 * @brief Maximum Count of Parallel IO Group
 */
#define SML_MAX_PARALLEL_IO_GROUP      (32)

/** @} */

/**
 * @brief 테이블스페이스 물리적 타입 아이디
 */
#define SML_TBS_DEVICE_TYPE_ID( aType ) ( ((aType) & SML_TBS_DEVICE_MASK) >> 4 )
/**
 * @brief 테이블스페이스 물리적 타입
 */
#define SML_TBS_DEVICE_TYPE( aType )    ( (aType) & SML_TBS_DEVICE_MASK )
/**
 * @brief 테이블스페이스 USAGE 타입
 */
#define SML_TBS_USAGE_TYPE( aType )    ( (aType) & SML_TBS_USAGE_MASK )
/**
 * @brief 테이블스페이스 Temporary 여부
 */
#define SML_TBS_IS_TEMPORARY( aAttr )    \
    ( ((aAttr) & SML_TBS_TEMPORARY_MASK) == SML_TBS_TEMPORARY_YES ? STL_TRUE : STL_FALSE)

/**
 * @brief 테이블스페이스 Temporary 여부
 */
#define SML_TBS_IS_MEMORY_TEMPORARY( aAttr )                                    \
    ( ((SML_TBS_DEVICE_TYPE( (aAttr) ) == SML_TBS_DEVICE_MEMORY) &&             \
       (SML_TBS_IS_TEMPORARY( (aAttr) ) == STL_TRUE)) ? STL_TRUE : STL_FALSE)

/**
 * @brief Datafile Minimum Size (1 mega)
 */
#define SML_DATAFILE_MIN_SIZE ( STL_INT64_C(1*1024*1024) )
/**
 * @brief Datafile Maximum Size (30 giga)
 */
#define SML_DATAFILE_MAX_SIZE ( STL_INT64_C(30*1024*1024*1024) )

/**
 * @brief Extent size class 1
 */
#define SML_EXTSIZE_CLASS_1   (   8 * SMP_PAGE_SIZE )
/**
 * @brief Extent size class 2
 */
#define SML_EXTSIZE_CLASS_2   (  16 * SMP_PAGE_SIZE )
/**
 * @brief Extent size class 3
 */
#define SML_EXTSIZE_CLASS_3   (  32 * SMP_PAGE_SIZE )
/**
 * @brief Extent size class 4
 */
#define SML_EXTSIZE_CLASS_4   (  64 * SMP_PAGE_SIZE )
/**
 * @brief Extent size class 5
 */
#define SML_EXTSIZE_CLASS_5   ( 128 * SMP_PAGE_SIZE )

/** @} */

/**
 * @addtogroup smlRelation
 * @{
 */

/**
 * @brief Relation Type
 * 가상 테이블인 Fixed Table 과 Dump Table을 위해
 * 하위 layer 인 Kernel layer 와 약속한 상수값이다.
 */
typedef enum
{
    /*
     * table relations
     */
    SML_PROCESS_FIXED_TABLE = KNL_FXTABLE_USAGE_FIXED_TABLE,
    SML_PROCESS_DUMP_TABLE  = KNL_FXTABLE_USAGE_DUMP_TABLE,  
    
    SML_MEMORY_PERSISTENT_HEAP_ROW_TABLE,      /**< Memory Heap Row-based Table */
    SML_MEMORY_PERSISTENT_HEAP_COLUMNAR_TABLE, /**< Memory Heap Column-based Table */
    SML_MEMORY_PERSISTENT_PENDING_TABLE,       /**< Memory Pending Table */

    /*
     * instant table은 반드시 연속적으로 와야 한다.
     */
    SML_MEMORY_INSTANT_FLAT_TABLE,            /**< Memory Instant Flat Table */
    SML_MEMORY_INSTANT_SORT_TABLE,            /**< Memory Instant Sort Table */
    SML_MEMORY_INSTANT_HASH_TABLE,            /**< Memory Instant Hash Table */

    /*
     * index relations
     * index relation들은 반드시 아래에 추가되어야 한다.
     */
    SML_MEMORY_PERSISTENT_BTREE_INDEX,        /**< Memory Btree Index */
    
    /*
     * instant index는 반드시 연속적으로 와야 한다.
     */
    SML_MEMORY_INSTANT_HASH_INDEX,            /**< Memory Instant Hash Index */
    
    SML_RELATION_TYPE_MAX
} smlRelationType;

#define SML_RELATION_IS_PERSISTENT_TABLE(aRelType)                                      \
    (((aRelType >= SML_MEMORY_PERSISTENT_HEAP_ROW_TABLE) &&                             \
      (aRelType <= SML_MEMORY_PERSISTENT_PENDING_TABLE)) ? STL_TRUE : STL_FALSE)
#define SML_RELATION_IS_PERSISTENT_INDEX(aRelType)                     \
    ((aRelType == SML_MEMORY_PERSISTENT_BTREE_INDEX) ? STL_TRUE : STL_FALSE)

#define SML_RELATION_IS_INSTANT_TABLE(aRelType)                        \
    ( ((aRelType >= SML_MEMORY_INSTANT_FLAT_TABLE) &&                  \
       (aRelType <= SML_MEMORY_INSTANT_HASH_TABLE)) ?                  \
      STL_TRUE : STL_FALSE )

#define SML_RELATION_IS_INSTANT_INDEX(aRelType)                        \
    ( ((aRelType >= SML_MEMORY_INSTANT_HASH_INDEX) &&                  \
       (aRelType <= SML_MEMORY_INSTANT_HASH_INDEX)) ?                  \
      STL_TRUE : STL_FALSE )

/**
 * @brief Segment 속성 구조체
 */
typedef struct smlSegmentAttr
{
    stlUInt32 mValidFlags;   /**< 멤버중 유효한 변수 */

    stlInt64  mInitial;
    stlInt64  mNext;
    stlInt64  mMinSize;
    stlInt64  mMaxSize;
    stlInt64  mShrinkToSize;
} smlSegmentAttr;

/**
 * @brief Columnar Table의 속성 구조체
 */
typedef struct smlColumnarAttr
{
    stlInt16     mMinRowCount;          /**< 하나의 page에 기록될 최소 record 수(생략 시 2개) */
    stlInt16     mContColumnThreshold;  /**< continuous column을 저장하기 위한 threshold */
    stlInt32     mColCount;             /**< Table의 전체 column 수 */
    stlInt16   * mColLengthArray;       /**< 각 Column Length Array */
} smlColumnarAttr;

#define SML_COLUMNAR_TABLE_MIN_ROW_COUNT                       (2)
#define SML_COLUMNAR_TABLE_MIN_ROW_COUNT_MIN                   (2)
// #define SML_COLUMNAR_TABLE_MIN_ROW_COUNT_MAX                   (256)

// #define SML_COLUMNAR_TABLE_CONTINUOUS_COLUMN_THRESHOLD         (2000)

#define SML_COLUMNAR_TABLE_CONTINUOUS_COLUMN_THRESHOLD_MIN              \
    (KNL_PROPERTY_COLUMNAR_TABLE_CONTINUOUS_COLUMN_THRESHOLD_MIN)
#define SML_COLUMNAR_TABLE_CONTINUOUS_COLUMN_THRESHOLD_MAX              \
    (KNL_PROPERTY_COLUMNAR_TABLE_CONTINUOUS_COLUMN_THRESHOLD_MAX)

/**
 * @brief Drop storage option
 * @todo SML_DROP_STORAGE_REUSE를 구현해야 함.
 */
typedef enum
{
    SML_DROP_STORAGE_NONE = 0,  /**< none */
    SML_DROP_STORAGE_DROP,      /**< MINEXTENT를 제외한 user extent 삭제 */
    SML_DROP_STORAGE_DROP_ALL,  /**< 할당된 모든 user extent 삭제 */
    SML_DROP_STORAGE_REUSE      /**< 할당된 extent를 삭제하지 않는다 */
} smlDropStorageOption;

/**
 * @defgroup smlSegmentAttr Segment Attribute Flags
 * @ingroup smlRelation
 * @{
 */
#define SML_SEGMENT_INITIAL_MASK        0x00000001
#define SML_SEGMENT_INITIAL_YES         0x00000001
#define SML_SEGMENT_INITIAL_NO          0x00000000

#define SML_SEGMENT_NEXT_MASK           0x00000002
#define SML_SEGMENT_NEXT_YES            0x00000002
#define SML_SEGMENT_NEXT_NO             0x00000000

#define SML_SEGMENT_MINSIZE_MASK        0x00000004
#define SML_SEGMENT_MINSIZE_YES         0x00000004
#define SML_SEGMENT_MINSIZE_NO          0x00000000

#define SML_SEGMENT_MAXSIZE_MASK        0x00000008
#define SML_SEGMENT_MAXSIZE_YES         0x00000008
#define SML_SEGMENT_MAXSIZE_NO          0x00000000

#define SML_SEGMENT_SHRINKTOSIZE_MASK   0x00000010
#define SML_SEGMENT_SHRINKTOSIZE_YES    0x00000010
#define SML_SEGMENT_SHRINKTOSIZE_NO     0x00000000

#define SML_SEGMENT_DEFAULT_ATTR         ( SML_SEGMENT_INITIAL_YES      |  \
                                           SML_SEGMENT_NEXT_YES         |  \
                                           SML_SEGMENT_MINSIZE_YES      |  \
                                           SML_SEGMENT_SHRINKTOSIZE_NO  |  \
                                           SML_SEGMENT_MAXSIZE_YES )
/** @} */

#define SML_INVALID_RELATION_ID    (0)  /**< invalid relation identifier */

/** @} */

/**
 * @addtogroup smlTable
 * @{
 */

/**
 * @defgroup smlTableType Table Type
 * @ingroup smlTable
 * @{
 */
#define SML_TABLE_TYPE_MEMORY_PERSISTENT_HEAP_ROW      0
#define SML_TABLE_TYPE_MEMORY_PERSISTENT_HEAP_COLUMNAR 1
#define SML_TABLE_TYPE_MEMORY_PERSISTENT_PENDING       2
#define SML_TABLE_TYPE_MEMORY_INSTANT_FLAT             3
#define SML_TABLE_TYPE_MEMORY_INSTANT_SORT             4
#define SML_TABLE_TYPE_MEMORY_INSTANT_HASH             5
#define SML_TABLE_TYPE_MAX                             6
/** @} */

/**
 * Table Type redifinition
 */
typedef stlInt32 smlTableType;

#define SML_GET_TABLE_TYPE(aRelType)                \
    ( (aRelType) - SML_MEMORY_PERSISTENT_HEAP_ROW_TABLE )

/**
 * @brief 테이블 속성 구조체
 */
typedef struct smlTableAttr
{
    stlUInt32       mValidFlags;   /**< 멤버중 유효한 변수 */

    stlBool         mLoggingFlag;
    stlBool         mLockingFlag;
    stlBool         mScrollable;
    stlInt16        mIniTrans;
    stlInt16        mMaxTrans;
    stlInt16        mPctFree;
    stlInt16        mPctUsed;

    smlSegmentAttr  mSegAttr;

    smlColumnarAttr mColumnarAttr;
} smlTableAttr;

/**
 * @defgroup smlTableAttr Table Attribute Flags
 * @ingroup smlTable
 * @{
 */
#define SML_TABLE_LOGGING_MASK           0x00000001
#define SML_TABLE_LOGGING_YES            0x00000001
#define SML_TABLE_LOGGING_NO             0x00000000

#define SML_TABLE_INITRANS_MASK          0x00000002
#define SML_TABLE_INITRANS_YES           0x00000002
#define SML_TABLE_INITRANS_NO            0x00000000

#define SML_TABLE_MAXTRANS_MASK          0x00000004
#define SML_TABLE_MAXTRANS_YES           0x00000004
#define SML_TABLE_MAXTRANS_NO            0x00000000

#define SML_TABLE_LOCKING_MASK           0x00000008
#define SML_TABLE_LOCKING_YES            0x00000008
#define SML_TABLE_LOCKING_NO             0x00000000

#define SML_TABLE_TYPE_MASK              0x000000F0
#define SML_TABLE_TYPE_DISK_PERSISTENT   0x00000000
#define SML_TABLE_TYPE_DISK_INSTANT      0x00000010
#define SML_TABLE_TYPE_MEMORY_PERSISTENT 0x00000020
#define SML_TABLE_TYPE_MEMORY_INSTANT    0x00000030

#define SML_TABLE_PCTFREE_MASK           0x00000100
#define SML_TABLE_PCTFREE_YES            0x00000100
#define SML_TABLE_PCTFREE_NO             0x00000000

#define SML_TABLE_PCTUSED_MASK           0x00000200
#define SML_TABLE_PCTUSED_YES            0x00000200
#define SML_TABLE_PCTUSED_NO             0x00000000

#define SML_TABLE_SCROLLABLE_MASK        0x00000400
#define SML_TABLE_SCROLLABLE_YES         0x00000400
#define SML_TABLE_SCROLLABLE_NO          0x00000000

#define SML_TABLE_DEFAULT_MEMORY_ATTR    ( SML_TABLE_LOGGING_YES   |            \
                                           SML_TABLE_INITRANS_YES  |            \
                                           SML_TABLE_MAXTRANS_YES  |            \
                                           SML_TABLE_PCTUSED_YES   |            \
                                           SML_TABLE_PCTFREE_YES   |            \
                                           SML_TABLE_SCROLLABLE_NO |            \
                                           SML_TABLE_TYPE_MEMORY_PERSISTENT )
/** @} */

#define SML_MIN_INITRANS             (1)   /**< minimum initrans */
#define SML_MAX_INITRANS             (32)  /**< maximum initrans */
#define SML_MIN_MAXTRANS             (1)   /**< minimum maxtrans */
#define SML_MAX_MAXTRANS             (32)  /**< maximum maxtrans */

#define SML_MIN_PCTFREE             (0)    /**< minimum pctfree */
#define SML_MAX_PCTFREE             (99)   /**< maximum pctfree */
#define SML_MIN_PCTUSED             (0)    /**< minimum pctused */
#define SML_MAX_PCTUSED             (99)   /**< maximum pctused */

/**
 * @brief Segment Type
 */
typedef enum
{
    SML_SEG_TYPE_MEMORY_BITMAP = 0,  /**< Memory bitmap segment */
    SML_SEG_TYPE_MEMORY_CIRCULAR,    /**< Memory circular segment */
    SML_SEG_TYPE_MEMORY_FLAT,        /**< Memory flat segment */
    SML_SEG_TYPE_MEMORY_PENDING,     /**< Memory pending segment */
    SML_SEG_TYPE_DISK_BITMAP,        /**< disk bitmap segment */
    SML_SEG_TYPE_DISK_CIRCULAR,      /**< disk circular segment */
    SML_SEG_TYPE_MAX
} smlSegType;

/** @} */

/**
 * @addtogroup smlIndex
 * @{
 */

/**
 * @defgroup smlIndexType Index Type
 * @ingroup smlIndex
 * @{
 */
#define SML_INDEX_TYPE_MEMORY_PERSISTENT_BTREE  ( 0 )
#define SML_INDEX_TYPE_MEMORY_INSTANT_HASH      ( 1 )
#define SML_INDEX_TYPE_MAX                      ( 2 )
/** @} */

/**
 * @brief Index Type redifinition
 */
typedef stlInt32 smlIndexType;

#define SML_INDEX_IS_INSTANT(aIndexType)                        \
    ( ((aIndexType >= SML_INDEX_TYPE_MEMORY_INSTANT_HASH) &&    \
       (aIndexType <= SML_INDEX_TYPE_MEMORY_INSTANT_HASH)) ?    \
      STL_TRUE : STL_FALSE )

#define SML_GET_INDEX_TYPE(aRelType)                    \
    ( (aRelType) - SML_MEMORY_PERSISTENT_BTREE_INDEX )

/**
 * @brief 인덱스 속성 구조체
 */
typedef struct smlIndexAttr
{
    stlUInt32       mValidFlags;   /**< 멤버중 유효한 변수 */

    stlBool         mLoggingFlag;
    stlBool         mSkipNullFlag;  /**< NULL을 키로 갖는 레코드는 무시한다(Hash Instant 에만 적용) */
    stlInt16        mIniTrans;
    stlInt16        mMaxTrans;
    stlInt16        mPctFree;
    stlBool         mUniquenessFlag;
    stlBool         mPrimaryFlag;

    smlSegmentAttr  mSegAttr;
} smlIndexAttr;


/**
 * @defgroup smlIndexAttr Index Attribute Flags
 * @ingroup smlIndex
 * @{
 */
#define SML_INDEX_LOGGING_MASK           0x00000001
#define SML_INDEX_LOGGING_YES            0x00000001
#define SML_INDEX_LOGGING_NO             0x00000000

#define SML_INDEX_INITRANS_MASK          0x00000002
#define SML_INDEX_INITRANS_YES           0x00000002
#define SML_INDEX_INITRANS_NO            0x00000000

#define SML_INDEX_MAXTRANS_MASK          0x00000004
#define SML_INDEX_MAXTRANS_YES           0x00000004
#define SML_INDEX_MAXTRANS_NO            0x00000000

#define SML_INDEX_UNIQUENESS_MASK        0x00000008
#define SML_INDEX_UNIQUENESS_YES         0x00000008
#define SML_INDEX_UNIQUENESS_NO          0x00000000

#define SML_INDEX_PRIMARY_MASK           0x00000010
#define SML_INDEX_PRIMARY_YES            0x00000010
#define SML_INDEX_PRIMARY_NO             0x00000000

#define SML_INDEX_PCTFREE_MASK           0x00000020
#define SML_INDEX_PCTFREE_YES            0x00000020
#define SML_INDEX_PCTFREE_NO             0x00000000

#define SML_INDEX_SKIP_NULL_MASK         0x00000040
#define SML_INDEX_SKIP_NULL_YES          0x00000040
#define SML_INDEX_SKIP_NULL_NO           0x00000000
/** @} */

/**
 * @brief 한 index가 가질 수 있는 최대 key column 개수
 */
#define  SML_MAX_INDEX_KEY_COL_COUNT   (32)

/**
 * @brief maximum parallel factor
 */
#define SML_MAX_INDEX_BUILD_PARALLEL_FACTOR   (64)  

/**
 * @defgroup smlUniqueViolationScope Uniqiue Violation Scope
 * @{
 */
/**
 * @brief Unique Violation이 발생하지 않음
 */
#define SML_UNIQUE_VIOLATION_SCOPE_NONE      (0)
/**
 * @brief Unique Violation이 해소되었음.
 */
#define SML_UNIQUE_VIOLATION_SCOPE_RESOLVED  (1)
/**
 * @brief Unique Violation이 Row Level에서 발생함.
 * <BR> Object에 Transaction/Statement Level의 Unique Violation을 검사해야함.
 */
#define SML_UNIQUE_VIOLATION_SCOPE_ROW       (2)
/**
 * @brief Unique Violation이 Statement Level에서 발생함.
 * <BR> Object에 Transaction Level의 Defered Constraint가 없다면 100% Unique Violation을 보장함.
 */
#define SML_UNIQUE_VIOLATION_SCOPE_STMT      (3)
/** @} */

/**
 * @brief Redifinition for Unique Violation Scope
 */
typedef stlUInt8  smlUniqueViolationScope;

/** @} */


/**
 * @addtogroup smlRecovery
 * @{
 */

#define SML_INCREMENTAL_BACKUP_LEVEL_MAX      (5)

#define SML_BACKUP_OBJECT_TYPE_NONE           (0)
#define SML_BACKUP_OBJECT_TYPE_DATABASE       (1)
#define SML_BACKUP_OBJECT_TYPE_TABLESPACE     (2)
#define SML_BACKUP_OBJECT_TYPE_CONTROLFILE    (3)

#define SML_BACKUP_TYPE_NONE                  (0)
#define SML_BACKUP_TYPE_FULL                  (1)
#define SML_BACKUP_TYPE_INCREMENTAL           (2)

#define SML_BACKUP_COMMAND_NONE               (0)
#define SML_BACKUP_COMMAND_BEGIN              (1)
#define SML_BACKUP_COMMAND_END                (2)
#define SML_BACKUP_COMMAND_PREVENT            (3)
#define SML_BACKUP_COMMAND_PREVENT_ROLLBACK   (4)

#define SML_INCREMENTAL_BACKUP_OPTION_DIFFERENTIAL   (0)
#define SML_INCREMENTAL_BACKUP_OPTION_CUMULATIVE     (1)

#define SML_DELETE_OBSOLETE_BACKUP_LIST   (0)
#define SML_DELETE_ALL_BACKUP_LIST        (1)

/**
 * @brief backup 시 parameter
 */
typedef struct smlBackupMessage
{
    stlUInt8      mBackupObject;             /**< Backup Object : DATABASE, CONTROLFILE, TABLESPACE */
    stlUInt8      mBackupType;               /**< Backup Type : FULL, INCREMENTAL */
    stlUInt8      mCommand;                  /**< Backup Command : BEGIN, END, PREVENT, PREVENT_ROLLBACK */
    stlUInt8      mIncrementalBackupOption;  /**< Incremental Backup Option : DIFFERENTIAL, CUMULATIVE */
    stlInt16      mIncrementalBackupLevel;   /**< Incremental Backup Level : 0 ~ 4 */
    smlTbsId      mTbsId;                    /**< Incremental Backup Target Tablespace Id */
    stlChar       mTarget[STL_MAX_FILE_PATH_LENGTH];  /**< Backup Destination Filename (Controlfile only) */
} smlBackupMessage;

/**
 * @brief Recovery Command
 */
#define SML_RECOVERY_AT_MOUNT_RECOVERY_BEGIN      (0)  /**< Mount phase에서 recovery begin */
#define SML_RECOVERY_AT_MOUNT_RECOVERY_PREVENT    (1)  /**< Mount phase에서 recovery prevent */
#define SML_RECOVERY_AT_MOUNT_RECOVERY_END        (2)  /**< Mount phase에서 recovery end */

/**
 * @brief Recovery Type
 */
typedef stlUInt8  smlRecoveryType;
#define SML_RECOVERY_TYPE_NONE                         (0x00)
#define SML_RECOVERY_TYPE_RESTART_RECOVERY             (0x00) /**< Restart Recovery */
#define SML_RECOVERY_TYPE_COMPLETE_MEDIA_RECOVERY      (0x01) /**< Complete Media Recovery */
#define SML_RECOVERY_TYPE_INCOMPLETE_MEDIA_RECOVERY    (0x02) /**< Incomplete Media Recovery */

/**
 * @brief Recovery Object Type
 */
typedef stlUInt8  smlRecoveryObjectType;
#define SML_RECOVERY_OBJECT_TYPE_NONE          (0x00)
#define SML_RECOVERY_OBJECT_TYPE_DATABASE      (0x01) /**< Recovery Target : Database */
#define SML_RECOVERY_OBJECT_TYPE_TABLESPACE    (0x02) /**< Recovery Target : Tablespace */
#define SML_RECOVERY_OBJECT_TYPE_DATAFILE      (0x03) /**< Recovery Target : Datafile */

/**
 * @brief Incomplete Media Recovery Option(IMR Option)
 */
typedef stlUInt8  smlImrOption;
#define SML_IMR_OPTION_NONE         (0x00) /**< COMPLETE MEDIA RECOVERY  */
#define SML_IMR_OPTION_BATCH        (0x01) /**< UNTIL TIME, UNTIL CHANGE */
#define SML_IMR_OPTION_INTERACTIVE  (0x02) /**< INTERACTIVE RECOVERY */

/**
 * @brief Batch, Interactive Incomplete Recovery Condition(IMR Condition)
 */
typedef stlUInt8 smlImrCondition;
#define SML_IMR_CONDITION_NONE        (0x00)
#define SML_IMR_CONDITION_TIME        (0x01) /**< UNTIL TIME   */
#define SML_IMR_CONDITION_CHANGE      (0x02) /**< UNTIL CHANGE   */
#define SML_IMR_CONDITION_BEGIN       (0x03) /**< Interactive Incomplete Recovery Begin */
#define SML_IMR_CONDITION_SUGGESTION  (0x04) /**< Recover suggested logfile */
#define SML_IMR_CONDITION_MANUAL      (0x05) /**< Recover input logfile */
#define SML_IMR_CONDITION_AUTO        (0x06) /**< Recover all logfiles */
#define SML_IMR_CONDITION_END         (0x07) /**< Interactive Incomplete Recovery End */

/**
 * @brief media recovery시 parameter
 */
typedef struct smlMediaRecoveryInfo
{
    smlRecoveryType          mRecoveryType;  /**< Media Recovery Type : COMPLETE, INCOMPLETE */
    smlRecoveryObjectType    mObjectType;    /**< Recovery Object : DATABASE, TABLESPACE, DATAFILE */
    smlImrOption             mImrOption;     /**< Incomplete Recovery Option : BATCH, INTERACTIVE */
    smlImrCondition          mImrCondition;  /**< Incomplete Recovery Condition : TIME, CHANGE, Interactive conds */
    stlChar                  mPadding[2];
    stlUInt16                mTbsId;         /**< Target Tablespace Id */
    stlInt64                 mUntilValue;    /**< Batch Incomplete Recovery Until Value */
} smlMediaRecoveryInfo;

typedef struct smlRestoreInfo
{
    stlUInt16       mTbsId;
    stlUInt8        mUntilType;
    stlChar         mPadding[5];
    stlInt64        mUntilValue;
} smlRestoreInfo;

#define SML_RESTORE_UNTIL_TYPE_NONE    (0x00)
#define SML_RESTORE_UNTIL_TYPE_TIME    (0x01)
#define SML_RESTORE_UNTIL_TYPE_CHANGE  (0x02)

/**
 * @brief datafile recovery 수행시 Datafile에 관련된 정보 
 */ 
typedef struct smlMediaRecoveryDatafile
{
    smlTbsId      mTbsId;                                 /**< Datafile의 Tablespace Id */
    smlDatafileId mDatafileId;                            /**< Datafile의 Datafile Id */
    stlBool       mUsingBackup;                           /**< Using Backup File Option */
    stlBool       mForCorruption;                         /**< Recover Corrupted Pages */
    stlChar       mPadding[2];
    stlChar       mBackupName[STL_MAX_FILE_PATH_LENGTH];  /**< Using Backup File Name */
    stlRingHead   mCorruptedPageList;                     /**< Corrupted Page List */
    stlRingEntry  mDatafileLink;                          /**< Recover Datafile List */
} smlMediaRecoveryDatafile;

/**
 * @brief datafile recovery 수행시 Information
 */ 
typedef struct smlDatafileRecoveryInfo
{
    smlMediaRecoveryInfo       mCommonInfo;         /**< Media Recovery Common Info */
    stlInt32                   mDatafileCount;      /**< Recover Datafile Count */
    stlChar                    mPadding[4];
    smlMediaRecoveryDatafile * mDatafileInfo;       /**< Recovery Datafile List */
} smlDatafileRecoveryInfo;

/**
 * @brief Mount Phase에서 Page Recovery 수행시에 Marshalling Offset.
 */
#define SML_GET_DATAFILE_RECOVERY_DATAFILE_OFFSET( aData )                      \
    (smlMediaRecoveryDatafile *)( (aData) + STL_OFFSETOF(smlDatafileRecoveryInfo, mDatafileInfo) )

/**
 * @brief Startup시 Log option 정의
 */
#define SML_STARTUP_LOG_OPTION_NORESETLOGS         (0x00)
#define SML_STARTUP_LOG_OPTION_RESETLOGS           (0x01)

/**
 * @brief pending log buffer size
 */
#define SML_PENDING_LOG_BUFFER_SIZE   (1 * 1024 * 1024) /* 1M */
/**
 * @brief minimum log group count
 */
#define SML_MINIMUM_LOG_GROUP_COUNT   (4)
/**
 * @brief minimum log file size (20M)
 */
#define SML_MINIMUM_LOG_FILE_SIZE     (20 * 1024 * 1024)
/**
 * @brief maximum log file size (10G)
 */
#define SML_MAXIMUM_LOG_FILE_SIZE     STL_INT64_C(10 * 1024 * 1024 * 1024)

/**
 * @defgroup smlLogMemberFlags Log Member Attribute Flags
 * @{
 */
#define SML_LOG_MEMBER_NAME_MASK  0x00000001        /**< Name Attribute */
#define SML_LOG_MEMBER_NAME_YES   0x00000001        /**< Name */
#define SML_LOG_MEMBER_NAME_NO    0x00000000        /**< No Name */

#define SML_LOG_MEMBER_REUSE_MASK 0x00000002        /**< Name Attribute */
#define SML_LOG_MEMBER_REUSE_YES  0x00000002        /**< Name */
#define SML_LOG_MEMBER_REUSE_NO   0x00000000        /**< No Name */
/** @} */

/**
 * @defgroup smlLogGroupFlags Log Group Attribute Flags
 * @{
 */
#define SML_LOG_GROUP_GROUPID_MASK   0x00000001        /**< Group Id Attribute */
#define SML_LOG_GROUP_GROUPID_YES    0x00000001        /**< Group Id */
#define SML_LOG_GROUP_GROUPID_NO     0x00000000        /**< No Group Id */

#define SML_LOG_GROUP_FILESIZE_MASK  0x00000002        /**< File Size Attribute */
#define SML_LOG_GROUP_FILESIZE_YES   0x00000002        /**< File Size */
#define SML_LOG_GROUP_FILESIZE_NO    0x00000000        /**< No File Size */

#define SML_LOG_GROUP_REUSE_MASK     0x00000004        /**< Reuse Flag Attribute */
#define SML_LOG_GROUP_REUSE_YES      0x00000004        /**< Reuse Flag*/
#define SML_LOG_GROUP_REUSE_NO       0x00000000        /**< No Reuse Flag */
/** @} */

/**
 * @defgroup smlLogStreamFlags Log Stream Attribute Flags
 * @{
 */
#define SML_LOG_STREAM_STREAMID_MASK   0x00000001        /**< Stream Id Attribute */
#define SML_LOG_STREAM_STREAMID_YES    0x00000001        /**< Stream Id */
#define SML_LOG_STREAM_STREAMID_NO     0x00000000        /**< No Stream Id */

#define SML_LOG_STREAM_BLOCKSIZE_MASK  0x00000002        /**< Block Size Attribute */
#define SML_LOG_STREAM_BLOCKSIZE_YES   0x00000002        /**< Block Size */
#define SML_LOG_STREAM_BLOCKSIZE_NO    0x00000000        /**< No Block Size */
/** @} */

/**
 * @brief invalid log group Id
 */
#define SML_INVALID_LOG_GROUP_ID      (-1)

typedef struct smlLogMemberAttr smlLogMemberAttr;
typedef struct smlLogGroupAttr smlLogGroupAttr;

struct smlLogMemberAttr
{
    stlUInt32          mValidFlags;
    stlChar            mName[STL_MAX_FILE_PATH_LENGTH];
    stlBool            mLogFileReuse;
    smlLogMemberAttr * mNext;
};

struct smlLogGroupAttr
{
    stlUInt32          mValidFlags;
    stlInt32           mLogGroupId;
    stlBool            mLogFileReuse;
    stlInt64           mFileSize;
    smlLogMemberAttr * mLogMemberAttr;
    smlLogGroupAttr  * mNext;
};

typedef struct smlLogStreamAttr
{
    stlUInt32         mValidFlags;
    stlInt16          mBlockSize;
    smlLogGroupAttr * mLogGroupAttr;
} smlLogStreamAttr;

/**
 * @defgroup smlLogFlusherState Log Flusher State
 * @{
 */
#define SML_LOG_FLUSHER_STATE_ACTIVE   (1)     /**< Log Flusher가 활동중인 상태 */
#define SML_LOG_FLUSHER_STATE_INACTIVE (2)     /**< Log Flusher가 멈춘상태 */
/** @} */

/**
 * @defgroup smlAgerState Ager State
 * @{
 */
#define SML_AGER_STATE_ACTIVE   (1)     /**< Ager가 활동중인 상태 */
#define SML_AGER_STATE_INACTIVE (2)     /**< Ager가 멈춘상태 */
/** @} */

/**
 * @defgroup smlTransactionWriteMode Transaction Commit Write Mode
 * @{
 */
#define SML_TRANSACTION_CWM_NONE      (-1)  /**< invalid */
#define SML_TRANSACTION_CWM_NO_WAIT   (0)   /**< no wait */
#define SML_TRANSACTION_CWM_WAIT      (1)   /**< wait */
/** @} */

/**
 * @defgroup smlCheckpointerState Checkpointer State
 * @{
 */
#define SML_CHECKPOINTER_STATE_ACTIVE   (1)     /**< Checkpointer가 활동중인 상태 */
#define SML_CHECKPOINTER_STATE_INACTIVE (2)     /**< Checkpointer가 멈춘상태 */
/** @} */

/**
 * @defgroup smlArchivelogMode Log Archive Mode
 * @{
 */
#define SML_NOARCHIVELOG_MODE     (0)  /**< noarchive mode */
#define SML_ARCHIVELOG_MODE       (1)  /**< archive mode */
/** @} */

/**
 * @defgroup smlCheckpointState Checkpoint 상태
 * @{
 */
#define SML_CHECKPOINT_STATE_IN_IDLE     (0)  /**< checkpoint가 유휴 상태 */
#define SML_CHECKPOINT_STATE_IN_PROGRESS (1)  /**< checkpoint가 진행중인 상태 */
/** @} */

/** @} */

/**
 * @addtogroup smlRecord
 * @{
 */

/**
 * @brief 레코드 아이디
 */
typedef struct smlRid
{
    smlTbsId  mTbsId;       /**< 테이블스페이스 아이디 */
    stlInt16  mOffset;      /**< 페이지내 옵셋 */
    smlPid    mPageId;      /**< 페이지 아이디 */
} smlRid;

#define SML_INVALID_RID_OFFSET (-1)

/**
 * @brief Invalid RID 설정 매크로
 */
#define SML_SET_INVALID_RID( aRid )             \
{                                               \
    (aRid)->mTbsId = 0;                         \
    (aRid)->mOffset = SML_INVALID_RID_OFFSET;   \
    (aRid)->mPageId = 0;                        \
}

/**
 * @brief RID 설정 매크로
 */
#define SML_MAKE_RID( aRid, aTbsId, aPageId, aOffset )  \
{                                                       \
    (aRid)->mTbsId = aTbsId;                            \
    (aRid)->mOffset = aOffset;                          \
    (aRid)->mPageId = aPageId;                          \
}

/**
 * @brief INVALID RID 정의
 */
#define SML_INVALID_RID ((smlRid){0,SML_INVALID_RID_OFFSET,0})    \
 


/**
 * @brief Rid가 INVALID한지 검사한다.
 */
#define SML_IS_INVALID_RID( aRid )                                          \
(                                                                           \
    ( ((aRid).mTbsId == 0) &&                                               \
      ((aRid).mPageId == 0) &&                                              \
      ((aRid).mOffset == SML_INVALID_RID_OFFSET) ) ? STL_TRUE : STL_FALSE   \
)

/**
 * @brief Invalid RID인지 체크
 */
#define SML_IS_EQUAL_RID(aRidA, aRidB)                             \
    (((aRidA).mTbsId == (aRidB).mTbsId) &&                         \
     ((aRidA).mOffset == (aRidB).mOffset) &&                       \
     ((aRidA).mPageId == (aRidB).mPageId) ? STL_TRUE : STL_FALSE)

typedef struct smlRowBlock
{
    stlInt32 mBlockSize;
    stlInt32 mUsedBlockSize;
    smlScn * mScnBlock;  /**< Value Block 정보 */
    smlRid * mRidBlock;  /**< Value Block 정보 */
} smlRowBlock;

/**
 * @brief Row Block의 배열 크기를 얻는다.
 * @param[in] aRowBlock Row Block 포인터
 */
#define SML_BLOCK_SIZE( aRowBlock )                     ( (aRowBlock)->mBlockSize )
/**
 * @brief Row Block의 사용된 배열의 크기를 얻는다.
 * @param[in] aRowBlock Row Block 포인터
 */
#define SML_USED_BLOCK_SIZE( aRowBlock )                ( (aRowBlock)->mUsedBlockSize )
/**
 * @brief Row Block의 사용한 크기를 증가시킨다.
 * @param[in] aRowBlock Row Block 포인터
 */
#define SML_INC_USED_BLOCK_SIZE( aRowBlock )            { (aRowBlock)->mUsedBlockSize++; }
/**
 * @brief Row Block의 사용한 크기를 설정한다.
 * @param[in] aRowBlock  Row Block 포인터
 * @param[in] aBlockSize 설정할 Block 크기
 */
#define SML_SET_USED_BLOCK_SIZE( aRowBlock, aBlockSize ) { (aRowBlock)->mUsedBlockSize = aBlockSize; }
/**
 * @brief Row Block의 사용한 크기를 재초기화한다.
 * @param[in] aRowBlock Row Block 포인터
 */
#define SML_RESET_USED_BLOCK( aRowBlock )               { (aRowBlock)->mUsedBlockSize = 0; }
/**
 * @brief Row Block의 RID 배열에서 aBlockIdx에 위치한 value를 얻는다
 * @param[in] aRowBlock Row Block 포인터
 * @param[in] aBlockIdx RID 배열상의 value 위치
 */
#define SML_GET_RID_VALUE( aRowBlock, aBlockIdx )       ( (aRowBlock)->mRidBlock[aBlockIdx] )
/**
 * @brief Row Block의 SCN 배열에서 aBlockIdx에 위치한 value를 얻는다
 * @param[in] aRowBlock Row Block 포인터
 * @param[in] aBlockIdx SCN 배열상의 value 위치
 */
#define SML_GET_SCN_VALUE( aRowBlock, aBlockIdx )       ( (aRowBlock)->mScnBlock[aBlockIdx] )
/**
 * @brief Row Block에 RID value를 설정한다.
 * @param[in] aRowBlock Row Block 포인터
 * @param[in] aBlockIdx RID 배열상의 value 위치
 * @param[in] aRid      추가할 RID Value
 */
#define SML_SET_RID_VALUE( aRowBlock, aBlockIdx, aRid ) { (aRowBlock)->mRidBlock[aBlockIdx] = aRid; }
/**
 * @brief Row Block에 SCN value를 설정한다.
 * @param[in] aRowBlock Row Block 포인터
 * @param[in] aBlockIdx SCN 배열상의 value 위치
 * @param[in] aScn      추가할 SCN Value
 */
#define SML_SET_SCN_VALUE( aRowBlock, aBlockIdx, aScn ) { (aRowBlock)->mScnBlock[aBlockIdx] = aScn; }

/**
 * @brief Value Block에 unique violation 여부를 체크한다.
 * @param[in] aValueBlock unique violation 여부를 기록할 value block
 * @param[in] aBlockIdx value block의 index
 * @param[in] aValue unique violation 여부 STL_TRUE or STL_FALSE
 */
#define SML_SET_UNIQUE_VIOLATION( aValueBlock, aBlockIdx, aValue )    \
    ( *(stlBool *)KNL_GET_BLOCK_DATA_PTR( aValueBlock, aBlockIdx ) = aValue )

/** @} */

/**
 * @addtogroup smlStatement
 * @{
 */

/**
 * @brief 트랜잭션내 순차 번호
 */
typedef stlUInt32 smlTcn;

/**
 * @brief 무한대 TCN
 */
#define SML_INVALID_TCN   (STL_UINT32_MAX)

/**
 * @brief 무한대 TCN
 */
#define SML_INFINITE_TCN  (STL_UINT32_MAX-1)

/**
 * @defgroup smlStatementAttr Attribute
 * @{
 */
#define SML_STMT_ATTR_UPDATABLE         0x00000001  /**< Updatable Statement */
#define SML_STMT_ATTR_READONLY          0x00000002  /**< ReadOnly Statement */
#define SML_STMT_ATTR_DDL_LOCK_TIMEOUT  0x00000004  /**< Lock Wait시에 DDL_LOCK_TIMEOUT을 사용 */
#define SML_STMT_ATTR_LOCKABLE          0x00000008  /**< Lockable Statement */
#define SML_STMT_ATTR_HOLDABLE          0x00000010  /**< Lockable Statement */

#define SML_STMT_STATE_ACTIVE       (1)
#define SML_STMT_STATE_COMMITTED    (2)

/** @} */

/**
 * @brief Savepoint on Lock
 */
typedef struct smlLockSavepoint
{
    void * mLockNode;
    void * mRelation;
} smlLockSavepoint;

/**
 * @brief Statement 구조체
 */
struct smlStatement
{
    stlInt64          mTransId;              /**< SMXL Statement Transaction ID */
    smlTransId        mSmlTransId;           /**< SML Statement Transaction ID */
    smlScn            mCommitScn;            /**< Transaction Commit Scn */
    smlScn            mTransViewScn;         /**< Transaction View Scn */
    smlScn            mMinTransViewScn;      /**< Mininum Transaction View Scn */
    void            * mUpdateRelHandle;      /**< Updatable Target Relation Handle */
    smlScn            mScn;                  /**< Statement System Change Number */
    stlTime           mBeginTime;            /**< Statement begin time */
    smlTcn            mTcn;                  /**< Statement Transaction Change Number */
    stlInt32          mAttr;                 /**< Statement Attribute @see smlStatementAttr */
    stlInt32          mState;
    stlInt64          mLockTimeout;          /**< Lock Timeout */
    smlIsolationLevel mIsolationLevel;
    stlInt32          mResetCount;           /**< statement reset count */
    smlRid            mUndoRid;              /**< Statement Rollback을 위한 Undo Record Identifier */
    stlInt64          mTimestamp;            /**< Statement가 생성한 시점의 순차 번호 */
    smlLockSavepoint  mLockSavepoint;        /**< 락 리스트상의 Savepoint 지점 */
    stlBool           mNeedSnapshotIterator; /**< Snapshot Iterator의 필요여부 */
    stlRingEntry      mStmtLink;             /**< Statement Link */
    stlRingHead       mIterators;            /**< Updatable Atomic Statement Execution Context */
    knlRegionMem      mStatementMem;         /**< region memory for statement */
    stlInt64          mActiveInstantCount;   /**< 해당 statement에서 생성한 Active Instant 개수 */
    stlBool           mIsUsing;
    stlBool           mIsCached;
};

/** @} */

/**
 * @addtogroup smlIterator
 * @{
 */

/**
 * @brief Iterator의 Transaction Read Mode
 * @remarks Iterator가 Transaction Level에서 읽어야할 대상을 선정하기 위한 모드
 * @par examples
 *  - SML_TRM_SNAPSHOT을 사용해야 하는 경우:
 *    DML, SELECT, ODBC SENSITIVE CURSOR
 *  - SML_TRM_COMMITTED를 사용해야 하는 경우:
 *    DDL
 */
typedef enum
{
    SML_TRM_SNAPSHOT,   /**< Older Transaction이 변경한 내용만 읽는다. */
    SML_TRM_COMMITTED   /**< Older/Elder Transaction이 변경한 내용을 읽는다. */
} smlTransReadMode;

/**
 * @brief Iterator의 Statement Read Mode
 * @remarks Iterator가 Statement Level에서 읽어야할 대상을 선정하기 위한 모드
 * @par examples
 *  - SML_SRM_SNAPSHOT을 사용해야 하는 경우:
 *    DDL을 제외한 모든 SQL
 *  - SML_SRM_RECENT를 사용해야 하는 경우:
 *    DDL
 */
typedef enum
{
    SML_SRM_SNAPSHOT,  /**< Older Statement에서 변경한 내용만 읽는다. */
    SML_SRM_RECENT     /**< Older와 My Statement에서 변경한 내용을 읽는다. */
} smlStmtReadMode;

typedef enum
{
    SML_SCAN_FORWARD,
    SML_SCAN_BACKWARD
} smlIteratorScanDirection;

typedef struct smlIteratorProperty
{
    stlUInt64  mSkipCount;
    stlUInt64  mReturnCount;
    stlChar  * mDumpOption;     /* Dump Table Iterator를 위한 Dump Option */
    stlBool    mGroupFetch;     /* group iterator를 할당하기 위해서는 true를 주면 된다. */
    stlBool    mSkipHitRecord;  /* 한번 hit된 record의 fetch를 skip할지 여부 */
} smlIteratorProperty;

#define SML_INIT_ITERATOR_PROP( aProp )         \
    (aProp).mSkipCount = 0;                     \
    (aProp).mReturnCount = STL_UINT64_MAX;      \
    (aProp).mDumpOption = NULL;                 \
    (aProp).mSkipHitRecord = STL_FALSE;         \
    (aProp).mGroupFetch = STL_FALSE

#define SML_INIT_ITERATOR_PROP_PTR( aProp )     \
    (aProp)->mSkipCount = 0;                    \
    (aProp)->mReturnCount = STL_UINT64_MAX;     \
    (aProp)->mDumpOption = NULL;                \
    (aProp)->mSkipHitRecord = STL_FALSE;        \
    (aProp)->mGroupFetch = STL_FALSE

#define SML_SET_ITERATOR_PROP( aProp, aSkipCount, aReturnCount, aDumpOpt, aGroupFetch, aSkipHitRecord ) \
    (aProp).mSkipCount = (aSkipCount);                                                              \
    (aProp).mReturnCount = (aReturnCount);                                                          \
    (aProp).mDumpOption = (aDumpOpt);                                                               \
    (aProp).mSkipHitRecord = (aSkipHitRecord);                                                      \
    (aProp).mGroupFetch = (aGroupFetch)


/**
 * @brief Fetch Record를 수행하기 위한 정보 : Module->mFetch를 위한 구조 
 */
typedef struct smlFetchRecordInfo
{
    void               * mRelationHandle;        /**< Table Handle (IN) */
    knlValueBlockList  * mColList;               /**< Read Column List (OUT) */
    knlValueBlockList  * mRowIdColList;          /**< RowID Column List (OUT) */
    knlPhysicalFilter  * mPhysicalFilter;        /**< Physical Filter (IN) */
    stlInt64             mTableLogicalId;        /**< RowID column value구성시 사용됨. (IN) */

    smlTransReadMode     mTransReadMode;         /**< Transaction Read Mode (IN) */
    smlStmtReadMode      mStmtReadMode;          /**< Statement Read Mode (IN) */
    smlScn             * mScnBlock;              /**< Scn Block List : array (IN/OUT) */
    knlExprStack       * mLogicalFilter;         /**< Logical Filter Stack (IN) */
    knlExprEvalInfo    * mLogicalFilterEvalInfo; /**< Logical Filter 수행 정보 (IN) */

    /* storage layer 에서 설정하는 부분 */
    smlScn               mViewScn;               /**< View Scn : Fetch Record 수행시 결정됨 (OUT) */
    smlTcn               mViewTcn;               /**< View Tcn : Fetch Record 수행시 결정됨 (OUT) */
} smlFetchRecordInfo;


typedef struct smlAggrFuncInfo   smlAggrFuncInfo;

typedef struct smlBlockJoinFetchInfo
{
    knlBlockJoinType       mBlockJoinType;        /**< Block Join */
    stlBool                mNullAwareComparison;  /**< Null aware comparison flag */
    
    /*
     * for join key
     */
    
    knlValueBlockList    * mKeyBlockList;          /**< Key 를 구성할 Block List */
    knlDataValueList     * mKeyConstList;          /**< Key 상수 List */

    /*
     * for join iteration
     */
    
    knlBlockRelationList * mResultRelationList;    /**< Result Relation List */
    
    knlValueBlockList    * mResultBlockList;       /**< Result Value Block List  */

    stlInt32               mResultBlockIdx;        /**< Result Block Idx */
    stlInt32               mLeftBlockIdx;          /**< Left Block Idx */
    
    stlInt32             * mResultRefLeftBlockIdx; /**< Left Block Idx of Result Refer for SENSITIVE, FOR UPDATE */

    stlBool                mIsEndOfJoin;           /**< End Of Join */

    /*
     * for join filter
     */
    
    stlBool               mReady4JoinFilter;       /**< Constant Value 가 이미 설정되었는지의 여부 */

    knlValueBlockList   * mLeftBlock4JoinFilter;   /**< Left Value Block List  */
    knlDataValueList    * mLeftConst4JoinFilter;   /**< Constant Value List for Join Filter */
 
    knlValueBlockList   * mRightBlock4JoinFilter;  /**< Right Value Block List  */
    knlDataValueList    * mRightConst4JoinFilter;  /**< Constant Value List for Join Filter */

    knlExprEvalInfo     * mPreHashEvalInfo;        /**< Prepare Hash-Key의 Evaluate Info */
    knlExprEvalInfo     * mPreJoinEvalInfo;        /**< Prepare Filter의 Evaluate Info */
    knlExprEvalInfo     * mOuterJoinFilter;        /**< Outer Join Filter */
    knlExprEvalInfo     * mResultFilter;           /**< Result 에 대한 filter(Inner Join Filter) */
    
} smlBlockJoinFetchInfo;

/**
 * @brief Fetch 를 수행하기 위한 정보 : Module->mFetchNext & Module->mFetchPrev 를 위한 구조
 */
typedef struct smlFetchInfo
{
    knlRange            * mRange;            /**< Range Conditions (IN) */
    knlPhysicalFilter   * mPhysicalFilter;   /**< Physical Filter (IN) */
    knlKeyCompareList   * mKeyCompList;      /**< Key Compare List (IN) */
    knlExprStack        * mLogicalFilter;    /**< Logical Filter (IN) */

    knlValueBlockList   * mColList;          /**< Read Column List (OUT) */
    knlValueBlockList   * mRowIdColList;     /**< RowID Column List (OUT) */

    smlRowBlock         * mRowBlock;         /**< Row Block (IN/OUT) */

    stlInt64              mTableLogicalId;   /**< RowID column의 value 구성시 사용됨. (IN) */
    
    stlInt64              mSkipCnt;          /**< Skip Count (IN/OUT) */
    stlInt64              mFetchCnt;         /**< Fetch Count (IN/OUT) */
    stlBool               mGroupKeyFetch;    /**< Group Key의 패치 여부 (IN) */

    stlInt8               mRowFlag;          /**< 특정 flag의 on/off 인 row만 읽을 경우 사용 (IN)
                                                  SML_INSTANT_TABLE_ROW_FLAG_XXX 값을 가짐 */
    stlBool               mIsEndOfScan;      /**< EOF (OUT) */

    knlExprEvalInfo     * mFilterEvalInfo;   /**< Filter 수행 정보 (IN/OUT) */
    smlFetchRecordInfo  * mFetchRecordInfo;  /**< Fetch Record Info (IN/OUT) */

    smlAggrFuncInfo        * mAggrFetchInfo;       /**< Aggregation 수행 정보 (IN/OUT) */
    smlBlockJoinFetchInfo  * mBlockJoinFetchInfo;  /**< Block Join Fetch Information */
} smlFetchInfo;


/**
 * @brief Merge Record를 수행하기 위한 정보 : Module->??? 를 위한 구조 
 */
typedef struct smlMergeRecordInfo
{
    knlValueBlockList  * mReadColList;           /**< Read Column List */
    knlValueBlockList  * mUpdateColList;         /**< Update Column List */

    smlAggrFuncInfo    * mAggrFuncInfoList;      /**< Aggregation 수행 정보 리스트 */
} smlMergeRecordInfo;


/**
 * @brief Sort Instant Table에 대한 Table Attribute
 */
typedef struct smlSortTableAttr
{
    stlBool            mTopDown;   /**< top-down으로 빌드할 것인가? true이면 mVolatile, mOnlyLeaf는 무시 */
    stlBool            mVolatile;  /**< volatile 모드이면 중간 merge를 하지 않음. scroll 불가. true이면 mOnlyLeaf는 무시 */
    stlBool            mLeafOnly;  /**< index에 internal 없이 leaf만 만들것인가? true면 search 불가 */
    stlBool            mDistinct;
    stlBool            mNullExcluded; /**< 키에 NULL이 있는 레코드는 넣지 않을 것인가를 결정 */
} smlSortTableAttr;


/**
 * @brief Fetch Prev 및 Fetch Next를 위한 최대 Fetch Count
 */
#define SML_FETCH_COUNT_MAX  ( STL_INT64_MAX )


/**
 * @brief Table Scan Iterator를 위한 Fetch Info 설정
 */
#define SML_SET_FETCH_INFO_FOR_TABLE( aFetchInfo,           \
                                      aPhysicalFilter,      \
                                      aLogicalFilter,       \
                                      aReadColumnList,      \
                                      aRowIDColumnList,     \
                                      aRowBlock,            \
                                      aSkipCnt,             \
                                      aFetchCnt,            \
                                      aGroupKeyFetch,       \
                                      aFilterEvalInfo )     \
    {                                                       \
        (aFetchInfo)->mRange = NULL;                        \
        (aFetchInfo)->mPhysicalFilter = (aPhysicalFilter);  \
        (aFetchInfo)->mKeyCompList = NULL;                  \
        (aFetchInfo)->mLogicalFilter = (aLogicalFilter);    \
        (aFetchInfo)->mColList = (aReadColumnList);         \
        (aFetchInfo)->mRowIdColList = (aRowIDColumnList);   \
        (aFetchInfo)->mRowBlock = (aRowBlock);              \
        (aFetchInfo)->mSkipCnt = (aSkipCnt);                \
        (aFetchInfo)->mFetchCnt = (aFetchCnt);              \
        (aFetchInfo)->mGroupKeyFetch = (aGroupKeyFetch);    \
        (aFetchInfo)->mRowFlag = SML_INSTANT_TABLE_ROW_FLAG_DONTCARE;          \
        (aFetchInfo)->mIsEndOfScan = STL_FALSE;             \
        (aFetchInfo)->mFilterEvalInfo = (aFilterEvalInfo);  \
        (aFetchInfo)->mFetchRecordInfo = NULL;              \
        (aFetchInfo)->mAggrFetchInfo = NULL;                \
        (aFetchInfo)->mBlockJoinFetchInfo = NULL;           \
    }


/**
 * @brief Index Scan Iterator를 위한 Fetch Info 설정
 */
#define SML_SET_FETCH_INFO_FOR_INDEX( aFetchInfo,               \
                                      aRange,                   \
                                      aPhysicalFilter,          \
                                      aKeyCompList,             \
                                      aLogicalFilter,           \
                                      aReadColumnList,          \
                                      aRowIDColumnList,         \
                                      aRowBlock,                \
                                      aSkipCnt,                 \
                                      aFetchCnt,                \
                                      aGroupKeyFetch,           \
                                      aFilterEvalInfo,          \
                                      aFetchRecordInfo )        \
    {                                                           \
        (aFetchInfo)->mRange  = (aRange);                       \
        (aFetchInfo)->mPhysicalFilter = (aPhysicalFilter);      \
        (aFetchInfo)->mKeyCompList = (aKeyCompList);            \
        (aFetchInfo)->mLogicalFilter = (aLogicalFilter);        \
        (aFetchInfo)->mColList = (aReadColumnList);             \
        (aFetchInfo)->mRowIdColList = (aRowIDColumnList);       \
        (aFetchInfo)->mRowBlock = (aRowBlock);                  \
        (aFetchInfo)->mSkipCnt = (aSkipCnt);                    \
        (aFetchInfo)->mFetchCnt = (aFetchCnt);                  \
        (aFetchInfo)->mGroupKeyFetch = (aGroupKeyFetch);        \
        (aFetchInfo)->mRowFlag = SML_INSTANT_TABLE_ROW_FLAG_DONTCARE;          \
        (aFetchInfo)->mIsEndOfScan = STL_FALSE;                 \
        (aFetchInfo)->mFilterEvalInfo = (aFilterEvalInfo);      \
        (aFetchInfo)->mFetchRecordInfo = (aFetchRecordInfo);    \
        (aFetchInfo)->mAggrFetchInfo = NULL;                    \
        (aFetchInfo)->mBlockJoinFetchInfo = NULL;               \
    }


/** @} */


/**
 * @addtogroup smlStatistics
 * @{
 */

/** @} */


/**
 * @addtogroup smlAggr
 * @{
 */

typedef stlStatus (*smlAggrAssignFuncPtr) ( smlAggrFuncInfo   * aAggrFuncInfo,
                                            stlInt32            aBlockIdx,
                                            smlEnv            * aEnv );

typedef stlStatus (*smlAggrBuildFuncPtr) ( smlAggrFuncInfo   * aAggrFuncInfo,
                                           stlInt32            aBlockIdx,
                                           smlEnv            * aEnv );

typedef stlStatus (*smlAggrBlockBuildFuncPtr) ( smlAggrFuncInfo * aAggrFuncInfo,
                                                stlInt32          aStartBlockIdx,
                                                stlInt32          aBlockCnt,
                                                smlEnv          * aEnv );

/**
 * @brief aggregation function에 대한 정보 관리하는 구조체
 */
typedef struct smlAggrFuncModule
{
    knlBuiltInAggrFunc          mFuncID;              /**< Built-In Aggregation Function ID */

    smlAggrAssignFuncPtr        mAssignFuncPtr;       /**< Assign Function Pointer */
    smlAggrBuildFuncPtr         mBuildFuncPtr;        /**< Build Function Pointer */
    smlAggrBlockBuildFuncPtr    mBlockBuildFuncPtr;   /**< Block Build Function Pointer */
} smlAggrFuncModule;

extern smlAggrFuncModule gSmlAggrFuncInfoArr[ KNL_BUILTIN_AGGREGATION_UNARY_MAX ];

typedef stlStatus (*smlMergeRecordAggrFunc) (void               * aTableHeader,
                                             stlChar            * aRow,
                                             smlAggrFuncInfo    * aAggrFuncInfo,
                                             stlInt32             aStartIdx,
                                             stlInt32             aEndIdx,
                                             knlValueBlockList  * aReadBlock,
                                             knlValueBlockList  * aUpdateBlock,
                                             smlEnv             * aEnv );

extern smlMergeRecordAggrFunc gSmlAggrMergeFuncInfoArr[KNL_BUILTIN_AGGREGATION_UNARY_MAX][2];

/**
 * @brief Aggregatioin Function 정보
 * @remark Distinct option이 적용된 Aggregatioin Function은 SQL layer에서 처리한다.
 */
struct smlAggrFuncInfo
{
    smlAggrFuncModule       * mAggrFuncModule;
    knlExprStack            * mInputArgStack;    /**< Aggregation 수행 대상에 대한 Expression Stack */
    knlExprEvalInfo        ** mEvalInfo;         /**< Aggregation 수행 대상에 대한 Expression 평가 정보 */
    knlValueBlockList       * mInputArgList;     /**< Aggregation 수행 대상 Value Block */
    knlValueBlockList       * mAggrValueList;    /**< Aggregation 수행 중간 결과가 저장될 Value Block */
    smlAggrFuncInfo         * mNext;
    smlMergeRecordAggrFunc  * mAggrMergeFunc;
};


/**
 * @brief 인스턴트 테이블의 row에 대해 flag 설정을 위한 컨트롤 값들
 */
#define SML_INSTANT_TABLE_ROW_FLAG_SET       1
#define SML_INSTANT_TABLE_ROW_FLAG_DONTCARE  0
#define SML_INSTANT_TABLE_ROW_FLAG_UNSET    -1

/** @} */

/**
 * @addtogroup smlDump
 * @{
 */

/**
 * @brief gdump INVALID NUMBER
 */ 
#define SML_DUMP_INVALID_VALUE        -1
/**
 * @brief gdump INIT NUMBER
 */ 
#define SML_DUMP_INIT_VALUE           0

/**
 * @brief gdump control file section system section 
 */ 
#define SML_DUMP_CTRLFILE_SECTION_SYS     0x01
/**
 * @brief gdump control file section log section 
 */ 
#define SML_DUMP_CTRLFILE_SECTION_LOG     0x02
/**
 * @brief gdump control file section db section 
 */ 
#define SML_DUMP_CTRLFILE_SECTION_DB      0x04
/**
 * @brief gdump control file section incremental backup section 
 */ 
#define SML_DUMP_CTRLFILE_SECTION_BACKUP  0x08
/**
 * @brief gdump control file section all section 
 */ 
#define SML_DUMP_CTRLFILE_SECTION_ALL     0x0F

/**
 * @brief gdump pch of page 
 */ 
#define SML_DUMP_PAGE_HEADER              0x01
/**
 * @brief gdump pch and frame
 */ 
#define SML_DUMP_PAGE_ALL                 0x02

/**
 * @brief max buffer size using gdump
 */ 
#define SML_DUMP_MAX_BUFFER_SIZE          (32 * 1024 * 1024)

/** @} */

/** @} */

#endif /* _SMLDEF_H_ */
