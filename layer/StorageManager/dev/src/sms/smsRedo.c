/*******************************************************************************
 * smsRedo.c
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
#include <smsRedo.h>
#include <smsmbRedo.h>
#include <smsmcRedo.h>
#include <smsmpRedo.h>
#include <smsmbMtxUndo.h>

/**
 * @file smsRedo.c
 * @brief Storage Manager Layer Segment Redo Internal Routines
 */

/**
 * @addtogroup smsRedo
 * @{
 */

smrRedoVector gSmsRedoVectors[SMR_SEGMENT_LOG_MAX_COUNT] =
{
    { /* SMR_LOG_MEMORY_CIRCULAR_UPDATE_SEGMENT_HDR */
        smsmcRedoUpdateSegHdr,
        NULL,
        "MEMORY_CIRCULAR_UPDATE_SEGMENT_HEADER"
    },
    { /* SMR_LOG_MEMORY_CIRCULAR_UPDATE_EXTENT_HDR */
        smsmcRedoUpdateExtHdr,
        NULL,
        "MEMORY_CIRCULAR_UPDATE_EXTENT_HEADER"
    },
    { /* SMR_LOG_MEMORY_BITMAP_INIT_INTERNAL_NODE */
        smsmbRedoInitInternal,
        NULL,
        "MEMORY_BITMAP_INIT_INTERNAL_NODE"
    },
    { /* SMR_LOG_MEMORY_BITMAP_INIT_LEAF_NODE */
        smsmbRedoInitLeaf,
        NULL,
        "MEMORY_BITMAP_INIT_LEAF_NODE"
    },
    { /* SMR_LOG_MEMORY_BITMAP_UPDATE_LEAF_STATUS */
        smsmbRedoUpdateLeafStatus,
        smsmbMtxUndoUpdateLeafStatus,
        "MEMORY_BITMAP_UPDATE_LEAF_STATUS"
    },
    { /* SMR_LOG_MEMORY_BITMAP_UPDATE_INTERNAL_STATUS */
        smsmbRedoUpdateInternalStatus,
        smsmbMtxUndoUpdateInternalStatus,
        "MEMORY_BITMAP_UPDATE_INTERNAL_STATUS"
    },
    { /* SMR_LOG_MEMORY_BITMAP_EXPAND_LEAF */
        smsmbRedoExpandLeaf,
        NULL,
        "MEMORY_BITMAP_EXPAND_LEAF"
    },
    { /* SMR_LOG_MEMORY_BITMAP_INSERT_INTERNAL_BIT */
        smsmbRedoInsertInternalBit,
        NULL,
        "SMR_LOG_MEMORY_BITMAP_INSERT_INTERNAL_BIT"
    },
    { /* SMR_LOG_MEMORY_BITMAP_INIT_EXTENT_MAP */
        smsmbRedoInitExtMap,
        NULL,
        "MEMORY_BITMAP_INIT_EXTENT_MAP"
    },
    { /* SMR_LOG_MEMORY_BITMAP_ADD_EXTENT */
        smsmbRedoAddExt,
        smsmbMtxUndoAddExt,
        "MEMORY_BITMAP_ADD_EXTENT"
    },
    { /* SMR_LOG_MEMORY_BITMAP_UPDATE_SEGMENT_HDR */
        smsmbRedoUpdateSegHdr,
        NULL,
        "MEMORY_BITMAP_UPDATE_SEGMENT_HEADER"
    },
    { /* SMR_LOG_MEMORY_BITMAP_FREE_EXTENT_MAP */
        smsmbRedoFreeExtMap,
        NULL,
        "MEMORY_BITMAP_FREE_EXTENT_MAP"
    },
    { /* SMR_LOG_MEMORY_PENDING_UPDATE_SEGMENT_HDR */
        smsmpRedoUpdateSegHdr,
        NULL,
        "MEMORY_PENDING_UPDATE_SEGMENT_HEADER"
    },
    { /* SMR_LOG_MEMORY_PENDING_UPDATE_EXTENT_HDR */
        smsmpRedoUpdateExtHdr,
        NULL,
        "MEMORY_PENDING_UPDATE_EXTENT_HEADER"
    },
    { /* SMR_LOG_INIT_SEGMENT_HDR */
        smsRedoInitSegHdr,
        NULL,
        "INIT_SEGMENT_HEADER"
    }
};

stlStatus smsRedoInitSegHdr( smlRid    * aDataRid,
                             void      * aData,
                             stlUInt16   aDataSize,
                             smpHandle * aPageHandle,
                             smlEnv    * aEnv )
{
    smsSegHdr * sSegHdr;

    sSegHdr = (smsSegHdr*)smpGetBody( SMP_FRAME( aPageHandle ) );
    stlMemcpy( sSegHdr, aData, STL_SIZEOF(smsSegHdr) );
    
    return STL_SUCCESS;
}

/** @} */
