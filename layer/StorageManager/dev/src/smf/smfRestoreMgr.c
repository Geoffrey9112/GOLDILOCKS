/*******************************************************************************
 * smfRestoreMgr.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smfRestoreMgr.c 9304 2013-08-07 07:39:53Z kyungoh $
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
#include <smg.h>
#include <sma.h>
#include <smfDef.h>
#include <smf.h>
#include <smp.h>
#include <smr.h>
#include <smd.h>
#include <smxl.h>
#include <smxm.h>
#include <smlGeneral.h>
#include <smfTbsMgr.h>
#include <smfRestoreMgr.h>
#include <smfRecovery.h>

/**
 * @file smfRestoreMgr.c
 * @brief Storage Manager Layer Tablespace Manager Internal Routines
 */

extern smfDatafileFuncs * gDatafileFuncs[];

/**
 * @addtogroup smf
 * @{
 */

/**
 * @brief Datafile을 validation하고 존재하지 않는 datafile은 생성한다.
 * @param[in] aTbsId Backup을 Restore하기 위한 Tablespace Id
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfRestoreDatafiles( smlTbsId   aTbsId,
                               smlEnv   * aEnv )
{
    smfTbsPersData        sTbsPersData;
    smfDatafilePersData   sDatafilePersData;
    void                * sTbsIterator;
    void                * sDatafileIterator;
    stlBool               sFileExist;
    stlBool               sExistBackup;
    stlInt32              sState = 0;
    stlUInt32             sTbsTypeId;
    stlFile               sFile;
    smfDatafileHeader     sDatafileHeader;
    stlInt32              sFlag;
    stlInt64              sDatabaseIo;
    stlChar               sTbsName[STL_MAX_SQL_IDENTIFIER_LENGTH];

    sFlag = STL_FOPEN_LARGEFILE | STL_FOPEN_WRITE | STL_FOPEN_READ;

    sDatabaseIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_FILE_IO,
                                                 KNL_ENV(aEnv) );

    if( sDatabaseIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

    STL_TRY( smfFirstTbsPersData( &sTbsPersData,
                                  &sTbsIterator,
                                  aEnv ) == STL_SUCCESS );

    while( 1 )
    {
        if( sTbsIterator == NULL )
        {
            break;
        }

        /**
         * 특정 Tablespace에 대한 restore의 경우 다른 tablespace는 skip한다.
         */
        if( (aTbsId != SML_INVALID_TBS_ID) && (aTbsId != sTbsPersData.mTbsId) )
        {
            STL_TRY( smfNextTbsPersData( &sTbsPersData,
                                         &sTbsIterator,
                                         aEnv ) == STL_SUCCESS );
            continue;
        }

        /**
         * Temp이거나 aging 중인 tablespace는 validate할 필요 없다.
         */
        if( (SML_TBS_IS_MEMORY_TEMPORARY( sTbsPersData.mAttr ) == STL_TRUE) ||
            (sTbsPersData.mState == SMF_TBS_STATE_AGING) )
        {
            STL_TRY( smfNextTbsPersData( &sTbsPersData,
                                         &sTbsIterator,
                                         aEnv ) == STL_SUCCESS );
            continue;
        }

        /**
         * Database Media Recovery시 Offline Tablespace는 Restore대상이 아님
         * Tablespace Media Recovery시 Restore 대상
         */
        if( (SMF_IS_ONLINE_TBS( sTbsPersData.mTbsId ) == STL_FALSE) &&
            (aTbsId == SML_INVALID_TBS_ID) )
        {
            STL_TRY( smfNextTbsPersData( &sTbsPersData,
                                         &sTbsIterator,
                                         aEnv ) == STL_SUCCESS );
            continue;
        }

        STL_TRY( smfFirstDatafilePersData( sTbsIterator,
                                           &sDatafilePersData,
                                           &sDatafileIterator,
                                           aEnv ) == STL_SUCCESS );

        while( 1 )
        {
            if( sDatafileIterator == NULL )
            {
                break;
            }

            /**
             * Aging 중인 datafile은 제외
             */
            if( sDatafilePersData.mState == SMF_DATAFILE_STATE_AGING )
            {
                STL_TRY( smfNextDatafilePersData( sTbsIterator,
                                                  &sDatafilePersData,
                                                  &sDatafileIterator,
                                                  aEnv ) == STL_SUCCESS );
                continue;
            }
            
            /**
             * Datafile이 존재하는지 검사한다.
             */
            STL_TRY( stlExistFile( sDatafilePersData.mName,
                                   &sFileExist,
                                   KNL_ERROR_STACK(aEnv) )
                     == STL_SUCCESS );

            if( sFileExist != STL_TRUE )
            {
                /* Restore를 위한 Backup이 없으면 종료. */
                STL_TRY( smfExistValidIncBackup( sTbsPersData.mTbsId,
                                                 &sExistBackup,
                                                 aEnv )
                         == STL_SUCCESS );

                STL_TRY_THROW( sExistBackup == STL_TRUE, RAMP_ERR_NOT_EXIST_VALID_BACKUP );

                /**
                 * Datafile이 존재하지 않으면 생성한다.
                 */
                sTbsTypeId = SML_TBS_DEVICE_TYPE_ID( sTbsPersData.mAttr );

                /**
                 * 새로 생성된 datafile을 recovery하기 위해
                 * controlfile의 checkpoint Lsn과 Lid를 초기화하고
                 * datafile header에 반영되도록 한다.
                 */
                sDatafilePersData.mCheckpointLsn = SMR_INVALID_LSN;
                sDatafilePersData.mCheckpointLid = SMR_INVALID_LID;

                STL_TRY( knlLogMsg( NULL,
                                    KNL_ENV( aEnv ),
                                    KNL_LOG_LEVEL_INFO,
                                    "    [RESTORE] recreate datafile ( %s )\n",
                                    sDatafilePersData.mName )
                         == STL_SUCCESS );

                STL_TRY( gDatafileFuncs[sTbsTypeId]->mCreateRedo( SML_INVALID_TRANSID,
                                                                  sTbsPersData.mTbsId,
                                                                  sTbsPersData.mTbsId, /* aHintTbsId */
                                                                  &sDatafilePersData,
                                                                  STL_TRUE,
                                                                  STL_FALSE,  /* aClrLogging */
                                                                  aEnv )
                         == STL_SUCCESS );

                STL_TRY( knlLogMsg( NULL,
                                    KNL_ENV( aEnv ),
                                    KNL_LOG_LEVEL_INFO,
                                    "    [RESTORE] recreate end\n" )
                         == STL_SUCCESS );
            }
            else
            {
                /**
                 * Datafile header의 유효성 검사
                 */
                STL_TRY( smgOpenFile( &sFile,
                                      sDatafilePersData.mName,
                                      sFlag,
                                      STL_FPERM_OS_DEFAULT,
                                      aEnv )
                         == STL_SUCCESS );
                sState = 1;
        
                STL_TRY( smfReadDatafileHeader( &sFile,
                                                sDatafilePersData.mName,
                                                &sDatafileHeader,
                                                SML_ENV(aEnv) )
                         == STL_SUCCESS );
    
                sState = 0;
                STL_TRY( smgCloseFile( &sFile,
                                       aEnv )
                         == STL_SUCCESS );

                /**
                 * Datafile이 존재하지만 controlfile의 정보와 다르면
                 * exception 처리한다.
                 */
                STL_TRY_THROW( (sDatafileHeader.mTbsPhysicalId == SMF_INVALID_PHYSICAL_TBS_ID) ||
                               (sDatafileHeader.mTbsPhysicalId == sTbsPersData.mTbsId),
                               RAMP_ERR_INVALID_DATAFILE );
                STL_TRY_THROW( (sDatafileHeader.mDatafileId == sDatafilePersData.mID) &&
                               (sDatafileHeader.mCreationTime == sDatafilePersData.mTimestamp),
                               RAMP_ERR_INVALID_DATAFILE );
            }

            /* MOUNT phase에서 restore datafile은 gmaster의 event handler로 수행되므로
             * 주어진 event가 cancel되었는지 체크한다. */
            if( knlGetCurrStartupPhase() < KNL_STARTUP_PHASE_OPEN )
            {
                STL_TRY( knlCheckEnvEvent( KNL_ENV( aEnv ) ) == STL_SUCCESS );
            }

            STL_TRY( smfNextDatafilePersData( sTbsIterator,
                                              &sDatafilePersData,
                                              &sDatafileIterator,
                                              aEnv ) == STL_SUCCESS );
        }
        
        STL_TRY( smfNextTbsPersData( &sTbsPersData,
                                     &sTbsIterator,
                                     aEnv ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_EXIST_VALID_BACKUP )
    {
        smfGetTbsName( sTbsPersData.mTbsId, sTbsName, aEnv );

        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_NOT_EXIST_VALID_INCREMENTAL_BACKUP,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sTbsName );
    }

    STL_CATCH( RAMP_ERR_INVALID_DATAFILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INVALID_DATAFILE,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sDatafilePersData.mName );
    }

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smgCloseFile( &sFile, aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief Datafile Recovery를 위해 Backup에서 Restore 한다.
 * @param [in]     aRecoveryInfo   Datafile, Page Information
 * @param [in,out] aEnv            envirionment
 */
stlStatus smfRestoreDatafile4MediaRecovery( smrMediaRecoveryInfo * aRecoveryInfo,
                                            smlEnv               * aEnv )
{
    smrRecoveryDatafileInfo  * sDatafileInfo;
    smrLsn                     sCurLsn;
    smrLid                     sCurLid;
    smrLsn                     sRestoredLsn = SMR_INVALID_LSN;
    smrLid                     sRestoredLid = SMR_INVALID_LID;
    stlBool                    sDoRestoreByIncBackup = STL_FALSE;
    stlBool                    sExistBackup = STL_FALSE;
    stlChar                    sTbsName[STL_MAX_SQL_IDENTIFIER_LENGTH];

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "[DATAFILE RECOVERY] restore start\n" )
             == STL_SUCCESS );
    
    /**
     * Full Backup 이용하여 Restore.
     */
    STL_RING_FOREACH_ENTRY( &aRecoveryInfo->mDatafileList, sDatafileInfo )
    {
        if( sDatafileInfo->mUsingBackup == STL_TRUE )
        {
            STL_TRY( smfRestoreDatafileByFullBackup( sDatafileInfo,
                                                     &sCurLsn,
                                                     &sCurLid,
                                                     aEnv )
                     == STL_SUCCESS );

            /**
             * lsn 업데이트.
             */
            if( (sRestoredLsn > sCurLsn) || (sRestoredLsn == SMR_INVALID_LSN) )
            {
                sRestoredLsn = sCurLsn;
                sRestoredLid = sCurLid;
            }  
        }
        else
        {
            sDoRestoreByIncBackup = STL_TRUE;
        }
    }

    if( sDoRestoreByIncBackup == STL_TRUE )
    {
        /**
         * QP Layer에서 Validation 체크하지만, Control File이 변경될 수 있기 때문에 다시 Validation
         */ 
        STL_RING_FOREACH_ENTRY( &aRecoveryInfo->mDatafileList, sDatafileInfo )
        {
            if( sDatafileInfo->mUsingBackup == STL_FALSE )
            {
                STL_TRY( smfExistValidIncBackup( sDatafileInfo->mTbsId,
                                                 &sExistBackup,
                                                 aEnv )
                         == STL_SUCCESS );

                STL_TRY_THROW( sExistBackup == STL_TRUE, RAMP_ERR_NOT_EXIST_VALID_BACKUP );
            }
        }
        
        STL_TRY( smfRestoreDatafileByIncBackup( aRecoveryInfo,
                                                &sCurLsn,
                                                &sCurLid,
                                                aEnv )
                 == STL_SUCCESS );

        if( (sRestoredLsn > sCurLsn) ||
            (sRestoredLsn == SMR_INVALID_LSN) )
        {
            sRestoredLsn = sCurLsn;
            sRestoredLid = sCurLid;
        }
    }

    SMR_MEDIA_RECOVERY_SET_FIELD( aEnv, RestoredChkptLsn, sRestoredLsn );
    SMR_MEDIA_RECOVERY_SET_FIELD( aEnv, RestoredChkptLid, sRestoredLid );
    
    /**
     * Incremental Backup 이용하여 Restore
     */
    
    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "[DATAFILE RECOVERY] restore end\n" )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_EXIST_VALID_BACKUP )
    {
        smfGetTbsName( sDatafileInfo->mTbsId, sTbsName, aEnv );

        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_NOT_EXIST_VALID_INCREMENTAL_BACKUP,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sTbsName );
    }
    
    STL_FINISH;

    (void) knlLogMsg( NULL,
                      KNL_ENV( aEnv ),
                      KNL_LOG_LEVEL_INFO,
                      "[DATAFILE RECOVERY] restore failed\n" );

    return STL_FAILURE;
}

