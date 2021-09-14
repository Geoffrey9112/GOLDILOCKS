/*******************************************************************************
 * smnUndo.c
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
#include <smp.h>
#include <smxl.h>
#include <smxm.h>
#include <sme.h>
#include <sms.h>
#include <smt.h>
#include <smf.h>
#include <smr.h>
#include <smnDef.h>
#include <smnmpb.h>
#include <smnmpbDef.h>
#include <smnUndo.h>
#include <smd.h>

/**
 * @file smnUndo.c
 * @brief Storage Manager Index Undo Internal Routines
 */

/**
 * @addtogroup smnUndo
 * @{
 */

extern smnIndexModule * gSmnIndexModules[];

smxlUndoFunc gSmnUndoFuncs[SMN_UNDO_LOG_MAX_COUNT] =
{
    smnUndoMemoryBtreeInsert,
    smnUndoMemoryBtreeDupKey,
    smnUndoMemoryBtreeDelete,
    smnUndoMemoryBtreeCreate,
    smnUndoMemoryBtreeDrop,
    smnUndoMemoryBtreeBlockInsert,
    smnUndoMemoryBtreeBlockDelete,
    smnUndoMemoryBtreeAlterDatatype,
    smnUndoMemoryBtreeAlterAttr
};

stlStatus smnUndoMemoryBtreeInsert( smxmTrans * aMiniTrans,
                                    smlRid      aTargetRid,
                                    void      * aLogBody,
                                    stlInt16    aLogSize,
                                    smlEnv    * aEnv )
{
    knlValueBlockList * sValCols = NULL;
    knlValueBlockList * sCurCol;
    knlValueBlockList * sPrevCol;
    stlChar             sValColBuf[SMP_PAGE_SIZE];
    stlChar           * sColPtr;
    stlChar           * sPtr = (stlChar*)aLogBody;
    smlRid              sRowRid;
    stlInt16            i;
    smlRid              sSegRid = smxmGetSegRid(aMiniTrans);
    void              * sRelationHandle;
    stlBool             sIsUniqueResolved;
    knlRegionMark       sMemMark;
    stlInt32            sState = 0;
    smnIndexHeader    * sIndexHeader;
    smlRid              sUndoRid = SML_INVALID_RID;
    smxlTransId         sTransId;
    stlUInt32           sAttr;
    stlInt16            sEmptyPageCount;
    smlPid              sEmptyPidList[KNL_ENV_MAX_LATCH_DEPTH];

    /* minitransaction 상태 정보 저장 */
    sAttr = smxmGetAttr( aMiniTrans );
    sTransId = smxmGetTransId( aMiniTrans );

    STL_TRY( smeGetRelationHandle( sSegRid, &sRelationHandle, aEnv) == STL_SUCCESS );
    sIndexHeader = (smnIndexHeader*)sRelationHandle;
    
    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;

    sPrevCol = NULL;
    for( i = 0; i < sIndexHeader->mKeyColCount; i++ )
    {
        STL_TRY( knlInitBlockNoBuffer( &sCurCol,
                                       STL_LAYER_STORAGE_MANAGER,
                                       1,
                                       sIndexHeader->mKeyColTypes[i],
                                       &aEnv->mOperationHeapMem,
                                       (knlEnv*)aEnv ) == STL_SUCCESS );
        if( i == 0 )
        {
            sValCols = sCurCol;
        }
        else
        {
            KNL_LINK_BLOCK_LIST( sPrevCol, sCurCol );
        }
        sPrevCol = sCurCol;
    }

    sRowRid.mTbsId = 0; /* 사용 안함 */ 
    STL_WRITE_INT32( &sRowRid.mPageId, sPtr );
    sPtr += STL_SIZEOF(smlPid);
    STL_WRITE_INT16( &sRowRid.mOffset, sPtr );
    sPtr += STL_SIZEOF(stlInt16);

    sColPtr = sPtr;
    smnmpbMakeValueListFromSlot( sColPtr, sValCols, sValColBuf );

    STL_TRY( smnmpbDeleteInternal( NULL, /* aStatement */
                                   aMiniTrans,
                                   sRelationHandle,
                                   sValCols,
                                   0,
                                   &sRowRid,
                                   &sUndoRid,
                                   STL_FALSE,
                                   &sIsUniqueResolved,
                                   &sEmptyPageCount,
                                   sEmptyPidList,
                                   aEnv ) == STL_SUCCESS );

    /* Delete 연산 후 empty가 된 node들을 empty node list에 연결한다
     * Empty node을 list에 연결할 때 Aging이 진행중이면 busy wait을 하게 되고
     * 이때 lock을 잡고 있으면 deadlock이 발생할 수 있기 때문에
     * delete연산 후 mini-transaction을 새로 begin해야한다.
     */
    if( sEmptyPageCount > 0 )
    {
        STL_TRY( smxmCommit( aMiniTrans, aEnv ) == STL_SUCCESS );

        STL_TRY( smxmBegin( aMiniTrans,
                            sTransId,
                            smsGetSegRid(SME_GET_SEGMENT_HANDLE(sRelationHandle)),
                            sAttr,
                            aEnv ) == STL_SUCCESS );

        STL_TRY( smnmpbRegisterEmptyNode( sRelationHandle,
                                          aMiniTrans,
                                          sEmptyPageCount,
                                          sEmptyPidList,
                                          STL_TRUE, /* aCheckAgingFlag */
                                          aEnv ) == STL_SUCCESS );
    }

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       (knlEnv*)aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                        &sMemMark,
                                        STL_FALSE, /* aFreeChunk */
                                        (knlEnv*)aEnv );
    }

    return STL_FAILURE;
}


