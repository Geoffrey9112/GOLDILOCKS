/*******************************************************************************
 * smsmbLog.c
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

/**
 * @file smsmbLog.c
 * @brief Storage Manager Layer Bitmap Segment Logging Internal Routines
 */

/**
 * @addtogroup smsmbLog
 * @{
 */

stlStatus smsmbWriteLogUpdateLeafStatus( smxmTrans   * aMiniTrans,
                                         smlTbsId      aTbsId,
                                         smlPid        aPageId,
                                         stlInt16      aOffset,
                                         smpFreeness   aBeforeFreeness,
                                         smpFreeness   aAfterFreeness,
                                         smlScn        aBeforeScn,
                                         smlScn        aAfterScn,
                                         smlEnv      * aEnv )
{
    stlChar  sLogData[STL_SIZEOF(stlInt32) * 2 + STL_SIZEOF(stlInt64) * 2];
    stlInt32 sLogOffset = 0;

    STL_WRITE_MOVE_INT32( sLogData, &aBeforeFreeness, sLogOffset );
    STL_WRITE_MOVE_INT32( sLogData, &aAfterFreeness, sLogOffset );
    STL_WRITE_MOVE_INT64( sLogData, &aBeforeScn, sLogOffset );
    STL_WRITE_MOVE_INT64( sLogData, &aAfterScn, sLogOffset );
    
    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_SEGMENT,
                           SMR_LOG_MEMORY_BITMAP_UPDATE_LEAF_STATUS,
                           SMR_REDO_TARGET_PAGE,
                           (void*)sLogData,
                           sLogOffset,
                           aTbsId,
                           aPageId,
                           aOffset,
                           SMXM_LOGGING_REDO_UNDO,
                           aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmbWriteLogUpdateInternalStatus( smxmTrans   * aMiniTrans,
                                             smlTbsId      aTbsId,
                                             smlPid        aPageId,
                                             stlInt16      aOffset,
                                             smpFreeness   aBeforeFreeness,
                                             smpFreeness   aAfterFreeness,
                                             smlEnv      * aEnv )
{
    stlChar  sLogData[STL_SIZEOF(stlInt32) * 2];
    stlInt32 sLogOffset = 0;

    STL_WRITE_MOVE_INT32( sLogData, &aBeforeFreeness, sLogOffset );
    STL_WRITE_MOVE_INT32( sLogData, &aAfterFreeness, sLogOffset );
    
    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_SEGMENT,
                           SMR_LOG_MEMORY_BITMAP_UPDATE_INTERNAL_STATUS,
                           SMR_REDO_TARGET_PAGE,
                           (void*)sLogData,
                           sLogOffset,
                           aTbsId,
                           aPageId,
                           aOffset,
                           SMXM_LOGGING_REDO_UNDO,
                           aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmbWriteLogFreeExtMap( smxmTrans   * aMiniTrans,
                                   smlTbsId      aTbsId,
                                   smlPid        aPageId,
                                   stlInt16      aOffset,
                                   stlInt32      aStartDesc,
                                   stlInt32      aEndDesc,
                                   smlEnv      * aEnv )
{
    stlChar    sLogData[STL_SIZEOF(stlInt32)*2];
    stlInt32   sLogOffset = 0;

    STL_WRITE_MOVE_INT32( sLogData, &aStartDesc, sLogOffset );
    STL_WRITE_MOVE_INT32( sLogData, &aEndDesc, sLogOffset );

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_SEGMENT,
                           SMR_LOG_MEMORY_BITMAP_FREE_EXTENT_MAP,
                           SMR_REDO_TARGET_PAGE,
                           (void*)sLogData,
                           sLogOffset,
                           aTbsId,
                           aPageId,
                           aOffset,
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmbWriteLogUnlinkLstExtMap( smxmTrans   * aMiniTrans,
                                        smsmbCache  * aCache,
                                        smlPid        aPrvPid,
                                        smlEnv      * aEnv )
{
    stlChar       sLogData[STL_SIZEOF(smlPid)*2];
    stlInt32      sLogOffset = 0;
    smpHandle     sPageHandle;
    smsmbSegHdr * sSegHdr;

    STL_TRY( smpAcquire( aMiniTrans,
                         aCache->mCommonCache.mSegRid.mTbsId,
                         aCache->mCommonCache.mSegRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

    sSegHdr = (smsmbSegHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );

    STL_WRITE_MOVE_INT32( sLogData, &sSegHdr->mLstExtMap, sLogOffset );
    STL_WRITE_MOVE_INT32( sLogData, &aPrvPid, sLogOffset );
    
    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_RECOVERY,
                           SMR_LOG_BYTES,
                           SMR_REDO_TARGET_PAGE,
                           (void*)sLogData,
                           sLogOffset,
                           aCache->mCommonCache.mSegRid.mTbsId,
                           aCache->mCommonCache.mSegRid.mPageId,
                           (stlChar*)(&sSegHdr->mLstExtMap) - (stlChar*)SMP_FRAME( &sPageHandle ),
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmbWriteLogUpdateSegHdr( smxmTrans   * aMiniTrans,
                                     smlTbsId      aTbsId,
                                     smlPid        aPageId,
                                     stlInt16      aOffset,
                                     smsmbSegHdr * aSegHdr,
                                     smlEnv      * aEnv )
{
    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_SEGMENT,
                           SMR_LOG_MEMORY_BITMAP_UPDATE_SEGMENT_HDR,
                           SMR_REDO_TARGET_PAGE,
                           (void*)aSegHdr,
                           STL_SIZEOF( smsmbSegHdr ),
                           aTbsId,
                           aPageId,
                           aOffset,
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmbWriteLogExpandLeaf( smxmTrans   * aMiniTrans,
                                   smlTbsId      aTbsId,
                                   smlPid        aPageId,
                                   stlInt16      aOffset,
                                   smlPid        aStartPid,
                                   stlInt32      aPageCount,
                                   smlEnv      * aEnv )
{
    stlChar    sLogData[STL_SIZEOF(stlInt32)*2];
    stlInt32   sLogOffset = 0;

    STL_WRITE_MOVE_INT32( sLogData, &aStartPid, sLogOffset );
    STL_WRITE_MOVE_INT32( sLogData, &aPageCount, sLogOffset );

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_SEGMENT,
                           SMR_LOG_MEMORY_BITMAP_EXPAND_LEAF,
                           SMR_REDO_TARGET_PAGE,
                           (void*)sLogData,
                           sLogOffset,
                           aTbsId,
                           aPageId,
                           aOffset,
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmbWriteLogInsertInternalBit( smxmTrans   * aMiniTrans,
                                          smlTbsId      aTbsId,
                                          smlPid        aPageId,
                                          stlInt16      aOffset,
                                          smlPid        aInsertPid,
                                          smpFreeness   aInsertFreeness,
                                          smlEnv      * aEnv )
{
    stlChar    sLogData[STL_SIZEOF(stlInt32)*2];
    stlInt32   sLogOffset = 0;
    stlInt32   sInsertFreeness;

    sInsertFreeness = aInsertFreeness;
    STL_WRITE_MOVE_INT32( sLogData, &aInsertPid, sLogOffset );
    STL_WRITE_MOVE_INT32( sLogData, &sInsertFreeness, sLogOffset );

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_SEGMENT,
                           SMR_LOG_MEMORY_BITMAP_INSERT_INTERNAL_BIT,
                           SMR_REDO_TARGET_PAGE,
                           (void*)sLogData,
                           sLogOffset,
                           aTbsId,
                           aPageId,
                           aOffset,
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmbWriteLogInitLeaf( smxmTrans   * aMiniTrans,
                                 smlTbsId      aTbsId,
                                 smlPid        aPageId,
                                 stlInt16      aOffset,
                                 smlEnv      * aEnv )
{
    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_SEGMENT,
                           SMR_LOG_MEMORY_BITMAP_INIT_LEAF,
                           SMR_REDO_TARGET_PAGE,
                           NULL,
                           0,
                           aTbsId,
                           aPageId,
                           aOffset,
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmbWriteLogInitInternal( smxmTrans   * aMiniTrans,
                                     smlTbsId      aTbsId,
                                     smlPid        aPageId,
                                     stlInt16      aOffset,
                                     smlEnv      * aEnv )
{
    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_SEGMENT,
                           SMR_LOG_MEMORY_BITMAP_INIT_INTERNAL,
                           SMR_REDO_TARGET_PAGE,
                           NULL,
                           0,
                           aTbsId,
                           aPageId,
                           aOffset,
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmbWriteLogInitExtMap( smxmTrans   * aMiniTrans,
                                   smlTbsId      aTbsId,
                                   smlPid        aPageId,
                                   stlInt16      aOffset,
                                   smlPid        aPrvMapPid,
                                   smlEnv      * aEnv )
{
    stlChar    sLogData[STL_SIZEOF(stlInt32)];
    stlInt32   sLogOffset = 0;

    STL_WRITE_MOVE_INT32( sLogData, &aPrvMapPid, sLogOffset );
    
    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_SEGMENT,
                           SMR_LOG_MEMORY_BITMAP_INIT_EXTENT_MAP,
                           SMR_REDO_TARGET_PAGE,
                           (void*)sLogData,
                           sLogOffset,
                           aTbsId,
                           aPageId,
                           aOffset,
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmbWriteLogAddExt( smxmTrans   * aMiniTrans,
                               smlTbsId      aTbsId,
                               smlPid        aPageId,
                               stlInt16      aOffset,
                               smlExtId      aExtId,
                               smlEnv      * aEnv )
{
    stlChar    sLogData[STL_SIZEOF(stlInt64)];
    stlInt32   sLogOffset = 0;

    STL_WRITE_MOVE_INT64( sLogData, &aExtId, sLogOffset );
    
    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_SEGMENT,
                           SMR_LOG_MEMORY_BITMAP_ADD_EXTENT,
                           SMR_REDO_TARGET_PAGE,
                           (void*)sLogData,
                           sLogOffset,
                           aTbsId,
                           aPageId,
                           aOffset,
                           SMXM_LOGGING_REDO_UNDO,
                           aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmbWriteLogLeafLink( smxmTrans   * aMiniTrans,
                                 smlTbsId      aTbsId,
                                 smlPid        aTargetPid,
                                 smlPid        aLinkPid,
                                 smlEnv      * aEnv )
{
    stlChar    sLogData[STL_SIZEOF(smlPid) * 2];
    smlPid     sOldPid = SMP_NULL_PID;
    stlInt32   sLogOffset = 0;

    STL_WRITE_MOVE_INT32( sLogData, &sOldPid, sLogOffset );
    STL_WRITE_MOVE_INT32( sLogData, &aLinkPid, sLogOffset );
    
    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_RECOVERY,
                           SMR_LOG_BYTES,
                           SMR_REDO_TARGET_PAGE,
                           (void*)sLogData,
                           sLogOffset,
                           aTbsId,
                           aTargetPid,
                           STL_SIZEOF( smpPhyHdr ) + STL_OFFSETOF( smsmbLeafHdr, mNextPid ),
                           SMXM_LOGGING_REDO_UNDO,
                           aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
