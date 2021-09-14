/*******************************************************************************
 * smsmbDef.h
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


#ifndef _SMSMBDEF_H_
#define _SMSMBDEF_H_ 1

/**
 * @file smsmbDef.h
 * @brief Storage Manager Layer Bitmap Segment Internal Definition
 */

/**
 * @defgroup smsmb Memory Bitmap Segment
 * @ingroup smsInternal
 * @{
 */

#define SMSMB_MAX_TREE_DEPTH 5

typedef struct smsmbExtMapHdr
{
    stlInt32 mExtDescCount;
    smlPid   mPrvExtMap;
} smsmbExtMapHdr;

#define SMSMB_EXTENT_DESC_COUNT ( (SMP_PAGE_SIZE -                \
                                   STL_SIZEOF(smpPhyHdr) -        \
                                   STL_SIZEOF(smpPhyTail) -       \
                                   STL_SIZEOF(smsmbExtMapHdr)) /  \
                                  STL_SIZEOF(smlExtId) )

typedef struct smsmbExtMap
{
    smlExtId mExtId[SMSMB_EXTENT_DESC_COUNT];
} smsmbExtMap;

typedef struct smsmbHwm
{
    smlPid   mPageId;
    stlInt32 mOffset;
} smsmbHwm;

typedef struct smsmbCache
{
    smsCommonCache mCommonCache;
    
    knlLatch       mLatch;
    
    smlPid         mFstExtMap;
    smlPid         mLstExtMap;
    smlPid         mRootNode;
    smsmbHwm       mMetaHwm;
    smsmbHwm       mTreeHwm[SMSMB_MAX_TREE_DEPTH];
} smsmbCache;

typedef struct smsmbInternalHdr
{
    stlInt16 mChildCount;
    stlInt16 mFreenessCount[SMP_FREENESS_MAX];
    stlChar  mAlign[4];
} smsmbInternalHdr;

#define SMSMB_INTERNAL_BIT_COUNT ( ((SMP_PAGE_SIZE -                 \
                                    STL_SIZEOF(smpPhyHdr) -          \
                                    STL_SIZEOF(smpPhyTail) -         \
                                    STL_SIZEOF(smsmbInternalHdr)) /  \
                                    (1 + 2*STL_SIZEOF(smlPid))) * 2 )

typedef struct smsmbInternal
{
    smlPid  mChildPid[SMSMB_INTERNAL_BIT_COUNT];
    stlChar mFreeness[SMSMB_INTERNAL_BIT_COUNT];
} smsmbInternal;

typedef struct smsmbLeafHdr
{
    stlInt16 mChildCount;
    stlInt16 mFreenessCount[SMP_FREENESS_MAX];
    smlPid   mNextPid;
} smsmbLeafHdr;

/*
#define SMSMB_LEAF_BIT_COUNT ( ((SMP_PAGE_SIZE -               \
                                STL_SIZEOF(smpPhyHdr) -        \
                                STL_SIZEOF(smpPhyTail) -       \
                                STL_SIZEOF(smsmbLeafHdr)) /    \
                                (1 + 2*STL_SIZEOF(smlPid))) * 2 )
*/

/**
 * (32 * 4)는 테스트를 통한 최대 성능치이다.
 */
#define SMSMB_LEAF_BIT_COUNT (32 * 4)

typedef struct smsmbLeaf
{
    smlScn  mScn[SMSMB_LEAF_BIT_COUNT];
    smlPid  mDataPid[SMSMB_LEAF_BIT_COUNT];
    stlChar mFreeness[SMSMB_LEAF_BIT_COUNT];
} smsmbLeaf;

typedef struct smsmbFxBitmap
{
    stlBool    mIsLeaf;
    stlChar    mFreeness[3];
    smlScn     mAgableScn;
    stlInt16   mOffset;
    stlInt16   mDepth;
    smlPid     mPageId;
    smlPid     mChildPid;
} smsmbFxBitmap;

typedef struct smsmbFxBitmapHdr
{
    stlBool    mIsLeaf;
    stlChar    mFreeness[3];
    stlInt16   mDepth;
    stlInt16   mChildCount;
    smlPid     mPageId;
    smlPid     mNextPid;
    stlInt16   mUnallocatedBitCount;
    stlInt16   mFullBitCount;
    stlInt16   mUpdateonlyBitCount;
    stlInt16   mInsertableBitCount;
    stlInt16   mFreeBitCount;
} smsmbFxBitmapHdr;

