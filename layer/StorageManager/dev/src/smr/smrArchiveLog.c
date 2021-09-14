/*******************************************************************************
 * smrArchiveLog.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smrArchiveLog.c 6951 2013-01-15 06:11:54Z xcom73 $
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
#include <smrDef.h>
#include <smf.h>
#include <smr.h>
#include <smrLogGroup.h>
#include <smrLogFile.h>
#include <smrArchiveLog.h>
#include <smrMediaRecoveryMgr.h>

/**
 * @file smrArchiveLog.c
 * @brief Storage Manager Layer Recovery Log File Internal Routines
 */

extern smrWarmupEntry * gSmrWarmupEntry;

/**
 * @addtogroup smrArchiveLog
 * @{
 */

stlStatus smrValidateLogfileByName( stlChar             * aFileName,
                                    smrLogGroupPersData * aLogGroupPersData,
                                    stlUInt32             aValidationFlag,
                                    stlBool             * aIsValid,
                                    stlBool             * aIsExist,
                                    smlEnv              * aEnv )
{
    stlChar         sBuffer[SMR_MAX_LOG_BLOCK_SIZE + SMF_DIRECT_IO_DEFAULT_BLOCKSIZE];
    smrLogFileHdr * sFileHdr;
    stlInt32        sState = 0;
    stlFile         sFile;
    stlInt32        sFlag;
    stlInt64        sLogIo;

    *aIsValid = STL_TRUE;
    *aIsExist = STL_FALSE;

    sFlag = STL_FOPEN_READ | STL_FOPEN_LARGEFILE;

    sLogIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_LOG_FILE_IO,
                                            KNL_ENV(aEnv) );

    if( sLogIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

    sFileHdr = (smrLogFileHdr *) STL_ALIGN( (stlInt64) sBuffer,
                                            SMF_DIRECT_IO_DEFAULT_BLOCKSIZE );

    /**
     * Logfile이 존재하는지 체크한다.
     */
    STL_TRY( stlExistFile( aFileName,
                           aIsExist,
                           KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    /**
     * Logfile이 존재하지 않으면 skip
     */
    STL_TRY_THROW( *aIsExist == STL_TRUE, RAMP_FINISH );    

    STL_TRY( smgOpenFile( &sFile,
                          aFileName,
                          sFlag,
                          STL_FPERM_OS_DEFAULT,
                          aEnv )
             == STL_SUCCESS );
    sState = 1;

    /**
     * Logfile의 file header를 구한다.
     */
    STL_TRY( stlReadFile( &sFile,
                          sFileHdr,
                          STL_ALIGN( STL_SIZEOF( smrLogFileHdr ), SMR_BLOCK_SIZE ),
                          NULL,
                          KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    sState = 0;
    STL_TRY( smgCloseFile( &sFile,
                           aEnv )
             == STL_SUCCESS );

    if( aValidationFlag & SMR_VALIDATE_LOGFILE_CREATION_TIME )
    {
        if( sFileHdr->mCreationTime <= smrGetDatabaseCreationTime() )
        {
            *aIsValid = STL_FALSE;
        }
    }

    if( aValidationFlag & SMR_VALIDATE_LOGFILE_GROUPID )
    {
        STL_DASSERT( aLogGroupPersData != NULL );

        if( sFileHdr->mLogGroupId != aLogGroupPersData->mLogGroupId )
        {
            *aIsValid = STL_FALSE;
        }
    }
    
    if( aValidationFlag & SMR_VALIDATE_LOGFILE_SEQUENCE )
    {
        STL_DASSERT( aLogGroupPersData != NULL );

        if( sFileHdr->mLogFileSeqNo != aLogGroupPersData->mFileSeqNo )
        {
            *aIsValid = STL_FALSE;
        }
    }
    
    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smgCloseFile( &sFile, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smrValidateArchiveLogfile( stlInt64   aLogFileSeq,
                                     stlChar  * aFileName,
                                     stlBool  * aIsValid,
                                     stlBool  * aIsExist,
                                     smlEnv   * aEnv )
{
    stlChar         sBuffer[SMR_MAX_LOG_BLOCK_SIZE + SMF_DIRECT_IO_DEFAULT_BLOCKSIZE];
    smrLogFileHdr * sFileHdr;
    stlFileInfo     sFileInfo;
    stlInt32        sState = 0;
    stlFile         sFile;
    stlInt32        sFlag;
    stlInt64        sLogIo;

    *aIsValid = STL_FALSE;
    *aIsExist = STL_FALSE;

    sFlag = STL_FOPEN_READ | STL_FOPEN_LARGEFILE;

    sLogIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_LOG_FILE_IO,
                                            KNL_ENV(aEnv) );

    if( sLogIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

    sFileHdr = (smrLogFileHdr *) STL_ALIGN( (stlInt64) sBuffer,
                                            SMF_DIRECT_IO_DEFAULT_BLOCKSIZE );

    STL_DASSERT( aFileName != NULL );

    /**
     * Archive logfile이 존재하는지 체크한다.
     */

    STL_TRY( stlExistFile( aFileName,
                           aIsExist,
                           KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    /**
     * Archive logfile이 존재하지 않으면 skip
     */

    STL_TRY_THROW( *aIsExist == STL_TRUE, SKIP_VALIDATE );

    STL_TRY( smgOpenFile( &sFile,
                          aFileName,
                          sFlag,
                          STL_FPERM_OS_DEFAULT,
                          aEnv )
             == STL_SUCCESS );
    sState = 1;

    /**
     * Archive logfile의 file header를 구한다.
     */
    if( stlReadFile( &sFile,
                     sFileHdr,
                     STL_ALIGN( STL_SIZEOF( smrLogFileHdr ), SMR_BLOCK_SIZE ),
                     NULL,
                     KNL_ERROR_STACK( aEnv ) ) != STL_SUCCESS )
    {
        stlPopError( KNL_ERROR_STACK( aEnv ) );
        
        /**
         * Archive logfile의 file header를 읽지 못했으면 valid하지 않다.
         */
        STL_THROW( SKIP_VALIDATE );
    }

    /**
     * Archive logfile의 file stat을 구한다.
     */
    STL_TRY( stlGetFileStatByHandle( &sFileInfo,
                                     STL_FINFO_SIZE,
                                     &sFile,
                                     KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    /**
     * Logfile Header에 기록된 file size만큼 기록이 되었는지 체크한다.
     */
    if( sFileInfo.mSize != sFileHdr->mFileSize )
    {
        STL_THROW( SKIP_VALIDATE );
    }

    if( (sFileHdr->mLogFileSeqNo == aLogFileSeq) &&
        (sFileHdr->mCreationTime > smrGetDatabaseCreationTime()) )
    {
        *aIsValid = STL_TRUE;
    }

    STL_RAMP( SKIP_VALIDATE );

    if( sState == 1 )
    {
        sState = 0;
        STL_TRY( smgCloseFile( &sFile,
                               aEnv )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smgCloseFile( &sFile, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smrGetValidArchiveLogfileBySeqNo( stlInt64   aLogFileSeq,
                                            stlChar  * aFileName,
                                            stlBool  * aIsValid,
                                            stlBool  * aIsExist,
                                            smlEnv   * aEnv )
{
    stlInt64   sArchivelogDirCount;
    stlInt32   sArchivelogDirId;

    *aIsExist = STL_FALSE;

    /* Media Recovery를 위한 archivelog file이 존재하는 directory의 수를 구한다. */
    sArchivelogDirCount = knlGetPropertyBigIntValueByID(
        KNL_PROPERTY_READABLE_ARCHIVELOG_DIR_COUNT,
        KNL_ENV(aEnv) );

    /* Media Recovery를 위한 archive logfile을 validation한다. */
    for( sArchivelogDirId = 0; sArchivelogDirId < sArchivelogDirCount; sArchivelogDirId++ )
    {
        STL_TRY( smrMakeArchiveLogfileName( aLogFileSeq,
                                            sArchivelogDirId,
                                            aFileName,
                                            aEnv ) == STL_SUCCESS );

        STL_TRY( smrValidateArchiveLogfile( aLogFileSeq,
                                            aFileName,
                                            aIsValid,
                                            aIsExist,
                                            aEnv ) == STL_SUCCESS );

        if( (*aIsExist == STL_TRUE) && (*aIsValid == STL_TRUE) )
        {
            break;
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Log file을 archiving한다.
 * @param[in] aLogStream Archiving을 수행할 Log stream
 * @param[in] aLogGroup Archiving을 수행할 Log group
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smrLogFileArchiving( smrLogStream   * aLogStream,
                               smrLogGroup    * aLogGroup,
                               smlEnv         * aEnv )
{
    smrLogMember    * sLogMember;
    stlChar         * sBuffer;
    stlChar         * sAlignedBuffer;
    stlChar           sDestFileName[STL_MAX_FILE_PATH_LENGTH];
    knlRegionMark     sMemMark;
    stlInt32          sState = 0;
    stlInt32          sBufferSize;
    stlBool           sIsValid;
    stlInt32          sFlag = 0;
    stlInt64          sLogIo;

    sBufferSize = aLogStream->mLogStreamPersData.mBlockSize * ( SMR_BULK_IO_BLOCK_UNIT_COUNT + 1 );

    KNL_INIT_REGION_MARK( &sMemMark );

    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               (knlEnv*)aEnv )
             == STL_SUCCESS );

    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                sBufferSize,
                                (void**)&sBuffer,
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 1;

    sAlignedBuffer = (stlChar*)STL_ALIGN( (stlInt64)sBuffer,
                                          aLogStream->mLogStreamPersData.mBlockSize );

    /* Archive logfile은 첫번째 Archivelog dir에만 만든다. */
    STL_TRY( smrMakeArchiveLogfileName( aLogGroup->mLogGroupPersData.mFileSeqNo,
                                        0,      /* aArchivelogDirId */
                                        sDestFileName,
                                        aEnv ) == STL_SUCCESS );

    sLogMember = STL_RING_GET_FIRST_DATA( &(aLogGroup->mLogMemberList) );

    /**
     * log archiving 도중에 장애가 발생할 경우 이전의 복사가 완료되었을 수도 있고
     * 그렇지 않을 수도 있다.
     * 만약 완료되었을 경우 CDC에서 사용할 수 있기 때문에 다시 복사를 수행하지 않고
     * skip하고, 마지막으로 archive된 logfile sequence number
     * (smrLogPersData.mLastInactiveLfsn)만 변경한다.
     */
    STL_TRY( smrCheckArchiveLogfile( sLogMember->mLogMemberPersData.mName,
                                     sDestFileName,
                                     &sIsValid,
                                     aEnv ) == STL_SUCCESS );

    STL_TRY_THROW( sIsValid == STL_FALSE, SKIP_ARCHIVING );

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "[ARCHIVELOG BEGIN] LOG(%s(%ld)) => ARCHIVE(%s)\n",
                        sLogMember->mLogMemberPersData.mName,
                        aLogGroup->mLogGroupPersData.mFileSeqNo,
                        sDestFileName ) == STL_SUCCESS );

    sFlag = ( STL_FOPEN_CREATE    |
              STL_FOPEN_LARGEFILE |
              STL_FOPEN_TRUNCATE  |
              STL_FOPEN_WRITE );

    sLogIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_LOG_FILE_IO,
                                            KNL_ENV(aEnv) );
        
    if( sLogIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

    sBufferSize = aLogStream->mLogStreamPersData.mBlockSize * SMR_BULK_IO_BLOCK_UNIT_COUNT;
    STL_TRY( stlCopyFile( sLogMember->mLogMemberPersData.mName,
                          sDestFileName,
                          sFlag,
                          STL_FPERM_OS_DEFAULT,
                          sAlignedBuffer,
                          sBufferSize,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    STL_RAMP( SKIP_ARCHIVING );

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_TRUE, /* aFreeChunk */
                                       (knlEnv*)aEnv ) == STL_SUCCESS );

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "[ARCHIVELOG END] (%s) : SUCCESS\n",
                        sDestFileName ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                        &sMemMark,
                                        STL_TRUE, /* aFreeChunk */
                                        (knlEnv*)aEnv );
    }

    (void)knlLogMsg( NULL,
                     KNL_ENV( aEnv ),
                     KNL_LOG_LEVEL_WARNING,
                     "Archiving was failed - %s\n",
                     sDestFileName );

    return STL_FAILURE;
}

/**
 * @brief 불완전 복구 후에 RESETLOGS로 open하는 경우 online log를 archiving 한다.
 * @param[in,out] aEnv invironment info
 */
stlStatus smrArchivingLogFiles4ResetLogs( smlEnv * aEnv )
{
    stlFile           sLogFile;
    smrLogStream    * sLogStream;
    smrLogGroup     * sLogGroup;
    smrLogMember    * sLogMember;
    stlChar           sLogBuffer[SMR_MAX_LOG_BLOCK_SIZE + SMF_DIRECT_IO_DEFAULT_BLOCKSIZE];
    smrLogFileHdr   * sLogFileHdr;
    stlChar         * sBuffer;
    stlChar         * sAlignedBuffer;
    stlChar           sDestFileName[STL_MAX_FILE_PATH_LENGTH];
    knlRegionMark     sMemMark;
    stlInt32          sState = 0;
    stlInt32          sAlignedBufferSize;
    stlInt32          sBufferSize;
    stlSize           sReadBytes;
    stlBool           sIsValid;
    stlInt32          sReadFlag = 0;
    stlInt32          sCopyFlag = 0;
    stlInt64          sLogIo;

    sLogStream = &gSmrWarmupEntry->mLogStream;

    sBufferSize = sLogStream->mLogStreamPersData.mBlockSize * (SMR_BULK_IO_BLOCK_UNIT_COUNT + 1);

    sLogFileHdr = (smrLogFileHdr*)STL_ALIGN( (stlInt64) sLogBuffer,
                                             SMF_DIRECT_IO_DEFAULT_BLOCKSIZE );

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV(aEnv),
                        KNL_LOG_LEVEL_INFO,
                        "[ARCHIVING] start archiving for reset logs\n" ) == STL_SUCCESS );
    
    KNL_INIT_REGION_MARK( &sMemMark );

    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                sBufferSize,
                                (void**)&sBuffer,
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    sAlignedBuffer = (stlChar*)STL_ALIGN( (stlInt64)sBuffer,
                                          sLogStream->mLogStreamPersData.mBlockSize );

    sAlignedBufferSize = sLogStream->mLogStreamPersData.mBlockSize * SMR_BULK_IO_BLOCK_UNIT_COUNT;
    
    sReadFlag = STL_FOPEN_READ | STL_FOPEN_LARGEFILE;

    sCopyFlag = ( STL_FOPEN_CREATE    |
                  STL_FOPEN_LARGEFILE |
                  STL_FOPEN_TRUNCATE  |
                  STL_FOPEN_WRITE );

    sLogIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_LOG_FILE_IO,
                                            KNL_ENV(aEnv) );
        
    if( sLogIo == 0 )
    {
        sReadFlag |= STL_FOPEN_DIRECT;
        sCopyFlag |= STL_FOPEN_DIRECT;
    }

    STL_RING_FOREACH_ENTRY( &(sLogStream->mLogGroupList), sLogGroup )
    {
        sLogMember = STL_RING_GET_FIRST_DATA( &(sLogGroup->mLogMemberList) );
        
        /**
         * Log file에서 Logfile Hdr를 읽고 FileSeqNo를 얻고 UNUSED인지 확인이 필요하다.
         */
        STL_TRY( smgOpenFile( &sLogFile,
                              sLogMember->mLogMemberPersData.mName,
                              sReadFlag,
                              STL_FPERM_OS_DEFAULT,
                              aEnv ) == STL_SUCCESS );
        sState = 2;

        STL_TRY( stlReadFile( &sLogFile,
                              (void*)sLogFileHdr,
                              SMR_MAX_LOG_BLOCK_SIZE,
                              &sReadBytes,
                              KNL_ERROR_STACK(aEnv) ) == STL_SUCCESS );

        STL_TRY_THROW( sLogFileHdr->mLogFileSeqNo != SMR_INVALID_LOG_FILE_SEQ_NO, RAMP_SKIP );

        /* RESETLOGS option으로 database를 open하기 위해 ACTIVE 상태의
         * logfile들을 첫번째 archivelog dir로 archiving한다. */
        STL_TRY( smrMakeArchiveLogfileName( sLogFileHdr->mLogFileSeqNo,
                                            0,    /* aArchivelogDirId */
                                            sDestFileName,
                                            aEnv ) == STL_SUCCESS );

        STL_TRY( smrCheckArchiveLogfile( sLogMember->mLogMemberPersData.mName,
                                         sDestFileName,
                                         &sIsValid,
                                         aEnv ) == STL_SUCCESS );

        STL_TRY_THROW( sIsValid == STL_FALSE, RAMP_SKIP );

        STL_TRY( knlLogMsg( NULL,
                            KNL_ENV( aEnv ),
                            KNL_LOG_LEVEL_INFO,
                            "[ARCHIVELOG BEGIN] LOG(%s(%ld)) => ARCHIVE(%s)\n",
                            sLogMember->mLogMemberPersData.mName,
                            sLogFileHdr->mLogFileSeqNo,
                            sDestFileName ) == STL_SUCCESS );

        STL_TRY( stlCopyFile( sLogMember->mLogMemberPersData.mName,
                              sDestFileName,
                              sCopyFlag,
                              STL_FPERM_OS_DEFAULT,
                              sAlignedBuffer,
                              sAlignedBufferSize,
                              KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
        STL_RAMP( RAMP_SKIP );
        
        sState = 1;
        STL_TRY( smgCloseFile( &sLogFile,
                               aEnv ) == STL_SUCCESS );
    }

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_TRUE, /* aFreeChunk */
                                       KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV(aEnv),
                        KNL_LOG_LEVEL_INFO,
                        "[ARCHIVING] end archiving for reset logs\n" ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)smgCloseFile( &sLogFile,
                                aEnv );
        case 1:
            (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                            &sMemMark,
                                            STL_TRUE, /* aFreeChunk */
                                            KNL_ENV( aEnv ) );
        default:
            (void)knlLogMsg( NULL,
                             KNL_ENV( aEnv ),
                             KNL_LOG_LEVEL_WARNING,
                             "[ARCHIVING] end archiving for reset logs\n",
                             sDestFileName );
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief Logfile을 복사하기 전에 archive 대상의 validation을 체크한다.
 * @param[in] aSource Source file path
 * @param[in] aTarget Target file path
 * @param[out] aIsValid Target logfile의 valid 여부
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smrCheckArchiveLogfile( stlChar   * aSource,
                                  stlChar   * aTarget,
                                  stlBool   * aIsValid,
                                  smlEnv    * aEnv )
{
    stlChar          sSrcBuffer[SMR_MAX_LOG_BLOCK_SIZE + SMF_DIRECT_IO_DEFAULT_BLOCKSIZE];
    stlChar          sDstBuffer[SMR_MAX_LOG_BLOCK_SIZE + SMF_DIRECT_IO_DEFAULT_BLOCKSIZE];
    smrLogFileHdr  * sSrcFileHdr;
    smrLogFileHdr  * sDstFileHdr;
    stlFileInfo      sFileInfo;
    stlFile          sSrcFile;
    stlFile          sDstFile;
    stlInt32         sState = 0;
    stlSize          sReadBytes;
    stlBool          sIsValid = STL_FALSE;
    stlInt32         sFlag;
    stlInt64         sLogIo;

    sFlag = STL_FOPEN_READ | STL_FOPEN_LARGEFILE;

    sLogIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_LOG_FILE_IO,
                                            KNL_ENV(aEnv) );

    if( sLogIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

    sSrcFileHdr = (smrLogFileHdr *) STL_ALIGN( (stlInt64) sSrcBuffer,
                                               SMF_DIRECT_IO_DEFAULT_BLOCKSIZE );
    sDstFileHdr = (smrLogFileHdr *) STL_ALIGN( (stlInt64) sDstBuffer,
                                               SMF_DIRECT_IO_DEFAULT_BLOCKSIZE );

    /**
     * Archive logfile이 존재하는지 체크한다.
     */
    STL_TRY( stlExistFile( aTarget,
                           &sIsValid,
                           KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    /**
     * Archive logfile이 존재하지 않으면 skip
     */
    STL_TRY_THROW( sIsValid == STL_TRUE, SKIP_VALIDATE );

    STL_TRY( smgOpenFile( &sDstFile,
                          aTarget,
                          sFlag,
                          STL_FPERM_OS_DEFAULT,
                          aEnv )
             == STL_SUCCESS );
    sState = 1;

    /**
     * Archive logfile의 file header를 구한다.
     */
    if( stlReadFile( &sDstFile,
                     sDstFileHdr,
                     STL_ALIGN( STL_SIZEOF( smrLogFileHdr ), SMR_BLOCK_SIZE ),
                     &sReadBytes,
                     KNL_ERROR_STACK( aEnv ) ) != STL_SUCCESS )
    {
        while( stlPopError( KNL_ERROR_STACK( aEnv ) ) != NULL )
        {
            /* Do Nothing */
        }

        sIsValid = STL_FALSE;

        STL_THROW( SKIP_VALIDATE );
    }

    if( sReadBytes != STL_SIZEOF( smrLogFileHdr ) )
    {
        sIsValid = STL_FALSE;

        STL_THROW( SKIP_VALIDATE );
    }

    sIsValid = STL_FALSE;

    STL_TRY( smgOpenFile( &sSrcFile,
                          aSource,
                          sFlag,
                          STL_FPERM_OS_DEFAULT,
                          aEnv )
             == STL_SUCCESS );
    sState = 2;

    /**
     * Online logfile의 file header를 구한다.
     */
    STL_TRY( stlReadFile( &sSrcFile,
                          sSrcFileHdr,
                          STL_ALIGN( STL_SIZEOF( smrLogFileHdr ), SMR_BLOCK_SIZE ),
                          NULL,
                          KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    /**
     * Archive logfile의 file stat을 구한다.
     */
    STL_TRY( stlGetFileStatByHandle( &sFileInfo,
                                     STL_FINFO_SIZE,
                                     &sDstFile,
                                     KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    /**
     * Archive logfile의 file stat의 크기가 같고,
     * logfile header의 모든 member가 동일하면 valid하다.
     */
    if( sSrcFileHdr->mFileSize == sFileInfo.mSize )
    {
        if( (sSrcFileHdr->mLogGroupId    == sDstFileHdr->mLogGroupId   ) &&
            (sSrcFileHdr->mBlockSize     == sDstFileHdr->mBlockSize    ) &&
            (sSrcFileHdr->mMaxBlockCount == sDstFileHdr->mMaxBlockCount) &&
            (sSrcFileHdr->mLogFileSeqNo  == sDstFileHdr->mLogFileSeqNo ) &&
            (sSrcFileHdr->mFileSize      == sDstFileHdr->mFileSize     ) &&
            (sSrcFileHdr->mPrevLastLsn   == sDstFileHdr->mPrevLastLsn  ) &&
            (sSrcFileHdr->mCreationTime  == sDstFileHdr->mCreationTime ) )
        {
            sIsValid = STL_TRUE;
        }
    }

    sState = 1;
    STL_TRY( smgCloseFile( &sSrcFile, aEnv ) == STL_SUCCESS );

    STL_RAMP( SKIP_VALIDATE );

    if( sState == 1 )
    {
        sState = 0;
        STL_TRY( smgCloseFile( &sDstFile, aEnv ) == STL_SUCCESS );
    }

    *aIsValid = sIsValid;
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)smgCloseFile( &sSrcFile, aEnv );
        case 1:
            (void)smgCloseFile( &sDstFile, aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief Archive logfile이 존재하는지 체크하고 open한다.
 * @param[in]  aFileName Logfile name
 * @param[out] aValidFile Logfile의 유효성 여부
 * @param[out] aExistFile Logfile 존재 여부
 * @param[out] aFile      Logfile 파일 포인터
 * @param[in,out] aEnv    Environment 구조체
 */
stlStatus smrOpenFile4MediaRecovery( stlChar  * aFileName,
                                     stlBool  * aValidFile,
                                     stlBool  * aExistFile,
                                     stlFile  * aFile,
                                     smlEnv   * aEnv )
{
    stlInt32   sFlag = 0;
    stlInt64   sLogIo;

    STL_TRY( smrValidateLogfileByName( aFileName,
                                       NULL,
                                       SMR_VALIDATE_LOGFILE_CREATION_TIME,
                                       aValidFile,
                                       aExistFile,
                                       aEnv ) == STL_SUCCESS );

    if( (*aExistFile == STL_TRUE) && (*aValidFile == STL_TRUE) )
    {
        sFlag = (STL_FOPEN_WRITE | STL_FOPEN_READ | STL_FOPEN_LARGEFILE);
        
        sLogIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_LOG_FILE_IO,
                                                KNL_ENV(aEnv) );
        
        if( sLogIo == 0 )
        {
            sFlag |= STL_FOPEN_DIRECT;
        }

        STL_TRY( smgOpenFile( aFile,
                              aFileName,
                              sFlag,
                              STL_FPERM_OS_DEFAULT,
                              aEnv )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Archivelog file이 존재하는지 체크하고 open한다.
 * @param[in]  aFileSeqNo Logfile sequence no
 * @param[out] aValidFile Logfile의 유효성 여부
 * @param[out] aExistFile Logfile 존재 여부
 * @param[out] aFile      Logfile 파일 포인터
 * @param[in,out] aEnv    Environment 구조체
 */
stlStatus smrOpenArchiveLogfile( stlInt64   aFileSeqNo,
                                 stlBool  * aValidFile,
                                 stlBool  * aExistFile,
                                 stlFile  * aFile,
                                 smlEnv   * aEnv )
{
    stlInt32   sFlag = 0;
    stlInt64   sLogIo;
    stlInt32   sArchivelogDirId;
    stlInt64   sArchivelogDirCount;

    /* Media Recovery를 위한 archivelog file이 존재하는 directory의 수를 구한다. */
    sArchivelogDirCount = knlGetPropertyBigIntValueByID(
        KNL_PROPERTY_READABLE_ARCHIVELOG_DIR_COUNT,
        KNL_ENV(aEnv) );

    /**
     * Recovery를 위해 처음 읽는 logfile 혹은, 다음 logfile을 위해
     * logfile의 이름을 생성한다.
     */
    if( smrIsLogfileCrossed( aFileSeqNo, aEnv ) != STL_TRUE )
    {
        STL_TRY( smrValidateArchiveLogfile( aFileSeqNo,
                                            SMR_MEDIA_RECOVERY_GET_FIELD(aEnv, LogfileName),
                                            aValidFile,
                                            aExistFile,
                                            aEnv ) == STL_SUCCESS );

        STL_TRY_THROW( ((*aExistFile != STL_TRUE) || (*aValidFile != STL_TRUE)),
                       RAMP_VALID_LOGFILE );
    }

    /* Media Recovery를 위한 다음 archive logfile을 validation한다. */
    for( sArchivelogDirId = 0; sArchivelogDirId < sArchivelogDirCount; sArchivelogDirId++ )
    {
        STL_TRY( smrMakeArchiveLogfileName( aFileSeqNo,
                                            sArchivelogDirId,
                                            SMR_MEDIA_RECOVERY_GET_FIELD(aEnv, LogfileName),
                                            aEnv ) == STL_SUCCESS );

        STL_TRY( smrValidateArchiveLogfile( aFileSeqNo,
                                            SMR_MEDIA_RECOVERY_GET_FIELD(aEnv, LogfileName),
                                            aValidFile,
                                            aExistFile,
                                            aEnv ) == STL_SUCCESS );

        if( (*aExistFile == STL_TRUE) && (*aValidFile == STL_TRUE) )
        {
            break;
        }
    }

    STL_RAMP( RAMP_VALID_LOGFILE );

    if( (*aExistFile == STL_TRUE) && (*aValidFile == STL_TRUE) )
    {
        sFlag = (STL_FOPEN_WRITE | STL_FOPEN_READ | STL_FOPEN_LARGEFILE);
        
        sLogIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_LOG_FILE_IO,
                                                KNL_ENV(aEnv) );
        
        if( sLogIo == 0 )
        {
            sFlag |= STL_FOPEN_DIRECT;
        }

        STL_TRY( smgOpenFile( aFile,
                              SMR_MEDIA_RECOVERY_GET_FIELD(aEnv, LogfileName),
                              sFlag,
                              STL_FPERM_OS_DEFAULT,
                              aEnv )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Archive log file에서 필요한 log를 읽는다.
 * @param[in] aLid read할 log의 lid
 * @param[in] aBuffer 읽은 log를 채울 공간
 * @param[out] aReadBytes 
 * @param[out] aEndOfFile
 * @param[out] aEndOfLog
 * @param[in,out] aEnv 
 */
stlStatus smrReadArchiveFileBlocks( smrLid         * aLid,
                                    stlChar        * aBuffer,
                                    stlInt64       * aReadBytes,
                                    stlBool        * aEndOfFile,
                                    stlBool        * aEndOfLog,
                                    smlEnv         * aEnv )
{
    smrLogBuffer   * sLogBuffer;
    stlChar          sBuffer[SMR_MAX_LOG_BLOCK_SIZE + SMF_DIRECT_IO_DEFAULT_BLOCKSIZE];
    smrLogFileHdr  * sLogFileHdr;
    smrLogBlockHdr   sLogBlockHdr;
    stlFile          sFile;
    stlInt64         sMaxBlockCount;
    stlInt32         sBlockSeq;
    stlInt16         sBlockSize;
    stlBool          sValid;
    stlBool          sExist;
    stlInt64         sBlockCount;
    stlInt64         i;
    stlInt64         sBufferSize;
    stlInt64         sFileSize;
    stlInt32         sState = 0;
    stlSize          sTotalReadBytes = 0;
    stlOffset        sOffset;
    stlInt64         sReadBytes = 0;
    smrLsn           sPrevLastLsn;

    sLogBuffer = (smrLogBuffer *)SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, LogBuffer );
    sBlockSize = SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, BlockSize );

    *aReadBytes = 0;
    *aEndOfFile = STL_FALSE;
    *aEndOfLog  = STL_FALSE; 

    if( smrGetImrOption(aEnv) == SML_IMR_OPTION_INTERACTIVE )
    {
        STL_TRY( smrOpenFile4MediaRecovery( SMR_MEDIA_RECOVERY_GET_FIELD(aEnv, LogfileName),
                                            &sValid,
                                            &sExist,
                                            &sFile,
                                            aEnv ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( smrOpenArchiveLogfile( SMR_FILE_SEQ_NO( aLid ),
                                        &sValid,
                                        &sExist,
                                        &sFile,
                                        aEnv ) == STL_SUCCESS );
    }

    STL_TRY_THROW( sExist == STL_TRUE, RAMP_ERR_ARCHIVELOG_FILE_NOT_EXIST );
    STL_TRY_THROW( sValid == STL_TRUE, RAMP_ERR_ARCHIVELOG_FILE_NOT_VALID );
    sState = 1;

    /**
     * 처음 읽은 file이면 file size, block size, max block count등을 저장한다.
     */
    if( smrIsLogfileCrossed(SMR_FILE_SEQ_NO(aLid), aEnv) == STL_TRUE )
    {
        sLogFileHdr = (smrLogFileHdr*)STL_ALIGN( (stlInt64)sBuffer,
                                                 SMF_DIRECT_IO_DEFAULT_BLOCKSIZE );

        /**
         * Log group마다 file 크기가 다를 수 있기 때문에 log file을 처음 읽을 때
         * log file header에 기록된 file size를 설정한다. 
         */
        STL_TRY( stlReadFile( &sFile,
                              (void *)sLogFileHdr,
                              SMR_MAX_LOG_BLOCK_SIZE,
                              NULL,
                              KNL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );

        sLogBuffer->mFileSeqNo = SMR_FILE_SEQ_NO( aLid );
        SMR_MEDIA_RECOVERY_SET_FIELD( aEnv,
                                      PrevLastLsn,
                                      sLogFileHdr->mPrevLastLsn );

        SMR_MEDIA_RECOVERY_SET_FIELD( aEnv,
                                      MaxBlockCount,
                                      sLogFileHdr->mMaxBlockCount );
    }

    sPrevLastLsn = SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, PrevLastLsn );

    sMaxBlockCount = SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, MaxBlockCount );

    /**
     * 파일에서 aLid의 위치의 Block Seq No부터 aBuffer에 올린다.
     */
    sBlockSeq = SMR_BLOCK_SEQ_NO( aLid );    
    sOffset = SMR_BLOCK_SEQ_FILE_OFFSET( sBlockSeq, sBlockSize );
    sFileSize = sMaxBlockCount * sBlockSize;

    /**
     * Logfile의 끝까지 읽었으면 EOF return
     */
    if( sFileSize == sOffset )
    {
        *aEndOfFile = STL_TRUE;
        STL_THROW( SKIP_READ_FILE_BLOCKS );
    }

    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    /**
     * 읽을수 있는 버퍼의 크기는 파일 사이즈를 초과할수 없다.
     */
    sBufferSize = STL_MIN( sLogBuffer->mBufferSize, (sFileSize - sOffset) );
    STL_ASSERT( sBufferSize > 0 );
    
    STL_TRY( stlReadFile( &sFile,
                          aBuffer,
                          sBufferSize,
                          &sTotalReadBytes,
                          KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    sBlockCount = sTotalReadBytes / sBlockSize;

    /**
     * media recovery를 수행해야 하는 마지막 Lsn보다
     * 읽은 Block의 Lsn이 크면 aEndOfLog 설정
     */
    for( i = 0; i < sBlockCount; i++ )
    {
        SMR_READ_LOG_BLOCK_HDR( &sLogBlockHdr, aBuffer + ( sBlockSize * i ) );

        if( SMR_IS_DUMMY_FILE_END_BLOCK( &sLogBlockHdr ) )
        {
            *aEndOfFile = STL_TRUE;
            break;
        }

        /**
         * archive logfile은 마지막으로 읽은 log의 Lsn보다 log block의 Lsn이 작을 수 없다.
         * 하지만 마지막 log block을 읽은 후 logfile을 읽었을 때 이전 logfile의 log block이
         * 남아 있을 수 있으므로 EOF를 return한다.
         */
        if( sPrevLastLsn > sLogBlockHdr.mLsn )
        {
            *aEndOfFile = STL_TRUE;
            break;
        }

        sReadBytes += sBlockSize;

        /**
         * logfile의 마지막 log block이면 buffer의 나머지 log block은 버린다.
         */
        if( SMR_IS_LAST_FILE_BLOCK( &sLogBlockHdr ) )
        {
            break;
        }
    }

    STL_RAMP( SKIP_READ_FILE_BLOCKS );
    
    *aReadBytes = sReadBytes;

    sState = 0;
    STL_TRY( smgCloseFile( &sFile,
                           aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_ARCHIVELOG_FILE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_LOGFILE_NOT_EXIST,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, LogfileName ) );
    }

    STL_CATCH( RAMP_ERR_ARCHIVELOG_FILE_NOT_VALID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_LOGFILE_NOT_VALID,
                      "logfile header was corrupted",
                      KNL_ERROR_STACK(aEnv),
                      SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, LogfileName ) );
    }

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smgCloseFile( &sFile, aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief Media Recover를 위한 첫번째 log를 찾기 위해 archive logfile을 scan한다.
 * @param[in] aFileSeqNo 첫번째 log를 찾기 위한 마지막 archive logfile의 file sequence no
 * @param[in] aTargetLsn 첫번째 log의 Lsn
 * @param[in] aBuffer logfile을 scan하기 위한 buffer
 * @param[in] aBufferSize logfile을 scan하기 위한 buffer size
 * @param[out] aRedoLid Redo를 시작할 Log Lid
 * @param[out] aExist Target Lsn의 존재 여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smrFindArchiveFileBlockByLsn( stlInt64   aFileSeqNo,
                                        smrLsn     aTargetLsn,
                                        stlChar  * aBuffer,
                                        stlInt64   aBufferSize,
                                        smrLid   * aRedoLid,
                                        stlBool  * aExist,
                                        smlEnv   * aEnv )
{
    stlChar          sBuffer[SMR_MAX_LOG_BLOCK_SIZE + SMF_DIRECT_IO_DEFAULT_BLOCKSIZE];
    smrLogBlockHdr   sLogBlockHdr;
    smrLogFileHdr  * sLogFileHdr;
    stlInt64         sFileSeqNo;
    stlInt32         sState = 0;
    stlInt32         sBlockSeq = 0;
    stlFile          sFile;
    stlInt16         sBlockSize;
    stlOffset        sOffset;
    stlBool          sValid;
    stlBool          sExist;
    stlInt64         sMaxBlockCount;
    stlInt64         sBlockCount;
    stlInt64         sBufferSize;
    stlInt64         i;
    smrLsn           sPrevLsn;

    *aExist = STL_FALSE;

    sLogFileHdr = (smrLogFileHdr*)STL_ALIGN( (stlInt64)sBuffer,
                                             SMF_DIRECT_IO_DEFAULT_BLOCKSIZE );

    /**
     * Logfile 기반의 incomplete media recovery의 경우
     * 주어진 logfile에서 recovery Lsn을 찾지 못할 경우 실패
     */
    if( smrGetImrOption(aEnv) == SML_IMR_OPTION_INTERACTIVE )
    {
        STL_TRY( smrOpenFile4MediaRecovery( SMR_MEDIA_RECOVERY_GET_FIELD(aEnv, LogfileName),
                                            &sValid,
                                            &sExist,
                                            &sFile,
                                            aEnv ) == STL_SUCCESS );
        
        STL_TRY_THROW( sExist == STL_TRUE, RAMP_ERR_ARCHIVELOG_FILE_NOT_EXIST );
        STL_TRY_THROW( sValid == STL_TRUE, RAMP_ERR_ARCHIVELOG_FILE_NOT_VALID );
        sState = 1;

        STL_TRY( stlReadFile( &sFile,
                              (void*)sLogFileHdr,
                              SMR_MAX_LOG_BLOCK_SIZE,
                              NULL,
                              KNL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );

        sBlockSize = sLogFileHdr->mBlockSize;
        sMaxBlockCount = sLogFileHdr->mMaxBlockCount;

        STL_TRY_THROW( aTargetLsn > sLogFileHdr->mPrevLastLsn,
                       RAMP_ERR_NOT_APPROPRIATE_LOGFILE );
                       
    }
    else
    {
        /**
         * media recover를 위한 첫번째 log가 archiving되었을 경우
         * archive logfile을 찾는다.
         */
        sFileSeqNo = aFileSeqNo;

        while( 1 )
        {
            STL_TRY_THROW( sFileSeqNo >= 0, RAMP_ERR_ARCHIVELOG_FILE_NOT_EXIST );

            STL_TRY( smrOpenArchiveLogfile( sFileSeqNo,
                                            &sValid,
                                            &sExist,
                                            &sFile,
                                            aEnv ) == STL_SUCCESS );

            STL_TRY_THROW( sExist == STL_TRUE, RAMP_ERR_ARCHIVELOG_FILE_NOT_EXIST );
            STL_TRY_THROW( sValid == STL_TRUE, RAMP_ERR_ARCHIVELOG_FILE_NOT_VALID );
            sState = 1;

            STL_TRY( stlReadFile( &sFile,
                                  (void*)sLogFileHdr,
                                  SMR_MAX_LOG_BLOCK_SIZE,
                                  NULL,
                                  KNL_ERROR_STACK( aEnv ) )
                     == STL_SUCCESS );

            sBlockSize = sLogFileHdr->mBlockSize;
            sMaxBlockCount = sLogFileHdr->mMaxBlockCount;

            if( aTargetLsn > sLogFileHdr->mPrevLastLsn )
            {
                break;
            }

            sState = 0;
            STL_TRY( smgCloseFile( &sFile,
                                   aEnv )
                     == STL_SUCCESS );

            sFileSeqNo--;
        }
    }

    sExist = STL_FALSE;
    sPrevLsn = sLogFileHdr->mPrevLastLsn;

    while( sBlockSeq < sMaxBlockCount - 1 )
    {
        sOffset = SMR_BLOCK_SEQ_FILE_OFFSET( sBlockSeq, sBlockSize );

        STL_TRY( stlSeekFile( &sFile,
                              STL_FSEEK_SET,
                              &sOffset,
                              KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

        sBufferSize = STL_MIN( aBufferSize,
                               (sLogFileHdr->mFileSize - sOffset) );
        STL_ASSERT( sBufferSize > 0 );

        STL_TRY( stlReadFile( &sFile,
                              (void*)aBuffer,
                              sBufferSize,
                              NULL,
                              KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

        sBlockCount = sBufferSize / sBlockSize;
        for( i = 0; i < sBlockCount; i++ )
        {
            SMR_READ_LOG_BLOCK_HDR( &sLogBlockHdr, aBuffer + (i * sBlockSize) );

            SMR_MAKE_LID( aRedoLid,
                          sLogFileHdr->mLogFileSeqNo,
                          sBlockSeq,
                          0 );

            if( (sLogBlockHdr.mLsn < sPrevLsn) ||
                (sLogBlockHdr.mLsn == SMR_INIT_LSN) )
            {
                STL_THROW( RAMP_SUCCESS );
            }

            if( sLogBlockHdr.mLsn >= aTargetLsn )
            {
                sExist = STL_TRUE;
                STL_THROW( RAMP_SUCCESS );
            }

            sBlockSeq++;
        }
    }

    STL_RAMP( RAMP_SUCCESS );

    sState = 0;
    STL_TRY( smgCloseFile( &sFile,
                           aEnv )
             == STL_SUCCESS );

    *aExist = sExist;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_ARCHIVELOG_FILE_NOT_EXIST )
    {
        (void)smrMakeArchiveLogfileName( aFileSeqNo,
                                         0,
                                         SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, LogfileName ),
                                         aEnv );

        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_LOGFILE_NOT_EXIST,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, LogfileName ) );
    }

    STL_CATCH( RAMP_ERR_ARCHIVELOG_FILE_NOT_VALID )
    {
        (void)smrMakeArchiveLogfileName( aFileSeqNo,
                                         0,
                                         SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, LogfileName ),
                                         aEnv );

        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_LOGFILE_NOT_VALID,
                      "logfile header was corrupted",
                      KNL_ERROR_STACK(aEnv),
                      SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, LogfileName ) );
    }

    STL_CATCH( RAMP_ERR_NOT_APPROPRIATE_LOGFILE )
    {
        (void)smrMakeArchiveLogfileName( aFileSeqNo,
                                         0,
                                         SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, LogfileName ),
                                         aEnv );

        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_NOT_APPROPRIATE_LOGFILE,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      SMR_MEDIA_RECOVERY_GET_FIELD(aEnv, LogfileName) );
    }

    STL_FINISH;

    switch( sState )
    {
        case 1 :
            (void)smgCloseFile( &sFile, aEnv );
        default :
            break;
    }

    return STL_FAILURE;
}

stlStatus smrMakeArchiveLogfileName( stlInt64   aFileSeqNo,
                                     stlInt32   aArchivelogDirId,
                                     stlChar  * aFileName,
                                     smlEnv   * aEnv )
{
    stlChar    sArchiveLogFile[KNL_PROPERTY_STRING_MAX_LENGTH];
    stlChar    sPropertyName[KNL_PROPERTY_STRING_MAX_LENGTH];
    stlChar    sLogfilePath[STL_MAX_FILE_PATH_LENGTH];

    /* ARCHIVELOG_DIR_# 에서 archive logfile을 찾는다. */
    stlSnprintf( sPropertyName,
                 KNL_PROPERTY_STRING_MAX_LENGTH,
                 "ARCHIVELOG_DIR_%d",
                 aArchivelogDirId + 1 );

    STL_TRY( knlGetPropertyValueByName( sPropertyName,
                                        sLogfilePath,
                                        KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_ARCHIVELOG_FILE,
                                      sArchiveLogFile,
                                      KNL_ENV( aEnv ) ) == STL_SUCCESS );

    stlSnprintf( aFileName,
                 STL_MAX_FILE_PATH_LENGTH,
                 "%s" STL_PATH_SEPARATOR "%s_%ld.log",
                 sLogfilePath,
                 sArchiveLogFile,
                 aFileSeqNo );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Incomplete Media Recovery 시 사용자로 부터 recovery를 위한 logfile을 입력받는다.
 * @param[in] aCurLsn Media Recovery를 시작할 Log의 Lsn
 * @param[in] aFileSeqNo Media Recovery를 시작할 Logfile의 seq no
 * @param[out] aExistFile Media Recovery를 시작할 Logfile의 의 존재 여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smrGetLogfile( smrLsn     aCurLsn,
                         stlInt64   aFileSeqNo,
                         stlBool  * aExistFile,
                         smlEnv   * aEnv )
{
    stlInt64   sArchivelogDirCount;
    stlInt32   sArchivelogDirId;
    stlBool    sValidFile;
    stlBool    sExistFile;

    *aExistFile = STL_FALSE;

    STL_TRY_THROW( smrGetImrOption(aEnv) == SML_IMR_OPTION_INTERACTIVE,
                   RAMP_ERR_INTERNAL );

    /* Media Recovery를 위한 archivelog file이 존재하는 directory의 수를 구한다. */
    sArchivelogDirCount = knlGetPropertyBigIntValueByID(
        KNL_PROPERTY_READABLE_ARCHIVELOG_DIR_COUNT,
        KNL_ENV(aEnv) );

    switch( smrGetImrCondition(aEnv) )
    {
        case SML_IMR_CONDITION_MANUAL:
            if( smrValidateArchiveLogfile( aFileSeqNo,
                                           SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, LogfileName ),
                                           &sValidFile,
                                           &sExistFile,
                                           aEnv ) == STL_SUCCESS )
            {
                if( (sExistFile == STL_TRUE) && (sValidFile == STL_TRUE) )
                {
                    *aExistFile = STL_TRUE;
                }
            }

            break;

        case SML_IMR_CONDITION_SUGGESTION:
            /**
             * SUGGESTION 과 AUTO 는 media recovery에 필요한 logfile을
             * 한건씩 처리하거나 존재하는 logfile은 모두 처리하느냐의 차이이다.
             * 따라서 SUGGESTION 을 선택했을 경우 USER 로 설정한 후 media recovery에
             * 필요한 logfile을 찾고 valid하면 redo를 수행한다. 그리고 SUGGESTION으로
             * 하나의 logfile을 redo한 후 새로운 logfile을 입력받는다.
             */
            smrSetIncompleteRecoveryCondition( SML_IMR_CONDITION_MANUAL, aEnv );

        case SML_IMR_CONDITION_AUTO:
            /**
             * 불완전 복구 시 필요한 archive logfile이 존재하면
             * 입력받지 않고 복구 수행
             */
            for( sArchivelogDirId = 0; sArchivelogDirId < sArchivelogDirCount; sArchivelogDirId++ )
            {
                STL_TRY( smrMakeArchiveLogfileName( aFileSeqNo,
                                                    sArchivelogDirId,
                                                    SMR_MEDIA_RECOVERY_GET_FIELD(aEnv, LogfileName),
                                                    aEnv ) == STL_SUCCESS );

                STL_TRY( smrValidateArchiveLogfile( aFileSeqNo,
                                                    SMR_MEDIA_RECOVERY_GET_FIELD(aEnv, LogfileName),
                                                    &sValidFile,
                                                    &sExistFile,
                                                    aEnv ) == STL_SUCCESS );

                if( (sExistFile == STL_TRUE) && (sValidFile == STL_TRUE) )
                {
                    *aExistFile = STL_TRUE;
                    break;
                }
            }

            break;

        default:
            break;
    }

    /* Logfile을 찾지 못하였으면 다음 Logfile에 대한 warning info와 함께
     * redo를 종료하고, 다음 incomplete recovery 구문을 수행하도록 한다. */
    if( *aExistFile == STL_FALSE )
    {
        STL_TRY( smrPushWarning4IncompleteRecovery( aCurLsn,
                                                    aFileSeqNo,
                                                    aEnv )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INTERNAL,
                      "- smrGetLogfile()",
                      KNL_ERROR_STACK(aEnv) );

        KNL_SYSTEM_FATAL_DASSERT( STL_FALSE, KNL_ENV(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Incomplete Media Recovery 시 복구를 위해 필요한 Logfile을 입력받기 위해 warning을 push한다.
 * @param[in] aSuggestLsn 복구를 위해 suggest할 다음 Lsn
 * @param[in] aSuggestFileSeqNo 복구를 위해 suggest할 다음 Logfile의 seq no
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smrPushWarning4IncompleteRecovery( smrLsn     aSuggestLsn,
                                             stlInt64   aSuggestFileSeqNo,
                                             smlEnv   * aEnv )
{
    stlChar    sFileName[STL_MAX_FILE_PATH_LENGTH];

    /* Interactive incomplete media recovery를 위한 suggestion logfile을
     * 첫번째 archivelog dir을 이용하여 file name 을 생성한다. */
    STL_TRY( smrMakeArchiveLogfileName( aSuggestFileSeqNo,
                                        0,      /* aArchivelogDirId */
                                        sFileName,
                                        aEnv ) == STL_SUCCESS );

    /**
     * Recovery를 위해 필요한 archive logfile을 제안한다.
     */
    stlPushError( STL_ERROR_LEVEL_WARNING,
                  SML_ERRCODE_WARNING_NEED_SPECIFIC_ARCHIVED_LOGFILE,
                  NULL,
                  KNL_ERROR_STACK(aEnv),
                  aSuggestLsn );

    stlPushError( STL_ERROR_LEVEL_WARNING,
                  SML_ERRCODE_WARNING_SUGGESTED_ARCHIVED_LOGFILE,
                  NULL,
                  KNL_ERROR_STACK(aEnv),
                  sFileName );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlBool smrIsLogfileCrossed( stlInt64   aFileSeqNo,
                             smlEnv   * aEnv )
{
    smrLogBuffer * sLogBuffer;

    sLogBuffer = (smrLogBuffer *)SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, LogBuffer );

    return (aFileSeqNo != sLogBuffer->mFileSeqNo);
}

/** @} */
