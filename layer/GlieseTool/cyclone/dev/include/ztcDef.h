/*******************************************************************************
 * ztcDef.h
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


#ifndef _ZTCDEF_H_
#define _ZTCDEF_H_ 1

/**
 * @file ztcDef.h
 * @brief Cyclone Definitions
 */

/**
 * @defgroup ztcError Error
 * @{
 */

/**
 * @brief error
 */

#define ZTC_ERRCODE_SERVICE_NOT_AVAILABLE        STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 0 )
#define ZTC_ERRCODE_INVALID_FILE_NAME            STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 1 )
#define ZTC_ERRCODE_CONFIGURE_FILE_PARSE         STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 2 )
#define ZTC_ERRCODE_INVALID_TABLE                STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 3 )
#define ZTC_ERRCODE_INVALID_SCHEMA               STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 4 )
#define ZTC_ERRCODE_DUPLICATE_OBJECT             STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 5 )
#define ZTC_ERRCODE_INVALID_PRIMARY_KEY          STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 6 )
#define ZTC_ERRCODE_INTERNAL_ERROR               STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 7 )
#define ZTC_ERRCODE_INVALID_DATA_TYPE            STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 8 )
#define ZTC_ERRCODE_SUPPLEMENTAL_LOG             STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 9 )
#define ZTC_ERRCODE_ALREADY_RUNNING              STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 10 )
#define ZTC_ERRCODE_NOT_RUNNING                  STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 11 )
#define ZTC_ERRCODE_INVALID_HOME                 STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 12 )
#define ZTC_ERRCODE_LOG_FILE_REUSED              STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 13 )
#define ZTC_ERRCODE_FAIL_ANALYZE_LOG             STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 14 )
#define ZTC_ERRCODE_INVALID_PORT                 STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 15 )
#define ZTC_ERRCODE_COMMUNICATION_LINK_FAILURE   STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 16 )
#define ZTC_ERRCODE_INVALID_COM_DATA             STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 17 )
#define ZTC_ERRCODE_MASTER_DISCONNT              STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 18 )
#define ZTC_ERRCODE_PROTOCOL                     STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 19 )
#define ZTC_ERRCODE_ALREADY_SLAVE_CONN           STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 20 )
#define ZTC_ERRCODE_INVALID_SLOT_ID              STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 21 )
#define ZTC_ERRCODE_INVALID_SUPPLEMENT_LOG       STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 22 )
#define ZTC_ERRCODE_INVALID_DDL_OPERATION        STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 23 )
#define ZTC_ERRCODE_INVALID_ANALYZE_VALUE        STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 24 )
#define ZTC_ERRCODE_INVALID_GROUP_NAME           STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 25 )
#define ZTC_ERRCODE_INVALID_MASTER_IP            STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 26 )
#define ZTC_ERRCODE_INVALID_CAPTURE_INFO         STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 27 )
#define ZTC_ERRCODE_INVALID_GROUP_COUNT          STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 28 )
#define ZTC_ERRCODE_DUPLICATE_GROUP_NAME         STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 29 )
#define ZTC_ERRCODE_INVALID_VALUE                STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 30 )
#define ZTC_ERRCODE_TOO_LOW_VALUE                STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 31 )
#define ZTC_ERRCODE_INVALID_HOST_PORT            STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 32 )
#define ZTC_ERRCODE_REDOLOG_READ_TIMEOUT         STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 33 )
#define ZTC_ERRCODE_INVALID_ARCHIVELOG           STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 34 )
#define ZTC_ERRCODE_INVALID_TEMP_FILE            STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 35 )
#define ZTC_ERRCODE_FAIL_WRITE_FILE              STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 36 )
#define ZTC_ERRCODE_FAIL_UPDATE_STATE            STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 37 )
#define ZTC_ERRCODE_FAIL_PROPAGATE_MODE          STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 38 )
#define ZTC_ERRCODE_NO_ACTIVE_LOG_FILE           STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 39 )
#define ZTC_ERRCODE_NO_RESTART_INFO              STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 40 )
#define ZTC_ERRCODE_FAIL_ADD_TABLE               STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 41 )
#define ZTC_ERRCODE_MUST_CONNECT_DB              STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 42 )
#define ZTC_ERRCODE_INVALID_META_FILE            STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 43 )
#define ZTC_ERRCODE_MUST_EXIST_LOG_FILE          STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 44 )
#define ZTC_ERRCODE_INVALID_CONN_INFO            STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 45 )
#define ZTC_ERRCODE_SLAVE_NOT_SYNC_MODE          STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 46 )
#define ZTC_ERRCODE_INVALID_SLAVE_CONN_INFO      STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE, 47 )

#define ZTC_MAX_ERROR     48

/** @} */

/**
 * @defgroup ztcExternal Cyclone Routines
 * @{
 */

/**
 * @addtogroup ztcDef
 * @{
 */

#define ZTC_DEFAULT_MASTER_PID_FILE          ".cymst.pid"
#define ZTC_DEFAULT_SLAVE_PID_FILE           ".cyslv.pid"
#define ZTC_DEFAULT_MASTER_CONFIGURE_FILE    "cyclone.master.conf"
#define ZTC_DEFAULT_SLAVE_CONFIGURE_FILE     "cyclone.slave.conf"
#define ZTC_DEFAULT_SCHEMANAME               "PUBLIC"
#define ZTC_DATA_SOURCE_IDENTIFIER           "[" STL_PRODUCT_NAME "]"
#define ZTC_DEFAULT_MASTER_TABLE_META_FILE   "cyclone.tablemeta"
#define ZTC_DEFAULT_MASTER_META_FILE         "cyclone.meta"
#define ZTC_ENV_LISTEN_PORT                  "GOLDILOCKS_LISTEN_PORT"

#define ZTC_LOG_FILE_PREFIX                  "lmLog_"

#define ZTC_DEFAULT_PORT                 ( 21102 )

#define ZTC_COMM_HEARTBEAT_TIMEOUT       ( 7 )

#define ZTC_SIGNAL_DEFINE                STL_SIGNAL_USR2        //ZL에서 INITIALIZE하기 때문에 QUIT을 사용할 수가 없다.

#define ZTC_TABLE_HASH_BUCKET_SIZE       ( 71 )

