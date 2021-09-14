/*******************************************************************************
 * smdmphDml.c
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
#include <sms.h>
#include <smd.h>
#include <smxm.h>
#include <smxl.h>
#include <smp.h>
#include <sme.h>
#include <smf.h>
#include <smkl.h>
#include <sma.h>
#include <smdmphRowDef.h>
#include <smdmphDef.h>
#include <smdDef.h>
#include <smdmph.h>

/**
 * @file smdmphDml.c
 * @brief Storage Manager Layer Memory Heap Table DML Routines
 */

/**
 * @todo row piece당 maximum column count 255로 제약하기
 */

/**
 * @addtogroup smd
 * @{
 */

/**
 * @brief 주어진 row의 총 길이를 구한다
 * @param[in] aRow 조사할 row의 시작 위치
 */
inline stlInt16 smdmphGetRowSize( stlChar * aRow )
{
    stlInt32   i;
    stlInt16   sRowSize;
    stlChar  * sPtr;
    stlInt64   sColSize;
    stlInt16   sColCount;

    SMDMPH_GET_ROW_HDR_SIZE( aRow, &sRowSize );
    sPtr = aRow + sRowSize;
    SMDMPH_GET_COLUMN_CNT( aRow, &sColCount );

    for( i = 0; i < sColCount; i++ )
    {
        SMP_GET_TOTAL_COLUMN_SIZE( sPtr, sColSize );
        sRowSize += sColSize;
        sPtr += sColSize;
    }

    return sRowSize;
}


/**
 * @brief 주어진 row의 commit scn을 구한다. commit 되지 않은경우 SML_INFINITE_SCN를 반환한다
 * @param[in] aTbsId Tablespace Identifier
 * @param[in] aPageHandle 조사할 row가 담긴 페이지의 핸들
 * @param[in] aRow 조사할 row의 시작 위치
 * @param[out] aCommitScn 구해진 Commit Scn
 * @param[out] aTransId aRow가 commit 안된 경우 변경한 transaction의 id
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmphGetCommitScn( smlTbsId      aTbsId,
                              smpHandle   * aPageHandle,
                              stlChar     * aRow,
                              smlScn      * aCommitScn,
                              smxlTransId * aTransId,
                              smlEnv      * aEnv )
{
    smpRts     * sRts;
    stlUInt8     sRtsSeq;
    smlScn       sScn;
    smxlTransId  sWaitTransId = SML_INVALID_TRANSID;
    stlBool      sIsSuccess = STL_FALSE;

    if( SMDMPH_IS_RTS_BOUND( aRow ) == STL_TRUE )
    {
        /* rts bound */
        SMDMPH_GET_RTSSEQ( aRow, &sRtsSeq );
        sRts = SMP_GET_NTH_RTS(SMP_FRAME(aPageHandle), sRtsSeq);
        sWaitTransId = sRts->mTransId;

        if( SMP_RTS_IS_STAMPED(sRts) == STL_TRUE )
        {
            sScn = sRts->mScn;
        }
        else if( SMP_RTS_IS_STABLE(sRts) == STL_TRUE )
        {
            sScn = 0;
        }
        else
        {
            STL_ASSERT( SMP_RTS_IS_NOT_STAMPED(sRts) == STL_TRUE );
            /* commit scn 구하면서 RTS stamping도 수행함 */
            STL_TRY( smpGetCommitScn( aTbsId,
                                      aPageHandle,
                                      sRtsSeq,
                                      0,
                                      0 /* Stmt View Scn : 사용 안함 */,
                                      &sWaitTransId,
                                      &sScn,
                                      aEnv ) == STL_SUCCESS );
        }
    }
    else
    {
        SMDMPH_GET_SCN( aRow, &sScn );
        if( SML_IS_VIEW_SCN( sScn ) == STL_TRUE )
        {
            SMDMPH_GET_TRANS_ID( aRow, &sWaitTransId );
            STL_TRY( smxlGetCommitScn( sWaitTransId,
                                       SML_MAKE_REAL_SCN(sScn),
                                       &sScn,
                                       aEnv ) == STL_SUCCESS );

            if( sScn != SML_INFINITE_SCN )
            {
                /* dummy page handle이 아니며, last version일 경우에만
                   lock escalation 후 row stamping을 한다 */
                if( (aPageHandle->mPch != NULL) &&
                    (aRow > (char*)aPageHandle->mFrame) &&
                    (aRow < (char*)aPageHandle->mFrame + SMP_PAGE_SIZE) )
                {
                    /**
                     * Lock 상승이 성공하는 경우에만 Stamping을 한다.
                     */
                    STL_TRY( knlEscalateLatch( &aPageHandle->mPch->mPageLatch,
                                               &sIsSuccess,
                                               KNL_ENV( aEnv ) )
                             == STL_SUCCESS );

                    if( sIsSuccess == STL_TRUE )
                    {
                        SMDMPH_UNSET_RTS_BOUND( aRow );
                        SMDMPH_SET_SCN( aRow, &sScn );

                        STL_TRY( smpSetDirty( aPageHandle, aEnv ) == STL_SUCCESS );
                    }
                }
                else
                {
                    /* dummy handle이거나 undo record가 적용된 버전이면 그냥 stamping한다 */
                    SMDMPH_UNSET_RTS_BOUND( aRow );
                    SMDMPH_SET_SCN( aRow, &sScn );
                }
            }
        }
    }

    *aCommitScn = sScn;
    *aTransId = sWaitTransId;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 주어진 row가 주어진 rtsseq를 사용하고 있으면 SCN을 stamping한다
 * @param[in] aRow stamping을 시도할 row
 * @param[in] aRtsSeq 체크할 Rts의 Sequence
 * @param[in] aCommitScn stamping할 commit scn
 * @param[in] aLastRtsVerNo 체크할 Rts의 최신 버전 번호
 * @param[out] aReferenceCount stabilize된 Rts 정보의 개수
 */
void smdmphStabilizeRow( stlChar  * aRow,
                         stlUInt8   aRtsSeq,
                         stlUInt8   aLastRtsVerNo,
                         smlScn     aCommitScn,
                         stlInt16 * aReferenceCount )
{
    stlUInt8  sRtsSeq;

    *aReferenceCount = 0;

    if( SMDMPH_IS_RTS_BOUND( aRow ) == STL_TRUE )
    {
        SMDMPH_GET_RTSSEQ( aRow, &sRtsSeq );
        if( sRtsSeq == aRtsSeq )
        {
            STL_ASSERT( aCommitScn != SML_INFINITE_SCN );
            SMDMPH_UNSET_RTS_BOUND( aRow );
            SMDMPH_SET_SCN( aRow, &aCommitScn );
            (*aReferenceCount)++;
        }
    }
}


/**
 * @brief 주어진 페이지를 재조정하여 최소의 공간만을 사용하도록 한다
 * @param[in] aMiniTrans  Mini-Transaction
 * @param[in] aPageHandle Compact 연산을 할 페이지의 Handle
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmphCompactPage( smxmTrans * aMiniTrans,
                             smpHandle * aPageHandle,
                             smlEnv    * aEnv )
{
    return smdmphCompactPageInternal( aMiniTrans,
                                      aPageHandle,
                                      STL_INT16_MAX,
                                      0,
                                      aEnv );
}


/**
 *
 * @brief 주어진 페이지를 compact하면서 특정 row slot을 주어진 크기만큼 확장한다
 * @param[in] aMiniTrans  Mini-Transaction
 * @param[in] aPageHandle 연산을 할 페이지의 Handle
 * @param[in] aSlotSeq 확장연산을 할 row의 slot sequence
 * @param[in] aExtendSize 기존 slot의 크기보다 확장할 크기
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmphExtendRowSlot( smxmTrans  * aMiniTrans,
                               smpHandle  * aPageHandle,
                               stlInt16     aSlotSeq,
                               stlInt16     aExtendSize,
                               smlEnv     * aEnv )
{
    return smdmphCompactPageInternal( aMiniTrans,
                                      aPageHandle,
                                      aSlotSeq,
                                      aExtendSize,
                                      aEnv );
}


/**
 * @brief 주어진 페이지를 compact하면서 특정 row slot을 주어진 크기만큼 확장한다
 * @param[in] aMiniTrans  Mini-Transaction
 * @param[in] aPageHandle 연산을 할 페이지의 Handle
 * @param[in] aSlotSeq 확장연산을 할 row의 slot sequence
 * @param[in] aExtendSize 기존 slot의 크기보다 확장할 크기
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmphCompactPageInternal( smxmTrans  * aMiniTrans,
                                     smpHandle  * aPageHandle,
                                     stlInt16     aSlotSeq,
                                     stlInt16     aExtendSize,
                                     smlEnv     * aEnv )
{
    stlChar              sBuffer[SMP_PAGE_SIZE + STL_SIZEOF(stlInt64)];
    stlChar            * sDummyFrame;
    smpHandle            sTmpHandle;
    smxlTransId          sTransId = SML_INVALID_TRANSID;
    stlInt16             sTotalRowSize;
    stlInt16             sAllocSize;
    stlChar            * sSrcRow;
    stlChar            * sDstRow;
    smpRts             * sSrcRts;
    stlUInt8             sSrcRtsSeq;
    stlUInt8             sRtsSeq;
    stlUInt8             sRtsVerNo;
    stlInt16             i;
    smlScn               sCommitScn;
    smlScn               sScn;
    smlScn               sAgableScn;
    smpRowItemIterator   sIterator;
    stlInt16             sPadSpace;
    stlUInt8             sDstRtsCnt = 0;
    stlUInt8             sDstRtsSeqs[SMP_MAX_RTS_COUNT];
    smdmphLogicalHdr   * sLogicalHdr;
    stlUInt16            sUnusedSpace;
    stlChar              sRedoData[STL_SIZEOF(stlInt16) * 2 ];
    stlInt32             sOffset = 0;
    smlTbsId             sTbsId = smpGetTbsId(aPageHandle);

    sAgableScn = smxlGetAgableTbsScn( sTbsId, aEnv );
    sUnusedSpace = smpGetUnusedSpace( SMP_FRAME( aPageHandle ) );
    
    stlMemset( sDstRtsSeqs, SMP_RTS_SEQ_NULL, STL_SIZEOF(stlUInt8) * SMP_MAX_RTS_COUNT );

    sDummyFrame = (stlChar*)STL_ALIGN( (stlInt64)sBuffer, 8 );
    SMP_MAKE_DUMMY_HANDLE( &sTmpHandle, sDummyFrame );
    stlMemcpy( sDummyFrame, SMP_FRAME(aPageHandle), SMP_PAGE_SIZE );

    STL_TRY( smpResetBody(aPageHandle) == STL_SUCCESS );

    for( i = 0; i < SMP_GET_CURRENT_RTS_COUNT( SMP_FRAME(&sTmpHandle) ); i++ )
    {
        smpTryStampRts( NULL, sTbsId, &sTmpHandle, i, NULL, aEnv );
    }

    SMP_INIT_ROWITEM_ITERATOR( sDummyFrame, &sIterator );
    sLogicalHdr = SMDMPH_GET_LOGICAL_HEADER( aPageHandle );
    
    SMP_FOREACH_ROWITEM_ITERATOR( &sIterator )
    {
        if( SMP_IS_FREE_ROWITEM( &sIterator ) == STL_TRUE )
        {
            continue;
        }
        
        sSrcRow = SMP_GET_CURRENT_ROWITEM( &sIterator );
        i = SMP_GET_CURRENT_ROWITEM_SEQ( &sIterator );

        if( SMDMPH_IS_RTS_BOUND( sSrcRow ) == STL_TRUE )
        {
            SMDMPH_GET_RTSSEQ( sSrcRow, &sRtsSeq );
            sSrcRts = SMP_GET_NTH_RTS(sDummyFrame, sRtsSeq);
            sTransId = sSrcRts->mTransId;
            sScn = sSrcRts->mScn;
        }
        else
        {
            sRtsSeq = SMP_RTS_SEQ_NULL;
            sSrcRts = NULL;
            SMDMPH_GET_TRANS_ID( sSrcRow, &sTransId );
            SMDMPH_GET_SCN( sSrcRow, &sScn );
        }

        if( SML_IS_VIEW_SCN( sScn ) == STL_TRUE )
        {
            if( (sRtsSeq == SMP_RTS_SEQ_NULL) &&
                (SMDMPH_IS_DELETED( sSrcRow ) == STL_TRUE) )
            {
                STL_TRY( smxlGetCommitScn( sTransId,
                                           SML_MAKE_REAL_SCN(sScn),
                                           &sCommitScn,
                                           aEnv ) == STL_SUCCESS );
            }
            else
            {
                sCommitScn = SML_INFINITE_SCN;
            }
        }
        else
        {
            sCommitScn = sScn;
        }

        sTotalRowSize = smdmphGetRowSize(sSrcRow);
        if( SMDMPH_IS_DELETED( sSrcRow ) == STL_TRUE )
        {
            /* recovery redo 시에는 agable SCN 이 infinite scn 이다 */
            if( (sCommitScn != SML_INFINITE_SCN) && (sCommitScn <= sAgableScn) )
            {
                /* free list를 사용하는 table에서는 이 slot을 free list에 매단다 */
                STL_TRY( smpFreeNthSlot( aPageHandle, i, 0 ) == STL_SUCCESS );
                continue;
            }
        }
        
        if( SMDMPH_IS_RTS_BOUND( sSrcRow ) == STL_TRUE )
        {
            if( sCommitScn >= sAgableScn )
            {
                if( sCommitScn != SML_INFINITE_SCN )
                {
                    sSrcRts = NULL;
                    SMDMPH_UNSET_RTS_BOUND( sSrcRow );
                    SMDMPH_SET_SCN( sSrcRow, &sCommitScn );
                }
                else
                {
                    SMDMPH_GET_RTSSEQ( sSrcRow, &sSrcRtsSeq );
                    if( sDstRtsSeqs[sSrcRtsSeq] == SMP_RTS_SEQ_NULL )
                    {
                        sRtsSeq = sDstRtsCnt;
                        STL_TRY( smpAllocRtsForCopy( aPageHandle,
                                                     sTransId,
                                                     sScn, /* SML_MAKE_VIEW_SCN을 하면 안됨. */
                                                     sRtsSeq,
                                                     aEnv ) == STL_SUCCESS );
                        STL_ASSERT( SMP_IS_VALID_RTS_SEQ(sRtsSeq) == STL_TRUE );
                        
                        sDstRtsSeqs[sSrcRtsSeq] = sRtsSeq;
                        sDstRtsCnt++;
                    }
                    else
                    {
                        sRtsSeq = sDstRtsSeqs[sSrcRtsSeq];
                    }
                }
            }
            else
            {
                sSrcRts = NULL;
                sCommitScn = 0;
                SMDMPH_UNSET_RTS_BOUND( sSrcRow );
                SMDMPH_SET_SCN( sSrcRow, &sCommitScn );
            }
        }
        else
        {
            sSrcRts = NULL;
            sRtsSeq = SMP_RTS_SEQ_NULL;
        }

        SMDMPH_GET_PAD_SPACE( sSrcRow, &sPadSpace );
        
        if( (sCommitScn != SML_INFINITE_SCN) && (i != aSlotSeq) )
        {
            if( sTotalRowSize < SMDMPH_MINIMUM_ROW_PIECE_SIZE )
            {
                sPadSpace = SMDMPH_MINIMUM_ROW_PIECE_SIZE - sTotalRowSize;
            }
            else
            {
                sPadSpace = 0;
            }
        }
        
        sAllocSize = sTotalRowSize + sPadSpace;
            
        /* 목적했던 row를 찾았으면 필요한 만큼 확장을 한다 */
        if( i == aSlotSeq )
        {
            STL_TRY( smpAllocNthSlot( aPageHandle,
                                      i,
                                      sAllocSize + aExtendSize,
                                      STL_TRUE,
                                      &sDstRow ) == STL_SUCCESS );
            STL_ASSERT( sDstRow != NULL );
        }
        else
        {
            STL_TRY( smpAllocNthSlot( aPageHandle,
                                      i,
                                      sAllocSize,
                                      STL_TRUE,
                                      &sDstRow ) == STL_SUCCESS );
            STL_ASSERT( sDstRow != NULL );
        }

        if( SMDMPH_IS_RTS_BOUND( sSrcRow ) == STL_TRUE )
        {
            STL_ASSERT( sSrcRts != NULL);
            STL_TRY( smpBeginUseRts( aPageHandle,
                                     sDstRow,
                                     sTransId,
                                     sSrcRts->mScn, /* SML_MAKE_VIEW_SCN을 하면 안됨 */
                                     sRtsSeq,
                                     &sRtsVerNo,
                                     aEnv ) == STL_SUCCESS );
        }
        /* 목적했던 row를 찾았으면 필요한 만큼 실제로 확장을 한다 */
        if( i == aSlotSeq )
        {
            STL_TRY( smpAllocNthSlot( aPageHandle,
                                      i,
                                      sAllocSize + aExtendSize,
                                      STL_FALSE,
                                      &sDstRow ) == STL_SUCCESS );
 
            stlMemcpy( sDstRow, sSrcRow, sTotalRowSize );
            sPadSpace += aExtendSize;
        }
        else
        {
            STL_TRY( smpAllocNthSlot( aPageHandle,
                                      i,
                                      sAllocSize,
                                      STL_FALSE,
                                      &sDstRow ) == STL_SUCCESS );

            STL_ASSERT( sDstRow != NULL );
            stlMemcpy( sDstRow, sSrcRow, sTotalRowSize );
        }

        if( SMDMPH_IS_RTS_BOUND( sSrcRow ) == STL_TRUE )
        {
            SMDMPH_SET_RTS_BOUND( sDstRow );
            SMDMPH_SET_RTSSEQ( sDstRow, &sRtsSeq );
        }

        SMDMPH_SET_PAD_SPACE( sDstRow, &sPadSpace );

        if( SMDMPH_IS_DELETED( sSrcRow ) != STL_TRUE )
        {
            SMP_INCREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) );
        }
        else
        {
            SMP_ADD_RECLAIMED_SPACE( SMP_FRAME(aPageHandle), sTotalRowSize + sPadSpace );
        }

        /**
         * Simple row가 아닌 조건 = Multiple piece로 구성된 row
         * - Continuous piece
         * - Slave piece
         * - Master이지만 Next Link를 갖는 piece
         */
        if( SMDMPH_IS_CONT_COL( sDstRow ) ||
            (SMDMPH_IS_MASTER_ROW( sDstRow ) == STL_FALSE) ||
            (SMDMPH_IS_MASTER_ROW( sDstRow ) && SMDMPH_HAS_NEXT_LINK( sDstRow ) ) )
        {
            if( sLogicalHdr->mSimplePage == STL_TRUE )
            {
                sLogicalHdr->mSimplePage = STL_FALSE;
            }
        }
    }

    /**
     * Compaction으로 인하여 공간이 확보된 경우는 logging을 한다.
     */
    
    if( smpGetUnusedSpace( SMP_FRAME(aPageHandle) ) > sUnusedSpace )
    {
        if( aMiniTrans != NULL )
        {
            STL_WRITE_MOVE_INT16( sRedoData, &aSlotSeq, sOffset );
            STL_WRITE_MOVE_INT16( sRedoData, &aExtendSize, sOffset );
            
            STL_TRY( smxmWriteLog( aMiniTrans,
                                   SMG_COMPONENT_TABLE,
                                   SMR_LOG_MEMORY_HEAP_COMPACTION,
                                   SMR_REDO_TARGET_PAGE,
                                   (void*)sRedoData,
                                   sOffset,
                                   smpGetTbsId(aPageHandle),
                                   smpGetPageId(aPageHandle),
                                   0,
                                   SMXM_LOGGING_REDO,
                                   aEnv )
                     == STL_SUCCESS );
        }
    }
    
    smdmphValidateTablePage( aPageHandle, aEnv );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief 주어진 row slot의 header에 정보를 세팅한다
 * @param[in,out] aSlot 정보를 세팅할 row의 시작위치
 * @param[in] aRtsSeq 사용하는 RTS의 순번
 * @param[in] aScn 현재 statement의 Scn
 * @param[in] aTcn 현재 statement의 Tcn
 * @param[in] aTransId 현재 statement의 Transaction Id
 * @param[in] aRollbackRid row의 이전버전 정보가 저장된 위치
 * @param[in] aColumnCount row의 총 컬럼 개수
 * @param[in] aPadSize row slot이 가지고 있는 여분의 공간의 크기
 * @param[in] aNextPieceRid 주어진 row의 다음 piece가 저장된 위치
 */
inline stlStatus smdmphWriteRowHeader( stlChar      * aSlot,
                                       stlUInt8       aRtsSeq,
                                       smlScn         aScn,
                                       smlTcn         aTcn,
                                       smxlTransId    aTransId,
                                       smlRid       * aRollbackRid,
                                       stlUInt16      aColumnCount,
                                       stlInt16       aPadSize,
                                       smlRid       * aNextPieceRid )
{
    smlPid   sInvalidPid = SMP_NULL_PID;
    stlInt16 sOffset = 0;

    STL_DASSERT( aPadSize >= 0 );
    STL_DASSERT( aColumnCount <= SMD_MAX_COLUMN_COUNT_IN_PIECE );
    
    SMDMPH_INIT_ROW_HDR_BITMAP( aSlot );
    SMDMPH_SET_MASTER_ROW( aSlot );
    SMDMPH_UNSET_HAS_NEXT_LINK( aSlot );
    SMDMPH_UNSET_DELETED( aSlot );
    SMDMPH_SET_TCN( aSlot, &aTcn );
    if( SMP_IS_VALID_RTS_SEQ(aRtsSeq) == STL_TRUE )
    {
        SMDMPH_SET_RTS_BOUND( aSlot );
        SMDMPH_SET_RTSSEQ( aSlot, &aRtsSeq );
    }
    else
    {
        /* DS mode에서는 row insert와 동시에 stable 상태로 세팅한다 */
        SMDMPH_UNSET_RTS_BOUND( aSlot );
        SMDMPH_SET_SCN( aSlot, &aScn );
    }

    SMDMPH_SET_TRANS_ID( aSlot, &aTransId );
    if( aRollbackRid != NULL )
    {
        SMDMPH_SET_ROLLBACK_PID( aSlot, &aRollbackRid->mPageId );
        SMDMPH_SET_ROLLBACK_OFFSET( aSlot, &aRollbackRid->mOffset );
    }
    else
    {
        SMDMPH_SET_ROLLBACK_PID( aSlot, &sInvalidPid );
        SMDMPH_SET_ROLLBACK_OFFSET( aSlot, &sOffset );
    }
    SMDMPH_SET_COLUMN_CNT( aSlot, &aColumnCount );
    SMDMPH_SET_PAD_SPACE( aSlot, &aPadSize );


    if( aNextPieceRid != NULL )
    {
        if( aNextPieceRid->mPageId != SMP_NULL_PID )
        {
            SMDMPH_SET_HAS_NEXT_LINK( aSlot );
            SMDMPH_SET_LINK_PID( aSlot, &aNextPieceRid->mPageId );
            SMDMPH_SET_LINK_OFFSET( aSlot, &aNextPieceRid->mOffset );
        }
    }

    return STL_SUCCESS;
}


/**
 * @brief 주어진 row slot의 body에 컬럼들을 기록한다
 * @param[in,out] aSlot 컬럼들을 기록할 row의 시작 위치
 * @param[in] aValueIdx 컬럼들을 기록할 block의 idx
 * @param[in] aFirstCol 기록을 시작할 첫 컬럼
 * @param[in] aLastCol 기록을 종료할 마지막 컬럼
 * @param[out] aPieceBodySize row 중 컬럼들을 기록한 부분의 길이
 */
inline stlStatus smdmphWriteRowBody( stlChar           * aSlot,
                                     stlInt32            aValueIdx,
                                     knlValueBlockList * aFirstCol,
                                     knlValueBlockList * aLastCol,
                                     stlUInt16         * aPieceBodySize )
{
    stlChar           * sPtr;
    stlChar           * sBeginPtr;
    knlValueBlockList * sCurCol = aFirstCol;
    stlUInt16           sSlotHdrSize;
    dtlDataValue      * sDataValue;
    stlBool             sIsZero;

    SMDMPH_GET_ROW_HDR_SIZE( aSlot, &sSlotHdrSize );
    sPtr = aSlot + sSlotHdrSize;
    sBeginPtr = sPtr;
    
    while( sCurCol != NULL )
    {
        sIsZero = STL_FALSE;
        
        sDataValue = KNL_GET_BLOCK_DATA_VALUE(sCurCol, aValueIdx);

        if( DTL_NUMERIC_IS_ZERO(sDataValue->mValue, sDataValue->mLength) )
        {
            if( dtlDataTypeGroup[sDataValue->mType] == DTL_GROUP_NUMBER )
            {
                sIsZero = STL_TRUE;
            }
        }
        
        SMP_WRITE_COLUMN_AND_MOVE_PTR( sPtr,
                                       sDataValue->mValue,
                                       sDataValue->mLength,
                                       sIsZero );
        sCurCol = sCurCol->mNext;
    }

    *aPieceBodySize = sPtr - sBeginPtr;

    return STL_SUCCESS;
}


/**
 * @brief 새 페이지에 row piece 한 개를 삽입한다
 * @param[in] aRelationHandle row를 삽입할 Relation의 handle
 * @param[in] aTransId transaction id
 * @param[in] aViewScn transaction의 view scn
 * @param[in] aRowPtr 삽입할 row를 가진 버퍼공간
 * @param[in] aRowSize 삽입할 row의 크기
 * @param[in] aFirstColOrd 삽입할 row piece중 첫 컬럼의 순번
 * @param[in] aIsForUpdate Update 중 delete/insert에 의한 호출인지 여부
 * @param[out] aRowRid 삽입한 row의 위치
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmphInsertRowPiece( void              * aRelationHandle,
                                smxlTransId         aTransId,
                                smlScn              aViewScn,
                                stlChar           * aRowPtr,
                                stlInt16            aRowSize,
                                stlInt32            aFirstColOrd,
                                stlBool             aIsForUpdate,
                                smlRid            * aRowRid,
                                smlEnv            * aEnv )
{
    smpHandle          sPageHandle;
    smlRid             sUndoRid;
    stlChar            sLogBuf[SMP_PAGE_SIZE];
    stlChar          * sLogPtr;
    stlChar *          sSlot = NULL;
    stlUInt8           sRtsSeq;
    stlUInt8           sRtsVerNo;
    stlInt16           sSlotSeq;
    stlUInt32          sAttr = SMXM_ATTR_REDO;
    smlRid             sSegRid;
    stlInt32           sState = 0;
    void             * sSegmentHandle = SME_GET_SEGMENT_HANDLE(aRelationHandle);
    smxmTrans          sMiniTrans;
    smlScn             sScn;
    smdmphLogicalHdr * sLogicalHdr;
    stlInt16           sPadSpace;
    stlInt16           sLogType = ( (aIsForUpdate == STL_TRUE) ?
                                    SMR_LOG_MEMORY_HEAP_INSERT_FOR_UPDATE :
                                    SMR_LOG_MEMORY_HEAP_INSERT );

    SMDMPH_GET_PAD_SPACE( aRowPtr, &sPadSpace );

    sSegRid = smsGetSegRid( sSegmentHandle );

    STL_TRY( smxmBegin( &sMiniTrans,
                        aTransId,
                        sSegRid,
                        sAttr,
                        aEnv )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( smdmphGetInsertablePage( aRelationHandle,
                                      &sMiniTrans,
                                      aTransId,
                                      aViewScn,
                                      aRowSize,
                                      &sPageHandle,
                                      &sRtsSeq,
                                      &sSlotSeq,
                                      aEnv )
             == STL_SUCCESS );

    /* write undo record */
    STL_TRY( smxlInsertUndoRecord( &sMiniTrans,
                                   SMG_COMPONENT_TABLE,
                                   SMD_UNDO_LOG_MEMORY_HEAP_INSERT,
                                   NULL,
                                   0,
                                   ((smlRid){ sPageHandle.mPch->mTbsId,
                                              sSlotSeq,
                                              sPageHandle.mPch->mPageId }),
                                   &sUndoRid,
                                   aEnv ) == STL_SUCCESS );

    STL_TRY( smpAllocSlot( &sPageHandle,
                           aRowSize,
                           STL_FALSE,
                           &sSlot,
                           &sSlotSeq ) == STL_SUCCESS );
    STL_ASSERT( sSlot != NULL );

    stlMemcpy( sSlot, aRowPtr, aRowSize - sPadSpace );
 
    if( SMP_IS_VALID_RTS_SEQ(sRtsSeq) == STL_TRUE )
    {
        STL_TRY( smpBeginUseRts( &sPageHandle,
                                 sSlot,
                                 aTransId,
                                 SML_MAKE_VIEW_SCN(aViewScn),
                                 sRtsSeq,
                                 &sRtsVerNo,
                                 aEnv ) == STL_SUCCESS );
        SMDMPH_SET_RTS_BOUND( sSlot );
        SMDMPH_SET_RTSSEQ( sSlot, &sRtsSeq );
    }
    else
    {
        if( SMXL_IS_UNDO_BOUND_TRANS( aTransId ) == STL_TRUE )
        {
            sScn = SML_MAKE_VIEW_SCN(aViewScn);
            SMDMPH_UNSET_RTS_BOUND( sSlot );
            SMDMPH_SET_SCN( sSlot, &sScn );
        }
        else
        {
            /* DS mode임 */
            sScn = 0;
            SMDMPH_UNSET_RTS_BOUND( sSlot );
            SMDMPH_SET_SCN( sSlot, &sScn );
        }
    }

    /**
     * Simple row가 아닌 조건 = Multiple piece로 구성된 row
     * - Continuous piece
     * - Slave piece
     * - Master이지만 Next Link를 갖는 piece
     */
    if( SMDMPH_IS_CONT_COL( sSlot ) ||
        (SMDMPH_IS_MASTER_ROW( sSlot ) == STL_FALSE) ||
        (SMDMPH_IS_MASTER_ROW( sSlot ) && SMDMPH_HAS_NEXT_LINK( sSlot ) ) )
    {
        sLogicalHdr = SMDMPH_GET_LOGICAL_HEADER( &sPageHandle );

        if( sLogicalHdr->mSimplePage == STL_TRUE )
        {
            sLogicalHdr->mSimplePage = STL_FALSE;
            STL_TRY( smxmWriteLog( &sMiniTrans,
                                   SMG_COMPONENT_TABLE,
                                   SMR_LOG_MEMORY_HEAP_UPDATE_LOGICAL_HDR,
                                   SMR_REDO_TARGET_PAGE,
                                   (stlChar*)sLogicalHdr,
                                   STL_SIZEOF(smdmphLogicalHdr),
                                   smpGetTbsId(&sPageHandle),
                                   smpGetPageId(&sPageHandle),
                                   0,
                                   SMXM_LOGGING_REDO,
                                   aEnv ) == STL_SUCCESS );
        }
    }

    /* write REDO log */
    sLogPtr = sLogBuf;
    STL_WRITE_INT64( sLogPtr, &aTransId );
    sLogPtr += STL_SIZEOF(smxlTransId);
    STL_WRITE_INT64( sLogPtr, &aViewScn );
    sLogPtr += STL_SIZEOF(smlScn);
    STL_WRITE_INT32( sLogPtr, &aFirstColOrd );
    sLogPtr += STL_SIZEOF(stlInt32);
    stlMemcpy( sLogPtr, sSlot, aRowSize - sPadSpace);
    sLogPtr += aRowSize - sPadSpace;

    STL_TRY( smxmWriteLog( &sMiniTrans,
                           SMG_COMPONENT_TABLE,
                           sLogType,
                           SMR_REDO_TARGET_PAGE,
                           sLogBuf,
                           sLogPtr - sLogBuf,
                           sPageHandle.mPch->mTbsId,
                           sPageHandle.mPch->mPageId,
                           sSlotSeq,  /* Physiological redo를 하기위해 offset이 아닌 seq 사용 */
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    SMP_INCREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(&sPageHandle) );

    if( (smpGetFreeness(&sPageHandle) == SMP_FREENESS_INSERTABLE) &&
        (smpIsPageOverInsertLimit(SMP_FRAME(&sPageHandle),
                                  SMD_GET_PCTFREE(aRelationHandle)) == STL_TRUE) )
    {
        STL_TRY( smdmphUpdatePageStatus( &sMiniTrans,
                                         aRelationHandle,
                                         sSegmentHandle,
                                         &sPageHandle,
                                         SMP_FREENESS_UPDATEONLY,
                                         0,  /* aScn */
                                         aEnv ) == STL_SUCCESS );
    }

    smpSetMaxViewScn( &sPageHandle, aViewScn );

    *aRowRid = ((smlRid){ smsGetTbsId(sSegmentHandle),
                          sSlotSeq,
                          smpGetPageId(&sPageHandle) });

    smdmphValidateTablePage( &sPageHandle, aEnv );
    STL_DASSERT( smpGetSegmentId( &sPageHandle ) ==
                 smsGetSegmentId( sSegmentHandle ) );

    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smxmCommit( &sMiniTrans, aEnv );
    }

    return STL_FAILURE;
} 


