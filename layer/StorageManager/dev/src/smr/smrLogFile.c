/*******************************************************************************
 * smrLogFile.c
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
#include <smg.h>
#include <smrDef.h>
#include <smf.h>
#include <smr.h>
#include <smrLogFile.h>
#include <smrLogGroup.h>
#include <smrLogMirror.h>
#include <smrArchiveLog.h>
#include <smrLogDump.h>

/**
 * @file smrLogFile.c
 * @brief Storage Manager Layer Recovery Log File Internal Routines
 */

extern smrWarmupEntry * gSmrWarmupEntry;

/**
 * @addtogroup smrLogFile
 * @{
 */

stlStatus smrFormatLogFile( stlChar  * aLogFileName,
                            stlInt64   aLogFileSize,
                            stlInt16   aBlockSize,
                            stlBool    aIsReuseFile,
                            smlEnv   * aEnv )
{
    stlFile          sFile;
    stlUInt32        sState = 0;
    stlInt64         sMaxBlockCount;
    stlChar        * sLogBlock;
    stlChar        * sAlignedLogBlock;
    smrLogFileHdr  * sLogFileHdr;
    smrLogBlockHdr   sLogBlockHdr;
    stlInt32         sBlockSeq;
    stlOffset        sOffset;
    stlInt32         sFlushBlockCount;
    stlInt32         sIdx;
    knlRegionMark    sMemMark;
    stlBool          sFileExist = STL_FALSE;
    stlFileInfo      sFileInfo;
    stlInt32         sFlag = 0;
    stlInt64         sLogIo;

    STL_TRY( stlExistFile( aLogFileName,
                           &sFileExist,
                           KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    sLogIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_LOG_FILE_IO,
                                            KNL_ENV(aEnv) );

    if( sFileExist == STL_TRUE )
    {
        STL_TRY_THROW( aIsReuseFile == STL_TRUE, RAMP_ERR_EXISTS_FILE  );

        STL_TRY( stlGetFileStatByName( &sFileInfo,
                                       aLogFileName,
                                       STL_FINFO_SIZE,
                                       KNL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );

        sFlag = (STL_FOPEN_WRITE | STL_FOPEN_LARGEFILE);

        if( sLogIo == 0 )
        {
            sFlag |= STL_FOPEN_DIRECT;
        }

        STL_TRY( smgOpenFile( &sFile,
                              aLogFileName,
                              sFlag,
                              STL_FPERM_OS_DEFAULT,
                              aEnv )
                 == STL_SUCCESS );
        sState = 1;

        if( sFileInfo.mSize > aLogFileSize )
        {
            STL_TRY( stlTruncateFile( &sFile,
                                      aLogFileSize,
                                      KNL_ERROR_STACK(aEnv) )
                     == STL_SUCCESS );
        }
    }
    else
    {
        sFlag = (STL_FOPEN_CREATE | STL_FOPEN_WRITE | STL_FOPEN_LARGEFILE);

        if( sLogIo == 0 )
        {
            sFlag |= STL_FOPEN_DIRECT;
        }

        STL_TRY( smgOpenFile( &sFile,
                              aLogFileName,
                              sFlag,
                              STL_FPERM_OS_DEFAULT,
                              aEnv )
                 == STL_SUCCESS );
        sState = 1;
    }


    KNL_INIT_REGION_MARK( &sMemMark );

    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               (knlEnv*)aEnv )
             == STL_SUCCESS );

    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                aBlockSize * ( SMR_BULK_IO_BLOCK_UNIT_COUNT + 1 ),
                                (void**)&sLogBlock,
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 2;

    sAlignedLogBlock = (stlChar*)STL_ALIGN( (stlInt64)sLogBlock,
                                            aBlockSize );
    stlMemset( sAlignedLogBlock, 0x00, SMR_BULK_IO_BLOCK_UNIT_COUNT * aBlockSize );

    /*
     * Log File Header 설정
     */
    sMaxBlockCount = aLogFileSize / aBlockSize;

    sLogFileHdr                 = (smrLogFileHdr*)sAlignedLogBlock;
    sLogFileHdr->mBlockSize     = aBlockSize;
    sLogFileHdr->mMaxBlockCount = sMaxBlockCount;
    sLogFileHdr->mLogFileSeqNo  = SMR_INVALID_LOG_FILE_SEQ_NO;
    sLogFileHdr->mLogGroupId    = -1;
    sLogFileHdr->mFileSize      = aLogFileSize;
    sLogFileHdr->mPrevLastLsn   = SMR_INVALID_LSN;
    sLogFileHdr->mCreationTime  = stlNow();

    STL_TRY( stlWriteFile( &sFile,
                           sAlignedLogBlock,
                           aBlockSize,
                           NULL,
                           KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    stlMemset( sAlignedLogBlock, 0x00, aBlockSize );

    /*
     * Log Block Header 설정
     */
    stlMemset( &sLogBlockHdr, 0x00, STL_SIZEOF( smrLogBlockHdr ) );
    sLogBlockHdr.mLsn = SMR_INIT_LSN;
    sLogBlockHdr.mLogCount = 0;
    sLogBlockHdr.mFirstLogOffset = 0;
    SMR_SET_BLOCK_INFO( &sLogBlockHdr,
                        SMR_FILE_END_FALSE,
                        SMR_CHAINED_BLOCK_FALSE,
                        SMR_INVALID_WRAP_NO,
                        SMR_BLOCK_WRITE_FINISH );

    for( sIdx = 0; sIdx < SMR_BULK_IO_BLOCK_UNIT_COUNT; sIdx++ )
    {
        SMR_WRITE_LOG_BLOCK_HDR( sAlignedLogBlock + (aBlockSize * sIdx),
                                 &sLogBlockHdr );
    }

    sBlockSeq = 0;
    sMaxBlockCount--;
    while( sBlockSeq < sMaxBlockCount )
    {
        sOffset = SMR_BLOCK_SEQ_FILE_OFFSET( sBlockSeq, aBlockSize );

        STL_TRY( stlSeekFile( &sFile,
                              STL_FSEEK_SET,
                              &sOffset,
                              KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

        if( ( sMaxBlockCount - sBlockSeq ) >= SMR_BULK_IO_BLOCK_UNIT_COUNT )
        {
            sFlushBlockCount = SMR_BULK_IO_BLOCK_UNIT_COUNT;
        }
        else
        {
            sFlushBlockCount = sMaxBlockCount - sBlockSeq;
        }

        STL_TRY( stlWriteFile( &sFile,
                               sAlignedLogBlock,
                               aBlockSize * sFlushBlockCount,
                               NULL,
                               KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

        sBlockSeq += sFlushBlockCount;
    }

    sState = 1;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_TRUE, /* aFreeChunk */
                                       (knlEnv*)aEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( smgCloseFile( &sFile,
                           aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_EXISTS_FILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_ALREADY_EXIST_FILE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      aLogFileName );
    }
 
    STL_FINISH;

    switch( sState )
    {
        case 2 :
            (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                            &sMemMark,
                                            STL_TRUE, /* aFreeChunk */
                                            (knlEnv*)aEnv );
        case 1 :
            (void)smgCloseFile( &sFile, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

/*
 * 주어진 aTargetLsn보다 같거나 큰 lsn 중 가장 작은 lsn을 갖는 log block을 반환한다.
 */
stlStatus smrFindFileBlock( smrLid              * aHintLid,
                            smrLsn                aTargetLsn,
                            stlChar             * aBuffer,
                            stlInt64              aBufferSize,
                            smrLid              * aTargetLid,
                            stlBool             * aFound,
                            smlEnv              * aEnv )
{
    smrLogStream   * sLogStream;
    smrLogGroup    * sCurLogGroup;
    smrLogGroup    * sStartLogGroup;
    smrLogMember   * sLogMember;
    smrLsn           sBlockLsn;
    stlInt16         sBlockSize;
    stlInt32         sState = 0;
    stlFile          sFile;
    stlInt32         sBlockSeq = SMR_BLOCK_SEQ_NO( aHintLid );
    stlInt64         sBlockCount;
    stlInt64         sReadSize;
    stlInt64         i;
    stlOffset        sOffset;
    stlInt32         sFlag = 0;
    stlInt64         sLogIo;

    *aFound = STL_FALSE;
    
    sLogStream = &gSmrWarmupEntry->mLogStream;
    sBlockSize = sLogStream->mLogStreamPersData.mBlockSize;

    STL_TRY( smrFindLogGroupBySeqNo( sLogStream,
                                     SMR_FILE_SEQ_NO( aHintLid ),
                                     &sStartLogGroup,
                                     aEnv ) == STL_SUCCESS );

    STL_ASSERT( sStartLogGroup != NULL );

    stlMemset( aBuffer, 0x00, aBufferSize );

    sCurLogGroup = sStartLogGroup;

    while( 1 )
    {
        if( sCurLogGroup->mLogGroupPersData.mPrevLastLsn < aTargetLsn )
        {
            break;
        }

        while( 1 )
        {
            sCurLogGroup = smrGetPrevLogGroup( sLogStream, sCurLogGroup );

            STL_TRY_THROW( sCurLogGroup != sStartLogGroup, RAMP_CONTINUE );
                
            if( sCurLogGroup->mLogGroupPersData.mState != SMR_LOG_GROUP_STATE_UNUSED )
            {
                break;
            }
        }
    }

    STL_RAMP( RAMP_CONTINUE );
    
    /*
     * sCurLogGroup에서 조건에 맞는 block을 찾는다.
     */
    sLogMember = STL_RING_GET_FIRST_DATA( &( sCurLogGroup->mLogMemberList ) );

    sFlag = (STL_FOPEN_READ | STL_FOPEN_LARGEFILE);
        
    sLogIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_LOG_FILE_IO,
                                            KNL_ENV(aEnv) );
        
    if( sLogIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

    STL_TRY( smgOpenFile( &sFile,
                          sLogMember->mLogMemberPersData.mName,
                          sFlag,
                          STL_FPERM_OS_DEFAULT,
                          aEnv )
             == STL_SUCCESS );
    sState = 1;

    /**
     * 초기값 설정
     */

    if( sCurLogGroup->mLogGroupPersData.mFileSeqNo != aHintLid->mFileSeqNo )
    {
        sBlockSeq = SMR_BLOCK_SEQ_FROM_OFFSET( ( sCurLogGroup->mLogGroupPersData.mFileSize -
                                                 sBlockSize ),
                                               sBlockSize );
    }
    
    SMR_MAKE_LID( aTargetLid,
                  sCurLogGroup->mLogGroupPersData.mFileSeqNo,
                  sBlockSeq,
                  0 );
    
    while( STL_TRUE )
    {
        sOffset = SMR_BLOCK_SEQ_FILE_OFFSET( sBlockSeq + 1, sBlockSize );

        sReadSize = STL_MIN( aBufferSize, sOffset );

        sOffset -= sReadSize;
        
        STL_TRY( stlSeekFile( &sFile,
                              STL_FSEEK_SET,
                              &sOffset,
                              KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

        STL_ASSERT( sReadSize > 0 );

        STL_TRY( stlReadFile( &sFile,
                              (void*)aBuffer,
                              sReadSize,
                              NULL,
                              KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

        sBlockCount = sReadSize / sBlockSize;

        /**
         * backward scanning
         * - 이전 round에서 사용한 블록이 있을수도 있다.
         */
        
        for( i = 0; i < sBlockCount; i++ )
        {
            sBlockLsn = SMR_READ_BLOCK_LSN( aBuffer +
                                            ((sBlockCount - 1 - i) * sBlockSize) );

            if( sBlockLsn == SMR_INIT_LSN )
            {
                continue;
            }
            
            /**
             * 유효한 블록을 만나면 이전 블록이 찾는 블록이다.
             * - TargetLsn 보다 작고 블록
             * - 이전 round에서 사용한 블록은 무시
             * - 한번도 사용되지 않은 블록은 무시
             */
            
            if( (sBlockLsn < aTargetLsn) &&
                (sBlockLsn > sCurLogGroup->mLogGroupPersData.mPrevLastLsn) &&
                (sBlockLsn != SMR_INIT_LSN) )
            {
                /**
                 * 찾은 Lid를 aTargetLid에 설정한다.
                 */
                
                *aFound = STL_TRUE;
    
                STL_THROW( RAMP_SUCCESS );
            }

            if( (sBlockSeq - i) == 0 )
            {
                /**
                 * 찾아도 없는 경우는 0번 Block이 원하는 Block이다.
                 */

                SMR_MAKE_LID( aTargetLid,
                              sCurLogGroup->mLogGroupPersData.mFileSeqNo,
                              0,
                              0 );
                
                *aFound = STL_TRUE;
    
                STL_THROW( RAMP_SUCCESS );
            }
            
            SMR_MAKE_LID( aTargetLid,
                          sCurLogGroup->mLogGroupPersData.mFileSeqNo,
                          (sBlockSeq - i),
                          0 );
        }

        sBlockSeq -= sBlockCount; 
    }

    STL_RAMP( RAMP_SUCCESS );

    if( sState == 1 )
    {
        sState = 0;
        STL_TRY( smgCloseFile( &sFile,
                               aEnv )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sState > 0 )
    {
        (void)smgCloseFile( &sFile, aEnv );
    }

    return STL_FAILURE;
}

/*
 * 주어진 aTargetLsn보다 같거나 큰 lsn 중 가장 작은 lsn을 갖는 log block을 반환한다.
 */
stlStatus smrFindFileBlockByLsn( smrLogGroup         * aLogGroup,
                                 smrLsn                aTargetLsn,
                                 stlChar             * aBuffer,
                                 stlInt64              aBufferSize,
                                 smrLid              * aTargetLid,
                                 stlBool             * aFound,
                                 smlEnv              * aEnv )
{
    smrLogStream   * sLogStream;
    smrLogGroup    * sCurLogGroup;
    smrLogMember   * sLogMember;
    smrLogBlockHdr   sLogBlockHdr;
    stlInt16         sBlockSize;
    stlInt32         sState = 0;
    stlFile          sFile;
    stlInt32         sBlockSeq = 0;
    stlInt64         sMaxBlockCount;
    stlInt64         sBlockCount;
    stlInt64         sReadSize;
    stlInt64         i;
    stlOffset        sOffset;
    stlInt32         sFlag = 0;
    stlInt64         sLogIo;

    *aFound = STL_FALSE;
    
    sLogStream = &gSmrWarmupEntry->mLogStream;
    sBlockSize = sLogStream->mLogStreamPersData.mBlockSize;

    sCurLogGroup = aLogGroup;

    while( 1 )
    {
        if( sCurLogGroup->mLogGroupPersData.mPrevLastLsn < aTargetLsn )
        {
            break;
        }

        while( 1 )
        {
            sCurLogGroup = smrGetPrevLogGroup( sLogStream, sCurLogGroup );

            STL_TRY_THROW( sCurLogGroup != aLogGroup, RAMP_SUCCESS );
                
            if( sCurLogGroup->mLogGroupPersData.mState != SMR_LOG_GROUP_STATE_UNUSED )
            {
                break;
            }
        }
    }

    stlMemset( aBuffer, 0x00, aBufferSize );

    /*
     * sCurLogGroup에서 조건에 맞는 block을 찾는다.
     */
    sMaxBlockCount = sCurLogGroup->mLogGroupPersData.mFileSize / sBlockSize;
    sLogMember = STL_RING_GET_FIRST_DATA( &( sCurLogGroup->mLogMemberList ) );

    sFlag = (STL_FOPEN_READ | STL_FOPEN_LARGEFILE);
        
    sLogIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_LOG_FILE_IO,
                                            KNL_ENV(aEnv) );
        
    if( sLogIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

    STL_TRY( smgOpenFile( &sFile,
                          sLogMember->mLogMemberPersData.mName,
                          sFlag,
                          STL_FPERM_OS_DEFAULT,
                          aEnv )
             == STL_SUCCESS );
    sState = 1;

    /* Log file header의 prevLastLsn이 target Lsn보다 작은 값이어야 함 */
    STL_TRY( stlReadFile( &sFile,
                          (void*)aBuffer,
                          STL_ALIGN( STL_SIZEOF(smrLogFileHdr), sBlockSize ),
                          NULL,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    STL_TRY_THROW( ((smrLogFileHdr*)aBuffer)->mPrevLastLsn < aTargetLsn, RAMP_SUCCESS );

    while( sBlockSeq < sMaxBlockCount - 1 )
    {
        sOffset = SMR_BLOCK_SEQ_FILE_OFFSET( sBlockSeq, sBlockSize );

        STL_TRY( stlSeekFile( &sFile,
                              STL_FSEEK_SET,
                              &sOffset,
                              KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

        sReadSize = STL_MIN( aBufferSize,
                             (sCurLogGroup->mLogGroupPersData.mFileSize - sOffset) );
        STL_ASSERT( sReadSize > 0 );

        STL_TRY( stlReadFile( &sFile,
                              (void*)aBuffer,
                              sReadSize,
                              NULL,
                              KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

        /* Log group의 prevLastLsn이 target Lsn보다 크거나 같다면
         * switching된 것이므로 archive logfile을 보도록 한다. */
        stlMemBarrier();
        STL_TRY_THROW( sCurLogGroup->mLogGroupPersData.mPrevLastLsn < aTargetLsn,
                       RAMP_SUCCESS );

        sBlockCount = sReadSize / sBlockSize;
        for( i = 0; i < sBlockCount; i++ )
        {
            SMR_READ_LOG_BLOCK_HDR( &sLogBlockHdr, aBuffer + (i * sBlockSize) );

            SMR_MAKE_LID( aTargetLid,
                          sCurLogGroup->mLogGroupPersData.mFileSeqNo,
                          sBlockSeq,
                          0 );

            if( sLogBlockHdr.mLsn >= aTargetLsn )
            {
                *aFound = STL_TRUE;
                STL_THROW( RAMP_SUCCESS );
            }

            if( sLogBlockHdr.mLsn == SMR_INIT_LSN )
            {
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

    return STL_SUCCESS;

    STL_FINISH;

    if( sState > 0 )
    {
        (void)smgCloseFile( &sFile, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smrValidateLogBlocks( smrLogGroup  * aLogGroup,
                                smrLogBuffer * aLogBuffer,
                                void         * aBuffer,
                                stlInt64       aFlushBytes,
                                stlInt16       aBlockSize,
                                smlEnv       * aEnv )
{
    smrLogBlockHdr   sLogBlockHdr;
    stlOffset        sOffset= 0;
    stlInt32         sInterErrPos = 0;
    
    while( sOffset < aFlushBytes )
    {
        SMR_READ_LOG_BLOCK_HDR( &sLogBlockHdr, aBuffer + sOffset );

        if( (sLogBlockHdr.mLsn == 0) &&
            (sLogBlockHdr.mLogCount == 0) &&
            (sLogBlockHdr.mBlockInfo == 0) &&
            (sLogBlockHdr.mFirstLogOffset == 0) )
        {
            STL_TRY( knlLogMsg( NULL,
                                KNL_ENV( aEnv ),
                                KNL_LOG_LEVEL_INFO,
                                "[VALIDATE LOG BLOCK] LOG_GROUP(CUR_BLOCK_SEQ(%d)) \n"
                                "LOG_BUFFER(BLOCK_COUNT(%ld), BLOCK_OFFSET(%ld), "
                                "REAR_FILE_BLOCK_SEQ_NO(%d), "
                                "FRONT_FILE_BLOCK_SEQ_NO(%d) LEFT_LOG_COUNT_IN_BLOCK(%d), "
                                "REAR_SBSN(%ld), REAR_LSN(%ld), FRONT_SBSN(%ld), FRONT_LSN(%d)) \n",
                                aLogGroup->mCurBlockSeq,
                                aLogBuffer->mBufferBlockCount,
                                aLogBuffer->mBlockOffset,
                                aLogBuffer->mRearFileBlockSeqNo,
                                aLogBuffer->mFrontFileBlockSeqNo,
                                aLogBuffer->mLeftLogCountInBlock,
                                aLogBuffer->mRearSbsn,
                                aLogBuffer->mRearLsn,
                                aLogBuffer->mFrontSbsn,
                                aLogBuffer->mFrontLsn ) == STL_SUCCESS );

            sInterErrPos = 1;
            STL_THROW( RAMP_ERR_INTERNAL );
        }

        sOffset += aBlockSize;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INTERNAL,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      "smrValidateLogBlocks()",
                      sInterErrPos );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrWriteFileBlocks( smrLogStream * aLogStream,
                              void         * aBuffer,
                              stlInt64       aFlushBytes,
                              stlBool        aDoValidate,
                              stlBool      * aIsValid,
                              stlLogger    * aLogger,
                              smlEnv       * aEnv )
{
    smrLogGroup    * sLogGroup;
    smrLogMember   * sLogMember;
    stlFile          sFile;
    stlInt32         sState = 0;
    stlInt32         sBlockSeq;
    stlOffset        sOffset;
    stlInt16         sBlockSize;
    stlInt64         sReqBlockCount;
    stlInt32         sFlag = 0;
    stlInt64         sLogIo;
    stlOffset        sPrevOffset;
    stlChar          sBuffer[SMR_MAX_LOG_BLOCK_SIZE * 2];
    stlChar        * sPrevBlock;
    smrLsn           sPrevLsn;
    smrLsn           sLsn;
    stlSize          sReadBytes;

    STL_TRY_THROW( aFlushBytes > 0, RAMP_SUCCESS );
    
    /**
     * current group을 찾는다.
     */
    sLogGroup      = aLogStream->mCurLogGroup4Disk;
    sBlockSize     = aLogStream->mLogStreamPersData.mBlockSize;
    sReqBlockCount = aFlushBytes / sBlockSize;

    STL_DASSERT( smrValidateLogBlocks( sLogGroup,
                                       aLogStream->mLogBuffer,
                                       aBuffer,
                                       aFlushBytes,
                                       sBlockSize,
                                       aEnv ) == STL_SUCCESS );

    /**
     * group의 member 별도 기록한다.
     */
    
    sBlockSeq = sLogGroup->mCurBlockSeq;
    sOffset = SMR_BLOCK_SEQ_FILE_OFFSET( sBlockSeq, sBlockSize );
    
    sFlag = (STL_FOPEN_WRITE | STL_FOPEN_READ | STL_FOPEN_LARGEFILE);

    sLogIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_LOG_FILE_IO,
                                            KNL_ENV(aEnv) );

    if( sLogIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

        
    STL_ASSERT( sLogGroup->mLogGroupPersData.mFileSize >=
                SMR_BLOCK_SEQ_FILE_OFFSET((sBlockSeq + sReqBlockCount), sBlockSize) );

    STL_RING_FOREACH_ENTRY( &(sLogGroup->mLogMemberList), sLogMember )
    {
        STL_TRY( smgOpenFile( &sFile,
                              sLogMember->mLogMemberPersData.mName,
                              sFlag,
                              STL_FPERM_OS_DEFAULT,
                              aEnv )
                 == STL_SUCCESS );
        sState = 1;

        /**
         * gsyncher에서 Log flush 할 때에 Logfile과 Buffer를 Validation한다.
         */ 
        if( aDoValidate == STL_TRUE )
        {
            sPrevBlock = (stlChar*) STL_ALIGN( (stlInt64)sBuffer, SMR_BLOCK_SIZE );

            sLsn = SMR_READ_BLOCK_LSN( aBuffer );

            if( sBlockSeq > 0 )
            {
                sPrevOffset = sOffset - SMR_BLOCK_SIZE;
            }
            else
            {
                sPrevOffset = 0;
            }

            STL_TRY( stlSeekFile( &sFile,
                                  STL_FSEEK_SET,
                                  &sPrevOffset,
                                  KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

            STL_TRY( stlReadFile( &sFile,
                                  (void*)sPrevBlock,
                                  SMR_BLOCK_SIZE,
                                  &sReadBytes,
                                  KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

            if( sBlockSeq > 0 )
            {
                sPrevLsn = SMR_READ_BLOCK_LSN( sPrevBlock );
            }
            else
            {
                sPrevLsn = ((smrLogFileHdr*)sPrevBlock)->mPrevLastLsn;
            }

            if( sLsn >= sPrevLsn )
            {
                *aIsValid = STL_TRUE;
            }
            else
            {
                *aIsValid = STL_FALSE;
                if( aLogger != NULL )
                {
                    STL_TRY( stlLogMsg( aLogger,
                                        KNL_ERROR_STACK( aEnv ),
                                        "[WRITE BLOCKS] Invalid blocks - "
                                        "File last lsn (%ld), Block lsn (%ld)\n",
                                        sPrevLsn,
                                        sLsn ) == STL_SUCCESS );
                }
                sState = 0;
                STL_TRY( smgCloseFile( &sFile,
                                       aEnv ) == STL_SUCCESS );
                STL_THROW( RAMP_SUCCESS );
            }
        }

        STL_TRY( stlSeekFile( &sFile,
                              STL_FSEEK_SET,
                              &sOffset,
                              KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

        STL_TRY( stlWriteFile( &sFile,
                               aBuffer,
                               aFlushBytes,
                               NULL,
                               KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

        sState = 0;
        STL_TRY( smgCloseFile( &sFile,
                               aEnv )
                 == STL_SUCCESS );
    }

    sLogGroup->mCurBlockSeq = (sBlockSeq + sReqBlockCount);

    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    knlSystemFatal( "can't write logs to logfile", KNL_ENV(aEnv) );
    
    if( sState == 1 )
    {
        (void)smgCloseFile( &sFile, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smrReadFileBlocks( smrLid   * aLid,
                             stlChar  * aBuffer,
                             stlInt64   aBufferSize,
                             stlInt64 * aReadBytes,
                             stlBool  * aEndOfFile,
                             stlBool  * aEndOfLog,
                             smlEnv   * aEnv )
{
    smrLogStream   * sLogStream;
    smrLogGroup    * sLogGroup;

    /**
     * FileSeqNo에 맞는 LogGroup을 찾는다.
     */
    sLogStream = &gSmrWarmupEntry->mLogStream;

    STL_TRY( smrFindLogGroupBySeqNo( sLogStream,
                                     SMR_FILE_SEQ_NO( aLid ),
                                     &sLogGroup,
                                     aEnv ) == STL_SUCCESS );

    STL_DASSERT( sLogGroup != NULL );
    
    STL_TRY( smrReadFileBlocksInternal( aLid,
                                        sLogGroup,
                                        aBuffer,
                                        aBufferSize,
                                        aReadBytes,
                                        aEndOfFile,
                                        aEndOfLog,
                                        aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrReadFileBlocksInternal( smrLid      * aLid,
                                     smrLogGroup * aLogGroup,
                                     stlChar     * aBuffer,
                                     stlInt64      aBufferSize,
                                     stlInt64    * aReadBytes,
                                     stlBool     * aEndOfFile,
                                     stlBool     * aEndOfLog,
                                     smlEnv      * aEnv )
{
    smrLogStream   * sLogStream;
    smrLogMember   * sLogMember;
    smrLogBlockHdr   sLogBlockHdr;
    stlFile          sFile;
    stlUInt32        sState = 0;
    stlInt64         sMaxBlockCount;
    smrLsn           sFirstLsn;
    stlInt16         sBlockSize;
    stlInt32         sBlockSeq;
    stlOffset        sOffset;
    stlInt64         sBlockCount;
    stlInt64         sReadBufferSize;
    stlInt64         sLeftSize;
    stlSize          sReadBytes = 0;
    stlInt64         sTotalReadBytes = 0;
    stlInt64         i;
    stlInt64         sBufferSize;
    stlInt32         sFlag = 0;
    stlInt64         sLogIo;

    *aReadBytes = 0;
    *aEndOfFile = STL_FALSE;
    *aEndOfLog  = STL_FALSE; 
    
    /**
     * FileSeqNo에 맞는 LogGroup을 찾는다.
     */
    sLogStream = &gSmrWarmupEntry->mLogStream;
    sBlockSize = sLogStream->mLogStreamPersData.mBlockSize;

    sFirstLsn = aLogGroup->mLogGroupPersData.mPrevLastLsn + 1;
    sMaxBlockCount = aLogGroup->mLogGroupPersData.mFileSize / sBlockSize;

    /**
     * 가리키는 Lid가 파일 범위를 벗어나면 aEndOfFile이나 aEndOfLog에 STL_TRUE 설정
     */
    if( SMR_BLOCK_SEQ_NO(aLid) >= ( sMaxBlockCount - 1 ) )
    {
        *aEndOfFile  = STL_TRUE;
        STL_THROW( RAMP_SUCCESS );
    }

    /**
     * LogMember의 파일을 읽는다.
     */

    /**
     * @todo 읽은 파일이 valid하지 않다면 다른 LogMember를 읽어야 한다.
     */
    sLogMember = STL_RING_GET_FIRST_DATA( &( aLogGroup->mLogMemberList ) );

    sFlag = (STL_FOPEN_WRITE | STL_FOPEN_READ | STL_FOPEN_LARGEFILE);
        
    sLogIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_LOG_FILE_IO,
                                            KNL_ENV(aEnv) );
        
    if( sLogIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

    /**
     * Log buffer는 초기에 align이 맞춰져 있다.
     */
    STL_TRY( smgOpenFile( &sFile,
                          sLogMember->mLogMemberPersData.mName,
                          sFlag,
                          STL_FPERM_OS_DEFAULT,
                          aEnv )
             == STL_SUCCESS );
    sState = 1;

    /**
     * 파일에서 aLid의 위치의 Block Seq No부터 aBuffer에 올린다.
     */
    sBlockSeq = SMR_BLOCK_SEQ_NO( aLid );    

    /**
     * 읽을수 있는 버퍼의 크기는 최대 SMR_BULK_IO_BLOCK_UNIT_COUNT * BlockSize 이다.
     */
    sReadBufferSize = STL_MIN( aBufferSize,
                               (sBlockSize * SMR_BULK_IO_BLOCK_UNIT_COUNT) );
    STL_ASSERT( sReadBufferSize > 0 );
    sLeftSize = aBufferSize;

    while( sTotalReadBytes < aBufferSize )
    {
        sOffset = SMR_BLOCK_SEQ_FILE_OFFSET( sBlockSeq, sBlockSize );

        /**
         * FILE_END가 설정되지 않은 파일의 끝에 도달했다면
         */
        
        if( aLogGroup->mLogGroupPersData.mFileSize == sOffset )
        {
            *aEndOfFile  = STL_TRUE;
            STL_THROW( RAMP_SUCCESS );
        }
        
        STL_TRY( stlSeekFile( &sFile,
                              STL_FSEEK_SET,
                              &sOffset,
                              KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

        /**
         * 읽을 수 있는 버퍼의 크기는
         * MIN( 요청된 size, 읽을 buffer의 최대 크기, file의 나머지 크기 )
         */
        sBufferSize = STL_MIN( sLeftSize, sReadBufferSize );
        sBufferSize = STL_MIN( sBufferSize,
                               (aLogGroup->mLogGroupPersData.mFileSize - sOffset) );
        STL_ASSERT( sBufferSize > 0 );

        STL_TRY( stlReadFile( &sFile,
                              (void*)(aBuffer + sTotalReadBytes),
                              sBufferSize,
                              &sReadBytes,
                              KNL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );

        /**
         * file을 읽은 후 요청된 size의 크기를 감소
         */
        sLeftSize -= sBufferSize;
        sBlockCount = sReadBytes / sBlockSize;

        /**
         * sFirstLsn 보다 읽은 Block의 Lsn이 작다면 aEndOfLog 설정
         */
    
        for( i = 0; i < sBlockCount; i++ )
        {
            SMR_READ_LOG_BLOCK_HDR( &sLogBlockHdr, aBuffer + sTotalReadBytes );

            /**
             * Dummy FILE_END block은 log가 없으므로 read bytes를 추가하지 않고
             * EndOfFile을 설정
             */
            if( SMR_IS_DUMMY_FILE_END_BLOCK( &sLogBlockHdr ) )
            {
                *aEndOfFile = STL_TRUE;
                STL_THROW( RAMP_SUCCESS );
            }

            if( sLogBlockHdr.mLsn < sFirstLsn )
            {
                *aEndOfFile  = STL_TRUE;
                STL_THROW( RAMP_SUCCESS );
            }

            sTotalReadBytes += sBlockSize;

            /**
             * FILE_END가 표시되어 있는 정상적인 log block의 size를 추가하고
             * EndOfFile을 설정
             */
            if( SMR_IS_LAST_FILE_BLOCK( &sLogBlockHdr ) )
            {
                *aEndOfFile = STL_TRUE;
                STL_THROW( RAMP_SUCCESS );
            }

            sBlockSeq++;
        }
    }

    STL_RAMP( RAMP_SUCCESS );

    if( (*aEndOfFile == STL_TRUE) &&
        (aLogGroup->mLogGroupPersData.mState == SMR_LOG_GROUP_STATE_CURRENT) )
    {
        *aEndOfLog  = STL_TRUE;
    }
    
    *aReadBytes = sTotalReadBytes;

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

stlStatus smrReadOnlineFileBlocks( smrLid         * aLid,
                                   stlChar        * aBuffer,
                                   stlInt16         aWrapNo,
                                   stlInt64       * aReadBytes,
                                   stlBool        * aSwitched,
                                   stlBool        * aEndOfFile,
                                   stlBool        * aEndOfLog,
                                   smlEnv         * aEnv )
{
    smrLogBuffer   * sLogBuffer;
    smrLogStream   * sLogStream;
    smrLogGroup    * sLogGroup;
    smrLogMember   * sLogMember;
    smrLogBlockHdr   sLogBlockHdr;
    stlFile          sFile;
    stlUInt32        sState = 0;
    stlInt64         sMaxBlockCount;
    smrLsn           sFirstLsn;
    stlInt16         sBlockSize;
    stlInt32         sBlockSeq;
    stlOffset        sOffset;
    stlInt64         sBlockCount;
    stlInt64         sReadBytes = 0;
    stlSize          sTotalReadBytes = 0;
    stlInt64         i;
    stlInt64         sBufferSize;
    stlInt32         sFlag = 0;
    stlInt64         sLogIo;

    *aReadBytes = 0;
    *aSwitched  = STL_FALSE;
    *aEndOfFile = STL_FALSE;
    *aEndOfLog  = STL_FALSE; 

    sLogBuffer = (smrLogBuffer *)SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, LogBuffer );

    /**
     * FileSeqNo에 맞는 LogGroup을 찾는다.
     */
    sLogStream = &gSmrWarmupEntry->mLogStream;
    sBlockSize = sLogStream->mLogStreamPersData.mBlockSize;

    KNL_BREAKPOINT( KNL_BREAKPOINT_SMRREADONLINEFILEBLOCKS_BEFORE_FIND_LOG_GROUP,
                    (knlEnv*)aEnv );

    STL_TRY( smrFindLogGroupBySeqNo( sLogStream,
                                     SMR_FILE_SEQ_NO( aLid ),
                                     &sLogGroup,
                                     aEnv ) == STL_SUCCESS );

    /**
     * Media recovery시 logfile sequence no로 log group을 못 찾았을 경우
     * Archive logfile을 이용한다.
     */
    if( sLogGroup == NULL )
    {
        STL_TRY( knlLogMsg( NULL,
                            KNL_ENV( aEnv ),
                            KNL_LOG_LEVEL_INFO,
                            "[MEDIA RECOVERY] target logfile was switched"
                            "( LogFileSeq : %ld )\n",
                            SMR_FILE_SEQ_NO( aLid ) )
                 == STL_SUCCESS );

        *aSwitched = STL_TRUE;
        STL_THROW( RAMP_SUCCESS );
    }

    /**
     * @todo 읽은 파일이 valid하지 않다면 다른 LogMember를 읽어야 한다.
     */
    sLogMember = STL_RING_GET_FIRST_DATA( &( sLogGroup->mLogMemberList ) );

    if( smrIsLogfileCrossed(SMR_FILE_SEQ_NO(aLid), aEnv) == STL_TRUE )
    {
        stlMemset( SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, LogfileName ),
                   0x00,
                   STL_MAX_FILE_PATH_LENGTH );
        stlMemcpy( SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, LogfileName ),
                   sLogMember->mLogMemberPersData.mName,
                   stlStrlen(sLogMember->mLogMemberPersData.mName) );

        sLogBuffer->mFileSeqNo = SMR_FILE_SEQ_NO( aLid );
        SMR_MEDIA_RECOVERY_SET_FIELD( aEnv,
                                      MaxBlockCount,
                                      sLogGroup->mLogGroupPersData.mFileSize / sBlockSize );
    }

    sFirstLsn = sLogGroup->mLogGroupPersData.mPrevLastLsn + 1;
    sMaxBlockCount = SMR_MEDIA_RECOVERY_GET_FIELD( aEnv, MaxBlockCount );

    /**
     * 가리키는 Lid가 파일 범위를 벗어나면 aEndOfFile이나 aEndOfLog에 STL_TRUE 설정
     */
    if( SMR_BLOCK_SEQ_NO(aLid) >= ( sMaxBlockCount - 1 ) )
    {
        *aEndOfFile  = STL_TRUE;
        STL_THROW( RAMP_SUCCESS );
    }

    /**
     * LogMember의 파일을 읽는다.
     */

    sFlag = (STL_FOPEN_WRITE | STL_FOPEN_READ | STL_FOPEN_LARGEFILE);
        
    sLogIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_LOG_FILE_IO,
                                            KNL_ENV(aEnv) );
        
    if( sLogIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

    STL_TRY( smgOpenFile( &sFile,
                          sLogMember->mLogMemberPersData.mName,
                          sFlag,
                          STL_FPERM_OS_DEFAULT,
                          aEnv )
             == STL_SUCCESS );
    sState = 1;

    /**
     * 파일에서 aLid의 위치의 Block Seq No부터 aBuffer에 올린다.
     */
    sBlockSeq = SMR_BLOCK_SEQ_NO( aLid );    
    sOffset = SMR_BLOCK_SEQ_FILE_OFFSET( sBlockSeq, sBlockSize );

    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    /**
     * 읽을 수 있는 버퍼의 크기는 파일 사이즈를 초과할수 없다.
     */
    sBufferSize = STL_MIN( sLogBuffer->mBufferSize,
                           (sLogGroup->mLogGroupPersData.mFileSize - sOffset) );
    STL_ASSERT( sBufferSize > 0 );

    KNL_BREAKPOINT( KNL_BREAKPOINT_SMRREADONLINEFILEBLOCKS_BEFORE_READ,
                    (knlEnv*)aEnv );

    STL_TRY( stlReadFile( &sFile,
                          aBuffer,
                          sBufferSize,
                          &sTotalReadBytes,
                          KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    sBlockCount = sTotalReadBytes / sBlockSize;

    /**
     * sFirstLsn 보다 읽은 Block의 Lsn이 작다면 aEndOfLog 설정
     */
    sReadBytes = 0;
    
    for( i = 0; i < sBlockCount; i++ )
    {
        SMR_READ_LOG_BLOCK_HDR( &sLogBlockHdr, aBuffer + ( sBlockSize * i ) );

        /**
         * Dummy FILE_END block은 log가 없으므로 read bytes를 추가하지 않고
         * EndOfFile을 설정
         */
        if( SMR_IS_DUMMY_FILE_END_BLOCK( &sLogBlockHdr ) )
        {
            *aEndOfFile = STL_TRUE;
            break;
        }

        if( sLogBlockHdr.mLsn < sFirstLsn )
        {
            *aEndOfFile = STL_TRUE;
            break;
        }

        /**
         * Service중에 Tablespace의 Media Recover 시 Online logfile이 switch
         * 될 수 있으므로 WrapNo를 체크해야 한다.
         */
        if( SMR_BLOCK_WRAP_NO( &sLogBlockHdr ) != aWrapNo )
        {
            STL_TRY( knlLogMsg( NULL,
                                KNL_ENV( aEnv ),
                                KNL_LOG_LEVEL_INFO,
                                "[MEDIA RECOVERY] target logfile was switched"
                                "( LogFileSeq : %ld, CurrentWrapNo : %d, ExpectedWrapNo : %d )\n",
                                sLogBuffer->mFileSeqNo,
                                SMR_BLOCK_WRAP_NO( &sLogBlockHdr ),
                                aWrapNo )
                     == STL_SUCCESS );

            *aSwitched = STL_TRUE;
            break;
        }

        sReadBytes += sBlockSize;

        /**
         * FILE_END가 표시되어 있는 정상적인 log block의 size를 추가하고
         * EndOfFile을 설정
         */
        if( SMR_IS_LAST_FILE_BLOCK( &sLogBlockHdr ) )
        {
            *aEndOfFile = STL_TRUE;
            break;
        }
    }

    STL_RAMP( RAMP_SUCCESS );

    if( (*aEndOfFile == STL_TRUE) &&
        (sLogGroup->mLogGroupPersData.mState == SMR_LOG_GROUP_STATE_CURRENT) )
    {
        *aEndOfLog  = STL_TRUE;
    }
    
    *aReadBytes = sReadBytes;

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

stlStatus smrLogFileSwitching( smrLogStream * aLogStream,
                               smrLsn         aLastLsn,
                               smlEnv       * aEnv )
{
    smrLogGroup         * sCurLogGroup;
    smrLogGroup         * sNextLogGroup;
    smrLogGroupPersData * sLogGroupPersData;
    smrLogFileHdr         sLogFileHdr;
    stlInt32              sState = 0;

    STL_TRY( smfAcquireDatabaseFileLatch( aEnv ) == STL_SUCCESS );
    sState = 1;
    
    aLogStream->mLogStreamPersData.mCurFileSeqNo++;

    sCurLogGroup = aLogStream->mCurLogGroup4Disk;
    sLogGroupPersData = &sCurLogGroup->mLogGroupPersData;
    sLogGroupPersData->mState = SMR_LOG_GROUP_STATE_ACTIVE;

    /*
     * 사용될 Log Group의 설정
     */
    sNextLogGroup = smrGetNextLogGroup( aLogStream, aLogStream->mCurLogGroup4Disk );

    if( smrIsCheckpointerActive() == STL_TRUE )
    {
        /**
         * Log File Switching시 현재의 next log group은 반드시 재사용 가능한 상태여야 한다.
         * - CURRENT와 ACTIVE 사이의 최소 Gap은 1 이하일 경우는 로깅자체가 중단되기 때문에
         *   항상 CURRENT log group 다음 log group은 재사용 가능 상태이다.
         */
        STL_ASSERT( SMR_IS_USABLE_LOG_FILE( sNextLogGroup ) == STL_TRUE );
    }
    
    sLogGroupPersData = &sNextLogGroup->mLogGroupPersData;
    sLogGroupPersData->mState = SMR_LOG_GROUP_STATE_CURRENT;
    sLogGroupPersData->mFileSeqNo = aLogStream->mLogStreamPersData.mCurFileSeqNo;
    sNextLogGroup->mCurBlockSeq = 0;
    sLogGroupPersData->mPrevLastLsn = aLastLsn;

    STL_TRY( smrWriteLogfileHdr( aLogStream,
                                 sNextLogGroup,
                                 &sLogFileHdr,
                                 aEnv ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( smfReleaseDatabaseFileLatch( aEnv ) == STL_SUCCESS );
    
    /**
     * LogMirror에서 LogFileHeader를 보내도록 함
     */
    STL_TRY( smrWriteLogMirrorBuffer( &sLogFileHdr,
                                      STL_SIZEOF( smrLogFileHdr ),
                                      STL_TRUE,
                                      aEnv ) == STL_SUCCESS );
    STL_TRY( smrCheckLogMirrorBuffer( aEnv ) == STL_SUCCESS );

    STL_TRY( smfSaveCtrlFile( aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) smfReleaseDatabaseFileLatch( aEnv );
    }

    return STL_FAILURE;
}

stlStatus smrLogFileSwitchingForce( smrLogStream * aLogStream,
                                    smrLogGroup  * aCurLogGroup,
                                    smrLsn         aLastLsn,
                                    smlEnv       * aEnv )
{
    smrLogGroup         * sCurLogGroup;
    smrLogGroup         * sNextLogGroup;
    smrLogGroupPersData * sLogGroupPersData;
    smrLogFileHdr         sLogFileHdr;

    aLogStream->mLogStreamPersData.mCurFileSeqNo++;

    sCurLogGroup = aCurLogGroup;
    sLogGroupPersData = &sCurLogGroup->mLogGroupPersData;
    sLogGroupPersData->mState = SMR_LOG_GROUP_STATE_ACTIVE;

    /*
     * 사용될 Log Group의 설정
     */
    sNextLogGroup = smrGetNextLogGroup( aLogStream, sCurLogGroup );

    sLogGroupPersData = &sNextLogGroup->mLogGroupPersData;
    sLogGroupPersData->mState = SMR_LOG_GROUP_STATE_CURRENT;
    sLogGroupPersData->mFileSeqNo = aLogStream->mLogStreamPersData.mCurFileSeqNo;
    sNextLogGroup->mCurBlockSeq = 0;
    sLogGroupPersData->mPrevLastLsn = aLastLsn;

    STL_TRY( smrWriteLogfileHdr( aLogStream,
                                 sNextLogGroup,
                                 &sLogFileHdr,
                                 aEnv ) == STL_SUCCESS );
    
    /**
     * LogMirror에서 LogFileHeader를 보내도록 함
     */
    STL_TRY( smrWriteLogMirrorBuffer( &sLogFileHdr,
                                      STL_SIZEOF( smrLogFileHdr ),
                                      STL_TRUE,
                                      aEnv ) == STL_SUCCESS );
    STL_TRY( smrCheckLogMirrorBuffer( aEnv ) == STL_SUCCESS );

    STL_TRY( smfSaveCtrlFile( aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrClearPartialLogBlocks( smrLid   * aPartialLid,
                                    stlInt32 * aClearBlockCount,
                                    smlEnv   * aEnv )
{
    smrLogStream   * sLogStream;
    smrLogGroup    * sLogGroup;
    smrLogMember   * sLogMember;
    smrLogBlockHdr   sLogBlockHdr;
    stlInt16         sBlockSize;
    stlInt32         sState = 0;
    stlChar        * sLogBlock;
    stlChar        * sLogAlignedBlock;
    stlFile          sFile;
    stlInt32         sBlockSeq = 0;
    stlInt64         sMaxBlockCount;
    stlInt64         sBlockCount;
    stlInt64         sBufferSize;
    stlInt64         sReadSize;
    stlInt64         i;
    stlOffset        sOffset;
    smrLsn           sPrevLsn;
    knlRegionMark    sMemMark;
    smrLogBuffer   * sLogBuffer;
    stlInt32         sFlag = 0;
    stlInt64         sLogIo;
    smrLogHdr        sLogHdr;
    smlRid           sInvalidRid = SML_INVALID_RID;

    *aClearBlockCount = 0;
    
    sLogStream = &gSmrWarmupEntry->mLogStream;
    sLogBuffer = sLogStream->mLogBuffer;
    sBlockSize = sLogStream->mLogStreamPersData.mBlockSize;

    STL_TRY( smrFindLogGroupBySeqNo( sLogStream,
                                     SMR_FILE_SEQ_NO( aPartialLid ),
                                     &sLogGroup,
                                     aEnv ) == STL_SUCCESS );

    STL_ASSERT( sLogGroup != NULL );

    sPrevLsn = sLogGroup->mLogGroupPersData.mPrevLastLsn;
        
    /**
     * Log Block Size로 align된 Log Block을 할당한다.
     */
    KNL_INIT_REGION_MARK( &sMemMark );

    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               (knlEnv*)aEnv )
             == STL_SUCCESS );

    sBufferSize = sBlockSize * SMR_BULK_IO_BLOCK_UNIT_COUNT;
    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                sBlockSize + sBufferSize,
                                (void**)&sLogBlock,
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 1;

    sLogAlignedBlock = (stlChar*)STL_ALIGN( (stlUInt64)sLogBlock,
                                            sBlockSize );
    stlMemset( sLogAlignedBlock, 0x00, sBufferSize );

    /**
     * sLogGroup에서 마지막 block을 찾는다.
     */
    sMaxBlockCount = sLogGroup->mLogGroupPersData.mFileSize / sBlockSize;
    sLogMember = STL_RING_GET_FIRST_DATA( &( sLogGroup->mLogMemberList ) );

    sFlag = (STL_FOPEN_WRITE | STL_FOPEN_READ | STL_FOPEN_LARGEFILE);
        
    sLogIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_LOG_FILE_IO,
                                            KNL_ENV(aEnv) );
    
    if( sLogIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }
        
    STL_TRY( smgOpenFile( &sFile,
                          sLogMember->mLogMemberPersData.mName,
                          sFlag,
                          STL_FPERM_OS_DEFAULT,
                          aEnv )
             == STL_SUCCESS );
    sState = 2;

    /**
     * Clear 1'nd Log Block
     */
    sBlockSeq = SMR_BLOCK_SEQ_NO( aPartialLid );

    sOffset = SMR_BLOCK_SEQ_FILE_OFFSET( sBlockSeq, sBlockSize );

    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    STL_TRY( stlReadFile( &sFile,
                          (void*)sLogAlignedBlock,
                          sBlockSize,
                          NULL,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    SMR_READ_LOG_BLOCK_HDR( &sLogBlockHdr, sLogAlignedBlock );

    STL_ASSERT( sLogBlockHdr.mLogCount != 0 );

    /**
     * 자신외에 Whole Log 또는 Partial Log라도 기록되지 않는 경우
     */
    
    if( (sLogBlockHdr.mLogCount == 1) &&
        (sLogBlockHdr.mFirstLogOffset == SMR_BLOCK_HDR_SIZE) )
    {
        /**
         * Dummy Log를 기록한다.
         */

        SMR_READ_LOG_HDR( &sLogHdr, sLogAlignedBlock + SMR_BLOCK_HDR_SIZE );

        sLogHdr.mSize = 0;
        sLogHdr.mLogPieceCount = 0;
        sLogHdr.mTransId = SML_INVALID_TRANSID;
        SMG_WRITE_RID( &sLogHdr.mSegRid, &sInvalidRid );
        
        SMR_WRITE_LOG_HDR( sLogAlignedBlock + SMR_BLOCK_HDR_SIZE, &sLogHdr );
    }
    else
    {
        sLogBlockHdr.mLsn = sLogBuffer->mRearLsn - 1;
        sLogBlockHdr.mLogCount--;
        gSmrWarmupEntry->mLsn -= 1;
    }

    SMR_WRITE_LOG_BLOCK_HDR( sLogAlignedBlock, &sLogBlockHdr );

    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    STL_TRY( stlWriteFile( &sFile,
                           sLogAlignedBlock,
                           sBlockSize,
                           NULL,
                           KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
    
    (*aClearBlockCount)++;
    sBlockSeq++;
    
    /**
     * Clear N'th Log Block
     */
    while( sBlockSeq < sMaxBlockCount - 1 )
    {
        sOffset = SMR_BLOCK_SEQ_FILE_OFFSET( sBlockSeq, sBlockSize );

        STL_TRY( stlSeekFile( &sFile,
                              STL_FSEEK_SET,
                              &sOffset,
                              KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

        sReadSize = STL_MIN( sBufferSize,
                             (sLogGroup->mLogGroupPersData.mFileSize - sOffset) );
        STL_ASSERT( sReadSize > 0 );

        STL_TRY( stlReadFile( &sFile,
                              sLogAlignedBlock,
                              sReadSize,
                              NULL,
                              KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

        sBlockCount = sReadSize / sBlockSize;
        for( i = 0; i < sBlockCount; i++ )
        {
            SMR_READ_LOG_BLOCK_HDR( &sLogBlockHdr, sLogAlignedBlock + (i * sBlockSize) );

            /**
             * 로그 파일이 재사용 되어 이전 로그를 만난 경우
             */
            if( sPrevLsn > sLogBlockHdr.mLsn )
            {
                break;
            }

            sPrevLsn = sLogBlockHdr.mLsn;
        
            sLogBlockHdr.mLsn = SMR_INIT_LSN;
            sLogBlockHdr.mLogCount = 0;
            SMR_SET_BLOCK_INFO( &sLogBlockHdr,
                                SMR_FILE_END_FALSE,
                                SMR_CHAINED_BLOCK_FALSE,
                                SMR_INVALID_WRAP_NO,
                                SMR_BLOCK_WRITE_FINISH );
            sLogBlockHdr.mFirstLogOffset = 0;
            
            SMR_WRITE_LOG_BLOCK_HDR( sLogAlignedBlock + (i * sBlockSize),
                                     &sLogBlockHdr );
            
            (*aClearBlockCount)++;
            sBlockSeq++;
        }
    
        if( i >= 1 )
        {
            STL_TRY( stlSeekFile( &sFile,
                                  STL_FSEEK_SET,
                                  &sOffset,
                                  KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

            STL_TRY( stlWriteFile( &sFile,
                                   sLogAlignedBlock,
                                   i * sBlockSize,
                                   NULL,
                                   KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
        }

        if( sPrevLsn > sLogBlockHdr.mLsn )
        {
            break;
        }
    }

    sState = 1;
    STL_TRY( smgCloseFile( &sFile,
                           aEnv )
             == STL_SUCCESS );

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_TRUE, /* aFreeChunk */
                                       (knlEnv*)aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2 :
            (void)smgCloseFile( &sFile, aEnv );
        case 1 :
            (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                            &sMemMark,
                                            STL_TRUE, /* aFreeChunk */
                                            (knlEnv*)aEnv );
        default :
            break;
    }
    
    return STL_FAILURE;
}

stlStatus smrMoveWritableFileBlock( smrLid  * aHintLid,
                                    smrLid  * aTargetLid,
                                    stlBool * aEndOfFile,
                                    smlEnv  * aEnv )
{
    smrLogStream   * sLogStream;
    smrLogGroup    * sLogGroup;
    smrLogMember   * sLogMember;
    smrLogBlockHdr   sLogBlockHdr;
    stlInt16         sBlockSize;
    stlInt32         sState = 0;
    stlChar        * sLogBlock;
    stlChar        * sLogAlignedBlock;
    stlFile          sFile;
    stlInt32         sBlockSeq = 0;
    stlInt64         sMaxBlockCount;
    stlInt64         sBlockCount;
    stlInt64         sBufferSize;
    stlInt64         sReadSize;
    stlInt64         i;
    stlOffset        sOffset;
    smrLsn           sPrevLsn;
    knlRegionMark    sMemMark;
    stlInt32         sFlag = 0;
    stlInt64         sLogIo;

    *aEndOfFile = STL_FALSE;

    sLogStream = &gSmrWarmupEntry->mLogStream;
    sBlockSize = sLogStream->mLogStreamPersData.mBlockSize;

    STL_TRY( smrFindLogGroupBySeqNo( sLogStream,
                                     SMR_FILE_SEQ_NO( aHintLid ),
                                     &sLogGroup,
                                     aEnv ) == STL_SUCCESS );

    STL_ASSERT( sLogGroup != NULL );

    sPrevLsn = sLogGroup->mLogGroupPersData.mPrevLastLsn;
        
    /*
     * Log Block Size로 align된 Log Block을 할당한다.
     */
    KNL_INIT_REGION_MARK( &sMemMark );

    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               (knlEnv*)aEnv )
             == STL_SUCCESS );

    sBufferSize = sBlockSize * SMR_BULK_IO_BLOCK_UNIT_COUNT;
    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                sBlockSize + sBufferSize,
                                (void**)&sLogBlock,
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 1;

    sLogAlignedBlock = (stlChar*)STL_ALIGN( (stlUInt64)sLogBlock,
                                            sBlockSize );
    stlMemset( sLogAlignedBlock, 0x00, sBufferSize );

    /*
     * sLogGroup에서 마지막 block을 찾는다.
     */
    sMaxBlockCount = sLogGroup->mLogGroupPersData.mFileSize / sBlockSize;
    sLogMember = STL_RING_GET_FIRST_DATA( &( sLogGroup->mLogMemberList ) );

    sFlag = (STL_FOPEN_WRITE | STL_FOPEN_READ | STL_FOPEN_LARGEFILE);
        
    sLogIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_LOG_FILE_IO,
                                            KNL_ENV(aEnv) );
        
    if( sLogIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

    STL_TRY( smgOpenFile( &sFile,
                          sLogMember->mLogMemberPersData.mName,
                          sFlag,
                          STL_FPERM_OS_DEFAULT,
                          aEnv )
             == STL_SUCCESS );
    sState = 2;

    sBlockSeq = SMR_BLOCK_SEQ_NO( aHintLid );

    while( sBlockSeq < sMaxBlockCount - 1 )
    {
        sOffset = SMR_BLOCK_SEQ_FILE_OFFSET( sBlockSeq, sBlockSize );

        STL_TRY( stlSeekFile( &sFile,
                              STL_FSEEK_SET,
                              &sOffset,
                              KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

        sReadSize = STL_MIN( sBufferSize,
                             (sLogGroup->mLogGroupPersData.mFileSize - sOffset) );
        STL_ASSERT( sReadSize > 0 );

        STL_TRY( stlReadFile( &sFile,
                              (void*)sLogAlignedBlock,
                              sReadSize,
                              NULL,
                              KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

        sBlockCount = sReadSize / sBlockSize;
        for( i = 0; i < sBlockCount; i++ )
        {
            SMR_READ_LOG_BLOCK_HDR( &sLogBlockHdr, sLogAlignedBlock + (i * sBlockSize) );

            SMR_MAKE_LID( aTargetLid,
                          sLogGroup->mLogGroupPersData.mFileSeqNo,
                          sBlockSeq,
                          0 );

            if( SMR_IS_LAST_FILE_BLOCK( &sLogBlockHdr ) )
            {
                *aEndOfFile = STL_TRUE;
                STL_THROW( RAMP_SUCCESS );
            }
            
            /*
             * 로그 파일이 재사용 되어 이전 로그를 만난 경우
             */
            if( sPrevLsn > sLogBlockHdr.mLsn )
            {
                /*
                 * 찾은 Lid를 가지고 리턴한다.
                 */
                STL_THROW( RAMP_SUCCESS );
            }

            sPrevLsn = sLogBlockHdr.mLsn;
            sBlockSeq++;
        }
    }

    /*
     * 사용할 수 있는 block이 없을 경우
     */
    *aEndOfFile = STL_TRUE;    

    STL_RAMP( RAMP_SUCCESS );

    sState = 1;
    STL_TRY( smgCloseFile( &sFile,
                           aEnv )
             == STL_SUCCESS );

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_TRUE, /* aFreeChunk */
                                       (knlEnv*)aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2 :
            (void)smgCloseFile( &sFile, aEnv );
        case 1 :
            (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                            &sMemMark,
                                            STL_TRUE, /* aFreeChunk */
                                            (knlEnv*)aEnv );
        default :
            break;
    }

    return STL_FAILURE;
}

stlInt32 smrGetMaxFileBlockCount( smrLogStream * aLogStream,
                                  smrLogGroup  * aLogGroup )
{
    return ((aLogGroup->mLogGroupPersData.mFileSize /
             aLogStream->mLogStreamPersData.mBlockSize) - 1);
}

stlStatus smrResetLogFile( smrLogStream * aLogStream,
                           smrLid       * aRestartLid,
                           smlEnv       * aEnv )
{
    smrLogGroup * sLogGroup;
    
    STL_TRY( smrFindLogGroupBySeqNo( aLogStream,
                                     SMR_FILE_SEQ_NO( aRestartLid ),
                                     &sLogGroup,
                                     aEnv ) == STL_SUCCESS );

    STL_ASSERT( sLogGroup != NULL );
    
    aLogStream->mCurLogGroup4Disk = sLogGroup;
    aLogStream->mCurLogGroup4Buffer = sLogGroup;
    sLogGroup->mCurBlockSeq = SMR_BLOCK_SEQ_NO( aRestartLid );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrWriteLogfileHdr( smrLogStream  * aLogStream,
                              smrLogGroup   * aLogGroup,
                              smrLogFileHdr * aLogFileHdr,
                              smlEnv        * aEnv )
{
    smrLogFileHdr  * sLogFileHdr;
    smrLogMember   * sLogMember;
    stlChar        * sAlignedLogBlock;
    stlChar        * sLogBlock;
    stlFile          sFile;
    stlInt32         sState = 0;
    stlInt16         sBlockSize;
    stlInt32         sFlag = 0;
    stlInt64         sLogIo;
    knlRegionMark    sMemMark;

    sBlockSize = aLogStream->mLogStreamPersData.mBlockSize;

    sFlag = (STL_FOPEN_WRITE | STL_FOPEN_LARGEFILE);

    sLogIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_LOG_FILE_IO,
                                            KNL_ENV(aEnv) );

    if( sLogIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

    KNL_INIT_REGION_MARK( &sMemMark );

    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               (knlEnv*)aEnv )
             == STL_SUCCESS );

    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                sBlockSize * 2,
                                (void**)&sLogBlock,
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 1;

    sAlignedLogBlock = (stlChar*)STL_ALIGN( (stlInt64)sLogBlock,
                                            sBlockSize );
    stlMemset( sAlignedLogBlock, 0x00, sBlockSize );

    /*
     * Log File Header 설정
     */
    sLogFileHdr                 = (smrLogFileHdr*)sAlignedLogBlock;
    sLogFileHdr->mBlockSize     = sBlockSize;
    sLogFileHdr->mMaxBlockCount = aLogGroup->mLogGroupPersData.mFileSize / sBlockSize;
    sLogFileHdr->mLogFileSeqNo  = aLogGroup->mLogGroupPersData.mFileSeqNo;
    sLogFileHdr->mLogGroupId    = aLogGroup->mLogGroupPersData.mLogGroupId;
    sLogFileHdr->mFileSize      = aLogGroup->mLogGroupPersData.mFileSize;
    sLogFileHdr->mPrevLastLsn   = aLogGroup->mLogGroupPersData.mPrevLastLsn;
    sLogFileHdr->mCreationTime  = stlNow();
    
    if( aLogFileHdr != NULL )
    {
        stlMemcpy( (stlChar*)aLogFileHdr, (stlChar*)sLogFileHdr, STL_SIZEOF( smrLogFileHdr ) );   
    }

    STL_RING_FOREACH_ENTRY( &(aLogGroup->mLogMemberList), sLogMember )
    {
        STL_TRY( smgOpenFile( &sFile,
                              sLogMember->mLogMemberPersData.mName,
                              sFlag,
                              STL_FPERM_OS_DEFAULT,
                              aEnv )
                 == STL_SUCCESS );
        sState = 2;

        STL_TRY( stlWriteFile( &sFile,
                               sLogFileHdr,
                               sBlockSize,
                               NULL,
                               KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

        sState = 1;
        STL_TRY( smgCloseFile( &sFile,
                               aEnv )
                 == STL_SUCCESS );
    }

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       (knlEnv*)aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2 :
            (void)smgCloseFile( &sFile, aEnv );
        case 1 :
            (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                            &sMemMark,
                                            STL_FALSE, /* aFreeChunk */
                                            (knlEnv*)aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}


/**
 * @brief Redo logfile의 Header를 읽어 Current Log Group을 찾는다.
 * @param[in] aLogStream  log stream
 * @param[out] aCurLogGroup  current log group
 * @param[in,out] aEnv  envrionment
 */ 
stlStatus smrFindCurrentLogGroupAtDisk( smrLogStream  * aLogStream,
                                        smrLogGroup  ** aCurLogGroup,
                                        smlEnv        * aEnv )
{
    smrLogGroup   * sCurLogGroup = NULL;
    smrLogGroup   * sNextLogGroup = NULL;
    smrLogMember  * sLogMember = NULL;
    smrLogFileHdr * sLogFileHdr;
    stlFile         sFile;
    stlInt32        sFlag = 0;
    stlInt64        sLogIo;
    stlInt32        sState = 0;
    stlChar       * sAlignedLogBlock;
    stlChar         sLogBlock[SMR_MAX_LOG_BLOCK_SIZE * 2];
    stlInt16        sBlockSize;
    smrLsn          sCurGroupPrevLastLsn;

    sBlockSize = aLogStream->mLogStreamPersData.mBlockSize;
    
    sFlag = STL_FOPEN_READ | STL_FOPEN_LARGEFILE;

    sLogIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_LOG_FILE_IO,
                                            KNL_ENV(aEnv) );

    if( sLogIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

    sAlignedLogBlock = (stlChar*) STL_ALIGN( (stlInt64) sLogBlock,
                                             sBlockSize );

    stlMemset( sAlignedLogBlock, 0x00, sBlockSize );
    sLogFileHdr = (smrLogFileHdr*)sAlignedLogBlock;

    /**
     * Control File 상의 Current Log Group을 구한다.
     */
    STL_RING_FOREACH_ENTRY( &aLogStream->mLogGroupList, sCurLogGroup )
    {
        if( sCurLogGroup->mLogGroupPersData.mState == SMR_LOG_GROUP_STATE_CURRENT )
        {
            break;
        }
    }

    sLogMember = STL_RING_GET_FIRST_DATA( &sCurLogGroup->mLogMemberList );

    STL_TRY( smgOpenFile( &sFile,
                          sLogMember->mLogMemberPersData.mName,
                          sFlag,
                          STL_FPERM_OS_DEFAULT,
                          aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( stlReadFile( &sFile,
                          sLogFileHdr,
                          STL_ALIGN( STL_SIZEOF( smrLogFileHdr ), sBlockSize ),
                          NULL,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( smgCloseFile( &sFile,
                           aEnv ) == STL_SUCCESS );

    sCurGroupPrevLastLsn = sLogFileHdr->mPrevLastLsn;

    /**
     * Old Control File이라면 Current State의 Log Group 보다 최신의 Log Group이 있을 수 있다.
     */ 
    sNextLogGroup = sCurLogGroup;
    
    while( STL_TRUE )
    {
        sNextLogGroup = smrGetNextLogGroup( aLogStream, sNextLogGroup );

        sLogMember = STL_RING_GET_FIRST_DATA( &sNextLogGroup->mLogMemberList );

        STL_TRY( smgOpenFile( &sFile,
                              sLogMember->mLogMemberPersData.mName,
                              sFlag,
                              STL_FPERM_OS_DEFAULT,
                              aEnv ) == STL_SUCCESS );
        sState = 1;

        STL_TRY( stlReadFile( &sFile,
                              sLogFileHdr,
                              STL_ALIGN( STL_SIZEOF( smrLogFileHdr ), sBlockSize ),
                              NULL,
                              KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

        sState = 0;
        STL_TRY( smgCloseFile( &sFile,
                               aEnv ) == STL_SUCCESS );

        if( sCurGroupPrevLastLsn < sLogFileHdr->mPrevLastLsn )
        {
            sCurLogGroup = sNextLogGroup;
            sCurGroupPrevLastLsn = sLogFileHdr->mPrevLastLsn;
        }
        else
        {
            break;
        }
    }

    *aCurLogGroup = sCurLogGroup;

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) smgCloseFile( &sFile, aEnv );
    }
    
    return STL_FAILURE;
}


/**
 * @brief Redo logfile 중 가장 최신의 Logfile에서 마지막 Lsn을 얻는다.
 * @param[out] aLastLsn
 * @param[in,out] aEnv  environment
 */ 
stlStatus smrReadLastLsnFromLogfile( smrLsn  * aLastLsn,
                                     smlEnv  * aEnv )
{
    smrLogStream      * sLogStream;
    smrLogGroup       * sLogGroup = NULL;
    smrLogMember      * sLogMember = NULL;
    smrLogFileHdr       sLogFileHdr;
    smrLogBlockHdr      sLogBlockHdr;
    smrLogCursor4Disk   sLogCursor;
    smrLsn              sLastLsn;
    stlChar           * sLogBlock;
    knlRegionMark       sMemMark;
    stlInt32            sState = 0;
    stlInt32            sValidationFlag;
    stlBool             sIsValid;
    stlBool             sFileExist;
    stlBool             sEndOfFile;
    stlInt32            sHintBlockSeq = 0;
    smrLsn              sHintLsn = SMR_INVALID_LSN;

    sLogStream = &gSmrWarmupEntry->mLogStream;
    
    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               KNL_ENV(aEnv) ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( smrFindCurrentLogGroupAtDisk( sLogStream,
                                           &sLogGroup,
                                           aEnv ) == STL_SUCCESS );

    /**
     * Current log group의 file seq no가 같다면 유효한 Hint Block Seq를 이용한다.
     */
    if( sLogGroup->mLogGroupPersData.mFileSeqNo ==
        SMR_FILE_SEQ_NO( &gSmrWarmupEntry->mLogPersData.mChkptLid ) )
    {
        sHintBlockSeq = SMR_BLOCK_SEQ_NO( &gSmrWarmupEntry->mLogPersData.mChkptLid );
        sHintLsn      = gSmrWarmupEntry->mLogPersData.mChkptLsn;
    }

    sValidationFlag = SMR_VALIDATE_LOGFILE_CREATION_TIME | SMR_VALIDATE_LOGFILE_GROUPID;

    sLogMember = STL_RING_GET_FIRST_DATA( &sLogGroup->mLogMemberList );

    /**
     * Valid하지 않다면 다른 Log Member를 확인해야 하는데?
     */  
    STL_TRY( smrValidateLogfileByName( sLogMember->mLogMemberPersData.mName,
                                       &sLogGroup->mLogGroupPersData,
                                       sValidationFlag,
                                       &sIsValid,
                                       &sFileExist,
                                       aEnv ) == STL_SUCCESS );

    STL_TRY_THROW( sFileExist == STL_TRUE, RAMP_ERR_FILE_NOT_EXIST );
    STL_TRY_THROW( sIsValid == STL_TRUE, RAMP_ERR_INVALID_MEMBER );

    STL_TRY( smrOpenLogCursor4Disk( &sLogCursor,
                                    sLogMember->mLogMemberPersData.mName,
                                    sHintBlockSeq,
                                    sHintLsn,
                                    &sLogFileHdr,
                                    aEnv ) == STL_SUCCESS );
    sState = 2;

    sLastLsn = sLogFileHdr.mPrevLastLsn;
    
    while( STL_TRUE )
    {
        STL_TRY( smrReadBlockCursor4Disk( &sLogCursor,
                                          &sLogBlock,
                                          &sEndOfFile,
                                          aEnv ) == STL_SUCCESS );
        
        if( sEndOfFile == STL_TRUE )
        {
            break;
        }

        SMR_READ_LOG_BLOCK_HDR( &sLogBlockHdr, sLogBlock );

        if( SMR_IS_DUMMY_FILE_END_BLOCK( &sLogBlockHdr ) == STL_TRUE )
        {
            break;
        }

        if( smrIsValidLog( &sLogCursor,
                           &sLogBlockHdr ) == STL_FALSE )
        {
            break;
        }

        if( sLastLsn < sLogBlockHdr.mLsn )
        {
            sLastLsn = sLogBlockHdr.mLsn;
        }
        
        if( SMR_IS_LAST_FILE_BLOCK( &sLogBlockHdr ) == STL_TRUE )
        {
            break;
        }
    }

    *aLastLsn = sLastLsn;

    sState = 1;
    STL_TRY( smrCloseLogCursor4Disk( &sLogCursor,
                                     aEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_TRUE, /* aFreeChunk */
                                       KNL_ENV(aEnv) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_FILE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_FILE_NOT_EXIST,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sLogMember->mLogMemberPersData.mName );
    }

    STL_CATCH( RAMP_ERR_INVALID_MEMBER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_LOGFILE_NOT_VALID,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sLogMember->mLogMemberPersData.mName );
    }
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) smrCloseLogCursor4Disk( &sLogCursor,
                                           aEnv );
        case 1:
            (void) knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                             &sMemMark,
                                             STL_TRUE, /* aFreeChunk */
                                             KNL_ENV(aEnv) );
    }

    return STL_FAILURE;
}

/** @} */
