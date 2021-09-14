/*******************************************************************************
 * smrRedo.c
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
#include <smxl.h>
#include <smf.h>
#include <sms.h>
#include <smt.h>
#include <smrLogFile.h>
#include <smrLogBuffer.h>
#include <smrRecoveryMgr.h>
#include <smrMediaRecoveryMgr.h>
#include <smrRedo.h>
#include <smrLogCursor.h>
#include <smrMtxUndo.h>

/**
 * @file smrRedo.c
 * @brief Storage Manager Layer Recovery Redo Internal Routines
 */
extern smrWarmupEntry * gSmrWarmupEntry;
extern smrRedoVector  * gSmrRedoVectorsTable[];

/**
 * @addtogroup smrRedo
 * @{
 */

smrRedoVector gSmrRedoVectors[SMR_RECOVERY_LOG_MAX_COUNT] =
{
    { /* SMR_LOG_CHKPT_BEGIN */
        smrRedoChkptBegin,
        NULL,
        "CHKPT_BEGIN"
    },
    { /* SMR_LOG_CHKPT_BODY */
        smrRedoChkptBody,
        NULL,
        "CHKPT_BODY"
    },
    { /* SMR_LOG_CHKPT_END */
        smrRedoChkptEnd,
        NULL,
        "CHKPT_END"
    },
    { /* SMR_LOG_BYTES */
        smrRedoBytes,
        smrMtxUndoBytes,
        "BYTES"
    },
    { /* SMR_LOG_DUMMMY */
        smrRedoDummy,
        NULL,
        "DUMMY"
    }
};

