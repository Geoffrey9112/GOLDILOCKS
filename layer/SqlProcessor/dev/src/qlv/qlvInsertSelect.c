/*******************************************************************************
 * qlvInsertSelect.c
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
 * @file qlvInsertSelect.c
 * @brief SQL Processor Layer INSERT .. SELECT statement Validation
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlvInsertSelect
 * @{
 */

/**
 * @brief INSERT .. SELECT 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlvValidateInsertSelect( smlTransId      aTransID,
                                   qllDBCStmt    * aDBCStmt,
                                   qllStatement  * aSQLStmt,
                                   stlChar       * aSQLString,
                                   qllNode       * aParseTree,
                                   qllEnv        * aEnv )
{
    ellDictHandle        * sAuthHandle    = NULL;

    qlpInsert            * sParseTree     = NULL;
    qlvInitInsertSelect  * sInitPlan      = NULL;

    qlpObjectName        * sObjectName    = NULL;
    stlBool                sObjectExist   = STL_FALSE;

    ellDictHandle          sSchemaHandle;

    stlInt32               sDeclTargetCnt = 0;
    stlInt32               sLoop          = 0;

    qlvInitNode          * sQueryNode     = NULL;

    stlInt32               sTargetCount   = 0;
    qlvInitTarget        * sTargets       = NULL;

    qlvInitSingleRow     * sRowExpr       = NULL;
    
    qlvRefTableList      * sWriteTableList = NULL;
    qlpBaseTableNode       sTableNode;

    qlpColumnRef           sColumnRefNode;
    qlpColumnName          sColumnNameNode;
    qlvInitExpression    * sColExpr      = NULL;
    stlBool                sIsUpdatable   = STL_FALSE;

    qlvInitStmtExprList  * sStmtExprList  = NULL;
    qlvInitExprList      * sQueryExprList = NULL;
    qlvStmtInfo            sStmtInfo;

    qlvRefExprItem       * sRefColumnItem    = NULL;

    qlvRefExprList       * sPrivColList = NULL;

    stlInt32               sTableCount;
    stlInt32               sQBIndex;
    
    ellInitDictHandle( &sSchemaHandle );
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_INSERT_SELECT_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_INTO_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_QUERY_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aParseTree->mType == QLL_STMT_INSERT_SELECT_TYPE) ||
                        (aParseTree->mType == QLL_STMT_INSERT_RETURNING_INTO_TYPE) ||
                        (aParseTree->mType == QLL_STMT_INSERT_RETURNING_QUERY_TYPE),
                        QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * 준비 작업
     **********************************************************/

    /**
     * 기본 정보 획득
     */
    
    sParseTree  = (qlpInsert *) aParseTree;
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlvInitInsertSelect),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlvInitInsertSelect) );

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
     * 임시 Query 단위 Expression List 설정
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
    stlStrcpy( sStmtInfo.mQBPrefix, QLV_QUERY_BLOCK_NAME_PREFIX_INS );
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
    
    /**********************************************************
     * Reference Table List 구성
     **********************************************************/

    /**
     * Reference Write Table List 생성
     */

    STL_TRY( qlvCreateRefTableList( & QLL_VALIDATE_MEM(aEnv),
                                    NULL,
                                    & sWriteTableList,
                                    aEnv )
             == STL_SUCCESS );

    
    /**
     * Write Table Node 생성
     */

    sTableNode.mType    = QLL_BASE_TABLE_NODE_TYPE;
    sTableNode.mNodePos = sParseTree->mRelation->mNodePos;
    sTableNode.mName    = sParseTree->mRelation->mName;
    sTableNode.mDumpOpt = NULL;
    sTableNode.mAlias   = sParseTree->mRelation->mAlias;

    sTableCount = 0;
    STL_TRY( qlvTableNodeValidation( & sStmtInfo,
                                     sInitPlan->mInitPlan.mValidationObjList,
                                     sWriteTableList,
                                     (qllNode *) & sTableNode,
                                     &(sInitPlan->mWriteTableNode),
                                     &sTableCount,
                                     aEnv )
             == STL_SUCCESS );

    STL_DASSERT( sTableCount == 1 );

    STL_TRY( qlvSetNodeToRefTableItem( sWriteTableList,
                                       sInitPlan->mWriteTableNode,
                                       aEnv )
             == STL_SUCCESS );

                               
    /**********************************************************
     * Insert Source Validation
     **********************************************************/

    /**
     * Table 정보 획득 
     */
    
    sInitPlan->mTotalColumnCnt = ellGetTableColumnCnt( & sInitPlan->mTableHandle );
    sInitPlan->mColumnHandle = ellGetTableColumnDictHandle( & sInitPlan->mTableHandle );

    /**
     * Insert Target Column 을 검증
     */

    STL_TRY( qlvValidateInsertTarget( & sStmtInfo,
                                      sInitPlan->mInitPlan.mValidationObjList,
                                      & sInitPlan->mTableHandle,
                                      sInitPlan->mTotalColumnCnt,
                                      sInitPlan->mColumnHandle,
                                      sParseTree->mSource,
                                      & sInitPlan->mInsertTarget,
                                      aEnv )
             == STL_SUCCESS );
    

    /**********************************************************
     * Select Subquery 를 검증
     **********************************************************/

    /**
     * Query Element Validation 
     */

    STL_TRY( qlvQueryNodeValidation( & sStmtInfo,
                                     STL_FALSE,
                                     sInitPlan->mInitPlan.mValidationObjList,
                                     NULL,
                                     (qllNode *) sParseTree->mSource->mSubquery,
                                     & sInitPlan->mSubQueryNode,
                                     aEnv )
             == STL_SUCCESS );

    ((qlvInitPlan *) sInitPlan)->mHasHintError = sStmtInfo.mHasHintError;
    

    /**
     * Insert Column 과 Select Column 개수가 동일한지 검사
     */

    sDeclTargetCnt = 0;
    
    for ( sLoop = 0; sLoop < sInitPlan->mTotalColumnCnt; sLoop++ )
    {
        if ( sInitPlan->mInsertTarget.mIsDeclareColumn[sLoop] == STL_TRUE )
        {
            sDeclTargetCnt++;
        }
        else
        {
            /* nothing to do */
        }
    }

    sQueryNode = sInitPlan->mSubQueryNode;
    if( sQueryNode->mType == QLV_NODE_TYPE_QUERY_SET )
    {
        sTargetCount = ((qlvInitQuerySetNode*) sQueryNode)->mSetTargetCount;
        sTargets     = ((qlvInitQuerySetNode*) sQueryNode)->mSetTargets;
    }
    else
    {
        sTargetCount = ((qlvInitQuerySpecNode*) sQueryNode)->mTargetCount;
        sTargets     = ((qlvInitQuerySpecNode*) sQueryNode)->mTargets;
    }
    
    if( sDeclTargetCnt != sTargetCount )
    {
        if( sDeclTargetCnt > sTargetCount )
        {
            STL_THROW( RAMP_ERR_NOT_ENOUGH_VALUES );
        }
        else
        {
            STL_THROW( RAMP_ERR_TOO_MANY_VALUES );
        }
    }

    /**********************************************************
     * Default Expression 과 Subquery Target Expression 을 조합
     **********************************************************/

    /**
     * Column Expression 공간 할당
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlvInitSingleRow ),
                                (void **) & sRowExpr,
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    stlMemset( sRowExpr, 0x00, STL_SIZEOF( qlvInitSingleRow ) );
             
    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aDBCStmt ), 
                                STL_SIZEOF( qlvInitExpression * ) * sInitPlan->mTotalColumnCnt, 
                                (void **) & sRowExpr->mValueExpr, 
                                KNL_ENV( aEnv ) ) 
             == STL_SUCCESS );
    
    stlMemset( sRowExpr->mValueExpr, 
               0x00, 
               STL_SIZEOF( qlvInitExpression * ) * sInitPlan->mTotalColumnCnt );


    /**
     * Default Expression 과 Subquery Target Expression 을 조합
     */
    
    for ( sLoop = 0; sLoop < sInitPlan->mTotalColumnCnt; sLoop++ )
    {
        if ( sInitPlan->mInsertTarget.mIsRowDefault[ sLoop ] == STL_TRUE )
        {
            /**
             * Default Expression 을 Column Expression 에 등록
             */
            
            sRowExpr->mValueExpr[ sLoop ] = sInitPlan->mInsertTarget.mRowDefaultExpr[ sLoop ];
        }
        else
        {
            /**
             * Subquery Target 을 Column Expression 에 등록
             */

            sRowExpr->mValueExpr[ sLoop ] = sTargets->mExpr;
        }

        /**
         * - ALWAYS IDENTITY 의 경우는 Default Expression 을 사용하게 됨.
         */

        if ( sInitPlan->mInsertTarget.mIsDeclareColumn[ sLoop ] == STL_TRUE )
        {
            sTargets++;
        }
        else
        {
            /* nothing to do */
        }
    }

    sInitPlan->mRowList = sRowExpr;


    /**********************************************************
     * Insert Column Expression List
     **********************************************************/

    /**
     * Privilege Column List 구성
     */

    STL_TRY( qlvCreateRefExprList( & sPrivColList,
                                   QLL_INIT_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );
    
    /**
     * Insert Column Expression List 구성 (전체 column)
     */

    STL_TRY( qlvCreateRefExprList( & sInitPlan->mInsertColExprList,
                                   QLL_INIT_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    stlMemset( & sColumnNameNode, 0x00, STL_SIZEOF( qlpColumnName ) );
    sColumnNameNode.mType = QLL_COLUMN_NAME_TYPE;

    for( sLoop = 0 ; sLoop < sInitPlan->mTotalColumnCnt ; sLoop++ )
    {
        /**
         * Column Expression을 list로 구성
         */

        if ( ellGetColumnUnused( & sInitPlan->mColumnHandle[ sLoop ] ) == STL_TRUE )
        {
            /**
             * UNUSED column 임
             */
            
            STL_TRY( qlvAddRefColumnItemOnBaseTable( QLL_INIT_PLAN( aDBCStmt ),
                                                     QLV_EXPR_PHRASE_TARGET,
                                                     0,  /* aNodePos */
                                                     (qlvInitBaseTableNode *) sWriteTableList->mHead->mTableNode,
                                                     & sInitPlan->mColumnHandle[ sLoop ],
                                                     STL_FALSE, /* aNeedCopy */
                                                     & sRefColumnItem,
                                                     aEnv )
                     == STL_SUCCESS );

            sColExpr = sRefColumnItem->mExprPtr;
        }
        else
        {
            sColumnNameNode.mColumnPos = 0;
            sColumnNameNode.mColumn    = ellGetColumnName( & sInitPlan->mColumnHandle[ sLoop ] );
            
            sColumnRefNode.mType            = QLL_COLUMN_REF_TYPE;
            sColumnRefNode.mNodePos         = 0;
            sColumnRefNode.mName            = & sColumnNameNode;
            sColumnRefNode.mIsSetOuterMark  = STL_FALSE;
            
            sIsUpdatable = STL_TRUE;
            STL_TRY( qlvValidateColumnRef( & sStmtInfo,
                                           QLV_EXPR_PHRASE_TARGET,
                                           sInitPlan->mInitPlan.mValidationObjList,
                                           sWriteTableList,
                                           STL_FALSE,
                                           0, /* Allowed Aggregation Depth */
                                           & sColumnRefNode,
                                           & sColExpr,
                                           NULL,
                                           & sIsUpdatable,
                                           KNL_BUILTIN_FUNC_INVALID,
                                           DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                           DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                           QLL_INIT_PLAN(aDBCStmt),
                                           aEnv )
                     == STL_SUCCESS );
        }

        /* add expr to expr list */
        STL_TRY( qlvAddExprToRefExprList( sInitPlan->mInsertColExprList,
                                          sColExpr,
                                          STL_FALSE,
                                          QLL_INIT_PLAN(aDBCStmt),
                                          aEnv )
                 == STL_SUCCESS );

        /**
         * Privilege 검사 목록 구성
         */
        
        if ( sInitPlan->mInsertTarget.mIsDeclareColumn[sLoop] == STL_TRUE )
        {
            /* add expr to expr list */
            STL_TRY( qlvAddExprToRefExprList( sPrivColList,
                                              sColExpr,
                                              STL_FALSE,
                                              QLL_INIT_PLAN(aDBCStmt),
                                              aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /* nothing to do */
        }
    }


    /**
     * INSERT ON TABLE 권한 검사
     */

    STL_TRY( qlaCheckTablePriv( aTransID,
                                aDBCStmt,
                                aSQLStmt,
                                ELL_TABLE_PRIV_ACTION_INSERT,
                                & sInitPlan->mTableHandle,
                                sPrivColList,
                                aEnv )
             == STL_SUCCESS );
    
    /**********************************************************
     * Insert 에 영향을 받는 SQL-Object 정보 획득 
     **********************************************************/

    STL_TRY( qlvGetRelObject4Insert( & sStmtInfo,
                                     & sInitPlan->mTableHandle,
                                     & sInitPlan->mRelObject,
                                     aEnv )
             == STL_SUCCESS );
    
    if ( (sInitPlan->mRelObject.mCheckNotNullCnt > 0) ||
         (sInitPlan->mRelObject.mCheckClauseCnt > 0) ||
         (sInitPlan->mRelObject.mHasIdentity == STL_TRUE) )
    {
        sInitPlan->mHasCheck = STL_TRUE;
    }
    else
    {
        sInitPlan->mHasCheck = STL_FALSE;
    }
    
    
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


    /**********************************************************
     * RETURNING 절 Validation
     **********************************************************/

    if( sParseTree->mReturnTarget != NULL )
    {
        STL_TRY( qlvValidateReturningPhrase( & sStmtInfo,
                                             sInitPlan->mInitPlan.mValidationObjList,
                                             sInitPlan->mWriteTableNode,
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

    
    /**
     * Init Plan 연결
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;
    
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

    STL_CATCH( RAMP_ERR_NOT_ENOUGH_VALUES )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_ENOUGH_VALUES,
                      qlgMakeErrPosString( aSQLString,
                                           sObjectName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_TOO_MANY_VALUES )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TOO_MANY_VALUES,
                      qlgMakeErrPosString( aSQLString,
                                           sObjectName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/** @} qlvInsertSelect */
