/*******************************************************************************
 * ztsm.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztsm.c 12157 2014-04-23 03:50:31Z lym999 $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file ztsm.c
 * @brief Gliese gserver Main Routines
 */

#include <stl.h>
#include <cml.h>
#include <knl.h>
#include <scl.h>
#include <sml.h>
#include <ell.h>
#include <qll.h>
#include <ssl.h>
#include <sll.h>
#include <ztsDef.h>
#include <ztsm.h>
#include <ztsd.h>
#include <ztss.h>

stlBool gZtsmWarmedUp = STL_FALSE;
stlFatalHandler  gZtsmOldFatalHandler;

extern sllHandle       * gZtsCurrentSllHandle;
extern sllSessionEnv   * gZtsSessionEnv;
extern stlBool           gZtsJoinGroupWorker;
extern sllSharedMemory * gZtsSharedMemory;
extern stlInt32          gZtsSharedServerId;


static knlWarmupFunc * gZtsmWarmupLayer[STL_LAYER_MAX] =
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

static knlCooldownFunc * gZtsmCooldownLayer[STL_LAYER_MAX] =
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
    stlGetOpt               sOpt;
    stlChar                 sCh;
    const stlChar         * sOptionArg;
    stlErrorStack           sErrorStack;
    stlInt32                sState = 0;
    stlChar                 sToListenerPath[STL_MAX_FILE_PATH_LENGTH];
    stlInt64                sTempInt64;
    stlChar               * sEndPtr;
    stlInt32                sIndex = -1;
    cmlSessionType          sSessionMode = CML_SESSION_NONE;
    stlBool                 sDaemonize = STL_TRUE;
    sllEnv                * sEnv = NULL;
    sllSessionEnv         * sSessionEnv = NULL;
    sllHandle               sHandle;
    sclMemoryManager        sMemoryMgr;
    sllProtocolSentence     sProtocolSentence;
    stlContext              sContext;
    stlInt32                sDedicateState = 0;
    stlChar                 sRoleName[STL_MAX_FILE_PATH_LENGTH] = {'\0',};
    stlInt32                sTopLayer = STL_LAYER_SERVER_LIBRARY;
    stlInt64                sMsgKey = 0;
    stlInt32                sConnectionType = KNL_CONNECTION_TYPE_DA;
    cmlCommandType          sCommandType = CML_COMMAND_MAX;

    STL_INIT_ERROR_STACK( &sErrorStack );

    STL_TRY( sllInitialize() == STL_SUCCESS );
    sState = 1;

    STL_TRY( stlHookFatalHandler( ztsmFatalHandler,
                                  & gZtsmOldFatalHandler )
             == STL_SUCCESS );

    stlRegisterErrorTable( STL_ERROR_MODULE_GLIESE_TOOL_GSERVER,
                           gZtseErrorTable );

    STL_TRY( stlInitGetOption( &sOpt,
                               aArgc,
                               (const stlChar* const *)aArgv,
                               &sErrorStack )
             == STL_SUCCESS );

    while( STL_TRUE )
    {
        if( stlGetOptionLong( &sOpt,
                              gZtsmOptOption,
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
            case 'd':
                STL_TRY_THROW( sSessionMode == CML_SESSION_NONE,
                               RAMP_ERR_INVALID_OPTION);
                sSessionMode = CML_SESSION_DEDICATE;
                stlStrncpy( sToListenerPath, sOptionArg, STL_MAX_FILE_PATH_LENGTH );
                break;
            case 's':
                STL_TRY_THROW( sSessionMode == CML_SESSION_NONE,
                               RAMP_ERR_INVALID_OPTION);
                sSessionMode = CML_SESSION_SHARED;

                STL_TRY( stlStrToInt64( sOptionArg,
                                        stlStrlen( sOptionArg ),
                                        &sEndPtr,
                                        10,
                                        &sTempInt64,
                                        &sErrorStack)
                         == STL_SUCCESS );

                STL_ASSERT( sTempInt64 >= 0 && sTempInt64 <= STL_INT32_MAX );
                sIndex = sTempInt64;
                break;
            case 'n':
                sDaemonize = STL_FALSE;
                break;
            default:
                STL_THROW( RAMP_ERR_INVALID_OPTION );
                break;
        }
    }

    STL_TRY( ztsmDaemonize( sDaemonize, &sErrorStack ) == STL_SUCCESS );

    STL_TRY( ztsmStartup( sDaemonize,
                          sSessionMode,
                          &sErrorStack )
             == STL_SUCCESS );
    sState = 2;

    if( sSessionMode == CML_SESSION_DEDICATE )
    {
        /* dedicate는 msg key사용 안함 */
        STL_DASSERT( sMsgKey == 0 );

        STL_TRY( ztsdInitializeDedicate( &sHandle,
                                         &sMemoryMgr,
                                         &sProtocolSentence,
                                         &sContext,
                                         sToListenerPath,
                                         &sCommandType,
                                         &sErrorStack )
                 == STL_SUCCESS );
        sDedicateState = 1;

        stlStrncpy( sRoleName, sHandle.mRoleName, STL_MAX_SQL_IDENTIFIER_LENGTH );

        sTopLayer = STL_LAYER_GLIESE_LIBRARY;
        sConnectionType = KNL_CONNECTION_TYPE_TCP;
    }
    
    STL_TRY( sslAllocEnv( (sslEnv**)&sEnv,
                          &sErrorStack ) == STL_SUCCESS );
    sState = 3;

    STL_TRY( sllInitializeEnv( sEnv, KNL_ENV_SHARED ) == STL_SUCCESS );
    sState = 4;

    knlSetTopLayer( sTopLayer, KNL_ENV(sEnv) );
    
    STL_TRY( sslAllocSession( sRoleName,
                              (sslSessionEnv**)&sSessionEnv,
                              SSL_ENV(sEnv) ) == STL_SUCCESS );
    sState = 5;

    KNL_LINK_SESS_ENV( sEnv, sSessionEnv );
    
    STL_TRY( sllInitializeSessionEnv( sSessionEnv,
                                      sConnectionType,
                                      KNL_SESSION_TYPE_DEDICATE,
                                      KNL_SESS_ENV_SHARED,
                                      sEnv ) == STL_SUCCESS );
    sState = 6;

    knlSetSessionTopLayer( (knlSessionEnv*)sSessionEnv, sTopLayer );
        
    KNL_ENABLE_SESSION_AGING( sSessionEnv );
    sState = 7;

    if( sSessionMode == CML_SESSION_DEDICATE )
    {
        sCommandType = CML_COMMAND_MAX;

        STL_TRY( ztsdProcessDedicate( &sHandle,
                                      &sMemoryMgr,
                                      &sProtocolSentence,
                                      sEnv ) 
                 == STL_SUCCESS );

        sDedicateState = 0;

        STL_TRY( ztsdFinalizeDedicate( &sHandle,
                                       &sMemoryMgr,
                                       &sProtocolSentence,
                                       &sContext,
                                       CML_COMMAND_MAX,  /* received command type */
                                       &sErrorStack )
                 == STL_SUCCESS );
    }
    else if( sSessionMode == CML_SESSION_SHARED )
    {
        STL_TRY_THROW( sIndex >= 0, RAMP_ERR_INVALID_OPTION );
        STL_TRY_THROW( sMsgKey != 0, RAMP_ERR_INVALID_OPTION );

        STL_TRY( ztssProcessShared( &sMsgKey, sIndex, sEnv ) == STL_SUCCESS );
    }
    else
    {
        STL_THROW( RAMP_ERR_INVALID_OPTION );
    }

    if( knlIsTerminatedSessionEnv( (knlSessionEnv *)sSessionEnv ) == STL_FALSE )
    {
        (void) sllFinalizeSessionEnv( sSessionEnv, sEnv );
        (void) sllFinalizeEnv( sEnv );
    
        (void) knlDisconnectSession( sSessionEnv );
    }

    sState = 1;
    STL_TRY( ztsmShutdown( &sErrorStack )
             == STL_SUCCESS );

    sState = 0;
    STL_TRY( sllTerminate() == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_OPTION )
    {
        STL_DASSERT( STL_FALSE );
    }

    STL_FINISH;

    if( sDedicateState == 1 )
    {
        if( sState >= 4 )
        {
            /**
             * sEnv가 활성화 상태면 sEnv의 error를 sErrorStack으로 옮긴다.
             */
            stlAppendErrors( &sErrorStack, KNL_ERROR_STACK(sEnv) );
            STL_INIT_ERROR_STACK( KNL_ERROR_STACK(sEnv) );
        }

        (void) ztsdFinalizeDedicate( &sHandle,
                                     &sMemoryMgr,
                                     &sProtocolSentence,
                                     &sContext,
                                     sCommandType,
                                     &sErrorStack );
    }

    switch( sState )
    {
        case 7:
            (void) knlKillSession( sSessionEnv );
            (void) ztsmShutdown( &sErrorStack );
            (void) sllTerminate();
            break;
        case 6:
            (void) sllFinalizeSessionEnv( sSessionEnv, sEnv );
        case 5:
            (void) sslFreeSession( (sslSessionEnv*)sSessionEnv, SSL_ENV(sEnv) );
        case 4:
            stlAppendErrors( &sErrorStack, KNL_ERROR_STACK(sEnv) );
            (void) sllFinalizeEnv( sEnv );
        case 3:
            (void) sslFreeEnv( SSL_ENV(sEnv), &sErrorStack );
        case 2:
            (void) ztsmShutdown( &sErrorStack );
        case 1:
            (void) sllTerminate();
        default:
            break;
    }

    if( sMsgKey != 0 )
    {
        STL_DASSERT( sSessionMode == CML_SESSION_SHARED );

        (void)stlWakeupExecProc( sMsgKey, &sErrorStack );
    }

    return STL_FAILURE;
}



