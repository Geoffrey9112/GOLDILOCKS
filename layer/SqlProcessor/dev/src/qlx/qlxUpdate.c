/*******************************************************************************
 * qlxUpdate.c
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
 * @file qlxUpdate.c
 * @brief SQL Processor Layer UPDATE statement execution
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @addtogroup qlxUpdate
 * @{
 */


/**
 * @brief UPDATE 구문을 실행한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement 객체
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlxExecuteUpdate( smlTransId      aTransID,
                            smlStatement  * aStmt,
                            qllDBCStmt    * aDBCStmt,
                            qllStatement  * aSQLStmt,
                            qllEnv        * aEnv )
{
    qlvInitUpdate        * sInitPlan       = NULL;
    qllOptimizationNode  * sOptNode       = NULL;
    qllDataArea          * sDataArea      = NULL;

    qllResultSetDesc       * sResultSetDesc = NULL;
    qlnInstantTable        * sInstantTable = NULL;

    stlBool             sIsScrollableInstant = STL_FALSE;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_UPDATE_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_UPDATE_WHERE_CURRENT_OF_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_UPDATE_RETURNING_INTO_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_UPDATE_RETURNING_QUERY_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mDataPlan != NULL, QLL_ERROR_STACK(aEnv) );


    /******************************************************
     * 정보 획득 
     ******************************************************/
    
    /**
     * 검증된 Plan 정보 획득 
     */

    STL_TRY( qlxUpdateGetValidPlan( aTransID,
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

    if ( aSQLStmt->mStmtType == QLL_STMT_UPDATE_RETURNING_QUERY_TYPE )
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
     * 하위 노드의 qlxUpdateValidBlockRow 함수에서 처리함
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
 * @brief UPDATE 구문의 Constraint 들의 Check Time (IMMEDIATE, DEFERRED) 을 획득
 * @param[in] aInitRelObject  Update Init Related Object
 * @param[in] aOptNode        Optimization Node
 * @param[in] aDataArea       Data Area (Data Optimization 결과물)
 * @param[in] aEnv            Environment
 * @remarks
 */
stlStatus qlxUpdateGetConstraintCheckTime(  qlvInitUpdateRelObject * aInitRelObject,
                                            qllOptimizationNode    * aOptNode,
                                            qllDataArea            * aDataArea,
                                            qllEnv                 * aEnv )
{
    qllExecutionNode      * sExecNode = NULL;
    qlnxUpdateStmt        * sExeUpdateStmt = NULL;
    
    stlInt32 i = 0;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aInitRelObject != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * UPDATE STATEMENT Execution Node 획득
     */

    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );
    sExeUpdateStmt = sExecNode->mExecNode;
    
    /**
     * PRIMARY KEY constraint 의 check time 획득
     */
    
    if ( aInitRelObject->mAffectPrimaryKeyCnt > 0 )
    {
        for ( i = 0; i < aInitRelObject->mAffectPrimaryKeyCnt; i++ )
        {
            if ( qllGetTransUniqueMode(aEnv) == QLL_UNIQUE_MODE_DEFERRED )
            {
                sExeUpdateStmt->mRelObject.mPrimaryKeyIsDeferred[i] = STL_TRUE;
            }
            else
            {
                sExeUpdateStmt->mRelObject.mPrimaryKeyIsDeferred[i] =
                    qlrcIsCheckTimeDeferred( & aInitRelObject->mAffectPrimaryKeyHandle[i], aEnv );
            }
            
            sExeUpdateStmt->mRelObject.mPrimaryKeyCollCnt[i] = 0;
        }
    }

    /**
     * UNIQUE constraint 의 check time 획득
     */
    
    if ( aInitRelObject->mAffectUniqueKeyCnt > 0 )
    {
        for ( i = 0; i < aInitRelObject->mAffectUniqueKeyCnt; i++ )
        {
            if ( qllGetTransUniqueMode(aEnv) == QLL_UNIQUE_MODE_DEFERRED )
            {
                sExeUpdateStmt->mRelObject.mUniqueKeyIsDeferred[i] = STL_TRUE;
            }
            else
            {
                sExeUpdateStmt->mRelObject.mUniqueKeyIsDeferred[i] =
                    qlrcIsCheckTimeDeferred( & aInitRelObject->mAffectUniqueKeyHandle[i], aEnv );
            }

            sExeUpdateStmt->mRelObject.mUniqueKeyCollCnt[i] = 0;
        }
    }

    /**
     * @todo FOREIGN KEY constraint 의 check time 획득
     */
    
    /**
     * @todo CHILD FOREIGN KEY constraint 의 check time 획득
     */
    
    /**
     * NOT NULL constraint 의 check time 획득
     */

    if ( aInitRelObject->mAffectCheckNotNullCnt > 0 )
    {
        for ( i = 0; i < aInitRelObject->mAffectCheckNotNullCnt; i++ )
        {
            sExeUpdateStmt->mRelObject.mNotNullIsDeferred[i] =
                qlrcIsCheckTimeDeferred( & aInitRelObject->mAffectCheckNotNullHandle[i], aEnv );

            sExeUpdateStmt->mRelObject.mNotNullCollCnt[i] = 0;
        }
    }
    
    /**
     * @todo CHECK constraint 의 check time 획득
     */

    /**
     * UNIQUE Index 의 check time 획득
     */
    
    if ( aInitRelObject->mAffectUniqueIndexCnt > 0 )
    {
        for ( i = 0; i < aInitRelObject->mAffectUniqueIndexCnt; i++ )
        {
            if ( qllGetTransUniqueMode(aEnv) == QLL_UNIQUE_MODE_DEFERRED )
            {
                sExeUpdateStmt->mRelObject.mUniqueIndexIsDeferred[i] = STL_TRUE;
            }
            else
            {
                sExeUpdateStmt->mRelObject.mUniqueIndexIsDeferred[i] = STL_FALSE;
            }

            sExeUpdateStmt->mRelObject.mUniqueIndexCollCnt[i] = 0;
        }
    }

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief UPDATE 구문의 Constraint 들의 collision count 를 RESET
 * @param[in] aInitRelObject  Update Init Related Object
 * @param[in] aOptNode        Optimization Node
 * @param[in] aDataArea       Data Area (Data Optimization 결과물)
 * @param[in] aEnv            Environment
 * @remarks
 */
stlStatus qlxUpdateResetConstraintCollision( qlvInitUpdateRelObject * aInitRelObject,
                                             qllOptimizationNode    * aOptNode,
                                             qllDataArea            * aDataArea,
                                             qllEnv                 * aEnv )
{
    qllExecutionNode      * sExecNode = NULL;
    qlnxUpdateStmt        * sExeUpdateStmt = NULL;
    
    stlInt32 i = 0;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aInitRelObject != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * UPDATE STATEMENT Execution Node 획득
     */

    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );
    sExeUpdateStmt = sExecNode->mExecNode;
    
    /**
     * PRIMARY KEY constraint 의 collision reset
     */
    
    if ( aInitRelObject->mAffectPrimaryKeyCnt > 0 )
    {
        for ( i = 0; i < aInitRelObject->mAffectPrimaryKeyCnt; i++ )
        {
            sExeUpdateStmt->mRelObject.mPrimaryKeyCollCnt[i] = 0;
        }
    }

    /**
     * UNIQUE constraint 의 collision reset
     */
    
    if ( aInitRelObject->mAffectUniqueKeyCnt > 0 )
    {
        for ( i = 0; i < aInitRelObject->mAffectUniqueKeyCnt; i++ )
        {
            sExeUpdateStmt->mRelObject.mUniqueKeyCollCnt[i] = 0;
        }
    }

    /**
     * @todo FOREIGN constraint 의 collision reset
     */

    /**
     * @todo CHILD FOREIGN KEY constraint 의 collision reset
     */
    
    /**
     * NOT NULL constraint 의 collision reset
     */

    if ( aInitRelObject->mAffectCheckNotNullCnt > 0 )
    {
        for ( i = 0; i < aInitRelObject->mAffectCheckNotNullCnt; i++ )
        {
            sExeUpdateStmt->mRelObject.mNotNullCollCnt[i] = 0;
        }
    }
    
    /**
     * @todo CHECK constraint 의 collision reset
     */

    /**
     * UNIQUE index 의 collision reset
     */
    
    if ( aInitRelObject->mAffectUniqueIndexCnt > 0 )
    {
        for ( i = 0; i < aInitRelObject->mAffectUniqueIndexCnt; i++ )
        {
            sExeUpdateStmt->mRelObject.mUniqueIndexCollCnt[i] = 0;
        }
    }

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief UPDATE 수행을 위한 Lock 을 잡고 유효한 Plan 을 얻는다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aDBCStmt      DBC Statement
 * @param[in]  aSQLStmt      SQL Statement
 * @param[in]  aQueryPhase   Query Processing Phase
 * @param[out] aInitPlan     UPDATE Init Plan
 * @param[out] aCodePlan     UPDATE Code Plan    
 * @param[out] aDataPlan     UPDATE Data Plan
 * @param[in]  aEnv          Environment
 */
stlStatus qlxUpdateGetValidPlan( smlTransId              aTransID,
                                 qllDBCStmt            * aDBCStmt,
                                 qllStatement          * aSQLStmt,
                                 qllQueryPhase           aQueryPhase,
                                 qlvInitUpdate        ** aInitPlan,
                                 qllOptimizationNode  ** aCodePlan,
                                 qllDataArea          ** aDataPlan,
                                 qllEnv                * aEnv )
{
    qlvInitUpdate        * sInitPlan = NULL;
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
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_UPDATE_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_UPDATE_WHERE_CURRENT_OF_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_UPDATE_RETURNING_INTO_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_UPDATE_RETURNING_QUERY_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aQueryPhase > QLL_PHASE_VALIDATION) &&
                        (aQueryPhase <= QLL_PHASE_EXECUTE),
                        QLL_ERROR_STACK(aEnv) );


    /**
     * 기본 정보 설정
     */
    
    sInitPlan = (qlvInitUpdate *) qllGetInitPlan( aSQLStmt );
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
        
            sInitPlan = (qlvInitUpdate *) qllGetInitPlan( aSQLStmt );
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
 * @brief Block Read 한 Row 들 중 유효한 것들만 갱신한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aDBCStmt         DBC Statement
 * @param[in]  aSQLStmt         SQL Statement
 * @param[in]  aDataArea        Data Area
 * @param[in]  aExecNode        UPDATE Statement Execution Node
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus qlxUpdateValidBlockRow( smlTransId             aTransID,
                                  smlStatement         * aStmt,
                                  qllDBCStmt           * aDBCStmt,
                                  qllStatement         * aSQLStmt,
                                  qllDataArea          * aDataArea,
                                  qllExecutionNode     * aExecNode,
                                  qllEnv               * aEnv )
{
    qlvInitUpdate       * sInitPlan = NULL;
    qlnoUpdateStmt      * sOptUpdateStmt   = NULL;
    qlnxUpdateStmt      * sExeUpdateStmt   = NULL;

    dtlDataValue          sDataValue;
    dtlBooleanType        sBoolValue       = STL_FALSE;
    
    knlExprEvalInfo       sExprEvalInfo;
    smlFetchRecordInfo  * sFetchRecordInfo = NULL;
    
    stlInt32              sBlockReadCnt    = 0;
    stlBool               sVersionConflict = STL_FALSE;
    stlBool               sSkipped         = STL_FALSE;
    stlBool               sDeleted         = STL_FALSE;
    stlBool               sIsMatched       = STL_FALSE;
    
    knlValueBlockList   * sSetBlock        = NULL;
    stlInt32              sBlockIdx        = 0;
    stlInt32              sSetIdx          = 0;

    qlnxFetchNodeInfo     sFetchNodeInfo;
    
        
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExecNode != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * UPDATE STATEMENT Node 획득
     */

    sInitPlan      = (qlvInitUpdate *) qllGetInitPlan( aSQLStmt );
    sOptUpdateStmt = (qlnoUpdateStmt *) aExecNode->mOptNode->mOptNode;
    sExeUpdateStmt = (qlnxUpdateStmt *) aExecNode->mExecNode;

    
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

    sExprEvalInfo.mContext = aDataArea->mExprDataContext;

    /* fetch record 정보 설정 */
    sFetchRecordInfo = & sExeUpdateStmt->mFetchRecordInfo;
    
    sFetchRecordInfo->mRelationHandle  = sOptUpdateStmt->mPhysicalHandle;
    sFetchRecordInfo->mColList         = sExeUpdateStmt->mReadColBlock;
    sFetchRecordInfo->mRowIdColList    = sExeUpdateStmt->mRowIdColBlock;
    sFetchRecordInfo->mPhysicalFilter  = NULL;
    sFetchRecordInfo->mTransReadMode   = SML_TRM_COMMITTED;
    sFetchRecordInfo->mStmtReadMode    = SML_SRM_SNAPSHOT;
    sFetchRecordInfo->mScnBlock        = sExeUpdateStmt->mRowBlock->mScnBlock;
    sFetchRecordInfo->mLogicalFilter   = NULL;
    sFetchRecordInfo->mLogicalFilterEvalInfo = NULL;
    
    
    /**
     * update record
     */

    /* SM이 모든 컬럼을 다시 읽도록 block의 used count를 초기화한다. */
    sBlockReadCnt = sExeUpdateStmt->mRowBlock->mUsedBlockSize;
    KNL_SET_ALL_BLOCK_USED_CNT( sExeUpdateStmt->mReadColBlock, 0 );
    if( sExeUpdateStmt->mRowIdColBlock != NULL )
    {
        KNL_SET_ONE_BLOCK_USED_CNT( sExeUpdateStmt->mRowIdColBlock, 0 );
    }

    
    /**
     * ROWID column 수행
     */
    
    if( sOptUpdateStmt->mRowIdColumn != NULL )
    {
        STL_TRY( qlxExecuteRowIdColumnBlock(
                     aTransID,
                     aStmt,
                     sOptUpdateStmt->mTableHandle,
                     sExeUpdateStmt->mRowBlock,
                     sOptUpdateStmt->mRowIdColumn,
                     sExeUpdateStmt->mRowIdColBlock,
                     0,
                     sBlockReadCnt,
                     aEnv )
                 == STL_SUCCESS );

        KNL_SET_ONE_BLOCK_USED_CNT( sExeUpdateStmt->mRowIdColBlock,
                                    sBlockReadCnt );        
    }                            

    /**
     * 조건을 만족하는 Row 만 넘어오기 때문에 모든 Row를 갱신
     */

    QLNX_INIT_FETCH_NODE_INFO( & sFetchNodeInfo,
                               aTransID,
                               aStmt,
                               aDBCStmt,
                               aSQLStmt,
                               aDataArea );
    
    /* Write 하기 위한 하나의 레코드에 대한 used count를 설정한다. */
    KNL_SET_ALL_BLOCK_USED_CNT( sExeUpdateStmt->mWriteColBlock, sBlockReadCnt );

    for ( sBlockIdx = 0; sBlockIdx < sBlockReadCnt; sBlockIdx++ )
    {
        STL_RAMP( BEGIN_UPDATE );

        /***********************************************
         * SET 절을 수행
         ***********************************************/

        if( sOptUpdateStmt->mSetChildNode != NULL )
        {
            stlInt64              sTmpUsedBlockCnt;
            stlBool               sEOF;
            knlValueBlockList   * sOuterColBlock;
            knlValueBlockList   * sOuterOrgColBlock;
            knlValueBlockList   * sRefColBlock;
            qlnxSubQueryList    * sExeSubQueryList;
            stlInt32              sLoop;


            /* Outer Column Block Value 설정 */
            if( sExeUpdateStmt->mSetOuterColBlock != NULL )
            {
                STL_DASSERT( sExeUpdateStmt->mSetOuterOrgColBlock != NULL );

                sOuterColBlock    = sExeUpdateStmt->mSetOuterColBlock;
                sOuterOrgColBlock = sExeUpdateStmt->mSetOuterOrgColBlock;

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

            QLNX_EXECUTE_NODE( aTransID,
                               aStmt,
                               aDBCStmt,
                               aSQLStmt,
                               aDataArea->mExecNodeList[ sOptUpdateStmt->mSetChildNode->mIdx ].mOptNode,
                               aDataArea,
                               aEnv );

            sEOF = STL_FALSE;
            QLNX_FETCH_NODE( & sFetchNodeInfo,
                             sOptUpdateStmt->mSetChildNode->mIdx,
                             0, /* Start Idx */
                             0,
                             1,
                             & sTmpUsedBlockCnt,
                             & sEOF,
                             aEnv );

            sExeSubQueryList = QLL_GET_EXECUTION_NODE( aDataArea,
                                                       sOptUpdateStmt->mSetChildNode->mIdx )->mExecNode;

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


        /**
         * SET 절 수행
         */
        
        sSetBlock = sExeUpdateStmt->mWriteColBlock;
        
        sExprEvalInfo.mBlockIdx   = sBlockIdx;
        sExprEvalInfo.mBlockCount = 1;
        
        while( sSetBlock != NULL )
        {
            sSetIdx = sOptUpdateStmt->mSetCodeStackIdx[ KNL_GET_BLOCK_COLUMN_ORDER( sSetBlock ) ];

            sExprEvalInfo.mExprStack   = (knlExprStack *) & sOptUpdateStmt->mSetCodeStack[ sSetIdx ];
            sExprEvalInfo.mResultBlock = sSetBlock;
                    
            STL_TRY( knlEvaluateOneExpression( & sExprEvalInfo,
                                               KNL_ENV( aEnv ) )
                     == STL_SUCCESS );
            sSetBlock = sSetBlock->mNext;
        }

        /***********************************************
         * Row 갱신 
         ***********************************************/

        sVersionConflict = STL_FALSE;
        sSkipped = STL_FALSE;
        sDeleted = STL_FALSE;
        
        sExeUpdateStmt->mIsUpdated[ sBlockIdx ] = STL_FALSE;
        
        STL_TRY( smlUpdateRecord(
                     aStmt,
                     sOptUpdateStmt->mPhysicalHandle,
                     & sExeUpdateStmt->mRowBlock->mRidBlock[sBlockIdx],
                     sExeUpdateStmt->mRowBlock->mScnBlock[sBlockIdx],
                     sBlockIdx,
                     sExeUpdateStmt->mWriteColBlock,
                     sExeUpdateStmt->mRelObject.mSuppLogBeforeColBlock,
                     sExeUpdateStmt->mRelObject.mSuppLogPrimaryKeyBlock,
                     & sVersionConflict,
                     & sSkipped,
                     SML_ENV(aEnv) )
                 == STL_SUCCESS );
        
        /***********************************************
         * Version Conflict 에 대한 처리
         ***********************************************/
                    
        if( sVersionConflict != STL_TRUE )
        {
            if ( sSkipped == STL_TRUE )
            {
                /**
                 * Version Conflict 이전에 이미 삭제에 성공함
                 */

                sExeUpdateStmt->mIsUpdated[ sBlockIdx ] = STL_FALSE;
            }
            else
            {
                /**
                 * Version Conflict 없이 Row 갱신에 성공함
                 */
                        
                aDataArea->mWriteRowCnt++;
                sExeUpdateStmt->mIsUpdated[ sBlockIdx ] = STL_TRUE;


                /**
                 * UPDATE 문장 유형에 따른 updated row 처리
                 */
                
                switch ( aSQLStmt->mStmtType )
                {
                    case QLL_STMT_UPDATE_TYPE:
                    case QLL_STMT_UPDATE_WHERE_CURRENT_OF_TYPE:
                        {
                            /**
                             * nothing to do
                             */
                            break;
                        }
                    case QLL_STMT_UPDATE_RETURNING_QUERY_TYPE:
                        {
                            if( sOptUpdateStmt->mReturnChildNode != NULL )
                            {
                                knlValueBlockList   * sOuterColBlock;
                                knlValueBlockList   * sOuterOrgColBlock;
                                knlValueBlockList   * sRefColBlock;
                                qlnxSubQueryList    * sExeSubQueryList;
                                stlInt32              sLoop;
                                stlBool               sEOF;
                                stlInt64              sTmpUsedBlockCnt;

                                /* Outer Column Block Value 설정 */
                                if( sExeUpdateStmt->mReturnOuterColBlock != NULL )
                                {
                                    STL_DASSERT( sExeUpdateStmt->mReturnOuterOrgColBlock != NULL );

                                    sOuterColBlock    = sExeUpdateStmt->mReturnOuterColBlock;
                                    sOuterOrgColBlock = sExeUpdateStmt->mReturnOuterOrgColBlock;

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
                                                   aDataArea->mExecNodeList[ sOptUpdateStmt->mReturnChildNode->mIdx ].mOptNode,
                                                   aDataArea,
                                                   aEnv );

                                sEOF = STL_FALSE;
                                QLNX_FETCH_NODE( & sFetchNodeInfo,
                                                 sOptUpdateStmt->mReturnChildNode->mIdx,
                                                 0, /* Start Idx */
                                                 0,
                                                 1,
                                                 & sTmpUsedBlockCnt,
                                                 & sEOF,
                                                 aEnv );

                                sExeSubQueryList = QLL_GET_EXECUTION_NODE( aDataArea,
                                                                           sOptUpdateStmt->mReturnChildNode->mIdx )->mExecNode;

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
                                                                 sOptUpdateStmt->mReturnExprCnt,
                                                                 sOptUpdateStmt->mReturnExprStack,
                                                                 aEnv )
                                     == STL_SUCCESS );

                            STL_TRY( qlnxAddMeetBlockIntoInstantTable(
                                         aSQLStmt->mResultSetDesc->mInstantTable,
                                         aSQLStmt->mResultSetDesc->mInstantFetchInfo.mRowBlock,
                                         aDataArea->mTargetBlock,
                                         sExeUpdateStmt->mIsUpdated,
                                         sBlockIdx,
                                         sBlockIdx+1,
                                         aEnv )
                                     == STL_SUCCESS );
                            
                            break;
                        }
                    case QLL_STMT_UPDATE_RETURNING_INTO_TYPE:
                        {
                            /* Outer Column Block Value 설정 */
                            if( sExeUpdateStmt->mReturnOuterColBlock != NULL )
                            {
                                knlValueBlockList   * sOuterColBlock    = NULL;
                                knlValueBlockList   * sOuterOrgColBlock = NULL;

                                STL_DASSERT( sExeUpdateStmt->mReturnOuterOrgColBlock != NULL );

                                sOuterColBlock    = sExeUpdateStmt->mReturnOuterColBlock;
                                sOuterOrgColBlock = sExeUpdateStmt->mReturnOuterOrgColBlock;

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
                            if( sOptUpdateStmt->mReturnChildNode != NULL )
                            {
                                stlBool       sEOF;
                                stlInt64      sTmpUsedBlockCnt;

                                QLNX_EXECUTE_NODE( aTransID,
                                                   aStmt,
                                                   aDBCStmt,
                                                   aSQLStmt,
                                                   aDataArea->mExecNodeList[ sOptUpdateStmt->mReturnChildNode->mIdx ].mOptNode,
                                                   aDataArea,
                                                   aEnv );

                                sEOF = STL_FALSE;
                                QLNX_FETCH_NODE( & sFetchNodeInfo,
                                                 sOptUpdateStmt->mReturnChildNode->mIdx,
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
                                                                 sOptUpdateStmt->mReturnExprCnt,
                                                                 sOptUpdateStmt->mReturnExprStack,
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
                         * 최신 레코드를 다시 읽음
                         */
                        
                        STL_TRY( smlFetchRecord(
                                     aStmt,
                                     sFetchRecordInfo,
                                     & sExeUpdateStmt->mRowBlock->mRidBlock[sBlockIdx],
                                     sBlockIdx,
                                     & sIsMatched,
                                     & sDeleted,
                                     NULL, /* aUpdated */
                                     SML_ENV(aEnv) )
                                 == STL_SUCCESS );

                        /**
                         * Data 존재 여부 설정
                         */
                        
                        if( ( sDeleted == STL_FALSE ) && ( sIsMatched == STL_TRUE ) )
                        {
                            /**
                             * 삭제되지 않은 정상 Row 임. UPDATE 를 다시 시도
                             */
                            
                            STL_THROW( BEGIN_UPDATE );
                        }
                        else
                        {
                            /**
                             * 다른 Tx 에 의해 Data 가 삭제되어 COMMIT 됨
                             * 갱신하지 않아야 할 Row 임.
                             */
                            
                            sExeUpdateStmt->mIsUpdated[sBlockIdx] = STL_FALSE;
                            break;
                        }
                        break;
                    }
                case QLV_SERIAL_RETRY_ONLY:
                case QLV_SERIAL_ROLLBACK_RETRY:
                    {
                        /**
                         * RETRY 를 수행을 해야 함.
                         * - RETRY 를 수행하기 전에 이미 갱신한 Row 는 인덱스 정보도 갱신해주어야 함.
                         * - 아직 갱신하지 않은 Row 는 인덱스 정보를 갱신하지 않도록 함.
                         */

                        
                        for( ; sBlockIdx < sBlockReadCnt; sBlockIdx++ )
                        {
                            sExeUpdateStmt->mIsUpdated[ sBlockIdx ] = STL_FALSE;
                        }

                        /**
                         * Query 를 재수행이 필요함을 설정한다.
                         */

                        sExeUpdateStmt->mVersionConflict = STL_TRUE;
                        STL_THROW( FINISH_UPDATE_VALID_BLOCK_ROW );
                            
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

    STL_RAMP( FINISH_UPDATE_VALID_BLOCK_ROW );
    
    /* SM이 모든 컬럼을 다시 읽었기 때문에 block의 used count를 원복한다. */
    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sExeUpdateStmt->mReadColBlock, 0, sBlockReadCnt );
    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sExeUpdateStmt->mWriteColBlock, 0, sBlockReadCnt );
    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sExeUpdateStmt->mRowIdColBlock, 0, sBlockReadCnt );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief UPDATE 의 Check Constraint 무결성을 검증한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aSQLStmt         SQL Statement
 * @param[in]  aDataArea        Data Area
 * @param[in]  aExecNode        UPDATE Statement Execution Node
 * @param[in]  aEnv             Environment
 */
stlStatus qlxUpdateIntegrityCheckConst( smlTransId          aTransID,
                                        qllStatement      * aSQLStmt,
                                        qllDataArea       * aDataArea,
                                        qllExecutionNode  * aExecNode,
                                        qllEnv            * aEnv )
{
    qlnoUpdateStmt          * sOptUpdateStmt = NULL;
    qlnxUpdateStmt          * sExeUpdateStmt = NULL;

    qlvInitUpdateRelObject  * sIniRelObject = NULL;
    qlxExecUpdateRelObject  * sExeRelObject = NULL;

    stlInt32 i = 0;
    stlInt32 sCollCnt = 0;
    stlInt32 sViolPos = 0;
    
    stlBool                   sObjectExist = STL_FALSE;
    
    ellDictHandle             sViolatedSchemaHandle;
    ellDictHandle           * sViolatedColumnHandle = NULL;

    ellInitDictHandle( & sViolatedSchemaHandle );


    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExecNode != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * UPDATE STATEMENT Node 획득
     */

    sOptUpdateStmt = (qlnoUpdateStmt *) aExecNode->mOptNode->mOptNode;
    sExeUpdateStmt = (qlnxUpdateStmt *) aExecNode->mExecNode;


    /**
     * Update Relation Object 정보 획득
     */
    
    sIniRelObject = sOptUpdateStmt->mRelObject;
    sExeRelObject = & sExeUpdateStmt->mRelObject;

    
    /**************************************************
     * Identity Column 의 not null 검사
     **************************************************/

    if ( sIniRelObject->mHasAffectIdentity == STL_TRUE )
    {
        qlrcCheckNotNull4Update( sExeUpdateStmt->mIsUpdated,
                                 NULL, /* aOldValueBlockList */
                                 sExeRelObject->mIdentityColumnBlock,
                                 & sCollCnt,
                                 & sViolPos );

        if( sCollCnt == 0 )
        {
            /* collision 이 없음 */
        }
        else
        {
            STL_TRY( ellGetSchemaDictHandleByID(
                         aTransID,
                         aSQLStmt->mViewSCN,
                         ellGetTableSchemaID( sOptUpdateStmt->mTableHandle ),
                         & sViolatedSchemaHandle,
                         & sObjectExist,
                         ELL_ENV(aEnv) )
                     == STL_SUCCESS );
            STL_DASSERT( sObjectExist == STL_TRUE );
            
            sViolatedColumnHandle = & sIniRelObject->mIdentityColumnHandle;
            
            STL_THROW( RAMP_ERR_NOT_NULL_INTEGRITY );
        }
    }
    else
    {
        /* nothing to do */
    }

    /**************************************************
     * NOT NULL constraint 검사 
     **************************************************/

    for ( i = 0; i < sIniRelObject->mAffectCheckNotNullCnt; i++ )
    {
        sCollCnt = 0;

        qlrcCheckNotNull4Update( sExeUpdateStmt->mIsUpdated,
                                 sExeRelObject->mOldNotNullColumnBlock[i],
                                 sExeRelObject->mNewNotNullColumnBlock[i],
                                 & sCollCnt,
                                 & sViolPos );

        if ( sCollCnt == 0 )
        {
            /* collision 이 없음 */
        }
        else
        {
            if ( sExeRelObject->mNotNullIsDeferred[i] == STL_TRUE )
            {
                /**
                 * DEFERRED 인 경우
                 */

                sExeRelObject->mNotNullCollCnt[i] = sExeRelObject->mNotNullCollCnt[i] + sCollCnt;
            }
            else
            {
                /**
                 * IMMEDIATE 인 경우
                 */

                STL_TRY( ellGetSchemaDictHandleByID(
                             aTransID,
                             aSQLStmt->mViewSCN,
                             ellGetTableSchemaID( sOptUpdateStmt->mTableHandle ),
                             & sViolatedSchemaHandle,
                             & sObjectExist,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                STL_DASSERT( sObjectExist == STL_TRUE );

                sViolatedColumnHandle =
                    ellGetCheckNotNullColumnHandle( & sIniRelObject->mAffectCheckNotNullHandle[i] );
                
                STL_THROW( RAMP_ERR_NOT_NULL_INTEGRITY );
            }
        }
    }
    

    /**************************************************
     * CHECK constraint 검사 
     **************************************************/

    /**
     * @todo Check Clause 를 검사해야 함.
     */
    
    STL_TRY_THROW( sIniRelObject->mAffectCheckClauseCnt == 0, RAMP_ERR_NOT_IMPLEMENTED );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_NULL_INTEGRITY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CANNOT_UPDATE_NULL,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      ellGetSchemaName( & sViolatedSchemaHandle ),
                      ellGetTableName( sOptUpdateStmt->mTableHandle ),
                      ellGetColumnName( sViolatedColumnHandle ) );
    }

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "qlxUpdateIntegrityCheckConst()" );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief UPDATE 의 Key Constraint 무결성을 검증하고 Index Key 를 갱신한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aDataArea        Data Area
 * @param[in]  aExecNode        UPDATE Statement Execution Node
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus qlxUpdateIntegrityKeyConstIndexKey( smlTransId          aTransID,
                                              smlStatement      * aStmt,
                                              qllDataArea       * aDataArea,
                                              qllExecutionNode  * aExecNode,
                                              qllEnv            * aEnv )
{
    qlnoUpdateStmt          * sOptUpdateStmt = NULL;
    qlnxUpdateStmt          * sExeUpdateStmt = NULL;

    qlvInitUpdateRelObject  * sInitRelObject = NULL;
    qlxExecUpdateRelObject  * sExecRelObject = NULL;

    stlInt32 sViolatedCnt = 0;
    stlInt32 sKeyCollCnt = 0;
    stlInt32 sNullCollCnt = 0;
    stlInt32 sViolPos = 0;
    
    smlUniqueViolationScope sUniqueViolationScope;

    stlBool         sObjectExist = STL_FALSE;

    ellDictHandle  * sViolatedHandle = NULL;
    ellDictHandle    sViolatedSchemaHandle;
    ellDictHandle  * sKeyColumnHandle = NULL;
    ellDictHandle  * sViolatedColumnHandle = NULL;
    
    stlInt32 i = 0;

    ellInitDictHandle( & sViolatedSchemaHandle );
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExecNode != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * UPDATE STATEMENT Node 획득
     */

    sOptUpdateStmt = (qlnoUpdateStmt *) aExecNode->mOptNode->mOptNode;
    sExeUpdateStmt = (qlnxUpdateStmt *) aExecNode->mExecNode;
    sInitRelObject = sOptUpdateStmt->mRelObject;
    sExecRelObject = & sExeUpdateStmt->mRelObject;

    
    /*******************************************************************
     * PRIMARY KEY Constraint 검사
     *******************************************************************/
    
    for ( i = 0; i < sInitRelObject->mAffectPrimaryKeyCnt; i++ )
    {
        STL_DASSERT( SML_USED_BLOCK_SIZE( sExeUpdateStmt->mRowBlock )
                     == KNL_GET_BLOCK_USED_CNT( sExecRelObject->mPrimaryKeyDeleteKeyBlock[i] ) );
        STL_DASSERT( SML_USED_BLOCK_SIZE( sExeUpdateStmt->mRowBlock )
                     == KNL_GET_BLOCK_USED_CNT( sExecRelObject->mPrimaryKeyInsertKeyBlock[i] ) );

        STL_DASSERT( KNL_GET_BLOCK_SKIP_CNT( sExecRelObject->mPrimaryKeyDeleteKeyBlock[i] )
                     == KNL_GET_BLOCK_SKIP_CNT( sExecRelObject->mPrimaryKeyInsertKeyBlock[i] ) );
        STL_DASSERT( KNL_GET_BLOCK_USED_CNT( sExecRelObject->mPrimaryKeyDeleteKeyBlock[i] )
                     == KNL_GET_BLOCK_USED_CNT( sExecRelObject->mPrimaryKeyInsertKeyBlock[i] ) );

        /**************************************************
         * PRIMARY KEY 의 NOT NULL 제약 
         **************************************************/

        sNullCollCnt = 0;
        
        qlrcCheckNotNull4Update( sExeUpdateStmt->mIsUpdated,
                                 sExecRelObject->mPrimaryKeyDeleteKeyBlock[i],
                                 sExecRelObject->mPrimaryKeyInsertKeyBlock[i],
                                 & sNullCollCnt,
                                 & sViolPos );

        if ( sNullCollCnt == 0 )
        {
            /* collision 이 없음 */
        }
        else
        {
            if ( sExecRelObject->mPrimaryKeyIsDeferred[i] == STL_TRUE )
            {
                /**
                 * DEFERRED 인 경우
                 */
            }
            else
            {
                /**
                 * IMMEDIATE 인 경우
                 */

                STL_TRY( ellGetSchemaDictHandleByID(
                             aTransID,
                             aStmt->mScn,
                             ellGetTableSchemaID( sOptUpdateStmt->mTableHandle ),
                             & sViolatedSchemaHandle,
                             & sObjectExist,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                STL_DASSERT( sObjectExist == STL_TRUE );

                sKeyColumnHandle = ellGetPrimaryKeyColumnHandle( & sInitRelObject->mAffectPrimaryKeyHandle[i] );
                sViolatedColumnHandle = & sKeyColumnHandle[sViolPos];
                
                STL_THROW( RAMP_ERR_NOT_NULL_INTEGRITY );
            }
        }
        
        /*************************************************
         * 각 Row 의 인덱스 갱신 
         *************************************************/

        sKeyCollCnt = 0;
        
        /**
         * Index Key 삭제 
         */
            
        STL_TRY( smlDeleteKeys( aStmt,
                                sInitRelObject->mAffectPrimaryKeyIndexPhyHandle[i],
                                sExecRelObject->mPrimaryKeyDeleteKeyBlock[i],
                                sExeUpdateStmt->mRowBlock,
                                sExeUpdateStmt->mIsUpdated,
                                & sKeyCollCnt,
                                & sUniqueViolationScope,
                                SML_ENV(aEnv) )
                 == STL_SUCCESS );

        /**
         * Index Key 추가
         */
                    
        STL_TRY( smlInsertKeys( aStmt,
                                sInitRelObject->mAffectPrimaryKeyIndexPhyHandle[i],
                                sExecRelObject->mPrimaryKeyInsertKeyBlock[i],
                                sExeUpdateStmt->mRowBlock,
                                sExeUpdateStmt->mIsUpdated,
                                STL_TRUE,  /* aIsDeferred */
                                & sKeyCollCnt,
                                & sUniqueViolationScope,
                                SML_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sUniqueViolationScope != SML_UNIQUE_VIOLATION_SCOPE_NONE )
        {
            if ( sUniqueViolationScope == SML_UNIQUE_VIOLATION_SCOPE_STMT )
            {
                /**
                 * Statement Scope 의 Uniqueness 를 위반한 경우
                 */
                                
                if ( sExecRelObject->mPrimaryKeyIsDeferred[i] == STL_TRUE )
                {
                    /**
                     * Deferred 로 Key Constraint를 처리하는 경우,
                     * 이후의 Statement 에 의해 Collision 이 해결될 수 있다.
                     */
                }
                else
                {
                    sViolatedHandle = & sInitRelObject->mAffectPrimaryKeyHandle[i];
                                
                    STL_TRY( ellGetSchemaDictHandleByID(
                                 aTransID,
                                 aStmt->mScn,
                                 ellGetConstraintSchemaID(sViolatedHandle),
                                 & sViolatedSchemaHandle,
                                 & sObjectExist,
                                 ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                    STL_DASSERT( sObjectExist == STL_TRUE );
                                
                    STL_THROW( RAMP_ERR_UNIQUE_INTEGRITY_VIOLATED );
                }
            }
            else
            {
                /**
                 * Row Scope 의 Uniqueness 를 위반한 경우
                 * 아직, Uniqueness 위배 여부를 판단할 수 없다.
                 */
            }
        }
            

        /*************************************************
         * Collision 정보 관리  
         *************************************************/

        sViolatedCnt = sKeyCollCnt + sNullCollCnt;
        if ( sViolatedCnt == 0 )
        {
            /* Collision 변경 없음 */
        }
        else
        {
            sExecRelObject->mPrimaryKeyCollCnt[i] += sViolatedCnt;
        }
    }

    /*******************************************************************
     * UNIQUE Constraint 검사
     *******************************************************************/
    
    for ( i = 0; i < sInitRelObject->mAffectUniqueKeyCnt; i++ )
    {
        /**
         * 갱신에 영향을 받는 인덱스임 
         */

        STL_DASSERT( SML_USED_BLOCK_SIZE( sExeUpdateStmt->mRowBlock )
                     == KNL_GET_BLOCK_USED_CNT( sExecRelObject->mUniqueKeyDeleteKeyBlock[i] ) );
        STL_DASSERT( SML_USED_BLOCK_SIZE( sExeUpdateStmt->mRowBlock )
                     == KNL_GET_BLOCK_USED_CNT( sExecRelObject->mUniqueKeyInsertKeyBlock[i] ) );

        STL_DASSERT( KNL_GET_BLOCK_SKIP_CNT( sExecRelObject->mUniqueKeyDeleteKeyBlock[i] )
                     == KNL_GET_BLOCK_SKIP_CNT( sExecRelObject->mUniqueKeyInsertKeyBlock[i] ) );
        STL_DASSERT( KNL_GET_BLOCK_USED_CNT( sExecRelObject->mUniqueKeyDeleteKeyBlock[i] )
                     == KNL_GET_BLOCK_USED_CNT( sExecRelObject->mUniqueKeyInsertKeyBlock[i] ) );

        /*************************************************
         * 각 Row 의 인덱스 갱신 
         *************************************************/

        sKeyCollCnt = 0;
        
        /**
         * Index Key 삭제 
         */
            
        STL_TRY( smlDeleteKeys( aStmt,
                                sInitRelObject->mAffectUniqueKeyIndexPhyHandle[i],
                                sExecRelObject->mUniqueKeyDeleteKeyBlock[i],
                                sExeUpdateStmt->mRowBlock,
                                sExeUpdateStmt->mIsUpdated,
                                & sKeyCollCnt,
                                & sUniqueViolationScope,
                                SML_ENV(aEnv) )
                 == STL_SUCCESS );

        /**
         * Index Key 추가
         */
                    
        STL_TRY( smlInsertKeys( aStmt,
                                sInitRelObject->mAffectUniqueKeyIndexPhyHandle[i],
                                sExecRelObject->mUniqueKeyInsertKeyBlock[i],
                                sExeUpdateStmt->mRowBlock,
                                sExeUpdateStmt->mIsUpdated,
                                STL_TRUE,  /* aIsDeferred */
                                & sKeyCollCnt,
                                & sUniqueViolationScope,
                                SML_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sUniqueViolationScope != SML_UNIQUE_VIOLATION_SCOPE_NONE )
        {
            if ( sUniqueViolationScope == SML_UNIQUE_VIOLATION_SCOPE_STMT )
            {
                /**
                 * Statement Scope 의 Uniqueness 를 위반한 경우
                 */
                                
                if ( sExecRelObject->mUniqueKeyIsDeferred[i] == STL_TRUE )
                {
                    /**
                     * Deferred 로 Key Constraint를 처리하는 경우,
                     * 이후의 Statement 에 의해 Collision 이 해결될 수 있다.
                     */
                }
                else
                {
                    sViolatedHandle = & sInitRelObject->mAffectUniqueKeyHandle[i];
                                
                    STL_TRY( ellGetSchemaDictHandleByID(
                                 aTransID,
                                 aStmt->mScn,
                                 ellGetConstraintSchemaID(sViolatedHandle),
                                 & sViolatedSchemaHandle,
                                 & sObjectExist,
                                 ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                    STL_DASSERT( sObjectExist == STL_TRUE );
                                
                    STL_THROW( RAMP_ERR_UNIQUE_INTEGRITY_VIOLATED );
                }
            }
            else
            {
                /**
                 * Row Scope 의 Uniqueness 를 위반한 경우
                 * 아직, Uniqueness 위배 여부를 판단할 수 없다.
                 */
            }
        }

        /*************************************************
         * Collision 정보 관리  
         *************************************************/
        
        if ( sKeyCollCnt == 0 )
        {
            /* Collision 변경 없음 */
        }
        else
        {
            sExecRelObject->mUniqueKeyCollCnt[i] += sKeyCollCnt;
        }
    }

    /*******************************************************************
     * FOREIGN KEY constraint 검사
     *******************************************************************/
    
    STL_TRY_THROW( sInitRelObject->mAffectForeignKeyCnt == 0, RAMP_ERR_NOT_IMPLEMENTED );

    /*******************************************************************
     * CHILD FOREIGN KEY constraint 검사
     *******************************************************************/

    STL_TRY_THROW( sInitRelObject->mAffectChildForeignKeyCnt == 0, RAMP_ERR_NOT_IMPLEMENTED );
    
    /*******************************************************************
     * Unique Index
     *******************************************************************/
    
    for ( i = 0; i < sInitRelObject->mAffectUniqueIndexCnt; i++ )
    {
        /**
         * 갱신에 영향을 받는 인덱스임 
         */

        STL_DASSERT( SML_USED_BLOCK_SIZE( sExeUpdateStmt->mRowBlock )
                     == KNL_GET_BLOCK_USED_CNT( sExecRelObject->mUniqueIndexDeleteKeyBlock[i] ) );
        STL_DASSERT( SML_USED_BLOCK_SIZE( sExeUpdateStmt->mRowBlock )
                     == KNL_GET_BLOCK_USED_CNT( sExecRelObject->mUniqueIndexInsertKeyBlock[i] ) );

        STL_DASSERT( KNL_GET_BLOCK_SKIP_CNT( sExecRelObject->mUniqueIndexDeleteKeyBlock[i] )
                     == KNL_GET_BLOCK_SKIP_CNT( sExecRelObject->mUniqueIndexInsertKeyBlock[i] ) );
        STL_DASSERT( KNL_GET_BLOCK_USED_CNT( sExecRelObject->mUniqueIndexDeleteKeyBlock[i] )
                     == KNL_GET_BLOCK_USED_CNT( sExecRelObject->mUniqueIndexInsertKeyBlock[i] ) );
            
        /*************************************************
         * 각 Row 의 인덱스 갱신 
         *************************************************/

        sKeyCollCnt = 0;
        
        /**
         * Index Key 삭제 
         */
            
        STL_TRY( smlDeleteKeys( aStmt,
                                sInitRelObject->mAffectUniqueIndexPhyHandle[i],
                                sExecRelObject->mUniqueIndexDeleteKeyBlock[i],
                                sExeUpdateStmt->mRowBlock,
                                sExeUpdateStmt->mIsUpdated,
                                & sKeyCollCnt,
                                & sUniqueViolationScope,
                                SML_ENV(aEnv) )
                 == STL_SUCCESS );

        /**
         * Index Key 추가
         */
                    
        STL_TRY( smlInsertKeys( aStmt,
                                sInitRelObject->mAffectUniqueIndexPhyHandle[i],
                                sExecRelObject->mUniqueIndexInsertKeyBlock[i],
                                sExeUpdateStmt->mRowBlock,
                                sExeUpdateStmt->mIsUpdated,
                                STL_TRUE,  /* aIsDeferred */
                                & sKeyCollCnt,
                                & sUniqueViolationScope,
                                SML_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sUniqueViolationScope != SML_UNIQUE_VIOLATION_SCOPE_NONE )
        {
            if ( sUniqueViolationScope == SML_UNIQUE_VIOLATION_SCOPE_STMT )
            {
                /**
                 * Statement Scope 의 Uniqueness 를 위반한 경우
                 */
                                
                if ( sExecRelObject->mUniqueIndexIsDeferred[i] == STL_TRUE )
                {
                    /**
                     * Deferred 로 Unique Index 를 처리하는 경우,
                     * 이후의 Statement 에 의해 Collision 이 해결될 수 있다.
                     */
                }
                else
                {
                    sViolatedHandle = & sInitRelObject->mAffectUniqueIndexHandle[i];
                                
                    STL_TRY( ellGetSchemaDictHandleByID(
                                 aTransID,
                                 aStmt->mScn,
                                 ellGetIndexSchemaID(sViolatedHandle),
                                 & sViolatedSchemaHandle,
                                 & sObjectExist,
                                 ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                    STL_DASSERT( sObjectExist == STL_TRUE );
                                
                    STL_THROW( RAMP_ERR_UNIQUE_INDEX_INTEGRITY_VIOLATED );
                }
            }
            else
            {
                /**
                 * Row Scope 의 Uniqueness 를 위반한 경우
                 * 아직, Uniqueness 위배 여부를 판단할 수 없다.
                 */
            }
        }

        /*************************************************
         * Collision 정보 관리  
         *************************************************/
        
        if ( sKeyCollCnt == 0 )
        {
            /* Collision 변경 없음 */
        }
        else
        {
            sExecRelObject->mUniqueIndexCollCnt[i] += sKeyCollCnt;
        }
    }
    
    /*******************************************************************
     * Non-Unique Index
     *******************************************************************/
    
    for ( i = 0; i < sInitRelObject->mAffectNonUniqueIndexCnt; i++ )
    {
        /**
         * 갱신에 영향을 받는 인덱스임 
         */

        STL_DASSERT( SML_USED_BLOCK_SIZE( sExeUpdateStmt->mRowBlock )
                     == KNL_GET_BLOCK_USED_CNT( sExecRelObject->mNonUniqueIndexDeleteKeyBlock[i] ) );
        STL_DASSERT( SML_USED_BLOCK_SIZE( sExeUpdateStmt->mRowBlock )
                     == KNL_GET_BLOCK_USED_CNT( sExecRelObject->mNonUniqueIndexInsertKeyBlock[i] ) );

        STL_DASSERT( KNL_GET_BLOCK_SKIP_CNT( sExecRelObject->mNonUniqueIndexDeleteKeyBlock[i] )
                     == KNL_GET_BLOCK_SKIP_CNT( sExecRelObject->mNonUniqueIndexInsertKeyBlock[i] ) );
        STL_DASSERT( KNL_GET_BLOCK_USED_CNT( sExecRelObject->mNonUniqueIndexDeleteKeyBlock[i] )
                     == KNL_GET_BLOCK_USED_CNT( sExecRelObject->mNonUniqueIndexInsertKeyBlock[i] ) );

        /*************************************************
         * 각 Row 의 인덱스 갱신 
         *************************************************/

        sKeyCollCnt = 0;
        
        /**
         * Index Key 삭제 
         */
            
        STL_TRY( smlDeleteKeys( aStmt,
                                sInitRelObject->mAffectNonUniqueIndexPhyHandle[i],
                                sExecRelObject->mNonUniqueIndexDeleteKeyBlock[i],
                                sExeUpdateStmt->mRowBlock,
                                sExeUpdateStmt->mIsUpdated,
                                & sKeyCollCnt,
                                & sUniqueViolationScope,
                                SML_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_PARAM_VALIDATE( sUniqueViolationScope != SML_UNIQUE_VIOLATION_SCOPE_RESOLVED,
                            QLL_ERROR_STACK(aEnv) );
                    
        /**
         * Index Key 추가
         */
                    
        STL_TRY( smlInsertKeys( aStmt,
                                sInitRelObject->mAffectNonUniqueIndexPhyHandle[i],
                                sExecRelObject->mNonUniqueIndexInsertKeyBlock[i],
                                sExeUpdateStmt->mRowBlock,
                                sExeUpdateStmt->mIsUpdated,
                                STL_TRUE,  /* aIsDeferred */
                                & sKeyCollCnt,
                                & sUniqueViolationScope,
                                SML_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_PARAM_VALIDATE( sUniqueViolationScope == SML_UNIQUE_VIOLATION_SCOPE_NONE,
                            QLL_ERROR_STACK(aEnv) );
    }
    
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_NULL_INTEGRITY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CANNOT_UPDATE_NULL,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      ellGetSchemaName( & sViolatedSchemaHandle ),
                      ellGetTableName( sOptUpdateStmt->mTableHandle ),
                      ellGetColumnName( sViolatedColumnHandle ) );
    }
    
    STL_CATCH( RAMP_ERR_UNIQUE_INTEGRITY_VIOLATED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_UNIQUE_CONSTRAINT_VIOLATED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      ellGetSchemaName( & sViolatedSchemaHandle ),
                      ellGetConstraintName( sViolatedHandle ) );
    }

    STL_CATCH( RAMP_ERR_UNIQUE_INDEX_INTEGRITY_VIOLATED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_UNIQUE_CONSTRAINT_VIOLATED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      ellGetSchemaName( & sViolatedSchemaHandle ),
                      ellGetIndexName( sViolatedHandle ) );
    }
    
    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "qlxUpdateIntegrityKeyConstIndexKey()" );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}




/**
 * @brief Statement(Immediate) Constraint 를 위배하였는지 검사하고 위배시 에러를 리턴한다.
 * @param[in] aTransID          Transaction ID
 * @param[in] aStmt             SM statement
 * @param[in] aSQLStmt          SQL Statement
 * @param[in] aDataArea         Data Area
 * @param[in] aExecNode         UPDATE Statement Execution Node
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus qlxUpdateCheckStmtCollision( smlTransId          aTransID,
                                       smlStatement      * aStmt,
                                       qllStatement      * aSQLStmt,
                                       qllDataArea       * aDataArea,
                                       qllExecutionNode  * aExecNode,
                                       qllEnv            * aEnv )
{
    qlnoUpdateStmt          * sOptUpdateStmt = NULL;
    qlnxUpdateStmt          * sExeUpdateStmt = NULL;
    qlvInitUpdateRelObject  * sInitRelObject = NULL;
    qlxExecUpdateRelObject  * sExecRelObject = NULL;

    ellDictHandle           * sViolatedHandle = NULL;
    ellDictHandle             sSchemaHandle;

    stlBool                   sObjectExist = STL_FALSE;
    
    stlInt32                  i = 0;

    ellInitDictHandle( & sSchemaHandle );
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExecNode != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * UPDATE STATEMENT Node 획득
     */

    sOptUpdateStmt = (qlnoUpdateStmt *) aExecNode->mOptNode->mOptNode;
    sExeUpdateStmt = (qlnxUpdateStmt *) aExecNode->mExecNode;
    sInitRelObject = sOptUpdateStmt->mRelObject;
    sExecRelObject = & sExeUpdateStmt->mRelObject;

    /***********************************************
     * PRIMARY KEY constraint
     ***********************************************/
    
    /**
     * PRIMARY KEY 의 Immediate Constraint 위반 Check 
     */

    sInitRelObject = sOptUpdateStmt->mRelObject;
    
    for( i = 0; i < sInitRelObject->mAffectPrimaryKeyCnt; i++ )
    {
        if ( sExecRelObject->mPrimaryKeyCollCnt[i] != 0 )
        {
            if( sExecRelObject->mPrimaryKeyIsDeferred[i] == STL_TRUE )
            {
                /**
                 * DEFERRED 인 경우
                 */

                STL_TRY_THROW( sExecRelObject->mIsSuppLog == STL_FALSE, RAMP_ERR_CANNOT_SUPPLEMENTAL_LOG_DATA );
                
                /**
                 * Deferred Constarint 정보에 등록
                 */
                
                STL_TRY( qlrcSetDeferredCollision( QLL_COLL_ITEM_CONSTRAINT,
                                                   & sInitRelObject->mAffectPrimaryKeyHandle[i],
                                                   sExecRelObject->mPrimaryKeyCollCnt[i],
                                                   aEnv )
                         == STL_SUCCESS );
                
            }
            else
            {
                /**
                 * IMMEDIATE 인 경우
                 */
                
                sViolatedHandle = & sInitRelObject->mAffectPrimaryKeyHandle[i];
                
                STL_TRY( ellGetSchemaDictHandleByID( aTransID,
                                                     aSQLStmt->mViewSCN,
                                                     ellGetConstraintSchemaID(sViolatedHandle),
                                                     & sSchemaHandle,
                                                     & sObjectExist,
                                                     ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                STL_DASSERT( sObjectExist == STL_TRUE );
            
                STL_THROW( RAMP_ERR_UNIQUE_INTEGRITY_VIOLATED );
            }
        }
    }

    /***********************************************
     * UNIQUE constraint
     ***********************************************/
    
    /**
     * UNIQUE constraint 의 Immediate Constraint 위반 Check 
     */
    
    for( i = 0; i < sInitRelObject->mAffectUniqueKeyCnt; i++ )
    {
        if ( sExecRelObject->mUniqueKeyCollCnt[i] != 0 )
        {
            if( sExecRelObject->mUniqueKeyIsDeferred[i] == STL_TRUE )
            {
                /**
                 * DEFERRED 인 경우
                 */

                /**
                 * Deferred Constarint 정보에 등록
                 */
                
                STL_TRY( qlrcSetDeferredCollision( QLL_COLL_ITEM_CONSTRAINT,
                                                   & sInitRelObject->mAffectUniqueKeyHandle[i],
                                                   sExecRelObject->mUniqueKeyCollCnt[i],
                                                   aEnv )
                         == STL_SUCCESS );
                
            }
            else
            {
                /**
                 * IMMEDIATE 인 경우
                 */
                
                sViolatedHandle = & sInitRelObject->mAffectUniqueKeyHandle[i];
                
                STL_TRY( ellGetSchemaDictHandleByID( aTransID,
                                                     aSQLStmt->mViewSCN,
                                                     ellGetConstraintSchemaID(sViolatedHandle),
                                                     & sSchemaHandle,
                                                     & sObjectExist,
                                                     ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                STL_DASSERT( sObjectExist == STL_TRUE );

                STL_THROW( RAMP_ERR_UNIQUE_INTEGRITY_VIOLATED );
            }
        }
    }

    /***********************************************
     * FOREIGN KEY constraint
     ***********************************************/
    
    /**
     * @todo FOREIGN KEY 에 대한 고려가 필요함.
     */

    STL_TRY_THROW( sInitRelObject->mAffectForeignKeyCnt == 0, RAMP_ERR_NOT_IMPLEMENTED );

    /***********************************************
     * CHILD FOREIGN KEY constraint
     ***********************************************/
    
    /**
     * @todo CHILD FOREIGN KEY 에 대한 고려가 필요함.
     */

    STL_TRY_THROW( sInitRelObject->mAffectChildForeignKeyCnt == 0, RAMP_ERR_NOT_IMPLEMENTED );

    /***********************************************
     * NOT NULL constraint
     ***********************************************/
    
    for( i = 0; i < sInitRelObject->mAffectCheckNotNullCnt; i++ )
    {
        if ( sExecRelObject->mNotNullCollCnt[i] != 0 )
        {
            /* IMMEDIATE 일 경우 이미 에러가 발생했어야 함 */
            STL_DASSERT( sExecRelObject->mNotNullIsDeferred[i] == STL_TRUE );

            /**
             * Deferred Constarint 정보에 등록
             */
            
            STL_TRY( qlrcSetDeferredCollision( QLL_COLL_ITEM_CONSTRAINT,
                                               & sInitRelObject->mAffectCheckNotNullHandle[i],
                                               sExecRelObject->mNotNullCollCnt[i],
                                               aEnv )
                     == STL_SUCCESS );
                
        }
        else
        {
            /* nothing to do */
        }
    
    }

    /***********************************************
     * CHECK constraint
     ***********************************************/
    
    /**
     * @todo CHECK 에 대한 고려가 필요함
     */
    

    /***********************************************
     * Unique Index
     ***********************************************/

    /**
     * UNIQUE Index 의 Immediate Constraint 위반 Check 
     */
    
    for( i = 0; i < sInitRelObject->mAffectUniqueIndexCnt; i++ )
    {
        if ( sExecRelObject->mUniqueIndexCollCnt[i] != 0 )
        {
            if( sExecRelObject->mUniqueIndexIsDeferred[i] == STL_TRUE )
            {
                /**
                 * DEFERRED 인 경우
                 */

                /**
                 * Deferred Constarint 정보에 등록
                 */
                
                STL_TRY( qlrcSetDeferredCollision( QLL_COLL_ITEM_UNIQUE_INDEX,
                                                   & sInitRelObject->mAffectUniqueIndexHandle[i],
                                                   sExecRelObject->mUniqueIndexCollCnt[i],
                                                   aEnv )
                         == STL_SUCCESS );
                
            }
            else
            {
                /**
                 * IMMEDIATE 인 경우
                 */
                
                sViolatedHandle = & sInitRelObject->mAffectUniqueIndexHandle[i];
                
                STL_TRY( ellGetSchemaDictHandleByID( aTransID,
                                                     aSQLStmt->mViewSCN,
                                                     ellGetIndexSchemaID(sViolatedHandle),
                                                     & sSchemaHandle,
                                                     & sObjectExist,
                                                     ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                STL_DASSERT( sObjectExist == STL_TRUE );

                STL_THROW( RAMP_ERR_UNIQUE_INDEX_INTEGRITY_VIOLATED );
            }
        }
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CANNOT_SUPPLEMENTAL_LOG_DATA )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CANNOT_MAKE_PRIMARY_KEY_SUPPLEMENTAL_LOG_DATA_WITH_VIOLATION,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_UNIQUE_INTEGRITY_VIOLATED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_UNIQUE_CONSTRAINT_VIOLATED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      ellGetSchemaName( & sSchemaHandle ),
                      ellGetConstraintName( sViolatedHandle ) );
    }

    STL_CATCH( RAMP_ERR_UNIQUE_INDEX_INTEGRITY_VIOLATED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_UNIQUE_CONSTRAINT_VIOLATED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      ellGetSchemaName( & sSchemaHandle ),
                      ellGetIndexName( sViolatedHandle ) );
    }
    
    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "qlxUpdateCheckStmtCollision()" );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlxUpdate */


