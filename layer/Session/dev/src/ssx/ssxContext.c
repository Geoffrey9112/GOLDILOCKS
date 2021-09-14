/*******************************************************************************
 * ssxContext.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ssxContext.c 9033 2013-07-17 08:40:27Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file ssxContext.c
 * @brief XA Context Routines
 */

#include <qll.h>
#include <sslDef.h>
#include <sslEnv.h>
#include <ssDef.h>
#include <ssx.h>

extern ssgWarmupEntry  * gSsgWarmupEntry;


stlStatus ssxLatchContext( knlXaContext * aContext,
                           sslEnv       * aEnv )
{
    stlBool  sIsTimedOut;
    
    STL_TRY( knlAcquireLatch( &aContext->mLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ssxUnlatchContext( knlXaContext * aContext,
                             sslEnv       * aEnv )
{
    STL_TRY( knlReleaseLatch( &aContext->mLatch,
                              KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ssxInitContext( knlXaContext * aContext,
                          sslEnv       * aEnv )
{
    STL_TRY( knlInitLatch( &(aContext->mLatch),
                           STL_TRUE,
                           KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlInitCondVar( &(aContext->mCondVar),
                             KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_RING_INIT_DATALINK( aContext,
                            STL_OFFSETOF( knlXaContext, mContextLink ) );
    STL_RING_INIT_DATALINK( aContext,
                            STL_OFFSETOF( knlXaContext, mFreeLink ) );
    
    aContext->mHashValue = 0;
    aContext->mState = KNL_XA_STATE_NOTR;
    aContext->mTransId = SML_INVALID_TRANSID;
    aContext->mDeferContext = NULL;
    aContext->mAssociateState = KNL_XA_ASSOCIATE_STATE_TRUE;
    aContext->mHeuristicDecision = KNL_XA_HEURISTIC_DECISION_NONE;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ssxAllocContextArray( sslEnv * aEnv )
{
    ssxContextArray * sContextArray;
    stlInt32          i;
    
    STL_TRY( knlCacheAlignedAllocDynamicMem( &gSsgWarmupEntry->mShmMem,
                                             STL_SIZEOF( ssxContextArray ),
                                             (void**)&sContextArray,
                                             KNL_ENV(aEnv) ) == STL_SUCCESS );

    STL_RING_ADD_LAST( &(gSsgWarmupEntry->mContextArrList), sContextArray );
    
    for( i = 0; i < SSX_CONTEXT_ARRAY_SIZE; i++ )
    {
        STL_TRY( ssxInitContext( &sContextArray->mContext[i],
                                 (sslEnv*)aEnv )
                 == STL_SUCCESS );
        
        STL_RING_ADD_LAST( &(gSsgWarmupEntry->mContextFreeList),
                           &sContextArray->mContext[i] );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ssxCreateContext( stlUInt32       aSessionId,
                            stlXid        * aXid,
                            knlXaContext ** aContext,
                            sslEnv        * aEnv )
{
    knlXaContext * sContext;
    stlBool        sIsTimedOut;
    stlInt32       sState = 0;

    STL_TRY( knlAcquireLatch( &gSsgWarmupEntry->mContextLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;

    if( STL_RING_IS_EMPTY( &gSsgWarmupEntry->mContextFreeList ) == STL_TRUE )
    {
        STL_TRY( ssxAllocContextArray( aEnv ) == STL_SUCCESS );
    }

    sContext = STL_RING_GET_FIRST_DATA( &(gSsgWarmupEntry->mContextFreeList) );
    STL_RING_UNLINK( &(gSsgWarmupEntry->mContextFreeList), sContext );
    
    sState = 0;
    STL_TRY( knlReleaseLatch( &gSsgWarmupEntry->mContextLatch,
                              KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( ssxInitContext( sContext, aEnv ) == STL_SUCCESS );
    
    stlMemcpy( &(sContext->mXid), aXid, STL_SIZEOF(stlXid) );

    sContext->mHashValue = knlGetXidHashValue( aXid );
    sContext->mSessionId = aSessionId;
    *aContext = sContext;
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) knlReleaseLatch( &gSsgWarmupEntry->mContextLatch,
                                KNL_ENV(aEnv) );
    }

    return STL_FAILURE;
}


stlStatus ssxWaitContext( knlXaContext * aContext,
                          stlBool      * aIsSuccess,
                          sslEnv       * aEnv )
{
    stlBool sIsTimedOut;
    
    STL_TRY( knlCondWait( &aContext->mCondVar,
                          STL_SET_SEC_TIME(1),
                          &sIsTimedOut,
                          KNL_ENV(aEnv) )
             == STL_SUCCESS );

    if( sIsTimedOut == STL_TRUE )
    {
        *aIsSuccess = STL_FALSE;
    }
    else
    {
        *aIsSuccess = STL_TRUE;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ssxWakeupContext( knlXaContext * aContext,
                            sslEnv       * aEnv )
{
    STL_TRY( knlCondBroadcast( &aContext->mCondVar,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ssxDestroyContext( knlXaContext * aContext,
                             sslEnv       * aEnv )
{
    stlBool     sIsTimedOut;
    stlInt32    sState = 0;
    
    STL_TRY( knlAcquireLatch( &gSsgWarmupEntry->mContextLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;

    aContext->mState = KNL_XA_STATE_NOTR;
    STL_RING_ADD_LAST( &(gSsgWarmupEntry->mContextFreeList), aContext );
    
    sState = 0;
    STL_TRY( knlReleaseLatch( &gSsgWarmupEntry->mContextLatch,
                              KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    if( sState == 1 )
    {
        (void) knlReleaseLatch( &gSsgWarmupEntry->mContextLatch,
                                KNL_ENV(aEnv) );
    }

    return STL_FAILURE;
}


stlStatus ssxAssociate( stlUInt32      aSessionId,
                        knlXaContext * aContext,
                        sslEnv       * aEnv )
{
    sslSessionEnv * sSslSessionEnv;
    smlSessionEnv * sSmlSessionEnv;
    
    sSslSessionEnv = SSL_SESS_ENV( aEnv );
    sSmlSessionEnv = SML_SESS_ENV( aEnv );

    STL_TRY( qllXaAssociate( aContext, QLL_ENV(aEnv) ) == STL_SUCCESS );
    
    sSslSessionEnv->mTransId = aContext->mTransId;
    sSmlSessionEnv->mTransId = aContext->mTransId;

    aContext->mSessionId = aSessionId;
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ssxDissociate( knlXaContext  * aContext,
                         sslSessionEnv * aSessionEnv,
                         stlBool         aIsCleanUp,
                         sslEnv        * aEnv )
{
    smlSessionEnv * sSmlSessionEnv;
    
    sSmlSessionEnv = (smlSessionEnv*)aSessionEnv;

    if ( aIsCleanUp == STL_TRUE )
    {
        /* ROLLBACK 될 Transaction 이므로 Dissociate 를 수행하지 않는다 */
    }
    else
    {
        STL_TRY( qllXaDissociate( aContext, QLL_ENV(aEnv) ) == STL_SUCCESS );
    }
    
    if( aContext->mAssociateState == KNL_XA_ASSOCIATE_STATE_TRUE )
    {
        aContext->mTransId = aSessionEnv->mTransId;
    }
    
    aSessionEnv->mTransId = SML_INVALID_TRANSID;
    sSmlSessionEnv->mTransId = SML_INVALID_TRANSID;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ssxGetLocalTrans( ssxLocalTrans * sLocalTrans,
                            sslEnv        * aEnv )
{
    sslSessionEnv * sSslSessionEnv;
    smlSessionEnv * sSmlSessionEnv;
    
    sSslSessionEnv = SSL_SESS_ENV( aEnv );
    sSmlSessionEnv = SML_SESS_ENV( aEnv );

    sLocalTrans->mSslTransId = sSslSessionEnv->mTransId;
    sLocalTrans->mSmlTransId = sSmlSessionEnv->mTransId;

    return STL_SUCCESS;
}


stlStatus ssxSetLocalTrans( ssxLocalTrans * sLocalTrans,
                            sslEnv        * aEnv )
{
    sslSessionEnv * sSslSessionEnv;
    smlSessionEnv * sSmlSessionEnv;
    
    sSslSessionEnv = SSL_SESS_ENV( aEnv );
    sSmlSessionEnv = SML_SESS_ENV( aEnv );

    sSslSessionEnv->mTransId = sLocalTrans->mSslTransId;
    sSmlSessionEnv->mTransId = sLocalTrans->mSmlTransId;

    return STL_SUCCESS;
}


stlStatus ssxCommit( smlTransId     aTransId,
                     knlXaContext * aXaContext,
                     sslEnv       * aEnv )
{
    ssxLocalTrans  sLocalTrans;
    
    STL_TRY( ssxGetLocalTrans( &sLocalTrans,
                               aEnv )
             == STL_SUCCESS );
        
    STL_TRY( qllCommit( aTransId,
                        NULL, /* aComment */
                        SML_TRANSACTION_CWM_NONE,
                        aXaContext,
                        QLL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ssxSetLocalTrans( &sLocalTrans,
                               aEnv )
             == STL_SUCCESS );
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ssxRollback( smlTransId     aTransId,
                       knlXaContext * aXaContext,
                       sslEnv       * aEnv )
{
    ssxLocalTrans  sLocalTrans;
    
    STL_TRY( ssxGetLocalTrans( &sLocalTrans,
                               aEnv )
             == STL_SUCCESS );
        
    STL_TRY( qllRollback( aTransId,
                          aXaContext,
                          QLL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ssxSetLocalTrans( &sLocalTrans,
                               aEnv )
             == STL_SUCCESS );
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


