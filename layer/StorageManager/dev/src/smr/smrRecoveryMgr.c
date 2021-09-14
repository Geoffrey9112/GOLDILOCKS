/*******************************************************************************
 * smrRecoveryMgr.c
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
#include <smf.h>
#include <smg.h>
#include <smxl.h>
#include <smkl.h>
#include <smr.h>
#include <smp.h>
#include <sme.h>
#include <sms.h>
#include <smq.h>
#include <smrDef.h>
#include <smlGeneral.h>
#include <smrRecoveryMgr.h>
#include <smrLogFile.h>
#include <smrLogCursor.h>
#include <smrLogBuffer.h>
#include <smrRedo.h>
#include <smrMtxUndo.h>
#include <smrLogGroup.h>
#include <smrLogStream.h>
#include <smrLogMember.h>

/**
 * @file smrRecoveryMgr.c
 * @brief Storage Manager Layer Recovery Manager Internal Routines
 */

extern smrWarmupEntry * gSmrWarmupEntry;
extern stlInt64         gDataStoreMode;

/**
 * @addtogroup smr
 * @{
 */

/**
 * @brief Component별 Redo Function Vector
 */
smrRedoVector * gSmrRedoVectorsTable[SMG_COMPONENT_MAX];

/**
 * @brief Recovery Function Vector를 Recovery Manager에 등록한다.
 * @param[in] aComponentClass Component Class
 * @param[in] aRedoVectors Redo & Mtx Undo Function Vector
 */
void smrRegisterRedoVectors( smgComponentClass   aComponentClass,
                             smrRedoVector     * aRedoVectors )
{
    gSmrRedoVectorsTable[aComponentClass] = aRedoVectors;
}

/**
 * @brief 비정상 종료상황에서 시스템을 정상상태로 회복한다.
 * @param[in] aLogOption 복구 후 startup 시 log option(NORESETLOGS, RESETLOGS)
 * @param[in,out] aEnv Environment 정보
 * @note 서버가 비정상 종료된 상황에서는 Recovery 절차를 수행하는 반면,
 * 정상적으로 종료된 상황에서는 별도의 Recovery 절차를 수행하지 않는다.
 */
stlStatus smrRecover( stlUInt8   aLogOption,
                      smlEnv   * aEnv )
{
    smlDataAccessMode sAccessMode;
    smrLid            sRedoLid;
    smrLsn            sOldestLsn;
    smlScn            sSystemScn;
    smrChkptArg       sArg;
    stlBool           sBuildAgableScn;

    KNL_BREAKPOINT( KNL_BREAKPOINT_SMRRECOVER_BEFORE_RECOVER,
                    (knlEnv*)aEnv );

    STL_DASSERT( knlGetCurrStartupPhase() == KNL_STARTUP_PHASE_MOUNT );

    sAccessMode = smfGetDataAccessMode();
    
    /**
     * CDS나 DS모드는 이전에 TDS모드로 운영되었다면 TDS운영당시 정상으로 SHUTDOWN된
     * 경우에만 이용가능하다.
     */
    if( smxlGetDataStoreMode() != SML_DATA_STORE_MODE_TDS )
    {
        if( (smfGetDataStoreMode() == SML_DATA_STORE_MODE_TDS) &&
            (smfGetServerState() != SML_SERVER_STATE_SHUTDOWN) )
        {
            STL_ASSERT( STL_FALSE );
        }
    }

    /**
     * ACCESS READ-ONLY는 이전에 서버가 정상적으로 종료된 상태여야만 한다.
     */
    if( sAccessMode == SML_DATA_ACCESS_MODE_READ_ONLY )
    {
        if( smfGetServerState() != SML_SERVER_STATE_SHUTDOWN )
        {
            STL_ASSERT( STL_FALSE );
        }
    }

    STL_TRY_THROW( smfGetServerState() != SML_SERVER_STATE_NONE, RAMP_SUCCESS );

    if( sAccessMode == SML_DATA_ACCESS_MODE_READ_ONLY )
    {
        STL_TRY( smrReadChkptLog( &gSmrWarmupEntry->mLogPersData.mChkptLid,
                                  gSmrWarmupEntry->mLogPersData.mChkptLsn,
                                  &sOldestLsn,
                                  &sSystemScn,
                                  aEnv ) == STL_SUCCESS );

        smxlSetSystemScn( sSystemScn );
        
        /**
         * - Build Relation & Segment Cache
         * - Build Sequence Cache
         * - Build Undo Relation
         * - Build Agable Scn
         * - Enable log flushing
         */
        STL_TRY( smeBuildCachesAtStartup( aEnv ) == STL_SUCCESS );
        STL_TRY( smqBuildCachesAtStartup( aEnv ) == STL_SUCCESS );
        STL_TRY( smxlAttachUndoRelations( aEnv ) == STL_SUCCESS );

        smrSetRecoveryPhase( SMR_RECOVERY_PHASE_DONE );

        STL_TRY( smxlBuildAgableScn4Restart( aEnv ) == STL_SUCCESS );
        
        smrActivateLogFlusher();

        STL_TRY( smxlRestructure( aEnv ) == STL_SUCCESS );
        STL_TRY( smklRestructure( aEnv ) == STL_SUCCESS );
    }
    else
    {
        if( aLogOption == SML_STARTUP_LOG_OPTION_NORESETLOGS )
        {
            if( smfGetServerState() != SML_SERVER_STATE_RECOVERED )
            {
                smfSetServerState( SML_SERVER_STATE_RECOVERING );
                smrSetRecoveryPhase( SMR_RECOVERY_PHASE_BEGIN );

                STL_TRY( knlLogMsg( NULL,
                                    KNL_ENV( aEnv ),
                                    KNL_LOG_LEVEL_INFO,
                                    "[RESTART REDO] begin\n" )
                         == STL_SUCCESS );
    
                /**
                 * 1. Analysis Phase
                 * - Build active transaction table
                 */
                STL_TRY( smrAnalysis( &sOldestLsn,
                                      &sRedoLid,
                                      &sSystemScn,
                                      aEnv ) == STL_SUCCESS );
        
                /**
                 * 2. Redo Phase
                 */
                STL_TRY( smrRestartRedo( sOldestLsn,
                                         sRedoLid,
                                         sSystemScn,
                                         aEnv ) == STL_SUCCESS );
            
                STL_TRY( knlLogMsg( NULL,
                                    KNL_ENV( aEnv ),
                                    KNL_LOG_LEVEL_INFO,
                                    "[RESTART REDO] end - restart lsn(%ld), restart scn(%ld)\n",
                                    gSmrWarmupEntry->mLsn,
                                    smxlGetSystemScn() )
                         == STL_SUCCESS );
    
                /**
                 * 3. Prepare Undo Phase
                 * - Build Relation & Segment Cache
                 * - Build Undo Relation
                 * - Enable log flushing
                 */
                STL_TRY( smeBuildCachesAtStartup( aEnv ) == STL_SUCCESS );
                STL_TRY( smxlAttachUndoRelations( aEnv ) == STL_SUCCESS );

                smrActivateLogFlusher();

                /**
                 * 4. Undo Phase
                 * - Undo & Build Agable Scn
                 */
                STL_TRY( knlLogMsg( NULL,
                                    KNL_ENV( aEnv ),
                                    KNL_LOG_LEVEL_INFO,
                                    "[RESTART UNDO] begin\n" )
                         == STL_SUCCESS );

                STL_TRY( smrRestartUndo( aEnv ) == STL_SUCCESS );

                STL_TRY( knlLogMsg( NULL,
                                    KNL_ENV( aEnv ),
                                    KNL_LOG_LEVEL_INFO,
                                    "[RESTART UNDO] end\n" )
                         == STL_SUCCESS );

                STL_DASSERT( smxlGetActiveTransCount() == 0 );
            
                STL_TRY( smqBuildCachesAtStartup( aEnv ) == STL_SUCCESS );
                
                smrSetRecoveryPhase( SMR_RECOVERY_PHASE_DONE );

                STL_TRY( smxlBuildAgableScn4Restart( aEnv ) == STL_SUCCESS );
            }
        }
        else
        {
            /**
             * Resetlogs로 startup한 후 controlfile을 보정한다.
             */
            STL_TRY( smfAdjustCtrlfile( aEnv ) == STL_SUCCESS );

            /**
             * - Build Relation & Segment Cache
             * - Build Sequence Cache
             * - Build Undo Relation
             * - Build Agable Scn
             * - Enable log flushing
             */
            STL_TRY( smeBuildCachesAtStartup( aEnv ) == STL_SUCCESS );
            STL_TRY( smqBuildCachesAtStartup( aEnv ) == STL_SUCCESS );
            STL_TRY( smxlAttachUndoRelations( aEnv ) == STL_SUCCESS );

            smrSetRecoveryPhase( SMR_RECOVERY_PHASE_DONE );

            STL_TRY( smxlBuildAgableScn4Restart( aEnv ) == STL_SUCCESS );

            smrActivateLogFlusher();
        }

        /**
         * MOUNT 단계에서 지정된 Irrecoverable segment들을 Unusable 상태로 설정한다.
         */
        STL_TRY( smsIrrecoverableSegmentToUnusable( aEnv ) == STL_SUCCESS );

        /**
         * 5. Finalize Recovery Process
         * - restructure undo, transaction and lock table
         * - Checkpointing
         */

        STL_TRY( smxlRestructure( aEnv ) == STL_SUCCESS );
        STL_TRY( smklRestructure( aEnv ) == STL_SUCCESS );
            
        if( gDataStoreMode == SML_DATA_STORE_MODE_TDS )
        {
            stlMemset( (void *)&sArg, 0x00, STL_SIZEOF(smrChkptArg) );
            sArg.mLogfileSeq = STL_INT64_MAX;
            sArg.mFlushBehavior = SMP_FLUSH_LOGGED_PAGE;

            STL_TRY( knlAddEnvEvent( SML_EVENT_CHECKPOINT,
                                     NULL,            /* aEventMem */
                                     (void*)&sArg,
                                     STL_SIZEOF( smrChkptArg ),
                                     SML_CHECKPOINT_ENV_ID,
                                     KNL_EVENT_WAIT_BLOCKED,
                                     STL_TRUE, /* aForceSuccess */
                                     KNL_ENV( aEnv ) ) == STL_SUCCESS );
        }
        
        smfSetServerState( SML_SERVER_STATE_RECOVERED );
    }

    smfSetDataStoreMode( gDataStoreMode );
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( (smxlGetDataStoreMode() == SML_DATA_STORE_MODE_TDS) &&
        (smrIsLogFlusherActive() == STL_TRUE) )
    {
        sBuildAgableScn = STL_FALSE;
        
        (void) knlAddEnvEvent( SML_EVENT_LOOPBACK,
                               NULL, /* aEventMem */
                               &sBuildAgableScn,
                               STL_SIZEOF( stlBool ),
                               SML_CHECKPOINT_ENV_ID,
                               KNL_EVENT_WAIT_BLOCKED,
                               STL_TRUE,
                               KNL_ENV( aEnv ) );
                               
    }

    (void) smrDeactivateLogFlusher();

    return STL_FAILURE;
}

