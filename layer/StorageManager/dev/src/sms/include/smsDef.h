/*******************************************************************************
 * smsDef.h
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


#ifndef _SMSDEF_H_
#define _SMSDEF_H_ 1

/**
 * @file smsDef.h
 * @brief Storage Manager Layer Segment Internal Definition
 */

/**
 * @defgroup smsInternal Component Internal Routines
 * @ingroup sms
 * @{
 */

#define SMS_SEG_HDR_PAGE_TYPE_SEGHDR     (0)
#define SMS_SEG_HDR_PAGE_TYPE_COLLEN     (1)

typedef struct smsSegHdr
{
    stlInt32   mPageType;         /* SMS_SEG_HDR_PAGE_TYPE_SEGHDR or SMS_SEG_HDR_PAGE_TYPE_COLLEN */
    stlChar    mPad2[4];
    /** @todo emum smlSegType을 4byte로 가정한다. 확인 필요 */
    smlSegType mSegType;
    smlTbsId   mTbsId;
    stlBool    mHasRelHdr;
    stlInt8    mState;
    smlScn     mValidScn;
    stlInt64   mValidSeq;
    stlInt64   mUsablePageCountInExt;
    stlInt32   mInitialExtents;
    stlInt32   mNextExtents;
    stlInt32   mMinExtents;
    stlInt32   mMaxExtents;
    stlInt32   mAllocExtCount;
    stlBool    mIsUsable;
    stlChar    mPad[3];
} smsSegHdr;

typedef stlStatus (*smsBuildCacheFunc) ( smlTbsId     aTbsId,
                                         smlPid       aSegHdrPid,
                                         void      ** aSegmentHandle,
                                         smlEnv     * aEnv );
typedef stlStatus (*smsAllocHandleFunc) ( smxlTransId       aTransId,
                                          smlRid            aSegRid,
                                          smlTbsId          aTbsId,
                                          smlSegmentAttr  * aAttr,
                                          stlBool           aHasRelHdr,
                                          smsSegHdr       * aSegHdr,
                                          void           ** aSegmentHandle,
                                          smlEnv          * aEnv );
typedef stlStatus (*smsCloneHandleFunc) ( smxlTransId   aTransId,
                                          smlRid        aNewSegRid,
                                          void        * aOrgSegHandle,
                                          smsSegHdr   * aNewSegHdr,
                                          void       ** aNewSegandle,
                                          smlEnv      * aEnv );
typedef stlStatus (*smsCreateFunc) ( smxlTransId    aTransId,
                                     smlTbsId       aTbsID,
                                     void         * aSegmentHandle,
                                     stlInt32       aInitialExtents,
                                     smlEnv       * aEnv );
typedef stlStatus (*smsDropFunc) ( smxlTransId   aTransId,
                                   smlTbsId      aTbsId,
                                   void        * aSegmentHandle,
                                   smlEnv      * aEnv );
typedef stlStatus (*smsInsertPendingDropFunc) ( smxlTransId   aTransId,
                                                smlTbsId      aTbsId,
                                                void        * aSegmentHandle,
                                                smlEnv      * aEnv );
typedef stlStatus (*smsResetFunc) ( smxlTransId   aTransId,
                                    smlTbsId      aTbsId,
                                    void        * aSegmentHandle,
                                    stlBool       aFreePages,
                                    smlEnv      * aEnv );
typedef stlStatus (*smsAllocPageFunc) ( smxmTrans       * aMiniTrans,
                                        smlTbsId          aTbsId,
                                        void            * aSegmentHandle,
                                        smpPageType       aPageType,
                                        smpIsAgableFunc   aIsAgableFunc,
                                        smpHandle       * aPageHandle,
                                        smlEnv          * aEnv );
typedef stlStatus (*smsPreparePageFunc) ( smxlTransId   aTransId,
                                          void        * aSegmentHandle,
                                          stlUInt32     aPageCount,
                                          smlEnv      * aEnv );
typedef stlStatus (*smsAllocMultiPageFunc) ( smxmTrans       * aMiniTrans,
                                             smlTbsId          aTbsId,
                                             void            * aSegmentHandle,
                                             smpPageType       aPageType,
                                             stlInt32          aPageCount,
                                             smpIsAgableFunc   aIsAgableFunc,
                                             smpHandle       * aPageHandle,
                                             smlEnv          * aEnv );
