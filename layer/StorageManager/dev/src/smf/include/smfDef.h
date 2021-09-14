/*******************************************************************************
 * smfDef.h
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


#ifndef _SMFDEF_H_
#define _SMFDEF_H_ 1

/**
 * @file smfDef.h
 * @brief Storage Manager Layer Data File Internal Definition
 */

/**
 * @defgroup smfInternal Component Internal Routines
 * @ingroup smf
 * @{
 */

/**
 * @addtogroup smrUndoLogType
 * @{
 */
#define SMF_UNDO_LOG_MEMORY_FILE_CREATE      0  /**< 메모리 파일 생성 언두 로그 */
#define SMF_UNDO_LOG_MEMORY_FILE_DROP        1  /**< 메모리 파일 삭제 언두 로그 */
#define SMF_UNDO_LOG_MEMORY_TBS_CREATE       2  /**< 메모리 테이블스페이스 생성 언두 로그 */
#define SMF_UNDO_LOG_MEMORY_TBS_DROP         3  /**< 메모리 테이블스페이스 삭제 언두 로그 */
#define SMF_UNDO_LOG_MEMORY_DATAFILE_ADD     4  /**< 메모리 데이터파일 추가 언두 로그 */
#define SMF_UNDO_LOG_MEMORY_DATAFILE_DROP    5  /**< 메모리 데이터파일 삭제 언두 로그 */
#define SMF_UNDO_LOG_MEMORY_TBS_RENAME       6  /**< 메모리 TBS 이름 변경 언두 로그 */
/** @} */

#define SMF_UNDO_LOG_MAX_COUNT             7

#define SMF_LOG_INVALID                       SMR_DATAFILE_LOG_MAX_COUNT

#define SMF_INVALID_PHYSICAL_TBS_ID           (0xFFFFFFFF)

/**
 * @brief Extent 내의 페이지 개수
 */
#define SMF_PAGE_COUNT_IN_EXT 32

/**
 * @brief Datafile Header structure
 */
typedef struct smfDatafileHeader
{
    stlInt32 mTbsPhysicalId;
    stlInt32 mDatafileId;
    smrLid   mCheckpointLid;   /** Datafile에 반영된 마지막 checkpoint Lid */
    smrLsn   mCheckpointLsn;   /** Datafile에 반영된 마지막 checkpoint Lsn */
    stlTime  mCreationTime;
} smfDatafileHeader;

#define SMF_INIT_DATAFILE_INFO( aDatafileInfo )                             \
    {                                                                       \
        stlMemset( (aDatafileInfo), 0x00, STL_SIZEOF(smfDatafileInfo) );    \
        aDatafileInfo->mIoGroupIdx = SMF_INVALID_IO_GROUP;                  \
    }

#define SMF_INIT_TBS_INFO( aTbsInfo )                                               \
    {                                                                               \
        stlMemset( &(aTbsInfo)->mTbsPersData, 0x00, STL_SIZEOF(smfTbsPersData) );   \
        (aTbsInfo)->mTbsPersData.mPageCountInExt = SMF_PAGE_COUNT_IN_EXT;           \
        (aTbsInfo)->mIsOnline4Scan = STL_TRUE;                                      \
        stlMemset( (aTbsInfo)->mDatafileInfoArray,                                  \
                   0x00,                                                            \
                   STL_SIZEOF(smfDatafileInfo*) * SML_DATAFILE_MAX_COUNT );         \
    }

typedef struct smfApplyBackupPlan
{
    smlTbsId  mTbsId;
    stlInt16  mLevel;
    stlChar   mObjectType;
    stlBool   mIsApplicable;
    stlBool   mIsFinished;
    stlChar   mPadding[1];
} smfApplyBackupPlan;

typedef struct smfTbsPendingArgs
{
    smxlTransId   mTransId;
    smlTbsId      mTbsId;
    stlBool       mAndDatafiles;
    stlInt8       mOfflineState;
    stlChar       mPadding[4];
    stlInt64      mRelationId;
} smfTbsPendingArgs;

typedef struct smfDatafilePendingArgs
{
    smxlTransId   mTransId;
    smlTbsId      mTbsId;
    smlDatafileId mDatafileId;
    stlChar       mAlign[4];
} smfDatafilePendingArgs;

typedef stlStatus (*smfCreateDatafileFunc) ( smxlTransId        aTransId,
                                             stlInt32           aTbsPhysicalId,
                                             stlInt32           aHintTbsId,
                                             smfTbsInfo       * aTbsInfo,
                                             smlDatafileAttr  * aDatafileAttr,
                                             stlBool            aUndoLogging,
                                             smfDatafileInfo ** aDatafileInfo,
                                             smlEnv           * aEnv );
typedef stlStatus (*smfPrepareDatafileFunc) ( smfTbsInfo       * aTbsInfo,
                                              stlFile          * aFile,
                                              smfCtrlBuffer    * aBuffer,
                                              smfDatafileInfo ** aDatafileInfo,
                                              smlEnv           * aEnv );
typedef stlStatus (*smfLoadDatafileFunc) ( smfTbsInfo      * aTbsInfo,
                                           smfDatafileInfo * aDatafileInfo,
                                           smlEnv          * aEnv );
typedef stlStatus (*smfDropDatafileFunc) ( smxlTransId       aTransId,
                                           smfTbsInfo      * aTbsInfo,
                                           smfDatafileInfo * aDatafileInfo,
                                           smlEnv          * aEnv );
typedef stlStatus (*smfExtendDatafileFunc)( smxmTrans        *aMiniTrans,
                                            smlTbsId          aTbsId,
                                            smlDatafileId     aDatafileId,
                                            smlEnv           *aEnv );
typedef stlStatus (*smfWritePageDatafileFunc)( smfTbsInfo       * aTbsInfo,
                                               smfDatafileInfo  * aDatafileInfo,
                                               stlUInt32          aPageSeqID,
                                               stlInt32           aPageCount,
                                               void             * aPage,
                                               smlEnv           * aEnv );
typedef stlStatus (*smfRecoveryDatafileFunc)( smxlTransId           aTransId,
                                              stlInt32              aTbsPhysicalId,
                                              stlInt32              aHintTbsId,
                                              smfDatafilePersData * aDatafilePersData,
                                              stlBool               aForRestore,
                                              stlBool               aClrLogging,
                                              smlEnv              * aEnv );

typedef struct smfDatafileFuncs
{
    smfCreateDatafileFunc    mCreate;
    smfPrepareDatafileFunc   mPrepare;
    smfLoadDatafileFunc      mLoad;
    smfDropDatafileFunc      mDrop;
    smfExtendDatafileFunc    mExtend;
    smfWritePageDatafileFunc mWritePage;
    smfRecoveryDatafileFunc  mCreateRedo;
    smfRecoveryDatafileFunc  mCreateUndo;
    smfRecoveryDatafileFunc  mDropRedo;
    smfRecoveryDatafileFunc  mDropUndo;
} smfDatafileFuncs;


#define SMF_INIT_DB_PERSISTENT_DATA( aDbPers, aTransTableSize, aUndoRelCount ) \
{                                                                              \
    stlInt16  sLevel;                                                          \
    stlMemset( (aDbPers), 0x00, STL_SIZEOF(smfDbPersData) );                   \
    (aDbPers)->mTransTableSize = aTransTableSize;                              \
    (aDbPers)->mUndoRelCount = aUndoRelCount;                                  \
    (aDbPers)->mTbsCount = 0;                                                  \
    (aDbPers)->mNewTbsId = 0;                                                  \
    for( sLevel = 0; sLevel < SML_INCREMENTAL_BACKUP_LEVEL_MAX; sLevel++ )     \
    {                                                                          \
        (aDbPers)->mIncBackup.mBackupLsn[sLevel] = SMR_INVALID_LSN;            \
    }                                                                          \
    (aDbPers)->mIncBackup.mBackupSeq = 0;                                      \
}

#define SMF_INIT_SYS_PERSISTENT_DATA( aSysPers )                    \
    {                                                               \
        stlMemset( (aSysPers), 0x00, STL_SIZEOF(smfSysPersData) );  \
        (aSysPers)->mServerState = SML_SERVER_STATE_NONE;           \
        (aSysPers)->mLastChkptLsn = SMR_INVALID_LSN;                \
        (aSysPers)->mBackupChunkCnt = 0;                            \
        (aSysPers)->mLogSectionOffset = 0;                          \
        (aSysPers)->mDbSectionOffset = 0;                           \
        (aSysPers)->mIncBackupSectionOffset = 0;                    \
        (aSysPers)->mTailSectionOffset = 0;                         \
    }

typedef struct smfParallelLoadArgs
{
    stlRingHead   mJobs;
    stlErrorStack mErrorStack;
} smfParallelLoadArgs;

typedef struct smfParallelLoadJob
{
    stlRingEntry  mJobLink;
    smlTbsId      mTbsId;
    smlDatafileId mDatafileId;
    stlInt16      mGroupId;
} smfParallelLoadJob;

typedef struct smfFile
{
    stlBool       mOpened;
    smlTbsId      mTbsId;
    stlUInt32     mRefCount;
    smlDatafileId mDatafileId;
    stlFile       mFile;
} smfFile;

#define SMF_OPEN_MAX_COUNT 1024

typedef struct smfFxTbs
{
    stlChar     mTbsName[STL_MAX_SQL_IDENTIFIER_LENGTH+1];
    smlTbsId    mTbsId;
    stlChar     mAttr[65];
    stlChar     mState[16];
    stlBool     mIsLogging;
    stlBool     mIsOnline;
    stlBool     mIsOnline4Scan;
    stlChar     mOfflineState[16];
    stlInt64    mExtSize;
    stlUInt16   mNewDatafileId;
    stlUInt32   mDatafileCount;
    stlInt64    mRelationId;
    stlInt32    mPageSize;
} smfFxTbs;

typedef struct smfFxDatafile
{
    smlTbsId   mTbsId;
    stlChar    mName[STL_MAX_FILE_PATH_LENGTH];
    stlChar    mState[16];
    stlChar    mShmState[16];
    stlTime    mTimestamp;
    smrLsn     mCheckpointLsn;
    stlUInt16  mId;
    stlInt16   mShmSegId;
    stlUInt64  mSize;
    stlUInt64  mUsableSize;
} smfFxDatafile;

typedef struct smfFxDatafileHeader
{
    stlChar    mName[STL_MAX_FILE_PATH_LENGTH];
    stlInt32   mTbsId;
    stlInt32   mDatafileId;
    smrLsn     mCheckpointLsn;
    stlTime    mCreationTime;
} smfFxDatafileHeader;

typedef struct smfFxDatafileCtrlPath
{
    smlTbsId      mTbsId;
    smlDatafileId mDatafileId;
} smfFxDatafileCtrlPath;

#define SMF_PAGE_DUMP_DATA_OFFSET_LENGTH (5)
#define SMF_PAGE_DUMP_DATA_LENGTH        (32)

typedef struct smfFxDiskPage
{
    stlChar   mOffSetStr[SMF_PAGE_DUMP_DATA_OFFSET_LENGTH + 1];
    stlChar   mData[(SMF_PAGE_DUMP_DATA_LENGTH * 2) + 1];
} smfFxDiskPage;

typedef struct smfFxDiskPageIterator
{
    smfDatafileHeader   mDatafileHeader;
    stlChar             mPageSnapshot[SMP_PAGE_SIZE];
    void              * mStartAddr;
    stlChar           * mCurAddr;
    stlUInt32           mSize;
    stlUInt32           mRemainSize;
} smfFxDiskPageIterator;

#define SMF_CTRL_STATE_LENGTH             16
#define SMF_CTRL_CHKPTLID_STRING_LENGTH   32

typedef struct smfFxCtrlSysSection
{
    stlChar    mCtrlFileName[STL_MAX_FILE_PATH_LENGTH + STL_MAX_FILE_NAME_LENGTH + 1];
    stlChar    mServerState[SMF_CTRL_STATE_LENGTH+1];
    stlChar    mDataStoreMode[SMF_CTRL_STATE_LENGTH+1];
    stlChar    mArchivelogMode[SMF_CTRL_STATE_LENGTH+1];
    stlChar    mChkptLid[SMF_CTRL_CHKPTLID_STRING_LENGTH+1];
    smrLsn     mCheckpointLsn;
    stlInt64   mLastInactiveLfsn;
    stlChar    mLogStreamState[SMF_CTRL_STATE_LENGTH+1];
    stlInt32   mLogGroupCount;
    stlInt16   mBlockSize;
    stlInt64   mCurFileSeqNo;
    stlUInt16  mTransTableSize;
    stlUInt16  mUndoRelCount;
    stlUInt16  mTbsCount;
    stlUInt16  mNewTbsId;
    stlChar    mCtrlFileState[SMF_CTRL_STATE_LENGTH + 1];
    stlBool    mIsPrimary;
} smfFxCtrlSysSection;

typedef struct smfFxCtrlPathCtrl
{
    stlFile           mFile;
    smfCtrlBuffer     mBuffer;
    stlInt32          mCtrlFileCount;
    stlInt32          mCurCtrlFileId;
    stlInt32          mLogGroupCount;
    stlInt32          mCurLogGroupId;
    stlInt32          mTbsCount;
    stlInt32          mCurTbsId;
    stlInt32          mFileCount;
    stlInt32          mCurFileId;
    stlInt64          mCurBackupSeq;
    smfSysPersData    mSysPersData;
    smfBackupIterator mBackupIterator;
    stlBool           mIsFileOpen;
    stlChar           mPad[7];
    stlInt64          mBackupChunk[SMF_BACKUP_CHUNK_SIZE / STL_SIZEOF(stlInt64)];
    stlChar           mCtrlFileName[STL_MAX_FILE_PATH_LENGTH + STL_MAX_FILE_NAME_LENGTH];
} smfFxCtrlPathCtrl;

typedef struct smfFxCtrlLogSection
{
    stlChar    mCtrlFileName[STL_MAX_FILE_PATH_LENGTH + STL_MAX_FILE_NAME_LENGTH + 1];
    stlChar    mLogGroupState[SMF_CTRL_STATE_LENGTH+1];
    stlInt32   mLogMemberCount;
    stlInt16   mLogGroupId;
    stlInt64   mFileSeqNo;
    stlInt64   mFileSize;
    smrLsn     mPrevLastLsn;
    stlChar    mMemberName[SMR_MAX_LOGMEMBER_COUNT][STL_MAX_FILE_PATH_LENGTH+1];
    stlChar    mCtrlFileState[SMF_CTRL_STATE_LENGTH + 1];
} smfFxCtrlLogSection;

#define SMF_CTRL_TBS_ATTR_STRING_LENGTH      64

typedef struct smfFxCtrlTbs
{
    stlChar     mCtrlFileName[STL_MAX_FILE_PATH_LENGTH + STL_MAX_FILE_NAME_LENGTH + 1];
    stlChar     mTbsName[STL_MAX_SQL_IDENTIFIER_LENGTH+1];
    smlTbsId    mTbsId;
    stlChar     mAttr[SMF_CTRL_TBS_ATTR_STRING_LENGTH+1];
    stlChar     mState[SMF_CTRL_STATE_LENGTH+1];
    stlBool     mIsLogging;
    stlBool     mIsOnline;
    stlChar     mOfflineState[SMF_CTRL_STATE_LENGTH+1];
    stlInt64    mExtSize;
    stlUInt16   mNewDatafileId;
    stlUInt32   mDatafileCount;
    stlInt64    mRelationId;
    stlChar     mCtrlFileState[SMF_CTRL_STATE_LENGTH + 1];
} smfFxCtrlTbs;

typedef struct smfFxCtrlDatafile
{
    stlChar    mCtrlFileName[STL_MAX_FILE_PATH_LENGTH + STL_MAX_FILE_NAME_LENGTH + 1];
    smlTbsId   mTbsId;
    stlUInt16  mID;
    stlChar    mName[STL_MAX_FILE_PATH_LENGTH+1];
    stlChar    mState[SMF_CTRL_STATE_LENGTH+1];
    stlBool    mAutoExtend;
    stlUInt64  mSize;
    stlUInt64  mNextSize;
    stlUInt64  mMaxSize;
    stlChar    mCtrlFileState[SMF_CTRL_STATE_LENGTH + 1];
} smfFxCtrlDatafile;

typedef struct smfFxBackupCtrlPath
{
    smlTbsId   mTbsId;
    stlBool    mTbsExist;
} smfFxBackupCtrlPath;

typedef struct smfFxBackup
{
    stlChar     mTbsName[STL_MAX_SQL_IDENTIFIER_LENGTH+1];
    smlTbsId    mTbsId;
    stlChar     mStatus[11];
} smfFxBackup;

typedef struct smfFxIncBackup
{
    stlInt64    mBackupSeq;
    stlInt16    mLevel;
    stlChar     mObjectType[STL_MAX_FILE_NAME_LENGTH + 1];
    stlChar     mBackupOption[13];
    stlUInt64   mBackupLsn;
    stlTime     mBeginTime;
    stlTime     mCompletionTime;
    stlChar     mFilePath[STL_MAX_FILE_PATH_LENGTH + STL_MAX_FILE_NAME_LENGTH + 1];
    stlChar     mCtrlFileName[STL_MAX_FILE_PATH_LENGTH + STL_MAX_FILE_NAME_LENGTH + 1];
    stlChar     mCtrlFileState[SMF_CTRL_STATE_LENGTH + 1];
    stlBool     mIsDeleted;
} smfFxIncBackup;

typedef struct smfFxIncBackupIterator
{
    stlChar           mCtrlFileName[STL_MAX_FILE_PATH_LENGTH + STL_MAX_FILE_NAME_LENGTH];
    stlChar           mBackupChunk[SMF_BACKUP_CHUNK_SIZE];
    smfSysPersData    mSysPersData;
    smfBackupIterator mBackupIterator;
    stlInt64          mBackupSeq;
    stlFile           mFile;
    stlBool           mFileOpen;
} smfFxIncBackupIterator;

/** @} */
    
#endif /* _SMFDEF_H_ */
