/*******************************************************************************
 * smdmpbDef.h
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


#ifndef _SMNMPBDEF_H_
#define _SMNMPBDEF_H_ 1

#include <stlDef.h>
#include <smlDef.h>

/**
 * @file smnmpbDef.h
 * @brief Storage Manager Layer Memory B-Tree Index Internal Definition
 */

/**
 * @defgroup smnmpbInternal Component Internal Routines
 * @ingroup smn
 * @{
 */

typedef struct smnmpbLogicalHdr
{
    smlPid       mPrevPageId;
    smlPid       mNextPageId;
    smlPid       mNextFreePageId;
    stlUInt16    mLevel;
    stlUInt8     mFlag;
    stlUInt8     mKeyColCount;
    stlUInt8     mKeyColTypes[SML_MAX_INDEX_KEY_COL_COUNT];
    stlUInt8     mKeyColFlags[SML_MAX_INDEX_KEY_COL_COUNT];
} smnmpbLogicalHdr;

#define  SMNMPB_NODE_FLAG_INIT               (0x00)
#define  SMNMPB_NODE_FLAG_LAST_SIBLING_MASK  (0x80)
#define  SMNMPB_NODE_FLAG_LAST_SIBLING       SMNMPB_NODE_FLAG_LAST_SIBLING_MASK
#define  SMNMPB_NODE_FLAG_EMPTY_LIST_MASK    (0x40)
#define  SMNMPB_NODE_FLAG_EMPTY_LIST         SMNMPB_NODE_FLAG_EMPTY_LIST_MASK

#define  SMNMPB_IS_LAST_SIBLING(aPageHandle)                                                      \
    ( (((SMNMPB_GET_LOGICAL_HEADER(aPageHandle))->mFlag & SMNMPB_NODE_FLAG_LAST_SIBLING_MASK)     \
       == SMNMPB_NODE_FLAG_LAST_SIBLING) ? STL_TRUE : STL_FALSE )
#define  SMNMPB_IS_ON_EMPTY_LIST(aPageHandle)                                                     \
    ( (((SMNMPB_GET_LOGICAL_HEADER(aPageHandle))->mFlag & SMNMPB_NODE_FLAG_EMPTY_LIST_MASK)       \
       == SMNMPB_NODE_FLAG_EMPTY_LIST) ? STL_TRUE : STL_FALSE )

#define  SMNMPB_GET_NODE_FLAG(aPageHandle) ((SMNMPB_GET_LOGICAL_HEADER(aPageHandle))->mFlag)


#define SMNMPB_COMPARE_FUNC( aType )  dtlPhysicalCompareFuncList[(aType)][(aType)]

#define  SMNMPB_GET_LOGICAL_HEADER(aPageHandle)                                          \
    ((smnmpbLogicalHdr*)(((smpOrderedSlotSignpost*)(SMP_GET_SIGNPOST(SMP_FRAME(aPageHandle)))) + 1))
#define  SMNMPB_GET_PREV_PAGE(aPageHandle) ((SMNMPB_GET_LOGICAL_HEADER(aPageHandle))->mPrevPageId)
#define  SMNMPB_GET_NEXT_PAGE(aPageHandle) ((SMNMPB_GET_LOGICAL_HEADER(aPageHandle))->mNextPageId)
#define  SMNMPB_GET_NEXT_FREE_PAGE(aPageHandle) ((SMNMPB_GET_LOGICAL_HEADER(aPageHandle))->mNextFreePageId)
#define  SMNMPB_GET_LEVEL(aPageHandle) ((SMNMPB_GET_LOGICAL_HEADER(aPageHandle))->mLevel)
#define  SMNMPB_GET_ACTIVE_KEY_COUNT(aPageHandle) ((SMNMPB_GET_LOGICAL_HEADER(aPageHandle))->mActiveKeyCount)
#define  SMNMPB_GET_KEY_COLUMN_COUNT(aPageHandle) ((SMNMPB_GET_LOGICAL_HEADER(aPageHandle))->mKeyColCount)
#define  SMNMPB_GET_KEY_COLUMN_TYPES(aPageHandle) ((SMNMPB_GET_LOGICAL_HEADER(aPageHandle))->mKeyColTypes)
#define  SMNMPB_GET_KEY_COLUMN_FLAGS(aPageHandle) ((SMNMPB_GET_LOGICAL_HEADER(aPageHandle))->mKeyColFlags)

#define  SMNMPB_IS_LEAF(aPageHandle) (SMNMPB_GET_LEVEL(aPageHandle) == 0 ? STL_TRUE : STL_FALSE )
#define  SMNMPB_IS_SEMI_LEAF(aPageHandle) (SMNMPB_GET_LEVEL(aPageHandle) == 1 ? STL_TRUE : STL_FALSE )

#define  SMNMPB_SET_PREV_PAGE(aPageHandle,aPid)                     \
    ((SMNMPB_GET_LOGICAL_HEADER(aPageHandle))->mPrevPageId = aPid)
#define  SMNMPB_SET_NEXT_PAGE(aPageHandle,aPid)                     \
    ((SMNMPB_GET_LOGICAL_HEADER(aPageHandle))->mNextPageId = aPid)