/**
 * @brief 체크포인트를 수행한다.
 * @param[in] aData 체크포인트 정보
 * @param[in] aDataSize 체크포인트 정보 크기
 * @param[out] aDone 성공여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smrCheckpointEventHandler( void      * aData,
                                     stlUInt32   aDataSize,
                                     stlBool   * aDone,
                                     void      * aEnv )
{
    smrChkptArg sArg;
    
    *aDone = STL_FALSE;

    STL_DASSERT( aDataSize == STL_SIZEOF(smrChkptArg) );

    stlMemset( (void *)&sArg, 0x00, STL_SIZEOF(smrChkptArg) );
    stlMemcpy( (void *)&sArg, aData, STL_SIZEOF(smrChkptArg) );

    /**
     * 대상 Log file이 이미 inactive라면 Skip 한다.
     */
    if( sArg.mLogfileSeq <= gSmrWarmupEntry->mLogPersData.mLastInactiveLfsn )
    {
        STL_TRY( knlLogMsg( NULL,
                            KNL_ENV( aEnv ),
                            KNL_LOG_LEVEL_INFO,
                            "[CHECKPOINT] CHECKPOINT was skipped (%ld)\n",
                            sArg.mLogfileSeq )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( smrCheckpoint( sArg.mFlushBehavior,
                                (smlEnv*)aEnv ) == STL_SUCCESS );
    }

    *aDone = STL_TRUE;

    return STL_SUCCESS;

    STL_FINISH;
    
    *aDone = STL_TRUE;

    (void) knlLogMsg( NULL,
                      KNL_ENV( aEnv ),
                      KNL_LOG_LEVEL_WARNING,
                      "[CHECKPOINT] CHECKPOINT was failed\n" );
    
    return STL_FAILURE;
}

/**
 * @brief 체크포인트를 수행한다.
 * @param[in,out] aFlushBehavior Datafile Flush Behavior
 * @param[in,out] aEnv           Environment 정보
 */
inline stlStatus smrCheckpoint( smpFlushBehavior   aFlushBehavior,
                                smlEnv           * aEnv )
{
    smrChkptInfo  * sChkptInfo;
    smrLsn          sMinFlushedLsn;
    stlUInt32       sState = 0;
    smpFlushArg     sArg;

    STL_DASSERT( KNL_SESS_ENV(aEnv)->mOpenFileCount == 0 );
    
    sChkptInfo = &(gSmrWarmupEntry->mChkptInfo);

    /**
     * checkpoint가 진행중임을 설정한다.
     */
    sChkptInfo->mChkptState = SML_CHECKPOINT_STATE_IN_PROGRESS;
    sState = 1;

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "[CHECKPOINT] begin\n" )
             == STL_SUCCESS );

    /**
     * Checkpoint Lsn 이전 로그와 연관된 페이지들이 디스크에 기록될때까지 기다린다.
     */

    stlMemset( &sArg, 0x00, STL_SIZEOF(smpFlushArg) );
    sArg.mBehavior = aFlushBehavior;
    sArg.mLogging = STL_TRUE;
    sArg.mForCheckpoint = STL_TRUE;
        
    STL_TRY( knlAddEnvEvent( SML_EVENT_PAGE_FLUSH,
                             NULL,            /* aEventMem */
                             &sArg,
                             STL_SIZEOF(smpFlushArg),
                             SML_PAGE_FLUSHER_ENV_ID,
                             KNL_EVENT_WAIT_BLOCKED,
                             STL_TRUE, /* aForceSuccess */
                             KNL_ENV( aEnv ) ) == STL_SUCCESS );

    sMinFlushedLsn = smpGetMinFlushedLsn();
        
    if( smrIsCheckpointerActive() == STL_TRUE )
    {
        STL_TRY( knlAddEnvEvent( SML_EVENT_ARCHIVELOG,
                                 NULL,            /* aEventMem */
                                 &sMinFlushedLsn,
                                 STL_SIZEOF( smrLsn ),
                                 SML_ARCHIVELOG_ENV_ID,
                                 KNL_EVENT_WAIT_BLOCKED,
                                 STL_TRUE, /* aForceSuccess */
                                 KNL_ENV( aEnv ) ) == STL_SUCCESS );
    }
    else
    {
        /**
         * Log flush thread가 수행중이지 않으면 직접 log file을 archiving한다.
         */
        STL_TRY( smrLogArchiving( sMinFlushedLsn,
                                  aEnv ) == STL_SUCCESS );
    }

    KNL_BREAKPOINT( KNL_BREAKPOINT_CHECKPOINT_BEFORE_CHKPT_LOGGING,
                    (knlEnv*)aEnv );

    /**
     * Recovery는 sMinFlushedLsn(Flush된 Lsn) + 1 부터 진행해야 한다.
     */
    STL_TRY( smrWriteChkptLog( sMinFlushedLsn + 1,
                               aEnv ) == STL_SUCCESS );

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "[CHECKPOINT] end\n" )
             == STL_SUCCESS );

    /**
     * checkpoint 상태를 idle로 설정한다.
     */
    sState = 0;
    sChkptInfo->mChkptState = SML_CHECKPOINT_STATE_IN_IDLE;

    STL_ASSERT( sChkptInfo->mChkptState == SML_CHECKPOINT_STATE_IN_IDLE );
    STL_DASSERT( KNL_SESS_ENV(aEnv)->mOpenFileCount == 0 );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        sChkptInfo->mChkptState = SML_CHECKPOINT_STATE_IN_IDLE;
    }

    STL_DASSERT( KNL_SESS_ENV(aEnv)->mOpenFileCount == 0 );
    
    return STL_FAILURE;
}

