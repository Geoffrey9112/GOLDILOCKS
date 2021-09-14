/*******************************************************************************
 * smpUndo.c
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
#include <smpDef.h>
#include <smpUndo.h>
/**
 * @file smpUndo.c
 * @brief Storage Manager Page Undo Internal Routines
 */

/**
 * @addtogroup smpUndo
 * @{
 */


smxlUndoFunc gSmpUndoFuncs[SMP_UNDO_LOG_MAX_COUNT] =
{
    smpUndoRtsVersioning
};


stlStatus smpUndoRtsVersioning( smxmTrans * aMiniTrans,
                                smlRid      aTargetRid,
                                void      * aLogBody,
                                stlInt16    aLogSize,
                                smlEnv    * aEnv )
{
    return STL_SUCCESS;
}




/** @} */
