/*******************************************************************************
 * smdmisTable.h
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smdmisTable.h 6320 2012-11-13 05:05:12Z egon $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file smdmisTable.h
 * @brief Storage Manager Layer Memory Instant Sort Index Table Routines
 */

/**
 * @defgroup smdmisTable Memory Instant Sort Index Table
 * @ingroup smdInternal
 * @{
 */

#ifndef _SMDMIS_TABLE_H_
#define _SMDMIS_TABLE_H_ 1

#include <smdmifTable.h>
#include <smdmifFixedPart.h>
#include <smdmifVarPart.h>


/* 각종 상수들 정의 */
#define SMDMIS_RADIX_RANGE                    0xFF

#define SMDMIS_INDEX_DEPTH_MAX                32

#define SMDMIS_MERGE_RUN_SIZE                 STL_SIZEOF(smdmisMergeRun)

#define SMDMIS_KEY_ROW_HEADER_SIZE            STL_SIZEOF(stlChar)

#define SMDMIS_KEY_ROW_HEADER_DUP_MASK        0x01
#define SMDMIS_KEY_ROW_HEADER_DUP_YES         0x01
#define SMDMIS_KEY_ROW_HEADER_DUP_NO          0x00

#define SMDMIS_MERGE_LEVEL_MAX                32

#define SMDMIS_KEY_COMPLEMENT_2BYTE           0x8000
#define SMDMIS_KEY_COMPLEMENT_4BYTE           0x80000000
#define SMDMIS_KEY_COMPLEMENT_8BYTE           0x8000000000000000L

#define SMDMIS_NULL_VALUE                     1
#define SMDMIS_NOT_NULL_VALUE                 0
#define SMDMIS_NULL_VALUE_NULLS_FIRST         SMDMIS_NOT_NULL_VALUE
#define SMDMIS_NOT_NULL_VALUE_NULLS_FIRST     SMDMIS_NULL_VALUE
#define SMDMIS_NUMERIC_NULL_VALUE             0xFF
#define SMDMIS_NUMERIC_NULL_VALUE_NULLS_FIRST 0x00

#define SMDMIS_NUMERIC_MINUS_PAD              0xFF
#define SMDMIS_NUMERIC_MINUS_SIGN             0x80

#define SMDMIS_SORT_METHOD_AUTO               0
#define SMDMIS_SORT_METHOD_MSD_RADIX          1
#define SMDMIS_SORT_METHOD_LSD_RADIX          2
#define SMDMIS_SORT_METHOD_KEY_COMP           3

#define SMDMIS_SWAP(a, b, c)                  (c) = (a); (a) = (b); (b) = (c)
#define SMDMIS_QSORT_STACK_SIZE               32
#define SMDMIS_QSORT_CUTOFF                   32

typedef stlUInt16 smdmisSortItem;
typedef stlChar*  smdmisKeyRow;
typedef stlChar*  smdmisKeyCol;
typedef stlUInt16 smdmisKeyColLength;

typedef struct smdmisTableHeader smdmisTableHeader;
typedef struct smdmisKeyColumn   smdmisKeyColumn;

typedef dtlCompareResult (*smdmisCompareKeyFunc)( smdmisTableHeader *aHeader,
                                                  stlChar           *aLeftRow,
                                                  stlChar           *aRightRow );

typedef stlUInt8 (*smdmisGetRowRadixFunc)( smdmifFixedRow aRow,
                                           stlUInt16      aColValOffset,
                                           stlUInt16      aColLenOffset );

typedef void (*smdmisModifyKeyValFunc)( smdmisKeyRow       aKeyRow,
                                        stlInt64           aColLen,
                                        smdmifDefColumn   *aKeyCol );

typedef void (*smdmisRecoverKeyValFunc)( smdmisKeyRow       aKeyRow,
                                         smdmifDefColumn   *aKeyCol );

