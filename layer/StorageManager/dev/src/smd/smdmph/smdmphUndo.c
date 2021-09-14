/*******************************************************************************
 * smdmphUndo.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smdmphUndo.c 13496 2014-08-29 05:38:43Z leekmo $
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
#include <smdmphRowDef.h>
#include <smdmphDef.h>
#include <smdmph.h>

/**
 * @file smdmphUndo.c
 * @brief Storage Manager Table Undo Internal Routines
 */

/**
 * @addtogroup smdmphUndo
 * @{
 */

smdTableUndoModule gSmdmphTableUndoModule =
{
    smdmphUndoMemoryHeapInsert,
    smdmphUndoMemoryHeapUpdate,
    smdmphUndoMemoryHeapMigrate,
    smdmphUndoMemoryHeapUpdateLink,
    smdmphUndoMemoryHeapDelete,
    NULL,                                    /* create */
    NULL,                                    /* drop */
    smdmphUndoMemoryHeapAppend,
    NULL,                                    /* alter attr */
    NULL,                                    /* pending create */
    NULL,                                    /* pending drop */
    smdmphUndoMemoryHeapBlockInsert,
    NULL                                     /* pending insert */
};

stlStatus smdmphUndoMemoryHeapInsert( smxmTrans * aMiniTrans,
                                      smlRid      aTargetRid,
                                      void      * aLogBody,
                                      stlInt16    aLogSize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv )
{
    stlInt16        sSlotSeq = aTargetRid.mOffset;
    stlChar       * sRowSlot;
    stlInt16        sRowHeaderSize;
    stlInt16        sTotalRowSize;
    stlChar         sLogBuf[SMP_PAGE_SIZE];
    stlChar       * sLogPtr;
    smxlTransId     sTransId;
    smlScn          sTransScn;
    smlRid          sSegRid = smxmGetSegRid(aMiniTrans);
    void          * sRelHandle;
    stlInt16        sPadSpace;

    STL_TRY( smeGetRelationHandle( sSegRid, &sRelHandle, aEnv) == STL_SUCCESS );

    sRowSlot = smpGetNthRowItem( aPageHandle, sSlotSeq );

    SMDMPH_GET_ROW_HDR_SIZE( sRowSlot, &sRowHeaderSize );
    sTotalRowSize =  smdmphGetRowSize(sRowSlot);

    SMDMPH_SET_DELETED(sRowSlot);

    /* Undo에 의해 작성되는 redo 로그는 redo시 Rts 할당을 받을 일이 없으므로 invalid 값을 세팅한다 */
    sTransId = SML_INVALID_TRANSID;
    sTransScn = SML_INFINITE_SCN;

    sLogPtr = sLogBuf;
    STL_WRITE_INT64( sLogPtr, &sTransId );
    sLogPtr += STL_SIZEOF(smxlTransId);
    STL_WRITE_INT64( sLogPtr, &sTransScn );
    sLogPtr += STL_SIZEOF(smlScn);
    stlMemcpy( sLogPtr, sRowSlot, sRowHeaderSize );
    sLogPtr += sRowHeaderSize;

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

    SMDMPH_GET_PAD_SPACE( sRowSlot, &sPadSpace );
    
    /* delete 되었으니 reclaimed space에 일단 추가한다 */
    SMP_ADD_RECLAIMED_SPACE( SMP_FRAME(aPageHandle), sTotalRowSize + sPadSpace );
    SMP_DECREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) );

    if( SMP_GET_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) ) == 0 )
    {
        STL_TRY( smdmphUpdatePageStatus( aMiniTrans,
                                         sRelHandle,
                                         SME_GET_SEGMENT_HANDLE(sRelHandle),
                                         aPageHandle,
                                         SMP_FREENESS_FREE,
                                         smxlGetSystemScn(),
                                         aEnv ) == STL_SUCCESS );
    }
    else
    {
        if( (smpGetFreeness(aPageHandle) == SMP_FREENESS_UPDATEONLY) &&
            (smpIsPageOverFreeLimit(SMP_FRAME(aPageHandle),
                                    SMD_GET_PCTUSED(sRelHandle)) == STL_TRUE) )
        {
            STL_TRY( smdmphUpdatePageStatus( aMiniTrans,
                                             sRelHandle,
                                             SME_GET_SEGMENT_HANDLE(sRelHandle),
                                             aPageHandle,
                                             SMP_FREENESS_INSERTABLE,
                                             0,  /* aScn */
                                             aEnv ) == STL_SUCCESS );
        }
    }

    smdmphValidatePadSpace( aPageHandle );
    STL_DASSERT( smpGetSegmentId( aPageHandle ) ==
                 smsGetSegmentId( SME_GET_SEGMENT_HANDLE(sRelHandle) ) );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}




