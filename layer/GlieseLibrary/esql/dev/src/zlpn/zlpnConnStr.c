/*******************************************************************************
 * zlpnConnStr.c
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

#include <stl.h>
#include <goldilocks.h>
#include <zlpeSqlca.h>
#include <zlpnConnect.h>
#include <zlpnConnStr.h>
#include <zlpuMisc.h>
#include <zlpySymTab.h>
#include <zllDbcNameHash.h>

/**
 * @file zlpnConnStr.c
 * @brief Gliese Embedded SQL in C precompiler connect library functions
 */

/**
 * @addtogroup zlpnConnect
 * @{
 */

stlStaticHash   *gConnObjSymTab = NULL;
stlSpinLock      gConnStrLatch;

/************************************************************
 * Connection Object
 ************************************************************/
stlStatus zlpnCreateSqlContextHash( stlErrorStack *aErrorStack )
{
    stlInitSpinLock( &gConnStrLatch );
    STL_TRY( stlCreateStaticHash( &gConnObjSymTab,
                                  ZLPL_CONN_STRING_HASH_BUCKET_COUNT,
                                  STL_OFFSETOF(zlplSqlContext, mHashLink),
                                  STL_OFFSETOF(zlplSqlContext, mConnName),
                                  STL_STATICHASH_MODE_BUCKETLOCK,
                                  aErrorStack )
            == STL_SUCCESS);

    return STL_SUCCESS;

    STL_FINISH;

    stlPushError( STL_ERROR_LEVEL_ABORT,
                  ZLPL_ERRCODE_CREATE_SYMTAB,
                  NULL,
                  aErrorStack );

    return STL_FAILURE;
}

