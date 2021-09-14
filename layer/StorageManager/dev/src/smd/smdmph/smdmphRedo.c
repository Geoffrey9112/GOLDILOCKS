/*******************************************************************************
 * smdmphRedo.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smdmphRedo.c 13496 2014-08-29 05:38:43Z leekmo $
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
#include <smd.h>
#include <smdmphRowDef.h>
#include <smdmphDef.h>
#include <smdmph.h>
#include <smdDef.h>

/**
 * @file smdmphRedo.c
 * @brief Storage Manager Memory Heap Table Redo Internal Routines
 */

/**
 * @addtogroup smdmphRedo
 * @{
 */

/* Memory Heap Table Redo Module */
smdTableRedoModule gSmdmphTableRedoModule = 
{
    smdmphRedoMemoryHeapInsert,
    smdmphRedoMemoryHeapUninsert,
    smdmphRedoMemoryHeapUpdate,
    smdmphRedoMemoryHeapUnupdate,
    smdmphRedoMemoryHeapMigrate,
    smdmphRedoMemoryHeapUnmigrate,
    smdmphRedoMemoryHeapDelete,
    smdmphRedoMemoryHeapUndelete,
    smdmphRedoMemoryHeapUpdateLink,
    smdmphRedoMemoryHeapInsertForUpdate,
    smdmphRedoMemoryHeapDeleteForUpdate,
    smdmphRedoMemoryHeapUpdateLogicalHdr,
    smdmphRedoMemoryHeapAppend,
    smdmphRedoMemoryHeapUnappend,
    NULL,        /* pending insert */
    smdmphRedoMemoryHeapCompaction
};


