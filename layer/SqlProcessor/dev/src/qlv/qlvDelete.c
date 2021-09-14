/*******************************************************************************
 * qlvDelete.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file qlvDelete.c
 * @brief SQL Processor Layer DELETE statement Validation
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @addtogroup qlvDelete
 * @{
 */

/**
 * @brief DELETE 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlvValidateDelete( smlTransId      aTransID,
                             qllDBCStmt    * aDBCStmt,
                             qllStatement  * aSQLStmt,
                             stlChar       * aSQLString,
                             qllNode       * aParseTree,
                             qllEnv        * aEnv )
{
    ellDictHandle        * sAuthHandle   = NULL;

    qlpDelete            * sParseTree    = NULL;
    qlvInitDelete        * sInitPlan     = NULL;

    qlpObjectName        * sObjectName   = NULL;
    stlBool                sObjectExist  = STL_FALSE;

    ellDictHandle          sSchemaHandle;

    qlvRefTableList      * sReadTableList = NULL;
    qlpBaseTableNode       sTableNode;

    qlvInitStmtExprList  * sStmtExprList  = NULL;
    qlvInitExprList      * sQueryExprList = NULL;
    qlvStmtInfo            sStmtInfo;

    stlInt32               sTableCount;
    stlInt32               sQBIndex;

    ellInitDictHandle( &sSchemaHandle );
 

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_DELETE_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_DELETE_WHERE_CURRENT_OF_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_DELETE_RETURNING_QUERY_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_DELETE_RETURNING_INTO_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aParseTree->mType == QLL_STMT_DELETE_TYPE) ||
                        (aParseTree->mType == QLL_STMT_DELETE_WHERE_CURRENT_OF_TYPE) ||
                        (aParseTree->mType == QLL_STMT_DELETE_RETURNING_QUERY_TYPE) ||
                        (aParseTree->mType == QLL_STMT_DELETE_RETURNING_INTO_TYPE),
                        QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * 준비 작업
     **********************************************************/

    /**
     * 기본 정보 획득
     */
    
    sParseTree  = (qlpDelete *) aParseTree;
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV( aEnv ) );


    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlvInitDelete ),
                                (void **) & sInitPlan,
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF( qlvInitDelete ) );

    STL_TRY( qlvSetInitPlan( aDBCStmt,
                             aSQLStmt,
                             (qlvInitPlan *) sInitPlan,
                             aEnv )
             == STL_SUCCESS );

    /**
     * Statement 단위 Expression List 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlvInitStmtExprList ),
                                (void **) & sStmtExprList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sStmtExprList->mPseudoColExprList,
                                   QLL_INIT_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sStmtExprList->mConstExprList,
                                   QLL_INIT_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    sStmtExprList->mHasSubQuery = STL_FALSE;
    sInitPlan->mStmtExprList = sStmtExprList;


    /**
     * Query 단위 Expression List 설정
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlvInitExprList ),
                                (void **) & sQueryExprList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sQueryExprList->mPseudoColExprList,
                                   QLL_INIT_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sQueryExprList->mAggrExprList,
                                   QLL_INIT_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sQueryExprList->mNestedAggrExprList,
                                   QLL_INIT_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    sQueryExprList->mStmtExprList = sStmtExprList;

    sInitPlan->mQueryExprList = sQueryExprList;


    /**
     * statement information 설정
     */

    sQBIndex = 0;
    sStmtInfo.mTransID       = aTransID;
    sStmtInfo.mHasHintError  = sParseTree->mHasHintErr;
    sStmtInfo.mQBIndex       = &sQBIndex;
    stlStrcpy( sStmtInfo.mQBPrefix, QLV_QUERY_BLOCK_NAME_PREFIX_DEL );
    sStmtInfo.mDBCStmt       = aDBCStmt;
    sStmtInfo.mSQLStmt       = aSQLStmt;
    sStmtInfo.mSQLString     = aSQLString;
    sStmtInfo.mQueryExprList = sQueryExprList;


    /**********************************************************
     * Table Name Validation
     **********************************************************/
    
    sObjectName = sParseTree->mRelation->mName;

    if ( sObjectName->mSchema == NULL )
    {
        /**
         * Schema Name 이 명시되지 않은 경우
         */
        
        STL_TRY( qlvGetTableDictHandleByAuthHandleNTblName( aTransID,
                                                            aDBCStmt,
                                                            aSQLStmt,
                                                            aSQLString,
                                                            sAuthHandle,
                                                            sObjectName->mObject,
                                                            sObjectName->mObjectPos,
                                                            & sInitPlan->mTableHandle,
                                                            & sObjectExist,
                                                            aEnv )
                 == STL_SUCCESS );
        
        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_TABLE_NOT_EXISTS );
    }
    else
    {
        /**
         * Schema Name 이 명시된 경우
         */

        STL_TRY( ellGetSchemaDictHandle( aTransID,
                                         aSQLStmt->mViewSCN,
                                         sObjectName->mSchema,
                                         & sSchemaHandle,
                                         & sObjectExist,
                                         ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SCHEMA_NOT_EXISTS );

        /**
         * Table 존재 여부 확인
         */

        STL_TRY( qlvGetTableDictHandleBySchHandleNTblName( aTransID,
                                                           aDBCStmt,
                                                           aSQLStmt,
                                                           aSQLString,
                                                           & sSchemaHandle,
                                                           sObjectName->mObject,
                                                           sObjectName->mObjectPos,
                                                           & sInitPlan->mTableHandle,
                                                           & sObjectExist,
                                                           aEnv )
                 == STL_SUCCESS );

        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_TABLE_NOT_EXISTS );
    }

    /**
     * Table Physical Handle 설정
     */

    sInitPlan->mTablePhyHandle = ellGetTableHandle( & sInitPlan->mTableHandle );
    
    
    /**
     * Built-In Table 인지 검사
     */

    STL_TRY_THROW( ellIsBuiltInTable( & sInitPlan->mTableHandle ) == STL_FALSE,
                   RAMP_ERR_TABLE_NOT_WRITABLE );

    /**
     * Base Table 인지 검사
     * @todo Updatable View 등을 고려하여 Writable Table 인지 검사
     */
    
    STL_TRY_THROW( ELL_TABLE_TYPE_BASE_TABLE == ellGetTableType( & sInitPlan->mTableHandle ),
                   RAMP_ERR_TABLE_NOT_WRITABLE );
    
    /**
     * DELETE ON TABLE 권한 검사
     */

    STL_TRY( qlaCheckTablePriv( aTransID,
                                aDBCStmt,
                                aSQLStmt,
                                ELL_TABLE_PRIV_ACTION_DELETE,
                                & sInitPlan->mTableHandle,
                                NULL,  /* aColumnList */
                                aEnv )
             == STL_SUCCESS );
                                
    /**********************************************************
     * Reference Table List 구성
     **********************************************************/

    /**
     * Read Reference Table List 생성
     */

    STL_TRY( qlvCreateRefTableList( & QLL_VALIDATE_MEM( aEnv ),
                                    NULL,
                                    & sReadTableList,
                                    aEnv )
             == STL_SUCCESS );


    /**
     * Read Table Node 생성
     */

    sTableNode.mType    = QLL_BASE_TABLE_NODE_TYPE;
    sTableNode.mNodePos = sParseTree->mRelation->mNodePos;
    sTableNode.mName    = sParseTree->mRelation->mName;
    sTableNode.mDumpOpt = NULL;
    sTableNode.mAlias   = sParseTree->mRelation->mAlias;


    /**
     * Read Reference Table List 구성
     */

    sTableCount = 0;
    STL_TRY( qlvTableNodeValidation( & sStmtInfo,
                                     sInitPlan->mInitPlan.mValidationObjList,
                                     sReadTableList,
                                     (qllNode *) & sTableNode,
                                     &(sInitPlan->mReadTableNode),
                                     &sTableCount,
                                     aEnv )
             == STL_SUCCESS );

    STL_DASSERT( sTableCount == 1 );

    STL_TRY( qlvSetNodeToRefTableItem( sReadTableList,
                                       sInitPlan->mReadTableNode,
                                       aEnv )
             == STL_SUCCESS );
    

    /**********************************************************
     * WHERE 절 Validation
     **********************************************************/

    /**
     * Where Expression 생성
     */
    
    if ( sParseTree->mWhere == NULL )
    {
        /**
         * WHERE 절이 없음
         */

        sInitPlan->mWhereExpr = NULL;
    }
    else
    {
        /**
         * WHERE Expression 검사
         */
        
        STL_TRY( qlvValidateValueExpr( & sStmtInfo,
                                       QLV_EXPR_PHRASE_CONDITION,
                                       sInitPlan->mInitPlan.mValidationObjList,
                                       sReadTableList,
                                       STL_FALSE,  /* Atomic */
                                       STL_FALSE,  /* Row Expr */
                                       0, /* Allowed Aggregation Depth */
                                       sParseTree->mWhere,
                                       & sInitPlan->mWhereExpr,
                                       NULL,
                                       KNL_BUILTIN_FUNC_INVALID,
                                       DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                       DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                       QLL_INIT_PLAN( aDBCStmt ),
                                       aEnv )
                 == STL_SUCCESS );

        /**
         * Rewrite Expression
         */

        if( sInitPlan->mWhereExpr != NULL )
        {
            STL_TRY( qlvRewriteExpr( sInitPlan->mWhereExpr,
                                     DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                     sQueryExprList,
                                     & sInitPlan->mWhereExpr,
                                     QLL_INIT_PLAN( aDBCStmt ),
                                     aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /* Do Nothing */
        }
    }


    /**********************************************************
     * Hint 분석
     **********************************************************/

    /**
     * Hint Info 생성
     */

    STL_TRY( qlvCreateHintInfo( &sInitPlan->mHintInfo,
                                1,
                                QLL_INIT_PLAN( sStmtInfo.mDBCStmt ),
                                aEnv )
             == STL_SUCCESS );

    STL_TRY( qlvValidateHintInfo( &sStmtInfo,
                                  STL_FALSE,
                                  STL_FALSE,
                                  STL_FALSE,
                                  (qlvInitNode*)sInitPlan->mReadTableNode,
                                  1,
                                  sParseTree->mHints,
                                  sInitPlan->mHintInfo,
                                  aEnv )
             == STL_SUCCESS );

    ((qlvInitPlan *) sInitPlan)->mHasHintError = sStmtInfo.mHasHintError;


    /**********************************************************
     * OFFSET .. LIMIT .. validation 
     **********************************************************/

    /**
     * OFFSET : Result Skip
     */

    if( sParseTree->mResultSkip == NULL )
    {
        sInitPlan->mResultSkip = NULL;
        sInitPlan->mSkipCnt = 0;
    }
    else
    {
        STL_TRY( qlvValidateValueExpr( & sStmtInfo,
                                       QLV_EXPR_PHRASE_OFFSETLIMIT,
                                       sInitPlan->mInitPlan.mValidationObjList,
                                       sReadTableList,
                                       STL_FALSE,  /* Atomic */
                                       STL_FALSE,  /* Row Expr */
                                       0, /* Allowed Aggregation Depth */
                                       sParseTree->mResultSkip,
                                       & sInitPlan->mResultSkip,
                                       NULL,
                                       KNL_BUILTIN_FUNC_INVALID,
                                       DTL_ITERATION_TIME_FOR_EACH_QUERY,
                                       DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                       QLL_INIT_PLAN( aDBCStmt ),
                                       aEnv )
                 == STL_SUCCESS );

        STL_TRY_THROW ( qlvIsValidSkipLimit( sInitPlan->mResultSkip, aEnv ) == STL_TRUE,
                        RAMP_ERR_INVALID_RESULT_SKIP );

        if( sInitPlan->mResultSkip->mType == QLV_EXPR_TYPE_VALUE )
        {
            STL_TRY( dtlGetIntegerFromString(
                         sInitPlan->mResultSkip->mExpr.mValue->mData.mString,
                         stlStrlen( sInitPlan->mResultSkip->mExpr.mValue->mData.mString ),
                         & sInitPlan->mSkipCnt,
                         QLL_ERROR_STACK( aEnv ) )
                     == STL_SUCCESS );

            STL_TRY_THROW( sInitPlan->mSkipCnt >= 0, RAMP_ERR_INVALID_RESULT_SKIP );
        }
        else
        {
            STL_DASSERT( sInitPlan->mResultSkip->mType == QLV_EXPR_TYPE_BIND_PARAM );
            sInitPlan->mSkipCnt = 0;
        }
    }

    
    /**
     * LIMIT : Result Limit
     */

    if( sParseTree->mResultLimit == NULL )
    {
        sInitPlan->mResultLimit = NULL;
        sInitPlan->mFetchCnt = QLL_FETCH_COUNT_MAX;
    }
    else
    {
        STL_TRY( qlvValidateValueExpr( & sStmtInfo,
                                       QLV_EXPR_PHRASE_OFFSETLIMIT,
                                       sInitPlan->mInitPlan.mValidationObjList,
                                       sReadTableList,
                                       STL_FALSE,  /* Atomic */
                                       STL_FALSE,  /* Row Expr */
                                       0, /* Allowed Aggregation Depth */
                                       sParseTree->mResultLimit,
                                       & sInitPlan->mResultLimit,
                                       NULL,
                                       KNL_BUILTIN_FUNC_INVALID,
                                       DTL_ITERATION_TIME_FOR_EACH_QUERY,
                                       DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                       QLL_INIT_PLAN( aDBCStmt ),
                                       aEnv )
                 == STL_SUCCESS );

        STL_TRY_THROW ( qlvIsValidSkipLimit( sInitPlan->mResultLimit, aEnv ) == STL_TRUE,
                        RAMP_ERR_INVALID_RESULT_LIMIT );

        if( sInitPlan->mResultLimit->mType == QLV_EXPR_TYPE_VALUE )
        {
            STL_TRY( dtlGetIntegerFromString(
                         sInitPlan->mResultLimit->mExpr.mValue->mData.mString,
                         stlStrlen( sInitPlan->mResultLimit->mExpr.mValue->mData.mString ),
                         & sInitPlan->mFetchCnt,
                         QLL_ERROR_STACK( aEnv ) )
                     == STL_SUCCESS );

            STL_TRY_THROW( sInitPlan->mFetchCnt > 0, RAMP_ERR_INVALID_RESULT_LIMIT );
        }
        else
        {
            STL_DASSERT( sInitPlan->mResultLimit->mType == QLV_EXPR_TYPE_BIND_PARAM );
            sInitPlan->mFetchCnt = QLL_FETCH_COUNT_MAX;
        }
    }


    /**********************************************************
     * DELETE 에 영향을 받는 SQL-Object 정보 획득 
     **********************************************************/
    
    STL_TRY( qlvGetRelObject4Delete( & sStmtInfo,
                                     sInitPlan,
                                     aEnv )
             == STL_SUCCESS );

    if ( (sInitPlan->mRelObject.mPrimaryKeyCnt > 0) ||
         (sInitPlan->mRelObject.mUniqueKeyCnt > 0) ||
         (sInitPlan->mRelObject.mForeignKeyCnt > 0) ||
         (sInitPlan->mRelObject.mUniqueIndexCnt > 0) ||
         (sInitPlan->mRelObject.mNonUniqueIndexCnt > 0) )
    {
        sInitPlan->mHasIndex = STL_TRUE;
    }
    else
    {
        sInitPlan->mHasIndex = STL_FALSE;
    }

    if ( sInitPlan->mRelObject.mDeferNotNullCnt > 0 )
    {
        sInitPlan->mHasCheck = STL_TRUE;
    }
    else
    {
        sInitPlan->mHasCheck = STL_FALSE;
    }

    /**********************************************************
     * DELETE 에 영향을 받는 SQL-Object 가 참조하는 Column List 구성
     **********************************************************/
    
    STL_TRY( qlvSetRefColumnListForDelete( & sStmtInfo,
                                           sInitPlan->mReadTableNode,
                                           sInitPlan,
                                           & sInitPlan->mRelObject,
                                           aEnv )
             == STL_SUCCESS );


    /**********************************************************
     * RETURNING 절 Validation
     **********************************************************/

    if( sParseTree->mReturnTarget != NULL )
    {
        STL_TRY( qlvValidateReturningPhrase( & sStmtInfo,
                                             sInitPlan->mInitPlan.mValidationObjList,
                                             sInitPlan->mReadTableNode,
                                             sParseTree->mReturnTarget,
                                             & sInitPlan->mReturnTargetCnt,
                                             & sInitPlan->mReturnTargetArray,
                                             aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sInitPlan->mReturnTargetCnt     = 0;
        sInitPlan->mReturnTargetArray   = NULL;
    }

    /**********************************************************
     * INTO 절 Validation
     **********************************************************/

    if( sParseTree->mIntoTarget != NULL )
    {
        /**
         * Target 개수만큼 INTO 절의 Out Paramter 공간 확보
         */

        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aDBCStmt ),
                                    STL_SIZEOF(qlvInitExpression) * sInitPlan->mReturnTargetCnt,
                                    (void **) & sInitPlan->mIntoTargetArray,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sInitPlan->mIntoTargetArray,
                   0x00,
                   STL_SIZEOF(qlvInitExpression) * sInitPlan->mReturnTargetCnt );


        /**
         * INTO phrase validation
         */

        STL_TRY( qlvValidateIntoPhrase( & sStmtInfo,
                                        sInitPlan->mReturnTargetCnt,
                                        sParseTree->mIntoTarget,
                                        sInitPlan->mIntoTargetArray,
                                        aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sInitPlan->mIntoTargetArray = NULL;
    }

    /**********************************************************
     * Version Conflict Resolution 을 위한 Serializability Action 
     **********************************************************/

    /**
     * Subquery 가 있다면 QLL_SERIAL_ROLLBACK_RETRY 해야 함.
     */
    
    if ( sQueryExprList->mStmtExprList->mHasSubQuery == STL_TRUE )
    {
        /**
         * Subquery 가 있는 경우
         */
        
        sInitPlan->mSerialAction = QLV_SERIAL_ROLLBACK_RETRY;
    }
    else
    {
        /**
         * Subquery 가 없는 경우
         */
        
        if ( sParseTree->mWhere == NULL )
        {
            /**
             * Where 절이 없는 경우 
             */
            sInitPlan->mSerialAction = QLV_SERIAL_NO_RETRY;
        }
        else
        {
            /**
             * Where 절이 있는 경우
             */
            sInitPlan->mSerialAction = QLV_SERIAL_RETRY_ONLY;
        }
    }
    
    /**********************************************************
     * 마무리 
     **********************************************************/

    /**
     * Init Plan 연결
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;


    /**
     * Target Info 설정
     */

    if( aSQLStmt->mStmtType == QLL_STMT_DELETE_RETURNING_QUERY_TYPE )
    {
        STL_TRY( qlfMakeQueryTarget( aSQLStmt,
                                     QLL_INIT_PLAN( aDBCStmt ),
                                     & sInitPlan->mTargetInfo,
                                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sInitPlan->mTargetInfo = NULL;
    }


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SCHEMA_NOT_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sObjectName->mSchemaPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sObjectName->mSchema );
    }

    STL_CATCH( RAMP_ERR_TABLE_NOT_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLE_OR_VIEW_NOT_EXIST,
                      qlgMakeErrPosString( aSQLString,
                                           sObjectName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sObjectName->mObject );
    }

    STL_CATCH( RAMP_ERR_TABLE_NOT_WRITABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLE_NOT_WRITABLE,
                      qlgMakeErrPosString( aSQLString,
                                           sObjectName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sObjectName->mObject );
    }

    STL_CATCH( RAMP_ERR_INVALID_RESULT_SKIP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_RESULT_OFFSET_MUST_BE_ZERO_OR_POSITIVE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mResultSkip->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_INVALID_RESULT_LIMIT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_RESULT_LIMIT_MUST_BE_POSITIVE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mResultLimit->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief DELETE 구문에 영향을 받는 SQL Object 정보를 획득한다.
 * @param[in]  aStmtInfo     Stmt Information
 * @param[in]  aInitPlan     DELETE Init Plan
 * @param[in]  aEnv          Environment
 */
stlStatus qlvGetRelObject4Delete( qlvStmtInfo   * aStmtInfo,
                                  qlvInitDelete * aInitPlan,
                                  qllEnv        * aEnv )
{
    stlInt32                  i            = 0;
     
    ellDictHandle           * sIndexHandle = NULL;
    qlvInitDeleteRelObject  * sRelObject   = NULL;
    qllDBCStmt              * sDBCStmt     = NULL;

    stlInt32         sNotNullCnt = 0;
    stlInt32         sDeferIdx = 0;
    ellDictHandle *  sNotNullHandleArray = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    

    /******************************************************
     * Key Constraint 정보 획득
     * - key constraint 를 위한 Index 정보도 key handle 에 포함되어 있음 
     ******************************************************/

    sDBCStmt   = aStmtInfo->mDBCStmt;
    sRelObject = & aInitPlan->mRelObject;
    
    /**
     * Primary key 정보 획득
     */

    STL_TRY( ellGetTablePrimaryKeyDictHandle( aStmtInfo->mTransID,
                                              aStmtInfo->mSQLStmt->mViewSCN,
                                              QLL_INIT_PLAN(sDBCStmt),
                                              & aInitPlan->mTableHandle,
                                              & sRelObject->mPrimaryKeyCnt,
                                              & sRelObject->mPrimaryKeyHandle,
                                              ELL_ENV(aEnv) )
             == STL_SUCCESS );

    if ( sRelObject->mPrimaryKeyCnt > 0 )
    {
        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(sDBCStmt),
                                    STL_SIZEOF(void *) * sRelObject->mPrimaryKeyCnt,
                                    (void **) & sRelObject->mPrimaryKeyIndexPhyHandle,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        for ( i = 0; i < sRelObject->mPrimaryKeyCnt; i++ )
        {
            sIndexHandle =
                ellGetConstraintIndexDictHandle( & sRelObject->mPrimaryKeyHandle[i] );

            sRelObject->mPrimaryKeyIndexPhyHandle[i] = ellGetIndexHandle( sIndexHandle );


            /**
             * @todo Index에 구성된 Key Column List를 Read Column 대상으로 추가
             */
            
            /* STL_TRY( ellAddTableReadColumnByAffectedIndex( aTableDictHandle, */
            /*                                                sIndexHandle, */
            /*                                                aRegionMem, */
            /*                                                sTableReadColumnList, */
            /*                                                aEnv ) */
            /*          == STL_SUCCESS ); */
        }
    }
    
    /**
     * Unique Key 정보 획득
     */

    STL_TRY( ellGetTableUniqueKeyDictHandle( aStmtInfo->mTransID,
                                             aStmtInfo->mSQLStmt->mViewSCN,
                                             QLL_INIT_PLAN(sDBCStmt),
                                             & aInitPlan->mTableHandle,
                                             & sRelObject->mUniqueKeyCnt,
                                             & sRelObject->mUniqueKeyHandle,
                                             ELL_ENV(aEnv) )
             == STL_SUCCESS );

    if ( sRelObject->mUniqueKeyCnt > 0 )
    {
        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(sDBCStmt),
                                    STL_SIZEOF(void *) * sRelObject->mUniqueKeyCnt,
                                    (void **) & sRelObject->mUniqueKeyIndexPhyHandle,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        for ( i = 0; i < sRelObject->mUniqueKeyCnt; i++ )
        {
            sIndexHandle =
                ellGetConstraintIndexDictHandle( & sRelObject->mUniqueKeyHandle[i] );
            sRelObject->mUniqueKeyIndexPhyHandle[i] = ellGetIndexHandle( sIndexHandle );
        }
    }
    
    /**
     * Foreign Key 정보 획득
     * @todo Foreign Key 구현해야 함.
     */

    STL_TRY( ellGetTableForeignKeyDictHandle( aStmtInfo->mTransID,
                                              aStmtInfo->mSQLStmt->mViewSCN,
                                              QLL_INIT_PLAN(sDBCStmt),
                                              & aInitPlan->mTableHandle,
                                              & sRelObject->mForeignKeyCnt,
                                              & sRelObject->mForeignKeyHandle,
                                              ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sRelObject->mForeignKeyCnt == 0, RAMP_ERR_NOT_IMPLEMENTED );

    if ( sRelObject->mForeignKeyCnt > 0 )
    {
        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(sDBCStmt),
                                    STL_SIZEOF(void *) * sRelObject->mForeignKeyCnt,
                                    (void **) & sRelObject->mForeignKeyIndexPhyHandle,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        for ( i = 0; i < sRelObject->mForeignKeyCnt; i++ )
        {
            sIndexHandle =
                ellGetConstraintIndexDictHandle( & sRelObject->mForeignKeyHandle[i] );
            sRelObject->mForeignKeyIndexPhyHandle[i] = ellGetIndexHandle( sIndexHandle );
        }
    }

    STL_TRY_THROW( sRelObject->mForeignKeyCnt == 0, RAMP_ERR_NOT_IMPLEMENTED );


    /**
     * Child Foreign Key 정보 획득
     * @todo Foreign Key 구현해야 함.
     */

    STL_TRY( ellGetTableChildForeignKeyDictHandle( aStmtInfo->mTransID,
                                                   aStmtInfo->mSQLStmt->mViewSCN,
                                                   QLL_INIT_PLAN(sDBCStmt),
                                                   & aInitPlan->mTableHandle,
                                                   & sRelObject->mChildForeignKeyCnt,
                                                   & sRelObject->mChildForeignKeyHandle,
                                                   ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sRelObject->mChildForeignKeyCnt == 0, RAMP_ERR_NOT_IMPLEMENTED );

    if ( sRelObject->mForeignKeyCnt > 0 )
    {
        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(sDBCStmt),
                                    STL_SIZEOF(void *) * sRelObject->mForeignKeyCnt,
                                    (void **) & sRelObject->mForeignKeyIndexPhyHandle,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        for ( i = 0; i < sRelObject->mForeignKeyCnt; i++ )
        {
            sIndexHandle =
                ellGetConstraintIndexDictHandle( & sRelObject->mForeignKeyHandle[i] );
            sRelObject->mForeignKeyIndexPhyHandle[i] = ellGetIndexHandle( sIndexHandle );
        }
    }
    
    /******************************************************
     * Index 정보 획득
     ******************************************************/

    /**
     * Unique Index 정보 획득
     */

    STL_TRY( ellGetTableUniqueIndexDictHandle( aStmtInfo->mTransID,
                                               aStmtInfo->mSQLStmt->mViewSCN,
                                               QLL_INIT_PLAN(sDBCStmt),
                                               & aInitPlan->mTableHandle,
                                               & sRelObject->mUniqueIndexCnt,
                                               & sRelObject->mUniqueIndexHandle,
                                               ELL_ENV(aEnv) )
             == STL_SUCCESS );

    if ( sRelObject->mUniqueIndexCnt > 0 )
    {
        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(sDBCStmt),
                                    STL_SIZEOF(void *) * sRelObject->mUniqueIndexCnt,
                                    (void **) & sRelObject->mUniqueIndexPhyHandle,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        for ( i = 0; i < sRelObject->mUniqueIndexCnt; i++ )
        {
            sRelObject->mUniqueIndexPhyHandle[i]
                = ellGetIndexHandle( & sRelObject->mUniqueIndexHandle[i] );
        }
    }
    
    /**
     * Non-Unique Index 정보 획득
     */

    STL_TRY( ellGetTableNonUniqueIndexDictHandle( aStmtInfo->mTransID,
                                                  aStmtInfo->mSQLStmt->mViewSCN,
                                                  QLL_INIT_PLAN(sDBCStmt),
                                                  & aInitPlan->mTableHandle,
                                                  & sRelObject->mNonUniqueIndexCnt,
                                                  & sRelObject->mNonUniqueIndexHandle,
                                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    if ( sRelObject->mNonUniqueIndexCnt > 0 )
    {
        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(sDBCStmt),
                                    STL_SIZEOF(void *) * sRelObject->mNonUniqueIndexCnt,
                                    (void **) & sRelObject->mNonUniqueIndexPhyHandle,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        for ( i = 0; i < sRelObject->mNonUniqueIndexCnt; i++ )
        {
            sRelObject->mNonUniqueIndexPhyHandle[i]
                = ellGetIndexHandle( & sRelObject->mNonUniqueIndexHandle[i] );
        }
    }

    /******************************************************
     * Deferrable NOT NULL 정보 획득
     ******************************************************/

    STL_TRY( ellGetTableCheckNotNullDictHandle( aStmtInfo->mTransID,
                                                aStmtInfo->mSQLStmt->mViewSCN,
                                                QLL_INIT_PLAN(sDBCStmt),
                                                & aInitPlan->mTableHandle,
                                                & sNotNullCnt,
                                                & sNotNullHandleArray,
                                                ELL_ENV(aEnv) )
             == STL_SUCCESS );

    sRelObject->mDeferNotNullCnt = 0;

    for ( i = 0; i < sNotNullCnt; i++ )
    {
        if ( ellGetConstraintAttrIsDeferrable( & sNotNullHandleArray[i] ) == STL_TRUE )
        {
            sRelObject->mDeferNotNullCnt++;
        }
    }
    
    if ( sRelObject->mDeferNotNullCnt > 0 )
    {
        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(sDBCStmt),
                                    STL_SIZEOF(ellDictHandle) * sRelObject->mDeferNotNullCnt,
                                    (void **) & sRelObject->mDeferNotNullHandle,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sDeferIdx = 0;
        for ( i = 0; i < sNotNullCnt; i++ )
        {
            if ( ellGetConstraintAttrIsDeferrable( & sNotNullHandleArray[i] ) == STL_TRUE )
            {
                stlMemcpy( & sRelObject->mDeferNotNullHandle[sDeferIdx],
                           & sNotNullHandleArray[i],
                           STL_SIZEOF(ellDictHandle) );
                sDeferIdx++;
            }
            else
            {
                /* nothing to do */
            }
        }
    }
    else
    {
        sRelObject->mDeferNotNullHandle = NULL;
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "qlvGetRelObject4Delete()" );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief DELETE 구문에 영향을 받는 SQL Object 를 위한 read Column List를 구축한다.
 * @param[in]  aStmtInfo          Stmt Information
 * @param[in]  aReadTableNode   Table Node for Reading
 * @param[in]  aDeleteNode      Init Delete Node
 * @param[in]  aRelObject       Delete 구문에 영향을 받는 SQL Object 정보
 * @param[in]  aEnv             Environment
 */
stlStatus qlvSetRefColumnListForDelete( qlvStmtInfo             * aStmtInfo,
                                        qlvInitNode             * aReadTableNode,
                                        qlvInitDelete           * aDeleteNode,
                                        qlvInitDeleteRelObject  * aRelObject,
                                        qllEnv                  * aEnv )
{
    qlvInitBaseTableNode    * sReadTableNode   = NULL;
    
    qlvInitDeleteRelObject  * sRelObject       = NULL;
    ellDictHandle           * sHandleArray     = NULL;
    ellDictHandle           * sIndexHandle     = NULL;
    stlInt32                  sLoop            = 0;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aReadTableNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aReadTableNode->mType == QLV_NODE_TYPE_BASE_TABLE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDeleteNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRelObject != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * 기본 정보 얻기
     */

    sReadTableNode   = (qlvInitBaseTableNode *) aReadTableNode;
    sRelObject       = aRelObject;
    

    /**
     * Primary Key 에 필요한 컬럼 추출
     */

    sHandleArray = sRelObject->mPrimaryKeyHandle;
    
    for( sLoop = 0 ; sLoop < sRelObject->mPrimaryKeyCnt ; sLoop++ )
    {
        sIndexHandle = ellGetConstraintIndexDictHandle( & sHandleArray[ sLoop ] );

        /* Index Key Column 정보를 Read Column List 에 추가 */
        STL_TRY( qlvAddRefColumnOnBaseTableByIndex( sReadTableNode,
                                                    sIndexHandle,
                                                    QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                    aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Unique Key 에 필요한 컬럼 추출
     */

    sHandleArray = sRelObject->mUniqueKeyHandle;
    
    for( sLoop = 0 ; sLoop < sRelObject->mUniqueKeyCnt ; sLoop++ )
    {
        sIndexHandle = ellGetConstraintIndexDictHandle( & sHandleArray[ sLoop ] );

        /* Index Key Column 정보를 Read Column List 에 추가 */
        STL_TRY( qlvAddRefColumnOnBaseTableByIndex( sReadTableNode,
                                                    sIndexHandle,
                                                    QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                    aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Foreign Key 에 필요한 컬럼 추출
     */

    sHandleArray = sRelObject->mForeignKeyHandle;
    
    for( sLoop = 0 ; sLoop < sRelObject->mForeignKeyCnt ; sLoop++ )
    {
        sIndexHandle = ellGetConstraintIndexDictHandle( & sHandleArray[ sLoop ] );

        /* Index Key Column 정보를 Read Column List 에 추가 */
        STL_TRY( qlvAddRefColumnOnBaseTableByIndex( sReadTableNode,
                                                    sIndexHandle,
                                                    QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                    aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Unique Index 에 필요한 컬럼 추출
     */

    sHandleArray = sRelObject->mUniqueIndexHandle;

    for( sLoop = 0 ; sLoop < sRelObject->mUniqueIndexCnt ; sLoop++ )
    {
        sIndexHandle = & sHandleArray[ sLoop ];

        /* Index Key Column 정보를 Read Column List 에 추가 */
        STL_TRY( qlvAddRefColumnOnBaseTableByIndex( sReadTableNode,
                                                    sIndexHandle,
                                                    QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                    aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Non-Unique Index 에 필요한 컬럼 추출
     */

    sHandleArray = sRelObject->mNonUniqueIndexHandle;
    
    for( sLoop = 0 ; sLoop < sRelObject->mNonUniqueIndexCnt ; sLoop++ )
    {
        sIndexHandle = & sHandleArray[ sLoop ];

        /* Index Key Column 정보를 Read Column List 에 추가 */
        STL_TRY( qlvAddRefColumnOnBaseTableByIndex( sReadTableNode,
                                                    sIndexHandle,
                                                    QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                    aEnv )
                 == STL_SUCCESS );
    }

    /**
     * Deferrable NOT NULL 에 필요한 컬럼 추출
     */

    sHandleArray = sRelObject->mDeferNotNullHandle;

    for( sLoop = 0 ; sLoop < sRelObject->mDeferNotNullCnt ; sLoop++ )
    {
        STL_TRY( qlvAddRefColumnOnBaseTableByConstraintColumn(
                     sReadTableNode,
                     ellGetCheckNotNullColumnHandle( & sHandleArray[sLoop] ),
                     QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                     aEnv )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlvDelete */
