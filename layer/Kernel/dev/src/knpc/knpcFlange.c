/*******************************************************************************
 * knpcFlange.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: knpcFlange.c 9033 2013-07-17 08:40:27Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file knpcFlange.c
 * @brief Plan Flange
 */

#include <knl.h>
#include <knDef.h>
#include <knpc.h>


/**
 * @brief Flange를 초기화 한다.
 * @param[in]     aFlange      Flange Pointer
 * @param[in]     aHashValue   Hash Value
 * @param[in]     aSqlString   SQL String
 * @param[in]     aSqlLength   SQL Length
 * @param[in]     aUserInfo    User Information
 * @param[in]     aCursorInfo  Cursor Information
 * @param[in]     aSqlInfo     SQL Information
 * @param[in]     aReInit      재초기화 여부
 */
void knpcInitFlange( knpcFlange        * aFlange,
                     stlInt32            aHashValue,
                     stlChar           * aSqlString,
                     stlInt32            aSqlLength,
                     knlPlanUserInfo   * aUserInfo,
                     knlPlanCursorInfo * aCursorInfo,
                     knlPlanSqlInfo    * aSqlInfo,
                     stlBool             aReInit )
{
    /**
     * intitialize the flange
     */
    
    stlMemset( aFlange->mPlans, 0x00, STL_SIZEOF(knpcPlan) * KNPC_MAX_PLAN_COUNT );
    
    STL_RING_INIT_DATALINK( aFlange,
                            STL_OFFSETOF(knpcFlange, mOverflowLink) );

    aFlange->mHashValue = aHashValue;
    aFlange->mHashBucketId = 0;
    aFlange->mRefCount = 0;
    aFlange->mPlanCount = 0;
    aFlange->mAge = 0;
    aFlange->mDropped = STL_FALSE;
    aFlange->mSqlLength = 0;
    aFlange->mLastFixTime = 0;
    aFlange->mSqlInfo.mBindCount = 0;
    aFlange->mSqlInfo.mStmtType = -1;

    if( aReInit == STL_FALSE )
    {
        stlInitSpinLock( &aFlange->mSpinLock );
    }

    /**
     * intitialize User information
     */

    if( aUserInfo != NULL )
    {
        aFlange->mUserInfo = *aUserInfo;
    }

    /**
     * intitialize Cursor information
     */

    if( aCursorInfo != NULL )
    {
        aFlange->mCursorInfo = *aCursorInfo;
    }
    
    /**
     * intitialize SQL information
     */
    
    if( aSqlInfo != NULL )
    {
        aFlange->mSqlInfo.mIsQuery   = aSqlInfo->mIsQuery;
        
        aFlange->mSqlInfo.mBindCount = aSqlInfo->mBindCount;
        aFlange->mSqlInfo.mStmtType = aSqlInfo->mStmtType;

        aFlange->mSqlInfo.mPrivCount = aSqlInfo->mPrivCount;
        aFlange->mSqlInfo.mPrivArraySize = aSqlInfo->mPrivArraySize;
        
        if( aSqlInfo->mPrivArray != NULL )
        {
            stlMemcpy( aFlange->mSqlInfo.mPrivArray,
                       aSqlInfo->mPrivArray,
                       aSqlInfo->mPrivArraySize );
        }
        else
        {
            aFlange->mSqlInfo.mPrivArray = NULL;
        }
    }
    else
    {
        aFlange->mSqlInfo.mPrivArray = NULL;
    }
    
    /**
     * initialize sql string
     */
    
    if( aSqlString != NULL )
    {
        STL_DASSERT( aFlange->mSqlString != NULL );
        
        stlMemcpy( aFlange->mSqlString, aSqlString, aSqlLength );
        aFlange->mSqlString[aSqlLength] = 0;
        aFlange->mSqlLength = aSqlLength;
    }
    else
    {
        aFlange->mSqlString = NULL;
        aFlange->mSqlLength = 0;
    }
}


/**
 * @brief Flange를 재초기화 한다.
 * @param[in]     aFlange      Flange Pointer
 */
