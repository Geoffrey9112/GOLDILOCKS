/*******************************************************************************
 * sslCommand.c
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
 * @brief Session Command Routines
 */

#include <qll.h>
#include <sslDef.h>
#include <ssDef.h>
#include <sslEnv.h>
#include <sslCommand.h>
#include <sslSession.h>
#include <sslStatement.h>
#include <ssc.h>
#include <sst.h>
#include <ssx.h>
#include <ssl.h>

extern ssgWarmupEntry  * gSsgWarmupEntry;

static knlBindType sslGetBindType( stlInt16 aInOutType )
{
    knlBindType sBindType;

    switch( aInOutType )
    {
        case SSL_PARAM_INPUT :
            sBindType = KNL_BIND_TYPE_IN;
            break;
        case SSL_PARAM_INPUT_OUTPUT :
            sBindType = KNL_BIND_TYPE_INOUT;
            break;
        case SSL_PARAM_OUTPUT :
            sBindType = KNL_BIND_TYPE_OUT;
            break;
        default :
            sBindType = KNL_BIND_TYPE_INVALID;
            break;
    }

    return sBindType;
}

static stlInt32 sslGetStmtType( qllNodeType aStatementType )
{
    stlInt32 sType;
    
    switch( aStatementType )
    {
        case QLL_STMT_ALTER_DOMAIN_TYPE :
            sType = SSL_STMT_TYPE_ALTER_DOMAIN;
            break;
        case QLL_STMT_ALTER_TABLE_ADD_COLUMN_TYPE :
        case QLL_STMT_ALTER_TABLE_DROP_COLUMN_TYPE :
        case QLL_STMT_ALTER_TABLE_RENAME_COLUMN_TYPE :
        case QLL_STMT_ALTER_TABLE_ADD_CONSTRAINT_TYPE :
        case QLL_STMT_ALTER_TABLE_ALTER_CONSTRAINT_TYPE :
        case QLL_STMT_ALTER_TABLE_DROP_CONSTRAINT_TYPE :
        case QLL_STMT_ALTER_TABLE_RENAME_CONSTRAINT_TYPE :
        case QLL_STMT_ALTER_TABLE_RENAME_TABLE_TYPE :
        case QLL_STMT_ALTER_TABLE_TABLESPACE_TYPE :
            sType = SSL_STMT_TYPE_ALTER_TABLE;
            break;
        case QLL_STMT_CALL_TYPE :
            sType = SSL_STMT_TYPE_CALL;
            break;
        case QLL_STMT_CREATE_ASSERTION_TYPE :
            sType = SSL_STMT_TYPE_CREATE_ASSERTION;
            break;
        case QLL_STMT_CREATE_CHARACTER_SET_TYPE :
            sType = SSL_STMT_TYPE_CREATE_CHARACTER_SET;
            break;
        case QLL_STMT_CREATE_COLLATION_TYPE :
            sType = SSL_STMT_TYPE_CREATE_COLLATION;
            break;
        case QLL_STMT_CREATE_DOMAIN_TYPE :
            sType = SSL_STMT_TYPE_CREATE_DOMAIN;
            break;
        case QLL_STMT_CREATE_INDEX_TYPE :
            sType = SSL_STMT_TYPE_CREATE_INDEX;
            break;
        case QLL_STMT_CREATE_SCHEMA_TYPE :
            sType = SSL_STMT_TYPE_CREATE_SCHEMA;
            break;
        case QLL_STMT_CREATE_TABLE_TYPE :
        case QLL_STMT_CREATE_TABLE_AS_SELECT_TYPE :
            sType = SSL_STMT_TYPE_CREATE_TABLE;
            break;
        case QLL_STMT_CREATE_TRANSLATION_TYPE :
            sType = SSL_STMT_TYPE_CREATE_TRANSLATION;
            break;
        case QLL_STMT_CREATE_VIEW_TYPE :
            sType = SSL_STMT_TYPE_CREATE_VIEW;
            break;
        case QLL_STMT_DELETE_TYPE :
        case QLL_STMT_DELETE_RETURNING_QUERY_TYPE :
        case QLL_STMT_DELETE_RETURNING_INTO_TYPE :
            sType = SSL_STMT_TYPE_DELETE_WHERE;
            break;
        case QLL_STMT_DROP_ASSERTION_TYPE :
            sType = SSL_STMT_TYPE_DROP_ASSERTION;
            break;
        case QLL_STMT_DROP_CHARACTER_SET_TYPE :
            sType = SSL_STMT_TYPE_DROP_CHARACTER_SET;
            break;
        case QLL_STMT_DROP_COLLATION_TYPE :
            sType = SSL_STMT_TYPE_DROP_COLLATION;
            break;
        case QLL_STMT_DROP_DOMAIN_TYPE :
            sType = SSL_STMT_TYPE_DROP_DOMAIN;
            break;
        case QLL_STMT_DROP_INDEX_TYPE :
            sType = SSL_STMT_TYPE_DROP_INDEX;
            break;
        case QLL_STMT_DROP_TABLE_TYPE :
            sType = SSL_STMT_TYPE_DROP_TABLE;
            break;
        case QLL_STMT_DROP_TRANSLATION_TYPE :
            sType = SSL_STMT_TYPE_DROP_TRANSLATION;
            break;
        case QLL_STMT_DROP_VIEW_TYPE :
            sType = SSL_STMT_TYPE_DROP_VIEW;
            break;
        case QLL_STMT_DELETE_WHERE_CURRENT_OF_TYPE :
            sType = SSL_STMT_TYPE_DYNAMIC_DELETE_CURSOR;
            break;
        case QLL_STMT_UPDATE_WHERE_CURRENT_OF_TYPE :
            sType = SSL_STMT_TYPE_DYNAMIC_UPDATE_CURSOR;
            break;
        case QLL_STMT_GRANT_DATABASE_TYPE :
        case QLL_STMT_GRANT_TABLESPACE_TYPE :
        case QLL_STMT_GRANT_SCHEMA_TYPE :
        case QLL_STMT_GRANT_TABLE_TYPE :
        case QLL_STMT_GRANT_USAGE_TYPE :
        case QLL_STMT_GRANT_ROLE_TYPE :
            sType = SSL_STMT_TYPE_GRANT;
            break;
        case QLL_STMT_INSERT_TYPE :
        case QLL_STMT_INSERT_RETURNING_QUERY_TYPE :
        case QLL_STMT_INSERT_RETURNING_INTO_TYPE :
            sType = SSL_STMT_TYPE_INSERT;
            break;
        case QLL_STMT_REVOKE_DATABASE_TYPE :
        case QLL_STMT_REVOKE_TABLESPACE_TYPE :
        case QLL_STMT_REVOKE_SCHEMA_TYPE :
        case QLL_STMT_REVOKE_TABLE_TYPE :
        case QLL_STMT_REVOKE_USAGE_TYPE :
        case QLL_STMT_REVOKE_ROLE_TYPE :
            sType = SSL_STMT_TYPE_REVOKE;
            break;
        case QLL_STMT_SELECT_TYPE :
        case QLL_STMT_SELECT_FOR_UPDATE_TYPE :
        case QLL_STMT_SELECT_INTO_TYPE :
        case QLL_STMT_SELECT_INTO_FOR_UPDATE_TYPE :
            sType = SSL_STMT_TYPE_SELECT_CURSOR;
            break;
        case QLL_STMT_UPDATE_TYPE :
        case QLL_STMT_UPDATE_RETURNING_QUERY_TYPE :
        case QLL_STMT_UPDATE_RETURNING_INTO_TYPE :
            sType = SSL_STMT_TYPE_UPDATE_WHERE;
            break;
        default :
            sType = SSL_STMT_TYPE_UNKNOWN;
            break;
    }

    return sType;
}

void sslSetCallStack( sslEnv * aEnv )
{
    stlBool         sShowCallStack;
    stlErrorStack * sErrorStack;
    stlErrorData  * sErrorData;
    stlInt32        sLength;
    stlChar         sCallStack[STL_MAX_ERROR_MESSAGE];
    
    if( aEnv != NULL )
    {
        sShowCallStack = knlGetPropertyBoolValueByID( KNL_PROPERTY_SHOW_CALLSTACK,
                                                      KNL_ENV( aEnv ) );

        if( sShowCallStack == STL_TRUE )
        {
            sErrorStack = KNL_ERROR_STACK( aEnv );
        
            stlPrintFirstErrorCallstackToStr( sErrorStack,
                                              sCallStack,
                                              STL_MAX_ERROR_MESSAGE );

            sErrorData = stlPopError( sErrorStack );

            if( sErrorData != NULL )
            {
                sLength = stlStrlen( sErrorData->mDetailErrorMessage );
                
                stlSnprintf( sErrorData->mDetailErrorMessage + sLength,
                             STL_MAX_ERROR_MESSAGE - sLength,
                             "\n%s",
                             sCallStack );

                stlPushErrorData( sErrorData, sErrorStack );
            }
        }
    }

    return;
}

