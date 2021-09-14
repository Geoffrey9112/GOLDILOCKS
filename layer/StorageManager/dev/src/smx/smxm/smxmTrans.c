/*******************************************************************************
 * smxmTrans.c
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
#include <smrDef.h>
#include <smr.h>
#include <smp.h>
#include <smt.h>
#include <smxl.h>
#include <smf.h>
#include <smxmDef.h>
#include <smxlDef.h>
#include <smxm.h>
#include <smxmBlock.h>
#include <smt.h>
#include <smlSuppLogDef.h>

/**
 * @file smxmTrans.c
 * @brief Storage Manager Layer Mini-Transaction Internal Routines
 */

extern stlInt64 gDataStoreMode;

/**
 * @addtogroup smxm
 * @{
 */

/**
 * @brief Mini-transaction을 시작한다.
 * @param[out] aMiniTrans Mini-Transaction 포인터
 * @param[in] aTransId 대상 Transaction Identifier
 * @param[in] aSegRid 관련 Segment Rid
 * @param[in] aAttr Mini-transaction Attribute
 * @param[in,out] aEnv Environment 정보
 * @see @a aAttr : smxmAttr
 */
stlStatus smxmBegin( smxmTrans   * aMiniTrans,
                     smxlTransId   aTransId,
                     smlRid        aSegRid,
                     stlInt32      aAttr,
                     smlEnv      * aEnv )
{
    stlUInt32 sAttr;
    
    aMiniTrans->mTransId = aTransId;
    aMiniTrans->mSegRid = aSegRid;
    aMiniTrans->mRedoLogPieceCount = 0;
    aMiniTrans->mUndoLogPieceCount = 0;
    aMiniTrans->mRedoLogSize = 0;
    aMiniTrans->mPageStackTop = -1;
    aMiniTrans->mTryRedoLogPieceCount = 0;
    aMiniTrans->mMiniTransStackIdx = -1;
    aMiniTrans->mLogging = STL_TRUE;
    
    sAttr = aAttr;
    if( (aTransId == SML_INVALID_TRANSID)           ||
        (gDataStoreMode <= SML_DATA_STORE_MODE_CDS) ||
        (smfGetDataAccessMode() == SML_DATA_ACCESS_MODE_READ_ONLY) )
    {
        aMiniTrans->mLogging = STL_FALSE;
        
        if( aAttr & SMXM_ATTR_REDO )
        {
            sAttr &= ~SMXM_ATTR_REDO;
        }
    }
    
    aMiniTrans->mAttr = sAttr;
    
    KNL_INIT_REGION_MARK( &aMiniTrans->mMemMark );
    SMXM_INIT_LOG_BLOCK( &aMiniTrans->mRedoLogBlock );

    /**
     * Logging은 이전에 Exclusive Latch를 획득한 Mini Transaction이 없는
     * 경우에만 가능하다.
     * 즉, Exclusive Latch를 획득한 상태에서 Logger에 대기해서는 안된다.
     */
    if( (aMiniTrans->mLogging == STL_TRUE) &&
        (aEnv->mExclMiniTransStack[0] == NULL) )
    {
        STL_TRY( smrWaitEnableLogging( aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Mini-transaction을 Commit한다.
 * @param[in] aMiniTrans Mini-Transaction 포인터
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smxmCommit( smxmTrans * aMiniTrans,
                      smlEnv    * aEnv )
{
    STL_TRY( smxmCommitWithInfo( aMiniTrans,
                                 NULL,
                                 NULL,
                                 aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Mini-transaction을 Commit하고 Log가 Disk로 내려가기를 기다린다.
 * @param[in] aMiniTrans Mini-Transaction 포인터
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smxmSyncCommit( smxmTrans * aMiniTrans,
                          smlEnv    * aEnv )
{
    smrLsn  sWrittenLsn = SMR_INVALID_LSN;
    smrSbsn sWrittenSbsn;
    
    STL_TRY( smxmCommitWithInfo( aMiniTrans,
                                 &sWrittenSbsn,
                                 &sWrittenLsn,
                                 aEnv ) == STL_SUCCESS );

    STL_TRY( smrFlushLog( sWrittenLsn,
                          sWrittenSbsn,
                          STL_TRUE, /* aSwitchBlock */
                          SML_TRANSACTION_CWM_WAIT,
                          aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Mini-transaction을 Commit한다.
 * @param[in] aMiniTrans Mini-Transaction 포인터
 * @param[out] aSbsn 해당 로그가 기록된 Stream Buffer상의 위치 정보
 * @param[out] aLsn 기록된 Log Sequence Number
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smxmCommitWithInfo( smxmTrans * aMiniTrans,
                              smrSbsn   * aSbsn,
                              smrLsn    * aLsn,
                              smlEnv    * aEnv )
{
    smrLsn      sLsn = SMR_INVALID_LSN;
    smpHandle * sPageHandle;
    stlInt32    i;
    smrSbsn     sSbsn = SMR_INVALID_SBSN;

    STL_TRY_THROW( aMiniTrans != NULL, RAMP_SUCCESS );

    if( aMiniTrans->mLogging == STL_TRUE )
    {
        if( aMiniTrans->mRedoLogPieceCount > 0 )
        {
            STL_TRY( smrWriteLogWithBlock( aMiniTrans->mTransId,
                                           (void*)&aMiniTrans->mRedoLogBlock,
                                           aMiniTrans->mRedoLogSize,
                                           aMiniTrans->mRedoLogPieceCount,
                                           aMiniTrans->mSegRid,
                                           &sSbsn,
                                           &sLsn,
                                           aEnv ) == STL_SUCCESS );

            smxlAddTryLogCount( aMiniTrans->mTransId,
                                aMiniTrans->mTryRedoLogPieceCount );
        }
    }
    else
    {
        /**
         * No Logging Transactional Operation을 수행했다면 ( = CDS 모드라면 )
         * Transaction Record의 Commit Scn 갱신을 위해서 로깅 관련 정보를
         * Transaction에 설정한다.
         */
        if( aMiniTrans->mTransId != SML_INVALID_TRANSID )
        {
            smxlAddTryLogCount( aMiniTrans->mTransId,
                                aMiniTrans->mTryRedoLogPieceCount );
        }
    }

    for( i = 0; i <= aMiniTrans->mPageStackTop; i++ )
    {
        sPageHandle = &aMiniTrans->mPageStack[i];

        if( smpCurLatchMode( sPageHandle ) == KNL_LATCH_MODE_EXCLUSIVE )
        {
            if( aMiniTrans->mRedoLogPieceCount > 0 )
            {
                smpSetRecoveryLsn( sPageHandle, sSbsn, sLsn, SMR_MAX_LPSN );
            }
            if( (aMiniTrans->mTryRedoLogPieceCount > 0) ||
                (aMiniTrans->mAttr & SMXM_ATTR_NOLOG_DIRTY) )
            {
                STL_TRY( smpSetDirty( sPageHandle, aEnv ) == STL_SUCCESS );
            }
        }
    }
    
    for( i = aMiniTrans->mPageStackTop; i >= 0; i-- )
    {
        sPageHandle = &aMiniTrans->mPageStack[i];

        /**
         * Page Validation을 허용한다면 페이지를 Validation한다.
         */
        if( (aMiniTrans->mAttr & SMXM_ATTR_NO_VALIDATE_PAGE) !=
            SMXM_ATTR_NO_VALIDATE_PAGE )
        {
            STL_TRY( smpValidatePage( sPageHandle,
                                      aEnv )
                     == STL_SUCCESS );
        }
        
        STL_TRY( smpRelease( sPageHandle, aEnv ) == STL_SUCCESS );
    }
    
    if( KNL_IS_VALID_REGION_MARK( &aMiniTrans->mMemMark ) == STL_TRUE )
    {
        STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mLogHeapMem,
                                           &aMiniTrans->mMemMark,
                                           STL_FALSE, /* aFreeChunk */
                                           (knlEnv*)aEnv )
                 == STL_SUCCESS );
    }

    if( aMiniTrans->mMiniTransStackIdx >= 0)
    {
        /**
         * Exclusive Mini-Transaction Stack의 마지막 Mini-Transaction은 자기 자신이여야
         * 한다. 즉, 서로 다른 2개의 Mini-Transaction은 Cross해서 사용할수 없다.
         * 
         * (1) 잘못된 사용된 예
         *  |-----------------------------------------| (MT1)
         *                  |-------------------------------------| (MT2)
         *
         * (2) 올바르게 사용된 예
         *  |-----------------------------------------| (MT1)
         *         |-------------------------| (MT2)
         */
        STL_ASSERT( aEnv->mMiniTransStackTop == aMiniTrans->mMiniTransStackIdx );
        STL_ASSERT( aEnv->mExclMiniTransStack[aEnv->mMiniTransStackTop] == aMiniTrans );
        
        /**
         * Mini Transaction Stack에서 자신을 Pop한다.
         */
        aEnv->mExclMiniTransStack[aEnv->mMiniTransStackTop] = NULL;
        aEnv->mMiniTransStackTop--;
    }
    
    STL_RAMP( RAMP_SUCCESS );
    
    if( aLsn != NULL )
    {
        *aLsn = sLsn;
    }

    if( aSbsn != NULL )
    {
        *aSbsn = sSbsn;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Mini-transaction Snapshot을 만들고 Commit한다.
 * @param[in] aMiniTrans Mini-Transaction 포인터
 * @param[in] aPageStack Mini-Transaction의 page stack을 복사하기 위한 stack
 * @param[out] aSnapshotMtx Mini-Transaction을 commit하기 전 snapshot
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smxmSnapshotAndCommit( smxmTrans      * aMiniTrans,
                                 smpCtrlHeader  * aPageStack,
                                 smxmTrans      * aSnapshotMtx,
                                 smlEnv         * aEnv )
{
    stlInt32    i;

    stlMemset( aSnapshotMtx, 0x00, STL_SIZEOF(smxmTrans) );

    for( i = 0; i < KNL_ENV_MAX_LATCH_DEPTH; i++ )
    {
        aSnapshotMtx->mPageStack[i].mPch = &aPageStack[i];
    }

    aSnapshotMtx->mTransId      = aMiniTrans->mTransId;
    aSnapshotMtx->mAttr         = aMiniTrans->mAttr;
    aSnapshotMtx->mSegRid       = aMiniTrans->mSegRid;
    aSnapshotMtx->mPageStackTop = aMiniTrans->mPageStackTop;

    for( i = 0; i <= aMiniTrans->mPageStackTop; i++ )
    {
        stlMemcpy( aSnapshotMtx->mPageStack[i].mPch,
                   aMiniTrans->mPageStack[i].mPch,
                   STL_SIZEOF(smpCtrlHeader) );
    }

    STL_TRY( smxmCommit( aMiniTrans, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Commit하기 전 Snapshot을 이용하여 Mini-transaction을 Begin한다.
 * @param[out] aMiniTrans Mini-Transaction 포인터
 * @param[in] aSnapshotMtx Mini-Transaction을 commit하기 전 떠놓은 snapshot
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smxmBeginWithSnapshot( smxmTrans      * aMiniTrans,
                                 smxmTrans      * aSnapshotMtx,
                                 smlEnv         * aEnv )
{
    smpHandle   sPageHandle;
    stlInt32    i;

    STL_TRY( smxmBegin( aMiniTrans,
                        smxmGetTransId( aSnapshotMtx ),
                        smxmGetSegRid( aSnapshotMtx ),
                        smxmGetAttr( aSnapshotMtx ),
                        aEnv ) == STL_SUCCESS );

    for( i = 0; i <= aSnapshotMtx->mPageStackTop; i++ )
    {
        STL_TRY( smpAcquire( aMiniTrans,
                             smpGetTbsId( &aSnapshotMtx->mPageStack[i] ),
                             smpGetPageId( &aSnapshotMtx->mPageStack[i] ),
                             knlCurLatchMode( &aSnapshotMtx->mPageStack[i].mPch->mPageLatch ),
                             &sPageHandle,
                             aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Mini-transaction을 Rollback한다.
 * @param[in] aMiniTrans Mini-Transaction 포인터
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smxmRollback( smxmTrans * aMiniTrans,
                        smlEnv    * aEnv )
{
    stlInt32       i;
    smrLogPieceHdr sLogPieceHdr;

    STL_TRY_THROW( aMiniTrans != NULL, RAMP_SUCCESS );
    
    for( i = aMiniTrans->mUndoLogPieceCount - 1; i >= 0; i-- )
    {
        SMR_READ_LOGPIECE_HDR( &sLogPieceHdr,
                               aMiniTrans->mUndoLogPieceAnchor[i] );

        STL_TRY( smrMtxUndo( &sLogPieceHdr,
                             aMiniTrans->mUndoLogPieceAnchor[i] + SMR_LOGPIECE_HDR_SIZE,
                             aEnv )
                 == STL_SUCCESS );
        
    }
    
    for( i = aMiniTrans->mPageStackTop; i >= 0; i-- )
    {
        STL_TRY( smpValidatePage( &aMiniTrans->mPageStack[i],
                                  aEnv )
                 == STL_SUCCESS );
        
        STL_TRY( smpRelease( &aMiniTrans->mPageStack[i], aEnv ) == STL_SUCCESS );
    }
    
    if( KNL_IS_VALID_REGION_MARK( &aMiniTrans->mMemMark ) == STL_TRUE )
    {
        STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mLogHeapMem,
                                           &aMiniTrans->mMemMark,
                                           STL_FALSE, /* aFreeChunk */
                                           (knlEnv*)aEnv )
                 == STL_SUCCESS );
    }
    
    if( aMiniTrans->mMiniTransStackIdx >= 0 )
    {
        STL_ASSERT( aMiniTrans->mMiniTransStackIdx == aEnv->mMiniTransStackTop );
        
        /**
         * Mini Transaction Stack에서 자신을 Pop한다.
         */
        aEnv->mExclMiniTransStack[aEnv->mMiniTransStackTop] = NULL;
        aEnv->mMiniTransStackTop--;
    }
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Mini-transaction에 Savepoint를 설정한다.
 * @param[in] aMiniTrans Mini-Transaction 포인터
 * @param[out] aSavepoint 설정된 Savepoint 포인터
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smxmMarkSavepoint( smxmTrans     * aMiniTrans,
                             smxmSavepoint * aSavepoint,
                             smlEnv        * aEnv )
{
    smxmLogBlock * sLogBlock;

    STL_TRY_THROW( aMiniTrans != NULL, RAMP_SUCCESS );
    
    aSavepoint->mPageStackTop = aMiniTrans->mPageStackTop;
    aSavepoint->mRedoLogSize = aMiniTrans->mRedoLogSize;
    
    sLogBlock = &aMiniTrans->mRedoLogBlock;
    aSavepoint->mRedoLogBlockCount = sLogBlock->mBlockCount;

    if( sLogBlock->mBlockCount == 0 )
    {
        aSavepoint->mRedoLogBlockSize = 0;
    }
    else
    {
        aSavepoint->mRedoLogBlockSize = sLogBlock->mLogBlockSlotArray[sLogBlock->mBlockCount-1].mSize;
    }
    
    aSavepoint->mRedoLogPieceCount = aMiniTrans->mRedoLogPieceCount;
    aSavepoint->mTryRedoLogPieceCount = aMiniTrans->mTryRedoLogPieceCount;
    aSavepoint->mUndoLogPieceCount = aMiniTrans->mUndoLogPieceCount;

    KNL_INIT_REGION_MARK( &aSavepoint->mMemMark );
    
    if( KNL_IS_VALID_REGION_MARK( &aMiniTrans->mMemMark ) == STL_TRUE )
    {
        STL_TRY( knlMarkRegionMem( &aEnv->mLogHeapMem,
                                   &aSavepoint->mMemMark,
                                   (knlEnv*)aEnv ) == STL_SUCCESS );
    }
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Savepoint까지 Mini-transaction에 Rollback한다.
 * @param[in] aMiniTrans Mini-Transaction 포인터
 * @param[in] aSavepoint Savepoint 포인터
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smxmRollbackToSavepoint( smxmTrans     * aMiniTrans,
                                   smxmSavepoint * aSavepoint,
                                   smlEnv        * aEnv )
{
    smrLogPieceHdr   sLogPieceHdr;
    smxmLogBlock   * sLogBlock;
    stlInt32         i;

    STL_TRY_THROW( aMiniTrans != NULL, RAMP_SUCCESS );
    
    for( i = aMiniTrans->mUndoLogPieceCount - 1;
         i > aSavepoint->mUndoLogPieceCount - 1;
         i-- )
    {
        SMR_READ_LOGPIECE_HDR( &sLogPieceHdr,
                               aMiniTrans->mUndoLogPieceAnchor[i] );

        STL_TRY( smrMtxUndo( &sLogPieceHdr,
                             aMiniTrans->mUndoLogPieceAnchor[i] + SMR_LOGPIECE_HDR_SIZE,
                             aEnv )
                 == STL_SUCCESS );
    }
    
    for( i = aMiniTrans->mPageStackTop; i >= aSavepoint->mPageStackTop + 1; i-- )
    {
        STL_TRY( smpRelease( &aMiniTrans->mPageStack[i], aEnv ) == STL_SUCCESS );
    }
    
    aMiniTrans->mPageStackTop = aSavepoint->mPageStackTop;
    aMiniTrans->mRedoLogSize = aSavepoint->mRedoLogSize;
    
    sLogBlock = &aMiniTrans->mRedoLogBlock;
    sLogBlock->mBlockCount = aSavepoint->mRedoLogBlockCount;

    if( sLogBlock->mBlockCount != 0 )
    {
        sLogBlock->mLogBlockSlotArray[sLogBlock->mBlockCount-1].mSize = aSavepoint->mRedoLogBlockSize;
    }
    
    aMiniTrans->mRedoLogPieceCount = aSavepoint->mRedoLogPieceCount;
    aMiniTrans->mTryRedoLogPieceCount = aSavepoint->mTryRedoLogPieceCount;
    aMiniTrans->mUndoLogPieceCount = aSavepoint->mUndoLogPieceCount;

    if( KNL_IS_VALID_REGION_MARK( &aSavepoint->mMemMark ) == STL_TRUE )
    {
        STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mLogHeapMem,
                                           &aSavepoint->mMemMark,
                                           STL_FALSE, /* aFreeChunk */
                                           (knlEnv*)aEnv )
                 == STL_SUCCESS );
    }
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Mini-transaction에 로그를 기록한다.
 * @param[in] aMiniTrans Mini-Transaction 포인터
 * @param[in] aComponentClass Component Class
 * @param[in] aLogType Log Type
 * @param[in] aRedoTargetType Redo Target Type
 * @param[in] aLogBody Log Body
 * @param[in] aLogSize 기록할 로그의 총 바이트수
 * @param[in] aTbsId Tablespace Identifier
 * @param[in] aPageId 관련 Page Identifier
 * @param[in] aOffset Page내 Offset
 * @param[in] aLoggingFlag Logging Flag
 * @param[in,out] aEnv Environment 정보
 * @see @a aComponentClass : smgComponentClass
 * @see @a aLogType : smrLogType
 * @see @a aRedoTargetType : smrRedoTargetType
 * @see @a aLoggingFlag : smxmLogging
 */
stlStatus smxmWriteLog( smxmTrans   * aMiniTrans,
                        stlChar       aComponentClass,
                        stlUInt16     aLogType,
                        stlChar       aRedoTargetType,
                        stlChar     * aLogBody,
                        stlUInt16     aLogSize,
                        smlTbsId      aTbsId,
                        smlPid        aPageId,
                        stlUInt16     aOffset,
                        stlUInt32     aLoggingFlag,
                        smlEnv      * aEnv )
{
    smrLogPieceHdr     sLogPieceHdr;
    void             * sLogAnchor;
    stlInt16           sBlockIdx;
    smxmLogBlockSlot * sLogBlockSlot;
    smxmLogBlock     * sLogBlock;
    void             * sSlotBufPtr;
    stlUInt16          sSlotSize;
    smlRid             sDataRid = { aTbsId, aOffset, aPageId };

    STL_TRY_THROW( aMiniTrans != NULL, RAMP_SUCCESS );
    STL_TRY_THROW( aMiniTrans->mLogging == STL_TRUE, RAMP_SKIP_REDO );

    /**
     * Component class가 relation 이하인 경우는 attribute에
     * 상관없이 로그를 기록한다.
     * - Undo Relation은 SMXM_ATTR_NOLOG_DIRTY라 해도 로그를 기록해야 한다.
     */
    if( aComponentClass >= SMG_COMPONENT_RELATION )
    {
        STL_TRY_THROW( aMiniTrans->mAttr & SMXM_ATTR_REDO, RAMP_SKIP_REDO );
    }
    
#ifdef STL_VALIDATE_DATABASE
    stlUInt16        i;
    smpHandle        sPageHandle;

    for( i = 0; i <= aMiniTrans->mPageStackTop; i++ )
    {
        sPageHandle = aMiniTrans->mPageStack[i];
        if( (smpGetTbsId(&sPageHandle) == aTbsId) &&
            (smpGetPageId(&sPageHandle) == aPageId) )
        {
            break;
        }
    }

    /**
     * 페이지와 관련 없는 로그를 검사하지 않는다.
     */
    if( (aLogType != SML_SUPPL_LOG_DDL_CLR) &&
        (aLogType != SML_SUPPL_LOG_UPDATE_BEFORE_COLS) )
    {
        if( (i > aMiniTrans->mPageStackTop) ||
            (smpCurLatchMode(&sPageHandle) != KNL_LATCH_MODE_EXCLUSIVE) )
        {
            for( i = 0; i <= aMiniTrans->mPageStackTop; i++ )
            {
                sPageHandle = aMiniTrans->mPageStack[i];
                
                (void) knlLogMsg( NULL,  /* aLogger */
                                  KNL_ENV(aEnv),
                                  KNL_LOG_LEVEL_ABORT,
                                  "PAGE ID : %d\nPCH : \n%s\n",
                                  smpGetPageId( &sPageHandle ),
                                  knlDumpBinaryForAssert( (stlChar*)sPageHandle.mPch,
                                                          STL_SIZEOF(smpCtrlHeader),
                                                          &aEnv->mLogHeapMem,
                                                          KNL_ENV(aEnv) ) );
                (void) knlLogMsg( NULL,  /* aLogger */
                                  KNL_ENV(aEnv),
                                  KNL_LOG_LEVEL_ABORT,
                                  "PAGE ID : %d\nPAGE : \n%s\n",
                                  smpGetPageId( &sPageHandle ),
                                  knlDumpBinaryForAssert( SMP_FRAME(&sPageHandle),
                                                          SMP_PAGE_SIZE,
                                                          &aEnv->mLogHeapMem,
                                                          KNL_ENV(aEnv) ) );

                /**
                 * 디버깅 코드
                 */
                stlSleep( STL_INT64_C(1000000) * 60 * 60 * 48 );
            }
        }
        
        KNL_ASSERT( (i <= aMiniTrans->mPageStackTop) &&
                    (smpCurLatchMode(&sPageHandle) == KNL_LATCH_MODE_EXCLUSIVE),
                    KNL_ENV(aEnv),
                    ("Writing log without acquiring any latch.\nRID(%d,%d,%d). Componet(%d) LogType(%d)",
                     aTbsId, aOffset, aPageId, aComponentClass, aLogType) );
    }
#endif

    STL_TRY_THROW( smtIsLogging(aTbsId) == STL_TRUE, RAMP_SUCCESS );

    /**
     * 로그 기록후 Log Block의 개수가 정해진 크기를 넘어설수 없다.
     */
    STL_ASSERT( (aMiniTrans->mRedoLogBlock.mBlockCount >= 0) &&
                (aMiniTrans->mRedoLogBlock.mBlockCount + 1) <
                SMXM_MAX_LOGBLOCK_COUNT );
    STL_ASSERT( aMiniTrans->mRedoLogPieceCount < (STL_UINT16_MAX - 1) );
    
    sLogPieceHdr.mType = aLogType;
    sLogPieceHdr.mComponentClass = aComponentClass;
    sLogPieceHdr.mSize = aLogSize;
    sLogPieceHdr.mRedoTargetType = aRedoTargetType;
    SMG_WRITE_RID( &sLogPieceHdr.mDataRid, &sDataRid );
    sLogPieceHdr.mPropagateLog = SMXL_TRANS_PROPAGATE_LOG( aMiniTrans->mTransId );
    sLogPieceHdr.mRepreparable = STL_TRUE;

    if( KNL_IS_VALID_REGION_MARK( &aMiniTrans->mMemMark ) == STL_FALSE )
    {
        STL_TRY( knlMarkRegionMem( &aEnv->mLogHeapMem,
                                   &aMiniTrans->mMemMark,
                                   (knlEnv*)aEnv ) == STL_SUCCESS );
    }

    STL_ASSERT( (aMiniTrans->mRedoLogSize + (SMR_LOGPIECE_HDR_SIZE + aLogSize)) <
                SMXM_MAX_LOGBLOCK_SIZE );

    /* redo log block을 기록한다. */
    sLogBlock = &(aMiniTrans->mRedoLogBlock);
    sBlockIdx = sLogBlock->mBlockCount - 1;
    
    if( sBlockIdx == -1 )
    {
        sBlockIdx++;
        sLogBlockSlot = &sLogBlock->mLogBlockSlotArray[sBlockIdx];
        
        STL_TRY( knlAllocRegionMem( &aEnv->mLogHeapMem,
                                    SMXM_MAX_BLOCK_SIZE,
                                    &sLogBlockSlot->mBuffer,
                                    (knlEnv*)aEnv ) == STL_SUCCESS );
        sLogBlockSlot->mSize = 0;
    }
    else
    {
        sLogBlockSlot = &sLogBlock->mLogBlockSlotArray[sBlockIdx];
    }

    if( (sLogBlockSlot->mSize + (SMR_LOGPIECE_HDR_SIZE + aLogSize)) >
        SMXM_MAX_BLOCK_SIZE )
    {
        sBlockIdx++;
        sLogBlockSlot = &sLogBlock->mLogBlockSlotArray[sBlockIdx];

        STL_TRY( knlAllocRegionMem( &aEnv->mLogHeapMem,
                                    SMXM_MAX_BLOCK_SIZE,
                                    &sLogBlockSlot->mBuffer,
                                    (knlEnv*)aEnv ) == STL_SUCCESS );
        sLogBlockSlot->mSize = 0;
    }

    sLogAnchor = sLogBlockSlot->mBuffer + sLogBlockSlot->mSize;
    
    sSlotBufPtr = sLogAnchor;
    sSlotSize = sLogBlockSlot->mSize;
    SMR_WRITE_LOGPIECE_HDR( sSlotBufPtr,
                            &sLogPieceHdr );
    sSlotSize += SMR_LOGPIECE_HDR_SIZE;

    if( aLogSize > 0 )
    {
        sSlotBufPtr += SMR_LOGPIECE_HDR_SIZE;
        STL_WRITE_BYTES( sSlotBufPtr,
                         aLogBody,
                         aLogSize );
        sSlotSize += aLogSize;
    }
    sLogBlockSlot->mSize = sSlotSize;

    STL_DASSERT( sLogBlockSlot->mSize <= SMXM_MAX_BLOCK_SIZE );

    sLogBlock->mBlockCount = sBlockIdx + 1;

    STL_ASSERT( (sLogBlock->mBlockCount > 0) &&
                (sLogBlock->mBlockCount < SMXM_MAX_LOGBLOCK_COUNT) );

    aMiniTrans->mRedoLogSize += (SMR_LOGPIECE_HDR_SIZE + aLogSize);
    aMiniTrans->mRedoLogPieceCount++;

    if( aLoggingFlag == SMXM_LOGGING_REDO_UNDO )
    {
        STL_ASSERT( aMiniTrans->mUndoLogPieceCount < SMXM_MAX_UNDO_COUNT );
        aMiniTrans->mUndoLogPieceAnchor[aMiniTrans->mUndoLogPieceCount] = sLogAnchor;
        aMiniTrans->mUndoLogPieceCount++;
    }

    STL_RAMP( RAMP_SKIP_REDO );

    aMiniTrans->mTryRedoLogPieceCount++;
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Mini-transaction에 TBS 로그를 기록한다.
 * @param[in] aMiniTrans Mini-Transaction 포인터
 * @param[in] aComponentClass Component Class
 * @param[in] aLogType Log Type
 * @param[in] aRedoTargetType Redo Target Type
 * @param[in] aLogBody Log Body
 * @param[in] aLogSize 기록할 로그의 총 바이트수
 * @param[in] aTbsId Tablespace Identifier
 * @param[in] aPageId 관련 Page Identifier
 * @param[in] aOffset Page내 Offset
 * @param[in,out] aEnv Environment 정보
 * @see @a aComponentClass : smgComponentClass
 * @see @a aLogType : smrLogType
 * @see @a aRedoTargetType : smrRedoTargetType
 */
stlStatus smxmWriteTbsLog( smxmTrans   * aMiniTrans,
                           stlChar       aComponentClass,
                           stlUInt16     aLogType,
                           stlChar       aRedoTargetType,
                           stlChar     * aLogBody,
                           stlUInt16     aLogSize,
                           smlTbsId      aTbsId,
                           smlPid        aPageId,
                           stlUInt16     aOffset,
                           smlEnv      * aEnv )
{
    smrLogPieceHdr     sLogPieceHdr;
    void             * sLogAnchor;
    stlInt16           sBlockIdx;
    smxmLogBlockSlot * sLogBlockSlot;
    smxmLogBlock     * sLogBlock;
    void             * sSlotBufPtr;
    stlUInt16          sSlotSize;
    smlRid             sDataRid = { aTbsId, aOffset, aPageId };

    STL_TRY_THROW( aMiniTrans != NULL, RAMP_SUCCESS );
    STL_TRY_THROW( aMiniTrans->mLogging == STL_TRUE, RAMP_SKIP_REDO );

    /**
     * 로그 기록후 Log Block의 개수가 정해진 크기를 넘어설수 없다.
     */
    STL_ASSERT( (aMiniTrans->mRedoLogBlock.mBlockCount >= 0) &&
                (aMiniTrans->mRedoLogBlock.mBlockCount + 1) <
                SMXM_MAX_LOGBLOCK_COUNT );
    STL_ASSERT( aMiniTrans->mRedoLogPieceCount < (STL_UINT16_MAX - 1) );
    
    sLogPieceHdr.mType = aLogType;
    sLogPieceHdr.mComponentClass = aComponentClass;
    sLogPieceHdr.mSize = aLogSize;
    sLogPieceHdr.mRedoTargetType = aRedoTargetType;
    SMG_WRITE_RID( &sLogPieceHdr.mDataRid, &sDataRid );
    sLogPieceHdr.mPropagateLog = SMXL_TRANS_PROPAGATE_LOG( aMiniTrans->mTransId );
    sLogPieceHdr.mRepreparable = STL_TRUE;

    if( KNL_IS_VALID_REGION_MARK( &aMiniTrans->mMemMark ) == STL_FALSE )
    {
        STL_TRY( knlMarkRegionMem( &aEnv->mLogHeapMem,
                                   &aMiniTrans->mMemMark,
                                   (knlEnv*)aEnv ) == STL_SUCCESS );
    }

    STL_ASSERT( (aMiniTrans->mRedoLogSize + (SMR_LOGPIECE_HDR_SIZE + aLogSize)) <
                SMXM_MAX_LOGBLOCK_SIZE );

    /* redo log block을 기록한다. */
    sLogBlock = &(aMiniTrans->mRedoLogBlock);
    sBlockIdx = sLogBlock->mBlockCount - 1;
    
    if( sBlockIdx == -1 )
    {
        sBlockIdx++;
        sLogBlockSlot = &sLogBlock->mLogBlockSlotArray[sBlockIdx];
        
        STL_TRY( knlAllocRegionMem( &aEnv->mLogHeapMem,
                                    SMXM_MAX_BLOCK_SIZE,
                                    &sLogBlockSlot->mBuffer,
                                    (knlEnv*)aEnv ) == STL_SUCCESS );
        sLogBlockSlot->mSize = 0;
    }
    else
    {
        sLogBlockSlot = &sLogBlock->mLogBlockSlotArray[sBlockIdx];
    }

    if( (sLogBlockSlot->mSize + (SMR_LOGPIECE_HDR_SIZE + aLogSize)) >
        SMXM_MAX_BLOCK_SIZE )
    {
        sBlockIdx++;
        sLogBlockSlot = &sLogBlock->mLogBlockSlotArray[sBlockIdx];

        STL_TRY( knlAllocRegionMem( &aEnv->mLogHeapMem,
                                    SMXM_MAX_BLOCK_SIZE,
                                    &sLogBlockSlot->mBuffer,
                                    (knlEnv*)aEnv ) == STL_SUCCESS );
        sLogBlockSlot->mSize = 0;
    }

    sLogAnchor = sLogBlockSlot->mBuffer + sLogBlockSlot->mSize;
    
    sSlotBufPtr = sLogAnchor;
    sSlotSize = sLogBlockSlot->mSize;
    SMR_WRITE_LOGPIECE_HDR( sSlotBufPtr,
                            &sLogPieceHdr );
    sSlotSize += SMR_LOGPIECE_HDR_SIZE;

    if( aLogSize > 0 )
    {
        sSlotBufPtr += SMR_LOGPIECE_HDR_SIZE;
        STL_WRITE_BYTES( sSlotBufPtr,
                         aLogBody,
                         aLogSize );
        sSlotSize += aLogSize;
    }
    sLogBlockSlot->mSize = sSlotSize;

    STL_DASSERT( sLogBlockSlot->mSize <= SMXM_MAX_BLOCK_SIZE );

    sLogBlock->mBlockCount = sBlockIdx + 1;

    STL_ASSERT( (sLogBlock->mBlockCount > 0) &&
                (sLogBlock->mBlockCount < SMXM_MAX_LOGBLOCK_COUNT) );

    aMiniTrans->mRedoLogSize += (SMR_LOGPIECE_HDR_SIZE + aLogSize);
    aMiniTrans->mRedoLogPieceCount++;

    STL_RAMP( RAMP_SKIP_REDO );

    aMiniTrans->mTryRedoLogPieceCount++;
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Page Access Handle을 Mini-transaction에 Push한다.
 * @param[in]     aMiniTrans Mini-Transaction 포인터
 * @param[in]     aLatchMode Latch Mode
 * @param[in]     aHandle    Page Access Handle
 * @param[in,out] aEnv       Environment 정보
 * @see aLatchMode : knlLatchMode
 */
void smxmPushPage( smxmTrans * aMiniTrans,
                   stlUInt32   aLatchMode,
                   smpHandle   aHandle,
                   smlEnv    * aEnv )
{
    STL_TRY_THROW( aMiniTrans != NULL, RAMP_SUCCESS );
    STL_ASSERT( (aMiniTrans->mPageStackTop + 1) < KNL_ENV_MAX_LATCH_DEPTH );

    aMiniTrans->mPageStackTop++;
    aMiniTrans->mPageStack[aMiniTrans->mPageStackTop] = aHandle;

    if( aLatchMode == KNL_LATCH_MODE_EXCLUSIVE )
    {
        if( aMiniTrans->mMiniTransStackIdx < 0 )
        {
            aEnv->mMiniTransStackTop++;
            aEnv->mExclMiniTransStack[aEnv->mMiniTransStackTop] = aMiniTrans;
            aMiniTrans->mMiniTransStackIdx = aEnv->mMiniTransStackTop;
        }
    }
    
    STL_RAMP( RAMP_SUCCESS );
}

/**
 * @brief 미니 트랜잭션으로 부터 트랜잭션 아이디를 얻는다.
 * @param[in] aMiniTrans Mini-Transaction 포인터
 * @return Transaction Identifier
 */
smxlTransId smxmGetTransId( smxmTrans * aMiniTrans )
{
    return aMiniTrans->mTransId;
}

/**
 * @brief 미니 트랜잭션으로 부터 세그먼트 RID를 얻는다.
 * @param[in] aMiniTrans Mini-Transaction 포인터
 * @return Segment Record Identifier
 */
smlRid smxmGetSegRid( smxmTrans * aMiniTrans )
{
    return aMiniTrans->mSegRid;
}

/**
 * @brief 미니 트랜잭션의 속성을 얻는다.
 * @param[in] aMiniTrans Mini-Transaction 포인터
 * @return Mini-Transaction Attribute
 * @see smxmAttr
 */
stlUInt32 smxmGetAttr( smxmTrans * aMiniTrans )
{
    return aMiniTrans->mAttr;
}

/**
 * @brief 미니 트랜잭션의 속성을 재설정 한다.
 * @param[in] aMiniTrans Mini-Transaction 포인터
 * @param[in] aAttr Mini-transaction Attribute
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smxmResetAttr( smxmTrans * aMiniTrans,
                         stlInt32    aAttr,
                         smlEnv    * aEnv )
{
    stlUInt32 sAttr;

    STL_DASSERT( aMiniTrans->mTryRedoLogPieceCount == 0 );
    
    sAttr = aAttr;
    
    if( (aMiniTrans->mTransId == SML_INVALID_TRANSID) ||
        (gDataStoreMode <= SML_DATA_STORE_MODE_CDS) )
    {
        if( aAttr & SMXM_ATTR_REDO )
        {
            sAttr &= ~SMXM_ATTR_REDO;
        }
    }
    
    aMiniTrans->mAttr = sAttr;

    return STL_SUCCESS;
}

/**
 * @brief 미니 트랜잭션의 Target Segment Rid를 설정한다.
 * @param[in] aMiniTrans Mini-Transaction 포인터
 * @param[in] aSegmentRid Segment Rid
 * @note 미니 트랜잭션 시작시 Segment Rid를 설정할수 없는 경우에 사용한다.
 */
inline void smxmSetSegmentRid( smxmTrans * aMiniTrans,
                               smlRid      aSegmentRid )
{
    aMiniTrans->mSegRid = aSegmentRid;
}

/**
 * @brief 미니 트랜잭션으로 부터 Page Handle을 얻는다.
 * @param[in] aMiniTrans Mini-Transaction 포인터
 * @param[in] aTbsId Tablespace Identifier
 * @param[in] aPageId Page Identifier
 * @param[in] aLatchMode Latch Mode
 * @return Page Access Handle
 * @see aLatchMode : knlLatchMode
 */
smpHandle * smxmGetPageHandle( smxmTrans * aMiniTrans,
                               smlTbsId    aTbsId,
                               smlPid      aPageId,
                               stlUInt16   aLatchMode )
{
    smpHandle * sPageHandle = NULL;
    stlInt32    i;
    
    for( i = 0; i <= aMiniTrans->mPageStackTop; i++ )
    {
        if( (aMiniTrans->mPageStack[i].mPch->mTbsId == aTbsId) &&
            (aMiniTrans->mPageStack[i].mPch->mPageId == aPageId) )
        {
            sPageHandle = &aMiniTrans->mPageStack[i];
            STL_ASSERT( smpCurLatchMode( sPageHandle ) >= aLatchMode );
            break;
        }
    }
            
    return sPageHandle;
}

inline stlInt16 smxmGetLatchedPageCount( smxmTrans * aMiniTrans )
{
    return aMiniTrans->mPageStackTop + 1;
}

/** @} */