stlStatus smnUndoMemoryBtreeDupKey( smxmTrans * aMiniTrans,
                                    smlRid      aTargetRid,
                                    void      * aLogBody,
                                    stlInt16    aLogSize,
                                    smlEnv    * aEnv )
{
    return smnUndoMemoryBtreeInsert( aMiniTrans,
                                     aTargetRid,
                                     aLogBody,
                                     aLogSize,
                                     aEnv );
}

stlStatus smnUndoMemoryBtreeDelete( smxmTrans * aMiniTrans,
                                    smlRid      aTargetRid,
                                    void      * aLogBody,
                                    stlInt16    aLogSize,
                                    smlEnv    * aEnv )
{
    knlValueBlockList * sValCols = NULL;
    knlValueBlockList * sCurCol;
    knlValueBlockList * sPrevCol = NULL;
    stlChar             sValColBuf[SMP_PAGE_SIZE];
    stlChar           * sColPtr;
    smlRid              sRowRid;
    stlInt16            i;
    smlRid              sSegRid = smxmGetSegRid(aMiniTrans);
    void              * sRelationHandle;
    knlRegionMark       sMemMark;
    stlInt32            sState = 0;
    smnIndexHeader    * sIndexHeader;

    STL_TRY( smeGetRelationHandle( sSegRid, &sRelationHandle, aEnv) == STL_SUCCESS );
    sIndexHeader = (smnIndexHeader*)sRelationHandle;
    
    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;

    sPrevCol = NULL;
    for( i = 0; i < sIndexHeader->mKeyColCount; i++ )
    {
        STL_TRY( knlInitBlockNoBuffer( &sCurCol,
                                       STL_LAYER_STORAGE_MANAGER,
                                       1,
                                       sIndexHeader->mKeyColTypes[i],
                                       &aEnv->mOperationHeapMem,
                                       (knlEnv*)aEnv ) == STL_SUCCESS );
        if( i == 0 )
        {
            sValCols = sCurCol;
        }
        else
        {
            KNL_LINK_BLOCK_LIST( sPrevCol, sCurCol );
        }
        sPrevCol = sCurCol;
    }

    sColPtr = (stlChar*)aLogBody;
    sColPtr += SMNMPB_IS_TBK(aLogBody) != STL_TRUE ? SMNMPB_RBK_HDR_SIZE : SMNMPB_TBK_HDR_SIZE;
    smnmpbMakeValueListFromSlot( sColPtr, sValCols, sValColBuf );
    sRowRid.mTbsId = 0; /* 사용 안함 */ 
    SMNMPB_GET_ROW_PID( (stlChar*)aLogBody, &sRowRid.mPageId );
    SMNMPB_GET_ROW_SEQ( (stlChar*)aLogBody, &sRowRid.mOffset );

    STL_TRY( smnmpbUndeleteInternal( NULL, /* aStatement */
                                     aMiniTrans,
                                     sRelationHandle,
                                     sValCols,
                                     &sRowRid,
                                     aEnv )
             == STL_SUCCESS );
    


    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       (knlEnv*)aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                        &sMemMark,
                                        STL_FALSE, /* aFreeChunk */
                                        (knlEnv*)aEnv );
    }

    return STL_FAILURE;
}

