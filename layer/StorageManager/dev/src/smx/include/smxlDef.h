/*******************************************************************************
 * smxlDef.h
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


#ifndef _SMXLDEF_H_
#define _SMXLDEF_H_ 1

/**
 * @file smxlDef.h
 * @brief Storage Manager Layer Local Transaction Component Internal Definition
 */

/**
 * @defgroup smxlInternal Component Internal Routines
 * @ingroup smxl
 * @{
 */

/**
 * @addtogroup smrUndoLogType
 * @{
 */

#define SMXL_UNDO_LOG_MEMORY_CIRCULAR_CREATE     0  /**< memory circular creation undo */
#define SMXL_UNDO_LOG_LOCK_FOR_PREPARE           1  /**< lock record for XA prepare */

/** @} */

#define SMXL_UNDO_LOG_MAX_COUNT                2

#define SMXL_PARENT_SHMMEM_CHUNK_SIZE ( 2 * 1024 * 100 )
#define SMXL_SHMMEM_CHUNK_SIZE        ( 2 * 1024 )

#define SMXL_INIT_TRANS( aTrans, aTransId, aIsShared, aEnv )                            \
    {                                                                                   \
        (aTrans)->mState = SMXL_STATE_IDLE;                                             \
        (aTrans)->mUndoRelEntry = NULL;                                                 \
        (aTrans)->mUndoSegRid = SML_INVALID_RID;                                        \
        (aTrans)->mCommitScn = SML_INFINITE_SCN;                                        \
        (aTrans)->mCommitSbsn = SMR_INVALID_SBSN;                                       \
        (aTrans)->mCommitLsn = SMR_INVALID_LSN;                                         \
        (aTrans)->mTransId = aTransId;                                                  \
        (aTrans)->mTryLogCount = 0;                                                     \
        (aTrans)->mTransSeq = 0;                                                        \
        (aTrans)->mTransViewScn = SML_INFINITE_SCN;                                     \
        (aTrans)->mWrittenTransRecord = STL_FALSE;                                      \
        (aTrans)->mAttr = SMXL_ATTR_NONE;                                               \
        (aTrans)->mTcn = 1;                                                             \
        (aTrans)->mPropagateLog = STL_TRUE;                                             \
        (aTrans)->mRepreparable = STL_TRUE;                                             \
        (aTrans)->mIsGlobal = STL_FALSE;                                                \
        (aTrans)->mTimestamp = 0;                                                       \
        (aTrans)->mBeginTime = 0;                                                       \
        (aTrans)->mBeginLsn = SMR_INVALID_LSN;                                          \
        (aTrans)->mUsedUndoPageCount = 0;                                               \
        STL_TRY( knlInitLatch( &((aTrans)->mLatch),                                     \
                               (aIsShared),                                             \
                               (knlEnv*)aEnv ) == STL_SUCCESS );                        \
        if( (aIsShared) == STL_TRUE )                                                   \
        {                                                                               \
            STL_TRY( knlCreateRegionMem( &(aTrans)->mShmMem,                            \
                                         KNL_ALLOCATOR_STORAGE_MANAGER_TRANSACTION,     \
                                         &gSmxlWarmupEntry->mParentMem,                 \
                                         KNL_MEM_STORAGE_TYPE_SHM,                      \
                                         SMXL_SHMMEM_CHUNK_SIZE,                        \
                                         SMXL_SHMMEM_CHUNK_SIZE,                        \
                                         (knlEnv*)aEnv ) == STL_SUCCESS );              \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            KNL_INIT_REGION_MEMORY( &(aTrans)->mShmMem );                               \
        }                                                                               \
        STL_RING_INIT_HEADLINK( &(aTrans)->mSavepointList,                              \
                                STL_OFFSETOF( smxlSavepoint,                            \
                                              mSavepointLink ) );                       \
    }
    
/*
 * Transaction Id의 invalid 설정은 하위 하위 자료의 유효성을
 * 상실시키는 효과를 갖는다.
 * 설정 순서를 보장하기 위해서 barrier를 사용한다.
 */
