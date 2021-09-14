/*******************************************************************************
 * smtDef.h
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


#ifndef _SMTDEF_H_
#define _SMTDEF_H_ 1

/**
 * @file smtDef.h
 * @brief Storage Manager Layer Extent Manager Internal Definition
 */

/**
 * @defgroup smtInternal Component Internal Routines
 * @ingroup smt
 * @{
 */

/**
 * @addtogroup smrUndoLogType
 * @{
 */
#define SMT_UNDO_LOG_MEMORY_UPDATE_EXT_BLOCK_MAP_HDR    0
/** @} */

#define SMT_UNDO_LOG_MAX_COUNT             1

typedef struct smtExtMapHdr
{
    stlUInt16 mExtCount;
    stlUInt16 mMaxExtCount;
    stlUInt16 mOnExtCount;
    stlUInt16 mOffExtCount;
} smtExtMapHdr;

/**
 * 한개의 Extent Block은 현재 계산상 2G까지 처리할수 있지만,
 * Scalability 성능을 고려하여 64M의 데이터 페이지를 관리할수 있도록 제한한다.
 */
#define SMT_MAX_EXT_BIT_COUNT( aPageCountInExt )   \
(                                                  \
    ( ( 8 * 1024 ) / aPageCountInExt )             \
)

#define SMT_EXT_BIT_ARRAY( smtExtMapHdr )                   \
    ( ((stlChar*)smtExtMapHdr) + STL_SIZEOF(smtExtMapHdr) )
    
#define SMT_EXT_IS_FREE( aPage )                                          \
    ( ((stlChar*)aPage) + STL_SIZEOF(smpPhyHdr) + STL_SIZEOF(smtExtMapHdr) )

#define SMT_EBMS_MASK          0x0000000F
#define SMT_EBMS_FREE          0x00000000
#define SMT_EBMS_USED          0x00000001
#define SMT_EBMS_DROP          0x00000002

#define SMT_EBS_MASK          0x0000000F
#define SMT_EBS_UNALLOCATED   0x00000000
#define SMT_EBS_FREE          0x00000001
#define SMT_EBS_FULL          0x00000002
#define SMT_EBS_PREPARED      0x00000010

typedef struct smtExtBlockDesc
{
    smlPid     mExtBlock;
    stlUInt32  mState;
} smtExtBlockDesc;

typedef struct smtExtBlockMapHdr
{
    stlUInt32 mExtBlockDescCount;
    stlUInt32 mState;
} smtExtBlockMapHdr;
    
#define SMT_MAX_EXT_BLOCK_DESC_COUNT            \
(                                               \
    ( SMP_PAGE_SIZE -                           \
      STL_SIZEOF( smpPhyHdr ) -                 \
      STL_SIZEOF( smpPhyTail ) -                \
      STL_SIZEOF( smtExtBlockMapHdr ) ) /       \
    STL_SIZEOF( smtExtBlockDesc )               \
)

typedef struct smtExtBlockMap
{
    smtExtBlockDesc mExtBlockDesc[SMT_MAX_EXT_BLOCK_DESC_COUNT];
} smtExtBlockMap;

#define SMT_EXT_BLOCK_MAP_PAGE_SEQ 0

#define SMT_OFFSET_MASK STL_INT64_C(0x000000000000FFFF)

#define SMT_MAKE_EXTENT_ID( aPageId, aOffset ) \
    ( ((stlUInt64)aPageId << 16) + aOffset )

#define SMT_EXTENT_PID( aExtId ) ( aExtId >> 16 )

#define SMT_EXTENT_OFFSET( aExtId ) ( aExtId & SMT_OFFSET_MASK )

typedef struct smtExtBlockPendingArgs
{
    smlPid    mExtBlockMapPid;
    smlTbsId  mTbsId;
    stlUInt16 mOffset; 
} smtExtBlockPendingArgs;


typedef struct smtTbsExtPathCtrl
{
    smlRid        mExtBlockMapRid;
    smlRid        mExtMapRid;
    smlDatafileId mDatafileId;
} smtTbsExtPathCtrl;

typedef struct smtFxTbsExt
{
    smlTbsId       mTbsId;
    stlChar        mState[2];
    smlDatafileId  mDatafileId;
    stlUInt32      mPageSeqId;
    smlPid         mDataPid;
} smtFxTbsExt;

typedef struct smtTbsExtBlockDescPathCtrl
{
    smlRid        mExtBlockMapRid;
    smlRid        mExtMapRid;
    smlDatafileId mDatafileId;
} smtTbsExtBlockDescPathCtrl;

typedef struct smtFxTbsExtBlockDesc
{
    smlTbsId       mTbsId;
    stlChar        mState[17];
    smlDatafileId  mDatafileId;
    stlUInt32      mSeqId;
    smlPid         mExtBlockPid;
    stlInt32       mPrepared;
} smtFxTbsExtBlockDesc;

#define SMT_MAX_EXT_MAP_HINT 7

/**
 * @brief 최대 테이블스페이스 힌트 사이즈
 */
#define SMS_MAX_TBS_HINT      8
#define SMT_INVALID_TBS_HINT  0xFFFFFFFF

typedef struct smtHint
{
    stlInt32 mTbsId;
    smlPid   mHintPid;
} smtHint;

/**
 * @brief 최대 테이블스페이스 검색 캐시 사이즈
 */
typedef struct smtHintPool
{
    stlInt32 mSequence;
    smtHint  mTbsHint[SMS_MAX_TBS_HINT];
} smtHintPool;

/** @} */
    
#endif /* _SMTDEF_H_ */
