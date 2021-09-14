/*******************************************************************************
 * smdmpfUndo.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smdmpfUndo.c 13496 2014-08-29 05:38:43Z leekmo $
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
#include <smr.h>
#include <sms.h>
#include <smr.h>
#include <smdDef.h>
#include <smdmpfRowDef.h>
#include <smdmpfDef.h>
#include <smdmpf.h>

/**
 * @file smdmpfUndo.c
 * @brief Storage Manager Table Undo Internal Routines
 */

/**
 * @addtogroup smdmpfUndo
 * @{
 */

smdTableUndoModule gSmdmpfTableUndoModule =
{
    smdmpfUndoMemoryHeapInsert,
    smdmpfUndoMemoryHeapUpdate,
    smdmpfUndoMemoryHeapMigrate,
    smdmpfUndoMemoryHeapUpdateLink,
    smdmpfUndoMemoryHeapDelete,
    NULL,                                    /* create */
    NULL,                                    /* drop */
    smdmpfUndoMemoryHeapAppend,
    NULL,                                    /* alter attr */
    NULL,                                    /* pending create */
    NULL,                                    /* pending drop */
    smdmpfUndoMemoryHeapBlockInsert,
    NULL                                     /* pending insert */
};

stlStatus smdmpfUndoMemoryHeapInsert( smxmTrans * aMiniTrans,
                                      smlRid      aTargetRid,
                                      void      * aLogBody,
                                      stlInt16    aLogSize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv )
{
    stlInt16        sSlotSeq = aTargetRid.mOffset;
    stlChar       * sRowSlot;
    stlChar         sLogBuf[SMP_PAGE_SIZE];
    stlChar       * sLogPtr;
    smxlTransId     sTransId;
    smlScn          sTransScn;
    smlRid          sSegRid = smxmGetSegRid(aMiniTrans);
    void          * sRelHandle;
    stlUInt8        sMemberPageType;

    STL_TRY( smeGetRelationHandle( sSegRid, &sRelHandle, aEnv) == STL_SUCCESS );

    sMemberPageType = SMDMPF_GET_MEMBER_PAGE_TYPE( SMP_FRAME(aPageHandle) );

    if( sMemberPageType == SMDMPF_MEMBER_PAGE_TYPE_CONT )
    {
        sRowSlot = (stlChar *)SMDMPF_GET_START_POS(SMP_FRAME(aPageHandle));
    }
    else
    {
        sRowSlot = SMDMPF_GET_NTH_ROW_HDR( SMP_FRAME(aPageHandle), sSlotSeq );
    }

    SMDMPF_SET_DELETED( sRowSlot );

    /* Undo에 의해 작성되는 redo 로그는 redo시 Rts 할당을 받을 일이 없으므로 invalid 값을 세팅한다 */
    sTransId = SML_INVALID_TRANSID;
    sTransScn = SML_INFINITE_SCN;

    sLogPtr = sLogBuf;
    STL_WRITE_INT64( sLogPtr, &sTransId );
    sLogPtr += STL_SIZEOF(smxlTransId);
    STL_WRITE_INT64( sLogPtr, &sTransScn );
    sLogPtr += STL_SIZEOF(smlScn);
    stlMemcpy( sLogPtr, sRowSlot, SMDMPF_ROW_HDR_LEN );
    sLogPtr += SMDMPF_ROW_HDR_LEN;

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_TABLE,
                           SMR_LOG_MEMORY_HEAP_UNINSERT,
                           SMR_REDO_TARGET_PAGE,
                           sLogBuf,
                           sLogPtr - sLogBuf,
                           aTargetRid.mTbsId,
                           aTargetRid.mPageId,
                           aTargetRid.mOffset,  /* Physiological redo를 하기위해 offset이 아닌 seq 사용 */
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    SMDMPF_DECREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) );

    if( SMDMPF_GET_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) ) == 0 )
    {
        STL_TRY( smdmpfUpdatePageStatus( aMiniTrans,
                                         sRelHandle,
                                         SME_GET_SEGMENT_HANDLE(sRelHandle),
                                         aPageHandle,
                                         SMP_FREENESS_FREE,
                                         smxlGetSystemScn(),
                                         aEnv ) == STL_SUCCESS );
    }
    else
    {
        /**
         * Insert가 undo되었으므로 UPDATEONLY page는 INSERTABLE이 된다.
         */
        if( smpGetFreeness(aPageHandle) == SMP_FREENESS_UPDATEONLY )
        {
            STL_TRY( smdmpfUpdatePageStatus( aMiniTrans,
                                             sRelHandle,
                                             SME_GET_SEGMENT_HANDLE(sRelHandle),
                                             aPageHandle,
                                             SMP_FREENESS_INSERTABLE,
                                             0,  /* aScn */
                                             aEnv ) == STL_SUCCESS );
        }
    }

    STL_DASSERT( smpGetSegmentId( aPageHandle ) ==
                 smsGetSegmentId( SME_GET_SEGMENT_HANDLE(sRelHandle) ) );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}