#define SMXL_RESET_TRANS( aTrans, aEnv, aCleanup )                              \
    {                                                                           \
        (aTrans)->mTransId = SML_INVALID_TRANSID;                               \
        stlMemBarrier();                                                        \
        (aTrans)->mState = SMXL_STATE_IDLE;                                     \
        (aTrans)->mUndoRelEntry = NULL;                                         \
        (aTrans)->mUndoSegRid = SML_INVALID_RID;                                \
        (aTrans)->mTryLogCount = 0;                                             \
        (aTrans)->mTransSeq += 1;                                               \
        (aTrans)->mCommitScn = SML_INFINITE_SCN;                                \
        (aTrans)->mCommitSbsn = SMR_INVALID_SBSN;                               \
        (aTrans)->mCommitLsn = SMR_INVALID_LSN;                                 \
        (aTrans)->mTransViewScn = SML_INFINITE_SCN;                             \
        (aTrans)->mWrittenTransRecord = STL_FALSE;                              \
        (aTrans)->mAttr = SMXL_ATTR_NONE;                                       \
        (aTrans)->mTcn = 1;                                                     \
        (aTrans)->mPropagateLog = STL_TRUE;                                     \
        (aTrans)->mRepreparable = STL_TRUE;                                     \
        (aTrans)->mIsGlobal = STL_FALSE;                                        \
        (aTrans)->mTimestamp = 0;                                               \
        (aTrans)->mBeginTime = 0;                                               \
        (aTrans)->mBeginLsn = SMR_INVALID_LSN;                                  \
        (aTrans)->mUsedUndoPageCount = 0;                                       \
        if( KNL_REGION_MEMORY_IS_CREATED( &(aTrans)->mShmMem ) == STL_TRUE )    \
        {                                                                       \
            STL_TRY( knlClearRegionMem( &(aTrans)->mShmMem,                     \
                                        (knlEnv*)aEnv )                         \
                     == STL_SUCCESS );                                          \
        }                                                                       \
        STL_RING_INIT_HEADLINK( &(aTrans)->mSavepointList,                      \
                                STL_OFFSETOF( smxlSavepoint,                    \
                                              mSavepointLink ) );               \
    }

#define SMXL_TRANS_SLOT( aTransId ) ( &SMXL_TRANS_TABLE(SMXL_TRANS_SLOT_ID(aTransId)) )

/**
 * restore session을 위한 transaction table
 */
#define SMXL_IS_MEDIA_RECOVERY_TRANS( aTransId ) \
    (((aTransId) & SMXL_MEDIA_RECOVERY_TRANS_MASK) == SMXL_MEDIA_RECOVERY_TRANS_MASK)

#define SMXL_MEDIA_RECOVERY_TRANS_SLOT( aEnv, aTransId )   \
     (&(SMXL_GET_MEDIA_RECOVERY_TRANS_TABLE(aEnv)[SMXL_TRANS_SLOT_ID(aTransId)]))

/**
 * System Transaction의 Redo Log는 CDC 대상이 아님
 */
#define SMXL_TRANS_PROPAGATE_LOG( aTransId ) \
    ( SMXL_IS_SYSTEM_TRANS( aTransId )? STL_FALSE : SMXL_TRANS_SLOT( aTransId )->mPropagateLog )

#define SMXL_MAX_UNDO_RELATION_COUNT 8

typedef struct smxlUndoRelEntry
{
    knlLatch   mLatch;
    stlBool    mIsFree;
    stlChar    mPadding1;
    stlInt16   mRelId;
    stlChar    mPadding2[4];
    smlRid     mSegRid;
    void     * mRelHandle;
    stlChar    mPadding3[40];   /**< padding for cache line */
} smxlUndoRelEntry;

typedef struct smxlUndoRelCache
{
    void        * mSegHandle;
    smlRid        mTransLstUndoRid;
    smlRid        mUndoNextRid;
    smxlTransId   mTransId;
    stlInt32      mRelSeqId;
} smxlUndoRelCache;

/**
 * Dummy Relation Header
 */
typedef struct smxlRelHdr
{
    stlInt32   mRelSeqId;
    stlChar    mAlign[4];
} smxlRelHdr;

/**
 * @brief Process Warmup을 위한 Entry 구조체
 */
typedef struct smxlWarmupEntry
{
    smlScn              mSystemScn;
    smlScn              mStableScn;
    smlScn              mMinTransViewScn;
    smlScn              mOldMinViewScn;
    smlScn              mOldSystemScn;
    smlScn              mOldIdleSystemScn;
    stlInt64            mTryStealUndoCount;
    stlChar             mPadding1[8];       /**< for align cache line */
    knlDynamicMem       mParentMem;
    smlScn              mAgableScn;
    smlScn              mAgableStmtScn;     /**< statement 단위의 Agable SCN */
    smlScn              mAgableViewScn;
    smxlTrans         * mTransTable;        /**< Warmup시 트랜잭션 테이블 구성을 위한 Entry Point */
    stlUInt32           mTransTableSize;    /**< Warmup시 트랜잭션 테이블 사이즈 구성을 위한 Entry Point */
    stlInt32            mUndoRelCount;
    smxlUndoRelEntry  * mUndoRelEntry;
    stlInt64            mDataStoreMode;
    stlInt16            mAgerState;
    stlChar             mPadding2[6];       /**< for align cache line */
    knlLatch            mLatch;             /**< for building agable scn */
} smxlWarmupEntry;

extern stlInt32            gSmxlTransTableSize;
extern stlInt32            gSmxlUndoRelCount;
extern smxlTrans         * gSmxlTransTable;
extern smxlUndoRelEntry  * gSmxlUndoRelEntry;