typedef struct smdmisIndexNode
{
    knlLogicalAddr     mMyAddr;
    knlLogicalAddr     mNext;
    knlLogicalAddr     mLeftMost;
    stlUInt16          mLevel;           /**< leaf's level is 0 */

    /* smdmisRadixNode와 공통 맴버들 */
    stlUInt16          mFirstKeyOffset;
    stlUInt16          mKeyCount;
    stlUInt16          mMaxKeyCount;
    smdmisSortItem     mSortItems[1];
} smdmisIndexNode;

struct smdmisKeyColumn
{
    stlInt32                   mColOrder;
    stlBool                    mAscending;
    stlBool                    mNotNull;
    stlBool                    mNullOrderLast;
    dtlPhysicalCompareFuncPtr  mPhysicalCompare;
    smdmisModifyKeyValFunc     mModifyKeyVal;
    smdmisRecoverKeyValFunc    mRecoverKeyVal;
};

typedef struct smdmisMergeRun smdmisMergeRun;
typedef struct smdmisLeafRun smdmisLeafRun;
typedef struct smdmisRunStackList smdmisRunStackList;

struct smdmisMergeRun
{
    smdmisMergeRun  *mParent;
    smdmisLeafRun   *mLess;      /**< min(mLeft->mLess, mRight-mLess) */
    smdmisMergeRun  *mLeft;
    smdmisMergeRun  *mRight;
};

struct smdmisLeafRun
{
    smdmisMergeRun  *mParent;
    smdmisLeafRun   *mLess;      /**< leaf 들은 자기 자신을 가리킨다. */
    smdmisIndexNode *mNode;
    stlUInt16        mIndex;     /**< merge 과정에서 현재 key를 가리킨다. */
    stlInt16         mPad[3];    /**< smdmisMergeRun과 크기를 맞추기 위해 */
};

struct smdmisRunStackList
{
    smdmisRunStackList *mNext;
    stlInt32            mItemCount;
    stlInt32            mMaxItemCount;
    smdmisIndexNode    *mItems[1];
};

typedef struct smdmisMergeEnv
{
    smdmisMergeRun     *mRoot;
    smdmisLeafRun      *mLeafRunList;
    smdmisMergeRun     *mMergeRunList;
    smdmisIndexNode    *mCachedLastNode;  /* 현재 merge env에 달린 node들 중 가장 큰 키값. 성능 최적화를 위해 cache함 */
    stlInt32            mLeafCount;
    stlInt32            mMergeCount;
    stlInt32            mCurLeafIndex;

    stlBool             mVolatileMode;
    stlInt32            mLevelCount[SMDMIS_MERGE_LEVEL_MAX];
    stlInt32            mLevel;

    smdmisRunStackList *mSortedList;
} smdmisMergeEnv;

struct smdmisTableHeader
{
    smdmifTableHeader      mBaseHeader;
    smlSortTableAttr       mAttr;

    stlUInt16              mKeyColCount;
    stlUInt16              mKeyRowLen;
    smdmisKeyColumn       *mKeyColInfo;
    smdmifDefColumn       *mKeyCols;           /**< Sort table이 별도로 값을 세팅한다. 최초 insert시 세팅해야 한다. */

    knlLogicalAddr         mRootNode;
    knlLogicalAddr         mFirstLeafNode;
    knlLogicalAddr         mUnorderedLeafList; /**< leaf's list for Volatile sort table */
    knlLogicalAddr         mFreeNode;

    knlLogicalAddr         mCurNodeBlock;
    stlInt32               mNodeSize;
    stlInt32               mCurBlockFreeSize;

    stlChar               *mColCombineMem1;
    stlChar               *mColCombineMem2;

    stlUInt16             *mRadixArray;
    stlUInt16              mRadixIndexFence;

    /* compare 최적화를 위한 맴버들 */
    smdmisCompareKeyFunc   mCompareKey;
    stlInt32               mFirstKeyOffset;
};

typedef struct smdmisIndexPath
{
    smdmisIndexNode *mParentNode;
    stlInt32         mPosition;
} smdmisIndexPath;

typedef struct smdmisRadixNode smdmisRadixNode;

struct smdmisRadixNode
{
    knlLogicalAddr     mMyAddr;
    smdmisRadixNode   *mNextOverflow;
    smdmisRadixNode   *mNextGreater;
    stlUInt16          mUsedRadixIndex;

