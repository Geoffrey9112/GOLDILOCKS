/*******************************************************************************
 * smnmihDef.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smnmihDef.h 7313 2013-02-19 12:00:28Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _SMNMIHDEF_H_
#define _SMNMIHDEF_H_ 1

#include <stlDef.h>
#include <smlDef.h>

/**
 * @file smnmihDef.h
 * @brief Storage Manager Layer Memory Instant Hash Index Internal Definition
 */

/**
 * @defgroup smnmihInternal Component Internal Routines
 * @ingroup smn
 * @{
 */

typedef struct smnmihDirSlot
{
    knlLogicalAddr mKeyLa;
    stlInt32       mOverflowCount;
    stlUInt32      mLastHashKey;
} smnmihDirSlot;

typedef struct smnmihColumnInfo
{
    stlInt32  mColOrder;
    stlInt32  mColLenSize;
} smnmihColumnInfo;

typedef struct smnmihIndexHeader
{
    smeRelationHeader   mRelHeader;           /**< 모든 테이블 해더의 공통 해더 */
    smdmifTableHeader * mBaseHeader;
    
    smnmihDirSlot     * mDirectory;
    stlUInt64           mBucketCount;
    stlUInt64           mModularConstant;
    stlUInt64           mModularShiftBit;
    stlUInt64           mAdjustConstant;
    knlLogicalAddr      mFreeBlockLa;
    knlLogicalAddr      mDataNodeLa;
    stlInt64            mBlockSize;
    stlInt64            mUsedBlockSize;
    stlInt64            mDataNodeSize;
    stlInt64            mTotalKeyCount;

    smdmifDefColumn  ** mRowColumn;
    smnmihColumnInfo  * mColumnInfo;
    stlInt32            mKeyColCount;

    stlBool             mSkipNull;
    
    stlChar           * mColCombineMem;
} smnmihIndexHeader;

typedef struct smnmihNodeHeader
{
    stlUInt16 mUsedSize;
    stlUInt16 mFreeSize;
} smnmihNodeHeader;

#define SMNMIH_MAX_HIT_KEY_HINT_COUNT ( 1024 )

typedef struct smnmihIterator
{
    smiIterator      mCommon;
    
    knlLogicalAddr   mNextKeyLa;
    stlInt64         mFetchDirIdx;
    stlBool          mKeyFetched;
    stlUInt32        mFilterHashKey;

    stlBool          mSkipHitKey;
    stlInt64         mHitKeyCount;
    stlInt64         mUnhitKeyCount;

    knlLogicalAddr   mHitKeyLa[SMNMIH_MAX_HIT_KEY_HINT_COUNT];
} smnmihIterator;

#define SMNMIH_USABLE_BLOCK_SIZE( aTableHeader )       \
    ( (aTableHeader)->mBaseHeader->mBlockSize - STL_SIZEOF( smdmifDefBlockHeader ) )

#define SMNMIH_DATA_NODE_SIZE   ( 4 * 1024 )

#define SMNMIH_FETCH_BIT_SIZE     ( STL_SIZEOF(stlBool) )
#define SMNMIH_HASH_KEY_SIZE      ( STL_SIZEOF(stlInt32) )
#define SMNMIH_ROWID_SIZE         ( STL_SIZEOF(knlLogicalAddr) )
#define SMNMIH_OVERFLOW_KEY_SIZE  ( STL_SIZEOF(knlLogicalAddr) )
#define SMNMIH_KEY_HEADER_SIZE    ( SMNMIH_HASH_KEY_SIZE +      \
                                    SMNMIH_ROWID_SIZE +         \
                                    SMNMIH_OVERFLOW_KEY_SIZE +  \
                                    SMNMIH_FETCH_BIT_SIZE )

