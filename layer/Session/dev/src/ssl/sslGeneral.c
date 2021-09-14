/*******************************************************************************
 * sslGeneral.c
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
 * @file sslGeneral.c
 * @brief Session Startup Routines
 */

#include <qll.h>
#include <sslDef.h>
#include <ssgStartup.h>

extern stlErrorRecord gSessionErrorTable[SML_MAX_ERROR + 1];
extern stlFatalHandler gSsgOldFatalHandler;

knlStartupFunc gStartupSS[KNL_STARTUP_PHASE_MAX] = 
{
    NULL,
    ssgStartupNoMount,
    ssgStartupMount,
    NULL,
    ssgStartupOpen
};

knlWarmupFunc gWarmupSS = ssgWarmup;

knlShutdownFunc gShutdownSS[KNL_SHUTDOWN_PHASE_MAX] =
{
    NULL,
    ssgShutdownInit,
    ssgShutdownDismount,
    NULL,
    ssgShutdownClose
};

knlCooldownFunc    gCooldownSS = ssgCooldown;

/**
 * @brief Session을 초기화한다.
 * @remark 프로세스당 한번 호출되어야 한다.
 */
stlStatus sslInitialize()
{
    if( stlGetProcessInitialized( STL_LAYER_SESSION ) == STL_FALSE )
    {
        stlRegisterErrorTable( STL_ERROR_MODULE_SESSION,
                               gSessionErrorTable);

        /**
         * 하위 Layer 초기화
         */
    
        STL_TRY( qllInitialize() == STL_SUCCESS );

        /**
         * FATAL Handler 등록
         */
    
        STL_TRY( stlHookFatalHandler( ssgFatalHandler,
                                      & gSsgOldFatalHandler )
                 == STL_SUCCESS );
    
        stlSetProcessInitialized( STL_LAYER_SESSION );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Session Layer를 종료한다.
 */
stlStatus sslTerminate()
{
    /* nothing to do */
    return STL_SUCCESS;
}