#define SMSMB_INIT_FX_BITMAP_HDR( aFxBitmapHdr )    \
{                                                   \
    (aFxBitmapHdr)->mIsLeaf = STL_FALSE;            \
    (aFxBitmapHdr)->mFreeness[0] = '\0';            \
    (aFxBitmapHdr)->mFreeness[1] = '\0';            \
    (aFxBitmapHdr)->mFreeness[2] = '\0';            \
    (aFxBitmapHdr)->mDepth = 0;                     \
    (aFxBitmapHdr)->mChildCount = 0;                \
    (aFxBitmapHdr)->mPageId = SMP_NULL_PID;         \
    (aFxBitmapHdr)->mUnallocatedBitCount = 0;       \
    (aFxBitmapHdr)->mFullBitCount = 0;              \
    (aFxBitmapHdr)->mUpdateonlyBitCount = 0;        \
    (aFxBitmapHdr)->mInsertableBitCount = 0;        \
    (aFxBitmapHdr)->mFreeBitCount = 0;              \
    (aFxBitmapHdr)->mNextPid = SMP_NULL_PID;        \
}

#define SMSMB_MAX_SEARCH_STACK_DEPTH ( SMSMB_LEAF_BIT_COUNT +    \
                                       (SMSMB_INTERNAL_BIT_COUNT * (SMSMB_MAX_TREE_DEPTH-1)) )

typedef struct smsmbSegIterator
{
    stlInt32        mNextDataPos;
    stlInt32        mDataPidCount;
    stlInt32        mNextLeafPid;
    void          * mSegmentHandle;
    smlPid          mDataPid[SMSMB_LEAF_BIT_COUNT];
} smsmbSegIterator;

typedef struct smsmbFxBitmapIterator
{
    smlTbsId        mTbsId;
    stlInt32        mSearchStackTop;
    smsmbFxBitmap   mSearchStack[SMSMB_MAX_SEARCH_STACK_DEPTH];
} smsmbFxBitmapIterator;

typedef struct smsmbFxBitmapHdrIterator
{
    smlTbsId         mTbsId;
    stlInt32         mSearchStackTop;
    smsmbFxBitmapHdr mSearchStack[SMSMB_MAX_SEARCH_STACK_DEPTH];
} smsmbFxBitmapHdrIterator;

typedef struct smsmbSegHdr
{
    smsSegHdr mCommonSegHdr;
    
    smlPid    mFstExtMap;
    smlPid    mLstExtMap;
    smlPid    mRootNode;
    smsmbHwm  mMetaHwm;
    smsmbHwm  mTreeHwm[SMSMB_MAX_TREE_DEPTH];
} smsmbSegHdr;

#define SMSMB_MAX_DATA_PAGE_HINT 8

#define SMSMB_COPY_SEGHDR_FROM_CACHE( aSegHdr, aSegCache )                                          \
    {                                                                                               \
        (aSegHdr)->mCommonSegHdr.mInitialExtents = (aSegCache)->mCommonCache.mInitialExtents;       \
        (aSegHdr)->mCommonSegHdr.mNextExtents = (aSegCache)->mCommonCache.mNextExtents;             \
        (aSegHdr)->mCommonSegHdr.mMinExtents = (aSegCache)->mCommonCache.mMinExtents;               \
        (aSegHdr)->mCommonSegHdr.mMaxExtents = (aSegCache)->mCommonCache.mMaxExtents;               \
        (aSegHdr)->mCommonSegHdr.mAllocExtCount = (aSegCache)->mCommonCache.mAllocExtCount;         \
        (aSegHdr)->mFstExtMap = (aSegCache)->mFstExtMap;                                            \
        (aSegHdr)->mLstExtMap = (aSegCache)->mLstExtMap;                                            \
        (aSegHdr)->mRootNode = (aSegCache)->mRootNode;                                              \
        (aSegHdr)->mMetaHwm = (aSegCache)->mMetaHwm;                                                \
        stlMemcpy( (aSegHdr)->mTreeHwm,                                                             \
                   (aSegCache)->mTreeHwm,                                                           \
                   STL_SIZEOF( smsmbHwm ) * SMSMB_MAX_TREE_DEPTH );                                 \
        (aSegHdr)->mCommonSegHdr.mTbsId = aTbsId;                                                   \
        (aSegHdr)->mCommonSegHdr.mUsablePageCountInExt = (aSegCache)->mCommonCache.mUsablePageCountInExt; \
        (aSegHdr)->mCommonSegHdr.mSegType = SML_SEG_TYPE_MEMORY_BITMAP;                             \
        (aSegHdr)->mCommonSegHdr.mHasRelHdr = (aSegCache)->mCommonCache.mHasRelHdr;                 \
        (aSegHdr)->mCommonSegHdr.mState = SMS_STATE_ALLOC_BODY;                                     \
        (aSegHdr)->mCommonSegHdr.mValidScn = (aSegCache)->mCommonCache.mValidScn;                   \
        (aSegHdr)->mCommonSegHdr.mIsUsable = (aSegCache)->mCommonCache.mIsUsable;                   \
    }

#define SMSMB_GET_HINT_OFFSET( aPageHandle ) ( *(stlInt32*)smpGetVolatileArea( (aPageHandle) ) )
#define SMSMB_SET_HINT_OFFSET( aPageHandle, aHintOffset )                   \
    {                                                                       \
        *(stlInt32*)smpGetVolatileArea( (aPageHandle) ) = (aHintOffset);    \
    }

/** @} */
    
#endif /* _SMSMBDEF_H_ */
