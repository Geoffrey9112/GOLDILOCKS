/*******************************************************************************
 * smrDef.h
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


#ifndef _SMRDEF_H_
#define _SMRDEF_H_ 1

/**
 * @file smrDef.h
 * @brief Storage Manager Layer Recovery Internal Definition
 */

/**
 * @defgroup smrInternal Component Internal Routines
 * @ingroup smr
 * @{
 */

/**
 * @brief if aLid1 <= aLid2 then return true
 */
#define SMR_IS_LESS_THAN_EQUAL_LID( aLid1, aLid2 )                                  \
    ( (SMR_FILE_SEQ_NO(aLid1) > SMR_FILE_SEQ_NO(aLid2)) ? STL_FALSE :               \
      (SMR_FILE_SEQ_NO(aLid1) < SMR_FILE_SEQ_NO(aLid2)) ? STL_TRUE :                \
      (SMR_BLOCK_SEQ_NO(aLid1) > SMR_BLOCK_SEQ_NO(aLid2)) ? STL_FALSE :             \
      (SMR_BLOCK_SEQ_NO(aLid1) < SMR_BLOCK_SEQ_NO(aLid2)) ? STL_TRUE :              \
      (SMR_BLOCK_OFFSET(aLid1) > SMR_BLOCK_OFFSET(aLid2)) ? STL_FALSE :             \
      (SMR_BLOCK_OFFSET(aLid1) <= SMR_BLOCK_OFFSET(aLid2)) ? STL_TRUE : STL_FALSE ) \

      
/**
 * @brief Log Persistent Information을 위한 구조체
 */
typedef struct smrLogPersData
{
    smrLid      mChkptLid;         /**< Checkpoint Log Idendifier */
    smrLsn      mChkptLsn;         /**< Checkpoint Log Sequence Number */
    smrLsn      mOldestLsn;
    stlInt64    mLastInactiveLfsn; /**< Latest inactive logfile sequence number */
    stlInt32    mArchivelogMode;   /**< Archivelog Mode */
    stlBool     mNeedResetLogs;    /**< need resetlogs when database open */
    stlChar     mPadding[3];
    stlTime     mCreationTime;     /**< Database Creation Time  */
} smrLogPersData;

/**
 * @brief 서버 Recovery 정보의 초기화 매크로
 */
#define SMR_INIT_LOG_PERSISTENT_DATA( aLogPersData )                      \
    {                                                                     \
        stlMemset( (aLogPersData), 0x00, STL_SIZEOF(smrLogPersData) );    \
        (aLogPersData)->mChkptLid = SMR_INVALID_LID;                      \
        (aLogPersData)->mChkptLsn = SMR_INVALID_LSN;                      \
        (aLogPersData)->mOldestLsn = SMR_INVALID_LSN;                     \
        (aLogPersData)->mLastInactiveLfsn = SMR_INVALID_LOG_FILE_SEQ_NO;  \
        (aLogPersData)->mArchivelogMode = SML_NOARCHIVELOG_MODE;          \
        (aLogPersData)->mNeedResetLogs = STL_FALSE;                       \
    }

#define SMR_LOG_STREAM_STATE_CREATING  (1)
#define SMR_LOG_STREAM_STATE_DROPPING  (2)
#define SMR_LOG_STREAM_STATE_UNUSED    (3)
#define SMR_LOG_STREAM_STATE_ACTIVE    (4)

#define SMR_LOG_STREAM_DEFAULT_BLOCKSIZE (512)
#define SMR_REVERSED_LOG_FILE_GAP        (1)  /** CURRENT Log Group과 Minimum ACTIVE Log Group과의 차 */

typedef struct smrLogStreamPersData
{
    stlInt32   mState;
    stlInt32   mLogGroupCount;
    stlInt32   mMaxBlockCount;
    stlInt16   mBlockSize;
    stlChar    mAlign[2];
    stlInt64   mCurFileSeqNo;
} smrLogStreamPersData;

#define SMR_INIT_LOG_STREAM_PERSISTENT_DATA( aLogStreamPersData )                       \
    {                                                                                   \
        (aLogStreamPersData)->mState = SMR_LOG_STREAM_STATE_UNUSED;                     \
        (aLogStreamPersData)->mLogGroupCount = 0;                                       \
        (aLogStreamPersData)->mBlockSize = 0;                                           \
        (aLogStreamPersData)->mCurFileSeqNo = -1;                                       \
        (aLogStreamPersData)->mMaxBlockCount = 0;                                       \
        stlMemset( &(aLogStreamPersData)->mAlign, 0x00, STL_SIZEOF( stlChar ) * 2 );    \
    }

