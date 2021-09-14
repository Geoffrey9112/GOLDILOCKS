/*******************************************************************************
 * smrStartup.c
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
#include <smf.h>
#include <smg.h>
#include <smr.h>
#include <smp.h>
#include <sms.h>
#include <smxl.h>
#include <smrRecoveryMgr.h>
#include <smrLogBuffer.h>
#include <smrPendBuffer.h>
#include <smrLogGroup.h>
#include <smrLogMember.h>
#include <smrLogMirrorDef.h>
#include <smrLogMirror.h>
#include <smrArchiveLog.h>

/**
 * @file smrStartup.c
 * @brief Storage Manager Layer Recovery Startup Internal Routines
 */

smrWarmupEntry  * gSmrWarmupEntry;
stlInt64          gSmrLogBlockSize;
stlInt64          gSmrBulkIoBlockCount;

extern smrRedoVector  gSmrRedoVectors[];

extern knlFxTableDesc gSmrLogBufferTableDesc;
extern knlFxTableDesc gSmrPendingLogBufferTableDesc;
extern knlFxTableDesc gSmrLogStreamTableDesc;
extern knlFxTableDesc gSmrLogGroupTableDesc;
extern knlFxTableDesc gSmrLogMemberTableDesc;
extern knlFxTableDesc gSmrLogBlockTableDesc;
extern knlFxTableDesc gSmrLogTableDesc;
extern knlFxTableDesc gSmrArchivelogTableDesc;

extern knlLatchScanCallback gSmrLogStreamLatchScanCallback;

extern stlUInt16  gSmrLogMirrorShmAddrIdx;

/**
 * @addtogroup smr
 * @{
 */

