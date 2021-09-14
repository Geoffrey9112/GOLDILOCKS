/*******************************************************************************
 * knlSession.c
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
 * @file knlSession.c
 * @brief Kernel Layer Session Routines
 */

#include <knl.h>
#include <knDef.h>
#include <knaLatch.h>
#include <knsSegment.h>
#include <kniProcess.h>

extern knsEntryPoint * gKnEntryPoint;

/**
 * @addtogroup knlSession
 * @{
 */

/**
 * @brief Session Environment Pool을 생성한다.
 * @param[in] aSessionEnvCount 생성될 Session Environment의 갯수
 * @param[in] aSessionEnvSize 생성될 Session Environment의 크기
 * @param[out] aErrorStack 에러 스택
 */
stlStatus knlCreateSessionEnvPool( stlUInt32       aSessionEnvCount,
                                   stlUInt32       aSessionEnvSize,
                                   stlErrorStack * aErrorStack )
{
    void          * sAddr;
    stlSize         sSize;
    knlEnv          sEnv;
    stlBool         sInitedEnv;
    stlErrorData  * sErrorData;
    stlUInt32       sIdx;
    kncManager    * sSessionEnvMgr;
    kncHeader     * sSessionEnvHeader;
    knlSessionEnv * sSessionEnv;
    stlInt64        sCacheAlignedSize = 0;
    stlInt64        sSessionManagerSize = 0;
    stlInt64        sSessionSize = 0;

    sInitedEnv = STL_FALSE;

    STL_PARAM_VALIDATE( gKnEntryPoint != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aSessionEnvCount != 0, aErrorStack );
    STL_PARAM_VALIDATE( aSessionEnvSize  != 0, aErrorStack );
    STL_PARAM_VALIDATE( gKnEntryPoint->mCacheAlignedSize != 0, aErrorStack );

    sCacheAlignedSize = gKnEntryPoint->mCacheAlignedSize;

    sSessionManagerSize = STL_ALIGN( STL_SIZEOF(kncManager), sCacheAlignedSize );
    sSessionSize        = STL_ALIGN( (STL_SIZEOF(kncHeader) + aSessionEnvSize),
                                     sCacheAlignedSize );

    sSize = sSessionManagerSize + ( sSessionSize * aSessionEnvCount );

    /*
     * 임시 knlEnv를 초기화 한다.
     */
    STL_TRY( knlInitializeEnv( &sEnv, KNL_ENV_HEAP ) == STL_SUCCESS );
    sInitedEnv = STL_TRUE;

    /*
     * Environment Pool을 생성한다.
     */
    STL_TRY_THROW( knlCacheAlignedAllocFixedStaticArea( sSize,
                                                        &sAddr,
                                                        &sEnv ) == STL_SUCCESS,
                   RAMP_ERR_SESSION );

    stlMemset( sAddr, 0x00, sSize );

    /*
     * Environment 관리자를 초기화한다.
     */
    sSessionEnvMgr            = (kncManager*)sAddr;
    sSessionEnvMgr->mCount    = aSessionEnvCount;
    sSessionEnvMgr->mSize     = aSessionEnvSize;
    sSessionEnvMgr->mFreeList = sAddr + sSessionManagerSize + sSessionSize;

    STL_TRY_THROW( knlInitLatch( &(sSessionEnvMgr->mLatch),
                                 STL_TRUE,
                                 &sEnv ) == STL_SUCCESS, RAMP_ERR_SESSION );

    /*
     * 생성된 각 Environment를 초기화한다.
     */
    for( sIdx = 0; sIdx < aSessionEnvCount; sIdx++ )
    {
        sSessionEnvHeader = sAddr + sSessionManagerSize + ( sSessionSize * sIdx );
        sSessionEnvHeader->mIdx = sIdx;

        sSessionEnv = (knlSessionEnv*)(((stlChar*)sSessionEnvHeader) + STL_SIZEOF(kncHeader));
        sSessionEnv->mUsed = STL_FALSE;
        sSessionEnv->mDead = STL_FALSE;
        sSessionEnv->mTermination = KNL_TERMINATION_NONE;
        sSessionEnv->mNeedPendingSignal = STL_FALSE;
        sSessionEnv->mPendingSignalNo = -1;
        sSessionEnv->mAccessKey = KNL_SESSION_ACCESS_LEAVE;
        sSessionEnv->mLogonTime = 0;
        sSessionEnv->mOpenFileCount = 0;

        knlInitSessionEvent( &sSessionEnv->mSessionEvent );

        if( sIdx == KNL_ADMIN_SESSION_ID )
        {
            sSessionEnvHeader->mNextFreeLink = NULL;
        }
        else
        {
            if( sIdx == (aSessionEnvCount - 1) )
            {
                sSessionEnvHeader->mNextFreeLink = NULL;
            }
            else
            {
                sSessionEnvHeader->mNextFreeLink = (void*)sSessionEnvHeader + sSessionSize;
            }
        }
    }

    gKnEntryPoint->mSessionEnvManager = sSessionEnvMgr;

    STL_TRY( knlFinalizeEnv( &sEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SESSION )
    {
        sErrorData = stlPopError( KNL_ERROR_STACK( &sEnv ) );

        if( sErrorData != NULL )
        {
            stlPushError( sErrorData->mErrorLevel,
                          sErrorData->mErrorCode,
                          sErrorData->mDetailErrorMessage,
                          aErrorStack );
        }
    }
    
    STL_FINISH;

    if( sInitedEnv == STL_TRUE )
    {
        (void)knlFinalizeEnv( &sEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief Session Environment를 초기화 한다.
 * @param[in]  aSessionEnv  초기화될 Session Environment 포인터
 * @param[in]  aConnectionType Session Connection Type
 * @param[in]  aSessionType Session Type
 * @param[in]  aSessEnvType Session Envrionment Type
 * @param[out] aEnv         Environment 포인터
 */
stlStatus knlInitializeSessionEnv( knlSessionEnv     * aSessionEnv,
                                   knlConnectionType   aConnectionType,
                                   knlSessionType      aSessionType,
                                   knlSessEnvType      aSessEnvType,
                                   knlEnv            * aEnv )
{
    stlUInt32 sMemType;
    stlInt32  sState  = 0;

    aSessionEnv->mConnectionType = aConnectionType;
    aSessionEnv->mSessionType = aSessionType;
    aSessionEnv->mSessEnvType = aSessEnvType;
    
    STL_TRY( knlSetInitializedFlag( aSessionEnv,
                                    STL_LAYER_KERNEL,
                                    aEnv )
             == STL_SUCCESS );
        
    if( aSessEnvType == KNL_SESS_ENV_SHARED )
    {
        STL_ASSERT( KNL_IS_SHARED_ENV(aEnv) == STL_TRUE );
    }
    
    aSessionEnv->mTopLayer      = STL_LAYER_KERNEL;
    aSessionEnv->mStatus        = KNL_SESSION_STATUS_CONNECTED;
    aSessionEnv->mTransScope    = KNL_TRANS_SCOPE_NONE;
    aSessionEnv->mOpenFileCount = 0;

    /**
     * initialize SQL Fix Block
     */
    
    STL_RING_INIT_HEADLINK( &(aSessionEnv->mSqlFixList),
                            STL_OFFSETOF( knlSqlFixBlock, mFixBlockLink ) );
    
    stlMemset( &(aSessionEnv->mSqlFixBlock), 0x00, STL_SIZEOF(knlSqlFixBlock) );
    
    aSessionEnv->mSqlFixBlock.mIsDefaultBlock = STL_TRUE;
    STL_RING_INIT_DATALINK( &(aSessionEnv->mSqlFixBlock),
                            STL_OFFSETOF(knlSqlFixBlock, mFixBlockLink) );

    STL_RING_ADD_LAST( &(aSessionEnv->mSqlFixList),
                       &(aSessionEnv->mSqlFixBlock) );

    if( aSessionType == KNL_SESSION_TYPE_DEDICATE )
    {
        sMemType = KNL_MEM_STORAGE_TYPE_HEAP;
    }
    else
    {
        if( aSessEnvType == KNL_SESS_ENV_HEAP )
        {
            sMemType = KNL_MEM_STORAGE_TYPE_HEAP;
        }
        else
        {
            sMemType = KNL_MEM_STORAGE_TYPE_SHM;
        }
    }

    STL_TRY( knlCreateDynamicMem( &aSessionEnv->mParameterMem,
                                  NULL, /* aParentMem */
                                  KNL_ALLOCATOR_KERNEL_BIND_PARAMETER,
                                  sMemType,
                                  KNL_PARAMETER_MEM_INIT_SIZE,
                                  KNL_PARAMETER_MEM_NEXT_SIZE,
                                  NULL, /* aMemController */
                                  aEnv )
             == STL_SUCCESS );
    sState = 1;
                                  
    STL_TRY( knlCreateDynamicMem( &aSessionEnv->mLongVaryingMem,
                                  NULL, /* aParentMem */
                                  KNL_ALLOCATOR_KERNEL_LONG_VARYING,
                                  sMemType,
                                  KNL_LONG_VARYING_MEM_INIT_SIZE,
                                  KNL_LONG_VARYING_MEM_NEXT_SIZE,
                                  NULL, /* aMemController */
                                  aEnv )
             == STL_SUCCESS );
    sState = 2;
    
    STL_TRY( knlCreateRegionMem( &aSessionEnv->mPropertyMem,
                                 KNL_ALLOCATOR_KERNEL_VARCHAR_PROPERTY,
                                 NULL,  /* aParentMem */
                                 sMemType,
                                 KNL_VARCHAR_PROPERTY_MEM_INIT_SIZE,
                                 KNL_VARCHAR_PROPERTY_MEM_NEXT_SIZE,
                                 aEnv )
             == STL_SUCCESS );
    sState = 3;
    
    knlInitSessionEvent( &(aSessionEnv->mSessionEvent) );
    
    /**
     * Shared Memory에서 SessionEnv를 할당받을 경우에만 Property Value를 사용할 수 있도록 한다.
     * - 즉, Shared Memory SessionEnv가 Initialize 돈 이후에만 Property를 사용할 수 있다.
     * Heap Memory에서는 직접 knlCopyPropertyValueToSessionEnv 를 수행하도록 해야 한다.
     */
    if( aSessEnvType == KNL_SESS_ENV_SHARED )
    {
        STL_TRY( knlCopyPropertyValueToSessionEnv( aSessionEnv,
                                                   aEnv )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;
    
    switch( sState )
    {
        case 3:
            (void) knlDestroyRegionMem( &aSessionEnv->mPropertyMem, 
                                        aEnv );
        case 2:
            (void) knlDestroyDynamicMem( &aSessionEnv->mLongVaryingMem, 
                                         aEnv );
        case 1:
            (void) knlDestroyDynamicMem( &aSessionEnv->mParameterMem, 
                                         aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief Session Environment를 정리한다.
 * @param[in] aDeadSessionEnv Dead Session Environment 포인터
 * @param[in] aEnv            Environment 포인터
 */
stlStatus knlCleanupSessionEnv( knlSessionEnv * aDeadSessionEnv,
                                knlEnv        * aEnv )
{
    knlSqlFixBlock * sSqlFixIter;
    knlSqlFixBlock * sSqlFixNextIter;
    stlInt32         i;
    
    if( KNL_REGION_MEMORY_TYPE( &aDeadSessionEnv->mPropertyMem ) == KNL_MEM_STORAGE_TYPE_SHM )
    {
        STL_TRY( knlDestroyRegionMem( &aDeadSessionEnv->mPropertyMem,
                                      aEnv )
                 == STL_SUCCESS );
    }
    
    if( KNL_DYNAMIC_MEMORY_TYPE( &aDeadSessionEnv->mLongVaryingMem ) == KNL_MEM_STORAGE_TYPE_SHM )
    {
        STL_TRY( knlDestroyDynamicMem( &aDeadSessionEnv->mLongVaryingMem,
                                       aEnv )
                 == STL_SUCCESS );
    }
    
    if( KNL_DYNAMIC_MEMORY_TYPE( &aDeadSessionEnv->mParameterMem ) == KNL_MEM_STORAGE_TYPE_SHM )
    {
        STL_TRY( knlDestroyDynamicMem( &aDeadSessionEnv->mParameterMem,
                                       aEnv )
                 == STL_SUCCESS );
    }

    STL_ASSERT( KNL_IS_SHARED_SESS_ENV( aDeadSessionEnv ) == STL_TRUE );

    /**
     * cleanup fixed sql
     */
    
    STL_RING_FOREACH_ENTRY_SAFE( &(aDeadSessionEnv->mSqlFixList),
                                 sSqlFixIter,
                                 sSqlFixNextIter )
    {
        for( i = 0; i < KNL_SQL_FIX_ARRAY_SIZE; i++ )
        {
            if( sSqlFixIter->mSqlArray[i] != NULL )
            {
                STL_TRY( knlUnfixSql( sSqlFixIter->mSqlArray[i],
                                      aEnv )
                         == STL_SUCCESS );
                    
                sSqlFixIter->mSqlArray[i] = NULL;
            }
        }
                     
        if( sSqlFixIter->mIsDefaultBlock == STL_FALSE )
        {
            STL_TRY( knlFreeDynamicMem( &(gKnEntryPoint->mSqlFixMem),
                                        sSqlFixIter,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
                
            STL_RING_UNLINK( &(aDeadSessionEnv->mSqlFixList), sSqlFixIter );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief Layer Session이 초기화되었음을 설정한다.
 * @param[in]  aSessionEnv  Session Environment 포인터
 * @param[in]  aLayerClass  대상 layer
 * @param[out] aEnv         Environment 포인터
 */
stlStatus knlSetInitializedFlag( knlSessionEnv * aSessionEnv,
                                 stlLayerClass   aLayerClass,
                                 knlEnv        * aEnv )
{
    aSessionEnv->mInitialized[aLayerClass] = STL_TRUE;

    return STL_SUCCESS;
}
                            

/**
 * @brief Layer Session이 초기화되었는지 여부를 반환한다.
 * @param[in]  aSessionEnv  Session Environment 포인터
 * @param[in]  aLayerClass  대상 layer
 */
stlBool knlGetInitializedFlag( knlSessionEnv * aSessionEnv,
                               stlLayerClass   aLayerClass )
{
    return aSessionEnv->mInitialized[aLayerClass];
}
                            

/**
 * @brief Session Environment를 종료시킨다.
 * @param[in] aSessionEnv 종료할 Session Environment 포인터
 * @param[in] aEnv        Environment 포인터
 */
stlStatus knlFinalizeSessionEnv( knlSessionEnv * aSessionEnv,
                                 knlEnv        * aEnv )
{
    knlSqlFixBlock * sSqlFixIter;
    knlSqlFixBlock * sSqlFixNextIter;
    stlInt32         i;
    stlInt32         sState = 2;
    
    sState = 2;
    STL_TRY( knlDestroyRegionMem( &aSessionEnv->mPropertyMem,
                                  aEnv )
             == STL_SUCCESS );

    knlValidateDynamicMem( &aSessionEnv->mLongVaryingMem, aEnv );
    sState = 1;
    STL_TRY( knlDestroyDynamicMem( &aSessionEnv->mLongVaryingMem,
                                   aEnv )
             == STL_SUCCESS );
    
    knlValidateDynamicMem( &aSessionEnv->mParameterMem, aEnv );
    sState = 0;
    STL_TRY( knlDestroyDynamicMem( &aSessionEnv->mParameterMem,
                                   aEnv )
             == STL_SUCCESS );
    
    if( KNL_IS_SHARED_SESS_ENV(aSessionEnv) == STL_TRUE )
    {
        STL_ASSERT( KNL_IS_SHARED_ENV(aEnv) == STL_TRUE );
        
        /**
         * cleanup fixed sql
         */
    
        STL_RING_FOREACH_ENTRY_SAFE( &(aSessionEnv->mSqlFixList),
                                     sSqlFixIter,
                                     sSqlFixNextIter )
        {
            for( i = 0; i < KNL_SQL_FIX_ARRAY_SIZE; i++ )
            {
                if( sSqlFixIter->mSqlArray[i] != NULL )
                {
                    STL_TRY( knlUnfixSql( sSqlFixIter->mSqlArray[i],
                                          aEnv )
                             == STL_SUCCESS );
                    
                    sSqlFixIter->mSqlArray[i] = NULL;
                }
            }
                     
            if( sSqlFixIter->mIsDefaultBlock == STL_FALSE )
            {
                STL_TRY( knlFreeDynamicMem( &(gKnEntryPoint->mSqlFixMem),
                                            sSqlFixIter,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                STL_RING_UNLINK( &(aSessionEnv->mSqlFixList), sSqlFixIter );
            }
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;
    
    switch( sState )
    {
        case 2:
            (void) knlDestroyDynamicMem( &aSessionEnv->mLongVaryingMem, 
                                         aEnv );
        case 1:
            (void) knlDestroyDynamicMem( &aSessionEnv->mParameterMem, 
                                         aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}


/**
 * @brief SAR 검증없이 Session Environment Pool에서 사용 가능한 Session Environment를 반환한다.
 * @param[out] aSessionEnv 사용 가능한 Session Environment
 * @param[out] aEnv        Environment 포인터
 */
stlStatus knlAllocSessionEnvUnsafe( void    ** aSessionEnv,
                                    knlEnv   * aEnv )
{
    kncManager   * sSessionEnvMgr;
    kncHeader    * sFreeSessionEnvHeader;
    stlBool        sIsTimedout = STL_FALSE;
    stlInt32       sState = 0;
    stlInt32       i;

    STL_PARAM_VALIDATE( gKnEntryPoint != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( gKnEntryPoint->mSessionEnvManager != NULL,
                        KNL_ERROR_STACK( aEnv ) );

    sSessionEnvMgr = gKnEntryPoint->mSessionEnvManager;

    STL_TRY( knlAcquireLatch( &(sSessionEnvMgr->mLatch),
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedout,
                              aEnv ) == STL_SUCCESS );
    sState = 1;

    /**
     * SessionEnv 관리자에서 사용 가능한 SessionEnv을 할당 받는다.
     */
    sFreeSessionEnvHeader = sSessionEnvMgr->mFreeList;

    STL_TRY_THROW( sFreeSessionEnvHeader != NULL, RAMP_ERR_INSUFFICIENT_RESOURCE );

    sSessionEnvMgr->mFreeList = sFreeSessionEnvHeader->mNextFreeLink;
    sFreeSessionEnvHeader->mNextFreeLink = NULL;
    
    sState = 0;
    STL_TRY( knlReleaseLatch( &(sSessionEnvMgr->mLatch),
                              aEnv )
             == STL_SUCCESS );

    *aSessionEnv = (void*)sFreeSessionEnvHeader + STL_SIZEOF(kncHeader);

    ((knlSessionEnv*)(*aSessionEnv))->mEnableAging = STL_FALSE;
    ((knlSessionEnv*)(*aSessionEnv))->mEnv = NULL;
    stlMemBarrier();
    ((knlSessionEnv*)(*aSessionEnv))->mTermination = KNL_TERMINATION_NONE;
    ((knlSessionEnv*)(*aSessionEnv))->mNeedPendingSignal = STL_FALSE;
    ((knlSessionEnv*)(*aSessionEnv))->mPendingSignalNo = -1;
    ((knlSessionEnv*)(*aSessionEnv))->mLogonTime = stlNow();

    KNL_INCREASE_SESSION_SEQ( *aSessionEnv );
    KNL_SET_SESSION_ACCESS_KEY( *aSessionEnv,
                                KNL_SESSION_ACCESS_LEAVE,
                                KNL_GET_SESSION_SEQ( *aSessionEnv ) );
    
    for( i = 0; i < STL_LAYER_MAX; i++ )
    {
        ((knlSessionEnv*)(*aSessionEnv))->mInitialized[i] = STL_FALSE;
    }
    
    knlInitSessionEvent( &(((knlSessionEnv*)(*aSessionEnv))->mSessionEvent) );

    stlMemBarrier();
    ((knlSessionEnv*)(*aSessionEnv))->mUsed = STL_TRUE;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INSUFFICIENT_RESOURCE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INSUFFICIENT_RESOURCE,
                      "exceeded maximum number of session environments",
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    switch( sState )
    {
        case 1 :
            knlReleaseLatch( &(sSessionEnvMgr->mLatch),
                             aEnv );
            break;
        default:
            break;
    }

    return STL_FAILURE;
}


/**
 * @brief Session Environment Pool에서 사용 가능한 Session Environment를 반환한다.
 * @param[out] aSessionEnv 사용 가능한 Session Environment
 * @param[out] aEnv        Environment 포인터
 */
stlStatus knlAllocSessionEnv( void    ** aSessionEnv,
                              knlEnv   * aEnv )
{
    /**
     * SESSION이 이용 가능한 상태인지 검사한다.
     */
    STL_TRY( knlValidateSar( KNL_SAR_SESSION,
                             KNL_ENV(aEnv)->mTopLayer,
                             aEnv ) == STL_SUCCESS );

    STL_TRY( knlAllocSessionEnvUnsafe( aSessionEnv,
                                       aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Session Environment Pool에서 사용 가능한 Admin Session Environment를 반환한다.
 * @param[out] aSessionEnv 사용 가능한 Session Environment
 * @param[out] aEnv        Environment 포인터
 */
stlStatus knlAllocAdminSessionEnv( void    ** aSessionEnv,
                                   knlEnv   * aEnv )
{
    kncManager   * sSessionEnvMgr;
    stlBool        sIsTimedout = STL_FALSE;
    stlInt32       sState = 0;
    stlInt32       i;

    STL_PARAM_VALIDATE( gKnEntryPoint != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( gKnEntryPoint->mSessionEnvManager != NULL,
                        KNL_ERROR_STACK( aEnv ) );

    sSessionEnvMgr = gKnEntryPoint->mSessionEnvManager;

    STL_TRY( knlAcquireLatch( &(sSessionEnvMgr->mLatch),
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedout,
                              aEnv ) == STL_SUCCESS );
    sState = 1;

    *aSessionEnv = knlGetSessionEnv( KNL_ADMIN_SESSION_ID );

    STL_TRY_THROW( knlIsUsedSessionEnv( *aSessionEnv ) == STL_FALSE,
                   RAMP_ERR_ADMIN_ALREADY_LOGGED_IN );

    ((knlSessionEnv*)(*aSessionEnv))->mEnableAging = STL_FALSE;
    ((knlSessionEnv*)(*aSessionEnv))->mEnv = NULL;
    stlMemBarrier();
    ((knlSessionEnv*)(*aSessionEnv))->mTermination = KNL_TERMINATION_NONE;
    ((knlSessionEnv*)(*aSessionEnv))->mNeedPendingSignal = STL_FALSE;
    ((knlSessionEnv*)(*aSessionEnv))->mPendingSignalNo = -1;
    ((knlSessionEnv*)(*aSessionEnv))->mLogonTime = stlNow();

    KNL_INCREASE_SESSION_SEQ( *aSessionEnv );
    KNL_SET_SESSION_ACCESS_KEY( *aSessionEnv,
                                KNL_SESSION_ACCESS_LEAVE,
                                KNL_GET_SESSION_SEQ( *aSessionEnv ) );
    
    for( i = 0; i < STL_LAYER_MAX; i++ )
    {
        ((knlSessionEnv*)(*aSessionEnv))->mInitialized[i] = STL_FALSE;
    }
    
    knlInitSessionEvent( &(((knlSessionEnv*)(*aSessionEnv))->mSessionEvent) );

    stlMemBarrier();
    ((knlSessionEnv*)(*aSessionEnv))->mUsed = STL_TRUE;
    
    sState = 0;
    STL_TRY( knlReleaseLatch( &(sSessionEnvMgr->mLatch),
                              aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_ADMIN_ALREADY_LOGGED_IN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_SERVER_REJECTED_THE_CONNECTION,
                      "admin is already logged in",
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    switch( sState )
    {
        case 1 :
            knlReleaseLatch( &(sSessionEnvMgr->mLatch),
                             aEnv );
            break;
        default:
            break;
    }

    return STL_FAILURE;
}


/**
 * @brief 사용한 Session Environment를 Session Pool에 반납한다.
 * @param[out] aSessionEnv 반납할 Session Envrionment
 * @param[out] aEnv        Environment 포인터
 */
stlStatus knlFreeSessionEnv( void    * aSessionEnv,
                             knlEnv  * aEnv )
{
    kncManager   * sSessionEnvMgr;
    kncHeader    * sSessionEnvHeader;
    stlBool        sIsSuccess = STL_FALSE;
    stlInt32       sState = 0;
    stlUInt32      sSessionId = 0;

    STL_PARAM_VALIDATE( gKnEntryPoint != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( gKnEntryPoint->mSessionEnvManager != NULL, KNL_ERROR_STACK(aEnv) );

    sSessionEnvMgr = gKnEntryPoint->mSessionEnvManager;

    sSessionEnvHeader = (kncHeader*)(aSessionEnv - STL_SIZEOF(kncHeader));

    while( sIsSuccess == STL_FALSE )
    {
        STL_TRY( knlTryLatch( &(sSessionEnvMgr->mLatch),
                              KNL_LATCH_MODE_EXCLUSIVE,
                              &sIsSuccess,
                              aEnv ) == STL_SUCCESS );
    }
    sState = 1;

    if( ((knlSessionEnv*)aSessionEnv)->mUsed == STL_FALSE )
    {
        knlGetSessionEnvId( aSessionEnv, &sSessionId, aEnv );
        knlLogMsgUnsafe( NULL,
                         aEnv,
                         KNL_LOG_LEVEL_WARNING,
                         "already freed SESSION ENV (%ld)\n",
                         sSessionId );
        
        STL_THROW( RAMP_SUCCESS );
    }

    /**
     * Admin Session이 아니라면 free 되는 Environment를 Environment 관리자에
     * 반환한다.
     */
    
    if( aSessionEnv != knlGetSessionEnv( KNL_ADMIN_SESSION_ID ) )
    {
        sSessionEnvHeader->mNextFreeLink = sSessionEnvMgr->mFreeList;
        sSessionEnvMgr->mFreeList = sSessionEnvHeader;
    }

    ((knlSessionEnv*)aSessionEnv)->mEnableAging = STL_FALSE;
    ((knlSessionEnv*)aSessionEnv)->mUsed = STL_FALSE;
    ((knlSessionEnv*)aSessionEnv)->mTermination = KNL_TERMINATION_NONE;
    
    KNL_INCREASE_SESSION_SEQ( aSessionEnv );

    STL_RAMP( RAMP_SUCCESS );
    
    sState = 0;
    STL_TRY( knlReleaseLatch( &(sSessionEnvMgr->mLatch),
                              aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1 :
            knlReleaseLatch( &(sSessionEnvMgr->mLatch),
                             aEnv );
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief Session Envrionment Pool에서 첫번째 Session Environment를 반환한다.
 * @remark 동시성 제어를 하지 않는다.
 */
void * knlGetFirstSessionEnv()
{
    kncHeader  * sSessionEnvHeader;
    void       * sSessionEnv;

    STL_TRY( gKnEntryPoint != NULL );
    STL_TRY( gKnEntryPoint->mSessionEnvManager != NULL );
    STL_TRY( gKnEntryPoint->mCacheAlignedSize != 0 );

    sSessionEnvHeader = (void*)gKnEntryPoint->mSessionEnvManager +
                        STL_ALIGN( STL_SIZEOF(kncManager), 
                                   gKnEntryPoint->mCacheAlignedSize );

    sSessionEnv = (void*)sSessionEnvHeader + STL_SIZEOF(kncHeader);

    return sSessionEnv;

    STL_FINISH;

    return NULL;
}

/**
 * @brief Session Envrionment Pool에서 마지막 Session Environment를 반환한다.
 * @remark 동시성 제어를 하지 않는다.
 */
void * knlGetLastSessionEnv()
{
    kncManager * sSessionEnvManager;
    kncHeader  * sSessionEnvHeader;
    void       * sSessionEnv;
    stlInt64     sCacheAlignedSize = 0;
    stlInt64     sSessionManagerSize = 0;
    stlInt64     sSessionSize = 0;

    STL_TRY( gKnEntryPoint != NULL );
    STL_TRY( gKnEntryPoint->mSessionEnvManager != NULL );
    STL_TRY( gKnEntryPoint->mCacheAlignedSize != 0 );

    sSessionEnvManager = gKnEntryPoint->mSessionEnvManager;

    sCacheAlignedSize = gKnEntryPoint->mCacheAlignedSize;

    sSessionManagerSize = STL_ALIGN( STL_SIZEOF(kncManager), sCacheAlignedSize );
    sSessionSize        = STL_ALIGN( (STL_SIZEOF(kncHeader) + sSessionEnvManager->mSize),
                                     sCacheAlignedSize );

    sSessionEnvHeader = (void*)sSessionEnvManager +
                        sSessionManagerSize +
                        (sSessionSize * (sSessionEnvManager->mCount - 1 ));

    sSessionEnv = (void*)sSessionEnvHeader + STL_SIZEOF(kncHeader);

    return sSessionEnv;

    STL_FINISH;

    return NULL;
}

/**
 * @brief 입력받은 Session Envrionment의 다음 Session Environment를 반환한다.
 * @param[in] aSessionEnv Session Environment
 * @return 다음 Session Environment가 없을 경우 NULL을 반환한다.
 * @remark 동시성 제어를 하지 않는다.
 */
void * knlGetNextSessionEnv( void * aSessionEnv )
{
    kncHeader  * sSessionEnv;
    kncHeader  * sNextSessionEnv;
    stlUInt32    sIdx;
    kncManager * sSessionEnvManager;
    stlInt64     sCacheAlignedSize = 0;
    stlInt64     sSessionManagerSize = 0;
    stlInt64     sSessionSize = 0;

    STL_TRY( gKnEntryPoint != NULL );
    STL_TRY( gKnEntryPoint->mSessionEnvManager != NULL );
    STL_TRY( gKnEntryPoint->mCacheAlignedSize != 0 );

    sSessionEnvManager = gKnEntryPoint->mSessionEnvManager;

    sCacheAlignedSize = gKnEntryPoint->mCacheAlignedSize;

    sSessionManagerSize = STL_ALIGN( STL_SIZEOF(kncManager), sCacheAlignedSize );
    sSessionSize        = STL_ALIGN( (STL_SIZEOF(kncHeader) + sSessionEnvManager->mSize),
                                     sCacheAlignedSize );

    sSessionEnv = (kncHeader*)(aSessionEnv - STL_SIZEOF(kncHeader));
    sIdx = sSessionEnv->mIdx + 1;

    STL_TRY( sIdx < sSessionEnvManager->mCount );

    sNextSessionEnv = (void*)sSessionEnvManager +
                      sSessionManagerSize +
                      ( sSessionSize * sIdx );

    return (void*)sNextSessionEnv + STL_SIZEOF(kncHeader);

    STL_FINISH;

    return NULL;
}

/**
 * @brief 입력받은 Session Envrionment의 ID를 반환한다.
 * @param[in]     aSessionEnv Session Environment 포인터
 * @param[out]    aId         Session Environment의 ID
 * @param[in,out] aEnv        Environment Pointer
 */
stlStatus knlGetSessionEnvId( void      * aSessionEnv,
                              stlUInt32 * aId,
                              knlEnv    * aEnv )
{
    kncHeader  * sSessionEnv;

    STL_TRY_THROW( gKnEntryPoint != NULL, RAMP_ERR_INTERNAL );
    STL_TRY_THROW( gKnEntryPoint->mSessionEnvManager != NULL, RAMP_ERR_INTERNAL );
    STL_TRY_THROW( aId != NULL, RAMP_ERR_INTERNAL );

    sSessionEnv = (kncHeader*)(aSessionEnv - STL_SIZEOF(kncHeader));

    *aId = sSessionEnv->mIdx;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_INTERNAL,
                      "knlGetSessionEnvId()",
                      KNL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 입력받은 Session Envrionment가 Admin Session인지 여부를 반환한다.
 * @param[in]     aSessionEnv Session Environment 포인터
 * @return Admin Session이면 STL_TRUE, 그러지 않다면 STL_FALSE
 */
stlBool knlIsAdminSession( void * aSessionEnv )
{
    kncHeader  * sSessionEnv;

    STL_TRY( gKnEntryPoint != NULL );
    STL_TRY( gKnEntryPoint->mSessionEnvManager != NULL );

    sSessionEnv = (kncHeader*)(aSessionEnv - STL_SIZEOF(kncHeader));

    if( sSessionEnv->mIdx == KNL_ADMIN_SESSION_ID )
    {
        return STL_TRUE;
    }
    else
    {
        return STL_FALSE;
    }

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief 주어진 Session Environment 아이디에 해당되는 Session Environment 포인터를 반환한다.
 * @param[in] aSessionEnvId Session Environment의 ID
 * @return Session Environment 포인터
 */
void * knlGetSessionEnv( stlUInt32 aSessionEnvId )
{
    kncHeader  * sSessionEnvHeader;
    kncManager * sSessionEnvManager;
    void       * sSessionEnv = NULL;
    stlInt64     sCacheAlignedSize = 0;
    stlInt64     sSessionManagerSize = 0;
    stlInt64     sSessionSize = 0;

    sSessionEnvManager = gKnEntryPoint->mSessionEnvManager;

    STL_TRY( aSessionEnvId < sSessionEnvManager->mCount );

    sCacheAlignedSize = gKnEntryPoint->mCacheAlignedSize;

    sSessionManagerSize = STL_ALIGN( STL_SIZEOF(kncManager), sCacheAlignedSize );
    sSessionSize        = STL_ALIGN( (STL_SIZEOF(kncHeader) + sSessionEnvManager->mSize),
                                     sCacheAlignedSize );

    sSessionEnvHeader = (void*)sSessionEnvManager +
                        sSessionManagerSize +
                        ( sSessionSize * aSessionEnvId );

    sSessionEnv = (void*)sSessionEnvHeader + STL_SIZEOF(kncHeader);

    return sSessionEnv;

    STL_FINISH;

    return NULL;
}

/**
 * @brief 입력받은 Session Envrionment의 사용여부를 판단한다.
 * @param[in] aSessionEnv Session Environment
 */
stlBool knlIsUsedSessionEnv( knlSessionEnv * aSessionEnv )
{
    return aSessionEnv->mUsed;
}

/**
 * @brief 입력받은 Session Envrionment의 종료 상태를 반환한다.
 * @param[in] aSessionEnv Session Environment
 */
knlTermination knlGetTerminationStatus( knlSessionEnv * aSessionEnv )
{
    return aSessionEnv->mTermination;
}

/**
 * @brief 입력받은 Session Envrionment의 종료여부를 판단한다.
 * @param[in] aSessionEnv Session Environment
 */
stlBool knlIsTerminatedSessionEnv( knlSessionEnv * aSessionEnv )
{
    return (aSessionEnv->mTermination == KNL_TERMINATION_NONE ) ? STL_FALSE : STL_TRUE;
}

/**
 * @brief 입력받은 Session Envrionment의 종료여부를 판단한다.
 * @param[in] aSessionEnv Session Environment
 */
stlBool knlIsDeadSessionEnv( knlSessionEnv * aSessionEnv )
{
    return aSessionEnv->mDead;
}

stlBool knlIsSharedSessionEnv( knlSessionEnv * aSessionEnv )
{
    return (aSessionEnv->mSessEnvType == KNL_SESS_ENV_SHARED) ? STL_TRUE : STL_FALSE;
}

/**
 * @brief Session 을 정상종료 한다.
 * @param[in,out]  aSessionEnv    커널 Session Environment
 */
void knlDisconnectSession( void * aSessionEnv )
{
    KNL_SET_SESSION_STATUS( aSessionEnv, KNL_SESSION_STATUS_DISCONNECTED );
    KNL_INCREASE_SESSION_SEQ( aSessionEnv );
    ((knlSessionEnv*)aSessionEnv)->mTermination = KNL_TERMINATION_NORMAL;
    stlMemBarrier();
}

/**
 * @brief Session 을 강제종료 한다.
 * @param[in,out]  aSessionEnv    커널 Session Environment
 */
void knlKillSession( void * aSessionEnv )
{
    KNL_INCREASE_SESSION_SEQ( aSessionEnv );
    ((knlSessionEnv*)aSessionEnv)->mTermination = KNL_TERMINATION_ABNORMAL;
    stlMemBarrier();
}

/**
 * @brief Session 을 더이상 사용하지 못하도록 막는다.
 * @param[in] aSessionEnv Session Environment
 */
void knlDeadSession( knlSessionEnv * aSessionEnv )
{
    aSessionEnv->mDead = STL_TRUE;
}

/**
 * @brief 'KILL -9' 에 의해서 종료된 세션을 정리한다.
 * @param[in] aSessionEnvId 종료 할 Session의 ID
 * @param[in] aSessionSerial 종료 할 Session의 Serial#
 * @param[out] aEnv Environment 포인터
 */
stlStatus knlCleanupKilledSession( stlUInt32    aSessionEnvId,
                                   stlUInt64    aSessionSerial,
                                   knlEnv     * aEnv )
{
    kncManager    * sSessionEnvMgr;
    stlBool         sIsTimedout = STL_FALSE;
    stlInt32        sState = 0;
    knlEnv        * sEnv;
    knlSessionEnv * sSessionEnv;

    STL_PARAM_VALIDATE( gKnEntryPoint != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( gKnEntryPoint->mSessionEnvManager != NULL,
                        KNL_ERROR_STACK( aEnv ) );

    sSessionEnvMgr = gKnEntryPoint->mSessionEnvManager;

    STL_TRY( knlAcquireLatch( &(sSessionEnvMgr->mLatch),
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedout,
                              aEnv ) == STL_SUCCESS );
    sState = 1;

    sSessionEnv = (knlSessionEnv*)knlGetSessionEnv( aSessionEnvId );

    STL_TRY_THROW( sSessionEnv->mUsed == STL_TRUE, 
                   RAMP_ERR_INVALID_SESSION_ID );
    
    STL_TRY_THROW( aSessionSerial == KNL_GET_SESSION_SEQ( sSessionEnv ),
                   RAMP_ERR_INVALID_SESSION_ID );
    
    sEnv = (knlEnv*)(sSessionEnv->mEnv);

    STL_TRY_THROW( sEnv != NULL, RAMP_ERR_INVALID_SESSION_ID );
    
    STL_TRY_THROW( stlIsActiveProc( &(sEnv->mThread.mProc) ) == STL_FALSE,
                   RAMP_ERR_KILL_ACTIVE_PROC );

    STL_TRY_THROW( knlResolveSystemFatal( STL_FALSE, sEnv ) == STL_TRUE,
                   RAMP_ERR_CRITICAL_SECTION_COUNT );

    KNL_SET_SESSION_ACCESS_KEY( sSessionEnv,
                                KNL_SESSION_ACCESS_LEAVE,
                                aSessionSerial );

    STL_TRY( knlEndSessionByID( aSessionEnvId,
                                aSessionSerial,
                                KNL_END_SESSION_OPTION_KILL,
                                KNL_ENV( aEnv ) ) == STL_SUCCESS );

    STL_TRY( knlKillSessionByID( aSessionEnvId,
                                 aSessionSerial,
                                 KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( knlReleaseLatch( &(sSessionEnvMgr->mLatch),
                              aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_SESSION_ID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_USER_SESSION_ID_DOSE_NOT_EXIST,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_CRITICAL_SECTION_COUNT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_SESSION_CANNOT_BE_KILLED_NOW,
                      "session in critical section",
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_KILL_ACTIVE_PROC )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_SESSION_CANNOT_BE_KILLED_NOW,
                      "process still alive",
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    if( sState == 1 )
    {
        (void) knlReleaseLatch( &(sSessionEnvMgr->mLatch), aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief SessionID를 통해 Session을 종료 한다.
 * @param[in] aSessionEnvId 종료 할 Session의 ID
 * @param[in] aSessionSerial 종료 할 Session의 Serial#
 * @param[out] aEnv Environment 포인터
 */
stlStatus knlKillSessionByID( stlUInt32    aSessionEnvId,
                              stlUInt64    aSessionSerial,
                              knlEnv     * aEnv )
{
    kncManager      * sSessionEnvManager;
    knlSessionEnv   * sSessionEnv;

    STL_TRY( gKnEntryPoint != NULL );
    STL_TRY( gKnEntryPoint->mSessionEnvManager != NULL );

    sSessionEnvManager = gKnEntryPoint->mSessionEnvManager;
    STL_TRY_THROW( aSessionEnvId < sSessionEnvManager->mCount,
                   RAMP_ERR_INVALID_SESSION_ID );

    sSessionEnv = (knlSessionEnv*)knlGetSessionEnv( aSessionEnvId );
    STL_TRY_THROW( sSessionEnv->mUsed == STL_TRUE, 
                   RAMP_ERR_INVALID_SESSION_ID );
    
    STL_TRY_THROW( aSessionSerial == KNL_GET_SESSION_SEQ( sSessionEnv ),
                   RAMP_ERR_INVALID_SESSION_ID );

    KNL_SET_SESSION_STATUS( sSessionEnv, KNL_SESSION_STATUS_SNIPED );
    KNL_INCREASE_SESSION_SEQ( sSessionEnv );
    ((knlSessionEnv*)sSessionEnv)->mTermination = KNL_TERMINATION_ABNORMAL;
    
    knlLogMsg( NULL,
               aEnv,
               KNL_LOG_LEVEL_INFO,
               "sniped session - id(%d), sequence(%ld)\n",
               aSessionEnvId,
               aSessionSerial );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_SESSION_ID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_USER_SESSION_ID_DOSE_NOT_EXIST,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 모든 사용자 세션들의 종료를 기다림.
 * @param[in] aEnv Environment Pointer
 */
stlStatus knlWaitAllUserSession( knlEnv * aEnv )
{
    knlSessionEnv * sSessEnv;
    knlEnv        * sEnv;
    stlBool         sExistActiveSession;

    while( 1 )
    {
        sSessEnv = knlGetFirstSessionEnv();

        sExistActiveSession = STL_FALSE;
        
        while( sSessEnv != NULL )
        {
            sEnv = sSessEnv->mEnv;

            if( (knlIsUsedSessionEnv(sSessEnv) == STL_TRUE) &&
                (KNL_IS_USER_LAYER(sEnv->mTopLayer) == STL_TRUE) )
            {
                sExistActiveSession = STL_TRUE;
                break;
            }

            sSessEnv = knlGetNextSessionEnv( sSessEnv );
        }

        if( sExistActiveSession == STL_FALSE )
        {
            break;
        }

        stlSleep( 10000 );
    }

    return STL_SUCCESS;
}

/**
 * @brief 모든 사용자 세션들의 종료시킴
 * @param[in] aEnv Environment Pointer
 */
stlStatus knlCloseAllUserSession( knlEnv * aEnv )
{
    knlSessionEnv * sSessEnv;
    knlEnv        * sEnv;

    sSessEnv = knlGetFirstSessionEnv();

    while( sSessEnv != NULL )
    {
        sEnv = sSessEnv->mEnv;

        if( (knlIsUsedSessionEnv(sSessEnv) == STL_TRUE) &&
            (KNL_IS_USER_LAYER(sEnv->mTopLayer) == STL_TRUE) )
        {
            STL_TRY( knlEndSession( sSessEnv,
                                    KNL_END_SESSION_OPTION_KILL,
                                    aEnv )
                     == STL_SUCCESS );
        }

        sSessEnv = knlGetNextSessionEnv( sSessEnv );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 입력받은 Session Envrionment의 종료 여부를 판단한다.
 * @param[in] aSessionEnv Session Environment
 */
stlBool knlIsEndSession( knlSessionEnv * aSessionEnv )
{
    return ( aSessionEnv->mSessionEvent.mSessEndEvent == KNL_END_SESSION_OPTION_IMMEDIATE ) ?
        STL_TRUE : STL_FALSE;
}

/**
 * @brief 입력받은 Session Envrionment의 KILL 여부를 판단한다.
 * @param[in] aSessionEnv Session Environment
 */
stlBool knlIsKilledSession( knlSessionEnv * aSessionEnv )
{
    return ( aSessionEnv->mSessionEvent.mSessEndEvent == KNL_END_SESSION_OPTION_KILL ) ?
        STL_TRUE : STL_FALSE;
}

/**
 * @brief 입력받은 Session Envrionment의 종료 옵션을 반환한다.
 * @param[in] aSessionEnv Session Environment
 */
knlSessEndOption knlGetEndSessionOption( knlSessionEnv * aSessionEnv )
{
    return aSessionEnv->mSessionEvent.mSessEndEvent;
}

/**
 * @brief Session의 종료 여부를 검사한다.
 * @param[in]  aSessionEnv Session Environment 포인터
 * @param[out] aEnv        Process Environment 포인터
 */
stlStatus knlCheckEndSession( void   * aSessionEnv,
                              knlEnv * aEnv )
{
    knlSessionEvent * sSessEvent;
    knlSessionEnv   * sSessionEnv;

    sSessionEnv = (knlSessionEnv*)aSessionEnv;

    sSessEvent = &(sSessionEnv->mSessionEvent);

    if( sSessEvent->mDisableEvent == STL_FALSE )
    {
        STL_TRY_THROW( sSessEvent->mSessEndEvent < KNL_END_SESSION_OPTION_IMMEDIATE,
                       RAMP_ERR_COMMUNICATION_LINK_FAILURE );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_COMMUNICATION_LINK_FAILURE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_COMMUNICATION_LINK_FAILURE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );

        sSessEvent->mDisableEvent = STL_TRUE;
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Session을 종료 한다.
 * @param[in] aSessionEnv Session Environment
 * @param[in] aOption 종료 Option
 * @param[out] aEnv Environment 포인터
 */
stlStatus knlEndSession( knlSessionEnv    * aSessionEnv,
                         knlSessEndOption   aOption,
                         knlEnv           * aEnv )
{
    STL_TRY_THROW( aSessionEnv->mUsed == STL_TRUE, 
                   RAMP_ERR_INVALID_SESSION_ID );

    aSessionEnv->mSessionEvent.mSessEndEvent = aOption;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_SESSION_ID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_USER_SESSION_ID_DOSE_NOT_EXIST,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Statement의 종료 여부를 검사한다.
 * @param[out] aEnv Environment 포인터
 */
stlStatus knlCheckEndStatement( knlEnv * aEnv )
{
    knlSessionEvent * sSessEvent;
    
    sSessEvent = &(KNL_SESS_ENV(aEnv)->mSessionEvent);
    
    if( sSessEvent->mDisableEvent == STL_FALSE )
    {
        /**
         * Canceled Statement 처리
         */
        STL_TRY_THROW( sSessEvent->mStmtEndEvent != KNL_END_STATEMENT_OPTION_CANCEL,
                       RAMP_ERR_OPERATION_CANCELED );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_OPERATION_CANCELED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_OPERATION_CANCELED,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Statement를 종료 한다.
 * @param[in] aSessionEnv Session Environment
 * @param[in] aOption 종료 Option
 * @param[out] aEnv Environment 포인터
 */
stlStatus knlEndStatement( knlSessionEnv    * aSessionEnv,
                           knlStmtEndOption   aOption,
                           knlEnv           * aEnv )
{
    STL_TRY_THROW( aSessionEnv->mUsed == STL_TRUE, 
                   RAMP_ERR_INVALID_SESSION_ID );

    aSessionEnv->mSessionEvent.mStmtEndEvent = aOption;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_SESSION_ID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_USER_SESSION_ID_DOSE_NOT_EXIST,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief SessionID를 통해 Session을 종료 한다.
 * @param[in] aSessionEnvId 종료 할 Session의 ID
 * @param[in] aSessionSerial 종료 할 Session의 Serial#
 * @param[in] aOption 종료 Option
 * @param[out] aEnv Environment 포인터
 */
stlStatus knlEndSessionByID( stlUInt32          aSessionEnvId,
                             stlUInt64          aSessionSerial,
                             knlSessEndOption   aOption,
                             knlEnv           * aEnv )
{
    kncManager      * sSessionEnvManager;
    knlSessionEnv   * sSessionEnv;
    knlSessionEvent * sSessEvent;

    STL_TRY( gKnEntryPoint != NULL );
    STL_TRY( gKnEntryPoint->mSessionEnvManager != NULL );

    sSessionEnvManager = gKnEntryPoint->mSessionEnvManager;
    STL_TRY_THROW( aSessionEnvId < sSessionEnvManager->mCount,
                   RAMP_ERR_INVALID_SESSION_ID );

    sSessionEnv = (knlSessionEnv*)knlGetSessionEnv( aSessionEnvId );
    STL_TRY_THROW( sSessionEnv->mUsed == STL_TRUE, 
                   RAMP_ERR_INVALID_SESSION_ID );
    
    STL_TRY_THROW( aSessionSerial == KNL_GET_SESSION_SEQ( sSessionEnv ),
                   RAMP_ERR_INVALID_SESSION_ID );
    
    sSessEvent = &sSessionEnv->mSessionEvent;
    
    sSessEvent->mSessEndEvent = aOption;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_SESSION_ID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_USER_SESSION_ID_DOSE_NOT_EXIST,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 해당 Session이 User Session인지 얻는다.
 * @param[in] aSessionEnvId 종료 할 Session의 ID
 * @param[out] aIsUserSession User Session 여부
 * @param[out] aEnv Environment 포인터
 */
stlStatus knlIsUserSession( stlUInt32   aSessionEnvId,
                            stlBool   * aIsUserSession,
                            knlEnv    * aEnv )
{
    kncManager      * sSessionEnvManager;
    knlSessionEnv   * sSessionEnv;

    STL_TRY( gKnEntryPoint != NULL );
    STL_TRY( gKnEntryPoint->mSessionEnvManager != NULL );

    sSessionEnvManager = gKnEntryPoint->mSessionEnvManager;
    STL_TRY_THROW( aSessionEnvId < sSessionEnvManager->mCount,
                   RAMP_ERR_INVALID_SESSION_ID );

    sSessionEnv = (knlSessionEnv*)knlGetSessionEnv( aSessionEnvId );
    STL_TRY_THROW( sSessionEnv->mUsed == STL_TRUE, 
                   RAMP_ERR_INVALID_SESSION_ID );

    if( KNL_IS_USER_LAYER(sSessionEnv->mTopLayer) == STL_TRUE )
    {
        *aIsUserSession = STL_TRUE;
    }
    else
    {
        *aIsUserSession = STL_FALSE;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_SESSION_ID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_USER_SESSION_ID_DOSE_NOT_EXIST,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

void knlInitSessionEvent( knlSessionEvent * aSessionEvent )
{
    aSessionEvent->mSessEndEvent     = KNL_END_SESSION_OPTION_NA;
    aSessionEvent->mStmtEndEvent     = KNL_END_STATEMENT_OPTION_NA;
    aSessionEvent->mQueryBeginTime   = 0;
    aSessionEvent->mIdleBeginTime    = 0;
    aSessionEvent->mStmtQueryTimeout = KNL_DEFAULT_STATEMENT_QUERY_TIMEOUT;
    aSessionEvent->mDisableEvent     = STL_FALSE;
}

/**
 * @brief Query Timeout을 활성화 시킨다.
 * @param[out] aEnv  Environment 포인터
 * @param[in] aStmtQueryTimeout  statement query timeout
 */
void knlEnableQueryTimeout( knlEnv  * aEnv,
                            stlInt32  aStmtQueryTimeout )
{
    knlSessionEvent * sSessEvent;

    knlEnableSessionEvent( aEnv );
    
    sSessEvent = &(KNL_SESS_ENV(aEnv)->mSessionEvent);
    
    sSessEvent->mQueryBeginTime = knlGetSystemTime();
    sSessEvent->mStmtQueryTimeout = aStmtQueryTimeout;
    sSessEvent->mStmtEndEvent = KNL_END_STATEMENT_OPTION_NA;
}

/**
 * @brief Query Timeout을 비활성화 시킨다.
 * @param[out] aEnv  Environment 포인터
 */
void knlDisableQueryTimeout( knlEnv  * aEnv )
{
    knlSessionEvent * sSessEvent;
    
    sSessEvent = &(KNL_SESS_ENV(aEnv)->mSessionEvent);
    
    sSessEvent->mQueryBeginTime = 0;
    sSessEvent->mStmtQueryTimeout = KNL_DEFAULT_STATEMENT_QUERY_TIMEOUT;
    sSessEvent->mStmtEndEvent = KNL_END_STATEMENT_OPTION_NA;
}

/**
 * @brief Session Event를 활성화 한다.
 * @param[out] aEnv  Environment 포인터
 */
void knlEnableSessionEvent( knlEnv  * aEnv )
{
    knlSessionEvent * sSessEvent;
    
    sSessEvent = &(KNL_SESS_ENV(aEnv)->mSessionEvent);

    sSessEvent->mDisableEvent = STL_FALSE;
}

/**
 * @brief Session Event를 비활성화 한다.
 * @param[out] aEnv  Environment 포인터
 */
void knlDisableSessionEvent( knlEnv  * aEnv )
{
    knlSessionEvent * sSessEvent;
    
    sSessEvent = &(KNL_SESS_ENV(aEnv)->mSessionEvent);
    
    sSessEvent->mDisableEvent = STL_TRUE;
}

/**
 * @brief Query Timeout을 검사한다.
 * @param[out] aEnv Environment 포인터
 */
stlStatus knlCheckQueryTimeout( knlEnv * aEnv )
{
    knlSessionEvent * sSessEvent;
    stlTime           sTimeout;
    stlTime           sNow;

    KNL_CHECK_THREAD_CANCELLATION( aEnv );
    STL_TRY( knlCheckEndSession( KNL_SESS_ENV(aEnv),
                                 aEnv )
             == STL_SUCCESS );
    STL_TRY( knlCheckEndStatement( aEnv ) == STL_SUCCESS );

    sSessEvent = &(KNL_SESS_ENV(aEnv))->mSessionEvent;

    /**
     * Query Timeout 처리
     */

    if( sSessEvent->mDisableEvent == STL_FALSE )
    {
        STL_TRY_THROW( sSessEvent->mQueryBeginTime != 0, RAMP_SUCCESS );

        sTimeout = sSessEvent->mStmtQueryTimeout;

        /* statement별 query timeout 값이 우선한다.
         * 이 값이 정의되어 있지 않으면 session의 query timeout을 참조한다.
         */
        if( sTimeout == KNL_DEFAULT_STATEMENT_QUERY_TIMEOUT )
        {
            sTimeout = knlGetPropertyBigIntValueByID( KNL_PROPERTY_QUERY_TIMEOUT,
                                                      aEnv );
        }

        STL_TRY_THROW( sTimeout != 0, RAMP_SUCCESS );

        sNow = knlGetSystemTime();

        STL_TRY_THROW( KNL_CHECK_TIMEOUT( sNow - sSessEvent->mQueryBeginTime,
                                          STL_SET_SEC_TIME(sTimeout) ) == STL_FALSE,
                       RAMP_ERR_QUERY_TIMEOUT );
    }

    STL_RAMP( RAMP_SUCCESS );
        
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_QUERY_TIMEOUT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_EXCEEDED_QUERY_TIMEOUT,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );

        /**
         * Exception처리시 다시 TIMEOUNT 발생을 막는다.
         */
        sSessEvent->mDisableEvent = STL_TRUE;
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Parent 쓰레도의 Query Timeout을 검사한다.
 * @param[in]     aParentEnv Parent Environment 포인터
 * @param[in,out] aEnv       Environment 포인터
 * @note 해당 함수는 exception 처리시 사용되어서는 안된다.
 *       Repeating Error Handling을 처리하지 않는다.
 */
stlStatus knlCheckParentQueryTimeout( knlEnv * aParentEnv,
                                      knlEnv * aEnv )
{
    knlSessionEvent * sSessEvent;
    stlTime           sTimeout;
    stlTime           sNow;

    sSessEvent = &(KNL_SESS_ENV(aParentEnv))->mSessionEvent;

    STL_TRY_THROW( sSessEvent->mSessEndEvent < KNL_END_SESSION_OPTION_IMMEDIATE,
                   RAMP_ERR_COMMUNICATION_LINK_FAILURE );

    STL_TRY_THROW( sSessEvent->mStmtEndEvent != KNL_END_STATEMENT_OPTION_CANCEL,
                   RAMP_ERR_OPERATION_CANCELED );
    
    /**
     * Query Timeout 처리
     */
    
    STL_TRY_THROW( sSessEvent->mQueryBeginTime != 0, RAMP_SUCCESS );

    sTimeout = knlGetPropertyBigIntValueByID( KNL_PROPERTY_QUERY_TIMEOUT,
                                              aEnv );
    STL_TRY_THROW( sTimeout != 0, RAMP_SUCCESS );
    
    sNow = knlGetSystemTime();

    STL_TRY_THROW( KNL_CHECK_TIMEOUT( sNow - sSessEvent->mQueryBeginTime,
                                      STL_SET_SEC_TIME(sTimeout) ) == STL_FALSE,
                   RAMP_ERR_QUERY_TIMEOUT );
    
    STL_RAMP( RAMP_SUCCESS );
        
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_QUERY_TIMEOUT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_EXCEEDED_QUERY_TIMEOUT,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_OPERATION_CANCELED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_OPERATION_CANCELED,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_COMMUNICATION_LINK_FAILURE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_COMMUNICATION_LINK_FAILURE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**             
 * @brief Idle Timeout을 활성화 시킨다.
 * @param[out] aEnv  Environment 포인터
 */
void knlEnableIdleTimeout( knlEnv  * aEnv )
{
    knlSessionEvent * sSessEvent;
        
    sSessEvent = &(KNL_SESS_ENV(aEnv)->mSessionEvent);
    sSessEvent->mIdleBeginTime = knlGetSystemTime();
}   

/**
 * @brief Idle Timeout을 비활성화 시킨다.
 * @param[out] aEnv  Environment 포인터
 */
void knlDisableIdleTimeout( knlEnv  * aEnv )
{
    knlSessionEvent * sSessEvent;
    
    sSessEvent = &(KNL_SESS_ENV(aEnv)->mSessionEvent);
    sSessEvent->mIdleBeginTime = 0;
}

/**
 * @brief Idle Timeout을 검사한다.
 * @param[in]  aSessionEnv Session Environment 포인터
 * @param[out] aEnv        Process Environment 포인터
 */
stlStatus knlCheckIdleTimeout( knlSessionEnv * aSessionEnv,
                               knlEnv        * aEnv )
{
    knlSessionEvent * sSessEvent;
    stlTime           sTimeout;
    stlTime           sNow;
    
    STL_TRY( knlCheckEndSession( aSessionEnv, aEnv ) == STL_SUCCESS );

    sSessEvent  = &(aSessionEnv)->mSessionEvent;

    /**
     * Idle Timeout 처리
     */

    STL_TRY_THROW( sSessEvent->mIdleBeginTime != 0, RAMP_SUCCESS );

    sTimeout = knlGetPropertyBigIntValue4SessEnvById( KNL_PROPERTY_IDLE_TIMEOUT,
                                                      aSessionEnv );
    
    STL_TRY_THROW( sTimeout != 0, RAMP_SUCCESS );
    
    sNow = knlGetSystemTime();
    
    STL_TRY_THROW( KNL_CHECK_TIMEOUT( sNow - sSessEvent->mIdleBeginTime,
                                      STL_SET_SEC_TIME(sTimeout) ) == STL_FALSE,
                   RAMP_ERR_IDLE_TIMEOUT );
        
    STL_RAMP( RAMP_SUCCESS );
        
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_IDLE_TIMEOUT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_EXCEEDED_IDLE_TIMEOUT,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief System 전체 Session중에서 file을 open한 session이 있는지 여부를 반환한다.
 * @return file을 open한 session이 있는지 여부
 */
stlBool knlHasFileOpenedSession()
{
    knlSessionEnv * sSessionEnv;
    stlBool         sUsed;
    stlBool         sInitialized;
    
    sSessionEnv = knlGetFirstSessionEnv();

    while( sSessionEnv != NULL )
    {
        sUsed = knlIsUsedSessionEnv( sSessionEnv );
        sInitialized = knlGetInitializedFlag( sSessionEnv, STL_LAYER_KERNEL );
        
        if( (sUsed == STL_TRUE) && (sInitialized == STL_TRUE) )
        {
            if( sSessionEnv->mOpenFileCount > 0 )
            {
                return STL_TRUE;
            }
        }
        
        sSessionEnv = knlGetNextSessionEnv( sSessionEnv );
    }

    return STL_FALSE;
}

/**
 * @brief Session의 Opened File 수를 증가시킨다.
 * @param[in] aSessionEnv Session Environment
 */
void knlIncOpenFileCount( knlSessionEnv * aSessionEnv )
{
    aSessionEnv->mOpenFileCount++;
}

/**
 * @brief Session에 열려 있는 파일이 존재하는지 여부를 반환한다.
 * @param[in] aSessionEnv Session Environment
 */
stlBool knlHasOpenedFile( knlSessionEnv * aSessionEnv )
{
    if( aSessionEnv->mOpenFileCount == 0 )
    {
        return STL_FALSE;
    }
    
    return STL_TRUE;
}

/**
 * @brief Session의 Opened File 수를 감소시킨다.
 * @param[in] aSessionEnv Session Environment
 */
void knlDecOpenFileCount( knlSessionEnv * aSessionEnv )
{
    aSessionEnv->mOpenFileCount--;
}

/**
 * @brief Sessioin Envrionment의 Connection Type을 얻는다.
 * @param[in] aSessEnv  커널 Session Environment
 */
knlConnectionType knlGetSessionConnectionType( knlSessionEnv * aSessEnv )
{
    return aSessEnv->mConnectionType;
}

/**
 * @brief Sessioin Envrionment의 Connection Type을 설정한다.
 * @param[in] aSessEnv    커널 Session Environment
 * @param[in] aConnectionType   설정할 Connection Type
 */
void knlSetSessionConnectionType( knlSessionEnv     * aSessEnv,
                                  knlConnectionType   aConnectionType )
{
    aSessEnv->mConnectionType = aConnectionType;
}

/**
 * @brief Sessioin Envrionment의 Top Layer를 얻는다.
 * @param[in] aSessEnv  커널 Session Environment
 */
stlLayerClass knlGetSessionTopLayer( knlSessionEnv * aSessEnv )
{
    return aSessEnv->mTopLayer;
}

/**
 * @brief Kernel Session Envrionment의 Transaction Scope를 얻는다.
 * @param[in] aSessEnv  커널 Session Environment
 */
knlTransScope knlGetTransScope( void * aSessEnv )
{
    return ((knlSessionEnv*)aSessEnv)->mTransScope;
}

/**
 * @brief Kernel Session Envrionment의 Transaction Scope를 설정한다.
 * @param[in] aSessEnv     커널 Session Environment
 * @param[in] aTransScope  Transaction Scope
 */
void knlSetTransScope( void          * aSessEnv,
                       knlTransScope   aTransScope )
{
    ((knlSessionEnv*)aSessEnv)->mTransScope = aTransScope;
}

/**
 * @brief Sessioin Envrionment의 Top Layer를 설정한다.
 * @param[in] aSessEnv    커널 Session Environment
 * @param[in] aTopLayer   설정할 Top Layer
 */
void knlSetSessionTopLayer( knlSessionEnv * aSessEnv,
                            stlLayerClass   aTopLayer )
{
    aSessEnv->mTopLayer = aTopLayer;
}

/**
 * @brief X$KN_SESS_STAT
 */
knlFxSessionInfo gKnSessStatRows[KNE_KL_SESS_STAT_ROW_COUNT] =
{
    {
        "PARAMETER_MEM_TOTAL_SIZE",
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        0
    },
    {
        "PARAMETER_MEM_INIT_SIZE",
        0,
        KNL_STAT_CATEGORY_INIT_MEMORY,
        0
    },
    {
        "LONG_VARYING_MEM_TOTAL_SIZE",
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        0
    },
    {
        "LONG_VARYING_MEM_INIT_SIZE",
        0,
        KNL_STAT_CATEGORY_INIT_MEMORY,
        0
    }
};

/**
 * @brief X$KN_SESS_STAT Table의 open 함수
 * @param[in]     aStmt          Statement Pointer
 * @param[in]     aDumpObjHandle Dump Object의 Handle
 * @param[in]     aPathCtrl      Path Controller
 * @param[in,out] aEnv           Kernel Environment
 */
stlStatus knlOpenFxKnSessStatCallback( void   * aStmt,
                                       void   * aDumpObjHandle,
                                       void   * aPathCtrl,
                                       knlEnv * aEnv )
{
    kneKnSessStatPathCtrl * sPathCtrl;
    knlSessionEnv         * sSessEnv;
    stlInt64              * sValues;

    sPathCtrl  = (kneKnSessStatPathCtrl *)aPathCtrl;

    sPathCtrl->mSessEnv = NULL;
    sPathCtrl->mIterator = 0;
    sValues = sPathCtrl->mValues;

    /*
     * 연결이 유효한 첫 Proc Env 에 대해 레코드들을 구축한다.
     */
    sSessEnv = (knlSessionEnv*)knlGetFirstSessionEnv();

    while( sSessEnv != NULL )
    {
        if( knlIsUsedSessionEnv( sSessEnv ) == STL_TRUE )
        {
            STL_TRY( knlGetSessionEnvId( sSessEnv,
                                         &sPathCtrl->mSessId,
                                         aEnv )
                     == STL_SUCCESS );
            
            sPathCtrl->mSessEnv = sSessEnv;
            
            /*
             * PARAMETER_MEM_TOTAL_SIZE
             */

            sValues[0] = sSessEnv->mParameterMem.mTotalSize;
            
            /*
             * PARAMETER_MEM_INIT_SIZE
             */

            sValues[1] = sSessEnv->mParameterMem.mInitSize;
            
            /*
             * LONG_VARYING_MEM_TOTAL_SIZE
             */

            sValues[2] = sSessEnv->mLongVaryingMem.mTotalSize;
            
            /*
             * LONG_VARYING_MEM_INIT_SIZE
             */

            sValues[3] = sSessEnv->mLongVaryingMem.mInitSize;
            
            break;
        }
        else
        {
            sSessEnv = (knlSessionEnv*)knlGetNextSessionEnv( sSessEnv );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief X$KL_SESS_STAT Table의 close 함수
 * @param[in]     aDumpObjHandle Dump Object의 Handle
 * @param[in]     aPathCtrl      Path Controller
 * @param[in,out] aEnv           Kernel Environment
 */
stlStatus knlCloseFxKnSessStatCallback( void   * aDumpObjHandle,
                                        void   * aPathCtrl,
                                        knlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief X$KN_SESS_STAT Table의 레코드 구축 함수
 * @param[in]     aDumpObjHandle Dump Object Handle
 * @param[in]     aPathCtrl      Path Controller
 * @param[in,out] aValueList     값을 담을 공간
 * @param[in]     aBlockIdx      Block Idx
 * @param[out]    aTupleExist    레코드 존재 여부
 * @param[in,out] aEnv           Kernel Environment
 */
stlStatus knlBuildRecordFxKnSessStatCallback( void              * aDumpObjHandle,
                                              void              * aPathCtrl,
                                              knlValueBlockList * aValueList,
                                              stlInt32            aBlockIdx,
                                              stlBool           * aTupleExist,
                                              knlEnv            * aEnv )
{
    kneKnSessStatPathCtrl * sPathCtrl;
    knlFxSessionInfo        sFxSessEnvInfo;
    knlFxSessionInfo      * sRowDesc;
    stlInt64              * sValues;
    knlSessionEnv         * sSessEnv;

    *aTupleExist = STL_FALSE;
    
    sPathCtrl = (kneKnSessStatPathCtrl*)aPathCtrl;
    sValues = sPathCtrl->mValues;

    /*
     * 하나의 Proc Env 에 대한 모든 레코드들을 Fetch 했다면,
     * 다음 유효한 Proc Env 에 대해 레코드들을 구축 
     */
    if( sPathCtrl->mIterator >= KNE_KL_SESS_STAT_ROW_COUNT )
    {
        sSessEnv = (knlSessionEnv*)knlGetNextSessionEnv( sPathCtrl->mSessEnv );

        while( sSessEnv != NULL )
        {
            if( knlIsUsedSessionEnv( sSessEnv ) == STL_TRUE )
            {
                STL_TRY( knlGetSessionEnvId( sSessEnv,
                                             & sPathCtrl->mSessId,
                                             aEnv )
                         == STL_SUCCESS );
                
                sPathCtrl->mSessEnv = sSessEnv;
                
                /*
                 * PARAMETER_MEM_TOTAL_SIZE
                 */

                sValues[0] = sSessEnv->mParameterMem.mTotalSize;
            
                /*
                 * PARAMETER_MEM_INIT_SIZE
                 */

                sValues[1] = sSessEnv->mParameterMem.mInitSize;
            
                /*
                 * LONG_VARYING_MEM_TOTAL_SIZE
                 */

                sValues[2] = sSessEnv->mLongVaryingMem.mTotalSize;
            
                /*
                 * LONG_VARYING_MEM_INIT_SIZE
                 */

                sValues[3] = sSessEnv->mLongVaryingMem.mInitSize;
                
                break;
            }
            else
            {
                sSessEnv = (knlSessionEnv*)knlGetNextSessionEnv( sSessEnv );
            }
        }

        sPathCtrl->mSessEnv = sSessEnv;
        sPathCtrl->mIterator = 0;
    }
    else
    {
        /*
         * 해당 Proc Env 의 레코드 정보가 아직 있음
         */
    }
    
    STL_TRY_THROW( sPathCtrl->mSessEnv != NULL, RAMP_SUCCESS );

    sRowDesc = &gKnSessStatRows[sPathCtrl->mIterator];

    sFxSessEnvInfo.mName     = sRowDesc->mName;
    sFxSessEnvInfo.mCategory = sRowDesc->mCategory;
    sFxSessEnvInfo.mSessId   = sPathCtrl->mSessId;
    sFxSessEnvInfo.mValue    = sValues[sPathCtrl->mIterator];

    STL_TRY( knlBuildFxRecord( gKnlSessionInfoColumnDesc,
                               &sFxSessEnvInfo,
                               aValueList,
                               aBlockIdx,
                               aEnv )
             == STL_SUCCESS );

    *aTupleExist = STL_TRUE;
    sPathCtrl->mIterator += 1;

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief X$KL_SESS_STAT Table 정보
 */
knlFxTableDesc gKnSessStatTabDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    NULL,
    knlOpenFxKnSessStatCallback,
    knlCloseFxKnSessStatCallback,
    knlBuildRecordFxKnSessStatCallback,
    STL_SIZEOF( kneKnSessStatPathCtrl ),
    "X$KN_SESS_STAT",
    "session environment information of Kernel layer",
    gKnlSessionInfoColumnDesc
};

/** @} */
