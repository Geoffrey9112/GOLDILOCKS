/*******************************************************************************
 * ztlm.c
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

/**
 * @file ztlm.c
 * @brief Gliese Listener Main Routines
 */

#include <stl.h>
#include <ztlDef.h>
#include <ztlc.h>
#include <ztlm.h>

stlLogger gZtlLogger;

extern ztlmTcpFilter   gZtlmTcpFilter;
extern stlChar         gZtlmDataDir[STL_MAX_FILE_PATH_LENGTH];
/**
 * @brief 시스템 Environment
 * @remark startup, shutdown 과 세션 Environment 획득 시 사용된다.
 */
stlChar                 gZtlSynchronizeFilePath[STL_MAX_FILE_PATH_LENGTH];
stlBool                 gZtlSilentMode = STL_FALSE;
stlBool                 gZtlDaemonize = STL_TRUE;
stlBool                 gZtlQuitListen = STL_FALSE;
stlChar                 gZtlUdsPath[STL_MAX_FILE_PATH_LENGTH];
stlChar                 gZtlConfFilePath[STL_MAX_FILE_PATH_LENGTH];
ztlmConfig              gZtlConfig;

stlProc                 gZtlProcess;
stlBool                 gZtlAlreadyStart = STL_FALSE;


/* for test */
const stlChar     * gZtlDispatcherIdxMaxFd;
const stlChar     * gZtlDispatcherMaxFd;


stlGetOptOption gZtlmOptOption[] =
{
    { "silent",      'i',  STL_FALSE,   "don't print message" },
    { "start",       's',  STL_FALSE,   "start listener" },
    { "stop",        'q',  STL_FALSE,   "stop listener" },
    { "status",      'u',  STL_FALSE,   "show listener status" },
    { "no-daemon",   'n',  STL_FALSE,   "listener doesn't daemonize" },
    { "no-show-revision", 'j',  STL_FALSE,   "for test : no show copyright and revision" },
    { "help",        'h',  STL_FALSE,   "show listner help messages" },
    { NULL,          '\0', STL_FALSE,  NULL }
};

void ztlmPrintHelp( stlChar ** aArgv )
{
    stlInt32   i = 0;

    STL_TRY( gZtlSilentMode == STL_FALSE );

    ztlmPrintf("\nUsage:\n"
               "  %s [options]\n\n", aArgv[0] );
    ztlmPrintf("Options:\n");
    ztlmPrintf("\n");
    for(i = 0; gZtlmOptOption[i].mName != NULL; i ++)
    {
        switch( gZtlmOptOption[i].mOptCh )
        {
            case 'n':    /* --no-daemon (hidden) */
            case 'j':    /* --no-show-revision  (hidden) */
                break;
            default :
                ztlmPrintf("  --%-13s%s\n", gZtlmOptOption[i].mName, gZtlmOptOption[i].mDescription);
                break;
        }
    }
    ztlmPrintf("\n");

    return;

    STL_FINISH;

    return;
}

void ztlmPrintErrorStack( stlErrorStack * aErrorStack )
{
    stlInt32 i;
    stlInt32 sErrorCode;
    stlInt32 sExtErrCode;
    stlChar  sSqlState[6];

    STL_TRY( gZtlSilentMode == STL_FALSE );

    ztlmPrintf( "\n" );
    
    for( i = aErrorStack->mTop; i >= 0; i-- )
    {
        sErrorCode = aErrorStack->mErrorData[i].mErrorCode;
        sExtErrCode = stlGetExternalErrorCode( &aErrorStack->mErrorData[i] );
        stlMakeSqlState( sExtErrCode, sSqlState );

        if( aErrorStack->mErrorData[i].mDetailErrorMessage[0] == '\0' )
        {
            ztlmPrintf( "ERR-%s(%d): %s",
                        sSqlState,
                        sErrorCode,
                        aErrorStack->mErrorData[i].mErrorMessage );
        }
        else
        {
            ztlmPrintf( "ERR-%s(%d): %s : %s",
                        sSqlState,
                        sErrorCode,
                        aErrorStack->mErrorData[i].mErrorMessage,
                        aErrorStack->mErrorData[i].mDetailErrorMessage );
        }
        
        ztlmPrintf( "\n" );
    }

    return;

    STL_FINISH;

    return;
}

/**
 * @brief Gliese listener를 startup 한다.
 */