    /* smdmisIndexNode와 공통 맴버들 */
    stlUInt16          mFirstKeyOffset;
    stlUInt16          mKeyCount;
    stlUInt16          mMaxKeyCount;
    smdmisSortItem     mSortItems[1];
};

typedef struct smdmisRadixBucket
{
    smdmisRadixNode *mFirst;
    smdmisRadixNode *mLast;
} smdmisRadixBucket;

typedef struct smdmisIndexBucket
{
    smdmisIndexNode *mFirst;
    smdmisIndexNode *mLast;
} smdmisIndexBucket;

/**
 * smdmisTableHeader macros
 */
#define SMDMIS_TABLE_CUR_NODE_BLOCK(header)    \
    ( (stlChar *)knlToPhysicalAddr((header)->mCurNodeBlock) )

#define SMDMIS_TABLE_INDEX_BUILT(header)       \
    ( (header)->mFirstLeafNode != KNL_LOGICAL_ADDR_NULL ? STL_TRUE : STL_FALSE )

#define SMDMIS_TABLE_IS_VOLATILE(header)             \
    ( (((header)->mAttr.mTopDown == STL_FALSE) &&    \
      ((header)->mAttr.mVolatile == STL_TRUE)) ?     \
      STL_TRUE : STL_FALSE )

#define SMDMIS_TABLE_USABLE_BLOCK_SIZE(header)       \
    ( (header)->mBaseHeader.mBlockSize - STL_SIZEOF( smdmifDefBlockHeader ) )

#define SMDMIS_TABLE_IS_RADIX_SORTED(header)         \
    ( (header)->mRadixArray != NULL ? STL_TRUE : STL_FALSE )


/**
 * smdmisIndexNode, smdmisRadixNode 공통 macro들
 */
#define SMDMIS_NODE_CLEAR(node)        \
    ( (node)->mKeyCount = 0 )

#define SMDMIS_NODE_IS_FULL(node)      \
    ( (node)->mKeyCount >= (node)->mMaxKeyCount ? STL_TRUE : STL_FALSE )

#define SMDMIS_NODE_CUR_KEY_OFFSET(node, keyRowLen)         \
    ( (node)->mFirstKeyOffset + (keyRowLen) * (node)->mKeyCount )

#define SMDMIS_NODE_ALLOC_KEY_AT(node, index, keyRow, keyRowLen)               \
    (node)->mSortItems[(index)] = SMDMIS_NODE_CUR_KEY_OFFSET(node, keyRowLen); \
    (keyRow) = (stlChar *)(node) + (node)->mSortItems[(index)];                \
    (node)->mKeyCount++

#define SMDMIS_NODE_ALLOC_KEY(node, keyRow, keyRowLen)     \
    SMDMIS_NODE_ALLOC_KEY_AT(node, (node)->mKeyCount, keyRow, keyRowLen)

#define SMDMIS_NODE_APPEND_KEY(node, keyRow, keyRowLen)                                         \
    (node)->mSortItems[(node)->mKeyCount] = SMDMIS_NODE_CUR_KEY_OFFSET(node, keyRowLen);        \
    stlMemcpy( (stlChar *)(node) + (node)->mSortItems[(node)->mKeyCount++], keyRow, keyRowLen )

#define SMDMIS_NODE_GET_KEY(node, index)               \
    ( (stlChar *)(node) + (node)->mSortItems[(index)] )

/**
 * smdmisIndexNode macros
 */
#define SMDMIS_INDEX_NODE_LEFTMOST_NODE(node)     \
    ( SMDMIS_TO_NODE_PTR( (node)->mLeftMost ) )

#define SMDMIS_INDEX_NODE_SET_LEFTMOST(node, child)  \
    ( (node)->mLeftMost = (child)->mMyAddr )

/* left는 온전한 리스트의 맴버이고, right는 단독 노드라 가정한다. */
#define SMDMIS_INDEX_NODE_ADD_TO_LIST(list, node)                       \
    {                                                                   \
        (node)->mNext   = (list)->mNext;                                \
        (list)->mNext   = (node)->mMyAddr;                              \
    }