stlStatus smdmphGetInsertablePage( void              * aRelationHandle,
                                   smxmTrans         * aMiniTrans,
                                   smxlTransId         aTransId,
                                   smlScn              aViewScn,
                                   stlInt16            aRowSize,
                                   smpHandle         * aPageHandle,
                                   stlUInt8          * aRtsSeq,
                                   stlInt16          * aSlotSeq,
                                   smlEnv            * aEnv )
{
    smpHandle          sPageHandle;
    stlChar *          sSlot = NULL;
    stlUInt8           sRtsSeq;
    stlInt16           sSlotSeq;
    stlUInt32          sAttr = SMXM_ATTR_REDO;
    smlRid             sSegRid;
    stlBool            sUseInsertablePageHint;
    stlUInt16          sRetryCnt = 0;
    stlInt32           sState = 1;
    void             * sSegmentHandle = SME_GET_SEGMENT_HANDLE(aRelationHandle);
    stlBool            sIsNewPage;
    smeHint          * sRelHint;
    stlInt16           sPctFreeSpace = (SMP_PAGE_SIZE * SMD_GET_PCTFREE(aRelationHandle)) / 100;
    smsSearchHint      sSearchHint;
    smdmphLogicalHdr   sLogicalHdr;

    sSegRid = smsGetSegRid( sSegmentHandle );
    SMS_INIT_SEARCH_HINT( &sSearchHint );

    sRelHint = smeFindRelHint( aRelationHandle,
                               smsGetValidScn(sSegmentHandle),
                               aEnv );
    
    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_USE_INSERTABLE_PAGE_HINT,
                                      &sUseInsertablePageHint,
                                      KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    while( STL_TRUE )
    {
        if( sRetryCnt > 0 )
        {
            sState = 0;
            STL_TRY( smxmCommit( aMiniTrans, aEnv ) == STL_SUCCESS );
            STL_ASSERT( sRetryCnt < SMDMPH_MAXIMUM_INSERTABLE_PAGE_SEARCH_COUNT + 1 );
            STL_TRY( smxmBegin( aMiniTrans,
                                aTransId,
                                sSegRid,
                                sAttr,
                                aEnv ) == STL_SUCCESS );
            sState = 1;

            SML_SESS_ENV(aEnv)->mRelationRetryCount++;

            /**
             * Insertable Page라 하더라도 넣지 못하는 경우가 있다.
             */
            
            if( sRelHint != NULL )
            {
                sRelHint->mHintPid = SMP_NULL_PID;
            }
        }

        if( (sUseInsertablePageHint == STL_FALSE) ||
            (sRelHint == NULL) ||
            (sRelHint->mHintPid == SMP_NULL_PID) )
        {
            if( sRetryCnt < SMDMPH_MAXIMUM_INSERTABLE_PAGE_SEARCH_COUNT )
            {
                STL_TRY( smsGetInsertablePage( aMiniTrans,
                                               sSegmentHandle,
                                               SMP_PAGE_TYPE_TABLE_DATA,
                                               smdmphIsAgable,
                                               &sSearchHint,
                                               &sPageHandle,
                                               &sIsNewPage,
                                               aEnv ) == STL_SUCCESS );
            }
            else
            {
                STL_TRY( smsAllocPage( aMiniTrans,
                                       sSegmentHandle,
                                       SMP_PAGE_TYPE_TABLE_DATA,
                                       smdmphIsAgable,
                                       &sPageHandle,
                                       aEnv ) == STL_SUCCESS );
                sIsNewPage = STL_TRUE;
            }

            if( sRelHint == NULL )
            {
                sRelHint = smeAddNewRelHint( smpGetPageId( &sPageHandle ),
                                             aRelationHandle,
                                             smsGetValidScn(sSegmentHandle),
                                             aEnv );
            }
            else
            {
                sRelHint->mHintPid = smpGetPageId( &sPageHandle );
            }
        }
        else
        {
            STL_TRY( smpAcquire( aMiniTrans,
                                 smsGetTbsId( sSegmentHandle ),
                                 sRelHint->mHintPid,
                                 KNL_LATCH_MODE_EXCLUSIVE,
                                 &sPageHandle,
                                 aEnv ) == STL_SUCCESS );
            sIsNewPage = STL_FALSE;
        }

        if( smpGetFreeness( &sPageHandle ) > SMP_FREENESS_INSERTABLE )
        {
            /**
             * 오래된 hint로 인하여 지워진 페이지를 참조할수도 있다.
             */
            
            if( smpIsAgable( &sPageHandle, smdmphIsAgable, aEnv ) == STL_TRUE )
            {
                STL_TRY( smdmphUpdatePageStatus( aMiniTrans,
                                                 aRelationHandle,
                                                 sSegmentHandle,
                                                 &sPageHandle,
                                                 SMP_FREENESS_INSERTABLE,
                                                 0,  /* aScn */
                                                 aEnv ) == STL_SUCCESS );
            }
            else
            {
                /**
                 * FREE 페이지를 참조하고 있는 트랜잭션이 존재할수도 있기 때문에
                 * 다시 시도한다.
                 */
                sRetryCnt++;
                continue;
            }
        }

        if( sIsNewPage == STL_TRUE )
        {
            stlMemset( &sLogicalHdr, 0x00, STL_SIZEOF(smdmphLogicalHdr) );
            
            sLogicalHdr.mSimplePage = STL_TRUE;
            sLogicalHdr.mSegmentSeq = smsGetValidSeqFromHandle( sSegmentHandle );
            
            STL_TRY( smpInitBody( &sPageHandle,
                                  SMP_ORDERED_SLOT_TYPE,
                                  STL_SIZEOF(smdmphLogicalHdr),
                                  (stlChar*)&sLogicalHdr,
                                  SMD_GET_INITRANS(aRelationHandle),
                                  SMD_GET_MAXTRANS(aRelationHandle),
                                  STL_TRUE,
                                  aMiniTrans,
                                  aEnv) == STL_SUCCESS );
        }

        if( smpGetFreeness( &sPageHandle ) < SMP_FREENESS_INSERTABLE )
        {
            sRetryCnt++;
            continue;
        }

        if( smpGetUnusedSpace(SMP_FRAME(&sPageHandle)) < aRowSize + STL_SIZEOF(smpOffsetSlot) )
        {
            if( smpGetReclaimedSpace(SMP_FRAME(&sPageHandle)) >= (aRowSize + STL_SIZEOF(smpOffsetSlot)) )
            {
                /* compact하면 충분한 공간이 생길 수도 있음 */
                STL_TRY( smdmphCompactPage( aMiniTrans,
                                            &sPageHandle,
                                            aEnv )
                         == STL_SUCCESS );
            }
            
            if( (smpGetUnusedSpace( SMP_FRAME(&sPageHandle) ) < sPctFreeSpace) &&
                (SMP_GET_PAGE_ROW_ITEM_COUNT(&sPageHandle) > 0) )
            {
                /* compact를 했는데도 사용중인 공간이 pctfree공간을 침범한 경우 */
                if( smpGetReclaimedSpace(SMP_FRAME(&sPageHandle)) == 0 )
                {
                    /* Updatable로 변경시키고 다음 페이지로 이동한다 */
                    STL_TRY( smdmphUpdatePageStatus( aMiniTrans,
                                                     aRelationHandle,
                                                     sSegmentHandle,
                                                     &sPageHandle,
                                                     SMP_FREENESS_UPDATEONLY,
                                                     0,  /* aScn */
                                                     aEnv ) == STL_SUCCESS );
                }
                else
                {
                    /* Insertable 상태로 두고 다음 페이지로 진행한다 */
                }
                sRetryCnt++;
                continue;
            }
            else
            {
                /* 현재 페이지가 사용 가능하다 */
            }
        }

        /* DS  mode가 아니면 */
        if( SMXL_IS_UNDO_BOUND_TRANS( aTransId ) == STL_TRUE )
        {
            STL_TRY( smpAllocRts( NULL,
                                  &sPageHandle,
                                  aTransId,
                                  SML_MAKE_VIEW_SCN(aViewScn),
                                  smdmphStabilizeRow,
                                  &sRtsSeq,
                                  aEnv ) == STL_SUCCESS );

            if( SMP_IS_VALID_RTS_SEQ(sRtsSeq) != STL_TRUE )
            {
                sRtsSeq = SMP_RTS_SEQ_NULL;
            }
        }
        else
        {
            sRtsSeq = SMP_RTS_SEQ_NULL;
        }

        STL_TRY( smpAllocSlot( &sPageHandle,
                               aRowSize,
                               STL_TRUE,
                               &sSlot,
                               &sSlotSeq ) == STL_SUCCESS );

        if( sSlot == NULL )
        {
            if( smpGetReclaimedSpace(SMP_FRAME(&sPageHandle)) == 0 )
            {
                STL_TRY( smdmphUpdatePageStatus( aMiniTrans,
                                                 aRelationHandle,
                                                 sSegmentHandle,
                                                 &sPageHandle,
                                                 SMP_FREENESS_UPDATEONLY,
                                                 0,  /* aScn */
                                                 aEnv ) == STL_SUCCESS );
            }
            sRetryCnt++;
            continue;
        }

        /* 모든 할당에 성공했으므로 루프에서 나간다 */
        break;
    }

    *aPageHandle = sPageHandle;
    *aRtsSeq = sRtsSeq;
    *aSlotSeq = sSlotSeq;
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 0 )
    {
        (void) smxmBegin( aMiniTrans, aTransId, sSegRid, sAttr, aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief 주어진 continuous column을 담은 row piece를 삽입한다
 * @param[in] aStatement 사용중인 statement
 * @param[in] aRelationHandle 삽입할 Relation의 handle
 * @param[in] aValueIdx 삽입할 row의 value block내에서의 순번
 * @param[in] aContCol 삽입할 Continuous column 정보
 * @param[in] aIsMaster Master인지 여부
 * @param[in] aNextRid 본 컬럼 바로 다음의 row piece의 위치
 * @param[in] aIsForUpdate Update 중 delete/insert에 의한 호출인지 여부
 * @param[out] aFirstRowRid 삽입된 첫 row piece의 위치
 * @param[out] aLastRowRid 삽입된 마지막 row piece의 위치
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmphInsertContCol( smlStatement      * aStatement,
                               void              * aRelationHandle,
                               stlInt32            aValueIdx,
                               knlValueBlockList * aContCol,
                               stlBool             aIsMaster,
                               smlRid            * aNextRid,
                               stlBool             aIsForUpdate,
                               smlRid            * aFirstRowRid,
                               smlRid            * aLastRowRid,
                               smlEnv            * aEnv )
{
    dtlDataValue     * sDataValue = KNL_GET_BLOCK_DATA_VALUE(aContCol, aValueIdx);
    smxlTransId        sTransId = SMA_GET_TRANS_ID( aStatement );
    smlScn             sViewScn = smxlGetTransViewScn( sTransId );
    smlRid             sPrevRid = (smlRid){0,0,SMP_NULL_PID};
    stlChar            sRowBuf[SMP_PAGE_SIZE];
    stlChar          * sRowPtr;
    stlChar          * sDataPtr;
    stlInt64           sColLen;
    stlUInt8           sColLenSize;
    stlInt16           sMaxPieceSize;
    stlInt64           sToOffset;
    stlInt64           sFromOffset = sDataValue->mLength;
    stlBool            sIsLast = STL_TRUE;
    stlUInt32          sColOrd = KNL_GET_BLOCK_COLUMN_ORDER( aContCol );
    stlInt16           sBasePadSpace = SMDMPH_MAX_ROW_HDR_LEN - SMDMPH_MIN_ROW_HDR_LEN;
    stlInt16           sPadSpace;
    stlInt16           sSlotSize;

    sMaxPieceSize = smdmphGetMaxFreeSize( aRelationHandle );

    if( aNextRid != NULL )
    {
        sPrevRid = *aNextRid;
    }
    
    /* 뒤쪽 piece부터 insert */
    sToOffset = sDataValue->mLength;
    while( sToOffset >= 0 )
    {
        /**
         * Migrate때를 대비해 최소 Row Size를 미리 확보하기 위해 
         * 미리 맨 뒤에 충분한 만큼의 기본 Pad Space를 마련해둔다
         */
        sRowPtr = sRowBuf + SMP_PAGE_SIZE - sBasePadSpace;
        while( sFromOffset >= 0 )
        {
            sColLen = sToOffset;
            SMP_GET_COLUMN_LEN_SIZE( sColLen, &sColLenSize );
            if( (sColLen + sColLenSize + SMDMPH_MAX_ROW_HDR_LEN) > sMaxPieceSize )
            {
                sColLen = (sColLen + sColLenSize + SMDMPH_MAX_ROW_HDR_LEN < sMaxPieceSize) ?
                    sColLen : (sMaxPieceSize - SMDMPH_MAX_ROW_HDR_LEN - SMP_MAX_COLUMN_LEN_SIZE);
                SMP_GET_COLUMN_LEN_SIZE( sColLen, &sColLenSize );
                sFromOffset = sToOffset - sColLen;
                /* write patial column */
                sRowPtr -= sColLen + sColLenSize;
                sDataPtr = (stlChar*)sDataValue->mValue
                    + sToOffset - sColLen;
                SMP_WRITE_COLUMN_ZERO_INSENS( sRowPtr, sDataPtr, sColLen );
                break;
            }
            else
            {
                sFromOffset = 0;
                if( sToOffset < sDataValue->mLength )
                {
                    /* write patial column */
                    sRowPtr -= sColLen + sColLenSize;
                    sDataPtr = (stlChar*)sDataValue->mValue;
                    SMP_WRITE_COLUMN_ZERO_INSENS( sRowPtr, sDataPtr, sColLen );
                    break;
                }
                else
                {
                    /* continuous column이 아님 */
                    STL_ASSERT( STL_FALSE );
                }
            }
        } /* while */

        /* write row header */
        if( sPrevRid.mPageId != SMP_NULL_PID )
        {
            sRowPtr -= SMDMPH_MAX_ROW_HDR_LEN;
        }
        else
        {
            sRowPtr -= SMDMPH_MIN_ROW_HDR_LEN;
        }
        
        /* Migrate 될 때를 대비하여 최소한의 Slot 크기를 확보한다. */
        sSlotSize = sRowBuf + SMP_PAGE_SIZE - sBasePadSpace - sRowPtr;
        if( sSlotSize < SMDMPH_MINIMUM_ROW_PIECE_SIZE )
        {
            sPadSpace = SMDMPH_MINIMUM_ROW_PIECE_SIZE - sSlotSize;
        }
        else
        {
            sPadSpace = 0;
        }
        sSlotSize += sPadSpace;

        STL_TRY( smdmphWriteRowHeader( sRowPtr,
                                       SMP_RTS_SEQ_NULL,  /* 나중에 다시 세팅 */
                                       SML_INFINITE_SCN,  /* 나중에 다시 세팅 */
                                       aStatement->mTcn,
                                       sTransId,
                                       NULL, /* &sUndoRid를 적을 필요 없음 */
                                       1,
                                       sPadSpace,
                                       &sPrevRid ) == STL_SUCCESS );

        if( (sFromOffset > 0) || (KNL_GET_BLOCK_COLUMN_ORDER(aContCol) > 0) )
        {
            SMDMPH_UNSET_MASTER_ROW( sRowPtr );
        }
        else
        {
            if( aIsMaster == STL_TRUE )
            {
                SMDMPH_SET_MASTER_ROW( sRowPtr );
            }
            else
            {
                SMDMPH_UNSET_MASTER_ROW( sRowPtr );
            }
        }

        if( sIsLast != STL_TRUE )
        {
            SMDMPH_SET_CONT_COL( sRowPtr );
        }
        else
        {
            SMDMPH_SET_CONT_COL_LAST( sRowPtr );
        }

        /* sSlotSize에 Migrate를 대비한 Pad space가 최대 4byte정도 더 들어갈 수 있음 */
        STL_TRY( smdmphInsertRowPiece( aRelationHandle,
                                       sTransId,
                                       sViewScn,
                                       sRowPtr,
                                       sSlotSize,
                                       sColOrd,
                                       aIsForUpdate,
                                       &sPrevRid,
                                       aEnv ) == STL_SUCCESS );
        if( sIsLast ==  STL_TRUE )
        {
            *aLastRowRid = sPrevRid;
            sIsLast = STL_FALSE;
        }

        if( sFromOffset == 0 )
        {
            /* 완료했음 */
            break;
        }
        sToOffset = sFromOffset;
    } /* while */

    *aFirstRowRid = sPrevRid;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
} 


/**
 * @brief 주어진 relation에 새 row를 삽입한다
 * @param[in] aStatement 사용중인 statement
 * @param[in] aRelationHandle row를 삽입할 Relation의 handle
 * @param[in] aValueIdx 삽입할 row의 value block내에서의 순번
 * @param[in] aInsertCols 삽입할 row의 value block
 * @param[out] aUniqueViolation 삽입할 row가 unique check에 걸려 insert되지 않았는지의 여부; normal table은 사용하지 않음.
 * @param[out] aRowRid 삽입한 row의 위치
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmphInsert( smlStatement      * aStatement,
                        void              * aRelationHandle,
                        stlInt32            aValueIdx,
                        knlValueBlockList * aInsertCols,
                        knlValueBlockList * aUniqueViolation,
                        smlRid            * aRowRid,
                        smlEnv            * aEnv )
{
    knlValueBlockList ** sColPtrArray;
    knlValueBlockList * sCurCol = aInsertCols;
    stlUInt32          sState = 0;
    smxlTransId        sTransId = SMA_GET_TRANS_ID( aStatement );
    smlScn             sViewScn = smxlGetTransViewScn( sTransId );
    void             * sSegmentHandle = SME_GET_SEGMENT_HANDLE(aRelationHandle);
    smlRid             sPrevRid = ((smlRid){ smsGetTbsId(sSegmentHandle), 0, SMP_NULL_PID });
    stlChar            sRowBuf[SMP_PAGE_SIZE];
    stlChar          * sRowPtr;
    knlRegionMark      sMemMark;
    stlInt32           sTotalColCount = 0;
    stlInt16           sPieceColCount = 0;
    stlInt64           sColLen;
    stlUInt8           sColLenSize;
    stlInt16           sTotalRowSize;
    stlInt32           i;
    stlInt16           sMaxPieceSize;
    stlInt32           sToOrd;
    stlInt32           sFromOrd;
    smlRid             sTmpFirstRowRid;
    smlRid             sTmpLastRowRid;
    stlInt16           sBasePadSpace = SMDMPH_MAX_ROW_HDR_LEN - SMDMPH_MIN_ROW_HDR_LEN;
    stlInt16           sSlotSize;
    stlInt16           sPadSpace;
    stlBool            sIsZero;
    dtlDataValue     * sDataValue;

    sMaxPieceSize = smdmphGetMaxFreeSize( aRelationHandle );
    
    while( sCurCol != NULL )
    {
        sTotalColCount++;
        KNL_VALIDATE_DATA_VALUE( sCurCol, aValueIdx, aEnv );
        sCurCol = sCurCol->mNext;
    }

    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                sTotalColCount * STL_SIZEOF(knlValueBlockList*),
                                (void**)&sColPtrArray,
                                (knlEnv*)aEnv ) == STL_SUCCESS );

    sCurCol = aInsertCols;
    for( i = 0; i < sTotalColCount; i++ )
    {
        sColPtrArray[i] = sCurCol;
        sCurCol = sCurCol->mNext;
    }

    /* 마지막 컬럼부터 insert */
    sToOrd = sTotalColCount - 1;
    sTotalRowSize = SMDMPH_MIN_ROW_HDR_LEN;
    while( sToOrd >= 0 )
    {
        /**
         * Migrate때를 대비해 최소 Row Size를 미리 확보하기 위해 
         * 미리 맨 뒤에 충분한 만큼의 기본 Pad Space를 마련해둔다
         */
        sRowPtr = sRowBuf + SMP_PAGE_SIZE - sBasePadSpace;
        sFromOrd = sToOrd;
        while( sFromOrd >= 0 )
        {
            sIsZero = STL_FALSE;
            sDataValue = KNL_GET_BLOCK_DATA_VALUE( sColPtrArray[sFromOrd], aValueIdx );
            sColLen = sDataValue->mLength;

            if( DTL_NUMERIC_IS_ZERO(sDataValue->mValue, sColLen) )
            {
                if( dtlDataTypeGroup[sDataValue->mType] == DTL_GROUP_NUMBER )
                {
                    sColLen = 0;
                    sIsZero = STL_TRUE;
                }
            }
            
            SMP_GET_COLUMN_LEN_SIZE( sColLen, &sColLenSize );
            
            if( SMDMPH_CONT_SIZE( sColLen, sColLenSize ) > sMaxPieceSize )
            {
                if( sFromOrd == sToOrd )
                {
                    /* continuous column */
                    STL_TRY( smdmphInsertContCol( aStatement,
                                                  aRelationHandle,
                                                  aValueIdx,
                                                  sColPtrArray[sFromOrd],
                                                  (sFromOrd == 0) ? STL_TRUE : STL_FALSE,
                                                  &sPrevRid,
                                                  STL_FALSE, /* aIsForUpdate */
                                                  &sTmpFirstRowRid,
                                                  &sTmpLastRowRid,
                                                  aEnv ) == STL_SUCCESS );
                    sPrevRid = sTmpFirstRowRid;
                    STL_THROW( RAMP_GOTO_PREV_COL );

                }
                else
                {
                    /* 바로 다음 컬럼부터 row 작성 */
                    sFromOrd++;
                }
                break;
            }
            else
            {
                /* atomic column */
                if( ((sTotalRowSize + sColLen + sColLenSize) > sMaxPieceSize) ||
                    (sPieceColCount >= SMD_MAX_COLUMN_COUNT_IN_PIECE) )
                {
                    /* 바로 다음 컬럼부터 row 작성 */
                    STL_ASSERT( sFromOrd < sToOrd );
                    sFromOrd++;
                    break;
                }
                /* write column */
                sRowPtr -= sColLen + sColLenSize;
                SMP_WRITE_COLUMN( sRowPtr, sDataValue->mValue, sColLen, sIsZero );
                sTotalRowSize += sColLen + sColLenSize;
                sFromOrd--;
                sPieceColCount++;
            }
        } /* while */

        if( sFromOrd == -1 )
        {
            /* 맨 첫 컬럼까지 다 처리했음 */
            sFromOrd = 0;
        }

        /* write row header */
        if( sPrevRid.mPageId != SMP_NULL_PID )
        {
            sRowPtr -= SMDMPH_MAX_ROW_HDR_LEN;
        }
        else
        {
            sRowPtr -= SMDMPH_MIN_ROW_HDR_LEN;
        }

        sSlotSize = sRowBuf + SMP_PAGE_SIZE - sBasePadSpace - sRowPtr;
        if( sSlotSize < SMDMPH_MINIMUM_ROW_PIECE_SIZE )
        {
            sPadSpace = SMDMPH_MINIMUM_ROW_PIECE_SIZE - sSlotSize;
        }
        else
        {
            sPadSpace = 0;
        }
        sSlotSize += sPadSpace;

        STL_TRY( smdmphWriteRowHeader( sRowPtr,
                                       SMP_RTS_SEQ_NULL,  /* 나중에 다시 세팅 */
                                       SML_INFINITE_SCN,
                                       aStatement->mTcn,
                                       sTransId,
                                       NULL, /* &sUndoRid를 적을 필요 없음 */
                                       sPieceColCount,
                                       sPadSpace,
                                       &sPrevRid ) == STL_SUCCESS );

        if( sFromOrd > 0 )
        {
            SMDMPH_UNSET_MASTER_ROW( sRowPtr );
        }
        else
        {
            SMDMPH_SET_MASTER_ROW( sRowPtr );
        }
        SMDMPH_UNSET_CONT_COL( sRowPtr );

        STL_TRY( smdmphInsertRowPiece( aRelationHandle,
                                       sTransId,
                                       sViewScn,
                                       sRowPtr,
                                       sSlotSize,
                                       sFromOrd,
                                       STL_FALSE, /* aIsForUpdate */
                                       &sPrevRid,
                                       aEnv ) == STL_SUCCESS );

        STL_RAMP( RAMP_GOTO_PREV_COL );

        sToOrd = sFromOrd - 1;
        sPieceColCount = 0;
        sTotalRowSize = SMDMPH_MAX_ROW_HDR_LEN;
    } /* while */

    *aRowRid = sPrevRid;

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       (knlEnv*)aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            {
                (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                                &sMemMark,
                                                STL_FALSE, /* aFreeChunk */
                                                (knlEnv*)aEnv );
            }
    }

    return STL_FAILURE;
} 


/**
 * @brief 주어진 relation에 새 row를 삽입한다
 * @param[in] aStatement 사용중인 statement
 * @param[in] aRelationHandle row를 삽입할 Relation의 handle
 * @param[in] aInsertCols 삽입할 row의 value block
 * @param[out] aUniqueViolation 삽입할 row가 unique check에 걸려 insert되지 않았는지의 여부; normal table은 사용하지 않음.
 * @param[in] aRowBlock Row Block 포인터
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmphBlockInsert( smlStatement      * aStatement,
                             void              * aRelationHandle,
                             knlValueBlockList * aInsertCols,
                             knlValueBlockList * aUniqueViolation,
                             smlRowBlock       * aRowBlock,
                             smlEnv            * aEnv )
{
    knlValueBlockList ** sColPtrArray;
    knlValueBlockList  * sCurCol = aInsertCols;
    stlUInt32            sState = 0;
    knlRegionMark        sMemMark;
    stlInt16             sTotalColCount = 0;
    stlInt64             sColLen;
    stlUInt8             sColLenSize;
    stlInt16             sTotalRowSize;
    stlInt32             i;
    stlInt16             sMaxPieceSize;
    stlInt32             sBlockIdx;
    stlInt32             sStartBlockIdx;
    stlInt32             sEndBlockIdx;
    smlRid               sRowRid;
    dtlDataValue       * sDataValue;

    sMaxPieceSize = smdmphGetMaxFreeSize( aRelationHandle );
    
    while( sCurCol != NULL )
    {
        sTotalColCount++;
        sCurCol = sCurCol->mNext;
    }

    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                sTotalColCount * STL_SIZEOF(knlValueBlockList*),
                                (void**)&sColPtrArray,
                                (knlEnv*)aEnv ) == STL_SUCCESS );

    sCurCol = aInsertCols;
    for( i = 0; i < sTotalColCount; i++ )
    {
        sColPtrArray[i] = sCurCol;
        sCurCol = sCurCol->mNext;
    }

    sStartBlockIdx = 0;
    sEndBlockIdx = -1;

    if( sTotalColCount <= SMD_MAX_COLUMN_COUNT_IN_PIECE )
    {
        sStartBlockIdx = KNL_GET_BLOCK_SKIP_CNT( aInsertCols );
        
        for( sBlockIdx = sStartBlockIdx;
             sBlockIdx < KNL_GET_BLOCK_USED_CNT( aInsertCols );
             sBlockIdx++ )
        {
            sTotalRowSize = SMDMPH_MIN_ROW_HDR_LEN;

            for( i = 0; i < sTotalColCount; i++ )
            {
                sDataValue = KNL_GET_BLOCK_DATA_VALUE( sColPtrArray[i], sBlockIdx );
                sColLen = sDataValue->mLength;
                
                if( DTL_NUMERIC_IS_ZERO(sDataValue->mValue, sColLen) )
                {
                    if( dtlDataTypeGroup[sDataValue->mType] == DTL_GROUP_NUMBER )
                    {
                        sColLen = 0;
                    }
                }
            
                SMP_GET_COLUMN_LEN_SIZE( sColLen, &sColLenSize );
                KNL_VALIDATE_DATA_VALUE( sColPtrArray[i], sBlockIdx, aEnv );
            
                if( SMDMPH_CONT_SIZE( sColLen, sColLenSize ) > sMaxPieceSize )
                {
                    break;
                }
                else
                {
                    if( (sTotalRowSize + sColLen + sColLenSize) > sMaxPieceSize )
                    {
                        break;
                    }

                    sTotalRowSize += sColLen + sColLenSize;
                }                
            }

            /**
             * 모든 컬럼을 한 페이지에 저장 가능한지 검사
             */
        
            if( i == sTotalColCount )
            {
                sEndBlockIdx = sBlockIdx;
                continue;
            }

            /**
             * 이전 레코드들을 Block 형태 삽입한다.
             */

            if( sStartBlockIdx < sBlockIdx )
            {
                STL_TRY( smdmphBlockInsertInternal( aStatement,
                                                    aRelationHandle,
                                                    aInsertCols,
                                                    sStartBlockIdx,
                                                    sEndBlockIdx,
                                                    aRowBlock,
                                                    sColPtrArray,
                                                    sTotalColCount,
                                                    aEnv )
                         == STL_SUCCESS );
            }
        
            /**
             * 현재 레코드를 Normal Insert로 삽입한다.
             */

            STL_TRY( smdmphInsert( aStatement,
                                   aRelationHandle,
                                   sBlockIdx,
                                   aInsertCols,
                                   aUniqueViolation,
                                   &sRowRid,
                                   aEnv )
                     == STL_SUCCESS );
        
            SML_SET_RID_VALUE( aRowBlock, sBlockIdx, sRowRid );

            sStartBlockIdx = sBlockIdx + 1;
        }

        /**
         * 남아 있는 레코드들을 삽입한다.
         */
        if( (sEndBlockIdx - sStartBlockIdx) >= 0 )
        {
            if( (sEndBlockIdx - sStartBlockIdx) == 0 )
            {
                /**
                 * 삽입할 레코드가 1개라면 Normal Insert로 삽입한다.
                 */

                STL_TRY( smdmphInsert( aStatement,
                                       aRelationHandle,
                                       sStartBlockIdx,
                                       aInsertCols,
                                       aUniqueViolation,
                                       &sRowRid,
                                       aEnv )
                         == STL_SUCCESS );

                SML_SET_RID_VALUE( aRowBlock, sStartBlockIdx, sRowRid );
            }
            else
            {
                /**
                 * 삽입할 레코드가 1개 이상이라면 Block Insert로 삽입한다.
                 */
            
                STL_TRY( smdmphBlockInsertInternal( aStatement,
                                                    aRelationHandle,
                                                    aInsertCols,
                                                    sStartBlockIdx,
                                                    sEndBlockIdx,
                                                    aRowBlock,
                                                    sColPtrArray,
                                                    sTotalColCount,
                                                    aEnv )
                         == STL_SUCCESS );
            }
        }
    }
    else
    {
        for( sBlockIdx = KNL_GET_BLOCK_SKIP_CNT( aInsertCols );
             sBlockIdx < KNL_GET_BLOCK_USED_CNT( aInsertCols );
             sBlockIdx++ )
        {
            STL_TRY( smdmphInsert( aStatement,
                                   aRelationHandle,
                                   sBlockIdx,
                                   aInsertCols,
                                   aUniqueViolation,
                                   &sRowRid,
                                   aEnv )
                     == STL_SUCCESS );

            SML_SET_RID_VALUE( aRowBlock, sBlockIdx, sRowRid );
        }
    }

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       (knlEnv*)aEnv ) == STL_SUCCESS );
    
    SML_SET_USED_BLOCK_SIZE( aRowBlock, sBlockIdx );
 
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            {
                (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                                &sMemMark,
                                                STL_FALSE, /* aFreeChunk */
                                                (knlEnv*)aEnv );
            }
    }

    return STL_FAILURE;
} 


