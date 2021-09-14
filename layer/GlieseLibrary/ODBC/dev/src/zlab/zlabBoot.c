/*******************************************************************************
 * zlabBoot.c
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

#include <goldilocks.h>
#include <scl.h>
#include <sll.h>
#include <zlDef.h>
#include <zle.h>

/**
 * @file zlabBoot.c
 * @brief Gliese API Internal Boot Routines.
 */

/**
 * @addtogroup zlb
 * @{
 */

extern stlErrorRecord gGlieseLibraryErrorTable[ZLE_MAX_ERROR + 1];

static knlWarmupFunc * gZlabWarmupLayer[STL_LAYER_MAX] =
{
    NULL,        /* STL_LAYER_NONE */
    NULL,        /* STL_LAYER_STANDARD */
    NULL,        /* STL_LAYER_DATATYPE */
    NULL,        /* STL_LAYER_COMMUNICATION */
    &gWarmupKN,  /* STL_LAYER_KERNEL */
    &gWarmupSC,  /* STL_LAYER_SERVER_COMMUNICATION */
    &gWarmupSM,  /* STL_LAYER_STORAGE_MANAGER */
    &gWarmupEL,  /* STL_LAYER_EXECUTION_LIBRARY */
    &gWarmupQP,  /* STL_LAYER_SQL_PROCESSOR */
    NULL,        /* STL_LAYER_PSM_PROCESSOR */
    &gWarmupSS,  /* STL_LAYER_SESSION */
    &gWarmupSL,  /* STL_LAYER_SERVER_LIBRARY */
    NULL,        /* STL_LAYER_GLIESE_LIBRARY */
    NULL,        /* STL_LAYER_GLIESE_TOOL */
    NULL,        /* STL_LAYER_GOLDILOCKS_LIBRARY */
    NULL,        /* STL_LAYER_GOLDILOCKS_TOOL */
};

static knlCooldownFunc * gZlabCooldownLayer[STL_LAYER_MAX] =
{
    NULL,         /* STL_LAYER_NONE */
    NULL,         /* STL_LAYER_STANDARD */
    NULL,         /* STL_LAYER_DATATYPE */
    NULL,         /* STL_LAYER_COMMUNICATION */
    &gCooldownKN, /* STL_LAYER_KERNEL */
    &gCooldownSC, /* STL_LAYER_SERVER_COMMUNICATION */
    &gCooldownSM, /* STL_LAYER_STORAGE_MANAGER */
    &gCooldownEL, /* STL_LAYER_EXECUTION_LIBRARY */
    &gCooldownQP, /* STL_LAYER_SQL_PROCESSOR */
    NULL,         /* STL_LAYER_PSM_PROCESSOR */
    &gCooldownSS, /* STL_LAYER_SESSION */
    &gCooldownSL, /* STL_LAYER_SERVER_LIBRARY */
    NULL,         /* STL_LAYER_GLIESE_LIBRARY */
    NULL,         /* STL_LAYER_GLIESE_TOOL */
    NULL,         /* STL_LAYER_GOLDILOCKS_LIBRARY */
    NULL,         /* STL_LAYER_GOLDILOCKS_TOOL */
};

stlUInt32 gZlabWarmState = ZLB_WARM_STATE_INIT;

static stlStatus zlabAllocSystem( void * aEnv,
                                  void * aSession )
{
    stlInt32 sState = 0;

    STL_TRY( sslInitializeEnv( (sslEnv*)aEnv, KNL_ENV_HEAP ) == STL_SUCCESS );
    sState = 1;

    knlSetTopLayer( STL_LAYER_GLIESE_LIBRARY, KNL_ENV( aEnv ) );

    stlMemset( aSession, 0x00, STL_SIZEOF( sslSessionEnv ) );

    STL_TRY( sslInitSessionEnv( (sslSessionEnv*)aSession,
                                KNL_CONNECTION_TYPE_NONE,
                                KNL_SESSION_TYPE_DEDICATE,
                                KNL_SESS_ENV_HEAP,
                                (sslEnv*)aEnv ) == STL_SUCCESS );
    sState = 2;

    knlSetSessionTopLayer( (knlSessionEnv*)aSession, STL_LAYER_GLIESE_LIBRARY );
    
    KNL_LINK_SESS_ENV( aEnv, aSession );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2 :
            (void)sslFiniSessionEnv( (sslSessionEnv*)aSession, (sslEnv*)aEnv );
        case 1 :
            (void)sslFinalizeEnv( (sslEnv*)aEnv );
        default :
            break;
    }

    return STL_FAILURE;
}