#define  SMNMPB_SET_NEXT_FREE_PAGE(aPageHandle,aPid)                  \
    ((SMNMPB_GET_LOGICAL_HEADER(aPageHandle))->mNextFreePageId = aPid)

#define  SMNMPB_MAX_INDEX_DEPTH  (KNL_ENV_MAX_LATCH_DEPTH / 4)

typedef struct smnmpbPathStackElem
{
    smlPid    mPid;
    stlInt16  mSeq;
    stlUInt64 mSmoNo;
} smnmpbPathStackElem;

typedef struct smnmpbPathStack
{
    stlInt16              mDepth;
    smnmpbPathStackElem   mStack[SMNMPB_MAX_INDEX_DEPTH];
} smnmpbPathStack;

typedef struct smnmpbPropKeyStr
{
    stlChar     * mPropKeyBody;
    smlRid        mRowRid;
    stlUInt16     mKeyBodySize;
    stlInt16      mBeginPos;
    stlBool       mIsPropKeyLast;
} smnmpbPropKeyStr;

/* Internal key Slot Header (ChildPID + RowPID + RowOffset + Columns)  */
/* 단, 각 internal sibling들의 Rightmost key는 ChildPID만 갖는다 */ 
#define SMNMPB_INTL_SLOT_HDR_SIZE (STL_SIZEOF(smlPid) + STL_SIZEOF(smlPid) + STL_SIZEOF(stlInt16))
#define SMNMPB_INTL_SLOT_GET_COLUMNS(aKey)                                             \
    ((stlChar*)(aKey) + STL_SIZEOF(smlPid) + STL_SIZEOF(smlPid) + STL_SIZEOF(stlInt16))

#define SMNMPB_INTL_SLOT_GET_CHILD_PID(aKey,aPid)    (STL_WRITE_INT32(aPid, aKey))
#define SMNMPB_INTL_SLOT_GET_ROW_PID(aKey,aPid)                                           \
    (STL_WRITE_INT32(aPid,(stlChar*)(aKey) + STL_SIZEOF(smlPid)))
#define SMNMPB_INTL_SLOT_GET_ROW_OFFSET(aKey,aOffset)                                     \
    (STL_WRITE_INT16(aOffset,(stlChar*)(aKey) + STL_SIZEOF(smlPid) + STL_SIZEOF(smlPid)))

#define SMNMPB_INTL_SLOT_SET_CHILD_PID(aKey,aPid)    (STL_WRITE_INT32(aKey, aPid))
#define SMNMPB_INTL_SLOT_SET_ROW_PID(aKey,aPid)                                           \
    (STL_WRITE_INT32((stlChar*)(aKey) + STL_SIZEOF(smlPid), aPid))
#define SMNMPB_INTL_SLOT_SET_ROW_OFFSET(aKey,aOffset)                                     \
    (STL_WRITE_INT16((stlChar*)(aKey) + STL_SIZEOF(smlPid) + STL_SIZEOF(smlPid), aOffset))


/* Leaf Key Slot Header의 구조 */
typedef enum
{
    SMNMPB_F_FIELD_DELETE_FLAG,
    SMNMPB_F_FIELD_DUPKEY_FLAG,
    SMNMPB_F_FIELD_ROW_PID,
    SMNMPB_F_FIELD_ROW_SEQ,
    SMNMPB_F_FIELD_INSERT_RTSSEQ,
    SMNMPB_F_FIELD_DELETE_RTSSEQ,
    SMNMPB_F_FIELD_RTS_VERSION_NO,
    /* Optional FIELDs */
    SMNMPB_O_FIELD_INSERT_TRANSID,
    SMNMPB_O_FIELD_INSERT_SCN,
    SMNMPB_O_FIELD_DELETE_TRANSID,
    SMNMPB_O_FIELD_DELETE_SCN,
} smdmphHdrFields;


/**********************************/
/* MEMORY B-TREE FIELD DEFINITION   */
/**********************************/

/** KEY FLAGS */
#define  SMNMPB_FLAG_DELETED           (0x80)
#define  SMNMPB_FLAG_DUPKEY            (0x40)
#define  SMNMPB_FLAG_TBK               (0x20)
#define  SMNMPB_INIT_KEY_HDR_BITMAP( aKey )     \
{                                               \
    *((stlChar*)(aKey)) = 0;                    \
}

#define SMNMPB_MAX_TCN                (0xFF)
#define SMNMPB_INVALID_TCN            (SMNMPB_MAX_TCN)

#define SMNMPB_INS_RTS_VER_NO_MASK    (0xF0)
#define SMNMPB_DEL_RTS_VER_NO_MASK    (0x0F)

/** KEY OFFSET & LENGTHs */
#define  SMNMPB_OFFSET_ROW_PID              (STL_SIZEOF(stlChar))
#define  SMNMPB_LEN_ROW_PID                 (STL_SIZEOF(smlPid))
#define  SMNMPB_OFFSET_ROW_SEQ              (SMNMPB_OFFSET_ROW_PID + SMNMPB_LEN_ROW_PID)
#define  SMNMPB_LEN_ROW_SEQ                 (STL_SIZEOF(stlInt16))
#define  SMNMPB_OFFSET_INS_RTSSEQ           (SMNMPB_OFFSET_ROW_SEQ + SMNMPB_LEN_ROW_SEQ)
#define  SMNMPB_LEN_INS_RTSSEQ              (STL_SIZEOF(stlUInt8))
#define  SMNMPB_OFFSET_DEL_RTSSEQ           (SMNMPB_OFFSET_INS_RTSSEQ + SMNMPB_LEN_INS_RTSSEQ)
#define  SMNMPB_LEN_DEL_RTSSEQ              (STL_SIZEOF(stlUInt8))

#define  SMNMPB_OFFSET_INS_TCN              (SMNMPB_OFFSET_DEL_RTSSEQ + SMNMPB_LEN_DEL_RTSSEQ)
#define  SMNMPB_LEN_INS_TCN                 (STL_SIZEOF(stlUInt8))
#define  SMNMPB_OFFSET_DEL_TCN              (SMNMPB_OFFSET_INS_TCN + SMNMPB_LEN_INS_TCN)
#define  SMNMPB_LEN_DEL_TCN                 (STL_SIZEOF(stlUInt8))

#define  SMNMPB_OFFSET_RTS_VERNO            (SMNMPB_OFFSET_DEL_TCN + SMNMPB_LEN_DEL_TCN)
#define  SMNMPB_LEN_RTS_VERNO               (STL_SIZEOF(stlUInt8))
#define  SMNMPB_OFFSET_INS_TRANS_ID         (SMNMPB_OFFSET_RTS_VERNO + SMNMPB_LEN_RTS_VERNO)
#define  SMNMPB_LEN_INS_TRANS_ID            (STL_SIZEOF(smxlTransId))
#define  SMNMPB_OFFSET_INS_SCN              (SMNMPB_OFFSET_INS_TRANS_ID + SMNMPB_LEN_INS_TRANS_ID)
#define  SMNMPB_LEN_INS_SCN                 (STL_SIZEOF(smlScn))
#define  SMNMPB_OFFSET_DEL_TRANS_ID         (SMNMPB_OFFSET_INS_SCN + SMNMPB_LEN_INS_SCN)
#define  SMNMPB_LEN_DEL_TRANS_ID            (STL_SIZEOF(smxlTransId))
#define  SMNMPB_OFFSET_DEL_SCN              (SMNMPB_OFFSET_DEL_TRANS_ID + SMNMPB_LEN_DEL_TRANS_ID)
#define  SMNMPB_LEN_DEL_SCN                 (STL_SIZEOF(smlScn))

#define  SMNMPB_MIN_LEAF_KEY_HDR_LEN        (SMNMPB_OFFSET_INS_TRANS_ID)
#define  SMNMPB_MAX_LEAF_KEY_HDR_LEN        (SMNMPB_OFFSET_DEL_SCN + SMNMPB_LEN_DEL_SCN)
#define  SMNMPB_RBK_HDR_SIZE                (SMNMPB_MIN_LEAF_KEY_HDR_LEN)
#define  SMNMPB_TBK_HDR_SIZE                (SMNMPB_MAX_LEAF_KEY_HDR_LEN)

#define  SMNMPB_GET_LEAF_KEY_HDR_SIZE( aKey, aKeyHdrSize )           \
{                                                                    \
    *(aKeyHdrSize) = (SMNMPB_IS_TBK( (aKey) ) == STL_TRUE) ?         \
    SMNMPB_TBK_HDR_SIZE : SMNMPB_RBK_HDR_SIZE;                       \
}


/** ROW FIELD INTERFACES */
#define  SMNMPB_IS_DELETED(aKey)                                                  \
    ( (((*(stlChar*)(aKey)) & SMNMPB_FLAG_DELETED) == SMNMPB_FLAG_DELETED) ?      \
      STL_TRUE : STL_FALSE )
#define  SMNMPB_IS_DUPKEY(aKey)                                                   \
    ( (((*(stlChar*)(aKey)) & SMNMPB_FLAG_DUPKEY) == SMNMPB_FLAG_DUPKEY) ?        \
      STL_TRUE : STL_FALSE )
#define  SMNMPB_IS_TBK(aKey)                                                      \
    ( (((*(stlChar*)(aKey)) & SMNMPB_FLAG_TBK) == SMNMPB_FLAG_TBK) ?              \
      STL_TRUE : STL_FALSE )

#define  SMNMPB_SET_DELETED( aKey )                   \
      {                                               \
          *(stlChar*)(aKey) |= SMNMPB_FLAG_DELETED;   \
      }
#define  SMNMPB_SET_DUPKEY( aKey )                    \
      {                                               \
          *(stlChar*)(aKey) |= SMNMPB_FLAG_DUPKEY;    \
      }
#define  SMNMPB_SET_TBK( aKey )                       \
      {                                               \
          *(stlChar*)(aKey) |= SMNMPB_FLAG_TBK;       \
      }

#define  SMNMPB_UNSET_DELETED(aKey)                      \
      {                                                  \
          *(stlChar*)(aKey) &= ~(SMNMPB_FLAG_DELETED);   \
      }
#define  SMNMPB_UNSET_DUPKEY(aKey)                       \
      {                                                  \
          *(stlChar*)(aKey) &= ~(SMNMPB_FLAG_DUPKEY);    \
      }