#define SMR_IS_ONLINE_STREAM( aLogStream )                                              \
    ( ( ((aLogStream)->mLogStreamPersData.mState == SMR_LOG_STREAM_STATE_DROPPING) ||   \
        ((aLogStream)->mLogStreamPersData.mState == SMR_LOG_STREAM_STATE_ACTIVE) ) ?    \
      STL_TRUE : STL_FALSE )

#define SMR_LOG_GROUP_STATE_CREATING  (1)
#define SMR_LOG_GROUP_STATE_DROPPING  (2)
#define SMR_LOG_GROUP_STATE_UNUSED    (3)
#define SMR_LOG_GROUP_STATE_ACTIVE    (4)
#define SMR_LOG_GROUP_STATE_CURRENT   (5)
#define SMR_LOG_GROUP_STATE_INACTIVE  (6)

#define SMR_LOG_GROUP_DEFAULT_FIILESIZE (512 * 1024)

typedef struct smrLogGroupPersData
{
    stlInt32   mState;
    stlInt32   mLogMemberCount;
    stlInt16   mLogGroupId;
    stlChar    mAlign[6];
    stlInt64   mFileSeqNo;
    stlInt64   mFileSize;
    smrLsn     mPrevLastLsn;
} smrLogGroupPersData;

#define SMR_LOG_MEMBER_STATE_CREATING  (1)
#define SMR_LOG_MEMBER_STATE_DROPPING  (2)
#define SMR_LOG_MEMBER_STATE_UNUSED    (3)
#define SMR_LOG_MEMBER_STATE_ACTIVE    (4)

typedef struct smrLogMemberPersData
{
    stlInt32 mState;
    stlChar  mAlign[4];
    stlChar  mName[STL_MAX_FILE_PATH_LENGTH];
} smrLogMemberPersData;

typedef struct smrLogBuffer
{
    knlLatch   mBufferLatch;
    stlInt32   mRearFileBlockSeqNo;
    stlInt32   mFrontFileBlockSeqNo;
    smrSbsn    mRearSbsn;
    smrLsn     mRearLsn;
    smrLid     mRearLid;
    void     * mBuffer;
    stlInt64   mBufferSize;
    stlInt64   mBufferBlockCount;
    stlInt64   mBlockOffset;
    stlInt64   mFileSeqNo;
    smrSbsn    mFrontSbsn;
    smrLsn     mFrontLsn;
    smrLsn     mGroupCommitLsn;
    smrSbsn    mLogSwitchingSbsn;
    stlInt8    mLeftLogCountInBlock;     /** for read */
    stlBool    mPartialLogDetected;      /** for read */
    stlBool    mEndOfLog;                /** for read */
    stlBool    mEndOfFile;               /** for read */
} smrLogBuffer;

#define SMR_REAR_POS( aLogBuffer )  ( (aLogBuffer)->mRearSbsn % (aLogBuffer)->mBufferBlockCount )
#define SMR_FRONT_POS( aLogBuffer ) ( (aLogBuffer)->mFrontSbsn % (aLogBuffer)->mBufferBlockCount )
#define SMR_BUFFER_POS( aSbsn, aLogBuffer ) ( (aSbsn) % (aLogBuffer)->mBufferBlockCount )

#define SMR_GET_LOG_BUFFER_BLOCK( aLogBuffer, aSbsn, aBlockSize ) \
    ( (aLogBuffer)->mBuffer + ( SMR_BUFFER_POS( (aSbsn), (aLogBuffer) ) * (aBlockSize) ) )
    
#define SMR_MAX_PENDING_LOG_BUFFER_COUNT (32)
#define SMR_PENDING_BLOCK_SIZE           (64)

#define SMR_PEND_CURSOR_CLOSED           (1)
#define SMR_PEND_CURSOR_ACTIVE           (2)

#define SMR_PEND_REAR_POS( aPendLogBuffer )                                 \
    ( (aPendLogBuffer)->mRearPbsn % (aPendLogBuffer)->mBufferBlockCount )
#define SMR_PEND_FRONT_POS( aPendLogBuffer )                                \
    ( (aPendLogBuffer)->mFrontPbsn % (aPendLogBuffer)->mBufferBlockCount )
#define SMR_PEND_BUFFER_ADDR( aPendLogBuffer, aPos )                \
    ( (aPendLogBuffer)->mBuffer + (aPos) * SMR_PENDING_BLOCK_SIZE )