#define ZTC_INVALID_LSN                  STL_INT64_C( 0xFFFFFFFFFFFFFFFF )
#define ZTC_TRANS_SLOT_MASK              STL_INT64_C( 0x000000000000FFFF )
#define ZTC_INVALID_TRANSID              STL_INT64_C( 0xFFFFFFFFFFFFFFFF )
#define ZTC_INVALID_SCN                  ( STL_UINT64_MAX )     //SML_INFINITE_SCN
#define ZTC_GIVEUP_SCN                   ( ZTC_INVALID_SCN - 1) //GiveUp Table의 CommitSCN에 사용됨 (Capture되더라도 전송되지 않음)

#define ZTC_TCP_SOCKET_BUFF_SIZE         ( 1024 * 1024 * 16 )
#define ZTC_SENDER_BUFFER_SIZE           ( 1024 * 1024 * 16 )
#define ZTC_RECEIVER_BUFFER_SIZE         ZTC_SENDER_BUFFER_SIZE

#define ZTC_INVALID_TABLE_LOGICAL_ID     ( -1 )

#define ZTC_INVALID_TABLE_PHYSICAL_ID    ( 0 )

#define ZTC_INVALID_APPLIER_ID           ( -1 )

#define ZTC_INVALID_FILE_SEQ_NO          ( -1 )

#define ZTC_INVALID_FILE_GROUP_ID        ( -1 )

#define ZTC_INVALID_SLOT_IDX             ( -1 )

#define ZTC_MEM_ALIGN                    ( 8 )

#define ZTC_DIRECT_IO_ALIGN              ( 512 )

#define ZTC_COMMAND_BUFFER_SIZE          ( 1024 * 8 )

#define ZTC_LOGBODY_MAX_SIZE             ( 8 * 1024 * 3 * 32 )      //SMR_MAX_LOGBODY_SIZE

#define ZTC_LOG_PIECE_MAX_SIZE           ( 1024 * 8 * 3 )               //LogPiece의 최대크기는 8K * 3

#define ZTC_CHUNK_ITEM_SIZE              ZTC_SENDER_BUFFER_SIZE     /** Chunk는 Network I/O 단위이므로 SENDER_BUFFER_SIZE와 동일하게 한다 */

#define ZTC_TRANS_CHUNK_ITEM_SIZE        ( 1024 * 32 )

#define ZTC_DEFAULT_BLOCK_SIZE           ( 512 )

#define ZTC_SKIP_COMMIT_MAX_SIZE         ( 100 )                    /** 해당 값이후만큼 COMMIT이 발생하면 Blank Commit을 Slave에 보내준다. */

#define ZTC_TRACE_BUFFER_SIZE            ( 1024 * 2 )

#define ZTC_TRANS_READ_LOG_COUNT         ( 2000 )

#define ZTC_APPLIER_TRANS_COMMIT_MAX_SIZE ( 2000 )

#define ZTC_APPLIER_ARRAY_SIZE           ( 1 )

#define ZTC_READ_BUFFER_BLOCK_COUNT      ( 1024 * 2 * 20 )                      //READ BYTE == BLOCK_COUNT * 512 Byte
#define ZTC_READ_BUFFER_BLOCK_MIN_COUNT  ( 1024 * 2 )                           //1Mbytes

#define ZTC_TRANSACTION_SORT_AREA_SIZE   STL_INT64_C( 1024 * 1024 * 500 )

#define ZTC_TRANSACTION_TABLE_SIZE        ( 1024 )

#define ZTC_CHUNK_ITEM_COUNT             ( 32 )

#define ZTC_APPLIER_CHUNK_ITEM_SIZE      ( 1024 * 256 )

#define ZTC_APPLIER_CHUNK_ITEM_COUNT     ( 512 )

#define ZTC_REGION_INIT_SIZE             ( 1024 * 8 )

#define ZTC_REGION_NEXT_SIZE             ( 1024 * 8 )

#define ZTC_TRANS_SLOT_ID( aTransId )    ( (stlInt16)(aTransId & ZTC_TRANS_SLOT_MASK) )

#define ZTC_APPLIER_SESSION_COUNT        ( 6 )

#define ZTC_SPINLOCK_STATUS_INIT         ( 0 )
#define ZTC_SPINLOCK_STATUS_LOCKED       ( 1 )

#define ZTC_SUBDIST_USED_FALSE           ( 0 )
#define ZTC_SUBDIST_USED_TRUE            ( 1 )

#define ZTC_LONG_VARYING_MEMORY_INIT_SIZE ( 32 * 1024 )         //ZTC_LOG_PIECE_MAX_SIZE 보다 커야 한다.
#define ZTC_LONG_VARYING_MEMORY_NEXT_SIZE ZTC_LONG_VARYING_MEMORY_INIT_SIZE

#define ZTC_DISTRIBUTOR_SLOT_SIZE        STL_UINT64_C( 960897 )   //19537, 505709, 611953, 960897, 2145351

#define ZTC_DEFAULT_SYNCHER_COUNT         ( 4 )
#define ZTC_DEFAULT_SYNC_ARRAY_SIZE       ( 1000 )
#define ZTC_DEFAULT_SQLSTATE_LENGTH       ( 6 )  
#define ZTC_SQLSTATE_DATA_RIGHT_TRUNCATED ( "01004" )
#define ZTC_SYNC_LOG_MSG_FREQUENCY        ( 1000000 )

#define ZTC_INIT_SPIN_LOCK( aSpinLock )     \
{                                           \
    aSpinLock = ZTC_SPINLOCK_STATUS_INIT;   \
}

#define ZTC_GET_DISTRIBUTE_SLOT_IDX( aRecordId, aSlotIdx )             \
{                                                                      \
    stlInt32 tmpSlotIdx;                                               \
    tmpSlotIdx  = aRecordId % ZTC_DISTRIBUTOR_SLOT_SIZE;               \
    *(aSlotIdx) = tmpSlotIdx;                                          \
}

#define ZTC_READ_COLUMN_AND_MOVE_PTR( aColPtr, aValue, aValueLen )              \
{                                                                               \
    stlUInt8 tmpColLenSize;                                                     \
    stlBool  tmpIsZero;                                                         \
                                                                                \
    ZTC_GET_COLUMN_LEN( *(aColPtr), &tmpColLenSize, (aValueLen), tmpIsZero );   \
                                                                                \
    if( tmpIsZero == STL_TRUE )                                                 \
    {                                                                           \
        DTL_NUMERIC_SET_ZERO( (aValue), *(aValueLen) );                         \
        *(aColPtr) += 1;                                                        \
    }                                                                           \
    else                                                                        \
    {                                                                           \
        *(aColPtr) += tmpColLenSize;                                            \
        stlMemcpy( (aValue), *(aColPtr), *(aValueLen) );                        \
        *(aColPtr) += *(aValueLen);                                             \
    }                                                                           \
}

