/*******************************************************************************
 * smdmpfDml.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smdmpfDml.c 13869 2014-11-06 09:27:41Z mkkim $
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
#include <smdmpfRowDef.h>
#include <smdmpfDef.h>
#include <smdDef.h>
#include <smdmpf.h>

/**
 * @file smdmpfDml.c
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
 * @brief 주어진 row의 commit scn을 구한다. commit 되지 않은경우 SML_INFINITE_SCN를 반환한다
 * @param[in] aTbsId Tablespace Identifier
 * @param[in] aPageHandle 조사할 row가 담긴 페이지의 핸들
 * @param[in] aRow 조사할 row의 시작 위치
 * @param[out] aCommitScn 구해진 Commit Scn
 * @param[out] aTransId aRow가 commit 안된 경우 변경한 transaction의 id
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmpfGetCommitScn( smlTbsId      aTbsId,
                              smpHandle   * aPageHandle,
                              stlChar     * aRow,
                              smlScn      * aCommitScn,
                              smxlTransId * aTransId,
                              smlEnv      * aEnv )
{
    smlScn       sScn;
    smxlTransId  sWaitTransId = SML_INVALID_TRANSID;
    stlBool      sIsSuccess = STL_FALSE;

    SMDMPF_GET_SCN( aRow, &sScn );
    if( SML_IS_VIEW_SCN( sScn ) == STL_TRUE )
    {
        SMDMPF_GET_TRANS_ID( aRow, &sWaitTransId );
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
                    SMDMPF_SET_SCN( aRow, &sScn );

                    STL_TRY( smpSetDirty( aPageHandle, aEnv ) == STL_SUCCESS );
                }
            }
            else
            {
                SMDMPF_SET_SCN( aRow, &sScn );
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
 * @brief 주어진 row slot의 header에 정보를 세팅한다
 * @param[in,out] aSlot 정보를 세팅할 row의 시작위치
 * @param[in] aTcn 현재 statement의 Tcn
 * @param[in] aTransId 현재 statement의 Transaction Id
 * @param[in] aColumnCount row의 총 컬럼 개수
 */
inline stlStatus smdmpfWriteRowHeader( stlChar      * aSlot,
                                       smlTcn         aTcn,
                                       smxlTransId    aTransId,
                                       stlUInt16      aColumnCount )
{
    smlPid   sInvalidPid = SMP_NULL_PID;
    stlInt16 sOffset     = 0;
    smlScn   sScn        = SML_INFINITE_SCN;

    STL_DASSERT( aColumnCount <= SMD_MAX_COLUMN_COUNT_IN_PIECE );
    
    SMDMPF_INIT_ROW_HDR_BITMAP( aSlot );
    SMDMPF_SET_MASTER_ROW( aSlot );
    SMDMPF_UNSET_HAS_NEXT_LINK( aSlot );
    SMDMPF_UNSET_DELETED( aSlot );
    SMDMPF_SET_TCN( aSlot, &aTcn );
    SMDMPF_SET_IN_USE( aSlot );
    SMDMPF_UNSET_RTS_BOUND( aSlot );
    SMDMPF_SET_SCN( aSlot, &sScn );

    SMDMPF_SET_TRANS_ID( aSlot, &aTransId );
    SMDMPF_SET_ROLLBACK_PID( aSlot, &sInvalidPid );
    SMDMPF_SET_ROLLBACK_OFFSET( aSlot, &sOffset );
    SMDMPF_SET_COLUMN_CNT( aSlot, &aColumnCount );

    return STL_SUCCESS;
}