/**
 * @brief 주어진 relation에 N개의 New row를 삽입한다
 * @param[in] aStatement 사용중인 statement
 * @param[in] aRelationHandle row를 삽입할 Relation의 handle
 * @param[in] aInsertCols 삽입할 row의 value block
 * @param[in] aStartBlockIdx 삽입할 row의 첫번째 value block
 * @param[in] aEndBlockIdx 삽입할 row의 마지막 value block
 * @param[in] aRowBlock Row Block 포인터
 * @param[in] aColPtrArray Column Value 포인터 배열
 * @param[in] aColCount 총 컬럼 개수
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmphBlockInsertInternal( smlStatement       * aStatement,
                                     void               * aRelationHandle,
                                     knlValueBlockList  * aInsertCols,
                                     stlInt32             aStartBlockIdx,
                                     stlInt32             aEndBlockIdx,
                                     smlRowBlock        * aRowBlock,
                                     knlValueBlockList ** aColPtrArray,
                                     stlInt16             aColCount,
                                     smlEnv             * aEnv )
{
    stlUInt32          sState = 0;
    smxlTransId        sTransId = SMA_GET_TRANS_ID( aStatement );
    smlScn             sViewScn = smxlGetTransViewScn( sTransId );
    void             * sSegmentHandle = SME_GET_SEGMENT_HANDLE(aRelationHandle);
    smlRid             sPrevRid = ((smlRid){ smsGetTbsId(sSegmentHandle), 0, SMP_NULL_PID });
    stlChar            sRowBuf[SMP_PAGE_SIZE];
    stlChar          * sRowPtr;
    stlChar          * sRowHeader;
    stlInt64           sColLen;
    stlUInt8           sColLenSize;
    stlInt16           sTotalRowSize;
    stlInt32           i;
    smpHandle          sPageHandle;
    smlRid             sUndoRid;
    stlChar            sLogBuf[SMP_PAGE_SIZE];
    stlChar          * sLogPtr;
    stlChar *          sSlot = NULL;
    stlUInt8           sRtsSeq = SMP_RTS_SEQ_NULL;
    stlUInt8           sRtsVerNo;
    stlInt16           sSlotSeq;
    stlUInt32          sAttr = SMXM_ATTR_REDO;
    smlRid             sSegRid;
    smxmTrans          sMiniTrans;
    smlScn             sScn;
    stlBool            sInsertablePage = STL_FALSE;
    smlRid             sRowRid;
    stlInt32           sBlockIdx;
    dtlDataValue     * sDataValue;
    stlInt32           sFirstColOrd = 0;
    stlInt16           sPadSpace;
    stlInt16           sRowOffsetArray[SMP_PAGE_SIZE / SMDMPH_MINIMUM_ROW_PIECE_SIZE];
    stlInt16           sArrayIdx = 0;
    smpCtrlHeader      sPageStack[KNL_ENV_MAX_LATCH_DEPTH];
    smxmTrans          sSnapshotMtx;
    stlBool            sIsZero;
#if defined( STL_DEBUG )
    stlInt16           sMaxPieceSize = smdmphGetMaxFreeSize( aRelationHandle );
#endif
    
    sSegRid = smsGetSegRid( sSegmentHandle );

    /* Redo log 보다 Undo log를 나중에 쓰기 때문에 Tx record를 미리 쓴다 */ 
    STL_TRY( smxlInsertTransRecord( sTransId, aEnv ) == STL_SUCCESS );

    STL_TRY( smxmBegin( &sMiniTrans,
                        sTransId,
                        sSegRid,
                        sAttr,
                        aEnv )
             == STL_SUCCESS );
    sState = 1;

    for( sBlockIdx = aStartBlockIdx; sBlockIdx <= aEndBlockIdx; sBlockIdx++ )
    {
        sTotalRowSize = SMDMPH_MIN_ROW_HDR_LEN;
            
        /**
         * Migrate때를 대비해 최소 Row Size를 미리 확보하기 위해 
         * 미리 맨 뒤에 충분한 만큼의 기본 Pad Space를 마련해둔다
         */
        sRowHeader = sRowBuf;
        sRowPtr = sRowHeader + SMDMPH_MIN_ROW_HDR_LEN;
        
        for( i = 0; i < aColCount; i++ )
        {
            sIsZero = STL_FALSE;
            sDataValue = KNL_GET_BLOCK_DATA_VALUE( aColPtrArray[i], sBlockIdx );
            sColLen = sDataValue->mLength;

            if( DTL_NUMERIC_IS_ZERO(sDataValue->mValue, sColLen) )
            {
                if( dtlDataTypeGroup[sDataValue->mType] == DTL_GROUP_NUMBER )
                {
                    sColLen = 0;
                    sIsZero = STL_TRUE;
                }
            }
            
            SMP_GET_COLUMN_LEN_SIZE( sColLen, &sColLenSize );

            STL_DASSERT( SMDMPH_CONT_SIZE( sColLen, sColLenSize ) < sMaxPieceSize );
            STL_DASSERT( sTotalRowSize + sColLen + sColLenSize <= sMaxPieceSize );
            
            /* write column */
            SMP_WRITE_COLUMN( sRowPtr, (stlChar*)sDataValue->mValue, sColLen, sIsZero );
            sRowPtr += sColLen + sColLenSize;
            
            sTotalRowSize += sColLen + sColLenSize;
        }
        
        if( sTotalRowSize < SMDMPH_MINIMUM_ROW_PIECE_SIZE )
        {
            sPadSpace = SMDMPH_MINIMUM_ROW_PIECE_SIZE - sTotalRowSize;
        }
        else
        {
            sPadSpace = 0;
        }
        sTotalRowSize += sPadSpace;

        while( STL_TRUE )
        {
            if( sInsertablePage == STL_FALSE )
            {
                STL_TRY( smdmphGetInsertablePage( aRelationHandle,
                                                  &sMiniTrans,
                                                  sTransId,
                                                  sViewScn,
                                                  sTotalRowSize,
                                                  &sPageHandle,
                                                  &sRtsSeq,
                                                  &sSlotSeq,
                                                  aEnv )
                         == STL_SUCCESS );

                /**
                 * NEW 페이지를 할당한 경우에 Mini-transaction rollback이 발생한 경우는
                 * INIT_PAGE_BODY로그가 기록되지 않아 recovery시에 문제가 발생할수 있다.
                 * - 지금까지의 로그를 기록하고 다시 시작한다.
                 */

                sState = 0;
                STL_TRY( smxmSnapshotAndCommit( &sMiniTrans,
                                                sPageStack,
                                                &sSnapshotMtx,
                                                aEnv )
                         == STL_SUCCESS );
                
                STL_TRY( smxmBeginWithSnapshot( &sMiniTrans,
                                                &sSnapshotMtx,
                                                aEnv )
                         == STL_SUCCESS );
                sState = 1;

                if( smpGetFreeness( &sPageHandle ) != SMP_FREENESS_INSERTABLE )
                {
                    /**
                     * Insertable Page를 구한 후 Mini-transaction 재시작으로 Page의 상태가 바뀔 수 있다.
                     * 만약 Insertable이 아니면 다시 retry한다.
                     */
                    sState = 0;
                    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

                    STL_TRY( smxmBegin( &sMiniTrans,
                                        sTransId,
                                        sSegRid,
                                        sAttr,
                                        aEnv ) == STL_SUCCESS );
                    sState = 1;

                    sInsertablePage = STL_FALSE;
                    continue;
                }

                STL_TRY( smdmphWriteRowHeader( sRowHeader,
                                               SMP_RTS_SEQ_NULL,  /* 나중에 다시 세팅 */
                                               SML_INFINITE_SCN,
                                               aStatement->mTcn,
                                               sTransId,
                                               NULL, /* &sUndoRid를 적을 필요 없음 */
                                               aColCount,
                                               sPadSpace,
                                               &sPrevRid ) == STL_SUCCESS );

                SMDMPH_SET_MASTER_ROW( sRowHeader );
                SMDMPH_UNSET_CONT_COL( sRowHeader );

                sInsertablePage = STL_TRUE;
                sArrayIdx = 0;
            }
            SMDMPH_SET_PAD_SPACE( sRowHeader, &sPadSpace );

            /* Insertable page를 구한 뒤 mini-transaction을 재시작하므로
             * rts와 record slot을 다시 alloc 해야 한다. */
            if( SMXL_IS_UNDO_BOUND_TRANS( sTransId ) == STL_TRUE )
            {
                STL_TRY( smpAllocRts( NULL,
                                      &sPageHandle,
                                      sTransId,
                                      SML_MAKE_VIEW_SCN(sViewScn),
                                      smdmphStabilizeRow,
                                      &sRtsSeq,
                                      aEnv ) == STL_SUCCESS );

                if( SMP_IS_VALID_RTS_SEQ(sRtsSeq) != STL_TRUE )
                {
                    sRtsSeq = SMP_RTS_SEQ_NULL;
                }
            }
            else
            {
                sRtsSeq = SMP_RTS_SEQ_NULL;
            }

            STL_TRY( smpAllocSlot( &sPageHandle,
                                   sTotalRowSize,
                                   STL_FALSE,
                                   &sSlot,
                                   &sSlotSeq ) == STL_SUCCESS );

            if( sSlot != NULL )
            {
                break;
            }

            /* write undo record */
            if( sArrayIdx > 0 )
            {
                STL_TRY( smxlInsertUndoRecord( &sMiniTrans,
                                               SMG_COMPONENT_TABLE,
                                               SMD_UNDO_LOG_MEMORY_HEAP_BLOCK_INSERT,
                                               (stlChar*)sRowOffsetArray,
                                               STL_SIZEOF(stlInt16) * sArrayIdx,
                                               sRowRid,
                                               &sUndoRid,
                                               aEnv ) == STL_SUCCESS );
            }

            sState = 0;
            STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
                
            STL_TRY( smxmBegin( &sMiniTrans,
                                sTransId,
                                sSegRid,
                                sAttr,
                                aEnv ) == STL_SUCCESS );
            sState = 1;
            sInsertablePage = STL_FALSE;
            sArrayIdx = 0;
        }

        /* Slot을 실제로 alloc한 slot seq를 row rid로 설정한다. */
        SML_MAKE_RID( &sRowRid,
                      sPageHandle.mPch->mTbsId,
                      sPageHandle.mPch->mPageId,
                      sSlotSeq );
        
        stlMemcpy( sSlot, sRowHeader, sTotalRowSize - sPadSpace );
 
        if( SMP_IS_VALID_RTS_SEQ(sRtsSeq) == STL_TRUE )
        {
            STL_TRY( smpBeginUseRts( &sPageHandle,
                                     sSlot,
                                     sTransId,
                                     SML_MAKE_VIEW_SCN(sViewScn),
                                     sRtsSeq,
                                     &sRtsVerNo,
                                     aEnv ) == STL_SUCCESS );
            SMDMPH_SET_RTS_BOUND( sSlot );
            SMDMPH_SET_RTSSEQ( sSlot, &sRtsSeq );
        }
        else
        {
            if( SMXL_IS_UNDO_BOUND_TRANS( sTransId ) == STL_TRUE )
            {
                sScn = SML_MAKE_VIEW_SCN(sViewScn);
                SMDMPH_UNSET_RTS_BOUND( sSlot );
                SMDMPH_SET_SCN( sSlot, &sScn );
            }
            else
            {
                /* DS mode임 */
                sScn = 0;
                SMDMPH_UNSET_RTS_BOUND( sSlot );
                SMDMPH_SET_SCN( sSlot, &sScn );
            }
        }

        sRowOffsetArray[sArrayIdx++] = sSlotSeq;
        smpSetMaxViewScn( &sPageHandle, sViewScn );

        /* write REDO log */
        sLogPtr = sLogBuf;
        STL_WRITE_INT64( sLogPtr, &sTransId );
        sLogPtr += STL_SIZEOF(smxlTransId);
        STL_WRITE_INT64( sLogPtr, &sViewScn );
        sLogPtr += STL_SIZEOF(smlScn);
        STL_WRITE_INT32( sLogPtr, &sFirstColOrd );
        sLogPtr += STL_SIZEOF(stlInt32);
        stlMemcpy( sLogPtr, sSlot, sTotalRowSize - sPadSpace);
        sLogPtr += sTotalRowSize - sPadSpace;

        /* block insert 시 mini-transaction에 하나의 page에 기록할 수 있는
         * 최대 레코드 수만큼의 undo log가 기록될 수 있고, slot header의
         * 최소 크기가 37 byte이므로 약 200여개의 undo log가 기록될 수 있다.*/
        STL_TRY( smxmWriteLog( &sMiniTrans,
                               SMG_COMPONENT_TABLE,
                               SMR_LOG_MEMORY_HEAP_INSERT,
                               SMR_REDO_TARGET_PAGE,
                               sLogBuf,
                               sLogPtr - sLogBuf,
                               sPageHandle.mPch->mTbsId,
                               sPageHandle.mPch->mPageId,
                               sSlotSeq,  /* Physiological redo를 하기위해 offset이 아닌 seq 사용 */
                               SMXM_LOGGING_REDO_UNDO,
                               aEnv ) == STL_SUCCESS );

        SMP_INCREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(&sPageHandle) );

        if( (smpGetFreeness(&sPageHandle) == SMP_FREENESS_INSERTABLE) &&
            (smpIsPageOverInsertLimit(SMP_FRAME(&sPageHandle),
                                      SMD_GET_PCTFREE(aRelationHandle)) == STL_TRUE) )
        {
            if( sArrayIdx > 0 )
            {
                STL_TRY( smxlInsertUndoRecord( &sMiniTrans,
                                               SMG_COMPONENT_TABLE,
                                               SMD_UNDO_LOG_MEMORY_HEAP_BLOCK_INSERT,
                                               (stlChar*)sRowOffsetArray,
                                               STL_SIZEOF(stlInt16) * sArrayIdx,
                                               sRowRid,
                                               &sUndoRid,
                                               aEnv ) == STL_SUCCESS );
            }

            STL_TRY( smdmphUpdatePageStatus( &sMiniTrans,
                                             aRelationHandle,
                                             sSegmentHandle,
                                             &sPageHandle,
                                             SMP_FREENESS_UPDATEONLY,
                                             0,  /* aScn */
                                             aEnv ) == STL_SUCCESS );
            
            sState = 0;
            STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
                
            STL_TRY( smxmBegin( &sMiniTrans,
                                sTransId,
                                sSegRid,
                                sAttr,
                                aEnv ) == STL_SUCCESS );
            sState = 1;

            sInsertablePage = STL_FALSE;
            sArrayIdx = 0;
        }
        
        SML_SET_RID_VALUE( aRowBlock, sBlockIdx, sRowRid );
    }

    if( sArrayIdx > 0 )
    {
        STL_TRY( smxlInsertUndoRecord( &sMiniTrans,
                                       SMG_COMPONENT_TABLE,
                                       SMD_UNDO_LOG_MEMORY_HEAP_BLOCK_INSERT,
                                       (stlChar*)sRowOffsetArray,
                                       STL_SIZEOF(stlInt16) * sArrayIdx,
                                       sRowRid,
                                       &sUndoRid,
                                       aEnv ) == STL_SUCCESS );
    }

    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) smxmRollback( &sMiniTrans, aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief 주어진 src row로부터 dst row로 컬럼들을 update하면서 복사한다
 * @param[in] aSrcRow source row의 시작 위치
 * @param[out] aDstRow target row의 시작 위치
 * @param[in] aColumns 복사시에 update를 수행할 컬럼들의 정보
 * @param[in] aBlockIdx update를 수행할 컬럼들의 정보들의 block idx
 * @param[in] aSrcRowPieceSize src row의 길이
 * @param[in] aTotalColumnCount 복사할 컬럼의 총 개수
 * @param[in] aFirstColOrd aSrcRow의 첫 컬럼의 순번
 * @param[in] aRedoLogPtr 컬럼을 복사하면서 작성할 redo log 버퍼의 시작 주소
 * @param[out] aRedoLogEnd 기록한 redo log의 끝
 * @param[in] aIsColSizeDiff update되는 컬럼들 중 길이가 변화되는 컬럼이 있는지 여부
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmphCopyAndUpdateColumns( stlChar            * aSrcRow,
                                      stlChar            * aDstRow,
                                      knlValueBlockList  * aColumns,
                                      stlInt32             aBlockIdx,
                                      stlInt16             aSrcRowPieceSize,
                                      stlInt16             aTotalColumnCount,
                                      stlInt32             aFirstColOrd,
                                      stlChar            * aRedoLogPtr,
                                      stlChar           ** aRedoLogEnd,
                                      stlBool              aIsColSizeDiff,
                                      smlEnv             * aEnv )
{
    knlValueBlockList * sCurCol;
    stlUInt8            sColLenSize;
    stlInt64            sColLen;
    stlInt32            i;
    stlChar           * sSrcPtr;
    stlChar           * sDstPtr;
    stlChar           * sPrevDst;
    stlInt16            sHeaderSize;
    dtlDataValue      * sDataValue;
    stlUInt8            sColOrdIdx;
    stlBool             sIsZero;

    SMDMPH_GET_ROW_HDR_SIZE( aSrcRow, &sHeaderSize );
    sSrcPtr = aSrcRow + sHeaderSize;

    for( i = aFirstColOrd; i < KNL_GET_BLOCK_COLUMN_ORDER(aColumns); i ++ )
    {
        /* 첫 update 컬럼 이전것은 일단 skip 한다 */
        SMP_GET_COLUMN_LEN( sSrcPtr, &sColLenSize, &sColLen, sIsZero );
        sSrcPtr += sColLenSize + sColLen;
    }
    if( aIsColSizeDiff == STL_TRUE )
    {
        /* outplace update를 위해 컬럼들을 한번에 복사한다 */
        stlMemcpy( aDstRow + sHeaderSize, aSrcRow + sHeaderSize, sSrcPtr - (aSrcRow + sHeaderSize) );
    }

    sDstPtr = aDstRow + (sSrcPtr - aSrcRow);
    for( i = KNL_GET_BLOCK_COLUMN_ORDER(aColumns), sCurCol = aColumns;
         (i - aFirstColOrd < aTotalColumnCount) && (sCurCol != NULL);
         i++ )
    {
        /* update column들의 범위내의 컬럼들을 복사한다 */
        SMP_GET_COLUMN_LEN( sSrcPtr, &sColLenSize, &sColLen, sIsZero );
        if( i == sCurCol->mColumnOrder )
        {
            /* DstSlot에 after value 기록 */
            sPrevDst = sDstPtr;
            sIsZero = STL_FALSE;

            sDataValue = KNL_GET_BLOCK_DATA_VALUE(sCurCol, aBlockIdx);
            
            if( DTL_NUMERIC_IS_ZERO(sDataValue->mValue, sDataValue->mLength) )
            {
                if( dtlDataTypeGroup[sDataValue->mType] == DTL_GROUP_NUMBER )
                {
                    sIsZero = STL_TRUE;
                }
            }
            
            SMP_WRITE_COLUMN_AND_MOVE_PTR( sDstPtr,
                                           sDataValue->mValue,
                                           sDataValue->mLength,
                                           sIsZero );
            sCurCol = sCurCol->mNext;

            if( aRedoLogPtr != NULL )
            {
                /* REDO log 기록 */
                sColOrdIdx = i - aFirstColOrd;
                STL_WRITE_INT8( aRedoLogPtr, &sColOrdIdx );
                aRedoLogPtr += STL_SIZEOF(stlInt8);
                stlMemcpy( aRedoLogPtr, sPrevDst, (sDstPtr - sPrevDst) );
                aRedoLogPtr += sDstPtr - sPrevDst;
            }

            /* SrcPtr 이동 */
            sSrcPtr += sColLenSize + sColLen;
        }
        else
        {
            if( aIsColSizeDiff == STL_TRUE )
            {
                /* 같은 내용을 copy */
                stlMemcpy( sDstPtr, sSrcPtr, sColLenSize + sColLen );
            }
            sSrcPtr += sColLenSize + sColLen;
            sDstPtr += sColLenSize + sColLen;
        }
    }

    if( (aIsColSizeDiff == STL_TRUE ) && (i - aFirstColOrd < aTotalColumnCount) )
    {
        /* 마지막 update column 뒤쪽을 copy */
        stlMemcpy( sDstPtr, sSrcPtr, aSrcRowPieceSize - ( sSrcPtr - aSrcRow ) );
    }

    if( aRedoLogPtr != NULL )
    {
        *aRedoLogEnd = aRedoLogPtr;
    }

    return STL_SUCCESS;
}


/**
 * @brief 주어진 row piece의 next piece rid를 갱신한다
 * @param[in] aMiniTrans 페이지 접근과 로깅시 사용할 mini transaction
 * @param[in] aTransId 현재 transaction id
 * @param[in] aViewScn 현재 transaction의 view scn
 * @param[in] aTcn 현재 statement의 tcn
 * @param[in] aTargetRid 갱신할 row의 row rid
 * @param[in] aNextRid 갱신할 row piece 다음의 row piece의 위치
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmphUpdateNextPieceRid( smxmTrans   * aMiniTrans,
                                    smxlTransId   aTransId,
                                    smlScn        aViewScn,
                                    smlTcn        aTcn,
                                    smlRid      * aTargetRid,
                                    smlRid      * aNextRid,
                                    smlEnv      * aEnv )
{
    stlChar          * sRowSlot;
    smpHandle          sPageHandle;
    smlRid             sOldNextRid;
    smlRid             sUndoRid;
    stlChar            sLogBuf[SMP_PAGE_SIZE];
    stlChar          * sLogPtr;
    stlInt16           sHdrSize;
    stlUInt8           sRtsSeq;
    stlUInt8           sRtsVerNo;
    stlUInt8           sOrgRtsSeq;
    stlInt16           sRetryCnt;
    smlScn             sCommitScn;
    smxlTransId        sWaitTransId;
    smlScn             sScn;
    smdmphLogicalHdr * sLogicalHdr;

    STL_TRY( smpAcquire( aMiniTrans,
                         aTargetRid->mTbsId,
                         aTargetRid->mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

    sRowSlot = smpGetNthRowItem( &sPageHandle, aTargetRid->mOffset );
    SMDMPH_GET_ROW_HDR_SIZE(sRowSlot, &sHdrSize);
    STL_ASSERT( SMDMPH_HAS_NEXT_LINK( sRowSlot ) == STL_TRUE );
    SMDMPH_GET_LINK_PID( sRowSlot, &sOldNextRid.mPageId );
    SMDMPH_GET_LINK_OFFSET( sRowSlot, &sOldNextRid.mOffset );

    STL_TRY( smdmphGetCommitScn( aTargetRid->mTbsId,
                                 &sPageHandle,
                                 sRowSlot,
                                 &sCommitScn,
                                 &sWaitTransId,
                                 aEnv ) == STL_SUCCESS );

    sLogPtr = sLogBuf;
    stlMemcpy( sLogPtr, sRowSlot, sHdrSize );
    if( sCommitScn < SML_INFINITE_SCN )
    {
        SMDMPH_UNSET_RTS_BOUND( sLogPtr );
        SMDMPH_SET_SCN( sLogPtr, &sCommitScn );
    }
    sLogPtr += sHdrSize;

    if( SMDMPH_IS_SAME_TRANS( aTransId,
                              SML_INFINITE_SCN, /* DML 중이라 내 커밋SCN은 무한대이다 */
                              sWaitTransId,
                              sCommitScn ) != STL_TRUE )
    {
        /* DS  mode가 아니면 */
        if( SMXL_IS_UNDO_BOUND_TRANS( aTransId ) == STL_TRUE )
        {
            sRetryCnt = 0;
            while( STL_TRUE )
            {
                STL_TRY( smpAllocRts( NULL,
                                      &sPageHandle,
                                      aTransId,
                                      SML_MAKE_VIEW_SCN(aViewScn),
                                      smdmphStabilizeRow,
                                      &sRtsSeq,
                                      aEnv ) == STL_SUCCESS );

                if( (SMP_IS_VALID_RTS_SEQ(sRtsSeq) != STL_TRUE) && (sRetryCnt == 0) )
                {
                    /* 처음으로 공간부족 발생. page를 일단 compact하여 최대한 빈 공간을 확보하고 진행 */
                    sRetryCnt++;
                    STL_TRY( smdmphCompactPage( aMiniTrans,
                                                &sPageHandle,
                                                aEnv )
                             == STL_SUCCESS );
                    
                    /* compact로 source row의 위치가 변경되었을 수 있으므로 다시 구한다 */
                    sRowSlot = smpGetNthRowItem( &sPageHandle, aTargetRid->mOffset );
                    continue;
                }

                break;
            }
        }
        else
        {
            sRtsSeq = SMP_RTS_SEQ_NULL;
        }

        /* write undo record */
        STL_TRY( smxlInsertUndoRecord( aMiniTrans,
                                       SMG_COMPONENT_TABLE,
                                       SMD_UNDO_LOG_MEMORY_HEAP_UPDATE_LINK,
                                       (void*)sLogBuf,
                                       sLogPtr - sLogBuf,
                                       *aTargetRid,
                                       &sUndoRid,
                                       aEnv ) == STL_SUCCESS );

        smpSetMaxViewScn( &sPageHandle, aViewScn );

        if( SMDMPH_IS_RTS_BOUND( sRowSlot ) == STL_TRUE )
        {
            SMDMPH_GET_RTSSEQ( sRowSlot, &sOrgRtsSeq );
        }
        else
        {
            sOrgRtsSeq = SMP_RTS_SEQ_NULL;
        }
        if( SMP_IS_VALID_RTS_SEQ(sRtsSeq) == STL_TRUE )
        {    
            /* RTS 조정 */
            STL_TRY( smpBeginUseRts( &sPageHandle,
                                     sRowSlot,
                                     aTransId,
                                     SML_MAKE_VIEW_SCN(aViewScn),
                                     sRtsSeq,
                                     &sRtsVerNo,
                                     aEnv ) == STL_SUCCESS );
            SMDMPH_SET_RTS_BOUND( sRowSlot );
            SMDMPH_SET_RTSSEQ( sRowSlot, &sRtsSeq );
        }
        else
        {
            if( SMXL_IS_UNDO_BOUND_TRANS( aTransId ) == STL_TRUE )
            {
                sScn = SML_MAKE_VIEW_SCN(aViewScn);
                SMDMPH_UNSET_RTS_BOUND( sRowSlot );
                SMDMPH_SET_SCN( sRowSlot, &sScn );
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
            STL_TRY( smpEndUseRts( &sPageHandle,
                                   sRowSlot,
                                   sOrgRtsSeq,
                                   0,
                                   aEnv ) == STL_SUCCESS );
        }
    }
    else
    {
        /* write undo record */
        STL_TRY( smxlInsertUndoRecord( aMiniTrans,
                                       SMG_COMPONENT_TABLE,
                                       SMD_UNDO_LOG_MEMORY_HEAP_UPDATE_LINK,
                                       (void*)sLogBuf,
                                       sLogPtr - sLogBuf,
                                       *aTargetRid,
                                       &sUndoRid,
                                       aEnv ) == STL_SUCCESS );
   }

    SMDMPH_SET_TCN( sRowSlot, &aTcn );
    SMDMPH_SET_TRANS_ID( sRowSlot, &aTransId );
    SMDMPH_SET_ROLLBACK_PID( sRowSlot, &sUndoRid.mPageId );
    SMDMPH_SET_ROLLBACK_OFFSET( sRowSlot, &sUndoRid.mOffset );
    SMDMPH_SET_HAS_NEXT_LINK( sRowSlot );
    SMDMPH_SET_LINK_PID( sRowSlot, &aNextRid->mPageId );
    SMDMPH_SET_LINK_OFFSET( sRowSlot, &aNextRid->mOffset );

    sLogicalHdr = SMDMPH_GET_LOGICAL_HEADER( &sPageHandle );

    if( sLogicalHdr->mSimplePage == STL_TRUE )
    {
        sLogicalHdr->mSimplePage = STL_FALSE;
        STL_TRY( smxmWriteLog( aMiniTrans,
                               SMG_COMPONENT_TABLE,
                               SMR_LOG_MEMORY_HEAP_UPDATE_LOGICAL_HDR,
                               SMR_REDO_TARGET_PAGE,
                               (stlChar*)sLogicalHdr,
                               STL_SIZEOF(smdmphLogicalHdr),
                               sPageHandle.mPch->mTbsId,
                               sPageHandle.mPch->mPageId,
                               0,
                               SMXM_LOGGING_REDO,
                               aEnv ) == STL_SUCCESS );
    }

    sLogPtr = sLogBuf;
    STL_WRITE_INT64( sLogPtr, &aTransId );
    sLogPtr += STL_SIZEOF(smxlTransId);
    STL_WRITE_INT64( sLogPtr, &aViewScn );
    sLogPtr += STL_SIZEOF(smlScn);
    stlMemcpy( sLogPtr, sRowSlot, sHdrSize );
    sLogPtr += sHdrSize;

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_TABLE,
                           SMR_LOG_MEMORY_HEAP_UPDATE_LINK,
                           SMR_REDO_TARGET_PAGE,
                           (void*)sLogBuf,
                           sLogPtr - sLogBuf,
                           aTargetRid->mTbsId,
                           aTargetRid->mPageId,
                           aTargetRid->mOffset,  /* Physiological redo를 하기위해 offset이 아닌 seq 사용 */
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 row piece의 컬럼들을 갱신하여 새로 생성한다
 * @param[in] aStatement 사용하는 statement
 * @param[in] aRelationHandle 갱신할 row piece가 존재하는 relation의 핸들
 * @param[in] aCopiedRowSlot 갱신할 row의 복사본의 포인터
 * @param[in] aAfterCols 갱신할 컬럼의 이후 이미지 정보들
 * @param[in] aValueIdx Value block에서 현재 처리할 row의 block 번호
 * @param[in] aPieceColCount 갱신할 row piece내에 존재하는 컬럼들의 개수
 * @param[in] aFirstColOrd 갱신할 row piece내의 첫 컬럼의 column order
 * @param[in] aPrevPieceRid 갱신할 row piece 바로 이전의 piece의 위치
 * @param[in] aNextPieceRid 갱신할 row piece 다음의 row piece의 위치
 * @param[out] aFirstRowRid 갱신된 첫 row piece의 위치
 * @param[out] aLastRowRid 갱신된 마지막 row piece의 위치
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmphRemakeUpdatedRowPiece( smlStatement      * aStatement,
                                       void              * aRelationHandle,
                                       stlChar           * aCopiedRowSlot,
                                       knlValueBlockList * aAfterCols,
                                       stlInt32            aValueIdx,
                                       stlInt16            aPieceColCount,
                                       stlInt32            aFirstColOrd,
                                       smlRid            * aPrevPieceRid,
                                       smlRid            * aNextPieceRid,
                                       smlRid            * aFirstRowRid,
                                       smlRid            * aLastRowRid,
                                       smlEnv            * aEnv )
{
    smxmTrans           sMiniTrans;
    stlChar             sRowBuf[SMP_PAGE_SIZE];
    stlChar           * sRowPtr;
    stlChar           * sRowStartPtr;
    stlUInt32           sAttr = SMXM_ATTR_REDO;
    void              * sSegmentHandle = SME_GET_SEGMENT_HANDLE(aRelationHandle);
    stlInt32            sState = 0;
    smlRid              sPrevPieceRid;
    stlInt16            sMaxPieceSize;
    knlValueBlockList * sCurCol = aAfterCols;
    stlInt32            sCurColOrd = aFirstColOrd;
    stlInt32            sFirstColOrd;
    stlInt16            sColCnt;
    stlUInt8            sColLenSize;
    stlInt64            sColLen;
    stlChar           * sColPtr;
    stlBool             sIsContCol = STL_FALSE;
    stlBool             sIsLastCol = STL_FALSE;
    smxlTransId         sTransId = SMA_GET_TRANS_ID(aStatement);
    smlScn              sViewScn = smxlGetTransViewScn( aStatement->mTransId );
    stlBool             sIsFinal = STL_FALSE;
    stlBool             sIsFirst = STL_TRUE;
    smlRid              sTmpFirstRowRid;
    smlRid              sTmpLastRowRid;
    smlRid              sDummyNextRid;
    stlUInt16           sSlotHdrSize;
    dtlDataValue      * sDataValue;
    stlInt16            sSlotSize;
    stlInt16            sPadSpace;
    stlBool             sIsZero;
    
    sMaxPieceSize = smdmphGetMaxFreeSize( aRelationHandle );
    
    SMDMPH_GET_ROW_HDR_SIZE(aCopiedRowSlot, &sSlotHdrSize);
    sColPtr = aCopiedRowSlot + sSlotHdrSize;
    /* 그냥 임시로 세팅할 next rid 임 */
    if( aNextPieceRid->mPageId != SMP_NULL_PID )
    {
        sDummyNextRid = *aNextPieceRid;
    }
    else
    {
        sDummyNextRid = ((smlRid){0,0,1});
    }

    sRowPtr = sRowBuf + SMDMPH_MAX_ROW_HDR_LEN;
    sPrevPieceRid = *aPrevPieceRid;
    sColCnt = 0;

    sFirstColOrd = aFirstColOrd;
    for( sCurColOrd = aFirstColOrd; sCurColOrd  < aFirstColOrd + aPieceColCount; sCurColOrd++ )
    {
        sIsContCol = STL_FALSE;
        sIsLastCol = (sCurColOrd == aFirstColOrd + aPieceColCount - 1) ? STL_TRUE : STL_FALSE;

        if( sCurCol == NULL )
        {
            /* 기존 컬럼 값을 copy */
            SMP_GET_COLUMN_LEN( sColPtr, &sColLenSize, &sColLen, sIsZero );
            if( sRowPtr + sColLenSize + sColLen <= sRowBuf + sMaxPieceSize )
            {
                stlMemcpy( sRowPtr, sColPtr, sColLenSize + sColLen );
                sRowPtr += sColLenSize + sColLen;
                sColPtr += sColLenSize + sColLen;
                sColCnt++;
                
                if( sIsLastCol != STL_TRUE )
                {
                    continue;
                }
                else
                {
                    sIsFinal = STL_TRUE;
                }
            }
            /* make row piece */
        }
        else
        {
            if( sCurColOrd == KNL_GET_BLOCK_COLUMN_ORDER(sCurCol) )
            {
                /* Update 이후 이미지를 copy */
                sIsZero = STL_FALSE;
                sDataValue = KNL_GET_BLOCK_DATA_VALUE(sCurCol, aValueIdx);
                sColLen = sDataValue->mLength;

                if( DTL_NUMERIC_IS_ZERO(sDataValue->mValue, sColLen) )
                {
                    if( dtlDataTypeGroup[sDataValue->mType] == DTL_GROUP_NUMBER )
                    {
                        sColLen = 0;
                        sIsZero = STL_TRUE;
                    }
                }
                
                SMP_GET_COLUMN_LEN_SIZE( sColLen, &sColLenSize );
                if( sRowPtr + sColLenSize + sColLen <= sRowBuf + sMaxPieceSize )
                {
                    SMP_WRITE_COLUMN( sRowPtr, sDataValue->mValue, sColLen, sIsZero );
                    sRowPtr += sColLenSize + sColLen;

                    SMP_GET_COLUMN_LEN( sColPtr, &sColLenSize, &sColLen, sIsZero );
                    sColPtr += sColLenSize + sColLen;
                    sCurCol = sCurCol->mNext;
                    sColCnt++;
                    
                    if( sIsLastCol != STL_TRUE )
                    {
                        continue;
                    }
                    else
                    {
                        sIsFinal = STL_TRUE;
                    }
                }
                else
                {
                    if( (sColCnt == 0 ) &&
                        SMDMPH_CONT_SIZE( sColLen, sColLenSize ) > sMaxPieceSize )
                    {
                        sIsContCol = STL_TRUE;
                        
                        if( sIsLastCol == STL_TRUE )
                        {
                            sIsFinal = STL_TRUE;
                        }
                    }
                    else
                    {
                        sIsContCol = STL_FALSE;
                    }
                    /* make row piece */
                }
            }
            else
            {
                /* 기존 컬럼 값을 copy */
                SMP_GET_COLUMN_LEN( sColPtr, &sColLenSize, &sColLen, sIsZero );
                if( sRowPtr + sColLenSize + sColLen <= sRowBuf + sMaxPieceSize )
                {
                    stlMemcpy( sRowPtr, sColPtr, sColLenSize + sColLen );
                    sRowPtr += sColLenSize + sColLen;
                    sColPtr += sColLenSize + sColLen;
                    sColCnt++;
                    if( sIsLastCol != STL_TRUE )
                    {
                        continue;
                    }
                    else
                    {
                        sIsFinal = STL_TRUE;
                    }
                }
                /* make row piece */
            }
        }

        if( sIsContCol == STL_TRUE )
        {
            /**
             * 현재 가리키는 column을 continuous column으로 저장하고 다음으로 이동
             */

            STL_TRY_THROW( (sCurColOrd == KNL_GET_BLOCK_COLUMN_ORDER(sCurCol)) &&
                           (SMDMPH_CONT_SIZE(sColLen, sColLenSize) > sMaxPieceSize),
                           RAMP_ERR_INTERNAL );

            if( (sIsFinal == STL_TRUE) &&
                (aNextPieceRid->mPageId == SMP_NULL_PID) )
            {
                STL_TRY( smdmphInsertContCol( aStatement,
                                              aRelationHandle,
                                              aValueIdx,
                                              sCurCol,
                                              STL_FALSE,   /* aIsMaster */
                                              NULL,
                                              STL_TRUE, /* aIsForUpdate */
                                              &sTmpFirstRowRid,
                                              &sTmpLastRowRid,
                                              aEnv ) == STL_SUCCESS );
            }
            else
            {
                STL_TRY( smdmphInsertContCol( aStatement,
                                              aRelationHandle,
                                              aValueIdx,
                                              sCurCol,
                                              STL_FALSE,   /* aIsMaster */
                                              &sDummyNextRid,
                                              STL_TRUE, /* aIsForUpdate */
                                              &sTmpFirstRowRid,
                                              &sTmpLastRowRid,
                                              aEnv ) == STL_SUCCESS );
            }

            if( sIsFirst == STL_TRUE )
            {
                *aFirstRowRid = sTmpFirstRowRid;
                sIsFirst = STL_FALSE;
            }
            SMP_GET_COLUMN_LEN( sColPtr, &sColLenSize, &sColLen, sIsZero );
            sColPtr += sColLenSize + sColLen;
            sCurCol = sCurCol->mNext;
        }
        else
        {
            if( (sIsFinal == STL_TRUE) &&
                (aNextPieceRid->mPageId == SMP_NULL_PID) )
            {
                /* last piece */
                sRowStartPtr = sRowBuf + (SMDMPH_MAX_ROW_HDR_LEN - SMDMPH_MIN_ROW_HDR_LEN);
                sSlotSize = sRowPtr - sRowStartPtr;
                if( sSlotSize < SMDMPH_MINIMUM_ROW_PIECE_SIZE )
                {
                    sPadSpace = SMDMPH_MINIMUM_ROW_PIECE_SIZE - sSlotSize;
                    sSlotSize += sPadSpace;
                }
                else
                {
                    sPadSpace = 0;
                }
                STL_TRY( smdmphWriteRowHeader( sRowStartPtr,
                                               0, /* 임시 rts seq */
                                               SML_INFINITE_SCN,
                                               aStatement->mTcn,
                                               sTransId,
                                               NULL,
                                               sColCnt,
                                               sPadSpace,
                                               NULL ) == STL_SUCCESS );

            }
            else
            {
                /* chained piece */
                sRowStartPtr = sRowBuf;
                sSlotSize = sRowPtr - sRowStartPtr;
                STL_TRY( smdmphWriteRowHeader( sRowStartPtr,
                                               0, /* 임시 rts seq */
                                               SML_INFINITE_SCN,
                                               aStatement->mTcn,
                                               sTransId,
                                               NULL,
                                               sColCnt,
                                               0,
                                               &sDummyNextRid ) == STL_SUCCESS );
            }
            /* MASTER/SLAVE flag 조정 */
            SMDMPH_UNSET_MASTER_ROW( sRowStartPtr );

            STL_TRY( smdmphInsertRowPiece( aRelationHandle,
                                           sTransId,
                                           sViewScn,
                                           sRowStartPtr,
                                           sSlotSize,
                                           sFirstColOrd,
                                           STL_TRUE, /* aIsForUpdate */
                                           &sTmpLastRowRid,
                                           aEnv ) == STL_SUCCESS );

            sTmpFirstRowRid = sTmpLastRowRid;
            if( sIsFirst == STL_TRUE )
            {
                *aFirstRowRid = sTmpLastRowRid;
                sIsFirst = STL_FALSE;
            }
            /* 현재 append 하려다 실패한 컬럼을 새 row piece에서 다시 시도 */
            if( sIsFinal != STL_TRUE )
            {
                sCurColOrd--;
            }
        }

        /* update prev piece's next link rid */
        STL_ASSERT( sPrevPieceRid.mPageId != SMP_NULL_PID );

        STL_TRY( smxmBegin( &sMiniTrans,
                            SMA_GET_TRANS_ID(aStatement),
                            smsGetSegRid(sSegmentHandle),
                            sAttr,
                            aEnv ) == STL_SUCCESS );
        sState = 1;

        STL_TRY( smdmphUpdateNextPieceRid( &sMiniTrans,
                                           sTransId,
                                           sViewScn,
                                           aStatement->mTcn,
                                           &sPrevPieceRid,
                                           &sTmpFirstRowRid,
                                           aEnv ) == STL_SUCCESS );

        sState = 0;
        STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

        sPrevPieceRid = sTmpLastRowRid;
        sRowPtr = sRowBuf + SMDMPH_MAX_ROW_HDR_LEN;
        sColCnt = 0;
        sFirstColOrd = sCurColOrd + 1;
    } /* for */

    *aLastRowRid = sTmpLastRowRid;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INTERNAL,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      "smdmphRemakeUpdatedRowPiece()",
                      0 );
        
        KNL_SYSTEM_FATAL_DASSERT( STL_FALSE, KNL_ENV(aEnv) );
    }
    
    STL_FINISH;

    if( sState == 1 )
    {
        (void)smxmCommit( &sMiniTrans, aEnv );
    }

    return STL_FAILURE;
}


