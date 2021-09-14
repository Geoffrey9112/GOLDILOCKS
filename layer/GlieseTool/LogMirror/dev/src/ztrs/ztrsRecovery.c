/*******************************************************************************
 * ztrsProtocol.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file ztrsProtocol.c
 * @brief GlieseTool LogMirror Sender Protocol Routines
 */

#include <goldilocks.h>
#include <ztr.h>

extern stlBool       gRunState;
extern ztrMasterMgr  gMasterMgr;

stlStatus ztrsInitializeRecovery( ztrRecoveryinfo * aRecoveryInfo,
                                  stlErrorStack   * aErrorStack )
{
    stlInt32 sState      = 0;
    stlInt64 sBufferSize;       
    
    ztrRecoveryMgr * sRecoveryMgr = &(gMasterMgr.mRecoveryMgr);
    
    /**
     * Recovery에 사용되는 Buffer의 사이즈는 Mirror Buffer 사이즈와 동일해야한다.
     * Recovery 이전에 Slave에 이미 Buffer Size를 알려줬기 때문이 이미 Slave는 buffer Size가 정해져있다.
     */
    sBufferSize = gMasterMgr.mLmInfo->mSize;
    
    STL_TRY( stlCreateRegionAllocator( &(sRecoveryMgr->mRegionMem),
                                       ZTR_REGION_INIT_SIZE,
                                       ZTR_REGION_NEXT_SIZE,
                                       aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztrmAllocEnvHandle( &(sRecoveryMgr->mEnvHandle),
                                 aErrorStack ) == STL_SUCCESS );
    sState = 2;
    
    STL_TRY( ztrmCreateConnection( &(sRecoveryMgr->mDbcHandle),
                                   sRecoveryMgr->mEnvHandle,
                                   aErrorStack ) == STL_SUCCESS );
    sState = 3;
    
    STL_RING_INIT_HEADLINK( &(sRecoveryMgr->mLogFileList),
                            STL_OFFSETOF( ztrLogFileInfo, mLink ) );
    
    /**
     * RedoLogFile List 얻기
     */
    STL_TRY( ztrsBuildLogFileInfo( aRecoveryInfo,
                                   aErrorStack ) == STL_SUCCESS );
    
    /**
     * Archive Log File 경로 얻기
     */
    STL_TRY( ztrsBuildArchiveInfo( aErrorStack ) == STL_SUCCESS );
    
    /**
     * Recovery에서 파일을 읽어들일 Buffer 생성
     */
    STL_TRY( stlAllocRegionMem( &(sRecoveryMgr->mRegionMem),
                                sBufferSize + STL_SIZEOF( ztrLogMirrorHdr ),
                                (void**)&sRecoveryMgr->mSendBuff,
                                aErrorStack ) == STL_SUCCESS );
    
    sRecoveryMgr->mBufferSize = sBufferSize;
    sRecoveryMgr->mWriteIdx   = 0;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    switch( sState )
    {
        case 3:
            (void) ztrmDestroyConnection( sRecoveryMgr->mDbcHandle,
                                          aErrorStack );
        case 2:
            (void) ztrmFreeEnvHandle( sRecoveryMgr->mEnvHandle, 
                                      aErrorStack );
        case 1:
            (void) stlDestroyRegionAllocator( &(sRecoveryMgr->mRegionMem),
                                              aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}

stlStatus ztrsFinalizeRecovery( stlErrorStack * aErrorStack )
{
    ztrRecoveryMgr * sRecoveryMgr = &(gMasterMgr.mRecoveryMgr);
    
    STL_TRY( ztrmDestroyConnection( sRecoveryMgr->mDbcHandle,
                                    aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztrmFreeEnvHandle( sRecoveryMgr->mEnvHandle, 
                                aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlDestroyRegionAllocator( &(sRecoveryMgr->mRegionMem),
                                        aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztrsDoRecovery( ztrRecoveryinfo * aRecoveryInfo,
                          stlErrorStack   * aErrorStack )
{
    stlInt32         sState        = 0;
    stlInt64         sFileSeqNo    = -1;
    stlInt32         sResult       = ZTR_RECOVERY_RESULT_MAX;
    stlInt16         sGroupId      = -1;
    ztrLogFileInfo * sLogFileInfo  = NULL;
    stlBool          sSendLogHdr   = STL_FALSE;
    stlBool          sRecoveryDone = STL_FALSE;
    stlBool          sLockFlusher  = STL_FALSE;
    stlChar          sFileName[ STL_MAX_FILE_PATH_LENGTH ] = {0,};
    
    ztrLogFileHdr    sLogFileHdr;
    ztrRecoveryMgr * sRecoveryMgr = &(gMasterMgr.mRecoveryMgr);
    
    STL_TRY( ztrsInitializeRecovery( aRecoveryInfo,
                                     aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztrsGetCurrentLogFileInfo( &sLogFileInfo,
                                        aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztrmLogMsg( aErrorStack,
                         "[SENDER-RECOVERY] START.\n" ) == STL_SUCCESS );
    
    while( 1 )
    {
        STL_ASSERT( sLogFileInfo != NULL );
        
        stlStrncpy( sFileName, sLogFileInfo->mName, stlStrlen( sLogFileInfo->mName ) + 1 );
        
        STL_TRY( ztrsGetLogFileSeqNoNGroupId( sFileName,
                                              &sFileSeqNo,
                                              &sGroupId,
                                              aErrorStack ) == STL_SUCCESS );
        
        STL_TRY_THROW( sLogFileInfo->mFileSeqNo <= sFileSeqNo, RAMP_ERR_INVALID_LOGFILE );
        
        if( sLogFileInfo->mFileSeqNo != sFileSeqNo )
        {
            /**
             * Online Redo Log가 reuse되었으므로, Archive Log를 읽어야 한다.
             */
            STL_TRY_THROW( sRecoveryMgr->mArchiveMode == STL_TRUE, RAMP_ERR_NO_ARCHIVELOG_FILE );
            
            STL_TRY( ztrsGetArchiveLogFile( sFileName,
                                            sLogFileInfo,
                                            sRecoveryMgr,
                                            aErrorStack ) == STL_SUCCESS );
            
            STL_TRY( ztrsValidateArchiveLogFile( sFileName,
                                                 sLogFileInfo,
                                                 aErrorStack ) == STL_SUCCESS );
        }
        else
        {
            STL_TRY_THROW( sLogFileInfo->mGroupId == sGroupId, RAMP_ERR_INVALID_LOGFILE );
        }
        
        if( sSendLogHdr == STL_TRUE )
        {
            STL_TRY( ztrsGetLogFileHdr( sFileName,
                                        &sLogFileHdr,
                                        aErrorStack ) == STL_SUCCESS );
            
            /**
             * File Sequence No를 다시 한번 확인해 본다.
             */
            if( sLogFileInfo->mFileSeqNo != sLogFileHdr.mLogFileSeqNo )
            {
                /**
                 * 읽는 도중에 LogFile Switch가 발생해서, ArchiveLog에서 읽어야 함.
                 */
                continue; 
            }
            else
            {
                /**
                 * 기존 Buffer모두 보낸 뒤에 LogFileHdr를 보낸다.
                 */
                STL_TRY( ztrsRecoveryProtocol( STL_FALSE,
                                               aErrorStack ) == STL_SUCCESS );
                
                STL_DASSERT( sRecoveryMgr->mWriteIdx == 0 );
                
                stlMemcpy( &sRecoveryMgr->mSendBuff[ sRecoveryMgr->mWriteIdx ],
                           (stlChar*)&sLogFileHdr,
                           STL_SIZEOF( ztrLogFileHdr ) );
    
                sRecoveryMgr->mWriteIdx += STL_SIZEOF( ztrLogFileHdr );
                
                STL_TRY( ztrsRecoveryProtocol( STL_TRUE,
                                               aErrorStack ) == STL_SUCCESS );
            }
            
            sSendLogHdr = STL_FALSE;
        }
        
        STL_TRY( ztrmLogMsg( aErrorStack,
                             "[SENDER-RECOVERY] %s \n",
                             sFileName ) == STL_SUCCESS );
        
        STL_TRY( ztrsReadLogFile( sFileName,
                                  sLogFileInfo,
                                  sRecoveryMgr,
                                  &sResult,
                                  aErrorStack ) == STL_SUCCESS );
        
        if( sResult == ZTR_RECOVERY_RESULT_READ_DONE )
        {
            /**
             * File에 쓰여진 Valid redo Log를 모두 읽은 상태임
             * 0. 기존의 데이터를 모두 보낸다.
             * 1. Log Flusher를 잠근다.
             * 3. Update 된 LogFile을 모두 읽어서 보낸다.
             * 4. LogBuffer에서 LogMirror가 동작하도록 설정한다.
             * 5. Log Flusher를 푼다.
             */
            STL_TRY( ztrsRecoveryProtocol( STL_FALSE,
                                           aErrorStack ) == STL_SUCCESS );
            
            if( sRecoveryDone == STL_FALSE )
            {
                STL_TRY( ztrsControlLogFlush( STL_TRUE,
                                              aErrorStack ) == STL_SUCCESS );
                sLockFlusher  = STL_TRUE;
                sRecoveryDone = STL_TRUE;
                continue;
            }
            else
            {
                STL_TRY( ztrmLogMsg( aErrorStack,
                                     "[SENDER-RECOVERY] DONE.\n" ) == STL_SUCCESS );
                /**
                 * Recovery가 동작했다는 것은 기존에 운영정보가 있다는 얘기이기 떄문에
                 * Recovery가 끝나면 정상적인 운영을 위하여 START되었다는 정보를 세팅한다.
                 * 해당 정보는 Log switch가 발생한 이후에도 START되었다고 세팅된다.
                 * 
                 * 반드시 LogMirror의 접속여부를 세팅하고 Flusher를 풀어야 한다.
                 * 순서를 바꾸면 안됨!!!
                 */
                gMasterMgr.mLmInfo->mIsStarted = STL_TRUE;
                gMasterMgr.mLmInfo->mConnected = ZTR_LOGMIRROR_CONNECTED;
                
                STL_TRY( ztrmLogMsg( aErrorStack,
                                     "[SENDER] LogMirror Attached to Shared Memory\n" ) == STL_SUCCESS );
                
                STL_TRY( ztrsControlLogFlush( STL_FALSE,
                                              aErrorStack ) == STL_SUCCESS );
                sLockFlusher = STL_FALSE;
                
                break;
            }
        }
        else if( sResult == ZTR_RECOVERY_RESULT_EOF )
        {
            /**
             * Buffer에 있는 내용을 모두 Flush 하고 LogFileHeader를 보내도록 설정함
             */
            STL_TRY( ztrsRecoveryProtocol( STL_FALSE,
                                           aErrorStack ) == STL_SUCCESS );
            
            STL_TRY( ztrsGetNextLogFileInfo( &sLogFileInfo,
                                             aErrorStack ) == STL_SUCCESS );
            
            sSendLogHdr = STL_TRUE;
        }
        else if ( sResult == ZTR_RECOVERY_RESULT_INVALID_WRAP_NO )
        {
            /**
             * Buffer에 있는 내용을 모두 Flush 하고.. Archive Log를 읽도록 해야 함
             */
            STL_TRY( ztrsRecoveryProtocol( STL_FALSE,
                                           aErrorStack ) == STL_SUCCESS );
        }
        else
        {
            STL_DASSERT( STL_TRUE );
        }
    }
    
    sState = 0;
    STL_TRY( ztrsFinalizeRecovery( aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_LOGFILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_INVALID_LOGFILE,
                      NULL,
                      aErrorStack,
                      sFileName );
    }
    STL_CATCH( RAMP_ERR_NO_ARCHIVELOG_FILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_INVALID_ARCHIVELOG,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    STL_TRY( ztrmLogMsg( aErrorStack,
                         "[SENDER-RECOVERY] FAIL.\n" ) == STL_SUCCESS );
    
    /**
     * Flusher가 잠겨있는 상태에서 오류가 발생했을 경우에는
     * 반드시 Flusher를 풀어주고 중단해야 한다. 그렇지 않을 경우에는 DB는 멈추게 된다.!!!!
     */
    if( sLockFlusher == STL_TRUE )
    {
        STL_TRY( ztrmLogMsg( aErrorStack,
                             "[SENDER] LogMirror Detached to Shared Memory\n" ) == STL_SUCCESS );
        
        /**
         * 아래는 반드시 성공해야 하긴하는데.....
         * 그렇지 않을 경우에는 GOLDILOCKS가 멈추게 된다.
         */
        (void) ztrsControlLogFlush( STL_FALSE,
                                    aErrorStack );
    }
    
    /**
     * 어떠한 경우라도 실패할 경우에는 아래의 값을 세팅해 줘야 함
     */
    gMasterMgr.mLmInfo->mIsStarted = STL_FALSE; 
    gMasterMgr.mLmInfo->mConnected = ZTR_LOGMIRROR_DISCONNECTED;
        
    switch( sState )
    {
        case 1:
            (void) ztrsFinalizeRecovery( aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}



stlStatus ztrsBuildLogFileInfo( ztrRecoveryinfo * aRecoveryInfo,
                                stlErrorStack   * aErrorStack )
{
    stlChar     sSqlString[ZTR_COMMAND_BUFFER_SIZE] = {0,};
    stlInt32    sState = 0;
    
    SQLHSTMT    sStmtHandle;
    SQLLEN      sIndicator[2];
    SQLRETURN   sResult;
    
    stlInt16    sGroupId;
    stlChar     sName[STL_MAX_FILE_PATH_LENGTH];
    
    ztrLogFileInfo * sLogFileInfo = NULL;
    ztrRecoveryMgr * sRecoveryMgr = &(gMasterMgr.mRecoveryMgr);
    
    stlSnprintf( sSqlString,
                 ZTR_COMMAND_BUFFER_SIZE,
                 "SELECT GROUP_ID, NAME "
                 "FROM FIXED_TABLE_SCHEMA.X$LOG_MEMBER ORDER BY GROUP_ID ASC" );
    
    STL_TRY( ztrmSQLAllocStmt( sRecoveryMgr->mDbcHandle,
                               &sStmtHandle,
                               aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztrmSQLPrepare( sStmtHandle,
                             (SQLCHAR*)sSqlString,
                             stlStrlen( sSqlString ),
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztrmSQLBindCol( sStmtHandle,
                             1,
                             SQL_C_SHORT, 
                             &sGroupId,
                             STL_SIZEOF( sGroupId ),
                             &sIndicator[0],
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztrmSQLBindCol( sStmtHandle,
                             2,
                             SQL_C_CHAR,
                             sName,
                             STL_SIZEOF( sName ),
                             &sIndicator[1],
                             aErrorStack ) == STL_SUCCESS );

    STL_TRY( ztrmSQLExecute( sStmtHandle, 
                             &sResult,
                             aErrorStack ) == STL_SUCCESS );
    
    while( 1 )
    {
        stlMemset( sName, 0x00, STL_SIZEOF( sName ) );
        
        STL_TRY( ztrmSQLFetch( sStmtHandle, 
                               &sResult, 
                               aErrorStack ) == STL_SUCCESS );
        
        if( sResult == SQL_NO_DATA )
        {
            break;
        }
        
        STL_TRY( stlAllocRegionMem( &(sRecoveryMgr->mRegionMem),
                                    STL_SIZEOF( ztrLogFileInfo ),
                                    (void**)&sLogFileInfo,
                                    aErrorStack ) == STL_SUCCESS );
        
        sLogFileInfo->mGroupId = sGroupId;
        stlStrncpy( sLogFileInfo->mName, sName, stlStrlen( sName ) + 1 );
        
        if( aRecoveryInfo->mLogGroupId == sGroupId )
        {
            sLogFileInfo->mActive     = STL_TRUE;
            sLogFileInfo->mFileSeqNo  = aRecoveryInfo->mLogFileSeqNo;
            sLogFileInfo->mBlockSeqNo = aRecoveryInfo->mLastBlockSeq;
        }
        else
        {
            sLogFileInfo->mActive     = STL_FALSE;
            sLogFileInfo->mFileSeqNo  = ZTR_INVALID_FILE_SEQ_NO;
            sLogFileInfo->mBlockSeqNo = 0;
        }
        
        STL_RING_INIT_DATALINK( sLogFileInfo,
                                STL_OFFSETOF( ztrLogFileInfo, mLink ) );
    
        STL_RING_ADD_LAST( &sRecoveryMgr->mLogFileList, 
                           sLogFileInfo );
    }
    
    sState = 0;
    STL_TRY( ztrmSQLFreeStmt( sStmtHandle, 
                              aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;   
    
    STL_FINISH;
    
    switch( sState )
    {
        case 1:
            (void) ztrmSQLFreeStmt( sStmtHandle, 
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztrsBuildArchiveInfo( stlErrorStack * aErrorStack )
{
    stlInt32    sState         = 0;
    stlChar     sSqlString[ZTR_COMMAND_BUFFER_SIZE] = {0,};
    stlChar     sPath[STL_MAX_FILE_PATH_LENGTH] = {0,};
    stlChar     sPrefix[STL_MAX_FILE_NAME_LENGTH] = {0,};
    
    SQLHSTMT    sStmtHandle;
    SQLLEN      sIndicator[2];
    SQLRETURN   sResult;
    
    ztrRecoveryMgr * sRecoveryMgr = &(gMasterMgr.mRecoveryMgr);
    
    stlSnprintf( sSqlString,
                 ZTR_COMMAND_BUFFER_SIZE,
                 "SELECT ARCHIVELOG_DIR_1, ARCHIVELOG_FILE "
                 "FROM  FIXED_TABLE_SCHEMA.X$ARCHIVELOG "
                 "WHERE ARCHIVELOG_MODE='ARCHIVELOG'" );
    
    STL_TRY( ztrmSQLAllocStmt( sRecoveryMgr->mDbcHandle,
                               &sStmtHandle,
                               aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztrmSQLPrepare( sStmtHandle,
                             (SQLCHAR*)sSqlString,
                             stlStrlen( sSqlString ),
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztrmSQLBindCol( sStmtHandle,
                             1,
                             SQL_C_CHAR,
                             sPath,
                             STL_SIZEOF( sPath ),
                             &sIndicator[0],
                             aErrorStack ) == STL_SUCCESS );

    STL_TRY( ztrmSQLBindCol( sStmtHandle,
                             2,
                             SQL_C_CHAR,
                             sPrefix,
                             STL_SIZEOF( sPrefix ),
                             &sIndicator[1],
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztrmSQLExecute( sStmtHandle, 
                             &sResult, 
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztrmSQLFetch( sStmtHandle, 
                           &sResult, 
                           aErrorStack ) == STL_SUCCESS );
    
    if( sResult == SQL_NO_DATA )
    {
        sRecoveryMgr->mArchiveMode = STL_FALSE;
    }
    else
    {
        stlStrncpy( sRecoveryMgr->mArchivePath, sPath, stlStrlen( sPath ) + 1 );
        stlStrncpy( sRecoveryMgr->mArchiveFilePrefix, sPrefix, stlStrlen( sPrefix ) + 1 );
        
        sRecoveryMgr->mArchiveMode = STL_TRUE;
    }
    
    sState = 0;
    STL_TRY( ztrmSQLFreeStmt( sStmtHandle, 
                              aErrorStack ) == STL_SUCCESS ); 
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    if( sState > 0 )
    {
        (void) ztrmSQLFreeStmt( sStmtHandle, 
                                aErrorStack );
    }
    
    return STL_FAILURE;
}

stlStatus ztrsGetCurrentLogFileInfo( ztrLogFileInfo ** aLogFileInfo,
                                     stlErrorStack   * aErrorStack )
{
    ztrLogFileInfo  * sLogFileInfo = NULL;
    ztrRecoveryMgr  * sRecoveryMgr = &(gMasterMgr.mRecoveryMgr);
    
    STL_RING_FOREACH_ENTRY( &(sRecoveryMgr->mLogFileList), sLogFileInfo )
    {
        if( sLogFileInfo->mActive == STL_TRUE )
        {
            break;   
        }
    }
    
    STL_DASSERT( (void*)sLogFileInfo != (void*)&sRecoveryMgr->mLogFileList );
    
    *aLogFileInfo = sLogFileInfo;
    
    return STL_SUCCESS;
}

stlStatus ztrsGetNextLogFileInfo( ztrLogFileInfo ** aLogFileInfo,
                                  stlErrorStack   * aErrorStack )
{
    ztrLogFileInfo   * sLogFileInfo    = NULL;
    ztrLogFileInfo   * sLogFileInfoBef = *aLogFileInfo;
    ztrRecoveryMgr   * sRecoveryMgr    = &(gMasterMgr.mRecoveryMgr);
    
    sLogFileInfo = STL_RING_GET_NEXT_DATA( &(sRecoveryMgr->mLogFileList), sLogFileInfoBef );
    
    if ( (void*)sLogFileInfo == (void*)&(sRecoveryMgr->mLogFileList) )
    {
        sLogFileInfo = STL_RING_GET_FIRST_DATA( &(sRecoveryMgr->mLogFileList) );
    }
    
    sLogFileInfo->mActive     = STL_TRUE;
    sLogFileInfo->mFileSeqNo  = sLogFileInfoBef->mFileSeqNo + 1;
    sLogFileInfo->mBlockSeqNo = 0;
    
    sLogFileInfoBef->mActive     = STL_FALSE;
    sLogFileInfoBef->mFileSeqNo  = ZTR_INVALID_FILE_SEQ_NO;
    
    *aLogFileInfo = sLogFileInfo;
    
    return STL_SUCCESS;
}


stlStatus ztrsGetLogFileSeqNoNGroupId( stlChar        * aFileName,
                                       stlInt64       * aFileSeqNo,
                                       stlInt16       * aGroupId,
                                       stlErrorStack  * aErrorStack )
{
    stlFile        sFile;
    stlInt32       sState  = 0;
    ztrLogFileHdr  sLogFileHdr;
    
    void         * sLogFileHdrBuffer = NULL;
    void         * sAlignedPtr       = NULL;
    
    STL_TRY( stlOpenFile( &sFile,
                          aFileName,
                          STL_FOPEN_READ | STL_FOPEN_DIRECT | STL_FOPEN_LARGEFILE,
                          STL_FPERM_OS_DEFAULT,
                          aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( stlAllocHeap( (void**)&sLogFileHdrBuffer,
                           ZTR_DEFAULT_BLOCK_SIZE * 2,
                           aErrorStack ) == STL_SUCCESS );
    
    sAlignedPtr = (void*)STL_ALIGN( (stlUInt64)sLogFileHdrBuffer , ZTR_MEM_ALIGN );
    
    STL_TRY( stlReadFile( &sFile,
                          sAlignedPtr,
                          ZTR_DEFAULT_BLOCK_SIZE,
                          NULL,
                          aErrorStack ) == STL_SUCCESS );
    
    stlMemcpy( (void*)&sLogFileHdr, sAlignedPtr, STL_SIZEOF( ztrLogFileHdr ) );
    
    stlFreeHeap( sLogFileHdrBuffer );
    
    sState = 0;
    STL_TRY( stlCloseFile( &sFile, 
                           aErrorStack ) == STL_SUCCESS );
    
    *aFileSeqNo = sLogFileHdr.mLogFileSeqNo;
    *aGroupId   = sLogFileHdr.mLogGroupId;
    
    return STL_SUCCESS;

    STL_FINISH;
    
    switch( sState )
    {
        case 1:
        (void) stlCloseFile( &sFile, 
                             aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztrsGetArchiveLogFile( stlChar        * aFileName,
                                 ztrLogFileInfo * aLogFileInfo,
                                 ztrRecoveryMgr * aRecoveryMgr,
                                 stlErrorStack  * aErrorStack )
{
    stlSnprintf( aFileName, 
                 STL_MAX_FILE_PATH_LENGTH, 
                 "%s"STL_PATH_SEPARATOR"%s_%ld.log", 
                 aRecoveryMgr->mArchivePath, aRecoveryMgr->mArchiveFilePrefix, aLogFileInfo->mFileSeqNo );
    
    return STL_SUCCESS;
}

stlStatus ztrsValidateArchiveLogFile( stlChar        * aFileName,
                                      ztrLogFileInfo * aLogFileInfo,
                                      stlErrorStack  * aErrorStack )
{
    stlFile        sFile;
    stlInt32       sState     = 0;
    stlInt32       sIdx       = 0;
    stlInt64       sFileSeqNo = -1;
    stlInt16       sGroupId   = -1;
    stlBool        sIsValid   = STL_FALSE;
    ztrLogFileHdr  sLogFileHdr;
    stlFileInfo    sFileInfo;
    
    STL_TRY( stlOpenFile( &sFile,
                          aFileName,
                          STL_FOPEN_READ,
                          STL_FPERM_OS_DEFAULT,
                          aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( stlReadFile( &sFile,
                          &sLogFileHdr,
                          STL_SIZEOF( ztrLogFileHdr ),
                          NULL,
                          aErrorStack ) == STL_SUCCESS );
    sState = 0;
    STL_TRY( stlCloseFile( &sFile, 
                           aErrorStack ) == STL_SUCCESS );
    
    /**
     * 최대 10초까지 기다려보고 ArchiveLog File이 정상적이지 않으면 Failure로 처리한다.
     */
    for( sIdx = 0; sIdx < 10; sIdx++ )
    {
        STL_TRY( stlGetFileStatByName( &sFileInfo,
                                       aFileName,
                                       STL_FINFO_MIN,
                                       aErrorStack ) == STL_SUCCESS );
        
        if( sLogFileHdr.mFileSize == sFileInfo.mSize )
        {
            sIsValid = STL_TRUE;
            break;
        }
                
        stlSleep( STL_SET_SEC_TIME( 1 ) );
    }
    
    STL_TRY_THROW( sIsValid == STL_TRUE, RAMP_ERR_ARCHIVELOG_READ_TIMEOUT );
    
    STL_TRY( ztrsGetLogFileSeqNoNGroupId( aFileName,
                                          &sFileSeqNo,
                                          &sGroupId,
                                          aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( aLogFileInfo->mFileSeqNo == sFileSeqNo, RAMP_ERR_INVALID_ARCHIVELOG_FILE );
    STL_TRY_THROW( aLogFileInfo->mGroupId   == sGroupId, RAMP_ERR_INVALID_ARCHIVELOG_FILE );
            
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_ARCHIVELOG_READ_TIMEOUT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_ARCHIVELOG_READ_TIMEOUT,
                      NULL,
                      aErrorStack,
                      aFileName );
    }
    STL_CATCH( RAMP_ERR_INVALID_ARCHIVELOG_FILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_INVALID_LOGFILE,
                      NULL,
                      aErrorStack,
                      aFileName );
    }
    STL_FINISH;
    
    switch( sState )
    {
        case 1:
        (void) stlCloseFile( &sFile, 
                             aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztrsReadLogFile( stlChar        * aFileName,
                           ztrLogFileInfo * aLogFileInfo,
                           ztrRecoveryMgr * aRecoveryMgr,
                           stlInt32       * aResult,
                           stlErrorStack  * aErrorStack )
{
    ztrLogCursor      sLogCursor;
    ztrLogBlockHdr    sLogBlockHdr;
    stlChar         * sLogBlock       = NULL;
    
    stlInt32          sBlockOffset    = 0;
    stlInt32          sState          = 0;
    stlBool           sEndOfFile      = STL_FALSE;
    stlInt32          sResult         = ZTR_RECOVERY_RESULT_MAX;
    
    STL_TRY( ztrsOpenLogCursor( &sLogCursor,
                                aLogFileInfo->mBlockSeqNo,
                                aFileName,
                                aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    while( 1 )
    {
        sBlockOffset = 0;
        
        STL_TRY( ztrsReadLogBlock( &sLogCursor,
                                   &sLogBlock,
                                   &sEndOfFile,
                                   aErrorStack ) == STL_SUCCESS );
        
        if( sEndOfFile == STL_TRUE )
        {
            sResult = ZTR_RECOVERY_RESULT_EOF;
            break;
        }
        
        ZTR_READ_MOVE_LOG_BLOCK_HDR( &sLogBlockHdr, sLogBlock, sBlockOffset );
        
        if( ZTR_IS_DUMMY_FILE_END_BLOCK( &sLogBlockHdr ) == STL_TRUE )
        {
            STL_TRY( ztrsWriteLogBlockToBuffer( sLogBlock, 
                                                aErrorStack ) == STL_SUCCESS );
            aLogFileInfo->mBlockSeqNo++;
            sResult = ZTR_RECOVERY_RESULT_EOF;
            break;
        }
        
        /**
         * Old(Garbage) Log를 만날 경우 경우에 여기서 빠져나간다.
         */
        switch( ztrsIsValidLog( &sLogCursor, &sLogBlockHdr ) )
        {
            case ZTR_LOG_STATUS_NONE:
            {
                STL_TRY( ztrsWriteLogBlockToBuffer( sLogBlock, 
                                                    aErrorStack ) == STL_SUCCESS );
                aLogFileInfo->mBlockSeqNo++;
                break;   
            }
            case ZTR_LOG_STATUS_INVALID_LOG:
            {
                sResult = ZTR_RECOVERY_RESULT_READ_DONE;
                STL_THROW( RAMP_SUCCESS );
                break;
            }
            case ZTR_LOG_STATUS_INVALID_WRAP_NO:
            {
                sResult = ZTR_RECOVERY_RESULT_INVALID_WRAP_NO;
                STL_THROW( RAMP_SUCCESS );
                break;
            }
            case ZTR_LOG_STATUS_MAX:
            default:
                break;
        }
        
    }// end of while( 1 )
    
    STL_RAMP( RAMP_SUCCESS );
    
    *aResult = sResult;
    
    sState = 0;
    STL_TRY( ztrsCloseLogCursor( &sLogCursor,
                                 aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    switch( sState )
    {
        case 1:
            (void) ztrsCloseLogCursor( &sLogCursor,
                                       aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztrsOpenLogCursor( ztrLogCursor  * aLogCursor,
                             stlInt64        aBlockSeq,
                             stlChar       * aFileName,
                             stlErrorStack * aErrorStack )
{
    stlUInt32        sState      = 0;
    void           * sBufferOrg  = NULL;
    void           * sBuffer     = NULL;
    stlOffset        sOffset     = 0;
    stlInt32         sBlockCount = 0;
    ztrLogFileHdr    sLogFileHdr;
    ztrLogBlockHdr   sLogBlockHdr;
    
    aLogCursor->mBuffer         = NULL;
    aLogCursor->mBufferOrg      = NULL;
    aLogCursor->mFileBlockSeq   = 0;
    aLogCursor->mBufferBlockSeq = -1;
    
    STL_TRY( stlOpenFile( &aLogCursor->mFile,
                          aFileName,
                          STL_FOPEN_READ | STL_FOPEN_DIRECT  | STL_FOPEN_LARGEFILE,
                          STL_FPERM_OS_DEFAULT,
                          aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    /**
     * LogFileHeader를 먼저 읽는다.
     */
    STL_TRY( stlAllocHeap( (void**)&sBufferOrg,
                           ZTR_DEFAULT_BLOCK_SIZE + ZTR_MEM_ALIGN,
                           aErrorStack ) == STL_SUCCESS );
    
    sBuffer = (void*)STL_ALIGN( (stlUInt64)sBufferOrg , ZTR_MEM_ALIGN );
    
    STL_TRY( stlReadFile( &aLogCursor->mFile,
                          sBuffer,
                          ZTR_DEFAULT_BLOCK_SIZE,
                          NULL,
                          aErrorStack ) == STL_SUCCESS );
    
    stlMemcpy( (void*)&sLogFileHdr, sBuffer, STL_SIZEOF( ztrLogFileHdr ) );
    
    stlFreeHeap( sBufferOrg );
    
    aLogCursor->mBlockSize     = sLogFileHdr.mBlockSize;
    aLogCursor->mMaxBlockCount = sLogFileHdr.mMaxBlockCount - 1;
    aLogCursor->mPrevLastLsn   = sLogFileHdr.mPrevLastLsn;
    
    STL_TRY( stlAllocHeap( (void**)&aLogCursor->mBufferOrg,
                           ZTR_DEFAULT_BLOCK_SIZE * ( ZTR_READ_BUFFER_BLOCK_COUNT + 1 ),
                           aErrorStack ) == STL_SUCCESS );
    sState = 2;
    
    aLogCursor->mBuffer = (stlChar*)STL_ALIGN( (stlUInt64)aLogCursor->mBufferOrg, ZTR_MEM_ALIGN );
    
    /**
     * WrapNo는 처음 위치한 BlockHeader에서 읽어야 한다.
     */
    sOffset = ZTR_BLOCK_SEQ_FILE_OFFSET( 0, ZTR_DEFAULT_BLOCK_SIZE );
    
    STL_TRY( stlSeekFile( &aLogCursor->mFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          aErrorStack ) == STL_SUCCESS );

    STL_TRY( stlReadFile( &aLogCursor->mFile,
                          (void*)aLogCursor->mBuffer,
                          ZTR_DEFAULT_BLOCK_SIZE,
                          NULL,
                          aErrorStack ) == STL_SUCCESS );
    
    ZTR_READ_LOG_BLOCK_HDR( &sLogBlockHdr, aLogCursor->mBuffer );
    
    aLogCursor->mWrapNo = ZTR_BLOCK_WRAP_NO( &sLogBlockHdr );
    
    /**
     * Log를 읽을 위치로 Jump!!!
     */
    sBlockCount = STL_MIN( aLogCursor->mMaxBlockCount - aBlockSeq,
                           ZTR_READ_BUFFER_BLOCK_COUNT );
    
    sOffset = ZTR_BLOCK_SEQ_FILE_OFFSET( aBlockSeq, ZTR_DEFAULT_BLOCK_SIZE );
    
    STL_TRY( stlSeekFile( &aLogCursor->mFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          aErrorStack ) == STL_SUCCESS );

    STL_TRY( stlReadFile( &aLogCursor->mFile,
                          (void*)aLogCursor->mBuffer,
                          ZTR_DEFAULT_BLOCK_SIZE * sBlockCount,
                          NULL,
                          aErrorStack ) == STL_SUCCESS );
    
    aLogCursor->mFileBlockSeq     = sBlockCount + aBlockSeq;
    aLogCursor->mBufferBlockCount = sBlockCount;

    return STL_SUCCESS;
    
    STL_FINISH;
    
    switch( sState )
    {
        case 2:
            stlFreeHeap( aLogCursor->mBufferOrg );
        case 1:
            (void) stlCloseFile( &aLogCursor->mFile,
                                 aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztrsCloseLogCursor( ztrLogCursor  * aLogCursor,
                              stlErrorStack * aErrorStack )
{
    if( aLogCursor->mBufferOrg != NULL )
    {
        stlFreeHeap( aLogCursor->mBufferOrg );
    }

    STL_TRY( stlCloseFile( &aLogCursor->mFile,
                           aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



stlStatus ztrsReadLogBlock( ztrLogCursor  * aLogCursor,
                            stlChar      ** aLogBlock,
                            stlBool       * aEndOfFile,
                            stlErrorStack * aErrorStack )
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
        
        sOffset = ZTR_BLOCK_SEQ_FILE_OFFSET( aLogCursor->mFileBlockSeq,
                                             aLogCursor->mBlockSize );
        
        STL_TRY( stlSeekFile( &aLogCursor->mFile,
                              STL_FSEEK_SET,
                              &sOffset,
                              aErrorStack ) == STL_SUCCESS );
        
        sBlockCount = STL_MIN( aLogCursor->mMaxBlockCount - aLogCursor->mFileBlockSeq,
                               ZTR_READ_BUFFER_BLOCK_COUNT );

        STL_ASSERT( sBlockCount > 0 );
        
        STL_TRY( stlReadFile( &aLogCursor->mFile,
                              (void*)aLogCursor->mBuffer,
                              ZTR_DEFAULT_BLOCK_SIZE * sBlockCount,
                              NULL,
                              aErrorStack ) == STL_SUCCESS );
        
        aLogCursor->mFileBlockSeq    += sBlockCount;
        aLogCursor->mBufferBlockCount = sBlockCount;
        aLogCursor->mBufferBlockSeq   = 0;
    }
    
    *aLogBlock = (aLogCursor->mBuffer +
                 (aLogCursor->mBufferBlockSeq * aLogCursor->mBlockSize));

    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlInt16 ztrsIsValidLog( ztrLogCursor    * aLogCursor,
                         ztrLogBlockHdr  * aLogBlockHdr )
{
    stlInt16 sLogStatus = ZTR_LOG_STATUS_NONE;
    
    if( aLogBlockHdr->mLsn < 0 )
    {
        /**
         * 초기화 되어 있는 LogBlock
         */
        sLogStatus = ZTR_LOG_STATUS_INVALID_LOG;
        STL_THROW( RAMP_RETURN );
    }
    
    if( aLogBlockHdr->mLsn < aLogCursor->mPrevLastLsn )
    {
        /**
         * LogFile이 재사용되었지만 기존의 데이터가 아직 남아있는 경우
         */
        sLogStatus = ZTR_LOG_STATUS_INVALID_LOG;
        STL_THROW( RAMP_RETURN );
    }

    if( ZTR_BLOCK_WRAP_NO( aLogBlockHdr ) != (aLogCursor->mWrapNo) )
    {
        /**
         * LogFile이 재사용되어 해당 LogFile을 더이상 읽을 수 없는 경우 
         * Archive에서 읽어야 함
         */
        sLogStatus = ZTR_LOG_STATUS_INVALID_WRAP_NO;
        STL_THROW( RAMP_RETURN );
    }

    STL_RAMP( RAMP_RETURN );
    
    return sLogStatus;
}


stlStatus ztrsWriteLogBlockToBuffer( stlChar       * aLogBlock,
                                     stlErrorStack * aErrorStack )
{
    ztrRecoveryMgr * sRecoveryMgr = &(gMasterMgr.mRecoveryMgr);
    
    /**
     * Header 정보 세팅
     */
    if( sRecoveryMgr->mWriteIdx + ZTR_DEFAULT_BLOCK_SIZE > sRecoveryMgr->mBufferSize )
    {
        STL_TRY( ztrsRecoveryProtocol( STL_FALSE,
                                       aErrorStack ) == STL_SUCCESS );
    }
    
    stlMemcpy( &sRecoveryMgr->mSendBuff[ sRecoveryMgr->mWriteIdx ],
               aLogBlock,
               ZTR_DEFAULT_BLOCK_SIZE );
    
    sRecoveryMgr->mWriteIdx += ZTR_DEFAULT_BLOCK_SIZE;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztrsGetLogFileHdr( stlChar       * aFileName,
                             ztrLogFileHdr * aLogFileHdr,
                             stlErrorStack * aErrorStack )
{
    stlFile          sFile;
    stlUInt32        sState      = 0;
    void           * sBufferOrg  = NULL;
    void           * sBuffer     = NULL;
    
    STL_TRY( stlOpenFile( &sFile,
                          aFileName,
                          STL_FOPEN_READ | STL_FOPEN_DIRECT  | STL_FOPEN_LARGEFILE,
                          STL_FPERM_OS_DEFAULT,
                          aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( stlAllocHeap( (void**)&sBufferOrg,
                           ZTR_DEFAULT_BLOCK_SIZE + ZTR_MEM_ALIGN,
                           aErrorStack ) == STL_SUCCESS );
    sState = 2;
    
    sBuffer = (void*)STL_ALIGN( (stlUInt64)sBufferOrg , ZTR_MEM_ALIGN );
    
    STL_TRY( stlReadFile( &sFile,
                          sBuffer,
                          ZTR_DEFAULT_BLOCK_SIZE,
                          NULL,
                          aErrorStack ) == STL_SUCCESS );
    
    stlMemcpy( (void*)aLogFileHdr, sBuffer, STL_SIZEOF( ztrLogFileHdr ) );
    
    sState = 1;
    stlFreeHeap( sBufferOrg );
    
    sState = 0;
    STL_TRY( stlCloseFile( &sFile,
                           aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;
    
    switch( sState )
    {
        case 2:
            stlFreeHeap( sBufferOrg );
        case 1:
            (void) stlCloseFile( &sFile,
                                 aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztrsControlLogFlush( stlBool         aIsLock,
                               stlErrorStack * aErrorStack )
{
    stlInt32    sState         = 0;
    stlChar     sSqlString[ZTR_COMMAND_BUFFER_SIZE] = {0,};
    
    SQLHSTMT    sStmtHandle;
    SQLRETURN   sResult;
    
    ztrRecoveryMgr * sRecoveryMgr = &(gMasterMgr.mRecoveryMgr);
    
    if( aIsLock == STL_TRUE )
    {
        /**
         * Log Flusher를 잠근다.
         */
        stlSnprintf( sSqlString,
                     ZTR_COMMAND_BUFFER_SIZE,
                     "ALTER SYSTEM STOP LOG FLUSHING" );
    }
    else
    {
        /**
         * Log Flusher를 푼다.
         */
        stlSnprintf( sSqlString,
                     ZTR_COMMAND_BUFFER_SIZE,
                     "ALTER SYSTEM START LOG FLUSHING" );
    }
    
    STL_TRY( ztrmSQLAllocStmt( sRecoveryMgr->mDbcHandle,
                               &sStmtHandle,
                               aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztrmSQLExecDirect( sStmtHandle, 
                                (SQLCHAR*)sSqlString,
                                stlStrlen( sSqlString ),
                                &sResult, 
                                aErrorStack ) == STL_SUCCESS );
    
    if( aIsLock == STL_TRUE )
    {
        STL_TRY( ztrmLogMsg( aErrorStack,
                             "[SENDER-RECOVERY] STOP FLUSH LOGS.\n" ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( ztrmLogMsg( aErrorStack,
                             "[SENDER-RECOVERY] RESUME FLUSH LOGS.\n" ) == STL_SUCCESS );
    }
    
    sState = 0;
    STL_TRY( ztrmSQLFreeStmt( sStmtHandle, 
                              aErrorStack ) == STL_SUCCESS ); 
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    if( sState > 0 )
    {
        (void) ztrmSQLFreeStmt( sStmtHandle, 
                                aErrorStack );
    }
    
    return STL_FAILURE;
}


/** @} */