stlStatus smdmphUndoMemoryHeapUpdate( smxmTrans * aMiniTrans,
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
    stlChar           * sAftRowPtr;
    stlInt16            sAftRowHeaderSize;
    stlInt16            sAftRowPieceSize;
    stlInt16            sAftColListSize;
    stlUInt8            sAftRtsSeq;
    stlUInt8            sBfrRtsSeq;
    stlInt16            sUpdateColCnt;
    stlInt16            sTotalColCnt;
    stlBool             sIsColSizeDiff;
    stlInt64            sRowSizeDiff;
    stlInt16            sPadSpace;
    stlInt32            sFirstColOrd;

    STL_TRY( smdmphAnalyzeUpdateLogRec( (stlChar*)aLogBody,
                                        &sMemMark,
                                        &sBfrRtsSeq,
                                        &sUpdateColCnt,
                                        &sFirstColOrd,
                                        &sUpdateCols,
                                        aEnv ) == STL_SUCCESS );
    sState = 1;

    /* after row를 구한다 */ 
    STL_TRY( smpAcquire( aMiniTrans,
                         aTargetRid.mTbsId,
                         aTargetRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         aPageHandle,
                         aEnv ) == STL_SUCCESS );

    sAftRowSlot = smpGetNthRowItem( aPageHandle, sSlotSeq );
    if( SMDMPH_IS_RTS_BOUND( sAftRowSlot ) == STL_TRUE )
    {
        SMDMPH_GET_RTSSEQ( sAftRowSlot, &sAftRtsSeq );
    }
    else
    {
        sAftRtsSeq = SMP_RTS_SEQ_NULL;
    }
    SMDMPH_GET_COLUMN_CNT( sAftRowSlot, &sTotalColCnt );
    SMDMPH_GET_ROW_HDR_SIZE( sAftRowSlot, &sAftRowHeaderSize );
    sAftRowPtr  = sAftRowSlot + sAftRowHeaderSize;

    STL_TRY( smdmphGetUpdatePieceSize( sAftRowPtr,
                                       sTotalColCnt,
                                       sFirstColOrd,
                                       sUpdateCols,
                                       NULL, /* aBeforeCols */
                                       0,    /* block idx */
                                       NULL,
                                       NULL,
                                       &sAftColListSize,
                                       &sIsColSizeDiff,
                                       &sRowSizeDiff,
                                       &sUpdateColCnt ) == STL_SUCCESS );

    sAftRowPieceSize = sAftRowHeaderSize + sAftColListSize;

    SMDMPH_GET_PAD_SPACE( sAftRowSlot, &sPadSpace );
    /* inplace undo가 가능해야 함 */
    STL_ASSERT( sRowSizeDiff <= sPadSpace );

    sLogPtr = sLogBuf + STL_SIZEOF(smxlTransId) + STL_SIZEOF(smlScn) 
        + sAftRowHeaderSize + STL_SIZEOF(stlInt32) + STL_SIZEOF(stlInt16);

    if( sUpdateColCnt > 0 )
    {
        /* undo log 처음의 row header와 update column count를 남겨두고 column들의
         * before image 부터 기록한다 */
        stlMemcpy( sRowBuf, sAftRowSlot, sAftRowPieceSize );
        /* row column 원복 */
        STL_TRY( smdmphCopyAndUpdateColumns( sRowBuf,
                                             sAftRowSlot, /* before == after slot */
                                             sUpdateCols,
                                             0,   /* block idx */
                                             sAftRowPieceSize,
                                             sTotalColCnt,
                                             sFirstColOrd,
                                             sLogPtr,
                                             &sLogEnd,
                                             sIsColSizeDiff,
                                             aEnv ) == STL_SUCCESS );

        sPadSpace -= sRowSizeDiff;
    }
    else
    {
        sLogEnd = sLogPtr;
    }

    /* row header 원복 */
    stlMemcpy( sAftRowSlot, aLogBody, sAftRowHeaderSize );
    /* undo 이후의 pad size로 갱신 */ 
    SMDMPH_SET_PAD_SPACE( sAftRowSlot, &sPadSpace );

    /* RTS 조정 */
    if( SMP_IS_VALID_RTS_SEQ(sBfrRtsSeq) != STL_TRUE )
    {
        /* sBfrRtsSeq가 NULL SEQ면 commit된 버전을 update한 경우이므로 rts 조정이 필요하다  */
        if( SMP_IS_VALID_RTS_SEQ( sAftRtsSeq ) == STL_TRUE )
        {
            STL_TRY( smpEndUseRts( aPageHandle,
                                   sAftRowSlot,
                                   sAftRtsSeq,
                                   0,
                                   aEnv ) == STL_SUCCESS );
        }
    }
    else
    {
        /* sBfrRtsSeq가 NULL SEQ이 아니면 같은 tx에 의해 생성된 버전을 update한 경우이므로 rts 조정이 필요없다 */
        /* 대신 update 이후 compact 등으로 사용하는 rts가 바뀌었을 수 있으므로 undo 이전의 rts seq를 */
        /* 그대로 유지시켜야 한다                                                                    */
        if( SMP_IS_VALID_RTS_SEQ( sAftRtsSeq ) == STL_TRUE )
        {
            /* RTS Bound row 일때만 rts seq를 세팅한다 */
            SMDMPH_SET_RTS_BOUND( sAftRowSlot );
            SMDMPH_SET_RTSSEQ( sAftRowSlot, &sAftRtsSeq );
        }
    }

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

    smdmphValidatePadSpace( aPageHandle );

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


stlStatus smdmphUndoMemoryHeapMigrate( smxmTrans * aMiniTrans,
                                       smlRid      aTargetRid,
                                       void      * aLogBody,
                                       stlInt16    aLogSize,
                                       smpHandle * aPageHandle,
                                       smlEnv    * aEnv )
{
    stlInt16            sSlotSeq = aTargetRid.mOffset;
    stlChar             sLogBuf[SMP_PAGE_SIZE];
    stlChar           * sLogPtr = aLogBody;
    stlChar           * sAftRowSlot;
    stlUInt8            sAftRtsSeq;
    stlUInt8            sBfrRtsSeq;
    stlInt16            sPadSpace;
    stlInt16            sTotalRowSize = 0;
    stlInt16            sBeforeRowSize = 0;
    stlInt16            sLogRowSize = smdmphGetRowSize(aLogBody);
    stlInt16            sLogPadSpace;

    SMDMPH_GET_PAD_SPACE(aLogBody, &sLogPadSpace);
    /* after row를 구한다 */ 
    STL_TRY( smpAcquire( aMiniTrans,
                         aTargetRid.mTbsId,
                         aTargetRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         aPageHandle,
                         aEnv ) == STL_SUCCESS );

    sAftRowSlot = smpGetNthRowItem( aPageHandle, sSlotSeq );
    if( SMDMPH_IS_RTS_BOUND( sAftRowSlot ) == STL_TRUE )
    {
        SMDMPH_GET_RTSSEQ( sAftRowSlot, &sAftRtsSeq );
    }
    else
    {
        sAftRtsSeq = SMP_RTS_SEQ_NULL;
    }
    sTotalRowSize = smdmphGetRowSize(sAftRowSlot);
    SMDMPH_GET_PAD_SPACE( sAftRowSlot, &sPadSpace );
    sTotalRowSize += sPadSpace;
    /* inplace undo가 가능해야 함 */
    STL_ASSERT( (sTotalRowSize >= aLogSize) &&
                (sLogRowSize + sLogPadSpace == sTotalRowSize) );
    /* undo log 반영 */
    stlMemcpy( sAftRowSlot, aLogBody, aLogSize );

    /* pad space가 runtime과 undo recover가 다를 수 있어서 재조정 필요 */
    sBeforeRowSize = smdmphGetRowSize((stlChar*)aLogBody);
    sPadSpace = sTotalRowSize - sBeforeRowSize;
    SMDMPH_SET_PAD_SPACE( sAftRowSlot, &sPadSpace );

    if( SMDMPH_IS_RTS_BOUND( aLogBody ) == STL_TRUE )
    {
        SMDMPH_GET_RTSSEQ( aLogBody, &sBfrRtsSeq );
    }
    else
    {
        sBfrRtsSeq = SMP_RTS_SEQ_NULL;
    }
    /* RTS 조정 */
    if( SMP_IS_VALID_RTS_SEQ(sBfrRtsSeq) != STL_TRUE )
    {
        /* sBfrRtsSeq가 NULL SEQ면 commit된 버전을 update한 경우이므로 rts 조정이 필요하다  */
        if( SMP_IS_VALID_RTS_SEQ( sAftRtsSeq ) == STL_TRUE )
        {
            STL_TRY( smpEndUseRts( aPageHandle,
                                   sAftRowSlot,
                                   sAftRtsSeq,
                                   0,
                                   aEnv ) == STL_SUCCESS );
        }
    }
    else
    {
        /* sBfrRtsSeq가 NULL SEQ이 아니면 같은 tx에 의해 생성된 버전을 update한 경우이므로 rts 조정이 필요없다 */
        /* 대신 update 이후 compact 등으로 사용하는 rts가 바뀌었을 수 있으므로 undo 이전의 rts seq를 */
        /* 그대로 유지시켜야 한다                                                                    */
        if( SMP_IS_VALID_RTS_SEQ( sAftRtsSeq ) == STL_TRUE )
        {
            SMDMPH_SET_RTS_BOUND( sAftRowSlot );
            SMDMPH_SET_RTSSEQ( sAftRowSlot, &sAftRtsSeq );
        }
    }

    sLogPtr = sLogBuf;
    stlMemcpy( sLogPtr, sAftRowSlot, sTotalRowSize );
    sLogPtr += sTotalRowSize;

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_TABLE,
                           SMR_LOG_MEMORY_HEAP_UNMIGRATE,
                           SMR_REDO_TARGET_PAGE,
                           sLogBuf,
                           sLogPtr - sLogBuf,
                           aTargetRid.mTbsId,
                           aTargetRid.mPageId,
                           aTargetRid.mOffset,  /* Physiological redo를 하기위해 offset이 아닌 seq 사용 */
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    smdmphValidatePadSpace( aPageHandle );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdmphUndoMemoryHeapUpdateLink( smxmTrans * aMiniTrans,
                                          smlRid      aTargetRid,
                                          void      * aLogBody,
                                          stlInt16    aLogSize,
                                          smpHandle * aPageHandle,
                                          smlEnv    * aEnv )
{
    stlChar    * sRowSlot;
    stlUInt8     sAftRtsSeq;
    smxlTransId  sTransId;
    smlScn       sViewScn;
    stlChar      sLogBuf[SMP_PAGE_SIZE];
    stlChar    * sLogPtr;
    stlInt16     sHdrSize;
    stlInt16     sPadSpace;

    STL_TRY( smpAcquire( aMiniTrans,
                         aTargetRid.mTbsId,
                         aTargetRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         aPageHandle,
                         aEnv ) == STL_SUCCESS );

    sRowSlot = smpGetNthRowItem( aPageHandle, aTargetRid.mOffset );
    SMDMPH_GET_PAD_SPACE( sRowSlot, &sPadSpace );
    if( SMDMPH_IS_RTS_BOUND( sRowSlot ) == STL_TRUE )
    {
        SMDMPH_GET_RTSSEQ( sRowSlot, &sAftRtsSeq );
    }
    else
    {
        sAftRtsSeq = SMP_RTS_SEQ_NULL;
    }

    SMDMPH_GET_ROW_HDR_SIZE( aLogBody,  &sHdrSize );
    stlMemcpy( sRowSlot, aLogBody, sHdrSize );

    /* pad space가 runtime과 undo recover가 다를 수 있어서 재조정 필요 */
    SMDMPH_SET_PAD_SPACE( sRowSlot, &sPadSpace );

    /* RTS 조정 */
    if( SMDMPH_IS_RTS_BOUND( aLogBody ) == STL_TRUE )
    {
        /* sBfrRtsSeq가 NULL SEQ이 아니면 같은 tx에 의해 생성된 버전을 update한 경우이므로 rts 조정이 필요없다 */
        /* 대신 update 이후 compact 등으로 사용하는 rts가 바뀌었을 수 있으므로 undo 이전의 rts seq를 */
        /* 그대로 유지시켜야 한다                                                                    */
        if( SMP_IS_VALID_RTS_SEQ(sAftRtsSeq) == STL_TRUE )
        {
            SMDMPH_SET_RTS_BOUND( sRowSlot );
            SMDMPH_SET_RTSSEQ( sRowSlot, &sAftRtsSeq );
        }
    }
    else
    {
        /* sBfrRtsSeq가 NULL SEQ면 commit된 버전을 update한 경우이므로 rts 조정이 필요하다  */
        if( SMP_IS_VALID_RTS_SEQ( sAftRtsSeq ) == STL_TRUE )
        {
            STL_TRY( smpEndUseRts( aPageHandle,
                                   sRowSlot,
                                   sAftRtsSeq,
                                   0,
                                   aEnv ) == STL_SUCCESS );
        }
    }

    sTransId = SML_INVALID_TRANSID;
    sViewScn = SML_INFINITE_SCN;
    sLogPtr = sLogBuf;
    STL_WRITE_INT64( sLogPtr, &sTransId );
    sLogPtr += STL_SIZEOF(smxlTransId);
    STL_WRITE_INT64( sLogPtr, &sViewScn );
    sLogPtr += STL_SIZEOF(smlScn);
    stlMemcpy( sLogPtr, sRowSlot, sHdrSize );
    sLogPtr += sHdrSize;


    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_TABLE,
                           SMR_LOG_MEMORY_HEAP_UPDATE_LINK,
                           SMR_REDO_TARGET_PAGE,
                           (void*)sLogBuf,
                           sLogPtr - sLogBuf,
                           aTargetRid.mTbsId,
                           aTargetRid.mPageId,
                           aTargetRid.mOffset,  /* Physiological redo를 하기위해 offset이 아닌 seq 사용 */
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdmphUndoMemoryHeapDelete( smxmTrans * aMiniTrans,
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
    stlUInt8           sAftRtsSeq;
    stlUInt8           sBfrRtsSeq;
    smlRid             sSegRid = smxmGetSegRid(aMiniTrans);
    void             * sRelHandle;
    stlInt16           sTotalRowSize;
    stlInt16           sPadSpace;

    STL_TRY( smeGetRelationHandle( sSegRid, &sRelHandle, aEnv) == STL_SUCCESS );

    /* after row를 구한다 */ 
    STL_TRY( smpAcquire( aMiniTrans,
                         aTargetRid.mTbsId,
                         aTargetRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         aPageHandle,
                         aEnv ) == STL_SUCCESS );

    sAftRowSlot = smpGetNthRowItem( aPageHandle, sSlotSeq );
    if( SMDMPH_IS_RTS_BOUND( sAftRowSlot ) == STL_TRUE )
    {
        SMDMPH_GET_RTSSEQ( sAftRowSlot, &sAftRtsSeq );
    }
    else
    {
        sAftRtsSeq = SMP_RTS_SEQ_NULL;
    }
    SMDMPH_GET_ROW_HDR_SIZE( sAftRowSlot, &sAftRowHeaderSize );
    SMDMPH_GET_PAD_SPACE( sAftRowSlot, &sPadSpace );

    if( SMDMPH_IS_RTS_BOUND( aLogBody ) == STL_TRUE )
    {
        SMDMPH_GET_RTSSEQ( (stlChar*)aLogBody, &sBfrRtsSeq );
    }
    else
    {
        sBfrRtsSeq = SMP_RTS_SEQ_NULL;
    }

    /* row header 원복 */
    stlMemcpy( sAftRowSlot, aLogBody, sAftRowHeaderSize );

    /* pad space가 runtime과 undo recover가 다를 수 있어서 재조정 필요 */

    SMDMPH_SET_PAD_SPACE( sAftRowSlot, &sPadSpace );

    sTotalRowSize = smdmphGetRowSize(sAftRowSlot);


    /* RTS 조정 */
    if( SMP_IS_VALID_RTS_SEQ(sBfrRtsSeq) != STL_TRUE )
    {
        /* sBfrRtsSeq가 NULL SEQ면 commit된 버전을 delete한 경우이므로 rts 조정이 필요하다  */
        if( SMP_IS_VALID_RTS_SEQ( sAftRtsSeq ) == STL_TRUE )
        {
            STL_TRY( smpEndUseRts( aPageHandle,
                                   sAftRowSlot,
                                   sAftRtsSeq,
                                   0,
                                   aEnv ) == STL_SUCCESS );
        }
    }
    else
    {
        /* sBfrRtsSeq가 NULL SEQ이 아니면 같은 tx에 의해 생성된 버전을 delete한 경우이므로 rts 조정이 필요없다 */
        /* 대신 delete 이후 compact 등으로 사용하는 rts가 바뀌었을 수 있으므로 undo 이전의 rts seq를 */
        /* 그대로 유지시켜야 한다                                                                    */
        if( SMP_IS_VALID_RTS_SEQ( sAftRtsSeq ) == STL_TRUE )
        {
            SMDMPH_SET_RTS_BOUND( sAftRowSlot );
            SMDMPH_SET_RTSSEQ( sAftRowSlot, &sAftRtsSeq );
        }
    }

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
    SMP_REMOVE_RECLAIMED_SPACE( SMP_FRAME(aPageHandle), sTotalRowSize + sPadSpace );
    SMP_INCREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) );

    if( SMP_GET_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) ) == 1 )
    {
        STL_TRY( smdmphUpdatePageStatus( aMiniTrans,
                                         sRelHandle,
                                         SME_GET_SEGMENT_HANDLE(sRelHandle),
                                         aPageHandle,
                                         SMP_FREENESS_INSERTABLE,
                                         0,  /* aScn */
                                         aEnv ) == STL_SUCCESS );
    }

    if( smpIsPageOverInsertLimit( SMP_FRAME(aPageHandle),
                                  SMD_GET_PCTFREE(sRelHandle) ) == STL_TRUE )
    {
        STL_TRY( smdmphUpdatePageStatus( aMiniTrans,
                                         sRelHandle,
                                         SME_GET_SEGMENT_HANDLE(sRelHandle),
                                         aPageHandle,
                                         SMP_FREENESS_UPDATEONLY,
                                         0,  /* aScn */
                                         aEnv ) == STL_SUCCESS );
    }

    smdmphValidatePadSpace( aPageHandle );
    STL_DASSERT( smpGetSegmentId( aPageHandle ) ==
                 smsGetSegmentId( SME_GET_SEGMENT_HANDLE(sRelHandle) ) );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmphUndoMemoryHeapAppend( smxmTrans * aMiniTrans,
                                      smlRid      aTargetRid,
                                      void      * aLogBody,
                                      stlInt16    aLogSize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv )
{
    stlInt16      sSlotSeq = aTargetRid.mOffset;
    stlChar       sLogBuf[SMP_PAGE_SIZE];
    stlChar     * sBfrPiece = aLogBody;
    stlChar     * sAftPiece;
    stlUInt8      sAftRtsSeq;
    stlUInt8      sBfrRtsSeq;
    stlInt16      sPadSpace;
    stlInt16      sAftPieceSize = 0;
    stlInt16      sAftPieceHdrSize;
    stlInt16      sBfrPieceSize;
    stlInt32      sLogOffset = 0;

    STL_WRITE_INT16( &sBfrPieceSize, sBfrPiece + SMDMPH_MIN_ROW_HDR_LEN );
    
    /**
     * after row를 구한다
     */
    
    STL_TRY( smpAcquire( aMiniTrans,
                         aTargetRid.mTbsId,
                         aTargetRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         aPageHandle,
                         aEnv ) == STL_SUCCESS );

    sAftPiece = smpGetNthRowItem( aPageHandle, sSlotSeq );
    if( SMDMPH_IS_RTS_BOUND( sAftPiece ) == STL_TRUE )
    {
        SMDMPH_GET_RTSSEQ( sAftPiece, &sAftRtsSeq );
    }
    else
    {
        sAftRtsSeq = SMP_RTS_SEQ_NULL;
    }
    
    sAftPieceSize = smdmphGetRowSize( sAftPiece );
    SMDMPH_GET_ROW_HDR_SIZE(sAftPiece, &sAftPieceHdrSize );
    SMDMPH_GET_PAD_SPACE( sAftPiece, &sPadSpace );
    sPadSpace += sAftPieceSize - sBfrPieceSize;
    
    /**
     * apply undo log
     */
    
    stlMemcpy( sAftPiece, sBfrPiece, SMDMPH_MIN_ROW_HDR_LEN );
    stlMemmove( sAftPiece + SMDMPH_MIN_ROW_HDR_LEN,
                sAftPiece + sAftPieceHdrSize,
                sBfrPieceSize - SMDMPH_MIN_ROW_HDR_LEN );
    
    if( SMDMPH_IS_RTS_BOUND( sBfrPiece ) == STL_TRUE )
    {
        SMDMPH_GET_RTSSEQ( sBfrPiece, &sBfrRtsSeq );
    }
    else
    {
        sBfrRtsSeq = SMP_RTS_SEQ_NULL;
    }
    
    /**
     * adjust piece header
     */
    
    SMDMPH_SET_PAD_SPACE( sAftPiece, &sPadSpace );
    
    if( SMP_IS_VALID_RTS_SEQ( sBfrRtsSeq ) != STL_TRUE )
    {
        if( SMP_IS_VALID_RTS_SEQ( sAftRtsSeq ) == STL_TRUE )
        {
            STL_TRY( smpEndUseRts( aPageHandle,
                                   sAftPiece,
                                   sAftRtsSeq,
                                   0,
                                   aEnv ) == STL_SUCCESS );
        }
    }
    else
    {
        if( SMP_IS_VALID_RTS_SEQ( sAftRtsSeq ) == STL_TRUE )
        {
            SMDMPH_SET_RTS_BOUND( sAftPiece );
            SMDMPH_SET_RTSSEQ( sAftPiece, &sAftRtsSeq );
        }
    }

    STL_WRITE_MOVE_BYTES( sLogBuf, sAftPiece, SMDMPH_MIN_ROW_HDR_LEN, sLogOffset );
    STL_WRITE_MOVE_INT16( sLogBuf, &sBfrPieceSize, sLogOffset );
    
    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_TABLE,
                           SMR_LOG_MEMORY_HEAP_UNAPPEND,
                           SMR_REDO_TARGET_PAGE,
                           sLogBuf,
                           sLogOffset,
                           aTargetRid.mTbsId,
                           aTargetRid.mPageId,
                           aTargetRid.mOffset,  /* Physiological redo를 하기위해 offset이 아닌 seq 사용 */
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    smdmphValidatePadSpace( aPageHandle );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmphUndoMemoryHeapBlockInsert( smxmTrans * aMiniTrans,
                                           smlRid      aTargetRid,
                                           void      * aLogBody,
                                           stlInt16    aLogSize,
                                           smpHandle * aPageHandle,
                                           smlEnv    * aEnv )
{
    stlInt16        sSlotSeq;
    stlChar       * sRowSlot;
    stlInt16        sRowHeaderSize;
    stlInt16        sTotalRowSize;
    stlChar         sLogBuf[SMP_PAGE_SIZE];
    stlChar       * sLogPtr;
    smxlTransId     sTransId;
    smlScn          sTransScn;
    smlRid          sSegRid = smxmGetSegRid(aMiniTrans);
    void          * sRelHandle;
    stlInt16        sRowOffsetArray[SMP_PAGE_SIZE / SMDMPH_MINIMUM_ROW_PIECE_SIZE];
    stlInt16        sBlockSize;
    stlInt32        i;
    stlInt16        sPadSpace;

    STL_TRY( smeGetRelationHandle( sSegRid, &sRelHandle, aEnv) == STL_SUCCESS );

    STL_TRY( smpAcquire( aMiniTrans,
                         aTargetRid.mTbsId,
                         aTargetRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         aPageHandle,
                         aEnv ) == STL_SUCCESS );

    sBlockSize = aLogSize / STL_SIZEOF(stlInt16);
    stlMemcpy( sRowOffsetArray, aLogBody, aLogSize );
    
    for( i = sBlockSize - 1; i >= 0; i-- )
    {
        sSlotSeq = sRowOffsetArray[i];
        
        sRowSlot = smpGetNthRowItem( aPageHandle, sSlotSeq );
        SMDMPH_GET_ROW_HDR_SIZE( sRowSlot, &sRowHeaderSize );
        sTotalRowSize =  smdmphGetRowSize(sRowSlot);

        SMDMPH_SET_DELETED(sRowSlot);

        /* Undo에 의해 작성되는 redo 로그는 redo시 Rts 할당을 받을 일이 없으므로 invalid 값을 세팅한다 */
        sTransId = SML_INVALID_TRANSID;
        sTransScn = SML_INFINITE_SCN;

        sLogPtr = sLogBuf;
        STL_WRITE_INT64( sLogPtr, &sTransId );
        sLogPtr += STL_SIZEOF(smxlTransId);
        STL_WRITE_INT64( sLogPtr, &sTransScn );
        sLogPtr += STL_SIZEOF(smlScn);
        stlMemcpy( sLogPtr, sRowSlot, sRowHeaderSize );
        sLogPtr += sRowHeaderSize;

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

        SMDMPH_GET_PAD_SPACE( sRowSlot, &sPadSpace );
        
        /* delete 되었으니 reclaimed space에 일단 추가한다 */
        SMP_ADD_RECLAIMED_SPACE( SMP_FRAME(aPageHandle), sTotalRowSize + sPadSpace );
        SMP_DECREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) );

    }

    if( SMP_GET_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) ) == 0 )
    {
        STL_TRY( smdmphUpdatePageStatus( aMiniTrans,
                                         sRelHandle,
                                         SME_GET_SEGMENT_HANDLE(sRelHandle),
                                         aPageHandle,
                                         SMP_FREENESS_FREE,
                                         smxlGetSystemScn(),
                                         aEnv ) == STL_SUCCESS );
    }
    else
    {
        if( (smpGetFreeness(aPageHandle) == SMP_FREENESS_UPDATEONLY) &&
            (smpIsPageOverFreeLimit(SMP_FRAME(aPageHandle),
                                    SMD_GET_PCTUSED(sRelHandle)) == STL_TRUE) )
        {
            STL_TRY( smdmphUpdatePageStatus( aMiniTrans,
                                             sRelHandle,
                                             SME_GET_SEGMENT_HANDLE(sRelHandle),
                                             aPageHandle,
                                             SMP_FREENESS_INSERTABLE,
                                             0,  /* aScn */
                                             aEnv ) == STL_SUCCESS );
        }
    }

    smdmphValidatePadSpace( aPageHandle );
    STL_DASSERT( smpGetSegmentId( aPageHandle ) ==
                 smsGetSegmentId( SME_GET_SEGMENT_HANDLE(sRelHandle) ) );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
