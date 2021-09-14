/*******************************************************************************
 * smrMediaRecoveryMgr.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smrMediaRecoveryMgr.c 7394 2013-02-26 03:01:12Z mkkim $
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
#include <smxlDef.h>
#include <smf.h>
#include <smg.h>
#include <smxl.h>
#include <smr.h>
#include <smp.h>
#include <sme.h>
#include <sms.h>
#include <smq.h>
#include <smrDef.h>
#include <smlGeneral.h>
#include <smrArchiveLog.h>
#include <smrRecoveryMgr.h>
#include <smrMediaRecoveryMgr.h>
#include <smrLogFile.h>
#include <smrLogCursor.h>
#include <smrLogBuffer.h>
#include <smrRedo.h>
#include <smrMtxUndo.h>
#include <smrLogGroup.h>
#include <smrLogStream.h>
#include <smrLogMember.h>

/**
 * @file smrMediaRecoveryMgr.c
 * @brief Storage Manager Layer Media Recovery Manager Internal Routines
 */

extern smrWarmupEntry  * gSmrWarmupEntry;
extern smxlWarmupEntry * gSmxlWarmupEntry;

/**
 * @addtogroup smr
 * @{
 */

/**
 * @brief Backup을 이용하여 Database를 복구한다.
 * @param[out] aIsFinishedRecovery Media Recovery가 완료되었는지 여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smrMediaRecoverDatabase( stlBool * aIsFinishedRecovery,
                                   smlEnv  * aEnv )
{
    smrLsn           sLastLsn;
    smpFlushArg      sArg;
    smrLsn           sLastChkptLsn;
    smrLid           sLastChkptLid;
    stlInt32         sChangeDatafile;
    stlUInt8         sMediaRecoveryType;
    
    /**
     * 1. Database Media Recovery 수행
     */
    STL_TRY( smrMediaRecoverInternal( SMR_INIT_LSN,
                                      &sLastLsn,
                                      aIsFinishedRecovery,
                                      aEnv ) == STL_SUCCESS );

    if( *aIsFinishedRecovery == STL_TRUE )
    {
        sLastChkptLsn = SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, LastChkptLsn );
        sLastChkptLid = SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, LastChkptLid );

        /**
         * 2. Media Recovery완료 후 datafile을 flush
         */
        if( sLastChkptLsn != SMR_INVALID_LSN )
        {
            sArg.mBehavior = SMP_FLUSH_DIRTY_PAGE;
            sArg.mLogging  = STL_FALSE;
            sArg.mForCheckpoint = STL_FALSE;

            STL_TRY( knlAddEnvEvent( SML_EVENT_PAGE_FLUSH,
                                     NULL,            /* aEventMem */
                                     &sArg,
                                     STL_SIZEOF(smpFlushArg),
                                     SML_PAGE_FLUSHER_ENV_ID,
                                     KNL_EVENT_WAIT_POLLING,
                                     STL_FALSE, /* aForceSuccess */
                                     KNL_ENV( aEnv ) )
                     == STL_SUCCESS );

            /**
             * 3. Datafile header에 최신 checkpoin Lsn을 설정한다.
             */
            sMediaRecoveryType = smrGetRecoveryType( aEnv );

            if( sMediaRecoveryType == SML_RECOVERY_TYPE_COMPLETE_MEDIA_RECOVERY )
            {
                sChangeDatafile = SMF_CHANGE_DATAFILE_HEADER_FOR_RECOVERY;
            }
            else
            {
                sChangeDatafile = SMF_CHANGE_DATAFILE_HEADER_FOR_INCOMPLETE_RECOVERY;
            }
            
            STL_TRY( smfChangeDatafileHeader( NULL,    /* aStatement */
                                              sChangeDatafile, /* aReason */
                                              SML_INVALID_TBS_ID,
                                              SML_INVALID_DATAFILE_ID,
                                              sLastChkptLid,
                                              sLastChkptLsn,
                                              aEnv )
                     == STL_SUCCESS );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Backup 또는 offline datafile로부터 Tablespace를 복구한다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smrMediaRecoverTablespace( smlEnv   * aEnv )
{
    smlTbsId          sTbsId;
    smrLsn            sUntilLsn;
    smrLsn            sLastLsn;
    stlInt32          sState = 0;
    smpFlushTbsArg    sArg;

    sTbsId = smrGetTbsId4MediaRecovery( aEnv );

    SMF_SET_TBS_PROCEED_MEDIA_RECOVERY( sTbsId, STL_TRUE );
    sState = 1;

    if( SMF_IS_ONLINE_TBS( sTbsId ) == STL_FALSE )
    {
        /**
         * 1. Offline Tablespace의 경우 Media Recovery를 위해
         *    Datafile을 load한다.
         */
        STL_TRY( smpShrinkTablespace( sTbsId, aEnv ) == STL_SUCCESS );

        STL_TRY( smfParallelLoad( sTbsId,
                                  aEnv ) == STL_SUCCESS );

        STL_TRY( smpExtendTablespace( NULL,
                                      sTbsId,
                                      STL_FALSE,   /* Need Pending */
                                      aEnv ) == STL_SUCCESS );

        STL_TRY( smfVerifyTbsCorruption( sTbsId,
                                         aEnv )
                 == STL_SUCCESS );
    }

    sUntilLsn = smfGetOfflineLsn( sTbsId );

    if( knlGetCurrStartupPhase() >= KNL_STARTUP_PHASE_OPEN )
    {
        /**
         * Open단계에서의 tablespace media recovery 수행 시
         * tablespace의 offline Lsn이 valid하지 않을 경우,
         * 최신 Lsn까지만 log를 scan할 수 있도록 한다.
         */
        if( SMR_IS_VALID_LOG_LSN(sUntilLsn) != STL_TRUE )
        {
            sUntilLsn = smrGetSystemLsn();
        }
    }

    /**
     * 2. Tablespace Media Recovery 수행
     */
    STL_TRY( smrMediaRecoverInternal( sUntilLsn,
                                      &sLastLsn,
                                      NULL,    /* aIsFinishedRecovery */
                                      aEnv ) == STL_SUCCESS );

    /**
     * 3. Media Recovery완료 후 datafile을 flush
     */
    sArg.mTbsId = sTbsId;
    sArg.mDatafileId = SML_INVALID_DATAFILE_ID;
    sArg.mReason = SMF_CHANGE_DATAFILE_HEADER_FOR_GENERAL;

    STL_TRY( knlAddEnvEvent( SML_EVENT_PAGE_FLUSH_TBS,
                             NULL,            /* aEventMem */
                             (void*)&sArg,                 /* aData */
                             STL_SIZEOF(smpFlushTbsArg),   /* aDataSize */
                             SML_PAGE_FLUSHER_ENV_ID,
                             KNL_EVENT_WAIT_POLLING,
                             STL_FALSE, /* aForceSuccess */
                             KNL_ENV( aEnv ) ) == STL_SUCCESS );

    /**
     * SML_ERRCODE_TABLESPACE_OFFLINE_ABNORMALLY을 error로 처리한다.
     */
    STL_TRY( stlGetLastErrorCode( KNL_ERROR_STACK(aEnv) ) !=
             SML_ERRCODE_TABLESPACE_OFFLINE_ABNORMALLY );
    
    if( SMF_IS_ONLINE_TBS( sTbsId ) != STL_TRUE )
    {
        STL_TRY( smfSetTablespaceOfflineState(
                     sTbsId,
                     SMF_OFFLINE_STATE_CONSISTENT,
                     SMR_INIT_LSN,
                     aEnv ) == STL_SUCCESS );
    }

    sState = 0;
    SMF_SET_TBS_PROCEED_MEDIA_RECOVERY( sTbsId, STL_FALSE );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState > 0 )
    {
        SMF_SET_TBS_PROCEED_MEDIA_RECOVERY( sTbsId, STL_FALSE );
    }

    return STL_FAILURE;
}

/**
 * @brief Datafile Media Recovery를 수행한다.
 * @param[in,out] aEnv Environment Info
 */ 
