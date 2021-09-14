/*******************************************************************************
 * smtUndo.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smtUndo.c 5831 2012-10-04 06:52:36Z mkkim $
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
#include <smxm.h>
#include <smp.h>
#include <smtDef.h>

/**
 * @file smtUndo.c
 * @brief Storage Manager Tablespace Undo Internal Routines
 */

/**
 * @addtogroup smtUndo
 * @{
 */

stlStatus smtUndoMemoryUpdateExtBlockMapHdrState( smxmTrans * aMiniTrans,
                                                  smlRid      aTargetRid,
                                                  void      * aLogBody,
                                                  stlInt16    aLogSize,
                                                  smlEnv    * aEnv )
{
    smtExtBlockMapHdr * sExtBlockMapHdr;
    stlUInt32           sState;
    stlInt16            sOffset = 0;
    smpHandle           sPageHandle;
    
    STL_TRY( smpAcquire( aMiniTrans,
                         aTargetRid.mTbsId,
                         aTargetRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv )
             == STL_SUCCESS );
    
    STL_READ_MOVE_INT32( &sState, aLogBody, sOffset );
    
    sExtBlockMapHdr = (smtExtBlockMapHdr*)smpGetBody( SMP_FRAME(&sPageHandle) );

    sExtBlockMapHdr->mState = sState;

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_TABLESPACE,
                           SMR_LOG_UPDATE_EXT_BLOCK_MAP_HDR_STATE,
                           SMR_REDO_TARGET_PAGE,
                           (void*)aLogBody,
                           aLogSize,
                           aTargetRid.mTbsId,
                           aTargetRid.mPageId,
                           0,
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

smxlUndoFunc gSmtUndoFuncs[SMT_UNDO_LOG_MAX_COUNT] =
{
    smtUndoMemoryUpdateExtBlockMapHdrState
};

/** @} */