stlStatus smdmphAllocRowPiece( smxmTrans   * aMiniTrans,
                               smpHandle   * aPageHandle,
                               smxlTransId   aTransId,
                               smlScn        aViewScn,
                               smlRid        aSrcPieceRid,
                               stlInt16      aSrcPieceSize,
                               stlInt16      aPieceSizeDiff,
                               stlChar    ** aDstPiece,
                               stlUInt8    * aDstRtsSeq,
                               stlBool     * aInSlotUpdate,
                               smlEnv      * aEnv )
{
    stlChar  * sSrcPiece;
    stlChar  * sDstPiece;
    stlInt16   sPadSpace;
    stlUInt8   sDstRtsSeq;
    stlInt16   sRetryPhase;
    stlInt16   sUnusedSpace;
    stlInt16   sNeededSpace;
    
    sRetryPhase = 0;
    sDstRtsSeq = SMP_RTS_SEQ_NULL;
    *aInSlotUpdate = STL_TRUE;

    sSrcPiece = smpGetNthRowItem( aPageHandle, aSrcPieceRid.mOffset );
    SMDMPH_GET_PAD_SPACE( sSrcPiece, &sPadSpace );
    
    while( STL_TRUE )
    {
        /**
         * 공간 할당이 가능한지 체크. 불가능하면 compact 시도
         */
        
        if( sRetryPhase == 1 )
        {
            if( smpGetReclaimedSpace(SMP_FRAME(aPageHandle)) >=
                (aSrcPieceSize + aPieceSizeDiff + STL_SIZEOF(smpOffsetSlot)) )
            {
                STL_TRY( smdmphCompactPage( aMiniTrans,
                                            aPageHandle,
                                            aEnv )
                         == STL_SUCCESS );
            
                /**
                 * compact 발생. before row slot 재설정 필요
                 */
            
                sSrcPiece = smpGetNthRowItem( aPageHandle, aSrcPieceRid.mOffset );
                SMDMPH_GET_PAD_SPACE( sSrcPiece, &sPadSpace );
            }
        }
        else if (sRetryPhase == 2)
        {
            sDstRtsSeq = SMP_RTS_SEQ_NULL;
            sDstPiece = NULL;
            break;
        }
        else
        {
            /* do nothing */
        }

        if( SMXL_IS_UNDO_BOUND_TRANS( aTransId ) == STL_TRUE )
        {
            STL_TRY( smpAllocRts( NULL,
                                  aPageHandle,
                                  aTransId,
                                  SML_MAKE_VIEW_SCN(aViewScn),
                                  smdmphStabilizeRow,
                                  &sDstRtsSeq,
                                  aEnv ) == STL_SUCCESS );

            if( SMP_IS_VALID_RTS_SEQ(sDstRtsSeq) != STL_TRUE )
            {
                if( (sRetryPhase == 0) &&
                    (smpGetReclaimedSpace(SMP_FRAME(aPageHandle))
                     >= smpGetRtsSize() * smpGetCurRtsCount(aPageHandle)) )
                {
                    /* 처음으로 공간부족 발생. page를 일단 compact하여 최대한 빈 공간을 확보하고 진행 */
                    sRetryPhase = 1;
                    continue;
                }
            }
        }

        if( sPadSpace < aPieceSizeDiff )
        {
            /**
             * update로 인해 row의 길이가 현재보다 더 길어진다면
             * outplace updating을 한다.
             */
            
            sUnusedSpace = smpGetUnusedSpace( SMP_FRAME(aPageHandle) );
            sNeededSpace = aSrcPieceSize + aPieceSizeDiff;
            
            if( sUnusedSpace >= sNeededSpace + STL_SIZEOF(smpOffsetSlot) )
            {
                /**
                 * 빈 공간이 충분하면 단순히 realloc한다
                 */
                STL_TRY( smpReallocNthSlot( aPageHandle,
                                            aSrcPieceRid.mOffset,
                                            sNeededSpace,
                                            STL_TRUE,
                                            &sDstPiece )
                         == STL_SUCCESS );
                
                *aInSlotUpdate = STL_FALSE;
            }
            else
            {
                if( (sUnusedSpace >= aPieceSizeDiff - sPadSpace) && (sRetryPhase == 1) )
                {
                    /**
                     * 페이지내에 piece diff 만큼의 공간이 있으면 slot의 pad 영역을 증가시킨다
                     */
                    STL_TRY( smdmphExtendRowSlot( aMiniTrans,
                                                  aPageHandle,
                                                  aSrcPieceRid.mOffset,
                                                  aPieceSizeDiff - sPadSpace,
                                                  aEnv )
                             == STL_SUCCESS );
                
                    sDstPiece = smpGetNthRowItem( aPageHandle, aSrcPieceRid.mOffset );
                    
                    if( SMP_IS_VALID_RTS_SEQ(sDstRtsSeq) == STL_TRUE )
                    {
                        STL_TRY( smpAllocRts( NULL,
                                              aPageHandle,
                                              aTransId,
                                              SML_MAKE_VIEW_SCN(aViewScn),
                                              smdmphStabilizeRow,
                                              &sDstRtsSeq,
                                              aEnv ) == STL_SUCCESS );
                    }
                }
                else
                {
                    sDstPiece = NULL;
                }
            }
        }
        else
        {
            /**
             * update로 인해 row의 길이가 현재보다 더 작거나 같다면 inplace updating을 한다
             */
            
            sDstPiece = sSrcPiece;
        }

        if( sDstPiece == NULL )
        {
            /* RTS나 Row Slot 할당에 실패하면 Compact한 후 재시도 한다 */
            sRetryPhase++;
            continue;
        }

        break;
    }

    *aDstPiece = sDstPiece;
    *aDstRtsSeq = sDstRtsSeq;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmphAppendRowPiece( smxmTrans   * aMiniTrans,
                                smpHandle   * aPageHandle,
                                smxlTransId   aTransId,
                                smlScn        aViewScn,
                                smlTcn        aTcn,
                                stlBool       aInSlotUpdate,
                                stlInt16      aSrcPieceSize,
                                stlChar     * aAppendPieceBody,
                                stlInt16      aAppendPieceBodySize,
                                stlUInt8      aDstRtsSeq,
                                stlInt16      aReallocPieceSize,
                                stlInt32      aFirstColOrd,
                                stlInt16      aAppendPieceColCount,
                                smlRid        aNxtPieceRid,
                                smlRid        aPieceRid,
                                smlEnv      * aEnv )
{
    stlChar            sLogBuf[SMP_PAGE_SIZE];
    smlRid             sUndoRid;
    stlUInt8           sSrcRtsSeq;
    stlUInt8           sRtsVerNo;
    stlChar          * sDstPiece;
    smlScn             sScn;
    stlBool            sChainedPiece = STL_TRUE;
    stlInt16           sTotalPieceColCount = 0;
    stlInt16           sPadSize = 0;
    stlChar          * sSrcPiece;
    stlInt16           sDstPieceHdrSize;
    stlInt32           sLogOffset = 0;
    smlScn             sCommitScn;
    smxlTransId        sWaitTransId;

    if( aNxtPieceRid.mPageId == SMP_NULL_PID )
    {
        sChainedPiece = STL_FALSE;
    }
    
    sSrcPiece = smpGetNthRowItem( aPageHandle, aPieceRid.mOffset );
    STL_DASSERT( SMDMPH_HAS_NEXT_LINK(sSrcPiece) == STL_FALSE );

    SMDMPH_GET_COLUMN_CNT( sSrcPiece, &sTotalPieceColCount );
    sTotalPieceColCount += aAppendPieceColCount;
    sDstPiece = sSrcPiece;

    STL_DASSERT( sTotalPieceColCount <= SMD_MAX_COLUMN_COUNT_IN_PIECE );
    
    /**
     * logging undo record
     */

    STL_WRITE_MOVE_BYTES( sLogBuf, sSrcPiece, SMDMPH_MIN_ROW_HDR_LEN, sLogOffset );
    STL_WRITE_MOVE_INT16( sLogBuf, &aSrcPieceSize, sLogOffset );
    STL_WRITE_MOVE_INT32( sLogBuf, &aFirstColOrd, sLogOffset );
    STL_WRITE_MOVE_INT16( sLogBuf, &aAppendPieceColCount, sLogOffset );
    
    if( SMDMPH_IS_RTS_BOUND( sLogBuf ) == STL_TRUE )
    {
        STL_TRY( smdmphGetCommitScn( smpGetTbsId(aPageHandle),
                                     aPageHandle,
                                     sSrcPiece,
                                     &sCommitScn,
                                     &sWaitTransId,
                                     aEnv ) == STL_SUCCESS );

        if( sCommitScn < SML_INFINITE_SCN )
        {
            /* committed trans */
            SMDMPH_UNSET_RTS_BOUND( sLogBuf );
            SMDMPH_SET_SCN( sLogBuf, &sCommitScn );
        }
        else
        {
#ifdef STL_DEBUG
            /**
             * Release Mode에서는 무시하고 넘어간다.
             * 이후 Scan시 문제가 발생할수 있지만 View Scn을 다시 획득한후 시도하면
             * 이전 버전을 보지 않기 때문에 현상이 해소된듯 보일수 있다.
             */
            SMDMPH_GET_TRANS_ID( sSrcPiece, &sWaitTransId );
            if( aTransId != sWaitTransId )
            {
                STL_THROW( RAMP_ERR_INTERNAL );
            }
#endif
        }
    }
    
    STL_TRY( smxlInsertUndoRecord( aMiniTrans,
                                   SMG_COMPONENT_TABLE,
                                   SMD_UNDO_LOG_MEMORY_HEAP_APPEND,
                                   sLogBuf,
                                   sLogOffset,
                                   aPieceRid,
                                   &sUndoRid,
                                   aEnv ) == STL_SUCCESS );

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
    
    if( aInSlotUpdate == STL_FALSE )
    {
        SMDMPH_GET_PAD_SPACE( sSrcPiece, &sPadSize );
        
        STL_TRY( smpReallocNthSlot( aPageHandle,
                                    aPieceRid.mOffset,
                                    aReallocPieceSize,
                                    STL_FALSE,
                                    &sDstPiece ) == STL_SUCCESS );
        STL_DASSERT( sDstPiece != NULL );
        
        SMP_ADD_RECLAIMED_SPACE( SMP_FRAME(aPageHandle), aSrcPieceSize + sPadSize );
    }

    smpSetMaxViewScn( aPageHandle, aViewScn );

    /**
     * copy piece & append piece
     */

    sPadSize = 0;
    
    if( sChainedPiece == STL_TRUE )
    {
        sDstPieceHdrSize = SMDMPH_MAX_ROW_HDR_LEN;
        
        if( aInSlotUpdate == STL_TRUE )
        {
            stlMemmove( sDstPiece + SMDMPH_MAX_ROW_HDR_LEN,
                        sSrcPiece + SMDMPH_MIN_ROW_HDR_LEN,
                        aSrcPieceSize - SMDMPH_MIN_ROW_HDR_LEN );
            
            SMDMPH_GET_PAD_SPACE( sDstPiece, &sPadSize );
            sPadSize -= (aAppendPieceBodySize +
                         SMDMPH_MAX_ROW_HDR_LEN -
                         SMDMPH_MIN_ROW_HDR_LEN) ;
        }
        else
        {
            stlMemcpy( sDstPiece, sSrcPiece, SMDMPH_MIN_ROW_HDR_LEN );
            stlMemcpy( sDstPiece + SMDMPH_MAX_ROW_HDR_LEN,
                       sSrcPiece + SMDMPH_MIN_ROW_HDR_LEN,
                       aSrcPieceSize - SMDMPH_MIN_ROW_HDR_LEN );
        }

        SMDMPH_SET_HAS_NEXT_LINK( sDstPiece );
        SMDMPH_SET_LINK_PID( sDstPiece, &aNxtPieceRid.mPageId );
        SMDMPH_SET_LINK_OFFSET( sDstPiece, &aNxtPieceRid.mOffset );
        
        /**
         * append piece
         */
        stlMemcpy( (sDstPiece +
                    SMDMPH_MAX_ROW_HDR_LEN +
                    aSrcPieceSize -
                    SMDMPH_MIN_ROW_HDR_LEN),
                   aAppendPieceBody,
                   aAppendPieceBodySize );
    }
    else
    {
        sDstPieceHdrSize = SMDMPH_MIN_ROW_HDR_LEN;
        
        if( aInSlotUpdate == STL_TRUE )
        {
            SMDMPH_GET_PAD_SPACE( sDstPiece, &sPadSize );
            sPadSize -= aAppendPieceBodySize;
        }
        else
        {
            stlMemcpy( sDstPiece, sSrcPiece, aSrcPieceSize );
        }
            
        /**
         * append piece
         */
        stlMemcpy( sDstPiece + aSrcPieceSize,
                   aAppendPieceBody,
                   aAppendPieceBodySize );
    }
    
    SMDMPH_SET_PAD_SPACE( sDstPiece, &sPadSize );
    
    /**
     * fill piece header
     */
    
    if( SMP_IS_VALID_RTS_SEQ(aDstRtsSeq) == STL_TRUE )
    {    
        /* RTS 조정 */
        STL_TRY( smpBeginUseRts( aPageHandle,
                                 sDstPiece,
                                 aTransId,
                                 SML_MAKE_VIEW_SCN(aViewScn),
                                 aDstRtsSeq,
                                 &sRtsVerNo,
                                 aEnv ) == STL_SUCCESS );
        SMDMPH_SET_RTS_BOUND( sDstPiece );
        SMDMPH_SET_RTSSEQ( sDstPiece, &aDstRtsSeq );
    }
    else
    {
        if( SMXL_IS_UNDO_BOUND_TRANS( aTransId ) == STL_TRUE )
        {
            sScn = SML_MAKE_VIEW_SCN( aViewScn );
            SMDMPH_UNSET_RTS_BOUND( sDstPiece );
            SMDMPH_SET_SCN( sDstPiece, &sScn );
        }
        else
        {
            /* DS mode */
            sScn = 0;
            SMDMPH_UNSET_RTS_BOUND( sDstPiece );
            SMDMPH_SET_SCN( sDstPiece, &sScn );
        }
    }

    SMDMPH_SET_COLUMN_CNT( sDstPiece, &sTotalPieceColCount );
    SMDMPH_SET_TCN( sDstPiece, &aTcn );
    SMDMPH_SET_TRANS_ID( sDstPiece, &aTransId );
    SMDMPH_SET_ROLLBACK_PID( sDstPiece, &sUndoRid.mPageId );
    SMDMPH_SET_ROLLBACK_OFFSET( sDstPiece, &sUndoRid.mOffset );

    /**
     * logging REDO log
     */
    
    sLogOffset = 0;
    STL_WRITE_MOVE_INT64( sLogBuf, &aTransId, sLogOffset );
    STL_WRITE_MOVE_INT64( sLogBuf, &aViewScn, sLogOffset );
    STL_WRITE_MOVE_INT16( sLogBuf, &sDstPieceHdrSize, sLogOffset );
    STL_WRITE_MOVE_BYTES( sLogBuf, sDstPiece, sDstPieceHdrSize, sLogOffset );
    STL_WRITE_MOVE_INT32( sLogBuf, &aFirstColOrd, sLogOffset );
    STL_WRITE_MOVE_INT16( sLogBuf, &aAppendPieceColCount, sLogOffset );
    STL_WRITE_MOVE_INT16( sLogBuf, &aAppendPieceBodySize, sLogOffset );
    STL_WRITE_MOVE_BYTES( sLogBuf, aAppendPieceBody, aAppendPieceBodySize, sLogOffset );

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_TABLE,
                           SMR_LOG_MEMORY_HEAP_APPEND,
                           SMR_REDO_TARGET_PAGE,
                           sLogBuf,
                           sLogOffset,
                           aPieceRid.mTbsId,
                           aPieceRid.mPageId,
                           aPieceRid.mOffset,
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    smdmphValidateTablePage( aPageHandle, aEnv );

    return STL_SUCCESS;

#ifdef STL_DEBUG
    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INTERNAL,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      "smdmphAppendRowPiece()",
                      0 );
        
        KNL_SYSTEM_FATAL_DASSERT( STL_FALSE, KNL_ENV(aEnv) );
    }
