/*******************************************************************************
 * sscPrepare.c
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
 * @file sscPrepare.c
 * @brief Session Layer Prepare Command Internal Routines
 */


#include <qll.h>
#include <sslDef.h>
#include <ssDef.h>
#include <sslSession.h>

stlStatus sscPrepare( stlChar              * aSQLTextString,
                      sslStatement         * aStatement,
                      stlBool              * aHasTransaction,
                      qllNodeType          * aStatementType,
                      stlBool              * aResultSet,
                      stlBool              * aWithoutHoldCursor,
                      stlBool              * aIsUpdatable,
                      ellCursorSensitivity * aSensitivity,
                      sslEnv               * aEnv )
{
    sslSessionEnv     * sSessionEnv;
    qllNode           * sParseTree;
    qllAccessMode       sStmtAccessMode;
    stlBool             sSupportGlobal;
    stlBool             sAutocommit;
    stlErrorData      * sErrorData = NULL;
    
    aStatement->mIsEndOfScan = STL_FALSE;

    sSessionEnv = SSL_SESS_ENV( aEnv );

    STL_PARAM_VALIDATE( aStatement->mCursorStmt == NULL, KNL_ERROR_STACK( aEnv ) );

    /*
     * Parsing
     */

    STL_TRY( qllResetSQL( &aStatement->mDbcStmt,
                          &aStatement->mSqlStmt,
                          QLL_ENV( aEnv ) ) == STL_SUCCESS );

    qllSetSQLStmtCursorProperty( & aStatement->mSqlStmt,
                                 aStatement->mCursorType,  
                                 aStatement->mSensitivity,
                                 aStatement->mScrollability,
                                 aStatement->mHoldability,
                                 aStatement->mUpdatability,
                                 ELL_CURSOR_DEFAULT_RETURNABILITY );
    
    STL_TRY( qllParseSQL( &aStatement->mDbcStmt,
                          &aStatement->mSqlStmt,
                          aSQLTextString,
                          &sParseTree,
                          QLL_ENV( aEnv ) ) == STL_SUCCESS );

    /**
     * Memory Allocator를 선정한다.
     */
    
    if( qllIsCacheableStmt( &(aStatement->mSqlStmt), QLL_ENV(aEnv) ) == STL_TRUE )
    {
        qllPlanCacheMemON( &(aStatement->mDbcStmt) );
    }
    else
    {
        qllPlanCacheMemOFF( &(aStatement->mDbcStmt) );
    }
    
    sStmtAccessMode = qllGetAccessMode( &aStatement->mSqlStmt );
    sAutocommit     = qllNeedAutocommit( &aStatement->mSqlStmt );
    sSupportGlobal  = qllSupportGlobalTransaction( &aStatement->mSqlStmt );

    if( knlGetTransScope( sSessionEnv ) == KNL_TRANS_SCOPE_GLOBAL )
    {
        STL_TRY_THROW( sSupportGlobal == STL_TRUE, RAMP_ERR_NOT_ALLOW_COMMAND_IN_XA );
    }
    
    if( (sAutocommit == STL_FALSE) &&
        (qllIsDDL( &aStatement->mSqlStmt ) == STL_TRUE) )
    {
        /**
         * Autocommit을 강제하는 DDL Statement가 아닌 경우는 Property를 따른다.
         */
        sAutocommit = knlGetPropertyBoolValueByID( KNL_PROPERTY_DDL_AUTOCOMMIT,
                                                   KNL_ENV(aEnv) );
    }
    
    /**
     * pre-commit을 해야 한다면, Oracle 과 마찬가지로 Validation 전에 COMMIT 해야 함.
     * - Validation 과정에서 얻은 Handle 정보가 Validation 후에 COMMIT 이 발생하면 유효하지 않게 됨.
     */
    
    if( sAutocommit == STL_TRUE )
    {
        /**
         * 이전 Statement들을 정리한다.
         */
        if( sSessionEnv->mTransId != SML_INVALID_TRANSID )
        {
            STL_TRY( qllCommit( sSessionEnv->mTransId,
                                NULL, /* aComment */
                                SML_TRANSACTION_CWM_NONE,
                                NULL, /* aXaContext */
                                QLL_ENV( aEnv ) )
                     == STL_SUCCESS );
            
            sSessionEnv->mTransId = SML_INVALID_TRANSID;
            knlSetTransScope( sSessionEnv, KNL_TRANS_SCOPE_NONE );
        }
    }
    
    if( qllGetTransAccessMode( QLL_ENV(aEnv) ) == QLL_ACCESS_MODE_READ_ONLY )
    {
        STL_TRY_THROW( sStmtAccessMode != QLL_ACCESS_MODE_READ_WRITE,
                       RAMP_ERR_TRANSACTION_IS_READ_ONLY );
    }

    /*
     * Validation
     */

    qllSetAtomicAttr( &aStatement->mSqlStmt, aStatement->mAtomicExecute );
      
    STL_TRY( qllValidateSQL( sSessionEnv->mTransId,
                             &aStatement->mDbcStmt,
                             &aStatement->mSqlStmt,
                             aSQLTextString,
                             sParseTree,
                             QLL_ENV( aEnv ) ) == STL_SUCCESS );

    if( aHasTransaction != NULL )
    {
        if( sSessionEnv->mTransId == SML_INVALID_TRANSID )
        {
            *aHasTransaction = STL_FALSE;
        }
        else
        {
            *aHasTransaction = STL_TRUE;
        }
    }

    if( aStatementType != NULL )
    {
        *aStatementType = qllGetStmtType( &aStatement->mSqlStmt );
    }

    if( aResultSet != NULL )
    {
        *aResultSet = qllNeedFetch( &aStatement->mSqlStmt );
    }

    if( aWithoutHoldCursor != NULL )
    {
        if( qllGetCursorHoldability( &aStatement->mSqlStmt ) == ELL_CURSOR_HOLDABILITY_WITHOUT_HOLD )
        {
            *aWithoutHoldCursor = STL_TRUE;
        }
        else
        {
            *aWithoutHoldCursor = STL_FALSE;
        }
    }

    if( aIsUpdatable != NULL )
    {
        if( qllGetCursorUpdatability( &aStatement->mSqlStmt ) == ELL_CURSOR_UPDATABILITY_FOR_UPDATE )
        {
            *aIsUpdatable = STL_TRUE;
        }
        else
        {
            *aIsUpdatable = STL_FALSE;
        }
    }

    if( aSensitivity != NULL )
    {
        *aSensitivity = qllGetCursorSensitivity( &aStatement->mSqlStmt );
    }

    aStatement->mNeedOptimization = STL_TRUE;
    aStatement->mOptimizeDecision |= SSL_OPTIMIZE_DECISION_FIRST_EXECUTION;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_ALLOW_COMMAND_IN_XA )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SSL_ERRCODE_NOT_ALLOW_COMMAND_IN_XA,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_TRANSACTION_IS_READ_ONLY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SSL_ERRCODE_TRANSACTION_IS_READ_ONLY,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    if( aHasTransaction != NULL )
    {
        if( sSessionEnv->mTransId == SML_INVALID_TRANSID )
        {
            *aHasTransaction = STL_FALSE;
        }
        else
        {
            *aHasTransaction = STL_TRUE;
        }
    }
    
    sErrorData = stlGetLastErrorData( KNL_ERROR_STACK( aEnv ) );

    if( sErrorData != NULL )
    {
        switch( stlGetExternalErrorCode( sErrorData ) )
        {
            case STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_NO_SUBCLASS :
            case STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_SERIALIZATION_FAILURE :
            case STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_INTEGRITY_CONSTRAINT_VIOLATION :
            case STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_STATEMENT_COMPLETION_UNKNOWN :
            case STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_TRIGGERED_ACTION_EXCEPTION :
                (void) qllRollback( sSessionEnv->mTransId,
                                    NULL, /* aXaContext */
                                    QLL_ENV( aEnv ) );
                (void) sslSetTransactionMode( sSessionEnv, aEnv );
            
                sSessionEnv->mTransId = SML_INVALID_TRANSID;
                knlSetTransScope( sSessionEnv, KNL_TRANS_SCOPE_NONE );

                if( aHasTransaction != NULL )
                {
                    *aHasTransaction = STL_FALSE;
                }
                break;
            default :
                break;
        }
    }

    return STL_FAILURE;
}
