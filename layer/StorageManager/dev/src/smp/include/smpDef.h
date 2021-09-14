/*******************************************************************************
 * smpDef.h
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


#ifndef _SMPDEF_H_
#define _SMPDEF_H_ 1

#include <smDef.h>

/**
 * @file smpDef.h
 * @brief Storage Manager Layer Page Access Internal Definition
 */

/**
 * @defgroup smpInternal Component Internal Routines
 * @ingroup smp
 * @{
 */

typedef struct smpPchArrayList smpPchArrayList;
struct smpPchArrayList
{
    smpPchArrayList * mNext;
    smlTbsId          mTbsId;
    smlDatafileId     mDatafileId;
    stlUInt32         mPchCount;
    knlLogicalAddr    mPchArray;
    stlInt16          mState;
    stlChar           mPadding[6];
};

typedef struct smpWarmupEntry
{
    knlLatch          mLatch;
    smpPchArrayList * mPchArrayList;
    smrLsn            mMinFlushedLsn;
    stlTime           mPageFlushingTime;
    stlInt64          mFlushedPageCount;
} smpWarmupEntry;

typedef struct smpAccessPendArgs
{
    smlTbsId      mTbsId;
    smlDatafileId mDatafileId;
    stlInt16      mPchArrayState;
    stlChar       mAlign[2];
} smpAccessPendArgs;

typedef stlStatus (*smpInitFunc) ( smlTbsId        aTbsId,
                                   smlDatafileId   aDatafileId,
                                   smlEnv        * aEnv );
typedef stlStatus (*smpExtendFunc) ( smlStatement  * aStatement,
                                     smlTbsId        aTbsId,
                                     smlDatafileId   aDatafileId,
                                     stlBool         aNeedPending,
                                     smlEnv        * aEnv );
typedef stlStatus (*smpShrinkFunc) ( smlStatement  * aStatement,
                                     smlTbsId        aTbsId,
                                     smlDatafileId   aDatafileId,
                                     stlInt16        aPchArrayState,
                                     smlEnv        * aEnv );
typedef stlStatus (*smpCreateFunc) ( smxmTrans  * aMiniTrans,
                                     smlTbsId     aTbsId,
                                     smlPid       aPageId,
                                     smpPageType  aPageType,
                                     smpHandle  * aPageHandle,
                                     smlEnv     * aEnv );
typedef stlStatus (*smpFixFunc) ( smlTbsId     aTbsId,
                                  smlPid       aPageId,
                                  smpHandle  * aPageHandle,
                                  smlEnv     * aEnv );

typedef struct smpAccessFuncs
{
    smpInitFunc   mInit;
    smpExtendFunc mExtend;
    smpShrinkFunc mShrink;
    smpCreateFunc mCreate;
    smpFixFunc    mFix;
} smpAccessFuncs;

/**
 * @addtogroup smrUndoLogType
 * @{
 */
#define SMP_UNDO_LOG_RTS_VERSIONING   0  /**< RTS VERSIONING */
/** @} */

#define SMP_UNDO_LOG_MAX_COUNT        1



/**
 * @brief Page의 RowItem(or Offset Slot)의 개수를 반환한다
 */
#define SMP_PAGE_GET_ROWITEM_COUNT(page)  ((SMP_GET_OFFSET_TABLE_HEADER(page))->mCurrentSlotCount)

#define SMP_PAGE_GET_FREE_SPACE(page)                                                     \
    (((smpOrderedSlotSignpost*)SMP_GET_SIGNPOST(page))->mLowWaterMark                     \
     - ((smpOrderedSlotSignpost*)SMP_GET_SIGNPOST(page))->mCommon.mHighWaterMark          \
     + ((smpOrderedSlotSignpost*)SMP_GET_SIGNPOST(page))->mReclaimedSlotSpace)

/**
 * @brief Page의 빈 공간이 PCTFREE를 이하로 떨어져 Update only로 변경해야하는지 체크한다
 */
#define SMP_PAGE_IS_OVER_INSERT_LIMIT(page,pctFree)                                          \
    (SMP_PAGE_GET_FREE_SPACE(page) <= (((stlUInt64)(pctFree) * SMP_PAGE_SIZE) / 100) ?       \
        STL_TRUE : STL_FALSE)

/**
 * @brief Page가 PCTUSED에 못미쳐서 Insertable로 변경해야하는지 체크한다
 */
#define SMP_PAGE_IS_OVER_FREE_LIMIT(page,pctUsed)                                            \
    (SMP_PAGE_GET_FREE_SPACE(page) >= (((stlUInt64)(100 - pctUsed) * SMP_PAGE_SIZE) / 100) ? \
        STL_TRUE : STL_FALSE)

typedef struct smpParallelLoadArgs
{
    stlRingHead   mJobs;
    stlErrorStack mErrorStack;
} smpParallelLoadArgs;

typedef struct smpParallelLoadJob
{
    stlRingEntry  mJobLink;
    smlTbsId      mTbsId;
    smlDatafileId mDatafileId;
} smpParallelLoadJob;

typedef struct smpParallelWriteArgs
{
    stlRingHead      mJobs;
    smpFlushBehavior mBehavior;
    stlBool          mLogging;
    stlBool          mForCheckpoint;
    stlErrorStack    mErrorStack;
    smpSystemInfo    mSystemInfo;
} smpParallelWriteArgs;

typedef struct smpParallelWriteJob
{
    stlRingEntry      mJobLink;
    smpPchArrayList * mPchArrayList;
} smpParallelWriteJob;

