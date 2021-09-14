/*******************************************************************************
 * smfBackupSegment.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smfBackupSegment.c 
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
#include <smg.h>
#include <smxl.h>
#include <smrDef.h>
#include <smr.h>
#include <smlGeneral.h>
#include <smfCtrlFileBuffer.h>
#include <smfCtrlFileMgr.h>
#include <smfBackupSegment.h>

/**
 * @file smfBackupSegment.c
 * @brief Storage Manager Layer Backup Segment Internal Routines
 */

/**
 * @addtogroup smf
 * @{
 */

/**
 * @brief Incremental Backup을 WarmupEntry에 추가 후 Control File에 기록한다.
 * @param[in]     aBackupFileName   Backup file Name
 * @param[in]     aBeginTime        Begin Backup timestamp 
 * @param[in]     aTbsId            Tablespace Id
 * @param[in]     aBackupObjectType Backup Object(DATABASE / TABLESPACE / CONTROLFILE)
 * @param[in]     aBackupOption     Backup Option( CUMULATIVE / DIFFERENTIAL )
 * @param[in]     aLevel            Incremental Backup Level
 * @param[in]     aBackupLsn        Backup된 page의 최신 Lsn
 * @param[in]     aBackupLid        Backup된 page의 최신 Lid
 * @param[in,out] aEnv              Environment 정보
 */
