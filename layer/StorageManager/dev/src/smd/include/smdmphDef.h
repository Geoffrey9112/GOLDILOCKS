/*******************************************************************************
 * smdmphDef.h
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


#ifndef _SMDMPHDEF_H_
#define _SMDMPHDEF_H_ 1


/**
 * @file smdmphDef.h
 * @brief Storage Manager Layer Memory Heap Table Internal Definition
 */

/**
 * @defgroup smdmphInternal Component Internal Routines
 * @ingroup smd
 * @{
 */

/**********************************/
/* MEMORY HEAP FIELD DEFINITION   */
/**********************************/

#define  SMDMPH_SET_DELETED( aRow )                       \
      {                                                   \
          *(stlChar*)(aRow) |= SMDMPH_FLAG_DELETED;       \
      }
#define  SMDMPH_SET_MASTER_ROW(aRow)                      \
      {                                                   \
          *(stlChar*)(aRow) |= SMDMPH_FLAG_IS_MASTER;     \
      }
#define  SMDMPH_SET_CONT_COL(aRow)                        \
      {                                                   \
          *(stlChar*)(aRow) |= SMDMPH_FLAG_IS_CONT_COL;   \
      }
#define  SMDMPH_SET_CONT_COL_LAST(aRow)                     \
    {                                                       \
        *(stlChar*)(aRow) |= SMDMPH_FLAG_IS_CONT_COL_LAST;  \
    }
#define  SMDMPH_SET_LOCKED(aRow)                          \
      {                                                   \
          *(stlChar*)(aRow) |= SMDMPH_FLAG_IS_LOCKED;     \
      }
#define  SMDMPH_SET_RTS_BOUND(aRow)                       \
      {                                                   \
          *(stlChar*)(aRow) |= SMDMPH_FLAG_IS_RTS_BOUND;  \
      }
#define  SMDMPH_SET_HAS_NEXT_LINK(aRow)                   \
      {                                                   \
          *(stlChar*)(aRow) |= SMDMPH_FLAG_HAS_NEXT_LINK; \
      }

#define  SMDMPH_UNSET_DELETED(aRow)                          \
      {                                                      \
          *(stlChar*)(aRow) &= ~(SMDMPH_FLAG_DELETED);       \
      }
#define  SMDMPH_UNSET_MASTER_ROW(aRow)                       \
      {                                                      \
          *(stlChar*)(aRow) &= ~(SMDMPH_FLAG_IS_MASTER);     \
      }
#define  SMDMPH_UNSET_CONT_COL(aRow)                         \
      {                                                      \
          *(stlChar*)(aRow) &= ~(SMDMPH_FLAG_IS_CONT_COL);   \
      }
#define  SMDMPH_UNSET_LOCKED(aRow)                           \
      {                                                      \
          *(stlChar*)(aRow) &= ~(SMDMPH_FLAG_IS_LOCKED);     \
      }
#define  SMDMPH_UNSET_RTS_BOUND(aRow)                        \
      {                                                      \
          *(stlChar*)(aRow) &= ~(SMDMPH_FLAG_IS_RTS_BOUND);  \
      }
#define  SMDMPH_UNSET_HAS_NEXT_LINK(aRow)                    \
      {                                                      \
          *(stlChar*)(aRow) &= ~(SMDMPH_FLAG_HAS_NEXT_LINK); \
      }


#define  SMDMPH_SET_TCN(aRow, aTcn)                                                     \
      ( STL_WRITE_INT32((stlChar*)(aRow) + SMDMPH_OFFSET_TCN, (aTcn)) )
#define  SMDMPH_SET_SCN(aRow, aScn)                                                     \
      ( STL_WRITE_INT64((stlChar*)(aRow) + SMDMPH_OFFSET_SCN_RTSSEQ, (aScn)) )
#define  SMDMPH_SET_RTSSEQ(aRow, aRtsSeq)                                               \
      ( STL_WRITE_INT8((stlChar*)(aRow) + SMDMPH_OFFSET_SCN_RTSSEQ, (aRtsSeq)) )
#define  SMDMPH_SET_TRANS_ID(aRow, aTransId)                                            \
      ( STL_WRITE_INT64((stlChar*)(aRow) + SMDMPH_OFFSET_TRANS_ID, (aTransId)) )
#define  SMDMPH_SET_ROLLBACK_PID(aRow, aPid)                                            \
      ( STL_WRITE_INT32((stlChar*)(aRow) + SMDMPH_OFFSET_ROLLBACK_PID, (aPid)) )
