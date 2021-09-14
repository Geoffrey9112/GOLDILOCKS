/*******************************************************************************
 * knlPlanCache.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: knlPlanCache.c 9033 2013-07-17 08:40:27Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file knlPlanCache.c
 * @brief Plan Cache
 */

#include <knl.h>
#include <knDef.h>
#include <knpc.h>

extern knsEntryPoint * gKnEntryPoint;


/**
 * @brief 주어진 SQL String과 관련된 SQL Handle의 Reference Count를 증가시킨다.
 * @param[in]     aSqlString     Target SQL String
 * @param[in]     aSqlLength     SQL String Length
 * @param[in]     aUserInfo      User Information
 * @param[in]     aCursorInfo    Cursor Information
 * @param[out]    aSqlHandle     SQL Handle
 * @param[out]    aSqlInfo       SQL Information
 * @param[in,out] aEnv           environment 정보
 */
stlStatus knlFixSql( stlChar           * aSqlString,
                     stlInt64            aSqlLength,
                     knlPlanUserInfo   * aUserInfo,
                     knlPlanCursorInfo * aCursorInfo,
                     knlSqlHandle      * aSqlHandle,
                     knlPlanSqlInfo    * aSqlInfo,
                     knlEnv            * aEnv )
{
    stlInt32         sHashValue;
    knpcHashBucket * sHashBucket;
    stlBool          sIsTimedOut;
    stlInt32         sState = 0;
    knlSqlHandle     sSqlHandle = NULL;
    knpcFlange     * sFlange;
    stlBool          sIsSuccess;
    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aSqlString != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSqlHandle != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aUserInfo != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCursorInfo != NULL, KNL_ERROR_STACK(aEnv) );

    STL_TRY_THROW( knlGetPropertyBoolValueByID( KNL_PROPERTY_PLAN_CACHE,
                                                KNL_ENV(aEnv) ) == STL_TRUE,
                   RAMP_FINISH );

    sHashValue = dtlHash32Any( (const stlUInt8*)aSqlString, aSqlLength );
    sHashBucket = knpcGetHashBucket( sHashValue );

    STL_TRY( knlAcquireLatch( &sHashBucket->mLatch,
                              KNL_LATCH_MODE_SHARED,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 1;

    sSqlHandle = (knlSqlHandle)knpcFindHashItem( sHashBucket,
                                                 sHashValue,
                                                 aSqlString,
                                                 aSqlLength,
                                                 aUserInfo,
                                                 aCursorInfo );

    if( sSqlHandle != NULL )
    {
        sFlange = (knpcFlange*)sSqlHandle;
        
        stlAcquireSpinLock( &sFlange->mSpinLock,
                            NULL /* Miss count */ );

        /**
         * knpcFindHashItem() 이후 Aging 되었다면
         */
        
        if( KNPC_IS_EMPTY_FLANGE( sFlange ) == STL_TRUE )
        {
            sSqlHandle = NULL;
        }
        else
        {
            sFlange->mLastFixTime = knlGetSystemTime();
            sFlange->mRefCount++;
            sFlange->mAge = 0;
            
            *aSqlInfo = sFlange->mSqlInfo;
        }

        stlReleaseSpinLock( &sFlange->mSpinLock );
    }

    sState = 0;
    STL_TRY( knlReleaseLatch( &sHashBucket->mLatch,
                              KNL_ENV( aEnv ) ) == STL_SUCCESS );

    STL_RAMP( RAMP_FINISH );

    *aSqlHandle = sSqlHandle;
    
    if( sSqlHandle != NULL )
    {
        /**
         * 공간 할당에 실패하면 Fixing 실패로 처리한다.
         */
        
        STL_TRY( knlAddFixedSql( sSqlHandle,
                                 STL_TRUE, /* aIgnoreError */
                                 &sIsSuccess,
                                 aEnv ) == STL_SUCCESS );

        if( sIsSuccess == STL_FALSE )
        {
            STL_TRY( knlUnfixSql( sSqlHandle, aEnv ) == STL_SUCCESS );
            
            *aSqlHandle = NULL;
        }
    }

    return STL_SUCCESS;
    
    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlReleaseLatch( &sHashBucket->mLatch, KNL_ENV( aEnv ) );
    }

    return STL_FAILURE;
}