#define SMDMIS_INDEX_NODE_GET_NEXT(node)     \
    ( (smdmisIndexNode *)knlToPhysicalAddr( (node)->mNext ) )

#define SMDMIS_INDEX_NODE_REMOVE_FIRST(nodeList, firstRet)     \
    {                                                          \
        STL_DASSERT( (nodeList) != NULL );                     \
        (firstRet) = (nodeList);                               \
        (nodeList) = SMDMIS_INDEX_NODE_GET_NEXT(nodeList);     \
        (firstRet)->mNext = KNL_LOGICAL_ADDR_NULL;             \
    }


/**
 * smdmisRadixNode macros
 */
#define SMDMIS_RADIX_NODE_INPLACE_SORTABLE(node)     \
    ( (node)->mNextOverflow == NULL ? STL_TRUE : STL_FALSE )

#define SMDMIS_RADIX_NODE_SORT_DONE(node, radixIndexFence)  \
    ( ((node)->mUsedRadixIndex == radixIndexFence - 1) ? STL_TRUE : STL_FALSE )

#define SMDMIS_RADIX_NODE_GET_NEXT_KEY(header, node, tempNode, index, key)          \
    while( (node) != NULL )                                                         \
    {                                                                               \
        if( (index) >= (node)->mKeyCount )                                          \
        {                                                                           \
            (tempNode) = (node)->mNextOverflow;                                     \
            smdmisFreeRadixNode( (header), (node) );                                \
            (node) = (tempNode);                                                    \
            (index) = 0;                                                            \
            continue;                                                               \
        }                                                                           \
        (key) = (stlChar *)(node) + (node)->mSortItems[(index)];                    \
        (index)++;                                                                  \
        break;                                                                      \
    }                                                                               \
    if( (node) == NULL )                                                            \
    {                                                                               \
        (key) = NULL;                                                               \
    }

/**
 * smdmisMergeRun macros
 */
#define SMDMIS_MERGE_RUN_SET_LEFT(parent, left)    \
    (parent)->mLeft = (left);                      \
    (left)->mParent = (parent)

#define SMDMIS_MERGE_RUN_SET_RIGHT(parent, right)  \
    (parent)->mRight = (right);                    \
    (right)->mParent = (parent)

#define SMDMIS_MERGE_RUN_INIT(run)                 \
    (run)->mParent = NULL;                         \
    (run)->mLeft   = NULL;                         \
    (run)->mRight  = NULL;                         \
    (run)->mLess   = NULL



/**
 * smdmisLeafRun macros
 */
#define SMDMIS_LEAF_RUN_NEXT(run)      \
    ( (run)->mIndex++ )

#define SMDMIS_LEAF_RUN_RESET(run)     \
    ( (run)->mIndex = 0 )

#define SMDMIS_LEAF_RUN_INIT(run)      \
    (run)->mParent = NULL;             \
    (run)->mNode   = NULL;             \
    (run)->mLess   = (run);            \
    SMDMIS_LEAF_RUN_RESET(run)

#define SMDMIS_LEAF_RUN_IS_END(run)    \
    ( ((run)->mNode == NULL) || ((run)->mIndex >= (run)->mNode->mKeyCount) ? STL_TRUE : STL_FALSE )

#define SMDMIS_LEAF_RUN_CUR_KEY(run)   \
    ( SMDMIS_NODE_GET_KEY( (run)->mNode, (run)->mIndex ) )



/**
 * smdmisMergeEnv macros
 */
#define SMDMIS_MERGE_ENV_ADD_NODE(env, node)                     \
    STL_DASSERT( (env)->mCurLeafIndex < (env)->mLeafCount );     \
    (env)->mLeafRunList[(env)->mCurLeafIndex].mNode = (node);      \
    (env)->mCurLeafIndex++

#define SMDMIS_MERGE_ENV_IS_FULL(env)      \
    ( (env)->mCurLeafIndex >= (env)->mLeafCount ? STL_TRUE : STL_FALSE )