#define  SMNMPB_UNSET_TBK(aKey)                          \
      {                                                  \
          *(stlChar*)(aKey) &= ~(SMNMPB_FLAG_TBK);       \
      }


#define  SMNMPB_GET_ROW_PID(aKey, aPid)                                              \
    ( STL_WRITE_INT32((aPid), (stlChar*)(aKey) + SMNMPB_OFFSET_ROW_PID) )
#define  SMNMPB_GET_ROW_SEQ(aKey, aSeq)                                              \
    ( STL_WRITE_INT16((aSeq), (stlChar*)(aKey) + SMNMPB_OFFSET_ROW_SEQ) )
#define  SMNMPB_GET_INS_RTSSEQ(aKey, aRtsSeq)                                        \
    ( STL_WRITE_INT8((aRtsSeq), (stlChar*)(aKey) + SMNMPB_OFFSET_INS_RTSSEQ) )
#define  SMNMPB_GET_DEL_RTSSEQ(aKey, aRtsSeq)                                        \
    ( STL_WRITE_INT8((aRtsSeq), (stlChar*)(aKey) + SMNMPB_OFFSET_DEL_RTSSEQ) )
#define  SMNMPB_GET_INS_TCN(aKey, aTcn)                                              \
    ( STL_WRITE_INT8((aTcn), (stlChar*)(aKey) + SMNMPB_OFFSET_INS_TCN) )
#define  SMNMPB_GET_DEL_TCN(aKey, aTcn)                                              \
    ( STL_WRITE_INT8((aTcn), (stlChar*)(aKey) + SMNMPB_OFFSET_DEL_TCN) )
#define  SMNMPB_GET_RTS_VERNO(aKey, aRtsVerNo)                                       \
    ( STL_WRITE_INT8((aRtsVerNo), (stlChar*)(aKey) + SMNMPB_OFFSET_RTS_VERNO) )
#define  SMNMPB_GET_TBK_INS_TRANS_ID(aKey, aTransId)                                 \
    ( STL_WRITE_INT64((aTransId), (stlChar*)(aKey) + SMNMPB_OFFSET_INS_TRANS_ID) )
#define  SMNMPB_GET_TBK_INS_SCN(aKey, aScn)                                          \
    ( STL_WRITE_INT64((aScn), (stlChar*)(aKey) + SMNMPB_OFFSET_INS_SCN) )
#define  SMNMPB_GET_TBK_DEL_TRANS_ID(aKey, aTransId)                                 \
    ( STL_WRITE_INT64((aTransId), (stlChar*)(aKey) + SMNMPB_OFFSET_DEL_TRANS_ID) )
#define  SMNMPB_GET_TBK_DEL_SCN(aKey, aScn)                                          \
    ( STL_WRITE_INT64((aScn), (stlChar*)(aKey) + SMNMPB_OFFSET_DEL_SCN) )

#define  SMNMPB_GET_INS_RTS_VER_NO(verno)                            \
    ( (((stlUInt8)(verno)) & SMNMPB_INS_RTS_VER_NO_MASK) >> 4 )
#define  SMNMPB_GET_DEL_RTS_VER_NO(verno)                            \
    ( ((stlUInt8)(verno)) & SMNMPB_DEL_RTS_VER_NO_MASK )

#define SMNMPB_GET_INS_RTS_INFO( aKey, aRtsSeq, aRtsVerNo )              \
{                                                                        \
    stlUInt8  tmpRtsVerField;                                            \
    SMNMPB_GET_INS_RTSSEQ( (aKey), (aRtsSeq) );                          \
    SMNMPB_GET_RTS_VERNO( (aKey), &tmpRtsVerField );                     \
    *(aRtsVerNo) = SMNMPB_GET_INS_RTS_VER_NO( tmpRtsVerField );          \
}
#define SMNMPB_GET_DEL_RTS_INFO( aKey, aRtsSeq, aRtsVerNo )              \
{                                                                        \
    stlUInt8  tmpRtsVerField;                                            \
    SMNMPB_GET_DEL_RTSSEQ( (aKey), (aRtsSeq) );                          \
    SMNMPB_GET_RTS_VERNO( (aKey), &tmpRtsVerField );                     \
    *(aRtsVerNo) = SMNMPB_GET_DEL_RTS_VER_NO( tmpRtsVerField );          \
}


#define  SMNMPB_SET_ROW_PID(aKey, aPid)                                                    \
      ( STL_WRITE_INT32((stlChar*)(aKey) + SMNMPB_OFFSET_ROW_PID, (aPid)) )
#define  SMNMPB_SET_ROW_SEQ(aKey, aSeq)                                                    \
      ( STL_WRITE_INT16((stlChar*)(aKey) + SMNMPB_OFFSET_ROW_SEQ, (aSeq)) )
#define  SMNMPB_SET_INS_RTSSEQ(aKey, aRtsSeq)                                              \
      ( STL_WRITE_INT8((stlChar*)(aKey) + SMNMPB_OFFSET_INS_RTSSEQ, (aRtsSeq)) )