stlStatus smdmphRedoMemoryHeapInsert( smlRid    * aDataRid,
                                      void      * aData,
                                      stlUInt16   aDataSize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv )
{
    stlInt16                 sSlotSeq = aDataRid->mOffset; /* Rid의 Offset 부분을 Slot Seq로 사용 */
    stlChar                * sSrcSlot;
    stlChar                * sDstSlot;
    stlChar                * sPtr = (stlChar*)aData;
    smxlTransId              sTransId;
    smlScn                   sScn;
    stlInt32                 sRetryCnt = 0;
    stlUInt8                 sRtsSeq = SMP_RTS_SEQ_NULL;
    stlUInt8                 sRtsVerNo;
    stlInt32                 sFirstColOrd;
    stlUInt16                sRowSize;
    stlInt16                 sPadSpace;

    STL_WRITE_INT64( &sTransId, sPtr );
    sPtr += STL_SIZEOF(smxlTransId);
    STL_WRITE_INT64( &sScn, sPtr );
    sPtr += STL_SIZEOF(smlScn);
    STL_WRITE_INT32( &sFirstColOrd, sPtr );
    sPtr += STL_SIZEOF(stlInt32);
    sSrcSlot = sPtr;
    sRowSize = aDataSize - (sPtr - (stlChar*)aData);
    SMDMPH_GET_PAD_SPACE( sSrcSlot, &sPadSpace );

    while( STL_TRUE )
    {
        if( smpIsNthOffsetSlotFree( aPageHandle, sSlotSeq ) != STL_TRUE )
        {
            /* log hole 발견. page를 일단 compact하여 최대한 빈 공간을 확보하고 진행 */
            SMD_TRY_INTERNAL_ERROR( sRetryCnt != 1,
                                    aEnv,
                                    ( aPageHandle, aEnv, NULL ) );
            
            sRetryCnt++;
            STL_TRY( smdmphCompactPage( NULL, aPageHandle, aEnv ) == STL_SUCCESS );
            continue;
        }

        if( SMDMPH_IS_RTS_BOUND( sSrcSlot ) == STL_TRUE )
        {
            STL_TRY( smpAllocRts( NULL,
                                  aPageHandle,
                                  sTransId,
                                  SML_MAKE_VIEW_SCN(sScn),
                                  smdmphStabilizeRow,
                                  &sRtsSeq,
                                  aEnv ) == STL_SUCCESS );

            if( (SMP_IS_VALID_RTS_SEQ(sRtsSeq) != STL_TRUE) &&
                (sRetryCnt == 0) )
            {
                /* page를 일단 compact하여 최대한 빈 공간을 확보하고 진행 */
                sRetryCnt++;
                STL_TRY( smdmphCompactPage( NULL, aPageHandle, aEnv ) == STL_SUCCESS );
                continue;
            }
        }

        STL_TRY( smpAllocNthSlot( aPageHandle,
                                  sSlotSeq,
                                  sRowSize + sPadSpace,
                                  STL_TRUE, /* aIsForTest */
                                  &sDstSlot ) == STL_SUCCESS );

        if( sDstSlot == NULL )
        {
            /* page를 일단 compact하여 최대한 빈 공간을 확보하고 진행 */
            SMD_TRY_INTERNAL_ERROR( sRetryCnt == 0,
                                    aEnv,
                                    ( aPageHandle, aEnv, NULL ) );
            
            sRetryCnt++;
            STL_TRY( smdmphCompactPage( NULL, aPageHandle, aEnv ) == STL_SUCCESS );
            continue;
        }

        break;
    }

    /* slot 실제 할당 */
    STL_TRY( smpAllocNthSlot( aPageHandle,
                              sSlotSeq,
                              sRowSize + sPadSpace,
                              STL_FALSE, /* aIsForTest */
                              &sDstSlot ) == STL_SUCCESS );

    stlMemcpy( sDstSlot, sSrcSlot, sRowSize );


    if( SMP_IS_VALID_RTS_SEQ(sRtsSeq) == STL_TRUE )
    {
        /* RTS 사용 시작 */
        STL_TRY( smpBeginUseRts( aPageHandle,
                                 sDstSlot,
                                 sTransId,
                                 SML_MAKE_VIEW_SCN(sScn),
                                 sRtsSeq,
                                 &sRtsVerNo,
                                 aEnv ) == STL_SUCCESS );
        SMDMPH_SET_RTS_BOUND( sDstSlot );
        SMDMPH_SET_RTSSEQ( sDstSlot, &sRtsSeq );
    }
    else
    {
        SMDMPH_UNSET_RTS_BOUND( sDstSlot );
        SMDMPH_SET_SCN( sDstSlot, &sScn );
    }

    SMP_INCREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) );

    smpSetMaxViewScn( aPageHandle, sScn );
    smdmphValidateTablePage( aPageHandle, aEnv );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdmphRedoMemoryHeapUninsert( smlRid    * aDataRid,
                                        void      * aData,
                                        stlUInt16   aDataSize,
                                        smpHandle * aPageHandle,
                                        smlEnv    * aEnv )
{
    stlChar          * sLogPtr = (stlChar*)aData;
    stlChar          * sBfrRowSlot;
    stlUInt8           sBfrRtsSeq;
    stlInt16           sBfrRowHeaderSize;
    smxlTransId        sTransId;
    smlScn             sTransViewScn;
    stlInt16           sTotalRowSize;
    stlInt16           sPadSpace;

    STL_WRITE_INT64( &sTransId, sLogPtr );
    sLogPtr += STL_SIZEOF(smxlTransId);
    STL_WRITE_INT64( &sTransViewScn, sLogPtr );
    sLogPtr += STL_SIZEOF(smlScn);

    SMD_TRY_INTERNAL_ERROR( (sTransId == SML_INVALID_TRANSID) &&
                            (sTransViewScn == SML_INFINITE_SCN) &&
                            (SMDMPH_IS_DELETED( sLogPtr ) == STL_TRUE),
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );

    /* before row 정보 수집 */
    sBfrRowSlot = smpGetNthRowItem( aPageHandle, aDataRid->mOffset );
    SMDMPH_GET_ROW_HDR_SIZE( sBfrRowSlot, &sBfrRowHeaderSize );
    sTotalRowSize =  smdmphGetRowSize(sBfrRowSlot);

    if( SMDMPH_IS_RTS_BOUND( sBfrRowSlot ) == STL_TRUE )
    {
        SMDMPH_GET_RTSSEQ( sBfrRowSlot, &sBfrRtsSeq );
    }
    else
    {
        sBfrRtsSeq = SMP_RTS_SEQ_NULL;
    }

    /* undo operation에 의해 작성된 로그. rts를 재 할당 할 필요가 없다 */

    /* row header 복사 */
    stlMemcpy( sBfrRowSlot,
               (stlChar*)aData + STL_SIZEOF(smxlTransId) + STL_SIZEOF(smlScn),
               sBfrRowHeaderSize );

    /* RTS 조정 */
    if( SMP_IS_VALID_RTS_SEQ(sBfrRtsSeq) == STL_TRUE )
    {
        SMDMPH_SET_RTS_BOUND( sBfrRowSlot );
        SMDMPH_SET_RTSSEQ( sBfrRowSlot, &sBfrRtsSeq );
    }

    SMDMPH_GET_PAD_SPACE( sBfrRowSlot, &sPadSpace );
    
    /* delete 되었으니 reclaimed space에 일단 추가한다 */
    SMP_ADD_RECLAIMED_SPACE( SMP_FRAME(aPageHandle), sTotalRowSize + sPadSpace );
    SMP_DECREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) );

    smdmphValidateTablePage( aPageHandle, aEnv );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdmphRedoMemoryHeapUpdate( smlRid    * aDataRid,
                                      void      * aData,
                                      stlUInt16   aDataSize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv )
{
    knlRegionMark       sMemMark;
    knlValueBlockList * sUpdateCols;
    stlChar           * sLogPtr = (stlChar*)aData;
    stlInt32            sState = 0;
    stlInt32            sRetryCnt = 0;
    stlChar           * sAftRowSlot;
    stlUInt8            sAftRtsSeq = SMP_RTS_SEQ_NULL;
    stlUInt8            sRtsVerNo;
    stlInt16            sTotalColCnt;
    stlInt16            sUpdateColCnt;
    stlChar           * sBfrRowSlot;
    stlChar           * sBfrRowPtr;
    stlUInt8            sBfrRtsSeq;
    stlInt16            sBfrRowHeaderSize;
    stlInt16            sBfrColListSize;
    stlInt16            sBfrRowPieceSize;
    stlBool             sIsColSizeDiff;
    stlInt64            sRowSizeDiff;
    stlInt16            sPadSpace;
    smxlTransId         sTransId;
    smlScn              sTransViewScn;
    stlChar             sRowBuf[SMP_PAGE_SIZE];
    stlInt32            sFirstColOrd;
    stlInt16            sUnusedSpace;
    stlInt64            sNeededSpace = 0;
    stlInt16            sNewPadSpace;

    STL_WRITE_INT64( &sTransId, sLogPtr );
    sLogPtr += STL_SIZEOF(smxlTransId);
    STL_WRITE_INT64( &sTransViewScn, sLogPtr );
    sLogPtr += STL_SIZEOF(smlScn);

    SMD_TRY_INTERNAL_ERROR( (sTransId != SML_INVALID_TRANSID) ||
                            (sTransViewScn != SML_INFINITE_SCN),
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );

    STL_TRY( smdmphAnalyzeUpdateLogRec( sLogPtr,
                                        &sMemMark,
                                        &sAftRtsSeq,
                                        &sUpdateColCnt,
                                        &sFirstColOrd,
                                        &sUpdateCols,
                                        aEnv ) == STL_SUCCESS );
    sState = 1;

    /* before row 정보 수집 */
    sBfrRowSlot = smpGetNthRowItem( aPageHandle, aDataRid->mOffset );

    SMDMPH_GET_COLUMN_CNT( sBfrRowSlot, &sTotalColCnt );
    SMDMPH_GET_ROW_HDR_SIZE( sBfrRowSlot, &sBfrRowHeaderSize );
    sBfrRowPtr  = sBfrRowSlot + sBfrRowHeaderSize;

    STL_TRY( smdmphGetUpdatePieceSize( sBfrRowPtr,
                                       sTotalColCnt,
                                       sFirstColOrd,
                                       sUpdateCols,
                                       NULL, /* aBeforeCols */
                                       0,    /* block dix */
                                       NULL,
                                       NULL,
                                       &sBfrColListSize,
                                       &sIsColSizeDiff,
                                       &sRowSizeDiff,
                                       &sUpdateColCnt ) == STL_SUCCESS );

    sBfrRowPieceSize = sBfrRowHeaderSize + sBfrColListSize;
    SMDMPH_GET_PAD_SPACE( sBfrRowSlot, &sPadSpace );

    while( STL_TRUE )
    {
        if( sRetryCnt > 0 )
        {
            /* compact 발생. before row slot 재설정 필요 */
            sBfrRowSlot = smpGetNthRowItem( aPageHandle, aDataRid->mOffset );
            sBfrRowPtr  = sBfrRowSlot + sBfrRowHeaderSize;
            SMDMPH_GET_PAD_SPACE( sBfrRowSlot, &sPadSpace );
        }

        if( SMDMPH_IS_RTS_BOUND( sLogPtr ) == STL_TRUE )
        {
            STL_TRY( smpAllocRts( NULL,
                                  aPageHandle,
                                  sTransId,
                                  SML_MAKE_VIEW_SCN(sTransViewScn),
                                  smdmphStabilizeRow,
                                  &sAftRtsSeq,
                                  aEnv ) == STL_SUCCESS );

            if( (SMP_IS_VALID_RTS_SEQ(sAftRtsSeq) != STL_TRUE) &&
                (sRetryCnt == 0) )
            {
                /* 처음으로 공간부족 발생. page를 일단 compact하여 최대한 빈 공간을 확보하고 진행 */
                sRetryCnt++;
                STL_TRY( smdmphCompactPage( NULL, aPageHandle, aEnv ) == STL_SUCCESS );
                continue;
            }
        }

        if( sPadSpace < sRowSizeDiff )
        {
            /* 재할당 테스트 */
            sUnusedSpace = smpGetUnusedSpace( SMP_FRAME(aPageHandle) );
            sNeededSpace = sBfrRowPieceSize + sRowSizeDiff;
            if( sUnusedSpace >= sNeededSpace + STL_SIZEOF(smpOffsetSlot) )
            {
                STL_TRY( smpReallocNthSlot( aPageHandle,
                                            aDataRid->mOffset,
                                            sNeededSpace,
                                            STL_TRUE, /* aIsForTest */
                                            &sAftRowSlot ) == STL_SUCCESS );
            }
            else if( (sUnusedSpace >= sRowSizeDiff - sPadSpace) &&
                     (sRetryCnt >= 1) )
            {
                /* case 3-1 : 페이지내에 row diff 만큼의 공간이 있으면 slot의 pad 영역을 증가시킨다 */
                STL_TRY( smdmphExtendRowSlot( NULL,
                                              aPageHandle,
                                              aDataRid->mOffset,
                                              sRowSizeDiff - sPadSpace,
                                              aEnv ) == STL_SUCCESS );
                sAftRowSlot = smpGetNthRowItem( aPageHandle, aDataRid->mOffset );
                sBfrRowSlot = sAftRowSlot;
                sBfrRowPtr  = sAftRowSlot + sBfrRowHeaderSize;

                /* extend 연산으로 rts seq가 변했을 수 있으므로 다시 할당받는다 */
                if( SMP_IS_VALID_RTS_SEQ(sAftRtsSeq) == STL_TRUE )
                {
                    STL_TRY( smpAllocRts( NULL,
                                          aPageHandle,
                                          sTransId,
                                          SML_MAKE_VIEW_SCN(sTransViewScn),
                                          smdmphStabilizeRow,
                                          &sAftRtsSeq,
                                          aEnv ) == STL_SUCCESS );

                    SMD_TRY_INTERNAL_ERROR( SMP_IS_VALID_RTS_SEQ(sAftRtsSeq) == STL_TRUE,
                                            aEnv,
                                            ( aPageHandle, aEnv, NULL ) );
                }
            }
            else
            {
                sAftRowSlot = NULL;
            }
        }
        else
        {
            sAftRowSlot = sBfrRowSlot;
        }

        if( sAftRowSlot == NULL )
        {
            /* page를 일단 compact하여 최대한 빈 공간을 확보하고 진행 */
            SMD_TRY_INTERNAL_ERROR( sRetryCnt < 2,
                                    aEnv,
                                    ( aPageHandle, aEnv, NULL ) );
            
            sRetryCnt++;
            STL_TRY( smdmphCompactPage( NULL, aPageHandle, aEnv ) == STL_SUCCESS );
            continue;
        }

        break;
    }

    if( SMDMPH_IS_RTS_BOUND( sBfrRowSlot ) == STL_TRUE )
    {
        SMDMPH_GET_RTSSEQ( sBfrRowSlot, &sBfrRtsSeq );
    }
    else
    {
        sBfrRtsSeq = SMP_RTS_SEQ_NULL;
    }

    if( SMP_IS_VALID_RTS_SEQ(sAftRtsSeq) == STL_TRUE )
    {
        /* RTS 조정 */
        STL_TRY( smpBeginUseRts( aPageHandle,
                                 sAftRowSlot,
                                 sTransId,
                                 SML_MAKE_VIEW_SCN(sTransViewScn),
                                 sAftRtsSeq,
                                 &sRtsVerNo,
                                 aEnv ) == STL_SUCCESS );
    }

    if( sBfrRowSlot == sAftRowSlot )
    {
        SMDMPH_GET_PAD_SPACE( sBfrRowSlot, &sPadSpace );
        /* inplace update */
        stlMemcpy( sRowBuf, sBfrRowSlot, sBfrRowPieceSize );
        /* row header 복사 */
        stlMemcpy( sAftRowSlot,
                   (stlChar*)aData + STL_SIZEOF(smxlTransId) + STL_SIZEOF(smlScn),
                   sBfrRowHeaderSize );

        /**
         * ONLINE 연산시에는 REALLOC 형태가 아니여서 PADDING SPACE가 설정되어 있을수도 있다.
         * PADDING SPACE의 재조정이 필요하다.
         */
        SMD_TRY_INTERNAL_ERROR( sPadSpace >= sRowSizeDiff,
                                aEnv,
                                ( aPageHandle, aEnv, NULL ) );
        
        sNewPadSpace = sPadSpace - sRowSizeDiff;
        SMDMPH_SET_PAD_SPACE( sAftRowSlot, &sNewPadSpace );
        
        if( sUpdateColCnt > 0 )
        {
            /* row column 복사 */
            STL_TRY( smdmphCopyAndUpdateColumns( sRowBuf,
                                                 sAftRowSlot,
                                                 sUpdateCols,
                                                 0,  /* block idx */
                                                 sBfrRowPieceSize,
                                                 sTotalColCnt,
                                                 sFirstColOrd,
                                                 NULL,
                                                 NULL,
                                                 sIsColSizeDiff,
                                                 aEnv ) == STL_SUCCESS );
        }
    }
    else
    {
        /* 실제로 slot 재할당 */
        STL_TRY( smpReallocNthSlot( aPageHandle,
                                    aDataRid->mOffset,
                                    sNeededSpace,
                                    STL_FALSE, /* aIsForTest */
                                    &sAftRowSlot ) == STL_SUCCESS );
        /* delete 되었으니 reclaimed space에 일단 추가한다 */
        SMP_ADD_RECLAIMED_SPACE( SMP_FRAME(aPageHandle), sBfrRowPieceSize + sPadSpace );
        
        /* outplace udpate */
        /* row header 복사 */
        stlMemcpy( sAftRowSlot,
                   sLogPtr,
                   sBfrRowHeaderSize );

        /**
         * ONLINE 연산시에는 REALLOC 형태가 아니여서 PADDING SPACE가 설정되어 있을수도 있다.
         * - REALLOC은 항상 PADDING SPACE가 0 이다.
         */
        sNewPadSpace = 0;
        SMDMPH_SET_PAD_SPACE( sAftRowSlot, &sNewPadSpace );

        /* row column 복사 */
        STL_TRY( smdmphCopyAndUpdateColumns( sBfrRowSlot,
                                             sAftRowSlot,
                                             sUpdateCols,
                                             0,   /* block idx */
                                             sBfrRowPieceSize,
                                             sTotalColCnt,
                                             sFirstColOrd,
                                             NULL,
                                             NULL,
                                             STL_TRUE,
                                             aEnv ) == STL_SUCCESS );
    }

    /* RTS 조정 */
    if( SMP_IS_VALID_RTS_SEQ(sAftRtsSeq) == STL_TRUE )
    {
        SMDMPH_SET_RTS_BOUND( sAftRowSlot );
        SMDMPH_SET_RTSSEQ( sAftRowSlot, &sAftRtsSeq );
    }

    if( SMP_IS_VALID_RTS_SEQ(sBfrRtsSeq) == STL_TRUE )
    {
        STL_TRY( smpEndUseRts( aPageHandle,
                               sBfrRowSlot,
                               sBfrRtsSeq,
                               0,
                               aEnv ) == STL_SUCCESS );
    }

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       (knlEnv*)aEnv ) == STL_SUCCESS );

    smpSetMaxViewScn( aPageHandle, sTransViewScn );
    smdmphValidateTablePage( aPageHandle, aEnv );

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


