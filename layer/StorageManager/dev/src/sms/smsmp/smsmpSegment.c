/*******************************************************************************
 * smsmpSegment.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smsmpSegment.c 5876 2012-10-08 09:27:01Z egon $
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
#include <smg.h>
#include <sms.h>
#include <smf.h>
#include <smsDef.h>
#include <smsmpDef.h>
#include <smsmpSegment.h>
#include <smsmpScan.h>
#include <smsManager.h>
#include <smsmpLog.h>

/**
 * @file smsmpSegment.c
 * @brief Storage Manager Layer Pending Segment Internal Routines
 */

/**
 * @addtogroup smsmpSegment
 * @{
 */

smsSegmentFuncs gMemoryPendingSegment =
{
    smsmpAllocHandle,       /* alloc handle */
    NULL,                   /* clone handle */
    smsmpCreate,            /* create segment */
    smsmpDrop,              /* drop segment */
    NULL,                   /* insert pending drop */
    smsmpReset,             /* reset */
    NULL,                   /* alloc page */
    NULL,                   /* prepare Page */
    NULL,                   /* alloc multiple page */
    NULL,                   /* steal pages */
    smsmpGetInsertablePage, /* get insertable page */
    smsmpUpdatePageStatus,  /* update page status */
    smsmpAllocSegIterator,  /* alloc segment iterator */
    smsmpFreeSegIterator,   /* free segment iterator */
    smsmpGetFirstPage,      /* get first page */
    smsmpGetNextPage,       /* get next page */
    smsmpBuildCache,        /* build cache */
    NULL,                   /* alloc extent */
    smsmpGetHeaderSize,     /* get header size */
    smsmpGetCacheSize,      /* get cache size */
    NULL,                   /* get used page count for statistics */
    NULL,                   /* shrink */
    NULL                    /* SetTransToHandle */
};