stlStatus sslConnect( stlChar           * aUserName,
                      stlChar           * aEncryptedPassword,
                      stlChar           * aRoleName,
                      stlChar           * aOldPassword,
                      stlChar           * aNewPassword,
                      stlProc             aProc,
                      stlChar           * aProgram,
                      stlChar           * aAddress,
                      stlInt64            aPort,
                      dtlCharacterSet     aCharacterSet,
                      stlInt32            aTimezone,
                      dtlCharacterSet   * aDBCharacterSet,
                      dtlCharacterSet   * aDBNCharCharacterSet,
                      stlInt32          * aDBTimezone,
                      sslEnv            * aEnv )
{
    sslSessionEnv              * sSessionEnv = SSL_SESS_ENV(aEnv);
    dtlIntervalDayToSecondType   sDBTimezone;

    /**
     * decide numa node
     */
    if( KNL_SESSION_TYPE(sSessionEnv) == KNL_SESSION_TYPE_DEDICATE )
    {
        STL_TRY( knlBindNumaNode( KNL_ENV(aEnv) ) == STL_SUCCESS );
    }

    /*
     * 사용자 인증
     */

    if( sscAuthenticateUser( aUserName,
                             aEncryptedPassword,
                             aRoleName,
                             aOldPassword,
                             aNewPassword,
                             aEnv )
        == STL_FAILURE )
    {
        if( knlGetPropertyBoolValueByID( KNL_PROPERTY_TRACE_LOGIN, KNL_ENV(aEnv) ) == STL_TRUE )
        {
            STL_TRY( knlLogMsg( &gSsgWarmupEntry->mLoginLogger,
                                KNL_ENV(aEnv),
                                KNL_LOG_LEVEL_WARNING,
                                "[LOGIN INFORMATION] failed to login \n"
                                "---- USER NAME  : %s\n"
                                "---- ROLE NAME  : %s\n"
                                "---- CONNECTION : %s\n"
                                "---- HOST       : %s\n"
                                "---- PORT       : %d\n"
                                "---- PROGRAM    : %s\n",
                                aUserName,
                                aRoleName,
                                ( aAddress == NULL ) ? "DA" : "TCP",
                                ( aAddress == NULL ) ? "" : aAddress,
                                aPort,
                                aProgram )
                     == STL_SUCCESS );
        }
        
        STL_TRY( STL_FALSE );
    }
    else
    {
        if( knlGetPropertyBoolValueByID( KNL_PROPERTY_TRACE_LOGIN, KNL_ENV(aEnv) ) == STL_TRUE )
        {
            STL_TRY( knlLogMsg( &gSsgWarmupEntry->mLoginLogger,
                                KNL_ENV(aEnv),
                                KNL_LOG_LEVEL_INFO,
                                "[LOGIN INFORMATION] login success \n"
                                "---- USER NAME  : %s\n"
                                "---- ROLE NAME  : %s\n"
                                "---- CONNECTION : %s\n"
                                "---- HOST       : %s\n"
                                "---- PORT       : %d\n"
                                "---- PROGRAM    : %s\n",
                                aUserName,
                                aRoleName,
                                ( aAddress == NULL ) ? "DA" : "TCP",
                                ( aAddress == NULL ) ? "" : aAddress,
                                aPort,
                                aProgram )
                     == STL_SUCCESS );
        }
    }
        
    /*
     * statement array 생성
     */
    STL_TRY( sslInitSessionStmtSlot( sSessionEnv, aEnv ) == STL_SUCCESS );

    /*
     * SESSION NLS 설정
     */

    STL_TRY( sslSetNLS( aCharacterSet, aTimezone, aEnv ) == STL_SUCCESS );

    if( aAddress != NULL )
    {
        stlStrncpy( sSessionEnv->mClientAddress, aAddress, STL_MAX_HOST_LEN );
    }
    else
    {
        sSessionEnv->mClientAddress[0] = '\0';
    }
    
    sSessionEnv->mClientPort = aPort;
    sSessionEnv->mClientProcId = aProc;
    stlSnprintf( sSessionEnv->mProgram, SSL_PROGRAM_NAME, "%s", aProgram );

    *aDBCharacterSet = ellGetDbCharacterSet();

    /**
     * @todo NCHAR CHARACTERSET 지원시 변경되어야 한다.
     */
    *aDBNCharCharacterSet = DTL_CHARACTERSET_MAX;

    sDBTimezone = ellGetDbTimezone();

    STL_TRY( dtlTimeZoneToGMTOffset( &sDBTimezone,
                                     aDBTimezone,
                                     KNL_ERROR_STACK(aEnv) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    sslSetCallStack( aEnv );

    return STL_FAILURE;
}

stlStatus sslDisconnect( sslEnv * aEnv )
{
    sslSessionEnv * sSessionEnv;

    sSessionEnv = SSL_SESS_ENV( aEnv );
    
    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS );

    if( sSessionEnv->mTransId != SML_INVALID_TRANSID )
    {
        if( knlGetTransScope( sSessionEnv ) == KNL_TRANS_SCOPE_GLOBAL )
        {
            STL_TRY( sslCleanupContext( sSessionEnv->mXaContext,
                                        sSessionEnv,
                                        aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            STL_TRY( qllRollback( sSessionEnv->mTransId,
                                  NULL, /* aXaContext */
                                  QLL_ENV( aEnv ) ) == STL_SUCCESS );
        }
        
        sSessionEnv->mTransId = SML_INVALID_TRANSID;
        sSessionEnv->mXaContext = NULL;
    }

    if( KNL_SESSION_TYPE(sSessionEnv) == KNL_SESSION_TYPE_DEDICATE )
    {
        STL_TRY( knlUnbindNumaNode( STL_FALSE,  /* aIsCleanup */
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    sslSetCallStack( aEnv );

    return STL_FAILURE;
}
                      
stlStatus sslCommit( sslEnv * aEnv )
{
    sslSessionEnv * sSessionEnv;
    stlErrorData  * sErrorData = NULL;

    sSessionEnv = SSL_SESS_ENV( aEnv );
    
    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( knlBreakPointWaitToStart( KNL_ENV(aEnv) ) == STL_SUCCESS );

    STL_TRY_THROW( knlGetTransScope( sSessionEnv ) != KNL_TRANS_SCOPE_GLOBAL,
                   RAMP_ERR_NOT_ALLOW_COMMAND );

    STL_TRY( sscCloseWithoutHoldCursors( aEnv ) == STL_SUCCESS );
    
    STL_TRY( qllCommit( sSessionEnv->mTransId,
                        NULL, /* aComment */
                        SML_TRANSACTION_CWM_NONE,
                        NULL, /* aXaContext */
                        QLL_ENV( aEnv ) ) == STL_SUCCESS );

    knlSetTransScope( sSessionEnv, KNL_TRANS_SCOPE_NONE );
    sSessionEnv->mTransId = SML_INVALID_TRANSID;

    if( knlGetEndSessionOption( (knlSessionEnv*)sSessionEnv ) == KNL_END_SESSION_OPTION_POST_TRANSACTION )
    {
        knlKillSession( sSessionEnv );
    }

    STL_TRY( sslSetTransactionMode( sSessionEnv, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_ALLOW_COMMAND )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SSL_ERRCODE_NOT_ALLOW_COMMAND_IN_XA,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    sErrorData = stlGetLastErrorData( KNL_ERROR_STACK( aEnv ) );

    if( sErrorData != NULL )
    {
        switch( stlGetExternalErrorCode( sErrorData ) )
        {
            case STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_NO_SUBCLASS :
            case STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_INTEGRITY_CONSTRAINT_VIOLATION :
            case STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_STATEMENT_COMPLETION_UNKNOWN :
            case STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_TRIGGERED_ACTION_EXCEPTION :
                (void) qllRollback( sSessionEnv->mTransId,
                                    NULL, /* aXaContext */
                                    QLL_ENV( aEnv ) );
                (void) sslSetTransactionMode( sSessionEnv, aEnv );
            
                sSessionEnv->mTransId = SML_INVALID_TRANSID;
                knlSetTransScope( sSessionEnv, KNL_TRANS_SCOPE_NONE );
                break;
            default :
                break;
        }
    }
    
    sslSetCallStack( aEnv );

    if( knlGetEndSessionOption( (knlSessionEnv*)sSessionEnv ) ==
        KNL_END_SESSION_OPTION_POST_TRANSACTION )
    {
        knlKillSession( sSessionEnv );
    }
    
    return STL_FAILURE;
}
                      
stlStatus sslRollback( sslEnv * aEnv )
{
    sslSessionEnv * sSessionEnv;

    sSessionEnv = SSL_SESS_ENV( aEnv );
    
    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS );

    STL_TRY_THROW( knlGetTransScope( sSessionEnv ) != KNL_TRANS_SCOPE_GLOBAL,
                   RAMP_ERR_NOT_ALLOW_COMMAND );
    
    STL_TRY( sscCloseWithoutHoldCursors( aEnv ) == STL_SUCCESS );
    
    STL_TRY( qllRollback( sSessionEnv->mTransId,
                          NULL, /* aXaContext */
                          QLL_ENV( aEnv ) ) == STL_SUCCESS );

    knlSetTransScope( sSessionEnv, KNL_TRANS_SCOPE_NONE );
    sSessionEnv->mTransId = SML_INVALID_TRANSID;

    if( knlGetEndSessionOption( (knlSessionEnv*)sSessionEnv ) == KNL_END_SESSION_OPTION_POST_TRANSACTION )
    {
        knlKillSession( sSessionEnv );
    }

    STL_TRY( sslSetTransactionMode( sSessionEnv, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_ALLOW_COMMAND )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SSL_ERRCODE_NOT_ALLOW_COMMAND_IN_XA,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    (void) sslSetTransactionMode( sSessionEnv, aEnv );
    sslSetCallStack( aEnv );

    if( knlGetEndSessionOption( (knlSessionEnv*)sSessionEnv ) ==
        KNL_END_SESSION_OPTION_POST_TRANSACTION )
    {
        knlKillSession( sSessionEnv );
    }

    return STL_FAILURE;
}
                      
stlStatus sslSetSessionAttr( sslConnectionAttr   aSessionAttr,
                             stlInt32            aIntValue,
                             stlChar           * aStrValue,
                             stlInt32            aStrLength,
                             sslEnv            * aEnv )
{
    sslSessionEnv     * sSessionEnv;
    sslIsolationLevel   sIsolationLevel;

    sSessionEnv = SSL_SESS_ENV( aEnv );
    
    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    switch( aSessionAttr )
    {
        case SSL_CONNECTION_ATTR_ACCESS_MODE :
            switch( aIntValue )
            {
                case QLL_ACCESS_MODE_READ_WRITE :
                case QLL_ACCESS_MODE_READ_ONLY :
                    sSessionEnv->mDefaultAccessMode = aIntValue;
                    qllSetTransAccessMode( (qllAccessMode)aIntValue,
                                           QLL_ENV(aEnv) );
                    break;
                default :
                    STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_VALUE );
                    break;
            }
            break;
        case SSL_CONNECTION_ATTR_TXN_ISOLATION :
            sIsolationLevel = (sslIsolationLevel)aIntValue;

            switch( sIsolationLevel )
            {
                case SSL_ISOLATION_LEVEL_READ_UNCOMMITTED :
                case SSL_ISOLATION_LEVEL_READ_COMMITTED :
                case SSL_ISOLATION_LEVEL_SERIALIZABLE :
                    sSessionEnv->mDefaultTxnIsolation = sIsolationLevel;
                    sSessionEnv->mCurrTxnIsolation    = sIsolationLevel;
                    break;
                default :
                    STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_VALUE );
                    break;
            }
            break;
        case SSL_CONNECTION_ATTR_TIMEZONE :
            {
                STL_TRY( qllSetSessTimeZoneOffset( aIntValue, QLL_ENV(aEnv) ) == STL_SUCCESS );
                break;
            }
        default :
            STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER );
            break;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_ATTRIBUTE_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SSL_ERRCODE_INVALID_ATTRIBUTE_VALUE,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SSL_ERRCODE_INVALID_ATTRIBUTE_OPTION_IDENTIFIER,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    sslSetCallStack( aEnv );

    return STL_FAILURE;
}

stlStatus sslGetSessionAttr( sslConnectionAttr   aSessionAttr,
                             stlInt32          * aIntValue,
                             stlChar           * aStrValue,
                             stlInt32            aBufferLength,
                             stlInt32          * aStrLength,
                             sslEnv            * aEnv )
{
    sslSessionEnv  * sSessionEnv;

    sSessionEnv = SSL_SESS_ENV( aEnv );
    
    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    switch( aSessionAttr )
    {
        case SSL_CONNECTION_ATTR_ACCESS_MODE :
            *aIntValue = (stlInt32)sSessionEnv->mDefaultAccessMode;
            break;
        case SSL_CONNECTION_ATTR_TXN_ISOLATION :
            *aIntValue = (stlInt32)sSessionEnv->mDefaultTxnIsolation;
            break;
        default :
            STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER );
            break;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SSL_ERRCODE_INVALID_ATTRIBUTE_OPTION_IDENTIFIER,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    sslSetCallStack( aEnv );

    return STL_FAILURE;
}

stlStatus sslAllocStatement( stlInt64  * aStatementId,
                             sslEnv    * aEnv )
{
    sslStatement    * sStatement = NULL;
    sslSessionEnv   * sSessionEnv;
    stlInt32          sState = 0;
    stlInt32          sIdx = 0;
    stlInt32          i;
    stlUInt32         sType;

    sSessionEnv = SSL_SESS_ENV( aEnv );
    
    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    /*
     * 비어있는 statement slot 검색
     */

    STL_TRY_THROW( sSessionEnv->mActiveStmtCount < sSessionEnv->mMaxStmtCount,
                   RAMP_ERR_LIMIT_ON_THE_NUMBER_OF_HANDLES_EXCEEDED );

    for( i = sSessionEnv->mNextStmtSlot; i < sSessionEnv->mMaxStmtCount; i++ )
    {
        if( sSessionEnv->mStatementSlot[i] == NULL )
        {
            break;
        }
    }

    if( i == sSessionEnv->mMaxStmtCount )
    {
        for( i = 0; i < sSessionEnv->mNextStmtSlot; i++ )
        {
            if( sSessionEnv->mStatementSlot[i] == NULL )
            {
                break;
            }
        }
    }

    STL_ASSERT( i < sSessionEnv->mMaxStmtCount );

    sIdx = i;
    sSessionEnv->mNextStmtSlot = sIdx + 1;

    /*
     * statement 할당
     */
                                 
    STL_TRY( knlAllocDynamicMem( &sSessionEnv->mMemStatement,
                                STL_SIZEOF( sslStatement ),
                                (void**)&sSessionEnv->mStatementSlot[sIdx],
                                KNL_ENV( aEnv ) ) == STL_SUCCESS );
    sState = 1;

    sStatement = sSessionEnv->mStatementSlot[sIdx];
    stlMemset( (void*)sStatement, 0x00, STL_SIZEOF( sslStatement ) );

    sSessionEnv->mActiveStmtCount++;

    /*
     * Bind
     */
    if( ((knlSessionEnv *)sSessionEnv)->mSessionType == KNL_SESSION_TYPE_SHARED )
    {
        sType = KNL_MEM_STORAGE_TYPE_SHM;
    }
    else
    {
        sType = KNL_MEM_STORAGE_TYPE_HEAP;
    }

    STL_TRY( knlCreateRegionMem( &sStatement->mColumnRegionMem,
                                 KNL_ALLOCATOR_SESSION_REGION_STATEMENT_COLUMN,
                                 NULL,  /* aParentMem */
                                 sType,
                                 KNL_DEFAULT_LARGE_REGION_SIZE,
                                 KNL_DEFAULT_LARGE_REGION_SIZE,
                                 KNL_ENV( aEnv ) ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( knlCreateRegionMem( &sStatement->mParamRegionMem,
                                 KNL_ALLOCATOR_SESSION_REGION_STATEMENT_PARAM,
                                 NULL,  /* aParentMem */
                                 sType,
                                 KNL_DEFAULT_LARGE_REGION_SIZE,
                                 KNL_DEFAULT_LARGE_REGION_SIZE,
                                 KNL_ENV( aEnv ) ) == STL_SUCCESS );
    sState = 3;

    STL_TRY( knlCreateBindContext( &sStatement->mBindContext,
                                   &sStatement->mParamRegionMem,
                                   KNL_ENV( aEnv ) ) == STL_SUCCESS );

    sSessionEnv->mStatementSeq++;

    sStatement->mId =
        SSL_MAKE_STATEMENT_ID( sIdx, SSL_GET_STATEMENT_SEQ( sSessionEnv->mStatementSeq ) );
    
    sStatement->mCurrArraySize    = 1;
    sStatement->mNextArraySize    = 1;
    sStatement->mExplainType      = SSL_EXPLAIN_PLAN_OFF;
    sStatement->mAtomicExecute    = SSL_ATOMIC_EXECUTION_OFF;
    sStatement->mRowsetFirstRow   = SSL_CURSOR_POSITION_BEFORE_FIRST;
    sStatement->mRowsetCurrentPos = 0;
    sStatement->mRowsetLastPos    = 0;
    sStatement->mIsEndOfScan      = STL_FALSE;
    sStatement->mIsRecompile      = STL_FALSE;
    sStatement->mTargetChanged    = STL_TRUE;
    sStatement->mViewScn          = SML_INFINITE_SCN;
    sStatement->mOptimizeDecision = SSL_OPTIMIZE_DECISION_NONE;
    sStatement->mNeedOptimization = STL_FALSE;
    sStatement->mCursorType       = ELL_CURSOR_DEFAULT_STANDARD_TYPE;
    sStatement->mSensitivity      = ELL_CURSOR_DEFAULT_SENSITIVITY;
    sStatement->mScrollability    = ELL_CURSOR_DEFAULT_SCROLLABILITY;
    sStatement->mHoldability      = ELL_CURSOR_DEFAULT_HOLDABILITY;
    sStatement->mUpdatability     = ELL_CURSOR_DEFAULT_UPDATABILITY;
    sStatement->mCursorName[0]    = '\0';
    sStatement->mResultSetCursor  = NULL;
    sStatement->mTargetInfo       = NULL;
    sStatement->mQueryTimeout     = KNL_DEFAULT_STATEMENT_QUERY_TIMEOUT;

    STL_TRY( qllInitDBC( &sStatement->mDbcStmt,
                         QLL_ENV( aEnv ) ) == STL_SUCCESS );

    STL_TRY( qllInitSQL( &sStatement->mSqlStmt,
                         QLL_ENV( aEnv ) ) == STL_SUCCESS );


    sSessionEnv->mLatestStatement = sStatement;

    *aStatementId = sStatement->mId;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_LIMIT_ON_THE_NUMBER_OF_HANDLES_EXCEEDED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SSL_ERRCODE_LIMIT_ON_THE_NUMBER_OF_HANDLES_EXCEEDED,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    sslSetCallStack( aEnv );

    switch( sState )
    {
        case 3:
            sStatement->mBindContext = NULL;

            (void)knlDestroyRegionMem( &sStatement->mParamRegionMem,
                                       KNL_ENV( aEnv ) );

            KNL_INIT_REGION_MEMORY( &sStatement->mParamRegionMem );
        case 2:
            (void)knlDestroyRegionMem( &sStatement->mColumnRegionMem,
                                       KNL_ENV( aEnv ) );

            KNL_INIT_REGION_MEMORY( &sStatement->mColumnRegionMem );
        case 1:
            sSessionEnv->mActiveStmtCount--;
            
            (void)knlFreeDynamicMem( &sSessionEnv->mMemStatement,
                                     sStatement,
                                     KNL_ENV( aEnv ) );

            sSessionEnv->mStatementSlot[sIdx] = NULL;
        default:
            break;
    }
        
    return STL_FAILURE;
}

stlStatus sslFreeStatement( stlInt64            aStatementId,
                            sslFreeStmtOption   aOption,
                            sslEnv            * aEnv )
{
    sslSessionEnv * sSessionEnv;
    sslStatement  * sStatement = NULL;
    stlInt32        sState = 0;
    stlInt32        sIdx = 0;

    sSessionEnv = SSL_SESS_ENV( aEnv );
    
    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( sstFind( aStatementId,
                      &sStatement,
                      aEnv ) == STL_SUCCESS );
    
    switch( aOption )
    {
        case SSL_FREE_STMT_OPTION_DROP :
            sIdx = SSL_GET_STATEMENT_SLOT( aStatementId );
            
            /**
             * Parameter Memory 해제
             */

            if( KNL_BIND_CONTEXT_HAS_SERVER_PARAMETER( sStatement->mBindContext ) == STL_TRUE )
            {
                sState = 6;
                STL_TRY( knlFreeServerBindContext( sStatement->mBindContext,
                                                   KNL_ENV(aEnv) )
                         == STL_SUCCESS );
            }

            sState = 5;
            STL_TRY( knlDestroyRegionMem( &sStatement->mParamRegionMem,
                                          KNL_ENV( aEnv ) ) == STL_SUCCESS );
    
            KNL_INIT_REGION_MEMORY( &sStatement->mParamRegionMem );
            
            STL_TRY( knlDestroyRegionMem( &sStatement->mColumnRegionMem,
                                          KNL_ENV( aEnv ) ) == STL_SUCCESS );
    
            KNL_INIT_REGION_MEMORY( &sStatement->mColumnRegionMem );    

            sState = 4;
            STL_TRY( sscCloseCursor( sStatement,
                                     aEnv ) == STL_SUCCESS );

            sState = 3;
            if( sStatement->mCursorDesc != NULL )
            {
                STL_TRY( qllDeleteCursor( sStatement->mCursorDesc->mDeclDesc,
                                          sStatement->mCursorDesc,
                                          QLL_ENV( aEnv ) ) == STL_SUCCESS );

                sStatement->mCursorDesc = NULL;
            }

            sState = 2;
            STL_TRY( qllFiniSQL( &sStatement->mDbcStmt,
                                 &sStatement->mSqlStmt,
                                 QLL_ENV( aEnv ) ) == STL_SUCCESS );

            sState = 1;
            STL_TRY( qllFiniDBC( &sStatement->mDbcStmt,
                                 QLL_ENV( aEnv ) ) == STL_SUCCESS );

            sStatement->mId = 0;

            sState = 0;
            sSessionEnv->mActiveStmtCount--;
            
            STL_TRY( knlFreeDynamicMem( &sSessionEnv->mMemStatement,
                                        sStatement,
                                        KNL_ENV( aEnv ) ) == STL_SUCCESS );

            sSessionEnv->mStatementSlot[sIdx] = NULL;
            break;
        case SSL_FREE_STMT_OPTION_RESET_PARAMS :

            /**
             * Parameter Memory 해제
             */

            if( KNL_BIND_CONTEXT_HAS_SERVER_PARAMETER( sStatement->mBindContext ) == STL_TRUE )
            {
                STL_TRY( knlFreeServerBindContext( sStatement->mBindContext,
                                                   KNL_ENV(aEnv) )
                         == STL_SUCCESS );
            }

            /**
             * reset statement region memory
             */
            
            STL_TRY( knlClearRegionMem( &sStatement->mParamRegionMem,
                                        KNL_ENV( aEnv ) ) == STL_SUCCESS );
    
            STL_TRY( knlCreateBindContext( &sStatement->mBindContext,
                                           &sStatement->mParamRegionMem,
                                           KNL_ENV( aEnv ) ) == STL_SUCCESS );

            sStatement->mNeedOptimization  = STL_TRUE;
            sStatement->mOptimizeDecision |= SSL_OPTIMIZE_DECISION_BIND_INFO;
            
            break;
        default :
            STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_VALUE );
            break;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_ATTRIBUTE_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SSL_ERRCODE_INVALID_ATTRIBUTE_VALUE,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    sslSetCallStack( aEnv );

    switch( sState )
    {
        case 6:
            (void)knlDestroyRegionMem( &sStatement->mParamRegionMem, KNL_ENV( aEnv ) );
        case 5:
            (void)sscCloseCursor( sStatement,
                                  aEnv );
        case 4:
            if( sStatement->mCursorDesc != NULL )
            {
                (void)qllDeleteCursor( sStatement->mCursorDesc->mDeclDesc,
                                       sStatement->mCursorDesc,
                                       QLL_ENV( aEnv ) );

                sStatement->mCursorDesc = NULL;
            }
        case 3:
            (void)qllFiniSQL( &sStatement->mDbcStmt,
                              &sStatement->mSqlStmt,
                              QLL_ENV( aEnv ) );
        case 2:
            (void)qllFiniDBC( &sStatement->mDbcStmt,
                              QLL_ENV( aEnv ) );
        case 1:
            sSessionEnv->mActiveStmtCount--;
            (void)knlFreeDynamicMem( &sSessionEnv->mMemStatement,
                                     sStatement,
                                     KNL_ENV( aEnv ) );
            sSessionEnv->mStatementSlot[sIdx] = NULL;
        default:
            break;
    }

    STL_DASSERT( STL_FALSE );
    
    return STL_FAILURE;
}

stlStatus sslCreateParameter( stlInt64                aStatementId,
                              stlInt32                aParamId,
                              sslParamType            aBindType,
                              dtlDataType             aDtlDataType,
                              stlInt64                aArg1,
                              stlInt64                aArg2,
                              dtlStringLengthUnit     aStringLengthUnit,
                              dtlIntervalIndicator    aIntervalIndicator,
                              knlValueBlockList    ** aINValueBlock,
                              knlValueBlockList    ** aOUTValueBlock,
                              sslEnv                * aEnv )
{
    sslSessionEnv     * sSessionEnv;
    sslStatement      * sStatement;

    dtlValueBlockList * sINParamList  = NULL;
    dtlValueBlockList * sOUTParamList = NULL;
    stlInt64            sBlockCount;

    stlBool             sAllocInParam  = STL_FALSE;
    stlBool             sAllocOutParam = STL_FALSE;

    sSessionEnv = SSL_SESS_ENV( aEnv );
    
    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( sstFind( aStatementId,
                      &sStatement,
                      aEnv ) == STL_SUCCESS );

    sStatement->mCurrArraySize = sStatement->mNextArraySize;

    sBlockCount = (stlInt32)sStatement->mCurrArraySize;

    /*
     * 기존에 정보가 있으면 제거
     */
    if( sStatement->mBindContext->mSize >= aParamId )
    {
        STL_TRY( knlGetParamValueBlock( sStatement->mBindContext,
                                        aParamId,
                                        &sINParamList,
                                        &sOUTParamList,
                                        KNL_ERROR_STACK(aEnv) ) == STL_SUCCESS );

        if( (aINValueBlock == NULL) || (*aINValueBlock == NULL) )
        {
            if( sINParamList != NULL )
            {
                if( KNL_GET_BLOCK_ALLOC_LAYER( sINParamList ) == STL_LAYER_SESSION )
                {
                    STL_TRY( knlFreeParamValues( sINParamList,
                                                 KNL_ENV(aEnv) ) == STL_SUCCESS );
                }
            }
        }

        if( sOUTParamList != NULL )
        {
            STL_TRY( knlFreeParamValues( sOUTParamList,
                                         KNL_ENV(aEnv) ) == STL_SUCCESS );
        }

        sINParamList  = NULL;
        sOUTParamList = NULL;
    }

    if( (aBindType == SSL_PARAM_INPUT) || (aBindType == SSL_PARAM_INPUT_OUTPUT) )
    {
        if( (aINValueBlock == NULL) || (*aINValueBlock == NULL) )
        {
            STL_TRY( knlAllocParamValues( aDtlDataType,
                                          aArg1,
                                          aArg2,
                                          aStringLengthUnit,
                                          aIntervalIndicator,
                                          STL_LAYER_SESSION,
                                          sBlockCount,
                                          &sINParamList,
                                          KNL_ENV(aEnv) ) == STL_SUCCESS );

            sAllocInParam = STL_TRUE;

            sStatement->mBindContext->mHasServerParameter = STL_TRUE;        

            if( aINValueBlock != NULL )
            {
                *aINValueBlock = sINParamList;
            }
        }
        else
        {
            sINParamList = *aINValueBlock; 
        }
    }
    
    if( (aBindType == SSL_PARAM_OUTPUT) || (aBindType == SSL_PARAM_INPUT_OUTPUT) )
    {
        if( (aOUTValueBlock == NULL) || (*aOUTValueBlock == NULL) )
        {
            STL_TRY( knlAllocParamValues( aDtlDataType,
                                          aArg1,
                                          aArg2,
                                          aStringLengthUnit,
                                          aIntervalIndicator,
                                          STL_LAYER_SESSION,
                                          sBlockCount,
                                          &sOUTParamList,
                                          KNL_ENV(aEnv) ) == STL_SUCCESS );

            sAllocOutParam = STL_TRUE;

            sStatement->mBindContext->mHasServerParameter = STL_TRUE;        

            if( aOUTValueBlock != NULL )
            {
                *aOUTValueBlock = sOUTParamList;
            }
        }
        else
        {
            sOUTParamList = *aOUTValueBlock; 
        }
    }
    
    STL_TRY( knlCreateOrReplaceBindParam( sStatement->mBindContext,
                                          &sStatement->mParamRegionMem,
                                          aParamId,
                                          sslGetBindType( aBindType ),
                                          sINParamList,
                                          sOUTParamList,
                                          KNL_ENV(aEnv) ) == STL_SUCCESS );

    sStatement->mNeedOptimization = STL_TRUE;
    sStatement->mOptimizeDecision |= SSL_OPTIMIZE_DECISION_BIND_INFO;

    return STL_SUCCESS;

    STL_FINISH;

    sslSetCallStack( aEnv );

    if( sAllocInParam == STL_TRUE )
    {
        (void)knlFreeParamValues( sINParamList, KNL_ENV(aEnv) );
    }

    if( sAllocOutParam == STL_TRUE )
    {
        (void)knlFreeParamValues( sOUTParamList, KNL_ENV(aEnv) );
    }

    return STL_FAILURE;    
}

stlStatus sslCreateParameter4Imp( stlInt64             aStatementId,
                                  stlInt32             aParamCount,
                                  dtlValueBlockList ** aValueBlock,
                                  sslEnv             * aEnv )
{
    sslSessionEnv * sSessionEnv;
    sslStatement  * sStatement;
    stlInt32        i;

    sSessionEnv = SSL_SESS_ENV( aEnv );
    
    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( sstFind( aStatementId,
                      &sStatement,
                      aEnv ) == STL_SUCCESS );

    for( i = 0; i < aParamCount; i++ )
    {
        STL_TRY( knlCreateOrReplaceBindParam( sStatement->mBindContext,
                                              &sStatement->mParamRegionMem,
                                              i + 1,
                                              KNL_BIND_TYPE_IN,
                                              aValueBlock[i],
                                              NULL,
                                              KNL_ENV(aEnv) ) == STL_SUCCESS );
    }


    sStatement->mNeedOptimization = STL_TRUE;
    sStatement->mOptimizeDecision |= SSL_OPTIMIZE_DECISION_BIND_INFO;
    
    return STL_SUCCESS;

    STL_FINISH;

    sslSetCallStack( aEnv );

    return STL_FAILURE;
}

stlStatus sslSetParameterData4Imp( stlInt64             aStatementId,
                                   stlInt32             aParamCount,
                                   dtlValueBlockList ** aValueBlock,
                                   sslEnv             * aEnv )
{
    sslSessionEnv     * sSessionEnv;
    sslStatement      * sStatement;
    knlBindParam      * sBindParam;
    knlValueBlockList * sValueBlock;
    stlInt32            i;

    sSessionEnv = SSL_SESS_ENV( aEnv );
    
    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( sstFind( aStatementId,
                      &sStatement,
                      aEnv ) == STL_SUCCESS );

    for( i = 0; i < aParamCount; i++ )
    {
        sValueBlock = aValueBlock[i];
        
        sBindParam = sStatement->mBindContext->mMap[i];
    
        sBindParam->mINValueBlock = sValueBlock;
        
        sBindParam->mExecuteValueBlockList = *sValueBlock;
        sBindParam->mExecuteValueBlockList.mValueBlock = &(sBindParam->mExecuteValueBlock);
        sBindParam->mExecuteValueBlock = *(sValueBlock->mValueBlock);
    }

    return STL_SUCCESS;

    STL_FINISH;

    sslSetCallStack( aEnv );

    return STL_FAILURE;
}

stlStatus sslGetParameterData( stlInt64         aStatementId,
                               stlInt32         aParamId,
                               dtlDataValue  ** aINBindValue,
                               dtlDataValue  ** aOUTBindValue,
                               sslEnv         * aEnv )
{
    sslSessionEnv     * sSessionEnv;
    sslStatement      * sStatement;
    knlValueBlockList * sINValueBlock = NULL;
    knlValueBlockList * sOUTValueBlock = NULL;

    sSessionEnv = SSL_SESS_ENV( aEnv );
    
    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( sstFind( aStatementId,
                      &sStatement,
                      aEnv ) == STL_SUCCESS );

    STL_TRY( knlGetParamValueBlock( sStatement->mBindContext,
                                    aParamId,
                                    &sINValueBlock,
                                    &sOUTValueBlock,
                                    KNL_ERROR_STACK(aEnv) ) == STL_SUCCESS );

    if( aINBindValue != NULL )
    {
        if( sINValueBlock != NULL )
        {
            *aINBindValue = DTL_GET_BLOCK_FIRST_DATA_VALUE( sINValueBlock );
        }
        else
        {
            *aINBindValue = NULL;
        }
    }

    if( aOUTBindValue != NULL )
    {
        if( sOUTValueBlock != NULL )
        {
            *aOUTBindValue = DTL_GET_BLOCK_FIRST_DATA_VALUE( sOUTValueBlock );
        }
        else
        {
            *aOUTBindValue = NULL;
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    sslSetCallStack( aEnv );

    return STL_FAILURE;
}

stlStatus sslReallocLongVaryingMem( dtlDataValue  * aBindValue,
                                    sslEnv        * aEnv )
{
    sslSessionEnv  * sSessionEnv;

    sSessionEnv = SSL_SESS_ENV( aEnv );
    
    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( knlReallocLongVaryingMem( aBindValue->mLength,
                                       (void**)&aBindValue->mValue,
                                       KNL_ENV(aEnv) )
             == STL_SUCCESS );

    aBindValue->mBufferSize = aBindValue->mLength;

    return STL_SUCCESS;

    STL_FINISH;

    sslSetCallStack( aEnv );

    return STL_FAILURE;
}

stlStatus sslPrepare( stlChar              * aSQLTextString,
                      stlInt64               aStatementId,
                      stlBool              * aHasTransaction,
                      stlInt32             * aStatementType,
                      stlBool              * aResultSet,
                      stlBool              * aWithoutHoldCursor,
                      stlBool              * aIsUpdatable,
                      ellCursorSensitivity * aSensitivity,
                      sslEnv               * aEnv )
{
    sslSessionEnv    * sSessionEnv;
    sslStatement     * sStatement;
    qllNodeType        sStatementType;

    sSessionEnv = SSL_SESS_ENV( aEnv );
    
    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( knlBreakPointWaitToStart( KNL_ENV(aEnv) ) == STL_SUCCESS );

    STL_TRY( sstFind( aStatementId,
                      &sStatement,
                      aEnv ) == STL_SUCCESS );


    /**
     * Idle Timeout을 비활성화 시킨다.
     */
    knlDisableIdleTimeout( KNL_ENV(aEnv) );

    STL_TRY( knlClearRegionMem( &sStatement->mColumnRegionMem,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    sStatement->mRowsetFirstRow   = SSL_CURSOR_POSITION_BEFORE_FIRST;
    sStatement->mRowsetCurrentPos = 0;
    sStatement->mRowsetLastPos    = 0;
    sStatement->mTargetChanged    = STL_TRUE;
    sStatement->mTargetInfo       = NULL;

    STL_TRY( sscPrepare( aSQLTextString,
                         sStatement,
                         aHasTransaction,
                         &sStatementType,
                         aResultSet,
                         aWithoutHoldCursor,
                         aIsUpdatable,
                         aSensitivity,
                         aEnv ) == STL_SUCCESS );

    if( aStatementType != NULL )
    {
        *aStatementType = sslGetStmtType( sStatementType );
    }

    if( STL_HAS_ERROR( KNL_ERROR_STACK(aEnv) ) == STL_TRUE )
    {
        sslSetCallStack( aEnv );
    }
    
    /**
     * Idle Timeout을 활성화 시킨다.
     */
    knlEnableIdleTimeout( KNL_ENV(aEnv) );

    return STL_SUCCESS;

    STL_FINISH;

    sslSetCallStack( aEnv );

    /**
     * Idle Timeout을 활성화 시킨다.
     */
    knlEnableIdleTimeout( KNL_ENV(aEnv) );

    return STL_FAILURE;
}

stlStatus sslExecute( stlInt64         aStatementId,
                      stlBool          aIsFirst,
                      stlBool        * aHasTransaction,
                      stlBool        * aIsRecompile,
                      stlBool        * aResultSet,
                      stlInt64       * aAffectedRowCount,
                      sslDCLContext  * aDCLContext,
                      sslEnv         * aEnv )
{
    sslSessionEnv * sSessionEnv;
    sslStatement  * sStatement;

    sSessionEnv = SSL_SESS_ENV( aEnv );
    
    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( knlBreakPointWaitToStart( (knlEnv*)aEnv ) == STL_SUCCESS );

    STL_TRY( sstFind( aStatementId,
                      &sStatement,
                      aEnv ) == STL_SUCCESS );

    /**
     * Idle Timeout을 비활성화 시킨다.
     */
    knlDisableIdleTimeout( KNL_ENV(aEnv) );

    /**
     * Query Timeout 활성화
     */
    knlEnableQueryTimeout( KNL_ENV(aEnv),
                           sStatement->mQueryTimeout );

    /**
     * Session Property 값으로 DBC Statement 의 Block Size 결정
     * - ODBC Statement 의 관련 Attribute 변경시 반영되어야 함.
     */

    sStatement->mRowsetFirstRow   = SSL_CURSOR_POSITION_BEFORE_FIRST;
    sStatement->mRowsetCurrentPos = 0;
    sStatement->mRowsetLastPos    = 0;
    sStatement->mCurrArraySize    = sStatement->mNextArraySize;
    
    STL_TRY( sscExecute( sStatement,
                         aIsFirst,
                         aHasTransaction,
                         aIsRecompile,
                         aResultSet,
                         aAffectedRowCount,
                         aDCLContext,
                         aEnv )
             == STL_SUCCESS );

    /**
     * Idle Timeout을 활성화 시킨다.
     */
    knlEnableIdleTimeout( KNL_ENV(aEnv) );

    /**
     * Query Timeout 비활성화
     */
    knlDisableQueryTimeout( KNL_ENV(aEnv) );

    if( STL_HAS_ERROR( KNL_ERROR_STACK(aEnv) ) == STL_TRUE )
    {
        sslSetCallStack( aEnv );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    sslSetCallStack( aEnv );

    /**
     * Query Timeout 비활성화
     */
    knlDisableQueryTimeout( KNL_ENV(aEnv) );
    
    /**
     * Idle Timeout을 활성화 시킨다.
     */
    knlEnableIdleTimeout( KNL_ENV(aEnv) );

    return STL_FAILURE;
}

stlStatus sslExecDirect( stlChar              * aSQLTextString,
                         stlInt64               aStatementId,
                         stlBool                aIsFirst,
                         stlBool              * aHasTransaction,
                         stlBool              * aIsRecompile,
                         stlInt32             * aStatementType,
                         stlBool              * aResultSet,
                         stlBool              * aWithoutHoldCursor,
                         stlBool              * aIsUpdatable,
                         ellCursorSensitivity * aSensitivity,
                         stlInt64             * aAffectedRowCount,
                         sslDCLContext        * aDCLContext,
                         sslEnv               * aEnv )
{
    sslSessionEnv * sSessionEnv;
    sslStatement  * sStatement;
    qllNodeType     sStatementType;

    sSessionEnv = SSL_SESS_ENV( aEnv );
    
    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( knlBreakPointWaitToStart( (knlEnv*)aEnv ) == STL_SUCCESS );

    STL_TRY( sstFind( aStatementId,
                      &sStatement,
                      aEnv ) == STL_SUCCESS );

    /**
     * Idle Timeout을 비활성화 시킨다.
     */
    knlDisableIdleTimeout( KNL_ENV(aEnv) );

    /**
     * Query Timeout 활성화
     */
    knlEnableQueryTimeout( KNL_ENV(aEnv),
                           sStatement->mQueryTimeout );

    STL_TRY( knlClearRegionMem( &sStatement->mColumnRegionMem,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Session Property 값으로 DBC Statement 의 Block Size 결정
     * - ODBC Statement 의 관련 Attribute 변경시 반영되어야 함.
     */
    
    sStatement->mRowsetFirstRow   = SSL_CURSOR_POSITION_BEFORE_FIRST;
    sStatement->mRowsetCurrentPos = 0;
    sStatement->mRowsetLastPos    = 0;
    sStatement->mTargetChanged    = STL_TRUE;
    sStatement->mTargetInfo       = NULL;
    sStatement->mCurrArraySize    = sStatement->mNextArraySize;
    
    STL_TRY( sscExecDirect( aSQLTextString,
                            sStatement,
                            aIsFirst,
                            aHasTransaction,
                            aIsRecompile,
                            &sStatementType,
                            aResultSet,
                            aWithoutHoldCursor,
                            aIsUpdatable,
                            aSensitivity,
                            aAffectedRowCount,
                            aDCLContext,
                            aEnv )
             == STL_SUCCESS );

    if( aStatementType != NULL )
    {
        *aStatementType = sslGetStmtType( sStatementType );
    }

    /**
     * Idle Timeout을 활성화 시킨다.
     */
    knlEnableIdleTimeout( KNL_ENV(aEnv) );

    /**
     * Query Timeout 비활성화
     */
    knlDisableQueryTimeout( KNL_ENV(aEnv) );

    if( STL_HAS_ERROR( KNL_ERROR_STACK(aEnv) ) == STL_TRUE )
    {
        sslSetCallStack( aEnv );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    sslSetCallStack( aEnv );

    /**
     * Query Timeout 비활성화
     */
    knlDisableQueryTimeout( KNL_ENV(aEnv) );
    
    /**
     * Idle Timeout을 활성화 시킨다.
     */
    knlEnableIdleTimeout( KNL_ENV(aEnv) );

    return STL_FAILURE;
}

stlStatus sslNumParams( stlInt64    aStatementId,
                        stlInt16  * aParameterCount,
                        sslEnv    * aEnv )
{
    sslSessionEnv * sSessionEnv;
    sslStatement  * sStatement;

    sSessionEnv = SSL_SESS_ENV( aEnv );
    
    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( sstFind( aStatementId,
                      &sStatement,
                      aEnv ) == STL_SUCCESS );

    /**
     * Idle Timeout을 비활성화 시킨다.
     */
    knlDisableIdleTimeout( KNL_ENV(aEnv) );

    STL_TRY( sscNumParams( sStatement,
                           aParameterCount,
                           aEnv ) == STL_SUCCESS );

    /**
     * Idle Timeout을 활성화 시킨다.
     */
    knlEnableIdleTimeout( KNL_ENV(aEnv) );

    return STL_SUCCESS;

    STL_FINISH;

    sslSetCallStack( aEnv );
    
    /**
     * Idle Timeout을 활성화 시킨다.
     */
    knlEnableIdleTimeout( KNL_ENV(aEnv) );

    return STL_FAILURE;
}


stlStatus sslNumResultCols( stlInt64     aStatementId,
                            stlInt16   * aColumnCount,
                            sslEnv     * aEnv )
{
    sslSessionEnv * sSessionEnv;
    sslStatement  * sStatement;

    sSessionEnv = SSL_SESS_ENV( aEnv );
    
    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( sstFind( aStatementId,
                      &sStatement,
                      aEnv ) == STL_SUCCESS );

    /**
     * Idle Timeout을 비활성화 시킨다.
     */
    knlDisableIdleTimeout( KNL_ENV(aEnv) );

    STL_TRY( sscNumResultCols( sStatement,
                               aColumnCount,
                               aEnv ) == STL_SUCCESS );

    /**
     * Idle Timeout을 활성화 시킨다.
     */
    knlEnableIdleTimeout( KNL_ENV(aEnv) );

    return STL_SUCCESS;

    STL_FINISH;

    sslSetCallStack( aEnv );
    
    /**
     * Idle Timeout을 활성화 시킨다.
     */
    knlEnableIdleTimeout( KNL_ENV(aEnv) );

    return STL_FAILURE;
}

stlStatus sslDescribeCol( stlInt64     aStatementId,
                          qllTarget ** aTargetInfo,
                          sslEnv     * aEnv )
{
    sslSessionEnv * sSessionEnv;
    sslStatement  * sStatement;

    sSessionEnv = SSL_SESS_ENV( aEnv );
    
    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( sstFind( aStatementId,
                      &sStatement,
                      aEnv ) == STL_SUCCESS );

    /**
     * Idle Timeout을 비활성화 시킨다.
     */
    knlDisableIdleTimeout( KNL_ENV(aEnv) );

    STL_TRY( sscDescribeCol( sStatement,
                             aTargetInfo,
                             aEnv ) == STL_SUCCESS );

    /**
     * Idle Timeout을 활성화 시킨다.
     */
    knlEnableIdleTimeout( KNL_ENV(aEnv) );

    return STL_SUCCESS;

    STL_FINISH;

    sslSetCallStack( aEnv );
    
    /**
     * Idle Timeout을 활성화 시킨다.
     */
    knlEnableIdleTimeout( KNL_ENV(aEnv) );

    return STL_FAILURE;
}

stlStatus sslGetParameterType( stlInt64     aStatementId,
                               stlInt32     aParamIdx,
                               stlInt16   * aInputOutputType,
                               sslEnv     * aEnv )
{
    sslSessionEnv * sSessionEnv;
    sslStatement  * sStatement;

    sSessionEnv = SSL_SESS_ENV( aEnv );
    
    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( sstFind( aStatementId,
                      &sStatement,
                      aEnv ) == STL_SUCCESS );

    /**
     * Idle Timeout을 비활성화 시킨다.
     */
    knlDisableIdleTimeout( KNL_ENV(aEnv) );

    STL_TRY( sscDescribeParam( sStatement,
                               aParamIdx,
                               aInputOutputType,
                               aEnv ) == STL_SUCCESS );

    /**
     * Idle Timeout을 활성화 시킨다.
     */
    knlEnableIdleTimeout( KNL_ENV(aEnv) );

    return STL_SUCCESS;

    STL_FINISH;

    sslSetCallStack( aEnv );
    
    /**
     * Idle Timeout을 활성화 시킨다.
     */
    knlEnableIdleTimeout( KNL_ENV(aEnv) );

    return STL_FAILURE;
}

stlStatus sslGetTargetType( stlInt64             aStatementId,
                            stlBool            * aChanged,
                            knlValueBlockList ** aTargetBlockList,
                            sslEnv             * aEnv )
{
    sslSessionEnv * sSessionEnv;
    sslStatement  * sStatement;
    void          * sDataPlan = NULL;
    stlBool         sNeedFetch = STL_FALSE;

    sSessionEnv = SSL_SESS_ENV( aEnv );
    
    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( sstFind( aStatementId,
                      &sStatement,
                      aEnv ) == STL_SUCCESS );

    /**
     * Idle Timeout을 비활성화 시킨다.
     */
    knlDisableIdleTimeout( KNL_ENV(aEnv) );

    *aTargetBlockList = NULL;
    
    sNeedFetch = qllNeedFetch( &sStatement->mSqlStmt );
    sDataPlan  = sStatement->mSqlStmt.mDataPlan;

    if( ( sNeedFetch == STL_TRUE) &&
        ( sDataPlan != NULL ) )        
    {
        if( ( sStatement->mTargetChanged == STL_TRUE ) ||
            ( sStatement->mIsRecompile == STL_TRUE ) )
        {
            *aChanged = STL_TRUE;
            *aTargetBlockList = ((qllDataArea*)sDataPlan)->mTargetBlock;
    
            sStatement->mTargetChanged = STL_FALSE;
        }
        else
        {
            *aChanged = STL_FALSE;
        }
    }

    /**
     * Idle Timeout을 활성화 시킨다.
     */
    knlEnableIdleTimeout( KNL_ENV(aEnv) );

    return STL_SUCCESS;

    STL_FINISH;

    sslSetCallStack( aEnv );
    
    /**
     * Idle Timeout을 활성화 시킨다.
     */
    knlEnableIdleTimeout( KNL_ENV(aEnv) );

    return STL_FAILURE;
}
                    
stlStatus sslSetFetchPosition( stlInt64     aStatementId,
                               stlInt64     aRowsetFirstRow,
                               stlInt64     aRowsetCurrentPos,
                               stlInt64     aRowsetLastPos,
                               sslEnv     * aEnv )
{
    sslSessionEnv * sSessionEnv;
    sslStatement  * sStatement;

    sSessionEnv = SSL_SESS_ENV( aEnv );
    
    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( sstFind( aStatementId,
                      &sStatement,
                      aEnv ) == STL_SUCCESS );

    sStatement->mRowsetFirstRow   = aRowsetFirstRow;
    sStatement->mRowsetCurrentPos = aRowsetCurrentPos;
    sStatement->mRowsetLastPos    = aRowsetLastPos;

    return STL_SUCCESS;

    STL_FINISH;

    sslSetCallStack( aEnv );
    
    return STL_FAILURE;
}

stlStatus sslGetFetchPosition( stlInt64                  aStatementId,
                               stlInt64                * aRowsetFirstRow,
                               stlInt64                * aRowsetCurrentPos,
                               stlInt64                * aRowsetLastPos,
                               stlInt64                * aKnownLastRowIdx,
                               ellCursorSensitivity    * aSensitivity,
                               ellCursorUpdatability   * aUpdatability,
                               ellCursorScrollability  * aScrollability,
                               knlValueBlockList      ** aTargetBlock,
                               sslEnv                  * aEnv )
{
    sslSessionEnv          * sSessionEnv;
    sslStatement           * sStatement;
    qllResultSetDesc       * sCursor;

    sSessionEnv = SSL_SESS_ENV( aEnv );
    
    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( sstFind( aStatementId,
                      &sStatement,
                      aEnv ) == STL_SUCCESS );

    *aRowsetFirstRow   = sStatement->mRowsetFirstRow;
    *aRowsetCurrentPos = sStatement->mRowsetCurrentPos;
    *aRowsetLastPos    = sStatement->mRowsetLastPos;
    *aTargetBlock      = NULL;

    if( sStatement->mCursorStmt != NULL )
    {
        sCursor = sStatement->mResultSetCursor;
        
        if( sCursor == NULL )
        {
            sCursor = qllGetCursorResultSetDesc( &sStatement->mSqlStmt );
        }

        *aTargetBlock = sCursor->mTargetBlock;

        if( sCursor->mCursorProperty.mSensitivity == ELL_CURSOR_SENSITIVITY_SENSITIVE )
        {
            *aSensitivity = ELL_CURSOR_SENSITIVITY_SENSITIVE;
        }
        else
        {
            *aSensitivity = ELL_CURSOR_SENSITIVITY_INSENSITIVE;
        }

        if( sCursor->mCursorProperty.mUpdatability == ELL_CURSOR_UPDATABILITY_FOR_UPDATE )
        {
            *aUpdatability = ELL_CURSOR_UPDATABILITY_FOR_UPDATE;
        }
        else
        {
            *aUpdatability = ELL_CURSOR_UPDATABILITY_FOR_READ_ONLY;
        }

        if( sCursor->mCursorProperty.mScrollability == ELL_CURSOR_SCROLLABILITY_SCROLL )
        {
            *aScrollability = ELL_CURSOR_SCROLLABILITY_SCROLL;
        }
        else
        {
            *aScrollability = ELL_CURSOR_SCROLLABILITY_NO_SCROLL;
        }

        if( sCursor->mKnownLastPosition == STL_TRUE )
        {
            *aKnownLastRowIdx = sCursor->mLastPositionIdx;
        }
        else
        {
            *aKnownLastRowIdx = SSL_LAST_ROW_IDX_UNKNOWN;
        }
    }
    else
    {
        *aSensitivity = ELL_CURSOR_SENSITIVITY_NA;
        *aKnownLastRowIdx = SSL_LAST_ROW_IDX_UNKNOWN;
    }

    return STL_SUCCESS;

    STL_FINISH;

    sslSetCallStack( aEnv );
    
    return STL_FAILURE;
}

stlStatus sslFetch( stlInt64             aStatementId,
                    stlInt64             aOffset,
                    knlValueBlockList ** aTargetBuffer,
                    stlBool            * aIsEndOfScan,
                    stlBool            * aIsBlockRead,
                    stlInt32           * aOneRowIdx,
                    qllCursorRowStatus * aOneRowStatus,
                    stlInt64           * aAbsIdxOfFirstRow,
                    stlInt64           * aKnownLastRowIdx,
                    sslEnv             * aEnv )
{
    sslSessionEnv * sSessionEnv;
    sslStatement  * sStatement;

    sSessionEnv = SSL_SESS_ENV( aEnv );
    
    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlCheckEndStatement( KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( sstFind( aStatementId,
                      &sStatement,
                      aEnv ) == STL_SUCCESS );

    /**
     * Idle Timeout을 비활성화 시킨다.
     */
    knlDisableIdleTimeout( KNL_ENV(aEnv) );

    /**
     * Query Timeout 활성화
     */
    knlEnableQueryTimeout( KNL_ENV(aEnv),
                           sStatement->mQueryTimeout );

    STL_TRY( sscFetch( sStatement,
                       aOffset,
                       aTargetBuffer,
                       aIsEndOfScan,
                       aIsBlockRead,
                       aOneRowIdx,
                       aOneRowStatus,
                       aAbsIdxOfFirstRow,
                       aKnownLastRowIdx,
                       aEnv ) == STL_SUCCESS );

    /**
     * Idle Timeout을 활성화 시킨다.
     */
    knlEnableIdleTimeout( KNL_ENV(aEnv) );

    /**
     * Query Timeout 비활성화
     */
    knlDisableQueryTimeout( KNL_ENV(aEnv) );

    return STL_SUCCESS;

    STL_FINISH;

    sslSetCallStack( aEnv );
    
    /**
     * Idle Timeout을 활성화 시킨다.
     */
    knlEnableIdleTimeout( KNL_ENV(aEnv) );

    return STL_FAILURE;
}

stlStatus sslCloseCursor( stlInt64    aStatementId,
                          sslEnv    * aEnv )
{
    sslSessionEnv * sSessionEnv;
    sslStatement  * sStatement;

    sSessionEnv = SSL_SESS_ENV( aEnv );
    
    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( sstFind( aStatementId,
                      &sStatement,
                      aEnv ) == STL_SUCCESS );

    /**
     * Idle Timeout을 비활성화 시킨다.
     */
    knlDisableIdleTimeout( KNL_ENV(aEnv) );

    STL_TRY( sscCloseCursor( sStatement,
                             aEnv ) == STL_SUCCESS );

    /**
     * Idle Timeout을 활성화 시킨다.
     */
    knlEnableIdleTimeout( KNL_ENV(aEnv) );

    return STL_SUCCESS;

    STL_FINISH;

    sslSetCallStack( aEnv );
    
    /**
     * Idle Timeout을 활성화 시킨다.
     */
    knlEnableIdleTimeout( KNL_ENV(aEnv) );

    return STL_FAILURE;
}

stlStatus sslSetCursorName( stlInt64   aStatementId,
                            stlChar  * aCursorName,
                            sslEnv   * aEnv )
{
    sslSessionEnv * sSessionEnv;
    sslStatement  * sStatement;

    sSessionEnv = SSL_SESS_ENV( aEnv );
    
    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( sstFind( aStatementId,
                      &sStatement,
                      aEnv ) == STL_SUCCESS );

    /**
     * Idle Timeout을 비활성화 시킨다.
     */
    knlDisableIdleTimeout( KNL_ENV(aEnv) );

    STL_TRY( sscSetCursorName( sStatement,
                               aCursorName,
                               aEnv ) == STL_SUCCESS );

    /**
     * Idle Timeout을 활성화 시킨다.
     */
    knlEnableIdleTimeout( KNL_ENV(aEnv) );

    return STL_SUCCESS;

    STL_FINISH;

    sslSetCallStack( aEnv );
    
    /**
     * Idle Timeout을 활성화 시킨다.
     */
    knlEnableIdleTimeout( KNL_ENV(aEnv) );

    return STL_FAILURE;
}

stlStatus sslGetCursorName( stlInt64    aStatementId,
                            stlChar   * aCursorName,
                            sslEnv    * aEnv )
{
    sslSessionEnv * sSessionEnv;
    sslStatement  * sStatement;

    sSessionEnv = SSL_SESS_ENV( aEnv );
    
    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( sstFind( aStatementId,
                      &sStatement,
                      aEnv ) == STL_SUCCESS );

    /**
     * Idle Timeout을 비활성화 시킨다.
     */
    knlDisableIdleTimeout( KNL_ENV(aEnv) );

    STL_TRY( sscGetCursorName( sStatement,
                               aCursorName,
                               aEnv ) == STL_SUCCESS );

    /**
     * Idle Timeout을 활성화 시킨다.
     */
    knlEnableIdleTimeout( KNL_ENV(aEnv) );

    return STL_SUCCESS;

    STL_FINISH;

    sslSetCallStack( aEnv );
    
    /**
     * Idle Timeout을 활성화 시킨다.
     */
    knlEnableIdleTimeout( KNL_ENV(aEnv) );

    return STL_FAILURE;
}

stlStatus sslSetStmtAttr( stlInt64           aStatementId,
                          sslStatementAttr   aAttribute,
                          stlInt64           aIntValue,
                          stlChar          * aStrValue,
                          stlInt32           aStrLength,
                          sslEnv           * aEnv )
{
    sslSessionEnv  * sSessionEnv;
    sslStatement   * sStatement;

    sSessionEnv = SSL_SESS_ENV( aEnv );
    
    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( sstFind( aStatementId,
                      &sStatement,
                      aEnv ) == STL_SUCCESS );

    /**
     * Idle Timeout을 비활성화 시킨다.
     */
    knlDisableIdleTimeout( KNL_ENV(aEnv) );

    switch( aAttribute )
    {
        case SSL_STATEMENT_ATTR_PARAMSET_SIZE :
            STL_TRY_THROW( aIntValue > 0, RAMP_ERR_INVALID_ATTRIBUTE_VALUE );
            sStatement->mNextArraySize = aIntValue;
            sStatement->mNeedOptimization = STL_TRUE;
            sStatement->mOptimizeDecision |= SSL_OPTIMIZE_DECISION_ARRAY_SIZE;
            break;
        case SSL_STATEMENT_ATTR_CONCURRENCY :
            sStatement->mUpdatability = (ellCursorUpdatability)aIntValue;
            break;
        case SSL_STATEMENT_ATTR_CURSOR_HOLDABILITY :
            sStatement->mHoldability = (ellCursorHoldability)aIntValue;
            break;
        case SSL_STATEMENT_ATTR_CURSOR_SCROLLABLE :
            sStatement->mScrollability = (ellCursorScrollability)aIntValue;
            break;
        case SSL_STATEMENT_ATTR_CURSOR_SENSITIVITY :
            sStatement->mSensitivity = (ellCursorSensitivity)aIntValue;
            break;
        case SSL_STATEMENT_ATTR_CURSOR_TYPE :
            sStatement->mCursorType = (ellCursorStandardType)aIntValue;
            break;
        case SSL_STATEMENT_ATTR_ATOMIC_EXECUTION :
            switch( aIntValue )
            {
                case SSL_ATOMIC_EXECUTION_OFF :
                case SSL_ATOMIC_EXECUTION_ON :
                    sStatement->mAtomicExecute = (sslAtomicExecution)aIntValue;
                    sStatement->mNeedOptimization = STL_TRUE;
                    sStatement->mOptimizeDecision |= SSL_OPTIMIZE_DECISION_ATOMIC_EXECUTION;
                    break;
                default:
                    STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_VALUE );
                    break;
            }
            break;
        case SSL_STATEMENT_ATTR_EXPLAIN_PLAN_OPTION :
            switch( aIntValue )
            {
                case SSL_EXPLAIN_PLAN_OFF        :
                case SSL_EXPLAIN_PLAN_ON         :
                case SSL_EXPLAIN_PLAN_ON_VERBOSE :
                case SSL_EXPLAIN_PLAN_ONLY       :
                    sStatement->mExplainType = (sslExplainType)aIntValue;
                    break;
                default:
                    STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_VALUE );
                    break;
            }
            break;
        case SSL_STATEMENT_ATTR_QUERY_TIMEOUT :
            sStatement->mQueryTimeout = aIntValue;
            break;
        default :
            STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER );
            break;
    }

    /**
     * Idle Timeout을 활성화 시킨다.
     */
    knlEnableIdleTimeout( KNL_ENV(aEnv) );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_ATTRIBUTE_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SSL_ERRCODE_INVALID_ATTRIBUTE_VALUE,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SSL_ERRCODE_INVALID_ATTRIBUTE_OPTION_IDENTIFIER,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    sslSetCallStack( aEnv );
    
    /**
     * Idle Timeout을 활성화 시킨다.
     */
    knlEnableIdleTimeout( KNL_ENV(aEnv) );

    return STL_FAILURE;    
}
                            
static stlStatus sslMakePlanText( sslStatement  * sStatement,
                                  stlChar       * aPlanText,
                                  stlInt32        aBufferLen,
                                  stlInt32      * aNeedLen,
                                  stlErrorStack * aErrorStack )
{
    qllExplainNodeText * sNodeText;
    qllExplainPredText * sPredText;
    stlInt32             sLineTextLen;
    stlInt32             sTotalTextLen = 0;
    stlInt32             sRemainLen    = aBufferLen;
    stlChar            * sCurBuf       = aPlanText;

    /*
     * QP에서 생성한 Plan Text 가져오기
     */
    sNodeText = sStatement->mSqlStmt.mExplainPlanText;

    if( sNodeText == NULL )
    {
        *aNeedLen = 0;
    }
    else
    {
        /*
         * node 출력
         */
        do
        {
            sLineTextLen = stlStrlen( sNodeText->mLineText );
            sTotalTextLen += sLineTextLen;

            if( sRemainLen == 0 )
            {
                /* Do Nothing */
            }
            else
            {
                if( sLineTextLen < sRemainLen )
                {
                    stlMemcpy( sCurBuf, sNodeText->mLineText, sLineTextLen );
                    sCurBuf += sLineTextLen;
                    sRemainLen -= sLineTextLen;
                }
                else
                {
                    stlMemcpy( sCurBuf, sNodeText->mLineText, sRemainLen - 1 );
                    sCurBuf += sRemainLen - 1;
                    sRemainLen = 0;
                }
            }
            
            sNodeText = sNodeText->mNext;
        }
        while( sNodeText != NULL );
        
        /*
         * predicate 출력
         */
        
        sNodeText = sStatement->mSqlStmt.mExplainPlanText;
        do
        {
            sPredText = sNodeText->mPredText;
            while( sPredText != NULL )
            {
                sLineTextLen = stlStrlen( sPredText->mLineText );
                sTotalTextLen += sLineTextLen;

                if( sRemainLen == 0 )
                {
                    /* Do Nothing */
                }
                else
                {
                    if( sLineTextLen < sRemainLen )
                    {
                        stlMemcpy( sCurBuf, sPredText->mLineText, sLineTextLen );
                        sCurBuf += sLineTextLen;
                        sRemainLen -= sLineTextLen;
                    }
                    else
                    {
                        stlMemcpy( sCurBuf, sPredText->mLineText, sRemainLen - 1 );
                        sCurBuf += sRemainLen - 1;
                        sRemainLen = 0;
                    }
                }

                sPredText = sPredText->mNext;
            } 
                
            sNodeText = sNodeText->mNext;
        }
        while( sNodeText != NULL );

        /**
         * @todo cost 출력
         */

        if( aBufferLen > 0 )
        {
            sCurBuf = '\0';
        }
        else
        {
            /* Do Nothing */
        }

        *aNeedLen = sTotalTextLen;
    }
    
    return STL_SUCCESS;
}

stlStatus sslGetStmtAttr( stlInt64           aStatementId,
                          sslStatementAttr   aAttribute,
                          stlInt64         * aIntValue,
                          stlChar          * aStrValue,
                          stlInt32           aBufferLength, 
                          stlInt32         * aStrLength,
                          sslEnv           * aEnv )
{
    sslSessionEnv  * sSessionEnv;
    sslStatement   * sStatement;

    sSessionEnv = SSL_SESS_ENV( aEnv );
    
    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( sstFind( aStatementId,
                      &sStatement,
                      aEnv ) == STL_SUCCESS );
    /**
     * Idle Timeout을 비활성화 시킨다.
     */
    knlDisableIdleTimeout( KNL_ENV(aEnv) );

    switch( aAttribute )
    {
        case SSL_STATEMENT_ATTR_EXPLAIN_PLAN_TEXT :
            STL_TRY( sslMakePlanText( sStatement,
                                      aStrValue,
                                      aBufferLength,
                                      aStrLength,
                                      KNL_ERROR_STACK(aEnv) ) == STL_SUCCESS );
            break;
        case SSL_STATEMENT_ATTR_QUERY_TIMEOUT :
            *aIntValue = sStatement->mQueryTimeout;

            if( *aIntValue == KNL_DEFAULT_STATEMENT_QUERY_TIMEOUT )
            {
                /*
                 * statement query timeout이 default 값이면 session query timeout을 따른다.
                 */
                *aIntValue = knlGetPropertyBigIntValueByID( KNL_PROPERTY_QUERY_TIMEOUT,
                                                            KNL_ENV(aEnv) );
            }
            break;
        default :
            STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER );
            break;
    }

    /**
     * Idle Timeout을 활성화 시킨다.
     */
    knlEnableIdleTimeout( KNL_ENV(aEnv) );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SSL_ERRCODE_INVALID_ATTRIBUTE_OPTION_IDENTIFIER,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    sslSetCallStack( aEnv );
    
    /**
     * Idle Timeout을 활성화 시킨다.
     */
    knlEnableIdleTimeout( KNL_ENV(aEnv) );

    return STL_FAILURE;    
}
                            
stlStatus sslSetPos( stlInt64    aStatementId,
                     stlInt64    aRowNumber,
                     stlInt16    aOperation,
                     sslEnv    * aEnv )
{
    sslSessionEnv  * sSessionEnv;
    sslStatement   * sStatement;

    sSessionEnv = SSL_SESS_ENV( aEnv );
    
    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( sstFind( aStatementId,
                      &sStatement,
                      aEnv ) == STL_SUCCESS );

    /**
     * Idle Timeout을 비활성화 시킨다.
     */
    knlDisableIdleTimeout( KNL_ENV(aEnv) );

    switch( aOperation )
    {
        case SSL_SET_POS_POSITION :
            STL_TRY( sscSetPos( sStatement,
                                aRowNumber,
                                aOperation,
                                aEnv ) == STL_SUCCESS );
            break;
        default :
            STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER );
            break;
    }

    /**
     * Idle Timeout을 활성화 시킨다.
     */
    knlEnableIdleTimeout( KNL_ENV(aEnv) );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SSL_ERRCODE_INVALID_ATTRIBUTE_OPTION_IDENTIFIER,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    sslSetCallStack( aEnv );
    
    /**
     * Idle Timeout을 활성화 시킨다.
     */
    knlEnableIdleTimeout( KNL_ENV(aEnv) );

    return STL_FAILURE;    
}

stlStatus sslGetProperty( knlPropertyID   aPropertyID,
                          void          * aValue,
                          sslEnv        * aEnv )
{
    sslSessionEnv * sSessionEnv;

    sSessionEnv = SSL_SESS_ENV( aEnv );

    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    /**
     * Idle Timeout을 비활성화 시킨다.
     */
    knlDisableIdleTimeout( KNL_ENV(aEnv) );

    STL_TRY( knlGetPropertyValueByID( aPropertyID,
                                      aValue,
                                      KNL_ENV( aEnv ) ) == STL_SUCCESS );

    /**
     * Idle Timeout을 활성화 시킨다.
     */
    knlEnableIdleTimeout( KNL_ENV(aEnv) );

    return STL_SUCCESS;

    STL_FINISH;

    sslSetCallStack( aEnv );
    
    /**
     * Idle Timeout을 활성화 시킨다.
     */
    knlEnableIdleTimeout( KNL_ENV(aEnv) );

    return STL_FAILURE;    
}

void sslSetSessionProcessInfo( stlProc        * aProcessId,
                               stlChar        * aProcessName,
                               sslSessionEnv  * aSessionEnv )
{
    aSessionEnv->mClientProcId = *aProcessId;
    
    stlStrncpy( aSessionEnv->mProgram, aProcessName, SSL_PROGRAM_NAME );
}

stlStatus sslEnterSession( stlUInt32       aSessionId,
                           stlInt64        aSessionSeq,
                           stlErrorStack * aErrorStack )
{
    stlUInt64       sCompareKey;
    stlUInt64       sWithKey;
    stlUInt64       sOldAccessKey;
    stlUInt64     * sAccessKey;
    sslSessionEnv * sSessionEnv;
    
    sSessionEnv = knlGetSessionEnv( aSessionId );
    
    /**
     * validate session access key
     */

    sCompareKey = KNL_MAKE_SESSION_ACCESS_KEY( KNL_SESSION_ACCESS_LEAVE, aSessionSeq );
    sWithKey    = KNL_MAKE_SESSION_ACCESS_KEY( KNL_SESSION_ACCESS_ENTER, aSessionSeq );
    sAccessKey  = KNL_GET_SESSION_ACCESS_KEY_POINTER( sSessionEnv );
    
    sOldAccessKey = stlAtomicCas64( sAccessKey,
                                    sWithKey,
                                    sCompareKey );

    /**
     * Session이 aging되었거나 재사용되었다.
     */
    STL_TRY_THROW( sOldAccessKey == sCompareKey,
                   RAMP_ERR_COMMUNICATION_LINK_FAILURE );
        
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_COMMUNICATION_LINK_FAILURE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SSL_ERRCODE_COMMUNICATION_LINK_FAILURE,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;    
}

void sslLeaveSession( stlUInt32  aSessionId,
                      stlInt64   aSessionSeq )
{
    stlUInt64       sCompareKey;
    stlUInt64       sWithKey;
    stlUInt64       sOldAccessKey;
    stlUInt64     * sAccessKey;
    stlUInt64       sSessionSeq;
    stlBool         sHasPendingSignal;
    stlInt32        sPendingSignalNo;
    sslSessionEnv * sSessionEnv;

    sSessionEnv = knlGetSessionEnv( aSessionId );
    
    sHasPendingSignal = KNL_HAS_PENDING_SIGNAL( sSessionEnv );
    sPendingSignalNo = KNL_PENDING_SIGNAL_NO( sSessionEnv );
    
    while( 1 )
    {
        sSessionSeq = KNL_GET_SESSION_SEQ( sSessionEnv );
        sCompareKey = KNL_MAKE_SESSION_ACCESS_KEY( KNL_SESSION_ACCESS_ENTER, sSessionSeq );
        sWithKey    = KNL_MAKE_SESSION_ACCESS_KEY( KNL_SESSION_ACCESS_LEAVE, sSessionSeq );
        sAccessKey  = KNL_GET_SESSION_ACCESS_KEY_POINTER( sSessionEnv );
    
        sOldAccessKey = stlAtomicCas64( sAccessKey,
                                        sWithKey,
                                        sCompareKey );

        if( sOldAccessKey == sCompareKey )
        {
            break;
        }
    }

    if( sHasPendingSignal == STL_TRUE )
    {
        (void) knlExecutePendingSignal( sPendingSignalNo );
    }
}
