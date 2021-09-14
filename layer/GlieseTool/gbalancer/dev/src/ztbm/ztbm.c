/*******************************************************************************
 * ztbm.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztbm.c 12157 2014-04-23 03:50:31Z lym999 $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file ztbm.c
 * @brief Gliese Balancer Main Routines
 */

#include <stl.h>
#include <cml.h>
#include <knl.h>
#include <scl.h>
#include <sll.h>
#include <ztbDef.h>
#include <ztbm.h>

stlBool gZtbmWarmedUp = STL_FALSE;
stlFatalHandler  gZtbmOldFatalHandler;

static knlWarmupFunc * gZtbmWarmupLayer[STL_LAYER_MAX] =
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

static knlCooldownFunc * gZtbmCooldownLayer[STL_LAYER_MAX] =
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

int main( int     aArgc,
          char ** aArgv )
{
    stlGetOpt         sOpt;
    stlChar           sCh;
    const stlChar   * sOptionArg;
    stlErrorStack     sErrorStack;
    stlInt32          sState = 0;
    stlBool           sDaemonize = STL_TRUE;
    sllEnv          * sEnv;
    sllSessionEnv   * sSessionEnv = NULL;
    stlBool           sEnableAging = STL_FALSE;
    stlChar         * sEndPtr;
    stlInt64          sMsgKey = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    STL_TRY( sllInitialize() == STL_SUCCESS );
    sState = 1;

    STL_TRY( stlHookFatalHandler( ztbmFatalHandler,
                                  & gZtbmOldFatalHandler )
             == STL_SUCCESS );

    stlRegisterErrorTable( STL_ERROR_MODULE_GLIESE_TOOL_GBALANCER,
                           gZtbeErrorTable );

    STL_TRY( stlInitGetOption( &sOpt,
                               aArgc,
                               (const stlChar* const *)aArgv,
                               &sErrorStack )
             == STL_SUCCESS );

    while( STL_TRUE )
    {
        if( stlGetOptionLong( &sOpt,
                              gZtbmOptOption,
                              &sCh,
                              &sOptionArg,
                              &sErrorStack ) != STL_SUCCESS )
        {
            if( stlGetLastErrorCode( &sErrorStack ) != STL_ERRCODE_EOF )
            {
                if( stlGetLastErrorCode( &sErrorStack ) == STL_ERRCODE_INVALID_ARGUMENT )
                {
                    STL_THROW( RAMP_ERR_INVALID_OPTION );
                }
                    
                STL_TRY( STL_FALSE );
            }
                
            stlPopError( &sErrorStack );
            break;
        }
        
        switch( sCh )
        {
            case 'm':
                STL_TRY( stlStrToInt64( sOptionArg,
                                        STL_NTS,
                                        &sEndPtr,
                                        10,
                                        &sMsgKey,
                                        &sErrorStack )
                         == STL_SUCCESS );
                break;
            default:
                STL_THROW( RAMP_ERR_INVALID_OPTION );
                break;
        }
    }

    STL_TRY_THROW( sMsgKey != 0, RAMP_ERR_INVALID_OPTION );

    STL_TRY( ztbmDaemonize( sDaemonize, &sErrorStack ) == STL_SUCCESS );

    STL_TRY( ztbmStartup( sDaemonize,
                          &sErrorStack )
             == STL_SUCCESS );
    sState = 2;

    STL_TRY( sslAllocEnv( (sslEnv **)&sEnv,
                          &sErrorStack ) == STL_SUCCESS );
    sState = 3;

    STL_TRY( sllInitializeEnv( SLL_ENV(sEnv), KNL_ENV_SHARED ) == STL_SUCCESS );
    sState = 4;

    STL_TRY( sslAllocSession( "",
                              (sslSessionEnv **)&sSessionEnv,
                              SSL_ENV(sEnv) )
             == STL_SUCCESS );
    sState = 5;

    STL_TRY( sllInitializeSessionEnv( (sllSessionEnv*)sSessionEnv,
                                      KNL_CONNECTION_TYPE_DA,
                                      KNL_SESSION_TYPE_DEDICATE,
                                      KNL_SESS_ENV_SHARED,
                                      SLL_ENV(sEnv) )
             == STL_SUCCESS );
    sState = 6;

    KNL_LINK_SESS_ENV( sEnv, sSessionEnv );
    KNL_ENABLE_SESSION_AGING( sSessionEnv );
    sEnableAging = STL_TRUE;

    STL_TRY( ztbmRun( &sMsgKey, sEnv, sSessionEnv ) == STL_SUCCESS );

    sState = 4;
    (void)sllFinalizeSessionEnv( (sllSessionEnv*)sSessionEnv,
                                 SLL_ENV(sEnv) );

    sState = 2;
    (void)sllFinalizeEnv( SLL_ENV(sEnv) );

    knlDisconnectSession( sSessionEnv );

    sState = 1;
    (void)ztbmShutdown( &sErrorStack );

    sState = 0;
    (void)sllTerminate();

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_OPTION )
    {
        STL_DASSERT( STL_FALSE );
    }

    STL_FINISH;

    switch( sState )
    {
        case 6:
            (void)sllFinalizeSessionEnv( (sllSessionEnv*)sSessionEnv, SLL_ENV(sEnv) );
        case 5:
            knlLogErrorCallStack( NULL, KNL_ENV(sEnv) );
            knlLogMsg( NULL,
                       KNL_ENV(sEnv),
                       KNL_LOG_LEVEL_FATAL,
                       "[BALANCER] process abnormally terminated\n" );

            if( sEnableAging == STL_FALSE )
            {
                (void)sslFreeSession( (void *)sSessionEnv, SSL_ENV(sEnv) );
            }
        case 4:
            stlAppendErrors( &sErrorStack, KNL_ERROR_STACK(sEnv) );
            (void)sllFinalizeEnv( SLL_ENV(sEnv) );
            if( sSessionEnv != NULL )
            {
                knlDisconnectSession( sSessionEnv );
            }
        case 3:
            if( sEnableAging == STL_FALSE )
            {
                (void)sslFreeEnv( SSL_ENV(sEnv), &sErrorStack );
            }
        case 2:
            (void)ztbmShutdown( &sErrorStack );
        case 1:
            (void)sllTerminate();
            break;
        default:
            break;
    }

    if( sMsgKey != 0 )
    {
        (void)stlWakeupExecProc( sMsgKey, &sErrorStack );
    }

    return STL_FAILURE;
}



stlStatus ztbmStartup( stlBool         aDoDaemonize,
                       stlErrorStack * aErrorStack )
{
    STL_TRY( stlSetSignalHandler( STL_SIGNAL_HUP,
                                  STL_SIGNAL_IGNORE,
                                  NULL,  /* aOldHandler */
                                  aErrorStack )
             == STL_SUCCESS );

    if( aDoDaemonize == STL_TRUE )
    {
        STL_TRY( stlSetSignalHandler( STL_SIGNAL_TERM,
                                      STL_SIGNAL_IGNORE,
                                      NULL,  /* aOldHandler */
                                      aErrorStack )
                 == STL_SUCCESS );
    }

    STL_TRY( stlSetSignalHandler( STL_SIGNAL_QUIT,
                                  STL_SIGNAL_IGNORE,
                                  NULL,  /* aOldHandler */
                                  aErrorStack )
             == STL_SUCCESS );

    if( aDoDaemonize == STL_TRUE )
    {
        STL_TRY( stlSetSignalHandler( STL_SIGNAL_INT,
                                      STL_SIGNAL_IGNORE,
                                      NULL,  /* aOldHandler */
                                      aErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( stlSetSignalHandler( STL_SIGNAL_INT,
                                      SIG_DFL,
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

    STL_TRY( ztbmWarmup( aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztbmShutdown( stlErrorStack * aErrorStack )
{
    STL_TRY( ztbmCooldown( aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztbmWarmup( stlErrorStack * aErrorStack )
{
    stlInt32         sState = 0;
    stlInt32         sLayer = 0;
    knlWarmupFunc  * sLayerFunc;
    sllEnv           sEnv;
    sllSessionEnv    sSessionEnv;

    STL_TRY( sllInitializeEnv( &sEnv, KNL_ENV_HEAP ) == STL_SUCCESS );
    sState = 1;

    knlSetTopLayer( STL_LAYER_SERVER_LIBRARY, KNL_ENV( &sEnv ) );

    stlMemset( (void*)&sSessionEnv, 0x00, STL_SIZEOF( knlSessionEnv ) );

    STL_TRY( sllInitializeSessionEnv( &sSessionEnv,
                                      KNL_CONNECTION_TYPE_NONE,
                                      KNL_SESSION_TYPE_DEDICATE,
                                      KNL_SESS_ENV_HEAP,
                                      &sEnv ) == STL_SUCCESS );
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
        sLayerFunc = gZtbmWarmupLayer[sLayer];

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
        gZtbmWarmedUp = STL_TRUE;
    }
    else
    {
        gZtbmWarmedUp = STL_FALSE;
    }

    sState = 2;
    STL_TRY( stlUnblockControllableSignals( STL_SIGNAL_UNKNOWN,
                                            aErrorStack )
             == STL_SUCCESS );

    sState = 1;
    STL_TRY( sllFinalizeSessionEnv( &sSessionEnv,
                                    &sEnv ) == STL_SUCCESS );

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
            stlAppendErrors( aErrorStack, KNL_ERROR_STACK(&sEnv) );
            (void) sllFinalizeEnv( &sEnv );
        default:
            break;
    }

    gZtbmWarmedUp = STL_FALSE;

    return STL_FAILURE;
}


stlStatus ztbmCooldown( stlErrorStack * aErrorStack )
{
    stlInt32         sLayer;
    sllEnv           sEnv;
    sllSessionEnv    sSessionEnv;
    stlInt32         sState = 0;

    if( gZtbmWarmedUp == STL_TRUE )
    {
        STL_TRY( sllInitializeEnv( &sEnv, KNL_ENV_HEAP ) == STL_SUCCESS );
        sState = 1;

        knlSetTopLayer( STL_LAYER_SERVER_LIBRARY, KNL_ENV( &sEnv ) );

        stlMemset( (void*)&sSessionEnv, 0x00, STL_SIZEOF( sllSessionEnv ) );

        STL_TRY( sllInitializeSessionEnv( &sSessionEnv,
                                          KNL_CONNECTION_TYPE_NONE,
                                          KNL_SESSION_TYPE_DEDICATE,
                                          KNL_SESS_ENV_HEAP,
                                          &sEnv ) == STL_SUCCESS );
        sState = 2;

        KNL_LINK_SESS_ENV( &sEnv, &sSessionEnv );

        gZtbmWarmedUp = STL_FALSE;

        (void) stlBlockControllableSignals( STL_SIGNAL_UNKNOWN,
                                            aErrorStack );
        sState = 3;

        /*
         * Layer 별 Cool-Down 수행
         */
        for ( sLayer = ( STL_LAYER_MAX - 1 ); sLayer >= 0; sLayer-- )
        {
            if ( gZtbmCooldownLayer[sLayer] == NULL )
            {
                continue;
            }
            else
            {
                (void)( (*gZtbmCooldownLayer[sLayer])( (void *)&sEnv ) == STL_SUCCESS );
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
            stlAppendErrors( aErrorStack, KNL_ERROR_STACK(&sEnv) );
            (void) sllFinalizeEnv( &sEnv );
        default:
            break;
    }

    return STL_SUCCESS;
}

/**
 * @brief Server Library Layer의 Fatal Handler.
 * @param[in] aCauseString Fatal 유발 원인을 알수 있는 스트링
 * @param[in] aSigInfo Signal Information
 * @param[in] aContext 이전 Context 정보
 */
void ztbmFatalHandler( const stlChar * aCauseString,
                       void          * aSigInfo,
                       void          * aContext )
{
    sllEnv          * sMyEnv   = NULL;
    sllSessionEnv   * sMySessionEnv = NULL;
    stlErrorStack     sErrorStack;
    stlUInt32         sSessionEnvId;
    stlUInt32         sEnvId;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sMyEnv = (sllEnv*) knlGetMyEnv();

    STL_TRY_THROW( sMyEnv != NULL, RAMP_FINISH );
    STL_TRY( knlGetEnvId( sMyEnv,
                          &sEnvId )
             == STL_SUCCESS );

    sMySessionEnv = KNL_ENV(sMyEnv)->mSessionEnv;

    STL_TRY_THROW( sMySessionEnv != NULL, RAMP_FINISH );

    STL_TRY( knlGetSessionEnvId( sMySessionEnv,
                                 &sSessionEnvId,
                                 KNL_ENV(sMyEnv) )
             == STL_SUCCESS );

    (void) knlLogMsgUnsafe( NULL,
                            (knlEnv*) sMyEnv,
                            KNL_LOG_LEVEL_FATAL,
                            "[BALANCER INFORMATION]\n"
                            "---- Env Id           : %u\n"
                            "---- SessionEnv Id    : %u\n",
                            sEnvId,
                            sSessionEnvId );

    STL_RAMP( RAMP_FINISH );

    STL_FINISH;

   /**
     * SESSION Layer의 Fatal Handler를 호출한다.
     */

    gZtbmOldFatalHandler( aCauseString, aSigInfo, aContext );
}


