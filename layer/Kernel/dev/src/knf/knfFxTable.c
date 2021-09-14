/*******************************************************************************
 * knfFxTable.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: knfFxTable.c 5797 2012-09-27 02:32:51Z jhkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file knfFxTable.c
 * @brief Kernel Layer File Manager Fixed Table Internal Routines
 */

#include <stl.h>
#include <dtl.h>
#include <knl.h>
#include <knlDef.h>
#include <knDef.h>
#include <knpDef.h>
#include <knpDataType.h>
#include <knpPropertyManager.h>
#include <knfFileMgr.h>

extern knsEntryPoint * gKnEntryPoint;

/**
 * @addtogroup knfFxTable
 * @{
 */

/**
 * @brief D$CRASH_FILE Table 정의
 */
knlFxColumnDesc gKnfDumpCrashFileColumnDesc[] =
{
    {
        "DUMMY",
        "dummy column",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knfFxCrashFile, mDummy ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        NULL,
        NULL,
        0,
        0,
        0,
        STL_FALSE  /* nullable */
    }
};

stlStatus knfGetDumpCrashFileObjectCallback( stlInt64   aTransID,
                                             void     * aStmt,
                                             stlChar  * aDumpObjName,
                                             void    ** aDumpObjHandle,
                                             knlEnv   * aEnv )
{
    /* Open Callback에서 Parsing */
    *aDumpObjHandle = (void*) aDumpObjName;
    
    return STL_SUCCESS;
}


