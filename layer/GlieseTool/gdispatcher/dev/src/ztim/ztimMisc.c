/*******************************************************************************
 * ztimMisc.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztimMisc.c 8377 2013-05-06 04:07:18Z mycomman $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file ztimMisc.c
 * @brief Gliese gserver Misc Routines
 */

#include <stl.h>
#include <cml.h>
#include <scl.h>
#include <ztiDef.h>
#include <ztim.h>


stlBool gZtimWarmedUp = STL_FALSE;

static knlWarmupFunc * gZtimWarmupLayer[STL_LAYER_MAX] =
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

static knlCooldownFunc * gZtimCooldownLayer[STL_LAYER_MAX] =
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

stlGetOptOption gZtimOptOption[] =
{
    { "msgqkey",        'm',  STL_TRUE,   "message queue key for startup" },
    { "index",          'd',  STL_TRUE,   "index" },
    { "no-daemon",      'n',  STL_FALSE,  "doesn't daemonize" },
    /* Dispatcher 최대 열 수 있는 파일의 수 */
    { NULL,             '\0', STL_FALSE,  NULL }
};


stlStatus ztimDaemonize( stlBool         aDaemonize,
                         stlErrorStack * aErrorStack )
{
    stlProc            sProc;
    stlBool            sIsChild = STL_TRUE;

    /**
     * Demonize
     */
    if( aDaemonize == STL_TRUE )
    {
        STL_TRY( stlForkProc( &sProc, &sIsChild, aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        /* Debug 등을 위하여 Daemonize를 하지 않을때는 fork를 하지 않는다 */
        sIsChild = STL_TRUE;
    }

    if( sIsChild == STL_FALSE )
    {
        /*
         * Parent process goes to exit
         */
        stlExit(0);
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Gliese Dispatcher를 startup 한다.
 */
stlStatus ztimStartup( stlBool         aDaemonize,
                       stlErrorStack * aErrorStack )
{
    STL_TRY( ztimDaemonize( aDaemonize, aErrorStack ) == STL_SUCCESS );

    /* Block Signals */
    STL_TRY( stlSetSignalHandler( STL_SIGNAL_HUP,
                                  STL_SIGNAL_IGNORE,
                                  NULL,  /* aOldHandler */
                                  aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlSetSignalHandler( STL_SIGNAL_TERM,
                                  STL_SIGNAL_IGNORE,
                                  NULL,  /* aOldHandler */
                                  aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlSetSignalHandler( STL_SIGNAL_QUIT,
                                  STL_SIGNAL_IGNORE,
                                  NULL,  /* aOldHandler */
                                  aErrorStack )
             == STL_SUCCESS );

    if( aDaemonize == STL_TRUE )
    {
        STL_TRY( stlSetSignalHandler( STL_SIGNAL_INT,
                                      STL_SIGNAL_IGNORE,
                                      NULL,  /* aOldHandler */
                                      aErrorStack )
                 == STL_SUCCESS );
    }

    STL_TRY( stlSetSignalHandler( STL_SIGNAL_USR2,
                                  STL_SIGNAL_IGNORE,
                                  NULL,  /* aOldHandler */
                                  aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlSetSignalHandler( STL_SIGNAL_CLD,
                                  STL_SIGNAL_IGNORE,
                                  NULL,  /* aOldHandler */
                                  aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztimWarmup( aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;


    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztimShutdown( stlErrorStack * aErrorStack )
{
    STL_TRY( ztimCooldown( aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztimWarmup( stlErrorStack * aErrorStack )
{
    stlInt32         sState = 0;
    stlInt32         sLayer = 0;
    knlWarmupFunc  * sLayerFunc;
    sllEnv           sEnv;
    sllSessionEnv    sSessionEnv;

    STL_TRY( sllInitializeEnv( &sEnv, KNL_ENV_HEAP ) == STL_SUCCESS );
    sState = 1;

    knlSetTopLayer( STL_LAYER_SERVER_LIBRARY, KNL_ENV( &sEnv ) );

    stlMemset( (void*)&sSessionEnv, 0x00, STL_SIZEOF( sllSessionEnv ) );

    STL_TRY( sllInitializeSessionEnv( &sSessionEnv,
                                      KNL_CONNECTION_TYPE_NONE,
                                      KNL_SESSION_TYPE_DEDICATE,
                                      KNL_SESS_ENV_HEAP,
                                      &sEnv )
             == STL_SUCCESS );
    sState = 2;

    KNL_LINK_SESS_ENV( &sEnv, &sSessionEnv );

    /*
     * Layer 별 Warm-Up 수행
     */

    STL_TRY( stlBlockControllableSignals( STL_SIGNAL_UNKNOWN,
                                          aErrorStack )
             == STL_SUCCESS );
    sState = 3;

    for( sLayer = 0; sLayer < STL_LAYER_MAX; sLayer++ )
    {
        sLayerFunc = gZtimWarmupLayer[sLayer];

        if( sLayerFunc == NULL )
        {
            continue;
        }
        else
        {
            STL_TRY( (*sLayerFunc)( (void *)&sEnv ) == STL_SUCCESS );
        }
    }

    if( sLayer == STL_LAYER_MAX )
    {
        gZtimWarmedUp = STL_TRUE;
    }
    else
    {
    	gZtimWarmedUp = STL_FALSE;
    }

    sState = 2;
    STL_TRY( stlUnblockControllableSignals( STL_SIGNAL_UNKNOWN,
                                            aErrorStack )
             == STL_SUCCESS );

    sState = 1;
    STL_TRY( sllFinalizeSessionEnv( &sSessionEnv,
                                    &sEnv )
             == STL_SUCCESS );

    sState = 0;
    STL_TRY( sllFinalizeEnv( &sEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 3:
            (void) stlUnblockControllableSignals( STL_SIGNAL_UNKNOWN, KNL_ERROR_STACK(&sEnv) );
        case 2:
            (void) sllFinalizeSessionEnv( &sSessionEnv, &sEnv );
        case 1:
            stlAppendErrors( (aErrorStack), KNL_ERROR_STACK((&sEnv)) );
            (void) sllFinalizeEnv( &sEnv );
        default:
            break;
    }
            
    gZtimWarmedUp = STL_FALSE;

    return STL_FAILURE;
}


stlStatus ztimCooldown( stlErrorStack * aErrorStack )
{
    stlInt32         sLayer;
    sllEnv           sEnv;
    sllSessionEnv    sSessionEnv;
    stlInt32         sState = 0;

    if( gZtimWarmedUp == STL_TRUE )
    {
        STL_TRY( sllInitializeEnv( &sEnv, KNL_ENV_HEAP ) == STL_SUCCESS );
        sState = 1;

        knlSetTopLayer( STL_LAYER_SERVER_LIBRARY, KNL_ENV( &sEnv ) );

        stlMemset( (void*)&sSessionEnv, 0x00, STL_SIZEOF( sllSessionEnv ) );

        STL_TRY( sllInitializeSessionEnv( &sSessionEnv,
                                          KNL_CONNECTION_TYPE_NONE,
                                          KNL_SESSION_TYPE_DEDICATE,
                                          KNL_SESS_ENV_HEAP,
                                          &sEnv )
                 == STL_SUCCESS );
        sState = 2;

        KNL_LINK_SESS_ENV( &sEnv, &sSessionEnv );

    	gZtimWarmedUp = STL_FALSE;

        (void) stlBlockControllableSignals( STL_SIGNAL_UNKNOWN,
                                            aErrorStack );
        sState = 3;

        /*
         * Layer 별 Cool-Down 수행
         */
        for ( sLayer = ( STL_LAYER_MAX - 1 ); sLayer >= 0; sLayer-- )
        {
            if ( gZtimCooldownLayer[sLayer] == NULL )
            {
                continue;
            }
            else
            {
                (void)( (*gZtimCooldownLayer[sLayer])( (void *)&sEnv ) == STL_SUCCESS );
            }
        }

        sState = 2;
        (void) stlUnblockControllableSignals( STL_SIGNAL_UNKNOWN,
                                              aErrorStack );
        
        sState = 1;
        (void) sllFinalizeSessionEnv( &sSessionEnv, &sEnv );

        sState = 0;
        (void) sllFinalizeEnv( &sEnv );
    }

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 3:
            (void) stlUnblockControllableSignals( STL_SIGNAL_UNKNOWN, KNL_ERROR_STACK(&sEnv) );
        case 2:
            (void) sllFinalizeSessionEnv( &sSessionEnv, &sEnv );
        case 1:
            stlAppendErrors( (aErrorStack), KNL_ERROR_STACK((&sEnv)) );
            (void) sllFinalizeEnv( &sEnv );
        default:
            break;
    }
            
    return STL_SUCCESS;
}

