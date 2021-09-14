/*******************************************************************************
 * qllStatement.c
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
 * @file qllStatement.c
 * @brief SQL Statement API
 */

#include <qll.h>

#include <qlDef.h>

/**
 * @addtogroup qllStatement
 * @{
 */

/**
 * @brief Invalid Function Pointer
 */
stlStatus qllValidateFuncNA( smlTransId     aTransID,
                             qllDBCStmt   * aDBCStmt,
                             qllStatement * aSQLStmt,
                             stlChar      * aSQLString,
                             qllNode      * aParseTree,
                             qllEnv       * aEnv )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  QLL_ERRCODE_INVALID_FUNCTION_SEQUENCE,
                  NULL,
                  QLL_ERROR_STACK(aEnv) );
    
    return STL_FAILURE;
}

/**
 * @brief Invalid Function Pointer
 */
stlStatus qllCodeOptimizeFuncNA( smlTransId     aTransID,
                                 qllDBCStmt   * aDBCStmt,
                                 qllStatement * aSQLStmt,
                                 qllEnv       * aEnv )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  QLL_ERRCODE_INVALID_FUNCTION_SEQUENCE,
                  NULL,
                  QLL_ERROR_STACK(aEnv) );
    
    return STL_FAILURE;
}

/**
 * @brief Invalid Function Pointer
 */
stlStatus qllDataOptimizeFuncNA( smlTransId     aTransID,
                                 qllDBCStmt   * aDBCStmt,
                                 qllStatement * aSQLStmt,
                                 qllEnv       * aEnv )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  QLL_ERRCODE_INVALID_FUNCTION_SEQUENCE,
                  NULL,
                  QLL_ERROR_STACK(aEnv) );

    return STL_FAILURE;
}


/**
 * @brief Invalid Function Pointer
 */
stlStatus qllExecuteFuncNA( smlTransId     aTransID,
                            smlStatement * aStmt,
                            qllDBCStmt   * aDBCStmt,
                            qllStatement * aSQLStmt,
                            qllEnv       * aEnv )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  QLL_ERRCODE_INVALID_FUNCTION_SEQUENCE,
                  NULL,
                  QLL_ERROR_STACK(aEnv) );
    
    return STL_FAILURE;
}



/**
 * @brief SQL Statement 를 초기화한다.
 * @param[in]  aSQLStmt  SQL Statement
 * @param[in]  aEnv      Environment
 * @remarks
 */
