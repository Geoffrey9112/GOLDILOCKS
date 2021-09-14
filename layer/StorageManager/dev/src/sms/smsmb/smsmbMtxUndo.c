/*******************************************************************************
 * smsmbRedo.c
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
#include <smsmbDef.h>
#include <smsmbSegment.h>

/**
 * @file smsmbRedo.c
 * @brief Storage Manager Layer Bitmap Segment Redo Internal Routines
 */

/**
 * @addtogroup smsmbRedo
 * @{
 */

stlStatus smsmbMtxUndoUpdateLeafStatus( smlRid    * aDataRid,
                                        void      * aData,
                                        stlUInt16   aDataSize,
                                        smpHandle * aPageHandle,
                                        smlEnv    * aEnv )
{
    stlInt16         sOffset;
    smsmbLeafHdr   * sLeafHdr;
    smsmbLeaf      * sLeaf;
    smpFreeness      sBeforeFreeness;
    smpFreeness      sAfterFreeness;
    smlScn           sBeforeScn;
    smlScn           sAfterScn;

    sOffset = 0;
    
    sLeafHdr = (smsmbLeafHdr*)smpGetBody( SMP_FRAME( aPageHandle ) );
    sLeaf = (smsmbLeaf*)((stlChar*)sLeafHdr + STL_SIZEOF(smsmbLeafHdr));

    STL_READ_MOVE_INT32( &sBeforeFreeness, aData, sOffset );
    STL_READ_MOVE_INT32( &sAfterFreeness, aData, sOffset );
    STL_READ_MOVE_INT64( &sBeforeScn, aData, sOffset );
    STL_READ_MOVE_INT64( &sAfterScn, aData, sOffset );

    sLeafHdr->mFreenessCount[sBeforeFreeness] += 1;
    sLeafHdr->mFreenessCount[sAfterFreeness] -= 1;
    
    smsmbSetFreeness( sLeaf->mFreeness,
                      aDataRid->mOffset,
                      sBeforeFreeness );

    sLeaf->mScn[aDataRid->mOffset] = sBeforeScn;

    return STL_SUCCESS;
}

stlStatus smsmbMtxUndoUpdateInternalStatus( smlRid    * aDataRid,
                                            void      * aData,
                                            stlUInt16   aDataSize,
                                            smpHandle * aPageHandle,
                                            smlEnv    * aEnv )
{
    stlInt16           sOffset;
    smsmbInternalHdr * sInternalHdr;
    smsmbInternal    * sInternal;
    smpFreeness        sBeforeFreeness;
    smpFreeness        sAfterFreeness;
    
    sOffset = 0;
    
    sInternalHdr = (smsmbInternalHdr*)smpGetBody( SMP_FRAME( aPageHandle ) );
    sInternal = (smsmbInternal*)((stlChar*)sInternalHdr +
                                 STL_SIZEOF(smsmbInternalHdr));
    
    STL_READ_MOVE_INT32( &sBeforeFreeness, aData, sOffset );
    STL_READ_MOVE_INT32( &sAfterFreeness, aData, sOffset );

    sInternalHdr->mFreenessCount[sBeforeFreeness] += 1;
    sInternalHdr->mFreenessCount[sAfterFreeness] -= 1;
    
    smsmbSetFreeness( sInternal->mFreeness,
                      aDataRid->mOffset,
                      sBeforeFreeness );
    
    return STL_SUCCESS;
}

stlStatus smsmbMtxUndoAddExt( smlRid    * aDataRid,
                              void      * aData,
                              stlUInt16   aDataSize,
                              smpHandle * aPageHandle,
                              smlEnv    * aEnv )
{
    smsmbExtMapHdr * sExtMapHdr;
    smsmbExtMap    * sExtMap;
    smlExtId         sExtId;
    stlInt32         sLogOffset = 0;
    
    STL_READ_MOVE_INT64( &sExtId, aData, sLogOffset );
    
    sExtMapHdr = (smsmbExtMapHdr*)smpGetBody( SMP_FRAME( aPageHandle ) );
    sExtMap = (smsmbExtMap*)((stlChar*)sExtMapHdr + STL_SIZEOF(smsmbExtMapHdr));

    sExtMap->mExtId[sExtMapHdr->mExtDescCount] = SML_INVALID_EXTID;
    sExtMapHdr->mExtDescCount--;

    return STL_SUCCESS;
}


/** @} */
