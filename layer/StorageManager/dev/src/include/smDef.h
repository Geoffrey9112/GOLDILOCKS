/*******************************************************************************
 * smDef.h
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


#ifndef _SMDEF_H_
#define _SMDEF_H_ 1

#include <smrLogDef.h>
#include <smpColumnDef.h>
#include <smgClassDef.h>

/**
 * @defgroup smInternal Storage Manager Layer Internal Routines
 * @internal
 * @{
 */

/**
 * @brief 트랜잭션 아이디
 */
typedef stlInt64 smxlTransId;

/**
 * @addtogroup smkl
 * @{
 */

#define SMKL_DEADLOCK_CHECK_INTERVAL  (100000)  /**< Deadlock check interval ( usec단위 ) */

#define SMKL_MAX_LOCKITEM_COUNT_IN_LOG   ( 256 )

#define SMKL_ITEM_RELATION    1    /**< Lock Item이 Relation이다. */
#define SMKL_ITEM_RECORD      2    /**< Lock Item이 Record이다. */

typedef struct smklItemLog
{
    stlUInt32     mGrantedMode;      /**< 대표락 모드 @see smlLockMode */
    stlInt16      mItemType;         /**< Lock Item Type ( Relation or Record )*/
    stlChar       mPadding[2];       /**< padding */
    smlRid        mRid;              /**< relation identifier or record identifier */
} smklItemLog;

typedef struct smklLockLog
{
    stlXid        mXid;
    stlInt64      mLockCount;
    smklItemLog   mLockItemArray[SMKL_MAX_LOCKITEM_COUNT_IN_LOG];
} smklLockLog;

typedef struct smklSystemInfo
{
    stlUInt32    mLockTableSize;
    stlUInt32    mLockWaitTableSize;
    stlUInt32    mLockHashTableSize;
} smklSystemInfo;
    
/** @} */

/**
 * @addtogroup smb
 * @{
 */

/**
 * @brief 최초 생성되는 Dictionary Table ID
 */
#define SMB_FIRST_DICTIONARY_TABLE_ID ( (smlRid) { 0, 0, 12 } )

#define SMB_DEFAULT_SYSTEM_MEMORY_DICT_TBS_EXTSIZE ( 8 * SMP_PAGE_SIZE )
#define SMB_DEFAULT_SYSTEM_MEMORY_UNDO_TBS_EXTSIZE ( 8 * SMP_PAGE_SIZE )
#define SMB_DEFAULT_SYSTEM_MEMORY_DATA_TBS_EXTSIZE ( 32 * SMP_PAGE_SIZE )
#define SMB_DEFAULT_SYSTEM_MEMORY_TEMP_TBS_EXTSIZE ( 32 * SMP_PAGE_SIZE )

/** @} */

/**
 * @addtogroup sms
 * @{
 */

#define SMS_SEGMENT_MAP_RID     ( (smlRid) { 0, 0, 1 } )

#define SMS_SEGMENT_MAP_SEGMENT_TYPE SML_SEG_TYPE_MEMORY_BITMAP

#define SMS_MAKE_SEGMENT_ID( aSegmentId, aSegmentRid )             \
{                                                                  \
    stlMemcpy( (aSegmentId), (aSegmentRid), STL_SIZEOF(smlRid) );  \
}

/**
 * @brief Segment가 유지해야 하는 최소 Extent 개수
 */
#define SMS_MIN_EXTENT_COUNT  (2)

/**
 * @brief Search Hint Context에서 관리되는 최대 Data Page 힌트개수
 */
#define SMS_MAX_PAGE_HINT_COUNT (4)

/**
 * @brief 반복적으로 Insertable 공간을 찾고자할때 필요한 힌트 정보
 */
typedef struct smsSearchHint
{
    stlBool  mIsValid;
    stlInt32 mHintCount;
    stlInt32 mCurHintPos;
    smlPid   mDataPid[SMS_MAX_PAGE_HINT_COUNT];
    smlRid   mLeafRid[SMS_MAX_PAGE_HINT_COUNT];
} smsSearchHint;

#define SMS_INIT_SEARCH_HINT( aSearchHint ) ( (aSearchHint)->mIsValid = STL_FALSE )

/**
 * @defgroup smsSegState Segment State
 * @ingroup sms
 * @{
 */
#define SMS_STATE_ALLOC_HDR   (1)    /**< Segment Header Page가 할당된 상태 */
#define SMS_STATE_ALLOC_BODY  (2)    /**< Segment Body가 할당된 상태 */
/** @} */

/**
 * @brief Segment warmup entry
 */
typedef struct smsWarmupEntry
{
    void        * mSegmentMapHandle;
    stlRingHead   mIrrecoverableSegmentList; /**< Irrecoverable Segment List */
} smsWarmupEntry;

/**
 * Irrecoverable segment item
 */
typedef struct smsIrrecoverableSegment
{
    stlInt64     mSegmentId;
    stlRingEntry mSegmentLink;
} smsIrrecoverableSegment;

#define SMS_INITIAL_DEFAULT( aPageCountInExt )  \
    (SMP_PAGE_SIZE * aPageCountInExt)
#define SMS_NEXT_DEFAULT( aPageCountInExt ) \
    (SMP_PAGE_SIZE * aPageCountInExt)
#define SMS_MINSIZE_DEFAULT( aPageCountInExt )  \
    (SMP_PAGE_SIZE * aPageCountInExt)
#define SMS_MAXSIZE_DEFAULT( aPageCountInExt )  \
    (((stlInt64)aPageCountInExt) * SMP_PAGE_SIZE * STL_INT32_MAX)

/** @} */

/**
 * @addtogroup smq
 * @{
 */

#define SMQ_SEQUENCE_MAP_RID                ( (smlRid) { 0, 0, 11 } )
#define SMQ_SESSION_SEQUENCE_BUCKET_COUNT   (7)

typedef struct smqSessSeqKey
{
    stlInt64 mId;
    smlScn   mValidScn;
} smqSessSeqKey;

typedef struct smqSessSeqItem
{
    stlRingEntry  mLink;
    smqSessSeqKey mSeqKey;
    stlInt64      mSeqValue;
} smqSessSeqItem;

/** @} */

/**
 * @addtogroup smg
 * @{
 */

typedef stlInt8 smgComponentClass;

/**
 * @brief 주어진 버퍼에서 Record ID를 읽는다.
 */
#define SMG_READ_MOVE_RID( aRid, aSrc, aOffset )                    \
    {                                                               \
        stlMemcpy( aRid, aSrc + aOffset, STL_SIZEOF( smlRid ) );    \
        aOffset += STL_SIZEOF( smlRid );                            \
    }

/**
 * @brief Record ID를 주어진 버퍼공간에 기록한다.
 */
#define SMG_WRITE_MOVE_RID( aDest, aRid, aOffset )                  \
    {                                                               \
        stlMemcpy( aDest + aOffset, aRid, STL_SIZEOF( smlRid ) );   \
        aOffset += STL_SIZEOF( smlRid );                            \
    }

/**
 * @brief Record ID를 주어진 버퍼공간에 기록한다.
 */
#define SMG_WRITE_RID( aRid, aSrc )                     \
    {                                                   \
        stlMemcpy( aRid, aSrc, STL_SIZEOF( smlRid ) );  \
    }


/**
 * @brief Pending Operation Identifier
 */
typedef enum
{
    SMG_PEND_CREATE_TABLESPACE,    /**< Create Tablespace */
    SMG_PEND_DROP_TABLESPACE,      /**< Drop Tablespace */
    SMG_PEND_CREATE_DATAFILE,      /**< Create Datafile */
    SMG_PEND_ADD_DATAFILE,         /**< Add Datafile */
    SMG_PEND_RENAME_DATAFILE,      /**< Rename Datafile */
    SMG_PEND_DROP_DATAFILE,        /**< Drop Datafile */
    SMG_PEND_CREATE_TABLE,         /**< Create Table */
    SMG_PEND_DROP_TABLE,           /**< Drop Table */
    SMG_PEND_FREE_LOCK,            /**< Free Lock */
    SMG_PEND_DROP_PENDING_TABLE,   /**< Drop Pending Segment */
    SMG_PEND_CREATE_INDEX,         /**< Create Index */
    SMG_PEND_DROP_INDEX,           /**< Drop Index */
    SMG_PEND_UNUSABLE_SEGMENT,     /**< Unusable Segment */
    SMG_PEND_USABLE_SEGMENT,       /**< Usable Segment */
    SMG_PEND_SHRINK_MEMORY,        /**< Shink Shared Database Page Memory */
    SMG_PEND_ADD_EXTENT_BLOCK,     /**< Add Extent Block */
    SMG_PEND_ONLINE_TABLESPACE,    /**< Online Tablespace */
    SMG_PEND_OFFLINE_TABLESPACE,   /**< Offline Tablespace */
    SMG_PEND_SET_SEGMENT,          /**< Set Pending Segment Identifier */
    SMG_PEND_ADD_LOG_GROUP,        /**< Add Log Group */
    SMG_PEND_ADD_LOG_MEMBER,       /**< Add Log Member */
    SMG_PEND_ADD_LOG_FILE,         /**< Add Log File */
    SMG_PEND_DROP_LOG_GROUP,       /**< Drop Log Group */
    SMG_PEND_DROP_LOG_MEMBER,      /**< Drop Log Member */
    SMG_PEND_RENAME_LOG_FILE,      /**< Rename Log File */
    SMG_PEND_DROP_SEQUENCE,        /**< Drop Sequence */
    SMG_MAX_PEND_COUNT
} smgPendOpId;
    
typedef struct smgPendOp smgPendOp;

/**
 * @brief Pending Operation의 Function 정의
 */
typedef stlStatus (*smgPendOpFunc) ( stlUInt32   aActionFlag,
                                     smgPendOp * aPendOp,
                                     smlEnv    * aEnv );

/**
 * @brief Pending Operation 정보를 위한 구조체
 */
struct smgPendOp
{
    smgPendOpId    mPendOpId;      /**< Pending Operation Identifier */
    stlUInt32      mActionFlag;    /**< Pending Action Flag @see smgPendActionFlags */
    stlInt64       mTimestamp;     /**< Statement unique identifier in transaction */
    void         * mArgs;          /**< Arguments */
    void         * mEventMem;      /**< Event Memory */
    stlUInt32      mEventMemSize;  /**< Event Memory Size */
    stlRingEntry   mOpLink;        /**< Next pending operation link */ 
};

/**
 * @brief Operation heap mamory size
 */
#define SMG_OPERATION_HEAPMEM_CHUNK_SIZE     ( 8 * 1024 * 12 )
/**
 * @brief Database level mamory size
 */
#define SMG_DATABASE_SHMMEM_CHUNK_SIZE       ( 1024 * 1024 )
/**
 * @brief Session mamory size
 */
#define SMG_SESSION_MEM_CHUNK_SIZE           ( 8 * 1024 )
/**
 * @brief Parent statement mamory size
 */
#define SMG_PARENT_STATEMENT_MEM_CHUNK_SIZE  ( 32 * 1024 )
/**
 * @brief Statement mamory size
 */
#define SMG_STATEMENT_MEM_CHUNK_SIZE         ( 16 * 1024 )
/**
 * @brief Session dynamic mamory size
 */
#define SMG_SESSION_DYNAMIC_CHUNK_SIZE       ( 1024 * 1024 )

/** @} */

/**
 * @addtogroup smr
 * @{
 */

/**
 * @brief Log Identifier 구조체
 */
typedef struct smrLid
{
    stlInt64 mFileSeqNo;
    stlInt32 mBlockInfo1;   /** BLOCK_SEQUENCE_NO 28 bit
                                BLOCK_OFFSET      4 bit */
    stlInt16 mPad1;         /** for padding */
    stlUInt8 mBlockInfo2;   /** BLOCK_OFFSET      8 bit */
    stlInt8  mPad2;         /** for padding */ 
} smrLid;

/**
 * @brief Stream별 Block 순차 번호
 */
typedef stlInt64 smrSbsn;

#define SMR_INVALID_SBSN (-1)     /**< Invalid Stream BlockSequence Number */
#define SMR_INVALID_LPSN (-1)     /**< Invalid Log Piece Sequence Number */
#define SMR_MAX_LPSN     (0x7FFF) /**< Maximum Log Piece Sequence Number */

#define SMR_BLOCK_OFFSET_PART1_MASK      (0x0000000F)
#define SMR_BLOCK_OFFSET_PART2_MASK      (0x000000FF)
#define SMR_BLOCK_OFFSET_PART1_PRECISION (4)
#define SMR_BLOCK_OFFSET_PART2_PRECISION (8)

#define SMR_FILE_SEQ_NO( aLid )  ( (aLid)->mFileSeqNo )
#define SMR_BLOCK_SEQ_NO( aLid ) ( (aLid)->mBlockInfo1 >> SMR_BLOCK_OFFSET_PART1_PRECISION )
#define SMR_BLOCK_OFFSET( aLid ) ( (((aLid)->mBlockInfo1 & SMR_BLOCK_OFFSET_PART1_MASK) <<      \
                                    SMR_BLOCK_OFFSET_PART2_PRECISION) +                         \
                                   (aLid)->mBlockInfo2 )

#define SMR_MAKE_LID( aLid, aFileSeqNo, aBlockSeqNo, aBlockOffset )                 \
    {                                                                               \
        (aLid)->mFileSeqNo   = (aFileSeqNo);                                        \
        (aLid)->mBlockInfo1  = (aBlockSeqNo) << SMR_BLOCK_OFFSET_PART1_PRECISION;   \
        (aLid)->mBlockInfo1 |= (aBlockOffset) >> SMR_BLOCK_OFFSET_PART2_PRECISION;  \
        (aLid)->mBlockInfo2  = (aBlockOffset) & SMR_BLOCK_OFFSET_PART2_MASK;        \
        (aLid)->mPad1        = 0;                                                   \
        (aLid)->mPad2        = 0;                                                   \
    }

/**
 * @brief 로그 순차 번호
 */
typedef stlInt64 smrLsn;

/**
 * @brief Invalid Lsn 번호
 */
#define SMR_INVALID_LSN              STL_INT64_C(0xFFFFFFFFFFFFFFFF)
/**
 * @brief 초기 Lsn
 */
#define SMR_INIT_LSN                 STL_INT64_C(0xFFFFFFFFFFFFFFFE)
/**
 * @brief Invalid Log File 번호
 */
#define SMR_INVALID_LOG_FILE_SEQ_NO  (-1)
/**
 * @brief Invalid Log Block Sequence 번호
 */
#define SMR_INVALID_LOG_BLOCK_SEQ_NO (-1)

/**
 * @brief Log Identifier 초기화 매크로
 */
#define SMR_INIT_LID( aLid )  SMR_MAKE_LID( aLid,0,0,0 )

/**
 * @brief Log Identifier의 Invalid 검사 매크로
 */
#define SMR_IS_INVALID_LID( aLid )                  \
    ((aLid)->mFileSeqNo == SMR_INVALID_LOG_FILE_SEQ_NO)

/**
 * @brief Log Identifier의 Valid 검사 매크로
 */
#define SMR_IS_VALID_LID( aLid )                    \
    ((aLid)->mFileSeqNo != SMR_INVALID_LOG_FILE_SEQ_NO)

/**
 * @brief Invalid Log Identifier를 설정하기 위한 매크로
 */
#define SMR_SET_INVALID_LID( aLid )                   \
{                                                     \
    (aLid)->mFileSeqNo = SMR_INVALID_LOG_FILE_SEQ_NO; \
}

/**
 * @brief Invalid Log Identifier를 설정하기 위한 매크로
 */
#define SMR_INVALID_LID (smrLid){SMR_INVALID_LOG_FILE_SEQ_NO,0,0,0,0}

/**
 * @brief 최대 Log Piece의 크기
 */
#define SMR_MAX_LOGPIECE_SIZE (SMP_PAGE_SIZE * 3)
/**
 * @brief 최대 Log Member의 수
 */
#define SMR_MAX_LOGMEMBER_COUNT        (5)
/**
 * @brief 최소 Log Member의 수
 */
#define SMR_MIN_LOGMEMBER_COUNT        (1)

/**
 * @defgroup smrRedoTargetType Redo Target Type
 * @ingroup smr
 * @{
 */
#define SMR_REDO_TARGET_PAGE            1   /**< Database Page Redo */
#define SMR_REDO_TARGET_UNDO_SEGMENT    2   /**< Undo Segment Redo */
#define SMR_REDO_TARGET_TRANSACTION     3   /**< Undo Segment Redo */
#define SMR_REDO_TARGET_CTRL            4   /**< Control File Redo */
#define SMR_REDO_TARGET_NONE            5   /**< no need recovery */
#define SMR_REDO_TARGET_TEST            6   /**< for test */
/** @} */

/**
 * @brief Recovery를 위한 Redo/Undo 함수 정의
 */
typedef struct smpHandle smpHandle;
typedef struct smxmTrans smxmTrans;
typedef stlStatus (*smrRedoFunc)( smlRid    * aDataRid,
                                  void      * aData,
                                  stlUInt16   aDataSize,
                                  smpHandle * aPageHandle,
                                  smlEnv    * aEnv );
typedef stlStatus (*smrMtxUndoFunc)( smlRid    * aDataRid,
                                     void      * aData,
                                     stlUInt16   aDataSize,
                                     smpHandle * aPageHandle,
                                     smlEnv    * aEnv );

typedef struct smrRedoVector
{
    smrRedoFunc      mRedoFunc;
    smrMtxUndoFunc   mMtxUndoFunc;
    stlChar        * mLogName;
} smrRedoVector;

/**
 * @brief 로그에 저장될수 있는 최대 Log Piece 개수
 */
#define SMR_LOG_HDR_SIZE ( STL_SIZEOF( smrLogHdr ) )

#define SMR_WRITE_MOVE_LOG_HDR( aDest, aSrc, aOffset )                             \
{                                                                                  \
    stlMemcpy( (aDest) + (aOffset), (aSrc), STL_SIZEOF( smrLogHdr ) );             \
    (aOffset) += STL_SIZEOF( smrLogHdr );                                          \
}

/**
 * @note mLsn의 옵셋은 항상 최상위에 위치해야 한다.
 *  <BR> mLsn은 Pending Log Buffer에서 직접 casting해서 사용되는 경우가 있다.
 */
#define SMR_WRITE_LOG_HDR( aDest, aSrc )                                    \
{                                                                           \
    stlMemcpy( (aDest), (aSrc), STL_SIZEOF( smrLogHdr ) );                  \
}

/**
 * @brief Log Piece Header 구조체를 Byte Buffer에 저장하는 매크로
 */
#define SMR_WRITE_LOGPIECE_HDR( aDest, aSrc )                               \
{                                                                           \
    stlMemcpy( (aDest), (aSrc), STL_SIZEOF( smrLogPieceHdr) );              \
}

/**
 * @brief Log Piece Header 구조체를 Byte Buffer에 저장하고, aOffset을 증가시키는 매크로
 */
#define SMR_WRITE_MOVE_LOGPIECE_HDR( aDest, aSrc, aOffset )                         \
{                                                                                   \
    stlMemcpy( (aDest) + (aOffset), (aSrc), STL_SIZEOF( smrLogPieceHdr) );          \
    (aOffset) += STL_SIZEOF( smrLogPieceHdr);                                       \
}

/**
 * @brief Log Piece Header의 크기
 */
#define SMR_LOGPIECE_HDR_SIZE ( STL_SIZEOF( smrLogPieceHdr) )

/**
 * @brief Byte Buffer로 부터 Log Identifier를 구성하는 매크로
 */
#define SMR_READ_LID( aDest, aSrc )                                       \
{                                                                         \
    STL_WRITE_INT64( &((aDest)->mFileSeqNo),  ((stlChar*)(aSrc) + 0) );   \
    STL_WRITE_INT32( &((aDest)->mBlockInfo1), ((stlChar*)(aSrc) + 8) );   \
    STL_WRITE_INT8(  &((aDest)->mBlockInfo2), ((stlChar*)(aSrc) + 12) );  \
    (aDest)->mPad1 = 0;                                                   \
    (aDest)->mPad2 = 0;                                                   \
}

/**
 * @brief Log Identifier를 Byte Buffer에 저장하는 매크로
 */
#define SMR_WRITE_LID( aDest, aSrc )                                      \
{                                                                         \
    STL_WRITE_INT64( ((stlChar*)(aDest) + 0), &((aSrc)->mFileSeqNo) );    \
    STL_WRITE_INT32( ((stlChar*)(aDest) + 8), &((aSrc)->mBlockInfo1) );   \
    STL_WRITE_INT8(  ((stlChar*)(aDest) + 12)  &((aSrc)->mBlockInfo2) );  \
}

/**
 * @brief Byte Buffer로 부터 Log Identifier를 구성하고, aOffset을 증가시키는 매크로
 */
#define SMR_READ_MOVE_LID( aDest, aSrc, aOffset )                                \
{                                                                                \
    STL_READ_MOVE_INT64( &((aDest)->mFileSeqNo),  (stlChar*)(aSrc), aOffset );   \
    STL_READ_MOVE_INT32( &((aDest)->mBlockInfo1), (stlChar*)(aSrc), aOffset );   \
    STL_READ_MOVE_INT8(  &((aDest)->mBlockInfo2), (stlChar*)(aSrc), aOffset );   \
    (aDest)->mPad1 = 0;                                                          \
    (aDest)->mPad2 = 0;                                                          \
}

/**
 * @brief Log Identifier를 Byte Buffer에 저장하고, aOffset을 증가시키는 매크로
 */
#define SMR_WRITE_MOVE_LID( aDest, aSrc, aOffset )                              \
{                                                                               \
    STL_WRITE_MOVE_INT64( (stlChar*)(aDest), &((aSrc)->mFileSeqNo),  aOffset ); \
    STL_WRITE_MOVE_INT32( (stlChar*)(aDest), &((aSrc)->mBlockInfo1), aOffset ); \
    STL_WRITE_MOVE_INT8(  (stlChar*)(aDest), &((aSrc)->mBlockInfo2), aOffset ); \
}

/**
 * @brief argument for checkpoint event
 */
typedef struct smrChkptArg
{
    stlInt64    mLogfileSeq;
    stlInt32    mFlushBehavior;
    stlChar     mPadding[4];
} smrChkptArg;

typedef struct smrChkptInfo
{
    smrLsn            mLastChkptLsn;
    stlUInt32         mChkptState;   /**< @see smrCheckpointState */
    stlChar           mPadding[4];
} smrChkptInfo;

#define SMR_RECOVERY_PHASE_NONE        STL_UINT64_C(0)
#define SMR_RECOVERY_PHASE_PREVENTED   STL_UINT64_C(1)
#define SMR_RECOVERY_PHASE_BEGIN       STL_UINT64_C(2)
#define SMR_RECOVERY_PHASE_ANALYSIS    STL_UINT64_C(3)
#define SMR_RECOVERY_PHASE_REDO        STL_UINT64_C(4)
#define SMR_RECOVERY_PHASE_UNDO        STL_UINT64_C(5)
#define SMR_RECOVERY_PHASE_DONE        STL_UINT64_C(6)

typedef struct smrRecoveryInfo
{
    smrLsn            mLogLsn;
    smxlTransId       mLogTransId;
    smlRid            mLogSegRid;
    stlUInt64         mRecoveryPhase;
    stlInt16          mLogLpsn;
    stlInt16          mLogPieceCount;
    stlInt16          mLogMirrorStat;
    stlChar           mPadding[2];
    /* Media Recovery Info */
    void            * mMediaRecoveryInfo;  /**< Media Recovery를 수행하기 위한 정보 */
} smrRecoveryInfo;

#define SMR_RECOVERY_PAGE_LIST_MAX_COUNT (256)

typedef struct smrCorruptedPageInfo
{
    stlInt32     mPageCount;
    stlChar      mPadding[4];
    smlPid       mPages[SMR_RECOVERY_PAGE_LIST_MAX_COUNT];
    stlRingEntry mCorruptedPageLink;
} smrCorruptedPageInfo;

typedef smlMediaRecoveryDatafile smrRecoveryDatafileInfo;

typedef struct smrMediaRecoveryInfo
{
    void                 * mLogBuffer;           /**< Log buffer for media recovery */
    void                 * mBufferBlock;         /**< Log buffer for media recovery */
    smrLsn                 mCurLsn;              /**< Redo를 수행할 Log Lsn */
    smrLsn                 mPrevLastLsn;         /**< Redo가 수행중인 Logfile의 Prev Last Lsn */
    stlInt64               mMaxBlockCount;       /**< Logfile의 최대 block 수 */
    stlInt16               mBlockSize;           /**< Log block size         */
    smlTbsId               mTbsId;               /**< Media recovery 대상 Tbs Id */
    stlUInt8               mMediaRecoveryType;   /**< Media recovery type */
    stlUInt8               mObjectType;          /**< Media Recovery Object Type */
    stlUInt8               mImrOption;           /**< Incomplete Recovery Option */
    stlUInt8               mImrCondition;        /**< Incomplete Recovery Condition */
    stlUInt8               mUsingBackupCtrlfile; /**< Backup Controlfile 사용 여부 */
    stlChar                mPadding[7];
    stlUInt64              mMediaRecoveryPhase;  /**< Media recovery phase */
    stlInt64               mUntilValue;          /**< UNTIL CHANGE 시 until value */
    stlChar                mLogfileName[STL_MAX_FILE_PATH_LENGTH]; /**< Logfile name */
    void                 * mTransTable;          /**< Transaction table for media recovery */
    void                 * mOrgTransTable;       /**< Orgininal Transaction Table */
    stlInt32               mOrgTransTableSize;
    knlRegionMark          mMemMark;             /**< Media recovery를 위한 mem mark */
    smrLsn                 mRestoredChkptLsn; /**< 각 tbs의 Backup 이용한 restored Checkpoint Lsn */
    smrLid                 mRestoredChkptLid; /**< 각 tbs의 Backup 이용한 restored Checkpoint Lid */
    smrLsn                 mLastChkptLsn;
    smrLid                 mLastChkptLid;
    stlRingHead            mDatafileList;     /**< Datafile Recovery Info  */
} smrMediaRecoveryInfo;

#define SMR_GET_MEDIA_RECOVERY_INFO(aEnv)                               \
    ( (smrMediaRecoveryInfo*)(SML_SESS_ENV(aEnv)->mMediaRecoveryInfo) )

#define SMR_SET_MEDIA_RECOVERY_INFO(aEnv, aMediaRecoveryInfo)               \
{                                                                           \
    SML_SESS_ENV(aEnv)->mMediaRecoveryInfo = ((void *)aMediaRecoveryInfo);  \
}

#define SMR_MEDIA_RECOVERY_SET_FIELD( aEnv, aField, aValue )       \
{                                                                  \
    SMR_GET_MEDIA_RECOVERY_INFO(aEnv)->m ## aField = (aValue);     \
}

#define SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, aField )        \
    ( SMR_GET_MEDIA_RECOVERY_INFO(aEnv)->m ## aField )

#define SMR_MEDIA_RECOVERY_IS_END_OF_FILE( aEnv )                   \
    ( SMR_GET_MEDIA_RECOVERY_INFO(aEnv)->mEndOfFile == STL_TRUE )

/** @} */

/**
 * @addtogroup sma
 * @{
 */

/**
 * @brief Transaction Commit Scn을 얻는다.
 * @param[in] aStatement Statement Handle
 */
#define SMA_GET_COMMIT_SCN( aStatement )     ( (aStatement)->mCommitScn )
/**
 * @brief Transaction Identifier를 얻는다.
 * @param[in] aStatement Statement Handle
 */
#define SMA_GET_TRANS_ID( aStatement )       ( (aStatement)->mTransId )
/**
 * @brief Transaction View Scn을 얻는다.
 * @param[in] aStatement Statement Handle
 */
#define SMA_GET_TRANS_VIEW_SCN( aStatement ) ( (aStatement)->mTransViewScn )
/**
 * @brief Transaction Change Number을 얻는다.
 * @param[in] aStatement Statement Handle
 */
#define SMA_GET_TCN( aStatement ) ( (aStatement)->mTcn )
/**
 * @brief Statement의 System Change Number을 얻는다.
 * @param[in] aStatement Statement Handle
 */
#define SMA_GET_STMT_VIEW_SCN( aStatement ) ( (aStatement)->mScn )

/**
 * @brief DDL Lock timeout을 사용해야 하는지 여부를 얻는다.
 * @param[in] aStatement Statement Handle
 */
#define SMA_GET_LOCK_TIMEOUT( aStatement ) ( (aStatement)->mLockTimeout )

/** @} */

/**
 * @addtogroup smf
 * @{
 */

#define SMF_DIRECT_IO_DEFAULT_BLOCKSIZE   (512)

typedef enum smfCtrlSection
{
    SMF_CTRL_SECTION_SYSTEM = 0,  /**< SYSTEM Section in Control File */
    SMF_CTRL_SECTION_INC_BACKUP,  /**< Incremental Backup Section in Control File */ 
    SMF_CTRL_SECTION_LOG,         /**< LOG Section in Control File */
    SMF_CTRL_SECTION_DB           /**< DB Section in Control File */
} smfCtrlSection;

/**
 * @defgroup smfTbsState Tablespace state
 * @ingroup smfInternal
 * @{
 */
#define SMF_TBS_STATE_INIT      0
#define SMF_TBS_STATE_CREATING  1
#define SMF_TBS_STATE_CREATED   2
#define SMF_TBS_STATE_DROPPING  3
#define SMF_TBS_STATE_AGING     4
#define SMF_TBS_STATE_DROPPED   5
/** @} */

/**
 * @defgroup smfDatafileState Datafile state
 * @ingroup smfInternal
 * @{
 */
#define SMF_DATAFILE_STATE_INIT      0
#define SMF_DATAFILE_STATE_CREATING  1
#define SMF_DATAFILE_STATE_CREATED   2
#define SMF_DATAFILE_STATE_DROPPING  3
#define SMF_DATAFILE_STATE_AGING     4
#define SMF_DATAFILE_STATE_DROPPED   5
/** @} */

/**
 * @defgroup smfOfflineState Offline tablespace state
 * @ingroup smfInternal
 * @{
 */
#define SMF_OFFLINE_STATE_NULL           0
#define SMF_OFFLINE_STATE_CONSISTENT     1
#define SMF_OFFLINE_STATE_INCONSISTENT   2
/** @} */

/**
 * @defgroup smfDatafileLoadState Datafile Loading state
 * @ingroup smfInternal
 * @{
 */
#define SMF_DATAFILE_SHM_STATE_NULL        0
#define SMF_DATAFILE_SHM_STATE_ALLOCATED   1
#define SMF_DATAFILE_SHM_STATE_LOADED      2
#define SMF_DATAFILE_SHM_STATE_PCH_INIT    3
/** @} */

/**
 * @defgroup smfControlFileIo  Control file io size
 * @remark Controlfile에 대해 direct io를 수행하기 위한 block, buffer, Max Read size
 * @ingroup smfInternal
 * @{
 */
#define SMF_CONTROLFILE_IO_BLOCK_SIZE     SMF_DIRECT_IO_DEFAULT_BLOCKSIZE
#define SMF_CONTROLFILE_IO_BUFFER_SIZE    (8192)
#define SMF_CONTROLFILE_IO_MAX_READ_SIZE  (8192 * 512)
/** @} */

typedef struct smfCtrlBuffer
{
    stlChar     mBlock[SMF_CONTROLFILE_IO_BLOCK_SIZE + SMF_CONTROLFILE_IO_BUFFER_SIZE];
    stlChar   * mBuffer;
    stlInt32    mMaxSize;
    stlInt32    mOffset;
    stlInt32    mReadSize;
} smfCtrlBuffer;

#define SMF_INIT_CTRL_BUFFER( aBuffer )                          \
{                                                                \
    (aBuffer)->mMaxSize = SMF_CONTROLFILE_IO_BUFFER_SIZE;        \
    (aBuffer)->mBuffer = (stlChar*)STL_ALIGN(                    \
        (stlInt64)((aBuffer)->mBlock),                           \
        SMF_CONTROLFILE_IO_BLOCK_SIZE );                         \
    (aBuffer)->mOffset = 0;                                      \
    (aBuffer)->mReadSize = 0;                                    \
    stlMemset( (aBuffer)->mBuffer, 0x00, (aBuffer)->mMaxSize );  \
}

typedef struct smfDatafilePersData
{
    stlChar    mName[STL_MAX_FILE_PATH_LENGTH];
    stlUInt16  mState;
    stlUInt16  mID;
    stlBool    mAutoExtend;
    stlChar    mAlign[3];
    stlUInt64  mSize;
    stlUInt64  mNextSize;
    stlUInt64  mMaxSize;
    stlUInt64  mTimestamp;        /** datafile creation time */
    smrLsn     mCheckpointLsn;    /** 마지막 checkpoint Lsn */
    smrLid     mCheckpointLid;    /** 마지막 checkpoint Lid */
    smrLsn     mCreationLsn;      /** datafile creation시 최종 checkpoint Lsn */
    smrLid     mCreationLid;      /** datafile creation시 최종 checkpoint Lid */
} smfDatafilePersData;

typedef struct smfDatafileInfo
{
    smfDatafilePersData   mDatafilePersData;
    stlUInt64             mMaxPageCount;
    stlUInt16             mShmIndex;
    stlInt16              mIoGroupIdx;
    stlInt16              mShmState;
    stlBool               mSkipFlush;
    stlChar               mAlign[1];
    stlUInt64             mShmSize;
    smrLsn                mRestoredChkptLsn;/** Backup을 이용하여 restore된 마지막 Checkpoint Lsn */
    smrLid                mRestoredChkptLid;/** Backup을 이용하여 restore된 마지막 Checkpoint Lid */
    knlLogicalAddr        mChunk;
    knlLogicalAddr        mPchArray;
    stlUInt32             mCorruptionCount;
    stlChar               mPadding[4];
} smfDatafileInfo;

typedef struct smfIncBackupInfo
{
    smrLsn     mBackupLsn[SML_INCREMENTAL_BACKUP_LEVEL_MAX]; /** 직전 incremental backup Lsn */
    stlInt64   mBackupSeq;               /** Incremental backup의 sequence no */
} smfIncBackupInfo;

typedef enum
{
    SMF_INCREMENTAL_BACKUP_OBJECT_TYPE_DATABASE = 0,
    SMF_INCREMENTAL_BACKUP_OBJECT_TYPE_TABLESPACE,
    SMF_INCREMENTAL_BACKUP_OBJECT_TYPE_CONTROLFILE,
    SMF_INCREMENTAL_BACKUP_OBJECT_TYPE_MAX
} smfIncBackupObjectType;

#define SMF_BACKUP_CHUNK_SIZE   (8192)  /** Backup Section의 Segment 크기 */

/**
 * @brief 구조체 smfBackupChunk의 Chunk에서 Chunk Header
 */
typedef struct smfBackupChunkHdr
{
    stlInt32   mBackupCnt;       /** Segment에 기록된 backup list 수 */
    stlInt32   mHighWaterMark;   /** 다음 BackupRecord가 할당 받을 offset */
    stlInt32   mLowWaterMark;    /** 다음 Backup file name이 기록될 버퍼에서 마지막 offset */
    stlChar    mPadding[4];
} smfBackupChunkHdr;

/**
 * @brief Backup Chunk에 기록되는 한개의 Incremental Backup 정보
 */
typedef struct smfBackupRecord
{
    smrLsn     mBackupLsn;       /** backup 시 최종 checkpoint Lsn */
    smrLid     mBackupLid;       /** backup 시 최종 checkpoint Lid */
    stlTime    mBeginTime;       /** backup beginning timestamp */
    stlTime    mCompletionTime;  /** backup completion timestamp */
    smlTbsId   mTbsId;           /** tablespace Id*/
    stlInt16   mLevel;           /** incremental backup level */
    stlInt16   mNameOffset;      /** backup file name이 기록된 위치 */
    stlInt16   mNameLength;      /** backup file name 길이 */
    stlUInt8   mObjectType;      /** backup object 구분 : database/tablespace/controlfile */
    stlUInt8   mBackupOption;    /** backup option : cumulative/differential*/
    stlBool    mIsDeleted;       /** Delte Obsolete Backup 이후 설정되는 flag */
    stlChar    mPadding[5];
} smfBackupRecord;

/**
 * @brief Backup Chunk
 */
typedef struct smfBackupChunk
{
    stlRingEntry mChunkLink;
    stlChar      mChunk[SMF_BACKUP_CHUNK_SIZE];
} smfBackupChunk;

/**
 * @brief WarmupEntry에서 관리되는 Backup Segment이고 Ring 형태로 Backup Chunk를 관리한다.
 */ 
typedef struct smfBackupSegment
{
    stlRingHead        mBackupChunkList;
    smfBackupChunk   * mCurBackupChunk;
} smfBackupSegment;

#define SMF_INIT_BACKUP_SEGMENT( aSegment )                                     \
    {                                                                           \
        STL_RING_INIT_HEADLINK( &(aSegment)->mBackupChunkList,                  \
                                STL_OFFSETOF( smfBackupChunk, mChunkLink ) );   \
        (aSegment)->mCurBackupChunk = NULL;                                     \
    }

#define SMF_INIT_BACKUP_CHUNK( aChunk )                                         \
    {                                                                           \
        STL_RING_INIT_DATALINK( (aChunk),                                       \
                                STL_OFFSETOF( smfBackupChunk, mChunkLink ) );   \
        stlMemset( (aChunk)->mChunk, 0x00, SMF_BACKUP_CHUNK_SIZE );             \
    }

/**
 * Incremental Backup을 이용한 Restore, Merge 시 Backup List를 scan하기 위한 iterator
 */
typedef struct smfBackupIterator
{
    smfBackupRecord   mBackupRecord;
    stlInt64          mBackupChunkSeq;
    stlInt32          mBackupSlotSeq;
    stlInt32          mBackupCntInChunk;
    stlBool           mNeedAbsBackupFilePath;
    stlChar           mFileName[STL_MAX_FILE_PATH_LENGTH + STL_MAX_FILE_NAME_LENGTH];
} smfBackupIterator;

#define SMF_INIT_BACKUP_ITERATOR( aIterator, aAbsFilePath )                     \
    {                                                                           \
        (aIterator)->mBackupChunkSeq   = -1;                                    \
        (aIterator)->mBackupSlotSeq    = -1;                                    \
        (aIterator)->mBackupCntInChunk = -1;                                    \
        (aIterator)->mNeedAbsBackupFilePath = (aAbsFilePath);                   \
        stlMemset( (aIterator)->mFileName,                                      \
                   0x00,                                                        \
                   STL_MAX_FILE_PATH_LENGTH + STL_MAX_FILE_NAME_LENGTH );       \
    }

#define SMF_GET_BACKUP_RECORD( aChunk, aIter )                          \
    (stlChar*)((stlChar*)(aChunk) + STL_SIZEOF(smfBackupChunkHdr) +     \
               STL_SIZEOF(smfBackupRecord) * (aIter)->mBackupSlotSeq)

#define SMF_GET_BACKUP_FILE_NAME( aChunk, aIter )                       \
    (stlChar*)((stlChar*)(aChunk) + (aIter)->mBackupRecord.mNameOffset)

/**
 * Backup section의 segment에서 slot을 할당
 */
#define SMF_ALLOC_BACKUP_SLOT( aChunk, aChunkHdr )              \
    ( (aChunk) + STL_SIZEOF(smfBackupChunkHdr) +                \
      STL_SIZEOF(smfBackupRecord) * ((aChunkHdr)->mBackupCnt) )

#define SMF_GET_NTH_BACKUP_SLOT( aChunk, aIdx )         \
    ( ((aChunk) + STL_SIZEOF(smfBackupChunkHdr) +       \
       STL_SIZEOF(smfBackupRecord) * (aIdx)) )

typedef struct smfTbsPersData
{
    smlTbsId          mTbsId;
    stlUInt16         mAttr;
    stlUInt16         mPageCountInExt;
    stlUInt16         mState;
    stlInt64          mRelationId;
    stlUInt16         mNewDatafileId;
    stlBool           mIsLogging;
    stlBool           mIsOnline;
    stlUInt32         mDatafileCount;
    smrLsn            mOfflineLsn;      /**< offline immedate시 복구해야 할 마지막 Lsn */
    stlInt8           mOfflineState;    /**< @see smfOfflineState */
    stlChar           mPadding[7];
    smfIncBackupInfo  mIncBackup;       /** Tablespace의 Incremental Backup 정보 */
    stlChar           mName[STL_MAX_SQL_IDENTIFIER_LENGTH];
} smfTbsPersData;

/** @} */

/**
 * @defgroup smfTbsBackupState Tablespace Backup state
 * @ingroup smfInternal
 * @{
 */
#define SMF_BACKUP_STATE_NONE                (0x00000000)
#define SMF_BACKUP_STATE_FULL_BACKUP         (0x00000001)
#define SMF_BACKUP_STATE_INCREMENTAL_BACKUP  (0x00000002)
#define SMF_BACKUP_STATE_WAITING             (0x00000004)
/** @} */

/**
 * @brief CONROL FILE MAX COUNT
 */ 
#define SMF_CTRL_FILE_MAX_COUNT         (8)
#define SMF_CTRL_FILE_PATH_MAX_LENGTH   (STL_MAX_FILE_PATH_LENGTH + STL_MAX_FILE_NAME_LENGTH)

/**
 * @brief Control File Load State
 * @remark
 *  Mount Phase에서 Control File Load 실패하면 Dismount 시에 SaveCtrlFile을 방지한다.
 */
typedef enum
{
    SMF_CTRL_FILE_LOAD_STATE_NONE = 0, /**< None of loaded */
    SMF_CTRL_FILE_LOAD_STATE_SYSTEM,   /**< Loaded till System Secion */
    SMF_CTRL_FILE_LOAD_STATE_BACKUP,   /**< Loaded till Backup Secion */
    SMF_CTRL_FILE_LOAD_STATE_DB,       /**< Loaded till Db Secion */
    SMF_CTRL_FILE_LOAD_STATE_LOG       /**< Loaded till Log Secion */
} smfCtrlLoadState;

typedef struct smfTbsBackupInfo
{
    stlUInt32         mBackupState;            /**< tablespace의 backup flag */
    stlUInt32         mBackupWaitCount;        /**< backup이 wait 해야 할 count */
    knlLatch          mBackupLatch;            /**< backup과 flusher 사이의 동시성 제어 */
} smfTbsBackupInfo;

#define SMF_INIT_TBS_BACKUP_INFO( aBackupInfo, aEnv )                   \
    {                                                                   \
        (aBackupInfo)->mBackupState = SMF_BACKUP_STATE_NONE;            \
        (aBackupInfo)->mBackupWaitCount = 0;                            \
        STL_TRY( knlInitLatch( &((aBackupInfo)->mBackupLatch),          \
                               STL_TRUE,                                \
                               KNL_ENV(aEnv) ) == STL_SUCCESS );        \
    }

typedef struct smfDbBackupInfo
{
    stlUInt32         mBackupState;
    stlUInt16         mBegunTbsCount;          /**< Backup이 수행중인 tablespace의 수 */
    stlChar           mPadding[2];
    knlLatch          mBackupLatch;
} smfDbBackupInfo;

#define SMF_INIT_DB_BACKUP_INFO( aBackupInfo, aEnv )                    \
    {                                                                   \
        (aBackupInfo)->mBackupState = SMF_BACKUP_STATE_NONE;            \
        (aBackupInfo)->mBegunTbsCount = 0;                              \
        STL_TRY( knlInitLatch( &((aBackupInfo)->mBackupLatch),          \
                               STL_TRUE,                                \
                               KNL_ENV(aEnv) ) == STL_SUCCESS );        \
    }

typedef struct smfTbsInfo
{
    smfDatafileInfo * mDatafileInfoArray[SML_DATAFILE_MAX_COUNT];
    stlBool           mIsOnline4Scan;          /**< online flag for scan */
    stlBool           mIsProceedMediaRecovery; /**< tablespace의 media recovery flag */
    stlBool           mIsProceedRestore;       /**< tablespace의 restore flag */
    smfTbsPersData    mTbsPersData;
    smfTbsBackupInfo  mBackupInfo;             /**< tablespace backup information */
    knlLatch          mLatch;
} smfTbsInfo;

typedef struct smfSysPersData
{
    stlInt64        mDataStoreMode;          /**< Data Store Mode */
    smlServerState  mServerState;            /**< Server State */
    smrLsn          mLastChkptLsn;           /**< CtrlFile Version 확인 위한 Checkpoint Lsn */
    stlInt64        mBackupChunkCnt;         /**< Incremental backup chunk count */
    stlOffset       mIncBackupSectionOffset; /**< Controlfile Backup Section Start Offset */
    stlOffset       mLogSectionOffset;       /**< Controlfile Log Section Start Offset */
    stlOffset       mDbSectionOffset;        /**< Controlfile Db Section Start Offset */
    stlOffset       mTailSectionOffset;      /**< Controlfile Tail Section Start Offset */
} smfSysPersData;

typedef struct smfDbPersData
{
    stlUInt16         mTransTableSize;
    stlUInt16         mUndoRelCount;
    stlUInt16         mTbsCount;
    stlUInt16         mNewTbsId;
    smlScn            mResetLogsScn;    /** RESETLOGS로 database open시 설정될 Scn */
    smfIncBackupInfo  mIncBackup;       /** DB의 Incremental Backup 정보 */
} smfDbPersData;
    
typedef struct smfWarmupEntry
{
    knlLatch          mLatch;
    smfTbsInfo     ** mTbsInfoArray;
    smfTbsInfo     ** mNoMountTbsInfoArray;
    stlChar           mCtrlFileName[SMF_CTRL_FILE_MAX_COUNT][SMF_CTRL_FILE_PATH_MAX_LENGTH];
    smfSysPersData    mSysPersData;
    smfDbPersData     mDbPersData;
    smlDataAccessMode mDataAccessMode;
    smlTbsId          mLastFlushedTbsId;
    smfDbBackupInfo   mBackupInfo;         /** database backup info */
    smlPid            mLastFlushedPid;
    stlInt32          mFlushedPageCount;
    stlInt64          mPageIoCount;
    stlInt64          mCtrlFileCnt;
    smfCtrlLoadState  mCtrlLoadState;
    smfBackupSegment  mBackupSegment;      /** Incremental Backup info */
} smfWarmupEntry;

typedef struct smfSystemInfo
{
    smlDataAccessMode mDataAccessMode;
    smlTbsId          mLastFlushedTbsId;
    smlPid            mLastFlushedPid;
    stlInt32          mFlushedPageCount;
    stlInt64          mPageIoCount;
} smfSystemInfo;

typedef struct smfInitialTbsData
{
    smlTbsId     mTbsId;
    stlUInt16    mAttr;
    stlUInt16    mPageCountInExt;
    stlChar    * mName;
} smfInitialTbsData;

/**
 * Incremental backup file header
 */
typedef struct smfIncBackupFileHeader
{
    stlInt32    mTbsCount;       /** Backup 된 tablespace count */
    smlTbsId    mTbsId;          /** Backup 된 tablespace Id( INVALID : DATABASE BACKUP) */
    stlChar     mPad[2];
    stlOffset   mBodySize;       /** Backup 된 file size */
    smrLsn      mPrevBackupLsn;  /** 이전 Backup의 마지막 Checkpoint Lsn */
    smrLsn      mMaxBackupLsn;   /** Backup 된 Max PageLsn */
    smrLsn      mBackupChkptLsn; /** Backup 시점에 마지막으로 수행된 Checkpoint Lsn */
    smrLid      mBackupChkptLid; /** Backup 시점에 마지막으로 수행된 Checkpoint Lid */
} smfIncBackupFileHeader;

/**
 * Incremental backup file tail
 */
typedef struct smfIncBackupFileTail
{
    smlTbsId    mTbsId;
    stlChar     mPad[6];
    stlUInt64   mBackupPageCount;
    stlOffset   mTbsOffset;
} smfIncBackupFileTail;

/**
 * @brief Invalid parallel io group
 */
#define SMF_INVALID_IO_GROUP  (-1)

/**
 * @brief Datafile Header를 변경하는 이유
 */
#define SMF_CHANGE_DATAFILE_HEADER_FOR_CHECKPOINT           (0)
#define SMF_CHANGE_DATAFILE_HEADER_FOR_RESTORE              (1)
#define SMF_CHANGE_DATAFILE_HEADER_FOR_GENERAL              (2)
#define SMF_CHANGE_DATAFILE_HEADER_FOR_RECOVERY             (3)
#define SMF_CHANGE_DATAFILE_HEADER_FOR_INCOMPLETE_RECOVERY  (4)

/**
 * @brief 주어진 테이블스페이스 속성을 얻는다.
 * @param[in] aTbsId 대상 Tablespace 아이디
 * @return 테이블 스페이스 속성
 * @see smlTablespaceAttr
 */
#define SMF_GET_TBS_ATTR( aTbsId )                                      \
    (gSmfWarmupEntry->mTbsInfoArray[(aTbsId)]->mTbsPersData.mAttr)
#define SMF_IS_LOGGING_TBS( aTbsId )                                    \
    (gSmfWarmupEntry->mTbsInfoArray[(aTbsId)]->mTbsPersData.mIsLogging)
#define SMF_IS_ONLINE_TBS( aTbsId )                                     \
    (gSmfWarmupEntry->mTbsInfoArray[(aTbsId)]->mTbsPersData.mIsOnline)
#define SMF_GET_PAGE_COUNT_IN_EXTENT( aTbsId )                                  \
    (gSmfWarmupEntry->mTbsInfoArray[(aTbsId)]->mTbsPersData.mPageCountInExt)
#define SMF_IS_ONLINE_TBS_FOR_SCAN( aTbsId )                    \
    (gSmfWarmupEntry->mTbsInfoArray[(aTbsId)]->mIsOnline4Scan)
#define SMF_GET_LOGICAL_PCH_ARRAY( aTbsId, aDatafileId )                                        \
    (gSmfWarmupEntry->mTbsInfoArray[(aTbsId)]->mDatafileInfoArray[(aDatafileId)]->mPchArray)
#define SMF_GET_LOGICAL_CHUNK( aTbsId, aDatafileId )                                            \
    (gSmfWarmupEntry->mTbsInfoArray[(aTbsId)]->mDatafileInfoArray[(aDatafileId)]->mChunk)
#define SMF_GET_TBS_OFFLINE_STATE( aTbsId )                                     \
    (gSmfWarmupEntry->mTbsInfoArray[(aTbsId)]->mTbsPersData.mOfflineState)
#define SMF_SET_TBS_OFFLINE_STATE( aTbsId, aState ) \
{                                                                                   \
    gSmfWarmupEntry->mTbsInfoArray[(aTbsId)]->mTbsPersData.mOfflineState = aState;  \
}

#define SMF_TBS_BACKUP_STATE( aTbsId )                      \
    (gSmfWarmupEntry->mTbsInfoArray[(aTbsId)]->mBackupInfo.mBackupState)

#define SMF_IS_TBS_BACKUP_IN_PROGRESS( aTbsId )                      \
    (                                                                           \
        ((gSmfWarmupEntry->mTbsInfoArray[(aTbsId)]->mBackupInfo.mBackupState)   \
         == SMF_BACKUP_STATE_FULL_BACKUP) ||                                    \
        ((gSmfWarmupEntry->mTbsInfoArray[(aTbsId)]->mBackupInfo.mBackupState)   \
         == SMF_BACKUP_STATE_INCREMENTAL_BACKUP)                                \
    )

#define SMF_SET_TBS_BACKUP_STATE( aTbsId, aState ) \
{                                                                                  \
    gSmfWarmupEntry->mTbsInfoArray[(aTbsId)]->mBackupInfo.mBackupState = aState;   \
}

#define SMF_SET_TBS_PROCEED_MEDIA_RECOVERY( aTbsId, aState )                    \
{                                                                               \
    gSmfWarmupEntry->mTbsInfoArray[(aTbsId)]->mIsProceedMediaRecovery = aState; \
}

#define SMF_IS_TBS_PROCEED_MEDIA_RECOVERY( aTbsId )     \
    (gSmfWarmupEntry->mTbsInfoArray[(aTbsId)]->mIsProceedMediaRecovery)

#define SMF_IS_TBS_PROCEED_RESTORE( aTbsId )     \
    (gSmfWarmupEntry->mTbsInfoArray[(aTbsId)]->mIsProceedRestore)

#define SMF_SET_TBS_PROCEED_RESTORE( aTbsId, aState )                     \
{                                                                         \
    gSmfWarmupEntry->mTbsInfoArray[(aTbsId)]->mIsProceedRestore = aState; \
}

#define SMF_TBS_BACKUP_WAIT_COUNT( aTbsId )                      \
    (gSmfWarmupEntry->mTbsInfoArray[(aTbsId)]->mBackupInfo.mBackupWaitCount)

#define SMF_TBS_AVAILABLE_BACKUP( aTbsId )                          \
    ( SMF_IS_LOGGING_TBS(aTbsId) &&                                 \
      (SML_TBS_IS_TEMPORARY(SMF_GET_TBS_ATTR(aTbsId)) != STL_TRUE) )

#define SMF_TBS_SET_SKIP_FLUSH_STATE( aTbsId, aDatafileId, aState ) \
{                                                                                                        \
    gSmfWarmupEntry->mTbsInfoArray[(aTbsId)]->mDatafileInfoArray[(aDatafileId)]->mSkipFlush = (aState);  \
}

#define SMF_TBS_GET_SKIP_FLUSH_STATE( aTbsId, aDatafileId ) \
    (gSmfWarmupEntry->mTbsInfoArray[(aTbsId)]->mDatafileInfoArray[(aDatafileId)]->mSkipFlush)

/** @} */

/**
 * @addtogroup smd
 * @{
 */

typedef struct smdWarmupEntry
{
    stlInt64 mVersionConflictCount;
} smdWarmupEntry;

typedef struct smdSystemInfo
{
    stlInt64 mVersionConflictCount;
} smdSystemInfo;

typedef struct smdRowInfo
{
    smpHandle         * mPageHandle;
    smlTbsId            mTbsId;
    stlChar           * mRow;
    smxlTransId         mTransId;
    smlScn              mViewScn;
    smlScn              mCommitScn;
    smlTcn              mTcn;
    knlValueBlockList * mColList;
    knlValueBlockList * mRowIdColList;
    stlInt64            mTableLogicalId;
    stlInt32            mBlockIdx;
    knlKeyCompareList * mKeyCompList;
} smdRowInfo;

typedef struct smdValueBlockList smdValueBlockList;

/**
 * Key Extract시 사용될 Value Block
 */
typedef struct smdValueBlock
{
    stlInt32             mAllocBlockCnt;
    stlInt32             mUsedBlockCnt;
    stlInt32             mSkipBlockCnt;
    dtlDataValue       * mKeyValue;
    knlColumnInReadRow * mColumnInRow;
} smdValueBlock;

/**
 * Key Extract시 사용될 Value Block List
 */
struct smdValueBlockList
{
    stlInt32             mColumnOrder;
    smdValueBlock      * mValueBlock;
    smdValueBlockList  * mNext;
};

#define SMD_PENDING_OPERATION_DROP_SEGMENT  (1)   /**< drop segment pending operation */
#define SMD_PENDING_OPERATION_UNDO          (2)   /**< undo pending operation */

typedef struct smdPendArg
{
    smlTbsId  mTbsId;
    stlInt64  mRelationId;
} smdPendArg;

typedef void (*smpIsUsableFixedSlot)( stlChar  * aRowHdr,
                                      stlBool  * aIsUsable );

/**
 * 하나의 Piece에 저장될수 있는 Column의 개수
 */
#define SMD_MAX_COLUMN_COUNT_IN_PIECE  (255)

/** @} */

/**
 * @addtogroup smp
 * @{
 */

/**
 * @brief PchArrayList State
 */ 
#define SMP_PCHARRAY_STATE_ACTIVE      (1)
#define SMP_PCHARRAY_STATE_INACTIVE    (2)
#define SMP_PCHARRAY_STATE_DROPPED     (3)

/**
 * @brief flushing target
 */
typedef enum
{
    SMP_FLUSH_TARGET_TABLESPACE,  /**< 특정 tablespace를 Flush한다. */
    SMP_FLUSH_TARGET_DATABASE     /**< 전체 database를 Flush한다. */
} smpFlushObject;

/**
 * @brief flushing behavior
 */
typedef enum
{
    SMP_FLUSH_LOGGED_PAGE, /**< Logging 된 페이지를 Flush 한다 */
    SMP_FLUSH_DIRTY_PAGE,  /**< Dirty 된 페이지를 Flush 한다 */
} smpFlushBehavior;

/**
 * @brief argument for flushing event
 */
typedef struct smpFlushArg
{
    smpFlushBehavior mBehavior;
    stlBool          mLogging;
    stlBool          mForCheckpoint;
} smpFlushArg;

typedef struct smpFlushTbsArg
{
    smlTbsId         mTbsId;
    smlDatafileId    mDatafileId;
    stlInt32         mReason;
} smpFlushTbsArg;

/**
 * @brief Page Control Header 구조체
 */
typedef struct smpCtrlHeader
{
    knlLatch       mPageLatch;         /**< Page Latch */
    knlLogicalAddr mFrame;             /**< Page Body가 저장된 논리적 포인터 */
    stlInt32       mFixedCount;        /**< Page의 Fixed 개수(DISK ONLY) */
    smlTbsId       mTbsId;             /**< Tablespace ID */
    stlBool        mDirty;             /**< Dirty Flag */
    stlChar        mPageType;          /**< Page Type @see smpPageType */
    smlPid         mPageId;            /**< Page ID */
    stlUInt32      mDeviceType;        /**< Device Type Flag @see smlTablespaceAttr */
    smrLsn         mFirstDirtyLsn;     /**< Page를 처음으로 dirty로 만든 Lsn */
    smrLsn         mRecoveryLsn;       /**< Recovery 지점을 결정하기 위한 Lsn */
    smrSbsn        mRecoverySbsn;      /**< Recovery 지점을 결정하기 위한 Sbsn */
    stlInt16       mRecoveryLpsn;     /**< Recovery 지점을 결정하기 위한 Log Piece Sequence Number */
    stlBool        mIsCorrupted;
    stlChar        mPadding[13];       /**< Padding for cache line */
} smpCtrlHeader;

/**
 * @brief Page Access Handle 구조체
 */
struct smpHandle
{
    smpCtrlHeader * mPch;         /**< Page Control Header */
    void          * mFrame;       /**< Page Frame */
};

typedef stlStatus (*smpIsAgableFunc)( smpHandle * aPageHandle,
                                      stlBool   * aIsAgable,
                                      smlEnv    * aEnv );

/**
 * @brief Page System Info
 */
typedef struct smpSystemInfo
{
    smrLsn    mMinFlushedLsn;       /** Flush된 최소 Lsn */
    stlTime   mPageFlushingTime;    /** 모든 Dirty Page를 Flush하는데 걸린 시간(ms) */
    stlInt64  mFlushedPageCount;    /** Flush한 Dirty Page 개수 */
} smpSystemInfo;

/**
 * @brief Page Type 정의
 */
typedef enum
{
    SMP_PAGE_TYPE_INIT = 0,          /**< Un-initilaized Page */
    SMP_PAGE_TYPE_UNFORMAT,          /**< Unformatted Page */
    SMP_PAGE_TYPE_EXT_BLOCK_MAP,     /**< Extent Block Map Page */
    SMP_PAGE_TYPE_EXT_MAP,           /**< Extent Map Page */
    SMP_PAGE_TYPE_BITMAP_HEADER,     /**< Table Data Page */
    SMP_PAGE_TYPE_CIRCULAR_HEADER,   /**< Circular Header Page */
    SMP_PAGE_TYPE_FLAT_HEADER,       /**< Flat Header Page */
    SMP_PAGE_TYPE_PENDING_HEADER,    /**< Pending Header Page */
    SMP_PAGE_TYPE_UNDO,              /**< Undo Data Page */
    SMP_PAGE_TYPE_TRANS,             /**< Transaction Data Page */
    SMP_PAGE_TYPE_BITMAP_INTERNAL,   /**< Bitmap Internal Page */
    SMP_PAGE_TYPE_BITMAP_LEAF,       /**< Bitmap Leaf Page */
    SMP_PAGE_TYPE_BITMAP_EXTENT_MAP, /**< Bitmap Leaf Extent Map Page */
    SMP_PAGE_TYPE_TABLE_DATA,        /**< Heap Table Data Page */
    SMP_PAGE_TYPE_TABLE_COLUMNAR_DATA, /**< Columnar Table Data Page */
    SMP_PAGE_TYPE_INDEX_DATA,        /**< Index Data Page */
    SMP_PAGE_TYPE_MAX
} smpPageType;

/**
 * @defgroup smpFreeness 페이지 자유공간 정도 정의
 * @ingroup smp
 * @{
 */
#define SMP_FREENESS_UNALLOCATED   (0)       /**< 아직 페이지가 할당되지 않은 경우 */
#define SMP_FREENESS_FULL          (1)       /**< 페이지에 자유공간이 없는 경우  */
#define SMP_FREENESS_UPDATEONLY    (2)       /**< 갱신만 가능한 경우  */
#define SMP_FREENESS_INSERTABLE    (3)       /**< 삽입과 갱신이 가능한 경우 */
#define SMP_FREENESS_FREE          (4)       /**< 페이지 전체가 빈 경우 */
#define SMP_FREENESS_MAX           (5)
/** @} */

/**
 * @brief Freeness의 Type redifinition
 */
typedef stlInt32 smpFreeness;

/**
 * @brief Page Frame을 구하는 매크로
 */
#define SMP_FRAME( aPageHandle ) (((smpHandle*)(aPageHandle))->mFrame)

/**
 * @brief 페이지 frame의 임시 복사본을 사용하기 위한 dummy handle을 만듬.
 */
#define SMP_MAKE_DUMMY_HANDLE( aPageHandle, aTempFrame )                \
{                                                            \
    ((smpHandle*)(aPageHandle))->mPch   = NULL;              \
    ((smpHandle*)(aPageHandle))->mFrame = aTempFrame;        \
}

#define SMP_IS_DUMMY_HANDLE( aPageHandle )                              \
    (((smpHandle*)(aPageHandle))->mPch == NULL ? STL_TRUE : STL_FALSE )
#define SMP_IS_SAME_HANDLE( aPageHandle1, aPageHandle2 )                                \
    ( (((smpHandle*)(aPageHandle1))->mPch == ((smpHandle*)(aPageHandle2))->mPch) &&     \
      (((smpHandle*)(aPageHandle1))->mFrame == ((smpHandle*)(aPageHandle2))->mFrame) )

/**
 * @brief 페이지 사이즈 정의
 */
#define SMP_PAGE_SIZE (8192)

#if DTL_LONGVARYING_INIT_STRING_SIZE < SMP_PAGE_SIZE
    #error Initial buffer size of long varying type should be greater than page size
#endif

/**
 * @brief Datafile precision
 */
#define SMP_DATAFILE_PRECISION 10
/**
 * @brief Page sequece precision
 */
#define SMP_PAGE_SEQ_PRECISION 22
/**
 * @brief Datafile precision mask
 */
#define SMP_DATAFILE_MASK      0xFFC00000
/**
 * @brief Page sequece precision mask
 */
#define SMP_PAGE_SEQ_MASK      0x003FFFFF

/**
 * @brief Page Identifier 구성 매크로
 */
#define SMP_MAKE_PID( aDatafileID, aPageSeq )                        \
    ( ((stlUInt32)(aDatafileID) << SMP_PAGE_SEQ_PRECISION) + (aPageSeq) )

/**
 * @brief Page Identifier로 부터 Datafile Identifier를 얻는 매크로
 */
#define SMP_DATAFILE_ID( aPageId )                                   \
    ( ((aPageId) & SMP_DATAFILE_MASK) >> SMP_PAGE_SEQ_PRECISION )

/**
 * @brief Page Identifier로 부터 Page Sequence Identifier를 얻는 매크로
 */
#define SMP_PAGE_SEQ_ID( aPageId )                                   \
    ( (aPageId) & SMP_PAGE_SEQ_MASK )

/**
 * @brief Null Page Identifier
 */
#define SMP_NULL_PID 0

/**
 * @brief Page Physical Header 구조체
 */
#define SMP_VOLATILE_AREA_SIZE 24

/**
 * @brief Page Self Identifier
 */ 
typedef struct smpPid
{
    smlTbsId  mTbsId;
    stlChar   mPadding[2];
    smlPid    mPageId;
} smpPid;
    
/**
 * @brief Page Physical Header 구조체
 */
typedef struct smpPhyHdr
{
    smpPageType  mPageType;                              /**< Page Type */
    smpFreeness  mFreeness;                              /**< Page Freeness */
    smrLsn       mLsn;                                   /**< Page Lsn */
    stlUInt64    mTimestamp;                             /**< Datafile creation timestamp */
    smlRid       mParentRid;                             /**< Parent Rid (bitmap segment only) */
    stlInt64     mSegmentId;                             /**< Segment Identifier */
    stlInt64     mMaxViewScn;                            /**< Maximum view scn */
    smlScn       mAgableScn;                             /**< 페이지의 재사용 가능한 Scn */
    stlChar      mVolatileArea[SMP_VOLATILE_AREA_SIZE];  /**< Page specific volatile area */
    smpPid       mSelfId;                                /**< Self Identifier: PageId와 Tbs Id */
} smpPhyHdr;

/**
 * Incremental backup을 이용한 media recovery 시
 * backup된 page가 apply대상인지 판단하기 위한 macro
 */
#define SMP_IS_APPLICABLE_BACKUP_PAGE( aPage, aTbsId )       \
    ( ((aTbsId) == SML_INVALID_TBS_ID) ||                    \
      ((aTbsId) == ((smpPhyHdr*)(aPage))->mSelfId.mTbsId) )

/**
 * @brief KNL_ASSERT에 사용될 Physical Header Dump format
 */
#define SMP_FORMAT_PHYSICAL_HDR( aPhyHdr )                                          \
    ( "PAGE_TYPE(%d), FREENESS(%d), LSN(%ld), AGABLE_SCN(%ld), "                    \
      "TIMESTAMP(%ld), SEGMENT_ID(%ld), MAX_VIEW_SCN(%ld), PARENT_RID(%d,%d,%d), "  \
      "PAGE_ID(%d,%d,%d) ",                                                         \
      (aPhyHdr)->mPageType,                                                         \
      (aPhyHdr)->mFreeness,                                                         \
      (aPhyHdr)->mLsn,                                                              \
      (aPhyHdr)->mAgableScn,                                                        \
      (aPhyHdr)->mTimestamp,                                                        \
      (aPhyHdr)->mSegmentId,                                                        \
      (aPhyHdr)->mMaxViewScn,                                                       \
      (aPhyHdr)->mParentRid.mTbsId,                                                 \
      (aPhyHdr)->mParentRid.mPageId,                                                \
      (aPhyHdr)->mParentRid.mOffset,                                                \
      (aPhyHdr)->mSelfId.mTbsId,                                                    \
      SMP_DATAFILE_ID( (aPhyHdr)->mSelfId.mPageId ),                                \
      SMP_PAGE_SEQ_ID( (aPhyHdr)->mSelfId.mPageId ) )

/**
 * @brief Page Physical Header 주소를 얻는 매크로
 */
#define SMP_GET_PHYSICAL_HDR( aPage )  ( (smpPhyHdr*)(aPage) )

/**
 * @brief Page Physical Header 초기화 매크로
 */
#define SMP_INIT_PHYSICAL_HDR( aPhyHdr, aPch, aPageType, aScn, aTimestamp, aSegmentId, aParentRid ) \
    {                                                                                               \
        (aPhyHdr)->mPageType = aPageType;                                                           \
        (aPhyHdr)->mSegmentId = aSegmentId;                                                         \
        (aPhyHdr)->mParentRid = aParentRid;                                                         \
        (aPhyHdr)->mFreeness = SMP_FREENESS_FREE;                                                   \
        (aPhyHdr)->mAgableScn = aScn;                                                               \
        (aPhyHdr)->mTimestamp = aTimestamp;                                                         \
        (aPhyHdr)->mMaxViewScn = 0;                                                                 \
        stlMemset( (aPhyHdr)->mVolatileArea, 0x00, SMP_VOLATILE_AREA_SIZE );                        \
        (aPhyHdr)->mSelfId.mTbsId = aPch->mTbsId;                                                   \
        (aPhyHdr)->mSelfId.mPageId = aPch->mPageId;                                                 \
    }

#define SMP_GET_PAGE_TYPE( aPage )     ( ((smpPhyHdr *)(aPage))->mPageType )

/**
 * @brief Page Physical Tail
 */

typedef struct smpPhyTail
{
    stlInt64   mChecksum;
} smpPhyTail;

#define SMP_GET_PHYSICAL_TAIL( aFrame )                                        \
    ((smpPhyTail*)( (aFrame) + SMP_PAGE_SIZE - STL_SIZEOF( smpPhyTail ) ))

/**
 * @brief Checksum type
 */ 
#define SMP_PAGE_CHECKSUM_TYPE_LSN    STL_INT64_C(0x0000000000000000)
#define SMP_PAGE_CHECKSUM_TYPE_CRC    STL_INT64_C(0x4000000000000000)

#define SMP_PAGE_CHECKSUM_TYPE_MASK   STL_INT64_C(0xC000000000000000)
#define SMP_PAGE_CHECKSUM_LSN_MASK    STL_INT64_C(0x3FFFFFFFFFFFFFFF)
#define SMP_PAGE_CHECKSUM_CRC_MASK    STL_INT64_C(0x00000000FFFFFFFF)

#define SMP_PAGE_CHECKSUM_TYPE_PRECISION (62)

/* ===================================================== */
/* Page Body Management                               */
/* ===================================================== */
/**
 * @brief Page Management Types
 */
 
/**
 * @defgroup smpPageManagementType Page Management Type
 * @ingroup smpInternal
 * @{
 */

#define SMP_RAW_CHUNK_TYPE    0
#define SMP_ORDERED_SLOT_TYPE 1
#define SMP_FIXED_SLOT_TYPE   2

/**
 * @brief Page Signpost. All signpost SHOULD be 8 byte aligned
 */
typedef struct smpSignpost
{
    stlUInt16  mPageMgmtType;     /**< Page Management Type */
    stlUInt16  mHighWaterMark;    /**< Page Usage High Water Mark */
} smpSignpost;

#define SMP_GET_PAGE_MGMT_TYPE( aPageFrame )                            \
    ( ((smpSignpost *)SMP_GET_SIGNPOST(aPageFrame))->mPageMgmtType )

/**
 * @brief Page Signpost for raw chunk type pages
 */
typedef struct smpRawChunkSignpost
{
    smpSignpost  mCommon;          /**< common info */
    stlChar      mPadding[4];      /**< Padding For 8 byte align */
} smpRawChunkSignpost;

/**
 * @brief Page Signpost for ordered slot type pages
 */
typedef struct smpOrderedSlotSignpost
{
    smpSignpost  mCommon;             /**< common info */
    stlUInt16    mLogicalHeaderSize;  /**< page logical header size. It SHOULD be 8 byte aligned */
    stlUInt8     mMinRtsCount;        /**< Minimum RTS Count */
    stlUInt8     mMaxRtsCount;        /**< Maximum RTS Count */
    stlUInt8     mCurRtsCount;        /**< Current RTS Count */
    stlUInt8     mHintRts;            /**< Next Hint RTS Sequence */
    stlUInt16    mLowWaterMark;       /**< lower bound of Row(key) data slots */
    stlUInt16    mReclaimedSlotSpace; /**< total space size of reclaimed data slots */
    stlUInt16    mActiveSlotCount;    /**< row/key count which is not deleted */
} smpOrderedSlotSignpost;

/**
 * @brief Page Signpost for fixed slot type pages
 */
typedef struct smpFixedSlotSignpost
{
    smpSignpost  mCommon;              /**< common info */
    stlUInt16    mLogicalHeaderSize;   /**< page logical header size. It SHOULD be 8 byte aligned */
    stlUInt16    mActiveSlotCount;     /**< row/key count which is not deleted */
    stlInt16     mAllocatedMaxSlotSeq; /**< Maximum slot seq of allocated slots */
    stlChar      mPadding[6];
} smpFixedSlotSignpost;

#define SMP_FIXED_SLOT_CONTINUOUS_COLUMN_MASK          (0x1000)
#define SMP_IS_FIXED_SLOT_CONTINUOUS_COLUMN(aColLen)    \
    (((aColLen) & SMP_FIXED_SLOT_CONTINUOUS_COLUMN_MASK) == SMP_FIXED_SLOT_CONTINUOUS_COLUMN_MASK)
#define SMP_GET_FIXED_SLOT_CONTINUOUS_COLUMN_LENGTH(aColLen)    \
    ((aColLen) & ~SMP_FIXED_SLOT_CONTINUOUS_COLUMN_MASK)

/**
 * @brief page의 signpost의 위치를 반환
 */
#define SMP_GET_SIGNPOST(page)  ((stlChar*)(page) + STL_SIZEOF(smpPhyHdr))

/**
 * @brief Logical Header의 크기를 반환
 */
#define SMP_GET_LOGICAL_HEADER_SIZE(page)    \
    (*(stlUInt16*)((stlChar*)(page) + STL_SIZEOF(smpPhyHdr) + STL_OFFSETOF(smpOrderedSlotSignpost,mLogicalHeaderSize)))
/**
 * @brief RTS의 최소 개수를 반환
 */
#define SMP_GET_MINIMUM_RTS_COUNT(page)    \
    (*(stlUInt8*)((stlChar*)(page) + STL_SIZEOF(smpPhyHdr) + STL_OFFSETOF(smpOrderedSlotSignpost,mMinRtsCount)))

/**
 * @brief RTS의 최대 개수를 반환
 */
#define SMP_GET_MAXIMUM_RTS_COUNT(page)    \
    (*(stlUInt8*)((stlChar*)(page) + STL_SIZEOF(smpPhyHdr) + STL_OFFSETOF(smpOrderedSlotSignpost,mMaxRtsCount)))

/**
 * @brief 현재 RTS의 개수를 반환
 */
#define SMP_GET_CURRENT_RTS_COUNT(page)    \
    (*(stlUInt8*)((stlChar*)(page) + STL_SIZEOF(smpPhyHdr) + STL_OFFSETOF(smpOrderedSlotSignpost,mCurRtsCount)))

/**
 * @brief Logical header 포인터를 반환
 */
#define SMP_GET_LOGICAL_HEADER(page) ((stlChar*)page + STL_SIZEOF(smpPhyHdr) + STL_SIZEOF(smpOrderedSlotSignpost))


/**
 * @brief 페이지내의 reclaimed space 크기를 증가시킴
 */
#define SMP_ADD_RECLAIMED_SPACE( aPageFrame, aFreeSize )                                               \
    {                                                                                                  \
        ((smpOrderedSlotSignpost*)(SMP_GET_SIGNPOST(aPageFrame)))->mReclaimedSlotSpace += (aFreeSize); \
        STL_DASSERT( ((smpOrderedSlotSignpost*)(SMP_GET_SIGNPOST(aPageFrame)))->mReclaimedSlotSpace <  \
                     SMP_PAGE_SIZE );                                                                  \
    }                                                                                                  \
    /**
     * @brief 페이지내의 reclaimed space 크기를 감소시킴
     */
#define SMP_REMOVE_RECLAIMED_SPACE( aPageFrame, aFreeSize )                                            \
    {                                                                                                  \
        ((smpOrderedSlotSignpost*)(SMP_GET_SIGNPOST(aPageFrame)))->mReclaimedSlotSpace -= (aFreeSize); \
        STL_DASSERT( ((smpOrderedSlotSignpost*)(SMP_GET_SIGNPOST(aPageFrame)))->mReclaimedSlotSpace <  \
                     SMP_PAGE_SIZE );                                                                  \
    }                                                                                                  \

/**
 * @brief 페이지내의 Active Slot 개수를 반환함
 */
#define SMP_GET_ACTIVE_SLOT_COUNT( aPageFrame )                                            \
    ( ((smpOrderedSlotSignpost*)(SMP_GET_SIGNPOST(aPageFrame)))->mActiveSlotCount )
/**
 * @brief 페이지내의 Active Slot 개수를 증가시킴
 */
#define SMP_INCREASE_ACTIVE_SLOT_COUNT( aPageFrame )                                       \
{                                                                                          \
    ((smpOrderedSlotSignpost*)(SMP_GET_SIGNPOST(aPageFrame)))->mActiveSlotCount++;         \
}
/**
 * @brief 페이지내의 Active Slot 개수를 감소시킴
 */
#define SMP_DECREASE_ACTIVE_SLOT_COUNT( aPageFrame )                                       \
{                                                                                          \
    ((smpOrderedSlotSignpost*)(SMP_GET_SIGNPOST(aPageFrame)))->mActiveSlotCount--;         \
}


#define SMP_RTS_OFFSET_CACHE_COUNT     (3)

/**
 * @brief Maximum Count of Parallel Load Factor
 */
#define SMP_MAX_PARALLEL_LOAD_FACTOR   (64)

/**
 * @brief Referred Transaction Slot Entry. 8 byte aligned.
 * Current Size is 48byte
 */
typedef struct smpRts
{
    stlUInt16   mRefCount;        /**< Reference Count */
    stlUInt16   mOffsetCache[SMP_RTS_OFFSET_CACHE_COUNT];  // 8byte align
    stlInt16    mPrevVerCnt;      /**< previous RTS version count : max 15 */
    stlInt16    mPrevOffset;      /**< previous RTS version location */
    smlPid      mPrevPid;         /**< previous RTS version location */
    smxlTransId mTransId;         /**< 8byte */
    smlScn      mScn;             /**< 8byte */
    smlScn      mPrevCommitScn;
} smpRts;


 /**
 * @brief 최대 RTS의 개수
 */
#define SMP_MAX_RTS_COUNT  (32)
#define SMP_MAX_RTS_VERSION_COUNT  (16) /* 한 페이지에서 최대 512개의tx를 관리할 수 있음 */
#define SMP_RTS_SEQ_NULL   (0xFE)
#define SMP_RTS_SEQ_STABLE (0xFF)

#define SMP_IS_VALID_RTS_SEQ(rtsseq)  ((rtsseq) < SMP_RTS_SEQ_NULL ? STL_TRUE : STL_FALSE )
#define SMP_IS_STABLE_RTS_SEQ(rtsseq) ((rtsseq) == SMP_RTS_SEQ_STABLE ? STL_TRUE : STL_FALSE )


#define SMP_MAX_RTS_VERSION_REC_SIZE                                        \
    (STL_SIZEOF(smxlTransId) + STL_SIZEOF(smlPid) + STL_SIZEOF(stlInt8) +   \
     (STL_SIZEOF(smlScn) * SMP_MAX_RTS_VERSION_COUNT))


#define SMP_RTS_IS_FREE(rts)         (((rts)->mRefCount) == 0 ? STL_TRUE : STL_FALSE )
#define SMP_RTS_IS_NOT_STAMPED(rts)  ( SML_IS_VIEW_SCN((rts)->mScn ) )
#define SMP_RTS_IS_STAMPED(rts)      ( SML_IS_VIEW_SCN((rts)->mScn ) == STL_TRUE ? STL_FALSE : STL_TRUE )
#define SMP_RTS_IS_STABLE(rts)       ( (rts)->mScn  == 0 ? STL_TRUE : STL_FALSE )

#define SMP_RTS_SET_FREE(rts)        ((rts)->mRefCount = 0)
#define SMP_RTS_SET_SCN(rts, scn)    ((rts)->mScn = scn )
#define SMP_RTS_SET_STABLE(rts)      ((rts)->mScn = 0 )


/**
 * @brief RTS Refernce Count를 반환한다
 */
#define SMP_RTS_GET_REF_COUNT(rts)   ((rts)->mRefCount )

/**
 * @brief RTS Refernce Count를 1 증가시킨다
 */
#define SMP_RTS_INCREASE_REF_COUNT(rts)   ((rts)->mRefCount++)
/**
 * @brief RTS Refernce Count를 1 감소시킨다
 */
#define SMP_RTS_DECREASE_REF_COUNT(rts)             \
{                                                   \
    STL_ASSERT( SMP_RTS_GET_REF_COUNT(rts)  > 0 );  \
    (rts)->mRefCount--;                             \
}
/**
 * @brief RTS Refernce Count를 n 증가시킨다
 */
#define SMP_RTS_INCREASE_REF_COUNT_BY_N(rts,n)     \
{                                                  \
    (rts)->mRefCount += n;                         \
}
/**
 * @brief RTS Refernce Count를 n 감소시킨다
 */
#define SMP_RTS_DECREASE_REF_COUNT_BY_N(rts,n)        \
{                                                     \
    STL_ASSERT( SMP_RTS_GET_REF_COUNT(rts) >= (n) );  \
    (rts)->mRefCount -= n;                            \
}




/**
 * @brief RTS를 처음 사용을 위해 초기화 한다
 */
#define SMP_INIT_RTS(rts)   {                         \
    (rts)->mRefCount        = 0; \
    (rts)->mOffsetCache[0]  = 0;                      \
    (rts)->mOffsetCache[1]  = 0;                      \
    (rts)->mOffsetCache[2]  = 0;                      \
    (rts)->mPrevVerCnt      = 0;                      \
    (rts)->mPrevOffset      = 0;                      \
    (rts)->mPrevPid         = 0;                      \
    (rts)->mTransId         = SML_INVALID_TRANSID;    \
    (rts)->mScn             = 0;                      \
    (rts)->mPrevCommitScn   = 0;                      \
}

/**
 * @brief RTS를 재사용을 위해 초기화 한다
 */
#define SMP_RESET_RTS(rts)   {                        \
    (rts)->mRefCount        = 0;                      \
    (rts)->mOffsetCache[0]  = 0;                      \
    (rts)->mOffsetCache[1]  = 0;                      \
    (rts)->mOffsetCache[2]  = 0;                      \
    (rts)->mTransId         = SML_INVALID_TRANSID;    \
    (rts)->mScn             = 0;                      \
}



/**
 * @brief 첫번째 RTS를 반환한다
 */
#define SMP_GET_FIRST_RTS(page)           \
    ((smpRts*)((stlChar*)(page) + STL_SIZEOF(smpPhyHdr) + STL_SIZEOF(smpOrderedSlotSignpost) + SMP_GET_LOGICAL_HEADER_SIZE(page)))


/***********************************/
/** RTS INTERFACES                 */
/***********************************/
#define  SMP_GET_NTH_RTS( aPageFrame, aRtsSeq )  ((SMP_GET_FIRST_RTS(aPageFrame)) + aRtsSeq)
#define  SMP_IS_SAME_TRANS( aRts, aTransId )     ((((smpRts*)(aRts))->mTransId == aTransId) ? STL_TRUE : STL_FALSE)





/**
 * @brief Offset Table header for ordering RowItems
 */
typedef struct smpOffsetTableHeader
{
    stlInt16   mCurrentSlotCount;
    stlInt16   mFirstFreeOffsetSlot;
} smpOffsetTableHeader;

/**
 * @brief Page의 Offset Table Header의 위치를 반환한다
 */
#define SMP_GET_OFFSET_TABLE_HEADER(page)                                                                  \
    ((smpOffsetTableHeader*)((stlChar*)page + STL_SIZEOF(smpPhyHdr) + STL_SIZEOF(smpOrderedSlotSignpost) + \
     SMP_GET_LOGICAL_HEADER_SIZE(page) + SMP_GET_CURRENT_RTS_COUNT(page)* STL_SIZEOF(smpRts)))



/** @} */

/**
 * @brief Offset Slot represents the offset of RowItem(when used) or 
 * next free slot #(when free)
 */
typedef stlInt16 smpOffsetSlot;

/**
 * @brief Free Offset Slot Seq Mask
 */
#define SMP_FREE_OFFSET_SLOT_SEQ_MASK  (0x7FFF)

/**
 * @brief NULL Offset Slot
 */
#define SMP_OFFSET_SLOT_NULL  SMP_FREE_OFFSET_SLOT_SEQ_MASK  

/**
 * @brief Free Offset Slot을 사용하는 page 타입인지 여부.
 * smpOffsetTableheader.mFirstFreeOffsetSlot 에 세팅됨.
 */
#define SMP_USE_FREE_SLOT_LIST_MASK    (0x8000)
#define SMP_USE_FREE_SLOT_LIST_YES     (0x8000)
#define SMP_USE_FREE_SLOT_LIST_NO      (0x0000)

/**
 * @brief check use FREE Offset Slot List
 */
#define SMP_CHECK_USE_FREE_SLOT_LIST(offsetTableHeader)                \
 (((offsetTableHeader->mFirstFreeOffsetSlot)&SMP_USE_FREE_SLOT_LIST_MASK) ==  \
  SMP_USE_FREE_SLOT_LIST_YES ?  STL_TRUE : STL_FALSE)

/**
 * @brief Free Offset Slot MASK
 */
#define SMP_OFFSET_SLOT_FREE_MASK  SMP_USE_FREE_SLOT_LIST_MASK

/**
 * @brief check FREE Offset Slot
 */
#define SMP_OFFSET_SLOT_IS_FREE(slot)     \
 (((*slot)&SMP_OFFSET_SLOT_FREE_MASK) == SMP_OFFSET_SLOT_FREE_MASK ? STL_TRUE : STL_FALSE)

/**
 * @brief next FREE Offset Slot
 */
#define SMP_GET_NEXT_FREE_SLOT(slot)     ((*slot)&SMP_FREE_OFFSET_SLOT_SEQ_MASK)

/**
 * @brief Page내에서 주어진 순서의 OffsetSlot을 반환한다
 */
#define SMP_GET_NTH_OFFSET_SLOT(page,seq)                            \
     ((smpOffsetSlot*)(SMP_GET_OFFSET_TABLE_HEADER(page) + 1) + seq)


/**
 * @brief fixed slot page에서 각 column의 start position을 기록한 array를 반환한다.
 */
#define SMP_GET_COLUMN_LENGTH( aPageHandle )                                                    \
    ( SMP_GET_SIGNPOST(SMP_FRAME(aPageHandle)) + STL_SIZEOF(smpFixedSlotSignpost) +             \
      ((smpFixedSlotSignpost *)SMP_GET_SIGNPOST(SMP_FRAME(aPageHandle)))->mLogicalHeaderSize )

/**
 * @brief fixed slot page에서 column value의 첫 저장 위치를 반환한다.
 */
#define SMP_GET_FIRST_VALUE_POS( aPageHandle, aColCount )                               \
    ( (stlChar *)SMP_GET_COLUMN_LENGTH(aPageHandle) + (aColCount) * STL_SIZEOF(stlInt16) )

/**
 * @brief Page의 RowItem을 dump하기 위한 iterator
 */
typedef struct smpRowItemIterator
{
    smpPhyHdr            * mPage;
    smpOffsetTableHeader * mHeader;
    stlUInt8             * mCurSlot;
    stlUInt8             * mSlotFence;
    stlInt16               mCurSeq;
} smpRowItemIterator;


#define SMP_GET_FIRST_ROWITEM( aPageFrame, aIterator, aRowItem )                        \
    {                                                                                   \
        (aIterator)->mPage = (smpPhyHdr*)(aPageFrame);                                  \
        (aIterator)->mHeader = (SMP_GET_OFFSET_TABLE_HEADER((aPageFrame)));             \
        (aIterator)->mCurSlot = (stlUInt8*)((aIterator)->mHeader + 1);                  \
        (aIterator)->mSlotFence = (stlUInt8*)((smpOffsetSlot*)((aIterator)->mCurSlot) + \
                                              (aIterator)->mHeader->mCurrentSlotCount); \
        (aIterator)->mCurSeq = 0;                                                       \
        while( ((aIterator)->mCurSlot != (aIterator)->mSlotFence) &&                    \
               (SMP_OFFSET_SLOT_IS_FREE((smpOffsetSlot*)((aIterator)->mCurSlot))) )     \
        {                                                                               \
            (aIterator)->mCurSlot+=2;                                                   \
            (aIterator)->mCurSeq++;                                                     \
        }                                                                               \
        (aRowItem) = ((aIterator)->mCurSlot == (aIterator)->mSlotFence) ?               \
            NULL : (stlChar*)(aPageFrame) + *(smpOffsetSlot*)((aIterator)->mCurSlot);   \
    }

#define SMP_GET_LAST_ROWITEM( aPageFrame, aIterator, aRowItem )                                 \
    {                                                                                           \
        (aIterator)->mPage = (smpPhyHdr*)(aPageFrame);                                          \
        (aIterator)->mHeader = (SMP_GET_OFFSET_TABLE_HEADER((aPageFrame)));                     \
        (aIterator)->mSlotFence = (stlUInt8*)((smpOffsetSlot*)((aIterator)->mHeader + 1) - 1);  \
        (aIterator)->mCurSlot = (stlUInt8*)((smpOffsetSlot*)((aIterator)->mSlotFence) +         \
                                            (aIterator)->mHeader->mCurrentSlotCount);           \
        (aIterator)->mCurSeq = (aIterator)->mHeader->mCurrentSlotCount - 1;                     \
        while( ((aIterator)->mCurSlot != (aIterator)->mSlotFence) &&                            \
               (SMP_OFFSET_SLOT_IS_FREE((smpOffsetSlot*)((aIterator)->mCurSlot))) )             \
        {                                                                                       \
            (aIterator)->mCurSlot-=2;                                                           \
            (aIterator)->mCurSeq--;                                                             \
        }                                                                                       \
        (aRowItem) = ((aIterator)->mCurSlot == (aIterator)->mSlotFence) ?                       \
            NULL : (stlChar*)(aPageFrame) + *(smpOffsetSlot*)((aIterator)->mCurSlot);           \
    }


#define SMP_GET_NEXT_ROWITEM( aIterator, aRowItem )                                             \
    {                                                                                           \
        for(;;)                                                                                 \
        {                                                                                       \
            (aIterator)->mCurSlot+=2;                                                           \
            ++(aIterator)->mCurSeq;                                                             \
            if( (aIterator)->mCurSlot >= (aIterator)->mSlotFence )                              \
            {                                                                                   \
                (aRowItem) = NULL;                                                              \
                break;                                                                          \
            }                                                                                   \
            else if( !SMP_OFFSET_SLOT_IS_FREE( (smpOffsetSlot*)((aIterator)->mCurSlot)) )       \
            {                                                                                   \
                (aRowItem) = (stlChar*)((aIterator)->mPage) +                                   \
                    *(smpOffsetSlot*)((aIterator)->mCurSlot);                                   \
                break;                                                                          \
            }                                                                                   \
        }                                                                                       \
    }

#define SMP_GET_PREV_ROWITEM( aIterator, aRowItem )                                             \
    {                                                                                           \
        for(;;)                                                                                 \
        {                                                                                       \
            (aIterator)->mCurSlot-=2;                                                           \
            --(aIterator)->mCurSeq;                                                             \
            if( (aIterator)->mCurSlot <= (aIterator)->mSlotFence )                              \
            {                                                                                   \
                (aRowItem) = NULL;                                                              \
                break;                                                                          \
            }                                                                                   \
            else if( !SMP_OFFSET_SLOT_IS_FREE( (smpOffsetSlot*)((aIterator)->mCurSlot)) )       \
            {                                                                                   \
                (aRowItem) = (stlChar*)((aIterator)->mPage) +                                   \
                    *(smpOffsetSlot*)((aIterator)->mCurSlot);                                   \
                break;                                                                          \
            }                                                                                   \
        }                                                                                       \
    }


#define SMP_GET_CURRENT_ROWITEM_SEQ( aIterator ) ((aIterator)->mCurSeq)

#define SMP_INIT_ROWITEM_ITERATOR( aPageFrame, aIterator )                                      \
    {                                                                                           \
        (aIterator)->mPage = (smpPhyHdr*)(aPageFrame);                                          \
        (aIterator)->mHeader = (SMP_GET_OFFSET_TABLE_HEADER((aPageFrame)));                     \
        (aIterator)->mCurSlot = (stlUInt8*)((smpOffsetSlot*)((aIterator)->mHeader + 1) - 1);    \
        (aIterator)->mSlotFence = (stlUInt8*)((smpOffsetSlot*)((aIterator)->mCurSlot) + 1 +     \
                                              (aIterator)->mHeader->mCurrentSlotCount);         \
        (aIterator)->mCurSeq = -1;                                                              \
    }

#define SMP_FOREACH_ROWITEM_ITERATOR( aIterator )               \
    for( (aIterator)->mCurSlot+=2, ++(aIterator)->mCurSeq;      \
         (aIterator)->mCurSlot < (aIterator)->mSlotFence;       \
         (aIterator)->mCurSlot+=2, ++(aIterator)->mCurSeq )

#define SMP_GET_CURRENT_ROWITEM( aIterator )                                            \
    ( (stlChar*)((aIterator)->mPage) + *(smpOffsetSlot*)((aIterator)->mCurSlot) )

#define SMP_IS_FREE_ROWITEM( aIterator )                                        \
    ( SMP_OFFSET_SLOT_IS_FREE( (smpOffsetSlot *)((aIterator)->mCurSlot) ) )

/**
 * @brief record 종류별로 stabilize하는 함수들의 prototype
 */
typedef void (*smpStabilizeRowFunc)( stlChar  * aRow,
                                     stlUInt8   aRtsSeq,
                                     stlUInt8   aLastRtsVerNo,
                                     smlScn     aCommitScn,
                                     stlInt16 * aRefereceCount );
typedef stlInt16 (*smpCountRefFunc)( stlChar  * aPageFrame,
                                     stlChar  * aRow,
                                     stlUInt8   aRtsSeq,
                                     smlEnv   * aEnv);


/* ===================================================== */
/* Slot내의 Header Field 관리용 정보                     */
/* ===================================================== */
#define SMP_SLOT_HDR_MAX_FIELD_CNT  (16)  /**< field의 최대 개수 */
#define SMP_SLOT_HDR_BITMAP_SIZE    (STL_SIZEOF(stlChar))   /**< bitmap을 위한 byte 수 */
/**
 * @brief Slot내의 한 field에 대한 정보
 */
typedef struct smpSlotHdrField
{
    stlChar   mBitMask;
    stlInt16  mFieldOffset;
    stlInt16  mFieldLen;
} smpSlotHdrField;

/**
 * @brief Slot내의 모든 Header Field의 정보
 */
typedef struct smpSlotHdrDesc
{
    stlInt16        mMaxSlotHdrLen;    /**< Slot Header의 최대 크기 */
    stlInt16        mFixedFieldCnt;    /**< 항상 존재하는 field의 개수 */
    stlInt16        mFixedFieldLen;    /**< 항상 존재하는 Field들이 차지하는 총 길이(bitmap 제외) */
    stlInt16        mOptionFieldCnt;   /**< bit에 따라 존재할 수도 있고 아닐 수도 있는 field의 개수 */
    smpSlotHdrField mFieldDesc[SMP_SLOT_HDR_MAX_FIELD_CNT]; /**< Field들의 정보. fixed이후에 option이 온다 */
} smpSlotHdrDesc;


/* ===================================================== */
/* Column Management                                     */
/* ===================================================== */

/**
 * @brief Maximum Column Length의 길이
 */
#define SMP_MAX_COLUMN_LEN_SIZE     (9)

/**
 * @brief 주어진 길이의 컬럼이 저장될때 사용되는 컬럼 길이정보field의 길이를 구한다
 */
#define SMP_GET_COLUMN_LEN_SIZE( aColLen, aColLenSize )                 \
    {                                                                   \
        if( STL_EXPECT_TRUE( (aColLen) <= SMP_COLUMN_LEN_1BYTE_MAX ) )  \
        {                                                               \
            *(stlUInt8*)(aColLenSize) = 1;                              \
        }                                                               \
        else if( ((aColLen) > SMP_COLUMN_LEN_1BYTE_MAX) &&              \
                 ((aColLen) <= SMP_COLUMN_LEN_2BYTE_MAX) )              \
        {                                                               \
            *(stlUInt8*)(aColLenSize) = 3;                              \
        }                                                               \
        else if( ((aColLen) > SMP_COLUMN_LEN_2BYTE_MAX) &&              \
                 ((aColLen) <= SMP_COLUMN_LEN_4BYTE_MAX) )              \
        {                                                               \
            *(stlUInt8*)(aColLenSize) = 5;                              \
        }                                                               \
        else                                                            \
        {                                                               \
            *(stlUInt8*)(aColLenSize) = 9;                              \
        }                                                               \
    }

/**
 * @brief Continuous Column이 저장될 때 사용되는 컬럼 길이정보의 길이를 구한다.
 * Continuous Column flag(1) + next page Id(4) + length(2)
 */
#define SMP_GET_CONT_COLUMN_LEN_SIZE( aColLenSize )     \
    {                                                   \
        *(stlUInt8*)(aColLenSize) = 1 + 4 + 2;          \
    }

/**
 * @brief 주어진 길이의 컬럼이 저장될때 사용되는 컬럼 길이정보field의 길이를 구한다
 */
#define SMP_GET_TOTAL_COLUMN_SIZE( aColPtr, aColSize )                              \
    {                                                                               \
        if( STL_EXPECT_TRUE(*(stlUInt8*)(aColPtr) <= SMP_COLUMN_LEN_1BYTE_MAX) )    \
        {                                                                           \
            aColSize = 1 + *(stlUInt8*)(aColPtr);                                   \
        }                                                                           \
        else                                                                        \
        {                                                                           \
            stlUInt16 tmpInt16;                                                     \
                                                                                    \
            if( *(stlUInt8*)(aColPtr) == SMP_COLUMN_LEN_ZERO )                      \
            {                                                                       \
                aColSize = 1;                                                       \
            }                                                                       \
            else                                                                    \
            {                                                                       \
                STL_WRITE_INT16(&tmpInt16, (stlChar*)(aColPtr) + 1);                \
                aColSize = 3 + (stlInt64)tmpInt16;                                  \
            }                                                                       \
        }                                                                           \
    }

/**
 * @brief 주어진 위치에 저장된 컬럼의 길이와 컬럼 data를 반환하고 포인터를 다음 컬럼으로 이동시킨다
 */
#define SMP_READ_COLUMN_AND_MOVE_PTR( aColPtr, aDataValue, aValueLen )                  \
    {                                                                                   \
        stlUInt8 tmpColLenSize;                                                         \
        stlBool  tmpIsZero;                                                             \
                                                                                        \
        SMP_GET_COLUMN_LEN( *(aColPtr), &tmpColLenSize, (aValueLen), tmpIsZero );       \
        *(aColPtr) += tmpColLenSize;                                                    \
                                                                                        \
        if( (aDataValue)->mBufferSize >= *(aValueLen) )                                 \
        {                                                                               \
            if( tmpIsZero == STL_TRUE )                                                 \
            {                                                                           \
                DTL_NUMERIC_SET_ZERO( (aDataValue)->mValue, (aDataValue)->mLength );    \
            }                                                                           \
            else                                                                        \
            {                                                                           \
                stlMemcpy( (aDataValue)->mValue, *(aColPtr), *(aValueLen) );            \
                (aDataValue)->mLength = *(aValueLen);                                   \
            }                                                                           \
        }                                                                               \
        *(aColPtr) += *(aValueLen);                                                     \
    }

/**
 * @brief 주어진 위치에 저장된 컬럼의 길이와 컬럼 data를 반환한다.
 */
#define SMP_READ_COLUMN_ZERO_INSENS( aColPtr, aValue, aValueLen )                   \
    {                                                                               \
        stlUInt8 tmpColLenSize;                                                     \
                                                                                    \
        SMP_GET_COLUMN_LEN_ZERO_INSENS( (aColPtr), &tmpColLenSize, (aValueLen) );   \
        stlMemcpy( (aValue), ((aColPtr) + tmpColLenSize), *(aValueLen) );           \
    }

/**
 * @brief 주어진 위치에 주어진 컬럼 data를 저장하고 포인터를 다음 컬럼으로 이동시킨다
 * @note aColPtr은 stlChar를 가정하고 있다.
 */
#define SMP_WRITE_COLUMN_AND_MOVE_PTR( aColPtr, aValue, aValueLen, aIsZero )    \
    {                                                                           \
        stlUInt8 tmpColLenSize;                                                 \
        if( STL_EXPECT_TRUE( (aValueLen) <= SMP_COLUMN_LEN_1BYTE_MAX ) )        \
        {                                                                       \
            if( aIsZero == STL_TRUE )                                           \
            {                                                                   \
                *(stlUInt8*)(aColPtr) = SMP_COLUMN_LEN_ZERO;                    \
                (aColPtr) += 1;                                                 \
            }                                                                   \
            else                                                                \
            {                                                                   \
                *(stlUInt8*)(aColPtr) = (stlUInt8)(aValueLen);                  \
                (aColPtr) += 1;                                                 \
                stlMemcpy( (aColPtr), (aValue), (aValueLen) );                  \
                (aColPtr) += (aValueLen);                                       \
            }                                                                   \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            stlUInt16 tmpInt16 = (aValueLen);                                   \
            tmpColLenSize = 3;                                                  \
            *(aColPtr) = SMP_COLUMN_LEN_2BYTE_FLAG;                             \
            STL_WRITE_INT16( (aColPtr) + 1, &tmpInt16 );                        \
            (aColPtr) += tmpColLenSize;                                         \
            stlMemcpy( (aColPtr), (aValue), (aValueLen) );                      \
            (aColPtr) += (aValueLen);                                           \
        }                                                                       \
    }

/**
 * @brief 주어진 위치에 주어진 컬럼 data를 저장하다.
 */
#define SMP_WRITE_COLUMN( aColPtr, aValue, aValueLen, aIsZero )                             \
    {                                                                                       \
        stlUInt8 tmpColLenSize;                                                             \
        if( STL_EXPECT_TRUE( (aValueLen) <= SMP_COLUMN_LEN_1BYTE_MAX ) )                    \
        {                                                                                   \
            if( aIsZero == STL_TRUE )                                                       \
            {                                                                               \
                *(stlUInt8*)(aColPtr) = SMP_COLUMN_LEN_ZERO;                                \
            }                                                                               \
            else                                                                            \
            {                                                                               \
                tmpColLenSize = 1;                                                          \
                *(stlUInt8*)(aColPtr) = (stlUInt8)(aValueLen);                              \
                stlMemcpy( ((stlUInt8*)(aColPtr) + tmpColLenSize), (aValue), (aValueLen) ); \
            }                                                                               \
        }                                                                                   \
        else                                                                                \
        {                                                                                   \
            stlUInt16 tmpInt16 = (aValueLen);                                               \
            tmpColLenSize = 3;                                                              \
            *(stlUInt8*)(aColPtr) = SMP_COLUMN_LEN_2BYTE_FLAG;                              \
            STL_WRITE_INT16( (stlUInt8*)(aColPtr) + 1, &tmpInt16 );                         \
            stlMemcpy( ((stlUInt8*)(aColPtr) + tmpColLenSize), (aValue), (aValueLen) );     \
        }                                                                                   \
    }

/**
 * @brief 주어진 위치에 주어진 컬럼 data를 저장하다.
 */
#define SMP_WRITE_COLUMN_ZERO_INSENS( aColPtr, aValue, aValueLen )                      \
    {                                                                                   \
        stlUInt8 tmpColLenSize;                                                         \
        if( STL_EXPECT_TRUE( (aValueLen) <= SMP_COLUMN_LEN_1BYTE_MAX ) )                \
        {                                                                               \
            tmpColLenSize = 1;                                                          \
            *(stlUInt8*)(aColPtr) = (stlUInt8)(aValueLen);                              \
            stlMemcpy( ((stlUInt8*)(aColPtr) + tmpColLenSize), (aValue), (aValueLen) ); \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            stlUInt16 tmpInt16 = (aValueLen);                                           \
            tmpColLenSize = 3;                                                          \
            *(stlUInt8*)(aColPtr) = SMP_COLUMN_LEN_2BYTE_FLAG;                          \
            STL_WRITE_INT16( (stlUInt8*)(aColPtr) + 1, &tmpInt16 );                     \
            stlMemcpy( ((stlUInt8*)(aColPtr) + tmpColLenSize), (aValue), (aValueLen) ); \
        }                                                                               \
    }

#define SMP_SKIP_AND_MOVE_COLUMN( aColPtr, aColLen )                    \
    {                                                                   \
        aColLen = *(stlUInt8*)(aColPtr);                                \
        if( STL_EXPECT_TRUE( aColLen <= SMP_COLUMN_LEN_1BYTE_MAX ) )    \
        {                                                               \
            (aColPtr) += aColLen;                                       \
            (aColPtr)++;                                                \
        }                                                               \
        else                                                            \
        {                                                               \
            stlUInt16 tmpInt16;                                         \
                                                                        \
            if( aColLen == SMP_COLUMN_LEN_ZERO )                        \
            {                                                           \
                (aColPtr)++;                                            \
            }                                                           \
            else                                                        \
            {                                                           \
                STL_WRITE_INT16(&tmpInt16, (stlChar*)(aColPtr) + 1);    \
                (aColPtr) += 3 + tmpInt16;                              \
            }                                                           \
        }                                                               \
    }

/**
 * @brief 주어진 위치에 저장된 컬럼의 길이와 컬럼 data를 반환한다.
 */
#define SMP_GET_COLUMN_LEN_ZERO_INSENS( aColPtr, aColLenSize, aColLen )             \
    {                                                                               \
        if( STL_EXPECT_TRUE(*(stlUInt8*)(aColPtr) <= SMP_COLUMN_LEN_1BYTE_MAX) )    \
        {                                                                           \
            *(aColLenSize) = 1;                                                     \
            *(aColLen) = *(stlUInt8*)(aColPtr);                                     \
        }                                                                           \
        else                                                                        \
        {                                                                           \
            stlUInt16 tmpInt16;                                                     \
                                                                                    \
            STL_WRITE_INT16(&tmpInt16, (stlChar*)(aColPtr) + 1);                    \
            *(aColLen) = (stlInt64)tmpInt16;                                        \
            *(aColLenSize) = 3;                                                     \
        }                                                                           \
    }


/* ===================================================== */
/* Slot Header Management                                */
/* ===================================================== */
/**
 * @brief 주어진 rowitem의 slot header의 크기를 구한다
 */
#define SMP_GET_SLOT_HDR_SIZE( aRowItem, aHdrDesc, aTotalLen )                                    \
{                                                                                                 \
    smpSlotHdrField * tmpCurField = &((aHdrDesc)->mFieldDesc[(aHdrDesc)->mFixedFieldCnt]);        \
    stlInt32          tmpIdx;                                                                     \
    *(aTotalLen) = SMP_SLOT_HDR_BITMAP_SIZE + (aHdrDesc)->mFixedFieldLen;                         \
    for( tmpIdx = 0; tmpIdx < (aHdrDesc)->mOptionFieldCnt; tmpIdx++ )                             \
    {                                                                                             \
        if( SMP_IS_SLOT_HDR_BIT_SET( (stlChar*)(aRowItem),                                        \
                                     (aHdrDesc),                                                  \
                                     (aHdrDesc)->mFixedFieldCnt + tmpIdx ) == STL_TRUE )          \
        {                                                                                         \
            *(aTotalLen) += tmpCurField->mFieldLen;                                               \
        }                                                                                         \
        tmpCurField++;                                                                            \
    }                                                                                             \
}
 
/**
 * @brief 주어진 rowitem의 slot header의 bitmap을 초기화한다
 */
#define SMP_INIT_SLOT_HDR_BITMAP( aRowItem )  { *(stlChar*)(aRowItem) = 0; }

/**
 * @brief 주어진 rowitem의 bitmap에 주어진 field의 bit가 세팅되어있는지 체크한다
 */
#define SMP_IS_SLOT_HDR_BIT_SET( aRowItem, aHdrDesc, aFieldId )                 \
    ( ((*(stlChar*)(aRowItem)) & (aHdrDesc)->mFieldDesc[(aFieldId)].mBitMask)   \
          == (aHdrDesc)->mFieldDesc[(aFieldId)].mBitMask                        \
              ? STL_TRUE : STL_FALSE )

/**
 * @brief 주어진 rowitem의 bitmap에 주어진 field의 bit를 세팅한다
 */
#define SMP_SET_SLOT_HDR_BIT( aRowItem, aHdrDesc, aFieldId )                                        \
{                                                                                                   \
    *(stlChar*)(aRowItem) = (*(stlChar*)(aRowItem)) | (aHdrDesc)->mFieldDesc[(aFieldId)].mBitMask;  \
}

/**
 * @brief 주어진 rowitem의 bitmap에 주어진 field의 bit를 세팅되지 않는 상태로 한다
 */
#define SMP_UNSET_SLOT_HDR_BIT( aRowItem, aHdrDesc, aFieldId )                                           \
{                                                                                                        \
    *(stlChar*)(aRowItem) = (*(stlChar*)(aRowItem)) & (~((aHdrDesc)->mFieldDesc[(aFieldId)].mBitMask));  \
}

/**
 * @brief 주어진 rowitem slot header의 주어진 field 시작위치를 반환한다
 */
#define SMP_GET_SLOT_HDR_FIELD_PTR( aRowItem, aHdrDesc, aFieldId, aRetPtr )                        \
{                                                                                                  \
    stlInt32   tmpIdx;                                                                             \
    smpSlotHdrField * tmpField = &((aHdrDesc)->mFieldDesc[(aFieldId)]);                            \
    if( (aFieldId) < (aHdrDesc)->mFixedFieldCnt )                                                  \
    {                                                                                              \
        *(aRetPtr) = (stlChar*)(aRowItem) + SMP_SLOT_HDR_BITMAP_SIZE + tmpField->mFieldOffset;     \
    }                                                                                              \
    else                                                                                           \
    {                                                                                              \
        *(aRetPtr) = (stlChar*)(aRowItem) + SMP_SLOT_HDR_BITMAP_SIZE +                             \
            (aHdrDesc)->mFixedFieldLen;                                                            \
        for( tmpIdx = (aHdrDesc)->mFixedFieldCnt; tmpIdx < (aFieldId); tmpIdx++ )                  \
        {                                                                                          \
            if( ((*(stlChar*)(aRowItem)) & (aHdrDesc)->mFieldDesc[tmpIdx].mBitMask)                \
                == (aHdrDesc)->mFieldDesc[tmpIdx].mBitMask )                                       \
            {                                                                                      \
                *(aRetPtr) += (aHdrDesc)->mFieldDesc[tmpIdx].mFieldLen;                            \
            }                                                                                      \
        }                                                                                          \
    }                                                                                              \
}

/**
 * @brief 주어진 rowitem slot header의 주어진 field의 값을 가져온다
 */
#define SMP_GET_SLOT_HDR_FIELD( aRowItem, aHdrDesc, aFieldId, aValue )                   \
{                                                                                        \
    stlChar * tmpPtr;                                                                    \
    SMP_GET_SLOT_HDR_FIELD_PTR( (stlChar*)(aRowItem), (aHdrDesc), (aFieldId), &tmpPtr ); \
    switch( (aHdrDesc)->mFieldDesc[(aFieldId)].mFieldLen )                               \
    {                                                                                    \
        case 1 :                                                                         \
            {                                                                            \
                *(stlChar*)(aValue) = *tmpPtr;                                           \
                break;                                                                   \
            }                                                                            \
        case 2 :                                                                         \
            {                                                                            \
                STL_WRITE_INT16( (aValue), tmpPtr );                                     \
                break;                                                                   \
            }                                                                            \
        case 4 :                                                                         \
            {                                                                            \
                STL_WRITE_INT32( (aValue), tmpPtr );                                     \
                break;                                                                   \
            }                                                                            \
        case 8 :                                                                         \
            {                                                                            \
                STL_WRITE_INT64( (aValue), tmpPtr );                                     \
                break;                                                                   \
            }                                                                            \
        default :                                                                        \
            {                                                                            \
                STL_ASSERT( STL_FALSE );                                                 \
            }                                                                            \
    }                                                                                    \
}

/**
 * @brief 주어진 rowitem slot header의 주어진 field의 값으로 세팅한다
 */
#define SMP_SET_SLOT_HDR_FIELD( aRowItem, aHdrDesc, aFieldId, aValue )                   \
{                                                                                        \
    stlChar * tmpPtr;                                                                    \
    SMP_GET_SLOT_HDR_FIELD_PTR( (stlChar*)(aRowItem), (aHdrDesc), (aFieldId), &tmpPtr ); \
    switch( (aHdrDesc)->mFieldDesc[(aFieldId)].mFieldLen )                               \
    {                                                                                    \
        case 1 :                                                                         \
            {                                                                            \
                *tmpPtr = *(stlChar*)(aValue);                                           \
                break;                                                                   \
            }                                                                            \
        case 2 :                                                                         \
            {                                                                            \
                STL_WRITE_INT16( tmpPtr, (aValue) );                                     \
                break;                                                                   \
            }                                                                            \
        case 4 :                                                                         \
            {                                                                            \
                STL_WRITE_INT32( tmpPtr, (aValue) );                                     \
                break;                                                                   \
            }                                                                            \
        case 8 :                                                                         \
            {                                                                            \
                STL_WRITE_INT64( tmpPtr, (aValue) );                                     \
                break;                                                                   \
            }                                                                            \
        default :                                                                        \
            {                                                                            \
                STL_ASSERT( STL_FALSE );                                                 \
            }                                                                            \
    }                                                                                    \
}

#define SMP_GET_NTH_ROW_ITEM( aPageHandle, aOffsetTableHeader, aRowItemSeq )    \
    ( (stlChar*)SMP_FRAME(aPageHandle) +                                        \
      *((smpOffsetSlot*)(aOffsetTableHeader + 1) + aRowItemSeq) )

/**
 * @brief page 내의 모든 row item의 개수을 반환한다
 * @param[in] aPageHandle 조사할 page의 handle
 */
#define SMP_GET_PAGE_ROW_ITEM_COUNT( aPageHandle )                              \
    ((SMP_GET_OFFSET_TABLE_HEADER(SMP_FRAME(aPageHandle)))->mCurrentSlotCount)

/** @} */

/**
 * @addtogroup smt
 * @{
 */

/** @} */


/**
 * @addtogroup smxm
 * @{
 */

/**
 * @brief Mini-Transaction 로그 블록들의 최대 개수
 */
#define SMXM_MAX_LOGBLOCK_COUNT  (32)
/**
 * @brief 기록할수 있는 Mini-Transaction Undo 로그의 최대 개수
 * block insert시 mini-transaction에 하나의 page에 기록할 수 있는
 * 최대 레코드 수만큼의 undo log가 기록될 수 있고, slot header의
 * 최소 크기가 37byte이므로 약 200여개의 undo log가 기록될 수 있다.
 */
#define SMXM_MAX_UNDO_COUNT      (256)
/**
 * @brief Mini-Transaction이 가질수 있는 로그 블록들의 바이트 단위 최대 크기
 */
#define SMXM_MAX_LOGBLOCK_SIZE   (SMXM_MAX_LOGBLOCK_COUNT * SMR_MAX_LOGPIECE_SIZE)

/**
 * @brief Mini-Transaction의 로그 블록 슬롯 구조체
 */
typedef struct smxmLogBlockSlot
{
    stlUInt16   mSize;          /**< Log Block내 저장된 데이터 크기 */
    void      * mBuffer;        /**< Log Block 포인터 */
} smxmLogBlockSlot;

/**
 * @brief Mini-Transaction의 로그 블록 구조체
 */
typedef struct smxmLogBlock
{
    stlInt16         mBlockCount;                                 /**< 사용중인 Log Block의 개수 */
    smxmLogBlockSlot mLogBlockSlotArray[SMXM_MAX_LOGBLOCK_COUNT]; /**< 로그 블록 Array */
} smxmLogBlock;

/**
 * @brief Mini-Transaction 구조체
 */
struct smxmTrans
{
    smxlTransId     mTransId;                                 /**< 트랜잭션 아이디 */
    smlTbsId        mDataTbsId;                               /**< 데이터 테이블스페이스 아이디 */
    stlUInt32       mRedoLogPieceCount;                       /**< 저장된 Redo Log Piece의 개수 */
    stlUInt32       mTryRedoLogPieceCount;                    /**< Redo Log 저장을 시도한 회수 */
    stlUInt32       mUndoLogPieceCount;                       /**< 저장된 Undo Log Piece의 개수 */ 
    stlInt16        mPageStackTop;                            /**< Page Stack의 Top */ 
    stlUInt32       mAttr;                                    /**< 속성 @see smxmAttr */ 
    smlRid          mSegRid;                                  /**< 관련 Segment의 Rid */ 
    knlRegionMark   mMemMark;                                 /**< Memory Mark */ 
    smpHandle       mPageStack[KNL_ENV_MAX_LATCH_DEPTH];      /**< Page Stack */ 
    smxmLogBlock    mRedoLogBlock;                            /**< Redo Log Block */ 
    stlUInt32       mRedoLogSize;                             /**< 기록된 Total Redo Log Size */
    stlInt16        mMiniTransStackIdx;                       /**< Index of Exclusive Mini-Transaction Stack */
    stlBool         mLogging;                                 /**< logging flag */
    void          * mUndoLogPieceAnchor[SMXM_MAX_UNDO_COUNT]; /**< Undo Log Piece Anchor */ 
};

/**
 * @brief Mini-Transaction의 Savepoint 구조체
 */
typedef struct smxmSavepoint
{
    stlInt16      mPageStackTop;               /**< Page Stack의 Top */
    stlInt16      mRedoLogBlockCount;          /**< Redo Log Block Count */ 
    stlUInt16     mRedoLogBlockSize;           /**< 마지막 Redo Log Block의 Size */ 
    stlUInt16     mRedoLogSize;                /**< Redo Log Size */ 
    stlUInt16     mRedoLogPieceCount;          /**< Redo Log Piece Count */ 
    stlInt16      mTryRedoLogPieceCount;       /**< Redo Log 저장을 시도한 회수 */ 
    stlUInt16     mUndoLogPieceCount;          /**< Undo Log Piece Count */ 
    knlRegionMark mMemMark;                    /**< Memory Mark */ 
} smxmSavepoint;

/**
 * @defgroup smxmLogging Mini-Transaction Logging
 * @ingroup smxm
 * @{
 */
#define SMXM_LOGGING_REDO      (1)    /**< Redo Log를 Logging하라 */
#define SMXM_LOGGING_REDO_UNDO (2)    /**< Redo Log와 Undo Log를 Logging하라 */
/** @} */

/**
 * @brief Log Block Cursor 초기화 매크로
 */
#define SMXM_INIT_LOG_BLOCK_CURSOR( aLogBlockCursor ) \
{                                                     \
    (aLogBlockCursor)->mCurBlockIdx = 0;              \
    (aLogBlockCursor)->mCurOffset = 0;                \
}

/**
 * @brief Log Block Cursor 구조체
 */
typedef struct smxmLogBlockCursor
{
    stlInt16 mCurBlockIdx;              /**< Current Log Block Index */
    stlInt16 mCurOffset;                /**< 읽은 위치 */
} smxmLogBlockCursor;

/**
 * @defgroup smxmAttr Mini-Transaction Attribute
 * @ingroup smxm
 * @{
 */
#define SMXM_ATTR_REDO             (0x00000001)   /**< Redo log를 허용한다. */
#define SMXM_ATTR_NO_VALIDATE_PAGE (0x00000002)   /**< Page를 Validate 한다. */
#define SMXM_ATTR_NOLOG_DIRTY      (0x00000004)   /**< No Logging이지만 Exclusive Page에
                                                      대해서 페이지를 Dirty시킨다. */
/** @} */

/** @} */


/**
 * @addtogroup smn
 * @{
 */

/**
 * @brief index block operation시 buffer에 저장할수 있는 최대 키 개수
 */
#define SMN_MAX_BLOCK_KEY_COUNT      (100)

/**
 * @brief Block Key Buffer 의 최대 크기
 * [used count][statement view scn][page_id \#1][offset \#1][ ... ][page_id \#n][offset \#n]
 */
#define SMN_MAX_BLOCK_KEY_BUFFER_SIZE                       \
    ( STL_SIZEOF(stlChar) +                                 \
      STL_SIZEOF(smlScn) +                                  \
      STL_SIZEOF(smlTcn) +                                  \
      (SMN_MAX_BLOCK_KEY_COUNT * STL_SIZEOF(smlPid)) +      \
      (SMN_MAX_BLOCK_KEY_COUNT * STL_SIZEOF(stlInt16)) )

#define SMN_MAX_KEY_BODY_SIZE (2000)

/** @} */

/**
 * @addtogroup smxl
 * @{
 */

#define SMXL_INIT_SYSTEM_SCN   (3)  /**< 최초 SYSTEM SCN */

#define SMXL_IS_AGABLE_VIEW_SCN( aAgableViewScn, aViewScn ) \
    ( ((aAgableViewScn) > (aViewScn)) ? STL_TRUE : STL_FALSE )

/**
 * @brief undo record header
 */
typedef struct smxlUndoRecHdr
{
    stlInt16   mType;                /**< log type */
    stlChar    mComponentClass;      /**< component class */
    stlChar    mFlag;                /**< delete flag */
    smlPid     mUndoSegPid;          /**< undo segment page identifier */ 
    smlPid     mTargetSegPid;        /**< target segment page identifier */ 
    stlInt16   mSize;                /**< undo body size */ 
    stlBool    mRedoLogged;          /**< need restart undoing ? */
    stlChar    mPadding[1];
    smlRid     mTargetDataRid;       /**< target data rid */
} smxlUndoRecHdr;

/**
 * @brief undo record header size
 */
#define SMXL_UNDO_REC_HDR_SIZE ( STL_SIZEOF( smxlUndoRecHdr ) )

/**
 * @brief read undo record header
 */
#define SMXL_READ_MOVE_UNDO_REC_HDR( aDest, aSrc, aOffset )                     \
    {                                                                           \
        stlMemcpy( (aDest), (aSrc) + (aOffset), STL_SIZEOF( smxlUndoRecHdr ) ); \
        (aOffset) += STL_SIZEOF( smxlUndoRecHdr );                              \
    }
#define SMXL_READ_UNDO_REC_HDR( aDest, aSrc )                       \
    {                                                               \
        stlMemcpy( (aDest), (aSrc), STL_SIZEOF( smxlUndoRecHdr ) ); \
    }

/**
 * @defgroup smxlState Transaction State
 * @ingroup smxl
 * @{
 */
#define SMXL_STATE_ACTIVE             1   /**< Active 상태 */
#define SMXL_STATE_BLOCK              2   /**< Block된 상태 */
#define SMXL_STATE_PREPARE            3   /**< Prepare된 상태 */
#define SMXL_STATE_COMMIT             4   /**< Commit된 상태 */
#define SMXL_STATE_ROLLBACK           5   /**< Rollback된 상태 */
#define SMXL_STATE_IDLE               6   /**< 유휴상태 */
#define SMXL_STATE_PRECOMMIT          7   /**< In-Memory Commit */
/** @} */

/**
 * @brief 트랜잭션 슬롯 구조체
 */
typedef struct smxlTrans
{
    knlLatch            mLatch;              /**< 동기화 latch */
    stlUInt32           mState;              /**< 트랜잭션 상태 */
    smlIsolationLevel   mIsolationLevel;     /**< 트랜잭션 독립성 레벨 */
    smlScn              mTransViewScn;
    smlScn              mCommitScn;
    smrSbsn             mCommitSbsn;
    smrLsn              mCommitLsn;
    stlUInt64           mTransSeq;
    stlInt64            mTryLogCount;    /**< Logging을 시도한 회수 */
    void              * mUndoRelEntry;
    smlRid              mUndoSegRid;
    smlTcn              mTcn;
    stlInt32            mAttr;
    stlBool             mWrittenTransRecord;
    stlBool             mPropagateLog;
    stlBool             mRepreparable;
    stlBool             mIsGlobal;
    stlInt64            mTimestamp;
    smxlTransId         mTransId;
    stlTime             mBeginTime;
    stlRingHead         mSavepointList;     /**< Savepoint 리스트 */
    knlRegionMem        mShmMem;
    smrLsn              mBeginLsn;          /**< transaction begin lsn */
    stlInt64            mUsedUndoPageCount; /**< used undo page count */
    stlChar             mPadding2[40];      /**< Padding for cache line */
} smxlTrans;


/**
 * @brief 트랜잭션 snapshot 구조체
 */
typedef struct smxlTransSnapshot
{
    smlRid             mUndoSegRid;      /**< undo segment rid */
    stlUInt32          mState;           /**< transaction state */
    stlBool            mRepreparable;    /**< re-preparable flag */
    stlBool            mUpdatable;       /**< updatable flag */
    smlIsolationLevel  mIsolationLevel;  /**< isolation level */
    smlScn             mTransViewScn;    /**< view scn */
} smxlTransSnapshot;

typedef struct smxlSavepoint
{
    stlChar           mName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    smlRid            mLstUndoRid;
    stlInt64          mTimestamp;
    smlLockSavepoint  mLockSavepoint;
    stlRingEntry      mSavepointLink;
} smxlSavepoint;

typedef stlStatus (*smxlUndoFunc)( smxmTrans * aMiniTrans,
                                   smlRid      aTargetRid,
                                   void      * aLogBody,
                                   stlInt16    aLogSize,
                                   smlEnv    * aEnv );

/**
 * @brief Checkpoint 트랜잭션 아이디
 */
#define SMXL_CHKPT_TRANSID    STL_INT64_C(0xFFFFFFFFFFFFFFFF - 1)

/**
 * @brief System 트랜잭션 아이디
 */
#define SMXL_SYSTEM_TRANSID   STL_INT64_C(0xFFFFFFFFFFFFFFFF - 2)

#define SMXL_TRANS_PAGE_PRECISION       32
#define SMXL_TRANS_OFFSET_PRECISION     16
#define SMXL_TRANS_SLOT_PRECISION       16
#define SMXL_TRANS_PAGE_MASK            STL_INT64_C(0xFFFFFFFF00000000)
#define SMXL_TRANS_OFFSET_MASK          STL_INT64_C(0x00000000FFFF0000)
#define SMXL_TRANS_SLOT_MASK            STL_INT64_C(0x0000000000003FFF)
#define SMXL_SYSTEM_TRANS_MASK          STL_INT64_C(0x0000000000008000)
#define SMXL_MEDIA_RECOVERY_TRANS_MASK  STL_INT64_C(0x0000000000004000)
#define SMXL_TRANS_SLOT_ALL_MASK        STL_INT64_C(0x000000000000FFFF)

#define SMXL_MEDIA_RECOVERY_TRANS_ID( aTransId )        \
    ((aTransId) | SMXL_MEDIA_RECOVERY_TRANS_MASK)

#define SMXL_MAKE_TRANS_ID( aTransPid, aTransOffset, aTransSlotId )                \
(                                                                                  \
    ( (stlInt64)aTransPid    << (SMXL_TRANS_OFFSET_PRECISION +                     \
                                 SMXL_TRANS_SLOT_PRECISION) ) +                    \
    ( (stlInt64)aTransOffset << SMXL_TRANS_SLOT_PRECISION ) +                      \
    ( (stlInt64)aTransSlotId )                                                     \
)

#define SMXL_TRANS_SLOT_ID( aTransId ) ( (stlInt16)(aTransId & SMXL_TRANS_SLOT_MASK) )
#define SMXL_GET_NEXT_TRANS_SLOT_ID( aTransId ) \
    ( ((stlInt16)((aTransId) & SMXL_TRANS_SLOT_MASK) == SMXL_TRANS_SLOT_MASK)?    \
      0 : (stlInt16)((aTransId) & SMXL_TRANS_SLOT_MASK) + 1 )

#define SMXL_TRANS_TABLE( aSlotId )     ( gSmxlTransTable[(aSlotId)] )
#define SMXL_TRANS_SLOT( aTransId ) ( &SMXL_TRANS_TABLE(SMXL_TRANS_SLOT_ID(aTransId)) )

#define SMXL_TO_TRANS_ID( aSmlTransId )                                     \
    (smxlTransId) ( SMXL_TRANS_PAGE_MASK   |                                \
                    SMXL_TRANS_OFFSET_MASK |                                \
                    (stlInt16)((aSmlTransId) & SMXL_TRANS_SLOT_ALL_MASK) )

#define SMXL_TO_SML_TRANS_ID( aSmxlTransId, aTransSeq )             \
    ((aSmxlTransId) == SML_INVALID_TRANSID) ? SML_INVALID_TRANSID : \
    (smlTransId)( ((aTransSeq) << SMXL_TRANS_SLOT_PRECISION) |      \
                  ((aSmxlTransId) & SMXL_TRANS_SLOT_ALL_MASK) )
#define SMXL_TRANS_SEQ( aSmlTransId )               \
    ( ((stlUInt64)(aSmlTransId)) >> SMXL_TRANS_SLOT_PRECISION )

#define SMXL_TRANS_OFFSET( aTransId )                                              \
(                                                                                  \
    (stlInt16)(((aTransId) & SMXL_TRANS_OFFSET_MASK) >> SMXL_TRANS_SLOT_PRECISION) \
)

#define SMXL_TRANS_PID( aTransId )                                                 \
(                                                                                  \
    (smlPid)((aTransId & SMXL_TRANS_PAGE_MASK) >> (SMXL_TRANS_OFFSET_PRECISION +   \
                                                  SMXL_TRANS_SLOT_PRECISION))      \
)

#define SMXL_TRANS_RID( aTransId )                                                 \
(                                                                                  \
    (smlRid){ SML_MEMORY_UNDO_SYSTEM_TBS_ID,                                       \
            SMXL_TRANS_OFFSET( aTransId ),                                         \
            SMXL_TRANS_PID( aTransId ) }                                           \
)

#define SMXL_IS_UNDO_BOUND_TRANS( aTransId )                                           \
(                                                                                      \
    (SMXL_TRANS_OFFSET( aTransId ) == SML_INVALID_RID_OFFSET) ? STL_FALSE : STL_TRUE   \
)

/**
 * @brief Checkpoint 트랜잭션 아이디
 */
#define SMXL_IS_SYSTEM_TRANS( aTransId )                                        \
(                                                                               \
    ((SMXL_SYSTEM_TRANS_MASK & (aTransId)) == SMXL_SYSTEM_TRANS_MASK) ?         \
    STL_TRUE : STL_FALSE                                                        \
)

/**
 * @defgroup smxlAttr Transaction Attribute
 * @ingroup smxl
 * @{
 */
#define SMXL_ATTR_NONE                (0x00000000)   /**< 초기 상태 */
#define SMXL_ATTR_UPDATABLE           (0x00000001)   /**< UPDATABLE TRANSACTION */
#define SMXL_ATTR_LOCKABLE            (0x00000002)   /**< LOCKABLE TRANSACTION */
/** @} */

/**
 * @brief Transaction Level Memory Chunk 크기
 */
#define SMXL_MEM_CHUNK_SIZE ( 1024 )

typedef struct smxlSystemInfo
{
    smlScn    mSystemScn;
    smlScn    mMinTransViewScn;
    smlScn    mAgableScn;
    smlScn    mAgableStmtScn;
    smlScn    mAgableViewScn;
    stlInt64  mTryStealUndoCount;
    stlInt64  mDataStoreMode;
    stlInt64  mTransTableSize;
    stlInt64  mUndoRelCount;
} smxlSystemInfo;

/**
 * @brief Transaction Table의 최대 크기
 * : TRANSACTION_TABLE_SIZE property의 최대값 참조
 */
#define SMXL_MAX_TRANS_TABLE_SIZE   (10240)

/**
 * @brief Media recover를 위한 transaction table을 session env에 할당하고 참조하기 위한 매크로
 */
#define SMXL_SET_MEDIA_RECOVERY_TRANS_TABLE( aEnv, aTransTable )      \
{                                                                     \
    SMR_GET_MEDIA_RECOVERY_INFO(aEnv)->mTransTable = (aTransTable);   \
}

#define SMXL_GET_MEDIA_RECOVERY_TRANS_TABLE( aEnv ) \
    ( (smxlTrans *)(SMR_GET_MEDIA_RECOVERY_INFO(aEnv)->mTransTable) )

/**
 * Media recovery session인지 판단
 */
#define SMR_PROCEED_MEDIA_RECOVERY( aEnv )  (SMR_GET_MEDIA_RECOVERY_INFO(aEnv) != NULL)

/** @} */

/**
 * @defgroup smi Iterator Internal Definitions
 * @ingroup smInternal
 * @{
 */

/* Every Relations SHOULD provide these function prototypes */
typedef stlStatus (*smiInitializeFunc) ( void   * aIterator,
                                         smlEnv * aEnv );

typedef stlStatus (*smiFinalizeFunc) ( void   * aIterator,
                                       smlEnv * aEnv );

typedef stlStatus (*smiResetFunc) ( void                      * aIterator, 
                                    smlEnv                    * aEnv );

typedef stlStatus (*smiFetchNextFunc) ( void          * aIterator,
                                        smlFetchInfo  * aFetchInfo,
                                        smlEnv        * aEnv );

typedef stlStatus (*smiFetchPrevFunc) ( void          * aIterator,
                                        smlFetchInfo  * aFetchInfo,
                                        smlEnv        * aEnv );

typedef stlStatus (*smiFetchAggrFunc) ( void          * aIterator,
                                        smlFetchInfo  * aFetchInfo,
                                        smlEnv        * aEnv );

typedef stlStatus (*smiFetchSamplingFunc) ( void          * aIterator,
                                            smlFetchInfo  * aFetchInfo,
                                            stlInt64        aSamplingPageCount,
                                            smlEnv        * aEnv );

typedef stlStatus (*smiMoveToRowIdFunc) ( void   * aIterator, 
                                          smlRid * aRowRid,
                                          smlEnv * aEnv );

typedef stlStatus (*smiMoveToPosNumFunc) ( void    * aIterator,
                                           stlInt64  aPosNum,
                                           smlEnv  * aEnv );

typedef stlStatus (*smiMoveToNextKeyFunc) ( void          * aIterator,
                                            smlFetchInfo  * aFetchInfo,
                                            smlEnv        * aEnv );

typedef stlStatus (*smiResetGroupFunc)( void          *aIterator,
                                        smlEnv        *aEnv );


/** TODO : smlIteratorGetCount 구현 필요 */

typedef enum smiIteratorState
{
    SMI_STATE_ACTIVE,
    SMI_STATE_CLOSED,
    SMI_STATE_FREED
} smiIteratorState;

/**
 * @brief Macro for Iterator::mViewScn
 */
#define SMI_GET_VIEW_SCN( aIterator ) ( ((smiIterator*)(aIterator))->mViewScn )

/**
 * @brief Iterator 구조체
 */
typedef struct smiIterator
{
    void                     * mRelationHandle;
    void                     * mIteratorModule;
    smlTransReadMode           mTransReadMode; 
    smlStmtReadMode            mStmtReadMode; 
    smlStatement             * mStatement;        /**< Iterator를 소유한 Statement */
    smlScn                     mValidObjectScn;   /**< Iterator가 시작할때 설정된 Object SCN */
    smlScn                     mViewScn;
    smlTcn                     mViewTcn;          /**< Iterator의 View 일관성 유지 */
    smlIteratorProperty        mProperty;

    knlRegionMarkEx          * mRegionMarkEx;     /**< Iterator Memory Mark */
    
    smlIteratorScanDirection   mScanDirection;
    smiIteratorState           mState;
    stlRingEntry               mIteratorLink;     /**< Iterator Link */
    smpRowItemIterator         mSlotIterator;
    
    smiFinalizeFunc            mFiniIterator;
    smiResetFunc               mResetIterator;
    smiFetchNextFunc           mFetchNext;
    smiFetchPrevFunc           mFetchPrev;
    smiFetchAggrFunc           mFetchAggr;
    smiFetchSamplingFunc       mFetchSampling;
    smiMoveToRowIdFunc         mMoveToRowId;
    smiMoveToPosNumFunc        mMoveToPosNum;

    /* group iterator를 위한 함수 */
    smiMoveToNextKeyFunc       mMoveToNextKey;
    smiResetGroupFunc          mResetGroup;
} smiIterator;

/** @} */

/**
 * @defgroup smeDef Relation Internal Definitions
 * @ingroup smInternal
 * @{
 */

#define SME_VALIDATION_BEHAVIOR_MASK    (0x0000000F)
#define SME_VALIDATION_BEHAVIOR_READ    (0x00000001)
#define SME_VALIDATION_BEHAVIOR_UPDATE  (0x00000002)

#define SME_RELATION_STATE_ACTIVE    (1)
#define SME_RELATION_STATE_DROPPING  (2)

typedef struct smeRelationHeader
{
    stlUInt16    mRelationType;
    stlUInt16    mRelHeaderSize;
    stlUInt16    mRelState;
    stlChar      mPadding[2];
    smxlTransId  mCreateTransId;
    smxlTransId  mDropTransId;
    smlScn       mCreateScn;
    smlScn       mDropScn;
    smlScn       mObjectScn;       /**< volatile 정보. 시스템 restart시 0으로 초기화 */
    stlUInt64    mSegmentHandle;   /**< volatile 정보. segment Handle 포인터를 저장 */
} smeRelationHeader;


#define SME_GET_RELATION_TYPE(relHandle)  (((smeRelationHeader*)relHandle)->mRelationType)
#define SME_GET_RELATION_STATE(relHandle)  (((smeRelationHeader*)relHandle)->mRelState)
#define SME_GET_OBJECT_SCN(relHandle)     (((smeRelationHeader*)relHandle)->mObjectScn)
#define SME_GET_SEGMENT_HANDLE(relHandle) ((void*)(((smeRelationHeader*)relHandle)->mSegmentHandle))

#define SME_SET_OBJECT_SCN(relHandle,objScn)              \
{                                                         \
    ((smeRelationHeader*)relHandle)->mObjectScn = objScn; \
}
#define SME_SET_RELATION_STATE(relHandle,relState)          \
{                                                           \
    ((smeRelationHeader*)relHandle)->mRelState = relState;  \
}
#define SME_SET_SEGMENT_HANDLE(relHandle,segHandle)                         \
{                                                                           \
    ((smeRelationHeader*)relHandle)->mSegmentHandle = (stlUInt64)segHandle; \
}

typedef struct smeScanFuncVector
{
    smiFetchNextFunc     mFetchNext;
    smiFetchPrevFunc     mFetchPrev;
    smiFetchAggrFunc     mFetchAggr;
    smiFetchSamplingFunc mFetchSampling;
} smeScanFuncVector;


/** TODO : insertOrUpdate interface */

typedef struct smeIteratorModule
{
    stlUInt32           mIteratorSize;

    /* Iterator Functions */
    smiInitializeFunc        mInitIterator;
    smiFinalizeFunc          mFiniIterator;
    smiResetFunc             mResetIterator;
    smiMoveToRowIdFunc       mMoveToRowId;
    smiMoveToPosNumFunc      mMoveToPosNum;
    smeScanFuncVector        mScanFunc;
} smeIteratorModule;

typedef struct smeGroupIteratorModule
{
    /* smeIteratorModule를 상속받는다. 즉 smeIteratorModule *로 캐스팅해서 사용할 수 있어야 한다. */
    stlUInt32           mIteratorSize;

    /* Iterator Functions */
    smiInitializeFunc        mInitIterator;
    smiFinalizeFunc          mFiniIterator;
    smiResetFunc             mResetIterator; 
    smiMoveToRowIdFunc       mMoveToRowId;
    smiMoveToPosNumFunc      mMoveToPosNum;
    smeScanFuncVector        mScanFunc;

    /* 추가되는 맴버들은 뒤에 두어야 한다. */
    smiMoveToNextKeyFunc     mMoveToNextKey;
    smiResetGroupFunc        mResetGroup;
} smeGroupIteratorModule;

/**
 * @brief 최대 RELATION 힌트 사이즈
 */
#define SME_MAX_SEGMENT_HINT 8

/**
 * @brief HINT 정보 ( Relation당 한 개 )
 */
typedef struct smeHint
{
    void   * mRelHandle;
    smlScn   mValidScn;
    smlPid   mHintPid;
} smeHint;

/**
 * @brief 최대 RELATION 검색 캐시 사이즈
 */
typedef struct smeHintPool
{
    stlInt32 mSequence;
    smeHint  mRelHint[SME_MAX_SEGMENT_HINT];
} smeHintPool;


/** @} */

/** @} */

#endif /* _SMDEF_H_ */