#define SMDMIS_MERGE_ENV_CLEAR(env)        \
    (env)->mCurLeafIndex = 0;              \
    (env)->mCachedLastNode = NULL 

#define SMDMIS_MERGE_ENV_IS_EMPTY(env)     \
    ( (env)->mCurLeafIndex == 0 ? STL_TRUE : STL_FALSE )

#define SMDMIS_MERGE_ENV_HAS_ONLY_ONE_SORTED_RUN(env) \
    ( (env)->mCurLeafIndex == 1 ? STL_TRUE : STL_FALSE )

#define SMDMIS_MERGE_ENV_FIRST_NODE(env)   \
    ( (env)->mLeafRunList[0].mNode )

#define SMDMIS_MERGE_ENV_GET_LAST_NODE(env)     \
    ( (env)->mCachedLastNode )

#define SMDMIS_MERGE_ENV_SET_LAST_NODE(env, node) \
    ( (env)->mCachedLastNode = (node) )

/**
 * smdmisRunStackList macros
 */
#define SMDMIS_RUN_STACK_LIST_IS_FULL(sortedList) \
    ( (sortedList)->mItemCount >= (sortedList)->mMaxItemCount ? STL_TRUE : STL_FALSE )

#define SMDMIS_RUN_STACK_LIST_CLEAR(sortedList)   \
    ( (sortedList)->mItemCount = 0)

#define SMDMIS_RUN_STACK_LIST_ADD_RUN(sortedList, node)                         \
    STL_DASSERT( SMDMIS_RUN_STACK_LIST_IS_FULL(sortedList) == STL_FALSE );      \
    (sortedList)->mItems[(sortedList)->mItemCount] = (node);                    \
    (sortedList)->mItemCount++

#define SMDMIS_RUN_STACK_LIST_INIT(sortedList, itemCount)    \
    (sortedList)->mNext = NULL;                              \
    (sortedList)->mItemCount = 0;                            \
    (sortedList)->mMaxItemCount = (itemCount)




#define SMDMIS_KEY_COL_OFFSET_VALUE(col)     \
    ( (col)->mOffset + STL_SIZEOF(smdmisKeyColLength) )

#define SMDMIS_KEY_COL_OFFSET_NULL_VALUE(col)  \
    ( (col)->mOffset - 1 )



/**
 * smdmisKeyRow macros
 */
#define SMDMIS_KEY_ROW_GET_FIXED_ROW_ADDR_PTR(keyRow) \
    ( (knlLogicalAddr *)((keyRow) + SMDMIS_KEY_ROW_HEADER_SIZE) )

#define SMDMIS_KEY_ROW_GET_FIXED_ROW_ADDR(keyRow) \
    ( *(knlLogicalAddr *)((keyRow) + SMDMIS_KEY_ROW_HEADER_SIZE) )

#define SMDMIS_KEY_ROW_GET_FIXED_ROW(keyRow)     \
    ( (smdmifFixedRow)knlToPhysicalAddr( SMDMIS_KEY_ROW_GET_FIXED_ROW_ADDR(keyRow) ) )

#define SMDMIS_KEY_ROW_WRITE_CHILD_ADDR(keyRow, fixedRowAddr) \
    ( *(knlLogicalAddr *)((keyRow) + SMDMIS_KEY_ROW_HEADER_SIZE) = (fixedRowAddr) )

#define SMDMIS_KEY_ROW_GET_CHILD(keyRow)        \
    ( *(knlLogicalAddr *)((keyRow) + SMDMIS_KEY_ROW_HEADER_SIZE) )

#define SMDMIS_KEY_ROW_GET_COL_BY_OFFSET(keyRow, offset)     \
    ( (keyRow) + (offset) )

#define SMDMIS_KEY_ROW_GET_COL(keyRow, col)     \
    ( (keyRow) + (col)->mOffset )

#define SMDMIS_KEY_ROW_GET_VAR_COL(keyRow, col) \
    ( knlToPhysicalAddr( *(knlLogicalAddr *)SMDMIS_KEY_ROW_GET_COL(keyRow, col) ) )