#endif
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmphMakeMigratedRow( smxmTrans   * aMiniTrans,
                                 smpHandle   * aPageHandle,
                                 smxlTransId   aTransId,
                                 smlScn        aViewScn,
                                 smlScn        aCommitScn,
                                 smlTcn        aTcn,
                                 stlChar     * aRowSlot,
                                 smlRid      * aRowRid,
                                 stlInt16      aRowSlotSize,
                                 smlEnv      * aEnv )
{
    smlRid             sUndoRid;
    stlInt16           sRowHdrSize;
    stlInt16           sPadSpace;
    stlInt16           sBfrPadSpace;
    stlInt32           sFirstColOrd = 0;
    stlInt16           sColCount = 0;
    stlChar            sLogBuf[SMP_PAGE_SIZE];
    stlChar          * sLogPtr;
    stlInt16           sRetryCnt;
    stlUInt8           sRtsSeq;
    stlUInt8           sRtsVerNo;
    stlUInt8           sOrgRtsSeq;
    smlScn             sScn;
    smlPid             sPid = SMP_NULL_PID;
    stlInt16           sOffset = 0;
    smdmphLogicalHdr * sLogicalHdr;

    SMDMPH_GET_PAD_SPACE( aRowSlot, &sBfrPadSpace );
    STL_ASSERT( aRowSlotSize + sBfrPadSpace >= SMDMPH_MINIMUM_ROW_PIECE_SIZE );

    /* Undo Log 부터 작성 */
    sLogPtr = sLogBuf;
    stlMemcpy( sLogPtr, aRowSlot, aRowSlotSize );
    /* Undo log의 row header는 가능한 경우(commit된 다른 trans의 row) stamping 한다 */
    if( SMDMPH_IS_RTS_BOUND( sLogPtr) == STL_TRUE )
    {
        if( aCommitScn < SML_INFINITE_SCN )
        {
            /* committed trans */
            SMDMPH_UNSET_RTS_BOUND( sLogPtr );
            SMDMPH_SET_SCN( sLogPtr, &aCommitScn );
        }
    }
    sLogPtr += aRowSlotSize;
    
    /* DS  mode가 아니면 */
    if( SMXL_IS_UNDO_BOUND_TRANS( aTransId ) == STL_TRUE )
    {
        sRetryCnt = 0;
        while( STL_TRUE )
        {
            STL_TRY( smpAllocRts( NULL,
                                  aPageHandle,
                                  aTransId,
                                  SML_MAKE_VIEW_SCN(aViewScn),
                                  smdmphStabilizeRow,
                                  &sRtsSeq,
                                  aEnv ) == STL_SUCCESS );

            if( (SMP_IS_VALID_RTS_SEQ(sRtsSeq) != STL_TRUE) && (sRetryCnt == 0) )
            {
                /* 처음으로 공간부족 발생. page를 일단 compact하여 최대한 빈 공간을 확보하고 진행 */
                sRetryCnt++;
                STL_TRY( smdmphCompactPage( aMiniTrans,
                                            aPageHandle,
                                            aEnv )
                         == STL_SUCCESS );
                
                /* compact로 source row의 위치가 변경되었을 수 있으므로 다시 구한다 */
                aRowSlot = smpGetNthRowItem( aPageHandle, aRowRid->mOffset );
                continue;
            }

            break;
        }
    }
    else
    {
        sRtsSeq = SMP_RTS_SEQ_NULL;
    }

    if( SMDMPH_IS_RTS_BOUND( aRowSlot ) == STL_TRUE )
    {
        SMDMPH_GET_RTSSEQ( aRowSlot, &sOrgRtsSeq );
    }
    else
    {
        sOrgRtsSeq = SMP_RTS_SEQ_NULL;
    }

    /* write undo record */
    STL_TRY( smxlInsertUndoRecord( aMiniTrans,
                                   SMG_COMPONENT_TABLE,
                                   SMD_UNDO_LOG_MEMORY_HEAP_MIGRATE,
                                   sLogBuf,
                                   sLogPtr - sLogBuf,
                                   ((smlRid){ aRowRid->mTbsId,
                                           aRowRid->mOffset, /* slot seq */
                                           aRowRid->mPageId }),
                                   &sUndoRid,
                                   aEnv ) == STL_SUCCESS );

    smpSetMaxViewScn( aPageHandle, aViewScn );

    /* row piece를 변경한다 */
    SMDMPH_SET_MASTER_ROW( aRowSlot );
    SMDMPH_SET_TCN( aRowSlot, &aTcn );
    if( SMP_IS_VALID_RTS_SEQ(sRtsSeq) == STL_TRUE )
    {
        /* RTS 조정 */
        STL_TRY( smpBeginUseRts( aPageHandle,
                                 aRowSlot,
                                 aTransId,
                                 SML_MAKE_VIEW_SCN(aViewScn),
                                 sRtsSeq,
                                 &sRtsVerNo,
                                 aEnv ) == STL_SUCCESS );

        SMDMPH_SET_RTS_BOUND( aRowSlot );
        SMDMPH_SET_RTSSEQ( aRowSlot, &sRtsSeq );
    }
    else
    {
        if( SMXL_IS_UNDO_BOUND_TRANS( aTransId ) == STL_TRUE )
        {
            sScn = SML_MAKE_VIEW_SCN(aViewScn);
            SMDMPH_UNSET_RTS_BOUND( aRowSlot );
            SMDMPH_SET_SCN( aRowSlot, &sScn );
        }
        else
        {
            sScn = 0;
            SMDMPH_UNSET_RTS_BOUND( aRowSlot );
            SMDMPH_SET_SCN( aRowSlot, &sScn );
        }
    }

    if( SMP_IS_VALID_RTS_SEQ(sOrgRtsSeq) == STL_TRUE )
    {
        STL_TRY( smpEndUseRts( aPageHandle,
                               aRowSlot,
                               sOrgRtsSeq,
                               0,
                               aEnv ) == STL_SUCCESS );
    }

    SMDMPH_SET_TRANS_ID( aRowSlot, &aTransId );
    SMDMPH_SET_ROLLBACK_PID( aRowSlot, &sUndoRid.mPageId );
    SMDMPH_SET_ROLLBACK_OFFSET( aRowSlot, &sUndoRid.mOffset );
    SMDMPH_SET_COLUMN_CNT( aRowSlot, &sColCount );
    /* 일단 next piece pid 를 invalid pid로 세팅해 둠 */
    SMDMPH_SET_HAS_NEXT_LINK( aRowSlot );
    SMDMPH_SET_LINK_PID( aRowSlot, &sPid );
    SMDMPH_SET_LINK_OFFSET( aRowSlot, &sOffset );
    SMDMPH_GET_ROW_HDR_SIZE( aRowSlot, &sRowHdrSize );
    sPadSpace = aRowSlotSize + sBfrPadSpace - sRowHdrSize;
    SMDMPH_SET_PAD_SPACE( aRowSlot, &sPadSpace );
    SMDMPH_UNSET_CONT_COL( aRowSlot );

    sLogicalHdr = SMDMPH_GET_LOGICAL_HEADER( aPageHandle );
    if( sLogicalHdr->mSimplePage == STL_TRUE )
    {
        sLogicalHdr->mSimplePage = STL_FALSE;
        STL_TRY( smxmWriteLog( aMiniTrans,
                               SMG_COMPONENT_TABLE,
                               SMR_LOG_MEMORY_HEAP_UPDATE_LOGICAL_HDR,
                               SMR_REDO_TARGET_PAGE,
                               (stlChar*)sLogicalHdr,
                               STL_SIZEOF(smdmphLogicalHdr),
                               smpGetTbsId(aPageHandle),
                               smpGetPageId(aPageHandle),
                               0,
                               SMXM_LOGGING_REDO,
                               aEnv ) == STL_SUCCESS );
    }
    
    /* Write REDO log */
    sLogPtr = sLogBuf;
    STL_WRITE_INT64( sLogPtr, &aTransId );
    sLogPtr += STL_SIZEOF(smxlTransId);
    STL_WRITE_INT64( sLogPtr, &aViewScn );
    sLogPtr += STL_SIZEOF(smlScn);
    stlMemcpy( sLogPtr, aRowSlot, sRowHdrSize );
    sLogPtr += sRowHdrSize;
    STL_WRITE_INT32( sLogPtr, &sFirstColOrd );
    sLogPtr += STL_SIZEOF(stlInt32);
    STL_WRITE_INT16( sLogPtr, &sColCount );
    sLogPtr += STL_SIZEOF(stlInt16);

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_TABLE,
                           SMR_LOG_MEMORY_HEAP_MIGRATE,
                           SMR_REDO_TARGET_PAGE,
                           sLogBuf,
                           sLogPtr - sLogBuf,
                           aRowRid->mTbsId,
                           aRowRid->mPageId,
                           aRowRid->mOffset,  /* Physiological redo를 하기위해 offset이 아닌 seq 사용 */
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    smdmphValidateTablePage( aPageHandle, aEnv );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdmphUpdateInPage( smxmTrans         * aMiniTrans,
                              smpHandle         * aPageHandle,
                              stlChar           * aUndoLog,
                              stlInt16            aUndoLogSize,
                              smlRid            * aSrcRowRid,
                              stlChar           * aSrcRowSlot,
                              stlInt16            aSrcRowSize,
                              stlUInt8            aSrcRtsSeq,
                              stlChar           * aDstRowSlot,
                              stlInt16            aDstRowSize,
                              stlUInt8            aDstRtsSeq,
                              stlInt32            aValueIdx,
                              knlValueBlockList * aAfterCols,
                              stlInt16            aPieceColCnt,
                              stlInt16            aUpdateColCnt,
                              stlInt32            aFirstColOrd,
                              stlBool             aIsColSizeDiff,
                              smxlTransId         aMyTransId,
                              smlScn              aMyViewScn,
                              smlTcn              aMyTcn,
                              smlEnv            * aEnv )
{
    stlChar             sRowBuf[SMP_PAGE_SIZE];
    stlChar             sLogBuf[SMP_PAGE_SIZE];
    stlChar           * sLogPtr;
    stlChar           * sLogEnd;
    smlRid              sUndoRid;
    stlInt16            sSrcRowHdrSize;
    stlInt16            sPadSpace;
    stlChar           * sNewSlot;
    stlUInt8            sRtsVerNo;
    smlScn              sScn;

    SMDMPH_GET_ROW_HDR_SIZE( aSrcRowSlot, &sSrcRowHdrSize );
    /* write undo record */
    STL_TRY( smxlInsertUndoRecord( aMiniTrans,
                                   SMG_COMPONENT_TABLE,
                                   SMD_UNDO_LOG_MEMORY_HEAP_UPDATE,
                                   aUndoLog,
                                   aUndoLogSize,
                                   ((smlRid){ aSrcRowRid->mTbsId,
                                              aSrcRowRid->mOffset, /* slot seq */
                                              aSrcRowRid->mPageId }),
                                   &sUndoRid,
                                   aEnv ) == STL_SUCCESS );

    smpSetMaxViewScn( aPageHandle, aMyViewScn );

    /* 이 이하로 실패할리가 없는 operation만 온다 */

    /* redo log 처음의 transRid, trans view scn, row header와 first col ord,   *
     * update column count를 남겨두고 column들의 after image 부터 기록한다     */
    sLogPtr = sLogBuf + STL_SIZEOF(smxlTransId) + STL_SIZEOF(smlScn)
        + sSrcRowHdrSize + STL_SIZEOF(stlInt32) + STL_SIZEOF(stlInt16);

    /* Page 내용 변경 */
    if( aDstRowSlot != aSrcRowSlot )
    {
        SMDMPH_GET_PAD_SPACE( aSrcRowSlot, &sPadSpace );
        
        /* outplace updating */
        STL_TRY( smpReallocNthSlot( aPageHandle,
                                    aSrcRowRid->mOffset,
                                    aDstRowSize,
                                    STL_FALSE,
                                    &sNewSlot ) == STL_SUCCESS );
        STL_DASSERT( (sNewSlot != NULL ) &&
                     (sNewSlot == aDstRowSlot ) );

        /* src row가 delete 되었으니 reclaimed space에 일단 추가한다 */
        SMP_ADD_RECLAIMED_SPACE( SMP_FRAME(aPageHandle), aSrcRowSize + sPadSpace );

        stlMemcpy( aDstRowSlot, aSrcRowSlot, sSrcRowHdrSize );
        STL_TRY( smdmphCopyAndUpdateColumns( aSrcRowSlot,
                                             aDstRowSlot,
                                             aAfterCols,
                                             aValueIdx,
                                             aSrcRowSize,
                                             aPieceColCnt,
                                             aFirstColOrd,
                                             sLogPtr,
                                             &sLogEnd,
                                             STL_TRUE,
                                             aEnv ) == STL_SUCCESS );
        sPadSpace = 0;
        SMDMPH_SET_PAD_SPACE( aDstRowSlot, &sPadSpace );
    }
    else
    {
        /* row의 길이가 현재보다 작거나 같다면 inplace updating을 한다 */
        if( aUpdateColCnt > 0 )
        {
            stlMemcpy( sRowBuf, aSrcRowSlot, aSrcRowSize );
            stlMemcpy( aDstRowSlot, sRowBuf, sSrcRowHdrSize );
            STL_TRY( smdmphCopyAndUpdateColumns( sRowBuf,
                                                 aDstRowSlot, /* == sRowSlot */
                                                 aAfterCols,
                                                 aValueIdx,
                                                 aSrcRowSize,
                                                 aPieceColCnt,
                                                 aFirstColOrd,
                                                 sLogPtr,
                                                 &sLogEnd,
                                                 aIsColSizeDiff,
                                                 aEnv ) == STL_SUCCESS );
            SMDMPH_GET_PAD_SPACE( aSrcRowSlot, &sPadSpace );
            sPadSpace += aSrcRowSize - aDstRowSize;
            SMDMPH_SET_PAD_SPACE( aDstRowSlot, &sPadSpace );
        }
        else
        {
            /* update할 컬럼이 없는 dummy update */
            sLogEnd = sLogPtr;
        }
    }

    if( SMP_IS_VALID_RTS_SEQ(aDstRtsSeq) == STL_TRUE )
    {    
        /* RTS 조정 */
        STL_TRY( smpBeginUseRts( aPageHandle,
                                 aDstRowSlot,
                                 aMyTransId,
                                 SML_MAKE_VIEW_SCN(aMyViewScn),
                                 aDstRtsSeq,
                                 &sRtsVerNo,
                                 aEnv ) == STL_SUCCESS );
        SMDMPH_SET_RTS_BOUND( aDstRowSlot );
        SMDMPH_SET_RTSSEQ( aDstRowSlot, &aDstRtsSeq );
    }
    else
    {
        if( SMXL_IS_UNDO_BOUND_TRANS( aMyTransId ) == STL_TRUE )
        {
            sScn = SML_MAKE_VIEW_SCN(aMyViewScn);
            SMDMPH_UNSET_RTS_BOUND( aDstRowSlot );
            SMDMPH_SET_SCN( aDstRowSlot, &sScn );
        }
        else
        {
            /* DS mode */
            sScn = 0;
            SMDMPH_UNSET_RTS_BOUND( aDstRowSlot );
            SMDMPH_SET_SCN( aDstRowSlot, &sScn );
        }
    }

    if( SMP_IS_VALID_RTS_SEQ(aSrcRtsSeq) == STL_TRUE )
    {    
        STL_TRY( smpEndUseRts( aPageHandle,
                               aSrcRowSlot,
                               aSrcRtsSeq,
                               0,
                               aEnv ) == STL_SUCCESS );
    }    

    SMDMPH_SET_TCN( aDstRowSlot, &aMyTcn );
    SMDMPH_SET_TRANS_ID( aDstRowSlot, &aMyTransId );
    SMDMPH_SET_ROLLBACK_PID( aDstRowSlot, &sUndoRid.mPageId );
    SMDMPH_SET_ROLLBACK_OFFSET( aDstRowSlot, &sUndoRid.mOffset );

    /* Write REDO log */
    sLogPtr = sLogBuf;
    STL_WRITE_INT64( sLogPtr, &aMyTransId );
    sLogPtr += STL_SIZEOF(smxlTransId);
    STL_WRITE_INT64( sLogPtr, &aMyViewScn );
    sLogPtr += STL_SIZEOF(smlScn);
    stlMemcpy( sLogPtr, aDstRowSlot, sSrcRowHdrSize );
    sLogPtr += sSrcRowHdrSize;
    STL_WRITE_INT32( sLogPtr, &aFirstColOrd );
    sLogPtr += STL_SIZEOF(stlInt32);
    STL_WRITE_INT16( sLogPtr, &aUpdateColCnt );
    sLogPtr += STL_SIZEOF(stlInt16);

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_TABLE,
                           SMR_LOG_MEMORY_HEAP_UPDATE,
                           SMR_REDO_TARGET_PAGE,
                           sLogBuf,
                           sLogEnd - sLogBuf,
                           aSrcRowRid->mTbsId,
                           aSrcRowRid->mPageId,
                           aSrcRowRid->mOffset,  /* Physiological redo를 하기위해 offset이 아닌 seq 사용 */
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    smdmphValidateTablePage( aPageHandle, aEnv );

    return STL_SUCCESS ;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdmphFindNextColRid( smlStatement * aStatement,
                                void         * aRelationHandle,
                                smlRid       * aPrevRid,
                                smlRid       * aRowRid,
                                smlRid       * aNextRid,
                                smlEnv       * aEnv )
{
    stlUInt32          sState = 0;
    smlRid             sCurRid = *aRowRid;
    smlRid             sNextRid = sCurRid;
    smpHandle          sPageHandle;
    stlChar          * sRowSlot;
    stlBool            sIsContColRow = STL_TRUE;

    while( 1 )
    {
        /* 주어진 페이지를 얻는다 */
        STL_TRY( smpAcquire( NULL,
                             sCurRid.mTbsId,
                             sCurRid.mPageId,
                             KNL_LATCH_MODE_SHARED,
                             &sPageHandle,
                             aEnv ) == STL_SUCCESS );
        sState = 1;

        sRowSlot = smpGetNthRowItem( &sPageHandle, sCurRid.mOffset );
        sIsContColRow = SMDMPH_IS_CONT_COL( sRowSlot );

        if( SMDMPH_HAS_NEXT_LINK( sRowSlot ) == STL_TRUE )
        {
            SMDMPH_GET_LINK_PID( sRowSlot, &sNextRid.mPageId );
            SMDMPH_GET_LINK_OFFSET( sRowSlot, &sNextRid.mOffset );
        }
        else
        {
            STL_ASSERT( sIsContColRow != STL_TRUE );
            sNextRid.mPageId = SMP_NULL_PID;
            sNextRid.mOffset = 0;
            break;
        }

        sState = 0;
        STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );

        if( sIsContColRow != STL_TRUE )
        {
            break;
        }
        
        *aPrevRid = sCurRid;
        sCurRid = sNextRid;
    }

    if( sState == 1 )
    {
        STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
    }

    *aRowRid = sCurRid;
    *aNextRid = sNextRid;

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpRelease( &sPageHandle, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smdmphFindLastPiece( smlRid     aStartRid,
                               smlRid   * aPrevRid,
                               smlRid   * aCurRid,
                               stlInt32 * aLastColOrd,
                               smlEnv   * aEnv )
{
    stlUInt32     sState = 0;
    smlRid        sCurRid = aStartRid;
    smlRid        sNextRid;
    smpHandle     sPageHandle;
    stlChar     * sRowSlot;
    stlInt16      sColumnCount;

    while( 1 )
    {
        /* 주어진 페이지를 얻는다 */
        STL_TRY( smpAcquire( NULL,
                             sCurRid.mTbsId,
                             sCurRid.mPageId,
                             KNL_LATCH_MODE_SHARED,
                             &sPageHandle,
                             aEnv ) == STL_SUCCESS );
        sState = 1;

        sRowSlot = smpGetNthRowItem( &sPageHandle, sCurRid.mOffset );
        SMDMPH_GET_COLUMN_CNT( sRowSlot, &sColumnCount );

        if( SMDMPH_IS_CONT_COL( sRowSlot ) == STL_TRUE )
        {
            if( SMDMPH_IS_CONT_COL_LAST( sRowSlot ) == STL_TRUE )
            {
                *aLastColOrd += sColumnCount;
            }
        }
        else
        {
            *aLastColOrd += sColumnCount;
        }
        
        if( SMDMPH_HAS_NEXT_LINK( sRowSlot ) == STL_TRUE )
        {
            
            sNextRid.mTbsId = sCurRid.mTbsId;
            SMDMPH_GET_LINK_PID( sRowSlot, &sNextRid.mPageId );
            SMDMPH_GET_LINK_OFFSET( sRowSlot, &sNextRid.mOffset );
        }
        else
        {
            break;
        }

        sState = 0;
        STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );

        *aPrevRid = sCurRid;
        sCurRid = sNextRid;
    }

    if( sState == 1 )
    {
        STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
    }

    *aCurRid = sCurRid;

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpRelease( &sPageHandle, aEnv );
    }

    return STL_FAILURE;
}


/**
 * @brief 주어진 row rid의 row의 컬럼들을 갱신한다
 * @param[in] aStatement 사용하는 statement
 * @param[in] aRelationHandle 갱신할 row가 존재하는 relation의 핸들
 * @param[in] aRowRid 갱신할 row의 row rid
 * @param[in] aRowScn 레코드 검색 당시의 SCN
 * @param[in] aValueIdx Value block에서 현재 처리할 row의 순서
 * @param[in] aAfterCols 갱신할 컬럼의 이후 이미지 정보들
 * @param[out] aBeforeCols 갱신할 컬럼의 이전 이미지를 기록할 버퍼
 * @param[in] aPrimaryKey 해당 레코드의 primary key
 * @param[out] aVersionConflict update 도중 version conflict를 발견했는지 여부
 * @param[out] aSkipped 이미 갱신된 Row인지 여부
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmphUpdate( smlStatement      * aStatement,
                        void              * aRelationHandle,
                        smlRid            * aRowRid,
                        smlScn              aRowScn,
                        stlInt32            aValueIdx,
                        knlValueBlockList * aAfterCols,
                        knlValueBlockList * aBeforeCols,
                        knlValueBlockList * aPrimaryKey,
                        stlBool           * aVersionConflict,
                        stlBool           * aSkipped,
                        smlEnv            * aEnv )
{
    knlValueBlockList * sCurCol = aAfterCols;
    knlValueBlockList * sBeforeCol;
    stlUInt32           sAttr = SMXM_ATTR_REDO;
    void              * sSegmentHandle = SME_GET_SEGMENT_HANDLE(aRelationHandle);
    smlTbsId            sTbsId = smsGetTbsId(sSegmentHandle);
    stlChar *           sSrcRowSlot;
    stlChar *           sDstRowSlot;
    stlInt16            sPieceColCnt;
    stlInt16            sUpdateColCnt;
    stlChar           * sUpdateColCntPtr;
    stlUInt32           sState = 0;
    smxmTrans           sMiniTrans;
    smpHandle           sPageHandle;
    stlChar           * sRowPtr;
    stlInt64            sRowSizeDiff;
    stlBool             sIsColSizeDiff;
    stlInt16            sSrcRowPieceSize;
    stlInt16            sSrcRowHeaderSize;
    stlInt16            sSrcRowBodySize;
    stlChar             sRowBuf[SMP_PAGE_SIZE];
    stlChar             sLogBuf[SMP_PAGE_SIZE];
    stlChar           * sLogPtr;
    stlChar           * sLogEnd = NULL;
    smxlTransId         sTransId;
    smlScn              sViewScn;
    smlTcn              sRowTcn = 0;
    smlTcn              sStmtTcn;
    stlUInt8            sSrcRtsSeq;
    stlUInt8            sDstRtsSeq;
    smlScn              sCommitScn;
    stlInt16            sPadSpace;
    stlInt16            sRetryPhase;
    smxlTransId         sWaitTransId;
    smlRid              sRowRid = *aRowRid;
    stlInt32            sFirstColOrd = 0;
    smlRid              sPrevRid = ((smlRid){sTbsId,0,SMP_NULL_PID});
    smlRid              sNextRid = ((smlRid){sTbsId,0,SMP_NULL_PID});
    stlBool             sIsContColRow;
    stlInt16            sUnusedSpace;
    stlInt64            sNeededSpace;
    stlInt16            i;
    stlBool             sIsMyTrans = STL_FALSE;
    stlBool             sIsMyLock = STL_FALSE;
    void              * sLockItem = NULL;
    smlRid              sTmpFirstRowRid;
    smlRid              sTmpLastRowRid;
    stlBool             sIsFirstPiece = STL_TRUE;
    stlBool             sSupplLogged = STL_FALSE;
    stlInt16            sMaxPieceSize;
    stlInt32            sInterErrPos = 0;
    stlBool             sUpdateInPlace = STL_FALSE;


    if( aPrimaryKey == NULL )
    {
        sBeforeCol = NULL;
    }
    else
    {
        sBeforeCol = aBeforeCols;
    }
    
    /**
     * 페이지에 저장할수 있는 크기와 사용자가 지정값중 작은 값을 설정한다.
     */
    sMaxPieceSize = STL_MIN( (SMP_PAGE_SIZE * (100 - SMD_GET_PCTFREE(aRelationHandle))) / 100,
                             smdmphGetMaxFreeSize( aRelationHandle ) );

    STL_DASSERT( aRowScn != 0 );

    *aVersionConflict = STL_FALSE;
    *aSkipped = STL_FALSE;

    while( 1 )
    {
        STL_TRY( smxmBegin( &sMiniTrans,
                            SMA_GET_TRANS_ID(aStatement),
                            smsGetSegRid(sSegmentHandle),
                            sAttr,
                            aEnv ) == STL_SUCCESS );
        sState = 1;

        sTransId = SMA_GET_TRANS_ID(aStatement);
        sStmtTcn = SMA_GET_TCN( aStatement );

        /* 주어진 페이지를 얻는다 */
        STL_TRY( smpAcquire( &sMiniTrans,
                             sTbsId,
                             sRowRid.mPageId,
                             KNL_LATCH_MODE_EXCLUSIVE,
                             &sPageHandle,
                             aEnv ) == STL_SUCCESS );

        sSrcRowSlot = smpGetNthRowItem( &sPageHandle, sRowRid.mOffset );
        
        STL_TRY( smdmphGetCommitScn( sTbsId,
                                     &sPageHandle,
                                     sSrcRowSlot,
                                     &sCommitScn,
                                     &sWaitTransId,
                                     aEnv ) == STL_SUCCESS );

        sIsMyTrans = SMDMPH_IS_SAME_TRANS( sTransId,
                                           SML_INFINITE_SCN, /* DML 중이라 내 커밋SCN은 무한대이다 */
                                           sWaitTransId,
                                           sCommitScn );
        
        /* My Transaction은 통과 */
        if( (sIsMyTrans == STL_FALSE) && (sIsFirstPiece == STL_TRUE) )
        {
            sLockItem = NULL;

            while( STL_TRUE )
            {
                /* 최종 버전이 Commit 된 레코드임 */
                if( sCommitScn != SML_INFINITE_SCN )
                {
                    /* Lock Bit이 설정되어 있지 않다면 대기할 필요 없다. */
                    if( SMDMPH_IS_LOCKED( sSrcRowSlot ) == STL_FALSE )
                    {
                        break;
                    }

                    /* 이미 Unlock된 경우라면 대기할 필요 없다. */
                    STL_TRY( smklIsActiveLockRecord( sTransId,
                                                     sRowRid,
                                                     &sLockItem,
                                                     &sIsMyLock,
                                                     aEnv )
                             == STL_SUCCESS );

                    if( sLockItem == NULL )
                    {
                        /* Lock Bit를 Clear 한다. */
                        SMDMPH_UNSET_LOCKED( sSrcRowSlot );
                        break;
                    }
                    
                    if( sIsMyLock == STL_TRUE )
                    {
                        break;
                    }
                }

                STL_ASSERT( sFirstColOrd == 0 );
                sState = 0;
                STL_TRY( smxmRollback( &sMiniTrans, aEnv ) == STL_SUCCESS );

                if( sLockItem != NULL )
                {
                    STL_TRY( smklWaitRecord( sTransId,
                                             SML_LOCK_X,
                                             sRowRid,
                                             SML_LOCK_TIMEOUT_INFINITE,
                                             aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    /* sWaitTransId가 종료될때까지 기다린다 */
                    STL_TRY( smxlWaitTrans( sTransId,
                                            sWaitTransId,
                                            SMA_GET_LOCK_TIMEOUT(aStatement),
                                            aEnv ) == STL_SUCCESS );
                }

                /* version conflict 발생. 상위에서 filter 적용 후 통과하면 stmt scn을 증가시키고 retry */
                STL_ASSERT( sFirstColOrd == 0 );
                *aVersionConflict = STL_TRUE;
                
                STL_THROW( RAMP_SKIP_UPDATE );
            } /* while */

            sIsFirstPiece = STL_FALSE;
        } /* if */

        SMDMPH_GET_TCN( sSrcRowSlot, &sRowTcn);
        
        /**
         * Version Conflict 검사
         */
        if( sIsMyTrans == STL_FALSE )
        {
            if( sCommitScn > aRowScn )
            {
                /* version conflict 발생. 상위에서 filter 적용 후 통과하면 stmt scn을 증가시키고 retry */
                *aVersionConflict = STL_TRUE;
            
                sState = 0;
                STL_TRY( smxmRollback( &sMiniTrans, aEnv ) == STL_SUCCESS );

                STL_THROW( RAMP_SKIP_UPDATE );
            }
        }
        else
        {
            if( (sCommitScn == SML_INFINITE_SCN) && (sRowTcn == sStmtTcn) )
            {
                /**
                 * 이미 해당 Statement에서 갱신한 레코드이다.
                 */
                sState = 0;
                STL_TRY( smxmRollback( &sMiniTrans, aEnv ) == STL_SUCCESS );

                *aSkipped = STL_TRUE;

                STL_THROW( RAMP_SKIP_UPDATE );
            }
        }

        sViewScn = smxlGetTransViewScn( aStatement->mTransId );

        if( SMDMPH_IS_DELETED( sSrcRowSlot ) == STL_TRUE )
        {
            sState = 0;
            STL_TRY( smxmRollback( &sMiniTrans, aEnv ) == STL_SUCCESS );

            sInterErrPos = 1;
            STL_THROW( RAMP_ERR_INTERNAL );
        }
        
        /**
         * Supplemental Log를 기록한다.
         */
        if( (aPrimaryKey != NULL) && (sSupplLogged == STL_FALSE) )
        {
            STL_TRY( smdWriteSupplementalLog( &sMiniTrans,
                                              SML_SUPPL_LOG_UPDATE,
                                              aValueIdx,
                                              aPrimaryKey,
                                              aAfterCols,
                                              aRowRid,
                                              aEnv )
                     == STL_SUCCESS );

            STL_TRY( smdWriteSupplUpdateColLogs( &sMiniTrans,
                                                 aAfterCols,
                                                 aValueIdx,
                                                 aRowRid,
                                                 aEnv )
                     == STL_SUCCESS );
            
            sSupplLogged = STL_TRUE;
        }

        /* next row piece 위치를 미리 구해둔다 */
        if( SMDMPH_HAS_NEXT_LINK( sSrcRowSlot ) == STL_TRUE )
        {
            SMDMPH_GET_LINK_PID( sSrcRowSlot, &sNextRid.mPageId );
            SMDMPH_GET_LINK_OFFSET( sSrcRowSlot, &sNextRid.mOffset );
        }
        else
        {
            sNextRid.mPageId = SMP_NULL_PID;
            sNextRid.mOffset = 0;
        }

        /* continuous column 외 공통 : row piece 분석*/
        sIsContColRow =  SMDMPH_IS_CONT_COL( sSrcRowSlot );
        SMDMPH_GET_COLUMN_CNT( sSrcRowSlot, &sPieceColCnt );
        SMDMPH_GET_PAD_SPACE( sSrcRowSlot, &sPadSpace );

        sRowSizeDiff = 0;
        SMDMPH_GET_ROW_HDR_SIZE( sSrcRowSlot, &sSrcRowHeaderSize );
        sRowPtr = sSrcRowSlot + sSrcRowHeaderSize;
        sIsColSizeDiff = STL_FALSE;
        sUpdateColCnt = 0;

        /* Undo Log 내용부터 미리 작성 */
        /* Undo log의 row header는 가능한 경우(commit된 다른 trans의 row) stamping 한다 */
        if( SMDMPH_IS_RTS_BOUND( sSrcRowSlot ) == STL_TRUE )
        {
            if( sCommitScn < SML_INFINITE_SCN )
            {
                SMDMPH_GET_RTSSEQ( sSrcRowSlot, &sSrcRtsSeq );
                STL_TRY( smpEndUseRts( &sPageHandle,
                                       sSrcRowSlot,
                                       sSrcRtsSeq,
                                       0,
                                       aEnv )
                         == STL_SUCCESS );
                SMDMPH_UNSET_RTS_BOUND( sSrcRowSlot );
                SMDMPH_SET_SCN( sSrcRowSlot, &sCommitScn );
            }
            else
            {
                SMDMPH_GET_TRANS_ID( sSrcRowSlot, &sWaitTransId );
                if( sTransId != sWaitTransId )
                {
                    sState = 0;
                    STL_TRY( smxmRollback( &sMiniTrans, aEnv ) == STL_SUCCESS );

                    sInterErrPos = 2;
                    STL_THROW( RAMP_ERR_INTERNAL );
                }
            }
        }

        sUpdateInPlace = STL_FALSE;
        sLogPtr  = sLogBuf;
        stlMemcpy( sLogPtr, sSrcRowSlot, sSrcRowHeaderSize );

        sLogPtr += sSrcRowHeaderSize;
        /* undo log에서 first update col ord와 update col cnt 만큼 건너뛰고 컬럼먼저 기술 */
        STL_WRITE_INT32(sLogPtr, &sFirstColOrd);
        sLogPtr += STL_SIZEOF(stlInt32);
        sUpdateColCntPtr = sLogPtr;
        sLogPtr += STL_SIZEOF(stlInt16);

        STL_TRY( smdmphGetUpdatePieceSize( sRowPtr,
                                           sPieceColCnt,
                                           sFirstColOrd,
                                           sCurCol,
                                           sBeforeCol,
                                           aValueIdx,
                                           sLogPtr,
                                           &sLogEnd,
                                           &sSrcRowBodySize,
                                           &sIsColSizeDiff,
                                           &sRowSizeDiff,
                                           &sUpdateColCnt ) == STL_SUCCESS );

        sSrcRowPieceSize = sSrcRowHeaderSize + sSrcRowBodySize;
        sNeededSpace = sSrcRowPieceSize + sRowSizeDiff;

        /* undo log의 update column count를 채움 */
        STL_WRITE_INT16( sUpdateColCntPtr, &sUpdateColCnt );

        if( sUpdateColCnt == 0 )
        {
            /* update로 현재 row piece에 변화가 없으므로 다음 row piece로 진행 */
            if( SMDMPH_IS_MASTER_ROW( sSrcRowSlot ) != STL_TRUE )
            {
                sState = 0;
                STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

                sFirstColOrd += sPieceColCnt;
                
                if( sIsContColRow == STL_TRUE )
                {
                    STL_TRY( smdmphFindNextColRid( aStatement,
                                                   aRelationHandle,
                                                   &sPrevRid,
                                                   &sRowRid,
                                                   &sNextRid,
                                                   aEnv ) == STL_SUCCESS );
                }
                
                if( sCurCol != NULL )
                {
                    if( sNextRid.mPageId == SMP_NULL_PID )
                    {
                        /**
                         * Trailing null
                         */

                        STL_TRY( smdmphAppendColumns( aStatement,
                                                      aRelationHandle,
                                                      sFirstColOrd,
                                                      sCurCol,
                                                      sPrevRid,
                                                      sRowRid,
                                                      aValueIdx,
                                                      aEnv )
                                 == STL_SUCCESS );
                        break;
                    }
                }
                else
                {
                    STL_DASSERT( STL_FALSE );
                    break;
                }

                sPrevRid = sRowRid;
                sRowRid = sNextRid;

                continue;
            }
            else
            {
                /* 첫 row piece면 dummy update를 해야한다 */
                STL_ASSERT( (sRowSizeDiff == 0) && (sIsColSizeDiff == STL_FALSE) );

                if( sIsContColRow == STL_TRUE )
                {
                    /**
                     * MASTER 이면서 Continuous column이라면 Continuous slave piece 검사시
                     * 증가 시키기 때문에 지금은 하나 감소시킨다.
                     */
                    sFirstColOrd -= sPieceColCnt;
                }

                sUpdateInPlace = STL_TRUE;
            }
        }

        if( (sUpdateInPlace == STL_TRUE) ||
            ((sNeededSpace <= sMaxPieceSize) && (sIsContColRow != STL_TRUE)) )
        {
            /* update 이후의 크기가 한 페이지에 들어갈 수 있는 row piece의 최대 크기를 */
            /* 넘지 않는 경우, 현재 페이지에서 해결 가능한지 타진한다                  */
            sRetryPhase = 0;
            sDstRtsSeq = SMP_RTS_SEQ_NULL;
            while( STL_TRUE )
            {
                /* 공간 할당이 가능한지 체크. 불가능하면 compact 시도     */
                /* 이도 안되면 새 페이지를 할당받아 Migrated Row로 만든다 */
                if( sRetryPhase == 1 )
                {
                    STL_TRY( smdmphCompactPage( &sMiniTrans,
                                                &sPageHandle,
                                                aEnv )
                             == STL_SUCCESS );
                    
                    /* compact 발생. before row slot 재설정 필요 */
                    sSrcRowSlot = smpGetNthRowItem( &sPageHandle, sRowRid.mOffset );
                    sRowPtr  = sSrcRowSlot + sSrcRowHeaderSize;
                    SMDMPH_GET_PAD_SPACE( sSrcRowSlot, &sPadSpace );

                    KNL_BREAKPOINT( KNL_BREAKPOINT_SMDMPHUPDATE_AFTER_COMPACT, KNL_ENV(aEnv) );
                }
                else if (sRetryPhase == 2)
                {
                    sDstRtsSeq = SMP_RTS_SEQ_NULL;
                    sDstRowSlot = NULL;
                    break;
                }
                else
                {
                    /* do nothing */
                }

                /* DS  mode가 아니면 */
                if( SMXL_IS_UNDO_BOUND_TRANS( sTransId ) == STL_TRUE )
                {
                    STL_TRY( smpAllocRts( NULL,
                                          &sPageHandle,
                                          sTransId,
                                          SML_MAKE_VIEW_SCN(sViewScn),
                                          smdmphStabilizeRow,
                                          &sDstRtsSeq,
                                          aEnv ) == STL_SUCCESS );

                    if( SMP_IS_VALID_RTS_SEQ(sDstRtsSeq) != STL_TRUE )
                    {
                        if( (sRetryPhase == 0) &&
                            (smpGetReclaimedSpace(SMP_FRAME(&sPageHandle)) >= smpGetRtsSize()) )
                        {
                            /* 처음으로 공간부족 발생. page를 일단 compact하여 최대한 빈 공간을 확보하고 진행 */
                            sRetryPhase = 1;
                            continue;
                        }
                    }
                }

                if( sPadSpace < sRowSizeDiff )
                {
                    /* case 3 : update로 인해 row의 길이가 현재보다 더 길어진다면
                       outplace updating을 한다                                  */
                    sUnusedSpace = smpGetUnusedSpace( SMP_FRAME(&sPageHandle) );
                    sNeededSpace = sSrcRowPieceSize + sRowSizeDiff;
                    if( sUnusedSpace >= sNeededSpace + STL_SIZEOF(smpOffsetSlot) )
                    {
                        /* case 3-1 : 빈 공간이 충분하면 단순히 realloc한다 */
                        STL_TRY( smpReallocNthSlot( &sPageHandle,
                                                    sRowRid.mOffset,
                                                    sNeededSpace,
                                                    STL_TRUE,
                                                    &sDstRowSlot ) == STL_SUCCESS );
                    }
                    else if( (sUnusedSpace >= sRowSizeDiff - sPadSpace) &&
                             (sRetryPhase == 1) )
                    {
                        /* case 3-1 : 페이지내에 row diff 만큼의 공간이 있으면 slot의 pad 영역을 증가시킨다 */
                        STL_TRY( smdmphExtendRowSlot( &sMiniTrans,
                                                      &sPageHandle,
                                                      sRowRid.mOffset,
                                                      sRowSizeDiff - sPadSpace,
                                                      aEnv ) == STL_SUCCESS );
                        sSrcRowSlot = smpGetNthRowItem( &sPageHandle, sRowRid.mOffset );
                        sRowPtr  = sSrcRowSlot + sSrcRowHeaderSize;
                        sDstRowSlot = sSrcRowSlot;

                        KNL_BREAKPOINT( KNL_BREAKPOINT_SMDMPHUPDATE_AFTER_COMPACT, KNL_ENV(aEnv) );
                        /* extend 연산으로 rts seq가 변했을 수 있으므로 다시 할당받는다 */
                        if( SMP_IS_VALID_RTS_SEQ(sDstRtsSeq) == STL_TRUE )
                        {
                            STL_TRY( smpAllocRts( NULL,
                                                  &sPageHandle,
                                                  sTransId,
                                                  SML_MAKE_VIEW_SCN(sViewScn),
                                                  smdmphStabilizeRow,
                                                  &sDstRtsSeq,
                                                  aEnv ) == STL_SUCCESS );
                        }
                    }
                    else
                    {
                        sDstRowSlot = NULL;
                    }
                }
                else
                {
                    /* case 4 : update로 인해 row의 길이가 현재보다 더 작거나 같다면 inplace updating을 한다 */
                    sDstRowSlot = sSrcRowSlot;
                }

                if( sDstRowSlot == NULL )
                {
                    /* RTS나 Row Slot 할당에 실패하면 Compact한 후 재시도 한다 */
                    sRetryPhase++;
                    continue;
                }

                break;
            }
        }
        else
        {
            /* update 이후 크기가 한 페이지에 담을 수 없는 크기임 */
            sDstRowSlot = NULL;
            sDstRtsSeq = SMP_RTS_SEQ_NULL;
        }

        STL_ASSERT( sPadSpace >= 0 );
        if( SMDMPH_IS_RTS_BOUND( sSrcRowSlot ) == STL_TRUE )
        {
            SMDMPH_GET_RTSSEQ( sSrcRowSlot, &sSrcRtsSeq );
        }
        else
        {
            sSrcRtsSeq = SMP_RTS_SEQ_NULL;
        }

        if( sDstRowSlot != NULL )
        {
            /* 현재 페이지에서 해결 가능한 경우 */
            STL_TRY( smdmphUpdateInPage( &sMiniTrans,
                                         &sPageHandle,
                                         sLogBuf,
                                         sLogEnd - sLogBuf,
                                         &sRowRid,
                                         sSrcRowSlot,
                                         sSrcRowPieceSize,
                                         sSrcRtsSeq,
                                         sDstRowSlot,
                                         sNeededSpace,
                                         sDstRtsSeq,
                                         aValueIdx,
                                         sCurCol,
                                         sPieceColCnt,
                                         sUpdateColCnt,
                                         sFirstColOrd,
                                         sIsColSizeDiff,
                                         sTransId,
                                         sViewScn,
                                         sStmtTcn,
                                         aEnv ) == STL_SUCCESS );

            /**
             * out-row update의 경우에만 freeness를 변경한다.
             */
            
            if( (sDstRowSlot != sSrcRowSlot) &&
                (smpGetFreeness(&sPageHandle) == SMP_FREENESS_INSERTABLE) &&
                (smpIsPageOverInsertLimit( SMP_FRAME(&sPageHandle),
                                           SMD_GET_PCTFREE(aRelationHandle)) == STL_TRUE) )
            {
                STL_TRY( smdmphUpdatePageStatus( &sMiniTrans,
                                                 aRelationHandle,
                                                 sSegmentHandle,
                                                 &sPageHandle,
                                                 SMP_FREENESS_UPDATEONLY,
                                                 0,  /* aScn */
                                                 aEnv ) == STL_SUCCESS );
            }

            sState = 0;
            STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
        }
        else
        {
            /* case 4 : 현재 페이지에서 해결 못함 -> out-place update */
            /* row piece 이미지를 미리 copy해 둔다 */ 
            stlMemcpy( sRowBuf, sSrcRowSlot, sSrcRowPieceSize );
            if( SMDMPH_IS_MASTER_ROW( sSrcRowSlot ) == STL_TRUE )
            {
                /* migrated row */
                STL_TRY( smdmphMakeMigratedRow( &sMiniTrans,
                                                &sPageHandle,
                                                sTransId,
                                                sViewScn,
                                                sCommitScn,
                                                sStmtTcn,
                                                sSrcRowSlot,
                                                &sRowRid,
                                                sSrcRowPieceSize,
                                                aEnv ) == STL_SUCCESS );
                sPrevRid = sRowRid;
            }
            else
            {
                STL_ASSERT( sPrevRid.mPageId != SMP_NULL_PID );

                /* 현재의 row piece를 지운다 */
                STL_TRY( smdmphDeleteRowPiece( &sMiniTrans,
                                               aRelationHandle,
                                               &sPageHandle,
                                               &sRowRid,
                                               sTransId,
                                               sViewScn,
                                               sStmtTcn,
                                               sCommitScn,
                                               STL_TRUE, /* aIsForUpdate */
                                               aEnv ) == STL_SUCCESS );

            }

            sState = 0;
            STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

            /* 새로운 piece를 만들고 이전 piece에 연결한다 */
            if( sIsContColRow == STL_TRUE )
            {
                STL_ASSERT( sUpdateColCnt == 1 );
                STL_TRY( smdmphDeleteContColRows( aRelationHandle,
                                                  &sNextRid,
                                                  &sNextRid,
                                                  sTransId,
                                                  sViewScn,
                                                  sStmtTcn,
                                                  STL_TRUE, /* aIsForUpdate */
                                                  ( (sBeforeCol == NULL) ? NULL :
                                                    KNL_GET_BLOCK_DATA_VALUE(sBeforeCol, aValueIdx) ),
                                                  aEnv ) == STL_SUCCESS );
            }

            STL_TRY( smdmphRemakeUpdatedRowPiece( aStatement,
                                                  aRelationHandle,
                                                  sRowBuf,
                                                  sCurCol,
                                                  aValueIdx,
                                                  sPieceColCnt,
                                                  sFirstColOrd,
                                                  &sPrevRid,
                                                  &sNextRid,
                                                  &sTmpFirstRowRid,
                                                  &sTmpLastRowRid,
                                                  aEnv ) == STL_SUCCESS );
            sRowRid = sTmpLastRowRid;
        }

        for( i = 0; i < sUpdateColCnt; i++ )
        {
            STL_ASSERT( sCurCol != NULL );
            sCurCol = sCurCol->mNext;

            if( aBeforeCols != NULL )
            {
                sBeforeCol = sBeforeCol->mNext;
            }
        }

        sFirstColOrd += sPieceColCnt;
        
        if( sCurCol != NULL )
        {
            if( sNextRid.mPageId == SMP_NULL_PID )
            {
                /**
                 * Trailing null
                 */

                STL_TRY( smdmphAppendColumns( aStatement,
                                              aRelationHandle,
                                              sFirstColOrd,
                                              sCurCol,
                                              sPrevRid,
                                              sRowRid,
                                              aValueIdx,
                                              aEnv )
                         == STL_SUCCESS );
                break;
            }
        }
        else
        {
            break;
        }
        
        sPrevRid = sRowRid;
        sRowRid = sNextRid;
    } /* while */

    /**
     * after supplemental log가 기록되었다면 before를 기록한다.
     */
    if( (sSupplLogged == STL_TRUE) && (aBeforeCols != NULL) )
    {
        STL_TRY( smxmBegin( &sMiniTrans,
                            SMA_GET_TRANS_ID(aStatement),
                            smsGetSegRid(sSegmentHandle),
                            sAttr,
                            aEnv ) == STL_SUCCESS );
        sState = 1;
        
        STL_TRY( smdWriteSupplUpdateBeforeColLogs( &sMiniTrans,
                                                   aBeforeCols,
                                                   aValueIdx,
                                                   aRowRid,
                                                   aEnv )
                 == STL_SUCCESS );
        
        sState = 0;
        STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
    }
    
    STL_RAMP( RAMP_SKIP_UPDATE );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INTERNAL,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      "smdmphUpdate()",
                      sInterErrPos );
        
        KNL_SYSTEM_FATAL_DASSERT( STL_FALSE, KNL_ENV(aEnv) );
    }
    
    STL_FINISH;

    switch( sState )
    {
        case 1:
            {
                (void)smxmCommit( &sMiniTrans, aEnv );
            }
    }

    return STL_FAILURE;
}


