/*******************************************************************************
 * smdmpfDef.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smdmpfDef.h 13496 2014-08-29 05:38:43Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _SMDMPFDEF_H_
#define _SMDMPFDEF_H_ 1


/**
 * @file smdmpfDef.h
 * @brief Storage Manager Layer Memory Heap Table Internal Definition
 */

/**
 * @defgroup smdmpfInternal Component Internal Routines
 * @ingroup smd
 * @{
 */

#define SMDMPF_ROW_HDR_LEN       SMDMPF_MIN_ROW_HDR_LEN

/**********************************/
/* MEMORY HEAP FIELD DEFINITION   */
/**********************************/

#define  SMDMPF_SET_DELETED( aRow )                       \
      {                                                   \
          *(stlChar*)(aRow) |= SMDMPF_FLAG_DELETED;       \
      }
#define  SMDMPF_SET_MASTER_ROW(aRow)                      \
      {                                                   \
          *(stlChar*)(aRow) |= SMDMPF_FLAG_IS_MASTER;     \
      }
#define  SMDMPF_SET_CONT_COL(aRow)                        \
      {                                                   \
          *(stlChar*)(aRow) |= SMDMPF_FLAG_IS_CONT_COL;   \
      }
#define  SMDMPF_SET_CONT_COL_LAST(aRow)                     \
    {                                                       \
        *(stlChar*)(aRow) |= SMDMPF_FLAG_IS_CONT_COL_LAST;  \
    }
#define  SMDMPF_SET_LOCKED(aRow)                          \
      {                                                   \
          *(stlChar*)(aRow) |= SMDMPF_FLAG_IS_LOCKED;     \
      }
#define  SMDMPF_SET_RTS_BOUND(aRow)                       \
      {                                                   \
          *(stlChar*)(aRow) |= SMDMPF_FLAG_IS_RTS_BOUND;  \
      }
#define  SMDMPF_SET_HAS_NEXT_LINK(aRow)                   \
      {                                                   \
          *(stlChar*)(aRow) |= SMDMPF_FLAG_HAS_NEXT_LINK; \
      }

#define  SMDMPF_UNSET_DELETED(aRow)                          \
      {                                                      \
          *(stlChar*)(aRow) &= ~(SMDMPF_FLAG_DELETED);       \
      }
#define  SMDMPF_UNSET_MASTER_ROW(aRow)                       \
      {                                                      \
          *(stlChar*)(aRow) &= ~(SMDMPF_FLAG_IS_MASTER);     \
      }
#define  SMDMPF_UNSET_CONT_COL(aRow)                         \
      {                                                      \
          *(stlChar*)(aRow) &= ~(SMDMPF_FLAG_IS_CONT_COL);   \
      }
#define  SMDMPF_UNSET_LOCKED(aRow)                           \
      {                                                      \
          *(stlChar*)(aRow) &= ~(SMDMPF_FLAG_IS_LOCKED);     \
      }
#define  SMDMPF_UNSET_RTS_BOUND(aRow)                        \
      {                                                      \
          *(stlChar*)(aRow) &= ~(SMDMPF_FLAG_IS_RTS_BOUND);  \
      }
#define  SMDMPF_UNSET_HAS_NEXT_LINK(aRow)                    \
      {                                                      \
          *(stlChar*)(aRow) &= ~(SMDMPF_FLAG_HAS_NEXT_LINK); \
      }
#define  SMDMPF_SET_IN_USE(aRow)                        \
    {                                                   \
        *(stlChar*)(aRow) |= SMDMPF_FLAG_IN_USE;        \
    }
#define  SMDMPF_UNSET_IN_USE(aRow)                      \
    {                                                   \
        *(stlChar*)(aRow) &= ~(SMDMPF_FLAG_IN_USE);     \
    }

