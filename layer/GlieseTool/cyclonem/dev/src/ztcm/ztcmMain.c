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
#include <ztcl.h>
#include <ztc.h>


/**
 * @brief 시스템 Environment
 */

stlBool      gRunState    = STL_FALSE;
stlBool      gRunSyncState = STL_FALSE;
stlBool      gUseUdp      = STL_FALSE;
stlInt32     gZtcLog      = STL_FALSE;
stlInt32     gZtcPerf     = STL_FALSE;
stlUInt64    gCommitIdx    = 0;
stlUInt64    gMsgCnt       = 0;
stlUInt64    gCbCnt        = 0;
stlUInt64    gCommitIdxOld = 0;
stlUInt64    gMsgCntOld    = 0;
stlUInt64    gCbCntOld     = 0;

extern ztcConfigure      gConfigure;
extern ztcSlaveMgr     * gSlaveMgr;
extern stlErrorRecord    gZtcErrorTable[];

ztcLibFunc               gZtcLibFunc;

stlGetOptOption gOptOption[] =
{ 
    { "start",  's',  STL_FALSE,   "Start Cyclonem" },
    { "stop",   't',  STL_FALSE,   "Stop Cyclonem" },
    { "master", 'm',  STL_FALSE,   "Master Mode" },
    { "slave",  'l',  STL_FALSE,   "Slave Mode" },
    { "status", 'u',  STL_FALSE,   "Status" },  
    { "conf",   'c',  STL_TRUE,    "Set Configure FILE" },
    { "silent", 'i',  STL_FALSE,   "Don't print message" }, 
    { "reset",  'r',  STL_FALSE,   "Reset Cyclonem meta" },
    { "sync",   'n',  STL_FALSE,   "Sync data(slave only)" },
    { "file",   'f',  STL_FALSE,   "Apply to file" },
    { "group",  'g',  STL_TRUE,    "Set group name" },
    { "key",    'k',  STL_TRUE,    "Encrypt/Decrypt key" },
    { "encrypt",'e',  STL_TRUE,    "Encrypt Password" },
    { "help",   'h',  STL_FALSE,   "Print Help Message" },
    { "UDP",    'U',  STL_FALSE,   "use UDP for solace test" },
    { "log",    'L',  STL_FALSE,   "log" },
    { "perf",   'P',  STL_FALSE,   "perf" },
    { NULL,    '\0',  STL_FALSE,   NULL } 
}; 

