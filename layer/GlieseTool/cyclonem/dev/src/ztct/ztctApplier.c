/*******************************************************************************
 * ztctApplier.c
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
extern ztcConfigure  gConfigure;

void * STL_THREAD_FUNC ztctApplierThread( stlThread * aThread, void * aArg )
{
    stlErrorStack   sErrorStack;
    stlInt32        sState      = 0;
    stlBool         sIsTimeout  = STL_TRUE;
    ztcApplierMgr * sApplierMgr = NULL;
    
    STL_INIT_ERROR_STACK( &sErrorStack );
    
    STL_TRY( dtlInitialize() == STL_SUCCESS );
    sState = 1;
    
    sApplierMgr = (ztcApplierMgr*)aArg;
    
    /**
     * Applier Start 대기
     * - Protocol 중에 Master에서 TableID를 Hash를 구성하고 Table 단위로 Prepare를 하는 단계가 있다. 
     * - 따라서 Handshake Protocol이 끝난 이후에 Applyer가 시작되어야 한다.
     */  
    do
    {
        STL_TRY( stlTimedAcquireSemaphore( &(gSlaveMgr->mApplierStartSem),
                                           STL_SET_SEC_TIME( 1 ),
                                           &sIsTimeout,
                                           &sErrorStack ) == STL_SUCCESS );
        STL_TRY_THROW( gRunState == STL_TRUE, RAMP_STOP );
        
    } while( sIsTimeout == STL_TRUE );
    
    /**
     * Applier를 초기화 한다.
     */
    STL_TRY( ztcaInitializeApplier( sApplierMgr,
                                    &sErrorStack ) == STL_SUCCESS );
    sState = 2;
    

    if( ztcaProcessFlow( sApplierMgr, 
                         &sErrorStack ) != STL_SUCCESS )
    {
        STL_TRY( gRunState == STL_FALSE );   
    }

    if( gConfigure.mApplyToFile == STL_TRUE )
    {
        STL_TRY( ztcaStopToFile( sApplierMgr,
                                 &sErrorStack )
                 == STL_SUCCESS );
    }

    sState = 1;
    STL_TRY( ztcaFinalizeApplier( sApplierMgr,
                                  &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcmLogMsg( &sErrorStack,
                         "[APPLIER] Finalize Done.\n" ) == STL_SUCCESS );
    
    STL_RAMP( RAMP_STOP );
    
    sState = 0;
    STL_TRY( dtlTerminate() == STL_SUCCESS );
    
    return NULL;

    STL_FINISH;
    
    (void) ztcmLogMsg( &sErrorStack,
                       "[APPLIER] Error Occurred.\n" );
    
    ztcmWriteLastError( &sErrorStack );
    
    gRunState = STL_FALSE;
    
    switch( sState )
    {
        case 2:
            if( gConfigure.mApplyToFile == STL_TRUE )
            {
                STL_TRY( ztcaStopToFile( sApplierMgr,
                                         &sErrorStack )
                         == STL_SUCCESS );
            }
            (void)ztcaFinalizeApplier( sApplierMgr,
                                       &sErrorStack );
        case 1:
            (void) dtlTerminate();
        default:
            break;
    }
    
    stlExitThread( aThread, STL_FAILURE, &sErrorStack );
    
    return NULL;
    
}

stlStatus ztctCreateApplierThread( stlErrorStack * aErrorStack )
{
    stlInt64  sApplierCount = gSlaveMgr->mApplierCount;
    stlInt64  sIdx          = 0;
    
    for( sIdx = 0; sIdx < sApplierCount; sIdx++ )
    {
        gSlaveMgr->mApplierMgr[ sIdx ].mAppId = sIdx;
        
        STL_TRY( stlCreateThread( &gSlaveMgr->mApplierMgr[ sIdx ].mThread,
                                  NULL,
                                  ztctApplierThread,
                                  &gSlaveMgr->mApplierMgr[ sIdx ],
                                  aErrorStack ) == STL_SUCCESS );
    }

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztctJoinApplierThread( stlErrorStack * aErrorStack )
{
    stlStatus sReturnStatus;
    stlInt64  sApplierCount = gSlaveMgr->mApplierCount;
    stlInt64  sIdx          = 0;
    
    for( sIdx = 0; sIdx < sApplierCount; sIdx++ )
    {
        STL_TRY( stlJoinThread( &gSlaveMgr->mApplierMgr[ sIdx ].mThread,
                                &sReturnStatus,
                                aErrorStack ) == STL_SUCCESS );
    }

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