stlStatus smdmpfUndoMemoryHeapUpdate( smxmTrans * aMiniTrans,
                                      smlRid      aTargetRid,
                                      void      * aLogBody,
                                      stlInt16    aLogSize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv )
{
    stlInt16            sSlotSeq = aTargetRid.mOffset;
    stlChar             sRowBuf[SMP_PAGE_SIZE];
    stlChar             sLogBuf[SMP_PAGE_SIZE];
    stlChar           * sLogPtr = aLogBody;
    stlChar           * sLogEnd;
    knlRegionMark       sMemMark;
    knlValueBlockList * sUpdateCols;
    stlInt32            sState = 0;
    smxlTransId         sTransId;
    smlScn              sTransScn;
    stlChar           * sAftRowSlot;
    stlInt16            sAftRowHeaderSize;
    stlInt16            sAftRowPieceSize;
    stlInt16            sAftColListSize;
    stlInt16            sUpdateColCnt;
    stlInt16            sTotalColCnt;
    stlInt32            sFirstColOrd;
    smlPid              sContColPid[SMD_MAX_COLUMN_COUNT_IN_PIECE];

    STL_TRY( smdmpfAnalyzeUpdateLogRec( (stlChar*)aLogBody,
                                        &sMemMark,
                                        &sUpdateColCnt,
                                        &sFirstColOrd,
                                        &sAftColListSize,
                                        &sUpdateCols,
                                        sContColPid,
                                        aEnv ) == STL_SUCCESS );
    sState = 1;

    /* after row를 구한다 */
    sAftRowSlot = SMDMPF_GET_NTH_ROW_HDR( SMP_FRAME(aPageHandle), sSlotSeq );

    SMDMPF_GET_COLUMN_CNT( sAftRowSlot, &sTotalColCnt );
    SMDMPF_GET_ROW_HDR_SIZE( sAftRowSlot, &sAftRowHeaderSize );

    sAftRowPieceSize = sAftRowHeaderSize + sAftColListSize;

    sLogPtr = sLogBuf + STL_SIZEOF(smxlTransId) + STL_SIZEOF(smlScn) 
        + sAftRowHeaderSize + STL_SIZEOF(stlInt32) + STL_SIZEOF(stlInt16);

    if( sUpdateColCnt > 0 )
    {
        /* undo log 처음의 row header와 update column count를 남겨두고 column들의
         * before image 부터 기록한다 */
        stlMemcpy( sRowBuf, sAftRowSlot, sAftRowPieceSize );
        /* row column 원복 */
        STL_TRY( smdmpfCopyAndUpdateColumns( aPageHandle,
                                             sSlotSeq,
                                             sUpdateCols,
                                             0,   /* block idx */
                                             sAftRowPieceSize,
                                             sTotalColCnt,
                                             sFirstColOrd,
                                             sContColPid,
                                             sLogPtr,
                                             &sLogEnd,
                                             aEnv ) == STL_SUCCESS );
    }
    else
    {
        sLogEnd = sLogPtr;
    }

    /* row header 원복 */
    stlMemcpy( sAftRowSlot, aLogBody, sAftRowHeaderSize );

    /* Undo에 의해 작성되는 redo 로그는 redo시 Rts 할당을 받을 일이 없으므로 invalid 값을 세팅한다 */
    sTransId = SML_INVALID_TRANSID;
    sTransScn = SML_INFINITE_SCN;

    sLogPtr = sLogBuf;
    STL_WRITE_INT64( sLogPtr, &sTransId );
    sLogPtr += STL_SIZEOF(smxlTransId);
    STL_WRITE_INT64( sLogPtr, &sTransScn );
    sLogPtr += STL_SIZEOF(smlScn);
    stlMemcpy( sLogPtr, sAftRowSlot, sAftRowHeaderSize );
    sLogPtr += sAftRowHeaderSize;
    STL_WRITE_INT32( sLogPtr, &sFirstColOrd );
    sLogPtr += STL_SIZEOF(stlInt32);
    STL_WRITE_INT16( sLogPtr, &sUpdateColCnt );
    sLogPtr += STL_SIZEOF(stlInt16);

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_TABLE,
                           SMR_LOG_MEMORY_HEAP_UNUPDATE,
                           SMR_REDO_TARGET_PAGE,
                           sLogBuf,
                           sLogEnd - sLogBuf,
                           aTargetRid.mTbsId,
                           aTargetRid.mPageId,
                           aTargetRid.mOffset,  /* Physiological redo를 하기위해 offset이 아닌 seq 사용 */
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       (knlEnv*)aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                  &sMemMark,
                                  STL_FALSE, /* aFreeChunk */
                                  (knlEnv*)aEnv );
    }

    return STL_FAILURE;
}


