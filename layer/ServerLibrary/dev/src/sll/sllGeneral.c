/*******************************************************************************
 * sllGeneral.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: sllGeneral.c 9468 2013-08-28 05:51:24Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file sllGeneral.c
 * @brief Server Library Startup Routines
 */

#include <stl.h>
#include <cml.h>
#include <scl.h>
#include <ssl.h>
#include <sll.h>
#include <slDef.h>
#include <slb.h>
#include <slp.h>

knlWarmupFunc           gWarmupSL = slbWarmUp;
knlCooldownFunc         gCooldownSL = slbCoolDown;
extern stlFatalHandler  gSlbOldFatalHandler;

knlStartupFunc gStartupSL[KNL_STARTUP_PHASE_MAX] =
{
    NULL,
    slbStartupNoMount,
    slbStartupMount,
    NULL,
    slbStartupOpen
};

knlShutdownFunc gShutdownSL[KNL_SHUTDOWN_PHASE_MAX] =
{
    NULL,
    slbShutdownInit,
    slbShutdownDismount,
    NULL,
    slbShutdownClose
};


/**
 * @brief Server Library Layer를 초기화한다.
 * @remark 프로세스당 한번 호출되어야 한다.
 */
stlStatus sllInitialize()
{
    if( stlGetProcessInitialized( STL_LAYER_SERVER_LIBRARY ) == STL_FALSE )
    {
        stlRegisterErrorTable( STL_ERROR_MODULE_SERVER_LIBRARY,
                               gServerLibraryErrorTable );

        /**
         * 하위 Layer 초기화
         */

        STL_TRY( sslInitialize() == STL_SUCCESS );

        /**
         * FATAL Handler 등록
         */
    
        STL_TRY( stlHookFatalHandler( slbFatalHandler,
                                      & gSlbOldFatalHandler )
                 == STL_SUCCESS );
    
        stlSetProcessInitialized( STL_LAYER_SERVER_LIBRARY );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Server Library Layer를 종료한다.
 */
stlStatus sllTerminate()
{
    /* nothing to do */
    return STL_SUCCESS;
}

/**
 * @brief Session의 Process Envrionment를 초기화 한다.
 * @param[out]    aEnv        초기화될 Process Environment 포인터
 * @param[in]     aEnvType    Env Type
 */
stlStatus sllInitializeEnv( sllEnv     * aEnv,
                            knlEnvType   aEnvType )
{
    stlInt32 sState = 0;
    
    STL_TRY( sslInitializeEnv( SSL_ENV(aEnv),
                               aEnvType )
             == STL_SUCCESS );
    sState = 1;

    knlSetTopLayer( STL_LAYER_SERVER_LIBRARY, KNL_ENV( aEnv ) );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void) sslFinalizeEnv( SSL_ENV(aEnv) );
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief Process Environemnt 초기화 
 * @param[out] aEnv   Environment Pointer
 */
stlStatus sllFinalizeEnv( sllEnv * aEnv )
{
    STL_TRY( sslFinalizeEnv( (sslEnv*)aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Server Library의 Session Environment를 초기화한다.
 * @param[in] aSessEnv Session의 Session Environment
 * @param[in] aConnectionType Session Connection Type
 * @param[in] aSessType Session Type
 * @param[in] aSessEnvType Session Envrionment Type
 * @param[in] aEnv Session의 Process Environment
 * @remarks
 */
stlStatus sllInitializeSessionEnv( sllSessionEnv     * aSessEnv,
                                   knlConnectionType   aConnectionType,
                                   knlSessionType      aSessType,
                                   knlSessEnvType      aSessEnvType,
                                   sllEnv            * aEnv )
{
    stlInt32 sState = 0;
    
    STL_TRY( sslInitSessionEnv( (sslSessionEnv*)aSessEnv,
                                aConnectionType,
                                aSessType,
                                aSessEnvType,
                                SSL_ENV(aEnv) ) == STL_SUCCESS );
    sState = 1;
    
    knlSetSessionTopLayer( (knlSessionEnv*)aSessEnv, STL_LAYER_SERVER_LIBRARY );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void) sslFiniSessionEnv( (sslSessionEnv*)aSessEnv,
                                      SSL_ENV(aEnv) );
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief Server Library의 Session Environment를 종료한다.
 * @param[in] aSessEnv Session의 Session Environment
 * @param[in] aEnv Session의 Process Environment
 * @remarks
 */
stlStatus sllFinalizeSessionEnv( sllSessionEnv * aSessEnv,
                                 sllEnv        * aEnv )
{
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSessEnv != NULL, KNL_ERROR_STACK( aEnv ) );

    /*
     * 하위 Layer 의 Session Env 를 종료
     */

    STL_TRY( sslFiniSessionEnv( (sslSessionEnv*)aSessEnv,
                                SSL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief session에 대해서 check timeout을 한다.
 * @param[in] aHandle   Communication Handle
 * @param[in] aEnv      Environment Pointer
 * @note 해당 함수는 세션레벨의 직렬 접근만을 보장한다.
 */
stlStatus sllCheckTimeout( sllHandle  * aHandle,
                           sllEnv     * aEnv )
{
    knlSessionEnv * sSessionEnv;
    
    STL_DASSERT( aHandle->mSessionId != SSL_INVALID_SESSION_ID );

    sSessionEnv = knlGetSessionEnv( aHandle->mSessionId );

    /**
     * 이미 죽은 세션은 error 처리 한다.
     */
    STL_TRY_THROW( aHandle->mSessionSeq == KNL_GET_SESSION_SEQ(sSessionEnv), RAMP_ERR_KILLED_SESSION );

    STL_TRY( knlCheckIdleTimeout( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_KILLED_SESSION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SLL_ERRCODE_SESSION_KILLED,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief kill session을 한다.
 * @param[in] aHandle   Communication Handle
 * @param[in] aEnv      Environment Pointer
 * @remark disconnect command없이 socket이 끊어졌을때 호출된다.
 * @note 해당 함수는 세션레벨의 직렬 접근만을 보장한다.
 */
stlStatus sllKillSession( sllHandle   * aHandle,
                          sllEnv      * aEnv )
{
    knlSessionEnv * sSessionEnv;

    if( aHandle->mSessionId != SSL_INVALID_SESSION_ID )
    {
        sSessionEnv = knlGetSessionEnv( aHandle->mSessionId );

        /**
         * 이미 죽은 세션은 무시한다.
         */
        if( aHandle->mSessionSeq == KNL_GET_SESSION_SEQ(sSessionEnv) )
        {
            knlKillSession( (void*)sSessionEnv );
        }
    }

    return STL_SUCCESS;
}

/**
 * @brief warmup entry에서 shared memory를 가져온다.
 * @param[out] aSharedMemory    shared memory
 * @param[in] aErrorStack       Error Stack Pointer
 * @remark sslDisconnect 내부에서 Finalize 되던 Env 를 대체 한다
 */
stlStatus sllGetSharedMemory( sllSharedMemory  ** aSharedMemory,
                              stlErrorStack     * aErrorStack )
{
    STL_DASSERT( gSlSharedMemory != NULL );
    
    *aSharedMemory = gSlSharedMemory;

    return STL_SUCCESS;
}


/**
 * @brief sllHandle로 부터 Request Group에 Wait Edge를 추가한다.
 * @param[in]  aHandle           Handle Pointer
 * @param[in]  aRequestGroupId   Request Group Identifier
 * @param[in]  aErrorStack       Error Stack Pointer
 */
stlStatus sllAddEdgeIntoWaitGraph( sllHandle      * aHandle,
                                   stlInt32         aRequestGroupId,
                                   stlErrorStack  * aErrorStack )
{
    sslSessionEnv * sSessionEnv;
    
    STL_TRY_THROW( aHandle->mSessionId != SSL_INVALID_SESSION_ID, RAMP_SKIP );
    
    STL_TRY( sslEnterSession( aHandle->mSessionId,
                              aHandle->mSessionSeq,
                              aErrorStack ) == STL_SUCCESS );

    sSessionEnv = knlGetSessionEnv( aHandle->mSessionId );
    
    if( sSessionEnv->mTransId != SML_INVALID_TRANSID )
    {
        smlAddEdgeIntoWaitGraph( sSessionEnv->mTransId, aRequestGroupId );
    }
    
    /**
     * leave session
     */
    sslLeaveSession( aHandle->mSessionId, aHandle->mSessionSeq );

    STL_RAMP( RAMP_SKIP );

    return STL_SUCCESS;

    STL_FINISH;

    stlPopError( aErrorStack );

    return STL_FAILURE;
}


/**
 * @brief sllHandle로 부터 Request Group에 Wait Edge를 삭제한다.
 * @param[in]  aHandle           Handle Pointer
 * @param[in]  aRequestGroupId   Request Group Identifier
 * @param[in]  aErrorStack       Error Stack Pointer
 */
stlStatus sllRemoveEdgeFromWaitGraph( sllHandle      * aHandle,
                                      stlInt32         aRequestGroupId,
                                      stlErrorStack  * aErrorStack )
{
    sslSessionEnv * sSessionEnv;
    
    STL_TRY_THROW( aHandle->mSessionId != SSL_INVALID_SESSION_ID, RAMP_SKIP );
    
    STL_TRY( sslEnterSession( aHandle->mSessionId,
                              aHandle->mSessionSeq,
                              aErrorStack ) == STL_SUCCESS );

    sSessionEnv = knlGetSessionEnv( aHandle->mSessionId );
    
    if( sSessionEnv->mTransId != SML_INVALID_TRANSID )
    {
        smlRemoveEdgeFromWaitGraph( sSessionEnv->mTransId, aRequestGroupId );
    }
    
    /**
     * leave session
     */
    sslLeaveSession( aHandle->mSessionId, aHandle->mSessionSeq );

    STL_RAMP( RAMP_SKIP );

    return STL_SUCCESS;

    STL_FINISH;

    stlPopError( aErrorStack );

    return STL_FAILURE;
}

stlStatus sllCmdStartUp( sllHandle      * aHandle,
                         sllEnv         * aEnv )
{
    sllCommandArgs         sCommandArgs;

    SLL_MARSHALLING_COMMAND_ARGS( &sCommandArgs, aHandle, NULL );

    return slpCmdStartUp( &sCommandArgs, aEnv );

}


