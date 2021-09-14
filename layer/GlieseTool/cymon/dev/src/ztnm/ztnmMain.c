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

stlGetOptOption gOptOption[] =
{
    { "start",  's',  STL_FALSE,   "Start Cymon" },
    { "stop",   't',  STL_FALSE,   "Stop Cymon" },
    { "status", 'u',  STL_FALSE,   "Status" },
    { "conf",   'c',  STL_TRUE,    "Set Configure FILE" },
    { "key",    'k',  STL_TRUE,    "Encrypt/Decrypt key" },
    { "cycle",  'y',  STL_TRUE,    "Set refresh cycle" },
    { "silent", 'i',  STL_FALSE,   "Don't print message" },
    { "help",   'h',  STL_FALSE,   "Print Help Message" },
    { NULL,    '\0',  STL_FALSE,   NULL } 
}; 

void ztnmPrintHelp()
{
    stlPrintf( "\n" );
    stlShowCopyright();
    stlShowVersionInfo();
    stlPrintf( "\n" );

    stlPrintf( "Usage:\n" );
    stlPrintf( "  cymon {--start|-s --stop|-t} [--status|-u]  [--conf|-c] [--cycle|-y] [--silent|-i]\n" );
    stlPrintf( "  cymon --encrypt PASSWD --key KEY \n\n" );
    stlPrintf(" Options:\n"
              "\n"
              "    --start              %s\n"
              "    --stop               %s\n"
              "    --status             %s\n"
              "    --conf   FILE        %s\n"
              "    --key    KEY         %s\n"
              "    --cycle  SECOND      %s\n"
              "    --silent             %s\n"
              "    --help               %s\n"
              "\n",
              gOptOption[0].mDescription,
              gOptOption[1].mDescription,
              gOptOption[2].mDescription,
              gOptOption[3].mDescription,
              gOptOption[4].mDescription,
              gOptOption[5].mDescription,
              gOptOption[6].mDescription,
              gOptOption[7].mDescription
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
    
    const stlChar * sOptionArg;
    
    stlErrorStack   sErrorStack;
    stlInt32        i;
    stlInt32        sKeyIdx = -1;

    
    STL_INIT_ERROR_STACK( &sErrorStack );
    
    STL_TRY( dtlInitialize() == STL_SUCCESS );
    sState = 1;
    
    stlRegisterErrorTable( STL_ERROR_MODULE_GLIESE_TOOL_CYMON,
                           gZtnErrorTable );
    
    STL_TRY( stlInitGetOption( &sOpt,
                               aArgc,
                               (const stlChar* const *)aArgv,
                               &sErrorStack ) == STL_SUCCESS );

    stlMemset( &gZtnConfigure, 0x00, STL_SIZEOF( ztnConfigure ) );
    stlMemset( &gZtnManager, 0x00, STL_SIZEOF( ztnManager ) );

    /**
     * key parameter를 hide하기 위해서 key parameter index를 찾는다.
     */
    for( i = 0; i < aArgc; i++ )
    {
        if( (stlStrcmp( aArgv[i], "-k" ) == 0) ||
            (stlStrcmp( aArgv[i], "--key" ) == 0) )
        {
            sKeyIdx = i;
            break;
        }
    }
    
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
            case 'k':
                if( stlStrlen( sOptionArg ) < STL_SIZEOF( gZtnConfigure.mKey ) )
                {
                    stlMemset( gZtnConfigure.mKey, 0x00, STL_SIZEOF( gZtnConfigure.mKey ) );
                    stlMemcpy( gZtnConfigure.mKey, sOptionArg, stlStrlen( sOptionArg ) );
                }
                else
                {
                    stlMemcpy( gZtnConfigure.mKey, sOptionArg, STL_SIZEOF( gZtnConfigure.mKey ) );
                }
                break;
            case 'i':
                gZtnConfigure.mIsSilent = STL_TRUE;
                break;
            case 'y':
                STL_TRY( ztnmSetMonitorCycle( sOptionArg,
                                              &sErrorStack ) == STL_SUCCESS );
                break;   
            case 'h':
                sHelp = STL_TRUE;
                break;
            default:
                sInvalidOption = STL_TRUE;
                break;
        }
    }

    /*
     * ps와 같은 tool에서 ID, Password와 같은 정보가 보이지 않게 감춤 처리한다.
     */
    if( sKeyIdx != -1 )
    {
        STL_TRY( stlHideArgument( aArgc,
                                  aArgv,
                                  &sErrorStack,
                                  2,
                                  sKeyIdx,
                                  sKeyIdx + 1)
                 == STL_SUCCESS );
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
        STL_TRY( ztnmInitializeEnv( &sErrorStack ) == STL_SUCCESS );
        sState = 2;
    
        STL_TRY( ztnfReadConfigure( &sErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztnmStart( &sErrorStack ) == STL_SUCCESS );    
    
        sState = 1;
        STL_TRY( ztnmFinalizeEnv( &sErrorStack ) == STL_SUCCESS );
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
        case 2:
            (void) ztnmFinalizeEnv( &sErrorStack );
        case 1:
            (void) dtlTerminate();
        default:
            break;
    }

    return -1;
}


/** @} */