stlStatus smdmphAppendColumns( smlStatement      * aStatement,
                               void              * aRelationHandle,
                               stlInt16            aFirstColOrd,
                               knlValueBlockList * aAppendColumns,
                               smlRid              aPrevRid,
                               smlRid              aRowRid,
                               stlInt32            aValueIdx,
                               smlEnv            * aEnv )
{
    knlValueBlockList ** sColPtrArray;
    knlValueBlockList * sCurCol = aAppendColumns;
    stlUInt32          sState = 0;
    smxlTransId        sTransId = SMA_GET_TRANS_ID( aStatement );
    smlScn             sViewScn = smxlGetTransViewScn( sTransId );
    void             * sSegmentHandle = SME_GET_SEGMENT_HANDLE(aRelationHandle);
    stlChar            sRowBuf[SMP_PAGE_SIZE];
    stlChar            sPieceBuf[SMP_PAGE_SIZE];
    stlChar          * sRowPtr;
    knlRegionMark      sMemMark;
    stlInt32           sTotalColCount = 0;
    stlInt16           sPieceColCount = 0;
    stlInt16           sSrcPieceColCount = 0;
    stlInt64           sColLen;
    stlUInt8           sColLenSize;
    stlInt16           sDstPieceSize;
    stlInt16           sSrcPieceSize;
    stlInt32           i;
    stlInt16           sMaxPieceSize;
    stlInt32           sToOrd;
    stlInt32           sFromOrd;
    smlRid             sTmpFirstRowRid;
    smlRid             sTmpLastRowRid;
    knlValueBlockList  sNullBlockList;
    knlValueBlock      sNullValueBlock;
    dtlDataValue       sNullValue;
    smxmTrans          sMiniTrans;
    stlUInt32          sAttr = SMXM_ATTR_REDO;
    smpHandle          sPageHandle;
    stlChar          * sSrcPiece;
    stlChar          * sDstPiece;
    smlRid             sCurPieceRid;
    smlRid             sNxtPieceRid = ((smlRid){ smsGetTbsId(sSegmentHandle), 0, SMP_NULL_PID });
    stlBool            sInSlotUpdate;
    stlUInt8           sDstRtsSeq;
    stlInt16           sSrcColCount = 0;
    stlBool            sIsContColLast = STL_FALSE;
    smlScn             sCommitScn;
    smxlTransId        sWaitTransId;
    stlInt16           sBasePadSpace = SMDMPH_MAX_ROW_HDR_LEN - SMDMPH_MIN_ROW_HDR_LEN;
    stlInt16           sPadSpace;
    stlInt16           sSlotSize;
    stlBool            sIsZero;
    dtlDataValue     * sDataValue;

    sMaxPieceSize = smdmphGetMaxFreeSize( aRelationHandle );

    sNullBlockList.mValueBlock = &sNullValueBlock;
    sNullValueBlock.mIsSepBuff = STL_FALSE;
    sNullValueBlock.mAllocLayer = STL_LAYER_STORAGE_MANAGER;
    sNullValueBlock.mDataValue = &sNullValue;
    
    sNullValue.mLength = 0;
    sNullValue.mValue = NULL;
    sNullValue.mBufferSize = 0;
    
    while( sCurCol->mNext != NULL )
    {
        sCurCol = sCurCol->mNext;
    }

    sTotalColCount = KNL_GET_BLOCK_COLUMN_ORDER(sCurCol) - aFirstColOrd + 1;

    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                sTotalColCount * STL_SIZEOF(knlValueBlockList*),
                                (void**)&sColPtrArray,
                                (knlEnv*)aEnv ) == STL_SUCCESS );

    sCurCol = aAppendColumns;
    for( i = 0; i < sTotalColCount; i++ )
    {
        if( (i + aFirstColOrd) == KNL_GET_BLOCK_COLUMN_ORDER(sCurCol) )
        {
            sColPtrArray[i] = sCurCol;
            sCurCol = sCurCol->mNext;
        }
        else
        {
            sColPtrArray[i] = &sNullBlockList;
        }
    }

    /* 마지막 컬럼부터 insert */
    sToOrd = sTotalColCount + aFirstColOrd - 1;
    sDstPieceSize = SMDMPH_MAX_ROW_HDR_LEN;
    while( sToOrd >= aFirstColOrd )
    {
        sRowPtr = sRowBuf + SMP_PAGE_SIZE - sBasePadSpace;
        sFromOrd = sToOrd;
        while( sFromOrd >= aFirstColOrd )
        {
            sIsZero = STL_FALSE;
            sDataValue = KNL_GET_BLOCK_DATA_VALUE( sColPtrArray[sFromOrd - aFirstColOrd],
                                                   aValueIdx );
            sColLen = sDataValue->mLength;
            
            if( DTL_NUMERIC_IS_ZERO(sDataValue->mValue, sColLen) )
            {
                if( dtlDataTypeGroup[sDataValue->mType] == DTL_GROUP_NUMBER )
                {
                    sColLen = 0;
                    sIsZero = STL_TRUE;
                }
            }
            
            SMP_GET_COLUMN_LEN_SIZE( sColLen, &sColLenSize );

            if( SMDMPH_CONT_SIZE( sColLen, sColLenSize ) > sMaxPieceSize )
            {
                if( sFromOrd == sToOrd )
                {
                    /* continuous column */
                    STL_TRY( smdmphInsertContCol( aStatement,
                                                  aRelationHandle,
                                                  aValueIdx,
                                                  sColPtrArray[sFromOrd - aFirstColOrd],
                                                  (sFromOrd == 0) ? STL_TRUE : STL_FALSE,
                                                  &sNxtPieceRid,
                                                  STL_FALSE, /* aIsForUpdate */
                                                  &sTmpFirstRowRid,
                                                  &sTmpLastRowRid,
                                                  aEnv ) == STL_SUCCESS );
                    sNxtPieceRid = sTmpFirstRowRid;
                    STL_THROW( RAMP_GOTO_PREV_COL );

                }
                else
                {
                    /* 바로 다음 컬럼부터 row 작성 */
                    sFromOrd++;
                }
                break;
            }
            else
            {
                /* atomic column */
                if( ((sDstPieceSize + sColLen + sColLenSize) > sMaxPieceSize) ||
                    (sPieceColCount >= SMD_MAX_COLUMN_COUNT_IN_PIECE) )
                {
                    /* 바로 다음 컬럼부터 row 작성 */
                    STL_ASSERT( sFromOrd < sToOrd );
                    sFromOrd++;
                    break;
                }
                /* write column */
                sRowPtr -= sColLen + sColLenSize;
                SMP_WRITE_COLUMN( sRowPtr, sDataValue->mValue, sColLen, sIsZero );
                sDstPieceSize += sColLen + sColLenSize;
                sFromOrd--;
                sPieceColCount++;
            }
        } /* while */

        if( sFromOrd < aFirstColOrd )
        {
            sFromOrd = aFirstColOrd;
        }

        if( sFromOrd > aFirstColOrd )
        {
            /**
             * insert slave piece
             */
            
            sPadSpace = 0;
            if( sNxtPieceRid.mPageId != SMP_NULL_PID )
            {
                sRowPtr -= SMDMPH_MAX_ROW_HDR_LEN;
                sSlotSize = sRowBuf + SMP_PAGE_SIZE - sBasePadSpace - sRowPtr;
            }
            else
            {
                sRowPtr -= SMDMPH_MIN_ROW_HDR_LEN;
                sSlotSize = sRowBuf + SMP_PAGE_SIZE - sBasePadSpace - sRowPtr;
                if( sSlotSize < SMDMPH_MINIMUM_ROW_PIECE_SIZE )
                {
                    sPadSpace = SMDMPH_MINIMUM_ROW_PIECE_SIZE - sSlotSize;
                    sSlotSize += sPadSpace;
                }
            }

            STL_TRY( smdmphWriteRowHeader( sRowPtr,
                                           SMP_RTS_SEQ_NULL,  /* 나중에 다시 세팅 */
                                           SML_INFINITE_SCN,
                                           aStatement->mTcn,
                                           sTransId,
                                           NULL, /* &sUndoRid를 적을 필요 없음 */
                                           sPieceColCount,
                                           sPadSpace,
                                           &sNxtPieceRid ) == STL_SUCCESS );

            SMDMPH_UNSET_MASTER_ROW( sRowPtr );
            SMDMPH_UNSET_CONT_COL( sRowPtr );

            STL_TRY( smdmphInsertRowPiece( aRelationHandle,
                                           sTransId,
                                           sViewScn,
                                           sRowPtr,
                                           sSlotSize,
                                           sFromOrd,
                                           STL_FALSE, /* aIsForUpdate */
                                           &sNxtPieceRid,
                                           aEnv ) == STL_SUCCESS );
        }
        else
        {
            /**
             * append target piece
             */
            
            break;
        }

        STL_RAMP( RAMP_GOTO_PREV_COL );

        sToOrd = sFromOrd - 1;
        sPieceColCount = 0;
        sDstPieceSize = SMDMPH_MAX_ROW_HDR_LEN;
    } /* while */

    /**
     * append target piece
     */

    if( sPieceColCount > 0 )
    {
        STL_TRY( smxmBegin( &sMiniTrans,
                            sTransId,
                            smsGetSegRid(sSegmentHandle),
                            sAttr,
                            aEnv ) == STL_SUCCESS );
        sState = 2;
            
        STL_TRY( smpAcquire( &sMiniTrans,
                             aRowRid.mTbsId,
                             aRowRid.mPageId,
                             KNL_LATCH_MODE_EXCLUSIVE,
                             &sPageHandle,
                             aEnv )
                 == STL_SUCCESS );

        sDstPiece = NULL;
        sSrcPiece = smpGetNthRowItem( &sPageHandle, aRowRid.mOffset );
        sSrcPieceSize = smdmphGetRowSize( sSrcPiece );
        SMDMPH_GET_COLUMN_CNT( sSrcPiece, &sSrcPieceColCount );
        sIsContColLast = SMDMPH_IS_CONT_COL_LAST( sSrcPiece );
        stlMemcpy( sPieceBuf, sSrcPiece, sSrcPieceSize );
        sSrcPiece = sPieceBuf;

        sDstPieceSize -= SMDMPH_MAX_ROW_HDR_LEN;
            
        if( sNxtPieceRid.mPageId != SMP_NULL_PID )
        {
            sDstPieceSize += SMDMPH_MAX_ROW_HDR_LEN - SMDMPH_MIN_ROW_HDR_LEN;
        }

        /**
         * 아래 경우는 outpage fashion으로 처리한다.
         * - 정해진 크기보다 큰 경우
         * - continuous column
         * - 한 개의 Piece에 수용할수 있는 컬럼의 개수가 초과된 경우
         */
        if( ((sDstPieceSize + sSrcPieceSize) <= sMaxPieceSize) &&
            (sIsContColLast != STL_TRUE) &&
            ((sSrcPieceColCount + sPieceColCount) <= SMD_MAX_COLUMN_COUNT_IN_PIECE) )
        {
            STL_TRY( smdmphAllocRowPiece( &sMiniTrans,
                                          &sPageHandle,
                                          sTransId,
                                          sViewScn,
                                          aRowRid,
                                          sSrcPieceSize,
                                          sDstPieceSize,
                                          &sDstPiece,
                                          &sDstRtsSeq,
                                          &sInSlotUpdate,
                                          aEnv ) == STL_SUCCESS );

            if( sDstPiece != NULL )
            {
                STL_TRY( smdmphAppendRowPiece( &sMiniTrans,
                                               &sPageHandle,
                                               sTransId,
                                               sViewScn,
                                               aStatement->mTcn,
                                               sInSlotUpdate,
                                               sSrcPieceSize,
                                               sRowPtr,                            /* aAppendPieceBody */
                                               sRowBuf + SMP_PAGE_SIZE -sBasePadSpace - sRowPtr,  /* aAppendPieceBodySize */
                                               sDstRtsSeq,
                                               sSrcPieceSize + sDstPieceSize,      /* aReallocPieceSize */
                                               aFirstColOrd,
                                               sPieceColCount,
                                               sNxtPieceRid,
                                               aRowRid,
                                               aEnv )
                         == STL_SUCCESS );
                    
                if( (smpGetFreeness(&sPageHandle) == SMP_FREENESS_INSERTABLE) &&
                    (smpIsPageOverInsertLimit( SMP_FRAME(&sPageHandle),
                                               SMD_GET_PCTFREE(aRelationHandle)) == STL_TRUE) )
                {
                    STL_TRY( smdmphUpdatePageStatus( &sMiniTrans,
                                                     aRelationHandle,
                                                     sSegmentHandle,
                                                     &sPageHandle,
                                                     SMP_FREENESS_UPDATEONLY,
                                                     0,  /* aScn */
                                                     aEnv ) == STL_SUCCESS );
                }

                sState = 1;
                STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

                STL_THROW( RAMP_SUCCESS );
            }
        }
            
        sState = 1;
        STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

        sPadSpace = 0;
        if( sNxtPieceRid.mPageId != SMP_NULL_PID )
        {
            sRowPtr -= SMDMPH_MAX_ROW_HDR_LEN;
            sSlotSize = sRowBuf + SMP_PAGE_SIZE - sBasePadSpace - sRowPtr;
        }
        else
        {
            sRowPtr -= SMDMPH_MIN_ROW_HDR_LEN;
            sSlotSize = sRowBuf + SMP_PAGE_SIZE - sBasePadSpace - sRowPtr;
            if( sSlotSize < SMDMPH_MINIMUM_ROW_PIECE_SIZE )
            {
                sPadSpace = SMDMPH_MINIMUM_ROW_PIECE_SIZE - sSlotSize;
                sSlotSize += sPadSpace;
            }
        }

        STL_TRY( smdmphWriteRowHeader( sRowPtr,
                                       SMP_RTS_SEQ_NULL,  /* 나중에 다시 세팅 */
                                       SML_INFINITE_SCN,
                                       aStatement->mTcn,
                                       sTransId,
                                       NULL, /* &sUndoRid를 적을 필요 없음 */
                                       sPieceColCount,
                                       sPadSpace,
                                       &sNxtPieceRid ) == STL_SUCCESS );

        SMDMPH_UNSET_MASTER_ROW( sRowPtr );
        SMDMPH_UNSET_CONT_COL( sRowPtr );

        /**
         * insert target piece as outpage fashion
         */
        STL_TRY( smdmphInsertRowPiece( aRelationHandle,
                                       sTransId,
                                       sViewScn,
                                       sRowPtr,
                                       sSlotSize,
                                       sFromOrd,
                                       STL_TRUE, /* aIsForUpdate */
                                       &sNxtPieceRid,
                                       aEnv ) == STL_SUCCESS );
                
    }
    else
    {
        /**
         * 모든 column이 slave piece로 저장된 경우
         */
    }
    
    STL_TRY( smxmBegin( &sMiniTrans,
                        SMA_GET_TRANS_ID(aStatement),
                        smsGetSegRid(sSegmentHandle),
                        sAttr,
                        aEnv ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( smpAcquire( &sMiniTrans,
                         aRowRid.mTbsId,
                         aRowRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv )
             == STL_SUCCESS );
            
    sSrcPiece = smpGetNthRowItem( &sPageHandle, aRowRid.mOffset );
    sSrcPieceSize = smdmphGetRowSize( sSrcPiece );
    SMDMPH_GET_COLUMN_CNT( sSrcPiece, &sSrcColCount );
    stlMemcpy( sPieceBuf, sSrcPiece, SMDMPH_MIN_ROW_HDR_LEN );
    stlMemcpy( sPieceBuf + SMDMPH_MAX_ROW_HDR_LEN,
               sSrcPiece + SMDMPH_MIN_ROW_HDR_LEN,
               sSrcPieceSize - SMDMPH_MIN_ROW_HDR_LEN );
            
    STL_TRY( smdmphGetCommitScn( aRowRid.mTbsId,
                                 &sPageHandle,
                                 sSrcPiece,
                                 &sCommitScn,
                                 &sWaitTransId,
                                 aEnv ) == STL_SUCCESS );

    if( SMDMPH_IS_MASTER_ROW( sSrcPiece ) == STL_TRUE )
    {
        /**
         * update master piece link
         */
        STL_TRY( smdmphMakeMigratedRow( &sMiniTrans,
                                        &sPageHandle,
                                        sTransId,
                                        sViewScn,
                                        sCommitScn,
                                        aStatement->mTcn,
                                        sSrcPiece,
                                        &aRowRid,
                                        sSrcPieceSize,
                                        aEnv ) == STL_SUCCESS );
        sCurPieceRid = aRowRid;
    }
    else
    {
        STL_TRY( smdmphDeleteRowPiece( &sMiniTrans,
                                       aRelationHandle,
                                       &sPageHandle,
                                       &aRowRid,
                                       sTransId,
                                       sViewScn,
                                       aStatement->mTcn,
                                       sCommitScn,
                                       STL_TRUE, /* aIsForUpdate */
                                       aEnv ) == STL_SUCCESS );
        sCurPieceRid = aPrevRid;
    }

    sState = 1;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    sPadSpace = 0;
    SMDMPH_SET_PAD_SPACE( sPieceBuf, &sPadSpace );
    SMDMPH_SET_TRANS_ID( sPieceBuf, &sTransId );
    SMDMPH_SET_TCN( sPieceBuf, &aStatement->mTcn );
    SMDMPH_UNSET_MASTER_ROW( sPieceBuf );
    SMDMPH_SET_HAS_NEXT_LINK( sPieceBuf );
    SMDMPH_SET_LINK_PID( sPieceBuf, &sNxtPieceRid.mPageId );
    SMDMPH_SET_LINK_OFFSET( sPieceBuf, &sNxtPieceRid.mOffset );

    STL_TRY( smdmphInsertRowPiece( aRelationHandle,
                                   sTransId,
                                   sViewScn,
                                   sPieceBuf,
                                   sSrcPieceSize + (SMDMPH_MAX_ROW_HDR_LEN -
                                                    SMDMPH_MIN_ROW_HDR_LEN ),
                                   aFirstColOrd - sSrcColCount,
                                   STL_TRUE, /* aIsForUpdate */
                                   &sNxtPieceRid,
                                   aEnv ) == STL_SUCCESS );

    STL_TRY( smxmBegin( &sMiniTrans,
                        SMA_GET_TRANS_ID(aStatement),
                        smsGetSegRid(sSegmentHandle),
                        sAttr,
                        aEnv ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( smdmphUpdateNextPieceRid( &sMiniTrans,
                                       sTransId,
                                       sViewScn,
                                       aStatement->mTcn,
                                       &sCurPieceRid,
                                       &sNxtPieceRid,
                                       aEnv ) == STL_SUCCESS );
            
    sState = 1;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    STL_RAMP( RAMP_SUCCESS );

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       (knlEnv*)aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) smxmRollback( &sMiniTrans, aEnv );
        case 1:
            (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                            &sMemMark,
                                            STL_FALSE, /* aFreeChunk */
                                            (knlEnv*)aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief 주어진 row rid의 row를 삭제한다
 * @param[in] aStatement 사용하는 statement
 * @param[in] aRelationHandle 삭제할 row가 존재하는 relation의 핸들
 * @param[in] aRowRid 갱신할 row의 row rid
 * @param[in] aRowScn 레코드 검색 당시의 SCN
 * @param[in] aValueIdx primary key가 사용하고 있는 block idx
 * @param[in] aPrimaryKey 해당 레코드의 primary key
 * @param[out] aVersionConflict delete 시도 도중에 version conflict 상황을 만났는지 여부
 * @param[out] aSkipped 이미 삭제된 Row인지 여부
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmphDelete( smlStatement      * aStatement,
                        void              * aRelationHandle,
                        smlRid            * aRowRid,
                        smlScn              aRowScn,
                        stlInt32            aValueIdx,
                        knlValueBlockList * aPrimaryKey,
                        stlBool           * aVersionConflict,
                        stlBool           * aSkipped,
                        smlEnv            * aEnv )
{
    smlRid             sRowRid = *aRowRid;
    smlRid             sNextPieceRid;
    stlUInt32          sAttr = SMXM_ATTR_REDO;
    void             * sSegmentHandle = SME_GET_SEGMENT_HANDLE(aRelationHandle);
    stlUInt32          sState = 0;
    smxmTrans          sMiniTrans;
    smpHandle          sPageHandle;
    stlChar          * sSrcRowSlot = NULL;
    smlScn             sCommitScn;
    smxlTransId        sTransId = SML_INVALID_TRANSID;
    smlScn             sViewScn;
    smlTcn             sStmtTcn = SML_INVALID_TCN;
    smlTcn             sRowTcn = 0;
    smxlTransId        sWaitTransId;
    stlBool            sIsFirstPiece = STL_TRUE;
    stlBool            sIsMyTrans = STL_FALSE;
    stlBool            sIsMyLock = STL_FALSE;
    void             * sLockItem = NULL;
    stlBool            sSupplLogged = STL_FALSE;
    smlTbsId           sTbsId = smsGetTbsId(sSegmentHandle);

    STL_DASSERT( aRowScn != 0 );
    *aVersionConflict = STL_FALSE;
    
    sTransId = SMA_GET_TRANS_ID(aStatement);
    sStmtTcn = SMA_GET_TCN( aStatement );

    while( sRowRid.mPageId != SMP_NULL_PID )
    {
        STL_TRY( smxmBegin( &sMiniTrans,
                            SMA_GET_TRANS_ID(aStatement),
                            smsGetSegRid(sSegmentHandle),
                            sAttr,
                            aEnv ) == STL_SUCCESS );
        sState = 1;

        /* 주어진 페이지를 얻는다 */
        STL_TRY( smpAcquire( &sMiniTrans,
                             sTbsId,
                             sRowRid.mPageId,
                             KNL_LATCH_MODE_EXCLUSIVE,
                             &sPageHandle,
                             aEnv ) == STL_SUCCESS );

        sSrcRowSlot = smpGetNthRowItem( &sPageHandle, sRowRid.mOffset );

        STL_TRY( smdmphGetCommitScn( sTbsId,
                                     &sPageHandle,
                                     sSrcRowSlot,
                                     &sCommitScn,
                                     &sWaitTransId,
                                     aEnv ) == STL_SUCCESS );

        /* My Transaction은 통과 */
        sIsMyTrans = SMDMPH_IS_SAME_TRANS( sTransId,
                                           SML_INFINITE_SCN, /* DML 중이라 내 커밋SCN은 무한대이다 */
                                           sWaitTransId,
                                           sCommitScn );
                
        /* My Transaction은 통과 */
        if( (sIsMyTrans == STL_FALSE) && (sIsFirstPiece == STL_TRUE) )
        {
            sLockItem = NULL;
                
            while( STL_TRUE )
            {
                /* 최종 버전이 Commit 된 레코드임 */
                if( sCommitScn != SML_INFINITE_SCN )
                {
                    /* Lock Bit이 설정되어 있지 않다면 대기할 필요 없다. */
                    if( SMDMPH_IS_LOCKED( sSrcRowSlot ) == STL_FALSE )
                    {
                        break;
                    }

                    /* 이미 Unlock된 경우라면 대기할 필요 없다. */
                    STL_TRY( smklIsActiveLockRecord( sTransId,
                                                     sRowRid,
                                                     &sLockItem,
                                                     &sIsMyLock,
                                                     aEnv )
                             == STL_SUCCESS );

                    if( sLockItem == NULL )
                    {
                        /* Lock Bit를 Clear 한다. */
                        SMDMPH_UNSET_LOCKED( sSrcRowSlot );
                        break;
                    }

                    if( sIsMyLock == STL_TRUE )
                    {
                        break;
                    }
                }

                sState = 0;
                STL_TRY( smxmRollback( &sMiniTrans, aEnv ) == STL_SUCCESS );

                if( sLockItem != NULL )
                {
                    STL_TRY( smklWaitRecord( sTransId,
                                             SML_LOCK_X,
                                             sRowRid,
                                             SML_LOCK_TIMEOUT_INFINITE,
                                             aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    /* sWaitTransId가 종료될때까지 기다린다 */
                    STL_TRY( smxlWaitTrans( sTransId,
                                            sWaitTransId,
                                            SMA_GET_LOCK_TIMEOUT(aStatement),
                                            aEnv )
                             == STL_SUCCESS );
                }

                /* version conflict 발생. 상위에서 filter 적용 후 통과하면 stmt scn을 증가시키고 retry */
                *aVersionConflict = STL_TRUE;

                STL_THROW( RAMP_SKIP_DELETE );
            }

            sIsFirstPiece = STL_FALSE;
        }

        SMDMPH_GET_TCN( sSrcRowSlot, &sRowTcn);
        
        /**
         * Version Conflict 검사
         */
        if( sIsMyTrans == STL_FALSE )
        {
            if( sCommitScn > aRowScn )
            {
                /* version conflict 발생. 상위에서 filter 적용 후 통과하면 stmt scn을 증가시키고 retry */
                *aVersionConflict = STL_TRUE;
            
                sState = 0;
                STL_TRY( smxmRollback( &sMiniTrans, aEnv ) == STL_SUCCESS );

                STL_THROW( RAMP_SKIP_DELETE );
            }
        }
        else
        {
            if( (sCommitScn == SML_INFINITE_SCN) && (sRowTcn == sStmtTcn) )
            {
                /**
                 * 이미 해당 Statement에서 갱신한 레코드이다.
                 */
                sState = 0;
                STL_TRY( smxmRollback( &sMiniTrans, aEnv ) == STL_SUCCESS );

                *aSkipped = STL_TRUE;

                STL_THROW( RAMP_SKIP_DELETE );
            }
        }

        /**
         * verify row
         */
        if( SMDMPH_IS_DELETED( sSrcRowSlot ) == STL_TRUE )
        {
            sState = 0;
            STL_TRY( smxmRollback( &sMiniTrans, aEnv ) == STL_SUCCESS );

            STL_THROW( RAMP_ERR_INTERNAL );
        }
        
        /**
         * Supplemental Log를 기록한다.
         */
        if( (aPrimaryKey != NULL) && (sSupplLogged == STL_FALSE) )
        {
            STL_TRY( smdWriteSupplementalLog( &sMiniTrans,
                                              SML_SUPPL_LOG_DELETE,
                                              aValueIdx,
                                              aPrimaryKey,
                                              NULL, /* aAfterCols */
                                              aRowRid,
                                              aEnv )
                     == STL_SUCCESS );
            
            sSupplLogged = STL_TRUE;
        }

        sViewScn = smxlGetTransViewScn( aStatement->mTransId );
        
        /* next row piece 위치를 미리 구해둔다 */
        if( SMDMPH_HAS_NEXT_LINK( sSrcRowSlot ) == STL_TRUE )
        {
            sNextPieceRid.mTbsId = sRowRid.mTbsId;
            SMDMPH_GET_LINK_PID( sSrcRowSlot, &sNextPieceRid.mPageId );
            SMDMPH_GET_LINK_OFFSET( sSrcRowSlot, &sNextPieceRid.mOffset );
        }
        else
        {
            sNextPieceRid.mTbsId = sRowRid.mTbsId;
            sNextPieceRid.mPageId = SMP_NULL_PID;
            sNextPieceRid.mOffset = 0;
        }

        /* row piece에 delete mark를 한다 */
        STL_TRY( smdmphDeleteRowPiece( &sMiniTrans,
                                       aRelationHandle,
                                       &sPageHandle,
                                       &sRowRid,
                                       sTransId,
                                       sViewScn,
                                       sStmtTcn,
                                       sCommitScn,
                                       STL_FALSE, /* aIsForUpdate */
                                       aEnv ) == STL_SUCCESS );

        sState = 0;
        STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

        sRowRid = sNextPieceRid;
    }

    STL_RAMP( RAMP_SKIP_DELETE );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INTERNAL,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      "smdmphDelete()",
                      0 );
        
        KNL_SYSTEM_FATAL_DASSERT( STL_FALSE, KNL_ENV(aEnv) );
    }
    
    STL_FINISH;

    switch( sState )
    {
        case 1:
            {
                (void)smxmCommit( &sMiniTrans, aEnv );
            }
    }

    return STL_FAILURE;
}

    

/**
 * @brief 주어진 row rid의 row piece를 삭제한다
 * @param[in] aMiniTrans 페이지 접근과 로깅시 사용할 mini transaction
 * @param[in] aRelationHandle 삭제할 row가 있는 Table의 handle
 * @param[in] aPageHandle 삭제할 row가 있는 페이지의 handle
 * @param[in] aRowRid 삭제할 row의 row rid
 * @param[in] aMyTransId delete를 수행하는 transaction의 ID
 * @param[in] aMyViewScn delete를 수행하는 Statement의 view scn
 * @param[in] aMyTcn delete를 수행하는 Statement의 tcn
 * @param[in] aCommitScn 삭제할 row의 commit scn
 * @param[in] aIsForUpdate Update 연산 중에 발생한 row piece delete 인지 여부
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmphDeleteRowPiece( smxmTrans         * aMiniTrans,
                                void              * aRelationHandle,
                                smpHandle         * aPageHandle,
                                smlRid            * aRowRid,
                                smxlTransId         aMyTransId,
                                smlScn              aMyViewScn,
                                smlTcn              aMyTcn,
                                smlScn              aCommitScn,
                                stlBool             aIsForUpdate,
                                smlEnv            * aEnv )
{
    stlInt16           sSrcRowHeaderSize;
    void             * sSegmentHandle = SME_GET_SEGMENT_HANDLE(aRelationHandle);
    stlChar            sLogBuf[SMP_PAGE_SIZE];
    stlChar          * sLogPtr;
    smlRid             sUndoRid;
    stlUInt8           sSrcRtsSeq;
    stlUInt8           sDstRtsSeq;
    stlUInt8           sRtsVerNo;
    stlInt16           sRetryCnt;
    stlChar          * sRowSlot;
    stlInt16           sTotalRowSize;
    smlScn             sScn;
    stlInt16           sPadSpace;
    stlInt16           sLogType = (aIsForUpdate == STL_TRUE) ?
        SMR_LOG_MEMORY_HEAP_DELETE_FOR_UPDATE : SMR_LOG_MEMORY_HEAP_DELETE;

    sRowSlot = smpGetNthRowItem( aPageHandle, aRowRid->mOffset );
    SMDMPH_GET_ROW_HDR_SIZE( sRowSlot, &sSrcRowHeaderSize );
    sTotalRowSize =  smdmphGetRowSize(sRowSlot);

    /* Undo Log 부터 작성 */
    sLogPtr  = sLogBuf;
    stlMemcpy( sLogPtr, sRowSlot, sSrcRowHeaderSize );
    /* Undo log의 row header는 가능한 경우(commit된 다른 trans의 row) stamping 한다 */
    if( SMDMPH_IS_RTS_BOUND( sLogPtr) == STL_TRUE )
    {
        if( aCommitScn < SML_INFINITE_SCN )
        {
            /* committed trans */
            SMDMPH_UNSET_RTS_BOUND( sLogPtr );
            SMDMPH_SET_SCN( sLogPtr, &aCommitScn );
        }
    }
    sLogPtr += sSrcRowHeaderSize;

    /* DS  mode가 아니면 */
    if( SMXL_IS_UNDO_BOUND_TRANS( aMyTransId ) == STL_TRUE )
    {
        sRetryCnt = 0;
        while( STL_TRUE )
        {
            STL_TRY( smpAllocRts( NULL,
                                  aPageHandle,
                                  aMyTransId,
                                  SML_MAKE_VIEW_SCN(aMyViewScn),
                                  smdmphStabilizeRow,
                                  &sDstRtsSeq,
                                  aEnv ) == STL_SUCCESS );

            if( (SMP_IS_VALID_RTS_SEQ(sDstRtsSeq) != STL_TRUE) && (sRetryCnt == 0) )
            {
                /* 처음으로 공간부족 발생. page를 일단 compact하여 최대한 빈 공간을 확보하고 진행 */
                sRetryCnt++;
                STL_TRY( smdmphCompactPage( aMiniTrans,
                                            aPageHandle,
                                            aEnv )
                         == STL_SUCCESS );
                
                /* compact로 source row의 위치가 변경되었을 수 있으므로 다시 구한다 */
                sRowSlot = smpGetNthRowItem( aPageHandle, aRowRid->mOffset );
                continue;
            }

            break;
        }
    }
    else
    {
        sDstRtsSeq = SMP_RTS_SEQ_NULL;
    }

    /* write undo record */
    STL_TRY( smxlInsertUndoRecord( aMiniTrans,
                                   SMG_COMPONENT_TABLE,
                                   SMD_UNDO_LOG_MEMORY_HEAP_DELETE,
                                   sLogBuf,
                                   sLogPtr - sLogBuf,
                                   ((smlRid){ aRowRid->mTbsId,
                                              aRowRid->mOffset, /* slot seq */
                                              aRowRid->mPageId }),
                                   &sUndoRid,
                                   aEnv ) == STL_SUCCESS );

    smpSetMaxViewScn( aPageHandle, aMyViewScn );

    if( SMDMPH_IS_RTS_BOUND( sRowSlot ) == STL_TRUE )
    {
        SMDMPH_GET_RTSSEQ( sRowSlot, &sSrcRtsSeq );
    }
    else
    {
        sSrcRtsSeq = SMP_RTS_SEQ_NULL;
    }

    if( SMP_IS_VALID_RTS_SEQ(sDstRtsSeq) == STL_TRUE )
    {    
        /* RTS 조정 */
        STL_TRY( smpBeginUseRts( aPageHandle,
                                 sRowSlot,
                                 aMyTransId,
                                 SML_MAKE_VIEW_SCN(aMyViewScn),
                                 sDstRtsSeq,
                                 &sRtsVerNo,
                                 aEnv ) == STL_SUCCESS );
        SMDMPH_SET_RTS_BOUND( sRowSlot );
        SMDMPH_SET_RTSSEQ( sRowSlot, &sDstRtsSeq );
    }
    else
    {
        if( SMXL_IS_UNDO_BOUND_TRANS( aMyTransId ) == STL_TRUE )
        {
            sScn = SML_MAKE_VIEW_SCN( aMyViewScn );
            SMDMPH_UNSET_RTS_BOUND( sRowSlot );
            SMDMPH_SET_SCN( sRowSlot, &sScn );
        }
        else
        {
            /* DS mode */
            sScn = 0;
            SMDMPH_UNSET_RTS_BOUND( sRowSlot );
            SMDMPH_SET_SCN( sRowSlot, &sScn );
        }
    }

    if( SMP_IS_VALID_RTS_SEQ(sSrcRtsSeq) == STL_TRUE )
    {
        STL_TRY( smpEndUseRts( aPageHandle,
                               sRowSlot,
                               sSrcRtsSeq,
                               0,
                               aEnv ) == STL_SUCCESS );
    }

    /* Set Delete bit & trans info */
    SMDMPH_SET_DELETED( sRowSlot );
    SMDMPH_SET_TCN( sRowSlot, &aMyTcn );
    SMDMPH_SET_TRANS_ID( sRowSlot, &aMyTransId );
    SMDMPH_SET_ROLLBACK_PID( sRowSlot, &sUndoRid.mPageId );
    SMDMPH_SET_ROLLBACK_OFFSET( sRowSlot, &sUndoRid.mOffset );

    /* Write REDO log */
    sLogPtr = sLogBuf;
    STL_WRITE_INT64( sLogPtr, &aMyTransId );
    sLogPtr += STL_SIZEOF(smxlTransId);
    STL_WRITE_INT64( sLogPtr, &aMyViewScn );
    sLogPtr += STL_SIZEOF(smlScn);
    stlMemcpy( sLogPtr, sRowSlot, sSrcRowHeaderSize );
    sLogPtr += sSrcRowHeaderSize;

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_TABLE,
                           sLogType,
                           SMR_REDO_TARGET_PAGE,
                           sLogBuf,
                           sLogPtr - sLogBuf,
                           aRowRid->mTbsId,
                           aRowRid->mPageId,
                           aRowRid->mOffset,  /* Physiological redo를 하기위해 offset이 아닌 seq 사용 */
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    SMDMPH_GET_PAD_SPACE( sRowSlot, &sPadSpace );
    
    /* delete 되었으니 reclaimed space에 일단 추가한다 */
    SMP_ADD_RECLAIMED_SPACE( SMP_FRAME(aPageHandle), sTotalRowSize + sPadSpace );
    SMP_DECREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) );

    if( SMP_GET_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) ) == 0 )
    {
        STL_TRY( smdmphUpdatePageStatus( aMiniTrans,
                                         aRelationHandle,
                                         sSegmentHandle,
                                         aPageHandle,
                                         SMP_FREENESS_FREE,
                                         smxlGetSystemScn(),
                                         aEnv ) == STL_SUCCESS );
    }
    else
    {
        if( (smpGetFreeness(aPageHandle) == SMP_FREENESS_UPDATEONLY) &&
            (smpIsPageOverFreeLimit(SMP_FRAME(aPageHandle),
                                    SMD_GET_PCTUSED(aRelationHandle)) == STL_TRUE) )
        {
            STL_TRY( smdmphUpdatePageStatus( aMiniTrans,
                                             aRelationHandle,
                                             sSegmentHandle,
                                             aPageHandle,
                                             SMP_FREENESS_INSERTABLE,
                                             0,  /* aScn */
                                             aEnv ) == STL_SUCCESS );
        }
    }

    smdmphValidateTablePage( aPageHandle, aEnv );
    
    STL_DASSERT( smpGetSegmentId( aPageHandle ) ==
                 smsGetSegmentId( sSegmentHandle ) );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdmphDeleteContColRows( void            * aRelationHandle,
                                   smlRid          * aStartRid,
                                   smlRid          * aNextColRid,
                                   smxlTransId       aMyTransId,
                                   smlScn            aMyViewScn,
                                   smlTcn            aMyTcn,
                                   stlBool           aIsForUpdate,
                                   dtlDataValue    * aBeforeValue,
                                   smlEnv          * aEnv )
{
    stlUInt32          sAttr = SMXM_ATTR_REDO;
    void             * sSegmentHandle = SME_GET_SEGMENT_HANDLE(aRelationHandle);
    stlUInt32          sState = 0;
    smxmTrans          sMiniTrans;
    smpHandle          sPageHandle;
    smlTbsId           sTbsId = smsGetTbsId(sSegmentHandle);
    stlChar          * sRowSlot;
    stlChar          * sColPtr;
    stlBool            sHasNextPiece = STL_TRUE;
    stlBool            sIsContColRow;
    smxlTransId        sWaitTransId;
    smlScn             sCommitScn;
    stlUInt16          sSlotHdrSize;
    stlUInt8           sColLenSize = 0;
    stlInt64           sColLen = 0;
    stlBool            sIsZero;
    stlInt32           sAllocSize;

    while( sHasNextPiece == STL_TRUE )
    {
        STL_TRY( smxmBegin( &sMiniTrans,
                            aMyTransId,
                            smsGetSegRid(sSegmentHandle),
                            sAttr,
                            aEnv ) == STL_SUCCESS );
        sState = 1;

        /* 다음 piece의 페이지를 얻는다 */
        STL_TRY( smpAcquire( &sMiniTrans,
                             sTbsId,
                             aNextColRid->mPageId,
                             KNL_LATCH_MODE_EXCLUSIVE,
                             &sPageHandle,
                             aEnv ) == STL_SUCCESS );

        sRowSlot = smpGetNthRowItem( &sPageHandle, aNextColRid->mOffset );
        sIsContColRow = SMDMPH_IS_CONT_COL( sRowSlot );

        STL_TRY( smdmphGetCommitScn( sTbsId,
                                     &sPageHandle,
                                     sRowSlot,
                                     &sCommitScn,
                                     &sWaitTransId,
                                     aEnv ) == STL_SUCCESS );

        if( aBeforeValue != NULL )
        {
            SMDMPH_GET_ROW_HDR_SIZE( sRowSlot, &sSlotHdrSize );
            sColPtr = sRowSlot + sSlotHdrSize;
            
            SMP_GET_COLUMN_LEN( sColPtr, &sColLenSize, &sColLen, sIsZero );
            STL_ASSERT( sIsZero == STL_FALSE );

            if( aBeforeValue->mBufferSize < (aBeforeValue->mLength + sColLen) )
            {
                if( (aBeforeValue->mType == DTL_TYPE_LONGVARCHAR) ||
                    (aBeforeValue->mType == DTL_TYPE_LONGVARBINARY) )
                {
                    if( aBeforeValue->mType == DTL_TYPE_LONGVARCHAR )
                    {
                        sAllocSize = STL_MIN( (aBeforeValue->mBufferSize + (aBeforeValue->mLength + sColLen)) * 2,
                                              DTL_LONGVARCHAR_MAX_PRECISION );
                    }
                    else
                    {
                        sAllocSize = STL_MIN( (aBeforeValue->mBufferSize + (aBeforeValue->mLength + sColLen)) * 2,
                                              DTL_LONGVARBINARY_MAX_PRECISION );
                    }
                            
                    STL_TRY( knlReallocAndMoveLongVaryingMem( aBeforeValue,
                                                              sAllocSize,
                                                              KNL_ENV(aEnv) )
                             == STL_SUCCESS );
                }
            }
            
            stlMemcpy( aBeforeValue->mValue + aBeforeValue->mLength,
                       sColPtr + sColLenSize,
                       sColLen );
                
            aBeforeValue->mLength += sColLen;
        }

        /* row piece에 delete mark를 한다 */
        STL_TRY( smdmphDeleteRowPiece( &sMiniTrans,
                                       aRelationHandle,
                                       &sPageHandle,
                                       aNextColRid,
                                       aMyTransId,
                                       aMyViewScn,
                                       aMyTcn,
                                       sCommitScn,
                                       aIsForUpdate,
                                       aEnv ) == STL_SUCCESS );

        /* next row piece 위치를 미리 구해둔다 */
        sHasNextPiece = SMDMPH_HAS_NEXT_LINK( sRowSlot );
        if( sHasNextPiece == STL_TRUE )
        {
            SMDMPH_GET_LINK_PID( sRowSlot, &aNextColRid->mPageId );
            SMDMPH_GET_LINK_OFFSET( sRowSlot, &aNextColRid->mOffset );
        }
        else
        {
            aNextColRid->mPageId = SMP_NULL_PID;
            aNextColRid->mOffset = 0;
        }

        sState = 0;
        STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

        if( sIsContColRow != STL_TRUE )
        {
            /* 연속된 continuous column과 마지막의 non-continuous column을 지우고 나간다 */
            break;
        }
    } /* while */

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smxmCommit( &sMiniTrans, aEnv );
    }

    return STL_FAILURE;
}


