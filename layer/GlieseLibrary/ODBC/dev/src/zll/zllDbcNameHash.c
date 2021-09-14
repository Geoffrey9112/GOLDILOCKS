/*******************************************************************************
 * zllDbcNameHash.c
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

#include <cml.h>
#include <goldilocks.h>

#if defined( ODBC_ALL ) || defined( ODBC_DA )
#include <ssl.h>
#endif

#include <zlDef.h>
#include <zle.h>
#include <zld.h>

#include <zlc.h>
#include <zls.h>

/**
 * @file zllDbcNameHash.c
 * @brief ODBC API Internal Connect Routines.
 */

stlStaticHash  *gDbcNameHash = NULL;
stlUInt32       gDbcNameCas = 0;

/**
 * @addtogroup zllDbc
 * @{
 */

static stlInt32 zllCompareDbcNameFunc(void *aKeyA, void *aKeyB)
{
    stlChar   *sKeyA    = (stlChar *)aKeyA;
    stlChar  **sKeyBPtr = (stlChar **)aKeyB;

    return stlStrcmp(sKeyA, *sKeyBPtr);
}

static stlUInt32 zllHashDbcNameByValueFunc(void * aKey, stlUInt32 aBucketCount)
{
    stlUInt32   sHashValue;

    sHashValue = stlGetHashValueString( (stlChar*) aKey );

    return sHashValue % aBucketCount;
}

static stlUInt32 zllHashDbcNameByPtrFunc(void * aKey, stlUInt32 aBucketCount)
{
    return zllHashDbcNameByValueFunc( *((stlChar**)aKey), aBucketCount );
}

stlStatus zllInitializeDbcHash( stlErrorStack *aErrorStack )
{
    stlUInt32  sOldValue;

    while( 1 )
    {
        sOldValue = stlAtomicCas32( &gDbcNameCas, 1, 0 );

        if( sOldValue == 0 )
        {
            if( gDbcNameHash == NULL )
            {
                STL_TRY( stlCreateStaticHash( &gDbcNameHash,
                                              STL_DBCHASH_BUCKET_COUNT,
                                              STL_OFFSETOF(zlcDbc, mDbcNameLink),
                                              STL_OFFSETOF(zlcDbc, mDbcName),
                                              STL_STATICHASH_MODE_BUCKETLOCK,
                                              aErrorStack )
                         == STL_SUCCESS );
            }

            break;
        }

        stlYieldThread();
    }

    gDbcNameCas = 0;

    return STL_SUCCESS;

    STL_FINISH;

    gDbcNameCas = 0;

    return STL_FAILURE;
}

stlStatus zllFinalizeDbcHash( stlErrorStack *aErrorStack )
{
    stlUInt32  sOldValue;

    while( 1 )
    {
        sOldValue = stlAtomicCas32( &gDbcNameCas, 1, 0 );

        if( sOldValue == 0 )
        {
            if( gDbcNameHash != NULL )
            {
                STL_TRY( stlDestroyStaticHash( &gDbcNameHash,
                                               aErrorStack )
                         == STL_SUCCESS );
                gDbcNameHash = NULL;
            }

            break;
        }

        stlYieldThread();
    }

    gDbcNameCas = 0;

    return STL_SUCCESS;

    STL_FINISH;

    gDbcNameCas = 0;

    return STL_FAILURE;
}

stlStatus SQL_API zllAddDbcToNameHash( void          *aDbc,
                                       stlErrorStack *aErrorStack )
{
    STL_TRY( stlInsertStaticHash( gDbcNameHash,
                                  zllHashDbcNameByPtrFunc,
                                  aDbc,
                                  aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus SQL_API zllRemoveDbcFromNameHash( void          *aDbc,
                                            stlErrorStack *aErrorStack )
{
    STL_TRY( stlRemoveStaticHash( gDbcNameHash,
                                  zllHashDbcNameByPtrFunc,
                                  aDbc,
                                  aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus SQL_API zllFindDbcFromNameHash( void          **aDbc,
                                          stlChar        *aConnName,
                                          stlErrorStack  *aErrorStack )
{
    zlcDbc   *sDbc = NULL;

    STL_TRY( stlFindStaticHash( gDbcNameHash,
                                zllHashDbcNameByValueFunc,
                                zllCompareDbcNameFunc,
                                aConnName,
                                (void **)&sDbc,
                                aErrorStack )
             == STL_SUCCESS );

    *aDbc = sDbc;

    return STL_SUCCESS;

    STL_FINISH;

    *aDbc = NULL;

    return STL_FAILURE;
}

stlStatus SQL_API zllSetDbcName( void          *aDbc,
                                 stlChar       *aDbcName,
                                 stlErrorStack *aErrorStack )
{
    zlcDbc      *sDbc = (zlcDbc *)aDbc;
    stlInt16     sConnNameLength = 0;

    if( aDbcName == NULL )
    {
        sDbc->mDbcName = NULL;
    }
    else
    {
        sConnNameLength = stlStrlen( aDbcName );
        if( sConnNameLength > 0 )
        {
            STL_TRY( stlAllocHeap( (void **)&sDbc->mDbcName,
                                   sConnNameLength + 1,
                                   aErrorStack )
                     == STL_SUCCESS );

            stlStrncpy( sDbc->mDbcName, aDbcName, sConnNameLength + 1 );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
