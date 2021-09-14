/*******************************************************************************
 * smsmcSegment.c
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
#include <smg.h>
#include <sms.h>
#include <smf.h>
#include <smsDef.h>
#include <smsmcDef.h>
#include <smsmcSegment.h>
#include <smsmcScan.h>
#include <smsManager.h>
#include <smsmcLog.h>

/**
 * @file smsmcSegment.c
 * @brief Storage Manager Layer Circular Segment Internal Routines
 */

/**
 * @addtogroup smsmcSegment
 * @{
 */

smsSegmentFuncs gMemoryCircularSegment =
{
    smsmcAllocHandle,  /* alloc handle */
    NULL,              /* clone handle */
    smsmcCreate,
    smsmcDrop,
    NULL,              /* insert pending drop */
    NULL,              /* reset */
    smsmcAllocPage,
    smsmcPreparePage,
    NULL,              /* alloc multiple page */
    smsmcStealPages,
    smsmcGetInsertablePage,
    NULL,             /* update page status */
    smsmcAllocSegIterator,
    smsmcFreeSegIterator,
    smsmcGetFirstPage,
    smsmcGetNextPage,
    smsmcBuildCache,
    NULL,              /* alloc extent */
    smsmcGetHeaderSize,
    smsmcGetCacheSize,
    NULL,              /* get used page count for statistics */
    smsmcShrinkTo,
    smsmcSetTransToHandle
};

