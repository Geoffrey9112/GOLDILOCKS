/*******************************************************************************
 * smfTbsMgr.c
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
#include <smfDef.h>
#include <smf.h>
#include <smr.h>
#include <smxl.h>
#include <smg.h>
#include <smfCtrlFileMgr.h>
#include <smfTbsMgr.h>

/**
 * @file smfTbsMgr.c
 * @brief Storage Manager Layer Datafile Startup Internal Routines
 */

smfWarmupEntry * gSmfWarmupEntry;
extern knlFxTableDesc gTablespaceTableDesc;
extern knlFxTableDesc gDatafileTableDesc;
extern knlFxTableDesc gDatafileHeaderTableDesc;
extern knlFxTableDesc gDiskPageDumpTableDesc;
extern knlFxTableDesc gCtrlSysSectionTableDesc;
extern knlFxTableDesc gCtrlLogSectionTableDesc;
extern knlFxTableDesc gCtrlDBSectionTableDesc;
extern knlFxTableDesc gCtrlDatafileTableDesc;
extern knlFxTableDesc gBackupTableDesc;
extern knlFxTableDesc gDumpIncBackupTableDesc;
extern knlFxTableDesc gCtrlIncBackupSectionTableDesc;
extern smrRedoVector gSmfRedoVectors[];
extern smxlUndoFunc gSmfUndoFuncs[];

/**
 * @addtogroup smf
 * @{
 */

