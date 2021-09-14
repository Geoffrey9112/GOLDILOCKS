/*******************************************************************************
 * smdmppTable.c
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
#include <sme.h>
#include <smf.h>
#include <smd.h>
#include <smg.h>
#include <sma.h>
#include <smkl.h>
#include <smdmppDef.h>
#include <smdmpp.h>
#include <smdDef.h>

extern smxlUndoFunc * gRecoveryUndoFuncs[];

/**
 * @file smdmppTable.c
 * @brief Storage Manager Layer Memory Pending Table Create/Drop Routines
 */

/**
 * @addtogroup smd
 * @{
 */

/* Memory Pending Table Module */
smdTableModule gSmdmppTableModule = 
{
    smdmppCreate,
    NULL, /* CreateIotFunc */
    NULL, /* CreateUndoFunc */
    smdmppDrop,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    smdmppDropAging,
    NULL,
    NULL,
    smdmppInsert,
    NULL,
    NULL,
    smdmppDelete,
    NULL, /* MergeFunc */
    NULL, /* RowCountFunc */
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    smdmppTruncate,
    NULL,
    NULL,  /* CleanupFunc */
    NULL,  /* BuildCacheFunc */
    NULL   /* Fetch4Index */
};

smeIteratorModule gSmdmppIteratorModule = 
{
    STL_SIZEOF(smdmppIterator),
    
    smdmppInitIterator,
    NULL,     /* finiIterator */
    NULL,     /* resetIterator */
    NULL,     /* moveToRowRid */
    NULL,     /* moveToPosNum */
    {
        smdmppFetchNext,
        NULL,  /* fetchPrev */
        NULL,  /* FetchAggrNext */
        NULL   /* FetchSampling */
    }
};