#define  SMDMPF_SET_TCN(aRow, aTcn)                                                     \
      ( STL_WRITE_INT32((stlChar*)(aRow) + SMDMPF_OFFSET_TCN, (aTcn)) )
#define  SMDMPF_SET_SCN(aRow, aScn)                                                     \
      ( STL_WRITE_INT64((stlChar*)(aRow) + SMDMPF_OFFSET_SCN_RTSSEQ, (aScn)) )
#define  SMDMPF_SET_RTSSEQ(aRow, aRtsSeq)                                               \
      ( STL_WRITE_INT8((stlChar*)(aRow) + SMDMPF_OFFSET_SCN_RTSSEQ, (aRtsSeq)) )
#define  SMDMPF_SET_TRANS_ID(aRow, aTransId)                                            \
      ( STL_WRITE_INT64((stlChar*)(aRow) + SMDMPF_OFFSET_TRANS_ID, (aTransId)) )
#define  SMDMPF_SET_ROLLBACK_PID(aRow, aPid)                                            \
      ( STL_WRITE_INT32((stlChar*)(aRow) + SMDMPF_OFFSET_ROLLBACK_PID, (aPid)) )
#define  SMDMPF_SET_ROLLBACK_OFFSET(aRow, aOffset)                                      \
      ( STL_WRITE_INT16((stlChar*)(aRow) + SMDMPF_OFFSET_ROLLBACK_OFFSET, (aOffset)) )
#define  SMDMPF_SET_COLUMN_CNT(aRow, aColCnt)                                           \
      ( STL_WRITE_INT16((stlChar*)(aRow) + SMDMPF_OFFSET_COLUMN_CNT, (aColCnt)) )
#define  SMDMPF_SET_PAD_SPACE(aRow, aPadSpace)                                          \
      ( STL_WRITE_INT16((stlChar*)(aRow) + SMDMPF_OFFSET_PAD_SPACE, (aPadSpace)) )
#define  SMDMPF_SET_LINK_PID(aRow, aPid)                                                \
      ( STL_WRITE_INT32((stlChar*)(aRow) + SMDMPF_OFFSET_LINK_PID, (aPid)) )
#define  SMDMPF_SET_LINK_OFFSET(aRow, aOffset)                                          \
      ( STL_WRITE_INT16((stlChar*)(aRow) + SMDMPF_OFFSET_LINK_OFFSET, (aOffset)) )


#define  SMDMPF_MAXIMUM_INSERTABLE_PAGE_SEARCH_COUNT (SMS_MAX_PAGE_HINT_COUNT*2)

#define  SMDMPF_COPY_ROW_HDR( aDst, aSrc )            \
{                                                     \
    stlMemcpy( aDst, aSrc, 31 );                      \
}


#define  SMDMPF_IS_SAME_TRANS( aTransIdA, aCommitScnA, aTransIdB, aCommitScnB )    \
    ( ((aCommitScnA  == aCommitScnB) &&                                            \
       ((aCommitScnA  != SML_INFINITE_SCN) ||                                      \
        ((aCommitScnA  == SML_INFINITE_SCN) && (aTransIdA == aTransIdB) ))) ?      \
      STL_TRUE :  STL_FALSE )

#define SMDMPF_CONT_SIZE( aColLen, aColLenSize )  \
    ( SMDMPF_ROW_HDR_LEN + aColLen + aColLenSize )

/**
 * @brief 주어진 위치에 주어진 컬럼 data를 저장하다.
 */