stlStatus smsmpAllocHandle( smxlTransId       aTransId,
                            smlRid            aSegRid,
                            smlTbsId          aTbsId,
                            smlSegmentAttr  * aAttr,
                            stlBool           aHasRelHdr,
                            smsSegHdr       * aSegHdr,
                            void           ** aSegmentHandle,
                            smlEnv          * aEnv )
{
    stlInt32      sState = 0;
    smsmpCache  * sCache;
    stlUInt16     sPageCountInExt;

    STL_DASSERT( aTbsId == SML_MEMORY_DICT_SYSTEM_TBS_ID );
    
    STL_TRY( smgAllocShmMem4Open( STL_SIZEOF( smsmpCache ),
                                  aSegmentHandle,
                                  aEnv ) == STL_SUCCESS );
    sState = 1;

    sCache = (smsmpCache*)(*aSegmentHandle);
    sPageCountInExt = smfGetPageCountInExt( aTbsId );
    
    stlMemset( sCache, 0x00, STL_SIZEOF( smsmpCache ) );

    sCache->mFirstHdrPid = SMP_NULL_PID;
    sCache->mCurDataPid = SMP_NULL_PID;
    sCache->mCurHdrPid = SMP_NULL_PID;
    sCache->mCommonCache.mAllocExtCount = 0;
    sCache->mCommonCache.mIsUsable = STL_TRUE;

    smsSetSegRid( (void*)sCache, aSegRid );
    smsSetSegmentId( (void*)sCache, aSegRid );
    smsSetSegType( (void*)sCache, SML_SEG_TYPE_MEMORY_PENDING );
    smsSetTbsId( (void*)sCache, aTbsId );
    smsSetHasRelHdr( (void*)sCache, aHasRelHdr );
    smsSetUsablePageCountInExt( (void*)sCache, smfGetPageCountInExt(aTbsId) - 1 );

    sCache->mCommonCache.mInitialExtents = 1;
    sCache->mCommonCache.mNextExtents = 1;
    sCache->mCommonCache.mMinExtents = 1;
    sCache->mCommonCache.mMaxExtents = (((stlInt64)sPageCountInExt) *
                                        SMP_PAGE_SIZE *
                                        STL_INT32_MAX);

    return STL_SUCCESS;
    
    STL_FINISH;

    if( sState == 1 )
    {
        (void)smgFreeShmMem4Open( *aSegmentHandle, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smsmpCreate( smxlTransId    aTransId,
                       smlTbsId       aTbsId,
                       void         * aSegmentHandle,
                       stlInt32       aInitialExtents,
                       smlEnv       * aEnv )
{
    smxmTrans     sMiniTrans;
    smlPid        sFirstDataPid;
    smlExtId      sFirstExtId;
    smsmpExtHdr * sFirstHdr;
    smpHandle     sFirstPageHandle;
    smpHandle     sPageHandle;
    stlInt32      sState = 0;
    smsmpCache  * sCache;
    stlInt32      sAllocExtCount = 0;

    STL_TRY( smxmBegin( &sMiniTrans,
                        aTransId,
                        SML_INVALID_RID,
                        SMXM_ATTR_REDO,
                        aEnv ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( smtAllocExtent( &sMiniTrans,
                             aTbsId,
                             &sFirstExtId,
                             &sFirstDataPid,
                             aEnv ) == STL_SUCCESS );
    sAllocExtCount++;

    STL_TRY( smpCreate( &sMiniTrans,
                        aTbsId,
                        sFirstDataPid,
                        SMP_PAGE_TYPE_PENDING_HEADER,
                        smsGetSegmentId( aSegmentHandle ),
                        0, /* aScn */
                        &sFirstPageHandle,
                        aEnv ) == STL_SUCCESS );
    sFirstHdr = (smsmpExtHdr*)smpGetBody( SMP_FRAME( &sFirstPageHandle ) );
    
    sFirstHdr->mNextHdrPid = SMP_NULL_PID;
    sFirstHdr->mExtId = sFirstExtId;

    STL_TRY( smsmpWriteLogUpdateExtHdr( &sMiniTrans,
                                        aTbsId,
                                        sFirstDataPid,
                                        sFirstHdr,
                                        aEnv ) == STL_SUCCESS );
    
    STL_TRY( smpCreate( &sMiniTrans,
                        aTbsId,
                        sFirstDataPid + 1,
                        SMP_PAGE_TYPE_UNFORMAT,
                        smsGetSegmentId( aSegmentHandle ),
                        0, /* aScn */
                        &sPageHandle,
                        aEnv ) == STL_SUCCESS );

    sCache = (smsmpCache*)(aSegmentHandle);

    sCache->mFirstHdrPid = sFirstDataPid;
    sCache->mCurDataPid = SMP_NULL_PID;
    sCache->mCurHdrPid = sFirstDataPid;
    sCache->mCommonCache.mAllocExtCount = sAllocExtCount;

    STL_TRY( smsmpWriteLogUpdateSegHdr( &sMiniTrans,
                                        smsGetSegRid(aSegmentHandle),
                                        sCache,
                                        aEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    if( sState == 1 )
    {
        (void)smxmRollback( &sMiniTrans, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smsmpDrop( smxlTransId   aTransId,
                     smlTbsId      aTbsId,
                     void        * aSegmentHandle,
                     smlEnv      * aEnv )
{
    smsmpCache     * sCache;
    smxmTrans        sMiniTrans;
    stlInt32         sState = 0;
    smpHandle        sPageHandle;
    smlPid           sExtHdrPid;
    smsmpExtHdr    * sExtHdr;
    stlBool          sIsSuccess;
    
    sCache = (smsmpCache*)aSegmentHandle;
    sExtHdrPid = sCache->mFirstHdrPid;

    while( sExtHdrPid != SMP_NULL_PID )
    {
        STL_TRY( smxmBegin( &sMiniTrans,
                            aTransId,
                            SML_INVALID_RID,
                            SMXM_ATTR_REDO,
                            aEnv ) == STL_SUCCESS );
        sState = 1;
        
        STL_TRY( smpAcquire( &sMiniTrans,
                             aTbsId,
                             sExtHdrPid,
                             KNL_LATCH_MODE_EXCLUSIVE,
                             &sPageHandle,
                             aEnv ) == STL_SUCCESS );
        
        sExtHdr = (smsmpExtHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );

        sIsSuccess = STL_TRUE;

        if( sExtHdr->mExtId != SML_INVALID_EXTID )
        {
            STL_TRY( smtFreeExtent( &sMiniTrans,
                                    aTbsId,
                                    sExtHdr->mExtId,
                                    &sIsSuccess,
                                    aEnv ) == STL_SUCCESS );

            /**
             * Extent Free가 성공했다면 다음 extent를 Free한다.
             */
            if( sIsSuccess == STL_TRUE )
            {
                sExtHdrPid = sExtHdr->mNextHdrPid;
                sExtHdr->mExtId = SML_INVALID_EXTID;
            }
        }
        else
        {
            /**
             * 이미 지워진 Extent라면 다음 extent를 free한다.
             */
            sExtHdrPid = sExtHdr->mNextHdrPid;
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

stlStatus smsmpReset( smxlTransId   aTransId,
                      smlTbsId      aTbsId,
                      void        * aSegmentHandle,
                      stlBool       aFreePages,
                      smlEnv      * aEnv )
{
    smsmpCache  * sCache;
    smxmTrans     sMiniTrans;
    stlInt32      sState = 0;
    smlRid        sUndoRid;
    smlRid        sSegRid;
    smpHandle     sPageHandle;
    smsmpSegHdr * sSegHdr;
    smlPid        sData[2];
    
    sCache = (smsmpCache*)aSegmentHandle;
    sSegRid = smsGetSegRid( aSegmentHandle );

    STL_TRY( smxmBegin( &sMiniTrans,
                        aTransId,
                        SML_INVALID_RID,
                        SMXM_ATTR_REDO,
                        aEnv ) == STL_SUCCESS );
    sState = 1;
    
    sData[0] = sCache->mCurHdrPid;
    sData[1] = sCache->mCurDataPid;
    
    STL_TRY( smxlInsertUndoRecord( &sMiniTrans,
                                   SMG_COMPONENT_SEGMENT,
                                   SMS_UNDO_LOG_MEMORY_RESET_PENDING_SEGMENT,
                                   (stlChar*)sData,
                                   STL_SIZEOF(smlPid) * 2,
                                   smsGetSegRid( aSegmentHandle ),
                                   &sUndoRid,
                                   aEnv ) == STL_SUCCESS );

    sCache->mCurHdrPid = sCache->mFirstHdrPid;
    sCache->mCurDataPid = SMP_NULL_PID;
    
    STL_TRY( smpAcquire( &sMiniTrans,
                         sSegRid.mTbsId,
                         sSegRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

    sSegHdr = (smsmpSegHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );

    sData[0] = sSegHdr->mCurHdrPid;
    sData[1] = sCache->mCurHdrPid;
    
    STL_TRY( smxmWriteLog( &sMiniTrans,
                           SMG_COMPONENT_RECOVERY,
                           SMR_LOG_BYTES,
                           SMR_REDO_TARGET_PAGE,
                           (void*)sData,
                           STL_SIZEOF( smlPid ) * 2,
                           sSegRid.mTbsId,
                           sSegRid.mPageId,
                           ((void*)&(sSegHdr->mCurHdrPid)) - SMP_FRAME(&sPageHandle),
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );
        
    sData[0] = sSegHdr->mCurDataPid;
    sData[1] = sCache->mCurDataPid;
    
    STL_TRY( smxmWriteLog( &sMiniTrans,
                           SMG_COMPONENT_RECOVERY,
                           SMR_LOG_BYTES,
                           SMR_REDO_TARGET_PAGE,
                           (void*)sData,
                           STL_SIZEOF( smlPid ) * 2,
                           sSegRid.mTbsId,
                           sSegRid.mPageId,
                           ((void*)&(sSegHdr->mCurDataPid)) - SMP_FRAME(&sPageHandle),
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );
        
    sSegHdr->mCurHdrPid = sCache->mCurHdrPid;
    sSegHdr->mCurDataPid = sCache->mCurDataPid;
    
    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
    
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

stlStatus smsmpAllocExtent( smsmpCache  * aCache,
                            smxlTransId   aTransId,
                            smlTbsId      aTbsId,
                            smlPid      * aFirstPid,
                            smlEnv      * aEnv )
{
    smxmTrans      sMiniTrans;
    smlPid         sFirstPid;
    smlExtId       sExtId;
    smsmpExtHdr  * sCurHdr;
    smsmpExtHdr  * sPrvHdr;
    smpHandle      sCurPageHandle;
    smpHandle      sPrvPageHandle;
    stlInt32       sState = 0;
    smlPid         sNextHdrPid;

    STL_TRY( smxmBegin( &sMiniTrans,
                        aTransId,
                        SML_INVALID_RID,
                        SMXM_ATTR_REDO | SMXM_ATTR_NO_VALIDATE_PAGE,
                        aEnv ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( smpFix( aTbsId,
                     aCache->mCurHdrPid,
                     &sPrvPageHandle,
                     aEnv ) == STL_SUCCESS );
    sState = 2;
    
    sPrvHdr = (smsmpExtHdr*)smpGetBody( SMP_FRAME( &sPrvPageHandle ) );
    sNextHdrPid = sPrvHdr->mNextHdrPid;

    sState = 1;
    STL_TRY( smpUnfix( &sPrvPageHandle, aEnv ) == STL_SUCCESS );
    
    if( sNextHdrPid != SMP_NULL_PID )
    {
        sFirstPid = sNextHdrPid;
    }
    else
    {
        STL_TRY( smtAllocExtent( &sMiniTrans,
                                 aTbsId,
                                 &sExtId,
                                 &sFirstPid,
                                 aEnv ) == STL_SUCCESS );
        aCache->mCommonCache.mAllocExtCount++;
        
        STL_TRY( smpCreate( &sMiniTrans,
                            aTbsId,
                            sFirstPid,
                            SMP_PAGE_TYPE_FLAT_HEADER,
                            0, /* aSegmentId */
                            0, /* aScn */
                            &sCurPageHandle,
                            aEnv ) == STL_SUCCESS );
        sCurHdr = (smsmpExtHdr*)smpGetBody( SMP_FRAME( &sCurPageHandle ) );
    
        STL_TRY( smpAcquire( &sMiniTrans,
                             aTbsId,
                             aCache->mCurHdrPid,
                             KNL_LATCH_MODE_EXCLUSIVE,
                             &sPrvPageHandle,
                             aEnv ) == STL_SUCCESS );
        sPrvHdr = (smsmpExtHdr*)smpGetBody( SMP_FRAME( &sPrvPageHandle ) );

        sCurHdr->mNextHdrPid = SMP_NULL_PID;
        sPrvHdr->mNextHdrPid = sFirstPid;
        
        STL_TRY( smsmpWriteLogUpdateExtHdr( &sMiniTrans,
                                            aTbsId,
                                            smpGetPageId( &sCurPageHandle ),
                                            sCurHdr,
                                            aEnv ) == STL_SUCCESS );

        STL_TRY( smsmpWriteLogUpdateExtHdr( &sMiniTrans,
                                            aTbsId,
                                            smpGetPageId( &sPrvPageHandle ),
                                            sPrvHdr,
                                            aEnv ) == STL_SUCCESS );
    }

    *aFirstPid = sFirstPid;

    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) smpUnfix( &sPrvPageHandle, aEnv );
        case 1:
            (void)smxmRollback( &sMiniTrans, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus smsmpAllocPage( smxmTrans   * aMiniTrans,
                          smlTbsId      aTbsId,
                          void        * aSegmentHandle,
                          smpPageType   aPageType,
                          smpHandle   * aPageHandle,
                          smlEnv      * aEnv )
{
    smsmpCache  * sCache;
    smlPid        sFirstPid;
    smlPid        sNewPageId;
    smpPhyHdr   * sPhyHdr;
    stlUInt16     sPageCountInExt;
    
    sCache = (smsmpCache*)aSegmentHandle;
    sPageCountInExt = smfGetPageCountInExt( aTbsId );

    if( sCache->mCurDataPid == SMP_NULL_PID )
    {
        /**
         * create 또는 reset된 이후에 사용된적이 없는 경우
         */
        sNewPageId = sCache->mCurHdrPid + 1;
    }
    else
    {
        if( (sCache->mCurDataPid - sCache->mCurHdrPid + 1) >= sPageCountInExt )
        {
            /*
             * 현재 Extent에 더 이상 빈 Page가 존재하지 않을 경우
             * 새 Extent를 할당받아서, 기존의 Extent list에 연결한다.
             */
            STL_TRY( smsmpAllocExtent( sCache,
                                       smxmGetTransId( aMiniTrans ),
                                       aTbsId,
                                       &sFirstPid,
                                       aEnv ) == STL_SUCCESS );
            sCache->mCurHdrPid = sFirstPid;
            sNewPageId = sFirstPid + 1;
        }
        else
        {
            /*
             * 현재 Extent에 빈 Page가 존재할 경우
             */
            sNewPageId = sCache->mCurDataPid + 1;
        }
    }

    sCache->mCurDataPid = sNewPageId;
    
    STL_TRY( smpAcquire( aMiniTrans,
                         aTbsId,
                         sNewPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         aPageHandle,
                         aEnv ) == STL_SUCCESS );
    
    sPhyHdr = (smpPhyHdr*)SMP_FRAME( aPageHandle );
    if( sPhyHdr->mPageType == SMP_PAGE_TYPE_UNFORMAT )
    {
        STL_TRY( smpInitHdr( aMiniTrans,
                             aTbsId,
                             aPageHandle,
                             SML_INVALID_RID,
                             aPageType,
                             smsGetSegmentId( aSegmentHandle ),
                             0, /* aScn */
                             aEnv ) == STL_SUCCESS );
    }

    STL_TRY( smsmpWriteLogUpdateSegHdr( aMiniTrans,
                                        smsGetSegRid( aSegmentHandle ),
                                        sCache,
                                        aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmpGetInsertablePage( smxmTrans       * aMiniTrans,
                                  smlTbsId          aTbsId,
                                  void            * aSegmentHandle,
                                  smpPageType       aPageType,
                                  smpIsAgableFunc   aIsAgableFunc,
                                  smsSearchHint   * aSearchHint,
                                  smpHandle       * aPageHandle,
                                  stlBool         * aIsNewPage,
                                  smlEnv          * aEnv )
{
    smsmpCache * sCache;
    smpPhyHdr  * sPhyHdr;
    
    sCache = (smsmpCache*)aSegmentHandle;
    *aIsNewPage = STL_FALSE;

    if( sCache->mCurDataPid != SMP_NULL_PID )
    {
        STL_TRY( smpAcquire( aMiniTrans,
                             aTbsId,
                             sCache->mCurDataPid,
                             KNL_LATCH_MODE_EXCLUSIVE,
                             aPageHandle,
                             aEnv ) == STL_SUCCESS );
    
        if( SMP_FREENESS_INSERTABLE > smpGetFreeness( aPageHandle ) )
        {
            STL_TRY( smsmpAllocPage( aMiniTrans,
                                     aTbsId,
                                     aSegmentHandle,
                                     aPageType,
                                     aPageHandle,
                                     aEnv )
                     == STL_SUCCESS );
            *aIsNewPage = STL_TRUE;

            STL_DASSERT( SMP_FREENESS_INSERTABLE <= smpGetFreeness( aPageHandle ) );
        }
    }
    else
    {
        STL_TRY( smsmpAllocPage( aMiniTrans,
                                 aTbsId,
                                 aSegmentHandle,
                                 aPageType,
                                 aPageHandle,
                                 aEnv )
                 == STL_SUCCESS );
        *aIsNewPage = STL_TRUE;
    }
    
    sPhyHdr = (smpPhyHdr*)SMP_FRAME( aPageHandle );
    if( sPhyHdr->mPageType == SMP_PAGE_TYPE_UNFORMAT )
    {
        STL_TRY( smpInitHdr( aMiniTrans,
                             aTbsId,
                             aPageHandle,
                             SML_INVALID_RID,
                             aPageType,
                             smsGetSegmentId( aSegmentHandle ),
                             0, /* aScn */
                             aEnv ) == STL_SUCCESS );
        *aIsNewPage = STL_TRUE;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmpUpdatePageStatus( smxmTrans   * aMiniTrans,
                                 smlTbsId      aTbsId,
                                 smpHandle   * aPageHandle,
                                 smpFreeness   aFreeness,
                                 smlScn        aScn,
                                 smlEnv      * aEnv )
{
    STL_TRY_THROW( smpGetFreeness( aPageHandle ) != aFreeness, RAMP_SUCCESS );
                                            
    STL_TRY( smpUpdateFreeness( aMiniTrans,
                                aTbsId,
                                aPageHandle,
                                aFreeness,
                                aEnv ) == STL_SUCCESS );

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmpBuildCache( smlTbsId    aTbsId,
                           smlPid      aSegHdrPid,
                           void     ** aSegmentHandle,
                           smlEnv    * aEnv )
{
    smsmpCache  * sCache;
    stlInt32      sState = 0;
    smpHandle     sPageHandle;
    smsmpSegHdr * sSegHdr;
    
    STL_TRY( smgAllocShmMem4Open( STL_SIZEOF( smsmpCache ),
                                  aSegmentHandle,
                                  aEnv ) == STL_SUCCESS );

    sCache = (smsmpCache*)(*aSegmentHandle);

    STL_TRY( smpFix( aTbsId,
                     aSegHdrPid,
                     &sPageHandle,
                     aEnv ) == STL_SUCCESS );
    sState = 1;

    sSegHdr = (smsmpSegHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );

    sCache->mFirstHdrPid = sSegHdr->mFirstHdrPid;
    sCache->mCurDataPid= sSegHdr->mCurDataPid;
    sCache->mCurHdrPid = sSegHdr->mCurHdrPid;
    sCache->mCommonCache.mAllocExtCount = sSegHdr->mCommonSegHdr.mAllocExtCount;
    sCache->mCommonCache.mIsUsable = sSegHdr->mCommonSegHdr.mIsUsable;
    sCache->mCommonCache.mValidScn = sSegHdr->mCommonSegHdr.mValidScn;
    sCache->mCommonCache.mValidSeq = sSegHdr->mCommonSegHdr.mValidSeq;
    
    smsSetSegRid( (void*)sCache,((smlRid){ aTbsId, 0, aSegHdrPid }) );
    smsSetSegmentId( (void*)sCache,((smlRid){ aTbsId, 0, aSegHdrPid }) );
    smsSetSegType( (void*)sCache, sSegHdr->mCommonSegHdr.mSegType );
    smsSetTbsId( (void*)sCache, sSegHdr->mCommonSegHdr.mTbsId );
    smsSetHasRelHdr( (void*)sCache, sSegHdr->mCommonSegHdr.mHasRelHdr );
    smsSetUsablePageCountInExt( (void*)sCache, smfGetPageCountInExt(aTbsId) - 1 );

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

stlInt32 smsmpGetHeaderSize()
{
    return STL_SIZEOF( smsmpSegHdr );
}

stlInt32 smsmpGetCacheSize()
{
    return STL_SIZEOF( smsmpCache );
}

/** @} */
