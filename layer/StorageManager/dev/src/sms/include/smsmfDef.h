/*******************************************************************************
 * smsmfDef.h
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


#ifndef _SMSMFDEF_H_
#define _SMSMFDEF_H_ 1

/**
 * @file smsmfDef.h
 * @brief Storage Manager Layer Flat Segment Internal Definition
 */

/**
 * @defgroup smsmf Memory Flat Segment
 * @ingroup smsInternal
 * @{
 */

typedef struct smsmfCache
{
    smsCommonCache   mCommonCache;
    smlPid           mFirstExtFirstPid;
    smlPid           mCurExtFirstPid;
} smsmfCache;

typedef struct smsmfExtHdr
{
    smlPid   mNextExtFirstPid;
    smlExtId mExtId;
} smsmfExtHdr;

typedef struct smsmfSegIterator
{
    smlPid    mNextPid;
} smsmfSegIterator;

/** @} */
    
#endif /* _SMSMFDEF_H_ */