stlStatus smrMediaRecoverDatafile( smlEnv * aEnv )
{
    smrMediaRecoveryInfo     * sRecoveryInfo = NULL;
    smrRecoveryDatafileInfo  * sDatafileInfo = NULL;
    stlChar                    sTbsName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    smrLsn                     sLastLsn;
    smpFlushTbsArg             sTbsArg;

    sRecoveryInfo =  SMR_GET_MEDIA_RECOVERY_INFO( aEnv );

    STL_DASSERT( sRecoveryInfo != NULL );

    /**
     * 1. Load Offline Talbespace
     */
    STL_RING_FOREACH_ENTRY( &sRecoveryInfo->mDatafileList, sDatafileInfo )
    {
        if( smrIsExistSameTbs4DatafileRecovery( sRecoveryInfo, sDatafileInfo ) == STL_TRUE )
        {
            continue;
        }

        SMF_SET_TBS_PROCEED_MEDIA_RECOVERY( sDatafileInfo->mTbsId, STL_TRUE );

        if( knlGetCurrStartupPhase() == KNL_STARTUP_PHASE_OPEN )
        {
            STL_TRY_THROW( (SMF_IS_ONLINE_TBS( sDatafileInfo->mTbsId ) == STL_FALSE),
                           RAMP_ERR_NOT_OFFLINE );
        }

        /**
         * 기존에 Datafile ShmState가 Loaded 이상이라면 PchArray를 Shrink하고, 다시 Load 한다.
         */
        STL_TRY( smpShrinkDatafile( sDatafileInfo->mTbsId,
                                    sDatafileInfo->mDatafileId,
                                    aEnv ) == STL_SUCCESS );

        STL_TRY( smfParallelLoad( sDatafileInfo->mTbsId,
                                  aEnv ) == STL_SUCCESS );

        STL_TRY( smpExtendTablespace( NULL,
                                      sDatafileInfo->mTbsId,
                                      STL_FALSE, /* aNeedPending */
                                      aEnv ) == STL_SUCCESS );
    }

    STL_RING_FOREACH_ENTRY( &sRecoveryInfo->mDatafileList, sDatafileInfo )
    {
        /**
         * Restore 이전에 Corruption Option을 위해 깨진 Page List 생성
         */
        if( sDatafileInfo->mForCorruption == STL_TRUE )
        {
            STL_TRY( smpMakeCorruptedPageList( sDatafileInfo,
                                               aEnv ) == STL_SUCCESS );
        }
    }
    
    /**
     * 2. Restore Datafile
     */
    
    STL_TRY( smfRestoreDatafile4MediaRecovery( sRecoveryInfo,
                                               aEnv )
             == STL_SUCCESS );

    /**
     * 3. Recover Datafile
     */
    
    STL_RING_FOREACH_ENTRY( &sRecoveryInfo->mDatafileList, sDatafileInfo )
    {
        if( smrIsExistSameTbs4DatafileRecovery( sRecoveryInfo, sDatafileInfo ) != STL_TRUE )
        {
            STL_TRY( smfVerifyTbsCorruption( sDatafileInfo->mTbsId,
                                             aEnv ) == STL_SUCCESS );
        }
    }

    STL_TRY( smrMediaRecoverInternal( SMR_INIT_LSN,
                                      &sLastLsn,
                                      NULL,    /* aIsFinishedRecovery */
                                      aEnv ) == STL_SUCCESS );

    STL_RING_FOREACH_ENTRY( &sRecoveryInfo->mDatafileList, sDatafileInfo )
    {
        sTbsArg.mTbsId = sDatafileInfo->mTbsId;
        sTbsArg.mDatafileId = sDatafileInfo->mDatafileId;
        sTbsArg.mReason = SMF_CHANGE_DATAFILE_HEADER_FOR_GENERAL;

        STL_TRY( knlAddEnvEvent( SML_EVENT_PAGE_FLUSH_TBS,
                                 NULL,
                                 (void*)&sTbsArg,
                                 STL_SIZEOF(smpFlushTbsArg),
                                 SML_PAGE_FLUSHER_ENV_ID,
                                 KNL_EVENT_WAIT_POLLING,
                                 STL_FALSE, /* aForceSuccess */
                                 KNL_ENV( aEnv ) ) == STL_SUCCESS );
        
        /**
         * SML_ERRCODE_TABLESPACE_OFFLINE_ABNORMALLY을 error로 처리한다.
         */
        STL_TRY( stlGetLastErrorCode( KNL_ERROR_STACK(aEnv) ) !=
                 SML_ERRCODE_TABLESPACE_OFFLINE_ABNORMALLY );
    }

    /**
     * Tablespace Page Recovery 진행 설정.
     */
    STL_RING_FOREACH_ENTRY( &sRecoveryInfo->mDatafileList, sDatafileInfo )
    {
        STL_TRY( smpDestroyCorruptedPageList( sDatafileInfo,
                                              aEnv ) == STL_SUCCESS );
        
        if( smrIsExistSameTbs4DatafileRecovery( sRecoveryInfo, sDatafileInfo ) == STL_TRUE )
        {
            continue;
        }

        SMF_SET_TBS_PROCEED_MEDIA_RECOVERY( sDatafileInfo->mTbsId, STL_FALSE );
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_OFFLINE )
    {
        smfGetTbsName( sDatafileInfo->mTbsId, sTbsName, aEnv );
        
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_NOT_OFFLINE_TABLESPACE,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sTbsName );
    }
    
    STL_FINISH;

    STL_RING_FOREACH_ENTRY( &sRecoveryInfo->mDatafileList, sDatafileInfo )
    {
        (void) smpDestroyCorruptedPageList( sDatafileInfo, aEnv );
        
        if( smrIsExistSameTbs4DatafileRecovery( sRecoveryInfo, sDatafileInfo )
            == STL_TRUE )
        {
            continue;
        }

        SMF_SET_TBS_PROCEED_MEDIA_RECOVERY( sDatafileInfo->mTbsId,
                                            STL_FALSE );

        if( SMF_IS_ONLINE_TBS( sDatafileInfo->mTbsId ) == STL_FALSE )
        {
            (void) smpShrinkCorruptedTbs( sDatafileInfo->mTbsId,
                                          aEnv );
        }
    }

    return STL_FAILURE;
}

/**
 * @brief Datafile Media Recovery: Redo 작업 중에 Rid가 Redoable인지 판단한다.
 * @param[in]     aRecoveryInfo  Datafile Recovery Information
 * @param[in]     aDataRid       Redo 대상인지 판단되는 Rid
 * @param[in]     aCurLsn        Log의 Lsn
 * @param[out]    aFound         Redo 대상인지 Flag
 * @param[in,out] aEnv           Enviornment Info
 * @remark Open phase 이면 tbs의 offline lsn까지만 유효하다.
 */ 
stlStatus smrIsRedoableRid4DatafileRecovery( smrMediaRecoveryInfo * aRecoveryInfo,
                                             smlRid               * aDataRid,
                                             smrLsn                 aCurLsn,
                                             stlBool              * aFound,
                                             smlEnv               * aEnv )
{
    smrRecoveryDatafileInfo * sDatafileInfo;
    smrLsn                    sOfflineLsn;
    stlBool                   sFoundLog = STL_FALSE;
    
    STL_RING_FOREACH_ENTRY( &aRecoveryInfo->mDatafileList, sDatafileInfo )
    {
        if( (aDataRid->mTbsId == sDatafileInfo->mTbsId) &&
            (SMP_DATAFILE_ID( aDataRid->mPageId ) == sDatafileInfo->mDatafileId) )
        {
            sOfflineLsn = smfGetOfflineLsn( sDatafileInfo->mTbsId );

            if( knlGetCurrStartupPhase() == KNL_STARTUP_PHASE_OPEN )
            {
                if( aCurLsn <= sOfflineLsn )
                {
                    sFoundLog = STL_TRUE;
                }
            }
            else
            {
                sFoundLog = STL_TRUE;
            }

            break;
        }
    }

    *aFound = sFoundLog;
    
    return STL_SUCCESS;
}

/**
 * @brief Datafile Recovery 수행시, DatafileInfo에 중복되는 Tablespace 검사.
 * @param[in] aRecoveryInfo  Media Recovery Info
 * @param[in] aDatafileInfo  Target Datafile Info
 */ 
stlBool smrIsExistSameTbs4DatafileRecovery( smrMediaRecoveryInfo    * aRecoveryInfo,
                                            smrRecoveryDatafileInfo * aDatafileInfo )
{
    smrRecoveryDatafileInfo * sTmpDatafileInfo;
    stlBool                   sExist = STL_FALSE;

    STL_RING_FOREACH_ENTRY( &aRecoveryInfo->mDatafileList, sTmpDatafileInfo )
    {
        if( sTmpDatafileInfo == aDatafileInfo )
        {
            break;
        }
        else
        {
            if( sTmpDatafileInfo->mTbsId == aDatafileInfo->mTbsId )
            {
                sExist = STL_TRUE;
                break;
            }
        }
        
        
    }

    return sExist;
}

/**
 * @brief Datafile Recovery: 상위 레이어에서 넘어온 DataRecoveryInfo를 MediaRecoveryInfo에 연결.
 * @param[in]  aRecoveryInfo  Qp Layer에서 넘어온 smlDatafileRecoveryInfo
 * @param[out] aMediaRecoveryInfo  SessionEnv에 연결되는 smrMediaRecoveryInfo
 * @param[in,out] aEnv  Environment
 */
stlStatus smrCreateMediaRecoveryDatafileInfo( smlDatafileRecoveryInfo * aRecoveryInfo,
                                              smrMediaRecoveryInfo    * aMediaRecoveryInfo,
                                              smlEnv                  * aEnv )
{
    smrRecoveryDatafileInfo * sDatafileInfo;
    stlInt32                  i;

    /* Recover할 Datafile을 저장할 List를 초기화한다. */
    STL_RING_INIT_HEADLINK( &aMediaRecoveryInfo->mDatafileList,
                            STL_OFFSETOF( smrRecoveryDatafileInfo, mDatafileLink ) );

    for( i = 0; i < aRecoveryInfo->mDatafileCount; i++ )
    {
        sDatafileInfo = &aRecoveryInfo->mDatafileInfo[i];

        /* Corrupted page를 저장할 List를 초기화한다. */
        STL_RING_INIT_HEADLINK( &sDatafileInfo->mCorruptedPageList,
                                STL_OFFSETOF( smrCorruptedPageInfo, mCorruptedPageLink ) );
        STL_RING_INIT_DATALINK( sDatafileInfo,
                                STL_OFFSETOF( smrRecoveryDatafileInfo, mDatafileLink ) );

        STL_RING_ADD_LAST( &aMediaRecoveryInfo->mDatafileList, sDatafileInfo );
    }
    
    return STL_SUCCESS;
}

/** @} */

/**
 * @addtogroup smrMediaRecoveryMgr
 * @{
 */

