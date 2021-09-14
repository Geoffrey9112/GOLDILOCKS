/*******************************************************************************
 * kntHash.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: kntHash.c 9033 2013-07-17 08:40:27Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file kntHash.c
 * @brief XA Hash Context Routines
 */

#include <knl.h>
#include <knDef.h>

extern knsEntryPoint  * gKnEntryPoint;
extern kntHashBucket  * gKnXaHashTable;

stlStatus kntInitHashTable( knlEnv * aEnv )
{
    stlInt64 i;
    
    for( i = 0; i < gKnEntryPoint->mXaHashSize; i++ )
    {
        STL_RING_INIT_HEADLINK( &(gKnXaHashTable[i].mContextList),
                                STL_OFFSETOF( knlXaContext, mContextLink ) );
        
        STL_TRY( knlInitLatch( &(gKnXaHashTable[i].mLatch),
                               STL_TRUE,
                               aEnv )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

kntHashBucket * kntGetHashBucket( stlInt32 aHashValue )
{
    return &gKnXaHashTable[((stlUInt32)aHashValue) % gKnEntryPoint->mXaHashSize];
}


knlXaContext * kntFindHashItem( kntHashBucket * aHashBucket,
                                stlInt32        aHashValue,
                                stlXid        * aXid )
{
    knlXaContext * sContextIter = NULL;
    knlXaContext * sContext = NULL;
    
    STL_RING_FOREACH_ENTRY( &aHashBucket->mContextList, sContextIter )
    {
        if( aHashValue == sContextIter->mHashValue )
        {
            if( dtlIsEqualXid( &sContextIter->mXid, aXid ) == STL_TRUE )
            {
                sContext = sContextIter;
                break;
            }
        }
    }

    return sContext;
}

