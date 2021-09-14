/*******************************************************************************
 * knlEnvironment.c
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
 * @file knlEnvironment.c
 * @brief Kernel Layer Environment Routines
 */

#include <knl.h>
#include <knDef.h>
#include <knaLatch.h>
#include <kniProcess.h>

extern knsEntryPoint * gKnEntryPoint;
extern stlInt32        gProcessTableID;

/**
 * @addtogroup knlEnvironment
 * @{
 */

/**
 * @brief Envrionment Pool을 생성한다.
 * @param[in] aEnvCount 생성될 Environment의 갯수
 * @param[in] aEnvSize 생성될 Environment의 크기
 * @param[out] aErrorStack 에러 스택
 */
stlStatus knlCreateEnvPool( stlUInt32       aEnvCount,
                            stlUInt32       aEnvSize,
                            stlErrorStack * aErrorStack )
{
    stlInt32       i = 0;
    
    void         * sAddr;
    stlSize        sSize;
    knlEnv         sEnv;
    stlBool        sInitedEnv;
    stlErrorData * sErrorData;
    stlUInt32      sIdx;
    kneManager   * sEnvMgr;
    kneHeader    * sEnvHeader;
    knlEnv       * sTargetEnv;
    stlInt64       sCacheAlignedSize = 0;
    stlInt64       sEnvManagerSize = 0;
    stlInt64       sEnvSize = 0;

    sInitedEnv = STL_FALSE;

    STL_PARAM_VALIDATE( gKnEntryPoint != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aEnvCount != 0, aErrorStack );
    STL_PARAM_VALIDATE( aEnvSize  != 0, aErrorStack );
    STL_PARAM_VALIDATE( gKnEntryPoint->mCacheAlignedSize != 0, aErrorStack );

    sCacheAlignedSize = gKnEntryPoint->mCacheAlignedSize;

    sEnvManagerSize = STL_ALIGN( STL_SIZEOF(kneManager), sCacheAlignedSize );
    sEnvSize        = STL_ALIGN( (STL_SIZEOF(kneHeader) + aEnvSize), sCacheAlignedSize );

    sSize = sEnvManagerSize + ( sEnvSize * aEnvCount );

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
                   RAMP_ERR_ENV );

    stlMemset( sAddr, 0x00, sSize );

    /*
     * Environment 관리자를 초기화한다.
     */
    sEnvMgr = (kneManager*)sAddr;
    sEnvMgr->mCount       = aEnvCount;
    sEnvMgr->mSize        = aEnvSize;
    sEnvMgr->mFreeEnvList = sAddr + sEnvManagerSize;

    STL_TRY_THROW( knlInitLatch( &(sEnvMgr->mLatch),
                                 STL_TRUE,
                                 &sEnv ) == STL_SUCCESS, RAMP_ERR_ENV );

    /*
     * 생성된 각 Environment를 초기화한다.
     */

    for( sIdx = 0; sIdx < aEnvCount; sIdx++ )
    {
        sEnvHeader = sAddr + sEnvManagerSize + ( sEnvSize * sIdx );
        sEnvHeader->mIdx = sIdx;

        sTargetEnv = (knlEnv*)(((stlChar*)sEnvHeader) + STL_SIZEOF(kneHeader));

        for ( i = STL_LAYER_NONE; i < STL_LAYER_MAX; i++ )
        {
            sTargetEnv->mOnFatalHandling[i] = STL_FALSE;
        }
        
        sTargetEnv->mState      = KNL_ENV_STATE_UNUSED;
        sTargetEnv->mProcessId  = -1;
        sTargetEnv->mCreationTime = 0;

        STL_RING_INIT_DATALINK( &sTargetEnv->mWaitItem,
                                STL_OFFSETOF(knlLatchWaiter, mRing) );

        STL_TRY( stlCreateSemaphore( &sTargetEnv->mWaitItem.mSem,
                                     "EnvSem",
                                     0,
                                     aErrorStack )
                 == STL_SUCCESS );
        
        STL_INIT_ERROR_STACK( &sTargetEnv->mErrorStack );
        
        if( sIdx == (aEnvCount - 1) )
        {
            sEnvHeader->mNextFreeLink = NULL;
        }
        else
        {
            sEnvHeader->mNextFreeLink = (void*)sEnvHeader + sEnvSize;
        }
    }

    gKnEntryPoint->mEnvManager = sEnvMgr;

    STL_TRY( knlFinalizeEnv( &sEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_ENV )
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
 * @brief Envrionment Pool을 종료한다.
 * @param[in] aErrorStack  Error Stack
 */
stlStatus knlDestroyEnvPool( stlErrorStack * aErrorStack )
{
    knlEnv * sEnv;
    
    if( gKnEntryPoint != NULL )
    {
        sEnv = (knlEnv*)knlGetFirstEnv();

        while( sEnv != NULL )
        {
            STL_TRY( stlDestroySemaphore( &sEnv->mWaitItem.mSem,
                                          aErrorStack )
                     == STL_SUCCESS );

            sEnv = (knlEnv*)knlGetNextEnv( sEnv );
        }
    }

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Envrionment를 초기화 한다.
 * @param[out] aEnv     초기화될 Environment 포인터
 * @param[in]  aEnvType Env 종류
 * @note ErrorStack 공간 할당이 실패한 경우에는 에러 메세지를 넘겨줄수 없다.
 */
stlStatus knlInitializeEnv( knlEnv     * aEnv,
                            knlEnvType   aEnvType )
{
    stlUInt32   i;
    stlInt32    sState = 0;

    STL_INIT_ERROR_STACK( KNL_ERROR_STACK( aEnv ) );

    aEnv->mLatchCount     = 0;
    aEnv->mSessionEnv       = NULL;
    aEnv->mEnterCriSecCount = 0;
    aEnv->mEnterNonCancelSecCount = 0;
    aEnv->mCancelThread     = STL_FALSE;
    aEnv->mTopLayer         = STL_LAYER_KERNEL;
    aEnv->mNumaNodeId       = KNI_INVALID_NUMA_NODE_ID;
    aEnv->mRequestGroupId   = -1;

    aEnv->mWaitItem.mForLogFlush = STL_FALSE;
    aEnv->mWaitItem.mFlushSbsn = 0;
    aEnv->mWaitItem.mFlushLsn = 0;
    
    aEnv->mWaitItem.mWaitState = KNL_WAITITEM_STATE_RESUMED;
    aEnv->mWaitItem.mLostSemValue = 0;
    STL_INIT_SPIN_LOCK( aEnv->mWaitItem.mSpinLock );
    
    for( i = 0; i < (KNL_ENV_MAX_LATCH_DEPTH + 1); i++ )
    {
        aEnv->mLatchStack[i] = KNL_LOGICAL_ADDR_NULL;
    }

    /**
     * Create Private Static Area Allocator
     */
    if( aEnvType == KNL_ENV_SHARED )
    {
        aEnv->mIsInShm = STL_TRUE;
        STL_TRY( knlCreateDynamicMem( &aEnv->mPrivateStaticArea,
                                      NULL, /* aParentMem */
                                      KNL_ALLOCATOR_KERNEL_PRIVATE_STATIC_AREA,
                                      KNL_MEM_STORAGE_TYPE_HEAP,
                                      KNL_DEFAULT_PRIVATE_STATIC_AREA_SIZE,
                                      KNL_DEFAULT_PRIVATE_STATIC_AREA_SIZE,
                                      NULL, /* aMemController */
                                      aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        aEnv->mIsInShm = STL_FALSE;
        STL_TRY( knlCreateDynamicMem( &aEnv->mPrivateStaticArea,
                                      NULL, /* aParentMem */
                                      KNL_ALLOCATOR_KERNEL_PRIVATE_STATIC_AREA,
                                      KNL_MEM_STORAGE_TYPE_HEAP,
                                      KNL_DEFAULT_LARGE_REGION_SIZE,
                                      KNL_DEFAULT_LARGE_REGION_SIZE,
                                      NULL, /* aMemController */
                                      aEnv )
                 == STL_SUCCESS );
    }
    sState = 1;

    /**
     * Event 초기화
     */
    STL_TRY( knlInitEvent( &aEnv->mEvent,
                           aEnv ) == STL_SUCCESS );

    /**
     * 자신의 Thread Handle 설정
     */
    STL_TRY( stlThreadSelf( &aEnv->mThread,
                            KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    if( aEnvType == KNL_ENV_SHARED )
    {
        /*
         * aEnv의 주소가 Environment Manager로 부터 할당되었는지 확인한다.
         * == ( aEnv가 Shared Memory 공간에 상주하는지 )
         */
        STL_ASSERT( (knlGetFirstEnv() <= (void*)aEnv) && ((void*)aEnv <= knlGetLastEnv()) );
        aEnv->mIsInShm = STL_TRUE;
    }

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void) knlDestroyDynamicMem( &aEnv->mPrivateStaticArea, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief Envrionment를 종료시킨다.
 * @param[in] aEnv 종료할 Environment 포인터
 */
stlStatus knlFinalizeEnv( knlEnv * aEnv )
{
    STL_ASSERT( aEnv->mLatchCount == 0 );
    STL_ASSERT( aEnv->mEnterCriSecCount == 0 );

    aEnv->mState = KNL_ENV_STATE_AGING;
    
    /**
     * Destory Private Static Area Allocator
     */
    knlValidateDynamicMem( &aEnv->mPrivateStaticArea, aEnv );
    STL_TRY( knlDestroyDynamicMem( &aEnv->mPrivateStaticArea,
                                   aEnv ) == STL_SUCCESS );

#ifdef STL_DEBUG
    stlInt32 sSemaphoreValue = 0;

    if( KNL_IS_SHARED_ENV( aEnv ) == STL_TRUE )
    {
        STL_TRY( stlGetSemaphoreValue( &aEnv->mWaitItem.mSem,
                                       &sSemaphoreValue,
                                       KNL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );

        STL_DASSERT( sSemaphoreValue == 0 );
    }
#endif
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Process를 적당한 Numa Node에 bind한다.
 * @param[in] aEnv 종료할 Environment 포인터
 */
stlStatus knlBindNumaNode( knlEnv * aEnv )
{
#if defined( STL_HAVE_CPUSET )
    
    stlCpuSet         sCpuSet;
    kniNumaInfo     * sNumaInfo;
    kniNumaNodeInfo * sNumaNodeInfo;
    stlInt16          sTopLayer;
    stlInt32          i;
    stlInt32          sNumaNodeId = -1;

    STL_TRY_THROW( KNL_IS_SHARED_ENV(aEnv) == STL_TRUE, RAMP_FINISH );
    STL_TRY_THROW( knlGetTopLayer(aEnv) >= STL_LAYER_SESSION, RAMP_FINISH );
        
    /**
     * decide numa node to bind
     */

    sNumaInfo = &(gKnEntryPoint->mNumaInfo);

    /**
     * Numa 정보가 설정되어 있는지 검사
     */
    STL_TRY_THROW( sNumaInfo->mNodeCount > 0, RAMP_FINISH );
    
    sTopLayer = knlGetTopLayer(aEnv);

    STL_CPU_ZERO( &sCpuSet );

    if( sTopLayer == STL_LAYER_SESSION )
    {
        /**
         * system thread는 무조건 mater node를 사용한다.
         */
        sNumaNodeInfo = &(sNumaInfo->mNodeInfo[sNumaInfo->mMasterNodeId]);
        sNumaNodeId = sNumaInfo->mMasterNodeId;
    }
    else
    {
        /**
         * User session 은 mater node를 우선으로 한다.
         */

        sNumaNodeInfo = &(sNumaInfo->mNodeInfo[sNumaInfo->mMasterNodeId]);

        if( sNumaNodeInfo->mBoundCount >= sNumaNodeInfo->mCpuCount )
        {
            /**
             * Bind가 적게 되어 있는 Node를 찾는다.
             */

            for( i = 0; i < KNI_MAX_NUMA_NODE_COUNT; i++ )
            {
                sNumaNodeInfo = &(sNumaInfo->mNodeInfo[i]);

                if( sNumaNodeInfo->mCpuCount == 0 )
                {
                    continue;
                }

                if( sNumaNodeInfo->mBoundCount < sNumaNodeInfo->mCpuCount )
                {
                    sNumaNodeId = i;
                    break;
                }
            }
        }
        else
        {
            sNumaNodeId = sNumaInfo->mMasterNodeId;
        }

        STL_TRY_THROW( sNumaNodeId >= 0, RAMP_FINISH );
        
        sNumaNodeInfo = &(sNumaInfo->mNodeInfo[sNumaNodeId]);
    }

    for( i = 0; i < sNumaNodeInfo->mCpuCount; i++ )
    {
        STL_CPU_SET( (sNumaNodeInfo->mCpuArray[i]), &sCpuSet );
    }
        
    STL_TRY( stlSetThreadAffinity( &aEnv->mThread,
                                   &sCpuSet,
                                   KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
        
    sNumaNodeInfo->mBoundCount++;

    aEnv->mNumaNodeId = sNumaNodeId;

    STL_RAMP( RAMP_FINISH );
    
#endif

    return STL_SUCCESS;

#if defined( STL_HAVE_CPUSET )

    STL_FINISH;

    return STL_FAILURE;
    
#endif
}

/**
 * @brief Process를 Master Numa Node에 bind한다.
 * @param[in] aEnv 종료할 Environment 포인터
 */
stlStatus knlBindNumaMasterNode( knlEnv * aEnv )
{
#if defined( STL_HAVE_CPUSET )
    
    stlCpuSet         sCpuSet;
    kniNumaInfo     * sNumaInfo;
    kniNumaNodeInfo * sNumaNodeInfo;
    stlInt32          i;

    STL_TRY_THROW( KNL_IS_SHARED_ENV(aEnv) == STL_TRUE, RAMP_FINISH );
    STL_TRY_THROW( knlGetTopLayer(aEnv) >= STL_LAYER_SESSION, RAMP_FINISH );
        
    /**
     * decide numa node to bind
     */

    sNumaInfo = &(gKnEntryPoint->mNumaInfo);

    /**
     * Numa 정보가 설정되어 있는지 검사
     */
    STL_TRY_THROW( sNumaInfo->mNodeCount > 0, RAMP_FINISH );
    
    STL_CPU_ZERO( &sCpuSet );

    sNumaNodeInfo = &(sNumaInfo->mNodeInfo[sNumaInfo->mMasterNodeId]);

    for( i = 0; i < sNumaNodeInfo->mCpuCount; i++ )
    {
        STL_CPU_SET( (sNumaNodeInfo->mCpuArray[i]), &sCpuSet );
    }
        
    STL_TRY( stlSetThreadAffinity( &aEnv->mThread,
                                   &sCpuSet,
                                   KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
        
    sNumaNodeInfo->mBoundCount++;

    aEnv->mNumaNodeId = sNumaInfo->mMasterNodeId;

    STL_RAMP( RAMP_FINISH );
    
#endif

    return STL_SUCCESS;

#if defined( STL_HAVE_CPUSET )

    STL_FINISH;

    return STL_FAILURE;
    
#endif
}

/**
 * @brief Process의 Numa 정보를 정리한다.
 * @param[in] aIsCleanup Cleanup 여부
 * @param[in] aEnv       종료할 Environment 포인터
 */
stlStatus knlUnbindNumaNode( stlBool   aIsCleanup,
                             knlEnv  * aEnv )
{
    
#if defined( STL_HAVE_CPUSET )
    
    kniNumaNodeInfo * sNumaNodeInfo;
    stlCpuSet         sCpuSet;
    stlInt32          i;

    if( gKnEntryPoint != NULL )
    {
        if( aEnv->mNumaNodeId != KNI_INVALID_NUMA_NODE_ID )
        {
            sNumaNodeInfo = &(gKnEntryPoint->mNumaInfo.mNodeInfo[aEnv->mNumaNodeId]);

            STL_DASSERT( sNumaNodeInfo->mBoundCount > 0 );
            
            sNumaNodeInfo->mBoundCount--;

            /**
             * Thread 소유자가 해제하는 경우라면
             */
            if( aIsCleanup == STL_FALSE )
            {
                STL_CPU_ZERO( &sCpuSet );

                for( i = 0; i < gKnEntryPoint->mCpuSetSize; i++ )
                {
                    STL_CPU_SET( i, &sCpuSet );
                }
        
                STL_TRY( stlSetThreadAffinity( &aEnv->mThread,
                                               &sCpuSet,
                                               KNL_ERROR_STACK( aEnv ) )
                         == STL_SUCCESS );
            }
        }

        aEnv->mNumaNodeId = KNI_INVALID_NUMA_NODE_ID;
    }
    
#endif

    return STL_SUCCESS;
    
#if defined( STL_HAVE_CPUSET )

    STL_FINISH;

    return STL_FAILURE;
    
#endif
}

/**
 * @brief Envrionment의 Heap 메모리를 정리한다.
 * @param[in] aEnv 종료할 Environment 포인터
 */
stlStatus knlCleanupHeapMem( knlEnv * aEnv )
{
    /**
     * Destory Private Static Area Allocator
     */
    STL_TRY( knlDestroyDynamicMem( &aEnv->mPrivateStaticArea,
                                   aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Envrionment Pool에서 사용 가능한 Envrionment를 반환한다.
 * @param[out] aEnv 사용 가능한 Envrionment
 * @param[out] aErrorStack 에러 스택
 * @note aErrorStack은 동시성 제어를 하지 않는다.
 */
stlStatus knlAllocEnv( void           ** aEnv,
                       stlErrorStack   * aErrorStack )
{
    stlInt32          i = 0;
    
    kneManager      * sEnvMgr;
    kneHeader       * sFreeEnvHeader;
    knlEnv            sEnv;
    stlErrorData    * sErrorData;
    stlBool           sIsTimedout = STL_FALSE;
    stlInt32          sState = 0;
    void            * sCheckEnv;
    stlThreadHandle   sThreadHandle;

    STL_PARAM_VALIDATE( gKnEntryPoint != NULL, aErrorStack );
    STL_PARAM_VALIDATE( gKnEntryPoint->mEnvManager != NULL, aErrorStack );

    sEnvMgr = gKnEntryPoint->mEnvManager;

    STL_TRY( stlThreadSelf( &sThreadHandle, aErrorStack ) == STL_SUCCESS );

    /*
     * 임시 knlEnv를 초기화 한다.
     */
    STL_TRY( knlInitializeEnv( &sEnv, KNL_ENV_HEAP ) == STL_SUCCESS );
    sState = 1;

    STL_TRY_THROW( knlAcquireLatch( &(sEnvMgr->mLatch),
                                    KNL_LATCH_MODE_EXCLUSIVE,
                                    KNL_LATCH_PRIORITY_NORMAL,
                                    STL_INFINITE_TIME,
                                    &sIsTimedout,
                                    &sEnv ) == STL_SUCCESS, RAMP_ERR_ENV );
    sState = 2;

    /*
     * 같은 Thread Handle을 갖고 있는지 확인한다.
     */

    sCheckEnv = knlGetFirstEnv();

    while( sCheckEnv != NULL )
    {
        if( KNL_GET_ENV_STATE(sCheckEnv) == KNL_ENV_STATE_USED )
        {
            STL_TRY_THROW( stlIsEqualThread( &(KNL_ENV(sCheckEnv)->mThread),
                                             &sThreadHandle ) != STL_TRUE,
                           RAMP_ERR_SERVER_REJECTED_THE_CONNECTION);
        }

        sCheckEnv = knlGetNextEnv( sCheckEnv );
    }

    /*
     * Environment 관리자에서 사용 가능한 Environment를 할당 받는다.
     */
    sFreeEnvHeader = sEnvMgr->mFreeEnvList;

    STL_TRY_THROW( sFreeEnvHeader != NULL, RAMP_ERR_INSUFFICIENT_RESOURCE );

    sEnvMgr->mFreeEnvList = sFreeEnvHeader->mNextFreeLink;
    sFreeEnvHeader->mNextFreeLink = NULL;
    
    sState = 1;
    STL_TRY_THROW( knlReleaseLatch( &(sEnvMgr->mLatch),
                                    &sEnv ) == STL_SUCCESS, RAMP_ERR_ENV );

    sState = 0;
    STL_TRY_THROW( knlFinalizeEnv( &sEnv ) == STL_SUCCESS, RAMP_ERR_ENV );

    *aEnv = (void*)sFreeEnvHeader + STL_SIZEOF(kneHeader);

    for ( i = STL_LAYER_NONE; i < STL_LAYER_MAX; i++ )
    {
        ((knlEnv*)*aEnv)->mOnFatalHandling[i] = STL_FALSE;
    }
    
    ((knlEnv*)*aEnv)->mState = KNL_ENV_STATE_USED;
    ((knlEnv*)*aEnv)->mEnterCriSecCount = 0;
    ((knlEnv*)*aEnv)->mEnterNonCancelSecCount = 0;
    ((knlEnv*)*aEnv)->mCancelThread = STL_FALSE;
    ((knlEnv*)*aEnv)->mProcessId = -1;
    ((knlEnv*)*aEnv)->mIsInShm = STL_TRUE;
    ((knlEnv*)*aEnv)->mCreationTime = stlNow();

    stlMemset( ((knlEnv*)*aEnv)->mLatchStack,
               0x00,
               STL_SIZEOF(knlLogicalAddr) * (KNL_ENV_MAX_LATCH_DEPTH + 1) );
    ((knlEnv*)*aEnv)->mLatchCount = 0;
    
    STL_TRY_THROW( kniAttachProcessTable( ((knlEnv*)*aEnv) ) == STL_SUCCESS,
                   RAMP_ERR_ATTACH_PROCESS_TABLE );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_ENV )
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

    STL_CATCH( RAMP_ERR_ATTACH_PROCESS_TABLE )
    {
        STL_ASSERT( knlInitializeEnv( &sEnv, KNL_ENV_HEAP ) == STL_SUCCESS );
        STL_ASSERT( knlAcquireLatch( &(sEnvMgr->mLatch),
                                     KNL_LATCH_MODE_EXCLUSIVE,
                                     KNL_LATCH_PRIORITY_NORMAL,
                                     STL_INFINITE_TIME,
                                     &sIsTimedout,
                                     &sEnv ) == STL_SUCCESS );

        sFreeEnvHeader->mNextFreeLink = sEnvMgr->mFreeEnvList;
        sEnvMgr->mFreeEnvList = sFreeEnvHeader;
        
        ((knlEnv*)*aEnv)->mState = KNL_ENV_STATE_UNUSED;
        ((knlEnv*)*aEnv)->mEnterCriSecCount = 0;
        ((knlEnv*)*aEnv)->mEnterNonCancelSecCount = 0;
        ((knlEnv*)*aEnv)->mCancelThread = STL_FALSE;
        ((knlEnv*)*aEnv)->mProcessId = -1;

        STL_INIT_THREAD_HANDLE( &(((knlEnv*)*aEnv)->mThread) );

        stlAppendErrors( aErrorStack, KNL_ERROR_STACK( *aEnv ) );
        
        STL_ASSERT( knlReleaseLatch( &(sEnvMgr->mLatch), &sEnv ) == STL_SUCCESS );
        STL_ASSERT( knlFinalizeEnv( &sEnv ) == STL_SUCCESS );
    }
    
    STL_CATCH( RAMP_ERR_INSUFFICIENT_RESOURCE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INSUFFICIENT_RESOURCE,
                      "exceeded maximum number of connections",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_SERVER_REJECTED_THE_CONNECTION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_SERVER_REJECTED_THE_CONNECTION,
                      "The data source rejected the establishment of the connection "
                      "for implementation-defined reasons.",
                      aErrorStack );
    }

    STL_FINISH;

    switch( sState )
    {
        case 2 :
            knlReleaseLatch( &(sEnvMgr->mLatch),
                             &sEnv );
        case 1 :
            knlFinalizeEnv( &sEnv );
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief 사용한 Envrionment를 Envrionment Pool에 반납한다.
 * @param[out] aEnv 반납할 Envrionment
 * @param[out] aErrorStack 에러 스택
 */
stlStatus knlFreeEnv( void           * aEnv,
                      stlErrorStack  * aErrorStack )
{
    stlInt32       i = 0;
    
    kneManager   * sEnvMgr;
    kneHeader    * sEnvHeader;
    knlEnv         sEnv;
    stlErrorData * sErrorData;
    stlBool        sIsSuccess = STL_FALSE;
    stlInt32       sState = 0;
    stlUInt32      sEnvId = 0;

    STL_PARAM_VALIDATE( gKnEntryPoint != NULL, aErrorStack );
    STL_PARAM_VALIDATE( gKnEntryPoint->mEnvManager != NULL, aErrorStack );

    STL_TRY( kniDetachProcessTable( ((knlEnv*)aEnv) ) == STL_SUCCESS );

    sEnvMgr = gKnEntryPoint->mEnvManager;

    sEnvHeader = (kneHeader*)(aEnv - STL_SIZEOF(kneHeader));

    /*
     * 임시 knlEnv를 초기화 한다.
     */
    STL_TRY( knlInitializeEnv( &sEnv, KNL_ENV_HEAP ) == STL_SUCCESS );
    sState = 1;

    while( sIsSuccess == STL_FALSE )
    {
        STL_TRY_THROW( knlTryLatch( &(sEnvMgr->mLatch),
                                    KNL_LATCH_MODE_EXCLUSIVE,
                                    &sIsSuccess,
                                    &sEnv ) == STL_SUCCESS, RAMP_ERR_ENV );
    }
    sState = 2;

    if( ((knlEnv*)aEnv)->mState == KNL_ENV_STATE_UNUSED )
    {
        knlGetEnvId( aEnv, &sEnvId );
        knlLogMsgUnsafe( NULL,
                         aEnv,
                         KNL_LOG_LEVEL_WARNING,
                         "already freed PROCESS ENV (%ld)\n",
                         sEnvId );
        
        STL_THROW( RAMP_SUCCESS );
    }

    /*
     * free 되는 Environment를 Environment 관리자에 반환한다.
     */
    sEnvHeader->mNextFreeLink = sEnvMgr->mFreeEnvList;
    sEnvMgr->mFreeEnvList = sEnvHeader;

    for ( i = STL_LAYER_NONE; i < STL_LAYER_MAX; i++ )
    {
        ((knlEnv*)aEnv)->mOnFatalHandling[i] = STL_FALSE;
    }

    ((knlEnv*)aEnv)->mState = KNL_ENV_STATE_UNUSED;
    ((knlEnv*)aEnv)->mEnterCriSecCount = 0;
    ((knlEnv*)aEnv)->mEnterNonCancelSecCount = 0;
    ((knlEnv*)aEnv)->mCancelThread = STL_FALSE;
    ((knlEnv*)aEnv)->mProcessId = -1;

    stlMemset( ((knlEnv*)aEnv)->mLatchStack,
               0x00,
               STL_SIZEOF(knlLogicalAddr) * (KNL_ENV_MAX_LATCH_DEPTH + 1) );
    ((knlEnv*)aEnv)->mLatchCount = 0;
            
    STL_INIT_THREAD_HANDLE( &((knlEnv*)aEnv)->mThread );

    STL_RAMP( RAMP_SUCCESS );
    
    sState = 1;
    STL_TRY_THROW( knlReleaseLatch( &(sEnvMgr->mLatch),
                                    &sEnv ) == STL_SUCCESS, RAMP_ERR_ENV );
    
    sState = 0;
    STL_TRY_THROW( knlFinalizeEnv( &sEnv ) == STL_SUCCESS, RAMP_ERR_ENV );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_ENV )
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

    switch( sState )
    {
        case 2 :
            knlReleaseLatch( &(sEnvMgr->mLatch),
                             &sEnv );
        case 1 :
            knlFinalizeEnv( &sEnv );
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief Envrionment 에 Request Group의 id를 기록한다.
 * @param[in] aRequestGroupId    Request Group Identifier
 * @param[in] aEnv               Environment
 */
void knlSetRequestGroupId( stlInt32   aRequestGroupId,
                           knlEnv   * aEnv )
{
    aEnv->mRequestGroupId = aRequestGroupId;
}

/**
 * @brief Envrionment Pool에서 첫번째 Environment를 반환한다.
 * @remark 동시성 제어를 하지 않는다.
 */
void * knlGetFirstEnv()
{
    void * sEnv;

    STL_TRY( gKnEntryPoint != NULL );
    STL_TRY( gKnEntryPoint->mEnvManager != NULL );
    STL_TRY( gKnEntryPoint->mCacheAlignedSize != 0 );

    sEnv = (void*)gKnEntryPoint->mEnvManager +
           STL_ALIGN( STL_SIZEOF(kneManager), gKnEntryPoint->mCacheAlignedSize ) +
           STL_SIZEOF(kneHeader);

    return sEnv;

    STL_FINISH;

    return NULL;
}

/**
 * @brief Envrionment Pool에서 마지막 Environment를 반환한다.
 * @remark 동시성 제어를 하지 않는다.
 */
void * knlGetLastEnv()
{
    void       * sEnv;
    kneManager * sEnvManager;
    stlInt64     sCacheAlignedSize = 0;
    stlInt64     sEnvManagerSize = 0;
    stlInt64     sEnvSize = 0;

    STL_TRY( gKnEntryPoint != NULL );
    STL_TRY( gKnEntryPoint->mEnvManager != NULL );
    STL_TRY( gKnEntryPoint->mCacheAlignedSize != 0 );

    sEnvManager = gKnEntryPoint->mEnvManager;

    sCacheAlignedSize = gKnEntryPoint->mCacheAlignedSize;

    sEnvManagerSize = STL_ALIGN( STL_SIZEOF(kneManager), sCacheAlignedSize );
    sEnvSize        = STL_ALIGN( (STL_SIZEOF(kneHeader) + sEnvManager->mSize),
                                 sCacheAlignedSize );

    sEnv = ( (void*)sEnvManager +
             sEnvManagerSize +
             (sEnvSize * (gKnEntryPoint->mEnvManager->mCount - 1)) +
             STL_SIZEOF(kneHeader) );
    
    return sEnv;

    STL_FINISH;

    return NULL;
}

/**
 * @brief 입력받은 Envrionment의 다음 Environment를 반환한다.
 * @param[in] aEnv Environment
 * @return 다음 Environment가 없을 경우 NULL을 반환한다.
 * @remark 동시성 제어를 하지 않는다.
 */
void * knlGetNextEnv( void * aEnv )
{
    kneHeader  * sEnv;
    kneHeader  * sNextEnv;
    stlUInt32    sIdx;
    kneManager * sEnvManager;
    stlInt64     sCacheAlignedSize = 0;
    stlInt64     sEnvManagerSize = 0;
    stlInt64     sEnvSize = 0;

    STL_TRY( gKnEntryPoint != NULL );
    STL_TRY( gKnEntryPoint->mEnvManager != NULL );
    STL_TRY( gKnEntryPoint->mCacheAlignedSize != 0 );

    sEnvManager = gKnEntryPoint->mEnvManager;

    sCacheAlignedSize = gKnEntryPoint->mCacheAlignedSize;

    sEnvManagerSize = STL_ALIGN( STL_SIZEOF(kneManager), sCacheAlignedSize );
    sEnvSize        = STL_ALIGN( (STL_SIZEOF(kneHeader) + sEnvManager->mSize),
                                 sCacheAlignedSize );

    sEnv = (kneHeader*)(aEnv - STL_SIZEOF(kneHeader));
    sIdx = sEnv->mIdx + 1;

    STL_TRY( sIdx < sEnvManager->mCount );

    sNextEnv = (void*)sEnvManager +
               sEnvManagerSize +
               ( sEnvSize * sIdx );

    return (void*)sNextEnv + STL_SIZEOF(kneHeader);

    STL_FINISH;

    return NULL;
}

/**
 * @brief Envrionment Pool에서 자신의 PROCESS에 속한 첫번째 Environment를 반환한다.
 * @remark 동시성 제어를 하지 않는다.
 */
void * knlGetFirstMyProcessEnv()
{
    knlEnv         * sMyEnv = NULL;
    knlEnv         * sEnv;
    stlProc          sProc;
    stlErrorStack    sErrorStack;
    knlSessionEnv  * sSessionEnv;

    STL_TRY( gKnEntryPoint != NULL );
    STL_TRY( gKnEntryPoint->mEnvManager != NULL );
    
    STL_INIT_ERROR_STACK( &sErrorStack );
    
    STL_TRY_THROW( stlGetCurrentProc( &sProc,
                                      &sErrorStack )
                   == STL_SUCCESS, RAMP_ERR );

    sEnv = knlGetFirstEnv();

    while( sEnv != NULL )
    {
        if( knlIsUsedEnv( sEnv ) == STL_TRUE )
        {
            if( stlIsEqualProc( &sEnv->mThread.mProc, &sProc ) == STL_TRUE )
            {
                sSessionEnv = KNL_SESS_ENV( sEnv );

                if( sSessionEnv != NULL )
                {
                    sMyEnv = sEnv;
                    break;
                }
            }
        }
        
        sEnv = knlGetNextEnv( sEnv );
    }

    STL_RAMP( RAMP_ERR );

    return sMyEnv;
    
    STL_FINISH;

    return NULL;
}

/**
 * @brief Envrionment Pool에서 자신의 PROCESS에 속한 다음 Environment를 반환한다.
 * @remark 동시성 제어를 하지 않는다.
 */
void * knlGetNextMyProcessEnv( knlEnv * aEnv )
{
    knlEnv         * sMyEnv = NULL;
    knlEnv         * sEnv;
    stlErrorStack    sErrorStack;
    stlProc          sProc;
    knlSessionEnv  * sSessionEnv;

    STL_TRY( gKnEntryPoint != NULL );
    STL_TRY( gKnEntryPoint->mEnvManager != NULL );
    
    STL_INIT_ERROR_STACK( &sErrorStack );
    
    STL_TRY_THROW( stlGetCurrentProc( &sProc,
                                      &sErrorStack )
                   == STL_SUCCESS, RAMP_ERR );

    sEnv = knlGetNextEnv( aEnv );

    while( sEnv != NULL )
    {
        if( knlIsUsedEnv( sEnv ) == STL_TRUE )
        {
            if( stlIsEqualProc( &sEnv->mThread.mProc, &sProc ) == STL_TRUE )
            {
                sSessionEnv = KNL_SESS_ENV( sEnv );

                if( sSessionEnv != NULL )
                {
                    sMyEnv = sEnv;
                    break;
                }
            }
        }
        
        sEnv = knlGetNextEnv( sEnv );
    }

    STL_RAMP( RAMP_ERR );

    return sMyEnv;
    
    STL_FINISH;

    return NULL;
}

/**
 * @brief 입력받은 Envrionment의 ID를 반환한다.
 * @param[in] aEnv Environment
 * @param[out] aId Environment의 ID
 */
stlStatus knlGetEnvId( void      * aEnv,
                       stlUInt32 * aId )
{
    kneHeader  * sEnv;

    STL_TRY( gKnEntryPoint != NULL );
    STL_TRY( gKnEntryPoint->mEnvManager != NULL );
    STL_TRY( aId != NULL );

    sEnv = (kneHeader*)(aEnv - STL_SIZEOF(kneHeader));

    *aId = sEnv->mIdx;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 Environment 아이디에 해당되는 Environment 포인터를 반환한다.
 * @param[in] aEnvId Environment의 ID
 * @return Environment 포인터
 */
void * knlGetEnv( stlUInt32 aEnvId )
{
    kneHeader  * sEnv;
    kneManager * sEnvManager;
    stlInt64     sCacheAlignedSize = 0;
    stlInt64     sEnvManagerSize = 0;
    stlInt64     sEnvSize = 0;

    sEnvManager = gKnEntryPoint->mEnvManager;

    sCacheAlignedSize = gKnEntryPoint->mCacheAlignedSize;

    sEnvManagerSize = STL_ALIGN( STL_SIZEOF(kneManager), sCacheAlignedSize );
    sEnvSize        = STL_ALIGN( (STL_SIZEOF(kneHeader) + sEnvManager->mSize),
                                 sCacheAlignedSize );

    sEnv = (void*)sEnvManager +
           sEnvManagerSize +
           ( sEnvSize * aEnvId );

    return (void*)sEnv + STL_SIZEOF(kneHeader);
}

/**
 * @brief 자신의 Environment를 얻는다.
 * @return 찾았다면 찾은 Environment 포인터를 반환하고, 못찾았다면 NULL을 반환한다.
 * @remarks
 * <BR> Env를 찾지 못한 경우는 아래와 같다.
 * <BR> 1) 자신의 Thread Handle을 얻지 못하는 경우
 * <BR> 2) Environemnt내에 구한 Thread handle을 가진 Env가 없는 경우
 */
void * knlGetMyEnv()
{
    knlEnv          * sEnv;
    knlEnv          * sMyEnv = NULL;
    stlThreadHandle   sThread;
    stlErrorStack     sErrorStack;
    knlSessionEnv   * sSessionEnv;

    STL_INIT_ERROR_STACK( &sErrorStack );
    
    STL_TRY_THROW( stlThreadSelf( &sThread,
                                  &sErrorStack )
                   == STL_SUCCESS, RAMP_ERR );
    
    sEnv = knlGetFirstEnv();

    while( sEnv != NULL )
    {
        if( knlIsUsedEnv( sEnv ) == STL_TRUE )
        {
            if( stlIsEqualThread( &sEnv->mThread, &sThread ) == STL_TRUE )
            {
                sSessionEnv = KNL_SESS_ENV( sEnv );

                if( sSessionEnv != NULL )
                {
                    if( knlIsTerminatedSessionEnv( sSessionEnv ) == STL_FALSE )
                    {
                        sMyEnv = sEnv;
                        break;
                    }
                }
            }
        }
        
        sEnv = knlGetNextEnv( sEnv );
    }

    STL_RAMP( RAMP_ERR );

    return sMyEnv;
}

void knlGetEnvStateString( knlEnv   * aEnv,
                           stlChar  * aStateString,
                           stlInt16   aBufferLength )
{
    stlInt32 i = 0;
    
    switch( aEnv->mState )
    {
        case KNL_ENV_STATE_AGING:
            stlStrncpy( aStateString, "AGING", aBufferLength );
            break;
        case KNL_ENV_STATE_UNUSED:
            stlStrncpy( aStateString, "UNUSED", aBufferLength );
            break;
        case KNL_ENV_STATE_USED:
            stlStrncpy( aStateString, "USED", aBufferLength );
            break;
        default:
            STL_ASSERT(0);
            break;
    }

    for ( i = STL_LAYER_NONE; i < STL_LAYER_MAX; i++ )
    {
        if ( aEnv->mOnFatalHandling[i] == STL_TRUE )
        {
            stlStrncpy( aStateString, "FATAL HANDLING", aBufferLength );
            break;
        }
    }
}

/**
 * @brief 입력받은 2개의 Env가 같은 Env인지 판단한다.
 * @param[in] aEnv1 Environment1
 * @param[in] aEnv2 Environment2
 * @return 같으면 STL_TRUE, 그렇지 않다면 STL_FALSE
 */
stlBool knlIsEqualEnv( void * aEnv1, void * aEnv2 )
{
    return stlIsEqualThread( &(KNL_ENV(aEnv1)->mThread),
                             &(KNL_ENV(aEnv2)->mThread) );
}

/**
 * @brief 입력받은 Envrionment의 사용여부를 판단한다.
 * @param[in] aEnv Environment
 */
stlBool knlIsUsedEnv( void * aEnv )
{
    if( ((knlEnv*)aEnv)->mState == KNL_ENV_STATE_UNUSED )
    {
        return STL_FALSE;
    }
    
    return STL_TRUE;
}

/**
 * @brief Envrionment에 Top Layer를 설정한다.
 * @param[in]     aLayer  설정할 Top Layer
 * @param[in,out] aEnv    커널 Environment
 */
void knlSetTopLayer( stlLayerClass    aLayer,
                     knlEnv         * aEnv )
{
    aEnv->mTopLayer = aLayer;
}

/**
 * @brief Envrionment의 Top Layer를 얻는다.
 * @param[in,out] aEnv    커널 Environment
 */
stlInt16 knlGetTopLayer( knlEnv * aEnv )
{
    return aEnv->mTopLayer;
}

/**
 * @brief X$KN_PROC_ENV Table 정의
 */
knlFxColumnDesc gKnEnvColumnDesc[] =
{
    {
        "ID",
        "environment ID",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( kneEnvFxRecord, mID ),
        STL_SIZEOF( stlUInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "TOP_LAYER",
        "top layer name",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( kneEnvFxRecord, mTopLayer ),
        STL_MAX_LAYER_NAME + 1,
        STL_FALSE  /* nullable */
    },
    {
        "STATE",
        "environment state",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( kneEnvFxRecord, mState ),
        KNE_STATE_LENGTH + 1,
        STL_FALSE  /* nullable */
    },
    {
        "NUMA_NODE_ID",
        "default numa node identifier",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( kneEnvFxRecord, mNumaNodeId ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "OS_PROC_ID",
        "os process identifier",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( kneEnvFxRecord, mOsProcId ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "PROCESS_ID",
        "process ID",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( kneEnvFxRecord, mProcessId ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "CREATION_TIME",
        "creation time",
        KNL_FXTABLE_DATATYPE_TIMESTAMP,
        STL_OFFSETOF( kneEnvFxRecord, mCreationTime ),
        STL_SIZEOF( stlTime ),
        STL_FALSE  /* nullable */
    },
    {
        "CRITICAL_SECTION_COUNT",
        "critical section count",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( kneEnvFxRecord, mEnterCriSecCount ),
        STL_SIZEOF( stlUInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "ACQUIRE_LATCH_COUNT",
        "acquire latch count",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( kneEnvFxRecord, mLatchCount ),
        STL_SIZEOF( stlUInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "REQ_LATCH_MODE",
        "request latch mode",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( kneEnvFxRecord, mReqLatchMode ),
        KNE_REQ_MODE_LENGTH + 1,
        STL_FALSE  /* nullable */
    },
    {
        "NEXT_FREE_ID",
        "next free environment ID",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( kneEnvFxRecord, mNextFreeID ),
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

/**
 * @brief X$KN_PROC_ENV Table의 open 함수
 * @param[in] aStmt Statement Pointer
 * @param[in] aDumpObjHandle Dump Object의 Handle
 * @param[in] aPathCtrl Path Controller
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus openFxKnEnvCallback( void   * aStmt,
                               void   * aDumpObjHandle,
                               void   * aPathCtrl,
                               knlEnv * aEnv )
{
    knlEnvPathControl * sCtl = (knlEnvPathControl*)aPathCtrl;

    sCtl->mEnv = knlGetFirstEnv();

    return STL_SUCCESS;
}

/**
 * @brief X$KN_PROC_ENV Table의 close 함수
 * @param[in] aDumpObjHandle Dump Object의 Handle
 * @param[in] aPathCtrl Path Controller
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus closeFxKnEnvCallback( void   * aDumpObjHandle,
                                void   * aPathCtrl,
                                knlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief X$KN_PROC_ENV Table의 레코드 구축 함수
 * @param[in] aDumpObjHandle Dump Object Handle
 * @param[in] aPathCtrl      Path Controller
 * @param[in,out] aValueList 값을 담을 공간
 * @param[in]  aBlockIdx   Block Idx
 * @param[out] aTupleExist 레코드 존재 여부
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus buildRecordFxKnEnvCallback( void              * aDumpObjHandle,
                                      void              * aPathCtrl,
                                      knlValueBlockList * aValueList,
                                      stlInt32            aBlockIdx,
                                      stlBool           * aTupleExist,
                                      knlEnv            * aEnv )
{
    knlEnvPathControl * sCtl = (knlEnvPathControl*)aPathCtrl;
    kneEnvFxRecord      sRec;
    knlEnv            * sEnv;
    kneHeader         * sHeader;
    kneHeader         * sFree;

    *aTupleExist = STL_FALSE;

    if( sCtl->mEnv != NULL )
    {
        sEnv = (knlEnv*)sCtl->mEnv;
        sHeader = (kneHeader*)(sCtl->mEnv - STL_SIZEOF(kneHeader));
        sFree = (kneHeader*)sHeader->mNextFreeLink;

        STL_TRY( knlGetEnvId( (void*)sEnv, &(sRec.mID) ) == STL_SUCCESS );

        if( sFree != NULL )
        {
            sRec.mNextFreeID = sFree->mIdx;
        }
        else
        {
            sRec.mNextFreeID = -1;
        }

        sRec.mLatchCount       = sEnv->mLatchCount;
        sRec.mEnterCriSecCount = sEnv->mEnterCriSecCount;
        sRec.mProcessId        = sEnv->mProcessId;
        sRec.mCreationTime     = sEnv->mCreationTime;
        sRec.mNumaNodeId       = sEnv->mNumaNodeId;
        sRec.mOsProcId         = sEnv->mThread.mProc.mProcID;

        knlGetEnvStateString( sEnv, sRec.mState, KNE_STATE_LENGTH );
    
        stlStrcpy( sRec.mReqLatchMode, knaGetLatchModeString( sEnv->mWaitItem.mReqMode ) );

        stlStrcpy( sRec.mTopLayer, gLayerString[sEnv->mTopLayer] );

        STL_TRY( knlBuildFxRecord( gKnEnvColumnDesc,
                                   &sRec,
                                   aValueList,
                                   aBlockIdx,
                                   aEnv ) == STL_SUCCESS );

        *aTupleExist = STL_TRUE;

        sCtl->mEnv = knlGetNextEnv( (void*)sEnv );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief X$KN_PROC_ENV Table 정보
 */
knlFxTableDesc gKnEnvTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    NULL,
    openFxKnEnvCallback,
    closeFxKnEnvCallback,
    buildRecordFxKnEnvCallback,
    STL_SIZEOF( kneEnvFxRecord ),
    "X$KN_PROC_ENV",
    "process environment information of kernel layer",
    gKnEnvColumnDesc
};

/**
 * @brief X$KN_PROC_STAT
 */
knlFxSessionInfo gKnProcStatRows[KNE_KL_PROC_STAT_ROW_COUNT] =
{
    {
        "PRIVATE_STATIC_AREA_TOTAL_SIZE",
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        0
    },
    {
        "PRIVATE_STATIC_AREA_INIT_SIZE",
        0,
        KNL_STAT_CATEGORY_INIT_MEMORY,
        0
    }
};

/**
 * @brief X$KN_PROC_STAT Table의 open 함수
 * @param[in]     aStmt          Statement Pointer
 * @param[in]     aDumpObjHandle Dump Object의 Handle
 * @param[in]     aPathCtrl      Path Controller
 * @param[in,out] aEnv           Kernel Environment
 */
stlStatus openFxKnProcStatCallback( void   * aStmt,
                                    void   * aDumpObjHandle,
                                    void   * aPathCtrl,
                                    knlEnv * aEnv )
{
    kneKnProcStatPathCtrl * sPathCtrl;
    void                  * sProcEnv;
    knlEnv                * sEnv;
    stlInt64              * sValues;

    sPathCtrl  = (kneKnProcStatPathCtrl *)aPathCtrl;

    sPathCtrl->mCurrEnv = NULL;
    sPathCtrl->mIterator = 0;
    sValues = sPathCtrl->mValues;

    /*
     * 연결이 유효한 첫 Proc Env 에 대해 레코드들을 구축한다.
     */
    sProcEnv = knlGetFirstEnv();

    while( sProcEnv != NULL )
    {
        if( knlIsUsedEnv( sProcEnv ) == STL_TRUE )
        {
            STL_TRY( knlGetEnvId( sProcEnv,
                                  &sPathCtrl->mProcId )
                     == STL_SUCCESS );
            
            sPathCtrl->mCurrEnv = sProcEnv;
            sEnv = KNL_ENV(sProcEnv);
            
            /*
             * PRIVATE_STATIC_AREA_TOTAL_SIZE
             */

            sValues[0] = sEnv->mPrivateStaticArea.mTotalSize;
            
            /*
             * PRIVATE_STATIC_AREA_INIT_SIZE
             */

            sValues[1] = sEnv->mPrivateStaticArea.mInitSize;
            
            break;
        }
        else
        {
            sProcEnv = knlGetNextEnv( sProcEnv );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief X$KL_PROC_STAT Table의 close 함수
 * @param[in]     aDumpObjHandle Dump Object의 Handle
 * @param[in]     aPathCtrl      Path Controller
 * @param[in,out] aEnv           Kernel Environment
 */
stlStatus closeFxKnProcStatCallback( void   * aDumpObjHandle,
                                     void   * aPathCtrl,
                                     knlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief X$KN_PROC_STAT Table의 레코드 구축 함수
 * @param[in]     aDumpObjHandle Dump Object Handle
 * @param[in]     aPathCtrl      Path Controller
 * @param[in,out] aValueList     값을 담을 공간
 * @param[in]     aBlockIdx      Block Idx
 * @param[out]    aTupleExist    레코드 존재 여부
 * @param[in,out] aEnv           Kernel Environment
 */
stlStatus buildRecordFxKnProcStatCallback( void              * aDumpObjHandle,
                                           void              * aPathCtrl,
                                           knlValueBlockList * aValueList,
                                           stlInt32            aBlockIdx,
                                           stlBool           * aTupleExist,
                                           knlEnv            * aEnv )
{
    kneKnProcStatPathCtrl * sPathCtrl;
    knlFxSessionInfo        sFxProcEnvInfo;
    knlFxSessionInfo      * sRowDesc;
    stlInt64              * sValues;
    void                  * sProcEnv;
    knlEnv                * sEnv;

    *aTupleExist = STL_FALSE;
    
    sPathCtrl = (kneKnProcStatPathCtrl*)aPathCtrl;
    sValues = sPathCtrl->mValues;

    /*
     * 하나의 Proc Env 에 대한 모든 레코드들을 Fetch 했다면,
     * 다음 유효한 Proc Env 에 대해 레코드들을 구축 
     */
    if( sPathCtrl->mIterator >= KNE_KL_PROC_STAT_ROW_COUNT )
    {
        sProcEnv = knlGetNextEnv( sPathCtrl->mCurrEnv );

        while( sProcEnv != NULL )
        {
            if( knlIsUsedEnv( sProcEnv ) == STL_TRUE )
            {
                STL_TRY( knlGetEnvId( sProcEnv,
                                      & sPathCtrl->mProcId )
                         == STL_SUCCESS );
                
                sPathCtrl->mCurrEnv = sProcEnv;
                sEnv = KNL_ENV(sProcEnv);
                
                /*
                 * PRIVATE_STATIC_AREA_TOTAL_SIZE
                 */

                sValues[0] = sEnv->mPrivateStaticArea.mTotalSize;
            
                /*
                 * PRIVATE_STATIC_AREA_INIT_SIZE
                 */

                sValues[1] = sEnv->mPrivateStaticArea.mInitSize;
                
                break;
            }
            else
            {
                sProcEnv = knlGetNextEnv( sProcEnv );
            }
        }

        sPathCtrl->mCurrEnv = sProcEnv;
        sPathCtrl->mIterator = 0;
    }
    else
    {
        /*
         * 해당 Proc Env 의 레코드 정보가 아직 있음
         */
    }
    
    STL_TRY_THROW( sPathCtrl->mCurrEnv != NULL, RAMP_SUCCESS );

    sRowDesc = &gKnProcStatRows[sPathCtrl->mIterator];

    sFxProcEnvInfo.mName     = sRowDesc->mName;
    sFxProcEnvInfo.mCategory = sRowDesc->mCategory;
    sFxProcEnvInfo.mSessId   = sPathCtrl->mProcId;
    sFxProcEnvInfo.mValue    = sValues[sPathCtrl->mIterator];

    STL_TRY( knlBuildFxRecord( gKnlSessionInfoColumnDesc,
                               &sFxProcEnvInfo,
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
 * @brief X$KL_PROC_STAT Table 정보
 */
knlFxTableDesc gKnProcStatTabDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    NULL,
    openFxKnProcStatCallback,
    closeFxKnProcStatCallback,
    buildRecordFxKnProcStatCallback,
    STL_SIZEOF( kneKnProcStatPathCtrl ),
    "X$KN_PROC_STAT",
    "process environment information of Kernel layer",
    gKnlSessionInfoColumnDesc
};

/** @} */
