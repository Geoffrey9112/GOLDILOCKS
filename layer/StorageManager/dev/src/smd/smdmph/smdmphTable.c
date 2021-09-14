/*******************************************************************************
 * smdmphTable.c
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
#include <smdmphRowDef.h>
#include <smdmphDef.h>
#include <smdmph.h>
#include <smdDef.h>

/**
 * @file smdmphTable.c
 * @brief Storage Manager Layer Memory Heap Table Create/Drop Routines
 */

/**
 * @addtogroup smd
 * @{
 */

/* Memory Heap Table Module */
smdTableModule gSmdmphTableModule = 
{
    smdmphCreate,
    NULL, /* CreateIotFunc */
    smdmphCreateUndo,
    smdmphDrop,
    smdmphCopy,
    smdmphAddColumns,
    smdmphDropColumns,
    smdmphAlterTableAttr,
    smdmphMergeTableAttr,
    smdmphCheckExistRow,
    smdmphCreateForTruncate,
    smdmphDropAging,
    smdmphInitialize,
    smdmphFinalize,
    smdmphInsert,
    smdmphBlockInsert,
    smdmphUpdate,
    smdmphDelete,
    NULL, /* MergeFunc */
    NULL, /* RowCountFunc */
    smdmphFetch,
    smdmphFetchWithRowid,
    smdmphLockRow,
    smdmphCompareKeyValue,
    smdmphExtractKeyValue,
    smdmphExtractValidKeyValue,
    smdmphSort,
    smdmphGetSlotBody,
    NULL,     /* TruncateFunc */
    NULL,     /* SetFlag */
    NULL,     /* CleanupFunc */
    NULL,     /* BuildCacheFunc */
    smdmphFetchRecord4Index
};

smeIteratorModule gSmdmphIteratorModule = 
{
    STL_SIZEOF(smdmphIterator),

    smdmphInitIterator,
    NULL,     /* finiIterator */
    smdmphResetIterator,
    smdmphMoveToRowRid,
    smdmphMoveToPosNum,
    {
        smdmphFetchNext,
        smdmphFetchPrev,
        smdmphFetchAggr,
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
stlStatus smdmphInitTableHeader( smlStatement  * aStatement,
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
    sTblHeader.mRelHeader.mRelationType = SML_MEMORY_PERSISTENT_HEAP_ROW_TABLE;
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
                         &sSegPage,
                         aEnv ) == STL_SUCCESS );

    sPtr = smpGetBody(SMP_FRAME(&sSegPage)) + smsGetHeaderSize( aSegmentHandle);

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

    sState = 2;
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

    return STL_SUCCESS;

    STL_FINISH;

    if( sLockHandle != NULL )
    {
        (void)smklFreeLockHandle( sLockHandle, aEnv );
        SMD_SET_LOCK_HANDLE( *aRelationHandle, NULL );
    }

    switch( sState )
    {
        case 2 :
            sCaches[0] = aSegmentHandle;
            sCaches[1] = NULL;
            smpSetVolatileArea( &sSegPage, &sCaches, STL_SIZEOF(void*) * 2 );
            smgFreeShmMem4Open( aRelationHandle, aEnv );
        case 1 :
            smxmRollback( &sMiniTrans, aEnv );
        default :
            break;
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
stlStatus smdmphCreate( smlStatement  * aStatement,
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

    KNL_BREAKPOINT( KNL_BREAKPOINT_SMDMPHCREATE_AFTER_ALLOCHDR,
                    KNL_ENV( aEnv ) );

    /**
     * smsCreate에 대한 rollback은 logical로 처리된다.
     */
    STL_TRY( smsCreate( sTransId,
                        aTbsId,
                        SML_SEG_TYPE_MEMORY_BITMAP,
                        sSegmentHandle,
                        0,  /* aInitialExtents(optimal) */
                        aEnv ) == STL_SUCCESS );

    STL_TRY( smdmphInitTableHeader( aStatement,
                                    &sSegmentRid,
                                    sSegmentHandle,
                                    aAttr,
                                    aRelationHandle,
                                    aEnv ) == STL_SUCCESS );

    stlMemset( &sPendArg, 0x00, STL_SIZEOF(smdTablePendArg) );
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
stlStatus smdmphDrop( smlStatement * aStatement,
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
    sPendArg.mHandle = aRelationHandle;
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
stlStatus smdmphCreateForTruncate( smlStatement          * aStatement,
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
    
    KNL_BREAKPOINT( KNL_BREAKPOINT_SMDMPHCREATEFORTRUNCATE_AFTER_ALLOCHDR,
                    KNL_ENV( aEnv ) );

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
    
    STL_TRY( smdmphInitHeaderForTruncate( aStatement,
                                          aOrgRelHandle,
                                          sNewSegHandle,
                                          aNewRelHandle,
                                          aEnv ) == STL_SUCCESS );
    
    /*
     * Add Pending Operations
     */
    stlMemset( &sPendArg, 0x00, STL_SIZEOF(smdTablePendArg) );
    sPendArg.mHandle = *aNewRelHandle;
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

stlStatus smdmphInitHeaderForTruncate( smlStatement  * aStatement,
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

    sOrgSegHandle = SME_GET_SEGMENT_HANDLE( aOrgRelHandle );
    sOrgSegmentRid = smsGetSegRid( sOrgSegHandle );
    sNewSegmentRid = smsGetSegRid( aNewSegHandle );
    
    STL_TRY( smxmBegin( &sMiniTrans,
                        SMA_GET_TRANS_ID( aStatement ),
                        sNewSegmentRid,
                        sAttr,
                        aEnv ) == STL_SUCCESS );
    sState = 1;
    
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
    sState = 2;

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
        sState = 3;
        
        SMD_SET_LOCK_HANDLE( *aNewRelHandle, sLockHandle );
    }

    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 3:
            (void)smklFreeLockHandle( *aNewRelHandle, aEnv );
        case 2:
            sCaches[0] = aNewSegHandle;
            sCaches[1] = NULL;
            smpSetVolatileArea( &sNewPageHandle, &sCaches, STL_SIZEOF(void*) * 2 );
            (void)smgFreeShmMem4Open( *aNewRelHandle, aEnv );
        case 1:
            (void)smxmRollback( &sMiniTrans, aEnv );
        default :
            break;
    }

    return STL_FAILURE;
}

stlStatus smdmphInitialize( smlStatement * aStatement,
                            void         * aRelationHandle,
                            smlEnv       * aEnv )
{
    return STL_SUCCESS;
}


stlStatus smdmphFinalize( smlStatement * aStatement,
                          void         * aRelationHandle,
                          smlEnv       * aEnv )
{
    return STL_SUCCESS;
}

stlStatus smdmphSort( void * RelationHandle, void * aTransactionHandle )
{
	return STL_SUCCESS;
}

stlStatus smdmphBuild( void * RelationHandle, void * aTransactionHandle )
{
	return STL_SUCCESS;
}

stlStatus smdmphDropAging( smxlTransId   aTransId,
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
         * Relation Cache를 삭제한다.
         */
        STL_TRY( smgFreeShmMem4Open( sRelationHandle,
                                     aEnv ) == STL_SUCCESS );
    }
                                    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmphCreateUndo( smxlTransId   aTransId,
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

    KNL_BREAKPOINT( KNL_BREAKPOINT_SMDMPHCREATEUNDO_AFTER_SET_DROPPING,
                    KNL_ENV( aEnv ) );
    
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
            
        STL_TRY( smdmphDropAging( sTransId,
                                  aRelationHandle,
                                  STL_TRUE,
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

stlStatus smdmphAddColumns( smlStatement      * aStatement,
                            void              * aRelationHandle,
                            knlValueBlockList * aAddColumns,
                            smlRowBlock       * aRowBlock,
                            smlEnv            * aEnv )
{
    smlPid               sCurPid = SMP_NULL_PID;
    stlChar              sPageBuf[SMP_PAGE_SIZE + STL_SIZEOF(stlInt64)];
    stlChar            * sFrame;
    smpHandle            sPageHandle;
    stlChar            * sRow;
    smlRid               sPrevRid;
    smlRid               sRowRid;
    stlInt16             sColumnCount;
    stlInt32             sLastColOrd;
    stlInt32             sBlockIdx;
    smlRid               sCurRid;
    stlInt32             sState = 0;

    /**
     * validate parameter
     */
    
    STL_DASSERT( SML_USED_BLOCK_SIZE( aRowBlock ) ==
                 KNL_GET_BLOCK_USED_CNT( aAddColumns ) );
    STL_DASSERT( KNL_GET_BLOCK_SKIP_CNT( aAddColumns ) == 0 );
    
    /**
     * Iterating value block
     */
    
    sFrame = (stlChar*)STL_ALIGN( (stlInt64)sPageBuf, STL_SIZEOF(stlInt64) );

    for( sBlockIdx = 0;
         sBlockIdx < SML_USED_BLOCK_SIZE( aRowBlock );
         sBlockIdx++ )
    {
        sCurRid = SML_GET_RID_VALUE( aRowBlock, sBlockIdx );

        if( sCurRid.mPageId != sCurPid )
        {
            /**
             * Query Timeout 검사
             */
            STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );

            STL_TRY( smpAcquire( NULL,
                                 sCurRid.mTbsId,
                                 sCurRid.mPageId,
                                 KNL_LATCH_MODE_SHARED,
                                 &sPageHandle,
                                 aEnv ) == STL_SUCCESS );
            sState = 1;
            
            sCurPid = sCurRid.mPageId;
            
            stlMemcpy( sFrame, SMP_FRAME( &sPageHandle ), SMP_PAGE_SIZE );
        
            sState = 0;
            STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
        }

        sRow = smpGetNthRowItem( &sPageHandle, sCurRid.mOffset );
        
        if( SMDMPH_HAS_NEXT_LINK( sRow ) == STL_TRUE )
        {
            /**
             * add column for chained row
             */
            
            sPrevRid = sCurRid;
                
            sRowRid.mTbsId = sCurRid.mTbsId;
            SMDMPH_GET_LINK_PID( sRow, &sRowRid.mPageId );
            SMDMPH_GET_LINK_OFFSET( sRow, &sRowRid.mOffset );
                
            SMDMPH_GET_COLUMN_CNT( sRow, &sColumnCount );
            sLastColOrd = sColumnCount - 1;
                
            /**
             * master라도 continuous column은 제외시킨다.
             */
            
            if( SMDMPH_IS_CONT_COL( sRow ) == STL_TRUE )
            {
                sLastColOrd = -1;
            }
            else
            {
                SMDMPH_GET_COLUMN_CNT( sRow, &sColumnCount );
                sLastColOrd = sColumnCount - 1;
            }
                
            STL_TRY( smdmphFindLastPiece( sRowRid,
                                          &sPrevRid,
                                          &sRowRid,
                                          &sLastColOrd,
                                          aEnv )
                     == STL_SUCCESS );

            STL_TRY( smdmphAppendColumns( aStatement,
                                          aRelationHandle,
                                          sLastColOrd + 1,
                                          aAddColumns,
                                          sPrevRid,
                                          sRowRid,
                                          sBlockIdx,
                                          aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            sPrevRid.mPageId = SMP_NULL_PID;
            sRowRid = sCurRid;
                
            SMDMPH_GET_COLUMN_CNT( sRow, &sColumnCount );

            STL_TRY( smdmphAppendColumns( aStatement,
                                          aRelationHandle,
                                          sColumnCount,
                                          aAddColumns,
                                          sPrevRid,
                                          sRowRid,
                                          sBlockIdx,
                                          aEnv )
                     == STL_SUCCESS );
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void) smpRelease( &sPageHandle, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus smdmphDropColumns( smlStatement      * aStatement,
                             void              * aRelationHandle,
                             knlValueBlockList * aDropColumns,
                             smlEnv            * aEnv )
{
    return STL_SUCCESS;
}

stlStatus smdmphCopy( smlStatement      * aStatement,
                      void              * aSrcRelHandle,
                      void              * aDstRelHandle,
                      knlValueBlockList * aColumnList,
                      smlEnv            * aEnv )
{
    return STL_FAILURE;
}

stlStatus smdmphAlterTableAttr( smlStatement  * aStatement,
                                void          * aRelationHandle,
                                smlTableAttr  * aTableAttr,
                                smlIndexAttr  * aIndexAttr,
                                smlEnv        * aEnv )
{
    smxmTrans             sMiniTrans;
    stlUInt32             sAttr = SMXM_ATTR_REDO;
    stlUInt32             sState = 0;
    void                * sSegmentHandle = NULL;
    smlRid                sSegmentRid;
    smpHandle             sPageHandle;
    smdTableHeader      * sTableHeader;
    smlRid                sUndoRid;
    smlTableAttr          sTableAttr;
    smxlTransId           sTransId = SMA_GET_TRANS_ID( aStatement );
    smdTableHeader      * sCache;
    stlInt16              sIniTrans;
    stlInt16              sMaxTrans;

    sCache = (smdTableHeader*)aRelationHandle;
    sSegmentHandle = SME_GET_SEGMENT_HANDLE( aRelationHandle );
    sSegmentRid = smsGetSegRid( sSegmentHandle );

    /**
     * extract original physical attribute
     */
    
    sTableAttr.mValidFlags = 0;
    sTableAttr.mSegAttr.mValidFlags = 0;
    
    STL_TRY( smdmphMergeTableAttr( aRelationHandle,
                                   &sTableAttr,
                                   aEnv )
             == STL_SUCCESS );

    /**
     * validate transaction attribute
     */
    
    if( (aTableAttr->mValidFlags & SML_TABLE_INITRANS_MASK) ==
        SML_TABLE_INITRANS_YES )
    {
        sIniTrans = aTableAttr->mIniTrans;
    }
    else
    {
        sIniTrans = sTableAttr.mIniTrans;
    }
    
    if( (aTableAttr->mValidFlags & SML_TABLE_MAXTRANS_MASK) ==
        SML_TABLE_MAXTRANS_YES )
    {
        sMaxTrans = aTableAttr->mMaxTrans;
    }
    else
    {
        sMaxTrans = sTableAttr.mMaxTrans;
    }
    
    STL_TRY_THROW( sMaxTrans >= sIniTrans, RAMP_ERR_TRANS_RANGE );
    
    /**
     * alter physical attribute
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
                                      smsGetHeaderSize( sSegmentHandle ) );

    /**
     * logging undo log
     */
    
    STL_TRY( smxlInsertUndoRecord( &sMiniTrans,
                                   SMG_COMPONENT_TABLE,
                                   SMD_UNDO_LOG_MEMORY_HEAP_ALTER_ATTR,
                                   (stlChar*)&sTableAttr,
                                   STL_SIZEOF(smlTableAttr),
                                   sSegmentRid,
                                   &sUndoRid,
                                   aEnv ) == STL_SUCCESS );

    /**
     * applys table attribute to relation header
     */
    
    if( (aTableAttr->mValidFlags & SML_TABLE_LOGGING_MASK) ==
        SML_TABLE_LOGGING_YES )
    {
        sTableHeader->mLoggingFlag = aTableAttr->mLoggingFlag;
    }

    if( (aTableAttr->mValidFlags & SML_TABLE_LOCKING_MASK) ==
        SML_TABLE_LOCKING_YES )
    {
        sTableHeader->mLockingFlag = aTableAttr->mLockingFlag;
    }
    
    if( (aTableAttr->mValidFlags & SML_TABLE_INITRANS_MASK) ==
        SML_TABLE_INITRANS_YES )
    {
        sTableHeader->mIniTrans = aTableAttr->mIniTrans;
    }
    
    if( (aTableAttr->mValidFlags & SML_TABLE_MAXTRANS_MASK) ==
        SML_TABLE_MAXTRANS_YES )
    {
        sTableHeader->mMaxTrans = aTableAttr->mMaxTrans;
    }
    
    if( (aTableAttr->mValidFlags & SML_TABLE_PCTFREE_MASK) ==
        SML_TABLE_PCTFREE_YES )
    {
        sTableHeader->mPctFree = aTableAttr->mPctFree;
    }
    
    if( (aTableAttr->mValidFlags & SML_TABLE_PCTUSED_MASK) ==
        SML_TABLE_PCTUSED_YES )
    {
        sTableHeader->mPctUsed = aTableAttr->mPctUsed;
    }

    /**
     * applys table attribute to relation cache
     */

    sCache->mLoggingFlag = sTableHeader->mLoggingFlag;
    sCache->mLockingFlag = sTableHeader->mLockingFlag;
    sCache->mIniTrans = sTableHeader->mIniTrans;
    sCache->mMaxTrans = sTableHeader->mMaxTrans;
    sCache->mPctFree = sTableHeader->mPctFree;
    sCache->mPctUsed = sTableHeader->mPctUsed;
    
    /**
     * logging redo log
     */
    
    STL_TRY( smxmWriteLog( &sMiniTrans,
                           SMG_COMPONENT_RELATION,
                           SMR_LOG_INIT_REL_HEADER,
                           SMR_REDO_TARGET_PAGE,
                           (stlChar*)sTableHeader,
                           STL_SIZEOF(smdTableHeader),
                           sSegmentRid.mTbsId,
                           sSegmentRid.mPageId,
                           (stlChar*)sTableHeader - (stlChar*)SMP_FRAME(&sPageHandle),
                           SMXM_LOGGING_REDO,
                           aEnv )
             == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
    
    /**
     * apply segment attribute
     */
    
    STL_TRY( smsAlterSegmentAttr( sTransId,
                                  sSegmentHandle,
                                  &aTableAttr->mSegAttr,
                                  aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_TRANS_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_TRANS_RANGE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }
    
    STL_FINISH;

    if( sState == 1 )
    {
        (void) smxmRollback( &sMiniTrans, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smdmphMergeTableAttr( void          * aRelationHandle,
                                smlTableAttr  * aTableAttr,
                                smlEnv        * aEnv )
{
    smlTableAttr     sTableAttr;
    smdTableHeader * sTableHeader;

    sTableHeader = (smdTableHeader*)aRelationHandle;

    sTableAttr.mValidFlags = SML_TABLE_DEFAULT_MEMORY_ATTR;
    sTableAttr.mLoggingFlag = sTableHeader->mLoggingFlag;
    sTableAttr.mLockingFlag = sTableHeader->mLockingFlag;
    sTableAttr.mIniTrans = sTableHeader->mIniTrans;
    sTableAttr.mMaxTrans = sTableHeader->mMaxTrans;
    sTableAttr.mPctFree = sTableHeader->mPctFree;
    sTableAttr.mPctUsed = sTableHeader->mPctUsed;
    
    if( (aTableAttr->mValidFlags & SML_TABLE_LOGGING_MASK) ==
        SML_TABLE_LOGGING_YES )
    {
        sTableAttr.mLoggingFlag = aTableAttr->mLoggingFlag;
    }

    if( (aTableAttr->mValidFlags & SML_TABLE_LOCKING_MASK) ==
        SML_TABLE_LOCKING_YES )
    {
        sTableAttr.mLockingFlag = aTableAttr->mLockingFlag;
    }
    
    if( (aTableAttr->mValidFlags & SML_TABLE_INITRANS_MASK) ==
        SML_TABLE_INITRANS_YES )
    {
        sTableAttr.mIniTrans = aTableAttr->mIniTrans;
    }
    
    if( (aTableAttr->mValidFlags & SML_TABLE_MAXTRANS_MASK) ==
        SML_TABLE_MAXTRANS_YES )
    {
        sTableAttr.mMaxTrans = aTableAttr->mMaxTrans;
    }
    
    if( (aTableAttr->mValidFlags & SML_TABLE_PCTFREE_MASK) ==
        SML_TABLE_PCTFREE_YES )
    {
        sTableAttr.mPctFree = aTableAttr->mPctFree;
    }
    
    if( (aTableAttr->mValidFlags & SML_TABLE_PCTUSED_MASK) ==
        SML_TABLE_PCTUSED_YES )
    {
        sTableAttr.mPctUsed = aTableAttr->mPctUsed;
    }

    smsMergeSegmentAttr( SME_GET_SEGMENT_HANDLE( aRelationHandle ),
                         &(aTableAttr->mSegAttr),
                         &(sTableAttr.mSegAttr) );
    
    *aTableAttr = sTableAttr;

    return STL_SUCCESS;
}

stlStatus smdmphCheckExistRow( smlStatement * aStatement,
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

        SMP_INIT_ROWITEM_ITERATOR( sFrame, &sSlotIterator );
        
        SMP_FOREACH_ROWITEM_ITERATOR( &sSlotIterator )
        {
            if( SMP_IS_FREE_ROWITEM( &sSlotIterator ) == STL_TRUE )
            {
                continue;
            }
        
            sRow = SMP_GET_CURRENT_ROWITEM( &sSlotIterator );
            
            if( SMDMPH_IS_DELETED( sRow ) != STL_TRUE )
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
