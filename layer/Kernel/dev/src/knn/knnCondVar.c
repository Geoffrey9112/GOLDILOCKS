/*******************************************************************************
 * knnCondVar.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file knnCondVar.c
 * @brief Kernel Layer Conditional Variable implementation routines
 */

#include <stl.h>
#include <dtl.h>
#include <knlDef.h>
#include <knl.h>
#include <knDef.h>
#include <knaLatch.h>

stlStatus knnInitCondVar( knlCondVar  * aCondVar,
                          knlEnv      * aEnv )
{
    STL_PARAM_VALIDATE( aCondVar != NULL, KNL_ERROR_STACK(aEnv) );

    STL_INIT_SPIN_LOCK( aCondVar->mSpinLock );
    
    STL_RING_INIT_HEADLINK( &aCondVar->mWaiters,
                            STL_OFFSETOF(knlLatchWaiter, mRing) );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

void knnFinCondVar( knlCondVar * aCondVar )
{
    STL_ASSERT( aCondVar->mSpinLock == STL_SPINLOCK_STATUS_INIT );
}

stlStatus knnCondWait( knlCondVar  * aCondVar,
                       stlInterval   aTimeout,
                       stlBool     * aIsTimedOut,
                       knlEnv      * aEnv )
{
    stlUInt64        sDummy = 0;
    knlLatchWaiter * sWaiter = KNL_WAITITEM(aEnv);
    knlLatchWaiter * sMyWaiter;
    stlBool          sFound = STL_FALSE;

    *aIsTimedOut = STL_FALSE;
    
    KNL_ENTER_CRITICAL_SECTION( aEnv );
    
    stlAcquireSpinLock( &aCondVar->mSpinLock, &sDummy );

#ifdef STL_DEBUG
    STL_RING_FOREACH_ENTRY( &aCondVar->mWaiters, sMyWaiter )
    {
        STL_ASSERT( sMyWaiter != sWaiter );
    }
#endif
    
    STL_RING_ADD_LAST( &aCondVar->mWaiters, sWaiter );
    
    stlReleaseSpinLock( &aCondVar->mSpinLock );

    KNL_CHECK_THREAD_CANCELLATION( aEnv );

    if( aTimeout == STL_INFINITE_TIME )
    {
        while( 1 )
        {
            KNL_CHECK_THREAD_CANCELLATION( aEnv );
            
            STL_TRY( stlTimedAcquireSemaphore( &sWaiter->mSem,
                                               KNL_CHECK_CANCELLATION_TIME,
                                               aIsTimedOut,
                                               KNL_ERROR_STACK(aEnv) )
                     == STL_SUCCESS );

            if( *aIsTimedOut == STL_FALSE )
            {
                break;
            }
        }
    }
    else
    {
        if( aTimeout < STL_TIMESLICE_TIME )
        {
            stlBusyWait();
            *aIsTimedOut = STL_TRUE;
        }
        else
        {
            STL_TRY( stlTimedAcquireSemaphore( &sWaiter->mSem,
                                               aTimeout,
                                               aIsTimedOut,
                                               KNL_ERROR_STACK(aEnv) )
                     == STL_SUCCESS );
        }
    }

    if( *aIsTimedOut == STL_FALSE )
    {
#ifdef STL_DEBUG
        stlAcquireSpinLock( &aCondVar->mSpinLock, &sDummy );
        
        STL_RING_FOREACH_ENTRY( &aCondVar->mWaiters, sMyWaiter )
        {
            STL_ASSERT( sMyWaiter != sWaiter );
        }
        
        stlReleaseSpinLock( &aCondVar->mSpinLock );
#endif
        STL_THROW( RAMP_FINISH );
    }

    stlAcquireSpinLock( &aCondVar->mSpinLock, &sDummy );

    STL_RING_FOREACH_ENTRY( &aCondVar->mWaiters, sMyWaiter )
    {
        if( sMyWaiter == sWaiter )
        {
            sFound = STL_TRUE;
            break;
        }
    }
            
    if( sFound == STL_TRUE )
    {
        STL_RING_UNLINK( &aCondVar->mWaiters, sMyWaiter );
                
        stlReleaseSpinLock( &aCondVar->mSpinLock );
    }
    else
    {
        stlReleaseSpinLock( &aCondVar->mSpinLock );

        STL_TRY( stlAcquireSemaphore( &sWaiter->mSem,
                                      KNL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );

        *aIsTimedOut = STL_FALSE;
    }

    STL_RAMP( RAMP_FINISH );

    KNL_LEAVE_CRITICAL_SECTION( aEnv );
    
    return STL_SUCCESS;

    STL_FINISH;

    KNL_LEAVE_CRITICAL_SECTION( aEnv );
    
    return STL_FAILURE;
}

stlStatus knnCondBroadcast( knlCondVar * aCondVar,
                            knlEnv     * aEnv )
{
    knlLatchWaiter * sCurWaiter;
    knlLatchWaiter * sNextWaiter;
    stlUInt32        sState = 0;
    stlUInt64        sDummy = 0;
    
    KNL_ENTER_CRITICAL_SECTION( aEnv );
    
    stlAcquireSpinLock( &aCondVar->mSpinLock, &sDummy );
    sState = 1;

    STL_RING_FOREACH_ENTRY_SAFE( &aCondVar->mWaiters, sCurWaiter, sNextWaiter )
    {
        STL_ASSERT( sCurWaiter != sNextWaiter );
        
        STL_RING_UNLINK( &aCondVar->mWaiters, sCurWaiter );
        
        STL_TRY( stlReleaseSemaphore( &sCurWaiter->mSem,
                                      KNL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );
    }

    sState = 0;
    stlReleaseSpinLock( &aCondVar->mSpinLock );

    KNL_LEAVE_CRITICAL_SECTION( aEnv );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        stlReleaseSpinLock( &aCondVar->mSpinLock );
    }
    
    KNL_LEAVE_CRITICAL_SECTION( aEnv );
    
    return STL_FAILURE;
}

knlFxColumnDesc gKnnSignalColumnDesc[] =
{
    {
        "SIGNAL_NUMBER",
        "signal number",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knnFxSignal, mSignalNumber ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        NULL,
        NULL,
        0,
        0,
        0,
        STL_FALSE  /* nullable */
    }
};


stlStatus knnOpenSignalCallback( void   * aStmt,
                                 void   * aDumpObjHandle,
                                 void   * aPathCtrl,
                                 knlEnv * aEnv )
{
    return STL_SUCCESS;
}

stlStatus knnCloseSignalCallback( void   * aDumpObjHandle,
                                    void   * aPathCtrl,
                                    knlEnv * aEnv )
{
    return STL_SUCCESS;
}

stlStatus knnGetDumpObjectCallback( stlInt64   aTransID,
                                    void     * aStmt,
                                    stlChar  * aDumpObjName,
                                    void    ** aDumpObjHandle,
                                    knlEnv   * aEnv )
{
    stlChar  * sSignalNumberStr;
    stlChar  * sDummy;
    stlInt32   sTokenCount;
    stlInt64   sSignalNumber;
    stlChar  * sEndPtr;
    stlProc    sMyProc;

    *aDumpObjHandle = NULL;
    
    STL_TRY( knlTokenizeNextDumpOption( &aDumpObjName,
                                        &sSignalNumberStr,
                                        &sDummy,
                                        &sDummy,
                                        &sTokenCount,
                                        KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    STL_TRY_THROW( sTokenCount == 1, RAMP_FINISH );

    STL_TRY( stlStrToInt64( sSignalNumberStr,
                            STL_NTS,
                            &sEndPtr,
                            10,
                            &sSignalNumber,
                            KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    STL_TRY( stlGetCurrentProc( &sMyProc,
                                KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    STL_TRY( stlKillProc( &sMyProc,
                          sSignalNumber,
                          KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS);

    STL_RAMP( RAMP_FINISH );

    /**
     * meaningless
     */
    *aDumpObjHandle = (void*)1;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus knnBuildRecordSignalCallback( void              * aDumpObjHandle,
                                        void              * aPathCtrl,
                                        knlValueBlockList * aValueList,
                                        stlInt32            aBlockIdx,
                                        stlBool           * aTupleExist,
                                        knlEnv            * aEnv )
{
    *aTupleExist = STL_FALSE;
    
    return STL_SUCCESS;
}

knlFxTableDesc gKnnSignalTableDesc =
{
    KNL_FXTABLE_USAGE_DUMP_TABLE,
    KNL_STARTUP_PHASE_OPEN,
    knnGetDumpObjectCallback,
    knnOpenSignalCallback,
    knnCloseSignalCallback,
    knnBuildRecordSignalCallback,
    1,
    "D$SIGNAL",
    "send signal",
    gKnnSignalColumnDesc
};

/** @} */