/**
 * @brief 레코드 한 건을 Fetch한다.
 * @param[in]  aStatement       Statement
 * @param[in]  aFetchRecordInfo Fetch Record 정보
 * @param[in]  aRowRid          검색할 레코드 아이디
 * @param[in]  aBlockIdx        현재 갱신 대상 row가 사용하고 있는 block idx
 * @param[out] aIsMatched       Physical Filter에 부합하는 레코드 존재 여부
 * @param[out] aDeleted         레코드 삭제 여부
 * @param[in,out] aEnv          Environment 구조체
 * @remark
 *     @a aAfterCols : 변경될 모든 컬럼은 컬럼 아이디순에 의해서 정렬되어 있어야 한다.
 * @note TransReadMode가 SML_TRM_SNAPSHOT일 경우는 aDeleted를 NULL로 설정해야 한다.
 * @par Example:
 * <table>
 *     <caption align="TOP"> SQL에서 Read Mode의 사용 </caption>
 *     <tr>
 *         <td>  </td>
 *         <th>  SML_TRM_SNAPSHOT  </th>
 *         <th>  SML_TRM_RECENT  </th>
 *     </tr>
 *     <tr>
 *         <th>  SML_SRM_SNAPSHOT </th>
 *         <td>  SELECT </td>
 *         <td>  DML </td>
 *     </tr>
 *     <tr>
 *         <th>  SML_SRM_RECENT </th>
 *         <td>  X </td>
 *         <td>  DDL, FOREIGN CONSTRAINT 검사 </td>
 *     </tr>
 * </table>
 */
