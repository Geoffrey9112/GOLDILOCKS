/*******************************************************************************
 * ztcmMain.c
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
 * @file ztcmMain.c
 * @brief GlieseTool Cyclone Main Routines
 */

#include <goldilocks.h>
#include <ztc.h>

/**
 * @brief 시스템 Environment
 */

stlBool      gRunState    = STL_FALSE;

extern ztcConfigure      gConfigure;
extern ztcMasterMgr    * gMasterMgr;
extern ztcSlaveMgr     * gSlaveMgr;
extern stlErrorRecord    gZtcErrorTable[];

stlGetOptOption gOptOption[] =
{ 
    { "start",  's',  STL_FALSE,   "Start Cyclone" },
    { "stop",   't',  STL_FALSE,   "Stop Cyclone" },
    { "master", 'm',  STL_FALSE,   "Master Mode" },
    { "slave",  'l',  STL_FALSE,   "Slave Mode" },
    { "status", 'u',  STL_FALSE,   "Status" },  
    { "conf",   'c',  STL_TRUE,    "Set Configure FILE" },
    { "silent", 'i',  STL_FALSE,   "Don't print message" }, 
    { "reset",  'r',  STL_FALSE,   "Reset Cyclone meta" },
    { "sync",   'n',  STL_FALSE,   "Sync data" },  
    { "group",  'g',  STL_TRUE,    "Set group name" },
    { "help",   'h',  STL_FALSE,   "Print Help Message" },
    { NULL,    '\0',  STL_FALSE,   NULL } 
}; 

void ztcmPrintHelp()
{
    stlPrintf( "\n" );
    stlShowCopyright();
    stlShowVersionInfo();
    stlPrintf( "\n" );

    stlPrintf( "Usage:\n" );
    stlPrintf( "  cyclone {--start|-s --stop|-t --master|-m --slave|-l} [--status|-u] [--conf|-c] [--silent|-i] [--reset|-r] [--sync|-n] [--group|-g]\n\n" );
    stlPrintf(" Options:\n"
              "\n"
              "    --start              %s\n"
              "    --stop               %s\n"  
              "    --master             %s\n"
              "    --slave              %s\n"
              "    --status             %s\n"
              "    --conf   FILE        %s\n"
              "    --silent             %s\n"
              "    --reset              %s\n"
              "    --sync               %s\n"
              "    --group  GROUP_NAME  %s\n"
              "    --help               %s\n"
              "\n",
              gOptOption[0].mDescription,
              gOptOption[1].mDescription,
              gOptOption[2].mDescription,
              gOptOption[3].mDescription,
              gOptOption[4].mDescription,
              gOptOption[5].mDescription,
              gOptOption[6].mDescription,
              gOptOption[7].mDescription,
              gOptOption[8].mDescription,
              gOptOption[9].mDescription,
              gOptOption[10].mDescription
              );
}

void ztcmMasterSignalFunc( stlInt32 aSignalNo )
{
    gRunState = STL_FALSE;
}

void ztcmSlaveSignalFunc( stlInt32 aSignalNo )
{
    gRunState = STL_FALSE;
}

