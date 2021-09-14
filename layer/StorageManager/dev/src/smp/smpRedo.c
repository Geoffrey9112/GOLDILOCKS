/*******************************************************************************
 * smpRedo.c
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
#include <smfDef.h>
#include <smf.h>
#include <smpDef.h>
#include <smp.h>
#include <smg.h>
#include <smrDef.h>
#include <smr.h>
#include <smxl.h>
#include <smxm.h>
#include <smpRedo.h>

/**
 * @file smpRedo.c
 * @brief Storage Manager Layer Datafile Redo Internal Routines
 */

/**
 * @addtogroup smpRedo
 * @{
 */

smrRedoVector gSmpRedoVectors[SMR_PAGE_ACCESS_LOG_MAX_COUNT] =
{
    { /* SMR_LOG_INIT_PAGE */
        smpRedoInitPage,
        NULL,
        "INIT_PAGE"
    },
    { /* SMR_LOG_INIT_PAGE_BODY */
        smpRedoInitPageBody,
        NULL,
        "INIT_PAGE_BODY"
    },
    { /* SMR_LOG_INIT_FIXED_PAGE_BODY */
        smpRedoInitFixedPageBody,
        NULL,
        "INIT_FIXED_PAGE_BODY"
    }
};

stlStatus smpRedoInitPage( smlRid    * aDataRid,
                           void      * aData,
                           stlUInt16   aDataSize,
                           smpHandle * aPageHandle,
                           smlEnv    * aEnv )
{
    smpPhyHdr sPhyHdr;

    STL_WRITE_BYTES( &sPhyHdr, aData, STL_SIZEOF( smpPhyHdr ) );

    SMP_INIT_PHYSICAL_HDR( SMP_GET_PHYSICAL_HDR( SMP_FRAME( aPageHandle ) ),
                           aPageHandle->mPch,
                           sPhyHdr.mPageType,
                           sPhyHdr.mAgableScn,
                           sPhyHdr.mTimestamp,
                           sPhyHdr.mSegmentId,
                           sPhyHdr.mParentRid );

    aPageHandle->mPch->mPageType = sPhyHdr.mPageType;
    
    return STL_SUCCESS;
}

stlStatus smpRedoInitPageBody( smlRid    * aDataRid,
                               void      * aData,
                               stlUInt16   aDataSize,
                               smpHandle * aPageHandle,
                               smlEnv    * aEnv )
{
    smpSignpost            * sSignpost = (smpSignpost*)SMP_GET_SIGNPOST( SMP_FRAME(aPageHandle) );
    smpOrderedSlotSignpost * sOSSignpost;
    smpRts                 * sRts;
    stlInt16                 sRtsCount;
    stlInt16                 i;
    smpOffsetTableHeader   * sOffsetTable;
    stlInt16                 sUseFreeList;
    stlChar                * sPage = (stlChar*)SMP_FRAME(aPageHandle);

    if( aDataSize == STL_SIZEOF(smpSignpost) )
    {
        STL_WRITE_BYTES( (stlChar*)sSignpost, aData, aDataSize );
    }
    else
    {
        STL_WRITE_BYTES( (stlChar*)sSignpost, aData, aDataSize - STL_SIZEOF(stlInt16) );
        STL_WRITE_INT16( &sUseFreeList, (stlChar*)aData + (aDataSize - STL_SIZEOF(stlInt16)) );

        STL_ASSERT( sSignpost->mPageMgmtType == SMP_ORDERED_SLOT_TYPE );

        sOSSignpost = (smpOrderedSlotSignpost*)sSignpost;
        sRtsCount = sOSSignpost->mCurRtsCount;
        sRts = (smpRts*)(sPage + sOSSignpost->mCommon.mHighWaterMark);
        for (i = 0; i <sRtsCount; i++)
        {
            SMP_INIT_RTS(sRts);
            sRts++;
        }
        sOSSignpost->mCommon.mHighWaterMark += STL_SIZEOF(smpRts) * sRtsCount;

        sOffsetTable = (smpOffsetTableHeader*)(sPage + sOSSignpost->mCommon.mHighWaterMark);
        sOffsetTable->mCurrentSlotCount = 0;
        if( (stlBool)sUseFreeList == STL_TRUE )
        {
            sOffsetTable->mFirstFreeOffsetSlot = SMP_USE_FREE_SLOT_LIST_YES | SMP_OFFSET_SLOT_NULL;
        }
        else
        {
            sOffsetTable->mFirstFreeOffsetSlot = SMP_USE_FREE_SLOT_LIST_NO | SMP_OFFSET_SLOT_NULL;
        }
        sOSSignpost->mCommon.mHighWaterMark += STL_SIZEOF(smpOffsetTableHeader);
    }

    return STL_SUCCESS;
}

stlStatus smpRedoInitFixedPageBody( smlRid    * aDataRid,
                                    void      * aData,
                                    stlUInt16   aDataSize,
                                    smpHandle * aPageHandle,
                                    smlEnv    * aEnv )
{
    stlChar * sSignpost = SMP_GET_SIGNPOST(aPageHandle->mFrame);

    STL_WRITE_BYTES( sSignpost, aData, aDataSize );

    return STL_SUCCESS;
}

/** @} */