stlStatus smdmphFetchRecord( smlStatement        * aStatement,
                             smlFetchRecordInfo  * aFetchRecordInfo,
                             smlRid              * aRowRid,
                             stlInt32              aBlockIdx,
                             stlBool             * aIsMatched,
                             stlBool             * aDeleted,
                             smlEnv              * aEnv )
{
    stlBool           sDeleted = STL_FALSE;
    smlScn            sRowScn;

    STL_PARAM_VALIDATE( aStatement != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aFetchRecordInfo != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aFetchRecordInfo->mRelationHandle != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aRowRid != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aIsMatched != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aFetchRecordInfo->mScnBlock != NULL, KNL_ERROR_STACK( aEnv ) );

    if( aDeleted != NULL )
    {
        *aDeleted = STL_FALSE;
    }
    
    if( aFetchRecordInfo->mTransReadMode == SML_TRM_SNAPSHOT )
    {
        aFetchRecordInfo->mViewScn = aStatement->mScn;
    }
    else
    {
        aFetchRecordInfo->mViewScn = smxlGetSystemScn();
    }

    if( aFetchRecordInfo->mStmtReadMode == SML_SRM_SNAPSHOT )
    {
        aFetchRecordInfo->mViewTcn = aStatement->mTcn;
    }
    else
    {
        aFetchRecordInfo->mViewTcn = SML_INFINITE_TCN;
    }

    sRowScn = aFetchRecordInfo->mScnBlock[ aBlockIdx ];
    STL_TRY( smdmphFetch( aStatement,
                          aFetchRecordInfo,
                          aRowRid,
                          aBlockIdx,
                          aIsMatched,
                          & sDeleted,
                          NULL, /* aUpdated */
                          aEnv ) == STL_SUCCESS );

    if( sDeleted == STL_TRUE )
    {
        /**
         * 삭제된 레코드를 만날수 있는 경우는 READ COMMITTED VIEW를 원하는 경우여야 한다.
         */
        KNL_ASSERT( aFetchRecordInfo->mTransReadMode == SML_TRM_COMMITTED,
                    KNL_ENV(aEnv),
                    ( "VIEW_SCN(%ld), ITERATOR ROW_SCN(%ld), FETCH ROW SCN(%ld)\n",
                      aFetchRecordInfo->mViewScn,
                      sRowScn,
                      aFetchRecordInfo->mScnBlock[ aBlockIdx ] ) );
    }
    
    if( aDeleted != NULL )
    {
        *aDeleted = sDeleted;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdmphFetchRecord4Index( smiIterator         * aIterator,
                                   void                * aBaseRelHandle,
                                   smlFetchRecordInfo  * aFetchRecordInfo,
                                   smpHandle           * aTablePageHandle,
                                   stlBool             * aTablePageLatchReleased,
                                   smlRid              * aRowRid,
                                   stlInt32              aBlockIdx,
                                   smlScn                aAgableViewScn,
                                   stlBool             * aIsMatched,
                                   smlEnv              * aEnv )
{
    smlScn       sCommitScn;
    smxlTransId  sWaitTransId;
    stlBool      sHasValidVersion = STL_FALSE;
    stlChar    * sRow;
    smdRowInfo   sRowInfo;
    smlTcn       sRowTcn;

    STL_PARAM_VALIDATE( aIterator != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aFetchRecordInfo != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aFetchRecordInfo->mRelationHandle != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aTablePageHandle != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aTablePageLatchReleased != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aRowRid != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aIsMatched != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aFetchRecordInfo->mScnBlock != NULL, KNL_ERROR_STACK( aEnv ) );

    aFetchRecordInfo->mViewScn = aIterator->mViewScn;
    aFetchRecordInfo->mViewTcn = aIterator->mViewTcn;

    aFetchRecordInfo->mScnBlock[aBlockIdx] = SML_INFINITE_SCN;

    /**
     * Get Valid Version
     */
    
    sRow = smpGetNthRowItem( aTablePageHandle, aRowRid->mOffset );

    sRowInfo.mPageHandle = aTablePageHandle;
    sRowInfo.mTbsId = aRowRid->mTbsId;
    sRowInfo.mRow = sRow;
    sRowInfo.mTransId = SMA_GET_TRANS_ID( aIterator->mStatement );
    sRowInfo.mViewScn = aFetchRecordInfo->mViewScn;
    sRowInfo.mCommitScn = SMA_GET_COMMIT_SCN( aIterator->mStatement );
    sRowInfo.mTcn = aFetchRecordInfo->mViewTcn;
    sRowInfo.mColList = aFetchRecordInfo->mColList;
    sRowInfo.mRowIdColList = aFetchRecordInfo->mRowIdColList;
    sRowInfo.mTableLogicalId = aFetchRecordInfo->mTableLogicalId;
    sRowInfo.mBlockIdx = aBlockIdx;
    
    if( SMXL_IS_AGABLE_VIEW_SCN( aAgableViewScn, smpGetMaxViewScn(aTablePageHandle) ) &&
        smdmphIsSimplePage(aTablePageHandle) )
    {
        STL_TRY( smdmphGetValidVersionSimplePage( aRowRid,
                                                  &sRowInfo,
                                                  aFetchRecordInfo->mPhysicalFilter,
                                                  aFetchRecordInfo->mLogicalFilterEvalInfo,
                                                  &sCommitScn,
                                                  &sWaitTransId,
                                                  &sHasValidVersion,
                                                  aIsMatched,
                                                  aTablePageLatchReleased,
                                                  aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( smdmphGetValidVersion( aRowRid,
                                        &sRowInfo,
                                        aFetchRecordInfo->mPhysicalFilter,
                                        aFetchRecordInfo->mLogicalFilterEvalInfo,
                                        &sCommitScn,
                                        &sRowTcn,
                                        &sWaitTransId,
                                        &sHasValidVersion,
                                        aIsMatched,
                                        aTablePageLatchReleased,
                                        aEnv )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdmphFetch( smlStatement        * aStatement,
                       smlFetchRecordInfo  * aFetchRecordInfo,
                       smlRid              * aRowRid,
                       stlInt32              aBlockIdx,
                       stlBool             * aIsMatched,
                       stlBool             * aDeleted,
                       stlBool             * aUpdated,
                       smlEnv              * aEnv )
{
    smpHandle    sPageHandle;
    smlScn       sCommitScn;
    smxlTransId  sWaitTransId;
    stlBool      sHasValidVersion = STL_FALSE;
    stlChar    * sRow;
    stlInt32     sState = 0;
    stlBool      sLatchReleased = STL_FALSE;
    smdRowInfo   sRowInfo;
    smlTcn       sRowTcn;

    STL_PARAM_VALIDATE( aStatement != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aFetchRecordInfo != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aFetchRecordInfo->mScnBlock != NULL, KNL_ERROR_STACK( aEnv ) );

    aFetchRecordInfo->mScnBlock[aBlockIdx] = SML_INFINITE_SCN;
    
    STL_TRY( smpAcquire( NULL,
                         aRowRid->mTbsId,
                         aRowRid->mPageId,
                         KNL_LATCH_MODE_SHARED,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );
    sState = 1;

    /**
     * Get Valid Version
     */
    
    sRow = smpGetNthRowItem( &sPageHandle, aRowRid->mOffset );

    sRowInfo.mPageHandle = &sPageHandle;
    sRowInfo.mTbsId = aRowRid->mTbsId;
    sRowInfo.mRow = sRow;
    sRowInfo.mTransId = SMA_GET_TRANS_ID( aStatement );
    sRowInfo.mViewScn = aFetchRecordInfo->mViewScn;
    sRowInfo.mCommitScn = SMA_GET_COMMIT_SCN( aStatement );
    sRowInfo.mTcn = aFetchRecordInfo->mViewTcn;
    sRowInfo.mColList = aFetchRecordInfo->mColList;
    sRowInfo.mRowIdColList = aFetchRecordInfo->mRowIdColList;
    sRowInfo.mTableLogicalId = aFetchRecordInfo->mTableLogicalId;    
    sRowInfo.mBlockIdx = aBlockIdx;
    
    STL_TRY( smdmphGetValidVersion( aRowRid,
                                    &sRowInfo,
                                    aFetchRecordInfo->mPhysicalFilter,
                                    aFetchRecordInfo->mLogicalFilterEvalInfo,
                                    &sCommitScn,
                                    &sRowTcn,
                                    &sWaitTransId,
                                    &sHasValidVersion,
                                    aIsMatched,
                                    &sLatchReleased,
                                    aEnv )
             == STL_SUCCESS );

    sState = 0;
    if( sLatchReleased == STL_FALSE )
    {
        STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
    }

    if( aUpdated != NULL )
    {
        *aUpdated = STL_FALSE;
    }
    
    if( sHasValidVersion == STL_FALSE )
    {
        STL_PARAM_VALIDATE( aDeleted != NULL, KNL_ERROR_STACK(aEnv) );
        *aDeleted = STL_TRUE;
    }
    else
    {
        if( sCommitScn == SML_INFINITE_SCN )
        {
            /**
             * My Transaction인 경우는 infinite가 될수 있으며, 이러한 경우는
             * View SCN을 사용한다.
             */
            sCommitScn = SMA_GET_STMT_VIEW_SCN( aStatement );
        }

        if( aUpdated != NULL )
        {
            if( SMA_GET_STMT_VIEW_SCN( aStatement ) < sCommitScn )
            {
                /**
                 * Scan 이후에 트랜잭션에 의해서 갱신된 경우
                 */
                *aUpdated = STL_TRUE;
            }
            else
            {
                if( SMA_GET_STMT_VIEW_SCN( aStatement ) == sCommitScn )
                {
                    if( SMA_GET_TCN( aStatement ) < sRowTcn )
                    {
                        /**
                         * Scan 이후에 My transaction에 의해서 갱신된 경우
                         */
                        *aUpdated = STL_TRUE;
                    }
                }
            }
        }

        /**
         * DDL statement의 경우 statement view scn보다 큰 Commit SCN을 볼수 있으며,
         * 이러한 경우는 최대값을 반환해야 한다.
         * 만약 Commit SCN 이 '0'인 경우는 statement view scn을 반환한다.
         * 즉, statement view scn 이후의 변경사항은 version conflict로 간주한다.
         */
        sCommitScn = STL_MAX( sCommitScn,
                              SMA_GET_STMT_VIEW_SCN( aStatement ) );
        
        aFetchRecordInfo->mScnBlock[aBlockIdx] = sCommitScn;
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        if( sLatchReleased == STL_FALSE )
        {
            (void)smpRelease( &sPageHandle, aEnv );
        }
    }

    return STL_FAILURE;
}


stlStatus smdmphFetchWithRowid( smlStatement        * aStatement,
                                smlFetchRecordInfo  * aFetchRecordInfo,
                                stlInt64              aSegmentId,
                                stlInt64              aSegmentSeq,
                                smlRid              * aRowRid,
                                stlInt32              aBlockIdx,
                                stlBool             * aIsMatched,
                                stlBool             * aDeleted,
                                smlEnv              * aEnv )
{
    smpHandle          sPageHandle;
    smlScn             sCommitScn;
    smxlTransId        sWaitTransId;
    stlBool            sHasValidVersion = STL_FALSE;
    stlChar          * sRow;
    stlInt32           sState = 0;
    stlBool            sLatchReleased = STL_FALSE;
    smdRowInfo         sRowInfo;
    smpPhyHdr        * sPhyHdr;
    smlDatafileId      sDatafileId;
    stlInt64           sPageSeqId;
    smlTcn             sRowTcn;
    smdmphLogicalHdr * sLogicalHdr;

    STL_PARAM_VALIDATE( aStatement != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aFetchRecordInfo != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aFetchRecordInfo->mScnBlock != NULL, KNL_ERROR_STACK( aEnv ) );

    aFetchRecordInfo->mScnBlock[aBlockIdx] = SML_INFINITE_SCN;
    
    /**
     * Check invaild rowid
     * - tablespace
     * - datafile
     * - page sequence identifier
     */

    sDatafileId = SMP_DATAFILE_ID( aRowRid->mPageId );
    sPageSeqId = SMP_PAGE_SEQ_ID( aRowRid->mPageId );
    
    STL_TRY_THROW( smfIsValidTbs( aRowRid->mTbsId ) == STL_TRUE,
                   RAMP_ERR_INVALID_ROWID );
    
    STL_TRY_THROW( smfIsValidDatafile( aRowRid->mTbsId, sDatafileId ) == STL_TRUE,
                   RAMP_ERR_INVALID_ROWID );
    
    STL_TRY_THROW( sPageSeqId < smfGetMaxPageCount( aRowRid->mTbsId, sDatafileId ),
                   RAMP_ERR_INVALID_ROWID );
    
    STL_TRY( smpAcquire( NULL,
                         aRowRid->mTbsId,
                         aRowRid->mPageId,
                         KNL_LATCH_MODE_SHARED,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );
    sState = 1;

    /**
     * Check invaild rowid
     * - page type
     * - segment identifier
     * - segment sequence
     */

    STL_TRY_THROW( aRowRid->mOffset >= 0, RAMP_ERR_INVALID_ROWID );

    sPhyHdr = SMP_GET_PHYSICAL_HDR( SMP_FRAME(&sPageHandle) );
    sLogicalHdr = (smdmphLogicalHdr*)smpGetLogicalHeader( &sPageHandle );
    
    STL_TRY_THROW( sPhyHdr->mPageType == SMP_PAGE_TYPE_TABLE_DATA,
                   RAMP_ERR_INVALID_ROWID );
    STL_TRY_THROW( sPhyHdr->mSegmentId == aSegmentId,
                   RAMP_ERR_INVALID_ROWID );
    STL_TRY_THROW( sLogicalHdr->mSegmentSeq == aSegmentSeq,
                   RAMP_ERR_INVALID_ROWID );
    
    /**
     * Check invisible rowid
     * - offset
     * - deleted
     * - master
     */

    if( aRowRid->mOffset >= SMP_GET_PAGE_ROW_ITEM_COUNT(&sPageHandle) )
    {
        *aIsMatched = STL_FALSE;
        STL_THROW( RAMP_SKIP_FETCH );
    }

    if( smpIsNthOffsetSlotFree( &sPageHandle, aRowRid->mOffset ) == STL_TRUE )
    {
        *aIsMatched = STL_FALSE;
        STL_THROW( RAMP_SKIP_FETCH );
    }

    sRow = smpGetNthRowItem( &sPageHandle, aRowRid->mOffset );

    /**
     * slave row piece는 rowid로 fetch할수 없다.
     */
    if( SMDMPH_IS_MASTER_ROW( sRow ) == STL_FALSE )
    {
        *aIsMatched = STL_FALSE;
        STL_THROW( RAMP_SKIP_FETCH );
    }

    /**
     * Get Valid Version
     */
    
    sRowInfo.mPageHandle = &sPageHandle;
    sRowInfo.mTbsId = aRowRid->mTbsId;
    sRowInfo.mRow = sRow;
    sRowInfo.mTransId = SMA_GET_TRANS_ID( aStatement );
    sRowInfo.mViewScn = aFetchRecordInfo->mViewScn;
    sRowInfo.mCommitScn = SMA_GET_COMMIT_SCN( aStatement );
    sRowInfo.mTcn = aFetchRecordInfo->mViewTcn;
    sRowInfo.mColList = aFetchRecordInfo->mColList;
    sRowInfo.mRowIdColList = aFetchRecordInfo->mRowIdColList;
    sRowInfo.mTableLogicalId = aFetchRecordInfo->mTableLogicalId;
    sRowInfo.mBlockIdx = aBlockIdx;
    
    STL_TRY( smdmphGetValidVersion( aRowRid,
                                    &sRowInfo,                                    
                                    aFetchRecordInfo->mPhysicalFilter,
                                    aFetchRecordInfo->mLogicalFilterEvalInfo,
                                    &sCommitScn,
                                    &sRowTcn,
                                    &sWaitTransId,
                                    &sHasValidVersion,
                                    aIsMatched,
                                    &sLatchReleased,
                                    aEnv )
             == STL_SUCCESS );

    STL_RAMP( RAMP_SKIP_FETCH );
    
    sState = 0;
    if( sLatchReleased == STL_FALSE )
    {
        STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
    }

    if( sHasValidVersion == STL_FALSE )
    {
        STL_PARAM_VALIDATE( aDeleted != NULL, KNL_ERROR_STACK(aEnv) );
        *aDeleted = STL_TRUE;
    }
    else
    {
        if( sCommitScn == SML_INFINITE_SCN )
        {
            /**
             * My Transaction인 경우는 infinite가 될수 있으며, 이러한 경우는
             * View SCN을 사용한다.
             * - 해당 View SCN 이후에 변경사항이 발생하면 Version Conflict 에러가 발생한다.
             */
            sCommitScn = SMA_GET_STMT_VIEW_SCN( aStatement );
        }
        else
        {
            sCommitScn = STL_MAX( sCommitScn,
                                  SMA_GET_STMT_VIEW_SCN( aStatement ) );
        }

        aFetchRecordInfo->mScnBlock[aBlockIdx] = sCommitScn;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_ROWID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INVALID_ROWID,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    if( sState == 1 )
    {
        if( sLatchReleased == STL_FALSE )
        {
            (void)smpRelease( &sPageHandle, aEnv );
        }
    }

    return STL_FAILURE;
}


stlStatus smdmphExtractValidKeyValue( void              * aRelationHandle,
                                      smxlTransId         aTransId,
                                      smlScn              aViewScn,
                                      smlTcn              aTcn,
                                      smlRid            * aRowRid,
                                      knlValueBlockList * aColumnList,
                                      smlEnv            * aEnv )
{
    smpHandle    sPageHandle;
    smlScn       sCommitScn;
    smxlTransId  sWaitTransId;
    stlBool      sHasValidVersion = STL_FALSE;
    stlChar    * sRow;
    stlInt32     sState = 0;
    stlBool      sLatchReleased = STL_FALSE;
    smdRowInfo   sRowInfo;
    stlBool      sIsMatched;
    smlTcn       sRowTcn;

    STL_TRY( smpAcquire( NULL,
                         aRowRid->mTbsId,
                         aRowRid->mPageId,
                         KNL_LATCH_MODE_SHARED,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );
    sState = 1;

    sRow = smpGetNthRowItem( &sPageHandle, aRowRid->mOffset );

    sRowInfo.mPageHandle = &sPageHandle;
    sRowInfo.mTbsId = aRowRid->mTbsId;
    sRowInfo.mRow = sRow;
    sRowInfo.mTransId = aTransId;
    sRowInfo.mViewScn = aViewScn;
    sRowInfo.mCommitScn = SML_INFINITE_SCN;
    sRowInfo.mTcn = aTcn;
    sRowInfo.mColList = aColumnList;
    sRowInfo.mRowIdColList = NULL;
    sRowInfo.mTableLogicalId = KNL_ANONYMOUS_TABLE_ID;
    sRowInfo.mBlockIdx = 0;
    
    STL_TRY( smdmphGetValidVersion( aRowRid,
                                    &sRowInfo,
                                    NULL,   /* aPhysicalFilter */
                                    NULL,   /* aLogicalFilterEvalInfo */
                                    &sCommitScn,
                                    &sRowTcn,
                                    &sWaitTransId,
                                    &sHasValidVersion,
                                    &sIsMatched,
                                    &sLatchReleased,
                                    aEnv )
             == STL_SUCCESS );

    sState = 0;
    if( sLatchReleased == STL_FALSE )
    {
        STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
    }

    STL_DASSERT( sHasValidVersion == STL_TRUE );
    
    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( aColumnList, 0, 1 );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        if( sLatchReleased == STL_FALSE )
        {
            (void)smpRelease( &sPageHandle, aEnv );
        }
    }

    return STL_FAILURE;
}


/**
 * @brief 주어진 row rid의 row에 lock을 건다
 * @param[in] aStatement 사용하는 statement
 * @param[in] aRelationHandle lock을 걸 row가 존재하는 relation의 핸들
 * @param[in] aRowRid lock을 걸 row의 row rid
 * @param[in] aRowScn 레코드 검색 당시의 SCN
 * @param[in] aLockMode lock을 걸 mode(SML_LOCK_S/SML_LOCK_X)
 * @param[in] aTimeInterval 락 타임아웃 시간( sec단위 )
 * @param[out] aVersionConflict Lock 시도 도중에 version conflict 상황을 만났는지 여부
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmphLockRow( smlStatement * aStatement,
                         void         * aRelationHandle,
                         smlRid       * aRowRid,
                         smlScn         aRowScn,
                         stlUInt16      aLockMode,
                         stlInt64       aTimeInterval,
                         stlBool      * aVersionConflict,
                         smlEnv       * aEnv )
{
    smlRid          sRowRid = *aRowRid;
    stlUInt32       sAttr = SMXM_ATTR_NOLOG_DIRTY;
    void          * sSegmentHandle = SME_GET_SEGMENT_HANDLE(aRelationHandle);
    stlUInt32       sState = 0;
    smxmTrans       sMiniTrans;
    stlChar       * sSrcRowSlot;
    stlBool         sIsMyTrans = STL_FALSE;
    void          * sLockItem = NULL;
    smxlTransId     sTransId;
    smxlTransId     sWaitTransId;
    smpHandle       sPageHandle;
    smlScn          sCommitScn;
    smlTbsId        sTbsId = smsGetTbsId(sSegmentHandle);

    STL_DASSERT( aRowScn != 0 );
    
    *aVersionConflict = STL_FALSE;
    sTransId = SMA_GET_TRANS_ID( aStatement );
    
    STL_TRY( smxmBegin( &sMiniTrans,
                        sTransId,
                        smsGetSegRid(sSegmentHandle),
                        sAttr,
                        aEnv ) == STL_SUCCESS );
    sState = 1;

    while( 1 )
    {
        /* 주어진 페이지를 얻는다 */
        STL_TRY( smpAcquire( &sMiniTrans,
                             sTbsId,
                             aRowRid->mPageId,
                             KNL_LATCH_MODE_EXCLUSIVE,
                             &sPageHandle,
                             aEnv ) == STL_SUCCESS );

        sSrcRowSlot = smpGetNthRowItem( &sPageHandle, aRowRid->mOffset );

        STL_TRY( smdmphGetCommitScn( sTbsId,
                                     &sPageHandle,
                                     sSrcRowSlot,
                                     &sCommitScn,
                                     &sWaitTransId,
                                     aEnv ) == STL_SUCCESS );

        /**
         * My Transaction은 통과
         */
        if( SMDMPH_IS_SAME_TRANS( sTransId,
                                  SML_INFINITE_SCN, /* DML 중이라 내 커밋SCN은 무한대이다 */
                                  sWaitTransId,
                                  sCommitScn ) == STL_TRUE )
        {
            sState = 0;
            STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

            STL_THROW( RAMP_SKIP_LOCK_BIT );
        }
                
        /**
         * 최종 버전이 Commit 된 레코드임
         */
        if( sCommitScn != SML_INFINITE_SCN )
        {
            /**
             * Lock Bit이 설정되어 있지 않다면 대기할 필요 없다.
             */
            if( SMDMPH_IS_LOCKED( sSrcRowSlot ) == STL_FALSE )
            {
                break;
            }

            /**
             * 이미 Unlock된 경우라면 대기할 필요 없다.
             */
            STL_TRY( smklIsActiveLockRecord( sTransId,
                                             sRowRid,
                                             &sLockItem,
                                             &sIsMyTrans,
                                             aEnv )
                     == STL_SUCCESS );

            if( sLockItem == NULL )
            {
                SMDMPH_UNSET_LOCKED( sSrcRowSlot );
                break;
            }

            if( sIsMyTrans == STL_TRUE )
            {
                break;
            }
        }

        sState = 0;
        STL_TRY( smxmRollback( &sMiniTrans, aEnv ) == STL_SUCCESS );

        /**
         * sWaitTransId가 종료될때까지 기다린다
         */
        if( sLockItem != NULL )
        {
            STL_TRY( smklWaitRecord( sTransId,
                                     SML_LOCK_X,
                                     sRowRid,
                                     aTimeInterval,
                                     aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            STL_TRY( smxlWaitTrans( sTransId,
                                    sWaitTransId,
                                    aTimeInterval,
                                    aEnv )
                     == STL_SUCCESS );
        }
        
        *aVersionConflict = STL_TRUE;
        
        STL_THROW( RAMP_SKIP_LOCK_BIT );
    }

    /**
     * Version Conflict 검사
     */
    if( (SMDMPH_IS_SAME_TRANS( sTransId,
                               SML_INFINITE_SCN, /* DML 중이라 내 커밋SCN은 무한대이다 */
                               sWaitTransId,
                               sCommitScn ) != STL_TRUE) &&
        (sCommitScn > aRowScn) )
    {
        *aVersionConflict = STL_TRUE;
        
        sState = 0;
        STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

        STL_THROW( RAMP_SKIP_LOCK_BIT );
    }

    /**
     * 모든 Lock Row는 { PAGE } -> { LOCK BUCKET } 순으로 이루어진다.
     * - 해당 룰이 위배되는 경우는 Deadlock이 발생할수 있다.
     */

    if( sIsMyTrans == STL_FALSE )
    {
        STL_TRY( smklLockRecord( SMA_GET_TRANS_ID(aStatement),
                                 SML_LOCK_X,
                                 sRowRid,
                                 aTimeInterval,
                                 aEnv ) == STL_SUCCESS );
        
        SMDMPH_SET_LOCKED( sSrcRowSlot );
    }
    
    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    STL_RAMP( RAMP_SKIP_LOCK_BIT );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)smxmCommit( &sMiniTrans, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}


/**
 * @brief 주어진 update log record(redo/undo)를 분석하여 정보를 구해낸다
 * @param[in] aLogRec 분석할 log record. row header가 기록된 위치.
 * @param[out] aMemMark 분석후 컬럼정보를 만드는데 필요한 메모리를 나중에 해제하기위한 이전 메모리 위치
 * @param[out] aRtsSeq log에 기록된 row가 사용하는 RTS의 순번
 * @param[out] aColCount log에 기록된 row의 총 column 개수
 * @param[out] aFirstColOrd  log에 기록된 row piece의 첫 column의 순번
 * @param[out] aColumns log에 기록된 row의 모든 컬럼 정보
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmphAnalyzeUpdateLogRec( stlChar            * aLogRec,
                                     knlRegionMark      * aMemMark,
                                     stlUInt8           * aRtsSeq,
                                     stlInt16           * aColCount,
                                     stlInt32           * aFirstColOrd,
                                     knlValueBlockList ** aColumns,
                                     smlEnv             * aEnv )
{
    knlValueBlockList * sFirstCol = NULL;
    knlValueBlockList * sCurCol;
    knlValueBlockList * sPrevCol;
    stlChar           * sLogPtr = aLogRec;
    stlInt32            sColOrd;
    stlUInt8            sColIdx;
    stlInt16            i;
    stlUInt8            sColLenSize;
    stlInt64            sColLen;
    stlChar           * sBuf;
    stlUInt16           sSlotHdrSize;
    stlBool             sIsZero;
    stlInt32            sState = 0;

    SMDMPH_GET_ROW_HDR_SIZE( sLogPtr, &sSlotHdrSize );
    sLogPtr += sSlotHdrSize;
    STL_WRITE_INT32( aFirstColOrd, sLogPtr );
    sLogPtr += STL_SIZEOF(stlInt32);
    STL_WRITE_INT16( aColCount, sLogPtr );
    sLogPtr += STL_SIZEOF(stlInt16);

    /* update column 정보를 세팅한다 */
    KNL_INIT_REGION_MARK(aMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               aMemMark,
                               (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;
  
    sPrevCol = NULL;
    for( i = 0; i < (*aColCount); i++ )
    {
        STL_WRITE_INT8( &sColIdx, sLogPtr );
        sColOrd = sColIdx + *aFirstColOrd;
        sLogPtr += STL_SIZEOF(stlInt8);
        SMP_GET_COLUMN_LEN(sLogPtr, &sColLenSize, &sColLen, sIsZero);
        sLogPtr += sColLenSize;

        STL_TRY( knlInitBlockNoBuffer( &sCurCol,
                                       STL_LAYER_STORAGE_MANAGER,
                                       1, /* aBlockCnt */
                                       DTL_TYPE_NUMBER, /* aDBType, ZERO SENSITIVE에서 사용 */
                                       &aEnv->mOperationHeapMem,
                                       (knlEnv*)aEnv ) == STL_SUCCESS );
        
        if( sIsZero == STL_FALSE )
        {
            if( sColLen > 0 )
            {
                STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                            sColLen,
                                            (void **) & sBuf,
                                            (knlEnv*)aEnv )
                         == STL_SUCCESS );
            
                KNL_SET_BLOCK_DATA_PTR( sCurCol, 0, sBuf, sColLen );
                stlMemcpy( sBuf, sLogPtr, sColLen );
                sLogPtr += sColLen;
            }
        }
        else
        {
            sColLen = 1;
            STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                        sColLen,
                                        (void **) & sBuf,
                                        (knlEnv*)aEnv )
                     == STL_SUCCESS );
            
            KNL_SET_BLOCK_DATA_PTR( sCurCol, 0, sBuf, sColLen );
            DTL_NUMERIC_DATA_SET_ZERO( sBuf );
        }

        KNL_SET_BLOCK_DATA_LENGTH( sCurCol, 0, sColLen );
        KNL_SET_BLOCK_COLUMN_ORDER( sCurCol, sColOrd );
        
        if( sFirstCol == NULL )
        {
            sFirstCol = sCurCol;
        }
        if( sPrevCol != NULL )
        {
            KNL_LINK_BLOCK_LIST( sPrevCol, sCurCol );
        }
        sPrevCol = sCurCol;
    }

    if( SMDMPH_IS_RTS_BOUND( aLogRec ) != STL_TRUE )
    {
        *aRtsSeq = SMP_RTS_SEQ_NULL;
    }
    else
    {
        SMDMPH_GET_RTSSEQ( aLogRec, aRtsSeq );
    }

    *aColumns = sFirstCol;

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                        aMemMark,
                                        STL_TRUE, /* aFreeChunk */
                                        (knlEnv*)aEnv );
    }

    return STL_FAILURE;
}


/**
 * @brief 주어진 row를 갱신할 경우에 늘어나는 row piece의 길이 차이를 구하고, undo log record를 기록한다
 * @param[in] aFirstColPtr 분석할 row piece의 첫 컬럼이 있는 위치.
 * @param[in] aTotalColCount 갱신할 row piece의 총 컬럼 개수
 * @param[in] aFirstColOrd 갱신할 row piece의 첫 컬럼의 order
 * @param[in] aUpdateCols 갱신할 컬럼들의 정보
 * @param[in] aBeforeCols 갱신 이전 컬럼들의 정보를 기록할 버퍼
 * @param[in] aBlockIdx 갱신할 컬럼들 block Idx
 * @param[out] aUndoLogPtr 조사를 하면서 undo log를 기록할 버퍼의 시작 주소
 * @param[out] aUndoLogEnd 기록한 undo log의 끝 주소
 * @param[out] aRowBodySize 기존 row piece의 컬럼기록 부분들의 총 길이
 * @param[out] aIsColSizeDiff 갱신에의해 길이가 변경된 컬럼이 있는지 여부
 * @param[out] aRowSizeDiff 갱신된 row piece와 이전 row piece의 길이 차이
 * @param[out] aUpdateColCount 갱신될 컬럼의 개수
 */
stlStatus smdmphGetUpdatePieceSize( stlChar            * aFirstColPtr,
                                    stlInt16             aTotalColCount,
                                    stlInt32             aFirstColOrd,
                                    knlValueBlockList  * aUpdateCols,
                                    knlValueBlockList  * aBeforeCols,
                                    stlInt32             aBlockIdx,
                                    stlChar            * aUndoLogPtr,
                                    stlChar           ** aUndoLogEnd,
                                    stlInt16           * aRowBodySize,
                                    stlBool            * aIsColSizeDiff,
                                    stlInt64           * aRowSizeDiff,
                                    stlInt16           * aUpdateColCount )
{
    stlUInt8            i;
    stlUInt8            sColLenSize;
    stlUInt8            sNewColLenSize;
    stlInt64            sTotalColSize;
    stlInt64            sColLen;
    stlChar           * sColPtr = aFirstColPtr;
    knlValueBlockList * sCurCol = aUpdateCols;
    knlValueBlockList * sBeforeCol = aBeforeCols;
    stlChar           * sLogPtr = aUndoLogPtr;
    stlInt64            sDataLen;
    dtlDataValue      * sDataValue;
    dtlDataValue      * sBeforeValue;
    stlBool             sIsZero;

    STL_DASSERT( aTotalColCount <= SMD_MAX_COLUMN_COUNT_IN_PIECE );
    
    *aIsColSizeDiff = STL_FALSE;
    *aRowSizeDiff = 0;
    *aUpdateColCount = 0;

    for( i = 0; i < aTotalColCount; i++ )
    {
        SMP_GET_COLUMN_LEN( sColPtr, &sColLenSize, &sColLen, sIsZero );
        sTotalColSize = sColLenSize + sColLen;
        
        if( sCurCol != NULL )
        {
            if( i + aFirstColOrd == KNL_GET_BLOCK_COLUMN_ORDER(sCurCol) )
            {
                sDataValue = KNL_GET_BLOCK_DATA_VALUE( sCurCol, aBlockIdx );
                sDataLen = sDataValue->mLength;

                if( DTL_NUMERIC_IS_ZERO(sDataValue->mValue, sDataLen) )
                {
                    if( dtlDataTypeGroup[sDataValue->mType] == DTL_GROUP_NUMBER )
                    {
                        sDataLen = 0;
                    }
                }
                
                SMP_GET_COLUMN_LEN_SIZE( sDataLen, &sNewColLenSize );
                
                if( (sDataLen + sNewColLenSize) != sTotalColSize )
                {
                    *aIsColSizeDiff = STL_TRUE;
                    *aRowSizeDiff += ((sDataLen + sNewColLenSize) - sTotalColSize);
                }
                if( aUndoLogPtr != NULL )
                {
                    /* Undo log에 컬럼 정보 기록( column seq, length, value ) */
                    STL_WRITE_INT8( sLogPtr, &i );
                    sLogPtr += STL_SIZEOF(stlInt8);
                    stlMemcpy( sLogPtr, sColPtr, sTotalColSize );
                    sLogPtr += sTotalColSize;
                }

                sCurCol = sCurCol->mNext;

                if( sBeforeCol != NULL )
                {
                    sBeforeValue = KNL_GET_BLOCK_DATA_VALUE( sBeforeCol, aBlockIdx );

                    if( sIsZero == STL_TRUE )
                    {
                        DTL_NUMERIC_SET_ZERO( sBeforeValue->mValue,
                                              sBeforeValue->mLength );
                    }
                    else
                    {
                        DTL_COPY_VALUE( sBeforeValue, sColPtr + sColLenSize, sColLen );
                        sBeforeValue->mLength = sColLen;
                    }
                    
                    sBeforeCol = sBeforeCol->mNext;
                }
                
                (*aUpdateColCount)++;
            }
        }
        sColPtr += sTotalColSize;
    }
    
    *aRowBodySize = sColPtr - aFirstColPtr;
    if( aUndoLogPtr != NULL )
    {
        *aUndoLogEnd = sLogPtr;
    }

    return STL_SUCCESS;
}


/**
 * @brief 주어진 row가 key column과 같은 값들을 갖는지 비교한다
 * @param[in] aRowRid 비교할 row의 위치
 * @param[in,out] aPageHandle Latch를 잡은 Page Handle
 * @param[in,out] aPageLatchRelease aPageHandle의 Latch 해제 여부
 * @param[in] aMyTransId 찾으려고 하는 version을 읽는 Transaction의 ID
 * @param[in] aMyViewScn 찾으려고 하는 version을 읽기위한 view Scn
 * @param[in] aMyCommitScn 찾으려고 하는 version을 읽기위한 view Scn
 * @param[in] aMyTcn 찾는 statement의 Tcn
 * @param[in] aKeyCompList 비교할 key 정보와 table column 버퍼등을 담은 구조체
 * @param[out] aIsMatch 찾은 버전과 key값이 동일한지 여부
 * @param[out] aCommitScn 찾은 버전의 Commit Scn
 * @param[in,out] aEnv storage manager environment
 */
stlStatus smdmphCompareKeyValue( smlRid              * aRowRid,
                                 smpHandle           * aPageHandle,
                                 stlBool             * aPageLatchRelease,
                                 smxlTransId           aMyTransId,
                                 smlScn                aMyViewScn,
                                 smlScn                aMyCommitScn,
                                 smlTcn                aMyTcn,
                                 knlKeyCompareList   * aKeyCompList,
                                 stlBool             * aIsMatch,
                                 smlScn              * aCommitScn,
                                 smlEnv              * aEnv )

{
    smxlTransId         sWaitTransId;
    stlChar           * sCurRow;
    smdRowInfo          sRowInfo;

    *aIsMatch = STL_FALSE;

    STL_DASSERT( *aPageLatchRelease == STL_FALSE );

    /**
     * 기존 row가 재사용되었는지 판단하다.
     * - Offset이 Row Item Count 범위를 넘어선 경우
     * - Offset에 해당되는 Slot이 Free 상태인 경우
     * - 기존 row가 Slave로 변경된 경우
     */
    if( SMP_GET_PAGE_ROW_ITEM_COUNT(aPageHandle) > aRowRid->mOffset )
    {
        if( smpIsNthOffsetSlotFree( aPageHandle, aRowRid->mOffset ) == STL_FALSE )
        {
            sCurRow = smpGetNthRowItem( aPageHandle, aRowRid->mOffset );
            if( SMDMPH_IS_MASTER_ROW( sCurRow ) == STL_TRUE )
            {
                *aIsMatch = STL_TRUE;
                sRowInfo.mPageHandle = aPageHandle;
                sRowInfo.mTbsId = aRowRid->mTbsId;
                sRowInfo.mRow = sCurRow;
                sRowInfo.mTransId = aMyTransId;
                sRowInfo.mViewScn = aMyViewScn;
                sRowInfo.mCommitScn = aMyCommitScn;
                sRowInfo.mTcn = aMyTcn;
                sRowInfo.mColList = NULL;
                sRowInfo.mRowIdColList = NULL;
                sRowInfo.mBlockIdx = 0;
                sRowInfo.mKeyCompList = aKeyCompList;
                
                STL_TRY( smdmphGetValidVersionAndCompare( &sRowInfo,
                                                          aCommitScn,
                                                          &sWaitTransId,
                                                          aIsMatch,
                                                          aPageLatchRelease,
                                                          aEnv ) == STL_SUCCESS );
            }
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 주어진 row로부터 Key를 추출한다.
 * @param[in] aPageHandle      Row가 저장되어 있는 Page Handle
 * @param[in] aFetchFirst      Fetch First 여부
 * @param[in] aRowIterator     Row Iterator pointer
 * @param[in] aColumnList      추출할 column들의 정보
 * @param[out] aKeyValueSize   총 Key의 바이트 단위 크기 배열
 * @param[out] aRowSeq         Row의 Sequence number
 * @param[out] aHasNullInBlock Value Block내에 Null Value가 존재하는지 여부
 * @param[out] aIsSuccess     성공여부
 * @param[in,out] aEnv storage manager environment
 * @remarks DML과 동시에 발생할수 없기 때문에 Latch를 획득하지 않는다.
 */
stlStatus smdmphExtractKeyValue( smpHandle          * aPageHandle,
                                 stlBool              aFetchFirst,
                                 smpRowItemIterator * aRowIterator,
                                 smdValueBlockList  * aColumnList,
                                 stlInt32           * aKeyValueSize,
                                 stlInt16           * aRowSeq,
                                 stlBool            * aHasNullInBlock,
                                 stlBool            * aIsSuccess,
                                 smlEnv             * aEnv )

{
    stlUInt8            sColLenSize;
    stlInt64            sColLen;
    smdValueBlockList * sCurCol;
    stlInt32            i;
    stlUInt16           sSlotHdrSize;
    stlChar           * sRow;
    smlTbsId            sTbsId = smpGetTbsId( aPageHandle );
    stlInt32            sStartColOrd;
    stlChar           * sCurPtr;
    smpHandle           sPageHandle;
    stlInt32            sState = 0;
    stlInt16            sColCount;
    smlRid              sNextRid;
    stlInt32            sKeyValueSize;
    stlInt32            sBlockIdx = 0;
    smdValueBlock     * sValueBlock;
    stlBool             sIsZero;

    /**
     * Value Block List는 초기화 된 상태임을 보장해야 한다.
     */
    STL_DASSERT( aColumnList->mValueBlock->mUsedBlockCnt == 0 );
    STL_DASSERT( aColumnList->mValueBlock->mSkipBlockCnt == 0 );
    
    *aIsSuccess = STL_FALSE;

    if( aFetchFirst == STL_TRUE )
    {
        SMP_INIT_ROWITEM_ITERATOR( SMP_FRAME(aPageHandle), aRowIterator );
    }

    SMP_FOREACH_ROWITEM_ITERATOR( aRowIterator )
    {
        if( SMP_IS_FREE_ROWITEM( aRowIterator ) == STL_TRUE )
        {
            continue;
        }
        
        sRow = SMP_GET_CURRENT_ROWITEM( aRowIterator );

        if( (SMDMPH_IS_MASTER_ROW( sRow ) == STL_FALSE) ||
            (SMDMPH_IS_DELETED( sRow ) == STL_TRUE) )
        {
            continue;
        }
        
        SMDMPH_GET_ROW_HDR_SIZE( sRow, &sSlotHdrSize );
        sCurPtr = sRow + sSlotHdrSize;
        sCurCol = aColumnList;

        sKeyValueSize = 0;
        sStartColOrd = 0;
        
        while( STL_TRUE )
        {
            SMDMPH_GET_COLUMN_CNT( sRow, &sColCount );

            /**
             * 읽어야할 컬럼이 범위내에 없다면 next piece로 이동한다.
             */
            STL_TRY_THROW( sStartColOrd + sColCount > sCurCol->mColumnOrder, RAMP_SKIP );
            
            for( i = 0; i < sColCount; i++ )
            {
                if( sCurCol == NULL )
                {
                    break;
                }
            
                SMP_GET_COLUMN_LEN( sCurPtr, &sColLenSize, &sColLen, sIsZero );

                if( (i + sStartColOrd) == sCurCol->mColumnOrder )
                {
                    if( (sIsZero == STL_FALSE) && (sColLen == 0) )
                    {
                        *aHasNullInBlock = STL_TRUE;
                    }

                    sValueBlock = sCurCol->mValueBlock;

                    /**
                     * Continuous column은 key size 제한으로 인하여 index key로 사용될수 없다.
                     */
                    
                    STL_TRY_THROW( SMDMPH_IS_CONT_COL( sRow ) == STL_FALSE, RAMP_ERR_TOO_LARGE_KEY );
                    
                    /**
                     * Chained Row Piece는 Key Value를 복사해야 Unfix가 가능하다.
                     */

                    STL_DASSERT( sValueBlock->mKeyValue[sBlockIdx].mBufferSize >= sColLen );
                    
                    if( sState == 1 )
                    {
                        if( sIsZero == STL_FALSE )
                        {
                            stlMemcpy( sValueBlock->mKeyValue[sBlockIdx].mValue,
                                       sCurPtr + sColLenSize,
                                       sColLen );
                            sValueBlock->mColumnInRow[sBlockIdx].mLength = sColLen;
                            sKeyValueSize += (sColLen + sColLenSize);
                        }
                        else
                        {
                            DTL_NUMERIC_SET_ZERO( sValueBlock->mKeyValue[sBlockIdx].mValue,
                                                  sValueBlock->mColumnInRow[sBlockIdx].mLength );
                            sKeyValueSize += 2;
                        }
                        
                        sValueBlock->mColumnInRow[sBlockIdx].mValue = sValueBlock->mKeyValue[sBlockIdx].mValue;
                    }
                    else
                    {
                        if( sIsZero == STL_TRUE )
                        {
                            DTL_NUMERIC_SET_ZERO( sValueBlock->mKeyValue[sBlockIdx].mValue,
                                                  sValueBlock->mColumnInRow[sBlockIdx].mLength );
                            sValueBlock->mColumnInRow[sBlockIdx].mValue = sValueBlock->mKeyValue[sBlockIdx].mValue;
                            sKeyValueSize += 2;
                        }
                        else
                        {
                            sValueBlock->mColumnInRow[sBlockIdx].mValue  = sCurPtr + sColLenSize;
                            sValueBlock->mColumnInRow[sBlockIdx].mLength = sColLen;
                            sKeyValueSize += (sColLen + sColLenSize);
                        }
                    }

                    sValueBlock->mUsedBlockCnt++;
                    sCurCol = sCurCol->mNext;
                }
                else
                {
                    SMP_GET_COLUMN_LEN( sCurPtr, &sColLenSize, &sColLen, sIsZero );
                }
            
                sCurPtr += sColLenSize + sColLen;
            }

            STL_RAMP( RAMP_SKIP );
        
            if( (SMDMPH_HAS_NEXT_LINK( sRow ) == STL_TRUE) && (sCurCol != NULL) )
            {
                if( SMDMPH_IS_CONT_COL( sRow ) == STL_FALSE )
                {
                    sStartColOrd += sColCount;
                }

                SMDMPH_GET_LINK_PID( sRow, &sNextRid.mPageId );
                SMDMPH_GET_LINK_OFFSET( sRow, &sNextRid.mOffset );
            }
            else
            {
                /**
                 * 더 이상 row piece를 fetch할 필요가 없다
                 */

                while( sCurCol != NULL )
                {
                    /**
                     * Trailing null
                     */
                    
                    sValueBlock = sCurCol->mValueBlock;
                    sValueBlock->mUsedBlockCnt++;
                    sValueBlock->mColumnInRow[sBlockIdx].mLength = 0;
                    sKeyValueSize += 1;
                    
                    sCurCol = sCurCol->mNext;
                    *aHasNullInBlock = STL_TRUE;
                }
                
                break;
            }

            if( sState == 1  )
            {
                sState = 0;
                STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
            }
        
            STL_TRY( smpFix( sTbsId,
                             sNextRid.mPageId,
                             &sPageHandle,
                             aEnv ) == STL_SUCCESS );
            sState = 1;
        
            sRow = smpGetNthRowItem( &sPageHandle, sNextRid.mOffset);
            SMDMPH_GET_ROW_HDR_SIZE( sRow, &sSlotHdrSize );
        
            sCurPtr = sRow + sSlotHdrSize;
        }

        if( sState == 1  )
        {
            sState = 0;
            STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
        }
        
        aKeyValueSize[sBlockIdx] = sKeyValueSize;
        aRowSeq[sBlockIdx] = SMP_GET_CURRENT_ROWITEM_SEQ( aRowIterator );

        sBlockIdx++;

        if( sBlockIdx >= aColumnList->mValueBlock->mAllocBlockCnt )
        {
            break;
        }
    }

    if( sBlockIdx > 0 )
    {
        *aIsSuccess = STL_TRUE;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_TOO_LARGE_KEY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_EXCEEDED_MAX_KEY_SIZE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      SMN_MAX_KEY_BODY_SIZE );
    }
 
    STL_FINISH;

    if( sState == 1  )
    {
        (void)smpUnfix( &sPageHandle, aEnv );
    }
        
    return STL_FAILURE;
}


/**
 * @brief 페이지의 Freeness(자유도) 상태를 변경한다.
 * @param[in] aMiniTrans Mini-transaction Pointer
 * @param[in] aRelationHandle Target Relation Handle
 * @param[in] aSegmentHandle Target Segment Handle
 * @param[in] aPageHandle 대상 페이지의 Page Handle
 * @param[in] aFreeness 변경할 Freeness
 * @param[in] aScn Freeness의 유효성 기간
 * @param[in,out] aEnv Environment 정보
 * @remark aScn은 aFreeness가 SMP_FREENESS_FREE일때만 유효하다.
 * @note 대상 페이지는 Fix(or Acquire)가 되어 있는 상태여야 한다.
 */
stlStatus smdmphUpdatePageStatus( smxmTrans   * aMiniTrans,
                                  void        * aRelationHandle,
                                  void        * aSegmentHandle,
                                  smpHandle   * aPageHandle,
                                  smpFreeness   aFreeness,
                                  smlScn        aScn,
                                  smlEnv      * aEnv )
{
    smeHint * sRelHint;
    
    STL_TRY( smsUpdatePageStatus( aMiniTrans,
                                  aSegmentHandle,
                                  aPageHandle,
                                  aFreeness,
                                  aScn,
                                  aEnv ) == STL_SUCCESS );
    
    if( (aFreeness == SMP_FREENESS_UPDATEONLY) ||
        (aFreeness == SMP_FREENESS_FREE) )
    {
        sRelHint = smeFindRelHint( aRelationHandle,
                                   smsGetValidScn(aSegmentHandle),
                                   aEnv );

        if( sRelHint != NULL )
        {
            if( sRelHint->mHintPid == smpGetPageId( aPageHandle ) )
            {
                sRelHint->mHintPid = SMP_NULL_PID;
            }
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmphIsAgable( smpHandle * aPageHandle,
                          stlBool   * aIsAgable,
                          smlEnv    * aEnv )
{
    smpRowItemIterator   sIterator;
    stlChar            * sSrcRow;
    smpRts             * sSrcRts;
    stlUInt8             sRtsSeq;
    smlScn               sCommitScn;
    smlScn               sScn;
    smxlTransId          sTransId = SML_INVALID_TRANSID;
    smlScn               sAgableScn;

    *aIsAgable = STL_TRUE;
    
    if( SMP_GET_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) ) > 0 )
    {
        *aIsAgable = STL_FALSE;
        STL_THROW( RAMP_FINISH );
    }
    
    sAgableScn = smxlGetAgableTbsScn( smpGetTbsId(aPageHandle), aEnv );
    
    SMP_INIT_ROWITEM_ITERATOR( SMP_FRAME(aPageHandle), &sIterator );
    
    SMP_FOREACH_ROWITEM_ITERATOR( &sIterator )
    {
        if( SMP_IS_FREE_ROWITEM( &sIterator ) == STL_TRUE )
        {
            continue;
        }
        
        sSrcRow = SMP_GET_CURRENT_ROWITEM( &sIterator );

        if( SMDMPH_IS_RTS_BOUND( sSrcRow ) == STL_TRUE )
        {
            SMDMPH_GET_RTSSEQ( sSrcRow, &sRtsSeq );
            sSrcRts = SMP_GET_NTH_RTS(SMP_FRAME(aPageHandle), sRtsSeq);
            sTransId = sSrcRts->mTransId;
            sScn = sSrcRts->mScn;
        }
        else
        {
            sRtsSeq = SMP_RTS_SEQ_NULL;
            sSrcRts = NULL;
            SMDMPH_GET_TRANS_ID( sSrcRow, &sTransId );
            SMDMPH_GET_SCN( sSrcRow, &sScn );
        }

        if( SML_IS_VIEW_SCN( sScn ) == STL_TRUE )
        {
            STL_TRY( smxlGetCommitScn( sTransId,
                                       SML_MAKE_REAL_SCN(sScn),
                                       &sCommitScn,
                                       aEnv ) == STL_SUCCESS );
        }
        else
        {
            sCommitScn = sScn;
        }

        if( sCommitScn >= sAgableScn )
        {
            *aIsAgable = STL_FALSE;
            STL_THROW( RAMP_FINISH );
        }
    }
    
    STL_RAMP( RAMP_FINISH );
    
    return STL_SUCCESS;

    STL_FINISH;

    stlPopError( KNL_ERROR_STACK(aEnv) );

    return STL_SUCCESS;
}


/**
 * @brief 주어진 Slot의 Slot Body 포인터를 얻는다.
 * @param[in] aSlot Slot의 첫번째 포인터
 * @return Slot의 Body 포인터
 */
inline void * smdmphGetSlotBody( void * aSlot )
{
    stlUInt16 sSlotHdrSize;

    SMDMPH_GET_ROW_HDR_SIZE( aSlot, &sSlotHdrSize );
    return (((stlChar*)aSlot) + sSlotHdrSize);
}

stlInt16 smdmphCountRtsRef( stlChar  * aPageFrame,
                            stlChar  * aRow,
                            stlUInt8   aRtsSeq,
                            smlEnv   * aEnv )
{
    stlUInt8   sRtsSeq;
    stlInt16   sStampingCount = 0;

    if( SMDMPH_IS_RTS_BOUND( aRow ) == STL_TRUE )
    {
        SMDMPH_GET_RTSSEQ(aRow, &sRtsSeq);
        if( aRtsSeq == sRtsSeq )
        {
            sStampingCount++;
        }
    }
    return sStampingCount;
}

stlBool smdmphIsSimplePage( smpHandle * aPageHandle )
{
    smdmphLogicalHdr * sLogicalHdr;
    
    sLogicalHdr = SMDMPH_GET_LOGICAL_HEADER( aPageHandle );
    
    return sLogicalHdr->mSimplePage;
}

stlInt16 smdmphGetMaxFreeSize( void * aRelationHandle )
{
    stlInt16 sMaxSize = 0;
    
    sMaxSize = smpGetPageMaxFreeSize( STL_SIZEOF(smdmphLogicalHdr),
                                      SMD_GET_INITRANS(aRelationHandle) );

    sMaxSize -= SMDMPH_MAX_PADDING_SIZE;

    return sMaxSize;
}

void smdmphValidatePadSpace( smpHandle * aPageHandle )
{
#ifdef STL_VALIDATE_DATABASE
    stlChar            * sTargetRow;
    stlChar            * sCurRow;
    smpRowItemIterator   sTargetIterator;
    smpRowItemIterator   sCurIterator;
    stlInt16             sTotalRowSize;
    stlInt16             sPadSpace;

    SMP_INIT_ROWITEM_ITERATOR( SMP_FRAME(aPageHandle), &sTargetIterator );

    SMP_FOREACH_ROWITEM_ITERATOR( &sTargetIterator )
    {
        if( SMP_IS_FREE_ROWITEM( &sTargetIterator ) == STL_TRUE )
        {
            continue;
        }
        
        sTargetRow = SMP_GET_CURRENT_ROWITEM( &sTargetIterator );
            
        sTotalRowSize = smdmphGetRowSize( sTargetRow );
        SMDMPH_GET_PAD_SPACE( sTargetRow, &sPadSpace );
        STL_ASSERT( (sPadSpace >= 0) &&
                    (sTargetRow + sTotalRowSize + sPadSpace
                     <= (stlChar*)SMP_FRAME(aPageHandle) + SMP_PAGE_SIZE - STL_SIZEOF(smpPhyTail)) );

        SMP_INIT_ROWITEM_ITERATOR( SMP_FRAME(aPageHandle), &sCurIterator );
        
        SMP_FOREACH_ROWITEM_ITERATOR( &sCurIterator )
        {
            if( SMP_IS_FREE_ROWITEM( &sCurIterator ) == STL_TRUE )
            {
                continue;
            }
        
            sCurRow = SMP_GET_CURRENT_ROWITEM( &sCurIterator );
            
            if( sCurRow != sTargetRow )
            {
                STL_ASSERT( (sCurRow < sTargetRow) ||
                            (sCurRow >= sTargetRow + sTotalRowSize + sPadSpace) );
            }
        }
    }
#endif
}

void smdmphValidateTablePage( smpHandle * aPageHandle, smlEnv * aEnv )
{
#ifdef STL_VALIDATE_DATABASE
    smdmphValidatePadSpace( aPageHandle );
    smpCheckRtsRefCount( SMP_FRAME( aPageHandle ), smdmphCountRtsRef, aEnv );
#endif
}

/** @} */
