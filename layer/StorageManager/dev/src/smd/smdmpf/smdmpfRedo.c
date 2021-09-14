/*******************************************************************************
 * smdmpfRedo.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smdmpfRedo.c 13496 2014-08-29 05:38:43Z leekmo $
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
#include <smdmpfRowDef.h>
#include <smdmpfDef.h>
#include <smdmpf.h>
#include <smdDef.h>

/**
 * @file smdmpfRedo.c
 * @brief Storage Manager Memory Heap Table Redo Internal Routines
 */

/**
 * @addtogroup smdmpfRedo
 * @{
 */

/* Memory Heap Table Redo Module */
smdTableRedoModule gSmdmpfTableRedoModule = 
{
    smdmpfRedoMemoryHeapInsert,
    smdmpfRedoMemoryHeapUninsert,
    smdmpfRedoMemoryHeapUpdate,
    smdmpfRedoMemoryHeapUnupdate,
    smdmpfRedoMemoryHeapMigrate,
    smdmpfRedoMemoryHeapUnmigrate,
    smdmpfRedoMemoryHeapDelete,
    smdmpfRedoMemoryHeapUndelete,
    smdmpfRedoMemoryHeapUpdateLink,
    smdmpfRedoMemoryHeapInsertForUpdate,
    smdmpfRedoMemoryHeapDeleteForUpdate,
    smdmpfRedoMemoryHeapUpdateLogicalHdr,
    smdmpfRedoMemoryHeapAppend,
    smdmpfRedoMemoryHeapUnappend,
    NULL,        /* pending insert */
    smdmpfRedoMemoryHeapCompaction   /* compaction */
};

inline stlBool smdmpfIsNthRowHdrFree( smpHandle  * aPageHandle,
                                      stlInt16     aSlotSeq,
                                      stlChar   ** aRowHdr )
{
    stlChar   * sRowSlot;

    sRowSlot = SMDMPF_GET_NTH_ROW_HDR( SMP_FRAME(aPageHandle), aSlotSeq );

    *aRowHdr = sRowSlot;

    return (SMDMPF_IS_IN_USE( sRowSlot ) != STL_TRUE);
}

