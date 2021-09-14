/*******************************************************************************
 * smsmbSegment.c
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

#include <stl.h>
#include <dtl.h>
#include <knl.h>
#include <scl.h>
#include <smlDef.h>
#include <smDef.h>
#include <smt.h>
#include <smxm.h>
#include <smxl.h>
#include <smp.h>
#include <smd.h>
#include <smg.h>
#include <smf.h>
#include <sms.h>
#include <smsDef.h>
#include <smsmbDef.h>
#include <smsManager.h>
#include <smsmbSegment.h>
#include <smsmbSpaceTree.h>
#include <smsmbExtMap.h>
#include <smsmbScan.h>
#include <smsmbLog.h>

/**
 * @file smsmbSegment.c
 * @brief Storage Manager Layer Bitmap Segment Internal Routines
 */

/**
 * @addtogroup smsmbSegment
 * @{
 */

smsSegmentFuncs gMemoryBitmapSegment =
{
    smsmbAllocHandle,
    smsmbCloneHandle,
    smsmbCreate,
    smsmbDrop,
    NULL,
    smsmbReset,
    smsmbAllocPage,
    NULL,               /* PreparePage */
    NULL,               /* AllocMultiPage */
    NULL,               /* StealPages */
    smsmbGetInsertablePage,
    smsmbUpdatePageStatus,
    smsmbAllocSegIterator,
    smsmbFreeSegIterator,
    smsmbGetFirstPage,
    smsmbGetNextPage,
    smsmbBuildCache,
    smsmbAllocExtent,
    smsmbGetHeaderSize,
    smsmbGetCacheSize,
    smsmbGetUsedPageCount,
    NULL,               /* Shrink */
    NULL                /* SetTransToHandle */
};

