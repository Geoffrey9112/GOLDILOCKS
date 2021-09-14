/*******************************************************************************
 * smrLogSyncher.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smrLogSyncher $
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
#include <smrLogDef.h>
#include <smr.h>
#include <smf.h>
#include <smg.h>
#include <smrLogBuffer.h>
#include <smrLogGroup.h>
#include <smrLogMember.h>
#include <smrLogFile.h>
#include <smrLogSyncher.h>
#include <smrPendBuffer.h>
#include <smrLogDump.h>

/**
 * @addtogroup smr
 * @{
 */

/**
 * @brief Warmup Entry를 재구성한다.
 * @param[in] aSrcWarmupEntry  source recovery component
 * @param[out] aDestWarmupEntry  destination recovery component
 * @param[in] aSmfWarmupEntry  datafile component
 * @param[in,out] aEnv  environment
 */  
stlStatus smrReproduceRecoveryWarmupEntry( void       *  aSrcWarmupEntry,
                                           void       ** aDestWarmupEntry,
                                           void       *  aSmfWarmupEntry,
                                           smlEnv     *  aEnv )
{
    stlInt32               sState = 0;
    stlFile                sFile;
    stlInt32               sFlag;
    stlBool                sIsValid;
    stlChar                sBlock[SMF_CONTROLFILE_IO_BLOCK_SIZE + SMF_CONTROLFILE_IO_BUFFER_SIZE];
    smfSysPersData       * sSysPersData;
    stlOffset              sOffset;
    stlInt64               sSize;
    stlSize                sReadBytes;
    stlInt32               i;
    stlInt32               j;
    smfCtrlBuffer          sBuffer;
    smrLogStreamPersData * sLogStreamPersData;
    smrLogStream         * sLogStream;
    smrLogGroup          * sLogGroup;
    smrLogMember         * sLogMember;
    smrWarmupEntry       * sDestWarmupEntry;
    smrWarmupEntry       * sSrcWarmupEntry;
    smfWarmupEntry       * sSmfWarmupEntry;
    stlInt64               sLogIo;

    sSrcWarmupEntry = (smrWarmupEntry *) aSrcWarmupEntry;
    sSmfWarmupEntry = (smfWarmupEntry *) aSmfWarmupEntry;
    
    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                STL_SIZEOF( smrWarmupEntry ),
                                (void**)&sDestWarmupEntry,
                                KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    stlMemset( sDestWarmupEntry, 0x00, STL_SIZEOF(smrWarmupEntry) );

    /**
     * WarmupEntry 멤버 변수
     */ 
    sDestWarmupEntry->mLogFlusherState = sSrcWarmupEntry->mLogFlusherState;
    sDestWarmupEntry->mCheckpointerState = sSrcWarmupEntry->mCheckpointerState;
    sDestWarmupEntry->mEnableLogging = sSrcWarmupEntry->mEnableLogging;
    sDestWarmupEntry->mEnableFlushing = sSrcWarmupEntry->mEnableFlushing;
    sDestWarmupEntry->mBlockedLfsn = sSrcWarmupEntry->mBlockedLfsn;
    sDestWarmupEntry->mLsn = sSrcWarmupEntry->mLsn;
    sDestWarmupEntry->mBlockLogFlushing = sSrcWarmupEntry->mBlockLogFlushing;
    sDestWarmupEntry->mSpinLock = sSrcWarmupEntry->mSpinLock;

    stlMemcpy( sDestWarmupEntry->mSyncValue,
               sSrcWarmupEntry->mSyncValue,
               STL_SIZEOF( stlInt64 ) * 2 );
    
    stlMemcpy( &sDestWarmupEntry->mRecoveryInfo,
               &sSrcWarmupEntry->mRecoveryInfo,
               STL_SIZEOF( smrRecoveryInfo ) );

    stlMemcpy( &sDestWarmupEntry->mChkptInfo,
               &sSrcWarmupEntry->mChkptInfo,
               STL_SIZEOF( smrChkptInfo ) );

    stlMemcpy( &sDestWarmupEntry->mLogBlockHdr,
               &sSrcWarmupEntry->mLogBlockHdr,
               STL_SIZEOF( smrLogBlockHdr ) );

    sFlag = STL_FOPEN_WRITE | STL_FOPEN_READ;

    sLogIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_LOG_FILE_IO,
                                            KNL_ENV(aEnv) );

    if( sLogIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

    sSysPersData = (smfSysPersData *)STL_ALIGN( (stlInt64)sBlock, SMF_CONTROLFILE_IO_BLOCK_SIZE );
    sSize = STL_ALIGN( (stlInt64)STL_SIZEOF( smfSysPersData ), SMF_CONTROLFILE_IO_BLOCK_SIZE );
    /**
     * control file check & open
     */ 
    for( i = 0; i < sSmfWarmupEntry->mCtrlFileCnt; i++ )
    {
        STL_TRY( smgOpenFile( &sFile,
                              sSmfWarmupEntry->mCtrlFileName[i],
                              sFlag,
                              STL_FPERM_OS_DEFAULT,
                              aEnv ) == STL_SUCCESS );
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
            break;
        }

        sState = 0;
        STL_TRY( smgCloseFile( &sFile,
                               aEnv ) == STL_SUCCESS );
    }

    STL_TRY_THROW( i < sSmfWarmupEntry->mCtrlFileCnt, RAMP_ERR_CTRLFILE_INVALID );

    SMF_INIT_CTRL_BUFFER( &sBuffer );
    
    /**
     * smrLogStream 구성
     */
    sOffset = sSysPersData->mLogSectionOffset;
    
    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          KNL_ERROR_STACK(aEnv) ) == STL_SUCCESS );

    sLogStream = &sDestWarmupEntry->mLogStream;
    sLogStreamPersData = &(sLogStream->mLogStreamPersData);

    STL_RING_INIT_HEADLINK( &sLogStream->mLogGroupList,
                            STL_OFFSETOF( smrLogGroup, mLogGroupLink ) );

    /* log pers data */
    STL_TRY( smfReadCtrlFile( &sFile,
                              &sBuffer,
                              (stlChar *) &sDestWarmupEntry->mLogPersData,
                              STL_SIZEOF( smrLogPersData ),
                              aEnv ) == STL_SUCCESS );

    /* log stream pers data */
    STL_TRY( smfReadCtrlFile( &sFile,
                              &sBuffer,
                              (stlChar *) sLogStreamPersData,
                              STL_SIZEOF( smrLogStreamPersData ),
                              aEnv ) == STL_SUCCESS );

    /* log group pers data */
    for( i = 0; i < sLogStreamPersData->mLogGroupCount; i++ )
    {
        STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                    STL_SIZEOF( smrLogGroup ),
                                    (void**)&sLogGroup,
                                    KNL_ENV(aEnv) ) == STL_SUCCESS );

        SMR_INIT_LOG_GROUP( sLogGroup );

        STL_RING_ADD_LAST( &sLogStream->mLogGroupList, sLogGroup );

        STL_TRY( smfReadCtrlFile( &sFile,
                                  &sBuffer,
                                  (stlChar*)&sLogGroup->mLogGroupPersData,
                                  STL_SIZEOF( smrLogGroupPersData ),
                                  aEnv ) == STL_SUCCESS );

        if( sLogGroup->mLogGroupPersData.mState == SMR_LOG_GROUP_STATE_CURRENT )
        {
            /**
             * Current Log group 4 Disk는 설정하지만,
             * Current Log group 4 Buffer는 다를 수 있기 때문에 여기에서 설정하지 않는다.
             */ 
            sLogStream->mCurLogGroup4Disk = sLogGroup;
        }

        /* log memeber pers data */
        for( j = 0; j < sLogGroup->mLogGroupPersData.mLogMemberCount; j++ )
        {
            STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                        STL_SIZEOF( smrLogMember ),
                                        (void**) &sLogMember,
                                        KNL_ENV(aEnv) ) == STL_SUCCESS );

            SMR_INIT_LOG_MEMBER( sLogMember );

            STL_RING_ADD_LAST( &sLogGroup->mLogMemberList, sLogMember );

            STL_TRY( smfReadCtrlFile( &sFile,
                                      &sBuffer,
                                      (stlChar*)&sLogMember->mLogMemberPersData,
                                      STL_SIZEOF( smrLogMemberPersData ),
                                      aEnv ) == STL_SUCCESS );
        }
    }

    sState = 0;
    STL_TRY( smgCloseFile( &sFile,
                           aEnv ) == STL_SUCCESS );

    /* LogStream Member 변수 */
    stlMemcpy( &sLogStream->mStatistics,
               &sSrcWarmupEntry->mLogStream.mStatistics,
               STL_SIZEOF( smrStatistics ) );

    sLogStream->mLogBuffer  = sSrcWarmupEntry->mLogStream.mLogBuffer;
    sLogStream->mPendLogBuffer = sSrcWarmupEntry->mLogStream.mPendLogBuffer;
    sLogStream->mPendBufferCount = sSrcWarmupEntry->mLogStream.mPendBufferCount;

    /* LogGroup Member 변수 */
    sLogStream->mCurLogGroup4Disk->mCurBlockSeq = sSrcWarmupEntry->mLogStream.mCurLogGroup4Disk->mCurBlockSeq;
    sLogStream->mCurLogGroup4Disk->mLogSwitchingSbsn = sSrcWarmupEntry->mLogStream.mCurLogGroup4Disk->mLogSwitchingSbsn;

    *aDestWarmupEntry = sDestWarmupEntry;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CTRLFILE_INVALID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_CORRUPTED_CONTROLFILE,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    if( sState == 1 )
    {
        (void) smgCloseFile( &sFile, aEnv );
    }
    
    return STL_FAILURE;
}

/**
 * @brief Log buffer에서 partialed chained log를 초기화한다.
 * @param[in,out] aLogBuffer  log buffer
 * @param[in] aSbsn  rear sbsn
 * @param[in,out] aEnv  environment
 */ 
stlStatus smrClearChainedLog( smrLogBuffer  * aLogBuffer,
                              smrSbsn         aSbsn,
                              smlEnv        * aEnv )
{
    smrLogBlockHdr  sLogBlockHdr;
    smrLogBlockHdr  sInitBlockHdr;
    smrLsn          sRearLsn = SMR_INVALID_LSN;
    smrSbsn         sSbsn;
    stlChar       * sBlock;
    stlInt8         sLogCount;
    stlInt64        sOffset;
    smrLogHdr       sLogHdr;
    stlChar       * sLogHdrAddr;

    sSbsn = aSbsn;

    sInitBlockHdr.mLsn = SMR_INIT_LSN;
    sInitBlockHdr.mLogCount = 0;
    SMR_SET_BLOCK_INFO( &sInitBlockHdr,
                        SMR_FILE_END_FALSE,
                        SMR_CHAINED_BLOCK_FALSE,
                        SMR_INVALID_WRAP_NO,
                        SMR_BLOCK_WRITE_FINISH );
    sInitBlockHdr.mFirstLogOffset = 0;

    sBlock = SMR_GET_LOG_BUFFER_BLOCK( aLogBuffer,
                                       sSbsn,
                                       SMR_BLOCK_SIZE );

    SMR_READ_LOG_BLOCK_HDR( &sLogBlockHdr, sBlock );

    /**
     * Log Header가 있는 Block을 찾는다: 이전 Block의 Hdr는 초기화한다.
     */ 
    while( sLogBlockHdr.mLogCount == 0 )
    {
        SMR_WRITE_LOG_BLOCK_HDR( sBlock, &sInitBlockHdr );

        sSbsn--;
        
        sBlock = SMR_GET_LOG_BUFFER_BLOCK( aLogBuffer,
                                           sSbsn,
                                           SMR_BLOCK_SIZE );

        SMR_READ_LOG_BLOCK_HDR( &sLogBlockHdr, sBlock );
    }

    /**
     * Block에서 Log Hdr를 찾는다.
     */
    sLogCount = sLogBlockHdr.mLogCount;
    sOffset = sLogBlockHdr.mFirstLogOffset;
    sLogHdrAddr = sBlock + sOffset;
    SMR_READ_LOG_HDR( &sLogHdr, sBlock + sOffset );

    while( sLogCount > 1 )
    {
        sOffset = sOffset + STL_SIZEOF( smrLogHdr ) + sLogHdr.mSize;
        sLogHdrAddr = sBlock + sOffset;

        sRearLsn = sLogHdr.mLsn;

        SMR_READ_LOG_HDR( &sLogHdr, sBlock + sOffset );
        sLogCount--;
    }

    /**
     * Log Count가 1인 경우 Log Count를 0으로 변경되는데
     * 해당 Block에 Chained Log가 있을 수 있다.
     */ 
    if( sLogBlockHdr.mLogCount == 1 )
    {
        sRearLsn = *((smrLsn*) SMR_GET_LOG_BUFFER_BLOCK( aLogBuffer,
                                                         sSbsn - 1,
                                                         SMR_BLOCK_SIZE ) );

        if( sLogBlockHdr.mFirstLogOffset == SMR_BLOCK_SIZE )
        {
            sLogBlockHdr.mFirstLogOffset = 0;
        }
    }

    sLogBlockHdr.mLogCount--;
    sLogBlockHdr.mLsn = sRearLsn;

    SMR_SET_BLOCK_INFO( &sLogBlockHdr,
                        SMR_FILE_END_FALSE,
                        SMR_CHAINED_BLOCK_FALSE,
                        SMR_WRAP_NO( aLogBuffer->mFileSeqNo ),
                        SMR_BLOCK_WRITE_FINISH );

    SMR_WRITE_LOG_BLOCK_HDR( sBlock, &sLogBlockHdr );

    aLogBuffer->mBlockOffset = (stlInt64)(sLogHdrAddr - sBlock );
    aLogBuffer->mRearSbsn = sSbsn;
    aLogBuffer->mRearLsn = sLogBlockHdr.mLsn;

    return STL_SUCCESS;
}

/**
 * @brief 주어진 Log Buffer의 마지막 Block에서 Partail Write된 Log를 초기화한다.
 * @param[in] aWarmupEntry  recovery warmup entry
 * @param[in,out] aEnv  enviornment
 */
