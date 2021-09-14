/*******************************************************************************
 * smdmpfTable.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smdmpfTable.c 13901 2014-11-13 01:31:33Z mkkim $
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
#include <smlGeneral.h>
#include <sms.h>
#include <smxm.h>
#include <smxl.h>
#include <smp.h>
#include <smd.h>
#include <sme.h>
#include <smf.h>
#include <smr.h>
#include <smg.h>
#include <sma.h>
#include <smkl.h>
#include <smdmpfRowDef.h>
#include <smdmpfDef.h>
#include <smdmpf.h>
#include <smdDef.h>

/**
 * @file smdmpfTable.c
 * @brief Storage Manager Layer Memory Heap Table Create/Drop Routines
 */

/**
 * @addtogroup smd
 * @{
 */

/* Memory Heap Fixed Length Table Module */
smdTableModule gSmdmpfTableModule = 
{
    smdmpfCreate,
    NULL, /* CreateIotFunc */
    smdmpfCreateUndo,
    smdmpfDrop,
    smdmpfCopy,
    NULL,
    smdmpfDropColumns,
    NULL,   /* alter table attr */
    NULL,   /* merget table attr */
    smdmpfCheckExistRow,
    smdmpfCreateForTruncate,
    smdmpfDropAging,
    smdmpfInitialize,
    smdmpfFinalize,
    smdmpfInsert,
    smdmpfBlockInsert,
    smdmpfUpdate,
    smdmpfDelete,
    NULL, /* MergeFunc */
    NULL, /* RowCountFunc */
    smdmpfFetch,
    smdmpfFetchWithRowid,
    smdmpfLockRow,
    smdmpfCompareKeyValue,
    smdmpfExtractKeyValue,
    smdmpfExtractValidKeyValue,
    smdmpfSort,
    smdmpfGetSlotBody,
    NULL,     /* TruncateFunc */
    NULL,     /* SetFlag */
    NULL,     /* CleanupFunc */
    smdmpfBuildCache,
    smdmpfFetchRecord4Index
};

smeIteratorModule gSmdmpfIteratorModule = 
{
    STL_SIZEOF(smdmpfIterator),

    smdmpfInitIterator,
    NULL,     /* finiIterator */
    smdmpfResetIterator,
    smdmpfMoveToRowRid,
    smdmpfMoveToPosNum,
    {
        smdmpfFetchNext,
        smdmpfFetchPrev,
        smdmpfFetchAggr,
        NULL    /* FetchSampling */
    }
};


/**
 * @brief segment page에 테이블헤더 정보를 기록한다.
 * @param[in] aStatement Statement 객체
 * @param[in] aSegmentRid table이 사용할 segment
 * @param[in] aSegmentHandle 기존에 생성된 segment handle
 * @param[in] aAttr table 관련 attribute 정보
 * @param[out] aRelationHandle 생성된 테이블을 조작할때 사용할 메모리 핸들
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smdmpfInitTableHeader( smlStatement  * aStatement,
                                 smlRid        * aSegmentRid,
                                 void          * aSegmentHandle,
                                 smlTableAttr  * aAttr,
                                 void         ** aRelationHandle,
                                 smlEnv        * aEnv )
{
    smdTableHeader        sTblHeader;
    smxmTrans             sMiniTrans;
    stlUInt32             sAttr = SMXM_ATTR_REDO;
    smpHandle             sSegPage;
    stlChar             * sPtr;
    void                * sCaches[2];
    stlUInt32             sState = 0;
    stlBool               sIsLocking;
    stlInt64              sIniTrans;
    stlInt64              sMaxTrans;
    stlInt64              sPctFree;
    stlInt64              sPctUsed;
    void                * sLockHandle = NULL;
    stlInt32              sMemberPageCount;
    void                * sColLengthArray = NULL;
    stlInt16              sRowCount;
    stlBool               sBuildCache = STL_FALSE;

    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_DEFAULT_TABLE_LOCKING,
                                      &sIsLocking,
                                      KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_DEFAULT_INITRANS,
                                      &sIniTrans,
                                      KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_DEFAULT_MAXTRANS,
                                      &sMaxTrans,
                                      KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_DEFAULT_PCTFREE,
                                      &sPctFree,
                                      KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_DEFAULT_PCTUSED,
                                      &sPctUsed,
                                      KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    stlMemset( (void*)&sTblHeader, 0x00, STL_SIZEOF(smdTableHeader) );
    sTblHeader.mRelHeader.mRelationType = SML_MEMORY_PERSISTENT_HEAP_COLUMNAR_TABLE;
    sTblHeader.mRelHeader.mRelHeaderSize = STL_SIZEOF(smdTableHeader);
    sTblHeader.mRelHeader.mCreateTransId = SMA_GET_TRANS_ID( aStatement );
    sTblHeader.mRelHeader.mDropTransId = SML_INVALID_TRANSID;
    sTblHeader.mRelHeader.mCreateScn = 0;
    sTblHeader.mRelHeader.mDropScn = 0;
    SME_SET_SEGMENT_HANDLE((void*)&sTblHeader, aSegmentHandle);
    SME_SET_OBJECT_SCN((void*)&sTblHeader, SML_INFINITE_SCN);
    SME_SET_RELATION_STATE((void*)&sTblHeader, SME_RELATION_STATE_ACTIVE);

    sTblHeader.mLoggingFlag = ((aAttr->mValidFlags & SML_TABLE_LOGGING_MASK) == SML_TABLE_LOGGING_YES) ?
        aAttr->mLoggingFlag : STL_TRUE;
    sTblHeader.mLockingFlag = ((aAttr->mValidFlags & SML_TABLE_LOCKING_MASK) == SML_TABLE_LOCKING_YES) ?
        aAttr->mLockingFlag : sIsLocking;
    sTblHeader.mIniTrans = ((aAttr->mValidFlags & SML_TABLE_INITRANS_MASK) == SML_TABLE_INITRANS_YES) ?
        aAttr->mIniTrans : sIniTrans;
    sTblHeader.mMaxTrans = ((aAttr->mValidFlags & SML_TABLE_MAXTRANS_MASK) == SML_TABLE_MAXTRANS_YES) ?
        aAttr->mMaxTrans : sMaxTrans;
    sTblHeader.mPctFree = ((aAttr->mValidFlags & SML_TABLE_PCTFREE_MASK) == SML_TABLE_PCTFREE_YES) ?
        aAttr->mPctFree : sPctFree;
    sTblHeader.mPctUsed = ((aAttr->mValidFlags & SML_TABLE_PCTUSED_MASK) == SML_TABLE_PCTUSED_YES) ?
        aAttr->mPctUsed : sPctUsed;
    SMD_SET_LOCK_HANDLE((void*)&sTblHeader, sLockHandle);

    /**
     * Columnar Table의 row를 저장하기 위해 필요한 member page 수와
     * column length array를 생성한다.
     */
    STL_TRY( smdmpfBuildColInfo( &aAttr->mColumnarAttr,
                                 &sMemberPageCount,
                                 &sColLengthArray,
                                 &sRowCount,
                                 aEnv ) == STL_SUCCESS );
    sState = 1;

    sTblHeader.mMemberPageCount = sMemberPageCount;
    sTblHeader.mRowCount        = sRowCount;
    sTblHeader.mContColumnThreshold = aAttr->mColumnarAttr.mContColumnThreshold;
    sTblHeader.mColCount = aAttr->mColumnarAttr.mColCount;

    STL_TRY( smxmBegin( &sMiniTrans,
                        aStatement->mTransId,
                        *aSegmentRid,
                        sAttr,
                        aEnv ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( smpAcquire( &sMiniTrans,
                         aSegmentRid->mTbsId,
                         aSegmentRid->mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sSegPage,
                         aEnv ) == STL_SUCCESS );

    sPtr = smpGetBody(SMP_FRAME(&sSegPage)) + smsGetHeaderSize( aSegmentHandle );

    stlMemcpy( sPtr, &sTblHeader, STL_SIZEOF(smdTableHeader) );

    STL_TRY( smxmWriteLog( &sMiniTrans,
                           SMG_COMPONENT_RELATION,
                           SMR_LOG_INIT_REL_HEADER,
                           SMR_REDO_TARGET_PAGE,
                           sPtr,
                           STL_SIZEOF(smdTableHeader),
                           sSegPage.mPch->mTbsId,
                           sSegPage.mPch->mPageId,
                           sPtr - (stlChar*)SMP_FRAME(&sSegPage),
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    STL_TRY( smgAllocShmMem4Open( STL_SIZEOF( smdTableHeader ),
                                  aRelationHandle,
                                  aEnv ) == STL_SUCCESS );
    sState = 3;

    stlMemcpy( *aRelationHandle,
               &sTblHeader, 
               STL_SIZEOF(smdTableHeader) );

    sCaches[0] = aSegmentHandle;
    sCaches[1] = *aRelationHandle;
    smpSetVolatileArea( &sSegPage, &sCaches, STL_SIZEOF(void*) * 2 );

    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    if( sTblHeader.mLockingFlag == STL_TRUE )
    {
        STL_TRY( smklAllocLockHandle( *aRelationHandle,
                                      &sLockHandle,
                                      aEnv ) == STL_SUCCESS );
        
        SMD_SET_LOCK_HANDLE(*aRelationHandle, sLockHandle);
    }

    /* Table의 column length를 기록한다. */
    STL_TRY( smdmpfWriteColumnLength( aStatement,
                                      aSegmentRid,
                                      aSegmentHandle,
                                      &aAttr->mColumnarAttr,
                                      aEnv ) == STL_SUCCESS );

    STL_TRY( smdmpfBuildCache( *aRelationHandle,
                               sColLengthArray,
                               aEnv ) == STL_SUCCESS );
    sBuildCache = STL_TRUE;

    return STL_SUCCESS;

    STL_FINISH;

    if( sBuildCache == STL_TRUE )
    {
        (void) smdmpfDestroyCache( *aRelationHandle, aEnv );
        sColLengthArray = NULL;
    }

    if( sLockHandle != NULL )
    {
        (void)smklFreeLockHandle( sLockHandle, aEnv );
        SMD_SET_LOCK_HANDLE( *aRelationHandle, NULL );
    }

    switch( sState )
    {
        case 3 :
            sCaches[0] = aSegmentHandle;
            sCaches[1] = NULL;
            smpSetVolatileArea( &sSegPage, &sCaches, STL_SIZEOF(void*) * 2 );
            (void) smgFreeShmMem4Open( aRelationHandle, aEnv );
        case 2 :
            (void) smxmRollback( &sMiniTrans, aEnv );
        case 1 :
            if( sColLengthArray != NULL )
            {
                (void) smdmpfDestroyColInfo( sColLengthArray, aEnv );
                sColLengthArray = NULL;
            }
        default :
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief Columnar table을 위해 column length를 persistent하게 저장한다.
 * @param[in] aStatement     Statement 객체
 * @param[in] aSegmentRid    table이 사용할 segment
 * @param[in] aSegmentHandle 기존에 생성된 segment handle
 * @param[in] aColumnAttr    Table의 column attribute
 * @param[in,out] aEnv       Environment 정보
 */
stlStatus smdmpfWriteColumnLength( smlStatement    * aStatement,
                                   smlRid          * aSegmentRid,
                                   void            * aSegmentHandle,
                                   smlColumnarAttr * aColumnAttr,
                                   smlEnv          * aEnv )
{
    smxmTrans          sMiniTrans;
    stlUInt32          sAttr = SMXM_ATTR_REDO;
    smpHandle          sPageHandle;
    smlPid             sColumnPageId;
    stlInt32           i;
    stlUInt16          sWrittenBytes = 0;
    stlUInt16          sLengthPos;
    stlUInt16          sOffset;
    stlUInt8           sColLenSize;
    stlUInt8           sColLenFlag;
    stlUInt8           sLength8;
    stlUInt16          sLength16;
    stlInt64           sColLen;
    stlChar          * sPtr;
    smdmpfColMetaHdr * sColMetaHdr;
    stlInt32           sState = 0;
    smlRid             sUndoRid;

    STL_PARAM_VALIDATE( aColumnAttr->mColCount > 0, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aColumnAttr->mColLengthArray != NULL, KNL_ERROR_STACK( aEnv ) );

    STL_TRY( smxmBegin( &sMiniTrans,
                        aStatement->mTransId,
                        *aSegmentRid,
                        sAttr,
                        aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( smpAcquire( &sMiniTrans,
                         aSegmentRid->mTbsId,
                         aSegmentRid->mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

    sPtr = smpGetBody(SMP_FRAME(&sPageHandle)) + smsGetHeaderSize( aSegmentHandle );

    /* sOffset : ColMetaHdr의 offset, sPtr : col length가 기록될 offset */
    sOffset = sPtr - (stlChar*)SMP_FRAME(&sPageHandle) + STL_SIZEOF(smdTableHeader);
    sPtr += (STL_SIZEOF(smdTableHeader) + STL_SIZEOF(smdmpfColMetaHdr));
    sLengthPos = sOffset + STL_SIZEOF(smdmpfColMetaHdr);

    for( i = 0; i < aColumnAttr->mColCount; i++ )
    {
        sColLen = aColumnAttr->mColLengthArray[i];
        SMP_GET_COLUMN_LEN_SIZE( sColLen, &sColLenSize );

        /* page에 더이상 쓸 공간이 없으면 새로운 page를 추가한다. */
        if( (sLengthPos + sWrittenBytes + sColLenSize) > (SMP_PAGE_SIZE - STL_SIZEOF(smpPhyTail)) )
        {

            STL_TRY( smsAllocColMetaHdr( &sMiniTrans,
                                         SML_SEG_TYPE_MEMORY_BITMAP,
                                         &sColumnPageId,
                                         aEnv ) == STL_SUCCESS );

            STL_TRY( smxlInsertUndoRecord( &sMiniTrans,
                                           SMG_COMPONENT_TABLE,
                                           SMD_UNDO_LOG_ALLOC_COLUMN_HDR_PAGE,
                                           NULL,
                                           0,
                                           ((smlRid){ aSegmentRid->mTbsId,
                                                   0,
                                                   sColumnPageId }),
                                           &sUndoRid,
                                           aEnv ) == STL_SUCCESS );

            /* 사용하던 page에 next page를 설정하고, logging한다. */
            sColMetaHdr = (smdmpfColMetaHdr *)(SMP_FRAME(&sPageHandle) + sOffset);
            stlMemset( (void*)sColMetaHdr, 0x00, STL_SIZEOF( smdmpfColMetaHdr ) );
            sColMetaHdr->mNextPageId = sColumnPageId;
            sColMetaHdr->mHighWaterMark = sLengthPos + sWrittenBytes;

            STL_TRY( smxmWriteLog( &sMiniTrans,
                                   SMG_COMPONENT_RELATION,
                                   SMR_LOG_SAVE_COLUMN_LENGTH,
                                   SMR_REDO_TARGET_PAGE,
                                   (void*)sColMetaHdr,
                                   sWrittenBytes + STL_SIZEOF( smdmpfColMetaHdr ),
                                   SMS_SEGMENT_MAP_RID.mTbsId,
                                   smpGetPageId( &sPageHandle ),
                                   sOffset,
                                   SMXM_LOGGING_REDO,
                                   aEnv )
                     == STL_SUCCESS );

            sState = 0;
            STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

            STL_TRY( smxmBegin( &sMiniTrans,
                                aStatement->mTransId,
                                *aSegmentRid,
                                sAttr,
                                aEnv ) == STL_SUCCESS );
            sState = 1;

            STL_TRY( smpAcquire( &sMiniTrans,
                                 aSegmentRid->mTbsId,
                                 sColumnPageId,
                                 KNL_LATCH_MODE_EXCLUSIVE,
                                 &sPageHandle,
                                 aEnv ) == STL_SUCCESS );

            sOffset = STL_SIZEOF( smpPhyHdr ) + STL_SIZEOF(stlInt32);
            sPtr = (stlChar *)(smpGetBody(SMP_FRAME(&sPageHandle)) + STL_SIZEOF(stlInt32) + STL_SIZEOF(smdmpfColMetaHdr));
            sLengthPos = sOffset + STL_SIZEOF(smdmpfColMetaHdr);
            sWrittenBytes = 0;
        }

        switch( sColLenSize )
        {
            case 1:
                sLength8 = (stlUInt8)aColumnAttr->mColLengthArray[i];

                STL_WRITE_MOVE_INT8( sPtr, &sLength8, sWrittenBytes );
                break;
            case 3:
                sColLenFlag = SMP_COLUMN_LEN_2BYTE_FLAG;
                sLength16 = aColumnAttr->mColLengthArray[i];

                STL_WRITE_MOVE_INT8( sPtr, &sColLenFlag, sWrittenBytes );
                STL_WRITE_MOVE_INT16( sPtr, &sLength16, sWrittenBytes );
                break;
            case 5:
            case 9:
            default:
                STL_DASSERT( STL_FALSE );
        }
    }

    sColMetaHdr = (smdmpfColMetaHdr *)(SMP_FRAME(&sPageHandle) + sOffset);
    stlMemset( (void*)sColMetaHdr, 0x00, STL_SIZEOF(smdmpfColMetaHdr) );
    sColMetaHdr->mNextPageId = SMP_NULL_PID;
    sColMetaHdr->mHighWaterMark = sLengthPos + sWrittenBytes;

    STL_DASSERT( sOffset == 320 );

    /* current page를 logging한다. */
    STL_TRY( smxmWriteLog( &sMiniTrans,
                           SMG_COMPONENT_RELATION,
                           SMR_LOG_SAVE_COLUMN_LENGTH,
                           SMR_REDO_TARGET_PAGE,
                           (void*)sColMetaHdr,
                           sWrittenBytes + STL_SIZEOF( smdmpfColMetaHdr ),
                           SMS_SEGMENT_MAP_RID.mTbsId,
                           smpGetPageId( &sPageHandle ),
                           sOffset,
                           SMXM_LOGGING_REDO,
                           aEnv )
             == STL_SUCCESS );

    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)smxmRollback( &sMiniTrans, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief Columnar Table의 member page 수를 구하고 column length array를 생성한다.
 * @param[in] aColumnAttr Table의 column attribute
 * @param[out] aMemberPageCount Table을 저장하기 위해 필요한 member page 수
 * @param[out] aColLengthArray 각 column length를 저장한 array
 * @param[out] aRowCount 각 member page에 저장될 row piece의 수
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smdmpfBuildColInfo( smlColumnarAttr  * aColumnAttr,
                              stlInt32         * aMemberPageCount,
                              void            ** aColLengthArray,
                              stlInt16         * aRowCount,
                              smlEnv           * aEnv )
{
    stlInt16     sPageFreeSize;
    stlInt64     sTotalRowSize;
    stlInt16     sAccColSize;
    stlInt16     sMemberPageCount;
    stlInt16     sRowCount;
    stlInt16     i;
    stlUInt8     sColLenSize;
    stlInt32     sState = 0;
    stlInt32     sColCount = aColumnAttr->mColCount;
    stlInt16   * sColLengthArray;
    stlInt16     sRowPieceSize;
    stlInt64     sMaxColumnSize = SMDMPF_ROW_HDR_LEN;
    stlInt16     sMinRowCount;
    stlInt16     sThreshold;
    stlInt16     sColLen;
    stlInt32     sSubColCount;

    sThreshold = aColumnAttr->mContColumnThreshold;

    STL_TRY( smgAllocShmMem4Open( STL_SIZEOF( stlInt16 ) * aColumnAttr->mColCount,
                                  (void**)&sColLengthArray,
                                  aEnv ) == STL_SUCCESS );
    sState = 1;

    stlMemcpy( sColLengthArray,
               aColumnAttr->mColLengthArray,
               aColumnAttr->mColCount * STL_SIZEOF( stlInt16 ) );

    sTotalRowSize = SMDMPF_ROW_HDR_LEN;

    for( i = 0; i < sColCount; i++ )
    {
        sColLen = sColLengthArray[i];
        SMP_GET_COLUMN_LEN_SIZE( sColLen, &sColLenSize );

        if( sColLen > sThreshold )
        {
            sColLen = sThreshold;
            SMP_GET_CONT_COLUMN_LEN_SIZE( &sColLenSize );
        }

        sTotalRowSize += sColLen + sColLenSize;

        if( sMaxColumnSize < (sColLen + sColLenSize) )
        {
            sMaxColumnSize = sColLen + sColLenSize;
        }
    }

    /**
     * row를 저장하기 위해 필요한 data page 수를 구한다.
     * - 전체 row size와 page body size, minimum row count를 이용하여
     *    row 를 저장하기 위해 필요한 page 수를 계산한다.
     */
    sPageFreeSize = SMDMPF_PAGE_MAX_FREE_SIZE( sColCount );
    sMinRowCount  = sPageFreeSize / sTotalRowSize;

    /**
     * Row의 전체 column을 하나의 페이지에 min row count보다 같거나 많이
     * 저장할 수 없으면 여러 개의 member page에 분산한다.
     */
    if( sMinRowCount < aColumnAttr->mMinRowCount )
    {
        sMinRowCount = sPageFreeSize / (sMaxColumnSize + SMDMPF_ROW_HDR_LEN);

        /**
         * 주어진 MIN ROW COUNT보다 같거나 많은 row를 입력할 수 있어야 한다.
         * 그렇지 못하면 table creation fail
         */
        STL_TRY_THROW( aColumnAttr->mMinRowCount <= sMinRowCount,
                       RAMP_ERR_MIN_ROW_VIOLATION );

        sMinRowCount = aColumnAttr->mMinRowCount;
    }

    sRowPieceSize    = sPageFreeSize / sMinRowCount;
    sMemberPageCount = 1;

    if( sTotalRowSize <= sRowPieceSize )
    {
        /**
         * 1. row를 하나의 page에 저장가능한 경우
         */
        sRowCount = sPageFreeSize / sTotalRowSize;
    }
    else
    {
        /**
         * 2. row를 여러 page에 저장하는 경우
         *    - 필요한 member page 수를 계산한다.
         */
        sPageFreeSize = SMDMPF_PAGE_MAX_FREE_SIZE( 0 );
        sRowPieceSize = sPageFreeSize / sMinRowCount;

        sAccColSize   = SMDMPF_ROW_HDR_LEN;

        /**
         * 2.1 모든 member page들에 저장되는 row 수가 동일해야 하기 때문에
         *     master page에 저장 가능한 row 수를 먼저 계산한다.
         */
        for( i = 0; i < sColCount; i++ )
        {
            /**
             * Member page에 저장 가능한 column의 수는
             * SMD_MAX_COLUMN_COUNT_IN_PIECE 보다 작거나 같다.
             */
            if( SMD_MAX_COLUMN_COUNT_IN_PIECE <= i )
            {
                sRowCount = sPageFreeSize / sAccColSize;
                break;
            }

            sColLen = sColLengthArray[i];
            SMP_GET_COLUMN_LEN_SIZE( sColLen, &sColLenSize );

            if( sColLen > sThreshold )
            {
                sColLen = sThreshold;
                SMP_GET_CONT_COLUMN_LEN_SIZE( &sColLenSize );
            }

            if( (sAccColSize + sColLen + sColLenSize + STL_SIZEOF(stlUInt16)) > sRowPieceSize )
            {
                sRowCount = sPageFreeSize / sAccColSize;
                break;
            }
            else
            {
                sAccColSize += sColLen + sColLenSize + STL_SIZEOF(stlUInt16);
            }
        }

        sAccColSize = SMDMPF_ROW_HDR_LEN;

        /**
         * 2.2 Slave page에 저장될 column 수는 저장 가능한 row 수에 의해 결정된다.
         */

        for( sSubColCount = 0; i < sColCount; i++ )
        {
            if( SMD_MAX_COLUMN_COUNT_IN_PIECE <= sSubColCount )
            {
                sMemberPageCount++;
                sAccColSize = SMDMPF_ROW_HDR_LEN;
                sSubColCount = 0;
            }

            sColLen = sColLengthArray[i];
            SMP_GET_COLUMN_LEN_SIZE( sColLen, &sColLenSize );

            if( sColLen > sThreshold )
            {
                sColLen = sThreshold;
                SMP_GET_CONT_COLUMN_LEN_SIZE( &sColLenSize );
            }

            if( ((sAccColSize + sColLen + sColLenSize + STL_SIZEOF(stlUInt16)) * sRowCount)
                <= sPageFreeSize )
            {
                sAccColSize += sColLen + sColLenSize + STL_SIZEOF(stlUInt16);
            }
            else
            {
                sMemberPageCount++;
                sAccColSize = SMDMPF_ROW_HDR_LEN + sColLen + sColLenSize;
                sSubColCount = 0;
            }
            sSubColCount++;
        }

        sMemberPageCount++;
    }

    STL_TRY_THROW( sMemberPageCount == 1, RAMP_ERR_COLUMNAR_TABLE_INVALID_ROW_SIZE );

    *aMemberPageCount = sMemberPageCount;
    *aColLengthArray  = sColLengthArray;
    *aRowCount        = sRowCount;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_MIN_ROW_VIOLATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_MIN_ROW_VIOLATION,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_COLUMNAR_TABLE_INVALID_ROW_SIZE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_COLUMNAR_TABLE_INVALID_ROW_SIZE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    switch( sState )
    {
        case 1:
            smgFreeShmMem4Open( sColLengthArray, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief Columnar Table의 table header에 column 정보를 기록한다.
 * @param[in] aRelationHandle Table Header
 * @param[in] aColumnMeta Table Column Meta Info
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smdmpfBuildCache( void    * aRelationHandle,
                            void    * aColumnMeta,
                            smlEnv  * aEnv )
{
    stlInt32         * sLastColIdArray = NULL;
    stlInt32           sPageSeq;
    stlInt16           sPageFreeSize;
    stlInt16           sAccColSize;
    stlInt32           sMemberPageCount;
    stlInt16           i;
    stlUInt8           sColLenSize;
    stlInt32           sColCount;
    stlInt16         * sColLengthArray;
    stlInt16           sRowCount;
    stlInt16           sThreshold;
    stlInt64           sColLen;
    stlInt32           sSubColCount;
    smdmpfColMetaHdr   sColHdr;
    smlRid             sSegRid;
    stlUInt16          sOffset;
    stlChar          * sPagePtr;
    stlUInt8           sColLengthSize;
    stlInt32           sColIdx;
    stlUInt16          sLength16;
    stlInt32           sPageType;
    smpHandle          sPageHandle;
    smlPid             sNextColPageId;
    stlInt32           sState = 0;

    sColCount = SMD_GET_COL_COUNT( aRelationHandle );
    sRowCount = SMD_GET_ROW_COUNT( aRelationHandle );
    sThreshold = SMD_GET_CONT_COLUMN_THRESHOLD( aRelationHandle );

    STL_DASSERT( sColCount > 0 );

    if( aColumnMeta != NULL )
    {
        sColLengthArray = (stlInt16 *)aColumnMeta;
    }
    else
    {
        sSegRid = smsGetSegRid( SME_GET_SEGMENT_HANDLE(aRelationHandle) );

        STL_TRY( smpFix( SMS_SEGMENT_MAP_RID.mTbsId,
                         sSegRid.mPageId,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );
        sState = 1;

        STL_TRY( smgAllocShmMem4Open( STL_SIZEOF(stlInt16) * sColCount,
                                      (void**)&sColLengthArray,
                                      aEnv ) == STL_SUCCESS );

        sPagePtr = smpGetBody(SMP_FRAME(&sPageHandle)) +
                   smsGetHeaderSize( SME_GET_SEGMENT_HANDLE(aRelationHandle) ) +
                   STL_SIZEOF(smdTableHeader);

        /* Startup시 relation header 다음에 기록된 column length를 읽는다. */
        stlMemcpy( (void *)&sColHdr,
                   sPagePtr,
                   STL_SIZEOF(smdmpfColMetaHdr) );

        sPagePtr += STL_SIZEOF(smdmpfColMetaHdr);
        sOffset = sPagePtr - (stlChar *)SMP_FRAME(&sPageHandle);

        sNextColPageId = sColHdr.mNextPageId;
        sColIdx = 0;

        while( STL_TRUE )
        {
            sPagePtr = (stlChar *)SMP_FRAME(&sPageHandle);

            while( sOffset < sColHdr.mHighWaterMark )
            {
                /* col length size를 구한다. */
                sColLengthSize = *(stlUInt8 *)(sPagePtr + sOffset);
                sOffset += 1;

                if( sColLengthSize <= SMP_COLUMN_LEN_1BYTE_MAX )
                {
                    sColLengthArray[sColIdx] = sColLengthSize;
                }
                else
                {
                    STL_DASSERT( sColLengthSize == SMP_COLUMN_LEN_2BYTE_FLAG );

                    STL_WRITE_INT16( &sLength16, sPagePtr + sOffset );
                    sColLengthArray[sColIdx] = sLength16;
                    sOffset += 2;
                }

                sColIdx++;
            }

            sState = 0;
            STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );

            if( sNextColPageId == SMP_NULL_PID )
            {
                break;
            }

            STL_TRY( smpFix( SMS_SEGMENT_MAP_RID.mTbsId,
                             sNextColPageId,
                             &sPageHandle,
                             aEnv ) == STL_SUCCESS );
            sState = 1;

            sPagePtr = smpGetBody(SMP_FRAME(&sPageHandle));

            STL_WRITE_INT32( &sPageType, sPagePtr );

            sPagePtr += STL_SIZEOF( stlInt32 );

            /* Column info를 저장한 seg hdr page */
            STL_DASSERT( smsIsSegHdrCollenPage(sPageType) == STL_TRUE );

            stlMemcpy( (void *)&sColHdr,
                       sPagePtr,
                       STL_SIZEOF(smdmpfColMetaHdr) );

            sPagePtr += STL_SIZEOF(smdmpfColMetaHdr);
            
            sOffset = sPagePtr - (stlChar *)SMP_FRAME(&sPageHandle);

            sNextColPageId = sColHdr.mNextPageId;
        }
    }

    /**
     * row를 저장하기 위해 필요한 data page 수를 구한다.
     */
    sMemberPageCount = SMD_GET_MEMBER_PAGE_COUNT( aRelationHandle );

    if( sMemberPageCount != 1 )
    {
        STL_TRY( smgAllocShmMem4Open( STL_SIZEOF( stlInt32 ) * sMemberPageCount,
                                      (void**)&sLastColIdArray,
                                      aEnv ) == STL_SUCCESS );

        sPageFreeSize = SMDMPF_PAGE_MAX_FREE_SIZE( 0 );
        sAccColSize   = SMDMPF_ROW_HDR_LEN;
        sPageSeq      = 0;

        for( sSubColCount = 0, i = 0; i < sColCount; i++ )
        {
            /**
             * Member page에 저장 가능한 column의 수는
             * SMD_MAX_COLUMN_COUNT_IN_PIECE 보다 작거나 같다.
             */
            if( SMD_MAX_COLUMN_COUNT_IN_PIECE <= sSubColCount)
            {
                sLastColIdArray[sPageSeq] = i - 1;
                sAccColSize = SMDMPF_ROW_HDR_LEN;
                sPageSeq++;
                sSubColCount = 0;
            }

            sColLen = sColLengthArray[i];

            if( sColLen > sThreshold )
            {
                sColLen = sThreshold;
            }
            SMP_GET_COLUMN_LEN_SIZE( sColLen, &sColLenSize );

            if( ((sAccColSize + sColLen + sColLenSize + STL_SIZEOF(stlUInt16)) * sRowCount)
                <= sPageFreeSize )
            {
                sAccColSize += sColLen + sColLenSize + STL_SIZEOF(stlUInt16);
            }
            else
            {
                sLastColIdArray[sPageSeq] = i - 1;
                sAccColSize = SMDMPF_ROW_HDR_LEN + sColLen + sColLenSize;
                sPageSeq++;
                sSubColCount = 0;
            }

            sSubColCount++;
        }

        sLastColIdArray[sPageSeq] = i - 1;
    }

    SMD_SET_LAST_COL_ID_ARRAY( aRelationHandle, sLastColIdArray );
    SMD_SET_COL_LENGTH_ARRAY( aRelationHandle, sColLengthArray );

    return STL_SUCCESS;     
    
    STL_FINISH;

    if( sLastColIdArray != NULL )
    {
        smgFreeShmMem4Open( sLastColIdArray, aEnv );
    }

    if( aColumnMeta == NULL )
    {
        if( sColLengthArray != NULL )
        {
            smgFreeShmMem4Open( sColLengthArray, aEnv );
        }
    }

    switch( sState )
    {
        case 1:
            (void) smpUnfix( &sPageHandle, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus smdmpfDestroyColInfo( void    * aColLengthArray,
                                smlEnv  * aEnv )
{
    STL_TRY( smgFreeShmMem4Open( aColLengthArray, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}    

stlStatus smdmpfDestroyCache( void    * aRelationHandle,
                              smlEnv  * aEnv )
{
    if( SMD_GET_COL_LENGTH_ARRAY( aRelationHandle ) != NULL )
    {
        STL_TRY( smgFreeShmMem4Open( SMD_GET_COL_LENGTH_ARRAY( aRelationHandle ),
                                     aEnv )
                 == STL_SUCCESS );

        SMD_SET_COL_LENGTH_ARRAY( aRelationHandle, NULL );
    }

    if( SMD_GET_LAST_COL_ID_ARRAY( aRelationHandle ) != NULL )
    {
        STL_TRY( smgFreeShmMem4Open( SMD_GET_LAST_COL_ID_ARRAY( aRelationHandle ),
                                     aEnv )
                 == STL_SUCCESS );

        SMD_SET_LAST_COL_ID_ARRAY( aRelationHandle, NULL );
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( SMD_GET_COL_LENGTH_ARRAY( aRelationHandle ) != NULL )
    {
        (void) smgFreeShmMem4Open( SMD_GET_COL_LENGTH_ARRAY( aRelationHandle ),
                                   aEnv );
        SMD_SET_COL_LENGTH_ARRAY( aRelationHandle, NULL );
    }

    if( SMD_GET_LAST_COL_ID_ARRAY( aRelationHandle ) != NULL )
    {
        (void) smgFreeShmMem4Open( SMD_GET_LAST_COL_ID_ARRAY( aRelationHandle ),
                                   aEnv );
        SMD_SET_LAST_COL_ID_ARRAY( aRelationHandle, NULL );
    }

    return STL_FAILURE;
}    

/**
 * @brief 테이블을 생성한다.
 * @param[in] aStatement Statement 객체
 * @param[in] aTbsId 생성할 테이블이 속할 Tablespace ID
 * @param[in] aAttr 생성할 테이블 속성을 지정한 구조체
 * @param[in] aLoggingUndo Undo 로그의 기록 여부
 * @param[out] aSegmentId 생성된 테이블의 물리적 아이디
 * @param[out] aRelationHandle 생성된 테이블을 조작할때 사용할 메모리 핸들
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smdmpfCreate( smlStatement  * aStatement,
                        smlTbsId        aTbsId,
                        smlTableAttr  * aAttr,
                        stlBool         aLoggingUndo,
                        stlInt64      * aSegmentId,
                        void         ** aRelationHandle,
                        smlEnv        * aEnv )
{
    smxlTransId     sTransId = SMA_GET_TRANS_ID(aStatement);
    smlRid          sSegmentRid;
    void          * sSegmentHandle = NULL;
    smdTablePendArg sPendArg;
    stlUInt32       sAttr = SMXM_ATTR_REDO;
    smlRid          sUndoRid;
    smxmTrans       sMiniTrans;
    stlInt32        sState = 0;

    STL_TRY( smxmBegin( &sMiniTrans,
                        sTransId,
                        SML_INVALID_RID,
                        sAttr,
                        aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( smsAllocSegHdr( &sMiniTrans,
                             aTbsId,
                             SML_SEG_TYPE_MEMORY_BITMAP,
                             &aAttr->mSegAttr,
                             STL_TRUE,
                             &sSegmentRid,
                             &sSegmentHandle,
                             aEnv ) == STL_SUCCESS );

    smxmSetSegmentRid( &sMiniTrans, sSegmentRid );
    sTransId = SMA_GET_TRANS_ID(aStatement);
    
    /*
     * Undo Logging
     */

    if( aLoggingUndo == STL_TRUE )
    {
        STL_TRY( smxlInsertUndoRecord( &sMiniTrans,
                                       SMG_COMPONENT_TABLE,
                                       SMD_UNDO_LOG_MEMORY_HEAP_CREATE,
                                       NULL,
                                       0,
                                       sSegmentRid,
                                       &sUndoRid,
                                       aEnv ) == STL_SUCCESS );
    }

    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    /**
     * smsCreate에 대한 rollback은 logical로 처리된다.
     */
    STL_TRY( smsCreate( sTransId,
                        aTbsId,
                        SML_SEG_TYPE_MEMORY_BITMAP,
                        sSegmentHandle,
                        0,  /* aInitialExtents(optimal) */
                        aEnv ) == STL_SUCCESS );

    stlMemset( &sPendArg, 0x00, STL_SIZEOF(smdTablePendArg) );

    STL_TRY( smdmpfInitTableHeader( aStatement,
                                    &sSegmentRid,
                                    sSegmentHandle,
                                    aAttr,
                                    aRelationHandle,
                                    aEnv ) == STL_SUCCESS );

    sPendArg.mHandle = *aRelationHandle;
    sPendArg.mTransId = sTransId;

    STL_TRY( smgAddPendOp( aStatement,
                           SMG_PEND_CREATE_TABLE,
                           SML_PEND_ACTION_COMMIT,
                           (void*)&sPendArg,
                           STL_SIZEOF( smdTablePendArg ),
                           0,
                           aEnv ) == STL_SUCCESS );

    STL_TRY( smgAddPendOp( aStatement,
                           SMG_PEND_DROP_TABLE,
                           SML_PEND_ACTION_ROLLBACK,
                           (void*)&sPendArg,
                           STL_SIZEOF( smdTablePendArg ),
                           28,
                           aEnv ) == STL_SUCCESS );
    
    STL_TRY( smgAddPendOp( aStatement,
                           SMG_PEND_FREE_LOCK,
                           SML_PEND_ACTION_ROLLBACK,
                           (void*)&sPendArg,
                           STL_SIZEOF( smdTablePendArg ),
                           24,
                           aEnv ) == STL_SUCCESS );

    *aSegmentId = smsGetSegmentId( sSegmentHandle );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)smxmRollback( &sMiniTrans, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief 테이블을 삭제한다.
 * @param[in] aStatement Statement 객체
 * @param[in] aRelationHandle 삭제할 테이블의 relation handle
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smdmpfDrop( smlStatement * aStatement,
                      void         * aRelationHandle,
                      smlEnv       * aEnv )
{
    smdTablePendArg  sPendArg;
    smxlTransId      sTransId = SMA_GET_TRANS_ID(aStatement);
    stlUInt32        sAttr = SMXM_ATTR_REDO;
    smlRid           sUndoRid;
    smxmTrans        sMiniTrans;
    smpHandle        sPageHandle;
    stlInt16         sOffset;
    stlUInt16        sData[2];
    smdTableHeader * sTableHeader;
    smlRid           sSegmentRid;
    void           * sSegHandle;
    stlInt32         sState = 0;

    sSegHandle = SME_GET_SEGMENT_HANDLE( aRelationHandle );
    sSegmentRid = smsGetSegRid( sSegHandle );
    
    /**
     * 1. Undo Logging
     */
    
    STL_TRY( smxmBegin( &sMiniTrans,
                        sTransId,
                        sSegmentRid,
                        sAttr,
                        aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( smpAcquire( &sMiniTrans,
                         sSegmentRid.mTbsId,
                         sSegmentRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

    sTableHeader = (smdTableHeader*)( smpGetBody(SMP_FRAME(&sPageHandle)) +
                                      smsGetHeaderSize( SME_GET_SEGMENT_HANDLE(aRelationHandle)) );

    STL_ASSERT( sTableHeader->mRelHeader.mRelState == SME_RELATION_STATE_ACTIVE );
    sOffset = ( (stlChar*)&sTableHeader->mRelHeader.mRelState -
                (stlChar*)SMP_FRAME(&sPageHandle) );

    sData[0] = sTableHeader->mRelHeader.mRelState;
    sData[1] = SME_RELATION_STATE_DROPPING;

    STL_TRY( smxmWriteLog( &sMiniTrans,
                           SMG_COMPONENT_RECOVERY,
                           SMR_LOG_BYTES,
                           SMR_REDO_TARGET_PAGE,
                           (void*)sData,
                           STL_SIZEOF( stlUInt16 ) * 2,
                           sSegmentRid.mTbsId,
                           sSegmentRid.mPageId,
                           sOffset,
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    STL_TRY( smxlInsertUndoRecord( &sMiniTrans,
                                   SMG_COMPONENT_TABLE,
                                   SMD_UNDO_LOG_MEMORY_HEAP_DROP,
                                   NULL,
                                   0,
                                   sSegmentRid,
                                   &sUndoRid,
                                   aEnv ) == STL_SUCCESS );

    sTableHeader->mRelHeader.mRelState = SME_RELATION_STATE_DROPPING;
    sTableHeader->mRelHeader.mDropTransId = sTransId;
    
    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    sTransId = SMA_GET_TRANS_ID(aStatement);
    
    /**
     * 2. Update Relation Cache
     */
    
    SME_SET_RELATION_STATE( aRelationHandle, SME_RELATION_STATE_DROPPING );
    ((smdTableHeader*)aRelationHandle)->mRelHeader.mDropTransId = sTransId;
    
    /**
     * 3. Add Pending Operations
     */
    
    stlMemset( &sPendArg, 0x00, STL_SIZEOF(smdTablePendArg) );
    sPendArg.mHandle  = aRelationHandle;
    sPendArg.mTransId = sTransId;

    STL_TRY( smgAddPendOp( aStatement,
                           SMG_PEND_DROP_TABLE,
                           SML_PEND_ACTION_COMMIT,
                           (void*)&sPendArg,
                           STL_SIZEOF( smdTablePendArg ),
                           28,
                           aEnv ) == STL_SUCCESS );

    STL_TRY( smgAddPendOp( aStatement,
                           SMG_PEND_FREE_LOCK,
                           SML_PEND_ACTION_COMMIT,
                           (void*)&sPendArg,
                           STL_SIZEOF( smdTablePendArg ),
                           24,
                           aEnv ) == STL_SUCCESS );

    /**
     * 4. Offline Tablespace에 속한 테이블이라면 Pending Operation으로 처리한다.
     */
    
    if( SMF_IS_ONLINE_TBS( smsGetTbsId( sSegHandle ) ) == STL_FALSE )
    {
        STL_TRY( smdInsertPendingOperation( sTransId,
                                            smsGetTbsId( sSegHandle ),
                                            SMD_PENDING_OPERATION_DROP_SEGMENT,
                                            sSegHandle,
                                            smsGetCacheSize( sSegHandle ),
                                            aEnv )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)smxmRollback( &sMiniTrans, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}


/**
 * @brief Truncate를 위해서 새로운 테이블을 생성한다.
 * @param[in] aStatement Statement 객체
 * @param[in] aOrgRelHandle Truncate 이전의 원본 Relation Handle
 * @param[in] aDropStorageOption extent의 drop 방법 지정
 * @param[out] aNewRelHandle 생성된 Relation Handle
 * @param[out] aNewSegmentId 생성된 Segment의 Identifier
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smdmpfCreateForTruncate( smlStatement          * aStatement,
                                   void                  * aOrgRelHandle,
                                   smlDropStorageOption    aDropStorageOption,
                                   void                 ** aNewRelHandle,
                                   stlInt64              * aNewSegmentId,
                                   smlEnv                * aEnv )
{
    void            * sOrgSegHandle;
    void            * sNewSegHandle;
    smdTablePendArg   sPendArg;
    smxmTrans         sMiniTrans;
    stlInt32          sState = 0;
    smlRid            sNewSegmentRid;
    smlRid            sUndoRid;
    stlUInt32         sAttr = SMXM_ATTR_REDO;
    smxlTransId       sTransId;
    stlInt32          sInitialExtents = SMS_MIN_EXTENT_COUNT;
        
    sOrgSegHandle = SME_GET_SEGMENT_HANDLE( aOrgRelHandle );
    sTransId = SMA_GET_TRANS_ID( aStatement );
    
    STL_TRY( smxmBegin( &sMiniTrans,
                        sTransId,
                        SML_INVALID_RID,
                        sAttr,
                        aEnv ) == STL_SUCCESS );
    sState = 1;

    /*
     * Create New Segment Header
     */
    STL_TRY( smsAllocSegHdrForTruncate( &sMiniTrans,
                                        sOrgSegHandle,
                                        &sNewSegmentRid,
                                        &sNewSegHandle,
                                        aEnv ) == STL_SUCCESS );

    smxmSetSegmentRid( &sMiniTrans, sNewSegmentRid );
    sTransId = SMA_GET_TRANS_ID( aStatement );
    
    /*
     * New Segment Undo Logging
     */
    STL_TRY( smxlInsertUndoRecord( &sMiniTrans,
                                   SMG_COMPONENT_TABLE,
                                   SMD_UNDO_LOG_MEMORY_HEAP_CREATE,
                                   NULL,
                                   0,
                                   sNewSegmentRid,
                                   &sUndoRid,
                                   aEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
    
    /*
     * Create New Segment
     */
    if( aDropStorageOption == SML_DROP_STORAGE_DROP )
    {
        sInitialExtents = smsGetMinExtents( sOrgSegHandle );
    }
    
    STL_TRY( smsCreate( sTransId,
                        smsGetTbsId(sOrgSegHandle),
                        SML_SEG_TYPE_MEMORY_BITMAP,
                        sNewSegHandle,
                        sInitialExtents,
                        aEnv ) == STL_SUCCESS );
    
    STL_TRY( smdmpfInitHeaderForTruncate( aStatement,
                                          aOrgRelHandle,
                                          sNewSegHandle,
                                          aNewRelHandle,
                                          aEnv ) == STL_SUCCESS );
    
    /*
     * Add Pending Operations
     */
    stlMemset( &sPendArg, 0x00, STL_SIZEOF(smdTablePendArg) );
    sPendArg.mHandle  = *aNewRelHandle;
    sPendArg.mTransId = sTransId;

    STL_TRY( smgAddPendOp( aStatement,
                           SMG_PEND_CREATE_TABLE,
                           SML_PEND_ACTION_COMMIT,
                           (void*)&sPendArg,
                           STL_SIZEOF( smdTablePendArg ),
                           0,
                           aEnv ) == STL_SUCCESS );

    STL_TRY( smgAddPendOp( aStatement,
                           SMG_PEND_DROP_TABLE,
                           SML_PEND_ACTION_ROLLBACK,
                           (void*)&sPendArg,
                           STL_SIZEOF( smdTablePendArg ),
                           28,
                           aEnv ) == STL_SUCCESS );
    
    STL_TRY( smgAddPendOp( aStatement,
                           SMG_PEND_FREE_LOCK,
                           SML_PEND_ACTION_ROLLBACK,
                           (void*)&sPendArg,
                           STL_SIZEOF( smdTablePendArg ),
                           24,
                           aEnv ) == STL_SUCCESS );

    *aNewSegmentId = smsGetSegmentId( sNewSegHandle );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)smxmRollback( &sMiniTrans, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus smdmpfInitHeaderForTruncate( smlStatement  * aStatement,
                                       void          * aOrgRelHandle,
                                       void          * aNewSegHandle,
                                       void         ** aNewRelHandle,
                                       smlEnv        * aEnv )
{
    smxmTrans             sMiniTrans;
    stlUInt32             sAttr = SMXM_ATTR_REDO;
    void                * sCaches[2];
    stlUInt32             sState = 0;
    void                * sLockHandle = NULL;
    void                * sOrgSegHandle = NULL;
    smlRid                sOrgSegmentRid;
    smpHandle             sOrgPageHandle;
    smdTableHeader      * sOrgTableHeader;
    smpHandle             sNewPageHandle;
    smdTableHeader      * sNewTableHeader;
    smlRid                sNewSegmentRid;
    smlColumnarAttr       sColumnarAttr;
    void                * sColLengthArray = NULL;
    stlInt32              sMemberPageCount;
    stlInt16              sRowCount;
    stlBool               sBuildCache = STL_FALSE;

    sOrgSegHandle = SME_GET_SEGMENT_HANDLE( aOrgRelHandle );
    sOrgSegmentRid = smsGetSegRid( sOrgSegHandle );
    sNewSegmentRid = smsGetSegRid( aNewSegHandle );

    sColumnarAttr.mMinRowCount = SMD_GET_ROW_COUNT( aOrgRelHandle );
    sColumnarAttr.mContColumnThreshold = SMD_GET_CONT_COLUMN_THRESHOLD( aOrgRelHandle );
    sColumnarAttr.mColCount = SMD_GET_COL_COUNT( aOrgRelHandle );
    sColumnarAttr.mColLengthArray = SMD_GET_COL_LENGTH_ARRAY( aOrgRelHandle );

    STL_TRY( smdmpfBuildColInfo( &sColumnarAttr,
                                 &sMemberPageCount,
                                 &sColLengthArray,
                                 &sRowCount,
                                 aEnv ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( smxmBegin( &sMiniTrans,
                        SMA_GET_TRANS_ID( aStatement ),
                        sNewSegmentRid,
                        sAttr,
                        aEnv ) == STL_SUCCESS );
    sState = 2;
    
    STL_TRY( smpAcquire( &sMiniTrans,
                         sOrgSegmentRid.mTbsId,
                         sOrgSegmentRid.mPageId,
                         KNL_LATCH_MODE_SHARED,
                         &sOrgPageHandle,
                         aEnv ) == STL_SUCCESS );

    sOrgTableHeader = (smdTableHeader*)( smpGetBody(SMP_FRAME(&sOrgPageHandle)) +
                                         smsGetHeaderSize( sOrgSegHandle ) );

    STL_TRY( smpAcquire( &sMiniTrans,
                         sNewSegmentRid.mTbsId,
                         sNewSegmentRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sNewPageHandle,
                         aEnv ) == STL_SUCCESS );

    sNewTableHeader = (smdTableHeader*)( smpGetBody(SMP_FRAME(&sNewPageHandle)) +
                                         smsGetHeaderSize(aNewSegHandle) );

    stlMemcpy( sNewTableHeader, sOrgTableHeader, STL_SIZEOF(smdTableHeader) );
    SME_SET_OBJECT_SCN( sNewTableHeader, SML_INFINITE_SCN );
    SME_SET_RELATION_STATE( sNewTableHeader, SME_RELATION_STATE_ACTIVE );
    SMD_SET_LOCK_HANDLE( sNewTableHeader, NULL );

    STL_TRY( smxmWriteLog( &sMiniTrans,
                           SMG_COMPONENT_RELATION,
                           SMR_LOG_INIT_REL_HEADER,
                           SMR_REDO_TARGET_PAGE,
                           (void*)sNewTableHeader,
                           STL_SIZEOF(smdTableHeader),
                           sNewSegmentRid.mTbsId,
                           sNewSegmentRid.mPageId,
                           (stlChar*)sNewTableHeader - (stlChar*)SMP_FRAME(&sNewPageHandle),
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    STL_TRY( smgAllocShmMem4Open( STL_SIZEOF( smdTableHeader ),
                                  aNewRelHandle,
                                  aEnv ) == STL_SUCCESS );
    sState = 3;

    SME_SET_SEGMENT_HANDLE( sNewTableHeader, aNewSegHandle );
    stlMemcpy( *aNewRelHandle, sNewTableHeader, STL_SIZEOF(smdTableHeader) );

    sCaches[0] = aNewSegHandle;
    sCaches[1] = *aNewRelHandle;
    smpSetVolatileArea( &sNewPageHandle, &sCaches, STL_SIZEOF(void*) * 2 );

    if( sNewTableHeader->mLockingFlag == SML_TABLE_LOGGING_YES )
    {
        STL_TRY( smklAllocLockHandle( *aNewRelHandle,
                                      &sLockHandle,
                                      aEnv ) == STL_SUCCESS );
        sState = 4;
        
        SMD_SET_LOCK_HANDLE( *aNewRelHandle, sLockHandle );
    }

    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    /* Table의 column length를 먼저 기록하고 relation header를 기록한다. */
    STL_TRY( smdmpfWriteColumnLength( aStatement,
                                      &sNewSegmentRid,
                                      aNewSegHandle,
                                      &sColumnarAttr,
                                      aEnv ) == STL_SUCCESS );

    STL_TRY( smdmpfBuildCache( *aNewRelHandle,
                               sColLengthArray,
                               aEnv ) == STL_SUCCESS );
    sBuildCache = STL_TRUE;

    return STL_SUCCESS;

    STL_FINISH;

    if( sBuildCache == STL_TRUE )
    {
        (void) smdmpfDestroyCache( *aNewRelHandle, aEnv );
        sColLengthArray = NULL;
    }

    switch( sState )
    {
        case 4:
            (void)smklFreeLockHandle( *aNewRelHandle, aEnv );
        case 3:
            sCaches[0] = aNewSegHandle;
            sCaches[1] = NULL;
            smpSetVolatileArea( &sNewPageHandle, &sCaches, STL_SIZEOF(void*) * 2 );
            (void)smgFreeShmMem4Open( *aNewRelHandle, aEnv );
        case 2:
            (void)smxmRollback( &sMiniTrans, aEnv );
        case 1:
            if( sColLengthArray != NULL )
            {
                (void) smdmpfDestroyColInfo( sColLengthArray, aEnv );
                sColLengthArray = NULL;
            }
        default :
            break;
    }

    return STL_FAILURE;
}

stlStatus smdmpfInitialize( smlStatement * aStatement,
                            void         * aRelationHandle,
                            smlEnv       * aEnv )
{
    return STL_SUCCESS;
}


stlStatus smdmpfFinalize( smlStatement * aStatement,
                          void         * aRelationHandle,
                          smlEnv       * aEnv )
{
    return STL_SUCCESS;
}

stlStatus smdmpfSort( void * RelationHandle, void * aTransactionHandle )
{
	return STL_SUCCESS;
}

stlStatus smdmpfBuild( void * RelationHandle, void * aTransactionHandle )
{
	return STL_SUCCESS;
}

stlStatus smdmpfDropAging( smxlTransId   aTransId,
                           void        * aRelationHandle,
                           stlBool       aOnStartup,
                           smlEnv      * aEnv )
{
    void * sRelationHandle;
    
    /**
     * 롤백하다가 비정상 종료된 경우에는 Segment는 Drop되었지만
     * Undo Record에 SMXL_UNDO_RECORD_DELETED가 설정되지 않는 경우가 있다.
     * 이러한 경우에는 Undo를 Skip한다.
     */
    if( aRelationHandle != NULL )
    {
        sRelationHandle = aRelationHandle;
        
        /**
         * Segment를 물리적으로 삭제한다.
         */
        STL_TRY( smsDrop( aTransId,
                          SME_GET_SEGMENT_HANDLE(aRelationHandle),
                          aOnStartup,
                          aEnv ) == STL_SUCCESS );

        /**
         * startup이 아닌 경우는 lock handle을 참조하고 있는 트랜잭션이 존재할수 있기 때문에
         * Ager에 의해서 처리되어야 한다.
         */
        if( aOnStartup == STL_TRUE )
        {
            /**
             * Lock Handle을 삭제한다.
             */
            if( SMD_GET_LOCK_HANDLE(sRelationHandle) != NULL )
            {
                STL_TRY( smklFreeLockHandle( sRelationHandle,
                                             aEnv ) == STL_SUCCESS );
            }
        }

        /**
         * Column info를 위한 cache를 삭제한다.
         */
        STL_TRY( smdmpfDestroyCache( aRelationHandle, aEnv ) == STL_SUCCESS );

        /**
         * Relation Cache를 삭제한다.
         */
        STL_TRY( smgFreeShmMem4Open( sRelationHandle,
                                     aEnv ) == STL_SUCCESS );
    }
                                    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmpfCreateUndo( smxlTransId   aTransId,
                            void        * aRelationHandle,
                            smlEnv      * aEnv )
{
    smxlTransId      sTransId = aTransId;
    stlUInt32        sAttr = SMXM_ATTR_REDO;
    smxmTrans        sMiniTrans;
    smpHandle        sPageHandle;
    stlInt16         sOffset;
    stlUInt16        sData[2];
    smdTableHeader * sTableHeader;
    smlRid           sSegmentRid;
    void           * sSegHandle;
    stlInt32         sState = 0;

    sSegHandle = SME_GET_SEGMENT_HANDLE( aRelationHandle );
    sSegmentRid = smsGetSegRid( sSegHandle );
    
    /*
     * 1. Undo Logging
     */
    STL_TRY( smxmBegin( &sMiniTrans,
                        sTransId,
                        sSegmentRid,
                        sAttr,
                        aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( smpAcquire( &sMiniTrans,
                         sSegmentRid.mTbsId,
                         sSegmentRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

    sTableHeader = (smdTableHeader*)( smpGetBody(SMP_FRAME(&sPageHandle)) +
                                      smsGetHeaderSize( SME_GET_SEGMENT_HANDLE(aRelationHandle)) );

    STL_ASSERT( sTableHeader->mRelHeader.mRelState == SME_RELATION_STATE_ACTIVE );
    sOffset = ( (stlChar*)&sTableHeader->mRelHeader.mRelState -
                (stlChar*)SMP_FRAME(&sPageHandle) );

    sData[0] = sTableHeader->mRelHeader.mRelState;
    sData[1] = SME_RELATION_STATE_DROPPING;

    STL_TRY( smxmWriteLog( &sMiniTrans,
                           SMG_COMPONENT_RECOVERY,
                           SMR_LOG_BYTES,
                           SMR_REDO_TARGET_PAGE,
                           (void*)sData,
                           STL_SIZEOF( stlUInt16 ) * 2,
                           sSegmentRid.mTbsId,
                           sSegmentRid.mPageId,
                           sOffset,
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );
    
    sTableHeader->mRelHeader.mRelState = SME_RELATION_STATE_DROPPING;
    sTableHeader->mRelHeader.mDropTransId = sTransId;
    
    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    /*
     * 2. Update Relation Cache
     */
    SME_SET_RELATION_STATE( aRelationHandle, SME_RELATION_STATE_DROPPING );
    ((smdTableHeader*)aRelationHandle)->mRelHeader.mDropTransId = sTransId;
    
    /**
     * Restart Recovery 단계에서 drop aging은 ager에 넘기지 않고 직접 처리한다.
     * - Restart Undo시에 Ager Transaction이 Active Transaction으로 인식되어 Rollback될수 있다.
     */
    if( (smfGetServerState() == SML_SERVER_STATE_RECOVERING) ||
        (smrProceedMediaRecovery(aEnv) == STL_TRUE) )
    {
        /**
         * Offline Tablespace에 속한 테이블이라면 Pending Operation으로 처리한다.
         */
        if( SMF_IS_ONLINE_TBS( smsGetTbsId( sSegHandle ) ) == STL_FALSE )
        {
            STL_TRY( smdInsertPendingOperation( sTransId,
                                                smsGetTbsId( sSegHandle ),
                                                SMD_PENDING_OPERATION_DROP_SEGMENT,
                                                sSegHandle,
                                                smsGetCacheSize( sSegHandle ),
                                                aEnv )
                     == STL_SUCCESS );
        }
            
        STL_TRY( smdmpfDropAging( sTransId,
                                  aRelationHandle,
                                  STL_TRUE,    /* on startup */
                                  aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) smxmCommit( &sMiniTrans, aEnv );
    }

    return STL_FAILURE;
}


stlStatus smdmpfDropColumns( smlStatement      * aStatement,
                             void              * aRelationHandle,
                             knlValueBlockList * aDropColumns,
                             smlEnv            * aEnv )
{
    return STL_SUCCESS;
}

stlStatus smdmpfCopy( smlStatement      * aStatement,
                      void              * aSrcRelHandle,
                      void              * aDstRelHandle,
                      knlValueBlockList * aColumnList,
                      smlEnv            * aEnv )
{
    return STL_FAILURE;
}

stlStatus smdmpfAlterTableAttr( smlStatement  * aStatement,
                                void          * aRelationHandle,
                                smlTableAttr  * aTableAttr,
                                smlIndexAttr  * aIndexAttr,
                                smlEnv        * aEnv )
{
    return STL_FAILURE;
}

stlStatus smdmpfMergeTableAttr( void          * aRelationHandle,
                                smlTableAttr  * aTableAttr,
                                smlEnv        * aEnv )
{
    return STL_FAILURE;
}

stlStatus smdmpfCheckExistRow( smlStatement * aStatement,
                               void         * aRelationHandle,
                               stlBool      * aExistRow,
                               smlEnv       * aEnv )
{
    void               * sSegHandle;
    void               * sSegIterator;
    smlPid               sCurPid;
    stlChar              sPageBuf[SMP_PAGE_SIZE + STL_SIZEOF(stlInt64)];
    stlChar            * sFrame;
    smlTbsId             sTbsId;
    smpHandle            sPageHandle;
    smpRowItemIterator   sSlotIterator;
    stlChar            * sRow;
    stlInt32             sState = 0;

    *aExistRow = STL_FALSE;
    
    sFrame = (stlChar*)STL_ALIGN( (stlInt64)sPageBuf, STL_SIZEOF(stlInt64) );
    
    sSegHandle = SME_GET_SEGMENT_HANDLE( aRelationHandle );
    sTbsId = smsGetTbsId( sSegHandle );
    
    STL_TRY( smsAllocSegIterator( aStatement,
                                  sSegHandle,
                                  &sSegIterator,
                                  aEnv ) == STL_SUCCESS );
    sState = 1;
    
    /**
     * Iterating page
     */
    
    STL_TRY( smsGetFirstPage( sSegHandle,
                              sSegIterator,
                              &sCurPid,
                              aEnv ) == STL_SUCCESS );

    while( sCurPid != SMP_NULL_PID )
    {
        /**
         * Query Timeout 검사
         */
        STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );
        
        STL_TRY( smpAcquire( NULL,
                             sTbsId,
                             sCurPid,
                             KNL_LATCH_MODE_SHARED,
                             &sPageHandle,
                             aEnv ) == STL_SUCCESS );
        sState = 2;

        stlMemcpy( sFrame, SMP_FRAME( &sPageHandle ), SMP_PAGE_SIZE );
        
        sState = 1;
        STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );

        /**
         * Iterating slot
         */
        SMP_MAKE_DUMMY_HANDLE( &sPageHandle, sFrame );

        SMDMPF_INIT_ROWITEM_ITERATOR( SMP_FRAME(&sPageHandle), &sSlotIterator );
        
        SMDMPF_FOREACH_ROWITEM_ITERATOR( &sSlotIterator )
        {
            sRow = (stlChar*)sSlotIterator.mCurSlot;
            
            if( SMDMPF_IS_IN_USE( sRow ) != STL_TRUE )
            {
                continue;
            }
        
            if( SMDMPF_IS_DELETED( sRow ) != STL_TRUE )
            {
                *aExistRow = STL_TRUE;
                STL_THROW( RAMP_FINISH );
            }
        }

        STL_TRY( smsGetNextPage( sSegHandle,
                                 sSegIterator,
                                 &sCurPid,
                                 aEnv ) == STL_SUCCESS );
    }

    STL_RAMP( RAMP_FINISH );

    sState = 0;
    STL_TRY( smsFreeSegIterator( sSegHandle,
                                 sSegIterator,
                                 aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) smpRelease( &sPageHandle, aEnv );
        case 1:
            (void) smsFreeSegIterator( sSegHandle,
                                       sSegIterator,
                                       aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}



/** @} */
