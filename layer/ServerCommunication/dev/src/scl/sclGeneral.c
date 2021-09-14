/*******************************************************************************
 * sclGeneral.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: sclGeneral.c 6676 2012-12-13 08:39:46Z egon $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stl.h>
#include <dtl.h>
#include <knl.h>
#include <scl.h>
#include <scb.h>
#include <scDef.h>
#include <sccCommunication.h>

/**
 * @file sclGeneral.c
 * @brief Server Communication Layer General Routines
 */

knlWarmupFunc           gWarmupSC = scbWarmUp;
knlCooldownFunc         gCooldownSC = scbCoolDown;
extern stlFatalHandler  gScbOldFatalHandler;

knlStartupFunc gStartupSC[KNL_STARTUP_PHASE_MAX] =
{
    NULL,
    scbStartupNoMount,
    scbStartupMount,
    NULL,
    scbStartupOpen
};

knlShutdownFunc gShutdownSC[KNL_SHUTDOWN_PHASE_MAX] =
{
    NULL,
    scbShutdownInit,
    scbShutdownDismount,
    NULL,
    scbShutdownClose
};


/**
 * @brief Server Communication Layer를 초기화한다.
 * @remark 프로세스당 한번 호출되어야 한다.
 */
stlStatus sclInitialize()
{
    if( stlGetProcessInitialized( STL_LAYER_SERVER_COMMUNICATION ) == STL_FALSE )
    {
        stlRegisterErrorTable( STL_ERROR_MODULE_SERVER_COMMUNICATION,
                               gServerCommunicationErrorTable );

        /**
         * 하위 Layer 초기화
         */

        STL_TRY( knlInitialize() == STL_SUCCESS );

        /**
         * FATAL Handler 등록
         */

        STL_TRY( stlHookFatalHandler( scbFatalHandler,
                                      & gScbOldFatalHandler )
                 == STL_SUCCESS );

        stlSetProcessInitialized( STL_LAYER_SERVER_COMMUNICATION );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Session의 Process Envrionment를 초기화 한다.
 * @param[out]    aEnv        초기화될 Process Environment 포인터
 * @param[in]     aEnvType    Env Type
 */
stlStatus sclInitializeEnv( sclEnv     * aEnv,
                            knlEnvType   aEnvType )
{
    stlInt32 sState = 0;
    
    STL_TRY( knlInitializeEnv( KNL_ENV(aEnv),
                               aEnvType )
             == STL_SUCCESS );
    sState = 1;

    knlSetTopLayer( STL_LAYER_SERVER_COMMUNICATION, KNL_ENV( aEnv ) );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void) knlFinalizeEnv( KNL_ENV(aEnv) );
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief Process Environemnt 초기화
 * @param[out] aEnv   Environment Pointer
 */
stlStatus sclFinalizeEnv( sclEnv * aEnv )
{
    STL_TRY( knlFinalizeEnv( (knlEnv*)aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Server Communication의 Session Environment를 초기화한다.
 * @param[in] aSessEnv Session의 Session Environment
 * @param[in] aConnectionType Session Connection Type
 * @param[in] aSessType Session Type
 * @param[in] aSessEnvType Session Envrionment Type
 * @param[in] aEnv Session의 Process Environment
 * @remarks
 */
stlStatus sclInitializeSessionEnv( sclSessionEnv     * aSessEnv,
                                   knlConnectionType   aConnectionType,
                                   knlSessionType      aSessType,
                                   knlSessEnvType      aSessEnvType,
                                   sclEnv            * aEnv )
{
    STL_TRY( knlInitializeSessionEnv( (knlSessionEnv*)aSessEnv,
                                      aConnectionType,
                                      aSessType,
                                      aSessEnvType,
                                      KNL_ENV(aEnv) ) == STL_SUCCESS );

    knlSetSessionTopLayer( (knlSessionEnv*)aSessEnv, STL_LAYER_SERVER_COMMUNICATION );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Server Communication의 Session Environment를 종료한다.
 * @param[in] aSessEnv Session의 Session Environment
 * @param[in] aEnv Session의 Process Environment
 * @remarks
 */
stlStatus sclFinalizeSessionEnv( sclSessionEnv * aSessEnv,
                                 sclEnv        * aEnv )
{
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSessEnv != NULL, KNL_ERROR_STACK( aEnv ) );

    /*
     * 하위 Layer 의 Session Env 를 종료
     */

    STL_TRY( knlFinalizeSessionEnv( (knlSessionEnv*)aSessEnv,
                                    KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

