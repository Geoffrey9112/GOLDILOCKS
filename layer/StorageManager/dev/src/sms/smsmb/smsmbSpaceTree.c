/*******************************************************************************
 * smsmbSpaceTree.c
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
#include <smf.h>
#include <sms.h>
#include <smsDef.h>
#include <smsmbDef.h>
#include <smsmbSpaceTree.h>
#include <smsmbSegment.h>
#include <smsmbExtMap.h>
#include <smsmbLog.h>

/**
 * @file smsmbSpaceTree.c
 * @brief Storage Manager Layer Bitmap Space Tree Internal Routines
 */

/**
 * @addtogroup smsmbSpaceTree
 * @{
 */

stlStatus smsmbPropagateLeaf( smxmTrans  * aMiniTrans,
                              smlTbsId     aTbsId,
                              smsmbCache * aCache,
                              smlPid       aNewPageId,
                              smlRid     * aLeafRid,
                              smlEnv     * aEnv )
{
    smsmbLeafHdr * sLeafHdr;
    smsmbLeaf    * sLeaf;
    smpHandle      sPageHandle;
    stlInt32       i;
    stlInt32       j;
    smpFreeness    sRepresentFreeness;
    stlUInt16      sPageCountInExt;

    sPageCountInExt = smfGetPageCountInExt( aTbsId );
    if( aCache->mRootNode == SMP_NULL_PID )
    {
        STL_TRY( smsmbAllocLeaf( aMiniTrans,
                                 aTbsId,
                                 SMP_NULL_PID,
                                 aCache,
                                 &sPageHandle,
                                 aEnv ) == STL_SUCCESS );
        
        aCache->mRootNode = smpGetPageId( &sPageHandle );
    }
    else
    {
        if( (aCache->mTreeHwm[0].mOffset + 1 + sPageCountInExt) >
            SMSMB_LEAF_BIT_COUNT )
        {
            STL_TRY( smsmbExpandLeaf( aMiniTrans,
                                      aTbsId,
                                      aCache,
                                      &sPageHandle,
                                      aEnv ) == STL_SUCCESS );
        }
        else
        {
            STL_TRY( smpAcquire( aMiniTrans,
                                 aTbsId,
                                 aCache->mTreeHwm[0].mPageId,
                                 KNL_LATCH_MODE_EXCLUSIVE,
                                 &sPageHandle,
                                 aEnv ) == STL_SUCCESS );
        }
    }
    
    sLeafHdr = (smsmbLeafHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );
    sLeaf = (smsmbLeaf*)((stlChar*)sLeafHdr + STL_SIZEOF(smsmbLeafHdr));
    
    sLeafHdr->mChildCount += sPageCountInExt;
        
    for( i = aCache->mTreeHwm[0].mOffset + 1, j = 0;
         j < sPageCountInExt;
         i++, j++ )
    {
        sLeaf->mDataPid[i] = aNewPageId + j;
        sLeafHdr->mFreenessCount[SMP_FREENESS_FREE] += 1;
        smsmbSetFreeness( sLeaf->mFreeness,
                          i,
                          SMP_FREENESS_FREE );
    }

    STL_TRY( smsmbWriteLogExpandLeaf( aMiniTrans,
                                      aTbsId,
                                      aCache->mTreeHwm[0].mPageId,
                                      aCache->mTreeHwm[0].mOffset + 1,
                                      aNewPageId,
                                      sPageCountInExt,
                                      aEnv ) == STL_SUCCESS );

    sRepresentFreeness = smsmbRepresentFreeness( (void*)sLeafHdr,
                                                 STL_TRUE );
    
    STL_TRY( smsmbUpdatePageStatusInternal( aMiniTrans,
                                            aTbsId,
                                            &sPageHandle,
                                            sRepresentFreeness,
                                            aEnv )
             == STL_SUCCESS );

    *aLeafRid = (smlRid){ aTbsId,
                          aCache->mTreeHwm[0].mOffset + 1,
                          aCache->mTreeHwm[0].mPageId };

    aCache->mTreeHwm[0].mOffset += sPageCountInExt;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmbExpandLeaf( smxmTrans  * aMiniTrans,
                           smlTbsId     aTbsId,
                           smsmbCache * aCache,
                           smpHandle  * aPageHandle,
                           smlEnv     * aEnv )
{
    smlRid      sPropagatedRid;
    smpPhyHdr * sPhyHdr;
    smlRid      sLogData[2];
    
    STL_TRY( smsmbAllocLeaf( aMiniTrans,
                             aTbsId,
                             aCache->mTreeHwm[0].mPageId,
                             aCache,
                             aPageHandle,
                             aEnv ) == STL_SUCCESS );
    
    STL_TRY( smsmbPropagateInternal( aMiniTrans,
                                     aTbsId,
                                     aCache,
                                     1,
                                     smpGetPageId( aPageHandle ),
                                     &sPropagatedRid,
                                     aEnv ) == STL_SUCCESS );
    
    sPhyHdr = (smpPhyHdr*)SMP_FRAME( aPageHandle );
    sLogData[0] = sPhyHdr->mParentRid;
    sPhyHdr->mParentRid = sPropagatedRid;
    sLogData[1] = sPhyHdr->mParentRid;

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_RECOVERY,
                           SMR_LOG_BYTES,
                           SMR_REDO_TARGET_PAGE,
                           (void*)sLogData,
                           STL_SIZEOF( smlRid ) * 2,
                           aTbsId,
                           smpGetPageId( aPageHandle ),
                           STL_OFFSETOF( smpPhyHdr, mParentRid ),
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmbPropagateInternal( smxmTrans  * aMiniTrans,
                                  smlTbsId     aTbsId,
                                  smsmbCache * aCache,
                                  stlInt16     aDepth,
                                  smlPid       aNewPageId,
                                  smlRid     * aPropagatedRid,
                                  smlEnv     * aEnv )
{
    smsmbInternalHdr * sInternalHdr;
    smpHandle          sPageHandle;
    smpHandle        * sCachedPageHandle;
    smpHandle          sRootPageHandle;
    smpFreeness        sRepresentFreeness;
    smpPhyHdr        * sPhyHdr;
    smlRid             sLogData[2];
    
    if( aCache->mTreeHwm[aDepth].mPageId == SMP_NULL_PID )
    {
        STL_TRY( smsmbAllocInternal( aMiniTrans,
                                     aTbsId,
                                     aCache,
                                     aDepth,
                                     &sPageHandle,
                                     aEnv ) == STL_SUCCESS );

        sCachedPageHandle = smxmGetPageHandle( aMiniTrans,
                                               aTbsId,
                                               aCache->mRootNode,
                                               KNL_LATCH_MODE_EXCLUSIVE );

        if( sCachedPageHandle == NULL )
        {
            STL_TRY( smpAcquire( aMiniTrans,
                                 aTbsId,
                                 aCache->mRootNode,
                                 KNL_LATCH_MODE_EXCLUSIVE,
                                 &sRootPageHandle,
                                 aEnv ) == STL_SUCCESS );
        }
        else
        {
            sRootPageHandle = *sCachedPageHandle;
        }

        sPhyHdr = (smpPhyHdr*)SMP_FRAME( &sRootPageHandle );
        sLogData[0] = sPhyHdr->mParentRid;
        sPhyHdr->mParentRid = (smlRid){ aTbsId, 0, smpGetPageId(&sPageHandle) };
        sLogData[1] = sPhyHdr->mParentRid;

        STL_TRY( smxmWriteLog( aMiniTrans,
                               SMG_COMPONENT_RECOVERY,
                               SMR_LOG_BYTES,
                               SMR_REDO_TARGET_PAGE,
                               (void*)sLogData,
                               STL_SIZEOF( smlRid ) * 2,
                               aTbsId,
                               smpGetPageId( &sRootPageHandle ),
                               STL_OFFSETOF( smpPhyHdr, mParentRid ),
                               SMXM_LOGGING_REDO,
                               aEnv ) == STL_SUCCESS );
    
        STL_TRY( smsmbInsertInternalBit( aMiniTrans,
                                         aTbsId,
                                         aCache,
                                         &sPageHandle,
                                         aDepth,
                                         aCache->mRootNode,
                                         smpGetFreeness( &sRootPageHandle ),
                                         aEnv ) == STL_SUCCESS );
        
        aCache->mRootNode = smpGetPageId( &sPageHandle );
    }
    else
    {
        if( (aCache->mTreeHwm[aDepth].mOffset + 1 + 1) > SMSMB_INTERNAL_BIT_COUNT )
        {
            STL_TRY( smsmbExpandInternal( aMiniTrans,
                                          aTbsId,
                                          aCache,
                                          aDepth,
                                          &sPageHandle,
                                          aEnv ) == STL_SUCCESS );
        }
        else
        {
            sCachedPageHandle = smxmGetPageHandle( aMiniTrans,
                                                   aTbsId,
                                                   aCache->mTreeHwm[aDepth].mPageId,
                                                   KNL_LATCH_MODE_EXCLUSIVE );

            if( sCachedPageHandle == NULL )
            {
                STL_TRY( smpAcquire( aMiniTrans,
                                     aTbsId,
                                     aCache->mTreeHwm[aDepth].mPageId,
                                     KNL_LATCH_MODE_EXCLUSIVE,
                                     &sPageHandle,
                                     aEnv ) == STL_SUCCESS );
            }
            else
            {
                sPageHandle = *sCachedPageHandle;
            }
        }
    }
    
    sInternalHdr = (smsmbInternalHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );

    aPropagatedRid->mTbsId = aTbsId;
    aPropagatedRid->mPageId = smpGetPageId( &sPageHandle );
    aPropagatedRid->mOffset = aCache->mTreeHwm[aDepth].mOffset + 1;

    STL_TRY( smsmbInsertInternalBit( aMiniTrans,
                                     aTbsId,
                                     aCache,
                                     &sPageHandle,
                                     aDepth,
                                     aNewPageId,
                                     SMP_FREENESS_FREE,
                                     aEnv ) == STL_SUCCESS );
                                     
    sRepresentFreeness = smsmbRepresentFreeness( (void*)sInternalHdr,
                                                 STL_FALSE );
    
    STL_TRY( smsmbUpdatePageStatusInternal( aMiniTrans,
                                            aTbsId,
                                            &sPageHandle,
                                            sRepresentFreeness,
                                            aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmbExpandInternal( smxmTrans  * aMiniTrans,
                               smlTbsId     aTbsId,
                               smsmbCache * aCache,
                               stlInt16     aDepth,
                               smpHandle  * aPageHandle,
                               smlEnv     * aEnv )
{
    smlRid      sPropagatedRid;
    smpPhyHdr * sPhyHdr;
    smlRid      sLogData[2];
    
    STL_TRY( smsmbAllocInternal( aMiniTrans,
                                 aTbsId,
                                 aCache,
                                 aDepth,
                                 aPageHandle,
                                 aEnv ) == STL_SUCCESS );
    
    STL_TRY( smsmbPropagateInternal( aMiniTrans,
                                     aTbsId,
                                     aCache,
                                     aDepth + 1,
                                     smpGetPageId( aPageHandle ),
                                     &sPropagatedRid,
                                     aEnv ) == STL_SUCCESS );

    sPhyHdr = (smpPhyHdr*)SMP_FRAME( aPageHandle );
    sLogData[0] = sPhyHdr->mParentRid;
    sPhyHdr->mParentRid = sPropagatedRid;
    sLogData[1] = sPhyHdr->mParentRid;

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_RECOVERY,
                           SMR_LOG_BYTES,
                           SMR_REDO_TARGET_PAGE,
                           (void*)sLogData,
                           STL_SIZEOF( smlRid ) * 2,
                           aTbsId,
                           smpGetPageId( aPageHandle ),
                           STL_OFFSETOF( smpPhyHdr, mParentRid ),
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmbAllocLeaf( smxmTrans  * aMiniTrans,
                          smlTbsId     aTbsId,
                          smlPid       aLastLeafPid,
                          smsmbCache * aCache,
                          smpHandle  * aPageHandle,
                          smlEnv     * aEnv )
{
    smsmbLeafHdr * sLeafHdr;
    smsmbLeafHdr * sLastLeafHdr;
    smsmbLeaf    * sLeaf;
    stlInt32       i;
    smlPid         sNewPageId;
    smpHandle      sLastLeafHandle;
    smpHandle    * sCachedPageHandle;
    stlUInt16      sPageCountInExt;

    sPageCountInExt = smfGetPageCountInExt( aTbsId );
    if( aCache->mMetaHwm.mOffset + 1 >= sPageCountInExt )
    {
        STL_TRY( smsmbAllocNewExtent( smxmGetTransId(aMiniTrans),
                                      aTbsId,
                                      aCache,
                                      NULL,
                                      aEnv ) == STL_SUCCESS );
    }

    
    sNewPageId = aCache->mMetaHwm.mPageId + aCache->mMetaHwm.mOffset + 1;
    
    STL_TRY( smpCreate( aMiniTrans,
                        aTbsId,
                        sNewPageId,
                        SMP_PAGE_TYPE_BITMAP_LEAF,
                        0, /* aSegmentId */
                        0, /* aScn */
                        aPageHandle,
                        aEnv ) == STL_SUCCESS );

    aCache->mMetaHwm.mOffset++;
    sLeafHdr = (smsmbLeafHdr*)smpGetBody( SMP_FRAME( aPageHandle ) );
    sLeaf = (smsmbLeaf*)((stlChar*)sLeafHdr + STL_SIZEOF(smsmbLeafHdr));

    sLeafHdr->mChildCount = 0;
    stlMemset( sLeafHdr->mFreenessCount,
               0x00,
               STL_SIZEOF( stlInt16 ) * SMP_FREENESS_MAX );
    sLeafHdr->mNextPid = SMP_NULL_PID;
        
    for( i = 0; i < SMSMB_LEAF_BIT_COUNT; i++ )
    {
        sLeaf->mDataPid[i] = SMP_NULL_PID;
        sLeaf->mScn[i] = 0;
        smsmbSetFreeness( sLeaf->mFreeness,
                          i,
                          SMP_FREENESS_UNALLOCATED );
    }

    STL_TRY( smsmbWriteLogInitLeaf( aMiniTrans,
                                    aTbsId,
                                    sNewPageId,
                                    0,
                                    aEnv ) == STL_SUCCESS );

    if( aLastLeafPid != SMP_NULL_PID )
    {
        /*
         * 이전 leaf에 New Leaf를 연결한다.
         */
        sCachedPageHandle = smxmGetPageHandle( aMiniTrans,
                                               aTbsId,
                                               aLastLeafPid,
                                               KNL_LATCH_MODE_EXCLUSIVE );

        if( sCachedPageHandle == NULL )
        {
            STL_TRY( smpAcquire( aMiniTrans,
                                 aTbsId,
                                 aLastLeafPid,
                                 KNL_LATCH_MODE_EXCLUSIVE,
                                 &sLastLeafHandle,
                                 aEnv ) == STL_SUCCESS );
        }
        else
        {
            sLastLeafHandle = *sCachedPageHandle;
        }
        
        sLastLeafHdr = (smsmbLeafHdr*)smpGetBody( SMP_FRAME( &sLastLeafHandle ) );
        sLastLeafHdr->mNextPid = sNewPageId;

        STL_TRY( smsmbWriteLogLeafLink( aMiniTrans,
                                        aTbsId,
                                        aLastLeafPid,
                                        sNewPageId,
                                        aEnv ) == STL_SUCCESS );
    }

    aCache->mTreeHwm[0] = (smsmbHwm){ sNewPageId, -1 };

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmbAllocInternal( smxmTrans  * aMiniTrans,
                              smlTbsId     aTbsId,
                              smsmbCache * aCache,
                              stlInt16     aDepth,
                              smpHandle  * aPageHandle,
                              smlEnv     * aEnv )
{
    smsmbInternalHdr * sInternalHdr;
    smsmbInternal    * sInternal;
    stlInt32           i;
    smlPid             sNewPageId;
    stlUInt16          sPageCountInExt;

    sPageCountInExt = smfGetPageCountInExt( aTbsId );
    if( aCache->mMetaHwm.mOffset + 1 >= sPageCountInExt )
    {
        STL_TRY( smsmbAllocNewExtent( smxmGetTransId(aMiniTrans),
                                      aTbsId,
                                      aCache,
                                      NULL,
                                      aEnv ) == STL_SUCCESS );
    }

    sNewPageId = aCache->mMetaHwm.mPageId + aCache->mMetaHwm.mOffset + 1;
    STL_TRY( smpCreate( aMiniTrans,
                        aTbsId,
                        sNewPageId,
                        SMP_PAGE_TYPE_BITMAP_INTERNAL,
                        0, /* aSegmentId */
                        0, /* aScn */
                        aPageHandle,
                        aEnv ) == STL_SUCCESS );

    aCache->mMetaHwm.mOffset++;
    aCache->mTreeHwm[aDepth].mPageId = sNewPageId;
    aCache->mTreeHwm[aDepth].mOffset = -1;
    
    sInternalHdr = (smsmbInternalHdr*)smpGetBody( SMP_FRAME( aPageHandle ) );
    sInternal = (smsmbInternal*)((stlChar*)sInternalHdr + STL_SIZEOF(smsmbInternalHdr));

    sInternalHdr->mChildCount = 0;
    stlMemset( sInternalHdr->mFreenessCount,
               0x00,
               STL_SIZEOF( stlInt16 ) * SMP_FREENESS_MAX );
        
    for( i = 0; i < SMSMB_INTERNAL_BIT_COUNT; i++ )
    {
        sInternal->mChildPid[i] = SMP_NULL_PID;
        smsmbSetFreeness( sInternal->mFreeness,
                          i,
                          SMP_FREENESS_UNALLOCATED );
    }

    STL_TRY( smsmbWriteLogInitInternal( aMiniTrans,
                                        aTbsId,
                                        sNewPageId,
                                        0,
                                        aEnv ) == STL_SUCCESS );
    
    aCache->mTreeHwm[aDepth] = (smsmbHwm){ sNewPageId, -1 };
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmbFindInsertablePage( smxmTrans       * aMiniTrans,
                                   smlTbsId          aTbsId,
                                   smsmbCache      * aCache,
                                   smpFreeness       aFreeness,
                                   smlPid            aStartPid,
                                   stlBool           aUseInsertablePageHint,
                                   smpIsAgableFunc   aIsAgableFunc,
                                   stlInt32        * aDataPidCount,
                                   smlPid          * aDataPageId,
                                   smlRid          * aLeafRid,        
                                   smlEnv          * aEnv )
{
    smlPid             sPageId;
    smpHandle          sPageHandle;
    smpPageType        sPageType;
    stlInt32           sState = 0;
    smsmbInternalHdr * sInternalHdr;
    smsmbInternal    * sInternal;
    smsmbLeafHdr     * sLeafHdr;
    smsmbLeaf        * sLeaf;
    smlRid             sBitRid;
    smpHandle        * sCachedPageHandle;
    stlInt64           sSegmentId;
 
    sPageId = aStartPid;
    
    *aLeafRid = SML_INVALID_RID;
    *aDataPidCount = 0;

    sSegmentId = smsGetSegmentId( aCache );
                         
    while( 1 )
    {
        sCachedPageHandle = smxmGetPageHandle( aMiniTrans,
                                               aTbsId,
                                               sPageId,
                                               KNL_LATCH_MODE_SHARED );

        if( sCachedPageHandle != NULL )
        {
            sPageHandle = *sCachedPageHandle;
        }
        else
        {
            STL_TRY( smpAcquire( NULL,
                                 aTbsId,
                                 sPageId,
                                 KNL_LATCH_MODE_SHARED,
                                 &sPageHandle,
                                 aEnv ) == STL_SUCCESS );
            sState = 1;
        }

        sPageType = smpGetPageType( &sPageHandle );

        if( sPageType == SMP_PAGE_TYPE_BITMAP_LEAF )
        {
            break;
        }
        
        KNL_ASSERT( sPageType == SMP_PAGE_TYPE_BITMAP_INTERNAL,
                    KNL_ENV(aEnv),
                    ( "PAGE_TYPE(%d), TBS_ID(%d), PAGE_ID(%d), PHYSICAL_ID(%ld)\nPAGE BODY\n%s",
                      sPageType,
                      aTbsId,
                      sPageId,
                      sSegmentId,
                      knlDumpBinaryForAssert( SMP_FRAME(&sPageHandle),
                                              SMP_PAGE_SIZE,
                                              &aEnv->mLogHeapMem,
                                              KNL_ENV(aEnv))) );

        sInternalHdr = (smsmbInternalHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );
        sInternal = (smsmbInternal*)((stlChar*)sInternalHdr + STL_SIZEOF(smsmbInternalHdr));

        STL_TRY( smsmbFindChildAndUpdateHintInternal( aTbsId,
                                                      &sPageHandle,
                                                      aFreeness,
                                                      sInternalHdr->mChildCount,
                                                      sInternal->mChildPid,
                                                      sInternal->mFreeness,
                                                      aUseInsertablePageHint,
                                                      &sPageId,
                                                      &sBitRid,
                                                      aEnv )
                 == STL_SUCCESS );
        
        STL_TRY_THROW( sPageId != SMP_NULL_PID, RAMP_SUCCESS );

        if( sState == 1 )
        {
            sState = 0;
            STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
        }
    }

    while( 1 )
    {
        sLeafHdr = (smsmbLeafHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );
        sLeaf = (smsmbLeaf*)((stlChar*)sLeafHdr + STL_SIZEOF(smsmbLeafHdr));
    
        if( (sLeafHdr->mFreenessCount[SMP_FREENESS_INSERTABLE] +
             sLeafHdr->mFreenessCount[SMP_FREENESS_FREE]) > 0 )
        {
            STL_TRY( smsmbFindChildAndUpdateHintLeaf( aTbsId,
                                                      &sPageHandle,
                                                      aFreeness,
                                                      sLeafHdr->mChildCount,
                                                      sLeaf->mDataPid,
                                                      sLeaf->mFreeness,
                                                      sLeaf->mScn,
                                                      aUseInsertablePageHint,
                                                      aIsAgableFunc,
                                                      aDataPidCount,
                                                      aDataPageId,
                                                      aLeafRid,
                                                      aEnv )
                     == STL_SUCCESS );

            if( *aDataPidCount > 0 )
            {
                break;
            }
        }

        /**
         * Insertable Data Page가 없다면 다음 Leaf로 이동
         */
        sPageId = sLeafHdr->mNextPid;

        /**
         * 더이상 leaf page가 없다면 종료
         */
        if( sPageId == SMP_NULL_PID )
        {
            break;
        }
        
        if( sState == 1 )
        {
            sState = 0;
            STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
        }

        sCachedPageHandle = smxmGetPageHandle( aMiniTrans,
                                               aTbsId,
                                               sPageId,
                                               KNL_LATCH_MODE_SHARED );

        if( sCachedPageHandle != NULL )
        {
            sPageHandle = *sCachedPageHandle;
        }
        else
        {
            STL_TRY( smpAcquire( NULL,
                                 aTbsId,
                                 sPageId,
                                 KNL_LATCH_MODE_SHARED,
                                 &sPageHandle,
                                 aEnv ) == STL_SUCCESS );
            sState = 1;
        }
    }

    STL_RAMP( RAMP_SUCCESS );

    if( sState == 1 )
    {
        sState = 0;
        STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpRelease( &sPageHandle, aEnv );
    }

    return STL_FAILURE;
}
                                   
stlStatus smsmbFindFreePage( smxmTrans       * aMiniTrans,
                             smlTbsId          aTbsId,
                             smlPid            aStartPid,
                             stlBool           aUseInsertableHint,
                             smpIsAgableFunc   aIsAgableFunc,
                             smlPid          * aDataPageId,
                             smlRid          * aLeafRid,
                             smlEnv          * aEnv )
{
    smlPid             sPageId;
    smpHandle          sPageHandle;
    smpPageType        sPageType;
    stlInt32           sState = 0;
    smsmbInternalHdr * sInternalHdr;
    smsmbInternal    * sInternal;
    smsmbLeafHdr     * sLeafHdr;
    smsmbLeaf        * sLeaf;
    smlRid             sBitRid;
    smpHandle        * sCachedPageHandle;

    sPageId = aStartPid;
    
    *aLeafRid = SML_INVALID_RID;
    *aDataPageId = SMP_NULL_PID;

    /**
     * 첫번째 Free Leaf Page를 찾는다.
     */
    while( 1 )
    {
        sCachedPageHandle = smxmGetPageHandle( aMiniTrans,
                                               aTbsId,
                                               sPageId,
                                               KNL_LATCH_MODE_SHARED );

        if( sCachedPageHandle != NULL )
        {
            sPageHandle = *sCachedPageHandle;
        }
        else
        {
            STL_TRY( smpAcquire( NULL,
                                 aTbsId,
                                 sPageId,
                                 KNL_LATCH_MODE_SHARED,
                                 &sPageHandle,
                                 aEnv ) == STL_SUCCESS );
            sState = 1;
        }

        sPageType = smpGetPageType( &sPageHandle );

        if( sPageType == SMP_PAGE_TYPE_BITMAP_LEAF )
        {
            break;
        }
        
        STL_ASSERT( sPageType == SMP_PAGE_TYPE_BITMAP_INTERNAL );

        sInternalHdr = (smsmbInternalHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );
        sInternal = (smsmbInternal*)((stlChar*)sInternalHdr + STL_SIZEOF(smsmbInternalHdr));

        STL_TRY( smsmbFindFirstFreePage( aTbsId,
                                         &sPageHandle,
                                         sInternalHdr->mChildCount,
                                         sInternal->mChildPid,
                                         sInternal->mFreeness,
                                         NULL,   /* aScnArray */
                                         aUseInsertableHint,
                                         aIsAgableFunc,
                                         &sPageId,
                                         &sBitRid,
                                         aEnv )
                 == STL_SUCCESS );
        
        STL_TRY_THROW( sPageId != SMP_NULL_PID, RAMP_SUCCESS );

        if( sState == 1 )
        {
            sState = 0;
            STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
        }
    }

    /**
     * Free Data Page를 찾는다.
     */
    while( 1 )
    {
        sLeafHdr = (smsmbLeafHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );
        sLeaf = (smsmbLeaf*)((stlChar*)sLeafHdr + STL_SIZEOF(smsmbLeafHdr));

        if( sLeafHdr->mFreenessCount[SMP_FREENESS_FREE] > 0 )
        {
            STL_TRY( smsmbFindFirstFreePage( aTbsId,
                                             &sPageHandle,
                                             sLeafHdr->mChildCount,
                                             sLeaf->mDataPid,
                                             sLeaf->mFreeness,
                                             sLeaf->mScn,
                                             aUseInsertableHint,
                                             aIsAgableFunc,
                                             aDataPageId,
                                             aLeafRid,
                                             aEnv )
                     == STL_SUCCESS );

            if( *aDataPageId != SMP_NULL_PID )
            {
                break;
            }
        }

        /**
         * Free Data Page가 없다면 다음 Leaf로 이동
         */
        sPageId = sLeafHdr->mNextPid;

        /**
         * 더이상 leaf page가 없다면 종료
         */
        if( sPageId == SMP_NULL_PID )
        {
            break;
        }
        
        if( sState == 1 )
        {
            sState = 0;
            STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
        }

        sCachedPageHandle = smxmGetPageHandle( aMiniTrans,
                                               aTbsId,
                                               sPageId,
                                               KNL_LATCH_MODE_SHARED );

        if( sCachedPageHandle != NULL )
        {
            sPageHandle = *sCachedPageHandle;
        }
        else
        {
            STL_TRY( smpAcquire( NULL,
                                 aTbsId,
                                 sPageId,
                                 KNL_LATCH_MODE_SHARED,
                                 &sPageHandle,
                                 aEnv ) == STL_SUCCESS );
            sState = 1;
        }
    }
    
    STL_RAMP( RAMP_SUCCESS );

    if( sState == 1 )
    {
        sState = 0;
        STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpRelease( &sPageHandle, aEnv );
    }

    return STL_FAILURE;
}
                                   
stlStatus smsmbInsertInternalBit( smxmTrans   * aMiniTrans,
                                  smlTbsId      aTbsId,
                                  smsmbCache  * aCache,
                                  smpHandle   * aPageHandle,
                                  stlInt16      aDepth,
                                  smlPid        aInsertPid,
                                  smpFreeness   aInsertFreeness,
                                  smlEnv      * aEnv )
{
    smsmbInternalHdr * sInternalHdr;
    smsmbInternal    * sInternal;
    
    sInternalHdr = (smsmbInternalHdr*)smpGetBody( SMP_FRAME( aPageHandle ) );
    sInternal = (smsmbInternal*)((stlChar*)sInternalHdr +
                                 STL_SIZEOF(smsmbInternalHdr));

    sInternalHdr->mChildCount += 1;
    smsmbSetFreeness( sInternal->mFreeness,
                      aCache->mTreeHwm[aDepth].mOffset + 1,
                      aInsertFreeness );

    sInternal->mChildPid[aCache->mTreeHwm[aDepth].mOffset + 1] = aInsertPid;
    sInternalHdr->mFreenessCount[aInsertFreeness] += 1;
    
    STL_TRY( smsmbWriteLogInsertInternalBit( aMiniTrans,
                                             aTbsId,
                                             aCache->mTreeHwm[aDepth].mPageId,
                                             aCache->mTreeHwm[aDepth].mOffset + 1,
                                             aInsertPid,
                                             aInsertFreeness,
                                             aEnv ) == STL_SUCCESS );
    
    aCache->mTreeHwm[aDepth].mOffset += 1;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmbFindFirstFreePage( smlTbsId          aTbsId,
                                  smpHandle       * aPageHandle,
                                  stlInt16          aChildCount,
                                  smlPid          * aChildArray,
                                  stlChar         * aFreenessArray,
                                  smlScn          * aScnArray,
                                  stlBool           aUseInsertableHint,
                                  smpIsAgableFunc   aIsAgableFunc,
                                  smlPid          * aChildPid,
                                  smlRid          * aBitRid,
                                  smlEnv          * aEnv )
{
    stlInt32      i;
    smpFreeness   sFreeness;
    smlPid        sPageId;
    stlInt32      sHintOffset;
    stlBool       sIsSuccess;

    *aChildPid = SMP_NULL_PID;
    *aBitRid = SML_INVALID_RID;
    
    sPageId = smpGetPageId( aPageHandle );
    sHintOffset = SMSMB_GET_HINT_OFFSET( aPageHandle );

    if( aUseInsertableHint == STL_TRUE )
    {
        for( i = sHintOffset; i < aChildCount; i++ )
        {
            sFreeness = smsmbGetFreeness( aFreenessArray, i );
        
            if( sFreeness < SMP_FREENESS_FREE )
            {
                continue;
            }

            /**
             * free될 당시 살아 있던 update transaction과 read-only statement가
             * 종료되었는지 검사한다.
             */
            if( (aScnArray != NULL) &&
                (smsmbIsAgableForAlloc(aTbsId,
                                       aScnArray[i],
                                       aChildArray[i],
                                       aIsAgableFunc,
                                       aEnv) == STL_FALSE) )
            {
                continue;
            }

            if( aScnArray != NULL )
            {
                STL_TRY( smpTestExclusive( aTbsId,
                                           aChildArray[i],
                                           &sIsSuccess,
                                           aEnv )
                         == STL_SUCCESS );
                
                if( sIsSuccess == STL_FALSE )
                {
                    continue;
                }
            }
            
            *aChildPid = aChildArray[i];
            SMSMB_SET_HINT_OFFSET( aPageHandle, i+1 );
            *aBitRid = (smlRid){ aTbsId, i, sPageId };
                    
            STL_THROW( RAMP_SUCCESS );
        }
    
        for( i = 0; i < sHintOffset; i++ )
        {
            sFreeness = smsmbGetFreeness( aFreenessArray, i );
        
            if( sFreeness < SMP_FREENESS_FREE )
            {
                continue;
            }

            /**
             * free될 당시 살아 있던 update transaction과 read-only statement가
             * 종료되었는지 검사한다.
             */
            if( (aScnArray != NULL) &&
                (smsmbIsAgableForAlloc(aTbsId,
                                       aScnArray[i],
                                       aChildArray[i],
                                       aIsAgableFunc,
                                       aEnv) == STL_FALSE) )
            {
                continue;
            }

            if( aScnArray != NULL )
            {
                STL_TRY( smpTestExclusive( aTbsId,
                                           aChildArray[i],
                                           &sIsSuccess,
                                           aEnv )
                         == STL_SUCCESS );
                
                if( sIsSuccess == STL_FALSE )
                {
                    continue;
                }
            }
            
            *aChildPid = aChildArray[i];
            SMSMB_SET_HINT_OFFSET( aPageHandle, i+1 );
            *aBitRid = (smlRid){ aTbsId, i, sPageId };
            break;
        }
    }
    else
    {
        for( i = 0; i < aChildCount; i++ )
        {
            sFreeness = smsmbGetFreeness( aFreenessArray, i );
        
            if( sFreeness < SMP_FREENESS_FREE )
            {
                continue;
            }

            /**
             * free될 당시 살아 있던 update transaction과 read-only statement가
             * 종료되었는지 검사한다.
             */
            if( (aScnArray != NULL) &&
                (smsmbIsAgableForAlloc(aTbsId,
                                       aScnArray[i],
                                       aChildArray[i],
                                       aIsAgableFunc,
                                       aEnv) == STL_FALSE) )
            {
                continue;
            }

            if( aScnArray != NULL )
            {
                STL_TRY( smpTestExclusive( aTbsId,
                                           aChildArray[i],
                                           &sIsSuccess,
                                           aEnv )
                         == STL_SUCCESS );
                
                if( sIsSuccess == STL_FALSE )
                {
                    continue;
                }
            }
            
            *aChildPid = aChildArray[i];
            SMSMB_SET_HINT_OFFSET( aPageHandle, i+1 );
            *aBitRid = (smlRid){ aTbsId, i, sPageId };
            break;
        }
    }

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmbFindChildAndUpdateHintInternal( smlTbsId      aTbsId,
                                               smpHandle   * aPageHandle,
                                               smpFreeness   aFreeness,
                                               stlInt16      aChildCount,
                                               smlPid      * aChildArray,
                                               stlChar     * aFreenessArray,
                                               stlBool       aUseInsertablePageHint,
                                               smlPid      * aChildPid,
                                               smlRid      * aBitRid,
                                               smlEnv      * aEnv )
{
    stlInt32      i;
    smpFreeness   sFreeness;
    stlInt32      sHintOffset;
    smlPid        sPageId;

    *aChildPid = SMP_NULL_PID;
    *aBitRid = SML_INVALID_RID;

    sPageId = smpGetPageId( aPageHandle );
    sHintOffset = SMSMB_GET_HINT_OFFSET( aPageHandle );
    
    if( aUseInsertablePageHint == STL_TRUE )
    {
        for( i = sHintOffset; i < aChildCount; i++ )
        {
            sFreeness = smsmbGetFreeness( aFreenessArray, i );
        
            if( sFreeness >= aFreeness )
            {
                *aChildPid = aChildArray[i];
                SMSMB_SET_HINT_OFFSET( aPageHandle, i+1 );
                *aBitRid = (smlRid){ aTbsId, i, sPageId };
            
                STL_THROW( RAMP_SUCCESS );
            }
        }
    
        for( i = 0; i < sHintOffset; i++ )
        {
            sFreeness = smsmbGetFreeness( aFreenessArray, i );
        
            if( sFreeness >= aFreeness )
            {
                *aChildPid = aChildArray[i];
                SMSMB_SET_HINT_OFFSET( aPageHandle, i+1 );
                *aBitRid = (smlRid){ aTbsId, i, sPageId };
                break;
            }
        }
    }
    else
    {
        for( i = 0; i < aChildCount; i++ )
        {
            sFreeness = smsmbGetFreeness( aFreenessArray, i );
        
            if( sFreeness >= aFreeness )
            {
                *aChildPid = aChildArray[i];
                SMSMB_SET_HINT_OFFSET( aPageHandle, i+1 );
                *aBitRid = (smlRid){ aTbsId, i, sPageId };
                break;
            }
        }
    }
    
    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;
}

stlStatus smsmbFindChildAndUpdateHintLeaf( smlTbsId          aTbsId,
                                           smpHandle       * aPageHandle,
                                           smpFreeness       aFreeness,
                                           stlInt16          aChildCount,
                                           smlPid          * aChildArray,
                                           stlChar         * aFreenessArray,
                                           smlScn          * aScnArray,
                                           stlBool           aUseInsertablePageHint,
                                           smpIsAgableFunc   aIsAgableFunc,
                                           stlInt32        * aChildPidCount,
                                           smlPid          * aChildPid,
                                           smlRid          * aBitRid,
                                           smlEnv          * aEnv )
{
    stlInt32      i;
    smpFreeness   sFreeness;
    stlInt32      sHintOffset;
    smlPid        sPageId;
    stlInt32      sChildPidCount = 0;

    *aChildPid = SMP_NULL_PID;
    *aBitRid = SML_INVALID_RID;

    sPageId = smpGetPageId( aPageHandle );
    sHintOffset = SMSMB_GET_HINT_OFFSET( aPageHandle );
    
    if( aUseInsertablePageHint == STL_TRUE )
    {
        for( i = sHintOffset; i < aChildCount; i++ )
        {
            sFreeness = smsmbGetFreeness( aFreenessArray, i );
        
            if( sFreeness >= aFreeness )
            {
                if( sFreeness == SMP_FREENESS_FREE )
                {
                    if( (aScnArray != NULL) &&
                        (smsmbIsAgableForAlloc(aTbsId,
                                               aScnArray[i],
                                               aChildArray[i],
                                               aIsAgableFunc,
                                               aEnv) == STL_FALSE) )
                    {
                        continue;
                    }
                }
                
                aChildPid[sChildPidCount] = aChildArray[i];
                aBitRid[sChildPidCount] = (smlRid){ aTbsId, i, sPageId };
                sChildPidCount++;
                SMSMB_SET_HINT_OFFSET( aPageHandle, i+1 );

                STL_TRY_THROW( sChildPidCount != SMS_MAX_PAGE_HINT_COUNT, RAMP_SUCCESS );
            }
        }
    
        for( i = 0; i < sHintOffset; i++ )
        {
            sFreeness = smsmbGetFreeness( aFreenessArray, i );
        
            if( sFreeness >= aFreeness )
            {
                if( sFreeness == SMP_FREENESS_FREE )
                {
                    if( (aScnArray != NULL) &&
                        (smsmbIsAgableForAlloc(aTbsId,
                                               aScnArray[i],
                                               aChildArray[i],
                                               aIsAgableFunc,
                                               aEnv) == STL_FALSE) )
                    {
                        continue;
                    }
                }
                
                aChildPid[sChildPidCount] = aChildArray[i];
                aBitRid[sChildPidCount] = (smlRid){ aTbsId, i, sPageId };
                sChildPidCount++;
                SMSMB_SET_HINT_OFFSET( aPageHandle, i+1 );
                
                STL_TRY_THROW( sChildPidCount != SMS_MAX_PAGE_HINT_COUNT, RAMP_SUCCESS );
            }
        }
    }
    else
    {
        for( i = 0; i < aChildCount; i++ )
        {
            sFreeness = smsmbGetFreeness( aFreenessArray, i );
        
            if( sFreeness >= aFreeness )
            {
                if( sFreeness == SMP_FREENESS_FREE )
                {
                    if( (aScnArray != NULL) &&
                        (smsmbIsAgableForAlloc(aTbsId,
                                               aScnArray[i],
                                               aChildArray[i],
                                               aIsAgableFunc,
                                               aEnv) == STL_FALSE) )
                    {
                        continue;
                    }
                }
                
                aChildPid[sChildPidCount] = aChildArray[i];
                aBitRid[sChildPidCount] = (smlRid){ aTbsId, i, sPageId };
                sChildPidCount++;
                SMSMB_SET_HINT_OFFSET( aPageHandle, i+1 );
                
                STL_TRY_THROW( sChildPidCount != SMS_MAX_PAGE_HINT_COUNT, RAMP_SUCCESS );
                break;
            }
        }
    }
    
    STL_RAMP( RAMP_SUCCESS );

    *aChildPidCount = sChildPidCount;

    return STL_SUCCESS;
}

/** @} */