stlStatus smdmpfRedoMemoryHeapInsert( smlRid    * aDataRid,
                                      void      * aData,
                                      stlUInt16   aDataSize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv )
{
    stlInt16                 sSlotSeq = aDataRid->mOffset; /* Rid의 Offset 부분을 Slot Seq로 사용 */
    stlChar                * sSrcSlot;
    stlChar                * sDstSlot[SMD_MAX_COLUMN_COUNT_IN_PIECE];
    stlChar                * sPtr = (stlChar*)aData;
    smxlTransId              sTransId;
    smlScn                   sScn;
    stlInt32                 sRetryCnt = 0;
    stlInt32                 sFirstColOrd;
    stlUInt16                sRowSize;
    stlInt16                 sSlotIdx;
    stlInt16                 sColCount;
    stlInt16                 sRowOffset;
    stlUInt8                 sMemberPageType;
    stlInt64                 sColSize;

    STL_WRITE_INT64( &sTransId, sPtr );
    sPtr += STL_SIZEOF(smxlTransId);
    STL_WRITE_INT64( &sScn, sPtr );
    sPtr += STL_SIZEOF(smlScn);
    STL_WRITE_INT32( &sFirstColOrd, sPtr );
    sPtr += STL_SIZEOF(stlInt32);
    sSrcSlot = sPtr;
    sRowSize = aDataSize - (sPtr - (stlChar*)aData);

    sMemberPageType = SMDMPF_GET_MEMBER_PAGE_TYPE( SMP_FRAME(aPageHandle) );

    if( sMemberPageType == SMDMPF_MEMBER_PAGE_TYPE_CONT )
    {
        sDstSlot[0] = (stlChar *)SMDMPF_GET_START_POS(SMP_FRAME(aPageHandle));
        stlMemcpy( sDstSlot[0], sSrcSlot, sRowSize );
    }
    else
    {
        sColCount = SMDMPF_GET_PAGE_COL_COUNT(SMP_FRAME(aPageHandle));

        while( STL_TRUE )
        {
            if( SMDMPF_IS_NTH_ROW_HDR_FREE( SMP_FRAME(aPageHandle), sSlotSeq ) != STL_TRUE )
            {
                /* log hole 발견. page를 일단 compact하여 최대한 빈 공간을 확보하고 진행 */
                SMD_TRY_INTERNAL_ERROR( sRetryCnt != 1,
                                        aEnv,
                                        ( aPageHandle, aEnv, NULL ) );
            
                sRetryCnt++;
                STL_TRY( smdmpfCompactPage( NULL,
                                            aPageHandle,
                                            NULL,
                                            aEnv ) == STL_SUCCESS );
                continue;
            }

            STL_TRY( smpAllocFixedNthSlotArray( aPageHandle,
                                                sSlotSeq,
                                                SMDMPF_GET_ROW_COUNT(SMP_FRAME(aPageHandle)),
                                                STL_TRUE, /* aIsForTest */
                                                SMDMPF_ROW_HDR_LEN,
                                                sColCount,
                                                sDstSlot ) == STL_SUCCESS );
        
            if( sDstSlot[0] == NULL )
            {
                /* page를 일단 compact하여 최대한 빈 공간을 확보하고 진행 */
                SMD_TRY_INTERNAL_ERROR( sRetryCnt == 0,
                                        aEnv,
                                        ( aPageHandle, aEnv, NULL ) );
            
                sRetryCnt++;
                STL_TRY( smdmpfCompactPage( NULL,
                                            aPageHandle,
                                            NULL,
                                            aEnv ) == STL_SUCCESS );
                continue;
            }

            break;
        }

        /* slot 실제 할당 */
        STL_TRY( smpAllocFixedNthSlotArray( aPageHandle,
                                            sSlotSeq,
                                            SMDMPF_GET_ROW_COUNT(SMP_FRAME(aPageHandle)),
                                            STL_FALSE, /* aIsForTest */
                                            SMDMPF_ROW_HDR_LEN,
                                            sColCount,
                                            sDstSlot ) == STL_SUCCESS );

        sSlotIdx = 0;
        stlMemcpy( sDstSlot[sSlotIdx], sSrcSlot, SMDMPF_ROW_HDR_LEN );

        sRowOffset = SMDMPF_ROW_HDR_LEN;

        for( sSlotIdx = 1; sSlotIdx <= sColCount; sSlotIdx++ )
        {

            SMP_GET_TOTAL_COLUMN_SIZE( sSrcSlot + sRowOffset,
                                       sColSize );

            /* write column */
            stlMemcpy( sDstSlot[sSlotIdx],
                       sSrcSlot + sRowOffset,
                       sColSize );

            sRowOffset += sColSize;
        }
    }

    SMDMPF_UNSET_RTS_BOUND( sDstSlot[0] );
    SMDMPF_SET_SCN( sDstSlot[0], &sScn );

    smpSetMaxViewScn( aPageHandle, sScn );
    smdmpfValidateTablePage( aPageHandle, aEnv );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdmpfRedoMemoryHeapUninsert( smlRid    * aDataRid,
                                        void      * aData,
                                        stlUInt16   aDataSize,
                                        smpHandle * aPageHandle,
                                        smlEnv    * aEnv )
{
    stlChar          * sLogPtr = (stlChar*)aData;
    stlChar          * sBfrRowSlot;
    smxlTransId        sTransId;
    smlScn             sTransViewScn;
    stlUInt8           sMemberPageType;

    STL_WRITE_INT64( &sTransId, sLogPtr );
    sLogPtr += STL_SIZEOF(smxlTransId);
    STL_WRITE_INT64( &sTransViewScn, sLogPtr );
    sLogPtr += STL_SIZEOF(smlScn);

    SMD_TRY_INTERNAL_ERROR( (sTransId == SML_INVALID_TRANSID) &&
                            (sTransViewScn == SML_INFINITE_SCN) &&
                            (SMDMPF_IS_DELETED( sLogPtr ) == STL_TRUE),
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );

    sMemberPageType = SMDMPF_GET_MEMBER_PAGE_TYPE( SMP_FRAME(aPageHandle) );

    /* before row 정보 수집 */
    if( sMemberPageType == SMDMPF_MEMBER_PAGE_TYPE_CONT )
    {
        sBfrRowSlot = (stlChar *)SMDMPF_GET_START_POS(SMP_FRAME(aPageHandle));
    }
    else
    {
        sBfrRowSlot = SMDMPF_GET_NTH_ROW_HDR( SMP_FRAME(aPageHandle), aDataRid->mOffset );
    }

    /* row header 복사 */
    stlMemcpy( sBfrRowSlot,
               (stlChar*)aData + STL_SIZEOF(smxlTransId) + STL_SIZEOF(smlScn),
               SMDMPF_ROW_HDR_LEN );

    SMDMPF_DECREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) );

    smdmpfValidateTablePage( aPageHandle, aEnv );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdmpfRedoMemoryHeapUpdate( smlRid    * aDataRid,
                                      void      * aData,
                                      stlUInt16   aDataSize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv )
{
    knlRegionMark       sMemMark;
    knlValueBlockList * sUpdateCols;
    stlChar           * sLogPtr = (stlChar*)aData;
    stlInt32            sState = 0;
    stlChar           * sRowHdr;
    stlInt16            sTotalColCnt;
    stlInt16            sUpdateColCnt;
    stlInt16            sBfrRowPieceSize;
    smxlTransId         sTransId;
    smlScn              sTransViewScn;
    stlInt32            sFirstColOrd;
    smlPid              sContColPid[SMD_MAX_COLUMN_COUNT_IN_PIECE];

    STL_DASSERT( (aDataRid->mTbsId  != 0) ||
                 (aDataRid->mPageId != 0) ||
                 (aDataRid->mOffset != 0) );

    STL_WRITE_INT64( &sTransId, sLogPtr );
    sLogPtr += STL_SIZEOF(smxlTransId);
    STL_WRITE_INT64( &sTransViewScn, sLogPtr );
    sLogPtr += STL_SIZEOF(smlScn);

    SMD_TRY_INTERNAL_ERROR( (sTransId != SML_INVALID_TRANSID) ||
                            (sTransViewScn != SML_INFINITE_SCN),
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );

    STL_TRY( smdmpfAnalyzeUpdateLogRec( sLogPtr,
                                        &sMemMark,
                                        &sUpdateColCnt,
                                        &sFirstColOrd,
                                        &sBfrRowPieceSize,
                                        &sUpdateCols,
                                        sContColPid,
                                        aEnv ) == STL_SUCCESS );
    sState = 1;

    /* before row 정보 수집 */
    sRowHdr = SMDMPF_GET_NTH_ROW_HDR( SMP_FRAME(aPageHandle), aDataRid->mOffset );

    SMDMPF_GET_COLUMN_CNT( sRowHdr, &sTotalColCnt );

    /* row header 복사 */
    stlMemcpy( sRowHdr,
               (stlChar*)aData + STL_SIZEOF(smxlTransId) + STL_SIZEOF(smlScn),
               SMDMPF_ROW_HDR_LEN );

    if( sUpdateColCnt > 0 )
    {
        /* row column 복사 */
        STL_TRY( smdmpfCopyAndUpdateColumns( aPageHandle,
                                             aDataRid->mOffset,
                                             sUpdateCols,
                                             0,  /* block idx */
                                             sBfrRowPieceSize,
                                             sTotalColCnt,
                                             sFirstColOrd,
                                             sContColPid,
                                             NULL,
                                             NULL,
                                             aEnv ) == STL_SUCCESS );
    }

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       (knlEnv*)aEnv ) == STL_SUCCESS );

    smpSetMaxViewScn( aPageHandle, sTransViewScn );
    smdmpfValidateTablePage( aPageHandle, aEnv );

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