stlStatus smdmphRedoMemoryHeapUnupdate( smlRid    * aDataRid,
                                        void      * aData,
                                        stlUInt16   aDataSize,
                                        smpHandle * aPageHandle,
                                        smlEnv    * aEnv )
{
    knlRegionMark       sMemMark;
    knlValueBlockList * sUpdateCols;
    stlChar           * sLogPtr = (stlChar*)aData;
    stlInt32            sState = 0;
    stlInt16            sTotalColCnt;
    stlInt16            sUpdateColCnt;
    stlChar           * sRowSlot;
    stlChar           * sRowPtr;
    stlUInt8            sBfrRtsSeq;
    stlUInt8            sAftRtsSeq;
    stlInt16            sBfrRowHeaderSize;
    stlInt16            sBfrColListSize;
    stlInt16            sBfrRowPieceSize;
    stlBool             sIsColSizeDiff;
    stlInt64            sRowSizeDiff;
    stlInt16            sPadSpace;
    smxlTransId         sTransId;
    smlScn              sTransViewScn;
    stlChar             sRowBuf[SMP_PAGE_SIZE];
    stlInt32            sFirstColOrd;
    stlInt16            sNewPadSpace;

    STL_WRITE_INT64( &sTransId, sLogPtr );
    sLogPtr += STL_SIZEOF(smxlTransId);
    STL_WRITE_INT64( &sTransViewScn, sLogPtr );
    sLogPtr += STL_SIZEOF(smlScn);

    SMD_TRY_INTERNAL_ERROR( (sTransId == SML_INVALID_TRANSID) &&
                            (sTransViewScn == SML_INFINITE_SCN),
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );

    STL_TRY( smdmphAnalyzeUpdateLogRec( sLogPtr,
                                        &sMemMark,
                                        &sAftRtsSeq,
                                        &sUpdateColCnt,
                                        &sFirstColOrd,
                                        &sUpdateCols,
                                        aEnv ) == STL_SUCCESS );
    sState = 1;

    /* before row 정보 수집 */
    sRowSlot = smpGetNthRowItem( aPageHandle, aDataRid->mOffset );

    SMDMPH_GET_COLUMN_CNT( sRowSlot, &sTotalColCnt );
    SMDMPH_GET_ROW_HDR_SIZE( sRowSlot, &sBfrRowHeaderSize );
    sRowPtr  = sRowSlot + sBfrRowHeaderSize;

    STL_TRY( smdmphGetUpdatePieceSize( sRowPtr,
                                       sTotalColCnt,
                                       sFirstColOrd,
                                       sUpdateCols,
                                       NULL, /* aBeforeCols */
                                       0,    /* block dix */
                                       NULL,
                                       NULL,
                                       &sBfrColListSize,
                                       &sIsColSizeDiff,
                                       &sRowSizeDiff,
                                       &sUpdateColCnt ) == STL_SUCCESS );

    sBfrRowPieceSize = sBfrRowHeaderSize + sBfrColListSize;
    SMDMPH_GET_PAD_SPACE( sRowSlot, &sPadSpace );
    
    SMD_TRY_INTERNAL_ERROR( sBfrRowPieceSize + sPadSpace >= sRowSizeDiff,
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );

    if( SMDMPH_IS_RTS_BOUND( sRowSlot ) == STL_TRUE )
    {
        SMDMPH_GET_RTSSEQ( sRowSlot, &sBfrRtsSeq );
    }
    else
    {
        sBfrRtsSeq = SMP_RTS_SEQ_NULL;
    }

    /* inplace update */
    stlMemcpy( sRowBuf, sRowSlot, sBfrRowPieceSize );
    /* row header 복사 */
    stlMemcpy( sRowSlot,
               (stlChar*)aData + STL_SIZEOF(smxlTransId) + STL_SIZEOF(smlScn),
               sBfrRowHeaderSize );

    /**
     * ONLINE 연산시에는 REALLOC 형태가 아니여서 PADDING SPACE가 설정되어 있을수도 있다.
     * PADDING SPACE의 재조정이 필요하다.
     */
    SMD_TRY_INTERNAL_ERROR( sPadSpace >= sRowSizeDiff,
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );
    
    sNewPadSpace = sPadSpace - sRowSizeDiff;
    SMDMPH_SET_PAD_SPACE( sRowSlot, &sNewPadSpace );

    if( sUpdateColCnt > 0 )
    {
        /* row column 복사 */
        STL_TRY( smdmphCopyAndUpdateColumns( sRowBuf,
                                             sRowSlot,
                                             sUpdateCols,
                                             0,  /* block idx */
                                             sBfrRowPieceSize,
                                             sTotalColCnt,
                                             sFirstColOrd,
                                             NULL,
                                             NULL,
                                             sIsColSizeDiff,
                                             aEnv ) == STL_SUCCESS );
    }

    /* RTS 조정 */
    if( SMP_IS_VALID_RTS_SEQ( sBfrRtsSeq ) == STL_TRUE )
    {
        if( SMP_IS_VALID_RTS_SEQ( sAftRtsSeq ) == STL_TRUE )
        {
            SMDMPH_SET_RTS_BOUND( sRowSlot );
            SMDMPH_SET_RTSSEQ( sRowSlot, &sBfrRtsSeq );
        }
        else
        {
            STL_TRY( smpEndUseRts( aPageHandle,
                                   sRowSlot,
                                   sBfrRtsSeq,
                                   0,
                                   aEnv ) == STL_SUCCESS );
        }
    }

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       (knlEnv*)aEnv ) == STL_SUCCESS );

    smdmphValidateTablePage( aPageHandle, aEnv );

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