#define SMR_PEND_FRONT_BUFFER_ADDR( aPendLogBuffer )                    \
    ( (aPendLogBuffer)->mBuffer +                                       \
      (SMR_PEND_FRONT_POS(aPendLogBuffer) * SMR_PENDING_BLOCK_SIZE) )
#define SMR_PEND_REAR_BUFFER_ADDR( aPendLogBuffer )                     \
    ( (aPendLogBuffer)->mBuffer +                                       \
      (SMR_PEND_REAR_POS(aPendLogBuffer) * SMR_PENDING_BLOCK_SIZE) )
#define SMR_PEND_FRONT_LSN( aPendLogBuffer )                    \
    ( *(smrLsn*)SMR_PEND_FRONT_BUFFER_ADDR(aPendLogBuffer) )

/**
 * @brief Pending Block Sequence Number
 */
typedef stlInt64 smrPbsn;

typedef struct smrPendLogBuffer
{
    knlLatch   mLatch;
    void     * mBuffer;
    stlInt32   mBufferSize;
    stlInt32   mBufferBlockCount;
    smrPbsn    mFrontPbsn;
    smrPbsn    mRearPbsn;
    smrLsn     mFrontLsn;
    smrLsn     mRearLsn;
    stlChar    mPadding[16];
} smrPendLogBuffer;

typedef struct smrPendLogCursor
{
    stlChar            mState[SMR_MAX_PENDING_LOG_BUFFER_COUNT];   /**< 각 버퍼의 상태 */
    smrPbsn            mEndPbsn[SMR_MAX_PENDING_LOG_BUFFER_COUNT]; /**< 각 버퍼의 최대 종료 조건 */
    smrLsn             mEndLsn;                                    /**< 종료 조건 */
    smrPendLogBuffer * mTargetPendBuffer;                          /**< Cursor가 읽어야 하는 대상 버퍼 */
    stlInt32           mReadBlockCount;                            /**< Cursor가 읽어야 하는 Block 개수 */
} smrPendLogCursor;

typedef struct smrStatistics
{
    stlInt64  mFlushCount;
    stlInt64  mFlushTryCount;
    stlInt64  mWaitCountByBufferFull;
    stlInt64  mWaitCount4Sync;
    stlInt64  mBlockedLoggingCount;
} smrStatistics;

#define SMR_INIT_LOG_STREAM_STATISTICS( aStatistics )   \
{                                                       \
    (aStatistics)->mFlushCount = 0;                     \
    (aStatistics)->mFlushTryCount = 0;                  \
    (aStatistics)->mWaitCountByBufferFull = 0;          \
    (aStatistics)->mWaitCount4Sync = 0;                 \
    (aStatistics)->mBlockedLoggingCount = 0;            \
}

typedef struct smrLogMember
{
    stlRingEntry           mLogMemberLink;
    smrLogMemberPersData   mLogMemberPersData;
} smrLogMember;

typedef struct smrLogGroup
{
    stlInt32             mCurBlockSeq;
    stlRingEntry         mLogGroupLink;
    stlRingHead          mLogMemberList;
    smrSbsn              mLogSwitchingSbsn;
    smrLogGroupPersData  mLogGroupPersData;
} smrLogGroup;

typedef struct smrLogStream
{
    knlLatch               mFileLatch;
    knlCondVar             mCondVar;
    smrStatistics          mStatistics;
    smrLogBuffer         * mLogBuffer;
    smrPendLogBuffer     * mPendLogBuffer;
    stlInt32               mPendBufferCount;
    stlChar                mPadding2[4];
    stlRingHead            mLogGroupList;
    smrLogStreamPersData   mLogStreamPersData;
    smrLogGroup          * mCurLogGroup4Disk;
    smrLogGroup          * mCurLogGroup4Buffer;
} smrLogStream;

#define SMR_INIT_LOG_STREAM( aStream )                                          \
{                                                                               \
    STL_TRY( knlInitLatch( &(aStream)->mFileLatch,                              \
                           STL_TRUE,                                            \
                           KNL_ENV( aEnv ) ) == STL_SUCCESS );                  \
    STL_TRY( knlInitCondVar( &(aStream)->mCondVar,                              \
                             KNL_ENV( aEnv ) ) == STL_SUCCESS );                \
    STL_RING_INIT_HEADLINK( &(aStream)->mLogGroupList,                          \
                            STL_OFFSETOF( smrLogGroup, mLogGroupLink ) );       \
    (aStream)->mLogBuffer = NULL;                                               \
    (aStream)->mCurLogGroup4Disk = NULL;                                        \
    (aStream)->mCurLogGroup4Buffer = NULL;                                      \
    (aStream)->mPendLogBuffer = NULL;                                           \
    (aStream)->mPendBufferCount = 0;                                            \
    SMR_INIT_LOG_STREAM_STATISTICS( &(aStream)->mStatistics );                  \
}

