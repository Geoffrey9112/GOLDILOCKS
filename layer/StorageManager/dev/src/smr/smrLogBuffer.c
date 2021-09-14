/*******************************************************************************
 * smrLogBuffer.c
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
#include <smlGeneral.h>
#include <smDef.h>
#include <smrDef.h>
#include <smp.h>
#include <smr.h>
#include <smf.h>
#include <smxl.h>
#include <smxm.h>
#include <smxlDef.h>
#include <smrLogFile.h>
#include <smrArchiveLog.h>
#include <smrLogGroup.h>
#include <smrLogBuffer.h>
#include <smrPendBuffer.h>
#include <smrLogMirror.h>

/**
 * @file smrLogBuffer.c
 * @brief Storage Manager Layer Recovery Log Buffer Internal Routines
 */

extern smrWarmupEntry * gSmrWarmupEntry;
extern stlUInt32        gTransTableSize;
extern stlInt64         gDataStoreMode;

/**
 * @addtogroup smr
 * @{
 */

/**
 * @brief Byte data를 로그에 기록한다.
 * @param[in] aTransId Transaction Identifier
 * @param[in] aLogData 기록할 Log Data
 * @param[in] aLogSize 기록할 Log Size
 * @param[in] aPieceCount 로그에 저장할 Log Piece의 개수
 * @param[in] aSegRid 로그와 관련된 Segment Rid
 * @param[in] aSwitchBlock 강제적인 Block Switching 여부
 * @param[out] aWrittenSbsn 기록된 로그의 로그 버퍼상 위치 정보
 * @param[out] aWrittenLsn 기록된 로그의 Lsn
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smrWriteLog( smxlTransId    aTransId,
                       void         * aLogData,
                       stlUInt32      aLogSize,
                       stlUInt16      aPieceCount,
                       smlRid         aSegRid,
                       stlBool        aSwitchBlock,
                       smrSbsn      * aWrittenSbsn,
                       smrLsn       * aWrittenLsn,
                       smlEnv       * aEnv )
{
    STL_TRY( smrWriteLogInternal( aTransId,
                                  SMR_LOGDATA_TYPE_BINARY,
                                  aLogData,
                                  aLogSize,
                                  aPieceCount,
                                  aSegRid,
                                  aSwitchBlock,
                                  aWrittenSbsn,
                                  aWrittenLsn,
                                  aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Mini-transaction Log Block을 로그에 기록한다.
 * @param[in] aTransId Transaction Identifier
 * @param[in] aLogBlock 기록할 Log Block
 * @param[in] aLogSize 기록할 Log Size
 * @param[in] aPieceCount 로그에 저장할 Log Piece의 개수
 * @param[in] aSegRid 로그와 관련된 Segment Rid
 * @param[out] aWrittenSbsn 기록된 로그를 빨리 찾기 위한 로그 버퍼상의 Hint 정보
 * @param[out] aWrittenLsn 기록된 로그의 Lsn
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smrWriteLogWithBlock( smxlTransId    aTransId,
                                void         * aLogBlock,
                                stlUInt32      aLogSize,
                                stlUInt16      aPieceCount,
                                smlRid         aSegRid,
                                smrSbsn      * aWrittenSbsn,
                                smrLsn       * aWrittenLsn,
                                smlEnv       * aEnv )
{
    STL_TRY( smrWriteLogInternal( aTransId,
                                  SMR_LOGDATA_TYPE_BLOCKARRAY,
                                  aLogBlock,
                                  aLogSize,
                                  aPieceCount,
                                  aSegRid,
                                  STL_FALSE, /* aSwitchBlock */
                                  aWrittenSbsn,
                                  aWrittenLsn,
                                  aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Checkpoint Log를 기록한다.
 * @param[in] aOldestLsn   Oldest Lsn
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smrWriteChkptLog( smrLsn    aOldestLsn,
                            smlEnv  * aEnv )
{
    smrLid              sChkptBeginLid;
    smrLsn              sChkptBeginLsn;
    smrLid              sChkptLid = SMR_INVALID_LID;
    smrLsn              sChkptLsn = SMR_INVALID_LSN;
    smrSbsn             sChkptSbsn;
    smxlTransId       * sTransArr;
    smlRid            * sUndoSegArr;
    smlScn            * sScnArr;
    stlChar           * sStateArr;
    stlBool           * sRepreparableArr;
    smxlTransId         sTransId;
    stlInt32            sTransCount = 0;
    knlRegionMark       sMemMark;
    stlInt32            sState = 0;
    stlInt64            sTransTableSize;
    smxlTransSnapshot   sTransSnapshot;
        
    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;

    /**
     * Checkpoint Begin Log 기록
     */
    STL_TRY( smrWriteChkptBeginLog( aOldestLsn,
                                    &sChkptBeginLid,
                                    &sChkptBeginLsn,
                                    &sChkptSbsn,
                                    aEnv )
             == STL_SUCCESS );

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "[CHECKPOINT] begin - checkpoint lid(%ld,%d,%d), "
                        "checkpoint lsn(%ld), oldest lsn(%ld)\n",
                        SMR_FILE_SEQ_NO(&sChkptBeginLid),
                        SMR_BLOCK_SEQ_NO(&sChkptBeginLid),
                        SMR_BLOCK_OFFSET(&sChkptBeginLid),
                        sChkptBeginLsn,
                        aOldestLsn )
             == STL_SUCCESS );

    sTransTableSize = smxlGetTransTableSize();

    /**
     * Active Transaction List 수집
     */
    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                sTransTableSize * STL_SIZEOF(smxlTransId),
                                (void**)&sTransArr,
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                sTransTableSize * STL_SIZEOF(smlRid),
                                (void**)&sUndoSegArr,
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                sTransTableSize * STL_SIZEOF(smlScn),
                                (void**)&sScnArr,
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                sTransTableSize * STL_SIZEOF(stlChar),
                                (void**)&sStateArr,
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                sTransTableSize * STL_SIZEOF(stlBool),
                                (void**)&sRepreparableArr,
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    stlMemset( sTransArr, 0x00, sTransTableSize * STL_SIZEOF(smxlTransId) );
    stlMemset( sUndoSegArr, 0x00, sTransTableSize * STL_SIZEOF(smlRid) );
    stlMemset( sScnArr, 0x00, sTransTableSize * STL_SIZEOF(smlScn) );
    stlMemset( sStateArr, 0x00, sTransTableSize * STL_SIZEOF(stlChar) );
    stlMemset( sRepreparableArr, 0x00, sTransTableSize * STL_SIZEOF(stlBool) );

    STL_TRY( smxlGetFirstActiveTrans( &sTransId,
                                      &sTransSnapshot,
                                      aEnv ) == STL_SUCCESS );

    while( sTransId != SML_INVALID_TRANSID )
    {
        /**
         * Undo가 필요한 트랜잭션만 기록한다.
         */
        if( SMXL_IS_UNDO_BOUND_TRANS( sTransId ) == STL_TRUE )
        {
            sTransArr[sTransCount] = sTransId;
            sUndoSegArr[sTransCount] = sTransSnapshot.mUndoSegRid;
            sScnArr[sTransCount] = sTransSnapshot.mTransViewScn;
            sStateArr[sTransCount] = sTransSnapshot.mState;
            sRepreparableArr[sTransCount] = sTransSnapshot.mRepreparable;
            sTransCount += 1;
        }
        
        STL_TRY( smxlGetNextActiveTrans( &sTransId,
                                         &sTransSnapshot,
                                         aEnv )
                 == STL_SUCCESS );
    }

    /**
     * Checkpoint Body 로그 기록
     */
    STL_TRY( smrWriteChkptBodyLogs( sTransArr,
                                    sUndoSegArr,
                                    sScnArr,
                                    sStateArr,
                                    sRepreparableArr,
                                    sTransCount,
                                    &sChkptLid,
                                    &sChkptLsn,
                                    &sChkptSbsn,
                                    aEnv )
             == STL_SUCCESS );

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "[CHECKPOINT] body - checkpoint lid(%ld,%d,%d), "
                        "checkpoint lsn(%ld), active transaction count(%d)\n",
                        SMR_FILE_SEQ_NO(&sChkptLid),
                        SMR_BLOCK_SEQ_NO(&sChkptLid),
                        SMR_BLOCK_OFFSET(&sChkptLid),
                        sChkptLsn,
                        sTransCount )
             == STL_SUCCESS );

    /**
     * Checkpoint End 로그 기록
     */
    STL_TRY( smrWriteChkptEndLog( aEnv,
                                  &sChkptLid,
                                  &sChkptLsn,
                                  &sChkptSbsn ) == STL_SUCCESS );

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "[CHECKPOINT] end - checkpoint lid(%ld,%d,%d), "
                        "checkpoint lsn(%ld)\n",
                        SMR_FILE_SEQ_NO(&sChkptLid),
                        SMR_BLOCK_SEQ_NO(&sChkptLid),
                        SMR_BLOCK_OFFSET(&sChkptLid),
                        sChkptLsn )
             == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       (knlEnv*)aEnv ) == STL_SUCCESS );

    /**
     * Log Flushing
     */
    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "[CHECKPOINT] flush redo log\n" )
             == STL_SUCCESS );

    STL_TRY( smrFlushLog( sChkptLsn,
                          sChkptSbsn,
                          STL_TRUE, /* aSwitchBlock */
                          SML_TRANSACTION_CWM_WAIT,
                          aEnv ) == STL_SUCCESS );

    /**
     * Datafile Header의 CheckpointLsn을 update한다.
     * Checkpoint시 backup begin된 datafile에 대해서는 flush를 skip하기 때문에
     * datafile이 flush되었는지 확인 후 datafile header를 update한다.
     */
    STL_TRY( smfChangeDatafileHeader( NULL,    /* aStatement */
                                      SMF_CHANGE_DATAFILE_HEADER_FOR_CHECKPOINT,
                                      SML_INVALID_TBS_ID,
                                      SML_INVALID_DATAFILE_ID,
                                      sChkptBeginLid,
                                      sChkptBeginLsn,
                                      aEnv )
             == STL_SUCCESS );

    /**
     * Save Control file
     */
    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "[CHECKPOINT] save control file\n" )
             == STL_SUCCESS );

    gSmrWarmupEntry->mLogPersData.mOldestLsn = aOldestLsn;
    
    STL_TRY( smfSaveCtrlFile( aEnv ) == STL_SUCCESS );
    
    gSmrWarmupEntry->mChkptInfo.mLastChkptLsn = sChkptBeginLsn;

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

/**
 * @brief Supplemental Log를 기록한다.
 * @param[in] aTransId Transaction Identifier
 * @param[in] aLogData 기록할 로그 데이터
 * @param[in] aLogSize aLogData의 바이트 단위 크기
 * @param[in] aLogType Supplemental Log Type
 * @param[in] aSegRid  관련된 Segment의 ID
 * @param[in,out] aEnv Environment 정보
 * @see aLogType : smlSupplementalLogType
 */
