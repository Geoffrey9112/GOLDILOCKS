/*******************************************************************************
 * qlxDelete.c
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
 * @file qlxDelete.c
 * @brief SQL Processor Layer DELETE statement execution
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @addtogroup qlxDelete
 * @{
 */


/**
 * @brief DELETE 구문을 실행한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement 
 * @param[in] aSQLStmt   SQL Statement 객체
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlxExecuteDelete( smlTransId      aTransID,
                            smlStatement  * aStmt,
                            qllDBCStmt    * aDBCStmt,
                            qllStatement  * aSQLStmt,
                            qllEnv        * aEnv )
{
    qlvInitDelete        * sInitPlan       = NULL;
    qllOptimizationNode  * sOptNode       = NULL;
    qllDataArea          * sDataArea      = NULL;

    qllResultSetDesc       * sResultSetDesc = NULL;
    qlnInstantTable        * sInstantTable  = NULL;

    stlBool             sIsScrollableInstant = STL_FALSE;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_DELETE_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_DELETE_WHERE_CURRENT_OF_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_DELETE_RETURNING_QUERY_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_DELETE_RETURNING_INTO_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mDataPlan != NULL, QLL_ERROR_STACK(aEnv) );


    /******************************************************
     * 정보 획득 
     ******************************************************/
    
    /**
     * 검증된 Plan 정보 획득 
     */

    STL_TRY( qlxDeleteGetValidPlan( aTransID,
                                    aDBCStmt,
                                    aSQLStmt,
                                    QLL_PHASE_EXECUTE,
                                    & sInitPlan,
                                    & sOptNode,
                                    & sDataArea,
                                    aEnv )
             == STL_SUCCESS );

    /**
     * Update Relation Handle 설정
     */

    STL_TRY( smlUpdateRelHandle( aStmt,
                                 sInitPlan->mTablePhyHandle,
                                 SML_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**************************************************
     * Instant Table 생성 
     **************************************************/

    if ( aSQLStmt->mStmtType == QLL_STMT_DELETE_RETURNING_QUERY_TYPE )
    {
        sResultSetDesc = aSQLStmt->mResultSetDesc;
        sInstantTable  = sResultSetDesc->mInstantTable;

        if ( sResultSetDesc->mCursorProperty.mScrollability == ELL_CURSOR_SCROLLABILITY_SCROLL )
        {
            sIsScrollableInstant = STL_TRUE;
        }
        else
        {
            sIsScrollableInstant = STL_FALSE;
        }

        stlMemset( sInstantTable, 0x00, STL_SIZEOF( qlnInstantTable ) );
        
        STL_TRY( qlnxCreateFlatInstantTable( aTransID,
                                             aStmt,
                                             sInstantTable,
                                             sIsScrollableInstant,
                                             aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /**
         * nothing to do
         */
    }

    /**
     * transaction id 설정
     */
    
    sDataArea->mTransID = aTransID;    
    
    /******************************************************
     * Initialize Execution Node 
     ******************************************************/

    if( sDataArea->mIsFirstExecution == STL_TRUE )
    {
        QLNX_INITIALIZE_NODE( aTransID,
                              aStmt,
                              aDBCStmt,
                              aSQLStmt,
                              sOptNode,
                              sDataArea,
                              aEnv );

        /* 초기화 Flag 설정 */
        sDataArea->mIsFirstExecution = STL_FALSE;
    }
    else
    {
        /* Do Nothing */
    }

    
    /******************************************************
     * Execute Execution Node 
     ******************************************************/

    QLNX_EXECUTE_NODE( aTransID,
                       aStmt,
                       aDBCStmt,
                       aSQLStmt,
                       sOptNode,
                       sDataArea,
                       aEnv );


    /******************************************************
     * RETURN 절 처리
     ******************************************************/

    /**
     * 하위 노드의 qlxDeleteValidBlockRow 함수에서 처리함
     */


    /******************************************************
     * INTO 절 처리
     ******************************************************/

    /**
     * 상위 RETURN 절 처리에서 INTO 블럭을 처리했음
     */

    /******************************************************
     * Finalize Execution Node
     ******************************************************/

    QLNX_FINALIZE_NODE( sOptNode,
                        sDataArea,
                        aEnv );

    return STL_SUCCESS;
    
    STL_FINISH;

    if ( sInstantTable != NULL )
    {
        (void) qlnxDropInstantTable( sInstantTable,
                                     aEnv );
    }
    
    /* Execution Node 종료 */
    QLNX_FINALIZE_NODE_IGNORE_FAILURE( sOptNode,
                                       sDataArea,
                                       aEnv );

    return STL_FAILURE;
}


/**
 * @brief DELETE 구문의 Constraint 들의 Check Time (IMMEDIATE, DEFERRED) 을 획득
 * @param[in] aInitRelObject  Update Init Related Object
 * @param[in] aOptNode        Optimization Node
 * @param[in] aDataArea       Data Area (Data Optimization 결과물)
 * @param[in] aEnv            Environment
 * @remarks
 */
stlStatus qlxDeleteGetConstraintCheckTime(  qlvInitDeleteRelObject * aInitRelObject,
                                            qllOptimizationNode    * aOptNode,
                                            qllDataArea            * aDataArea,
                                            qllEnv                 * aEnv )
{
    qllExecutionNode      * sExecNode = NULL;
    qlnxDeleteStmt        * sExecDeleteStmt = NULL;
    
    stlInt32 i = 0;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aInitRelObject != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * DELETE STATEMENT Execution Node 획득
     */

    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );
    sExecDeleteStmt = sExecNode->mExecNode;

    
    /**
     * PRIMARY KEY constraint 의 check time 획득
     */
    
    if ( aInitRelObject->mPrimaryKeyCnt > 0 )
    {
        for ( i = 0; i < aInitRelObject->mPrimaryKeyCnt; i++ )
        {
            if ( qllGetTransUniqueMode(aEnv) == QLL_UNIQUE_MODE_DEFERRED )
            {
                sExecDeleteStmt->mRelObject.mPrimaryKeyIsDeferred[i] = STL_TRUE;
            }
            else
            {
                sExecDeleteStmt->mRelObject.mPrimaryKeyIsDeferred[i] =
                    qlrcIsCheckTimeDeferred( & aInitRelObject->mPrimaryKeyHandle[i], aEnv );
            }
            
            sExecDeleteStmt->mRelObject.mPrimaryKeyCollCnt[i] = 0;
        }
    }

    /**
     * UNIQUE constraint 의 check time 획득
     */
    
    if ( aInitRelObject->mUniqueKeyCnt > 0 )
    {
        for ( i = 0; i < aInitRelObject->mUniqueKeyCnt; i++ )
        {
            if ( qllGetTransUniqueMode(aEnv) == QLL_UNIQUE_MODE_DEFERRED )
            {
                sExecDeleteStmt->mRelObject.mUniqueKeyIsDeferred[i] = STL_TRUE;
            }
            else
            {
                sExecDeleteStmt->mRelObject.mUniqueKeyIsDeferred[i] =
                    qlrcIsCheckTimeDeferred( & aInitRelObject->mUniqueKeyHandle[i], aEnv );
            }
            sExecDeleteStmt->mRelObject.mUniqueKeyCollCnt[i] = 0;
        }
    }

    /**
     * FOREIGN constraint 의 check time 획득
     */
    
    if ( aInitRelObject->mForeignKeyCnt > 0 )
    {
        for ( i = 0; i < aInitRelObject->mForeignKeyCnt; i++ )
        {
            sExecDeleteStmt->mRelObject.mForeignKeyIsDeferred[i] =
                qlrcIsCheckTimeDeferred( & aInitRelObject->mForeignKeyHandle[i], aEnv );

            sExecDeleteStmt->mRelObject.mForeignKeyCollCnt[i] = 0;
        }
    }

    /**
     * @todo CHILD FOREIGN KEY constraint 의 check time 획득
     */
    
    /**
     * deferrable NOT NULL constraint 의 check time 획득
     */

    if ( aInitRelObject->mDeferNotNullCnt > 0 )
    {
        for ( i = 0; i < aInitRelObject->mDeferNotNullCnt; i++ )
        {
            sExecDeleteStmt->mRelObject.mNotNullIsDeferred[i] =
                qlrcIsCheckTimeDeferred( & aInitRelObject->mDeferNotNullHandle[i], aEnv );

            sExecDeleteStmt->mRelObject.mNotNullCollCnt[i] = 0;
        }
    }
    
    /**
     * CHECK constraint 의 check time 획득
     */

    /**
     * UNIQUE Index의 check time 획득
     */
    
    if ( aInitRelObject->mUniqueIndexCnt > 0 )
    {
        for ( i = 0; i < aInitRelObject->mUniqueIndexCnt; i++ )
        {
            if ( qllGetTransUniqueMode(aEnv) == QLL_UNIQUE_MODE_DEFERRED )
            {
                sExecDeleteStmt->mRelObject.mUniqueIndexIsDeferred[i] = STL_TRUE;
            }
            else
            {
                sExecDeleteStmt->mRelObject.mUniqueIndexIsDeferred[i] = STL_FALSE;
            }
            sExecDeleteStmt->mRelObject.mUniqueIndexCollCnt[i] = 0;
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief DELETE 구문의 Constraint 들의 Collision Count 를 RESET
 * @param[in] aInitRelObject  Update Init Related Object
 * @param[in] aOptNode        Optimization Node
 * @param[in] aDataArea       Data Area (Data Optimization 결과물)
 * @param[in] aEnv            Environment
 * @remarks
 */
stlStatus qlxDeleteResetConstraintCollision( qlvInitDeleteRelObject * aInitRelObject,
                                             qllOptimizationNode    * aOptNode,
                                             qllDataArea            * aDataArea,
                                             qllEnv                 * aEnv )
{
    qllExecutionNode      * sExecNode = NULL;
    qlnxDeleteStmt        * sExecDeleteStmt = NULL;
    
    stlInt32 i = 0;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aInitRelObject != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * DELETE STATEMENT Execution Node 획득
     */

    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );
    sExecDeleteStmt = sExecNode->mExecNode;

    
    /**
     * PRIMARY KEY constraint 
     */
    
    if ( aInitRelObject->mPrimaryKeyCnt > 0 )
    {
        for ( i = 0; i < aInitRelObject->mPrimaryKeyCnt; i++ )
        {
            sExecDeleteStmt->mRelObject.mPrimaryKeyCollCnt[i] = 0;
        }
    }

    /**
     * UNIQUE constraint 
     */
    
    if ( aInitRelObject->mUniqueKeyCnt > 0 )
    {
        for ( i = 0; i < aInitRelObject->mUniqueKeyCnt; i++ )
        {
            sExecDeleteStmt->mRelObject.mUniqueKeyCollCnt[i] = 0;
        }
    }

    /**
     * FOREIGN constraint 
     */
    
    if ( aInitRelObject->mForeignKeyCnt > 0 )
    {
        for ( i = 0; i < aInitRelObject->mForeignKeyCnt; i++ )
        {
            sExecDeleteStmt->mRelObject.mForeignKeyCollCnt[i] = 0;
        }
    }

    /**
     * @todo CHILD FOREIGN KEY constraint 
     */
    
    /**
     * NOT NULL constraint 
     */

    if ( aInitRelObject->mDeferNotNullCnt > 0 )
    {
        for ( i = 0; i < aInitRelObject->mDeferNotNullCnt; i++ )
        {
            sExecDeleteStmt->mRelObject.mNotNullCollCnt[i] = 0;
        }
    }
    
    /**
     * @todo CHECK constraint 
     */

    /**
     * UNIQUE Index
     */
    
    if ( aInitRelObject->mUniqueIndexCnt > 0 )
    {
        for ( i = 0; i < aInitRelObject->mUniqueIndexCnt; i++ )
        {
            sExecDeleteStmt->mRelObject.mUniqueIndexCollCnt[i] = 0;
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief DELETE 수행을 위한 Lock 을 잡고 유효한 Plan 을 얻는다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aDBCStmt      DBC Statement
 * @param[in]  aSQLStmt      SQL Statement
 * @param[in]  aQueryPhase   Query Processing Phase
 * @param[out] aInitPlan     DELETE Init Plan
 * @param[out] aCodePlan     DELETE Code Plan    
 * @param[out] aDataPlan     DELETE Data Plan
 * @param[in]  aEnv          Environment
 */
stlStatus qlxDeleteGetValidPlan( smlTransId              aTransID,
                                 qllDBCStmt            * aDBCStmt,
                                 qllStatement          * aSQLStmt,
                                 qllQueryPhase           aQueryPhase,
                                 qlvInitDelete        ** aInitPlan,
                                 qllOptimizationNode  ** aCodePlan,
                                 qllDataArea          ** aDataPlan,
                                 qllEnv                * aEnv )
{
    qlvInitDelete        * sInitPlan = NULL;
    qllOptimizationNode  * sCodePlan = NULL;
    qllDataArea          * sDataPlan = NULL;
    
    stlBool  sIsValid       = STL_FALSE;
    stlBool  sSegmentExist  = STL_FALSE;
    stlBool  sNeedRecompile = STL_FALSE;

    /**
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_DELETE_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_DELETE_WHERE_CURRENT_OF_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_DELETE_RETURNING_QUERY_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_DELETE_RETURNING_INTO_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aQueryPhase > QLL_PHASE_VALIDATION) &&
                        (aQueryPhase <= QLL_PHASE_EXECUTE),
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 설정
     */

    sInitPlan = (qlvInitDelete *) qllGetInitPlan( aSQLStmt );
    sCodePlan = (qllOptimizationNode *) aSQLStmt->mCodePlan;
    sDataPlan = (qllDataArea *) aSQLStmt->mDataPlan;

    
    /**
     * Handle Validation
     */

    STL_TRY( ellIsRecentDictHandle( aTransID,
                                    & sInitPlan->mTableHandle,
                                    & sIsValid,
                                    ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    if( sIsValid == STL_TRUE )
    {
        if ( sInitPlan->mInitPlan.mValidationObjList == NULL )
        {
            STL_DASSERT( qloGetValidTblStatList( aSQLStmt ) == NULL );
            sIsValid = STL_TRUE;
        }
        else
        {
            STL_TRY( qllIsRecentAccessObject( aTransID,
                                              sInitPlan->mInitPlan.mValidationObjList,
                                              ( aQueryPhase == QLL_PHASE_EXECUTE
                                                ? (void*)qloGetValidTblStatList( aSQLStmt )
                                                : NULL ),
                                              & sIsValid,
                                              aEnv )
                     == STL_SUCCESS );
        }
    }
    else
    {
        sIsValid = STL_FALSE;
    }

    if( sIsValid == STL_TRUE )
    {
        /**
         * Table 의 구조가 변하지 않음
         */

        sNeedRecompile = STL_FALSE;
    }
    else
    {
        /**
         * Table 의 구조가 변함
         */
        
        sNeedRecompile = STL_TRUE;
    }

    /**
     * 유효한 Plan 을 얻을 때까지 Recompile 을 반복
     */
    
    while ( 1 )
    {
        STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );
        
        if ( sNeedRecompile == STL_TRUE )
        {
            if( aSQLStmt->mSqlHandle != NULL )
            {
                /**
                 * Plan을 삭제한다.
                 */
                
                STL_TRY( knlDiscardSql( aSQLStmt->mSqlHandle,
                                        KNL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                STL_TRY( knlUnfixSql( aSQLStmt->mSqlHandle,
                                      KNL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                aSQLStmt->mSqlHandle = NULL;
            }
            
            STL_TRY( qlgRecompileSQL( aTransID,
                                      aDBCStmt,
                                      aSQLStmt,
                                      STL_TRUE,  /* aSearchPlanCache */
                                      aQueryPhase,
                                      aEnv ) == STL_SUCCESS );
            
            /**
             * 기본 정보를 다시 획득
             */
            
            sInitPlan = (qlvInitDelete *) qllGetInitPlan( aSQLStmt );
            sCodePlan = (qllOptimizationNode *) aSQLStmt->mCodePlan;
            sDataPlan = (qllDataArea *) aSQLStmt->mDataPlan;
        }
        else
        {
            /**
             * while 문이 반복될 경우 Recompile 을 해야 함.
             */
            sNeedRecompile = STL_TRUE;
        }
    
        /**
         * Query 수행 단계에 따라 Lock 획득
         */
    
        if ( aQueryPhase == QLL_PHASE_EXECUTE )
        {
            /**
             * IX LOCK 획득
             * - Statement 가 시작되었으므로 
             * - 구조가 변경되었다 하더라도 Physical Handle 은 Aging 되지 않아 Lock 설정이 가능하다.
             */
            
            STL_TRY( smlLockTable( aTransID,
                                   SML_LOCK_IX,
                                   sInitPlan->mTablePhyHandle,
                                   SML_LOCK_TIMEOUT_INFINITE,  
                                   &sSegmentExist,
                                   SML_ENV(aEnv) )
                     == STL_SUCCESS );

            if ( sSegmentExist == STL_TRUE )
            {
                /* segment 가 유효함 */
            }
            else
            {
                /* segment 가 존재하지 않는 경우 */
                continue;
            }

            /**
             * Lock 이후의 Table Handle 의 유효성 검증
             */

            STL_TRY( ellIsRecentDictHandle( aTransID,
                                            & sInitPlan->mTableHandle,
                                            & sIsValid,
                                            ELL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            if ( sIsValid == STL_TRUE )
            {
                if( sInitPlan->mInitPlan.mValidationObjList == NULL )
                {
                    STL_DASSERT( qloGetValidTblStatList( aSQLStmt ) == NULL );
                    sIsValid = STL_TRUE;
                }
                else
                {
                    STL_TRY( qllIsRecentAccessObject( aTransID,
                                                      sInitPlan->mInitPlan.mValidationObjList,
                                                      ( aQueryPhase == QLL_PHASE_EXECUTE
                                                        ? (void*)qloGetValidTblStatList( aSQLStmt )
                                                        : NULL ),
                                                      & sIsValid,
                                                      aEnv )
                             == STL_SUCCESS );
                }
            }
            else
            {
                sIsValid = STL_FALSE;
            }

            if( sIsValid == STL_TRUE )
            {
                /**
                 * - Handle 검사와 IX lock 중간에 Table 구조가 변경되었는지 검사
                 * - IX lock 이후에는 Table 구조 및 관련 Object(Index, Constraint 등)이 변경되지 않는다.
                 */
            }
            else
            {
                /**
                 * 다시 Recompile 해야 함
                 */

                continue;
            }
        }
        else
        {
            /**
             * Execute 이전에는 Lock 을 잡지 않음
             */
        }

        break;
    }

    /**
     * Output 설정
     */

    if ( aInitPlan != NULL )
    {
        *aInitPlan = sInitPlan;
    }
            
    if ( aCodePlan != NULL )
    {
        *aCodePlan = sCodePlan;
    }

    if ( aDataPlan != NULL )
    {
        *aDataPlan = sDataPlan;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Block Read 한 Row 들 중 유효한 것들만 제거한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aDBCStmt         DBC Statement
 * @param[in]  aSQLStmt         SQL Statement
 * @param[in]  aDataArea        Data Area
 * @param[in]  aExecNode        DELETE Statement Execution Node
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus qlxDeleteValidBlockRow( smlTransId             aTransID,
                                  smlStatement         * aStmt,
                                  qllDBCStmt           * aDBCStmt,
                                  qllStatement         * aSQLStmt,
                                  qllDataArea          * aDataArea,
                                  qllExecutionNode     * aExecNode,
                                  qllEnv               * aEnv )
{
    qlvInitDelete       * sInitPlan        = NULL;
    qlnoDeleteStmt      * sOptDeleteStmt   = NULL;
    qlnxDeleteStmt      * sExeDeleteStmt   = NULL;

    dtlDataValue          sDataValue;
    dtlBooleanType        sBoolValue       = STL_FALSE;

    smlFetchRecordInfo  * sFetchRecordInfo = NULL;
    
    stlInt32              sBlockReadCnt    = 0;
    stlBool               sVersionConflict = STL_FALSE;
    stlBool               sSkipped         = STL_FALSE;
    stlBool               sDeleted         = STL_FALSE;
    stlBool               sIsMatched       = STL_FALSE;
    
    stlInt32              sBlockIdx        = 0;

    qlnxFetchNodeInfo     sFetchNodeInfo;
    

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExecNode != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * DELETE STATEMENT Node 획득
     */

    sInitPlan      = (qlvInitDelete *) qllGetInitPlan( aSQLStmt );
    sOptDeleteStmt = (qlnoDeleteStmt *) aExecNode->mOptNode->mOptNode;
    sExeDeleteStmt = (qlnxDeleteStmt *) aExecNode->mExecNode;
    
    /**
     * retry filter 수행 결과 저장하기 위한 data value 초기화
     */
    
    STL_TRY( dtlInitDataValue( DTL_TYPE_BOOLEAN,
                               STL_SIZEOF(dtlBooleanType),
                               & sDataValue,
                               QLL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    sDataValue.mValue = & sBoolValue;

    
    /**
     * Block 단위로 Query Timeout 검사
     */
    
    STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );

        
    /**
     * Block Read 한 Row 들 중
     * 유효한 ( 모든 WHERE 조건을 만족하고, Version Conflict 가 발생하지 않은 )
     * Row 들만을 삭제한다.
     */

    /* fetch record 정보 설정 */
    sFetchRecordInfo = & sExeDeleteStmt->mFetchRecordInfo;

    sFetchRecordInfo->mRelationHandle  = sOptDeleteStmt->mPhysicalHandle;
    sFetchRecordInfo->mColList         = sExeDeleteStmt->mReadColBlock;
    sFetchRecordInfo->mRowIdColList    = sExeDeleteStmt->mRowIdColBlock;
    sFetchRecordInfo->mPhysicalFilter  = NULL;
    sFetchRecordInfo->mTransReadMode   = SML_TRM_COMMITTED;
    sFetchRecordInfo->mStmtReadMode    = SML_SRM_SNAPSHOT;
    sFetchRecordInfo->mScnBlock        = sExeDeleteStmt->mRowBlock->mScnBlock;
    sFetchRecordInfo->mLogicalFilter   = NULL;
    sFetchRecordInfo->mLogicalFilterEvalInfo = NULL;
    
    /* SM이 모든 컬럼을 다시 읽도록 block의 used count를 초기화한다. */
    sBlockReadCnt = sExeDeleteStmt->mRowBlock->mUsedBlockSize;
    KNL_SET_ALL_BLOCK_USED_CNT( sExeDeleteStmt->mReadColBlock, 0 );
    KNL_SET_ALL_BLOCK_USED_CNT( sExeDeleteStmt->mRowIdColBlock, 0 );


    /**
     * 조건을 만족하는 Row 만 넘어오기 때문에 모든 Row를 삭제
     */
    
    QLNX_INIT_FETCH_NODE_INFO( & sFetchNodeInfo,
                               aTransID,
                               aStmt,
                               aDBCStmt,
                               aSQLStmt,
                               aDataArea );

    for( sBlockIdx = 0; sBlockIdx < sBlockReadCnt; sBlockIdx++ )
    {
        STL_RAMP( BEGIN_DELETE );
        
        /***********************************************
         * Row 삭제 
         ***********************************************/

        sVersionConflict = STL_FALSE;
        sSkipped = STL_FALSE;
        sDeleted = STL_FALSE;
        
        sExeDeleteStmt->mIsDeleted[ sBlockIdx ] = STL_FALSE;

        STL_TRY( smlDeleteRecord(
                     aStmt,
                     sOptDeleteStmt->mPhysicalHandle,
                     & sExeDeleteStmt->mRowBlock->mRidBlock[ sBlockIdx ],
                     sExeDeleteStmt->mRowBlock->mScnBlock[ sBlockIdx ],
                     sBlockIdx,
                     sExeDeleteStmt->mRelObject.mSuppLogPrimaryKeyBlock,
                     & sVersionConflict,
                     & sSkipped,
                     SML_ENV(aEnv) )
                 == STL_SUCCESS );

            
        /**
         * Version Conflict 에 대한 처리
         */
                    
        if( sVersionConflict != STL_TRUE )
        {
            if ( sSkipped == STL_TRUE )
            {
                /**
                 * Version Conflict 이전에 이미 삭제에 성공함
                 */

                sExeDeleteStmt->mIsDeleted[ sBlockIdx ] = STL_FALSE;
            }
            else
            {
                /**
                 * Version Conflict 없이 Row 삭제를 성공함
                 */
                    
                aDataArea->mWriteRowCnt++;
                sExeDeleteStmt->mIsDeleted[ sBlockIdx ] = STL_TRUE;
                    
                /**
                 * DELETE 문장 유형에 따른 deleted row 처리
                 */
                    
                switch ( aSQLStmt->mStmtType )
                {
                    case QLL_STMT_DELETE_TYPE:
                    case QLL_STMT_DELETE_WHERE_CURRENT_OF_TYPE:
                        {
                            /**
                             * nothing to do
                             */
                            break;
                        }
                    case QLL_STMT_DELETE_RETURNING_QUERY_TYPE:
                        {
                            if( sOptDeleteStmt->mReturnChildNode != NULL )
                            {
                                knlValueBlockList   * sOuterColBlock;
                                knlValueBlockList   * sOuterOrgColBlock;
                                knlValueBlockList   * sRefColBlock;
                                qlnxSubQueryList    * sExeSubQueryList;
                                stlInt32              sLoop;
                                stlBool               sEOF;
                                stlInt64              sTmpUsedBlockCnt;

                                /* Outer Column Block Value 설정 */
                                if( sExeDeleteStmt->mReturnOuterColBlock != NULL )
                                {
                                    STL_DASSERT( sExeDeleteStmt->mReturnOuterOrgColBlock != NULL );

                                    sOuterColBlock    = sExeDeleteStmt->mReturnOuterColBlock;
                                    sOuterOrgColBlock = sExeDeleteStmt->mReturnOuterOrgColBlock;

                                    while( sOuterColBlock != NULL )
                                    {
                                        STL_TRY( knlCopyDataValue( KNL_GET_BLOCK_DATA_VALUE( sOuterOrgColBlock, sBlockIdx ),
                                                                   KNL_GET_BLOCK_FIRST_DATA_VALUE( sOuterColBlock ),
                                                                   KNL_ENV( aEnv ) )
                                                 == STL_SUCCESS );

                                        sOuterColBlock    = sOuterColBlock->mNext;
                                        sOuterOrgColBlock = sOuterOrgColBlock->mNext;
                                    }
                                }

                                /* Return SubQuery Fetch 수행 */
                                QLNX_EXECUTE_NODE( aTransID,
                                                   aStmt,
                                                   aDBCStmt,
                                                   aSQLStmt,
                                                   aDataArea->mExecNodeList[ sOptDeleteStmt->mReturnChildNode->mIdx ].mOptNode,
                                                   aDataArea,
                                                   aEnv );

                                sEOF = STL_FALSE;
                                QLNX_FETCH_NODE( & sFetchNodeInfo,
                                                 sOptDeleteStmt->mReturnChildNode->mIdx,
                                                 0, /* Start Idx */
                                                 0,
                                                 1,
                                                 & sTmpUsedBlockCnt,
                                                 & sEOF,
                                                 aEnv );

                                sExeSubQueryList = QLL_GET_EXECUTION_NODE( aDataArea,
                                                                           sOptDeleteStmt->mReturnChildNode->mIdx )->mExecNode;

                                if( sBlockIdx > 0 )
                                {
                                    for( sLoop = 0 ; sLoop < sExeSubQueryList->mSubQueryCnt ; sLoop++ )
                                    {
                                        sRefColBlock = sExeSubQueryList->mRefColBlockList[ sLoop ];

                                        while( sRefColBlock != NULL )
                                        {
                                            if( DTL_GET_BLOCK_IS_SEP_BUFF( sRefColBlock ) == STL_TRUE )
                                            {
                                                STL_TRY( knlCopyDataValue(
                                                             KNL_GET_BLOCK_FIRST_DATA_VALUE( sRefColBlock ),
                                                             KNL_GET_BLOCK_DATA_VALUE( sRefColBlock, sBlockIdx ),
                                                             KNL_ENV( aEnv ) )
                                                         == STL_SUCCESS );
                                            }
                                            sRefColBlock = sRefColBlock->mNext;
                                        }
                                    }
                                }
                            }

                            STL_TRY( qlnxExecuteReturnExprBlock( aDataArea,
                                                                 STL_TRUE, /* aIsReturnQuery */
                                                                 aDataArea->mTargetBlock,
                                                                 sBlockIdx,
                                                                 sOptDeleteStmt->mReturnExprCnt,
                                                                 sOptDeleteStmt->mReturnExprStack,
                                                                 aEnv )
                                     == STL_SUCCESS );

                            STL_TRY( qlnxAddMeetBlockIntoInstantTable(
                                         aSQLStmt->mResultSetDesc->mInstantTable,
                                         aSQLStmt->mResultSetDesc->mInstantFetchInfo.mRowBlock,
                                         aDataArea->mTargetBlock,
                                         sExeDeleteStmt->mIsDeleted,
                                         sBlockIdx,
                                         sBlockIdx+1,
                                         aEnv )
                                     == STL_SUCCESS );

                            break;
                        }
                    case QLL_STMT_DELETE_RETURNING_INTO_TYPE:
                        {
                            /* Outer Column Block Value 설정 */
                            if( sExeDeleteStmt->mReturnOuterColBlock != NULL )
                            {
                                knlValueBlockList   * sOuterColBlock    = NULL;
                                knlValueBlockList   * sOuterOrgColBlock = NULL;

                                STL_DASSERT( sExeDeleteStmt->mReturnOuterOrgColBlock != NULL );

                                sOuterColBlock    = sExeDeleteStmt->mReturnOuterColBlock;
                                sOuterOrgColBlock = sExeDeleteStmt->mReturnOuterOrgColBlock;

                                while( sOuterColBlock != NULL )
                                {
                                    STL_TRY( knlCopyDataValue( KNL_GET_BLOCK_DATA_VALUE( sOuterOrgColBlock, 0 ),
                                                               KNL_GET_BLOCK_FIRST_DATA_VALUE( sOuterColBlock ),
                                                               KNL_ENV( aEnv ) )
                                             == STL_SUCCESS );

                                    sOuterColBlock    = sOuterColBlock->mNext;
                                    sOuterOrgColBlock = sOuterOrgColBlock->mNext;
                                }
                            }

                            /* SubQuery를 위한 Child Node 수행 */
                            if( sOptDeleteStmt->mReturnChildNode != NULL )
                            {
                                stlBool       sEOF;
                                stlInt64      sTmpUsedBlockCnt;

                                QLNX_EXECUTE_NODE( aTransID,
                                                   aStmt,
                                                   aDBCStmt,
                                                   aSQLStmt,
                                                   aDataArea->mExecNodeList[ sOptDeleteStmt->mReturnChildNode->mIdx ].mOptNode,
                                                   aDataArea,
                                                   aEnv );

                                sEOF = STL_FALSE;
                                QLNX_FETCH_NODE( & sFetchNodeInfo,
                                                 sOptDeleteStmt->mReturnChildNode->mIdx,
                                                 0, /* Start Idx */
                                                 0,
                                                 1,
                                                 & sTmpUsedBlockCnt,
                                                 & sEOF,
                                                 aEnv );
                            }

                            STL_TRY( qlnxExecuteReturnExprBlock( aDataArea,
                                                                 STL_FALSE, /* aIsReturnQuery */
                                                                 aDataArea->mIntoBlock,
                                                                 sBlockIdx,
                                                                 sOptDeleteStmt->mReturnExprCnt,
                                                                 sOptDeleteStmt->mReturnExprStack,
                                                                 aEnv )
                                     == STL_SUCCESS );
                            KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( aDataArea->mIntoBlock, 0, 1 );
                            
                            break;
                        }
                    default:
                        {
                            STL_ASSERT(0);
                            break;
                        }
                }
            }
        }
        else
        {
            /**************************************************
             * Version Conflict 가 발생함.
             * - 즉, 다른 Tx 이 변경 또는 삭제 후 Commit 함.
             **************************************************/

            switch ( sInitPlan->mSerialAction )
            {
                case QLV_SERIAL_NO_RETRY:
                    {
                        /**********************************************************
                         * NO RETRY 인 경우 (WHERE 절이 없는 경우)
                         **********************************************************/
                            
                        /**
                         * commit된 최신 레코드를 다시 읽음
                         */
                            
                        STL_TRY( smlFetchRecord(
                                     aStmt,
                                     sFetchRecordInfo,
                                     & sExeDeleteStmt->mRowBlock->mRidBlock[ sBlockIdx ],
                                     sBlockIdx,
                                     & sIsMatched,
                                     & sDeleted,
                                     NULL, /* aUpdated */
                                     SML_ENV(aEnv) )
                                 == STL_SUCCESS );

                        /* debugging 용 assert */
                        if ( sDeleted == STL_FALSE )
                        {
                            STL_DASSERT( sIsMatched == STL_TRUE );
                        }
                        
                        if( ( sDeleted == STL_FALSE ) && ( sIsMatched == STL_TRUE ) )
                        {
                            /**
                             * 삭제되지 않은 정상 Row 임, DELETE 를 다시 시도 
                             */
                            STL_THROW( BEGIN_DELETE );
                        }
                        else
                        {
                            /**
                             * 다른 Tx 에 의해 Data 가 삭제되어 COMMIT 됨
                             * 삭제하지 않아야 할 Row 임.
                             */
                                
                            sExeDeleteStmt->mIsDeleted[ sBlockIdx ] = STL_FALSE;
                            break;
                        }
                        break;
                    }
                case QLV_SERIAL_RETRY_ONLY:
                case QLV_SERIAL_ROLLBACK_RETRY:
                    {
                        /**
                         * RETRY 를 수행을 해야 함.
                         * - RETRY 를 수행하기 전에 이미 삭제한 Row 는 인덱스 정보도 갱신해주어야 함.
                         * - 아직 삭제하지 않은 Row 는 인덱스 정보를 갱신하지 않도록 함.
                         */

                        
                        for( ; sBlockIdx < sBlockReadCnt; sBlockIdx++ )
                        {
                            sExeDeleteStmt->mIsDeleted[ sBlockIdx ] = STL_FALSE;
                        }

                        /**
                         * Query 를 재수행이 필요함을 설정한다.
                         */

                        sExeDeleteStmt->mVersionConflict = STL_TRUE;
                        STL_THROW( FINISH_DELETE_VALID_BLOCK_ROW );
                            
                        break;
                    }
                default:
                    {
                        STL_ASSERT(0);
                        break;
                    }
            }
        }
        
    } /* for */

    
    STL_RAMP( FINISH_DELETE_VALID_BLOCK_ROW );
    
    /* SM이 모든 컬럼을 다시 읽었기 때문에 block의 used count를 원복한다. */
    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sExeDeleteStmt->mReadColBlock, 0, sBlockReadCnt );
    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sExeDeleteStmt->mRowIdColBlock, 0, sBlockReadCnt );

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief DELETE 의 Check Constraint 무결성을 검증한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aDataArea        Data Area
 * @param[in]  aExecNode        DELETE Statement Execution Node
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus qlxDeleteIntegrityCheckConstraint( smlTransId         aTransID,
                                             smlStatement     * aStmt,
                                             qllDataArea      * aDataArea,
                                             qllExecutionNode * aExecNode,
                                             qllEnv           * aEnv )
{
    qlnoDeleteStmt          * sOptDeleteStmt = NULL;
    qlnxDeleteStmt          * sExeDeleteStmt = NULL;

    qlvInitDeleteRelObject  * sInitRelObject = NULL;
    qlxExecDeleteRelObject  * sExecRelObject = NULL;

    stlInt32                sCollisionCnt = 0;

    stlInt32 i = 0;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExecNode != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * DELETE STATEMENT Node 획득
     */

    sOptDeleteStmt = (qlnoDeleteStmt *) aExecNode->mOptNode->mOptNode;
    sExeDeleteStmt = (qlnxDeleteStmt *) aExecNode->mExecNode;
    sInitRelObject = sOptDeleteStmt->mRelObject;
    sExecRelObject = & sExeDeleteStmt->mRelObject;

    /*****************************************************
     * Deferred NOT NULL Constraint 검사
     *****************************************************/
    
    for ( i = 0; i < sInitRelObject->mDeferNotNullCnt; i++ )
    {
        sCollisionCnt = 0;
        
        if ( sExecRelObject->mNotNullIsDeferred[i] == STL_TRUE )
        {
            qlrcCheckNotNull4Delete( sExeDeleteStmt->mIsDeleted,
                                     sExecRelObject->mNotNullColumnBlock[i],
                                     & sCollisionCnt );
                                    
            
            if ( sCollisionCnt == 0 )
            {
                /**
                 * Collision 해소가 없음
                 */
            }
            else
            {
                /**
                 * Collision 해소 정보를 누적
                 */
                
                STL_DASSERT( sCollisionCnt < 0 );
                
                sExecRelObject->mNotNullCollCnt[i] += sCollisionCnt;
            }
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


    
/**
 * @brief DELETE 의 Key Constraint 무결성을 검증하고 Index Key 를 삭제한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aDataArea        Data Area
 * @param[in]  aExecNode        DELETE Statement Execution Node
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus qlxDeleteIntegrityKeyConstIndexKey( smlTransId         aTransID,
                                              smlStatement     * aStmt,
                                              qllDataArea      * aDataArea,
                                              qllExecutionNode * aExecNode,
                                              qllEnv           * aEnv )
{
    qlnoDeleteStmt          * sOptDeleteStmt = NULL;
    qlnxDeleteStmt          * sExeDeleteStmt = NULL;

    qlvInitDeleteRelObject  * sInitRelObject = NULL;
    qlxExecDeleteRelObject  * sExecRelObject = NULL;

    stlInt32    sCollisionCnt = 0;
    stlInt32    sKeyCollCnt = 0;
    stlInt32    sNullCollCnt = 0;
    
    
    smlUniqueViolationScope sUniqueViolationScope;

    stlInt32 i = 0;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExecNode != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * DELETE STATEMENT Node 획득
     */

    sOptDeleteStmt = (qlnoDeleteStmt *) aExecNode->mOptNode->mOptNode;
    sExeDeleteStmt = (qlnxDeleteStmt *) aExecNode->mExecNode;
    sInitRelObject = sOptDeleteStmt->mRelObject;
    sExecRelObject = & sExeDeleteStmt->mRelObject;


    /*****************************************************
     * Primary Key Constraint 검사
     *****************************************************/
    
    for ( i = 0; i < sInitRelObject->mPrimaryKeyCnt; i++ )
    {
        /**
         * 각 Row 의 Primary Key Constraint 검사
         */

        STL_DASSERT( SML_USED_BLOCK_SIZE( sExeDeleteStmt->mRowBlock )
                     == KNL_GET_BLOCK_USED_CNT( sExecRelObject->mPrimaryKeyIndexBlock[i] ) );
        
        sCollisionCnt = 0;

        /**
         * deferred 일 경우 not null 속성 검사 
         */

        if ( sExecRelObject->mPrimaryKeyIsDeferred[i] == STL_TRUE )
        {
            qlrcCheckNotNull4Delete( sExeDeleteStmt->mIsDeleted,
                                    sExecRelObject->mPrimaryKeyIndexBlock[i],
                                    & sNullCollCnt );
        }
        else
        {
            sNullCollCnt = 0;
        }
        
        /**
         * Index Key 삭제 
         */
                
        STL_TRY( smlDeleteKeys( aStmt,
                                sInitRelObject->mPrimaryKeyIndexPhyHandle[i],
                                sExecRelObject->mPrimaryKeyIndexBlock[i],
                                sExeDeleteStmt->mRowBlock,
                                sExeDeleteStmt->mIsDeleted,
                                & sKeyCollCnt,
                                & sUniqueViolationScope,
                                SML_ENV(aEnv) )
                 == STL_SUCCESS );

        /**
         * Collision 처리
         */
                
        STL_DASSERT( (sUniqueViolationScope != SML_UNIQUE_VIOLATION_SCOPE_RESOLVED) ||
                     (sExecRelObject->mPrimaryKeyIsDeferred[i] == STL_TRUE) );

        sCollisionCnt = sNullCollCnt + sKeyCollCnt;
        
        if ( sCollisionCnt == 0 )
        {
            /**
             * Collision 해소가 없음
             */
        }
        else
        {
            /**
             * Collision 해소 정보를 누적
             */
            
            STL_DASSERT( sCollisionCnt < 0 );

            sExecRelObject->mPrimaryKeyCollCnt[i] += sCollisionCnt;
        }
    }


    /*****************************************************
     * Unique Key Constraint 검사
     *****************************************************/

    for ( i = 0; i < sInitRelObject->mUniqueKeyCnt; i++ )
    {
        /**
         * 각 Row 의 Unique Key Constraint 검사
         */

        STL_DASSERT( SML_USED_BLOCK_SIZE( sExeDeleteStmt->mRowBlock )
                     == KNL_GET_BLOCK_USED_CNT( sExecRelObject->mUniqueKeyIndexBlock[i] ) );

        sCollisionCnt = 0;
        
        /**
         * Index Key 삭제 
         */
                
        STL_TRY( smlDeleteKeys( aStmt,
                                sInitRelObject->mUniqueKeyIndexPhyHandle[i],
                                sExecRelObject->mUniqueKeyIndexBlock[i],
                                sExeDeleteStmt->mRowBlock,
                                sExeDeleteStmt->mIsDeleted,
                                & sCollisionCnt,
                                & sUniqueViolationScope,
                                SML_ENV(aEnv) )
                 == STL_SUCCESS );

        /**
         * Collision 처리 
         */

        STL_DASSERT( (sUniqueViolationScope != SML_UNIQUE_VIOLATION_SCOPE_RESOLVED) ||
                     (sExecRelObject->mUniqueKeyIsDeferred[i] == STL_TRUE) );
        
        if ( sCollisionCnt == 0 )
        {
            /**
             * Collision 해소가 없음
             */
        }
        else
        {
            /**
             * Collision 해소 정보를 누적
             */
            
            STL_DASSERT( sCollisionCnt < 0 );

            sExecRelObject->mUniqueKeyCollCnt[i] += sCollisionCnt;
        }
    }


    /*****************************************************
     * Foreign Key Constraint 검사
     *****************************************************/

    /**
     * @todo FOREIGN KEY 구현해야 함
     */

    STL_TRY_THROW( sInitRelObject->mForeignKeyCnt == 0, RAMP_ERR_NOT_IMPLEMENTED );
    
    /*****************************************************
     * Child Foreign Key 검사
     *****************************************************/

    /**
     * @todo Child Foreign Key 가 DELETE 로 인해 Constraint 를 위배하는 지 검사해야 함.
     * - Self Reference 가 있다면 Stmt Collision 목록을 관리해야 함.
     */

    STL_TRY_THROW( sInitRelObject->mChildForeignKeyCnt == 0, RAMP_ERR_NOT_IMPLEMENTED );

    /*****************************************************
     * Unique Index 에 Index Key 삭제 
     *****************************************************/
    
    for ( i = 0; i < sInitRelObject->mUniqueIndexCnt; i++ )
    {
        /**
         * 각 Row 에 대응하는  Unique Index 의 Key 제거
         */

        STL_DASSERT( SML_USED_BLOCK_SIZE( sExeDeleteStmt->mRowBlock )
                     == KNL_GET_BLOCK_USED_CNT( sExecRelObject->mUniqueIndexBlock[i] ) );

        sCollisionCnt = 0;
        
        STL_TRY( smlDeleteKeys( aStmt,
                                sInitRelObject->mUniqueIndexPhyHandle[i],
                                sExecRelObject->mUniqueIndexBlock[i],
                                sExeDeleteStmt->mRowBlock,
                                sExeDeleteStmt->mIsDeleted,
                                & sCollisionCnt,
                                & sUniqueViolationScope,
                                SML_ENV(aEnv) )
                 == STL_SUCCESS );

        /**
         * Collision 처리 
         */

        STL_DASSERT( (sUniqueViolationScope != SML_UNIQUE_VIOLATION_SCOPE_RESOLVED) ||
                     (sExecRelObject->mUniqueIndexIsDeferred[i] == STL_TRUE) );
        
        if ( sCollisionCnt == 0 )
        {
            /**
             * Collision 해소가 없음
             */
        }
        else
        {
            /**
             * Collision 해소 정보를 누적
             */
            
            STL_DASSERT( sCollisionCnt < 0 );

            sExecRelObject->mUniqueIndexCollCnt[i] += sCollisionCnt;
        }
    }
    

    /*****************************************************
     * Non-Unique Index 에 Index Key 삭제 
     *****************************************************/
    
    for ( i = 0; i < sInitRelObject->mNonUniqueIndexCnt; i++ )
    {
        /**
         * 각 Row 에 대응하는  Non-Unique Index 의 Key 제거
         */

        sCollisionCnt = 0;
        
        STL_DASSERT( SML_USED_BLOCK_SIZE( sExeDeleteStmt->mRowBlock )
                     == KNL_GET_BLOCK_USED_CNT( sExecRelObject->mNonUniqueIndexBlock[i] ) );

        STL_TRY( smlDeleteKeys( aStmt,
                                sInitRelObject->mNonUniqueIndexPhyHandle[i],
                                sExecRelObject->mNonUniqueIndexBlock[i],
                                sExeDeleteStmt->mRowBlock,
                                sExeDeleteStmt->mIsDeleted,
                                & sCollisionCnt,
                                & sUniqueViolationScope,
                                SML_ENV(aEnv) )
                 == STL_SUCCESS );
        
        STL_DASSERT( sUniqueViolationScope == SML_UNIQUE_VIOLATION_SCOPE_NONE );
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "qlxDeleteIntegrityKeyConstIndexKey()" );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief DELETE 구문에 대한 collision 정보를 검사한다.
 * @param[in] aTransID          Transaction ID
 * @param[in] aStmt             SM statement
 * @param[in] aDataArea         Data Area
 * @param[in] aExecNode         UPDATE Statement Execution Node
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus qlxDeleteCheckStmtCollision( smlTransId          aTransID,
                                       smlStatement      * aStmt,
                                       qllDataArea       * aDataArea,
                                       qllExecutionNode  * aExecNode,
                                       qllEnv            * aEnv )
{
    qlnoDeleteStmt          * sOptDeleteStmt = NULL;
    qlnxDeleteStmt          * sExeDeleteStmt = NULL;
    qlvInitDeleteRelObject  * sInitRelObject = NULL;
    qlxExecDeleteRelObject  * sExecRelObject = NULL;

    stlInt32                  i = 0;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExecNode != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * DELETE STATEMENT Node 획득
     */

    sOptDeleteStmt = (qlnoDeleteStmt *) aExecNode->mOptNode->mOptNode;
    sExeDeleteStmt = (qlnxDeleteStmt *) aExecNode->mExecNode;
    sInitRelObject = sOptDeleteStmt->mRelObject;
    sExecRelObject = & sExeDeleteStmt->mRelObject;

    /**
     * Primary Key 의 Immediate Constraint 위반 Check 
     */

    sInitRelObject = sOptDeleteStmt->mRelObject;
    
    for( i = 0; i < sInitRelObject->mPrimaryKeyCnt; i++ )
    {
        if ( sExecRelObject->mPrimaryKeyCollCnt[i] != 0 )
        {
            /* collision 이 해소되는 경우만 있어야 함 */
            STL_DASSERT( sExecRelObject->mPrimaryKeyCollCnt[i] < 0 );
            /* IMMEDIATE 일 경우 이미 에러가 발생했어야 함 */
            STL_DASSERT( sExecRelObject->mPrimaryKeyIsDeferred[i] == STL_TRUE );

            /* INSERT, UPDATE 가 constraint 를 위반한 상태에서 supplemental log data 를 남길 수 없다. */
            STL_DASSERT( sExecRelObject->mIsSuppLog == STL_FALSE );
            
            /**
             * Deferred Constarint 정보에 등록
             */

            STL_TRY( qlrcSetDeferredCollision( QLL_COLL_ITEM_CONSTRAINT,
                                               & sInitRelObject->mPrimaryKeyHandle[i],
                                               sExecRelObject->mPrimaryKeyCollCnt[i],
                                               aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /* nothing to to */
        }
    }

    /**
     * Unique Key 의 Immediate Constraint 위반 Check 
     */
    
    for( i = 0; i < sInitRelObject->mUniqueKeyCnt; i++ )
    {
        if ( sExecRelObject->mUniqueKeyCollCnt[i] != 0 )
        {
            /* collision 이 해소되는 경우만 있어야 함 */
            STL_DASSERT( sExecRelObject->mUniqueKeyCollCnt[i] < 0 );
            /* IMMEDIATE 일 경우 이미 에러가 발생했어야 함 */
            STL_DASSERT( sExecRelObject->mUniqueKeyIsDeferred[i] == STL_TRUE );

            /**
             * Deferred Constarint 정보에 등록
             */

            STL_TRY( qlrcSetDeferredCollision( QLL_COLL_ITEM_CONSTRAINT,
                                               & sInitRelObject->mUniqueKeyHandle[i],
                                               sExecRelObject->mUniqueKeyCollCnt[i],
                                               aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /* nothing to to */
        }
    }

    /**
     * @todo Foreign Key 에 대한 고려가 필요함.
     */
    
    for ( i = 0; i < sInitRelObject->mForeignKeyCnt; i++ )
    {
        if ( sExecRelObject->mForeignKeyCollCnt[i] != 0 )
        {
            STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
        }
    }

    /**
     * @todo CHILD FOREIGN KEY 에 대한 고려가 필요함
     */

    /**
     * Deferred NOT NULL 의 Immediate Constraint 위반 Check 
     */

    for( i = 0; i < sInitRelObject->mDeferNotNullCnt; i++ )
    {
        if ( sExecRelObject->mNotNullCollCnt[i] != 0 )
        {
            /* collision 이 해소되는 경우만 있어야 함 */
            STL_DASSERT( sExecRelObject->mNotNullCollCnt[i] < 0 );
            /* IMMEDIATE 일 경우 이미 에러가 발생했어야 함 */
            STL_DASSERT( sExecRelObject->mNotNullIsDeferred[i] == STL_TRUE );

            /**
             * Deferred Constarint 정보에 등록
             */

            STL_TRY( qlrcSetDeferredCollision( QLL_COLL_ITEM_CONSTRAINT,
                                               & sInitRelObject->mDeferNotNullHandle[i],
                                               sExecRelObject->mNotNullCollCnt[i],
                                               aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /* nothing to to */
        }
    }
    
    /**
     * @todo CHECK 에 대한 고려가 필요함
     */


    /**
     * Unique Index 의 Immediate Constraint 위반 Check 
     */
    
    for( i = 0; i < sInitRelObject->mUniqueIndexCnt; i++ )
    {
        if ( sExecRelObject->mUniqueIndexCollCnt[i] != 0 )
        {
            /* collision 이 해소되는 경우만 있어야 함 */
            STL_DASSERT( sExecRelObject->mUniqueIndexCollCnt[i] < 0 );
            /* IMMEDIATE 일 경우 이미 에러가 발생했어야 함 */
            STL_DASSERT( sExecRelObject->mUniqueIndexIsDeferred[i] == STL_TRUE );

            /**
             * Deferred Constarint 정보에 등록
             */

            STL_TRY( qlrcSetDeferredCollision( QLL_COLL_ITEM_UNIQUE_INDEX,
                                               & sInitRelObject->mUniqueIndexHandle[i],
                                               sExecRelObject->mUniqueIndexCollCnt[i],
                                               aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /* nothing to to */
        }
    }

    
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "qlxDeleteCheckStmtCollision()" );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} qlxDelete */


