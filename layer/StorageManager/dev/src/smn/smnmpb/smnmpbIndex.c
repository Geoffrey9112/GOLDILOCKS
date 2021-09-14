/*******************************************************************************
 * smnmpbIndex.c
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
#include <sml.h>
#include <smDef.h>
#include <sms.h>
#include <sme.h>
#include <smf.h>
#include <smp.h>
#include <smg.h>
#include <smd.h>
#include <sma.h>
#include <smr.h>
#include <smxm.h>
#include <smxl.h>
#include <smnDef.h>
#include <smnmpbDef.h>
#include <smnmpb.h>

/**
 * @file smnmpbIndex.c
 * @brief Storage Manager Layer Memory B-Tree Index Create/Drop Routines
 */

/**
 * @addtogroup smn
 * @{
 */

/* Memory B-Tree Index Module */
smnIndexModule gSmnmpbModule = 
{
    smnmpbCreate,
    smnmpbCreateUndo,
    smnmpbDrop,
    smnmpbAlterDataType,
    smnmpbAlterIndexAttr,
    smnmpbCreateForTruncate,
    smnmpbDropAging,
    smnmpbInitialize,
    smnmpbFinalize,
    smnmpbBuild,
    smnmpbBuildUnusable,
    smnmpbBuildForTruncate,
    smnmpbInsert,
    smnmpbDelete,
    smnmpbBlockInsert,
    smnmpbBlockDelete,
    smnmpbGetSlotBody,
    smnmpbInitModuleSpecificData,
    smnmpbDestModuleSpecificData,
    NULL, /* Truncate Instant Index */
};


smeIteratorModule gSmnmpbIteratorModule = 
{
    STL_SIZEOF(smnmpbIterator),

    smnmpbInitIterator,
    NULL,   /* FiniIterator */
    smnmpbResetIterator,
    smnmpbMoveToRowRid,
    smnmpbMoveToPosNum,
    {
        smnmpbFetchNext,
        smnmpbFetchPrev,
        smnmpbFetchAggr,
        NULL   /* FetchSampling */
    }
};