/**
 * @brief Archivelog를 수행한다.
 * @param[in] aData Archivelog 정보
 * @param[in] aDataSize Archivelog 정보 크기
 * @param[out] aDone 성공여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smrArchivelogEventHandler( void      * aData,
                                     stlUInt32   aDataSize,
                                     stlBool   * aDone,
                                     void      * aEnv )
{
    smrLsn sFlushedLsn;
    
    *aDone = STL_FALSE;

    STL_WRITE_INT64( &sFlushedLsn, aData );

    STL_TRY( smrArchivelog( sFlushedLsn,
                            (smlEnv*)aEnv )
             == STL_SUCCESS );

    *aDone = STL_TRUE;

    return STL_SUCCESS;

    STL_FINISH;
    
    *aDone = STL_TRUE;

    return STL_FAILURE;
}

/**
 * @brief 주어진 aLsn 과 연관된 페이지들이 디스크에 기록될때까지 기다린다.
 * @param[in]     aLsn Log Sequence Number
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smrWaitPageFlusher( smrLsn   aLsn,
                              smlEnv * aEnv )
{
    stlBool sIsTimedOut;
    
    while( 1 )
    {
        if( smpGetMinFlushedLsn() > aLsn )
        {
            break;
        }

        STL_TRY( knlCondWait( &gSmrWarmupEntry->mLogStream.mCondVar,
                              STL_TIMESLICE_TIME,
                              &sIsTimedOut,
                              KNL_ENV( aEnv ) ) == STL_SUCCESS );
            
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Media Recovery를 진행하고 있는지에 대한 여부
 * @param[in,out] aEnv Environment 정보
 * @todo Media Recovery 구현중 고려해야함.
 */
stlBool smrProceedMediaRecovery( smlEnv * aEnv )
{
    return SMR_PROCEED_MEDIA_RECOVERY( aEnv );
}

stlBool smrProceedOnlineMediaRecovery( smlEnv * aEnv )
{
    if( (smrProceedMediaRecovery(aEnv) == STL_TRUE) &&
        (knlGetCurrStartupPhase() >= KNL_STARTUP_PHASE_OPEN) )
    {
        return STL_TRUE;
    }

    return STL_FALSE;
}

/**
 * @brief Log Archiving을 수행한다.
 * @param[in]     aFlushedLsn 디스크로 저장된 Minimum Lsn
 * @param[in,out] aEnv        Environment 정보
 */
stlStatus smrArchivelog( smrLsn   aFlushedLsn,
                         smlEnv * aEnv )
{
    STL_DASSERT( KNL_SESS_ENV(aEnv)->mOpenFileCount == 0 );
    
    STL_TRY( smrLogArchiving( aFlushedLsn,
                              aEnv ) == STL_SUCCESS );

    STL_TRY( smfSaveCtrlFile( aEnv ) == STL_SUCCESS );

    STL_DASSERT( KNL_SESS_ENV(aEnv)->mOpenFileCount == 0 );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( KNL_SESS_ENV(aEnv)->mOpenFileCount == 0 );
    
    return STL_FAILURE;
}

/**
 * @brief 현재 Redo 중인 Log의 Lsn을 얻는다.
 * @return Redo Log Lsn
 */
inline smrLsn smrGetCurRedoLogLsn()
{
    return gSmrWarmupEntry->mRecoveryInfo.mLogLsn;
}

void smrGetRecoveryInfo( smrRecoveryInfo * aRecoveryInfo )
{
    stlMemcpy( aRecoveryInfo,
               &gSmrWarmupEntry->mRecoveryInfo,
               STL_SIZEOF( smrRecoveryInfo ) );
}

smrLsn smrGetSystemLsn()
{
    return gSmrWarmupEntry->mLsn;
}

inline void smrGetConsistentChkptLsnAndLid( smrLsn * aLsn,
                                            smrLid * aLid )
{
    stlInt64    sSyncValueB;

    while( STL_TRUE )
    {
        sSyncValueB = gSmrWarmupEntry->mSyncValue[1];
        stlMemBarrier();

        *aLsn = gSmrWarmupEntry->mLogPersData.mChkptLsn;
        *aLid = gSmrWarmupEntry->mLogPersData.mChkptLid;

        if( sSyncValueB == gSmrWarmupEntry->mSyncValue[0] )
        {
            break;
        }
    }
}

inline void smrSetConsistentChkptLsnAndLid( smrLsn   aLsn,
                                            smrLid   aLid )
{
    gSmrWarmupEntry->mSyncValue[0] += 1;
    stlMemBarrier();

    gSmrWarmupEntry->mLogPersData.mChkptLsn = aLsn;
    gSmrWarmupEntry->mLogPersData.mChkptLid = aLid;

    gSmrWarmupEntry->mSyncValue[1] += 1;
}

/**
 * @brief 가장 최근에 수행된 checkpoint의 Lsn을 얻는다.
 * @return Last Checkpoint Lsn
 */
inline smrLsn smrGetLastCheckpointLsn()
{
    return gSmrWarmupEntry->mLogPersData.mChkptLsn;
}

/**
 * @brief 가장 최근에 수행된 checkpoint의 Lid를 얻는다.
 * @return Last Checkpoint Lid
 */
inline smrLid smrGetLastCheckpointLid()
{
    return gSmrWarmupEntry->mLogPersData.mChkptLid;
}

/**
 * @brief Database가 생성된 시간을 얻는다.
 * @return Database creation time
 */
inline stlTime smrGetDatabaseCreationTime()
{
    return gSmrWarmupEntry->mLogPersData.mCreationTime;
}

void smrGetChkptInfo( smrChkptInfo * aChkptInfo )
{
    stlMemcpy( aChkptInfo,
               &gSmrWarmupEntry->mChkptInfo,
               STL_SIZEOF( smrChkptInfo ) );
}

stlStatus smrSetLogFlusherState( stlInt16   aState,
                                 smlEnv   * aEnv )
{
    gSmrWarmupEntry->mLogFlusherState = aState;
    return STL_SUCCESS;
}

stlStatus smrSetCheckpointerState( stlInt16   aState,
                                   smlEnv   * aEnv )
{
    gSmrWarmupEntry->mCheckpointerState = aState;
    return STL_SUCCESS;
}

inline stlInt16 smrGetCheckpointState()
{
    return gSmrWarmupEntry->mChkptInfo.mChkptState;
}

inline stlInt64 smrGetLogFlushingState()
{
    return gSmrWarmupEntry->mBlockLogFlushing;
}