stlStatus smdmpfRedoMemoryHeapUnupdate( smlRid    * aDataRid,
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
    stlInt16            sBfrRowHeaderSize;
    stlInt16            sBfrColListSize;
    stlInt16            sBfrRowPieceSize;
    smxlTransId         sTransId;
    smlScn              sTransViewScn;
    stlInt32            sFirstColOrd;
    smlPid              sContColPid[SMD_MAX_COLUMN_COUNT_IN_PIECE];

    STL_WRITE_INT64( &sTransId, sLogPtr );
    sLogPtr += STL_SIZEOF(smxlTransId);
    STL_WRITE_INT64( &sTransViewScn, sLogPtr );
    sLogPtr += STL_SIZEOF(smlScn);

    SMD_TRY_INTERNAL_ERROR( (sTransId == SML_INVALID_TRANSID) &&
                            (sTransViewScn == SML_INFINITE_SCN),
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );

    STL_TRY( smdmpfAnalyzeUpdateLogRec( sLogPtr,
                                        &sMemMark,
                                        &sUpdateColCnt,
                                        &sFirstColOrd,
                                        &sBfrColListSize,
                                        &sUpdateCols,
                                        sContColPid,
                                        aEnv ) == STL_SUCCESS );
    sState = 1;

    /* before row 정보 수집 */
    sRowSlot = SMDMPF_GET_NTH_ROW_HDR( SMP_FRAME(aPageHandle), aDataRid->mOffset );

    SMDMPF_GET_COLUMN_CNT( sRowSlot, &sTotalColCnt );
    SMDMPF_GET_ROW_HDR_SIZE( sRowSlot, &sBfrRowHeaderSize );

    sBfrRowPieceSize = sBfrRowHeaderSize + sBfrColListSize;
    
    /* row header 복사 */
    stlMemcpy( sRowSlot,
               (stlChar*)aData + STL_SIZEOF(smxlTransId) + STL_SIZEOF(smlScn),
               sBfrRowHeaderSize );

    if( sUpdateColCnt > 0 )
    {
        /* row column 복사 */
        STL_TRY( smdmpfCopyAndUpdateColumns( aPageHandle,
                                             aDataRid->mOffset,
                                             sUpdateCols,
                                             0,  /* block idx */
                                             sBfrRowPieceSize,
                                             sTotalColCnt,
                                             sFirstColOrd,
                                             sContColPid,
                                             NULL,
                                             NULL,
                                             aEnv ) == STL_SUCCESS );
    }

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       (knlEnv*)aEnv ) == STL_SUCCESS );

    smdmpfValidateTablePage( aPageHandle, aEnv );

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