#define SMR_INIT_LOG_GROUP( aGroup )                                            \
{                                                                               \
    STL_RING_INIT_DATALINK( (aGroup),                                           \
                            STL_OFFSETOF( smrLogGroup, mLogGroupLink ) );       \
    STL_RING_INIT_HEADLINK( &(aGroup)->mLogMemberList,                          \
                            STL_OFFSETOF( smrLogMember, mLogMemberLink ) );     \
}

#define SMR_INIT_LOG_MEMBER( aMember )                                           \
{                                                                                \
    STL_RING_INIT_DATALINK( (aMember),                                           \
                            STL_OFFSETOF( smrLogMember, mLogMemberLink ) );      \
}

/**
 * @defgroup smrBackupState Backup 상태
 * @ingroup smr
 * @{
 */
#define SMR_BACKUP_STATE_IN_IDLE        (0)  /** backup이 진행중이지 않은 상태 */
#define SMR_BACKUP_STATE_IN_PROGRESS    (1)  /** backup이 진행중인 상태 */
/** @} */

/**
 * @defgroup smrMRState Media Recovery 상태
 * @ingroup smr
 * @{
 */
#define SMR_MEDIA_RECOVERY_STATE_IN_IDLE        (0)  /** Media Recovery가 진행중이지 않은 상태 */
#define SMR_MEDIA_RECOVERY_STATE_IN_PROGRESS    (1)  /** Media Recovery가 진행중인 상태 */
/** @} */

typedef struct smrPendAddLogMember
{
    stlInt16   mLogGroupId;
    stlChar    mName[STL_MAX_FILE_PATH_LENGTH];
} smrPendAddLogMember;

typedef struct smrPendAddLogGroup
{
    stlInt16   mLogGroupId;
} smrPendAddLogGroup;

typedef struct smrPendDropLogGroup
{
    stlInt16   mLogGroupId;
    stlInt32   mPrevGroupState;
} smrPendDropLogGroup;

typedef struct smrPendDropLogMember
{
    stlInt16   mLogGroupId;
    stlChar    mName[STL_MAX_FILE_PATH_LENGTH];
    stlInt32   mPrevMemberState;
} smrPendDropLogMember;

typedef struct smrPendRenameLogFile
{
    stlInt16   mLogGroupId;
    stlChar    mFromName[STL_MAX_FILE_PATH_LENGTH];
    stlChar    mToName[STL_MAX_FILE_PATH_LENGTH];
} smrPendRenameLogFile;

/**
 * 최소 log block size(512)
 */
#define SMR_MIN_LOG_BLOCK_SIZE (512)
/**
 * 최대 log block size(4096)
 */
#define SMR_MAX_LOG_BLOCK_SIZE (4096)

typedef struct smrWarmupEntry
{
    smrLogStream      mLogStream;
    stlInt16          mLogFlusherState;
    stlInt16          mCheckpointerState;
    stlBool           mEnableLogging;
    stlBool           mEnableFlushing;
    stlChar           mPadding1[2];
    stlInt64          mBlockedLfsn;        /**< blocked log file sequence number */
    smrLogPersData    mLogPersData;
    smrLsn            mLsn;
    smrRecoveryInfo   mRecoveryInfo;
    smrChkptInfo      mChkptInfo;
    stlInt64          mSyncValue[2];       /**< 일관성을 보장하며 Log pers data를 읽기 위한 자료구조 */
    stlInt64          mBlockLogFlushing;   /**< log flushing state */
    stlUInt32         mSpinLock;           /**< mEnableFlushing을 위한 Lock */
    smrLogBlockHdr    mLogBlockHdr;        /**< Partial log 대비를 위한 snapshot */
    stlChar           mPadding2[39];       /**< to align cache line */
    /* cache line align을 맞추기 위해 64 byte의 배수로 맞춰야 함 */
} smrWarmupEntry;

/**
 * @brief Log Block의 크기
 */
extern stlInt64          gSmrLogBlockSize;
extern stlInt64          gSmrBulkIoBlockCount;

#define SMR_BLOCK_SIZE   (gSmrLogBlockSize)

#define SMR_MAX_LOGBODY_SIZE (SMXM_MAX_LOGBLOCK_SIZE)
#define SMR_BLOCK_HDR_SIZE   ( 8 + 1 + 2 + 2 )

#define SMR_BULK_IO_BLOCK_UNIT_COUNT (gSmrBulkIoBlockCount)