stlStatus smrGetMediaRecoveryBeginLsn( smrLsn   * aMinChkptLsn,
                                       smrLid   * aHintLid,
                                       smlEnv   * aEnv )
{
    smlTbsId              sTbsId;
    smlRecoveryObjectType sObjectType;

    sTbsId = SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, TbsId );

    sObjectType = smrGetRecoveryObjectType(aEnv);
    
    if( sObjectType == SML_RECOVERY_OBJECT_TYPE_DATAFILE )
    {
        STL_TRY( smrGetRestoredLsn( aHintLid,
                                    aMinChkptLsn,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( smrGetMinChkptLsn( sTbsId,
                                    aHintLid,
                                    aMinChkptLsn,
                                    aEnv )
                 == STL_SUCCESS );
    }
    
    /**
     * Datafile의 최소 CheckpointLsn과 controlfile에 기록된
     * 마지막 CheckpointLsn들 중 최소값부터 recovery를 시작한다.
     */
    if( *aMinChkptLsn > smrGetLastCheckpointLsn() )
    {
        smrGetConsistentChkptLsnAndLid( aMinChkptLsn,
                                        aHintLid );
    }

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief Backup 또는 offline datafile로부터 시스템을 정상상태로 회복한다.
 * @param[in] aUntilLsn Media Recovery를 수행할 마지막 Lsn
 * @param[out] aLastLsn 마지막 Media Recovery Redo Log의 Lsn
 * @param[out] aIsFinishedRecovery Media Recovery 완료 여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smrMediaRecoverInternal( smrLsn      aUntilLsn,
                                   smrLsn    * aLastLsn,
                                   stlBool   * aIsFinishedRecovery,
                                   smlEnv    * aEnv )
{
    stlChar         * sBuffer;
    smrLogBuffer    * sLogBuffer = NULL;
    smrLogCursor      sLogCursor;
    stlInt64          sBufferSize;
    smrLsn            sOldestLsn;
    smrLid            sRedoLid;
    smrLsn            sMinChkptLsn;
    smrLid            sHintLid;
    stlInt64          sFileSeqNo;
    stlInt16          sBlockSize;
    stlBool           sExistFile;

    /**
     * Archivelog Mode가 아니면 Media Recovery를 수행할 수 없다.
     */
    STL_TRY_THROW( smrGetArchivelogMode() == SML_ARCHIVELOG_MODE,
                   RAMP_ERR_CANNOT_MEDIA_RECOVER );

    if( smrGetMediaRecoveryPhase(aEnv) == SMR_RECOVERY_PHASE_NONE )
    {
        STL_TRY( knlLogMsg( NULL,
                            KNL_ENV( aEnv ),
                            KNL_LOG_LEVEL_INFO,
                            "[MEDIA RECOVERY] begin (%s, %ld)\n",
                            (smrGetRecoveryType(aEnv) == SML_RECOVERY_TYPE_COMPLETE_MEDIA_RECOVERY)?
                            "COMPLETE" : "INCOMPLETE",
                            smrGetUntilValue(aEnv) )
                 == STL_SUCCESS );

        smrSetMediaRecoveryPhase( SMR_RECOVERY_PHASE_BEGIN, aEnv );
    }

    sLogBuffer  = (smrLogBuffer *)SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, LogBuffer );
    sBufferSize = sLogBuffer->mBufferSize;
    sBuffer     = sLogBuffer->mBuffer;
    sBlockSize  = SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, BlockSize );

    if( smrGetMediaRecoveryPhase(aEnv) < SMR_RECOVERY_PHASE_REDO )
    {
        /**
         * Interactive incomplete media recovery가 begin된 후
         * 아직 Redo를 시작하지 못한 상태에서 입력받은 command가
         * END인 경우 media recovery 종료
         */
        if( smrGetImrOption(aEnv) == SML_IMR_OPTION_INTERACTIVE )
        {
            if( smrGetImrCondition(aEnv) == SML_IMR_CONDITION_END )
            {
                smrSetMediaRecoveryPhase( SMR_RECOVERY_PHASE_DONE, aEnv );
                STL_THROW( RAMP_END_REDO );
            }
        }

        /**
         * Interactive incomplete media recovery의 경우
         * recovery를 위한 첫번째 logfile을 입력받은 후 analysis를 수행한다.
         */
        STL_TRY( smrGetMediaRecoveryBeginLsn( &sMinChkptLsn,
                                              &sHintLid,
                                              aEnv ) == STL_SUCCESS );

        if( smrGetImrOption(aEnv) == SML_IMR_OPTION_INTERACTIVE )
        {
            STL_TRY( smrGetLogfile( sMinChkptLsn,
                                    SMR_FILE_SEQ_NO( &sHintLid ),
                                    &sExistFile,
                                    aEnv ) == STL_SUCCESS );

            STL_TRY_THROW( sExistFile == STL_TRUE, RAMP_SUCCESS );
        }

        /**
         * 1. Analysis Phase
         * - Build active transaction table
         */
        STL_TRY( smrAnalysis4MediaRecovery( &sLogCursor,
                                            sMinChkptLsn,
                                            sHintLid,
                                            &sOldestLsn,
                                            &sRedoLid,
                                            &sFileSeqNo,
                                            aEnv ) == STL_SUCCESS );

        /**
         * Analysis 후 Log buffer 다시 초기화
         */
        SMR_INIT_LOG_BUFFER( sLogBuffer,
                             sBufferSize,
                             sBuffer,
                             STL_FALSE, /* aIsShared */
                             sBlockSize );
        sLogBuffer->mFileSeqNo = -1;

        smrSetMediaRecoveryPhase( SMR_RECOVERY_PHASE_REDO, aEnv );

        STL_TRY( knlLogMsg( NULL,
                            KNL_ENV( aEnv ),
                            KNL_LOG_LEVEL_INFO,
                            "    [MEDIA RECOVERY REDO] ready to redo - start lid(%ld,%d,%d)"
                            ", start lsn(%ld)\n",
                            SMR_FILE_SEQ_NO(&sRedoLid),
                            SMR_BLOCK_SEQ_NO(&sRedoLid),
                            SMR_BLOCK_OFFSET(&sRedoLid),
                            sOldestLsn ) == STL_SUCCESS );

        /**
         * Prepare recovery
         */
        STL_TRY( smrPrepare4MediaRecovery( sRedoLid,
                                           sOldestLsn,
                                           sFileSeqNo,
                                           aEnv ) == STL_SUCCESS );

        *aLastLsn = sOldestLsn;
    }
    else
    {
        sOldestLsn = SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, CurLsn );
    }

    /**
     * 2. Redo Phase
     */
    if( smrProceedIncompleteMediaRecovery(aEnv) == STL_FALSE )
    {
        STL_TRY( smrRedo4MediaRecovery( sOldestLsn,
                                        aUntilLsn,
                                        aLastLsn,
                                        aEnv ) == STL_SUCCESS );

        smrSetMediaRecoveryPhase( SMR_RECOVERY_PHASE_DONE, aEnv );
    }
    else
    {
        if( smrGetImrCondition(aEnv) != SML_IMR_CONDITION_END )
        {
            STL_TRY( smrRedo4MediaRecovery( sOldestLsn,
                                            aUntilLsn,
                                            aLastLsn,
                                            aEnv ) == STL_SUCCESS );

        }

        /**
         * 3. Incomplete media recovery 경우만 rollback 수행
         */
        if( (smrGetRecoveryType(aEnv) == SML_RECOVERY_TYPE_INCOMPLETE_MEDIA_RECOVERY) &&
            ((smrGetImrOption(aEnv) != SML_IMR_OPTION_INTERACTIVE) ||
             (smrGetImrCondition(aEnv) == SML_IMR_CONDITION_END)) )
        {
            STL_TRY( smeBuildCachesAtStartup( aEnv ) == STL_SUCCESS );
            STL_TRY( smxlAttachUndoRelations( aEnv ) == STL_SUCCESS );

            smrSetMediaRecoveryPhase( SMR_RECOVERY_PHASE_UNDO, aEnv );

            STL_TRY( knlLogMsg( NULL,
                                KNL_ENV( aEnv ),
                                KNL_LOG_LEVEL_INFO,
                                "    [MEDIA RECOVERY UNDO] begin\n" )
                     == STL_SUCCESS );

            STL_TRY( smrRestartUndo( aEnv ) == STL_SUCCESS );

            STL_TRY( smqBuildCachesAtStartup( aEnv ) == STL_SUCCESS );
        
            STL_TRY( knlLogMsg( NULL,
                                KNL_ENV( aEnv ),
                                KNL_LOG_LEVEL_INFO,
                                "    [MEDIA RECOVERY UNDO] end\n" )
                     == STL_SUCCESS );

            /**
             * INCOMPLETE media recovery를 수행한 후 OPEN DATABASE 시
             * 반드시 RESETLOGS option으로 open해야 한다. 이를 위해
             * Controlfile에 INCOMPLETE media recovery를 설정한다.
             */
            gSmrWarmupEntry->mLogPersData.mNeedResetLogs = STL_TRUE;

            gSmrWarmupEntry->mLogPersData.mOldestLsn = smrGetSystemLsn();

            smfSetResetLogsScn( smxlGetSystemScn() );

            STL_TRY( knlLogMsg( NULL,
                                KNL_ENV( aEnv ),
                                KNL_LOG_LEVEL_INFO,
                                "[INCOMPLETE MEDIA RECOVERY] resetlogs lsn(%ld), scn(%ld)\n",
                                gSmrWarmupEntry->mLogPersData.mOldestLsn,
                                smfGetResetLogsScn() )
                     == STL_SUCCESS );

            smrSetMediaRecoveryPhase( SMR_RECOVERY_PHASE_DONE, aEnv );
        }
    }

    STL_RAMP( RAMP_END_REDO );

    if( smrGetMediaRecoveryPhase(aEnv) == SMR_RECOVERY_PHASE_DONE )
    {
        STL_TRY( knlLogMsg( NULL,
                            KNL_ENV( aEnv ),
                            KNL_LOG_LEVEL_INFO,
                            "[MEDIA RECOVERY] end\n" )
                 == STL_SUCCESS );

        KNL_BREAKPOINT( KNL_BREAKPOINT_SMRMEDIARECOVER_AFTER_RECOVERY,
                        (knlEnv*)aEnv );

        if( aIsFinishedRecovery != NULL )
        {
            *aIsFinishedRecovery = STL_TRUE;
        }

        STL_TRY( smfSaveCtrlFile( aEnv ) == STL_SUCCESS );
    }

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CANNOT_MEDIA_RECOVER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_NOT_ARCHIVELOG_MODE,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    if( (aIsFinishedRecovery == NULL) ||
        (*aIsFinishedRecovery == STL_TRUE) )
    {
        /**
         * Media Recovery 수행 실패
         */
        STL_TRY( knlLogMsg( NULL,
                            KNL_ENV( aEnv ),
                            KNL_LOG_LEVEL_INFO,
                            "[MEDIA RECOVERY] failed (%s)\n",
                            stlGetLastErrorMessage(KNL_ERROR_STACK(aEnv)) )
                 == STL_SUCCESS );
    }

    return STL_FAILURE;
}

