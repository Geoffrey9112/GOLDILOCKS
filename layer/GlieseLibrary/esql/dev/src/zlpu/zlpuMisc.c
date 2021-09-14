/*******************************************************************************
 * zlpuMisc.c
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
#include <zlpnConnStr.h>
#include <zlpuMisc.h>
#include <zlpySymTab.h>

/**
 * @file zlpuMisc.c
 * @brief Gliese Embedded SQL in C precompiler utility library functions
 */

/**
 * @addtogroup zlpuMisc
 * @{
 */

/**
 * @brief External
 */

extern stlErrorRecord gZlplErrorTable[];

/**
 * @brief Internal
 */

/* Global variables */

stlSemaphore          gAllocatorSem;
stlAllocator          gAllocator;
stlDynamicAllocator   gDynMemory;
stlErrorStack         gErrorStack;
stlUInt32             gIsInit = STL_FALSE;

stlSpinLock           gCtxtRingSpinLock;
stlRingHead           gCtxtRing;

void zlpuInitialize(void)
{
    SQLRETURN      sReturn;

    STL_INIT_ERROR_STACK(&gErrorStack);

    /* GOLDILOCKSESQL은 Standard Library를 사용하므로, 초기화를 해주어야 한다. */
    /* 만약, Datatype layer까지 사용하게 되면, dtlInitialize()로 변경한다. */
    STL_TRY(stlInitialize() == STL_SUCCESS);
    stlRegisterErrorTable(STL_ERROR_MODULE_GLIESE_LIBRARY_GOLDILOCKSESQL,
                          gZlplErrorTable);

    STL_TRY_THROW(stlCreateSemaphore(&gAllocatorSem,
                                     "MEMSEM",
                                     1,
                                     &gErrorStack)
                  == STL_SUCCESS, RAMP_ERR_CREATE_SEMAPHORE);

    STL_TRY_THROW(stlCreateRegionAllocator(&gAllocator,
                                           ZLPL_REGION_INIT_SIZE,
                                           ZLPL_REGION_NEXT_SIZE,
                                           &gErrorStack)
                  == STL_SUCCESS, RAMP_ERR_CREATE_ALLOCATOR);

    STL_TRY_THROW(stlCreateDynamicAllocator( & gDynMemory,
                                             ZLPL_DYNAMIC_INIT_SIZE,
                                             ZLPL_DYNAMIC_NEXT_SIZE,
                                             & gErrorStack)
                  == STL_SUCCESS, RAMP_ERR_CREATE_ALLOCATOR);

    STL_TRY(zlpuAllocContextMem(&gDefaultContext)
            == STL_SUCCESS);
    ZLPL_INIT_CONTEXT(gDefaultContext);

    STL_TRY( zlpnCreateSqlContextHash(&gErrorStack)
             == STL_SUCCESS );

    stlInitSpinLock( &gCtxtRingSpinLock );
    STL_RING_INIT_HEADLINK( &gCtxtRing, STL_OFFSETOF( zlplSqlContext, mListLink ) );

    sReturn = SQLAllocHandle(SQL_HANDLE_ENV,
                             NULL,
                             &gEnv);
    switch(sReturn)
    {
        case SQL_SUCCESS:
        case SQL_SUCCESS_WITH_INFO:
            break;
        case SQL_INVALID_HANDLE:
        case SQL_ERROR:
        default:
            /* Environment 할당 전이므로, 특별한 Error 처리 방법을 생각해야함 */
            STL_THROW(RAMP_ERR_ALLOC_ENV);
            break;
    }

    /* SQLSetEnvAttr is sets attributes that govern aspects of environments */
    sReturn = SQLSetEnvAttr(gEnv,
                            SQL_ATTR_ODBC_VERSION,
                            (SQLPOINTER)SQL_OV_ODBC3,
                            0);
    switch(sReturn)
    {
        case SQL_SUCCESS:
        case SQL_SUCCESS_WITH_INFO:
            break;
        case SQL_INVALID_HANDLE:
        case SQL_ERROR:
        default:
            STL_THROW(RAMP_ERR_SET_ENV_ATTR);
            break;
    }

    stlAtExit(zlpuFinalize);

    stlAtomicSet32(&gIsInit, STL_TRUE);

    return;

    STL_CATCH(RAMP_ERR_CREATE_SEMAPHORE)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_CREATE_SEMAPHORE,
                      NULL,
                      &gErrorStack );
    }
    STL_CATCH(RAMP_ERR_CREATE_ALLOCATOR)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_ALLOCATE_MEM,
                      NULL,
                      &gErrorStack );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_CREATE_ALLOCATOR,
                      NULL,
                      &gErrorStack );
    }
    STL_CATCH(RAMP_ERR_ALLOC_ENV)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_ALLOCATE_ENV,
                      NULL,
                      &gErrorStack );
    }
    STL_CATCH(RAMP_ERR_SET_ENV_ATTR)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_SET_ENV_ATTR,
                      NULL,
                      &gErrorStack );
    }
    STL_FINISH;

    stlPushError( STL_ERROR_LEVEL_ABORT,
                  ZLPL_ERRCODE_INITIALIZE,
                  NULL,
                  &gErrorStack );

    /* Error Message를 출력하고, Application 종료 : 현재 단계에서는,
     * Server에 어떤 action도 취하지 않았으므로, 별다르게 정리해 줄 내용은 없음
     */
    zlpuPrintErrorStack(&gErrorStack);

    stlExit(-1);
    return;
}