/**
 * @brief 새 페이지에 row piece 한 개를 삽입한다
 * @param[in] aRelationHandle row를 삽입할 Relation의 handle
 * @param[in] aValueIdx 삽입할 row의 value block내에서의 순번
 * @param[in] aTransId transaction id
 * @param[in] aViewScn transaction의 view scn
 * @param[in] aTcn 현재 statement의 Tcn
 * @param[in] aInsertCols 삽입할 row의 value block
 * @param[in] aFirstColOrd 삽입할 row piece중 첫 컬럼의 순번
 * @param[in] aIsMasterRow Master Row 여부
 * @param[in] aIsForUpdate Update 중 delete/insert에 의한 호출인지 여부
 * @param[in] aContColPid Row piece에 포함된 continuous column들의 첫번째 page pid
 * @param[in] aContColCount Row piece에 포함된 continuous column들의 수
 * @param[in,out] aPrevChainedPageId Row를 저장하기 위해 여러 page를 사용하는 경우 prev chained page Id
 * @param[out] aRowRid 삽입한 row의 위치
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmpfInsertRowPiece( void               * aRelationHandle,
                                stlInt32             aValueIdx,
                                smxlTransId          aTransId,
                                smlScn               aViewScn,
                                smlTcn               aTcn,
                                knlValueBlockList ** aInsertCols,
                                stlInt32             aFirstColOrd,
                                stlBool              aIsMasterRow,
                                stlBool              aIsForUpdate,
                                smlPid             * aContColPid,
                                stlInt16             aContColCount,
                                smlPid             * aPrevChainedPageId,
                                smlRid             * aRowRid,
                                smlEnv             * aEnv )
{
    dtlDataValue       * sDataValue;
    smlRid               sSegRid;
    stlInt32             sState = 0;
    smxmTrans            sMiniTrans;
    void               * sSegmentHandle = SME_GET_SEGMENT_HANDLE(aRelationHandle);
    stlUInt32            sAttr = SMXM_ATTR_REDO;
    smpHandle            sPageHandle;
    stlInt16             sSlotSeq;
    stlInt16             i;
    stlInt16             sContColPidSeq = 0;
    smlRid               sUndoRid;
    stlChar              sRowBuf[SMP_PAGE_SIZE];
    stlChar            * sRowPtr;
    stlChar              sLogBuf[SMP_PAGE_SIZE];
    stlChar            * sLogPtr;
    stlChar            * sSlotArray[SMD_MAX_COLUMN_COUNT_IN_PIECE + 1];
    stlUInt8             sColLenSize;
    stlUInt8             sLogicalPageType;
    stlInt64             sColLen;
    stlInt16             sThreshold = SMD_GET_CONT_COLUMN_THRESHOLD( aRelationHandle );
    stlBool              sIsZero;
    stlInt16             sSlotIdx;
    stlUInt16            sColCount;
    smlScn               sScn;
    smdmpfLogicalHdr   * sLogicalHdr;
    stlInt16             sLogType = ( (aIsForUpdate == STL_TRUE) ?
                                      SMR_LOG_MEMORY_HEAP_INSERT_FOR_UPDATE :
                                      SMR_LOG_MEMORY_HEAP_INSERT );

    sSegRid = smsGetSegRid( sSegmentHandle );

    STL_TRY( smxmBegin( &sMiniTrans,
                        aTransId,
                        sSegRid,
                        sAttr,
                        aEnv )
             == STL_SUCCESS );
    sState = 1;

    /* Insertable anchor page를 구하거나,
     * anchor가 아니면 logical page type를 체크하지 않아도 됨 */
    if( *aPrevChainedPageId == SMP_NULL_PID )
    {
        sLogicalPageType = SMDMPF_MEMBER_PAGE_ANCHOR;
    }
    else
    {
        sLogicalPageType = SMDMPF_MEMBER_PAGE_TYPE_NONE;
    }

    /**
     * Continuous column은 new page 할당해서 저장한 후 pid를 return 한다.
     */
    STL_TRY( smdmpfGetInsertablePage( aRelationHandle,
                                      &sMiniTrans,
                                      aTransId,
                                      aViewScn,
                                      *aPrevChainedPageId,
                                      sLogicalPageType,
                                      &sPageHandle,
                                      &sSlotSeq,
                                      aEnv )
             == STL_SUCCESS );

    *aPrevChainedPageId = SMDMPF_GET_PREV_CHAINED_PAGE_ID(SMP_FRAME(&sPageHandle));

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

    STL_TRY( smpAllocFixedSlotArray( &sPageHandle,
                                     SMD_GET_ROW_COUNT( aRelationHandle ),
                                     STL_FALSE,
                                     SMDMPF_ROW_HDR_LEN,
                                     SMDMPF_GET_PAGE_COL_COUNT( SMP_FRAME(&sPageHandle) ),
                                     smdmpfIsUsableRowHdr,
                                     sSlotArray,
                                     &sSlotSeq ) == STL_SUCCESS );

    STL_ASSERT( sSlotArray[0] != NULL );

    sColCount = SMDMPF_GET_PAGE_COL_COUNT( SMP_FRAME(&sPageHandle) );

    STL_TRY( smdmpfWriteRowHeader( sRowBuf,
                                   aTcn,
                                   aTransId,
                                   sColCount ) == STL_SUCCESS );

    /* row header 기록 */
    sSlotIdx = 0;
    stlMemcpy( sSlotArray[sSlotIdx], sRowBuf, SMDMPF_ROW_HDR_LEN );

    sScn = SML_MAKE_VIEW_SCN(aViewScn);
    SMDMPF_UNSET_RTS_BOUND( sSlotArray[sSlotIdx] );
    SMDMPF_SET_SCN( sSlotArray[sSlotIdx], &sScn );

    /* master row이면 row header slot에 master flag 설정 */
    if( aIsMasterRow == STL_TRUE )
    {
        SMDMPF_SET_MASTER_ROW( sSlotArray[sSlotIdx] );
    }
    else
    {
        SMDMPF_UNSET_MASTER_ROW( sSlotArray[sSlotIdx] );
    }

    sRowPtr = sRowBuf + SMDMPF_ROW_HDR_LEN;

    for( i = 0; i < sColCount; i++ )
    {
        sSlotIdx++;
        sDataValue = KNL_GET_BLOCK_DATA_VALUE( aInsertCols[aFirstColOrd + i], aValueIdx );
        sColLen    = sDataValue->mLength;
        sIsZero    = STL_FALSE;

        if( DTL_NUMERIC_IS_ZERO(sDataValue->mValue, sColLen) )
        {
            if( dtlDataTypeGroup[sDataValue->mType] == DTL_GROUP_NUMBER )
            {
                sColLen = 0;
                sIsZero = STL_TRUE;
            }
        }

        if( sColLen > sThreshold )
        {
            sColLen = sThreshold;
            SMP_GET_CONT_COLUMN_LEN_SIZE( &sColLenSize );

            SMP_WRITE_CONT_COLUMN( sSlotArray[sSlotIdx],
                                   sDataValue->mValue,
                                   aContColPid[sContColPidSeq++],
                                   sColLen );
        }
        else
        {
            SMP_GET_COLUMN_LEN_SIZE( sColLen, &sColLenSize );

            SMP_WRITE_COLUMN( sSlotArray[sSlotIdx],
                              sDataValue->mValue,
                              sColLen,
                              sIsZero );
        }

        /* write column */
        stlMemcpy( sRowPtr, sSlotArray[sSlotIdx], sColLen + sColLenSize );

        sRowPtr += sColLen + sColLenSize;
    }

    if( aContColCount > 0 )
    {
        sLogicalHdr = SMDMPF_GET_LOGICAL_HEADER( SMP_FRAME(&sPageHandle) );

        if( sLogicalHdr->mSimplePage == STL_TRUE )
        {
            sLogicalHdr->mSimplePage = STL_FALSE;
            STL_TRY( smxmWriteLog( &sMiniTrans,
                                   SMG_COMPONENT_TABLE,
                                   SMR_LOG_MEMORY_HEAP_UPDATE_LOGICAL_HDR,
                                   SMR_REDO_TARGET_PAGE,
                                   (stlChar*)sLogicalHdr,
                                   STL_SIZEOF(smdmpfLogicalHdr),
                                   sPageHandle.mPch->mTbsId,
                                   sPageHandle.mPch->mPageId,
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
    stlMemcpy( sLogPtr, sRowBuf, sRowPtr - sRowBuf );
    sLogPtr += sRowPtr - sRowBuf;

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

    if( (smpGetFreeness(&sPageHandle) == SMP_FREENESS_INSERTABLE) &&
        (SMDMPF_IS_PAGE_OVER_INSERT_LIMIT(SMP_FRAME(&sPageHandle)) == STL_TRUE) )
    {
        STL_TRY( smdmpfUpdatePageStatus( &sMiniTrans,
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

    /* write redo log */

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

stlStatus smdmpfGetInsertablePage( void              * aRelationHandle,
                                   smxmTrans         * aMiniTrans,
                                   smxlTransId         aTransId,
                                   smlScn              aViewScn,
                                   smlPid              aChainedPageId,
                                   stlUInt8            aLogicalPageType,
                                   smpHandle         * aPageHandle,
                                   stlInt16          * aSlotSeq,
                                   smlEnv            * aEnv )
{
    /**
     * Anchor member page는 insertable anchor page를 구하고
     * 나머지 page는 chained page를 구한다.
     */
    if( aChainedPageId == SMP_NULL_PID )
    {
        STL_TRY( smdmpfGetInsertablePageList( aRelationHandle,
                                              aMiniTrans,
                                              aTransId,
                                              aViewScn,
                                              aLogicalPageType,
                                              aPageHandle,
                                              aSlotSeq,
                                              aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( smpAcquire( aMiniTrans,
                             smsGetTbsId( SME_GET_SEGMENT_HANDLE(aRelationHandle) ),
                             aChainedPageId,
                             KNL_LATCH_MODE_EXCLUSIVE,
                             aPageHandle,
                             aEnv ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmpfGetInsertablePageList( void              * aRelationHandle,
                                       smxmTrans         * aMiniTrans,
                                       smxlTransId         aTransId,
                                       smlScn              aViewScn,
                                       stlUInt8            aLogicalPageType,
                                       smpHandle         * aPageHandle,
                                       stlInt16          * aSlotSeq,
                                       smlEnv            * aEnv )
{
    smpHandle          sPageHandle;
    stlChar          * sSlotArray[SMD_MAX_COLUMN_COUNT_IN_PIECE + 1];
    stlInt16           sSlotSeq;
    stlUInt32          sAttr = SMXM_ATTR_REDO;
    smlRid             sSegRid;
    stlBool            sUseInsertablePageHint;
    stlUInt16          sRetryCnt = 0;
    stlInt32           sState = 1;
    void             * sSegmentHandle = SME_GET_SEGMENT_HANDLE(aRelationHandle);
    stlBool            sIsNewPage;
    smeHint          * sRelHint;
    smsSearchHint      sSearchHint;

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
            STL_ASSERT( sRetryCnt < SMDMPF_MAXIMUM_INSERTABLE_PAGE_SEARCH_COUNT + 1 );
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
            if( sRetryCnt < SMDMPF_MAXIMUM_INSERTABLE_PAGE_SEARCH_COUNT )
            {
                STL_TRY( smsGetInsertablePage( aMiniTrans,
                                               sSegmentHandle,
                                               SMP_PAGE_TYPE_TABLE_COLUMNAR_DATA,
                                               smdmpfIsAgable,
                                               &sSearchHint,
                                               &sPageHandle,
                                               &sIsNewPage,
                                               aEnv ) == STL_SUCCESS );
            }
            else
            {
                STL_TRY( smsAllocPage( aMiniTrans,
                                       sSegmentHandle,
                                       SMP_PAGE_TYPE_TABLE_COLUMNAR_DATA,
                                       smdmpfIsAgable,
                                       &sPageHandle,
                                       aEnv ) == STL_SUCCESS );

                sIsNewPage = STL_TRUE;
            }

            /**
             * New page를 alloc 받았으면 새로운 member page들을 할당한다.
             */
            if( sIsNewPage == STL_TRUE )
            {
                STL_TRY( smdmpfAllocMemberPages( aRelationHandle,
                                                 aMiniTrans,
                                                 aTransId,
                                                 &sPageHandle,
                                                 aEnv ) == STL_SUCCESS );
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
            
            if( smpIsAgable( &sPageHandle, smdmpfIsAgable, aEnv ) == STL_TRUE )
            {
                STL_TRY( smdmpfUpdatePageStatus( aMiniTrans,
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

        /**
         * Anchor page가 아니면 retry
         */
        if( SMDMPF_IS_ANCHOR_MEMBER_PAGE(SMP_FRAME(&sPageHandle)) != STL_TRUE )
        {
            sRetryCnt++;
            continue;
        }

        if( smpGetFreeness( &sPageHandle ) < SMP_FREENESS_INSERTABLE )
        {
            sRetryCnt++;
            continue;
        }

        if( SMDMPF_GET_SLOT_COUNT(SMP_FRAME(&sPageHandle)) == SMD_GET_ROW_COUNT(aRelationHandle) )
        {
            STL_TRY( smdmpfCompactPage( aMiniTrans,
                                        &sPageHandle,
                                        NULL,
                                        aEnv ) == STL_SUCCESS );
        }

        STL_TRY( smpAllocFixedSlotArray( &sPageHandle,
                                         SMD_GET_ROW_COUNT( aRelationHandle ),
                                         STL_TRUE,
                                         SMDMPF_ROW_HDR_LEN,
                                         SMDMPF_GET_PAGE_COL_COUNT( SMP_FRAME(&sPageHandle) ),
                                         smdmpfIsUsableRowHdr,
                                         sSlotArray,
                                         &sSlotSeq ) == STL_SUCCESS );

        if( sSlotArray[0] == NULL )
        {
            STL_TRY( smdmpfUpdatePageStatus( aMiniTrans,
                                             aRelationHandle,
                                             sSegmentHandle,
                                             &sPageHandle,
                                             SMP_FREENESS_UPDATEONLY,
                                             0,  /* aScn */
                                             aEnv ) == STL_SUCCESS );
            sRetryCnt++;
            continue;
        }

        /* 모든 할당에 성공했으므로 루프에서 나간다 */
        break;
    }

    *aPageHandle = sPageHandle;
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
 * @brief Columnar table을 위한 member page 를 할당한다.
 * @param[in] aRelationHandle Relation Handle
 * @param[in] aMiniTrans Mini-transaction Pointer
 * @param[in] aTransId 현재 statement의 Transaction Id
 * @param[out] aAnchorPage 할당한 Anchor 페이지의 Page Handle
 * @param[in,out] aEnv Environment 정보
 * @note 
 */
stlStatus smdmpfAllocMemberPages( void           * aRelationHandle,
                                  smxmTrans      * aMiniTrans,
                                  smxlTransId      aTransId,
                                  smpHandle      * aAnchorPage,
                                  smlEnv         * aEnv )
{
    smxmTrans          sMiniTrans;
    smpHandle          sPageHandle;
    smpHandle          sPrevPageHandle;
    stlUInt32          sAttr = SMXM_ATTR_REDO;
    smlRid             sSegRid;
    stlInt32           sState = 1;
    void             * sSegmentHandle = SME_GET_SEGMENT_HANDLE(aRelationHandle);
    smdmpfLogicalHdr   sLogicalHdr;
    stlInt32           sPageCount = SMD_GET_MEMBER_PAGE_COUNT(aRelationHandle);
    stlInt16           sFromOrd;
    stlInt16           sToOrd = -1;
    stlInt32         * sLastColIdArray = SMD_GET_LAST_COL_ID_ARRAY( aRelationHandle );
    smlPid             sPrevPageId;
    stlInt32           i;
    smlTbsId           sTbsId = smsGetTbsId( sSegmentHandle );
    stlUInt8           sMemberPageType;

    sSegRid = smsGetSegRid( sSegmentHandle );

    stlMemset( &sLogicalHdr, 0x00, STL_SIZEOF(smdmpfLogicalHdr) );

    sLogicalHdr.mSimplePage     = STL_TRUE;
    sLogicalHdr.mMemberPageType = SMDMPF_MEMBER_PAGE_TYPE_MASTER;
    sLogicalHdr.mFromOrd        = 0;
    sLogicalHdr.mToOrd          = (sPageCount == 1)? SMD_GET_COL_COUNT(aRelationHandle) - 1: sLastColIdArray[0];
    sLogicalHdr.mRowCount       = SMD_GET_ROW_COUNT( aRelationHandle );
    sLogicalHdr.mNextChainedPid = SMP_NULL_PID;
    sLogicalHdr.mPrevChainedPid = SMP_NULL_PID;
    sLogicalHdr.mSegmentSeq     = smsGetValidSeqFromHandle( sSegmentHandle );

    if( sPageCount > 1 )
    {
        sMemberPageType = SMDMPF_MEMBER_PAGE_TYPE_MASTER;
        sPrevPageId     = SMP_NULL_PID;

        sLogicalHdr.mSimplePage     = STL_FALSE;
        sLogicalHdr.mPrevChainedPid = sPrevPageId;

        for( i = 0; i < sPageCount - 1; i++ )
        {
            STL_TRY( smxmBegin( &sMiniTrans,
                                aTransId,
                                sSegRid,
                                sAttr,
                                aEnv )
                     == STL_SUCCESS );
            sState = 1;

            STL_TRY( smsAllocPage( &sMiniTrans,
                                   sSegmentHandle,
                                   SMP_PAGE_TYPE_TABLE_COLUMNAR_DATA,
                                   smdmpfIsAgable,
                                   &sPageHandle,
                                   aEnv ) == STL_SUCCESS );

            /**
             * Prev member page의 logical header를 저장하고 logging한다.
             */
            if( sPrevPageId != SMP_NULL_PID )
            {
                STL_TRY( smpAcquire( &sMiniTrans,
                                     sTbsId,
                                     sPrevPageId,
                                     KNL_LATCH_MODE_EXCLUSIVE,
                                     &sPrevPageHandle,
                                     aEnv ) == STL_SUCCESS );

                sLogicalHdr.mMemberPageType = sMemberPageType;
                sLogicalHdr.mNextChainedPid = smpGetPageId( &sPageHandle );

                sFromOrd = (i > 1)? sLastColIdArray[i - 2] + 1: 0;
                sToOrd   = sLastColIdArray[i - 1];
                sLogicalHdr.mFromOrd = sFromOrd;
                sLogicalHdr.mToOrd   = sToOrd;

                STL_DASSERT( sFromOrd <= sToOrd );
                STL_DASSERT( sLogicalHdr.mFromOrd <= sLogicalHdr.mToOrd );

                STL_TRY( smpInitFixedPageBody( &sMiniTrans,
                                               &sPrevPageHandle,
                                               STL_TRUE,
                                               SMP_FIXED_SLOT_TYPE,
                                               STL_SIZEOF(smdmpfLogicalHdr),
                                               (stlChar*)&sLogicalHdr,
                                               SMDMPF_ROW_HDR_LEN,
                                               sLogicalHdr.mFromOrd,
                                               sLogicalHdr.mToOrd,
                                               SMD_GET_COL_LENGTH_ARRAY(aRelationHandle),
                                               SMD_GET_CONT_COLUMN_THRESHOLD(aRelationHandle),
                                               aEnv ) == STL_SUCCESS );

                if( sMemberPageType == SMDMPF_MEMBER_PAGE_TYPE_MASTER )
                {
                    sMemberPageType = SMDMPF_MEMBER_PAGE_TYPE_SLAVE;
                }

                sLogicalHdr.mPrevChainedPid = sPrevPageId;
            }

            sPrevPageId = smpGetPageId( &sPageHandle );

            sState = 0;
            STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
        }

        /**
         * Anchor Page의 직전 page init
         */
        STL_DASSERT( sPrevPageId != SMP_NULL_PID );

        sLogicalHdr.mMemberPageType = sMemberPageType;
        sLogicalHdr.mNextChainedPid = smpGetPageId( aAnchorPage );

        sLogicalHdr.mFromOrd = sToOrd + 1;
        sLogicalHdr.mToOrd   = sLastColIdArray[i - 1];

        STL_TRY( smxmBegin( &sMiniTrans,
                            aTransId,
                            sSegRid,
                            sAttr,
                            aEnv )
                 == STL_SUCCESS );
        sState = 1;

        STL_TRY( smpAcquire( &sMiniTrans,
                             sTbsId,
                             sPrevPageId,
                             KNL_LATCH_MODE_EXCLUSIVE,
                             &sPrevPageHandle,
                             aEnv ) == STL_SUCCESS );

        STL_DASSERT( sLogicalHdr.mFromOrd <= sLogicalHdr.mToOrd );

        STL_TRY( smpInitFixedPageBody( &sMiniTrans,
                                       &sPrevPageHandle,
                                       STL_TRUE,
                                       SMP_FIXED_SLOT_TYPE,
                                       STL_SIZEOF(smdmpfLogicalHdr),
                                       (stlChar*)&sLogicalHdr,
                                       SMDMPF_ROW_HDR_LEN,
                                       sLogicalHdr.mFromOrd,
                                       sLogicalHdr.mToOrd,
                                       SMD_GET_COL_LENGTH_ARRAY(aRelationHandle),
                                       SMD_GET_CONT_COLUMN_THRESHOLD(aRelationHandle),
                                       aEnv ) == STL_SUCCESS );

        sState = 0;
        STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

        sLogicalHdr.mNextChainedPid = SMP_NULL_PID;
        sLogicalHdr.mPrevChainedPid = sPrevPageId;

        sLogicalHdr.mFromOrd  = sLastColIdArray[sPageCount - 2] + 1;
        sLogicalHdr.mToOrd    = sLastColIdArray[sPageCount - 1];

        sLogicalHdr.mMemberPageType = SMDMPF_MEMBER_PAGE_TYPE_SLAVE;
    }

    /**
     * Anchor Page Init
     */
    sLogicalHdr.mMemberPageType |= SMDMPF_MEMBER_PAGE_ANCHOR;

    STL_DASSERT( sLogicalHdr.mFromOrd <= sLogicalHdr.mToOrd );

    STL_TRY( smpInitFixedPageBody( aMiniTrans,
                                   aAnchorPage,
                                   STL_TRUE,
                                   SMP_FIXED_SLOT_TYPE,
                                   STL_SIZEOF(smdmpfLogicalHdr),
                                   (stlChar*)&sLogicalHdr,
                                   SMDMPF_ROW_HDR_LEN,
                                   sLogicalHdr.mFromOrd,
                                   sLogicalHdr.mToOrd,
                                   SMD_GET_COL_LENGTH_ARRAY(aRelationHandle),
                                   SMD_GET_CONT_COLUMN_THRESHOLD(aRelationHandle),
                                   aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smxmCommit( &sMiniTrans, aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief 주어진 continuous column을 담은 row piece를 삽입한다
 * @param[in] aStatement 사용중인 statement
 * @param[in] aRelationHandle 삽입할 Relation의 handle
 * @param[in] aValueIdx 삽입할 row의 value block내에서의 순번
 * @param[in] aFromOrder Member page의 첫번째 column sequence
 * @param[in] aToOrder Member page의 마지막 column order
 * @param[in] aContCol 삽입할 Continuous column 정보
 * @param[in] aIsMaster Master인지 여부
 * @param[in] aIsForUpdate Update 중 delete/insert에 의한 호출인지 여부
 * @param[out] aContColPid Continuous column을 저장한 첫 row piece의 위치
 * @param[out] aContColCount Continuous column count
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmpfInsertContCols( smlStatement       * aStatement,
                                void               * aRelationHandle,
                                stlInt32             aValueIdx,
                                stlInt32             aFromOrder,
                                stlInt32             aToOrder,
                                knlValueBlockList ** aContCol,
                                stlBool              aIsMaster,
                                stlBool              aIsForUpdate,
                                smlPid             * aContColPid,
                                stlInt16           * aContColCount,
                                smlEnv             * aEnv )
{
    dtlDataValue     * sDataValue;
    stlInt16           sContColCount = 0;
    stlInt32           i;
    stlInt16           sThreshold = SMD_GET_CONT_COLUMN_THRESHOLD( aRelationHandle );

    for( i = aToOrder; i >= aFromOrder; i-- )
    {
        sDataValue = KNL_GET_BLOCK_DATA_VALUE( aContCol[i], aValueIdx );

        if( sDataValue->mLength > sThreshold )
        {
            STL_TRY( smdmpfInsertContCol( SMA_GET_TRANS_ID(aStatement),
                                          SMA_GET_TCN( aStatement ),
                                          aRelationHandle,
                                          aValueIdx,
                                          aContCol[i],
                                          STL_FALSE, /* aIsForUpdate */
                                          &aContColPid[sContColCount],
                                          aEnv ) == STL_SUCCESS );

            sContColCount++;
        }
    }

    *aContColCount = sContColCount;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
} 

/**
 * @brief 주어진 continuous column을 담은 row piece를 삽입한다
 * @param[in] aTransId 현재 statement의 Transaction Id
 * @param[in] aStmtTcn 현재 statement의 Tcn
 * @param[in] aRelationHandle 삽입할 Relation의 handle
 * @param[in] aValueIdx 삽입할 row의 value block내에서의 순번
 * @param[in] aContCol 삽입할 Continuous column 정보
 * @param[in] aIsForUpdate Update 중 delete/insert에 의한 호출인지 여부
 * @param[out] aFirstPageId 삽입된 첫 row piece의 위치
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmpfInsertContCol( smxlTransId         aTransId,
                               smlTcn              aStmtTcn,
                               void              * aRelationHandle,
                               stlInt32            aValueIdx,
                               knlValueBlockList * aContCol,
                               stlBool             aIsForUpdate,
                               smlPid            * aFirstPageId,
                               smlEnv            * aEnv )
{
    smlRid             sSegRid;
    stlInt32           sState = 0;
    smxmTrans          sMiniTrans;
    stlUInt32          sAttr = SMXM_ATTR_REDO;
    void             * sSegmentHandle = SME_GET_SEGMENT_HANDLE(aRelationHandle);
    dtlDataValue     * sDataValue = KNL_GET_BLOCK_DATA_VALUE(aContCol, aValueIdx);
    smlScn             sViewScn = smxlGetTransViewScn( aTransId );
    stlChar            sRowBuf[SMP_PAGE_SIZE];
    stlChar          * sRowPtr;
    stlChar          * sRowHdr;
    stlChar          * sDataPtr;
    stlInt16           sPieceLength;
    stlInt16           sMaxPieceSize;
    stlInt64           sToOffset;
    stlInt64           sFromOffset = sDataValue->mLength;
    stlUInt8           sColLenSize;
    stlInt16           sThreshold = SMD_GET_CONT_COLUMN_THRESHOLD( aRelationHandle );
    smlPid             sNextPid = SMP_NULL_PID;
    stlChar          * sSlot;
    smlRid             sUndoRid;
    stlChar            sLogBuf[SMP_PAGE_SIZE];
    stlChar          * sLogPtr;
    stlInt32           sFirstColOrd = KNL_GET_BLOCK_COLUMN_ORDER( aContCol );
    smpHandle          sPageHandle;
    smdmpfLogicalHdr   sLogicalHdr;
    stlInt16           sLogType = ( (aIsForUpdate == STL_TRUE) ?
                                    SMR_LOG_MEMORY_HEAP_INSERT_FOR_UPDATE :
                                    SMR_LOG_MEMORY_HEAP_INSERT );

    sMaxPieceSize = SMDMPF_PAGE_MAX_FREE_SIZE( 0 );
    sSegRid = smsGetSegRid( sSegmentHandle );

    /* 뒤쪽 piece부터 insert */
    sToOffset = sDataValue->mLength;
    SMP_GET_CONT_COLUMN_LEN_SIZE( &sColLenSize );

    while( sToOffset > sThreshold  )
    {
        sRowHdr = sRowBuf + SMP_PAGE_SIZE;

        if( (sToOffset - sThreshold + sColLenSize + SMDMPF_ROW_HDR_LEN) > sMaxPieceSize )
        {
            /**
             * Continuous column을 저장하기 위해 next piece가 필요한 경우,
             * continuous flag, next page id, length를 제외하고 남은
             * 공간에 value piece를 쓸 수 있다.
             */
            sPieceLength = sMaxPieceSize - (SMDMPF_ROW_HDR_LEN + sColLenSize);
        }
        else
        {
            /**
             * Member page에 기록될 threshold 크기를 제외한 나머지 piece가
             * 모두 저장가능할 때 threshold를 뺀 크기만큼이 piece length이다.
             */
            sPieceLength = sToOffset - sThreshold;
        }

        sRowHdr -= (sPieceLength + SMDMPF_ROW_HDR_LEN + sColLenSize);
        sRowPtr = sRowHdr + SMDMPF_ROW_HDR_LEN;

        STL_TRY( smdmpfWriteRowHeader( sRowHdr,
                                       aStmtTcn,
                                       aTransId,
                                       1 ) == STL_SUCCESS );

        sFromOffset = sToOffset - sPieceLength;
        sDataPtr = (stlChar *)sDataValue->mValue + sFromOffset;

        /* continuous column의 next piece의 Pid를 기록 */
        SMP_WRITE_CONT_COLUMN( sRowPtr,
                               sDataPtr,
                               sNextPid,
                               sPieceLength );

        STL_TRY( smxmBegin( &sMiniTrans,
                            aTransId,
                            sSegRid,
                            sAttr,
                            aEnv )
                 == STL_SUCCESS );
        sState = 1;

        /**
         * continuous column을 저장하기 위해 new page를 할당한다.
         */ 
        STL_TRY( smsAllocPage( &sMiniTrans,
                               sSegmentHandle,
                               SMP_PAGE_TYPE_TABLE_COLUMNAR_DATA,
                               smdmpfIsAgable,
                               &sPageHandle,
                               aEnv ) == STL_SUCCESS );

        stlMemset( &sLogicalHdr, 0x00, STL_SIZEOF(smdmpfLogicalHdr) );

        sLogicalHdr.mSimplePage     = STL_FALSE;
        sLogicalHdr.mMemberPageType = SMDMPF_MEMBER_PAGE_TYPE_CONT;
        sLogicalHdr.mSegmentSeq     = smsGetValidSeqFromHandle( sSegmentHandle );
        sLogicalHdr.mRowCount       = 1;

        STL_TRY( smpInitFixedPageBody( &sMiniTrans,
                                       &sPageHandle,
                                       STL_TRUE,
                                       SMP_FIXED_SLOT_TYPE,
                                       STL_SIZEOF(smdmpfLogicalHdr),
                                       (stlChar*)&sLogicalHdr,
                                       0,
                                       0,
                                       0,
                                       NULL,
                                       SMD_GET_CONT_COLUMN_THRESHOLD(aRelationHandle),
                                       aEnv ) == STL_SUCCESS );

        /* write undo record */
        STL_TRY( smxlInsertUndoRecord( &sMiniTrans,
                                       SMG_COMPONENT_TABLE,
                                       SMD_UNDO_LOG_MEMORY_HEAP_INSERT,
                                       NULL,
                                       0,
                                       ((smlRid){ sPageHandle.mPch->mTbsId,
                                               0,
                                               sPageHandle.mPch->mPageId }),
                                       &sUndoRid,
                                       aEnv ) == STL_SUCCESS );

        sSlot = (stlChar *)SMDMPF_GET_START_POS(SMP_FRAME(&sPageHandle));

        stlMemcpy( sSlot, sRowHdr, (sRowBuf + SMP_PAGE_SIZE - sRowHdr) );

        /* write REDO log */
        sLogPtr = sLogBuf;
        STL_WRITE_INT64( sLogPtr, &aTransId );
        sLogPtr += STL_SIZEOF(smxlTransId);
        STL_WRITE_INT64( sLogPtr, &sViewScn );
        sLogPtr += STL_SIZEOF(smlScn);
        STL_WRITE_INT32( sLogPtr, &sFirstColOrd );
        sLogPtr += STL_SIZEOF(stlInt32);
        stlMemcpy( sLogPtr, sRowHdr, (sRowBuf + SMP_PAGE_SIZE - sRowHdr) );
        sLogPtr += (sRowBuf + SMP_PAGE_SIZE - sRowHdr);

        STL_TRY( smxmWriteLog( &sMiniTrans,
                               SMG_COMPONENT_TABLE,
                               sLogType,
                               SMR_REDO_TARGET_PAGE,
                               sLogBuf,
                               sLogPtr - sLogBuf,
                               sPageHandle.mPch->mTbsId,
                               sPageHandle.mPch->mPageId,
                               0,  /* Physiological redo를 하기위해 offset이 아닌 seq 사용 */
                               SMXM_LOGGING_REDO,
                               aEnv ) == STL_SUCCESS );

        SMDMPF_INCREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(&sPageHandle) );

        /**
         * continuous page는 하나의 piece를 기록하면 UPDATEONLY가 된다.
         */
        STL_TRY( smdmpfUpdatePageStatus( &sMiniTrans,
                                         aRelationHandle,
                                         sSegmentHandle,
                                         &sPageHandle,
                                         SMP_FREENESS_UPDATEONLY,
                                         0,  /* aScn */
                                         aEnv ) == STL_SUCCESS );

        smpSetMaxViewScn( &sPageHandle, sViewScn );

        sState = 0;
        STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

        sToOffset = sFromOffset;
        sNextPid = smpGetPageId( &sPageHandle );
    }

    *aFirstPageId = sNextPid;

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smxmCommit( &sMiniTrans, aEnv );
    }

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
stlStatus smdmpfInsert( smlStatement      * aStatement,
                        void              * aRelationHandle,
                        stlInt32            aValueIdx,
                        knlValueBlockList * aInsertCols,
                        knlValueBlockList * aUniqueViolation,
                        smlRid            * aRowRid,
                        smlEnv            * aEnv )
{
    knlValueBlockList ** sColPtrArray;
    knlValueBlockList  * sCurCol = aInsertCols;
    smxlTransId          sTransId = SMA_GET_TRANS_ID( aStatement );
    smlScn               sViewScn = smxlGetTransViewScn( sTransId );
    knlRegionMark        sMemMark;
    stlInt32             sTotalColCount = 0;
    stlInt32             sState = 0;
    smlRid               sRowRid;
    stlInt32             sMemberPageCount;
    stlInt32           * sLastColIdArray;
    stlBool              sIsMasterRow;
    smlPid               sNextChainedPageId = SMP_NULL_PID;
    stlInt32             sPageSeq;
    stlInt32             sColOrd;
    stlInt32             sFromOrd;
    smlPid               sContColPid[SMD_MAX_COLUMN_COUNT_IN_PIECE];
    stlInt16             sContColCount;

    sTotalColCount = SMD_GET_COL_COUNT(aRelationHandle);

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
    for( sColOrd = 0; sColOrd < sTotalColCount; sColOrd++ )
    {
        sColPtrArray[sColOrd] = sCurCol;
        sCurCol = sCurCol->mNext;
    }

    sCurCol = aInsertCols;

    sMemberPageCount = SMD_GET_MEMBER_PAGE_COUNT( aRelationHandle );
    sLastColIdArray = SMD_GET_LAST_COL_ID_ARRAY( aRelationHandle );

    /* Member page가 하나인 경우 last column Id array는 NULL이다 */
    if( sLastColIdArray == NULL )
    {
        sTotalColCount--;
        sLastColIdArray = &sTotalColCount;
    }

    for( sPageSeq = sMemberPageCount - 1; sPageSeq >= 0; sPageSeq-- )
    {
        /**
         * Column value를 해당 member page에 기록한다.
         */
        sFromOrd = (sPageSeq == 0)? 0: sLastColIdArray[sPageSeq - 1] + 1;

        /**
         * Member page에 저장될 LONG VARBINARY, LONG VARCHAR column을 먼저 저장한다.
         */
        STL_TRY( smdmpfInsertContCols( aStatement,
                                       aRelationHandle,
                                       aValueIdx,
                                       sFromOrd,
                                       sLastColIdArray[sPageSeq],
                                       sColPtrArray,
                                       (sPageSeq == 0) ? STL_TRUE : STL_FALSE,
                                       STL_FALSE, /* aIsForUpdate */
                                       sContColPid,
                                       &sContColCount,
                                       aEnv ) == STL_SUCCESS );

        if( sFromOrd == 0 )
        {
            sIsMasterRow = STL_TRUE;
        }
        else
        {
            sIsMasterRow = STL_FALSE;
        }

        STL_TRY( smdmpfInsertRowPiece( aRelationHandle,
                                       aValueIdx,
                                       sTransId,
                                       sViewScn,
                                       aStatement->mTcn,
                                       sColPtrArray,
                                       sFromOrd,
                                       sIsMasterRow,
                                       STL_FALSE,     /* aIsForUpdate */
                                       sContColPid,
                                       sContColCount,
                                       &sNextChainedPageId,
                                       &sRowRid,
                                       aEnv ) == STL_SUCCESS );

        if( sIsMasterRow == STL_TRUE )
        {
            *aRowRid = sRowRid;
        }
    }

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
stlStatus smdmpfBlockInsert( smlStatement      * aStatement,
                             void              * aRelationHandle,
                             knlValueBlockList * aInsertCols,
                             knlValueBlockList * aUniqueViolation,
                             smlRowBlock       * aRowBlock,
                             smlEnv            * aEnv )
{
    stlInt32    sBlockIdx;
    smlRid      sRowRid;
    stlInt16  * sColLengthArray;
    stlInt32    i;
    stlInt32    sColCount;
    stlBool     sIsBlockInsertable = STL_TRUE;
    stlInt16    sThreshold;

    if( SMD_GET_MEMBER_PAGE_COUNT(aRelationHandle) > 1 )
    {
        sIsBlockInsertable = STL_FALSE;
    }

    if( sIsBlockInsertable == STL_TRUE )
    {
        sColCount = SMD_GET_COL_COUNT(aRelationHandle);
        sColLengthArray = SMD_GET_COL_LENGTH_ARRAY( aRelationHandle );
        sThreshold = SMD_GET_CONT_COLUMN_THRESHOLD( aRelationHandle );

        for( i = 0; i < sColCount; i++ )
        {
            if( sColLengthArray[i] > sThreshold )
            {
                sIsBlockInsertable = STL_FALSE;
                break;
            }
        }
    }

    /**
     * Member page가 한개일 때만 block insert가 가능하다.
     */
    if( sIsBlockInsertable == STL_TRUE )
    {
        STL_TRY( smdmpfBlockInsertInternal( aStatement,
                                            aRelationHandle,
                                            aInsertCols,
                                            KNL_GET_BLOCK_SKIP_CNT( aInsertCols ),
                                            KNL_GET_BLOCK_USED_CNT( aInsertCols ),
                                            aRowBlock,
                                            aEnv )
                 == STL_SUCCESS );

        sBlockIdx = KNL_GET_BLOCK_USED_CNT( aInsertCols );
    }
    else
    {
        for( sBlockIdx = KNL_GET_BLOCK_SKIP_CNT( aInsertCols );
             sBlockIdx < KNL_GET_BLOCK_USED_CNT( aInsertCols );
             sBlockIdx++ )
        {
            STL_TRY( smdmpfInsert( aStatement,
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

    SML_SET_USED_BLOCK_SIZE( aRowBlock, sBlockIdx );
 
    return STL_SUCCESS;

    STL_FINISH;

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
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmpfBlockInsertInternal( smlStatement       * aStatement,
                                     void               * aRelationHandle,
                                     knlValueBlockList  * aInsertCols,
                                     stlInt32             aStartBlockIdx,
                                     stlInt32             aEndBlockIdx,
                                     smlRowBlock        * aRowBlock,
                                     smlEnv             * aEnv )
{
    stlUInt32          sState = 0;
    smxlTransId        sTransId = SMA_GET_TRANS_ID( aStatement );
    smlScn             sViewScn = smxlGetTransViewScn( sTransId );
    void             * sSegmentHandle = SME_GET_SEGMENT_HANDLE(aRelationHandle);
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
    stlInt16           sRowOffsetArray[SMP_PAGE_SIZE / SMDMPF_MINIMUM_ROW_PIECE_SIZE];
    stlInt16           sArrayIdx = 0;
    smpCtrlHeader      sPageStack[KNL_ENV_MAX_LATCH_DEPTH];
    smxmTrans          sSnapshotMtx;
    stlBool            sIsZero;
    stlChar          * sSlotArray[SMD_MAX_COLUMN_COUNT_IN_PIECE + 1];
    knlValueBlockList  * sCurCol = aInsertCols;
    knlValueBlockList ** sColPtrArray;
    stlInt32             sColCount = 0;
    knlRegionMark        sMemMark;
    stlInt16             sSlotIdx;
    stlInt16             sRowOffset;

    sSegRid = smsGetSegRid( sSegmentHandle );

    while( sCurCol != NULL )
    {
        sColCount++;
        sCurCol = sCurCol->mNext;
    }

    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                sColCount * STL_SIZEOF(knlValueBlockList*),
                                (void**)&sColPtrArray,
                                (knlEnv*)aEnv ) == STL_SUCCESS );

    sCurCol = aInsertCols;
    for( i = 0; i < sColCount; i++ )
    {
        sColPtrArray[i] = sCurCol;
        sCurCol = sCurCol->mNext;
    }

    /* Redo log 보다 Undo log를 나중에 쓰기 때문에 Tx record를 미리 쓴다 */ 
    STL_TRY( smxlInsertTransRecord( sTransId, aEnv ) == STL_SUCCESS );

    STL_TRY( smxmBegin( &sMiniTrans,
                        sTransId,
                        sSegRid,
                        sAttr,
                        aEnv )
             == STL_SUCCESS );
    sState = 2;

    for( sBlockIdx = aStartBlockIdx; sBlockIdx < aEndBlockIdx; sBlockIdx++ )
    {
        sTotalRowSize = SMDMPF_ROW_HDR_LEN;

        sRowHeader = sRowBuf;
        sRowPtr = sRowHeader + SMDMPF_ROW_HDR_LEN;

        for( i = 0; i < sColCount; i++ )
        {
            sIsZero = STL_FALSE;
            sDataValue = KNL_GET_BLOCK_DATA_VALUE( sColPtrArray[i], sBlockIdx );
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

            /* write column */
            SMP_WRITE_COLUMN( sRowPtr, (stlChar*)sDataValue->mValue, sColLen, sIsZero );
            sRowPtr += sColLen + sColLenSize;

            sTotalRowSize += sColLen + sColLenSize;
        }
        
        while( STL_TRUE )
        {
            if( sInsertablePage == STL_FALSE )
            {
                /**
                 * Block insert를 위한 page는 member page가 한개인 경우에만 가능하므로
                 * Logical page type이 MASERT이고 ANCHOR인 insertable page를 구한다.
                 */
                STL_TRY( smdmpfGetInsertablePage( aRelationHandle,
                                                  &sMiniTrans,
                                                  sTransId,
                                                  sViewScn,
                                                  SMP_NULL_PID,
                                                  ( SMDMPF_MEMBER_PAGE_TYPE_MASTER &
                                                    SMDMPF_MEMBER_PAGE_ANCHOR ),
                                                  &sPageHandle,
                                                  &sSlotSeq,
                                                  aEnv )
                         == STL_SUCCESS );

                /**
                 * NEW 페이지를 할당한 경우에 Mini-transaction rollback이 발생한 경우는
                 * INIT_PAGE_BODY로그가 기록되지 않아 recovery시에 문제가 발생할수 있다.
                 * - 지금까지의 로그를 기록하고 다시 시작한다.
                 */

                sState = 1;
                STL_TRY( smxmSnapshotAndCommit( &sMiniTrans,
                                                sPageStack,
                                                &sSnapshotMtx,
                                                aEnv )
                         == STL_SUCCESS );
                
                STL_TRY( smxmBeginWithSnapshot( &sMiniTrans,
                                                &sSnapshotMtx,
                                                aEnv )
                         == STL_SUCCESS );
                sState = 2;

                if( smpGetFreeness( &sPageHandle ) != SMP_FREENESS_INSERTABLE )
                {
                    /**
                     * Insertable Page를 구한 후 Mini-transaction 재시작으로 Page의 상태가 바뀔 수 있다.
                     * 만약 Insertable이 아니면 다시 retry한다.
                     */
                    sState = 1;
                    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

                    STL_TRY( smxmBegin( &sMiniTrans,
                                        sTransId,
                                        sSegRid,
                                        sAttr,
                                        aEnv ) == STL_SUCCESS );
                    sState = 2;

                    sInsertablePage = STL_FALSE;
                    continue;
                }

                STL_TRY( smdmpfWriteRowHeader( sRowHeader,
                                               aStatement->mTcn,
                                               sTransId,
                                               sColCount ) == STL_SUCCESS );

                SMDMPF_SET_MASTER_ROW( sRowHeader );
                SMDMPF_UNSET_CONT_COL( sRowHeader );

                sInsertablePage = STL_TRUE;
                sArrayIdx = 0;
            }

            STL_TRY( smpAllocFixedSlotArray( &sPageHandle,
                                             SMD_GET_ROW_COUNT( aRelationHandle ),
                                             STL_FALSE,
                                             SMDMPF_ROW_HDR_LEN,
                                             SMDMPF_GET_PAGE_COL_COUNT( SMP_FRAME(&sPageHandle) ),
                                             smdmpfIsUsableRowHdr,
                                             sSlotArray,
                                             &sSlotSeq ) == STL_SUCCESS );

            if( sSlotArray[0] != NULL )
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

            sState = 1;
            STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
                
            STL_TRY( smxmBegin( &sMiniTrans,
                                sTransId,
                                sSegRid,
                                sAttr,
                                aEnv ) == STL_SUCCESS );
            sState = 2;
            sInsertablePage = STL_FALSE;
            sArrayIdx = 0;
        }

        /* Slot을 실제로 alloc한 slot seq를 row rid로 설정한다. */
        SML_MAKE_RID( &sRowRid,
                      sPageHandle.mPch->mTbsId,
                      sPageHandle.mPch->mPageId,
                      sSlotSeq );

        sSlotIdx = 0;

        /* row header 기록 */
        stlMemcpy( sSlotArray[sSlotIdx], sRowHeader, SMDMPF_ROW_HDR_LEN );

        sScn = SML_MAKE_VIEW_SCN(sViewScn);
        SMDMPF_UNSET_RTS_BOUND( sSlotArray[sSlotIdx] );
        SMDMPF_SET_SCN( sSlotArray[sSlotIdx], &sScn );

        /* master row이면 row header slot에 master flag 설정 */
        SMDMPF_SET_MASTER_ROW( sSlotArray[sSlotIdx] );
        sRowOffset = SMDMPF_ROW_HDR_LEN;
        sSlotIdx++;

        for( i = 0; i < sColCount; i++, sSlotIdx++ )
        {
            SMP_GET_COLUMN_LEN( sRowHeader + sRowOffset,
                                &sColLenSize,
                                &sColLen,
                                sIsZero );

            /* write column */
            stlMemcpy( sSlotArray[sSlotIdx],
                       sRowHeader + sRowOffset,
                       sColLenSize + sColLen );

            sRowOffset += sColLenSize + sColLen;
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
        stlMemcpy( sLogPtr, sRowBuf, sTotalRowSize );
        sLogPtr += sTotalRowSize;

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

        if( (smpGetFreeness(&sPageHandle) == SMP_FREENESS_INSERTABLE) &&
            (SMDMPF_IS_PAGE_OVER_INSERT_LIMIT(SMP_FRAME(&sPageHandle)) == STL_TRUE) )
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

            STL_TRY( smdmpfUpdatePageStatus( &sMiniTrans,
                                             aRelationHandle,
                                             sSegmentHandle,
                                             &sPageHandle,
                                             SMP_FREENESS_UPDATEONLY,
                                             0,  /* aScn */
                                             aEnv ) == STL_SUCCESS );
            
            sState = 1;
            STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
                
            STL_TRY( smxmBegin( &sMiniTrans,
                                sTransId,
                                sSegRid,
                                sAttr,
                                aEnv ) == STL_SUCCESS );
            sState = 2;

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

    sState = 1;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

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
    }

    return STL_FAILURE;
}

/**
 * @brief 주어진 src row로부터 dst row로 컬럼들을 update하면서 복사한다
 * @param[in] aPageHandle update할 row가 존재하는 page handle
 * @param[in] aSlotSeq update할 row의 slot seq
 * @param[in] aColumns 복사시에 update를 수행할 컬럼들의 정보
 * @param[in] aBlockIdx update를 수행할 컬럼들의 정보들의 block idx
 * @param[in] aSrcRowPieceSize src row의 길이
 * @param[in] aTotalColumnCount 복사할 컬럼의 총 개수
 * @param[in] aFirstColOrd aSrcRow의 첫 컬럼의 순번
 * @param[in] aContColPid Continuous column의 next page Id
 * @param[in] aRedoLogPtr 컬럼을 복사하면서 작성할 redo log 버퍼의 시작 주소
 * @param[out] aRedoLogEnd 기록한 redo log의 끝
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmpfCopyAndUpdateColumns( smpHandle          * aPageHandle,
                                      stlInt16             aSlotSeq,
                                      knlValueBlockList  * aColumns,
                                      stlInt32             aBlockIdx,
                                      stlInt16             aSrcRowPieceSize,
                                      stlInt16             aTotalColumnCount,
                                      stlInt32             aFirstColOrd,
                                      smlPid             * aContColPid,
                                      stlChar            * aRedoLogPtr,
                                      stlChar           ** aRedoLogEnd,
                                      smlEnv             * aEnv )
{
    knlValueBlockList * sCurCol;
    stlUInt8            sColLenSize;
    stlChar           * sColPtr;
    dtlDataValue      * sDataValue;
    stlUInt8            sColOrdIdx;
    stlBool             sIsZero;
    stlInt32            sColOrder;

    sCurCol = aColumns;

    while( sCurCol != NULL )
    {
        sColOrder = KNL_GET_BLOCK_COLUMN_ORDER(sCurCol);

        sColPtr = smdmpfGetNthColValue( aPageHandle,
                                        aSlotSeq,
                                        sColOrder );

        if( sColPtr == NULL )
        {
            break;
        }

        sDataValue = KNL_GET_BLOCK_DATA_VALUE(sCurCol, aBlockIdx);

        if( aContColPid[sColOrder - aFirstColOrd] != SMP_NULL_PID )
        {
            SMP_WRITE_CONT_COLUMN( sColPtr,
                                   sDataValue->mValue,
                                   aContColPid[sColOrder - aFirstColOrd],
                                   sDataValue->mLength );

            SMP_GET_CONT_COLUMN_LEN_SIZE( &sColLenSize );
        }
        else
        {
            sIsZero = STL_FALSE;

            if( DTL_NUMERIC_IS_ZERO(sDataValue->mValue, sDataValue->mLength) )
            {
                if( dtlDataTypeGroup[sDataValue->mType] == DTL_GROUP_NUMBER )
                {
                    sIsZero = STL_TRUE;
                }
            }

            /* write column */
            SMP_WRITE_COLUMN( sColPtr, sDataValue->mValue, sDataValue->mLength, sIsZero );
            SMP_GET_COLUMN_LEN_SIZE( sDataValue->mLength, &sColLenSize );
        }

        sCurCol = sCurCol->mNext;

        if( aRedoLogPtr != NULL )
        {
            /* REDO log 기록 */
            sColOrdIdx = (stlUInt8)(sColOrder - aFirstColOrd);
            STL_WRITE_INT8( aRedoLogPtr, &sColOrdIdx );
            aRedoLogPtr += STL_SIZEOF(stlInt8);
            stlMemcpy( aRedoLogPtr, sColPtr, sDataValue->mLength + sColLenSize );
            aRedoLogPtr += sDataValue->mLength + sColLenSize;
        }
    }

    if( aRedoLogPtr != NULL )
    {
        *aRedoLogEnd = aRedoLogPtr;
    }

    return STL_SUCCESS;
}


stlStatus smdmpfUpdateInPage( void              * aRelationHandle,
                              smxmTrans         * aMiniTrans,
                              smpHandle         * aPageHandle,
                              stlChar           * aUndoLog,
                              stlInt16            aUndoLogSize,
                              smlRid            * aRowRid,
                              stlChar           * aRowSlot,
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
    stlChar             sLogBuf[SMP_PAGE_SIZE];
    stlChar           * sLogPtr;
    stlChar           * sLogEnd;
    smlRid              sUndoRid;
    smlScn              sScn;
    knlValueBlockList * sCurCol = aAfterCols;
    stlInt32            sColOrder;
    dtlDataValue      * sDataValue;
    stlInt64            sColLen;
    stlBool             sIsZero;
    stlBool             sIsContCol;
    stlUInt8            sColLenSize;
    stlUInt8            sColOrdIdx;
    stlChar           * sColPtr;
    stlInt16            sUpdateColCnt = aUpdateColCnt;
    smlPid              sContColPid;
    stlInt16            sThreshold = SMD_GET_CONT_COLUMN_THRESHOLD( aRelationHandle );
    stlBool             sUsedContCols = STL_FALSE;
    smdmpfLogicalHdr  * sLogicalHdr;

    smpSetMaxViewScn( aPageHandle, aMyViewScn );

    /* redo log 처음의 transRid, trans view scn, row header와 first col ord,   *
     * update column count를 남겨두고 column들의 after image 부터 기록한다     */
    sLogPtr = sLogBuf + STL_SIZEOF(smxlTransId) + STL_SIZEOF(smlScn)
        + SMDMPF_ROW_HDR_LEN + STL_SIZEOF(stlInt32) + STL_SIZEOF(stlInt16);

    /* row의 길이가 현재보다 작거나 같다면 inplace updating을 한다 */
    if( aUpdateColCnt > 0 )
    {
        while( sCurCol != NULL )
        {
            sColOrder = KNL_GET_BLOCK_COLUMN_ORDER(sCurCol);

            sColPtr = smdmpfGetNthColValue( aPageHandle,
                                            aRowRid->mOffset,
                                            sColOrder );

            if( sColPtr == NULL )
            {
                STL_DASSERT( sUpdateColCnt == 0 );
                break;
            }

            /**
             * continuous column의 경우 continuous column이 저장된
             * piece를 delete한다.
             */
            SMP_GET_FIXED_COLUMN_LEN( sColPtr,
                                      &sColLenSize,
                                      &sColLen,
                                      sIsContCol,
                                      sIsZero );

            if( sIsContCol == STL_TRUE )
            {
                SMP_READ_CONT_COLUMN_PID( sColPtr, &sContColPid );

                STL_TRY( smdmpfDeleteContCol( aRelationHandle,
                                              aMyTransId,
                                              aMyViewScn,
                                              aMyTcn,
                                              sContColPid,
                                              STL_TRUE,
                                              aEnv ) == STL_SUCCESS );
            }

            /**
             * continuous column을 저장한다.
             */

            sIsZero = STL_FALSE;
            sDataValue = KNL_GET_BLOCK_DATA_VALUE(sCurCol, aValueIdx);

            if( sDataValue->mLength > sThreshold )
            {
                STL_TRY( smdmpfInsertContCol( aMyTransId,
                                              aMyTcn,
                                              aRelationHandle,
                                              aValueIdx,
                                              sCurCol,
                                              STL_TRUE, /* aIsForUpdate */
                                              &sContColPid,
                                              aEnv ) == STL_SUCCESS );

                sColLen = sThreshold;
                SMP_GET_CONT_COLUMN_LEN_SIZE( &sColLenSize );

                SMP_WRITE_CONT_COLUMN( sColPtr,
                                       sDataValue->mValue,
                                       sContColPid,
                                       sColLen );

                sUsedContCols = STL_TRUE;
            }
            else
            {
                if( DTL_NUMERIC_IS_ZERO(sDataValue->mValue, sDataValue->mLength) )
                {
                    if( dtlDataTypeGroup[sDataValue->mType] == DTL_GROUP_NUMBER )
                    {
                        sIsZero = STL_TRUE;
                    }
                }

                /* write column */
                SMP_WRITE_COLUMN( sColPtr, sDataValue->mValue, sDataValue->mLength, sIsZero );
                sColLen = sDataValue->mLength;
                SMP_GET_COLUMN_LEN_SIZE( sColLen, &sColLenSize );
            }

            sCurCol = sCurCol->mNext;

            /* REDO log 기록 */
            sColOrdIdx = (stlUInt8)(sColOrder - aFirstColOrd);
            STL_WRITE_INT8( sLogPtr, &sColOrdIdx );
            sLogPtr += STL_SIZEOF(stlInt8);
            stlMemcpy( sLogPtr, sColPtr, sColLen + sColLenSize);
            sLogPtr += sColLen + sColLenSize;

            sUpdateColCnt--;
        }
    }

    /* write undo record */
    STL_TRY( smxlInsertUndoRecord( aMiniTrans,
                                   SMG_COMPONENT_TABLE,
                                   SMD_UNDO_LOG_MEMORY_HEAP_UPDATE,
                                   aUndoLog,
                                   aUndoLogSize,
                                   *aRowRid,
                                   &sUndoRid,
                                   aEnv ) == STL_SUCCESS );

    sLogEnd = sLogPtr;

    sScn = SML_MAKE_VIEW_SCN(aMyViewScn);
    SMDMPF_UNSET_RTS_BOUND( aRowSlot );
    SMDMPF_SET_SCN( aRowSlot, &sScn );

    SMDMPF_SET_TCN( aRowSlot, &aMyTcn );
    SMDMPF_SET_TRANS_ID( aRowSlot, &aMyTransId );
    SMDMPF_SET_ROLLBACK_PID( aRowSlot, &sUndoRid.mPageId );
    SMDMPF_SET_ROLLBACK_OFFSET( aRowSlot, &sUndoRid.mOffset );

    /* Write REDO log */
    sLogPtr = sLogBuf;
    STL_WRITE_INT64( sLogPtr, &aMyTransId );
    sLogPtr += STL_SIZEOF(smxlTransId);
    STL_WRITE_INT64( sLogPtr, &aMyViewScn );
    sLogPtr += STL_SIZEOF(smlScn);
    stlMemcpy( sLogPtr, aRowSlot, SMDMPF_ROW_HDR_LEN );
    sLogPtr += SMDMPF_ROW_HDR_LEN;
    STL_WRITE_INT32( sLogPtr, &aFirstColOrd );
    sLogPtr += STL_SIZEOF(stlInt32);
    STL_WRITE_INT16( sLogPtr, &aUpdateColCnt );
    sLogPtr += STL_SIZEOF(stlInt16);

    if( sUsedContCols == STL_TRUE )
    {
        sLogicalHdr = SMDMPF_GET_LOGICAL_HEADER( SMP_FRAME(aPageHandle) );

        if( sLogicalHdr->mSimplePage == STL_TRUE )
        {
            sLogicalHdr->mSimplePage = STL_FALSE;
            STL_TRY( smxmWriteLog( aMiniTrans,
                                   SMG_COMPONENT_TABLE,
                                   SMR_LOG_MEMORY_HEAP_UPDATE_LOGICAL_HDR,
                                   SMR_REDO_TARGET_PAGE,
                                   (stlChar*)sLogicalHdr,
                                   STL_SIZEOF(smdmpfLogicalHdr),
                                   smpGetTbsId(aPageHandle),
                                   smpGetPageId(aPageHandle),
                                   0,
                                   SMXM_LOGGING_REDO,
                                   aEnv ) == STL_SUCCESS );
        }
    }

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_TABLE,
                           SMR_LOG_MEMORY_HEAP_UPDATE,
                           SMR_REDO_TARGET_PAGE,
                           sLogBuf,
                           sLogEnd - sLogBuf,
                           aRowRid->mTbsId,
                           aRowRid->mPageId,
                           aRowRid->mOffset,  /* Physiological redo를 하기위해 offset이 아닌 seq 사용 */
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    smdmpfValidateTablePage( aPageHandle, aEnv );

    return STL_SUCCESS ;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdmpfFindNextColRid( smlStatement * aStatement,
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
        sIsContColRow = SMDMPF_IS_CONT_COL( sRowSlot );

        if( SMDMPF_HAS_NEXT_LINK( sRowSlot ) == STL_TRUE )
        {
            SMDMPF_GET_LINK_PID( sRowSlot, &sNextRid.mPageId );
            SMDMPF_GET_LINK_OFFSET( sRowSlot, &sNextRid.mOffset );
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

stlStatus smdmpfFindLastPiece( smlRid     aStartRid,
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
        SMDMPF_GET_COLUMN_CNT( sRowSlot, &sColumnCount );

        if( SMDMPF_IS_CONT_COL( sRowSlot ) == STL_TRUE )
        {
            if( SMDMPF_IS_CONT_COL_LAST( sRowSlot ) == STL_TRUE )
            {
                *aLastColOrd += sColumnCount;
            }
        }
        else
        {
            *aLastColOrd += sColumnCount;
        }
        
        if( SMDMPF_HAS_NEXT_LINK( sRowSlot ) == STL_TRUE )
        {
            
            sNextRid.mTbsId = sCurRid.mTbsId;
            SMDMPF_GET_LINK_PID( sRowSlot, &sNextRid.mPageId );
            SMDMPF_GET_LINK_OFFSET( sRowSlot, &sNextRid.mOffset );
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
 * @param[out] aBeforeCols  변경될 Record의 이전 값을 저장하기 위한 버퍼
 * @param[in] aPrimaryKey 해당 레코드의 primary key
 * @param[out] aVersionConflict update 도중 version conflict를 발견했는지 여부
 * @param[out] aSkipped 이미 갱신된 Row인지 여부
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmpfUpdate( smlStatement      * aStatement,
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
    stlChar *           sSrcRowHdr;
    stlInt16            sPieceColCnt;
    stlInt16            sUpdateColCnt;
    stlChar           * sUpdateColCntPtr;
    stlUInt32           sState = 0;
    smxmTrans           sMiniTrans;
    smpHandle           sPageHandle;
    stlBool             sIsColSizeDiff;
    stlChar             sLogBuf[SMP_PAGE_SIZE];
    stlChar           * sLogPtr;
    stlChar           * sLogEnd = NULL;
    smxlTransId         sTransId;
    smlScn              sViewScn;
    smlTcn              sRowTcn = 0;
    smlTcn              sStmtTcn;
    smlScn              sCommitScn;
    smxlTransId         sWaitTransId;
    stlBool             sHasNextPiece = STL_TRUE;
    smlRid              sRowRid = *aRowRid;
    stlInt32            sFirstColOrd = 0;
    smlRid              sNextRid = ((smlRid){sTbsId,0,SMP_NULL_PID});
    stlBool             sIsContColRow;
    stlInt16            i;
    stlBool             sIsMyTrans = STL_FALSE;
    stlBool             sIsMyLock = STL_FALSE;
    void              * sLockItem = NULL;
    stlBool             sIsFirstPiece = STL_TRUE;
    stlBool             sSupplLogged = STL_FALSE;
    stlInt32            sInterErrPos = 0;

    STL_DASSERT( aRowScn != 0 );

    if( aPrimaryKey == NULL )
    {
        sBeforeCol = NULL;
    }
    else
    {
        sBeforeCol = aBeforeCols;
    }
    
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

        sSrcRowHdr = SMDMPF_GET_NTH_ROW_HDR(SMP_FRAME(&sPageHandle), sRowRid.mOffset);
        
        STL_TRY( smdmpfGetCommitScn( sTbsId,
                                     &sPageHandle,
                                     sSrcRowHdr,
                                     &sCommitScn,
                                     &sWaitTransId,
                                     aEnv ) == STL_SUCCESS );

        sIsMyTrans = SMDMPF_IS_SAME_TRANS( sTransId,
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
                    if( SMDMPF_IS_LOCKED( sSrcRowHdr ) == STL_FALSE )
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
                        SMDMPF_UNSET_LOCKED( sSrcRowHdr );
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

        SMDMPF_GET_TCN( sSrcRowHdr, &sRowTcn);
        
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

        if( SMDMPF_IS_DELETED( sSrcRowHdr ) == STL_TRUE )
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
        sNextRid.mPageId = SMDMPF_GET_NEXT_CHAINED_PAGE_ID(SMP_FRAME(&sPageHandle));

        if( sNextRid.mPageId != SMP_NULL_PID )
        {
            sHasNextPiece = STL_TRUE;
            sNextRid.mOffset = sRowRid.mOffset;
        }
        else
        {
            sHasNextPiece = STL_FALSE;
            sNextRid.mOffset = 0;
        }

        /* continuous column 외 공통 : row piece 분석*/
        SMDMPF_GET_COLUMN_CNT( sSrcRowHdr, &sPieceColCnt );

        sIsColSizeDiff = STL_FALSE;
        sUpdateColCnt = 0;

        sLogPtr = sLogBuf;
        stlMemcpy( sLogPtr, sSrcRowHdr, SMDMPF_ROW_HDR_LEN );

        sLogPtr += SMDMPF_ROW_HDR_LEN;
        /* undo log에서 first update col ord와 update col cnt 만큼 건너뛰고 컬럼먼저 기술 */
        STL_WRITE_INT32(sLogPtr, &sFirstColOrd);
        sLogPtr += STL_SIZEOF(stlInt32);
        sUpdateColCntPtr = sLogPtr;
        sLogPtr += STL_SIZEOF(stlInt16);

        STL_TRY( smdmpfMakeUndoLog( aRelationHandle,
                                    &sPageHandle,
                                    sRowRid.mOffset,
                                    sPieceColCnt,
                                    sFirstColOrd,
                                    sCurCol,
                                    sBeforeCol,
                                    aValueIdx,
                                    sLogPtr,
                                    &sLogEnd,
                                    &sIsContColRow,
                                    &sUpdateColCnt ) == STL_SUCCESS );

        /* undo log의 update column count를 채움 */
        STL_WRITE_INT16( sUpdateColCntPtr, &sUpdateColCnt );

        if( sUpdateColCnt == 0 )
        {
            if( SMDMPF_IS_MASTER_ROW( sSrcRowHdr ) != STL_TRUE )
            {
                sState = 0;
                STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

                sFirstColOrd += sPieceColCnt;

                if( sHasNextPiece == STL_TRUE )
                {
                    sNextRid.mTbsId = sRowRid.mTbsId;
                    STL_DASSERT( sNextRid.mPageId != SMP_NULL_PID );
                }
                
                if( sCurCol != NULL )
                {
                    if( sHasNextPiece == STL_FALSE )
                    {
                        /**
                         * Trailing null
                         */

                        /* STL_TRY( smdmpfAppendColumns( aStatement, */
                        /*                               aRelationHandle, */
                        /*                               sFirstColOrd, */
                        /*                               sCurCol, */
                        /*                               sPrevRid, */
                        /*                               sRowRid, */
                        /*                               aValueIdx, */
                        /*                               aEnv ) */
                        /*          == STL_SUCCESS ); */
                        break;
                    }
                }
                else
                {
                    STL_DASSERT( STL_FALSE );
                    break;
                }

                sRowRid = sNextRid;

                continue;
            }
            else
            {
                if( sIsContColRow == STL_TRUE )
                {
                    /**
                     * MASTER 이면서 Continuous column이라면 Continuous slave piece 검사시
                     * 증가 시키기 때문에 지금은 하나 감소시킨다.
                     */
                    sFirstColOrd -= sPieceColCnt;
                }

                /**
                 * Dummy update를 위해서 일단 flag를 off한다.
                 */
                sIsContColRow = STL_FALSE;
            }
        }

        /**
         * Continuous column check
         */

        /* 현재 페이지에서 해결 가능한 경우 */
        STL_TRY( smdmpfUpdateInPage( aRelationHandle,
                                     &sMiniTrans,
                                     &sPageHandle,
                                     sLogBuf,
                                     sLogEnd - sLogBuf,
                                     &sRowRid,
                                     sSrcRowHdr,
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

        sState = 0;
        STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

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
            if( sHasNextPiece == STL_FALSE )
            {
                /**
                 * Trailing null
                 */

                /* STL_TRY( smdmpfAppendColumns( aStatement, */
                /*                               aRelationHandle, */
                /*                               sFirstColOrd, */
                /*                               sCurCol, */
                /*                               sPrevRid, */
                /*                               sRowRid, */
                /*                               aValueIdx, */
                /*                               aEnv ) */
                /*          == STL_SUCCESS ); */
                break;
            }
        }
        else
        {
            break;
        }

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
                      "smdmpfUpdate()",
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
stlStatus smdmpfDelete( smlStatement      * aStatement,
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

        sSrcRowSlot = SMDMPF_GET_NTH_ROW_HDR( SMP_FRAME(&sPageHandle), sRowRid.mOffset );

        STL_TRY( smdmpfGetCommitScn( sTbsId,
                                     &sPageHandle,
                                     sSrcRowSlot,
                                     &sCommitScn,
                                     &sWaitTransId,
                                     aEnv ) == STL_SUCCESS );

        /* My Transaction은 통과 */
        sIsMyTrans = SMDMPF_IS_SAME_TRANS( sTransId,
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
                    if( SMDMPF_IS_LOCKED( sSrcRowSlot ) == STL_FALSE )
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
                        SMDMPF_UNSET_LOCKED( sSrcRowSlot );
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

        SMDMPF_GET_TCN( sSrcRowSlot, &sRowTcn);
        
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
        if( SMDMPF_IS_DELETED( sSrcRowSlot ) == STL_TRUE )
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
        sNextPieceRid.mPageId = SMDMPF_GET_NEXT_CHAINED_PAGE_ID(SMP_FRAME(&sPageHandle));

        if( sNextPieceRid.mPageId != SMP_NULL_PID )
        {
            sNextPieceRid.mTbsId = sRowRid.mTbsId;
            sNextPieceRid.mOffset = sRowRid.mOffset;
        }
        else
        {
            sNextPieceRid.mTbsId = sRowRid.mTbsId;
            sNextPieceRid.mPageId = SMP_NULL_PID;
            sNextPieceRid.mOffset = 0;
        }

        /* row piece에 delete mark를 한다 */
        STL_TRY( smdmpfDeleteRowPiece( &sMiniTrans,
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
                      "smdmpfDelete()",
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
stlStatus smdmpfDeleteRowPiece( smxmTrans         * aMiniTrans,
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
    void             * sSegmentHandle = SME_GET_SEGMENT_HANDLE(aRelationHandle);
    stlChar            sLogBuf[SMP_PAGE_SIZE];
    stlChar          * sLogPtr;
    smlRid             sUndoRid;
    stlChar          * sRowSlot;
    smlScn             sScn = 0;
    stlInt16           sLogType = (aIsForUpdate == STL_TRUE) ?
        SMR_LOG_MEMORY_HEAP_DELETE_FOR_UPDATE : SMR_LOG_MEMORY_HEAP_DELETE;

    sRowSlot = SMDMPF_GET_NTH_ROW_HDR( SMP_FRAME(aPageHandle), aRowRid->mOffset );

    /* Undo Log 부터 작성 */
    sLogPtr  = sLogBuf;
    stlMemcpy( sLogPtr, sRowSlot, SMDMPF_ROW_HDR_LEN );
    /* Undo log의 row header는 가능한 경우(commit된 다른 trans의 row) stamping 한다 */
    sLogPtr += SMDMPF_ROW_HDR_LEN;

    /* write undo record */
    STL_TRY( smxlInsertUndoRecord( aMiniTrans,
                                   SMG_COMPONENT_TABLE,
                                   SMD_UNDO_LOG_MEMORY_HEAP_DELETE,
                                   sLogBuf,
                                   sLogPtr - sLogBuf,
                                   *aRowRid,
                                   &sUndoRid,
                                   aEnv ) == STL_SUCCESS );

    smpSetMaxViewScn( aPageHandle, aMyViewScn );

    if( SMXL_IS_UNDO_BOUND_TRANS( aMyTransId ) == STL_TRUE )
    {
        sScn = SML_MAKE_VIEW_SCN( aMyViewScn );
    }

    SMDMPF_UNSET_RTS_BOUND( sRowSlot );
    SMDMPF_SET_SCN( sRowSlot, &sScn );

    /* Set Delete bit & trans info */
    SMDMPF_SET_DELETED( sRowSlot );
    SMDMPF_SET_TCN( sRowSlot, &aMyTcn );
    SMDMPF_SET_TRANS_ID( sRowSlot, &aMyTransId );
    SMDMPF_SET_ROLLBACK_PID( sRowSlot, &sUndoRid.mPageId );
    SMDMPF_SET_ROLLBACK_OFFSET( sRowSlot, &sUndoRid.mOffset );

    /* Write REDO log */
    sLogPtr = sLogBuf;
    STL_WRITE_INT64( sLogPtr, &aMyTransId );
    sLogPtr += STL_SIZEOF(smxlTransId);
    STL_WRITE_INT64( sLogPtr, &aMyViewScn );
    sLogPtr += STL_SIZEOF(smlScn);
    stlMemcpy( sLogPtr, sRowSlot, SMDMPF_ROW_HDR_LEN );
    sLogPtr += SMDMPF_ROW_HDR_LEN;

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

    SMDMPF_DECREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) );

    if( SMDMPF_GET_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) ) == 0 )
    {
        STL_TRY( smdmpfUpdatePageStatus( aMiniTrans,
                                         aRelationHandle,
                                         sSegmentHandle,
                                         aPageHandle,
                                         SMP_FREENESS_FREE,
                                         smxlGetSystemScn(),
                                         aEnv ) == STL_SUCCESS );
    }
    else
    {
        if( smpGetFreeness(aPageHandle) == SMP_FREENESS_UPDATEONLY )
        {
            STL_TRY( smdmpfUpdatePageStatus( aMiniTrans,
                                             aRelationHandle,
                                             sSegmentHandle,
                                             aPageHandle,
                                             SMP_FREENESS_INSERTABLE,
                                             0,  /* aScn */
                                             aEnv ) == STL_SUCCESS );
        }
    }

    smdmpfValidateTablePage( aPageHandle, aEnv );

    STL_DASSERT( smpGetSegmentId( aPageHandle ) ==
                 smsGetSegmentId( sSegmentHandle ) );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdmpfDeleteContCol( void         * aRelationHandle,
                               smxlTransId    aMyTransId,
                               smlScn         aMyViewScn,
                               smlTcn         aMyTcn,
                               smlPid         aContColPid,
                               stlBool        aIsForUpdate,
                               smlEnv       * aEnv )
{
    stlUInt32          sAttr = SMXM_ATTR_REDO;
    void             * sSegmentHandle = SME_GET_SEGMENT_HANDLE(aRelationHandle);
    stlChar            sLogBuf[SMP_PAGE_SIZE];
    stlChar          * sLogPtr;
    stlUInt32          sState = 0;
    smxmTrans          sMiniTrans;
    smpHandle          sPageHandle;
    smlTbsId           sTbsId = smsGetTbsId(sSegmentHandle);
    stlChar          * sRowSlot;
    stlChar          * sContColPtr;
    smxlTransId        sWaitTransId;
    smlScn             sCommitScn;
    smlPid             sContColPid = aContColPid;
    smlRid             sUndoRid;
    smlScn             sScn = 0;
    stlInt16           sLogType = (aIsForUpdate == STL_TRUE) ?
        SMR_LOG_MEMORY_HEAP_DELETE_FOR_UPDATE : SMR_LOG_MEMORY_HEAP_DELETE;

    while( sContColPid != SMP_NULL_PID )
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
                             sContColPid,
                             KNL_LATCH_MODE_EXCLUSIVE,
                             &sPageHandle,
                             aEnv ) == STL_SUCCESS );

        sRowSlot = (stlChar *)SMDMPF_GET_START_POS( SMP_FRAME(&sPageHandle) );
        sContColPtr = sRowSlot + SMDMPF_ROW_HDR_LEN;

        SMP_READ_CONT_COLUMN_PID( sContColPtr, &sContColPid );

        STL_TRY( smdmpfGetCommitScn( sTbsId,
                                     &sPageHandle,
                                     sRowSlot,
                                     &sCommitScn,
                                     &sWaitTransId,
                                     aEnv ) == STL_SUCCESS );

        /* row piece에 delete mark를 한다 */

        /* Undo Log 부터 작성 */
        sLogPtr  = sLogBuf;
        stlMemcpy( sLogPtr, sRowSlot, SMDMPF_ROW_HDR_LEN );
        sLogPtr += SMDMPF_ROW_HDR_LEN;

        /* write undo record */
        STL_TRY( smxlInsertUndoRecord( &sMiniTrans,
                                       SMG_COMPONENT_TABLE,
                                       SMD_UNDO_LOG_MEMORY_HEAP_DELETE,
                                       sLogBuf,
                                       sLogPtr - sLogBuf,
                                       ((smlRid){ sPageHandle.mPch->mTbsId,
                                               0,
                                               sPageHandle.mPch->mPageId }),
                                       &sUndoRid,
                                       aEnv ) == STL_SUCCESS );

        smpSetMaxViewScn( &sPageHandle, aMyViewScn );

        if( SMXL_IS_UNDO_BOUND_TRANS( aMyTransId ) == STL_TRUE )
        {
            sScn = SML_MAKE_VIEW_SCN( aMyViewScn );
        }

        SMDMPF_UNSET_RTS_BOUND( sRowSlot );
        SMDMPF_SET_SCN( sRowSlot, &sScn );

        /* Set Delete bit & trans info */
        SMDMPF_SET_DELETED( sRowSlot );
        SMDMPF_SET_TCN( sRowSlot, &aMyTcn );
        SMDMPF_SET_TRANS_ID( sRowSlot, &aMyTransId );
        SMDMPF_SET_ROLLBACK_PID( sRowSlot, &sUndoRid.mPageId );
        SMDMPF_SET_ROLLBACK_OFFSET( sRowSlot, &sUndoRid.mOffset );

        /* Write REDO log */
        sLogPtr = sLogBuf;
        STL_WRITE_INT64( sLogPtr, &aMyTransId );
        sLogPtr += STL_SIZEOF(smxlTransId);
        STL_WRITE_INT64( sLogPtr, &aMyViewScn );
        sLogPtr += STL_SIZEOF(smlScn);
        stlMemcpy( sLogPtr, sRowSlot, SMDMPF_ROW_HDR_LEN );
        sLogPtr += SMDMPF_ROW_HDR_LEN;

        STL_TRY( smxmWriteLog( &sMiniTrans,
                               SMG_COMPONENT_TABLE,
                               sLogType,
                               SMR_REDO_TARGET_PAGE,
                               sLogBuf,
                               sLogPtr - sLogBuf,
                               sPageHandle.mPch->mTbsId,
                               sPageHandle.mPch->mPageId,
                               0,  /* Physiological redo를 하기위해 offset이 아닌 seq 사용 */
                               SMXM_LOGGING_REDO,
                               aEnv ) == STL_SUCCESS );

        sState = 0;
        STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
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
stlStatus smdmpfFetchRecord( smlStatement        * aStatement,
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
    STL_TRY( smdmpfFetch( aStatement,
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


stlStatus smdmpfFetchRecord4Index( smiIterator         * aIterator,
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
    stlChar    * sPageBody;

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

    sPageBody = SMDMPF_GET_PAGE_BODY( SMP_FRAME(aTablePageHandle) );

    sRow = sPageBody + aRowRid->mOffset * SMDMPF_ROW_HDR_LEN;

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
        smdmpfIsSimplePage(aTablePageHandle) )
    {
        STL_TRY( smdmpfGetValidVersionSimplePage( aBaseRelHandle,
                                                  aRowRid,
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
        STL_TRY( smdmpfGetValidVersion( aBaseRelHandle,
                                        aRowRid,
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


stlStatus smdmpfFetch( smlStatement        * aStatement,
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
    
    sRow = SMDMPF_GET_NTH_ROW_HDR( SMP_FRAME(&sPageHandle), aRowRid->mOffset );

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
    
    STL_TRY( smdmpfGetValidVersion( aFetchRecordInfo->mRelationHandle,
                                    aRowRid,
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


stlStatus smdmpfFetchWithRowid( smlStatement        * aStatement,
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
    smdmpfLogicalHdr * sLogicalHdr;

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
    sLogicalHdr = (smdmpfLogicalHdr*)smpGetLogicalHeader( &sPageHandle );
    
    STL_TRY_THROW( sPhyHdr->mPageType == SMP_PAGE_TYPE_TABLE_COLUMNAR_DATA,
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

    if( aRowRid->mOffset >= SMDMPF_GET_SLOT_COUNT( SMP_FRAME(&sPageHandle) ) )
    {
        *aIsMatched = STL_FALSE;
        STL_THROW( RAMP_SKIP_FETCH );
    }

    if( SMDMPF_IS_NTH_ROW_HDR_FREE(SMP_FRAME(&sPageHandle), aRowRid->mOffset) == STL_TRUE )
    {
        *aIsMatched = STL_FALSE;
        STL_THROW( RAMP_SKIP_FETCH );
    }

    sRow = SMDMPF_GET_NTH_ROW_HDR( SMP_FRAME(&sPageHandle), aRowRid->mOffset );

    /**
     * slave row piece는 rowid로 fetch할수 없다.
     */
    if( SMDMPF_IS_MASTER_ROW( sRow ) == STL_FALSE )
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
    
    STL_TRY( smdmpfGetValidVersion( aFetchRecordInfo->mRelationHandle,
                                    aRowRid,
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


stlStatus smdmpfExtractValidKeyValue( void              * aRelationHandle,
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

    sRow = SMDMPF_GET_NTH_ROW_HDR( SMP_FRAME(&sPageHandle), aRowRid->mOffset );

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
    
    STL_TRY( smdmpfGetValidVersion( aRelationHandle,
                                    aRowRid,
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
stlStatus smdmpfLockRow( smlStatement * aStatement,
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

        sSrcRowSlot = SMDMPF_GET_NTH_ROW_HDR( SMP_FRAME(&sPageHandle), aRowRid->mOffset );

        STL_TRY( smdmpfGetCommitScn( sTbsId,
                                     &sPageHandle,
                                     sSrcRowSlot,
                                     &sCommitScn,
                                     &sWaitTransId,
                                     aEnv ) == STL_SUCCESS );

        /**
         * My Transaction은 통과
         */
        if( SMDMPF_IS_SAME_TRANS( sTransId,
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
            if( SMDMPF_IS_LOCKED( sSrcRowSlot ) == STL_FALSE )
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
                SMDMPF_UNSET_LOCKED( sSrcRowSlot );
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
    if( (SMDMPF_IS_SAME_TRANS( sTransId,
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
        
        SMDMPF_SET_LOCKED( sSrcRowSlot );
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
 * @param[out] aColCount log에 기록된 row의 총 column 개수
 * @param[out] aFirstColOrd  log에 기록된 row piece의 첫 column의 순번
 * @param[out] aRowPieceSize  log에 기록된 row piece의 크기
 * @param[out] aColumns log에 기록된 row의 모든 컬럼 정보
 * @param[out] aContColPid Continuous column의 next pid
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmpfAnalyzeUpdateLogRec( stlChar            * aLogRec,
                                     knlRegionMark      * aMemMark,
                                     stlInt16           * aColCount,
                                     stlInt32           * aFirstColOrd,
                                     stlInt16           * aRowPieceSize,
                                     knlValueBlockList ** aColumns,
                                     smlPid             * aContColPid,
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
    stlInt16            sColLen;
    stlChar           * sBuf;
    stlUInt16           sSlotHdrSize;
    stlBool             sIsZero;
    stlBool             sIsContCol;
    stlInt32            sState = 0;
    stlInt16            sRowPieceSize = 0;

    sSlotHdrSize = SMDMPF_ROW_HDR_LEN;
    /* SMDMPF_GET_ROW_HDR_SIZE( sLogPtr, &sSlotHdrSize ); */
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

        SMP_GET_FIXED_COLUMN_LEN( sLogPtr,
                                  &sColLenSize,
                                  &sColLen,
                                  sIsContCol,
                                  sIsZero );

        aContColPid[sColIdx] = SMP_NULL_PID;

        if( sIsContCol == STL_TRUE )
        {
            SMP_READ_CONT_COLUMN_PID( sLogPtr, &aContColPid[sColIdx] );
        }

        sLogPtr += sColLenSize;
        sRowPieceSize += (sColLen + sColLenSize);

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

    *aColumns = sFirstCol;
    *aRowPieceSize = sRowPieceSize;

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
 * @brief 주어진 row의 undo log record를 기록한다
 * @param[in] aRelationHandle 갱신할 row의 relation의 핸들
 * @param[in] aPageHandle 갱신할 row piece가 저장된 data page handle
 * @param[in] aSlotSeq 갱신할 row 가 저장된 slot의 sequence
 * @param[in] aTotalColCount 갱신할 row piece의 총 컬럼 개수
 * @param[in] aFirstColOrd 갱신할 row piece의 첫 컬럼의 order
 * @param[in] aUpdateCols 갱신할 컬럼들의 정보
 * @param[in] aBeforeCols 갱신할 컬럼들의 이전 정보를 기록할 버퍼 공간
 * @param[in] aBlockIdx 갱신할 컬럼들 block Idx
 * @param[out] aUndoLogPtr 조사를 하면서 undo log를 기록할 버퍼의 시작 주소
 * @param[out] aUndoLogEnd 기록한 undo log의 끝 주소
 * @param[out] aExistContCol Continuous column 존재 여부
 * @param[out] aUpdateColCount 갱신될 컬럼의 개수
 */
stlStatus smdmpfMakeUndoLog( void               * aRelationHandle,
                             smpHandle          * aPageHandle,
                             stlInt16             aSlotSeq,
                             stlInt16             aTotalColCount,
                             stlInt32             aFirstColOrd,
                             knlValueBlockList  * aUpdateCols,
                             knlValueBlockList  * aBeforeCols,
                             stlInt32             aBlockIdx,
                             stlChar            * aUndoLogPtr,
                             stlChar           ** aUndoLogEnd,
                             stlBool            * aExistContCol,
                             stlInt16           * aUpdateColCount )
{
    stlUInt8            sColIdx;
    stlUInt8            sColLenSize;
    stlInt64            sTotalColSize;
    stlInt64            sColLen;
    stlChar           * sColPtr;
    knlValueBlockList * sCurCol = aUpdateCols;
    knlValueBlockList * sBeforeCol = aBeforeCols;
    stlChar           * sLogPtr = aUndoLogPtr;
    stlInt32            sColOrder;
    dtlDataValue      * sBeforeValue;
    stlBool             sIsZero;

    STL_DASSERT( aTotalColCount <= SMD_MAX_COLUMN_COUNT_IN_PIECE );
    
    *aUpdateColCount = 0;
    *aExistContCol   = STL_FALSE;

    while( sCurCol != NULL )
    {
        sColOrder = KNL_GET_BLOCK_COLUMN_ORDER(sCurCol);
        
        sColPtr = smdmpfGetNthColValue( aPageHandle,
                                        aSlotSeq,
                                        sColOrder );

        if( sColPtr == NULL )
        {
            break;
        }

        /**
         * Continuous column여부를 포함한 column length를 구함
         */
        
        SMP_GET_COLUMN_LEN( sColPtr, &sColLenSize, &sColLen, sIsZero );
        sTotalColSize = sColLenSize + sColLen;
        
        SMP_GET_TOTAL_COLUMN_SIZE( sColPtr, sTotalColSize );
        
        if( SMP_IS_CONT_COLUMN(sColPtr) == STL_TRUE )
        {
            if( *aExistContCol == STL_FALSE )
            {
                *aExistContCol = STL_TRUE;
            }
        }

        if( aUndoLogPtr != NULL )
        {
            /* Undo log에 컬럼 정보 기록( column seq, length, value ) */
            sColIdx = (stlUInt8)(sColOrder - aFirstColOrd);
            STL_WRITE_INT8( sLogPtr, &sColIdx );
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
stlStatus smdmpfCompareKeyValue( smlRid              * aRowRid,
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
    if( SMDMPF_GET_SLOT_COUNT(SMP_FRAME(aPageHandle)) >= aRowRid->mOffset )
    {
        if( SMDMPF_IS_NTH_ROW_HDR_FREE(SMP_FRAME(aPageHandle), aRowRid->mOffset) == STL_FALSE )
        {
            sCurRow = SMDMPF_GET_NTH_ROW_HDR( SMP_FRAME(aPageHandle), aRowRid->mOffset );
            if( SMDMPF_IS_MASTER_ROW( sCurRow ) == STL_TRUE )
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
                
                STL_TRY( smdmpfGetValidVersionAndCompare( &sRowInfo,
                                                          aRowRid->mOffset,
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
stlStatus smdmpfExtractKeyValue( smpHandle          * aPageHandle,
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
    stlChar           * sRowHdr;
    smlTbsId            sTbsId = smpGetTbsId( aPageHandle );
    smpHandle           sPageHandle;
    stlInt32            sState = 0;
    stlInt32            sKeyValueSize;
    stlInt32            sBlockIdx = 0;
    smdValueBlock     * sValueBlock;
    stlBool             sIsZero;
    stlBool             sIsContCol;
    stlChar           * sColPtr;
    stlInt32            sColOrder;
    smlPid              sChainedPageId;

    /**
     * Value Block List는 초기화 된 상태임을 보장해야 한다.
     */
    STL_DASSERT( aColumnList->mValueBlock->mUsedBlockCnt == 0 );
    STL_DASSERT( aColumnList->mValueBlock->mSkipBlockCnt == 0 );
    
    *aIsSuccess = STL_FALSE;

    STL_TRY_THROW( SMDMPF_GET_MEMBER_PAGE_TYPE(SMP_FRAME(aPageHandle)) == SMDMPF_MEMBER_PAGE_TYPE_MASTER,
                   RAMP_SKIP_EXTRACT );

    if( aFetchFirst == STL_TRUE )
    {
        /* init slot iterator */
        SMDMPF_INIT_ROWITEM_ITERATOR( SMP_FRAME(aPageHandle), aRowIterator );
    }

    SMDMPF_FOREACH_ROWITEM_ITERATOR( aRowIterator )
    {
        sRowHdr = (stlChar*)aRowIterator->mCurSlot;

        if( SMDMPF_IS_IN_USE( sRowHdr ) != STL_TRUE )
        {
            continue;
        }

        if( SMDMPF_IS_DELETED( sRowHdr ) == STL_TRUE )
        {
            continue;
        }

        sCurCol = aColumnList;

        sKeyValueSize = 0;

        sPageHandle = *aPageHandle;

        while( sCurCol != NULL )
        {
            sColOrder = KNL_GET_BLOCK_COLUMN_ORDER(sCurCol);

            sColPtr = smdmpfGetNthColValue( &sPageHandle,
                                            aRowIterator->mCurSeq,
                                            sColOrder );

            if( sColPtr == NULL )
            {
                /**
                 * @todo next chained page에서 key를 추출해야 함
                 */
                sChainedPageId = SMDMPF_GET_NEXT_CHAINED_PAGE_ID(SMP_FRAME(&sPageHandle));

                STL_DASSERT( sChainedPageId != SMP_NULL_PID );

                if( sState == 1 )
                {
                    sState = 0;
                    STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
                }

                STL_TRY( smpFix( sTbsId,
                                 sChainedPageId,
                                 &sPageHandle,
                                 aEnv ) == STL_SUCCESS );
                sState = 1;

                continue;
            }

            SMP_GET_FIXED_COLUMN_LEN( sColPtr,
                                      &sColLenSize,
                                      &sColLen,
                                      sIsContCol,
                                      sIsZero);

            /**
             * Continuous column은 key size 제한으로 인하여 index key로 사용될수 없다.
             * @todo Continuous column 처리
             */
                    
            STL_TRY_THROW( sIsContCol == STL_FALSE, RAMP_ERR_TOO_LARGE_KEY );
                    
            sValueBlock = sCurCol->mValueBlock;

            /* Column에 null value가 포함되어 있음 */
            if( (sIsZero == STL_FALSE) && (sColLen == 0) )
            {
                *aHasNullInBlock = STL_TRUE;
            }

            /**
             * Chained Row Piece는 Key Value를 복사해야 Unfix가 가능하다.
             */

            STL_DASSERT( sValueBlock->mKeyValue[sBlockIdx].mBufferSize >= sColLen );

            if( sState == 1 )
            {
                if( sIsZero == STL_FALSE )
                {
                    stlMemcpy( sValueBlock->mKeyValue[sBlockIdx].mValue,
                               sColPtr + sColLenSize,
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
                    sValueBlock->mColumnInRow[sBlockIdx].mValue  = sColPtr + sColLenSize;
                    sValueBlock->mColumnInRow[sBlockIdx].mLength = sColLen;
                    sKeyValueSize += (sColLen + sColLenSize);
                }
            }

            sValueBlock->mUsedBlockCnt++;
            sCurCol = sCurCol->mNext;
        }

        if( sState == 1 )
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

    STL_RAMP( RAMP_SKIP_EXTRACT );

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
stlStatus smdmpfUpdatePageStatus( smxmTrans   * aMiniTrans,
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

stlStatus smdmpfIsAgable( smpHandle * aPageHandle,
                          stlBool   * aIsAgable,
                          smlEnv    * aEnv )
{
    smpRowItemIterator   sIterator;
    stlChar            * sSrcRow;
    smlScn               sCommitScn;
    smlScn               sScn;
    smxlTransId          sTransId = SML_INVALID_TRANSID;
    smlScn               sAgableScn;

    *aIsAgable = STL_TRUE;
    
    if( SMDMPF_GET_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) ) > 0 )
    {
        *aIsAgable = STL_FALSE;
        STL_THROW( RAMP_FINISH );
    }
    
    sAgableScn = smxlGetAgableTbsScn( smpGetTbsId(aPageHandle), aEnv );

    SMDMPF_INIT_ROWITEM_ITERATOR( SMP_FRAME(aPageHandle), &sIterator );
    
    SMDMPF_FOREACH_ROWITEM_ITERATOR( &sIterator )
    {
        if( SMDMPF_IS_IN_USE( sIterator.mCurSlot ) != STL_TRUE )
        {
            continue;
        }
        
        sSrcRow = (stlChar*)sIterator.mCurSlot;

        SMDMPF_GET_TRANS_ID( sSrcRow, &sTransId );
        SMDMPF_GET_SCN( sSrcRow, &sScn );

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
inline void * smdmpfGetSlotBody( void * aSlot )
{
    stlUInt16 sSlotHdrSize;

    sSlotHdrSize = SMDMPF_ROW_HDR_LEN;

    return (((stlChar*)aSlot) + sSlotHdrSize);
}

stlBool smdmpfIsSimplePage( smpHandle * aPageHandle )
{
    smdmpfLogicalHdr * sLogicalHdr;
    
    sLogicalHdr = SMDMPF_GET_LOGICAL_HEADER( SMP_FRAME(aPageHandle) );
    
    return sLogicalHdr->mSimplePage;
}

stlInt16 smdmpfGetMaxFreeSize( void * aRelationHandle )
{
    stlInt16 sMaxSize = 0;
    
    sMaxSize = smpGetPageMaxFreeSize( STL_SIZEOF(smdmpfLogicalHdr),
                                      SMD_GET_INITRANS(aRelationHandle) );

    sMaxSize -= SMDMPF_MAX_PADDING_SIZE;

    return sMaxSize;
}

void smdmpfValidateTablePage( smpHandle * aPageHandle, smlEnv * aEnv )
{
#ifdef STL_VALIDATE_DATABASE
    /**
     * @todo validate table page
     */
#endif
}

stlStatus smdmpfCompactPage( smxmTrans  * aMiniTrans,
                             smpHandle  * aPageHandle,
                             stlInt16   * aFreeSlotCount,
                             smlEnv     * aEnv )
{
    smxlTransId          sTransId = SML_INVALID_TRANSID;
    stlChar            * sSrcRow;
    smlScn               sCommitScn;
    smlScn               sScn;
    smlScn               sAgableScn;
    smpRowItemIterator   sIterator;
    stlChar              sRedoData[STL_SIZEOF(stlInt16) * 2 ];
    smlTbsId             sTbsId = smpGetTbsId(aPageHandle);
    stlInt16             sFreeSlotCount = 0;
    stlInt32             sOffset = 0;

    sAgableScn = smxlGetAgableTbsScn( sTbsId, aEnv );

    SMDMPF_INIT_ROWITEM_ITERATOR( SMP_FRAME(aPageHandle), &sIterator );

    SMDMPF_FOREACH_ROWITEM_ITERATOR( &sIterator )
    {
        sSrcRow = (stlChar*)sIterator.mCurSlot;
        if( SMDMPF_IS_IN_USE( sSrcRow ) != STL_TRUE )
        {
            continue;
        }

        if( SMDMPF_IS_DELETED( sSrcRow ) != STL_TRUE )
        {
            continue;
        }

        SMDMPF_GET_TRANS_ID( sSrcRow, &sTransId );
        SMDMPF_GET_SCN( sSrcRow, &sScn );

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

        if( sCommitScn < sAgableScn )
        {
            SMDMPF_UNSET_IN_USE( sSrcRow );
            sFreeSlotCount++;
        }
    }

    if( aFreeSlotCount != NULL )
    {
        *aFreeSlotCount = sFreeSlotCount;
    }

    if( sFreeSlotCount > 0 )
    {
        if( aMiniTrans != NULL )
        {
            STL_WRITE_MOVE_INT16( sRedoData, &sFreeSlotCount, sOffset );

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

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

void smdmpfIsUsableRowHdr( stlChar  * aRowHdr,
                           stlBool  * aIsUsable )
{
    *aIsUsable = SMDMPF_IS_IN_USE( aRowHdr )? STL_FALSE: STL_TRUE;
}

/** @} */