stlStatus ztlmStartup( stlErrorStack * aErrorStack )
{
    stlChar             sProgram[STL_PATH_MAX];
    stlBool             sRunning = STL_FALSE;

    /* CML_UDS_COMMAND_LISTENER_STATUS를 보내고 받기 성공하면 이미 실행중인것임 */
    STL_TRY( ztlcGetStatus( STL_FALSE,       /* PrintStatus */
                            &sRunning,       /* aRunning */
                            aErrorStack )
             == STL_SUCCESS );

    if( sRunning == STL_TRUE )
    {
        /* sendCommand가 성공하면 이미 실행중인것임 */
        STL_THROW( RAMP_ERR_ALREADY_START );
    }
    else
    {
        (void)stlPopError( aErrorStack );
    }

    /* Change the file mode mask */
    /* umask(0); */

    /* Create a new SID for the child process */
    /* sid = setsid(); */

    /* Block Signals */
    STL_TRY( stlSetSignalHandler( STL_SIGNAL_HUP,
                                  STL_SIGNAL_IGNORE,
                                  NULL,  /* aOldHandler */
                                  aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlSetSignalHandler( STL_SIGNAL_TERM,
                                  STL_SIGNAL_IGNORE,
                                  NULL,  /* aOldHandler */
                                  aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlSetSignalHandler( STL_SIGNAL_QUIT,
                                  STL_SIGNAL_IGNORE,
                                  NULL,  /* aOldHandler */
                                  aErrorStack )
             == STL_SUCCESS );

    if( gZtlDaemonize == STL_TRUE )
    {
        STL_TRY( stlSetSignalHandler( STL_SIGNAL_INT,
                                      STL_SIGNAL_IGNORE,
                                      NULL,  /* aOldHandler */
                                      aErrorStack )
                 == STL_SUCCESS );
    }

    STL_TRY( stlSetSignalHandler( STL_SIGNAL_USR2,
                                  STL_SIGNAL_IGNORE,
                                  NULL,  /* aOldHandler */
                                  aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlSetSignalHandler( STL_SIGNAL_CLD,
                                  STL_SIGNAL_IGNORE,
                                  NULL,  /* aOldHandler */
                                  aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlGetCurrentProc( &gZtlProcess,
                                aErrorStack ) == STL_SUCCESS );

    /* sProgram 사용되는데는 없음. stlGetProcName가 성공인지 체크 */
    STL_TRY( stlGetProcName( &gZtlProcess,
                             sProgram,
                             STL_PATH_MAX,
                             aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_ALREADY_START )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTLE_ERRCODE_ALREADY_START,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}


int main( int     aArgc,
          char ** aArgv )
{
    stlInt32                sState = 0;
    stlGetOpt               sOpt;
    stlChar                 sCh;
    const stlChar         * sOptionArg;
    stlErrorStack           sErrorStack;
    stlBool                 sStartCommand = STL_FALSE;
    stlBool                 sStatusCommand = STL_FALSE;
    stlBool                 sHelpCommand = STL_FALSE;
    stlBool                 sQuitCommand = STL_FALSE;
    stlProc                 sProc;
    stlBool                 sIsChild = STL_TRUE;
    stlBool                 sIsFound = STL_FALSE;
    stlBool                 sNoShowRevision = STL_FALSE;

    STL_INIT_ERROR_STACK( &sErrorStack );

    STL_DASSERT( gZtleErrorTable[ZTLE_MAX_ERROR].mErrorMessage == NULL );


    STL_TRY( cmlInitialize() == STL_SUCCESS );
    sState = 1;

    stlRegisterErrorTable( STL_ERROR_MODULE_GLIESE_TOOL_GLISTENER,
                           gZtleErrorTable );

    STL_TRY( stlInitGetOption( &sOpt,
                               aArgc,
                               (const stlChar* const *)aArgv,
                               &sErrorStack )
             == STL_SUCCESS );

    while( STL_TRUE )
    {
        if( stlGetOptionLong( &sOpt,
                              gZtlmOptOption,
                              &sCh,
                              &sOptionArg,
                              &sErrorStack ) != STL_SUCCESS )
        {
            if( stlGetLastErrorCode( &sErrorStack ) != STL_ERRCODE_EOF )
            {
                if( stlGetLastErrorCode( &sErrorStack ) == STL_ERRCODE_INVALID_ARGUMENT )
                {
                    STL_THROW( RAMP_ERR_INVALID_OPTION );
                }
                    
                STL_TRY( STL_FALSE );
            }
                
            stlPopError( &sErrorStack );
            break;
        }
        
        /* sStatus, sQuitCommand, sStart 파라미터는 서로 베타적이어야 함. */
        switch( sCh )
        {
            case 'j':
                sNoShowRevision = STL_TRUE;
                break;
            case 'i':
                gZtlSilentMode = STL_TRUE;
                break;
            case 's':   /* start command */
                /* quit, status와 같이 오면 안됨 */
                STL_TRY_THROW( sQuitCommand == STL_FALSE &&
                               sStatusCommand == STL_FALSE,
                               RAMP_ERR_INVALID_OPTION );

                sStartCommand = STL_TRUE;
                break;
            case 'q':   /* quit command */
                /* start, status와 같이 오면 안됨 */
                STL_TRY_THROW( sStartCommand == STL_FALSE &&
                               sStatusCommand == STL_FALSE,
                               RAMP_ERR_INVALID_OPTION );

                sQuitCommand = STL_TRUE;
                break;
            case 'u':   /* status command */
                /* start, quit와 같이 오면 안됨 */
                STL_TRY_THROW( sStartCommand == STL_FALSE &&
                               sQuitCommand == STL_FALSE,
                               RAMP_ERR_INVALID_OPTION );
                sStatusCommand = STL_TRUE;
                break;
            case 'n':
                gZtlDaemonize = STL_FALSE;
                break;
            case 'h':
                sHelpCommand = STL_TRUE;
                break;
            default:
                STL_THROW( RAMP_ERR_INVALID_OPTION );
                break;
        }
    }

    if( gZtlSilentMode == STL_FALSE && sNoShowRevision == STL_FALSE )
    {
        stlPrintf( "\n" );
        stlShowCopyright();
        stlShowVersionInfo();
        stlPrintf( "\n" );
    }


    STL_TRY( stlGetEnv( gZtlmDataDir,
                        STL_MAX_FILE_PATH_LENGTH,
                        STL_ENV_DB_DATA,
                        &sIsFound,
                        &sErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( sIsFound == STL_TRUE, RAMP_ERROR_ENV_DBHOME );

    STL_TRY( ztlmGetConf( &sErrorStack ) == STL_SUCCESS );

    STL_TRY( ztlcGetUnixDomainPath( gZtlUdsPath,
                                    STL_SIZEOF(gZtlUdsPath),
                                    STL_LISTENER_NAME,
                                    gZtlConfig.mTcpPort,
                                    &sErrorStack )
             == STL_SUCCESS );

    if( sHelpCommand == STL_TRUE )
    {
        /* help 면 help만 보여주고 exit */
        ztlmPrintHelp( aArgv );
    }
    else if( sQuitCommand == STL_TRUE )
    {
        STL_TRY( ztlcRequestQuit( &sErrorStack ) == STL_SUCCESS );
    }
    else if( sStatusCommand == STL_TRUE  )
    {
        STL_TRY( ztlcGetStatus( STL_TRUE,   /* PrintStatus */
                                NULL,       /* aRunning */
                                &sErrorStack )
                 == STL_SUCCESS );
    }
    else if( sStartCommand == STL_TRUE  )
    {
        /**
         * Daemonize
         */
        if( gZtlDaemonize == STL_TRUE )
        {
            STL_TRY( stlForkProc( &sProc, &sIsChild, &sErrorStack ) == STL_SUCCESS );
        }
        else
        {
            /* Debug 등을 위하여 Daemonize를 하지 않을때는 fork를 하지 않는다 */
            sIsChild = STL_TRUE;
        }

        if( sIsChild == STL_TRUE )
        {
            STL_TRY( ztlmStartup( &sErrorStack ) == STL_SUCCESS );

            STL_TRY( stlCreateLogger( &gZtlLogger,
                                      "listener.trc",
                                      gZtlConfig.mLogDir,
                                      STL_FALSE,   /* aNeedLock */
                                      ZTL_MAX_LOGGER_SIZE,
                                      &sErrorStack )
                     == STL_SUCCESS );
            sState = 2;

            /* Do Main Job: Listen */
            STL_TRY( ztlcRun( &sErrorStack ) == STL_SUCCESS );

            sState = 1;
            STL_TRY( stlDestoryLogger( &gZtlLogger,
                                       &sErrorStack )
                     == STL_SUCCESS );
        }
        else
        {
            stlSleep( STL_SET_MSEC_TIME(100) );
        }
    }
    else
    {
        STL_THROW( RAMP_ERR_INVALID_OPTION );
    }

    sState = 0;
    STL_TRY( stlTerminate() == STL_SUCCESS );

    if( gZtlmTcpFilter.mIpAddress != NULL )
    {
        stlFreeHeap( gZtlmTcpFilter.mIpAddress );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_OPTION )
    {
        ztlmPrintHelp( aArgv );
    }

    STL_CATCH( RAMP_ERROR_ENV_DBHOME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTLE_ERRCODE_UNDEFINED_ENV_VARIABLE,
                      NULL,
                      &sErrorStack,
                      STL_ENV_DB_DATA );
    }

    STL_FINISH;

    ztlmPrintErrorStack( &sErrorStack );

    switch(sState)
    {
        case 2:
            (void)stlDestoryLogger( &gZtlLogger, &sErrorStack );
            break;
        case 1:
            (void)stlTerminate();
            break;
        default:
            if( gZtlmTcpFilter.mIpAddress != NULL )
            {
                stlFreeHeap( gZtlmTcpFilter.mIpAddress );
            }
            break;
    }

    return STL_FAILURE;
}