void zlpuFinalize(void)
{
    zlplSqlContext  *sSqlContext;

    STL_TRY( zlpnGetContextList( &sSqlContext )
             == STL_SUCCESS );

    while( sSqlContext != NULL  )
    {
        STL_TRY( zlpyDestroySymbolTable( sSqlContext )
                 == STL_SUCCESS );

        STL_TRY( zlpnUnlinkContextList( sSqlContext )
                 == STL_SUCCESS );

        STL_TRY( zlpnDeleteSqlContextFromHash( sSqlContext,
                                               &gErrorStack )
                 == STL_SUCCESS );

        STL_TRY( zlpuFreeContextMem( &sSqlContext )
                 == STL_SUCCESS );

        STL_TRY( zlpnGetContextList( &sSqlContext )
                 == STL_SUCCESS );
    }

    STL_TRY( zlpnDestroySqlContextHash(&gErrorStack)
             == STL_SUCCESS );

    STL_TRY(zlpuFreeContextMem(&gDefaultContext)
            == STL_SUCCESS);

    SQLFreeHandle(SQL_HANDLE_ENV, gEnv);
    stlFinSpinLock( &gCtxtRingSpinLock );

    STL_TRY_THROW(stlDestroySemaphore(&gAllocatorSem,
                                      &gErrorStack)
                  == STL_SUCCESS, RAMP_ERR_DESTROY_SEMAPHORE);

    STL_TRY_THROW(stlDestroyRegionAllocator(&gAllocator,
                                            &gErrorStack)
                  == STL_SUCCESS, RAMP_ERR_DESTROY_ALLOCATOR);

    STL_TRY_THROW( stlDestroyDynamicAllocator( & gDynMemory,
                                               & gErrorStack )
                   == STL_SUCCESS, RAMP_ERR_DESTROY_ALLOCATOR);

    /* zlplInitialize()에서 stlInitialize()를 했으므로, stlTerminate() */
    /* 만약, dtlInitialize()를 한다면, dtlTerminate()로 */
    stlTerminate();

    return;

    STL_CATCH(RAMP_ERR_DESTROY_SEMAPHORE)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_DESTROY_SEMAPHORE,
                      NULL,
                      &gErrorStack );
    }
    STL_CATCH(RAMP_ERR_DESTROY_ALLOCATOR)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_DESTROY_ALLOCATOR,
                      NULL,
                      &gErrorStack );
    }
    STL_FINISH;

    stlPushError( STL_ERROR_LEVEL_ABORT,
                  ZLPL_ERRCODE_FINALIZE,
                  NULL,
                  &gErrorStack );

    /* Error Message를 출력 : 정리해 줄 내용이 있는 지 검토해야 함
     */
    zlpuPrintErrorStack(&gErrorStack);

    return;
}

void zlpuPrintErrorStack(stlErrorStack *aErrorStack)
{
    stlInt32       i;
    stlErrorData * sErrorData;

    if(aErrorStack != NULL)
    {
        for(i = aErrorStack->mTop; i >= 0; i--)
        {
            sErrorData = &aErrorStack->mErrorData[i];
            stlPrintf( "------------------------------------------------------\n" );
            stlPrintf( "Error level           : %d\n", sErrorData->mErrorLevel );
            stlPrintf( "Internal error code   : %d\n", sErrorData->mErrorCode );
            stlPrintf( "System error code     : %d\n", sErrorData->mSystemErrorCode );
            stlPrintf( "Primary Error message : %s\n", sErrorData->mErrorMessage );
            stlPrintf( "Detail Error message  : %s\n", sErrorData->mDetailErrorMessage );
            stlPrintf( "------------------------------------------------------\n" );
            stlPrintf( "\n" );
        }
    }
}