#define SMXL_TRANS_TABLE_SIZE           ( gSmxlTransTableSize )
#define SMXL_UNDO_REL_ENTRY( aSlotId )  ( gSmxlUndoRelEntry[(aSlotId)] )
#define SMXL_UNDO_REL_COUNT             ( gSmxlUndoRelCount )

#define SMXL_TRANSACTION_RECORD     0x01
#define SMXL_UNDO_RECORD            0x02
#define SMXL_UNDO_RECORD_DELETED    0x04

typedef struct smxlTransRecord
{
    smxlTransId mTransId;
    smlScn      mCommitScn;
    smlScn      mTransViewScn;
    stlTime     mCommitTime;
    stlInt16    mCommentSize;       /**< offset이 바뀌면 smlLogDef.h.in도 수정되어야 한다 */
    stlBool     mGlobalTrans;
    stlChar     mPadding[5];
} smxlTransRecord;

#define SMXL_READ_UNDO_REC_TYPE( aDest, aSrc )                                                  \
    {                                                                                           \
        STL_WRITE_INT16( aDest, ((stlChar*)(aSrc) + STL_OFFSETOF( smxlUndoRecHdr, mType )) );   \
    }
#define SMXL_READ_UNDO_REC_CLASS( aDest, aSrc )                                                     \
    {                                                                                               \
        STL_WRITE_INT8( aDest, ((stlChar*)(aSrc) + STL_OFFSETOF( smxlUndoRecHdr, mComponentClass )) ); \
    }
#define SMXL_READ_UNDO_REC_SIZE( aDest, aSrc )                                                  \
    {                                                                                           \
        STL_WRITE_INT16( aDest, ((stlChar*)(aSrc) + STL_OFFSETOF( smxlUndoRecHdr, mSize )) );   \
    }

#define SMXL_WRITE_UNDO_REC_HDR( aDest, aSrc )                      \
    {                                                               \
        stlMemcpy( (aDest), (aSrc), STL_SIZEOF( smxlUndoRecHdr ) ); \
    }

#define SMXL_WRITE_UNDO_REC_DELETE_FLAG( aDest, aSrc )                                              \
    {                                                                                               \
        STL_WRITE_INT8( ((stlChar*)(aDest) + STL_OFFSETOF( smxlUndoRecHdr, mFlag )), &((aSrc)->mFlag) ); \
    }

#define SMXL_WRITE_MOVE_UNDO_REC_HDR( aDest, aSrc, aOffset )                    \
    {                                                                           \
        stlMemcpy( (aDest) + (aOffset), (aSrc), STL_SIZEOF( smxlUndoRecHdr ) ); \
        (aOffset) += STL_SIZEOF( smxlUndoRecHdr );                              \
    }

/**
 * @brief KNL_ASSERT에 사용될 Physical Header Dump format
 */
#define SMXL_FORMAT_TRANS_SLOT( aTrans )                                            \
    ( "STATE(%d), TRANS_ID(%lx), ISOLATION_LEVEL(%d), TRANS_VIEW_SCN(%ld) \n"       \
      "COMMIT_SCN(%ld), COMMIT_LSN(%ld), UNDO_SEG_RID(%d,%d,%d)",                   \
      (aTrans)->mState,                                                             \
      (aTrans)->mTransId,                                                           \
      (aTrans)->mIsolationLevel,                                                    \
      (aTrans)->mTransViewScn,                                                      \
      (aTrans)->mCommitScn,                                                         \
      (aTrans)->mCommitLsn,                                                         \
      (aTrans)->mUndoSegRid.mTbsId,                                                 \
      (aTrans)->mUndoSegRid.mPageId,                                                \
      (aTrans)->mUndoSegRid.mOffset )

typedef struct smxlFxTrans
{
    smxlTransId         mSmxlTransId;
    smxlTransId         mSmlTransId;
    stlInt16            mSlotId;
    stlChar             mState[11];
    stlChar             mAttr[51];
    stlChar             mIsolationLevel[21];
    smlScn              mTransViewScn;
    smlScn              mCommitScn;
    smlTcn              mTcn;
    smrLsn              mBeginLsn;
    stlUInt64           mTransSeq;
    stlTime             mBeginTime;
    stlBool             mPropagateLog;
    stlBool             mRepreparable;
    stlBool             mGlobal;
    stlInt64            mUsedUndoPageCount;
    stlInt64            mUndoSegmentId;
} smxlFxTrans;

typedef struct smxlFxUndoSeg
{
    stlInt64    mSegmentId;
    stlInt64    mPhysicalId;
    stlInt64    mAllocPageCount;
} smxlFxUndoSeg;

/** @} */
    
#endif /* _SMXLDEF_H_ */