#define SMDMPF_WRITE_COLUMN( aColPtr, aValue, aValueLen, aIsZero, aColLenSize )                 \
    {                                                                                           \
        if( STL_EXPECT_TRUE( (aValueLen) <= SMP_COLUMN_LEN_1BYTE_MAX ) )                        \
        {                                                                                       \
            if( aIsZero == STL_TRUE )                                                           \
            {                                                                                   \
                *(stlUInt8*)(aColPtr) = SMP_COLUMN_LEN_ZERO;                                    \
            }                                                                                   \
            else                                                                                \
            {                                                                                   \
                *aColLenSize = 1;                                                               \
                *(stlUInt8*)(aColPtr) = (stlUInt8)(aValueLen);                                  \
                stlMemcpy( ((stlUInt8*)(aColPtr) + *aColLenSize), (aValue), (aValueLen) );      \
            }                                                                                   \
        }                                                                                       \
        else                                                                                    \
        {                                                                                       \
            stlUInt16 tmpInt16 = (aValueLen);                                                   \
            *aColLenSize = 3;                                                                   \
            *(stlUInt8*)(aColPtr) = SMP_COLUMN_LEN_2BYTE_FLAG;                                  \
            STL_WRITE_INT16( (stlUInt8*)(aColPtr) + 1, &tmpInt16 );                             \
            stlMemcpy( ((stlUInt8*)(aColPtr) + *aColLenSize), (aValue), (aValueLen) );          \
        }                                                                                       \
    }


/**
 * @brief Continuous column인지 여부를 반환한다.
 */
#define SMP_IS_CONT_COLUMN( aColPtr )                   \
    (*(stlUInt8*)(aColPtr) == SMP_COLUMN_CONT_FLAG)

/**
 * @brief Continuous column의 column length를 반환한다.
 */
#define SMP_GET_CONT_COLUMN_LEN( aColPtr, aColLen )                                     \
    {                                                                                   \
        stlUInt16 tmpInt16;                                                             \
                                                                                        \
        STL_WRITE_INT16( &tmpInt16, (stlChar *)(aColPtr) + 1 + STL_SIZEOF(smlPid) );    \
                                                                                        \
        *(aColLen) = (stlInt64)tmpInt16;                                                \
    }

#define SMP_GET_FIXED_COLUMN_LEN( aColPtr, aColLenSize, aColLen, aIsContCol, aIsZero )          \
    {                                                                                           \
        aIsContCol = (*(stlUInt8*)(aColPtr) == SMP_COLUMN_CONT_FLAG);                           \
        if( STL_EXPECT_TRUE( aIsContCol == STL_FALSE ) )                                        \
        {                                                                                       \
            SMP_GET_COLUMN_LEN( aColPtr, aColLenSize, aColLen, aIsZero );                       \
        }                                                                                       \
        else                                                                                    \
        {                                                                                       \
            SMP_GET_CONT_COLUMN_LEN( aColPtr, aColLen );                                        \
            *(aColLenSize) = STL_SIZEOF(stlUInt8) + STL_SIZEOF(stlUInt16) + STL_SIZEOF(smlPid); \
            aIsZero = STL_FALSE;                                                                \
        }                                                                                       \
    }

#define SMP_WRITE_CONT_COLUMN( aColPtr, aRowPtr, aNextPid, aPieceLen )          \
    {                                                                           \
        stlUInt16   tmpOffset;                                                  \
        stlUInt16   tmpInt16 = (aPieceLen);                                     \
        *(stlUInt8*)(aColPtr) = SMP_COLUMN_CONT_FLAG;                           \
        tmpOffset = STL_SIZEOF( stlUInt8 );                                     \
        stlMemcpy( (aColPtr) + tmpOffset, &(aNextPid), STL_SIZEOF(smlPid) );    \
        tmpOffset += STL_SIZEOF( smlPid );                                      \
        stlMemcpy( (aColPtr) + tmpOffset, &(tmpInt16), STL_SIZEOF(stlInt16) );  \
        tmpOffset += STL_SIZEOF( stlInt16 );                                    \
        stlMemcpy( (aColPtr) + tmpOffset, (aRowPtr), (aPieceLen) );             \
    }

#define SMP_READ_CONT_COLUMN_PID( aColPtr, aPid )               \
    {                                                           \
        STL_WRITE_INT32( (aPid), ((stlChar *)(aColPtr) + 1) );  \
    }