stlStatus qllInitSQL( qllStatement * aSQLStmt,
                      qllEnv       * aEnv )
{
    stlMemset( aSQLStmt, 0x00, STL_SIZEOF(qllStatement) );

    /********************************
     * SQL 구문 정보 
     ********************************/

    aSQLStmt->mRetrySQL = NULL;
    aSQLStmt->mStmtSQL[0] = '\0';
    aSQLStmt->mStartTime = knlGetSystemTime();
    aSQLStmt->mIsRecompile = STL_FALSE;
    aSQLStmt->mKeepParseTree = STL_FALSE;
    aSQLStmt->mStmtAttr = QLL_STMT_ATTR_NA;
    aSQLStmt->mStmtType = QLL_STMT_NA;
    aSQLStmt->mLastSuccessPhase = QLL_PHASE_NA;
    aSQLStmt->mViewSCN = SML_INFINITE_SCN;
    aSQLStmt->mStmt = NULL;
    aSQLStmt->mSqlHandle = NULL;

    /********************************
     * Layer 간 Context
     ********************************/
    
    aSQLStmt->mIsAtomic = STL_FALSE;

    aSQLStmt->mBindCount   = 0;
    aSQLStmt->mBindContext = NULL;

    aSQLStmt->mDCLContext.mType = QLL_DCL_TYPE_NONE;
    aSQLStmt->mDCLContext.mAccessMode = 0;
    aSQLStmt->mDCLContext.mGMTOffset = 0;
    aSQLStmt->mDCLContext.mIsolationLevel = SML_TIL_READ_COMMITTED;
    
    /********************************
     * Plan
     ********************************/
    
    aSQLStmt->mInitPlan = NULL;
    aSQLStmt->mCodePlan = NULL;
    aSQLStmt->mDataPlan = NULL;

    aSQLStmt->mExplainPlanText = NULL;

    /********************************
     * Cursor Result Set Descriptor 
     ********************************/

    aSQLStmt->mStmtCursorProperty.mIsDbcCursor             = STL_TRUE;
    aSQLStmt->mStmtCursorProperty.mProperty.mStandardType  = ELL_CURSOR_STANDARD_NA;
    
    aSQLStmt->mStmtCursorProperty.mProperty.mSensitivity   = ELL_CURSOR_SENSITIVITY_NA;
    aSQLStmt->mStmtCursorProperty.mProperty.mScrollability = ELL_CURSOR_SCROLLABILITY_NA;
    aSQLStmt->mStmtCursorProperty.mProperty.mHoldability   = ELL_CURSOR_HOLDABILITY_NA;
    aSQLStmt->mStmtCursorProperty.mProperty.mUpdatability  = ELL_CURSOR_UPDATABILITY_NA;
    aSQLStmt->mStmtCursorProperty.mProperty.mReturnability = ELL_CURSOR_RETURNABILITY_NA;

    aSQLStmt->mResultSetDesc = NULL;
    
    /********************************
     * Cycle Detector
     ********************************/

    aSQLStmt->mViewCycleList = NULL;
    
    /********************************
     * Long Type Data Value List
     ********************************/

    aSQLStmt->mLongTypeValueList.mCount = 0;
    aSQLStmt->mLongTypeValueList.mHead  = NULL;
    aSQLStmt->mLongTypeValueList.mTail  = NULL;

    /********************************
     * Trace Log를 위한 정보
     ********************************/

    STL_TRY( qllClearOptInfoSQL( aSQLStmt,
                                 aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
                          

/**
 * @brief SQL Statement 를 종료한다.
 * @param[in]  aDBCStmt  DBC Statement
 * @param[in]  aSQLStmt  SQL Statement
 * @param[in]  aEnv      Environment
 * @remarks
 */
stlStatus qllFiniSQL( qllDBCStmt   * aDBCStmt,
                      qllStatement * aSQLStmt,
                      qllEnv       * aEnv )
{
    if( aSQLStmt->mSqlHandle != NULL )
    {
        STL_TRY( knlUnfixSql( aSQLStmt->mSqlHandle,
                              KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        aSQLStmt->mSqlHandle = NULL;
    }
    

    /**
     * Long Type Data Value List 정리
     */

    if( aSQLStmt->mLongTypeValueList.mCount > 0 )
    {
        STL_TRY( qlndFreeLongTypeValues( & aSQLStmt->mLongTypeValueList,
                                         aEnv )
                 == STL_SUCCESS );
    }

    STL_DASSERT( aSQLStmt->mLongTypeValueList.mCount == 0 );
    STL_DASSERT( aSQLStmt->mLongTypeValueList.mHead  == NULL );
    STL_DASSERT( aSQLStmt->mLongTypeValueList.mTail  == NULL );

    
    /**
     * Heap 메모리 정리 
     */

    STL_TRY( knlClearRegionMem( & aEnv->mHeapMemParsing,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY( knlClearRegionMem( & aEnv->mHeapMemRunning,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Share 메모리 정리 
     */

    STL_TRY( knlClearRegionMem( & aDBCStmt->mShareMemInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY( knlClearRegionMem( & aDBCStmt->mNonPlanCacheMem,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY( knlClearRegionMem( & aDBCStmt->mPlanCacheMem,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlClearRegionMem( & aDBCStmt->mShareMemDataPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY( knlClearRegionMem( & aDBCStmt->mShareMemStmt,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    aDBCStmt->mShareMemCodePlan = NULL;

    return STL_SUCCESS;

    STL_FINISH;

    if( aSQLStmt->mLongTypeValueList.mCount > 0 )
    {
        (void) qlndFreeLongTypeValues( & aSQLStmt->mLongTypeValueList,
                                       aEnv );
    }

    STL_DASSERT( aSQLStmt->mLongTypeValueList.mCount == 0 );
    STL_DASSERT( aSQLStmt->mLongTypeValueList.mHead  == NULL );
    STL_DASSERT( aSQLStmt->mLongTypeValueList.mTail  == NULL );

    (void) knlClearRegionMem( & aEnv->mHeapMemParsing,
                              KNL_ENV(aEnv) );

    (void) knlClearRegionMem( & aEnv->mHeapMemRunning,
                              KNL_ENV(aEnv) );

    (void) knlClearRegionMem( & aDBCStmt->mShareMemInitPlan,
                              KNL_ENV(aEnv) );
    
    (void) knlClearRegionMem( & aDBCStmt->mNonPlanCacheMem,
                              KNL_ENV(aEnv) );
    
    (void) knlClearRegionMem( & aDBCStmt->mPlanCacheMem,
                              KNL_ENV(aEnv) );

    (void) knlClearRegionMem( & aDBCStmt->mShareMemDataPlan,
                              KNL_ENV(aEnv) );
    
    (void) knlClearRegionMem( & aDBCStmt->mShareMemStmt,
                              KNL_ENV(aEnv) );

    aDBCStmt->mShareMemCodePlan = NULL;
    
    return STL_FAILURE;
}


/**
 * @brief SQL Statement의 Opt Info를 Clear한다.
 * @param[in]  aSQLStmt  SQL Statement
 * @param[in]  aEnv      Environment
 * @remarks
 */

stlStatus qllClearOptInfoSQL( qllStatement  * aSQLStmt,
                              qllEnv        * aEnv )
{
    stlMemset( &(aSQLStmt->mOptInfo), 0x00, STL_SIZEOF( qllOptInfo ) );

    return STL_SUCCESS;
}


/**
 * @brief SQL Statement 를 RESET한다.
 * @param[in]  aDBCStmt  DBC Statement
 * @param[in]  aSQLStmt  SQL Statement
 * @param[in]  aEnv      Environment
 * @remarks
 */
stlStatus qllResetSQL( qllDBCStmt   * aDBCStmt,
                       qllStatement * aSQLStmt,
                       qllEnv       * aEnv )
{
    STL_TRY( qllFiniSQL( aDBCStmt, aSQLStmt, aEnv ) == STL_SUCCESS );

    STL_TRY( qllInitSQL( aSQLStmt, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Code Optimization 수행 전 메모리를 정리한다.
 * @param[in]  aDBCStmt  DBC Statement
 * @param[in]  aSQLStmt  SQL Statement
 * @param[in]  aEnv      Environment
 * @remarks
 * SQLExecute() 수행시 Optimize 를 재수행해야 할 경우 호출한다.
 * 첫 Optimize 일 경우 수행해도 무방하다.
 */
stlStatus qllCleanMemBeforeOptimizeCodeSQL( qllDBCStmt   * aDBCStmt,
                                            qllStatement * aSQLStmt,
                                            qllEnv       * aEnv )
{
    /*
     * Parameter Vaildation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    

    /**
     * Code/Data Plan 초기화 
     */

    aSQLStmt->mCodePlan = NULL;
    aSQLStmt->mDataPlan = NULL;

    /***************************************
     * Long Varying Memory 반납
     ***************************************/

    /**
     * Long Type Block 정리
     */

    if( aSQLStmt->mLongTypeValueList.mCount > 0 )
    {
        STL_TRY( qlndFreeLongTypeValues( & aSQLStmt->mLongTypeValueList,
                                         aEnv )
                 == STL_SUCCESS );
    }

    STL_DASSERT( aSQLStmt->mLongTypeValueList.mCount == 0 );
    STL_DASSERT( aSQLStmt->mLongTypeValueList.mHead  == NULL );
    STL_DASSERT( aSQLStmt->mLongTypeValueList.mTail  == NULL );

    /***************************************
     * Shared Memory 반납
     ***************************************/
    
    /**
     * Non-Cacheable Memory 반납
     */

    STL_TRY( knlClearRegionMem( & aDBCStmt->mNonPlanCacheMem,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Cacheable Memory 반납
     */

    STL_TRY( knlClearRegionMem( & aDBCStmt->mPlanCacheMem,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Data Plan Memory 반납
     */

    STL_TRY( knlClearRegionMem( & aDBCStmt->mShareMemDataPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /***************************************
     * Heap Memory 반납
     ***************************************/
    
    /**
     * Running Memory 반납
     */

    STL_TRY( knlClearRegionMem( & aEnv->mHeapMemRunning,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    

    return STL_SUCCESS;

    STL_FINISH;

    (void) qlndFreeLongTypeValues( & aSQLStmt->mLongTypeValueList, aEnv );
    
    (void) knlClearRegionMem( & aDBCStmt->mNonPlanCacheMem, KNL_ENV(aEnv) );
    (void) knlClearRegionMem( & aDBCStmt->mPlanCacheMem, KNL_ENV(aEnv) );
    (void) knlClearRegionMem( & aDBCStmt->mShareMemDataPlan, KNL_ENV(aEnv) );

    (void) knlClearRegionMem( & aEnv->mHeapMemRunning, KNL_ENV(aEnv) );
    
    return STL_FAILURE;
}

/**
 * @brief Data Optimization 수행 전 메모리를 정리한다.
 * @param[in]  aDBCStmt  DBC Statement
 * @param[in]  aSQLStmt  SQL Statement
 * @param[in]  aEnv      Environment
 * @remarks
 * SQLExecute() 수행시 Optimize 를 재수행해야 할 경우 호출한다.
 * 첫 Optimize 일 경우 수행해도 무방하다.
 */
stlStatus qllCleanMemBeforeOptimizeDataSQL( qllDBCStmt   * aDBCStmt,
                                            qllStatement * aSQLStmt,
                                            qllEnv       * aEnv )
{
    aSQLStmt->mDataPlan = NULL;

    /**
     * Long Type Block 정리
     */

    if( aSQLStmt->mLongTypeValueList.mCount > 0 )
    {
        STL_TRY( qlndFreeLongTypeValues( & aSQLStmt->mLongTypeValueList,
                                         aEnv )
                 == STL_SUCCESS );
    }

    STL_DASSERT( aSQLStmt->mLongTypeValueList.mCount == 0 );
    STL_DASSERT( aSQLStmt->mLongTypeValueList.mHead  == NULL );
    STL_DASSERT( aSQLStmt->mLongTypeValueList.mTail  == NULL );

    /**
     * Data Plan Memory 반납
     */

    STL_TRY( knlClearRegionMem( & aDBCStmt->mShareMemDataPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Running Memory 반납
     */

    STL_TRY( knlClearRegionMem( & aEnv->mHeapMemRunning,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) qlndFreeLongTypeValues( & aSQLStmt->mLongTypeValueList, aEnv );

    (void) knlClearRegionMem( & aDBCStmt->mShareMemDataPlan,
                              KNL_ENV(aEnv) );
    
    (void) knlClearRegionMem( & aEnv->mHeapMemRunning,
                              KNL_ENV(aEnv) );

    return STL_FAILURE;
}

/**
 * @brief SQL Statement 의 소분류 값을 얻는다.
 * @param[in] aSQLStmt SQL Statement
 * @return
 * SQL Statement 의 Type
 */
qllNodeType qllGetStmtType( qllStatement * aSQLStmt )
{
    return aSQLStmt->mStmtType;
}

/**
 * @brief SQL Statement 가 DDL 구문인지의 여부
 * @param[in] aSQLStmt  SQL Statement
 * @remarks
 */
stlBool  qllIsDDL( qllStatement * aSQLStmt )
{
    return ( ((aSQLStmt->mStmtAttr & QLL_STMT_ATTR_DDL_MASK) ==
              QLL_STMT_ATTR_DDL_YES) ? STL_TRUE : STL_FALSE );
}

/**
 * @brief SQL Statement 의 access mode를 얻는다.
 * @param[in] aSQLStmt  SQL Statement
 * @return
 * SQL Statement 의 Access Mode
 * @remarks
 */
qllAccessMode qllGetAccessMode( qllStatement * aSQLStmt )
{
    qllAccessMode sAccessMode = QLL_STMT_ATTR_DB_ACCESS_NONE;
    
    switch ( aSQLStmt->mStmtAttr & QLL_STMT_ATTR_DB_ACCESS_MASK )
    {
        case QLL_STMT_ATTR_DB_ACCESS_NONE:
            sAccessMode = QLL_ACCESS_MODE_NONE;
            break;
        case QLL_STMT_ATTR_DB_ACCESS_WRITE:
            sAccessMode = QLL_ACCESS_MODE_READ_WRITE;
            break;
        case QLL_STMT_ATTR_DB_ACCESS_READ:
            sAccessMode = QLL_ACCESS_MODE_READ_ONLY;
            break;
        case QLL_STMT_ATTR_DB_ACCESS_UNKNOWN:
            sAccessMode = QLL_ACCESS_MODE_UNKNOWN;
            break;
        default:
            STL_ASSERT(0);
            break;
    }

    return sAccessMode;
}


/**
 * @brief Unknown SQL Statement 의 access mode를 얻는다.
 * @param[in] aSQLStmt  SQL Statement
 * @return
 * SQL Statement 의 Access Mode
 * @remarks
 * Parsing 단계에서 Access Mode 를 알 수 없는 경우(OPEN cursor) Validation 이후 단계에서 Access Mode 를 얻는다.
 */
qllAccessMode qllGetAccessMode4UnknownStmt( qllStatement * aSQLStmt )
{
    qllAccessMode sAccessMode = QLL_STMT_ATTR_DB_ACCESS_NONE;

    qlcrInitOpenCursor * sOpenCursor = NULL;
    
    STL_DASSERT( (aSQLStmt->mStmtAttr & QLL_STMT_ATTR_DB_ACCESS_MASK) == QLL_STMT_ATTR_DB_ACCESS_UNKNOWN );
    STL_DASSERT( aSQLStmt->mInitPlan != NULL );
    
    switch( aSQLStmt->mStmtType )
    {
        case QLL_STMT_OPEN_CURSOR_TYPE:
            {
                sOpenCursor = (qlcrInitOpenCursor *) aSQLStmt->mInitPlan;
                switch ( sOpenCursor->mCursorStmtAttr & QLL_STMT_ATTR_DB_ACCESS_MASK )
                {
                    case QLL_STMT_ATTR_DB_ACCESS_NONE:
                        sAccessMode = QLL_ACCESS_MODE_NONE;
                        break;
                    case QLL_STMT_ATTR_DB_ACCESS_WRITE:
                        sAccessMode = QLL_ACCESS_MODE_READ_WRITE;
                        break;
                    case QLL_STMT_ATTR_DB_ACCESS_READ:
                        sAccessMode = QLL_ACCESS_MODE_READ_ONLY;
                        break;
                    case QLL_STMT_ATTR_DB_ACCESS_UNKNOWN:
                        STL_ASSERT(0);
                        break;
                    default:
                        STL_ASSERT(0);
                        break;
                }
                break;
            }
        default:
            STL_DASSERT(0);
            break;
    }

    return sAccessMode;
}



/**
 * @brief SQL Statement 가 Session의 상태 전이를 요구하는 구문인지의 여부 
 * @param[in] aSQLStmt  SQL Statement
 * @return
 * Session의 상태 전이를 요구한다면 STL_TRUE, 그러지 않다면 STL_FALSE를 반환
 * @remarks
 */
stlBool qllNeedStateTransition( qllStatement * aSQLStmt )
{
    return ( ((aSQLStmt->mStmtAttr & QLL_STMT_ATTR_STATE_TRANS_MASK) ==
              QLL_STMT_ATTR_STATE_TRANS_YES) ? STL_TRUE : STL_FALSE );
}

/**
 * @brief SQL Statement 가 강제 Commit을 요구하는 구문인지의 여부 
 * @param[in] aSQLStmt  SQL Statement
 * @return
 * Session의 Commit을 요구한다면 STL_TRUE, 그러지 않다면 STL_FALSE를 반환
 * @remarks
 */
stlBool qllNeedAutocommit( qllStatement * aSQLStmt )
{
    return ( ((aSQLStmt->mStmtAttr & QLL_STMT_ATTR_AUTOCOMMIT_MASK) ==
              QLL_STMT_ATTR_AUTOCOMMIT_YES) ? STL_TRUE : STL_FALSE );
}

/**
 * @brief SQL Statement 가 Global Transaction 내에서 수행될수 있는 구문인지의 여부 
 * @param[in] aSQLStmt  SQL Statement
 * @return
 * 가능하다면 STL_TRUE, 그러지 않다면 STL_FALSE를 반환
 * @remarks
 */
stlBool qllSupportGlobalTransaction( qllStatement * aSQLStmt )
{
    return ( ((aSQLStmt->mStmtAttr & QLL_STMT_ATTR_SUPPORT_GLOBAL_MASK) ==
              QLL_STMT_ATTR_SUPPORT_GLOBAL_YES) ? STL_TRUE : STL_FALSE );
}

/**
 * @brief SQL Statement 가 Snapshot Iterator가 필요한 구문인지 여부
 * @param[in] aSQLStmt  SQL Statement
 * @return
 * 필요하다면 STL_TRUE, 그러지 않다면 STL_FALSE를 반환
 * @remarks
 */
stlBool qllNeedSnapshotIterator( qllStatement * aSQLStmt )
{
    return ( ((aSQLStmt->mStmtAttr & QLL_STMT_ATTR_SNAPSHOT_ITERATOR_MASK) ==
              QLL_STMT_ATTR_SNAPSHOT_ITERATOR_YES) ? STL_TRUE : STL_FALSE );
}

/**
 * @brief SQL Statement 가 Lock을 요청할수 있는 구문인지의 여부 
 * @param[in] aSQLStmt  SQL Statement
 * @return
 * Session의 Lock을 요청할수 있다면 STL_TRUE, 그러지 않다면 STL_FALSE를 반환
 * @remarks
 */
stlBool qllIsLockableStmt( qllStatement * aSQLStmt )
{
    return ( ((aSQLStmt->mStmtAttr & QLL_STMT_ATTR_LOCKABLE_MASK) ==
              QLL_STMT_ATTR_LOCKABLE_YES) ? STL_TRUE : STL_FALSE );
}


/**
 * @brief Unknown SQL Statement 의 Lock을 요청할수 있는 구문인지의 여부를 얻는다.
 * @param[in] aSQLStmt  SQL Statement
 * @return
 * Session의 Lock을 요청할수 있다면 STL_TRUE, 그러지 않다면 STL_FALSE를 반환
 * @remarks
 * Unknown Statement 의 경우 Validation 이후 단계에서 정보를 얻는다.
 */
stlBool qllIsLockableStmt4UnknownStmt( qllStatement * aSQLStmt )
{
    stlBool  sResult = STL_FALSE;

    qlcrInitOpenCursor * sOpenCursor = NULL;
    
    STL_DASSERT( (aSQLStmt->mStmtAttr & QLL_STMT_ATTR_DB_ACCESS_MASK) == QLL_STMT_ATTR_DB_ACCESS_UNKNOWN );
    STL_DASSERT( aSQLStmt->mInitPlan != NULL );
    
    switch( aSQLStmt->mStmtType )
    {
        case QLL_STMT_OPEN_CURSOR_TYPE:
            {
                sOpenCursor = (qlcrInitOpenCursor *) aSQLStmt->mInitPlan;
                if ( (sOpenCursor->mCursorStmtAttr & QLL_STMT_ATTR_LOCKABLE_MASK) == QLL_STMT_ATTR_LOCKABLE_YES )
                {
                    sResult = STL_TRUE;
                }
                else
                {
                    sResult = STL_FALSE;
                }
                break;
            }
        default:
            {
                STL_DASSERT(0);
                break;
            }
    }

    return sResult;
}


/**
 * @brief SQL Statement 가 Atomic Execution을 지원하는지 여부
 * @param[in] aSQLStmt  SQL Statement
 * @return
 * Session의 지원한다면 STL_TRUE, 그러지 않다면 STL_FALSE를 반환
 * @remarks
 */
stlBool qllSupportAtomicExecution( qllStatement * aSQLStmt )
{
    return ( ((aSQLStmt->mStmtAttr & QLL_STMT_ATTR_ATOMIC_EXECUTION_MASK) ==
              QLL_STMT_ATTR_ATOMIC_EXECUTION_YES) ? STL_TRUE : STL_FALSE );
}


/**
 * @brief SQL Statement 가 Fetch 가 필요한 구문인지의 여부 
 * @param[in] aSQLStmt  SQL Statement
 * @remarks
 */
stlBool qllNeedFetch( qllStatement * aSQLStmt )
{
    return ( ((aSQLStmt->mStmtAttr & QLL_STMT_ATTR_FETCH_MASK) ==
              QLL_STMT_ATTR_FETCH_YES) ? STL_TRUE : STL_FALSE );
}


/**
 * @brief SQL Statement 가 SQL Cache에 대상이되는 구문인지의 여부 
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 * @remarks
 */
stlBool qllIsCacheableStmt( qllStatement * aSQLStmt,
                            qllEnv       * aEnv )
{
    if( (aSQLStmt->mStmtAttr & QLL_STMT_ATTR_PLAN_CACHE_MASK) ==
        QLL_STMT_ATTR_PLAN_CACHE_NO )
    {
        return STL_FALSE;
    }
    
    if( knlGetPropertyBoolValueByID( KNL_PROPERTY_PLAN_CACHE,
                                     KNL_ENV(aEnv) )
        == STL_FALSE )
    {
        return STL_FALSE;
    }
    
    return STL_TRUE;
}


/**
 * @brief SQL Statement 가 접근하는 Object 들이 최신 handle 인지 검사
 * @param[in] aTransID        Transaction ID
 * @param[in] aAccessObjList  접근하는 SQL Object List
 * @param[in] aTableStatList  접근하는 Table의 Stat List
 * @param[in] aIsValid        유효성 여부 
 * @param[in] aEnv            Environment
 * @return
 * - STL_TRUE : 모두 유효함
 * - STL_FALSE: 유효하지 않은 객체가 있음.
 * aTableStatList가 NULL인 경우 Table Stat에 대한 체크를 하지 않는다.
 */
stlStatus qllIsRecentAccessObject( smlTransId         aTransID,
                                   ellObjectList    * aAccessObjList,
                                   void             * aTableStatList,
                                   stlBool          * aIsValid,
                                   qllEnv           * aEnv )
{
    stlBool sResult = STL_FALSE;
    
    ellObjectItem * sObjectItem = NULL;

    stlInt64 sPageCount = 0;
    stlInt64 sRecompileCheckMinPageCount = 0;
    stlFloat64 sRecompilePagePercent = (stlFloat64)0.0;
    stlFloat64 sPercent = (stlFloat64)0.0;
    qloValidTblStatItem * sTableStatItem = NULL;

    /**
     * Paramter Validation
     */

    STL_DASSERT( aAccessObjList != NULL );

    /**
     * Object List 가 변경되었는 지 확인
     */

    sResult = STL_TRUE;

    if ( STL_RING_IS_EMPTY( & aAccessObjList->mHeadList ) == STL_TRUE )
    {
        sResult = STL_TRUE;
    }
    else
    {
        STL_RING_FOREACH_ENTRY( & aAccessObjList->mHeadList, sObjectItem )
        {
            STL_TRY( ellIsRecentDictHandle( aTransID,
                                            & sObjectItem->mObjectHandle,
                                            & sResult,
                                            ELL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            if ( sResult == STL_FALSE )
            {
                /**
                 * 유효하지 않은 Handle 임
                 */
                STL_THROW( RAMP_FINISH );
            }
        }
    }

    /**
     * Table Stat List의 통계정보가 변경되었는지 확인
     */

    STL_TRY_THROW( aTableStatList != NULL, RAMP_FINISH );

    sRecompilePagePercent =
        (stlFloat64)knlGetPropertyBigIntValueByID( KNL_PROPERTY_RECOMPILE_PAGE_PERCENT,
                                                   KNL_ENV(aEnv) );

    /* 값이 0이면 체크하지 않는다. */
    STL_TRY_THROW( sRecompilePagePercent > 0, RAMP_FINISH );

    sRecompileCheckMinPageCount =
        (stlFloat64)knlGetPropertyBigIntValueByID( KNL_PROPERTY_RECOMPILE_CHECK_MINIMUM_PAGE_COUNT,
                                                   KNL_ENV(aEnv) );

    sTableStatItem = ((qloValidTblStatList*)aTableStatList)->mHead;
    while( sTableStatItem != NULL )
    {
        STL_DASSERT( sTableStatItem->mPageCount > 0 );

        STL_TRY( smlGetUsedPageCount( sTableStatItem->mTablePhyHandle,
                                      &sPageCount,
                                      SML_ENV(aEnv) )
                 == STL_SUCCESS );

        if( sPageCount >= sRecompileCheckMinPageCount )
        {
            if( sPageCount == 0 )
            {
                sPageCount = 1; /* Default Page Count */
            }

            if( (stlFloat64)sPageCount >= sTableStatItem->mPageCount )
            {
                sPercent =
                    ( (stlFloat64)sPageCount - sTableStatItem->mPageCount ) / sTableStatItem->mPageCount
                    * (stlFloat64)100.0;
            }
            else
            {
                sPercent =
                    ( sTableStatItem->mPageCount - (stlFloat64)sPageCount ) / sTableStatItem->mPageCount
                    * (stlFloat64)100.0;
            }

            if( sPercent > sRecompilePagePercent )
            {
                sResult = STL_FALSE;
                STL_THROW( RAMP_FINISH );
            }
        }

        sTableStatItem = sTableStatItem->mNext;
    }



    STL_RAMP( RAMP_FINISH );


    /**
     * Output 설정
     */

    *aIsValid = sResult;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief SQL Statement 의 최근 수행한 SQL String 을 얻는다.
 * @param[in]  aSQLStmt SQL Statement
 * @return
 * - SQL String
 * - NULL if error occurred
 */
stlChar * qllGetSQLString( qllStatement * aSQLStmt )
{
    return aSQLStmt->mRetrySQL;
}


/**
 * @brief SQL Statement 의 Cursor Property 를 설정한다.
 * @param[in]  aSQLStmt         SQL Statement
 * @param[in]  aStandardType    Cursor Standard Type
 * @param[in]  aSensitivity     Cursor Sensitivity
 * @param[in]  aScrollability   Cursor Scrollability
 * @param[in]  aHoldability     Cursor Holdability
 * @param[in]  aUpdatability    Cursor Updatability
 * @param[in]  aReturnability   Cursor Returnability
 * @remarks
 */
void qllSetSQLStmtCursorProperty( qllStatement             * aSQLStmt,
                                  ellCursorStandardType      aStandardType,
                                  ellCursorSensitivity       aSensitivity,
                                  ellCursorScrollability     aScrollability,
                                  ellCursorHoldability       aHoldability,
                                  ellCursorUpdatability      aUpdatability,
                                  ellCursorReturnability     aReturnability )
{
    /**
     * Parameter Validation
     */

    STL_DASSERT( (aStandardType > ELL_CURSOR_STANDARD_NA) &&
                 (aStandardType < ELL_CURSOR_STANDARD_MAX) );
    STL_DASSERT( (aSensitivity > ELL_CURSOR_SENSITIVITY_NA) &&
                 (aSensitivity < ELL_CURSOR_SENSITIVITY_MAX) );
    STL_DASSERT( (aScrollability > ELL_CURSOR_SCROLLABILITY_NA) &&
                 (aScrollability < ELL_CURSOR_SCROLLABILITY_MAX) );
    STL_DASSERT( (aHoldability > ELL_CURSOR_HOLDABILITY_NA) &&
                 (aHoldability < ELL_CURSOR_HOLDABILITY_MAX) );
    STL_DASSERT( (aUpdatability > ELL_CURSOR_UPDATABILITY_NA) &&
                 (aUpdatability < ELL_CURSOR_UPDATABILITY_MAX) );
    STL_DASSERT( (aReturnability > ELL_CURSOR_RETURNABILITY_NA) &&
                 (aReturnability < ELL_CURSOR_RETURNABILITY_MAX) );

    /**
     * SQL Statement 의 Cursor Property 설정
     */

    aSQLStmt->mStmtCursorProperty.mIsDbcCursor             = STL_TRUE;
    aSQLStmt->mStmtCursorProperty.mProperty.mStandardType  = aStandardType;

    aSQLStmt->mStmtCursorProperty.mProperty.mSensitivity   = aSensitivity;
    aSQLStmt->mStmtCursorProperty.mProperty.mScrollability = aScrollability;
    aSQLStmt->mStmtCursorProperty.mProperty.mHoldability   = aHoldability;
    aSQLStmt->mStmtCursorProperty.mProperty.mUpdatability  = aUpdatability;
    aSQLStmt->mStmtCursorProperty.mProperty.mReturnability = aReturnability;
}




/**
 * @brief Query 의 Target Information 을 획득 
 * @param[in]  aSQLStmt            SQL Statement
 * @param[in]  aRegionMem          Region Memory
 * @param[out] aTargetExprList     Target 의 Expression List
 * @param[out] aTargetCodeStack    Target 의 Code Stack
 * @param[out] aTargetDataContext  Target 의 Data Context
 * @param[in]  aEnv                Environment
 * @return
 * ellCursorUpdatability
 */
stlStatus qllGetQueryTargetContext( qllStatement        * aSQLStmt,
                                    knlRegionMem        * aRegionMem,
                                    void               ** aTargetExprList,
                                    knlExprStack       ** aTargetCodeStack,
                                    knlExprContextInfo ** aTargetDataContext,
                                    qllEnv              * aEnv )
{
    qlnoSelectStmt       * sOptSelectStmt = NULL;
    qlnoInsertStmt       * sOptInsertStmt = NULL;
    qlnoUpdateStmt       * sOptUpdateStmt = NULL;
    qlnoDeleteStmt       * sOptDeleteStmt = NULL;
    qllOptimizationNode  * sOptNode       = NULL;
    qllOptimizationNode  * sOptQueryNode  = NULL;
    qlnoQuerySpec        * sOptQuerySpec  = NULL;
    qlnoQuerySet         * sOptQuerySet   = NULL;
    qllOptimizationNode  * sOptChildNode  = NULL;
    qlnoInstant          * sOptInstant    = NULL;
    qlvRefExprItem       * sRefExprItem   = NULL;
    qlvInitExpression    * sExpr          = NULL;
    stlInt32               sLoop          = 0;
    knlExprStack         * sCodeStack     = NULL;

    stlInt32               sTargetCount   = 0;
    qlvRefExprList       * sTargetColList = NULL;
    
    /**
     * Parameter Validation
     */

    STL_ASSERT( aTargetCodeStack != NULL );
    STL_ASSERT( aTargetDataContext != NULL );

    if ( aSQLStmt != NULL )
    {
        switch( aSQLStmt->mStmtType )
        {
            case QLL_STMT_SELECT_TYPE:
            case QLL_STMT_SELECT_FOR_UPDATE_TYPE:
            case QLL_STMT_SELECT_INTO_TYPE:
            case QLL_STMT_SELECT_INTO_FOR_UPDATE_TYPE:
                {
                    STL_ASSERT( aSQLStmt->mDataPlan != NULL );

                    sOptNode       = (qllOptimizationNode*)aSQLStmt->mCodePlan;
                    sOptSelectStmt = (qlnoSelectStmt*)sOptNode->mOptNode;
                    sOptQueryNode  = sOptSelectStmt->mQueryNode;

                    if( sOptQueryNode->mType == QLL_PLAN_NODE_QUERY_SPEC_TYPE )
                    {
                        sOptQuerySpec = (qlnoQuerySpec*) sOptQueryNode->mOptNode;

                        sTargetCount   = sOptQuerySpec->mTargetCount;
                        sTargetColList = sOptQuerySpec->mTargetColList;
                        sOptChildNode  = sOptQuerySpec->mChildNode;
                        sCodeStack   = sOptQuerySpec->mTargetsCodeStack;
                        
                    }
                    else
                    {
                        STL_DASSERT( sOptQueryNode->mType == QLL_PLAN_NODE_QUERY_SET_TYPE );
                        
                        sOptQuerySet = (qlnoQuerySet*) sOptQueryNode->mOptNode;

                        sTargetCount   = sOptQuerySet->mTargetCount;
                        sTargetColList = sOptQuerySet->mTargetList;
                        sOptChildNode  = sOptQuerySet->mChildNode;

                        /**
                         * Query Set 은 Target Stack 이 존재하지 않는다
                         */
                        
                        STL_TRY( knlAllocRegionMem(
                                     aRegionMem,
                                     STL_SIZEOF( knlExprStack ) * sTargetCount,
                                     (void **) & sCodeStack,
                                     KNL_ENV(aEnv) )
                                 == STL_SUCCESS );

                        for( sLoop = 0 ; sLoop < sTargetCount ; sLoop++ )
                        {
                            STL_TRY( knlExprInit( & sCodeStack[ sLoop ],
                                                  1,
                                                  aRegionMem,
                                                  KNL_ENV( aEnv ) )
                                     == STL_SUCCESS );
                        }
                    }

                    if ( sOptChildNode->mType != QLL_PLAN_NODE_SORT_INSTANT_ACCESS_TYPE )
                    {
                        /**
                         * ORDER BY 가 없는 경우
                         */
                        
                        *aTargetExprList = (void*) sTargetColList;
                        *aTargetCodeStack = sCodeStack;
                    }
                    else
                    {
                        sOptInstant   = (qlnoInstant*) sOptChildNode->mOptNode;
                        
                        /* Order By 절이 오는 경우 */
                        *aTargetExprList = (void*) sTargetColList;
                        
                        STL_TRY( knlAllocRegionMem(
                                     aRegionMem,
                                     STL_SIZEOF( knlExprStack ) * sTargetCount,
                                     (void **) & sCodeStack,
                                     KNL_ENV(aEnv) )
                                 == STL_SUCCESS );
                        
                        *aTargetCodeStack = sCodeStack;
                        
                        sRefExprItem = sTargetColList->mHead;
                        for( sLoop = 0 ; sLoop < sTargetCount ; sLoop++ )
                        {
                            if( sRefExprItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr->mType ==
                                QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT )
                            {
                                STL_TRY( knlExprInit( & sCodeStack[ sLoop ],
                                                      1,
                                                      aRegionMem,
                                                      KNL_ENV( aEnv ) )
                                         == STL_SUCCESS );
                            }
                            else
                            {
                                STL_DASSERT( sRefExprItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr->mType ==
                                             QLV_EXPR_TYPE_INNER_COLUMN );
                                
                                sExpr = sRefExprItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr;
                                
                                if( sOptInstant->mColExprStack[ *(sExpr->mExpr.mInnerColumn->mIdx) ] ==
                                    NULL )
                                {
                                    STL_TRY( knlExprInit( & sCodeStack[ sLoop ],
                                                          1,
                                                          aRegionMem,
                                                          KNL_ENV( aEnv ) )
                                             == STL_SUCCESS );
                                }
                                else
                                {
                                    stlMemcpy( & sCodeStack[ sLoop ],
                                               sOptInstant->mColExprStack[ *(sExpr->mExpr.mInnerColumn->mIdx) ],
                                               STL_SIZEOF( knlExprStack ) );
                                }
                            }
                            sRefExprItem = sRefExprItem->mNext;
                        }
                    }
                    
                    *aTargetDataContext =
                        ((qllDataArea*) aSQLStmt->mDataPlan)->mExprDataContext;
                    break;
                }
            case QLL_STMT_INSERT_RETURNING_QUERY_TYPE:
                {
                    STL_ASSERT( aSQLStmt->mDataPlan != NULL );

                    sOptNode = (qllOptimizationNode*)aSQLStmt->mCodePlan;
                    sOptInsertStmt = (qlnoInsertStmt*)sOptNode->mOptNode;

                    *aTargetExprList = 
                        (void*)sOptInsertStmt->mReturnExprList;
                    *aTargetCodeStack =
                        sOptInsertStmt->mReturnExprStack;
                    *aTargetDataContext =
                        ((qllDataArea*) aSQLStmt->mDataPlan)->mExprDataContext;
                    break;
                }
            case QLL_STMT_UPDATE_RETURNING_QUERY_TYPE:
                {
                    STL_ASSERT( aSQLStmt->mDataPlan != NULL );

                    sOptNode = (qllOptimizationNode*)aSQLStmt->mCodePlan;
                    sOptUpdateStmt = (qlnoUpdateStmt*)sOptNode->mOptNode;

                    *aTargetExprList = 
                        (void*)sOptUpdateStmt->mReturnExprList;
                    *aTargetCodeStack =
                        sOptUpdateStmt->mReturnExprStack;
                    *aTargetDataContext =
                        ((qllDataArea*) aSQLStmt->mDataPlan)->mExprDataContext;
                    break;
                }
            case QLL_STMT_DELETE_RETURNING_QUERY_TYPE:
                {
                    STL_ASSERT( aSQLStmt->mDataPlan != NULL );

                    sOptNode = (qllOptimizationNode*)aSQLStmt->mCodePlan;
                    sOptDeleteStmt = (qlnoDeleteStmt*)sOptNode->mOptNode;

                    *aTargetExprList = 
                        (void*)sOptDeleteStmt->mReturnExprList;
                    *aTargetCodeStack =
                        sOptDeleteStmt->mReturnExprStack;
                    *aTargetDataContext =
                        ((qllDataArea*) aSQLStmt->mDataPlan)->mExprDataContext;
                    break;
                }
            default:
                {
                    *aTargetExprList    = NULL;
                    *aTargetCodeStack   = NULL;
                    *aTargetDataContext = NULL;
                    break;
                }
        }
    }
    else
    {
        *aTargetExprList    = NULL;
        *aTargetCodeStack   = NULL;
        *aTargetDataContext = NULL;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief DCL의 유형을 얻는다.
 * @param[in]  aSQLStmt SQL Statement
 * @return qllDCLType
 */
inline qllDCLType qllGetDCLType( qllStatement * aSQLStmt )
{
    return aSQLStmt->mDCLContext.mType;
}

/**
 * @brief DCL의 Access Mode를 얻는다.
 * @param[in]  aSQLStmt SQL Statement
 * @return stlInt16
 */
inline stlInt16 qllGetDCLAccessMode( qllStatement * aSQLStmt )
{
    return aSQLStmt->mDCLContext.mAccessMode;
}

/**
 * @brief DCL의 Isolation Level을 얻는다.
 * @param[in]  aSQLStmt SQL Statement
 * @return smlIsolationLevel
 */
inline smlIsolationLevel qllGetDCLIsolationLevel( qllStatement * aSQLStmt )
{
    return aSQLStmt->mDCLContext.mIsolationLevel;
}

/**
 * @brief DCL의 Unique Mode를 얻는다.
 * @param[in]  aSQLStmt SQL Statement
 * @return stlInt16
 */
inline stlInt16 qllGetDCLUniqueMode( qllStatement * aSQLStmt )
{
    return aSQLStmt->mDCLContext.mUniqueMode;
}

/**
 * @brief DCL의 GMT Offset을 얻는다.
 * @param[in]  aSQLStmt SQL Statement
 * @return stlInt16
 */
inline stlInt16 qllGetDCLGMTOffset( qllStatement * aSQLStmt )
{
    return aSQLStmt->mDCLContext.mGMTOffset;
}

/**
 * @brief Atomic 여부를 설정한다.
 * @param[in]   aSQLStmt   SQL Statement
 * @param[in]   aIsAtomic  Atomic 여부
 */
void qllSetAtomicAttr( qllStatement  * aSQLStmt, stlBool aIsAtomic )
{
    aSQLStmt->mIsAtomic = aIsAtomic;
}


/**
 * @brief Atomic 여부를 반환한다.
 * @param[in]   aSQLStmt   SQL Statement
 * @return
 * stlBool
 * @remarks
 */
stlBool qllGetAtomicAttr( qllStatement  * aSQLStmt )
{
    return aSQLStmt->mIsAtomic;
}


/**
 * @brief Init Plan을 반환한다.
 * @param[in]   aSQLStmt   SQL Statement
 * @return
 * qlvInitPlan *
 * @remarks DML에 한하여 stmt의 init plan이 없는 경우 plan cache에 저장된 init plan node가 반환
 */
inline void * qllGetInitPlan( qllStatement  * aSQLStmt )
{
    return ( ( aSQLStmt->mInitPlan != NULL )
             ? aSQLStmt->mInitPlan
             : (void *) QLL_GET_CACHED_INIT_PLAN( aSQLStmt->mCodePlan ) );
}


/** @} qllStatement */



