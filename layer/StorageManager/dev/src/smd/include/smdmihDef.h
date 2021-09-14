/*******************************************************************************
 * smdmihDef.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smdmihDef.h 7313 2013-02-19 12:00:28Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _SMDMIHDEF_H_
#define _SMDMIHDEF_H_ 1

#include <stlDef.h>
#include <smlDef.h>

/**
 * @file smdmihDef.h
 * @brief Storage Manager Layer Memory Instant Hash Table Internal Definition
 */

/**
 * @defgroup smdmihInternal Component Internal Routines
 * @ingroup smn
 * @{
 */

typedef struct smdmihDirSlot
{
    knlLogicalAddr mKeyLa;
    stlUInt32      mCollisionCount;
    stlUInt32      mHashKey;
} smdmihDirSlot;

typedef struct smdmihColumnInfo
{
    stlInt32  mColOrder;
    stlInt32  mColLenSize;
} smdmihColumnInfo;

typedef struct smdmihTableHeader
{
    smeRelationHeader   mRelHeader;           /**< 모든 테이블 해더의 공통 해더 */
    smdmifTableHeader   mBaseHeader;
    
    smdmihDirSlot     * mDirectory;
    stlInt32            mBucketModulusIdx;
    stlInt32            mBucketCount;
    stlUInt64           mModularConstant;
    stlUInt64           mModularShiftBit;
    stlUInt64           mAdjustConstant;
    knlLogicalAddr      mDataNodeLa;
    knlLogicalAddr      mFreeDataBlockLa;
    stlInt64            mBlockSize;
    stlInt64            mUsedDataBlockSize;
    stlInt64            mDataNodeSize;
    stlInt32            mStartBucketIdx;
    stlInt32            mEndBucketIdx;

    smdmifDefColumn   * mColumns;
    smdmihColumnInfo  * mColumnInfo;
    stlInt32            mKeyColCount;
    stlInt32            mRowColCount;
    stlInt32            mVarRowLen;
    stlInt32            mRowLen;
    stlInt64            mRowCount;

    stlBool             mUniqueness;
    stlBool             mSkipNull;
    stlBool             mIsAllColumnLen1;
    
    stlChar           * mColCombineMem1;
    stlChar           * mColCombineMem2;
} smdmihTableHeader;

typedef struct smdmihNodeHeader
{
    stlUInt32 mUsedSize;
    stlUInt32 mFreeSize;
} smdmihNodeHeader;

typedef struct smdmihCommonIterator
{
    smiIterator      mCommon;
} smdmihCommonIterator;
    
typedef struct smdmihIterator
{
    smiIterator      mCommon;
    
    smdmihDirSlot  * mDirectory;
    stlInt32         mDirIdx;
    knlLogicalAddr   mNextKeyLa;
} smdmihIterator;

#define SMDMIH_CHECK_COLLISION( aDirSlot, aHashKey ) \
    ( (((aDirSlot)->mCollisionCount > 0) && (aDirSlot->mHashKey == (aHashKey))) ? STL_FALSE : STL_TRUE ) \

#define SMDMIH_SET_COLLIDE_HASH_KEY( aDirSlot, aHashKey )   \
    {                                                       \
        if( (aDirSlot)->mCollisionCount == 0 )              \
        {                                                   \
            (aDirSlot)->mHashKey = (aHashKey);              \
        }                                                   \
        if( (aDirSlot)->mCollisionCount < STL_UINT32_MAX )  \
        {                                                   \
            (aDirSlot)->mCollisionCount += 1;               \
        }                                                   \
    }

#define SMDMIH_USABLE_BLOCK_SIZE( aTableHeader )       \
    ( (aTableHeader)->mBaseHeader.mBlockSize - STL_SIZEOF( smdmifDefBlockHeader ) )
#define SMDMIH_USABLE_DATA_BLOCK_SIZE( aTableHeader )        \
    ( (aTableHeader)->mBaseHeader.mBlockSize -               \
      STL_SIZEOF( smdmifDefBlockHeader ) -                   \
      STL_SIZEOF(smdmihNodeHeader) )

#define SMDMIH_DIR_SLOT( aHeader, aHashKey, aDirSlot, aBucketIdx )              \
    {                                                                           \
        (aBucketIdx) = KNL_HASH_BUCKET_IDX( (aHeader)->mBucketCount,            \
                                            (aHashKey),                         \
                                            (aHeader)->mModularConstant,        \
                                            (aHeader)->mModularShiftBit,        \
                                            (aHeader)->mAdjustConstant );       \
                                                                                \
        (aDirSlot) = &(aHeader)->mDirectory[(aBucketIdx)];                      \
    }

#define SMDMIH_ADJUST_SCAN_HINT( aTableHeader, aBucketIdx )                         \
    {                                                                               \
        (aTableHeader)->mStartBucketIdx = STL_MIN( (aTableHeader)->mStartBucketIdx, \
                                                   (aBucketIdx) );                  \
        (aTableHeader)->mEndBucketIdx = STL_MAX( (aTableHeader)->mEndBucketIdx,     \
                                                 (aBucketIdx) );                    \
    }

#define SMDMIH_INITIAL_MODULUS_ID    ( 5 )
#define SMDMIH_DATA_NODE_SIZE        ( 4 * 1024 )

