/*******************************************************************************
 * ztrtReceiver.c
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
 * @file ztrtReceiver.c
 * @brief GlieseTool LogMirror Receiver Thread Routines
 */

#include <goldilocks.h>
#include <ztr.h>

extern stlBool     gRunState;
extern ztrSlaveMgr gSlaveMgr;

void * STL_THREAD_FUNC ztrtReceiverThread( stlThread * aThread, void * aArg )
{
    stlInt32        sState = 0;
    stlErrorStack   sErrorStack;
    
    STL_INIT_ERROR_STACK( &sErrorStack );
    
    STL_TRY( dtlInitialize() == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztrmLogMsg( &sErrorStack,
                         "[RECEIVER] START.\n" ) == STL_SUCCESS );
    
    if( ztrrReceiverMain( &sErrorStack ) != STL_SUCCESS )
    {
        STL_TRY( gRunState == STL_FALSE );
    }
    
    sState = 0;
    STL_TRY( dtlTerminate() == STL_SUCCESS );
    
    return NULL;

    STL_FINISH;
    
    (void) ztrmLogMsg( &sErrorStack,
                       "[RECEIVER] Error Occurred.\n" );
    
    ztrmWriteLastError( &sErrorStack );
    
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



stlStatus ztrtCreateReceiverThread( stlErrorStack * aErrorStack )
{
    STL_TRY( stlCreateThread( &(gSlaveMgr.mRcvThread),
                              NULL,
                              ztrtReceiverThread,
                              NULL,
                              aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztrtJoinReceiverThread( stlErrorStack * aErrorStack )
{
    stlStatus  sReturnStatus;
    
    STL_TRY( stlJoinThread( &(gSlaveMgr.mRcvThread),
                            &sReturnStatus,
                            aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}




/** @} */