/**
 * @brief Media recovery를 수행하기 위한 analysis phase
 * @param[in] aLogCursor Log cursor
 * @param[in] aMinChkptLsn Media recovery를 수행하기 위한 min chkpt log Lsn
 * @param[in] aHintLid Media recovery를 수행하기 위한 chkpt log를 찾기 위한 hint lid
 * @param[out] aOldestLsn Media recovery를 수행하기 위한 min log Lsn
 * @param[out] aRedoLid Media recovery를 수행하기 위한 min log Lid
 * @param[out] aFileSeqNo Redo를 시작할 logfile sequence no
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smrAnalysis4MediaRecovery( smrLogCursor   * aLogCursor,
                                     smrLsn           aMinChkptLsn,
                                     smrLid           aHintLid,
                                     smrLsn         * aOldestLsn,
                                     smrLid         * aRedoLid,
                                     stlInt64       * aFileSeqNo,
                                     smlEnv         * aEnv )
                              
{
    smrTrans           * sTransTable = NULL;
    smrLsn               sOldestLsn;
    smlScn               sSystemScn;
    smrLid               sRedoLid;
    stlInt64             sFileSeqNo;
    stlBool              sEndOfLog = STL_FALSE;
    stlBool              sIsChkptEnd = STL_FALSE;
    stlBool              sEndOfFile;
    stlInt32             sState = 0;
    stlInt32             i;
    knlRegionMark        sMemMark;
    
    STL_TRY_THROW( SMR_IS_VALID_LID( &gSmrWarmupEntry->mLogPersData.mChkptLid ),
                   RAMP_SUCCESS );

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "    [MEDIA RECOVERY] analysis begin\n" )
             == STL_SUCCESS );

    smrSetMediaRecoveryPhase( SMR_RECOVERY_PHASE_ANALYSIS, aEnv );

    /**
     * Media Recovery를 시작할 첫번째 checkpoint log를 읽어서
     * OldestLsn과 Lid, System Lsn을 구한다.
     */
    if( smrReadChkptLog4MediaRecovery( aHintLid,
                                       aMinChkptLsn,
                                       &sRedoLid,
                                       &sOldestLsn,
                                       &sSystemScn,
                                       &sFileSeqNo,
                                       aEnv ) != STL_SUCCESS )
    {
        /* Checkpoint log가 포함된 logfile을 찾지 못한 경우 Logfile 기반의
         * 불완전 복구라면 suggestion logfile warning을 push한다. */
        STL_TRY( smrGetImrOption(aEnv) == SML_IMR_OPTION_INTERACTIVE );

        STL_TRY( smrPushWarning4IncompleteRecovery( aMinChkptLsn,
                                                    SMR_FILE_SEQ_NO( &aHintLid ),
                                                    aEnv )
                 == STL_SUCCESS );

        STL_TRY( STL_FALSE );
    }

    *aOldestLsn = sOldestLsn;
    *aRedoLid = sRedoLid;
    *aFileSeqNo = sFileSeqNo;

    /**
     * Database에 대한 media recovery의 경우 system Scn을 설정한다.
     */
    if( smrGetTbsId4MediaRecovery(aEnv) == SML_INVALID_TBS_ID )
    {
        smxlSetSystemScn( sSystemScn );
    }

    /**
     * Media Recovery 용 transaction table 초기화
     */
    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                (SMXL_MAX_TRANS_TABLE_SIZE + 1) * STL_SIZEOF( smrTrans ),
                                (void**)&sTransTable,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    for( i = 0; i < SMXL_MAX_TRANS_TABLE_SIZE; i++ )
    {
        SMR_INIT_TRANS_TABLE( &sTransTable[i] );
    }

    STL_TRY( smrOpenLogCursor( sOldestLsn,
                               aLogCursor,
                               STL_FALSE, /* aIsForRecover */
                               aEnv ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( smrPrepare4MediaRecovery( sRedoLid,
                                       sOldestLsn,
                                       sFileSeqNo,
                                       aEnv ) == STL_SUCCESS );

    STL_TRY( smrReadNextLog4MediaRecovery( aLogCursor,
                                           &sEndOfLog,
                                           &sEndOfFile,
                                           aEnv ) == STL_SUCCESS );

    if( smrGetImrOption(aEnv) == SML_IMR_OPTION_INTERACTIVE )
    {
        STL_TRY_THROW( sEndOfFile == STL_FALSE, RAMP_SUCCESS );
    }

    while( sEndOfLog == STL_FALSE )
    {
        STL_TRY( smrAnalyzeLog( aLogCursor,
                                &sIsChkptEnd,
                                sTransTable,
                                aEnv ) == STL_SUCCESS );

        /**
         * Checkpoint End까지 분석한다.
         */
        if( sIsChkptEnd == STL_TRUE )
        {
            break;
        }
        
        STL_TRY( smrReadNextLog4MediaRecovery( aLogCursor,
                                               &sEndOfLog,
                                               &sEndOfFile,
                                               aEnv ) == STL_SUCCESS );

        if( smrGetImrOption(aEnv) == SML_IMR_OPTION_INTERACTIVE )
        {
            STL_TRY_THROW( sEndOfFile == STL_FALSE, RAMP_SUCCESS );
        }
    }

    /**
     * OldestLsn 당시 Transaction Table을 재구성한다.
     */
    for( i = 0; i < SMXL_MAX_TRANS_TABLE_SIZE; i++ )
    {
        if( (sTransTable[i].mTransId != SML_INVALID_TRANSID) &&
            (sTransTable[i].mBeginLsn == 0) )
        {
            if( smrProceedOnlineMediaRecovery( aEnv ) == STL_TRUE )
            {
                smxlBindTrans4MediaRecovery(
                    SMXL_MEDIA_RECOVERY_TRANS_ID(sTransTable[i].mTransId),
                    aEnv );
                smxlSetTransViewScn4MediaRecovery(
                    SMXL_MEDIA_RECOVERY_TRANS_ID(sTransTable[i].mTransId),
                    sTransTable[i].mTransViewScn,
                    aEnv );
                smxlSetUndoSegRid4MediaRecovery(
                    SMXL_MEDIA_RECOVERY_TRANS_ID(sTransTable[i].mTransId),
                    sTransTable[i].mUndoSegRid,
                    aEnv );
            }
            else
            {
                smxlBindTrans( sTransTable[i].mTransId );
                smxlSetTransViewScn( sTransTable[i].mTransId,
                                     sTransTable[i].mTransViewScn );
                smxlSetUndoSegRid( sTransTable[i].mTransId,
                                   sTransTable[i].mUndoSegRid );
                if( sTransTable[i].mState == SMXL_STATE_PREPARE )
                {
                    smxlSetTransState( sTransTable[i].mTransId,
                                       SMXL_STATE_PREPARE );
                }
                smxlSetRepreparable( sTransTable[i].mTransId,
                                     sTransTable[i].mRepreparable );
            }
        }
    }

    sState = 1;
    STL_TRY( smrCloseLogCursor( aLogCursor,
                                aEnv ) == STL_SUCCESS );

    /**
     * Media recovery를 위해 사용된 log buffer 메모리를 해제한다. 
     */
    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_TRUE, /* aFreeChunk */
                                       KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "    [MEDIA RECOVERY] analysis done\n" )
             == STL_SUCCESS );

    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    switch( sState )
    {
        case 2:
            (void)smrCloseLogCursor( aLogCursor, aEnv );
        case 1:
            (void) knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                             &sMemMark,
                                             STL_TRUE, /* aFreeChunk */
                                             KNL_ENV(aEnv) );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus smrPrepare4MediaRecovery( smrLid     aRedoLid,
                                    smrLsn     aOldestLsn,
                                    stlInt64   aFileSeqNo,
                                    smlEnv   * aEnv )
{
    smrLogBuffer   * sLogBuffer;
    stlInt64         sReadBytes;
    stlInt16         sBlockSize;
    smrLsn           sFrontLsn;
    smrLogBlockHdr   sLogBlockHdr;
    stlInt32         sLeftLogCount = 0;
    stlInt16         sBlockOffset;
    smrLid           sRedoLid;

    sLogBuffer = (smrLogBuffer *)SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, LogBuffer );

    sLogBuffer->mEndOfFile = STL_FALSE;
    sLogBuffer->mEndOfLog = STL_FALSE;
    
    /**
     * Media recover를 위해 log file을 읽어서 buffer에 채운다.
     */
    STL_TRY( smrReadFileBlocks4MediaRecovery( &aRedoLid,
                                              SMR_WRAP_NO( aFileSeqNo ),
                                              sLogBuffer->mBuffer,
                                              &sReadBytes,
                                              &sLogBuffer->mEndOfFile,
                                              &sLogBuffer->mEndOfLog,
                                              aEnv ) == STL_SUCCESS );

    /* Interactive incomplete recovery시 END 이면 종료한다. */
    if( smrGetImrOption(aEnv) == SML_IMR_OPTION_INTERACTIVE )
    {
        STL_TRY_THROW( smrGetImrCondition(aEnv) != SML_IMR_CONDITION_END,
                       RAMP_SUCCESS );
    }

    sBlockSize = SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, BlockSize );

    if( sReadBytes > 0 )
    {
        SMR_READ_LOG_BLOCK_HDR( &sLogBlockHdr,
                                sLogBuffer->mBuffer + (sReadBytes - sBlockSize) );

        STL_TRY( smrFindLogInBlock( sLogBuffer->mBuffer,
                                    aOldestLsn,
                                    &sBlockOffset,
                                    &sFrontLsn,
                                    &sLeftLogCount,
                                    aEnv ) == STL_SUCCESS );

        SMR_MAKE_LID( &sRedoLid,
                      SMR_FILE_SEQ_NO( &aRedoLid ),
                      SMR_BLOCK_SEQ_NO( &aRedoLid ),
                      sBlockOffset );

        sLogBuffer->mRearSbsn = sReadBytes / sBlockSize;
        sLogBuffer->mFrontSbsn = 0;
        sLogBuffer->mLeftLogCountInBlock = sLeftLogCount;
        sLogBuffer->mFrontLsn = sFrontLsn;
        sLogBuffer->mRearLsn = sLogBlockHdr.mLsn;
    }
    else
    {
        SMR_MAKE_LID( &sRedoLid,
                      SMR_FILE_SEQ_NO( &aRedoLid ),
                      SMR_BLOCK_SEQ_NO( &aRedoLid ),
                      SMR_BLOCK_HDR_SIZE );

        sLogBuffer->mFrontSbsn = -1;
        sLogBuffer->mRearSbsn = 0;
        sLogBuffer->mLeftLogCountInBlock = 0;
        sLogBuffer->mFrontLsn = 0;
        sLogBuffer->mRearLsn = 0;
    }

    sLogBuffer->mBlockOffset = SMR_BLOCK_OFFSET( &sRedoLid );
    sLogBuffer->mFrontFileBlockSeqNo = SMR_BLOCK_SEQ_NO( &sRedoLid );
    sLogBuffer->mRearFileBlockSeqNo = sLogBuffer->mFrontFileBlockSeqNo + (sReadBytes / sBlockSize);
    sLogBuffer->mFileSeqNo = SMR_FILE_SEQ_NO( &sRedoLid );
    sLogBuffer->mRearLid = sRedoLid;

    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Media recover를 위해 첫번째 log file block을 찾는다.
 * @param[in] aChkptLid Media recover 수행을 위한 checkpoint Lid
 * @param[in] aMinChkptLsn Media recover 수행을 위한 checkpoint Lsn
 * @param[in] aBuffer Log를 읽기위한 buffer block
 * @param[in] aBufferSize Log를 읽기위한 buffer block의 크기
 * @param[out] aFileSeqNo Media recover를 수행할 첫번째 Logfile Seq No
 * @param[out] aRedoLid Media recovery를 수행하기 위한 min log Lid
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smrFindFileBlock4MediaRecovery( smrLid        aChkptLid,
                                          smrLsn        aMinChkptLsn,
                                          stlChar     * aBuffer,
                                          stlInt64      aBufferSize,
                                          stlInt64    * aFileSeqNo,
                                          smrLid      * aRedoLid,
                                          smlEnv      * aEnv )
{
    smrLogStream   * sLogStream;
    smrLogGroup    * sLogGroup = NULL;
    stlInt64         sFileSeqNo;
    stlBool          sFound = STL_FALSE;
    stlBool          sExist = STL_FALSE;

    /**
     * Media recover를 시작할 log group을 찾는다.
     */
    sLogStream = &gSmrWarmupEntry->mLogStream;

    /**
     * Interactive incomplete media recovery이 아닌 경우
     * online logfile에서 첫번째 log file block을 찾는다.
     */
    if( smrGetImrOption(aEnv) != SML_IMR_OPTION_INTERACTIVE )
    {
        /**
         * Hint Lid가 invalid Lid이거나 archiving되지 않은 logfile의 경우
         * online log group에서 media recovery 시작 log를 찾는다.
         */
        if( (SMR_IS_INVALID_LID( &aChkptLid ) == STL_TRUE) ||
            gSmrWarmupEntry->mLogPersData.mLastInactiveLfsn < SMR_FILE_SEQ_NO( &aChkptLid ) )
        {
            STL_TRY( smrFindLogGroupByLsn( sLogStream,
                                           aMinChkptLsn,
                                           &sFileSeqNo,
                                           &sLogGroup ) == STL_SUCCESS );
        }

        /**
         * media recover를 위한 첫번째 log가 online redo log에 존재하는 경우
         */
        if( sLogGroup != NULL )
        {
            STL_TRY( smrFindFileBlockByLsn( sLogGroup,
                                            aMinChkptLsn,
                                            aBuffer,
                                            aBufferSize,
                                            aRedoLid,
                                            &sFound,
                                            aEnv ) == STL_SUCCESS );
        }
    }
    else
    {
        /* if( smrGetImrOption() == SML_IMR_OPTION_INTERACTIVE ) */
        /* Interactive incomplete recovery시 END 이면 종료한다. */
        STL_TRY_THROW( smrGetImrCondition(aEnv) != SML_IMR_CONDITION_END,
                       RAMP_SUCCESS );
    }
    
    /**
     * Media recover를 시작할 log가 online redo log에 없을 경우
     * archive logfile에서 찾는다.
     */
    if( sFound == STL_FALSE )
    {
        if( SMR_IS_INVALID_LID( &aChkptLid ) == STL_TRUE )
        {
            sFileSeqNo = gSmrWarmupEntry->mLogPersData.mLastInactiveLfsn;
        }
        else
        {
            sFileSeqNo = SMR_FILE_SEQ_NO( &aChkptLid );
        }

        STL_TRY( smrFindArchiveFileBlockByLsn(
                     sFileSeqNo,
                     aMinChkptLsn,
                     aBuffer,
                     aBufferSize,
                     aRedoLid,
                     &sExist,
                     aEnv ) == STL_SUCCESS );

        STL_TRY_THROW( sExist == STL_TRUE, RAMP_ERR_NOT_APPROPRIATE_LOGFILE );
    }

    *aFileSeqNo = sFileSeqNo;

    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_APPROPRIATE_LOGFILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_NOT_APPROPRIATE_LOGFILE,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      SMR_MEDIA_RECOVERY_GET_FIELD(aEnv, LogfileName) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Media recover를 위해 log file을 log buffer에 복사한다.
 * @param[in] aLid read할 log의 lid
 * @param[in] aWrapNo 현재 logfile의 WrapNo
 * @param[in] aBuffer 읽은 Log를 채울 공간
 * @param[out] aReadBytes 
 * @param[out] aEndOfFile
 * @param[out] aEndOfLog
 * @param[in,out] aEnv 
 */
stlStatus smrReadFileBlocks4MediaRecovery( smrLid         * aLid,
                                           stlInt16         aWrapNo,
                                           stlChar        * aBuffer,
                                           stlInt64       * aReadBytes,
                                           stlBool        * aEndOfFile,
                                           stlBool        * aEndOfLog,
                                           smlEnv         * aEnv )
{
    stlBool        sIsValid;
    stlBool        sIsExist;
    stlBool        sSwitched = STL_FALSE;
    smrLogBuffer * sLogBuffer;

    while( STL_TRUE )
    {
        /**
         * Read하려는 valid archive logfile을 구한다.
         */
        if( smrGetImrOption(aEnv) != SML_IMR_OPTION_INTERACTIVE )
        {
            STL_TRY( smrGetValidArchiveLogfileBySeqNo(
                         SMR_FILE_SEQ_NO( aLid ),
                         SMR_MEDIA_RECOVERY_GET_FIELD(aEnv, LogfileName),
                         &sIsValid,
                         &sIsExist,
                         aEnv ) == STL_SUCCESS );
        }

        if( sSwitched == STL_TRUE )
        {
            /**
             * Online logfile이 archive되어 switch 되었는데
             * archive logfile이 valid하지 않으면 exception
             */
            STL_TRY_THROW( (sIsExist == STL_TRUE) && (sIsValid == STL_TRUE),
                           RAMP_ERR_ARCHIVELOG_FILE_NOT_EXIST );

            /**
             * Switch되었으면 현재 recovery중인 logfile seqNo를 reset한다.
             */
            sLogBuffer = (smrLogBuffer *)SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, LogBuffer );
            sLogBuffer->mFileSeqNo = -1;
        }

        if( (sIsValid == STL_TRUE) ||
            (smrGetImrOption(aEnv) == SML_IMR_OPTION_INTERACTIVE) )
        {
            /**
             * Archive logfile를 읽는다.
             */
            STL_TRY( smrReadArchiveFileBlocks( aLid,
                                               aBuffer,
                                               aReadBytes,
                                               aEndOfFile,
                                               aEndOfLog,
                                               aEnv ) == STL_SUCCESS );
        }
        else
        {
            /**
             * Online logfile에서 media recover를 위한 log를 읽는다.
             */
            STL_TRY( smrReadOnlineFileBlocks( aLid,
                                              aBuffer,
                                              aWrapNo,
                                              aReadBytes,
                                              &sSwitched,
                                              aEndOfFile,
                                              aEndOfLog,
                                              aEnv ) == STL_SUCCESS );
        }

        /**
         * 읽은 Log가 존재한다면 처리해야 한다.
         */

        if( *aReadBytes > 0 )
        {
            break;
        }
        
        if( *aEndOfLog == STL_TRUE )
        {
            break;
        }
        
        if( *aEndOfFile == STL_TRUE )
        {
            break;
        }
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_ARCHIVELOG_FILE_NOT_EXIST )
    {
        (void)smrMakeArchiveLogfileName( SMR_FILE_SEQ_NO( aLid ),
                                         0,
                                         SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, LogfileName ),
                                         aEnv );

        if( sIsExist == STL_FALSE )
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          SML_ERRCODE_LOGFILE_NOT_EXIST,
                          NULL,
                          KNL_ERROR_STACK(aEnv),
                          SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, LogfileName ) );
        }
        else
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          SML_ERRCODE_LOGFILE_NOT_VALID,
                          "logfile header was corrupted",
                          KNL_ERROR_STACK(aEnv),
                          SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, LogfileName ) );
        }
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrReadNextLog4MediaRecovery( smrLogCursor   * aLogCursor,
                                        stlBool        * aEndOfLog,
                                        stlBool        * aEndOfFile,
                                        smlEnv         * aEnv )
{
    STL_TRY( smrReadLog4MediaRecovery( &aLogCursor->mLogHdr,
                                       aLogCursor->mLogBody,
                                       aEndOfLog,
                                       aEndOfFile,
                                       aEnv ) == STL_SUCCESS );

    if( *aEndOfFile == STL_FALSE )
    {
        aLogCursor->mCurLsn = aLogCursor->mLogHdr.mLsn;
    }

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Media Recovery를 위해 Log를 하나 읽는다.
 *
 */
stlStatus smrReadLog4MediaRecovery( smrLogHdr      * aLogHdr,
                                    stlChar        * aLogBody,
                                    stlBool        * aEndOfLog,
                                    stlBool        * aNeedValidLogfile,
                                    smlEnv         * aEnv )
{
    smrLogBuffer   * sLogBuffer;
    stlInt32         sTotalReadBytes;
    stlInt32         sReadBytes;
    stlChar        * sBlock;
    smrLogBlockHdr   sLogBlockHdr;
    stlInt16         sBlockSize;

    *aEndOfLog = STL_FALSE;
    *aNeedValidLogfile = STL_FALSE;
    sLogBuffer = (smrLogBuffer *)SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, LogBuffer );
    sBlockSize = SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, BlockSize );

    STL_RAMP( RAMP_RETRY );

    sTotalReadBytes = 0;

    if( sLogBuffer->mLeftLogCountInBlock == 0 )
    {
        STL_TRY( smrReadLogFromBlock4MediaRecovery( STL_TRUE,
                                                    aEndOfLog,
                                                    aNeedValidLogfile,
                                                    aEnv ) == STL_SUCCESS );

        if( smrGetImrOption(aEnv) == SML_IMR_OPTION_INTERACTIVE )
        {
            STL_TRY_THROW( *aNeedValidLogfile == STL_FALSE, RAMP_SUCCESS );
        }

        /**
         * 더이상 처리할 log가 없으면 종료
         */
        STL_TRY_THROW( *aEndOfLog == STL_FALSE, RAMP_SUCCESS );

        /**
         * Ineractive incomplete media recovery 시 END를 입력받았을 경우 종료
         */
        if( smrGetImrOption(aEnv) == SML_IMR_OPTION_INTERACTIVE )
        {
            STL_TRY_THROW( smrGetImrCondition(aEnv) != SML_IMR_CONDITION_END,
                           RAMP_SUCCESS );
        }
    }

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
        STL_TRY( smrReadLogFromBlock4MediaRecovery( STL_FALSE,
                                                    aEndOfLog,
                                                    aNeedValidLogfile,
                                                    aEnv ) == STL_SUCCESS );

        if( smrGetImrOption(aEnv) == SML_IMR_OPTION_INTERACTIVE )
        {
            STL_TRY_THROW( *aNeedValidLogfile == STL_FALSE, RAMP_SUCCESS );
        }

        /**
         * 더이상 처리할 log가 없거나 ineractive incomplete media recovery 시
         * END를 입력받았을 경우 종료
         */
        if( *aEndOfLog != STL_TRUE )
        {
            /* Interactive incomplete recovery시 END 이면 종료한다. */
            if( smrGetImrOption(aEnv) == SML_IMR_OPTION_INTERACTIVE )
            {
                STL_TRY_THROW( smrGetImrCondition(aEnv) != SML_IMR_CONDITION_END,
                               RAMP_SUCCESS );
            }
        }

        sBlock = sLogBuffer->mBuffer + (SMR_FRONT_POS(sLogBuffer) * sBlockSize);
        SMR_READ_LOG_BLOCK_HDR( &sLogBlockHdr, sBlock );

        /**
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

/**
 * @brief Media Recovery를 위해 Log block에서 log를 가져온다.
 * @param[in] aReadLogHdr 
 * @param[out] aEndOfLog  마지막 log 여부
 * @param[out] aNeedValidLogfile Media recovery를 위한 valid logfile 필요여부
 * @param[in,out] aEnv Environment
 */
