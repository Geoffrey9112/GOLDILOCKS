/*******************************************************************************
 * ztnmGeneral.c
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
 * @file ztnmGeneral.c
 * @brief GlieseTool Cyclone Monitor General Routines
 */

#include <goldilocks.h>
#include <ztn.h>

ztnManager      gZtnManager;
ztnConfigure    gZtnConfigure;
stlBool         gZtnRunState;
extern stlBool  gZtnNoDaemon;

const stlChar *const gZtnMasterStateString[] =
{
    "N/A",
    "RUNNING",
    "READY",
    NULL
};

const stlChar *const gZtnSlaveStateString[] =
{
    "N/A",
    "RUNNING",
    NULL
};


stlStatus ztnmInitializeEnv( stlErrorStack * aErrorStack )
{
    stlInt32  sState = 0;
    
    STL_TRY( stlCreateRegionAllocator( &gZtnManager.mAllocator,
                                       ZTN_REGION_INIT_SIZE,
                                       ZTN_REGION_NEXT_SIZE,
                                       aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( stlSetSignalHandler( ZTN_SIGNAL_DEFINE,
                                  ztnmMonitorSignalFunc,
                                  &gZtnConfigure.mOldSignalFunc,
                                  aErrorStack ) == STL_SUCCESS );
    sState = 2;
    
    /**
     * 기타 Signal을 무시한다.
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
    
    STL_RING_INIT_HEADLINK( &gZtnManager.mGroupList,
                            STL_OFFSETOF( ztnGroupItem, mLink ) );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    switch( sState )
    {
        case 2:
            (void)stlSetSignalHandler( ZTN_SIGNAL_DEFINE,
                                       gZtnConfigure.mOldSignalFunc,
                                       NULL,
                                       aErrorStack );
        case 1:
            (void)stlDestroyRegionAllocator( &gZtnManager.mAllocator,
                                             aErrorStack );
        default:
            break;
        
    }
    
    return STL_FAILURE;
}


stlStatus ztnmFinalizeEnv( stlErrorStack * aErrorStack )
{
    STL_TRY( stlSetSignalHandler( ZTN_SIGNAL_DEFINE,
                                  gZtnConfigure.mOldSignalFunc,
                                  NULL,
                                  aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlDestroyRegionAllocator( &gZtnManager.mAllocator,
                                        aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;   
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztnmStart( stlErrorStack * aErrorStack )
{
    stlProc   sProc;
    stlBool   sIsChild = STL_FALSE;
    
    stlFile   sFile;
    stlChar   sFileName[STL_MAX_FILE_PATH_LENGTH];
    stlChar   sWriteBuff[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlSize   sWriteSize = 0;

    stlBool   sExist = STL_FALSE;
    stlInt32  sState = 0;
    
    STL_TRY_THROW( STL_RING_IS_EMPTY( &gZtnManager.mGroupList ) != STL_TRUE, RAMP_ERR_INVALID_GROUP_COUNT );
    
    if( gZtnNoDaemon == STL_TRUE )
    {
        sIsChild = STL_TRUE;
    }
    else
    {
        STL_TRY( stlForkProc( &sProc,
                              &sIsChild,
                              aErrorStack ) == STL_SUCCESS );
    }
            
    if( sIsChild == STL_FALSE )
    {
        /**
         * PId 관련 처리해야 함
         */
    }
    else
    {
        STL_TRY( stlGetCurrentProc( &sProc, aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztnmGetPidFileName( sFileName, 
                                     aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( stlExistFile( sFileName,
                               &sExist,
                               aErrorStack ) == STL_SUCCESS );

        if ( sExist == STL_TRUE )
        {
            STL_TRY( ztnmProcessAliveCheck( sFileName, aErrorStack ) == STL_SUCCESS );
        }
        
        STL_TRY( stlOpenFile( &sFile,
                              sFileName,
                              STL_FOPEN_CREATE | STL_FOPEN_WRITE | STL_FOPEN_BINARY,
                              STL_FPERM_OS_DEFAULT,
                              aErrorStack ) == STL_SUCCESS );
        sState = 1;
        
        stlSnprintf( sWriteBuff,
                     STL_SIZEOF( sWriteBuff ),
                     "%d\n",
                     sProc.mProcID );
        
        STL_TRY( stlLockFile( &sFile, 
                              STL_FLOCK_EXCLUSIVE,
                              aErrorStack ) == STL_SUCCESS );
        sState = 2;
        
        STL_TRY( stlWriteFile( &sFile,
                               sWriteBuff,
                               stlStrlen( sWriteBuff ),
                               &sWriteSize,
                               aErrorStack ) == STL_SUCCESS );
        
        sState = 1;
        STL_TRY( stlUnlockFile( &sFile, 
                                aErrorStack ) == STL_SUCCESS );

        sState = 0;
        STL_TRY( stlCloseFile( &sFile,
                               aErrorStack ) == STL_SUCCESS );

        /**
         * Running!!!
         */
        gZtnRunState = STL_TRUE;
        ztnmLogMsg( aErrorStack, "Initializing \n" );

        /**
         * Do Monitor
         */
        STL_TRY( ztnmDoMonitor( aErrorStack ) == STL_SUCCESS );

        STL_TRY( ztnmLogMsg( aErrorStack,
                             "STOP Done.\n" ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;   
    
    STL_CATCH( RAMP_ERR_INVALID_GROUP_COUNT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTN_ERRCODE_INVALID_GROUP_COUNT,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    ztnmWriteLastError( aErrorStack );
    
    switch( sState )
    {
        case 2:
            (void)stlUnlockFile( &sFile, 
                                 aErrorStack );
        case 1:
            (void)stlCloseFile( &sFile,
                                aErrorStack );
        default:
            break;   
    }
    
    return STL_FAILURE;
}


stlStatus ztnmStop( stlErrorStack * aErrorStack )
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
    
    STL_TRY( ztnmGetPidFileName( sFileName, 
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
                     ZTN_SIGNAL_DEFINE,
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
                      ZTN_ERRCODE_NOT_RUNNING,
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


stlStatus ztnmProcessAliveCheck( stlChar       * aFileName,
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
                      ZTN_ERRCODE_ALREADY_RUNNING,
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


stlStatus ztnmGetPidFileName( stlChar       * aFileName,
                              stlErrorStack * aErrorStack )
{
    stlChar   sFileName[STL_MAX_FILE_PATH_LENGTH];
    
    stlSnprintf( sFileName, 
                 STL_SIZEOF( sFileName ),
                 "%sconf"STL_PATH_SEPARATOR"%s",
                 gZtnConfigure.mHomeDir,
                 ZTN_DEFAULT_PID_FILE );
    
    stlStrncpy( aFileName, 
                sFileName, 
                stlStrlen( sFileName ) + 1 );
    
    return STL_SUCCESS;
}


stlStatus ztnmGetConfigureFileName( stlChar       * aFileName,
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
     *   . cyclone.master.conf
     */
    
    if( stlStrlen( gZtnConfigure.mUserConfigure ) > 0 )
    {
        sAbsolute = stlStrchr( gZtnConfigure.mUserConfigure, '/' );
        
        STL_TRY( stlExistFile( gZtnConfigure.mUserConfigure,
                               &sExist,
                               aErrorStack ) == STL_SUCCESS );
        
        if( sExist == STL_FALSE )
        {
            if( sAbsolute == gZtnConfigure.mUserConfigure )
            {
                stlStrncpy( sFileName, 
                            gZtnConfigure.mUserConfigure, 
                            stlStrlen( gZtnConfigure.mUserConfigure ) + 1 );
                
                STL_TRY_THROW( sExist == STL_TRUE, RAMP_ERR_FILE_NOT_EXIST );
            }
            
            stlSnprintf( sFileName, 
                         STL_SIZEOF( sFileName ), 
                         "%sconf"STL_PATH_SEPARATOR"%s", 
                         gZtnConfigure.mHomeDir, 
                         gZtnConfigure.mUserConfigure );
            
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
                        gZtnConfigure.mUserConfigure, 
                        stlStrlen( gZtnConfigure.mUserConfigure ) + 1 );
        }
    }
    else
    {
        stlSnprintf( sFileName, 
                     STL_SIZEOF( sFileName ), 
                     "%sconf"STL_PATH_SEPARATOR"%s", 
                     gZtnConfigure.mHomeDir, 
                     ZTN_DEFAULT_CONFIGURE_FILE );
        
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
                      ZTN_ERRCODE_INVALID_FILE_NAME,
                      NULL,
                      aErrorStack,
                      sFileName );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztnmSetConfigureFileName( const stlChar * aConfigureFile,
                                    stlErrorStack * aErrorStack )
{
    STL_TRY_THROW( stlStrlen( aConfigureFile ) < STL_MAX_FILE_PATH_LENGTH, RAMP_ERR_INVALID_FILE_NAME );
    
    stlStrncpy( gZtnConfigure.mUserConfigure, 
                aConfigureFile,
                stlStrlen( aConfigureFile ) + 1 );
    
    return STL_SUCCESS;   
    
    STL_CATCH( RAMP_ERR_INVALID_FILE_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTN_ERRCODE_INVALID_FILE_NAME,
                      NULL,
                      aErrorStack,
                      aConfigureFile );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztnmSetHomeDir( stlErrorStack * aErrorStack )
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
    
    stlSnprintf( gZtnConfigure.mHomeDir, 
                 STL_MAX_FILE_PATH_LENGTH, 
                 "%s"STL_PATH_SEPARATOR, 
                 sPath );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_HOMEDIR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTN_ERRCODE_INVALID_HOME,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztnmAddGroupItem( stlChar       * aGroupName,
                            stlErrorStack * aErrorStack )
{
    ztnGroupItem  * sGroup        = NULL;
    ztnGroupItem  * sCompareGroup = NULL;
    
    if( STL_RING_IS_EMPTY( &gZtnManager.mGroupList ) != STL_TRUE )
    {
        STL_RING_FOREACH_ENTRY( &gZtnManager.mGroupList, sCompareGroup )
        {
            STL_TRY_THROW( stlStrcasecmp( sCompareGroup->mName, aGroupName ) != 0, RAMP_ERR_DUPLICATE_GROUP );
        }
    }
    
    STL_TRY( stlAllocRegionMem( &gZtnManager.mAllocator,
                                STL_SIZEOF( ztnGroupItem ),
                                (void**)&sGroup,
                                aErrorStack ) == STL_SUCCESS );

    STL_RING_INIT_DATALINK( sGroup,
                            STL_OFFSETOF( ztnGroupItem, mLink ) );
    
    stlMemset( sGroup, 0x00, STL_SIZEOF( ztnGroupItem ) );
    
    stlStrncpy( sGroup->mName, 
                aGroupName,
                stlStrlen( aGroupName ) + 1 );
        
    stlToupperString( sGroup->mName, sGroup->mName );
    gZtnConfigure.mGroupCount++;
    
    STL_RING_ADD_LAST( &gZtnManager.mGroupList, 
                       sGroup );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_DUPLICATE_GROUP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTN_ERRCODE_DUPLICATE_GROUP_NAME,
                      NULL,
                      aErrorStack,
                      aGroupName );
        
    }
    STL_FINISH;
    
    return STL_FAILURE;   
}


void ztnmPrintErrorstack( stlErrorStack * aErrorStack )
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


void ztnmWriteLastError( stlErrorStack * aErrorStack )
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
        
        (void) ztnmLogMsg( aErrorStack,
                           "ERR-%s(%d): %s %s\n",
                           sSqlState,
                           sErrorCode,
                           aErrorStack->mErrorData[i].mErrorMessage,
                           aErrorStack->mErrorData[i].mDetailErrorMessage );
    }
}

stlStatus ztnmSetPort( stlChar       * aPort,
                       stlErrorStack * aErrorStack )
{
    stlChar      * sEndPtr;
    stlInt64       sPort = 0;
    ztnGroupItem * sGroup = NULL;
    
    STL_TRY_THROW( STL_RING_IS_EMPTY( &gZtnManager.mGroupList ) != STL_TRUE, RAMP_ERR_INVALID_GROUP );
    
    sGroup = STL_RING_GET_LAST_DATA( &gZtnManager.mGroupList );
    
    STL_TRY( stlStrToInt64( aPort,
                            STL_NTS,
                            &sEndPtr,
                            10,
                            &sPort, 
                            aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( *sEndPtr == 0, RAMP_ERR_INVALID_VALUE );
    
    STL_TRY_THROW( sPort < 65536, RAMP_ERR_INVALID_VALUE );
    
    sGroup->mPort = (stlInt32) sPort;
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTN_ERRCODE_INVALID_PORT,
                      NULL,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_INVALID_GROUP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTN_ERRCODE_INVALID_GROUP_NAME,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztnmSetDsnToGlobalArea( stlChar       * aDsn,
                                  stlErrorStack * aErrorStack )
{
    stlInt32   sLen = 0;
    
    sLen = stlStrlen( aDsn );
    
    STL_TRY_THROW( sLen < STL_MAX_SQL_IDENTIFIER_LENGTH, RAMP_ERR_TOO_LONG_VALUE );
    
    stlStrncpy( gZtnConfigure.mDsn, aDsn, sLen + 1 );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_TOO_LONG_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTN_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "DSN",
                      0 );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztnmSetDsnToGroupArea( stlChar       * aDsn,
                                 stlErrorStack * aErrorStack )
{
    stlInt32       sLen   = 0;
    ztnGroupItem * sGroup = NULL;
    
    sLen = stlStrlen( aDsn );
    
    STL_TRY_THROW( sLen < STL_MAX_SQL_IDENTIFIER_LENGTH, RAMP_ERR_TOO_LONG_VALUE );
    
    STL_TRY_THROW( STL_RING_IS_EMPTY( &gZtnManager.mGroupList ) != STL_TRUE, RAMP_ERR_INVALID_GROUP );
    
    sGroup = STL_RING_GET_LAST_DATA( &gZtnManager.mGroupList );
    
    stlStrncpy( sGroup->mDsn, aDsn, sLen + 1 );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_TOO_LONG_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTN_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "DSN",
                      0 );
    }
    STL_CATCH( RAMP_ERR_INVALID_GROUP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTN_ERRCODE_INVALID_GROUP_NAME,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlChar * ztnmGetDsn()
{
    stlChar      * sRet   = NULL;
    stlInt32       sLen   = 0;
    ztnGroupItem * sGroup = NULL;
    stlBool        sFound = STL_FALSE;
    
    /**
     * 하나의  서버에서 실행되므로 처음나오는 DSN으로 세팅한다.
     */
    STL_RING_FOREACH_ENTRY( &gZtnManager.mGroupList, sGroup )
    {
        sLen = stlStrlen( sGroup->mDsn );
        
        if( sLen > 0 )
        {
            sFound = STL_TRUE;
            break;   
        }
    }
    
    if( sFound == STL_TRUE )
    {
        sRet = sGroup->mDsn;
    }
    else
    {
        if( stlStrlen( gZtnConfigure.mDsn ) > 0 )
        {
            sRet = gZtnConfigure.mDsn;
        }
    }
    
    return sRet;
}


stlStatus ztnmSetHostIpToGlobalArea( stlChar       * aHostIp,
                                     stlErrorStack * aErrorStack )
{
    stlInt32   sLen = 0;
    
    sLen = stlStrlen( aHostIp );
    
    STL_TRY_THROW( sLen < 16, RAMP_ERR_TOO_LONG_IP );
    
    stlStrncpy( gZtnConfigure.mHostIp, aHostIp, sLen + 1 );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_TOO_LONG_IP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTN_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "Host IP",
                      0 );
    }
    STL_FINISH;
    
    return STL_FAILURE; 
    
}


stlStatus ztnmSetHostIpToGroupArea( stlChar       * aHostIp,
                                    stlErrorStack * aErrorStack )
{
    stlInt32       sLen   = 0;
    ztnGroupItem * sGroup = NULL;
    
    sLen = stlStrlen( aHostIp );
    
    STL_TRY_THROW( sLen < 16, RAMP_ERR_TOO_LONG_IP );
    
    STL_TRY_THROW( STL_RING_IS_EMPTY( &gZtnManager.mGroupList ) != STL_TRUE, RAMP_ERR_INVALID_GROUP );
    
    sGroup = STL_RING_GET_LAST_DATA( &gZtnManager.mGroupList );
    
    stlStrncpy( sGroup->mHostIp, aHostIp, sLen + 1 );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_TOO_LONG_IP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTN_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "Host IP",
                      0 );
    }
    STL_CATCH( RAMP_ERR_INVALID_GROUP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTN_ERRCODE_INVALID_GROUP_NAME,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlChar * ztnmGetHostIp()
{
    stlChar      * sRet   = NULL;
    stlInt32       sLen   = 0;
    ztnGroupItem * sGroup = NULL;
    stlBool        sFound = STL_FALSE;
    
    STL_RING_FOREACH_ENTRY( &gZtnManager.mGroupList, sGroup )
    {
        sLen = stlStrlen( sGroup->mHostIp );
        
        if( sLen > 0 )
        {
            sFound = STL_TRUE;
            break;   
        }
    }
    
    if( sFound == STL_TRUE )
    {
        sRet = sGroup->mHostIp;
    }
    else
    {
        if( stlStrlen( gZtnConfigure.mHostIp ) > 0 )
        {
            sRet = gZtnConfigure.mHostIp;
        }
    }
    
    return sRet;
}


stlStatus ztnmSetHostPortToGlobalArea( stlChar       * aPort,
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
    
    gZtnConfigure.mHostPort = (stlInt32) sPort;
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTN_ERRCODE_INVALID_PORT,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztnmSetHostPortToGroupArea( stlChar       * aPort,
                                      stlErrorStack * aErrorStack )
{
    stlChar      * sEndPtr;
    stlInt64       sPort = 0;
    ztnGroupItem * sGroup = NULL;
    
    STL_TRY( stlStrToInt64( aPort,
                            STL_NTS,
                            &sEndPtr,
                            10,
                            &sPort, 
                            aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( *sEndPtr == 0, RAMP_ERR_INVALID_VALUE );
    
    STL_TRY_THROW( sPort < 65536, RAMP_ERR_INVALID_VALUE );
    
    STL_TRY_THROW( STL_RING_IS_EMPTY( &gZtnManager.mGroupList ) != STL_TRUE, RAMP_ERR_INVALID_GROUP );
    
    sGroup = STL_RING_GET_LAST_DATA( &gZtnManager.mGroupList );
    
    sGroup->mHostPort = (stlInt32) sPort;
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTN_ERRCODE_INVALID_PORT,
                      NULL,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_INVALID_GROUP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTN_ERRCODE_INVALID_GROUP_NAME,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztnmGetHostPort( stlInt32      * aPort,
                           stlErrorStack * aErrorStack )
{
    stlInt32       sPort = 0;
    ztnGroupItem * sGroup = NULL;
    stlBool        sFound = STL_FALSE;
    
    STL_RING_FOREACH_ENTRY( &gZtnManager.mGroupList, sGroup )
    {
        if( sGroup->mHostPort )
        {
            sFound = STL_TRUE;
            break;   
        }
    }
    
    if( sFound == STL_TRUE )
    {
        sPort = sGroup->mHostPort;
    }
    else
    {
        sPort = gZtnConfigure.mHostPort;
    }
    
    STL_TRY_THROW( sPort < 65536 && 
                   sPort != 0, RAMP_ERR_INVALID_VALUE );
    
    *aPort = sPort;
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTN_ERRCODE_INVALID_PORT,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztnmLogMsg( stlErrorStack  * aErrorStack,
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
    
    STL_TRY( ztnmAddLogMsg( sLogMsg,
                            sLogSize,
                            aErrorStack ) == STL_SUCCESS );
    
    sState = 0;
    stlFreeHeap( sLogMsg );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)stlFreeHeap( sLogMsg );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztnmAddLogMsg( stlChar        * aMsg,
                         stlInt32         aLength,
                         stlErrorStack  * aErrorStack )
{
    stlFile   sFile;
    stlChar   sFileName[STL_MAX_FILE_PATH_LENGTH];
    stlSize   sWrittenBytes;
    stlInt32  sState = 0;
    
    stlSnprintf( sFileName,
                 STL_MAX_FILE_PATH_LENGTH,
                 "%strc"STL_PATH_SEPARATOR"cymonm.trc",
                 gZtnConfigure.mHomeDir );
    
    STL_TRY( stlOpenFile( &sFile,
                          sFileName,
                          STL_FOPEN_CREATE |
                          STL_FOPEN_WRITE  |
                          STL_FOPEN_APPEND,
                          STL_FPERM_OS_DEFAULT,
                          aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztnmWriteTimeStamp( &sFile,
                                 aErrorStack ) == STL_SUCCESS );

    STL_TRY( stlWriteFile( &sFile,
                           (void*)aMsg,
                           aLength,
                           &sWrittenBytes,
                           aErrorStack ) == STL_SUCCESS );
    
    sState = 0;
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


stlStatus ztnmWriteTimeStamp( stlFile        * aFile,
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


stlStatus ztnmGetIntervalInfo( stlInt16        aGroupId,
                               stlInt64        aRedoLogFileSeq,
                               stlInt32        aRedoLogBlockSeq,
                               stlInt64        aCaptureFileSeq,
                               stlInt32        aCaptureBlockSeq,
                               stlInt64      * aInterval,
                               stlInt64      * aIntervalSize,
                               stlErrorStack * aErrorStack )
{
    stlInt64  sIdx          = 0;
    stlInt64  sLoop         = 0;
    stlInt16  sGroupId      = 0;
    stlInt64  sInterval     = 0;
    stlInt64  sIntervalSize = 0;
    
    if( aCaptureFileSeq > aRedoLogFileSeq )
    {
        sInterval     = 0;
        sIntervalSize = 0;
           
        STL_THROW( RAMP_DONE );
    }
    else
    {
        if( aCaptureFileSeq == aRedoLogFileSeq )
        {
            if( aCaptureBlockSeq >= aRedoLogBlockSeq )
            {
                sInterval     = 0;
                sIntervalSize = 0;
           
                STL_THROW( RAMP_DONE );
            }
            else
            {
                sInterval     = aRedoLogBlockSeq - aCaptureBlockSeq;
                sIntervalSize = sInterval * ZTN_DEFAULT_LOG_BLOCK_SIZE;
           
                STL_THROW( RAMP_DONE );
            }
        }
    }
    
    /**
     * FILE 하나이상 차이날 경우.
     */
    sLoop    = aRedoLogFileSeq - aCaptureFileSeq - 1;
    sGroupId = aGroupId;
    
    STL_DASSERT( gZtnManager.mLogFileInfo[sGroupId].mGroupId == aGroupId );
    
    sInterval     += aRedoLogBlockSeq;
    sIntervalSize += aRedoLogBlockSeq * ZTN_DEFAULT_LOG_BLOCK_SIZE;
    
    for( sIdx = 0; sIdx < sLoop; sIdx++ )
    {
        sGroupId--;
        if( sGroupId < 0 )
        {
            sGroupId = gZtnManager.mLogFileCount - 1;
        }
        
        sInterval += gZtnManager.mLogFileInfo[sGroupId].mBlockCount;
        sIntervalSize += gZtnManager.mLogFileInfo[sGroupId].mFileSize;
    }
    
    sInterval     += gZtnManager.mLogFileInfo[sGroupId].mBlockCount - aCaptureBlockSeq;
    sIntervalSize += (gZtnManager.mLogFileInfo[sGroupId].mBlockCount - aCaptureBlockSeq) * ZTN_DEFAULT_LOG_BLOCK_SIZE;
        
    STL_RAMP( RAMP_DONE );
    
    *aInterval     = sInterval;
    *aIntervalSize = sIntervalSize;
         
    return STL_SUCCESS;
}


stlStatus ztnmPrintStatus( stlErrorStack * aErrorStack )
{
    stlChar     sFileName[STL_MAX_FILE_PATH_LENGTH];
    stlFile     sFile;
    stlInt32    sState     = 0;
    stlBool     sExist     = STL_FALSE;
    stlBool     sStarted   = STL_FALSE;
    stlOffset   sFileSize  = 0;
    stlOffset   sOffset    = 0;
    stlChar   * sReadBuff  = NULL;
    stlChar   * sEndPtr    = NULL;
    stlInt64    sCheckProcId;
    stlProc     sCheckProc;
    
    STL_TRY( ztnmGetPidFileName( sFileName, 
                                 aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlExistFile( sFileName,
                           &sExist,
                           aErrorStack ) == STL_SUCCESS );    
    
    stlPrintf("======================================\n");
    stlPrintf("|             CYMONM STATUS          |\n");
    stlPrintf("======================================\n");
    
    if( sExist == STL_TRUE )
    {
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

        if( sFileSize == 0 )
        {
            sStarted = STL_FALSE;
        }
        else
        {
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
                                    &sCheckProcId, 
                                    aErrorStack ) == STL_SUCCESS );
    
            sCheckProc.mProcID = sCheckProcId;
            
            sState = 2;
            stlFreeHeap( sReadBuff );
            
            if( stlKillProc( &sCheckProc,
                             0,           //NULL Signal, Error Check
                             aErrorStack ) == STL_SUCCESS )
            {
                sStarted = STL_TRUE;
            }
        }
        
        sState = 1;
        STL_TRY( stlUnlockFile( &sFile, 
                                aErrorStack ) == STL_SUCCESS );
        sState = 0;
        STL_TRY( stlCloseFile( &sFile,
                               aErrorStack ) == STL_SUCCESS );
    }
    
    if ( sStarted == STL_FALSE )
    {
        stlPrintf("|      Service is not running...     |\n");
    }
    else
    {
        stlPrintf("|       Service is running...        |\n");
    }
    
    stlPrintf("--------------------------------------\n");
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    switch( sState )
    {
        case 3:
            stlFreeHeap( sReadBuff );
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


stlStatus ztnmSetMonitorCycle( const stlChar * aCycle,
                               stlErrorStack * aErrorStack )
{
    stlChar  * sEndPtr    = NULL;
    stlInt64   sCycle     = 0;
    
    STL_TRY( stlStrToInt64( aCycle,
                            STL_NTS,
                            &sEndPtr,
                            10,
                            &sCycle, 
                            aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( *sEndPtr == 0, RAMP_ERR_INVALID_VALUE ); 
       
    gZtnConfigure.mCycleMs = sCycle * STL_TIME_C(1000);
    
    STL_TRY_THROW( sCycle > 0, RAMP_ERR_INVALID_VALUE );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTN_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "cycle",
                      sCycle );;
    }
    STL_FINISH;
    
    return STL_FAILURE;
}



stlStatus ztnmSetLibConfigToGlobalArea( stlChar       * aLibConfig,
                                        stlErrorStack * aErrorStack )
{
    stlInt32       sLen   = 0;

    sLen = stlStrlen( aLibConfig );

    STL_TRY_THROW( sLen < STL_SIZEOF( gZtnConfigure.mLibConfig ), RAMP_ERR_INVALID_VALUE );

    stlStrcpy( gZtnConfigure.mLibConfig, aLibConfig );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTN_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "LIB_CONFIG" );
    }
    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztnmSetLibConfigToGroupArea( stlChar       * aLibConfig,
                                       stlErrorStack * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  ZTN_ERRCODE_INVALID_VALUE,
                  NULL,
                  aErrorStack,
                  "LIB_CONFIG" );

    return STL_FAILURE;
}

stlStatus ztnmGetLibConfig( stlChar       * aLibConfig,
                            stlInt32        aBuffSize,
                            stlErrorStack * aErrorStack )
{
    stlInt32       sLen   = 0;
    stlChar      * sPos = NULL;

    sLen = stlStrlen( gZtnConfigure.mLibConfig );

    if( sLen == 0 )
    {
        STL_DASSERT( aBuffSize > stlStrlen( ZTN_DEFAULT_LIB_CONFIG ) );

        stlStrcpy( aLibConfig, ZTN_DEFAULT_LIB_CONFIG );
    }
    else
    {
        sPos = gZtnConfigure.mLibConfig;

        /**
         * ['] 제거
         */
        if( sPos[0] == '\'' )
        {
            sPos++;
            sLen--;
        }

        sLen = stlSnprintf( aLibConfig,
                            aBuffSize,
                            "%s",
                            sPos );

        if( aLibConfig[ sLen - 1 ] == '\'' )
        {
            aLibConfig[ sLen - 1 ] = 0x00;
            sLen--;
        }

        STL_TRY_THROW( sLen + 1 < aBuffSize, RAMP_ERR_INVALID_VALUE );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTN_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "LIB_CONFIG" );
    }
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztnmSetMonitorFileToGlobalArea( const stlChar * aMonitorFile,
                                          stlErrorStack * aErrorStack )
{
    stlInt32       sLen   = 0;

    sLen = stlStrlen( aMonitorFile );

    STL_TRY_THROW( sLen < STL_SIZEOF( gZtnConfigure.mStateFile ), RAMP_ERR_INVALID_VALUE );

    stlStrcpy( gZtnConfigure.mStateFile, aMonitorFile );

    STL_TRY( ztnmChangePath( gZtnConfigure.mStateFile,
                             STL_SIZEOF( gZtnConfigure.mStateFile ),
                             aErrorStack ) == STL_SUCCESS );

    ztnmLogMsg( aErrorStack, "Monitoring file [%s] \n", gZtnConfigure.mStateFile );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTN_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "STATE_FILE" );
    }
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztnmGetMonitorFile( stlChar       * aMonitorFile,
                              stlInt32        aBuffSize,
                              stlErrorStack * aErrorStack )
{
    stlInt32       sLen   = 0;
    stlChar      * sPos = NULL;

    sLen = stlStrlen( gZtnConfigure.mStateFile );

    if( sLen == 0 )
    {
        sLen = stlSnprintf( aMonitorFile,
                            aBuffSize,
                            "%strc"STL_PATH_SEPARATOR"%s",
                            gZtnConfigure.mHomeDir,
                            ZTN_DEFAULT_STATE_FILE );

        STL_TRY_THROW( sLen + 1 < aBuffSize, RAMP_ERR_INVALID_VALUE );
    }
    else
    {
        sPos = gZtnConfigure.mStateFile;

        /**
         * ['] 제거
         */
        if( sPos[0] == '\'' )
        {
            sPos++;
            sLen--;
        }

        sLen = stlSnprintf( aMonitorFile,
                            aBuffSize,
                            "%s",
                            sPos );

        if( aMonitorFile[ sLen - 1 ] == '\'' )
        {
            aMonitorFile[ sLen - 1 ] = 0x00;
            sLen--;
        }

        STL_TRY_THROW( sLen + 1 < aBuffSize, RAMP_ERR_INVALID_VALUE );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTN_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "STATE_FILE" );
    }
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztnmSetMonitorCycleMsToGlobalArea( const stlChar * aCycleMs,
                                             stlErrorStack * aErrorStack )
{
    stlChar  * sEndPtr    = NULL;
    stlInt64   sCycleMs   = 0;

    STL_TRY( stlStrToInt64( aCycleMs,
                            STL_NTS,
                            &sEndPtr,
                            10,
                            &sCycleMs,
                            aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( *sEndPtr == 0, RAMP_ERR_INVALID_VALUE );

    gZtnConfigure.mCycleMs = sCycleMs;

    STL_TRY_THROW( sCycleMs > 0, RAMP_ERR_INVALID_VALUE );

    ztnmLogMsg( aErrorStack, "Monitoring Cycle [%ld] millisecond \n", gZtnConfigure.mCycleMs );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTN_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "cycle_ms",
                      sCycleMs );;
    }
    STL_FINISH;

    return STL_FAILURE;
}

stlInt64 ztnmGetMonitorCycleMs()
{
    stlInt64 sRet = 0;

    if( gZtnConfigure.mCycleMs != 0 )
    {
        sRet = gZtnConfigure.mCycleMs;
    }
    else
    {
        sRet = ZTN_DEFAULT_MONITOR_CYCLE_MS;
    }

    return sRet;
}


stlStatus ztnmChangePath( stlChar           * aFilePath,
                          stlInt32            aBuffSize,
                          stlErrorStack     * aErrorStack )
{
    stlInt32   sLen = stlStrlen( aFilePath );
    stlChar  * sPosStart;
    stlChar  * sPosEnd;
    stlInt64   sEnvKeyLen;
    stlChar    sEnvKey[STL_MAX_FILE_PATH_LENGTH];
    stlChar    sEnvValue[STL_MAX_FILE_PATH_LENGTH];
    stlBool    sIsFound;

    sPosStart = stlStrchr( aFilePath, '$' );

    if( sPosStart != NULL )
    {
        sPosEnd = stlStrchr( sPosStart, '/' );
        if( sPosEnd == NULL )
        {
            sPosEnd = &aFilePath[sLen];
        }
        else
        {
            sPosEnd = sPosEnd;
        }

        sEnvKeyLen = sPosEnd - sPosStart;

        STL_TRY_THROW( sEnvKeyLen < STL_MAX_FILE_PATH_LENGTH, RAMP_ERR_OVERFLOW );

        /**
         * sEnvKey에 '$' 는 빼고 저장.
         */
        stlMemcpy( sEnvKey, sPosStart + 1, sEnvKeyLen - 1 );

        STL_TRY( stlGetEnv( sEnvValue,
                            STL_SIZEOF(sEnvValue),
                            sEnvKey,
                            &sIsFound,
                            aErrorStack ) == STL_SUCCESS );

        STL_TRY_THROW( sIsFound == STL_TRUE, RAMP_ERR_NOT_FOUND_ENV );

        STL_TRY_THROW( sLen - sEnvKeyLen + stlStrlen(sEnvValue) < STL_MAX_FILE_PATH_LENGTH,
                       RAMP_ERR_OVERFLOW );

        /**
         * sPosStart ~ sPosEnd까지를 env 값으로 변경한다.
         */
        stlMemmove( sPosStart + stlStrlen(sEnvValue), sPosEnd, stlStrlen(sPosEnd) );
        stlMemcpy( sPosStart, sEnvValue, stlStrlen(sEnvValue) );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTN_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "buffer overflow" );
    }
    STL_CATCH( RAMP_ERR_NOT_FOUND_ENV )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTN_ERRCODE_INVALID_VALUE,
                      NULL,
                      aErrorStack,
                      "[%s] env not found",
                      sEnvValue );
    }
    STL_FINISH;

    return STL_FAILURE;
}



/** @} */
