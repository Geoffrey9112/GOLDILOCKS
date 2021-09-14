/*******************************************************************************
 * smqDef.h
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


#ifndef _SMQDEF_H_
#define _SMQDEF_H_ 1

/**
 * @file smqDef.h
 * @brief Storage Manager Layer Sequence Internal Definition
 */

/**
 * @defgroup smqInternal Component Internal Routines
 * @ingroup smq
 * @{
 */

typedef struct smqWarmupEntry
{
    void * mSequenceMapHandle;
} smqWarmupEntry;

/*
 * Sequence Map 페이지에 한개의 Sequence만 저장하기 위한 방안으로
 * Record에 Dummy를 기록하고, PCTFREE = 100, PCTUSED = 10으로 설정한다.
 */
#define SMQ_SEQUENCE_RECORD_DUMMY_SIZE (7000)

typedef struct smqRecord
{
    stlInt64  mStartWith;
    stlInt64  mIncrementBy;
    stlInt64  mMaxValue;
    stlInt64  mMinValue;
    stlInt64  mCacheSize;
    stlInt64  mRestartValue;
    stlBool   mCycle;
} smqRecord;

typedef struct smqCache
{
    knlLatch  mLatch;
    smlRid    mRid;
    stlInt64  mId;
    stlInt64  mNxtValue;
    stlInt64  mCurValue;
    stlInt64  mCacheCount;
    smlScn    mValidScn;
    smqRecord mRecord;
} smqCache;

#define SMQ_BUILD_COLUMN_LIST( aColumns, aRecord, aDummy )                                      \
    do                                                                                          \
    {                                                                                           \
        KNL_SET_BLOCK_DATA_PTR((aColumns)[0],                                                   \
                               0,                                                               \
                               (void*)(aRecord),                                                \
                               STL_SIZEOF( smqRecord ));                                        \
        KNL_SET_BLOCK_DATA_LENGTH((aColumns)[0], 0, STL_SIZEOF( smqRecord ));                   \
        KNL_SET_BLOCK_COLUMN_ORDER((aColumns)[0], 0);                                           \
        KNL_LINK_BLOCK_LIST((aColumns)[0], (aColumns)[1]);                                      \
                                                                                                \
        if( aDummy == NULL )                                                                    \
        {                                                                                       \
            KNL_LINK_BLOCK_LIST((aColumns)[0], NULL);                                           \
        }                                                                                       \
        else                                                                                    \
        {                                                                                       \
            KNL_SET_BLOCK_DATA_PTR((aColumns)[1],                                               \
                                   0,                                                           \
                                   (void*)(aDummy),                                             \
                                   SMQ_SEQUENCE_RECORD_DUMMY_SIZE);                             \
            KNL_SET_BLOCK_DATA_LENGTH((aColumns)[1], 0, SMQ_SEQUENCE_RECORD_DUMMY_SIZE);        \
            KNL_SET_BLOCK_COLUMN_ORDER((aColumns)[1], 1);                                       \
            KNL_LINK_BLOCK_LIST((aColumns)[1], NULL);                                           \
        }                                                                                       \
    } while( 0 )

#define SMQ_BUILD_RECORD( aRecord, aAttr )                         \
do                                                                 \
{                                                                  \
    if( (aAttr)->mValidFlags & SML_SEQ_STARTWITH_MASK )            \
    {                                                              \
        (aRecord)->mStartWith = (aAttr)->mStartWith;               \
    }                                                              \
    if( (aAttr)->mValidFlags & SML_SEQ_INCREMENTBY_MASK )          \
    {                                                              \
        (aRecord)->mIncrementBy = (aAttr)->mIncrementBy;           \
    }                                                              \
    if( (aAttr)->mValidFlags & SML_SEQ_MAXVALUE_MASK )             \
    {                                                              \
        (aRecord)->mMaxValue = (aAttr)->mMaxValue;                 \
    }                                                              \
    if( (aAttr)->mValidFlags & SML_SEQ_MINVALUE_MASK )             \
    {                                                              \
        (aRecord)->mMinValue = (aAttr)->mMinValue;                 \
    }                                                              \
    if( (aAttr)->mValidFlags & SML_SEQ_CACHESIZE_MASK )            \
    {                                                              \
        (aRecord)->mCacheSize = (aAttr)->mCacheSize;               \
    }                                                              \
    if( (aAttr)->mValidFlags & SML_SEQ_CYCLE_MASK )                \
    {                                                              \
        (aRecord)->mCycle = (aAttr)->mCycle;                       \
    }                                                              \
} while( 0 )

#define SMQ_COLUMN_COUNT  2

typedef struct smqFxSeqTable
{
    stlInt64  mSeqId;
    stlInt64  mStartWith;
    stlInt64  mIncrementBy;
    stlInt64  mMaxValue;
    stlInt64  mMinValue;
    stlInt64  mCacheSize;
    stlInt64  mNxtValue;
    stlInt64  mCurValue;
    stlInt64  mRestartValue;
    stlInt64  mCacheCount;
    stlBool   mCycle;
} smqFxSeqTable;

typedef struct smqSequencePathCtrl
{
    smlStatement      * mStatement;
    smiIterator       * mIterator;
    knlRegionMark       mMemMark;
    knlValueBlockList * mFetchCols[SMQ_COLUMN_COUNT];
    smlRowBlock         mRowBlock;
    stlChar             mDummy[SMQ_SEQUENCE_RECORD_DUMMY_SIZE];
    smqRecord           mSeqRecord;

} smqSequencePathCtrl;

typedef struct smqPendArg
{
    void        * mSequenceHandle;
    smxlTransId   mTransId;
} smqPendArg;

#define SMQ_UNDO_LOG_CREATE_SEQUENCE   (0)   /**< create sequence undo log */

#define SMQ_UNDO_LOG_MAX_COUNT (1)

/** @} */
    
#endif /* _SMQDEF_H_ */
