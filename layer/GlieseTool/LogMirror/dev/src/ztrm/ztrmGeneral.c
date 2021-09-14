/*******************************************************************************
 * ztrmGeneral.c
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
 * @file ztrmGeneral.c
 * @brief GlieseTool LogMirror General Routines
 */

#include <goldilocks.h>
#include <ztr.h>

ztrConfigure        gZtrConfigure;
ztrMasterMgr        gMasterMgr;
ztrSlaveMgr         gSlaveMgr;

extern stlBool      gRunState;

ztrCommunicateCallback gCallbackForNormal =
{
    ztrsCreateListener,
    ztrrCreateReceiver,
    ztrmDestroyHandle,
    ztrmSendHdrNData,
    ztrmRecvHdrNData,
    ztrmRecvHdrNDataWithPoll,
    ztrsCheckSlaveState
};

ztrCommunicateCallback gCallbackForInifiniband =
{
    ztrsCreateListener4IB,
    ztrrCreateReceiver4IB,
    ztrmDestroyHandle4IB,
    ztrmSendHdrNData4IB,
    ztrmRecvHdrNData4IB,
    ztrmRecvHdrNDataWithPoll4IB,
    ztrsCheckSlaveState4IB
};

void ztrmInitSpinLock( ztrSpinLock * aSpinLock )
{
    ZTR_INIT_SPIN_LOCK( *aSpinLock );
}

void ztrmFinSpinLock( ztrSpinLock * aSpinLock )
{
    STL_ASSERT( *aSpinLock == ZTR_SPINLOCK_STATUS_INIT );
}

void ztrmAcquireSpinLock( ztrSpinLock * aSpinLock )
{
    stlUInt32  sRetVal;
    stlUInt32  sMaxRetryCnt = 1000;
    stlUInt32  sSpinCnt     = 0;

    sSpinCnt = sMaxRetryCnt;
    do
    {
        sRetVal = stlAtomicCas32( aSpinLock,
                                  ZTR_SPINLOCK_STATUS_LOCKED,
                                  ZTR_SPINLOCK_STATUS_INIT );

        if( sRetVal == ZTR_SPINLOCK_STATUS_INIT )
        {
            break;
        }
        
        sSpinCnt--;
        
        if( sSpinCnt == 0 )
        {
            stlBusyWait();
            sSpinCnt = sMaxRetryCnt;
        }
    } while( STL_TRUE );
}


void ztrmTrySpinLock( ztrSpinLock * aSpinLock,
                      stlBool     * aIsSuccess )
{
    stlUInt32  sRetVal;

    *aIsSuccess = STL_TRUE;

    sRetVal = stlAtomicCas32( aSpinLock,
                              ZTR_SPINLOCK_STATUS_LOCKED,
                              ZTR_SPINLOCK_STATUS_INIT );

    if(sRetVal != ZTR_SPINLOCK_STATUS_INIT)
    {
        *aIsSuccess = STL_FALSE;
    }
}


void ztrmReleaseSpinLock( ztrSpinLock * aSpinLock )
{
    stlUInt32 sRetVal;

    sRetVal = stlAtomicCas32( aSpinLock,
                              ZTR_SPINLOCK_STATUS_INIT,
                              ZTR_SPINLOCK_STATUS_LOCKED );

    STL_ASSERT( sRetVal == ZTR_SPINLOCK_STATUS_LOCKED );
}


void ztrmPrintErrorstack( stlErrorStack * aErrorStack )
{
    stlInt32 i;
    stlInt32 sErrorCode;
    stlInt32 sExtErrCode;
    stlChar  sSqlState[6];

    for( i = aErrorStack->mTop; i >= 0; i-- )
    {
        sErrorCode = aErrorStack->mErrorData[i].mErrorCode;
        sExtErrCode = stlGetExternalErrorCode( &aErrorStack->mErrorData[i] );
        stlMakeSqlState( sExtErrCode, sSqlState );
 
        stlPrintf( "ERR-%s(%d): %s %s\n",
                   sSqlState,
                   sErrorCode,
                   aErrorStack->mErrorData[i].mErrorMessage,
                   aErrorStack->mErrorData[i].mDetailErrorMessage );
        
    }
}


