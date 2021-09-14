/*******************************************************************************
 * ztymMain.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztymMain.c $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <zty.h>

extern stlErrorRecord gZtyErrorTable[];

stlLogger   * gZtyLogger;
void        * gZtySmrWarmupEntry;
void        * gZtySmfWarmupEntry;
stlBool       gZtySilent = STL_FALSE;
stlBool       gZtyCopyRight = STL_TRUE;

stlGetOptOption gZtyOptOption[] =
{
    { "log",        'l',  STL_FALSE, "Log trace msg" },
    { "silent",     's',  STL_FALSE, "Do not print message" },
    { "copy-right", 'c',  STL_FALSE, "Do not print copy right" },
    { "help",       'h',  STL_FALSE, "Print help message" },
    { NULL,         '\0', STL_FALSE, NULL }
};

void ztymPrintHelp()
{
    stlPrintf( "Usage:\n" );
    stlPrintf( "  gsyncher [options]\n\n" );
    stlPrintf( "  options: \n\n" );
    stlPrintf( "     -l  --log                 %s\n"
               "     -s  --silent              %s\n"
               "     -c  --copy-right          %s\n"
               "     -h  --help                %s\n"
               "\n",
               gZtyOptOption[0].mDescription,
               gZtyOptOption[1].mDescription,
               gZtyOptOption[2].mDescription,
               gZtyOptOption[3].mDescription );
}

stlStatus ztymParseArgs( stlInt32    aArgc,
                         stlChar   * aArgv[],
                         stlLogger * aLogger,
                         smlEnv    * aEnv )
{
    stlGetOpt       sOpt;
    stlChar         sCh;
    const stlChar * sOptionArg;
    stlBool         sInvalidOption = STL_FALSE;
    stlChar         sFilePath[STL_MAX_FILE_PATH_LENGTH];
    
    STL_TRY( stlInitGetOption( &sOpt,
                               aArgc,
                               (const stlChar* const *)aArgv,
                               KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    while( STL_TRUE )
    {
        if( stlGetOptionLong( &sOpt,
                              gZtyOptOption,
                              &sCh,
                              &sOptionArg,
                              KNL_ERROR_STACK(aEnv) ) != STL_SUCCESS )
        {
            if( stlGetLastErrorCode( KNL_ERROR_STACK(aEnv) ) != STL_ERRCODE_EOF )
            {
                if( stlGetLastErrorCode( KNL_ERROR_STACK(aEnv) ) == STL_ERRCODE_INVALID_ARGUMENT )
                {
                    (void)stlPopError( KNL_ERROR_STACK(aEnv) );
                    sInvalidOption = STL_TRUE;
                    break;
                }
                
                STL_TRY( STL_FALSE );
            }
                    
            (void)stlPopError( KNL_ERROR_STACK(aEnv) );
            break;
        }

        switch( sCh )
        {
            case 'l':
                stlMemset( aLogger, 0x00, STL_SIZEOF( stlLogger ) );

                STL_TRY( ztymGetHomePath( sFilePath,
                                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
    
                STL_TRY( stlCreateLogger( aLogger,
                                          "gsyncher.trc",
                                          sFilePath,
                                          STL_FALSE, /* aNeedLock */
                                          ZTY_MAX_LOGGER_SIZE,
                                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

                STL_TRY( ztymPrintStartLogMsg( aLogger,
                                               aEnv ) == STL_SUCCESS );

                STL_TRY( stlLogMsg( aLogger,
                                    KNL_ERROR_STACK(aEnv),
                                    "[INIT] Log started.\n" ) == STL_SUCCESS );

                gZtyLogger = aLogger;
                break;
            case 's':
                gZtySilent = STL_TRUE;
                break;
            case 'c':
                gZtyCopyRight = STL_FALSE;
                break;
            default:
                sInvalidOption = STL_TRUE;
                break;
        }
    }

    if( gZtyCopyRight == STL_TRUE )
    {
        stlPrintf("\n");
        stlShowCopyright();
        stlShowVersionInfo();
    }

    if( sInvalidOption == STL_TRUE )
    {
        ztymPrintHelp();
        STL_TRY( STL_FALSE );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

int main( int    aArgc,
          char * aArgv[] )
{
    stlInt32        sState = 0;
    smlEnv          sEnv;
    smlSessionEnv   sSessionEnv;
    stlShm          sShm;
    stlLogger       sLogger;

    gZtyLogger = NULL;

    STL_TRY( ztymInitialize( &sEnv,
                             &sSessionEnv ) == STL_SUCCESS );
    sState = 1;

    stlRegisterErrorTable( STL_ERROR_MODULE_GLIESE_TOOL_GSYNCHER,
                           gZtyErrorTable );

    STL_TRY( ztymParseArgs( aArgc,
                            aArgv,
                            &sLogger,
                            &sEnv ) == STL_SUCCESS );

    STL_TRY( ztymAttachStaticArea( &sShm,
                                   &sEnv ) == STL_SUCCESS );
    sState = 2;

    /**
     * Log Flush 하기 전에 다른 Application 종료시켜야 한다.
     */
    STL_TRY( ztymClearProcess( &sShm,
                               &sEnv ) == STL_SUCCESS );

    STL_TRY( ztymSetWarmupEntry( &sShm,
                                 &sEnv ) == STL_SUCCESS );

    STL_TRY( ztymFlushAllLogs( &sEnv ) == STL_SUCCESS );

    sState = 1;
    STL_TRY( ztymDetachStaticArea( &sShm,
                                   &sEnv ) == STL_SUCCESS );

    if( gZtySilent == STL_FALSE )
    {
        stlPrintf( "\n[FINI] Log sync complete.\n" );
    }

    if( gZtyLogger != NULL )
    {
        STL_TRY( stlLogMsg( &sLogger,
                            KNL_ERROR_STACK( &sEnv ),
                            "[FINI] Log sync complete.\n" ) == STL_SUCCESS );

        gZtyLogger = NULL;
        
        STL_TRY( stlDestoryLogger( &sLogger,
                                   KNL_ERROR_STACK( &sEnv ) ) == STL_SUCCESS );
    }
    
    sState = 0;
    STL_TRY( ztymFinalize( &sEnv,
                           &sSessionEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) ztymDetachStaticArea( &sShm,
                                         &sEnv );
        case 1:
            if( gZtyLogger != NULL )
            {
                ztymWriteLastError( KNL_ERROR_STACK( &sEnv ) );

                (void)stlLogMsg( &sLogger,
                                 KNL_ERROR_STACK( &sEnv ),
                                 "[FINI] Log sync failed.\n" );

                gZtyLogger = NULL;
                (void) stlDestoryLogger( &sLogger,
                                         KNL_ERROR_STACK( &sEnv ) );
            }

            ztymPrintErrorStack( KNL_ERROR_STACK( &sEnv ) );

            (void) ztymFinalize( &sEnv,
                                 &sSessionEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}