static stlStatus zlabFreeSystem( sslEnv        * aEnv,
                                 sslSessionEnv * aSession )
{
    stlInt32 sState = 2;

    sState = 1;
    STL_TRY( sslFiniSessionEnv( (sslSessionEnv*)aSession, (sslEnv*)aEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( sslFinalizeEnv( (sslEnv*)aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;
    
    switch( sState )
    {
        case 2 :
            (void)sslFiniSessionEnv( (sslSessionEnv*)aSession, (sslEnv*)aEnv );
        case 1 :
            (void)sslFinalizeEnv( (sslEnv*)aEnv );
        default :
            break;
    }

    return STL_FAILURE;
}

stlStatus zlabWarmup( stlErrorStack * aErrorStack )
{
    stlBool         sRetry;
    stlUInt32       sOldState;
    sslEnv          sSystemEnv;
    sslSessionEnv   sSystemSession;
    stlBool         sIsAllocSystem = STL_FALSE;

    stlInt32        sLayer;
    knlWarmupFunc * sLayerFunc;
    stlInt32        sSignalState = 0;

    do
    {
        sRetry = STL_FALSE;

        switch( gZlabWarmState )
        {
            case ZLB_WARM_STATE_INIT :
                sOldState = stlAtomicCas32( &gZlabWarmState,
                                            ZLB_WARM_STATE_WARMMING,
                                            ZLB_WARM_STATE_INIT );

                if( sOldState == ZLB_WARM_STATE_INIT )
                {
                    STL_TRY( zlabAllocSystem( (void*)&sSystemEnv,
                                              (void*)&sSystemSession ) == STL_SUCCESS );
                    sIsAllocSystem = STL_TRUE;

                    /*
                     * Layer 별 Warm-Up 수행
                     */

                    STL_TRY( stlBlockControllableSignals( STL_SIGNAL_UNKNOWN,
                                                          KNL_ERROR_STACK(&sSystemEnv) )
                             == STL_SUCCESS );
                    sSignalState = 1;
    
                    for( sLayer = 0; sLayer < STL_LAYER_MAX; sLayer++ )
                    {
                        sLayerFunc = gZlabWarmupLayer[sLayer];
            
                        if( sLayerFunc == NULL )
                        {
                            continue;
                        }
                        else
                        {
                            STL_TRY( (*sLayerFunc)( (void *)&sSystemEnv ) == STL_SUCCESS );
                        }
                    }

                    sSignalState = 0;
                    STL_TRY( stlUnblockControllableSignals( STL_SIGNAL_UNKNOWN,
                                                            KNL_ERROR_STACK(&sSystemEnv) )
                             == STL_SUCCESS );

                    gZlabWarmState = ZLB_WARM_STATE_WARMED;
                }
                else
                {
                    sRetry = STL_TRUE;

                    stlYieldThread();
                }
                break;
            case ZLB_WARM_STATE_WARMMING :
            case ZLB_WARM_STATE_COOLING :
                stlYieldThread();
                sRetry = STL_TRUE;
                break;
            case ZLB_WARM_STATE_WARMED :
                break;
        }
    } while( sRetry == STL_TRUE );

    if( sIsAllocSystem == STL_TRUE )
    {
        sIsAllocSystem = STL_FALSE;
        STL_TRY( zlabFreeSystem( (void*)&sSystemEnv,
                                 (void*)&sSystemSession ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sSignalState == 1 )
    {
        (void) stlUnblockControllableSignals( STL_SIGNAL_UNKNOWN,
                                              KNL_ERROR_STACK(&sSystemEnv) );
    }

    if( sIsAllocSystem == STL_TRUE )
    {
        stlAppendErrors( aErrorStack, KNL_ERROR_STACK( &sSystemEnv ) );
        (void)zlabFreeSystem( (void*)&sSystemEnv, (void*)&sSystemSession );
    }

    gZlabWarmState = ZLB_WARM_STATE_INIT;
    
    return STL_FAILURE;
}

stlStatus zlabCoolDown( stlErrorStack * aErrorStack )
{
    stlUInt32         sOldState;
    sslEnv            sSystemEnv;
    sslSessionEnv     sSystemSession;
    stlBool           sIsAllocSystem = STL_FALSE;
    
    stlInt32          sLayer;
    knlCooldownFunc * sLayerFunc;
    stlInt32          sSignalState = 0;

    sOldState = stlAtomicCas32( &gZlabWarmState,
                                ZLB_WARM_STATE_COOLING,
                                ZLB_WARM_STATE_WARMED );
    
    if( sOldState == ZLB_WARM_STATE_WARMED )
    {
        STL_TRY( zlabAllocSystem( (void*)&sSystemEnv,
                                  (void*)&sSystemSession ) == STL_SUCCESS );
        sIsAllocSystem = STL_TRUE;
    
        STL_TRY( stlBlockControllableSignals( STL_SIGNAL_UNKNOWN,
                                              KNL_ERROR_STACK(&sSystemEnv) )
                 == STL_SUCCESS );
        sSignalState = 1;
    
        /*
         * Layer 별 Cool-Down 수행
         */
    
        for ( sLayer = ( STL_LAYER_MAX - 1 ); sLayer >= 0; sLayer-- )
        {
            sLayerFunc = gZlabCooldownLayer[sLayer];
            
            if( sLayerFunc == NULL )
            {
                continue;
            }
            else
            {
                STL_TRY( (*sLayerFunc)( (void *)&sSystemEnv ) == STL_SUCCESS );
            }
        }
    
        sSignalState = 0;
        STL_TRY( stlUnblockControllableSignals( STL_SIGNAL_UNKNOWN,
                                                KNL_ERROR_STACK(&sSystemEnv) )
                 == STL_SUCCESS );

        gZlabWarmState = ZLB_WARM_STATE_INIT;
    }
    
    if( sIsAllocSystem == STL_TRUE )
    {
        sIsAllocSystem = STL_FALSE;
        STL_TRY( zlabFreeSystem( (void*)&sSystemEnv,
                                 (void*)&sSystemSession ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;
    
    if( sSignalState == 1 )
    {
        (void) stlUnblockControllableSignals( STL_SIGNAL_UNKNOWN, KNL_ERROR_STACK(&sSystemEnv) );
    }

    if( sIsAllocSystem == STL_TRUE )
    {
        stlAppendErrors( aErrorStack, KNL_ERROR_STACK( &sSystemEnv ) );
        (void)zlabFreeSystem( (void*)&sSystemEnv, (void*)&sSystemSession );
    }
    
    gZlabWarmState = ZLB_WARM_STATE_INIT;
    
    return STL_FAILURE;
}

/** @} */