stlStatus smsmcAllocHandle( smxlTransId       aTransId,
                            smlRid            aSegRid,
                            smlTbsId          aTbsId,
                            smlSegmentAttr  * aAttr,
                            stlBool           aHasRelHdr,
                            smsSegHdr       * aSegHdr,
                            void           ** aSegmentHandle,
                            smlEnv          * aEnv )
{
    stlInt32      sState = 0;
    smsmcCache  * sCache;

    STL_TRY( smgAllocShmMem4Open( STL_SIZEOF( smsmcCache ),
                                  aSegmentHandle,
                                  aEnv ) == STL_SUCCESS );
    sState = 1;

    sCache = (smsmcCache*)(*aSegmentHandle);

    sCache->mCurDataPid = SMP_NULL_PID;
    sCache->mHdrPid = SMP_NULL_PID;
    sCache->mCurExtId = 0;
    sCache->mCommonCache.mAllocExtCount = 0;
    sCache->mCommonCache.mIsUsable = STL_TRUE;
    sCache->mLastTransId = SML_INVALID_TRANSID;
    sCache->mLastTransViewScn = SML_INFINITE_SCN;

    smsSetSegRid( (void*)sCache, aSegRid );
    smsSetSegmentId( (void*)sCache, aSegRid );
    smsSetSegType( (void*)sCache, SML_SEG_TYPE_MEMORY_CIRCULAR );
    smsSetTbsId( (void*)sCache, aTbsId );
    smsSetHasRelHdr( (void*)sCache, aHasRelHdr );
    smsSetUsablePageCountInExt( (void*)sCache, smfGetPageCountInExt(aTbsId) - 1 );

    return STL_SUCCESS;
    
    STL_FINISH;

    if( sState == 1 )
    {
        (void)smgFreeShmMem4Open( *aSegmentHandle, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smsmcCreate( smxlTransId    aTransId,
                       smlTbsId       aTbsId,
                       void         * aSegmentHandle,
                       stlInt32       aInitialExtents,
                       smlEnv       * aEnv )
{
    smxmTrans     sMiniTrans;
    smlPid        sFirstDataPid;
    smlPid        sSecondDataPid;
    smlExtId      sFirstExtId;
    smlExtId      sSecondExtId;
    smsmcExtHdr * sFirstHdr;
    smsmcExtHdr * sSecondHdr;
    smpHandle     sFirstPageHandle;
    smpHandle     sSecondPageHandle;
    smpHandle     sPageHandle;
    stlInt32      sState = 0;
    smsmcCache  * sCache;
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
                        SMP_PAGE_TYPE_CIRCULAR_HEADER,
                        0, /* aSegmentId */
                        0, /* aScn */
                        &sFirstPageHandle,
                        aEnv ) == STL_SUCCESS );
    sFirstHdr = (smsmcExtHdr*)smpGetBody( SMP_FRAME( &sFirstPageHandle ) );
    
    STL_TRY( smtAllocExtent( &sMiniTrans,
                             aTbsId,
                             &sSecondExtId,
                             &sSecondDataPid,
                             aEnv ) == STL_SUCCESS );
    sAllocExtCount++;

    STL_TRY( smpCreate( &sMiniTrans,
                        aTbsId,
                        sSecondDataPid,
                        SMP_PAGE_TYPE_CIRCULAR_HEADER,
                        0, /* aSegmentId */
                        0, /* aScn */
                        &sSecondPageHandle,
                        aEnv ) == STL_SUCCESS );
    sSecondHdr = (smsmcExtHdr*)smpGetBody( SMP_FRAME( &sSecondPageHandle ) );

    sFirstHdr->mNextHdrPid = sSecondDataPid;
    sFirstHdr->mPrevHdrPid = sSecondDataPid;
    sFirstHdr->mLastTransId = SML_INVALID_TRANSID;
    sFirstHdr->mLastTransViewScn = SML_INFINITE_SCN;
    sFirstHdr->mExtId = sFirstExtId;

    sSecondHdr->mNextHdrPid = sFirstDataPid;
    sSecondHdr->mPrevHdrPid = sFirstDataPid;
    sSecondHdr->mLastTransId = SML_INVALID_TRANSID;
    sSecondHdr->mLastTransViewScn = SML_INFINITE_SCN;
    sSecondHdr->mExtId = sSecondExtId;

    STL_TRY( smsmcWriteLogUpdateExtHdr( &sMiniTrans,
                                        aTbsId,
                                        sFirstDataPid,
                                        sFirstHdr,
                                        aEnv ) == STL_SUCCESS );
    
    STL_TRY( smsmcWriteLogUpdateExtHdr( &sMiniTrans,
                                        aTbsId,
                                        sSecondDataPid,
                                        sSecondHdr,
                                        aEnv ) == STL_SUCCESS );

    STL_TRY( smpCreate( &sMiniTrans,
                        aTbsId,
                        sFirstDataPid + 1,
                        SMP_PAGE_TYPE_UNFORMAT,
                        0, /* aSegmentId */
                        SML_INFINITE_SCN, /* aScn */
                        &sPageHandle,
                        aEnv ) == STL_SUCCESS );

    sCache = (smsmcCache*)(aSegmentHandle);

    sCache->mCurDataPid = sFirstDataPid + 1;
    sCache->mHdrPid = sFirstDataPid;
    sCache->mCurExtId = sFirstExtId;
    sCache->mCommonCache.mAllocExtCount = sAllocExtCount;

    STL_TRY( smsmcWriteLogUpdateSegHdr( &sMiniTrans,
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

stlStatus smsmcDrop( smxlTransId   aTransId,
                     smlTbsId      aTbsId,
                     void        * aSegmentHandle,
                     smlEnv      * aEnv )
{
    smxmTrans     sMiniTrans;
    smlExtId      sExtId;
    smsmcExtHdr * sExtHdr;
    smpHandle     sPageHandle;
    stlInt32      sState = 0;
    smsmcCache  * sCache;
    stlBool       sIsSuccess;
    smlPid        sHdrPageId;
    smlPid        sNextHdrPageId;
    
    sCache = (smsmcCache*)(aSegmentHandle);

    sHdrPageId = sCache->mHdrPid;

    while( sHdrPageId != SMP_NULL_PID )
    {
        STL_TRY( smxmBegin( &sMiniTrans,
                            aTransId,
                            smsGetSegRid( aSegmentHandle ),
                            SMXM_ATTR_REDO,
                            aEnv ) == STL_SUCCESS );
        sState = 1;
    
        STL_TRY( smpAcquire( NULL,
                             aTbsId,
                             sHdrPageId,
                             KNL_LATCH_MODE_EXCLUSIVE,
                             &sPageHandle,
                             aEnv ) == STL_SUCCESS );
        sState = 2;

        sExtHdr = (smsmcExtHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );

        sNextHdrPageId = sExtHdr->mNextHdrPid;
        sExtId = sExtHdr->mExtId;

        sState = 1;
        STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );

        STL_TRY( smtFreeExtent( &sMiniTrans,
                                aTbsId,
                                sExtId,
                                &sIsSuccess,
                                aEnv ) == STL_SUCCESS );

        if( sIsSuccess == STL_TRUE )
        {
            sHdrPageId = sNextHdrPageId;
        }

        sState = 0;
        STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
        
        if( sHdrPageId == sCache->mHdrPid )
        {
            /**
             * rounding 되는 시점에 종료한다.
             */
            break;
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) smpRelease( &sPageHandle, aEnv );
        case 1:
            (void) smxmCommit( &sMiniTrans, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus smsmcAllocPage( smxmTrans       * aMiniTrans,
                          smlTbsId          aTbsId,
                          void            * aSegmentHandle,
                          smpPageType       aPageType,
                          smpIsAgableFunc   aIsAgableFunc,
                          smpHandle       * aPageHandle,
                          smlEnv          * aEnv )
{
    smsmcCache  * sCache;
    smlExtId      sExtId;
    smlPid        sFirstPid;
    smlPid        sNextHdrPid;
    smpHandle     sHdrHandle;
    stlInt32      sState = 0;
    smsmcExtHdr * sHdr;
    smlPid        sNewPageId;
    smpPhyHdr   * sPhyHdr;
    stlUInt16     sPageCountInExt;
    smxlTransId   sTransId;
    smlScn        sCommitScn;
    stlInt32      sTryCount = 0;
    
    sCache = (smsmcCache*)aSegmentHandle;
    sPageCountInExt = smfGetPageCountInExt( aTbsId );
    sTransId = smxmGetTransId( aMiniTrans );

    /**
     * 현재 Extent에서 가용 페이지가 있는지 검사한다.
     */
    if( (sCache->mCurDataPid - sCache->mHdrPid + 1) >= sPageCountInExt )
    {
        /**
         * 가용 공간이 부족하다면 Extent의 재사용 여부를 판단하거나
         * 새로운 Extent를 할당한다.
         */
        STL_TRY( smpFix( aTbsId,
                         sCache->mHdrPid,
                         &sHdrHandle,
                         aEnv ) == STL_SUCCESS );
        sState = 1;
        
        sHdr = (smsmcExtHdr*)smpGetBody( SMP_FRAME( &sHdrHandle ) );
        sNextHdrPid = sHdr->mNextHdrPid;

        sState = 0;
        STL_TRY( smpUnfix( &sHdrHandle, aEnv ) == STL_SUCCESS );
        
        STL_TRY( smpFix( aTbsId,
                         sNextHdrPid,
                         &sHdrHandle,
                         aEnv ) == STL_SUCCESS );
        sState = 1;
        
        sHdr = (smsmcExtHdr*)smpGetBody( SMP_FRAME( &sHdrHandle ) );
        
        if( sHdr->mLastTransId == SML_INVALID_TRANSID )
        {
            /**
             * 한번도 사용된 적이 없는 Extent는 무조건 재사용가능하다.
             */
            sCommitScn = 0;
        }
        else
        {
            /**
             * 재사용 가능여부 계산
             */
            STL_TRY( smxlGetCommitScn( sHdr->mLastTransId,
                                       sHdr->mLastTransViewScn,
                                       &sCommitScn,
                                       aEnv )
                     == STL_SUCCESS );
        }

        sTryCount = 0;
        
        while( 1 )
        {
            if( sCommitScn <= smxlGetAgableScn( aEnv ) )
            {
                sState = 0;
                STL_TRY( smpUnfix( &sHdrHandle, aEnv ) == STL_SUCCESS );

                /**
                 * 이전 Extent Header에 Aging 정보 설정
                 */
                STL_TRY( smpAcquire( aMiniTrans,
                                     aTbsId,
                                     sCache->mHdrPid,
                                     KNL_LATCH_MODE_EXCLUSIVE,
                                     &sHdrHandle,
                                     aEnv ) == STL_SUCCESS );
                sHdr = (smsmcExtHdr*)smpGetBody( SMP_FRAME( &sHdrHandle ) );
                
                sHdr->mLastTransId = sCache->mLastTransId;
                sHdr->mLastTransViewScn = sCache->mLastTransViewScn;
            
                STL_TRY( smsmcWriteLogUpdateExtHdr( aMiniTrans,
                                                    aTbsId,
                                                    smpGetPageId( &sHdrHandle ),
                                                    sHdr,
                                                    aEnv ) == STL_SUCCESS );

#ifdef STL_DEBUG
                if( smxlIsTransRecordRecorded( aMiniTrans->mTransId ) == STL_TRUE )
                {
                    smpHandle     tmpHandle;
                    smsmcExtHdr * tmpHdr;
                    
                    STL_TRY( smpFix( aTbsId,
                                     sNextHdrPid,
                                     &tmpHandle,
                                     aEnv ) == STL_SUCCESS );
                    tmpHdr = (smsmcExtHdr*)smpGetBody( SMP_FRAME( &tmpHandle ) );
                
                    STL_DASSERT( (tmpHdr->mLastTransId != sCache->mLastTransId) ||
                                 (tmpHdr->mLastTransViewScn != sCache->mLastTransViewScn) );

                    STL_TRY( smpUnfix( &tmpHandle, aEnv ) == STL_SUCCESS );
                }
#endif
            
                sNewPageId = sNextHdrPid + 1;
                sCache->mHdrPid = sNextHdrPid;

                break;
            }
            else
            {
                if( sTryCount == 1 )
                {
                    /**
                     * 재사용 불가능하다면 새로운 Extent를 할당한다.
                     */
        
                    sState = 0;
                    STL_TRY( smpUnfix( &sHdrHandle, aEnv ) == STL_SUCCESS );
            
                    STL_TRY( smsmcAllocExtent( sCache,
                                               sTransId,
                                               aTbsId,
                                               &sFirstPid,
                                               &sExtId,
                                               aEnv ) == STL_SUCCESS );
                    sCache->mHdrPid = sFirstPid;
                    sCache->mCurExtId = sExtId;
                    sNewPageId = sFirstPid + 1;
                    
                    break;
                }
                
                smxlBuildAgableScn( aEnv );
                sTryCount++;
            }
        }
    }
    else
    {
        /**
         * 가용 페이지가 존재하면
         */
        sNewPageId = sCache->mCurDataPid + 1;
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
                             SML_INFINITE_SCN, /* aScn */
                             aEnv ) == STL_SUCCESS );
    }

    STL_TRY( smsmcWriteLogUpdateSegHdr( aMiniTrans,
                                        smsGetSegRid( aSegmentHandle ),
                                        sCache,
                                        aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)smpUnfix( &sHdrHandle, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus smsmcPreparePage( smxlTransId   aTransId,
                            void        * aSegmentHandle,
                            stlUInt32     aPageCount,
                            smlEnv      * aEnv )
{
    smsmcCache  * sCache;
    smlExtId      sExtId;
    smlPid        sFirstPid;
    smlPid        sNextHdrPid;
    smpHandle     sHdrHandle;
    stlInt32      sState = 0;
    smsmcExtHdr * sHdr;
    stlUInt16     sPageCountInExt;
    smlTbsId      sTbsId;
    smlScn        sCommitScn;
    
    sCache = (smsmcCache*)aSegmentHandle;
    sTbsId = smsGetTbsId( aSegmentHandle );
    sPageCountInExt = smfGetPageCountInExt( sTbsId );

    if( (sCache->mCurDataPid - sCache->mHdrPid + 1) >= sPageCountInExt )
    {
        STL_TRY( smpFix( sTbsId,
                         sCache->mHdrPid,
                         &sHdrHandle,
                         aEnv ) == STL_SUCCESS );
        sState = 1;
        
        sHdr = (smsmcExtHdr*)smpGetBody( SMP_FRAME( &sHdrHandle ) );
        sNextHdrPid = sHdr->mNextHdrPid;

        sState = 0;
        STL_TRY( smpUnfix( &sHdrHandle, aEnv ) == STL_SUCCESS );
        
        STL_TRY( smpFix( sTbsId,
                         sNextHdrPid,
                         &sHdrHandle,
                         aEnv ) == STL_SUCCESS );
        sState = 1;
        
        sHdr = (smsmcExtHdr*)smpGetBody( SMP_FRAME( &sHdrHandle ) );
        
        if( sHdr->mLastTransId == SML_INVALID_TRANSID )
        {
            /**
             * 한번도 사용된 적이 없는 Extent는 무조건 재사용가능하다.
             */
            sCommitScn = 0;
        }
        else
        {
            /**
             * 재사용 가능여부 계산
             */
            STL_TRY( smxlGetCommitScn( sHdr->mLastTransId,
                                       sHdr->mLastTransViewScn,
                                       &sCommitScn,
                                       aEnv )
                     == STL_SUCCESS );
        }
                                       
        if( sCommitScn <= smxlGetAgableScn( aEnv ) )
        {
            sState = 0;
            STL_TRY( smpUnfix( &sHdrHandle, aEnv ) == STL_SUCCESS );
        }
        else
        {
            sState = 0;
            STL_TRY( smpUnfix( &sHdrHandle, aEnv ) == STL_SUCCESS );
            
            STL_TRY( smsmcAllocExtent( sCache,
                                       aTransId,
                                       sTbsId,
                                       &sFirstPid,
                                       &sExtId,
                                       aEnv ) == STL_SUCCESS );
        }
    }
    else
    {
        /**
         * Nothing to do
         */
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1 :
            (void)smpUnfix( &sHdrHandle, aEnv );
        default :
            break;
    }
    
    return STL_FAILURE;
}

stlStatus smsmcStealPages( smxmTrans     * aMiniTrans,
                           smlTbsId        aTbsId,
                           void          * aDstSegHandle,
                           void          * aSrcSegHandle,
                           stlInt64        aShrinkToPageCount,
                           stlInt64      * aStolenPageCount,
                           smlEnv        * aEnv )
{
    smsmcCache  * sSrcCache;
    smsmcCache  * sDstCache;
    smlPid        sFirstHdrPid = SMP_NULL_PID;
    smlPid        sLastHdrPid = SMP_NULL_PID;
    smlPid        sNextHdrPid;
    smlPid        sOrgNextHdrPid;
    smpHandle     sHdrHandle;
    stlInt32      sState = 0;
    smsmcExtHdr * sHdr;
    stlUInt16     sPageCountInExt;
    smlScn        sCommitScn;
    stlInt64      sStolenPageCount = 0;
    stlInt64      sAllocPageCount = 0;
    stlInt64      sMinUndoPageCount;
    
    sSrcCache = (smsmcCache*)aSrcSegHandle;
    sDstCache = (smsmcCache*)aDstSegHandle;
    sPageCountInExt = smfGetPageCountInExt( aTbsId );

    sMinUndoPageCount = knlGetPropertyBigIntValueByID( KNL_PROPERTY_MINIMUM_UNDO_PAGE_COUNT,
                                                       KNL_ENV(aEnv) );
    
    sAllocPageCount = smsGetAllocPageCount( aSrcSegHandle );

    sNextHdrPid = sSrcCache->mHdrPid;

    STL_TRY( smpFix( aTbsId,
                     sSrcCache->mHdrPid,
                     &sHdrHandle,
                     aEnv ) == STL_SUCCESS );
    sState = 1;
        
    sHdr = (smsmcExtHdr*)smpGetBody( SMP_FRAME( &sHdrHandle ) );
    sNextHdrPid = sHdr->mNextHdrPid;
    sFirstHdrPid = sNextHdrPid;

    sState = 0;
    STL_TRY( smpUnfix( &sHdrHandle, aEnv ) == STL_SUCCESS );

    while( 1 )
    {
        STL_TRY( smpFix( aTbsId,
                         sNextHdrPid,
                         &sHdrHandle,
                         aEnv ) == STL_SUCCESS );
        sState = 1;

        sHdr = (smsmcExtHdr*)smpGetBody( SMP_FRAME( &sHdrHandle ) );
        
        if( sHdr->mLastTransId == SML_INVALID_TRANSID )
        {
            /**
             * 한번도 사용된 적이 없는 Extent는 무조건 재사용가능하다.
             */
            sCommitScn = 0;
        }
        else
        {
            /**
             * 재사용 가능여부 계산
             */
            STL_TRY( smxlGetCommitScn( sHdr->mLastTransId,
                                       sHdr->mLastTransViewScn,
                                       &sCommitScn,
                                       aEnv )
                     == STL_SUCCESS );
        }

        /**
         * Extent가 Aable하지 않다면 여기까지 Steal한다.
         */
        if( sCommitScn > smxlGetAgableScn( aEnv ) )
        {
            break;
        }

        /**
         * 현재 사용중인 Extent가 Agable 하다고 해서 재사용하지 않는다.
         * - 모든 Extent를 다 검사한 경우
         */
        if( sHdr->mNextHdrPid == sSrcCache->mHdrPid )
        {
            break;
        }

        if( aShrinkToPageCount == 0 )
        {
            /**
             * Steal한 페이지 개수가 정해진 개수를 초과하면 여기까지 Steal한다.
             */
            if( (sStolenPageCount + sPageCountInExt) > SMSMC_MAX_STEAL_PAGE_COUNT )
            {
                break;
            }
        }
        else
        {
            /**
             * 남아야할 페이지 개수 조건을 검사한다.
             */
            if( ( (sSrcCache->mCommonCache.mAllocExtCount * sPageCountInExt) -
                  (sStolenPageCount) ) <= aShrinkToPageCount )
            {
                break;
            }
        }
        
        /**
         * Source Segment에 정해진 개수 이하로 페이지가 남는다면
         * 여기까지 Steal한다.
         */
        if( (sAllocPageCount - (sStolenPageCount + sPageCountInExt)) <= sMinUndoPageCount )
        {
            break;
        }
        
        sLastHdrPid = sNextHdrPid;
        sNextHdrPid = sHdr->mNextHdrPid;
        sStolenPageCount += sPageCountInExt;
        
        sState = 0;
        STL_TRY( smpUnfix( &sHdrHandle, aEnv ) == STL_SUCCESS );
    }

    sState = 0;
    STL_TRY( smpUnfix( &sHdrHandle, aEnv ) == STL_SUCCESS );
    
    *aStolenPageCount = sStolenPageCount;
    
    if( sStolenPageCount > 0 )
    {
        if( aDstSegHandle == NULL )
        {
            /**
             * Dest가 Null이라면 Tablespace에 반납한다.
             */

            STL_TRY( smsmcFreeExtents( smxmGetTransId(aMiniTrans),
                                       aTbsId,
                                       sSrcCache,
                                       sFirstHdrPid,
                                       sLastHdrPid,
                                       aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /**
             * Source Segment의 Unlink
             */
            STL_TRY( smpAcquire( aMiniTrans,
                                 aTbsId,
                                 sSrcCache->mHdrPid,
                                 KNL_LATCH_MODE_EXCLUSIVE,
                                 &sHdrHandle,
                                 aEnv ) == STL_SUCCESS );
            sHdr = (smsmcExtHdr*)smpGetBody( SMP_FRAME( &sHdrHandle ) );
            sHdr->mNextHdrPid = sNextHdrPid;
        
            STL_TRY( smsmcWriteLogUpdateExtHdr( aMiniTrans,
                                                aTbsId,
                                                smpGetPageId( &sHdrHandle ),
                                                sHdr,
                                                aEnv ) == STL_SUCCESS );

            STL_TRY( smpAcquire( aMiniTrans,
                                 aTbsId,
                                 sNextHdrPid,
                                 KNL_LATCH_MODE_EXCLUSIVE,
                                 &sHdrHandle,
                                 aEnv ) == STL_SUCCESS );
            sHdr = (smsmcExtHdr*)smpGetBody( SMP_FRAME( &sHdrHandle ) );
            sHdr->mPrevHdrPid = sSrcCache->mHdrPid;
        
            STL_TRY( smsmcWriteLogUpdateExtHdr( aMiniTrans,
                                                aTbsId,
                                                smpGetPageId( &sHdrHandle ),
                                                sHdr,
                                                aEnv ) == STL_SUCCESS );

            sSrcCache->mCommonCache.mAllocExtCount -= (sStolenPageCount / sPageCountInExt);
        
            STL_TRY( smsmcWriteLogUpdateSegHdr( aMiniTrans,
                                                smsGetSegRid( aSrcSegHandle ),
                                                sSrcCache,
                                                aEnv ) == STL_SUCCESS );
        
            /**
             * Target Segment의 Link
             */

            STL_TRY( smpAcquire( aMiniTrans,
                                 aTbsId,
                                 sDstCache->mHdrPid,
                                 KNL_LATCH_MODE_EXCLUSIVE,
                                 &sHdrHandle,
                                 aEnv ) == STL_SUCCESS );
            sHdr = (smsmcExtHdr*)smpGetBody( SMP_FRAME( &sHdrHandle ) );
            sOrgNextHdrPid = sHdr->mNextHdrPid;
            sHdr->mNextHdrPid = sFirstHdrPid;
        
            STL_TRY( smsmcWriteLogUpdateExtHdr( aMiniTrans,
                                                aTbsId,
                                                smpGetPageId( &sHdrHandle ),
                                                sHdr,
                                                aEnv ) == STL_SUCCESS );

            STL_TRY( smpAcquire( aMiniTrans,
                                 aTbsId,
                                 sOrgNextHdrPid,
                                 KNL_LATCH_MODE_EXCLUSIVE,
                                 &sHdrHandle,
                                 aEnv ) == STL_SUCCESS );
            sHdr = (smsmcExtHdr*)smpGetBody( SMP_FRAME( &sHdrHandle ) );
            sHdr->mPrevHdrPid = sLastHdrPid;
        
            STL_TRY( smsmcWriteLogUpdateExtHdr( aMiniTrans,
                                                aTbsId,
                                                smpGetPageId( &sHdrHandle ),
                                                sHdr,
                                                aEnv ) == STL_SUCCESS );

            STL_TRY( smpAcquire( aMiniTrans,
                                 aTbsId,
                                 sFirstHdrPid,
                                 KNL_LATCH_MODE_EXCLUSIVE,
                                 &sHdrHandle,
                                 aEnv ) == STL_SUCCESS );
            sHdr = (smsmcExtHdr*)smpGetBody( SMP_FRAME( &sHdrHandle ) );
            sHdr->mPrevHdrPid = sDstCache->mHdrPid;
        
            STL_TRY( smsmcWriteLogUpdateExtHdr( aMiniTrans,
                                                aTbsId,
                                                smpGetPageId( &sHdrHandle ),
                                                sHdr,
                                                aEnv ) == STL_SUCCESS );

            STL_TRY( smpAcquire( aMiniTrans,
                                 aTbsId,
                                 sLastHdrPid,
                                 KNL_LATCH_MODE_EXCLUSIVE,
                                 &sHdrHandle,
                                 aEnv ) == STL_SUCCESS );
            sHdr = (smsmcExtHdr*)smpGetBody( SMP_FRAME( &sHdrHandle ) );
            sHdr->mNextHdrPid = sOrgNextHdrPid;
        
            STL_TRY( smsmcWriteLogUpdateExtHdr( aMiniTrans,
                                                aTbsId,
                                                smpGetPageId( &sHdrHandle ),
                                                sHdr,
                                                aEnv ) == STL_SUCCESS );

            sDstCache->mCommonCache.mAllocExtCount += (sStolenPageCount / sPageCountInExt);
        
            STL_TRY( smsmcWriteLogUpdateSegHdr( aMiniTrans,
                                                smsGetSegRid( aDstSegHandle ),
                                                sDstCache,
                                                aEnv ) == STL_SUCCESS );
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpUnfix( &sHdrHandle, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smsmcGetInsertablePage( smxmTrans       * aMiniTrans,
                                  smlTbsId          aTbsId,
                                  void            * aSegmentHandle,
                                  smpPageType       aPageType,
                                  smpIsAgableFunc   aIsAgableFunc,
                                  smsSearchHint   * aSearchHint,
                                  smpHandle       * aPageHandle,
                                  stlBool         * aIsNewPage,
                                  smlEnv          * aEnv )
{
    smsmcCache * sCache;
    smlPid       sPageId;
    smpPhyHdr  * sPhyHdr;
    smpHandle  * sPageCachedHandle;
    
    sCache = (smsmcCache*)aSegmentHandle;
    sPageId = sCache->mCurDataPid;
    *aIsNewPage = STL_FALSE;
    
    sPageCachedHandle = smxmGetPageHandle( aMiniTrans,
                                           aTbsId,
                                           sPageId,
                                           KNL_LATCH_MODE_EXCLUSIVE );
    
    if( sPageCachedHandle == NULL )
    {
        STL_TRY( smpAcquire( aMiniTrans,
                             aTbsId,
                             sPageId,
                             KNL_LATCH_MODE_EXCLUSIVE,
                             aPageHandle,
                             aEnv ) == STL_SUCCESS );
    }
    else
    {
        *aPageHandle = *sPageCachedHandle;
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
                             SML_INFINITE_SCN, /* aScn */
                             aEnv ) == STL_SUCCESS );
        *aIsNewPage = STL_TRUE;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmcBuildCache( smlTbsId    aTbsId,
                           smlPid      aSegHdrPid,
                           void     ** aSegmentHandle,
                           smlEnv    * aEnv )
{
    smsmcCache  * sCache;
    stlInt32      sState = 0;
    smpHandle     sPageHandle;
    smsmcSegHdr * sSegHdr;
    
    STL_TRY( smgAllocShmMem4Open( STL_SIZEOF( smsmcCache ),
                                  aSegmentHandle,
                                  aEnv ) == STL_SUCCESS );

    sCache = (smsmcCache*)(*aSegmentHandle);

    STL_TRY( smpFix( aTbsId,
                     aSegHdrPid,
                     &sPageHandle,
                     aEnv ) == STL_SUCCESS );
    sState = 1;

    sSegHdr = (smsmcSegHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );

    sCache->mCurDataPid= sSegHdr->mCurDataPid;
    sCache->mHdrPid = sSegHdr->mHdrPid;
    sCache->mCurExtId = sSegHdr->mCurExtId;
    sCache->mLastTransId = SML_INVALID_TRANSID;
    sCache->mLastTransViewScn = SML_INFINITE_SCN;
    
    sCache->mCommonCache.mAllocExtCount = sSegHdr->mCommonSegHdr.mAllocExtCount;
    sCache->mCommonCache.mIsUsable = sSegHdr->mCommonSegHdr.mIsUsable;
    sCache->mCommonCache.mValidScn = sSegHdr->mCommonSegHdr.mValidScn;
    sCache->mCommonCache.mValidSeq = sSegHdr->mCommonSegHdr.mValidSeq;
    
    smsSetHasRelHdr( (void*)sCache, sSegHdr->mCommonSegHdr.mHasRelHdr );
    smsSetSegRid( (void*)sCache,((smlRid){ aTbsId, 0, aSegHdrPid }) );
    smsSetSegmentId( (void*)sCache, ((smlRid){ aTbsId, 0, aSegHdrPid }) );
    smsSetSegType( (void*)sCache, sSegHdr->mCommonSegHdr.mSegType );
    smsSetTbsId( (void*)sCache, sSegHdr->mCommonSegHdr.mTbsId );
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

stlStatus smsmcShrinkTo( smxlTransId      aTransId,
                         void           * aSegmentHandle,
                         smlSegmentAttr * aAttr,
                         smlEnv         * aEnv )
{
    stlUInt64    sBytes;
    smlTbsId     sTbsId;
    stlInt64     sFreedPageCount;
    smxmTrans    sMiniTrans;
    stlInt32     sState = 0;
    
    STL_DASSERT( aAttr->mValidFlags & SML_SEGMENT_SHRINKTOSIZE_MASK );
    
    sTbsId = smsGetTbsId( aSegmentHandle );
    sBytes = aAttr->mShrinkToSize;

    STL_TRY( smxmBegin( &sMiniTrans,
                        aTransId,
                        smsGetSegRid( aSegmentHandle ),
                        SMXM_ATTR_REDO,
                        aEnv ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( smsmcStealPages( &sMiniTrans,
                              sTbsId,
                              NULL, /* aDstSegHandle */
                              aSegmentHandle,
                              STL_ALIGN( sBytes, SMP_PAGE_SIZE ) / SMP_PAGE_SIZE,
                              &sFreedPageCount,
                              aEnv )
             == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smxmCommit( &sMiniTrans, aEnv );
    }

    return STL_FAILURE;
}


stlStatus smsmcAllocExtent( smsmcCache  * aCache,
                            smxlTransId   aTransId,
                            smlTbsId      aTbsId,
                            smlPid      * aFristPid,
                            smlExtId    * aExtId,
                            smlEnv      * aEnv )
{
    smxmTrans      sMiniTrans;
    smlPid         sFirstPid;
    smlExtId       sExtId;
    smsmcExtHdr  * sCurHdr;
    smsmcExtHdr  * sPrvHdr;
    smsmcExtHdr  * sNxtHdr;
    smpHandle      sCurPageHandle;
    smpHandle      sPrvPageHandle;
    smpHandle      sNxtPageHandle;
    smpHandle      sPageHandle;
    stlInt32       sState = 0;

    STL_TRY( smxmBegin( &sMiniTrans,
                        aTransId,
                        SML_INVALID_RID,
                        SMXM_ATTR_REDO,
                        aEnv ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( smtAllocExtent( &sMiniTrans,
                             aTbsId,
                             &sExtId,
                             &sFirstPid,
                             aEnv ) == STL_SUCCESS );
    aCache->mCommonCache.mAllocExtCount++;
    
    STL_TRY( smpCreate( &sMiniTrans,
                        aTbsId,
                        sFirstPid,
                        SMP_PAGE_TYPE_CIRCULAR_HEADER,
                        0, /* aSegmentId */
                        0, /* aScn */
                        &sCurPageHandle,
                        aEnv ) == STL_SUCCESS );
    sCurHdr = (smsmcExtHdr*)smpGetBody( SMP_FRAME( &sCurPageHandle ) );
    
    STL_TRY( smpAcquire( &sMiniTrans,
                         aTbsId,
                         aCache->mHdrPid,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPrvPageHandle,
                         aEnv ) == STL_SUCCESS );
    sPrvHdr = (smsmcExtHdr*)smpGetBody( SMP_FRAME( &sPrvPageHandle ) );

    STL_TRY( smpAcquire( &sMiniTrans,
                         aTbsId,
                         sPrvHdr->mNextHdrPid,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sNxtPageHandle,
                         aEnv ) == STL_SUCCESS );
    sNxtHdr = (smsmcExtHdr*)smpGetBody( SMP_FRAME( &sNxtPageHandle ) );

    sCurHdr->mNextHdrPid = sPrvHdr->mNextHdrPid;
    sCurHdr->mPrevHdrPid = aCache->mHdrPid;
    sCurHdr->mLastTransId = SML_INVALID_TRANSID;
    sCurHdr->mLastTransViewScn = SML_INFINITE_SCN;
    sCurHdr->mExtId = sExtId;

    STL_DASSERT( (sPrvHdr->mLastTransId != aCache->mLastTransId) ||
                 (sPrvHdr->mLastTransViewScn != aCache->mLastTransViewScn) );
                
    sPrvHdr->mLastTransId = aCache->mLastTransId;
    sPrvHdr->mLastTransViewScn = aCache->mLastTransViewScn;
    sPrvHdr->mNextHdrPid = sFirstPid;
    
    sNxtHdr->mPrevHdrPid = sFirstPid;

    STL_TRY( smsmcWriteLogUpdateExtHdr( &sMiniTrans,
                                        aTbsId,
                                        smpGetPageId( &sCurPageHandle ),
                                        sCurHdr,
                                        aEnv ) == STL_SUCCESS );

    STL_TRY( smsmcWriteLogUpdateExtHdr( &sMiniTrans,
                                        aTbsId,
                                        smpGetPageId( &sPrvPageHandle ),
                                        sPrvHdr,
                                        aEnv ) == STL_SUCCESS );

    STL_TRY( smsmcWriteLogUpdateExtHdr( &sMiniTrans,
                                        aTbsId,
                                        smpGetPageId( &sNxtPageHandle ),
                                        sNxtHdr,
                                        aEnv ) == STL_SUCCESS );

    STL_TRY( smpCreate( &sMiniTrans,
                        aTbsId,
                        sFirstPid + 1,
                        SMP_PAGE_TYPE_UNFORMAT,
                        0, /* aSegmentId */
                        SML_INFINITE_SCN, /* aScn */
                        &sPageHandle,
                        aEnv ) == STL_SUCCESS );

    *aFristPid = sFirstPid;
    *aExtId    = sExtId;

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

stlStatus smsmcFreeExtents( smxlTransId   aTransId,
                            smlTbsId      aTbsId,
                            smsmcCache  * aCache,
                            smlPid        aFirstHdrPid,
                            smlPid        aLastHdrPid,
                            smlEnv      * aEnv )
{
    smxmTrans     sMiniTrans;
    smlExtId      sExtId;
    smsmcExtHdr * sExtHdr;
    smsmcExtHdr * sFreeExtHdr;
    smpHandle     sPageHandle;
    smpHandle     sFreeHandle;
    stlInt32      sState = 0;
    stlBool       sIsSuccess;
    smlPid        sCurHdrPid;
    smlPid        sNextHdrPid;
    
    sCurHdrPid = aFirstHdrPid;

    while( 1 )
    {
        STL_TRY( smxmBegin( &sMiniTrans,
                            aTransId,
                            smsGetSegRid( aCache ),
                            SMXM_ATTR_REDO,
                            aEnv ) == STL_SUCCESS );
        sState = 1;
    
        STL_TRY( smpFix( aTbsId,
                         sCurHdrPid,
                         &sFreeHandle,
                         aEnv )
                 == STL_SUCCESS );
        sState = 2;

        sFreeExtHdr = (smsmcExtHdr*)smpGetBody( SMP_FRAME( &sFreeHandle ) );

        sNextHdrPid = sFreeExtHdr->mNextHdrPid;
        sExtId = sFreeExtHdr->mExtId;

        sState = 1;
        STL_TRY( smpUnfix( &sFreeHandle, aEnv ) == STL_SUCCESS );

        STL_TRY( smtFreeExtent( &sMiniTrans,
                                aTbsId,
                                sExtId,
                                &sIsSuccess,
                                aEnv ) == STL_SUCCESS );

        if( sIsSuccess == STL_TRUE )
        {
            STL_TRY( smpAcquire( &sMiniTrans,
                                 aTbsId,
                                 aCache->mHdrPid,
                                 KNL_LATCH_MODE_EXCLUSIVE,
                                 &sPageHandle,
                                 aEnv ) == STL_SUCCESS );

            sExtHdr = (smsmcExtHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );
            sExtHdr->mNextHdrPid = sNextHdrPid;
            
            STL_TRY( smsmcWriteLogUpdateExtHdr( &sMiniTrans,
                                                aTbsId,
                                                aCache->mHdrPid,
                                                sExtHdr,
                                                aEnv ) == STL_SUCCESS );
            
            STL_TRY( smpAcquire( &sMiniTrans,
                                 aTbsId,
                                 sNextHdrPid,
                                 KNL_LATCH_MODE_EXCLUSIVE,
                                 &sPageHandle,
                                 aEnv ) == STL_SUCCESS );

            sExtHdr = (smsmcExtHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );
            sExtHdr->mPrevHdrPid = aCache->mHdrPid;
            
            STL_TRY( smsmcWriteLogUpdateExtHdr( &sMiniTrans,
                                                aTbsId,
                                                sNextHdrPid,
                                                sExtHdr,
                                                aEnv ) == STL_SUCCESS );
            
            aCache->mCommonCache.mAllocExtCount -= 1;
        
            STL_TRY( smsmcWriteLogUpdateSegHdr( &sMiniTrans,
                                                smsGetSegRid( aCache ),
                                                aCache,
                                                aEnv ) == STL_SUCCESS );
            
            /**
             * 지운 Extent가 Last라면 종료한다.
             */
            if( sCurHdrPid == aLastHdrPid )
            {
                sState = 0;
                STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
                break;
            }
            
            sCurHdrPid = sNextHdrPid;
        }

        sState = 0;
        STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) smpUnfix( &sFreeHandle, aEnv );
        case 1:
            (void) smxmCommit( &sMiniTrans, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

stlInt32 smsmcGetHeaderSize()
{
    return STL_SIZEOF( smsmcSegHdr );
}

stlInt32 smsmcGetCacheSize()
{
    return STL_SIZEOF( smsmcCache );
}

stlStatus smsmcSetTransToHandle( void        * aSegmentHandle,
                                 smxlTransId   aTransId,
                                 smlScn        aTransViewScn,
                                 smlEnv      * aEnv )
{
    smsmcCache * sCache;

    sCache = (smsmcCache*)aSegmentHandle;

    /**
     * case 1)
     *   |------------------|  A trans
     *                        |------------------|  B trans
     *  - TRANS_SCN : A < B
     *  - TRANS RECORD 기록 순서 : A -> B
     *  - COMMIT_SCN(B)로 Agable 판단이 가능하다.
     *                        
     * case 2)
     *   |------------------|  A trans
     *               |------------------|  B trans
     *  - case 1과 동일하다.
     *            
     * case 3)
     *            |---------|  A trans
     *   |----------------------------|  B trans
     *  - TRANS_SCN : A > B
     *  - TRANS RECORD 기록 순서 : A -> B
     *  - B가 먼저 시작했지만 B가 Agable View Scn에 의해서 Agable하다고 판단되다면
     *    A 또한 Agable 가능하다고 판단해야 한다.
     *  - COMMIT_SCN(B)로 Agable 판단이 가능하다.
     *
     * case 1,2,3을 고려할때 TRANS SCN의 순서에 상관없이 늦게 시작한 트랜잭션으로
     * Agable을 판단해야 한다.
     */
    
    sCache->mLastTransViewScn = aTransViewScn;
    sCache->mLastTransId = aTransId;
   
    return STL_SUCCESS;
}

/** @} */
