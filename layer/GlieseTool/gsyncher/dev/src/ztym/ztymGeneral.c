/*******************************************************************************
 * ztymGeneral.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztymGeneral.c $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file ztymGeneral.c
 * @brief gsyncher Internal Routines
 */

#include <sml.h>
#include <zty.h>

extern stlLogger * gZtyLogger;
extern void      * gZtySmrWarmupEntry;
extern void      * gZtySmfWarmupEntry;
extern stlBool     gZtySilent;

/**
 * @brief Initailze
 * @param[in,out] aEnv         storage manager environment
 * @param[in]     aSessionEnv  storage manager session environment
 */
stlStatus ztymInitialize( smlEnv        * aEnv,
                          smlSessionEnv * aSessionEnv )
{
    stlInt32   sState = 0;

    STL_TRY( smlInitialize() == STL_SUCCESS );

    STL_TRY( smlInitializeEnv( aEnv,
                               KNL_ENV_HEAP ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( smlInitializeSessionEnv( aSessionEnv,
                                      KNL_CONNECTION_TYPE_NONE,
                                      KNL_SESSION_TYPE_DEDICATE,
                                      KNL_SESS_ENV_HEAP,
                                      aEnv ) == STL_SUCCESS );
    sState = 2;
    
    KNL_LINK_SESS_ENV( aEnv, aSessionEnv );

    STL_TRY( knlInitializePropertyInHeap( KNL_ENV(aEnv) ) == STL_SUCCESS );
    sState = 3;

    STL_TRY( knlCopyPropertyValueToSessionEnv( KNL_SESS_ENV(aEnv),
                                               KNL_ENV( aEnv ) ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    switch( sState )
    {
        case 3:
            (void)knlFinalizePropertyInHeap( KNL_ENV( aEnv ) );
        case 2:
            (void)smlFinalizeSessionEnv( aSessionEnv, aEnv );
        case 1:
            (void)smlFinalizeEnv( aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}


/**
 * @brief Finalize
 * @param[in] aEnv         storage manager environment
 * @param[in] aSessionEnv  storage manager session environment
 */
stlStatus ztymFinalize( smlEnv        * aEnv,
                        smlSessionEnv * aSessionEnv )
{
    stlInt32  sState = 2;

    STL_TRY( knlFinalizePropertyInHeap( KNL_ENV( aEnv ) ) == STL_SUCCESS );

    sState = 1;
    STL_TRY( smlFinalizeSessionEnv( aSessionEnv, aEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( smlFinalizeEnv( aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)smlFinalizeSessionEnv( aSessionEnv, aEnv );
        case 1:
            (void)smlFinalizeEnv( aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}


/**
 * @brief get trc home directroy path
 * @param[out] aFilePath  file path name
 * @param[in,out] aErrorStack  error stack
 */ 
stlStatus ztymGetHomePath( stlChar       * aFilePath,
                           stlErrorStack * aErrorStack )
{
    stlChar  sPath[STL_MAX_FILE_PATH_LENGTH] = { 0, };
    stlBool  sFound = STL_FALSE;

    STL_TRY( stlGetEnv( sPath,
                        STL_SIZEOF( sPath ),
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
    
    stlSnprintf( aFilePath,
                 STL_MAX_FILE_PATH_LENGTH, 
                 "%s"STL_PATH_SEPARATOR"trc"STL_PATH_SEPARATOR, 
                 sPath );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_HOMEDIR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTY_ERRCODE_INVALID_HOME,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief 로그 파일에 gsyncher start msg를 출력한다.
 * @param[in] aLogger  logger manager
 * @param[in,out] aEnv  environment
 */
stlStatus ztymPrintStartLogMsg( stlLogger * aLogger,
                                smlEnv    * aEnv )
{
    stlFile    sFile;
    stlChar    sFileName[STL_MAX_FILE_PATH_LENGTH];
    stlChar    sBuffer[1024];
    stlChar    sVersionStr[256];
    stlExpTime sExpTime;
    stlInt32   sState = 0;
    stlOffset  sOffset = 0;

    stlSnprintf( sFileName,
                 STL_MAX_FILE_PATH_LENGTH,
                 "%s%s%s",
                 aLogger->mFilePath,
                 STL_PATH_SEPARATOR,
                 aLogger->mFileName );

    STL_TRY( stlOpenFile( &sFile,
                          sFileName,
                          STL_FOPEN_CREATE |
                          STL_FOPEN_WRITE |
                          STL_FOPEN_READ,
                          STL_FPERM_OS_DEFAULT,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_END,
                          &sOffset,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
    
    stlMakeExpTimeByLocalTz(&sExpTime, stlNow() );
    stlGetVersionString( sVersionStr, 256 );
    
    stlSnprintf( sBuffer,
                 1024,
                 "\n\n"
                 "=================================================\n"
                 " gsyncher start\n"
                 " TIME    : %04d-%02d-%02d %02d:%02d:%02d.%06d\n"
                 " VERSION : %s\n"
                 "=================================================\n"
                 "\n",
                 sExpTime.mYear + 1900,
                 sExpTime.mMonth + 1,
                 sExpTime.mDay,
                 sExpTime.mHour,
                 sExpTime.mMinute,
                 sExpTime.mSecond,
                 sExpTime.mMicroSecond,
                 sVersionStr );

    STL_TRY( stlPutStringFile( sBuffer,
                               &sFile,
                               KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    sState = 0;
    STL_TRY( stlCloseFile( &sFile,
                           KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) stlCloseFile( &sFile,
                             KNL_ERROR_STACK( aEnv ) );
    }
    
    return STL_FAILURE;
}

/**
 * @brief Print error stack
 * @param[in] aErrorStack  error stack
 */
void ztymPrintErrorStack( stlErrorStack * aErrorStack )
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
 
        stlPrintf( "\nERR-%s(%d): %s %s\n",
                   sSqlState,
                   sErrorCode,
                   aErrorStack->mErrorData[i].mErrorMessage,
                   aErrorStack->mErrorData[i].mDetailErrorMessage );
        
    }
}


/**
 * @brief error stack의 내용을 log file에 기록
 * @param[in] aErrorStack  error stack
 */
void ztymWriteLastError( stlErrorStack * aErrorStack )
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
        
        (void) stlLogMsg( gZtyLogger,
                          aErrorStack,
                          "ERR-%s(%d): %s %s\n",
                          sSqlState,
                          sErrorCode,
                          aErrorStack->mErrorData[i].mErrorMessage,
                          aErrorStack->mErrorData[i].mDetailErrorMessage );
    }
}


/**
 * @brief shared memory 정보를 얻고 attach
 * @param[out] aShm  Share memory info
 * @param[in,out] aEnv  environment
 */ 
stlStatus ztymAttachStaticArea( stlShm * aShm,
                                smlEnv * aEnv )
{
    cmlShmStaticFileInfo sCmlShmInfo;

    STL_TRY( cmlLoadStaticAreaInfo( &sCmlShmInfo,
                                    KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    STL_TRY( stlAttachShm( aShm,
                           sCmlShmInfo.mName,
                           (stlInt32)sCmlShmInfo.mKey,
                           (void*)sCmlShmInfo.mAddr,
                           KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    if( gZtySilent == STL_FALSE )
    {
        stlPrintf( "\n[SHARED MEM] Attached to shm - Name(%s), Key(%d)\n",
                   aShm->mName,
                   aShm->mKey );
    }

    if( gZtyLogger != NULL )
    {
        STL_TRY( stlLogMsg( gZtyLogger,
                            KNL_ERROR_STACK(aEnv),
                            "[SHARED MEM] Attached to shm - Name(%s), Key(%d)\n",
                            aShm->mName,
                            aShm->mKey ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief shared memory detach
 * @param[in] aShm  Share memory info
 * @param[in,out] aEnv  environment
 */ 
stlStatus ztymDetachStaticArea( stlShm * aShm,
                                smlEnv * aEnv )
{
    STL_TRY( stlDetachShm( aShm,
                           KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    if( gZtySilent == STL_FALSE )
    {
        stlPrintf( "\n[SHARED MEM] Detached from shm.\n" );
    }

    if( gZtyLogger != NULL )
    {
        STL_TRY( stlLogMsg( gZtyLogger,
                            KNL_ERROR_STACK(aEnv),
                            "[SHARED MEM] Detached from shm.\n" ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Storage Manager Layer의 Component Entry 들을 설정한다.
 * @param[in] aShm  Share memory info
 * @param[in,out] aEnv  environment
 */ 
stlStatus ztymSetWarmupEntry( stlShm * aShm,
                              smlEnv * aEnv )
{
    void              ** sSmWarmupEntry;
    void              ** sSmComponentEntry;
    
    /**
     * SM Layer의 StorageManagerWarmupEntry 접근
     */
    sSmWarmupEntry = knlGetLayerEntryAddr( aShm->mAddr, STL_LAYER_STORAGE_MANAGER );

    /**
     * SM Layer의 ComponentWarmupEntry 접근
     */
    sSmComponentEntry = *sSmWarmupEntry;

    gZtySmfWarmupEntry = sSmComponentEntry[ZTY_COMPONENT_DATAFILE];

    STL_TRY( smlReproduceRecoveryWarmupEntry( sSmComponentEntry[ZTY_COMPONENT_RECOVERY],
                                              &gZtySmrWarmupEntry,
                                              gZtySmfWarmupEntry,
                                              aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief shared memory에 attach한 process를 종료
 * @param[in] aShm  shared memory
 * @param[in,out] aEnv  environment
 */ 
stlStatus ztymClearProcess( stlShm    * aShm,
                            smlEnv    * aEnv )
{
    void           ** sKnlEntryAddr;
    void            * sKnlEntryPoint;
    knlStartupPhase   sCurrPhase = KNL_STARTUP_PHASE_NONE;
    stlBool           sIsActive = STL_FALSE;

    /**
     * shared memory 상에 stlLayerClass 순서대로 각 Layer의 Entry Point가 있다.
     * KL Layer 접근
     */
    sKnlEntryAddr = knlGetLayerEntryAddr( aShm->mAddr, STL_LAYER_KERNEL );

    sKnlEntryPoint = *sKnlEntryAddr;
    
    sCurrPhase = knlGetStartupPhase( sKnlEntryPoint );

    STL_TRY_THROW( sCurrPhase == KNL_STARTUP_PHASE_OPEN, RAMP_ERR_INVALID_STARTUP_PHASE );

    STL_TRY( knlKillProcesses4Syncher( sKnlEntryPoint,
                                       &sIsActive,
                                       gZtySilent,
                                       gZtyLogger,
                                       KNL_ENV( aEnv ) ) == STL_SUCCESS );

    STL_TRY_THROW( sIsActive == STL_FALSE, RAMP_ERR_GMASTER_ACTIVE );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_STARTUP_PHASE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTY_ERRCODE_INVALID_PHASE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      gPhaseString[sCurrPhase],
                      gPhaseString[KNL_STARTUP_PHASE_OPEN] );
    }

    STL_CATCH( RAMP_ERR_GMASTER_ACTIVE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTY_ERRCODE_GMASTER_ACTIVE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief Log Buffer를 Flush한다.
 * @param[in,out] aEnv
 */ 
stlStatus ztymFlushAllLogs( smlEnv * aEnv )
{
    stlBool sSwitchedLogfile = STL_FALSE;
    
    STL_TRY( smlFlushAllLogs4Syncher( gZtySmrWarmupEntry,
                                      &sSwitchedLogfile,
                                      gZtySilent,
                                      gZtyLogger,
                                      aEnv ) == STL_SUCCESS );

    if( sSwitchedLogfile == STL_TRUE )
    {
        if( gZtySilent == STL_FALSE )
        {
            stlPrintf( "\n[CONTROLFILE] Saving controlfile caused by logfile switching.\n" );
        }

        STL_TRY( smlSaveCtrlFile4Syncher( gZtySmfWarmupEntry,
                                          gZtySmrWarmupEntry,
                                          gZtySilent,
                                          gZtyLogger,
                                          aEnv ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