stlStatus ztrmLogMsg( stlErrorStack  * aErrorStack,
                      const stlChar  * aFormat,
                      ... )
{
    va_list          sVarArgList;
    stlInt32         sLogSize;
    stlChar        * sLogMsg = NULL;
    stlInt32         sState  = 0;
    
    va_start(sVarArgList, aFormat);
    sLogSize = stlGetVprintfBufferSize( aFormat, sVarArgList );
    va_end(sVarArgList);
    
    STL_TRY( stlAllocHeap( (void**)&sLogMsg,
                           sLogSize + 1,
                           aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    va_start(sVarArgList, aFormat);
    stlVsnprintf( sLogMsg,
                  sLogSize + 1,
                  aFormat,
                  sVarArgList );
    va_end(sVarArgList);
    
    STL_TRY( stlAcquireSemaphore( &(gZtrConfigure.mLogSem),
                                  aErrorStack ) == STL_SUCCESS );
    sState = 2;
    
    STL_TRY( ztrmAddLogMsg( sLogMsg,
                            sLogSize,
                            aErrorStack ) == STL_SUCCESS );
    
    sState = 1;
    STL_TRY( stlReleaseSemaphore( &(gZtrConfigure.mLogSem),
                                  aErrorStack ) == STL_SUCCESS );
    
    sState = 0;
    stlFreeHeap( sLogMsg );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)stlReleaseSemaphore( &(gZtrConfigure.mLogSem),
                                       aErrorStack ) ;
        case 1:
            (void)stlFreeHeap( sLogMsg );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztrmAddLogMsg( stlChar        * aMsg,
                         stlInt32         aLength,
                         stlErrorStack  * aErrorStack )
{
    stlFile   sFile;
    stlChar   sFileName[STL_MAX_FILE_PATH_LENGTH];
    stlSize   sWrittenBytes;
    stlInt32  sState = 0;
    
    if( gZtrConfigure.mIsMaster == STL_TRUE )
    {
        stlSnprintf( sFileName,
                     STL_MAX_FILE_PATH_LENGTH,
                     "%strc"STL_PATH_SEPARATOR"LogMirror_master.trc",
                     gZtrConfigure.mHomeDir );
    }
    else
    {
        stlSnprintf( sFileName,
                     STL_MAX_FILE_PATH_LENGTH,
                     "%strc"STL_PATH_SEPARATOR"LogMirror_slave.trc",
                     gZtrConfigure.mHomeDir );
    }
    
    STL_TRY( stlOpenFile( &sFile,
                          sFileName,
                          STL_FOPEN_CREATE |
                          STL_FOPEN_WRITE  |
                          STL_FOPEN_APPEND,
                          STL_FPERM_OS_DEFAULT,
                          aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztrmWriteTimeStamp( &sFile,
                                 aErrorStack ) == STL_SUCCESS );

    STL_TRY( stlWriteFile( &sFile,
                           (void*)aMsg,
                           aLength,
                           &sWrittenBytes,
                           aErrorStack ) == STL_SUCCESS );
    
    sState = 2;
    STL_TRY( stlCloseFile( &sFile,
                           aErrorStack ) == STL_SUCCESS );

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

void ztrmWriteLastError( stlErrorStack * aErrorStack )
{
    stlInt32 sCount = aErrorStack->mTop;
    stlInt32 i = 0;
    stlInt32 sErrorCode;
    stlInt32 sExtErrCode;
    stlChar  sSqlState[6];

    for( i = sCount;i >= 0; i-- )
    {
        sErrorCode = aErrorStack->mErrorData[i].mErrorCode;
        sExtErrCode = stlGetExternalErrorCode( &aErrorStack->mErrorData[i] );
        stlMakeSqlState( sExtErrCode, sSqlState );
        
        (void) ztrmLogMsg( aErrorStack,
                           "ERR-%s(%d): %s %s\n",
                           sSqlState,
                           sErrorCode,
                           aErrorStack->mErrorData[i].mErrorMessage,
                           aErrorStack->mErrorData[i].mDetailErrorMessage );
    }
}

stlStatus ztrmWriteTimeStamp( stlFile        * aFile,
                              stlErrorStack  * aErrorStack )
{
    stlExpTime sExpTime;
    stlChar    sBuf[1024];
    
    stlMakeExpTimeByLocalTz(&sExpTime, stlNow() );
    
    stlSnprintf( sBuf,
                 1024,
                 "\n[%04d-%02d-%02d %02d:%02d:%02d.%06d]\n",
                 sExpTime.mYear + 1900,
                 sExpTime.mMonth + 1,
                 sExpTime.mDay,
                 sExpTime.mHour,
                 sExpTime.mMinute,
                 sExpTime.mSecond,
                 sExpTime.mMicroSecond );
    
    STL_TRY( stlPutStringFile( sBuf,
                               aFile,
                               aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztrmProcessAliveCheck( stlChar       * aFileName,
                                 stlErrorStack * aErrorStack )
{
    stlFile    sFile;
    stlOffset  sFileSize  = 0;
    stlOffset  sOffset    = 0;
    stlInt32   sState     = 0;
    
    stlChar    sLineBuff[STL_MAX_SQL_IDENTIFIER_LENGTH * 2] = {0,};
    stlChar  * sEndPtr   = NULL;
    stlProc    sCheckProc;
    stlInt64   sCheckProcId;
    
    STL_TRY( stlOpenFile( &sFile,
                          aFileName,
                          STL_FOPEN_READ | STL_FOPEN_WRITE | STL_FOPEN_BINARY,
                          STL_FPERM_OS_DEFAULT,
                          aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( stlLockFile( &sFile, 
                          STL_FLOCK_EXCLUSIVE,
                          aErrorStack ) == STL_SUCCESS );
    sState = 2;
    
    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_END,
                          &sFileSize,
                          aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( sFileSize > 0, RAMP_SUCCESS );
    
    sOffset = 0;
    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          aErrorStack ) == STL_SUCCESS );
    
    if( stlGetStringFile( sLineBuff,
                          STL_SIZEOF( sLineBuff ),
                          &sFile,
                          aErrorStack ) != STL_SUCCESS )
    {
        STL_TRY( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_EOF );
        (void)stlPopError( aErrorStack );
    }
    
    STL_TRY( stlStrToInt64( sLineBuff,
                            STL_NTS,
                            &sEndPtr,
                            10,
                            &sCheckProcId, 
                            aErrorStack ) == STL_SUCCESS );
    
    sCheckProc.mProcID = sCheckProcId;
    
    if( (*sEndPtr != '\0') && (*sEndPtr != '\n') )
    {
        STL_TRY( stlTruncateFile( &sFile,
                                  0,
                                  aErrorStack ) == STL_SUCCESS );
        
        STL_THROW( RAMP_SUCCESS );
    }
    
    /**
     * PROCESS가 살아있는지 확인
     */
    STL_TRY_THROW( stlKillProc( &sCheckProc,
                                0,           //NULL Signal, Error Check
                                aErrorStack ) != STL_SUCCESS, RAMP_ERR_ALREADY_RUNNING );

    /**
     * PID관련 파일은 있지만 Process가 존재하지 않을 경우에...
     */
    (void)stlPopError( aErrorStack );
    
    STL_TRY( stlTruncateFile( &sFile,
                              0,
                              aErrorStack ) == STL_SUCCESS );
    
    STL_RAMP( RAMP_SUCCESS );
    
    sState = 1;
    STL_TRY( stlUnlockFile( &sFile, 
                            aErrorStack ) == STL_SUCCESS );
    sState = 0;
    STL_TRY( stlCloseFile( &sFile,
                           aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_ALREADY_RUNNING )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_ALREADY_RUNNING,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    switch( sState )
    {
        case 2:
            (void) stlUnlockFile( &sFile, 
                                  aErrorStack );
        case 1:
            (void) stlCloseFile( &sFile,
                                 aErrorStack );
        default:
            break; 
    }
    
    return STL_FAILURE;
}


stlStatus ztrmStart( stlBool         aIsMaster,
                     stlBool         aDoReset,
                     stlErrorStack * aErrorStack )
{    
    stlProc   sProc;
    stlBool   sIsChild = STL_FALSE;
    stlFile   sFile;
    stlChar   sFileName[STL_MAX_FILE_PATH_LENGTH];
    stlChar   sWriteBuff[STL_MAX_SQL_IDENTIFIER_LENGTH];
    
    stlBool   sExist = STL_FALSE;
    stlInt32  sState = 0;
    stlSize   sWriteSize = 0;

    gZtrConfigure.mIsMaster = aIsMaster;
    
    /**
     * Configure File 읽기
     */
    STL_TRY( ztrcReadConfigure( aErrorStack ) == STL_SUCCESS );
    
    if( aIsMaster == STL_TRUE )
    {
        STL_TRY( ztrmSetLogMirrorShmKeyFromDB( aErrorStack ) == STL_SUCCESS );
    }
    
    STL_TRY( stlForkProc( &sProc,
                          &sIsChild,
                          aErrorStack ) == STL_SUCCESS );
    
    if( sIsChild == STL_FALSE )
    {
        /**
         * PId 관련 처리해야 함
         */
    }
    else
    {
        STL_TRY( stlGetCurrentProc( &sProc, aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztrmGetPidFileName( sFileName, 
                                     aIsMaster,
                                     aErrorStack ) == STL_SUCCESS );
    
        STL_TRY( stlExistFile( sFileName,
                               &sExist,
                               aErrorStack ) == STL_SUCCESS );

        if ( sExist == STL_TRUE )
        {
            STL_TRY( ztrmProcessAliveCheck( sFileName, aErrorStack ) == STL_SUCCESS );
        }
        
        /**
         * AIX에서는 fork 후에 Semaphore를 child가 정상적으로 인식하지 못한다.
         * 따라서, 반드시 fork 이후에 Semaphore를 생성해야 한다!!!!!
         */
        STL_TRY( stlCreateSemaphore( &(gZtrConfigure.mLogSem),
                                     "LogSem",
                                     1,
                                     aErrorStack ) == STL_SUCCESS );
        sState = 1;
        
        STL_TRY( stlOpenFile( &sFile,
                              sFileName,
                              STL_FOPEN_CREATE | STL_FOPEN_WRITE | STL_FOPEN_BINARY,
                              STL_FPERM_OS_DEFAULT,
                              aErrorStack ) == STL_SUCCESS );
        sState = 2;
        
        stlSnprintf( sWriteBuff,
                     STL_SIZEOF( sWriteBuff ),
                     "%d\n",
                     sProc.mProcID );
        
        STL_TRY( stlLockFile( &sFile, 
                              STL_FLOCK_EXCLUSIVE,
                              aErrorStack ) == STL_SUCCESS );
        sState = 3;
        
        STL_TRY( stlWriteFile( &sFile,
                               sWriteBuff,
                               stlStrlen( sWriteBuff ),
                               &sWriteSize,
                               aErrorStack ) == STL_SUCCESS );
        
        sState = 2;
        STL_TRY( stlUnlockFile( &sFile, 
                                aErrorStack ) == STL_SUCCESS );

        sState = 1;
        STL_TRY( stlCloseFile( &sFile,
                               aErrorStack ) == STL_SUCCESS );
        
        ///////// DO SOMETHING  /////////////////////
        
        gRunState = STL_TRUE;
  
        /**
         * Thread 생성 및 Wait
         */
        
        STL_TRY( ztrmCreateThreads( aErrorStack ) == STL_SUCCESS );

        STL_TRY( ztrmLogMsg( aErrorStack,
                             "STARTUP Done.\n" ) == STL_SUCCESS );

        if( gZtrConfigure.mIsSilent == STL_FALSE )
        {
            if( gZtrConfigure.mIsMaster == STL_TRUE )
            {
                stlPrintf( "LogMirror Startup done as Master.\n" );
            }
            else
            {
                stlPrintf( "LogMirror Startup done as Slave.\n" );
            }
        }

        STL_TRY( ztrmJoinThreads( aIsMaster, aErrorStack ) == STL_SUCCESS );

        STL_TRY( ztrmLogMsg( aErrorStack,
                             "STOP Done.\n" ) == STL_SUCCESS );
        
        
        /////////////////////////////////////////////////
        
        sState = 0;
        STL_TRY( stlDestroySemaphore( &(gZtrConfigure.mLogSem),
                                      aErrorStack ) == STL_SUCCESS );
        
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    switch( sState )
    {
        case 3:
            (void) stlUnlockFile( &sFile, 
                                  aErrorStack );
        case 2:
            (void) stlCloseFile( &sFile,
                                 aErrorStack );
        case 1:
            (void) stlDestroySemaphore( &(gZtrConfigure.mLogSem),
                                        aErrorStack );
        default:
            break;   
    }
    
    return STL_FAILURE;
}
                    
stlStatus ztrmStop( stlBool         aIsMaster,
                    stlErrorStack * aErrorStack )
{
    stlProc    sProc;
    stlInt64   sProcId;
    stlFile    sFile;
    stlChar    sFileName[STL_MAX_FILE_PATH_LENGTH];
    stlChar  * sReadBuff  = NULL;
    stlChar  * sEndPtr    = NULL;
    stlBool    sExist     = STL_FALSE;
    stlOffset  sOffset    = 0;
    stlInt32   sState     = 0;
    stlOffset  sFileSize  = 0;
    
    STL_TRY( ztrmGetPidFileName( sFileName, 
                                 aIsMaster,
                                 aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlExistFile( sFileName,
                           &sExist,
                           aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( sExist == STL_TRUE, RAMP_ERR_NOT_FILE_EXIST );
    
    STL_TRY( stlOpenFile( &sFile,
                          sFileName,
                          STL_FOPEN_READ | STL_FOPEN_WRITE | STL_FOPEN_BINARY,
                          STL_FPERM_OS_DEFAULT,
                          aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( stlLockFile( &sFile, 
                          STL_FLOCK_EXCLUSIVE,
                          aErrorStack ) == STL_SUCCESS );
    sState = 2;
    
    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_END,
                          &sFileSize,
                          aErrorStack ) == STL_SUCCESS );
    
    sOffset = 0;
    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlAllocHeap( (void**)&sReadBuff,
                           sFileSize,
                           aErrorStack ) == STL_SUCCESS );
    sState = 3;
            
    stlMemset( sReadBuff, 0x00, sFileSize );

    STL_TRY( stlReadFile( &sFile,
                          sReadBuff,
                          sFileSize,
                          NULL,
                          aErrorStack ) == STL_SUCCESS );
        
    STL_TRY( stlStrToInt64( sReadBuff,
                            STL_NTS,
                            &sEndPtr,
                            10,
                            &sProcId, 
                            aErrorStack ) == STL_SUCCESS );
    
    sProc.mProcID = sProcId;

    if( stlKillProc( &sProc,
                     ZTR_SIGNAL_DEFINE,
                     aErrorStack ) != STL_SUCCESS ) 
    {
        //Nothing To Do.
    }
            
    sState = 2;
    stlFreeHeap( sReadBuff );
    
    sState = 1;
    STL_TRY( stlUnlockFile( &sFile, 
                            aErrorStack ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( stlCloseFile( &sFile,
                           aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlRemoveFile( sFileName, 
                            aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_NOT_FILE_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_NOT_RUNNING,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    switch( sState )
    {
        case 3:
            stlFreeHeap( sReadBuff );
        case 2:
            (void) stlUnlockFile( &sFile, 
                                  aErrorStack );
        case 1:
            (void)stlCloseFile( &sFile,
                                aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztrmSetHomeDir( stlErrorStack * aErrorStack )
{
    stlChar   sPath[STL_MAX_FILE_PATH_LENGTH] = {0,};
    stlBool   sFound = STL_FALSE;
        
    STL_TRY( stlGetEnv( sPath, 
                        STL_SIZEOF(sPath),
                        STL_ENV_DB_DATA,
                        &sFound,
                        aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( sFound == STL_TRUE, RAMP_ERR_INVALID_HOMEDIR );
    
    /**
     * GOLDILOCKS_DATA 끝에 '/'를 제거한다.
     */
    if ( sPath[ stlStrlen( sPath ) - 1 ] == '/' ||
         sPath[ stlStrlen( sPath ) - 1 ] == '\\' )
    {
        sPath[ stlStrlen( sPath ) ] = '\0';
    }
    
    stlSnprintf( gZtrConfigure.mHomeDir, 
                 STL_MAX_FILE_PATH_LENGTH, 
                 "%s"STL_PATH_SEPARATOR, 
                 sPath );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_HOMEDIR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_INVALID_HOME,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztrmCreateThreads( stlErrorStack * aErrorStack )
{
    stlInt32 sState = 0;
    
    if( gZtrConfigure.mIsMaster == STL_TRUE )
    {
        STL_TRY( ztrmInitializeEnv4Master( aErrorStack ) == STL_SUCCESS );
        sState = 1;
        
        STL_TRY( ztrtCreateSenderThread( aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( ztrmInitializeEnv4Slave( aErrorStack ) == STL_SUCCESS );
        sState = 1;
        
        /**
         * Receiver Thread 생성
         */
        STL_TRY( ztrtCreateReceiverThread( aErrorStack ) == STL_SUCCESS );
        
        /**
         * Applier Thread 생성
         */
        STL_TRY( ztrtCreateFlusherThread( aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    if( sState == 1 )
    {
        if( gZtrConfigure.mIsMaster == STL_TRUE )
        {
            (void) ztrmFinalizeEnv4Master( aErrorStack );
        }
        else
        {
            (void) ztrmFinalizeEnv4Slave( aErrorStack );
        }
    }
    
    return STL_FAILURE;
}


stlStatus ztrmJoinThreads( stlBool          aIsMaster,
                           stlErrorStack  * aErrorStack )
{
    if( aIsMaster == STL_TRUE )
    {
        STL_TRY( ztrtJoinSenderThread( aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztrmFinalizeEnv4Master( aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( ztrtJoinReceiverThread( aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztrtJoinFlusherThread( aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztrmFinalizeEnv4Slave( aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;   
}

stlStatus ztrmInitializeEnv4Master( stlErrorStack  * aErrorStack )
{
    stlInt32    sState    = 0;
    void      * sBaseAddr = NULL;
    
    STL_TRY( stlSetSignalHandler( ZTR_SIGNAL_DEFINE,
                                  ztrmMasterSignalFunc,
                                  &gZtrConfigure.mOldSignalFunc,
                                  aErrorStack ) == STL_SUCCESS );
    
    /**
     * 기타 Sginal을 무시한다.
     */
    STL_TRY( stlSetSignalHandler( STL_SIGNAL_HUP,
                                  STL_SIGNAL_IGNORE,
                                  NULL,
                                  aErrorStack ) == STL_SUCCESS );

    STL_TRY( stlSetSignalHandler( STL_SIGNAL_TERM,
                                  STL_SIGNAL_IGNORE,
                                  NULL,
                                  aErrorStack ) == STL_SUCCESS );

    STL_TRY( stlSetSignalHandler( STL_SIGNAL_QUIT,
                                  STL_SIGNAL_IGNORE,
                                  NULL, 
                                  aErrorStack ) == STL_SUCCESS );

    STL_TRY( stlSetSignalHandler( STL_SIGNAL_INT,
                                  STL_SIGNAL_IGNORE,
                                  NULL,
                                  aErrorStack )== STL_SUCCESS );

    STL_TRY( stlSetSignalHandler( STL_SIGNAL_CLD,
                                  STL_SIGNAL_IGNORE,
                                  NULL,
                                  aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( stlAttachShm( &(gMasterMgr.mSharedMem),
                           ZTR_LOGMIRROR_DEFAULT_SHM_NAME,
                           (stlInt32)gZtrConfigure.mShmKey,
                           NULL,
                           aErrorStack ) == STL_SUCCESS );
    sState = 2;
    
    sBaseAddr = stlGetShmBaseAddr( &(gMasterMgr.mSharedMem) );
    
    gMasterMgr.mLmInfo  = sBaseAddr;
    gMasterMgr.mLmHdr   = sBaseAddr + ZTR_LOGMIRROR_BUFF_OFFSET;
    gMasterMgr.mLogData = sBaseAddr + ZTR_LOGMIRROR_BUFF_OFFSET + STL_SIZEOF( ztrLogMirrorHdr );
    
    STL_TRY( stlCreateRegionAllocator( &(gMasterMgr.mRegionMem),
                                       ZTR_REGION_INIT_SIZE,
                                       ZTR_REGION_NEXT_SIZE,
                                       aErrorStack ) == STL_SUCCESS );
    sState = 3;
    
    /**
     * Send/Rcv Buffer를 Initialize 한다.
     */
    STL_TRY( stlAllocRegionMem( &(gMasterMgr.mRegionMem),
                                ZTR_DEFAULT_COMM_BUFFER_SIZE,
                                (void**)&gMasterMgr.mSendBuff,
                                aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlAllocRegionMem( &(gMasterMgr.mRegionMem),
                                ZTR_DEFAULT_COMM_BUFFER_SIZE,
                                (void**)&gMasterMgr.mRecvBuff,
                                aErrorStack ) == STL_SUCCESS );
    
    /**
     * NETWORK INTERFACE 설정
     */
    if( ztrmGetIBMode() == STL_FALSE )
    {
        ztrmSetCommunicateCallback( &gCallbackForNormal );
    }
    else
    {
        ztrmSetCommunicateCallback( &gCallbackForInifiniband );
    }
    
    return STL_SUCCESS;   
    
    STL_FINISH;
    
    switch( sState )
    {
        case 3:
            (void) stlDestroyRegionAllocator( &(gMasterMgr.mRegionMem),
                                              aErrorStack );
        case 2:
            (void) stlDetachShm( &(gMasterMgr.mSharedMem),
                                 aErrorStack );
        case 1:
            (void) stlSetSignalHandler( ZTR_SIGNAL_DEFINE,
                                        gZtrConfigure.mOldSignalFunc,
                                        NULL,
                                        aErrorStack );
        default:
            break;   
    }
    
    return STL_FAILURE;
}


stlStatus ztrmFinalizeEnv4Master( stlErrorStack  * aErrorStack )
{
    STL_TRY( stlDestroyRegionAllocator( &(gMasterMgr.mRegionMem),
                                        aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlDetachShm( &(gMasterMgr.mSharedMem),
                           aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlSetSignalHandler( ZTR_SIGNAL_DEFINE,
                                  gZtrConfigure.mOldSignalFunc,
                                  NULL,
                                  aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztrmInitializeEnv4Slave( stlErrorStack  * aErrorStack )
{
    stlInt32       sState     = 0;
    stlInt32       sIdx       = 0;
    ztrChunkItem * sChunkItem = NULL;
    
    STL_TRY( stlSetSignalHandler( ZTR_SIGNAL_DEFINE,
                                  ztrmSlaveSignalFunc,
                                  &gZtrConfigure.mOldSignalFunc,
                                  aErrorStack ) == STL_SUCCESS );
    
    /**
     * 기타 Sginal을 무시한다.
     */
    STL_TRY( stlSetSignalHandler( STL_SIGNAL_HUP,
                                  STL_SIGNAL_IGNORE,
                                  NULL,
                                  aErrorStack ) == STL_SUCCESS );

    STL_TRY( stlSetSignalHandler( STL_SIGNAL_TERM,
                                  STL_SIGNAL_IGNORE,
                                  NULL,
                                  aErrorStack ) == STL_SUCCESS );

    STL_TRY( stlSetSignalHandler( STL_SIGNAL_QUIT,
                                  STL_SIGNAL_IGNORE,
                                  NULL, 
                                  aErrorStack ) == STL_SUCCESS );

    STL_TRY( stlSetSignalHandler( STL_SIGNAL_INT,
                                  STL_SIGNAL_IGNORE,
                                  NULL,
                                  aErrorStack )== STL_SUCCESS );

    STL_TRY( stlSetSignalHandler( STL_SIGNAL_CLD,
                                  STL_SIGNAL_IGNORE,
                                  NULL,
                                  aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    
    STL_TRY( stlCreateRegionAllocator( &(gSlaveMgr.mRegionMem),
                                       ZTR_REGION_INIT_SIZE,
                                       ZTR_REGION_NEXT_SIZE,
                                       aErrorStack ) == STL_SUCCESS );
    sState = 2;
    
    STL_TRY( stlCreateSemaphore( &(gSlaveMgr.mWaitWriteSem),
                                 "lmwwsem",
                                 0,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 3;
    
    STL_TRY( stlCreateSemaphore( &(gSlaveMgr.mWaitReadSem),
                                 "lmwrsem",
                                 0,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 4;
    
    STL_TRY( stlCreateSemaphore( &(gSlaveMgr.mWaitFlushSem),
                                 "lmwfsem",
                                 0,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 5;
    
    
    STL_RING_INIT_HEADLINK( &gSlaveMgr.mWaitWriteCkList,
                            STL_OFFSETOF( ztrChunkItem, mLink ) );
    
    STL_RING_INIT_HEADLINK( &gSlaveMgr.mWriteCkList,
                            STL_OFFSETOF( ztrChunkItem, mLink ) );
    
    STL_RING_INIT_HEADLINK( &gSlaveMgr.mWaitReadCkList,
                            STL_OFFSETOF( ztrChunkItem, mLink ) );
    
    STL_RING_INIT_HEADLINK( &gSlaveMgr.mReadCkList,
                            STL_OFFSETOF( ztrChunkItem, mLink ) );
    
    gSlaveMgr.mWaitWriteState = STL_FALSE;
    gSlaveMgr.mWaitReadState  = STL_FALSE;
    
    ztrmInitSpinLock( &(gSlaveMgr.mWriteSpinLock) );
    ztrmInitSpinLock( &(gSlaveMgr.mReadSpinLock) );
    
    /**
     * CHUNK는 Direct I/O에 사용되는 Buffer임으로, 512 Byte align에 맞도록 한다.
     */
    for( sIdx = 0; sIdx < ZTR_DEFAULT_SLAVE_CHUNK_COUNT; sIdx++ )
    {
        STL_TRY( stlAllocRegionMem( &(gSlaveMgr.mRegionMem),
                                    STL_SIZEOF( ztrChunkItem ),
                                    (void**)&sChunkItem,
                                    aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( stlAllocRegionMem( &(gSlaveMgr.mRegionMem),
                                    ZTR_DEFAULT_SLAVE_CHUNK_SIZE + ZTR_MEM_ALIGN,
                                    (void**)&sChunkItem->mOrgBuffer,
                                    aErrorStack ) == STL_SUCCESS );
        
        sChunkItem->mBuffer        = (void*)STL_ALIGN( (stlUInt64)sChunkItem->mOrgBuffer, ZTR_MEM_ALIGN );
        sChunkItem->mWriteIdx      = 0;
        sChunkItem->mReadIdx       = 0;
        sChunkItem->mHasLogFileHdr = STL_FALSE;
        
        STL_RING_INIT_DATALINK( sChunkItem,
                                STL_OFFSETOF( ztrChunkItem, mLink ) );
        
        /**
         * 처음에는 Wait Write ChunkList에 넣는다.
         */
        STL_RING_ADD_LAST( &gSlaveMgr.mWriteCkList, 
                           sChunkItem );
    }
    
    /**
     * Send/Rcv Buffer를 Initialize 한다.
     */
    STL_TRY( stlAllocRegionMem( &(gSlaveMgr.mRegionMem),
                                ZTR_DEFAULT_COMM_BUFFER_SIZE,
                                (void**)&gSlaveMgr.mSendBuff,
                                aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlAllocRegionMem( &(gSlaveMgr.mRegionMem),
                                ZTR_DEFAULT_COMM_BUFFER_SIZE,
                                (void**)&gSlaveMgr.mRecvBuff,
                                aErrorStack ) == STL_SUCCESS );
    
    /**
     * NETWORK INTERFACE 설정
     */
    if( ztrmGetIBMode() == STL_FALSE )
    {
        ztrmSetCommunicateCallback( &gCallbackForNormal );
    }
    else
    {
        ztrmSetCommunicateCallback( &gCallbackForInifiniband );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    switch( sState )
    {
        case 5:
            ztrmFinSpinLock( &(gSlaveMgr.mWriteSpinLock) );
            ztrmFinSpinLock( &(gSlaveMgr.mReadSpinLock) );
            
            (void) stlDestroySemaphore( &(gSlaveMgr.mWaitFlushSem),
                                        aErrorStack );
        case 4:
            
            (void) stlDestroySemaphore( &(gSlaveMgr.mWaitReadSem),
                                        aErrorStack );
        case 3:
            (void) stlDestroySemaphore( &(gSlaveMgr.mWaitWriteSem),
                                        aErrorStack );
        case 2:
            (void) stlDestroyRegionAllocator( &(gSlaveMgr.mRegionMem),
                                              aErrorStack );
        case 1:
            (void) stlSetSignalHandler( ZTR_SIGNAL_DEFINE,
                                        gZtrConfigure.mOldSignalFunc,
                                        NULL,
                                        aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;  
}


stlStatus ztrmFinalizeEnv4Slave( stlErrorStack  * aErrorStack )
{
    ztrmFinSpinLock( &(gSlaveMgr.mWriteSpinLock) );
    ztrmFinSpinLock( &(gSlaveMgr.mReadSpinLock) );
    
    STL_TRY( stlDestroySemaphore( &(gSlaveMgr.mWaitFlushSem),
                                  aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlDestroySemaphore( &(gSlaveMgr.mWaitReadSem),
                                  aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlDestroySemaphore( &(gSlaveMgr.mWaitWriteSem),
                                  aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlDestroyRegionAllocator( &(gSlaveMgr.mRegionMem),
                                        aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlSetSignalHandler( ZTR_SIGNAL_DEFINE,
                                  gZtrConfigure.mOldSignalFunc,
                                  NULL,
                                  aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;   
}


stlStatus ztrmSetLogMirrorShmKeyFromDB( stlErrorStack * aErrorStack )
{
    SQLHENV       sEnvHandle;
    SQLHDBC       sDbcHandle;
    SQLHSTMT      sStmtHandle;
    SQLLEN        sIndicator;
    SQLRETURN     sResult;
    
    stlChar       sSqlString[1024] = {0,};
    stlInt64      sShmKey = 0;
    stlInt32      sState  = 0;
    
    STL_TRY( ztrmAllocEnvHandle( &sEnvHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztrmCreateConnection( &sDbcHandle,
                                   sEnvHandle,
                                   aErrorStack ) == STL_SUCCESS );
    sState = 2;
    STL_TRY( ztrmSQLAllocStmt( sDbcHandle,
                               &sStmtHandle,
                               aErrorStack ) == STL_SUCCESS );
    sState = 3;

    
    //////////////////////////////////////////////
    
    stlSnprintf( sSqlString,
                 1024,
                 "SELECT KEY FROM X$SHM_SEGMENT WHERE NAME='%s'",
                 ZTR_LOGMIRROR_DEFAULT_SHM_NAME );
    
    STL_TRY( ztrmSQLBindCol( sStmtHandle,
                             1,
                             SQL_C_SBIGINT, 
                             &sShmKey,
                             STL_SIZEOF( sShmKey ),
                             &sIndicator,
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztrmSQLExecDirect( sStmtHandle, 
                                (SQLCHAR*)sSqlString,
                                stlStrlen( sSqlString ),
                                &sResult, 
                                aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztrmSQLFetch( sStmtHandle, 
                           &sResult, 
                           aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( sResult != SQL_NO_DATA, RAMP_ERR_NO_SHM_KEY );
    
    gZtrConfigure.mShmKey = sShmKey;
    
    //////////////////////////////////////////////
    
    
    sState = 2;
    STL_TRY( ztrmSQLFreeStmt( sStmtHandle,
                              aErrorStack ) == SQL_SUCCESS );
    
    sState = 1;
    STL_TRY( ztrmDestroyConnection( sDbcHandle,
                                    aErrorStack ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( ztrmFreeEnvHandle( sEnvHandle, 
                                aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;   
    
    STL_CATCH( RAMP_ERR_NO_SHM_KEY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_INVALID_SHM_KEY,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;

    switch( sState )
    {
        case 3:
            (void) ztrmSQLFreeStmt( sStmtHandle,
                                    aErrorStack );
        case 2:
            (void) ztrmDestroyConnection( sDbcHandle,
                                          aErrorStack );
        case 1:
            (void) ztrmFreeEnvHandle( sEnvHandle, 
                                      aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
    
    
}

stlStatus ztrmGetPidFileName( stlChar       * aFileName,
                              stlBool         aIsMaster,
                              stlErrorStack * aErrorStack )
{
    stlChar   sFileName[STL_MAX_FILE_PATH_LENGTH];
    
    if( aIsMaster == STL_TRUE )
    {
        stlSnprintf( sFileName, 
                     STL_SIZEOF( sFileName ), 
                     "%sconf"STL_PATH_SEPARATOR"%s", 
                     gZtrConfigure.mHomeDir,
                     ZTR_DEFAULT_MASTER_PID_FILE );
    }
    else
    {
        stlSnprintf( sFileName, 
                     STL_SIZEOF( sFileName ), 
                     "%sconf"STL_PATH_SEPARATOR"%s", 
                     gZtrConfigure.mHomeDir,
                     ZTR_DEFAULT_SLAVE_PID_FILE );
    }
    
    stlStrncpy( aFileName, 
                sFileName, 
                stlStrlen( sFileName ) + 1 );
    
    return STL_SUCCESS;
}


stlStatus ztrmSetUserId( stlChar       * aUserId,
                         stlErrorStack * aErrorStack )
{
    stlInt32   sLen = 0;
    
    sLen = stlStrlen( aUserId );
    
    STL_TRY_THROW( sLen < STL_MAX_SQL_IDENTIFIER_LENGTH, RAMP_ERR_TOO_LONG_USER_ID );
    
    stlStrncpy( gZtrConfigure.mUserId, aUserId, sLen + 1 );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_TOO_LONG_USER_ID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "USER_ID",
                      aUserId );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztrmGetUserId( stlChar       * aUserId,
                         stlErrorStack * aErrorStack )
{
    stlInt32 sLen = 0;
    
    sLen = stlStrlen( gZtrConfigure.mUserId );
    
    STL_TRY_THROW( sLen > 0, RAMP_ERR_INVALID_USER_ID );
    
    stlStrncpy( aUserId, gZtrConfigure.mUserId, sLen + 1 );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_USER_ID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "USER_ID",
                      "no data" );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztrmSetUserPw( stlChar       * aUserPw,
                         stlErrorStack * aErrorStack )
{
    stlInt32   sLen = 0;
    
    sLen = stlStrlen( aUserPw );
    
    STL_TRY_THROW( sLen < STL_MAX_SQL_IDENTIFIER_LENGTH, RAMP_ERR_TOO_LONG_USER_PW );
    
    stlStrncpy( gZtrConfigure.mUserPw, aUserPw, sLen + 1 );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_TOO_LONG_USER_PW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "USER_PW",
                      "invalid Password" );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztrmGetUserPw( stlChar       * aUserPw,
                         stlErrorStack * aErrorStack )
{
    stlInt32 sLen = 0;
    
    sLen = stlStrlen( gZtrConfigure.mUserPw );
    
    STL_TRY_THROW( sLen > 0, RAMP_ERR_INVALID_USER_PW );
    
    stlStrncpy( aUserPw, gZtrConfigure.mUserPw, sLen + 1 );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_USER_PW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "USER_PW",
                      "no data" );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztrmSetConfigureFile( const stlChar * aConfigureFile,
                                stlErrorStack * aErrorStack )
{
    STL_TRY_THROW( stlStrlen( aConfigureFile ) < STL_MAX_FILE_PATH_LENGTH, RAMP_ERR_INVALID_FILE_NAME );
    
    stlStrncpy( gZtrConfigure.mUserConfigure, 
                aConfigureFile,
                stlStrlen( aConfigureFile ) + 1 );
    
    return STL_SUCCESS;   
    
    STL_CATCH( RAMP_ERR_INVALID_FILE_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "Configure file",
                      aConfigureFile );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztrmGetConfigureFileName( stlChar       * aFileName,
                                    stlErrorStack * aErrorStack )
{
    stlChar   sFileName[STL_MAX_FILE_PATH_LENGTH];
    stlChar * sAbsolute = NULL;
    stlBool   sExist    = STL_FALSE;
    
    /**
     * Configure File 경로 검색
     * - Configure 옵션을 설정했을 경우
     *   1. 현재 Directory에서 검색
     *   2. GOLDILOCKS_DATA/conf에서 검색
     *   3. 절대 경로일 경우 해당 Path에서만 검색
     * - Configure 옵션을 설정하지 않았을 경우
     *   1. master : logmirror.master.conf
     *   2. slave  : logmirror.slave.conf
     */
    
    if( stlStrlen( gZtrConfigure.mUserConfigure ) > 0 )
    {
        sAbsolute = stlStrchr( gZtrConfigure.mUserConfigure, '/' );
        
        STL_TRY( stlExistFile( gZtrConfigure.mUserConfigure,
                               &sExist,
                               aErrorStack ) == STL_SUCCESS );
        
        if( sExist == STL_FALSE )
        {
            if( sAbsolute == gZtrConfigure.mUserConfigure )
            {
                stlStrncpy( sFileName, 
                            gZtrConfigure.mUserConfigure, 
                            stlStrlen( gZtrConfigure.mUserConfigure ) + 1 );
                
                STL_TRY_THROW( sExist == STL_TRUE, RAMP_ERR_FILE_NOT_EXIST );
            }
            
            stlSnprintf( sFileName, 
                         STL_SIZEOF( sFileName ), 
                         "%sconf"STL_PATH_SEPARATOR"%s", 
                         gZtrConfigure.mHomeDir, 
                         gZtrConfigure.mUserConfigure );
            
            STL_TRY( stlExistFile( sFileName,
                                   &sExist,
                                   aErrorStack ) == STL_SUCCESS );
            
            STL_TRY_THROW( sExist == STL_TRUE, RAMP_ERR_FILE_NOT_EXIST );
            
            stlStrncpy( aFileName, 
                        sFileName, 
                        stlStrlen( sFileName ) + 1 );
        }
        else
        {
            stlStrncpy( aFileName, 
                        gZtrConfigure.mUserConfigure, 
                        stlStrlen( gZtrConfigure.mUserConfigure ) + 1 );
        }
        
    }
    else
    {
        if( gZtrConfigure.mIsMaster == STL_TRUE )
        {
            stlSnprintf( sFileName, 
                         STL_SIZEOF( sFileName ), 
                         "%sconf"STL_PATH_SEPARATOR"%s", 
                         gZtrConfigure.mHomeDir, 
                         ZTR_DEFAULT_MASTER_CONFIGURE_FILE );
        }
        else
        {
            stlSnprintf( sFileName, 
                         STL_SIZEOF( sFileName ), 
                         "%sconf"STL_PATH_SEPARATOR"%s", 
                         gZtrConfigure.mHomeDir, 
                         ZTR_DEFAULT_SLAVE_CONFIGURE_FILE );
        }
        
        STL_TRY( stlExistFile( sFileName,
                               &sExist,
                               aErrorStack ) == STL_SUCCESS );
            
        STL_TRY_THROW( sExist == STL_TRUE, RAMP_ERR_FILE_NOT_EXIST );
        
        stlStrncpy( aFileName, 
                    sFileName, 
                    stlStrlen( sFileName ) + 1 );
    }
        
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_FILE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_INVALID_FILE_NAME,
                      NULL,
                      aErrorStack,
                      sFileName );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztrmSetLogPath( stlChar       * aLogPath,
                          stlErrorStack * aErrorStack )
{
    stlInt32   sLength  = 0;
    stlChar    sLogPath[ STL_MAX_FILE_PATH_LENGTH ];
    
    sLength = stlStrlen( aLogPath );
    
    STL_TRY_THROW( sLength < STL_MAX_FILE_PATH_LENGTH &&
                   sLength > 2, RAMP_ERR_INVALID_LOG_PATH );
    
    /**
     * 앞뒤의 ' 를 제거한다.
     */
    stlMemset( sLogPath, 0x00, STL_MAX_FILE_PATH_LENGTH );
    
    stlMemcpy( sLogPath,
               &aLogPath[1],
               sLength - 2 );
    
    sLength = stlStrlen( sLogPath );
    
    /**
     * Directory가 '/', '\'로 끝날 경우에 제거한다.
     */
    if ( sLogPath[ sLength - 1 ] == '/' ||
         sLogPath[ sLength - 1 ] == '\\' )
    {
        sLength = sLength - 1;
    }
    else
    {
        sLength = sLength;
    }
    
    stlStrncpy( gZtrConfigure.mLogPath, 
                sLogPath,
                sLength + 1 );
    
    return STL_SUCCESS;   
    
    STL_CATCH( RAMP_ERR_INVALID_LOG_PATH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "LOG_PATH",
                      aLogPath );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztrmGetLogPath( stlChar       * aLogPath,
                          stlErrorStack * aErrorStack )
{
    STL_TRY_THROW( stlStrlen( gZtrConfigure.mLogPath ) > 0, RAMP_ERR_INVALID_CONFIGURE_FILE );
    
    stlStrncpy( aLogPath, 
                gZtrConfigure.mLogPath, 
                stlStrlen( gZtrConfigure.mLogPath ) + 1 );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_CONFIGURE_FILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "Configure file",
                      "no data" );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztrmGetLogFileName( stlChar       * aFileName,
                              stlInt64        aFileSeq,
                              stlErrorStack * aErrorStack )
{
    stlChar  sLogPath[ STL_MAX_FILE_PATH_LENGTH ];
    
    STL_TRY( ztrmGetLogPath( sLogPath, aErrorStack ) == STL_SUCCESS );
    
    stlSnprintf( aFileName,
                 STL_MAX_FILE_PATH_LENGTH,
                 "%s"STL_PATH_SEPARATOR"%s%ld.log",
                 sLogPath,
                 ZTR_LOG_FILE_PREFIX, 
                 aFileSeq );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztrmGetControlFileName( stlChar       * aFileName,
                                  stlErrorStack * aErrorStack )
{
    stlChar  sLogPath[ STL_MAX_FILE_PATH_LENGTH ];
    
    STL_TRY( ztrmGetLogPath( sLogPath, aErrorStack ) == STL_SUCCESS );
    
    stlSnprintf( aFileName,
                 STL_MAX_FILE_PATH_LENGTH,
                 "%s"STL_PATH_SEPARATOR"%s",
                 sLogPath,
                 ZTR_DEFAULT_CONTROL_FILE );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztrmSetPort( stlChar       * aPort,
                       stlErrorStack * aErrorStack )
{
    stlChar    * sEndPtr;
    stlInt64     sPort = 0;
    
    STL_TRY( stlStrToInt64( aPort,
                            STL_NTS,
                            &sEndPtr,
                            10,
                            &sPort, 
                            aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( *sEndPtr == 0, RAMP_ERR_INVALID_VALUE );
    
    STL_TRY_THROW( sPort < 65536, RAMP_ERR_INVALID_VALUE );
    
    gZtrConfigure.mPort = (stlInt32) sPort;
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_INVALID_PORT,
                      NULL,
                      aErrorStack,
                      sPort );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztrmGetPort( stlInt32      * aPort,
                       stlErrorStack * aErrorStack )
{
    stlInt64    sPort       = 0;
    stlBool     sFound      = STL_FALSE;
    stlChar   * sEndPtr     = NULL;
    stlChar     sTmpStr[16] = {0,};
    
    STL_TRY( stlGetEnv( (stlChar*)sTmpStr, 
                        STL_SIZEOF( sTmpStr ),
                        "LOGMIRROR_PORT",
                        &sFound,
                        aErrorStack ) == STL_SUCCESS );

    if( sFound == STL_TRUE )
    {
        STL_TRY( stlStrToInt64( sTmpStr,
                                STL_NTS,
                                &sEndPtr,
                                10,
                                &sPort, 
                                aErrorStack ) == STL_SUCCESS );
    
        /** 입력된 String이 정상적으로 변환이 되었을 경우에는 EndPtr는 0 된다. */
        //STL_TRY_THROW( *sEndPtr == 0, RAMP_ERR_INVALID_VALUE );
    }
    else
    {
        sPort = ZTR_DEFAULT_PORT;
    }
    
    if( gZtrConfigure.mPort != 0 )
    {
        sPort = gZtrConfigure.mPort;
    }
    
    STL_TRY_THROW( sPort < 65536, RAMP_ERR_INVALID_VALUE );
    
    *aPort = (stlInt32) sPort;
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_INVALID_PORT,
                      NULL,
                      aErrorStack,
                      sPort );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztrmSetMasterIp( stlChar       * aMasterIp,
                           stlErrorStack * aErrorStack )
{
    stlInt32   sLen = 0;
    
    sLen = stlStrlen( aMasterIp );
    
    STL_TRY_THROW( sLen < 16, RAMP_ERR_TOO_LONG_IP );
    
    stlStrncpy( gZtrConfigure.mMasterIp, aMasterIp, sLen + 1 );
    
    return STL_SUCCESS;
       
    STL_CATCH( RAMP_ERR_TOO_LONG_IP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "MASTER_IP",
                      aMasterIp );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlChar * ztrmGetMasterIp()
{
    stlInt32   sLen = 0;
    stlChar  * ret = NULL;
    
    sLen = stlStrlen( gZtrConfigure.mMasterIp );
    
    if( sLen > 0 )
    {
        ret = gZtrConfigure.mMasterIp;
    }
    
    return ret;
}

stlStatus ztrmSetDsn( stlChar       * aDsn,
                      stlErrorStack * aErrorStack )
{
    stlInt32   sLen = 0;
    
    sLen = stlStrlen( aDsn );
    
    STL_TRY_THROW( sLen < STL_MAX_SQL_IDENTIFIER_LENGTH, RAMP_ERR_TOO_LONG_VALUE );
    
    stlStrncpy( gZtrConfigure.mDsn, aDsn, sLen + 1 );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_TOO_LONG_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "DSN",
                      0 );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlChar * ztrmGetDsn()
{
    stlChar  * ret = NULL;
    
    if( stlStrlen( gZtrConfigure.mDsn ) > 0 )
    {
        ret = gZtrConfigure.mDsn;
    }
    
    return ret;
}


stlStatus ztrmSetHostIp( stlChar       * aHostIp,
                         stlErrorStack * aErrorStack )
{
    stlInt32       sLen   = 0;
    
    sLen = stlStrlen( aHostIp );
    
    STL_TRY_THROW( sLen < 16, RAMP_ERR_TOO_LONG_IP );
    
    stlStrncpy( gZtrConfigure.mHostIp, aHostIp, sLen + 1 );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_TOO_LONG_IP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "Host IP",
                      0 );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlChar * ztrmGetHostIp()
{
    stlChar  * ret = NULL;
    
    if( stlStrlen( gZtrConfigure.mHostIp ) > 0 )
    {
        ret = gZtrConfigure.mHostIp;
    }
    
    return ret;
}

stlStatus ztrmSetHostPort( stlChar       * aPort,
                           stlErrorStack * aErrorStack )
{
    stlChar      * sEndPtr;
    stlInt64       sPort = 0;
    
    STL_TRY( stlStrToInt64( aPort,
                            STL_NTS,
                            &sEndPtr,
                            10,
                            &sPort, 
                            aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( *sEndPtr == 0, RAMP_ERR_INVALID_VALUE );
    
    STL_TRY_THROW( sPort < 65536, RAMP_ERR_INVALID_VALUE );
    
    gZtrConfigure.mHostPort = (stlInt32) sPort;
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_INVALID_PORT,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}   

stlStatus ztrmGetHostPort( stlInt32      * aPort,
                           stlErrorStack * aErrorStack )
{
    stlInt32 sPort = 0;
    
    if( gZtrConfigure.mHostPort > 0 )
    {
        sPort = gZtrConfigure.mHostPort;
    }
    
    STL_TRY_THROW( sPort < 65536 && 
                   sPort != 0, RAMP_ERR_INVALID_VALUE );
    
    *aPort = sPort;
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_INVALID_PORT,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlBool ztrmGetIBMode()
{
    return gZtrConfigure.mIsInfiniband;   
}


/** @} */

