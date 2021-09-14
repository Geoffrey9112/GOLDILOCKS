/*******************************************************************************
 * knpcHash.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: knpcHash.c 9033 2013-07-17 08:40:27Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file knpcHash.c
 * @brief Plan Hash
 */

#include <knl.h>
#include <knDef.h>
#include <knpc.h>

extern knsEntryPoint * gKnEntryPoint;

/**
 * @brief Hash Bucket에서 주어진 SQL을 갖는 Flange를 반환한다.
 * @param[in]     aHashBucket    Hash Bucket
 * @param[in]     aHashValue     Hash Value
 * @param[in]     aSqlString     SQL String
 * @param[in]     aSqlLength     SQL Length
 * @param[in]     aUserInfo      User Information
 * @param[in]     aCursorInfo    Cursor Information
 */
knpcFlange * knpcFindHashItem( knpcHashBucket    * aHashBucket,
                               stlInt32            aHashValue,
                               stlChar           * aSqlString,
                               stlInt64            aSqlLength,
                               knlPlanUserInfo   * aUserInfo,
                               knlPlanCursorInfo * aCursorInfo )
{
    knpcFlange * sFlangeIter = NULL;
    knpcFlange * sFlange = NULL;
    
    STL_RING_FOREACH_ENTRY( &aHashBucket->mOverflowList, sFlangeIter )
    {
        if( aHashValue == sFlangeIter->mHashValue )
        {
            if( sFlangeIter->mDropped == STL_FALSE )
            {
                if( sFlangeIter->mSqlLength == aSqlLength )
                {
                    /**
                     * compare user information
                     */
                
                    if( aUserInfo->mUserId == sFlangeIter->mUserInfo.mUserId )
                    {
                        /**
                         * query 인 경우 cursor information 을 비교
                         */
                        if ( sFlangeIter->mSqlInfo.mIsQuery == STL_TRUE )
                        {
                            if ( (sFlangeIter->mCursorInfo.mIsDbcCursor   == aCursorInfo->mIsDbcCursor)   &&
                                 (sFlangeIter->mCursorInfo.mStandardType  == aCursorInfo->mStandardType)  &&
                                 (sFlangeIter->mCursorInfo.mSensitivity   == aCursorInfo->mSensitivity)   &&
                                 (sFlangeIter->mCursorInfo.mScrollability == aCursorInfo->mScrollability) &&
                                 (sFlangeIter->mCursorInfo.mHoldability   == aCursorInfo->mHoldability)   &&
                                 (sFlangeIter->mCursorInfo.mUpdatability  == aCursorInfo->mUpdatability)  &&
                                 (sFlangeIter->mCursorInfo.mReturnability == aCursorInfo->mReturnability) )
                            {
                                /* cursor 속성이 동일함 */
                            }
                            else
                            {
                                /* cursor 속성이 다름 */
                                continue;
                            }
                        }

                        /**
                         * SQL string 비교
                         */
                        if( stlMemcmp( sFlangeIter->mSqlString,
                                       aSqlString,
                                       aSqlLength ) == 0 )
                        {
                            sFlange = sFlangeIter;
                            break;
                        }
                    }
                }
            }
        }
    }

    return sFlange;
}


/**
 * @brief 주어진 Hash Value에 대한 Hash Bucket을 구한다.
 * @param[in]     aHashValue    Hash Value
 */
knpcHashBucket * knpcGetHashBucket( stlInt32 aHashValue )
{
    knpcHashBucket * sPlanHash;

    sPlanHash = (knpcHashBucket*)gKnEntryPoint->mPlanHash;
    
    return &(sPlanHash[((stlUInt32)aHashValue) % gKnEntryPoint->mPlanHashSize]);
}


/**
 * @brief Plan Cache 관리자를 초기화한다.
 * @param[in]     aSharedEntry    Shared Entry Pointer
 * @param[in,out] aEnv            environment 정보
 */
stlStatus knpcInitPlanCache( knsEntryPoint  * aSharedEntry,
                             knlEnv         * aEnv )

{
    stlInt64         sHashSize;
    knpcClock      * sClock;
    knpcHashBucket * sPlanHash;
    stlInt32         i;
    stlInt64         sPlanCacheSize;

    /**
     * @todo Property
     */
    
    sHashSize = KNPC_PLAN_HASH_SIZE;

    aSharedEntry->mPlanHashSize = sHashSize;

    /**
     * initialize hash bucket array
     */
    
    STL_TRY( knlAllocFixedStaticArea( STL_SIZEOF( knpcHashBucket ) * sHashSize,
                                      (void**)&sPlanHash,
                                      KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    aSharedEntry->mPlanHash = (void*)sPlanHash;

    for( i = 0; i < sHashSize; i++ )
    {
        STL_TRY( knlInitLatch( &sPlanHash[i].mLatch,
                               STL_TRUE,   /* aIsInShm */
                               KNL_ENV(aEnv) ) == STL_SUCCESS );

        STL_RING_INIT_HEADLINK( &(sPlanHash[i].mOverflowList),
                                STL_OFFSETOF( knpcFlange, mOverflowLink ) );
        
        sPlanHash[i].mBucketId = i;
    }

    /**
     * initialize clocks
     */

    aSharedEntry->mClockCount = knlGetPropertyBigIntValueByID( KNL_PROPERTY_PLAN_CLOCK_COUNT,
                                                               aEnv );
    
    STL_TRY( knlAllocFixedStaticArea( STL_SIZEOF( knpcClock ) * aSharedEntry->mClockCount,
                                      (void**)&sClock,
                                      KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    aSharedEntry->mClock = (void*)sClock;

    sPlanCacheSize = knlGetPropertyBigIntValueByID( KNL_PROPERTY_PLAN_CACHE_SIZE,
                                                    aEnv );
    
    STL_TRY( knlInitMemController( &(aSharedEntry->mMemController),
                                   sPlanCacheSize,
                                   KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    for( i = 0; i < aSharedEntry->mClockCount; i++ )
    {
        STL_TRY( knpcInitClock( &sClock[i],
                                i,
                                &(aSharedEntry->mMemController),
                                aEnv ) == STL_SUCCESS );
    }
    
    aSharedEntry->mClockAgingCount = 0;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