#define SMR_INIT_LOG_BUFFER( aStreamBuffer, aBufferSize, aBuffer, aIsShared, aBlockSize )   \
    {                                                                                       \
        STL_TRY( knlInitLatch( &(aStreamBuffer)->mBufferLatch,                              \
                               (aIsShared),                                                 \
                               KNL_ENV( aEnv ) ) == STL_SUCCESS );                          \
        (aStreamBuffer)->mBuffer = (void*)STL_ALIGN( (stlUInt64)aBuffer, aBlockSize );      \
        (aStreamBuffer)->mBufferSize = STL_ALIGN( aBufferSize, aBlockSize );                \
        (aStreamBuffer)->mBufferBlockCount = (aStreamBuffer)->mBufferSize / aBlockSize;     \
        (aStreamBuffer)->mBlockOffset = SMR_BLOCK_HDR_SIZE;                                 \
        (aStreamBuffer)->mLeftLogCountInBlock = 0;                                          \
        (aStreamBuffer)->mPartialLogDetected = STL_FALSE;                                   \
        (aStreamBuffer)->mEndOfLog = STL_FALSE;                                             \
        (aStreamBuffer)->mEndOfFile = STL_FALSE;                                            \
        (aStreamBuffer)->mFileSeqNo = 0;                                                    \
        (aStreamBuffer)->mRearFileBlockSeqNo = 0;                                           \
        (aStreamBuffer)->mFrontFileBlockSeqNo = 0;                                          \
        (aStreamBuffer)->mRearLsn = SMR_INVALID_LSN;                                        \
        (aStreamBuffer)->mLogSwitchingSbsn = -1;                                            \
        (aStreamBuffer)->mFrontLsn = SMR_INVALID_LSN;                                       \
        (aStreamBuffer)->mRearLid = (smrLid){0,0,0};                                        \
        (aStreamBuffer)->mRearSbsn = 0;                                                     \
        (aStreamBuffer)->mGroupCommitLsn = 0;                                               \
        (aStreamBuffer)->mFrontSbsn = -1;                                                   \
    }

/**
 * @brief Media Recovery 를 위한 자료구조
 */

#define SMR_MEDIA_RECOVERY_IS_USING_BACKUP_CTRLFILE( aFlag ) \
    ( ((aFlag) & SML_USING_BACKUP_CTRLFILE_MASK) == SML_USING_BACKUP_CTRLFILE_YES )

#define SMR_WRAP_NO( aFileSeq ) ( (stlInt16)((aFileSeq) & SMR_WRAP_NO_CREATION_MASK) )

#define SMR_READ_BLOCK_INFO( aBlock, aBlockInfo )                   \
    {                                                               \
        STL_WRITE_INT16( (aBlockInfo), (stlChar*)(aBlock) + 9 );    \
    }

#define SMR_WRITE_BLOCK_INFO( aBlock, aBlockInfo )                  \
    {                                                               \
        STL_WRITE_INT16( (stlChar*)(aBlock) + 9, (aBlockInfo) );    \
    }

#define SMR_SET_BLOCK_INFO( aBlock, aFileEnd, aChained, aWrapNo, aWrite )       \
    {                                                                           \
        (aBlock)->mBlockInfo = (aFileEnd) | (aChained) | (aWrapNo) | (aWrite);  \
    }

#define SMR_SET_CHAINED_BLOCK( aBlock )                 \
    {                                                   \
        (aBlock)->mBlockInfo |= SMR_CHAINED_BLOCK_TRUE; \
    }

/**
 * @note mLsn의 옵셋은 항상 최상위에 위치해야 한다.
 *  <BR> mLsn은 Log Buffer에서 직접 casting해서 사용되는 경우가 있다.
 */
#define SMR_WRITE_MOVE_LOG_BLOCK_HDR( aDest, aSrc, aOffset )                            \
{                                                                                       \
    STL_WRITE_MOVE_INT64( (stlChar*)(aDest), &((aSrc)->mLsn),            aOffset );     \
    STL_WRITE_MOVE_INT8(  (stlChar*)(aDest), &((aSrc)->mLogCount),       aOffset );     \
    STL_WRITE_MOVE_INT16( (stlChar*)(aDest), &((aSrc)->mBlockInfo),      aOffset );     \
    STL_WRITE_MOVE_INT16( (stlChar*)(aDest), &((aSrc)->mFirstLogOffset), aOffset );     \
}

