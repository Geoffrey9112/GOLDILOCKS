/*******************************************************************************
 * smnUndo.c
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
#include <smlSuppLogDef.h>
#include <smDef.h>
#include <smp.h>
#include <smxl.h>
#include <smxm.h>
#include <smr.h>
#include <smgDef.h>

/**
 * @file smgUndo.c
 * @brief Storage Manager General Undo Internal Routines
 */

/**
 * @addtogroup smgUndo
 * @{
 */

stlStatus smgUndoSupplLogDdl( smxmTrans * aMiniTrans,
                              smlRid      aTargetRid,
                              void      * aLogBody,
                              stlInt16    aLogSize,
                              smlEnv    * aEnv )
{
    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_EXTERNAL,
                           SML_SUPPL_LOG_DDL_CLR,
                           SMR_REDO_TARGET_NONE,
                           aLogBody,
                           aLogSize,
                           aTargetRid.mTbsId,
                           aTargetRid.mPageId,
                           aTargetRid.mOffset,
                           SMXM_LOGGING_REDO,
                           aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

smxlUndoFunc gSmgUndoFuncs[SMG_UNDO_SUPPL_LOG_MAX_COUNT] =
{
    smgUndoSupplLogDdl
};

/** @} */