stlStatus smrClearPartialLog4Syncher( smrWarmupEntry * aWarmupEntry,
                                      smlEnv         * aEnv )
{
    smrLogBuffer   * sLogBuffer;
    smrSbsn          sSbsn;
    stlChar        * sLastBlock;
    stlChar        * sBlock;
    smrLogBlockHdr   sLogBlockHdr;
    stlInt16         sBlockSize;
    smrLsn           sRearLsn = SMR_INVALID_LSN;
    smrLsn           sRearBlockLsn;

    sLogBuffer  = aWarmupEntry->mLogStream.mLogBuffer;
    sSbsn       = sLogBuffer->mRearSbsn;
    sBlockSize  = aWarmupEntry->mLogStream.mLogStreamPersData.mBlockSize;
    
    sLastBlock = SMR_GET_LOG_BUFFER_BLOCK( sLogBuffer,
                                           sSbsn,
                                           sBlockSize );

    SMR_READ_LOG_BLOCK_HDR( &sLogBlockHdr, sLastBlock );

    /**
     * Partial Log 처리.
     * LogCount == 0
     *   Chained Log == true: Chained Log를 지우기 위해 LogBlockHdr를 재설정.
     *   Cahined Log == false: LogBlockHdr Snap shot을 Copy.
     * LogCount != 0: Log Count가 1 이상이다. LogBlockHdr Snap shot을 Copy.
     */ 
    if( SMR_IS_BLOCK_WRITE_FINISH( &sLogBlockHdr ) == STL_FALSE )
    {
        if( aWarmupEntry->mLogBlockHdr.mLogCount == 0 )
        {
            sBlock = SMR_GET_LOG_BUFFER_BLOCK( sLogBuffer,
                                               sSbsn - 1,
                                               sBlockSize );

            SMR_READ_LOG_BLOCK_HDR( &sLogBlockHdr, sBlock );

            /**
             * Chained Log가 Partial 되었다: Chained Log를 찾아서 지워준다.
             */ 
            if( SMR_IS_CHAINED_BLOCK( &sLogBlockHdr ) == STL_TRUE )
            {
                STL_TRY( smrClearChainedLog( sLogBuffer,
                                             sSbsn,
                                             aEnv ) == STL_SUCCESS );
            }
            else
            {
                /**
                 * LogCount == 0 && Chained Log Partial == FALSE:
                 * 즉 Block switch 되고 Partial 됨.
                 */
                SMR_WRITE_LOG_BLOCK_HDR( sLastBlock, &aWarmupEntry->mLogBlockHdr );
                sRearLsn = *((smrLsn*) SMR_GET_LOG_BUFFER_BLOCK( sLogBuffer,
                                                                 sLogBuffer->mRearSbsn - 1,
                                                                 sBlockSize ));
                sLogBuffer->mRearLsn = sRearLsn;
                sLogBuffer->mBlockOffset = SMR_LOG_HDR_SIZE;
            }
        }
        else
        {
            /**
             * LogCount != 0: 마지막 Block의 Log Count가 1 이상이다.
             */
            SMR_WRITE_LOG_BLOCK_HDR( sLastBlock, &aWarmupEntry->mLogBlockHdr );
            sLogBuffer->mRearLsn = aWarmupEntry->mLogBlockHdr.mLsn;
        }
    }
    else
    {
        /**
         * Log Buffer
         * BLOCK:  1  |  2  |  3  |  4  |  5  |
         *       ------------------------------
         * LSN:   104 | 105 | 105 |  20 |  21 |
         *       ------------------------------
         * RearSbsn이 Old Block 4를 가리킨다면 Block 3에 대해 검사를 해야 한다.
         * LSN 105의 Log가 
         *  Chained라면 Block 2와 3은 초기화 되어야 한다.
         *  Chained가 아니라면 정상으로 간주하고 넘어간다.
         */ 
        sBlock = SMR_GET_LOG_BUFFER_BLOCK( sLogBuffer,
                                           sSbsn - 1,
                                           sBlockSize );

        sRearBlockLsn = SMR_READ_BLOCK_LSN( sBlock );

        if( sRearBlockLsn > sLogBlockHdr.mLsn )
        {
            sSbsn--;

            SMR_READ_LOG_BLOCK_HDR( &sLogBlockHdr, sBlock );
        }
        
        if( SMR_IS_CHAINED_BLOCK( &sLogBlockHdr ) == STL_TRUE )
        {
            STL_TRY( smrClearChainedLog( sLogBuffer,
                                         sSbsn,
                                         aEnv ) == STL_SUCCESS );
        }
    }

    if( sLogBuffer->mFrontSbsn >= sLogBuffer->mRearSbsn )
    {
        /**
         * Partial Log를 지우고 RearSbsn을 감소시키다 보면
         * 이렇게 FrontSbsn을 재조정할 수 있다.
         */ 
        sLogBuffer->mFrontSbsn = sLogBuffer->mRearSbsn - 1;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief current redo logfile에서 유효한 마지막 Block 위치를 얻고, shm memory와 동기화한다.
 * @param[in] aLogStream  log stream
 * @param[in] aLogger  logging manager
 * @param[in,out] aEnv  environtment
 * @remarks log file의 유효한 마지막 Block에서 Lsn을 얻고 Shm memory의 Front Sbsn과 비교 판단한다.
 * Log Buffer의 마지막 Block에 Chained Log가 있고 이 Log가 Partial 상태에 따라 Front Sbsn이
 * 앞으로 또는 뒤로 움직여야 하는 드문 상황이 생길 수 있다.
 */ 
stlStatus smrRefineBufferPos( smrLogStream * aLogStream,
                              stlLogger    * aLogger,
                              smlEnv       * aEnv )
{
    smrLogBuffer        * sLogBuffer;
    smrLogGroup         * sLogGroup;
    smrLogMember        * sLogMember;
    smrLogCursor4Disk     sLogCursor;
    smrLogFileHdr         sLogFileHdr;
    smrLogBlockHdr        sLogBlockHdr;
    smrLsn                sCurBlockLsn = SMR_INVALID_LSN;
    smrLsn                sPrevBlockLsn;   /* 이전 Block Lsn */
    smrLsn                sChainedPrevLsn; /* Chained Log의 이전 Block Lsn */
    smrLsn                sLastValidLsn;
    smrSbsn               sFrontSbsn;
    stlInt32              sState = 0;
    stlInt32              sCurBlockSeq = 0;
    stlInt32              sChainedBlockSeq = 0;
    stlInt32              sChainedBlockCnt = 0;
    stlChar             * sLogBlock;
    stlBool               sEndOfFile;
    stlInt16              sBlockSize;
    stlInt32              sInterErrPos = 0;
    knlRegionMark         sMemMark;

    sLogBuffer = aLogStream->mLogBuffer;
    sBlockSize = aLogStream->mLogStreamPersData.mBlockSize;

    sLogGroup = aLogStream->mCurLogGroup4Disk;

    sLogMember = STL_RING_GET_FIRST_DATA( &sLogGroup->mLogMemberList );

    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               KNL_ENV(aEnv) ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( smrOpenLogCursor4Disk( &sLogCursor,
                                    sLogMember->mLogMemberPersData.mName,
                                    0, /* aHintBlockSeq */
                                    SMR_INVALID_LSN, /* aHintLsn */
                                    &sLogFileHdr,
                                    aEnv ) == STL_SUCCESS );
    sState = 2;

    sChainedPrevLsn   = -1;
    sPrevBlockLsn     = sLogFileHdr.mPrevLastLsn;
    sLogBlockHdr.mLsn = sPrevBlockLsn;

    if( sBlockSize != sLogFileHdr.mBlockSize )
    {
        sInterErrPos = 1;
        STL_THROW( RAMP_ERR_INTERNAL );
    }

    /**
     * Current Log group의 redo log file을 대상으로 마지막으로 쓰인 Lsn과 Block Sequence를 얻는다.
     */ 
    while( STL_TRUE )
    {
        STL_TRY( smrReadBlockCursor4Disk( &sLogCursor,
                                          &sLogBlock,
                                          &sEndOfFile,
                                          aEnv ) == STL_SUCCESS );

        if( sEndOfFile == STL_TRUE )
        {
            /**
             * File 끝가지 읽었다: Log Switching 중이었다.
             */ 
            if( aLogger != NULL )
            {
                STL_TRY( stlLogMsg( aLogger,
                                    KNL_ERROR_STACK( aEnv ),
                                    "[REFINE BUFFER POSITION] Reached to end of file - "
                                    "Block seq(%d), max block count(%d)\n",
                                    sCurBlockSeq,
                                    sLogCursor.mBufferBlockCount )
                         == STL_SUCCESS );
            }

            break;
        }

        sPrevBlockLsn = sLogBlockHdr.mLsn;
        
        SMR_READ_LOG_BLOCK_HDR( &sLogBlockHdr, sLogBlock );

        if( SMR_IS_DUMMY_FILE_END_BLOCK( &sLogBlockHdr ) )
        {
            if( aLogger != NULL )
            {
                STL_TRY( stlLogMsg( aLogger,
                                    KNL_ERROR_STACK( aEnv ),
                                    "[REFINE BUFFER POSITION] Reached to dummy file end block. - "
                                    "Block seq(%d)\n",
                                    sCurBlockSeq )
                         == STL_SUCCESS );
            }
            break;
        }

        if( smrIsValidLog( &sLogCursor,
                           &sLogBlockHdr ) == STL_FALSE )
        {
            break;
        }

        sCurBlockLsn = sLogBlockHdr.mLsn;

        if( SMR_IS_CHAINED_BLOCK( &sLogBlockHdr ) == STL_TRUE )
        {
            if( sPrevBlockLsn != sCurBlockLsn )
            {
                sChainedPrevLsn  = sPrevBlockLsn;
                sChainedBlockSeq = sCurBlockSeq;
            }
        }
        else
        {
            sChainedPrevLsn = -1;
        }

        if( SMR_IS_LAST_FILE_BLOCK( &sLogBlockHdr ) )
        {
            STL_TRY( stlLogMsg( aLogger,
                                KNL_ERROR_STACK( aEnv ),
                                "[REFINE BUFFER POSITION] Reached to end of file - "
                                "Block seq(%d), max block count(%d)\n",
                                sCurBlockSeq,
                                sLogCursor.mBufferBlockCount )
                     == STL_SUCCESS );
            break;
        }

        sCurBlockSeq++;
    }

    /**
     * Redo Log File이 파셜 여부에 따라서 Last Valid Lsn이 달라진다.
     */ 
    if( sChainedPrevLsn != -1) 
    {
        if( aLogger != NULL )
        {
            STL_TRY( stlLogMsg( aLogger,
                                KNL_ERROR_STACK( aEnv ),
                                "[REFINE BUFFER POSITION] Partial log detected - "
                                "Header block seq(%d), current block seq(%d), Lsn(%ld)\n",
                                sChainedBlockSeq,
                                sCurBlockSeq - 1,
                                sLogBlockHdr.mLsn ) == STL_SUCCESS );
        }
        
        sCurBlockSeq  = sChainedBlockSeq;
        sLastValidLsn = sChainedPrevLsn;
    }
    else
    {
        sLastValidLsn = sCurBlockLsn;
    }

    sFrontSbsn = sLogBuffer->mFrontSbsn;
    
    sLogBlock = SMR_GET_LOG_BUFFER_BLOCK( sLogBuffer,
                                          sFrontSbsn,
                                          sBlockSize );

    SMR_READ_LOG_BLOCK_HDR( &sLogBlockHdr, sLogBlock );

    /**
     * FrontSbsn을 수정할 필요가 있다면 수정한다.
     * 
     * Log Buffer
     * BLOCK:  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |
     *       ------------------------------------------------
     * LSN:   104 | 105 | 105 | 105 | 105 | 105 | 106 | 106 |
     *       ------------------------------------------------
     * (Front Sbsn이 가르키는 Block과 같은 Lsn을 갖아야 하는 Lsn으로)
     * sLastValidLsn이 105라면, 최종 FrontSbsn의 위치는 Block 6이 되어야 한다.
     */
    if( sLastValidLsn < sLogBlockHdr.mLsn )
    {
        while( sLastValidLsn < sLogBlockHdr.mLsn )
        {
            sLogBlock = SMR_GET_LOG_BUFFER_BLOCK( sLogBuffer,
                                                  sFrontSbsn,
                                                  sBlockSize );
            
            SMR_READ_LOG_BLOCK_HDR( &sLogBlockHdr, sLogBlock );
            
            if( sLastValidLsn >= sLogBlockHdr.mLsn )
            {
                break;
            }

            if( (sLogBuffer->mRearSbsn - sFrontSbsn) >
                (sLogBuffer->mBufferBlockCount - 1) )
            {
                if( aLogger != NULL )
                {
                    STL_TRY( stlLogMsg( aLogger,
                                        KNL_ERROR_STACK( aEnv ),
                                        "[REFINE BUFFER POSITION] Failed to refine front sbsn - "
                                        "Front sbsn(%ld), rear sbsn(%ld), buffer count(%d)\n",
                                        sFrontSbsn,
                                        sLogBuffer->mRearSbsn,
                                        sLogBuffer->mBufferBlockCount ) == STL_SUCCESS );
                }

                sInterErrPos = 2;
                STL_THROW( RAMP_ERR_INTERNAL );
            }
            
            sFrontSbsn--;
        }
    }
    else
    {
        while( sLastValidLsn > sLogBlockHdr.mLsn )
        {
            sLogBlock = SMR_GET_LOG_BUFFER_BLOCK( sLogBuffer,
                                                  sFrontSbsn,
                                                  sBlockSize );

            SMR_READ_LOG_BLOCK_HDR( &sLogBlockHdr, sLogBlock );

            if( sFrontSbsn >= (sLogBuffer->mRearSbsn - 1) )
            {
                if( aLogger != NULL )
                {
                    STL_TRY( stlLogMsg( aLogger,
                                        KNL_ERROR_STACK( aEnv ),
                                        "[REFINE BUFFER POSITION] Failed to refine front sbsn - "
                                        "Front sbsn(%ld), rear sbsn(%ld), buffer count(%d)\n",
                                        sFrontSbsn,
                                        sLogBuffer->mRearSbsn,
                                        sLogBuffer->mBufferBlockCount ) == STL_SUCCESS );
                }

                sInterErrPos = 3;
                STL_THROW( RAMP_ERR_INTERNAL );
            }

            if( sLastValidLsn <= sLogBlockHdr.mLsn )
            {
                break;
            }

            sFrontSbsn++;
        }

        sChainedBlockCnt = 0;

        while( STL_TRUE )
        {
            if( SMR_IS_CHAINED_BLOCK( &sLogBlockHdr ) == STL_FALSE )
            {
                break;
            }

            sLogBlock = SMR_GET_LOG_BUFFER_BLOCK( sLogBuffer,
                                                  sFrontSbsn + (sChainedBlockCnt + 1),
                                                  sBlockSize );

            SMR_READ_LOG_BLOCK_HDR( &sLogBlockHdr, sLogBlock );

            if( sLastValidLsn < sLogBlockHdr.mLsn )
            {
                break;
            }

            if( (sFrontSbsn + sChainedBlockCnt) >= (sLogBuffer->mRearSbsn - 1) )
            {
                if( aLogger != NULL )
                {
                    STL_TRY( stlLogMsg( aLogger,
                                        KNL_ERROR_STACK( aEnv ),
                                        "[REFINE BUFFER POSITION] Failed to refine front sbsn - "
                                        "Front sbsn(%ld), rear sbsn(%ld), buffer count(%d)\n",
                                        sFrontSbsn,
                                        sLogBuffer->mRearSbsn,
                                        sLogBuffer->mBufferBlockCount ) == STL_SUCCESS );
                }

                sInterErrPos = 4;
                STL_THROW( RAMP_ERR_INTERNAL );
            }

            sChainedBlockCnt++;
        }

        sFrontSbsn += sChainedBlockCnt;
    }

    sLogBuffer->mFrontSbsn = sFrontSbsn;
    
    sLogGroup->mCurBlockSeq = sCurBlockSeq;

    /**
     * FrontSbsn부터 Block을 탐색하여 RearFileBlockSeqNo를 구한다.
     * RearFileBlockSeqNo는 Log Buffer에서 RearSbsn이 위치한 Block이 File과 맵핑되는 번호.
     * (RearSbsn - FrontSbsn)을 (BlockSeq - 1)에 더한다.
     */
    if( (sLogGroup->mLogSwitchingSbsn >= sLogBuffer->mFrontSbsn) &&
        (sLogGroup->mLogSwitchingSbsn <= sLogBuffer->mRearSbsn) )
    {
        sLogBuffer->mRearFileBlockSeqNo = sLogBuffer->mRearSbsn - sLogGroup->mLogSwitchingSbsn;
        sLogBuffer->mFileSeqNo = sLogFileHdr.mLogFileSeqNo + 1;
        aLogStream->mCurLogGroup4Buffer = smrGetNextLogGroup( aLogStream,
                                                              sLogGroup );

        aLogStream->mCurLogGroup4Buffer->mLogSwitchingSbsn = -1;
    }
    else
    {
        sLogBuffer->mRearFileBlockSeqNo = sCurBlockSeq +
            (sLogBuffer->mRearSbsn - sLogBuffer->mFrontSbsn - 1);
        sLogBuffer->mFileSeqNo = sLogFileHdr.mLogFileSeqNo;
        aLogStream->mCurLogGroup4Buffer = sLogGroup;
    }

    sState = 1;
    STL_TRY( smrCloseLogCursor4Disk( &sLogCursor,
                                     aEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_TRUE, /* aFreeChunk */
                                       KNL_ENV(aEnv) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INTERNAL,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      "smrRefineLogBuffer()",
                      sInterErrPos );
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

/**
 * @brief log buffer의 block을 스위치할지 판단하고 스위치한다.
 * @param[in,out] aLogStream  log stream
 * @param[in,out] aEnv  environment 
 */ 
stlStatus smrSwitchBufferBlock( smrLogStream * aLogStream,
                                smlEnv       * aEnv )
{
    smrLogBuffer   * sLogBuffer;
    smrLsn           sLsn;
    smrLsn           sPrevBlockLsn;
    stlChar        * sBlock;

    sLogBuffer = aLogStream->mLogBuffer;

    sBlock = SMR_GET_LOG_BUFFER_BLOCK( sLogBuffer,
                                       sLogBuffer->mRearSbsn - 1,
                                       SMR_BLOCK_SIZE );

    sPrevBlockLsn = SMR_READ_BLOCK_LSN( sBlock );

    sBlock = SMR_GET_LOG_BUFFER_BLOCK( sLogBuffer,
                                       sLogBuffer->mRearSbsn,
                                       SMR_BLOCK_SIZE );

    sLsn = SMR_READ_BLOCK_LSN( sBlock );

    /**
     * RearSbsn의 Block Lsn이 바로 이전 Block Lsn 보다 작다면 Block Switch 하면 안된다.
     */ 
    STL_TRY_THROW( sLsn >= sPrevBlockLsn, RAMP_SKIP );

    if( sLogBuffer->mBlockOffset > SMR_BLOCK_HDR_SIZE )
    {
        if( sLogBuffer->mRearFileBlockSeqNo + 1 >=
            smrGetMaxFileBlockCount( aLogStream, aLogStream->mCurLogGroup4Buffer ) )
        {
            aLogStream->mCurLogGroup4Buffer->mLogSwitchingSbsn = sLogBuffer->mRearSbsn + 1;

            SMR_BUFFER_BLOCK_SWITCH( sLogBuffer );

            sLogBuffer->mFileSeqNo += 1;
            sLogBuffer->mRearFileBlockSeqNo = 0;
            aLogStream->mCurLogGroup4Buffer = smrGetNextLogGroup( aLogStream,
                                                                  aLogStream->mCurLogGroup4Buffer );
                                                                   
        }
        else
        {
            SMR_BUFFER_BLOCK_SWITCH( sLogBuffer );
        }
    }

    STL_RAMP( RAMP_SKIP );

    return STL_SUCCESS;
}


/**
 * @brief 모든 Log를 Flush한다.
 * @param[in]  aSmrWarmupEntry  recovery component entry
 * @param[out] aSwitchedLogfile  log file switching flag
 * @param[in]  aSilent  print message silent flag
 * @param[in]  aLogger  logging manager
 * @param[in,out] aEnv  environment
 */ 
stlStatus smrFlushAllLogs4Syncher( void      * aSmrWarmupEntry,
                                   stlBool   * aSwitchedLogfile,
                                   stlBool     aSilent,
                                   stlLogger * aLogger,
                                   smlEnv    * aEnv )
{
    smrWarmupEntry * sWarmupEntry;
    smrLogStream   * sLogStream;
    smrLogBuffer   * sLogBuffer;
    smrLsn           sLastLsn;
    smrSbsn          sLastSbsn;
    smrLsn           sFrontLsn;
    smrSbsn          sFrontSbsn;
    stlChar        * sBlock;
    smrLogGroup    * sFirstGroup;
    smrLogGroup    * sLastGroup;
    
    /**
     * Control file의 LogGroupPersData를 이용하기 위해 WarmupEntry를 재구성한다.
     */
    sWarmupEntry = (smrWarmupEntry*) aSmrWarmupEntry;
    sLogStream   = &sWarmupEntry->mLogStream;
    sLogBuffer   = sLogStream->mLogBuffer;
    
    gSmrLogBlockSize = sLogStream->mLogStreamPersData.mBlockSize;

    /**
     * 마지막 Log Block의 Patial 상태를 확인.
     */ 
    STL_TRY( smrClearPartialLog4Syncher( sWarmupEntry,
                                         aEnv ) == STL_SUCCESS );

    STL_TRY( smrRefineBufferPos( sLogStream,
                                 aLogger,
                                 aEnv ) == STL_SUCCESS );

    sFrontSbsn = sLogBuffer->mFrontSbsn;
    sBlock = SMR_GET_LOG_BUFFER_BLOCK( sLogBuffer,
                                       sFrontSbsn,
                                       SMR_BLOCK_SIZE );
    sFrontLsn = SMR_READ_BLOCK_LSN( sBlock );
    sFirstGroup = sLogStream->mCurLogGroup4Disk;
    
    STL_TRY( smrSwitchBufferBlock( sLogStream,
                                   aEnv ) == STL_SUCCESS );

    sLastSbsn = sLogBuffer->mRearSbsn;
    
    STL_TRY( smrFlushBuffer4Syncher( sLogStream,
                                     sLastSbsn,
                                     aSwitchedLogfile,
                                     aSilent,
                                     aLogger,
                                     aEnv ) == STL_SUCCESS );

    sLastLsn = sWarmupEntry->mLsn - 1;
    
    STL_TRY( smrApplyPendingLogs4Syncher( sLogStream,
                                          sLastLsn,
                                          &sLastSbsn,
                                          aSwitchedLogfile,
                                          aSilent,
                                          aLogger,
                                          aEnv ) == STL_SUCCESS );

    STL_TRY( smrSwitchBufferBlock( sLogStream,
                                   aEnv ) == STL_SUCCESS );

    sLastSbsn = sLogBuffer->mRearSbsn;

    STL_TRY( smrFlushBuffer4Syncher( sLogStream,
                                     sLastSbsn,
                                     aSwitchedLogfile,
                                     aSilent,
                                     aLogger,
                                     aEnv ) == STL_SUCCESS );

    sLastSbsn = sLogBuffer->mRearSbsn - 1;
    sBlock = SMR_GET_LOG_BUFFER_BLOCK( sLogBuffer,
                                       sLastSbsn,
                                       SMR_BLOCK_SIZE );
    sLastLsn = SMR_READ_BLOCK_LSN( sBlock );
    sLastGroup = sLogStream->mCurLogGroup4Disk;
    
    if( aSilent == STL_FALSE )
    {
        stlPrintf( "\n[FLUSH] Log buffer flushed - "
                   "Log group from id (%d) to (%d), "
                   "lsn from (%ld) to (%ld), "
                   "total buffer (%ld) bytes\n",
                   sFirstGroup->mLogGroupPersData.mLogGroupId,
                   sLastGroup->mLogGroupPersData.mLogGroupId,
                   sFrontLsn,
                   sLastLsn,
                   ( (sLastLsn >= sFrontLsn) ?
                     ((sLastSbsn - sFrontSbsn) * SMR_BLOCK_SIZE) : 0 ) );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Stream의 Log Buffer를 Flush.
 * @param[in] aLogStream  log stream
 * @param[in] aTargetSbsn  target log stream buffer sequence number
 * @param[out] aSwitchedLogfile  log file switching flag
 * @param[in] aSilent  silent msg
 * @param[in] aLogger  logging manager
 * @param[in,out] aEnv  envrionment
 * @see smrFlushBufferInternal()
 */
stlStatus smrFlushBuffer4Syncher( smrLogStream * aLogStream,
                                  smrSbsn        aTargetSbsn,
                                  stlBool      * aSwitchedLogfile,
                                  stlBool        aSilent,
                                  stlLogger    * aLogger,
                                  smlEnv       * aEnv )
{
    smrLogBuffer * sLogBuffer;
    smrSbsn        sTargetSbsn;
    stlInt64       sMaxFlushLogBlockCount;
    smrLsn         sFrontLsn;
    smrLsn         sRearLsn;
    stlChar      * sBlock;
    stlInt32       sInterErrPos = 0;
    stlInt32       sFrontBlockSeq;
    stlInt32       sLastBlockSeq;

    sLogBuffer = aLogStream->mLogBuffer;
    
    STL_TRY_THROW( aTargetSbsn > sLogBuffer->mFrontSbsn, RAMP_SUCCESS );

    sMaxFlushLogBlockCount = SMR_FLUSH_LOG_BLOCK_COUNT;

    if( aTargetSbsn >= sLogBuffer->mRearSbsn )
    {
        aTargetSbsn = sLogBuffer->mRearSbsn - 1;
    }

    sFrontBlockSeq = aLogStream->mCurLogGroup4Disk->mCurBlockSeq;
    sBlock = SMR_GET_LOG_BUFFER_BLOCK( sLogBuffer,
                                       sLogBuffer->mFrontSbsn,
                                       SMR_BLOCK_SIZE );
    sFrontLsn = SMR_READ_BLOCK_LSN( sBlock );

    sBlock = SMR_GET_LOG_BUFFER_BLOCK( sLogBuffer,
                                       aTargetSbsn,
                                       SMR_BLOCK_SIZE );
    sRearLsn = SMR_READ_BLOCK_LSN( sBlock );

    while( aTargetSbsn > sLogBuffer->mFrontSbsn )
    {
        sTargetSbsn = STL_MIN( sLogBuffer->mFrontSbsn + sMaxFlushLogBlockCount,
                               aTargetSbsn );

        if( SMR_BUFFER_POS( sTargetSbsn, sLogBuffer ) <= SMR_FRONT_POS(sLogBuffer) )
        {
            STL_TRY( smrWriteBufferBlocks4Syncher( aLogStream,
                                                   (sLogBuffer->mBufferBlockCount -
                                                    (SMR_FRONT_POS(sLogBuffer) + 1)),
                                                   aSwitchedLogfile,
                                                   aLogger,
                                                   aEnv ) == STL_SUCCESS );

            /**
             * @note gsyncher는 다른 process를 다 종료시키기 때문에 이런 상황이 없어야 하지 않나?
             */ 
            if( aTargetSbsn <= sLogBuffer->mFrontSbsn )
            {
                sInterErrPos = 1;
                STL_THROW( RAMP_ERR_INTERNAL );
            }

            STL_TRY( smrWriteBufferBlocks4Syncher( aLogStream,
                                                   SMR_BUFFER_POS(sTargetSbsn, sLogBuffer) + 1,
                                                   aSwitchedLogfile,
                                                   aLogger,
                                                   aEnv ) == STL_SUCCESS );
        }
        else
        {
            STL_TRY( smrWriteBufferBlocks4Syncher( aLogStream,
                                                   (SMR_BUFFER_POS(sTargetSbsn, sLogBuffer) -
                                                    SMR_FRONT_POS(sLogBuffer)),
                                                   aSwitchedLogfile,
                                                   aLogger,
                                                   aEnv ) == STL_SUCCESS );
        }
    }

    sLastBlockSeq = aLogStream->mCurLogGroup4Disk->mCurBlockSeq;

    if( (aLogger != NULL) &&
        ((sLastBlockSeq - sFrontBlockSeq) > 0) )
    {
        STL_TRY( stlLogMsg( aLogger,
                            KNL_ERROR_STACK( aEnv ),
                            "[LOG FLUSH] Log buffer flushed - "
                            "log group id (%d), "
                            "file block seq from (%d) to (%d), "
                            "lsn from (%ld) to (%ld)\n",
                            aLogStream->mCurLogGroup4Disk->mLogGroupPersData.mLogGroupId,
                            sFrontBlockSeq,
                            sLastBlockSeq,
                            sFrontLsn,
                            sRearLsn ) == STL_SUCCESS );
    }

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INTERNAL,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      "smrFlushBuffer4Syncher()",
                      sInterErrPos );
    }

    STL_FINISH;

    if( aLogger != NULL )
    {
        (void) stlLogMsg( aLogger,
                          KNL_ERROR_STACK( aEnv ),
                          "[LOG FLUSH] Log buffer flushing failed.\n" );
    }

    return STL_FAILURE;
}

/**
 * @brief Block 단위로 Log Buffer를 파일로 Flush
 * @param[in] aLogStream  log stream
 * @param[in] aFlushBlockCount  taget block count
 * @param[out] aSwitchedLogfile  log file switching flag
 * @param[in] aLogger  logging manager
 * @param[in,out] aEnv  environment
 * @see smrWriteBufferBlocks()
 */
stlStatus smrWriteBufferBlocks4Syncher( smrLogStream * aLogStream,
                                        stlInt64       aFlushBlockCount,
                                        stlBool      * aSwitchedLogfile,
                                        stlLogger    * aLogger, 
                                        smlEnv       * aEnv )
{
    smrLogBuffer   * sLogBuffer;
    stlInt32         sBlockSize;
    stlInt64         sFlushBytes = 0;
    stlChar        * sBlock;
    smrLogBlockHdr   sLogBlockHdr;
    void           * sTargetBuffer;
    smrSbsn          sLastSbsn;
    stlInt64         sLogSwitchingSbsn;
    stlInt64         sFlushBlockCount;
    stlBool          sIsValid = STL_FALSE;
    stlInt32         sInterErrPos = 0;

    sLogBuffer = aLogStream->mLogBuffer;
    sBlockSize = aLogStream->mLogStreamPersData.mBlockSize;

    sLogSwitchingSbsn = aLogStream->mCurLogGroup4Disk->mLogSwitchingSbsn;
    sFlushBlockCount = aFlushBlockCount;

    /**
     * Flush하고자 하는 영역 내에 Log Swithcing SBSN이 존재하는 경우 Log Swithching이 발생한다.
     * flush할 buffer에 다수개의 switching sbsn이 존재할수 있다.
     */
    while( ((sLogBuffer->mFrontSbsn + 1) <= sLogSwitchingSbsn) &&
           ((sLogBuffer->mFrontSbsn + 1 + sFlushBlockCount) > sLogSwitchingSbsn) )
    {
        /**
         * Log file로 flush하려고 하는 마지막 Sbsn을 구한다.
         * 이는 log switching 후 flush해야 할 log가 남아있는지를 확인하기 위해서 필요하다.
         */
        sLastSbsn = sLogBuffer->mFrontSbsn + sFlushBlockCount;

        STL_TRY( smrMarkFileEnd( sLogBuffer,
                                 sLogSwitchingSbsn - 1,
                                 sBlockSize,
                                 aEnv )
                 == STL_SUCCESS );

        sFlushBytes = (sLogSwitchingSbsn - (sLogBuffer->mFrontSbsn + 1)) * sBlockSize;
        sTargetBuffer = sLogBuffer->mBuffer + (SMR_BUFFER_POS(sLogBuffer->mFrontSbsn + 1,
                                                              sLogBuffer)
                                               * sBlockSize);

        /**
         * Log Switching SBSN과 (Front SBSN + 1)이 동일하다면, 즉,
         * FILE_END가 설정된 block이 이미 flush되었다면, FILE_END를 설정한
         * Dummy block을 하나 flush한다.
         */
        if( sLogSwitchingSbsn == (sLogBuffer->mFrontSbsn + 1) )
        {
            STL_DASSERT( sFlushBytes == 0 );
            
            STL_TRY( smrWriteDummyFileEndBlock( aLogStream,
                                                aEnv ) == STL_SUCCESS );
        }
        else
        {
            STL_TRY( smrWriteFileBlocks( aLogStream,
                                         sTargetBuffer,
                                         sFlushBytes,
                                         STL_TRUE, /* aDoValidate */
                                         &sIsValid, /* aIsValid */
                                         aLogger,
                                         aEnv ) == STL_SUCCESS );

            if( sIsValid == STL_FALSE )
            {
                sInterErrPos = 1;
                STL_THROW( RAMP_ERR_INTERNAL );
            }
        }

        sLogBuffer->mFrontSbsn += (sFlushBytes / sBlockSize);

        sBlock = sLogBuffer->mBuffer + (SMR_FRONT_POS(sLogBuffer) * sBlockSize);
        SMR_READ_LOG_BLOCK_HDR( &sLogBlockHdr, sBlock );

        /**
         * chained log가 존재하는 block이라면 이전 lsn을 stable lsn으로 설정한다.
         */
        if( SMR_IS_CHAINED_BLOCK( &sLogBlockHdr ) == STL_TRUE )
        {
            sLogBuffer->mFrontLsn = sLogBlockHdr.mLsn - 1;
        }
        else
        {
            sLogBuffer->mFrontLsn = sLogBlockHdr.mLsn;
        }

        STL_TRY( smrLogSwitching4Syncher( aLogStream,
                                          aSwitchedLogfile,
                                          aLogger,
                                          aEnv ) == STL_SUCCESS );
        
        /**
         * next group의 switching sbsn을 검사한다.
         */
        sLogSwitchingSbsn = aLogStream->mCurLogGroup4Disk->mLogSwitchingSbsn;
        sFlushBlockCount = sLastSbsn - sLogBuffer->mFrontSbsn;
    }

    sFlushBytes = (sFlushBlockCount * sBlockSize);
    sTargetBuffer = sLogBuffer->mBuffer + (SMR_BUFFER_POS(sLogBuffer->mFrontSbsn + 1,
                                                          sLogBuffer)
                                           * sBlockSize);

    if( sFlushBytes > 0 )
    {
        STL_TRY( smrWriteFileBlocks( aLogStream,
                                     sTargetBuffer,
                                     sFlushBytes,
                                     STL_TRUE, /* aDoValidate */
                                     &sIsValid,
                                     aLogger,
                                     aEnv ) == STL_SUCCESS );

        if( sIsValid == STL_FALSE )
        {
            sInterErrPos = 2;
            STL_THROW( RAMP_ERR_INTERNAL );
        }
    }
    
    sLogBuffer->mFrontSbsn += (sFlushBytes / sBlockSize);
    sLogBuffer->mFrontFileBlockSeqNo += (sFlushBytes / sBlockSize);

    sBlock = sLogBuffer->mBuffer + (SMR_FRONT_POS(sLogBuffer) * sBlockSize);
    SMR_READ_LOG_BLOCK_HDR( &sLogBlockHdr, sBlock );

    /**
     * chained log가 존재하는 block이라면 이전 lsn을 stable lsn으로 설정한다.
     */
    if( SMR_IS_CHAINED_BLOCK( &sLogBlockHdr ) == STL_TRUE )
    {
        sLogBuffer->mFrontLsn = sLogBlockHdr.mLsn - 1;
    }
    else
    {
        sLogBuffer->mFrontLsn = sLogBlockHdr.mLsn;
    }

    STL_DASSERT( sLogBuffer->mFrontSbsn < sLogBuffer->mRearSbsn );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INTERNAL,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      "smrWriteBufferBlocks4Syncher()",
                      sInterErrPos );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief pending log buffer를 log buffer에 기록한다.
 * @param[in] aLogStream  log stream
 * @param[in] aEndLsn  last lsn
 * @param[out] aSbsn  written last sbsn
 * @param[out] aSwitchedLogfile  log file switching flag
 * @param[in] aSilent  silent msg
 * @param[in] aLogger  logging manager
 * @param[in,out] aEnv  environment
 */ 
stlStatus smrApplyPendingLogs4Syncher( smrLogStream * aLogStream,
                                       smrLsn         aEndLsn,
                                       smrSbsn      * aSbsn,
                                       stlBool      * aSwitchedLogfile,
                                       stlBool        aSilent,
                                       stlLogger    * aLogger,
                                       smlEnv       * aEnv )
{
    smrLogHdr        * sLogHdr;
    void             * sLogData = NULL;
    smrPendLogCursor   sPendLogCursor;
    
    STL_PARAM_VALIDATE( aLogStream != NULL, KNL_ERROR_STACK( aEnv ) );

    STL_TRY_THROW( aLogStream->mPendBufferCount > 0, RAMP_SUCCESS );

    STL_TRY( smrPreparePendingLogCursor( aLogStream,
                                         aEndLsn,
                                         &sPendLogCursor,
                                         aEnv ) == STL_SUCCESS );

    while( STL_TRUE )
    {
        STL_TRY( smrReadPendingLog( aLogStream,
                                    &sPendLogCursor,
                                    &sLogHdr,
                                    &sLogData,
                                    aEnv ) == STL_SUCCESS );

        if( sLogData == NULL )
        {
            break;
        }

        STL_TRY( smrWritePendingLogToLogBuffer( aLogStream,
                                                sLogHdr,
                                                sLogData,
                                                aSbsn,
                                                aSwitchedLogfile,
                                                aSilent,
                                                aLogger,
                                                aEnv ) == STL_SUCCESS );

        STL_TRY( smrMovePendingLogCursor( &sPendLogCursor,
                                          aEnv ) == STL_SUCCESS );
    }
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
}


/**
 * @brief pending log를 log buffer에 기록한다.
 * @param[in] aLogStream  log stream
 * @param[in] aLogHdr  log header
 * @param[in] aLogData  log data
 * @param[out] aWrittenSbsn  written last sbsn
 * @param[out] aSwitchedLogfile  log file switching flag
 * @param[in] aSilent  silent msg
 * @param[in] aLogger  logging manager
 * @param[in,out] aEnv  environment
 * @see smrWriteStreamLogInternal()
 */
stlStatus smrWritePendingLogToLogBuffer( smrLogStream   * aLogStream,
                                         smrLogHdr      * aLogHdr,
                                         void           * aLogData,
                                         smrSbsn        * aWrittenSbsn,
                                         stlBool        * aSwitchedLogfile,
                                         stlBool          aSilent,
                                         stlLogger      * aLogger,
                                         smlEnv         * aEnv )
{
    smrLogBuffer       * sLogBuffer;
    stlInt32             sWrittenBytes;
    stlUInt32            sTotalWrittenBytes = 0;
    smrLid               sWrittenLid = SMR_INVALID_LID; 
    void               * sLogData;
    stlInt32             sNeedBlockCount = 0;
    smrLogHdr          * sLogHdr;
    stlInt64             sWrittenSbsn = SMR_INVALID_SBSN;
    stlInt32             sInterErrPos = 0;

    sLogBuffer = aLogStream->mLogBuffer;

    STL_DASSERT( (sLogBuffer->mRearSbsn - sLogBuffer->mFrontSbsn) <=
                 sLogBuffer->mBufferBlockCount );

    while( STL_TRUE )
    {
        sNeedBlockCount = smrNeedBlockCount( aLogStream, aLogHdr->mSize );

        if( (sLogBuffer->mRearFileBlockSeqNo + sNeedBlockCount) >=
            smrGetMaxFileBlockCount( aLogStream, aLogStream->mCurLogGroup4Buffer ) )
        {
            /**
             * @note File across chained log 방지
             * * Log Group을 변경한다.
             * * Log Group이 Usable이 아닌 상황이 발생하면 안된다.
             */
            if( sLogBuffer->mBlockOffset > SMR_BLOCK_HDR_SIZE )
            {
                /**
                 * cache coherency 문제로 인해 RearSbsn을 증가시키기 전에
                 * LogSwitchingSbsn을 증가 시킨다.
                 */
                aLogStream->mCurLogGroup4Buffer->mLogSwitchingSbsn = sLogBuffer->mRearSbsn + 1;
                SMR_BUFFER_BLOCK_SWITCH( sLogBuffer );
            }
            else
            {
                aLogStream->mCurLogGroup4Buffer->mLogSwitchingSbsn = sLogBuffer->mRearSbsn;
            }
            
            sLogBuffer->mFileSeqNo += 1;
            sLogBuffer->mRearFileBlockSeqNo = 0;
            aLogStream->mCurLogGroup4Buffer = smrGetNextLogGroup( aLogStream,
                                                                  aLogStream->mCurLogGroup4Buffer );

            if( SMR_IS_USABLE_LOG_FILE( aLogStream->mCurLogGroup4Buffer ) == STL_FALSE )
            {
                sInterErrPos = 1;
                STL_THROW( RAMP_ERR_INTERNAL );
            }
        }
        else
        {
            /**
             * @note log buffer size <<  pend buffer count * pend buffer size 인 경우
             * log buffer를 여러번 flush 하는 상황이 생길 수 있다.
             */ 
            if( (sLogBuffer->mRearSbsn + sNeedBlockCount - sLogBuffer->mFrontSbsn) >=
                (sLogBuffer->mBufferBlockCount - 1) )
            {
                STL_TRY( smrFlushBuffer4Syncher( aLogStream,
                                                 sLogBuffer->mRearSbsn,
                                                 aSwitchedLogfile,
                                                 aSilent,
                                                 aLogger,
                                                 aEnv ) == STL_SUCCESS );
            }
            
            break;
        }
    }

    sLogHdr = aLogHdr;

    /**
     * LSN 기록 순서는 보장되어야 한다.
     */
    KNL_ASSERT( aLogHdr->mLsn > sLogBuffer->mRearLsn,
                KNL_ENV(aEnv),
                ("LSN(%ld), REAR_LSN(%ld)",
                 aLogHdr->mLsn,
                 sLogBuffer->mRearLsn) );

    while( sTotalWrittenBytes < aLogHdr->mSize )
    {
        sLogData = aLogData + sTotalWrittenBytes;

        STL_TRY( smrWriteLogToBlock4Syncher( aLogStream,
                                             sLogHdr,
                                             aLogHdr->mLsn,
                                             sLogData,
                                             aLogHdr->mSize - sTotalWrittenBytes,
                                             &sWrittenBytes,
                                             &sWrittenSbsn,
                                             aEnv ) == STL_SUCCESS );

        sLogHdr = NULL;
        sLogBuffer->mRearLsn = aLogHdr->mLsn;
        sTotalWrittenBytes += sWrittenBytes;
    }

    sLogBuffer->mRearLid = sWrittenLid;

    *aWrittenSbsn = sWrittenSbsn;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INTERNAL,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      "smrWritePendingLogToLogBuffer()",
                      sInterErrPos );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief pending log를 log buffer에 기록한다.
 * @param[in] aLogStream  log stream
 * @param[in] aLogHdr  log header
 * @param[in] aLsn  log sequence number
 * @param[in] aLogData  log data
 * @param[in] aLogSize  log size
 * @param[out] aWrittenBytes  written bytes
 * @param[out] aWrittenSbsn  written last sbsn
 * @param[in,out] aEnv  environment
 */
stlStatus smrWriteLogToBlock4Syncher( smrLogStream  * aLogStream,
                                      smrLogHdr     * aLogHdr,
                                      smrLsn          aLsn,
                                      void          * aLogData,
                                      stlInt32        aLogSize,
                                      stlInt32      * aWrittenBytes,
                                      smrSbsn       * aWrittenSbsn,
                                      smlEnv        * aEnv )
{
    smrLogBuffer   * sLogBuffer;
    stlChar        * sBlock;
    stlInt32         sBlockSize;
    smrLogBlockHdr   sLogBlockHdr;
    stlInt32         sWriteBytes;
    stlUInt16        sBlockOffset;

    sLogBuffer = aLogStream->mLogBuffer;
    sBlockSize = aLogStream->mLogStreamPersData.mBlockSize;

    STL_DASSERT( smrValidateLogBuffer( sLogBuffer,
                                       sBlockSize ) == STL_TRUE );
    
    STL_ASSERT( sLogBuffer->mRearFileBlockSeqNo <
                 smrGetMaxFileBlockCount(aLogStream, aLogStream->mCurLogGroup4Buffer) );

    if( (sLogBuffer->mBlockOffset + SMR_LOG_HDR_SIZE) > sBlockSize )
    {
        SMR_BUFFER_BLOCK_SWITCH( sLogBuffer );
    }

    sBlock = sLogBuffer->mBuffer + ( SMR_REAR_POS( sLogBuffer ) * sBlockSize );

    if( sLogBuffer->mBlockOffset == SMR_BLOCK_HDR_SIZE )
    {
        sLogBlockHdr.mLsn = aLsn;
        sLogBlockHdr.mLogCount = 0;
        sLogBlockHdr.mFirstLogOffset = 0;
        SMR_SET_BLOCK_INFO( &sLogBlockHdr,
                            SMR_FILE_END_FALSE,
                            SMR_CHAINED_BLOCK_FALSE,
                            SMR_WRAP_NO( sLogBuffer->mFileSeqNo ),
                            SMR_BLOCK_WRITE_FINISH );

        SMR_WRITE_LOG_BLOCK_HDR( sBlock, &sLogBlockHdr );
    }
    else
    {
        SMR_READ_LOG_BLOCK_HDR( &sLogBlockHdr, sBlock );
    }

    if( aLogHdr != NULL )
    {
        if( sLogBlockHdr.mLogCount == 0 )
        {
            sLogBlockHdr.mFirstLogOffset = sLogBuffer->mBlockOffset;
        }
        sLogBlockHdr.mLogCount++;
        sLogBlockHdr.mLsn = aLsn;
        
        STL_ASSERT( sLogBuffer->mBlockOffset >= sLogBlockHdr.mFirstLogOffset );
        
        SMR_WRITE_LOG_HDR( sBlock + sLogBuffer->mBlockOffset, aLogHdr );
        sLogBuffer->mBlockOffset += SMR_LOG_HDR_SIZE;
    }
    else
    {
        STL_WRITE_INT64( sBlock, &aLsn );
    }

    if( aLogSize > (sBlockSize - sLogBuffer->mBlockOffset) )
    {
        sWriteBytes = sBlockSize - sLogBuffer->mBlockOffset;

        /**
         * chained block 정보 설정
         */
        SMR_SET_CHAINED_BLOCK( &sLogBlockHdr );
    }
    else
    {
        sWriteBytes = aLogSize;
    }

    /**
     * @note PengdingLog에 대해서만 이 함수에 진입을 하여 LOGDATA_TYPE을 고려하지 않는다.
     */
    stlMemcpy( sBlock + sLogBuffer->mBlockOffset,
               aLogData,
               sWriteBytes );

    if( aWrittenSbsn != NULL )
    {
        *aWrittenSbsn = sLogBuffer->mRearSbsn;
    }

    *aWrittenBytes = sWriteBytes;

    sLogBuffer->mBlockOffset += sWriteBytes;

    SMR_WRITE_LOG_BLOCK_HDR( sBlock, &sLogBlockHdr );
    
    if( SMR_GET_LOG_COUNT( sBlock ) == 0 )
    {
        sBlockOffset = sLogBuffer->mBlockOffset;
        SMR_WRITE_FIRST_OFFSET( sBlock, sBlockOffset );
        STL_DASSERT( sBlockOffset > 0 );
    }

    STL_DASSERT( smrValidateLogBuffer( sLogBuffer,
                                       sBlockSize ) == STL_TRUE );

    return STL_SUCCESS;
}


/**
 * @brief log switching
 * @param[in] aLogStream  log stream
 * @param[out] aSwitchedLogfile  log file switching flag
 * @param[in]  aLogger  logging manager
 * @param[in,out] aEnv  environment
 */ 
stlStatus smrLogSwitching4Syncher( smrLogStream * aLogStream,
                                   stlBool      * aSwitchedLogfile,
                                   stlLogger    * aLogger,
                                   smlEnv       * aEnv )
{
    smrLogBuffer        * sLogBuffer;
    smrLogGroup         * sCurLogGroup;
    smrLogGroup         * sNextLogGroup;
    smrLogGroupPersData * sLogGroupPersData;

    /**
     * @note gmaster가 down된 상태이기 때문에 checkpoint event와 log archving을 하지 않는다.
     */

    aLogStream->mLogStreamPersData.mCurFileSeqNo++;
    
    sLogBuffer = aLogStream->mLogBuffer;
    sLogBuffer->mFrontFileBlockSeqNo = 0;
    
    sCurLogGroup = aLogStream->mCurLogGroup4Disk;
    
    sLogGroupPersData = &sCurLogGroup->mLogGroupPersData;
    sLogGroupPersData->mState = SMR_LOG_GROUP_STATE_ACTIVE;

    sNextLogGroup = smrGetNextLogGroup( aLogStream, aLogStream->mCurLogGroup4Disk );

    /**
     * Log File Switching시 현재의 next log group은 반드시 재사용 가능한 상태여야 한다.
     * - CURRENT와 ACTIVE 사이의 최소 Gap은 1 이하일 경우는 로깅자체가 중단되기 때문에
     *   항상 CURRENT log group 다음 log group은 재사용 가능 상태이다.
     */
    if( SMR_IS_USABLE_LOG_FILE( sNextLogGroup ) == STL_FALSE )
    {
        if( aLogger != NULL )
        {
            STL_TRY( stlLogMsg( aLogger,
                                KNL_ERROR_STACK( aEnv ),
                                "[LOG SWITCHING] Next log group is not usable\n" )
                     == STL_SUCCESS );
        }

        STL_DASSERT( 0 );
    }
    
    sLogGroupPersData = &sNextLogGroup->mLogGroupPersData;
    sLogGroupPersData->mState = SMR_LOG_GROUP_STATE_CURRENT;
    sLogGroupPersData->mFileSeqNo = aLogStream->mLogStreamPersData.mCurFileSeqNo;

    sNextLogGroup->mCurBlockSeq = 0;
    sLogGroupPersData->mPrevLastLsn = sLogBuffer->mFrontLsn;

    STL_TRY( smrWriteLogfileHdr( aLogStream,
                                 sNextLogGroup,
                                 NULL,  /* aLogFileHdr */
                                 aEnv ) == STL_SUCCESS );

    aLogStream->mCurLogGroup4Disk = sNextLogGroup;

    *aSwitchedLogfile = STL_TRUE;

    if( aLogger != NULL )
    {
        STL_TRY( stlLogMsg( aLogger,
                            KNL_ERROR_STACK(aEnv),
                            "[LOG SWITCHING] From group '%d' to group '%d', "
                            "log switching is done.\n",
                            sCurLogGroup->mLogGroupPersData.mLogGroupId,
                            sNextLogGroup->mLogGroupPersData.mLogGroupId )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( aLogger != NULL )
    {
        (void)stlLogMsg( aLogger,
                         KNL_ERROR_STACK(aEnv),
                         "[LOG SWITCHING] Log switching is failed.\n" );
    }

    return STL_FAILURE;
}


/**
 * @brief syncher를 위해 control file에 log section을 기록한다.
 * @param[in] aFile  file descriptor
 * @param[in] aWarmupEntry  recovery warmup entry 
 * @param[out] aWrittenBytes  written bytes
 * @param[in,out] aEnv  environment
 */ 
stlStatus smrWriteLogCtrlSection4Syncher( stlFile  * aFile,
                                          void     * aWarmupEntry,
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
    smrWarmupEntry       * sWarmupEntry;

    SMF_INIT_CTRL_BUFFER( &sBuffer );

    sWarmupEntry = (smrWarmupEntry*) aWarmupEntry;

    /**
     * Logging 관련 Persistent Information 기록
     */
    STL_TRY( smfWriteCtrlFile( aFile,
                               &sBuffer,
                               (stlChar *)&sWarmupEntry->mLogPersData,
                               STL_SIZEOF( smrLogPersData ),
                               &sWrittenBytes,
                               aEnv ) == STL_SUCCESS );
    sTotalWrittenBytes = sWrittenBytes;

    /**
     * Log Stream 관련 Persistent Information 기록
     */
    STL_TRY( smfWriteCtrlFile( aFile,
                               &sBuffer,
                               (stlChar *)&sWarmupEntry->mLogStream.mLogStreamPersData,
                               STL_SIZEOF( smrLogStreamPersData ),
                               &sWrittenBytes,
                               aEnv ) == STL_SUCCESS );
    sTotalWrittenBytes += sWrittenBytes;
    
    STL_TRY( smrFirstLogGroupPersData( &sWarmupEntry->mLogStream,
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
            
        STL_TRY( smrNextLogGroupPersData( &sWarmupEntry->mLogStream,
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


/** @} */
