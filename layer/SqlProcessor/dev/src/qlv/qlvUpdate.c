/*******************************************************************************
 * qlvUpdate.c
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
 * @file qlvUpdate.c
 * @brief SQL Processor Layer UPDATE statement Validation
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @addtogroup qlvUpdate
 * @{
 */

/**
 * @brief UPDATE 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlvValidateUpdate( smlTransId      aTransID,
                             qllDBCStmt    * aDBCStmt,
                             qllStatement  * aSQLStmt,
                             stlChar       * aSQLString,
                             qllNode       * aParseTree,
                             qllEnv        * aEnv )
{
    ellDictHandle        * sAuthHandle   = NULL;

    qlpUpdate            * sParseTree    = NULL;
    qlvInitUpdate        * sInitPlan     = NULL;

    qlpObjectName        * sObjectName   = NULL;
    stlBool                sObjectExist  = STL_FALSE;

    stlChar              * sAliasName    = NULL;

    ellDictHandle          sSchemaHandle;

    qlvRefTableList      * sReadTableList  = NULL;
    qlvRefTableList      * sWriteTableList = NULL;
    qlpBaseTableNode       sTableNode;

    stlInt32               sSetColCnt = 0;
    qlvInitExpression    * sInitExpr = NULL;
    qlvRefExprItem       * sListItem = NULL;        
    ellDictHandle        * sColumnHandle = NULL;
        
    qlvInitStmtExprList  * sStmtExprList  = NULL;
    qlvInitExprList      * sQueryExprList = NULL;
    qlvStmtInfo            sStmtInfo;

    stlInt32               sCnt = 0;
    stlInt32               sTableCnt = 0;

    qlpListElement      * sSetElement   = NULL;
    qllNode             * sNode         = NULL;

    stlInt32              sQBIndex;

    ellInitDictHandle( &sSchemaHandle );
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_UPDATE_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_UPDATE_WHERE_CURRENT_OF_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_UPDATE_RETURNING_QUERY_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_UPDATE_RETURNING_INTO_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aParseTree->mType == QLL_STMT_UPDATE_TYPE) ||
                        (aParseTree->mType == QLL_STMT_UPDATE_WHERE_CURRENT_OF_TYPE) ||
                        (aParseTree->mType == QLL_STMT_UPDATE_RETURNING_QUERY_TYPE) ||
                        (aParseTree->mType == QLL_STMT_UPDATE_RETURNING_INTO_TYPE),
                        QLL_ERROR_STACK(aEnv) );

    /**********************************************************
     * 준비 작업
     **********************************************************/

    /**
     * 기본 정보 획득
     */
    
    sParseTree  = (qlpUpdate *) aParseTree;
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV( aEnv ) );

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlvInitUpdate ),
                                (void **) & sInitPlan,
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF( qlvInitUpdate ) );

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
    stlStrcpy( sStmtInfo.mQBPrefix, QLV_QUERY_BLOCK_NAME_PREFIX_UPD );
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
     * Table Alias 정보
     */
    
    sAliasName = ( sParseTree->mRelation->mAlias == NULL )
        ? NULL : QLP_GET_PTR_VALUE( sParseTree->mRelation->mAlias->mAliasName );


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
    
    /**********************************************************
     * 기본 정보 초기화 
     **********************************************************/

    /**
     * Table 의 Total Column 정보 획득 
     */
    
    sInitPlan->mTotalColumnCnt = ellGetTableColumnCnt( & sInitPlan->mTableHandle );

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aDBCStmt ),
                                STL_SIZEOF(stlInt32) * sInitPlan->mTotalColumnCnt,
                                (void **) & sInitPlan->mSetCodeStackIdx,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan->mSetCodeStackIdx,
               0xFF, /* -1로 초기화 idx는 0부터 양수로 증가하므로 used 체크시 -1은 unused임. */
               STL_SIZEOF(stlInt32) * sInitPlan->mTotalColumnCnt );


    /**********************************************************
     * Reference Table List 구성
     **********************************************************/

    /**
     * Base Table Node 생성
     */

    sTableNode.mType    = QLL_BASE_TABLE_NODE_TYPE;
    sTableNode.mNodePos = sParseTree->mRelation->mNodePos;
    sTableNode.mName    = sParseTree->mRelation->mName;
    sTableNode.mDumpOpt = NULL;
    sTableNode.mAlias   = sParseTree->mRelation->mAlias;


    /**
     * Read Reference Table List 생성
     */

    STL_TRY( qlvCreateRefTableList( & QLL_VALIDATE_MEM(aEnv),
                                    NULL,
                                    & sReadTableList,
                                    aEnv )
             == STL_SUCCESS );

    /**
     * Read Reference Table List 구성
     */

    sTableCnt = 0;
    STL_TRY( qlvTableNodeValidation( & sStmtInfo,
                                     sInitPlan->mInitPlan.mValidationObjList,
                                     sReadTableList,
                                     (qllNode *) & sTableNode,
                                     &(sInitPlan->mReadTableNode),
                                     &sTableCnt,
                                     aEnv )
             == STL_SUCCESS );

    STL_DASSERT( sTableCnt == 1 );

    STL_TRY( qlvSetNodeToRefTableItem( sReadTableList,
                                       sInitPlan->mReadTableNode,
                                       aEnv )
             == STL_SUCCESS );
    
    /**
     * Write Reference Table List 생성
     */

    STL_TRY( qlvCreateRefTableList( & QLL_VALIDATE_MEM(aEnv),
                                    NULL,
                                    & sWriteTableList,
                                    aEnv )
             == STL_SUCCESS );
    
    /**
     * Write Base Table Node 생성
     */

    sTableCnt = 0;
    STL_TRY( qlvTableNodeValidation( & sStmtInfo,
                                     sInitPlan->mInitPlan.mValidationObjList,
                                     sWriteTableList,
                                     (qllNode *) & sTableNode,
                                     &(sInitPlan->mWriteTableNode),
                                     &sTableCnt,
                                     aEnv )
             == STL_SUCCESS );

    STL_DASSERT( sTableCnt == 1 );

    STL_TRY( qlvSetNodeToRefTableItem( sWriteTableList,
                                       sInitPlan->mWriteTableNode,
                                       aEnv )
             == STL_SUCCESS );


    /**********************************************************
     * UPDATE SET Validation
     **********************************************************/

    /**
     * SET 절 정보 초기화 
     */

    sInitPlan->mSetColumnCnt = 0;
    QLP_LIST_FOR_EACH( sParseTree->mSet, sSetElement )
    {
        sNode  = (qllNode*)QLP_LIST_GET_POINTER_VALUE( sSetElement );

        if( sNode->mType == QLL_TARGET_TYPE )
        {
            sInitPlan->mSetColumnCnt++;
        }
        else
        {
            sInitPlan->mSetColumnCnt += ((qlpRowTarget*)sNode)->mSetColumnList->mCount;
        }
    }

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(ellDictHandle) * sInitPlan->mSetColumnCnt,
                                (void **) & sInitPlan->mSetColumnHandle,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan->mSetColumnHandle,
               0x00,
               STL_SIZEOF(ellDictHandle) * sInitPlan->mSetColumnCnt );

    STL_TRY( knlAllocRegionMem(
                 QLL_INIT_PLAN(aDBCStmt),
                 STL_SIZEOF(qlvInitExpression *) * sInitPlan->mSetColumnCnt,
                 (void **) & sInitPlan->mSetExpr,
                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan->mSetExpr,
               0x00,
               STL_SIZEOF(qlvInitExpression *) * sInitPlan->mSetColumnCnt );


    /**
     * Set Column List 정보 초기화 
     */
    
    STL_TRY( qlvCreateRefExprList( & sInitPlan->mSetColExprList,
                                   QLL_INIT_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );


    /**
     * Update Set Expression 을 검증
     */

    STL_TRY( qlvValidateUpdateSet( & sStmtInfo,
                                   sInitPlan,
                                   sReadTableList,
                                   sWriteTableList,
                                   sAliasName,
                                   sParseTree->mSet,
                                   aEnv )
             == STL_SUCCESS );

    /**
     * UPDATE ON TABLE 권한 검사
     */

    STL_TRY( qlaCheckTablePriv( aTransID,
                                aDBCStmt,
                                aSQLStmt,
                                ELL_TABLE_PRIV_ACTION_UPDATE,
                                & sInitPlan->mTableHandle,
                                sInitPlan->mSetColExprList,
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
                                       QLL_INIT_PLAN(aDBCStmt),
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
                                       QLL_INIT_PLAN(aDBCStmt),
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
                                       QLL_INIT_PLAN(aDBCStmt),
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
     * UPDATE 에 영향을 받는 SQL-Object 정보 획득 
     **********************************************************/

    STL_TRY( qlvGetRelObject4Update( & sStmtInfo,
                                     sInitPlan,
                                     aEnv )
             == STL_SUCCESS );

    /**********************************************************
     * UPDATE 에 영향을 받는 SQL-Object 가 참조하는 Column List 구성
     **********************************************************/
    
    STL_TRY( qlvSetRefColumnListForUpdate( & sStmtInfo,
                                           sInitPlan->mReadTableNode,
                                           sInitPlan->mWriteTableNode,
                                           sInitPlan,
                                           & sInitPlan->mRelObject,
                                           aEnv )
             == STL_SUCCESS );


    if ( (sInitPlan->mRelObject.mAffectCheckNotNullCnt > 0) ||
         (sInitPlan->mRelObject.mAffectCheckClauseCnt > 0) ||
         (sInitPlan->mRelObject.mHasAffectIdentity == STL_TRUE) )
    {
        sInitPlan->mHasCheck = STL_TRUE;
    }
    else
    {
        sInitPlan->mHasCheck = STL_FALSE;
    }
    
    
    if ( (sInitPlan->mRelObject.mAffectPrimaryKeyCnt > 0) ||
         (sInitPlan->mRelObject.mAffectUniqueKeyCnt > 0) ||
         (sInitPlan->mRelObject.mAffectForeignKeyCnt > 0) ||
         (sInitPlan->mRelObject.mAffectUniqueIndexCnt > 0) ||
         (sInitPlan->mRelObject.mAffectNonUniqueIndexCnt > 0) )
    {
        sInitPlan->mHasIndex = STL_TRUE;
    }
    else
    {
        sInitPlan->mHasIndex = STL_FALSE;
    }


    
    /**********************************************************
     * RETURNING 절 Validation
     **********************************************************/

    STL_TRY( qlvCreateRefExprList( & sInitPlan->mShareReadColList,
                                   QLL_INIT_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sInitPlan->mShareWriteColList,
                                   QLL_INIT_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );
    
    if( sParseTree->mReturnTarget != NULL )
    {
        sInitPlan->mIsReturnNew = sParseTree->mIsReturnNew;

        if( sParseTree->mIsReturnNew == STL_TRUE )
        {
            STL_TRY( qlvValidateReturningPhrase( & sStmtInfo,
                                                 sInitPlan->mInitPlan.mValidationObjList,
                                                 sInitPlan->mWriteTableNode,
                                                 sParseTree->mReturnTarget,
                                                 & sInitPlan->mReturnTargetCnt,
                                                 & sInitPlan->mReturnTargetArray,
                                                 aEnv )
                     == STL_SUCCESS );

            /* SET 대상이 아닌 Write Column Expression 분류 */
            sListItem = ((qlvInitBaseTableNode *) sInitPlan->mWriteTableNode)->mRefColumnList->mHead;
            while( sListItem != NULL )
            {
                sInitExpr = sListItem->mExprPtr;
                sColumnHandle = sInitExpr->mExpr.mColumn->mColumnHandle;
                for( sSetColCnt = 0 ; sSetColCnt < sInitPlan->mSetColumnCnt ; sSetColCnt++ )
                {
                    if( ellGetColumnIdx( sColumnHandle ) ==
                        ellGetColumnIdx( & sInitPlan->mSetColumnHandle[ sSetColCnt ] ) )
                    {
                        break;
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }

                if( sSetColCnt == sInitPlan->mSetColumnCnt )
                {
                    /* SET 대상이 아닌 Write Column Expression */
                    /* Read Column List에 추가 */
                    if( sInitExpr != NULL )
                    {
                        /* Share Write Column List에 Write Column 추가 */
                        STL_TRY( qlvAddExprToRefColExprList( sInitPlan->mShareWriteColList,
                                                             sInitExpr,
                                                             QLL_INIT_PLAN(aDBCStmt),
                                                             aEnv )
                                 == STL_SUCCESS );

                        /* Read Column List에 Write Column 추가 */
                        STL_TRY( qlvCopyExpr( sInitExpr,
                                              & sInitExpr,
                                              QLL_INIT_PLAN(aDBCStmt),
                                              aEnv )
                                 == STL_SUCCESS );

                        sInitExpr->mExpr.mColumn->mRelationNode = sInitPlan->mReadTableNode;
                        
                        STL_TRY( qlvAddExprToRefColExprList(
                                     ((qlvInitBaseTableNode *) sInitPlan->mReadTableNode)->mRefColumnList,
                                     sInitExpr,
                                     QLL_INIT_PLAN(aDBCStmt),
                                     aEnv )
                                 == STL_SUCCESS );

                        /* Share Read Column List에 Read Column 추가 */
                        STL_TRY( qlvAddExprToRefColExprList( sInitPlan->mShareReadColList,
                                                             sInitExpr,
                                                             QLL_INIT_PLAN(aDBCStmt),
                                                             aEnv )
                                 == STL_SUCCESS );
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }
                else
                {
                    /* SET 대상인 Write Column Expression */
                    /* Do Nothing */
                }

                sListItem = sListItem->mNext;
            }

            if( ((qlvInitBaseTableNode *) sInitPlan->mWriteTableNode)->mRefRowIdColumn != NULL )
            {
                if( ((qlvInitBaseTableNode *) sInitPlan->mReadTableNode)->mRefRowIdColumn != NULL )
                {
                    /**
                     * Read / Write RowId Column이 중복으로 처리됨을 방지하기 위해
                     * RETURNING NEW 에 쓰인 ROWID column 들의 relation node를 ReadTableNode로 변경한다.
                     * 
                     * 예) UPDAE T1 SET I1 = 1
                     *     WHERE ROWID = 'AAAA24AAAAAAACAAAAAiAAK' 
                     *           -----                             
                     *     RETURNING NEW ROWID INTO :V1;
                     *               ---------
                     */
                    
                    for( sCnt = 0; sCnt < sInitPlan->mReturnTargetCnt; sCnt++ )
                    {
                        STL_TRY( qlvChangeRelationNodeForRowIdCol(
                                     sInitPlan->mWriteTableNode,
                                     sInitPlan->mReadTableNode,
                                     sInitPlan->mReturnTargetArray[ sCnt ].mExpr,
                                     aEnv )
                                 == STL_SUCCESS );
                    }
                }
                else
                {
                    /**
                     * 예) UPDAE T1 SET I1 = 1
                     *     WHERE I2 = 3
                     *     RETURNING NEW ROWID INTO :V1;
                     *               ---------
                     */
                    
                    /* Do Nothing */
                }
            }
            else
            {
                /* RETURNING NEW 에 ROWID column이 쓰이지 않은 경우 */
                
                /* Do Nothing */
            }
        }
        else
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

        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
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

    if( aSQLStmt->mStmtType == QLL_STMT_UPDATE_RETURNING_QUERY_TYPE )
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

    /* STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED_HINT ) */
    /* { */
    /*     stlPushError( STL_ERROR_LEVEL_ABORT, */
    /*                   QLL_ERRCODE_NOT_IMPLEMENTED, */
    /*                   qlgMakeErrPosString( aSQLString, */
    /*                                        sHint->mNodePos, */
    /*                                        aEnv ), */
    /*                   QLL_ERROR_STACK(aEnv), */
    /*                   "qlvValidateUpdate()" ); */
    /* } */

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
 * @brief UPDATE 구문의 SET 절을 Validation 한다.
 * @param[in] aStmtInfo       Stmt Information
 * @param[in] aInitPlan       UPDATE Init Plan
 * @param[in] aReadTableList  Reference Table List for Reading
 * @param[in] aWriteTableList Reference Table List for Writing
 * @param[in] aUpdateSet      SET 절 Parse Tree
 * @param[in] aTableAliasName Table Alias Name 
 * @param[in] aEnv            Environment
 */
stlStatus qlvValidateUpdateSet( qlvStmtInfo     * aStmtInfo,
                                qlvInitUpdate   * aInitPlan,
                                qlvRefTableList * aReadTableList,
                                qlvRefTableList * aWriteTableList,
                                stlChar         * aTableAliasName,
                                qlpList         * aUpdateSet,
                                qllEnv          * aEnv )
{
    qlpListElement      * sSetElement   = NULL;
    qlpListElement      * sColumnElem   = NULL;
    qllNode             * sNode         = NULL;
    qlpTarget           * sSetClause    = NULL;
    qllNode             * sSetExpr      = NULL;

    qlpRowTarget        * sRowTarget    = NULL;
    qlvInitExpression   * sValueExpr    = NULL;

    qlpColumnRef        * sColumnRefNode;
    qlvInitExpression   * sSetColExpr   = NULL;
    
    stlChar             * sColumnName   = NULL;
    stlInt32              sColumnIdx    = 0;

    stlBool               sObjectExist  = STL_FALSE;
    stlBool               sIsUpdatable  = STL_FALSE;

    stlInt32              i = 0;
    stlInt32              sIdx;
    stlInt32              sErrPos;

    /*
     * Paramenter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aReadTableList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aWriteTableList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aUpdateSet != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Update Set 절을 Validation 한다.
     */

    i = 0;
    QLP_LIST_FOR_EACH( aUpdateSet, sSetElement )
    {
        sNode  = (qllNode*)QLP_LIST_GET_POINTER_VALUE( sSetElement );

        if( sNode->mType == QLL_TARGET_TYPE )
        {
            /* Single Set Column */
            sSetClause  = (qlpTarget*)sNode;

            sColumnRefNode = sSetClause->mUpdateColumn;
            sColumnName    = sColumnRefNode->mName->mColumn;

            sSetExpr    = sSetClause->mExpr;

            /**
             * SET Column Expression을 list로 구성
             */

            /**
             * 유효한 Column Name 인지 검사
             */

            sIsUpdatable = STL_TRUE;
            STL_TRY( qlvValidateColumnRef( aStmtInfo,
                                           QLV_EXPR_PHRASE_TARGET,
                                           aInitPlan->mInitPlan.mValidationObjList,
                                           aWriteTableList,
                                           STL_FALSE,
                                           0, /* Allowed Aggregation Depth */
                                           sColumnRefNode,
                                           & sSetColExpr,
                                           NULL,
                                           & sIsUpdatable,
                                           KNL_BUILTIN_FUNC_INVALID,
                                           DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                           DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                           QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                           aEnv )
                     == STL_SUCCESS );

            STL_TRY( ellGetColumnDictHandle( aStmtInfo->mTransID,
                                             aStmtInfo->mSQLStmt->mViewSCN,
                                             & aInitPlan->mTableHandle,
                                             sColumnName,
                                             & aInitPlan->mSetColumnHandle[i],
                                             & sObjectExist,
                                             ELL_ENV(aEnv) )
                     == STL_SUCCESS );

            STL_DASSERT( sObjectExist == STL_TRUE );

            /**
             * 동일한 Column 이 사용되는지 검사
             */

            sColumnIdx = ellGetColumnIdx( & aInitPlan->mSetColumnHandle[i] );
            /* mSetCodeStackIdx의 초기값(unused)은 -1로 되어 있음 */
            STL_TRY_THROW( aInitPlan->mSetCodeStackIdx[sColumnIdx] < 0,
                           RAMP_ERR_DUPLICATE_COLUMN_NAME );
            aInitPlan->mSetCodeStackIdx[sColumnIdx] = i;

            /* add expr to expr list */
            STL_TRY( qlvAddExprToRefExprList( aInitPlan->mSetColExprList,
                                              sSetColExpr,
                                              STL_FALSE,
                                              QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                              aEnv )
                     == STL_SUCCESS );


            /**
             * Set Expression 을 Validation
             */

            if ( ((qlpValueExpr *) sSetExpr)->mExpr->mType == QLL_DEFAULT_TYPE )
            {
                /**
                 * DEFAULT expression 인 경우
                 */

                STL_TRY( qlvValidateDefaultBNF( aStmtInfo,
                                                aInitPlan->mInitPlan.mValidationObjList,
                                                sSetExpr,
                                                & aInitPlan->mSetColumnHandle[i],
                                                & aInitPlan->mSetExpr[i],
                                                KNL_BUILTIN_FUNC_INVALID,
                                                DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                                QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                /**
                 * Value Expression 을 검증
                 */

                STL_TRY( qlvValidateValueExpr( aStmtInfo,
                                               QLV_EXPR_PHRASE_TARGET,
                                               aInitPlan->mInitPlan.mValidationObjList,
                                               aReadTableList,
                                               STL_FALSE,  /* Atomic */
                                               STL_TRUE,  /* Row Expr */
                                               0, /* Allowed Aggregation Depth */
                                               sSetExpr,
                                               & aInitPlan->mSetExpr[i],
                                               NULL,
                                               KNL_BUILTIN_FUNC_INVALID,
                                               DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                               DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                               QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                               aEnv )
                         == STL_SUCCESS );

                if( aInitPlan->mSetExpr[i]->mType == QLV_EXPR_TYPE_SUB_QUERY )
                {
                    STL_DASSERT( aInitPlan->mSetExpr[i]->mExpr.mSubQuery->mTargetCount > 0 );
                    if( aInitPlan->mSetExpr[i]->mExpr.mSubQuery->mTargetCount > 1 )
                    {
                        sErrPos = sSetExpr->mNodePos;
                        STL_THROW( RAMP_ERR_TOO_MANY_VALUES );
                    }
                }
            }

            /**
             * Rewrite Expression
             */

            STL_TRY( qlvRewriteExpr( aInitPlan->mSetExpr[i],
                                     DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                     aStmtInfo->mQueryExprList,
                                     & aInitPlan->mSetExpr[i],
                                     QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                     aEnv )
                     == STL_SUCCESS );

            i++;
        }
        else
        {
            /* Row SubQuery Expression을 갖는 Multiple Set Column */
            STL_DASSERT( sNode->mType == QLL_ROW_TARGET_TYPE );
            sRowTarget = (qlpRowTarget*)sNode;
            STL_DASSERT( sRowTarget->mSetColumnList->mCount > 1 );


            /**
             * Set Expression 을 Validation
             */

            if( ((qlpValueExpr*)(sRowTarget->mValueExpr))->mExpr->mType == QLL_DEFAULT_TYPE )
            {
                sErrPos = sRowTarget->mValueExpr->mNodePos;

                STL_THROW( RAMP_ERR_NOT_ENOUGH_VALUES );
            }

            /**
             * Value Expression 을 검증
             */

            STL_TRY( qlvValidateValueExpr( aStmtInfo,
                                           QLV_EXPR_PHRASE_TARGET,
                                           aInitPlan->mInitPlan.mValidationObjList,
                                           aReadTableList,
                                           STL_FALSE,  /* Atomic */
                                           STL_TRUE,  /* Row Expr */
                                           0, /* Allowed Aggregation Depth */
                                           sRowTarget->mValueExpr,
                                           &sValueExpr,
                                           NULL,
                                           KNL_BUILTIN_FUNC_INVALID,
                                           DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                           DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                           QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                           aEnv )
                     == STL_SUCCESS );

            /* Expression이 SubQuery이어야 하고,
             * Target Count가 Set Column List의 Count와 동일해야 한다. */
            if( sValueExpr->mType != QLV_EXPR_TYPE_SUB_QUERY )
            {
                sErrPos = sRowTarget->mValueExpr->mNodePos;

                STL_THROW( RAMP_ERR_NOT_ENOUGH_VALUES );
            }

            if( sValueExpr->mExpr.mSubQuery->mTargetCount !=
                sRowTarget->mSetColumnList->mCount )
            {
                sErrPos = sRowTarget->mValueExpr->mNodePos;

                if( sValueExpr->mExpr.mSubQuery->mTargetCount <
                    sRowTarget->mSetColumnList->mCount )
                {
                    STL_THROW( RAMP_ERR_NOT_ENOUGH_VALUES );
                }
                else
                {
                    STL_DASSERT( sValueExpr->mExpr.mSubQuery->mTargetCount >
                                 sRowTarget->mSetColumnList->mCount );
                    STL_THROW( RAMP_ERR_TOO_MANY_VALUES );
                }
            }

            /**
             * Rewrite Expression
             */

            STL_TRY( qlvRewriteExpr( sValueExpr,
                                     DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                     aStmtInfo->mQueryExprList,
                                     &sValueExpr,
                                     QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                     aEnv )
                     == STL_SUCCESS );

            sIdx = 0;
            sColumnElem = QLP_LIST_GET_FIRST( sRowTarget->mSetColumnList );
            while( sColumnElem != NULL )
            {
                sColumnRefNode = (qlpColumnRef*)QLP_LIST_GET_POINTER_VALUE( sColumnElem );
                sColumnName    = sColumnRefNode->mName->mColumn;

                /**
                 * 유효한 Column Name 인지 검사
                 */

                sIsUpdatable = STL_TRUE;
                STL_TRY( qlvValidateColumnRef( aStmtInfo,
                                               QLV_EXPR_PHRASE_TARGET,
                                               aInitPlan->mInitPlan.mValidationObjList,
                                               aWriteTableList,
                                               STL_FALSE,
                                               0, /* Allowed Aggregation Depth */
                                               sColumnRefNode,
                                               & sSetColExpr,
                                               NULL,
                                               & sIsUpdatable,
                                               KNL_BUILTIN_FUNC_INVALID,
                                               DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                               DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                               QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                               aEnv )
                         == STL_SUCCESS );

                STL_TRY( ellGetColumnDictHandle( aStmtInfo->mTransID,
                                                 aStmtInfo->mSQLStmt->mViewSCN,
                                                 & aInitPlan->mTableHandle,
                                                 sColumnName,
                                                 & aInitPlan->mSetColumnHandle[i],
                                                 & sObjectExist,
                                                 ELL_ENV(aEnv) )
                         == STL_SUCCESS );

                STL_DASSERT( sObjectExist == STL_TRUE );
                
                /**
                 * 동일한 Column 이 사용되는지 검사
                 */

                sColumnIdx = ellGetColumnIdx( & aInitPlan->mSetColumnHandle[i] );
                /* mSetCodeStackIdx의 초기값(unused)은 -1로 되어 있음 */
                STL_TRY_THROW( aInitPlan->mSetCodeStackIdx[sColumnIdx] < 0,
                               RAMP_ERR_DUPLICATE_COLUMN_NAME );
                aInitPlan->mSetCodeStackIdx[sColumnIdx] = i;

                /* add expr to expr list */
                STL_TRY( qlvAddExprToRefExprList( aInitPlan->mSetColExprList,
                                                  sSetColExpr,
                                                  STL_FALSE,
                                                  QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                  aEnv )
                         == STL_SUCCESS );

                if( sIdx == 0 )
                {
                    aInitPlan->mSetExpr[i] = sValueExpr;
                }
                else
                {
                    STL_TRY( qlvCopyExpr( sValueExpr,
                                          &(aInitPlan->mSetExpr[i]),
                                          QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                          aEnv )
                             == STL_SUCCESS );
                }
                aInitPlan->mSetExpr[i]->mExpr.mSubQuery->mRefIdx = sIdx;

                i++;
                sIdx++;
                sColumnElem = QLP_LIST_GET_NEXT_ELEM( sColumnElem );
            }

            sErrPos = sRowTarget->mValueExpr->mNodePos;
        }
    }


    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_DUPLICATE_COLUMN_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DUPLICATE_COLUMN_NAME,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sSetClause->mNamePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
 
    STL_CATCH( RAMP_ERR_NOT_ENOUGH_VALUES )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_ENOUGH_VALUES,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sErrPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_TOO_MANY_VALUES )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TOO_MANY_VALUES,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sErrPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief UPDATE 구문에 영향을 받는 SQL Object 정보를 획득한다.
 * @param[in]  aStmtInfo       Stmt Information
 * @param[in]  aInitPlan     DELETE Init Plan
 * @param[in]  aEnv          Environment
 */
stlStatus qlvGetRelObject4Update( qlvStmtInfo   * aStmtInfo,
                                  qlvInitUpdate * aInitPlan,
                                  qllEnv        * aEnv )
{
    qlvInitUpdateRelObject * sRelObject   = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );

    /******************************************************
     * Key Constraint 정보 획득
     * - key constraint 를 위한 Index 정보도 key handle 에 포함되어 있음 
     ******************************************************/

    sRelObject = & aInitPlan->mRelObject;
    
    /**
     * Primary key 정보 획득
     */

    STL_TRY( ellGetTablePrimaryKeyDictHandle( aStmtInfo->mTransID,
                                              aStmtInfo->mSQLStmt->mViewSCN,
                                              QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                              & aInitPlan->mTableHandle,
                                              & sRelObject->mTotalPrimaryKeyCnt,
                                              & sRelObject->mTotalPrimaryKeyHandle,
                                              ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Unique Key 정보 획득
     */

    STL_TRY( ellGetTableUniqueKeyDictHandle( aStmtInfo->mTransID,
                                             aStmtInfo->mSQLStmt->mViewSCN,
                                             QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                             & aInitPlan->mTableHandle,
                                             & sRelObject->mTotalUniqueKeyCnt,
                                             & sRelObject->mTotalUniqueKeyHandle,
                                             ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Foreign Key 정보 획득
     * @todo Foreign Key 구현해야 함.
     */

    STL_TRY( ellGetTableForeignKeyDictHandle( aStmtInfo->mTransID,
                                              aStmtInfo->mSQLStmt->mViewSCN,
                                              QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                              & aInitPlan->mTableHandle,
                                              & sRelObject->mTotalForeignKeyCnt,
                                              & sRelObject->mTotalForeignKeyHandle,
                                              ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sRelObject->mTotalForeignKeyCnt == 0, RAMP_ERR_NOT_IMPLEMENTED );

    /**
     * Child Foreign Key 정보 획득
     * @todo Foreign Key 구현해야 함.
     */

    STL_TRY( ellGetTableChildForeignKeyDictHandle( aStmtInfo->mTransID,
                                                   aStmtInfo->mSQLStmt->mViewSCN,
                                                   QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                   & aInitPlan->mTableHandle,
                                                   & sRelObject->mTotalChildForeignKeyCnt,
                                                   & sRelObject->mTotalChildForeignKeyHandle,
                                                   ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sRelObject->mTotalChildForeignKeyCnt == 0, RAMP_ERR_NOT_IMPLEMENTED );
    
    /******************************************************
     * Index 정보 획득
     ******************************************************/

    /**
     * Unique Index 정보 획득
     */

    STL_TRY( ellGetTableUniqueIndexDictHandle( aStmtInfo->mTransID,
                                               aStmtInfo->mSQLStmt->mViewSCN,
                                               QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                               & aInitPlan->mTableHandle,
                                               & sRelObject->mTotalUniqueIndexCnt,
                                               & sRelObject->mTotalUniqueIndexHandle,
                                               ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Non-Unique Index 정보 획득
     */

    STL_TRY( ellGetTableNonUniqueIndexDictHandle( aStmtInfo->mTransID,
                                                  aStmtInfo->mSQLStmt->mViewSCN,
                                                  QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                  & aInitPlan->mTableHandle,
                                                  & sRelObject->mTotalNonUniqueIndexCnt,
                                                  & sRelObject->mTotalNonUniqueIndexHandle,
                                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /******************************************************
     * Check Constraint 정보 획득 
     ******************************************************/
    
    /**
     * Check Not Null Constraint 정보 획득
     */

    STL_TRY( ellGetTableCheckNotNullDictHandle( aStmtInfo->mTransID,
                                                aStmtInfo->mSQLStmt->mViewSCN,
                                                QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                & aInitPlan->mTableHandle,
                                                & sRelObject->mTotalCheckNotNullCnt,
                                                & sRelObject->mTotalCheckNotNullHandle,
                                                ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Check Clause Constraint 정보 획득
     * @todo Check Clause Constraint 를 구현해야 함.
     */

    STL_TRY( ellGetTableCheckClauseDictHandle( aStmtInfo->mTransID,
                                               aStmtInfo->mSQLStmt->mViewSCN,
                                               QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                               & aInitPlan->mTableHandle,
                                               & sRelObject->mTotalCheckClauseCnt,
                                               & sRelObject->mTotalCheckClauseHandle,
                                               ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sRelObject->mTotalCheckClauseCnt == 0, RAMP_ERR_NOT_IMPLEMENTED );
                                              
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "qlvGetRelObject4Update()" );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief UPDATE 구문에 영향을 받는 SQL Object 를 위한 read & write Column List를 구축한다.
 * @param[in]  aStmtInfo        Stmt Information
 * @param[in]  aReadTableNode   Table Node for Reading
 * @param[in]  aWriteTableNode  Table Node for Writing
 * @param[in]  aUpdateNode      Init Update Node
 * @param[in]  aRelObject       Update 구문에 영향을 받는 SQL Object 정보
 * @param[in]  aEnv             Environment
 */
stlStatus qlvSetRefColumnListForUpdate( qlvStmtInfo             * aStmtInfo,
                                        qlvInitNode             * aReadTableNode,
                                        qlvInitNode             * aWriteTableNode,
                                        qlvInitUpdate           * aUpdateNode,
                                        qlvInitUpdateRelObject  * aRelObject,
                                        qllEnv                  * aEnv )
{
    qlvInitBaseTableNode    * sReadTableNode   = NULL;
    qlvInitBaseTableNode    * sWriteTableNode  = NULL;
    ellDictHandle           * sTableHandle     = NULL;
    ellDictHandle           * sColumnHandle    = NULL;
    qlvRefExprList          * sWriteColumnList = NULL;
    
    qlvInitUpdateRelObject  * sRelObject       = NULL;
    ellDictHandle           * sHandleArray     = NULL;
    ellDictHandle           * sIndexHandle     = NULL;
    stlInt32                  sLoop            = 0;
    stlInt32                  sColCnt          = 0;
    stlBool                   sAddedReadCols   = STL_FALSE;

    stlBool  sIsAffected = STL_FALSE;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aReadTableNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aReadTableNode->mType == QLV_NODE_TYPE_BASE_TABLE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aWriteTableNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aWriteTableNode->mType == QLV_NODE_TYPE_BASE_TABLE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aUpdateNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRelObject != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * 기본 정보 얻기
     */

    sReadTableNode   = (qlvInitBaseTableNode *) aReadTableNode;
    sWriteTableNode  = (qlvInitBaseTableNode *) aWriteTableNode;

    sTableHandle     = & sReadTableNode->mTableHandle;

    sWriteColumnList = sWriteTableNode->mRefColumnList;

    sRelObject       = aRelObject;

    /****************************************************************************
     * PRIMARY KEY constraint
     ****************************************************************************/

    /**
     * 공간 할당
     */

    if( sRelObject->mTotalPrimaryKeyCnt > 0 )
    {
        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                    STL_SIZEOF(ellDictHandle) * sRelObject->mTotalPrimaryKeyCnt,
                                    (void **) & sRelObject->mAffectPrimaryKeyHandle,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlMemset( sRelObject->mAffectPrimaryKeyHandle,
                   0x00,
                   STL_SIZEOF(ellDictHandle) * sRelObject->mTotalPrimaryKeyCnt );

        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                    STL_SIZEOF(void *) * sRelObject->mTotalPrimaryKeyCnt,
                                    (void **) & sRelObject->mAffectPrimaryKeyIndexPhyHandle,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlMemset( sRelObject->mAffectPrimaryKeyIndexPhyHandle,
                   0x00,
                   STL_SIZEOF(void *) * sRelObject->mTotalPrimaryKeyCnt );
    }
    else
    {
        sRelObject->mAffectPrimaryKeyCnt = 0;
        sRelObject->mAffectPrimaryKeyHandle = NULL;
        sRelObject->mAffectPrimaryKeyIndexPhyHandle = NULL;
    }

    /**
     * UPDATE 컬럼의 영향을 받는 PRIMARY KEY 선별
     */
    
    sRelObject->mAffectPrimaryKeyCnt = 0;
    sHandleArray = sRelObject->mTotalPrimaryKeyHandle;
    
    for( sLoop = 0 ; sLoop < sRelObject->mTotalPrimaryKeyCnt ; sLoop++ )
    {
        /**
         * UPDATE Column 과 관련 있는 인덱스인지 검사
         */

        sAddedReadCols = STL_FALSE;
        sIndexHandle = ellGetConstraintIndexDictHandle( & sHandleArray[ sLoop ] );

        /* Write 대상이 되는 column list와 관련된 Index Key Column이 존재하는지 검사 */
        STL_TRY( qlvIsAffectedIndexByColumnList( sIndexHandle,
                                                 sWriteColumnList,
                                                 & sIsAffected,
                                                 aEnv )
                 == STL_SUCCESS );

        if( sIsAffected == STL_TRUE )
        {
            /* Index Key Column 정보를 Read Column List 에 추가 */
            STL_TRY( qlvAddRefColumnOnBaseTableByIndex( sReadTableNode,
                                                        sIndexHandle,
                                                        QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                        aEnv )
                     == STL_SUCCESS );
            
            sAddedReadCols = STL_TRUE;

            stlMemcpy( & sRelObject->mAffectPrimaryKeyHandle[sRelObject->mAffectPrimaryKeyCnt],
                       & sRelObject->mTotalPrimaryKeyHandle[sLoop],
                       STL_SIZEOF(ellDictHandle) );

            sRelObject->mAffectPrimaryKeyIndexPhyHandle[sRelObject->mAffectPrimaryKeyCnt]
                = ellGetIndexHandle( sIndexHandle );

            sRelObject->mAffectPrimaryKeyCnt++;
        }
        else
        {
            /* UPDATE Column 에 영향을 받지 않는 인덱스 */
        }

        
        /**
         * Supplemental Logging 적용 여부 
         */

        if( ellGetTableNeedSupplePK( sTableHandle, ELL_ENV( aEnv ) ) == STL_TRUE )
        {
            if( sAddedReadCols == STL_FALSE )
            {
                STL_TRY( qlvAddRefColumnOnBaseTableByIndex( sReadTableNode,
                                                            sIndexHandle,
                                                            QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                            aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                /* Do Nothing */
            }
        }
        else
        {
            /* Nothing To Do */
        }
    }

    /****************************************************************************
     * UNIQUE constraint
     ****************************************************************************/

    /**
     * 공간 할당
     */
    
    if( sRelObject->mTotalUniqueKeyCnt > 0 )
    {
        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                    STL_SIZEOF(ellDictHandle) * sRelObject->mTotalUniqueKeyCnt,
                                    (void **) & sRelObject->mAffectUniqueKeyHandle,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlMemset( sRelObject->mAffectUniqueKeyHandle,
                   0x00,
                   STL_SIZEOF(ellDictHandle) * sRelObject->mTotalUniqueKeyCnt );

        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                    STL_SIZEOF(void *) * sRelObject->mTotalUniqueKeyCnt,
                                    (void **) & sRelObject->mAffectUniqueKeyIndexPhyHandle,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlMemset( sRelObject->mAffectUniqueKeyIndexPhyHandle,
                   0x00,
                   STL_SIZEOF(void *) * sRelObject->mTotalUniqueKeyCnt );
    }
    else
    {
        sRelObject->mAffectUniqueKeyCnt = 0;
        sRelObject->mAffectUniqueKeyHandle = NULL;
        sRelObject->mAffectUniqueKeyIndexPhyHandle = NULL;
    }

    /**
     * UPDATE 컬럼의 영향을 받는 UNIQUE KEY 선별
     */
    
    sRelObject->mAffectUniqueKeyCnt = 0;
    sHandleArray = sRelObject->mTotalUniqueKeyHandle;
    
    for( sLoop = 0 ; sLoop < sRelObject->mTotalUniqueKeyCnt ; sLoop++ )
    {
        /**
         * UPDATE Column 과 관련 있는 인덱스인지 검사
         */

        sIndexHandle = ellGetConstraintIndexDictHandle( & sHandleArray[ sLoop ] );

        /* Write 대상이 되는 column list와 관련된 Index Key Column이 존재하는지 검사 */
        STL_TRY( qlvIsAffectedIndexByColumnList( sIndexHandle,
                                                 sWriteColumnList,
                                                 & sIsAffected,
                                                 aEnv )
                 == STL_SUCCESS );

        if( sIsAffected == STL_TRUE )
        {
            /* Index Key Column 정보를 Read Column List 에 추가 */
            STL_TRY( qlvAddRefColumnOnBaseTableByIndex( sReadTableNode,
                                                        sIndexHandle,
                                                        QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                        aEnv )
                     == STL_SUCCESS );
            
            stlMemcpy( & sRelObject->mAffectUniqueKeyHandle[sRelObject->mAffectUniqueKeyCnt],
                       & sRelObject->mTotalUniqueKeyHandle[sLoop],
                       STL_SIZEOF(ellDictHandle) );

            sRelObject->mAffectUniqueKeyIndexPhyHandle[sRelObject->mAffectUniqueKeyCnt]
                = ellGetIndexHandle( sIndexHandle );

            sRelObject->mAffectUniqueKeyCnt++;
        }
        else
        {
            /* UPDATE Column 에 영향을 받지 않는 인덱스 */
        }
    }

    /****************************************************************************
     * FOREIGN KEY constraint
     ****************************************************************************/

    /**
     * @todo FOREIGN KEY 구현해야 함
     */
    
    STL_TRY_THROW( sRelObject->mTotalForeignKeyCnt == 0, RAMP_ERR_NOT_IMPLEMENTED );

    sRelObject->mAffectForeignKeyCnt = 0;

    /****************************************************************************
     * CHILD FOREIGN KEY constraint
     ****************************************************************************/

    /**
     * @todo CHILD FOREIGN KEY 구현해야 함
     */
    
    STL_TRY_THROW( sRelObject->mTotalChildForeignKeyCnt == 0, RAMP_ERR_NOT_IMPLEMENTED );

    sRelObject->mAffectChildForeignKeyCnt = 0;
    
    /****************************************************************************
     * unique index
     ****************************************************************************/

    /**
     * 공간 할당
     */
    
    if( sRelObject->mTotalUniqueIndexCnt > 0 )
    {
        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                    STL_SIZEOF(ellDictHandle) * sRelObject->mTotalUniqueIndexCnt,
                                    (void **) & sRelObject->mAffectUniqueIndexHandle,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlMemset( sRelObject->mAffectUniqueIndexHandle,
                   0x00,
                   STL_SIZEOF(ellDictHandle) * sRelObject->mTotalUniqueIndexCnt );

        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                    STL_SIZEOF(void *) * sRelObject->mTotalUniqueIndexCnt,
                                    (void **) & sRelObject->mAffectUniqueIndexPhyHandle,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlMemset( sRelObject->mAffectUniqueIndexPhyHandle,
                   0x00,
                   STL_SIZEOF(void *) * sRelObject->mTotalUniqueIndexCnt );
    }
    else
    {
        sRelObject->mAffectUniqueIndexCnt = 0;
        sRelObject->mAffectUniqueIndexHandle = NULL;
        sRelObject->mAffectUniqueIndexPhyHandle = NULL;
    }

    /**
     * UPDATE 컬럼의 영향을 받는 Unique Index 선별
     */

    sRelObject->mAffectUniqueIndexCnt = 0;
    sHandleArray = sRelObject->mTotalUniqueIndexHandle;
    
    for( sLoop = 0 ; sLoop < sRelObject->mTotalUniqueIndexCnt ; sLoop++ )
    {
        /**
         * UPDATE Column 과 관련 있는 인덱스인지 검사
         */

        sIndexHandle = & sHandleArray[ sLoop ];

        /* Write 대상이 되는 column list와 관련된 Index Key Column이 존재하는지 검사 */
        STL_TRY( qlvIsAffectedIndexByColumnList( sIndexHandle,
                                                 sWriteColumnList,
                                                 & sIsAffected,
                                                 aEnv )
                 == STL_SUCCESS );

        if( sIsAffected == STL_TRUE )
        {
            /* Index Key Column 정보를 Read Column List 에 추가 */
            STL_TRY( qlvAddRefColumnOnBaseTableByIndex( sReadTableNode,
                                                        sIndexHandle,
                                                        QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                        aEnv )
                     == STL_SUCCESS );

            stlMemcpy( & sRelObject->mAffectUniqueIndexHandle[sRelObject->mAffectUniqueIndexCnt],
                       & sRelObject->mTotalUniqueIndexHandle[sLoop],
                       STL_SIZEOF(ellDictHandle) );

            sRelObject->mAffectUniqueIndexPhyHandle[sRelObject->mAffectUniqueIndexCnt]
                = ellGetIndexHandle( sIndexHandle );

            sRelObject->mAffectUniqueIndexCnt++;
        }
        else
        {
            /* UPDATE Column 에 영향을 받지 않는 인덱스 */
        }
    }

    /****************************************************************************
     * non-unique index
     ****************************************************************************/

    /**
     * 공간 할당
     */

    if( sRelObject->mTotalNonUniqueIndexCnt > 0 )
    {
        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                    STL_SIZEOF(ellDictHandle) * sRelObject->mTotalNonUniqueIndexCnt,
                                    (void **) & sRelObject->mAffectNonUniqueIndexHandle,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlMemset( sRelObject->mAffectNonUniqueIndexHandle,
                   0x00,
                   STL_SIZEOF(ellDictHandle) * sRelObject->mTotalNonUniqueIndexCnt );

        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                    STL_SIZEOF(void *) * sRelObject->mTotalNonUniqueIndexCnt,
                                    (void **) & sRelObject->mAffectNonUniqueIndexPhyHandle,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlMemset( sRelObject->mAffectNonUniqueIndexPhyHandle,
                   0x00,
                   STL_SIZEOF(void *) * sRelObject->mTotalNonUniqueIndexCnt );
    }
    else
    {
        sRelObject->mAffectNonUniqueIndexCnt = 0;
        sRelObject->mAffectNonUniqueIndexHandle = NULL;
        sRelObject->mAffectNonUniqueIndexPhyHandle = NULL;
    }

    /**
     * UPDATE 컬럼의 영향을 받는 Non-Unique Index 선별
     */
    
    sRelObject->mAffectNonUniqueIndexCnt = 0;
    sHandleArray = sRelObject->mTotalNonUniqueIndexHandle;
    
    for( sLoop = 0 ; sLoop < sRelObject->mTotalNonUniqueIndexCnt ; sLoop++ )
    {
        /**
         * UPDATE Column 과 관련 있는 인덱스인지 검사
         */

        sIndexHandle = & sHandleArray[ sLoop ];

        /* Write 대상이 되는 column list와 관련된 Index Key Column이 존재하는지 검사 */
        STL_TRY( qlvIsAffectedIndexByColumnList( sIndexHandle,
                                                 sWriteColumnList,
                                                 & sIsAffected,
                                                 aEnv )
                 == STL_SUCCESS );

        if( sIsAffected == STL_TRUE )
        {
            /* Index Key Column 정보를 Read Column List 에 추가 */
            STL_TRY( qlvAddRefColumnOnBaseTableByIndex( sReadTableNode,
                                                        sIndexHandle,
                                                        QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                        aEnv )
                     == STL_SUCCESS );

            stlMemcpy( & sRelObject->mAffectNonUniqueIndexHandle[sRelObject->mAffectNonUniqueIndexCnt],
                       & sRelObject->mTotalNonUniqueIndexHandle[sLoop],
                       STL_SIZEOF(ellDictHandle) );

            sRelObject->mAffectNonUniqueIndexPhyHandle[sRelObject->mAffectNonUniqueIndexCnt]
                = ellGetIndexHandle( sIndexHandle );

            sRelObject->mAffectNonUniqueIndexCnt++;
        }
        else
        {
            /* UPDATE Column 에 영향을 받지 않는 인덱스 */
        }
    }

    /****************************************************************************
     * NOT NULL constraint
     ****************************************************************************/

    /**
     * 공간 할당
     */
    
    if( sRelObject->mTotalCheckNotNullCnt > 0 )
    {
        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                    STL_SIZEOF(ellDictHandle) * sRelObject->mTotalCheckNotNullCnt,
                                    (void **) & sRelObject->mAffectCheckNotNullHandle,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlMemset( sRelObject->mAffectCheckNotNullHandle,
                   0x00,
                   STL_SIZEOF(ellDictHandle) * sRelObject->mTotalCheckNotNullCnt );

        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                    STL_SIZEOF(stlBool) * sRelObject->mTotalCheckNotNullCnt,
                                    (void **) & sRelObject->mAffectCheckNotNullDefer,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlMemset( sRelObject->mAffectCheckNotNullDefer,
                   0x00,
                   STL_SIZEOF(stlBool) * sRelObject->mTotalCheckNotNullCnt );
    }
    else
    {
        sRelObject->mAffectCheckNotNullCnt = 0;
        sRelObject->mAffectCheckNotNullHandle = NULL;
        sRelObject->mAffectCheckNotNullDefer = NULL;
    }

    /**
     * UPDATE 컬럼의 영향을 받는 NOT NULL constraint 선별
     */
    
    sRelObject->mAffectCheckNotNullCnt = 0;
    sHandleArray = sRelObject->mTotalCheckNotNullHandle;
    
    for( sLoop = 0 ; sLoop < sRelObject->mTotalCheckNotNullCnt ; sLoop++ )
    {
        sColumnHandle =
            ellGetCheckNotNullColumnHandle( & sRelObject->mTotalCheckNotNullHandle[ sLoop ] );
    
        for( sColCnt = 0 ; sColCnt < aUpdateNode->mSetColumnCnt ; sColCnt++ )
        {
            if( ellGetColumnID( sColumnHandle )
                 == ellGetColumnID( & aUpdateNode->mSetColumnHandle[ sColCnt ] ) )
            {
                stlMemcpy( & sRelObject->mAffectCheckNotNullHandle[sRelObject->mAffectCheckNotNullCnt],
                           & sRelObject->mTotalCheckNotNullHandle[sLoop],
                           STL_SIZEOF(ellDictHandle) );

                sRelObject->mAffectCheckNotNullDefer[sRelObject->mAffectCheckNotNullCnt]
                    = ellGetConstraintAttrIsDeferrable( & sHandleArray[ sLoop ] );
                
                if ( sRelObject->mAffectCheckNotNullDefer[sRelObject->mAffectCheckNotNullCnt] == STL_TRUE )
                {
                    /**
                     * Deferrable NOT NULL 인 경우 Read Column 목록에 포함시킨다.
                     */

                    STL_TRY( qlvAddRefColumnOnBaseTableByConstraintColumn(
                                 sReadTableNode,
                                 ellGetCheckNotNullColumnHandle( & sHandleArray[sLoop] ),
                                 QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                 aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    
                }

                sRelObject->mAffectCheckNotNullCnt++;
                
                break;
            }
        }
    }
    
    /****************************************************************************
     * CHECK constraint
     ****************************************************************************/

    /**
     * @todo CHECK 구현해야 함
     */
    
    STL_TRY_THROW( sRelObject->mTotalCheckClauseCnt == 0, RAMP_ERR_NOT_IMPLEMENTED );

    sRelObject->mAffectCheckClauseCnt = 0;
    
    /****************************************************************************
     * identity column
     ****************************************************************************/

    aRelObject->mHasAffectIdentity = STL_FALSE;
    ellInitDictHandle( & sRelObject->mIdentityColumnHandle );

    for( sColCnt = 0 ; sColCnt < aUpdateNode->mSetColumnCnt ; sColCnt++ )
    {
        if ( ellGetColumnIdentityHandle( & aUpdateNode->mSetColumnHandle[ sColCnt ] ) == NULL )
        {
            /* identity column 이 아님 */
        }
        else
        {
            aRelObject->mHasAffectIdentity = STL_TRUE;
            stlMemcpy( & aRelObject->mIdentityColumnHandle,
                       & aUpdateNode->mSetColumnHandle[ sColCnt ],
                       STL_SIZEOF( ellDictHandle ) );
            break;
        }
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "qlvSetRefColumnListForUpdate()" );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 인자로 받은 expression node를 순회하면서 ROWID column의 relation node를 변경한다.
 * @param[in]     aSrcTableNode   source qlvInitNode
 * @param[in]     aDstTableNode   dest   qlvInitNode
 * @param[in,out] aInitExprCode   qlvInitExpression 
 * @param[in]     aEnv            Environment
 */
stlStatus qlvChangeRelationNodeForRowIdCol( qlvInitNode        * aSrcTableNode,
                                            qlvInitNode        * aDstTableNode,
                                            qlvInitExpression  * aInitExprCode,
                                            qllEnv             * aEnv )
{
    stlInt32             sLoop         = 0;
    
    qlvInitFunction    * sInitFunction = NULL;
    qlvInitTypeCast    * sInitTypeCast = NULL;
    qlvInitCaseExpr    * sInitCaseExpr = NULL;
    qlvInitListFunc    * sInitListFunc = NULL;
    qlvInitListCol     * sInitListCol  = NULL;
    qlvInitRowExpr     * sInitRowExpr  = NULL;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSrcTableNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDstTableNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExprCode != NULL, QLL_ERROR_STACK(aEnv) );

    switch( aInitExprCode->mType )
    {
        case QLV_EXPR_TYPE_ROWID_COLUMN :
            {
                if( aSrcTableNode == aInitExprCode->mExpr.mRowIdColumn->mRelationNode )
                {                    
                    aInitExprCode->mExpr.mRowIdColumn->mRelationNode = aDstTableNode;
                }
                else
                {
                    /* Do Nothing */
                }
                
                break;
            }
        case QLV_EXPR_TYPE_FUNCTION :
            {
                sInitFunction = aInitExprCode->mExpr.mFunction;
                
                for( sLoop = 0 ; sLoop < sInitFunction->mArgCount ; sLoop++ )
                {
                    STL_TRY( qlvChangeRelationNodeForRowIdCol( aSrcTableNode,
                                                               aDstTableNode,
                                                               sInitFunction->mArgs[ sLoop ] ,
                                                               aEnv )
                             == STL_SUCCESS );
                }
                
                break;
            }
        case QLV_EXPR_TYPE_CAST :
            {
                sInitTypeCast = aInitExprCode->mExpr.mTypeCast;
                
                for( sLoop = 0 ; sLoop < DTL_CAST_INPUT_ARG_CNT ; sLoop++ )
                {
                    STL_TRY( qlvChangeRelationNodeForRowIdCol( aSrcTableNode,
                                                               aDstTableNode,
                                                               sInitTypeCast->mArgs[ sLoop ] ,
                                                               aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLV_EXPR_TYPE_CASE_EXPR :
            {
                sInitCaseExpr = aInitExprCode->mExpr.mCaseExpr;
                
                for( sLoop = 0 ; sLoop < sInitCaseExpr->mCount ; sLoop++ )
                {
                    STL_TRY( qlvChangeRelationNodeForRowIdCol( aSrcTableNode,
                                                               aDstTableNode,
                                                               sInitCaseExpr->mWhenArr[ sLoop ] ,
                                                               aEnv )
                             == STL_SUCCESS );

                    STL_TRY( qlvChangeRelationNodeForRowIdCol( aSrcTableNode,
                                                               aDstTableNode,
                                                               sInitCaseExpr->mThenArr[ sLoop ] ,
                                                               aEnv )
                             == STL_SUCCESS );
                }

                STL_TRY( qlvChangeRelationNodeForRowIdCol( aSrcTableNode,
                                                           aDstTableNode,
                                                           sInitCaseExpr->mDefResult ,
                                                           aEnv )
                         == STL_SUCCESS );
                
                break;
            }
        case QLV_EXPR_TYPE_ROW_EXPR :
            {
                sInitRowExpr = aInitExprCode->mExpr.mRowExpr;
                
                for( sLoop = 0 ; sLoop < sInitRowExpr->mArgCount ; sLoop++ )
                {
                    STL_TRY( qlvChangeRelationNodeForRowIdCol( aSrcTableNode,
                                                               aDstTableNode,
                                                               sInitRowExpr->mArgs[ sLoop ] ,
                                                               aEnv )
                             == STL_SUCCESS );
                }
                
                break;
            }
        case QLV_EXPR_TYPE_LIST_COLUMN :
            {
                sInitListCol = aInitExprCode->mExpr.mListCol;
                
                for( sLoop = 0 ; sLoop < sInitListCol->mArgCount ; sLoop++ )
                {
                    STL_TRY( qlvChangeRelationNodeForRowIdCol( aSrcTableNode,
                                                               aDstTableNode,
                                                               sInitListCol->mArgs[ sLoop ] ,
                                                               aEnv )
                             == STL_SUCCESS );
                }
                
                break;
            }
        case QLV_EXPR_TYPE_LIST_FUNCTION :
            {
                sInitListFunc = aInitExprCode->mExpr.mListFunc;
                
                for( sLoop = 0 ; sLoop < sInitListFunc->mArgCount ; sLoop++ )
                {
                    STL_TRY( qlvChangeRelationNodeForRowIdCol( aSrcTableNode,
                                                               aDstTableNode,
                                                               sInitListFunc->mArgs[ sLoop ] ,
                                                               aEnv )
                             == STL_SUCCESS );
                }
                
                break;
            }
        case QLV_EXPR_TYPE_REFERENCE :
            {
                STL_TRY( qlvChangeRelationNodeForRowIdCol( aSrcTableNode,
                                                           aDstTableNode,
                                                           aInitExprCode->mExpr.mReference->mOrgExpr,
                                                           aEnv )
                         == STL_SUCCESS );
                
                break;
            }
        default:
            {
                /* 나머지 Expression들은 내부에 RowId Column Expr을 포함할 수 없다. */
                
                /* Do Nothing */
                break;
            }
    }

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;    
}


/** @} qlvUpdate */
