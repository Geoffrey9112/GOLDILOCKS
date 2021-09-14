/*******************************************************************************
 * ztctDistributor.c
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
 * @file ztctApplier.c
 * @brief GlieseTool Cyclone Applier Thread Routines
 */

#include <goldilocks.h>
#include <ztc.h>

extern ztcSlaveMgr * gSlaveMgr;
extern stlBool       gRunState;

void * STL_THREAD_FUNC ztctDistbtThread( stlThread * aThread, void * aArg )
{
    stlErrorStack   sErrorStack;
    stlInt32        sState        = 0;
    stlInt64        sIdx          = 0;
    stlBool         sIsTimeout    = STL_TRUE;
    stlInt64        sApplierCount = gSlaveMgr->mApplierCount;
    
    STL_INIT_ERROR_STACK( &sErrorStack );
    
    STL_TRY( dtlInitialize() == STL_SUCCESS );
    sState = 1;

    /**
     * Distributor Start 대기
     * - Applier가 모두 뜰때까지 대기한다.
     * - Sub Distributor가 뜰때까지 대기해야한다. +1 ...
     */
    for( sIdx = 0; sIdx < sApplierCount + 1; sIdx++ )
    {
        do
        {
            STL_TRY( stlTimedAcquireSemaphore( &(gSlaveMgr->mDistbtStartSem),
                                               STL_SET_SEC_TIME( 1 ),
                                               &sIsTimeout,
                                               &sErrorStack ) == STL_SUCCESS );
        
            STL_TRY_THROW( gRunState == STL_TRUE, RAMP_STOP );
        
        } while( sIsTimeout == STL_TRUE );
    }
    
    STL_TRY( ztcmLogMsg( &sErrorStack,
                         "[DISTRIBUTOR] Initialize Done.\n" ) == STL_SUCCESS );
    
    if( ztcbDistributeFlow( &sErrorStack ) != STL_SUCCESS )
    {
        STL_TRY( gRunState == STL_FALSE );   
    }

    STL_TRY( ztcmLogMsg( &sErrorStack,
                         "[DISTRIBUTOR] Finalize Done.\n" ) == STL_SUCCESS );
    
    STL_RAMP( RAMP_STOP );
    
    sState = 0;
    STL_TRY( dtlTerminate() == STL_SUCCESS );
    
    return NULL;

    STL_FINISH;
    
    (void) ztcmLogMsg( &sErrorStack,
                       "[DISTRIBUTOR] Error Occurred.\n" );
    
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

stlStatus ztctCreateDistbtThread( stlErrorStack * aErrorStack )
{
    STL_TRY( stlCreateThread( &gSlaveMgr->mDistbtThread,
                              NULL,
                              ztctDistbtThread,
                              NULL,
                              aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztctJoinDistbtThread( stlErrorStack * aErrorStack )
{
    stlStatus sReturnStatus;

    STL_TRY( stlJoinThread( &gSlaveMgr->mDistbtThread,
                            &sReturnStatus,
                            aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


void * ztctSubDistbtThread( stlThread * aThread, void * aArg )
{
    stlErrorStack   sErrorStack;
    stlInt32        sState = 0;
    
    STL_INIT_ERROR_STACK( &sErrorStack );
    
    STL_TRY( dtlInitialize() == STL_SUCCESS );
    sState = 1;

    STL_TRY( ztcbInitializeSubDistributor( &sErrorStack ) == STL_SUCCESS );
    sState = 2;
    
    STL_TRY( ztcmLogMsg( &sErrorStack,
                         "[SUB-DISTRIBUTOR] Initialize Done.\n" ) == STL_SUCCESS );
    
    if( ztcbSubDistributeFlow( &sErrorStack ) != STL_SUCCESS )
    {
        STL_TRY( gRunState == STL_FALSE );   
    }
    
    sState = 1;
    STL_TRY( ztcbFinalizeSubDistributor( &sErrorStack ) == STL_SUCCESS );

    STL_TRY( ztcmLogMsg( &sErrorStack,
                         "[SUB-DISTRIBUTOR] Finalize Done.\n" ) == STL_SUCCESS );
    sState = 0;
    STL_TRY( dtlTerminate() == STL_SUCCESS );
    
    return NULL;

    STL_FINISH;
    
    (void) ztcmLogMsg( &sErrorStack,
                       "[SUB-DISTRIBUTOR] Error Occurred.\n" );
    
    ztcmWriteLastError( &sErrorStack );
    
    gRunState = STL_FALSE;
    
    switch( sState )
    {
        case 2:
            (void) ztcbFinalizeSubDistributor( &sErrorStack );
        case 1:
            (void) dtlTerminate();
        default:
            break;
    }
    
    stlExitThread( aThread, STL_FAILURE, &sErrorStack );
    
    return NULL;
    
}

stlStatus ztctCreateSubDistbtThread( stlErrorStack * aErrorStack )
{
    STL_TRY( stlCreateThread( &gSlaveMgr->mSubDistbtThread,
                              NULL,
                              ztctSubDistbtThread,
                              NULL,
                              aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztctJoinSubDistbtThread( stlErrorStack * aErrorStack )
{
    stlStatus sReturnStatus;

    STL_TRY( stlJoinThread( &gSlaveMgr->mSubDistbtThread,
                            &sReturnStatus,
                            aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