#define  SMNMPB_SET_DEL_RTSSEQ(aKey, aRtsSeq)                                              \
      ( STL_WRITE_INT8((stlChar*)(aKey) + SMNMPB_OFFSET_DEL_RTSSEQ, (aRtsSeq)) )
#define  SMNMPB_SET_INS_TCN(aKey, aTcn)                                                    \
      ( STL_WRITE_INT8((stlChar*)(aKey) + SMNMPB_OFFSET_INS_TCN, (aTcn)) )
#define  SMNMPB_SET_DEL_TCN(aKey, aTcn)                                                    \
      ( STL_WRITE_INT8((stlChar*)(aKey) + SMNMPB_OFFSET_DEL_TCN, (aTcn)) )
#define  SMNMPB_SET_RTS_VERNO(aKey, aRtsVerNo)                                             \
      ( STL_WRITE_INT8((stlChar*)(aKey) + SMNMPB_OFFSET_RTS_VERNO, (aRtsVerNo)) )
#define  SMNMPB_SET_TBK_INS_TRANS_ID(aKey, aTransId)                                       \
      ( STL_WRITE_INT64((stlChar*)(aKey) + SMNMPB_OFFSET_INS_TRANS_ID, (aTransId)) )
#define  SMNMPB_SET_TBK_INS_SCN(aKey, aScn)                                                \
      ( STL_WRITE_INT64((stlChar*)(aKey) + SMNMPB_OFFSET_INS_SCN, (aScn)) )
#define  SMNMPB_SET_TBK_DEL_TRANS_ID(aKey, aTransId)                                       \
      ( STL_WRITE_INT64((stlChar*)(aKey) + SMNMPB_OFFSET_DEL_TRANS_ID, (aTransId)) )
#define  SMNMPB_SET_TBK_DEL_SCN(aKey, aScn)                                                \
      ( STL_WRITE_INT64((stlChar*)(aKey) + SMNMPB_OFFSET_DEL_SCN, (aScn)) )

#define  SMNMPB_SET_INS_RTS_VER_NO(verno,res)                       \
{                                                                   \
    (res) = (res) & (~SMNMPB_INS_RTS_VER_NO_MASK);                  \
    (res) |= ((stlUInt8)(verno)) << 4;                              \
}
#define  SMNMPB_SET_DEL_RTS_VER_NO(verno,res)                       \
{                                                                   \
    (res) = (res) & (~SMNMPB_DEL_RTS_VER_NO_MASK);                  \
    (res) |= ((stlUInt8)(verno)) & SMNMPB_DEL_RTS_VER_NO_MASK;      \
}

#define  SMNMPB_SET_INS_RTS_INFO( aKey, aRtsSeq, aRtsVerNo )          \
{                                                                     \
    stlUInt8  tmpRtsVerField;                                         \
    SMNMPB_SET_INS_RTSSEQ( (aKey), (aRtsSeq) );                       \
    SMNMPB_GET_RTS_VERNO( (aKey), &tmpRtsVerField );                  \
    SMNMPB_SET_INS_RTS_VER_NO( *(aRtsVerNo), tmpRtsVerField );        \
    SMNMPB_SET_RTS_VERNO( (aKey), &tmpRtsVerField );                  \
}
#define  SMNMPB_SET_DEL_RTS_INFO( aKey, aRtsSeq, aRtsVerNo )          \
{                                                                     \
    stlUInt8  tmpRtsVerField;                                         \
    SMNMPB_SET_DEL_RTSSEQ( (aKey), (aRtsSeq) );                       \
    SMNMPB_GET_RTS_VERNO( (aKey), &tmpRtsVerField );                  \
    SMNMPB_SET_DEL_RTS_VER_NO( *(aRtsVerNo), tmpRtsVerField );        \
    SMNMPB_SET_RTS_VERNO( (aKey), &tmpRtsVerField );                  \
}



/* Key Data과 Offset Slot들이 한 페이지 내에서 차지할 수 있는 최대 영역 */
#define SMNMPB_P_HDR1 ( STL_SIZEOF(smpPhyHdr) + STL_SIZEOF(smpOrderedSlotSignpost) )
#define SMNMPB_P_HDR2 ( STL_SIZEOF(smnmpbLogicalHdr) + (STL_SIZEOF(smpRts) * SMP_MAX_RTS_COUNT) )
#define SMNMPB_P_HDR3 ( STL_SIZEOF(smpOffsetTableHeader) + STL_SIZEOF(smpPhyTail) )

#define SMNMPB_MAX_DATASECTION_SIZE ( SMP_PAGE_SIZE - (SMNMPB_P_HDR1 + SMNMPB_P_HDR2 + SMNMPB_P_HDR3) )
/* Maximum Key Body size : 각 키 컬럼들의 column length field + column length 의 합. 2012/02/25 현재 3326 */
/* column length field : column length <= 251 면 1, 더 크면 3 */
#define SMNMPB_MAX_KEY_BODY_SIZE_CALC ( (SMNMPB_MAX_DATASECTION_SIZE / 2) - STL_SIZEOF(smpOffsetSlot) - SMNMPB_MAX_SLOT_HDR_LEN )

#define SMNMPB_MAX_FREE_EXTENT_COUNT  (1024)

typedef stlInt16 smnmpbSortOffset;

