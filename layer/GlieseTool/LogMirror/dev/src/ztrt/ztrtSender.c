/*******************************************************************************
 * ztrtSender.c
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
 * @file ztrtSender.c
 * @brief GlieseTool LogMirror Sender Thread Routines
 */

#include <goldilocks.h>
#include <ztr.h>

extern stlBool        gRunState;
extern ztrMasterMgr   gMasterMgr;

void * STL_THREAD_FUNC ztrtSenderThread( stlThread * aThread, void * aArg )
{
    stlInt32        sState = 0;
    stlErrorStack   sErrorStack;
    
    STL_INIT_ERROR_STACK( &sErrorStack );
    
    STL_TRY( dtlInitialize() == STL_SUCCESS );
    sState = 1;
    
    STL_RAMP( RAMP_RETRY );
    
    if( ztrsSenderMain( &sErrorStack ) == STL_SUCCESS )
    {
        /**
         * SLAVE가 정상적으로 연결이 끊긴 경우이며, SLAVE를 다시 대기하도록 한다.
         */
        STL_THROW( RAMP_RETRY );
    }
    else
    {
        STL_TRY( gRunState == STL_FALSE );
    }
    
    sState = 0;
    STL_TRY( dtlTerminate() == STL_SUCCESS );
    
    return NULL;

    STL_FINISH;
    
    (void) ztrmLogMsg( &sErrorStack,
                       "[SENDER] Error Occurred.\n" );
    
    ztrmWriteLastError( &sErrorStack );
    
    gRunState = STL_FALSE;
    
    switch( sState )
    {
        case 1:
            (void) dtlTerminate();
        default:
            break;
    }
    
    stlExitThread( aThread, STL_FAILURE, &sErrorStack );
    
    return NULL; 
}

stlStatus ztrtCreateSenderThread( stlErrorStack * aErrorStack )
{
    STL_TRY( stlCreateThread( &(gMasterMgr.mThread),
                              NULL,
                              ztrtSenderThread,
                              NULL,
                              aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztrtJoinSenderThread( stlErrorStack * aErrorStack )
{
    stlStatus  sReturnStatus;
    
    STL_TRY( stlJoinThread( &(gMasterMgr.mThread),
                            &sReturnStatus,
                            aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
