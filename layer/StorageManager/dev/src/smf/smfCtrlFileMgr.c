/*******************************************************************************
 * smfCtrlFileMgr.c
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
#include <smg.h>
#include <smxl.h>
#include <smrDef.h>
#include <smr.h>
#include <smlGeneral.h>
#include <smfCtrlFileMgr.h>
#include <smfCtrlFileBuffer.h>

/**
 * @file smfCtrlFileMgr.c
 * @brief Storage Manager Layer Control File Manager Internal Routines
 * @todo Control file에 대한 동시성 제어 로직이 추가되어야 함.
 * @todo smfSaveCtrlFile은 에러가 발생하지 않도록 설계되어야 한다.
 */

/**
 * @addtogroup smf
 * @{
 */

extern smfDatafileFuncs * gDatafileFuncs[];

/**
 * @brief Control File을 Open하고 Checksum 검사 후 타겟 Section으로 이동.
 * @param[out]    aFile     Control File Descriptor
 * @param[in]     aFilePath Control File Path
 * @param[in]     aSection  Target Section
 * @param[in,out] aIsValid  Checksum Validation result
 * @param[in,out] aEnv      Envrionment info
 * @remarks
 * <BR> Fixed Table을 위한 Open은 PushError를 하지 않고 Validation 결과를 이용하도록 한다
 */ 
