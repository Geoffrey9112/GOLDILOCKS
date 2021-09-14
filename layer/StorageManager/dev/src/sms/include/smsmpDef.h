/*******************************************************************************
 * smsmpDef.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smsmpDef.h 5694 2012-09-20 03:28:39Z egon $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _SMSMPDEF_H_
#define _SMSMPDEF_H_ 1

/**
 * @file smsmpDef.h
 * @brief Storage Manager Layer Pending Segment Internal Definition
 */

/**
 * @defgroup smsmp Memory Pending Segment
 * @ingroup smsInternal
 * @{
 */

typedef struct smsmpCache
{
    smsCommonCache   mCommonCache;
    smlPid           mFirstHdrPid;
    smlPid           mCurDataPid;
    smlPid           mCurHdrPid;
} smsmpCache;

typedef struct smsmpExtHdr
{
    smlPid   mNextHdrPid;
    stlChar  mPad[4];
    smlExtId mExtId;
} smsmpExtHdr;

typedef struct smsmpSegHdr
{
    smsSegHdr mCommonSegHdr;
    smlPid    mFirstHdrPid;
    smlPid    mCurDataPid;
    smlPid    mCurHdrPid;
} smsmpSegHdr;

typedef struct smsmpSegIterator
{
    smlPid        mCurDataPid;
    smlPid        mHdrPid;
    smlPid        mEndDataPid;
    stlBool       mEndOfScan;
} smsmpSegIterator;

/** @} */
    
#endif /* _SMSMPDEF_H_ */