/**
 * ALLOC_SIZE(2bytes) + ROW_PID(4bytes) + ROW_OFFSET(2bytes) + TCN(1byte)
 */
#define SMNMPB_RUN_KEY_HDR_LEN ( STL_SIZEOF(stlInt16) + STL_SIZEOF(smlPid) + STL_SIZEOF(stlInt16) + STL_SIZEOF(stlInt8) )

#define SMNMPB_RUN_KEY_SET_TOTAL_SIZE( aKey, aTotalSize )   \
    ( STL_WRITE_INT16((stlChar*)(aKey), (aTotalSize)) )
#define SMNMPB_RUN_KEY_SET_ROW_PID( aKey, aPid )                            \
    ( STL_WRITE_INT32((stlChar*)(aKey) + STL_SIZEOF(stlInt16), (aPid)) )
#define SMNMPB_RUN_KEY_SET_ROW_OFFSET( aKey, aOffset )                                              \
    ( STL_WRITE_INT16((stlChar*)(aKey) + STL_SIZEOF(stlInt16) + STL_SIZEOF(smlPid), (aOffset)) )
#define SMNMPB_RUN_KEY_SET_TCN( aKey, aTcn )                                                        \
    ( STL_WRITE_INT8((stlChar*)(aKey) + STL_SIZEOF(stlInt16) + STL_SIZEOF(smlPid) + STL_SIZEOF(stlInt16), (aTcn)) )

#define SMNMPB_RUN_KEY_GET_TOTAL_SIZE( aKey, aTotalSize )   \
    ( STL_WRITE_INT16((aTotalSize), (stlChar*)(aKey)) )
#define SMNMPB_RUN_KEY_GET_ROW_PID( aKey, aPid )                            \
    ( STL_WRITE_INT32((aPid), (stlChar*)(aKey) + STL_SIZEOF(stlInt16)) )
#define SMNMPB_RUN_KEY_GET_ROW_OFFSET( aKey, aOffset )                                              \
    ( STL_WRITE_INT16((aOffset), (stlChar*)(aKey) + STL_SIZEOF(stlInt16) + STL_SIZEOF(smlPid)) )
#define SMNMPB_RUN_KEY_GET_TCN( aKey, aTcn )                                                        \
    ( STL_WRITE_INT8((aTcn), (stlChar*)(aKey) + STL_SIZEOF(stlInt16) + STL_SIZEOF(smlPid) + STL_SIZEOF(stlInt16)) )


typedef struct smnmpbIterator
{
    smiIterator          mCommon;
    smlRid               mRowRid;
    void               * mBaseRelHandle;
    smlScn               mAgableViewScn;
    smlTbsId             mBaseRelTbsId;
    stlChar              mPadding[2];
    smlPid               mLastValidKeyPid;
    smlPid               mPageBufPid;
    stlInt16             mCurSlotSeq;
    stlInt16             mSlotSeqFence;
    stlInt64             mPageBuf[SMP_PAGE_SIZE/STL_SIZEOF(stlInt64)];
} smnmpbIterator;

typedef struct smnmpbParallelArgs
{
    smlEnv            * mParentEnv;
    smlTbsId            mTbsId;
    smxlTransId         mTransId;
    stlInt64            mParallelFactor;
    stlInt32            mParallelId;
    smlStatement      * mStatement;
    void              * mIndexHandle;
    void              * mBaseTableHandle;
    stlUInt16           mKeyColCount;
    knlValueBlockList * mIndexColList;
    stlUInt8          * mKeyColFlags;
    stlUInt8          * mKeyColTypes;
    stlInt8           * mKeyColOrder;
    void              * mSegIterator;
    stlInt64            mCutOffRunCount;
    stlInt32            mBuildExtentPageCount;
    stlInt32            mMaxMergeRunCount;
    stlBool             mIsUniqueIndex;
    stlBool             mIsPrimaryIndex;
    stlBool             mIsLogging;
    knlCompareFunc      mSortCompare;
    knlCompareFunc      mMergeCompare;
    smlPid              mMergedRunList[SMN_MAX_MERGED_RUN_LIST];
    smlPid              mFreeExtentArray[SMNMPB_MAX_FREE_EXTENT_COUNT];
    smlPid              mPageId[SMNMPB_MAX_INDEX_DEPTH + 1];    /**< result of sub-tree building */
    smlPid              mFirstPageId[SMNMPB_MAX_INDEX_DEPTH+1];
    stlInt32            mHeight;
    stlInt64            mTotalRunCount;
    stlInt64            mSortElapsedTime;
    stlInt64            mMergeElapsedTime;
    stlErrorStack       mErrorStack;
} smnmpbParallelArgs;

typedef struct smnmpbBuildIterator
{
    smlTbsId           mTbsId;
    smpHandle          mPageHandle;
    knlQueueInfo     * mFreeQueueInfo;
    smlPid             mCurExtentPid;
    smlPid             mNextExtentPid;
    stlInt32           mPageSeq;
    stlInt32           mBuildExtentPageCount;
    smpRowItemIterator mRowItemIterator;
} smnmpbBuildIterator;

#define SMNMPB_COMPARE_QUICK_SORT (0)
#define SMNMPB_COMPARE_HEAP_SORT  (1)