/**
 * @brief 테이블을 생성한다.
 * @param[in] aStatement Statement 객체
 * @param[in] aTbsId 생성할 테이블이 속할 Tablespace ID
 * @param[in] aAttr 생성할 테이블 속성을 지정한 구조체 ( nullable )
 * @param[in] aLoggingUndo Undo 로그의 기록 여부
 * @param[out] aSegmentId 생성된 테이블의 물리적 아이디
 * @param[out] aRelationHandle 생성된 테이블을 조작할때 사용할 메모리 핸들
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smdmppCreate( smlStatement  * aStatement,
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
    stlUInt32       sAttr = SMXM_ATTR_REDO;
    smlRid          sUndoRid;
    smxmTrans       sMiniTrans;
    stlInt32        sState = 0;

    STL_DASSERT( aTbsId == SML_MEMORY_DICT_SYSTEM_TBS_ID );
    
    STL_TRY( smxmBegin( &sMiniTrans,
                        sTransId,
                        SML_INVALID_RID,
                        sAttr,
                        aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( smsAllocSegHdr( &sMiniTrans,
                             aTbsId,
                             SML_SEG_TYPE_MEMORY_PENDING,
                             NULL,   /* smlSegmentAttr */
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
                                       SMD_UNDO_LOG_MEMORY_PENDING_CREATE,
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
                        SML_SEG_TYPE_MEMORY_PENDING,
                        sSegmentHandle,
                        0,  /* aInitialExtents(optimal) */
                        aEnv ) == STL_SUCCESS );

    STL_TRY( smdmppInitTableHeader( aStatement,
                                    &sSegmentRid,
                                    sSegmentHandle,
                                    aRelationHandle,
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
stlStatus smdmppDrop( smlStatement * aStatement,
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
    
    STL_TRY( smxlInsertUndoRecord( &sMiniTrans,
                                   SMG_COMPONENT_TABLE,
                                   SMD_UNDO_LOG_MEMORY_PENDING_DROP,
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
    
    /*
     * 2. Update Relation Cache
     */
    SME_SET_RELATION_STATE( aRelationHandle, SME_RELATION_STATE_DROPPING );
    ((smdTableHeader*)aRelationHandle)->mRelHeader.mDropTransId = sTransId;
    
    /*
     * 3. Add Pending Operation
     */
    stlMemset( &sPendArg, 0x00, STL_SIZEOF(smdTablePendArg) );
    sPendArg.mHandle = aRelationHandle;
    sPendArg.mTransId = sTransId;

    STL_TRY( smgAddPendOp( aStatement,
                           SMG_PEND_DROP_PENDING_TABLE,
                           SML_PEND_ACTION_COMMIT,
                           (void*)&sPendArg,
                           STL_SIZEOF( smdTablePendArg ),
                           28,
                           aEnv ) == STL_SUCCESS );

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
 * @brief 테이블의 내용을 초기화 시킨다.
 * @param[in] aStatement Statement 객체
 * @param[in] aRelationHandle 삭제할 테이블의 relation handle
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smdmppTruncate( smlStatement * aStatement,
                          void         * aRelationHandle,
                          smlEnv       * aEnv )
{
    STL_TRY( smsReset( SMA_GET_TRANS_ID(aStatement),
                       SME_GET_SEGMENT_HANDLE( aRelationHandle ),
                       STL_FALSE,  /* aFreePages */
                       aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmppDropPending( stlUInt32   aActionFlag,
                             smgPendOp * aPendOp,
                             smlEnv    * aEnv )
{
    smdTablePendArg   * sPendArg = (smdTablePendArg*)aPendOp->mArgs;
    smeRelationHeader * sRelationHeader;
    stlInt32            sOffset = 0;
    stlChar             sBuffer[32];
    smlRid              sSegRid;
    smlTableType        sTableType;
    smlScn              sObjectAgableScn;
    void              * sLockHandle;

    sRelationHeader = (smeRelationHeader*)(sPendArg->mHandle);

    sLockHandle = smdGetLockHandle( sPendArg->mHandle );

    if( sLockHandle != NULL )
    {
        STL_TRY( smklMarkFreeLockHandle( sLockHandle, aEnv ) == STL_SUCCESS );
    }

    STL_TRY( smxlGetCommitScnFromCache( sPendArg->mTransId,
                                        &sRelationHeader->mObjectScn,
                                        aEnv ) == STL_SUCCESS );
    sRelationHeader->mDropScn = sRelationHeader->mObjectScn;

    sSegRid = smsGetSegRid( SME_GET_SEGMENT_HANDLE(sPendArg->mHandle) );
    sTableType = SML_GET_TABLE_TYPE( sRelationHeader->mRelationType );

    sObjectAgableScn = smxlGetNextSystemScn();

    STL_WRITE_MOVE_INT64( sBuffer, &sRelationHeader->mObjectScn, sOffset );
    STL_WRITE_MOVE_INT64( sBuffer, &sObjectAgableScn, sOffset );
    STL_WRITE_MOVE_INT32( sBuffer, &sTableType, sOffset );
    SMG_WRITE_MOVE_RID( sBuffer, &sSegRid, sOffset );

    STL_DASSERT( sOffset < STL_SIZEOF( sBuffer ) );
    STL_DASSERT( sOffset <= aPendOp->mEventMemSize );

    STL_TRY( knlAddEnvEvent( SML_EVENT_DROP_TABLE,
                             aPendOp->mEventMem,
                             sBuffer,
                             sOffset,
                             SML_AGER_ENV_ID,
                             KNL_EVENT_WAIT_NO,
                             STL_TRUE, /* aForceSuccess */
                             KNL_ENV( aEnv ) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmppDropAging( smxlTransId   aTransId,
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
         * Lock Handle을 삭제한다.
         */
        if( SMD_GET_LOCK_HANDLE(sRelationHandle) != NULL )
        {
            STL_TRY( smklFreeLockHandle( sRelationHandle,
                                         aEnv ) == STL_SUCCESS );
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

/**
 * @brief segment page에 테이블헤더 정보를 기록한다.
 * @param[in] aStatement Statement 객체
 * @param[in] aSegmentRid table이 사용할 segment
 * @param[in] aSegmentHandle 기존에 생성된 segment handle
 * @param[out] aRelationHandle 생성된 테이블을 조작할때 사용할 메모리 핸들
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smdmppInitTableHeader( smlStatement  * aStatement,
                                 smlRid        * aSegmentRid,
                                 void          * aSegmentHandle,
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
    void                * sLockHandle = NULL;

    stlMemset( (void*)&sTblHeader, 0x00, STL_SIZEOF(smdTableHeader) );
    sTblHeader.mRelHeader.mRelationType = SML_MEMORY_PERSISTENT_PENDING_TABLE;
    sTblHeader.mRelHeader.mRelHeaderSize = STL_SIZEOF(smdTableHeader);
    sTblHeader.mRelHeader.mCreateTransId = SMA_GET_TRANS_ID( aStatement );
    sTblHeader.mRelHeader.mDropTransId = SML_INVALID_TRANSID;
    sTblHeader.mRelHeader.mCreateScn = 0;
    sTblHeader.mRelHeader.mDropScn = 0;
    SME_SET_SEGMENT_HANDLE((void*)&sTblHeader, aSegmentHandle);
    SME_SET_OBJECT_SCN((void*)&sTblHeader, SML_INFINITE_SCN);
    SME_SET_RELATION_STATE((void*)&sTblHeader, SME_RELATION_STATE_ACTIVE);

    sTblHeader.mLoggingFlag = STL_TRUE;
    sTblHeader.mLockingFlag = STL_FALSE;
    sTblHeader.mIniTrans = 0;
    sTblHeader.mMaxTrans = 0;
    sTblHeader.mPctFree = 0;
    sTblHeader.mPctUsed = 0;
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

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2 :
            {
                smgFreeShmMem4Open( aRelationHandle, aEnv );
            }
        case 1 :
            {
                smxmRollback( &sMiniTrans, aEnv );
            }
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
stlStatus smdmppInsert( smlStatement      * aStatement,
                        void              * aRelationHandle,
                        stlInt32            aValueIdx,
                        knlValueBlockList * aInsertCols,
                        knlValueBlockList * aUniqueViolation,
                        smlRid            * aRowRid,
                        smlEnv            * aEnv )
{
    void         * sSegmentHandle = SME_GET_SEGMENT_HANDLE(aRelationHandle);
    smlRid         sSegRid;
    stlUInt32      sAttr = SMXM_ATTR_REDO;
    smpHandle      sPageHandle;
    stlBool        sIsNewPage = STL_FALSE;
    stlChar      * sSlot = NULL;
    stlInt16       sSlotSeq;
    stlInt32       sState = 0;
    stlInt32       sRowSize = 0;
    stlChar        sRowBody[SMP_PAGE_SIZE];
    smxmTrans      sMiniTrans;
    stlChar      * sColPtr;
    stlInt64       sColLen;
    stlUInt8       sColLenSize;
    smlRid         sDataRid;
    smlRid         sUndoRid;
    stlInt32       sOperationType;
    smxlUndoRecHdr sUndoRecHdr;
    stlInt64       sValidSeq = 0;
    smlRid         sTargetSegRid;
    
    sSegRid = smsGetSegRid( sSegmentHandle );

    STL_DASSERT( aInsertCols->mNext == NULL );
    
    sColPtr = (stlChar*)KNL_GET_BLOCK_DATA_PTR( aInsertCols, aValueIdx );
    sColLen = KNL_GET_BLOCK_DATA_LENGTH( aInsertCols, aValueIdx );
    SMP_GET_COLUMN_LEN_SIZE( sColLen, &sColLenSize );
    sRowSize = sColLen + sColLenSize;
    STL_DASSERT( sRowSize < SMP_PAGE_SIZE );
    STL_WRITE_INT32( &sOperationType, sColPtr );
    SMP_WRITE_COLUMN_ZERO_INSENS( sRowBody, sColPtr, sColLen );
    

    STL_TRY( smxmBegin( &sMiniTrans,
                        SMA_GET_TRANS_ID( aStatement ),
                        sSegRid,
                        sAttr,
                        aEnv ) == STL_SUCCESS );
    sState = 1;

    while( 1 )
    {
        STL_TRY( smsGetInsertablePage( &sMiniTrans,
                                       sSegmentHandle,
                                       SMP_PAGE_TYPE_TABLE_DATA,
                                       NULL,  /* aIsAgableFunc */
                                       NULL,  /* aSearchHint */
                                       &sPageHandle,
                                       &sIsNewPage,
                                       aEnv ) == STL_SUCCESS );

        if( sIsNewPage == STL_TRUE )
        {
            STL_TRY( smpInitBody( &sPageHandle,
                                  SMP_ORDERED_SLOT_TYPE,
                                  0,          /* aLogicalHeaderSize */
                                  NULL,       /* aLogicalHeader */
                                  0,          /* aMinRtsCount */
                                  0,          /* aMaxRtsCount */
                                  STL_FALSE,  /* aUseFreeSlotList */
                                  &sMiniTrans,
                                  aEnv) == STL_SUCCESS );
        }
    
        STL_TRY( smpAllocSlot( &sPageHandle,
                               sRowSize  + SMDMPP_ROW_HDR_SIZE,
                               STL_TRUE,  /* aIsForTest */
                               &sSlot,
                               &sSlotSeq ) == STL_SUCCESS );

        if( sSlot != NULL )
        {
            break;
        }
        
        STL_TRY( smsUpdatePageStatus( &sMiniTrans,
                                      sSegmentHandle,
                                      &sPageHandle,
                                      SMP_FREENESS_UPDATEONLY,
                                      0,  /* aScn */
                                      aEnv ) == STL_SUCCESS );
    }
    
    STL_TRY( smpAllocSlot( &sPageHandle,
                           sRowSize + SMDMPP_ROW_HDR_SIZE,
                           STL_FALSE,  /* aIsForTest */
                           &sSlot,
                           &sSlotSeq ) == STL_SUCCESS );

    STL_ASSERT( sSlot != NULL );

    /**
     * Header 설정
     */

    if( sOperationType == SMD_PENDING_OPERATION_UNDO )
    {
        /**
         * Undo Type은 segment validation 정보가 필요하다
         */
        
        SMXL_READ_UNDO_REC_HDR( &sUndoRecHdr,
                                sRowBody + sColLenSize + STL_SIZEOF(stlInt32) );

        SML_MAKE_RID( &sTargetSegRid,
                      SML_MEMORY_DICT_SYSTEM_TBS_ID,
                      sUndoRecHdr.mTargetSegPid,
                      0 );

        STL_TRY( smsGetValidSeq( sTargetSegRid,
                                 &sValidSeq,
                                 aEnv )
                 == STL_SUCCESS );
    }
    
    SMDMPP_UNSET_DELETED( sSlot );
    SMDMPP_SET_VALID_SEQ( sSlot, sValidSeq );
    
    stlMemcpy( sSlot + SMDMPP_ROW_HDR_SIZE, sRowBody, sRowSize );
    
    STL_TRY( smxmWriteLog( &sMiniTrans,
                           SMG_COMPONENT_TABLE,
                           SMR_LOG_MEMORY_PENDING_INSERT,
                           SMR_REDO_TARGET_PAGE,
                           sSlot,
                           sRowSize + SMDMPP_ROW_HDR_SIZE,
                           sPageHandle.mPch->mTbsId,
                           sPageHandle.mPch->mPageId,
                           sSlotSeq,  /* Physiological redo를 하기위해 offset이 아닌 seq 사용 */
                           SMXM_LOGGING_REDO,
                           aEnv )
             == STL_SUCCESS );

    sDataRid.mTbsId = sPageHandle.mPch->mTbsId;
    sDataRid.mPageId = sPageHandle.mPch->mPageId;
    sDataRid.mOffset = sSlotSeq;

    /**
     * Drop Segment type은 Servie 상태에서만 rollback 가능해야 한다.
     */
    
    if( (sOperationType == SMD_PENDING_OPERATION_DROP_SEGMENT) &&
        (smfGetServerState() != SML_SERVER_STATE_RECOVERING) )
    {
        STL_TRY( smxlInsertUndoRecord( &sMiniTrans,
                                       SMG_COMPONENT_TABLE,
                                       SMD_UNDO_LOG_MEMORY_PENDING_INSERT,
                                       NULL,
                                       0,
                                       sDataRid,
                                       &sUndoRid,
                                       aEnv ) == STL_SUCCESS );
    }

    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) smxmCommit( &sMiniTrans, aEnv );
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
stlStatus smdmppDelete( smlStatement      * aStatement,
                        void              * aRelationHandle,
                        smlRid            * aRowRid,
                        smlScn              aRowScn,
                        stlInt32            aValueIdx,
                        knlValueBlockList * aPrimaryKey,
                        stlBool           * aVersionConflict,
                        stlBool           * aSkipped,
                        smlEnv            * aEnv )
{
    stlUInt32          sAttr = SMXM_ATTR_REDO;
    void             * sSegmentHandle = SME_GET_SEGMENT_HANDLE(aRelationHandle);
    smxmTrans          sMiniTrans;
    smpHandle          sPageHandle;
    stlChar          * sRowHdr;
    stlChar          * sRow;
    stlChar          * sRowBody;
    stlInt64           sRowLen;
    stlInt64           sRowLenSize;
    stlInt64           sHandleBuffer[SMP_PAGE_SIZE/STL_SIZEOF(stlInt64)];
    void             * sHeapSegmentHandle;
    stlInt32           sOffset = 0;
    stlInt32           sOperationType;
    stlInt32           sState = 0;
    stlChar            sData[2];
    smxlUndoFunc     * sUndoFuncs;
    smxlUndoRecHdr     sUndoRecHdr;
    smlRid             sTargetSegRid;
    stlInt64           sNewValidSeq;
    stlInt64           sOldValidSeq;
    
    sHeapSegmentHandle = (void*)sHandleBuffer;
    
    STL_TRY( smxmBegin( &sMiniTrans,
                        SMA_GET_TRANS_ID(aStatement),
                        smsGetSegRid(sSegmentHandle),
                        sAttr,
                        aEnv ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( smpAcquire( &sMiniTrans,
                         smsGetTbsId(sSegmentHandle),
                         aRowRid->mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );
    
    sRowHdr = smpGetNthRowItem( &sPageHandle, aRowRid->mOffset );
    sRow = sRowHdr + SMDMPP_ROW_HDR_SIZE;

    SMDMPP_SET_DELETED( sRowHdr );

    SMP_GET_COLUMN_LEN_ZERO_INSENS( sRow, &sRowLenSize, &sRowLen );
    sRowBody = sRow + sRowLenSize;
    STL_READ_MOVE_INT32( &sOperationType, sRowBody, sOffset );

    switch( sOperationType )
    {
        case SMD_PENDING_OPERATION_DROP_SEGMENT:
                    
            STL_READ_MOVE_BYTES( sHeapSegmentHandle,
                                 sRowBody,
                                 sRowLen - sOffset,
                                 sOffset );
                    
            STL_TRY( smsPendingDrop( SMA_GET_TRANS_ID(aStatement),
                                     sHeapSegmentHandle,
                                     aEnv )
                     == STL_SUCCESS );
                    
            break;
                    
        case SMD_PENDING_OPERATION_UNDO:

            SMXL_READ_MOVE_UNDO_REC_HDR( &sUndoRecHdr,
                                         sRowBody,
                                         sOffset );
            
            SML_MAKE_RID( &sTargetSegRid,
                          SML_MEMORY_DICT_SYSTEM_TBS_ID,
                          sUndoRecHdr.mTargetSegPid,
                          0 );

            STL_TRY( smsGetValidSeq( sTargetSegRid,
                                     &sNewValidSeq,
                                     aEnv )
                     == STL_SUCCESS );

            SMDMPP_GET_VALID_SEQ( sRowHdr, sOldValidSeq );

            smxmSetSegmentRid( &sMiniTrans, sTargetSegRid );
            
            /**
             * segment가 drop 되었거나 재사용된 경우는 undo를 하지 않는다.
             */
            
            if( sOldValidSeq == sNewValidSeq )
            {
                sUndoFuncs = gRecoveryUndoFuncs[(stlInt16)sUndoRecHdr.mComponentClass];
                STL_ASSERT( sUndoFuncs != NULL );

                STL_TRY( sUndoFuncs[sUndoRecHdr.mType]( &sMiniTrans,
                                                        sUndoRecHdr.mTargetDataRid,
                                                        sRowBody + sOffset,
                                                        sUndoRecHdr.mSize,
                                                        aEnv )
                         == STL_SUCCESS );
            }
            
            break;
            
        default:
            STL_DASSERT( 0 );
            break;
    }

    sData[0] = sRowHdr[0];
    sData[1] = sRowHdr[0];

    STL_TRY( smxmWriteLog( &sMiniTrans,
                           SMG_COMPONENT_RECOVERY,
                           SMR_LOG_BYTES,
                           SMR_REDO_TARGET_PAGE,
                           (void*)sData,
                           STL_SIZEOF(stlChar) * 2,
                           smsGetTbsId(sSegmentHandle),
                           aRowRid->mPageId,
                           (stlChar*)sRowHdr - (stlChar*)SMP_FRAME(&sPageHandle),
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

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
 * @brief 주어진 Iterator를 초기화한다
 * @param[in,out] aIterator 초기화할 iterator
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmppInitIterator ( void   * aIterator,
                               smlEnv * aEnv )
{
    smdmppIterator * sIterator = (smdmppIterator*)aIterator;
    void           * sSegHandle;

    sSegHandle = SME_GET_SEGMENT_HANDLE( sIterator->mCommon.mRelationHandle );
    
    sIterator->mRowRid.mTbsId = smsGetTbsId( sSegHandle );
    sIterator->mRowRid.mPageId = SMP_NULL_PID;
    sIterator->mRowRid.mOffset = -1;
    sIterator->mPageBufPid = SMP_NULL_PID;
    
    STL_TRY( smsAllocSegIterator( sIterator->mCommon.mStatement,
                                  sSegHandle,
                                  &sIterator->mSegIterator,
                                  aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 일반 Scan을 위해 열린 Iterator에서 다음 row를 반환한다
 * @param[in]       aIterator    이전에 읽은 위치를 가진 iterator
 * @param[in,out]   aFetchInfo   Fetch 정보
 * @param[in]       aEnv         Storage Manager Environment
 */
stlStatus smdmppFetchNext( void          * aIterator,
                           smlFetchInfo  * aFetchInfo,
                           smlEnv        * aEnv )
{
    smdmppIterator    * sIterator = (smdmppIterator*)aIterator;
    smpHandle           sPageHandle;
    smlPid              sPid = sIterator->mPageBufPid;
    stlInt16            sCurSlotSeq = sIterator->mRowRid.mOffset;
    stlChar           * sRow;
    void              * sSegHandle;
    stlInt32            sState = 0;
    
    STL_PARAM_VALIDATE( aFetchInfo->mRowBlock != NULL, KNL_ERROR_STACK(aEnv) );
    SML_RESET_USED_BLOCK( aFetchInfo->mRowBlock );
    
    aFetchInfo->mIsEndOfScan = STL_FALSE;

    sSegHandle = SME_GET_SEGMENT_HANDLE( sIterator->mCommon.mRelationHandle );

    if( sPid == SMP_NULL_PID )
    {
        STL_TRY( smsGetFirstPage( sSegHandle,
                                  sIterator->mSegIterator,
                                  &sPid,
                                  aEnv ) == STL_SUCCESS );

        if( sPid != SMP_NULL_PID )
        {
            /**
             * cache page to iterator buffer
             */
            
            STL_TRY( smpAcquire( NULL,
                                 sIterator->mRowRid.mTbsId,
                                 sPid,
                                 KNL_LATCH_MODE_SHARED,
                                 &sPageHandle,
                                 aEnv ) == STL_SUCCESS );
            sState = 1;

            stlMemcpy( sIterator->mPageBuf, SMP_FRAME(&sPageHandle), SMP_PAGE_SIZE );
            
            sState = 0;
            STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
            sIterator->mPageBufPid = sPid;
            sCurSlotSeq = -1;
        }
    }
    
    /**
     * iterating pages
     */

    while( sPid != SMP_NULL_PID )
    {
        /**
         * iterating rows
         */
        
        while( 1 )
        {
            if( sCurSlotSeq == -1 )
            {
                SMP_GET_FIRST_ROWITEM( sIterator->mPageBuf,
                                       &sIterator->mCommon.mSlotIterator,
                                       sRow );
            }
            else
            {
                SMP_GET_NEXT_ROWITEM( &sIterator->mCommon.mSlotIterator, sRow );
            }

            if( sRow == NULL )
            {
                break;
            }

            sCurSlotSeq = SMP_GET_CURRENT_ROWITEM_SEQ( &sIterator->mCommon.mSlotIterator );
            
            if( SMDMPP_IS_DELETED( sRow ) != STL_TRUE )
            {
                sIterator->mRowRid = (smlRid) { 0,
                                                sCurSlotSeq,
                                                sIterator->mPageBufPid };
                
                SML_SET_RID_VALUE( aFetchInfo->mRowBlock, 0, sIterator->mRowRid );
                SML_SET_SCN_VALUE( aFetchInfo->mRowBlock, 0, 0 );
                SML_SET_USED_BLOCK_SIZE( aFetchInfo->mRowBlock, 1 );
                
                STL_THROW( RAMP_SUCCESS );
            }
        }

        STL_TRY( smsGetNextPage( SME_GET_SEGMENT_HANDLE(sIterator->mCommon.mRelationHandle),
                                 sIterator->mSegIterator,
                                 &sPid,
                                 aEnv ) == STL_SUCCESS );

        sCurSlotSeq = -1;
        
        if( sPid != SMP_NULL_PID )
        {
            /**
             * cache page to iterator buffer
             */
        
            STL_TRY( smpAcquire( NULL,
                                 sIterator->mRowRid.mTbsId,
                                 sPid,
                                 KNL_LATCH_MODE_SHARED,
                                 &sPageHandle,
                                 aEnv ) == STL_SUCCESS );
            sState = 1;

            stlMemcpy( sIterator->mPageBuf, SMP_FRAME(&sPageHandle), SMP_PAGE_SIZE );
            
            sState = 0;
            STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
            
            sIterator->mPageBufPid = sPid;
            sIterator->mRowRid = (smlRid){ 0, -1, sPid };
        }
        else
        {
            sIterator->mPageBufPid = SMP_NULL_PID;
            sIterator->mRowRid = (smlRid){ 0, -1, SMP_NULL_PID };
        }
    }

    aFetchInfo->mIsEndOfScan = STL_TRUE;
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) smpRelease( &sPageHandle, aEnv );
    }

    return STL_FAILURE;
}

/** @} */