#define SMR_WRITE_LOG_BLOCK_HDR( aDest, aSrc )                                \
{                                                                             \
    STL_WRITE_INT64( ((stlChar*)(aDest) + 0),  &((aSrc)->mLsn) );             \
    STL_WRITE_INT8(  ((stlChar*)(aDest) + 8),  &((aSrc)->mLogCount) );        \
    STL_WRITE_INT16( ((stlChar*)(aDest) + 9),  &((aSrc)->mBlockInfo) );       \
    STL_WRITE_INT16( ((stlChar*)(aDest) + 11), &((aSrc)->mFirstLogOffset) );  \
    STL_ASSERT( (aSrc)->mFirstLogOffset < SMR_BLOCK_SIZE );                   \
}

#define SMR_READ_BLOCK_LSN( aLogBlockHdr ) ( *(smrLsn*)(aLogBlockHdr) )

#define SMR_BUFFER_BLOCK_SWITCH( aLogBuffer )             \
{                                                         \
    if( (aLogBuffer)->mBlockOffset > SMR_BLOCK_HDR_SIZE ) \
    {                                                     \
        (aLogBuffer)->mRearSbsn++;                        \
        (aLogBuffer)->mRearFileBlockSeqNo++;              \
        (aLogBuffer)->mBlockOffset = SMR_BLOCK_HDR_SIZE;  \
    }                                                     \
}

#define SMR_WRITE_FIRST_OFFSET( aLogBlock, aBlockOffset )              \
{                                                                      \
    STL_WRITE_INT16( ((stlChar*)(aLogBlock) + 11), &aBlockOffset );    \
}

#define SMR_GET_LOG_COUNT( aLogBlock ) (*((stlChar*)(aLogBlock) + 8))

#define SMR_IS_USABLE_LOG_FILE( aLogGroup )                                 \
(                                                                           \
    ((aLogGroup)->mLogGroupPersData.mState == SMR_LOG_GROUP_STATE_UNUSED)  ||   \
    ((aLogGroup)->mLogGroupPersData.mState == SMR_LOG_GROUP_STATE_INACTIVE)     \
)

#define SMR_IS_ONLINE_LOG_GROUP( aLogGroup )                                \
(                                                                           \
    ((aLogGroup)->mLogGroupPersData.mState == SMR_LOG_GROUP_STATE_ACTIVE)  ||   \
    ((aLogGroup)->mLogGroupPersData.mState == SMR_LOG_GROUP_STATE_CURRENT)      \
)

typedef struct smrTrans
{
    smrLsn       mBeginLsn;
    smrLsn       mCommitLsn;
    smlRid       mUndoSegRid;
    smlScn       mTransViewScn;
    smxlTransId  mTransId;
    stlInt32     mState;
    stlBool      mRepreparable;
} smrTrans;

#define SMR_INIT_TRANS_TABLE( aTransTable )          \
{                                                    \
    (aTransTable)->mBeginLsn = 0;                    \
    (aTransTable)->mCommitLsn = 0;                   \
    (aTransTable)->mUndoSegRid = SML_INVALID_RID;    \
    (aTransTable)->mTransId = SML_INVALID_TRANSID;   \
    (aTransTable)->mState = SMXL_STATE_IDLE;         \
    (aTransTable)->mRepreparable = STL_TRUE;         \
}

typedef struct smrLogCursor
{
    smrLogHdr        mLogHdr;
    stlChar        * mLogBody;
    smrLsn           mCurLsn;
    knlRegionMark    mMemMark;
    stlBool          mIsForRecover;
} smrLogCursor;

#define SMR_BLOCK_SEQ_FROM_OFFSET( aOffset, aBlockSize )    \
    ( (((stlInt64)(aOffset)) / (aBlockSize)) - 1 )

#define SMR_LOG_BUFFER_IDX( aLogStream, aLogFileNo ) \
    ( (aLogFileNo) % (aLogStream)->mLogBuffer->mBufferBlockCount )

#define SMR_MAX_TRANS_COUNT_IN_PIECE  ( 512 )

typedef enum
{
    SMR_LOGDATA_TYPE_BINARY,
    SMR_LOGDATA_TYPE_BLOCKARRAY
} smrLogDataType;

#define SMR_VALIDATE_LOGFILE_CREATION_TIME (0x00000001)
#define SMR_VALIDATE_LOGFILE_GROUPID       (0x00000002)
#define SMR_VALIDATE_LOGFILE_SEQUENCE      (0x00000004)
#define SMR_VALIDATE_LOGFILE_MASK          (0x00000007)

#define SMR_FLUSH_LOG_BLOCK_COUNT        (100000)