stlStatus ztsmStartup( stlBool         aDoDaemonize,
                       cmlSessionType  aSessionMode,
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

    STL_TRY( ztsmWarmup( aErrorStack, aSessionMode ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztsmShutdown( stlErrorStack * aErrorStack )
{
    STL_TRY( ztsmCooldown( aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztsmWarmup( stlErrorStack * aErrorStack,
                      cmlSessionType  aSessionMode )
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
        sLayerFunc = gZtsmWarmupLayer[sLayer];

        if( sLayerFunc == NULL )
        {
            continue;
        }
        else
        {
            if( (*sLayerFunc)( (void *)&sEnv ) != STL_SUCCESS )
            {
                /**
                 * SYSDBA로 접근했을 경우에는 warm-up이 실패하더라도,
                 * 계속 진행해야 한다.
                 */

                /**
                 * dedicate면 error stack초기화 하고 break;
                 * shared면 goto STL_FINISH
                 */
                STL_TRY( aSessionMode == CML_SESSION_DEDICATE );

                STL_INIT_ERROR_STACK( KNL_ERROR_STACK(&sEnv) );
                break;
            }
        }
    }

    if( sLayer == STL_LAYER_MAX )
    {
        gZtsmWarmedUp = STL_TRUE;
    }
    else
    {
        gZtsmWarmedUp = STL_FALSE;
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
            
    gZtsmWarmedUp = STL_FALSE;

    return STL_FAILURE;
}


stlStatus ztsmCooldown( stlErrorStack * aErrorStack )
{
    stlInt32         sLayer;
    sllEnv           sEnv;
    sllSessionEnv    sSessionEnv;
    stlInt32         sState = 0;

    if( gZtsmWarmedUp == STL_TRUE )
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

        gZtsmWarmedUp = STL_FALSE;

        (void) stlBlockControllableSignals( STL_SIGNAL_UNKNOWN,
                                            aErrorStack );
        sState = 3;

        /*
         * Layer 별 Cool-Down 수행
         */
        for ( sLayer = ( STL_LAYER_MAX - 1 ); sLayer >= 0; sLayer-- )
        {
            if ( gZtsmCooldownLayer[sLayer] == NULL )
            {
                continue;
            }
            else
            {
                (void)( (*gZtsmCooldownLayer[sLayer])( (void *)&sEnv ) == STL_SUCCESS );
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
void ztsmFatalHandler( const stlChar * aCauseString,
                       void          * aSigInfo,
                       void          * aContext )
{
    sllEnv          * sMyEnv   = NULL;
    sllSessionEnv   * sSessEnv = NULL;
    stlErrorStack     sErrorStack;
    stlUInt32         sServerSessionId = -1;
    stlUInt32         sUserSessionId = -1;
    
    STL_INIT_ERROR_STACK( &sErrorStack );
    
    sMyEnv = (sllEnv*) knlGetMyEnv();
    
    STL_TRY_THROW( sMyEnv != NULL, RAMP_FINISH );
    STL_TRY_THROW( knlGetTopLayer(KNL_ENV(sMyEnv))
                   >= STL_LAYER_SERVER_LIBRARY,
                   RAMP_FINISH );

    if( KNL_GET_FATAL_HANDLING( sMyEnv, STL_LAYER_GLIESE_TOOL ) == STL_FALSE )
    {
        /**
         * 무한 Fatal을 방지해야 한다.
         */
        
        KNL_SET_FATAL_HANDLING( sMyEnv, STL_LAYER_GLIESE_TOOL );
        
        sSessEnv = (sllSessionEnv*)SSL_SESS_ENV( sMyEnv );
        STL_DASSERT( sSessEnv != NULL );

        if( sSessEnv != gZtsSessionEnv )
        {
            knlGetSessionEnvId( sSessEnv,
                                &sUserSessionId,
                                KNL_ENV(sMyEnv) );
        }

        if( gZtsSessionEnv != NULL )
        {
            knlGetSessionEnvId( gZtsSessionEnv,
                                &sServerSessionId,
                                KNL_ENV(sMyEnv) );
        }
        
        (void) knlLogMsgUnsafe( NULL,
                                (knlEnv*) sMyEnv,
                                KNL_LOG_LEVEL_FATAL,
                                "[SHARED/DEDICATE SERVER INFORMATION]\n"
                                "---- Session Type      : %d\n"
                                "---- Server Session Id : %d\n"
                                "---- User Session Id   : %d\n",
                                KNL_SESSION_TYPE( sSessEnv ),
                                sServerSessionId,
                                sUserSessionId );
        
        /**
         * dedicate session을 skip한다.
         */
        STL_TRY_THROW( KNL_SESSION_TYPE( sSessEnv ) == KNL_SESSION_TYPE_SHARED,
                       RAMP_FINISH );

        if( gZtsCurrentSllHandle != NULL )
        {
            (void) sclEnqueueResponse( gZtsCurrentSllHandle->mSclHandle.mCommunication.mIpc->mResponseEvent,
                                       &(gZtsCurrentSllHandle->mSclHandle),
                                       SCL_QCMD_SERVER_FAILURE,
                                       0,                   /* CmUnitCount */
                                       SCL_ENV(sMyEnv) );

            KNL_SET_SESSION_ACCESS_KEY( sSessEnv,
                                        KNL_SESSION_ACCESS_LEAVE,
                                        KNL_GET_SESSION_SEQ( sSessEnv ) );

#ifdef STL_DEBUG
            (void) knlSystemFatal( "shared server fatal\n", KNL_ENV(sMyEnv) );
#endif
            knlKillSession( sSessEnv );
        }
        
        /**
         * Session Link 정보를 원복한다.
         */
        KNL_LINK_SESS_ENV( sMyEnv, gZtsSessionEnv );

        if( gZtsSharedMemory != NULL )
        {
            /**
             * Deadlock Detection을 위한 worker 정보를 설정한다.
             */
            if( gZtsJoinGroupWorker == STL_TRUE )
            {
                smlDecRequestGroupWorker( SLL_REQUEST_GROUP_ID( gZtsSharedMemory,
                                                                gZtsSharedServerId ) );
            }
        }
    }
    
    STL_RAMP( RAMP_FINISH );
    
   /**
     * SESSION Layer의 Fatal Handler를 호출한다.
     */
    
    gZtsmOldFatalHandler( aCauseString, aSigInfo, aContext );
}


