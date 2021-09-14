/*******************************************************************************
 * smsmbExtMap.c
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
#include <smsmbExtMap.h>
#include <smsmbSpaceTree.h>
#include <smsmbLog.h>

/**
 * @file smsmbExtMap.c
 * @brief Storage Manager Layer Bitmap Extent Map Internal Routines
 */

/**
 * @addtogroup smsmbExtMap
 * @{
 */

stlStatus smsmbAllocNewExtent( smxlTransId   aTransId,
                               smlTbsId      aTbsId,
                               smsmbCache  * aCache,
                               smlPid      * aFirstDataPid,
                               smlEnv      * aEnv )
{
    smpHandle        sPageHandle;
    smlPid           sFirstPid;
    smlExtId         sExtId;
    smlExtId         sMetaExtId = SML_INVALID_EXTID;
    smsmbExtMapHdr * sExtMapHdr;
    smlPid           sExtMapPid = SMP_NULL_PID;
    smlPid           sMetaPid = SMP_NULL_PID;
    smsmbHwm         sMetaHwm;
    stlUInt32        sPageCountInExt;
    smsmbSegHdr    * sSegHdr;
    stlInt32         sState = 0;
    smxmTrans        sMiniTrans;
    stlInt64         sSegmentId;
    stlUInt32        sAttr = SMXM_ATTR_REDO;

    if( smtIsLogging( aTbsId ) == STL_TRUE )
    {
        sAttr = SMXM_ATTR_REDO;
    }
    else
    {
        sAttr = SMXM_ATTR_NOLOG_DIRTY;
    }
    
    sPageCountInExt = smfGetPageCountInExt( aTbsId );
    sSegmentId = smsGetSegmentId( aCache );

    STL_TRY( smxmBegin( &sMiniTrans,
                        aTransId,
                        smsGetSegRid( aCache ),
                        sAttr,
                        aEnv ) == STL_SUCCESS );
    sState = 1;

    if( aCache->mLstExtMap == SMP_NULL_PID )
    {
        /**
         * 최초 Extent가 할당되는 경우
         */
        STL_TRY( smtAllocExtent( &sMiniTrans,
                                 aTbsId,
                                 &sExtId,
                                 &sFirstPid,
                                 aEnv ) == STL_SUCCESS );

        STL_TRY( smpCreate( &sMiniTrans,
                            aTbsId,
                            sFirstPid,
                            SMP_PAGE_TYPE_BITMAP_EXTENT_MAP,
                            sSegmentId,
                            0, /* aScn */
                            &sPageHandle,
                            aEnv ) == STL_SUCCESS );
    
        STL_TRY( smsmbInitExtMap( &sMiniTrans,
                                  aTbsId,
                                  &sPageHandle,
                                  SMP_NULL_PID,
                                  aEnv ) == STL_SUCCESS );

        sExtMapPid = sFirstPid;
        sMetaHwm = (smsmbHwm){ sFirstPid, 0 };

        STL_THROW( RAMP_ADD_EXTENT );
    }
    
    /**
     * 발생할수 있는 모든 CASE는 Extent Descriptor 개수와 Meta Page 개수로 구분한다.
     * (CASE-1) Extent Descriptor가 Full이고, Meta Page가 Full인 경우
     * (CASE-2) Extent Descriptor가 Full이고, Meta Page의 여유공간이 있는 경우
     * (CASE-3) Extent Descriptor의 여유공간이 있고, Meta Page가 Full인 경우
     * (CASE-4) Extent Descriptor와 Meta Page 모두 여유공간이 있는 경우
     * 상하의 관계로 인한 복잡도가 상당히 높은 모듈이기 때문에 최대의 단순화가 필요하다.
     * 많은 if문보다는 코드의 중복을 허락함으로써 로직을 단순화시킨다.
     */
    sMetaHwm = aCache->mMetaHwm;

    STL_TRY( smpAcquire( &sMiniTrans,
                         aTbsId,
                         aCache->mLstExtMap,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv )
             == STL_SUCCESS );
        
    sExtMapHdr = (smsmbExtMapHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );

    /**
     * [CASE-3]에서 필요한 Extent Descriptor를 확보하기 위해서
     * 2 만큼의 Extent Descriptor가 필요하다.
     */
    if( (sExtMapHdr->mExtDescCount + 2) > SMSMB_EXTENT_DESC_COUNT )
    {
        /**
         * (CASE-1,2) Extent Descriptor가 부족한 경우
         */
        if( (aCache->mMetaHwm.mOffset + 1) >= sPageCountInExt )
        {
            STL_TRY_THROW( aCache->mCommonCache.mMaxExtents >
                           aCache->mCommonCache.mAllocExtCount,
                           RAMP_ERR_EXCEED_MAXSIZE );
    
            /**
             * (CASE-1) Extent for Extent Descriptor가 필요한 경우
             * - 2개의 Extent를 만들어야 하고, 2번의 Mini-transaction을 사용한다.
             */
            STL_TRY( smtAllocExtent( &sMiniTrans,
                                     aTbsId,
                                     &sMetaExtId,
                                     &sMetaPid,
                                     aEnv ) == STL_SUCCESS );
                
            sMetaHwm = (smsmbHwm){ sMetaPid, 0};
            sExtMapPid = sMetaHwm.mPageId + sMetaHwm.mOffset;
                
            STL_TRY( smpCreate( &sMiniTrans,
                                aTbsId,
                                sExtMapPid,
                                SMP_PAGE_TYPE_BITMAP_EXTENT_MAP,
                                sSegmentId,
                                0, /* aScn */
                                &sPageHandle,
                                aEnv ) == STL_SUCCESS );
            
            STL_TRY( smsmbInitExtMap( &sMiniTrans,
                                      aTbsId,
                                      &sPageHandle,
                                      aCache->mLstExtMap,
                                      aEnv ) == STL_SUCCESS );
                
            STL_TRY( smsmbAddExtent( &sMiniTrans,
                                     aTbsId,
                                     &sPageHandle,
                                     sMetaExtId,
                                     aEnv ) == STL_SUCCESS );

            /**
             * Segment Header페이지에 로깅한다.
             * - 상위에서 Latch가 획득된 상태이고, 한번더 Latch를 획득해도 무방하다.
             */
            aCache->mMetaHwm = sMetaHwm;
            aCache->mCommonCache.mAllocExtCount += 1;
                
            STL_TRY( smpAcquire( &sMiniTrans,
                                 aCache->mCommonCache.mSegRid.mTbsId,
                                 aCache->mCommonCache.mSegRid.mPageId,
                                 KNL_LATCH_MODE_EXCLUSIVE,
                                 &sPageHandle,
                                 aEnv ) == STL_SUCCESS );

            sSegHdr = (smsmbSegHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );

            SMSMB_COPY_SEGHDR_FROM_CACHE( sSegHdr, aCache );
        
            if( sAttr == SMXM_ATTR_REDO )
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
                
            STL_TRY( smxmBegin( &sMiniTrans,
                                aTransId,
                                smsGetSegRid( aCache ),
                                sAttr,
                                aEnv ) == STL_SUCCESS );
            sState = 1;
                
            STL_TRY( smpAcquire( &sMiniTrans,
                                 aTbsId,
                                 sExtMapPid,
                                 KNL_LATCH_MODE_EXCLUSIVE,
                                 &sPageHandle,
                                 aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /**
             * (CASE-2) New Extent Map 페이지가 필요한 경우
             */
            sMetaHwm.mOffset++;
            sExtMapPid = sMetaHwm.mPageId + sMetaHwm.mOffset;
                
            STL_TRY( smpCreate( &sMiniTrans,
                                aTbsId,
                                sExtMapPid,
                                SMP_PAGE_TYPE_BITMAP_EXTENT_MAP,
                                sSegmentId,
                                0, /* aScn */
                                &sPageHandle,
                                aEnv ) == STL_SUCCESS );
            
            STL_TRY( smsmbInitExtMap( &sMiniTrans,
                                      aTbsId,
                                      &sPageHandle,
                                      aCache->mLstExtMap,
                                      aEnv ) == STL_SUCCESS );
        }
    }
    else
    {
        /**
         * (CASE-3,4) Extent Descriptor가 충분한 경우
         */
        if( (aCache->mMetaHwm.mOffset + 1) >= sPageCountInExt )
        {
            STL_TRY_THROW( aCache->mCommonCache.mMaxExtents >
                           aCache->mCommonCache.mAllocExtCount,
                           RAMP_ERR_EXCEED_MAXSIZE );
    
            /**
             * (CASE-3) Bitmap Leaf or Internal을 위한 Meta extent가 필요한 경우
             * - 2개의 Extent를 만들어야 하고, 2번의 Mini-transaction을 사용한다.
             */
            STL_TRY( smtAllocExtent( &sMiniTrans,
                                     aTbsId,
                                     &sMetaExtId,
                                     &sMetaPid,
                                     aEnv ) == STL_SUCCESS );
                
            sMetaHwm = (smsmbHwm){ sMetaPid, -1};
                
            STL_TRY( smsmbAddExtent( &sMiniTrans,
                                     aTbsId,
                                     &sPageHandle,
                                     sMetaExtId,
                                     aEnv ) == STL_SUCCESS );

            /**
             * Segment Header페이지에 로깅한다.
             * - 상위에서 Latch가 획득된 상태이고, 한번더 Latch를 획득해도 무방하다.
             */
            aCache->mMetaHwm = sMetaHwm;
            aCache->mCommonCache.mAllocExtCount += 1;
                
            STL_TRY( smpAcquire( &sMiniTrans,
                                 aCache->mCommonCache.mSegRid.mTbsId,
                                 aCache->mCommonCache.mSegRid.mPageId,
                                 KNL_LATCH_MODE_EXCLUSIVE,
                                 &sPageHandle,
                                 aEnv ) == STL_SUCCESS );

            sSegHdr = (smsmbSegHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );

            SMSMB_COPY_SEGHDR_FROM_CACHE( sSegHdr, aCache );
        
            if( sAttr == SMXM_ATTR_REDO )
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
                
            STL_TRY( smxmBegin( &sMiniTrans,
                                aTransId,
                                smsGetSegRid( aCache ),
                                sAttr,
                                aEnv ) == STL_SUCCESS );
            sState = 1;
                
            STL_TRY( smpAcquire( &sMiniTrans,
                                 aTbsId,
                                 aCache->mLstExtMap,
                                 KNL_LATCH_MODE_EXCLUSIVE,
                                 &sPageHandle,
                                 aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /**
             * (CASE-4) Nothing to do
             */
        }
    }
        
    STL_TRY_THROW( aCache->mCommonCache.mMaxExtents >
                   aCache->mCommonCache.mAllocExtCount,
                   RAMP_ERR_EXCEED_MAXSIZE );
    
    STL_TRY( smtAllocExtent( &sMiniTrans,
                             aTbsId,
                             &sExtId,
                             &sFirstPid,
                             aEnv ) == STL_SUCCESS );

    STL_RAMP( RAMP_ADD_EXTENT );
    
    STL_TRY( smsmbAddExtent( &sMiniTrans,
                             aTbsId,
                             &sPageHandle,
                             sExtId,
                             aEnv ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
    
    if( sExtMapPid != SMP_NULL_PID )
    {
        aCache->mLstExtMap = sExtMapPid;
        
        if( aCache->mFstExtMap == SMP_NULL_PID )
        {
            aCache->mFstExtMap = sExtMapPid;
        }
    }

    aCache->mMetaHwm = sMetaHwm;
    aCache->mCommonCache.mAllocExtCount += 1;

    if( aFirstDataPid != NULL )
    {
        *aFirstDataPid = sFirstPid;
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_EXCEED_MAXSIZE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_EXCEED_SEGMENT_MAXSIZE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      ( ((stlInt64)sPageCountInExt) *
                        aCache->mCommonCache.mMaxExtents *
                        SMP_PAGE_SIZE) );
    }
    
    STL_FINISH;

    if( sState == 1 )
    {
        /**
         * smtAllocExtent이외의 부분에서는 에러가 발생하지 않는다고
         * 가정한다.
         */
        smxmCommit( &sMiniTrans, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smsmbInitExtMap( smxmTrans  * aMiniTrans,
                           smlTbsId     aTbsId,
                           smpHandle  * aPageHandle,
                           smlPid       aPrvMapPid,
                           smlEnv     * aEnv )
{
    smsmbExtMapHdr * sExtMapHdr;
    smsmbExtMap    * sExtMap;
    stlInt32         i;
    
    sExtMapHdr = (smsmbExtMapHdr*)smpGetBody( SMP_FRAME( aPageHandle ) );
    sExtMap = (smsmbExtMap*)((stlChar*)sExtMapHdr + STL_SIZEOF(smsmbExtMapHdr));

    sExtMapHdr->mPrvExtMap = aPrvMapPid;
    sExtMapHdr->mExtDescCount = 0;

    for( i = 0; i < SMSMB_EXTENT_DESC_COUNT; i++ )
    {
        sExtMap->mExtId[i] = SML_INVALID_EXTID;
    }

    STL_TRY( smsmbWriteLogInitExtMap( aMiniTrans,
                                      aTbsId,
                                      smpGetPageId( aPageHandle ),
                                      0,
                                      sExtMapHdr->mPrvExtMap,
                                      aEnv ) == STL_SUCCESS );
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmbAddExtent( smxmTrans  * aMiniTrans,
                          smlTbsId     aTbsId,
                          smpHandle  * aPageHandle,
                          smlExtId     aExtId,
                          smlEnv     * aEnv )
{
    smsmbExtMapHdr * sExtMapHdr;
    smsmbExtMap    * sExtMap;
    
    sExtMapHdr = (smsmbExtMapHdr*)smpGetBody( SMP_FRAME( aPageHandle ) );
    sExtMap = (smsmbExtMap*)((stlChar*)sExtMapHdr + STL_SIZEOF(smsmbExtMapHdr));

    STL_ASSERT( sExtMapHdr->mExtDescCount < SMSMB_EXTENT_DESC_COUNT );
    
    sExtMap->mExtId[sExtMapHdr->mExtDescCount] = aExtId;

    STL_TRY( smsmbWriteLogAddExt( aMiniTrans,
                                  aTbsId,
                                  smpGetPageId( aPageHandle ),
                                  sExtMapHdr->mExtDescCount,
                                  aExtId,
                                  aEnv ) == STL_SUCCESS );

    sExtMapHdr->mExtDescCount++;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
    

/** @} */
