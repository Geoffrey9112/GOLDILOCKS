/*******************************************************************************
 * sslSession.c
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
 * @file sslSession.c
 * @brief Session Layer Session Environment Routines
 */

#include <qll.h>
#include <sslDef.h>
#include <sslSession.h>
#include <ssDef.h>

/**
 * @addtogroup sslSessEnv
 * @{
 */

stlStatus sslAllocSession( stlChar        * aRoleName,
                           sslSessionEnv ** aSessionEnv,
                           sslEnv         * aEnv )
{
    STL_PARAM_VALIDATE( aSessionEnv != NULL, KNL_ERROR_STACK( aEnv ) );

    if( stlStrcmp( aRoleName, QLL_ROLE_STRING_ADMIN ) == 0 ) 
    {
        STL_TRY( knlAllocAdminSessionEnv( (void**)aSessionEnv,
                                          KNL_ENV( aEnv ) ) == STL_SUCCESS );
    }
    else if( stlStrcmp( aRoleName, QLL_ROLE_STRING_SYSDBA ) == 0 ) 
    {
        STL_TRY( knlAllocSessionEnvUnsafe( (void**)aSessionEnv,
                                               KNL_ENV( aEnv ) ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( knlAllocSessionEnv( (void**)aSessionEnv,
                                     KNL_ENV( aEnv ) ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus sslFreeSession( sslSessionEnv * aSessionEnv,
                          sslEnv        * aEnv )
{
    STL_PARAM_VALIDATE( aSessionEnv != NULL, KNL_ERROR_STACK( aEnv ) );

    STL_TRY( knlFreeSessionEnv( (void*)aSessionEnv,
                                KNL_ENV( aEnv ) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
}

/**
 * @brief Session의 Session Environment를 초기화한다.
 * @param[in] aSessEnv Session의 Session Environment
 * @param[in] aConnectionType Session Connection Type
 * @param[in] aSessType Session Type
 * @param[in] aSessEnvType Session Envrionment Type
 * @param[in] aEnv Session의 Process Environment
 * @remarks
 */
stlStatus sslInitSessionEnv( sslSessionEnv     * aSessEnv,
                             knlConnectionType   aConnectionType,
                             knlSessionType      aSessType,
                             knlSessEnvType      aSessEnvType,
                             sslEnv            * aEnv )
{
    stlUInt32 sMemType = 0;
    stlInt32  sState = 0;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSessEnv != NULL, KNL_ERROR_STACK( aEnv ) );

    /* admin session이고 shared 모드이면 error */
    STL_TRY_THROW( aSessType != KNL_SESSION_TYPE_SHARED ||
                   knlIsAdminSession( aSessEnv ) == STL_FALSE,
                   RAMP_ERR_ADMIN_MUST_DEDICATED );

    /*
     * 하위 Layer 의 Session Env 를 초기화
     */
    
    STL_TRY( qllInitSessionEnv( (qllSessionEnv *)aSessEnv,
                                aConnectionType,
                                aSessType,
                                aSessEnvType,
                                QLL_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 1;

    switch (aSessEnvType)
    {
        case KNL_SESS_ENV_HEAP:
            sMemType = KNL_MEM_STORAGE_TYPE_HEAP;
            break;
        case KNL_SESS_ENV_SHARED:
            /**
             * admin session은 shared memory를 사용하지 않는다.
             * - static memory가 부족한 상황에서도 세션을 사용할수
             *   있어야 한다.
             */
            if( knlIsAdminSession( aSessEnv ) == STL_TRUE )
            {
                sMemType = KNL_MEM_STORAGE_TYPE_HEAP;
            }
            else
            {
                sMemType = KNL_MEM_STORAGE_TYPE_SHM;
            }
            break;
        default:
            STL_PARAM_VALIDATE( 1 == 0, QLL_ERROR_STACK(aEnv) );
            break;
    }

    STL_TRY( knlCreateDynamicMem( &aSessEnv->mMemStatement,
                                  NULL, /* aParentMem */
                                  KNL_ALLOCATOR_SESSION_DYNAMIC_STATEMENT,
                                  sMemType,
                                  SSL_SESSION_MEM_INIT_SIZE,
                                  SSL_SESSION_MEM_NEXT_SIZE,
                                  NULL, /* aMemController */
                                  KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 2;

    aSessEnv->mClientProcId.mProcID = -1;
    aSessEnv->mProgram[0]           = '\0';
    aSessEnv->mTransId              = SML_INVALID_TRANSID;
    aSessEnv->mStatementSeq         = 0;
    aSessEnv->mDefaultAccessMode    = QLL_ACCESS_MODE_READ_WRITE;
    aSessEnv->mNextAccessMode       = QLL_ACCESS_MODE_NONE;
    aSessEnv->mDefaultTxnIsolation  = SSL_ISOLATION_LEVEL_READ_COMMITTED;
    aSessEnv->mCurrTxnIsolation     = SSL_ISOLATION_LEVEL_READ_COMMITTED;
    aSessEnv->mNextTxnIsolation     = SSL_ISOLATION_LEVEL_NONE;
    aSessEnv->mDefaultUniqueMode    = QLL_UNIQUE_MODE_DEFAULT;
    aSessEnv->mNextUniqueMode       = QLL_UNIQUE_MODE_NONE;
    aSessEnv->mActiveStmtCount      = 0;
    aSessEnv->mNextStmtSlot         = 0;
    aSessEnv->mHasWithoutHoldCursor = STL_FALSE;
    aSessEnv->mLatestStatement      = NULL;
    aSessEnv->mStatementSlot        = NULL;
    aSessEnv->mXaContext            = NULL;

    knlSetSessionTopLayer( (knlSessionEnv*)aSessEnv, STL_LAYER_SESSION );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_ADMIN_MUST_DEDICATED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_ADMIN_MUST_DEDICATED,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    switch (sState)
    {
        case 2: 
            (void) knlDestroyDynamicMem( & aSessEnv->mMemStatement,
                                         KNL_ENV(aEnv) );
        case 1:
            (void) qllFiniSessionEnv( (qllSessionEnv *) aSessEnv,
                                      QLL_ENV(aEnv) );
        default:
            break;
    }
    
    return STL_FAILURE;
}

stlStatus sslInitSessionStmtSlot( sslSessionEnv * aSessEnv,
                                  sslEnv        * aEnv )
{
    /*
     * statement array 생성
     */
    aSessEnv->mMaxStmtCount =
        knlGetPropertyBigIntValueByID( KNL_PROPERTY_MAXIMUM_CONCURRENT_ACTIVITIES,
                                       KNL_ENV(aEnv) );

    STL_TRY( knlAllocDynamicMem( &aSessEnv->mMemStatement,
                                 STL_SIZEOF( sslStatement* ) * aSessEnv->mMaxStmtCount,
                                 (void**)&aSessEnv->mStatementSlot,
                                 KNL_ENV( aEnv ) ) == STL_SUCCESS );

    stlMemset( (void*)aSessEnv->mStatementSlot,
               0x00,
               STL_SIZEOF( sslStatement* ) * aSessEnv->mMaxStmtCount );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Ager Cleanup 연산을 위해서 Session Envrionment를 정리한다.
 * @param[in]     aSessEnv  Session Environment 포인터
 * @param[in,out] aEnv      Environment 포인터
 */
stlStatus sslCleanupSessionEnv( sslSessionEnv  * aSessEnv,
                                sslEnv         * aEnv )
{
    sslStatement * sStatement;
    stlInt64       sActiveStatement = 0;
    stlInt64       i;
    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSessEnv != NULL, KNL_ERROR_STACK( aEnv ) );

    /**
     * MemStatement가 공유메모리에 할당되어 있다면 Cleanup한다.
     */
    if( (KNL_DYNAMIC_MEMORY_IS_CREATED( &aSessEnv->mMemStatement ) == STL_TRUE) &&
        (KNL_DYNAMIC_MEMORY_TYPE( &aSessEnv->mMemStatement ) == KNL_MEM_STORAGE_TYPE_SHM) )
    {
        /**
         * Active statement를 정리한다.
         */

        if( aSessEnv->mStatementSlot != NULL )
        {
            for( i = 0; i < aSessEnv->mMaxStmtCount; i++ )
            {
                sStatement = aSessEnv->mStatementSlot[i];

                if( sStatement != NULL )
                {
                    sActiveStatement++;

                    if( KNL_REGION_MEMORY_TYPE( &(sStatement->mParamRegionMem) ) == KNL_MEM_STORAGE_TYPE_SHM )
                    {
                        STL_TRY( knlDestroyRegionMem( &(sStatement->mParamRegionMem),
                                                      KNL_ENV( aEnv ) )
                                 == STL_SUCCESS );
                    }
                
                    if( KNL_REGION_MEMORY_TYPE( &(sStatement->mColumnRegionMem) ) == KNL_MEM_STORAGE_TYPE_SHM )
                    {
                        STL_TRY( knlDestroyRegionMem( &(sStatement->mColumnRegionMem),
                                                      KNL_ENV( aEnv ) )
                                 == STL_SUCCESS );
                    }
                
                    if( aSessEnv->mActiveStmtCount <= sActiveStatement )
                    {
                        break;
                    }
                }
            }
        }

        STL_TRY( knlDestroyDynamicMem( & aSessEnv->mMemStatement,
                                       KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Session의 Session Environment를 종료한다.
 * @param[in] aSessEnv Session의 Session Environment
 * @param[in] aEnv Session의 Process Environment
 * @remarks
 */
stlStatus sslFiniSessionEnv( sslSessionEnv * aSessEnv,
                             sslEnv        * aEnv )
{
    stlInt32 sState = 2;
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSessEnv != NULL, KNL_ERROR_STACK( aEnv ) );

    STL_DASSERT( aSessEnv->mActiveStmtCount == 0 );

    knlSetSessionConnectionType( (knlSessionEnv*)aSessEnv, KNL_CONNECTION_TYPE_NONE );

    sState = 1;
    if( aSessEnv->mStatementSlot != NULL )
    {
        STL_TRY( knlFreeDynamicMem( &aSessEnv->mMemStatement,
                                    aSessEnv->mStatementSlot,
                                    KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
        aSessEnv->mStatementSlot = NULL;
    }
    
    aSessEnv->mClientProcId.mProcID = -1;
    aSessEnv->mProgram[0]           = '\0';
    aSessEnv->mTransId              = SML_INVALID_TRANSID;
    aSessEnv->mStatementSeq         = 0;
    aSessEnv->mDefaultAccessMode    = QLL_ACCESS_MODE_READ_WRITE;
    aSessEnv->mNextAccessMode       = QLL_ACCESS_MODE_NONE;
    aSessEnv->mDefaultTxnIsolation  = SSL_ISOLATION_LEVEL_READ_COMMITTED;
    aSessEnv->mCurrTxnIsolation     = SSL_ISOLATION_LEVEL_READ_COMMITTED;
    aSessEnv->mNextTxnIsolation     = SSL_ISOLATION_LEVEL_NONE;
    aSessEnv->mMaxStmtCount         = 0;
    aSessEnv->mActiveStmtCount      = 0;
    aSessEnv->mNextStmtSlot         = 0;
    aSessEnv->mHasWithoutHoldCursor = STL_FALSE;
    aSessEnv->mLatestStatement      = NULL;

    knlValidateDynamicMem( & aSessEnv->mMemStatement, KNL_ENV( aEnv ) );
    sState = 0;
    STL_TRY( knlDestroyDynamicMem( & aSessEnv->mMemStatement,
                                   KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    /*
     * 하위 Layer 의 Session Env 를 종료
     */

    STL_TRY( qllFiniSessionEnv( (qllSessionEnv*)aSessEnv,
                                QLL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            if( aSessEnv->mStatementSlot != NULL )
            {
                (void) knlFreeDynamicMem( &aSessEnv->mMemStatement,
                                          aSessEnv->mStatementSlot,
                                          KNL_ENV( aEnv ) );
                aSessEnv->mStatementSlot = NULL;
            }
        case 1:
            (void) knlDestroyDynamicMem( & aSessEnv->mMemStatement, KNL_ENV( aEnv ) );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus sslSetNLS( dtlCharacterSet   aCharacterSet,
                     stlInt32          aTimezone,
                     sslEnv          * aEnv )
{

    STL_TRY( qllSetLoginCharacterSet( aCharacterSet,
                                      QLL_ENV(aEnv) ) == STL_SUCCESS );

    STL_TRY( qllSetLoginTimeZoneOffset( aTimezone,
                                        QLL_ENV(aEnv) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus sslSetTransactionMode( sslSessionEnv * aSessionEnv,
                                 sslEnv        * aEnv )
{
    STL_PARAM_VALIDATE( aSessionEnv != NULL,
                        KNL_ERROR_STACK( aEnv ) );

    /*
     * ACCESS MODE
     */
    if( aSessionEnv->mNextAccessMode == QLL_ACCESS_MODE_NONE )
    {
        qllSetTransAccessMode( aSessionEnv->mDefaultAccessMode, QLL_ENV(aEnv) );
    }
    else
    {
        qllSetTransAccessMode( aSessionEnv->mNextAccessMode, QLL_ENV(aEnv) );
    }

    aSessionEnv->mNextAccessMode = QLL_ACCESS_MODE_NONE;

    /*
     * ISOLATION LEVEL
     */
    if( aSessionEnv->mNextTxnIsolation == SSL_ISOLATION_LEVEL_NONE )
    {
        aSessionEnv->mCurrTxnIsolation = aSessionEnv->mDefaultTxnIsolation;
    }
    else
    {
        aSessionEnv->mCurrTxnIsolation = aSessionEnv->mNextTxnIsolation;
    }

    aSessionEnv->mNextTxnIsolation = SSL_ISOLATION_LEVEL_NONE;

    /*
     * UNIQUE MODE
     */
    if( aSessionEnv->mNextUniqueMode == QLL_UNIQUE_MODE_NONE )
    {
        qllSetTransUniqueMode( aSessionEnv->mDefaultUniqueMode, QLL_ENV(aEnv) );
    }
    else
    {
        qllSetTransUniqueMode( aSessionEnv->mNextUniqueMode, QLL_ENV(aEnv) );
    }

    aSessionEnv->mNextUniqueMode = QLL_UNIQUE_MODE_NONE;

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief X$SESSION Table 정의
 */
knlFxColumnDesc gSessionColumnDesc[] =
{
    {
        "ID",
        "Session identifier",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( sssSessionFxRecord, mID ),
        STL_SIZEOF( stlUInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "SERIAL",
        "Session serial number",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( sssSessionFxRecord, mSerial ),
        STL_SIZEOF( stlUInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "TRANS_ID",
        "transaction identifier",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( sssSessionFxRecord, mTransId ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "TOP_LAYER",
        "top layer",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( sssSessionFxRecord, mTopLayer ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "CONNECTION",
        "connection type",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( sssSessionFxRecord, mConnectionType ),
        SS_CONNECTION_TYPE_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "USER_ID",
        "Gliese user ID",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( sssSessionFxRecord, mUserId ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "STATUS",
        "Status of the session",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( sssSessionFxRecord, mStatus ),
        SS_STATUS_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "WATCH",
        "Watching session status",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( sssSessionFxRecord, mWatch ),
        SS_WATCH_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "SERVER",
        "Server type",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( sssSessionFxRecord, mServerType ),
        SS_SERVER_TYPE_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "CLIENT_PROCESS",
        "Client process identifier",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( sssSessionFxRecord, mClientProcId ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "SERVER_PROCESS",
        "Server process identifier",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( sssSessionFxRecord, mServerProcId ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "LOGON_TIME",
        "logon time",
        KNL_FXTABLE_DATATYPE_TIMESTAMP,
        STL_OFFSETOF( sssSessionFxRecord, mLogonTime ),
        STL_SIZEOF( stlTime ),
        STL_FALSE  /* nullable */
    },
    {
        "PROGRAM",
        "Program name",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( sssSessionFxRecord, mProgram ),
        SS_PROGRAM_LENGTH,
        STL_TRUE  /* nullable */
    },
    {
        "ADDRESS",
        "client address",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( sssSessionFxRecord, mClientAddress ),
        STL_MAX_HOST_LEN + 1,
        STL_TRUE  /* nullable */
    },
    {
        "PORT",
        "client port",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( sssSessionFxRecord, mClientPort ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        NULL,
        NULL,
        0,
        0,
        0,
        STL_FALSE  /* nullable */
    }
};

stlStatus openFxSessionCallback( void   * aStmt,
                                 void   * aDumpObjHandle,
                                 void   * aPathCtrl,
                                 knlEnv * aEnv )
{
    sssSessionPathControl * sCtl = (sssSessionPathControl*)aPathCtrl;

    sCtl->mSession = knlGetFirstSessionEnv();

    return STL_SUCCESS;
}

stlStatus closeFxSessionCallback( void   * aDumpObjHandle,
                                  void   * aPathCtrl,
                                  knlEnv * aEnv )
{
    return STL_SUCCESS;
}

stlStatus buildRecordFxSessionCallback( void              * aDumpObjHandle,
                                        void              * aPathCtrl,
                                        knlValueBlockList * aValueList,
                                        stlInt32            aBlockIdx,
                                        stlBool           * aTupleExist,
                                        knlEnv            * aEnv )
{
    sssSessionPathControl * sCtl = (sssSessionPathControl*)aPathCtrl;
    sssSessionFxRecord      sRec;
    sslSessionEnv         * sSession;
    knlEnv                * sKnlEnv;

    *aTupleExist = STL_FALSE;

    while( sCtl->mSession != NULL )
    {
        sSession = (sslSessionEnv*)sCtl->mSession;

        /*
         * 사용중인 Session만 출력한다.
         */
        if( ((knlSessionEnv*)sSession)->mUsed == STL_FALSE )
        {
            sCtl->mSession = knlGetNextSessionEnv( (void*)sSession );
            continue;
        }

        STL_TRY( knlGetSessionEnvId( (void*)sSession,
                                     &(sRec.mID),
                                     aEnv ) == STL_SUCCESS );

        sRec.mUserId   = ((ellSessionEnv*)sSession)->mLoginAuthID;
        sRec.mSerial   = KNL_GET_SESSION_SEQ( sSession );
        sRec.mTopLayer = knlGetSessionTopLayer( (knlSessionEnv*)sSession );
        sRec.mLogonTime = ((knlSessionEnv*)sSession)->mLogonTime;
        sRec.mTransId = sSession->mTransId;

        switch( KNL_GET_SESSION_ACCESS_MASK( sSession ) )
        {
            case KNL_SESSION_ACCESS_ENTER:
                stlSnprintf( sRec.mWatch, SS_WATCH_LENGTH, "ENTER" );
                break;
            case KNL_SESSION_ACCESS_LEAVE:
                stlSnprintf( sRec.mWatch, SS_WATCH_LENGTH, "LEAVE" );
                break;
            default :
                stlSnprintf( sRec.mWatch, SS_WATCH_LENGTH, "UNKNOWN" );
                break;
        }
        
        switch( KNL_GET_SESSION_STATUS(sSession) )
        {
            case KNL_SESSION_STATUS_CONNECTED:
                stlSnprintf( sRec.mStatus, SS_STATUS_LENGTH, "CONNECTED" );
                break;
            case KNL_SESSION_STATUS_DISCONNECTED:
                stlSnprintf( sRec.mStatus, SS_STATUS_LENGTH, "DISCONNECTED" );
                break;
            case KNL_SESSION_STATUS_SIGNALED:
                stlSnprintf( sRec.mStatus, SS_STATUS_LENGTH, "SIGNALED" );
                break;
            case KNL_SESSION_STATUS_SNIPED:
                stlSnprintf( sRec.mStatus, SS_STATUS_LENGTH, "SNIPED" );
                break;
            case KNL_SESSION_STATUS_DEAD:
                stlSnprintf( sRec.mStatus, SS_STATUS_LENGTH, "DEAD" );
                break;
            default :
                stlSnprintf( sRec.mStatus, SS_STATUS_LENGTH, "UNKNOWN" );
                break;
        }
            
        switch( knlGetSessionConnectionType( (knlSessionEnv *)sSession ) )
        {
            case KNL_CONNECTION_TYPE_DA :
                stlSnprintf( sRec.mConnectionType, SS_CONNECTION_TYPE_LENGTH, "DA" );
                break;
            case KNL_CONNECTION_TYPE_TCP :
                stlSnprintf( sRec.mConnectionType, SS_CONNECTION_TYPE_LENGTH, "TCP" );
                break;
            default:
                stlSnprintf( sRec.mConnectionType, SS_CONNECTION_TYPE_LENGTH, "UNKNOWN" );
                break;
        }

        switch( ((knlSessionEnv*)sSession)->mSessionType )
        {
            case KNL_SESSION_TYPE_DEDICATE :
                stlSnprintf( sRec.mServerType, SS_SERVER_TYPE_LENGTH, "DEDICATED" );
                break;
            case KNL_SESSION_TYPE_SHARED :
                stlSnprintf( sRec.mServerType, SS_SERVER_TYPE_LENGTH, "SHARED" );
                break;
            default :
                stlSnprintf( sRec.mServerType, SS_SERVER_TYPE_LENGTH, "UNKNOWN" );
                break;
        }

        sKnlEnv = KNL_SESS_LINKED_ENV( sSession );

        if( sKnlEnv != NULL )
        {
            sRec.mServerProcId = sKnlEnv->mThread.mProc.mProcID;
        }
        else
        {
            sRec.mServerProcId = 0;
        }
        
        sRec.mClientProcId = sSession->mClientProcId.mProcID;
        sRec.mClientPort = sSession->mClientPort;
        stlStrcpy( sRec.mProgram, sSession->mProgram ); 
        stlStrcpy( sRec.mClientAddress, sSession->mClientAddress ); 

        STL_TRY( knlBuildFxRecord( gSessionColumnDesc,
                                   &sRec,
                                   aValueList,
                                   aBlockIdx,
                                   aEnv ) == STL_SUCCESS );

        sCtl->mSession = knlGetNextSessionEnv( (void*)sSession );

        *aTupleExist = STL_TRUE;
        break;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief X$SESSION Table 정보
 */
knlFxTableDesc gSessionTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    NULL,
    openFxSessionCallback,
    closeFxSessionCallback,
    buildRecordFxSessionCallback,
    STL_SIZEOF( sssSessionFxRecord ),
    "X$SESSION",
    "session information fixed table",
    gSessionColumnDesc
};

/**
 * @brief X$SS_SESS_STAT
 */
knlFxSessionInfo gSssSessStatRows[SSS_SS_SESS_ENV_ROW_COUNT] =
{
    {
        "STATEMENT_MEMORY_TOTAL_SIZE",
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        0
    },
    {
        "STATEMENT_MEMORY_INIT_SIZE",
        0,
        KNL_STAT_CATEGORY_INIT_MEMORY,
        0
    }
};

stlStatus openFxSsSessStatCallback( void   * aStmt,
                                    void   * aDumpObjHandle,
                                    void   * aPathCtrl,
                                    knlEnv * aKnlEnv )
{
    sssSessStatPathControl * sPathCtrl;
    knlSessionEnv          * sSessEnv;
    stlInt64               * sValues;

    sPathCtrl  = (sssSessStatPathControl*)aPathCtrl;

    sPathCtrl->mCurEnv = NULL;
    sPathCtrl->mIterator = 0;
    sValues = sPathCtrl->mValues;
    
    sSessEnv = knlGetFirstSessionEnv();

    while( sSessEnv != NULL )
    {
        if( knlIsUsedSessionEnv( sSessEnv ) == STL_TRUE )
        {
            STL_TRY( knlGetSessionEnvId( sSessEnv,
                                         &sPathCtrl->mSessId,
                                         aKnlEnv )
                     == STL_SUCCESS );
            
            sPathCtrl->mCurEnv = sSessEnv;

            sValues[0] = ((sslSessionEnv*)sSessEnv)->mMemStatement.mTotalSize;
            sValues[1] = ((sslSessionEnv*)sSessEnv)->mMemStatement.mInitSize;
            
            break;
        }
        
        sSessEnv = knlGetNextSessionEnv( sSessEnv );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus closeFxSsSessStatCallback( void   * aDumpObjHandle,
                                     void   * aPathCtrl,
                                     knlEnv * aKnlEnv )
{
    return STL_SUCCESS;
}

stlStatus buildRecordFxSsSessStatCallback( void              * aDumpObjHandle,
                                           void              * aPathCtrl,
                                           knlValueBlockList * aValueList,
                                           stlInt32            aBlockIdx,
                                           stlBool           * aTupleExist,
                                           knlEnv            * aKnlEnv )
{
    sssSessStatPathControl * sPathCtrl;
    knlFxSessionInfo         sFxSessionInfo;
    knlFxSessionInfo       * sRowDesc;
    stlInt64               * sValues;
    knlSessionEnv          * sSessEnv;

    *aTupleExist = STL_FALSE;
    
    sPathCtrl = (sssSessStatPathControl*)aPathCtrl;
    sValues = sPathCtrl->mValues;

    if( sPathCtrl->mIterator >= SSS_SS_SESS_ENV_ROW_COUNT )
    {
        sSessEnv = knlGetNextSessionEnv( sPathCtrl->mCurEnv );

        while( sSessEnv != NULL )
        {
            if( knlIsUsedSessionEnv( sSessEnv ) == STL_TRUE )
            {
                STL_TRY( knlGetSessionEnvId( sSessEnv,
                                             &sPathCtrl->mSessId,
                                             aKnlEnv )
                         == STL_SUCCESS );
            
                sValues[0] = ((sslSessionEnv*)sSessEnv)->mMemStatement.mTotalSize;
                sValues[1] = ((sslSessionEnv*)sSessEnv)->mMemStatement.mInitSize;
                break;
            }
        
            sSessEnv = knlGetNextSessionEnv( sSessEnv );
        }

        sPathCtrl->mCurEnv = sSessEnv;
        sPathCtrl->mIterator = 0;
    }
    
    STL_TRY_THROW( sPathCtrl->mCurEnv != NULL, RAMP_SUCCESS );

    sRowDesc = &gSssSessStatRows[sPathCtrl->mIterator];

    sFxSessionInfo.mName     = sRowDesc->mName;
    sFxSessionInfo.mCategory = sRowDesc->mCategory;
    sFxSessionInfo.mSessId   = sPathCtrl->mSessId;
    sFxSessionInfo.mValue    = sValues[sPathCtrl->mIterator];

    STL_TRY( knlBuildFxRecord( gKnlSessionInfoColumnDesc,
                               &sFxSessionInfo,
                               aValueList,
                               aBlockIdx,
                               aKnlEnv )
             == STL_SUCCESS );

    *aTupleExist = STL_TRUE;
    sPathCtrl->mIterator += 1;

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gSsSessStatTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    NULL,
    openFxSsSessStatCallback,
    closeFxSsSessStatCallback,
    buildRecordFxSsSessStatCallback,
    STL_SIZEOF( sssSessStatPathControl ),
    "X$SS_SESS_STAT",
    "session statistic information of session layer",
    gKnlSessionInfoColumnDesc
};

/** @} sslSessEnv */
