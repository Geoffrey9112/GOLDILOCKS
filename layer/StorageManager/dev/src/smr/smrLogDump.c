/*******************************************************************************
 * smrLogDump.c
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
#include <smnDef.h>
#include <smrDef.h>
#include <smpDef.h>
#include <smsDef.h>
#include <smdDef.h>
#include <smtDef.h>
#include <smfDef.h>
#include <smxlDef.h>
#include <smqDef.h>
#include <smrLogDump.h>

extern smrRedoVector gSmfRedoVectors[];
extern smrRedoVector gSmpRedoVectors[];
extern smrRedoVector gSmtRedoVectors[];
extern smrRedoVector gSmrRedoVectors[];
extern smrRedoVector gSmsRedoVectors[];
extern smrRedoVector gSmxlRedoVectors[];
extern smrRedoVector gSmeRedoVectors[];
extern smrRedoVector gSmdRedoVectors[];
extern smrRedoVector gSmnRedoVectors[];
extern smrRedoVector gSmqRedoVectors[];
extern smrRedoVector gSmgRedoVectors[];

/**
 * @file smrLogDump.c
 * @brief Storage Manager Layer Log File Dump Internal Routines
 */

/**
 * @addtogroup smr
 * @{
 */

static stlChar * smrLogTypeString( smgComponentClass aClass, stlUInt16 aType )
{
    stlChar * sString = NULL;

    switch( aClass )
    {
        case SMG_COMPONENT_DATAFILE :
            sString = gSmfRedoVectors[aType].mLogName;
            break;
        case SMG_COMPONENT_PAGE_ACCESS :
            sString = gSmpRedoVectors[aType].mLogName;
            break;
        case SMG_COMPONENT_TABLESPACE :
            sString = gSmtRedoVectors[aType].mLogName;
            break;
        case SMG_COMPONENT_GENERAL :
            STL_ASSERT( 0 );
            break;
        case SMG_COMPONENT_RECOVERY :
            sString = gSmrRedoVectors[aType].mLogName;
            break;
        case SMG_COMPONENT_SEGMENT :
            sString = gSmsRedoVectors[aType].mLogName;
            break;
        case SMG_COMPONENT_TRANSACTION :
            sString = gSmxlRedoVectors[aType].mLogName;
            break;
        case SMG_COMPONENT_RELATION :
            sString = gSmeRedoVectors[aType].mLogName;
            break;
        case SMG_COMPONENT_TABLE :
            sString = gSmdRedoVectors[aType].mLogName;
            break;
        case SMG_COMPONENT_INDEX :
            sString = gSmnRedoVectors[aType].mLogName;
            break;
        case SMG_COMPONENT_SEQUENCE :
            sString = gSmqRedoVectors[aType].mLogName;
            break;
        case SMG_COMPONENT_EXTERNAL :
            sString = gSmgRedoVectors[aType].mLogName;
            break;
        case SMG_COMPONENT_TEST :
            STL_ASSERT( 0 );
            break;
        default :
            STL_ASSERT( 0 );
            break;
    }

    return sString;
}

static stlChar * smrGetComponentClassString( smgComponentClass aClass )
{
    stlChar * sString = NULL;

    switch( aClass )
    {
        case SMG_COMPONENT_DATAFILE :
            sString = "DATAFILE";
            break;
        case SMG_COMPONENT_PAGE_ACCESS :
            sString = "PAGE_ACCESS";
            break;
        case SMG_COMPONENT_TABLESPACE :
            sString = "TABLESPACE";
            break;
        case SMG_COMPONENT_GENERAL :
            sString = "GENERAL";
            break;
        case SMG_COMPONENT_RECOVERY :
            sString = "RECOVERY";
            break;
        case SMG_COMPONENT_SEGMENT :
            sString = "SEGMENT";
            break;
        case SMG_COMPONENT_TRANSACTION :
            sString = "TRANSACTION";
            break;
        case SMG_COMPONENT_RELATION :
            sString = "ACCESS";
            break;
        case SMG_COMPONENT_TABLE :
            sString = "TABLE";
            break;
        case SMG_COMPONENT_INDEX :
            sString = "INDEX";
            break;
        case SMG_COMPONENT_SEQUENCE :
            sString = "SEQUENCE";
            break;
        case SMG_COMPONENT_EXTERNAL :
            sString = "EXTERNAL";
            break;
        case SMG_COMPONENT_TEST :
            sString = "TEST";
            break;
        default :
            STL_ASSERT( 0 );
            break;
    }

    return sString;
}

static stlChar * smrGetRedoTypeString( stlChar aRedoType )
{
    stlChar * sString = NULL;

    switch( aRedoType )
    {
        case SMR_REDO_TARGET_PAGE :
            sString = "PAGE";
            break;
        case SMR_REDO_TARGET_UNDO_SEGMENT :
            sString = "UNDO";
            break;
        case SMR_REDO_TARGET_TRANSACTION :
            sString = "TRANSACTION";
            break;
        case SMR_REDO_TARGET_CTRL :
            sString = "CONTROL_FILE";
            break;
        case SMR_REDO_TARGET_NONE :
            sString = "NONE";
            break;
        case SMR_REDO_TARGET_TEST :
            sString = "TEST";
            break;
        default :
            STL_ASSERT( 0 );
            break;
    }

    return sString;
}

/**
 * @brief Log File을 Dump한다.
 * @param[in] aFilePath Log File의 전체 경로
 * @param[in] aTargetLsn     Target Lsn
 * @param[in] aLsnOffset     Offset of Target Lsn
 * @param[in] aFetchTotalCnt Fetch Count of Log
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smrDumpLogFile( stlChar * aFilePath,
                          stlInt64  aTargetLsn,
                          stlInt64  aLsnOffset,
                          stlInt64  aFetchTotalCnt,
                          smlEnv  * aEnv )
{
    stlUInt32             sState = 0;
    smrLogHdr             sLogHdr;
    smrLogPieceHdr        sLogPieceHdr;
    stlChar             * sLogBlock;
    stlChar             * sLogBody;
    stlChar             * sLogPieceHexString;
    stlChar             * sLogPieceCharString;
    stlUInt32             sLogBodyOffset;
    stlChar             * sLogPieceBody;
    stlInt32              sIdx;
    stlInt32              sBlockOffset;
    smrLogFileHdr         sLogFileHdr;
    smrLogBlockHdr        sLogBlockHdr;
    stlInt32              sLeftLogCount;
    stlInt32              sTotalReadBytes;
    stlInt32              sReadBytes;
    stlInt32              sLogIdx = 0;
    smrLogCursor4Disk     sLogCursor;
    stlBool               sEndOfFile = STL_FALSE;
    stlChar               sColumn1[17];
    stlChar               sColumn2[17];
    stlChar               sColumn3[17];
    stlChar               sColumn4[17];
    stlChar               sChar1[9];
    stlChar               sChar2[9];
    stlChar               sChar3[9];
    stlChar               sChar4[9];
    stlInt32              sLogPieceHexOffset;
    stlInt32              sLogPieceCharOffset;
    stlInt32              sBlockSeq = 0;
    stlInt32              sLogHdrBlockSeq = 0;
    smlRid                sSegRid;
    smlRid                sDataRid;
    stlExpTime            sExpTime;
    stlInt64              sFetchCount = 0;
    knlRegionMark         sMemMark;
    
    /**
     * Allocate log buffer
     */
    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               KNL_ENV(aEnv) ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                SMR_MAX_LOGBODY_SIZE,
                                (void**)&sLogBody,
                                KNL_ENV(aEnv) ) == STL_SUCCESS );

    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                SMR_MAX_LOGPIECE_SIZE * 2,
                                (void**)&sLogPieceHexString,
                                KNL_ENV(aEnv) ) == STL_SUCCESS );

    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                SMR_MAX_LOGPIECE_SIZE,
                                (void**)&sLogPieceCharString,
                                KNL_ENV(aEnv) ) == STL_SUCCESS );

    /**
     * Iterating logs
     */
    
    STL_TRY( smrOpenLogCursor4Disk( &sLogCursor,
                                    aFilePath,
                                    0, /* aHintBlockSeq */
                                    SMR_INVALID_LSN, /* aHintLsn */
                                    &sLogFileHdr,
                                    aEnv ) == STL_SUCCESS );
    sState = 2;

    stlMakeExpTimeByLocalTz( &sExpTime, sLogFileHdr.mCreationTime );

    stlPrintf( "\n===========================================================" );
    stlPrintf( "\n [LOG FILE HEADER]"
               "\n-----------------------------------------------------------"
               "\n LOG_GROUP_ID    : %d"
               "\n BLOCK_SIZE      : %d"
               "\n FILE_SIZE       : %ld"
               "\n FILE_SEQUENCE   : %ld"
               "\n PREV_LAST_LSN   : %ld"
               "\n CREATION TIME   : %04d-%02d-%02d %02d:%02d:%02d.%06d",
               sLogFileHdr.mLogGroupId,
               sLogFileHdr.mBlockSize,
               sLogFileHdr.mFileSize,
               sLogFileHdr.mLogFileSeqNo,
               sLogFileHdr.mPrevLastLsn,
               sExpTime.mYear + 1900,
               sExpTime.mMonth + 1,
               sExpTime.mDay,
               sExpTime.mHour,
               sExpTime.mMinute,
               sExpTime.mSecond,
               sExpTime.mMicroSecond );
    stlPrintf( "\n===========================================================\n" );

    while( 1 )
    {
        sBlockOffset = 0;
        STL_TRY( smrReadBlockCursor4Disk( &sLogCursor,
                                          &sLogBlock,
                                          &sEndOfFile,
                                          aEnv ) == STL_SUCCESS );

        if( sEndOfFile == STL_TRUE )
        {
            break;
        }

        sBlockSeq++;
        SMR_READ_MOVE_LOG_BLOCK_HDR( &sLogBlockHdr, sLogBlock, sBlockOffset );

        if( SMR_IS_DUMMY_FILE_END_BLOCK( &sLogBlockHdr ) )
        {
            stlPrintf( "\n[FILE_END] : BLOCK(%d), LSN(%ld)\n", sBlockSeq - 1, sLogBlockHdr.mLsn );
            STL_THROW( RAMP_SUCCESS );
        }

        STL_TRY_THROW( smrIsValidLog( &sLogCursor,
                                      &sLogBlockHdr ) == STL_TRUE ,
                       RAMP_SUCCESS );
        
        sBlockOffset = sLogBlockHdr.mFirstLogOffset;
        sLeftLogCount = sLogBlockHdr.mLogCount;
        
        while( sLeftLogCount > 0 )
        {
            sLeftLogCount--;
            
            SMR_READ_LOG_HDR( &sLogHdr, sLogBlock + sBlockOffset );
            sBlockOffset += SMR_LOG_HDR_SIZE;
            sLogHdrBlockSeq = sBlockSeq;

            if( sLogHdr.mSize > (sLogCursor.mBlockSize - sBlockOffset) )
            {
                sReadBytes = sLogCursor.mBlockSize - sBlockOffset;
            }
            else
            {
                sReadBytes = sLogHdr.mSize;
            }
        
            stlMemcpy( sLogBody, sLogBlock + sBlockOffset, sReadBytes );
            sTotalReadBytes = sReadBytes;
            sBlockOffset += sReadBytes;
        
            while( sTotalReadBytes < sLogHdr.mSize )
            {
                sBlockOffset = 0;
                STL_TRY( smrReadBlockCursor4Disk( &sLogCursor,
                                                  &sLogBlock,
                                                  &sEndOfFile,
                                                  aEnv ) == STL_SUCCESS );

                if( sEndOfFile == STL_TRUE )
                {
                    stlPrintf( "Partial Log detected\n" );
                    STL_THROW( RAMP_SUCCESS );
                }

                sBlockSeq++;
                SMR_READ_MOVE_LOG_BLOCK_HDR( &sLogBlockHdr, sLogBlock, sBlockOffset );

                if( smrIsValidLog( &sLogCursor, &sLogBlockHdr ) == STL_FALSE )
                {
                    stlPrintf( "Partial Log detected\n" );
                    STL_THROW( RAMP_SUCCESS );
                }
                
                if( (sLogHdr.mSize - sTotalReadBytes) > (sLogCursor.mBlockSize - sBlockOffset) )
                {
                    sReadBytes = sLogCursor.mBlockSize - sBlockOffset;
                }
                else
                {
                    sReadBytes = (sLogHdr.mSize - sTotalReadBytes);
                }
        
                stlMemcpy( sLogBody + sTotalReadBytes, sLogBlock + sBlockOffset, sReadBytes );
            
                sTotalReadBytes += sReadBytes;
                sBlockOffset += sReadBytes;
                sLeftLogCount = sLogBlockHdr.mLogCount;
            }

            if( aTargetLsn != SMR_INVALID_LSN )
            {
                if( sLogHdr.mLsn < ( aTargetLsn + aLsnOffset ) )
                {
                    continue;
                }
                else
                {
                    if( aFetchTotalCnt != SML_DUMP_INVALID_VALUE )
                    {
                        if( sLogHdr.mLsn > (aTargetLsn + aFetchTotalCnt) )
                        {
                            STL_THROW( RAMP_SUCCESS );
                        }
                    }
                }
            }

            if( aFetchTotalCnt != SML_DUMP_INVALID_VALUE )
            {
                STL_TRY_THROW( sFetchCount != aFetchTotalCnt, RAMP_SUCCESS );
            }
            
            sFetchCount++;
            
            SMG_WRITE_RID( &sSegRid, &sLogHdr.mSegRid );
        
            stlPrintf( "\n[LOG #%d] : ", sLogIdx++ );
            stlPrintf( "BLOCK(%d), ", sLogHdrBlockSeq - 1 );
            stlPrintf( "LSN(%ld), ", sLogHdr.mLsn );
            stlPrintf( "SIZE(%d), ", sLogHdr.mSize );
            stlPrintf( "PIECE_COUNT(%d), ", sLogHdr.mLogPieceCount );
            stlPrintf( "TRANS_ID(%lX), ", sLogHdr.mTransId );
            stlPrintf( "RID(%d,%d,%d)\n", sSegRid.mTbsId,
                                          sSegRid.mOffset,
                                          sSegRid.mPageId );

            sLogBodyOffset = 0;
            
            for( sIdx = 0; sIdx < sLogHdr.mLogPieceCount; sIdx++ )
            {
                SMR_READ_MOVE_LOGPIECE_HDR( &sLogPieceHdr,
                                            sLogBody,
                                            sLogBodyOffset );
                sLogPieceBody = sLogBody + sLogBodyOffset;

                SMG_WRITE_RID( &sDataRid, &sLogPieceHdr.mDataRid );
        
                stlPrintf( "[PIECE #%d] : ", sIdx );
                stlPrintf( "TYPE(%s), ", smrLogTypeString( sLogPieceHdr.mComponentClass,
                                                           sLogPieceHdr.mType ) );
                stlPrintf( "SIZE(%d), ", sLogPieceHdr.mSize );
                stlPrintf( "CLASS(%s), ", smrGetComponentClassString( sLogPieceHdr.mComponentClass ) );
                stlPrintf( "REDO_TYPE(%s), ", smrGetRedoTypeString( sLogPieceHdr.mRedoTargetType ) );
                stlPrintf( "PROPAGATE_LOG(%s), ", (sLogPieceHdr.mPropagateLog == STL_TRUE)? "YES":"NO" );
                stlPrintf( "RID(%d,%d,%d)\n", sDataRid.mTbsId,
                                              sDataRid.mOffset,
                                              sDataRid.mPageId );

                STL_TRY( stlBinary2HexString( (stlUInt8*)sLogPieceBody,
                                              sLogPieceHdr.mSize,
                                              sLogPieceHexString,
                                              SMR_MAX_LOGPIECE_SIZE * 2,
                                              KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
                STL_TRY( stlBinary2CharString( (stlUInt8*)sLogPieceBody,
                                               sLogPieceHdr.mSize,
                                               sLogPieceCharString,
                                               SMR_MAX_LOGPIECE_SIZE,
                                               KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

                sLogPieceHexOffset = 0;
                sLogPieceCharOffset = 0;

                while( sLogPieceCharOffset < sLogPieceHdr.mSize )
                {
                    stlStrcpy( sColumn1, "                " );
                    stlStrcpy( sColumn2, "                " );
                    stlStrcpy( sColumn3, "                " );
                    stlStrcpy( sColumn4, "                " );
                    stlStrcpy( sChar1,   "        " );
                    stlStrcpy( sChar2,   "        " );
                    stlStrcpy( sChar3,   "        " );
                    stlStrcpy( sChar4,   "        " );

                    SMR_WRITE_HEX_STRING_TO_COLUMN( sColumn1,
                                                    sLogPieceHexString,
                                                    sLogPieceHexOffset,
                                                    sLogPieceHdr.mSize * 2 );
                    SMR_WRITE_HEX_STRING_TO_COLUMN( sColumn2,
                                                    sLogPieceHexString,
                                                    sLogPieceHexOffset,
                                                    sLogPieceHdr.mSize * 2 );
                    SMR_WRITE_HEX_STRING_TO_COLUMN( sColumn3,
                                                    sLogPieceHexString,
                                                    sLogPieceHexOffset,
                                                    sLogPieceHdr.mSize * 2 );
                    SMR_WRITE_HEX_STRING_TO_COLUMN( sColumn4,
                                                    sLogPieceHexString,
                                                    sLogPieceHexOffset,
                                                    sLogPieceHdr.mSize * 2 );

                    SMR_WRITE_CHAR_STRING_TO_COLUMN( sChar1,
                                                     sLogPieceCharString,
                                                     sLogPieceCharOffset,
                                                     sLogPieceHdr.mSize );
                    SMR_WRITE_CHAR_STRING_TO_COLUMN( sChar2,
                                                     sLogPieceCharString,
                                                     sLogPieceCharOffset,
                                                     sLogPieceHdr.mSize );
                    SMR_WRITE_CHAR_STRING_TO_COLUMN( sChar3,
                                                     sLogPieceCharString,
                                                     sLogPieceCharOffset,
                                                     sLogPieceHdr.mSize );
                    SMR_WRITE_CHAR_STRING_TO_COLUMN( sChar4,
                                                     sLogPieceCharString,
                                                     sLogPieceCharOffset,
                                                     sLogPieceHdr.mSize );

                    stlPrintf( " %s %s %s %s    %s %s %s %s\n",
                               sColumn1, sColumn2, sColumn3, sColumn4,
                               sChar1, sChar2, sChar3, sChar4 );
                }
                
                sLogBodyOffset += sLogPieceHdr.mSize;
            }
        }

        if( SMR_IS_LAST_FILE_BLOCK( &sLogBlockHdr ) )
        {
            stlPrintf( "\n[FILE_END] : BLOCK(%d), LSN(%ld)\n", sBlockSeq - 1, sLogBlockHdr.mLsn );
            STL_THROW( RAMP_SUCCESS );
        }
    }

    STL_RAMP( RAMP_SUCCESS );

    sState = 1;
    STL_TRY( smrCloseLogCursor4Disk( &sLogCursor,
                                     aEnv ) == STL_SUCCESS );
        
    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_TRUE, /* aFreeChunk */
                                       KNL_ENV(aEnv) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)smrCloseLogCursor4Disk( &sLogCursor, aEnv );
        case 1:
            (void) knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                             &sMemMark,
                                             STL_TRUE, /* aFreeChunk */
                                             KNL_ENV(aEnv) );
        default:
            break;
    }

    return STL_FAILURE;
}