/**
 * @brief D$CRASH_FILE Table의 open 함수
 * @param[in] aStmt Statement Pointer
 * @param[in] aDumpObjHandle Dump Object의 Handle
 * @param[in] aPathCtrl Path Controller
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knfOpenFxDumpCrashFileCallback( void   * aStmt,
                                          void   * aDumpObjHandle,
                                          void   * aPathCtrl,
                                          knlEnv * aEnv )
{
    stlChar       * sCurPtr;
    stlChar       * sToken1;
    stlChar       * sToken2;
    stlChar       * sToken3;
    stlInt32        sTokenCount;
    knfFxPathCtrl * sPathCtrl;
    stlBool         sIsExist = STL_TRUE;
    stlUInt64       sNum64Bit;
    stlChar       * sEndPtr;
    stlInt32        sStrLen;

    sPathCtrl = (knfFxPathCtrl*)aPathCtrl;
    sCurPtr   = (stlChar*) aDumpObjHandle;

    /**
     * file name
     */
    
    STL_TRY( knlTokenizeNextDumpOption( &sCurPtr,
                                        &sToken1,
                                        &sToken2,
                                        &sToken3,
                                        &sTokenCount,
                                        aEnv ) == STL_SUCCESS );

    if( sTokenCount == 2 )
    {
        /**
         * target file이 xx.xx로 구성되어 있는 경우
         */
        sStrLen = stlSnprintf( sPathCtrl->mFileName,
                               STL_MAX_FILE_NAME_LENGTH + STL_MAX_FILE_PATH_LENGTH,
                               "%s.%s",
                               sToken1,
                               sToken2 );
    }
    else
    {
        if( sTokenCount == 1 )
        {
            stlStrncpy( sPathCtrl->mFileName,
                        sToken1,
                        STL_MAX_FILE_NAME_LENGTH + STL_MAX_FILE_PATH_LENGTH );
        }
        else
        {
            /**
             * target file이 xx.xx.xx로 구성되어 있는 경우는 지원하지 않는다.
             */
            STL_THROW( RAMP_ERR_FINISH );
        }
    }

    /**
     * 절대 path로 변환
     */

    STL_TRY( stlMergeAbsFilePath( knpGetHomeDir(),
                                  sPathCtrl->mFileName,
                                  sPathCtrl->mFileName,
                                  STL_MAX_FILE_NAME_LENGTH + STL_MAX_FILE_PATH_LENGTH,
                                  &sStrLen,
                                  KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    STL_TRY( stlExistFile( sPathCtrl->mFileName,
                           &sIsExist,
                           KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sIsExist == STL_TRUE, RAMP_ERR_FINISH );

    /**
     * file offset
     */
    
    STL_TRY( knlTokenizeNextDumpOption( &sCurPtr,
                                        &sToken1,
                                        &sToken2,
                                        &sToken3,
                                        &sTokenCount,
                                        aEnv ) == STL_SUCCESS );    
    
    STL_TRY_THROW( sTokenCount == 1, RAMP_ERR_FINISH );
    
    STL_TRY( stlStrToUInt64( sToken1,
                             STL_NTS,
                             &sEndPtr,
                             10,
                             &sNum64Bit,
                             KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
    sPathCtrl->mOffset = sNum64Bit;
    
    /**
     * crash bytes
     */
    
    STL_TRY( knlTokenizeNextDumpOption( &sCurPtr,
                                        &sToken1,
                                        &sToken2,
                                        &sToken3,
                                        &sTokenCount,
                                        aEnv ) == STL_SUCCESS );    
    
    STL_TRY_THROW( sTokenCount == 1, RAMP_ERR_FINISH );

    sPathCtrl->mCrashBytesSize = stlStrlen( sToken1 );
    stlMemcpy( sPathCtrl->mCrashBytes,
               sToken1,
               STL_MIN( sPathCtrl->mCrashBytesSize,
                        KNF_MAX_CRASH_BYTES ) );

    /**
     * Option: repeat count
     */ 
    STL_TRY( knlTokenizeNextDumpOption( &sCurPtr,
                                        &sToken1,
                                        &sToken2,
                                        &sToken3,
                                        &sTokenCount,
                                        aEnv ) == STL_SUCCESS );

    if( sTokenCount == 0 )
    {
        sPathCtrl->mRepeatCnt  = 1;
        sPathCtrl->mRepeatUnit = 0;
    
        STL_THROW( RAMP_SKIP );
    }

    STL_TRY( stlStrToUInt64( sToken1,
                             STL_NTS,
                             &sEndPtr,
                             10,
                             &sNum64Bit,
                             KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
    
    sPathCtrl->mRepeatCnt = sNum64Bit;
    
    /**
     * Option: repeating unit
     */ 
    STL_TRY( knlTokenizeNextDumpOption( &sCurPtr,
                                        &sToken1,
                                        &sToken2,
                                        &sToken3,
                                        &sTokenCount,
                                        aEnv ) == STL_SUCCESS );

    STL_TRY_THROW( sTokenCount == 1, RAMP_ERR_FINISH );

    STL_TRY( stlStrToUInt64( sToken1,
                             STL_NTS,
                             &sEndPtr,
                             10,
                             &sNum64Bit,
                             KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
    
    sPathCtrl->mRepeatUnit = sNum64Bit;
    
    STL_RAMP( RAMP_SKIP );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_FINISH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_INVALID_DUMP_OPTION_STRING,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sPathCtrl->mFileName );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief D$CRASH_FILE Table의 close 함수
 * @param[in] aDumpObjHandle Dump Object의 Handle
 * @param[in] aPathCtrl Path Controller
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knfCloseFxDumpCrashFileCallback( void   * aDumpObjHandle,
                                           void   * aPathCtrl,
                                           knlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief D$CRASH_FILE Table의 레코드 구축 함수
 * @param[in] aDumpObjHandle Dump Object Handle
 * @param[in] aPathCtrl Path Controller
 * @param[in,out] aValueList 값을 담을 공간
 * @param[in]  aBlockIdx   Block Idx
 * @param[out] aTupleExist 레코드 존재 여부
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knfBuildRecordFxCrashFileCallback( void              * aDumpObjHandle,
                                             void              * aPathCtrl,
                                             knlValueBlockList * aValueList,
                                             stlInt32            aBlockIdx,
                                             stlBool           * aTupleExist,
                                             knlEnv            * aEnv )
{
    knfFxPathCtrl * sPathCtrl;
    stlInt32        sState = 0;
    stlFile         sFile;
    stlInt8         sCrashBytes[KNF_MAX_CRASH_BYTES];
    stlInt64        sCrashBytesSize;
    stlInt64        i;
    stlOffset       sOffset;
    
    *aTupleExist = STL_FALSE;
    
    sPathCtrl = (knfFxPathCtrl*)aPathCtrl;

    STL_TRY( stlOpenFile( &sFile,
                          sPathCtrl->mFileName,
                          STL_FOPEN_WRITE,
                          STL_FPERM_OS_DEFAULT,
                          KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( dtlToBinaryStringFromString( sPathCtrl->mCrashBytes,
                                          sPathCtrl->mCrashBytesSize,
                                          KNF_MAX_CRASH_BYTES,
                                          sCrashBytes,
                                          &sCrashBytesSize,
                                          KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );
    
    for( i = 0; i < sPathCtrl->mRepeatCnt; i++ )
    {
        sOffset = sPathCtrl->mOffset + ( i * sPathCtrl->mRepeatUnit );
        
        STL_TRY( stlSeekFile( &sFile,
                              STL_FSEEK_SET,
                              &sOffset,
                              KNL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );
        
        STL_TRY( stlWriteFile( &sFile,
                               sCrashBytes,
                               sCrashBytesSize,
                               NULL,
                               KNL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );
    }
    
    STL_TRY( stlSyncFile( &sFile,
                          KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( stlCloseFile( &sFile,
                           KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );
                          
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) stlCloseFile( &sFile, KNL_ERROR_STACK(aEnv) );
    }
    
    return STL_FAILURE;
}

knlFxTableDesc gKnfCrashFileTableDesc =
{
    KNL_FXTABLE_USAGE_DUMP_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    knfGetDumpCrashFileObjectCallback,
    knfOpenFxDumpCrashFileCallback,
    knfCloseFxDumpCrashFileCallback,
    knfBuildRecordFxCrashFileCallback,
    STL_SIZEOF(knfFxPathCtrl),
    "D$CRASH_FILE",
    "crash file - option string ( file_name, start_offset, crash_bytes [repeat_count, repeating_unit] )",
    gKnfDumpCrashFileColumnDesc
};

/**
 * @brief X$DB_FILE Table 정의
 */
knlFxColumnDesc gKnDbFileColumnDesc[] =
{
    {
        "PATH",
        "path and name of database file",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( knfFxDbFile, mPath ),
        STL_MAX_FILE_PATH_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "TYPE",
        "type of database file",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( knfFxDbFile, mFileType ),
        KNF_DB_FILE_TYPE_STRING_LEN,
        STL_FALSE  /* nullable */
    },
    {
        NULL,
        NULL,
        0,
        0,
        0,
        STL_FALSE  /* nullable */
    }
};

/**
 * @brief X$DB_FILE Table의 open 함수
 * @param[in] aStmt          Statement Pointer
 * @param[in] aDumpObjHandle Dump Object의 Handle
 * @param[in] aPathCtrl Path Controller
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knfOpenFxDbFileCallback( void   * aStmt,
                                   void   * aDumpObjHandle,
                                   void   * aPathCtrl,
                                   knlEnv * aEnv )
{
    knfFxDbFilePathCtrl  * sDbFilePathCtrl;
    stlInt32               sState = 0;
    stlBool                sIsSuccess;

    sDbFilePathCtrl = (knfFxDbFilePathCtrl *)aPathCtrl;

    STL_TRY( knlAcquireLatch( &(gKnEntryPoint->mFileMgr.mFileLatch),
                              KNL_LATCH_MODE_SHARED,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsSuccess,
                              aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( knfInitDbFileIterator( &sDbFilePathCtrl->mCurFile,
                                    aEnv )
             == STL_SUCCESS );

    sDbFilePathCtrl->mValidSeq = knlGetExclLockSeq( &(gKnEntryPoint->mFileMgr.mFileLatch) );

    sState = 0;
    STL_TRY( knlReleaseLatch( &(gKnEntryPoint->mFileMgr.mFileLatch), aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) knlReleaseLatch( &(gKnEntryPoint->mFileMgr.mFileLatch), aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief X$DB_FILE Table의 close 함수
 * @param[in] aDumpObjHandle Dump Object의 Handle
 * @param[in] aPathCtrl Path Controller
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knfCloseFxDbFileCallback( void   * aDumpObjHandle,
                                          void   * aPathCtrl,
                                          knlEnv * aEnv )
{
    STL_TRY( knfFiniDbFileIterator( aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief X$DB_FILE Table의 레코드 구축 함수
 * @param[in] aDumpObjHandle Dump Object Handle
 * @param[in] aPathCtrl Path Controller
 * @param[in,out] aValueList 값을 담을 공간
 * @param[in]  aBlockIdx   Block Idx
 * @param[out] aTupleExist 레코드 존재 여부
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knfBuildRecordFxDbFileCallback( void              * aDumpObjHandle,
                                                void              * aPathCtrl,
                                                knlValueBlockList * aValueList,
                                                stlInt32            aBlockIdx,
                                                stlBool           * aTupleExist,
                                                knlEnv            * aEnv )
{
    knfFxDbFilePathCtrl  * sPathCtrl;
    knfFxDbFile            sFxRecord;
    stlInt32               sState = 0;
    stlBool                sIsSuccess;

    *aTupleExist = STL_FALSE;

    sPathCtrl = (knfFxDbFilePathCtrl *)aPathCtrl;

    STL_TRY( knlAcquireLatch( &(gKnEntryPoint->mFileMgr.mFileLatch),
                              KNL_LATCH_MODE_SHARED,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsSuccess,
                              aEnv ) == STL_SUCCESS );
    sState = 1;

    /**
     * File Manager의 변경사항 검사
     */
    STL_TRY_THROW( sPathCtrl->mValidSeq == knlGetExclLockSeq( &(gKnEntryPoint->mFileMgr.mFileLatch) ),
                   RAMP_ERR_MODIFIED_OBJECT );

    STL_TRY( knfNextDbFile( &(sPathCtrl->mCurFile), aEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( knlReleaseLatch( &(gKnEntryPoint->mFileMgr.mFileLatch), aEnv )
             == STL_SUCCESS );

    if( sPathCtrl->mCurFile != NULL )
    {
        stlStrcpy( sFxRecord.mPath,
                   sPathCtrl->mCurFile->mFileName );
        knfFileTypeString( sPathCtrl->mCurFile->mFileType, sFxRecord.mFileType );
        
        *aTupleExist = STL_TRUE;

        STL_TRY( knlBuildFxRecord( gKnDbFileColumnDesc,
                                   &sFxRecord,
                                   aValueList,
                                   aBlockIdx,
                                   aEnv )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_MODIFIED_OBJECT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_MODIFIED_DUMP_OBJECT,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    if( sState == 1 )
    {
        (void) knlReleaseLatch( &(gKnEntryPoint->mFileMgr.mFileLatch), aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief X$DB_FILE Table 정보
 */
knlFxTableDesc gKnDbFileTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_MOUNT,
    NULL,
    knfOpenFxDbFileCallback,
    knfCloseFxDbFileCallback,
    knfBuildRecordFxDbFileCallback,
    STL_SIZEOF( knfFxDbFilePathCtrl ),
    "X$DB_FILE",
    "show database files",
    gKnDbFileColumnDesc
};

/** @} */