stlStatus smrCreateLogCtrlSection( stlFile  * aFile,
                                   stlSize  * aWrittenBytes,
                                   smlEnv   * aEnv )
{
    stlSize       sWrittenBytes;
    stlSize       sTotalWrittenBytes = 0;
    smfCtrlBuffer sBuffer;

    SMF_INIT_CTRL_BUFFER( &sBuffer );

    SMR_INIT_LOG_PERSISTENT_DATA( &(gSmrWarmupEntry->mLogPersData) );

    gSmrWarmupEntry->mLogPersData.mCreationTime = stlNow();
    
    STL_TRY( smfWriteCtrlFile( aFile,
                               &sBuffer,
                               (stlChar *)&(gSmrWarmupEntry->mLogPersData),
                               STL_SIZEOF( smrLogPersData ),
                               &sWrittenBytes,
                               aEnv )
             == STL_SUCCESS );
    sTotalWrittenBytes += sWrittenBytes;

    SMR_INIT_LOG_STREAM_PERSISTENT_DATA( &(gSmrWarmupEntry->mLogStream.mLogStreamPersData) );

    STL_TRY( smfWriteCtrlFile( aFile,
                               &sBuffer,
                               (stlChar *)&(gSmrWarmupEntry->mLogStream.mLogStreamPersData),
                               STL_SIZEOF( smrLogStreamPersData ),
                               &sWrittenBytes,
                               aEnv )
             == STL_SUCCESS );

    /**
     * Controlfile buffer에 남아 있는 내용을 disk에 flush한다.
     */
    STL_TRY( smfFlushCtrlFile( aFile, &sBuffer, aEnv ) == STL_SUCCESS );

    sTotalWrittenBytes += sWrittenBytes;
    
    *aWrittenBytes = STL_ALIGN( (stlInt64)sTotalWrittenBytes, SMF_CONTROLFILE_IO_BLOCK_SIZE );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrWriteLogCtrlSection( stlFile  * aFile,
                                  stlSize  * aWrittenBytes,
                                  smlEnv   * aEnv )
{
    stlSize                sWrittenBytes = 0;
    stlInt32               sTotalWrittenBytes = 0;
    smrLogGroupPersData    sLogGroupPersData;
    void                 * sLogGroupIterator;
    smrLogMemberPersData   sLogMemberPersData;
    void                 * sLogMemberIterator;
    smfCtrlBuffer          sBuffer;

    SMF_INIT_CTRL_BUFFER( &sBuffer );

    /**
     * Logging 관련 Persistent Information 기록
     */
    STL_TRY( smfWriteCtrlFile( aFile,
                               &sBuffer,
                               (stlChar *)&gSmrWarmupEntry->mLogPersData,
                               STL_SIZEOF( smrLogPersData ),
                               &sWrittenBytes,
                               aEnv ) == STL_SUCCESS );
    sTotalWrittenBytes = sWrittenBytes;

    /**
     * Log Stream 관련 Persistent Information 기록
     */
    STL_TRY( smfWriteCtrlFile( aFile,
                               &sBuffer,
                               (stlChar *)&gSmrWarmupEntry->mLogStream.mLogStreamPersData,
                               STL_SIZEOF( smrLogStreamPersData ),
                               &sWrittenBytes,
                               aEnv ) == STL_SUCCESS );
    sTotalWrittenBytes += sWrittenBytes;
    
    STL_TRY( smrFirstLogGroupPersData( &gSmrWarmupEntry->mLogStream,
                                       &sLogGroupPersData,
                                       &sLogGroupIterator,
                                       aEnv )
             == STL_SUCCESS );

    /**
     * Log Group 관련 Persistent Information 기록
     */
    while( 1 )
    {
        if( sLogGroupIterator == NULL )
        {
            break;
        }
            
        STL_TRY( smfWriteCtrlFile( aFile,
                                   &sBuffer,
                                   (stlChar *)&sLogGroupPersData,
                                   STL_SIZEOF( smrLogGroupPersData ),
                                   &sWrittenBytes,
                                   aEnv ) == STL_SUCCESS );
        sTotalWrittenBytes += sWrittenBytes;

        STL_TRY( smrFirstLogMemberPersData( sLogGroupIterator,
                                            &sLogMemberPersData,
                                            &sLogMemberIterator,
                                            aEnv )
                 == STL_SUCCESS );
            
        /**
         * Log Member 관련 Persistent Information 기록
         */
        while( 1 )
        {
            if( sLogMemberIterator == NULL )
            {
                break;
            }

            STL_TRY( smfWriteCtrlFile( aFile,
                                       &sBuffer,
                                       (stlChar *)&sLogMemberPersData,
                                       STL_SIZEOF( smrLogMemberPersData ),
                                       &sWrittenBytes,
                                       aEnv ) == STL_SUCCESS );
            sTotalWrittenBytes += sWrittenBytes;
                
            STL_TRY( smrNextLogMemberPersData( sLogGroupIterator,
                                               &sLogMemberPersData,
                                               &sLogMemberIterator,
                                               aEnv )
                     == STL_SUCCESS );
        }
            
        STL_TRY( smrNextLogGroupPersData( &gSmrWarmupEntry->mLogStream,
                                          &sLogGroupPersData,
                                          &sLogGroupIterator,
                                          aEnv )
                 == STL_SUCCESS );
    }

    /**
     * Controlfile buffer에 남아 있는 내용을 disk에 flush한다.
     */
    STL_TRY( smfFlushCtrlFile( aFile, &sBuffer, aEnv ) == STL_SUCCESS );

    *aWrittenBytes = STL_ALIGN( (stlInt64)sTotalWrittenBytes, SMF_CONTROLFILE_IO_BLOCK_SIZE );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrLoadLogCtrlSection( stlInt64 * aMinLogFileSize,
                                 smlEnv   * aEnv )
{
    stlFile                sFile;
    stlInt32               sState = 0;
    smrLogStreamPersData * sLogStreamPersData;
    smrLogStream         * sLogStream;
    smrLogGroup          * sLogGroup;
    smrLogMember         * sLogMember;
    stlInt32               i;
    stlInt32               j;
    smfCtrlBuffer          sBuffer;

    *aMinLogFileSize = STL_INT64_MAX;

    STL_TRY( smfOpenCtrlSection( &sFile,
                                 SMF_CTRL_SECTION_LOG,
                                 aEnv ) == STL_SUCCESS );
    sState = 1;

    SMF_INIT_CTRL_BUFFER( &sBuffer );

    /**
     * Logging 관련 Persistent Information 기록
     */

    STL_TRY( smfReadCtrlFile( &sFile,
                              &sBuffer,
                              (stlChar *)&gSmrWarmupEntry->mLogPersData,
                              STL_SIZEOF( smrLogPersData ),
                              aEnv ) == STL_SUCCESS );

    sLogStream = &gSmrWarmupEntry->mLogStream;
    sLogStreamPersData = &(sLogStream->mLogStreamPersData);
        
    STL_TRY( smfReadCtrlFile( &sFile,
                              &sBuffer,
                              (stlChar *)sLogStreamPersData,
                              STL_SIZEOF( smrLogStreamPersData ),
                              aEnv ) == STL_SUCCESS );

    for( i = 0; i < sLogStreamPersData->mLogGroupCount; i++ )
    {
        STL_TRY( smgAllocShmMem4Mount( STL_SIZEOF( smrLogGroup ),
                                       (void**)&sLogGroup,
                                       aEnv )
                 == STL_SUCCESS );

        SMR_INIT_LOG_GROUP( sLogGroup );

        STL_RING_ADD_LAST( &sLogStream->mLogGroupList, sLogGroup );
            
        STL_TRY( smfReadCtrlFile( &sFile,
                                  &sBuffer,
                                  (stlChar *)&sLogGroup->mLogGroupPersData,
                                  STL_SIZEOF( smrLogGroupPersData ),
                                  aEnv ) == STL_SUCCESS );

        *aMinLogFileSize = (*aMinLogFileSize > sLogGroup->mLogGroupPersData.mFileSize) ?
            sLogGroup->mLogGroupPersData.mFileSize : *aMinLogFileSize;

        for( j = 0; j < sLogGroup->mLogGroupPersData.mLogMemberCount; j++ )
        {
            STL_TRY( smgAllocShmMem4Mount( STL_SIZEOF( smrLogMember ),
                                           (void**)&sLogMember,
                                           aEnv )
                     == STL_SUCCESS );

            SMR_INIT_LOG_MEMBER( sLogMember );

            STL_RING_ADD_LAST( &sLogGroup->mLogMemberList, sLogMember );
            
            STL_TRY( smfReadCtrlFile( &sFile,
                                      &sBuffer,
                                      (stlChar *)&sLogMember->mLogMemberPersData,
                                      STL_SIZEOF( smrLogMemberPersData ),
                                      aEnv ) == STL_SUCCESS );
        }
    }
    
    sState = 0;
    STL_TRY( smfCloseCtrlSection( &sFile,
                                  aEnv ) == STL_SUCCESS );

    smfSetCtrlFileLoadState( SMF_CTRL_FILE_LOAD_STATE_LOG );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smfCloseCtrlSection( &sFile, aEnv );
    }

    return STL_FAILURE;
}

static stlChar * smrGetLogStreamStateString( stlUInt16  aState )
{
    stlChar * sString = NULL;

    switch( aState )
    {
        case SMR_LOG_STREAM_STATE_CREATING :
            sString = "CREATING";
            break;
        case SMR_LOG_STREAM_STATE_DROPPING :
            sString = "DROPPING";
            break;
        case SMR_LOG_STREAM_STATE_UNUSED :
            sString = "UNUSED";
            break;
        case SMR_LOG_STREAM_STATE_ACTIVE :
            sString = "ACTIVE";
            break;
        default :
            STL_ASSERT( 0 );
            break;
    }

    return sString;
}

void smrLogStreamStateString( stlUInt16   aState,
                              stlUInt32   aSize,
                              stlChar   * aString )
{
    stlStrncpy( aString,
                smrGetLogStreamStateString( aState ),
                aSize );
}

static stlChar * smrGetLogGroupStateString( stlUInt16  aState )
{
    stlChar * sString = NULL;

    switch( aState )
    {
        case SMR_LOG_GROUP_STATE_CREATING :
            sString = "CREATING";
            break;
        case SMR_LOG_GROUP_STATE_DROPPING :
            sString = "DROPPING";
            break;
        case SMR_LOG_GROUP_STATE_UNUSED :
            sString = "UNUSED";
            break;
        case SMR_LOG_GROUP_STATE_ACTIVE :
            sString = "ACTIVE";
            break;
        case SMR_LOG_GROUP_STATE_CURRENT :
            sString = "CURRENT";
            break;
        case SMR_LOG_GROUP_STATE_INACTIVE :
            sString = "INACTIVE";
            break;
        default :
            STL_ASSERT( 0 );
            break;
    }

    return sString;
}

void smrLogGroupStateString( stlUInt16   aState,
                             stlUInt32   aSize,
                             stlChar   * aString )
{
    stlStrncpy( aString,
                smrGetLogGroupStateString( aState ),
                aSize );
}

stlStatus smrDumpLogCtrlSection( stlChar * aFilePath,
                                 smlEnv  * aEnv )
{
    stlFile                sFile;
    stlInt32               sState = 0;
    smrLogPersData         sLogPersData;
    smrLogStreamPersData   sLogStreamPersData;
    smrLogGroupPersData    sLogGroupPersData;
    smrLogMemberPersData   sLogMemberPersData;
    stlInt32               i;
    stlInt32               j;
    stlExpTime             sExpTime;
    smfCtrlBuffer          sBuffer;
    stlBool                sIsValid;

    STL_TRY( smfOpenCtrlSectionWithPath( &sFile,
                                         aFilePath,
                                         SMF_CTRL_SECTION_LOG,
                                         &sIsValid, /* aIsValid */
                                         aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY_THROW( sIsValid == STL_TRUE, RAMP_ERR_CTRLFILE_CORRUPTED );

    SMF_INIT_CTRL_BUFFER( &sBuffer );

    stlPrintf( "\n [LOG SECTION]\n" );
    stlPrintf( "-----------------------------------------------------------\n" );
    
    STL_TRY( smfReadCtrlFile( &sFile,
                              &sBuffer,
                              (stlChar *)&sLogPersData,
                              STL_SIZEOF( smrLogPersData ),
                              aEnv ) == STL_SUCCESS );

    stlMakeExpTimeByLocalTz( &sExpTime, sLogPersData.mCreationTime );

    stlPrintf( "   DATABASE CREATION TIME : %04d-%02d-%02d %02d:%02d:%02d.%06d\n",
               sExpTime.mYear + 1900,
               sExpTime.mMonth + 1,
               sExpTime.mDay,
               sExpTime.mHour,
               sExpTime.mMinute,
               sExpTime.mSecond,
               sExpTime.mMicroSecond );

    stlPrintf( "\n  [CHECKPOINT]\n"
               "    LID                               :  %ld,%d,%d\n"
               "    LSN                               :  %ld\n"
               "    RECOVERY LSN                      :  %ld\n"
               "    ARCHIVELOG MODE                   :  %s\n"
               "    LAST INACTIVATED LOGFILE SEQUENCE :  %ld\n",
               SMR_FILE_SEQ_NO( &sLogPersData.mChkptLid ),
               SMR_BLOCK_SEQ_NO( &sLogPersData.mChkptLid ),
               SMR_BLOCK_OFFSET( &sLogPersData.mChkptLid ),
               sLogPersData.mChkptLsn,
               sLogPersData.mOldestLsn,
               (sLogPersData.mArchivelogMode == 1) ? "ARCHIVELOG" : "NOARCHIVELOG",
               sLogPersData.mLastInactiveLfsn );

    STL_TRY( smfReadCtrlFile( &sFile,
                              &sBuffer,
                              (stlChar *)&sLogStreamPersData,
                              STL_SIZEOF( smrLogStreamPersData ),
                              aEnv ) == STL_SUCCESS );

    stlPrintf( "\n  [LOG STREAM]\n"
               "    STATE          :  %s\n"
               "    GROUP COUNT    :  %d\n"
               "    BLOCK SIZE     :  %d\n"
               "    FILE SEQUENCE  :  %d\n",
               smrGetLogStreamStateString( sLogStreamPersData.mState ),
               sLogStreamPersData.mLogGroupCount,
               sLogStreamPersData.mBlockSize,
               sLogStreamPersData.mCurFileSeqNo );
    
    for( i = 0; i < sLogStreamPersData.mLogGroupCount; i++ )
    {
        STL_TRY( smfReadCtrlFile( &sFile,
                                  &sBuffer,
                                  (stlChar *)&sLogGroupPersData,
                                  STL_SIZEOF( smrLogGroupPersData ),
                                  aEnv ) == STL_SUCCESS );

        stlPrintf( "\n    [LOG GROUP #%d]\n"
                   "      STATE         : %s\n"
                   "      SIZE          : %ld\n"
                   "      MEMBER COUNT  : %d\n"
                   "      FILE SEQUENCE : %ld\n"
                   "      PREV LAST LSN : %ld\n",
                   sLogGroupPersData.mLogGroupId,
                   smrGetLogGroupStateString( sLogGroupPersData.mState ),
                   sLogGroupPersData.mFileSize,
                   sLogGroupPersData.mLogMemberCount,
                   sLogGroupPersData.mFileSeqNo,
                   sLogGroupPersData.mPrevLastLsn );

        for( j = 0; j < sLogGroupPersData.mLogMemberCount; j++ )
        {
            STL_TRY( smfReadCtrlFile( &sFile,
                                      &sBuffer,
                                      (stlChar *)&sLogMemberPersData,
                                      STL_SIZEOF( smrLogMemberPersData ),
                                      aEnv ) == STL_SUCCESS );
            stlPrintf( "      MEMBER #%d     : \"%s\"\n", j, sLogMemberPersData.mName );
        }
    }
    
    sState = 0;
    STL_TRY( smfCloseCtrlSection( &sFile,
                                  aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_RAMP( RAMP_ERR_CTRLFILE_CORRUPTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_CTRLFILE_CORRUPTED,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      aFilePath );
    }

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smfCloseCtrlSection( &sFile, aEnv );
    }

    return STL_FAILURE;
}

/** @} */

/**
 * @addtogroup smrRecoveryMgr
 * @{
 */

stlStatus smrAnalysis( smrLsn * aOldestLsn,
                       smrLid * aRedoLid,
                       smlScn * aSystemScn,
                       smlEnv * aEnv )
{
    smrLogCursor    sLogCursor;
    smrLid          sRedoLid = SMR_INVALID_LID;
    stlBool         sEndOfLog = STL_FALSE;
    smrLsn          sOldestLsn;
    smlScn          sSystemScn;
    knlRegionMark   sMemMark;
    stlInt32        sState = 0;
    stlBool         sIsChkptEnd = STL_FALSE;
    stlBool         sFound;
    smrTrans      * sTransTable = NULL;
    stlInt32        i;
    stlInt64        sTransTableSize;
    smrLogBuffer  * sLogBuffer;

    smrSetRecoveryPhase( SMR_RECOVERY_PHASE_ANALYSIS );

    STL_TRY_THROW( SMR_IS_VALID_LID( &gSmrWarmupEntry->mLogPersData.mChkptLid ),
                   RAMP_SUCCESS );

    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               KNL_ENV(aEnv) ) == STL_SUCCESS );
    sState = 1;

    sTransTableSize = smxlGetTransTableSize();
    
    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                sTransTableSize * STL_SIZEOF( smrTrans ),
                                (void*)&sTransTable,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    for( i = 0; i < sTransTableSize; i++ )
    {
        SMR_INIT_TRANS_TABLE( &sTransTable[i] );
    }

    STL_TRY( smrReadChkptLog( &gSmrWarmupEntry->mLogPersData.mChkptLid,
                              gSmrWarmupEntry->mLogPersData.mChkptLsn,
                              &sOldestLsn,
                              &sSystemScn,
                              aEnv ) == STL_SUCCESS );

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "[RESTART REOD] read checkpoint log - checkpoint log id(%ld,%d,%d), "
                        "oldest lsn(%ld), system scn(%ld)\n",
                        SMR_FILE_SEQ_NO(&gSmrWarmupEntry->mLogPersData.mChkptLid),
                        SMR_BLOCK_SEQ_NO(&gSmrWarmupEntry->mLogPersData.mChkptLid),
                        SMR_BLOCK_OFFSET(&gSmrWarmupEntry->mLogPersData.mChkptLid),
                        sOldestLsn,
                        sSystemScn ) == STL_SUCCESS );
    
    smxlSetSystemScn( sSystemScn );

    STL_TRY( smrOpenLogCursor( sOldestLsn,
                               &sLogCursor,
                               STL_FALSE, /* aIsForRecover */
                               aEnv ) == STL_SUCCESS );

    sLogBuffer = gSmrWarmupEntry->mLogStream.mLogBuffer;

    STL_TRY( smrFindFileBlock( &gSmrWarmupEntry->mLogPersData.mChkptLid,
                               sOldestLsn,
                               sLogBuffer->mBuffer,
                               sLogBuffer->mBufferSize,
                               &sRedoLid,
                               &sFound,
                               aEnv ) == STL_SUCCESS );

    STL_ASSERT( sFound == STL_TRUE );

    STL_TRY( smrPrepareRedo( sRedoLid,
                             sOldestLsn,
                             aEnv ) == STL_SUCCESS );

    *aOldestLsn = sOldestLsn;
    *aRedoLid   = sRedoLid;
    *aSystemScn = sSystemScn;

    STL_TRY( smrReadNextLog( &sLogCursor,
                             &sEndOfLog,
                             aEnv ) == STL_SUCCESS );

    while( sEndOfLog == STL_FALSE )
    {
        STL_TRY( smrAnalyzeLog( &sLogCursor,
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
        
        STL_TRY( smrReadNextLog( &sLogCursor,
                                 &sEndOfLog,
                                 aEnv ) == STL_SUCCESS );
    }

    /**
     * OldestLsn 당시 Transaction Table을 재구성한다.
     */
    for( i = 0; i < sTransTableSize; i++ )
    {
        if( (sTransTable[i].mTransId != SML_INVALID_TRANSID) &&
            (sTransTable[i].mBeginLsn == 0) )
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

    STL_TRY( smrCloseLogCursor( &sLogCursor,
                                aEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_TRUE, /* aFreeChunk */
                                       (knlEnv*)aEnv ) == STL_SUCCESS );
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                        &sMemMark,
                                        STL_TRUE, /* aFreeChunk */
                                        (knlEnv*)aEnv );
    }
    
    return STL_FAILURE;
}