/** @} */

/**
 * @addtogroup smrLogDump
 * @{
 */

stlStatus smrOpenLogCursor4Disk( smrLogCursor4Disk * aLogCursor,
                                 stlChar           * aFilePath,
                                 stlInt32            aHintBlockSeq,
                                 smrLsn              aHintLsn,
                                 smrLogFileHdr     * aLogFileHdr,
                                 smlEnv            * aEnv )
{
    stlUInt32       sState = 0;
    stlOffset       sOffset;
    stlInt32        sBlockCount = 0;
    smrLogBlockHdr  sLogBlockHdr;
    stlInt32        sFlag;
    stlInt64        sLogIo;
    stlInt32        sMaxBlockSize = SMR_MAX_LOG_BLOCK_SIZE;
    stlChar         sBuffer[SMR_MAX_LOG_BLOCK_SIZE * 2];
    smrLogFileHdr * sLogFileHdr;

    sLogFileHdr = (smrLogFileHdr*) STL_ALIGN( (stlInt64) sBuffer, sMaxBlockSize );
    
    aLogCursor->mBuffer = NULL;
    aLogCursor->mFileBlockSeq = 0;
    aLogCursor->mBufferBlockSeq = -1;

    sFlag = STL_FOPEN_READ | STL_FOPEN_LARGEFILE;

    sLogIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_LOG_FILE_IO,
                                            KNL_ENV(aEnv) );

    if( sLogIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

    STL_TRY( smgOpenFile( &aLogCursor->mFile,
                          aFilePath,
                          sFlag,
                          STL_FPERM_OS_DEFAULT,
                          aEnv )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( stlReadFile( &aLogCursor->mFile,
                          sLogFileHdr,
                          STL_ALIGN( STL_SIZEOF( smrLogFileHdr ), sMaxBlockSize ),
                          NULL,
                          KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    aLogCursor->mBlockSize = sLogFileHdr->mBlockSize;

    /**
     * @todo size를 static하게 정해져 있는데,
     * log block size와 log buffer block count가
     * 변경 가능한 스펙이 추가된다면 수정되어야 한다.
     */
    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                aLogCursor->mBlockSize * (SMR_DUMP_LOG_BUFFER_BLOCK_COUNT + 1),
                                (void**)&aLogCursor->mMisAlignedBuffer,
                                KNL_ENV( aEnv ) ) == STL_SUCCESS );

    /**
     * Mount Phase에서 Log Switch하고 Open할 경우( Hint Block Sequnce는 유효하지 않다. )
     */
    if( aHintLsn == SMR_INVALID_LSN )
    {
        sOffset = SMR_BLOCK_SEQ_FILE_OFFSET( 0, sLogFileHdr->mBlockSize );
    }
    else
    {
        STL_DASSERT( aHintLsn >= sLogFileHdr->mPrevLastLsn );
        sOffset = SMR_BLOCK_SEQ_FILE_OFFSET( aHintBlockSeq, sLogFileHdr->mBlockSize );
    }

    aLogCursor->mBuffer = (stlChar*) STL_ALIGN( (stlInt64)aLogCursor->mMisAlignedBuffer,
                                                SMF_DIRECT_IO_DEFAULT_BLOCKSIZE );

    aLogCursor->mMaxBlockCount = sLogFileHdr->mMaxBlockCount - 1;

    sBlockCount = STL_MIN( aLogCursor->mMaxBlockCount,
                           SMR_DUMP_LOG_BUFFER_BLOCK_COUNT );

    stlMemset( aLogCursor->mBuffer, 0x00, aLogCursor->mBlockSize );
    
    STL_TRY( stlSeekFile( &aLogCursor->mFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    STL_TRY( stlReadFile( &aLogCursor->mFile,
                          (void*)aLogCursor->mBuffer,
                          aLogCursor->mBlockSize * sBlockCount,
                          NULL,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    SMR_READ_LOG_BLOCK_HDR( &sLogBlockHdr, aLogCursor->mBuffer );
    
    aLogCursor->mFileBlockSeq = sBlockCount;
    aLogCursor->mBufferBlockCount = sBlockCount;

    /**
     * HintLsn이 Invalid가 아닌데 LogBlockHdr.mLsn과 다르면 다시 Logfile을 처음부터 읽는다.
     * (group은 같지만 logfile이 최신이 아니라 해당 log가 없는 경우에 해당.)
     */ 
    if( (aHintLsn != SMR_INVALID_LSN) &&
        (aHintLsn != sLogBlockHdr.mLsn ) )
    {
        sOffset = SMR_BLOCK_SIZE;
    }
    else
    {
        STL_THROW( RAMP_SUCCESS );
    }

    stlMemset( aLogCursor->mBuffer, 0x00, aLogCursor->mBlockSize );

    STL_TRY( stlSeekFile( &aLogCursor->mFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    STL_TRY( stlReadFile( &aLogCursor->mFile,
                          (void*)aLogCursor->mBuffer,
                          aLogCursor->mBlockSize * sBlockCount,
                          NULL,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    SMR_READ_LOG_BLOCK_HDR( &sLogBlockHdr, aLogCursor->mBuffer );
    
    aLogCursor->mFileBlockSeq = sBlockCount;
    aLogCursor->mBufferBlockCount = sBlockCount;

    STL_RAMP( RAMP_SUCCESS );
    
    /**
     * Log switching 직후라면 Log File Hdr의 File Sequence랑 첫번째 Log Block의
     * File Wrap Number가 다르다.
     */ 
    aLogCursor->mWrapNo = SMR_BLOCK_WRAP_NO( &sLogBlockHdr );

    stlMemcpy( aLogFileHdr, sLogFileHdr, STL_SIZEOF( smrLogFileHdr ) );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)smgCloseFile( &aLogCursor->mFile, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus smrReadBlockCursor4Disk( smrLogCursor4Disk  * aLogCursor,
                                   stlChar           ** aLogBlock,
                                   stlBool            * aEndOfFile,
                                   smlEnv             * aEnv )
{
    stlOffset sOffset;
    stlInt32  sBlockCount = 0;
    
    *aEndOfFile = STL_FALSE;
    
    aLogCursor->mBufferBlockSeq++;

    if( aLogCursor->mBufferBlockSeq >= aLogCursor->mBufferBlockCount )
    {
        if( aLogCursor->mFileBlockSeq >= aLogCursor->mMaxBlockCount )
        {
            *aEndOfFile = STL_TRUE;
            STL_THROW( RAMP_SUCCESS );
        }
        
        sOffset = SMR_BLOCK_SEQ_FILE_OFFSET( aLogCursor->mFileBlockSeq,
                                             aLogCursor->mBlockSize );
        STL_TRY( stlSeekFile( &aLogCursor->mFile,
                              STL_FSEEK_SET,
                              &sOffset,
                              KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

        sBlockCount = STL_MIN( aLogCursor->mMaxBlockCount - aLogCursor->mFileBlockSeq,
                               SMR_DUMP_LOG_BUFFER_BLOCK_COUNT );

        STL_ASSERT( sBlockCount > 0 );
        
        STL_TRY( stlReadFile( &aLogCursor->mFile,
                              (void*)aLogCursor->mBuffer,
                              aLogCursor->mBlockSize * sBlockCount,
                              NULL,
                              KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

        aLogCursor->mFileBlockSeq += sBlockCount;
        aLogCursor->mBufferBlockCount = sBlockCount;
        aLogCursor->mBufferBlockSeq = 0;
    }
    
    *aLogBlock = (aLogCursor->mBuffer +
                  (aLogCursor->mBufferBlockSeq * aLogCursor->mBlockSize));

    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrCloseLogCursor4Disk( smrLogCursor4Disk * aLogCursor,
                                  smlEnv            * aEnv )
{
    STL_TRY( smgCloseFile( &aLogCursor->mFile,
                           aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlBool smrIsValidLog( smrLogCursor4Disk * aLogCursor,
                       smrLogBlockHdr    * aLogBlockHdr )
{
    stlBool sValid = STL_TRUE;
    
    if( aLogBlockHdr->mLsn < 0 )
    {
        sValid = STL_FALSE;
        STL_THROW( RAMP_RETURN );
    }

    if( SMR_BLOCK_WRAP_NO( aLogBlockHdr ) != (aLogCursor->mWrapNo) )
    {
        sValid = STL_FALSE;
        STL_THROW( RAMP_RETURN );
    }

    STL_RAMP( RAMP_RETURN );
    
    return sValid;
}

/** @} */
