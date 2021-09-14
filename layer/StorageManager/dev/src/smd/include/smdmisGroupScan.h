/*******************************************************************************
 * smdmisGroupScan.h
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id:
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file smdmisGroupScan.h
 * @brief Storage Manager Layer Memory Instant Sort Table Routines
 */

/**
 * @defgroup smdmisGroupScan Memory Instant Sort Table
 * @ingroup smdInternal
 * @{
 */

#include <smdmisTable.h>
#include <smdmisScan.h>

typedef struct smdmisGroupIterator
{
    smiIterator               mCommon;

    knlLogicalAddr            mCurNode;
    stlInt32                  mCurIndex;
    knlLogicalAddr            mLastNode;
    stlInt32                  mLastIndex;

    smdmisScanFindFirstFunc   mFindFirst;
    smdmisScanTryFindLastFunc mTryFindLast;
    smdmisKeyRow              mTransKey; /* key range의 값을 key와 비교하기 위해서는 transform이 필요한데, 거기 사용되는 메모리이다. */
    stlBool                  *mCompWhenRightRemains;
    stlBool                  *mRadixEqualIncluded;
    stlUInt16                 mRadixIndexFence;
    stlBool                   mForMinRange;
    stlBool                   mCompMethodDecided;
    /* 여기까지는 smdmisIterator와 동일함을 유지해야 한다. */

    /* group start 유무를 알려준다 */
    stlBool                   mGroupStart;

    /* group의 맨 첫 row를 저장하기 위한 맴버들 */
    knlLogicalAddr            mGroupFirstNode;
    stlInt32                  mGroupFirstIndex;
} smdmisGroupIterator;

#define SMDMIS_GROUP_ITERATOR_GET_TABLE_HEADER(aIterator)    \
    ( (smdmisTableHeader *)((aIterator)->mCommon.mRelationHandle) )

#define SMDMIS_GROUP_ITERATOR_RESET(aIterator)        \
    SMDMIS_TABLE_ITERATOR_RESET(aIterator);           \
    (aIterator)->mGroupStart = STL_FALSE;             \

#define SMDMIS_GROUP_ITERATOR_INIT(aIterator)         \
    SMDMIS_TABLE_ITERATOR_INIT(aIterator);            \
    (aIterator)->mGroupStart = STL_FALSE;             \

#define SMDMIS_GROUP_ITERATOR_IS_BEFORE_FIRST(aIterator) \
    SMDMIS_TABLE_ITERATOR_IS_BEFORE_FIRST(aIterator)

#define SMDMIS_GROUP_ITERATOR_IS_END(aIterator)         \
    SMDMIS_TABLE_ITERATOR_IS_END(aIterator)

#define SMDMIS_GROUP_ITERATOR_MARK_CURRENT(aIterator)      \
    (aIterator)->mGroupFirstNode = (aIterator)->mCurNode;  \
    (aIterator)->mGroupFirstIndex = (aIterator)->mCurIndex;

#define SMDMIS_GROUP_ITERATOR_RETURN_TO_MARK(aIterator)    \
    (aIterator)->mCurNode = (aIterator)->mGroupFirstNode;  \
    (aIterator)->mCurIndex = (aIterator)->mGroupFirstIndex;

stlStatus smdmisGroupScanInitIterator( void   *aIterator,
                                       smlEnv *aEnv );

stlStatus smdmisGroupScanResetIterator( void   *aIterator,
                                        smlEnv *aEnv );

stlStatus smdmisGroupScanFetchNext( void          * aIterator,
                                    smlFetchInfo  * aFetchInfo,
                                    smlEnv        * aEnv );

stlStatus smdmisGroupScanFetchPrev( void          * aIterator,
                                    smlFetchInfo  * aFetchInfo,
                                    smlEnv        * aEnv );

stlStatus smdmisGroupScanMoveToRowRid( void   *aIterator,
                                       smlRid *aRowRid,
                                       smlEnv *aEnv );

stlStatus smdmisGroupScanMoveToPosNum( void     *aIterator,
                                       stlInt64  aPosNum,
                                       smlEnv   *aEnv );

stlStatus smdmisGroupScanNextGroup( void          *aIterator,
                                    smlFetchInfo  *aFetchInfo,
                                    smlEnv        *aEnv );

stlStatus smdmisGroupScanResetGroup( void          *aIterator,
                                     smlEnv        *aEnv );

/** @} */

