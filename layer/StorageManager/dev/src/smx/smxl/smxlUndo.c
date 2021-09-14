/*******************************************************************************
 * smxlUndo.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smxlUndo.c 7374 2013-02-25 01:37:39Z mkkim $
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
#include <smxl.h>
#include <smxm.h>
#include <sme.h>
#include <smf.h>
#include <sms.h>
#include <smxlUndo.h>
#include <smxlDef.h>

/**
 * @file smxlUndo.c
 * @brief Storage Manager Transaction Undo Internal Routines
 */

/**
 * @addtogroup smxlUndo
 * @{
 */

smxlUndoFunc gSmxlUndoFuncs[SMXL_UNDO_LOG_MAX_COUNT] =
{
    smxlUndoMemoryCircularCreate,
    smxlUndoLockForPrepare
};

stlStatus smxlUndoMemoryCircularCreate( smxmTrans * aMiniTrans,
                                        smlRid      aTargetRid,
                                        void      * aLogBody,
                                        stlInt16    aLogSize,
                                        smlEnv    * aEnv )
{
    smxlTransId   sTransId;
    void        * sSegmentHandle;

    sTransId = smxmGetTransId( aMiniTrans );

    STL_TRY( smsGetSegmentHandle( aTargetRid,
                                  &sSegmentHandle,
                                  aEnv )
             == STL_SUCCESS);

    if( sSegmentHandle != NULL )
    {
        STL_TRY( smsDrop( sTransId,
                          sSegmentHandle,
                          STL_FALSE,  /* aOnStartup */
                          aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /**
         * Segment Header만 할당되어 있는 경우
         */
        STL_TRY( smsFreeSegMapPage( sTransId,
                                    aTargetRid,
                                    aEnv ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smxlUndoLockForPrepare( smxmTrans * aMiniTrans,
                                  smlRid      aTargetRid,
                                  void      * aLogBody,
                                  stlInt16    aLogSize,
                                  smlEnv    * aEnv )
{
    return STL_SUCCESS;
}

/** @} */