stlStatus smrWriteSupplementalLog( smxlTransId    aTransId,
                                   void         * aLogData,
                                   stlUInt32      aLogSize,
                                   stlUInt16      aLogType,
                                   smlRid         aSegRid,
                                   smlEnv       * aEnv )
{
    smrLogPieceHdr   sLogPieceHdr;
    stlChar          sLogBody[SMR_MAX_LOGPIECE_SIZE];
    stlUInt32        sOffset = 0;
    smrLogHdr        sLogHdr;
    smrLid           sWrittenLid;
    smrSbsn          sWrittenSbsn;
    smrLsn           sWrittenLsn;
    smlRid           sInvalidRid;

    STL_DASSERT( aLogSize + STL_SIZEOF(smrLogPieceHdr) < SMR_MAX_LOGPIECE_SIZE );
    STL_DASSERT( aLogType < SML_SUPPL_LOG_MAX );

    sInvalidRid = SML_INVALID_RID;
    SMG_WRITE_RID( &sLogPieceHdr.mDataRid, &sInvalidRid );

    sLogPieceHdr.mType = aLogType;
    sLogPieceHdr.mComponentClass = SMG_COMPONENT_EXTERNAL;
    sLogPieceHdr.mRedoTargetType = SMR_REDO_TARGET_NONE;

    SMR_WRITE_MOVE_LOGPIECE_HDR( sLogBody, &sLogPieceHdr, sOffset );
    STL_WRITE_MOVE_BYTES( sLogBody, aLogData, aLogSize, sOffset );

    sLogPieceHdr.mSize = sOffset - SMR_LOGPIECE_HDR_SIZE;
    sLogPieceHdr.mPropagateLog = SMXL_TRANS_PROPAGATE_LOG( aTransId );
    sLogPieceHdr.mRepreparable = STL_TRUE;
    SMR_WRITE_LOGPIECE_HDR( sLogBody, &sLogPieceHdr );
    
    sLogHdr.mLogPieceCount = 1;
    sLogHdr.mTransId = smxlGetTransId(aTransId);
    sLogHdr.mSize = sOffset;
    SMG_WRITE_RID( &sLogHdr.mSegRid, &aSegRid );
    sLogHdr.mPadding[0] = 0;
    sLogHdr.mPadding[1] = 0;

    STL_TRY( smrWriteStreamLog( &gSmrWarmupEntry->mLogStream,
                                &sLogHdr,
                                SMR_LOGDATA_TYPE_BINARY,
                                (void*)sLogBody,
                                STL_TRUE,   /* aUsePendingBuffer */
                                STL_FALSE,  /* aSwitchBlock */
                                &sWrittenLid,
                                &sWrittenSbsn,
                                &sWrittenLsn,
                                aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Checkpoint Begin Log를 기록한다.
 * @param[in] aOldestLsn   Oldest Lsn
 * @param[out] aChkptLid 기록된 Checkpoint Log의 Lid
 * @param[out] aChkptLsn 기록된 Checkpoint Log의 Lsn
 * @param[out] aChkptSbsn 기록된 Checkpoint Log의 Sbsn
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smrWriteChkptBeginLog( smrLsn    aOldestLsn,
                                 smrLid  * aChkptLid,
                                 smrLsn  * aChkptLsn,
                                 smrSbsn * aChkptSbsn,
                                 smlEnv  * aEnv )
{
    smrLogPieceHdr   sLogPieceHdr;
    stlChar          sLogBody[SMR_MAX_LOGPIECE_SIZE];
    stlUInt32        sOffset = 0;
    smlScn           sSystemScn;
    smrLogHdr        sLogHdr;
    smrLogStream   * sLogStream;
    smlRid           sInvalidRid;

    sLogStream = &gSmrWarmupEntry->mLogStream;
    
    sInvalidRid = SML_INVALID_RID;
    SMG_WRITE_RID( &sLogPieceHdr.mDataRid, &sInvalidRid );

    sSystemScn = smxlGetSystemScn();
    
    sLogPieceHdr.mType = SMR_LOG_CHKPT_BEGIN;
    sLogPieceHdr.mComponentClass = SMG_COMPONENT_RECOVERY;
    sLogPieceHdr.mRedoTargetType = SMR_REDO_TARGET_CTRL;

    SMR_WRITE_MOVE_LOGPIECE_HDR( sLogBody, &sLogPieceHdr, sOffset );
    STL_WRITE_MOVE_INT64( sLogBody, &aOldestLsn, sOffset );
    STL_WRITE_MOVE_INT64( sLogBody, &sSystemScn, sOffset );
    
    sLogPieceHdr.mSize = sOffset - SMR_LOGPIECE_HDR_SIZE;
    sLogPieceHdr.mPropagateLog = STL_FALSE;
    sLogPieceHdr.mRepreparable = STL_TRUE;
    SMR_WRITE_LOGPIECE_HDR( sLogBody, &sLogPieceHdr );
    
    sLogHdr.mLogPieceCount = 1;
    sLogHdr.mTransId = SMXL_CHKPT_TRANSID;
    sLogHdr.mSize = sOffset;
    SMG_WRITE_RID( &sLogHdr.mSegRid, &sInvalidRid );
    sLogHdr.mPadding[0] = 0;
    sLogHdr.mPadding[1] = 0;

    /**
     * Pending Buffer를 사용하지 않고 Log Buffer에 기록한다.
     */
    STL_TRY( smrWriteStreamLog( sLogStream,
                                &sLogHdr,
                                SMR_LOGDATA_TYPE_BINARY,
                                (void*)sLogBody,
                                STL_FALSE, /* aUsePendingBuffer */
                                STL_FALSE, /* aSwitchBlock */
                                aChkptLid,
                                aChkptSbsn,
                                aChkptLsn,
                                aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Checkpoint Body Log 들을 기록한다.
 * @param[in] aTransArr        Active Transaction Array
 * @param[in] aUndoSegArr      Active Transaction들의 Undo Segment Rid Array
 * @param[in] aScnArr          Active Transaction들의 Transaction View Scn Array
 * @param[in] aStateArr        Active Transaction들의 State Array
 * @param[in] aRepreparableArr Active Transaction들의 Repreparable Array
 * @param[in] aTransCount      aTransArr의 크기(단위: 트랜잭션)
 * @param[out] aChkptLid       기록된 Checkpoint Log의 Lid
 * @param[out] aChkptLsn       기록된 Checkpoint Log의 Lsn
 * @param[out] aChkptSbsn      기록된 Checkpoint Log의 Sbsn
 * @param[in,out] aEnv         Environment 정보
 */
stlStatus smrWriteChkptBodyLogs( smxlTransId * aTransArr,
                                 smlRid      * aUndoSegArr,
                                 smlScn      * aScnArr,
                                 stlChar     * aStateArr,
                                 stlBool     * aRepreparableArr,
                                 stlInt32      aTransCount,
                                 smrLid      * aChkptLid,
                                 smrLsn      * aChkptLsn,
                                 smrSbsn     * aChkptSbsn,
                                 smlEnv      * aEnv )
{
    smrLogPieceHdr   sLogPieceHdr;
    stlChar          sLogBody[SMR_MAX_LOGPIECE_SIZE];
    stlUInt32        sOffset;
    smrLogHdr        sLogHdr;
    smlRid           sInvalidRid;
    stlInt32         sTransCount = 0;
    stlInt32         sTotalTransCount = 0;

    while( sTotalTransCount < aTransCount )
    {
        sTransCount = STL_MIN( SMR_MAX_TRANS_COUNT_IN_PIECE,
                               aTransCount - sTotalTransCount );
        sOffset = 0;
        
        sInvalidRid = SML_INVALID_RID;
        SMG_WRITE_RID( &sLogPieceHdr.mDataRid, &sInvalidRid );
    
        sLogPieceHdr.mType = SMR_LOG_CHKPT_BODY;
        sLogPieceHdr.mComponentClass = SMG_COMPONENT_RECOVERY;
        sLogPieceHdr.mRedoTargetType = SMR_REDO_TARGET_CTRL;

        SMR_WRITE_MOVE_LOGPIECE_HDR( sLogBody, &sLogPieceHdr, sOffset );
        STL_WRITE_MOVE_INT32( sLogBody, &sTransCount, sOffset );
        STL_WRITE_MOVE_BYTES( sLogBody,
                              aTransArr + sTotalTransCount,
                              STL_SIZEOF(smxlTransId) * sTransCount,
                              sOffset );
        STL_WRITE_MOVE_BYTES( sLogBody,
                              aUndoSegArr + sTotalTransCount,
                              STL_SIZEOF(smlRid) * sTransCount,
                              sOffset );
        STL_WRITE_MOVE_BYTES( sLogBody,
                              aScnArr + sTotalTransCount,
                              STL_SIZEOF(smlScn) * sTransCount,
                              sOffset );
        STL_WRITE_MOVE_BYTES( sLogBody,
                              aStateArr + sTotalTransCount,
                              STL_SIZEOF(stlChar) * sTransCount,
                              sOffset );
        STL_WRITE_MOVE_BYTES( sLogBody,
                              aRepreparableArr + sTotalTransCount,
                              STL_SIZEOF(stlBool) * sTransCount,
                              sOffset );

        sLogPieceHdr.mSize = sOffset - SMR_LOGPIECE_HDR_SIZE;
        sLogPieceHdr.mPropagateLog = STL_FALSE;
        sLogPieceHdr.mRepreparable = STL_TRUE;
        SMR_WRITE_LOGPIECE_HDR( sLogBody, &sLogPieceHdr );
    
        sLogHdr.mLogPieceCount = 1;
        sLogHdr.mTransId = SMXL_CHKPT_TRANSID;
        sLogHdr.mSize = sOffset;
        SMG_WRITE_RID( &sLogHdr.mSegRid, &sInvalidRid );
        sLogHdr.mPadding[0] = 0;
        sLogHdr.mPadding[1] = 0;

        STL_DASSERT( sOffset < SMR_MAX_LOGPIECE_SIZE );
        
        /**
         * Pending Buffer를 사용하지 않고 Log Buffer에 기록한다.
         */
        STL_TRY( smrWriteStreamLog( &gSmrWarmupEntry->mLogStream,
                                    &sLogHdr,
                                    SMR_LOGDATA_TYPE_BINARY,
                                    (void*)sLogBody,
                                    STL_FALSE, /* aUsePendingBuffer */
                                    STL_FALSE, /* aSwitchBlock */
                                    aChkptLid,
                                    aChkptSbsn,
                                    aChkptLsn,
                                    aEnv ) == STL_SUCCESS );

        sTotalTransCount += sTransCount;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Checkpoint End Log를 기록한다.
 * @param[out]    aChkptLid    기록된 Checkpoint Log의 Lid
 * @param[out]    aChkptLsn    기록된 Checkpoint Log의 Lsn
 * @param[out]    aChkptSbsn   기록된 Checkpoint Log의 Sbsn
 * @param[in,out] aEnv         Environment 정보
 */
stlStatus smrWriteChkptEndLog( smlEnv   * aEnv,
                               smrLid   * aChkptLid,
                               smrLsn   * aChkptLsn,
                               smrSbsn  * aChkptSbsn )
{
    smrLogPieceHdr   sLogPieceHdr;
    stlChar          sLogBody[SMR_MAX_LOGPIECE_SIZE];
    smrLogHdr        sLogHdr;
    smlRid           sInvalidRid;

    sInvalidRid = SML_INVALID_RID;
    SMG_WRITE_RID( &sLogPieceHdr.mDataRid, &sInvalidRid );
    
    sLogPieceHdr.mType = SMR_LOG_CHKPT_END;
    sLogPieceHdr.mComponentClass = SMG_COMPONENT_RECOVERY;
    sLogPieceHdr.mRedoTargetType = SMR_REDO_TARGET_CTRL;
    sLogPieceHdr.mSize = 0;
    sLogPieceHdr.mPropagateLog = STL_FALSE;
    sLogPieceHdr.mRepreparable = STL_TRUE;
    SMR_WRITE_LOGPIECE_HDR( sLogBody, &sLogPieceHdr );
    
    sLogHdr.mLogPieceCount = 1;
    sLogHdr.mTransId = SMXL_CHKPT_TRANSID;
    sLogHdr.mSize = SMR_LOGPIECE_HDR_SIZE;
    SMG_WRITE_RID( &sLogHdr.mSegRid, &sInvalidRid );
    sLogHdr.mPadding[0] = 0;
    sLogHdr.mPadding[1] = 0;

    /**
     * Pending Buffer를 사용하지 않고 Log Buffer에 기록한다.
     */
    STL_TRY( smrWriteStreamLog( &gSmrWarmupEntry->mLogStream,
                                &sLogHdr,
                                SMR_LOGDATA_TYPE_BINARY,
                                (void*)sLogBody,
                                STL_FALSE, /* aUsePendingBuffer */
                                STL_FALSE, /* aSwitchBlock */
                                aChkptLid,
                                aChkptSbsn,
                                aChkptLsn,
                                aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Dummy Log를 기록한다.
 * @param[in,out] aEnv  Environment 정보
 */
stlStatus smrWriteDummyLog( smlEnv * aEnv )
{
    smrLogPieceHdr sLogPieceHdr;
    smrLogHdr      sLogHdr;
    smrLid         sDummyLid;
    smrSbsn        sDummySbsn;
    smrLsn         sDummyLsn;
    smlRid         sInvalidRid = SML_INVALID_RID;
    stlChar        sLogBody[SMR_MAX_LOGPIECE_SIZE];

    sLogHdr.mSize = SMR_LOGPIECE_HDR_SIZE;
    sLogHdr.mLogPieceCount = 1;
    sLogHdr.mTransId = SML_INVALID_TRANSID;
    SMG_WRITE_RID( &sLogHdr.mSegRid, &sInvalidRid );
    
    sLogPieceHdr.mType = SMR_LOG_DUMMY;
    sLogPieceHdr.mComponentClass = SMG_COMPONENT_RECOVERY;
    sLogPieceHdr.mRedoTargetType = SMR_REDO_TARGET_CTRL;
    sLogPieceHdr.mSize = 0;
    sLogPieceHdr.mPropagateLog = STL_FALSE;
    sLogPieceHdr.mRepreparable = STL_TRUE;
    SMR_WRITE_LOGPIECE_HDR( sLogBody, &sLogPieceHdr );
    
    STL_TRY( smrWriteStreamLog( &gSmrWarmupEntry->mLogStream,
                                &sLogHdr,
                                SMR_LOGDATA_TYPE_BINARY,
                                (void*)sLogBody,
                                STL_FALSE, /* aUsePendingBuffer */
                                STL_TRUE, /* aSwitchBlock */
                                &sDummyLid,
                                &sDummySbsn,
                                &sDummyLsn,
                                aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 기록된 모든 로그를 디스크로 내린다.
 * @param[in] aForceDiskIo  자신이 디스크 IO를 수행하는지 여부
 * @param[in,out] aEnv  Environment 정보
 * @remarks SESSION/SYSTEM Fatal 상황이나 사용자에 의한 강제적인 로그 Flush에 사용된다.
 */
stlStatus smrFlushAllLogs( stlBool    aForceDiskIo,
                           smlEnv   * aEnv )
{
    smrLogStream * sLogStream;
    smrLogBuffer * sLogBuffer;
    stlBool        sIsTimedOut;
    stlUInt32      sState = 0;
    smrSbsn        sLastSbsn;
    stlBool        sRetry;
    smrLsn         sLastLsn;

    sLogStream = &gSmrWarmupEntry->mLogStream;
    sLogBuffer = sLogStream->mLogBuffer;

    STL_RAMP( RAMP_RETRY );

    sState = 0;
    sRetry = STL_FALSE;
    
    STL_TRY( knlAcquireLatch( &sLogBuffer->mBufferLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_HIGH,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;

    sLastSbsn = sLogBuffer->mRearSbsn;
    sLastLsn = gSmrWarmupEntry->mLsn - 1;
    
    /**
     * Pending Log에 Flush해야할 로그가 존재한다면 Pending Log 기록이
     * 종료할때까지 기다린다.
     */
    STL_TRY( smrWaitPendingLog( sLogStream,
                                sLastLsn,
                                aEnv )
             == STL_SUCCESS );
    
    /**
     * Pending Buffer에 남아 있는 로그를 기록한다.
     * - flusher에 의해서 발생된 것이라면 flusher에 대기해서는 안된다.
     */
    STL_TRY( smrApplyPendingLogs( sLogStream,
                                  sLastLsn,
                                  &sLastSbsn,
                                  &sRetry,
                                  (aForceDiskIo == STL_TRUE) ? STL_FALSE : STL_TRUE,
                                  aEnv )
             == STL_SUCCESS );

    if( sRetry == STL_TRUE )
    {
        STL_TRY( knlAcquireLatch( &sLogBuffer->mBufferLatch,
                                  KNL_LATCH_MODE_EXCLUSIVE,
                                  KNL_LATCH_PRIORITY_HIGH,
                                  STL_INFINITE_TIME,
                                  &sIsTimedOut,
                                  (knlEnv*)aEnv ) == STL_SUCCESS );
        sState = 1;
    }

    /**
     * 마지막까지 flush한다.
     */
    if( (sLastSbsn == sLogBuffer->mRearSbsn) ||
        (sLastSbsn == SMR_INVALID_SBSN) )
    {
        if( sLogBuffer->mBlockOffset > SMR_BLOCK_HDR_SIZE )
        {
            if( sLogBuffer->mRearFileBlockSeqNo + 1 >=
                smrGetMaxFileBlockCount( sLogStream, sLogStream->mCurLogGroup4Buffer ) )
            {
                /**
                 * cache coherency 문제로 인해 RearSbsn을 증가시키기 전에
                 * LogSwitchingSbsn을 증가 시킨다.
                 */
                sLogStream->mCurLogGroup4Buffer->mLogSwitchingSbsn = sLogBuffer->mRearSbsn + 1;
                stlMemBarrier();
                SMR_BUFFER_BLOCK_SWITCH( sLogBuffer );
                
                sLogBuffer->mFileSeqNo += 1;
                sLogBuffer->mRearFileBlockSeqNo = 0;
                sLogStream->mCurLogGroup4Buffer = smrGetNextLogGroup( sLogStream,
                                                                      sLogStream->mCurLogGroup4Buffer );
            }
            else
            {
                SMR_BUFFER_BLOCK_SWITCH( sLogBuffer );
            }
        }
        
        /**
         * 기록된 마지막 SBSN으로 Flush SBSN을 설정한다.
         */
        sLastSbsn = sLogBuffer->mRearSbsn - 1;
        sLastLsn = sLogBuffer->mRearLsn;
    }
        
    sState = 0;
    STL_TRY( knlReleaseLatch( &sLogBuffer->mBufferLatch,
                              (knlEnv*)aEnv ) == STL_SUCCESS );

    if( aForceDiskIo == STL_TRUE )
    {
        STL_TRY( smrFlushBuffer( sLogStream,
                                 sLastSbsn,
                                 aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( smrWaitFlusher( sLogStream,
                                 sLastLsn,
                                 sLastSbsn,
                                 SML_TRANSACTION_CWM_WAIT,
                                 aEnv ) == STL_SUCCESS );
    }

    /**
     * 기록 도중 중간에 실패하면 그 이후부터 다시한다.
     */
    STL_TRY_THROW( sRetry == STL_FALSE, RAMP_RETRY );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        knlReleaseLatch( &sLogBuffer->mBufferLatch, (knlEnv*)aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief Log Flusher에서 호출되며, Stream의 최신 로그까지 Disk로 저장한다.
 * @param[out] aFlushedLsn Flush 된 마지막 LSN 값
 * @param[out] aFlushedSbsn Flush 된 마지막 SBSN 값
 * @param[in,out] aEnv Environment 정보
 * @remarks Page Flusher에 의해서 발생한다.
 */
stlStatus smrFlushStreamLogForFlusher( stlInt64 * aFlushedLsn,
                                       stlInt64 * aFlushedSbsn,
                                       smlEnv   * aEnv )
{
    smrLogStream * sLogStream;
    smrLogBuffer * sLogBuffer;
    smrSbsn        sTargetSbsn;
    stlBool        sIsSuccess;
    stlInt64       sBlockFlush = 1;
    stlUInt32      sState = 0;

    sLogStream = &gSmrWarmupEntry->mLogStream;
    sLogBuffer = sLogStream->mLogBuffer;

    while( sBlockFlush > 0 )
    {
        STL_TRY( knlTryLatch( &sLogStream->mFileLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              &sIsSuccess,
                              KNL_ENV(aEnv) ) == STL_SUCCESS );

        if( sIsSuccess == STL_FALSE )
        {
            break;
        }
        
        sState = 1;

        sTargetSbsn = sLogBuffer->mRearSbsn - 1;
            
        if( sTargetSbsn <= sLogBuffer->mFrontSbsn )
        {
            *aFlushedLsn = smrGetFrontLsn();
            *aFlushedSbsn = smrGetFrontSbsn();
            
            sState = 0;
            STL_TRY( knlReleaseLatch( &sLogStream->mFileLatch,
                                      KNL_ENV(aEnv) ) == STL_SUCCESS );
            break;
        }
        
        /**
         * Log buffer flush가 enable이면 flush하고 종료
         * 그렇지 않으면 flush하지 않고 Latch 푼 후 retry
         */
        sBlockFlush = gSmrWarmupEntry->mBlockLogFlushing;

        if( sBlockFlush == 0 )
        {
            STL_TRY( smrFlushBufferInternal( sLogStream,
                                             sTargetSbsn,
                                             aEnv )
                     == STL_SUCCESS );
        }

        *aFlushedLsn = smrGetFrontLsn();
        *aFlushedSbsn = smrGetFrontSbsn();
            
        sState = 0;
        STL_TRY( knlReleaseLatch( &sLogStream->mFileLatch,
                                  KNL_ENV(aEnv) ) == STL_SUCCESS );

        STL_TRY( smrCondBroadcast( sLogStream,
                                   aEnv )
                 == STL_SUCCESS );
        
        /**
         * Log buffer를 flush하지 못했으면 sleep 후 retry
         */
        if( sBlockFlush > 0 )
        {
            stlSleep( 10000 );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlReleaseLatch( &sLogStream->mFileLatch, KNL_ENV(aEnv) );
    }

    return STL_FAILURE;
}

/**
 * @brief Log Flusher에서 호출되며, Conditional Variable에 대기하고 있는 트랜잭션들을 깨운다.
 * @param[in]     aLogStream   Log Stream Pointer
 * @param[in,out] aEnv         Environment 정보
 * @remarks Transaction Syncr에 대기하는 트랜잭션들은 LSN과 SBSN 비교를 통해서 대상을 선정하고,
 *          Log Blocking으로 인해 대기하는 트랜잭션들은 무조건 wakeup 대상에 포함된다.
 */
stlStatus smrCondBroadcast( smrLogStream * aLogStream,
                            smlEnv       * aEnv )
{
    knlLatchWaiter * sCurWaiter;
    knlLatchWaiter * sNextWaiter;
    stlUInt32        sState = 0;
    smrLogBuffer   * sLogBuffer;
    knlCondVar     * sCondVar;

    sLogBuffer = aLogStream->mLogBuffer;
    sCondVar = &aLogStream->mCondVar;
    
    stlAcquireSpinLock( &sCondVar->mSpinLock,
                        NULL /* Miss count */ );
    sState = 1;

    STL_RING_FOREACH_ENTRY_SAFE( &sCondVar->mWaiters, sCurWaiter, sNextWaiter )
    {
        STL_ASSERT( sCurWaiter != sNextWaiter );

        if( sCurWaiter->mForLogFlush == STL_TRUE )
        {
            /**
             * SBSN 과 LSN 두 조건을 모두 만족시키지 못하는 경우는 깨우지 않는다.
             */
            
            if( ( sLogBuffer->mFrontSbsn < sCurWaiter->mFlushSbsn ) &&
                ( sLogBuffer->mFrontLsn < sCurWaiter->mFlushLsn ) )
            {
                continue;
            }
        }
        
        STL_RING_UNLINK( &sCondVar->mWaiters, sCurWaiter );
        
        STL_TRY( stlReleaseSemaphore( &sCurWaiter->mSem,
                                      KNL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );
    }

    sState = 0;
    stlReleaseSpinLock( &sCondVar->mSpinLock );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        stlReleaseSpinLock( &sCondVar->mSpinLock );
    }
    
    return STL_FAILURE;
}


/**
 * @brief 시스템이 Logging을 허용할때까지 기다린다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smrWaitEnableLogging( smlEnv * aEnv )
{
    stlBool        sIsTimedOut;
    smrLogStream * sLogStream;
    
    sLogStream = &gSmrWarmupEntry->mLogStream;
    
    while( gSmrWarmupEntry->mEnableLogging == STL_FALSE )
    {
        STL_TRY( knlCondWait( &sLogStream->mCondVar,
                              STL_TIMESLICE_TIME,
                              &sIsTimedOut,
                              KNL_ENV( aEnv ) ) == STL_SUCCESS );

        STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Logging System의 Front LSN을 얻는다.
 * @return Front LSN
 */
stlInt64 smrGetFrontLsn()
{
    smrLogStream * sLogStream;
    smrLogBuffer * sLogBuffer;

    sLogStream = &gSmrWarmupEntry->mLogStream;
    STL_DASSERT( sLogStream != NULL );
    sLogBuffer = sLogStream->mLogBuffer;
    STL_DASSERT( sLogBuffer != NULL );

    return sLogBuffer->mFrontLsn;
}

/**
 * @brief Logging System의 Front SBSN을 얻는다.
 * @return Front SBSN
 */
stlInt64 smrGetFrontSbsn()
{
    smrLogStream * sLogStream;
    smrLogBuffer * sLogBuffer;

    sLogStream = &gSmrWarmupEntry->mLogStream;
    STL_DASSERT( sLogStream != NULL );
    sLogBuffer = sLogStream->mLogBuffer;
    STL_DASSERT( sLogBuffer != NULL );

    return sLogBuffer->mFrontSbsn;
}

/**
 * @brief Logging System의 Rear SBSN을 얻는다.
 * @return Rear SBSN
 */
stlInt64 smrGetRearSbsn()
{
    smrLogStream * sLogStream;
    smrLogBuffer * sLogBuffer;

    sLogStream = &gSmrWarmupEntry->mLogStream;
    STL_DASSERT( sLogStream != NULL );
    sLogBuffer = sLogStream->mLogBuffer;
    STL_DASSERT( sLogBuffer != NULL );

    return sLogBuffer->mRearSbsn;
}

/** @} */

/**
 * @addtogroup smrLogBuffer
 * @{
 */

stlStatus smrWriteLogInternal( smxlTransId      aTransId,
                               smrLogDataType   aLogDataType,
                               void           * aLogData,
                               stlUInt32        aLogSize,
                               stlUInt16        aPieceCount,
                               smlRid           aSegRid,
                               stlBool          aSwitchBlock,
                               smrSbsn        * aWrittenSbsn,
                               smrLsn         * aWrittenLsn,
                               smlEnv         * aEnv )
{
    smrLsn         sWrittenLsn;
    smrLogHdr      sLogHdr;
    smrLogStream * sLogStream;
    smxlTransId    sTransId;
    
    STL_TRY_THROW( aTransId != SML_INVALID_TRANSID, RAMP_SUCCESS );
    STL_TRY_THROW( aLogSize < SMR_MAX_LOGBODY_SIZE, RAMP_SUCCESS );

    /*
     * 사용자가 aTransId를 Non-Physical Transaction Identifier를 줄수도 있기 때문에, 
     * Physical Transaction Identifier로 변환시킨다.
     */
    if( SMXL_IS_SYSTEM_TRANS( aTransId ) != STL_TRUE )
    {
        sTransId = smxlGetTransId( aTransId );
    }
    else
    {
        sTransId = aTransId;
    }

    sLogStream = &gSmrWarmupEntry->mLogStream;

    sLogHdr.mLogPieceCount = aPieceCount;
    sLogHdr.mTransId = sTransId;
    sLogHdr.mSize = aLogSize;
    SMG_WRITE_RID( &sLogHdr.mSegRid, &aSegRid );
    sLogHdr.mPadding[0] = 0;
    sLogHdr.mPadding[1] = 0;

    STL_TRY( smrWriteStreamLog( sLogStream,
                                &sLogHdr,
                                aLogDataType,
                                aLogData,
                                STL_TRUE,  /* aUsePendingBuffer */
                                aSwitchBlock,
                                NULL,      /* aWrittenLid */
                                aWrittenSbsn,
                                &sWrittenLsn,
                                aEnv ) == STL_SUCCESS );
            
    if( aWrittenLsn != NULL )
    {
        *aWrittenLsn = sWrittenLsn;
    }
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrWriteStreamLog( smrLogStream   * aLogStream,
                             smrLogHdr      * aLogHdr,
                             smrLogDataType   aLogDataType,
                             void           * aLogData,
                             stlBool          aUsePendingBuffer,
                             stlBool          aSwitchBlock,
                             smrLid         * aWrittenLid,
                             smrSbsn        * aWrittenSbsn,
                             smrLsn         * aWrittenLsn,
                             smlEnv         * aEnv )
{
    stlUInt32      sState;
    stlBool        sIsSuccess;
    smrLsn         sLsn;
    stlBool        sIsTimedOut;
    stlBool        sRetry;
    smrLogBuffer * sLogBuffer;

    STL_ASSERT( aWrittenLsn != NULL );

    STL_RAMP( RAMP_RETRY );

    sRetry = STL_FALSE;
    sState = 0;
    sIsSuccess = STL_TRUE;
    
    sLogBuffer = aLogStream->mLogBuffer;
    
    /**
     * Pending Buffer를 사용할수 있는 경우는 Try Latch를 사용한다.
     */
    if( (aUsePendingBuffer == STL_TRUE) &&
        (aLogStream->mPendBufferCount > 0) )
    {
        STL_TRY( knlTryLatch( &sLogBuffer->mBufferLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              &sIsSuccess,
                              (knlEnv*)aEnv ) == STL_SUCCESS );
        
        /**
         * Try Latch가 실패한다면 Logging Scalability를 높이기 위해서 Pending Buffer에
         * 기록한다. 만약 Try Latch가 성공하면 Pending Buffer에 있는 Pending 로그들을
         * Log Buffer에 반영하고, 자신의 로그도 기록한다.
         */
        if( sIsSuccess == STL_TRUE )
        {
            sState = 1;
        }
        else
        {
            /**
             * Pending Buffer에만 기록하고 리턴한다.
             * - 이후 Try Latch를 성공한 다른 트랜잭션에 의해서 Log Buffer에 기록된다.
             */
            STL_TRY( smrWritePendingLog( aLogStream,
                                         aLogHdr,
                                         aLogDataType,
                                         aLogData,
                                         &sIsSuccess,
                                         aWrittenLsn,
                                         aEnv )
                     == STL_SUCCESS );

            if( sIsSuccess == STL_TRUE )
            {
                /**
                 * Log Buffer에 기록되지 않았기 때문에 정확한 SBSN을 알수없다.
                 */
                if( aWrittenSbsn != NULL )
                {
                    *aWrittenSbsn = SMR_INVALID_SBSN;
                }

                STL_THROW( RAMP_FINISH );
            }
        }
    }

    if( sState == 0 )
    {
        STL_TRY( knlAcquireLatchWithInfo( &sLogBuffer->mBufferLatch,
                                          KNL_LATCH_MODE_EXCLUSIVE,
                                          KNL_LATCH_PRIORITY_NORMAL,
                                          STL_INFINITE_TIME,
                                          1,                 /* aRetryCount */
                                          &sIsTimedOut,
                                          &sIsSuccess,
                                          (knlEnv*)aEnv ) == STL_SUCCESS );

        STL_TRY_THROW( sIsSuccess == STL_TRUE, RAMP_RETRY );
        sState = 1;
    }

    sLsn = stlAtomicInc64( (stlUInt64*)&(gSmrWarmupEntry->mLsn) );

    /**
     * 강제적으로 Log Buffer에 기록해야 하는 경우는 남아있는 Pending Log를
     * Log Buffer에 반영해야 한다.
     * - Checkpoint Log가 이에 해당된다.
     */
    if( aLogStream->mPendBufferCount > 0 )
    {
        if( sLsn != (sLogBuffer->mRearLsn + 1) )
        {
            STL_TRY( smrWaitPendingLog( aLogStream,
                                        sLsn - 1,
                                        aEnv )
                     == STL_SUCCESS );

            /**
             * 모든 Pending Log들을 Log Buffer에 반영한다.
             */
            STL_TRY( smrApplyPendingLogs( aLogStream,
                                          sLsn - 1,
                                          NULL,     /* aLastSbsn */
                                          &sRetry,
                                          STL_TRUE, /* aWaitFlusher */
                                          aEnv )
                     == STL_SUCCESS );

            /**
             * Retry가 발생할때는 Buffer Latch에 락이 풀려있다.
             */
            STL_TRY_THROW( sRetry == STL_FALSE, RAMP_RETRY );
        }
        
        /**
         * 최신 로그만이 로그버퍼에 기록될수 있다.
         */
        KNL_ASSERT( sLsn > sLogBuffer->mRearLsn,
                    KNL_ENV(aEnv),
                    ("LSN(%ld), REAR_LSN(%ld)",
                     sLsn,
                     sLogBuffer->mRearLsn) );
    }

    aLogHdr->mLsn = sLsn;
    
    /**
     * 자신의 로그를 기록한다.
     */
    STL_TRY( smrWriteStreamLogInternal( aLogStream,
                                        aLogHdr,
                                        aLogDataType,
                                        aLogData,
                                        STL_FALSE, /* aPendingLog */
                                        STL_TRUE,  /* aWaitFlusher */
                                        aWrittenLid,
                                        aWrittenSbsn,
                                        aWrittenLsn,
                                        &sRetry,
                                        aEnv )
             == STL_SUCCESS );

    STL_TRY_THROW( sRetry == STL_FALSE, RAMP_RETRY );

    if( aSwitchBlock == STL_TRUE )
    {
        SMR_BUFFER_BLOCK_SWITCH( sLogBuffer );
    }
    
    sState = 0;
    if( aLogStream->mPendBufferCount > 0 )
    {
        STL_TRY( knlReleaseLatchWithInfo( &sLogBuffer->mBufferLatch,
                                          aLogStream->mPendBufferCount,
                                          (knlEnv*)aEnv ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( knlReleaseLatchWithInfo( &sLogBuffer->mBufferLatch,
                                          1,
                                          (knlEnv*)aEnv ) == STL_SUCCESS );
    }

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        if( aLogStream->mPendBufferCount > 0 )
        {
            knlReleaseLatchWithInfo( &sLogBuffer->mBufferLatch,
                                     aLogStream->mPendBufferCount,
                                     (knlEnv*)aEnv );
        }
        else
        {
            knlReleaseLatchWithInfo( &sLogBuffer->mBufferLatch,
                                     1,
                                     (knlEnv*)aEnv );
        }
    }

    return STL_FAILURE;
}

stlStatus smrWriteStreamLogInternal( smrLogStream   * aLogStream,
                                     smrLogHdr      * aLogHdr,
                                     smrLogDataType   aLogDataType,
                                     void           * aLogData,
                                     stlBool          aPendingLog,
                                     stlBool          aWaitFlusher,
                                     smrLid         * aWrittenLid,
                                     smrSbsn        * aWrittenSbsn,
                                     smrLsn         * aWrittenLsn,
                                     stlBool        * aRetry,
                                     smlEnv         * aEnv )
{
    smrLogBuffer       * sLogBuffer;
    smxmLogBlockCursor   sLogBlockCursor;
    stlInt32             sWrittenBytes;
    stlUInt32            sTotalWrittenBytes = 0;
    smrLid               sWrittenLid = SMR_INVALID_LID; 
    void               * sLogData;
    stlInt32             sNeedBlockCount = 0;
    smrLogHdr          * sLogHdr;
    smrSbsn              sWrittenSbsn = SMR_INVALID_SBSN;
    smrSbsn              sWaitSbsn;
    stlBool              sIsTimedOut;
    stlUInt32            sState = 0;
    stlInt32             sGuaranteeDis;
    
    *aRetry = STL_FALSE;

    sGuaranteeDis = (SML_PENDING_LOG_BUFFER_SIZE / SMR_BLOCK_SIZE);
    
    sLogBuffer = aLogStream->mLogBuffer;
    
    SMXM_INIT_LOG_BLOCK_CURSOR( &sLogBlockCursor );
    
    STL_DASSERT( (sLogBuffer->mRearSbsn - sLogBuffer->mFrontSbsn) <=
                 (sLogBuffer->mBufferBlockCount - sGuaranteeDis) );

    while( 1 )
    {
        sNeedBlockCount = smrNeedBlockCount( aLogStream, aLogHdr->mSize );

        /**
         * File across chained log를 허용하지 않기 위해서
         */
        if( (sLogBuffer->mRearFileBlockSeqNo + sNeedBlockCount) >=
            smrGetMaxFileBlockCount( aLogStream, aLogStream->mCurLogGroup4Buffer ) )
        {
            if( sLogBuffer->mBlockOffset > SMR_BLOCK_HDR_SIZE )
            {
                /**
                 * cache coherency 문제로 인해 RearSbsn을 증가시키기 전에
                 * LogSwitchingSbsn을 증가 시킨다.
                 */
                aLogStream->mCurLogGroup4Buffer->mLogSwitchingSbsn = sLogBuffer->mRearSbsn + 1;
                stlMemBarrier();
                SMR_BUFFER_BLOCK_SWITCH( sLogBuffer );
            }
            else
            {
                aLogStream->mCurLogGroup4Buffer->mLogSwitchingSbsn = sLogBuffer->mRearSbsn;
            }
            
            sLogBuffer->mFileSeqNo += 1;
            sLogBuffer->mRearFileBlockSeqNo = 0;
            aLogStream->mCurLogGroup4Buffer = smrGetNextLogGroup( aLogStream,
                                                                  aLogStream->mCurLogGroup4Buffer );
            
            /**
             * CURRENT 상태의 Log Group과 ACTIVE 상태의 Log Group과의 차가 정해진 범위를
             * 넘어서면 모든 트랜잭션의 Log 기록을 막아야 한다.
             */
            STL_TRY( smrTryDisableLogging( aLogStream,
                                           aLogStream->mCurLogGroup4Buffer,
                                           aEnv ) == STL_SUCCESS );
        }
        else
        {
            /**
             * RearSbsn이 Front Sbsn을 덮어치지 않게 최대 Log Size 간격을 보장한다.
             */ 
            if( (sLogBuffer->mRearSbsn + sNeedBlockCount - sLogBuffer->mFrontSbsn) >=
                (sLogBuffer->mBufferBlockCount - sGuaranteeDis - 1) )
            {
                aLogStream->mStatistics.mWaitCountByBufferFull++;

                sState = 0;
                STL_TRY( knlReleaseLatch( &sLogBuffer->mBufferLatch,
                                          (knlEnv*)aEnv ) == STL_SUCCESS );

                if( aWaitFlusher == STL_TRUE )
                {
                    /**
                     * MAXIMUM_FLUSH_LOG_BLOCK_COUNT의 MinValue/2 정도를 기다린다.
                     */
                    
                    sWaitSbsn = STL_MIN( sLogBuffer->mFrontSbsn + (1000/2),
                                         sLogBuffer->mRearSbsn );
                    
                    STL_TRY( smrWaitFlusher( aLogStream,
                                             STL_INT64_MAX,
                                             sWaitSbsn,
                                             SML_TRANSACTION_CWM_WAIT,
                                             aEnv ) == STL_SUCCESS );
                }
                
                *aRetry = STL_TRUE;
                STL_THROW( RAMP_RETRY );
            }
            
            break;
        }
    }

    sLogHdr = aLogHdr;

    /**
     * LSN 기록 순서는 보장되어야 한다.
     */
    KNL_ASSERT( aLogHdr->mLsn > sLogBuffer->mRearLsn,
                KNL_ENV(aEnv),
                ("LSN(%ld), REAR_LSN(%ld)",
                 aLogHdr->mLsn,
                 sLogBuffer->mRearLsn) );

    while( sTotalWrittenBytes < aLogHdr->mSize )
    {
        if( aLogDataType == SMR_LOGDATA_TYPE_BLOCKARRAY )
        {
            sLogData = aLogData;
        }
        else
        {
            sLogData = aLogData + sTotalWrittenBytes;
        }
        
        STL_TRY( smrWriteLogToBlock( aLogStream,
                                     &sLogBlockCursor,
                                     sLogHdr,
                                     aLogHdr->mLsn,
                                     aLogDataType,
                                     sLogData,
                                     aLogHdr->mSize - sTotalWrittenBytes,
                                     &sWrittenBytes,
                                     &sWrittenLid,
                                     &sWrittenSbsn,
                                     aEnv ) == STL_SUCCESS );

        sLogHdr = NULL;
        sLogBuffer->mRearLsn = aLogHdr->mLsn;
        sTotalWrittenBytes += sWrittenBytes;
    }

    sLogBuffer->mRearLid = sWrittenLid;
    
    if( aWrittenLsn != NULL )
    {
        *aWrittenLsn = aLogHdr->mLsn;
    }

    if( aWrittenSbsn != NULL )
    {
        *aWrittenSbsn = sWrittenSbsn;
    }
    
    if( aWrittenLid != NULL )
    {
        *aWrittenLid = sWrittenLid;
    }

    STL_RAMP( RAMP_RETRY );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 0 )
    {
        (void)knlAcquireLatch( &sLogBuffer->mBufferLatch,
                               KNL_LATCH_MODE_EXCLUSIVE,
                               KNL_LATCH_PRIORITY_NORMAL,
                               STL_INFINITE_TIME,
                               &sIsTimedOut,
                               (knlEnv*)aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief Log Buffer의 aLsn 이전의 로그들을 Disk로 저장한다.
 * @param[in]     aLsn          디스크로 저장되어야할 Maximum Lsn
 * @param[in]     aSbsn         로그가 기록될 당시 버퍼 위치에 대한 힌트 정보
 * @param[in]     aSwitchBlock  aSbsn에 해당하는 로그가 block switching이 필요한지 여부
 * @param[in]     aWriteMode    Sync/Async 여부
 * @param[in,out] aEnv          Environment 정보
 * @see @a aWriteMode : smlTransactionWriteMode
 * @remarks @a aSwitchBlock : Transaction Commit/Rollback시에는 이미 block을 switch한 상태에서
 *                            flush를 요구하기 때문에 래치 획득이후에 block switching을 피하기
 *                            위해서 필요한 파라이터이다.
 */
stlStatus smrFlushLog( smrLsn      aLsn,
                       smrSbsn     aSbsn,
                       stlBool     aSwitchBlock,
                       stlInt64    aWriteMode,
                       smlEnv    * aEnv )
{
    smrLogBuffer * sLogBuffer;
    smrLogStream * sLogStream;
    stlUInt32      sState;
    smrSbsn        sLastSbsn;
    stlBool        sRetry;
    stlBool        sSkipFlush;
    stlBool        sIsSuccess = STL_FALSE;

    sLogStream = &gSmrWarmupEntry->mLogStream;
    sLogBuffer = sLogStream->mLogBuffer;

    STL_RAMP( RAMP_RETRY );

    sState = 0;
    sRetry = STL_FALSE;

    /**
     * 아래 2가지 경우에서 Pending Log를 기록해야 한다.
     * (1) Flusher가 In-Active 상태일 경우
     * (2) Flusher가 Active 상태이지만 Sync Mode가 SYNC 인 경우
     */
    if( smrIsLogFlusherActive() == STL_TRUE )
    {
        if( aWriteMode == SML_TRANSACTION_CWM_NO_WAIT )
        {
            sLastSbsn = sLogBuffer->mRearSbsn;
            STL_THROW( RAMP_SUCCESS );
        }
    }

    STL_TRY_THROW( sLogBuffer->mFrontLsn < aLsn, RAMP_SUCCESS );

    /**
     * aLsn 이전 로그들이 이미 로그 버퍼에 저장되어 있고, block switching이
     * 이루어진 경우라면 래치를 획득할 필요가 없다.
     */
    if( aSwitchBlock == STL_FALSE )
    {
        if( aSbsn != SMR_INVALID_SBSN )
        {
            sLastSbsn = aSbsn;
            STL_THROW( RAMP_SKIP_APPLY_PENDING_LOGS );
        }
    }

    STL_TRY( smrAcquireLatchOrJoinGroup( sLogStream,
                                         aLsn,
                                         &aLsn,
                                         &sSkipFlush,
                                         aEnv ) == STL_SUCCESS );

    /**
     * Group Commit의 Member라면 Pending log를 기록할 필요가 없다.
     */
    if( sSkipFlush == STL_TRUE )
    {
        /**
         * LSN 만으로 Group Commit을 기다려야 한다.
         */
        sLastSbsn = STL_INT64_MAX;
        STL_THROW( RAMP_SKIP_APPLY_PENDING_LOGS );
    }
    
    sState = 1;

    /**
     * 래치 획득이후 다시 한번 확인한다.
     */
    if( sLogBuffer->mFrontLsn >= aLsn )
    {
        sState = 0;
        STL_TRY( knlReleaseLatch( &sLogBuffer->mBufferLatch,
                                  KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        STL_THROW( RAMP_SUCCESS );
    }

    sLastSbsn = aSbsn;

    /**
     * Flush 해야할 LSN이 버퍼 기록된 LSN 보다 크다면 다른 트랜잭션들의
     * Pending Log 기록이 종료할때까지 기다린다.
     */
    if( aLsn > sLogBuffer->mRearLsn )
    {
        STL_TRY( smrWaitPendingLog( sLogStream,
                                    aLsn,
                                    aEnv )
                 == STL_SUCCESS );
    }
    
    /**
     * Pending Buffer에 남아 있는 로그를 기록한다.
     */
    STL_TRY( smrApplyPendingLogs( sLogStream,
                                  aLsn,
                                  &sLastSbsn,
                                  &sRetry,
                                  STL_TRUE, /* aWaitFlusher */
                                  aEnv )
             == STL_SUCCESS );

    /**
     * 기록 도중 중간에 실패하면 그 이후부터 다시한다.
     */
    STL_TRY_THROW( sRetry == STL_FALSE, RAMP_RETRY );

    /**
     * 파라마터 SBSN 힌트도 유효하지 않고, 적용된 Pending Log도 없다면
     * 마지막까지 flush한다.
     */
    if( (sLastSbsn == sLogBuffer->mRearSbsn) ||
        (sLastSbsn == SMR_INVALID_SBSN) )
    {
        if( sLogBuffer->mBlockOffset > SMR_BLOCK_HDR_SIZE )
        {
            if( sLogBuffer->mRearFileBlockSeqNo + 1 >=
                smrGetMaxFileBlockCount( sLogStream, sLogStream->mCurLogGroup4Buffer ) )
            {
                /**
                 * cache coherency 문제로 인해 RearSbsn을 증가시키기 전에
                 * LogSwitchingSbsn을 증가 시킨다.
                 */
                sLogStream->mCurLogGroup4Buffer->mLogSwitchingSbsn = sLogBuffer->mRearSbsn + 1;
                stlMemBarrier();
                SMR_BUFFER_BLOCK_SWITCH( sLogBuffer );
                
                sLogBuffer->mFileSeqNo += 1;
                sLogBuffer->mRearFileBlockSeqNo = 0;
                sLogStream->mCurLogGroup4Buffer = smrGetNextLogGroup( sLogStream,
                                                                      sLogStream->mCurLogGroup4Buffer );
            }
            else
            {
                SMR_BUFFER_BLOCK_SWITCH( sLogBuffer );
            }
        }
        
        /**
         * 현재 SBSN으로 Flush SBSN을 설정한다.
         */
        sLastSbsn = sLogBuffer->mRearSbsn - 1;
    }
        
    sState = 0;
    STL_TRY( knlReleaseLatch( &sLogBuffer->mBufferLatch,
                              (knlEnv*)aEnv ) == STL_SUCCESS );

    sLogStream->mStatistics.mWaitCount4Sync++;

    STL_RAMP( RAMP_SKIP_APPLY_PENDING_LOGS );

    STL_TRY( smrFlushBufferByTrans( sLogStream,
                                    sLastSbsn,
                                    &sIsSuccess,
                                    aEnv )
             == STL_SUCCESS );

    /**
     * Transaction Log Flushing이 성공하면 Flusher에 대기할 필요가 없다.
     */

    if( sIsSuccess == STL_TRUE )
    {
        STL_TRY( smrCondBroadcast( sLogStream,
                                   aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( smrWaitFlusher( sLogStream,
                                 aLsn,
                                 sLastSbsn,
                                 aWriteMode,
                                 aEnv ) == STL_SUCCESS );
    }
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        knlReleaseLatch( &sLogBuffer->mBufferLatch, (knlEnv*)aEnv );
    }

    return STL_FAILURE;
}

stlStatus smrFlushBuffer( smrLogStream * aLogStream,
                          smrSbsn        aTargetSbsn,
                          smlEnv       * aEnv )
{
    stlBool        sIsTimedOut;
    stlInt64       sBlockFlush = 1;
    stlUInt32      sState = 0;
    smrLogBuffer * sLogBuffer;

    sLogBuffer = aLogStream->mLogBuffer;

    while( sBlockFlush > 0 )
    {
        if( aTargetSbsn <= sLogBuffer->mFrontSbsn )
        {
            break;
        }
        
        STL_TRY( knlAcquireLatch( &aLogStream->mFileLatch,
                                  KNL_LATCH_MODE_EXCLUSIVE,
                                  KNL_LATCH_PRIORITY_HIGH,
                                  STL_INFINITE_TIME,
                                  &sIsTimedOut,
                                  KNL_ENV(aEnv) ) == STL_SUCCESS );
        sState = 1;

        /**
         * Log buffer flush가 enable이면 flush하고 종료
         * 그렇지 않으면 flush하지 않고 Latch 푼 후 retry
         */
        sBlockFlush = gSmrWarmupEntry->mBlockLogFlushing;

        if( sBlockFlush == 0 )
        {
            STL_TRY( smrFlushBufferInternal( aLogStream,
                                             aTargetSbsn,
                                             aEnv )
                     == STL_SUCCESS );
        }

        sState = 0;
        STL_TRY( knlReleaseLatch( &aLogStream->mFileLatch,
                                  KNL_ENV(aEnv) ) == STL_SUCCESS );

        /**
         * Log buffer를 flush하지 못했으면 sleep 후 retry
         */
        if( sBlockFlush > 0 )
        {
            stlSleep( 10000 );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlReleaseLatch( &aLogStream->mFileLatch, KNL_ENV(aEnv) );
    }

    return STL_FAILURE;
}


stlStatus smrFlushBufferByTrans( smrLogStream * aLogStream,
                                 smrSbsn        aTargetSbsn,
                                 stlBool      * aIsSuccess,
                                 smlEnv       * aEnv )
{
    stlUInt32  sState = 0;

    STL_TRY( knlTryLatch( &aLogStream->mFileLatch,
                          KNL_LATCH_MODE_EXCLUSIVE,
                          aIsSuccess,
                          KNL_ENV(aEnv) ) == STL_SUCCESS );

    if( *aIsSuccess == STL_TRUE )
    {
        sState = 1;

        *aIsSuccess = (gSmrWarmupEntry->mBlockLogFlushing == 0) ? STL_TRUE : STL_FALSE;

        /**
         * Latch를 잡았을 경우 Log buffer flush가 enable이면 flush하고 종료
         */
        if( *aIsSuccess == STL_TRUE )
        {
            STL_TRY( smrFlushBufferInternal( aLogStream,
                                             aTargetSbsn,
                                             aEnv )
                     == STL_SUCCESS );
        }

        sState = 0;
        STL_TRY( knlReleaseLatch( &aLogStream->mFileLatch,
                                  KNL_ENV(aEnv) ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlReleaseLatch( &aLogStream->mFileLatch, KNL_ENV(aEnv) );
    }

    return STL_FAILURE;
}

stlStatus smrStopLogFlushing( smlEnv  * aEnv )
{
    stlBool          sIsTimedOut;
    smrLogStream   * sLogStream;
    stlUInt32        sState = 0;

    sLogStream = &gSmrWarmupEntry->mLogStream;

    STL_TRY( knlAcquireLatch( &sLogStream->mFileLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              KNL_ENV(aEnv) ) == STL_SUCCESS );
    sState = 1;

    gSmrWarmupEntry->mBlockLogFlushing++;

    sState = 0;
    STL_TRY( knlReleaseLatch( &sLogStream->mFileLatch,
                              KNL_ENV(aEnv) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlReleaseLatch( &sLogStream->mFileLatch, KNL_ENV(aEnv) );
    }

    return STL_FAILURE;
}

stlStatus smrStartLogFlushing( smlEnv  * aEnv )
{
    stlBool          sIsTimedOut;
    smrLogStream   * sLogStream;
    stlUInt32        sState = 0;

    sLogStream = &gSmrWarmupEntry->mLogStream;

    STL_TRY( knlAcquireLatch( &sLogStream->mFileLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              KNL_ENV(aEnv) ) == STL_SUCCESS );
    sState = 1;

    if( gSmrWarmupEntry->mBlockLogFlushing > 0 )
    {
        gSmrWarmupEntry->mBlockLogFlushing--;
    }

    sState = 0;
    STL_TRY( knlReleaseLatch( &sLogStream->mFileLatch,
                              KNL_ENV(aEnv) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlReleaseLatch( &sLogStream->mFileLatch, KNL_ENV(aEnv) );
    }

    return STL_FAILURE;
}

stlStatus smrFlushBufferInternal( smrLogStream * aLogStream,
                                  smrSbsn        aTargetSbsn,
                                  smlEnv       * aEnv )
{
    smrLogBuffer   * sLogBuffer;
    stlInt64         sMaxFlushLogBlockCount;
    smrSbsn          sTargetSbsn;

    sLogBuffer = aLogStream->mLogBuffer;

    sMaxFlushLogBlockCount =
        knlGetPropertyBigIntValueByID( KNL_PROPERTY_MAXIMUM_FLUSH_LOG_BLOCK_COUNT,
                                       KNL_ENV( aEnv ) );

    aLogStream->mStatistics.mFlushTryCount++;
    
    if( aTargetSbsn >= sLogBuffer->mRearSbsn )
    {
        /**
         * mRearSbsn를 No Latch로 읽기 때문에 동시성 문제로 인하여 가비지 로그가
         * 저장될수 있다.
         * 현재 mRearSbsn보다 하나 작은 SBSN까지 디스크에 저장한다.
         */
        aTargetSbsn = sLogBuffer->mRearSbsn - 1;
    }

    if( aTargetSbsn <= sLogBuffer->mFrontSbsn )
    {
        STL_THROW( RAMP_SUCCESS );
    }

    aLogStream->mStatistics.mFlushCount++;
    
    while( aTargetSbsn > sLogBuffer->mFrontSbsn )
    {
        sTargetSbsn = STL_MIN( sLogBuffer->mFrontSbsn + sMaxFlushLogBlockCount,
                               aTargetSbsn );
        
        if( SMR_BUFFER_POS( sTargetSbsn, sLogBuffer ) <= SMR_FRONT_POS(sLogBuffer) )
        {
            /*
             *                                      front
             *                                        |
             * dddddddd--------------------------------ddddddddd
             *        |
             *      target
             * 'd'로 표시된 부분이 flush 되어야 한다.
             */
            STL_TRY( smrWriteBufferBlocks( aLogStream,
                                           (sLogBuffer->mBufferBlockCount -
                                            (SMR_FRONT_POS(sLogBuffer) + 1)),
                                           aEnv ) == STL_SUCCESS );

            /*
             * Log flush를 수행하면서 log switching이 발생할 수 있고,
             * 만약, log switching에 의해 flush하려고 했던 log가 이미
             * flush되었을 경우 더이상 flush를 하지 않는다.
             */
            if( aTargetSbsn <= sLogBuffer->mFrontSbsn )
            {
                STL_THROW( RAMP_SUCCESS );
            }

            STL_TRY( smrWriteBufferBlocks( aLogStream,
                                           SMR_BUFFER_POS(sTargetSbsn, sLogBuffer) + 1,
                                           aEnv ) == STL_SUCCESS );
        }
        else
        {
            /*
             *       front
             *         |
             * ---------dddddddddddddddddddddddddddddd----------
             *                                       |
             *                                     target
             * 'd'로 표시된 부분이 flush 되어야 한다.
             */
            STL_TRY( smrWriteBufferBlocks( aLogStream,
                                           (SMR_BUFFER_POS( sTargetSbsn, sLogBuffer ) -
                                            SMR_FRONT_POS(sLogBuffer)),
                                           aEnv ) == STL_SUCCESS );
        }

        /**
         * @todo Memory Barrier가 필요하다.
         * - FrontSbsn을 빠르게 전파할 필요가 있다.
         */
    }

    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrWriteBufferBlocks( smrLogStream * aLogStream,
                                stlInt64       aFlushBlockCount,
                                smlEnv       * aEnv )
{
    smrLogBuffer   * sLogBuffer;
    stlInt32         sBlockSize;
    stlInt64         sFlushBytes = 0;
    stlChar        * sBlock;
    smrLogBlockHdr   sLogBlockHdr;
    void           * sTargetBuffer;
    smrSbsn          sLastSbsn;
    stlInt32         sState = 0;
    smrSbsn          sLogSwitchingSbsn;
    stlInt64         sFlushBlockCount;
    
    sLogBuffer = aLogStream->mLogBuffer;
    sBlockSize = aLogStream->mLogStreamPersData.mBlockSize;

    sLogSwitchingSbsn = aLogStream->mCurLogGroup4Disk->mLogSwitchingSbsn;
    sFlushBlockCount = aFlushBlockCount;
    
    /**
     * Flush하고자 하는 영역 내에 Log Swithcing SBSN이 존재하는 경우
     * Log Swithching이 발생한다.
     * flush할 buffer에 다수개의 switching sbsn이 존재할수 있다.
     */
    while( ((sLogBuffer->mFrontSbsn + 1) <= sLogSwitchingSbsn) &&
           ((sLogBuffer->mFrontSbsn + 1 + sFlushBlockCount) > sLogSwitchingSbsn) )
    {
        /*
         * Log file로 flush하려고 하는 마지막 Sbsn을 구한다.
         * 이는 log switching 후 flush해야 할 log가 남아있는지를 확인하기 위해서 필요하다.
         */
        sLastSbsn = sLogBuffer->mFrontSbsn + sFlushBlockCount;

        STL_TRY( smrMarkFileEnd( sLogBuffer,
                                 sLogSwitchingSbsn - 1,
                                 sBlockSize,
                                 aEnv )
                 == STL_SUCCESS );

        sFlushBytes = (sLogSwitchingSbsn - (sLogBuffer->mFrontSbsn + 1)) * sBlockSize;
        sTargetBuffer = sLogBuffer->mBuffer + (SMR_BUFFER_POS(sLogBuffer->mFrontSbsn + 1,
                                                              sLogBuffer)
                                               * sBlockSize);

        /**
         * Log Switching SBSN과 (Front SBSN + 1)이 동일하다면, 즉,
         * FILE_END가 설정된 block이 이미 flush되었다면, FILE_END를 설정한
         * Dummy block을 하나 flush한다.
         */
        if( sLogSwitchingSbsn == (sLogBuffer->mFrontSbsn + 1) )
        {
            STL_DASSERT( sFlushBytes == 0 );
            
            STL_TRY( smrWriteDummyFileEndBlock( aLogStream,
                                                aEnv ) == STL_SUCCESS );
        }
        else
        {
            STL_TRY( smrWriteLogMirrorBuffer( sTargetBuffer,
                                              sFlushBytes,
                                              STL_FALSE,
                                              aEnv ) == STL_SUCCESS );
            
            STL_TRY( smrWriteFileBlocks( aLogStream,
                                         sTargetBuffer,
                                         sFlushBytes,
                                         STL_FALSE, /* aDoValidate */
                                         NULL, /* aIsValidate */
                                         NULL, /* aLogger */
                                         aEnv ) == STL_SUCCESS );
            
            STL_TRY( smrCheckLogMirrorBuffer( aEnv ) == STL_SUCCESS );
        }

        sLogBuffer->mFrontSbsn += (sFlushBytes / sBlockSize);
        
        sBlock = sLogBuffer->mBuffer + (SMR_FRONT_POS(sLogBuffer) * sBlockSize);
        SMR_READ_LOG_BLOCK_HDR( &sLogBlockHdr, sBlock );

        /**
         * chained log가 존재하는 block이라면 이전 lsn을 stable lsn으로 설정한다.
         */
        if( SMR_IS_CHAINED_BLOCK( &sLogBlockHdr ) == STL_TRUE )
        {
            sLogBuffer->mFrontLsn = sLogBlockHdr.mLsn - 1;
        }
        else
        {
            sLogBuffer->mFrontLsn = sLogBlockHdr.mLsn;
        }

        KNL_LEAVE_NON_CANCELLABLE_SECTION( aEnv );
        sState = 1;
        
        STL_TRY( smrLogSwitching( aLogStream,
                                  STL_TRUE,   /* aFlushLog */
                                  STL_TRUE,   /* aFileSwitching */
                                  STL_TRUE,   /* aDoCheckpoint */
                                  aEnv ) == STL_SUCCESS );
        
        sState = 0;
        KNL_ENTER_NON_CANCELLABLE_SECTION( aEnv );
        
        /*
         * Log가 switch되면서 checkpoint가 수행되고, 이때 dirty page를 flush하기 위해
         * log를 flush하기 때문에, 처음에 flush하려고 했던 log가 이미 flush되었을 경우
         * 더이상 flush를 하지 말아야 한다.
         */
        if( sLastSbsn <= sLogBuffer->mFrontSbsn )
        {
            STL_THROW( RAMP_SUCCESS );
        }

        /**
         * next group의 switching sbsn을 검사한다.
         */
        sLogSwitchingSbsn = aLogStream->mCurLogGroup4Disk->mLogSwitchingSbsn;
        sFlushBlockCount = sLastSbsn - sLogBuffer->mFrontSbsn;
    }

    sFlushBytes = (sFlushBlockCount * sBlockSize);
    sTargetBuffer = sLogBuffer->mBuffer + (SMR_BUFFER_POS(sLogBuffer->mFrontSbsn + 1,
                                                          sLogBuffer)
                                           * sBlockSize);

    if( sFlushBytes > 0 )
    {
        STL_TRY( smrWriteLogMirrorBuffer( sTargetBuffer,
                                          sFlushBytes,
                                          STL_FALSE,
                                          aEnv ) == STL_SUCCESS );
    
        STL_TRY( smrWriteFileBlocks( aLogStream,
                                     sTargetBuffer,
                                     sFlushBytes,
                                     STL_FALSE, /* aDoValidate */
                                     NULL, /* aIsValidate */
                                     NULL, /* aLogger */
                                     aEnv ) == STL_SUCCESS );
    
        STL_TRY( smrCheckLogMirrorBuffer( aEnv ) == STL_SUCCESS );
    }
    
    sLogBuffer->mFrontSbsn += (sFlushBytes / sBlockSize);
    sLogBuffer->mFrontFileBlockSeqNo += (sFlushBytes / sBlockSize);

    sBlock = sLogBuffer->mBuffer + (SMR_FRONT_POS(sLogBuffer) * sBlockSize);
    SMR_READ_LOG_BLOCK_HDR( &sLogBlockHdr, sBlock );
    
    /**
     * chained log가 존재하는 block이라면 이전 lsn을 stable lsn으로 설정한다.
     */
    if( SMR_IS_CHAINED_BLOCK( &sLogBlockHdr ) == STL_TRUE )
    {
        sLogBuffer->mFrontLsn = sLogBlockHdr.mLsn - 1;
    }
    else
    {
        sLogBuffer->mFrontLsn = sLogBlockHdr.mLsn;
    }

    STL_DASSERT( sLogBuffer->mFrontSbsn < sLogBuffer->mRearSbsn );

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        KNL_ENTER_NON_CANCELLABLE_SECTION( aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief Log file에 dummy FILE_END block을 flush한다.
 * @param[in] aLogStream Log stream
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smrWriteDummyFileEndBlock( smrLogStream * aLogStream,
                                     smlEnv       * aEnv )
{
    smrLogGroup    * sLogGroup;
    stlChar          sBuffer[SMP_PAGE_SIZE];
    stlChar        * sLogBlock;
    smrLogBlockHdr   sFileEndBlock;
    stlInt16         sBlockSize;

    sBlockSize = aLogStream->mLogStreamPersData.mBlockSize;
    sLogGroup  = aLogStream->mCurLogGroup4Disk;

    /**
     * 만약 file의 물리적인 마지막 block까지 logging이 되었다면
     * dummy FILE_END block은 기록하지 않는다.
     */
    if( sLogGroup->mLogGroupPersData.mFileSize >
        SMR_BLOCK_SEQ_FILE_OFFSET(sLogGroup->mCurBlockSeq, sBlockSize) )
    {
        sLogBlock = (stlChar *)STL_ALIGN( (stlUInt64)sBuffer, sBlockSize );

        /* to remove valgrind warning */
        stlMemset( sLogBlock, 0x00, sBlockSize );

        sFileEndBlock.mLsn = SMR_INVALID_LSN;
        sFileEndBlock.mLogCount = 0;
        SMR_SET_BLOCK_INFO( &sFileEndBlock,
                            SMR_FILE_END_TRUE,
                            SMR_CHAINED_BLOCK_FALSE,
                            SMR_INVALID_WRAP_NO,
                            SMR_BLOCK_WRITE_FINISH );
        sFileEndBlock.mFirstLogOffset = 0;
        SMR_WRITE_LOG_BLOCK_HDR( sLogBlock, &sFileEndBlock );

        STL_TRY( smrWriteLogMirrorBuffer( sLogBlock,
                                          sBlockSize,
                                          STL_FALSE,
                                          aEnv ) == STL_SUCCESS );
            
        STL_TRY( smrWriteFileBlocks( aLogStream,
                                     sLogBlock,
                                     sBlockSize,
                                     STL_FALSE, /* aDoValidate */
                                     NULL, /* aIsValid */
                                     NULL, /* aLogger */
                                     aEnv ) == STL_SUCCESS );
            
        STL_TRY( smrCheckLogMirrorBuffer( aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrWakeupFlusher( smlEnv * aEnv )
{
    stlUInt32   sEnvId;
    knlEnv    * sLogFlusherEnv;
    
    sEnvId = SML_LOG_FLUSHER_ENV_ID;
    sLogFlusherEnv = knlGetEnv( sEnvId );

    STL_DASSERT( sLogFlusherEnv->mEvent.mEventListener != NULL );

    STL_TRY( knlWakeupEventListener( sLogFlusherEnv->mEvent.mEventListener,
                                     KNL_ENV(aEnv) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrWaitFlusher( smrLogStream * aLogStream,
                          smrLsn         aFlushLsn,
                          smrSbsn        aFlushSbsn,
                          stlInt64       aWriteMode,
                          smlEnv       * aEnv )
{
    smrLogBuffer   * sLogBuffer;
    stlBool          sIsTimedOut;
    knlLatchWaiter * sWaiter;
    stlInterval      sInterval = 10;

    sLogBuffer = aLogStream->mLogBuffer;

    if( smrIsLogFlusherActive() == STL_TRUE )
    {
        STL_TRY_THROW( aWriteMode == SML_TRANSACTION_CWM_WAIT, RAMP_SUCCESS );

        sWaiter = KNL_WAITITEM(aEnv);
        sWaiter->mForLogFlush = STL_TRUE;
    
        STL_TRY( smrWakeupFlusher( aEnv ) == STL_SUCCESS );

        while( 1 )
        {
            if( sLogBuffer->mFrontSbsn >= aFlushSbsn )
            {
                break;
            }

            /**
             * chained log 를 고려한다.
             */
            if( sLogBuffer->mFrontLsn >= aFlushLsn )
            {
                break;
            }

            sWaiter->mFlushSbsn = aFlushSbsn;
            sWaiter->mFlushLsn = aFlushLsn;

            sInterval = STL_MIN( STL_TIMESLICE_TIME, sInterval );
            
            STL_TRY( knlCondWait( &aLogStream->mCondVar,
                                  sInterval,
                                  &sIsTimedOut,
                                  KNL_ENV( aEnv ) ) == STL_SUCCESS );

            sInterval = sInterval * 10;
        }
        
        sWaiter->mForLogFlush = STL_FALSE;
    }
    else
    {
        STL_TRY( smrFlushBuffer( aLogStream,
                                 aFlushSbsn,
                                 aEnv ) == STL_SUCCESS );
    }

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrWriteLogToBlock( smrLogStream       * aLogStream,
                              smxmLogBlockCursor * aLogBlockCursor,
                              smrLogHdr          * aLogHdr,
                              smrLsn               aLsn,
                              smrLogDataType       aLogDataType,
                              void               * aLogData,
                              stlInt32             aLogSize,
                              stlInt32           * aWrittenBytes,
                              smrLid             * aWrittenLid,
                              smrSbsn            * aWrittenSbsn,
                              smlEnv             * aEnv )
{
    smrLogBuffer   * sLogBuffer;
    stlChar        * sBlock;
    stlInt32         sBlockSize;
    smrLogBlockHdr   sLogBlockHdr;
    stlInt32         sWriteBytes;
    stlUInt16        sBlockOffset;

    sLogBuffer = aLogStream->mLogBuffer;
    sBlockSize = aLogStream->mLogStreamPersData.mBlockSize;

    /**
     * @note Partial Logging Check: Log Block Hdr를 쓰기 전에 BlockInfo에 write on
     * 마지막에 write off를 설정한다.
     */
    
    STL_DASSERT( smrValidateLogBuffer( sLogBuffer,
                                       sBlockSize )
                 == STL_TRUE );
    
    STL_ASSERT( sLogBuffer->mRearFileBlockSeqNo <
                smrGetMaxFileBlockCount(aLogStream, aLogStream->mCurLogGroup4Buffer) );

    /**
     * Block 스위치 판단 후 Block을 얻는다.
     */ 
    if( (sLogBuffer->mBlockOffset + SMR_LOG_HDR_SIZE) > sBlockSize )
    {
        SMR_BUFFER_BLOCK_SWITCH( sLogBuffer );
    }

    sBlock = sLogBuffer->mBuffer + (SMR_REAR_POS(sLogBuffer) * sBlockSize);

    if( sLogBuffer->mBlockOffset == SMR_BLOCK_HDR_SIZE )
    {
        sLogBlockHdr.mLsn = aLsn;
        sLogBlockHdr.mLogCount = 0;
        sLogBlockHdr.mFirstLogOffset = 0;
        SMR_SET_BLOCK_INFO( &sLogBlockHdr,
                            SMR_FILE_END_FALSE,
                            SMR_CHAINED_BLOCK_FALSE,
                            SMR_WRAP_NO( sLogBuffer->mFileSeqNo ),
                            SMR_BLOCK_WRITE_FINISH );

        stlMemcpy( &gSmrWarmupEntry->mLogBlockHdr,
                   &sLogBlockHdr,
                   STL_SIZEOF( smrLogBlockHdr ) );

        SMR_SET_BLOCK_WRITE_START( &sLogBlockHdr );
        
        SMR_WRITE_LOG_BLOCK_HDR( sBlock, &sLogBlockHdr );
    }
    else
    {
        SMR_READ_LOG_BLOCK_HDR( &sLogBlockHdr, sBlock );

        stlMemcpy( &gSmrWarmupEntry->mLogBlockHdr,
                   &sLogBlockHdr,
                   STL_SIZEOF( smrLogBlockHdr ) );
        
        SMR_SET_BLOCK_WRITE_START( &sLogBlockHdr );
        
        SMR_WRITE_BLOCK_INFO( sBlock, &sLogBlockHdr.mBlockInfo );
    }
    
    /**
     * aLogHdr가 NULL이 아니라면 Log를 처음 쓰는 것이고, NULL이라면 Log가 Chained 되었다.
     */ 
    if( aLogHdr != NULL )
    {
        if( aWrittenLid != NULL )
        {
            SMR_MAKE_LID( aWrittenLid,
                          sLogBuffer->mFileSeqNo,
                          sLogBuffer->mRearFileBlockSeqNo,
                          sLogBuffer->mBlockOffset );
        }

        if( sLogBlockHdr.mLogCount == 0 )
        {
            sLogBlockHdr.mFirstLogOffset = sLogBuffer->mBlockOffset;
        }
        sLogBlockHdr.mLogCount++;
        sLogBlockHdr.mLsn = aLsn;

        STL_ASSERT( sLogBuffer->mBlockOffset >= sLogBlockHdr.mFirstLogOffset );
        
        SMR_WRITE_LOG_HDR( sBlock + sLogBuffer->mBlockOffset, aLogHdr );
        sLogBuffer->mBlockOffset += SMR_LOG_HDR_SIZE;
    }
    else
    {
        sLogBlockHdr.mLsn = aLsn;
    }

    if( aLogSize > (sBlockSize - sLogBuffer->mBlockOffset) )
    {
        sWriteBytes = sBlockSize - sLogBuffer->mBlockOffset;

        /**
         * chained block 정보 설정
         */
        SMR_SET_CHAINED_BLOCK( &sLogBlockHdr );
    }
    else
    {
        sWriteBytes = aLogSize;
    }

    /**
     * Log Body를 Log Block에 기록
     */  
    if( aLogDataType == SMR_LOGDATA_TYPE_BLOCKARRAY )
    {
        STL_TRY( smxmCopyLogFromBlock( aLogBlockCursor,
                                       aLogData,
                                       sBlock + sLogBuffer->mBlockOffset,
                                       sWriteBytes,
                                       aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_ASSERT( aLogDataType == SMR_LOGDATA_TYPE_BINARY );
        stlMemcpy( sBlock + sLogBuffer->mBlockOffset,
                   aLogData,
                   sWriteBytes );
    }

    if( aWrittenSbsn != NULL )
    {
        *aWrittenSbsn = sLogBuffer->mRearSbsn;
    }

    *aWrittenBytes = sWriteBytes;
    sLogBuffer->mBlockOffset += sWriteBytes;

    /**
     * @note Log Block Header를 마지막에 기록한다.
     */
    SMR_WRITE_LOG_BLOCK_HDR( sBlock, &sLogBlockHdr );

    if( SMR_GET_LOG_COUNT( sBlock ) == 0 )
    {
        sBlockOffset = sLogBuffer->mBlockOffset;
        SMR_WRITE_FIRST_OFFSET( sBlock, sBlockOffset );
        STL_DASSERT( sBlockOffset > 0 );
    }

    SMR_SET_BLOCK_WRITE_FINISH( &sLogBlockHdr );

    SMR_WRITE_BLOCK_INFO( sBlock, &sLogBlockHdr.mBlockInfo );

    STL_DASSERT( smrValidateLogBuffer( sLogBuffer,
                                       sBlockSize )
                 == STL_TRUE );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrMarkFileEnd( smrLogBuffer * aLogBuffer,
                          smrSbsn        aSbsn,
                          stlInt32       aBlockSize,
                          smlEnv       * aEnv )
{
    stlChar  * sBlock;
    stlInt16   sBlockInfo;
    
    sBlock = aLogBuffer->mBuffer + (SMR_BUFFER_POS(aSbsn, aLogBuffer) * aBlockSize);

    SMR_READ_BLOCK_INFO( sBlock, &sBlockInfo );

    sBlockInfo = sBlockInfo | SMR_FILE_END_TRUE | SMR_CHAINED_BLOCK_FALSE;

    SMR_WRITE_BLOCK_INFO( sBlock, &sBlockInfo );

    return STL_SUCCESS;
}

stlStatus smrLogSwitching( smrLogStream * aLogStream,
                           stlBool        aFlushLog,
                           stlBool        aFileSwitching,
                           stlBool        aDoCheckpoint,
                           smlEnv       * aEnv )
{
    smrLogBuffer * sLogBuffer;
    smrLsn         sCheckpointLsn;
    smrLogGroup  * sLogGroup;
    stlInt64       sLogFileSeq;
    smrChkptArg    sArg;

    sLogBuffer = aLogStream->mLogBuffer;
    sLogBuffer->mFrontFileBlockSeqNo = 0;

    /**
     * File Switching을 원한다면
     */
    if( aFileSwitching == STL_TRUE )
    {
        sLogGroup = aLogStream->mCurLogGroup4Disk;
        sLogFileSeq = sLogGroup->mLogGroupPersData.mFileSeqNo;

        STL_TRY( smrLogFileSwitching( aLogStream,
                                      sLogBuffer->mFrontLsn,
                                      aEnv ) == STL_SUCCESS );

        aLogStream->mCurLogGroup4Disk = smrGetNextLogGroup( aLogStream,
                                                            sLogGroup );
        
        /**
         * Checkpointer가 active이면 checkpoint event를 발생시켜서
         * ACTIVE 상태의 log file이 archiving될 수 있도록 한다.
         */
        if( (aDoCheckpoint == STL_TRUE) && (smrIsCheckpointerActive() == STL_TRUE) )
        {
            stlMemset( (void *)&sArg, 0x00, STL_SIZEOF(smrChkptArg) );
            sArg.mLogfileSeq = sLogFileSeq;
            sArg.mFlushBehavior = SMP_FLUSH_LOGGED_PAGE;

            STL_TRY( knlAddEnvEvent( SML_EVENT_CHECKPOINT,
                                     NULL,            /* aEventMem */
                                     (void*)&sArg,
                                     STL_SIZEOF( smrChkptArg ),
                                     SML_CHECKPOINT_ENV_ID,
                                     KNL_EVENT_WAIT_NO,
                                     STL_TRUE, /* aForceSuccess */
                                     KNL_ENV( aEnv ) ) == STL_SUCCESS );
        }
    }
    else
    {
        aLogStream->mCurLogGroup4Disk = smrGetNextLogGroup( aLogStream,
                                                            aLogStream->mCurLogGroup4Disk );
    }
    
    /**
     * Log Flushing을 원한다면
     */
    if( aFlushLog == STL_TRUE )
    {
        /**
         * Checkpointer가 active가 아니면 log file을 직접 archiving한다.
         */
        if( smrIsCheckpointerActive() != STL_TRUE )
        {
            /**
             * Log file switching 시 CURRENT ACTIVE log file의 마지막 lsn을
             * 이용하여 checkpoint event 발생
             */
            
            sCheckpointLsn = sLogBuffer->mFrontLsn;
            
            /**
             * CURRENT ACTIVE log file의 마지막 lsn까지의 log와 dirty page를 flush한다.
             * @todo system thread들이 떠있지 않은 상태이기 때문에 checkpoint를 직접 수행해야 한다.
             *       todo가 추가 되면 ASSERT 제거
             */

            STL_ASSERT( STL_FALSE );
            
            /**
             * Flush된 dirty page의 max lsn으로 이전 log file을 archiving한다.
             */
            STL_TRY( smrLogArchiving( sCheckpointLsn,
                                      aEnv ) == STL_SUCCESS );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrTryDisableLogging( smrLogStream * aLogStream,
                                smrLogGroup  * aLogGroup,
                                smlEnv       * aEnv )
{
    smrLogGroup  * sLogGroup;
    stlInt32       i;
    
    sLogGroup = aLogGroup;

    for( i = 0; i <= SMR_REVERSED_LOG_FILE_GAP; i++ )
    {
        sLogGroup = smrGetNextLogGroup( aLogStream, sLogGroup );

        /**
         * log flusher가 이미 Log Group을 증가 시켰을수도 있다.
         */
        if( sLogGroup->mLogGroupPersData.mState == SMR_LOG_GROUP_STATE_CURRENT )
        {
            continue;
        }
        
        if( SMR_IS_USABLE_LOG_FILE( sLogGroup ) == STL_FALSE )
        {
            gSmrWarmupEntry->mBlockedLfsn = sLogGroup->mLogGroupPersData.mFileSeqNo;

            /**
             * Archiever가 No Latch로 확인하기 때문에 Barrier가 필요하다.
             */
            stlMemBarrier();
        
            STL_TRY( knlLogMsg( NULL,
                                KNL_ENV(aEnv),
                                KNL_LOG_LEVEL_INFO,
                                "[LOG FLUSHER] disable logging - blocked lfsn(%ld)\n",
                                gSmrWarmupEntry->mBlockedLfsn )
                     == STL_SUCCESS );
        
            gSmrWarmupEntry->mEnableLogging = STL_FALSE;
            aLogStream->mStatistics.mBlockedLoggingCount++;
            break;
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrTryEnableLogging( smrLogStream * aLogStream,
                               smlEnv       * aEnv )
{
    if( gSmrWarmupEntry->mEnableLogging == STL_FALSE )
    {
        if( gSmrWarmupEntry->mBlockedLfsn <= gSmrWarmupEntry->mLogPersData.mLastInactiveLfsn )
        {
            gSmrWarmupEntry->mEnableLogging = STL_TRUE;
            
            STL_TRY( knlLogMsg( NULL,
                                KNL_ENV(aEnv),
                                KNL_LOG_LEVEL_INFO,
                                "[ARCHIVING] enable logging - blocked lfsn(%ld), inactivated lfsn(%ld)\n",
                                gSmrWarmupEntry->mBlockedLfsn,
                                gSmrWarmupEntry->mLogPersData.mLastInactiveLfsn )
                     == STL_SUCCESS );
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrReadLogFromBlock( smrLogStream * aLogStream,
                               stlBool        aReadLogHdr,
                               stlBool      * aEndOfLog,
                               smlEnv       * aEnv )
{
    stlInt64         sReadBufferBytes;
    smrLogBuffer   * sLogBuffer;
    smrLogBlockHdr   sLogBlockHdr;
    stlChar        * sBlock;
    smrLid           sLid;
    stlInt32         sBlockSize;
    void           * sBuffer;
    stlInt64         sBufferSize;
    
    *aEndOfLog = STL_FALSE;
    
    sLogBuffer = aLogStream->mLogBuffer;
    sBlockSize = aLogStream->mLogStreamPersData.mBlockSize;

    while( 1 )
    {
        /*
         * Buffer가 Empty라면 Disk로부터 로그를 읽는다.
         */
        if( sLogBuffer->mRearSbsn == (sLogBuffer->mFrontSbsn + 1) )
        {
            if( sLogBuffer->mEndOfLog == STL_TRUE )
            {
                sLogBuffer->mLeftLogCountInBlock = 0;
                sLogBuffer->mBlockOffset = SMR_BLOCK_HDR_SIZE;
                *aEndOfLog = STL_TRUE;

                STL_THROW( RAMP_SUCCESS );
            }

            if( sLogBuffer->mEndOfFile == STL_TRUE )
            {
                STL_TRY( smrLogSwitching( aLogStream,
                                          STL_FALSE,  /* aFlushLog */
                                          STL_FALSE,  /* aFileSwitching */
                                          STL_FALSE,  /* aDoCheckpoint */
                                          aEnv ) == STL_SUCCESS );

                sLogBuffer->mFileSeqNo += 1;
                sLogBuffer->mLeftLogCountInBlock = 0;
                sLogBuffer->mBlockOffset = SMR_BLOCK_HDR_SIZE;
                sLogBuffer->mRearFileBlockSeqNo = 0;
                sLogBuffer->mFrontFileBlockSeqNo = -1;
            }
            
            SMR_MAKE_LID( &sLid,
                          sLogBuffer->mFileSeqNo,
                          sLogBuffer->mFrontFileBlockSeqNo + 1,
                          0 );

            sBuffer = sLogBuffer->mBuffer + (SMR_REAR_POS(sLogBuffer) * sBlockSize);
            sBufferSize = sLogBuffer->mBufferSize - (SMR_REAR_POS(sLogBuffer) * sBlockSize);
            
            STL_TRY( smrReadFileBlocks( &sLid,
                                        sBuffer,
                                        sBufferSize,
                                        &sReadBufferBytes,
                                        &sLogBuffer->mEndOfFile,
                                        &sLogBuffer->mEndOfLog,
                                        aEnv ) == STL_SUCCESS );

            /*
             * 적어도 하나의 로그 블록을 읽었다면
             */
            if( sReadBufferBytes != 0 )
            {
                sLogBuffer->mRearFileBlockSeqNo = (sLogBuffer->mFrontFileBlockSeqNo +
                                                   (sReadBufferBytes / sBlockSize));
                STL_ASSERT( sLogBuffer->mRearFileBlockSeqNo <=
                            smrGetMaxFileBlockCount( aLogStream,
                                                     aLogStream->mCurLogGroup4Buffer ) );
                sLogBuffer->mRearSbsn += (sReadBufferBytes / sBlockSize);
                break;
            }
        }
        else
        {
            break;
        }
    }

    sLogBuffer->mFrontFileBlockSeqNo++;
    sLogBuffer->mFrontSbsn++;
    sBlock = sLogBuffer->mBuffer + (SMR_FRONT_POS(sLogBuffer) * sBlockSize);

    SMR_READ_LOG_BLOCK_HDR( &sLogBlockHdr, sBlock );

    sLogBuffer->mFrontLsn = sLogBlockHdr.mLsn;
    sLogBuffer->mLeftLogCountInBlock = sLogBlockHdr.mLogCount;

    if( aReadLogHdr == STL_TRUE )
    {
        sLogBuffer->mBlockOffset = sLogBlockHdr.mFirstLogOffset;
    }
    else
    {
        sLogBuffer->mBlockOffset = SMR_BLOCK_HDR_SIZE;
    }
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrReadStreamLog( smrLogStream * aLogStream,
                            smrLogHdr    * aLogHdr,
                            stlChar      * aLogBody,
                            stlBool      * aEndOfLog,
                            smlEnv       * aEnv )
{
    smrLogBuffer * sLogBuffer;
    stlInt32       sTotalReadBytes = 0;
    stlInt32       sReadBytes;
    stlChar      * sBlock;
    stlInt16       sBlockSize;
    smrLogBlockHdr sLogBlockHdr;

    *aEndOfLog = STL_FALSE;

    sLogBuffer = aLogStream->mLogBuffer;
    sBlockSize = aLogStream->mLogStreamPersData.mBlockSize;

    STL_RAMP( RAMP_RETRY );

    sTotalReadBytes = 0;
    
    *aEndOfLog = STL_FALSE;
    
    if( sLogBuffer->mLeftLogCountInBlock == 0 )
    {
        STL_TRY( smrReadLogFromBlock( aLogStream,
                                      STL_TRUE,
                                      aEndOfLog,
                                      aEnv ) == STL_SUCCESS );
        STL_TRY_THROW( *aEndOfLog == STL_FALSE, RAMP_SUCCESS );
    }

    SMR_MAKE_LID( &sLogBuffer->mRearLid,
                  sLogBuffer->mFileSeqNo,
                  sLogBuffer->mFrontFileBlockSeqNo,
                  sLogBuffer->mBlockOffset );
    
    sBlock = sLogBuffer->mBuffer + (SMR_FRONT_POS(sLogBuffer) * sBlockSize);
    SMR_READ_LOG_HDR( aLogHdr, sBlock + sLogBuffer->mBlockOffset );
    sLogBuffer->mBlockOffset += SMR_LOG_HDR_SIZE;
    sLogBuffer->mRearLsn = aLogHdr->mLsn;

    if( aLogHdr->mSize > (sBlockSize - sLogBuffer->mBlockOffset) )
    {
        sReadBytes = sBlockSize - sLogBuffer->mBlockOffset;
    }
    else
    {
        sReadBytes = aLogHdr->mSize;
    }

    stlMemcpy( aLogBody, sBlock + sLogBuffer->mBlockOffset, sReadBytes );
    sTotalReadBytes += sReadBytes;
    sLogBuffer->mBlockOffset += sReadBytes;
    sLogBuffer->mLeftLogCountInBlock--;
    
    while( sTotalReadBytes < aLogHdr->mSize )
    {
        STL_TRY( smrReadLogFromBlock( aLogStream,
                                      STL_FALSE,
                                      aEndOfLog,
                                      aEnv ) == STL_SUCCESS );

        /**
         * Partial log detected
         */
        if( *aEndOfLog == STL_TRUE )
        {
            sLogBuffer->mPartialLogDetected = STL_TRUE;
            STL_THROW( RAMP_SUCCESS );
        }

        sBlock = sLogBuffer->mBuffer + (SMR_FRONT_POS(sLogBuffer) * sBlockSize);
        SMR_READ_LOG_BLOCK_HDR( &sLogBlockHdr, sBlock );

        /*
         * Block Chained Log의 partial log detection은 중간 Block의 First Log Offset이
         * SMR_BLOCK_HDR_SIZE인 것으로 판단할수 있다.
         * 이러한 경우는 Media Recovery시에 발생할수 있으며 Skip한다.
         */
        if( sLogBlockHdr.mFirstLogOffset == SMR_BLOCK_HDR_SIZE )
        {
            STL_THROW( RAMP_RETRY );
        }
        
        if( (aLogHdr->mSize - sTotalReadBytes) >
            (sBlockSize - sLogBuffer->mBlockOffset) )
        {
            sReadBytes = (sBlockSize - sLogBuffer->mBlockOffset);
        }
        else
        {
            sReadBytes = (aLogHdr->mSize - sTotalReadBytes);
        }

        stlMemcpy( aLogBody + sTotalReadBytes,
                   sBlock + sLogBuffer->mBlockOffset,
                   sReadBytes );
        
        sTotalReadBytes += sReadBytes;
        sLogBuffer->mBlockOffset += sReadBytes;
    }

    STL_ASSERT( sLogBuffer->mBlockOffset <= sBlockSize );
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrReadChkptLog( smrLid * aChkptLid,
                           smrLsn   aChkptLsn,
                           smrLsn * aOldestLsn,
                           smlScn * aSystemScn,
                           smlEnv * aEnv )
{
    stlChar      * sBuffer;
    stlChar      * sBlockBuffer;
    stlChar        sLogBody[SMR_MAX_LOGPIECE_SIZE];
    smrLogHdr      sLogHdr;
    stlInt16       sBlockSize;
    stlInt16       sBlockOffset = 0;
    stlInt32       sBodyOffset = 0;
    stlInt32       sTotalReadBytes = 0;
    stlInt32       sReadBytes = 0;
    smrLogStream * sLogStream;
    smrLogGroup  * sLogGroup;
    smrLogPieceHdr sLogPieceHdr;
    stlInt64       sReadBufferBytes;
    stlBool        sEndOfFile;
    stlBool        sEndOfLog;
    smrLogBlockHdr sLogBlockHdr;
    knlRegionMark  sMemMark;
    stlInt32       sState = 0;

    sLogStream = &gSmrWarmupEntry->mLogStream;
    sBlockSize = sLogStream->mLogStreamPersData.mBlockSize;

    KNL_INIT_REGION_MARK( &sMemMark );

    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               (knlEnv*)aEnv )
             == STL_SUCCESS );

    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                SMR_MAX_LOGPIECE_SIZE * 2,
                                (void**)&sBuffer,
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 1;

    sBlockBuffer = (stlChar*)STL_ALIGN( (stlUInt64)sBuffer,
                                        sBlockSize );

    /**
     * Checkpoint Lid에 해당하는 log가 online log group에 존재하지 않는 경우,
     * Controlfile이 최신이 아니기 때문에 media recovery를 수행해야 한다.
     */
    STL_TRY( smrFindLogGroupBySeqNo( sLogStream,
                                     SMR_FILE_SEQ_NO( aChkptLid ),
                                     &sLogGroup,
                                     aEnv ) == STL_SUCCESS );

    STL_TRY_THROW( sLogGroup != NULL, RAMP_ERR_OLD_CONTROL_FILE );

    STL_TRY( smrReadFileBlocksInternal( aChkptLid,
                                        sLogGroup,
                                        sBlockBuffer,
                                        SMR_MAX_LOGPIECE_SIZE,
                                        &sReadBufferBytes,
                                        &sEndOfFile,
                                        &sEndOfLog,
                                        aEnv ) == STL_SUCCESS );

    /**
     * Checkpoint Lid에 유효한 log가 없으면 media recovery를 수행해야 한다.
     */
    STL_TRY_THROW( sReadBufferBytes > 0, RAMP_ERR_OLD_CONTROL_FILE );

    SMR_READ_LOG_HDR( &sLogHdr, sBlockBuffer + SMR_BLOCK_OFFSET( aChkptLid ) );

    /**
     * Checkpoint Lid에서 읽은 log의 Lsn이 checkpoint Lsn이 아닌 경우,
     * Controlfile이 최신이 아니기 때문에 media recovery를 수행해야 한다.
     */
    STL_TRY_THROW( sLogHdr.mLsn == aChkptLsn, RAMP_ERR_OLD_CONTROL_FILE );
    
    sBlockOffset = SMR_LOG_HDR_SIZE + SMR_BLOCK_OFFSET( aChkptLid );
    SMR_READ_LOG_BLOCK_HDR( &sLogBlockHdr, sBlockBuffer );

    STL_DASSERT( SMR_IS_VALID_LOG_LSN( sLogBlockHdr.mLsn ) );

    STL_ASSERT( sLogBlockHdr.mFirstLogOffset <= SMR_BLOCK_OFFSET( aChkptLid ) );

    sTotalReadBytes = 0;

    /*
     * Checkpoint Log Body를 구성한다.
     */
    while( sTotalReadBytes < sLogHdr.mSize )
    {
        if( sBlockOffset >= sLogStream->mLogStreamPersData.mBlockSize )
        {
            sBlockOffset = SMR_BLOCK_HDR_SIZE;
            sBlockBuffer += sLogStream->mLogStreamPersData.mBlockSize;
        }

        if( (sLogHdr.mSize - sTotalReadBytes) >
            (sLogStream->mLogStreamPersData.mBlockSize - sBlockOffset) )
        {
            sReadBytes = sLogStream->mLogStreamPersData.mBlockSize - sBlockOffset;
        }
        else
        {
            sReadBytes = (sLogHdr.mSize - sTotalReadBytes);
        }
        
        stlMemcpy( sLogBody + sTotalReadBytes, sBlockBuffer + sBlockOffset, sReadBytes );
        sBlockOffset += sReadBytes;
        sTotalReadBytes += sReadBytes;
    }

    /*
     * Checkpoint Log Body를 읽는다.
     */
    SMR_READ_MOVE_LOGPIECE_HDR( &sLogPieceHdr, sLogBody, sBodyOffset );
    STL_READ_MOVE_INT64( aOldestLsn, sLogBody, sBodyOffset );
    STL_READ_MOVE_INT64( aSystemScn, sLogBody, sBodyOffset );

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       (knlEnv*)aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_OLD_CONTROL_FILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_OLD_CONTROL_FILE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    if( sState == 1 )
    {
        (void) knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                         &sMemMark,
                                         STL_FALSE, /* aFreeChunk */
                                         (knlEnv*)aEnv );
    }

    return STL_FAILURE;
}

stlStatus smrFindLogInBlock( stlChar   * aBlock,
                             smrLsn      aTargetLsn,
                             stlInt16  * aBlockOffset,
                             smrLsn    * aFoundLsn,
                             stlInt32  * aLeftLogCount,
                             smlEnv    * aEnv )
{
    smrLogHdr      sLogHdr;
    smrLogBlockHdr sLogBlockHdr;
    stlInt16       sBlockOffset;
    stlInt32       sLeftLogCount;
    
    *aBlockOffset = 0;
    *aFoundLsn = 0;
    *aLeftLogCount = 0;
    
    SMR_READ_LOG_BLOCK_HDR( &sLogBlockHdr, aBlock );

    STL_DASSERT( SMR_IS_VALID_LOG_LSN( sLogBlockHdr.mLsn ) );

    sBlockOffset = sLogBlockHdr.mFirstLogOffset;
    sLeftLogCount = sLogBlockHdr.mLogCount;

    STL_DASSERT( sLeftLogCount > 0 );

    while( sLeftLogCount > 0 )
    {
        SMR_READ_LOG_HDR( &sLogHdr, aBlock + sBlockOffset );
        
        *aFoundLsn = sLogHdr.mLsn;
        *aBlockOffset = sBlockOffset;
        *aLeftLogCount = sLeftLogCount;
        
        if( sLogHdr.mLsn >= aTargetLsn )
        {
            break;
        }

        sLeftLogCount--;
        sBlockOffset += (SMR_LOG_HDR_SIZE + sLogHdr.mSize);
    }

    return STL_SUCCESS;
}

stlStatus smrFormatLogBuffer( void     * aBuffer,
                              stlInt64   aBufferSize,
                              stlInt16   aBlockSize,
                              smlEnv   * aEnv )
{
    smrLogBlockHdr sLogBlockHdr;
    stlInt64       i;

    stlMemset( aBuffer, 0x00, aBufferSize );

    sLogBlockHdr.mLsn = SMR_INIT_LSN;
    sLogBlockHdr.mLogCount = 0;
    sLogBlockHdr.mFirstLogOffset = 0;
    SMR_SET_BLOCK_INFO( &sLogBlockHdr,
                        SMR_FILE_END_FALSE,
                        SMR_CHAINED_BLOCK_FALSE,
                        SMR_INVALID_WRAP_NO,
                        SMR_BLOCK_WRITE_FINISH );

    for( i = 0; i < aBufferSize / aBlockSize; i++ )
    {
        SMR_WRITE_LOG_BLOCK_HDR( aBuffer + (i * aBlockSize),
                                 &sLogBlockHdr );
    }

    *(stlInt64*) (aBuffer + (i * aBlockSize)) = 0xdeadbeefdeadbeef;

    return STL_SUCCESS;
}

stlBool smrValidateLogBuffer( smrLogBuffer * aLogBuffer,
                              stlInt32       aBlockSize )
{
    stlBool  sResult = STL_TRUE;

    STL_DASSERT( (aLogBuffer->mBlockOffset >= SMR_BLOCK_HDR_SIZE) &&
                 (aLogBuffer->mBlockOffset <= SMR_BLOCK_SIZE) );
    
    /**
     * BUFFER OVERFLOW
     */
    
    if( *(stlInt64*)(aLogBuffer->mBuffer +
                     (((stlInt64)aLogBuffer->mBufferBlockCount) *
                      aBlockSize))
        != 0xdeadbeefdeadbeef )
    {
        sResult = STL_FALSE;
    }

    return sResult;
}

stlInt32 smrNeedBlockCount( smrLogStream * aLogStream,
                            stlUInt32      aLogSize )
{
    smrLogBuffer * sLogBuffer;
    stlInt32       sNeedBlockCount = 0;
    stlInt32       sWrittenBytes;
    stlInt32       sTotalWrittenBytes = 0;
    stlInt16       sBlockOffset;
    stlInt32       sBlockSize;

    sLogBuffer = aLogStream->mLogBuffer;
    sBlockOffset = sLogBuffer->mBlockOffset;
    sBlockSize = aLogStream->mLogStreamPersData.mBlockSize;
    
    if( (sBlockOffset + SMR_LOG_HDR_SIZE) > sBlockSize )
    {
        sBlockOffset = SMR_BLOCK_HDR_SIZE;
        sNeedBlockCount++;
    }

    sBlockOffset += SMR_LOG_HDR_SIZE;

    while( sTotalWrittenBytes < aLogSize )
    {
        if( aLogSize > (sBlockSize - sBlockOffset) )
        {
            sWrittenBytes = (sBlockSize - sBlockOffset);
            sNeedBlockCount++;
            sBlockOffset = SMR_BLOCK_HDR_SIZE;
        }
        else
        {
            sWrittenBytes = aLogSize;
            sBlockOffset += sWrittenBytes;
        }
        
        sTotalWrittenBytes += sWrittenBytes;
    }

    return sNeedBlockCount;
}    

stlStatus smrLogArchiving( smrLsn    aFlushedLsn,
                           smlEnv  * aEnv )
{
    smrLogStream * sLogStream;
    smrLogGroup  * sCurLogGroup;
    smrLogGroup  * sNextLogGroup;

    /**
     * @todo 사용자에 의한 log archiving을 지원해야 한다.
     */

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV(aEnv),
                        KNL_LOG_LEVEL_INFO,
                        "[ARCHIVING] stable lsn(%ld)\n",
                        aFlushedLsn )
             == STL_SUCCESS );
    
    sLogStream = &gSmrWarmupEntry->mLogStream;
    sCurLogGroup  = sLogStream->mCurLogGroup4Disk;
    
    sNextLogGroup = smrGetNextLogGroup( sLogStream, sCurLogGroup );

    while( 1 )
    {
        if( sNextLogGroup == sLogStream->mCurLogGroup4Disk )
        {
            break;
        }

        sCurLogGroup  = sNextLogGroup;
        sNextLogGroup = smrGetNextLogGroup( sLogStream, sCurLogGroup );

        if( sCurLogGroup->mLogGroupPersData.mState != SMR_LOG_GROUP_STATE_ACTIVE )
        {
            continue;
        }

        if( sNextLogGroup->mLogGroupPersData.mPrevLastLsn <= aFlushedLsn )
        {
            if( smrGetArchivelogMode() == SML_ARCHIVELOG_MODE )
            {
                STL_TRY( smrLogFileArchiving( sLogStream,
                                              sCurLogGroup,
                                              aEnv ) == STL_SUCCESS );
            }

            STL_TRY( knlLogMsg( NULL,
                                KNL_ENV(aEnv),
                                KNL_LOG_LEVEL_INFO,
                                "[ARCHIVING] inactivate group #%d(%ld)\n",
                                sCurLogGroup->mLogGroupPersData.mLogGroupId,
                                sCurLogGroup->mLogGroupPersData.mFileSeqNo )
                     == STL_SUCCESS );
                
            gSmrWarmupEntry->mLogPersData.mLastInactiveLfsn =
                sCurLogGroup->mLogGroupPersData.mFileSeqNo;
            sCurLogGroup->mLogGroupPersData.mState = SMR_LOG_GROUP_STATE_INACTIVE;

            if( smrIsCheckpointerActive() == STL_TRUE )
            {
                STL_TRY( knlCondBroadcast( &(sLogStream->mCondVar),
                                           KNL_ENV( aEnv ) ) == STL_SUCCESS );
            }
        }                    
    }
        
    STL_TRY( smrTryEnableLogging( sLogStream, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

inline stlBool smrIsLogFlusherActive()
{
    return (gSmrWarmupEntry->mLogFlusherState == SML_LOG_FLUSHER_STATE_ACTIVE)? STL_TRUE : STL_FALSE;
}

inline stlBool smrIsCheckpointerActive()
{
    return (gSmrWarmupEntry->mCheckpointerState == SML_CHECKPOINTER_STATE_ACTIVE)? STL_TRUE : STL_FALSE;
}

/**
 * @brief 주어진 Lsn까지의 log가 disk에 flush되었는지 확인한다.
 * @param[in] aLsn Dirty page의 lsn
 */
inline stlBool smrIsLogFlushedByLsn( smrLsn aLsn )
{
    smrLogStream * sLogStream;
    smrLogBuffer * sLogBuffer;
    stlBool        sResult = STL_FALSE;

    sLogStream = &gSmrWarmupEntry->mLogStream;
    sLogBuffer = sLogStream->mLogBuffer;

    if( sLogBuffer->mFrontLsn >= aLsn )
    {
        sResult = STL_TRUE;
    }

    return sResult;
}

/**
 * @brief 래치 획득과함께 Group Commit을 위한 Group에 Join한다.
 * @param[in]  aLogStream       Log Stream Pointer
 * @param[in]  aLsn             Group에 Join될 Lsn
 * @param[out] aGroupCommitLsn  Group Commit Lsn
 * @param[out] aSkipFlush       Pending Log Flushing을 Skip할지 여부
 * @param[in]  aEnv             Environment
 */
stlStatus smrAcquireLatchOrJoinGroup( smrLogStream * aLogStream,
                                      smrLsn         aLsn,
                                      smrLsn       * aGroupCommitLsn,
                                      stlBool      * aSkipFlush,
                                      smlEnv       * aEnv )
{
    knlLatch        * sLatch;
    smrLogBuffer    * sLogBuffer;
    knlLatchWaiter  * sMyWaiter = KNL_WAITITEM(aEnv);
    knlLatchWaiter  * sFlushWaiter;
    stlBool           sFound = STL_FALSE;
    stlThreadHandle * sMyThread;

    sLogBuffer = aLogStream->mLogBuffer;
    sLatch = &sLogBuffer->mBufferLatch;
    
    *aSkipFlush = STL_FALSE;
    sMyWaiter->mForLogFlush = STL_TRUE;
    
    if( KNL_IS_SHARED_ENV( aEnv ) == STL_TRUE )
    {
        sMyThread = &(KNL_ENV(aEnv)->mThread);
    }
    else
    {
        sMyThread = NULL;
    }
        
    KNL_ENTER_NON_CANCELLABLE_SECTION( aEnv );
    KNL_ENTER_CRITICAL_SECTION( aEnv );
    
    STL_RAMP( RAMP_RETRY );
    
    stlAcquireSpinLock( &sLatch->mSpinLock,
                        NULL /* Miss count */ );

    if( sLatch->mCurMode == KNL_LATCH_MODE_INIT )
    {
        /**
         * Group Commit Driver가 됨.
         */
        
        sLogBuffer->mGroupCommitLsn = STL_MAX( sLogBuffer->mGroupCommitLsn, aLsn );

        sLatch->mCurMode = KNL_LATCH_MODE_EXCLUSIVE;
        STL_ASSERT( sLatch->mRefCount == 0 );
        sLatch->mRefCount++;
        sLatch->mExclLockSeq++;
        sLatch->mExclOwner = sMyThread;
        
        stlReleaseSpinLock( &sLatch->mSpinLock );
        
        KNL_PUSH_LATCH( KNL_ENV(aEnv), sLatch );
    }
    else
    {
        sLogBuffer->mGroupCommitLsn = STL_MAX( sLogBuffer->mGroupCommitLsn, aLsn );
        
        if( sLatch->mCurMode == KNL_LATCH_MODE_EXCLUSIVE )
        {
            if( sLatch->mExclOwner != NULL )
            {
                if( sLatch->mExclOwner == sMyThread )
                {
                    /* 호환됨 */
                    sLatch->mRefCount++;

                    stlReleaseSpinLock( &sLatch->mSpinLock );
            
                    KNL_PUSH_LATCH( KNL_ENV(aEnv), sLatch );
                    
                    STL_THROW( RAMP_SUCCESS );
                }
            }
            
            sFound = STL_FALSE;

            /**
             * Waiting List에 Log Flush를 대기하는 트랜잭션이 존재하는지 검사한다.
             */
            
            STL_RING_FOREACH_ENTRY( &sLatch->mWaiters, sFlushWaiter )
            {
                if( sFlushWaiter->mForLogFlush == STL_TRUE )
                {
                    sFound = STL_TRUE;
                    break;
                }
            }
            
            if( sFound == STL_FALSE )
            {
                /**
                 * Group Commit Driver가 됨.
                 */
                
                sMyWaiter->mReqMode = KNL_LATCH_MODE_EXCLUSIVE;
                
                STL_RING_ADD_FIRST( &sLatch->mWaiters,
                                    sMyWaiter );
                
                sLatch->mWaitCount++;
                
                stlReleaseSpinLock( &sLatch->mSpinLock );
                
                KNL_LEAVE_CRITICAL_SECTION( aEnv );
                
                STL_TRY( stlAcquireSemaphore( &sMyWaiter->mSem,
                                              KNL_ERROR_STACK(aEnv) )
                         == STL_SUCCESS );
                
                KNL_ENTER_CRITICAL_SECTION( aEnv );
                
                STL_THROW( RAMP_RETRY );
            }
            else
            {
                /**
                 * Group Commit Member가 됨.
                 */
                
                stlReleaseSpinLock( &sLatch->mSpinLock );
                
                KNL_LEAVE_CRITICAL_SECTION( aEnv );
                KNL_LEAVE_NON_CANCELLABLE_SECTION( aEnv );
                
                *aSkipFlush = STL_TRUE;
                STL_THROW( RAMP_SKIP );
            }
        }
        else
        {
            STL_DASSERT( STL_FALSE );
        }
    }

    STL_RAMP( RAMP_SUCCESS );
    
    *aGroupCommitLsn = sLogBuffer->mGroupCommitLsn;
    *aSkipFlush = STL_FALSE;
    
    STL_RAMP( RAMP_SKIP );
                        
    sMyWaiter->mForLogFlush = STL_FALSE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

#if 0
stlStatus smrGetLastChkptLsn( smrLogStream  * aLogStream,
                              smrLsn        * aLastChkptLsn,
                              smlEnv        * aEnv )
{
    smrLogGroup    * sLogGroup;
    smrLsn           sChkptLsn;
    smrLsn           sLastChkptLsn = SMR_INVALID_LSN;

    STL_RING_FOREACH_ENTRY( &aLogStream->mLogGroupList, sLogGroup )
    {
        /**
         * Log group의 checkpoint Lsn을 가져온다.
         */ 

        STL_TRY( smrGetChkptLsn( sLogGroup, &sChkptLsn, aEnv )
                 == STL_SUCCESS );

        if( sLastChkptLsn == SMR_INVALID_LSN )
        {
            sLastChkptLsn = sChkptLsn;
        }
        else
        {
            sLastChkptLsn = STL_MAX( sChkptLsn, sLastChkptLsn );
        }
    }

    *aLastChkptLsn = sLastChkptLsn;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
#endif

/** @} */