stlStatus smrRedo( smrLogCursor * aCursor,
                   smlEnv       * aEnv )
{
    stlChar                 * sLogBody;
    stlChar                 * sLogPieceBody;
    smrLogHdr               * sLogHdr;
    stlInt16                  sLogPieceCount;
    stlInt32                  sLogBodyOffset;
    smrLogPieceHdr            sLogPieceHdr;
    stlInt16                  sLpsn = 0;
    smlRid                    sDataRid;
    smrMediaRecoveryInfo    * sMediaRecoveryInfo = NULL;
    stlBool                   sIsRedoable = STL_TRUE;
#ifdef STL_DEBUG
    smxlTransId               sOrgTransId;
#endif
    
    sLogHdr        = smrGetCurLogHdr( aCursor );
    sLogBody       = smrGetCurLogBody( aCursor );
    sLogPieceCount = sLogHdr->mLogPieceCount;

    sLogBodyOffset = 0;

    if( smrProceedMediaRecovery( aEnv ) == STL_TRUE )
    {
        sMediaRecoveryInfo = SMR_GET_MEDIA_RECOVERY_INFO( aEnv );
    }
    
    if( SMXL_IS_SYSTEM_TRANS( sLogHdr->mTransId ) == STL_FALSE )
    {
        if( smrProceedOnlineMediaRecovery( aEnv ) != STL_TRUE )
        {
#ifdef STL_DEBUG
            sOrgTransId = smxlGetUnsafeTransId( sLogHdr->mTransId );
#endif
            smxlBindTrans( sLogHdr->mTransId );
        }
        else
        {
            /**
             * Online tablespace media recovery를 수행할 때만
             * media recovery 전용의 transaction table을 사용.
             */
#ifdef STL_DEBUG
            sOrgTransId = smxlGetUnsafeTransId4MediaRecovery(
                SMXL_MEDIA_RECOVERY_TRANS_ID(sLogHdr->mTransId),
                aEnv );
#endif
            smxlBindTrans4MediaRecovery(
                SMXL_MEDIA_RECOVERY_TRANS_ID(sLogHdr->mTransId),
                aEnv );
        }
    }

    while( sLogPieceCount > 0 )
    {
        SMR_READ_MOVE_LOGPIECE_HDR( &sLogPieceHdr,
                                    sLogBody,
                                    sLogBodyOffset );

        sLogPieceBody = sLogBody + sLogBodyOffset;
        SMG_WRITE_RID( &sDataRid, &sLogPieceHdr.mDataRid );

        /**
         * Tablespace Media Recovery의 경우 해당 Tablespace에 대한 Redo가 아니면 skip
         */
        if( smrProceedMediaRecovery(aEnv) == STL_TRUE )
        {
            STL_TRY_THROW( (smrGetTbsId4MediaRecovery(aEnv) == SML_INVALID_TBS_ID) ||
                           (sDataRid.mTbsId == smrGetTbsId4MediaRecovery(aEnv)),
                           RAMP_SKIP_REDO_LOG_PIECE );

            /**
             * Datafile Recovery의 경우
             * - smrGetTbsId4MediaRecovery(aEnv)의 Value는 SML_INVALID_TBS_ID.
             * - 각 Datafile List를 순회하면서 Datafile List를 검사.
             */ 
            if( STL_RING_IS_EMPTY( &sMediaRecoveryInfo->mDatafileList ) == STL_FALSE )
            {
                STL_TRY( smrIsRedoableRid4DatafileRecovery( sMediaRecoveryInfo,
                                                            &sDataRid,
                                                            aCursor->mCurLsn,
                                                            &sIsRedoable,
                                                            aEnv )
                         == STL_SUCCESS );
                
                STL_TRY_THROW( sIsRedoable == STL_TRUE, RAMP_SKIP_REDO_LOG_PIECE );
            }
        }

        switch( sLogPieceHdr.mRedoTargetType )
        {
            case SMR_REDO_TARGET_PAGE:
                STL_TRY( smrRedoPage( sLogHdr,
                                      &sLogPieceHdr,
                                      sLogPieceBody,
                                      sLogHdr->mLsn,
                                      sLpsn,
                                      aEnv ) == STL_SUCCESS );
                break;
            case SMR_REDO_TARGET_UNDO_SEGMENT:
#ifdef STL_DEBUG
                if( sLogPieceHdr.mType != SMR_LOG_INSERT_TRANSACTION_RECORD )
                {
                    if( smrProceedOnlineMediaRecovery( aEnv ) != STL_TRUE )
                    {
                        STL_ASSERT( sOrgTransId == sLogHdr->mTransId );
                    }
                    else
                    {
                        STL_ASSERT( sOrgTransId ==
                                    SMXL_MEDIA_RECOVERY_TRANS_ID(sLogHdr->mTransId) );
                    }
                }
                else
                {
                    if( smrProceedOnlineMediaRecovery( aEnv ) != STL_TRUE )
                    {
                        sOrgTransId = sLogHdr->mTransId;
                    }
                    else
                    {
                        sOrgTransId = SMXL_MEDIA_RECOVERY_TRANS_ID(sLogHdr->mTransId);
                    }
                }
#endif
                STL_TRY( smrRedoUndoSegment( sLogHdr,
                                             &sLogPieceHdr,
                                             sLogPieceBody,
                                             sLogHdr->mLsn,
                                             sLpsn,
                                             aEnv ) == STL_SUCCESS );
                break;
            case SMR_REDO_TARGET_TRANSACTION:
                
                /**
                 * refine log header
                 * - IN-DOUBT Transaction은 하나의 로그에 다수개의
                 *   COMMIT/ROLLBACK log piece가 존재할수 있기 때문에
                 *   sLogHdr.mTransId를 수정해야 한다.
                 */
                
                if( (sLogPieceHdr.mType == SMR_LOG_COMMIT) ||
                    (sLogPieceHdr.mType == SMR_LOG_ROLLBACK) )
                {
                    sLogHdr->mTransId = smxlGetTransIdFromLog( sLogPieceBody );
                }
                
                STL_TRY( smrRedoTransaction( sLogHdr,
                                             &sLogPieceHdr,
                                             sLogPieceBody,
                                             sLogHdr->mLsn,
                                             sLpsn,
                                             aEnv ) == STL_SUCCESS );

                if( (sLogPieceHdr.mType == SMR_LOG_COMMIT) ||
                    (sLogPieceHdr.mType == SMR_LOG_ROLLBACK) )
                {
                    if( smrProceedOnlineMediaRecovery( aEnv ) != STL_TRUE )
                    {
                        STL_TRY( smxlFreeTrans( sLogHdr->mTransId,
                                                SML_PEND_ACTION_NONE,
                                                STL_FALSE, /* aCleanup */
                                                SML_TRANSACTION_CWM_WAIT,
                                                aEnv ) == STL_SUCCESS );
                    }
                    else
                    {
                        STL_TRY( smxlFreeTrans4MediaRecovery(
                                     SMXL_MEDIA_RECOVERY_TRANS_ID(sLogHdr->mTransId),
                                     aEnv ) == STL_SUCCESS );
                    }
                }

                break;
            case SMR_REDO_TARGET_CTRL:
                STL_TRY( smrRedoCtrl( &sLogPieceHdr,
                                      sLogPieceBody,
                                      sLogHdr->mLsn,
                                      aEnv ) == STL_SUCCESS );
                break;
            case SMR_REDO_TARGET_TEST:
                STL_TRY( smrRedoTest( &sLogPieceHdr,
                                      sLogPieceBody,
                                      sLogHdr->mLsn,
                                      aEnv ) == STL_SUCCESS );
                break;
            case SMR_REDO_TARGET_NONE:
                /**
                 * no need recovery
                 */
                break;
            default:
                STL_ASSERT( STL_FALSE );
                break;
        }

        if( smrProceedMediaRecovery(aEnv) == STL_FALSE )
        {
            gSmrWarmupEntry->mRecoveryInfo.mLogLpsn = sLpsn;
        }

        STL_RAMP( RAMP_SKIP_REDO_LOG_PIECE );

        sLogBodyOffset += sLogPieceHdr.mSize;
        sLogPieceCount--;
        sLpsn++;

        STL_TRY( knlCheckEndSession( KNL_SESS_ENV(aEnv),
                                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* MOUNT phase의 recovery는 gmaster의 event handler로 수행되므로
         * 주어진 event가 cancel되었는지 체크한다. */
        if( knlGetCurrStartupPhase() < KNL_STARTUP_PHASE_OPEN )
        {
            STL_TRY( knlCheckEnvEvent( KNL_ENV( aEnv ) ) == STL_SUCCESS );
        }
    }
    
    if( smrProceedMediaRecovery(aEnv) == STL_FALSE )
    {
        gSmrWarmupEntry->mRecoveryInfo.mLogLsn        = sLogHdr->mLsn;
        gSmrWarmupEntry->mRecoveryInfo.mLogTransId    = sLogHdr->mTransId;
        gSmrWarmupEntry->mRecoveryInfo.mLogPieceCount = sLogPieceCount;
        SMG_WRITE_RID( &gSmrWarmupEntry->mRecoveryInfo.mLogSegRid, &sLogHdr->mSegRid );
    }

    return STL_SUCCESS;

    STL_FINISH;

    stlPushError( STL_ERROR_LEVEL_ABORT,
                  SML_ERRCODE_FAILED_TO_REDO,
                  NULL,
                  KNL_ERROR_STACK(aEnv),
                  sLogHdr->mLsn,
                  sLpsn, 
                  sLogHdr->mSegRid );
    
    return STL_FAILURE;
}

stlStatus smrAnalyzeLog( smrLogCursor * aCursor,
                         stlBool      * aIsChkptEnd,
                         smrTrans     * aTransTable,
                         smlEnv       * aEnv )
{
    stlChar        * sLogBody;
    stlChar        * sLogPieceBody;
    smrLogHdr      * sLogHdr;
    stlInt16         sLogPieceCount;
    stlInt32         sLogBodyOffset;
    smrLogPieceHdr   sLogPieceHdr;
    stlInt16         sTransSlotId;
    smrTrans       * sTrans;
    smxlTransId      sTransId;

    *aIsChkptEnd = STL_FALSE;
    
    sLogHdr        = smrGetCurLogHdr( aCursor );
    sLogBody       = smrGetCurLogBody( aCursor );
    sLogPieceCount = sLogHdr->mLogPieceCount;

    sLogBodyOffset = 0;

    while( sLogPieceCount > 0 )
    {
        SMR_READ_MOVE_LOGPIECE_HDR( &sLogPieceHdr,
                                    sLogBody,
                                    sLogBodyOffset );

        sLogPieceBody = sLogBody + sLogBodyOffset;

        if( ( sLogPieceHdr.mRedoTargetType == SMR_REDO_TARGET_TRANSACTION ) &&
            ( (sLogPieceHdr.mType == SMR_LOG_COMMIT) ||
              (sLogPieceHdr.mType == SMR_LOG_ROLLBACK) ) )
        {
            sTransId = smxlGetTransIdFromLog( sLogPieceBody );
        }
        else
        {
            sTransId = sLogHdr->mTransId;
        }
        
        /**
         * SYSTEM Transaction은 Transaction Table에 포함시켜서는 안된다.
         */
        if( sTransId == SMXL_SYSTEM_TRANSID )
        {
            sLogBodyOffset += sLogPieceHdr.mSize;
            sLogPieceCount--;
            continue;
        }
        
        sTransSlotId = SMXL_TRANS_SLOT_ID( sTransId );
        sTrans = &aTransTable[ sTransSlotId ];
        
        switch( sLogPieceHdr.mRedoTargetType )
        {
            case SMR_REDO_TARGET_PAGE:
                
                /**
                 * Active transaction table을 갱신한다.
                 */
                if( (sTrans->mBeginLsn == 0) && (sTrans->mCommitLsn == 0) )
                {
                    sTrans->mTransId = sTransId;
                }
                break;
                
            case SMR_REDO_TARGET_TEST:
            case SMR_REDO_TARGET_UNDO_SEGMENT:
                
                /**
                 * Begin Transaction Log는 Active transaction table을 갱신한다.
                 */
                if( sLogPieceHdr.mType == SMR_LOG_INSERT_TRANSACTION_RECORD )
                {
                    if( sTrans->mCommitLsn == 0 )
                    {
                        /**
                         *                      T1   
                         *                 |<-------->|
                         * -------------------------------------------
                         *          ^                        ^
                         *      oldestlsn                checkpoint
                         *     
                         * T1가 초기 Active transaction의 대상에 속하지 않음을 설정한다.
                         */
                        
                        sTrans->mBeginLsn = sLogHdr->mLsn;
                        sTrans->mTransId = sTransId;
                        sTrans->mTransViewScn = smxlGetTransViewScnFromUndoLog( sLogPieceBody );
                    }
                    else
                    {
                        /**
                         *          T1             T2   
                         *   |<----------->|  |<-------->|
                         * -------------------------------------------
                         *          ^                        ^
                         *      oldestlsn                checkpoint
                         *
                         * T1과 T2의 Transaction ID가 동일할 경우
                         * T2는 Active transaction table을 갱신해서는 안된다.
                         */
                    }                        
                }
                break;
                
            case SMR_REDO_TARGET_TRANSACTION:
                
                if( (sTrans->mBeginLsn == 0) && (sTrans->mCommitLsn == 0) )
                {
                    /**
                     *           T2   
                     *   |<--------------->|
                     * -------------------------------------------
                     *          ^                        ^
                     *      oldestlsn                checkpoint
                     *     
                     * T2는 redo 시작시점에 Active Transaction이기 때문에 유효한 View Scn을 가져야 한다.
                     */
                    if( (sLogPieceHdr.mType == SMR_LOG_COMMIT) ||
                        (sLogPieceHdr.mType == SMR_LOG_ROLLBACK) )
                    {
                        sTrans->mCommitLsn = sLogHdr->mLsn;
                    }

                    if( sLogPieceHdr.mType == SMR_LOG_PREPARE )
                    {
                        sTrans->mState = SMXL_STATE_PREPARE;
                        sTrans->mRepreparable = sLogPieceHdr.mRepreparable;
                    }
                    
                    sTrans->mTransId = sTransId;
                    sTrans->mTransViewScn = smxlGetTransViewScnFromLog( sLogPieceBody );
                }
                break;
                
            case SMR_REDO_TARGET_CTRL:

                /**
                 * Checkpoint Body 로그라면 Active Transaction List를 분석한다.
                 */
                if( sLogPieceHdr.mType == SMR_LOG_CHKPT_BODY )
                {
                    STL_TRY( smrAnalyzeChkptBody( sLogPieceBody,
                                                  sLogPieceHdr.mSize,
                                                  aTransTable,
                                                  aEnv )
                             == STL_SUCCESS );
                }
                
                /**
                 * Checkpoint End 로그라면 종료한다.
                 */
                if( sLogPieceHdr.mType == SMR_LOG_CHKPT_END )
                {
                    *aIsChkptEnd = STL_TRUE;

                    /**
                     * OldestLsn 당시 Active Transaction Table 구성에 필요한 정보가
                     * 모두 수집되었기 때문에 더이상 분석할 필요가 없다.
                     */
                    STL_THROW( RAMP_SUCCESS );
                }
                break;
                
            case SMR_REDO_TARGET_NONE:
                /**
                 * ignore
                 */
                break;
                
            default:
                STL_ASSERT( STL_FALSE );
                break;
        }

        sLogBodyOffset += sLogPieceHdr.mSize;
        sLogPieceCount--;
    }

    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrAnalyzeChkptBody( void      * aData,
                               stlUInt16   aDataSize,
                               smrTrans  * aTransTable,
                               smlEnv    * aEnv )
{
    stlInt32        sOffset = 0;
    stlInt32        sTransCount = 0;
    smxlTransId   * sTransArr;
    smlRid        * sUndoSegArr;
    smlScn        * sScnArr;
    stlChar       * sStateArr;
    stlChar       * sRepreparableArr;
    stlInt32        i;
    stlInt32        sTransSlotId;
    stlInt64        sTransTableSize;
        
    STL_READ_MOVE_INT32( &sTransCount, aData, sOffset );

    if( SMR_PROCEED_MEDIA_RECOVERY( aEnv ) == STL_TRUE )
    {
        sTransTableSize = SMXL_MAX_TRANS_TABLE_SIZE;
    }
    else
    {
        sTransTableSize = smxlGetTransTableSize();
    }
    
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
    
    STL_READ_MOVE_BYTES( sTransArr,
                         aData,
                         sTransCount * STL_SIZEOF(smxlTransId),
                         sOffset );
    STL_READ_MOVE_BYTES( sUndoSegArr,
                         aData,
                         sTransCount * STL_SIZEOF(smlRid),
                         sOffset );
    STL_READ_MOVE_BYTES( sScnArr,
                         aData,
                         sTransCount * STL_SIZEOF(smlScn),
                         sOffset );
    STL_READ_MOVE_BYTES( sStateArr,
                         aData,
                         sTransCount * STL_SIZEOF(stlChar),
                         sOffset );
    STL_READ_MOVE_BYTES( sRepreparableArr,
                         aData,
                         sTransCount * STL_SIZEOF(stlBool),
                         sOffset );

    for( i = 0; i < sTransCount; i++ )
    {
        STL_ASSERT( SMXL_IS_UNDO_BOUND_TRANS( sTransArr[i] ) == STL_TRUE );

        sTransSlotId = SMXL_TRANS_SLOT_ID( sTransArr[i] );

        /**
         * OldestLsn 이전에 Active 였다면 Transaction Table에 추가한다.
         */
        if( (aTransTable[sTransSlotId].mBeginLsn == 0) &&
            (aTransTable[sTransSlotId].mCommitLsn == 0) )
        {
            /**
             *                      T1   
             *   |<---------------------------------->|
             * -------------------------------------------
             *          ^                        ^
             *      oldestlsn                checkpoint
             *     
             * T은 redo 시작시점에 Active Transaction이기 때문에 유효한 View Scn을 가져야 한다.
             */
            
            aTransTable[sTransSlotId].mTransId = sTransArr[i];
            aTransTable[sTransSlotId].mUndoSegRid = sUndoSegArr[i];
            aTransTable[sTransSlotId].mTransViewScn = sScnArr[i];
            aTransTable[sTransSlotId].mState = sStateArr[i];
            aTransTable[sTransSlotId].mRepreparable = sRepreparableArr[i];
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrRedoPage( smrLogHdr      * aLogHdr,
                       smrLogPieceHdr * aLogPieceHdr,
                       stlChar        * aLogPieceBody,
                       smrLsn           aLsn,
                       stlInt16         aLpsn,
                       smlEnv         * aEnv )
{
    smlRid          sDataRid;
    smpHandle       sPageHandle;
    stlUInt32       sState = 0;
    smrRedoVector * sRedoVectors;
    smrLsn          sRecoveryLsn;
    stlChar         sBeforePageImage[SMP_PAGE_SIZE];

    SMG_WRITE_RID( &sDataRid, &aLogPieceHdr->mDataRid );

    STL_TRY_THROW( smfIsRedoableDatafile( sDataRid.mTbsId,
                                          SMP_DATAFILE_ID(sDataRid.mPageId) )
                   == STL_TRUE, RAMP_SUCCESS );

    STL_DASSERT( smtIsLogging( sDataRid.mTbsId ) == STL_TRUE );

    STL_DASSERT( smfGetDatafileCorruptionCount( sDataRid.mTbsId,
                                                SMP_DATAFILE_ID(sDataRid.mPageId)) == 0 );
    
    STL_TRY( smpAcquire( NULL,
                         sDataRid.mTbsId,
                         sDataRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );
    sState = 1;

    /**
     * Relation 이상의 경우에 회복 불가한 Segment는 redo 하지 않는다.
     * - 향후 Drop 될것을 가정한다.
     */
    if( aLogPieceHdr->mComponentClass > SMG_COMPONENT_RELATION )
    {
        STL_TRY_THROW( smsIsIrrecoverableSegment( aLogHdr->mSegRid )
                       == STL_FALSE, RAMP_SUCCESS );
    }
    
    sRecoveryLsn = smpGetRecoveryLsn( &sPageHandle );

    STL_TRY_THROW( sRecoveryLsn <= aLsn, RAMP_SUCCESS );

    /**
     * 하나의 로그에는 n개의 Piece가 존재할수 있기 때문에 Piece 별로
     * redo 여부를 결정해야 한다. 만약 LSN이 같다면 Lpsn으로 비교한다.
     * 이러한 경우는 서버가 구동되어 있는 상태에서 n번의 recovery를 반복할 경우
     * 발생할수 있다. 
     */
    if( sRecoveryLsn == aLsn )
    {
        STL_TRY_THROW( smpGetRecoveryLpsn( &sPageHandle ) < aLpsn, RAMP_SUCCESS );
    }
    
    sRedoVectors = gSmrRedoVectorsTable[(stlInt16)aLogPieceHdr->mComponentClass];
    STL_ASSERT( sRedoVectors != NULL );

    stlMemcpy( sBeforePageImage, SMP_FRAME(&sPageHandle), SMP_PAGE_SIZE );
    sState = 2;
    
    STL_TRY( sRedoVectors[aLogPieceHdr->mType].mRedoFunc( &sDataRid,
                                                          aLogPieceBody,
                                                          aLogPieceHdr->mSize,
                                                          &sPageHandle,
                                                          aEnv )
             == STL_SUCCESS );
    sState = 1;

    smpSetRecoveryLsn( &sPageHandle,
                       SMR_INVALID_SBSN,
                       aLsn,
                       aLpsn );
    
    STL_TRY( smpSetDirty( &sPageHandle, aEnv ) == STL_SUCCESS );

    STL_RAMP( RAMP_SUCCESS );

    if( sState == 1 )
    {
        sState = 0;
        STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            stlMemcpy( SMP_FRAME(&sPageHandle), sBeforePageImage, SMP_PAGE_SIZE );
        case 1:
            (void)smpRelease( &sPageHandle, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus smrRedoUndoSegment( smrLogHdr      * aLogHdr,
                              smrLogPieceHdr * aLogPieceHdr,
                              stlChar        * aLogPieceBody,
                              smrLsn           aLsn,
                              stlInt16         aLpsn,
                              smlEnv         * aEnv )
{
    if( smrProceedOnlineMediaRecovery( aEnv ) != STL_TRUE )
    {
        smxlSetUndoSegRid( aLogHdr->mTransId,
                           smxlGetUndoSegRidFromLog( aLogPieceBody ) );

        smxlSetTransRecordRecorded( aLogHdr->mTransId );

        if( aLogPieceHdr->mType == SMR_LOG_INSERT_TRANSACTION_RECORD )
        {
            smxlSetTransViewScn( aLogHdr->mTransId,
                                 smxlGetTransViewScnFromUndoLog( aLogPieceBody ) );
        }
    }
    else
    {
        smxlSetUndoSegRid4MediaRecovery(
            SMXL_MEDIA_RECOVERY_TRANS_ID( aLogHdr->mTransId ),
            smxlGetUndoSegRidFromLog( aLogPieceBody ),
            aEnv );

        smxlSetTransRecordRecorded4MediaRecovery(
            SMXL_MEDIA_RECOVERY_TRANS_ID( aLogHdr->mTransId ),
            aEnv );

        if( aLogPieceHdr->mType == SMR_LOG_INSERT_TRANSACTION_RECORD )
        {
            smxlSetTransViewScn4MediaRecovery(
                SMXL_MEDIA_RECOVERY_TRANS_ID( aLogHdr->mTransId ),
                smxlGetTransViewScnFromUndoLog( aLogPieceBody ),
                aEnv );
        }
    }
    
    STL_TRY( smrRedoPage( aLogHdr,
                          aLogPieceHdr,
                          aLogPieceBody,
                          aLsn,
                          aLpsn,
                          aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrRedoTransaction( smrLogHdr      * aLogHdr,
                              smrLogPieceHdr * aLogPieceHdr,
                              stlChar        * aLogPieceBody,
                              smrLsn           aLsn,
                              stlInt16         aLpsn,
                              smlEnv         * aEnv )
{
    smlRid sDataRid;

    SMG_WRITE_RID( &sDataRid, &aLogPieceHdr->mDataRid );

    if( aLogPieceHdr->mType == SMR_LOG_PREPARE )
    {
        smxlSetTransState( aLogHdr->mTransId, SMXL_STATE_PREPARE );
        smxlSetRepreparable( aLogHdr->mTransId, aLogPieceHdr->mRepreparable );
    }
    else
    {
        /**
         * Restart Recovery 혹은 Database Media Recovery시
         * System Scn을 변경한다.
         */
        if( (smrProceedMediaRecovery(aEnv) == STL_FALSE) ||
            (smrGetTbsId4MediaRecovery(aEnv) == SML_INVALID_TBS_ID) )
        {
            smxlSetSystemScn( smxlGetCommitScnFromLog( aLogPieceBody ) );
        }

        if( SML_IS_INVALID_RID( sDataRid ) == STL_FALSE )
        {
            STL_TRY( smrRedoPage( aLogHdr,
                                  aLogPieceHdr,
                                  aLogPieceBody,
                                  aLsn,
                                  aLpsn,
                                  aEnv ) == STL_SUCCESS );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrRedoCtrl( smrLogPieceHdr * aLogPieceHdr,
                       stlChar        * aLogPieceBody,
                       smrLsn           aLsn,
                       smlEnv         * aEnv )
{
    smrRedoVector * sRedoVectors;
    smlRid          sDataRid;

    SMG_WRITE_RID( &sDataRid, &aLogPieceHdr->mDataRid );
    sRedoVectors = gSmrRedoVectorsTable[(stlInt16)aLogPieceHdr->mComponentClass];
    STL_ASSERT( sRedoVectors != NULL );

    /**
     * @note Idempotent는 Redo 함수 자체적으로 해결해야 한다
     */
    STL_TRY( sRedoVectors[aLogPieceHdr->mType].mRedoFunc( &sDataRid,
                                                          aLogPieceBody,
                                                          aLogPieceHdr->mSize,
                                                          NULL,  /* aPageHandle */
                                                          aEnv )
             == STL_SUCCESS );

    STL_TRY( smfSaveCtrlFile( aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrRedoTest( smrLogPieceHdr * aLogPieceHdr,
                       stlChar        * aLogPieceBody,
                       smrLsn           aLsn,
                       smlEnv         * aEnv )
{
    smrRedoVector * sRedoVectors;
    smlRid          sDataRid;

    SMG_WRITE_RID( &sDataRid, &aLogPieceHdr->mDataRid );
    sRedoVectors = gSmrRedoVectorsTable[(stlInt16)aLogPieceHdr->mComponentClass];
    STL_ASSERT( sRedoVectors != NULL );
        
    STL_TRY( sRedoVectors[aLogPieceHdr->mType].mRedoFunc( &sDataRid,
                                                          aLogPieceBody,
                                                          aLogPieceHdr->mSize,
                                                          NULL,
                                                          aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrRedoBytes( smlRid    * aDataRid,
                        void      * aData,
                        stlUInt16   aDataSize,
                        smpHandle * aPageHandle,
                        smlEnv    * aEnv )
{
    stlChar * sAfterImage;

    sAfterImage = (stlChar*)aData + (aDataSize / 2);
    stlMemcpy( ((stlChar*)SMP_FRAME( aPageHandle )) + aDataRid->mOffset,
               sAfterImage,
               (aDataSize / 2) );
    
    return STL_SUCCESS;
}

stlStatus smrRedoDummy( smlRid    * aDataRid,
                        void      * aData,
                        stlUInt16   aDataSize,
                        smpHandle * aPageHandle,
                        smlEnv    * aEnv )
{
    return STL_SUCCESS;
}

stlStatus smrRedoChkptBegin( smlRid    * aDataRid,
                             void      * aData,
                             stlUInt16   aDataSize,
                             smpHandle * aPageHandle,
                             smlEnv    * aEnv )
{
    return STL_SUCCESS;
}

stlStatus smrRedoChkptBody( smlRid    * aDataRid,
                            void      * aData,
                            stlUInt16   aDataSize,
                            smpHandle * aPageHandle,
                            smlEnv    * aEnv )
{
    return STL_SUCCESS;
}

stlStatus smrRedoChkptEnd( smlRid    * aDataRid,
                           void      * aData,
                           stlUInt16   aDataSize,
                           smpHandle * aPageHandle,
                           smlEnv    * aEnv )
{
    return STL_SUCCESS;
}

stlBool smrIsCheckpointLog( smrLogCursor * aCursor,
                            smlEnv       * aEnv )
{
    stlChar        * sLogBody;
    stlInt32         sLogBodyOffset;
    smrLogPieceHdr   sLogPieceHdr;
    stlBool          sIsChkptLog;

    sIsChkptLog = STL_FALSE;
    
    sLogBody       = smrGetCurLogBody( aCursor );
    sLogBodyOffset = 0;

    SMR_READ_MOVE_LOGPIECE_HDR( &sLogPieceHdr,
                                sLogBody,
                                sLogBodyOffset );

    if( sLogPieceHdr.mComponentClass == SMG_COMPONENT_RECOVERY )
    {
        if( sLogPieceHdr.mType == SMR_LOG_CHKPT_BEGIN )
        {
            sIsChkptLog = STL_TRUE;
        }
    }

    return sIsChkptLog;
}

/** @} */