#define SMP_GET_CONT_COLUMN_VALUE_PTR( aColPtr, aPtr )                                  \
    {                                                                                   \
        (aPtr) = (stlChar *)(aColPtr) + 1 + STL_SIZEOF(smlPid) + STL_SIZEOF(stlInt16);  \
    }

#define SMDMPF_MEMBER_PAGE_TYPE_MASK     (0x0F)
#define SMDMPF_MEMBER_PAGE_TYPE_NONE     (0x00)
#define SMDMPF_MEMBER_PAGE_TYPE_MASTER   (0x01)
#define SMDMPF_MEMBER_PAGE_TYPE_SLAVE    (0x02)
#define SMDMPF_MEMBER_PAGE_TYPE_CONT     (0x04)
#define SMDMPF_MEMBER_PAGE_ANCHOR        (0x80)

typedef struct smdmpfLogicalHdr
{
    stlBool   mSimplePage;
    stlUInt8  mMemberPageType;      /** member page type */
    stlInt16  mRowCount;            /** page에 저장되는 row count */
    stlChar   mPadding[4];
    stlInt32  mFromOrd;             /** page에 저장되는 시작 column order */
    stlInt32  mToOrd;               /** page에 저장되는 마지막 column order */
    smlPid    mNextChainedPid;       /** next chained member page Pid */
    smlPid    mPrevChainedPid;       /** next chained member page Pid */
    stlInt64  mSegmentSeq;
} smdmpfLogicalHdr;

typedef struct smdmpfColMetaHdr
{
    smlPid     mNextPageId;       /* Next Column Page Id */
    stlInt16   mHighWaterMark;
    stlChar    mPad[2];
} smdmpfColMetaHdr;

#define SMDMPF_GET_LOGICAL_HEADER(aPageFrame)                                           \
    ((smdmpfLogicalHdr*)(((smpFixedSlotSignpost*)(SMP_GET_SIGNPOST(aPageFrame))) + 1))

#define SMDMPF_GET_SLOT_COUNT(aPageFrame)                                               \
    (((smpFixedSlotSignpost*)(SMP_GET_SIGNPOST(aPageFrame)))->mAllocatedMaxSlotSeq + 1)

#define SMDMPF_GET_START_POS(aPageFrame) \
    (SMDMPF_GET_LOGICAL_HEADER(aPageFrame) + 1)

#define SMDMPF_GET_PAGE_COL_COUNT(aPageFrame)                  \
    (SMDMPF_GET_LOGICAL_HEADER(aPageFrame)->mToOrd -           \
     SMDMPF_GET_LOGICAL_HEADER(aPageFrame)->mFromOrd + 1)

#define SMDMPF_GET_PAGE_BODY(aPageFrame)                                \
    ((stlChar *)SMDMPF_GET_START_POS(aPageFrame) +                      \
     STL_SIZEOF(stlInt16) * SMDMPF_GET_PAGE_COL_COUNT(aPageFrame))

#define SMDMPF_GET_NTH_ROW_HDR(aPageFrame, aSeq)                        \
    (SMDMPF_GET_PAGE_BODY(aPageFrame) + ((aSeq) * SMDMPF_ROW_HDR_LEN))

#define SMDMPF_IS_NTH_ROW_HDR_FREE(aPageFrame, aSeq)                            \
    ((SMDMPF_GET_SLOT_COUNT(aPageFrame) <= aSeq) ||                             \
     (SMDMPF_IS_IN_USE(SMDMPF_GET_NTH_ROW_HDR(aPageFrame, aSeq)) != STL_TRUE))

#define SMDMPF_GET_NEXT_CHAINED_PAGE_ID(aPageFrame)             \
    (SMDMPF_GET_LOGICAL_HEADER(aPageFrame)->mNextChainedPid)