stlStatus smrReadLogFromBlock4MediaRecovery( stlBool          aReadLogHdr,
                                             stlBool        * aEndOfLog,
                                             stlBool        * aNeedValidLogfile,
                                             smlEnv         * aEnv )
{
    smrLogBuffer   * sLogBuffer;
    stlInt64         sReadBufferBytes;
    smrLid           sLid;
    stlChar        * sBlock;
    void           * sBuffer;
    stlInt64         sFileSeqNo;
    smrLogBlockHdr   sLogBlockHdr;
    stlInt16         sBlockSize;
    stlBool          sExistFile;

    *aNeedValidLogfile = STL_TRUE;
    *aEndOfLog = STL_FALSE;
    sLogBuffer = (smrLogBuffer *)SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, LogBuffer );
    sBlockSize = SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, BlockSize );

    while( 1 )
    {
        /**
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
            
            /**
             * Current Logfile을 끝까지 다 읽었으면 다음 Logfile을
             * 읽기 위해 fileSeqNo를 증가하고, blockSeqNo들은 초기화 한다.
             */
            if( sLogBuffer->mEndOfFile == STL_TRUE )
            {
                sLogBuffer->mLeftLogCountInBlock = 0;
                sLogBuffer->mBlockOffset = SMR_BLOCK_HDR_SIZE;
                sLogBuffer->mRearFileBlockSeqNo = 0;
                sLogBuffer->mFrontFileBlockSeqNo = -1;

                sFileSeqNo = sLogBuffer->mFileSeqNo + 1;

                /**
                 * Logfile단위의 불완전 복구이면 다음 logfile을 입력받는다.
                 */
                if( smrGetImrOption(aEnv) == SML_IMR_OPTION_INTERACTIVE )
                {
                    STL_TRY( smrGetLogfile( sLogBuffer->mRearLsn + 1,
                                            sFileSeqNo,
                                            &sExistFile,
                                            aEnv ) == STL_SUCCESS );

                    STL_TRY_THROW( sExistFile == STL_TRUE, RAMP_SUCCESS );
                    sLogBuffer->mEndOfFile = STL_FALSE;
                }
            }
            else
            {
                sFileSeqNo = sLogBuffer->mFileSeqNo;
            }

            SMR_MAKE_LID( &sLid,
                          sFileSeqNo,
                          sLogBuffer->mFrontFileBlockSeqNo + 1,
                          0 );

            sLogBuffer->mRearSbsn = 0;
            sLogBuffer->mFrontSbsn = -1;
            sBuffer = sLogBuffer->mBuffer + (SMR_REAR_POS(sLogBuffer) * sBlockSize);

            /**
             * Logfile을 읽어서 log buffer를 채운다.
             */
            STL_TRY( smrReadFileBlocks4MediaRecovery( &sLid,
                                                      SMR_WRAP_NO( sFileSeqNo ),
                                                      sBuffer,
                                                      &sReadBufferBytes,
                                                      &sLogBuffer->mEndOfFile,
                                                      &sLogBuffer->mEndOfLog,
                                                      aEnv ) == STL_SUCCESS );

            /* Interactive incomplete recovery시 END 이면 종료한다. */
            if( smrGetImrOption(aEnv) == SML_IMR_OPTION_INTERACTIVE )
            {
                STL_TRY_THROW( smrGetImrCondition(aEnv) != SML_IMR_CONDITION_END,
                               RAMP_SUCCESS );
            }

            /*
             * 적어도 하나의 로그 블록을 읽었다면
             */
            if( sReadBufferBytes != 0 )
            {
                sLogBuffer->mRearFileBlockSeqNo = (sLogBuffer->mFrontFileBlockSeqNo +
                                                   (sReadBufferBytes / sBlockSize));

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

    *aNeedValidLogfile = STL_FALSE;

    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Datafile을 redo log(archive, online redo log file)을 이용하여 복구한다.
 * @param[in]     aOldestLsn Media recovery를 수행하기 위한 min log Lsn
 * @param[in]     aUntilLsn Media Recovery를 위한 마지막 Lsn
 * @param[out]    aLastLsn Media Recovery Redo를 수행한 마지막 Lsn
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smrRedo4MediaRecovery( smrLsn     aOldestLsn,
                                 smrLsn     aUntilLsn,
                                 smrLsn   * aLastLsn,
                                 smlEnv   * aEnv )
{
    stlBool                sEndOfLog;
    stlBool                sEndOfFile;
    stlInt64               sCurFileSeqNo = -1;
    smrLogBuffer         * sLogBuffer;
    smrLogCursor           sLogCursor;
    stlInt32               sState = 0;
    smrMediaRecoveryInfo * sMediaRecoveryInfo;
    
    sLogBuffer = (smrLogBuffer *)SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, LogBuffer );

    sMediaRecoveryInfo = SMR_GET_MEDIA_RECOVERY_INFO( aEnv );
    
    STL_TRY( smrOpenLogCursor( aOldestLsn,
                               &sLogCursor,
                               (smrGetRecoveryType(aEnv) == SML_RECOVERY_TYPE_INCOMPLETE_MEDIA_RECOVERY),
                               aEnv ) == STL_SUCCESS );
    sState = 1;

    while( 1 )
    {
        /**
         * Log cursor를 이용하여 Log를 하나씩 가져온다.
         */
        STL_TRY( smrReadNextLog4MediaRecovery( &sLogCursor,
                                               &sEndOfLog,
                                               &sEndOfFile,
                                               aEnv ) == STL_SUCCESS );

        /**
         * Logfile단위의 불완전 복구의 경우, logfile 하나를 redo한 후
         * 다음 logfile에 대한 사용자의 입력이 필요하면 다음 구문에서 수행한다.
         */
        if( smrGetImrOption(aEnv) == SML_IMR_OPTION_INTERACTIVE )
        {
            STL_TRY_THROW( sEndOfFile == STL_FALSE, RAMP_NEED_NEXT_LOGFILE );
        }

        if( smrIsRedoFinished(&sLogCursor, aUntilLsn, aEnv) == STL_TRUE )
        {
            break;
        }

        if( sEndOfLog == STL_TRUE )
        {
            break;
        }

        if( smrIsCheckpointLog( &sLogCursor, aEnv ) == STL_TRUE )
        {
            *aLastLsn = sLogCursor.mCurLsn;
            sMediaRecoveryInfo->mLastChkptLsn = sLogCursor.mCurLsn;

            SMR_MAKE_LID( &sMediaRecoveryInfo->mLastChkptLid,
                          sLogBuffer->mFileSeqNo,
                          sLogBuffer->mFrontFileBlockSeqNo,
                          sLogBuffer->mBlockOffset );
        }

        /**
         * Media Recovery Redo
         */
        STL_TRY( smrRedo( &sLogCursor,
                          aEnv ) == STL_SUCCESS );

        /**
         * Redo를 수행하는 중에 logfile이 바뀔 때마다 logfile이름을 trace log에 기록한다.
         */
        if( smrIsLogfileCrossed( sCurFileSeqNo, aEnv ) == STL_TRUE )
        {
            sCurFileSeqNo = sLogBuffer->mFileSeqNo;

            STL_TRY( knlLogMsg( NULL,
                                KNL_ENV( aEnv ),
                                KNL_LOG_LEVEL_INFO,
                                "    [MEDIA RECOVERY REDO] redo has performing - "
                                "logfile(%s), lsn(%ld)\n",
                                SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, LogfileName ),
                                sLogCursor.mCurLsn )
                     == STL_SUCCESS );
        }
    }

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "    [MEDIA RECOVERY REDO] redo end - lsn(%ld)\n",
                        sLogCursor.mCurLsn ) == STL_SUCCESS );

    /**
     * 불완전 복구의 경우 마지막으로 redo된 Lsn보다 datafile의 checkpoint Lsn이
     * 큰 값이 존재하면 안된다.
     */
    if( smrGetRecoveryType(aEnv) == SML_RECOVERY_TYPE_INCOMPLETE_MEDIA_RECOVERY )
    {
        STL_TRY( smfCheckDatafile4IncompleteMediaRecovery(
                     sLogCursor.mCurLsn,
                     aEnv ) == STL_SUCCESS );
    }

    STL_RAMP( RAMP_NEED_NEXT_LOGFILE );

    SMR_MEDIA_RECOVERY_SET_FIELD( aEnv, CurLsn, sLogCursor.mCurLsn );

    sState = 0;
    STL_TRY( smrCloseLogCursor( &sLogCursor,
                                aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState > 0 )
    {
        SMR_MEDIA_RECOVERY_SET_FIELD( aEnv, CurLsn, sLogCursor.mCurLsn );
        (void)smrCloseLogCursor( &sLogCursor, aEnv );
    }

    return STL_FAILURE;
}

