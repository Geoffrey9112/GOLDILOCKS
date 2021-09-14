/*******************************************************************************
 * ztcnSync.c
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
 * @file ztcnSync.c
 * @brief GlieseTool Cyclone Sync Routines
 */

#include <goldilocks.h>
#include <ztc.h>

extern stlBool       gRunState;

void * STL_THREAD_FUNC ztctSyncherThread( stlThread * aThread, void * aArg )
{
    stlErrorStack    sErrorStack;
    stlInt32         sState = 0;
    ztcSyncher     * sSyncher = (ztcSyncher*)aArg;
    
    STL_INIT_ERROR_STACK( &sErrorStack );
    
    STL_TRY( dtlInitialize() == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztcnInitializeSyncher( sSyncher,
                                    &sErrorStack ) == STL_SUCCESS );
    sState = 2;
    
    STL_TRY( ztcmLogMsg( &sErrorStack,
                         "[SYNCHER %ld] Initialize Syncher.\n",
                         sSyncher->mSyncherId ) == STL_SUCCESS );
    
    STL_TRY( ztcnDoSyncher( sSyncher,
                            &sErrorStack ) == STL_SUCCESS );
    
    sState = 1;
    STL_TRY( ztcnFinalizeSyncher( sSyncher,
                                  &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcmLogMsg( &sErrorStack,
                         "[SYNCHER %ld] Finalize Syncher.\n",
                         sSyncher->mSyncherId ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( dtlTerminate() == STL_SUCCESS );
    
    return NULL;

    STL_FINISH;
    
    (void) ztcmLogMsg( &sErrorStack,
                       "[SYNCHER %ld] Error Occurred.\n",
                       sSyncher->mSyncherId );
    
    ztcmWriteLastError( &sErrorStack );
    
    gRunState = STL_FALSE;
    
    switch( sState )
    {
        case 1:
            (void) dtlTerminate();
            break;
        default:
            break;
    }
    
    stlExitThread( aThread, STL_FAILURE, &sErrorStack );
    
    return NULL;
}


stlStatus ztctCreateSyncherThread( ztcSyncher    * aSyncher,
                                   stlErrorStack * aErrorStack )
{
    /**
     * Syncher Thread 생성
     */
    STL_TRY( stlCreateThread( &(aSyncher->mThread),
                              NULL,
                              ztctSyncherThread,
                              (void*)aSyncher,
                              aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztctJoinSyncherThread( ztcSyncher    * aSyncher,
                                 stlErrorStack * aErrorStack )
{
    stlStatus        sReturnStatus;
    
    /**
     * Syncher Thread join
     */
    STL_TRY( stlJoinThread( &(aSyncher->mThread),
                            &sReturnStatus,
                            aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;    
}

/** @} */
