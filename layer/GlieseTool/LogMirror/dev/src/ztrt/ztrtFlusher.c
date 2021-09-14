/*******************************************************************************
 * ztrtFlusher.c
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
 * @file ztrtFlusher.c
 * @brief GlieseTool LogMirror Flusher Thread Routines
 */

#include <goldilocks.h>
#include <ztr.h>

extern stlBool      gRunState;
extern ztrSlaveMgr  gSlaveMgr;
extern stlBool      gZtrFlusherState;

void * STL_THREAD_FUNC ztrtFlusherThread( stlThread * aThread, void * aArg )
{
    stlInt32        sState = 0;
    stlErrorStack   sErrorStack;
    
    STL_INIT_ERROR_STACK( &sErrorStack );
    
    STL_TRY( dtlInitialize() == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztrmLogMsg( &sErrorStack,
                         "[FLUSHER] START.\n" ) == STL_SUCCESS );
    
    if( ztrfFlusherMain( &sErrorStack ) != STL_SUCCESS )
    {
        STL_TRY( gZtrFlusherState == STL_FALSE );
    }
    
    sState = 0;
    STL_TRY( dtlTerminate() == STL_SUCCESS );
    
    return NULL;

    STL_FINISH;
    
    (void) ztrmLogMsg( &sErrorStack,
                       "[FLUSHER] Error Occurred.\n" );
    
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


stlStatus ztrtCreateFlusherThread( stlErrorStack * aErrorStack )
{
    STL_TRY( stlCreateThread( &(gSlaveMgr.mFshThread),
                              NULL,
                              ztrtFlusherThread,
                              NULL,
                              aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztrtJoinFlusherThread( stlErrorStack * aErrorStack )
{
    stlStatus  sReturnStatus;
    
    STL_TRY( stlJoinThread( &(gSlaveMgr.mFshThread),
                            &sReturnStatus,
                            aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}




/** @} */