typedef struct smnmpbCompareInfo
{
    stlChar                   * mSortBlock;
    stlInt64                    mCompareCount;
    stlInt32                    mKeyColCount;
    stlUInt8                  * mKeyColFlags;
    stlInt8                   * mKeyColOrder;
    stlInt64                    mUniqueViolated;
    dtlPhysicalCompareFuncPtr   mPhysicalCompare[SML_MAX_INDEX_KEY_COL_COUNT];
} smnmpbCompareInfo;

typedef struct smnmpbSortBlockHdr
{
    stlInt64   mKeyCount;
    stlInt64   mLowWaterMark;
    stlInt64   mHighWaterMark;
    stlInt64   mTotalSize;
    stlInt64   mActiveSlotIdx;
} smnmpbSortBlockHdr;

typedef struct smnmpbSortIterator
{
    smnmpbSortBlockHdr * mSortBlockHdr;
    stlChar            * mSortBlock;
} smnmpbSortIterator;
    
typedef struct smnmpbMergeRunIterator
{
    smlTbsId           mTbsId;
    smpHandle          mPageHandle;
    smlPid             mFirstExtentPid;
    smlPid             mCurExtentPid;
    smlPid             mPrevExtentPid;
    stlInt32           mPageSeq;
    stlInt32           mBuildExtentPageCount;
} smnmpbMergeRunIterator;

typedef struct smnmpbTreeIterator
{
    smxlTransId         mTransId;
    smlTbsId            mTbsId;
    smxmTrans         * mMiniTrans;
    stlInt32            mMiniTransState;
    stlInt32            mMiniTransAttr;
    void              * mIndexHandle;
    stlInt16            mHeight;
    smlPid              mExtentPid;
    stlInt32            mPageSeq;
    stlInt32            mBuildExtentPageCount;
    stlInt32            mKeyColCount;
    stlUInt8          * mKeyColFlags;
    stlUInt8          * mKeyColTypes;
    knlQueueInfo      * mFreeQueueInfo;
    smnmpbCompareInfo * mCompareInfo;
    smpHandle           mLeafPageHandle;
    stlChar             mPrevKey[SMN_MAX_KEY_BODY_SIZE + SMNMPB_MAX_LEAF_KEY_HDR_LEN];
    smlPid              mPageId[SMNMPB_MAX_INDEX_DEPTH+1];
    smlPid              mFirstPageId[SMNMPB_MAX_INDEX_DEPTH+1];
} smnmpbTreeIterator;

typedef struct smnmpbBbuildPageHdr
{
    smlPid     mNextExtentPid;
    stlChar    mPadding[4];
} smnmpbBbuildPageHdr;

#define SMNMPB_SET_NEXT_EXTENT( aPageHandle, aExtentPid )                                                  \
{                                                                                                          \
    ((smnmpbBbuildPageHdr*)SMP_GET_LOGICAL_HEADER(SMP_FRAME(aPageHandle)))->mNextExtentPid = (aExtentPid); \
}
#define SMNMPB_GET_NEXT_EXTENT( aPageHandle )                                                   \
    ( ((smnmpbBbuildPageHdr*)SMP_GET_LOGICAL_HEADER(SMP_FRAME(aPageHandle)))->mNextExtentPid )


/* for D$MEMORY_BTREE_KEY */

typedef struct smnmpbFxKey
{
    smlPid      mCurPageId;
    stlUInt16   mLevel;
    smlPid      mParentPageId;
    stlInt16    mSeq;
    stlInt16    mKeyStatusFlags;
    smlPid      mChildPageId;
    smlPid      mRowPageId;
    stlInt16    mRowOffset;
    stlInt16    mInsertTcn;
    stlInt16    mInsertRtsSeq;
    stlInt16    mInsertRtsVerNo;
    smxlTransId mInsertTransId;
    smlScn      mInsertScn;
    stlInt16    mDeleteTcn;
    stlInt16    mDeleteRtsSeq;
    stlInt16    mDeleteRtsVerNo;
    smxlTransId mDeleteTransId;
    smlScn      mDeleteScn;
    stlInt16    mColumnSeq;
    stlInt64    mColumn[SMP_PAGE_SIZE/STL_SIZEOF(stlInt64)];
} smnmpbFxKey;


typedef struct smnmpbFxKeyIterator
{
    smlTbsId          mTbsId;
    smnmpbPathStack   mPathStack;
    stlBool           mPrinted[SMNMPB_MAX_INDEX_DEPTH];
    stlUInt16         mLevel;
    stlUInt16         mTotalKeyCount;
    stlUInt16         mNextColumnIdx;
    stlUInt16         mTotalColumnCount;
    stlInt64          mPageBuf[SMP_PAGE_SIZE/STL_SIZEOF(stlInt64)];
    smnmpbFxKey       mKeyRecord;
} smnmpbFxKeyIterator;

/* for D$MEMORY_BTREE_HEADER */

