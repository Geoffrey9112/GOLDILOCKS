/*******************************************************************************
 * ztctReceiver.c
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
 * @file ztctReceiver.c
 * @brief GlieseTool Cyclone Slave/Receiver Communicator Routines
 */

#include <goldilocks.h>
#include <ztc.h>

extern ztcSlaveMgr  * gSlaveMgr;

void * STL_THREAD_FUNC ztctSubThread( stlThread * aThread, void * aArg )
{
    stlErrorStack   sErrorStack;
    stlInt32        sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    STL_TRY( dtlInitialize() == STL_SUCCESS );
    sState = 1;

    /**
     * Subscriber 생성
     */
    STL_TRY( ztcrCreateSub( &sErrorStack ) == STL_SUCCESS );

    gRunState = STL_FALSE;

    sState = 0;
    STL_TRY( dtlTerminate() == STL_SUCCESS );

    return NULL;

    STL_FINISH;

    (void) ztcmLogMsg( &sErrorStack,
                       "[SUB] Error Occurred.\n" );

    ztcmWriteLastError( &sErrorStack );

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

stlStatus ztctCreateThreadCM4Slave( stlErrorStack * aErrorStack )
{
    stlInt32  sState = 0;
    
    STL_TRY( ztcrInitializeCM4Slave( aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    /**
     * Subscriber Thread 생성
     */
    STL_TRY( stlCreateThread( &(gSlaveMgr->mCMMgr.mSubThread),
                              NULL,
                              ztctSubThread,
                              NULL,
                              aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    switch( sState )
    {
        case 1:
            (void)ztcrFinalizeCM4Slave( aErrorStack );
            break;
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus ztctJoinThreadCM4Slave( stlErrorStack * aErrorStack )
{
    stlStatus     sReturnStatus;
    
    /**
     * Subscriber Thread join
     */
    STL_TRY( stlJoinThread( &(gSlaveMgr->mCMMgr.mSubThread),
                            &sReturnStatus,
                            aErrorStack ) == STL_SUCCESS );

    /**
     * CM Slave Finalize
     */
    STL_TRY( ztcrFinalizeCM4Slave( aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE; 
}

/** @} */
