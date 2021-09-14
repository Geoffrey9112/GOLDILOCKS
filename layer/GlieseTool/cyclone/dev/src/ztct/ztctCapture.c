/*******************************************************************************
 * ztctCapture.c
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
 * @file ztctCapture.c
 * @brief GlieseTool Cyclone Capture Thread Routines
 */


#include <goldilocks.h>
#include <ztc.h>

extern stlBool        gRunState;
extern ztcMasterMgr * gMasterMgr;

void * STL_THREAD_FUNC ztctCaptureThread( stlThread * aThread, void * aArg )
{
    stlErrorStack     sErrorStack;
    ztcLogFileInfo  * sLogFileInfo = NULL;
    stlInt32          sState       = 0;
    stlBool           sIsTimeout   = STL_TRUE;
    ztcCaptureMgr   * sCaptureMgr  = &(gMasterMgr->mCaptureMgr);
    
    STL_INIT_ERROR_STACK( &sErrorStack );
    
    STL_TRY( dtlInitialize() == STL_SUCCESS );
    sState = 1;

    /**
     * CAPTURE를 Restart 할 경우 여기 부터 시작한다.
     */
    STL_RAMP( RAMP_RETRY );
    
    sCaptureMgr->mCaptureMonitorStatus = STL_FALSE;
    
    /**
     * Capture를 위한 Transaction 관리자를 초기화 한다.
     */
    STL_TRY( ztccInitializeCapture( &sErrorStack ) == STL_SUCCESS );
    sState = 2;
    
    STL_TRY( ztcmLogMsg( &sErrorStack,
                         "[CAPTURE] Initialize Done.\n" ) == STL_SUCCESS );
    
    gMasterMgr->mCaptureRestart = STL_FALSE;
    
    /**
     * Sender에서 Restart 위치를 보내주면 거기서 부터 Recovery 한후에 capture를 수행한다.
     */
    do
    {
        STL_TRY( stlTimedAcquireSemaphore( &(gMasterMgr->mCaptureStartSem),
                                           STL_SET_SEC_TIME( 1 ),
                                           &sIsTimeout,
                                           &sErrorStack ) == STL_SUCCESS );
        
        STL_TRY_THROW( gRunState == STL_TRUE, RAMP_STOP );
        
    } while( sIsTimeout == STL_TRUE );
    
    STL_TRY( ztcmGetCurrentLogFileInfo( &sLogFileInfo, 
                                        &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcmLogMsg( &sErrorStack,
                         "[CAPTURE] \n"
                         "    FileName      = %s\n"
                         "    FileSeqNo     = %ld\n"
                         "    WrapNo        = %d\n"
                         "    BlockSeq      = %d\n"
                         "    LogNo         = %d\n"
                         "    LastCommitLSN = %ld\n"
                         "    RestartLSN    = %ld\n",
                         sLogFileInfo->mName,
                         sLogFileInfo->mCaptureInfo.mFileSeqNo,
                         sLogFileInfo->mCaptureInfo.mWrapNo,
                         sLogFileInfo->mCaptureInfo.mBlockSeq,
                         sLogFileInfo->mCaptureInfo.mReadLogNo,
                         sLogFileInfo->mCaptureInfo.mLastCommitLSN,
                         sLogFileInfo->mCaptureInfo.mRestartLSN ) == STL_SUCCESS );

    /**
     * Capture의 recovery를 수행한다.
     */ 
    if ( sLogFileInfo->mCaptureInfo.mRestartLSN != ZTC_INVALID_LSN )
    {
        if( ztccRecoveryCapture( sLogFileInfo,
                                 &sErrorStack ) != STL_SUCCESS )
        {
            /**
             * STOP에 의한 Failure이면... 
             */
            if( gRunState == STL_FALSE )
            {
                STL_THROW( RAMP_STOP );
            }
            else
            {
                STL_TRY( STL_FALSE );   
            }
        }
    }
    
    STL_TRY( ztcmLogMsg( &sErrorStack,
                         "[CAPTURE] Capture Start.\n" ) == STL_SUCCESS );
    
    sCaptureMgr->mCaptureMonitorStatus = STL_TRUE;
    
    /**
     * Redo Log File을 캡쳐한다.
     */
    if( ztccCapture( sLogFileInfo,
                     &sErrorStack ) != STL_SUCCESS )
    {
        if( gMasterMgr->mCaptureRestart == STL_TRUE && 
            gRunState == STL_TRUE )
        {
            /**
             * SLAVE가 종료했을 경우에 MASTER는 다시 SLAVE가 접속하기를 대기해야 한다.
             * CAPTURE 정보를 초기화 한다.
             */
            sState = 2;
            STL_TRY( ztccFinalizeCapture( &sErrorStack ) == STL_SUCCESS );
            
            STL_TRY( ztcmLogMsg( &sErrorStack,
                                 "[CAPTURE] STOP & Re-Initialize.\n" ) == STL_SUCCESS );
            
            STL_THROW( RAMP_RETRY );
        }
        
        /**
         * STOP에 의한 Failure이면... 그냥 진행!!!
         */
        STL_TRY( gRunState == STL_FALSE );
    }
    
    STL_RAMP( RAMP_STOP );
    
    sState = 1;
    STL_TRY( ztccFinalizeCapture( &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcmLogMsg( &sErrorStack,
                         "[CAPTURE] Finalize Done.\n" ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( dtlTerminate() == STL_SUCCESS );
    
    return NULL;

    STL_FINISH;
    
    (void) ztcmLogMsg( &sErrorStack,
                       "[CAPTURE] Error Occurred.\n" );
    
    ztcmWriteLastError( &sErrorStack );
    
    /**
     * Capture의 Error Code 세팅해야 함!!
     * Sender에서 Slave에 보낼 수 있도록!!
     */
    
    gRunState = STL_FALSE;
    
    switch( sState )
    {
        case 2:
            (void)ztccFinalizeCapture( &sErrorStack );
        case 1 :
            (void) dtlTerminate();
        default:
            break;
    }
    
    stlExitThread( aThread, STL_FAILURE, &sErrorStack );
    
    return NULL;
    
}

stlStatus ztctCreateCaptureThread( stlErrorStack * aErrorStack )
{
    STL_TRY( stlCreateThread( &(gMasterMgr->mCaptureThread),
                              NULL,
                              ztctCaptureThread,
                              NULL,
                              aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztctJoinCaptureThread( stlErrorStack * aErrorStack )
{
    stlStatus sReturnStatus;

    STL_TRY( stlJoinThread( &(gMasterMgr->mCaptureThread),
                            &sReturnStatus,
                            aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


void * STL_THREAD_FUNC ztctStatusThread( stlThread * aThread, void * aArg )
{
    stlErrorStack     sErrorStack;
    stlInt32          sState       = 0;
    
    STL_INIT_ERROR_STACK( &sErrorStack );
    
    STL_TRY( dtlInitialize() == STL_SUCCESS );
    sState = 1;

    (void) ztccCheckCaptureStatus( &sErrorStack );
    
    sState = 0;
    STL_TRY( dtlTerminate() == STL_SUCCESS );
    
    return NULL;

    STL_FINISH;
    
    switch( sState )
    {
        case 1 :
            (void) dtlTerminate();
        default:
            break;
    }
    
    stlExitThread( aThread, STL_FAILURE, &sErrorStack );
    
    return NULL;
    
}


stlStatus ztctCreateStatusThread( stlErrorStack * aErrorStack )
{
    STL_TRY( stlCreateThread( &(gMasterMgr->mStatusThread),
                              NULL,
                              ztctStatusThread,
                              NULL,
                              aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztctJoinStatusThread( stlErrorStack * aErrorStack )
{
    stlStatus sReturnStatus;

    STL_TRY( stlJoinThread( &(gMasterMgr->mStatusThread),
                            &sReturnStatus,
                            aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