#define SMNMIH_HASH_KEY_OFFSET       ( 0 )
#define SMNMIH_ROWID_OFFSET          ( SMNMIH_HASH_KEY_OFFSET + SMNMIH_HASH_KEY_SIZE )
#define SMNMIH_OVERFLOW_KEY_OFFSET   ( SMNMIH_ROWID_OFFSET + SMNMIH_ROWID_SIZE )
#define SMNMIH_FETCH_BIT_OFFSET      ( SMNMIH_OVERFLOW_KEY_OFFSET + SMNMIH_OVERFLOW_KEY_SIZE )

#define SMNMIH_GET_HASH_KEY( aKey, aHashKey )   \
    ( STL_WRITE_INT32( &(aHashKey), (aKey) ) )
#define SMNMIH_GET_ROWID( aKey, aRowLa )                                \
    ( STL_WRITE_INT64( &(aRowLa), ((aKey) + SMNMIH_ROWID_OFFSET) ) )
#define SMNMIH_GET_OVERFLOW_KEY( aKey, aOverflowKeyLa )                             \
    ( STL_WRITE_INT64( &(aOverflowKeyLa), ((aKey) + SMNMIH_OVERFLOW_KEY_OFFSET) ) )
#define SMNMIH_GET_FETCH_BIT( aKey )            \
    ( *((aKey) + SMNMIH_FETCH_BIT_OFFSET) )

#define SMNMIH_SET_HASH_KEY( aKey, aHashKey )   \
    ( STL_WRITE_INT32( (aKey), &(aHashKey) ) )
#define SMNMIH_SET_ROWID( aKey, aRowLa )                                \
    ( STL_WRITE_INT64( ((aKey) + SMNMIH_ROWID_OFFSET), &(aRowLa) ) )
#define SMNMIH_SET_OVERFLOW_KEY( aKey, aOverflowKeyLa )                             \
    ( STL_WRITE_INT64( ((aKey) + SMNMIH_OVERFLOW_KEY_OFFSET), &(aOverflowKeyLa) ) )
#define SMNMIH_SET_FETCH_BIT( aKey, aFetchBit )    \
    ( *((aKey) + SMNMIH_FETCH_BIT_OFFSET) = (aFetchBit) )

#define SMNMIH_COL_LEN_SIZE( aSize ) ( STL_SIZEOF(stlInt16) )

#define SMNMIH_NODE_USED_SIZE( aNode ) ( ((smnmihNodeHeader*)(aNode))->mUsedSize )

#define SMNMIH_RESET_ITERATOR( aIterator )                  \
    {                                                       \
        (aIterator)->mNextKeyLa = KNL_LOGICAL_ADDR_NULL;    \
        (aIterator)->mKeyFetched = STL_FALSE;               \
        (aIterator)->mFilterHashKey = 0;                    \
        (aIterator)->mFetchDirIdx = -1;                     \
        (aIterator)->mHitKeyCount = 0;                      \
        (aIterator)->mUnhitKeyCount = STL_INT64_MAX;        \
    }

#define SMNMIH_INIT_ITERATOR( aIterator )       \
    {                                           \
        SMNMIH_RESET_ITERATOR( aIterator );     \
    }

#define SMNMIH_SET_VAR_COL( aKey, aColumn, aVarColLa )                      \
    ( *(knlLogicalAddr *)( (aKey) + (aColumn)->mOffset ) = (aVarColLa) )

#define SMNMIH_DIR_SLOT( aHeader, aHashKey, aDirSlot, aBucketIdx )              \
    {                                                                           \
        (aBucketIdx) = KNL_HASH_BUCKET_IDX( (aHeader)->mBucketCount,            \
                                            (aHashKey),                         \
                                            (aHeader)->mModularConstant,        \
                                            (aHeader)->mModularShiftBit,        \
                                            (aHeader)->mAdjustConstant );       \
                                                                                \
        (aDirSlot) = &(aHeader)->mDirectory[(aBucketIdx)];                      \
    }

/** @} */
    
#endif /* _SMNMIHDEF_H_ */
