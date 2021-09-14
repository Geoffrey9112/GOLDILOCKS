/*******************************************************************************
 * qllSqlAPI.c
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
 * @file qllSqlAPI.c
 * @brief SQL API
 */

#include <qll.h>

#include <qlDef.h>
#include <qlpParserMacro.h>


/**
 * @addtogroup qllParsing
 * @{
 */


/**
 * @brief 주어진 SQL String 을 Parsing 하고 Parse Tree 를 생성한다.
 * @param[in]  aDBCStmt        DBC Statement
 * @param[in]  aSQLStmt        SQL Statement
 * @param[in]  aSQLString      SQL Plain Text
 * @param[out] aParseTree      Parse Tree
 * @param[in]  aEnv            Environment
 * @remarks
 */
stlStatus qllParseSQL( qllDBCStmt    * aDBCStmt,
                       qllStatement  * aSQLStmt,
                       stlChar       * aSQLString,
                       qllNode      ** aParseTree,
                       qllEnv        * aEnv )
{
    STL_TRY( qlgParseSQL( aDBCStmt,
                          aSQLStmt,
                          QLL_PHASE_PARSING,
                          aSQLString,
                          STL_FALSE,  /* aIsRecompile */
                          STL_TRUE,   /* aSearchPlanCache */
                          aParseTree,
                          aEnv )
             == STL_SUCCESS );

    aSQLStmt->mLastSuccessPhase = QLL_PHASE_PARSING;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qllParing */



/**
 * @addtogroup qllValidation
 * @{
 */



/**
 * @brief SQL Statement 객체의 Parse Tree 에 Ini Plan 정보를 생성하고, 구문의 유효성을 검사한다.
 * @param[in] aTransID    Transaction ID
 * @param[in] aDBCStmt    DBC Statement
 * @param[in] aSQLStmt    SQL Statement
 * @param[in] aSQLString  SQL String
 * @param[in] aParseTree  Parse Tree
 * @param[in] aEnv        Environment
 * @remarks
 */
stlStatus qllValidateSQL( smlTransId      aTransID,
                          qllDBCStmt    * aDBCStmt,
                          qllStatement  * aSQLStmt,
                          stlChar       * aSQLString,
                          qllNode       * aParseTree,
                          qllEnv        * aEnv )
{
    /**
     * Handle 사용시 Aging 되지 않게 함.
     */

    smlSetScnToDisableAging( aTransID, SML_SESS_ENV(aEnv) );

    /**
     * Dictionary View SCN 설정
     * - VIEW 의 query validation 을 수행하는 경우가 있어 qlgValidateSQL() 에서 설정하면 안됨
     */

    aSQLStmt->mViewSCN = smlGetSystemScn();
    aSQLStmt->mStmt = NULL;
    
    /**
     * Validate 수행
     */

    STL_TRY( qlgValidateSQL( aTransID,
                             aDBCStmt,
                             aSQLStmt,
                             QLL_PHASE_VALIDATION,
                             aSQLString,
                             aParseTree,
                             aEnv )
             == STL_SUCCESS );
    
    /**
     * Aging 가능하게 함.
     */
    
    smlUnsetScnToEnableAging( SML_SESS_ENV(aEnv) );

    aSQLStmt->mLastSuccessPhase = QLL_PHASE_VALIDATION;
    
    return STL_SUCCESS;

    STL_FINISH;

    smlUnsetScnToEnableAging( SML_SESS_ENV(aEnv) );

    return STL_FAILURE;
}



/** @} qllValidation */



/**
 * @addtogroup qllBinding
 * @{
 */


/**
 * @brief SQL Statement 에 Bind Parameter의 DB Data Type 정보를 설정한다.
 * @param[in] aSQLStmt      SQL Statement
 * @param[in] aBindContext  Bind Type Context
 * @param[in] aEnv          Environment
 * @remarks
 * Validation 이후와 Optimization 단계 이전에 수행한다.
 */
void qllSetBindContext( qllStatement   * aSQLStmt,
                        knlBindContext * aBindContext,
                        qllEnv         * aEnv )
{
    /**
     * Fixed Table 정보 누적
     */

    qlgAddCallCntBindContext( aEnv );

    /**
     * Bind Context 연결
     */
    
    aSQLStmt->mBindContext = aBindContext;
}


/** @} qllBinding */



/**
 * @addtogroup qllOptimization
 * @{
 */


/**
 * @brief SQL Statement 의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement 객체
 * @param[in] aEnv       Environment
 * @remarks
 * Bind Context 이후 Optimize Data Area 이전에 수행되어야 한다.
 */
stlStatus qllOptimizeCodeSQL( smlTransId      aTransID,
                              qllDBCStmt    * aDBCStmt,
                              qllStatement  * aSQLStmt,
                              qllEnv        * aEnv )
{
    /**
     * Handle 사용시 Aging 되지 않게 함.
     */

    smlSetScnToDisableAging( aTransID, SML_SESS_ENV(aEnv) );

    /**
     * Code Optimization 수행
     */

    STL_TRY( qlgOptimizeCodeSQL( aTransID,
                                 aDBCStmt,
                                 aSQLStmt,
                                 QLL_PHASE_CODE_OPTIMIZE,
                                 aEnv )
             == STL_SUCCESS );
    
    /**
     * Aging 가능하게 함.
     */
    
    smlUnsetScnToEnableAging( SML_SESS_ENV(aEnv) );

    aSQLStmt->mLastSuccessPhase = QLL_PHASE_CODE_OPTIMIZE;
    
    return STL_SUCCESS;

    STL_FINISH;
    
    smlUnsetScnToEnableAging( SML_SESS_ENV(aEnv) );

    return STL_FAILURE;
}




/**
 * @brief SQL Statement 의 최적화된 Data Area 를 생성한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 * Data Area 는 Plan Cache 대상이 되지 않는 영역이다.
 * Optimize Code Area 이후 Execute SQL 전에 수행되어야 한다.
 */
stlStatus qllOptimizeDataSQL( smlTransId      aTransID,
                              qllDBCStmt    * aDBCStmt,
                              qllStatement  * aSQLStmt,
                              qllEnv        * aEnv )
{
    /**
     * Handle 사용시 Aging 되지 않게 함.
     */

    smlSetScnToDisableAging( aTransID, SML_SESS_ENV(aEnv) );

    /**
     * Data Optimization 수행
     */

    STL_TRY( qlgOptimizeDataSQL( aTransID,
                                 aDBCStmt,
                                 aSQLStmt,
                                 QLL_PHASE_DATA_OPTIMIZE,
                                 aEnv )
             == STL_SUCCESS );
    
    /**
     * Aging 가능하게 함.
     */
    
    smlUnsetScnToEnableAging( SML_SESS_ENV(aEnv) );

    KNL_BREAKPOINT( KNL_BREAKPOINT_QLL_OPTIMIZE_DATA_SQL_COMPLETE, KNL_ENV(aEnv) );

    aSQLStmt->mLastSuccessPhase = QLL_PHASE_DATA_OPTIMIZE;
    
    return STL_SUCCESS;

    STL_FINISH;

    smlUnsetScnToEnableAging( SML_SESS_ENV(aEnv) );

    return STL_FAILURE;
}

/** @} qllOptimization */





/**
 * @addtogroup qllExecution
 * @{
 */


/**
 * @brief Prepare 된 SQL 이 plan을 가지고 있는지의 여부
 * @param[in] aSQLStmt   SQL Statement
 * @return stlBool
 * @remarks
 * Recompile 등이 발생하면, Plan 이 존재하지 않을 수 있다.
 */
stlBool qllHasPlan( qllStatement  * aSQLStmt )
{
    return ( (aSQLStmt->mInitPlan == NULL) &&
             (aSQLStmt->mCodePlan == NULL) ) ? STL_FALSE : STL_TRUE;
}


/**
 * @brief SQL Statement 권한을 다시 검사한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qllRecheckPrivSQL( smlTransId      aTransID,
                             smlStatement  * aStmt,
                             qllDBCStmt    * aDBCStmt,
                             qllStatement  * aSQLStmt,
                             qllEnv        * aEnv )
{
    /**
     * Privilege Check 수행
     */
    
    STL_TRY( qlgRecheckPrivSQL( aTransID,
                                aStmt,
                                aDBCStmt,
                                aSQLStmt,
                                QLL_PHASE_RECHECK_PRIVILEGE,
                                aEnv )
             == STL_SUCCESS );

    aSQLStmt->mLastSuccessPhase = QLL_PHASE_RECHECK_PRIVILEGE;

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief SQL Statement 를 실행한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement 객체
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qllExecuteSQL( smlTransId      aTransID,
                         smlStatement  * aStmt,
                         qllDBCStmt    * aDBCStmt,
                         qllStatement  * aSQLStmt,
                         qllEnv        * aEnv )
{
    /**
     * Parameter Validation
     */
    
    if ( (aSQLStmt->mStmtAttr & QLL_STMT_ATTR_DB_ACCESS_MASK) == QLL_STMT_ATTR_DB_ACCESS_NONE )
    {
        STL_PARAM_VALIDATE( aStmt == NULL, QLL_ERROR_STACK(aEnv) );
    }

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );

    KNL_BREAKPOINT( KNL_BREAKPOINT_QLL_EXECUTE_SQL_BEFORE, KNL_ENV(aEnv) );
    
    /**
     * Dictionary View SCN 설정
     */

    if ( aStmt != NULL )
    {
        /* recompile 시 View SCN 설정을 위한 statement 연결 */
        aSQLStmt->mStmt = aStmt;

        if ( aStmt->mNeedSnapshotIterator == STL_TRUE )
        {
            /* snapshot iterator 를 사용하는 경우 */
            aSQLStmt->mViewSCN = aStmt->mScn;
        }
        else
        {
            /* DDL 등 recent iterator 를 사용하는 경우 */
            aSQLStmt->mViewSCN = SML_MAXIMUM_STABLE_SCN;
        }
    }
    else
    {
        aSQLStmt->mStmt = NULL;
        aSQLStmt->mViewSCN = smlGetSystemScn();
    }
    
    /**
     * Execution 수행
     */

    STL_TRY( qlgExecuteSQL( aTransID,
                            aStmt,
                            aDBCStmt,
                            aSQLStmt,
                            QLL_PHASE_EXECUTE,
                            aEnv )
             == STL_SUCCESS );

    KNL_BREAKPOINT( KNL_BREAKPOINT_QLL_EXECUTE_SQL_COMPLETE, KNL_ENV(aEnv) );

    aSQLStmt->mLastSuccessPhase = QLL_PHASE_EXECUTE;

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief 구문 수행 중 RETRY 에러 상황인 경우, Recompile 한다.
 * @param[in]  aTransID     Transaction ID
 * @param[in]  aDBCStmt     DBC Statement
 * @param[in]  aSQLStmt     SQL Stmt
 * @param[in]  aQueryPhase  구문 수행 단계
 * @param[in]  aEnv         Environment
 * @remarks
 */
stlStatus qllRecompileSQL( smlTransId     aTransID,
                           qllDBCStmt   * aDBCStmt,
                           qllStatement * aSQLStmt,
                           qllQueryPhase  aQueryPhase,
                           qllEnv       * aEnv )
{
    stlBool sSearchPlanCache = STL_TRUE;

    /**
     * Handle 사용시 Aging 되지 않게 함.
     */

    smlSetScnToDisableAging( aTransID, SML_SESS_ENV(aEnv) );
    
    if( aSQLStmt->mSqlHandle != NULL )
    {
        /**
         * Cache를 풀고 Plan Cache를 사용하지 않도록 설정한다.
         */
        
        STL_TRY( knlUnfixSql( aSQLStmt->mSqlHandle,
                              KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        aSQLStmt->mSqlHandle = NULL;
        sSearchPlanCache = STL_FALSE;
    }
    
    STL_TRY( qlgRecompileSQL( aTransID,
                              aDBCStmt,
                              aSQLStmt,
                              sSearchPlanCache,
                              aQueryPhase,
                              aEnv )
             == STL_SUCCESS );

    /**
     * Aging 가능하게 함.
     */
    
    smlUnsetScnToEnableAging( SML_SESS_ENV(aEnv) );
    
    return STL_SUCCESS;

    STL_FINISH;

    smlUnsetScnToEnableAging( SML_SESS_ENV(aEnv) );

    return STL_FAILURE;
}


/**
 * @brief SQL Statement 가 Recompile 되었는지의 여부
 * @param[in]  aSQLStmt   SQL Statement 객체
 * @return
 * stlBool
 * @remarks
 */
stlBool qllIsRecompileAndReset( qllStatement * aSQLStmt )
{
    stlBool   sResult = aSQLStmt->mIsRecompile;
    
    aSQLStmt->mIsRecompile = STL_FALSE;
    
    return sResult;
}


/**
 * @brief DML SQL Statement로 인해 영향받은 row의 수를 반환한다.
 * @param[in]  aSQLStmt   SQL Statement 객체
 * @param[out] aRowCnt    영향받은 row의 수
 * @param[in]  aEnv       Environment
 * @remarks
 */
stlStatus qllGetAffectedRowCnt( qllStatement  * aSQLStmt,
                                stlInt64      * aRowCnt,
                                qllEnv        * aEnv )
{
    stlInt64 sCount = 0;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRowCnt != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * INSERT, DELETE, UPDATE로 인해 영향받은 레코드의 수를 설정한다.
     * 그 외 statement는 -1 을 설정한다.
     */

    switch ( aSQLStmt->mStmtType )
    {
        case QLL_STMT_DELETE_TYPE:
        case QLL_STMT_DELETE_WHERE_CURRENT_OF_TYPE:
        case QLL_STMT_DELETE_RETURNING_QUERY_TYPE:
        case QLL_STMT_DELETE_RETURNING_INTO_TYPE:
            {
                sCount = ((qllDataArea *) aSQLStmt->mDataPlan)->mWriteRowCnt;
                break;
            }
        case QLL_STMT_FETCH_CURSOR_TYPE:
            {
                sCount = (((qlcrDataFetchCursor *) aSQLStmt->mDataPlan)->mHasData == STL_TRUE )
                    ? 1 : 0;
                break;
            }
        case QLL_STMT_INSERT_TYPE:
        case QLL_STMT_INSERT_RETURNING_INTO_TYPE:
        case QLL_STMT_INSERT_RETURNING_QUERY_TYPE:
        case QLL_STMT_INSERT_SELECT_TYPE:
            {
                sCount = ((qllDataArea *) aSQLStmt->mDataPlan)->mWriteRowCnt;
                break;
            }
        case QLL_STMT_MERGE_INTO_TYPE:
            {
                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                break;
            }
        case QLL_STMT_SELECT_INTO_TYPE:
        case QLL_STMT_SELECT_INTO_FOR_UPDATE_TYPE:
            {
                sCount = ((qllDataArea *) aSQLStmt->mDataPlan)->mFetchRowCnt;
                break;
            }
        case QLL_STMT_UPDATE_TYPE:
        case QLL_STMT_UPDATE_WHERE_CURRENT_OF_TYPE:
        case QLL_STMT_UPDATE_RETURNING_QUERY_TYPE:
        case QLL_STMT_UPDATE_RETURNING_INTO_TYPE:
            {
                sCount = ((qllDataArea *) aSQLStmt->mDataPlan)->mWriteRowCnt;
                break;
            }
        default:
            {
                sCount = -1;
                break;
            }
    }

    /**
     * Output 설정
     */

    *aRowCnt = sCount;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "qllGetAffectedRowCnt()" );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/** @} qllExecution */







/**
 * @addtogroup qllExplainPlan
 * @{
 */

/**
 * @brief SQL Statement 의 SQL Plan Text를 생성한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aIsVerbose Verbose 여부
 * @param[in] aEnv       Environment
 * @remarks
 * Candidate Area 는 Plan Cache 대상이 되지 않는 영역이다.
 * Optimize Code Area 이후에 수행되어야 한다.
 */
stlStatus qllExplainSQL( smlTransId      aTransID,
                         qllDBCStmt    * aDBCStmt,
                         qllStatement  * aSQLStmt,
                         stlBool         aIsVerbose,
                         qllEnv        * aEnv )
{
    KNL_BREAKPOINT( KNL_BREAKPOINT_QLL_EXPLAIN_SQL_BEGIN, KNL_ENV(aEnv) );

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Alloc 가능한 Memory 설정
     */
    
    qlgSetQueryPhaseMemMgr( aDBCStmt, QLL_PHASE_DATA_OPTIMIZE, aEnv );
    
    
    /**
     * SQL Plan Text 구성
     */

    if( aIsVerbose == STL_FALSE )
    {
        STL_TRY( qleExplainPlan( aTransID,
                                 aDBCStmt,
                                 aSQLStmt,
                                 &aSQLStmt->mExplainPlanText,
                                 aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( qleTraceExplainPlan( aTransID,
                                      aDBCStmt,
                                      aSQLStmt,
                                      2,
                                      &aSQLStmt->mExplainPlanText,
                                      aEnv )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    qlgDisableQueryPhaseMemMgr( aDBCStmt, aEnv );
    
    (void) knlClearRegionMem( & aEnv->mHeapMemRunning, KNL_ENV(aEnv) );
    
    return STL_FAILURE;
}


/**
 * @brief SQL Statement 의 SQL Plan Text를 Trace Log에 출력한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 */
stlStatus qllTraceExplainSQL( smlTransId      aTransID,
                              qllDBCStmt    * aDBCStmt,
                              qllStatement  * aSQLStmt,
                              qllEnv        * aEnv )
{
    return qllTraceExplainSQL_Internal( aTransID,
                                        aDBCStmt,
                                        aSQLStmt,
                                        STL_FALSE,
                                        aEnv );
}


/**
 * @brief Error가 발생한 SQL Statement 의 SQL Plan Text를 Trace Log에 출력한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 * Candidate Area 는 Plan Cache 대상이 되지 않는 영역이다.
 * Optimize Code Area 이후에 수행되어야 한다.
 */
stlStatus qllTraceExplainErrorSQL( smlTransId     aTransID,
                                   qllDBCStmt   * aDBCStmt,
                                   qllStatement * aSQLStmt,
                                   qllEnv       * aEnv )
{
    /* Error인 경우 Init Plan 및 Code Plan이 모두 없는 경우가 있을 수 있다.
     * 이를 체크하여 모두 없는 경우 Trace Log를 출력하지 않는다. */
    if( (aSQLStmt->mInitPlan == NULL) &&
        (aSQLStmt->mCodePlan == NULL) )
    {
        return STL_SUCCESS;
    }
    else
    {
        return qllTraceExplainSQL_Internal( aTransID,
                                            aDBCStmt,
                                            aSQLStmt,
                                            STL_TRUE,
                                            aEnv );
    }
}


/**
 * @brief SQL Statement 의 SQL Plan Text를 Trace Log에 출력한다.
 * @param[in]   aTransID    Transaction ID
 * @param[in]   aDBCStmt    DBC Statement
 * @param[in]   aSQLStmt    SQL Statement
 * @param[in]   aIsErrorSQL SQL의 Error 여부
 * @param[in]   aEnv        Environment
 * @remarks
 * Candidate Area 는 Plan Cache 대상이 되지 않는 영역이다.
 * Optimize Code Area 이후에 수행되어야 한다.
 */
stlStatus qllTraceExplainSQL_Internal( smlTransId     aTransID,
                                       qllDBCStmt   * aDBCStmt,
                                       qllStatement * aSQLStmt,
                                       stlBool        aIsErrorSQL,
                                       qllEnv       * aEnv )
{
    knlRegionMark         sRegionMemMark;
    qllExplainNodeText  * sPlanText     = NULL;

    stlInt32              sSqlPlanLevel;


    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );


#define _QLL_TRACE_MODULE_INFO_MS( _aPhrase, _aTime, _aCallCount )                              \
    {                                                                                           \
        stlSnprintf( sTmpStr, 7, "%.2lf",                                                       \
                     sTotalTime == 0                                                            \
                     ? (stlFloat64)0.0                                                          \
                     : ( (stlFloat64)(_aTime) * (stlFloat64)100.0 / (stlFloat64)sTotalTime ) ); \
        STL_TRY( qllTraceBody( &QLL_DATA_PLAN( aDBCStmt ),                                      \
                               aEnv,                                                            \
                               "| %9s | %1d:%02d:%02d.%02d | %6s %% | %8d |\n",                 \
                               (_aPhrase),                                                      \
                               (_aTime) / 3600000000,                                           \
                               ((_aTime) / 60000000) % 60,                                      \
                               ((_aTime) / 1000000) % 60,                                       \
                               ((_aTime) % 1000000) / 10000,                                    \
                               sTmpStr,                                                         \
                               (_aCallCount) )                                                  \
                 == STL_SUCCESS );                                                              \
    }

#define _QLL_TRACE_MODULE_INFO_US( _aPhrase, _aTime, _aCallCount )                              \
    {                                                                                           \
        stlSnprintf( sTmpStr, 7, "%.2lf",                                                       \
                     sTotalTime == 0                                                            \
                     ? (stlFloat64)0.0                                                          \
                     : ( (stlFloat64)(_aTime) * (stlFloat64)100.0 / (stlFloat64)sTotalTime ) ); \
        STL_TRY( qllTraceBody( &QLL_DATA_PLAN( aDBCStmt ),                                      \
                               aEnv,                                                            \
                               "| %9s | %1d:%02d:%02d.%06d | %6s %% | %8d |\n",                 \
                               (_aPhrase),                                                      \
                               (_aTime) / 3600000000,                                           \
                               ((_aTime) / 60000000) % 60,                                      \
                               ((_aTime) / 1000000) % 60,                                       \
                               ((_aTime) % 1000000),                                            \
                               sTmpStr,                                                         \
                               (_aCallCount) )                                                  \
                 == STL_SUCCESS );                                                              \
    }

    /* SQL Plan Level 설정 */
    sSqlPlanLevel = QLL_OPT_GET_TRACE_SQL_PLAN_LEVEL( aEnv );
    if( ( knlGetPropertyBigIntValueByID( KNL_PROPERTY_TRACE_LOG_ID,
                                         KNL_ENV(aEnv) ) / 10000 )
        != QLL_OPT_ADMIN_LAYER_ID )
    {
        if( (sSqlPlanLevel != 2) && (sSqlPlanLevel != 92) )
        {
            STL_THROW( RAMP_FINISH );
        }
    }


    /**
     * Alloc 가능한 Memory 설정
     */
    
    qlgSetQueryPhaseMemMgr( aDBCStmt, QLL_PHASE_DATA_OPTIMIZE, aEnv );
    

    /**
     * Trace Logger 생성
     */

    if( QLL_IS_INIT_TRACE_LOGGER(aEnv) )
    {
        /* Trace Logger 종료 */
        STL_TRY( qllTraceDestroyLogger( aEnv ) == STL_SUCCESS );
    }

    STL_TRY( qllTraceCreateLogger( aEnv ) == STL_SUCCESS );

    /* Region Memory Mark */
    KNL_INIT_REGION_MARK( &sRegionMemMark );
    STL_TRY( knlMarkRegionMem( &QLL_DATA_PLAN( aDBCStmt ),
                               &sRegionMemMark,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * SQL Plan Text 구성
     */

    STL_TRY( qleTraceExplainPlan( aTransID,
                                  aDBCStmt,
                                  aSQLStmt,
                                  sSqlPlanLevel,
                                  &sPlanText,
                                  aEnv )
             == STL_SUCCESS );

    /* 앞의 log와의 구분을 위한 라인출력 */
    (void)qllTraceString( "\n\n", aEnv );

    /* SQL 구문 출력 */
    STL_TRY( qllTraceBody( &QLL_DATA_PLAN( aDBCStmt ),
                           aEnv,
                           "< SQL Query String >\n"
                           "----------------------\n"
                           "%s\n\n\n",
                           aSQLStmt->mRetrySQL )
             == STL_SUCCESS );

    /* Trace Explain Plan */
    STL_TRY( qlncTraceExplainPlan( sPlanText,
                                   aEnv )
             == STL_SUCCESS );

    /* Result 출력 */
    STL_TRY( qllTraceBody( &QLL_DATA_PLAN( aDBCStmt ),
                           aEnv,
                           "\n\n\n< Result >\n"
                           "----------------------\n"
                           "  %s\n\n\n",
                           ( aIsErrorSQL == STL_FALSE
                             ? "SUCCESS" : "FAILURE" ) )
             == STL_SUCCESS );

    /* Execute Type 출력 */
    STL_TRY( qllTraceBody( &QLL_DATA_PLAN( aDBCStmt ),
                           aEnv,
                           "< Execution Type >\n"
                           "----------------------\n"
                           "  %s\n\n\n",
                           ( aSQLStmt->mOptInfo.mIsExecDirect == STL_TRUE
                             ? "DIRECT EXECUTE" : "PREPARE EXECUTE" ) )
             == STL_SUCCESS );

    /* Bind Param Value 출력 */
    if( (sSqlPlanLevel >= 2) && (aSQLStmt->mCodePlan != NULL) )
    {
        qllOptimizationNode * sCodePlan         = NULL;
        qlvRefExprList      * sAllExprList      = NULL;
        qlvRefExprItem      * sRefExprItem      = NULL;
        qlvInitBindParam    * sBindParam        = NULL;
        knlValueBlockList   * sINValueBlock     = NULL;
        knlValueBlockList   * sOUTValueBlock    = NULL;
        dtlDataValue        * sBindValue        = NULL;
        stlInt32              sCount;
        stlInt32              sDisplaySize;
        stlChar             * sDisplayValue     = NULL;
        stlInt64              sLength;
        qlvInitBindParam   ** sBindParamArr     = NULL;
        qlvInitExpression   * sIntoExprArr      = NULL;
        stlInt32              sIntoExprCount    = 0;
        stlInt32              i;
        stlInt64              sPrecision        = DTL_PRECISION_NA;
        stlInt64              sScale            = DTL_SCALE_NA;
        dtlStringLengthUnit   sStringLengthUint = DTL_STRING_LENGTH_UNIT_NA;
        dtlIntervalIndicator  sIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;


        STL_TRY( qllTraceBody( &QLL_DATA_PLAN( aDBCStmt ),
                               aEnv,
                               "< Bind Param Value >\n"
                               "--------------------------\n")
                 == STL_SUCCESS );

        switch( aSQLStmt->mStmtType )
        {
            case QLL_STMT_SELECT_TYPE :
            case QLL_STMT_SELECT_FOR_UPDATE_TYPE :
            case QLL_STMT_SELECT_INTO_TYPE :
            case QLL_STMT_SELECT_INTO_FOR_UPDATE_TYPE :
            case QLL_STMT_DELETE_TYPE :
            case QLL_STMT_DELETE_WHERE_CURRENT_OF_TYPE :
            case QLL_STMT_DELETE_RETURNING_QUERY_TYPE :
            case QLL_STMT_DELETE_RETURNING_INTO_TYPE :
            case QLL_STMT_UPDATE_TYPE :
            case QLL_STMT_UPDATE_WHERE_CURRENT_OF_TYPE :
            case QLL_STMT_UPDATE_RETURNING_QUERY_TYPE :
            case QLL_STMT_UPDATE_RETURNING_INTO_TYPE :
            case QLL_STMT_INSERT_TYPE :
            case QLL_STMT_INSERT_RETURNING_INTO_TYPE :
            case QLL_STMT_INSERT_RETURNING_QUERY_TYPE :
                {
                    sCodePlan = (qllOptimizationNode *) aSQLStmt->mCodePlan;

                    break;
                }
            default:
                {
                    sCodePlan = NULL;

                    break;
                }
        }

        STL_DASSERT( aSQLStmt->mBindContext != NULL );

        STL_TRY_THROW( (sCodePlan != NULL) && (aSQLStmt->mBindContext->mSize > 0),
                       RAMP_NO_BIND_PARAM );

        switch( sCodePlan->mType )
        {
            case QLL_PLAN_NODE_STMT_SELECT_TYPE:
                STL_DASSERT( (((qlnoSelectStmt*)(sCodePlan->mOptNode))->mStmtExprList != NULL) &&
                             (((qlnoSelectStmt*)(sCodePlan->mOptNode))->mStmtExprList->mAllExprList != NULL) );
                sAllExprList = ((qlnoSelectStmt*)(sCodePlan->mOptNode))->mStmtExprList->mAllExprList;
                sIntoExprCount = ((qlnoSelectStmt*)(sCodePlan->mOptNode))->mTargetCnt;
                sIntoExprArr = ((qlnoSelectStmt*)(sCodePlan->mOptNode))->mIntoTargetArray;
                break;
            case QLL_PLAN_NODE_STMT_INSERT_TYPE:
                STL_DASSERT( (((qlnoInsertStmt*)(sCodePlan->mOptNode))->mStmtExprList != NULL) &&
                             (((qlnoInsertStmt*)(sCodePlan->mOptNode))->mStmtExprList->mAllExprList != NULL) );
                sAllExprList = ((qlnoInsertStmt*)(sCodePlan->mOptNode))->mStmtExprList->mAllExprList;
                sIntoExprCount = ((qlnoInsertStmt*)(sCodePlan->mOptNode))->mReturnExprCnt;
                sIntoExprArr = ((qlnoInsertStmt*)(sCodePlan->mOptNode))->mIntoTargetArray;
                break;
            case QLL_PLAN_NODE_STMT_UPDATE_TYPE:
                STL_DASSERT( (((qlnoUpdateStmt*)(sCodePlan->mOptNode))->mStmtExprList != NULL) &&
                             (((qlnoUpdateStmt*)(sCodePlan->mOptNode))->mStmtExprList->mAllExprList != NULL) );
                sAllExprList = ((qlnoUpdateStmt*)(sCodePlan->mOptNode))->mStmtExprList->mAllExprList;
                sIntoExprCount = ((qlnoUpdateStmt*)(sCodePlan->mOptNode))->mReturnExprCnt;
                sIntoExprArr = ((qlnoUpdateStmt*)(sCodePlan->mOptNode))->mIntoTargetArray;
                break;
            case QLL_PLAN_NODE_STMT_DELETE_TYPE:
                STL_DASSERT( (((qlnoDeleteStmt*)(sCodePlan->mOptNode))->mStmtExprList != NULL) &&
                             (((qlnoDeleteStmt*)(sCodePlan->mOptNode))->mStmtExprList->mAllExprList != NULL) );
                sAllExprList = ((qlnoDeleteStmt*)(sCodePlan->mOptNode))->mStmtExprList->mAllExprList;
                sIntoExprCount = ((qlnoDeleteStmt*)(sCodePlan->mOptNode))->mReturnExprCnt;
                sIntoExprArr = ((qlnoDeleteStmt*)(sCodePlan->mOptNode))->mIntoTargetArray;
                break;
            default:
                STL_DASSERT( 0 );
                break;
        }

        /* Bind Param Expression 수집 */
        STL_TRY( knlAllocRegionMem( &QLL_DATA_PLAN( aDBCStmt ),
                                    STL_SIZEOF( qlvInitBindParam* ) * aSQLStmt->mBindContext->mSize,
                                    (void**) &sBindParamArr,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlMemset( sBindParamArr, 0x00, STL_SIZEOF( qlvInitBindParam* ) * aSQLStmt->mBindContext->mSize );

        sCount = 0;
        sRefExprItem = sAllExprList->mHead;
        while( sRefExprItem != NULL )
        {
            if( sRefExprItem->mExprPtr->mType == QLV_EXPR_TYPE_BIND_PARAM )
            {
                sBindParam = sRefExprItem->mExprPtr->mExpr.mBindParam;

                if( sBindParamArr[sBindParam->mBindParamIdx] == NULL )
                {
                    sBindParamArr[sBindParam->mBindParamIdx] = sBindParam;
                    sCount++;
                }
            }

            sRefExprItem = sRefExprItem->mNext;
        }

        if( sIntoExprArr != NULL )
        {
            STL_DASSERT( sIntoExprCount > 0 );

            for( i = 0; i < sIntoExprCount; i++ )
            {
                STL_DASSERT( sIntoExprArr[i].mType == QLV_EXPR_TYPE_BIND_PARAM );
                sBindParam = sIntoExprArr[i].mExpr.mBindParam;
                if( sBindParamArr[sBindParam->mBindParamIdx] == NULL )
                {
                    sBindParamArr[sBindParam->mBindParamIdx] = sBindParam;
                    sCount++;
                }
            }
        }

        STL_TRY_THROW( sCount > 0, RAMP_NO_BIND_PARAM );

        /* Bind Param Type을 찾아 출력 */
        for( sCount = 0; sCount < aSQLStmt->mBindContext->mSize; sCount++ )
        {
            if( sBindParamArr[sCount] != NULL )
            {
                sBindParam = sBindParamArr[sCount];

                if( sBindParam->mHostName == NULL )
                {
                    STL_TRY( qllTraceBody( &QLL_DATA_PLAN( aDBCStmt ),
                                           aEnv,
                                           "  %2d - ?",
                                           sCount )
                             == STL_SUCCESS );
                }
                else
                {
                    STL_TRY( qllTraceBody( &QLL_DATA_PLAN( aDBCStmt ),
                                           aEnv,
                                           "  %2d - :%s",
                                           sCount,
                                           sBindParam->mHostName )
                             == STL_SUCCESS );
                }

                if( sBindParam->mBindType == KNL_BIND_TYPE_IN )
                {
                    (void)qllTraceString( "(IN", aEnv );
                }
                else if( sBindParam->mBindType == KNL_BIND_TYPE_OUT )
                {
                    (void)qllTraceString( "(OUT", aEnv );
                }
                else
                {
                    STL_DASSERT( sBindParam->mBindType == KNL_BIND_TYPE_INOUT );
                    (void)qllTraceString( "(IN/OUT", aEnv );
                }

                STL_TRY( knlGetParamValueBlock( aSQLStmt->mBindContext,
                                                sBindParam->mBindParamIdx + 1,
                                                &sINValueBlock,
                                                &sOUTValueBlock,
                                                KNL_ERROR_STACK(aEnv) ) == STL_SUCCESS );
                if( sINValueBlock != NULL )
                {
                    sBindValue = DTL_GET_BLOCK_FIRST_DATA_VALUE( sINValueBlock );
                    sPrecision = DTL_GET_BLOCK_ARG_NUM1( sINValueBlock );
                    sScale     = DTL_GET_BLOCK_ARG_NUM2( sINValueBlock );
                    sStringLengthUint  = DTL_GET_BLOCK_STRING_LENGTH_UNIT( sINValueBlock );
                    sIntervalIndicator = DTL_GET_BLOCK_INTERVAL_INDICATOR( sINValueBlock );
                }
                else if( sOUTValueBlock != NULL )
                {
                    sBindValue = DTL_GET_BLOCK_FIRST_DATA_VALUE( sOUTValueBlock );
                    sPrecision = DTL_GET_BLOCK_ARG_NUM1( sOUTValueBlock );
                    sScale     = DTL_GET_BLOCK_ARG_NUM2( sOUTValueBlock );
                    sStringLengthUint  = DTL_GET_BLOCK_STRING_LENGTH_UNIT( sOUTValueBlock );
                    sIntervalIndicator = DTL_GET_BLOCK_INTERVAL_INDICATOR( sOUTValueBlock );
                }
                else
                {
                    sBindValue = NULL;
                }

                if( sBindValue != NULL )
                {
                    /* Type Info Check */
                    STL_TRY( dtlVaildateDataTypeInfo( sBindValue->mType,
                                                      sPrecision,
                                                      sScale,
                                                      sStringLengthUint,
                                                      sIntervalIndicator,
                                                      KNL_ERROR_STACK(aEnv) )
                           == STL_SUCCESS );

                    sDisplaySize = dtlGetDisplaySize( sBindValue->mType,
                                                      gResultDataTypeDef[sBindValue->mType].mArgNum1,
                                                      gResultDataTypeDef[sBindValue->mType].mArgNum2,
                                                      gResultDataTypeDef[sBindValue->mType].mIntervalIndicator,
                                                      KNL_DT_VECTOR(aEnv),
                                                      aEnv );

                    STL_TRY( knlAllocRegionMem( &QLL_DATA_PLAN( aDBCStmt ),
                                                sDisplaySize + 1,
                                                (void**) &sDisplayValue,
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    STL_TRY( dtlToStringDataValue( sBindValue,
                                                   sPrecision,
                                                   sScale,
                                                   sDisplaySize,
                                                   sDisplayValue,
                                                   &sLength,
                                                   KNL_DT_VECTOR(aEnv),
                                                   aEnv,
                                                   KNL_ERROR_STACK(aEnv) )
                             == STL_SUCCESS );
                    sDisplayValue[sLength] = '\0';

                    STL_TRY( qllTraceBody( &QLL_DATA_PLAN( aDBCStmt ),
                                           aEnv,
                                           ", \"%s\")\n",
                                           sDisplayValue )
                             == STL_SUCCESS );
                }
                else
                {
                    STL_TRY( qllTraceBody( &QLL_DATA_PLAN( aDBCStmt ),
                                           aEnv,
                                           ", NULL)\n" )
                             == STL_SUCCESS );
                }
            }
            else
            {
                STL_DASSERT( 0 );
            }
        }

        STL_THROW( RAMP_BIND_PARAM_FINISH );


        STL_RAMP( RAMP_NO_BIND_PARAM );


        STL_TRY( qllTraceBody( &QLL_DATA_PLAN( aDBCStmt ),
                               aEnv,
                               "  No Bind Param.\n" )
                 == STL_SUCCESS );


        STL_RAMP( RAMP_BIND_PARAM_FINISH );


        (void)qllTraceString( "\n\n", aEnv );
    }

    /* Time Info 출력 */
    if( sSqlPlanLevel >= 2 )
    {
        stlTime   sTotalTime;
        stlTime   sTmpTime;
        stlChar   sTmpStr[8];

        sTotalTime =
            ( aSQLStmt->mOptInfo.mParseTime +
              aSQLStmt->mOptInfo.mValidateTime +
              aSQLStmt->mOptInfo.mCodeOptTime +
              aSQLStmt->mOptInfo.mDataOptTime +
              aSQLStmt->mOptInfo.mExecuteTime +
              aSQLStmt->mOptInfo.mFetchTime );

        if( knlGetPropertyBoolValueByID( KNL_PROPERTY_TRACE_LOG_TIME_DETAIL,
                                         KNL_ENV(aEnv) ) == STL_FALSE )
        {
            STL_TRY( qllTraceBody( &QLL_DATA_PLAN( aDBCStmt ),
                                   aEnv,
                                   "< Time Info >\n"
                                   "================================================\n"
                                   "| Module    | Time       | Rate     | Call     |\n"
                                   "------------------------------------------------\n")
                     == STL_SUCCESS );

            _QLL_TRACE_MODULE_INFO_MS( "Parse    ",
                                       aSQLStmt->mOptInfo.mParseTime,
                                       aSQLStmt->mOptInfo.mParseCallCount );
            _QLL_TRACE_MODULE_INFO_MS( "Validate ",
                                       aSQLStmt->mOptInfo.mValidateTime,
                                       aSQLStmt->mOptInfo.mValidateCallCount );

            sTmpTime =
                ( aSQLStmt->mOptInfo.mCodeOptTime -
                  aSQLStmt->mOptInfo.mOptimizerTime );

            _QLL_TRACE_MODULE_INFO_MS( "Code Opt ",
                                       sTmpTime,
                                       aSQLStmt->mOptInfo.mCodeOptCallCount );
            _QLL_TRACE_MODULE_INFO_MS( "Optimizer",
                                       aSQLStmt->mOptInfo.mOptimizerTime,
                                       aSQLStmt->mOptInfo.mOptimizerCallCount );
            _QLL_TRACE_MODULE_INFO_MS( "Data Opt ",
                                       aSQLStmt->mOptInfo.mDataOptTime,
                                       aSQLStmt->mOptInfo.mDataOptCallCount );
            _QLL_TRACE_MODULE_INFO_MS( "Execute  ",
                                       aSQLStmt->mOptInfo.mExecuteTime,
                                       aSQLStmt->mOptInfo.mExecuteCallCount );
            _QLL_TRACE_MODULE_INFO_MS( "Fetch    ",
                                       aSQLStmt->mOptInfo.mFetchTime,
                                       aSQLStmt->mOptInfo.mFetchCallCount );

            STL_TRY( qllTraceBody( &QLL_DATA_PLAN( aDBCStmt ),
                                   aEnv,
                                   "| Total     | %1d:%02d:%02d.%02d | 100.00 %% |          |\n",
                                   sTotalTime / 3600000000,
                                   (sTotalTime / 60000000) % 60,
                                   (sTotalTime / 1000000) % 60,
                                   (sTotalTime % 1000000) / 10000 )
                     == STL_SUCCESS );

            (void)qllTraceString( "================================================\n", aEnv );
        }
        else
        {
            STL_TRY( qllTraceBody( &QLL_DATA_PLAN( aDBCStmt ),
                                   aEnv,
                                   "< Time Info >\n"
                                   "====================================================\n"
                                   "| Module    | Time           | Rate     | Call     |\n"
                                   "----------------------------------------------------\n")
                     == STL_SUCCESS );

            _QLL_TRACE_MODULE_INFO_US( "Parse    ",
                                       aSQLStmt->mOptInfo.mParseTime,
                                       aSQLStmt->mOptInfo.mParseCallCount );
            _QLL_TRACE_MODULE_INFO_US( "Validate ",
                                       aSQLStmt->mOptInfo.mValidateTime,
                                       aSQLStmt->mOptInfo.mValidateCallCount );

            sTmpTime =
                ( aSQLStmt->mOptInfo.mCodeOptTime -
                  aSQLStmt->mOptInfo.mOptimizerTime );

            _QLL_TRACE_MODULE_INFO_US( "Code Opt ",
                                       sTmpTime,
                                       aSQLStmt->mOptInfo.mCodeOptCallCount );
            _QLL_TRACE_MODULE_INFO_US( "Optimizer",
                                       aSQLStmt->mOptInfo.mOptimizerTime,
                                       aSQLStmt->mOptInfo.mOptimizerCallCount );
            _QLL_TRACE_MODULE_INFO_US( "Data Opt ",
                                       aSQLStmt->mOptInfo.mDataOptTime,
                                       aSQLStmt->mOptInfo.mDataOptCallCount );
            _QLL_TRACE_MODULE_INFO_US( "Execute  ",
                                       aSQLStmt->mOptInfo.mExecuteTime,
                                       aSQLStmt->mOptInfo.mExecuteCallCount );
            _QLL_TRACE_MODULE_INFO_US( "Fetch    ",
                                       aSQLStmt->mOptInfo.mFetchTime,
                                       aSQLStmt->mOptInfo.mFetchCallCount );

            STL_TRY( qllTraceBody( &QLL_DATA_PLAN( aDBCStmt ),
                                   aEnv,
                                   "| Total     | %1d:%02d:%02d.%06d | 100.00 %% |          |\n",
                                   sTotalTime / 3600000000,
                                   (sTotalTime / 60000000) % 60,
                                   (sTotalTime / 1000000) % 60,
                                   (sTotalTime % 1000000) )
                     == STL_SUCCESS );

            (void)qllTraceString( "====================================================\n", aEnv );

        }
    }

    /* 뒤의 log와의 구분을 위한 라인출력 */
    (void)qllTraceString( "\n\n", aEnv );

    /* Region Memory UnMark */
    STL_TRY( knlFreeUnmarkedRegionMem( &QLL_DATA_PLAN( aDBCStmt ),
                                       &sRegionMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Trace Logger 종료
     */

    if( QLL_IS_INIT_TRACE_LOGGER(aEnv) )
    {
        /* Trace Logger 종료 */
        STL_TRY( qllTraceDestroyLogger( aEnv ) == STL_SUCCESS );
    }


    STL_RAMP( RAMP_FINISH );

    
    return STL_SUCCESS;
    
    STL_FINISH;

    qlgDisableQueryPhaseMemMgr( aDBCStmt, aEnv );
    
    (void) knlClearRegionMem( & aEnv->mHeapMemRunning, KNL_ENV(aEnv) );
    
    return STL_FAILURE;
}

/** @} qllExplainPlan */
