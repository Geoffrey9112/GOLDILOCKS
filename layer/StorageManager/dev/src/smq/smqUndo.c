/*******************************************************************************
 * smqUndo.c
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
#include <smq.h>
#include <smg.h>
#include <smf.h>
#include <smqDef.h>
#include <smqUndo.h>
#include <smqMap.h>

/**
 * @file smqUndo.c
 * @brief Storage Manager Sequence Undo Internal Routines
 */

/**
 * @addtogroup smqUndo
 * @{
 */

smxlUndoFunc gSmqUndoFuncs[SMQ_UNDO_LOG_MAX_COUNT] =
{
    smqUndoCreateSequence
};

stlStatus smqUndoCreateSequence( smxmTrans * aMiniTrans,
                                 smlRid      aTargetRid,
                                 void      * aLogBody,
                                 stlInt16    aLogSize,
                                 smlEnv    * aEnv )
{
    void  * sSequenceHandle;

    /**
     * Restart Undo 시에는 Cache가 Build되지 않은 상태이다.
     */
    
    if( smfGetServerState() != SML_SERVER_STATE_RECOVERING )
    {
        STL_TRY( smqGetSequenceHandle( aTargetRid,
                                       (void**)&sSequenceHandle,
                                       aEnv )
                 == STL_SUCCESS );
    
        /**
         * Cache 삭제
         */

        STL_TRY( smgFreeShmMem4Open( sSequenceHandle,
                                     aEnv )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