typedef stlStatus (*smsStealPagesFunc) ( smxmTrans   * aMiniTrans,
                                         smlTbsId      aTbsId,
                                         void        * aDstSegHandle,
                                         void        * aSrcSegHandle,
                                         stlInt64      aShrinkToPageCount,
                                         stlInt64    * aStolenPageCount,
                                         smlEnv      * aEnv );
typedef stlStatus (*smsGetInsertablePageFunc) ( smxmTrans       * aMiniTrans,
                                                smlTbsId          aTbsId,
                                                void            * aSegmentHandle,
                                                smpPageType       aPageType,
                                                smpIsAgableFunc   aIsAgableFunc,
                                                smsSearchHint   * aSearchHint,
                                                smpHandle       * aPageHandle,
                                                stlBool         * aIsNewPage,
                                                smlEnv          * aEnv );
typedef stlStatus (*smsUpdatePageStatusFunc) ( smxmTrans   * aMiniTrans,
                                               smlTbsId      aTbsId,
                                               smpHandle   * aPageHandle,
                                               smpFreeness   aFreeness,
                                               smlScn        aScn,
                                               smlEnv      * aEnv );
typedef stlStatus (*smsAllocExtentFunc)( smxlTransId   aTransId,
                                         smlTbsId      aTbsId,
                                         void        * aCache,
                                         smpPageType   aPageType,
                                         smlPid      * aFirstDataPid,
                                         smlEnv      * aEnv );
typedef stlStatus (*smsAllocSegIteratorFunc) ( smlStatement  * aStatement,
                                               void         ** aSegIterator,
                                               smlEnv        * aEnv );
typedef stlStatus (*smsFreeSegIteratorFunc) ( void   * aSegIterator,
                                              smlEnv * aEnv );
typedef stlStatus (*smsGetFirstPageFunc) ( smlTbsId      aTbsId,
                                           void        * aSegmentHandle,
                                           void        * aSegIterator,
                                           smlPid      * aPageId,
                                           smlEnv      * aEnv );
typedef stlStatus (*smsGetNextPageFunc) ( smlTbsId      aTbsId,
                                          void        * aSegIterator,
                                          smlPid      * aPageId,
                                          smlEnv      * aEnv );
typedef stlInt32 (*smsGetHeaderSizeFunc) ();
typedef stlInt32 (*smsGetCacheSizeFunc) ();
typedef stlInt64 (*smsGetUsablePageCountInExtFunc) ( void * aSegmentHandle );
typedef stlStatus (*smsSetTransToHandleFunc) ( void       * aSegmentHandle,
                                               smxlTransId   aTransId,
                                               smlScn        aTransViewScn,
                                               smlEnv      * aEnv );
typedef stlStatus (*smsShrinkToFunc) ( smxlTransId      aTransId,
                                       void           * aSegmentHandle,
                                       smlSegmentAttr * aAttr,
                                       smlEnv         * aEnv );
typedef stlStatus (*smsGetUsedPageCountFunc) ( void       * aSegmentHandle,
                                               stlInt64   * aUsedPageCount,
                                               smlEnv     * aEnv );


typedef struct smsSegmentFuncs
{
    smsAllocHandleFunc           mAllocHandle;
    smsCloneHandleFunc           mCloneHandle;
    smsCreateFunc                mCreate;
    smsDropFunc                  mDrop;
    smsInsertPendingDropFunc     mInsertPendingDrop;
    smsResetFunc                 mReset;
    smsAllocPageFunc             mAllocPage;
    smsPreparePageFunc           mPreparePage;
    smsAllocMultiPageFunc        mAllocMultiPage;
    smsStealPagesFunc            mStealPages;
    smsGetInsertablePageFunc     mGetInsertablePage;
    smsUpdatePageStatusFunc      mUpdatePageStatus;
    smsAllocSegIteratorFunc      mAllocSegIterator;
    smsFreeSegIteratorFunc       mFreeSegIterator;
    smsGetFirstPageFunc          mGetFirstPage;
    smsGetNextPageFunc           mGetNextPage;
    smsBuildCacheFunc            mBuildCache;
    smsAllocExtentFunc           mAllocExtent;
    smsGetHeaderSizeFunc         mGetHeaderSize;
    smsGetCacheSizeFunc          mGetCacheSize;
    smsGetUsedPageCountFunc      mGetUsedPageCount;
    smsShrinkToFunc              mShrinkTo;
    smsSetTransToHandleFunc      mSetTransToHandle;
} smsSegmentFuncs;

