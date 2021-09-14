/*******************************************************************************
 * smsmcLog.c
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
#include <smsDef.h>
#include <smsmcDef.h>

/**
 * @file smsmcLog.c
 * @brief Storage Manager Layer Circular Segment Logging Internal Routines
 */

/**
 * @addtogroup smsmcLog
 * @{
 */

stlStatus smsmcWriteLogUpdateSegHdr( smxmTrans  * aMiniTrans,
                                     smlRid       aSegRid,
                                     smsmcCache * aCache,
                                     smlEnv     * aEnv )
{
    smpHandle     sPageHandle;
    smsmcSegHdr * sSegHdr;
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

    sSegHdr = (smsmcSegHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );

    sSegHdr->mCommonSegHdr.mSegType = aCache->mCommonCache.mSegType;
    sSegHdr->mCommonSegHdr.mTbsId = aCache->mCommonCache.mTbsId;
    sSegHdr->mCommonSegHdr.mHasRelHdr = aCache->mCommonCache.mHasRelHdr;
    sSegHdr->mCommonSegHdr.mState = SMS_STATE_ALLOC_BODY;
    sSegHdr->mCurDataPid = aCache->mCurDataPid;
    sSegHdr->mCurExtId = aCache->mCurExtId;
    sSegHdr->mHdrPid = aCache->mHdrPid;
    sSegHdr->mCommonSegHdr.mAllocExtCount = aCache->mCommonCache.mAllocExtCount;
    sSegHdr->mCommonSegHdr.mIsUsable = aCache->mCommonCache.mIsUsable;

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_SEGMENT,
                           SMR_LOG_MEMORY_CIRCULAR_UPDATE_SEGMENT_HDR,
                           SMR_REDO_TARGET_PAGE,
                           (void*)sSegHdr,
                           STL_SIZEOF( smsmcSegHdr ),
                           aSegRid.mTbsId,
                           aSegRid.mPageId,
                           aSegRid.mOffset,
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmcWriteLogUpdateExtHdr( smxmTrans   * aMiniTrans,
                                     smlTbsId      aTbsId,
                                     smlPid        aPageId,
                                     smsmcExtHdr * aExtHdr,
                                     smlEnv      * aEnv )
{
    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_SEGMENT,
                           SMR_LOG_MEMORY_CIRCULAR_UPDATE_EXT_HDR,
                           SMR_REDO_TARGET_PAGE,
                           (void*)aExtHdr,
                           STL_SIZEOF( smsmcExtHdr ),
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