stlStatus smdmphRedoMemoryHeapMigrate( smlRid    * aDataRid,
                                       void      * aData,
                                       stlUInt16   aDataSize,
                                       smpHandle * aPageHandle,
                                       smlEnv    * aEnv )
{
    knlRegionMark       sMemMark;
    knlValueBlockList * sUpdateCols;
    stlChar           * sLogPtr = (stlChar*)aData;
    stlInt32            sState = 0;
    stlInt32            sRetryCnt = 0;
    stlUInt8            sAftRtsSeq = SMP_RTS_SEQ_NULL;
    stlUInt8            sRtsVerNo;
    stlInt16            sUpdateColCnt;
    stlChar           * sRowSlot;
    stlUInt8            sBfrRtsSeq;
    stlInt16            sRowHeaderSize;
    stlInt16            sRowPieceSize;
    stlInt16            sPadSpace;
    smxlTransId         sTransId;
    smlScn              sTransViewScn;
    stlInt32            sFirstColOrd;

    STL_WRITE_INT64( &sTransId, sLogPtr );
    sLogPtr += STL_SIZEOF(smxlTransId);
    STL_WRITE_INT64( &sTransViewScn, sLogPtr );
    sLogPtr += STL_SIZEOF(smlScn);

    SMD_TRY_INTERNAL_ERROR( (sTransId != SML_INVALID_TRANSID) &&
                            (sTransViewScn != SML_INFINITE_SCN),
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );

    STL_TRY( smdmphAnalyzeUpdateLogRec( sLogPtr,
                                        &sMemMark,
                                        &sAftRtsSeq,
                                        &sUpdateColCnt,
                                        &sFirstColOrd,
                                        &sUpdateCols,
                                        aEnv ) == STL_SUCCESS );
    sState = 1;
    
    SMD_TRY_INTERNAL_ERROR( (sFirstColOrd == 0) && (sUpdateColCnt == 0),
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );

    /* before row 정보 수집 */
    sRowSlot = smpGetNthRowItem( aPageHandle, aDataRid->mOffset );

    while( STL_TRUE )
    {
        if( sRetryCnt > 0 )
        {
            /* compact 발생. before row slot 재설정 필요 */
        }

        if( SMDMPH_IS_RTS_BOUND( sLogPtr ) == STL_TRUE )
        {
            STL_TRY( smpAllocRts( NULL,
                                  aPageHandle,
                                  sTransId,
                                  SML_MAKE_VIEW_SCN(sTransViewScn),
                                  smdmphStabilizeRow,
                                  &sAftRtsSeq,
                                  aEnv ) == STL_SUCCESS );

            if( (SMP_IS_VALID_RTS_SEQ(sAftRtsSeq) != STL_TRUE) &&
                (sRetryCnt == 0) )
            {
                /* 처음으로 공간부족 발생. page를 일단 compact하여 최대한 빈 공간을 확보하고 진행 */
                sRetryCnt++;
                STL_TRY( smdmphCompactPage( NULL, aPageHandle, aEnv ) == STL_SUCCESS );
                continue;
            }
        }

        break;
    }

    sRowPieceSize = smdmphGetRowSize( sRowSlot );
    SMDMPH_GET_PAD_SPACE( sRowSlot, &sPadSpace );
    
    SMD_TRY_INTERNAL_ERROR( sRowPieceSize + sPadSpace >= SMDMPH_MINIMUM_ROW_PIECE_SIZE,
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );

    if( SMDMPH_IS_RTS_BOUND( sRowSlot ) == STL_TRUE )
    {
        SMDMPH_GET_RTSSEQ( sRowSlot, &sBfrRtsSeq );
    }
    else
    {
        sBfrRtsSeq = SMP_RTS_SEQ_NULL;
    }

    if( SMP_IS_VALID_RTS_SEQ(sAftRtsSeq) == STL_TRUE )
    {
        /* RTS 조정 */
        STL_TRY( smpBeginUseRts( aPageHandle,
                                 sRowSlot,
                                 sTransId,
                                 SML_MAKE_VIEW_SCN(sTransViewScn),
                                 sAftRtsSeq,
                                 &sRtsVerNo,
                                 aEnv ) == STL_SUCCESS );
    }

    SMDMPH_GET_ROW_HDR_SIZE( sLogPtr, &sRowHeaderSize );
    stlMemcpy( sRowSlot, sLogPtr, sRowHeaderSize );

    /**
     * PADDING SPACE가 runtime과 다를 수 있으므로 재조정이 필요하다.
     */
    sPadSpace = sRowPieceSize + sPadSpace - sRowHeaderSize;
    SMDMPH_SET_PAD_SPACE( sRowSlot, &sPadSpace );

    /* RTS 조정 */
    if( SMP_IS_VALID_RTS_SEQ(sAftRtsSeq) == STL_TRUE )
    {
        SMDMPH_SET_RTS_BOUND( sRowSlot );
        SMDMPH_SET_RTSSEQ( sRowSlot, &sAftRtsSeq );
    }

    if( SMP_IS_VALID_RTS_SEQ(sBfrRtsSeq) == STL_TRUE )
    {
        STL_TRY( smpEndUseRts( aPageHandle,
                               sRowSlot,
                               sBfrRtsSeq,
                               0,
                               aEnv ) == STL_SUCCESS );
    }

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       (knlEnv*)aEnv ) == STL_SUCCESS );

    smpSetMaxViewScn( aPageHandle, sTransViewScn );
    smdmphValidateTablePage( aPageHandle, aEnv );

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