#define ZTC_SKIP_COLUMN_AND_MOVE_PTR( aColPtr )                                 \
{                                                                               \
    stlUInt8 tmpColLenSize;                                                     \
    stlInt64 tmpValueLen;                                                       \
    stlBool  tmpIsZero;                                                         \
                                                                                \
    ZTC_GET_COLUMN_LEN( *(aColPtr), &tmpColLenSize, &tmpValueLen, tmpIsZero );  \
                                                                                \
    if( tmpIsZero == STL_TRUE )                                                 \
    {                                                                           \
        *(aColPtr) += 1;                                                        \
    }                                                                           \
    else                                                                        \
    {                                                                           \
        *(aColPtr) += tmpColLenSize;                                            \
        *(aColPtr) += tmpValueLen;                                              \
    }                                                                           \
}

#define ZTC_READ_COLUMN_VALUE_LEN( aColPtr, aValueLen )                         \
{                                                                               \
    stlUInt8 tmpColLenSize;                                                     \
    stlBool  tmpIsZero;                                                         \
                                                                                \
    ZTC_GET_COLUMN_LEN( *(aColPtr), &tmpColLenSize, (aValueLen), tmpIsZero );   \
                                                                                \
    if( tmpIsZero == STL_TRUE )                                                 \
    {                                                                           \
        *(aValueLen) = 0;                                                       \
    }                                                                           \
}

#define ZTC_READ_COLUMN_VALUE_LEN_AND_MOVE_PTR( aColPtr, aValueLen )            \
{                                                                               \
    stlUInt8 tmpColLenSize;                                                     \
    stlBool  tmpIsZero;                                                         \
                                                                                \
    ZTC_GET_COLUMN_LEN( *(aColPtr), &tmpColLenSize, (aValueLen), tmpIsZero );   \
                                                                                \
    if( tmpIsZero == STL_TRUE )                                                 \
    {                                                                           \
        *(aColPtr) += 1;                                                        \
        *(aValueLen) = 0;                                                       \
    }                                                                           \
    else                                                                        \
    {                                                                           \
        *(aColPtr) += tmpColLenSize;                                            \
    }                                                                           \
}

#define ZTC_SET_INDICATOR( aIndicator, aValue ) \
    {                                           \
        if( (aValue) == 0 )                     \
        {                                       \
            (aIndicator) = SQL_NULL_DATA;       \
        }                                       \
        else                                    \
        {                                       \
            (aIndicator) = (aValue);            \
        }                                       \
    }

#define ZTC_WAIT_SYNC_MANAGER_TURN( aRunState, aIsTurnOff )     \
    {                                                           \
        while( STL_TRUE )                                       \
        {                                                       \
            STL_TRY( (aRunState) == STL_TRUE );                 \
                                                                \
            if( (aIsTurnOff) == STL_FALSE )                     \
            {                                                   \
                break;                                          \
            }                                                   \
                                                                \
            stlSleep( STL_SET_MSEC_TIME( 1 ) );                 \
        }                                                       \
    }

/** @} */

/**
 * @addtogroup ztcStructure
 * @{
 */


/**
 * @brief Cyclone Environment
 */

typedef stlUInt32  ztcSpinLock;

typedef struct ztcChunkItem
{
    stlRingEntry   mLink;
    stlInt64       mWriteIdx;
    stlInt64       mReadIdx;
    stlChar      * mBuffer;
} ztcChunkItem;


typedef struct ztcConfigureParam
{
    void           * mYyScanner;
    stlErrorStack  * mErrorStack;
    stlChar        * mBuffer;
    stlInt32         mPosition;
    stlInt32         mLength;
    stlChar          mErrMsg[1024];
    stlStatus        mErrStatus;
} ztcConfigureParam;