stlStatus smsmbBuildCache( smlTbsId     aTbsId,
                           smlPid       aSegHdrPid,
                           void      ** aSegmentHandle,
                           smlEnv     * aEnv )
{
    smsmbCache  * sCache;
    stlInt32      sState = 0;
    smpHandle     sPageHandle;
    smsmbSegHdr * sSegHdr;
    
    STL_TRY( smgAllocShmMem4Open( STL_SIZEOF( smsmbCache ),
                                  aSegmentHandle,
                                  aEnv ) == STL_SUCCESS );

    sCache = (smsmbCache*)(*aSegmentHandle);

    STL_TRY( smsmbInitCache( sCache,
                             aEnv ) == STL_SUCCESS );

    STL_TRY( smpFix( aTbsId,
                     aSegHdrPid,
                     &sPageHandle,
                     aEnv ) == STL_SUCCESS );
    sState = 1;

    sSegHdr = (smsmbSegHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );

    sCache->mFstExtMap= sSegHdr->mFstExtMap;
    sCache->mLstExtMap = sSegHdr->mLstExtMap;
    sCache->mRootNode = sSegHdr->mRootNode;
    sCache->mMetaHwm = sSegHdr->mMetaHwm;
    stlMemcpy( sCache->mTreeHwm,
               sSegHdr->mTreeHwm,
               STL_SIZEOF( smsmbHwm ) * SMSMB_MAX_TREE_DEPTH );
    
    sCache->mCommonCache.mInitialExtents = sSegHdr->mCommonSegHdr.mInitialExtents;
    sCache->mCommonCache.mNextExtents = sSegHdr->mCommonSegHdr.mNextExtents;
    sCache->mCommonCache.mMinExtents = sSegHdr->mCommonSegHdr.mMinExtents;
    sCache->mCommonCache.mMaxExtents = sSegHdr->mCommonSegHdr.mMaxExtents;
    sCache->mCommonCache.mAllocExtCount = sSegHdr->mCommonSegHdr.mAllocExtCount;
    sCache->mCommonCache.mIsUsable = sSegHdr->mCommonSegHdr.mIsUsable;

    if( smfIsValidTbs( sSegHdr->mCommonSegHdr.mTbsId ) == STL_TRUE )
    {
        sCache->mCommonCache.mUsablePageCountInExt =
            smfGetPageCountInExt( sSegHdr->mCommonSegHdr.mTbsId );
    }
    else
    {
        /**
         * 이후에 aging될 Segment이다.
         */
        sCache->mCommonCache.mUsablePageCountInExt = 0;
    }
    
    sCache->mCommonCache.mSegType = sSegHdr->mCommonSegHdr.mSegType;
    sCache->mCommonCache.mTbsId = sSegHdr->mCommonSegHdr.mTbsId;
    sCache->mCommonCache.mValidScn = sSegHdr->mCommonSegHdr.mValidScn;
    sCache->mCommonCache.mValidSeq = sSegHdr->mCommonSegHdr.mValidSeq;
    sCache->mCommonCache.mHasRelHdr = sSegHdr->mCommonSegHdr.mHasRelHdr;
    sCache->mCommonCache.mSegRid = (smlRid){ SML_MEMORY_DICT_SYSTEM_TBS_ID,
                                             0,
                                             aSegHdrPid };
    
    smsSetSegmentId( (void*)sCache, sCache->mCommonCache.mSegRid );
    
    smpSetVolatileArea( &sPageHandle, &sCache, STL_SIZEOF(void*) );

    sState = 0;
    STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) smpUnfix( &sPageHandle, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smsmbAllocHandle( smxlTransId       aTransId,
                            smlRid            aSegRid,
                            smlTbsId          aTbsId,
                            smlSegmentAttr  * aAttr,
                            stlBool           aHasRelHdr,
                            smsSegHdr       * aSegHdr,
                            void           ** aSegmentHandle,
                            smlEnv          * aEnv )
{
    smsmbCache * sCache;
    stlUInt64    sBytes;
    stlUInt16    sPageCountInExt;

    *aSegmentHandle = NULL;
    
    STL_TRY( smgAllocShmMem4Open( STL_SIZEOF( smsmbCache ),
                                  aSegmentHandle,
                                  aEnv ) == STL_SUCCESS );

    sCache = (smsmbCache*)(*aSegmentHandle);
    sPageCountInExt = smfGetPageCountInExt( aTbsId );

    STL_TRY( smsmbInitCache( sCache,
                             aEnv ) == STL_SUCCESS );

    smsSetSegRid( (void*)sCache, aSegRid );
    smsSetSegmentId( (void*)sCache, aSegRid );
    smsSetSegType( (void*)sCache, SML_SEG_TYPE_MEMORY_BITMAP );
    smsSetTbsId( (void*)sCache, aTbsId );
    smsSetHasRelHdr( (void*)sCache, aHasRelHdr );
    smsSetValidScn( (void*)sCache, smxlGetTransViewScn( aTransId ) );
    smsSetUsablePageCountInExt( (void*)sCache, smfGetPageCountInExt( aTbsId ) );
    smsSetValidSeq( (void*)sCache, 0 );

    if( aSegHdr != NULL )
    {
        smsSetValidSeq( (void*)sCache, aSegHdr->mValidSeq );
    }

    sBytes = ( (aAttr->mValidFlags & SML_SEGMENT_INITIAL_MASK) ?
               aAttr->mInitial : SMS_INITIAL_DEFAULT(sPageCountInExt) );
    sCache->mCommonCache.mInitialExtents = STL_ALIGN( STL_ALIGN( sBytes,
                                                                 SMP_PAGE_SIZE ) / SMP_PAGE_SIZE,
                                                      sPageCountInExt ) / sPageCountInExt;

    /**
     * 세그먼트 초기 사이즈는 Meta Extent를 고려해서 2이상이여야 한다.
     */
    if( sCache->mCommonCache.mInitialExtents < 2 )
    {
        sCache->mCommonCache.mInitialExtents = 2;
    }
    
    sBytes = ( (aAttr->mValidFlags & SML_SEGMENT_NEXT_MASK) ?
               aAttr->mNext : SMS_NEXT_DEFAULT(sPageCountInExt) );
    sCache->mCommonCache.mNextExtents = STL_ALIGN( STL_ALIGN( sBytes,
                                                              SMP_PAGE_SIZE ) / SMP_PAGE_SIZE,
                                                   sPageCountInExt ) / sPageCountInExt;
    
    sBytes = ( (aAttr->mValidFlags & SML_SEGMENT_MINSIZE_MASK) ?
               aAttr->mMinSize : SMS_MINSIZE_DEFAULT(sPageCountInExt) );
    sCache->mCommonCache.mMinExtents = STL_ALIGN( STL_ALIGN( sBytes,
                                                             SMP_PAGE_SIZE ) / SMP_PAGE_SIZE,
                                                  sPageCountInExt ) / sPageCountInExt;
    
    /**
     * 세그먼트 초기 사이즈는 Meta Extent를 고려해서 2이상이여야 한다.
     */
    if( sCache->mCommonCache.mMinExtents < 2 )
    {
        sCache->mCommonCache.mMinExtents = 2;
    }
    
    sBytes = ( (aAttr->mValidFlags & SML_SEGMENT_MAXSIZE_MASK) ?
               aAttr->mMaxSize : SMS_MAXSIZE_DEFAULT(sPageCountInExt) );

    sCache->mCommonCache.mMaxExtents = STL_ALIGN( STL_ALIGN( sBytes,
                                                             SMP_PAGE_SIZE ) / SMP_PAGE_SIZE,
                                                  sPageCountInExt ) / sPageCountInExt;
    
    return STL_SUCCESS;

    STL_FINISH;

    if( *aSegmentHandle != NULL )
    {
        (void)smgFreeShmMem4Open( *aSegmentHandle, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smsmbCloneHandle( smxlTransId     aTransId,
                            smlRid          aNewSegRid,
                            void          * aOrgSegHandle,
                            smsSegHdr     * aNewSegHdr,
                            void         ** aNewSegHandle,
                            smlEnv        * aEnv )
{
    smsmbCache  * sOrgCache;
    smsmbCache  * sNewCache = NULL;

    *aNewSegHandle = NULL;
    sOrgCache = (smsmbCache*)aOrgSegHandle;
    
    STL_TRY( smgAllocShmMem4Open( STL_SIZEOF( smsmbCache ),
                                  (void**)&sNewCache,
                                  aEnv ) == STL_SUCCESS );

    STL_TRY( smsmbInitCache( sNewCache, aEnv ) == STL_SUCCESS );

    stlMemcpy( &sNewCache->mCommonCache,
               &sOrgCache->mCommonCache,
               STL_SIZEOF( smsCommonCache ) );
    sNewCache->mCommonCache.mAllocExtCount = 0;
    smsSetSegRid( (void*)sNewCache, aNewSegRid );
    smsSetSegmentId( (void*)sNewCache, aNewSegRid );
    smsSetValidScn( (void*)sNewCache, smxlGetTransViewScn( aTransId ) );
    smsSetValidSeq( (void*)sNewCache, 0 );

    if( aNewSegHdr != NULL )
    {
        smsSetValidSeq( (void*)sNewCache, aNewSegHdr->mValidSeq );
    }
    
    *aNewSegHandle = (void*)sNewCache;
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sNewCache != NULL )
    {
        (void)smgFreeShmMem4Open( sNewCache, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smsmbCreate( smxlTransId    aTransId,
                       smlTbsId       aTbsId,
                       void         * aSegHandle,
                       stlInt32       aInitialExtents,
                       smlEnv       * aEnv )
{
    stlInt32     sInitialExtents = aInitialExtents;
    smxmTrans    sMiniTrans;
    stlInt32     sState = 0;
    smsmbCache * sCache;

    sCache = (smsmbCache*)aSegHandle;

    if( aInitialExtents == 0 )
    {
        sInitialExtents = STL_MAX( sCache->mCommonCache.mInitialExtents,
                                   sCache->mCommonCache.mMinExtents );
    }
    
    STL_TRY( smxmBegin( &sMiniTrans,
                        aTransId,
                        smsGetSegRid( aSegHandle ),
                        SMXM_ATTR_REDO,
                        aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( smsmbAllocNewExtent( aTransId,
                                  aTbsId,
                                  sCache,
                                  NULL, /* aFirstPid */
                                  aEnv ) == STL_SUCCESS );

    /**
     * Meta Extent를 제외한다.
     */
    sInitialExtents -= 1;

    STL_DASSERT( sInitialExtents > 0 );
    
    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    STL_TRY( smsmbAddNewExtent( aTransId,
                                aTbsId,
                                sCache,
                                sInitialExtents,
                                STL_TRUE,  /* aIsForCreate */
                                aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        /**
         * - Create 도중 실패하는 경우는 relation component undo logging으로
         *   Undo 된다.
         */
        (void)smxmCommit( &sMiniTrans, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smsmbDrop( smxlTransId   aTransId,
                     smlTbsId      aTbsId,
                     void        * aSegmentHandle,
                     smlEnv      * aEnv )
{
    smsmbCache     * sCache;
    smxmTrans        sMiniTrans;
    stlInt32         sState = 0;
    smlPid           sExtMapPageId;
    smpHandle        sPageHandle;
    smsmbExtMapHdr * sExtMapHdr;
    smsmbExtMap    * sExtMap;
    stlInt32         i;
    stlInt32         sExtDescIdx;
    stlInt32         sStartExtDescIdx = 0;
    stlInt32         sEndExtDescIdx = 0;
    stlInt32         sFreeExtCount = 0;
    stlBool          sIsSuccess;

    sCache = (smsmbCache*)aSegmentHandle;
    
    sExtMapPageId = sCache->mLstExtMap;
    
    sExtDescIdx = -1;
        
    while( sExtMapPageId != SMP_NULL_PID )
    {
        STL_TRY( smxmBegin( &sMiniTrans,
                            aTransId,
                            smsGetSegRid( aSegmentHandle ),
                            SMXM_ATTR_REDO,
                            aEnv ) == STL_SUCCESS );
        sState = 1;
        
        STL_TRY( smpAcquire( &sMiniTrans,
                             aTbsId,
                             sExtMapPageId,
                             KNL_LATCH_MODE_EXCLUSIVE,
                             &sPageHandle,
                             aEnv ) == STL_SUCCESS );

        sExtMapHdr = (smsmbExtMapHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );
        sExtMap = (smsmbExtMap*)((stlChar*)sExtMapHdr + STL_SIZEOF(smsmbExtMapHdr));

        if( sExtDescIdx == -1 )
        {
            sExtDescIdx = sExtMapHdr->mExtDescCount - 1;
        }
        
        /**
         * Bitmap Extent Map 페이지가 제일 마지막에 삭제되어야 하기 때문에
         * Backward 순으로 삭제한다.
         */

        sIsSuccess = STL_TRUE;
        sFreeExtCount = 0;
        
        for( i = sExtDescIdx; ( (i >= 0) &&
                                ((sExtDescIdx - i) < (KNL_ENV_MAX_LATCH_DEPTH/2)) ); i-- )
        {
            if( sExtMap->mExtId[i] != SML_INVALID_EXTID )
            {
                STL_TRY( smtFreeExtent( &sMiniTrans,
                                        aTbsId,
                                        sExtMap->mExtId[i],
                                        &sIsSuccess,
                                        aEnv ) == STL_SUCCESS );

                if( sIsSuccess == STL_FALSE )
                {
                    break;
                }

                if( sFreeExtCount == 0 )
                {
                    /**
                     * Free되는 최초 extent를 기록한다.
                     */
                    sStartExtDescIdx = i;
                }
                
                sEndExtDescIdx = i;
                sFreeExtCount++;

                sExtMap->mExtId[i] = SML_INVALID_EXTID;
            }
        }

        /**
         * 1개의 Extent라도 Free를 했다면 logging을 수행한다.
         */
        if( sFreeExtCount > 0 )
        {
            STL_TRY( smsmbWriteLogFreeExtMap( &sMiniTrans,
                                              aTbsId,
                                              sExtMapPageId,
                                              0,
                                              sStartExtDescIdx,
                                              sEndExtDescIdx,
                                              aEnv ) == STL_SUCCESS );
        }

        sExtDescIdx = i;
        
        /**
         * Extent Map의 끝까지 Extent를 free 했다면 link를 재조정한다.
         */
        if( i < 0 )
        {
            sExtMapPageId = sExtMapHdr->mPrvExtMap;

            /**
             * mLstExtMap을 이전 Extent Map으로 설정한다.
             */
            STL_TRY( smsmbWriteLogUnlinkLstExtMap( &sMiniTrans,
                                                   sCache,
                                                   sExtMapPageId,
                                                   aEnv )
                     == STL_SUCCESS );
        }
            
        sState = 0;
        STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
    }

    sCache->mLstExtMap = SMP_NULL_PID;

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smxmRollback( &sMiniTrans, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smsmbReset( smxlTransId   aTransId,
                      smlTbsId      aTbsId,
                      void        * aSegHandle,
                      stlBool       aFreePages,
                      smlEnv      * aEnv )
{
    smsmbCache  * sCache;
    stlInt32      i;
    
    sCache = (smsmbCache*)aSegHandle;

    /**
     * free pages
     */

    if( aFreePages == STL_TRUE )
    {
        if( sCache->mLstExtMap != SMP_NULL_PID )
        {
            STL_TRY( smsmbDrop( aTransId,
                                aTbsId,
                                aSegHandle,
                                aEnv )
                     == STL_SUCCESS );
        }
    }

    /**
     * reset handle
     */
    
    sCache->mCommonCache.mAllocExtCount = 0;
    
    sCache->mFstExtMap = SMP_NULL_PID;
    sCache->mLstExtMap = SMP_NULL_PID;
    sCache->mRootNode = SMP_NULL_PID;
    sCache->mMetaHwm = (smsmbHwm){ SMP_NULL_PID, -1 };

    for( i = 0; i < SMSMB_MAX_TREE_DEPTH; i++ )
    {
        sCache->mTreeHwm[i] = (smsmbHwm){ SMP_NULL_PID, -1 };
    }

    STL_TRY( knlInitLatch( &sCache->mLatch,
                           STL_TRUE,   /* aIsInShm */
                           KNL_ENV(aEnv) ) == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmbAllocPage( smxmTrans       * aMiniTrans,
                          smlTbsId          aTbsId,
                          void            * aSegmentHandle,
                          smpPageType       aPageType,
                          smpIsAgableFunc   aIsAgableFunc,
                          smpHandle       * aPageHandle,
                          smlEnv          * aEnv )
{
    smsmbCache    * sSegCache;
    smxmSavepoint   sSavepoint;
    smlPid          sDataPid;
    smlRid          sLeafRid;
    smpPhyHdr     * sPhyHdr;
    stlInt32        i;
    stlBool         sUseInsertablePageHint;
    smsHint       * sSegHint;
    smlPid          sStartPid;
    stlBool         sIsSuccess = STL_TRUE;

    sSegCache = (smsmbCache*)aSegmentHandle;
    sSegHint = smsFindSegHint( sSegCache,
                               smsGetValidScn( sSegCache ),
                               aEnv );

    sUseInsertablePageHint =
        knlGetPropertyBoolValueByID( KNL_PROPERTY_USE_INSERTABLE_PAGE_HINT,
                                     KNL_ENV( aEnv ) );

    STL_RAMP( RAMP_RETRY );
    
    if( (sSegHint != NULL) && (sUseInsertablePageHint == STL_TRUE) )
    {
        sStartPid = sSegHint->mHintPid;
    }
    else
    {
        sStartPid = sSegCache->mRootNode;
    }
    
    while( 1 )
    {
        for( i = 0; i < 2; i++ )
        {
            STL_TRY( smsmbFindFreePage( aMiniTrans,
                                        aTbsId,
                                        sStartPid,
                                        sUseInsertablePageHint,
                                        aIsAgableFunc,
                                        &sDataPid,
                                        &sLeafRid,
                                        aEnv )
                     == STL_SUCCESS );

            if( sDataPid != SMP_NULL_PID )
            {
                break;
            }

            /**
             * Insertable Hint를 사용하지 않았음에도 없다면
             */
            if( sUseInsertablePageHint == STL_FALSE )
            {
                break;
            }
            
            sStartPid = sSegCache->mRootNode;
            sUseInsertablePageHint = STL_FALSE;
        }

        if( sDataPid != SMP_NULL_PID )
        {
            break;
        }

        /**
         * Free Page가 없다면 확장한다.
         */
        STL_TRY( smsmbAddNewExtent( smxmGetTransId( aMiniTrans ),
                                    aTbsId,
                                    sSegCache,
                                    sSegCache->mCommonCache.mNextExtents,
                                    STL_FALSE, /* aIsForCreate */
                                    aEnv )
                 == STL_SUCCESS );

        sStartPid = sSegCache->mRootNode;
    }
                                
    STL_TRY( smxmMarkSavepoint( aMiniTrans,
                                &sSavepoint,
                                aEnv ) == STL_SUCCESS );

    STL_TRY( smpTryAcquire( aMiniTrans,
                            aTbsId,
                            sDataPid,
                            KNL_LATCH_MODE_EXCLUSIVE,
                            aPageHandle,
                            &sIsSuccess,
                            aEnv ) == STL_SUCCESS );

    /**
     * 아래와 같은 경우에는 재시도 한다.
     * - Try Latch에 실패한 경우
     * - 사용자가 지정한 Freeness보다 실제 Freeness가 충분하지 않은 경우
     * - Aging이 블가능한 경우
     */
    if( (sIsSuccess == STL_FALSE ) ||
        (SMP_FREENESS_FREE > smpGetFreeness(aPageHandle)) ||
        (smpIsAgable( aPageHandle, aIsAgableFunc, aEnv ) == STL_FALSE ) )
    {
        SML_SESS_ENV(aEnv)->mSegmentRetryCount++;
        STL_TRY( smxmRollbackToSavepoint( aMiniTrans,
                                          &sSavepoint,
                                          aEnv ) == STL_SUCCESS );
        
        STL_THROW( RAMP_RETRY );
    }
    
    if( sSegHint == NULL )
    {
        smsAddNewSegHint( sLeafRid.mPageId,
                          (void*)sSegCache,
                          smsGetValidScn( sSegCache ),
                          aEnv );
    }
    else
    {
        sSegHint->mHintPid = sLeafRid.mPageId;
    }

    sPhyHdr = (smpPhyHdr*)SMP_FRAME( aPageHandle );
    
    if( sPhyHdr->mPageType == SMP_PAGE_TYPE_UNFORMAT )
    {
        STL_TRY( smpInitHdr( aMiniTrans,
                             aTbsId,
                             aPageHandle,
                             sLeafRid,
                             aPageType,
                             smsGetSegmentId( aSegmentHandle ),
                             0, /* aScn */
                             aEnv ) == STL_SUCCESS );
    }
    
    STL_TRY( smsmbUpdatePageStatus( aMiniTrans,
                                    aTbsId,
                                    aPageHandle,
                                    SMP_FREENESS_INSERTABLE,
                                    0,
                                    aEnv ) == STL_SUCCESS );

    smpInitVolatileArea( aPageHandle );
                                    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmbGetInsertablePage( smxmTrans       * aMiniTrans,
                                  smlTbsId          aTbsId,
                                  void            * aSegmentHandle,
                                  smpPageType       aPageType,
                                  smpIsAgableFunc   aIsAgableFunc,
                                  smsSearchHint   * aSearchHint,
                                  smpHandle       * aPageHandle,
                                  stlBool         * aIsNewPage,
                                  smlEnv          * aEnv )
{
    STL_TRY( smsmbGetInsertablePageInternal( aMiniTrans,
                                             aTbsId,
                                             aSegmentHandle,
                                             aPageType,
                                             SMP_FREENESS_INSERTABLE,
                                             aIsAgableFunc,
                                             aSearchHint,
                                             aPageHandle,
                                             aIsNewPage,
                                             aEnv ) == STL_SUCCESS );
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmbGetInsertablePageInternal( smxmTrans       * aMiniTrans,
                                          smlTbsId          aTbsId,
                                          void            * aSegmentHandle,
                                          smpPageType       aPageType,
                                          smpFreeness       aFreeness,
                                          smpIsAgableFunc   aIsAgableFunc,
                                          smsSearchHint   * aSearchHint,
                                          smpHandle       * aPageHandle,
                                          stlBool         * aIsNewPage,
                                          smlEnv          * aEnv )
{
    smsmbCache         * sSegCache;
    smlPid               sDataPageId;
    smlRid               sLeafRid;
    smpPhyHdr          * sPhyHdr;
    smlPid               sStartPid;
    smsHint            * sSegHint;
    stlBool              sUseInsertablePageHint;
    smxmSavepoint        sSavepoint;
    
    sSegCache = (smsmbCache*)aSegmentHandle;
    sSegHint = smsFindSegHint( sSegCache,
                               smsGetValidScn( sSegCache ),
                               aEnv );
    *aIsNewPage = STL_FALSE;

    sUseInsertablePageHint =
        knlGetPropertyBoolValueByID( KNL_PROPERTY_USE_INSERTABLE_PAGE_HINT,
                                     KNL_ENV( aEnv ) );

    STL_RAMP( RAMP_RETRY );
    
    if( (sSegHint != NULL) && (sUseInsertablePageHint == STL_TRUE) )
    {
        if( aSearchHint->mIsValid == STL_FALSE )
        {
            sStartPid = sSegHint->mHintPid;
        }
        else
        {
            if( aSearchHint->mCurHintPos < aSearchHint->mHintCount )
            {
                STL_THROW( RAMP_SKIP_FINDING );
            }

            sStartPid = sSegCache->mRootNode;
        }
    }
    else
    {
        sStartPid = sSegCache->mRootNode;
    }
    
    while( 1 )
    {
        aSearchHint->mCurHintPos = 0;
        
        STL_TRY( smsmbFindInsertablePage( aMiniTrans,
                                          aTbsId,
                                          sSegCache,
                                          aFreeness,
                                          sStartPid,
                                          sUseInsertablePageHint,
                                          aIsAgableFunc,
                                          &aSearchHint->mHintCount,
                                          aSearchHint->mDataPid,
                                          aSearchHint->mLeafRid,
                                          aEnv ) == STL_SUCCESS );
        
        if( aSearchHint->mHintCount != 0 )
        {
            aSearchHint->mIsValid = STL_TRUE;
            break;
        }

        if( sStartPid == sSegCache->mRootNode )
        {
            STL_TRY( smsmbAddNewExtent( smxmGetTransId( aMiniTrans ),
                                        aTbsId,
                                        sSegCache,
                                        sSegCache->mCommonCache.mNextExtents,
                                        STL_FALSE, /* aIsForCreate */
                                        aEnv ) == STL_SUCCESS );
        }
        
        sStartPid = sSegCache->mRootNode;
    }

    STL_RAMP( RAMP_SKIP_FINDING );

    sDataPageId = aSearchHint->mDataPid[aSearchHint->mCurHintPos];
    sLeafRid = aSearchHint->mLeafRid[aSearchHint->mCurHintPos];
    aSearchHint->mCurHintPos++;
    
    STL_TRY( smxmMarkSavepoint( aMiniTrans,
                                &sSavepoint,
                                aEnv ) == STL_SUCCESS );
    
    STL_TRY( smpAcquire( aMiniTrans,
                         aTbsId,
                         sDataPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         aPageHandle,
                         aEnv ) == STL_SUCCESS );

    /**
     * 아래와 같은 경우에는 재시도 한다.
     * - 사용자가 지정한 Freeness보다 실제 Freeness가 충분하지 않은 경우
     * - FREE 페이지가 Agable하지 않은 경우
     */
    if( (aFreeness > smpGetFreeness(aPageHandle)) ||
        ( (smpGetFreeness(aPageHandle) == SMP_FREENESS_FREE) &&
          (smpIsAgable(aPageHandle, aIsAgableFunc, aEnv) == STL_FALSE) ) )
    {
        SML_SESS_ENV(aEnv)->mSegmentRetryCount++;
        STL_TRY( smxmRollbackToSavepoint( aMiniTrans,
                                          &sSavepoint,
                                          aEnv ) == STL_SUCCESS );
        STL_THROW( RAMP_RETRY );
    }
    
    if( sSegHint == NULL )
    {
        smsAddNewSegHint( sLeafRid.mPageId,
                          (void*)sSegCache,
                          smsGetValidScn( sSegCache ),
                          aEnv );
    }
    else
    {
        sSegHint->mHintPid = sLeafRid.mPageId;
    }

    sPhyHdr = (smpPhyHdr*)SMP_FRAME( aPageHandle );
    
    if( sPhyHdr->mPageType == SMP_PAGE_TYPE_UNFORMAT )
    {
        STL_TRY( smpInitHdr( aMiniTrans,
                             aTbsId,
                             aPageHandle,
                             sLeafRid,
                             aPageType,
                             smsGetSegmentId( aSegmentHandle ),
                             0, /* aScn */
                             aEnv ) == STL_SUCCESS );
        *aIsNewPage = STL_TRUE;
    }
    else
    {
        if( smpGetFreeness( aPageHandle ) == SMP_FREENESS_FREE )
        {
            *aIsNewPage = STL_TRUE;
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmbUpdatePageStatus( smxmTrans   * aMiniTrans,
                                 smlTbsId      aTbsId,
                                 smpHandle   * aPageHandle,
                                 smpFreeness   aFreeness,
                                 smlScn        aScn,
                                 smlEnv      * aEnv )
{
    smpHandle          sPageHandle;
    smlRid             sParentRid;
    smsmbLeafHdr     * sLeafHdr;
    smsmbLeaf        * sLeaf;
    smpPhyHdr        * sPhyHdr;
    smpFreeness        sBeforeFreeness;
    smpFreeness        sAfterFreeness;
    smpFreeness        sRepresentFreeness;
    smlScn             sBeforeScn;
    smlScn             sAfterScn;
    
    STL_TRY_THROW( smpGetFreeness( aPageHandle ) != aFreeness, RAMP_SUCCESS );
    
    sPhyHdr = (smpPhyHdr*)SMP_FRAME( aPageHandle );
    sParentRid = sPhyHdr->mParentRid;

    KNL_ASSERT( sParentRid.mOffset != SML_INVALID_RID_OFFSET,
                KNL_ENV(aEnv),
                SMP_FORMAT_PHYSICAL_HDR( sPhyHdr ) );
    
    STL_TRY( smpAcquire( aMiniTrans,
                         aTbsId,
                         sParentRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );
    
    sLeafHdr = (smsmbLeafHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );
    sLeaf = (smsmbLeaf*)((stlChar*)sLeafHdr + STL_SIZEOF(smsmbLeafHdr));

    STL_ASSERT( sLeaf->mDataPid[sParentRid.mOffset] == smpGetPageId( aPageHandle ) );
                
    sBeforeFreeness = smsmbGetFreeness( sLeaf->mFreeness,
                                        sParentRid.mOffset );
    sLeafHdr->mFreenessCount[sBeforeFreeness] -= 1;
    sBeforeScn = sLeaf->mScn[sParentRid.mOffset];
    
    smsmbSetFreeness( sLeaf->mFreeness,
                      sParentRid.mOffset,
                      aFreeness );
    sLeaf->mScn[sParentRid.mOffset] = aScn;
    
    sAfterFreeness = smsmbGetFreeness( sLeaf->mFreeness,
                                       sParentRid.mOffset );
    sLeafHdr->mFreenessCount[sAfterFreeness] += 1;
    sAfterScn = sLeaf->mScn[sParentRid.mOffset];

    STL_TRY( smsmbWriteLogUpdateLeafStatus( aMiniTrans,
                                            aTbsId,
                                            sParentRid.mPageId,
                                            sParentRid.mOffset,
                                            sBeforeFreeness,
                                            sAfterFreeness,
                                            sBeforeScn,
                                            sAfterScn,
                                            aEnv ) == STL_SUCCESS );

    sRepresentFreeness = smsmbRepresentFreeness( (void*)sLeafHdr,
                                                 STL_TRUE );

    STL_TRY( smsmbUpdatePageStatusInternal( aMiniTrans,
                                            aTbsId,
                                            &sPageHandle,
                                            sRepresentFreeness,
                                            aEnv )
             == STL_SUCCESS );
                                            
    STL_TRY( smpUpdateFreeness( aMiniTrans,
                                aTbsId,
                                aPageHandle,
                                aFreeness,
                                aEnv ) == STL_SUCCESS );
    
    STL_TRY( smpUpdateAgableScn( NULL,
                                 aPageHandle,
                                 aScn,
                                 aEnv ) == STL_SUCCESS );

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmbUpdatePageStatusInternal( smxmTrans   * aMiniTrans,
                                         smlTbsId      aTbsId,
                                         smpHandle   * aPageHandle,
                                         smpFreeness   aFreeness,
                                         smlEnv      * aEnv )
{
    smpHandle          sPageHandle;
    smlRid             sParentRid;
    smpPhyHdr        * sPhyHdr;
    smpFreeness        sBeforeFreeness;
    smpFreeness        sAfterFreeness;
    smsmbInternalHdr * sInternalHdr;
    smsmbInternal    * sInternal;
    smpFreeness        sRepresentFreeness;

    sPageHandle = *aPageHandle;
    sRepresentFreeness = aFreeness;
    
    while( 1 )
    {
        sPhyHdr = (smpPhyHdr*)SMP_FRAME( &sPageHandle );
        sParentRid = sPhyHdr->mParentRid;

        if( smpGetFreeness( &sPageHandle ) == sRepresentFreeness )
        {
            break;
        }

        STL_TRY( smpUpdateFreeness( aMiniTrans,
                                    aTbsId,
                                    &sPageHandle,
                                    sRepresentFreeness,
                                    aEnv ) == STL_SUCCESS );

        if( SML_IS_INVALID_RID( sParentRid ) )
        {
            break;
        }
        
        STL_TRY( smpAcquire( aMiniTrans,
                             aTbsId,
                             sParentRid.mPageId,
                             KNL_LATCH_MODE_EXCLUSIVE,
                             &sPageHandle,
                             aEnv ) == STL_SUCCESS );

        KNL_ASSERT( (smpGetPageType( &sPageHandle ) == SMP_PAGE_TYPE_BITMAP_INTERNAL) ||
                    (smpGetPageType( &sPageHandle ) == SMP_PAGE_TYPE_BITMAP_LEAF),
                    aEnv,
                    SMP_FORMAT_PHYSICAL_HDR( (smpPhyHdr*)SMP_FRAME(&sPageHandle) ) );
    
        sInternalHdr = (smsmbInternalHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );
        sInternal = (smsmbInternal*)((stlChar*)sInternalHdr +
                                     STL_SIZEOF(smsmbInternalHdr));
    
        sBeforeFreeness = smsmbGetFreeness( sInternal->mFreeness,
                                            sParentRid.mOffset );
        sInternalHdr->mFreenessCount[sBeforeFreeness] -= 1;
    
        smsmbSetFreeness( sInternal->mFreeness,
                          sParentRid.mOffset,
                          aFreeness );
    
        sAfterFreeness = smsmbGetFreeness( sInternal->mFreeness,
                                           sParentRid.mOffset );
        sInternalHdr->mFreenessCount[sAfterFreeness] += 1;

        STL_TRY( smsmbWriteLogUpdateInternalStatus( aMiniTrans,
                                                    aTbsId,
                                                    sParentRid.mPageId,
                                                    sParentRid.mOffset,
                                                    sBeforeFreeness,
                                                    sAfterFreeness,
                                                    aEnv ) == STL_SUCCESS );
        
        sRepresentFreeness = smsmbRepresentFreeness( (void*)sInternalHdr,
                                                     STL_FALSE );
    
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

smpFreeness smsmbRepresentFreeness( void    * aHeader,
                                    stlBool   aIsLeaf )
{
    stlInt16    * sFreenessCount;
    smpFreeness   sRepresentFreeness = SMP_FREENESS_UNALLOCATED;
    stlInt32      i;

    if( aIsLeaf == STL_TRUE )
    {
        sFreenessCount = ((smsmbLeafHdr*)aHeader)->mFreenessCount;
    }
    else
    {
        sFreenessCount = ((smsmbInternalHdr*)aHeader)->mFreenessCount;
    }

    for( i = SMP_FREENESS_MAX - 1; i > 0; i-- )
    {
        if( sFreenessCount[i] > 0 )
        {
            sRepresentFreeness = (smpFreeness)i;
            break;
        }
    }

    return sRepresentFreeness;
}

void smsmbSetFreeness( stlChar     * aBitArray,
                       stlInt32      aOffset,
                       smpFreeness   aFreeness )
{
    stlChar     * sBit;
    stlChar       sBitMask;
    smpFreeness   sFreeness;

    sBit = aBitArray + ( aOffset / (STL_BIT_COUNT(stlChar) / 4) );

    sBitMask = 0xF << ((aOffset * 4) % STL_BIT_COUNT(stlChar));
    sFreeness = aFreeness << ((aOffset * 4) % STL_BIT_COUNT(stlChar));
    *sBit &= (~sBitMask);
    *sBit |= (sBitMask & sFreeness);
}

smpFreeness smsmbGetFreeness( stlChar  * aBitArray,
                              stlInt32   aOffset )
{
    stlChar     * sBit;
    stlChar       sBitMask;
    smpFreeness   sFreeness;

    sBit = aBitArray + ( aOffset / (STL_BIT_COUNT(stlChar) / 4) );

    sBitMask = 0xF << ((aOffset * 4) % STL_BIT_COUNT(stlChar));
    sFreeness = *sBit & sBitMask;
    sFreeness = sFreeness >> ((aOffset * 4) % STL_BIT_COUNT(stlChar));

    return sFreeness;
}

stlBool smsmbIsAgableForAlloc( smlTbsId          aTbsId,
                               smlScn            aPageScn,
                               smlPid            aPageId,
                               smpIsAgableFunc   aIsAgableFunc,
                               smlEnv          * aEnv )
{
    stlBool   sIsAgable = STL_FALSE;
    stlBool   sIsSuccess;
    smpHandle sPageHandle;
    
    if( aPageScn == 0 )
    {
        return STL_TRUE;
    }
    
    /**
     * 페이지를 삭제할 당시 봐야하는 Transaction과 statement들이
     * 모두 종료되었을때 재사용한다.
     */
    
    if( SMXL_IS_AGABLE_VIEW_SCN( smxlGetAgableViewScn(aEnv), aPageScn ) == STL_TRUE )
    {
        return STL_TRUE;
    }

    (void) smpTryAcquire( NULL,  /* aMiniTrans */
                          aTbsId,
                          aPageId,
                          KNL_LATCH_MODE_SHARED,
                          &sPageHandle,
                          &sIsSuccess,
                          aEnv );

    if( sIsSuccess == STL_TRUE )
    {
        sIsAgable = smpIsAgable( &sPageHandle,
                                 aIsAgableFunc,
                                 aEnv );

        (void) smpRelease( &sPageHandle, aEnv );
    }

    return sIsAgable;
}

stlBool smsmbIsAgableForScan( smlTbsId   aTbsId,
                              smlScn     aPageScn,
                              smlEnv   * aEnv )
{
    if( aPageScn == 0 )
    {
        return STL_TRUE;
    }
    
    /**
     * 페이지를 삭제할 당시 봐야하는 Transaction과 statement들이
     * 모두 종료되었을때 재사용한다.
     */
    
    if( SMXL_IS_AGABLE_VIEW_SCN( smxlGetAgableViewScn(aEnv), aPageScn ) == STL_TRUE )
    {
        return STL_TRUE;
    }

    return STL_FALSE;
}

stlStatus smsmbAddNewExtent( smxlTransId   aTransId,
                             smlTbsId      aTbsId,
                             smsmbCache  * aCache,
                             stlInt32      aExtCount,
                             stlBool       aIsForCreate,
                             smlEnv      * aEnv )
{
    smxmTrans     sMiniTrans;
    smlPid        sFirstPid;
    stlInt32      sState = 0;
    smsmbSegHdr * sSegHdr;
    smpHandle     sPageHandle;
    stlInt32      i;
    smlRid        sLeafRid;
    stlUInt32     sAttr = SMXM_ATTR_REDO;

    if( smtIsLogging( aTbsId ) == STL_TRUE )
    {
        sAttr = SMXM_ATTR_REDO;
    }
    else
    {
        sAttr = SMXM_ATTR_NOLOG_DIRTY;
    }
    
    for( i = 0; i < aExtCount; i++ )
    {
        STL_TRY( smxmBegin( &sMiniTrans,
                            aTransId,
                            smsGetSegRid( aCache ),
                            sAttr,
                            aEnv ) == STL_SUCCESS );
        sState = 1;

        STL_TRY( smpAcquire( &sMiniTrans,
                             aCache->mCommonCache.mSegRid.mTbsId,
                             aCache->mCommonCache.mSegRid.mPageId,
                             KNL_LATCH_MODE_EXCLUSIVE,
                             &sPageHandle,
                             aEnv ) == STL_SUCCESS );

        sSegHdr = (smsmbSegHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );

        STL_TRY( smsmbAllocNewExtent( aTransId,
                                      aTbsId,
                                      aCache,
                                      &sFirstPid,
                                      aEnv ) == STL_SUCCESS );

        STL_TRY( smsmbPropagateLeaf( &sMiniTrans,
                                     aTbsId,
                                     aCache,
                                     sFirstPid,
                                     &sLeafRid,
                                     aEnv ) == STL_SUCCESS );

        SMSMB_COPY_SEGHDR_FROM_CACHE( sSegHdr, aCache );

        /**
         * Segment단의 로깅은 mini-transaction의 Attribute와 상관없이 로깅이
         * 이루어질수 있다.
         * No Logging Tablespace의 경우는 강제적으로 로깅을 하지 않는다.
         * - 단, 최초 세그먼트 생성시에는 segment 상태를 로깅해야 한다.
         */
        if( (aIsForCreate == STL_TRUE) || (sAttr == SMXM_ATTR_REDO) )
        {
            STL_TRY( smsmbWriteLogUpdateSegHdr( &sMiniTrans,
                                                aCache->mCommonCache.mSegRid.mTbsId,
                                                aCache->mCommonCache.mSegRid.mPageId,
                                                0,
                                                sSegHdr,
                                                aEnv ) == STL_SUCCESS );
        }
        
        sState = 0;
        STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)smxmRollback( &sMiniTrans, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}

stlStatus smsmbAllocExtent( smxlTransId   aTransId,
                            smlTbsId      aTbsId,
                            void        * aCache,
                            smpPageType   aPageType,
                            smlPid      * aFirstDataPid,
                            smlEnv      * aEnv )
{
    smxmTrans     sMiniTrans;
    smlPid        sFirstPid;
    stlInt32      sState = 0;
    smsmbSegHdr * sSegHdr;
    smpHandle     sPageHandle;
    smsmbCache  * sCache;
    stlUInt16     i;
    stlUInt32     sPageCountInExt;
    smlRid        sLeafRid;
    smlRid        sParentRid;
    stlInt64      sSegmentId;
    stlUInt32     sAttr = SMXM_ATTR_REDO;

    STL_PARAM_VALIDATE( aFirstDataPid != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aCache != NULL, KNL_ERROR_STACK( aEnv ) );

    sPageCountInExt = smfGetPageCountInExt( aTbsId );
    sSegmentId = smsGetSegmentId( aCache );
    
    sCache = (smsmbCache*)aCache;
    
    if( smtIsLogging( aTbsId ) == STL_TRUE )
    {
        sAttr = SMXM_ATTR_REDO;
    }
    else
    {
        sAttr = SMXM_ATTR_NOLOG_DIRTY;
    }
    
    STL_TRY( smxmBegin( &sMiniTrans,
                        aTransId,
                        smsGetSegRid( aCache ),
                        sAttr,
                        aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( smpAcquire( &sMiniTrans,
                         sCache->mCommonCache.mSegRid.mTbsId,
                         sCache->mCommonCache.mSegRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

    sSegHdr = (smsmbSegHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );

    STL_TRY( smsmbAllocNewExtent( aTransId,
                                  aTbsId,
                                  sCache,
                                  &sFirstPid,
                                  aEnv ) == STL_SUCCESS );

    STL_TRY( smsmbPropagateLeaf( &sMiniTrans,
                                 aTbsId,
                                 sCache,
                                 sFirstPid,
                                 &sLeafRid,
                                 aEnv ) == STL_SUCCESS );

    SMSMB_COPY_SEGHDR_FROM_CACHE( sSegHdr, sCache );
        
    /**
     * Segment단의 로깅은 mini-transaction의 Attribute와 상관없이 로깅이
     * 이루어질수 있다.
     * No Logging Tablespace의 경우는 강제적으로 로깅을 하지 않는다.
     */
    if( sAttr == SMXM_ATTR_REDO )
    {
        STL_TRY( smsmbWriteLogUpdateSegHdr( &sMiniTrans,
                                            sCache->mCommonCache.mSegRid.mTbsId,
                                            sCache->mCommonCache.mSegRid.mPageId,
                                            0,
                                            sSegHdr,
                                            aEnv ) == STL_SUCCESS );
    }
    
    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    /**
     * 페이지 freeness설정과 초기화
     */

    for( i = 0; i < sPageCountInExt; i++ )
    {
        STL_TRY( smxmBegin( &sMiniTrans,
                            aTransId,
                            smsGetSegRid( aCache ),
                            SMXM_ATTR_REDO |
                            SMXM_ATTR_NO_VALIDATE_PAGE,
                            aEnv ) == STL_SUCCESS );
        sState = 1;
    
        STL_TRY( smpCreate( &sMiniTrans,
                            aTbsId,
                            sFirstPid + i,
                            aPageType,
                            sSegmentId,
                            0,  /* aScn */
                            &sPageHandle,
                            aEnv ) == STL_SUCCESS );

        sParentRid.mTbsId  = sLeafRid.mTbsId;
        sParentRid.mPageId = sLeafRid.mPageId;
        sParentRid.mOffset = sLeafRid.mOffset + i;

        STL_TRY( smpInitHdr( &sMiniTrans,
                             aTbsId,
                             &sPageHandle,
                             sParentRid,
                             aPageType,
                             sSegmentId,
                             0, /* aScn */
                             aEnv ) == STL_SUCCESS );
        
        STL_TRY( smsmbUpdatePageStatus( &sMiniTrans,
                                        aTbsId,
                                        &sPageHandle,
                                        SMP_FREENESS_INSERTABLE,
                                        0,
                                        aEnv ) == STL_SUCCESS );
        sState = 0;
        STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
    }
    
    *aFirstDataPid = sFirstPid;
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)smxmRollback( &sMiniTrans, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}

stlInt32 smsmbGetHeaderSize()
{
    return STL_SIZEOF( smsmbSegHdr );
}

stlInt32 smsmbGetCacheSize()
{
    return STL_SIZEOF( smsmbCache );
}

stlStatus smsmbGetUsedPageCount( void       * aSegmentHandle,
                                 stlInt64   * aUsedPageCount,
                                 smlEnv     * aEnv )
{
    smsmbCache    * sCache;
    smpHandle       sPageHandle;
    smpPageType     sPageType;
    stlInt32        sState = 0;
    smsmbLeafHdr  * sLeafHdr;
    smlTbsId        sTbsId;
    stlInt64        sUsedPageCount = 0;
    
    sCache = (smsmbCache*)aSegmentHandle;

    sTbsId = smsGetTbsId( aSegmentHandle );

    if( SMF_IS_ONLINE_TBS(sTbsId) == STL_TRUE )
    {
        STL_TRY( smpFix( sTbsId,
                         sCache->mRootNode,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );
        sState = 1;

        sPageType = smpGetPageType( &sPageHandle );

        if( sPageType == SMP_PAGE_TYPE_BITMAP_INTERNAL )
        {
            sUsedPageCount = ( sCache->mCommonCache.mAllocExtCount *
                               smfGetPageCountInExt( sTbsId ) );
        }
        else
        {
            sLeafHdr = (smsmbLeafHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );

            sUsedPageCount = ( sLeafHdr->mFreenessCount[SMP_FREENESS_FULL] +
                               sLeafHdr->mFreenessCount[SMP_FREENESS_UPDATEONLY] +
                               sLeafHdr->mFreenessCount[SMP_FREENESS_INSERTABLE] );
        }

        sState = 0;
        STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
    }
    else
    {
        sUsedPageCount = ( sCache->mCommonCache.mAllocExtCount *
                           smfGetPageCountInExt( sTbsId ) );
    }
    
    *aUsedPageCount = sUsedPageCount;
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) smpUnfix( &sPageHandle, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smsmbInitCache( smsmbCache * aCache,
                          smlEnv     * aEnv )
{
    stlInt32 i;

    aCache->mCommonCache.mInitialExtents = 0;
    aCache->mCommonCache.mNextExtents = 0;
    aCache->mCommonCache.mMinExtents = 0;
    aCache->mCommonCache.mMaxExtents = 0;
    aCache->mCommonCache.mAllocExtCount = 0;
    aCache->mCommonCache.mIsUsable = STL_TRUE;

    aCache->mFstExtMap = SMP_NULL_PID;
    aCache->mLstExtMap = SMP_NULL_PID;
    aCache->mRootNode = SMP_NULL_PID;
    aCache->mMetaHwm = (smsmbHwm){ SMP_NULL_PID, -1 };

    for( i = 0; i < SMSMB_MAX_TREE_DEPTH; i++ )
    {
        aCache->mTreeHwm[i] = (smsmbHwm){ SMP_NULL_PID, -1 };
    }

    STL_TRY( knlInitLatch( &aCache->mLatch,
                           STL_TRUE,   /* aIsInShm */
                           KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