stlStatus smdmphRedoMemoryHeapUpdateLink( smlRid    * aDataRid,
                                          void      * aData,
                                          stlUInt16   aDataSize,
                                          smpHandle * aPageHandle,
                                          smlEnv    * aEnv )
{
    stlChar           * sRowSlot = smpGetNthRowItem( aPageHandle, aDataRid->mOffset );
    stlChar           * sAftRowSlot;
    smxlTransId         sTransId;
    smlScn              sTransViewScn;
    smpRts            * sRts;
    stlUInt8            sOrgRtsSeq;
    stlUInt8            sRtsSeq = SMP_RTS_SEQ_NULL;
    stlUInt8            sRtsVerNo;
    smlRid              sNextRid;
    stlChar           * sLogPtr;
    stlInt16            sRetryCnt = 0;
    stlBool             sIsMyTrans;
    stlUInt16           sSlotHdrSize;
    smlScn              sScn;
    smxlTransId         sRowTransId;
    stlInt16            sPadSpace;

    SMD_TRY_INTERNAL_ERROR( SMDMPH_HAS_NEXT_LINK( sRowSlot ) == STL_TRUE,
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );

    sLogPtr = (stlChar*)aData;
    STL_WRITE_INT64( &sTransId, sLogPtr );
    sLogPtr += STL_SIZEOF(smxlTransId);
    STL_WRITE_INT64( &sTransViewScn, sLogPtr );
    sLogPtr += STL_SIZEOF(smlScn);
    sAftRowSlot = sLogPtr;

    if( (sTransId == SML_INVALID_TRANSID) && (sTransViewScn == SML_INFINITE_SCN) )
    {
        /* Undo로 인한 redo log */
        if( SMDMPH_IS_RTS_BOUND( sRowSlot ) == STL_TRUE )
        {
            SMDMPH_GET_RTSSEQ( sRowSlot, &sOrgRtsSeq );
        }
        else
        {
            sOrgRtsSeq = SMP_RTS_SEQ_NULL;
        }

        SMDMPH_GET_ROW_HDR_SIZE( sAftRowSlot, &sSlotHdrSize );
        stlMemcpy( sRowSlot, sAftRowSlot, sSlotHdrSize );

        if( SMP_IS_VALID_RTS_SEQ( sOrgRtsSeq ) == STL_TRUE )
        {
            if( SMDMPH_IS_RTS_BOUND( sAftRowSlot ) == STL_TRUE )
            {
                /* my tx */
                SMDMPH_SET_RTS_BOUND( sRowSlot );
                SMDMPH_SET_RTSSEQ( sRowSlot, &sOrgRtsSeq );
            }
            else
            {
                STL_TRY( smpEndUseRts( aPageHandle,
                                       sRowSlot,
                                       sOrgRtsSeq,
                                       0,
                                       aEnv ) == STL_SUCCESS );
            }
        }
    }
    else
    {
        if( SMDMPH_IS_RTS_BOUND( sRowSlot ) == STL_TRUE )
        {
            SMDMPH_GET_RTSSEQ( sRowSlot, &sOrgRtsSeq );
            sRts = SMP_GET_NTH_RTS(SMP_FRAME(aPageHandle), sRtsSeq);
            if( (SMP_RTS_IS_NOT_STAMPED( sRts ) == STL_TRUE) &&
                (sRts->mTransId == sTransId) &&
                (SML_MAKE_REAL_SCN(sRts->mScn )) == sTransViewScn )
            {
                sIsMyTrans = STL_TRUE;
            }
            else
            {
                sIsMyTrans = STL_FALSE;
            }
        }
        else
        {
            SMDMPH_GET_SCN( sRowSlot, &sScn );
            SMDMPH_GET_TRANS_ID( sRowSlot, &sRowTransId );
            if( (sRowTransId == sTransId) && (SML_MAKE_REAL_SCN(sScn) == sTransViewScn) )
            {
                sIsMyTrans = STL_TRUE;
            }
            else
            {
                sIsMyTrans = STL_FALSE;
            }
        }

        if( sIsMyTrans != STL_TRUE )
        {
            /* DS  mode가 아니면 */
            if( SMXL_IS_UNDO_BOUND_TRANS( sTransId ) == STL_TRUE )
            {
                if( SMDMPH_IS_RTS_BOUND( sAftRowSlot ) == STL_TRUE )
                {
                    sRetryCnt = 0;
                    while( STL_TRUE )
                    {
                        STL_TRY( smpAllocRts( NULL,
                                              aPageHandle,
                                              sTransId,
                                              SML_MAKE_VIEW_SCN(sTransViewScn),
                                              smdmphStabilizeRow,
                                              &sRtsSeq,
                                              aEnv ) == STL_SUCCESS );

                        if( (SMP_IS_VALID_RTS_SEQ(sRtsSeq) != STL_TRUE) && (sRetryCnt == 0) )
                        {
                            /* 처음으로 공간부족 발생. page를 일단 compact하여 최대한 빈 공간을 확보하고 진행 */
                            sRetryCnt++;
                            STL_TRY( smdmphCompactPage( NULL, aPageHandle, aEnv ) == STL_SUCCESS );
                            /* compact로 source row의 위치가 변경되었을 수 있으므로 다시 구한다 */
                            sRowSlot = smpGetNthRowItem( aPageHandle, aDataRid->mOffset );
                            continue;
                        }

                        break;
                    }
                }
            }

            if( SMDMPH_IS_RTS_BOUND( sRowSlot ) == STL_TRUE )
            {
                SMDMPH_GET_RTSSEQ( sRowSlot, &sOrgRtsSeq );
            }
            else
            {
                sOrgRtsSeq = SMP_RTS_SEQ_NULL;
            }

            SMDMPH_GET_PAD_SPACE( sRowSlot, &sPadSpace );
            SMDMPH_GET_ROW_HDR_SIZE( sAftRowSlot, &sSlotHdrSize );
            stlMemcpy( sRowSlot, sAftRowSlot, sSlotHdrSize );

            /* PadSpace가 runtime과 다를 수 있으므로 되돌린다 */
            SMDMPH_SET_PAD_SPACE( sRowSlot, &sPadSpace );

            if( SMP_IS_VALID_RTS_SEQ(sRtsSeq) == STL_TRUE )
            {    
                /* RTS 조정 */
                STL_TRY( smpBeginUseRts( aPageHandle,
                                         sRowSlot,
                                         sTransId,
                                         SML_MAKE_VIEW_SCN(sTransViewScn),
                                         sRtsSeq,
                                         &sRtsVerNo,
                                         aEnv ) == STL_SUCCESS );
                SMDMPH_SET_RTS_BOUND( sRowSlot );
                SMDMPH_SET_RTSSEQ( sRowSlot, &sRtsSeq );
            }
            else
            {
                if( SMXL_IS_UNDO_BOUND_TRANS( sTransId ) == STL_TRUE )
                {
                    sScn = SML_MAKE_VIEW_SCN(sTransViewScn);
                    SMDMPH_UNSET_RTS_BOUND( sRowSlot );
                    SMDMPH_SET_SCN( sRowSlot, sScn );
                }
                else
                {
                    /* DS mode */
                    sScn = 0;
                    SMDMPH_UNSET_RTS_BOUND( sRowSlot );
                    SMDMPH_SET_SCN( sRowSlot, &sScn );
                }
            }

            if( SMP_IS_VALID_RTS_SEQ(sOrgRtsSeq) == STL_TRUE )
            {
                STL_TRY( smpEndUseRts( aPageHandle,
                                       sRowSlot,
                                       sOrgRtsSeq,
                                       0,
                                       aEnv ) == STL_SUCCESS );
            }
        }
        else
        {
            SMDMPH_GET_LINK_PID( sAftRowSlot, &sNextRid.mPageId);
            SMDMPH_GET_LINK_OFFSET( sAftRowSlot, &sNextRid.mOffset);
            SMDMPH_SET_HAS_NEXT_LINK( sRowSlot );
            SMDMPH_SET_LINK_PID( sRowSlot, &sNextRid.mPageId );
            SMDMPH_SET_LINK_OFFSET( sRowSlot, &sNextRid.mOffset );
        }
    }

    smpSetMaxViewScn( aPageHandle, sTransViewScn );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdmphRedoMemoryHeapUnmigrate( smlRid    * aDataRid,
                                         void      * aData,
                                         stlUInt16   aDataSize,
                                         smpHandle * aPageHandle,
                                         smlEnv    * aEnv )
{
    stlChar  * sRowSlot = smpGetNthRowItem( aPageHandle, aDataRid->mOffset );
    stlInt16   sBfrRowSize;
    stlInt16   sAftRowSize;
    stlInt16   sPadSpace;
    stlUInt8   sBfrRtsSeq;

    sBfrRowSize = smdmphGetRowSize(sRowSlot);
    SMDMPH_GET_PAD_SPACE( sRowSlot, &sPadSpace );
    if( SMDMPH_IS_RTS_BOUND( sRowSlot ) == STL_TRUE )
    {
        SMDMPH_GET_RTSSEQ( sRowSlot, &sBfrRtsSeq );
    }
    else
    {
        sBfrRtsSeq = SMP_RTS_SEQ_NULL;
    }

    sAftRowSize = smdmphGetRowSize(aData);

    SMD_TRY_INTERNAL_ERROR( sBfrRowSize + sPadSpace >= sAftRowSize,
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );

    stlMemcpy( sRowSlot, aData, sAftRowSize );

    sPadSpace = sPadSpace + sBfrRowSize - sAftRowSize;
    SMDMPH_SET_PAD_SPACE( sRowSlot, &sPadSpace );

    if( SMP_IS_VALID_RTS_SEQ( sBfrRtsSeq ) == STL_TRUE )
    {
        if( SMDMPH_IS_RTS_BOUND( sRowSlot ) == STL_TRUE )
        {
            SMDMPH_SET_RTS_BOUND( sRowSlot );
            SMDMPH_SET_RTSSEQ( sRowSlot, &sBfrRtsSeq );
        }
        else
        {
            STL_TRY( smpEndUseRts( aPageHandle,
                                   sRowSlot,
                                   sBfrRtsSeq,
                                   0,
                                   aEnv ) == STL_SUCCESS );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdmphRedoMemoryHeapDelete( smlRid    * aDataRid,
                                      void      * aData,
                                      stlUInt16   aDataSize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv )
{
    stlChar          * sLogPtr = (stlChar*)aData;
    stlInt32           sRetryCnt = 0;
    stlUInt8           sAftRtsSeq = SMP_RTS_SEQ_NULL;
    stlChar          * sBfrRowSlot;
    stlUInt8           sBfrRtsSeq;
    stlUInt8           sRtsVerNo;
    stlInt16           sBfrRowHeaderSize;
    smxlTransId        sTransId;
    smlScn             sTransViewScn;
    stlInt16           sTotalRowSize;
    stlInt16           sPadSpace;

    STL_WRITE_INT64( &sTransId, sLogPtr );
    sLogPtr += STL_SIZEOF(smxlTransId);
    STL_WRITE_INT64( &sTransViewScn, sLogPtr );
    sLogPtr += STL_SIZEOF(smlScn);

    SMD_TRY_INTERNAL_ERROR( (sTransId != SML_INVALID_TRANSID) ||
                            (sTransViewScn != SML_INFINITE_SCN),
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );

    /* before row 정보 수집 */
    sBfrRowSlot = smpGetNthRowItem( aPageHandle, aDataRid->mOffset );
    SMDMPH_GET_ROW_HDR_SIZE( sBfrRowSlot, &sBfrRowHeaderSize );
    sTotalRowSize =  smdmphGetRowSize(sBfrRowSlot);

    while( STL_TRUE )
    {
        if( sRetryCnt > 0 )
        {
            /* compact 발생. before row slot 재설정 필요 */
            sBfrRowSlot = smpGetNthRowItem( aPageHandle, aDataRid->mOffset );
        }

        if( SMDMPH_IS_RTS_BOUND( sLogPtr ) == STL_TRUE )
        {
            STL_TRY( smpAllocRts( NULL,
                                  aPageHandle,
                                  sTransId,
                                  SML_MAKE_VIEW_SCN(sTransViewScn),
                                  smdmphStabilizeRow,
                                  &sAftRtsSeq,
                                  aEnv ) == STL_SUCCESS );

            if( SMP_IS_VALID_RTS_SEQ(sAftRtsSeq) != STL_TRUE )
            {
                /* 처음으로 공간부족 발생. page를 일단 compact하여 최대한 빈 공간을 확보하고 진행 */
                if( sRetryCnt == 0 )
                {
                    sRetryCnt++;
                    STL_TRY( smdmphCompactPage( NULL, aPageHandle, aEnv ) == STL_SUCCESS );
                    continue;
                }
            }
        }

        break;
    }

    if( SMDMPH_IS_RTS_BOUND( sBfrRowSlot ) == STL_TRUE )
    {
        SMDMPH_GET_RTSSEQ( sBfrRowSlot, &sBfrRtsSeq );
    }
    else
    {
        sBfrRtsSeq = SMP_RTS_SEQ_NULL;
    }

    if( SMP_IS_VALID_RTS_SEQ(sAftRtsSeq) == STL_TRUE )
    {
        /* RTS 사용 시작 */
        STL_TRY( smpBeginUseRts( aPageHandle,
                                 sBfrRowSlot,
                                 sTransId,
                                 SML_MAKE_VIEW_SCN(sTransViewScn),
                                 sAftRtsSeq,
                                 &sRtsVerNo,
                                 aEnv ) == STL_SUCCESS );
    }

    SMDMPH_GET_PAD_SPACE( sBfrRowSlot, &sPadSpace );

    /* row header 복사 */
    stlMemcpy( sBfrRowSlot,
               (stlChar*)aData + STL_SIZEOF(smxlTransId) + STL_SIZEOF(smlScn),
               sBfrRowHeaderSize );

    /* Pad Space가 runtime과 다를 수 있으므로 되돌린다. */
    SMDMPH_SET_PAD_SPACE( sBfrRowSlot, &sPadSpace );

    /* RTS 조정 */
    if( SMP_IS_VALID_RTS_SEQ(sAftRtsSeq) == STL_TRUE )
    {
        SMDMPH_SET_RTS_BOUND( sBfrRowSlot );
        SMDMPH_SET_RTSSEQ( sBfrRowSlot, &sAftRtsSeq );
    }

    if( SMP_IS_VALID_RTS_SEQ(sBfrRtsSeq) == STL_TRUE )
    {
        STL_TRY( smpEndUseRts( aPageHandle,
                               sBfrRowSlot,
                               sBfrRtsSeq,
                               0,
                               aEnv ) == STL_SUCCESS );
    }

    /* delete 되었으니 reclaimed space에 일단 추가한다 */
    SMP_ADD_RECLAIMED_SPACE( SMP_FRAME(aPageHandle), sTotalRowSize + sPadSpace );
    SMP_DECREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) );

    smpSetMaxViewScn( aPageHandle, sTransViewScn );
    smdmphValidateTablePage( aPageHandle, aEnv );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdmphRedoMemoryHeapUndelete( smlRid    * aDataRid,
                                        void      * aData,
                                        stlUInt16   aDataSize,
                                        smpHandle * aPageHandle,
                                        smlEnv    * aEnv )
{
    stlChar          * sLogPtr = (stlChar*)aData;
    stlChar          * sRowSlot;
    stlUInt8           sBfrRtsSeq;
    stlInt16           sBfrRowHeaderSize;
    smxlTransId        sTransId;
    smlScn             sTransViewScn;
    stlInt16           sTotalRowSize;
    stlInt16           sPadSpace;

    STL_WRITE_INT64( &sTransId, sLogPtr );
    sLogPtr += STL_SIZEOF(smxlTransId);
    STL_WRITE_INT64( &sTransViewScn, sLogPtr );
    sLogPtr += STL_SIZEOF(smlScn);

    SMD_TRY_INTERNAL_ERROR( (sTransId == SML_INVALID_TRANSID) &&
                            (sTransViewScn == SML_INFINITE_SCN) &&
                            (SMDMPH_IS_DELETED( sLogPtr ) != STL_TRUE),
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );

    /* before row 정보 수집 */
    sRowSlot = smpGetNthRowItem( aPageHandle, aDataRid->mOffset );
    SMDMPH_GET_ROW_HDR_SIZE( sRowSlot, &sBfrRowHeaderSize );
    sTotalRowSize =  smdmphGetRowSize(sRowSlot);

    if( SMDMPH_IS_RTS_BOUND( sRowSlot ) == STL_TRUE )
    {
        SMDMPH_GET_RTSSEQ( sRowSlot, &sBfrRtsSeq );
    }
    else
    {
        sBfrRtsSeq = SMP_RTS_SEQ_NULL;
    }
        
    SMDMPH_GET_PAD_SPACE( sRowSlot, &sPadSpace );

    /* undo operation에 의해 작성된 로그. rts를 재 할당 할 필요가 없다 */

    /* row header 복사 */
    stlMemcpy( sRowSlot,
               (stlChar*)aData + STL_SIZEOF(smxlTransId) + STL_SIZEOF(smlScn),
               sBfrRowHeaderSize );

    /* Pad Space가 runtime과 다를 수 있으므로 되돌린다. */
    SMDMPH_SET_PAD_SPACE( sRowSlot, &sPadSpace );

    /* RTS 조정 */
    if( SMP_IS_VALID_RTS_SEQ( sBfrRtsSeq ) == STL_TRUE )
    {
        if( SMDMPH_IS_RTS_BOUND( sRowSlot ) == STL_TRUE )
        {
            SMDMPH_SET_RTS_BOUND( sRowSlot );
            SMDMPH_SET_RTSSEQ( sRowSlot, &sBfrRtsSeq );
        }
        else
        {
            STL_TRY( smpEndUseRts( aPageHandle,
                                   sRowSlot,
                                   sBfrRtsSeq,
                                   0,
                                   aEnv ) == STL_SUCCESS );
        }
    }

    /* undelete 되었으니 reclaimed space에서 삭제한다 */
    SMP_REMOVE_RECLAIMED_SPACE( SMP_FRAME(aPageHandle), sTotalRowSize + sPadSpace );
    SMP_INCREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) );

    smdmphValidateTablePage( aPageHandle, aEnv );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdmphRedoMemoryHeapInsertForUpdate( smlRid    * aDataRid,
                                               void      * aData,
                                               stlUInt16   aDataSize,
                                               smpHandle * aPageHandle,
                                               smlEnv    * aEnv )
{
    /* CDC를 위해 REDO TYPE만 따로 만들고 실제 redo는 일반 insert 와 동일하다 */
    return smdmphRedoMemoryHeapInsert( aDataRid, aData, aDataSize, aPageHandle, aEnv );
}


stlStatus smdmphRedoMemoryHeapDeleteForUpdate( smlRid    * aDataRid,
                                            void      * aData,
                                            stlUInt16   aDataSize,
                                            smpHandle * aPageHandle,
                                            smlEnv    * aEnv )
{
    /* CDC를 위해 REDO TYPE만 따로 만들고 실제 redo는 일반 delete 와 동일하다 */
    return smdmphRedoMemoryHeapDelete( aDataRid, aData, aDataSize, aPageHandle, aEnv );
}

stlStatus smdmphRedoMemoryHeapUpdateLogicalHdr( smlRid    * aDataRid,
                                                void      * aData,
                                                stlUInt16   aDataSize,
                                                smpHandle * aPageHandle,
                                                smlEnv    * aEnv )
{
    smdmphLogicalHdr * sLogicalHdr;

    STL_DASSERT( aDataSize == STL_SIZEOF(smdmphLogicalHdr) );
    sLogicalHdr = SMDMPH_GET_LOGICAL_HEADER( aPageHandle );
    stlMemcpy( sLogicalHdr, aData, STL_SIZEOF(smdmphLogicalHdr) );

    return STL_SUCCESS;
}

stlStatus smdmphRedoMemoryHeapAppend( smlRid    * aDataRid,
                                      void      * aData,
                                      stlUInt16   aDataSize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv )
{
    stlChar            sLogBuf[SMP_PAGE_SIZE];
    smlRid             sUndoRid;
    stlUInt8           sSrcRtsSeq;
    stlUInt8           sDstRtsSeq;
    stlUInt8           sRtsVerNo;
    stlBool            sChainedPiece = STL_FALSE;
    stlInt16           sTotalPieceColCount = 0;
    stlInt16           sPadSize = 0;
    stlChar          * sSrcPiece;
    stlChar          * sDstPiece;
    stlInt16           sDstPieceHdrSize;
    stlInt32           sLogOffset = 0;
    smxlTransId        sTransId;
    smlScn             sViewScn;
    smlRid             sNxtPieceRid;
    stlChar          * sAftPieceHdr;
    stlChar          * sAppendPieceBody;
    stlInt32           sFirstColOrd;
    stlInt16           sAppendPieceColCount;
    stlInt16           sAppendPieceBodySize;
    stlInt16           sSrcPieceSize;
    stlInt16           sPieceSizeDiff = 0;
    stlBool            sInSlotUpdate;
    smlTcn             sTcn;

    STL_READ_MOVE_INT64( &sTransId, aData, sLogOffset );
    STL_READ_MOVE_INT64( &sViewScn, aData, sLogOffset );
    STL_READ_MOVE_INT16( &sDstPieceHdrSize, aData, sLogOffset );
    STL_READ_MOVE_BYTES( sLogBuf, aData, sDstPieceHdrSize, sLogOffset );
    STL_READ_MOVE_INT32( &sFirstColOrd, aData, sLogOffset );
    STL_READ_MOVE_INT16( &sAppendPieceColCount, aData, sLogOffset );
    STL_READ_MOVE_INT16( &sAppendPieceBodySize, aData, sLogOffset );
    STL_READ_MOVE_BYTES( sLogBuf + sDstPieceHdrSize, aData, sAppendPieceBodySize, sLogOffset );
    
    sAftPieceHdr = sLogBuf;
    sAppendPieceBody = sLogBuf + sDstPieceHdrSize;

    SMDMPH_GET_LINK_PID( sAftPieceHdr, &sNxtPieceRid.mPageId );
    SMDMPH_GET_LINK_OFFSET( sAftPieceHdr, &sNxtPieceRid.mOffset );
    SMDMPH_GET_TCN( sAftPieceHdr, &sTcn );
    SMDMPH_GET_ROLLBACK_PID( sAftPieceHdr, &sUndoRid.mPageId );
    SMDMPH_GET_ROLLBACK_OFFSET( sAftPieceHdr, &sUndoRid.mOffset );
    
    if( SMDMPH_HAS_NEXT_LINK( sAftPieceHdr ) == STL_TRUE )
    {
        sChainedPiece = STL_TRUE;
        sPieceSizeDiff = sAppendPieceBodySize;
        sPieceSizeDiff += SMDMPH_MAX_ROW_HDR_LEN - SMDMPH_MIN_ROW_HDR_LEN;
    }
    else
    {
        sPieceSizeDiff = sAppendPieceBodySize;
    }

    sSrcPiece = smpGetNthRowItem( aPageHandle, aDataRid->mOffset );
    sSrcPieceSize = smdmphGetRowSize( sSrcPiece );

    SMD_TRY_INTERNAL_ERROR( SMDMPH_HAS_NEXT_LINK( sSrcPiece ) == STL_FALSE,
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );

    STL_TRY( smdmphAllocRowPiece( NULL,
                                  aPageHandle,
                                  sTransId,
                                  sViewScn,
                                  *aDataRid,
                                  sSrcPieceSize,
                                  sPieceSizeDiff,
                                  &sDstPiece,
                                  &sDstRtsSeq,
                                  &sInSlotUpdate,
                                  aEnv )
             == STL_SUCCESS );
    STL_DASSERT( sDstPiece != NULL );
    
    sSrcPiece = smpGetNthRowItem( aPageHandle, aDataRid->mOffset );
    SMDMPH_GET_COLUMN_CNT( sSrcPiece, &sTotalPieceColCount );
    sTotalPieceColCount += sAppendPieceColCount;
    sDstPiece = sSrcPiece;

    /**
     * decrease source RTS reference
     */
    
    if( SMDMPH_IS_RTS_BOUND( sSrcPiece ) == STL_TRUE )
    {
        SMDMPH_GET_RTSSEQ( sSrcPiece, &sSrcRtsSeq );
    }
    else
    {
        sSrcRtsSeq = SMP_RTS_SEQ_NULL;
    }
    
    if( SMP_IS_VALID_RTS_SEQ(sSrcRtsSeq) == STL_TRUE )
    {
        STL_TRY( smpEndUseRts( aPageHandle,
                               sSrcPiece,
                               sSrcRtsSeq,
                               0,
                               aEnv ) == STL_SUCCESS );
    }

    /**
     * allocate space
     */

    if( sInSlotUpdate == STL_FALSE )
    {
        SMDMPH_GET_PAD_SPACE( sSrcPiece, &sPadSize );
        
        STL_TRY( smpReallocNthSlot( aPageHandle,
                                    aDataRid->mOffset,
                                    sSrcPieceSize + sPieceSizeDiff,
                                    STL_FALSE,
                                    &sDstPiece ) == STL_SUCCESS );
        STL_DASSERT( sDstPiece != NULL );
        
        SMP_ADD_RECLAIMED_SPACE( SMP_FRAME(aPageHandle), sSrcPieceSize + sPadSize );
    }

    /**
     * copy piece & append piece
     */

    sPadSize = 0;
    
    if( sChainedPiece == STL_TRUE )
    {
        if( sInSlotUpdate == STL_TRUE )
        {
            stlMemmove( sDstPiece + SMDMPH_MAX_ROW_HDR_LEN,
                        sSrcPiece + SMDMPH_MIN_ROW_HDR_LEN,
                        sSrcPieceSize - SMDMPH_MIN_ROW_HDR_LEN );
            
            SMDMPH_GET_PAD_SPACE( sDstPiece, &sPadSize );
            sPadSize -= (sAppendPieceBodySize +
                         SMDMPH_MAX_ROW_HDR_LEN -
                         SMDMPH_MIN_ROW_HDR_LEN) ;
        }
        else
        {
            stlMemcpy( sDstPiece, sSrcPiece, SMDMPH_MIN_ROW_HDR_LEN );
            stlMemcpy( sDstPiece + SMDMPH_MAX_ROW_HDR_LEN,
                       sSrcPiece + SMDMPH_MIN_ROW_HDR_LEN,
                       sSrcPieceSize - SMDMPH_MIN_ROW_HDR_LEN );
        }

        /**
         * copy header
         */
        stlMemcpy( sDstPiece, sAftPieceHdr, sDstPieceHdrSize );
        
        /**
         * append piece
         */
        stlMemcpy( (sDstPiece +
                    SMDMPH_MAX_ROW_HDR_LEN +
                    sSrcPieceSize -
                    SMDMPH_MIN_ROW_HDR_LEN),
                   sAppendPieceBody,
                   sAppendPieceBodySize );
    }
    else
    {
        if( sInSlotUpdate == STL_TRUE )
        {
            SMDMPH_GET_PAD_SPACE( sDstPiece, &sPadSize );
            sPadSize -= sAppendPieceBodySize;
        }
        else
        {
            stlMemcpy( sDstPiece, sSrcPiece, sSrcPieceSize );
        }
            
        /**
         * append piece
         */
        stlMemcpy( sDstPiece + sSrcPieceSize,
                   sAppendPieceBody,
                   sAppendPieceBodySize );
    }
    
    SMDMPH_SET_PAD_SPACE( sDstPiece, &sPadSize );
    
    /**
     * fill piece header
     */
    
    if( SMP_IS_VALID_RTS_SEQ(sDstRtsSeq) == STL_TRUE )
    {    
        /* RTS 조정 */
        STL_TRY( smpBeginUseRts( aPageHandle,
                                 sDstPiece,
                                 sTransId,
                                 SML_MAKE_VIEW_SCN(sViewScn),
                                 sDstRtsSeq,
                                 &sRtsVerNo,
                                 aEnv ) == STL_SUCCESS );
        
        SMDMPH_SET_RTS_BOUND( sDstPiece );
        SMDMPH_SET_RTSSEQ( sDstPiece, &sDstRtsSeq );
    }

    SMDMPH_SET_COLUMN_CNT( sDstPiece, &sTotalPieceColCount );
    SMDMPH_SET_TCN( sDstPiece, &sTcn );
    SMDMPH_SET_TRANS_ID( sDstPiece, &sTransId );
    SMDMPH_SET_ROLLBACK_PID( sDstPiece, &sUndoRid.mPageId );
    SMDMPH_SET_ROLLBACK_OFFSET( sDstPiece, &sUndoRid.mOffset );

    smpSetMaxViewScn( aPageHandle, sViewScn );
    smdmphValidateTablePage( aPageHandle, aEnv );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmphRedoMemoryHeapUnappend( smlRid    * aDataRid,
                                        void      * aData,
                                        stlUInt16   aDataSize,
                                        smpHandle * aPageHandle,
                                        smlEnv    * aEnv )
{
    stlChar     * sBfrPiece;
    stlChar     * sAftPieceHdr = aData;
    stlUInt8      sBfrRtsSeq;
    stlInt16      sPadSpace;
    stlInt16      sAftPieceSize = 0;
    stlInt16      sBfrPieceSize = 0;
    stlInt16      sBfrPieceHdrSize;

    STL_WRITE_INT16( &sAftPieceSize, sAftPieceHdr + SMDMPH_MIN_ROW_HDR_LEN );
    
    sBfrPiece = smpGetNthRowItem( aPageHandle, aDataRid->mOffset );
    
    if( SMDMPH_IS_RTS_BOUND( sBfrPiece ) == STL_TRUE )
    {
        SMDMPH_GET_RTSSEQ( sBfrPiece, &sBfrRtsSeq );
    }
    else
    {
        sBfrRtsSeq = SMP_RTS_SEQ_NULL;
    }
    
    sBfrPieceSize = smdmphGetRowSize( sBfrPiece );
    SMDMPH_GET_PAD_SPACE( sBfrPiece, &sPadSpace );
    sPadSpace += sBfrPieceSize - sAftPieceSize;
    
    /**
     * apply log data
     */

    SMDMPH_GET_ROW_HDR_SIZE( sBfrPiece, &sBfrPieceHdrSize );
    stlMemcpy( sBfrPiece, sAftPieceHdr, SMDMPH_MIN_ROW_HDR_LEN );
    stlMemmove( sBfrPiece + SMDMPH_MIN_ROW_HDR_LEN,
                sBfrPiece + sBfrPieceHdrSize,
                sAftPieceSize - SMDMPH_MIN_ROW_HDR_LEN );

    /**
     * adjust piece header
     */
    
    SMDMPH_SET_PAD_SPACE( sBfrPiece, &sPadSpace );
    
    if( SMP_IS_VALID_RTS_SEQ(sBfrRtsSeq) == STL_TRUE )
    {
        if( SMDMPH_IS_RTS_BOUND( sBfrPiece ) == STL_TRUE )
        {
            SMDMPH_SET_RTS_BOUND( sBfrPiece );
            SMDMPH_SET_RTSSEQ( sBfrPiece, &sBfrRtsSeq );
        }
        else
        {
            STL_TRY( smpEndUseRts( aPageHandle,
                                   sBfrPiece,
                                   sBfrRtsSeq,
                                   0,
                                   aEnv ) == STL_SUCCESS );
        }
    }
    
    smdmphValidatePadSpace( aPageHandle );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdmphRedoMemoryPendingInsert( smlRid    * aDataRid,
                                         void      * aData,
                                         stlUInt16   aDataSize,
                                         smpHandle * aPageHandle,
                                         smlEnv    * aEnv )
{
    return STL_SUCCESS;
}


stlStatus smdmphRedoMemoryHeapCompaction( smlRid    * aDataRid,
                                          void      * aData,
                                          stlUInt16   aDataSize,
                                          smpHandle * aPageHandle,
                                          smlEnv    * aEnv )
{
    stlInt16    sExtendSlotSeq;
    stlInt16    sExtendSize;
    stlInt32    sOffset = 0;

    STL_READ_MOVE_INT16( &sExtendSlotSeq, aData, sOffset );
    STL_READ_MOVE_INT16( &sExtendSize, aData, sOffset );

    STL_TRY( smdmphCompactPageInternal( NULL,
                                        aPageHandle,
                                        sExtendSlotSeq,
                                        sExtendSize,
                                        aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