stlStatus smfOpenCtrlSectionWithPath( stlFile        * aFile,
                                      stlChar        * aFilePath,
                                      smfCtrlSection   aSection,
                                      stlBool        * aIsValid,
                                      smlEnv         * aEnv )
{
    stlUInt32        sState = 0;
    stlOffset        sOffset;
    smfSysPersData * sSysPersData;
    stlInt32         sFlag;
    stlInt32         sSize;
    stlInt64         sSysFileIo;
    stlBool          sIsValid = STL_FALSE;
    stlSize          sReadBytes;
    stlChar          sBlock[SMF_CONTROLFILE_IO_BLOCK_SIZE + SMF_CONTROLFILE_IO_BUFFER_SIZE];

    sFlag = STL_FOPEN_READ;

    sSysFileIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_SYSTEM_FILE_IO,
                                                KNL_ENV(aEnv) );

    if( sSysFileIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

    /* controlfile을 read only로만 읽음 */
    STL_TRY( stlOpenFile( aFile,
                          aFilePath,
                          sFlag,
                          STL_FPERM_OS_DEFAULT,
                          KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    sState = 1;

    sSysPersData = (smfSysPersData *)STL_ALIGN( (stlInt64)sBlock, SMF_CONTROLFILE_IO_BLOCK_SIZE );
    sSize = STL_ALIGN( (stlInt64)STL_SIZEOF( smfSysPersData ), SMF_CONTROLFILE_IO_BLOCK_SIZE );

    STL_TRY( stlReadFile( aFile,
                          (stlChar *)sSysPersData,
                          sSize,
                          &sReadBytes,
                          KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    STL_TRY_THROW( sSize == sReadBytes, RAMP_ERR_CTRLFILE_CORRUPTED );

    STL_TRY( smfValidateCtrlFileChecksum( aFile,
                                          sSysPersData,
                                          &sIsValid,
                                          aEnv ) == STL_SUCCESS );

    if( aIsValid == NULL )
    {
        STL_TRY_THROW( sIsValid == STL_TRUE, RAMP_ERR_CTRLFILE_CORRUPTED );
    }
    else
    {
        *aIsValid = sIsValid;
    }

    switch( aSection )
    {
        case SMF_CTRL_SECTION_SYSTEM:
            sOffset = 0;
            break;
        case SMF_CTRL_SECTION_INC_BACKUP:
            sOffset = sSysPersData->mIncBackupSectionOffset;
            break;
        case SMF_CTRL_SECTION_LOG:
            sOffset = sSysPersData->mLogSectionOffset;
            break;
        case SMF_CTRL_SECTION_DB:
            sOffset = sSysPersData->mDbSectionOffset;
            break;
        default:
            STL_DASSERT( 0 );
            break;
    }

    STL_TRY( stlSeekFile( aFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CTRLFILE_CORRUPTED )
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
        (void) stlCloseFile( aFile, KNL_ERROR_STACK( aEnv ) );
    }

    return STL_FAILURE;
}

stlStatus smfOpenCtrlSection( stlFile        * aFile,
                              smfCtrlSection   aSection,
                              smlEnv         * aEnv )
{
    STL_TRY( smfOpenCtrlSectionWithPath( aFile,
                                         gSmfWarmupEntry->mCtrlFileName[0],
                                         aSection,
                                         NULL, /* aIsValid */
                                         aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smfCloseCtrlSection( stlFile * aFile,
                               smlEnv  * aEnv )
{
    STL_TRY( stlCloseFile( aFile,
                           KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Control File을 메모리에 구성한다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfLoadSysCtrlSection( smlEnv * aEnv )
{
    stlFile           sFile;
    stlUInt32         sState = 0;
    smfCtrlBuffer     sBuffer;

    STL_TRY( smfOpenCtrlSection( &sFile,
                                 SMF_CTRL_SECTION_SYSTEM,
                                 aEnv ) == STL_SUCCESS );
    sState = 1;

    SMF_INIT_CTRL_BUFFER( &sBuffer );

    STL_TRY( smfReadCtrlFile( &sFile,
                              &sBuffer,
                              (stlChar *)&gSmfWarmupEntry->mSysPersData,
                              STL_SIZEOF( smfSysPersData ),
                              aEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( smfCloseCtrlSection( &sFile,
                                  aEnv ) == STL_SUCCESS );

    smfSetCtrlFileLoadState( SMF_CTRL_FILE_LOAD_STATE_SYSTEM );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smfCloseCtrlSection( &sFile, aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief Control File의 Backup Section을 메모리에 구성한다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfLoadBackupCtrlSection( smlEnv * aEnv )
{
    stlFile             sFile;
    stlUInt32           sState = 0;
    smfSysPersData    * sSysPersData;
    smfBackupSegment  * sBackupSegment;
    smfBackupChunk    * sBackupChunk = NULL;
    smfCtrlBuffer       sBuffer;
    stlInt64            i;

    sSysPersData   = &gSmfWarmupEntry->mSysPersData;
    sBackupSegment = &gSmfWarmupEntry->mBackupSegment;
    
    STL_TRY( smfOpenCtrlSection( &sFile,
                                 SMF_CTRL_SECTION_INC_BACKUP,
                                 aEnv ) == STL_SUCCESS );
    sState = 1;

    SMF_INIT_CTRL_BUFFER( &sBuffer );

    /**
     * Control File에 Backup Persistent Data의 크기를 512 align으로 기록하였다.
     */ 
    SMF_INIT_CTRL_BUFFER( &sBuffer );
    
    for( i = 0; i < sSysPersData->mBackupChunkCnt; i++ )
    {
        STL_TRY( smgAllocShmMem4Mount( STL_SIZEOF( smfBackupChunk ),
                                       (void**)&sBackupChunk,
                                       aEnv ) == STL_SUCCESS );

        SMF_INIT_BACKUP_CHUNK( sBackupChunk );
        
        STL_TRY( smfReadCtrlFile( &sFile, 
                                  &sBuffer,
                                  (stlChar *)sBackupChunk->mChunk,
                                  SMF_BACKUP_CHUNK_SIZE,
                                  aEnv ) == STL_SUCCESS );

        STL_RING_ADD_LAST( &sBackupSegment->mBackupChunkList, sBackupChunk );
    }

    sBackupSegment->mCurBackupChunk = sBackupChunk;
    
    sState = 0;
    STL_TRY( smfCloseCtrlSection( &sFile,
                                  aEnv ) == STL_SUCCESS );
    
    smfSetCtrlFileLoadState( SMF_CTRL_FILE_LOAD_STATE_BACKUP );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smfCloseCtrlSection( &sFile, aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief 메모리에 구성된 Control File 정보를 디스크에 저장하고 삭제한다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfUnloadCtrlFile( smlEnv * aEnv )
{
    if( smfGetCtrlFileLoadState() >= SMF_CTRL_FILE_LOAD_STATE_LOG )
    {
        STL_TRY( smfSaveCtrlFile( aEnv ) == STL_SUCCESS );
    }

    smfFreeTbsInfoArray( aEnv );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Control File 정보를 디스크에 저장한다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfSaveCtrlFile( smlEnv * aEnv )
{
    stlFile           sFile;
    stlBool           sIsTimedOut;
    stlUInt32         sState = 0;
    stlInt32          sFlag;
    stlInt64          sSysFileIo;
    stlInt64          sCtrlFileCnt;
    stlInt32          i;

    STL_TRY_THROW( smfGetDataAccessMode() == SML_DATA_ACCESS_MODE_READ_WRITE, RAMP_SKIP );

    sCtrlFileCnt = smfGetCtrlFileCount();

    STL_TRY( knlAcquireLatch( &gSmfWarmupEntry->mLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;

    sFlag = STL_FOPEN_WRITE | STL_FOPEN_READ;

    sSysFileIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_SYSTEM_FILE_IO,
                                                KNL_ENV(aEnv) );

    if( sSysFileIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

    /**
     * Shutdown Abort와 같이 System Failure로 System이 죽는 경우
     * System이 죽기 전 모든 Control File을 쓰도록 한다.
     */
    knlIncOpenFileCount( KNL_SESS_ENV(aEnv) );
    sState = 2;
    
    for( i  = 0; i < sCtrlFileCnt; i++ )
    {
        STL_TRY( smgOpenFile( &sFile,
                              gSmfWarmupEntry->mCtrlFileName[i],
                              sFlag,
                              STL_FPERM_OS_DEFAULT,
                              aEnv )
                 == STL_SUCCESS );
        sState = 3;

        STL_TRY( smfSaveCtrlFileInternal( &sFile,
                                          aEnv ) == STL_SUCCESS );

        sState = 2;
        STL_TRY( smgCloseFile( &sFile,
                               aEnv )
                 == STL_SUCCESS );
    }

    sState = 1;
    knlDecOpenFileCount( KNL_SESS_ENV(aEnv) );
    
    sState = 0;
    STL_TRY( knlReleaseLatch( &gSmfWarmupEntry->mLatch,
                              (knlEnv*)aEnv ) == STL_SUCCESS );

    STL_RAMP( RAMP_SKIP );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 3:
            (void)smgCloseFile( &sFile, aEnv );
        case 2:
            knlDecOpenFileCount( KNL_SESS_ENV(aEnv) );
        case 1:
            (void)knlReleaseLatch( &gSmfWarmupEntry->mLatch, (knlEnv*)aEnv );
        default:
            break;
    }

    knlSystemFatal( "can't save control file", KNL_ENV(aEnv) );

    return STL_FAILURE;
}

/**
 * @brief Control File 정보를 디스크에 저장한다.
 * @param[in]     aFile Controlfile Desc
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfSaveCtrlFileInternal( stlFile  * aFile,
                                   smlEnv   * aEnv )
{
    stlSize      sWrittenBytes = 0;
    stlOffset    sOffset = 0;
    
    /**
     * Control file Version 확인을 위해 Checkpoint Lsn 설정.
     */ 
    smfSetLastChkptLsn( smrGetLastCheckpointLsn() );
    
    /**
     * System 관련 Persistent Information은 Section별 Offset
     * 기록을 위해서 마지막에 기록한다.
     */
    STL_TRY( smfWriteSysCtrlSection( aFile,
                                     &sWrittenBytes,
                                     aEnv ) == STL_SUCCESS );
    sOffset += sWrittenBytes;
    gSmfWarmupEntry->mSysPersData.mIncBackupSectionOffset = sOffset;
    
    /**
     * incremental Backup Section을 생성한다.
     */
    STL_TRY( smfWriteBackupCtrlSection( aFile,
                                        &sWrittenBytes,
                                        aEnv ) == STL_SUCCESS );
    sOffset += sWrittenBytes;
    gSmfWarmupEntry->mSysPersData.mLogSectionOffset = sOffset;
    
    /**
     * controlfile의 log section을 생성한다.
     */
    STL_TRY( smrWriteLogCtrlSection( aFile,
                                     &sWrittenBytes,
                                     aEnv ) == STL_SUCCESS );

    sOffset += sWrittenBytes;
    gSmfWarmupEntry->mSysPersData.mDbSectionOffset = sOffset;

    /**
     * controlfile의 DB section을 생성한다.
     */
    STL_TRY( smfWriteDbCtrlSection( aFile,
                                    &sWrittenBytes,
                                    aEnv ) == STL_SUCCESS );

    sOffset += sWrittenBytes;
    gSmfWarmupEntry->mSysPersData.mTailSectionOffset = sOffset;
    
    /**
     * 전체 System 관련 Persistent Information 기록
     */
    STL_TRY( smfWriteSysCtrlSection( aFile,
                                     &sWrittenBytes,
                                     aEnv ) == STL_SUCCESS );
    
    /**
     * Control File의 Checksum을 기록한다.
     */
    STL_TRY( smfWriteCtrlFileChecksum( aFile,
                                       aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief Control file에 Warmup Entry의 System persistent data를 기록한다.
 * @param[in]  aFile          Control file descriptor
 * @param[out] aWrittenBytes  File written bytes
 * @param[in,out] aEnv        Envrionment info
 */ 
stlStatus smfWriteSysCtrlSection( stlFile  * aFile,
                                  stlSize  * aWrittenBytes,
                                  smlEnv   * aEnv )
{
    stlOffset     sOffset = 0;
    stlSize       sWrittenBytes;
    smfCtrlBuffer sBuffer;

    SMF_INIT_CTRL_BUFFER( &sBuffer );

    STL_TRY( stlSeekFile( aFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
    
    STL_TRY( smfWriteCtrlFile( aFile,
                               &sBuffer,
                               (stlChar *)&(gSmfWarmupEntry->mSysPersData),
                               STL_SIZEOF( smfSysPersData ),
                               &sWrittenBytes,
                               aEnv )
             == STL_SUCCESS );

    STL_TRY( smfFlushCtrlFile( aFile, &sBuffer, aEnv ) == STL_SUCCESS );

    *aWrittenBytes = STL_ALIGN( (stlInt64)sWrittenBytes, SMF_CONTROLFILE_IO_BLOCK_SIZE );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Warmup Entry의 Incremental Backup 정보를 Control File에 기록한다.
 * @param[in]     aFile          File descriptor
 * @param[out]    aWrittenBytes  File written bytes
 * @param[in,out] aEnv           Environment info
 */ 
stlStatus smfWriteBackupCtrlSection( stlFile  * aFile,
                                     stlSize  * aWrittenBytes,
                                     smlEnv   * aEnv )
{
    stlSize             sWrittenBytes = 0;
    stlSize             sTotalWrittenBytes = 0;
    smfBackupSegment  * sBackupSegment;
    smfBackupChunk    * sBackupChunk;
    smfCtrlBuffer       sBuffer;

    sBackupSegment  = &gSmfWarmupEntry->mBackupSegment;
    
    SMF_INIT_CTRL_BUFFER( &sBuffer );

    STL_RING_FOREACH_ENTRY( &sBackupSegment->mBackupChunkList, sBackupChunk )
    {
        STL_TRY( smfWriteCtrlFile( aFile,
                                   &sBuffer,
                                   sBackupChunk->mChunk,
                                   SMF_BACKUP_CHUNK_SIZE,
                                   &sWrittenBytes,
                                   aEnv )
                 == STL_SUCCESS );
        
        sTotalWrittenBytes += sWrittenBytes;
    }

    STL_TRY( smfFlushCtrlFile( aFile, &sBuffer, aEnv ) == STL_SUCCESS );

    *aWrittenBytes = STL_ALIGN( (stlInt64)sTotalWrittenBytes, SMF_CONTROLFILE_IO_BLOCK_SIZE );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 새로운 Control File을 생성한다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfCreateCtrlFile( smlEnv * aEnv )
{
    stlFile        sFile;
    stlUInt32      sState = 0;
    stlSize        sWrittenBytes = 0;
    stlOffset      sOffset = 0;
    stlInt32       sFlag;
    stlInt64       sSysFileIo;
    stlInt64       sCtrlFileCnt;    
    stlInt32       i;
    stlBool        sIsExist = STL_FALSE;

    sFlag = STL_FOPEN_CREATE | STL_FOPEN_WRITE | STL_FOPEN_READ;

    sSysFileIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_SYSTEM_FILE_IO,
                                                KNL_ENV(aEnv) );

    sCtrlFileCnt = knlGetPropertyBigIntValueByID( KNL_PROPERTY_CONTROL_FILE_COUNT,
                                                  KNL_ENV(aEnv) );
   
    if( sSysFileIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }
    /**
     * Control file이 존재하면 error
     */
    STL_TRY( stlExistFile( gSmfWarmupEntry->mCtrlFileName[0],
                           &sIsExist,
                           KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sIsExist == STL_FALSE, RAMP_ERR_ALREADY_EXIST_FILE );
    
    STL_TRY( smgOpenFile( &sFile,
                          gSmfWarmupEntry->mCtrlFileName[0],
                          sFlag,
                          STL_FPERM_OS_DEFAULT,
                          aEnv )
             == STL_SUCCESS );
    sState = 1;

    SMF_INIT_SYS_PERSISTENT_DATA( &gSmfWarmupEntry->mSysPersData );

    /**
     * 초기화된 system persistent data를 기록한다.
     */
    STL_TRY( smfWriteSysCtrlSection( &sFile,
                                     &sWrittenBytes,
                                     aEnv ) == STL_SUCCESS );

    sOffset = sWrittenBytes;
    gSmfWarmupEntry->mSysPersData.mIncBackupSectionOffset = sOffset;
    
    /**
     * controlfile의 incremental backup section 생성은 backup persistent data만 기록한다.
     */
    STL_TRY( smfCreateBackupCtrlSection( &sFile,
                                         &sWrittenBytes,
                                         aEnv ) == STL_SUCCESS );
    sOffset += sWrittenBytes;
    gSmfWarmupEntry->mSysPersData.mLogSectionOffset = sOffset;

    /**
     * controlfile의 log section을 생성한다.
     */
    STL_TRY( smrCreateLogCtrlSection( &sFile,
                                      &sWrittenBytes,
                                      aEnv ) == STL_SUCCESS );

    sOffset += sWrittenBytes;
    gSmfWarmupEntry->mSysPersData.mDbSectionOffset = sOffset;

    /**
     * controlfile의 DB section을 생성한다.
     */
    STL_TRY( smfCreateDbCtrlSection( &sFile,
                                     &sWrittenBytes,
                                     aEnv ) == STL_SUCCESS );

    sOffset += sWrittenBytes;
    gSmfWarmupEntry->mSysPersData.mTailSectionOffset = sOffset;
    
    /**
     * system persistent data를 기록한다.
     */
    STL_TRY( smfWriteSysCtrlSection( &sFile,
                                     &sWrittenBytes,
                                     aEnv ) == STL_SUCCESS );

    /**
     * Control File의 Checksum을 계산하여 기록한다.
     */
    STL_TRY( smfWriteCtrlFileChecksum( &sFile,
                                       aEnv )
             == STL_SUCCESS );

    sState = 0;
    STL_TRY( smgCloseFile( &sFile,
                           aEnv )
             == STL_SUCCESS );

    /**
     * Mutiplexing Control File
     */
    for( i  = 1; i < sCtrlFileCnt; i++ )
    {
        STL_TRY( smfCopyCtrlFile( gSmfWarmupEntry->mCtrlFileName[0],
                                  gSmfWarmupEntry->mCtrlFileName[i],
                                  sFlag,
                                  aEnv ) == STL_SUCCESS );        
    }

    gSmfWarmupEntry->mCtrlFileCnt = sCtrlFileCnt;
   
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_ALREADY_EXIST_FILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_ALREADY_EXIST_FILE,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      gSmfWarmupEntry->mCtrlFileName[0] );
    }
    
    STL_FINISH;

    if( sState == 1 )
    {
        (void)stlRemoveFile( gSmfWarmupEntry->mCtrlFileName[0],
                             KNL_ERROR_STACK( aEnv ) );
        (void)smgCloseFile( &sFile, aEnv );
    }
    
    return STL_FAILURE;
}

stlStatus smfExistControlFiles( smlEnv * aEnv )
{
    stlInt32  i;
    stlInt64  sCtrlFileCnt;
    stlBool   sIsExist;

    sCtrlFileCnt = smfGetCtrlFileCount();

    for( i = 0; i < sCtrlFileCnt; i++ )
    {
        STL_TRY( stlExistFile( gSmfWarmupEntry->mCtrlFileName[i],
                               &sIsExist,
                               KNL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );

        STL_TRY_THROW( sIsExist == STL_TRUE, RAMP_ERR_NOT_EXIT_CTRL_FILE );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_EXIT_CTRL_FILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_FILE_NOT_EXIST,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      gSmfWarmupEntry->mCtrlFileName[i] );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief NoMount Phase에서 Mount Database를 위해 Controlfile의 Validation을 검사한다.
 * @param[in,out] aEnv   Environment 정보
 * @remarks Mount Database를 위한 Validation 요소
 * <BR> 1. Control Files 파일 유무
 * <BR> 2. File Permission( can read, write? )
 * <BR> 3. Version이 같은가.
 * <BR> 4. Checksum 검사.( Version 검사 중에 smfOpenCtrlSectionWithPath에서 수행 )
 */
stlStatus smfValidateControlfile4Mount( smlEnv   * aEnv )
{
    stlFile        sFile;
    stlInt64       i = 0;
    stlInt64       sCtrlFileCnt;
    stlInt32       sState = 0;
    smrLsn         sLastChkptLsn;
    stlTime        sDbCreationTime;
    smfCtrlBuffer  sBuffer;
    smfSysPersData sSysPersData;
    smrLogPersData sLogPersData;
    stlFileInfo    sFileInfo;
    stlBool        sIsValid = STL_FALSE;
    
    sCtrlFileCnt = smfGetCtrlFileCount();

    /**
     * 1. Control File들의 유무 확인.
     */ 
    STL_TRY( smfExistControlFiles( aEnv ) == STL_SUCCESS );

    /**
     * 2. Control File Write, Read Permission 확인.
     */
    for( i = 0; i < sCtrlFileCnt; i++ )
    {
        STL_TRY( stlGetFileStatByName( &sFileInfo,
                                       gSmfWarmupEntry->mCtrlFileName[i],
                                       STL_FINFO_UPROT,
                                       KNL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );
        /* read write 권한 검사.*/
        STL_TRY_THROW( (sFileInfo.mProtection & (STL_FPERM_UREAD | STL_FPERM_UWRITE)) ==
                       (STL_FPERM_UREAD | STL_FPERM_UWRITE),
                       RAMP_ERR_FILE_IS_READ_ONLY );
    }
    
    /**
     * 3. Version 검사.
     * Main(0번) Control File에서 smfSysPersData와 smrLogPersData를 읽는다.
     */ 
    STL_TRY( smfOpenCtrlSection( &sFile,
                                 SMF_CTRL_SECTION_SYSTEM,
                                 aEnv ) == STL_SUCCESS );
    sState = 1;

    SMF_INIT_CTRL_BUFFER( &sBuffer );
    
    STL_TRY( smfReadCtrlFile( &sFile,
                              &sBuffer,
                              (stlChar*) &sSysPersData,
                              STL_SIZEOF( smfSysPersData ),
                              aEnv ) == STL_SUCCESS );

    SMF_INIT_CTRL_BUFFER( &sBuffer );
    
    STL_TRY( smfMoveCtrlFilePosition( &sFile,
                                      &sBuffer,
                                      STL_FSEEK_SET,
                                      sSysPersData.mLogSectionOffset,
                                      aEnv ) == STL_SUCCESS );

    STL_TRY( smfReadCtrlFile( &sFile,
                              &sBuffer,
                              (stlChar*) &sLogPersData,
                              STL_SIZEOF( smrLogPersData ),
                              aEnv ) == STL_SUCCESS );
    
    sLastChkptLsn     = sSysPersData.mLastChkptLsn;
    sDbCreationTime = sLogPersData.mCreationTime;
    
    sState = 0;
    STL_TRY( smfCloseCtrlSection( &sFile, aEnv ) == STL_SUCCESS );

    /**
     *  Secondary(1번 이상) Control File을 읽고 Primary와 비교한다.
     */ 
    for( i = 1; i < sCtrlFileCnt; i++ )
    {
        STL_TRY( smfOpenCtrlSectionWithPath( &sFile,
                                             gSmfWarmupEntry->mCtrlFileName[i],
                                             SMF_CTRL_SECTION_SYSTEM,
                                             NULL, /* aIsValid */
                                             aEnv )
                 == STL_SUCCESS );
        sState = 1;

        STL_TRY( smfValidateCtrlFileVersion( &sFile,
                                             gSmfWarmupEntry->mCtrlFileName[i],
                                             sLastChkptLsn,
                                             sDbCreationTime,
                                             &sIsValid, 
                                             aEnv ) == STL_SUCCESS );

        STL_TRY_THROW( sIsValid == STL_TRUE, RAMP_ERR_INCONSISTENT_CTRL_FILE );
        
        sState = 0;
        STL_TRY( smfCloseCtrlSection( &sFile,
                                      aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_FILE_IS_READ_ONLY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_FILE_IS_READ_ONLY,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      gSmfWarmupEntry->mCtrlFileName[i] );
    }

    STL_CATCH( RAMP_ERR_INCONSISTENT_CTRL_FILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INCONSISTENT_CONTROL_FILE,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      gSmfWarmupEntry->mCtrlFileName[i] );
    }
    
    STL_FINISH;

    if( sState == 1 )
    {
        (void) smfCloseCtrlSection( &sFile, aEnv );
    }
    
    return STL_FAILURE;
}

/**
 * @brief Mount Phase에서 Open Database를 위해 Controlfile의 Validation을 체크한다.
 * @param[in]  aLogOption    NORESETLOGS/RESETLOGS
 * @param[in,out] aEnv   Environment 정보
 * @remarks Controlfile의 Last Checkpoint 정보를 이용하여 이후의 checkpoint가
 * <BR> 발생하지 않았으면 Valid하고, 이후에 checkpoint log가 있으면 invalid로 판단한다.
 */
stlStatus smfValidateControlfile4Open( stlUInt8   aLogOption,
                                       smlEnv   * aEnv )
{
    STL_TRY( smrValidateCheckpointLog( aLogOption, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Control File을 backup한다.
 * @param[in] aBackupType Backup type ( FULL / INCREMENTAL )
 * @param[in] aTarget Control file을 backup할 target
 * @param[in,out] aEnv Environment 정보
 * @remarks aTarget이 존재한다면 backup fail
 */
stlStatus smfBackupCtrlFile( stlUInt8     aBackupType,
                             stlChar    * aTarget,
                             smlEnv     * aEnv )
{
    stlBool           sIsTimedOut;
    stlUInt32         sState = 0;
    stlInt32          sFlag;
    stlInt64          sSysFileIo;
    stlChar           sFilePath[STL_MAX_FILE_PATH_LENGTH];
    stlChar           sTarget[STL_MAX_FILE_PATH_LENGTH];
    stlInt32          sFileNameLen;
    stlBool           sIsExist;

    if( aBackupType == SML_BACKUP_TYPE_FULL )
    {
        STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_LOG_DIR,
                                          sFilePath,
                                          KNL_ENV( aEnv ) ) == STL_SUCCESS );

        STL_TRY( stlMergeAbsFilePath( sFilePath,
                                      aTarget,
                                      sTarget,
                                      STL_MAX_FILE_PATH_LENGTH,
                                      &sFileNameLen,
                                      KNL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_BACKUP_DIR_1,
                                          sFilePath,
                                          KNL_ENV( aEnv ) ) == STL_SUCCESS );

        STL_TRY( stlMergeAbsFilePath( sFilePath,
                                      aTarget,
                                      sTarget,
                                      STL_MAX_FILE_PATH_LENGTH,
                                      &sFileNameLen,
                                      KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
    }

    STL_TRY( stlExistFile( sTarget,
                           &sIsExist,
                           KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    /**
     * Backup할 target이 이미 존재한다면 fail
     */
    STL_TRY_THROW( sIsExist == STL_FALSE, RAMP_ERR_ALREADY_EXIST_FILE );

    STL_TRY( knlAcquireLatch( &gSmfWarmupEntry->mLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;

    sFlag = STL_FOPEN_CREATE | STL_FOPEN_WRITE;

    sSysFileIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_SYSTEM_FILE_IO,
                                                KNL_ENV(aEnv) );

    if( sSysFileIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

    STL_TRY( smfCopyCtrlFile( gSmfWarmupEntry->mCtrlFileName[0],
                              sTarget,
                              sFlag,
                              aEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( knlReleaseLatch( &gSmfWarmupEntry->mLatch,
                              (knlEnv*)aEnv ) == STL_SUCCESS );

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "[INCREMENTAL BACKUP] control file - '%s'\n",
                        sTarget ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_ALREADY_EXIST_FILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_ALREADY_EXIST_FILE,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sTarget );
    }

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)knlReleaseLatch( &gSmfWarmupEntry->mLatch, (knlEnv*)aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus smfCopyCtrlFile( stlChar  * aSource,
                           stlChar  * aTarget,
                           stlInt32   aTargetFlag,
                           smlEnv   * aEnv )
{
    stlChar   * sBuffer;
    stlChar     sBlock[SMF_CONTROLFILE_IO_BLOCK_SIZE + SMF_CONTROLFILE_IO_BUFFER_SIZE];
    stlInt32    sBufferSize;

    sBufferSize = SMF_CONTROLFILE_IO_BUFFER_SIZE;
    sBuffer = (stlChar*)STL_ALIGN( (stlInt64)sBlock, SMF_CONTROLFILE_IO_BLOCK_SIZE );

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV(aEnv),
                        KNL_LOG_LEVEL_INFO,
                        ".... copy control file from '%s' to '%s'\n",
                        aSource,
                        aTarget )
             == STL_SUCCESS );
    
    STL_TRY( stlCopyFile( aSource,
                          aTarget,
                          aTargetFlag,
                          STL_FPERM_OS_DEFAULT,
                          sBuffer,
                          sBufferSize,
                          KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 현재 서버의 상태를 얻는다.
 * @return smlServerState
 */
inline smlServerState smfGetServerState()
{
    return gSmfWarmupEntry->mSysPersData.mServerState;
}

/**
 * @brief 현재 데이터베이스 접근 모드를 얻는다.
 * @return smlDataAccessMode
 */
inline smlDataAccessMode smfGetDataAccessMode()
{
    return gSmfWarmupEntry->mDataAccessMode;
}

/**
 * @brief 현재 DATABASE의 STORE MODE를 얻는다.
 * @return Data Store Mode
 * @see smlDataStoreMode
 */
inline stlInt64 smfGetDataStoreMode()
{
    return gSmfWarmupEntry->mSysPersData.mDataStoreMode;
}

/**
 * @brief 서버의 상태를 설정한다.
 * @param aServerState 설정할 서버 상태
 */
inline void smfSetServerState( smlServerState aServerState )
{
    gSmfWarmupEntry->mSysPersData.mServerState = aServerState;
}

/**
 * @brief 현재 DATABASE의 STORE MODE를 설정한다.
 * @see smlDataStoreMode
 */
inline void smfSetDataStoreMode( stlInt64 aDataStoreMode )
{
    gSmfWarmupEntry->mSysPersData.mDataStoreMode = aDataStoreMode;
}

/**
 * @brief Ctrl File Version 확인을 위해 Last Checkpoint Lsn을 SysPersData에 설정한다.
 * @param aChkptLsn Checkpoint한 Lsn Value
 */ 
inline void smfSetLastChkptLsn( smrLsn aChkptLsn )
{
    gSmfWarmupEntry->mSysPersData.mLastChkptLsn = aChkptLsn;
}

/**
 * @brief smfWarmupEntry에 기록된 현재 Control File의 개수를 얻는다.
 */ 
inline stlInt64 smfGetCtrlFileCount()
{
    return gSmfWarmupEntry->mCtrlFileCnt;
}

/**
 * @brief 현재 데이터베이스 접근 모드를 설정한다.
 */
inline void smfSetDataAccessMode( smlDataAccessMode aDataAccessMode )
{
    gSmfWarmupEntry->mDataAccessMode = aDataAccessMode;
}

/**
 * @brief 현재 데이터베이스의 트랜잭션 테이블 개수를 얻는다.
 */
inline stlUInt16 smfGetTransTableSize()
{
    return gSmfWarmupEntry->mDbPersData.mTransTableSize;
}

/**
 * @brief 현재 데이터베이스의 트랜잭션 테이블 개수를 설정한다.
 */
inline void smfSetTransTableSize( stlUInt16 aTransTableSize )
{
    gSmfWarmupEntry->mDbPersData.mTransTableSize = aTransTableSize;
}

/**
 * @brief 현재 데이터베이스의 언두 릴레이션 개수를 얻는다.
 */
inline stlUInt16 smfGetUndoRelCount()
{
    return gSmfWarmupEntry->mDbPersData.mUndoRelCount;
}

/**
 * @brief 현재 데이터베이스의 언두 릴레이션 개수를 설정한다.
 */
inline void smfSetUndoRelCount( stlUInt16 aUndoRelCount )
{
    gSmfWarmupEntry->mDbPersData.mUndoRelCount = aUndoRelCount;
}

/**
 * @brief Resetlogs 시 설정할 system SCN을 설정한다.
 */
inline void smfSetResetLogsScn( smlScn aSystemScn )
{
    gSmfWarmupEntry->mDbPersData.mResetLogsScn = aSystemScn;
}

/**
 * @brief Resetlogs 시 설정할 system SCN을 얻는다.
 */
inline smlScn smfGetResetLogsScn()
{
    return gSmfWarmupEntry->mDbPersData.mResetLogsScn;
}

/**
 * @brief 현재 데이터베이스의 TbsCount를 얻는다.
 */
inline stlUInt16 smfGetTbsCount()
{
    return gSmfWarmupEntry->mDbPersData.mTbsCount;
}

/**
 * @brief 현재 Control file load state를 설정한다.
 */
inline void smfSetCtrlFileLoadState( smfCtrlLoadState  aState )
{
    gSmfWarmupEntry->mCtrlLoadState = aState;
}

/**
 * @brief 현재 Control file load state를 얻는다.
 */
inline smfCtrlLoadState smfGetCtrlFileLoadState()
{
    return gSmfWarmupEntry->mCtrlLoadState;
}

static stlChar * smfGetServerStateString( smlServerState aServerState )
{
    stlChar * sString = NULL;

    switch( aServerState )
    {
        case SML_SERVER_STATE_NONE :
            sString = "NONE";
            break;
        case SML_SERVER_STATE_RECOVERED :
            sString = "RECOVERED";
            break;
        case SML_SERVER_STATE_RECOVERING :
            sString = "RECOVERING";
            break;
        case SML_SERVER_STATE_SERVICE :
            sString = "SERVICE";
            break;
        case SML_SERVER_STATE_SHUTDOWN :
            sString = "SHUTDOWN";
            break;
        default :
            STL_DASSERT( 0 );
            break;
    }

    return sString;
}

void smfServerStateString( smlServerState   aServerState,
                           stlChar        * aString )
{
    stlStrcpy( aString, smfGetServerStateString( aServerState ) );
}

static stlChar * smfGetDataStoreModeString( stlInt64 aDataStoreMode )
{
    stlChar * sString = NULL;

    switch( aDataStoreMode )
    {
        case SML_DATA_STORE_MODE_CDS :
            sString = "CDS";
            break;
        case SML_DATA_STORE_MODE_TDS :
            sString = "TDS";
            break;
        default :
            STL_DASSERT( 0 );
            break;
    }

    return sString;
}

void smfDataStoreModeString( stlInt64    aDataStoreMode,
                             stlChar   * aString )
{
    stlStrcpy( aString, smfGetDataStoreModeString( aDataStoreMode ) );
}

/**
 * @brief Control File의 System Section을 Dump한다.
 * @param[in] aFilePath Control File Path
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfDumpSysCtrlSection( stlChar * aFilePath,
                                 smlEnv  * aEnv )
{
    stlFile           sFile;
    stlUInt32         sState = 0;
    smfSysPersData    sSysPersData;
    smfCtrlBuffer     sBuffer;
    stlBool           sIsValid;

    STL_TRY( smfOpenCtrlSectionWithPath( &sFile,
                                         aFilePath,
                                         SMF_CTRL_SECTION_SYSTEM,
                                         &sIsValid, /* aIsValid */
                                         aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY_THROW( sIsValid == STL_TRUE, RAMP_ERR_CTRLFILE_CORRUPTED );

    SMF_INIT_CTRL_BUFFER( &sBuffer );

    STL_TRY( smfReadCtrlFile( &sFile,
                              &sBuffer,
                              (stlChar *)&sSysPersData,
                              STL_SIZEOF( smfSysPersData ),
                              aEnv ) == STL_SUCCESS );

    stlPrintf( " [SYSTEM SECTION]\n"
               "-----------------------------------------------------------\n"
               "  SERVER STATE                      : %s\n"
               "  DATA STORE MODE                   : %s\n"
               "  LAST CHECKPOINT LSN               : %ld\n"
               "  INCREMENTAL BACKUP CHUNK COUNT    : %ld\n"
               "  INCREMENTAL BACKUP SECTION OFFSET : %d\n"
               "  LOG  SECTION OFFSET               : %ld\n"
               "  DB   SECTION OFFSET               : %ld\n",
               smfGetServerStateString( sSysPersData.mServerState ),
               smfGetDataStoreModeString( sSysPersData.mDataStoreMode ),
               sSysPersData.mLastChkptLsn,
               sSysPersData.mBackupChunkCnt,
               sSysPersData.mIncBackupSectionOffset,
               sSysPersData.mLogSectionOffset,
               sSysPersData.mDbSectionOffset );
    
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

/**
 * @brief Control File의 Backup Section을 Dump한다.
 * @param[in] aFilePath Control File Path
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfDumpIncBackupCtrlSection( stlChar * aFilePath,
                                       smlEnv  * aEnv )
{
    stlFile            sFile;
    stlUInt32          sState = 0;
    stlBool            sExistBackup;
    smfSysPersData     sSysPersData;
    smfCtrlBuffer      sBuffer;
    stlChar            sObjectType[SMF_CTRL_STATE_LENGTH];
    smfBackupIterator  sBackupIterator;
    stlBool            sIsValid;
    stlChar            sBackupBuffer[SMF_BACKUP_CHUNK_SIZE * 2];
    stlChar          * sBackupChunk;
    stlInt64           sBackupSeq = 0;

    STL_TRY( smfOpenCtrlSectionWithPath( &sFile,
                                         aFilePath,
                                         SMF_CTRL_SECTION_SYSTEM,
                                         &sIsValid, /* aIsValid */
                                         aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY_THROW( sIsValid == STL_TRUE, RAMP_ERR_CTRLFILE_CORRUPTED );

    sBackupChunk = (stlChar*) STL_ALIGN( (stlInt64) sBackupBuffer, SMF_CONTROLFILE_IO_BLOCK_SIZE );
    
    SMF_INIT_CTRL_BUFFER( &sBuffer );

    STL_TRY( smfReadCtrlFile( &sFile,
                              &sBuffer,
                              (stlChar *)&sSysPersData,
                              STL_SIZEOF( smfSysPersData ),
                              aEnv ) == STL_SUCCESS );

    stlPrintf( "\n [INCREMENTAL BACKUP SECTION]\n" );
    stlPrintf( "-----------------------------------------------------------\n" );
    
    if( sSysPersData.mBackupChunkCnt > 0 )
    {
        SMF_INIT_BACKUP_ITERATOR( &sBackupIterator, STL_FALSE );

        STL_TRY( smfGetFirstIncBackup4Disk( &sFile,
                                            &sSysPersData,
                                            &sBackupIterator,
                                            sBackupChunk,
                                            &sExistBackup,
                                            aEnv ) == STL_SUCCESS );
        
        while( sExistBackup == STL_TRUE )
        {
            if( sBackupIterator.mBackupRecord.mIsDeleted == STL_TRUE )
            {
                STL_TRY( smfGetNextIncBackup4Disk( &sFile,
                                                   &sSysPersData,
                                                   &sBackupIterator,
                                                   sBackupChunk,
                                                   &sExistBackup,
                                                   aEnv ) == STL_SUCCESS );
            }
            
            switch( sBackupIterator.mBackupRecord.mObjectType )
            {
                case SMF_INCREMENTAL_BACKUP_OBJECT_TYPE_DATABASE:
                    stlStrcpy( sObjectType, "database" );
                    break;
                case SMF_INCREMENTAL_BACKUP_OBJECT_TYPE_TABLESPACE:
                    stlStrcpy( sObjectType, "tablespace" );
                    break;
                case SMF_INCREMENTAL_BACKUP_OBJECT_TYPE_CONTROLFILE:
                    stlStrcpy( sObjectType, "control" );
                    break;
                default:
                    stlStrcpy( sObjectType, "unknown" );
                    break;
            }

            stlPrintf("  [ BACKUP #%d ]\n"
                      "           FILE PATH     : %s\n"
                      "           BACKUP LSN    : %ld\n"
                      "           BACKUP LEVEL  : %d\n"
                      "           BACKUP OBJECT : %s\n",
                      sBackupSeq,
                      sBackupIterator.mFileName,
                      sBackupIterator.mBackupRecord.mBackupLsn,
                      sBackupIterator.mBackupRecord.mLevel,
                      sObjectType );

            sBackupSeq++;

            STL_TRY( smfGetNextIncBackup4Disk( &sFile,
                                               &sSysPersData,
                                               &sBackupIterator,
                                               sBackupChunk,
                                               &sExistBackup,
                                               aEnv ) == STL_SUCCESS );
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
 * @addtogroup smfCtrlFileMgr
 * @{
 */

void smfFreeTbsInfoArray( smlEnv * aEnv )
{
    smfTbsInfo      * sTbsInfo;
    smfDatafileInfo * sDatafileInfo;
    stlUInt32         i;
    stlUInt32         j;
    
    for( i = 0; i < SML_TBS_MAX_COUNT; i++ )
    {
        sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[i];

        /**
         * sTbsInfo가 NoMount Phase에서 할당한 주소와 같다면 Skip
         */ 
        if( (sTbsInfo == NULL) ||
            (sTbsInfo == gSmfWarmupEntry->mNoMountTbsInfoArray[i]) )
        {
            continue;
        }

        for( j = 0; j < SML_DATAFILE_MAX_COUNT; j++ )
        {
            sDatafileInfo = sTbsInfo->mDatafileInfoArray[j];

            if( sDatafileInfo == NULL )
            {
                continue;
            }

            (void)smgFreeShmMem4Mount( (void*)sDatafileInfo, aEnv );
            sTbsInfo->mDatafileInfoArray[j] = NULL;
            
        }
        
        (void)smgFreeShmMem4Mount( (void*)sTbsInfo, aEnv );
        gSmfWarmupEntry->mTbsInfoArray[i] = NULL;;
    }
    
    /*
     * 해당 함수는 BOOT-DOWN에서만 사용됨으로 예외 처리하지 않는다.
     * Kernel의 전체 Variable Area가 Free될때 처리된다.
     */
}

stlStatus smfWriteCtrlFileChecksum( stlFile   * aFile,
                                    smlEnv    * aEnv )
{
    stlOffset      sOffset;
    smfSysPersData sSysPersData;
    smfCtrlBuffer  sBuffer;
    stlChar      * sData;
    stlUInt32      sCrc32 = STL_CRC32_INIT_VALUE;
    knlRegionMark  sMemMark;
    stlInt32       sState = 0;
    stlSize        sWrittenBytes;
    stlInt64       sBufferSize;
    stlInt64       sTotalSize;
    stlInt64       sReadSize;
    stlInt64       sDoneSize;

    KNL_INIT_REGION_MARK( &sMemMark );
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               KNL_ENV(aEnv) ) == STL_SUCCESS );
    sState = 1;

    sOffset = 0;
    STL_TRY( stlSeekFile( aFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    SMF_INIT_CTRL_BUFFER( &sBuffer );

    STL_TRY( smfReadCtrlFile( aFile,
                              &sBuffer,
                              (stlChar*)&sSysPersData,
                              STL_SIZEOF( smfSysPersData ),
                              aEnv ) == STL_SUCCESS );

    sBufferSize = STL_MIN( sSysPersData.mTailSectionOffset, SMF_CONTROLFILE_IO_MAX_READ_SIZE );

    STL_DASSERT( sBufferSize == STL_ALIGN( (stlInt64) sBufferSize, SMF_CONTROLFILE_IO_BLOCK_SIZE ) );
    
    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                sBufferSize,
                                (void*)&sData,
                                KNL_ENV(aEnv) ) == STL_SUCCESS );

    sOffset = 0;
    STL_TRY( stlSeekFile( aFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    SMF_INIT_CTRL_BUFFER( &sBuffer );
    
    sDoneSize = 0;
    sTotalSize = sSysPersData.mTailSectionOffset;
    while( STL_TRUE )
    {
        if( sDoneSize == sTotalSize )
        {
            break;
        }

        sReadSize = STL_MIN( sBufferSize, (sTotalSize - sDoneSize) );
        
        STL_TRY( smfReadCtrlFile( aFile,
                                  &sBuffer,
                                  sData,
                                  sReadSize,
                                  aEnv ) == STL_SUCCESS );

        STL_TRY( stlCrc32( (void*) sData,
                           sReadSize,
                           &sCrc32,
                           KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

        sDoneSize += sReadSize;
    }
    
    sOffset = sSysPersData.mTailSectionOffset;
    STL_TRY( stlSeekFile( aFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    SMF_INIT_CTRL_BUFFER( &sBuffer );

    STL_TRY( smfWriteCtrlFile( aFile,
                               &sBuffer,
                               (stlChar *) &sCrc32,
                               STL_SIZEOF( stlUInt32 ),
                               &sWrittenBytes,
                               aEnv )
             == STL_SUCCESS );
    
    STL_TRY( smfFlushCtrlFile( aFile,
                               &sBuffer,
                               aEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       KNL_ENV(aEnv) ) == STL_SUCCESS );
    return STL_SUCCESS;
    
    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                        &sMemMark,
                                        STL_FALSE, /* aFreeChunk */
                                        KNL_ENV(aEnv) );
    }

    return STL_FAILURE;
}

/**
 * @brief Control file의 Checksum을 이용하여 Validation 검사를 한다.
 * @param[in]  aFile           File Descriptor
 * @param[in]  aSysPersData    Control File의 System Section
 * @param[out] aIsValid        Control File의 Validation 여부
 * @param[in]  aEnv            Envrionment 정보
 */ 
stlStatus smfValidateCtrlFileChecksum( stlFile        * aFile,
                                       smfSysPersData * aSysPersData,
                                       stlBool        * aIsValid,
                                       smlEnv         * aEnv )
{
    stlOffset        sOffset;
    smfCtrlBuffer    sBuffer;
    stlUInt32        sChecksumValue = 0;
    stlUInt32        sCrc32 = STL_CRC32_INIT_VALUE;
    stlInt32         sState = 0;
    stlChar        * sData;
    stlInt64         sBufferSize;
    stlInt64         sTotalSize;
    stlInt64         sReadSize;
    stlInt64         sDoneSize;
    knlRegionMark    sMemMark;

    *aIsValid = STL_FALSE;

    sBufferSize = STL_MIN( aSysPersData->mTailSectionOffset,
                           SMF_CONTROLFILE_IO_MAX_READ_SIZE );

    STL_TRY_THROW( (sBufferSize > 0) &&
                   (sBufferSize == STL_ALIGN((stlInt64)sBufferSize, SMF_CONTROLFILE_IO_BLOCK_SIZE)),
                   RAMP_SKIP_VALIDATE );

    SMF_INIT_CTRL_BUFFER( &sBuffer );

    KNL_INIT_REGION_MARK( &sMemMark );
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               KNL_ENV(aEnv) ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                sBufferSize,
                                (void*)&sData,
                                KNL_ENV(aEnv) ) == STL_SUCCESS );

    sOffset = aSysPersData->mTailSectionOffset;

    /**
     * Control File의 Checksum을 읽어온다.
     */ 
    STL_TRY( stlSeekFile( aFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    STL_TRY( smfReadCtrlFile( aFile,
                              &sBuffer,
                              (stlChar *) &sChecksumValue,
                              STL_SIZEOF( stlUInt32 ),
                              aEnv ) == STL_SUCCESS );

    /**
     * Control File의 Checksum 계산을 위해 Checksum Section 전까지 모두 읽어온다.
     */  
    sOffset = 0;
    STL_TRY( stlSeekFile( aFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    SMF_INIT_CTRL_BUFFER( &sBuffer );

    sDoneSize = 0;
    sTotalSize = aSysPersData->mTailSectionOffset;
    while( STL_TRUE )
    {
        if( sDoneSize == sTotalSize )
        {
            break;
        }

        sReadSize = STL_MIN( sBufferSize, (sTotalSize - sDoneSize) );
        
        STL_TRY( smfReadCtrlFile( aFile,
                                  &sBuffer,
                                  sData,
                                  sReadSize,
                                  aEnv ) == STL_SUCCESS );

        STL_TRY( stlCrc32( (void*) sData,
                           (stlInt32) sReadSize,
                           &sCrc32,
                           KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

        sDoneSize += sReadSize;
    }

    *aIsValid = (sCrc32 == sChecksumValue ) ? STL_TRUE : STL_FALSE;

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       KNL_ENV(aEnv) ) == STL_SUCCESS );

    STL_RAMP( RAMP_SKIP_VALIDATE );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
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
 * @brief Control File의 Validation 확인.
 * @param[in]  aFile           Validation 대상 File
 * @param[in]  aFilePath       Control file path
 * @param[in]  aLastChkptLsn   비교되는 Checkpoint Lsn
 * @param[in]  aDbCreationTime 비교되는 Creation Time
 * @param[out] aIsValid        Version 검사 결과
 * @param[in,out] aEnv         Environment
 * @remarks SysPersData의 Last Checkpoint Lsn과 LogPersData의 Db Creation Time으로 검사.
 */ 
stlStatus smfValidateCtrlFileVersion( stlFile   * aFile,
                                      stlChar   * aFilePath,
                                      smrLsn      aLastChkptLsn,
                                      stlTime     aDbCreationTime,
                                      stlBool   * aIsValid,
                                      smlEnv    * aEnv )
{
    smfCtrlBuffer   sBuffer;
    smfSysPersData  sSysPersData;
    smrLogPersData  sLogPersData;
    stlBool         sIsValid = STL_TRUE;

    SMF_INIT_CTRL_BUFFER( &sBuffer );

    /**
     * SysPersData와 LogPersData를 읽는다.
     */ 
    STL_TRY( smfReadCtrlFile( aFile,
                              &sBuffer,
                              (stlChar *) &sSysPersData,
                              STL_SIZEOF( smfSysPersData ),
                              aEnv ) == STL_SUCCESS );

    SMF_INIT_CTRL_BUFFER( &sBuffer );

    STL_TRY( smfMoveCtrlFilePosition( aFile,
                                      &sBuffer,
                                      STL_FSEEK_SET,
                                      sSysPersData.mLogSectionOffset,
                                      aEnv ) == STL_SUCCESS );

    STL_TRY( smfReadCtrlFile( aFile,
                              &sBuffer,
                              (stlChar *) &sLogPersData,
                              STL_SIZEOF( smrLogPersData ),
                              aEnv ) == STL_SUCCESS );

    /**
     * Parameter로 들어온 기준값과 비교.
     */ 
    if( (sSysPersData.mLastChkptLsn != aLastChkptLsn) ||
        (sLogPersData.mCreationTime != aDbCreationTime) )
    {
        STL_TRY( knlLogMsg( NULL,
                            KNL_ENV(aEnv),
                            KNL_LOG_LEVEL_WARNING,
                            "[CONTROL FILE CONSISTENCY] inconsistent control file '%s' (%ld, %lu) with primary (%ld, %lu)'\n",
                            aFilePath,
                            sSysPersData.mLastChkptLsn,
                            sLogPersData.mCreationTime,
                            aLastChkptLsn,
                            aDbCreationTime )
                 == STL_SUCCESS );

        sIsValid = STL_FALSE;
    }

    if( aIsValid != NULL )
    {
        *aIsValid = sIsValid;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smfAcquireDatabaseFileLatch( smlEnv * aEnv )
{
    stlBool  sIsTimedOut;

    return knlAcquireLatch( &gSmfWarmupEntry->mLatch,
                            KNL_LATCH_MODE_EXCLUSIVE,
                            KNL_LATCH_PRIORITY_NORMAL,
                            STL_INFINITE_TIME,
                            &sIsTimedOut,
                            (knlEnv*)aEnv );
}

stlStatus smfReleaseDatabaseFileLatch( smlEnv * aEnv )
{
    return knlReleaseLatch( &gSmfWarmupEntry->mLatch, (knlEnv*)aEnv );
}

stlStatus smfAdjustDbSection( smlEnv * aEnv )
{
    smxlSetSystemScn( smfGetResetLogsScn() );

    return STL_SUCCESS;
}

/**
 * @brief 불완전 복구 후 resetlogs를 위해 Warmup Entry를 정리한다.
 */
stlStatus smfAdjustIncBackupSection( smlEnv * aEnv )
{
    STL_TRY( smfDeleteAllBackupList( aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief 불완전 복구 후 resetlogs를 위해 controlfile을 adjust한다.
 */
stlStatus smfAdjustCtrlfile( smlEnv * aEnv )
{
    /**
     * Resetlogs를 위해 Log를 Reset 전 Online log를 archiving 한다.
     */   
    STL_TRY( smrArchivingLogFiles4ResetLogs( aEnv ) == STL_SUCCESS );

    /**
     * Db Section을 보정한다.
     */
    STL_TRY( smfAdjustDbSection( aEnv ) == STL_SUCCESS );

    /**
     * Log Section을 보정한다.
     */
    STL_TRY( smrAdjustLogSection( aEnv ) == STL_SUCCESS );

    /**
     * Incremental Backup Section을 보정하고 smfSaveCtrlFileInternal 수행한다.
     */
    STL_TRY( smfAdjustIncBackupSection( aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Controlfile에 incremental backup section을 생성한다.
 * @param[in]  aFile          Control file descriptor
 * @param[out] aWrittenBytes  File written bytes
 * @param[in,out] aEnv        Envrionment info
 * @remarks Control file을 처음 만들때 BackupSection에 아무것도 생성하지 않는다.
 */
stlStatus smfCreateBackupCtrlSection( stlFile  * aFile,
                                      stlSize  * aWrittenBytes,
                                      smlEnv   * aEnv )
{
    /* nothing to do */
    *aWrittenBytes = 0;
    
    return STL_SUCCESS;
}

/**
 * @brief Control File을 Backup Control File에서 Restore한다.
 * @param[in]     aBackupCtrlFile  Backup Control File Name
 * @param[in,out] aEnv             Environment
 */
stlStatus smfRestoreCtrlFile( stlChar * aBackupCtrlFile,
                              smlEnv  * aEnv )
{
    stlFile          sFile;
    stlInt32         sState = 0;
    stlBool          sIsExist;
    stlInt32         sFileNameLen;
    stlInt32         sFlag;
    stlInt64         sSysFileIo;
    stlInt64         sSize;
    stlSize          sReadBytes;
    stlChar          sFilePath[STL_MAX_FILE_PATH_LENGTH + STL_MAX_FILE_NAME_LENGTH];
    stlChar          sBackup[STL_MAX_FILE_PATH_LENGTH + STL_MAX_FILE_NAME_LENGTH];
    stlInt64         sCtrlFileCnt;
    stlInt64         i;
    smfSysPersData * sSysPersData;
    stlChar          sBlock[SMF_CONTROLFILE_IO_BLOCK_SIZE + SMF_CONTROLFILE_IO_BUFFER_SIZE];
    stlBool          sIsValid = STL_FALSE;

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV(aEnv),
                        KNL_LOG_LEVEL_INFO,
                        "[RESTORE CONTROLFILE] start - '%s'\n",
                        aBackupCtrlFile )
             == STL_SUCCESS );
    /**
     * Backup Control File 존재하는지 검사.
     */
    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_LOG_DIR,
                                      sFilePath,
                                      KNL_ENV( aEnv ) ) == STL_SUCCESS );

    STL_TRY( stlMergeAbsFilePath( sFilePath,
                                  aBackupCtrlFile,
                                  sBackup,
                                  STL_MAX_FILE_PATH_LENGTH,
                                  &sFileNameLen,
                                  KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
    
    STL_TRY( stlExistFile( sBackup,
                           &sIsExist,
                           KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    STL_TRY_THROW( sIsExist == STL_TRUE, RAMP_ERR_NOT_EXIST_FILE );

    sSysFileIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_SYSTEM_FILE_IO,
                                                KNL_ENV(aEnv) );

    sFlag = STL_FOPEN_READ;
    
    if( sSysFileIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

    /**
     * Restore 전 Backup Control File Checksum 검사.
     */

    sSysPersData = (smfSysPersData *)STL_ALIGN( (stlInt64)sBlock, SMF_CONTROLFILE_IO_BLOCK_SIZE );
    sSize = STL_ALIGN( (stlInt64)STL_SIZEOF( smfSysPersData ), SMF_CONTROLFILE_IO_BLOCK_SIZE );
    
    STL_TRY( stlOpenFile( &sFile,
                          sBackup,
                          sFlag,
                          STL_FPERM_OS_DEFAULT,
                          KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( stlReadFile( &sFile,
                          (stlChar *)sSysPersData,
                          sSize,
                          &sReadBytes,
                          KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    STL_TRY( smfValidateCtrlFileChecksum( &sFile,
                                          sSysPersData,
                                          &sIsValid,
                                          aEnv )
             == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( stlCloseFile( &sFile, KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    STL_TRY_THROW( sIsValid == STL_TRUE, RAMP_ERR_CTRLFILE_CORRUPTED );
    
    sFlag = STL_FOPEN_CREATE | STL_FOPEN_WRITE;
    
    if( sSysFileIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }
    
    sCtrlFileCnt = smfGetCtrlFileCount();
    
    for( i = 0; i < sCtrlFileCnt; i++ )
    {    
        STL_TRY( smfCopyCtrlFile( sBackup,
                                  gSmfWarmupEntry->mCtrlFileName[i],
                                  sFlag,
                                  aEnv ) == STL_SUCCESS );
    }        

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV(aEnv),
                        KNL_LOG_LEVEL_INFO,
                        "[RESTORE CONTROLFILE] end - '%s'\n",
                        aBackupCtrlFile )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_EXIST_FILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_FILE_NOT_EXIST,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sBackup );
    }

    STL_CATCH( RAMP_ERR_CTRLFILE_CORRUPTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_CTRLFILE_CORRUPTED,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sBackup );
    }

    STL_FINISH;

    if( sState == 1 )
    {
        (void) stlCloseFile( &sFile, KNL_ERROR_STACK(aEnv) );
    }

    (void) knlLogMsg( NULL,
                      KNL_ENV(aEnv),
                      KNL_LOG_LEVEL_INFO,
                      "[RESTORE CONTROLFILE] failed - '%s'\n",
                      aBackupCtrlFile );
    
    return STL_FAILURE;
}


stlStatus smfSaveCtrlFile4Syncher( void      * aSmfWarmupEntry,
                                   void      * aSmrWarmupEntry,
                                   stlBool     aSilent,
                                   stlLogger * aLogger,
                                   smlEnv    * aEnv )
{
    smfWarmupEntry * sWarmupEntry;
    stlFile          sFile;
    stlInt32         sState = 0;
    stlInt64         sCtrlFileCnt;
    stlInt32         i = 0;
    stlBool          sIsValid;
    stlChar          sBlock[SMF_CONTROLFILE_IO_BLOCK_SIZE + SMF_CONTROLFILE_IO_BUFFER_SIZE];
    smfSysPersData * sSysPersData;
    stlInt64         sSize;
    stlSize          sReadBytes;
    stlInt32         sFlag;
    stlChar          sMsg[1024] = { 0, };
    stlInt64         sSysFileIo;
    
    STL_DASSERT( aSmfWarmupEntry != NULL );
    STL_DASSERT( aSmrWarmupEntry != NULL );

    sWarmupEntry = aSmfWarmupEntry;

    sCtrlFileCnt = sWarmupEntry->mCtrlFileCnt;

    sFlag = STL_FOPEN_WRITE | STL_FOPEN_READ;

    sSysFileIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_SYSTEM_FILE_IO,
                                                KNL_ENV(aEnv) );

    if( sSysFileIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

    sSysPersData = (smfSysPersData *)STL_ALIGN( (stlInt64)sBlock, SMF_CONTROLFILE_IO_BLOCK_SIZE );
    sSize = STL_ALIGN( (stlInt64)STL_SIZEOF( smfSysPersData ), SMF_CONTROLFILE_IO_BLOCK_SIZE );
    for( i = 0; i < sCtrlFileCnt; i++ )
    {
        STL_TRY( stlOpenFile( &sFile,
                              sWarmupEntry->mCtrlFileName[i],
                              sFlag,
                              STL_FPERM_OS_DEFAULT,
                              KNL_ERROR_STACK(aEnv) ) == STL_SUCCESS );

        sState = 1;

        /**
         * validation checksum
         */
        STL_TRY( stlReadFile( &sFile,
                              (stlChar*) sSysPersData,
                              sSize,
                              &sReadBytes,
                              KNL_ERROR_STACK(aEnv) ) == STL_SUCCESS );
        
        STL_TRY( smfValidateCtrlFileChecksum( &sFile,
                                              sSysPersData,
                                              &sIsValid,
                                              aEnv ) == STL_SUCCESS );

        if( sIsValid == STL_TRUE )
        {
            STL_TRY( smfSaveCtrlFileInternal4Syncher( &sFile,
                                                      sSysPersData,
                                                      aSmrWarmupEntry,
                                                      aEnv ) == STL_SUCCESS );
        }

        sState = 0;
        STL_TRY( stlCloseFile( &sFile,
                               KNL_ERROR_STACK(aEnv) ) == STL_SUCCESS );

        if( sIsValid == STL_TRUE )
        {
            stlSnprintf( sMsg,
                         1024,
                         "[CONTROLFILE] '%s' control file is saved.\n",
                         sWarmupEntry->mCtrlFileName[i] );
        }
        else
        {
            stlSnprintf( sMsg,
                         1024,
                         "[CONTROLFILE] '%s' control file is corrupted.\n",
                         sWarmupEntry->mCtrlFileName[i] );
        }
                     
        if( aLogger != NULL )
        {
            stlLogMsg( aLogger,
                       KNL_ERROR_STACK( aEnv ),
                       sMsg );
        }

        if( aSilent == STL_FALSE )
        {
            stlPrintf( "\n%s", sMsg );
            
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        if( aSilent == STL_FALSE )
        {
            stlPrintf( "\n[CONTROLFILE] Saving control file is failed.\n" );
        }

        stlLogMsg( aLogger,
                   KNL_ERROR_STACK( aEnv ),
                   "[CONTROLFILE] '%s' control file is failed.\n",
                   sWarmupEntry->mCtrlFileName[i] );

        (void) stlCloseFile( &sFile, KNL_ERROR_STACK(aEnv) );
    }

    return STL_FAILURE;
}


stlStatus smfSaveCtrlFileInternal4Syncher( stlFile        * aFile,
                                           smfSysPersData * aSysPersData,
                                           void           * aSmrWarmupEntry,  
                                           smlEnv         * aEnv )
{
    stlSize   sWrittenBytes = 0;
    stlOffset sOffset;

    sOffset = aSysPersData->mLogSectionOffset;

    STL_TRY( stlSeekFile( aFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    STL_TRY( smrWriteLogCtrlSection4Syncher( aFile,
                                             aSmrWarmupEntry,
                                             &sWrittenBytes,
                                             aEnv ) == STL_SUCCESS );

    STL_TRY( smfWriteCtrlFileChecksum( aFile,
                                       aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