stlStatus zlpnDestroySqlContextHash( stlErrorStack *aErrorStack )
{
    stlUInt32           sBucketSeq;
    zlplSqlContext     *sSqlContext;

    STL_TRY( stlGetFirstStaticHashItem( gConnObjSymTab,
                                        & sBucketSeq,
                                        (void **) & sSqlContext,
                                        aErrorStack )
            == STL_SUCCESS );

    while( sSqlContext != NULL )
    {
        STL_TRY( zlpyDestroySymbolTable( sSqlContext )
                 == STL_SUCCESS );

        STL_TRY( zlpnUnlinkContextList( sSqlContext )
                 == STL_SUCCESS );

        STL_TRY( zlpnDeleteSqlContextFromHash( sSqlContext,
                                               aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( zlpuFreeContextMem( &sSqlContext )
                 == STL_SUCCESS );

        STL_TRY( stlGetNextStaticHashItem( gConnObjSymTab,
                                           & sBucketSeq,
                                           (void **) & sSqlContext,
                                           aErrorStack )
                == STL_SUCCESS );
    }

    (void)stlDestroyStaticHash( &gConnObjSymTab,
                                aErrorStack );
    gConnObjSymTab = NULL;
    stlFinSpinLock( &gConnStrLatch );

    return STL_SUCCESS;

    STL_FINISH;

    stlPushError( STL_ERROR_LEVEL_ABORT,
                  ZLPL_ERRCODE_DESTROY_SYMTAB,
                  NULL,
                  aErrorStack );

    return STL_FAILURE;
}


stlInt32 zlpnCompareConnStrFunc(void *aKeyA, void *aKeyB)
{
    stlChar   *sKeyA    = (stlChar *)aKeyA;
    stlChar   *sKeyB    = (stlChar *)aKeyB;

    return stlStrcmp(sKeyA, sKeyB);
}

stlUInt32 zlpnHashConnStrFunc(void * aKey, stlUInt32 aBucketCount)
{
    stlUInt32   sHashValue;

    sHashValue = stlGetHashValueString( (stlChar*) aKey );

    return sHashValue % aBucketCount;
}

stlStatus zlpnAddSqlContextToHash( zlplSqlContext  *aSqlContext,
                                   stlErrorStack   *aErrorStack )
{
    STL_TRY( stlInsertStaticHash( gConnObjSymTab,
                                  zlpnHashConnStrFunc,
                                  aSqlContext,
                                  aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlpnAddUniqueSqlContextToHash( zlplSqlContext   *aSqlContext,
                                         stlBool          *aIsDuplicate,
                                         stlErrorStack    *aErrorStack )
{
    STL_TRY( stlInsertUniqueStaticHash( gConnObjSymTab,
                                        zlpnHashConnStrFunc,
                                        zlpnCompareConnStrFunc,
                                        aSqlContext,
                                        aIsDuplicate,
                                        aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlpnLookupSqlContext( stlChar          *aConnStr,
                                zlplSqlContext  **aSqlContext,
                                stlErrorStack    *aErrorStack )
{
    STL_TRY( stlFindStaticHash( gConnObjSymTab,
                                zlpnHashConnStrFunc,
                                zlpnCompareConnStrFunc,
                                aConnStr,
                                (void **)aSqlContext,
                                aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

zlplSqlContext *zlpnLookupSqlContextDebug( stlChar *aConnStr )
{
    stlErrorStack    sErrorStack;
    zlplSqlContext  *sSqlContext = NULL;

    STL_INIT_ERROR_STACK( &sErrorStack );

    STL_TRY( stlFindStaticHash( gConnObjSymTab,
                                zlpnHashConnStrFunc,
                                zlpnCompareConnStrFunc,
                                aConnStr,
                                (void **)&sSqlContext,
                                &sErrorStack )
             == STL_SUCCESS );

    if( sSqlContext != NULL )
    {
        stlPrintf( "Conn Name : %s, Found !!\n" );
    }
    else
    {
        stlPrintf( "Conn Name : %s, NOT Found !!\n" );
    }

    return sSqlContext;

    STL_FINISH;

    return NULL;
}

stlStatus zlpnDeleteSqlContextFromHash( zlplSqlContext   *aSqlContext,
                                        stlErrorStack    *aErrorStack )
{
    STL_TRY( stlRemoveStaticHash( gConnObjSymTab,
                                  zlpnHashConnStrFunc,
                                  aSqlContext,
                                  aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlpnGetRealContext( zlplSqlContext   *aGivenSqlContext,
                              zlplSqlArgs      *aSqlArgs,
                              zlplSqlContext  **aRealSqlContext )
{
    void                *sDbc = NULL;
    zlplSqlContext      *sSqlContext = NULL;
    stlBool              sIsDuplicated = STL_FALSE;

    stlErrorStack        sErrorStack;

    STL_INIT_ERROR_STACK(&sErrorStack);
    STL_TRY_THROW( stlSetThreadOnce( &gThreadOnce,
                                     zlpuInitialize,
                                     &sErrorStack )
                   == STL_SUCCESS, RAMP_ERR_SET_THREAD_ONCE );

    *aRealSqlContext = NULL;

    if( aSqlArgs->conn == NULL )
    {
        if( aGivenSqlContext != NULL )
        {
            *aRealSqlContext = aGivenSqlContext;
        }
        else
        {
            *aRealSqlContext = gDefaultContext;
        }
    }
    else
    {
        /*
         * Connection Name이 주어진 경우에는 주어진 이름으로
         * Context를 찾아서 반환한다.
         * 만약, Context를 찾지 못한 경우에는 해당 이름으로 DBC를 찾아서,
         * DBC가 존재한다면 Context를 할당하여 DBC를 연결하여 주고,
         * DBC가 존재하지 않는다면, Context가 없는 것으로 간주한다.
         */
        STL_TRY( zlpnLookupSqlContext( aSqlArgs->conn,
                                       &sSqlContext,
                                       &sErrorStack )
                 == STL_SUCCESS );

        if( sSqlContext == NULL )
        {
            (void)zllFindDbcFromNameHash( &sDbc,
                                          aSqlArgs->conn,
                                          &sErrorStack );

            if( sDbc != NULL )
            {
                STL_TRY( zlpuAllocContextMem( &sSqlContext )
                         == STL_SUCCESS );

                sSqlContext->mConnInfo.mDbc = sDbc;
                sSqlContext->mIsConnected = STL_TRUE;

                stlStrncpy( sSqlContext->mConnName,
                            aSqlArgs->conn,
                            ZLPL_MAX_CONN_STRING_LENGTH );

                STL_TRY( zlpnAddUniqueSqlContextToHash( sSqlContext,
                                                        &sIsDuplicated,
                                                        ZLPL_CTXT_ERROR_STACK(sSqlContext) )
                         == STL_SUCCESS );

                /*
                 * Duplicate가 되었기 때문에, 여기서 다시 한번 Lookup을 수행하면,
                 * 반드시 Symbol이 존재해야 한다.
                 */
                if( sIsDuplicated == STL_TRUE )
                {
                    STL_TRY( zlpuFreeContextMem( &sSqlContext )
                             == STL_SUCCESS );

                    STL_TRY( zlpnLookupSqlContext( aSqlArgs->conn,
                                                   &sSqlContext,
                                                   &sErrorStack )
                             == STL_SUCCESS );
                }
                else /* else of if( sIsDuplicated == STL_TRUE ) */
                {
                    ZLPL_INIT_SQLCA(sSqlContext, aSqlArgs->sql_ca);
                    STL_TRY( zlpyCreateSymbolTable( sSqlContext )
                             == STL_SUCCESS );

                    STL_TRY( zlpnPutContextList( sSqlContext )
                             == STL_SUCCESS );
                } /* end of if( sIsDuplicated == STL_TRUE ) */

                *aRealSqlContext = sSqlContext;
            } /* end of if( sDbc != NULL ) */
        }
        else /* else of if( sSqlContext == NULL ) */
        {
            *aRealSqlContext = sSqlContext;
        } /* end of if( sSqlContext == NULL ) */
    }

    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_SET_THREAD_ONCE)
    {
        ((zlplSqlca *)aSqlArgs->sql_ca)->sqlcode = -1;
        zlpeSetErrorMsg((zlplSqlca *)aSqlArgs->sql_ca, "Fail to initialize.");
        zlpuPrintErrorStack(&sErrorStack);
        stlExit(-1);
    }
    STL_FINISH;

    INIT_SQLCA(aSqlArgs->sql_ca);

    return STL_FAILURE;
}

stlStatus zlpnPutContextList( zlplSqlContext *aSqlContext )
{
    stlUInt64            sMissCnt;

    stlAcquireSpinLock( &gCtxtRingSpinLock, &sMissCnt );

    if( aSqlContext->mInList == STL_TRUE )
    {
        STL_RING_UNLINK( &gCtxtRing, aSqlContext );
    }
    STL_RING_ADD_LAST( &gCtxtRing, aSqlContext );
    aSqlContext->mInList = STL_TRUE;

    stlReleaseSpinLock( &gCtxtRingSpinLock );

    return STL_SUCCESS;
}

stlStatus zlpnGetContextList( zlplSqlContext **aSqlContext )
{
    stlUInt64            sMissCnt;

    stlAcquireSpinLock( &gCtxtRingSpinLock, &sMissCnt );

    if( STL_RING_IS_EMPTY( &gCtxtRing ) == STL_TRUE )
    {
        *aSqlContext = NULL;
    }
    else
    {
        *aSqlContext = (zlplSqlContext *)STL_RING_GET_FIRST_DATA( &gCtxtRing );
        //STL_RING_UNLINK( &gCtxtRing, *aSqlContext );
    }

    stlReleaseSpinLock( &gCtxtRingSpinLock );

    return STL_SUCCESS;
}

stlStatus zlpnUnlinkContextList( zlplSqlContext *aSqlContext )
{
    stlUInt64            sMissCnt;

    if( aSqlContext->mInList == STL_TRUE )
    {
        stlAcquireSpinLock( &gCtxtRingSpinLock, &sMissCnt );
        STL_RING_UNLINK( &gCtxtRing, aSqlContext );
        stlReleaseSpinLock( &gCtxtRingSpinLock );
        aSqlContext->mInList = STL_FALSE;
    }

    return STL_SUCCESS;
}

/** @} */
