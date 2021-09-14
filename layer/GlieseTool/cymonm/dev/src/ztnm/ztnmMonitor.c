/*******************************************************************************
 * ztnmMonitor.c
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
 * @file ztnmMonitor.c
 * @brief GlieseTool Cyclone Monitor Routines
 */

#include <goldilocks.h>
#include <ztn.h>

extern ztnManager   gZtnManager;
extern ztnConfigure gZtnConfigure;
extern stlBool      gZtnRunState;
extern stlBool      gZtnPrintStdout;


stlStatus ztnmInitializeMonitor( stlFile          * aFdMonitor,
                                 stlErrorStack    * aErrorStack )
{
    stlInt32      sState = 0;
    stlChar       sFilePath[STL_MAX_FILE_PATH_LENGTH];

    STL_TRY( ztnmGetMonitorFile( sFilePath,
                                 STL_SIZEOF( sFilePath ),
                                 aErrorStack )
             == STL_SUCCESS );


    if( stlOpenFile( aFdMonitor,
                     sFilePath,
                     STL_FOPEN_CREATE | STL_FOPEN_TRUNCATE | STL_FOPEN_WRITE | STL_FOPEN_BINARY,
                     STL_FPERM_OS_DEFAULT,
                     aErrorStack ) != STL_SUCCESS )
    {
        ztnmLogMsg( aErrorStack, "%s file open failed\n", sFilePath );

        STL_TRY( 0 );
    }
    sState = 1;
    ztnmLogMsg( aErrorStack, "[%s] file open ok\n", sFilePath );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)stlCloseFile( aFdMonitor, aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus ztnmFinalizeMonitor( stlFile          * aFdMonitor,
                               stlErrorStack    * aErrorStack )
{
    (void)stlCloseFile( aFdMonitor, aErrorStack );

    return STL_SUCCESS;
}

stlStatus ztnmWriteState( stlFile           * aFd,
                          stlChar           * aState,
                          stlSize             aLen,
                          stlErrorStack     * aErrorStack )
{
    stlSize     sWrittenBytes = 0;
    stlOffset   sOffset = 0;

    STL_TRY( stlSeekFile( aFd,
                          STL_FSEEK_SET,
                          &sOffset,
                          aErrorStack )
             == STL_SUCCESS );


    STL_TRY( stlWriteFile( aFd,
                           aState,
                           aLen,
                           &sWrittenBytes,
                           aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( aLen == sWrittenBytes, RAMP_ERR_WRITE_FILE );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_WRITE_FILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTN_ERRCODE_ERROR_WRITE_FILE,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztnmDoMonitor( stlErrorStack * aErrorStack )
{
    stlChar             sLibConfig[STL_MAX_FILE_PATH_LENGTH];
    stlInt64            sCycleMs = ztnmGetMonitorCycleMs();
    ztclMonitor       * sMonitorContext = NULL;
    char                sData[ZTCL_MONITOR_STATE_SIZE];
    stlInt32            sState = 0;
    stlInt32            sGroupCnt;
    stlChar           * sGroupList[ZTCL_MAX_MONITOR_QUEUE_CNT];
    ztnGroupItem      * sGroup = NULL;
    stlFile             sFile;
    stlChar             sFileName[STL_MAX_FILE_PATH_LENGTH];

    STL_TRY( ztnmInitializeMonitor( &sFile,
                                    aErrorStack )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( ztnmGetLibConfig( sLibConfig,
                               STL_SIZEOF( sLibConfig ),
                               aErrorStack )
             == STL_SUCCESS );
    ztnmLogMsg( aErrorStack, "LibConfigFile %s \n", sLibConfig );

    sGroupCnt = 0;
    STL_RING_FOREACH_ENTRY( &gZtnManager.mGroupList, sGroup )
    {
        sGroupList[sGroupCnt++] = sGroup->mName;

        ztnmLogMsg( aErrorStack, "GroupName %s \n", sGroup->mName );

        if( sGroupCnt == ZTCL_MAX_MONITOR_QUEUE_CNT )
        {
            ztnmLogMsg( aErrorStack, "Overflow Queue count : %s \n", ZTCL_MAX_MONITOR_QUEUE_CNT );
            break;
        }
    }

    STL_TRY_THROW( gZtnLibFunc.mInitializeMonitor( sLibConfig,
                                                   ztnmLogMsgCallback,
                                                   sGroupList,
                                                   sGroupCnt,
                                                   &sMonitorContext )
                   == ZTCL_SUCCESS,
                   RAMP_ERR_COMMUNICATION );
    sState = 2;

    STL_TRY( ztnmGetMonitorFile( sFileName,
                                 STL_SIZEOF( sFileName ),
                                 aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztnmLogMsg( aErrorStack,
                         "STARTUP Done. Output file[%s]\n", sFileName )
             == STL_SUCCESS );

    if( gZtnConfigure.mIsSilent == STL_FALSE )
    {
        stlPrintf( "Cymonm Startup done. Output file [%s] \n", sFileName );
    }

    while( 1 )
    {
        stlMemset( sData, 0x00, ZTCL_MONITOR_STATE_SIZE );
        STL_TRY_THROW( gZtnLibFunc.mGetState( sMonitorContext,
                                              sData )
                       == ZTCL_SUCCESS,
                       RAMP_ERR_COMMUNICATION );
        
        if( gZtnPrintStdout == STL_TRUE )
        {
            printf( "%s\n", sData );
        }

        STL_TRY( ztnmWriteState( &sFile,
                                 sData,
                                 ZTCL_MONITOR_STATE_SIZE,
                                 aErrorStack )
                 == STL_SUCCESS );

        if( gZtnRunState == STL_FALSE )
        {
            break;
        }
        
        stlSleep( STL_SET_MSEC_TIME( sCycleMs ) );
    }
    
    return STL_SUCCESS;   
    
    STL_CATCH( RAMP_ERR_COMMUNICATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTN_ERRCODE_COMMUNICATION_LINK_FAILURE,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)gZtnLibFunc.mFinalizeMonitor( sMonitorContext );
        case 1:
            (void)ztnmFinalizeMonitor( &sFile, aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}






/** @} */