#define SMDMIH_FETCH_BIT_SIZE     ( STL_SIZEOF(stlBool) )
#define SMDMIH_HASH_KEY_SIZE      ( STL_SIZEOF(stlInt32) )
#define SMDMIH_OVERFLOW_KEY_SIZE  ( STL_SIZEOF(knlLogicalAddr) )
#define SMDMIH_KEY_HEADER_SIZE    ( SMDMIH_HASH_KEY_SIZE +      \
                                    SMDMIH_OVERFLOW_KEY_SIZE +  \
                                    SMDMIH_FETCH_BIT_SIZE )

#define SMDMIH_HASH_KEY_OFFSET       ( 0 )
#define SMDMIH_OVERFLOW_KEY_OFFSET   ( SMDMIH_HASH_KEY_OFFSET + SMDMIH_HASH_KEY_SIZE )
#define SMDMIH_FETCH_BIT_OFFSET      ( SMDMIH_OVERFLOW_KEY_OFFSET + SMDMIH_OVERFLOW_KEY_SIZE )

#define SMDMIH_GET_HASH_KEY( aKey, aHashKey )   \
    ( STL_WRITE_INT32( &(aHashKey), (aKey) ) )
#define SMDMIH_GET_OVERFLOW_KEY( aKey, aOverflowKeyLa )                             \
    ( STL_WRITE_INT64( &(aOverflowKeyLa), ((aKey) + SMDMIH_OVERFLOW_KEY_OFFSET) ) )
#define SMDMIH_GET_FETCH_BIT( aKey )         \
    ( *((aKey) + SMDMIH_FETCH_BIT_OFFSET) )

#define SMDMIH_SET_HASH_KEY( aKey, aHashKey )   \
    ( STL_WRITE_INT32( (aKey), &(aHashKey) ) )
#define SMDMIH_SET_OVERFLOW_KEY( aKey, aOverflowKeyLa )                             \
    ( STL_WRITE_INT64( ((aKey) + SMDMIH_OVERFLOW_KEY_OFFSET), &(aOverflowKeyLa) ) )
#define SMDMIH_SET_FETCH_BIT( aKey, aFetchBit )    \
    ( *((aKey) + SMDMIH_FETCH_BIT_OFFSET) = (aFetchBit) )

#define SMDMIH_COL_LEN_SIZE( aSize ) ( (aSize <= 255) ? STL_SIZEOF(stlInt8) : STL_SIZEOF(stlInt16) )

#define SMDMIH_NODE_USED_SIZE( aNode ) ( ((smdmihNodeHeader*)(aNode))->mUsedSize )

#define SMDMIH_RESET_ITERATOR( aIterator )                  \
    {                                                       \
        (aIterator)->mNextKeyLa = KNL_LOGICAL_ADDR_NULL;    \
        (aIterator)->mDirectory = NULL;                     \
        (aIterator)->mDirIdx = -1;                          \
    }

#define SMDMIH_INIT_ITERATOR( aIterator )       \
    {                                           \
        SMDMIH_RESET_ITERATOR( aIterator );     \
    }

#define SMDMIH_INDEX_BUILT( aTableHeader )       \
    ( (aTableHeader)->mDirectory != NULL ? STL_TRUE : STL_FALSE )

#define SMDMIH_SET_VAR_COL( aKey, aColumn, aVarColLa )                      \
    ( *(knlLogicalAddr *)( (aKey) + (aColumn)->mOffset ) = (aVarColLa) )

typedef struct smdmihFxTables
{
    stlInt64      mInstTableId;
    smlTbsId      mTbsId;
    stlInt32      mKeyColCount;
    stlInt32      mRowColCount;
    stlBool       mUniqueness;
    stlBool       mSkipNull;
    stlInt32      mAllocExtCount;
    stlInt32      mRowLen;
} smdmihFxTables;
    
typedef struct smdmihFxRow
{
    stlInt64       mDirSeq;
    stlInt64       mRowSeq;
    stlInt32       mHashValue;
    stlInt32       mColumnOrder;
    stlChar        mStringValue[64];
} smdmihFxRow;

typedef struct smdmihFxDir
{
    stlInt64     mDirSeq;
    stlUInt32    mCollisionCount;
    stlUInt32    mHashKey;
} smdmihFxDir;

typedef struct smdmihFxPathCtrl
{
    smdmihDirSlot   * mDirectory;
    stlInt64          mDirIdx;
    knlLogicalAddr    mKeyLa;
    knlLogicalAddr    mNextKeyLa;
    stlInt64          mRowSeq;
    stlInt32          mColumnOrder;
} smdmihFxPathCtrl;

typedef stlStatus (*smdmihMergeRecordAggrFunc) (void               * aTableHeader,
                                                stlChar            * aRow,
                                                smlAggrFuncInfo    * aAggrFuncInfo,
                                                stlInt32             aStartIdx,
                                                stlInt32             aEndIdx,
                                                knlValueBlockList  * aReadBlock,
                                                knlValueBlockList  * aUpdateBlock,
                                                smlEnv             * aEnv );

/** @} */
    
#endif /* _SMDMIHDEF_H_ */