/**
 * @brief segment page에 인덱스헤더 정보를 기록한다.
 * @param[in] aStatement Statement 객체
 * @param[in] aSegmentHandle 기존에 생성된 segment handle
 * @param[in] aBaseTableRelHandle Index가 생성될 테이블의 relation handle
 * @param[in] aKeyColCount Index가 가지는 key column의 개수
 * @param[in] aIndexColList 각 key column들의 list. Fetch를 위한 Buffer와 type, base table order도 있음
 * @param[in] aKeyColFlags Index가 가지는 key column들의 flag 배열
 * @param[in] aAttr index 관련 attribute 정보
 * @param[out] aRelationHandle 생성된 Index를 조작할때 사용할 메모리 핸들
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smnmpbInitIndexHeader( smlStatement       * aStatement,
                                 void               * aSegmentHandle,
                                 void               * aBaseTableRelHandle,
                                 smlIndexAttr       * aAttr,
                                 stlUInt16            aKeyColCount,
                                 knlValueBlockList  * aIndexColList,
                                 stlUInt8           * aKeyColFlags,
                                 void              ** aRelationHandle,
                                 smlEnv             * aEnv )
{
    smnIndexHeader        sIdxHeader;
    smxmTrans             sMiniTrans;
    stlUInt32             sAttr = SMXM_ATTR_REDO;
    smpHandle             sSegPage;
    stlChar             * sPtr;
    void                * sCaches[2];
    stlUInt32             sState = 0;
    stlBool               sIsLogging;
    stlInt64              sIniTrans;
    stlInt64              sMaxTrans;
    stlInt64              sPctFree;
    smlRid                sSegRid = smsGetSegRid(aSegmentHandle);
    stlInt16              i;
    stlUInt8              sKeyColTypes[SML_MAX_INDEX_KEY_COL_COUNT];
    stlUInt32             sKeyColOrder[SML_MAX_INDEX_KEY_COL_COUNT];
    stlUInt64             sKeyColSize[SML_MAX_INDEX_KEY_COL_COUNT];
    knlValueBlockList   * sCol;

    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_DEFAULT_INDEX_LOGGING,
                                      &sIsLogging,
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

    stlMemset( (void*)&sIdxHeader, 0x00, STL_SIZEOF(smnIndexHeader) );
    sIdxHeader.mRelHeader.mRelationType = SML_MEMORY_PERSISTENT_BTREE_INDEX;
    sIdxHeader.mRelHeader.mRelHeaderSize = STL_SIZEOF(smnIndexHeader);
    sIdxHeader.mRelHeader.mCreateTransId = SMA_GET_TRANS_ID( aStatement );
    sIdxHeader.mRelHeader.mDropTransId = SML_INVALID_TRANSID;
    sIdxHeader.mRelHeader.mCreateScn = 0;
    sIdxHeader.mRelHeader.mDropScn = 0;
    SME_SET_SEGMENT_HANDLE((void*)&sIdxHeader, aSegmentHandle);
    SME_SET_OBJECT_SCN((void*)&sIdxHeader, SML_INFINITE_SCN);
    SME_SET_RELATION_STATE((void*)&sIdxHeader, SME_RELATION_STATE_ACTIVE);

    sIdxHeader.mLoggingFlag = (((aAttr->mValidFlags & SML_INDEX_LOGGING_MASK) == SML_INDEX_LOGGING_YES) ?
                               aAttr->mLoggingFlag : sIsLogging);
    sIdxHeader.mIniTrans = (((aAttr->mValidFlags & SML_INDEX_INITRANS_MASK) == SML_INDEX_INITRANS_YES) ?
                            aAttr->mIniTrans : sIniTrans);
    sIdxHeader.mMaxTrans = (((aAttr->mValidFlags & SML_INDEX_MAXTRANS_MASK) == SML_INDEX_MAXTRANS_YES) ?
                            aAttr->mMaxTrans : sMaxTrans);
    sIdxHeader.mUniqueness = (((aAttr->mValidFlags & SML_INDEX_UNIQUENESS_MASK) == SML_INDEX_UNIQUENESS_YES) ?
                              aAttr->mUniquenessFlag : STL_FALSE);
    sIdxHeader.mPrimary = (((aAttr->mValidFlags & SML_INDEX_PRIMARY_MASK) == SML_INDEX_PRIMARY_YES) ?
                              aAttr->mPrimaryFlag : STL_FALSE);
    sIdxHeader.mPctFree = (((aAttr->mValidFlags & SML_INDEX_PCTFREE_MASK) == SML_INDEX_PCTFREE_YES) ?
                           aAttr->mPctFree : sPctFree);

    sIdxHeader.mBaseTableSegRid = smsGetSegRid(SME_GET_SEGMENT_HANDLE(aBaseTableRelHandle));
    sIdxHeader.mRootPageId = SMP_NULL_PID;
    sIdxHeader.mMirrorRootPid = SMP_NULL_PID;
    sIdxHeader.mIndexSmoNo = 0;

    sIdxHeader.mEmptyNodeCount = 0;
    sIdxHeader.mFirstEmptyNode = SMP_NULL_PID;
    sIdxHeader.mLastEmptyNode = SMP_NULL_PID;
    sIdxHeader.mBaseRelHandle = NULL;
    sIdxHeader.mModuleSpecificData = NULL;
    sIdxHeader.mIsAgingEmptyNode = STL_FALSE;
    sIdxHeader.mKeyColCount = aKeyColCount;
    sIdxHeader.m1ByteFastBuild = STL_TRUE;

    for( i = 0,sCol = aIndexColList; sCol != NULL; i++,sCol = sCol->mNext )
    {
        sKeyColTypes[i] = KNL_GET_BLOCK_DB_TYPE(sCol);
        sKeyColOrder[i] = KNL_GET_BLOCK_COLUMN_ORDER(sCol);
        sKeyColSize[i]  = KNL_GET_BLOCK_DATA_BUFFER_SIZE(sCol);

        if( KNL_GET_BLOCK_DATA_BUFFER_SIZE( sCol ) >= SMP_COLUMN_LEN_1BYTE_MAX )
        {
            sIdxHeader.m1ByteFastBuild = STL_FALSE;
        }
    }

    stlMemcpy( sIdxHeader.mKeyColTypes, sKeyColTypes, aKeyColCount );
    stlMemcpy( sIdxHeader.mKeyColFlags, aKeyColFlags, aKeyColCount );
    stlMemcpy( sIdxHeader.mKeyColOrder, sKeyColOrder, aKeyColCount * STL_SIZEOF(stlUInt32) );
    stlMemcpy( sIdxHeader.mKeyColSize, sKeyColSize, aKeyColCount * STL_SIZEOF(stlUInt64) );

    STL_TRY( smxmBegin( &sMiniTrans,
                        aStatement->mTransId,
                        sSegRid,
                        sAttr,
                        aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( smpAcquire( &sMiniTrans,
                         sSegRid.mTbsId,
                         sSegRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sSegPage,
                         aEnv ) == STL_SUCCESS );

    sPtr = (stlChar*)smpGetBody(SMP_FRAME(&sSegPage)) + smsGetHeaderSize( aSegmentHandle);

    stlMemcpy( sPtr, &sIdxHeader, STL_SIZEOF(smnIndexHeader) );

    STL_TRY( smxmWriteLog( &sMiniTrans,
                           SMG_COMPONENT_RELATION,
                           SMR_LOG_INIT_REL_HEADER,
                           SMR_REDO_TARGET_PAGE,
                           sPtr,
                           STL_SIZEOF(smnIndexHeader),
                           sSegPage.mPch->mTbsId,
                           sSegPage.mPch->mPageId,
                           sPtr - (stlChar*)SMP_FRAME(&sSegPage),
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    STL_TRY( smgAllocShmMem4Open( STL_SIZEOF( smnIndexHeader ),
                                  aRelationHandle,
                                  aEnv ) == STL_SUCCESS );
    sState = 2;

    stlMemcpy( *aRelationHandle,
               &sIdxHeader, 
               STL_SIZEOF(smnIndexHeader) );

    STL_TRY( smnmpbInitModuleSpecificData( *aRelationHandle, aEnv ) == STL_SUCCESS );
    sState = 3;
                 
    sCaches[0] = aSegmentHandle;
    sCaches[1] = *aRelationHandle;
    smpSetVolatileArea( &sSegPage, &sCaches, STL_SIZEOF(void*) * 2 );

    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 3 :
            sCaches[0] = aSegmentHandle;
            sCaches[1] = NULL;
            smpSetVolatileArea( &sSegPage, &sCaches, STL_SIZEOF(void*) * 2 );
            
            (void)smnmpbDestModuleSpecificData( *aRelationHandle, aEnv );
        case 2 :
            (void)smgFreeShmMem4Open( aRelationHandle, aEnv );
        case 1 :
            (void)smxmRollback( &sMiniTrans, aEnv );
        default :
            break;
    }

    return STL_FAILURE;
}


/**
 * @brief Index를 생성한다.
 * @param[in] aStatement Statement 객체
 * @param[in] aTbsId 생성할 Index가 속할 Tablespace ID
 * @param[in] aAttr 생성할 Index의 속성을 지정한 구조체
 * @param[in] aBaseTableHandle Index를 생성할 base Table의 handle
 * @param[in] aKeyColCount Index가 가지는 key column의 개수
 * @param[in] aIndexColList 각 key column들의 list. Fetch를 위한 Buffer와 type, base table order도 있음
 * @param[in] aKeyColFlags Index가 가지는 key column들의 flag 배열
 * @param[out] aSegmentId 생성된 Index의 물리적 아이디
 * @param[out] aRelationHandle 생성된 Index를 조작할때 사용할 메모리 핸들
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smnmpbCreate( smlStatement       * aStatement,
                        smlTbsId             aTbsId,
                        smlIndexAttr       * aAttr,
                        void               * aBaseTableHandle,
                        stlUInt16            aKeyColCount,
                        knlValueBlockList  * aIndexColList,
                        stlUInt8           * aKeyColFlags,
                        stlInt64           * aSegmentId,
                        void              ** aRelationHandle,
                        smlEnv             * aEnv )
{
    smxlTransId     sTransId = SMA_GET_TRANS_ID(aStatement);
    smlRid          sSegmentRid = SML_INVALID_RID;
    void          * sSegmentHandle;
    smnmpbPendArg   sPendArg;
    stlUInt32       sAttr = SMXM_ATTR_REDO;
    smlRid          sUndoRid;
    smxmTrans       sMiniTrans;
    stlInt32        sState = 0;

    STL_TRY( smxmBegin( &sMiniTrans,
                        sTransId,
                        sSegmentRid,
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

    STL_TRY( smxlInsertUndoRecord( &sMiniTrans,
                                   SMG_COMPONENT_INDEX,
                                   SMN_UNDO_LOG_MEMORY_BTREE_CREATE,
                                   NULL,
                                   0,
                                   sSegmentRid,
                                   &sUndoRid,
                                   aEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    KNL_BREAKPOINT( KNL_BREAKPOINT_SMNMPBCREATE_AFTER_ALLOCHDR,
                    KNL_ENV( aEnv ) );

    STL_TRY( smnmpbInitIndexHeader( aStatement,
                                    sSegmentHandle,
                                    aBaseTableHandle,
                                    aAttr,
                                    aKeyColCount,
                                    aIndexColList,
                                    aKeyColFlags,
                                    aRelationHandle,
                                    aEnv ) == STL_SUCCESS );

    *aSegmentId = smsGetSegmentId( sSegmentHandle );

    stlMemset( &sPendArg, 0x00, STL_SIZEOF(smnmpbPendArg) );
    sPendArg.mRelationHandle = *aRelationHandle;
    sPendArg.mTransId = sTransId;
    
    STL_TRY( smgAddPendOp( aStatement,
                           SMG_PEND_CREATE_INDEX,
                           SML_PEND_ACTION_COMMIT,
                           (void*)&sPendArg,
                           STL_SIZEOF( smnmpbPendArg ),
                           0,
                           aEnv ) == STL_SUCCESS );

    /**
     * no-logging index는 Pending Operation을 추가하지 않는다.
     * - rollback시 drop aging을 호출하기 때문에 지워진 segment를
     *   Aging할수 있다.
     */

    if( SMN_GET_LOGGING_FLAG( *aRelationHandle ) == STL_TRUE )
    {
        STL_TRY( smgAddPendOp( aStatement,
                               SMG_PEND_DROP_INDEX,
                               SML_PEND_ACTION_ROLLBACK,
                               (void*)&sPendArg,
                               STL_SIZEOF( smnmpbPendArg ),
                               28,
                               aEnv ) == STL_SUCCESS );
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
 * @brief Index 생성을 롤백한다.
 * @param[in] aTransId Transaction Identifier
 * @param[in] aRelationHandle 삭제할 Index의 relation handle
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smnmpbCreateUndo( smxlTransId   aTransId,
                            void        * aRelationHandle,
                            smlEnv      * aEnv )
{
    smxlTransId      sTransId = aTransId;
    stlUInt32        sAttr = SMXM_ATTR_REDO;
    smxmTrans        sMiniTrans;
    smpHandle        sPageHandle;
    stlInt16         sOffset;
    stlUInt16        sData[2];
    smnIndexHeader * sIndexHeader;
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

    sIndexHeader = (smnIndexHeader*)( smpGetBody(SMP_FRAME(&sPageHandle)) +
                                      smsGetHeaderSize( SME_GET_SEGMENT_HANDLE(aRelationHandle)) );

    sOffset = ( (stlChar*)&sIndexHeader->mRelHeader.mRelState -
                (stlChar*)SMP_FRAME(&sPageHandle) );

    sData[0] = sIndexHeader->mRelHeader.mRelState;
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
    
    sIndexHeader->mRelHeader.mRelState = SME_RELATION_STATE_DROPPING;
    sIndexHeader->mRelHeader.mDropTransId = sTransId;
    
    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    /*
     * 2. Update Relation Cache
     */
    SME_SET_RELATION_STATE( aRelationHandle, SME_RELATION_STATE_DROPPING );
    ((smnIndexHeader*)aRelationHandle)->mRelHeader.mDropTransId = sTransId;
    
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
            
        STL_TRY( smnmpbDropAging( sTransId,
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


/**
 * @brief Index를 삭제한다.
 * @param[in] aStatement Statement 객체
 * @param[in] aRelationHandle 삭제할 Index의 relation handle
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smnmpbDrop( smlStatement * aStatement,
                      void         * aRelationHandle,
                      smlEnv       * aEnv )
{
    smxlTransId      sTransId = SMA_GET_TRANS_ID(aStatement);
    smnmpbPendArg    sPendArg;
    stlUInt32        sAttr = SMXM_ATTR_REDO;
    smlRid           sUndoRid;
    smxmTrans        sMiniTrans;
    smpHandle        sPageHandle;
    stlInt16         sOffset;
    stlUInt16        sData[2];
    smnIndexHeader * sIndexHeader;
    smlRid           sSegmentRid;
    void           * sSegmentHandle;
    stlInt32         sState = 0;

    sSegmentHandle = SME_GET_SEGMENT_HANDLE( aRelationHandle );
    sSegmentRid = smsGetSegRid( sSegmentHandle );
    
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

    sIndexHeader = (smnIndexHeader*)( smpGetBody(SMP_FRAME(&sPageHandle)) +
                                      smsGetHeaderSize( sSegmentHandle ) );

    sOffset = ( (stlChar*)&sIndexHeader->mRelHeader.mRelState -
                (stlChar*)SMP_FRAME(&sPageHandle) );

    sData[0] = sIndexHeader->mRelHeader.mRelState;
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
                                   SMG_COMPONENT_INDEX,
                                   SMN_UNDO_LOG_MEMORY_BTREE_DROP,
                                   NULL,
                                   0,
                                   sSegmentRid,
                                   &sUndoRid,
                                   aEnv ) == STL_SUCCESS );

    sIndexHeader->mRelHeader.mRelState = SME_RELATION_STATE_DROPPING;
    sIndexHeader->mRelHeader.mDropTransId = sTransId;
    
    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    sTransId = SMA_GET_TRANS_ID(aStatement);
    
    /*
     * 2. Update Relation Cache
     */
    SME_SET_RELATION_STATE( aRelationHandle, SME_RELATION_STATE_DROPPING );
    ((smnIndexHeader*)aRelationHandle)->mRelHeader.mDropTransId = sTransId;
    
    /*
     * 3. Add Pending Operation
     */
    stlMemset( &sPendArg, 0x00, STL_SIZEOF(smnmpbPendArg) );
    sPendArg.mRelationHandle = aRelationHandle;
    sPendArg.mTransId = sTransId;
    
    /**
     * Offline Tablespace에 속한 테이블이라면 Pending Operation으로 처리한다.
     */
    
    if( SMF_IS_ONLINE_TBS( smsGetTbsId( sSegmentHandle ) ) == STL_FALSE )
    {
        STL_TRY( smdInsertPendingOperation( sTransId,
                                            smsGetTbsId( sSegmentHandle ),
                                            SMD_PENDING_OPERATION_DROP_SEGMENT,
                                            sSegmentHandle,
                                            smsGetCacheSize( sSegmentHandle ),
                                            aEnv )
                 == STL_SUCCESS );
        
        STL_TRY( smgAddPendOp( aStatement,
                               SMG_PEND_DROP_INDEX,
                               SML_PEND_ACTION_COMMIT,
                               (void*)&sPendArg,
                               STL_SIZEOF( smnmpbPendArg ),
                               28,
                               aEnv ) == STL_SUCCESS );
    }
    else
    {
        /**
         * Restart Recovery 단계에서 drop aging은 ager에 넘기지 않고 직접 처리한다.
         */
        if( smfGetServerState() == SML_SERVER_STATE_RECOVERING )
        {
            STL_TRY( smnmpbDropAging( sTransId,
                                      aRelationHandle,
                                      STL_TRUE,
                                      aEnv ) == STL_SUCCESS );
        }
        else
        {
            STL_TRY( smgAddPendOp( aStatement,
                                   SMG_PEND_DROP_INDEX,
                                   SML_PEND_ACTION_COMMIT,
                                   (void*)&sPendArg,
                                   STL_SIZEOF( smnmpbPendArg ),
                                   28,
                                   aEnv ) == STL_SUCCESS );
        }
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

stlStatus smnmpbAlterDataType( smlStatement  * aStatement,
                               void          * aRelationHandle,
                               stlInt32        aColumnOrder,
                               dtlDataType     aDataType,
                               stlInt64        aColumnSize,
                               smlEnv        * aEnv )
{
    smxmTrans             sMiniTrans;
    stlUInt32             sAttr = SMXM_ATTR_REDO;
    stlUInt32             sState = 0;
    void                * sSegmentHandle = NULL;
    smlRid                sSegmentRid;
    smpHandle             sPageHandle;
    smnIndexHeader      * sIndexHeader;
    smlRid                sUndoRid;
    smxlTransId           sTransId = SMA_GET_TRANS_ID( aStatement );
    smnIndexHeader      * sCache;
    stlChar               sBuffer[STL_SIZEOF(stlInt32) + STL_SIZEOF(stlUInt8) + STL_SIZEOF(stlInt64)];
    stlInt32              sOffset = 0;
    stlUInt8              sDataType = aDataType;

    sCache = (smnIndexHeader*)aRelationHandle;
    sSegmentHandle = SME_GET_SEGMENT_HANDLE( aRelationHandle );
    sSegmentRid = smsGetSegRid( sSegmentHandle );

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

    sIndexHeader = (smnIndexHeader*)( smpGetBody(SMP_FRAME(&sPageHandle)) +
                                      smsGetHeaderSize( sSegmentHandle ) );

    sOffset = 0;
    STL_WRITE_MOVE_INT32( sBuffer, &aColumnOrder, sOffset );
    STL_WRITE_MOVE_INT8( sBuffer, &(sCache->mKeyColTypes[aColumnOrder]), sOffset );
    STL_WRITE_MOVE_INT64( sBuffer, &(sCache->mKeyColSize[aColumnOrder]), sOffset );

    /**
     * logging undo log
     */

    STL_TRY( smxlInsertUndoRecord( &sMiniTrans,
                                   SMG_COMPONENT_INDEX,
                                   SMN_UNDO_LOG_ALTER_DATATYPE,
                                   (stlChar*)sBuffer,
                                   sOffset,
                                   sSegmentRid,
                                   &sUndoRid,
                                   aEnv ) == STL_SUCCESS );

    /**
     * applys table attribute to relation cache
     */

    sCache->mKeyColTypes[aColumnOrder] = sDataType;
    sCache->mKeyColSize[aColumnOrder] = aColumnSize;

    sIndexHeader->mKeyColTypes[aColumnOrder] = sDataType;
    sIndexHeader->mKeyColSize[aColumnOrder] = aColumnSize;

    sOffset = 0;
    STL_WRITE_MOVE_INT32( sBuffer, &aColumnOrder, sOffset );
    STL_WRITE_MOVE_INT8( sBuffer, &sDataType, sOffset );
    STL_WRITE_MOVE_INT64( sBuffer, &aColumnSize, sOffset );
    
    /**
     * logging redo log
     */
    
    STL_TRY( smxmWriteLog( &sMiniTrans,
                           SMG_COMPONENT_INDEX,
                           SMR_LOG_ALTER_DATATYPE,
                           SMR_REDO_TARGET_PAGE,
                           (stlChar*)sBuffer,
                           sOffset,
                           sSegmentRid.mTbsId,
                           sSegmentRid.mPageId,
                           (stlChar*)sIndexHeader - (stlChar*)SMP_FRAME(&sPageHandle),
                           SMXM_LOGGING_REDO,
                           aEnv )
             == STL_SUCCESS );
    
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

stlStatus smnmpbAlterIndexAttr( smlStatement * aStatement,
                                void         * aRelationHandle,
                                smlIndexAttr * aIndexAttr,
                                smlEnv       * aEnv )
{
    smxmTrans             sMiniTrans;
    stlUInt32             sAttr = SMXM_ATTR_REDO;
    stlUInt32             sState = 0;
    void                * sSegmentHandle = NULL;
    smlRid                sSegmentRid;
    smpHandle             sPageHandle;
    smnIndexHeader      * sIndexHeader;
    smlRid                sUndoRid;
    smlIndexAttr          sIndexAttr;
    smxlTransId           sTransId = SMA_GET_TRANS_ID( aStatement );
    smnIndexHeader      * sCache;
    stlInt16              sIniTrans;
    stlInt16              sMaxTrans;

    sCache = (smnIndexHeader*)aRelationHandle;
    sSegmentHandle = SME_GET_SEGMENT_HANDLE( aRelationHandle );
    sSegmentRid = smsGetSegRid( sSegmentHandle );

    /**
     * extract original physical attribute
     */
    
    sIndexAttr.mValidFlags = 0;
    sIndexAttr.mSegAttr.mValidFlags = 0;
    
    STL_TRY( smnmpbMergeIndexAttr( aRelationHandle,
                                   &sIndexAttr,
                                   aEnv )
             == STL_SUCCESS );

    /**
     * validate transaction attribute
     */
    
    if( (aIndexAttr->mValidFlags & SML_INDEX_INITRANS_MASK) ==
        SML_INDEX_INITRANS_YES )
    {
        sIniTrans = aIndexAttr->mIniTrans;
    }
    else
    {
        sIniTrans = sIndexAttr.mIniTrans;
    }
    
    if( (aIndexAttr->mValidFlags & SML_INDEX_MAXTRANS_MASK) ==
        SML_INDEX_MAXTRANS_YES )
    {
        sMaxTrans = aIndexAttr->mMaxTrans;
    }
    else
    {
        sMaxTrans = sIndexAttr.mMaxTrans;
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

    sIndexHeader = (smnIndexHeader*)( smpGetBody(SMP_FRAME(&sPageHandle)) +
                                      smsGetHeaderSize( sSegmentHandle ) );

    /**
     * logging undo log
     */
    
    STL_TRY( smxlInsertUndoRecord( &sMiniTrans,
                                   SMG_COMPONENT_INDEX,
                                   SMD_UNDO_LOG_MEMORY_BTREE_ALTER_ATTR,
                                   (stlChar*)&sIndexAttr,
                                   STL_SIZEOF(smlIndexAttr),
                                   sSegmentRid,
                                   &sUndoRid,
                                   aEnv ) == STL_SUCCESS );

    /**
     * applys index attribute to relation header
     */
    
    if( (aIndexAttr->mValidFlags & SML_INDEX_INITRANS_MASK) ==
        SML_INDEX_INITRANS_YES )
    {
        sIndexHeader->mIniTrans = aIndexAttr->mIniTrans;
    }
    
    if( (aIndexAttr->mValidFlags & SML_INDEX_MAXTRANS_MASK) ==
        SML_INDEX_MAXTRANS_YES )
    {
        sIndexHeader->mMaxTrans = aIndexAttr->mMaxTrans;
    }
    
    if( (aIndexAttr->mValidFlags & SML_INDEX_PCTFREE_MASK) ==
        SML_INDEX_PCTFREE_YES )
    {
        sIndexHeader->mPctFree = aIndexAttr->mPctFree;
    }
    
    /**
     * applys table attribute to relation cache
     */

    sCache->mIniTrans = sIndexHeader->mIniTrans;
    sCache->mMaxTrans = sIndexHeader->mMaxTrans;
    sCache->mPctFree = sIndexHeader->mPctFree;
    
    /**
     * logging redo log
     */
    
    STL_TRY( smxmWriteLog( &sMiniTrans,
                           SMG_COMPONENT_RELATION,
                           SMR_LOG_INIT_REL_HEADER,
                           SMR_REDO_TARGET_PAGE,
                           (stlChar*)sIndexHeader,
                           STL_SIZEOF(smnIndexHeader),
                           sSegmentRid.mTbsId,
                           sSegmentRid.mPageId,
                           (stlChar*)sIndexHeader - (stlChar*)SMP_FRAME(&sPageHandle),
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
                                  &aIndexAttr->mSegAttr,
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

stlStatus smnmpbMergeIndexAttr( void          * aRelationHandle,
                                smlIndexAttr  * aIndexAttr,
                                smlEnv        * aEnv )
{
    smlIndexAttr     sIndexAttr;
    smnIndexHeader * sIndexHeader;

    sIndexHeader = (smnIndexHeader*)aRelationHandle;

    sIndexAttr.mValidFlags = ( SML_INDEX_INITRANS_YES |
                               SML_INDEX_MAXTRANS_YES |
                               SML_INDEX_PCTFREE_YES );
    sIndexAttr.mIniTrans = sIndexHeader->mIniTrans;
    sIndexAttr.mMaxTrans = sIndexHeader->mMaxTrans;
    sIndexAttr.mPctFree = sIndexHeader->mPctFree;
    
    if( (aIndexAttr->mValidFlags & SML_INDEX_INITRANS_MASK) ==
        SML_INDEX_INITRANS_YES )
    {
        sIndexAttr.mIniTrans = aIndexAttr->mIniTrans;
    }
    
    if( (aIndexAttr->mValidFlags & SML_INDEX_MAXTRANS_MASK) ==
        SML_INDEX_MAXTRANS_YES )
    {
        sIndexAttr.mMaxTrans = aIndexAttr->mMaxTrans;
    }
    
    if( (aIndexAttr->mValidFlags & SML_INDEX_PCTFREE_MASK) ==
        SML_INDEX_PCTFREE_YES )
    {
        sIndexAttr.mPctFree = aIndexAttr->mPctFree;
    }
    
    smsMergeSegmentAttr( SME_GET_SEGMENT_HANDLE( aRelationHandle ),
                         &(aIndexAttr->mSegAttr),
                         &(sIndexAttr.mSegAttr) );
    
    *aIndexAttr = sIndexAttr;
    
    return STL_SUCCESS;
}

/**
 * @brief Truncate를 위해서 새로운 인덱스를 생성한다.
 * @param[in] aStatement Statement 객체
 * @param[in] aOrgRelHandle Truncate 이전의 원본 Relation Handle
 * @param[in] aNewBaseRelHandle Truncate을 위해 새로 생성된 Base Table의 Relation Handle
 * @param[in] aDropStorageOption extent의 drop 방법 지정
 * @param[out] aNewRelHandle 생성된 Relation Handle
 * @param[out] aNewSegmentId 생성된 Segment의 Identifier
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smnmpbCreateForTruncate( smlStatement          * aStatement,
                                   void                  * aOrgRelHandle,
                                   void                  * aNewBaseRelHandle,
                                   smlDropStorageOption    aDropStorageOption,
                                   void                 ** aNewRelHandle,
                                   stlInt64              * aNewSegmentId,
                                   smlEnv                * aEnv )
{
    smxlTransId      sTransId = SMA_GET_TRANS_ID(aStatement);
    void           * sOrgSegHandle;
    void           * sNewSegHandle;
    smnmpbPendArg    sPendArg;
    smxmTrans        sMiniTrans;
    stlInt32         sState = 0;
    smlRid           sNewSegmentRid = SML_INVALID_RID;
    smlRid           sUndoRid;
    stlUInt32        sAttr = SMXM_ATTR_REDO;
    stlInt32         sInitialExtents = SMS_MIN_EXTENT_COUNT;
    
    sOrgSegHandle = SME_GET_SEGMENT_HANDLE( aOrgRelHandle );

    STL_TRY( smxmBegin( &sMiniTrans,
                        sTransId,
                        sNewSegmentRid,
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
    sTransId = SMA_GET_TRANS_ID(aStatement);
    
    /*
     * New Segment Undo Logging
     */
    STL_TRY( smxlInsertUndoRecord( &sMiniTrans,
                                   SMG_COMPONENT_INDEX,
                                   SMN_UNDO_LOG_MEMORY_BTREE_CREATE,
                                   NULL,
                                   0,
                                   sNewSegmentRid,
                                   &sUndoRid,
                                   aEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    KNL_BREAKPOINT( KNL_BREAKPOINT_SMNMPBCREATEFORTRUNCATE_AFTER_ALLOCHDR,
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
                        aEnv )
             == STL_SUCCESS );

    STL_TRY( smnmpbInitHeaderForTruncate( aStatement,
                                          aOrgRelHandle,
                                          aNewBaseRelHandle,
                                          sNewSegHandle,
                                          aNewRelHandle,
                                          aEnv ) == STL_SUCCESS );

    /*
     * Add Pending Operation
     */
    stlMemset( &sPendArg, 0x00, STL_SIZEOF(smnmpbPendArg) );
    sPendArg.mRelationHandle = *aNewRelHandle;
    sPendArg.mTransId = sTransId;
    
    STL_TRY( smgAddPendOp( aStatement,
                           SMG_PEND_CREATE_INDEX,
                           SML_PEND_ACTION_COMMIT,
                           (void*)&sPendArg,
                           STL_SIZEOF( smnmpbPendArg ),
                           0,
                           aEnv ) == STL_SUCCESS );

    /**
     * no-logging index는 Pending Operation을 추가하지 않는다.
     * - rollback시 drop aging을 호출하기 때문에 지워진 segment를
     *   Aging할수 있다.
     */

    if( SMN_GET_LOGGING_FLAG( *aNewRelHandle ) == STL_TRUE )
    {
        STL_TRY( smgAddPendOp( aStatement,
                               SMG_PEND_DROP_INDEX,
                               SML_PEND_ACTION_ROLLBACK,
                               (void*)&sPendArg,
                               STL_SIZEOF( smnmpbPendArg ),
                               28,
                               aEnv ) == STL_SUCCESS );
    }
    
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

stlStatus smnmpbInitHeaderForTruncate( smlStatement  * aStatement,
                                       void          * aOrgRelHandle,
                                       void          * aNewBaseRelHandle,
                                       void          * aNewSegHandle,
                                       void         ** aNewRelHandle,
                                       smlEnv        * aEnv )
{
    smxmTrans             sMiniTrans;
    stlUInt32             sAttr = SMXM_ATTR_REDO;
    void                * sCaches[2];
    stlUInt32             sState = 0;
    void                * sOrgSegHandle = NULL;
    smlRid                sOrgSegmentRid;
    smpHandle             sOrgPageHandle;
    smnIndexHeader      * sOrgIndexHeader;
    smpHandle             sNewPageHandle;
    smnIndexHeader      * sNewIndexHeader;
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

    sOrgIndexHeader = (smnIndexHeader*)( smpGetBody(SMP_FRAME(&sOrgPageHandle)) +
                                         smsGetHeaderSize( sOrgSegHandle ) );

    STL_TRY( smpAcquire( &sMiniTrans,
                         sNewSegmentRid.mTbsId,
                         sNewSegmentRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sNewPageHandle,
                         aEnv ) == STL_SUCCESS );

    sNewIndexHeader = (smnIndexHeader*)( smpGetBody(SMP_FRAME(&sNewPageHandle)) +
                                         smsGetHeaderSize(aNewSegHandle) );

    stlMemcpy( sNewIndexHeader, sOrgIndexHeader, STL_SIZEOF(smnIndexHeader) );
    SME_SET_OBJECT_SCN( sNewIndexHeader, SML_INFINITE_SCN );
    SME_SET_RELATION_STATE( sNewIndexHeader, SME_RELATION_STATE_ACTIVE );
    sNewIndexHeader->mBaseTableSegRid = smsGetSegRid(SME_GET_SEGMENT_HANDLE(aNewBaseRelHandle));
    sNewIndexHeader->mMirrorRootPid = SMP_NULL_PID;
    sNewIndexHeader->mRootPageId = SMP_NULL_PID;
    sNewIndexHeader->mIndexSmoNo = 0;
    sNewIndexHeader->mEmptyNodeCount = 0;
    sNewIndexHeader->mFirstEmptyNode = SMP_NULL_PID;
    sNewIndexHeader->mLastEmptyNode = SMP_NULL_PID;
    sNewIndexHeader->mBaseRelHandle = NULL;
    sNewIndexHeader->mModuleSpecificData = NULL;
    sNewIndexHeader->mIsAgingEmptyNode = STL_FALSE;

    STL_TRY( smxmWriteLog( &sMiniTrans,
                           SMG_COMPONENT_RELATION,
                           SMR_LOG_INIT_REL_HEADER,
                           SMR_REDO_TARGET_PAGE,
                           (void*)sNewIndexHeader,
                           STL_SIZEOF(smnIndexHeader),
                           sNewSegmentRid.mTbsId,
                           sNewSegmentRid.mPageId,
                           (stlChar*)sNewIndexHeader - (stlChar*)SMP_FRAME(&sNewPageHandle),
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    STL_TRY( smgAllocShmMem4Open( STL_SIZEOF( smnIndexHeader ),
                                  aNewRelHandle,
                                  aEnv ) == STL_SUCCESS );
    sState = 2;

    stlMemcpy( *aNewRelHandle, sNewIndexHeader, STL_SIZEOF(smnIndexHeader) );
    SME_SET_SEGMENT_HANDLE( *aNewRelHandle, aNewSegHandle );

    STL_TRY( smnmpbInitModuleSpecificData( *aNewRelHandle, aEnv ) == STL_SUCCESS );
    sState = 3;
    
    sCaches[0] = aNewSegHandle;
    sCaches[1] = *aNewRelHandle;
    smpSetVolatileArea( &sNewPageHandle, &sCaches, STL_SIZEOF(void*) * 2 );

    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 3 :
            sCaches[0] = aNewSegHandle;
            sCaches[1] = NULL;
            smpSetVolatileArea( &sNewPageHandle, &sCaches, STL_SIZEOF(void*) * 2 );
    
            (void)smnmpbDestModuleSpecificData( *aNewRelHandle, aEnv );
        case 2 :
            (void)smgFreeShmMem4Open( *aNewRelHandle, aEnv );
        case 1 :
            (void)smxmRollback( &sMiniTrans, aEnv );
        default :
            break;
    }

    return STL_FAILURE;
}

stlStatus smnmpbInitialize( smlStatement * aStatement,
                            void         * aRelationHandle,
                            smlEnv       * aEnv )
{
    return STL_SUCCESS;
}


stlStatus smnmpbFinalize( smlStatement * aStatement,
                          void         * aRelationHandle,
                          smlEnv       * aEnv )
{
    return STL_SUCCESS;
}


stlStatus smnmpbBuildForTruncate( smlStatement     * aStatement,
                                  void             * aOrgIndexHandle,
                                  void             * aNewIndexHandle,
                                  smlEnv           * aEnv )
{
    smxmTrans             sMiniTrans;
    stlUInt16             sState = 0;
    stlUInt32             sAttr;
    stlUInt64             sSmoNo = 0;
    void                * sOrgSegHandle;
    void                * sNewSegHandle;
    smlRid                sNewSegRid;
    smpHandle             sOrgRootPageHandle;
    smpHandle             sNewRootPageHandle;
    smpHandle             sNewSegPageHandle;
    smnIndexHeader      * sNewIndexHeader;
    smnmpbLogicalHdr      sNewLogicalHdr;
    smnmpbLogicalHdr    * sOrgLogicalHdr;
    stlBool               sIsLogging = ((smnIndexHeader*)aOrgIndexHandle)->mLoggingFlag;
    
    sOrgSegHandle = SME_GET_SEGMENT_HANDLE( aOrgIndexHandle );
    sNewSegHandle = SME_GET_SEGMENT_HANDLE( aNewIndexHandle );
    sNewSegRid = smsGetSegRid( sNewSegHandle );

    if( sIsLogging == STL_TRUE )
    {
        sAttr = SMXM_ATTR_REDO;
    }
    else
    {
        sAttr = SMXM_ATTR_NOLOG_DIRTY;
    }
    
    /*    
     * Index 생성시 logical header에 key column 정보를 저장하기 위해
     * root page 한 개는 최소한 있어야 함
     */
    STL_TRY( smxmBegin( &sMiniTrans,
                        aStatement->mTransId,
                        sNewSegRid,
                        sAttr,
                        aEnv ) == STL_SUCCESS );
    sState = 1;

    /*
     * 원본 Root Page로 부터 새로운 Root Page를 만든다
     */
    STL_TRY( smpAcquire( &sMiniTrans,
                         smsGetTbsId(sOrgSegHandle),
                         ((smnIndexHeader*)aOrgIndexHandle)->mRootPageId,
                         KNL_LATCH_MODE_SHARED,
                         &sOrgRootPageHandle,
                         aEnv ) == STL_SUCCESS );

    STL_TRY( smsAllocPage( &sMiniTrans,
                           sNewSegHandle,
                           SMP_PAGE_TYPE_INDEX_DATA,
                           NULL,  /* aIsAgableFunc */
                           &sNewRootPageHandle,
                           aEnv ) == STL_SUCCESS );

    /* SMO No 초기화 */
    smpSetVolatileArea( &sNewRootPageHandle,
                        &sSmoNo,
                        STL_SIZEOF(stlUInt64) );

    sOrgLogicalHdr = SMNMPB_GET_LOGICAL_HEADER( &sOrgRootPageHandle );
    
    stlMemset( &sNewLogicalHdr, 0x00, STL_SIZEOF(smnmpbLogicalHdr) );
    sNewLogicalHdr.mPrevPageId     = SMP_NULL_PID;
    sNewLogicalHdr.mNextPageId     = SMP_NULL_PID;
    sNewLogicalHdr.mNextFreePageId = SMP_NULL_PID;
    sNewLogicalHdr.mLevel          = 0;
    sNewLogicalHdr.mKeyColCount    = sOrgLogicalHdr->mKeyColCount;
    
    stlMemcpy( &sNewLogicalHdr.mKeyColTypes,
               &sOrgLogicalHdr->mKeyColTypes,
               sOrgLogicalHdr->mKeyColCount );
    stlMemcpy( &sNewLogicalHdr.mKeyColFlags,
               &sOrgLogicalHdr->mKeyColFlags,
               sOrgLogicalHdr->mKeyColCount );

    STL_TRY( smpInitBody( &sNewRootPageHandle,
                          SMP_ORDERED_SLOT_TYPE,
                          STL_SIZEOF(smnmpbLogicalHdr),
                          (void*)&sNewLogicalHdr,
                          SMN_GET_INITRANS(aNewIndexHandle),
                          SMN_GET_MAXTRANS(aNewIndexHandle),
                          STL_FALSE,
                          &sMiniTrans,
                          aEnv) == STL_SUCCESS );

    /*
     * Index Header에 Root Page Id 설정 & 로깅
     */
    STL_TRY( smpAcquire( &sMiniTrans,
                         sNewSegRid.mTbsId,
                         sNewSegRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sNewSegPageHandle,
                         aEnv ) == STL_SUCCESS );

    sNewIndexHeader = (smnIndexHeader*)( (stlChar*)smpGetBody(SMP_FRAME(&sNewSegPageHandle)) +
                                         smsGetHeaderSize(sNewSegHandle) );

    sNewIndexHeader->mRootPageId = smpGetPageId( &sNewRootPageHandle );
    SMN_SET_ROOT_PAGE_ID( aNewIndexHandle, smpGetPageId(&sNewRootPageHandle) );

    /**
     * leaf가 root인 경우는 mirroring 시키지 않는다.
     */
    sNewIndexHeader->mMirrorRootPid = SMP_NULL_PID;
    SMN_SET_MIRROR_ROOT_PID( aNewIndexHandle, SMP_NULL_PID );
    
    STL_TRY( smxmWriteLog( &sMiniTrans,
                           SMG_COMPONENT_INDEX,
                           SMR_LOG_SET_ROOT_PAGE_ID,
                           SMR_REDO_TARGET_PAGE,
                           (stlChar*)&sNewIndexHeader->mRootPageId,
                           STL_SIZEOF(smlPid),
                           sNewSegRid.mTbsId,
                           sNewSegRid.mPageId,
                           (stlChar*)sNewIndexHeader - (stlChar*)SMP_FRAME(&sNewSegPageHandle),
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

	return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        smxmRollback( &sMiniTrans, aEnv );
    }

    return STL_FAILURE;
}


stlStatus smnmpbCreatePending( stlUInt32   aActionFlag,
                               smgPendOp * aPendOp,
                               smlEnv    * aEnv )
{
    smnmpbPendArg     * sPendArg = (smnmpbPendArg*)aPendOp->mArgs;
    smeRelationHeader * sRelationHeader;

    sRelationHeader = (smeRelationHeader*)(sPendArg->mRelationHandle);

    STL_TRY( smxlGetCommitScnFromCache( sPendArg->mTransId,
                                        &sRelationHeader->mObjectScn,
                                        aEnv ) == STL_SUCCESS );
    sRelationHeader->mCreateScn = sRelationHeader->mObjectScn;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smnmpbDropPending( stlUInt32   aActionFlag,
                             smgPendOp * aPendOp,
                             smlEnv    * aEnv )
{
    smnmpbPendArg     * sPendArg = (smnmpbPendArg*)aPendOp->mArgs;
    smeRelationHeader * sRelationHeader;
    stlInt32            sOffset = 0;
    stlChar             sBuffer[32];
    smlRid              sSegRid;
    smlIndexType        sIndexType;
    smlScn              sObjectAgableScn;

    sRelationHeader = (smeRelationHeader*)(sPendArg->mRelationHandle);

    STL_TRY( smxlGetCommitScnFromCache( sPendArg->mTransId,
                                        &sRelationHeader->mObjectScn,
                                        aEnv ) == STL_SUCCESS );
    sRelationHeader->mDropScn = sRelationHeader->mObjectScn;

    sSegRid = smsGetSegRid( SME_GET_SEGMENT_HANDLE(sPendArg->mRelationHandle) );
    sIndexType = SML_GET_INDEX_TYPE( sRelationHeader->mRelationType );

    sObjectAgableScn = smxlGetNextSystemScn();

    STL_WRITE_MOVE_INT64( sBuffer, &sRelationHeader->mObjectScn, sOffset );
    STL_WRITE_MOVE_INT64( sBuffer, &sObjectAgableScn, sOffset );
    STL_WRITE_MOVE_INT32( sBuffer, &sIndexType, sOffset );
    SMG_WRITE_MOVE_RID( sBuffer, &sSegRid, sOffset );

    STL_DASSERT( sOffset < STL_SIZEOF( sBuffer ) );
    STL_DASSERT( sOffset <= aPendOp->mEventMemSize );

    STL_TRY( knlAddEnvEvent( SML_EVENT_DROP_INDEX,
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


stlStatus smnmpbDropAging( smxlTransId   aTransId,
                           void        * aRelationHandle,
                           stlBool       aOnStartup,
                           smlEnv      * aEnv )
{
    /**
     * 롤백하다가 비정상 종료된 경우에는 Segment는 Drop되었지만
     * Undo Record에 SMXL_UNDO_RECORD_DELETED가 설정되지 않는 경우가 있다.
     * 이러한 경우에는 Undo를 Skip한다.
     */
    if( aRelationHandle != NULL )
    {
        /**
         * Segment를 물리적으로 삭제한다.
         */
        STL_TRY( smsDrop( aTransId,
                          SME_GET_SEGMENT_HANDLE(aRelationHandle),
                          aOnStartup,
                          aEnv ) == STL_SUCCESS );

        /**
         * Relation Cache를 삭제한다.
         */
        
        STL_TRY( smnmpbDestModuleSpecificData( aRelationHandle,
                                               aEnv )
                 == STL_SUCCESS );
        
        STL_TRY( smgFreeShmMem4Open( aRelationHandle,
                                     aEnv ) == STL_SUCCESS );
    }
                                    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smnmpbInitModuleSpecificData( void   * aRelationHandle,
                                        smlEnv * aEnv )
{
    smnmpbStatistics * sStatistics;
    
    STL_TRY( smgAllocShmMem4Open( STL_SIZEOF( smnmpbStatistics ),
                                  (void*)&sStatistics,
                                  aEnv ) == STL_SUCCESS );
    
    SMNMPB_INIT_STATISTICS( sStatistics );
    SMN_SET_MODULE_SPECIFIC_DATA( aRelationHandle, sStatistics );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smnmpbDestModuleSpecificData( void   * aRelationHandle,
                                        smlEnv * aEnv )
{
    smnmpbStatistics * sStatistics;

    sStatistics = SMNMPB_GET_MODULE_SPECIFIC_DATA( aRelationHandle );

    STL_DASSERT( sStatistics != NULL );
    
    STL_TRY( smgFreeShmMem4Open( sStatistics,
                                 aEnv )
             == STL_SUCCESS );
    
    SMN_SET_MODULE_SPECIFIC_DATA( aRelationHandle, NULL );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
