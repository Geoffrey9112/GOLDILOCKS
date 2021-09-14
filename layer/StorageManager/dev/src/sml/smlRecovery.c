/*******************************************************************************
 * smlRecovery.c
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
#include <smDef.h>
#include <smgDef.h>
#include <smrDef.h>
#include <smf.h>
#include <smr.h>
#include <smp.h>
#include <smg.h>
#include <sms.h>
#include <smxl.h>
#include <smxm.h>
#include <smlGeneral.h>
#include <smrLogMember.h>

extern smrWarmupEntry * gSmrWarmupEntry;
extern stlInt64         gDataStoreMode;

/**
 * @file smlRecovery.c
 * @brief Storage Manager Layer Recovery Routines
 */

/**
 * @addtogroup smlRecovery
 * @{
 */

/**
 * @brief 비정상 종료상황에서 시스템을 정상상태로 회복한다.
 * @param[in] aLogOption 복구 후 startup 시 Log option(NORESETLOGS/RESETLOGS)
 * @param[in,out] aEnv Environment 정보
 * @note 서버가 비정상 종료된 상황에서는 Recovery 절차를 수행하는 반면,
 * 정상적으로 종료된 상황에서는 별도의 Recovery 절차를 수행하지 않는다.
 */
stlStatus smlRecover( stlUInt8   aLogOption,
                      smlEnv   * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    STL_DASSERT( smfGetDataAccessMode() == SML_DATA_ACCESS_MODE_READ_WRITE );
    
    STL_TRY( smrRecover( aLogOption, aEnv ) == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief Flusher에서 log를 flush한다.
 * @param[in] aFlushAll 기록된 모든 로그를 기록할지 여부
 * @param[out] aFlushedLsn Flush 된 마지막 LSN 값
 * @param[out] aFlushedSbsn Flush 된 마지막 SBSN 값
 * @param[in,out] aEnv Environment 정보
 * @note Log Flusher에서만 사용한다.
 */
stlStatus smlFlushLogForFlusher( stlBool    aFlushAll,
                                 stlInt64 * aFlushedLsn,
                                 stlInt64 * aFlushedSbsn,
                                 smlEnv   * aEnv )
{
    stlInt32 sState = 0;
    
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    STL_DASSERT( smfGetDataAccessMode() == SML_DATA_ACCESS_MODE_READ_WRITE );
    STL_DASSERT( smxlGetDataStoreMode() == SML_DATA_STORE_MODE_TDS );

    stlAcquireSpinLock( &(gSmrWarmupEntry->mSpinLock),
                        NULL /* Miss count */ );
    sState = 1;
    
    STL_TRY_THROW( gSmrWarmupEntry->mEnableFlushing == STL_TRUE, RAMP_SUCCESS );
    
    if( aFlushAll == STL_TRUE )
    {
        STL_TRY( smrFlushAllLogs( STL_TRUE /* aForceDiskIo */,
                                  aEnv )
                 == STL_SUCCESS );
        
        *aFlushedLsn = smrGetFrontLsn();
        *aFlushedSbsn = smrGetFrontSbsn();
    }
    else
    {
        STL_TRY( smrFlushStreamLogForFlusher( aFlushedLsn,
                                              aFlushedSbsn,
                                              aEnv )
                 == STL_SUCCESS );
    }

    STL_RAMP( RAMP_SUCCESS );
    
    sState = 0;
    stlReleaseSpinLock( &(gSmrWarmupEntry->mSpinLock) );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        stlReleaseSpinLock( &(gSmrWarmupEntry->mSpinLock) );
    }

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 기록된 모든 로그를 디스크로 내린다.
 * @param[in,out] aEnv  Environment 정보
 */
stlStatus smlFlushAllLogs( smlEnv * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    STL_DASSERT( smfGetDataAccessMode() == SML_DATA_ACCESS_MODE_READ_WRITE );
    
    if( smxlGetDataStoreMode() == SML_DATA_STORE_MODE_TDS )
    {
        /**
         * Flush를 대기하지 않는 경우(ALTER SYSTEM FLUSH LOGS),
         * Flush logs가 enabled가 아니면 exception
         * - Pending Log의 apply시 block될수도 있다.
         */
        STL_TRY_THROW( gSmrWarmupEntry->mBlockLogFlushing == 0,
                       RAMP_ERR_DISABLED_LOG_FLUSHING );
        
        STL_TRY( smrFlushAllLogs( STL_FALSE /* aForceDiskIo */,
                                  aEnv )
                 == STL_SUCCESS );
    }
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_DISABLED_LOG_FLUSHING )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_DISABLED_LOG_FLUSHING,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 체크포인트를 수행한다.
 * @param[in,out] aEnv     Environment 정보
 */
stlStatus smlCheckpoint( smlEnv  * aEnv )
{
    smrChkptArg    sArg;

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY_THROW( gDataStoreMode == SML_DATA_STORE_MODE_TDS, RAMP_ERR_NOT_SUPPORT );
    
    STL_TRY_THROW( smfGetDataAccessMode() == SML_DATA_ACCESS_MODE_READ_WRITE,
                   RAMP_ERR_READ_ONLY_DATABASE );

    /**
     * ALTER SYSTEM CHECKPINT도 Event로 수행한다.
     */
    stlMemset( (void *)&sArg, 0x00, STL_SIZEOF(smrChkptArg) );
    sArg.mLogfileSeq = STL_INT64_MAX;
    sArg.mFlushBehavior = SMP_FLUSH_LOGGED_PAGE;

    STL_TRY( knlAddEnvEvent( SML_EVENT_CHECKPOINT,
                             NULL,            /* aEventMem */
                             (void*)&sArg,
                             STL_SIZEOF( smrChkptArg ),
                             SML_CHECKPOINT_ENV_ID,
                             KNL_EVENT_WAIT_POLLING,
                             STL_FALSE,  /* aForceSuccess */
                             KNL_ENV( aEnv ) ) == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_SUPPORT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_UNABLE_CHECKPOINT_IN_CDS_MODE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }
    
    STL_CATCH( RAMP_ERR_READ_ONLY_DATABASE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_DATABASE_IS_READ_ONLY,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief Shutdown시에 체크포인트를 수행한다.
 * @param[in,out] aEnv  Environment 정보
 */
stlStatus smlCheckpoint4Shutdown( smlEnv * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY_THROW( knlGetCurrStartupPhase() >= KNL_STARTUP_PHASE_OPEN,
                   RAMP_SUCCESS );
    STL_TRY_THROW( smfGetDataAccessMode() == SML_DATA_ACCESS_MODE_READ_WRITE,
                   RAMP_SUCCESS );
    STL_TRY_THROW( smfGetServerState() != SML_SERVER_STATE_RECOVERING,
                   RAMP_SUCCESS );
        
    STL_TRY( smpFlushPage4Shutdown( aEnv ) == STL_SUCCESS );

    STL_TRY( smrWriteChkptLog( gSmrWarmupEntry->mLsn,
                               aEnv ) == STL_SUCCESS );

    smfSetServerState( SML_SERVER_STATE_SHUTDOWN );
    smfSetDataStoreMode( smxlGetDataStoreMode() );

    STL_RAMP( RAMP_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief Shutdown시 Dirty Page들을 Flush한다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smlFlushPages4Shutdown( smlEnv * aEnv )
{
    smpFlushArg sArg;

    sArg.mBehavior = SMP_FLUSH_DIRTY_PAGE;
    sArg.mLogging = STL_TRUE;
    sArg.mForCheckpoint = STL_FALSE;
    
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    STL_TRY_THROW( knlGetCurrStartupPhase() >= KNL_STARTUP_PHASE_OPEN,
                   RAMP_SUCCESS );
    STL_TRY_THROW( smlGetDataAccessMode() == SML_DATA_ACCESS_MODE_READ_WRITE,
                   RAMP_SUCCESS );

    STL_TRY( knlAddEnvEvent( SML_EVENT_PAGE_FLUSH,
                             NULL,            /* aEventMem */
                             &sArg,
                             STL_SIZEOF(smpFlushArg),
                             SML_PAGE_FLUSHER_ENV_ID,
                             KNL_EVENT_WAIT_POLLING,
                             STL_TRUE,  /* aForceSuccess */
                             KNL_ENV( aEnv ) ) == STL_SUCCESS );

    /**
     * Datafile을 Flush한 후 마지막으로 수행된 checkpoint lsn을
     * datafile header에 기록한다.
     */
    STL_TRY( smfChangeDatafileHeader( NULL,
                                      SMF_CHANGE_DATAFILE_HEADER_FOR_GENERAL,
                                      SML_INVALID_TBS_ID,
                                      SML_INVALID_DATAFILE_ID,
                                      SMR_INVALID_LID,
                                      SMR_INVALID_LSN,
                                      aEnv )
             == STL_SUCCESS );

    STL_RAMP( RAMP_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief Log Flusher의 상태를 설정한다.
 * @param[in] aState 설정할 Log Flusher State
 * @param[in,out] aEnv Environment 정보
 * @see aState : smlLogFlusherState
 */
stlStatus smlSetLogFlusherState( stlInt16   aState,
                                 smlEnv   * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( smrSetLogFlusherState( aState,
                                    aEnv ) == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief Checkpointer의 상태를 설정한다.
 * @param[in] aState 설정할 Checkpointer State
 * @param[in,out] aEnv Environment 정보
 * @see aState : smlCheckpointerState
 */
stlStatus smlSetCheckpointerState( stlInt16   aState,
                                 smlEnv   * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( smrSetCheckpointerState( aState,
                                      aEnv ) == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief Log Group을 switch한다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smlSwitchLogGroup( smlEnv * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    STL_DASSERT( smfGetDataAccessMode() == SML_DATA_ACCESS_MODE_READ_WRITE );
    
    STL_TRY( smrSwitchLogGroup( aEnv ) == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief Supplemental DDL Log를 기록한다.
 * @param[in] aTransId    Transaction Identifier
 * @param[in] aLogData    기록할 로그 데이터
 * @param[in] aLogSize    aLogData의 바이트 단위 크기
 * @param[in] aStmtType   Statement Type
 * @param[in] aPhysicalId 관련된 Relation의 Physical Identifier
 * @param[in,out] aEnv    Environment 정보
 * @see aLogType : smlSupplementalLogType
 */
stlStatus smlWriteSupplementalDdlLog( smlTransId     aTransId,
                                      void         * aLogData,
                                      stlUInt16      aLogSize,
                                      stlUInt32      aStmtType,
                                      stlInt64       aPhysicalId,
                                      smlEnv       * aEnv )
{
    smlRid          sSegRid;
    smlRid          sUndoRid;
    smxmTrans       sMiniTrans;
    stlInt32        sState = 0;
    knlRegionMark   sMemMark;
    stlChar       * sLogBuffer;
    stlInt32        sLogOffset = 0;
    
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    STL_DASSERT( smfGetDataAccessMode() == SML_DATA_ACCESS_MODE_READ_WRITE );
    STL_DASSERT( aLogSize <= SML_MAX_SUPPLEMENTAL_DDL_LOG_SIZE );
    
    stlMemcpy( &sSegRid, &aPhysicalId, STL_SIZEOF(smlRid) );
    
    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                aLogSize + STL_SIZEOF(stlUInt32),
                                (void**)&sLogBuffer,
                                (knlEnv*)aEnv ) == STL_SUCCESS );

    STL_WRITE_MOVE_INT32( sLogBuffer, &aStmtType, sLogOffset );
    STL_WRITE_MOVE_BYTES( sLogBuffer, aLogData, aLogSize, sLogOffset );

    STL_TRY( smxmBegin( &sMiniTrans,
                        SMXL_TO_TRANS_ID( aTransId ),
                        sSegRid,
                        SMXM_ATTR_REDO,
                        aEnv )
             == STL_SUCCESS );
    sState = 2;

    STL_TRY( smxlInsertUndoRecord( &sMiniTrans,
                                   SMG_COMPONENT_EXTERNAL,
                                   SMG_UNDO_SUPPL_LOG_DDL,
                                   sLogBuffer,
                                   sLogOffset,
                                   sSegRid,
                                   &sUndoRid,
                                   aEnv ) == STL_SUCCESS );
    
    sState = 1;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    STL_TRY( smrWriteSupplementalLog( SMXL_TO_TRANS_ID( aTransId ),
                                      (stlChar*)sLogBuffer,
                                      sLogOffset,
                                      SML_SUPPL_LOG_DDL,
                                      sSegRid,
                                      aEnv )
             == STL_SUCCESS );

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       (knlEnv*)aEnv ) == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
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

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 해당 세그먼트를 Irrecoverable Segment로 추가한다.
 * @param[in] aSegmentId 대상 Segment Identifier
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smlAddIrrecoverableSegment( stlInt64    aSegmentId,
                                      smlEnv    * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );

    STL_TRY( smsAddIrrecoverableSegment( aSegmentId,
                                         aEnv )
             == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}   

/**
 * @brief 주어진 Logfile의 이름을 절대 경로로 변경한다.
 * @param[in,out] aFileName Merge할 Logfile Name
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smlConvAbsLogfilePath( stlChar  * aFileName,
                                 smlEnv   * aEnv )
{
    stlChar   sFileName[STL_MAX_FILE_PATH_LENGTH];
    stlChar   sLogDir[STL_MAX_FILE_PATH_LENGTH];
    stlInt32  sFileNameLen;
    
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( knlGetPropertyValueByName( "LOG_DIR",
                                        sLogDir,
                                        KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    STL_TRY( stlMergeAbsFilePath( sLogDir,
                                  aFileName,
                                  sFileName,
                                  STL_MAX_FILE_PATH_LENGTH,
                                  &sFileNameLen,
                                  KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    stlStrncpy( aFileName,
                sFileName,
                STL_MAX_FILE_PATH_LENGTH );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 주어진 File이 File System에 존재하는지 검사한다.
 * @param[in] aFileName 검사할 Logfile Name
 * @param[out] aExist 파일 존재 여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smlExistLogfile( stlChar  * aFileName,
                           stlBool  * aExist,
                           smlEnv   * aEnv )
{
    stlChar   sFileName[STL_MAX_FILE_PATH_LENGTH];
    stlChar   sLogDir[STL_MAX_FILE_PATH_LENGTH];
    stlInt32  sFileNameLen;
    
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( knlGetPropertyValueByName( "LOG_DIR",
                                        sLogDir,
                                        KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    STL_TRY( stlMergeAbsFilePath( sLogDir,
                                  aFileName,
                                  sFileName,
                                  STL_MAX_FILE_PATH_LENGTH,
                                  &sFileNameLen,
                                  KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    STL_TRY( stlExistFile( sFileName,
                           aExist,
                           KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 주어진 Logfile이 Database에 존재하는지 검사한다.
 * @param[in] aFileName 검사할 Logfile Name
 * @param[out] aExist 파일 존재 여부
 * @param[out] aGroupId  Logfile이 속한 Group Id
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smlIsUsedLogfile( stlChar   * aFileName,
                            stlBool   * aExist,
                            stlInt16  * aGroupId,
                            smlEnv    * aEnv )
{
    stlChar        sFileName[STL_MAX_FILE_PATH_LENGTH];
    stlChar        sLogDir[STL_MAX_FILE_PATH_LENGTH];
    stlInt32       sFileNameLen;
    stlInt32       sState = 0;
    
    *aExist = STL_FALSE;
    
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( knlGetPropertyValueByName( "LOG_DIR",
                                        sLogDir,
                                        KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    STL_TRY( stlMergeAbsFilePath( sLogDir,
                                  aFileName,
                                  sFileName,
                                  STL_MAX_FILE_PATH_LENGTH,
                                  &sFileNameLen,
                                  KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    STL_TRY( smfAcquireDatabaseFileLatch( aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( smrIsUsedLogFile( aFileName,
                               aExist,
                               aGroupId,
                               aEnv ) == STL_SUCCESS );
   
    sState = 0;
    STL_TRY( smfReleaseDatabaseFileLatch( aEnv ) == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smfReleaseDatabaseFileLatch( aEnv );
    }

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

stlStatus smlCheckLogGroupId( stlInt32   aLogGroupId,
                              stlBool  * aFound,
                              smlEnv   * aEnv )
{
    return smrCheckLogGroupId( aLogGroupId,
                               aFound,
                               aEnv );
}

stlInt64 smlGetMinLogFileSize( smlEnv   * aEnv )
{
    stlInt64 sLogBufferSize;
    stlInt64 sPendLogBufCnt;

    /**
     * @todo property 값이 refine 되어 gSmrWarmupEntry에서 관리되기 때문에 property 대신 WarmupEntry를 사용해야 한다.
     */ 
    sLogBufferSize = knlGetPropertyBigIntValueByID( KNL_PROPERTY_LOG_BUFFER_SIZE,
                                                    KNL_ENV( aEnv ) );
    sPendLogBufCnt = knlGetPropertyBigIntValueByID( KNL_PROPERTY_PENDING_LOG_BUFFER_COUNT,
                                                    KNL_ENV( aEnv ) );

    return( sLogBufferSize + (sPendLogBufCnt * SML_PENDING_LOG_BUFFER_SIZE) );
}

/**
 * @brief Tablespace에 대해 Media Recovery를 수행한다.
 * @param[in]     aTbsId        Recovery를 수행할 Tablespace의 Identifier
 * @param[in,out] aEnv          Environment 정보
 */
stlStatus smlMediaRecoverTablespace( smlTbsId   aTbsId,
                                     smlEnv   * aEnv )
{
    stlInt32          sState = 0;
    knlRegionMark     sMemMark;

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );

    STL_TRY( smgPrepareMediaRecoveryAtOpen( SML_RECOVERY_OBJECT_TYPE_TABLESPACE,
                                            aTbsId,
                                            &sMemMark,
                                            aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( smrMediaRecoverTablespace( aEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( smgFinalizeMediaRecoveryAtOpen( &sMemMark,
                                             aEnv ) == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState > 0 )
    {
        (void)smgFinalizeMediaRecoveryAtOpen( &sMemMark, aEnv );
    }

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );

    return STL_FAILURE;
}

/**
 * @brief Datafile에 대해 Media Recovery를 수행한다.
 * @param[in]     aDatafileRecoveryInfo Recovery를 수행할 Datafile
 * @param[in,out] aEnv                  Environment 정보
 */ 
stlStatus smlMediaRecoverDatafile( smlDatafileRecoveryInfo * aDatafileRecoveryInfo,
                                   smlEnv                  * aEnv )
{
    smrMediaRecoveryInfo    * sRecoveryInfo;
    smrRecoveryDatafileInfo * sDatafileInfo;
    stlInt32                  sState = 0;
    smrLsn                    sSystemLsn;
    knlRegionMark             sMemMark;

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );

    STL_TRY( smgPrepareMediaRecoveryAtOpen( SML_RECOVERY_OBJECT_TYPE_DATAFILE,
                                            SML_INVALID_TBS_ID,
                                            &sMemMark,
                                            aEnv ) == STL_SUCCESS );
    sState = 1;

    sRecoveryInfo = SMR_GET_MEDIA_RECOVERY_INFO( aEnv );

    /**
     * Parameter들 sml -> smr 이관 작업.
     */
    STL_TRY( smrCreateMediaRecoveryDatafileInfo( aDatafileRecoveryInfo,
                                                 sRecoveryInfo,
                                                 aEnv ) == STL_SUCCESS );

    /**
     * Offline Lsn setting
     */ 
    sSystemLsn = smrGetSystemLsn();

    STL_RING_FOREACH_ENTRY( &sRecoveryInfo->mDatafileList, sDatafileInfo )
    {
        if( smrIsExistSameTbs4DatafileRecovery( sRecoveryInfo, sDatafileInfo ) == STL_TRUE )
        {
            continue;
        }
        
        if( SMR_IS_VALID_LOG_LSN( smfGetOfflineLsn( sDatafileInfo->mTbsId ) ) == STL_FALSE )
        {
            smfSetOfflineLsn( sDatafileInfo->mTbsId,
                              sSystemLsn );
        }
    }
    
    STL_TRY( smrMediaRecoverDatafile( aEnv ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( smgFinalizeMediaRecoveryAtOpen( &sMemMark,
                                             aEnv ) == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    if( sState > 0 )
    {
        (void)smgFinalizeMediaRecoveryAtOpen( &sMemMark, aEnv );
    }

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

stlStatus smlRestoreTablespace( smlTbsId   aTbsId,
                                smlEnv   * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );

    STL_TRY( smfRestoreDatafiles( aTbsId, aEnv ) == STL_SUCCESS );

    STL_TRY( smfMergeBackup( aTbsId,
                             SML_RESTORE_UNTIL_TYPE_NONE,
                             -1,
                             aEnv ) == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );

    return STL_FAILURE;
}

/**
 * @brief Checkpoint의 진행 상태를 얻는다.
 * @return Checkpoint의 진행 상태
 * @see smlCheckpointerState
 */
inline stlInt16 smlGetCheckpointState()
{
    return smrGetCheckpointState();
}

/**
 * @brief Control File을 Backup Control File에서 Restore한다.
 * @param[in]     aBackupCtrlFile  Backup Control File Name
 * @param[in,out] aEnv             Environment
 */
stlStatus smlRestoreCtrlFile( stlChar * aBackupCtrlFile,
                              smlEnv  * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );

    STL_TRY( smfRestoreCtrlFile( aBackupCtrlFile, aEnv ) == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );

    return STL_FAILURE;
}

/**
 * @brief System LSN을 구한다.
 * @return System LSN
 */
stlInt64 smlGetSystemLsn()
{
    return smrGetSystemLsn();
}

/**
 * @brief System SBSN을 구한다.
 * @return System SBSN
 */
stlInt64 smlGetSystemSbsn()
{
    return smrGetRearSbsn();
}

/**
 * @brief Tablespace Id에 대해 Valid한 Incremental Backup이 있는지 확인.
 * @param[in] aTbsId  Tablespace Id
 * @param[out] aExist  Exist Flag
 * @param[in,out] aEnv  Environment
 */ 
stlStatus smlExistValidIncBackup( smlTbsId   aTbsId,
                                  stlBool  * aExist,
                                  smlEnv   * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );

    STL_TRY( smfExistValidIncBackup( aTbsId,
                                     aExist,
                                     aEnv ) == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief MOUNT phase에서 Recovery와 특정 Operation이 동시에 수행될 수 없도록 한다.
 * @param[in]     aRecoveryPhase  Media Recovery Phase
 * @param[in,out] aEnv           Environment
 */
stlStatus smlSetRecoveryPhaseAtMount( stlUInt8   aRecoveryPhase,
                                      smlEnv   * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );

    STL_TRY( smrSetRecoveryPhaseAtMount( aRecoveryPhase,
                                         aEnv )
             == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );

    return STL_FAILURE;
}

stlUInt64 smlGetRecoveryPhaseAtMount()
{
    return smrGetRecoveryPhase();
}

/**
 * @brief Database의 ARCHIVELOG mode를 변경하기 위해 media recovery가
 *        필요한 offline tablespace가 존재하는지 확인한다.
 * @param[in] aMode ARCHIVELOG mode( ARCHIVELOG or NOARCHIVELOG )
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smlValidateAlterArchivelog( stlInt32   aMode,
                                      smlEnv   * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    STL_DASSERT( smfGetDataAccessMode() == SML_DATA_ACCESS_MODE_READ_WRITE );

    STL_TRY_THROW( knlGetCurrStartupPhase() == KNL_STARTUP_PHASE_MOUNT,
                   RAMP_ERR_ONLY_MOUNT_PHASE );

    /**
     * CDS나 DS모드에서는 ARCHIVELOG를 지원할 수 없다.
     */
    if( smxlGetDataStoreMode() != SML_DATA_STORE_MODE_TDS )
    {
        STL_TRY_THROW( aMode == SML_NOARCHIVELOG_MODE,
                       RAMP_ERR_TDS_ONLY_ARCHIVELOG );
    }

    /**
     * INCONSISTENT 상태인 Offline Tablespace가 존재하는지 체크한다.
     * 만약 있으면 ARCHIVELOG -> NOARCHIVELOG로 변경할 수 없다.
     */
    if( aMode == SML_NOARCHIVELOG_MODE )
    {
        STL_TRY( smfValidateOfflineTbsConsistency( SML_ENV(aEnv) ) == STL_SUCCESS );
    }

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );

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

    return STL_FAILURE;
}

/**
 * @brief 주어진 Backup File의 절대경로를 포함한 File Name이 물리적으로 존재하는지 검사하고 File Name을 반환한다.
 * @param[in] aFileName 검사할 Backup File Name
 * @param[out] aAbsFileName 검사할 Backup File의 절대경로 + File Name
 * @param[out] aExist 파일 존재 여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smlGetAbsBackupFileName( stlChar  * aFileName,
                                   stlChar  * aAbsFileName,
                                   stlBool  * aExist,
                                   smlEnv   * aEnv )
{
    stlInt64  sBackupDirCount;
    stlInt32  i;

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );

    sBackupDirCount = knlGetPropertyBigIntValueByID(
        KNL_PROPERTY_READABLE_BACKUP_DIR_COUNT,
        KNL_ENV(aEnv) );

    for( i = 0; i < sBackupDirCount; i++ )
    {
        STL_TRY( smfGetAbsBackupFileName( aFileName,
                                          i,
                                          aAbsFileName,
                                          aEnv )
                 == STL_SUCCESS );

        STL_TRY( stlExistFile( aAbsFileName,
                               aExist,
                               KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

        if( *aExist == STL_TRUE )
        {
            break;
        }
    }

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief Media Recovery를 위한 Validation을 한다.
 * @param[in] aTbsId  tablespace id
 * @param[in] aRecoveryObject  recovery object type
 * @param[in,out] aEnv  environment
 */ 
stlStatus smlValidateMediaRecovery( smlTbsId   aTbsId,
                                    stlUInt8   aRecoveryObject,
                                    smlEnv   * aEnv )
{
    smrLsn sLastLsn;

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );

    STL_TRY( smrReadLastLsnFromLogfile( &sLastLsn,
                                        aEnv ) == STL_SUCCESS );


    STL_TRY( smfValidateMediaRecoveryDatafile( aTbsId,
                                               sLastLsn,
                                               aRecoveryObject,
                                               aEnv ) == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );

    return STL_FAILURE;
}

/** @} */

