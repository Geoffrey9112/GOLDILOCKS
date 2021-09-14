/*******************************************************************************
 * ztrmMain.c
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
 * @file ztrmMain.c
 * @brief GlieseTool LogMirror Main Routines
 */

#include <goldilocks.h>
#include <ztr.h>

stlBool                  gRunState;

extern ztrConfigure      gZtrConfigure;
extern stlErrorRecord    gZtrErrorTable[];

stlGetOptOption gZtrOptOption[] =
{ 
    { "start",      's', STL_FALSE, "Start LogMirror" },
    { "stop",       't', STL_FALSE, "Stop LogMirror" },
    { "master",     'm', STL_FALSE, "Master Mode" },
    { "slave",      'l', STL_FALSE, "Slave Mode" },
    { "conf",       'c', STL_TRUE,  "Set Configure file" },
    { "infiniband", 'f', STL_FALSE, "Infiniband" },
    { "silent",     'i', STL_FALSE, "Don't print message" },  
    { "help",       'h', STL_FALSE, "Print Help Message" },
    { NULL,        '\0', STL_FALSE, NULL } 
}; 


void ztrmPrintHelp()
{
    stlPrintf( "Usage:\n" );
    stlPrintf( "  LogMirror {--start|-s --stop|-t --master|-m --slave|-l} [--conf|-c] [--silent|-i] \n\n" );
    stlPrintf(" Options:\n"
              "\n"
              "    --start                   %s\n"
              "    --stop                    %s\n"  
              "    --master                  %s\n"
              "    --slave                   %s\n"
              "    --conf     FILE           %s\n"
              "    --infiniband              %s\n"
              "    --silent                  %s\n"
              "    --help                    %s\n"
              "\n",
              gZtrOptOption[0].mDescription,
              gZtrOptOption[1].mDescription,
              gZtrOptOption[2].mDescription,
              gZtrOptOption[3].mDescription,
              gZtrOptOption[4].mDescription,
              gZtrOptOption[5].mDescription,
              gZtrOptOption[6].mDescription,
              gZtrOptOption[7].mDescription
              );
}


void ztrmMasterSignalFunc( stlInt32 aSignalNo )
{
    gRunState = STL_FALSE;
}

void ztrmSlaveSignalFunc( stlInt32 aSignalNo )
{
    gRunState = STL_FALSE;
}

int main( int    aArgc,
          char * aArgv[] )
{
    stlBool           sStart    = STL_FALSE;
    stlBool           sStop     = STL_FALSE;
    stlBool           sIsMaster = STL_FALSE;
    stlBool           sIsSlave  = STL_FALSE;
    stlInt32          sState    = 0;
    
    stlGetOpt         sOpt;
    stlBool           sInvalidOption = STL_FALSE;
    stlChar           sCh;
    const stlChar   * sOptionArg;
    
    stlErrorStack     sErrorStack;
    
    STL_INIT_ERROR_STACK( &sErrorStack );
    
    STL_TRY( dtlInitialize() == STL_SUCCESS );
    sState = 1;
    
    stlRegisterErrorTable( STL_ERROR_MODULE_GLIESE_TOOL_LOGMIRROR,
                           gZtrErrorTable );
    
    stlMemset( &gZtrConfigure, 0x00, STL_SIZEOF( ztrConfigure ) );

    STL_TRY( stlInitGetOption( &sOpt,
                               aArgc,
                               (const stlChar* const *)aArgv,
                               &sErrorStack ) == STL_SUCCESS );
    
    while( STL_TRUE )
    {
        if( stlGetOptionLong( &sOpt,
                              gZtrOptOption,
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
                sStop  = STL_TRUE;
                break;
            case 'm':
                sIsMaster = STL_TRUE;
                break;
            case 'l':
                sIsSlave = STL_TRUE;
                break;
            case 'c':
                STL_TRY( ztrmSetConfigureFile( sOptionArg, 
                                               &sErrorStack ) == STL_SUCCESS );
                break;
            case 'i':
                gZtrConfigure.mIsSilent = STL_TRUE;
                break;
            case 'f':
                gZtrConfigure.mIsInfiniband = STL_TRUE;    
                break;
            default:
                sInvalidOption = STL_TRUE;
                break;
        }
    }
    
    if( ( sInvalidOption == STL_TRUE ) || 
        ( sIsMaster == STL_FALSE && sIsSlave == STL_FALSE ) ||
        ( sIsMaster == STL_TRUE && sIsSlave == STL_TRUE ) || 
        ( sStart == STL_FALSE && sStop == STL_FALSE ) ||
        ( sStart == STL_TRUE && sStop == STL_TRUE ) )
    {
        ztrmPrintHelp();
        STL_THROW( RAMP_EXIT );
    }
    
    STL_TRY( ztrmSetHomeDir( &sErrorStack ) == STL_SUCCESS );
    
    if( sStart == STL_TRUE )
    {
        STL_TRY( ztrmStart( sIsMaster,
                            STL_FALSE,
                            &sErrorStack ) == STL_SUCCESS );
    }
    
    if( sStop == STL_TRUE )
    {
        STL_TRY( ztrmStop( sIsMaster,
                           &sErrorStack ) == STL_SUCCESS );
        
        /**
         * 정상적으로 종료될수 있도록 Sleep을 해준다.
         * 종료후 곧바로 재시작할 경우 Process가 종료 안되어있을 수 있으므로....
         */
        stlSleep( STL_SET_SEC_TIME( 2 ) );
        
        if( gZtrConfigure.mIsSilent == STL_FALSE )  
        {
            stlPrintf( "stop done.\n" );
        }
    }

    
    STL_RAMP( RAMP_EXIT );

    sState = 0;
    STL_TRY( dtlTerminate() == STL_SUCCESS );

    return 0;
    
    STL_FINISH;
    
    (void) ztrmPrintErrorstack( &sErrorStack );
    
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