stlBool smrIsRedoFinished( smrLogCursor * aLogCursor,
                           smrLsn         aUntilLsn,
                           smlEnv       * aEnv )
{
    /**
     * 불완전 복구를 위한 Until 조건까지 Redo가 완료되었으면 종료
     */
    if( smrIsIncompleteRecoveryFinished( aLogCursor, aEnv ) == STL_TRUE )
    {
        return STL_TRUE;
    }

    /**
     * Service 중에 tablespace media recovery 수행 시 tablespace는
     * offline되어 있어야 하고, 이때 offline 시점까지만 복구한다.
     */
    if( (SMR_IS_VALID_LOG_LSN( aUntilLsn ) == STL_TRUE) &&
        (aLogCursor->mCurLsn > aUntilLsn) )
    {
        return STL_TRUE;
    }

    return STL_FALSE;
}

stlBool smrIsIncompleteRecoveryFinished( smrLogCursor  * aLogCursor,
                                         smlEnv        * aEnv )
{
    stlBool   sIsFinished = STL_FALSE;

    switch( smrGetImrOption(aEnv) )
    {
        case SML_IMR_OPTION_BATCH:
            switch( smrGetImrCondition(aEnv) )
            {
                case SML_IMR_CONDITION_TIME:
                    break;
                case SML_IMR_CONDITION_CHANGE:
                    if( aLogCursor->mCurLsn > smrGetUntilValue(aEnv) )
                    {
                        sIsFinished = STL_TRUE;
                    }
                    break;
                default:
                    break;
            }
            break;
        case SML_IMR_OPTION_INTERACTIVE:
            if( smrGetImrCondition(aEnv) == SML_IMR_CONDITION_END )
            {
                sIsFinished = STL_TRUE;
            }
            break;
        default:
            break;
    }

    return sIsFinished;
}

smlTbsId smrGetTbsId4MediaRecovery( smlEnv * aEnv )
{
    return SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, TbsId );
}