typedef struct smsCommonCache
{
    smlSegType mSegType;
    smlTbsId   mTbsId;
    stlBool    mHasRelHdr;
    stlBool    mIsUsable;
    smlRid     mSegRid;
    stlInt64   mSegId;
    smlScn     mValidScn;
    stlInt64   mValidSeq;
    stlInt64   mUsablePageCountInExt;
    stlInt32   mInitialExtents;
    stlInt32   mNextExtents;
    stlInt32   mMinExtents;
    stlInt32   mMaxExtents;
    stlInt32   mAllocExtCount;
    stlChar    mPaddingForCacheAlign[4];
} smsCommonCache;

typedef struct smsFxSegment
{
    stlChar    mSegType[21];
    smlTbsId   mTbsId;
    stlInt64   mSegmentId;
    stlInt64   mAllocPageCount;
    
    stlBool    mHasRelHdr;
    stlBool    mIsUsable;
    stlChar    mState[21];
    smlScn     mValidScn;
    stlInt64   mValidSeq;
    stlInt64   mUsablePageCountInExt;
    stlInt32   mInitialExtents;
    stlInt32   mNextExtents;
    stlInt32   mMinExtents;
    stlInt32   mMaxExtents;
} smsFxSegment;

typedef struct smsSegmentPathCtrl
{
    void   * mSegIterator;
    stlBool  mFetchStarted;
} smsSegmentPathCtrl;

/**
 * @addtogroup smrUndoLogType
 * @{
 */
#define SMS_UNDO_LOG_ALTER_SEGMENT_HDR                0  /**< alter Segment Header */
#define SMS_UNDO_LOG_MEMORY_SET_UNUSABLE_SEGMENT      1  /**< Set Unusable Segment */
#define SMS_UNDO_LOG_MEMORY_SET_USABLE_SEGMENT        2  /**< Set Usable Segment */
#define SMS_UNDO_LOG_MEMORY_RESET_PENDING_SEGMENT     3  /**< Reset Pending Segment */
/** @} */

#define SMS_UNDO_LOG_MAX_COUNT       4

/**
 * @brief 최대 세그먼트 힌트 사이즈
 */
#define SMS_MAX_SEGMENT_HINT 8

typedef struct smsHint
{
    void   * mSegHandle;
    smlScn   mValidScn;
    smlPid   mHintPid;
} smsHint;

/**
 * @brief 최대 세그먼트 검색 캐시 사이즈
 */
typedef struct smsHintPool
{
    stlInt32 mSequence;
    smsHint  mSegHint[SMS_MAX_SEGMENT_HINT];
} smsHintPool;

/**
 * @brief segment handle alloc시 segment header를 할당할 필요가 있는지 질의
 */
#define SMS_IS_NEED_SEGMENT_HDR(aSegType)  ((aSegType) != SML_SEG_TYPE_MEMORY_FLAT)


/**
 * @brief segment handle로부터 할당한 extent 개수 얻기
 */
#define SMS_SEGMENT_HANDLE_GET_ALLOC_EXT_COUNT(aSegHandle) \
    ( ((smsCommonCache *)(aSegHandle))->mAllocExtCount )

/**
 * @brief SMS_TRY_INTERNAL_ERROR 매크로
 * @remarks aParams 전체는 반드시 ()안에 들어 있어야 한다. <BR>
 * @par example
 * SML_ASSERT( aLsn < 10, aExpression, (aEnv, "LSN(%d)\n", aLsn) );
 */
#define SMS_TRY_INTERNAL_ERROR( aExpression, aEnv, aParams )    \
    do                                                          \
    {                                                           \
        if( !(aExpression) )                                    \
        {                                                       \
            knlLogMsg( NULL,                                    \
                       KNL_ENV(aEnv),                           \
                       KNL_LOG_LEVEL_FATAL,                     \
                       "ASSERT(%s)\n"                           \
                       "failed in file %s line %d\n",           \
                       #aExpression,                            \
                       __FILE__,                                \
                       __LINE__ );                              \
                                                                \
            STL_TRY( smsInternalError aParams == STL_SUCCESS ); \
        }                                                       \
    } while( 0 )

typedef struct smsPendArg
{
    void        * mSegmentHandle;
    smxlTransId   mTransId;
} smsPendArg;

/** @} */
    
#endif /* _SMSDEF_H_ */