void knpcResetFlange( knpcFlange * aFlange )
{
    /**
     * intitialize the flange
     */
    
    stlMemset( aFlange->mPlans, 0x00, STL_SIZEOF(knpcPlan) * KNPC_MAX_PLAN_COUNT );
    
    STL_RING_INIT_DATALINK( aFlange,
                            STL_OFFSETOF(knpcFlange, mOverflowLink) );

    aFlange->mHashValue = 0;
    aFlange->mHashBucketId = 0;
    aFlange->mRefCount = 0;
    aFlange->mPlanCount = 0;
    aFlange->mAge = 0;
    aFlange->mDropped = STL_FALSE;
    aFlange->mSqlLength = 0;
    aFlange->mSqlInfo.mBindCount = 0;
    aFlange->mSqlInfo.mStmtType = -1;

    aFlange->mSqlString = NULL;
    aFlange->mSqlLength = 0;
}


/**
 * @brief Clock상의 Empty Flange를 얻는다.
 * @param[in]     aClock       Clock Pointer
 * @param[in]     aHashValue   Hash Value for SQL
 * @param[in]     aSqlString   SQL String
 * @param[in]     aSqlLength   SQL Length
 * @param[in]     aUserInfo    User Information
 * @param[in]     aCursorInfo  Cursor Information
 * @param[in]     aSqlInfo     SQL Information
 * @param[out]    aFlange      Flange Pointer
 * @param[in,out] aEnv         environment 정보
 */