#define SMDMIS_KEY_ROW_SET_VAR_COL(keyRow, col, varColAddr)     \
    ( *(knlLogicalAddr *)SMDMIS_KEY_ROW_GET_COL(keyRow, col) = (varColAddr) )

#define SMDMIS_KEY_ROW_WRITE_COL(keyRow, col, sourceCol, len)        \
    *(smdmisKeyColLength *)( (keyRow) + (col)->mOffset ) = (len);    \
    stlMemcpy( (keyRow) + (col)->mOffset + STL_SIZEOF( smdmisKeyColLength ), (sourceCol), (len) )

#define SMDMIS_KEY_ROW_INIT_HEADER(keyRow) \
    ( *(keyRow) = 0 )

#define SMDMIS_KEY_ROW_INIT(keyRow, keyRowLen) \
    ( stlMemset( (keyRow), 0, (keyRowLen) ) )

#define SMDMIS_KEY_ROW_IS_DUP(keyRow)      \
    ( (*(keyRow) & SMDMIS_KEY_ROW_HEADER_DUP_MASK) == SMDMIS_KEY_ROW_HEADER_DUP_YES ? STL_TRUE : STL_FALSE )

#define SMDMIS_KEY_ROW_SET_DUP(keyRow)     \
    ( *(keyRow) = (*(keyRow) & ~SMDMIS_KEY_ROW_HEADER_DUP_MASK) | SMDMIS_KEY_ROW_HEADER_DUP_YES )

#define SMDMIS_KEY_ROW_GET_RADIX(keyRow, offset)   \
    ( *((stlUInt8 *)(keyRow) + (offset)) )

#define SMDMIS_KEY_ROW_SET_NULL(keyRow, col)   \
    ( *(stlUInt8 *)((keyRow) + SMDMIS_KEY_COL_OFFSET_NULL_VALUE(col)) = (SMDMIS_NULL_VALUE) )

#define SMDMIS_KEY_ROW_SET_NOT_NULL(keyRow, col)   \
    ( *(stlUInt8 *)((keyRow) + SMDMIS_KEY_COL_OFFSET_NULL_VALUE(col)) = (SMDMIS_NOT_NULL_VALUE) )

#define SMDMIS_KEY_ROW_SET_NUMERIC_NULL(keyRow, col)   \
    ( *(stlUInt8 *)((keyRow) + SMDMIS_KEY_COL_OFFSET_VALUE(col)) = (SMDMIS_NUMERIC_NULL_VALUE) )

#define SMDMIS_KEY_ROW_SET_NULL_FIRST(keyRow, col)   \
    ( *(stlUInt8 *)((keyRow) + SMDMIS_KEY_COL_OFFSET_NULL_VALUE(col)) = (SMDMIS_NULL_VALUE_NULLS_FIRST) )

#define SMDMIS_KEY_ROW_SET_NOT_NULL_FIRST(keyRow, col)   \
    ( *(stlUInt8 *)((keyRow) + SMDMIS_KEY_COL_OFFSET_NULL_VALUE(col)) = (SMDMIS_NOT_NULL_VALUE_NULLS_FIRST) )

#define SMDMIS_KEY_ROW_SET_NUMERIC_NULL_FIRST(keyRow, col)   \
    ( *(stlUInt8 *)((keyRow) + SMDMIS_KEY_COL_OFFSET_VALUE(col)) = (SMDMIS_NUMERIC_NULL_VALUE_NULLS_FIRST) )

/**
 * radix sort에 사용되는 매크로
 */
#define SMDMIS_COMPARE_KEY( key1, key2, radix1, radix2, offset, result )     \
    (radix1) = SMDMIS_KEY_ROW_GET_RADIX( key1, offset );                     \
    (radix2) = SMDMIS_KEY_ROW_GET_RADIX( key2, offset );                     \
    (result) = ( (radix1) < (radix2) ? DTL_COMPARISON_LESS :                 \
                 ((radix1) > (radix2) ? DTL_COMPARISON_GREATER : DTL_COMPARISON_EQUAL) )


/**
 * smdmisKeyCol macros
 */
#define SMDMIS_KEY_COL_ALIGN        \
    ( STL_SIZEOF(smdmisKeyColLength) )