stlStatus smrRestartRedo( smrLsn   aOldestLsn,
                          smrLid   aRedoLid,
                          smlScn   aSystemScn,
                          smlEnv * aEnv )
{
    smrLogBuffer  * sLogBuffer;
    smrLogCursor    sLogCursor;
    stlBool         sEndOfLog = STL_FALSE;
    knlRegionMark   sMemMark;
    stlInt32        sState = 0;
    smpFlushArg     sArg;

    smrSetRecoveryPhase( SMR_RECOVERY_PHASE_REDO );

    STL_TRY_THROW( SMR_IS_VALID_LID( &gSmrWarmupEntry->mLogPersData.mChkptLid ),
                   RAMP_SUCCESS );

    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( smrOpenLogCursor( aOldestLsn,
                               &sLogCursor,
                               STL_TRUE, /* aIsForRecover */
                               aEnv ) == STL_SUCCESS );
    
    STL_TRY( smrPrepareRedo( aRedoLid,
                             aOldestLsn,
                             aEnv ) == STL_SUCCESS );

    sLogBuffer = gSmrWarmupEntry->mLogStream.mLogBuffer;

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "[RESTART REDO] ready to redo - start lid(%ld,%d,%d), lsn(%ld)\n",
                        SMR_FILE_SEQ_NO(&(sLogBuffer->mRearLid)),
                        SMR_BLOCK_SEQ_NO(&(sLogBuffer->mRearLid)),
                        SMR_BLOCK_OFFSET(&(sLogBuffer->mRearLid)),
                        sLogBuffer->mFrontLsn ) == STL_SUCCESS );

    STL_TRY( smrReadNextLog( &sLogCursor,
                             &sEndOfLog,
                             aEnv ) == STL_SUCCESS );

    while( sEndOfLog == STL_FALSE )
    {
        STL_TRY( smrRedo( &sLogCursor,
                          aEnv ) == STL_SUCCESS );
        
        STL_TRY( smrReadNextLog( &sLogCursor,
                                 &sEndOfLog,
                                 aEnv ) == STL_SUCCESS );
    }

    STL_TRY( smrCloseLogCursor( &sLogCursor,
                                aEnv ) == STL_SUCCESS );

    STL_DASSERT( SMR_IS_LESS_THAN_EQUAL_LID( (&gSmrWarmupEntry->mLogPersData.mChkptLid),
                                             (&sLogBuffer->mRearLid) )
                 == STL_TRUE );

    STL_TRY( smrClearPartialLog( aEnv ) == STL_SUCCESS );

    /**
     * ACTIVE 로그 그룹을 INACTIVE로 변경하기 위해서 수행한다.
     * - 해당 과정이 생략되면 LOG SWITCHING GAP을 보장할수 없다.
     */
    sArg.mBehavior = SMP_FLUSH_DIRTY_PAGE;
    sArg.mLogging = STL_FALSE;
    sArg.mForCheckpoint = STL_FALSE;

    STL_TRY( knlAddEnvEvent( SML_EVENT_PAGE_FLUSH,
                             NULL,            /* aEventMem */
                             &sArg,
                             STL_SIZEOF(smpFlushArg),
                             SML_PAGE_FLUSHER_ENV_ID,
                             KNL_EVENT_WAIT_BLOCKED,
                             STL_TRUE, /* aForceSuccess */
                             KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    /**
     * Recovery 완료 후 datafile header에 최신 checkpoint Lsn을 설정한다.
     */
    STL_TRY( smfChangeDatafileHeader( NULL,   /* aStatement */
                                      SMF_CHANGE_DATAFILE_HEADER_FOR_GENERAL,
                                      SML_INVALID_TBS_ID,
                                      SML_INVALID_DATAFILE_ID,
                                      SMR_INVALID_LID,
                                      SMR_INVALID_LSN,
                                      aEnv )
             == STL_SUCCESS );

    STL_TRY( smrResetLog( aEnv ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_TRUE, /* aFreeChunk */
                                       (knlEnv*)aEnv ) == STL_SUCCESS );
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                        &sMemMark,
                                        STL_TRUE, /* aFreeChunk */
                                        (knlEnv*)aEnv );
    }
    
    return STL_FAILURE;
}