stlStatus smnUndoMemoryBtreeCreate( smxmTrans * aMiniTrans,
                                    smlRid      aTargetRid,
                                    void      * aLogBody,
                                    stlInt16    aLogSize,
                                    smlEnv    * aEnv )
{
    smxlTransId   sTransId;
    void         * sRelationHandle;
    void         * sSegmentHandle;
    smlIndexType   sIndexType;
    stlBool        sOnStartup = STL_FALSE;

    sTransId = smxmGetTransId( aMiniTrans );
    
    if( (smfGetServerState() == SML_SERVER_STATE_RECOVERING) ||
        (smrProceedMediaRecovery(aEnv) == STL_TRUE) )
    {
        sOnStartup = STL_TRUE;
    }

    STL_TRY( smeGetRelationHandle( aTargetRid,
                                   &sRelationHandle,
                                   aEnv )
             == STL_SUCCESS);

    STL_TRY( smsGetSegmentHandle( aTargetRid,
                                  &sSegmentHandle,
                                  aEnv )
             == STL_SUCCESS);

    if( sRelationHandle != NULL )
    {
        sIndexType = SML_GET_INDEX_TYPE( SME_GET_RELATION_TYPE( sRelationHandle ) );
        
        if( smtIsLogging( smsGetTbsId( sSegmentHandle ) ) == STL_FALSE )
        {
            STL_TRY( gSmnIndexModules[sIndexType]->mDropAging( sTransId,
                                                               sRelationHandle,
                                                               sOnStartup,
                                                               SML_ENV( aEnv ) )
                     == STL_SUCCESS );
        }
        else
        {
            /**
             * new mini-transaction의 사용은 undo page의 delete flag와 atomic함을 보장하지
             * 못한다. 따라서, restart로 인하여 create undo가 여러번 호출될수 있음을
             * 고려해야 한다.
             */
    
            if( SME_GET_RELATION_STATE( sRelationHandle ) != SME_RELATION_STATE_DROPPING )
            {
                STL_TRY( gSmnIndexModules[sIndexType]->mCreateUndo( sTransId,
                                                                    sRelationHandle,
                                                                    SML_ENV( aEnv ) )
                         == STL_SUCCESS );
            }
        }
    }
    else
    {
        if( sSegmentHandle != NULL )
        {
            STL_TRY( smsDrop( sTransId,
                              sSegmentHandle,
                              sOnStartup,
                              aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /**
             * Segment Header만 할당되어 있는 경우
             */
            STL_TRY( smsFreeSegMapPage( sTransId,
                                        aTargetRid,
                                        aEnv ) == STL_SUCCESS );
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smnUndoMemoryBtreeDrop( smxmTrans * aMiniTrans,
                                  smlRid      aTargetRid,
                                  void      * aLogBody,
                                  stlInt16    aLogSize,
                                  smlEnv    * aEnv )
{
    smpHandle        sPageHandle;
    stlInt16         sOffset;
    stlUInt16        sData[2];
    void           * sSegmentHandle;
    void           * sRelationHandle;
    smnIndexHeader * sIndexHeader;

    STL_TRY( smeGetRelationHandle( aTargetRid,
                                   &sRelationHandle,
                                   aEnv ) == STL_SUCCESS);
    sSegmentHandle = SME_GET_SEGMENT_HANDLE( sRelationHandle );
    
    STL_TRY( smpAcquire( aMiniTrans,
                         aTargetRid.mTbsId,
                         aTargetRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

    sIndexHeader = (smnIndexHeader*)( smpGetBody(SMP_FRAME(&sPageHandle)) +
                                      smsGetHeaderSize( sSegmentHandle) );

    sOffset = ( (stlChar*)&sIndexHeader->mRelHeader.mRelState -
                (stlChar*)SMP_FRAME(&sPageHandle) );

    sData[0] = sIndexHeader->mRelHeader.mRelState;
    sIndexHeader->mRelHeader.mRelState = SME_RELATION_STATE_ACTIVE;
    sData[1] = sIndexHeader->mRelHeader.mRelState;

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_RECOVERY,
                           SMR_LOG_BYTES,
                           SMR_REDO_TARGET_PAGE,
                           (void*)sData,
                           STL_SIZEOF( stlUInt16 ) * 2,
                           aTargetRid.mTbsId,
                           aTargetRid.mPageId,
                           sOffset,
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    SME_SET_RELATION_STATE( sRelationHandle, SME_RELATION_STATE_ACTIVE );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smnUndoMemoryBtreeBlockInsert( smxmTrans * aMiniTrans,
                                         smlRid      aTargetRid,
                                         void      * aLogBody,
                                         stlInt16    aLogSize,
                                         smlEnv    * aEnv )
{
    smlRid              sSegRid = smxmGetSegRid(aMiniTrans);
    void              * sIndexRelationHandle;
    void              * sBaseRelationHandle;
    stlBool             sIsUniqueResolved;
    knlRegionMark       sMemMark;
    stlInt32            sState = 0;
    stlInt32            sOrgTransState = 1;
    smlRid              sUndoRid = SML_INVALID_RID;
    smlRid              sRowRid[SMN_MAX_BLOCK_KEY_COUNT];
    stlInt8             sBlockCount;
    smlScn              sStmtViewScn;
    smlTcn              sStmtTcn;
    stlInt32            i;
    stlInt32            j;
    stlInt32            k;
    knlValueBlockList * sIndexColList = NULL;
    knlValueBlockList * sCurIndexCol;
    knlValueBlockList * sPrevIndexCol = NULL;
    knlValueBlockList   sTableColList[SML_MAX_INDEX_KEY_COL_COUNT];
    stlChar           * sValue;
    smnIndexHeader    * sIndexHeader;
    stlInt32            sOffset = 0;
    smxmTrans           sMiniTrans;
    smxmTrans           sSnapshotMtx;
    stlInt16            sEmptyPageCount;
    smlPid              sEmptyPidList[KNL_ENV_MAX_LATCH_DEPTH];
    smpCtrlHeader       sPageStack[KNL_ENV_MAX_LATCH_DEPTH];
    stlUInt32           sAttr;

    STL_TRY( smeGetRelationHandle( sSegRid,
                                   &sIndexRelationHandle,
                                   aEnv)
             == STL_SUCCESS );
    sIndexHeader = (smnIndexHeader*)sIndexRelationHandle;
    STL_TRY( smeGetRelationHandle( SMN_GET_BASE_TABLE_SEGMENT_RID( sIndexRelationHandle ),
                                   &sBaseRelationHandle,
                                   aEnv)
             == STL_SUCCESS );
    
    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;

    /**
     * Index column list 구성
     */
    
    sPrevIndexCol = NULL;
    for( i = 0; i < sIndexHeader->mKeyColCount; i++ )
    {
        STL_TRY( knlInitBlockNoBuffer( &sCurIndexCol,
                                       STL_LAYER_STORAGE_MANAGER,
                                       1,
                                       sIndexHeader->mKeyColTypes[i],
                                       &aEnv->mOperationHeapMem,
                                       (knlEnv*)aEnv ) == STL_SUCCESS );
        if( i == 0 )
        {
            sIndexColList = sCurIndexCol;
        }
        else
        {
            KNL_LINK_BLOCK_LIST( sPrevIndexCol, sCurIndexCol );
        }

        STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                    sIndexHeader->mKeyColSize[i],
                                    (void**)&sValue,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        KNL_SET_BLOCK_DATA_PTR( sCurIndexCol, 0, sValue, sIndexHeader->mKeyColSize[i] );
        KNL_SET_BLOCK_COLUMN_ORDER( sCurIndexCol, sIndexHeader->mKeyColOrder[i] );
        
        sPrevIndexCol = sCurIndexCol;
    }
    
    /**
     * Table column list 구성
     */
    
    sTableColList[0] = *sIndexColList;
    sCurIndexCol = sIndexColList->mNext;

    for( i = 1; i < sIndexHeader->mKeyColCount; i++ )
    {
        for( j = 0; j < i; j++ )
        {
            if( sTableColList[j].mColumnOrder > sCurIndexCol->mColumnOrder )
            {
                for( k = i - 1; k >= j; k--)
                {
                    sTableColList[k + 1] = sTableColList[k];
                }
                break;
            }
        }
        
        sTableColList[j] = *sCurIndexCol;
        sCurIndexCol = sCurIndexCol->mNext; 
    }
    
    sTableColList[sIndexHeader->mKeyColCount - 1].mNext = NULL;
    
    for( i = 0; i < sIndexHeader->mKeyColCount - 1; i++ )
    {
        sTableColList[i].mNext = &sTableColList[i + 1];
    }

    /**
     * Unmarshalling undo record
     */
    
    STL_READ_MOVE_INT8( &sBlockCount, aLogBody, sOffset );
    STL_READ_MOVE_INT64( &sStmtViewScn, aLogBody, sOffset );
    STL_READ_MOVE_INT32( &sStmtTcn, aLogBody, sOffset );

    for( i = 0; i < sBlockCount; i++ )
    {
        sRowRid[i].mTbsId = smsGetTbsId( SME_GET_SEGMENT_HANDLE(sBaseRelationHandle) );
        STL_READ_MOVE_INT32( &sRowRid[i].mPageId, aLogBody, sOffset );
        STL_READ_MOVE_INT16( &sRowRid[i].mOffset, aLogBody, sOffset );
    }

    /**
     * delete keys
     */
    
    if( SMN_GET_LOGGING_FLAG( sIndexRelationHandle ) == STL_TRUE )
    {
        sAttr = SMXM_ATTR_REDO;
    }
    else
    {
        sAttr = SMXM_ATTR_NOLOG_DIRTY;
    }
        
    for( i = sBlockCount - 1; i >= 0; i-- )
    {
        STL_TRY( smdExtractValidKeyValue( sBaseRelationHandle,
                                          smxmGetTransId( aMiniTrans ),
                                          sStmtViewScn,
                                          sStmtTcn + 1,
                                          &sRowRid[i],
                                          sTableColList,
                                          aEnv )
                 == STL_SUCCESS );

        STL_TRY( smxmBegin( &sMiniTrans,
                            smxmGetTransId( aMiniTrans ),
                            sSegRid,
                            sAttr,
                            aEnv )
                 == STL_SUCCESS);
        sState = 2;
        
        STL_TRY( smnmpbDeleteInternal( NULL, /* aStatement */
                                       &sMiniTrans,
                                       sIndexRelationHandle,
                                       sIndexColList,
                                       0,
                                       &sRowRid[i],
                                       &sUndoRid,
                                       STL_FALSE,
                                       &sIsUniqueResolved,
                                       &sEmptyPageCount,
                                       sEmptyPidList,
                                       aEnv )
                 == STL_SUCCESS );

        sState = 1;
        STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

        /* Multi block에 대한 undo시 logging양이 많아서 log switch를 발생시킬 수 있고
         * 그 때 archiving이 느리면 hang이 발생할 수 있다.
         * 따라서 block별로 mini-transaction을 commit후 새로 begin한다.
         */
        sOrgTransState = 0;
        STL_TRY( smxmSnapshotAndCommit( aMiniTrans,
                                        sPageStack,
                                        &sSnapshotMtx,
                                        aEnv ) == STL_SUCCESS );

        /* Delete 연산 후 empty가 된 node들을 empty node list에 연결한다
         * Empty node을 list에 연결할 때 Aging이 진행중이면 busy wait을 하게 되고
         * 이때 lock을 잡고 있으면 deadlock이 발생할 수 있기 때문에
         * delete연산 후 mini-transaction을 새로 begin해야한다. 
         */
        if( sEmptyPageCount > 0 )
        {
            STL_TRY( smxmBegin( &sMiniTrans,
                                smxmGetTransId( aMiniTrans ),
                                sSegRid,
                                SMXM_ATTR_NOLOG_DIRTY,
                                aEnv ) == STL_SUCCESS );
            sState = 2;

            STL_TRY( smnmpbRegisterEmptyNode( sIndexRelationHandle,
                                              &sMiniTrans,
                                              sEmptyPageCount,
                                              sEmptyPidList,
                                              STL_TRUE, /* aCheckAgingFlag */
                                              aEnv ) == STL_SUCCESS );
            sState = 1;
            STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
        }

        (*(stlInt8*)aLogBody)--;

        /* Undo를 위해 mini-transaction을 새로 begin하고
         * 이전에 잡혀있던 page latch를 다시 획득한다.
         */
        STL_TRY( smxmBeginWithSnapshot( aMiniTrans,
                                        &sSnapshotMtx,
                                        aEnv ) == STL_SUCCESS );
        sOrgTransState = 1;
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
        case 2:
            (void)smxmCommit( &sMiniTrans, aEnv );
        case 1:
            (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                            &sMemMark,
                                            STL_FALSE, /* aFreeChunk */
                                            (knlEnv*)aEnv );
        default:
            break;
    }

    if( sOrgTransState == 0 )
    {
        (void) smxmBeginWithSnapshot( aMiniTrans,
                                      &sSnapshotMtx,
                                      aEnv );
    }

    return STL_FAILURE;
}

stlStatus smnUndoMemoryBtreeBlockDelete( smxmTrans * aMiniTrans,
                                         smlRid      aTargetRid,
                                         void      * aLogBody,
                                         stlInt16    aLogSize,
                                         smlEnv    * aEnv )
{
    smlRid              sSegRid = smxmGetSegRid(aMiniTrans);
    void              * sIndexRelationHandle;
    void              * sBaseRelationHandle;
    knlRegionMark       sMemMark;
    stlInt32            sState = 0;
    stlInt32            sOrgTransState = 1;
    smlRid              sRowRid[SMN_MAX_BLOCK_KEY_COUNT];
    stlInt8             sBlockCount;
    smlScn              sStmtViewScn;
    smlTcn              sStmtTcn;
    stlInt32            i;
    stlInt32            j;
    stlInt32            k;
    knlValueBlockList * sIndexColList = NULL;
    knlValueBlockList * sCurIndexCol;
    knlValueBlockList * sPrevIndexCol = NULL;
    knlValueBlockList   sTableColList[SML_MAX_INDEX_KEY_COL_COUNT];
    stlChar           * sValue;
    smnIndexHeader    * sIndexHeader;
    stlInt32            sOffset = 0;
    smxmTrans           sMiniTrans;
    smxmTrans           sSnapshotMtx;
    smpCtrlHeader       sPageStack[KNL_ENV_MAX_LATCH_DEPTH];

    STL_TRY( smeGetRelationHandle( sSegRid,
                                   &sIndexRelationHandle,
                                   aEnv)
             == STL_SUCCESS );
    sIndexHeader = (smnIndexHeader*)sIndexRelationHandle;
    STL_TRY( smeGetRelationHandle( SMN_GET_BASE_TABLE_SEGMENT_RID( sIndexRelationHandle ),
                                   &sBaseRelationHandle,
                                   aEnv)
             == STL_SUCCESS );
    
    
    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;

    /**
     * Index column list 구성
     */
    
    sPrevIndexCol = NULL;
    for( i = 0; i < sIndexHeader->mKeyColCount; i++ )
    {
        STL_TRY( knlInitBlockNoBuffer( &sCurIndexCol,
                                       STL_LAYER_STORAGE_MANAGER,
                                       1,
                                       sIndexHeader->mKeyColTypes[i],
                                       &aEnv->mOperationHeapMem,
                                       (knlEnv*)aEnv ) == STL_SUCCESS );
        if( i == 0 )
        {
            sIndexColList = sCurIndexCol;
        }
        else
        {
            KNL_LINK_BLOCK_LIST( sPrevIndexCol, sCurIndexCol );
        }

        STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                    sIndexHeader->mKeyColSize[i],
                                    (void**)&sValue,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        KNL_SET_BLOCK_DATA_PTR( sCurIndexCol, 0, sValue, sIndexHeader->mKeyColSize[i] );
        KNL_SET_BLOCK_COLUMN_ORDER( sCurIndexCol, sIndexHeader->mKeyColOrder[i] );
        
        sPrevIndexCol = sCurIndexCol;
    }
    
    /**
     * Table column list 구성
     */
    
    sTableColList[0] = *sIndexColList;
    sCurIndexCol = sIndexColList->mNext;

    for( i = 1; i < sIndexHeader->mKeyColCount; i++ )
    {
        for( j = 0; j < i; j++ )
        {
            if( sTableColList[j].mColumnOrder > sCurIndexCol->mColumnOrder )
            {
                for( k = i - 1; k >= j; k--)
                {
                    sTableColList[k + 1] = sTableColList[k];
                }
                break;
            }
        }
        
        sTableColList[j] = *sCurIndexCol;
        sCurIndexCol = sCurIndexCol->mNext; 
    }
    
    sTableColList[sIndexHeader->mKeyColCount - 1].mNext = NULL;
    
    for( i = 0; i < sIndexHeader->mKeyColCount - 1; i++ )
    {
        sTableColList[i].mNext = &sTableColList[i + 1];
    }

    /**
     * Unmarshalling undo record
     */
    
    STL_READ_MOVE_INT8( &sBlockCount, aLogBody, sOffset );
    STL_READ_MOVE_INT64( &sStmtViewScn, aLogBody, sOffset );
    STL_READ_MOVE_INT32( &sStmtTcn, aLogBody, sOffset );

    for( i = 0; i < sBlockCount; i++ )
    {
        sRowRid[i].mTbsId = smsGetTbsId( SME_GET_SEGMENT_HANDLE(sBaseRelationHandle) );
        STL_READ_MOVE_INT32( &sRowRid[i].mPageId, aLogBody, sOffset );
        STL_READ_MOVE_INT16( &sRowRid[i].mOffset, aLogBody, sOffset );
    }

    /**
     * delete keys
     */
    
    for( i = sBlockCount - 1; i >= 0; i-- )
    {
        STL_TRY( smdExtractValidKeyValue( sBaseRelationHandle,
                                          smxmGetTransId( aMiniTrans ),
                                          sStmtViewScn,
                                          sStmtTcn,
                                          &sRowRid[i],
                                          sTableColList,
                                          aEnv )
                 == STL_SUCCESS );
        
        STL_TRY( smxmBegin( &sMiniTrans,
                            smxmGetTransId( aMiniTrans ),
                            sSegRid,
                            SMXM_ATTR_NOLOG_DIRTY,
                            aEnv )
                 == STL_SUCCESS);
        sState = 2;
        
        STL_TRY( smnmpbUndeleteInternal( NULL, /* aStatement */
                                         &sMiniTrans,
                                         sIndexRelationHandle,
                                         sIndexColList,
                                         &sRowRid[i],
                                         aEnv )
                 == STL_SUCCESS );
        
        (*(stlInt8*)aLogBody)--;
        
        sState = 1;
        STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

        /* Multi block에 대한 undo시 logging양이 많아서 log switch를 발생시킬 수 있고
         * 그 때 archiving이 느리면 hang이 발생할 수 있다.
         * 따라서 block별로 mini-transaction을 commit후 새로 begin한다.
         */
        sOrgTransState = 0;
        STL_TRY( smxmSnapshotAndCommit( aMiniTrans,
                                        sPageStack,
                                        &sSnapshotMtx,
                                        aEnv ) == STL_SUCCESS );

        /* Undo를 위해 mini-transaction을 새로 begin하고
         * 이전에 잡혀있던 page latch를 다시 획득한다.
         */
        STL_TRY( smxmBeginWithSnapshot( aMiniTrans,
                                        &sSnapshotMtx,
                                        aEnv ) == STL_SUCCESS );
        sOrgTransState = 1;
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
        case 2:
            (void)smxmCommit( &sMiniTrans, aEnv );
        case 1:
            (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                            &sMemMark,
                                            STL_FALSE, /* aFreeChunk */
                                            (knlEnv*)aEnv );
        default:
            break;
    }

    if( sOrgTransState == 0 )
    {
        (void) smxmBeginWithSnapshot( aMiniTrans,
                                      &sSnapshotMtx,
                                      aEnv );
    }

    return STL_FAILURE;
}

stlStatus smnUndoMemoryBtreeAlterDatatype( smxmTrans * aMiniTrans,
                                           smlRid      aTargetRid,
                                           void      * aLogBody,
                                           stlInt16    aLogSize,
                                           smlEnv    * aEnv )
{
    smpHandle        sPageHandle;
    void           * sSegmentHandle;
    void           * sRelationHandle;
    smnIndexHeader * sIndexHeader;
    smnIndexHeader * sCache;
    stlUInt8         sDataType;
    stlInt32         sColumnOrder;
    stlInt64         sColumnSize;
    stlInt32         sOffset = 0;

    STL_READ_MOVE_INT32( &sColumnOrder, aLogBody, sOffset );
    STL_READ_MOVE_INT8( &sDataType, aLogBody, sOffset );
    STL_READ_MOVE_INT64( &sColumnSize, aLogBody, sOffset );
    
    STL_TRY( smeGetRelationHandle( aTargetRid,
                                   &sRelationHandle,
                                   aEnv ) == STL_SUCCESS);
    sSegmentHandle = SME_GET_SEGMENT_HANDLE( sRelationHandle );
    sCache = (smnIndexHeader*)sRelationHandle;
    
    STL_TRY( smpAcquire( aMiniTrans,
                         aTargetRid.mTbsId,
                         aTargetRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

    sIndexHeader = (smnIndexHeader*)( smpGetBody(SMP_FRAME(&sPageHandle)) +
                                      smsGetHeaderSize( sSegmentHandle) );

    sCache->mKeyColTypes[sColumnOrder] = sDataType;
    sCache->mKeyColSize[sColumnOrder] = sColumnSize;

    sIndexHeader->mKeyColTypes[sColumnOrder] = sDataType;
    sIndexHeader->mKeyColSize[sColumnOrder] = sColumnSize;

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_INDEX,
                           SMR_LOG_ALTER_DATATYPE,
                           SMR_REDO_TARGET_PAGE,
                           (void*)aLogBody,
                           aLogSize,
                           aTargetRid.mTbsId,
                           aTargetRid.mPageId,
                           (stlChar*)sIndexHeader - (stlChar*)SMP_FRAME(&sPageHandle),
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smnUndoMemoryBtreeAlterAttr( smxmTrans * aMiniTrans,
                                       smlRid      aTargetRid,
                                       void      * aLogBody,
                                       stlInt16    aLogSize,
                                       smlEnv    * aEnv )
{
    smpHandle             sPageHandle;
    smnIndexHeader      * sIndexHeader;
    smlIndexAttr          sIndexAttr;
    void                * sRelationHandle;
    void                * sSegmentHandle;
    smnIndexHeader      * sCache;
    smlRid                sSegmentRid;

    stlMemcpy( &sIndexAttr, aLogBody, STL_SIZEOF(smlIndexAttr) );
    
    STL_TRY( smeGetRelationHandle( aTargetRid,
                                   &sRelationHandle,
                                   aEnv ) == STL_SUCCESS);
    
    sCache = (smnIndexHeader*)sRelationHandle;
    sSegmentHandle = SME_GET_SEGMENT_HANDLE( sRelationHandle );
    sSegmentRid = smsGetSegRid( sSegmentHandle );
    
    STL_TRY( smpAcquire( aMiniTrans,
                         aTargetRid.mTbsId,
                         aTargetRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

    sIndexHeader = (smnIndexHeader*)( smpGetBody(SMP_FRAME(&sPageHandle)) +
                                      smsGetHeaderSize( sSegmentHandle ) );

    /**
     * applys table attribute to relation header
     */

    sIndexHeader->mIniTrans = sIndexAttr.mIniTrans;
    sIndexHeader->mMaxTrans = sIndexAttr.mMaxTrans;
    sIndexHeader->mPctFree = sIndexAttr.mPctFree;
    
    /**
     * applys table attribute to relation cache
     */

    sCache->mIniTrans = sIndexHeader->mIniTrans;
    sCache->mMaxTrans = sIndexHeader->mMaxTrans;
    sCache->mPctFree = sIndexHeader->mPctFree;
    
    /**
     * logging redo log
     */
    
    STL_TRY( smxmWriteLog( aMiniTrans,
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
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
