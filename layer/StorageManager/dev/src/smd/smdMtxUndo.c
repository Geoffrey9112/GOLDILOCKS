/*******************************************************************************
 * smdMtxUndo.c
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
#include <smdDef.h>
#include <smdmphRowDef.h>
#include <smdmphDef.h>
#include <smdmpfRowDef.h>
#include <smdmpfDef.h>
#include <smdmph.h>
#include <smdmpf.h>

/**
 * @file smdMtxUndo.c
 * @brief Storage Manager Layer Table Mini-Transaction Undo Internal Routines
 */

/**
 * @addtogroup smdMtxUndo
 * @{
 */

extern smdTableMtxUndoModule gSmdmphTableMtxUndoModule;
extern smdTableMtxUndoModule gSmdmpfTableMtxUndoModule;

smdTableMtxUndoModule * gSmdTableMtxUndoModule[SMP_PAGE_TYPE_MAX] =
{
    NULL,        /* INIT */
    NULL,        /* UNFORMAT */
    NULL,        /* EXT_BLOCK_MAP */
    NULL,        /* EXT_MAP */
    NULL,        /* BITMAP_HEADER */
    NULL,        /* CIRCULAR_HEADER */
    NULL,        /* FLAT_HEADER */
    NULL,        /* PENDING_HEADER */
    NULL,        /* UNDO */
    NULL,        /* TRANS */
    NULL,        /* BITMAP_INTERNAL */
    NULL,        /* BITMAP_LEAF */
    NULL,        /* BITMAP_EXTENT_MAP */
    &gSmdmphTableMtxUndoModule,        /* TABLE_DATA */
    &gSmdmpfTableMtxUndoModule,        /* COLUMNAR_TABLE_DATA */
    NULL         /* INDEX_DATA*/
};

stlStatus smdMtxUndoMemoryHeapInsert( smlRid    * aDataRid,
                                      void      * aData,
                                      stlUInt16   aDataSize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv )
{
    smpPageType     sPageType;

    sPageType = SMP_GET_PAGE_TYPE( SMP_FRAME(aPageHandle) );

    STL_TRY( gSmdTableMtxUndoModule[sPageType]->mUndoInsert( aDataRid,
                                                             aData,
                                                             aDataSize,
                                                             aPageHandle,
                                                             aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