typedef struct smrFxLogBuffer
{
    stlInt64   mBufferSize;
    stlInt64   mBufferBlockCount;
    stlInt16   mBlockOffset;
    stlInt64   mFileSeqNo;
    stlInt32   mRearFileBlockSeqNo;
    stlInt32   mFrontFileBlockSeqNo;
    stlInt16   mLeftLogCountInBlock;
    smrSbsn    mRearSbsn;
    smrLsn     mRearLsn;
    smrSbsn    mFrontSbsn;
    smrLsn     mFrontLsn;
    stlInt64   mRearLidFileSeqNo;
    stlInt32   mRearLidBlockSeqNo;
    stlInt16   mRearLidBlockOffset;
} smrFxLogBuffer;

typedef struct smrFxPendingLogBuffer
{
    stlInt64   mBufferSize;
    stlInt64   mBufferBlockCount;
    smrSbsn    mRearPbsn;
    smrLsn     mRearLsn;
    smrSbsn    mFrontPbsn;
    smrLsn     mFrontLsn;
} smrFxPendingLogBuffer;

typedef struct smrFxLogStream
{
    stlInt32   mLogGroupCount;
    stlInt64   mCurFileSeqNo;
    stlInt16   mBlockSize;
    stlChar    mState[16];
    stlInt64   mFlushCount;
    stlInt64   mFlushTryCount;
    stlInt64   mWaitCountByBufferFull;
    stlInt64   mWaitCount4Sync;
    stlInt64   mBlockedLoggingCount;
} smrFxLogStream;

typedef struct smrFxLogStreamPathCtrl
{
    void *               mLogStreamIterator;
    smrLogStreamPersData mLogStreamPersData;
} smrFxLogStreamPathCtrl;


typedef struct smrFxLogGroup
{
    stlInt16   mLogGroupId;
    stlInt16   mOrdLogGroupId;
    stlInt64   mFileSeqNo;
    stlInt64   mFileSize;
    stlInt32   mLogMemberCount;
    stlInt64   mPrevLastLsn;
    stlInt64   mLogSwitchingSbsn;
    stlChar    mState[16];
} smrFxLogGroup;

typedef struct smrFxLogGroupPathCtrl
{
    void *               mLogGroupIterator;
    stlInt16             mOrdLogGroupId;
    smrLogGroupPersData  mLogGroupPersData;
} smrFxLogGroupPathCtrl;

typedef struct smrFxLogMember
{
    stlInt16   mLogGroupId;
    stlChar    mName[STL_MAX_FILE_PATH_LENGTH];
} smrFxLogMember;

typedef struct smrFxLogMemberPathCtrl
{
    void *                mLogGroupIterator;
    smrLogGroupPersData   mLogGroupPersData;
    void *                mLogMemberIterator;
    smrLogMemberPersData  mLogMemberPersData;
} smrFxLogMemberPathCtrl;

typedef struct smrFxRecoveryInfo
{
    smrLsn     mRedoLogLsn;
    smrLsn     mRedoLogFirstLsn;
    stlChar    mTransId[21];
    smlTbsId   mRedoLogSegTbsId;
    stlInt16   mRedoLogSegOffset;
    smlPid     mRedoLogSegPageId;
    stlInt16   mRedoLogPieceCount;
} smrFxRecoveryInfo;

typedef struct smrFxChkptInfo
{
    smrLsn     mLastChkptLsn;
    stlChar    mChkptState[9];
} smrFxChkptInfo;

#define SMR_DUMP_LOG_BUFFER_BLOCK_COUNT (2048)
#define SMR_DUMP_LOG_BODY_STRING_SIZE   (4000)

typedef struct smrFxLogPathCtrl
{
    stlFile    mFile;
    stlInt16   mCurPieceSeq;
    stlInt64   mCurBlockSeq;
    stlInt64   mCurLogSeq;
    stlInt32   mFileBlockSeq;
    stlInt32   mState;
    stlInt64   mLogHdrBlockSeq;
    stlChar  * mBuffer;
    stlChar    mLogBody[SMR_MAX_LOGBODY_SIZE];
    stlInt32   mMaxBlockCount;
    stlInt32   mBufferBlockSeq;
    stlInt32   mBufferBlockCount;
    stlInt16   mBlockOffset;
    stlInt16   mLeftLogCount;
    stlInt16   mLeftPieceCount;
    stlInt32   mLogBodyOffset;
    stlInt16   mBlockSize;
    stlInt16   mWrapNo;
    smrLogHdr  mLogHdr;
} smrFxLogPathCtrl;

