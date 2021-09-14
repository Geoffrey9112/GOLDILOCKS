/*******************************************************************************
 * smdmpfMtxUndo.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smdmpfMtxUndo.c 13496 2014-08-29 05:38:43Z leekmo $
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
#include <smxm.h>
#include <smdmpfDef.h>
#include <smdmpfRowDef.h>
#include <smdmpf.h>
#include <smdDef.h>

/**
 * @file smdmpfMtxUndo.c
 * @brief Storage Manager Layer Table Mini-Transaction Undo Internal Routines
 */

/**
 * @addtogroup smdmpfMtxUndo
 * @{
 */

smdTableMtxUndoModule gSmdmpfTableMtxUndoModule =
{
    smdmpfMtxUndoMemoryHeapInsert
};

stlStatus smdmpfMtxUndoMemoryHeapInsert( smlRid    * aDataRid,
                                         void      * aData,
                                         stlUInt16   aDataSize,
                                         smpHandle * aPageHandle,
                                         smlEnv    * aEnv )
{
    stlChar       * sRowSlot;

    sRowSlot = SMDMPF_GET_NTH_ROW_HDR( SMP_FRAME(aPageHandle), aDataRid->mOffset );
    SMDMPF_SET_DELETED(sRowSlot);

    SMP_DECREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) );

    return STL_SUCCESS;
}

/** @} */
