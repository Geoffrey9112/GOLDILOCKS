/*******************************************************************************
 * ztnmMain.c
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
 * @file ztnmMain.c
 * @brief GlieseTool Cyclone Monitor Routines
 */

#include <goldilocks.h>
#include <ztn.h>

extern stlErrorRecord    gZtnErrorTable[];
extern ztnConfigure      gZtnConfigure;
extern ztnManager        gZtnManager;
extern stlBool           gZtnRunState;
ztnLibFunc               gZtnLibFunc;
stlBool                  gZtnNoDaemon = STL_FALSE;
stlBool                  gZtnPrintStdout = STL_FALSE;

stlGetOptOption gOptOption[] =
{
    { "start",     's',  STL_FALSE,   "Start Cymonm" },
    { "stop",      't',  STL_FALSE,   "Stop Cymonm" },
    { "status",    'u',  STL_FALSE,   "Status" },
    { "conf",      'c',  STL_TRUE,    "Set Configure FILE" },
    { "silent",    'i',  STL_FALSE,   "Don't print message" },
    { "help",      'h',  STL_FALSE,   "Print Help Message" },
    { "no-daemon", 'X',  STL_FALSE,   "no-daemon" },
    { "stdout",    'P',  STL_FALSE,   "print stdout" },
    { NULL,    '\0',  STL_FALSE,   NULL } 
}; 

void ztnmPrintHelp()
{

    stlPrintf( "\n" );
    stlShowCopyright();
    stlShowVersionInfo();
    stlPrintf( "\n" );

    stlPrintf( "Usage:\n" );
    stlPrintf( "  cymonm {--start|-s --stop|-t} [--status|-u]  [--conf|-c]  [--silent|-i]\n\n" );
    stlPrintf(" Options:\n"
              "\n"
              "    --start                  %s\n"
              "    --stop                   %s\n"
              "    --status                 %s\n"
              "    --conf      FILE         %s\n"
              "    --silent                 %s\n"
              "    --help                   %s\n"
              "\n",
              gOptOption[0].mDescription,
              gOptOption[1].mDescription,
              gOptOption[2].mDescription,
              gOptOption[3].mDescription,
              gOptOption[4].mDescription,
              gOptOption[5].mDescription
              );
}

void ztnmMonitorSignalFunc( stlInt32 aSignalNo )
{
    gZtnRunState = STL_FALSE;
}

