/*******************************************************************************
 * sscExecDirect.c
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
 * @file sscExecDirect.c
 * @brief Session Layer ExecDirect Command Internal Routines
 */


#include <qll.h>
#include <sslDef.h>
#include <sslSession.h>
#include <ssc.h>
#include <ssDef.h>

stlStatus sscExecDirect( stlChar              * aSQLTextString,
                         sslStatement         * aStatement,
                         stlBool                aIsFirst,
                         stlBool              * aHasTransaction,
                         stlBool              * aIsRecompile,
                         qllNodeType          * aStatementType,
                         stlBool              * aHasResultSet,
                         stlBool              * aWithoutHoldCursor,
                         stlBool              * aIsUpdatable,
                         ellCursorSensitivity * aSensitivity,
                         stlInt64             * aAffectedRowCount,
                         sslDCLContext        * aDCLContext,
                         sslEnv               * aEnv )
{
    sslSessionEnv          * sSessionEnv = NULL;
    qllNode                * sParseTree;
    smlStatement           * sSmStmt = NULL;
    stlInt32                 sState = 0;
    stlBool                  sNeedLockDDL;
    stlBool                  sHasResultSet;
    stlBool                  sHasCursorName;
    qllAccessMode            sStmtAccessMode;
    qllAccessMode            sKnownAccessMode;
    stlBool                  sIsDCL;
    stlUInt64                sArraySize;
    stlUInt64                sArrayIdx = 0;
    stlInt32                 sBlockReadCount = 1;
    stlInt64                 sAffectedRowCount;
    stlBool                  sSupportAtomic;
    stlInt32                 sAttr = 0;
    stlBool                  sAutocommit;
    stlBool                  sSupportGlobal;
    stlBool                  sNeedSnapshot;
    stlBool                  sLockableStmt;
    stlBool                  sKnownLockableStmt;
    stlBool                  sNeedTransaction = STL_FALSE;
    stlBool                  sNeedRollbackAtException = STL_FALSE;
    knlBindContext         * sBindContext;
    stlErrorData           * sErrorData = NULL;

    aStatement->mIsEndOfScan = STL_FALSE;

    sBindContext = aStatement->mBindContext;
    sArraySize   = aStatement->mCurrArraySize;
    
    sSessionEnv = SSL_SESS_ENV( aEnv );

    STL_PARAM_VALIDATE( aStatement->mCursorStmt == NULL, KNL_ERROR_STACK( aEnv ) );
    STL_DASSERT( sArraySize > 0 );

    SSL_INIT_DCL_CONTEXT( aDCLContext );

    if( aIsFirst == STL_TRUE )
    {
        STL_TRY( knlInitExecuteParamValueBlocks( aStatement->mBindContext,
                                                 sArraySize,
                                                 KNL_ERROR_STACK(aEnv) ) == STL_SUCCESS );
    }

    /*
     * Parsing
     */
    STL_TRY( qllResetSQL( &aStatement->mDbcStmt,
                          &aStatement->mSqlStmt,
                          QLL_ENV( aEnv ) ) == STL_SUCCESS );

    STL_TRY( qllClearOptInfoSQL( &(aStatement->mSqlStmt),
                                 QLL_ENV( aEnv ) )
             == STL_SUCCESS );
    aStatement->mSqlStmt.mOptInfo.mIsExecDirect = STL_TRUE;
    aStatement->mSqlStmt.mOptInfo.mIsExplainPlanOnly =
        ( aStatement->mExplainType == SSL_EXPLAIN_PLAN_ONLY
          ? STL_TRUE : STL_FALSE );

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
    sNeedLockDDL    = qllIsDDL( &aStatement->mSqlStmt );
    sNeedSnapshot   = qllNeedSnapshotIterator( &aStatement->mSqlStmt );
    sAutocommit     = qllNeedAutocommit( &aStatement->mSqlStmt );
    sSupportGlobal  = qllSupportGlobalTransaction( &aStatement->mSqlStmt );
    sIsDCL          = qllNeedStateTransition( &aStatement->mSqlStmt );
    sSupportAtomic  = qllSupportAtomicExecution( &aStatement->mSqlStmt );
    sHasResultSet   = qllNeedFetch( &aStatement->mSqlStmt );
    sHasCursorName  = SSR_HAS_CURSOR_NAME( aStatement );
    sLockableStmt   = qllIsLockableStmt( &aStatement->mSqlStmt );

    if( knlGetTransScope( sSessionEnv ) == KNL_TRANS_SCOPE_GLOBAL )
    {
        STL_TRY_THROW( sSupportGlobal == STL_TRUE, RAMP_ERR_NOT_ALLOW_COMMAND_IN_XA );
    }

    if( (sAutocommit == STL_FALSE) && (sNeedLockDDL == STL_TRUE) )
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
        sNeedRollbackAtException = STL_TRUE;
        
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

            knlSetTransScope( sSessionEnv, KNL_TRANS_SCOPE_NONE );
            sSessionEnv->mTransId = SML_INVALID_TRANSID;
        }
    }

    if( qllGetTransAccessMode( QLL_ENV(aEnv) ) == QLL_ACCESS_MODE_READ_ONLY )
    {
        STL_TRY_THROW( sStmtAccessMode != QLL_ACCESS_MODE_READ_WRITE,
                       RAMP_ERR_TRANSACTION_IS_READ_ONLY );
    }
    
    if( aStatement->mAtomicExecute == STL_TRUE )
    {
        STL_TRY_THROW( sSupportAtomic == STL_TRUE, RAMP_ERR_ATOMIC_EXECUTION );
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

    /*
     * Optimization
     */

    qllSetBindContext( &aStatement->mSqlStmt, sBindContext, QLL_ENV(aEnv) );

    STL_TRY( qllOptimizeCodeSQL( sSessionEnv->mTransId,
                                 &aStatement->mDbcStmt,
                                 &aStatement->mSqlStmt,
                                 QLL_ENV( aEnv ) ) == STL_SUCCESS );

    STL_TRY( qllOptimizeDataSQL( sSessionEnv->mTransId,
                                 &aStatement->mDbcStmt,
                                 &aStatement->mSqlStmt,
                                 QLL_ENV( aEnv ) ) == STL_SUCCESS );

    /**
     * Cursor의 updatability가 for update 라면 트랜잭션을 할당해야 한다.
     */
    
    if( sSessionEnv->mTransId == SML_INVALID_TRANSID )
    {
        if( knlGetCurrStartupPhase() >= KNL_STARTUP_PHASE_OPEN )
        {
            if ( sStmtAccessMode == QLL_ACCESS_MODE_UNKNOWN )
            {
                /**
                 * Unknown Statemnet 의  Access Mode 를 얻는다.
                 */
                sKnownAccessMode   = qllGetAccessMode4UnknownStmt( & aStatement->mSqlStmt );
                sKnownLockableStmt = qllIsLockableStmt4UnknownStmt( & aStatement->mSqlStmt );
            }
            else
            {
                sKnownAccessMode   = sStmtAccessMode;
                sKnownLockableStmt = sLockableStmt;
            }
            
            switch (sKnownAccessMode)
            {
                case QLL_ACCESS_MODE_NONE:
                    sNeedTransaction = STL_FALSE;
                    break;
                case QLL_ACCESS_MODE_READ_WRITE:
                    sNeedTransaction = STL_TRUE;
                    STL_TRY_THROW( qllGetTransAccessMode( QLL_ENV(aEnv) ) == QLL_ACCESS_MODE_READ_WRITE,
                                   RAMP_ERR_TRANSACTION_IS_READ_ONLY );
                    break;
                case QLL_ACCESS_MODE_READ_ONLY:
                    /**
                     * read only 라도 isolaction level 이 serializable 이라면 트랜잭션이 필요하다.
                     */
                    if( sSessionEnv->mCurrTxnIsolation == SSL_ISOLATION_LEVEL_SERIALIZABLE )
                    {
                        sNeedTransaction = STL_TRUE;
                    }
                    else
                    {
                        sNeedTransaction = STL_FALSE;
                    }
                    break;
                case QLL_ACCESS_MODE_UNKNOWN:
                    /* UNKNOWN 상태가 존재하지 않아야 함 */
                    sNeedTransaction = STL_FALSE;
                    STL_DASSERT(0);
                    break;
                default:
                    sNeedTransaction = STL_FALSE;
                    STL_DASSERT(0);
                    break;
            }
            
            if ( sKnownLockableStmt == STL_TRUE )
            {
                sNeedTransaction = STL_TRUE;
            }
            
            /**
             * qllGetCursorUpdatability()는 init plan이 있는 경우에만 확인할수 있다.
             */
        
            if( (qllGetCursorUpdatability( & aStatement->mSqlStmt ) == ELL_CURSOR_UPDATABILITY_FOR_UPDATE) ||
                (sNeedTransaction == STL_TRUE) )
            {
                STL_TRY( smlBeginTrans( sSessionEnv->mCurrTxnIsolation,
                                        STL_FALSE,  /* aIsGlobalTrans */
                                        &sSessionEnv->mTransId,
                                        SML_ENV( aEnv ) ) == STL_SUCCESS );
        
                if( knlGetTransScope( sSessionEnv ) == KNL_TRANS_SCOPE_NONE )
                {
                    knlSetTransScope( sSessionEnv, KNL_TRANS_SCOPE_LOCAL );
                }
            }
        }
    }

    /*
     * Explain Plan
     */

    if( aStatement->mExplainType == SSL_EXPLAIN_PLAN_ONLY )
    {
        STL_TRY( qllExplainSQL( sSessionEnv->mTransId,
                                &aStatement->mDbcStmt,
                                &aStatement->mSqlStmt,
                                STL_FALSE, /* Is Verbose */
                                QLL_ENV( aEnv ) ) == STL_SUCCESS );
        
        if( QLL_OPT_TRACE_SUCCESS_SQL_PLAN_ON( aEnv ) == STL_TRUE )
        {
            STL_TRY( qllTraceExplainSQL( sSessionEnv->mTransId,
                                         &aStatement->mDbcStmt,
                                         &aStatement->mSqlStmt,
                                         QLL_ENV( aEnv ) ) == STL_SUCCESS );
        }

        if( aAffectedRowCount != NULL )
        {
            *aAffectedRowCount = -1;
        }
        
        STL_THROW( RAMP_IGNORE_EXECUTE );
    }

    /*
     * Execution
     */

    switch( sStmtAccessMode )
    {
        case QLL_ACCESS_MODE_NONE :
            break;
        case QLL_ACCESS_MODE_READ_WRITE :
            
            /**
             * DML은 qp로부터 aUpdateRelHandle을 얻어와 정확한 값을 설정해야 한다.
             */
            
            sAttr = SML_STMT_ATTR_UPDATABLE;
            
            if( sLockableStmt == STL_TRUE )
            {
                sAttr |= SML_STMT_ATTR_LOCKABLE;
            }

            if ( qllGetCursorUpdatability( & aStatement->mSqlStmt ) == ELL_CURSOR_UPDATABILITY_FOR_UPDATE )
            {
                sAttr |= SML_STMT_ATTR_LOCKABLE;
            }
            
            STL_TRY( smlAllocStatement( sSessionEnv->mTransId,
                                        NULL, /* aUpdateRelHandle, qllExecuteSQL()에서 설정함 */
                                        sAttr,
                                        ((sNeedLockDDL == STL_TRUE) ?
                                         SML_LOCK_TIMEOUT_PROPERTY : SML_LOCK_TIMEOUT_INFINITE),
                                        sNeedSnapshot,  /* aNeedSnapshotIterator */
                                        &sSmStmt,
                                        SML_ENV( aEnv ) ) == STL_SUCCESS );
            aStatement->mViewScn = sSmStmt->mScn;
            sState = 1;
            break;
        case QLL_ACCESS_MODE_READ_ONLY :
            
            sAttr = SML_STMT_ATTR_READONLY;
            if( sLockableStmt == STL_TRUE )
            {
                sAttr |= SML_STMT_ATTR_LOCKABLE;
            }

            if ( qllGetCursorUpdatability( & aStatement->mSqlStmt ) == ELL_CURSOR_UPDATABILITY_FOR_UPDATE )
            {
                sAttr |= SML_STMT_ATTR_LOCKABLE;
            }
            
            if ( qllGetCursorHoldability( & aStatement->mSqlStmt ) == ELL_CURSOR_HOLDABILITY_WITH_HOLD )
            {
                sAttr |= SML_STMT_ATTR_HOLDABLE;
            }
            
            
            STL_TRY( smlAllocStatement( sSessionEnv->mTransId,
                                        NULL, /* aUpdateRelHandle */
                                        sAttr,
                                        SML_LOCK_TIMEOUT_INVALID,
                                        sNeedSnapshot,  /* aNeedSnapshotIterator */
                                        &sSmStmt,
                                        SML_ENV( aEnv ) ) == STL_SUCCESS );
            aStatement->mViewScn = sSmStmt->mScn;
            sState = 1;
            break;
        case QLL_ACCESS_MODE_UNKNOWN:
            /**
             * OPEN cursor, EXEC proc 과 같이 내부적으로 Statement 를 할당하여 사용함.
             */
            break;
        default:
            STL_ASSERT(0);
            break;
    }

    /**
     * cached plan 인 경우 recheck privilege 가 필요함
     * - 비용이 싼 연산임
     * - 성능을 향상시키려면 Cached Plan 인지 생성한 Plan 인지 검사해야 함.
     */
    STL_TRY( qllRecheckPrivSQL( sSessionEnv->mTransId,
                                sSmStmt,
                                &aStatement->mDbcStmt,
                                &aStatement->mSqlStmt,
                                QLL_ENV( aEnv ) ) == STL_SUCCESS );
    
    if( sArraySize > 1 )
    {
        sBlockReadCount = knlGetPropertyBigIntValueByID( KNL_PROPERTY_BLOCK_READ_COUNT,
                                                         KNL_ENV(aEnv) );

        sBlockReadCount = STL_MIN( sBlockReadCount,
                                   knlGetExecuteValueBlockSize( sBindContext ) );
    }

    sBlockReadCount = STL_MIN( sBlockReadCount, sArraySize );

    STL_TRY( knlSetExecuteValueBlockSize( sBindContext,
                                          sBlockReadCount,
                                          KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );
    
    if( aAffectedRowCount != NULL )
    {
        *aAffectedRowCount = 0;
    }
    
    while( STL_TRUE )
    {
        if( qllExecuteSQL( sSessionEnv->mTransId,
                           sSmStmt,
                           &aStatement->mDbcStmt,
                           &aStatement->mSqlStmt,
                           QLL_ENV( aEnv ) ) == STL_SUCCESS )
        {
            /* Do Nothing */
        }
        else
        {
            if( QLL_OPT_TRACE_FAILURE_SQL_PLAN_ON( aEnv ) == STL_TRUE )
            {
                STL_TRY( qllTraceExplainErrorSQL( sSessionEnv->mTransId,
                                                  &aStatement->mDbcStmt,
                                                  &aStatement->mSqlStmt,
                                                  QLL_ENV( aEnv ) )
                         == STL_SUCCESS );
            }

            STL_THROW( STL_FINISH_LABEL );
        }

        if( aAffectedRowCount != NULL )
        {
            STL_TRY( qllGetAffectedRowCnt( &aStatement->mSqlStmt,
                                           &sAffectedRowCount,
                                           QLL_ENV( aEnv ) ) == STL_SUCCESS );

            *aAffectedRowCount += sAffectedRowCount;
        }

        sArrayIdx += sBlockReadCount;

        if( sArrayIdx >= sArraySize )
        {
            break;
        }

        STL_TRY( knlMoveExecuteParamValueBlocks( sBindContext,
                                                 sBlockReadCount,
                                                 KNL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );

        sBlockReadCount = STL_MIN( sBlockReadCount,
                                   (sArraySize - sArrayIdx) );

        STL_TRY( knlSetExecuteValueBlockSize( sBindContext,
                                              sBlockReadCount,
                                              KNL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );
    }


    STL_RAMP( RAMP_IGNORE_EXECUTE );

    
    if( sIsDCL == STL_TRUE )
    {
        STL_TRY( sscSetDCL( aStatement, aDCLContext, aEnv ) == STL_SUCCESS );

        if( aDCLContext->mType == SSL_DCL_TYPE_END_TRANSACTION )
        {
            STL_TRY( sscCloseWithoutHoldCursors( aEnv ) == STL_SUCCESS );
        }
    }

    if( aIsRecompile != NULL )
    {
        aStatement->mIsRecompile = qllIsRecompileAndReset( &aStatement->mSqlStmt );
        *aIsRecompile = aStatement->mIsRecompile;
    }
    
    if( aStatementType != NULL )
    {
        *aStatementType = qllGetStmtType( &aStatement->mSqlStmt );
    }

    if( sHasResultSet == STL_TRUE )
    {
        aStatement->mCursorStmt = sSmStmt;

        if( sHasCursorName == STL_TRUE )
        {
            STL_TRY( qllSetNamedCursorOpen( sSmStmt,
                                            & aStatement->mDbcStmt,
                                            & aStatement->mSqlStmt,
                                            aStatement->mCursorDesc,
                                            QLL_ENV(aEnv) )
                     == STL_SUCCESS );
        }

        if( qllGetCursorHoldability( &aStatement->mSqlStmt ) == ELL_CURSOR_HOLDABILITY_WITHOUT_HOLD )
        {
            sSessionEnv->mHasWithoutHoldCursor = STL_TRUE;
        }
    }
    else
    {
        if( ( aStatement->mExplainType == SSL_EXPLAIN_PLAN_ON ) ||
            ( aStatement->mExplainType == SSL_EXPLAIN_PLAN_ON_VERBOSE ) )
        {
            STL_TRY( qllExplainSQL( sSessionEnv->mTransId,
                                    &aStatement->mDbcStmt,
                                    &aStatement->mSqlStmt,
                                    ( aStatement->mExplainType == SSL_EXPLAIN_PLAN_ON
                                      ? STL_FALSE : STL_TRUE ), /* Is Verbose */
                                    QLL_ENV( aEnv ) ) == STL_SUCCESS );
        }
    
        if( QLL_OPT_TRACE_SUCCESS_SQL_PLAN_ON( aEnv ) == STL_TRUE )
        {
            STL_TRY( qllTraceExplainSQL( sSessionEnv->mTransId,
                                         &aStatement->mDbcStmt,
                                         &aStatement->mSqlStmt,
                                         QLL_ENV( aEnv ) ) == STL_SUCCESS );
        }

        sState = 0;
        if( sSmStmt != NULL )
        {
            STL_TRY( smlFreeStatement( sSmStmt,
                                       SML_ENV( aEnv ) ) == STL_SUCCESS );
            aStatement->mViewScn = SML_INFINITE_SCN;
        }
    }
    
    if( aHasResultSet != NULL )
    {
        *aHasResultSet = sHasResultSet;
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

    /**
     * post-commit을 해야 한다면
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
                                SML_TRANSACTION_CWM_WAIT,
                                NULL, /* aXaContext */
                                QLL_ENV( aEnv ) )
                     == STL_SUCCESS );
            
            sSessionEnv->mTransId = SML_INVALID_TRANSID;
            knlSetTransScope( sSessionEnv, KNL_TRANS_SCOPE_NONE );
        }
    }

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

    STL_CATCH( RAMP_ERR_ATOMIC_EXECUTION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SSL_ERRCODE_STATEMENT_CANNOT_EXECUTE_AS_ATOMIC_ACTION,
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
                break;
            default :
                break;
        }
    }

    switch( sState )
    {
        case 1 :
            (void)smlFreeStatement( sSmStmt,
                                    SML_ENV( aEnv ) );
            aStatement->mViewScn = SML_INFINITE_SCN;
        default :
            break;
    }

    if( sNeedRollbackAtException == STL_TRUE )
    {
        /**
         * 이전 Statement들을 정리한다.
         */
        if( sSessionEnv->mTransId != SML_INVALID_TRANSID )
        {
            (void) qllRollback( sSessionEnv->mTransId,
                                NULL, /* aXaContext */
                                QLL_ENV( aEnv ) );
            sSessionEnv->mTransId = SML_INVALID_TRANSID;
            knlSetTransScope( sSessionEnv, KNL_TRANS_SCOPE_NONE );
        }
    }
    
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
    
    return STL_FAILURE;
}