stlStatus zlpuAllocContextMem(zlplSqlContext **aSqlContext)
{
    stlBool         sIsAcquireSem = STL_FALSE;
    zlplSqlContext *sSqlContext = NULL;

    STL_TRY( stlAcquireSemaphore( &gAllocatorSem,
                                  &gErrorStack )
             == STL_SUCCESS );
    sIsAcquireSem = STL_TRUE;

    STL_TRY_THROW( stlAllocRegionMem( &gAllocator,
                                      STL_SIZEOF(zlplSqlContext),
                                      (void **)&sSqlContext,
                                      &gErrorStack )
                   == STL_SUCCESS, RAMP_ERR_ALLOCATE_MEM );

    sIsAcquireSem = STL_FALSE;
    STL_TRY( stlReleaseSemaphore( &gAllocatorSem,
                                  &gErrorStack )
             == STL_SUCCESS );

    ZLPL_INIT_CONTEXT(sSqlContext);

    STL_TRY_THROW( stlCreateRegionAllocator( &sSqlContext->mAllocator,
                                             ZLPL_REGION_INIT_SIZE,
                                             ZLPL_REGION_NEXT_SIZE,
                                             &gErrorStack )
                   == STL_SUCCESS, RAMP_ERR_CREATE_ALLOCATOR );

    STL_TRY_THROW( stlCreateDynamicAllocator( & sSqlContext->mDynMemory,
                                              ZLPL_DYNAMIC_INIT_SIZE,
                                              ZLPL_DYNAMIC_NEXT_SIZE,
                                              & gErrorStack )
                  == STL_SUCCESS, RAMP_ERR_CREATE_ALLOCATOR );

    *aSqlContext = sSqlContext;

    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_ALLOCATE_MEM)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_ALLOCATE_MEM,
                      NULL,
                      &gErrorStack );
    }
    STL_CATCH(RAMP_ERR_CREATE_ALLOCATOR)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_ALLOCATE_MEM,
                      NULL,
                      &gErrorStack );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_CREATE_ALLOCATOR,
                      NULL,
                      &gErrorStack );
    }
    STL_FINISH;

    if(sIsAcquireSem == STL_TRUE)
    {
        sIsAcquireSem = STL_FALSE;
        (void)stlReleaseSemaphore(&gAllocatorSem,
                                  &gErrorStack);
    }

    *aSqlContext = NULL;

    return STL_FAILURE;
}

stlStatus zlpuFreeContextMem(zlplSqlContext **aSqlContext)
{
    STL_TRY_THROW( stlDestroyRegionAllocator( &(*aSqlContext)->mAllocator,
                                              &gErrorStack )
                   == STL_SUCCESS, RAMP_ERR_DESTROY_ALLOCATOR );

    STL_TRY_THROW( stlDestroyDynamicAllocator( & (*aSqlContext)->mDynMemory,
                                               & gErrorStack )
                   == STL_SUCCESS, RAMP_ERR_DESTROY_ALLOCATOR);

    *aSqlContext = NULL;

    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_DESTROY_ALLOCATOR)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_DESTROY_ALLOCATOR,
                      NULL,
                      &gErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlpuAllocStmt(SQLHANDLE   *aStmtHandle,
                        SQLHANDLE    aDbc,
                        zlplSqlArgs *aSqlArgs)
{
    SQLRETURN  sReturn;

    sReturn = SQLAllocHandle(SQL_HANDLE_STMT,
                             aDbc,
                             aStmtHandle);
    STL_TRY(zlpeCheckError(sReturn,
                           SQL_HANDLE_DBC,
                           aDbc,
                           (zlplSqlca *)aSqlArgs->sql_ca,
                           (stlChar *)aSqlArgs->sql_state)
            == STL_SUCCESS);

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlpuFreeStmt(SQLHANDLE  aStmtHandle)
{
    SQLFreeHandle(SQL_HANDLE_STMT, aStmtHandle);

    return STL_SUCCESS;
}

stlInt32 zlpuGetMinArraySize( stlInt32          aIterationValue,
                              stlInt32          aHostVarCount,
                              zlplSqlHostVar   *aHostVar,
                              stlInt32         *aIteration )
{
    stlInt32     i;
    stlInt32     sMinArraySize = STL_INT32_MAX;

    for(i = 0; i < aHostVarCount; i ++)
    {
        if( sMinArraySize > aHostVar[i].arr_size )
        {
            sMinArraySize = aHostVar[i].arr_size;
        }
    }

    if( aIterationValue != 0 )
    {
        sMinArraySize = STL_MIN( sMinArraySize, aIterationValue );
    }

    if( (sMinArraySize == 0) || (sMinArraySize == STL_INT32_MAX) )
    {
        *aIteration = 1;
    }
    else
    {
        *aIteration = sMinArraySize;
    }

    return sMinArraySize;
}


/** @} */