int main( int    aArgc,
          char * aArgv[] )
{
    stlInt32        sState    = 0;
    stlGetOpt       sOpt;
    stlBool         sHelp     = STL_FALSE;
    stlBool         sStart    = STL_FALSE;
    stlBool         sStop     = STL_FALSE;
    stlBool         sStatus   = STL_FALSE;
    stlBool         sInvalidOption = STL_FALSE;
    stlChar         sCh;
    stlDsoHandle    sLibHandle;

    const stlChar * sOptionArg;
    
    stlErrorStack   sErrorStack;
    
    STL_INIT_ERROR_STACK( &sErrorStack );
    
    STL_TRY( dtlInitialize() == STL_SUCCESS );
    sState = 1;
    
    stlRegisterErrorTable( STL_ERROR_MODULE_GLIESE_TOOL_CYMONM,
                           gZtnErrorTable );
    
    STL_TRY( stlInitGetOption( &sOpt,
                               aArgc,
                               (const stlChar* const *)aArgv,
                               &sErrorStack ) == STL_SUCCESS );

    stlMemset( &gZtnConfigure, 0x00, STL_SIZEOF( ztnConfigure ) );
    stlMemset( &gZtnManager, 0x00, STL_SIZEOF( ztnManager ) );
    
    while( STL_TRUE )
    {
        if( stlGetOptionLong( &sOpt,
                              gOptOption,
                              &sCh,
                              &sOptionArg,
                              &sErrorStack ) != STL_SUCCESS )
        {
            if( stlGetLastErrorCode( &sErrorStack ) != STL_ERRCODE_EOF )
            {
                if( stlGetLastErrorCode( &sErrorStack ) == STL_ERRCODE_INVALID_ARGUMENT )
                {
                    sInvalidOption = STL_TRUE;
                    (void)stlPopError( &sErrorStack );
                    break;
                }
                
                STL_TRY( STL_FALSE );
            }
                    
            (void)stlPopError( &sErrorStack );
            break;
        }

        switch( sCh )
        {
            case 's':
                sStart = STL_TRUE;
                break;
            case 't':
                sStop = STL_TRUE;
                break;
            case 'u':
                sStatus = STL_TRUE;
                break;
            case 'c':
                STL_TRY( ztnmSetConfigureFileName( sOptionArg, 
                                                   &sErrorStack ) == STL_SUCCESS );
                break;
            case 'i':
                gZtnConfigure.mIsSilent = STL_TRUE;
                break;
            case 'h':
                sHelp = STL_TRUE;
                break;
            case 'P':
                gZtnPrintStdout = STL_TRUE;
                break;
            case 'X':
                gZtnNoDaemon = STL_TRUE;
                break;
            default:
                sInvalidOption = STL_TRUE;
                break;
        }
    }
    
    if( sInvalidOption == STL_TRUE || 
        sHelp          == STL_TRUE )
    {
        ztnmPrintHelp();
        STL_THROW( RAMP_EXIT );
    }
    
    STL_TRY( ztnmSetHomeDir( &sErrorStack ) == STL_SUCCESS );
    
    if( sStatus == STL_TRUE )
    {
        STL_TRY( ztnmPrintStatus( &sErrorStack ) == STL_SUCCESS );
        STL_THROW( RAMP_EXIT );
    }
    
    if( ( sStart == STL_FALSE && sStop == STL_FALSE ) ||
        ( sStart == STL_TRUE && sStop == STL_TRUE ) )
    {
        ztnmPrintHelp();
        STL_THROW( RAMP_EXIT );
    }
    

    if( sStart == STL_TRUE )
    {
        STL_TRY( ztnmOpenLibrary( &sLibHandle,
                                  &sErrorStack )
                 == STL_SUCCESS );
        sState = 2;

        STL_TRY( ztnmInitializeEnv( &sErrorStack ) == STL_SUCCESS );
        sState = 3;
    
        STL_TRY( ztnfReadConfigure( &sErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztnmStart( &sErrorStack ) == STL_SUCCESS );    
    
        sState = 2;
        STL_TRY( ztnmFinalizeEnv( &sErrorStack ) == STL_SUCCESS );

        sState = 1;
        STL_TRY( ztnmCloseLibrary( sLibHandle,
                                   &sErrorStack )
                 == STL_SUCCESS );
    }

    if( sStop == STL_TRUE )
    {
        STL_TRY( ztnmStop( &sErrorStack ) == STL_SUCCESS );
        
        /**
         * 정상적으로 종료될수 있도록 Sleep을 해준다.
         * 종료후 곧바로 재시작할 경우 Process가 종료 안되어있을 수 있으므로....
         */
        stlSleep( STL_SET_SEC_TIME( 2 ) );
        
        if( gZtnConfigure.mIsSilent == STL_FALSE )  
        {
            stlPrintf( "stop done.\n" );
        }
    }
    
    STL_RAMP( RAMP_EXIT );

    sState = 0;
    STL_TRY( dtlTerminate() == STL_SUCCESS );

    return 0;

    STL_FINISH;
    
    ztnmPrintErrorstack( &sErrorStack );
    
    switch( sState )
    {
        case 3:
            (void) ztnmFinalizeEnv( &sErrorStack );
        case 2:
            (void) ztnmCloseLibrary( sLibHandle, &sErrorStack );
        case 1:
            (void) dtlTerminate();
        default:
            break;
    }

    return -1;
}

/**
 * library 내부에서 호출하는 log 함수
 */
void ztnmLogMsgCallback( const char * aLog )
{
    stlErrorStack  sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    ztnmLogMsg( &sErrorStack, aLog );
}



stlStatus ztnmOpenLibrary( stlDsoHandle   * aLibHandle,
                           stlErrorStack  * aErrorStack )
{
	stlDsoHandle    sLibHandle;
    stlChar 	  * sLibraryPath;

    sLibraryPath = "libcyclonem.so";

    STL_TRY( stlOpenLibrary( sLibraryPath,
                             &sLibHandle,
                             aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlGetSymbol( sLibHandle,
                           "ztclGetVersion",
                           (void **)&gZtnLibFunc.mGetVersion,
                           aErrorStack )
             == STL_SUCCESS );

    /**
     * lib version check
     */
    STL_TRY_THROW( stlStrcmp( gZtnLibFunc.mGetVersion(), ZTCL_API_VERSION ) == 0, RAMP_ERR_LIB_VERSION );

    STL_TRY( stlGetSymbol( sLibHandle,
                           "ztclInitializeMonitor",
                           (void **)&gZtnLibFunc.mInitializeMonitor,
                           aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlGetSymbol( sLibHandle,
                           "ztclFinalizeMonitor",
                           (void **)&gZtnLibFunc.mFinalizeMonitor,
                           aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlGetSymbol( sLibHandle,
                           "ztclGetState",
                           (void **)&gZtnLibFunc.mGetState,
                           aErrorStack )
             == STL_SUCCESS );

    *aLibHandle = sLibHandle;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_LIB_VERSION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTN_ERRCODE_INVALID_LIB_VERSION,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztnmCloseLibrary( stlDsoHandle    aLibHandle,
                            stlErrorStack * aErrorStack )
{
    stlMemset( (void *)&gZtnLibFunc, 0x00, STL_SIZEOF( gZtnLibFunc ) );

    STL_TRY( stlCloseLibrary( aLibHandle,
                              aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