#define SMDMIS_KEY_COL_SIZE(aColSize)          \
    ( STL_SIZEOF(smdmisKeyColLength) + (aColSize) )

#define SMDMIS_KEY_COL_GET_LEN(aCol)    \
    ( *(smdmisKeyColLength*)(aCol) )

#define SMDMIS_KEY_COL_GET_VALUE(aCol)  \
    ( (aCol) + STL_SIZEOF(smdmisKeyColLength) )


/**
 * smdmisIndexPath macros
 */
#define SMDMIS_INDEX_PATH_SET(path, node, pos)  \
    (path)->mParentNode = (node);               \
    (path)->mPosition   = (pos)

#define SMDMIS_INDEX_PATH_PARENT(path)          \
    ( (path)->mParentNode )

#define SMDMIS_INDEX_PATH_POS(path)             \
    ( (path)->mPosition )

#define SMDMIS_TO_NODE_PTR(nodeAddr)   \
    ( (smdmisIndexNode*)knlToPhysicalAddr(nodeAddr) )

#define SMDMIS_TO_RADIX_NODE_PTR(nodeAddr) \
    ( (smdmisRadixNode*)knlToPhysicalAddr(nodeAddr) )

stlStatus smdmisTableCreate( smlStatement        *aStatement,
                             smlTbsId             aTbsId,
                             smlSortTableAttr    *aAttr,
                             void                *aBaseTableHandle,
                             stlUInt16            aKeyColCount,
                             knlValueBlockList   *aKeyColList,
                             stlUInt8            *aKeyColFlags,
                             stlInt64            *aRelationId,
                             void               **aRelationHandle,
                             smlEnv              *aEnv );

stlStatus smdmisTableDrop( smlStatement *aStatement,
                           void         *aRelationHandle,
                           smlEnv       *aEnv );

stlStatus smdmisTableInsert( smlStatement      *aStatement,
                             void              *aRelationHandle,
                             stlInt32           aValueIdx,
                             knlValueBlockList *aInsertCols,
                             knlValueBlockList *aUniqueViolation,
                             smlRid            *aRowRid,
                             smlEnv            *aEnv );

stlStatus smdmisTableBlockInsert( smlStatement      *aStatement,
                                  void              *aRelationHandle,
                                  knlValueBlockList *aInsertCols,
                                  knlValueBlockList *aUniqueViolation,
                                  smlRowBlock       *aRowBlock,
                                  smlEnv            *aEnv );

stlStatus smdmisTableMerge( smlStatement       * aStatement,
                            void               * aRelationHandle,
                            knlValueBlockList  * aInsertCols,
                            smlMergeRecordInfo * aMergeRecordInfo,
                            smlEnv             * aEnv );

stlStatus smdmisTableBuildMergeEnv( smlStatement       *aStatement,
                                    smdmisTableHeader  *aHeader,
                                    smdmisMergeEnv    **aMergeEnv,
                                    smlEnv             *aEnv );

void smdmisTableReadyToMerge( smdmisTableHeader *aHeader,
                              smdmisMergeEnv    *aMergeEnv );

stlStatus smdmisTableBuildIndex( smlStatement      *aStatement,
                                 smdmisTableHeader *aHeader,
                                 smdmifTableHeader *aBaseHeader,
                                 smlEnv            *aEnv );

smdmisKeyRow smdmisTableGetNext( smdmisTableHeader *aHeader,
                                 smdmisMergeEnv    *aMergeEnv );

inline dtlCompareResult smdmisCompareKey( smdmisTableHeader *aHeader,
                                          stlChar           *aLeftRow,
                                          stlChar           *aRightRow );

stlStatus smdmisTableInvalidateRadix( smdmisTableHeader *aHeader,
                                      smlEnv            *aEnv );

stlStatus smdmisTableTruncate( smlStatement *aStatement,
                               void         *aRelationHandle,
                               smlEnv       *aEnv );

stlStatus smdmisTableVerify( smdmisTableHeader *aHeader );

#endif /*_SMDMIF_TABLE_H_ */

/** @} */
