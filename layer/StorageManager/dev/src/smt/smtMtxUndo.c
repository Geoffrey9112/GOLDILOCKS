/*******************************************************************************
 * smtRedo.c
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
#include <smf.h>
#include <smp.h>
#include <smtDef.h>
#include <smt.h>
#include <smg.h>
#include <smxm.h>
#include <smtTbs.h>
#include <smtExtMgr.h>
#include <smtMtxUndo.h>

/**
 * @file smtRedo.c
 * @brief Storage Manager Layer Datafile Mini-Transaction Undo Internal Routines
 */

/**
 * @addtogroup smtMtxUndo
 * @{
 */

stlStatus smtMtxUndoAllocExtent( smlRid    * aDataRid,
                                 void      * aData,
                                 stlUInt16   aDataSize,
                                 smpHandle * aPageHandle,
                                 smlEnv    * aEnv )
{
    stlInt16       sOffset;
    stlChar      * sExtBitArray;
    smtExtMapHdr * sExtMapHdr;
    
    sOffset = aDataRid->mOffset;
    
    sExtMapHdr = (smtExtMapHdr*)smpGetBody( SMP_FRAME( aPageHandle ) );
    sExtBitArray = SMT_EXT_BIT_ARRAY( sExtMapHdr );
    smtOffExtBit( sExtBitArray, sOffset );
    
    sExtMapHdr->mOnExtCount -= 1;
    sExtMapHdr->mOffExtCount += 1;
    
    return STL_SUCCESS;
}

stlStatus smtMtxUndoAddExtentBlock( smlRid    * aDataRid,
                                    void      * aData,
                                    stlUInt16   aDataSize,
                                    smpHandle * aPageHandle,
                                    smlEnv    * aEnv )
{
    stlInt16            sOffset;
    smtExtBlockMapHdr * sExtBlockMapHdr;
    smtExtBlockMap    * sExtBlockMap;
    smlPid              sExtBlockPid;
    smtExtBlockDesc   * sExtBlockDesc;

    sOffset = aDataRid->mOffset;
    STL_WRITE_INT32( &sExtBlockPid, aData );
    
    sExtBlockMapHdr = (smtExtBlockMapHdr*)smpGetBody( SMP_FRAME( aPageHandle ) );
    sExtBlockMap = (smtExtBlockMap*)((stlChar*)sExtBlockMapHdr +
                                     STL_SIZEOF( smtExtBlockMapHdr ));

    sExtBlockDesc = &sExtBlockMap->mExtBlockDesc[sOffset];

    sExtBlockDesc->mExtBlock = SMP_NULL_PID;
    sExtBlockDesc->mState = SMT_EBS_UNALLOCATED;
        
    return STL_SUCCESS;
}

stlStatus smtMtxUndoUpdateExtBlockDescState( smlRid    * aDataRid,
                                             void      * aData,
                                             stlUInt16   aDataSize,
                                             smpHandle * aPageHandle,
                                             smlEnv    * aEnv )
{
    smtExtBlockMapHdr * sExtBlockMapHdr;
    smtExtBlockMap    * sExtBlockMap;
    stlUInt32           sBeforeState;
    stlUInt32           sAfterState;
    stlInt16            sOffset = 0;
    
    STL_READ_MOVE_INT32( &sBeforeState, aData, sOffset );
    STL_READ_MOVE_INT32( &sAfterState, aData, sOffset );
    
    sExtBlockMapHdr = (smtExtBlockMapHdr*)smpGetBody( SMP_FRAME( aPageHandle ) );
    sExtBlockMap = (smtExtBlockMap*)((stlChar*)sExtBlockMapHdr +
                                     STL_SIZEOF( smtExtBlockMapHdr ));

    sExtBlockMap->mExtBlockDesc[aDataRid->mOffset].mState = sBeforeState;

    return STL_SUCCESS;
}

/** @} */
