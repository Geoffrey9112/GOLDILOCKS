/*******************************************************************************
 * sslEnv.c
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
 * @file sslEnv.c
 * @brief Session Environment Routines
 */

#include <qll.h>
#include <sslDef.h>

/**
 * @addtogroup sslProcEnv
 * @{
 */

stlStatus sslAllocEnv( sslEnv         ** aAllocEnv,
                       stlErrorStack   * aErrorStack )
{
    STL_PARAM_VALIDATE( aAllocEnv != NULL, aErrorStack );

    STL_TRY( knlAllocEnv( (void**)aAllocEnv,
                          aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus sslFreeEnv( sslEnv        * aFreeEnv,
                      stlErrorStack * aErrorStack )
{
    STL_PARAM_VALIDATE( aFreeEnv != NULL, aErrorStack );

    STL_TRY( knlFreeEnv( (void*)aFreeEnv,
                         aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
}

/**
 * @brief Session의 Process Envrionment를 초기화 한다.
 * @param[out]    aEnv        초기화될 Process Environment 포인터
 * @param[in]     aEnvType    Env Type
 */
stlStatus sslInitializeEnv( sslEnv     * aEnv,
                            knlEnvType   aEnvType )
{
    stlInt32 sState = 0;
    
    /*
     * 하위 layer 의 Process Environment 초기화
     */
    
    STL_TRY( qllInitializeEnv( QLL_ENV( aEnv ), aEnvType ) == STL_SUCCESS );
    sState = 1;

    aEnv->mXaRecoverScanInfo.mScanState = SSL_XA_SCAN_STATE_NONE;
    
    STL_TRY( knlCreateRegionMem( &aEnv->mOperationHeapMem,
                                 KNL_ALLOCATOR_SESSION_OPERATION,
                                 NULL,  /* aParentMem */
                                 KNL_MEM_STORAGE_TYPE_HEAP,
                                 KNL_DEFAULT_CHUNK_SIZE,
                                 KNL_DEFAULT_CHUNK_SIZE,
                                 KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 2;
    
    /*
     * Environment에 Top Layer 설정
     */
    
    knlSetTopLayer( STL_LAYER_SESSION, KNL_ENV( aEnv ) );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) knlDestroyRegionMem( &aEnv->mOperationHeapMem, KNL_ENV(aEnv) );
        case 1:
            (void) qllFinalizeEnv( QLL_ENV(aEnv) );
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief Session의 Process Envrionment를 종료한다.
 * @param[in] aEnv 종료할 Process Environment 포인터
 */
stlStatus sslFinalizeEnv( sslEnv * aEnv )
{
    STL_TRY( knlDestroyRegionMem( &aEnv->mOperationHeapMem,
                                  KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    KNL_INIT_REGION_MEMORY( &aEnv->mOperationHeapMem );
    
    /*
     * 하위 layer 의 Process Environment 를 종료
     */
    
    STL_TRY( qllFinalizeEnv( QLL_ENV( aEnv ) ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} sslProcEnv */