#define SMDMPF_GET_PREV_CHAINED_PAGE_ID(aPageFrame)             \
    (SMDMPF_GET_LOGICAL_HEADER(aPageFrame)->mPrevChainedPid)

#define SMDMPF_GET_MEMBER_PAGE_TYPE(aPageFrame)                                                 \
    (SMDMPF_GET_LOGICAL_HEADER(aPageFrame)->mMemberPageType & SMDMPF_MEMBER_PAGE_TYPE_MASK)

#define SMDMPF_IS_ANCHOR_MEMBER_PAGE(aPageFrame)                                               \
    ((SMDMPF_GET_LOGICAL_HEADER(aPageFrame)->mMemberPageType & SMDMPF_MEMBER_PAGE_ANCHOR) ==   \
     SMDMPF_MEMBER_PAGE_ANCHOR)

#define SMDMPF_GET_ROW_COUNT(aPageFrame)               \
    (SMDMPF_GET_LOGICAL_HEADER(aPageFrame)->mRowCount)

#define SMDMPF_IS_PAGE_OVER_INSERT_LIMIT(aPageFrame)                           \
    (SMDMPF_GET_SLOT_COUNT(aPageFrame) == SMDMPF_GET_ROW_COUNT(aPageFrame))

/**
 * @brief 페이지내의 Active Slot 개수를 반환함
 */
#define SMDMPF_GET_ACTIVE_SLOT_COUNT( aPageFrame )                                      \
    ( ((smpFixedSlotSignpost*)(SMP_GET_SIGNPOST(aPageFrame)))->mActiveSlotCount )
/**
 * @brief 페이지내의 Active Slot 개수를 증가시킴
 */
#define SMDMPF_INCREASE_ACTIVE_SLOT_COUNT( aPageFrame )                                 \
    {                                                                                   \
        ((smpFixedSlotSignpost*)(SMP_GET_SIGNPOST(aPageFrame)))->mActiveSlotCount++;    \
    }

/**
 * @brief 페이지내의 Active Slot 개수를 감소시킴
 */
#define SMDMPF_DECREASE_ACTIVE_SLOT_COUNT( aPageFrame )                                 \
    {                                                                                   \
        ((smpFixedSlotSignpost*)(SMP_GET_SIGNPOST(aPageFrame)))->mActiveSlotCount--;    \
    }

#define SMDMPF_GET_ALLOCATED_MAX_SLOT_SEQ( aPageFrame )                                 \
    {                                                                                   \
        ((smpFixedSlotSignpost*)(SMP_GET_SIGNPOST(aPageFrame)))->mAllocatedMaxSlotSeq;  \
    }

/**
 * @brief Fixed slot page의 max free size =
 * SMP_PAGE_SIZE - 
 *  ( physical header +
 *    physical tail +
 *    fixed slot signpost
 *    fixed slot signpost
 *    start offset * (column # - 1) )
 */
#define SMDMPF_PAGE_MAX_FREE_SIZE(aColCount)                    \
    ( SMP_PAGE_SIZE - (STL_SIZEOF(smpPhyHdr) +                  \
                       STL_SIZEOF(smpPhyTail) +                 \
                       STL_SIZEOF(smpFixedSlotSignpost) +       \
                       STL_SIZEOF(smdmpfLogicalHdr) +           \
                       (aColCount) * STL_SIZEOF(stlUInt16)) )

#define SMDMPF_INIT_ROWITEM_ITERATOR( aPageFrame, aIterator )                           \
    {                                                                                   \
        (aIterator)->mPage = (smpPhyHdr*)(aPageFrame);                                  \
        (aIterator)->mHeader = NULL;                                                    \
        (aIterator)->mCurSlot = (stlUInt8*)(SMDMPF_GET_PAGE_BODY(aPageFrame) -          \
                                            SMDMPF_ROW_HDR_LEN);                        \
        (aIterator)->mSlotFence = (stlUInt8*)(SMDMPF_GET_PAGE_BODY(aPageFrame) +        \
                                              SMDMPF_ROW_HDR_LEN *                      \
                                              SMDMPF_GET_SLOT_COUNT(aPageFrame));       \
        (aIterator)->mCurSeq = -1;                                                      \
    }