stlStatus smdmpfUndoMemoryHeapMigrate( smxmTrans * aMiniTrans,
                                       smlRid      aTargetRid,
                                       void      * aLogBody,
                                       stlInt16    aLogSize,
                                       smpHandle * aPageHandle,
                                       smlEnv    * aEnv )
{
    return STL_FAILURE;
}


stlStatus smdmpfUndoMemoryHeapUpdateLink( smxmTrans * aMiniTrans,
                                          smlRid      aTargetRid,
                                          void      * aLogBody,
                                          stlInt16    aLogSize,
                                          smpHandle * aPageHandle,
                                          smlEnv    * aEnv )
{
    return STL_FAILURE;
}


stlStatus smdmpfUndoMemoryHeapDelete( smxmTrans * aMiniTrans,
                                      smlRid      aTargetRid,
                                      void      * aLogBody,
                                      stlInt16    aLogSize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv )
{
    stlInt16           sSlotSeq = aTargetRid.mOffset;
    stlChar            sLogBuf[SMP_PAGE_SIZE];
    stlChar          * sLogPtr = aLogBody;
    smxlTransId        sTransId;
    smlScn             sTransScn;
    stlChar          * sAftRowSlot;
    stlInt16           sAftRowHeaderSize;
    smlRid             sSegRid = smxmGetSegRid(aMiniTrans);
    void             * sRelHandle;
    stlUInt8           sMemberPageType;

    STL_TRY( smeGetRelationHandle( sSegRid, &sRelHandle, aEnv) == STL_SUCCESS );

    sMemberPageType = SMDMPF_GET_MEMBER_PAGE_TYPE( SMP_FRAME(aPageHandle) );

    /* after row를 구한다 */ 
    if( sMemberPageType == SMDMPF_MEMBER_PAGE_TYPE_CONT )
    {
        sAftRowSlot = (stlChar *)SMDMPF_GET_START_POS(SMP_FRAME(aPageHandle));
    }
    else
    {
        sAftRowSlot = SMDMPF_GET_NTH_ROW_HDR( SMP_FRAME(aPageHandle), sSlotSeq );
    }

    SMDMPF_GET_ROW_HDR_SIZE( sAftRowSlot, &sAftRowHeaderSize );

    /* row header 원복 */
    stlMemcpy( sAftRowSlot, aLogBody, sAftRowHeaderSize );

    /* Undo에 의해 작성되는 redo 로그는 redo시 Rts 할당을 받을 일이 없으므로 invalid 값을 세팅한다 */
    sTransId = SML_INVALID_TRANSID;
    sTransScn = SML_INFINITE_SCN;

    sLogPtr = sLogBuf;
    STL_WRITE_INT64( sLogPtr, &sTransId );
    sLogPtr += STL_SIZEOF(smxlTransId);
    STL_WRITE_INT64( sLogPtr, &sTransScn );
    sLogPtr += STL_SIZEOF(smlScn);
    stlMemcpy( sLogPtr, sAftRowSlot, sAftRowHeaderSize );
    sLogPtr += sAftRowHeaderSize;

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_TABLE,
                           SMR_LOG_MEMORY_HEAP_UNDELETE,
                           SMR_REDO_TARGET_PAGE,
                           sLogBuf,
                           sLogPtr - sLogBuf,
                           aTargetRid.mTbsId,
                           aTargetRid.mPageId,
                           aTargetRid.mOffset,  /* Physiological redo를 하기위해 offset이 아닌 seq 사용 */
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    /* delete가 rollback 되었으니 reclaimed space에서 일단 제거한다 */
    SMDMPF_INCREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) );

    if( SMDMPF_GET_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) ) == 1 )
    {
        STL_TRY( smdmpfUpdatePageStatus( aMiniTrans,
                                         sRelHandle,
                                         SME_GET_SEGMENT_HANDLE(sRelHandle),
                                         aPageHandle,
                                         SMP_FREENESS_INSERTABLE,
                                         0,  /* aScn */
                                         aEnv ) == STL_SUCCESS );
    }

    if( (smpGetFreeness(aPageHandle) == SMP_FREENESS_INSERTABLE) &&
        (SMDMPF_IS_PAGE_OVER_INSERT_LIMIT(SMP_FRAME(aPageHandle)) == STL_TRUE) )
    {
        STL_TRY( smdmpfUpdatePageStatus( aMiniTrans,
                                         sRelHandle,
                                         SME_GET_SEGMENT_HANDLE(sRelHandle),
                                         aPageHandle,
                                         SMP_FREENESS_UPDATEONLY,
                                         0,  /* aScn */
                                         aEnv ) == STL_SUCCESS );
    }

    STL_DASSERT( smpGetSegmentId( aPageHandle ) ==
                 smsGetSegmentId( SME_GET_SEGMENT_HANDLE(sRelHandle) ) );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmpfUndoMemoryHeapAppend( smxmTrans * aMiniTrans,
                                      smlRid      aTargetRid,
                                      void      * aLogBody,
                                      stlInt16    aLogSize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv )
{
    STL_DASSERT( STL_FALSE );

    return STL_SUCCESS;
}

