/*******************************************************************************
 * smsmpLog.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smsmpLog.c 5376 2012-08-22 03:20:49Z mkkim $
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
#include <smsDef.h>
#include <smsmpDef.h>

/**
 * @file smsmpLog.c
 * @brief Storage Manager Layer Pending Segment Logging Internal Routines
 */

/**
 * @addtogroup smsmpLog
 * @{
 */

stlStatus smsmpWriteLogUpdateSegHdr( smxmTrans  * aMiniTrans,
                                     smlRid       aSegRid,
                                     smsmpCache * aCache,
                                     smlEnv     * aEnv )
{
    smpHandle     sPageHandle;
    smsmpSegHdr * sSegHdr;
    smpHandle   * sCachedPageHandle;

    sCachedPageHandle = smxmGetPageHandle( aMiniTrans,
                                           aSegRid.mTbsId,
                                           aSegRid.mPageId,
                                           KNL_LATCH_MODE_EXCLUSIVE );

    if( sCachedPageHandle == NULL )
    {
        STL_TRY( smpAcquire( aMiniTrans,
                             aSegRid.mTbsId,
                             aSegRid.mPageId,
                             KNL_LATCH_MODE_EXCLUSIVE,
                             &sPageHandle,
                             aEnv ) == STL_SUCCESS );
    }
    else
    {
        sPageHandle = *sCachedPageHandle;
    }

    sSegHdr = (smsmpSegHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );

    sSegHdr->mCommonSegHdr.mSegType = aCache->mCommonCache.mSegType;
    sSegHdr->mCommonSegHdr.mTbsId = aCache->mCommonCache.mTbsId;
    sSegHdr->mCommonSegHdr.mHasRelHdr = aCache->mCommonCache.mHasRelHdr;
    sSegHdr->mCommonSegHdr.mState = SMS_STATE_ALLOC_BODY;
    sSegHdr->mFirstHdrPid = aCache->mFirstHdrPid;
    sSegHdr->mCurDataPid = aCache->mCurDataPid;
    sSegHdr->mCurHdrPid = aCache->mCurHdrPid;
    sSegHdr->mCommonSegHdr.mAllocExtCount = aCache->mCommonCache.mAllocExtCount;
    sSegHdr->mCommonSegHdr.mIsUsable = aCache->mCommonCache.mIsUsable;

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_SEGMENT,
                           SMR_LOG_MEMORY_PENDING_UPDATE_SEGMENT_HDR,
                           SMR_REDO_TARGET_PAGE,
                           (void*)sSegHdr,
                           STL_SIZEOF( smsmpSegHdr ),
                           aSegRid.mTbsId,
                           aSegRid.mPageId,
                           aSegRid.mOffset,
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmpWriteLogUpdateExtHdr( smxmTrans   * aMiniTrans,
                                     smlTbsId      aTbsId,
                                     smlPid        aPageId,
                                     smsmpExtHdr * aExtHdr,
                                     smlEnv      * aEnv )
{
    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_SEGMENT,
                           SMR_LOG_MEMORY_PENDING_UPDATE_EXT_HDR,
                           SMR_REDO_TARGET_PAGE,
                           (void*)aExtHdr,
                           STL_SIZEOF( smsmpExtHdr ),
                           aTbsId,
                           aPageId,
                           0,
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
