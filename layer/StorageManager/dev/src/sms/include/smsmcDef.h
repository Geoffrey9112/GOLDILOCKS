/*******************************************************************************
 * smsmcDef.h
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


#ifndef _SMSMCDEF_H_
#define _SMSMCDEF_H_ 1

/**
 * @file smsmcDef.h
 * @brief Storage Manager Layer Circular Segment Internal Definition
 */

/**
 * @defgroup smsmc Memory Circular Segment
 * @ingroup smsInternal
 * @{
 */

/**
 * @brief Steal할 수 있는 최대 페이지 개수
 */
#define SMSMC_MAX_STEAL_PAGE_COUNT        (1024)

typedef struct smsmcCache
{
    smsCommonCache   mCommonCache;
    smlPid           mCurDataPid;
    smlPid           mHdrPid;
    smlExtId         mCurExtId;
    smxlTransId      mLastTransId;
    smlScn           mLastTransViewScn;
} smsmcCache;

typedef struct smsmcExtHdr
{
    smlPid      mNextHdrPid;
    smlPid      mPrevHdrPid;
    smlExtId    mExtId;
    smxlTransId mLastTransId;
    smlScn      mLastTransViewScn;
} smsmcExtHdr;

typedef struct smsmcSegHdr
{
    smsSegHdr mCommonSegHdr;
    smlPid    mCurDataPid;
    smlPid    mHdrPid;
    smlExtId  mCurExtId;
} smsmcSegHdr;

typedef struct smsmcSegIterator
{
    smlPid        mFirstPid;
    smlPid        mCurDataPid;
    smlPid        mHdrPid;
} smsmcSegIterator;

typedef struct smsmcFxExtIterator
{
    smlTbsId mTbsId;
    smlPid   mStartExtHdrPid;
    smlPid   mNextExtHdrPid;
} smsmcFxExtIterator;

typedef struct smsmcFxExtHdr
{
    smlPid      mCurrHdrPid;
    smlPid      mNextHdrPid;
    smlPid      mPrevHdrPid;
    smlExtId    mExtId;
    smxlTransId mLastTransId;
    smlScn      mLastTransViewScn;
} smsmcFxExtHdr;

/** @} */
    
#endif /* _SMSMCDEF_H_ */
