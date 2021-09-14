/*******************************************************************************
 * smdmisScan.h
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smdmisScan.h 6127 2012-10-29 10:46:38Z htkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file smdmisScan.h
 * @brief Storage Manager Layer Memory Instant Sort Table Routines
 */

/**
 * @defgroup smdmisScan Memory Instant Sort Table
 * @ingroup smdInternal
 * @{
 */

#include <smdmisTable.h>

typedef struct smdmisIterator smdmisIterator;

typedef stlStatus (*smdmisScanFindFirstFunc)( smdmisTableHeader  *aHeader,
                                              smdmisIterator     *aIterator,
                                              knlCompareList     *aMinRange );

typedef stlStatus (*smdmisScanTryFindLastFunc)( smdmisTableHeader  *aHeader,
                                                smdmisIterator     *aIterator,
                                                knlCompareList     *aMinRange );

struct smdmisIterator
{
    smiIterator               mCommon;

    knlLogicalAddr            mCurNode;
    stlInt32                  mCurIndex;
    knlLogicalAddr            mLastNode;
    stlInt32                  mLastIndex;

    smdmisScanFindFirstFunc   mFindFirst;
    smdmisScanTryFindLastFunc mTryFindLast;
    smdmisKeyRow              mTransKey; /* key range의 값을 key와 비교하기 위해서는 transform이 필요한데, 거기 사용되는 메모리이다. */
    dtlCompareResult         *mCompWhenRightRemains;
    stlBool                  *mRadixEqualIncluded;
    stlUInt16                 mRadixIndexFence;
    stlBool                   mForMinRange;
    stlBool                   mCompMethodDecided;
};

#define SMDMIS_INDEX_BEFORE_FIRST        -1

#define SMDMIS_TABLE_ITERATOR_GET_TABLE_HEADER(aIterator)    \
    ( (smdmisTableHeader *)((aIterator)->mCommon.mRelationHandle) )

#define SMDMIS_TABLE_ITERATOR_RESET(aIterator)          \
    (aIterator)->mCurNode = KNL_LOGICAL_ADDR_NULL;      \
    (aIterator)->mCurIndex = SMDMIS_INDEX_BEFORE_FIRST; \
    (aIterator)->mLastNode = KNL_LOGICAL_ADDR_NULL;     \
    (aIterator)->mLastIndex = SMDMIS_INDEX_BEFORE_FIRST;\
    (aIterator)->mRadixIndexFence = 0;                  \
    (aIterator)->mForMinRange = STL_TRUE

#define SMDMIS_TABLE_ITERATOR_INIT(aIterator)           \
    SMDMIS_TABLE_ITERATOR_RESET(aIterator);             \
    (aIterator)->mFindFirst = NULL;                     \
    (aIterator)->mTryFindLast = NULL;                   \
    (aIterator)->mTransKey = NULL;                      \
    (aIterator)->mCompWhenRightRemains = NULL;          \
    (aIterator)->mRadixEqualIncluded = NULL;            \
    (aIterator)->mCompMethodDecided = STL_FALSE

#define SMDMIS_TABLE_ITERATOR_IS_BEFORE_FIRST(aIterator) \
    ( ((aIterator)->mCurNode == KNL_LOGICAL_ADDR_NULL) && ((aIterator)->mCurIndex == SMDMIS_INDEX_BEFORE_FIRST) ? STL_TRUE : STL_FALSE )

#define SMDMIS_TABLE_ITERATOR_IS_END(aIterator)         \
    ( ((aIterator)->mCurNode == (aIterator)->mLastNode) && ((aIterator)->mCurIndex > (aIterator)->mLastIndex) ? STL_TRUE : STL_FALSE )

stlStatus smdmisScanInitIterator( void   *aIterator,
                                  smlEnv *aEnv );

stlStatus smdmisScanResetIterator( void   *aIterator,
                                   smlEnv *aEnv );

stlStatus smdmisScanFindFirstAndLast( smdmisTableHeader *aHeader,
                                      smdmisIterator    *aIterator,
                                      smlFetchInfo      *aFetchInfo,
                                      smlEnv            *aEnv );

stlStatus smdmisScanFetchNext( void          * aIterator,
                               smlFetchInfo  * aFetchInfo,
                               smlEnv        * aEnv );

stlStatus smdmisScanFetchPrev( void          * aIterator,
                               smlFetchInfo  * aFetchInfo,
                               smlEnv        * aEnv );

stlBool smdmisScanFilt( smdmisTableHeader  *aHeader,
                        smdmisKeyRow        aKeyRow,
                        smlFetchInfo       *aFetchInfo );

/** @} */

