/*******************************************************************************
 * knlXaHash.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: knlXaHash.c 9033 2013-07-17 08:40:27Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file knlXaHash.c
 * @brief XA Context Hash Routines
 */

#include <knl.h>
#include <knDef.h>
#include <knt.h>

extern knsEntryPoint  * gKnEntryPoint;
extern kntHashBucket  * gKnXaHashTable;

/**
 * @brief 주어진 XID와 관련된 Context를 찾는다.
 * @param[in]     aXid        XID 포인터
 * @param[in]     aHashValue  XID Hash Value
 * @param[out]    aContext    찾은 Context
 * @param[out]    aFound      찾았는지 여부
 * @param[in,out] aEnv        environment 정보
 */
stlStatus knlFindXaContext( stlXid        * aXid,
                            stlInt32        aHashValue,
                            knlXaContext ** aContext,
                            stlBool       * aFound,
                            knlEnv        * aEnv )
{
    kntHashBucket  * sHashBucket;
    stlBool          sIsTimedOut;
    stlInt32         sState = 0;
    knlXaContext   * sContext;
    
    *aFound = STL_TRUE;

    sHashBucket = kntGetHashBucket( aHashValue );

    STL_TRY( knlAcquireLatch( &sHashBucket->mLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              aEnv )
             == STL_SUCCESS );
    sState = 1;
    
    sContext = kntFindHashItem( sHashBucket, aHashValue, aXid );
    
    sState = 0;
    STL_TRY( knlReleaseLatch( &sHashBucket->mLatch,
                              aEnv ) == STL_SUCCESS );

    if( sContext == NULL )
    {
        *aFound = STL_FALSE;
    }

    *aContext = sContext;

    return STL_SUCCESS;
    
    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlReleaseLatch( &sHashBucket->mLatch, aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief 주어진 Context를 Hash에 연결한다.
 * @param[in]     aContext     Context Pointer
 * @param[out]    aIsSuccess   성공 여부
 * @param[in,out] aEnv         environment 정보
 */
stlStatus knlLinkXaContext( knlXaContext * aContext,
                            stlBool      * aIsSuccess,
                            knlEnv       * aEnv )
{
    kntHashBucket  * sHashBucket;
    stlBool          sIsTimedOut;
    stlInt32         sState = 0;
    
    *aIsSuccess = STL_FALSE;

    sHashBucket = kntGetHashBucket( aContext->mHashValue );

    STL_TRY( knlAcquireLatch( &sHashBucket->mLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              aEnv )
             == STL_SUCCESS );
    sState = 1;
    
    if( kntFindHashItem( sHashBucket, aContext->mHashValue, &(aContext->mXid) ) == NULL )
    {
        STL_RING_ADD_LAST( &(sHashBucket->mContextList), aContext );
        *aIsSuccess = STL_TRUE;
    }
    
    sState = 0;
    STL_TRY( knlReleaseLatch( &sHashBucket->mLatch,
                              aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlReleaseLatch( &sHashBucket->mLatch, aEnv );
    }

    return STL_FAILURE;
}


/**
 * @brief 주어진 Context를 Hash에서 제거한다.
 * @param[in]     aContext  Context Pointer
 * @param[in,out] aEnv      environment 정보
 */
stlStatus knlUnlinkXaContext( knlXaContext * aContext,
                              knlEnv       * aEnv )
{
    kntHashBucket  * sHashBucket;
    stlBool          sIsTimedOut;
    stlInt32         sState = 0;
    
    sHashBucket = kntGetHashBucket( aContext->mHashValue );

    STL_TRY( knlAcquireLatch( &sHashBucket->mLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              aEnv )
             == STL_SUCCESS );
    sState = 1;
    
    STL_RING_UNLINK( &(sHashBucket->mContextList), aContext );
    
    sState = 0;
    STL_TRY( knlReleaseLatch( &sHashBucket->mLatch,
                              aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlReleaseLatch( &sHashBucket->mLatch, aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief 주어진 XID의 Hash Value를 구한다.
 * @param[in]     aXid  XID Pointer
 */
stlInt32 knlGetXidHashValue( stlXid * aXid )
{
    return KNT_XID_HASH_VALUE( aXid );
}

/**
 * @brief XID가 같은지 검사한다.
 * @param[in]  aXid        XID Pointer
 * @param[in]  aHashValue  XID Hash Value
 * @param[in]  aContext    XA Context
 */
stlBool knlIsEqualXid( stlXid       * aXid,
                       stlInt32       aHashValue,
                       knlXaContext * aContext )
{
    stlBool sResult = STL_FALSE;
    
    if( aHashValue == aContext->mHashValue )
    {
        if( dtlIsEqualXid( &aContext->mXid, aXid ) == STL_TRUE )
        {
            sResult = STL_TRUE;
        }
    }

    return sResult;
}