#define  SMDMPH_SET_ROLLBACK_OFFSET(aRow, aOffset)                                      \
      ( STL_WRITE_INT16((stlChar*)(aRow) + SMDMPH_OFFSET_ROLLBACK_OFFSET, (aOffset)) )
#define  SMDMPH_SET_COLUMN_CNT(aRow, aColCnt)                                           \
      ( STL_WRITE_INT16((stlChar*)(aRow) + SMDMPH_OFFSET_COLUMN_CNT, (aColCnt)) )
#define  SMDMPH_SET_PAD_SPACE(aRow, aPadSpace)                                          \
      ( STL_WRITE_INT16((stlChar*)(aRow) + SMDMPH_OFFSET_PAD_SPACE, (aPadSpace)) )
#define  SMDMPH_SET_LINK_PID(aRow, aPid)                                                \
      ( STL_WRITE_INT32((stlChar*)(aRow) + SMDMPH_OFFSET_LINK_PID, (aPid)) )
#define  SMDMPH_SET_LINK_OFFSET(aRow, aOffset)                                          \
      ( STL_WRITE_INT16((stlChar*)(aRow) + SMDMPH_OFFSET_LINK_OFFSET, (aOffset)) )


#define  SMDMPH_MAXIMUM_INSERTABLE_PAGE_SEARCH_COUNT (SMS_MAX_PAGE_HINT_COUNT*2)

#define  SMDMPH_COPY_ROW_HDR( aDst, aSrc )            \
{                                                     \
    stlMemcpy( aDst, aSrc, 37 );                      \
}


#define  SMDMPH_IS_SAME_TRANS( aTransIdA, aCommitScnA, aTransIdB, aCommitScnB )    \
    ( ((aCommitScnA  == aCommitScnB) &&                                            \
       ((aCommitScnA  != SML_INFINITE_SCN) ||                                      \
        ((aCommitScnA  == SML_INFINITE_SCN) && (aTransIdA == aTransIdB) ))) ?      \
      STL_TRUE :  STL_FALSE )

#define SMDMPH_CONT_SIZE( aColLen, aColLenSize )  \
    ( SMDMPH_MAX_ROW_HDR_LEN + aColLen + aColLenSize )

typedef struct smdmphLogicalHdr
{
    stlBool   mSimplePage;
    stlChar   mPadding[7];
    stlInt64  mSegmentSeq;
} smdmphLogicalHdr;

#define  SMDMPH_GET_LOGICAL_HEADER(aPageHandle)                                          \
    ((smdmphLogicalHdr*)(((smpOrderedSlotSignpost*)(SMP_GET_SIGNPOST(SMP_FRAME(aPageHandle)))) + 1))

typedef struct smdmphIterator
{
    smiIterator          mCommon;
    smlRid               mRowRid;
    smlScn               mAgableViewScn;
    void               * mSegIterator;
    smlPid               mPageBufPid;
    stlInt64             mPageBuf[SMP_PAGE_SIZE/STL_SIZEOF(stlInt64)];
} smdmphIterator;

typedef struct smdmphFxOffsetTblHdr
{
    smlTbsId mTbsId;
    smlPid   mPageId;
    stlInt16 mSlotCount;
} smdmphFxOffsetTblHdr;

typedef struct smdmphOffsetTblHdrPathCtrl
{
    void   * mSegIterator;
    void   * mSegHandle;
    smlPid   mCurPageId;
} smdmphOffsetTblHdrPathCtrl;

typedef struct smdmphFxRow
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
} smdmphFxRow;


typedef struct smdmphFxRowIterator
{
    smlTbsId          mTbsId;
    void            * mSegIterator;
    smpHandle         mPageHandle;
    stlInt64          mPageBuf[SMP_PAGE_SIZE/STL_SIZEOF(stlInt64)];
    smlPid            mCurPid;
    stlInt16          mCurSlotSeq;
    stlInt16          mTotalSlotCount;
    smdmphFxRow       mRowRecord;
} smdmphFxRowIterator;


typedef struct smdmphFxLogicalHdr
{
    smlPid     mCurPid;
    stlBool    mSimplePage;
} smdmphFxLogicalHdr;

typedef struct smdmphFxLogicalHdrIterator
{
    smlTbsId        mTbsId;
    void          * mSegIterator;
    smlPid          mCurPid;
} smdmphFxLogicalHdrIterator;

/**
 * @brief Update시 기록되는 undo record의 최대 크기를 고려한 사이즈
 * - first column order(4) + relative column order
 */
#define SMDMPH_MAX_PADDING_SIZE  ( SMD_MAX_COLUMN_COUNT_IN_PIECE + STL_SIZEOF(stlInt32) )

/** @} */
    
#endif /* _SMDDEF_H_ */