typedef struct smnmpbFxHdr
{
    smlPid       mCurPageId;
    smlPid       mParentPageId;
    stlInt16     mMinRtsCount;
    stlInt16     mMaxRtsCount;
    stlInt16     mCurRtsCount;
    stlInt16     mHighWaterMark;
    stlInt16     mLowWaterMark;
    stlInt16     mTotalKeyCount;
    smlPid       mPrevPageId;
    smlPid       mNextPageId;
    smlPid       mNextFreePageId;
    stlUInt16    mLevel;
    stlUInt16    mActiveKeyCount;
    stlUInt16    mKeyColCount;
    stlUInt64    mSmoNo;
    stlUInt16    mFlags;
} smnmpbFxHdr;


typedef struct smnmpbFxHdrIterator
{
    smlTbsId          mTbsId;
    smnmpbPathStack   mPathStack;
    stlBool           mPrinted[SMNMPB_MAX_INDEX_DEPTH];
    stlUInt16         mLevel;
    stlUInt16         mTotalKeyCount;
    stlInt64          mPageBuf[SMP_PAGE_SIZE/STL_SIZEOF(stlInt64)];
    smnmpbFxHdr       mHdrRecord;
} smnmpbFxHdrIterator;

typedef struct smnmpbPendArg
{
    void       * mRelationHandle;
    smxlTransId  mTransId;
} smnmpbPendArg;

typedef struct smnmpbStatistics
{
    stlInt64 mSplitCount;
    stlInt64 mAgingCount;
    stlInt64 mCompactCount;
    stlInt64 mLinkTraverseCount;
    stlInt64 mSortElapsedTime;
    stlInt64 mMergeElapsedTime;
    stlInt64 mBuildElapsedTime;
} smnmpbStatistics;

typedef struct smnmpbFxStatistics
{
    stlInt64 mSegmentId;
    stlInt64 mSplitCount;
    stlInt64 mAgingCount;
    stlInt64 mCompactCount;
    stlInt64 mLinkTraverseCount;
    stlInt64 mEmptyNodeCount;
    stlInt64 mSortElapsedTime;
    stlInt64 mMergeElapsedTime;
    stlInt64 mBuildElapsedTime;
} smnmpbFxStatistics;

typedef struct smnmpbFxStatisticsIterator
{
    void    * mSegIterator;
    stlBool   mFetchStarted;
} smnmpbFxStatisticsIterator;

typedef struct smnmpbFxVerifyOrder
{
    stlInt64  mPhysicalId;
    stlBool   mResult;
} smnmpbFxVerifyOrder;

typedef struct smnmpbFxVerifyOrderItem
{
    smlPid          mPageId;
    stlInt64        mPageBuf[SMP_PAGE_SIZE/STL_SIZEOF(stlInt64)];
    stlChar         mPrevKeyBuf[SMN_MAX_KEY_BODY_SIZE + SMNMPB_MAX_LEAF_KEY_HDR_LEN];
    stlChar       * mPrevKey;
} smnmpbFxVerifyOrderItem;

typedef struct smnmpbFxVerifyOrderIterator
{
    smnmpbCompareInfo       mCompareInfo;
    stlInt8                 mKeyColOrder[SML_MAX_INDEX_KEY_COL_COUNT];
    smlTbsId                mTbsId;
    smnmpbFxVerifyOrderItem mStack[SMNMPB_MAX_INDEX_DEPTH];
    stlBool                 mWorkDone;
} smnmpbFxVerifyOrderIterator;

typedef struct smnmpbFxVerifyStructure
{
    stlInt64  mPhysicalId;
    stlBool   mResult;
} smnmpbFxVerifyStructure;

typedef struct smnmpbFxVerifyStructureItem
{
    smlPid   mPrevC;     /**< 이전 Child Page Id */
    smlPid   mPrevCn;    /**< 이전 Child Page의 Next Page */
    stlInt64 mPageBuf[SMP_PAGE_SIZE/STL_SIZEOF(stlInt64)];
} smnmpbFxVerifyStructureItem;

typedef struct smnmpbFxVerifyStructureIterator
{
    smlTbsId                    mTbsId;
    smnmpbFxVerifyStructureItem mStack[SMNMPB_MAX_INDEX_DEPTH];
    stlBool                     mWorkDone;
} smnmpbFxVerifyStructureIterator;


#define SMNMPB_INIT_STATISTICS( aStatistics )          \
{                                                      \
    (aStatistics)->mSplitCount = 0;                    \
    (aStatistics)->mAgingCount = 0;                    \
    (aStatistics)->mCompactCount = 0;                  \
    (aStatistics)->mLinkTraverseCount = 0;             \
    (aStatistics)->mSortElapsedTime = 0;               \
    (aStatistics)->mMergeElapsedTime = 0;              \
    (aStatistics)->mBuildElapsedTime = 0;              \
}

#define SMNMPB_INC_STATISTICS( aStatistics, aVariable )      \
{                                                            \
    (aStatistics)->m ## aVariable += 1;                      \
}

#define SMNMPB_GET_MODULE_SPECIFIC_DATA(relHandle)                      \
    (smnmpbStatistics*)(((smnIndexHeader*)(relHandle))->mModuleSpecificData)

#define SMNMPB_GET_PCTFREE(aIndexHandle)                      \
    (((smnIndexHeader*)(aIndexHandle))->mPctFree)

/** @} */
    
#endif /* _SMNMPBDEF_H_ */