stlStatus smfInsertBackupRecord( stlChar  * aBackupFileName,
                                 stlTime    aBeginTime,
                                 smlTbsId   aTbsId,
                                 stlUInt8   aBackupObjectType,
                                 stlUInt8   aBackupOption,
                                 stlInt16   aLevel,
                                 smrLsn     aBackupLsn,
                                 smrLid     aBackupLid,
                                 smlEnv   * aEnv )
{
    stlFile             sFile;
    stlInt32            sFlag;
    stlInt64            sSysFileIo;
    stlBool             sDeleteFile;
    stlSize             sFileNameSize;
    smfBackupChunk    * sBackupChunk;
    smfBackupRecord   * sBackupRecord;
    stlBool             sIsTimeOut;
    stlInt32            sState = 0;
    stlInt64            sCtrlFileCnt;
    stlInt64            i;
    stlTime             sCompletionTime; 

    sCtrlFileCnt = smfGetCtrlFileCount();
    
    STL_TRY( knlAcquireLatch( &gSmfWarmupEntry->mLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimeOut,
                              KNL_ENV(aEnv) ) == STL_SUCCESS );
    sState = 1;
    
    sFileNameSize = stlStrlen( aBackupFileName ) + 1;

    if( aBackupObjectType != SMF_INCREMENTAL_BACKUP_OBJECT_TYPE_CONTROLFILE )
    {
        smfSetPrevBackupInfo( aTbsId,
                              aLevel,
                              aBackupLsn );
    }

    /**
     * Backup list를 추가하기 위해 current backup chunk를 가져온다.
     */
    sBackupChunk = smfGetCurrBackupChunk( );

    if( sBackupChunk == NULL )
    {
        STL_TRY( smfAllocBackupChunk( aEnv ) == STL_SUCCESS );

        sBackupChunk = smfGetCurrBackupChunk( );
    }
    
    /**
     * Current backup chunk에서 backup record를 기록할 slot을 할당받는다.
     */
    smfAllocBackupSlot( sBackupChunk,
                        sFileNameSize,
                        (void **)&sBackupRecord );

    if( sBackupRecord == NULL )
    {
        STL_TRY( smfAllocBackupChunk( aEnv ) == STL_SUCCESS );

        sBackupChunk = smfGetCurrBackupChunk( );
        
        smfAllocBackupSlot( sBackupChunk,
                            sFileNameSize,
                            (void **)&sBackupRecord );
        
        /**
         * Backup chunk 추가 후 slot을 할당은 반드시 성공해야 한다.
         */
        STL_DASSERT( sBackupRecord != NULL );
    }

    /**
     * 할당 받은 backup slot에 backup record를 기록한다.
     */
    sCompletionTime = stlNow();
    
    smfWriteBackupRecord( sBackupRecord,
                          sBackupChunk,
                          aBackupFileName,
                          sFileNameSize,
                          aBeginTime,
                          sCompletionTime,
                          aBackupLsn,
                          aBackupLid,
                          aTbsId,
                          aLevel,
                          aBackupObjectType,
                          aBackupOption );

    sFlag = STL_FOPEN_WRITE | STL_FOPEN_READ;

    sSysFileIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_SYSTEM_FILE_IO,
                                                KNL_ENV(aEnv) );

    if( sSysFileIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

    for( i = 0; i < sCtrlFileCnt; i++ )
    {
        STL_TRY( smgOpenFile( &sFile,
                              gSmfWarmupEntry->mCtrlFileName[i],
                              sFlag,
                              STL_FPERM_OS_DEFAULT,
                              aEnv ) == STL_SUCCESS );
        sState = 2;
        
        STL_TRY( smfSaveCtrlFileInternal( &sFile,
                                          aEnv ) == STL_SUCCESS );
        
        sState = 1;
        STL_TRY( smgCloseFile( &sFile,
                               aEnv ) == STL_SUCCESS );
    }
    
    sState = 0;
    STL_TRY( knlReleaseLatch( &gSmfWarmupEntry->mLatch,
                              (knlEnv*)aEnv ) == STL_SUCCESS );

    /**
     * Database Level 0 Backup의 경우,
     * DEFAULT_REMOVAL_OBSOLETE_BACKUP_LIST property가 TRUE이면
     * controlfile에서 obsolete backup list를 삭제한다.
     */
    if( (aLevel == 0) &&
        (aBackupObjectType == SMF_INCREMENTAL_BACKUP_OBJECT_TYPE_DATABASE) )
    {
        if( knlGetPropertyBoolValueByID(
                KNL_PROPERTY_DEFAULT_REMOVAL_OBSOLETE_BACKUP_LIST,
                KNL_ENV( aEnv ) ) )
        {
            /**
             * DEFAULT_REMOVAL_BACKUP_FILE property가 TRUE이면
             * obsolete backup list 삭제 시 backup file도 함께 삭제한다.
             */
            sDeleteFile = knlGetPropertyBoolValueByID(
                KNL_PROPERTY_DEFAULT_REMOVAL_BACKUP_FILE,
                KNL_ENV( aEnv ) );

            STL_TRY( smfDeleteBackupList( SML_DELETE_OBSOLETE_BACKUP_LIST,
                                          sDeleteFile,
                                          aEnv ) == STL_SUCCESS );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)smgCloseFile( &sFile, aEnv );
        case 1:
            (void)knlReleaseLatch( &gSmfWarmupEntry->mLatch, (knlEnv*)aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}

/**
 * @brief BackupSegment에 기록된 첫번째 Backup Record를 꺼내온다.
 * @param[in]  aBackupSegment   Backup Segment
 * @param[out] aBackupChunk     Backup Chunk
 * @param[out] aBackupIterator  Backup Iterator
 * @param[out] aExistBackup     Backup Record의 존재 여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfGetFirstIncBackup( smfBackupSegment  * aBackupSegment,
                                smfBackupChunk   ** aBackupChunk,
                                smfBackupIterator * aBackupIterator,
                                stlBool           * aExistBackup,
                                smlEnv            * aEnv )
{
    smfBackupChunk    * sBackupChunk  = NULL;
    smfBackupChunkHdr * sBackupChunkHdr = NULL;
    stlInt64            sBackupDirCount;
    stlInt32            i;
    stlBool             sExistFile;

    *aExistBackup = STL_FALSE;
    
    sBackupChunk  = smfGetFirstBackupChunk( aBackupSegment );

    STL_TRY_THROW( sBackupChunk != NULL, RAMP_SKIP );
    
    sBackupChunkHdr = (smfBackupChunkHdr *)sBackupChunk->mChunk;

    aBackupIterator->mBackupCntInChunk = sBackupChunkHdr->mBackupCnt;
    aBackupIterator->mBackupChunkSeq  = 0;
    aBackupIterator->mBackupSlotSeq   = 0;

    stlMemcpy( (stlChar *)&(aBackupIterator->mBackupRecord),
               SMF_GET_BACKUP_RECORD( sBackupChunkHdr, aBackupIterator ),
               STL_SIZEOF(smfBackupRecord) );

    /* Backup의 절대경로를 포함하여 backup file의 존재유무 확인 */
    if( aBackupIterator->mNeedAbsBackupFilePath == STL_TRUE )
    {
        /* Incremental Backup directory의 수를 구한다. */
        sBackupDirCount = knlGetPropertyBigIntValueByID(
            KNL_PROPERTY_READABLE_BACKUP_DIR_COUNT,
            KNL_ENV(aEnv) );

        for( i = 0; i < sBackupDirCount; i++ )
        {
            STL_TRY( smfGetAbsBackupFileName( SMF_GET_BACKUP_FILE_NAME( sBackupChunkHdr, aBackupIterator ),
                                              i,
                                              aBackupIterator->mFileName,
                                              aEnv )
                     == STL_SUCCESS );

            STL_TRY( stlExistFile( aBackupIterator->mFileName,
                                   &sExistFile,
                                   KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

            if( sExistFile == STL_TRUE )
            {
                break;
            }
        }

        if( sExistFile != STL_TRUE )
        {
            STL_TRY( knlLogMsg( NULL,
                                KNL_ENV( aEnv ),
                                KNL_LOG_LEVEL_INFO,
                                "Incremental backup file does not exist( %s )\n",
                                aBackupIterator->mFileName )
                     == STL_SUCCESS );

            STL_THROW( RAMP_SKIP );
        }
    }
    else
    {
        stlStrncpy( aBackupIterator->mFileName,
                    SMF_GET_BACKUP_FILE_NAME( sBackupChunkHdr, aBackupIterator ),
                    aBackupIterator->mBackupRecord.mNameLength );
    }

    aBackupIterator->mBackupSlotSeq++;

    *aBackupChunk = sBackupChunk;
    *aExistBackup = STL_TRUE;

    STL_RAMP( RAMP_SKIP );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief BackupSegment에 기록된 Backup Record를 하나씩 꺼내온다.
 * @param[in]     aBackupSegment   Backup Segment
 * @param[in,out] aBackupChunk     Backup Chunk
 * @param[out]    aBackupIterator  Backup Iterator
 * @param[out]    aExistBackup     Backup List의 존재 여부
 * @param[in,out] aEnv             Environment 정보
 */
stlStatus smfGetNextIncBackup( smfBackupSegment  * aBackupSegment,
                               smfBackupChunk   ** aBackupChunk,
                               smfBackupIterator * aBackupIterator,
                               stlBool           * aExistBackup,
                               smlEnv            * aEnv )
{
    smfBackupChunk    * sBackupChunk = NULL;
    smfBackupChunkHdr * sBackupChunkHdr;
    stlInt64            sBackupDirCount;
    stlInt32            i;
    stlBool             sExistFile;

    *aExistBackup = STL_FALSE;

    /**
     * Parameter의 Chunk를 계속 이용할지 확인.
     */ 
    sBackupChunkHdr = (smfBackupChunkHdr *)((*aBackupChunk)->mChunk);
    
    if( aBackupIterator->mBackupSlotSeq >= sBackupChunkHdr->mBackupCnt )
    {
        sBackupChunk  = smfGetNextBackupChunk( aBackupSegment,
                                               *aBackupChunk );
        *aBackupChunk = sBackupChunk;

        aBackupIterator->mBackupChunkSeq++;
        aBackupIterator->mBackupSlotSeq = 0;
    }
    else
    {
        sBackupChunk = *aBackupChunk;
    }

    STL_TRY_THROW( sBackupChunk != NULL, RAMP_SKIP );

    sBackupChunkHdr = (smfBackupChunkHdr *) sBackupChunk->mChunk;
    
    aBackupIterator->mBackupCntInChunk = sBackupChunkHdr->mBackupCnt;

    stlMemcpy( (stlChar *)&(aBackupIterator->mBackupRecord),
               SMF_GET_BACKUP_RECORD( sBackupChunkHdr, aBackupIterator ),
               STL_SIZEOF(smfBackupRecord) );

    /* Backup의 절대경로를 포함하여 backup file의 존재유무 확인 */
    if( aBackupIterator->mNeedAbsBackupFilePath == STL_TRUE )
    {
        /* Incremental Backup directory의 수를 구한다. */
        sBackupDirCount = knlGetPropertyBigIntValueByID(
            KNL_PROPERTY_READABLE_BACKUP_DIR_COUNT,
            KNL_ENV(aEnv) );

        for( i = 0; i < sBackupDirCount; i++ )
        {
            STL_TRY( smfGetAbsBackupFileName( SMF_GET_BACKUP_FILE_NAME( sBackupChunkHdr, aBackupIterator ),
                                              i,
                                              aBackupIterator->mFileName,
                                              aEnv )
                     == STL_SUCCESS );

            STL_TRY( stlExistFile( aBackupIterator->mFileName,
                                   &sExistFile,
                                   KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

            if( sExistFile == STL_TRUE )
            {
                break;
            }
        }

        if( sExistFile != STL_TRUE )
        {
            STL_TRY( knlLogMsg( NULL,
                                KNL_ENV( aEnv ),
                                KNL_LOG_LEVEL_INFO,
                                "Incremental backup file does not exist( %s )\n",
                                aBackupIterator->mFileName )
                     == STL_SUCCESS );

            STL_THROW( RAMP_SKIP );
        }
    }
    else
    {
        stlStrncpy( aBackupIterator->mFileName,
                    SMF_GET_BACKUP_FILE_NAME( sBackupChunkHdr, aBackupIterator ),
                    aBackupIterator->mBackupRecord.mNameLength );
    }

    aBackupIterator->mBackupSlotSeq++;

    *aExistBackup = STL_TRUE;

    STL_RAMP( RAMP_SKIP );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Controlfile에 기록된 첫번째 Backup Record를 꺼내온다.
 * @param[in] aFile Controlfile의 file pointer
 * @param[in] aSysPersData System Persistent Data
 * @param[out] aBackupIterator Backup Iterator
 * @param[out] aBackupChunk Control file에서 Backup Chunk를 읽을 버퍼
 * @param[out] aExistBackup Backup List의 존재 여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfGetFirstIncBackup4Disk( stlFile           * aFile,
                                     smfSysPersData    * aSysPersData,
                                     smfBackupIterator * aBackupIterator,
                                     stlChar           * aBackupChunk,
                                     stlBool           * aExistBackup,
                                     smlEnv            * aEnv )
{
    stlInt32            sReadSize;
    smfBackupChunkHdr * sBackupChunkHdr;
    stlOffset           sOffset;
    smfCtrlBuffer       sBuffer;
    stlInt64            sBackupDirCount;
    stlInt32            i;
    stlBool             sExistFile = STL_FALSE;
    
    *aExistBackup = STL_FALSE;

    /**
     * Systemp Persistent Data에서 Chunk의 개수가 0이면 Skip
     */ 
    STL_TRY_THROW( aSysPersData->mBackupChunkCnt > 0, RAMP_SKIP );

    SMF_INIT_CTRL_BUFFER( &sBuffer );

    sBackupChunkHdr = (smfBackupChunkHdr *) aBackupChunk;
    
    sOffset = aSysPersData->mIncBackupSectionOffset;

    STL_TRY( stlSeekFile( aFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    sReadSize = SMF_BACKUP_CHUNK_SIZE;

    STL_TRY( smfReadCtrlFile( aFile,
                              &sBuffer,
                              (stlChar*)sBackupChunkHdr,
                              sReadSize,
                              aEnv ) == STL_SUCCESS );

    /**
     * Backup Chunk를 읽고 Backup Count는 0이면 안된다.
     */ 
    STL_DASSERT( sBackupChunkHdr->mBackupCnt != 0 );

    aBackupIterator->mBackupCntInChunk = sBackupChunkHdr->mBackupCnt;
    aBackupIterator->mBackupChunkSeq  = 0;
    aBackupIterator->mBackupSlotSeq   = 0;

    stlMemcpy( (stlChar *)&(aBackupIterator->mBackupRecord),
               SMF_GET_BACKUP_RECORD( sBackupChunkHdr, aBackupIterator ),
               STL_SIZEOF(smfBackupRecord) );

    if( aBackupIterator->mNeedAbsBackupFilePath == STL_TRUE )
    {
        sBackupDirCount = knlGetPropertyBigIntValueByID(
            KNL_PROPERTY_READABLE_BACKUP_DIR_COUNT,
            KNL_ENV(aEnv) );

        for( i = 0; i < sBackupDirCount; i++ )
        {
            STL_TRY( smfGetAbsBackupFileName( SMF_GET_BACKUP_FILE_NAME( sBackupChunkHdr, aBackupIterator ),
                                              i,
                                              aBackupIterator->mFileName,
                                              aEnv )
                     == STL_SUCCESS );

            STL_TRY( stlExistFile( aBackupIterator->mFileName,
                                   &sExistFile,
                                   KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

            if( sExistFile == STL_TRUE )
            {
                break;
            }
        }
    }
    else
    {
        stlStrncpy( aBackupIterator->mFileName,
                    SMF_GET_BACKUP_FILE_NAME( sBackupChunkHdr, aBackupIterator ),
                    aBackupIterator->mBackupRecord.mNameLength );
    }
    
    aBackupIterator->mBackupSlotSeq++;

    *aExistBackup = STL_TRUE;

    STL_RAMP( RAMP_SKIP );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Controlfile에 기록된 Backup Record를 하나씩 꺼내온다.
 * @param[in] aFile Controlfile의 file pointer
 * @param[in] aSysPersData System Persistent Data
 * @param[out] aBackupIterator Backup Iterator
 * @param[in,out] aBackupChunk Backup Chunk
 * @param[out] aExistBackup Backup List의 존재 여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfGetNextIncBackup4Disk( stlFile           * aFile,
                                    smfSysPersData    * aSysPersData,
                                    smfBackupIterator * aBackupIterator,
                                    stlChar           * aBackupChunk,
                                    stlBool           * aExistBackup,
                                    smlEnv            * aEnv )
{
    stlInt32            sReadSize;
    smfBackupChunkHdr * sBackupChunkHdr;
    stlOffset           sOffset;
    smfCtrlBuffer       sBuffer;
    stlInt64            sBackupDirCount;
    stlInt32            i;
    stlBool             sExistFile = STL_FALSE;
    
    *aExistBackup = STL_FALSE;

    /**
     * Backup Chunk에서 Backup 정보를 얻을지 Control File을 읽을지 판단한다. 
     */
    sBackupChunkHdr = (smfBackupChunkHdr *) aBackupChunk;
    
    if( sBackupChunkHdr->mBackupCnt <= aBackupIterator->mBackupSlotSeq )
    {
        aBackupIterator->mBackupChunkSeq++;
        aBackupIterator->mBackupSlotSeq = 0;
        
        /**
         * Control File을 읽어야 하고, Chunk 개수로 판단 
         */
        STL_TRY_THROW( aSysPersData->mBackupChunkCnt > aBackupIterator->mBackupChunkSeq,
                       RAMP_SKIP );

        SMF_INIT_CTRL_BUFFER( &sBuffer );

        sOffset = aSysPersData->mIncBackupSectionOffset +
            SMF_BACKUP_CHUNK_SIZE * aBackupIterator->mBackupChunkSeq;
    
        STL_TRY( stlSeekFile( aFile,
                              STL_FSEEK_SET,
                              &sOffset,
                              KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

        sReadSize = SMF_BACKUP_CHUNK_SIZE;

        STL_TRY( smfReadCtrlFile( aFile,
                                  &sBuffer,
                                  (stlChar*)sBackupChunkHdr,
                                  sReadSize,
                                  aEnv ) == STL_SUCCESS );

        aBackupIterator->mBackupCntInChunk = sBackupChunkHdr->mBackupCnt;
    }

    stlMemcpy( (stlChar *)&(aBackupIterator->mBackupRecord),
               SMF_GET_BACKUP_RECORD( sBackupChunkHdr, aBackupIterator ),
               STL_SIZEOF(smfBackupRecord) );

    if( aBackupIterator->mNeedAbsBackupFilePath == STL_TRUE )
    {
        sBackupDirCount = knlGetPropertyBigIntValueByID(
            KNL_PROPERTY_READABLE_BACKUP_DIR_COUNT,
            KNL_ENV(aEnv) );

        for( i = 0; i < sBackupDirCount; i++ )
        {
            STL_TRY( smfGetAbsBackupFileName( SMF_GET_BACKUP_FILE_NAME( sBackupChunkHdr, aBackupIterator ),
                                              i,
                                              aBackupIterator->mFileName,
                                              aEnv )
                     == STL_SUCCESS );

            STL_TRY( stlExistFile( aBackupIterator->mFileName,
                                   &sExistFile,
                                   KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

            if( sExistFile == STL_TRUE )
            {
                break;
            }
        }
    }
    else
    {
        stlStrncpy( aBackupIterator->mFileName,
                    SMF_GET_BACKUP_FILE_NAME( sBackupChunkHdr, aBackupIterator ),
                    aBackupIterator->mBackupRecord.mNameLength );
    }
    
    aBackupIterator->mBackupSlotSeq++;

    *aExistBackup = STL_TRUE;
    
    STL_RAMP( RAMP_SKIP );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ALTER DATABASE DELETE ...  BACKUP 을 수행한다.
 * @param[in] aTarget 삭제할 Backup List 범위(ALL or OBSOLETE)
 * @param[in] aDeleteFile Backup file을 함께 삭제할지 여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfDeleteBackupList( stlUInt8    aTarget,
                               stlBool     aDeleteFile,
                               smlEnv    * aEnv )
{
    stlFile             sFile;
    stlInt32            sFlag;
    stlInt64            sSysFileIo;
    stlInt32            sState = 0;
    smfSysPersData    * sSysPersData;
    smfSysPersData      sTmpSysPersData;
    stlInt64            sFirstChunkId;
    stlInt32            sFirstBackupRecordId;
    stlBool             sIsTimeOut;
    knlRegionMark       sMemMark;
    stlInt64            sCtrlFileCnt;
    stlInt64            i;
    stlFile             sBackupFile;
    stlChar             sBackupFileName[SMF_CTRL_FILE_PATH_MAX_LENGTH];
    stlBool             sBackupFileOpened = STL_FALSE;
    stlSize             sWrittenBytes;

    sSysPersData    = &gSmfWarmupEntry->mSysPersData;

    STL_TRY_THROW( sSysPersData->mBackupChunkCnt > 0, RAMP_SKIP );

    sCtrlFileCnt = smfGetCtrlFileCount();
    
    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_CONTROL_FILE_TEMP_NAME,
                                      sBackupFileName,
                                      KNL_ENV( aEnv ) ) == STL_SUCCESS );

    sSysFileIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_SYSTEM_FILE_IO,
                                                KNL_ENV(aEnv) );
    
    KNL_INIT_REGION_MARK( &sMemMark );
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               KNL_ENV(aEnv) ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( knlAcquireLatch( &gSmfWarmupEntry->mLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimeOut,
                              KNL_ENV(aEnv) ) == STL_SUCCESS );
    sState = 2;

    /**
     * System & Backup Section을 Temp file에 기록한다.
     */
    sFlag = STL_FOPEN_CREATE | STL_FOPEN_TRUNCATE | STL_FOPEN_WRITE | STL_FOPEN_READ;

    if( sSysFileIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

    STL_TRY( smgOpenFile( &sBackupFile,
                          sBackupFileName,
                          sFlag,
                          STL_FPERM_OS_DEFAULT,
                          aEnv ) == STL_SUCCESS );
    sBackupFileOpened = STL_TRUE;
    
    STL_TRY( smfWriteSysCtrlSection( &sBackupFile,
                                     &sWrittenBytes,
                                     aEnv ) == STL_SUCCESS );
    
    STL_TRY( smfWriteBackupCtrlSection( &sBackupFile,
                                        &sWrittenBytes,
                                        aEnv ) == STL_SUCCESS );

    stlMemcpy( &sTmpSysPersData, sSysPersData, STL_SIZEOF(smfSysPersData) );

    /**
     * WarmupEntry의 Backup Stream을 정리한다.
     */ 
    if( aTarget == SML_DELETE_ALL_BACKUP_LIST )
    {
        STL_TRY( smfDeleteAllBackupList( aEnv ) == STL_SUCCESS );

        sFirstChunkId        = sSysPersData->mBackupChunkCnt - 1;
        sFirstBackupRecordId = STL_INT32_MAX;
    }
    else
    {
        STL_TRY( smfDeleteObsoleteBackupList( &sFirstChunkId,
                                              &sFirstBackupRecordId,
                                              aEnv ) == STL_SUCCESS );
    }

    /**
     * Option에 따라서 Backup file을 물리적으로 삭제한다.
     */ 
    if( aDeleteFile == STL_TRUE )
    {
        STL_TRY( smfDeleteBackupFile( &sBackupFile,
                                      &sTmpSysPersData,
                                      sFirstChunkId,
                                      sFirstBackupRecordId,
                                      aEnv )
                 == STL_SUCCESS );
    }

    sBackupFileOpened = STL_FALSE;
    STL_TRY( smgCloseFile( &sBackupFile, aEnv ) == STL_SUCCESS );
    
    sFlag = STL_FOPEN_WRITE | STL_FOPEN_READ;

    if( sSysFileIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

    for( i = 0; i < sCtrlFileCnt; i++ )
    {
        STL_TRY( smgOpenFile( &sFile,
                              gSmfWarmupEntry->mCtrlFileName[i],
                              sFlag,
                              STL_FPERM_OS_DEFAULT,
                              aEnv ) == STL_SUCCESS );
        sState = 3;
        
        STL_TRY( smfSaveCtrlFileInternal( &sFile,
                                          aEnv ) == STL_SUCCESS );
        
        sState = 2;
        STL_TRY( smgCloseFile( &sFile,
                               aEnv ) == STL_SUCCESS );
    }
    
    sState = 1;
    STL_TRY( knlReleaseLatch( &gSmfWarmupEntry->mLatch,
                              (knlEnv*)aEnv ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_RAMP( RAMP_SKIP );

    return STL_SUCCESS;

    STL_FINISH;

    if( sBackupFileOpened == STL_TRUE )
    {
        (void) smgCloseFile( &sBackupFile, aEnv );
    }

    switch( sState )
    {
        case 3:
            (void)smgCloseFile( &sFile, aEnv );
        case 2:
            (void)knlReleaseLatch( &gSmfWarmupEntry->mLatch, (knlEnv*)aEnv );
        case 1:
            (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                            &sMemMark,
                                            STL_FALSE, /* aFreeChunk */
                                            KNL_ENV(aEnv) );
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief Backup Segment에서 모든 Backup Chunk를 지운다.
 * @param[in,out] aEnv envrionemnt info
 */ 
stlStatus smfDeleteAllBackupList( smlEnv  * aEnv )
{
    smlTbsId            sTbsId;
    stlBool             sTbsExist;
    stlInt32            i;
    smfIncBackupInfo  * sIncBackup;
    smfSysPersData    * sSysPersData;

    /**
     * WarmupEntry의 Backup Stream에 연결된 Backup Segment를 제거한다.
     */
    sSysPersData = &gSmfWarmupEntry->mSysPersData;

    STL_TRY( smfTruncateBackupSegment( sSysPersData->mBackupChunkCnt,
                                       aEnv ) == STL_SUCCESS );

    STL_DASSERT( sSysPersData->mBackupChunkCnt == 0 );

    SMF_INIT_BACKUP_SEGMENT( &gSmfWarmupEntry->mBackupSegment );

    /**
     * DbPersData의 Backup info 재설정.
     */
    sIncBackup = &(gSmfWarmupEntry->mDbPersData.mIncBackup);
    
    for( i = 0; i < SML_INCREMENTAL_BACKUP_LEVEL_MAX; i++ )
    {
        sIncBackup->mBackupLsn[i] = SMR_INVALID_LSN;
    }

    /**
     * Tablespace Incremental Backup Info 초기화
     */
    STL_TRY( smfFirstTbs( &sTbsId,
                          &sTbsExist,
                          aEnv ) == STL_SUCCESS );

    while( sTbsExist == STL_TRUE )
    {
        smfInitIncrementalBackupInfo( sTbsId );

        STL_TRY( smfNextTbs( &sTbsId,
                             &sTbsExist,
                             aEnv ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

   STL_FINISH;

   return STL_FAILURE;
}

/** @} */

/**
 * @addtogroup smfBackupSegment
 * @{
 */

/**
 * @brief Backup section에서 Obsolete한 Backup Record를 지운다.
 * @param[out] aEndChunkId  첫번째 Valid한 backup이 저장된 segment Id
 * @param[out] aEndRecordId 첫번째 Valid한 backup이 저장된 segment 내의 slot Id
 * @param[in,out] aEnv   envrionemnt info
 * @remark Latest Level0 Backup이 속한 Chunk에 Obsolete Backup Record가 남아 있을수 있다.
 */ 
stlStatus smfDeleteObsoleteBackupList( stlInt64 * aEndChunkId,
                                       stlInt32 * aEndRecordId,
                                       smlEnv   * aEnv )
{
    smfBackupSegment  * sBackupSegment;
    smfBackupChunk    * sBackupChunk    = NULL;
    smfBackupRecord   * sBackupRecord   = NULL;
    stlInt64            sLatestChunkId;
    stlInt32            sLatestSlotId;
    stlInt64            i;
    
    /**
     * OBSOLETE BACKUP LIST를 삭제하기 위해 Lastet Level 0 Database Backup의 정보를 구한다.
     */
    STL_TRY( smfFindLatestLevel0Backup( &sLatestChunkId,
                                        &sLatestSlotId,
                                        aEnv ) == STL_SUCCESS );

    STL_TRY_THROW( (sLatestChunkId >= 0) && (sLatestSlotId > 0), STL_RAMP_SKIP_COMPACT );

    sBackupSegment  = &gSmfWarmupEntry->mBackupSegment;
        
    STL_TRY( smfTruncateBackupSegment( sLatestChunkId,
                                       aEnv ) == STL_SUCCESS );


    sBackupChunk = smfGetFirstBackupChunk( sBackupSegment );

    /**
     * Chunk가 해제되고 Chunk에 남아 있는 Obsolete한 Backup에 대해 Obsolete 설정을 한다.
     */ 
    for( i = 0; i < sLatestSlotId; i++ )
    {
        sBackupRecord = (smfBackupRecord*)( sBackupChunk->mChunk + STL_SIZEOF(smfBackupChunkHdr) + ( STL_SIZEOF(smfBackupRecord) * i ) );
        
        sBackupRecord->mIsDeleted = STL_TRUE;
    }
    
    STL_RAMP( STL_RAMP_SKIP_COMPACT );

    *aEndChunkId  = sLatestChunkId;
    *aEndRecordId = sLatestSlotId;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Backup Stream에 Backup Chunk를 추가한다.
 * @param[in,out] aEnv Environment info
 */
stlStatus smfAllocBackupChunk( smlEnv * aEnv )
{
    smfSysPersData    * sSysPersData;
    smfBackupSegment  * sBackupSegment;
    smfBackupChunk    * sBackupChunk = NULL; 
    smfBackupChunkHdr   sBackupChunkHdr;

    sSysPersData    = &gSmfWarmupEntry->mSysPersData;
    sBackupSegment  = &gSmfWarmupEntry->mBackupSegment;

    /**
     * Backup Chunk 하나를 할당하고 WarmupEntry의 Backup Segment에 추가한다.
     */ 
    STL_TRY( smgAllocShmMem4Mount( STL_SIZEOF( smfBackupChunk ),
                                   (void**)&sBackupChunk,
                                   aEnv ) == STL_SUCCESS );

    SMF_INIT_BACKUP_CHUNK( sBackupChunk );

    /**
     * Backup Record를 기록할 Chunk 하나를 초기화한다.
     */
    stlMemset( &sBackupChunkHdr, 0x00, STL_SIZEOF( smfBackupChunkHdr ) );
    
    sBackupChunkHdr.mBackupCnt = 0;
    sBackupChunkHdr.mHighWaterMark = STL_SIZEOF( smfBackupChunkHdr );
    sBackupChunkHdr.mLowWaterMark = SMF_BACKUP_CHUNK_SIZE - 1;

    stlMemcpy( sBackupChunk->mChunk, (stlChar *)&sBackupChunkHdr, STL_SIZEOF(smfBackupChunkHdr) );

    STL_RING_ADD_LAST( &sBackupSegment->mBackupChunkList, sBackupChunk );

    sBackupSegment->mCurBackupChunk = sBackupChunk;

    sSysPersData->mBackupChunkCnt++;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Backup Segment의 Backup Chunk를 삭제한다.
 * @param[in]     aDeleteCnt 삭제하는 Segment count
 * @param[in,out] aEnv     Environment Info
 */
stlStatus smfTruncateBackupSegment( stlInt64   aDeleteCnt,
                                    smlEnv   * aEnv )
{
    smfSysPersData    * sSysPersData;
    smfBackupSegment  * sBackupSegment;
    smfBackupChunk    * sBackupChunk = NULL;
    smfBackupChunk    * sPrevChunk   = NULL;
    stlInt64            i;

    sSysPersData    = &gSmfWarmupEntry->mSysPersData;
    sBackupSegment  = &gSmfWarmupEntry->mBackupSegment;

    for( i = 0; i < aDeleteCnt; i++, sSysPersData->mBackupChunkCnt-- )
    {
        if( sBackupChunk == NULL )
        {
            sBackupChunk = (smfBackupChunk *) STL_RING_GET_FIRST_DATA( &sBackupSegment->mBackupChunkList );
        }
        else
        {
            sPrevChunk   = sBackupChunk;
            sBackupChunk = (smfBackupChunk *) STL_RING_GET_NEXT_DATA( &sBackupSegment->mBackupChunkList, sBackupChunk );
        }
        
        STL_RING_UNLINK( &sBackupSegment->mBackupChunkList, sBackupChunk );
        if( sBackupChunk != NULL )
        {
            STL_TRY( smgFreeShmMem4Mount( sBackupChunk, aEnv ) == STL_SUCCESS );
        }
        sBackupChunk = sPrevChunk;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief WarmupEntry의 BackupSegment에서 현재 Backup chunk를 얻는다.
 */
smfBackupChunk * smfGetCurrBackupChunk( )
{
    smfBackupSegment * sBackupSegment;

    sBackupSegment = &gSmfWarmupEntry->mBackupSegment;
    return sBackupSegment->mCurBackupChunk;
}

/**
 * @brief 주어진 BackupSegment의 첫번째 Backup chunk를 얻는다.
 * @param[in] aBackupSegment  Backup Segment
 */
smfBackupChunk * smfGetFirstBackupChunk( smfBackupSegment * aBackupSegment )
{
    return (smfBackupChunk * )STL_RING_GET_FIRST_DATA( &aBackupSegment->mBackupChunkList );
}

/**
 * @brief 주어진 BackupSegment의 Next Backup chunk를 얻는다.
 * @param[in]  aBackupSegment  Backup Segment
 * @param[in]  aCurrBackupChunk  Current Backup chunk
 */
smfBackupChunk * smfGetNextBackupChunk( smfBackupSegment * aBackupSegment,
                                        smfBackupChunk   * aCurrBackupChunk )
{
    smfBackupChunk * sLastChunk = NULL;

    sLastChunk = (smfBackupChunk *) STL_RING_GET_LAST_DATA( &aBackupSegment->mBackupChunkList );

    if( sLastChunk == aCurrBackupChunk )
    {
        return NULL;
    }
    
    return (smfBackupChunk * ) STL_RING_GET_NEXT_DATA( &aBackupSegment->mBackupChunkList, aCurrBackupChunk );
}

/**
 * @brief 주어진 BackupSegment의 Next Backup chunk를 얻는다.
 * @param[in]  aBackupSegment  Backup Segment
 * @param[in]  aCurrBackupChunk  Current Backup chunk
 */
smfBackupChunk * smfGetPrevBackupChunk( smfBackupSegment * aBackupSegment,
                                        smfBackupChunk   * aCurrBackupChunk )
{
    smfBackupChunk * sFirstChunk = NULL;

    sFirstChunk = (smfBackupChunk *) STL_RING_GET_FIRST_DATA( &aBackupSegment->mBackupChunkList );

    if( sFirstChunk == aCurrBackupChunk )
    {
        return NULL;
    }
    
    return (smfBackupChunk * ) STL_RING_GET_PREV_DATA( &aBackupSegment->mBackupChunkList, aCurrBackupChunk );
}

/**
 * @brief 주어진 BackupSegment의 모든 Chunk에서 Total Backup Count를 얻는ㄷ.
 * @param[in] aBackupSegment  Backup Segment
 */ 
stlInt64 smfGetTotalBackupCount( smfBackupSegment * aBackupSegment )
{
    smfBackupChunk    * sBackupChunk = NULL;
    smfBackupChunkHdr * sBackupChunkHdr = NULL;
    stlInt64            sBackupCnt = 0;

    STL_RING_FOREACH_ENTRY( &aBackupSegment->mBackupChunkList, sBackupChunk )
    {
        sBackupChunkHdr = (smfBackupChunkHdr *)sBackupChunk->mChunk;

        sBackupCnt += sBackupChunkHdr->mBackupCnt;
    }

    return sBackupCnt;
}

/**
 * @brief Backup Chunk에 backup 정보를 기록하기 위해 slot을 얻는다.
 * @param[in]  aBackupChunk    Backup Chunk
 * @param[in]  aFileNameSize   File Name Length
 * @param[out] aBackupRecord   Backup Record in Segment
 * @remark 성공하면 slot return, 실패하면 NULL return
 */
void smfAllocBackupSlot( smfBackupChunk * aBackupChunk,
                         stlSize          aFileNameSize,
                         void          ** aBackupRecord )
{
    smfBackupChunkHdr * sBackupChunkHdr;
    smfBackupRecord   * sBackupRecord;

    sBackupRecord = NULL;

    sBackupChunkHdr = (smfBackupChunkHdr *)aBackupChunk->mChunk;
    
    if( (sBackupChunkHdr->mHighWaterMark + STL_SIZEOF(smfBackupRecord) + aFileNameSize) <
        sBackupChunkHdr->mLowWaterMark )
    {
        sBackupRecord = (smfBackupRecord *)SMF_ALLOC_BACKUP_SLOT( aBackupChunk->mChunk, sBackupChunkHdr );
    }

    *aBackupRecord = sBackupRecord;
}

/**
 * @brief Backup slot에 backup 정보를 기록한다.
 */
void smfWriteBackupRecord( smfBackupRecord   * aBackupRecord,
                           smfBackupChunk    * aBackupChunk,
                           stlChar           * aBackupFileName,
                           stlSize             aFileNameSize,
                           stlTime             aBeginTime,
                           stlTime             aCompletionTime,
                           smrLsn              aBackupLsn,
                           smrLid              aBackupLid,
                           smlTbsId            aTbsId,
                           stlInt16            aLevel,
                           stlUInt8            aBackupObjectType,
                           stlUInt8            aBackupOption )
{
    smfBackupChunkHdr * sBackupChunkHdr;
    
    sBackupChunkHdr = (smfBackupChunkHdr *)aBackupChunk->mChunk;

    aBackupRecord->mBackupLsn    = aBackupLsn;
    aBackupRecord->mBackupLid    = aBackupLid;
    aBackupRecord->mTbsId        = aTbsId;
    aBackupRecord->mLevel        = aLevel;
    aBackupRecord->mObjectType   = aBackupObjectType;
    aBackupRecord->mBackupOption = aBackupOption;
    aBackupRecord->mNameOffset   = sBackupChunkHdr->mLowWaterMark - aFileNameSize;
    aBackupRecord->mNameLength   = aFileNameSize;
    aBackupRecord->mBeginTime    = aBeginTime;
    aBackupRecord->mCompletionTime = aCompletionTime;
    aBackupRecord->mIsDeleted    = STL_FALSE;

    stlStrncpy( aBackupChunk->mChunk + sBackupChunkHdr->mLowWaterMark - aFileNameSize,
                aBackupFileName,
                aFileNameSize );

    sBackupChunkHdr->mLowWaterMark -= aFileNameSize;
    sBackupChunkHdr->mHighWaterMark += STL_SIZEOF( smfBackupRecord );
    sBackupChunkHdr->mBackupCnt++;
}

/**
 * @brief Backup Segment에서 Latest Level 0 Database Backup을 찾는다.
 * @param[out]    aChunkId    첫번째 Valid한 backup이 저장된 segment Id
 * @param[out]    aRecordId   첫번째 Valid한 backup이 저장된 segment 내의 slot Id
 * @param[in,out] aEnv      Environment 정보
 */
stlStatus smfFindLatestLevel0Backup( stlInt64  * aChunkId,
                                     stlInt32  * aRecordId,
                                     smlEnv    * aEnv )
{
    stlInt32            i;
    stlInt64            sCurChunkId;
    smfSysPersData    * sSysPersData;
    smfBackupSegment  * sBackupSegment;
    smfBackupChunk    * sBackupChunk  = NULL;
    smfBackupChunkHdr * sBackupChunkHdr = NULL;
    smfBackupRecord   * sBackupRecord   = NULL;

    *aChunkId  = -1;
    *aRecordId = 0;

    sSysPersData    = &gSmfWarmupEntry->mSysPersData;
    sBackupSegment  = &gSmfWarmupEntry->mBackupSegment;
    
    sCurChunkId = sSysPersData->mBackupChunkCnt - 1;

    sBackupChunk = smfGetCurrBackupChunk( );
    
    while( sBackupChunk != NULL )
    {
        sBackupChunkHdr = (smfBackupChunkHdr *) sBackupChunk->mChunk;
        
        for( i = sBackupChunkHdr->mBackupCnt - 1; i >= 0; i-- )
        {
            sBackupRecord = (smfBackupRecord *)SMF_GET_NTH_BACKUP_SLOT( sBackupChunk->mChunk, i );

            if( (sBackupRecord->mLevel == 0) &&
                (sBackupRecord->mObjectType == SMF_INCREMENTAL_BACKUP_OBJECT_TYPE_DATABASE) )
            {
                *aChunkId  = sCurChunkId;
                *aRecordId = i;
                break;
            }
        }

        if( *aChunkId >= 0 )
        {
            break;
        }

        sBackupChunk = smfGetPrevBackupChunk( sBackupSegment,
                                              sBackupChunk );
        
        sCurChunkId--;
    }

    return STL_SUCCESS;
}

/**
 * @brief Backup file을 삭제한다.
 * @param[in] aFile          Backuped control file descriptor
 * @param[in] aSysPersData   System Persistent Data
 * @param[in] aEndChunkId    첫번째 Valid한 backup이 저장된 segment Id
 * @param[in] aEndRecordId   첫번째 Valid한 backup이 저장된 segment 내의 slot Id
 * @param[in,out] aEnv       Environment 정보
 */
stlStatus smfDeleteBackupFile( stlFile        * aFile,
                               smfSysPersData * aSysPersData,
                               stlInt32         aEndChunkId,
                               stlInt32         aEndRecordId,
                               smlEnv         * aEnv )
{
    smfBackupIterator sBackupIterator;
    stlBool           sExistBackup;
    stlBool           sExistFile;
    stlChar           sBackupBuffer[SMF_BACKUP_CHUNK_SIZE * 2];
    stlChar         * sBackupChunk;
    
    SMF_INIT_BACKUP_ITERATOR( &sBackupIterator, STL_TRUE );

    sBackupChunk = (stlChar*) STL_ALIGN( (stlInt64) sBackupBuffer, SMF_CONTROLFILE_IO_BLOCK_SIZE );
    
    STL_TRY( smfGetFirstIncBackup4Disk( aFile,
                                        aSysPersData,
                                        &sBackupIterator,
                                        sBackupChunk,
                                        &sExistBackup,
                                        aEnv ) == STL_SUCCESS );

    while( sExistBackup == STL_TRUE )
    {
        if( sBackupIterator.mBackupChunkSeq >= aEndChunkId )
        {
            if( (sBackupIterator.mBackupSlotSeq - 1) == aEndRecordId )
            {
                break;
            }
        }

        STL_TRY( stlExistFile( sBackupIterator.mFileName,
                               &sExistFile,
                               KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

        /**
         * Backup File을 삭제한다.
         */
        if( sExistFile == STL_TRUE )
        {
            STL_TRY( stlRemoveFile( sBackupIterator.mFileName,
                                    KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

            STL_TRY( knlLogMsg( NULL,
                                KNL_ENV( aEnv ),
                                KNL_LOG_LEVEL_INFO,
                                "[DELETE BACKUP LIST] '%s' is deleted\n",
                                sBackupIterator.mFileName )
                     == STL_SUCCESS );
        }
        else
        {
            STL_TRY( knlLogMsg( NULL,
                                KNL_ENV( aEnv ),
                                KNL_LOG_LEVEL_INFO,
                                "[DELETE BACKUP LIST] '%s' is not exist\n",
                                sBackupIterator.mFileName )
                     == STL_SUCCESS );

        }

        /**
         * 다음 Incremental Backup를 가져온다.
         */
        STL_TRY( smfGetNextIncBackup4Disk( aFile,
                                           aSysPersData,
                                           &sBackupIterator,
                                           sBackupChunk,
                                           &sExistBackup,
                                           aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @}
 */ 