int main( int    aArgc,
          char * aArgv[] )
{
    stlInt32          sState    = 0;
    stlBool           sStart    = STL_FALSE;
    stlBool           sStop     = STL_FALSE;
    stlBool           sIsMaster = STL_FALSE;
    stlBool           sIsSlave  = STL_FALSE;
    stlBool           sDoReset  = STL_FALSE;
    stlBool           sStatus   = STL_FALSE;
    
    stlGetOpt         sOpt;
    stlBool           sInvalidOption = STL_FALSE;
    stlChar           sCh;
    const stlChar   * sOptionArg;
    
    stlErrorStack     sErrorStack;
    
    STL_INIT_ERROR_STACK( &sErrorStack );
    
    STL_TRY( dtlInitialize() == STL_SUCCESS );
    sState = 1;
    
    stlRegisterErrorTable( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE,
                           gZtcErrorTable );

    STL_TRY( stlInitGetOption( &sOpt,
                               aArgc,
                               (const stlChar* const *)aArgv,
                               &sErrorStack )
             == STL_SUCCESS );

    stlMemset( &gConfigure, 0x00, STL_SIZEOF( ztcConfigure ) );
    
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
                    (void)stlPopError( &sErrorStack );
                    sInvalidOption = STL_TRUE;
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
            case 'm':
                sIsMaster = STL_TRUE;
                break;
            case 'l':
                sIsSlave = STL_TRUE;
                break;
            case 'u':
                sStatus = STL_TRUE;
                break;
            case 'c':
                STL_TRY( ztcmSetConfigureFile( sOptionArg, 
                                               &sErrorStack ) == STL_SUCCESS );
                break;
            case 'i':
                gConfigure.mIsSilent = STL_TRUE;
                break;
            case 'r':
                sDoReset  = STL_TRUE;
                break;
            case 'n':
                ztcmSetSyncMode( STL_TRUE );
                /**
                 * SYNC시에는 RESET 되어야 함..
                 */
                sDoReset  = STL_TRUE;
                break;
            case 'g':
                STL_TRY( ztcmSetGroupName( sOptionArg,
                                           &sErrorStack ) == STL_SUCCESS );
                break;
            default:
                sInvalidOption = STL_TRUE;
                break;
        }
    }

    if( sInvalidOption == STL_TRUE )
    {
        ztcmPrintHelp();
        STL_THROW( RAMP_EXIT );
    }
    
    STL_TRY( ztcmSetHomeDir( &sErrorStack ) == STL_SUCCESS );
    
    if( sStatus == STL_TRUE )
    {
        if( sIsMaster == STL_TRUE )
        {
            STL_TRY( ztcmPrintStatus( STL_TRUE, 
                                      &sErrorStack ) == STL_SUCCESS );
        }
        else if( sIsSlave == STL_TRUE )
        {
            STL_TRY( ztcmPrintStatus( STL_FALSE, 
                                      &sErrorStack ) == STL_SUCCESS );
        }
        else
        {
            ztcmPrintHelp();
        }
        
        STL_THROW( RAMP_EXIT );
    }
    
    if( ( sIsMaster == STL_FALSE && sIsSlave == STL_FALSE ) ||
        ( sIsMaster == STL_TRUE && sIsSlave == STL_TRUE ) || 
        ( sStart == STL_FALSE && sStop == STL_FALSE ) ||
        ( sStart == STL_TRUE && sStop == STL_TRUE ) )
    {
        ztcmPrintHelp();
        STL_THROW( RAMP_EXIT );
    }
    
    if( sStart == STL_TRUE )
    {
        STL_TRY( ztcmStart( sIsMaster,
                            sDoReset,
                            &sErrorStack ) == STL_SUCCESS );    
    }

    if( sStop == STL_TRUE )
    {
        STL_TRY( ztcmStop( sIsMaster,
                           &sErrorStack ) == STL_SUCCESS );
        
        /**
         * 정상적으로 종료될수 있도록 Sleep을 해준다.
         * 종료후 곧바로 재시작할 경우 Process가 종료 안되어있을 수 있으므로....
         */
        stlSleep( STL_SET_SEC_TIME( 2 ) );
        
        if( gConfigure.mIsSilent == STL_FALSE )  
        {
            stlPrintf( "stop done.\n" );
        }
    }

    STL_RAMP( RAMP_EXIT );

    sState = 0;
    STL_TRY( dtlTerminate() == STL_SUCCESS );

    return 0;

    STL_FINISH;

    if( gConfigure.mIsSilent == STL_FALSE )  
    {
        ztcmPrintErrorstack( &sErrorStack );
    }
    
    switch( sState )
    {
        case 1:
        {
            (void) dtlTerminate();
        }
        default:
            break;
    }

    return -1;
}


/** @} */
