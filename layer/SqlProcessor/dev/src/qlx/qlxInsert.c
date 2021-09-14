/*******************************************************************************
 * qlxInsert.c
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
 * @file qlxInsert.c
 * @brief SQL Processor Layer INSERT statement execution
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @addtogroup qlxInsert
 * @{
 */


/**
 * @brief INSERT VALUES 구문을 실행한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement 객체
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlxExecuteInsertValues( smlTransId      aTransID,
                                  smlStatement  * aStmt,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  qllEnv        * aEnv )
{
    qlvInitInsertValues  * sInitPlan      = NULL;
    qllOptimizationNode  * sOptNode       = NULL;
    qllDataArea          * sDataArea      = NULL;

    qllResultSetDesc       * sResultSetDesc = NULL;
    qlnInstantTable        * sInstantTable = NULL;

    stlBool             sIsScrollableInstant = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_INSERT_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_INTO_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_QUERY_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mDataPlan != NULL, QLL_ERROR_STACK(aEnv) );


    /******************************************************
     * 정보 획득 
     ******************************************************/
    
    /**
     * 검증된 Plan 정보 획득 
     */

    STL_TRY( qlxInsertGetValidPlan( aTransID,
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

    if ( aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_QUERY_TYPE )
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
 * @brief INSERT 구문의 Constraint 들의 Check Time (IMMEDIATE, DEFERRED) 을 획득
 * @param[in] aInitRelObject  Insert Init Related Object
 * @param[in] aOptNode        Optimization Node
 * @param[in] aDataArea       Data Area (Data Optimization 결과물)
 * @param[in] aEnv            Environment
 * @remarks
 */
stlStatus qlxInsertGetConstraintCheckTime(  qlvInitInsertRelObject * aInitRelObject,
                                            qllOptimizationNode    * aOptNode,
                                            qllDataArea            * aDataArea,
                                            qllEnv                 * aEnv )
{
    qllExecutionNode      * sExecNode = NULL;
    qlnxInsertStmt        * sExecInsertStmt = NULL;
    
    stlInt32 i = 0;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aInitRelObject != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * INSERT STATEMENT Execution Node 획득
     */

    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );
    sExecInsertStmt = sExecNode->mExecNode;

    /**
     * PRIMARY KEY constraint 의 check time 획득
     */
    
    if ( aInitRelObject->mPrimaryKeyCnt > 0 )
    {
        for ( i = 0; i < aInitRelObject->mPrimaryKeyCnt; i++ )
        {
            if ( qllGetTransUniqueMode(aEnv) == QLL_UNIQUE_MODE_DEFERRED )
            {
                sExecInsertStmt->mRelObject.mPrimaryKeyIsDeferred[i] = STL_TRUE;
            }
            else
            {
                sExecInsertStmt->mRelObject.mPrimaryKeyIsDeferred[i] =
                    qlrcIsCheckTimeDeferred( & aInitRelObject->mPrimaryKeyHandle[i], aEnv );
            }
            
            sExecInsertStmt->mRelObject.mPrimaryKeyCollCnt[i] = 0;
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
                sExecInsertStmt->mRelObject.mUniqueKeyIsDeferred[i] = STL_TRUE;
            }
            else
            {
                sExecInsertStmt->mRelObject.mUniqueKeyIsDeferred[i] =
                    qlrcIsCheckTimeDeferred( & aInitRelObject->mUniqueKeyHandle[i], aEnv );
            }

            sExecInsertStmt->mRelObject.mUniqueKeyCollCnt[i] = 0;
        }
    }

    /**
     * FOREIGN constraint 의 check time 획득
     */
    
    if ( aInitRelObject->mForeignKeyCnt > 0 )
    {
        for ( i = 0; i < aInitRelObject->mForeignKeyCnt; i++ )
        {
            sExecInsertStmt->mRelObject.mForeignKeyIsDeferred[i] =
                qlrcIsCheckTimeDeferred( & aInitRelObject->mForeignKeyHandle[i], aEnv );

            sExecInsertStmt->mRelObject.mForeignKeyCollCnt[i] = 0;
        }
    }

    /**
     * @todo
     * CHILD FOREIGN KEY constraint 의 check time 획득
     */

    /**
     * NOT NULL constraint 의 check time 획득
     */
    
    if ( aInitRelObject->mCheckNotNullCnt > 0 )
    {
        for ( i = 0; i < aInitRelObject->mCheckNotNullCnt; i++ )
        {
            sExecInsertStmt->mRelObject.mNotNullIsDeferred[i] =
                qlrcIsCheckTimeDeferred( & aInitRelObject->mCheckNotNullHandle[i], aEnv );
            
            sExecInsertStmt->mRelObject.mNotNullCollCnt[i] = 0;
        }
    }

    /**
     * CHECK constraint 의 check time 획득
     */
    
    if ( aInitRelObject->mCheckClauseCnt > 0 )
    {
        for ( i = 0; i < aInitRelObject->mCheckClauseCnt; i++ )
        {
            sExecInsertStmt->mRelObject.mCheckIsDeferred[i] =
                qlrcIsCheckTimeDeferred( & aInitRelObject->mCheckClauseHandle[i], aEnv );
            
            sExecInsertStmt->mRelObject.mCheckCollCnt[i] = 0;
        }
    }
    
    /**
     * UNIQUE Index의 check time 획득
     */
    
    if ( aInitRelObject->mUniqueIndexCnt > 0 )
    {
        for ( i = 0; i < aInitRelObject->mUniqueIndexCnt; i++ )
        {
            if ( qllGetTransUniqueMode(aEnv) == QLL_UNIQUE_MODE_DEFERRED )
            {
                sExecInsertStmt->mRelObject.mUniqueIndexIsDeferred[i] = STL_TRUE;
            }
            else
            {
                sExecInsertStmt->mRelObject.mUniqueIndexIsDeferred[i] = STL_FALSE;
            }
            sExecInsertStmt->mRelObject.mUniqueIndexCollCnt[i] = 0;
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief INSERT VALUES 수행을 위한 Lock 을 잡고 유효한 Plan 을 얻는다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aDBCStmt      DBC Statement
 * @param[in]  aSQLStmt      SQL Statement
 * @param[in]  aQueryPhase   Query Processing Phase
 * @param[out] aInitPlan  INSERT VALUES Init Plan
 * @param[out] aCodePlan  INSERT VALUES Code Plan    
 * @param[out] aDataPlan  INSERT VALUES Data Plan
 * @param[in]  aEnv          Environment
 */
stlStatus qlxInsertGetValidPlan( smlTransId             aTransID,
                                 qllDBCStmt           * aDBCStmt,
                                 qllStatement         * aSQLStmt,
                                 qllQueryPhase          aQueryPhase,
                                 qlvInitInsertValues ** aInitPlan,
                                 qllOptimizationNode ** aCodePlan,
                                 qllDataArea         ** aDataPlan,
                                 qllEnv               * aEnv )
{
    qlvInitInsertValues * sInitPlan = NULL;
    qllOptimizationNode * sCodePlan = NULL;
    qllDataArea         * sDataPlan = NULL;
    
    stlBool  sIsValid       = STL_FALSE;
    stlBool  sSegmentExist  = STL_FALSE;
    stlBool  sNeedRecompile = STL_FALSE;

    /**
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_INSERT_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_INTO_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_QUERY_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aQueryPhase > QLL_PHASE_VALIDATION) &&
                        (aQueryPhase <= QLL_PHASE_EXECUTE),
                        QLL_ERROR_STACK(aEnv) );
    

    /**
     * 기본 정보 설정
     */

    if( (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_INTO_TYPE) ||
        (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_QUERY_TYPE) )
    {
        sInitPlan =
            ((qlvInitInsertReturnInto *) qllGetInitPlan( aSQLStmt ))->mInitInsertValues;
    }
    else
    {
        sInitPlan = (qlvInitInsertValues *) qllGetInitPlan( aSQLStmt );
    }
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
            
            if( (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_INTO_TYPE) ||
                (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_QUERY_TYPE) )
            {
                sInitPlan =
                    ((qlvInitInsertReturnInto *) qllGetInitPlan( aSQLStmt ))->mInitInsertValues;
            }
            else
            {
                sInitPlan = (qlvInitInsertValues *) qllGetInitPlan( aSQLStmt );
            }
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
    
        if( aQueryPhase == QLL_PHASE_EXECUTE )
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
    
            if( sIsValid == STL_TRUE )
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
 * @brief INSERT Row 들의 Check Constraint 무결성을 검증한다.
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aSQLStmt           SQL Statement
 * @param[in]  aTableHandle       Table Handle
 * @param[in]  aIniRelObject      Related Object 의 Init Plan
 * @param[in]  aExeRelObject      Related Object 의 Exec Plan
 * @param[in]  aEnv               Environment
 */
stlStatus qlxInsertIntegrityCheckConst( smlTransId               aTransID,
                                        qllStatement           * aSQLStmt,
                                        ellDictHandle          * aTableHandle,
                                        qlvInitInsertRelObject * aIniRelObject,
                                        qlxExecInsertRelObject * aExeRelObject,
                                        qllEnv                 * aEnv )
{
    ellDictHandle    sViolatedSchemaHandle;
    ellDictHandle  * sViolatedColumnHandle = NULL;
    stlBool          sObjectExist = STL_FALSE;

    stlInt32    i = 0;
    stlInt32    sCollCnt = 0;
    stlInt32    sViolPos = 0;
    
    ellInitDictHandle( & sViolatedSchemaHandle );
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIniRelObject != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExeRelObject != NULL, QLL_ERROR_STACK(aEnv) );
    
    /**************************************************
     * Identity Column 의 Not Null 검사
     **************************************************/

    if ( aIniRelObject->mHasIdentity == STL_TRUE )
    {
        qlrcCheckNotNull4Insert( aExeRelObject->mIdentityColumnBlock, & sCollCnt, & sViolPos );

        if( sCollCnt == 0 )
        {
            /* collision 이 없음 */
        }
        else
        {
            STL_TRY( ellGetSchemaDictHandleByID(
                         aTransID,
                         aSQLStmt->mViewSCN,
                         ellGetTableSchemaID( aTableHandle ),
                         & sViolatedSchemaHandle,
                         & sObjectExist,
                         ELL_ENV(aEnv) )
                     == STL_SUCCESS );
            STL_DASSERT( sObjectExist == STL_TRUE );
            
            sViolatedColumnHandle = & aIniRelObject->mIdentityColumnHandle;
            
            STL_THROW( RAMP_ERR_NOT_NULL_INTEGRITY );
        }
    }
    else
    {
        /* nothing to do */
    }

    /**************************************************
     * NOT NULL constraint 검사
     * - PRIMARY KEY constraint 의 Not Null 검사는 Primary Key 에서 이루어진다.
     **************************************************/
    
    for ( i = 0; i < aIniRelObject->mCheckNotNullCnt; i++ )
    {
        sCollCnt = 0;

        qlrcCheckNotNull4Insert( aExeRelObject->mNotNullColumnBlock[i], & sCollCnt, & sViolPos );

        if ( sCollCnt == 0 )
        {
            /* collision 이 없음 */
        }
        else
        {
            if ( aExeRelObject->mNotNullIsDeferred[i] == STL_TRUE )
            {
                /**
                 * DEFERRED 인 경우
                 */

                aExeRelObject->mNotNullCollCnt[i] = aExeRelObject->mNotNullCollCnt[i] + sCollCnt;
            }
            else
            {
                /**
                 * IMMEDIATE 인 경우
                 */

                STL_TRY( ellGetSchemaDictHandleByID(
                             aTransID,
                             aSQLStmt->mViewSCN,
                             ellGetTableSchemaID( aTableHandle ),
                             & sViolatedSchemaHandle,
                             & sObjectExist,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                STL_DASSERT( sObjectExist == STL_TRUE );

                sViolatedColumnHandle = ellGetCheckNotNullColumnHandle( & aIniRelObject->mCheckNotNullHandle[i] );
                
                STL_THROW( RAMP_ERR_NOT_NULL_INTEGRITY );
            }
        }
    }

    /**************************************************
     * CHECK constraint 검사
     **************************************************/
    
    /**
     * @todo CHECK constraint 검사
     */

    STL_TRY_THROW( aIniRelObject->mCheckClauseCnt == 0, RAMP_ERR_NOT_IMPLEMENTED );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_NULL_INTEGRITY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CANNOT_INSERT_NULL_INTO,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      ellGetSchemaName( & sViolatedSchemaHandle ),
                      ellGetTableName( aTableHandle ),
                      ellGetColumnName( sViolatedColumnHandle ) );
    }

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "qlxInsertIntegrityCheckConst()" );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief INSERT VALUES 의 Key Constraint 무결성을 검증하고 Index Key 를 추가한다.
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aStmt              Statement
 * @param[in]  aTableHandle       Table Handle
 * @param[in]  aIniRelObject      Related Object 의 Init Plan
 * @param[in]  aExeRelObject      Related Object 의 Data Plan
 * @param[in]  aRowBlock          SM Row Block
 * @param[in]  aEnv               Environment
 */
stlStatus qlxInsertIntegrityKeyConstIndexKey( smlTransId               aTransID,
                                              smlStatement           * aStmt,
                                              ellDictHandle          * aTableHandle,
                                              qlvInitInsertRelObject * aIniRelObject,
                                              qlxExecInsertRelObject * aExeRelObject,
                                              smlRowBlock            * aRowBlock,
                                              qllEnv                 * aEnv )
{
    stlInt32 sViolatedCnt = 0;
    stlInt32 sKeyCollCnt = 0;
    stlInt32 sNullCollCnt = 0;
    
    stlInt32 sViolPos = 0;

    stlBool  sObjectExist = STL_FALSE;
    
    ellDictHandle  sViolatedSchemaHandle;
    ellDictHandle  * sKeyColumnHandle = NULL;
    ellDictHandle  * sViolatedColumnHandle = NULL;

    smlUniqueViolationScope sUniqueViolationScope;

    stlInt32 i = 0;

    ellInitDictHandle( & sViolatedSchemaHandle );
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIniRelObject != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExeRelObject != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRowBlock != NULL, QLL_ERROR_STACK(aEnv) );

    /*******************************************************************
     * PRIMARY KEY Constraint 검사
     *******************************************************************/
    
    for ( i = 0; i < aIniRelObject->mPrimaryKeyCnt; i++ )
    {
        /**
         * 각 Row 의 Primary Key Constraint 검사
         */

        STL_DASSERT( SML_USED_BLOCK_SIZE( aRowBlock )
                     == KNL_GET_BLOCK_USED_CNT( aExeRelObject->mPrimaryKeyIndexBlock[i] ) );

        sViolatedCnt = 0;
        sKeyCollCnt = 0;
        sNullCollCnt = 0;

        /**
         * PRIMARY KEY 의 not null 속성 검사
         */
            
        qlrcCheckNotNull4Insert( aExeRelObject->mPrimaryKeyIndexBlock[i], & sNullCollCnt, & sViolPos );

        if ( sNullCollCnt > 0 )
        {
            if ( aExeRelObject->mPrimaryKeyIsDeferred[i] == STL_TRUE )
            {
                /**
                 * Deferred Constraint Violation
                 */
            }
            else
            {
                /**
                 * Immediate Constraint Violation
                 */

                STL_TRY( ellGetSchemaDictHandleByID(
                             aTransID,
                             aStmt->mScn,
                             ellGetTableSchemaID( aTableHandle ),
                             & sViolatedSchemaHandle,
                             & sObjectExist,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                STL_DASSERT( sObjectExist == STL_TRUE );

                sKeyColumnHandle = ellGetPrimaryKeyColumnHandle( & aIniRelObject->mPrimaryKeyHandle[i] );
                sViolatedColumnHandle = & sKeyColumnHandle[sViolPos];

                /* Oracle, 하위 호환성을 위해 error 를 동일하게 하자 */
                STL_THROW( RAMP_ERR_NOT_NULL_INTEGRITY );
            }
        }
        else
        {
            STL_DASSERT( sNullCollCnt == 0 );
        }
        
        /**
         * Index Key 추가 
         */
            
        STL_TRY( smlInsertKeys( aStmt,
                                aIniRelObject->mPrimaryKeyIndexPhyHandle[i],
                                aExeRelObject->mPrimaryKeyIndexBlock[i],
                                aRowBlock,
                                NULL,  /* aBlockFilter */
                                aExeRelObject->mPrimaryKeyIsDeferred[i],
                                & sKeyCollCnt,
                                & sUniqueViolationScope,
                                SML_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sUniqueViolationScope != SML_UNIQUE_VIOLATION_SCOPE_NONE )        
        {
            if ( aExeRelObject->mPrimaryKeyIsDeferred[i] == STL_TRUE )
            {
                /**
                 * Deferred Constraint Violation
                 */
            }
            else
            {
                /**
                 * Immediate Constraint Violation
                 */

                STL_TRY( ellGetSchemaDictHandleByID(
                             aTransID,
                             aStmt->mScn,
                             ellGetConstraintSchemaID( & aIniRelObject->mPrimaryKeyHandle[i] ),
                             & sViolatedSchemaHandle,
                             & sObjectExist,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                STL_DASSERT( sObjectExist == STL_TRUE );

                STL_THROW( RAMP_ERR_PRIMARY_KEY_INTEGRITY );
            }
        }
        else
        {
            /* no violation */
        }

        /**
         * Collision 처리 
         */

        sViolatedCnt = sNullCollCnt + sKeyCollCnt;
        
        if ( sViolatedCnt == 0 )
        {
            /**
             * Collision 증가가 없음
             */
        }
        else
        {
            /**
             * Collision 증가 정보를 누적
             */
            
            STL_DASSERT( sViolatedCnt > 0 );

            aExeRelObject->mPrimaryKeyCollCnt[i] += sViolatedCnt;
        }
    }

    /*******************************************************************
     * UNIQUE Constraint 검사
     *******************************************************************/
    
    for ( i = 0; i < aIniRelObject->mUniqueKeyCnt; i++ )
    {
        /**
         * 각 Row 의 Unique Key Constraint 검사
         */

        STL_DASSERT( SML_USED_BLOCK_SIZE( aRowBlock )
                     == KNL_GET_BLOCK_USED_CNT( aExeRelObject->mUniqueKeyIndexBlock[i] ) );

        sKeyCollCnt = 0;
        
        /**
         * Index Key 추가 
         */
            
        STL_TRY( smlInsertKeys( aStmt,
                                aIniRelObject->mUniqueKeyIndexPhyHandle[i],
                                aExeRelObject->mUniqueKeyIndexBlock[i],
                                aRowBlock,
                                NULL,  /* aBlockFilter */
                                aExeRelObject->mUniqueKeyIsDeferred[i],
                                & sKeyCollCnt,
                                & sUniqueViolationScope,
                                SML_ENV(aEnv) )
                 == STL_SUCCESS );

        sViolatedCnt = sKeyCollCnt;
        
        /**
         * Collision 처리 
         */
            
        if ( sUniqueViolationScope != SML_UNIQUE_VIOLATION_SCOPE_NONE )
        {
            if ( aExeRelObject->mUniqueKeyIsDeferred[i] == STL_TRUE )
            {
                /**
                 * Deferred Constraint Violation
                 */
            }
            else
            {
                /**
                 * Immediate Constraint Violation
                 */

                STL_TRY( ellGetSchemaDictHandleByID(
                             aTransID,
                             aStmt->mScn,
                             ellGetConstraintSchemaID( & aIniRelObject->mUniqueKeyHandle[i] ),
                             & sViolatedSchemaHandle,
                             & sObjectExist,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                STL_DASSERT( sObjectExist == STL_TRUE );

                STL_THROW( RAMP_ERR_UNIQUE_KEY_INTEGRITY );
            }
        }
        else
        {
            /* no violation */
        }

        if ( sViolatedCnt == 0 )
        {
            /**
             * Collision 증가가 없음
             */
        }
        else
        {
            /**
             * Collision 증가 정보를 누적
             */
            
            STL_DASSERT( sViolatedCnt > 0 );

            aExeRelObject->mUniqueKeyCollCnt[i] += sViolatedCnt;
        }
    }

    /*******************************************************************
     * FOREIGN KEY constraint 검사
     *******************************************************************/
    
    /**
     * @todo FOREIGN KEY 구현해야 함
     */

    STL_TRY_THROW( aIniRelObject->mForeignKeyCnt == 0, RAMP_ERR_NOT_IMPLEMENTED );

    /*******************************************************************
     * CHILD FOREIGN KEY constraint 검사
     *******************************************************************/
    
    /**
     * @todo CHILD FOREIGN KEY 구현해야 함
     * -- DEFERRED 인 경우 INSERT 시에도 foreign key violation 을 resolution 할 수 있어야 함.
     */

    /* STL_TRY_THROW( aIniRelObject->mChildForeignKeyCnt == 0, RAMP_ERR_NOT_IMPLEMENTED ); */
    
    /*******************************************************************
     * Unique Index
     *******************************************************************/
    
    for ( i = 0; i < aIniRelObject->mUniqueIndexCnt; i++ )
    {
        /**
         * 각 Row 의 Index Key 추가 
         */

        STL_DASSERT( SML_USED_BLOCK_SIZE( aRowBlock )
                     == KNL_GET_BLOCK_USED_CNT( aExeRelObject->mUniqueIndexBlock[i] ) );

        sKeyCollCnt = 0;
        
        STL_TRY( smlInsertKeys( aStmt,
                                aIniRelObject->mUniqueIndexPhyHandle[i],
                                aExeRelObject->mUniqueIndexBlock[i],
                                aRowBlock,
                                NULL,       /* aBlockFilter */
                                aExeRelObject->mUniqueIndexIsDeferred[i],
                                & sKeyCollCnt,
                                & sUniqueViolationScope,
                                SML_ENV(aEnv) )
                 == STL_SUCCESS );

        sViolatedCnt = sKeyCollCnt;
        
        /**
         * Collision 처리 
         */
            
        if ( sUniqueViolationScope != SML_UNIQUE_VIOLATION_SCOPE_NONE )
        {
            if ( aExeRelObject->mUniqueIndexIsDeferred[i] == STL_TRUE )
            {
                /**
                 * Deferred Constraint Violation
                 */
            }
            else
            {
                /**
                 * Immediate Constraint Violation
                 */

                STL_TRY( ellGetSchemaDictHandleByID(
                             aTransID,
                             aStmt->mScn,
                             ellGetIndexSchemaID( & aIniRelObject->mUniqueIndexHandle[i] ),
                             & sViolatedSchemaHandle,
                             & sObjectExist,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                STL_DASSERT( sObjectExist == STL_TRUE );

                STL_THROW( RAMP_ERR_UNIQUE_INDEX_INTEGRITY );
            }
        }
        else
        {
            /* no violation */
        }

        if ( sViolatedCnt == 0 )
        {
            /**
             * Collision 증가가 없음
             */
        }
        else
        {
            /**
             * Collision 증가 정보를 누적
             */
            
            STL_DASSERT( sViolatedCnt > 0 );

            aExeRelObject->mUniqueIndexCollCnt[i] += sViolatedCnt;
        }
    }

    /*******************************************************************
     * Non-Unique Index
     *******************************************************************/
    
    for ( i = 0; i < aIniRelObject->mNonUniqueIndexCnt; i++ )
    {
        /**
         * 각 Row 의 Index Key 추가 
         */

        STL_DASSERT( SML_USED_BLOCK_SIZE( aRowBlock )
                     == KNL_GET_BLOCK_USED_CNT( aExeRelObject->mNonUniqueIndexBlock[i] ) );

        sKeyCollCnt = 0;
        
        STL_TRY( smlInsertKeys( aStmt,
                                aIniRelObject->mNonUniqueIndexPhyHandle[i],
                                aExeRelObject->mNonUniqueIndexBlock[i],
                                aRowBlock,
                                NULL,       /* aBlockFilter */
                                STL_FALSE,  /* aIsDeferred */
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
                      QLL_ERRCODE_CANNOT_INSERT_NULL_INTO,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      ellGetSchemaName( & sViolatedSchemaHandle ),
                      ellGetTableName( aTableHandle ),
                      ellGetColumnName( sViolatedColumnHandle ) );
    }
    
    STL_CATCH( RAMP_ERR_PRIMARY_KEY_INTEGRITY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_UNIQUE_CONSTRAINT_VIOLATED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      ellGetSchemaName( & sViolatedSchemaHandle ),
                      ellGetConstraintName( & aIniRelObject->mPrimaryKeyHandle[i] ) );
    }

    STL_CATCH( RAMP_ERR_UNIQUE_KEY_INTEGRITY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_UNIQUE_CONSTRAINT_VIOLATED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      ellGetSchemaName( & sViolatedSchemaHandle ),
                      ellGetConstraintName( & aIniRelObject->mUniqueKeyHandle[i] ) );
    }

    STL_CATCH( RAMP_ERR_UNIQUE_INDEX_INTEGRITY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_UNIQUE_CONSTRAINT_VIOLATED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      ellGetSchemaName( & sViolatedSchemaHandle ),
                      ellGetIndexName( & aIniRelObject->mUniqueIndexHandle[i] ) );
    }

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "qlxInsertIntegrityKeyConstIndexKey()" );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief INSERT 구문에 대한 collision 정보를 검사한다.
 * @param[in] aTransID          Transaction ID
 * @param[in] aStmt             SM statement
 * @param[in] aDataArea         Data Area
 * @param[in] aExecNode         UPDATE Statement Execution Node
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus qlxInsertCheckStmtCollision( smlTransId          aTransID,
                                       smlStatement      * aStmt,
                                       qllDataArea       * aDataArea,
                                       qllExecutionNode  * aExecNode,
                                       qllEnv            * aEnv )
{
    qlnoInsertStmt          * sOptInsertStmt = NULL;
    qlnxInsertStmt          * sExeInsertStmt = NULL;
    qlvInitInsertRelObject  * sInitRelObject = NULL;
    qlxExecInsertRelObject  * sExecRelObject = NULL;

    stlInt32                  i = 0;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExecNode != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * INSERT STATEMENT Node 획득
     */

    sOptInsertStmt = (qlnoInsertStmt *) aExecNode->mOptNode->mOptNode;
    sExeInsertStmt = (qlnxInsertStmt *) aExecNode->mExecNode;
    sInitRelObject = sOptInsertStmt->mRelObject;
    sExecRelObject = & sExeInsertStmt->mRelObject;

    /**
     * Primary Key 의 Immediate Constraint 위반 Check 
     */

    sInitRelObject = sOptInsertStmt->mRelObject;
    
    for( i = 0; i < sInitRelObject->mPrimaryKeyCnt; i++ )
    {
        if ( sExecRelObject->mPrimaryKeyCollCnt[i] != 0 )
        {
            /* collision 이 증가되는 경우만 있어야 함 */
            STL_DASSERT( sExecRelObject->mPrimaryKeyCollCnt[i] > 0 );
            /* IMMEDIATE 일 경우 이미 에러가 발생했어야 함 */
            STL_DASSERT( sExecRelObject->mPrimaryKeyIsDeferred[i] == STL_TRUE );

            /**
             * Supplemental Log Data 를 남길 수 없음.
             */
            
            STL_TRY_THROW( sExecRelObject->mIsSuppLog == STL_FALSE, RAMP_ERR_CANNOT_SUPPLEMENTAL_LOG_DATA );
            
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
            /* collision 이 증가되는 경우만 있어야 함 */
            STL_DASSERT( sExecRelObject->mUniqueKeyCollCnt[i] > 0 );
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
     * NOT NULL Immediate Constraint 위반 Check 
     */

    for( i = 0; i < sInitRelObject->mCheckNotNullCnt; i++ )
    {
        if ( sExecRelObject->mNotNullCollCnt[i] != 0 )
        {
            /* collision 이 증가되는 경우만 있어야 함 */
            STL_DASSERT( sExecRelObject->mNotNullCollCnt[i] > 0 );
            /* IMMEDIATE 일 경우 이미 에러가 발생했어야 함 */
            STL_DASSERT( sExecRelObject->mNotNullIsDeferred[i] == STL_TRUE );

            /**
             * Deferred Constarint 정보에 등록
             */

            STL_TRY( qlrcSetDeferredCollision( QLL_COLL_ITEM_CONSTRAINT,
                                               & sInitRelObject->mCheckNotNullHandle[i],
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
     * @todo CHECK Constraint 에 대한 고려가 필요함
     */

    /**
     * Unique Index 의 Immediate Constraint 위반 Check 
     */
    
    for( i = 0; i < sInitRelObject->mUniqueIndexCnt; i++ )
    {
        if ( sExecRelObject->mUniqueIndexCollCnt[i] != 0 )
        {
            /* collision 이 증가되는 경우만 있어야 함 */
            STL_DASSERT( sExecRelObject->mUniqueIndexCollCnt[i] > 0 );
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

    STL_CATCH( RAMP_ERR_CANNOT_SUPPLEMENTAL_LOG_DATA )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CANNOT_MAKE_PRIMARY_KEY_SUPPLEMENTAL_LOG_DATA_WITH_VIOLATION,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "qlxInsertCheckStmtCollision()" );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/** @} qlxInsert */


