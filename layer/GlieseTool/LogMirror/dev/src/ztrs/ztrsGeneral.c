/*******************************************************************************
 * ztrsGeneral.c
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
 * @file ztrsGeneral.c
 * @brief GlieseTool LogMirror Sender Routines
 */

#include <goldilocks.h>
#include <ztr.h>

extern stlBool       gRunState;
extern ztrConfigure  gZtrConfigure;
extern ztrMasterMgr  gMasterMgr;

extern ztrCommunicateCallback gZtrCommCallback;

stlStatus ztrsInitializeSender( stlErrorStack * aErrorStack )
{
    /**
     * Slave의 접속을 기다린다.
     */
    STL_TRY( gZtrCommCallback.mCreateListenerFunc( aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztrsFinalizeSender( stlErrorStack * aErrorStack )
{
    STL_TRY( gZtrCommCallback.mDestroyHandleFunc( gMasterMgr.mSenderHandle,
                                                  aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztrsSenderMain( stlErrorStack * aErrorStack )
{
    stlInt32 sState = 0;
    
    /**
     * Initialize
     */
    STL_TRY( ztrsInitializeSender( aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    /**
     * Protocol을 수행한다.
     */
    STL_TRY( ztrsDoProtocol( aErrorStack ) == STL_SUCCESS );
    
    /**
     * Finalize
     */
    sState = 0;
    STL_TRY( ztrsFinalizeSender( aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    switch( sState )
    {
        case 1:
            (void) ztrsFinalizeSender( aErrorStack );  
        default:
            break;
    }
    
    return STL_FAILURE;
}

/** @} */
