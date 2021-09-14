/*******************************************************************************
 * ztim.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztim.c 12157 2014-04-23 03:50:31Z lym999 $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file ztim.c
 * @brief Gliese gserver Main Routines
 */

#include <stl.h>
#include <cml.h>
#include <knl.h>
#include <scl.h>
#include <sll.h>
#include <ztiDef.h>
#include <ztim.h>
#include <ztis.h>

stlFatalHandler             gZtimOldFatalHandler;

extern ztiDispatcher        gZtimDispatcher;

/**
 * @brief 시스템 Environment
 * @remark startup, shutdown 과 세션 Environment 획득 시 사용된다.
 */

int main( int     aArgc,
          char ** aArgv )
{
    stlGetOpt               sOpt;
    stlChar                 sCh;
    const stlChar         * sOptionArg;
    stlErrorStack           sErrorStack;
    stlInt32                sState = 0;
    stlInt64                sTempInt64;
    stlChar               * sEndPtr;
    stlInt32                sIndex = -1;
    stlBool                 sDaemonize = STL_TRUE;
    sllEnv                * sEnv;
    sllSessionEnv         * sSessionEnv = NULL;
    stlBool                 sEnableAging = STL_FALSE;
    stlInt64                sMsgKey = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    STL_TRY( sllInitialize() == STL_SUCCESS );
    sState = 1;

    STL_TRY( stlHookFatalHandler( ztimFatalHandler,
                                  & gZtimOldFatalHandler )
             == STL_SUCCESS );


    stlRegisterErrorTable( STL_ERROR_MODULE_GLIESE_TOOL_GDISPATCHER,
                           gZtieErrorTable );

    STL_TRY( stlInitGetOption( &sOpt,
                               aArgc,
                               (const stlChar* const *)aArgv,
                               &sErrorStack )
             == STL_SUCCESS );

    while( STL_TRUE )
    {
        if( stlGetOptionLong( &sOpt,
                              gZtimOptOption,
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

    STL_TRY_THROW( sMsgKey != 0, RAMP_ERR_INVALID_OPTION );
    STL_TRY_THROW( sIndex >= 0, RAMP_ERR_INVALID_OPTION );

    STL_TRY( ztimStartup( sDaemonize, &sErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( sslAllocEnv( (sslEnv**)&sEnv,
                          &sErrorStack ) == STL_SUCCESS );
    sState = 3;

    STL_TRY( sllInitializeEnv( sEnv, KNL_ENV_SHARED ) == STL_SUCCESS );
    sState = 4;

    STL_TRY( sslAllocSession( "",
                              (sslSessionEnv**)&sSessionEnv,
                              SSL_ENV(sEnv) )
             == STL_SUCCESS );
    sState = 5;

    STL_TRY( sllInitializeSessionEnv( sSessionEnv,
                                      KNL_CONNECTION_TYPE_DA,
                                      KNL_SESSION_TYPE_DEDICATE,
                                      KNL_SESS_ENV_SHARED,
                                      sEnv )
             == STL_SUCCESS );
    sState = 6;

    KNL_LINK_SESS_ENV( sEnv, sSessionEnv );
    KNL_ENABLE_SESSION_AGING( sSessionEnv );
    sEnableAging = STL_TRUE;

    STL_TRY( ztimRun( &sMsgKey,
                      sIndex,
                      sEnv )
             == STL_SUCCESS );

    sState = 4;
    (void)sllFinalizeSessionEnv( sSessionEnv,
                                 sEnv );

    sState = 2;
    (void)sllFinalizeEnv( sEnv );

    knlDisconnectSession( sSessionEnv );

    sState = 1;
    (void)ztimShutdown( &sErrorStack );

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
            (void)sllFinalizeSessionEnv( sSessionEnv, sEnv );
        case 5:
            knlLogErrorCallStack( NULL, KNL_ENV(sEnv) );
            knlLogMsg( NULL,
                       KNL_ENV(sEnv),
                       KNL_LOG_LEVEL_FATAL,
                       "[DISPATCHER-%d] process abnormally terminated\n",
                       sIndex );

            if( sEnableAging == STL_FALSE )
            {
                (void)sslFreeSession( (void *)sSessionEnv, SSL_ENV(sEnv) );
            }
        case 4:
            stlAppendErrors( &sErrorStack, KNL_ERROR_STACK(sEnv) );
            (void)sllFinalizeEnv( sEnv );
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
            (void)ztimShutdown( &sErrorStack );
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


/**
 * @brief Server Library Layer의 Fatal Handler.
 * @param[in] aCauseString Fatal 유발 원인을 알수 있는 스트링
 * @param[in] aSigInfo Signal Information
 * @param[in] aContext 이전 Context 정보
 */
void ztimFatalHandler( const stlChar * aCauseString,
                       void          * aSigInfo,
                       void          * aContext )
{
    sllEnv          * sMyEnv   = NULL;
    sllSessionEnv   * sMySessionEnv = NULL;
    stlErrorStack     sErrorStack;
    stlInt32          sIdx;
    stlUInt32         sSessionEnvId;
    stlUInt32         sEnvId;
    stlStatus         sRtn;

    STL_INIT_ERROR_STACK( &sErrorStack );

    /**
     * sendThread가 있으면 종료할때까지 기다린다.
     * (critical section 영역에 있지 않도록 함)
     */
    gZtimDispatcher.mExitProcess = STL_TRUE;
    if( gZtimDispatcher.mSendThread.mHandle != 0 )
    {
        (void)stlJoinThread( &gZtimDispatcher.mSendThread, &sRtn, &sErrorStack );
    }


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
                            "[DISPATCHER INFORMATION]\n"
                            "---- Dispatcher Id    : %u\n"
                            "---- Env Id           : %u\n"
                            "---- SessionEnv Id    : %u\n",
                            gZtimDispatcher.mId,
                            sEnvId,
                            sSessionEnvId );

    for( sIdx = 0; sIdx < gZtimDispatcher.mMaxContextCount; sIdx++ )
    {
        if( gZtimDispatcher.mContext[sIdx].mUsed == STL_TRUE )
        {
            if( ZTI_INDEX_UDS == sIdx )
            {
                (void)ztimFinalizeUds( &gZtimDispatcher.mContext[ZTI_INDEX_UDS], sMyEnv );
            }
            else
            {
                STL_TRY_THROW( gZtimDispatcher.mContext[sIdx].mSllHandle != NULL, RAMP_FINISH );

                (void) knlLogMsgUnsafe( NULL,
                                        (knlEnv*) sMyEnv,
                                        KNL_LOG_LEVEL_FATAL,
                                        "[DISPATCHER INFORMATION]\n"
                                        "---- Session Id   : %u\n"
                                        "---- Session Seq  : %ld\n",
                                        gZtimDispatcher.mContext[sIdx].mSllHandle->mSessionId,
                                        gZtimDispatcher.mContext[sIdx].mSllHandle->mSessionSeq );

                /**
                 * mSllHandle이 초기화되고 mUsed == STL_TRUE로 설정되고
                 * mUsed == STL_FALSE로 설정되고 mSllHandle이 종료됨으로
                 * 여기의 mSllHandle는 초기화 되고 종료되지 않은 상태임.
                 */
                (void) sllKillSession( gZtimDispatcher.mContext[sIdx].mSllHandle, sMyEnv );
            }
        }
    }

    STL_RAMP( RAMP_FINISH );


    STL_FINISH;

   /**
     * SESSION Layer의 Fatal Handler를 호출한다.
     */
    gZtimOldFatalHandler( aCauseString, aSigInfo, aContext );
}