typedef struct smrFxLogBlock
{
    stlInt64  mBlockSeq;
    smrLsn    mLsn;
    stlInt16  mLogCount;
    stlInt16  mWrapNo;
    stlBool   mFileEnd;
    stlBool   mChained;
    stlInt16  mFirstLogOffset;
    stlInt16  mUnusedSpace;
    stlChar   mHexBody[SMR_DUMP_LOG_BODY_STRING_SIZE];
    stlChar   mStrBody[SMR_DUMP_LOG_BODY_STRING_SIZE];
} smrFxLogBlock;

typedef struct smrFxLog
{
    stlInt64    mLogSeq;
    stlInt16    mPieceSeq;
    smrLsn      mLsn;
    stlInt64    mBlockSeq;
    stlInt64    mSegPhysicalId;
    smlRid      mRowId;
    smxlTransId mTransId;
    stlChar     mType[64];
    stlChar     mTarget[64];
    stlChar     mClass[64];
    stlInt32    mSize;
    stlBool     mPropagateLog;
    stlChar     mHexBody[SMR_DUMP_LOG_BODY_STRING_SIZE];
    stlChar     mStrBody[SMR_DUMP_LOG_BODY_STRING_SIZE];
} smrFxLog;

#define SMR_ARCHIVELOG_MODE_STRING_LENGTH    12

typedef struct smrFxArchivelog
{
    stlChar     mMode[SMR_ARCHIVELOG_MODE_STRING_LENGTH + 1];
    stlInt64    mLastArchivedLfsn;
    stlInt16    mDirCount;
    stlChar     mDir1[STL_MAX_FILE_PATH_LENGTH + 1];
    stlChar     mDir2[STL_MAX_FILE_PATH_LENGTH + 1];
    stlChar     mDir3[STL_MAX_FILE_PATH_LENGTH + 1];
    stlChar     mDir4[STL_MAX_FILE_PATH_LENGTH + 1];
    stlChar     mDir5[STL_MAX_FILE_PATH_LENGTH + 1];
    stlChar     mDir6[STL_MAX_FILE_PATH_LENGTH + 1];
    stlChar     mDir7[STL_MAX_FILE_PATH_LENGTH + 1];
    stlChar     mDir8[STL_MAX_FILE_PATH_LENGTH + 1];
    stlChar     mDir9[STL_MAX_FILE_PATH_LENGTH + 1];
    stlChar     mDir10[STL_MAX_FILE_PATH_LENGTH + 1];
    stlChar     mFormat[STL_MAX_FILE_NAME_LENGTH + 1];
} smrFxArchivelog;

typedef struct smrLogCursor4Disk
{
    stlInt32   mFileBlockSeq;
    stlChar  * mMisAlignedBuffer;
    stlChar  * mBuffer;
    stlInt32   mMaxBlockCount;
    stlInt32   mBufferBlockSeq;
    stlInt32   mBufferBlockCount;
    stlInt16   mBlockSize;
    stlInt16   mWrapNo;
    stlFile    mFile;
} smrLogCursor4Disk;

#define SMR_WRITE_HEX_STRING_TO_COLUMN( aColumn, aHexString, aHexOffset, aMaxSize ) \
{                                                                                   \
    do                                                                              \
    {                                                                               \
        if( (aHexOffset + 16) >= aMaxSize )                                         \
        {                                                                           \
            STL_READ_MOVE_BYTES( aColumn,                                           \
                                 aHexString,                                        \
                                 aMaxSize - aHexOffset,                             \
                                 aHexOffset );                                      \
        }                                                                           \
        else                                                                        \
        {                                                                           \
            STL_READ_MOVE_BYTES( aColumn,                                           \
                                 aHexString,                                        \
                                 16,                                                \
                                 aHexOffset );                                      \
        }                                                                           \
    }                                                                               \
    while( 0 );                                                                     \
}

#define SMR_WRITE_CHAR_STRING_TO_COLUMN( aColumn, aCharString, aCharOffset, aMaxSize )  \
{                                                                                       \
    do                                                                                  \
    {                                                                                   \
        if( (aCharOffset + 8) >= aMaxSize )                                             \
        {                                                                               \
            STL_READ_MOVE_BYTES( aColumn,                                               \
                                 aCharString,                                           \
                                 aMaxSize - aCharOffset,                                \
                                 aCharOffset );                                         \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            STL_READ_MOVE_BYTES( aColumn,                                               \
                                 aCharString,                                           \
                                 8,                                                     \
                                 aCharOffset );                                         \
        }                                                                               \
    }                                                                                   \
    while( 0 );                                                                         \
}

/** @} */
    
#endif /* _SMRDEF_H_ */
