/*******************************************************************************
 * smsUndo.c
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
#include <smp.h>
#include <smxl.h>
#include <smxm.h>
#include <smf.h>
#include <sms.h>
#include <smsDef.h>
#include <smsUndo.h>
#include <smsmpDef.h>

/**
 * @file smsUndo.c
 * @brief Storage Manager Table Undo Internal Routines
 */

/**
 * @addtogroup smsUndo
 * @{
 */

smxlUndoFunc gSmsUndoFuncs[SMS_UNDO_LOG_MAX_COUNT] =
{
    smsUndoAlterSegmentAttr,
    smsUndoSetUnusableSegment,
    smsUndoSetUsableSegment,
    smsUndoResetPendingSegment
};

stlStatus smsUndoAlterSegmentAttr( smxmTrans * aMiniTrans,
                                   smlRid      aTargetRid,
                                   void      * aLogBody,
                                   stlInt16    aLogSize,
                                   smlEnv    * aEnv )
{
    smpHandle             sPageHandle;
    smsSegHdr           * sSegmentHeader;
    smlSegmentAttr        sSegmentAttr;
    void                * sSegmentHandle;
    smsCommonCache      * sCache;
    smlRid                sSegmentRid;
    stlUInt16             sPageCountInExt;
    stlInt64              sBytes;

    stlMemcpy( &sSegmentAttr, aLogBody, STL_SIZEOF(smlSegmentAttr) );
    
    STL_TRY( smsGetSegmentHandle( aTargetRid,
                                  &sSegmentHandle,
                                  aEnv )
             == STL_SUCCESS);
    
    sCache = (smsCommonCache*)sSegmentHandle;
    sSegmentRid = smsGetSegRid( sSegmentHandle );
    sPageCountInExt = smfGetPageCountInExt( smsGetTbsId(sSegmentHandle) );
    
    STL_TRY( smpAcquire( aMiniTrans,
                         aTargetRid.mTbsId,
                         aTargetRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

    sSegmentHeader = (smsSegHdr*)smpGetBody( SMP_FRAME(&sPageHandle) );

    /**
     * applys segment attribute to segment header
     */

    STL_DASSERT( sSegmentAttr.mValidFlags == SML_SEGMENT_DEFAULT_ATTR );

    sBytes = sSegmentAttr.mInitial;
    sSegmentHeader->mInitialExtents = STL_ALIGN( STL_ALIGN( sBytes,
                                                            SMP_PAGE_SIZE ) / SMP_PAGE_SIZE,
                                                 sPageCountInExt ) / sPageCountInExt;

    sBytes = sSegmentAttr.mNext;
    sSegmentHeader->mNextExtents = STL_ALIGN( STL_ALIGN( sBytes,
                                                         SMP_PAGE_SIZE ) / SMP_PAGE_SIZE,
                                              sPageCountInExt ) / sPageCountInExt;
    
    sBytes = sSegmentAttr.mMinSize;
    sSegmentHeader->mMinExtents = STL_ALIGN( STL_ALIGN( sBytes,
                                                        SMP_PAGE_SIZE ) / SMP_PAGE_SIZE,
                                             sPageCountInExt ) / sPageCountInExt;
    
    sBytes = sSegmentAttr.mMaxSize;
    sSegmentHeader->mMaxExtents = STL_ALIGN( STL_ALIGN( sBytes,
                                                        SMP_PAGE_SIZE ) / SMP_PAGE_SIZE,
                                             sPageCountInExt ) / sPageCountInExt;
    
    /**
     * applys segment attribute to segment cache
     */

    sCache->mInitialExtents = sSegmentHeader->mInitialExtents;
    sCache->mNextExtents = sSegmentHeader->mNextExtents;
    sCache->mMinExtents = sSegmentHeader->mMinExtents;
    sCache->mMaxExtents = sSegmentHeader->mMaxExtents;
    
    /**
     * logging redo log
     */
    
    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_SEGMENT,
                           SMR_LOG_INIT_SEGMENT_HDR,
                           SMR_REDO_TARGET_PAGE,
                           (stlChar*)sSegmentHeader,
                           STL_SIZEOF(smsSegHdr),
                           sSegmentRid.mTbsId,
                           sSegmentRid.mPageId,
                           (stlChar*)sSegmentHeader - (stlChar*)SMP_FRAME(&sPageHandle),
                           SMXM_LOGGING_REDO,
                           aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsUndoSetUnusableSegment( smxmTrans * aMiniTrans,
                                     smlRid      aTargetRid,
                                     void      * aLogBody,
                                     stlInt16    aLogSize,
                                     smlEnv    * aEnv )
{
    smpHandle        sPageHandle;
    stlInt16         sOffset;
    stlBool          sData[2];
    void           * sSegmentHandle;
    smsSegHdr      * sSegmentHeader;
    
    STL_TRY( smsGetSegmentHandle( aTargetRid,
                                  &sSegmentHandle,
                                  aEnv )
             == STL_SUCCESS );
    
    STL_TRY( smpAcquire( aMiniTrans,
                         aTargetRid.mTbsId,
                         aTargetRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

    sSegmentHeader = (smsSegHdr*)smpGetBody(SMP_FRAME(&sPageHandle));

    sOffset = ( (stlChar*)&sSegmentHeader->mIsUsable -
                (stlChar*)SMP_FRAME(&sPageHandle) );

    sData[0] = STL_FALSE;
    sData[1] = STL_TRUE;

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_RECOVERY,
                           SMR_LOG_BYTES,
                           SMR_REDO_TARGET_PAGE,
                           (void*)sData,
                           STL_SIZEOF( stlBool ) * 2,
                           aTargetRid.mTbsId,
                           aTargetRid.mPageId,
                           sOffset,
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    sSegmentHeader->mIsUsable = STL_TRUE;
    
    /*
     * 2. Update Segment Cache
     */
    
    ((smsCommonCache*)sSegmentHandle)->mIsUsable = STL_TRUE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsUndoSetUsableSegment( smxmTrans * aMiniTrans,
                                   smlRid      aTargetRid,
                                   void      * aLogBody,
                                   stlInt16    aLogSize,
                                   smlEnv    * aEnv )
{
    smpHandle        sPageHandle;
    stlInt16         sOffset;
    stlBool          sData[2];
    void           * sSegmentHandle;
    smsSegHdr      * sSegmentHeader;
    
    STL_TRY( smsGetSegmentHandle( aTargetRid,
                                  &sSegmentHandle,
                                  aEnv )
             == STL_SUCCESS );

    STL_TRY( smpAcquire( aMiniTrans,
                         aTargetRid.mTbsId,
                         aTargetRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

    sSegmentHeader = (smsSegHdr*)smpGetBody(SMP_FRAME(&sPageHandle));

    sOffset = ( (stlChar*)&sSegmentHeader->mIsUsable -
                (stlChar*)SMP_FRAME(&sPageHandle) );

    sData[0] = STL_TRUE;
    sData[1] = STL_FALSE;

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_RECOVERY,
                           SMR_LOG_BYTES,
                           SMR_REDO_TARGET_PAGE,
                           (void*)sData,
                           STL_SIZEOF( stlBool ) * 2,
                           aTargetRid.mTbsId,
                           aTargetRid.mPageId,
                           sOffset,
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );
    
    sSegmentHeader->mIsUsable = STL_FALSE;

    /**
     * 할당되었던 extent 삭제
     */
    
    STL_TRY( smsReset( smxmGetTransId( aMiniTrans ),
                       sSegmentHandle,
                       STL_TRUE, /* aFreePages */
                       aEnv )
             == STL_SUCCESS );
    
    /*
     * 2. Update Segment Cache
     */
    
    ((smsCommonCache*)sSegmentHandle)->mIsUsable = STL_FALSE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsUndoResetPendingSegment( smxmTrans * aMiniTrans,
                                      smlRid      aTargetRid,
                                      void      * aLogBody,
                                      stlInt16    aLogSize,
                                      smlEnv    * aEnv )
{
    smpHandle        sPageHandle;
    /* stlInt16         sOffset; */
    smlPid           sData[2];
    void           * sSegmentHandle;
    smsmpSegHdr    * sSegmentHeader;
    smsmpCache     * sCache;

    STL_DASSERT( aLogSize == (STL_SIZEOF(smlPid) * 2) );
    
    stlMemcpy( (stlChar*)sData, aLogBody, aLogSize );
    
    STL_TRY( smsGetSegmentHandle( aTargetRid,
                                  &sSegmentHandle,
                                  aEnv )
             == STL_SUCCESS );

    sCache = (smsmpCache*)sSegmentHandle;
    
    STL_TRY( smpAcquire( aMiniTrans,
                         aTargetRid.mTbsId,
                         aTargetRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

    sSegmentHeader = (smsmpSegHdr*)smpGetBody(SMP_FRAME(&sPageHandle));

    /* sOffset = ( (stlChar*)&sSegmentHeader->mCurHdrPid - */
    /*             (stlChar*)SMP_FRAME(&sPageHandle) ); */

    sSegmentHeader->mCurHdrPid = sData[0];
    sSegmentHeader->mCurDataPid = sData[1];

    sCache->mCurHdrPid = sData[0];
    sCache->mCurDataPid = sData[1];
    
    sData[0] = sCache->mCurHdrPid;
    sData[1] = sCache->mCurHdrPid;
    
    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_RECOVERY,
                           SMR_LOG_BYTES,
                           SMR_REDO_TARGET_PAGE,
                           (void*)sData,
                           STL_SIZEOF( smlPid ) * 2,
                           aTargetRid.mTbsId,
                           aTargetRid.mPageId,
                           ((void*)&(sSegmentHeader->mCurHdrPid)) - SMP_FRAME(&sPageHandle),
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );
        
    sData[0] = sCache->mCurDataPid;
    sData[1] = sCache->mCurDataPid;
    
    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_RECOVERY,
                           SMR_LOG_BYTES,
                           SMR_REDO_TARGET_PAGE,
                           (void*)sData,
                           STL_SIZEOF( smlPid ) * 2,
                           aTargetRid.mTbsId,
                           aTargetRid.mPageId,
                           ((void*)&(sSegmentHeader->mCurDataPid)) - SMP_FRAME(&sPageHandle),
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
