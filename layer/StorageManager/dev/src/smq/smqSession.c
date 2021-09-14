/*******************************************************************************
 * smqSequence.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smqSequence.c 3429 2012-02-01 12:39:04Z xcom73 $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stl.h>
#include <dtl.h>
#include <knl.h>
#include <scl.h>
#include <smlDef.h>
#include <smDef.h>
#include <smg.h>
#include <smp.h>
#include <smqDef.h>

/**
 * @file smqSession.c
 * @brief Storage Manager Layer Sequence Session Internal Routines
 */

/**
 * @addtogroup smqSession
 * @{
 */

static stlUInt32 smqSeqHashFunc( void * aKey, stlUInt32 aBucketCount )
{
    return (*(stlInt64*)aKey) % aBucketCount;
}

static stlInt32 smqSeqCompareFunc( void * aKeyA, void * aKeyB )
{
    smqSessSeqKey * sSeqKeyA;
    smqSessSeqKey * sSeqKeyB;
    stlInt32        sRet;

    sSeqKeyA = (smqSessSeqKey*)aKeyA;
    sSeqKeyB = (smqSessSeqKey*)aKeyB;

    if( sSeqKeyA->mId > sSeqKeyB->mId )
    {
        sRet = 1;
    }
    else
    {
        if( sSeqKeyA->mId == sSeqKeyB->mId )
        {
            sRet = 0;
        }
        else
        {
            sRet = -1;
        }
    }

    if( sRet == 0 )
    {
        if( sSeqKeyA->mValidScn > sSeqKeyB->mValidScn )
        {
            sRet = 1;
        }
        else
        {
            if( sSeqKeyA->mValidScn == sSeqKeyB->mValidScn )
            {
                sRet = 0;
            }
            else
            {
                sRet = -1;
            }
        }
    }

    return sRet;
}

stlStatus smqSetSessSeqValue( smqCache * aCache,
                              stlInt64   aSeqValue,
                              smlEnv   * aEnv )
{
    smlSessionEnv  * sSessEnv;
    smqSessSeqItem * sSeqItem;
    smqSessSeqKey    sSeqKey;
    
    sSessEnv = SML_SESS_ENV( aEnv );

    sSeqKey.mId = aCache->mId;
    sSeqKey.mValidScn = aCache->mValidScn;
    
    STL_TRY( knlFindStaticHash( sSessEnv->mSeqHash,
                                smqSeqHashFunc,
                                smqSeqCompareFunc,
                                (void*)&sSeqKey,
                                (void**)&sSeqItem,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    if( sSeqItem == NULL )
    {
        STL_TRY( knlAllocRegionMem( &sSessEnv->mSessionMem,
                                    STL_SIZEOF( smqSessSeqItem ),
                                    (void**)&sSeqItem,
                                    KNL_ENV(aEnv) ) == STL_SUCCESS );

        sSeqItem->mSeqKey = sSeqKey;
        sSeqItem->mSeqValue = aSeqValue;

        STL_TRY( knlInsertStaticHash( sSessEnv->mSeqHash,
                                      smqSeqHashFunc,
                                      (void*)sSeqItem,
                                      KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        sSeqItem->mSeqValue = aSeqValue;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smqRemoveSessSeqValue( smqCache * aCache,
                                 smlEnv   * aEnv )
{
    smlSessionEnv   * sSessEnv;
    smqSessSeqItem  * sSeqItem = NULL;
    smqSessSeqKey     sSeqKey;

    sSessEnv = SML_SESS_ENV( aEnv );
    
    sSeqKey.mId = aCache->mId;
    sSeqKey.mValidScn = aCache->mValidScn;
    
    STL_TRY( knlFindStaticHash( sSessEnv->mSeqHash,
                                smqSeqHashFunc,
                                smqSeqCompareFunc,
                                (void*)&sSeqKey,
                                (void**)&sSeqItem,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    if( sSeqItem != NULL )
    {
        STL_TRY( knlRemoveStaticHash( sSessEnv->mSeqHash,
                                      smqSeqHashFunc,
                                      sSeqItem,
                                      KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
                                     
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smqFindSessSeqValue( smqCache * aCache,
                               stlBool  * aFound,
                               stlInt64 * aSeqValue,
                               smlEnv   * aEnv )
{
    smlSessionEnv   * sSessEnv;
    smqSessSeqItem  * sSeqItem = NULL;
    smqSessSeqKey     sSeqKey;

    sSessEnv = SML_SESS_ENV( aEnv );

    sSeqKey.mId = aCache->mId;
    sSeqKey.mValidScn = aCache->mValidScn;
    
    STL_TRY( knlFindStaticHash( sSessEnv->mSeqHash,
                                smqSeqHashFunc,
                                smqSeqCompareFunc,
                                (void*)&sSeqKey,
                                (void**)&sSeqItem,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    if( sSeqItem == NULL )
    {
        *aFound = STL_FALSE;
    }
    else
    {
        *aFound = STL_TRUE;
        *aSeqValue = sSeqItem->mSeqValue;
    }
                                     
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} */