/**
 * @brief 주어진 SQL Handle의 Reference Count를 감소시킨다.
 * @param[in]     aSqlHandle     SQL Handle
 * @param[in,out] aEnv           environment 정보
 */
stlStatus knlUnfixSql( knlSqlHandle   aSqlHandle,
                       knlEnv       * aEnv )
{
    knpcFlange  * sFlange;
    
    sFlange = (knpcFlange*)aSqlHandle;

    STL_DASSERT( sFlange->mRefCount > 0 );
    
    stlAcquireSpinLock( &sFlange->mSpinLock,
                        NULL /* Miss count */ );
    sFlange->mRefCount--;
    stlReleaseSpinLock( &sFlange->mSpinLock );
    
    STL_TRY( knlRemoveFixedSql( aSqlHandle,
                                aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 주어진 SQL Handle을 Discard 시킨다.
 * @param[in]     aSqlHandle     SQL Handle
 * @param[in,out] aEnv           environment 정보
 * @remarks Discard 표시만 한다. 물리적인 삭제는 Aging때 이루어진다.
 */
stlStatus knlDiscardSql( knlSqlHandle   aSqlHandle,
                         knlEnv       * aEnv )
{
    knpcFlange     * sFlange;
    knpcHashBucket * sHashBucket;
    stlBool          sIsTimedOut;
    stlInt32         sState = 0;

    sFlange = (knpcFlange*)aSqlHandle;

    STL_DASSERT( sFlange->mRefCount > 0 );
    
    sHashBucket = knpcGetHashBucket( sFlange->mHashValue );
    
    STL_TRY( knlAcquireLatch( &sHashBucket->mLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 1;
    
    stlAcquireSpinLock( &sFlange->mSpinLock,
                        NULL /* Miss count */ );
    sFlange->mDropped = STL_TRUE;
    stlReleaseSpinLock( &sFlange->mSpinLock );

    STL_RING_UNLINK( &(sHashBucket->mOverflowList), sFlange );
    
    sState = 0;
    STL_TRY( knlReleaseLatch( &sHashBucket->mLatch,
                              KNL_ENV( aEnv ) ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlReleaseLatch( &sHashBucket->mLatch, KNL_ENV( aEnv ) );
    }

    return STL_FAILURE;
}


/**
 * @brief 모든 Clock을 정리한다.
 * @param[in,out] aEnv environment 정보
 */
stlStatus knlCleanupPlans( knlEnv * aEnv )
{
    stlBool     sIsSuccess;
    knpcClock * sClock;
    stlInt32    i;

    sClock = (knpcClock*)(gKnEntryPoint->mClock);
    
    for( i = 0; i < gKnEntryPoint->mClockCount; i++ )
    {
        STL_TRY( knpcAgingClock( &sClock[i],
                                 0,               /* aDeadAge */
                                 STL_INT32_MAX,   /* aAgingThreshold */
                                 0,               /* aAgingPlanInterval */
                                 &sIsSuccess,
                                 aEnv )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 주어진 SQL을 Caching한다.
 * @param[in]     aSqlString     Target SQL String
 * @param[in]     aSqlLength     SQL String Length
 * @param[in]     aPlanMem       Plan Region Memory
 * @param[in]     aCodePlan      Code Plan Address
 * @param[in]     aUserInfo      User Information
 * @param[in]     aCursorInfo    Cursor Information
 * @param[in]     aSqlInfo       SQL Information
 * @param[in]     aCompareFunc   Plan Compare Function
 * @param[out]    aSqlHandle     SQL Handle ( nullable )
 * @param[in,out] aEnv           environment 정보
 */
stlStatus knlCacheSql( stlChar            * aSqlString,
                       stlInt64             aSqlLength,
                       knlRegionMem       * aPlanMem,
                       void               * aCodePlan,
                       knlPlanUserInfo    * aUserInfo,
                       knlPlanCursorInfo  * aCursorInfo,
                       knlPlanSqlInfo     * aSqlInfo,
                       knlComparePlanFunc   aCompareFunc,
                       knlSqlHandle       * aSqlHandle,
                       knlEnv             * aEnv )
{
    stlInt32         sHashValue;
    knpcHashBucket * sHashBucket;
    stlBool          sIsTimedOut;
    stlInt32         sState = 0;
    knlSqlHandle     sSqlHandle = NULL;
    knpcFlange     * sFlange;
    knpcPlan       * sPlan;
    knpcClock      * sClock;
    stlBool          sIsSuccess;
    stlBool          sSamePlan;

    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aSqlString != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPlanMem != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aUserInfo != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCursorInfo != NULL, KNL_ERROR_STACK(aEnv) );
    
    sHashValue = dtlHash32Any( (const stlUInt8*)aSqlString, aSqlLength );
    sHashBucket = knpcGetHashBucket( sHashValue );

    /**
     * New SQL을 구성한다.
     * - Critical Section 구간을 줄이기 위해서 Latch 획득전에 할당하고,
     *   이미 Caching 되어 있으면 할당된 Cache를 삭제한다.
     */

    STL_TRY( knpcGetClock( &sClock, aEnv ) == STL_SUCCESS );

    STL_TRY( knpcAllocFlange( sClock,
                              sHashValue,
                              aSqlString,
                              aSqlLength,
                              aUserInfo,
                              aCursorInfo,
                              aSqlInfo,
                              &sFlange,
                              aEnv ) == STL_SUCCESS );

    /**
     * SQL을 위한 적당한 공간이 없다면 Caching하지 않는다.
     */
    
    STL_TRY_THROW( sFlange != NULL, RAMP_FINISH );

    STL_TRY( knlAcquireLatch( &sHashBucket->mLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 1;

    /**
     * 기존 같은 Plan이 있는지 확인한다.
     * - Critical Section 구간이 길어진다면 SQL Plan을 중복해서 등록하는
     *   방안도 고려해볼수 있다.
     */
    
    sSqlHandle = (void*)knpcFindHashItem( sHashBucket,
                                          sHashValue,
                                          aSqlString,
                                          aSqlLength,
                                          aUserInfo,
                                          aCursorInfo );

    if( sSqlHandle == NULL )
    {
        sPlan = knpcAddPlan( sClock,
                             sFlange,
                             aPlanMem,
                             aCodePlan );
        
        /**
         * New SQL을 Hash에 삽입한다.
         */

        STL_RING_ADD_FIRST( &(sHashBucket->mOverflowList), sFlange );
        sFlange->mHashBucketId = sHashBucket->mBucketId;
        
        sSqlHandle = (knlSqlHandle)sFlange;
    }
    else
    {
        STL_TRY( knpcFreeFlange( sClock,
                                 sFlange,
                                 aEnv ) == STL_SUCCESS );

        sFlange = (knpcFlange*)sSqlHandle;
        
        sSamePlan = knpcComparePlan( sFlange, aCompareFunc, aSqlInfo, aCodePlan );

        /**
         * 기존 SQL에 New Plan을 삽입한다.
         */

        stlAcquireSpinLock( &sFlange->mSpinLock,
                            NULL /* Miss count */ );

        if( sSamePlan == STL_FALSE )
        {
            sPlan = knpcAddPlan( sClock,
                                 sFlange,
                                 aPlanMem,
                                 aCodePlan );
        }
        else
        {
            sPlan = NULL;
        }
        
        if( sPlan == NULL )
        {
            stlReleaseSpinLock( &sFlange->mSpinLock );
            
            sState = 0;
            STL_TRY( knlReleaseLatch( &sHashBucket->mLatch,
                                      KNL_ENV( aEnv ) ) == STL_SUCCESS );
            
            sSqlHandle = NULL;
            STL_THROW( RAMP_FINISH );
        }
        else
        {
            sFlange->mRefCount++;
            sFlange->mAge = 0;
            stlReleaseSpinLock( &sFlange->mSpinLock );
        }
    }

    sState = 0;
    STL_TRY( knlReleaseLatch( &sHashBucket->mLatch,
                              KNL_ENV( aEnv ) ) == STL_SUCCESS );

    STL_TRY( knlStealRegionMem( aPlanMem,
                                NULL,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_RAMP( RAMP_FINISH );

    if( aSqlHandle != NULL )
    {
        *aSqlHandle = sSqlHandle;
    }

    if( sSqlHandle != NULL )
    {
        /**
         * 공간 할당에 실패하면 Caching 실패로 처리한다.
         */
        
        STL_TRY( knlAddFixedSql( sSqlHandle,
                                 STL_TRUE, /* aIgnoreError */
                                 &sIsSuccess,
                                 aEnv ) == STL_SUCCESS );

        if( sIsSuccess == STL_FALSE )
        {
            STL_TRY( knlUnfixSql( sSqlHandle, aEnv ) == STL_SUCCESS );
            
            *aSqlHandle = NULL;
        }
    }

    return STL_SUCCESS;
    
    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlReleaseLatch( &sHashBucket->mLatch, KNL_ENV( aEnv ) );
    }

    return STL_FAILURE;
}


/**
 * @brief 기존 SQL Handle에 New Plan을 Caching한다.
 * @param[in]     aSqlHandle     SQL Handle
 * @param[in]     aPlanMem       Plan Region Memory
 * @param[in]     aCodePlan      Code Plan Address
 * @param[out]    aPlanHandle    PLAN Handle ( nullable )
 * @param[in,out] aEnv           environment 정보
 * @note knlFixSql()이 호출된 이후에 사용되어야 한다.
 */
stlStatus knlCachePlan( knlSqlHandle     aSqlHandle,
                        knlRegionMem   * aPlanMem,
                        void           * aCodePlan,
                        knlPlanHandle  * aPlanHandle,
                        knlEnv         * aEnv )
{
    knpcFlange * sFlange;
    knpcPlan   * sPlan;
    knpcClock  * sClock;
    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aSqlHandle != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, KNL_ERROR_STACK(aEnv) );

    STL_TRY( knpcGetClock( &sClock, aEnv ) == STL_SUCCESS );
    
    sFlange = (knpcFlange*)aSqlHandle;
    
    STL_DASSERT( sFlange->mRefCount > 0 );

    /**
     * Flange에 연결한다.
     */
    
    stlAcquireSpinLock( &sFlange->mSpinLock,
                        NULL /* Miss count */ );
    
    sPlan = knpcAddPlan( sClock,
                         sFlange,
                         aPlanMem,
                         aCodePlan );
    stlReleaseSpinLock( &sFlange->mSpinLock );

    if( aPlanHandle != NULL )
    {
        *aPlanHandle = (void*)sPlan;
        
        STL_TRY( knlStealRegionMem( aPlanMem,
                                    NULL,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Session에 Binding되어 있는 Clock Allocator를 반환한다.
 * @param[in,out] aEnv  environment 정보
 */
knlDynamicMem * knlGetPlanRegionMem( knlEnv * aEnv )
{
    knpcClock * sClock;
    
    STL_TRY( knpcGetClock( &sClock, aEnv ) == STL_SUCCESS );

    return &(sClock->mDynamicMem);

    STL_FINISH;

    return NULL;
}


/**
 * @brief User Info를 설정한다.
 * @param[out]     aUserInfo      User Information
 * @param[in]      aUserId        User Identifier
 */
void knlSetPlanUserInfo( knlPlanUserInfo  * aUserInfo,
                         stlInt64           aUserId )
{
    aUserInfo->mUserId = aUserId;
}


/**
 * @brief SQL Info를 설정한다.
 * @param[out]     aSqlInfo    SQL Information
 * @param[in]      aIsQuery    Query or DML
 * @param[in]      aStmtType   SQL Statement Yype
 * @param[in]      aBindCount  Bind Count
 * @param[in]      aPrivCount  Privilege Count
 * @param[in]      aPrivSize   Privilege Size
 * @param[in]      aPrivArray  Privilege Array
 */
void knlSetPlanSqlInfo( knlPlanSqlInfo  * aSqlInfo,
                        stlBool           aIsQuery,
                        stlInt32          aStmtType,
                        stlInt32          aBindCount,
                        stlInt32          aPrivCount,
                        stlInt32          aPrivSize,
                        void            * aPrivArray )
{
    aSqlInfo->mIsQuery  = aIsQuery;
    
    aSqlInfo->mStmtType = aStmtType;
    aSqlInfo->mBindCount = aBindCount;
    aSqlInfo->mPrivCount     = aPrivCount;
    aSqlInfo->mPrivArraySize = aPrivSize;
    aSqlInfo->mPrivArray     = aPrivArray;
}


/**
 * @brief Cursor Info를 설정한다.
 * @param[out] aCursorInfo      Statement 의 Cursor Information
 * @param[in]  aIsDbcCursor     ODBC/JDBC cursor 속성 여부
 * @param[in]  aStandardType    Cursor Standard Type
 * @param[in]  aSensitivity     Cursor Sensitivity
 * @param[in]  aScrollability   Cursor Scrollability
 * @param[in]  aHoldability     Cursor Holdability
 * @param[in]  aUpdatability    Cursor Updatability
 * @param[in]  aReturnability   Cursor Returnability
 * @remarks
 */
void knlSetPlanStmtCursorInfo( knlPlanCursorInfo        * aCursorInfo,
                               stlBool                    aIsDbcCursor,
                               stlInt32                   aStandardType,
                               stlInt32                   aSensitivity,
                               stlInt32                   aScrollability,
                               stlInt32                   aHoldability,
                               stlInt32                   aUpdatability,
                               stlInt32                   aReturnability )
                               /* ellCursorStandardType      aStandardType, */
                               /* ellCursorSensitivity       aSensitivity, */
                               /* ellCursorScrollability     aScrollability, */
                               /* ellCursorHoldability       aHoldability, */
                               /* ellCursorUpdatability      aUpdatability, */
                               /* ellCursorReturnability     aReturnability ) */
{
    aCursorInfo->mIsDbcCursor   = aIsDbcCursor;

    aCursorInfo->mStandardType  = aStandardType;
    aCursorInfo->mSensitivity   = aSensitivity;
    aCursorInfo->mScrollability = aScrollability;
    aCursorInfo->mHoldability   = aHoldability;
    aCursorInfo->mUpdatability  = aUpdatability;
    aCursorInfo->mReturnability = aReturnability;
    
}

/**
 * @brief SQL Info를 설정한다.
 * @param[in]      aSqlHandle  SQL Handle
 * @param[out]     aSqlInfo    SQL Information
 */
void knlGetPlanSqlInfo( knlSqlHandle    * aSqlHandle,
                        knlPlanSqlInfo  * aSqlInfo )
{
    knpcFlange * sFlange;
    
    sFlange = (knpcFlange*)aSqlHandle;
    *aSqlInfo = sFlange->mSqlInfo;
}


/**
 * @brief Iterator를 초기화 시킨다.
 * @param[in]     aIterator     Plan Iterator
 * @param[in]     aSqlHandle    SQL Handle
 * @param[in,out] aEnv          environment 정보
 */
stlStatus knlInitPlanIterator( knlPlanIterator * aIterator,
                               knlSqlHandle    * aSqlHandle,
                               knlEnv          * aEnv )
{
    aIterator->mPlanIdx = 0;
    aIterator->mSqlHandle = aSqlHandle;
    
    return STL_SUCCESS;
}
    

/**
 * @brief Next Plan을 얻는다.
 * @param[in]     aIterator     Plan Iterator
 * @param[out]    aCodePlan     Code Plan Pointer
 * @param[out]    aPlanSize     Code Plan Size
 * @param[in,out] aEnv          environment 정보
 */
stlStatus knlPlanFetchNext( knlPlanIterator  * aIterator,
                            void            ** aCodePlan,
                            stlInt64         * aPlanSize,
                            knlEnv           * aEnv )
{
    knpcFlange * sFlange;
    knpcPlan   * sPlan;

    *aCodePlan = NULL;
    
    sFlange = (knpcFlange*)(aIterator->mSqlHandle);

    while( aIterator->mPlanIdx < KNPC_MAX_PLAN_COUNT )
    {
        sPlan = &(sFlange->mPlans[aIterator->mPlanIdx]);

        if( sPlan->mIsUsed == STL_FALSE )
        {
            aIterator->mPlanIdx++;
            continue;
        }

        *aCodePlan = sPlan->mCodePlan;
        *aPlanSize = knlGetArenaSize( sPlan->mPlanMem );
        aIterator->mPlanIdx++;
        
        break;
    }

    return STL_SUCCESS;
}


/**
 * @brief Iterator를 종료 시킨다.
 * @param[in]     aIterator     Plan Iterator
 * @param[in,out] aEnv          environment 정보
 */
stlStatus knlFiniPlanIterator( knlPlanIterator * aIterator,
                               knlEnv          * aEnv )
{
    return STL_SUCCESS;
}


/**
 * @brief Flange Iterator를 초기화 시킨다.
 * @param[in]     aIterator     Plan Iterator
 * @param[in,out] aEnv          environment 정보
 */
stlStatus knlInitFlangeIterator( knlFlangeIterator * aIterator,
                                 knlEnv            * aEnv )
{
    stlBool          sIsTimedOut;
    stlInt32         sState = 0;
    knpcClock      * sClock;
    
    sClock = (knpcClock*)(gKnEntryPoint->mClock);
    
    STL_TRY( knlAcquireLatch( &(sClock->mLatch),
                              KNL_LATCH_MODE_SHARED,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              aEnv )
             == STL_SUCCESS );
    sState = 1;

    aIterator->mClockId     = sClock->mClockId;
    aIterator->mFlangeBlock = (void*)STL_RING_GET_FIRST_DATA( &(sClock->mBlockList) );
    aIterator->mFlangeId    = 0;
    aIterator->mSqlHandle   = NULL;
    
    sState = 0;
    STL_TRY( knlReleaseLatch( &(sClock->mLatch),
                              KNL_ENV( aEnv ) ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlReleaseLatch( &(sClock->mLatch), KNL_ENV( aEnv ) );
    }

    return STL_FAILURE;
}
    

/**
 * @brief Next Flange를 얻는다.
 * @param[in]     aIterator     Plan Iterator
 * @param[out]    aSqlHandle    SQL Handle
 * @param[out]    aRefCount     Reference Count
 * @param[in,out] aEnv          environment 정보
 */
stlStatus knlFlangeFetchNext( knlFlangeIterator  * aIterator,
                              knlSqlHandle      ** aSqlHandle,
                              stlInt64           * aRefCount,
                              knlEnv             * aEnv )
{
    knpcFlange * sFlange;
    knpcClock  * sClock = NULL;
    stlInt32     sState = 0;
    stlInt32     sFlangeState = 0;
    stlBool      sIsSuccess = STL_FALSE;
    stlBool      sIsTimedOut;

    *aSqlHandle = NULL;
    *aRefCount  = 0;
    
    sClock = &(((knpcClock*)(gKnEntryPoint->mClock))[aIterator->mClockId]);

    if( aIterator->mSqlHandle != NULL )
    {
        /**
         * 이전 flange를 unfix 한다.
         */

        sFlange = (knpcFlange*)aIterator->mSqlHandle;

        stlAcquireSpinLock( &sFlange->mSpinLock, NULL );
        sFlange->mRefCount--;
        stlReleaseSpinLock( &sFlange->mSpinLock );
        
        STL_TRY( knlRemoveFixedSql( sFlange, aEnv ) == STL_SUCCESS );

        aIterator->mSqlHandle = NULL;
    }

    while( 1 )
    {
        if( aIterator->mFlangeId >= KNPC_MAX_FLANGE_COUNT )
        {
            /**
             * move to next flange block
             */

            STL_TRY( knlAcquireLatch( &(sClock->mLatch),
                                      KNL_LATCH_MODE_SHARED,
                                      KNL_LATCH_PRIORITY_NORMAL,
                                      STL_INFINITE_TIME,
                                      &sIsTimedOut,
                                      aEnv )
                     == STL_SUCCESS );
            sState = 1;
        
            aIterator->mFlangeBlock = STL_RING_GET_NEXT_DATA( &(sClock->mBlockList),
                                                              aIterator->mFlangeBlock );
        
            if( STL_RING_IS_HEADLINK( &(sClock->mBlockList), aIterator->mFlangeBlock ) == STL_TRUE )
            {
                sState = 0;
                STL_TRY( knlReleaseLatch( &(sClock->mLatch),
                                          KNL_ENV( aEnv ) ) == STL_SUCCESS );

                aIterator->mClockId++;
            
                /**
                 * move to next clock
                 */

                STL_TRY_THROW( aIterator->mClockId < gKnEntryPoint->mClockCount, RAMP_FINISH );

                sClock = &(((knpcClock*)(gKnEntryPoint->mClock))[aIterator->mClockId]);
            
                STL_TRY( knlAcquireLatch( &(sClock->mLatch),
                                          KNL_LATCH_MODE_SHARED,
                                          KNL_LATCH_PRIORITY_NORMAL,
                                          STL_INFINITE_TIME,
                                          &sIsTimedOut,
                                          aEnv )
                         == STL_SUCCESS );
                sState = 1;
            
                aIterator->mClockId     = sClock->mClockId;
                aIterator->mFlangeBlock = (void*)STL_RING_GET_FIRST_DATA( &(sClock->mBlockList) );
            }
        
            aIterator->mFlangeId = 0;

            sState = 0;
            STL_TRY( knlReleaseLatch( &(sClock->mLatch),
                                      KNL_ENV( aEnv ) ) == STL_SUCCESS );
        }
    
        sFlange = &(((knpcFlangeBlock*)(aIterator->mFlangeBlock))->mFlanges[aIterator->mFlangeId]);
        
        stlAcquireSpinLock( &sFlange->mSpinLock, NULL );
        aIterator->mFlangeId++;

        if( (KNPC_IS_EMPTY_FLANGE( sFlange ) == STL_TRUE) || (sFlange->mPlanCount == 0 ) )
        {
            /**
             * move to next flange
             */
            
            stlReleaseSpinLock( &sFlange->mSpinLock );
        }
        else
        {
            /**
             * found target flange
             */

            *aRefCount = sFlange->mRefCount;
            sFlange->mRefCount++;
            sFlangeState = 1;
            stlReleaseSpinLock( &sFlange->mSpinLock );
            
            break;
        }
    }
    
    STL_TRY( knlAddFixedSql( sFlange,
                             STL_FALSE, /* aIgnoreError */
                             &sIsSuccess,
                             aEnv )
             == STL_SUCCESS );
    sFlangeState = 2;

    *aSqlHandle = (knlSqlHandle)sFlange;
    aIterator->mSqlHandle = (knlSqlHandle)sFlange;

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void) knlReleaseLatch( &(sClock->mLatch), KNL_ENV( aEnv ) );
        default:
            break;
    }

    switch( sFlangeState )
    {
        case 2:
            (void) knlRemoveFixedSql( sFlange, aEnv );
        case 1:
            stlAcquireSpinLock( &sFlange->mSpinLock, NULL );
            sFlange->mRefCount--;
            stlReleaseSpinLock( &sFlange->mSpinLock );
            aIterator->mSqlHandle = NULL;
        default:
            break;
    }

    return STL_FAILURE;
}


/**
 * @brief Flange Iterator를 종료 시킨다.
 * @param[in]     aIterator     Plan Iterator
 * @param[in,out] aEnv          environment 정보
 */
stlStatus knlFiniFlangeIterator( knlFlangeIterator * aIterator,
                                 knlEnv            * aEnv )
{
    knpcFlange * sFlange = NULL;
    
    if( aIterator->mSqlHandle != NULL )
    {
        /**
         * 이전 flange를 unfix 한다.
         */

        sFlange = (knpcFlange*)aIterator->mSqlHandle;

        stlAcquireSpinLock( &sFlange->mSpinLock, NULL );
        sFlange->mRefCount--;
        stlReleaseSpinLock( &sFlange->mSpinLock );
        
        STL_TRY( knlRemoveFixedSql( sFlange, aEnv ) == STL_SUCCESS );

        aIterator->mSqlHandle = NULL;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Fixed SQL 정보를 Fixing Block에 추가한다.
 * @param[in]     aSqlHandle    SQL Handle
 * @param[in]     aIgnoreError  Error 발생 여부
 * @param[out]    aIsSuccess    성공 여부
 * @param[in,out] aEnv          environment 정보
 */
stlStatus knlAddFixedSql( knlSqlHandle   aSqlHandle,
                          stlBool        aIgnoreError,
                          stlBool      * aIsSuccess,
                          knlEnv       * aEnv )
{
    stlBool          sFound = STL_FALSE;
    stlInt32         i;
    knlSqlFixBlock * sSqlFixIter;
    knlSessionEnv  * sSessionEnv;
    knlSqlFixBlock * sSqlFixBlock;
    
    sSessionEnv = KNL_SESS_ENV( aEnv );
    
    STL_RING_FOREACH_ENTRY( &(sSessionEnv->mSqlFixList), sSqlFixIter )
    {
        for( i = 0; i < KNL_SQL_FIX_ARRAY_SIZE; i++ )
        {
            if( sSqlFixIter->mSqlArray[i] == NULL )
            {
                sFound = STL_TRUE;
                break;
            }
        }

        if( sFound == STL_TRUE )
        {
            break;
        }
    }

    if( sFound == STL_TRUE )
    {
        sSqlFixIter->mSqlArray[i] = aSqlHandle;
    }
    else
    {
        if( knlAllocDynamicMem( &(gKnEntryPoint->mSqlFixMem),
                                STL_SIZEOF(knlSqlFixBlock),
                                (void**)&sSqlFixBlock,
                                KNL_ENV(aEnv) )
            != STL_SUCCESS )
        {
            STL_TRY( aIgnoreError == STL_TRUE );
            
            stlPopError( KNL_ERROR_STACK(aEnv) );
        }
        else
        {
            stlMemset( sSqlFixBlock, 0x00, STL_SIZEOF(knlSqlFixBlock) );
    
            sSqlFixBlock->mIsDefaultBlock = STL_FALSE;
            STL_RING_INIT_DATALINK( sSqlFixBlock,
                                    STL_OFFSETOF(knlSqlFixBlock, mFixBlockLink) );

            STL_RING_ADD_LAST( &(sSessionEnv->mSqlFixList), sSqlFixBlock );

            sSqlFixBlock->mSqlArray[0] = aSqlHandle;
            sFound = STL_TRUE;
        }
    }

    *aIsSuccess = sFound;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Fixed SQL 정보를 Fixing Block에서 삭제한다.
 * @param[in]     aSqlHandle    SQL Handle
 * @param[in,out] aEnv          environment 정보
 */
stlStatus knlRemoveFixedSql( knlSqlHandle   aSqlHandle,
                             knlEnv       * aEnv )
{
    stlInt32         i;
    knlSqlFixBlock * sSqlFixIter;
    knlSessionEnv  * sSessionEnv;

    sSessionEnv = KNL_SESS_ENV( aEnv );
    
    STL_RING_FOREACH_ENTRY( &(sSessionEnv->mSqlFixList), sSqlFixIter )
    {
        for( i = 0; i < KNL_SQL_FIX_ARRAY_SIZE; i++ )
        {
            if( sSqlFixIter->mSqlArray[i] == aSqlHandle )
            {
                sSqlFixIter->mSqlArray[i] = NULL;
                STL_THROW( RAMP_FINISH );
            }
        }
    }

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;
}