stlStatus smdmpfRedoMemoryHeapMigrate( smlRid    * aDataRid,
                                       void      * aData,
                                       stlUInt16   aDataSize,
                                       smpHandle * aPageHandle,
                                       smlEnv    * aEnv )
{
    return STL_FAILURE;
}


stlStatus smdmpfRedoMemoryHeapUpdateLink( smlRid    * aDataRid,
                                          void      * aData,
                                          stlUInt16   aDataSize,
                                          smpHandle * aPageHandle,
                                          smlEnv    * aEnv )
{
    return STL_FAILURE;
}


stlStatus smdmpfRedoMemoryHeapUnmigrate( smlRid    * aDataRid,
                                         void      * aData,
                                         stlUInt16   aDataSize,
                                         smpHandle * aPageHandle,
                                         smlEnv    * aEnv )
{
    return STL_FAILURE;
}


stlStatus smdmpfRedoMemoryHeapDelete( smlRid    * aDataRid,
                                      void      * aData,
                                      stlUInt16   aDataSize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv )
{
    stlChar          * sLogPtr = (stlChar*)aData;
    stlChar          * sBfrRowSlot;
    stlInt16           sBfrRowHeaderSize;
    smxlTransId        sTransId;
    smlScn             sTransViewScn;
    stlUInt8           sMemberPageType;

    STL_WRITE_INT64( &sTransId, sLogPtr );
    sLogPtr += STL_SIZEOF(smxlTransId);
    STL_WRITE_INT64( &sTransViewScn, sLogPtr );
    sLogPtr += STL_SIZEOF(smlScn);

    SMD_TRY_INTERNAL_ERROR( (sTransId != SML_INVALID_TRANSID) ||
                            (sTransViewScn != SML_INFINITE_SCN),
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );

    sMemberPageType = SMDMPF_GET_MEMBER_PAGE_TYPE( SMP_FRAME(aPageHandle) );

    /* before row 정보 수집 */
    if( sMemberPageType == SMDMPF_MEMBER_PAGE_TYPE_CONT )
    {
        sBfrRowSlot = (stlChar *)SMDMPF_GET_START_POS(SMP_FRAME(aPageHandle));
    }
    else
    {
        sBfrRowSlot = SMDMPF_GET_NTH_ROW_HDR( SMP_FRAME(aPageHandle), aDataRid->mOffset );
    }

    SMDMPF_GET_ROW_HDR_SIZE( sBfrRowSlot, &sBfrRowHeaderSize );

    /* row header 복사 */
    stlMemcpy( sBfrRowSlot,
               (stlChar*)aData + STL_SIZEOF(smxlTransId) + STL_SIZEOF(smlScn),
               sBfrRowHeaderSize );

    /* delete 되었으니 reclaimed space에 일단 추가한다 */
    SMDMPF_DECREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) );

    smpSetMaxViewScn( aPageHandle, sTransViewScn );
    smdmpfValidateTablePage( aPageHandle, aEnv );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdmpfRedoMemoryHeapUndelete( smlRid    * aDataRid,
                                        void      * aData,
                                        stlUInt16   aDataSize,
                                        smpHandle * aPageHandle,
                                        smlEnv    * aEnv )
{
    stlChar          * sLogPtr = (stlChar*)aData;
    stlChar          * sRowSlot;
    stlInt16           sBfrRowHeaderSize;
    smxlTransId        sTransId;
    smlScn             sTransViewScn;
    stlUInt8           sMemberPageType;

    STL_WRITE_INT64( &sTransId, sLogPtr );
    sLogPtr += STL_SIZEOF(smxlTransId);
    STL_WRITE_INT64( &sTransViewScn, sLogPtr );
    sLogPtr += STL_SIZEOF(smlScn);

    SMD_TRY_INTERNAL_ERROR( (sTransId == SML_INVALID_TRANSID) &&
                            (sTransViewScn == SML_INFINITE_SCN) &&
                            (SMDMPF_IS_DELETED( sLogPtr ) != STL_TRUE),
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );

    sMemberPageType = SMDMPF_GET_MEMBER_PAGE_TYPE( SMP_FRAME(aPageHandle) );

    /* before row 정보 수집 */ 
    if( sMemberPageType == SMDMPF_MEMBER_PAGE_TYPE_CONT )
    {
        sRowSlot = (stlChar *)SMDMPF_GET_START_POS(SMP_FRAME(aPageHandle));
    }
    else
    {
        sRowSlot = SMDMPF_GET_NTH_ROW_HDR( SMP_FRAME(aPageHandle), aDataRid->mOffset );
    }

    SMDMPF_GET_ROW_HDR_SIZE( sRowSlot, &sBfrRowHeaderSize );

    /* row header 복사 */
    stlMemcpy( sRowSlot,
               (stlChar*)aData + STL_SIZEOF(smxlTransId) + STL_SIZEOF(smlScn),
               sBfrRowHeaderSize );

    SMDMPF_INCREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) );

    smdmpfValidateTablePage( aPageHandle, aEnv );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdmpfRedoMemoryHeapInsertForUpdate( smlRid    * aDataRid,
                                               void      * aData,
                                               stlUInt16   aDataSize,
                                               smpHandle * aPageHandle,
                                               smlEnv    * aEnv )
{
    /* CDC를 위해 REDO TYPE만 따로 만들고 실제 redo는 일반 insert 와 동일하다 */
    return smdmpfRedoMemoryHeapInsert( aDataRid, aData, aDataSize, aPageHandle, aEnv );
}