stlStatus smdmpfUndoMemoryHeapBlockInsert( smxmTrans * aMiniTrans,
                                           smlRid      aTargetRid,
                                           void      * aLogBody,
                                           stlInt16    aLogSize,
                                           smpHandle * aPageHandle,
                                           smlEnv    * aEnv )
{
    stlInt16        sSlotSeq;
    stlChar       * sRowSlot;
    stlChar         sLogBuf[SMP_PAGE_SIZE];
    stlChar       * sLogPtr;
    smxlTransId     sTransId;
    smlScn          sTransScn;
    smlRid          sSegRid = smxmGetSegRid(aMiniTrans);
    void          * sRelHandle;
    stlInt16        sRowOffsetArray[SMP_PAGE_SIZE / SMDMPF_MINIMUM_ROW_PIECE_SIZE];
    stlInt16        sBlockSize;
    stlInt32        i;

    STL_TRY( smeGetRelationHandle( sSegRid, &sRelHandle, aEnv) == STL_SUCCESS );

    sBlockSize = aLogSize / STL_SIZEOF(stlInt16);
    stlMemcpy( sRowOffsetArray, aLogBody, aLogSize );
    
    for( i = sBlockSize - 1; i >= 0; i-- )
    {
        sSlotSeq = sRowOffsetArray[i];

        sRowSlot = SMDMPF_GET_NTH_ROW_HDR( SMP_FRAME(aPageHandle), sSlotSeq );

        SMDMPF_SET_DELETED( sRowSlot );

        /* Undo에 의해 작성되는 redo 로그는 redo시 Rts 할당을 받을 일이 없으므로 invalid 값을 세팅한다 */
        sTransId = SML_INVALID_TRANSID;
        sTransScn = SML_INFINITE_SCN;

        sLogPtr = sLogBuf;
        STL_WRITE_INT64( sLogPtr, &sTransId );
        sLogPtr += STL_SIZEOF(smxlTransId);
        STL_WRITE_INT64( sLogPtr, &sTransScn );
        sLogPtr += STL_SIZEOF(smlScn);
        stlMemcpy( sLogPtr, sRowSlot, SMDMPF_ROW_HDR_LEN );
        sLogPtr += SMDMPF_ROW_HDR_LEN;

        STL_TRY( smxmWriteLog( aMiniTrans,
                               SMG_COMPONENT_TABLE,
                               SMR_LOG_MEMORY_HEAP_UNINSERT,
                               SMR_REDO_TARGET_PAGE,
                               sLogBuf,
                               sLogPtr - sLogBuf,
                               aTargetRid.mTbsId,
                               aTargetRid.mPageId,
                               sSlotSeq,  /* Physiological redo를 하기위해 offset이 아닌 seq 사용 */
                               SMXM_LOGGING_REDO,
                               aEnv ) == STL_SUCCESS );

        SMDMPF_DECREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) );
    }

    if( SMDMPF_GET_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) ) == 0 )
    {
        STL_TRY( smdmpfUpdatePageStatus( aMiniTrans,
                                         sRelHandle,
                                         SME_GET_SEGMENT_HANDLE(sRelHandle),
                                         aPageHandle,
                                         SMP_FREENESS_FREE,
                                         smxlGetSystemScn(),
                                         aEnv ) == STL_SUCCESS );
    }
    else
    {
        /**
         * Insert가 undo되었으므로 UPDATEONLY page는 INSERTABLE이 된다.
         */
        if( smpGetFreeness(aPageHandle) == SMP_FREENESS_UPDATEONLY )
        {
            STL_TRY( smdmpfUpdatePageStatus( aMiniTrans,
                                             sRelHandle,
                                             SME_GET_SEGMENT_HANDLE(sRelHandle),
                                             aPageHandle,
                                             SMP_FREENESS_INSERTABLE,
                                             0,  /* aScn */
                                             aEnv ) == STL_SUCCESS );
        }
    }

    STL_DASSERT( smpGetSegmentId( aPageHandle ) ==
                 smsGetSegmentId( SME_GET_SEGMENT_HANDLE(sRelHandle) ) );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