/**
 * @brief Recovery Component를 NO MOUNT 모드로 전이한다.
 * @param[out] aWarmupEntry Recovery Component의 Warmup Entry 포인터 
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smrStartupNoMount( void  ** aWarmupEntry,
                             smlEnv * aEnv )
{
    smrWarmupEntry * sWarmupEntry;
    smrLogStream   * sLogStream;

    /**
     * structure validation
     */
    STL_DASSERT( (STL_SIZEOF(smrPendLogBuffer) % 64) == 0 );
    
    smrRegisterRedoVectors( SMG_COMPONENT_RECOVERY,
                            gSmrRedoVectors );
    
    STL_TRY( knlCacheAlignedAllocFixedStaticArea( STL_SIZEOF( smrWarmupEntry ),
                                                  (void**)&sWarmupEntry,
                                                  (knlEnv*)aEnv ) == STL_SUCCESS );

    sLogStream = &(sWarmupEntry->mLogStream);

    SMR_INIT_LOG_STREAM( sLogStream );
    SMR_INIT_LOG_STREAM_PERSISTENT_DATA( &(sLogStream->mLogStreamPersData) );

    sWarmupEntry->mLsn                 = 0;
    sWarmupEntry->mLogFlusherState     = SML_LOG_FLUSHER_STATE_INACTIVE;
    sWarmupEntry->mCheckpointerState   = SML_CHECKPOINTER_STATE_INACTIVE;
    sWarmupEntry->mEnableLogging       = STL_TRUE;
    sWarmupEntry->mBlockedLfsn         = -1;
    sWarmupEntry->mEnableFlushing      = STL_FALSE;
    sWarmupEntry->mBlockLogFlushing    = 0;

    stlInitSpinLock( &(sWarmupEntry->mSpinLock) );

    sWarmupEntry->mRecoveryInfo.mLogLsn        = SMR_INVALID_LSN;
    sWarmupEntry->mRecoveryInfo.mLogTransId    = 0;
    SML_SET_INVALID_RID( &(sWarmupEntry->mRecoveryInfo.mLogSegRid) );
    sWarmupEntry->mRecoveryInfo.mLogPieceCount = 0;
    sWarmupEntry->mRecoveryInfo.mRecoveryPhase = SMR_RECOVERY_PHASE_NONE;
    sWarmupEntry->mRecoveryInfo.mLogMirrorStat = 0;
    sWarmupEntry->mRecoveryInfo.mMediaRecoveryInfo = NULL;

    /**
     * Checkpoint information
     * mChkptState : checkpoint 수행 상태
     */
    sWarmupEntry->mChkptInfo.mLastChkptLsn   = 0;
    sWarmupEntry->mChkptInfo.mChkptState     = SML_CHECKPOINT_STATE_IN_IDLE;

    SMR_INIT_LOG_PERSISTENT_DATA( &(sWarmupEntry->mLogPersData ) );

    sWarmupEntry->mSyncValue[0] = 0;
    sWarmupEntry->mSyncValue[1] = 0;

    gSmrWarmupEntry = sWarmupEntry;
    *aWarmupEntry = (void*)sWarmupEntry;

    STL_TRY( smrRegisterFxTables( aEnv ) == STL_SUCCESS );

    STL_TRY( knlRegisterLatchScanCallback( &gSmrLogStreamLatchScanCallback,
                                           KNL_ENV( aEnv ) ) == STL_SUCCESS );

    smgRegisterPendOp( SMG_PEND_ADD_LOG_GROUP, smrDoPendAddLogGroup );
    smgRegisterPendOp( SMG_PEND_ADD_LOG_MEMBER, smrDoPendAddLogMember );
    smgRegisterPendOp( SMG_PEND_DROP_LOG_GROUP, smrDoPendDropLogGroup );
    smgRegisterPendOp( SMG_PEND_DROP_LOG_MEMBER, smrDoPendDropLogMember );
    smgRegisterPendOp( SMG_PEND_RENAME_LOG_FILE, smrDoPendRenameLogFile );
    smgRegisterPendOp( SMG_PEND_ADD_LOG_FILE, smrDoPendAddLogFile );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Recovery Component를 MOUNT 모드로 전이한다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smrStartupMount( smlEnv * aEnv )
{
    stlUInt32      i;
    smrLogStream * sLogStream;
    stlChar      * sLogBuffer;
    stlInt64       sLogBufferSize;
    stlInt64       sPendLogBufferCount;
    stlInt64       sMinLogFileSize;
    stlInt16       sBlockSize;

    sLogStream = &gSmrWarmupEntry->mLogStream;

    SMR_INIT_LOG_STREAM( sLogStream );
    SMR_INIT_LOG_STREAM_PERSISTENT_DATA( &(sLogStream->mLogStreamPersData) );
    SMR_INIT_LOG_PERSISTENT_DATA( &(gSmrWarmupEntry->mLogPersData ) );

    STL_TRY( smrLoadLogCtrlSection( &sMinLogFileSize, aEnv ) == STL_SUCCESS );
    
    gSmrWarmupEntry->mChkptInfo.mLastChkptLsn = gSmrWarmupEntry->mLogPersData.mChkptLsn;
    gSmrWarmupEntry->mSyncValue[0] = 0;
    gSmrWarmupEntry->mSyncValue[1] = 0;
    gSmrWarmupEntry->mRecoveryInfo.mMediaRecoveryInfo = NULL;

    sBlockSize = sLogStream->mLogStreamPersData.mBlockSize;
    STL_TRY( smrAdjustGlobalVariables( NULL, aEnv ) == STL_SUCCESS );

    /**
     * 초기화가 안된 상태라면 SKIP 한다.
     */
    STL_TRY_THROW( SMR_IS_ONLINE_STREAM( sLogStream ) == STL_TRUE,
                   RAMP_SUCCESS );

    /**
     * Log Buffer 할당 및 초기화
     */
    sLogBufferSize = knlGetPropertyBigIntValueByID( KNL_PROPERTY_LOG_BUFFER_SIZE,
                                                    KNL_ENV( aEnv ) );
    sPendLogBufferCount =
        knlGetPropertyBigIntValueByID( KNL_PROPERTY_PENDING_LOG_BUFFER_COUNT,
                                       KNL_ENV( aEnv ) );

    /**
     * 최소 로그파일 크기가 로그 버퍼와 펜딩로그버퍼들의 합보다 작으면
     * Checkpoint가 실패할 수 있으므로 로그버퍼의 크기를 조정한다
     */
    if( sLogBufferSize + (sPendLogBufferCount * SML_PENDING_LOG_BUFFER_SIZE) > sMinLogFileSize )
    {
        sLogBufferSize = sMinLogFileSize - (sPendLogBufferCount * SML_PENDING_LOG_BUFFER_SIZE);
        knlLogMsg( NULL,
                   KNL_ENV(aEnv),
                   KNL_LOG_LEVEL_WARNING,
                   "Too large Online Redo Log Buffer and pending log buffer size.\n"
                   "Online Redo Log Buffer is resized to %ld bytes.\n",
                   sLogBufferSize );
    }

    STL_TRY( knlCacheAlignedAllocFixedStaticArea( STL_SIZEOF(smrLogBuffer),
                                                  (void**)&sLogStream->mLogBuffer,
                                                  KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    /**
     * ALIGN 과 BUFFER OVERFLOW 검증을 고려해 BLOCK SIZE * 2 만큼의 추가 메모리를 할당한다
     */
    STL_TRY( knlAllocFixedStaticArea( sLogBufferSize + (sBlockSize*2),
                                      (void**)&sLogBuffer,
                                      (knlEnv*)aEnv )
             == STL_SUCCESS );

    SMR_INIT_LOG_BUFFER( sLogStream->mLogBuffer,
                         sLogBufferSize,
                         sLogBuffer,
                         STL_TRUE, /* aIsShared */
                         sBlockSize );
        
    STL_TRY( smrFormatLogBuffer( sLogStream->mLogBuffer->mBuffer,
                                 sLogStream->mLogBuffer->mBufferSize,
                                 sBlockSize,
                                 aEnv ) == STL_SUCCESS );
        
    /**
     * Pending Log Buffer 할당 및 초기화
     */
        
    sLogStream->mPendBufferCount = sPendLogBufferCount;

    STL_TRY( knlCacheAlignedAllocFixedStaticArea( STL_SIZEOF(smrPendLogBuffer) * sPendLogBufferCount,
                                                  (void**)&sLogStream->mPendLogBuffer,
                                                  (knlEnv*)aEnv )
             == STL_SUCCESS );
        
    for( i = 0; i < sPendLogBufferCount; i++ )
    {
        STL_TRY( smrInitPendLogBuffer( &sLogStream->mPendLogBuffer[i],
                                       aEnv )
                 == STL_SUCCESS );
        
        STL_TRY( smrFormatPendLogBuffer( &sLogStream->mPendLogBuffer[i],
                                         aEnv )
                 == STL_SUCCESS );
    }
        
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief OPEN Mode로 전환시 Recovery Component의 유효성을 검사한다.
 * @param[in]  aLogOption    NORESETLOGS/RESETLOGS
 * @param[in,out] aEnv       Environment 정보
 */
stlStatus smrValidateOpen( stlChar   aLogOption,
                           smlEnv  * aEnv )
{
    stlBool               sFileExist;
    stlBool               sIsValid;
    stlFileInfo           sFileInfo;
    void *                sLogGroupIterator;
    smrLogGroupPersData   sLogGroupPersData;
    void *                sLogMemberIterator;
    smrLogMemberPersData  sLogMemberPersData;
    stlInt32              sValidationFlag;

    sValidationFlag = SMR_VALIDATE_LOGFILE_CREATION_TIME | SMR_VALIDATE_LOGFILE_GROUPID;

    /* STARTUP OPEN RESETLOGS 의 경우 Controlfile에 저장된
     * LogGroup의 FileSequence보다 실제 Logfile의 FileSequence가
     * 더 크기 때문에 validation할 수 없다. */
    if( aLogOption == SML_STARTUP_LOG_OPTION_NORESETLOGS )
    {
        sValidationFlag |= SMR_VALIDATE_LOGFILE_SEQUENCE;
    }

    /**
     * Validate redo Log file
     */

    STL_TRY( smrFirstLogGroupPersData( &gSmrWarmupEntry->mLogStream,
                                       &sLogGroupPersData,
                                       &sLogGroupIterator,
                                       aEnv )
             == STL_SUCCESS );

    while( sLogGroupIterator != NULL )
    {
        STL_TRY( smrFirstLogMemberPersData( sLogGroupIterator,
                                            &sLogMemberPersData,
                                            &sLogMemberIterator,
                                            aEnv )
                 == STL_SUCCESS );
        
        while( sLogMemberIterator != NULL )
        {
            STL_TRY( stlExistFile( sLogMemberPersData.mName,
                                   &sFileExist,
                                   KNL_ERROR_STACK(aEnv) )
                     == STL_SUCCESS );

            /**
             * logfile 존재 여부 검사
             */
            STL_TRY_THROW( sFileExist == STL_TRUE, RAMP_ERR_FILE_NOT_EXIST );
                    
            STL_TRY( stlGetFileStatByName( &sFileInfo,
                                           sLogMemberPersData.mName,
                                           STL_FINFO_UPROT,
                                           KNL_ERROR_STACK(aEnv) )
                     == STL_SUCCESS );

            /**
             * logfile permission 검사
             */
            STL_TRY_THROW( (sFileInfo.mProtection & (STL_FPERM_UREAD | STL_FPERM_UWRITE)) ==
                           (STL_FPERM_UREAD | STL_FPERM_UWRITE),
                           RAMP_ERR_FILE_IS_READ_ONLY );
    
            /**
             * logfile header 검사
             */
            STL_TRY( smrValidateLogfileByName( sLogMemberPersData.mName,
                                               &sLogGroupPersData,
                                               sValidationFlag,
                                               &sIsValid,
                                               &sFileExist,
                                               aEnv )
                     == STL_SUCCESS );

            STL_TRY_THROW( sIsValid == STL_TRUE, RAMP_ERR_INVALID_MEMBER );
            
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
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_FILE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_FILE_NOT_EXIST,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sLogMemberPersData.mName );
    }
    
    STL_CATCH( RAMP_ERR_FILE_IS_READ_ONLY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_FILE_IS_READ_ONLY,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sLogMemberPersData.mName );
    }
    
    STL_CATCH( RAMP_ERR_INVALID_MEMBER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_LOGFILE_NOT_VALID,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sLogMemberPersData.mName );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Recovery Component를 PREOPEN 모드로 전이한다.
 * @param[in] aStartupInfo Startup 시 정보
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smrStartupPreOpen( knlStartupInfo * aStartupInfo,
                             smlEnv         * aEnv )
{
    STL_TRY_THROW( smfGetServerState() != SML_SERVER_STATE_NONE, RAMP_SUCCESS );

    STL_TRY( smrRecover( aStartupInfo->mLogOption, aEnv ) == STL_SUCCESS );

    smfSetServerState( SML_SERVER_STATE_SERVICE );

    STL_TRY( smfSaveCtrlFile( aEnv ) == STL_SUCCESS );
    
    /**
     * LogMirror 영역을 생성한다.
     */
    STL_TRY( smrCreateLogMirrorArea( SML_ENV(aEnv) ) == STL_SUCCESS );

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Recovery Component를 OPEN 모드로 전이한다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smrStartupOpen( smlEnv  * aEnv )
{
    STL_TRY( smrAdjustGlobalVariables( NULL, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Media Recovery를 수행한다.
 * @param[out]  aIsFinishedRecovery  Media Recovery가 완료되었는지 여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smrMediaRecover( stlBool  * aIsFinishedRecovery,
                           smlEnv   * aEnv )
{
    stlUInt8    sMediaRecoveryType;
    stlUInt8    sRecoveryObjectType;

    *aIsFinishedRecovery = STL_TRUE;

    STL_TRY_THROW( smfGetServerState() != SML_SERVER_STATE_NONE, RAMP_SUCCESS );

    sMediaRecoveryType = smrGetRecoveryType( aEnv );
    sRecoveryObjectType = smrGetRecoveryObjectType( aEnv );

    switch( sRecoveryObjectType )
    {
        case SML_RECOVERY_OBJECT_TYPE_DATABASE:
            switch( sMediaRecoveryType )
            {
                case SML_RECOVERY_TYPE_INCOMPLETE_MEDIA_RECOVERY:
                    /* LOGFILE 기반의 불완전 복구의 경우 END 구문을 수행했을 때 완료된다. */
                    if( smrGetImrOption(aEnv) == SML_IMR_OPTION_INTERACTIVE )
                    {
                        *aIsFinishedRecovery = STL_FALSE;
                    }
                case SML_RECOVERY_TYPE_COMPLETE_MEDIA_RECOVERY:
                    STL_TRY( smrMediaRecoverDatabase( aIsFinishedRecovery,
                                                      aEnv ) == STL_SUCCESS );
                    break;
                default:
                    STL_DASSERT( STL_FALSE );
                    break;
            }
            break;
        case SML_RECOVERY_OBJECT_TYPE_TABLESPACE:
            STL_TRY( smrMediaRecoverTablespace( aEnv ) == STL_SUCCESS );
            break;
        case SML_RECOVERY_OBJECT_TYPE_DATAFILE:
            STL_TRY( smrMediaRecoverDatafile( aEnv ) == STL_SUCCESS );
            break;
        default:
            STL_DASSERT( STL_FALSE );
            break;
    }

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Recovery Component를 Warmup 시킨다.
 * @param[in] aWarmupEntry Recovery Component의 Warmup Entry 포인터 
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smrWarmup( void   * aWarmupEntry,
                     smlEnv * aEnv )
{
    smrRegisterRedoVectors( SMG_COMPONENT_RECOVERY,
                            gSmrRedoVectors );
    
    gSmrWarmupEntry = (smrWarmupEntry*)aWarmupEntry;

    STL_TRY( smrAdjustGlobalVariables( NULL, aEnv ) == STL_SUCCESS );

    STL_TRY( smrRegisterFxTables( aEnv ) == STL_SUCCESS );

    STL_TRY( knlRegisterLatchScanCallback( &gSmrLogStreamLatchScanCallback,
                                           KNL_ENV( aEnv ) ) == STL_SUCCESS );

    smgRegisterPendOp( SMG_PEND_ADD_LOG_GROUP, smrDoPendAddLogGroup );
    smgRegisterPendOp( SMG_PEND_ADD_LOG_MEMBER, smrDoPendAddLogMember );
    smgRegisterPendOp( SMG_PEND_DROP_LOG_GROUP, smrDoPendDropLogGroup );
    smgRegisterPendOp( SMG_PEND_DROP_LOG_MEMBER, smrDoPendDropLogMember );
    smgRegisterPendOp( SMG_PEND_RENAME_LOG_FILE, smrDoPendRenameLogFile );
    smgRegisterPendOp( SMG_PEND_ADD_LOG_FILE, smrDoPendAddLogFile );

    /**
     * LogMirror용 Segment Idx를 구한다.
     */
    STL_TRY( knlGetSegmentIdxByName( SMR_LOGMIRROR_DEFAULT_SHM_NAME,
                                     &gSmrLogMirrorShmAddrIdx,
                                     KNL_ENV( aEnv ) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Recovery Component를 Cooldown 시킨다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smrCooldown( smlEnv * aEnv )
{
    gSmrLogMirrorShmAddrIdx = SMR_LOGMIRROR_INVALID_IDX;
    
    return STL_SUCCESS;
}

/**
 * @brief Recovery Component를 Bootdown 시킨다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smrBootdown( smlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief Recovery Component를 Post Close 시킨다.
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smrShutdownPostClose( smlEnv * aEnv )
{
    smrDeactivateLogFlusher();
    smrSetRecoveryPhase( SMR_RECOVERY_PHASE_NONE );
    
    /**
     * LogMirror 영역을 없앤다.
     */
    STL_TRY( smrDestroyLogMirrorArea( SML_ENV(aEnv) ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief Recovery Component를 Close 시킨다.
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smrShutdownClose( smlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief Recovery Component를 Dismount 시킨다.
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smrShutdownDismount( smlEnv * aEnv )
{
    smrLogStream   * sLogStream;
    
    STL_TRY( smfUnloadCtrlFile( aEnv ) == STL_SUCCESS );

    sLogStream = &(gSmrWarmupEntry->mLogStream);

    SMR_INIT_LOG_STREAM( sLogStream );
    SMR_INIT_LOG_STREAM_PERSISTENT_DATA( &(sLogStream->mLogStreamPersData) );
    SMR_INIT_LOG_PERSISTENT_DATA( &(gSmrWarmupEntry->mLogPersData ) );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Fixed Table 들을 등록함
 * @param[in]  aEnv   Environment
 * @remarks
 * NO_MOUNT 단계와 Warm-Up 단계에서 동일한 순서로 등록되어야 함.
 */
stlStatus smrRegisterFxTables( smlEnv * aEnv )
{
    STL_TRY( knlRegisterFxTable( &gSmrLogBufferTableDesc,
                                 KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlRegisterFxTable( &gSmrPendingLogBufferTableDesc,
                                 KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlRegisterFxTable( &gSmrLogStreamTableDesc,
                                 KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlRegisterFxTable( &gSmrLogGroupTableDesc,
                                 KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlRegisterFxTable( &gSmrLogMemberTableDesc,
                                 KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlRegisterFxTable( &gSmrLogBlockTableDesc,
                                 KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlRegisterFxTable( &gSmrLogTableDesc,
                                 KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlRegisterFxTable( &gSmrArchivelogTableDesc,
                                 KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

void smrSetDatabaseMediaRecoveryInfo( void  * aMediaRecoveryInfo )
{
    gSmrWarmupEntry->mRecoveryInfo.mMediaRecoveryInfo = aMediaRecoveryInfo;
}

void * smrGetDatabaseMediaRecoveryInfo()
{
    return gSmrWarmupEntry->mRecoveryInfo.mMediaRecoveryInfo;
}

stlBool smrIsSuitableLogBlockSize( stlInt64   aBlockSize )
{
    stlBool   sIsSuitable = STL_FALSE;

    /* LOG_BLOCK_SIZE는 512, 1024, 2048, 4096 만 지원한다. */
    switch( aBlockSize )
    {
        case  512:
        case 1024:
        case 2048:
        case 4096:
            sIsSuitable = STL_TRUE;
            break;
        default:
            break;
    }

    return sIsSuitable;
}

stlStatus smrAdjustGlobalVariables( stlInt64  * aBlockSize,
                                    smlEnv    * aEnv )
{
    if( aBlockSize != NULL )
    {
        gSmrLogBlockSize = *aBlockSize;
    }
    else
    {    
        gSmrLogBlockSize = gSmrWarmupEntry->mLogStream.mLogStreamPersData.mBlockSize;
    }

    /* Database creation을 위해 startup mount시 아직 log block size가
     * 설정되지 않은 상태(0)이기 때문에 min log block size를 default값으로 설정한다. */
    if( gSmrLogBlockSize == 0 )
    {
        gSmrLogBlockSize = SMR_MIN_LOG_BLOCK_SIZE;
    }

    STL_DASSERT( gSmrLogBlockSize > 0 );

    gSmrBulkIoBlockCount = 8 * (SMR_MAX_LOG_BLOCK_SIZE / SMR_BLOCK_SIZE) * 1024;

    return STL_SUCCESS;
}

/**
 * @brief SM Layer에서 사용되는 database file을 register한다.
 * @param[out] aEnv Environment 구조체
 */
stlStatus smrRegisterDbFiles( smlEnv * aEnv )
{
    smrLogGroupPersData   sLogGroupPersData;
    smrLogMemberPersData  sLogMemberPersData;
    void *                sLogGroupIterator;
    void *                sLogMemberIterator;

    STL_TRY( smrFirstLogGroupPersData( &gSmrWarmupEntry->mLogStream,
                                       &sLogGroupPersData,
                                       &sLogGroupIterator,
                                       aEnv )
             == STL_SUCCESS );

    while( sLogGroupIterator != NULL )
    {
        STL_TRY( smrFirstLogMemberPersData( sLogGroupIterator,
                                            &sLogMemberPersData,
                                            &sLogMemberIterator,
                                            aEnv )
                 == STL_SUCCESS );
        
        while( sLogMemberIterator != NULL )
        {
            STL_TRY( knlAddDbFile( sLogMemberPersData.mName,
                                   KNL_DATABASE_FILE_TYPE_REDO_LOG,
                                   KNL_ENV(aEnv) )
                     == STL_SUCCESS );

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
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