stlStatus smdmpfRedoMemoryHeapDeleteForUpdate( smlRid    * aDataRid,
                                               void      * aData,
                                               stlUInt16   aDataSize,
                                               smpHandle * aPageHandle,
                                               smlEnv    * aEnv )
{
    /* CDC를 위해 REDO TYPE만 따로 만들고 실제 redo는 일반 delete 와 동일하다 */
    return smdmpfRedoMemoryHeapDelete( aDataRid, aData, aDataSize, aPageHandle, aEnv );
}

stlStatus smdmpfRedoMemoryHeapUpdateLogicalHdr( smlRid    * aDataRid,
                                                void      * aData,
                                                stlUInt16   aDataSize,
                                                smpHandle * aPageHandle,
                                                smlEnv    * aEnv )
{
    smdmpfLogicalHdr * sLogicalHdr;

    STL_DASSERT( aDataSize == STL_SIZEOF(smdmpfLogicalHdr) );
    sLogicalHdr = SMDMPF_GET_LOGICAL_HEADER( SMP_FRAME(aPageHandle) );
    stlMemcpy( sLogicalHdr, aData, STL_SIZEOF(smdmpfLogicalHdr) );

    return STL_SUCCESS;
}

stlStatus smdmpfRedoMemoryHeapAppend( smlRid    * aDataRid,
                                      void      * aData,
                                      stlUInt16   aDataSize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv )
{
    return STL_FAILURE;
}

stlStatus smdmpfRedoMemoryHeapUnappend( smlRid    * aDataRid,
                                        void      * aData,
                                        stlUInt16   aDataSize,
                                        smpHandle * aPageHandle,
                                        smlEnv    * aEnv )
{
    return STL_FAILURE;
}


stlStatus smdmpfRedoMemoryPendingInsert( smlRid    * aDataRid,
                                         void      * aData,
                                         stlUInt16   aDataSize,
                                         smpHandle * aPageHandle,
                                         smlEnv    * aEnv )
{
    return STL_SUCCESS;
}


stlStatus smdmpfRedoMemoryHeapCompaction( smlRid    * aDataRid,
                                          void      * aData,
                                          stlUInt16   aDataSize,
                                          smpHandle * aPageHandle,
                                          smlEnv    * aEnv )
{
    STL_TRY( smdmpfCompactPage( NULL,
                                aPageHandle,
                                NULL,
                                aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