/**
 * @brief Media Recovery를 위해 Checkpoint Log를 읽는다.
 * @param[in] aChkptLid Checkpoint Lid
 * @param[in] aChkptLsn Checkpoint Lsn
 * @param[out] aHintRedoLid Oldest Lsn을 위한 hint Lid
 * @param[out] aOldestLsn Oldest Lsn
 * @param[out] aSystemScn Checkpoint Log에 기록된 System SCN
 * @param[out] aFileSeqNo Logfile sequence no
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smrReadChkptLog4MediaRecovery( smrLid     aChkptLid,
                                         smrLsn     aChkptLsn,
                                         smrLid   * aHintRedoLid,
                                         smrLsn   * aOldestLsn,
                                         smlScn   * aSystemScn,
                                         stlInt64 * aFileSeqNo,
                                         smlEnv   * aEnv )
{
    smrLogBuffer   * sLogBuffer;
    stlChar        * sBlockBuffer;
    stlChar          sLogBody[SMR_MAX_LOGPIECE_SIZE];
    smrLogHdr        sLogHdr;
    stlInt64         sFileSeqNo = SMR_INVALID_LOG_FILE_SEQ_NO;
    stlInt16         sBlockSize;
    stlInt16         sBlockOffset = 0;
    stlInt32         sBodyOffset = 0;
    stlInt32         sTotalReadBytes = 0;
    stlInt64         sReadBytes;
    smrLogPieceHdr   sLogPieceHdr;
    stlBool          sEndOfFile;
    stlBool          sEndOfLog;
    smrLogBlockHdr   sLogBlockHdr;

    sLogBuffer = (smrLogBuffer *)SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, LogBuffer );
    sBlockBuffer = sLogBuffer->mBuffer;
    sBlockSize = SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, BlockSize );

    /**
     * Checkpoint Lid, Checkpoint Lsn을 이용해서 checkpoint log를 찾는다.
     */
    STL_TRY( smrFindFileBlock4MediaRecovery( aChkptLid,
                                             aChkptLsn,
                                             sBlockBuffer,
                                             sLogBuffer->mBufferSize,
                                             &sFileSeqNo,
                                             aHintRedoLid,
                                             aEnv ) == STL_SUCCESS );

    /**
     * Checkpoint log를 읽는다.
     */
    STL_TRY( smrReadFileBlocks4MediaRecovery( &aChkptLid,
                                              SMR_WRAP_NO( sFileSeqNo ),
                                              sBlockBuffer,
                                              &sReadBytes,
                                              &sEndOfFile,
                                              &sEndOfLog,
                                              aEnv ) == STL_SUCCESS );

    SMR_READ_LOG_HDR( &sLogHdr, sBlockBuffer + SMR_BLOCK_OFFSET( &aChkptLid ) );
    sBlockOffset = SMR_LOG_HDR_SIZE + SMR_BLOCK_OFFSET( &aChkptLid );
    SMR_READ_LOG_BLOCK_HDR( &sLogBlockHdr, sBlockBuffer );

    STL_DASSERT( SMR_IS_VALID_LOG_LSN( sLogBlockHdr.mLsn ) );

    STL_TRY_THROW( sLogBlockHdr.mFirstLogOffset <= SMR_BLOCK_OFFSET( &aChkptLid ),
                   RAMP_ERR_INTERNAL );

    sTotalReadBytes = 0;

    /*
     * Checkpoint Log Body를 구성한다.
     */
    while( sTotalReadBytes < sLogHdr.mSize )
    {
        if( sBlockOffset >= sBlockSize )
        {
            sBlockOffset = SMR_BLOCK_HDR_SIZE;
            sBlockBuffer += sBlockSize;
        }

        if( (sLogHdr.mSize - sTotalReadBytes) >
            (sBlockSize - sBlockOffset) )
        {
            sReadBytes = sBlockSize - sBlockOffset;
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
    *aFileSeqNo = sFileSeqNo;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INTERNAL,
                      "- smrReadChkptLog4MediaRecovery()",
                      KNL_ERROR_STACK(aEnv) );

        KNL_SYSTEM_FATAL_DASSERT( STL_FALSE, KNL_ENV(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Datafile header에 기록된 checkpoint Lsn 중 최소값을 가져온다.
 * @param[in]  aTbsId 최소 checkpoint Lsn을 구하는 target tablespace Id(INVALID_ID이면 전체 DB)
 * @param[out] aChkptLid checkpoint Lid
 * @param[out] aMinChkptLsn min checkpoint Lsn
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smrGetMinChkptLsn( smlTbsId    aTbsId,
                             smrLid    * aChkptLid,
                             smrLsn    * aMinChkptLsn,
                             smlEnv    * aEnv )
{
    if( aTbsId == SML_INVALID_TBS_ID )
    {
        STL_TRY( smfGetMinChkptLsn( aChkptLid,
                                    aMinChkptLsn,
                                    aEnv ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( smfGetTbsMinChkptLsn( aTbsId,
                                       aChkptLid,
                                       aMinChkptLsn,
                                       aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Datafile Recovery 중 Restore한 Checkpoint Lsn 중 최소값을 가져온다.
 * @param[out]    aChkptLid      checkpoint Lid
 * @param[out]    aMinChkptLsn   min checkpoint Lsn
 * @param[in,out] aEnv           Environment 구조체
 */
stlStatus smrGetRestoredLsn( smrLid    * aChkptLid,
                             smrLsn    * aMinChkptLsn,
                             smlEnv    * aEnv )
{
    smrMediaRecoveryInfo * sRecoveryInfo = SMR_GET_MEDIA_RECOVERY_INFO( aEnv );

    STL_ASSERT( sRecoveryInfo != NULL );
    STL_ASSERT( STL_RING_IS_EMPTY( &sRecoveryInfo->mDatafileList ) != STL_TRUE );

    *aMinChkptLsn = SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, RestoredChkptLsn );
    *aChkptLid    = SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, RestoredChkptLid );

    return STL_SUCCESS;
}

inline stlUInt8 smrGetImrOption( smlEnv  * aEnv )
{
    smrMediaRecoveryInfo * sRecoveryInfo = SMR_GET_MEDIA_RECOVERY_INFO( aEnv );

    return sRecoveryInfo->mImrOption;
}

inline stlBool smrGetRecoveryType( smlEnv  * aEnv )
{
    smrMediaRecoveryInfo * sRecoveryInfo = SMR_GET_MEDIA_RECOVERY_INFO( aEnv );

    return sRecoveryInfo->mMediaRecoveryType;
}

inline stlUInt8 smrGetRecoveryObjectType( smlEnv  * aEnv )
{
    smrMediaRecoveryInfo * sRecoveryInfo = SMR_GET_MEDIA_RECOVERY_INFO( aEnv );

    return sRecoveryInfo->mObjectType;
}

inline stlBool smrProceedIncompleteMediaRecovery( smlEnv  * aEnv )
{
    smrMediaRecoveryInfo * sRecoveryInfo = SMR_GET_MEDIA_RECOVERY_INFO( aEnv );

    if( sRecoveryInfo != NULL )
    {
        return (sRecoveryInfo->mMediaRecoveryType == SML_RECOVERY_TYPE_INCOMPLETE_MEDIA_RECOVERY);
    }

    return STL_FALSE;
}

stlStatus smrInitRecoveryContextAtMount( smlMediaRecoveryInfo * aRecoveryInfo,
                                         smlEnv               * aEnv )
{
    smrMediaRecoveryInfo  * sRecoveryInfo;
    smxlTrans             * sOrgTransTable = NULL;
    smrLogBuffer          * sLogBuffer = NULL;
    stlChar               * sBuffer;
    stlInt64                sBufferSize;
    stlInt32                sOrgTransTableSize;
    stlInt32                sState = 0;
    stlInt16                sBlockSize;

    sRecoveryInfo = SMR_GET_MEDIA_RECOVERY_INFO( aEnv );

    stlMemset( sRecoveryInfo, 0x00, STL_SIZEOF(smrMediaRecoveryInfo) );

    sRecoveryInfo->mMediaRecoveryType    = aRecoveryInfo->mRecoveryType;
    sRecoveryInfo->mObjectType           = aRecoveryInfo->mObjectType;
    sRecoveryInfo->mImrOption            = aRecoveryInfo->mImrOption;
    sRecoveryInfo->mImrCondition         = aRecoveryInfo->mImrCondition;
    sRecoveryInfo->mTbsId                = aRecoveryInfo->mTbsId;
    sRecoveryInfo->mUntilValue           = aRecoveryInfo->mUntilValue;

    sRecoveryInfo->mCurLsn               = SMR_INVALID_LSN;
    sRecoveryInfo->mPrevLastLsn          = SMR_INVALID_LSN;
    sRecoveryInfo->mRestoredChkptLsn     = SMR_INVALID_LSN;
    sRecoveryInfo->mRestoredChkptLid     = SMR_INVALID_LID;
    sRecoveryInfo->mLastChkptLsn         = SMR_INVALID_LSN;
    sRecoveryInfo->mLastChkptLid         = SMR_INVALID_LID;
    sRecoveryInfo->mMediaRecoveryPhase   = SMR_RECOVERY_PHASE_NONE;
    sRecoveryInfo->mLogBuffer            = NULL;
    sRecoveryInfo->mTransTable           = NULL;
    sRecoveryInfo->mOrgTransTable        = NULL;

    /* Recover할 Datafile을 저장할 List를 초기화한다. */
    STL_RING_INIT_HEADLINK( &sRecoveryInfo->mDatafileList,
                            STL_OFFSETOF( smrRecoveryDatafileInfo, mDatafileLink ) );

    sBlockSize = gSmrWarmupEntry->mLogStream.mLogStreamPersData.mBlockSize;
    SMR_MEDIA_RECOVERY_SET_FIELD( aEnv, BlockSize, sBlockSize );

    sBufferSize = knlGetPropertyBigIntValueByID( KNL_PROPERTY_MEDIA_RECOVERY_LOG_BUFFER_SIZE,
                                                 KNL_ENV(aEnv) );

    /**
     * MOUNT 단계에서 수행되는 media recovery를 위해 transaction table을
     * recreate 해서 사용하고, media recovery가 종료할 때 원복한다.
     */
    STL_TRY( smgAllocShmMem4Open( (SMXL_MAX_TRANS_TABLE_SIZE + 1) * STL_SIZEOF( smxlTrans ),
                                  (void**)&sRecoveryInfo->mTransTable,
                                  aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( smxlReplaceTransactionTable( &sOrgTransTable,
                                          &sOrgTransTableSize,
                                          sRecoveryInfo->mTransTable,
                                          aEnv )
             == STL_SUCCESS );
    sState = 2;

    SMR_MEDIA_RECOVERY_SET_FIELD( aEnv, OrgTransTable, (void *)sOrgTransTable );
    SMR_MEDIA_RECOVERY_SET_FIELD( aEnv, OrgTransTableSize, sOrgTransTableSize );

    STL_TRY( smgAllocShmMem4Open( sBufferSize + (sBlockSize * 2),
                                  (void**)&sBuffer,
                                  aEnv ) == STL_SUCCESS );
    sState = 3;

    SMR_MEDIA_RECOVERY_SET_FIELD( aEnv, BufferBlock, (void *)sBuffer );

    STL_TRY( smgAllocShmMem4Open( STL_SIZEOF( smrLogBuffer ),
                                  (void**)&sLogBuffer,
                                  aEnv ) == STL_SUCCESS );
    sState = 4;

    SMR_INIT_LOG_BUFFER( sLogBuffer,
                         sBufferSize,
                         sBuffer,
                         STL_FALSE, /* aIsShared */
                         sBlockSize );
    sLogBuffer->mFileSeqNo = -1;

    SMR_MEDIA_RECOVERY_SET_FIELD( aEnv, LogBuffer, (void *)sLogBuffer );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 4:
            (void)smgFreeShmMem4Open( (void *)sLogBuffer, aEnv );
        case 3:
            (void)smgFreeShmMem4Open( (void *)sBuffer, aEnv );
        case 2:
            (void)smxlRestoreTransactionTable( sOrgTransTable,
                                               sOrgTransTableSize,
                                               aEnv );
        case 1:
            (void)smgFreeShmMem4Open( (void *)sRecoveryInfo->mTransTable, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus smrFiniRecoveryContextAtMount( smlEnv  * aEnv )
{
    smxlTrans       * sOrgTransTable = NULL;
    stlInt32          sOrgTransTableSize;
    stlInt32          sState = 3;

    sOrgTransTable = (smxlTrans *)SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, OrgTransTable );
    sOrgTransTableSize = SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, OrgTransTableSize );

    if( sOrgTransTable != NULL )
    {
        STL_TRY( smxlRestoreTransactionTable( sOrgTransTable,
                                              sOrgTransTableSize,
                                              aEnv )
                 == STL_SUCCESS );
        sOrgTransTable = NULL;
    }

    sState = 3;
    STL_TRY( smgFreeShmMem4Open( SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, LogBuffer ),
                                 aEnv ) == STL_SUCCESS );

    sState = 2;
    STL_TRY( smgFreeShmMem4Open( SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, BufferBlock ),
                                 aEnv ) == STL_SUCCESS );

    sState = 1;
    
    sState = 0;
    STL_TRY( smgFreeShmMem4Open( SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, TransTable ),
                                 aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sOrgTransTable != NULL )
    {
        (void)smxlRestoreTransactionTable( sOrgTransTable,
                                           sOrgTransTableSize,
                                           aEnv );
    }

    switch( sState )
    {
        case 3:
            (void)smgFreeShmMem4Open( SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, LogBuffer ),
                                      aEnv );
        case 2:
            (void)smgFreeShmMem4Open( SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, BufferBlock ),
                                      aEnv );
        case 1:
            (void)smgFreeShmMem4Open( SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, TransTable ),
                                      aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus smrInitRecoveryContextAtOpen( smlRecoveryObjectType   aObjectType,
                                        smlTbsId                aTbsId,
                                        smlEnv                * aEnv )
{
    smrMediaRecoveryInfo  * sRecoveryInfo;
    smxlTrans             * sTransTable = NULL;
    smrLogBuffer          * sLogBuffer = NULL;
    stlChar               * sBuffer;
    stlInt64                sBufferSize;
    stlInt32                i;
    stlInt16                sBlockSize;

    sRecoveryInfo = SMR_GET_MEDIA_RECOVERY_INFO( aEnv );

    stlMemset( sRecoveryInfo, 0x00, STL_SIZEOF(smrMediaRecoveryInfo) );

    sRecoveryInfo->mMediaRecoveryType    = SML_RECOVERY_TYPE_COMPLETE_MEDIA_RECOVERY;
    sRecoveryInfo->mObjectType           = aObjectType;
    sRecoveryInfo->mTbsId                = aTbsId;
    sRecoveryInfo->mImrOption            = SML_IMR_OPTION_NONE;
    sRecoveryInfo->mImrCondition         = SML_IMR_CONDITION_NONE;
    sRecoveryInfo->mUntilValue           = -1;

    sRecoveryInfo->mCurLsn               = SMR_INVALID_LSN;
    sRecoveryInfo->mPrevLastLsn          = SMR_INVALID_LSN;
    sRecoveryInfo->mRestoredChkptLsn     = SMR_INVALID_LSN;
    sRecoveryInfo->mRestoredChkptLid     = SMR_INVALID_LID;
    sRecoveryInfo->mLastChkptLsn         = SMR_INVALID_LSN;
    sRecoveryInfo->mLastChkptLid         = SMR_INVALID_LID;
    sRecoveryInfo->mMediaRecoveryPhase   = SMR_RECOVERY_PHASE_NONE;
    sRecoveryInfo->mLogBuffer            = NULL;
    sRecoveryInfo->mTransTable           = NULL;
    sRecoveryInfo->mOrgTransTable        = NULL;

    sBlockSize = gSmrWarmupEntry->mLogStream.mLogStreamPersData.mBlockSize;
    SMR_MEDIA_RECOVERY_SET_FIELD( aEnv, BlockSize, sBlockSize );

    sBufferSize = knlGetPropertyBigIntValueByID( KNL_PROPERTY_MEDIA_RECOVERY_LOG_BUFFER_SIZE,
                                                 KNL_ENV(aEnv) );

    /**
     * OPEN 단계에서 media recovery를 위한 log buffer 할당
     */
    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                (SMXL_MAX_TRANS_TABLE_SIZE + 1) * STL_SIZEOF( smxlTrans ),
                                (void**)&sTransTable,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    for( i = 0; i < SMXL_MAX_TRANS_TABLE_SIZE; i++ )
    {
        SMXL_INIT_TRANS( &sTransTable[i], i, STL_FALSE, aEnv );
    }

    /**
     * Media Recovery를 수행하는 동안 참조할 transaction table을 session env에 설정
     */
    SMXL_SET_MEDIA_RECOVERY_TRANS_TABLE( aEnv, sTransTable );

    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                sBufferSize + (sBlockSize * 2),
                                (void**)&sBuffer,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                STL_SIZEOF( smrLogBuffer ),
                                (void**)&sLogBuffer,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    SMR_INIT_LOG_BUFFER( sLogBuffer,
                         sBufferSize,
                         sBuffer,
                         STL_FALSE, /* aIsShared */
                         sBlockSize );
    sLogBuffer->mFileSeqNo = -1;

    SMR_MEDIA_RECOVERY_SET_FIELD( aEnv, LogBuffer, (void *)sLogBuffer );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrFiniRecoveryContextAtOpen( smlEnv  * aEnv )
{
    /* nothing to do */
    SMXL_SET_MEDIA_RECOVERY_TRANS_TABLE( aEnv, NULL );

    return STL_SUCCESS;
}

stlInt64 smrGetUntilValue( smlEnv  * aEnv )
{
    smrMediaRecoveryInfo * sRecoveryInfo = SMR_GET_MEDIA_RECOVERY_INFO( aEnv );

    return sRecoveryInfo->mUntilValue;
}

inline stlUInt8 smrGetImrCondition( smlEnv  * aEnv )
{
    smrMediaRecoveryInfo * sRecoveryInfo = SMR_GET_MEDIA_RECOVERY_INFO( aEnv );

    return sRecoveryInfo->mImrCondition;
}

void smrSetIncompleteRecoveryCondition( stlUInt8   aCondition,
                                        smlEnv   * aEnv )
{
    smrMediaRecoveryInfo * sRecoveryInfo = SMR_GET_MEDIA_RECOVERY_INFO( aEnv );

    sRecoveryInfo->mImrCondition = aCondition;
}

stlUInt64 smrGetMediaRecoveryPhase( smlEnv  * aEnv )
{
    smrMediaRecoveryInfo * sRecoveryInfo = SMR_GET_MEDIA_RECOVERY_INFO( aEnv );

    return stlAtomicRead64( &(sRecoveryInfo->mMediaRecoveryPhase) );
}

void smrSetMediaRecoveryPhase( stlUInt64   aPhase,
                               smlEnv    * aEnv )
{
    smrMediaRecoveryInfo * sRecoveryInfo = SMR_GET_MEDIA_RECOVERY_INFO( aEnv );

    stlAtomicSet64( &(sRecoveryInfo->mMediaRecoveryPhase), aPhase );
}

stlStatus smrSetRecoveryPhaseAtMount( stlUInt8   aRecoveryPhase,
                                      smlEnv   * aEnv )
{
    stlUInt64    sOldValue;

    switch( aRecoveryPhase )
    {
        case SML_RECOVERY_AT_MOUNT_RECOVERY_BEGIN:
            /* 이미 Recovery가 수행 중인 경우 or Shutdown 중에는 Media Recovery 또는
             * Restart Recovery를 시작할 수 없다. */
            sOldValue = stlAtomicCas64( &(gSmrWarmupEntry->mRecoveryInfo.mRecoveryPhase),
                                        SMR_RECOVERY_PHASE_BEGIN,
                                        SMR_RECOVERY_PHASE_NONE );

            STL_TRY_THROW( sOldValue == SMR_RECOVERY_PHASE_NONE,
                           RAMP_ERR_CANNOT_BEGIN_RECOVERY );
            break;

        case SML_RECOVERY_AT_MOUNT_RECOVERY_PREVENT:
            sOldValue = stlAtomicCas64( &(gSmrWarmupEntry->mRecoveryInfo.mRecoveryPhase),
                                        SMR_RECOVERY_PHASE_PREVENTED,
                                        SMR_RECOVERY_PHASE_NONE );

            STL_TRY_THROW( sOldValue == SMR_RECOVERY_PHASE_NONE,
                           RAMP_ERR_CANNOT_BEGIN_RECOVERY );
            break;

        case SML_RECOVERY_AT_MOUNT_RECOVERY_END:
            sOldValue = stlAtomicRead64( &(gSmrWarmupEntry->mRecoveryInfo.mRecoveryPhase) );

            if( sOldValue != SMR_RECOVERY_PHASE_NONE )
            {
                STL_TRY_THROW( sOldValue >= SMR_RECOVERY_PHASE_BEGIN,
                               RAMP_ERR_INTERNAL );

                STL_TRY_THROW( sOldValue != SMR_RECOVERY_PHASE_PREVENTED,
                               RAMP_ERR_CANNOT_BEGIN_RECOVERY );
            }
            else
            {
                /**
                 * SMR_RECOVERY_PHASE_NONE인 경우 END는 무시한다.
                 * - Open동안 exception이 발생한 경우는 Recovery Phase가 NONE으로 변경될수 있다.
                 */
            }

            stlAtomicSet64( &(gSmrWarmupEntry->mRecoveryInfo.mRecoveryPhase),
                            SMR_RECOVERY_PHASE_NONE );
            break;

        default:
            STL_THROW( RAMP_ERR_INTERNAL );
            break;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CANNOT_BEGIN_RECOVERY )
    {
        if( sOldValue == SMR_RECOVERY_PHASE_PREVENTED )
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          SML_ERRCODE_DATABASE_IN_SHUTDOWN,
                          NULL,
                          KNL_ERROR_STACK(aEnv) );
        }
        else
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          SML_ERRCODE_RECOVERY_IN_PROGRESS,
                          NULL,
                          KNL_ERROR_STACK(aEnv) );
        }
    }

    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INTERNAL,
                      "- smrSetRecoveryPhaseAtMount()",
                      KNL_ERROR_STACK(aEnv),
                      aRecoveryPhase );

        KNL_SYSTEM_FATAL_DASSERT( STL_FALSE, KNL_ENV(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
