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
#include <smsmbRedo.h>
#include <smsmbSegment.h>

/**
 * @file smsmbRedo.c
 * @brief Storage Manager Layer Bitmap Segment Redo Internal Routines
 */

/**
 * @addtogroup smsmbRedo
 * @{
 */

stlStatus smsmbRedoUpdateLeafStatus( smlRid    * aDataRid,
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

    sLeafHdr->mFreenessCount[sBeforeFreeness] -= 1;
    sLeafHdr->mFreenessCount[sAfterFreeness] += 1;
    
    smsmbSetFreeness( sLeaf->mFreeness,
                      aDataRid->mOffset,
                      sAfterFreeness );

    sLeaf->mScn[aDataRid->mOffset] = sAfterScn;
    
    return STL_SUCCESS;
}

stlStatus smsmbRedoUpdateInternalStatus( smlRid    * aDataRid,
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

    sInternalHdr->mFreenessCount[sBeforeFreeness] -= 1;
    sInternalHdr->mFreenessCount[sAfterFreeness] += 1;
    
    smsmbSetFreeness( sInternal->mFreeness,
                      aDataRid->mOffset,
                      sAfterFreeness );
    
    return STL_SUCCESS;
}

stlStatus smsmbRedoFreeExtMap( smlRid    * aDataRid,
                               void      * aData,
                               stlUInt16   aDataSize,
                               smpHandle * aPageHandle,
                               smlEnv    * aEnv )
{
    stlInt16         sOffset;
    smsmbExtMapHdr * sExtMapHdr;
    smsmbExtMap    * sExtMap;
    stlInt32         sStartExtDescIdx;
    stlInt32         sEndExtDescIdx;
    stlInt32         i;
    
    sOffset = 0;
    
    sExtMapHdr = (smsmbExtMapHdr*)smpGetBody( SMP_FRAME( aPageHandle ) );
    sExtMap = (smsmbExtMap*)((stlChar*)sExtMapHdr + STL_SIZEOF(smsmbExtMapHdr));

    STL_READ_MOVE_INT32( &sStartExtDescIdx, aData, sOffset );
    STL_READ_MOVE_INT32( &sEndExtDescIdx, aData, sOffset );

    for( i = sStartExtDescIdx; i <= sEndExtDescIdx; i++ )
    {
        sExtMap->mExtId[i] = SML_INVALID_EXTID;
    }
    
    return STL_SUCCESS;
}

stlStatus smsmbRedoUpdateSegHdr( smlRid    * aDataRid,
                                 void      * aData,
                                 stlUInt16   aDataSize,
                                 smpHandle * aPageHandle,
                                 smlEnv    * aEnv )
{
    smsmbSegHdr * sDestSegHdr;
    
    sDestSegHdr = (smsmbSegHdr*)smpGetBody( SMP_FRAME( aPageHandle ) );
    STL_WRITE_BYTES( sDestSegHdr, aData, STL_SIZEOF(smsmbSegHdr) );
    
    return STL_SUCCESS;
}

stlStatus smsmbRedoExpandLeaf( smlRid    * aDataRid,
                               void      * aData,
                               stlUInt16   aDataSize,
                               smpHandle * aPageHandle,
                               smlEnv    * aEnv )
{
    smlPid         sStartPid;
    stlInt32       sPageCount;
    stlInt32       sLogOffset = 0;
    smsmbLeafHdr * sLeafHdr;
    smsmbLeaf    * sLeaf;
    stlInt32       i;
    stlInt32       j;

    STL_READ_MOVE_INT32( &sStartPid, aData, sLogOffset );
    STL_READ_MOVE_INT32( &sPageCount, aData, sLogOffset );

    sLeafHdr = (smsmbLeafHdr*)smpGetBody( SMP_FRAME( aPageHandle ) );
    sLeaf = (smsmbLeaf*)((stlChar*)sLeafHdr + STL_SIZEOF(smsmbLeafHdr));
    
    sLeafHdr->mChildCount += sPageCount;

    for( i = aDataRid->mOffset, j = 0; j < sPageCount; i++, j++ )
    {
        sLeaf->mDataPid[i] = sStartPid + j;
        sLeaf->mScn[i] = 0;
        sLeafHdr->mFreenessCount[SMP_FREENESS_FREE] += 1;
        smsmbSetFreeness( sLeaf->mFreeness,
                          i,
                          SMP_FREENESS_FREE );
    }
    
    return STL_SUCCESS;
}