stlStatus smrRestartUndo( smlEnv * aEnv )
{
    smxlTransId        sTransId;
    smxlTransSnapshot  sTransSnapshot;

    smrSetRecoveryPhase( SMR_RECOVERY_PHASE_UNDO );

    STL_TRY( smxlGetFirstActiveTrans( &sTransId,
                                      &sTransSnapshot,
                                      aEnv ) == STL_SUCCESS );

    while( sTransId != SML_INVALID_TRANSID )
    {
        if( sTransSnapshot.mState == SMXL_STATE_PREPARE )
        {
            STL_TRY( smxlPreparePreparedTransaction( sTransId,
                                                     aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            STL_TRY( smxlRestartRollback( sTransId, aEnv ) == STL_SUCCESS );
            STL_TRY( smxlFreeTrans( sTransId,
                                    SML_PEND_ACTION_ROLLBACK,
                                    STL_FALSE, /* aCleanup */
                                    SML_TRANSACTION_CWM_WAIT,
                                    aEnv ) == STL_SUCCESS );
        }

        STL_TRY( smxlGetNextActiveTrans( &sTransId,
                                         &sTransSnapshot,
                                         aEnv ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrClearPartialLog( smlEnv * aEnv )
{
    smrLogStream * sLogStream;
    smrLogBuffer * sLogBuffer;
    stlInt32       sClearBlockCount;
    smrLid         sPartialLid;

    sLogStream = &gSmrWarmupEntry->mLogStream;
    sLogBuffer = sLogStream->mLogBuffer;

    STL_TRY_THROW( sLogBuffer->mPartialLogDetected == STL_TRUE, RAMP_SUCCESS );

    sPartialLid = sLogBuffer->mRearLid;
    
    STL_TRY( smrClearPartialLogBlocks( &sPartialLid,
                                       &sClearBlockCount,
                                       aEnv )
             == STL_SUCCESS );

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "[RESTART REDO] clear partial log - detected lid(%d,%d,%d), "
                        "lsn(%ld), clear blocks(%d)\n",
                        SMR_FILE_SEQ_NO( &sPartialLid ),
                        SMR_BLOCK_SEQ_NO( &sPartialLid ),
                        SMR_BLOCK_OFFSET( &sPartialLid ),
                        gSmrWarmupEntry->mLsn,
                        sClearBlockCount )
             == STL_SUCCESS );

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrResetLog( smlEnv * aEnv )
{
    smrLogStream * sLogStream;
    smrLogBuffer * sLogBuffer;
    smrLid         sRestartLid;
    stlBool        sEndOfFile = STL_FALSE;
    smrLogGroup  * sLogGroup = NULL;

    sLogStream = &gSmrWarmupEntry->mLogStream;
    sLogBuffer = sLogStream->mLogBuffer;

    /**
     * find current logfile group
     */
    STL_RING_FOREACH_ENTRY( &sLogStream->mLogGroupList, sLogGroup )
    {
        if( sLogGroup->mLogGroupPersData.mState == SMR_LOG_GROUP_STATE_CURRENT )
        {
            break;
        }
    }

    /**
     * rear가 가리키는 logfile group이 CURRENT가 아니라면
     * ALTER SYSTEM SWITCH LOGFILE 이 발생한 경우
     */
    if( sLogGroup->mLogGroupPersData.mFileSeqNo ==
        SMR_FILE_SEQ_NO( &sLogBuffer->mRearLid ) )
    {
        STL_TRY( smrMoveWritableFileBlock( &sLogBuffer->mRearLid,
                                           &sRestartLid,
                                           &sEndOfFile,
                                           aEnv ) == STL_SUCCESS );

        if( sEndOfFile == STL_TRUE )
        {
            STL_TRY( smrLogSwitching( sLogStream,
                                      STL_FALSE,  /* aFlushLog */
                                      STL_TRUE,   /* aFileSwitching */
                                      STL_FALSE,  /* aDoCheckpoint */
                                      aEnv ) == STL_SUCCESS );

            SMR_MAKE_LID( &sRestartLid,
                          sRestartLid.mFileSeqNo + 1,
                          0,
                          SMR_BLOCK_HDR_SIZE );
        }
    }
    else
    {
        /**
         * ALTER SYSTEM SWITCH LOGFILE 이 발생한 경우
         */
        SMR_MAKE_LID( &sRestartLid,
                      SMR_FILE_SEQ_NO(&sLogBuffer->mRearLid) + 1,
                      0,
                      SMR_BLOCK_HDR_SIZE );
    }
    
    sLogBuffer->mBlockOffset = SMR_BLOCK_HDR_SIZE;
    sLogBuffer->mRearFileBlockSeqNo = SMR_BLOCK_SEQ_NO( &sRestartLid );
    sLogBuffer->mFrontFileBlockSeqNo = 0;
    sLogBuffer->mFileSeqNo = SMR_FILE_SEQ_NO( &sRestartLid );
    sLogBuffer->mRearLsn = gSmrWarmupEntry->mLsn - 1;
    sLogBuffer->mFrontLsn = gSmrWarmupEntry->mLsn - 1;
    sLogBuffer->mRearLid = sRestartLid;
    sLogBuffer->mFrontSbsn = -1;
    sLogBuffer->mRearSbsn = 0;
    sLogBuffer->mLeftLogCountInBlock = 0;
    sLogBuffer->mPartialLogDetected = STL_FALSE;
    sLogBuffer->mEndOfLog = STL_FALSE;
    sLogBuffer->mEndOfFile = STL_FALSE;

    STL_RING_FOREACH_ENTRY( &(sLogStream->mLogGroupList), sLogGroup )
    {
        sLogGroup->mLogSwitchingSbsn = -1;
    }
    
    STL_TRY( smrFormatLogBuffer( sLogBuffer->mBuffer,
                                 sLogBuffer->mBufferSize,
                                 sLogStream->mLogStreamPersData.mBlockSize,
                                 aEnv )
             == STL_SUCCESS );

    STL_TRY( smrResetLogFile( sLogStream,
                              &sRestartLid,
                              aEnv ) == STL_SUCCESS );
    
    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "[RESTART REDO] reset logstream - restart lid(%ld,%d,%d)\n",
                        SMR_FILE_SEQ_NO(&sRestartLid),
                        SMR_BLOCK_SEQ_NO(&sRestartLid),
                        SMR_BLOCK_OFFSET(&sRestartLid) ) == STL_SUCCESS );
    
    /**
     * 모든 Dirty Page들이 디스크로 내려갔음을 보장하기 때문에
     * ACTIVE 로그 그룹들을 INACTVIE 로 변경한다.
     */

    STL_TRY( smrLogArchiving( sLogBuffer->mRearLsn,
                              aEnv )
             == STL_SUCCESS );

    STL_TRY( smfSaveCtrlFile( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrPrepareRedo( smrLid    aRedoLid,
                          smrLsn    aOldestLsn,
                          smlEnv  * aEnv )
{
    smrLogStream   * sLogStream;
    smrLogBuffer   * sLogBuffer;
    stlInt64         sReadBytes;
    stlInt16         sBlockSize;
    smrLsn           sFrontLsn;
    smrLogBlockHdr   sLogBlockHdr;
    stlInt32         sLeftLogCount = 0;
    stlInt16         sBlockOffset;

    sLogStream = &gSmrWarmupEntry->mLogStream;
    sLogBuffer = sLogStream->mLogBuffer;
    sBlockSize = sLogStream->mLogStreamPersData.mBlockSize;
    sLogBuffer->mEndOfFile = STL_FALSE;
    sLogBuffer->mEndOfLog = STL_FALSE;

    STL_TRY( smrFindLogGroupBySeqNo( sLogStream,
                                     SMR_FILE_SEQ_NO( &aRedoLid ),
                                     &sLogStream->mCurLogGroup4Buffer,
                                     aEnv ) == STL_SUCCESS );
    
    sLogStream->mCurLogGroup4Disk = sLogStream->mCurLogGroup4Buffer;

    STL_TRY( smrReadFileBlocks( &aRedoLid,
                                sLogBuffer->mBuffer,
                                sLogBuffer->mBufferSize,
                                &sReadBytes,
                                &sLogBuffer->mEndOfFile,
                                &sLogBuffer->mEndOfLog,
                                aEnv ) == STL_SUCCESS );

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

        SMR_MAKE_LID( &aRedoLid,
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
        SMR_MAKE_LID( &aRedoLid,
                      SMR_FILE_SEQ_NO( &aRedoLid ),
                      SMR_BLOCK_SEQ_NO( &aRedoLid ),
                      SMR_BLOCK_HDR_SIZE );
            
        sLogBuffer->mFrontSbsn = -1;
        sLogBuffer->mRearSbsn = 0;
        sLogBuffer->mLeftLogCountInBlock = 0;
        sLogBuffer->mFrontLsn = 0;
        sLogBuffer->mRearLsn = 0;
    }

    sLogBuffer->mBlockOffset = SMR_BLOCK_OFFSET( &aRedoLid );
    sLogBuffer->mFrontFileBlockSeqNo = SMR_BLOCK_SEQ_NO( &aRedoLid );
    sLogBuffer->mRearFileBlockSeqNo = sLogBuffer->mFrontFileBlockSeqNo + (sReadBytes / sBlockSize);
    sLogBuffer->mFileSeqNo = SMR_FILE_SEQ_NO( &aRedoLid );
    sLogBuffer->mRearLid = aRedoLid;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

inline void smrSetArchivelogMode( stlInt32   aMode )
{
    gSmrWarmupEntry->mLogPersData.mArchivelogMode = aMode;
}

inline stlInt32 smrGetArchivelogMode()
{
    return gSmrWarmupEntry->mLogPersData.mArchivelogMode;
}

/**
 * @brief ALTER DATABASE (ARCHIVELOG | NOARCHIVELOG) 로 변경되는 archivelog mode를 controlfile에 반영한다.
 * @param[in] aMode 변경할 archivelog mode (ARCHIVELOG or NOARCHIVELOG)
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smrChangeArchivelogMode( stlInt32   aMode,
                                   smlEnv   * aEnv )
{
    smrSetArchivelogMode( aMode );

    STL_TRY( smfSaveCtrlFile( aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrAlterArchivelogEventHandler( void      * aData,
                                          stlUInt32   aDataSize,
                                          stlBool   * aDone,
                                          void      * aEnv )
{
    stlInt32    sMode;

    sMode = *(stlInt32 *)aData;

    STL_DASSERT( smlEnterStorageManager( SML_ENV(aEnv) ) == STL_SUCCESS );
    STL_DASSERT( smfGetDataAccessMode() == SML_DATA_ACCESS_MODE_READ_WRITE );

    STL_TRY_THROW( knlGetCurrStartupPhase() == KNL_STARTUP_PHASE_MOUNT,
                   RAMP_ERR_ONLY_MOUNT_PHASE );

    /**
     * CDS나 DS모드에서는 ARCHIVELOG를 지원할 수 없다.
     */
    if( smxlGetDataStoreMode() != SML_DATA_STORE_MODE_TDS )
    {
        STL_TRY_THROW( sMode == SML_NOARCHIVELOG_MODE,
                       RAMP_ERR_TDS_ONLY_ARCHIVELOG );
    }

    /**
     * INCONSISTENT 상태인 Offline Tablespace가 존재하는지 체크한다.
     * 만약 있으면 ARCHIVELOG -> NOARCHIVELOG로 변경할 수 없다.
     */
    if( sMode == SML_NOARCHIVELOG_MODE )
    {
        STL_TRY( smfValidateOfflineTbsConsistency( SML_ENV(aEnv) ) == STL_SUCCESS );
    }

    STL_TRY( smrChangeArchivelogMode( sMode, SML_ENV(aEnv) ) == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "ALTER DATABASE %s\n",
                        (sMode == SML_ARCHIVELOG_MODE)? "ARCHIVELOG": "NOARCHIVELOG" )
             == STL_SUCCESS );

    *aDone = STL_TRUE;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_ONLY_MOUNT_PHASE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_NOT_APPROPRIATE_PHASE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      gPhaseString[KNL_STARTUP_PHASE_MOUNT] );
    }

    STL_CATCH( RAMP_ERR_TDS_ONLY_ARCHIVELOG )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_UNABLE_TO_ARCHIVELOG_IN_CDS_MODE,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    *aDone = STL_TRUE;

    return STL_FAILURE;
}

stlUInt64 smrGetRecoveryPhase()
{
    return gSmrWarmupEntry->mRecoveryInfo.mRecoveryPhase;
}

void smrSetRecoveryPhase( stlUInt64   aRecoveryPhase )
{
    gSmrWarmupEntry->mRecoveryInfo.mRecoveryPhase = aRecoveryPhase;
}

void smrActivateLogFlusher()
{
    gSmrWarmupEntry->mEnableFlushing = STL_TRUE;
}

void smrDeactivateLogFlusher()
{
    stlAcquireSpinLock( &(gSmrWarmupEntry->mSpinLock),
                        NULL /* Miss count */ );
    
    gSmrWarmupEntry->mEnableFlushing = STL_FALSE;
    
    stlReleaseSpinLock( &(gSmrWarmupEntry->mSpinLock) );
}

stlStatus smrValidateCheckpointLog( stlUInt8   aLogOption,
                                    smlEnv   * aEnv )
{
    /**
     * Incomplete media recovery를 수행한 후 반드시 RESETLOGS로 STARTUP해야 한다.
     */
    if( aLogOption == SML_STARTUP_LOG_OPTION_RESETLOGS )
    {
        STL_TRY_THROW( gSmrWarmupEntry->mLogPersData.mNeedResetLogs == STL_TRUE,
                       RAMP_ERR_CANNOT_OPEN_RESETLOGS );
    }
    else
    {
        STL_ASSERT( aLogOption == SML_STARTUP_LOG_OPTION_NORESETLOGS );

        /**
         * Incomplete media recovery가 아닌 경우 RESETLOGS로 STARTUP할 수 없다.
         */
        STL_TRY_THROW( gSmrWarmupEntry->mLogPersData.mNeedResetLogs != STL_TRUE,
                       RAMP_ERR_MUST_OPEN_RESETLOGS );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CANNOT_OPEN_RESETLOGS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_CANNOT_OPEN_RESETLOGS,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_MUST_OPEN_RESETLOGS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_MUST_OPEN_RESETLOGS,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrAdjustLogSection( smlEnv * aEnv )
{
    smrLogStream * sLogStream;
    smrLogBuffer * sLogBuffer;
    smrLogGroup  * sLogGroup = NULL;
    smrLogMember * sLogMember;
    stlInt16       sLogGroupId;
    stlBool        sIsFirst = STL_TRUE;

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "[OPEN RESETLOGS] adjust lsn(%ld), scn(%ld)\n",
                        gSmrWarmupEntry->mLogPersData.mOldestLsn,
                        smfGetResetLogsScn() )
             == STL_SUCCESS );

    /**
     * Log Buffer 초기화
     */
    sLogStream = &gSmrWarmupEntry->mLogStream;
    sLogBuffer = sLogStream->mLogBuffer;

    sLogBuffer->mBlockOffset = SMR_BLOCK_HDR_SIZE;
    sLogBuffer->mLeftLogCountInBlock = 0;
    sLogBuffer->mPartialLogDetected = STL_FALSE;
    sLogBuffer->mEndOfLog = STL_FALSE;
    sLogBuffer->mEndOfFile = STL_FALSE;
    sLogBuffer->mFileSeqNo = 0;
    sLogBuffer->mRearFileBlockSeqNo = 0;
    sLogBuffer->mFrontFileBlockSeqNo = 0;
    sLogBuffer->mRearLsn = SMR_INVALID_LSN;
    sLogBuffer->mFrontLsn = SMR_INVALID_LSN;
    sLogBuffer->mRearLid = (smrLid){0,0,0};
    sLogBuffer->mRearSbsn = 0;
    sLogBuffer->mFrontSbsn = -1;
        
    STL_TRY( smrFormatLogBuffer( sLogBuffer->mBuffer,
                                 sLogBuffer->mBufferSize,
                                 sLogStream->mLogStreamPersData.mBlockSize,
                                 aEnv )
             == STL_SUCCESS );

    gSmrWarmupEntry->mLsn = gSmrWarmupEntry->mLogPersData.mOldestLsn;

    /**
     * Controlfile의 log section을 초기화한다.
     */
    gSmrWarmupEntry->mLogPersData.mChkptLid = SMR_INVALID_LID;
    gSmrWarmupEntry->mLogPersData.mChkptLsn = SMR_INVALID_LSN;
    gSmrWarmupEntry->mLogPersData.mOldestLsn = SMR_INVALID_LSN;
    gSmrWarmupEntry->mLogPersData.mCreationTime = stlNow();
    gSmrWarmupEntry->mLogPersData.mLastInactiveLfsn = SMR_INVALID_LOG_FILE_SEQ_NO;

    sLogStream->mLogStreamPersData.mCurFileSeqNo = 0;

    STL_RING_FOREACH_ENTRY( &(sLogStream->mLogGroupList), sLogGroup )
    {
        sLogGroupId = sLogGroup->mLogGroupPersData.mLogGroupId;

        STL_RING_FOREACH_ENTRY( &(sLogGroup->mLogMemberList), sLogMember )
        {
            STL_TRY( smrFormatLogFile( sLogMember->mLogMemberPersData.mName,
                                       sLogGroup->mLogGroupPersData.mFileSize,
                                       sLogStream->mLogStreamPersData.mBlockSize,
                                       STL_TRUE,
                                       aEnv ) == STL_SUCCESS );
        }

        sLogGroup->mLogGroupPersData.mLogGroupId = sLogGroupId;

        if( sIsFirst == STL_TRUE )
        {
            sIsFirst = STL_FALSE;
            sLogGroup->mLogGroupPersData.mState = SMR_LOG_GROUP_STATE_CURRENT;
            sLogGroup->mLogGroupPersData.mFileSeqNo =
                sLogStream->mLogStreamPersData.mCurFileSeqNo;

            sLogGroup->mLogGroupPersData.mPrevLastLsn = smrGetSystemLsn() - 1;
            sLogStream->mCurLogGroup4Disk = sLogGroup;
            sLogStream->mCurLogGroup4Buffer = sLogGroup;
        }
        else
        {
            sLogGroup->mLogGroupPersData.mState = SMR_LOG_GROUP_STATE_UNUSED;
            sLogGroup->mLogGroupPersData.mPrevLastLsn = SMR_INVALID_LSN;
            sLogGroup->mLogGroupPersData.mFileSeqNo = -1;
        }

        sLogGroup->mCurBlockSeq = 0;
        sLogGroup->mLogSwitchingSbsn = -1;

        STL_TRY( smrWriteLogfileHdr( sLogStream,
                                     sLogGroup,
                                     NULL,
                                     aEnv ) == STL_SUCCESS );
    }

    gSmrWarmupEntry->mLogPersData.mNeedResetLogs = STL_FALSE;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