void ztcmPrintHelp()
{
    stlPrintf( "\n" );
    stlShowCopyright();
    stlShowVersionInfo();
    stlPrintf( "\n" );

    stlPrintf( "Usage:\n" );
    stlPrintf( "  cyclonem {--start|-s --stop|-t --master|-m --slave|-l} [--status|-u] "
               "[--conf|-c] [--silent|-i] [--reset|-r] [--sync|-n] [--group|-g]"
               "[--file|-f] [--encrypt|-e] [--key|-k] \n" );
    stlPrintf( "  cyclonem --encrypt PASSWD --key KEY \n\n" );
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
              "    --file               %s\n"
              "    --group  GROUP_NAME  %s\n"
              "    --key    KEY         %s\n"
              "    --encrypt  PASSWD    %s\n"
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
              gOptOption[10].mDescription,
              gOptOption[11].mDescription,
              gOptOption[12].mDescription,
              gOptOption[13].mDescription
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
    stlBool           sEncrypt  = STL_FALSE;
    stlChar           sPasswd[STL_MAX_SQL_IDENTIFIER_LENGTH];
    
    stlGetOpt         sOpt;
    stlBool           sInvalidOption = STL_FALSE;
    stlChar           sCh;
    const stlChar   * sOptionArg;
    stlErrorStack     sErrorStack;
    stlDsoHandle      sLibHandle;
    stlInt32          sKeyIdx = -1;
    stlInt32          i;

    STL_INIT_ERROR_STACK( &sErrorStack );

    stlRegisterErrorTable( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONEM,
                           gZtcErrorTable );

    STL_TRY( dtlInitialize() == STL_SUCCESS );
    sState = 1;


    STL_TRY( stlInitGetOption( &sOpt,
                               aArgc,
                               (const stlChar* const *)aArgv,
                               &sErrorStack )
             == STL_SUCCESS );

    stlMemset( &gConfigure, 0x00, STL_SIZEOF( ztcConfigure ) );
    
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
            case 'f':
                gConfigure.mApplyToFile = STL_TRUE;
                break;
            case 'g':
                STL_TRY( ztcmSetGroupName( sOptionArg,
                                           &sErrorStack ) == STL_SUCCESS );
                break;
            case 'k':
                if( stlStrlen( sOptionArg ) < STL_SIZEOF( gConfigure.mKey ) )
                {
                    stlMemset( gConfigure.mKey, 0x00, STL_SIZEOF( gConfigure.mKey ) );
                    stlMemcpy( gConfigure.mKey, sOptionArg, stlStrlen( sOptionArg ) );
                }
                else
                {
                    stlMemcpy( gConfigure.mKey, sOptionArg, STL_SIZEOF( gConfigure.mKey ) );
                }
                break;
            case 'e':
                sEncrypt  = STL_TRUE;

                STL_TRY_THROW( stlStrlen( sOptionArg ) < STL_SIZEOF( sPasswd ), RAMP_PASSWD_TOO_LONG );
                stlStrcpy( sPasswd, sOptionArg );
                break;
            case 'U':
                gUseUdp = STL_TRUE;
                break;
            case 'L':
                gZtcLog = STL_TRUE;
                break;
            case 'P':
                gZtcPerf = STL_TRUE;
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

    if( sEncrypt == STL_TRUE )
    {
        if( (sStart == STL_TRUE) ||
            (sStop == STL_TRUE) ||
            (sStatus == STL_TRUE) )
        {
            ztcmPrintHelp();
        }
        else
        {
            /**
             * password encrypt해서 출력
             */
            STL_TRY( ztcmPrintCipherText( sPasswd, &sErrorStack ) == STL_SUCCESS );
        }

        STL_THROW( RAMP_EXIT );
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
    
    /**
     * master는 sync모드 사용 불가
     */
    if( (sIsMaster == STL_TRUE) && (ztcmGetSyncMode() == STL_TRUE) )
    {
        ztcmPrintHelp();
        STL_THROW( RAMP_EXIT );
    }


    if( ( sIsMaster == STL_FALSE && sIsSlave == STL_FALSE ) ||
        ( sIsMaster == STL_TRUE && sIsSlave == STL_TRUE ) || 
        ( sStart == STL_FALSE && sStop == STL_FALSE ) ||
        ( sStart == STL_TRUE && sStop == STL_TRUE ) ||
        ( sIsMaster == STL_TRUE && gConfigure.mApplyToFile == STL_TRUE ) ||
        ( ztcmGetSyncMode() == STL_TRUE && gConfigure.mApplyToFile == STL_TRUE ) )
    {
        ztcmPrintHelp();
        STL_THROW( RAMP_EXIT );
    }
    
    gRunState = STL_TRUE;

    if( sStart == STL_TRUE )
    {
        STL_TRY( ztcmOpenLibrary( &sLibHandle,
        						  &sErrorStack )
                 == STL_SUCCESS );
        sState = 2;

        STL_TRY( ztcmStart( sIsMaster,
                            sDoReset,
                            &sErrorStack ) == STL_SUCCESS );

        sState = 1;
        STL_TRY( ztcmCloseLibrary( sLibHandle,
        						   &sErrorStack )
                 == STL_SUCCESS );
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

    STL_CATCH( RAMP_PASSWD_TOO_LONG )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_VALUE_TOO_LONG,
                      NULL,
                      &sErrorStack,
                      "PASSWD");
    }
    STL_FINISH;

    if( gConfigure.mIsSilent == STL_FALSE )  
    {
        ztcmPrintErrorstack( &sErrorStack );
    }
    
    switch( sState )
    {
        case 2:
            ztcmCloseLibrary( sLibHandle, &sErrorStack );
        case 1:
        {
            (void) dtlTerminate();
        }
        default:
            break;
    }

    return -1;
}

