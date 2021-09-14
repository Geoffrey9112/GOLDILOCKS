/*******************************************************************************
 * ztrfGeneral.c
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
 * @file ztrfGeneral.c
 * @brief GlieseTool LogMirror Flusher General Routines
 */

#include <goldilocks.h>
#include <ztr.h>

extern stlBool      gRunState;
extern ztrSlaveMgr  gSlaveMgr;
extern stlBool      gZtrFlusherState;

stlStatus ztrfInitializeFlusher( stlErrorStack * aErrorStack )
{
    /**
     * Do Someting..
     */
    
    return STL_SUCCESS;
}

stlStatus ztrfFinalizeFlusher( stlErrorStack * aErrorStack )
{
    /**
     * Do Someting..
     */
    
    return STL_SUCCESS;
}

stlStatus ztrfFlusherMain( stlErrorStack * aErrorStack )
{
    stlInt32  sState = 0;
    
    STL_TRY( ztrfInitializeFlusher( aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztrfDoFlush( aErrorStack ) == STL_SUCCESS );
        
    sState = 0;
    STL_TRY( ztrfFinalizeFlusher( aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    switch( sState )
    {
        case 1:
            (void)ztrfFinalizeFlusher( aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}

/** @} */