#define SMDMPF_FOREACH_ROWITEM_ITERATOR( aIterator )            \
    for( ++(aIterator)->mCurSeq,                                \
             (aIterator)->mCurSlot += SMDMPF_ROW_HDR_LEN;       \
         (aIterator)->mCurSlot < (aIterator)->mSlotFence;       \
         ++(aIterator)->mCurSeq,                                \
             (aIterator)->mCurSlot += SMDMPF_ROW_HDR_LEN )      \

typedef struct smdmpfIterator
{
    smiIterator          mCommon;
    smlRid               mRowRid;
    smlScn               mAgableViewScn;
    void               * mSegIterator;
    smlPid               mPageBufPid;
    stlInt16             mRowSeq;
    stlInt64             mPageBuf[SMP_PAGE_SIZE/STL_SIZEOF(stlInt64)];
} smdmpfIterator;

typedef struct smdmpfFxOffsetTblHdr
{
    smlTbsId mTbsId;
    smlPid   mPageId;
    stlInt16 mSlotCount;
} smdmpfFxOffsetTblHdr;

typedef struct smdmpfOffsetTblHdrPathCtrl
{
    void   * mSegIterator;
    void   * mSegHandle;
    smlPid   mCurPageId;
} smdmpfOffsetTblHdrPathCtrl;

typedef struct smdmpfFxRow
{
    stlInt16     mTbsId;
    stlInt32     mCurPageId;
    stlInt16     mCurSlotSeq;
    stlInt16     mIsDeleted;
    stlInt16     mIsMaster;
    stlInt16     mIsContCol;
    stlInt16     mIsLocked;
    stlInt32     mTcn;
    stlInt16     mIsRtsBound;
    stlInt16     mRtsSeq;
    stlInt64     mScn;
    stlInt64     mTransId;
    stlInt32     mRollbackPid;
    stlInt16     mRollbackOffset;
    stlInt16     mColCount;
    stlInt16     mPadSpace;
    stlInt16     mHasNextLink;
    stlInt32     mNextPid;
    stlInt16     mNextOffset;
} smdmpfFxRow;


typedef struct smdmpfFxRowIterator
{
    smlTbsId          mTbsId;
    void            * mSegIterator;
    smpHandle         mPageHandle;
    stlInt64          mPageBuf[SMP_PAGE_SIZE/STL_SIZEOF(stlInt64)];
    smlPid            mCurPid;
    stlInt16          mCurSlotSeq;
    stlInt16          mTotalSlotCount;
    smdmpfFxRow       mRowRecord;
} smdmpfFxRowIterator;


typedef struct smdmpfFxLogicalHdr
{
    smlPid     mCurPid;
    stlBool    mSimplePage;
} smdmpfFxLogicalHdr;

typedef struct smdmpfFxLogicalHdrIterator
{
    smlTbsId        mTbsId;
    void          * mSegIterator;
    smlPid          mCurPid;
} smdmpfFxLogicalHdrIterator;

/**
 * @brief Update시 기록되는 undo record의 최대 크기를 고려한 사이즈
 * - first column order(4) + relative column order
 */
#define SMDMPF_MAX_PADDING_SIZE  ( SMD_MAX_COLUMN_COUNT_IN_PIECE + STL_SIZEOF(stlInt32) )

/**
 * @brief variable column을 처리하기 위한 minimum value length
 * - default 32보다 큰 value length가 삽입되는 경우 continuous value를 생성한다.
 */
#define SMDMPF_CONTINUOUS_COLUMN_DEFAULT_LENGTH  (32)

/** @} */
    
#endif /* _SMDDEF_H_ */