/**
 * @brief Datafile Component를 NO MOUNT 모드로 전이한다.
 * @param[out] aWarmupEntry Datafile Component의 Warmup Entry 포인터 
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfStartupNoMount( void  ** aWarmupEntry,
                             smlEnv * aEnv )
{
    smfWarmupEntry * sWarmupEntry;
    stlInt32         i;
    stlChar          sPropertyName[KNL_PROPERTY_STRING_MAX_LENGTH];

    STL_TRY( knlCacheAlignedAllocFixedStaticArea( STL_SIZEOF( smfWarmupEntry ),
                                                  (void**)&sWarmupEntry,
                                                  (knlEnv*)aEnv ) == STL_SUCCESS );

    stlMemset( (void*)sWarmupEntry,
               0x00,
               STL_SIZEOF(smfWarmupEntry) );

    /**
     * NOMOUNT 용 Tablespace info array 생성
     */
    
    STL_TRY( knlCacheAlignedAllocFixedStaticArea( SML_TBS_MAX_COUNT * STL_SIZEOF(smfTbsInfo*),
                                                  (void**)&sWarmupEntry->mTbsInfoArray,
                                                  (knlEnv*)aEnv ) == STL_SUCCESS );
    sWarmupEntry->mNoMountTbsInfoArray = sWarmupEntry->mTbsInfoArray;

    stlMemset( (void*)sWarmupEntry->mTbsInfoArray,
               0x00,
               SML_TBS_MAX_COUNT * STL_SIZEOF(smfTbsInfo*) );

    /**
     * no-mount phase를 위해 Dummy Tablespace infomation을 채운다.
     */
    
    STL_TRY( smfFillDummyTbsInfo( sWarmupEntry->mTbsInfoArray,
                                  aEnv )
             == STL_SUCCESS );

    STL_TRY( knlInitLatch( &sWarmupEntry->mLatch,
                           STL_TRUE,
                           (knlEnv*)aEnv ) == STL_SUCCESS );

    sWarmupEntry->mDataAccessMode = SML_DATA_ACCESS_MODE_READ_WRITE;
    sWarmupEntry->mLastFlushedTbsId = 0;
    sWarmupEntry->mLastFlushedPid = 0;
    sWarmupEntry->mFlushedPageCount = 0;
    sWarmupEntry->mPageIoCount = 0;
    sWarmupEntry->mCtrlLoadState = SMF_CTRL_FILE_LOAD_STATE_NONE;
    sWarmupEntry->mCtrlFileCnt = knlGetPropertyBigIntValueByID( KNL_PROPERTY_CONTROL_FILE_COUNT,
                                                                KNL_ENV(aEnv) );

    SMF_INIT_BACKUP_SEGMENT( &sWarmupEntry->mBackupSegment );
    
    for( i = 0; i < SMF_CTRL_FILE_MAX_COUNT; i++ )
    {
        stlSnprintf( sPropertyName,
                     KNL_PROPERTY_STRING_MAX_LENGTH,
                     "CONTROL_FILE_%d",
                     i );

        STL_TRY( knlGetPropertyValueByName( sPropertyName,
                                            (void*)sWarmupEntry->mCtrlFileName[i],
                                            KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
    }
    
    /**
     * Database의 Backup을 위한 정보 초기화
     */
    SMF_INIT_DB_BACKUP_INFO( &sWarmupEntry->mBackupInfo, aEnv );

    gSmfWarmupEntry = sWarmupEntry;
    *aWarmupEntry = (void*)sWarmupEntry;

    smrRegisterRedoVectors( SMG_COMPONENT_DATAFILE,
                            gSmfRedoVectors );
    
    smxlRegisterUndoFuncs( SMG_COMPONENT_DATAFILE, 
                           gSmfUndoFuncs );

    STL_TRY( smfRegisterFxTables( aEnv ) == STL_SUCCESS );

    smgRegisterPendOp( SMG_PEND_CREATE_TABLESPACE, smfCreateTbsPending );
    smgRegisterPendOp( SMG_PEND_DROP_TABLESPACE, smfDropTbsPending );
    smgRegisterPendOp( SMG_PEND_ADD_DATAFILE, smfAddDatafilePending );
    smgRegisterPendOp( SMG_PEND_DROP_DATAFILE, smfDropDatafilePending );
    smgRegisterPendOp( SMG_PEND_ONLINE_TABLESPACE, smfOnlineTbsPending );
    smgRegisterPendOp( SMG_PEND_OFFLINE_TABLESPACE, smfOfflineTbsPending );
    smgRegisterPendOp( SMG_PEND_CREATE_DATAFILE, smfCreateDatafilePending );
    smgRegisterPendOp( SMG_PEND_RENAME_DATAFILE, smfRenameDatafilePending );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Datafile Component를 MOUNT 모드로 전이한다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfStartupMount( smlEnv * aEnv )
{
    /**
     * MOUNT/OPEN 용 Tablespace info array 생성
     */
    
    STL_TRY( knlCacheAlignedAllocFixedStaticArea( SML_TBS_MAX_COUNT * STL_SIZEOF(smfTbsInfo*),
                                                  (void**)&gSmfWarmupEntry->mTbsInfoArray,
                                                  (knlEnv*)aEnv ) == STL_SUCCESS );

    stlMemset( (void*)gSmfWarmupEntry->mTbsInfoArray,
               0x00,
               SML_TBS_MAX_COUNT * STL_SIZEOF(smfTbsInfo*) );
    
    STL_TRY( smfLoadSysCtrlSection( aEnv ) == STL_SUCCESS );
    STL_TRY( smfLoadBackupCtrlSection( aEnv ) == STL_SUCCESS );
    STL_TRY( smfLoadDbCtrlSection( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Datafile Component를 PREOPEN 모드로 전이한다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfStartupPreOpen( smlEnv * aEnv )
{
    if( smfGetServerState() != SML_SERVER_STATE_NONE )
    {
        STL_TRY( knlLogMsg( NULL,
                            KNL_ENV(aEnv),
                            KNL_LOG_LEVEL_INFO,
                            "[STARTUP-SM] LOAD DATAFILES\n" )
                 == STL_SUCCESS );
    
        STL_TRY( smfParallelLoad( SMF_INVALID_PHYSICAL_TBS_ID,
                                  aEnv )
                 == STL_SUCCESS );
        
        STL_TRY( knlLogMsg( NULL,
                            KNL_ENV(aEnv),
                            KNL_LOG_LEVEL_INFO,
                            "[STARTUP-SM] REFINE TABLESPACE AND DATAFILE\n" )
                 == STL_SUCCESS );
    
        STL_TRY( smfRefineAtStartupPreOpen( aEnv ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Datafile Component를 OPEN 모드로 전이한다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfStartupOpen( smlEnv * aEnv )
{
    STL_TRY( smfRefineAtStartupOpen( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Datafile Component를 Warmup 시킨다.
 * @param[in] aWarmupEntry Datafile Component의 Warmup Entry 포인터 
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfWarmup( void   * aWarmupEntry,
                     smlEnv * aEnv )
{
    gSmfWarmupEntry = (smfWarmupEntry*)aWarmupEntry;

    smrRegisterRedoVectors( SMG_COMPONENT_DATAFILE,
                            gSmfRedoVectors );
    
    smxlRegisterUndoFuncs( SMG_COMPONENT_DATAFILE, 
                           gSmfUndoFuncs );

    STL_TRY( smfRegisterFxTables( aEnv ) == STL_SUCCESS );
    
    smgRegisterPendOp( SMG_PEND_CREATE_TABLESPACE, smfCreateTbsPending );
    smgRegisterPendOp( SMG_PEND_DROP_TABLESPACE, smfDropTbsPending );
    smgRegisterPendOp( SMG_PEND_ADD_DATAFILE, smfAddDatafilePending );
    smgRegisterPendOp( SMG_PEND_DROP_DATAFILE, smfDropDatafilePending );
    smgRegisterPendOp( SMG_PEND_ONLINE_TABLESPACE, smfOnlineTbsPending );
    smgRegisterPendOp( SMG_PEND_OFFLINE_TABLESPACE, smfOfflineTbsPending );
    smgRegisterPendOp( SMG_PEND_CREATE_DATAFILE, smfCreateDatafilePending );
    smgRegisterPendOp( SMG_PEND_RENAME_DATAFILE, smfRenameDatafilePending );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Datafile Component를 Cooldown 시킨다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfCooldown( smlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief Datafile Component를 Bootdown 시킨다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfBootdown( smlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief Datafile Component를 Post Close 시킨다.
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smfShutdownPostClose( smlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief Datafile Component를 Close 시킨다.
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smfShutdownClose( smlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief Datafile Component를 Dismount 시킨다.
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smfShutdownDismount( smlEnv * aEnv )
{
    gSmfWarmupEntry->mTbsInfoArray = gSmfWarmupEntry->mNoMountTbsInfoArray;
    
    return STL_SUCCESS;
}

/**
 * @brief Fixed Table 들을 등록함
 * @param[in]  aEnv   Environment
 * @remarks
 * NO_MOUNT 단계와 Warm-Up 단계에서 동일한 순서로 등록되어야 함.
 */
stlStatus smfRegisterFxTables( smlEnv * aEnv )
{
    STL_TRY( knlRegisterFxTable( &gTablespaceTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );
    
    STL_TRY( knlRegisterFxTable( &gDatafileTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );
    
    STL_TRY( knlRegisterFxTable( &gDatafileHeaderTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );

    STL_TRY( knlRegisterFxTable( &gDiskPageDumpTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );

    STL_TRY( knlRegisterFxTable( &gCtrlSysSectionTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );

    STL_TRY( knlRegisterFxTable( &gCtrlLogSectionTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );
    
    STL_TRY( knlRegisterFxTable( &gCtrlDBSectionTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );

    STL_TRY( knlRegisterFxTable( &gCtrlDatafileTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );

    STL_TRY( knlRegisterFxTable( &gBackupTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );

    STL_TRY( knlRegisterFxTable( &gDumpIncBackupTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );

    STL_TRY( knlRegisterFxTable( &gCtrlIncBackupSectionTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief MOUNT Mode로 전환시 Datafile Component의 유효성을 검사한다.
 * @param[in,out] aEnv       Environment 정보
 */
stlStatus smfValidateMount( smlEnv * aEnv )
{
    /**
     * Validate Control file
     */
    STL_TRY( smfValidateControlfile4Mount( aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
                              

/**
 * @brief OPEN Mode로 전환시 Datafile Component의 유효성을 검사한다.
 * @param[in]  aLogOption    NORESETLOGS/RESETLOGS
 * @param[in]  aLastLsn      last flushed lsn
 * @param[in,out] aEnv       Environment 정보
 */
stlStatus smfValidateOpen( stlUInt8   aLogOption,
                           smrLsn     aLastLsn,
                           smlEnv   * aEnv )
{
    smfTbsPersData        sTbsPersData;
    void                * sTbsIterator;
    smfDatafilePersData   sDatafilePersData;
    void                * sDatafileIterator;
    stlBool               sFileExist;
    stlFileInfo           sFileInfo;
    stlInt32              sState = 0;
    stlFile               sFile;
    smfDatafileHeader     sDatafileHeader;
    stlInt32              sFlag;
    stlInt64              sDatabaseIo;

    /**
     * Validate Controlfile
     */
    STL_TRY( smfValidateControlfile4Open( aLogOption, aEnv ) == STL_SUCCESS );

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
         * 아래 경우는 검증할 필요가 없다.
         * - Aging Tablespace
         * - Memory Temporary Tablespace
         * - OFFLINE Tablespace
         */
        if( (SML_TBS_IS_MEMORY_TEMPORARY( sTbsPersData.mAttr ) == STL_TRUE) ||
            (sTbsPersData.mState == SMF_TBS_STATE_AGING)   ||
            (sTbsPersData.mState == SMF_TBS_STATE_DROPPED) ||
            (sTbsPersData.mState == SMF_TBS_STATE_DROPPING) ||
            (sTbsPersData.mIsOnline == STL_FALSE) )
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

            if( (sDatafilePersData.mState == SMF_DATAFILE_STATE_DROPPING) ||
                (sDatafilePersData.mState == SMF_DATAFILE_STATE_DROPPED)  ||
                (sDatafilePersData.mState == SMF_DATAFILE_STATE_AGING) )
            {
                STL_TRY( smfNextDatafilePersData( sTbsIterator,
                                                  &sDatafilePersData,
                                                  &sDatafileIterator,
                                                  aEnv ) == STL_SUCCESS );
                continue;
            }
            
            /**
             * 1. datafile이 존재하는지 검사
             */
    
            STL_TRY( stlExistFile( sDatafilePersData.mName,
                                   &sFileExist,
                                   KNL_ERROR_STACK(aEnv) )
                     == STL_SUCCESS );
        
            STL_TRY_THROW( sFileExist == STL_TRUE, RAMP_ERR_FILE_NOT_EXIST );
                    
            /**
             * 2. datafile의 Read Write모드 검사
             */
    
            STL_TRY( stlGetFileStatByName( &sFileInfo,
                                           sDatafilePersData.mName,
                                           STL_FINFO_UPROT,
                                           KNL_ERROR_STACK(aEnv) )
                     == STL_SUCCESS );

            STL_TRY_THROW( (sFileInfo.mProtection & (STL_FPERM_UREAD | STL_FPERM_UWRITE)) ==
                           (STL_FPERM_UREAD | STL_FPERM_UWRITE),
                           RAMP_ERR_FILE_IS_READ_ONLY );
            
            /**
             * 3. datafile header의 유효성 검사
             */

            sFlag = STL_FOPEN_READ | STL_FOPEN_LARGEFILE;
        
            sDatabaseIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_FILE_IO,
                                                         KNL_ENV(aEnv) );

            if( sDatabaseIo == 0 )
            {
                sFlag |= STL_FOPEN_DIRECT;
            }

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

            STL_TRY_THROW( (sDatafileHeader.mTbsPhysicalId == SMF_INVALID_PHYSICAL_TBS_ID) ||
                           (sDatafileHeader.mTbsPhysicalId == sTbsPersData.mTbsId),
                           RAMP_ERR_INVALID_DATAFILE );
            STL_TRY_THROW( sDatafileHeader.mDatafileId == sDatafilePersData.mID,
                           RAMP_ERR_INVALID_DATAFILE );

            /**
             * Datafile의 checkpoint Lsn이 최신 checkpoint Lsn이
             * 아닌 경우 Media Recovery가 필요하다.
             */
            STL_TRY_THROW( ((sDatafileHeader.mCheckpointLsn) >= smrGetLastCheckpointLsn()) ||
                           (smfGetRestoredLsn(sDatafileIterator) >= smrGetLastCheckpointLsn()),
                           RAMP_ERR_NEED_MEDIA_RECOVERY );

            /**
             * Log file과 Datafile의 Lsn 검사.
             */
            STL_TRY_THROW( sDatafileHeader.mCheckpointLsn <= aLastLsn,
                           RAMP_ERR_DATAFILE_MORE_RECENT );

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
    
    STL_CATCH( RAMP_ERR_FILE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_FILE_NOT_EXIST,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sDatafilePersData.mName );
    }
    
    STL_CATCH( RAMP_ERR_FILE_IS_READ_ONLY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_FILE_IS_READ_ONLY,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sDatafilePersData.mName );
    }
    
    STL_CATCH( RAMP_ERR_INVALID_DATAFILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INVALID_DATAFILE,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sDatafilePersData.mName );
    }

    STL_CATCH( RAMP_ERR_NEED_MEDIA_RECOVERY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_NEED_MEDIA_RECOVERY,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sTbsPersData.mName );
    }

    STL_CATCH( RAMP_ERR_DATAFILE_MORE_RECENT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_DATAFILE_RECENT_THAN_REDO_LOGFILE,
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
 * @brief SM Layer에서 사용되는 database file을 register한다.
 * @param[out] aEnv Environment 구조체
 */
stlStatus smfRegisterDbFiles( smlEnv * aEnv )
{
    stlInt32              i;
    smfTbsPersData        sTbsPersData;
    void                * sTbsIterator;
    smfDatafilePersData   sDatafilePersData;
    void                * sDatafileIterator;
    stlChar               sTmpFileName[SMF_CTRL_FILE_PATH_MAX_LENGTH];

    /* control files */
    for( i = 0; i < gSmfWarmupEntry->mCtrlFileCnt; i++ )
    {
        STL_TRY( knlAddDbFile( gSmfWarmupEntry->mCtrlFileName[i],
                               KNL_DATABASE_FILE_TYPE_CONTROL,
                               KNL_ENV(aEnv) ) == STL_SUCCESS );
    }

    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_CONTROL_FILE_TEMP_NAME,
                                      sTmpFileName,
                                      KNL_ENV( aEnv ) ) == STL_SUCCESS );

    /* temporary control file */
    STL_TRY( knlAddDbFile( sTmpFileName,
                           KNL_DATABASE_FILE_TYPE_TEMP,
                           KNL_ENV(aEnv) ) == STL_SUCCESS );

    /* datafiles */
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
         * 아래 경우는 등록하지 않는다.
         * - Aging Tablespace
         * - Memory Temporary Tablespace
         */
        if( (SML_TBS_IS_MEMORY_TEMPORARY( sTbsPersData.mAttr ) == STL_TRUE) ||
            (sTbsPersData.mState == SMF_TBS_STATE_AGING)   ||
            (sTbsPersData.mState == SMF_TBS_STATE_DROPPED) ||
            (sTbsPersData.mState == SMF_TBS_STATE_DROPPING) )
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

            if( (sDatafilePersData.mState == SMF_DATAFILE_STATE_DROPPING) ||
                (sDatafilePersData.mState == SMF_DATAFILE_STATE_DROPPED)  ||
                (sDatafilePersData.mState == SMF_DATAFILE_STATE_AGING) )
            {
                STL_TRY( smfNextDatafilePersData( sTbsIterator,
                                                  &sDatafilePersData,
                                                  &sDatafileIterator,
                                                  aEnv ) == STL_SUCCESS );
                continue;
            }

            STL_TRY( knlAddDbFile( sDatafilePersData.mName,
                                   KNL_DATABASE_FILE_TYPE_DATAFILE,
                                   KNL_ENV(aEnv) )
                     == STL_SUCCESS );

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

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