typedef struct ztcGroupItem
{
    stlRingEntry   mLink;
    stlChar        mName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar        mUserId[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar        mUserPw[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar        mDsn[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar        mHostIp[16];
    stlInt32       mHostPort;
    stlChar        mPadding[4];
} ztcGroupItem;


typedef struct ztcConfigure
{
    stlAllocator     mAllocator;
    stlRingHead      mGroupList;
    ztcGroupItem   * mCurrentGroupPtr;
    stlSemaphore     mLogSem;
    stlSignalFunc    mOldSignalFunc;
    stlChar          mUserId[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar          mUserPw[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar          mHomeDir[STL_MAX_FILE_PATH_LENGTH];
    stlChar          mUserConfigure[STL_MAX_FILE_PATH_LENGTH];
    stlChar          mSetGroupName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar          mArchivePath[STL_MAX_FILE_PATH_LENGTH];
    stlChar          mArchiveFilePrefix[STL_MAX_FILE_NAME_LENGTH];
    stlChar          mDsn[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar          mUserLogFilePath[STL_MAX_FILE_PATH_LENGTH];
    stlChar          mTransFilePath[STL_MAX_FILE_PATH_LENGTH];
    stlChar          mHostIp[16];
    stlChar          mMasterIp[16];
    stlInt64         mTransSortAreaSize;
    stlInt64         mReadLogBlockCount;
    stlInt64         mApplierCommitSize;
    stlInt64         mApplierArraySize;
    stlInt64         mApplierCount;
    stlInt64         mGiveupInterval;
    stlInt64         mTransTableSize;
    
    stlInt32         mCommChunkCount;
    stlInt32         mGroupCount;
    stlInt32         mPort;
    stlInt32         mHostPort;
    stlInt32         mGoldilocksListenPort;
    stlInt32         mSyncherCount;
    stlInt32         mSyncArraySize;
    stlInt32         mUpdateApplyMode;
    
    stlBool          mPropagateMode;
    stlBool          mArchiveMode;
    stlBool          mIsMaster;
    stlBool          mDoReset;
    stlBool          mIsSupplementalLog;
    stlBool          mIsSilent;
    stlBool          mIsSync;
} ztcConfigure;


typedef struct ztcStaticHash
{
    stlUInt16       mBucketCount;
    stlUInt16       mLinkOffset;
    stlUInt16       mKeyOffset;
    stlChar         mPadding[2];
} ztcStaticHash;

typedef struct ztcStaticHashBucket
{
    stlRingHead   mList;
} ztcStaticHashBucket;

typedef stlUInt32 ( *ztcHashingFunc )( void * aKey, stlUInt32 aBucketCount );

typedef stlInt32 ( *ztcCompareFunc )( void * aKeyA, void * aKeyB );

#define ZTC_STATICHASH_GET_NTH_BUCKET( hashPtr, Seq )  \
        ( ztcStaticHashBucket* )((( ztcStaticHash* )hashPtr) + 1) + Seq

/**
 * For MASTER ........
 */

/**
 * Restart 또는 비정상 종료 이후에 재시작할 위치를 저장한다.
 * 파일에서 사용될 구조체
 */
typedef struct ztcCaptureInfo
{
    stlInt64   mRestartLSN;
    stlInt64   mLastCommitLSN;
    stlInt64   mFileSeqNo;
    stlInt32   mRedoLogGroupId;
    stlInt32   mBlockSeq;
    stlInt32   mReadLogNo;
    stlInt16   mWrapNo;
    stlChar    mPadding[2];
} ztcCaptureInfo;


typedef struct ztcLogFileInfo
{
    stlRingEntry    mLink;
    stlChar         mName[STL_MAX_FILE_PATH_LENGTH];
    ztcCaptureInfo  mCaptureInfo;
    stlTime         mLastModifiedTime;
    stlInt64        mFileSize;
    stlInt32        mBlockCount;
    stlInt16        mGroupId;
    stlBool         mActive;
    stlChar         mPadding[3];
} ztcLogFileInfo;


typedef struct ztcCaptureProgress
{
    stlInt64   mLSN;
    stlInt64   mFileSeqNo;
    stlInt32   mRedoLogGroupId;
    stlInt32   mBlockSeq;
    stlInt32   mReadLogNo;      /** 읽어야 하는 LogNo */
    stlInt16   mWrapNo;         //Todo stlInt8? 16?
    stlChar    mPadding[2];
} ztcCaptureProgress;


typedef struct ztcCaptureTableInfo
{
    stlRingEntry mLink;
    stlInt64     mPhysicalId;    //Physical ID
    stlInt64     mGiveUpTransId;    //DDL을 발생시킨 Transaction ID
    stlInt64     mDdlLSN;        //해당 Table에 DDL을 발생시킨 LSN
    stlUInt64    mCommitSCN;
    stlInt32     mGiveUpCount;
    stlInt32     mTableId;       //Internal 관리 ID (FlowFile에 쓰여지는 ID)
    stlChar      mSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar      mTableName[STL_MAX_SQL_IDENTIFIER_LENGTH];
} ztcCaptureTableInfo;


typedef struct ztcMonitorInfo
{
    stlChar    mSlaveIp[32];
    stlInt64   mFileSeqNo;
    stlInt32   mBlockSeq;
    stlInt32   mMasterState;
    stlInt32   mSlaveState;
    stlInt32   mPort;
} ztcMonitorInfo;


typedef stlStatus (* ztcTransCommitCallback)( stlChar            * aData,
                                              stlInt64             aTransId,
                                              stlUInt64            aCommitSCN,
                                              ztcCaptureProgress * aProgress,
                                              stlErrorStack      * aErrorStack );

typedef stlStatus (* ztcTransRollbackCallback)( stlChar            * aData,
                                                stlInt64             aTransId,
                                                stlUInt64            aCommitSCN,
                                                ztcCaptureProgress * aProgress,
                                                stlErrorStack      * aErrorStack );

typedef stlStatus (* ztcCheckRecoveryEndCallback)( stlInt64   aLSN,
                                                   stlBool  * aStatus );

typedef struct ztcCaptureCallback
{
    ztcTransCommitCallback      mTransCommitFunc;
    ztcTransRollbackCallback    mTransRollbackFunc;
    ztcCheckRecoveryEndCallback mCheckRecoveryEndFunc;
} ztcCaptureCallback;

/**
 * Restart 또는 비정상종료 이후에 재시작할 위치를 저장한다.
 */
typedef struct ztcRestartInfo
{
    stlRingEntry       mLink;
    ztcCaptureProgress mProgress;
} ztcRestartInfo;


typedef struct ztcTransSlotHdr
{
    stlRingHead       mTransDataList;     /* ztcTransHdr 의 List Header */
    stlRingHead       mTransChunkList;    /* Transaction에서 사용하는 ChunkList */
    ztcRestartInfo    mRestartInfo;       /* RestartInfo List에서의 위치 */
    stlInt64          mTransId;
    stlInt64          mSize;              /* 매달려있는 데이터의 전체 사이즈 */
    stlInt64          mLogCount;          /* Filtered LogCount */
    stlInt64          mFileLogCount;      /* File에 저장된 Log의 수 */
    stlInt64          mFileSize;
    stlBool           mIsBegin;
    stlBool           mGiveUp;
    stlBool           mPropagated;
    stlBool           mIsRebuild;
    stlChar           mPadd[4];
} ztcTransSlotHdr;


typedef struct ztcTransHdr
{
    stlInt64     mLSN;
    stlInt64     mTransId;
    stlInt64     mRecordId;
    stlInt64     mTablePId;   //Table Physical Id (SCN 비교를 위해 Table을 검색하기 위해서 사용한다.)
    stlInt32     mTableId;    //TableID - Physical ID를 저장하지 않고 Cyclone 내부적으로 사용하는 TableID를 저장한다.
    stlInt32     mSlotIdx;    //Distribute 시에 해당 Transaction이 사용될 Slot Id
    stlUInt16    mLogType;
    stlUInt16    mSize;       //Data Size
    stlInt16     mSubDistAppId;
    stlBool      mTxBeginFlag; 
    stlBool      mTxCommitFlag; 
} ztcTransHdr;


typedef struct ztcTransData
{
    stlRingEntry    mLink;
    stlInt64        mBufferSize;
    ztcTransHdr     mTransHdr;
} ztcTransData;


typedef struct ztcCaptureMgr
{
    stlAllocator       mRegionMem;
    ztcTransSlotHdr  * mTransSlotHdr;
    stlRingHead        mRestartInfoList;              /* ztcRestartInfo List Header */
    stlRingHead        mTransChunkFreeList;
    stlChar          * mLogBodyBuffer;
    stlInt64           mRecoveryLastCommitLSN;        //Recovery시 사용되는 Commit LSN 정보
    stlInt64           mTransTableMax;
    stlInt32           mTransSkipCount;
    stlBool            mCaptureMonitorStatus;
    stlChar            mPadd[3];
} ztcCaptureMgr;


typedef struct ztcLogCursor
{
    stlChar    * mBuffer;
    stlChar    * mBufferOrg;
    stlFile      mFile;
    stlInt32     mFileBlockSeq;
    stlInt32     mMaxBlockCount;
    stlInt32     mBufferBlockSeq;
    stlInt32     mBufferBlockCount;
    stlInt64     mPrevLastLsn;
    stlInt16     mBlockSize;
    stlInt16     mWrapNo;
    stlChar      mPadding[4];
} ztcLogCursor;


typedef struct ztcMasterCMMgr
{
    stlContext     mListenContext;
    stlThread      mListenThread;
    
    stlContext     mSenderContext;
    stlThread      mSenderThread;    
    stlBool        mSenderStatus;
    stlChar        mPadd[7];
    
    stlContext     mMonitorContext;
    stlThread      mMonitorThread;
    stlBool        mMonitorStatus;
    stlChar        mPadd2[7];
} ztcMasterCMMgr;


typedef struct ztcMasterMgr
{
    stlAllocator     mRegionMem;
    ztcStaticHash  * mTableHash;                //Filtered Table Hash
    stlRingHead      mLogFileList;
    
    stlRingHead      mTableMetaList;
    stlInt64         mMaxTableId;
    
    ztcCaptureMgr    mCaptureMgr;
    ztcMasterCMMgr   mCMMgr;
    stlThread        mCaptureThread;
    stlThread        mStatusThread;
    
    stlInt32         mTableCount;
    stlInt32         mLogFileCount;
    
    SQLHDBC          mDbcHandle;
    SQLHENV          mEnvHandle;
    
    stlSemaphore     mCaptureStartSem;          //Capture 시작 대기 Semaphore
    
    stlChar          mSlaveUserId[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar          mSlaveUserPw[STL_MAX_SQL_IDENTIFIER_LENGTH];
    
    stlInt32         mSlaveGoldilocksListenPort;
    stlBool          mSlavePropagateMode;
    stlBool          mDbConnected;
    stlBool          mCaptureRestart;           //Sender가 종료했을때 Capture를 Restart해야할지 여부
    stlBool          mWaitWriteState;           //Sender가 대기중인지를 판단한다.
    stlBool          mWaitReadState;            //Capture가 대기중인지를 판단한다.
    
    /**
     * Capture에서 사용
     */
    stlRingHead      mWaitWriteCkList;          //비어있는 Chunk List.
    stlRingHead      mWriteCkList;              //Capture에서 사용중인 ChunkList
    stlSemaphore     mWaitWriteSem;             //Capture가 Sender에서 사용한 Chunk를 반환 대기하기 위한 Semaphore
    ztcSpinLock      mWriteSpinLock;
    stlChar          mPadding2[4];
    
    /**
     * Sender에서 사용
     */
    stlRingHead      mWaitReadCkList;           //Send를 대기중인 Chunk List.
    stlRingHead      mReadCkList;               //Send 중인 ChunkList
    stlSemaphore     mWaitReadSem;              //Sender가 Capture의 데이터를 대기하기 위한 Semaphore
    ztcSpinLock      mReadSpinLock;
    stlChar          mPadding3[4];

    dtlCharacterSet  mCharacterSet;
    stlInt32         mTimezone;
} ztcMasterMgr;

/**
 * Important !!
 * - Meta 구조체는 Member의 순서를 바꾸면 안됨
 */
typedef struct ztcMasterTableMeta
{
    stlRingEntry   mLink;
    stlRingHead    mColumnMetaList;
    stlChar        mSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar        mTableName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar        mSlaveSchema[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar        mSlaveTable[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlInt64       mFileOffset;
    stlInt64       mPhysicalId;
    stlInt64       mDdlLSN;
    stlUInt64      mCommitSCN;
    stlInt32       mTableId;
    stlInt32       mColumnCount;
} ztcMasterTableMeta;


typedef struct ztcMasterColumnMeta
{
    stlRingEntry   mLink;
    stlChar        mColumnName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar        mDataType[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlBool        mIsPrimary;
    stlBool        mIsUnique;
    stlBool        mIsNullable;
    stlBool        mPadd;
} ztcMasterColumnMeta;

/**
 * For SLAVE ........
 */

typedef struct ztcStmtInfo
{
    SQLHSTMT         mInsertStmt;
    SQLHSTMT         mDeleteStmt;
    SQLHSTMT         mUpdateStmt;
    SQLUSMALLINT   * mInsertStatus;
    SQLUSMALLINT   * mDeleteStatus;
    SQLUSMALLINT   * mUpdateStatus;
    SQLUSMALLINT   * mInsertOperation;
    SQLUSMALLINT   * mDeleteOperation;
    SQLUSMALLINT   * mUpdateOperation;
    stlInt64         mInsertIdx;
    stlInt64         mDeleteIdx;
    stlInt64         mUpdateIdx;
    stlInt64         mInsertPrevIdx;
    stlInt64         mDeletePrevIdx;
    stlInt64         mUpdatePrevIdx;
} ztcStmtInfo;

typedef struct ztcSlaveTableInfo
{
    stlRingEntry     mLink;
    stlInt64         mTableIdAtDB;              //DB에서의 TableID
    stlInt32         mTableId;                  //Meta에서의 TableID
    stlUInt32        mColumnCount;
    stlRingHead      mColumnList;               // ztcColumnInfo

    stlChar          mMasterSchema[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar          mMasterTable[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar          mSlaveSchema[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar          mSlaveTable[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar          mPKIndexName[STL_MAX_SQL_IDENTIFIER_LENGTH];
} ztcSlaveTableInfo;


typedef struct ztcApplierTableInfo
{
    stlRingEntry         mLink;
    stlRingHead          mColumnList;               //ztcColumnValue
    ztcSlaveTableInfo  * mInfoPtr;
    ztcStmtInfo          mStmtInfo;                 //Stmt Handle (ztcStmtInfo)
    stlInt32             mTableId;                  //Meta에서의 TableID
    
    stlBool              mNeedUpdatePrepare;        //Update시에 Prepare가 필요한지 여부를 나타냄
    stlBool              mHasLongVariableCol;       //Column 중에 Long Variable Column이 존재하는지 여부... Insert에서 사용됨..
    stlBool              mUpdateLongVariableCol;    //Long Variable Column이 Update 되었는지 여부
    stlChar              mPadding;
} ztcApplierTableInfo;

typedef struct ztcColumnInfo
{
    stlRingEntry     mLink;
    stlInt64         mColumnId;
    stlInt64         mDataTypeId;
    stlChar          mColumnName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar          mDataTypeStr[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlUInt32        mHashValue;
    stlBool          mIsPrimary;
    stlBool          mIsNullable;
    stlBool          mIsUnique;
    stlChar          mPadding1;

    SQLULEN          mColumnSize;
    SQLLEN           mBufferLength;
    SQLSMALLINT      mValueType;
    SQLSMALLINT      mParameterType;
    SQLSMALLINT      mDecimalDigits;
    
    /**
     * SQL_CHAR, SQL_VARCHAR 일 경우, String Length Unit을 저장
     */
    SQLSMALLINT      mStringLengthUnit;
    
    /**
     * Numeric 일 경우 Precision, Scale을 저장
     */
    stlInt32         mNumericPrecision;
    stlInt32         mNumericScale;
    
    /** 
     * INTERVAL 일 경우 LeadingPrecision, SecondPrecision을 저장
     */
    SQLINTEGER       mLeadingPrecision;
    SQLSMALLINT      mSecondPrecision;
    
    /**
     * Value를 저장하기 위한 공간
     */
    stlInt64         mValueMaxSize;
    stlInt64         mStrValueMaxSize;
} ztcColumnInfo;


typedef struct ztcColumnValue
{
    stlRingEntry     mLink;
    ztcColumnInfo  * mInfoPtr;
    stlBool          mIsUpdated;                    //해당 Column이 Update 되었는지 여부
    stlChar          mPadding1[7];
    
    /**
     * Value를 저장하기 위한 공간
     */
    stlChar        * mValue;                        //데이터가 저장될 Pointer.
    stlChar        * mStrValue;                     //Data Convertion을 위해서 필요 (Date/Time/Interval)
    stlInt64       * mValueSize;
    SQLLEN         * mIndicator;
    stlChar          mPadding2[4];
    
    /**
     * Update 수행시 Before Value를 저장하기 위한 공간
     */
    stlChar        * mBefValue;                     //데이터가 저장될 Pointer.
    stlChar        * mBefStrValue;
    stlInt64       * mBefValueSize;
    SQLLEN         * mBefIndicator;
    stlChar          mPadding3[4];
    
    /**
     * PrimaryKey 값을 저장하기 위한 공간
     */
    stlChar        * mKeyValue;                     // Key데이터가 저장될 Pointer.
    stlChar        * mKeyStrValue;                  //Data Convertion을 위해서 필요 (Date/Time/Interval)
    stlInt64       * mKeyValueSize;
    SQLLEN         * mKeyIndicator;
    stlChar          mPadding4[4];
    
    /**
     * Update 수행시 Returning Value를 저장하기 위한 공간
     */
    stlChar        * mRetValue;                     //데이터가 저장될 Pointer.
    stlChar        * mRetStrValue;
    stlInt64       * mRetValueSize;
    SQLLEN         * mRetIndicator;
    stlChar          mPadding5[4];
} ztcColumnValue;

/**
 * Applier에서 Execute된 Record
 */
typedef struct ztcApplierRecItem
{
    stlRingEntry     mLink;
    stlInt64         mRefCount;
    stlInt32         mIdx;
    stlInt32         mPadd;
} ztcApplierRecItem;


typedef struct ztcApplierMgr
{
    stlRingEntry           mLink;
    stlAllocator           mRegionMem;
    stlRingHead            mRecordList; 
    
    ztcStaticHash        * mTableHash;             // ztcApplierTableInfo
    stlThread              mThread;
    SQLHDBC                mDbcHandle;
    SQLHSTMT               mStateStmtHandle;        /* Slave의 Commit 상태 저장을 위한 Statement Handle */
    ztcCaptureInfo         mRestartInfo;            /* 마지막 Commit이 발생했을 때 Restart Info */
    stlInt64               mApplyCount;             /* 한 Transaction이 수행된 INSERT/UPDATE/DELETE의 수... */
    stlInt64               mApplyCountAfterCommit;  /* COMMIT이후에 발생된 INSERT/UPDATE/DELETE의 수.. */
    stlInt64               mArraySize;
    
    ztcApplierRecItem    * mItemArr;
    
    ztcApplierTableInfo  * mPrevTableInfo;
    
    stlInt32               mPrevTransType;
    stlInt32               mAppId;
    
    stlUInt32              mWaitSubDistState;
    ztcSpinLock            mSubDistSpinLock;
    
    stlBool                mWaitWriteState;
    stlBool                mWaitReadState;
    stlBool                mAnalyzeState;
    stlBool                mWaitDistState;              //해당 Applier에서 Vacate를 대기하고 있는지 여부를 기록한다. 만약 TRUE이면 새로운 Tx가 Begin되기전에 기존의 Tx가 Commit되어 Vacate 되어야 한다.
    stlChar                mPadding1[4];
    
    /**
     * Long Varchar, Long VarBinary를 위한 Allocator
     */
    stlDynamicAllocator    mLongVaryingMem;
    
    /**
     * Distributor에서 사용
     */
    stlRingHead            mWaitWriteCkList;  
    stlRingHead            mWriteCkList;      
    stlSemaphore           mWaitWriteSem;
    ztcSpinLock            mWriteSpinLock;
    stlChar                mPadding2[4];
    
    /**
     * Applier에서 사용
     */
    stlRingHead            mWaitReadCkList;
    stlRingHead            mReadCkList;
    stlSemaphore           mWaitReadSem;
    ztcSpinLock            mReadSpinLock;
    stlChar                mPadding3[4];
} ztcApplierMgr;


typedef struct ztcSlaveCMMgr
{
    stlContext     mContext;
    stlThread      mThread;
} ztcSlaveCMMgr;


typedef struct ztcRecStateItem
{
    stlInt64         mRefCount;         //해당 Item을 사용한 Count
    stlInt64         mSubRefCount;      //Sub-Distributor에서 사용하는 Count
    stlInt64         mSlotNo;           //Not Used...
    ztcSpinLock      mSpinLock;
    stlInt32         mAppId;
} ztcRecStateItem;


typedef struct ztcSlaveMgr
{
    stlAllocator       mRegionMem;
    ztcStaticHash    * mTableHash;           //Filtered Table Hash
    stlRingHead        mTableList;           //Master Table과 적용할 Slave Table이 다를 경우 Match 정보 : smlSlaveTableInfo
    ztcApplierMgr    * mApplierMgr;
    ztcSlaveCMMgr      mCMMgr;
    
    SQLHDBC            mDbcHandle;
    SQLHENV            mEnvHandle;
    
    dtlFuncVector      mDTVector;
    ztcCaptureInfo     mRestartInfo;
    
    stlInt64           mApplierCount;
    stlInt32           mTableCount;
    stlInt32           mCurApplierIdx;
    stlInt32           mGoldilocksListenPort;
    stlBool            mWaitReadState;        //Distributor가 대기중인지를 판단한다.
    stlBool            mWaitWriteState;       //Receiver가 대기중인지를 판단한다.
    stlBool            mSubWaitReadState;     //Sub-Dist
    stlBool            mSubWaitWriteState;    //Sub-Dist
    
    stlSemaphore       mDistbtStartSem;       //시작 대기
    stlSemaphore       mApplierStartSem;      //시작 대기
    stlThread          mDistbtThread;         //Distributor Thread
    stlThread          mSubDistbtThread;      //Sub-Distributor Thread
    
    dtlCharacterSet    mCharacterSet;
    SQLINTEGER         mTimezone;
    
    /**
     * Record의 동시성 제어를 위한 공간 - Distributor에서 사용
     */
    ztcRecStateItem   * mItemArr;
    
    /**
     * Receiver에서 사용
     */
    stlRingHead        mWaitWriteCkList;     //비어있는 Chunk List.
    stlRingHead        mWriteCkList;         //Receiver에서 사용중인 ChunkList
    stlSemaphore       mWaitWriteSem;        //Receiver Semaphore
    ztcSpinLock        mWriteSpinLock;
    stlChar            mPadding2[4];
    
    /**
     * Distributor에서 사용
     */
    stlRingHead        mWaitReadCkList;      //Distribute를 대기중인 Chunk List.
    stlRingHead        mReadCkList;          //Distribute중인 ChunkList
    stlSemaphore       mWaitReadSem;         //Distributor Semaphore
    ztcSpinLock        mReadSpinLock;
    stlChar            mPadding3[4];
    
    /**
     * Sub-Distributor에서 사용
     */
    stlRingHead        mSubWaitWriteCkList;
    stlRingHead        mSubWriteCkList;      
    stlSemaphore       mSubWaitWriteSem;
    ztcSpinLock        mSubWriteSpinLock;
    stlChar            mPadding4[4];
    
    stlRingHead        mSubWaitReadCkList;
    stlRingHead        mSubReadCkList;
    stlSemaphore       mSubWaitReadSem;
    ztcSpinLock        mSubReadSpinLock;
    stlChar            mSubPadding5[4];
    
} ztcSlaveMgr;


/**
 * Sync
 */

/**
 * Syncher Manager에서 할당하는 Buffer Info에서 할당과 해제를 반복하는 Column.
 */
typedef struct ztcSyncColumnValue
{
    stlRingEntry                      mLink;
    SQLLEN                          * mIndicator;
    stlChar                         * mValue;
    ztcColumnInfo                   * mColumnInfoPtr;
    SQL_LONG_VARIABLE_LENGTH_STRUCT * mLongValue;
} ztcSyncColumnValue;

/**
 * Syncher Manager에서 Syncher에게 보내는 Table 정보.
 */
typedef struct ztcSyncTableInfo
{
    stlRingHead  mColumnInfoList; /** ztcColumnInfo List */
    stlRingEntry mLink;
    stlChar      mMasterSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar      mMasterTableName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar      mSlaveSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar      mSlaveTableName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlUInt64    mViewScn;
    stlInt64     mPhysicalId;
    stlInt32     mColumnCount;
    stlChar      mPadding[4];
} ztcSyncTableInfo;

typedef enum
{
    ZTC_SYNC_NEXT_JOB_NONE  = 0,
    ZTC_SYNC_NEXT_JOB_BINDPARAM,
    ZTC_SYNC_NEXT_JOB_KEEP_EXEC,
    ZTC_SYNC_NEXT_JOB_FINISH
} ztcSyncNextJob;

/**
 * Syncher Manager에서 할당하여 과 복하는 Buffer.
 */ 
typedef struct ztcSyncBufferInfo
{
    stlSemaphore       mSyncSem;
    stlRingHead        mColumnValueList;  /** ztcSyncColumnValue List */
    stlRingEntry       mLink;
    stlInt32           mNextJob;
    stlBool            mIsFetched;
    stlChar            mPadding1[3];
    stlUInt64          mFetchedCount;
    ztcSyncTableInfo * mTableInfoPtr;
    stlInt64           mErrCount;
} ztcSyncBufferInfo;

typedef struct ztcSyncMgr
{
    stlDynamicAllocator  mDynamicMem;
    SQLHDBC              mDbcHandle;
    SQLHENV              mEnvHandle;
    SQLHDBC              mSlaveDbcHandle;
    SQLHENV              mSlaveEnvHandle;
    stlRingHead          mSyncherList;        /** ztcSyncher List  */
    stlRingHead          mBufferInfoList;     /** ztcSyncBufferInfo List */
    stlRingHead          mTableList;          /** ztcSyncTableInfo List */
    stlInt64             mSyncherCount;
    stlInt64             mArraySize;

    dtlFuncVector        mDTVector;
    dtlCharacterSet      mCharacterSet;
    SQLINTEGER           mTimezone;
    SQLUSMALLINT       * mArrRowStatus;
    SQLULEN              mRowFetchedCount;
    
    stlChar              mSlaveId[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar              mSlavePw[STL_MAX_SQL_IDENTIFIER_LENGTH];
} ztcSyncMgr;

typedef struct ztcSyncher
{
    stlAllocator        mRegionMem;
    stlRingEntry        mLink;
    stlThread           mThread;
    SQLHENV             mEnvHandle;
    SQLHDBC             mDbcHandle;
    stlInt64            mSyncherId;
    stlInt64            mArraySize;
    SQLUSMALLINT      * mArrRecStatus;
    SQLULEN             mRecProcessedCount;
    ztcSyncBufferInfo * mBufferInfoPtr;
} ztcSyncher;

/**
 * Communication
 */
typedef struct ztcProtocolHdr
{
    stlInt32    mCategory;
    stlInt32    mType;
    stlInt64    mLength;
} ztcProtocolHdr;


typedef struct ztcProtocolColumnInfo
{
    stlChar    mColumnName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar    mDataType[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlInt32   mTableId;
    stlBool    mIsPrimary;
    stlBool    mIsUnique;
    stlBool    mIsNullable;
    stlChar    mPadding;
} ztcProtocolColumnInfo;


typedef enum
{
    ZTC_CATEGORY_NONE = 0,
    ZTC_CATEGORY_INIT,
    ZTC_CATEGORY_HANDSHAKE,
    ZTC_CATEGORY_SYNC,
    ZTC_CATEGORY_DATA,
    ZTC_CATEGORY_CONTROL,
    ZTC_CATEGORY_MONITOR,
    ZTC_CATEGORY_ERROR
} ztcProtocolCategory;


typedef enum
{
    ZTC_INIT_REQUEST_CLIENT_TYPE = 0,
    ZTC_INIT_RESPONSE_CLIENT_TYPE,
    ZTC_INIT_CLIENT_OK,    
    
    ZTC_HANDSHAKE_REQUEST_TABLE_COUNT,
    ZTC_HANDSHAKE_RESPONSE_TABLE_COUNT,
    
    ZTC_HANDSHAKE_REQUEST_TABLE_NAME,
    ZTC_HANDSHAKE_RESPONSE_TABLE_NAME,
    ZTC_HANDSHAKE_RESPONSE_INVALID_TABLE_NAME,
    
    ZTC_HANDSHAKE_REQUEST_TABLE_ID,
    ZTC_HANDSHAKE_RESPONSE_TABLE_ID,
    
    ZTC_HANDSHAKE_REQUEST_COLUMN_COUNT,
    ZTC_HANDSHAKE_RESPONSE_COLUMN_COUNT,
    
    ZTC_HANDSHAKE_REQUEST_COLUMN_INFO,
    ZTC_HANDSHAKE_RESPONSE_COLUMN_INFO,
    ZTC_HANDSHAKE_RESPONSE_COLUMN_INFO_SUCCESS,
    ZTC_HANDSHAKE_RESPONSE_COLUMN_INFO_FAIL,
    
    ZTC_HANDSHAKE_REQUEST_CAPTURE_INFO,
    ZTC_HANDSHAKE_RESPONSE_CAPTURE_INFO,
    
    ZTC_SYNC_REQUEST_CONN_INFO,
    ZTC_SYNC_RESPONSE_CONN_INFO,
    ZTC_SYNC_MODE_OK,
    ZTC_SYNC_MODE_NOT_OK,
    
    ZTC_DATA_SEND,
    ZTC_DATA_RECEIVE_OK,
    
    ZTC_CONTROL_STOP,
    
    ZTC_CONTROL_HEARTBEAT,
    
    ZTC_CONTROL_SYNC_START,
    ZTC_CONTROL_SYNC_DONE,
    
    ZTC_MONITOR_REQUEST_MONITOR_INFO,
    ZTC_MONITOR_RESPONSE_MONITOR_INFO,
    
    ZTC_ERROR_SLAVE_ALREADY_CONNECTED,
    ZTC_ERROR_PROTOCOL_ERROR,
    ZTC_ERROR_MASTER,
    ZTC_ERROR_SLAVE,
    
    ZTC_PROTOCOL_MAX
} ztcProtocolTYPE;


typedef enum
{
    ZTC_CAPTURE_STATUS_NONE = 0,
    ZTC_CAPTURE_STATUS_EOF,
    ZTC_CAPTURE_STATUS_READ_DONE,
    ZTC_CAPTURE_STATUS_INVALID_WRAP_NO,
    ZTC_CAPTURE_STATUS_ERR,
    ZTC_CAPTURE_STATUS_MAX
} ztcCaptureStatus;


typedef enum
{
    ZTC_LOG_STATUS_NONE = 0,
    ZTC_LOG_STATUS_INVALID_LOG,
    ZTC_LOG_STATUS_INVALID_WRAP_NO,
    ZTC_LOG_STATUS_MAX
} ztcLogStatus;


typedef enum
{
    ZTC_CAPTURE_TYPE_NONE   = 0,
    ZTC_CAPTURE_TYPE_BEGIN,
    ZTC_CAPTURE_TYPE_COMMIT,
    ZTC_CAPTURE_TYPE_INSERT,
    ZTC_CAPTURE_TYPE_UNINSERT,
    ZTC_CAPTURE_TYPE_DELETE,
    ZTC_CAPTURE_TYPE_UNDELETE,
    ZTC_CAPTURE_TYPE_SUPPL_DEL,
    ZTC_CAPTURE_TYPE_UPDATE,
    ZTC_CAPTURE_TYPE_UNUPDATE,
    ZTC_CAPTURE_TYPE_INSERT_FOR_UPDATE,
    ZTC_CAPTURE_TYPE_DELETE_FOR_UPDATE,
    ZTC_CAPTURE_TYPE_APPEND_FOR_UPDATE,
    ZTC_CAPTURE_TYPE_UNAPPEND_FOR_UPDATE,
    ZTC_CAPTURE_TYPE_SUPPL_UPD,
    ZTC_CAPTURE_TYPE_SUPPL_UPD_COLS,
    ZTC_CAPTURE_TYPE_SUPPL_UPD_BEF_COLS,
    ZTC_CAPTURE_TYPE_MAX
} ztcCaptureTransType;

typedef enum
{
    ZTC_CAPTURE_READ_NORMAL = 0,
    ZTC_CAPTURE_READ_WRITING,
    ZTC_CAPTURE_READ_EOF,
    ZTC_CAPTURE_READ_MAX
} ztcCaptureReadType;

typedef enum
{
    ZTC_MASTER_STATE_NA = 0,
    ZTC_MASTER_STATE_RUNNING,
    ZTC_MASTER_STATE_READY,
    ZTC_MASTER_STATE_MAX
} ztcMasterState;

typedef enum
{
    ZTC_SLAVE_STATE_NA = 0,
    ZTC_SLAVE_STATE_RUNNING,
    ZTC_SLAVE_STATE_MAX
} ztcSlaveState;


/** @} */

/** @} */

#endif /* _ZTCDEF_H_ */
