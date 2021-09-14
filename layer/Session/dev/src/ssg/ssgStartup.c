/*******************************************************************************
 * ssgStartup.c
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
 * @file ssgStartup.c
 * @brief Session Layer Startup Internal Routines
 */


#include <qll.h>
#include <sslDef.h>
#include <ssDef.h>
#include <ssx.h>
#include <ssgStartup.h>

extern knlFxTableDesc gSessionTableDesc;
extern knlFxTableDesc gSsSessStatTableDesc;
extern knlFxTableDesc gSsStmtStatTableDesc;
extern knlFxTableDesc gSsStmtTableDesc;
extern knlFxTableDesc gSsxGlobalTransTableDesc;

stlFatalHandler   gSsgOldFatalHandler;
ssgWarmupEntry  * gSsgWarmupEntry;

/**
 * @addtogroup ssgStartup
 * @{
 */

/**
 * @brief Session Layer를 No-Mount 상태로 전환시킨다.
 * @param[in] aStartupInfo Startup 시 정보 (Recovery type(restart or media), Tablespace Id)
 * @param[in,out] aEnv Environment 구조체
 * @remarks
 */
stlStatus ssgStartupNoMount( knlStartupInfo * aStartupInfo,
                             void           * aEnv )
{
    ssgWarmupEntry ** sWarmupEntry;

    STL_ASSERT( aEnv != NULL );

    STL_TRY( knlGetEntryPoint( STL_LAYER_SESSION,
                               (void **) & sWarmupEntry,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlAllocFixedStaticArea( STL_SIZEOF( ssgWarmupEntry ),
                                      (void**)&gSsgWarmupEntry,
                                      KNL_ENV(aEnv) ) == STL_SUCCESS );

    *sWarmupEntry = gSsgWarmupEntry;
    
    STL_TRY( knlInitLatch( &(gSsgWarmupEntry->mContextLatch),
                           STL_TRUE,
                           KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_RING_INIT_HEADLINK( &(gSsgWarmupEntry->mContextArrList),
                            STL_OFFSETOF( ssxContextArray, mContextArrayLink ) );
    STL_RING_INIT_HEADLINK( &(gSsgWarmupEntry->mContextFreeList),
                            STL_OFFSETOF( knlXaContext, mFreeLink ) );
    

    STL_TRY( knlCreateDynamicMem( &gSsgWarmupEntry->mShmMem,
                                  NULL, /* aParentMem */
                                  KNL_ALLOCATOR_SESSION_SHARED_MEMORY,
                                  KNL_MEM_STORAGE_TYPE_SHM,
                                  SSG_DATABASE_SHMMEM_CHUNK_SIZE,
                                  SSG_DATABASE_SHMMEM_CHUNK_SIZE,
                                  NULL, /* aMemController */
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ssxAllocContextArray( SSL_ENV(aEnv) ) == STL_SUCCESS );

    /**
     * create logger
     */
    
    STL_TRY( knlCreateLogger( &gSsgWarmupEntry->mXaLogger,
                              SSX_XA_LOGGER_NAME,
                              SSX_XA_LOGGER_MAXSIZE,
                              KNL_LOG_LEVEL_FATAL,
                              KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    STL_TRY( knlCreateLogger( &gSsgWarmupEntry->mLoginLogger,
                              SSS_LOGIN_LOGGER_NAME,
                              SSS_LOGIN_LOGGER_MAXSIZE,
                              KNL_LOG_LEVEL_FATAL,
                              KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    /**
     * Fixed Table 등록
     */

    STL_TRY( ssgRegisterFxTables( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Session Layer를 Mount 상태로 전환시킨다.
 * @param[in] aStartupInfo Startup 시 정보 (Recovery type(restart or media), Tablespace Id)
 * @param[in,out] aEnv Environment 구조체
 * @remarks
 */
stlStatus ssgStartupMount( knlStartupInfo * aStartupInfo,
                           void           * aEnv )
{
    return STL_SUCCESS;
}


/**
 * @brief Session Layer를 Open 상태로 전환시킨다.
 * @param[in] aStartupInfo Startup 시 정보 (Recovery type(restart or media), Tablespace Id)
 * @param[in,out] aEnv Environment 구조체
 * @remarks
 */
stlStatus ssgStartupOpen( knlStartupInfo * aStartupInfo,
                          void           * aEnv )
{
    smlTransId      sTransId = SML_INVALID_TRANSID;
    stlXid          sXid;
    knlXaContext  * sXaContext;
    stlBool         sIsSuccess;

    STL_TRY( smlFetchPreparedTransaction( &sTransId,
                                          STL_TRUE, /* aRecoverLock */
                                          &sXid,
                                          SML_ENV(aEnv) )
             == STL_SUCCESS );
    
    while( sTransId != SML_INVALID_TRANSID )
    {
        STL_TRY( ssxCreateContext( 0,    /* aSessionId */
                                   &sXid,
                                   &sXaContext,
                                   SSL_ENV(aEnv) )
                 == STL_SUCCESS );

        sXaContext->mState = KNL_XA_STATE_PREPARED;
        sXaContext->mTransId = sTransId;
        sXaContext->mAssociateState = KNL_XA_ASSOCIATE_STATE_FALSE;
       
        STL_TRY( knlLinkXaContext( sXaContext,
                                   &sIsSuccess,
                                   KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        STL_DASSERT( sIsSuccess == STL_TRUE );
        
        STL_TRY( smlFetchPreparedTransaction( &sTransId,
                                              STL_TRUE, /* aRecoverLock */
                                              &sXid,
                                              aEnv )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Session Layer를 Warm-up 단계로 전환시킨다.
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus ssgWarmup( void * aEnv )
{
    ssgWarmupEntry ** sWarmupEntry;
    
    STL_TRY( knlGetEntryPoint( STL_LAYER_SESSION,
                               (void **) & sWarmupEntry,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Fixed Table 등록
     */

    STL_TRY( ssgRegisterFxTables( aEnv ) == STL_SUCCESS );
    
    gSsgWarmupEntry = *sWarmupEntry;

    return STL_SUCCESS;

    STL_FINISH;

    knlLogMsgUnsafe( NULL,
                     KNL_ENV( aEnv ),
                     KNL_LOG_LEVEL_FATAL,
                     "[SESSION WARMUP FAIL] %s\n",
                     stlGetLastErrorMessage( KNL_ERROR_STACK( aEnv ) ) );
    
    return STL_FAILURE;
}

/**
 * @brief Session Layer를 Cool-down 시킨다.
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus ssgCooldown( void * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief Session Layer를 INIT 시킨다.
 * @param[in] aShutdownInfo 종료 시 정보
 * @param[in,out] aEnv Environment 구조체
 * @remarks
 */
stlStatus ssgShutdownInit( knlShutdownInfo * aShutdownInfo,
                           void            * aEnv )
{
    sslEnv * sEnv = (sslEnv *) aEnv;

    STL_ASSERT( aEnv != NULL );

    STL_TRY_THROW( aShutdownInfo->mShutdownMode > KNL_SHUTDOWN_MODE_ABORT,
                   RAMP_SHUTDOWN_ABORT );

    STL_TRY( knlDestroyLogger( &gSsgWarmupEntry->mXaLogger,
                               KNL_ENV( sEnv ) ) == STL_SUCCESS );

    STL_TRY( knlDestroyLogger( &gSsgWarmupEntry->mLoginLogger,
                               KNL_ENV( sEnv ) ) == STL_SUCCESS );

    STL_TRY( knlDestroyDynamicMem( &gSsgWarmupEntry->mShmMem,
                                   KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( sEnv != NULL );
    
    STL_RAMP( RAMP_SHUTDOWN_ABORT );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Session Layer를 DISMOUNT 시킨다.
 * @param[in] aShutdownInfo 종료 시 정보
 * @param[in,out] aEnv Environment 구조체
 * @remarks
 */
stlStatus ssgShutdownDismount( knlShutdownInfo * aShutdownInfo,
                               void            * aEnv )
{
    STL_ASSERT( aEnv != NULL );
    
    STL_TRY_THROW( aShutdownInfo->mShutdownMode > KNL_SHUTDOWN_MODE_ABORT,
                   RAMP_SHUTDOWN_ABORT );

    STL_RAMP( RAMP_SHUTDOWN_ABORT );

    return STL_SUCCESS;
}

/**
 * @brief Session Layer를 CLOSE 시킨다.
 * @param[in] aShutdownInfo 종료 시 정보
 * @param[in,out] aEnv Environment 구조체
 * @remarks
 */
stlStatus ssgShutdownClose( knlShutdownInfo * aShutdownInfo,
                            void            * aEnv )
{
    STL_ASSERT( aEnv != NULL );

    STL_TRY_THROW( aShutdownInfo->mShutdownMode > KNL_SHUTDOWN_MODE_ABORT,
                   RAMP_SHUTDOWN_ABORT );

    STL_RAMP( RAMP_SHUTDOWN_ABORT );

    return STL_SUCCESS;
}



/**
 * @brief Session Layer의 Fatal Handler.
 * @param[in] aCauseString Fatal 유발 원인을 알수 있는 스트링
 * @param[in] aSigInfo Signal Information
 * @param[in] aContext 이전 Context 정보
 */
void ssgFatalHandler( const stlChar * aCauseString,
                      void          * aSigInfo,
                      void          * aContext )
{
    sslEnv          * sMyEnv   = NULL;
    sslSessionEnv   * sSessEnv = NULL;
    qllStatement    * sSQLStmt = NULL;
    stlChar         * sSQLText = NULL;
    stlUInt32         sEnvId = -1;
    stlUInt32         sSessionEnvId = -1;
    
    sMyEnv = (sslEnv*) knlGetMyEnv();

    STL_TRY_THROW( sMyEnv != NULL, RAMP_FINISH );
    STL_TRY_THROW( knlGetTopLayer(KNL_ENV(sMyEnv))
                   >= STL_LAYER_SESSION,
                   RAMP_FINISH );
    STL_TRY_THROW( knlGetEnvId( sMyEnv, &sEnvId ) == STL_SUCCESS,
                   RAMP_FINISH );

    if( KNL_GET_FATAL_HANDLING( sMyEnv, STL_LAYER_SESSION ) == STL_FALSE )
    {
        /**
         * 무한 Fatal을 방지해야 한다.
         */
        
        KNL_SET_FATAL_HANDLING( sMyEnv, STL_LAYER_SESSION );
    
        sSessEnv = SSL_SESS_ENV( sMyEnv );

        STL_TRY_THROW( sSessEnv != NULL, RAMP_SKIP_FATAL );
        
        STL_TRY_THROW( knlGetSessionEnvId( sSessEnv,
                                           &sSessionEnvId,
                                           KNL_ENV(sMyEnv) )
                       == STL_SUCCESS, RAMP_FINISH );
        
        /**
         * 가장 최근 수행된 SQL Statement 정보 
         */
        
        if ( sSessEnv->mLatestStatement != NULL )
        {
            sSQLStmt = &sSessEnv->mLatestStatement->mSqlStmt;
            sSQLText = qllGetSQLString( sSQLStmt );

            if ( sSQLText != NULL )
            {
                /**
                 * Fatal 당시의 정보를 남긴다.
                 */
                (void) knlLogMsgUnsafe( NULL,
                                        (knlEnv*) sMyEnv,
                                        KNL_LOG_LEVEL_FATAL,
                                        "[SESSION INFORMATION]\n"
                                        "---- Process env id : %d\n"
                                        "---- Session id     : %d\n"
                                        "---- Program        : %s\n"
                                        "---- Latest SQL text: %s\n",
                                        sEnvId,
                                        sSessionEnvId,
                                        sSessEnv->mProgram,
                                        sSQLText );
            }
            else
            {
                (void) knlLogMsgUnsafe( NULL,
                                        (knlEnv*) sMyEnv,
                                        KNL_LOG_LEVEL_FATAL,
                                        "[SESSION INFORMATION]\n"
                                        "---- Process env id : %d\n"
                                        "---- Session id     : %d\n"
                                        "---- Program        : %s\n"
                                        "---- Latest SQL text: N/A\n",
                                        sEnvId,
                                        sSessionEnvId,
                                        sSessEnv->mProgram );
            }
        }
        else
        {
            (void) knlLogMsgUnsafe( NULL,
                                    (knlEnv*) sMyEnv,
                                    KNL_LOG_LEVEL_FATAL,
                                    "[SESSION INFORMATION]\n"
                                    "---- Process env id : %d\n"
                                    "---- Session id     : %d\n"
                                    "---- Program        : %s\n"
                                    "---- Latest SQL text: N/A\n",
                                    sEnvId,
                                    sSessionEnvId,
                                    sSessEnv->mProgram );
        }
    }

    STL_RAMP( RAMP_FINISH );
    
    /**
     * SQL Processor Layer의 Fatal Handler를 호출한다.
     */
    
    gSsgOldFatalHandler( aCauseString, aSigInfo, aContext );

    STL_RAMP( RAMP_SKIP_FATAL );
}


/**
 * @brief Fixed Table 들을 등록함
 * @param[in]  aEnv   Environment
 * @remarks
 * NO_MOUNT 단계와 Warm-Up 단계에서 동일한 순서로 등록되어야 함.
 */
stlStatus ssgRegisterFxTables( void * aEnv )
{
    /* X$SESSION Fixed Table을 등록한다 */
    STL_TRY( knlRegisterFxTable( &gSessionTableDesc, aEnv ) == STL_SUCCESS );

    /* X$SS_SESS_STAT Fixed Table을 등록한다 */
    STL_TRY( knlRegisterFxTable( &gSsSessStatTableDesc, aEnv ) == STL_SUCCESS );

    /* X$SS_STMT_STAT Fixed Table을 등록한다 */
    STL_TRY( knlRegisterFxTable( &gSsStmtStatTableDesc, aEnv ) == STL_SUCCESS );

    /* X$STATEMENT Fixed Table을 등록한다 */
    STL_TRY( knlRegisterFxTable( &gSsStmtTableDesc, aEnv ) == STL_SUCCESS );

    /* X$GLOBAL_TRANSACTION Fixed Table을 등록한다 */
    STL_TRY( knlRegisterFxTable( &gSsxGlobalTransTableDesc, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
/** @} ssgStartup */