typedef struct smpPchLatchPathCtrl
{
    smpPchArrayList * mPchArrayList;
    stlInt32          mPchPosition;
} smpPchLatchPathCtrl;


typedef struct smpFxSignpost
{
    smlPid       mPageId;             /**< Page ID */
    stlUInt16    mPageMgmtType;       /**< Page Management Type */
    stlUInt16    mHighWaterMark;      /**< Page Usage High Water Mark */
    stlUInt16    mLogicalHeaderSize;  /**< page logical header size. It SHOULD be 8 byte aligned */
    stlUInt16    mMinRtsCount;        /**< Minimum Maximum RTS Count */
    stlUInt16    mMaxRtsCount;        /**< Maximum Maximum RTS Count */
    stlUInt16    mCurRtsCount;        /**< Current Maximum RTS Count */
    stlUInt16    mHintRts;            /**< Next Hint RTS Sequence */
    stlUInt16    mLowWaterMark;       /**< lower bound of Row(key) data slots */
    stlUInt16    mReclaimedSlotSpace; /**< total space size of reclaimed data slots */
    stlUInt16    mActiveSlotCount;    /**< total count of active slots */
} smpFxSignpost;

typedef struct smpFxSignpostIterator
{
    smlTbsId        mTbsId;
    void          * mSegIterator;
    smlPid          mCurPid;
    smpFxSignpost   mSignpostRecord;
} smpFxSignpostIterator;

typedef struct smpFxRts
{
    smlPid       mCurPid;
    stlUInt16    mSequence;
    stlChar      mStatus[16];         /**< Rts Status */
    stlUInt16    mRefCount;           /**< Reference Count */
    stlUInt16    mOffsetCache0;       /**< Offset cache[0] */
    stlUInt16    mOffsetCache1;       /**< Offset cache[0] */
    stlUInt16    mOffsetCache2;       /**< Offset cache[0] */
    stlUInt16    mPrevVerCnt;         /**< previous version count */
    stlUInt16    mPrevOffset;         /**< offset of position where previous version locate */
    stlUInt32    mPrevPid;            /**< pid of position where previous version locate */
    stlUInt64    mTransId;            /**< Current Transaction ID */
    stlUInt64    mScn;                /**< Current Scn */
    stlUInt64    mPrevScn;            /**< commit scn of first previous version */
} smpFxRts;

typedef struct smpFxRtsIterator
{
    smlTbsId        mTbsId;
    void          * mSegIterator;
    smlPid          mCurPid;
    stlInt64        mPageBuf[SMP_PAGE_SIZE / STL_SIZEOF(stlInt64)];
    stlUInt8        mRtsCount;
    smpRts        * mCurRts;
    stlUInt16       mCurRtsSeq;
    smpFxRts        mRtsRecord;
} smpFxRtsIterator;

typedef struct smpFxSegHdr
{
    smlPid     mCurPid;
    stlInt64   mPhysicalId;
    stlChar    mFreeness[3];
    smrLsn     mLsn;
    stlInt64   mParentId;
    smlScn     mAgableScn;
    stlChar    mVolatileArea[SMP_VOLATILE_AREA_SIZE];
} smpFxSegHdr;

typedef struct smpFxSegHdrIterator
{
    smlTbsId        mTbsId;
    void          * mSegIterator;
    smlPid          mCurPid;
} smpFxSegHdrIterator;

#define SMP_PAGE_DUMP_DATA_LENGTH        (32)
#define SMP_PAGE_DUMP_DATA_OFFSET_LENGTH (5)

typedef struct smlRid smpFxPageDumpHandle;

typedef struct smpFxPageDump
{
    stlChar   mOffSetStr[SMP_PAGE_DUMP_DATA_OFFSET_LENGTH + 1];
    stlChar   mData[(SMP_PAGE_DUMP_DATA_LENGTH * 2) + 1];
} smpFxPageDump;

typedef struct smpFxPageIterator
{
    smlTbsId    mTbsId;
    smlPid      mPageId;
    smpHandle   mPageHandle;
    void      * mStartAddr;
    stlChar     mPageSnapshot[SMP_PAGE_SIZE];
    stlChar   * mCurAddr;
    stlUInt32   mSize;
    stlUInt32   mRemainSize;
} smpFxPageIterator;

#define SMP_PCH_ADDR_LENGTH              (18)
#define SMP_PCH_BOOLEAN_LENGTH           (1)
#define SMP_PCH_PAGE_TYPE_LENGTH         (20)
#define SMP_PCH_DEVICE_TYPE_LENGTH       (8)

typedef struct smlRid smpFxPchDumpHandle;

typedef struct smpFxPchDump
{
    stlUInt16  mTbsId;
    stlUInt32  mPageId;
    stlChar    mFrame[SMP_PCH_ADDR_LENGTH + 1];
    stlInt32   mFixedCount;
    stlBool    mDirty;
    stlChar    mPageType[SMP_PCH_PAGE_TYPE_LENGTH + 1];
    stlChar    mDeviceType[SMP_PCH_DEVICE_TYPE_LENGTH + 1];
    stlInt64   mFirstDirtyLsn;
    stlInt64   mRecoveryLsn;
    stlInt64   mRecoverySbsn;
    stlInt16   mRecoveryLpsn;
} smpFxPchDump;

typedef struct smpFxPchIterator
{
    smlTbsId      mTbsId;
    smlDatafileId mDatafileId;
    stlInt64      mPageSeqId;
} smpFxPchIterator;

/** @} */
    
#endif /* _SMPDEF_H_ */
