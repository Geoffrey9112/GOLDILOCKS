/*******************************************************************************
 * smdmphMtxUndo.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smdmphMtxUndo.c 13496 2014-08-29 05:38:43Z leekmo $
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
#include <smdmphDef.h>
#include <smdmphRowDef.h>
#include <smdmph.h>
#include <smdDef.h>

/**
 * @file smdmphMtxUndo.c
 * @brief Storage Manager Layer Table Mini-Transaction Undo Internal Routines
 */

/**
 * @addtogroup smdmphMtxUndo
 * @{
 */

smdTableMtxUndoModule gSmdmphTableMtxUndoModule =
{
    smdmphMtxUndoMemoryHeapInsert
};


stlStatus smdmphMtxUndoMemoryHeapInsert( smlRid    * aDataRid,
                                         void      * aData,
                                         stlUInt16   aDataSize,
                                         smpHandle * aPageHandle,
                                         smlEnv    * aEnv )
{
    stlInt16        sRowHeaderSize;
    stlInt16        sTotalRowSize;
    stlChar       * sRowSlot;
    stlInt16        sPadSpace;

    sRowSlot = smpGetNthRowItem( aPageHandle, aDataRid->mOffset );
    SMDMPH_GET_ROW_HDR_SIZE( sRowSlot, &sRowHeaderSize );
    sTotalRowSize =  smdmphGetRowSize(sRowSlot);
    SMDMPH_SET_DELETED(sRowSlot);

    SMDMPH_GET_PAD_SPACE( sRowSlot, &sPadSpace );

    /* delete 되었으니 reclaimed space에 일단 추가한다 */
    SMP_ADD_RECLAIMED_SPACE( SMP_FRAME(aPageHandle), sTotalRowSize + sPadSpace );
    SMP_DECREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) );

    smdmphValidatePadSpace( aPageHandle );

    return STL_SUCCESS;
}

/** @} */
