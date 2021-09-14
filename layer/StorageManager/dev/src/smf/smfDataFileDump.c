/*******************************************************************************
 * smfDataFileDump.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smfDataFileDump.c $
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
#include <smf.h>
#include <smfDef.h>
#include <smfDataFileDump.h>
#include <smfTbsMgr.h>

/**
 * @file smfDataFileDump.c
 * @brief Storage Manager Layer Datafile Dump Internal Routines
 */

/**
 * @addtogroup smf
 * @{
 */

/**
 * @brief Datafile Header를 Dump한다.
 * @param[in] aFilePath Datafile Path
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfDumpDatafileHeader( stlChar * aFilePath,
                                 smlEnv  * aEnv )
{
    stlInt32           sFileState = 0;
    smfDatafileHeader  sDatafileHeader;
    stlFile            sFile;
    stlExpTime         sExpTime;

    STL_TRY( smgOpenFile( &sFile,
                          aFilePath,
                          STL_FOPEN_READ |
                          STL_FOPEN_LARGEFILE,
                          STL_FPERM_OS_DEFAULT,
                          aEnv )
             == STL_SUCCESS );
    sFileState = 1;

    STL_TRY( smfReadDatafileHeader( &sFile,
                                    aFilePath,
                                    &sDatafileHeader,
                                    SML_ENV(aEnv) )
             == STL_SUCCESS );

    stlMakeExpTimeByLocalTz( &sExpTime, sDatafileHeader.mCreationTime );

    stlPrintf( "  FILE                   : %s\n",  aFilePath );
    stlPrintf( "  Tablespace Physical Id : %d\n",  sDatafileHeader.mTbsPhysicalId );
    stlPrintf( "  Datafile Id            : %d\n",  sDatafileHeader.mDatafileId );
    stlPrintf( "  Last Checkpoint Lsn    : %ld\n", sDatafileHeader.mCheckpointLsn );
    stlPrintf( "  Last Checkpoint Lid    : (%ld, %d, %d)\n",
               SMR_FILE_SEQ_NO( &sDatafileHeader.mCheckpointLid ),
               SMR_BLOCK_SEQ_NO( &sDatafileHeader.mCheckpointLid ),
               SMR_BLOCK_OFFSET( &sDatafileHeader.mCheckpointLid ) );
    stlPrintf( "  Creation TIME          : %04d-%02d-%02d %02d:%02d:%02d.%06d\n",
               sExpTime.mYear + 1900,
               sExpTime.mMonth + 1,
               sExpTime.mDay,
               sExpTime.mHour,
               sExpTime.mMinute,
               sExpTime.mSecond,
               sExpTime.mMicroSecond );

    sFileState = 0;
    STL_TRY( smgCloseFile( &sFile,
                           aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sFileState == 1 )
    {
        (void)smgCloseFile( &sFile, aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief Datafile의 특정 Page를 Dump.
 * @param[in]     aFilePath   Datafile Path
 * @param[in]     aPageSeqId  Page Seqeunce Id
 * @param[in]     aFetchCnt   Page Fetch Count
 * @param[in,out] aEnv        Environment 정보
 */ 
stlStatus smfDumpDatafilePage( stlChar  * aFilePath,
                               stlInt64   aPageSeqId,
                               stlInt64   aFetchCnt,
                               smlEnv   * aEnv )
{
    stlInt32    sFileState = 0;
    stlFile     sFile;
    stlOffset   sFileOffset;
    stlInt32    sFlag;
    smlPid      sPid;
    stlChar     sBuffer[SMP_PAGE_SIZE * 2];
    stlChar   * sPage;
    stlInt64    sFetchCnt;
    stlInt32    sIdx;
    
    sPage = (stlChar*)( STL_ALIGN( (stlInt64)sBuffer, SMP_PAGE_SIZE ) );
    
    sFlag = STL_FOPEN_READ | STL_FOPEN_LARGEFILE;
    
    STL_TRY( smgOpenFile( &sFile,
                          aFilePath,
                          sFlag,
                          STL_FPERM_OS_DEFAULT,
                          aEnv ) == STL_SUCCESS );
    sFileState = 1;

    sPid = (smlPid) aPageSeqId;

    /**
     * Datafile Header Size 포함하여 Offset을 계산한다.
     */ 
    sFileOffset = SMP_PAGE_SIZE * ((stlInt64)sPid + 1);

    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_SET,
                          &sFileOffset,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    sFetchCnt = (aFetchCnt == -1) ? 1 : aFetchCnt;
    
    for( sIdx = 0; sIdx < sFetchCnt; sIdx++ )
    {
        STL_TRY( stlReadFile( &sFile,
                              sPage,
                              SMP_PAGE_SIZE,
                              NULL,
                              KNL_ERROR_STACK(aEnv) ) == STL_SUCCESS );
    
        STL_TRY( smfDumpPage( sPage,
                              SML_DUMP_PAGE_ALL,
                              aEnv ) == STL_SUCCESS );
    }

    sFileState = 0;
    STL_TRY( smgCloseFile( &sFile,
                           aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sFileState )
    {
        case 1:
            (void) smgCloseFile( &sFile, aEnv );
            break;
        default:
            break;
    }
    
    return STL_FAILURE;
}

/**
 * @brief Incremental Backup의 Header와 Tail을 Dump한다.
 * @param[in]     aFilePath  Backup File 경로
 * @param[in]     aDumpBody  Dump Body flag
 * @param[in]     aTbsId     tablespace Id
 * @param[in]     aInPageSeqId Page Sequence Id
 * @param[in]     aFetchCnt  Dump Body Fetch Count
 * @param[in,out] aEnv       Envrionment 정보
 */ 
stlStatus smfDumpIncBackupFile( stlChar * aFilePath,
                                stlInt8   aDumpBody,
                                stlUInt16 aTbsId,
                                stlInt64  aInPageSeqId,
                                stlInt64  aFetchCnt,
                                smlEnv  * aEnv )
{
    stlInt32                sState = 0;
    stlFile                 sFile;
    stlOffset               sFileOffset;
    stlChar               * sTailBuffer;
    stlInt32                sFlag;
    stlSize                 sTailSize;
    stlBool                 sFileExist = STL_FALSE;
    smfIncBackupFileHeader  sFileHeader;
    smfIncBackupFileTail  * sFileTail;
    smpPhyHdr             * sPhyHdr;
    stlInt32                i;
    stlChar                 sObject[SMF_DUMP_INC_BACKUP_OBJECT_TYPE_LEN] = {0};
    stlInt64                sPageCount;
    stlInt64                sBufferSize;
    stlChar               * sBuffer;
    stlChar               * sPages;
    stlInt64                sTbsBackupSize;
    stlInt64                sDoneSize;
    stlInt64                sReadSize;
    stlSize                 sCheckSize;
    stlInt64                sIdxPage;
    stlDynamicAllocator     sAllocator;
    stlInt64                sFetchCnt = 0;
    stlInt64                sInFetchCnt;
    stlInt64                sPageSeqId;
    smlTbsId                sPrevTbsId = SML_INVALID_TBS_ID;

    sFlag = STL_FOPEN_LARGEFILE | STL_FOPEN_READ;

    STL_TRY( stlCreateDynamicAllocator( &sAllocator,
                                        SML_DUMP_MAX_BUFFER_SIZE + SMP_PAGE_SIZE,
                                        SML_DUMP_MAX_BUFFER_SIZE,
                                        KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );
    sState = 1;

    sBufferSize = SML_DUMP_MAX_BUFFER_SIZE + SMP_PAGE_SIZE;
    
    STL_TRY( stlAllocDynamicMem( &sAllocator,
                                 sBufferSize,
                                 (void**)&sBuffer,
                                 KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    sPages = (stlChar*)STL_ALIGN( (stlInt64)sBuffer, SMP_PAGE_SIZE );
    sBufferSize = SML_DUMP_MAX_BUFFER_SIZE;
    
    STL_TRY( stlExistFile( aFilePath,
                           &sFileExist,
                           KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sFileExist == STL_TRUE, RAMP_ERR_FILE_NOT_EXIST );

    STL_TRY( smgOpenFile( &sFile,
                          aFilePath,
                          sFlag,
                          STL_FPERM_OS_DEFAULT,
                          aEnv ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( smfReadBackupFileHeader( &sFile,
                                      &sFileHeader,
                                      aEnv ) == STL_SUCCESS );

    sTailSize = STL_ALIGN( sFileHeader.mTbsCount * STL_SIZEOF( smfIncBackupFileTail ),
                           SMP_PAGE_SIZE );

    STL_TRY( stlAllocDynamicMem( &sAllocator,
                                 sTailSize + SMP_PAGE_SIZE,
                                 (void**)&sTailBuffer,
                                 KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    stlMemset( sTailBuffer, 0x00, sTailSize + SMP_PAGE_SIZE );

    sFileTail = (smfIncBackupFileTail *)STL_ALIGN( (stlInt64)sTailBuffer, SMP_PAGE_SIZE );

    sFileOffset = SMP_PAGE_SIZE + sFileHeader.mBodySize;

    smfGetIncBackupObjectType( sFileHeader.mTbsId, sObject );
    
    STL_TRY( smfReadBackupFileTail( &sFile,
                                    sFileOffset,
                                    sTailSize,
                                    sFileTail,
                                    aEnv ) == STL_SUCCESS );

    stlPrintf( "\n INCREMENTAL FILE HEADER \n"
               "----------------------------------------------------------------------------\n"
               "  OBJECT TYPE(%s), TBS COUNT(%d), BODY SIZE(%ld)\n"
               "  LSN: PREV(%ld), MAX (%ld), CHKPT(%ld)\n"
               "  CHKPT LID: File Seq No(%ld), Block Info1(%d), Block Info2(%d)\n"
               "----------------------------------------------------------------------------\n",
               sObject,
               sFileHeader.mTbsCount,
               sFileHeader.mBodySize,
               sFileHeader.mPrevBackupLsn,
               sFileHeader.mMaxBackupLsn,
               sFileHeader.mBackupChkptLsn,
               sFileHeader.mBackupChkptLid.mFileSeqNo,
               sFileHeader.mBackupChkptLid.mBlockInfo1,
               sFileHeader.mBackupChkptLid.mBlockInfo2 );

    stlPrintf( "\n INCREMENTAL FILE TAIL\n"
               "----------------------------------------------------------------------------\n" );
               
    for( i = 0; i < sFileHeader.mTbsCount; i++ )
    {
        stlPrintf( "  TABLESPACE ID(%03d), BACKUP PAGE COUNT(%05lu), TABLESPACE OFFSET(%ld)\n",
                   sFileTail[i].mTbsId,
                   sFileTail[i].mBackupPageCount,
                   sFileTail[i].mTbsOffset );
    }

    if( aDumpBody != SML_DUMP_INIT_VALUE )
    {
        for( i = 0; i < sFileHeader.mTbsCount; i++ )
        {
            if( (aTbsId == SML_INVALID_TBS_ID) ||
                (aTbsId == sFileTail[i].mTbsId) )
            {
                sFileOffset = sFileTail[i].mTbsOffset;

                STL_TRY( stlSeekFile( &sFile,
                                      STL_FSEEK_SET,
                                      &sFileOffset,
                                      KNL_ERROR_STACK(aEnv) ) == STL_SUCCESS );

                sDoneSize = 0;
                sTbsBackupSize = sFileTail[i].mBackupPageCount * SMP_PAGE_SIZE;
                while( STL_TRUE )
                {
                    if( sDoneSize >= sTbsBackupSize )
                    {
                        break;
                    }

                    sReadSize = STL_MIN( sBufferSize, (sTbsBackupSize - sDoneSize) );

                    STL_TRY( stlReadFile( &sFile,
                                          sPages,
                                          sReadSize,
                                          &sCheckSize,
                                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

                    STL_ASSERT( sReadSize == sCheckSize );

                    sPageCount = sReadSize / SMP_PAGE_SIZE;

                    sInFetchCnt = (aFetchCnt == -1) ? sPageCount : aFetchCnt;

                    if( sPrevTbsId != sFileTail[i].mTbsId )
                    {
                        sFetchCnt = 0;
                        sPrevTbsId = sFileTail[i].mTbsId;
                    }

                    for( sIdxPage = 0; sIdxPage < sPageCount; sIdxPage++ )
                    {
                        sPhyHdr = (smpPhyHdr*)(sPages + (sIdxPage * SMP_PAGE_SIZE));
                        
                        sPageSeqId = (stlInt64)SMP_PAGE_SEQ_ID( sPhyHdr->mSelfId.mPageId );

                        if( sFetchCnt >= sInFetchCnt )
                        {
                            break;
                        }
                        
                        if( aInPageSeqId != SML_DUMP_INVALID_VALUE )
                        {
                            if( sPageSeqId < aInPageSeqId )
                            {
                                continue;
                            }
                        }
                        
                        STL_TRY( smfDumpPage( sPages + (sIdxPage * SMP_PAGE_SIZE),
                                              aDumpBody,
                                              aEnv ) == STL_SUCCESS );

                        sFetchCnt++;
                    }

                    sDoneSize += sReadSize;
                }
            }
        }
    }
    
    sState = 1;
    STL_TRY( smgCloseFile( &sFile, aEnv ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( stlDestroyDynamicAllocator( &sAllocator,
                                         KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_FILE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_FILE_NOT_EXIST,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      aFilePath );
    }
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) smgCloseFile( &sFile, aEnv );
        case 1:
            (void) stlDestroyDynamicAllocator( &sAllocator,
                                               KNL_ERROR_STACK(aEnv) );
        default:
            break;
    }
    
    return STL_FAILURE;
}
/** @} */

/**
 * @addtogroup smfDataFileDump
 * @{
 */

/**
 * @brief Hex String을 길이 16의 Column에 저장한다.
 * @param [out] aColumn     Column
 * @param [in]  aHexString  Hex String
 * @param [out] aHexOffset  Hex Offset
 * @param [in]  aMaxSize    Hex String의 최대 길이.
 */ 
void smfWriteHexStringToColumn( stlChar  * aColumn,
                                stlChar  * aHexString,
                                stlInt32 * aHexOffset,
                                stlInt32   aMaxSize )
{
    if( (*aHexOffset + 16) >= aMaxSize )
    {
        STL_READ_MOVE_BYTES( aColumn, aHexString, aMaxSize - *aHexOffset, *aHexOffset );
    }
    else
    {
        STL_READ_MOVE_BYTES( aColumn, aHexString, 16, *aHexOffset );
    }
}

void smfGetDumpPageType( smpPageType   aPageType,
                         stlChar     * aTypeStr )
{
    switch( aPageType )
    {
        case SMP_PAGE_TYPE_INIT :
            stlStrncpy( aTypeStr, "INIT", SMF_DUMP_PAGE_TYPE_LEN + 1 );
            break;
        case SMP_PAGE_TYPE_UNFORMAT :
            stlStrncpy( aTypeStr, "UNFORMAT", SMF_DUMP_PAGE_TYPE_LEN + 1 );
            break;
        case SMP_PAGE_TYPE_EXT_BLOCK_MAP :
            stlStrncpy( aTypeStr, "EXT_BLOCK_MAP", SMF_DUMP_PAGE_TYPE_LEN + 1 );
            break;
        case SMP_PAGE_TYPE_EXT_MAP :
            stlStrncpy( aTypeStr, "EXT_MAP", SMF_DUMP_PAGE_TYPE_LEN + 1 );
            break;
        case SMP_PAGE_TYPE_BITMAP_HEADER :
            stlStrncpy( aTypeStr, "BITMAP_HEADER", SMF_DUMP_PAGE_TYPE_LEN + 1 );
            break;
        case SMP_PAGE_TYPE_CIRCULAR_HEADER:
            stlStrncpy( aTypeStr, "CIRCULAR_HEADER", SMF_DUMP_PAGE_TYPE_LEN + 1 );
            break;
        case SMP_PAGE_TYPE_FLAT_HEADER :
            stlStrncpy( aTypeStr, "FLAT_HEADER", SMF_DUMP_PAGE_TYPE_LEN + 1 );
            break;
        case SMP_PAGE_TYPE_PENDING_HEADER :
            stlStrncpy( aTypeStr, "PENDING_HEADER", SMF_DUMP_PAGE_TYPE_LEN + 1 );
            break;
        case SMP_PAGE_TYPE_UNDO :
            stlStrncpy( aTypeStr, "UNDO", SMF_DUMP_PAGE_TYPE_LEN + 1 );
            break;
        case SMP_PAGE_TYPE_TRANS :
            stlStrncpy( aTypeStr, "TRANS", SMF_DUMP_PAGE_TYPE_LEN + 1 );
            break;
        case SMP_PAGE_TYPE_BITMAP_INTERNAL :
            stlStrncpy( aTypeStr, "BITMAP_INTERNAL", SMF_DUMP_PAGE_TYPE_LEN + 1 );
            break;
        case SMP_PAGE_TYPE_BITMAP_LEAF :
            stlStrncpy( aTypeStr, "BITMAP_LEAF", SMF_DUMP_PAGE_TYPE_LEN + 1 );
            break;
        case SMP_PAGE_TYPE_BITMAP_EXTENT_MAP :
            stlStrncpy( aTypeStr, "BITMAP_EXTENT_MAP", SMF_DUMP_PAGE_TYPE_LEN + 1 );
            break;
        case SMP_PAGE_TYPE_TABLE_DATA :
            stlStrncpy( aTypeStr, "TABLE_DATA", SMF_DUMP_PAGE_TYPE_LEN + 1 );
            break;
        case SMP_PAGE_TYPE_TABLE_COLUMNAR_DATA :
            stlStrncpy( aTypeStr, "TABLE_COLUMNAR_DATA", SMF_DUMP_PAGE_TYPE_LEN + 1 );
            break;
        case SMP_PAGE_TYPE_INDEX_DATA :
            stlStrncpy( aTypeStr, "INDEX_DATA", SMF_DUMP_PAGE_TYPE_LEN + 1 );
            break;
        case SMP_PAGE_TYPE_MAX :
            stlStrncpy( aTypeStr, "MAX", SMF_DUMP_PAGE_TYPE_LEN + 1 );
            break;
        default:
            stlStrncpy( aTypeStr, "UNKNOWN", SMF_DUMP_PAGE_TYPE_LEN + 1 );
            break;
    }
}

void smfGetDumpFreeness( smpFreeness   aFreeness,
                         stlChar     * aFreeStr )
{
    switch( aFreeness )
    {
        case SMP_FREENESS_UNALLOCATED:
            stlStrncpy( aFreeStr, "UALLOCATED", SMF_DUMP_FREENESS_TYPE_LEN );
            break;
        case SMP_FREENESS_FULL:
            stlStrncpy( aFreeStr, "FULL", SMF_DUMP_FREENESS_TYPE_LEN );
            break;
        case SMP_FREENESS_UPDATEONLY:
            stlStrncpy( aFreeStr, "UPDATE ONLY", SMF_DUMP_FREENESS_TYPE_LEN );
            break;
        case SMP_FREENESS_INSERTABLE:
            stlStrncpy( aFreeStr, "INSERTABLE", SMF_DUMP_FREENESS_TYPE_LEN );
            break;
        case SMP_FREENESS_FREE:
            stlStrncpy( aFreeStr, "FREE", SMF_DUMP_FREENESS_TYPE_LEN );
            break;
        default:
            break;
    }
}

void smfGetIncBackupObjectType( smlTbsId   aTbsId,
                                stlChar  * aObject )
{
    if( aTbsId == SML_INVALID_TBS_ID )
    {
        stlStrncpy( aObject, "DATABASE", SMF_DUMP_INC_BACKUP_OBJECT_TYPE_LEN );
    }
    else if( aTbsId < SML_INVALID_TBS_ID )
    {
        stlStrncpy( aObject, "TABLESPACE", SMF_DUMP_INC_BACKUP_OBJECT_TYPE_LEN );
    }
}

stlStatus smfDumpPage( stlChar * aPage,
                       stlInt8   aDumpFlag,
                       smlEnv  * aEnv )
{
    smpPhyHdr * sPhyHdr;
    stlChar     sStrOffset[SMF_DATAFILE_DUMP_PAGE_OFFSET_LEN];
    stlChar     sColumn1[SMF_DATAFILE_DUMP_COLUMN_MAX_SIZE];
    stlChar     sColumn2[SMF_DATAFILE_DUMP_COLUMN_MAX_SIZE];
    stlChar     sColumn3[SMF_DATAFILE_DUMP_COLUMN_MAX_SIZE];
    stlChar     sColumn4[SMF_DATAFILE_DUMP_COLUMN_MAX_SIZE];
    stlChar     sHexString[SMP_PAGE_SIZE * 2 + 1];
    stlInt32    sHexOffset;
    stlChar     sPageTypeStr[SMF_DUMP_PAGE_TYPE_LEN] = { '\0' };
    stlChar     sFreenessStr[SMF_DUMP_FREENESS_TYPE_LEN] = { '\0' };
    stlInt32    sRemainSize;
    stlInt32    sPageOffset;

    sPhyHdr = (smpPhyHdr *) aPage;
    smfGetDumpPageType( sPhyHdr->mPageType, sPageTypeStr );
    smfGetDumpFreeness( sPhyHdr->mFreeness, sFreenessStr );

    stlPrintf( "\n"
               "  TABLESPACE ID : %d\n"
               "  DATAFILE ID : %d\n"
               "  PAGE SEQUENCE ID : %d\n"
               "----------------------------------------------------------------------------\n",
               sPhyHdr->mSelfId.mTbsId,
               SMP_DATAFILE_ID( sPhyHdr->mSelfId.mPageId ),
               SMP_PAGE_SEQ_ID( sPhyHdr->mSelfId.mPageId ) );
    
    stlPrintf( "  [PHYSICAL HEADER] TYPE(%s), FREENESS(%s), LSN(%ld), TIMESTAMP(%lu), PARENT RID(%d,%d,%u), SEGMENT ID(%ld), MAX VIEW SCN(%ld), AGABLE SCN(%lu), SELF ID(%d,%u)\n"
               "----------------------------------------------------------------------------\n",
                   sPageTypeStr,
               sFreenessStr,
               sPhyHdr->mLsn,
               sPhyHdr->mTimestamp,
               sPhyHdr->mParentRid.mTbsId,
               sPhyHdr->mParentRid.mOffset,
               sPhyHdr->mParentRid.mPageId,
               sPhyHdr->mSegmentId,
               sPhyHdr->mMaxViewScn,
               sPhyHdr->mAgableScn,
               sPhyHdr->mSelfId.mTbsId,
               sPhyHdr->mSelfId.mPageId );

    if( (aDumpFlag & SML_DUMP_PAGE_ALL) == SML_DUMP_PAGE_ALL )
    {
        STL_TRY( stlBinary2HexString( (stlUInt8*) aPage,
                                      SMP_PAGE_SIZE,
                                      sHexString,
                                      (SMP_PAGE_SIZE * 2) + 1,
                                      KNL_ERROR_STACK(aEnv) ) == STL_SUCCESS );

        sRemainSize = SMP_PAGE_SIZE;
        sHexOffset  = 0;

        while( sRemainSize > 0 )
        {
            stlStrcpy( sColumn1, "                " );
            stlStrcpy( sColumn2, "                " );
            stlStrcpy( sColumn3, "                " );
            stlStrcpy( sColumn4, "                " );

            sPageOffset = SMP_PAGE_SIZE - sRemainSize;

            stlSnprintf( sStrOffset,
                         SMF_DATAFILE_DUMP_PAGE_OFFSET_LEN,
                         "%04X",
                         sPageOffset );

            smfWriteHexStringToColumn( sColumn1,
                                       sHexString,
                                       &sHexOffset,
                                       (SMP_PAGE_SIZE * 2) + 1 );
            smfWriteHexStringToColumn( sColumn2,
                                       sHexString,
                                       &sHexOffset,
                                       (SMP_PAGE_SIZE * 2) + 1 );
            smfWriteHexStringToColumn( sColumn3,
                                       sHexString,
                                       &sHexOffset,
                                       (SMP_PAGE_SIZE * 2) + 1 );
            smfWriteHexStringToColumn( sColumn4,
                                       sHexString,
                                       &sHexOffset,
                                       (SMP_PAGE_SIZE * 2) + 1 );

            stlPrintf( " %s    %s %s %s %s\n",
                       sStrOffset,
                       sColumn1, sColumn2, sColumn3, sColumn4 );
        
            sRemainSize -= SMF_DATAFILE_DUMP_LINE_LENGTH;
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