/**
 * @brief Incremental backup을 이용하여 datafile을 merge한다.
 * @param[in] aTbsId Backup을 Restore하기 위한 Tablespace Id
 * @param[in] aUntilType Restore를 종료할 Until Type
 * @param[in] aUntilValue Restore를 종료할 Until Value
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfMergeBackup( smlTbsId   aTbsId,
                          stlUInt8   aUntilType,
                          stlInt64   aUntilValue,
                          smlEnv   * aEnv )
{
    stlChar             * sBuffer;
    stlChar             * sPageFrames;
    stlInt64              sPageCount;
    stlFile               sFile;
    stlInt64              sBackupIdx;
    stlInt64              sBufferSize;
    stlBool               sExistBackup;
    stlBool               sIsApplicable;
    stlBool               sIsSuccess;
    smfBackupIterator     sBackupIterator;
    stlInt32              sState = 0;
    smrLsn                sBackupChkptLsn;
    smrLid                sBackupChkptLid;
    stlInt64              sApplyBackupPlanSize;
    smfApplyBackupPlan  * sApplyBackupPlan;
    smpFlushArg           sArg;
    smpFlushTbsArg        sTbsArg;
    knlRegionMark         sMemMark;
    smfBackupSegment    * sBackupSegment;
    smfBackupChunk      * sBackupChunk = NULL;
    smfSysPersData      * sSysPersData;
    stlInt64              sTotalBackupCount;

    /**
     * 모든 datafile에 backup page를 apply한다.
     * 이를 위해 datafile header를 읽어서 datafile info의 last restored lsn을 초기화한다.
     */

    if( aTbsId != SML_INVALID_TBS_ID )
    {
        if( SMF_IS_ONLINE_TBS( aTbsId ) == STL_FALSE )
        {
            /**
             * 1. Offline Tablespace의 경우 Media Recovery를 위해
             *    Datafile을 load한다.
             */
            STL_TRY( smpShrinkTablespace( aTbsId, aEnv ) == STL_SUCCESS );

            STL_TRY( smfParallelLoad( aTbsId,
                                      aEnv ) == STL_SUCCESS );

            STL_TRY( smpExtendTablespace( NULL,
                                          aTbsId,
                                          STL_FALSE,
                                          aEnv ) == STL_SUCCESS );
        }

        SMF_SET_TBS_PROCEED_RESTORE( aTbsId, STL_TRUE );
    }

    STL_TRY( smfInitDatafileRestoredLsn( aTbsId, aEnv ) == STL_SUCCESS );

    /**
     * Incremental Backup이 없으면 merge단계 skip
     */
    sSysPersData   = &gSmfWarmupEntry->mSysPersData;
    sBackupSegment = &gSmfWarmupEntry->mBackupSegment;

    sTotalBackupCount = smfGetTotalBackupCount( sBackupSegment );
    
    STL_TRY_THROW( sSysPersData->mBackupChunkCnt > 0, RAMP_SKIP_MERGE_BACKUP );

    STL_TRY( smfOpenCtrlSection( &sFile,                                 
                                 SMF_CTRL_SECTION_INC_BACKUP,
                                 aEnv ) == STL_SUCCESS );
    sState = 1;

    sPageCount = knlGetPropertyBigIntValueByID( KNL_PROPERTY_MAXIMUM_FLUSH_PAGE_COUNT,
                                                KNL_ENV( aEnv ) );

    sBufferSize = (sPageCount + 1) * SMP_PAGE_SIZE;

    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 2;

    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                sBufferSize,
                                (void**)&sBuffer,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sPageFrames = (stlChar*)STL_ALIGN( (stlInt64)sBuffer, SMP_PAGE_SIZE );
    sBufferSize = sPageCount * SMP_PAGE_SIZE;

    /**
     * Apply할 backup list를 analysis하기 위한 heap memory 할당
     */
    sApplyBackupPlanSize = sTotalBackupCount * STL_SIZEOF( smfApplyBackupPlan );

    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                sApplyBackupPlanSize,
                                (void**)&sApplyBackupPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( (void *)sApplyBackupPlan,
               0x00,
               STL_SIZEOF(smfApplyBackupPlan) * sTotalBackupCount );

    /**
     * Apply할 backup list를 분석하여 apply backup plan을 생성한다.
     */
    STL_TRY( smfMakeApplyBackupPlan( &sFile,
                                     aTbsId,
                                     aUntilType,
                                     aUntilValue,
                                     sApplyBackupPlan,
                                     aEnv ) == STL_SUCCESS );

    SMF_INIT_BACKUP_ITERATOR( &sBackupIterator, STL_TRUE );

    /**
     * Controlfile에 기록되어 있는 첫번째 Incremental Backup를 가져온다.
     */
    STL_TRY( smfGetFirstIncBackup( &gSmfWarmupEntry->mBackupSegment,
                                   &sBackupChunk,
                                   &sBackupIterator,
                                   &sExistBackup,
                                   aEnv ) == STL_SUCCESS );
    
    sBackupIdx = 0;

    while( sExistBackup == STL_TRUE )
    {
        if( sApplyBackupPlan[sBackupIdx].mIsFinished == STL_TRUE )
        {
            break;
        }

        if( sApplyBackupPlan[sBackupIdx].mIsApplicable == STL_FALSE )
        {
            /**
             * 다음 Incremental Backup를 가져온다.
             */
            STL_TRY( smfGetNextIncBackup( &gSmfWarmupEntry->mBackupSegment,
                                          &sBackupChunk,
                                          &sBackupIterator,
                                          &sExistBackup,
                                          aEnv ) == STL_SUCCESS );
            
            sBackupIdx++;
            continue;
        }

        /**
         * Applicable backup인지 체크한다.
         */
        STL_TRY( smfIsApplicableBackup( aTbsId,
                                        &(sBackupIterator.mBackupRecord),
                                        &sIsApplicable,
                                        aEnv ) == STL_SUCCESS );

        /**
         * 복구할 backup이 존재하는 경우
         */
        if( sIsApplicable == STL_TRUE )
        {
            STL_TRY( smfApplyBackup( aTbsId,
                                     sPageFrames,
                                     sBufferSize,
                                     &sBackupIterator,
                                     &sBackupChkptLsn,
                                     &sBackupChkptLid,
                                     &sIsSuccess,
                                     aEnv ) == STL_SUCCESS );

            /**
             * Apply할 backup file이 없는 경우 다음 backup file을 
             * apply하지 않고 종료한 후 redo log를 이용한 media recovery를 수행한다.
             */
            if( sIsSuccess == STL_FALSE )
            {
                break;
            }

            /**
             * Backup file을 merge한 후 backup file의 checkpoint 정보를
             * datafile runtime 정보에 저장한다.
             */
            STL_TRY( smfUpdateDatafileRestoredLsn( aTbsId,
                                                   sBackupChkptLsn,
                                                   sBackupChkptLid,
                                                   aEnv ) == STL_SUCCESS );
        }

        /**
         * 다음 Incremental Backup를 가져온다.
         */
        STL_TRY( smfGetNextIncBackup( &gSmfWarmupEntry->mBackupSegment,
                                      &sBackupChunk,
                                      &sBackupIterator,
                                      &sExistBackup,
                                      aEnv ) == STL_SUCCESS );

        sBackupIdx++;
    }

    sState = 1;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sState = 0;
    STL_TRY( smfCloseCtrlSection( &sFile, aEnv ) == STL_SUCCESS );

    if( aTbsId == SML_INVALID_TBS_ID )
    {
        /**
         * 모든 datafile을 flush한다.
         */
        sArg.mBehavior = SMP_FLUSH_DIRTY_PAGE;
        sArg.mLogging  = STL_FALSE;
        sArg.mForCheckpoint = STL_FALSE;

        STL_TRY( knlAddEnvEvent( SML_EVENT_PAGE_FLUSH,
                                 NULL,     /* aEventMem */
                                 &sArg,
                                 STL_SIZEOF(smpFlushArg),
                                 SML_PAGE_FLUSHER_ENV_ID,
                                 KNL_EVENT_WAIT_POLLING,
                                 STL_FALSE,  /* aForceSuccess */
                                 KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        STL_TRY( smfChangeDatafileHeader( NULL,
                                          SMF_CHANGE_DATAFILE_HEADER_FOR_RESTORE,
                                          SML_INVALID_TBS_ID,
                                          SML_INVALID_DATAFILE_ID,
                                          SMR_INVALID_LID,
                                          SMR_INVALID_LSN,
                                          aEnv ) == STL_SUCCESS );
    }
    else
    {
        /**
         * 해당 tablespace의 datafile만 flush한다.
         */
        sTbsArg.mTbsId = aTbsId;
        sTbsArg.mDatafileId = SML_INVALID_DATAFILE_ID;
        sTbsArg.mReason = SMF_CHANGE_DATAFILE_HEADER_FOR_RESTORE;

        STL_TRY( knlAddEnvEvent( SML_EVENT_PAGE_FLUSH_TBS,
                                 NULL,                         /* aEventMem */
                                 (void*)&sTbsArg,              /* aData */
                                 STL_SIZEOF(smpFlushTbsArg),   /* aDataSize */
                                 SML_PAGE_FLUSHER_ENV_ID,
                                 KNL_EVENT_WAIT_POLLING,
                                 STL_FALSE,  /* aForceSuccess */
                                 KNL_ENV( aEnv ) ) == STL_SUCCESS );

        SMF_SET_TBS_PROCEED_RESTORE( aTbsId, STL_FALSE );
    }

    /**
     * Merge Backup은 SML_ERRCODE_TABLESPACE_OFFLINE_ABNORMALLY을 error로 처리한다.
     */
    STL_TRY( stlGetLastErrorCode( KNL_ERROR_STACK(aEnv) ) !=
             SML_ERRCODE_TABLESPACE_OFFLINE_ABNORMALLY );
    
    STL_RAMP( RAMP_SKIP_MERGE_BACKUP );

    return STL_SUCCESS;
    
    STL_FINISH;

    if( aTbsId != SML_INVALID_TBS_ID )
    {
        SMF_SET_TBS_PROCEED_RESTORE( aTbsId, STL_FALSE );
    }

    switch( sState )
    {
        case 2:
            (void) knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                             &sMemMark,
                                             STL_FALSE, /* aFreeChunk */
                                             KNL_ENV(aEnv) );
        case 1:
            (void) smfCloseCtrlSection( &sFile, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief Backup된 datafile page를 pch array에 apply한다.
 * @param[in] aTbsId Merge할 tablespace Id ( invalid Id : database )
 * @param[in] aBuffer Backup file read를 위한 buffer
 * @param[in] aBufferSize Backup file read buffer 크기
 * @param[in] aBackupIterator Incremental backup 정보
 * @param[out] aBackupChkptLsn Backup file에 기록된 최신 checkpoint Lsn
 * @param[out] aBackupChkptLid Backup file에 기록된 최신 checkpoint Lid
 * @param[out] aIsSuccess Backup file의 apply가 완료되었는지 여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfApplyBackup( smlTbsId            aTbsId,
                          stlChar           * aBuffer,
                          stlInt32            aBufferSize,
                          smfBackupIterator * aBackupIterator,
                          smrLsn            * aBackupChkptLsn,
                          smrLid            * aBackupChkptLid,
                          stlBool           * aIsSuccess,
                          smlEnv            * aEnv )
{
    stlChar                * sTailBuffer;
    stlInt32                 sTbsIdx;
    stlInt32                 sFlag;
    stlInt32                 sState = 0;
    stlInt64                 sDatabaseIo;
    stlFile                  sFile;
    stlSize                  sTailSize;
    stlOffset                sFileOffset;
    smfIncBackupFileHeader   sFileHeader;
    smfIncBackupFileTail   * sFileTail;
    stlBool                  sFileExist;
    knlRegionMark            sMemMark;

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "    [RESTORE] merge datafile begin ( backup file - %s )\n",
                        aBackupIterator->mFileName )
             == STL_SUCCESS );

    sFlag = STL_FOPEN_LARGEFILE | STL_FOPEN_READ;

    sDatabaseIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_FILE_IO,
                                                 KNL_ENV(aEnv) );

    if( sDatabaseIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

    /**
     * Backup file을 apply 하기 전에 존재하는지 검사한다.
     */
    STL_TRY( stlExistFile( aBackupIterator->mFileName,
                           &sFileExist,
                           KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    if( sFileExist == STL_FALSE )
    {
        STL_TRY( knlLogMsg( NULL,
                            KNL_ENV( aEnv ),
                            KNL_LOG_LEVEL_INFO,
                            "    [RESTORE] backup file does not exist ( backup file - %s )\n",
                            aBackupIterator->mFileName )
                 == STL_SUCCESS );

        *aIsSuccess = STL_FALSE;
        STL_THROW( RAMP_APPLY_DONE );
    }

    *aIsSuccess = STL_TRUE;

    /**
     * Backup file open
     */
    STL_TRY( smgOpenFile( &sFile,
                          aBackupIterator->mFileName,
                          sFlag,
                          STL_FPERM_OS_DEFAULT,
                          aEnv ) == STL_SUCCESS );
    sState = 1;

    /**
     * Backup file의 file header를 읽음
     */
    STL_TRY( smfReadBackupFileHeader( &sFile,
                                      &sFileHeader,
                                      aEnv ) == STL_SUCCESS );

    sTailSize = STL_ALIGN( sFileHeader.mTbsCount * STL_SIZEOF( smfIncBackupFileTail ),
                           SMP_PAGE_SIZE );

    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 2;

    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                sTailSize + SMP_PAGE_SIZE,
                                (void**)&sTailBuffer,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sFileTail = (smfIncBackupFileTail *)STL_ALIGN( (stlInt64)sTailBuffer, SMP_PAGE_SIZE );

    sFileOffset = SMP_PAGE_SIZE + sFileHeader.mBodySize;

    /**
     * Backup file의 file tail을 읽음
     */
    STL_TRY( smfReadBackupFileTail( &sFile,
                                    sFileOffset,
                                    sTailSize,
                                    sFileTail,
                                    aEnv ) == STL_SUCCESS );

    sTbsIdx = 0;

    if( aTbsId != SML_INVALID_TBS_ID )
    {
        for( ; sTbsIdx < sFileHeader.mTbsCount; sTbsIdx++ )
        {
            if( sFileTail[sTbsIdx].mTbsId == aTbsId )
            {
                break;
            }
        }
    }

    /**
     * Backup에 기록된 page들을 pch array에 반영한다.
     */
    while( STL_TRUE )
    {
        /**
         * Backup file에 존재하는 tablespace를 모두 반영하면 종료
         */
        if( sTbsIdx >= sFileHeader.mTbsCount )
        {
            break;
        }

        /**
         * Backup file에는 존재하지만 controlfile에는 존재하지 않는 경우 skip
         */
        if( gSmfWarmupEntry->mTbsInfoArray[sFileTail[sTbsIdx].mTbsId] == NULL )
        {
            sTbsIdx++;
            continue;
        }

        /**
         * 특정 tablespace만 복구할 경우 해당 tablespace 반영 후 종료
         */
        if( aTbsId != SML_INVALID_TBS_ID )
        {
            if( aTbsId != sFileTail[sTbsIdx].mTbsId )
            {
                break;
            }
        }
        else
        {
            /**
             * Database Media Recovery 시 offline된 tablespace에 대해서는
             * backup을 merge하지 않는다.
             */
            if( SMF_IS_ONLINE_TBS( sFileTail[sTbsIdx].mTbsId ) == STL_FALSE )
            {
                sTbsIdx++;
                continue;
            }
        }

        STL_TRY( smfApplyTbs( &sFile,
                              &sFileTail[sTbsIdx],
                              aBackupIterator,
                              aBuffer,
                              aBufferSize,
                              aEnv ) == STL_SUCCESS );

        sTbsIdx++;
    }

    sState = 1;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sState = 0;
    STL_TRY( smgCloseFile( &sFile,
                           aEnv ) == STL_SUCCESS );

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "    [RESTORE] merge datafile end\n" )
             == STL_SUCCESS );

    /**
     * Apply 완료 후 backup file의 checkpoint 정보를 return하여
     * datafile의 runtime 정보를 갱신한다.
     */
    *aBackupChkptLsn = sFileHeader.mBackupChkptLsn;
    *aBackupChkptLid = sFileHeader.mBackupChkptLid;

    STL_RAMP( RAMP_APPLY_DONE );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                             &sMemMark,
                                             STL_FALSE, /* aFreeChunk */
                                             KNL_ENV(aEnv) );
        case 1:
            (void) smgCloseFile( &sFile, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smfIsApplicableTbs( smlTbsId           aTbsId,
                              smfBackupRecord  * aBackupRecord,
                              stlBool          * aIsApplicable,
                              smlEnv           * aEnv )
{
    smrLsn    sTbsRestoredChkptLsn = SMR_INIT_LSN;
    smrLid    sDummyLid;

    *aIsApplicable = STL_TRUE;

    STL_TRY( smfGetTbsMinRestoredChkptInfo( aTbsId,
                                            &sTbsRestoredChkptLsn,
                                            &sDummyLid,
                                            aEnv ) == STL_SUCCESS );

    /**
     * Backup보다 tablespace의 restored Lsn이 더 크면
     * backup을 반영할 필요가 없다.
     */
    if( aBackupRecord->mBackupLsn < sTbsRestoredChkptLsn )
    {
        *aIsApplicable = STL_FALSE;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smfApplyTbs( stlFile              * aFile,
                       smfIncBackupFileTail * sFileTail,
                       smfBackupIterator    * aBackupIterator,
                       stlChar              * aBuffer,
                       stlInt32               aBufferSize,
                       smlEnv               * aEnv )
{
    smlTbsId      sTbsId;
    smlDatafileId sDatafileId = SML_INVALID_DATAFILE_ID;
    stlOffset     sTbsOffset;
    stlSize       sSize;
    stlInt64      sTbsBackupSize;
    stlInt64      sOffset;
    void        * sPchArrayList = NULL;
    stlInt32      sReadSize;
    stlInt32      sPageCount;
    stlInt64      i;
    stlBool       sIsApplicable;

    sTbsId     = sFileTail->mTbsId;
    sTbsOffset = sFileTail->mTbsOffset;
    sTbsBackupSize = sFileTail->mBackupPageCount * SMP_PAGE_SIZE;

    STL_TRY( smfIsApplicableTbs( sTbsId,
                                 &(aBackupIterator->mBackupRecord),
                                 &sIsApplicable,
                                 aEnv ) == STL_SUCCESS );

    if( sIsApplicable == STL_FALSE )
    {
        sTbsOffset = sFileTail->mTbsOffset + sTbsBackupSize;

        STL_TRY( stlSeekFile( aFile,
                              STL_FSEEK_SET,
                              &sTbsOffset,
                              KNL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );

        STL_THROW( RAMP_SKIP_APPLY );
    }

    STL_TRY( stlSeekFile( aFile,
                          STL_FSEEK_SET,
                          &sTbsOffset,
                          KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    sOffset = 0;

    while( sOffset < sTbsBackupSize )
    {
        /**
         * Backup file을 읽기 위해 read size를 구한다.
         *  - Buffer 크기와 남은 file의 크기 중 최소값
         */
        sReadSize = STL_MIN( aBufferSize, (sTbsBackupSize - sOffset) );

        STL_TRY( stlReadFile( aFile,
                              aBuffer,
                              sReadSize,
                              &sSize,
                              KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

        STL_DASSERT( sReadSize == sSize );

        sPageCount = sReadSize / SMP_PAGE_SIZE;

        for( i = 0; i < sPageCount; i++ )
        {
            /**
             * apply 가능한 backup page만 pch에 반영한다.
             */
            if( SMP_IS_APPLICABLE_BACKUP_PAGE( (aBuffer + i * SMP_PAGE_SIZE),
                                               sTbsId ) == STL_TRUE )
            {
                STL_TRY( smpApplyBackup( aBackupIterator->mFileName,
                                         (aBuffer + i * SMP_PAGE_SIZE),
                                         &sDatafileId,
                                         (void **)&sPchArrayList,
                                         aEnv )
                         == STL_SUCCESS );
            }
        }

        /* MOUNT phase에서 merge datafile은 gmaster의 event handler로 수행되므로
         * 주어진 event가 cancel되었는지 체크한다. */
        if( knlGetCurrStartupPhase() < KNL_STARTUP_PHASE_OPEN )
        {
            STL_TRY( knlCheckEnvEvent( KNL_ENV( aEnv ) ) == STL_SUCCESS );
        }

        sOffset += sReadSize;
    }

    STL_RAMP( RAMP_SKIP_APPLY );

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief Incremental backup을 이용한 merge backup을 위해 runtime datafile정보를 초기화 한다.
 * @param[in]     aTbsId Merge backup을 수행할 tablespace Id( Invalid Id : database merge )
 * @param[in,out] aEnv   Environment 정보
 */
stlStatus smfInitDatafileRestoredLsn( smlTbsId   aTbsId,
                                      smlEnv   * aEnv )
{
    smfTbsInfo          * sTbsInfo;
    smfDatafileInfo     * sDatafileInfo;
    smfDatafileHeader     sDatafileHeader;
    smlDatafileId         sDatafileId;
    smlTbsId              sTbsId;
    stlFile               sFile;
    stlBool               sExistTbs;
    stlBool               sExistFile;
    stlInt32              sState = 0;
    stlInt32              sFlag;
    stlInt64              sDatabaseIo;

    sFlag = STL_FOPEN_LARGEFILE | STL_FOPEN_READ;

    sDatabaseIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_FILE_IO,
                                                 KNL_ENV(aEnv) );

    if( sDatabaseIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

    STL_TRY( smfFirstTbs( &sTbsId, &sExistTbs, aEnv ) == STL_SUCCESS );

    /**
     * Tablespace의 모든 datafile header를 읽어서
     * runtime 정보에 restored Lsn을 설정한다.
     */
    while( sExistTbs == STL_TRUE )
    {
        if( (aTbsId != SML_INVALID_TBS_ID) && (aTbsId != sTbsId) )
        {
            STL_TRY( smfNextTbs( &sTbsId,
                                 &sExistTbs,
                                 aEnv ) == STL_SUCCESS );

            continue;
        }

        /**
         * Database media recovery 시 offline tablespace는 제외
         */
        if( (aTbsId == SML_INVALID_TBS_ID) &&
            (SMF_IS_ONLINE_TBS( sTbsId ) == STL_FALSE) )
        {
            STL_TRY( smfNextTbs( &sTbsId,
                                 &sExistTbs,
                                 aEnv ) == STL_SUCCESS );

            continue;
        }

        sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[sTbsId];

        /**
         * temporary tablespace는 제외
         */
        if( (sTbsInfo != NULL) &&
            (SML_TBS_IS_TEMPORARY( sTbsInfo->mTbsPersData.mAttr ) != STL_TRUE) )
        {
            STL_TRY( smfFirstDatafile( sTbsId,
                                       &sDatafileId,
                                       &sExistFile,
                                       aEnv ) == STL_SUCCESS );

            while( sExistFile == STL_TRUE )
            {
                sDatafileInfo = sTbsInfo->mDatafileInfoArray[sDatafileId];

                STL_TRY( smgOpenFile( &sFile,
                                      sDatafileInfo->mDatafilePersData.mName,
                                      sFlag,
                                      STL_FPERM_OS_DEFAULT,
                                      aEnv )
                         == STL_SUCCESS );
                sState = 1;

                STL_TRY( smfReadDatafileHeader( &sFile,
                                                sDatafileInfo->mDatafilePersData.mName,
                                                &sDatafileHeader,
                                                SML_ENV(aEnv) )
                         == STL_SUCCESS );

                sState = 0;
                STL_TRY( smgCloseFile( &sFile,
                                       aEnv )
                         == STL_SUCCESS );

                /**
                 * Datafile header의 checkpoit Lsn과 checkpoint Lid를
                 * datafile runtime 정보에 설정한다.
                 */
                sDatafileInfo->mRestoredChkptLsn = sDatafileHeader.mCheckpointLsn;
                sDatafileInfo->mRestoredChkptLid = sDatafileHeader.mCheckpointLid;

                if( sDatafileInfo->mRestoredChkptLsn == SMR_INVALID_LSN )
                {
                    sDatafileInfo->mRestoredChkptLsn =
                        sDatafileInfo->mDatafilePersData.mCreationLsn;
                    sDatafileInfo->mRestoredChkptLid =
                        sDatafileInfo->mDatafilePersData.mCreationLid;
                }

                STL_TRY( smfNextDatafile( sTbsId,
                                          &sDatafileId,
                                          &sExistFile,
                                          aEnv ) == STL_SUCCESS );
            }
        }

        STL_TRY( smfNextTbs( &sTbsId,
                             &sExistTbs,
                             aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smgCloseFile( &sFile, aEnv );
    }
    
    return STL_FAILURE;
}

/**
 * @brief Incremental backup을 이용한 merge backup 중에 runtime datafile정보를 갱신한다.
 * @param[in]     aTbsId Merge backup을 수행할 tablespace Id( Invalid Id : database merge )
 * @param[in]     aBackupChkptLsn Backup file의 최신 checkpoint Lsn
 * @param[in]     aBackupChkptLid Backup file의 최신 checkpoint Lid
 * @param[in,out] aEnv   Environment 정보
 */
stlStatus smfUpdateDatafileRestoredLsn( smlTbsId   aTbsId,
                                        smrLsn     aBackupChkptLsn,
                                        smrLid     aBackupChkptLid,
                                        smlEnv   * aEnv )
{
    smfTbsInfo          * sTbsInfo;
    smfDatafileInfo     * sDatafileInfo;
    smlDatafileId         sDatafileId;
    smlTbsId              sTbsId;
    stlBool               sExistTbs;
    stlBool               sExistFile;

    STL_TRY( smfFirstTbs( &sTbsId, &sExistTbs, aEnv ) == STL_SUCCESS );

    /**
     * Datafile의 runtime 정보에 merge가 완료된 backup file의
     * 최신 checkpoint Lsn, Lid를 저장한다.
     */
    while( sExistTbs == STL_TRUE )
    {
        if( (aTbsId != SML_INVALID_TBS_ID) && (aTbsId != sTbsId) )
        {
            STL_TRY( smfNextTbs( &sTbsId,
                                 &sExistTbs,
                                 aEnv ) == STL_SUCCESS );

            continue;
        }

        sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[sTbsId];

        /**
         * temporary tablespace는 제외
         */
        if( (sTbsInfo != NULL) &&
            (SML_TBS_IS_TEMPORARY( sTbsInfo->mTbsPersData.mAttr ) != STL_TRUE) )
        {
            STL_TRY( smfFirstDatafile( sTbsId,
                                       &sDatafileId,
                                       &sExistFile,
                                       aEnv ) == STL_SUCCESS );

            while( sExistFile == STL_TRUE )
            {
                sDatafileInfo = sTbsInfo->mDatafileInfoArray[sDatafileId];

                /**
                 * Merge한 backup보다 최신 datafile의 정보는 변경하지 않는다.
                 */
                if( sDatafileInfo->mRestoredChkptLsn < aBackupChkptLsn )
                {
                    sDatafileInfo->mRestoredChkptLsn = aBackupChkptLsn;
                    sDatafileInfo->mRestoredChkptLid = aBackupChkptLid;
                }

                STL_TRY( smfNextDatafile( sTbsId,
                                          &sDatafileId,
                                          &sExistFile,
                                          aEnv ) == STL_SUCCESS );
            }
        }

        STL_TRY( smfNextTbs( &sTbsId,
                             &sExistTbs,
                             aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_SUCCESS;
}

/**
 * @brief Datafile Info에서 최소의 Restored Lsn을 구한다.
 * @param[in]  aTbsId       Tablespace Identifier
 * @param[out] aMinRestoredLsn Minimum Restored Lsn
 * @param[in,out] aEnv      Environment 정보
 */
stlStatus smfGetMinRestoredLsn( smlTbsId    aTbsId,
                                smrLsn    * aMinRestoredLsn,
                                smlEnv    * aEnv )
{
    smfTbsInfo  * sTbsInfo;
    smlTbsId      sTbsId;
    stlBool       sExistTbs;
    smrLsn        sMinLsn = SMR_INIT_LSN;
    smrLsn        sTbsRestoredChkptLsn = SMR_INIT_LSN;
    smrLid        sDummyLid;

    STL_TRY( smfFirstTbs( &sTbsId, &sExistTbs, aEnv ) == STL_SUCCESS );

    /**
     * Tablespace의 모든 datafile에서 최소 restored Lsn을 구한다.
     */
    while( sExistTbs )
    {
        if( (aTbsId != SML_INVALID_TBS_ID) && (aTbsId != sTbsId) )
        {
            STL_TRY( smfNextTbs( &sTbsId,
                                 &sExistTbs,
                                 aEnv ) == STL_SUCCESS );

            continue;
        }

        sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[sTbsId];

        /**
         * temporary tablespace는 복구에서 제외
         */
        if( (sTbsInfo != NULL) &&
            (SML_TBS_IS_TEMPORARY( sTbsInfo->mTbsPersData.mAttr ) != STL_TRUE) )
        {
            STL_TRY( smfGetTbsMinRestoredChkptInfo( sTbsId,
                                                    &sTbsRestoredChkptLsn,
                                                    &sDummyLid,
                                                    aEnv ) == STL_SUCCESS );

            if( (sMinLsn == SMR_INIT_LSN) || (sMinLsn > sTbsRestoredChkptLsn) )
            {
                sMinLsn = sTbsRestoredChkptLsn;
            }
        }

        STL_TRY( smfNextTbs( &sTbsId,
                             &sExistTbs,
                             aEnv ) == STL_SUCCESS );
    }

    *aMinRestoredLsn = sMinLsn;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Tablespace의 datafile Info에서 최소의 Restored Lsn을 구한다.
 * @param[in]  aTbsId       Tablespace Identifier
 * @param[out] aRestoredChkptLsn Restored Checkpoint Lsn
 * @param[out] aRestoredChkptLid Restored Checkpoint Lid
 * @param[in,out] aEnv      Environment 정보
 */
stlStatus smfGetTbsMinRestoredChkptInfo( smlTbsId    aTbsId,
                                         smrLsn    * aRestoredChkptLsn,
                                         smrLid    * aRestoredChkptLid,
                                         smlEnv    * aEnv )
{
    smfTbsInfo          * sTbsInfo;
    smfDatafileInfo     * sDatafileInfo;
    smlDatafileId         sDatafileId;
    stlBool               sExistFile;
    smrLsn                sMinLsn = SMR_INIT_LSN;
    smrLid                sMinLid = SMR_INVALID_LID;

    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];

    STL_TRY( smfFirstDatafile( aTbsId,
                               &sDatafileId,
                               &sExistFile,
                               aEnv ) == STL_SUCCESS );

    while( sExistFile == STL_TRUE )
    {
        sDatafileInfo = sTbsInfo->mDatafileInfoArray[sDatafileId];

        if( sDatafileInfo->mRestoredChkptLsn == SMR_INVALID_LSN )
        {
            sDatafileInfo->mRestoredChkptLsn = 0;
        }

        if( (sMinLsn == SMR_INIT_LSN) ||
            (sMinLsn > sDatafileInfo->mRestoredChkptLsn) )
        {
            sMinLsn = sDatafileInfo->mRestoredChkptLsn;
            sMinLid = sDatafileInfo->mRestoredChkptLid;
        }

        STL_TRY( smfNextDatafile( aTbsId,
                                  &sDatafileId,
                                  &sExistFile,
                                  aEnv ) == STL_SUCCESS );
    }

    *aRestoredChkptLsn = sMinLsn;
    *aRestoredChkptLid = sMinLid;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Incremental Backup을 수행하기 위해 직전 backup의 backup Lsn을 구한다.
 * @param[in] aTbsId Backup을 수행할 tablespace Id ( invalid Id : database )
 * @param[in] aBackupOption Backup Option ( CUMULATIVE / DIFFERENTIAL )
 * @param[in] aBackupLevel Backup Level
 * @return 이전 backup의 Lsn
 */
smrLsn smfGetPrevBackupLsn( smlTbsId   aTbsId,
                            stlUInt8   aBackupOption,
                            stlInt16   aBackupLevel )
{
    smfIncBackupInfo  * sIncBackup;
    smrLsn              sPrevBackupLsn = SMR_INVALID_LSN;
    stlInt16            sBackupLevel;

    if( aTbsId == SML_INVALID_TBS_ID )
    {
        sIncBackup = &(gSmfWarmupEntry->mDbPersData.mIncBackup);
    }
    else
    {
        sIncBackup = &(gSmfWarmupEntry->mTbsInfoArray[aTbsId]->mTbsPersData.mIncBackup);
    }

    if( aBackupLevel == 0 )
    {
        /**
         * Level 0 Backup 시 유효한 최소 Lsn (0)을 return한다.
         */
        sPrevBackupLsn = 0;
    }
    else
    {
        if( aBackupOption == SML_INCREMENTAL_BACKUP_OPTION_DIFFERENTIAL )
        {
            /**
             * Differential의 경우 주어진 Level보다 같거나 높은 Level의
             * 유효한 Backup Lsn을 이용하여 backup을 수행한다.
             */
            sBackupLevel = aBackupLevel;
        }
        else
        {
            /**
             * Cumulative의 경우 주어진 Level보다 높은 Level의
             * 유효한 Backup Lsn을 이용하여 backup을 수행한다.
             */
            sBackupLevel = aBackupLevel - 1;
        }

        while( sBackupLevel >= 0 )
        {
            if( sIncBackup->mBackupLsn[sBackupLevel] != SMR_INVALID_LSN )
            {
                sPrevBackupLsn = sIncBackup->mBackupLsn[sBackupLevel];
                break;
            }

            sBackupLevel--;
        }
    }

    return sPrevBackupLsn;
}

/**
 * @brief Incremental Backup 수행 후 Controolfile에 bacup 정보를 기록한다.
 * @param[in] aTbsId Backup을 수행한 tablespace Id ( invalid Id : database )
 * @param[in] aBackupLevel Backup Level
 * @param[in] aBackupLsn Backup 수행 직전의 checkpoint Lsn
 * @return 이전 backup의 Lsn
 */
void smfSetPrevBackupInfo( smlTbsId   aTbsId,
                           stlInt16   aBackupLevel,
                           smrLsn     aBackupLsn )
{
    smfTbsInfo        * sTbsInfo;
    smfIncBackupInfo  * sIncBackup;
    stlInt16            sBackupLevel;

    if( aTbsId == SML_INVALID_TBS_ID )
    {
        sIncBackup = &(gSmfWarmupEntry->mDbPersData.mIncBackup);
    }
    else
    {
        sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];
        sIncBackup = &(sTbsInfo->mTbsPersData.mIncBackup);
    }

    /**
     * Level 0 Backup 시 Backup Lsn을 초기화한다.
     */
    if( aBackupLevel == 0 )
    {
        for( sBackupLevel = 0;
             sBackupLevel < SML_INCREMENTAL_BACKUP_LEVEL_MAX;
             sBackupLevel++ )
        {
            sIncBackup->mBackupLsn[sBackupLevel] = SMR_INVALID_LSN;
        }
    }

    sIncBackup->mBackupLsn[aBackupLevel] = aBackupLsn;
}

void smfInitIncrementalBackupInfo( smlTbsId   aTbsId )
{
    smfIncBackupInfo  * sIncBackup;
    stlInt16            sBackupLevel;

    sIncBackup = &(gSmfWarmupEntry->mTbsInfoArray[aTbsId]->mTbsPersData.mIncBackup);

    for( sBackupLevel = 0;
         sBackupLevel < SML_INCREMENTAL_BACKUP_LEVEL_MAX;
         sBackupLevel++ )
    {
        sIncBackup->mBackupLsn[sBackupLevel] = SMR_INVALID_LSN;
    }
}

stlInt64 smfGetBackupSequence( smlTbsId   aTbsId )
{
    smfTbsInfo        * sTbsInfo;
    smfIncBackupInfo  * sIncBackup;
    stlInt64            sBackupSeq;

    if( aTbsId == SML_INVALID_TBS_ID )
    {
        sIncBackup = &(gSmfWarmupEntry->mDbPersData.mIncBackup);
    }
    else
    {
        sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];
        sIncBackup = &(sTbsInfo->mTbsPersData.mIncBackup);
    }

    sBackupSeq = sIncBackup->mBackupSeq;
    sIncBackup->mBackupSeq += 1;

    return sBackupSeq;
}

stlStatus smfReadBackupFileHeader( stlFile                * aFile,
                                   smfIncBackupFileHeader * aFileHeader,
                                   smlEnv                 * aEnv )
{
    stlOffset    sOffset = 0;
    stlChar      sBuffer[SMP_PAGE_SIZE * 2];
    stlChar    * sPage;

    sPage = (stlChar*)( STL_ALIGN( (stlInt64)sBuffer, SMP_PAGE_SIZE ) );
    
    STL_TRY( stlSeekFile( aFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    STL_TRY( stlReadFile( aFile,
                          sPage,
                          SMP_PAGE_SIZE,
                          NULL,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    stlMemcpy( aFileHeader, sPage, STL_SIZEOF(smfIncBackupFileHeader) );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smfWriteBackupFileHeader( stlFile                * aFile,
                                    smfIncBackupFileHeader * aFileHeader,
                                    stlSize                * aWrittenBytes,
                                    smlEnv                 * aEnv )
{
    stlSize         sSize;
    stlChar       * sBlock;
    stlOffset       sOffset = 0;
    stlInt32        sState = 0;
    stlChar       * sFileHeader;
    knlRegionMark   sMemMark;

    sSize = STL_SIZEOF( smfIncBackupFileHeader ) + SMP_PAGE_SIZE * 2;

    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                sSize,
                                (void**)&sBlock,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sFileHeader = (stlChar *)STL_ALIGN( (stlInt64)sBlock, SMP_PAGE_SIZE );

    stlMemset( sFileHeader, 0x00, SMP_PAGE_SIZE );
    stlMemcpy( sFileHeader, aFileHeader, STL_SIZEOF(smfIncBackupFileHeader) );

    STL_TRY( stlSeekFile( aFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    STL_TRY( stlWriteFile( aFile,
                           sFileHeader,
                           SMP_PAGE_SIZE,
                           aWrittenBytes,
                           KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                         &sMemMark,
                                         STL_FALSE, /* aFreeChunk */
                                         KNL_ENV(aEnv) );
    }

    return STL_FAILURE;
}

stlStatus smfReadBackupFileTail( stlFile              * aFile,
                                 stlOffset              aOffset,
                                 stlSize                aTailSize,
                                 smfIncBackupFileTail * aFileTail,
                                 smlEnv               * aEnv )
{
    stlOffset     sOffset = aOffset;

    STL_TRY( stlSeekFile( aFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    STL_TRY( stlReadFile( aFile,
                          aFileTail,
                          aTailSize,
                          NULL,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smfIsApplicableBackup( smlTbsId            aTbsId,
                                 smfBackupRecord   * aBackupRecord,
                                 stlBool           * aIsApplicable,
                                 smlEnv            * aEnv )
{
    smrLsn   sMinRestoredLsn;

    *aIsApplicable = STL_TRUE;

    /**
     * Datafile의 min restored Lsn을 구한다.
     */
    STL_TRY( smfGetMinRestoredLsn( aTbsId,
                                   &sMinRestoredLsn,
                                   aEnv ) == STL_SUCCESS );

    /**
     * 현재 datafile의 최소 Lsn보다 큰 Lsn이 backup에 존재하면
     * backup을 apply한다.
     */
    if( aBackupRecord->mBackupLsn < sMinRestoredLsn )
    {
        *aIsApplicable = STL_FALSE;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

smrLsn smfGetRestoredLsn( void  * aDatafileInfo )
{
    return ((smfDatafileInfo *)aDatafileInfo)->mRestoredChkptLsn;
}

/**
 * @brief Incremental Backup을 이용한 Restore 시 Backup List를 분석하여 apply plan을 생성한다.
 * @param[in] aFile Controlfile의 file pointer
 * @param[in] aTbsId Backup을 Restore하기 위한 Tablespace Id
 * @param[in] aUntilType Restore를 종료할 Until Type
 * @param[in] aUntilValue Restore를 종료할 Until Value
 * @param[out] aApplyBackupPlan Backup List를 분석하여 생성된 apply plan
 * @param[in,out] aEnv Controlfile의 file pointer
 */
stlStatus smfMakeApplyBackupPlan( stlFile             * aFile,
                                  smlTbsId              aTbsId,
                                  stlUInt8              aUntilType,
                                  stlInt64              aUntilValue,
                                  smfApplyBackupPlan  * aApplyBackupPlan,
                                  smlEnv              * aEnv )
{
    stlInt64            sBackupIdx;
    smfBackupIterator   sBackupIterator;
    stlInt64            i;
    stlBool             sExistBackup;
    smfBackupChunk    * sBackupChunk = NULL;

    SMF_INIT_BACKUP_ITERATOR( &sBackupIterator, STL_TRUE );

    STL_TRY( smfGetFirstIncBackup( &gSmfWarmupEntry->mBackupSegment,
                                   &sBackupChunk,
                                   &sBackupIterator,
                                   &sExistBackup,
                                   aEnv ) == STL_SUCCESS );

    sBackupIdx = 0;

    while( sExistBackup == STL_TRUE )
    {
        /**
         * 현재 backup list 이전의 backup list를 보고 applicable을 설정한다.
         */
        aApplyBackupPlan[sBackupIdx].mTbsId        = sBackupIterator.mBackupRecord.mTbsId;
        aApplyBackupPlan[sBackupIdx].mLevel        = sBackupIterator.mBackupRecord.mLevel;
        aApplyBackupPlan[sBackupIdx].mObjectType   = sBackupIterator.mBackupRecord.mObjectType;
        aApplyBackupPlan[sBackupIdx].mIsApplicable = STL_TRUE;

        /**
         * Controlfile backup은 applicable이 아니다.
         */
        if( sBackupIterator.mBackupRecord.mObjectType == SMF_INCREMENTAL_BACKUP_OBJECT_TYPE_CONTROLFILE )
        {
            aApplyBackupPlan[sBackupIdx].mIsApplicable = STL_FALSE;

            /**
             * 다음 Incremental Backup를 가져온다.
             */
            STL_TRY( smfGetNextIncBackup( &gSmfWarmupEntry->mBackupSegment,
                                          &sBackupChunk,
                                          &sBackupIterator,
                                          &sExistBackup,
                                          aEnv ) == STL_SUCCESS );

            sBackupIdx++;
            continue;
        }

        /**
         * 불완전 복구를 위한 until value가 포함된 backup을
         * apply 하지 않기 위해 until condition check
         */
        if( aUntilType == SML_RESTORE_UNTIL_TYPE_CHANGE )
        {
            if( sBackupIterator.mBackupRecord.mBackupLsn > aUntilValue )
            {
                aApplyBackupPlan[sBackupIdx].mIsApplicable = STL_FALSE;
                aApplyBackupPlan[sBackupIdx].mIsFinished = STL_TRUE;
                break;
            }
        }
        
        for( i = sBackupIdx - 1; i >= 0; i-- )
        {
            /**
             * 이전 backup list가 applicable이 아니면 skip
             */
            if( aApplyBackupPlan[i].mIsApplicable == STL_FALSE )
            {
                continue;
            }

            /**
             * database level 0 backup인 경우 이전 backup들은 모두 inappplicable이다.
             */
            if( (sBackupIterator.mBackupRecord.mTbsId == SML_INVALID_TBS_ID) &&
                (sBackupIterator.mBackupRecord.mLevel == 0) )
            {
                aApplyBackupPlan[i].mIsApplicable = STL_FALSE;

                if( (aApplyBackupPlan[i].mTbsId == SML_INVALID_TBS_ID) &&
                    (aApplyBackupPlan[i].mLevel == 0) )
                {
                    break;
                }
            }
            else
            {
                /**
                 * 동일한 object의 backup 이면
                 */
                if( aApplyBackupPlan[i].mTbsId == sBackupIterator.mBackupRecord.mTbsId )
                {
                    /**
                     * Level이 높은 backup이 있으면 종료
                     */
                    if( aApplyBackupPlan[i].mLevel < sBackupIterator.mBackupRecord.mLevel )
                    {
                        break;
                    }

                    /**
                     * Level이 '0'이 아닌 경우 같은 backup이 있으면 종료
                     */
                    if( (sBackupIterator.mBackupRecord.mLevel != 0) &&
                        (aApplyBackupPlan[i].mLevel == sBackupIterator.mBackupRecord.mLevel) )
                    {
                        break;
                    }

                    aApplyBackupPlan[i].mIsApplicable = STL_FALSE;
                }
            }
        }

        /**
         * 다음 Incremental Backup를 가져온다.
         */
        STL_TRY( smfGetNextIncBackup( &gSmfWarmupEntry->mBackupSegment,
                                      &sBackupChunk,
                                      &sBackupIterator,
                                      &sExistBackup,
                                      aEnv ) == STL_SUCCESS );

        sBackupIdx++;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Datafile Recovery에서 Full Backup을 이용한 Restore
 * @param [in]     aDatafileInfo       Datafile Information
 * @param [out]    aRestoredLsn        Backup File에서 얻는 Restored Lsn
 * @param [out]    aRestoredLid        Backup File에서 얻는 Restored Lid
 * @param [in,out] aEnv                environment
 */ 
stlStatus smfRestoreDatafileByFullBackup( smrRecoveryDatafileInfo  * aDatafileInfo,
                                          smrLsn                   * aRestoredLsn,
                                          smrLid                   * aRestoredLid,
                                          smlEnv                   * aEnv )
{ 
    stlInt32                   sState = 0;
    stlFile                    sFile;
    stlInt32                   sFlag;
    stlInt64                   sSysFileIo;
    smfDatafileHeader          sFullBackupHeader;

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "    [RESTORE] restore begin ( backup file - %s )\n",
                        aDatafileInfo->mBackupName )
             == STL_SUCCESS );
    
    sFlag = STL_FOPEN_READ;

    sSysFileIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_SYSTEM_FILE_IO,
                                                KNL_ENV(aEnv) );

    if( sSysFileIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }
    
    /**
     * src file open.
     */
    STL_TRY( stlOpenFile( &sFile,
                          aDatafileInfo->mBackupName,
                          sFlag,
                          STL_FPERM_OS_DEFAULT,
                          KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    sState = 1;
    
    /**
     * (Backup)Datafile Header에서 Lsn 얻는다.
     */
    STL_TRY( smfReadDatafileHeader( &sFile,
                                    aDatafileInfo->mBackupName,
                                    &sFullBackupHeader,
                                    aEnv )
             == STL_SUCCESS );
    
    *aRestoredLsn = sFullBackupHeader.mCheckpointLsn;
    *aRestoredLid = sFullBackupHeader.mCheckpointLid;

    if( aDatafileInfo->mForCorruption == STL_TRUE )
    {
        STL_TRY( smpRestoreDatafileByFullBackup4Corruption( &sFile,
                                                            aDatafileInfo,
                                                            sFullBackupHeader.mCheckpointLsn,
                                                            aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( smpRestoreDatafileByFullBackup( &sFile,
                                                 aDatafileInfo,
                                                 sFullBackupHeader.mCheckpointLsn,
                                                 aEnv )
                 == STL_SUCCESS );
    }

    
    sState = 0;
    STL_TRY( stlCloseFile( &sFile,
                           KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "    [RESTORE] restore end ( backup file - %s )\n",
                        aDatafileInfo->mBackupName )
             == STL_SUCCESS );
             
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void) stlCloseFile( &sFile, KNL_ERROR_STACK( aEnv ) );
        default:
            (void) knlLogMsg( NULL,
                              KNL_ENV( aEnv ),
                              KNL_LOG_LEVEL_INFO,
                              "    [RESTORE] restore fail ( backup file - %s )\n",
                              aDatafileInfo->mBackupName );
            break;
    }
    
    return STL_FAILURE;
}


/**
 * @brief Datafile Recovery에서 Datafile을 Incremental Backup 이용하여 Restore
 * @param [in]     aRecoveryInfo       Media Recovery Information
 * @param [out]    aRestoredLsn        Backup File에서 얻는 Restored Lsn
 * @param [out]    aRestoredLid        Backup File에서 얻는 Restored Lid
 * @param [in,out] aEnv                environment
 */ 
stlStatus smfRestoreDatafileByIncBackup( smrMediaRecoveryInfo * aRecoveryInfo,
                                         smrLsn               * aRestoredLsn,
                                         smrLid               * aRestoredLid,
                                         smlEnv               * aEnv )
{
    smfApplyBackupPlan * sApplyBackupPlan;
    stlInt64             sApplyBackupPlanSize;
    stlFile              sFile;
    stlChar            * sBuffer;
    stlChar            * sPageFrames;
    stlInt64             sPageCount;
    stlInt64             sBackupIdx;
    stlInt64             sBufferSize;
    stlBool              sExistBackup;
    stlBool              sIsSuccess;
    smfBackupIterator    sBackupIterator;
    stlInt32             sState = 0;
    knlRegionMark        sMemMark;
    smfBackupSegment   * sBackupSegment;
    smfBackupChunk     * sBackupChunk = NULL;
    smfSysPersData     * sSysPersData;
    stlInt64             sTotalBackupCount;
    
    sSysPersData   = &gSmfWarmupEntry->mSysPersData;
    sBackupSegment = &gSmfWarmupEntry->mBackupSegment;
    
    STL_TRY_THROW( sSysPersData->mBackupChunkCnt > 0, RAMP_SKIP );

    STL_TRY( smfOpenCtrlSection( &sFile,
                                 SMF_CTRL_SECTION_INC_BACKUP,
                                 aEnv ) == STL_SUCCESS );
    sState = 1;

    sPageCount = knlGetPropertyBigIntValueByID( KNL_PROPERTY_MAXIMUM_FLUSH_PAGE_COUNT,
                                                KNL_ENV( aEnv ) );

    sBufferSize = (sPageCount + 1) * SMP_PAGE_SIZE;

    KNL_INIT_REGION_MARK( &sMemMark );

    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 2;

    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                sBufferSize,
                                (void **) &sBuffer,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sPageFrames = (stlChar *) STL_ALIGN( (stlInt64) sBuffer, SMP_PAGE_SIZE );
    sBufferSize = sPageCount * SMP_PAGE_SIZE;

    sTotalBackupCount = smfGetTotalBackupCount( sBackupSegment );
    
    sApplyBackupPlanSize = sTotalBackupCount * STL_SIZEOF( smfApplyBackupPlan );

     STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                sApplyBackupPlanSize,
                                (void**)&sApplyBackupPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( (void *)sApplyBackupPlan,
               0x00,
               STL_SIZEOF(smfApplyBackupPlan) * sTotalBackupCount );

    /**
     * Apply할 backup list를 분석하여 apply backup plan을 생성한다.
     */
    STL_TRY( smfMakeApplyBackupPlan( &sFile,
                                     SML_INVALID_TBS_ID,
                                     SML_RESTORE_UNTIL_TYPE_NONE,
                                     -1,
                                     sApplyBackupPlan,
                                     aEnv ) == STL_SUCCESS );

    SMF_INIT_BACKUP_ITERATOR( &sBackupIterator, STL_TRUE );

    STL_TRY( smfGetFirstIncBackup( &gSmfWarmupEntry->mBackupSegment,
                                   &sBackupChunk,
                                   &sBackupIterator,
                                   &sExistBackup,
                                   aEnv ) == STL_SUCCESS );

    sBackupIdx = 0;

    while( sExistBackup == STL_TRUE )
    {
        if( sApplyBackupPlan[sBackupIdx].mIsApplicable == STL_FALSE )
        {
            /**
             * 다음 Incremental Backup를 가져온다.
             */
            STL_TRY( smfGetNextIncBackup( &gSmfWarmupEntry->mBackupSegment,
                                          &sBackupChunk,
                                          &sBackupIterator,
                                          &sExistBackup,
                                          aEnv ) == STL_SUCCESS );

            sBackupIdx++;
            continue;
        }

        /**
         * Merge Backup 과 다르게 Applicable 인지 확인 위해 Restored Lsn 검사를 하지 않음.
         */
        STL_TRY( smfApplyBackup4MediaRecovery( aRecoveryInfo,
                                               sPageFrames,
                                               sBufferSize,
                                               &sBackupIterator,
                                               aRestoredLsn,
                                               aRestoredLid,
                                               &sIsSuccess,
                                               aEnv ) == STL_SUCCESS );
        

        /**
         * 다음 Incremental Backup를 가져온다.
         */
        STL_TRY( smfGetNextIncBackup( &gSmfWarmupEntry->mBackupSegment,
                                      &sBackupChunk,
                                      &sBackupIterator,
                                      &sExistBackup,
                                      aEnv ) == STL_SUCCESS );

        sBackupIdx++;
    }
    
    sState = 1;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sState = 0;
    STL_TRY( smfCloseCtrlSection( &sFile, aEnv ) == STL_SUCCESS );
                                         
    STL_RAMP( RAMP_SKIP );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                             &sMemMark,
                                             STL_FALSE, /* aFreeChunk */
                                             KNL_ENV(aEnv) );
        case 1:
            (void) smfCloseCtrlSection( &sFile, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}

/**
 * @brief Recovery를 위해 Incremental Backup File을 이용하여 Restore한다.
 * @param [in]     aRecoveryInfo  Meida Recovery Info
 * @param [in]     aBuffer        Backup File Read위한 Buffer
 * @param [in]     aBufferSize    Buffer Size
 * @param [in]     aBackupIterator  Incrermental Backup File 정보
 * @param [out]    aRestoredLsn   Backup File에 기록된 최신 Checkpoint Lsn
 * @param [out]    aRestoredLid   Backup File에 기록된 최신 Checkpoint Lid
 * @param [out]    aIsSuccess     Backup File의 Apply가 완료되었는지 여부
 * @param [in,out] aEnv           envrionment 
 */ 
stlStatus smfApplyBackup4MediaRecovery( smrMediaRecoveryInfo * aRecoveryInfo,
                                        stlChar              * aBuffer,
                                        stlInt32               aBufferSize,
                                        smfBackupIterator    * aBackupIterator,
                                        smrLsn               * aRestoredLsn,
                                        smrLid               * aRestoredLid,
                                        stlBool              * aIsSuccess,
                                        smlEnv               * aEnv )
{
    stlChar                  * sTailBuffer;
    stlInt32                   sTbsIdx;
    stlInt32                   sFlag;
    stlInt32                   sState = 0;
    stlInt64                   sDatabaseIo;
    stlFile                    sFile;
    stlSize                    sTailSize;
    stlOffset                  sFileOffset;
    smfIncBackupFileHeader     sFileHeader;
    smfIncBackupFileTail     * sFileTail;
    smfIncBackupFileTail     * sTempTail;
    stlBool                    sFileExist;
    knlRegionMark              sMemMark;
    smrLsn                     sBackupChkptLsn = SMR_INVALID_LSN;
    smrLid                     sBackupChkptLid = SMR_INVALID_LID;
    smrRecoveryDatafileInfo  * sDatafileInfo;
    
    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "    [RESTORE] restore begin ( backup file - %s )\n",
                        aBackupIterator->mFileName )
             == STL_SUCCESS );

    sFlag = STL_FOPEN_LARGEFILE | STL_FOPEN_READ;

    sDatabaseIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_FILE_IO,
                                                 KNL_ENV(aEnv) );

    if( sDatabaseIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

    /**
     * Backup file을 apply 하기 전에 존재하는지 검사한다.
     */
    STL_TRY( stlExistFile( aBackupIterator->mFileName,
                           &sFileExist,
                           KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    if( sFileExist == STL_FALSE )
    {
        STL_TRY( knlLogMsg( NULL,
                            KNL_ENV( aEnv ),
                            KNL_LOG_LEVEL_INFO,
                            "    [RESTORE] backup file does not exist ( backup file - %s )\n",
                            aBackupIterator->mFileName )
                 == STL_SUCCESS );

        *aIsSuccess = STL_FALSE;
        STL_THROW( RAMP_APPLY_DONE );
    }

    *aIsSuccess = STL_TRUE;

    /**
     * Backup file open
     */
    STL_TRY( smgOpenFile( &sFile,
                          aBackupIterator->mFileName,
                          sFlag,
                          STL_FPERM_OS_DEFAULT,
                          aEnv ) == STL_SUCCESS );
    sState = 1;

    /**
     * Backup file의 file header를 읽음
     */
    STL_TRY( smfReadBackupFileHeader( &sFile,
                                      &sFileHeader,
                                      aEnv ) == STL_SUCCESS );

    sTailSize = STL_ALIGN( sFileHeader.mTbsCount * STL_SIZEOF( smfIncBackupFileTail ),
                           SMP_PAGE_SIZE );

    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 2;

    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                sTailSize + SMP_PAGE_SIZE,
                                (void**)&sTailBuffer,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sTailBuffer, 0x00, sTailSize + SMP_PAGE_SIZE );

    sFileTail = (smfIncBackupFileTail *)STL_ALIGN( (stlInt64)sTailBuffer, SMP_PAGE_SIZE );

    sFileOffset = SMP_PAGE_SIZE + sFileHeader.mBodySize;

    /**
     * Backup file의 file tail을 읽음
     */
    STL_TRY( smfReadBackupFileTail( &sFile,
                                    sFileOffset,
                                    sTailSize,
                                    sFileTail,
                                    aEnv ) == STL_SUCCESS );
    
    /**
     * Datafile Array에서 Using Backup Option이 FALSE인 Datafile만 적용한다.
     */
    STL_RING_FOREACH_ENTRY( &aRecoveryInfo->mDatafileList, sDatafileInfo )
    {
        if( sDatafileInfo->mUsingBackup == STL_FALSE )
        {
            for( sTbsIdx = 0; sTbsIdx < sFileHeader.mTbsCount; sTbsIdx++ )
            {
                if( sFileTail[sTbsIdx].mTbsId == sDatafileInfo->mTbsId )
                {
                    break;
                }
            }

            if( sTbsIdx >= sFileHeader.mTbsCount )
            {
                sTempTail = NULL;
            }
            else
            {
                sTempTail = &sFileTail[sTbsIdx];
            }
            
            if( sDatafileInfo->mForCorruption == STL_TRUE )
            {
                STL_TRY( smpRestoreDatafileByIncBackup4Corruption( &sFile,
                                                                   sDatafileInfo,
                                                                   aBackupIterator,
                                                                   sTempTail,
                                                                   aBuffer,
                                                                   aBufferSize,
                                                                   aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                STL_TRY( smpRestoreDatafileByIncBackup( &sFile,
                                                        sDatafileInfo,
                                                        aBackupIterator,
                                                        sTempTail,
                                                        aBuffer,
                                                        aBufferSize,
                                                        aEnv )
                     == STL_SUCCESS );
            }
        }
    }

    sState = 1;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( smgCloseFile( &sFile, aEnv ) == STL_SUCCESS );

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "    [RESTORE] merge datafile end\n" )
             == STL_SUCCESS );

    sBackupChkptLsn = sFileHeader.mBackupChkptLsn;
    sBackupChkptLid = sFileHeader.mBackupChkptLid;
    
    STL_RAMP( RAMP_APPLY_DONE );

    *aRestoredLsn = sBackupChkptLsn;
    *aRestoredLid = sBackupChkptLid;
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                             &sMemMark,
                                             STL_FALSE, /* aFreeChunk */
                                             KNL_ENV(aEnv) );
        case 1:
            (void) smgCloseFile( &sFile, aEnv );
    }
    
    return STL_FAILURE;
}

/**
 * @brief Tablespace Id에 대해 Valid한 Incremental Backup이 있는지 확인.
 * @param[in] aTbsId  Tablespace Id
 * @param[out] aExist  Exist Flag
 * @param[in,out] aEnv  Environment
 * @remark Incremental Backup Header의 TbsId를 확인하여 판단한다.
 * <BR> Database(SML_INVALID_TBS_ID) Backup, 또는
 * <BR> 해당 Tablespace Id를 갖는 Backup이 존재하는지 확인.
 */ 
stlStatus smfExistValidIncBackup( smlTbsId   aTbsId,
                                  stlBool  * aExist,
                                  smlEnv   * aEnv )
{
    knlRegionMark             sMemMark;
    stlInt32                  sState = 0;
    stlFile                   sCtrlFile;
    stlFile                   sIncFile;
    stlInt64                  sPlanSize;
    smfApplyBackupPlan      * sApplyBackupPlan;
    smfBackupIterator         sBackupIterator;
    stlBool                   sExistBackup   = STL_FALSE;
    stlBool                   sExistFile     = STL_FALSE;
    stlBool                   sIncBackupOpen = STL_FALSE;
    stlInt32                  sFlag;
    stlInt64                  sDatabaseIo;
    smfIncBackupFileHeader    sFileHeader;
    smfBackupSegment        * sBackupSegment;
    smfBackupChunk          * sBackupChunk;
    smfSysPersData          * sSysPersData;
    stlInt64                  sTotalBackupCount;
    
    *aExist = STL_FALSE;

    sSysPersData   = &gSmfWarmupEntry->mSysPersData;
    sBackupSegment = &gSmfWarmupEntry->mBackupSegment;

    sTotalBackupCount = smfGetTotalBackupCount( sBackupSegment );

    STL_TRY_THROW( sSysPersData->mBackupChunkCnt > 0, RAMP_ERR_NOT_EXIST_BACKUP );
    
    KNL_INIT_REGION_MARK( &sMemMark );

    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               KNL_ENV( aEnv ) ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( smfOpenCtrlSection( &sCtrlFile,
                                 SMF_CTRL_SECTION_INC_BACKUP,
                                 aEnv ) == STL_SUCCESS );
    sState = 2;

    sPlanSize = sTotalBackupCount * STL_SIZEOF( smfApplyBackupPlan );

    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                sPlanSize,
                                (void**)&sApplyBackupPlan,
                                KNL_ENV(aEnv) ) == STL_SUCCESS );

    stlMemset( (void *)sApplyBackupPlan,
               0x00,
               STL_SIZEOF(smfApplyBackupPlan) * sTotalBackupCount );

    STL_TRY( smfMakeApplyBackupPlan( &sCtrlFile,
                                     SML_INVALID_TBS_ID,
                                     SML_RESTORE_UNTIL_TYPE_NONE,
                                     -1,
                                     sApplyBackupPlan,
                                     aEnv ) == STL_SUCCESS );

    SMF_INIT_BACKUP_ITERATOR( &sBackupIterator, STL_TRUE );

    STL_TRY( smfGetFirstIncBackup( &gSmfWarmupEntry->mBackupSegment,
                                   &sBackupChunk,
                                   &sBackupIterator,
                                   &sExistBackup,
                                   aEnv ) == STL_SUCCESS );
    
    sFlag = STL_FOPEN_LARGEFILE | STL_FOPEN_READ;

    sDatabaseIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_FILE_IO,
                                                 KNL_ENV(aEnv) );

    if( sDatabaseIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }
    
    while( sExistBackup == STL_TRUE )
    {
        STL_TRY( stlExistFile( sBackupIterator.mFileName,
                               &sExistFile,
                               KNL_ERROR_STACK(aEnv) ) == STL_SUCCESS );

        STL_TRY_THROW( sExistFile == STL_TRUE, RAMP_SKIP );

        STL_TRY( smgOpenFile( &sIncFile,
                              sBackupIterator.mFileName,
                              sFlag,
                              STL_FPERM_OS_DEFAULT,
                              aEnv ) == STL_SUCCESS );
        sIncBackupOpen = STL_TRUE;
        
        STL_TRY( smfReadBackupFileHeader( &sIncFile,
                                          &sFileHeader,
                                          aEnv ) == STL_SUCCESS );

        sIncBackupOpen = STL_FALSE;
        STL_TRY( smgCloseFile( &sIncFile,
                               aEnv ) == STL_SUCCESS );
        
        if( (sFileHeader.mTbsId == SML_INVALID_TBS_ID) ||
            (sFileHeader.mTbsId == aTbsId ) )
        {
            *aExist = STL_TRUE;
            break;
        }
        
        STL_RAMP( RAMP_SKIP );
        STL_TRY( smfGetNextIncBackup( &gSmfWarmupEntry->mBackupSegment,
                                      &sBackupChunk,
                                      &sBackupIterator,
                                      &sExistBackup,
                                      aEnv ) == STL_SUCCESS );
    }
    
    sState = 1;
    STL_TRY( smfCloseCtrlSection( &sCtrlFile, aEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       KNL_ENV(aEnv) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_EXIST_BACKUP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_NOT_EXIST_BACKUP,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    if( sIncBackupOpen == STL_TRUE )
    {
        (void) smgCloseFile( &sIncFile, aEnv );
    }
    
    switch( sState )
    {
        case 2:
            (void) smfCloseCtrlSection( &sCtrlFile, aEnv );
        case 1:
            (void) knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                             &sMemMark,
                                             STL_FALSE,
                                             KNL_ENV( aEnv ) );
        default:
            break;
    }
    
    return STL_FAILURE;
}


/** @} */