stlStatus smsmbRedoInsertInternalBit( smlRid    * aDataRid,
                                      void      * aData,
                                      stlUInt16   aDataSize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv )
{
    smlPid             sNewPageId;
    stlInt32           sFreeness;
    stlInt32           sLogOffset = 0;
    smsmbInternalHdr * sInternalHdr;
    smsmbInternal    * sInternal;

    STL_READ_MOVE_INT32( &sNewPageId, aData, sLogOffset );
    STL_READ_MOVE_INT32( &sFreeness, aData, sLogOffset );

    sInternalHdr = (smsmbInternalHdr*)smpGetBody( SMP_FRAME( aPageHandle ) );
    sInternal = (smsmbInternal*)((stlChar*)sInternalHdr + STL_SIZEOF(smsmbInternalHdr));
    
    sInternalHdr->mChildCount += 1;

    sInternal->mChildPid[aDataRid->mOffset] = sNewPageId;
    sInternalHdr->mFreenessCount[sFreeness] += 1;
    smsmbSetFreeness( sInternal->mFreeness,
                      aDataRid->mOffset,
                      sFreeness );

    return STL_SUCCESS;
}

stlStatus smsmbRedoInitLeaf( smlRid    * aDataRid,
                             void      * aData,
                             stlUInt16   aDataSize,
                             smpHandle * aPageHandle,
                             smlEnv    * aEnv )
{
    smsmbLeafHdr * sLeafHdr;
    smsmbLeaf    * sLeaf;
    stlInt32       i;

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
    
    return STL_SUCCESS;
}

stlStatus smsmbRedoInitInternal( smlRid    * aDataRid,
                                 void      * aData,
                                 stlUInt16   aDataSize,
                                 smpHandle * aPageHandle,
                                 smlEnv    * aEnv )
{
    smsmbInternalHdr * sInternalHdr;
    smsmbInternal    * sInternal;
    stlInt32           i;

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

    return STL_SUCCESS;
}

stlStatus smsmbRedoInitExtMap( smlRid    * aDataRid,
                               void      * aData,
                               stlUInt16   aDataSize,
                               smpHandle * aPageHandle,
                               smlEnv    * aEnv )
{
    smsmbExtMapHdr * sExtMapHdr;
    smsmbExtMap    * sExtMap;
    stlInt32         i;
    smlPid           sPrvMapPid;
    stlInt32         sLogOffset = 0;
    
    STL_READ_MOVE_INT32( &sPrvMapPid, aData, sLogOffset );
    
    sExtMapHdr = (smsmbExtMapHdr*)smpGetBody( SMP_FRAME( aPageHandle ) );
    sExtMap = (smsmbExtMap*)((stlChar*)sExtMapHdr + STL_SIZEOF(smsmbExtMapHdr));

    sExtMapHdr->mPrvExtMap = sPrvMapPid;
    sExtMapHdr->mExtDescCount = 0;
        
    for( i = 0; i < SMSMB_EXTENT_DESC_COUNT; i++ )
    {
        sExtMap->mExtId[i] = SML_INVALID_EXTID;
    }

    return STL_SUCCESS;
}

stlStatus smsmbRedoAddExt( smlRid    * aDataRid,
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

    sExtMap->mExtId[sExtMapHdr->mExtDescCount] = sExtId;
    sExtMapHdr->mExtDescCount++;

    return STL_SUCCESS;
}


/** @} */