stlStatus knpcAllocFlange( knpcClock          * aClock,
                           stlInt32             aHashValue,
                           stlChar            * aSqlString,
                           stlInt64             aSqlLength,
                           knlPlanUserInfo    * aUserInfo,
                           knlPlanCursorInfo  * aCursorInfo,
                           knlPlanSqlInfo     * aSqlInfo,
                           knpcFlange        ** aFlange,
                           knlEnv             * aEnv )
{
    knpcFlangeBlock * sFlangeBlock;
    stlInt32          sRetryCount = 0;
    stlBool           sIsSuccess = STL_TRUE;
    stlBool           sIsTimedOut;
    stlInt32          sState = 0;
    stlChar         * sSqlString = NULL;
    knpcClockHand   * sFreeHand;
    knpcClockHand   * sAgingHand;
    stlInt32          i;
    void            * sBuffer = NULL;
    void            * sPrivArray = NULL;
    stlInt32          sSqlLength;
    knpcFlange      * sFlange;
    
    *aFlange = NULL;

    /**
     * Null terminate string
     */
    sSqlLength = aSqlLength + 1;

    /**
     * allocate sql string & bind type array
     */
    
    if( knlAllocDynamicMem( &aClock->mDynamicMem,
                            ( sSqlLength + aSqlInfo->mPrivArraySize),
                            (void**)&sBuffer,
                            KNL_ENV(aEnv) )
        != STL_SUCCESS )
    {
        stlPopError( KNL_ERROR_STACK(aEnv) );

        STL_TRY( knpcAgingClock( aClock,
                                 KNPC_DEAD_AGE,
                                 KNPC_CLOCK_AGING_THRESHOLD,
                                 knlGetPropertyBigIntValueByID( KNL_PROPERTY_AGING_PLAN_INTERVAL,
                                                                aEnv ),
                                 &sIsSuccess,
                                 aEnv ) == STL_SUCCESS );

        if( knlAllocDynamicMem( &aClock->mDynamicMem,
                                (sSqlLength + aSqlInfo->mPrivArraySize),
                                (void**)&sBuffer,
                                KNL_ENV(aEnv) )
            != STL_SUCCESS )
        {
            stlPopError( KNL_ERROR_STACK(aEnv) );

            STL_THROW( RAMP_FINISH );
        }
    }

    sPrivArray = sBuffer;
    sSqlString = sBuffer + aSqlInfo->mPrivArraySize;

    /**
     * allocate flange
     */

    while( sRetryCount < 2 )
    {
        STL_TRY( knlAcquireLatch( &aClock->mLatch,
                                  KNL_LATCH_MODE_EXCLUSIVE,
                                  KNL_LATCH_PRIORITY_NORMAL,
                                  STL_INFINITE_TIME,
                                  &sIsTimedOut,
                                  KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
        sState = 1;
    
        /**
         * Empty Flange를 찾는다.
         */

        sFreeHand = &(aClock->mFreeHand);
        sAgingHand = &(aClock->mAgingHand);
                                  
        sFlangeBlock = sFreeHand->mBlockHand;

        while( 1 )
        {
            for( i = sFreeHand->mFlangeHand; i < KNPC_MAX_FLANGE_COUNT; i++ )
            {
                sFlange = &(sFlangeBlock->mFlanges[i]);
                
                if( KNPC_IS_EMPTY_FLANGE( sFlange ) == STL_TRUE )
                {
                    sFlange->mSqlString = sSqlString;
                    sFlange->mSqlInfo.mPrivArray = sPrivArray;
                    sFlange->mBuffer = sBuffer;
                    
                    knpcInitFlange( sFlange,
                                    aHashValue,
                                    aSqlString,
                                    aSqlLength,
                                    aUserInfo,
                                    aCursorInfo,
                                    aSqlInfo,
                                    STL_TRUE );
                    
                    sFlange->mRefCount = 1;   /* defence aging */
                    sFlange->mLastFixTime = knlGetSystemTime();
                    
                    sFreeHand->mFlangeHand = (i + 1);
                    *aFlange = sFlange;
            
                    sState = 0;
                    STL_TRY( knlReleaseLatch( &aClock->mLatch,
                                              KNL_ENV( aEnv ) )
                             == STL_SUCCESS );
                    
                    STL_THROW( RAMP_FINISH );
                }
            }

            /**
             * 해당 Block에는 더이상 Empty가 없다는 것을 설정한다.
             */
            
            sFreeHand->mFlangeHand = i;

            /**
             * next block
             */
            
            sFlangeBlock = STL_RING_GET_NEXT_DATA( &aClock->mBlockList, sFlangeBlock );

            if( STL_RING_IS_HEADLINK( &(aClock->mBlockList),
                                      sFlangeBlock ) == STL_TRUE )
            {
                sFlangeBlock = STL_RING_GET_FIRST_DATA( &aClock->mBlockList );
            }

            /**
             * aging flanges until the aging block
             */
            
            if( sFlangeBlock == sAgingHand->mBlockHand )
            {
                /**
                 * Extending clock
                 */
                break;
            }
            
            sFreeHand->mFlangeHand = 0;
            sFreeHand->mBlockHand = sFlangeBlock;
        }

        sState = 0;
        STL_TRY( knlReleaseLatch( &aClock->mLatch,
                                  KNL_ENV( aEnv ) ) == STL_SUCCESS );

        /**
         * Extending clock
         */

        STL_TRY( knpcExtendClock( aClock,
                                  &sIsSuccess,
                                  aEnv ) == STL_SUCCESS );

        /**
         * Aging clock
         */

        if( sIsSuccess == STL_FALSE )
        {
            STL_TRY( knpcAgingClock( aClock,
                                     KNPC_DEAD_AGE,
                                     KNPC_CLOCK_AGING_THRESHOLD,
                                     knlGetPropertyBigIntValueByID( KNL_PROPERTY_AGING_PLAN_INTERVAL,
                                                                    aEnv ),
                                     &sIsSuccess,
                                     aEnv ) == STL_SUCCESS );

            if( sIsSuccess == STL_FALSE )
            {
                /**
                 * failed to allocate flange
                 */

                if( sBuffer != NULL )
                {
                    STL_TRY( knlFreeDynamicMem( &aClock->mDynamicMem,
                                                sBuffer,
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );
                }

                break;
            }
        }

        sRetryCount++;
    }

    if( sBuffer != NULL )
    {
        STL_TRY( knlFreeDynamicMem( &aClock->mDynamicMem,
                                    sBuffer,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    
    STL_RAMP( RAMP_FINISH );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlReleaseLatch( &aClock->mLatch, KNL_ENV( aEnv ) );
    }

    if( sBuffer != NULL )
    {
        (void) knlFreeDynamicMem( &aClock->mDynamicMem,
                                  sBuffer,
                                  KNL_ENV(aEnv) );
    }
    
    return STL_FAILURE;
}


/**
 * @brief Flange를 Empty로 만든다.
 * @param[in]     aClock     Clock Pointer
 * @param[in]     aFlange    Flange Pointer
 * @param[in,out] aEnv       environment 정보
 */
stlStatus knpcFreeFlange( knpcClock  * aClock,
                          knpcFlange * aFlange,
                          knlEnv     * aEnv )
{
    stlInt32 i;

    for( i = 0; i < KNPC_MAX_PLAN_COUNT; i++ )
    {
        if( aFlange->mPlans[i].mIsUsed == STL_TRUE )
        {
            STL_TRY( knpcFreePlan( aFlange,
                                   &aFlange->mPlans[i],
                                   aEnv )
                     == STL_SUCCESS );
        }
    }

    if( aFlange->mBuffer != NULL )
    {
        STL_TRY( knlFreeDynamicMem( &aClock->mDynamicMem,
                                    aFlange->mBuffer,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        aFlange->mBuffer = NULL;
    }

    knpcResetFlange( aFlange );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