/**
 * library 내부에서 호출하는 log 함수
 */
void ztcmLogMsgCallback( const char * aLog )
{
    stlErrorStack  sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    ztcmLogMsg( &sErrorStack, aLog );
}


stlStatus ztcmOpenLibrary( stlDsoHandle   * aZtclLib,
                           stlErrorStack  * aErrorStack )
{
	stlDsoHandle    sZtclLib;
    stlChar 	  * sLibraryPath;

    if( gUseUdp == STL_FALSE )
    {
        sLibraryPath = "libcyclonem.so";
    }
    else
    {
        sLibraryPath = "libcyclonem.udp.so";
    }

    STL_TRY( stlOpenLibrary( sLibraryPath,
                             &sZtclLib,
                             aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlGetSymbol( sZtclLib,
                           "ztclGetVersion",
                           (void **)&gZtcLibFunc.mGetVersion,
                           aErrorStack )
             == STL_SUCCESS );

    /**
     * lib version check
     */
    STL_TRY_THROW( stlStrcmp( gZtcLibFunc.mGetVersion(), ZTCL_API_VERSION ) == 0, RAMP_ERR_LIB_VERSION );

    STL_TRY( stlGetSymbol( sZtclLib,
                           "ztclInitializeSender",
                           (void **)&gZtcLibFunc.mInitializeSender,
                           aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlGetSymbol( sZtclLib,
                           "ztclPubData",
                           (void **)&gZtcLibFunc.mPubData,
                           aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlGetSymbol( sZtclLib,
                           "ztclFinalizeSender",
                           (void **)&gZtcLibFunc.mFinalizeSender,
                           aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlGetSymbol( sZtclLib,
                           "ztclSaveData",
                           (void **)&gZtcLibFunc.mSaveData,
                           aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlGetSymbol( sZtclLib,
                           "ztclLoadData",
                           (void **)&gZtcLibFunc.mLoadData,
                           aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlGetSymbol( sZtclLib,
                           "ztclDeleteSenderQueue",
                           (void **)&gZtcLibFunc.mDeleteSenderQueue,
                           aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlGetSymbol( sZtclLib,
                           "ztclInitializeReceiver",
                           (void **)&gZtcLibFunc.mInitializeReceiver,
                           aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlGetSymbol( sZtclLib,
                           "ztclAckData",
                           (void **)&gZtcLibFunc.mAckData,
                           aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlGetSymbol( sZtclLib,
                           "ztclPauseReceiver",
                           (void **)&gZtcLibFunc.mPauseReceiver,
                           aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlGetSymbol( sZtclLib,
                           "ztclResumeReceiver",
                           (void **)&gZtcLibFunc.mResumeReceiver,
                           aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlGetSymbol( sZtclLib,
                           "ztclFinalizeReceiver",
                           (void **)&gZtcLibFunc.mFinalizeReceiver,
                           aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlGetSymbol( sZtclLib,
                           "ztclDeleteReceiverQueue",
                           (void **)&gZtcLibFunc.mDeleteReceiverQueue,
                           aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlGetSymbol( sZtclLib,
                           "ztclGetState",
                           (void **)&gZtcLibFunc.mGetState,
                           aErrorStack )
                 == STL_SUCCESS );

    STL_TRY( stlGetSymbol( sZtclLib,
                           "ztclGetQueueState",
                           (void **)&gZtcLibFunc.mGetQueueState,
                           aErrorStack )
                 == STL_SUCCESS );

    *aZtclLib = sZtclLib;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_LIB_VERSION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_LIB_VERSION,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztcmCloseLibrary( stlDsoHandle    aZtclLib,
                            stlErrorStack * aErrorStack )
{
    stlMemset( (void *)&gZtcLibFunc, 0x00, STL_SIZEOF( gZtcLibFunc ) );

    STL_TRY( stlCloseLibrary( aZtclLib,
                              aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} */
