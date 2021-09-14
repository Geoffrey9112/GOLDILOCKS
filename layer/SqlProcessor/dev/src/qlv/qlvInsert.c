/*******************************************************************************
 * qlvInsert.c
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
 * @file qlvInsert.c
 * @brief SQL Processor Layer INSERT statement Validation
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @addtogroup qlvInsert
 * @{
 */


/**
 * @brief INSERT VALUES 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlvValidateInsertValues( smlTransId      aTransID,
                                   qllDBCStmt    * aDBCStmt,
                                   qllStatement  * aSQLStmt,
                                   stlChar       * aSQLString,
                                   qllNode       * aParseTree,
                                   qllEnv        * aEnv )
{
    ellDictHandle        * sAuthHandle   = NULL;

    qlpInsert            * sParseTree    = NULL;
    qlpInsertSource      * sInsertSource = NULL;
    qlvInitInsertValues  * sInitPlan     = NULL;

    qlpObjectName        * sObjectName   = NULL;
    stlBool                sObjectExist  = STL_FALSE;

    ellDictHandle          sSchemaHandle;

    qlvRefTableList      * sWriteTableList = NULL;
    qlpBaseTableNode       sTableNode;

    qlpColumnRef           sColumnRefNode;
    qlpColumnName          sColumnNameNode;
    qlvInitExpression    * sColExpr       = NULL;
    stlBool                sIsUpdatable   = STL_FALSE;
    stlInt32               sLoop          = 0;

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
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_INSERT_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_INTO_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_QUERY_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aParseTree->mType == QLL_STMT_INSERT_TYPE) ||
                        (aParseTree->mType == QLL_STMT_INSERT_RETURNING_INTO_TYPE) ||
                        (aParseTree->mType == QLL_STMT_INSERT_RETURNING_QUERY_TYPE),
                        QLL_ERROR_STACK(aEnv) );

    
    /**********************************************************
     * 준비 작업
     **********************************************************/

    /**
     * 기본 정보 획득
     */
    
    sParseTree    = (qlpInsert *) aParseTree;
    sInsertSource = sParseTree->mSource;
    sAuthHandle   = ellGetCurrentUserHandle( ELL_ENV(aEnv) );

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF( qlvInitInsertValues ),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlvInitInsertValues) );

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

    KNL_BREAKPOINT( KNL_BREAKPOINT_QLL_VALIDATE_SQL_GET_TABLE_HANDLE, KNL_ENV(aEnv) );
    
    
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
                                      sInsertSource,
                                      & sInitPlan->mInsertTarget,
                                      aEnv )
             == STL_SUCCESS );
    
    /**
     * Insert Source 를 검증한다.
     */

    STL_TRY( qlvValidateMultiValues( & sStmtInfo,
                                     sInitPlan,
                                     sInsertSource,
                                     aEnv )
             == STL_SUCCESS );


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
        /**
         * INSERT VALUES 구문은 single-row 인지 validation 단계에서 판단할 수 있다.
         */

        if ( aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_INTO_TYPE )
        {
            STL_TRY_THROW( sInitPlan->mMultiValueCnt == 1,
                           RAMP_ERR_SINGLE_ROW_EXPRESSION );
        }
        else
        {
            STL_DASSERT( aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_QUERY_TYPE );
        }

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

    STL_CATCH( RAMP_ERR_SINGLE_ROW_EXPRESSION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INTO_CLAUSE_MULTIPLE_ROWS,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief INSERT INTO target column 을 Validation 한다.
 * @param[in]     aStmtInfo           Stmt Information
 * @param[in]     aValidationObjList  Validation Object List
 * @param[in]     aTableHandle        Table Handle
 * @param[in]     aTableColumnCnt     Table 의 Column Count
 * @param[in]     aTableColumnHandle  Table 의 Column Handle Array
 * @param[in]     aInsertSource       Insert Source Parse Tree
 * @param[in,out] aInsertTarget       Insert Target Init Plan
 * @param[in]     aEnv                Environment
 */
stlStatus qlvValidateInsertTarget( qlvStmtInfo         * aStmtInfo,
                                   ellObjectList       * aValidationObjList,
                                   ellDictHandle       * aTableHandle,
                                   stlInt32              aTableColumnCnt,
                                   ellDictHandle       * aTableColumnHandle,
                                   qlpInsertSource     * aInsertSource,
                                   qlvInitInsertTarget * aInsertTarget,
                                   qllEnv              * aEnv )
{
    qlpListElement * sTargetElement = NULL;
    qlpValue       * sTargetColumn = NULL;
    stlChar        * sColumnName = NULL;
    stlInt32         sColumnIdx = 0;
    ellDictHandle    sColumnHandle;

    stlBool   sObjectExist = STL_FALSE;

    stlInt32  sUsedColCnt = 0;
    stlInt32  i = 0;

    ellInitDictHandle( & sColumnHandle );
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValidationObjList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableColumnCnt > 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableColumnHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInsertSource != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInsertTarget != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * mTargetColumnIdx 공간
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                STL_SIZEOF(stlInt32) * aTableColumnCnt,
                                (void **) & aInsertTarget->mTargetColumnIdx,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( aInsertTarget->mTargetColumnIdx,
               0x00,
               STL_SIZEOF(stlInt32) * aTableColumnCnt );
    
    /**
     * mIsDeclareColumn 공간
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                STL_SIZEOF(stlBool) * aTableColumnCnt,
                                (void **) & aInsertTarget->mIsDeclareColumn,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( aInsertTarget->mIsDeclareColumn,
               0x00,
               STL_SIZEOF(stlBool) * aTableColumnCnt );
    
    /**
     * mIsRowDefault 공간 
     */
    
    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                STL_SIZEOF(stlBool) * aTableColumnCnt,
                                (void **) & aInsertTarget->mIsRowDefault,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( aInsertTarget->mIsRowDefault,
               0x00,
               STL_SIZEOF(stlBool) * aTableColumnCnt );

    /**
     * mRowDefaultExpr 공간 
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                STL_SIZEOF(qlvInitExpression *) * aTableColumnCnt,
                                (void **) & aInsertTarget->mRowDefaultExpr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( aInsertTarget->mRowDefaultExpr,
               0x00,
               STL_SIZEOF(qlvInitExpression *) * aTableColumnCnt );

    
    /**
     * Insert Target Validation
     */

    if ( aInsertSource->mIsAllColumn == STL_TRUE )
    {
        /**
         * Target Column 이 기술되어 있지 않음
         * - INSERT INTO table_name VALUES ...
         */

        sUsedColCnt = 0;
        for ( i = 0; i < aTableColumnCnt; i++ )
        {
            if ( ellGetColumnUnused( & aTableColumnHandle[i] ) == STL_TRUE )
            {
                /**
                 * UNUSED Column 임
                 */
                
                aInsertTarget->mIsDeclareColumn[i] = STL_FALSE;
                aInsertTarget->mIsRowDefault[i] = STL_TRUE;
            }
            else
            {
                aInsertTarget->mTargetColumnIdx[sUsedColCnt]
                    = ellGetColumnIdx( & aTableColumnHandle[i] );

                sUsedColCnt++;

                aInsertTarget->mIsDeclareColumn[i] = STL_TRUE;
                
                if ( ellGetColumnIdentityHandle( & aTableColumnHandle[i] ) == NULL )
                {
                    /**
                     * Identity Column 이 아님
                     * 모든 Multi Values 에 Row DEFAULT 값을 설정한는 것은 아님 
                     */
                    aInsertTarget->mIsRowDefault[i] = STL_FALSE;
                }
                else
                {
                    if ( ellGetColumnIdentityGenOption( & aTableColumnHandle[i] )
                         == ELL_IDENTITY_GENERATION_ALWAYS )
                    {
                        /**
                         * Indentity Column 이 GENERATION_ALWAYS 옵션이라면,
                         * 해당 값은 DEFAULT Value 로 처리되어야 함.
                         */
                        aInsertTarget->mIsRowDefault[i] = STL_TRUE;
                    }
                    else
                    {
                        aInsertTarget->mIsRowDefault[i] = STL_FALSE;
                    }
                }
            }
        }
    }
    else
    {
        /**
         * Target Column 을 검증
         * - INSERT INTO table_name (column_name, ...) VALUES ...
         */

        i = 0;
        QLP_LIST_FOR_EACH( aInsertSource->mColumns, sTargetElement )
        {
            sTargetColumn = (qlpValue *) QLP_LIST_GET_POINTER_VALUE( sTargetElement );
            sColumnName = QLP_GET_PTR_VALUE( sTargetColumn );
        
            /**
             * 유효한 Column Name 인지 검사
             */
            
            STL_TRY( ellGetColumnDictHandle( aStmtInfo->mTransID,
                                             aStmtInfo->mSQLStmt->mViewSCN,
                                             aTableHandle,
                                             sColumnName,
                                             & sColumnHandle,
                                             & sObjectExist,
                                             ELL_ENV(aEnv) )
                     == STL_SUCCESS );
            STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_COLUMN_NOT_EXISTS );
            
            /**
             * 동일한 Column 이 사용되는지 검사
             */

            sColumnIdx = ellGetColumnIdx( & sColumnHandle );
            STL_TRY_THROW( aInsertTarget->mIsDeclareColumn[sColumnIdx] == STL_FALSE,
                           RAMP_ERR_DUPLICATE_COLUMN_NAME );

            aInsertTarget->mTargetColumnIdx[i] = sColumnIdx;
            aInsertTarget->mIsDeclareColumn[sColumnIdx] = STL_TRUE;

            if ( ellGetColumnIdentityHandle( & sColumnHandle ) == NULL )
            {
                /**
                 * Identity Column 이 아님
                 */
                aInsertTarget->mIsRowDefault[sColumnIdx] = STL_FALSE;
            }
            else
            {
                if ( ellGetColumnIdentityGenOption( & aTableColumnHandle[i] )
                     == ELL_IDENTITY_GENERATION_ALWAYS )
                {
                    /**
                     * Indentity Column 이 GENERATION_ALWAYS 옵션이라면,
                     * 해당 값은 DEFAULT Value 로 처리되어야 함.
                     */
                    
                    aInsertTarget->mIsRowDefault[sColumnIdx] = STL_TRUE;
                }
                else
                {
                    aInsertTarget->mIsRowDefault[sColumnIdx] = STL_FALSE;
                }
            }

            i++;
        }
        
        /**
         * Target Column 에 기술되지 않은 컬럼은 DEFAULT 값을 설정해야 함.
         */

        for ( i = 0; i < aTableColumnCnt; i++ )
        {
            if ( aInsertTarget->mIsDeclareColumn[i] == STL_TRUE )
            {
                /*
                 * nothing to do - 이미 설정됨 
                 */
            }
            else
            {
                /**
                 * 정의되지 않은 Target Column 임
                 */
                aInsertTarget->mIsRowDefault[i] = STL_TRUE;
            }
        }
        
    }

    /**
     * 모든 Multi-Values 에 해당하는 Row Default Value 생성
     */

    for ( i = 0; i < aTableColumnCnt; i++ )
    {
        if ( aInsertTarget->mIsRowDefault[i] == STL_TRUE )
        {
            /**
             * 모든 Row 에 적용할 Default Value Expression 을 생성
             */
            
            STL_TRY( qlvValidateDefaultBNF( aStmtInfo,
                                            aValidationObjList,
                                            (qllNode *) aInsertSource,
                                            & aTableColumnHandle[i],
                                            & aInsertTarget->mRowDefaultExpr[i],
                                            KNL_BUILTIN_FUNC_INVALID,
                                            DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                            QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                            aEnv )
                     == STL_SUCCESS );

            /**
             * Rewrite Expression
             */

            STL_TRY( qlvRewriteExpr( aInsertTarget->mRowDefaultExpr[i],
                                     DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                     aStmtInfo->mQueryExprList,
                                     & aInsertTarget->mRowDefaultExpr[i],
                                     QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                     aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /**
             * 모든 Row에 적용할 Default Value 가 없음
             */

            aInsertTarget->mRowDefaultExpr[i] = NULL;
        }
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_COLUMN_NOT_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_IDENTIFIER,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sTargetColumn->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sColumnName );
    }

    STL_CATCH( RAMP_ERR_DUPLICATE_COLUMN_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DUPLICATE_COLUMN_NAME,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sTargetColumn->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Multi Values Expression 을 Validation 한다.
 * @param[in]  aStmtInfo     Stmt Information
 * @param[in]  aInitPlan     Insert Values Init Plan
 * @param[in]  aInsertSource Insert Source Parse Tree
 * @param[in]  aEnv          Environment
 */
stlStatus qlvValidateMultiValues( qlvStmtInfo         * aStmtInfo,
                                  qlvInitInsertValues * aInitPlan,
                                  qlpInsertSource     * aInsertSource,
                                  qllEnv              * aEnv )
{
    qlpListElement * sValuesElement = NULL;
    qlpList        * sSingleRowExpr = NULL;

    qlvInitSingleRow * sCurrRowExpr = NULL;
    qlvInitSingleRow * sPrevRowExpr = NULL;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInsertSource != NULL, QLL_ERROR_STACK(aEnv) );
    
    /**
     * Multi VALUES 를 검증
     */

    sPrevRowExpr = NULL;

    aInitPlan->mMultiValueCnt = 0;
    
    if ( aInsertSource->mIsDefault == STL_TRUE )
    {
        aInitPlan->mMultiValueCnt = 1;
        
        /**
         * INSERT INTO ... DEFAULT VALUES
         */

        STL_TRY( qlvValidateSingleValues( aStmtInfo,
                                          aInitPlan,
                                          aInsertSource,
                                          NULL,
                                          & sCurrRowExpr,
                                          aEnv )
                 == STL_SUCCESS );

        /**
         * Multi Values 를 연결
         */

        aInitPlan->mRowList = sCurrRowExpr;
    }
    else
    {
        QLP_LIST_FOR_EACH( aInsertSource->mValues, sValuesElement )
        {
            aInitPlan->mMultiValueCnt++;
            
            /**
             * Single VALUES 검증
             */
            
            sSingleRowExpr = (qlpList *) QLP_LIST_GET_POINTER_VALUE( sValuesElement );
            
            STL_TRY( qlvValidateSingleValues( aStmtInfo,
                                              aInitPlan,
                                              aInsertSource,
                                              sSingleRowExpr,
                                              & sCurrRowExpr,
                                              aEnv )
                     == STL_SUCCESS );
            
            /**
             * Multi Values 를 연결
             */
            
            if ( sPrevRowExpr == NULL )
            {
                aInitPlan->mRowList = sCurrRowExpr;
            }
            else
            {
                sPrevRowExpr->mNext = sCurrRowExpr;
            }

            sPrevRowExpr = sCurrRowExpr;
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Single Values Expression 을 Validation 한다.
 * @param[in]  aStmtInfo     Stmt Information
 * @param[in]  aInitPlan     Insert Values Init Plan
 * @param[in]  aInsertSource Insert Source Parse Tree
 * @param[in]  aValuesExpr   Single Values Parse Tree
 * @param[out] aSingleRow    Single Row Expression Code 
 * @param[in]  aEnv          Environment
 */
stlStatus qlvValidateSingleValues( qlvStmtInfo         * aStmtInfo,
                                   qlvInitInsertValues * aInitPlan,
                                   qlpInsertSource     * aInsertSource,
                                   qlpList             * aValuesExpr,
                                   qlvInitSingleRow   ** aSingleRow,
                                   qllEnv              * aEnv )
{
    qlpListElement * sTargetElement = NULL;
    
    qlpListElement * sValueElement = NULL;
    qllNode        * sValueNode = NULL;

    stlInt32  sColumnIdx = 0;
    
    stlInt32  i = 0;

    qlvInitSingleRow * sRowExpr = NULL;

    qlvInitExpression  * sTempExprCode = NULL;

    stlBool              sIsAtomic = STL_FALSE;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInsertSource != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSingleRow != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Single Row Expression 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                STL_SIZEOF(qlvInitSingleRow),
                                (void **) & sRowExpr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sRowExpr, 0x00, STL_SIZEOF(qlvInitSingleRow) );
    
    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                STL_SIZEOF(qlvInitExpression *) * aInitPlan->mTotalColumnCnt,
                                (void **) & sRowExpr->mValueExpr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sRowExpr->mValueExpr,
               0x00,
               STL_SIZEOF(qlvInitExpression *) * aInitPlan->mTotalColumnCnt );


    /**
     * ATOMIC 여부 정보 획득
     */
    
    sIsAtomic = qllGetAtomicAttr( aStmtInfo->mSQLStmt );
    
    /**
     * Insert Source Validation
     */

    if ( aInsertSource->mIsAllColumn == STL_TRUE )
    {
        /**
         * Target Column 이 없는 Value Expression 검사
         * - INSERT INTO table_name VALUES ( ... )
         */
        
        if ( aInsertSource->mIsDefault == STL_TRUE )
        {
            /**
             * 모두 Default Value 임
             * INSERT INTO table_name DEFAULT VALUES;
             */

            for ( i = 0; i < aInitPlan->mTotalColumnCnt; i++ )
            {
                /**
                 * 모든 컬럼에 대해 Default Value Expression 생성
                 */

                if ( aInitPlan->mInsertTarget.mIsRowDefault[i] == STL_TRUE )
                {
                    /**
                     * 항상 Defualt 인 컬럼은 이미 생성되어 있음.
                     */
                }
                else
                {
                    STL_TRY( qlvValidateDefaultBNF( aStmtInfo,
                                                    aInitPlan->mInitPlan.mValidationObjList,
                                                    (qllNode *) aInsertSource,
                                                    & aInitPlan->mColumnHandle[i],
                                                    & sRowExpr->mValueExpr[i],
                                                    KNL_BUILTIN_FUNC_INVALID,
                                                    DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                                    QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                    aEnv )
                             == STL_SUCCESS );

                    
                    /**
                     * Rewrite Expression
                     */

                    STL_TRY( qlvRewriteExpr( sRowExpr->mValueExpr[i],
                                             DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                             aStmtInfo->mQueryExprList,
                                             & sRowExpr->mValueExpr[i],
                                             QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                             aEnv )
                             == STL_SUCCESS );
                }
            }
        }
        else
        {
            /**
             * INSERT INTO table_name VALUES ( ... )
             */
            
            sColumnIdx = 0;
            QLP_LIST_FOR_EACH( aValuesExpr, sValueElement )
            {
                sValueNode = (qllNode *) QLP_LIST_GET_POINTER_VALUE( sValueElement );

                while ( (sColumnIdx < aInitPlan->mTotalColumnCnt) &&
                        (ellGetColumnUnused( & aInitPlan->mColumnHandle[sColumnIdx] ) == STL_TRUE) )
                {
                    /**
                     * UNUSED column 인 경우
                     */
                    
                    sColumnIdx++;
                }
                
                STL_TRY_THROW( sColumnIdx < aInitPlan->mTotalColumnCnt,
                               RAMP_ERR_TOO_MANY_VALUES );

                if ( ((qlpValueExpr *) sValueNode)->mExpr->mType == QLL_DEFAULT_TYPE )
                {
                    /**
                     * Default Expression 을 검증
                     */
                    
                    if ( aInitPlan->mInsertTarget.mIsRowDefault[sColumnIdx] == STL_TRUE )
                    {
                        /* ALWAYS DEFAULT 라면 Expression List 에 넣지 않음 */
                        STL_TRY( qlvValidateDefaultBNF(
                                     aStmtInfo,
                                     aInitPlan->mInitPlan.mValidationObjList,
                                     sValueNode,
                                     & aInitPlan->mColumnHandle[sColumnIdx],
                                     & sTempExprCode,
                                     KNL_BUILTIN_FUNC_INVALID,
                                     DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                     QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                     aEnv )
                                 == STL_SUCCESS );
                    }
                    else
                    {
                        STL_TRY( qlvValidateDefaultBNF(
                                     aStmtInfo,
                                     aInitPlan->mInitPlan.mValidationObjList,
                                     sValueNode,
                                     & aInitPlan->mColumnHandle[sColumnIdx],
                                     & sRowExpr->mValueExpr[sColumnIdx],
                                     KNL_BUILTIN_FUNC_INVALID,
                                     DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                     QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                     aEnv )
                                 == STL_SUCCESS );

                        /**
                         * Rewrite Expression
                         */

                        STL_TRY( qlvRewriteExpr( sRowExpr->mValueExpr[sColumnIdx],
                                                 DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                                 aStmtInfo->mQueryExprList,
                                                 & sRowExpr->mValueExpr[sColumnIdx],
                                                 QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                 aEnv )
                                 == STL_SUCCESS );
                    }
                }
                else
                {
                    /**
                     * Value Expression 을 검증
                     */
                    
                    if ( aInitPlan->mInsertTarget.mIsRowDefault[sColumnIdx] == STL_TRUE )
                    {
                        /* ALWAYS DEFAULT 라면 Expression List 에 넣지 않음 */
                        STL_TRY( qlvValidateValueExpr( aStmtInfo,
                                                       QLV_EXPR_PHRASE_TARGET,
                                                       aInitPlan->mInitPlan.mValidationObjList,
                                                       NULL,
                                                       STL_FALSE,  /* Atomic */
                                                       STL_FALSE,  /* Row Expr */
                                                       0, /* Allowed Aggregation Depth */
                                                       sValueNode,
                                                       & sTempExprCode,
                                                       NULL,
                                                       KNL_BUILTIN_FUNC_INVALID,
                                                       DTL_ITERATION_TIME_NONE,
                                                       DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                                       QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                       aEnv )
                                 == STL_SUCCESS );
                    }
                    else
                    {                       
                        STL_TRY( qlvValidateValueExpr( aStmtInfo,
                                                       QLV_EXPR_PHRASE_TARGET,
                                                       aInitPlan->mInitPlan.mValidationObjList,
                                                       NULL,
                                                       sIsAtomic,  /* Atomic */
                                                       STL_FALSE,  /* Row Expr */
                                                       0, /* Allowed Aggregation Depth */
                                                       sValueNode,
                                                       & sRowExpr->mValueExpr[sColumnIdx],
                                                       NULL,
                                                       KNL_BUILTIN_FUNC_INVALID,
                                                       DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                                       DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                                       QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                       aEnv )
                                 == STL_SUCCESS );

                        /**
                         * Rewrite Expression
                         */

                        STL_TRY( qlvRewriteExpr( sRowExpr->mValueExpr[sColumnIdx],
                                                 DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                                 aStmtInfo->mQueryExprList,
                                                 & sRowExpr->mValueExpr[sColumnIdx],
                                                 QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                 aEnv )
                                 == STL_SUCCESS );
                    }
                }
                
                sColumnIdx++;
            }
            

            while ( (sColumnIdx < aInitPlan->mTotalColumnCnt) &&
                    (ellGetColumnUnused( & aInitPlan->mColumnHandle[sColumnIdx] ) == STL_TRUE) )
            {
                /**
                 * UNUSED column 인 경우
                 */
                
                sColumnIdx++;
            }
            
            if ( sColumnIdx == aInitPlan->mTotalColumnCnt )
            {
                /* OK */
            }
            else
            {
                sValueElement = QLP_LIST_GET_LAST( aValuesExpr );
                sValueNode = (qllNode *) QLP_LIST_GET_POINTER_VALUE( sValueElement );
                
                STL_THROW( RAMP_ERR_NOT_ENOUGH_TABLE_VALUES );
            }
        }
    }
    else
    {
        /**
         * Target Column 을 가지는  Value Expression 검사
         * - INSERT INTO table_name (columns) VALUES ( ... )
         */

        for( sTargetElement = QLP_LIST_GET_FIRST( aInsertSource->mColumns ),
                 sValueElement = QLP_LIST_GET_FIRST( aValuesExpr ),
                 i = 0;
             (sTargetElement != NULL) && (sValueElement != NULL);   
             sTargetElement = QLP_LIST_GET_NEXT_ELEM( sTargetElement ),
                 sValueElement = QLP_LIST_GET_NEXT_ELEM( sValueElement ),
                 i++ )
        {
            sColumnIdx = aInitPlan->mInsertTarget.mTargetColumnIdx[i];
        
            /**
             * Value Expression 검사
             */

            sValueNode = (qllNode *) QLP_LIST_GET_POINTER_VALUE( sValueElement );
            
            if ( ((qlpValueExpr *) sValueNode)->mExpr->mType == QLL_DEFAULT_TYPE )
            {
                /**
                 * Column Default Expression 을 생성
                 */
                
                if ( aInitPlan->mInsertTarget.mIsRowDefault[sColumnIdx] == STL_TRUE )
                {
                    /* ALWAYS DEFAULT 라면 Expression List 에 넣지 않음 */
                    STL_TRY( qlvValidateDefaultBNF( aStmtInfo,
                                                    aInitPlan->mInitPlan.mValidationObjList,
                                                    sValueNode,
                                                    & aInitPlan->mColumnHandle[sColumnIdx],
                                                    & sTempExprCode,
                                                    KNL_BUILTIN_FUNC_INVALID,
                                                    DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                                    QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                    aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    STL_TRY( qlvValidateDefaultBNF( aStmtInfo,
                                                    aInitPlan->mInitPlan.mValidationObjList,
                                                    sValueNode,
                                                    & aInitPlan->mColumnHandle[sColumnIdx],
                                                    & sRowExpr->mValueExpr[sColumnIdx],
                                                    KNL_BUILTIN_FUNC_INVALID,
                                                    DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                                    QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                    aEnv )
                             == STL_SUCCESS );
                    
                    /**
                     * Rewrite Expression
                     */

                    STL_TRY( qlvRewriteExpr( sRowExpr->mValueExpr[sColumnIdx],
                                             DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                             aStmtInfo->mQueryExprList,
                                             & sRowExpr->mValueExpr[sColumnIdx],
                                             QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                             aEnv )
                             == STL_SUCCESS );
                }
            }
            else
            {
                if ( aInitPlan->mInsertTarget.mIsRowDefault[sColumnIdx] == STL_TRUE )
                {
                    /* ALWAYS DEFAULT 라면 Expression List 에 넣지 않음 */
                    STL_TRY( qlvValidateValueExpr( aStmtInfo,
                                                   QLV_EXPR_PHRASE_TARGET,
                                                   aInitPlan->mInitPlan.mValidationObjList,
                                                   NULL,
                                                   STL_FALSE,  /* Atomic */
                                                   STL_FALSE,  /* Row Expr */
                                                   0, /* Allowed Aggregation Depth */
                                                   sValueNode,
                                                   & sTempExprCode,
                                                   NULL,
                                                   KNL_BUILTIN_FUNC_INVALID,
                                                   DTL_ITERATION_TIME_NONE,
                                                   DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                                   QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                   aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    STL_TRY( qlvValidateValueExpr( aStmtInfo,
                                                   QLV_EXPR_PHRASE_TARGET,
                                                   aInitPlan->mInitPlan.mValidationObjList,
                                                   NULL,
                                                   sIsAtomic,  /* Atomic */
                                                   STL_FALSE,  /* Row Expr */
                                                   0, /* Allowed Aggregation Depth */
                                                   sValueNode,
                                                   & sRowExpr->mValueExpr[sColumnIdx],
                                                   NULL,
                                                   KNL_BUILTIN_FUNC_INVALID,
                                                   DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                                   DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                                   QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                   aEnv )
                             == STL_SUCCESS );

                    /**
                     * Rewrite Expression
                     */

                    STL_TRY( qlvRewriteExpr( sRowExpr->mValueExpr[sColumnIdx],
                                             DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                             aStmtInfo->mQueryExprList,
                                             & sRowExpr->mValueExpr[sColumnIdx],
                                             QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                             aEnv )
                             == STL_SUCCESS );
                }
            }
        }

       
        /**
         * Column 개수와 Value 개수가 동일해야 함.
         */
        
        if ( sTargetElement == NULL )
        {
            /* OK */
        }
        else
        {
            sValueElement = QLP_LIST_GET_LAST( aValuesExpr );
            sValueNode = (qllNode *) QLP_LIST_GET_POINTER_VALUE( sValueElement );
            
            STL_THROW( RAMP_ERR_NOT_ENOUGH_VALUES );
        }

        if ( sValueElement == NULL )
        {
            /* OK */
        }
        else
        {
            sValueNode = (qllNode *) QLP_LIST_GET_POINTER_VALUE( sValueElement );
            STL_THROW( RAMP_ERR_TOO_MANY_VALUES );
        }
    }

    /**
     * Row Default Expression 적용
     * - 모든 Row 에 대해 적용할 Default Expression 이라면 이를 대체함.
     *  - INSERT INTO table_name( ... ) VALUES ( ... ) 일 경우, 기술하지 않은 컬럼
     *  - INSERT INTO table_name VALUES ( ... ) 일 경우, ALWAYS identity 컬럼이 이에 해당 
     */

    for ( i = 0; i < aInitPlan->mTotalColumnCnt; i++ )
    {
        if ( aInitPlan->mInsertTarget.mIsRowDefault[i] == STL_TRUE )
        {
            sRowExpr->mValueExpr[i] = aInitPlan->mInsertTarget.mRowDefaultExpr[i];
        }
        else
        {
            /* nothing to do */
        }
        
        /*
         * Row Expression 이 존재해야 함.
         */
        
        STL_PARAM_VALIDATE( sRowExpr->mValueExpr[i] != NULL, QLL_ERROR_STACK(aEnv) );
    }
    
    /**
     * Output 설정
     */

    sRowExpr->mNext = NULL;
    *aSingleRow = sRowExpr;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_ENOUGH_TABLE_VALUES )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_ENOUGH_VALUES,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sValueNode->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_NOT_ENOUGH_VALUES )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_ENOUGH_VALUES,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sValueNode->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_TOO_MANY_VALUES )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TOO_MANY_VALUES,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sValueNode->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief INSERT 구문에 영향을 받는 SQL Object 정보를 획득한다.
 * @param[in]  aStmtInfo     Stmt Information
 * @param[in]  aTableHandle  Table Handle
 * @param[out] aRelObject    Related Objects 
 * @param[in]  aEnv          Environment
 */
stlStatus qlvGetRelObject4Insert( qlvStmtInfo            * aStmtInfo,
                                  ellDictHandle          * aTableHandle,
                                  qlvInitInsertRelObject * aRelObject,
                                  qllEnv                 * aEnv )
{
    stlInt32        i = 0;

    stlInt32        sColumnCnt = 0;
    ellDictHandle * sColumnHandle = NULL;
    ellDictHandle * sIndexHandle = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRelObject != NULL, QLL_ERROR_STACK(aEnv) );

    /******************************************************
     * Key Constraint 정보 획득
     * - key constraint 를 위한 Index 정보도 key handle 에 포함되어 있음 
     ******************************************************/
    
    /**
     * Primary key 정보 획득
     */

    STL_TRY( ellGetTablePrimaryKeyDictHandle( aStmtInfo->mTransID,
                                              aStmtInfo->mSQLStmt->mViewSCN,
                                              QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                              aTableHandle,
                                              & aRelObject->mPrimaryKeyCnt,
                                              & aRelObject->mPrimaryKeyHandle,
                                              ELL_ENV(aEnv) )
             == STL_SUCCESS );

    if ( aRelObject->mPrimaryKeyCnt > 0 )
    {
        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                    STL_SIZEOF(void *) * aRelObject->mPrimaryKeyCnt,
                                    (void **) & aRelObject->mPrimaryKeyIndexPhyHandle,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        for ( i = 0; i < aRelObject->mPrimaryKeyCnt; i++ )
        {
            sIndexHandle =
                ellGetConstraintIndexDictHandle( & aRelObject->mPrimaryKeyHandle[i] );
            aRelObject->mPrimaryKeyIndexPhyHandle[i] = ellGetIndexHandle( sIndexHandle );
        }
    }
    
    /**
     * Unique Key 정보 획득
     */

    STL_TRY( ellGetTableUniqueKeyDictHandle( aStmtInfo->mTransID,
                                             aStmtInfo->mSQLStmt->mViewSCN,
                                             QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                             aTableHandle,
                                             & aRelObject->mUniqueKeyCnt,
                                             & aRelObject->mUniqueKeyHandle,
                                             ELL_ENV(aEnv) )
             == STL_SUCCESS );

    if ( aRelObject->mUniqueKeyCnt > 0 )
    {
        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                    STL_SIZEOF(void *) * aRelObject->mUniqueKeyCnt,
                                    (void **) & aRelObject->mUniqueKeyIndexPhyHandle,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        for ( i = 0; i < aRelObject->mUniqueKeyCnt; i++ )
        {
            sIndexHandle =
                ellGetConstraintIndexDictHandle( & aRelObject->mUniqueKeyHandle[i] );
            aRelObject->mUniqueKeyIndexPhyHandle[i] = ellGetIndexHandle( sIndexHandle );
        }
    }
    
    /**
     * Foreign Key 정보 획득
     * @todo Foreign Key 구현해야 함.
     */

    STL_TRY( ellGetTableForeignKeyDictHandle( aStmtInfo->mTransID,
                                              aStmtInfo->mSQLStmt->mViewSCN,
                                              QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                              aTableHandle,
                                              & aRelObject->mForeignKeyCnt,
                                              & aRelObject->mForeignKeyHandle,
                                              ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( aRelObject->mForeignKeyCnt == 0, RAMP_ERR_NOT_IMPLEMENTED );

    if ( aRelObject->mForeignKeyCnt > 0 )
    {
        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                    STL_SIZEOF(void *) * aRelObject->mForeignKeyCnt,
                                    (void **) & aRelObject->mForeignKeyIndexPhyHandle,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        for ( i = 0; i < aRelObject->mForeignKeyCnt; i++ )
        {
            sIndexHandle =
                ellGetConstraintIndexDictHandle( & aRelObject->mForeignKeyHandle[i] );
            aRelObject->mForeignKeyIndexPhyHandle[i] = ellGetIndexHandle( sIndexHandle );
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
                                               QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                               aTableHandle,
                                               & aRelObject->mUniqueIndexCnt,
                                               & aRelObject->mUniqueIndexHandle,
                                               ELL_ENV(aEnv) )
             == STL_SUCCESS );

    if ( aRelObject->mUniqueIndexCnt > 0 )
    {
        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                    STL_SIZEOF(void *) * aRelObject->mUniqueIndexCnt,
                                    (void **) & aRelObject->mUniqueIndexPhyHandle,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        for ( i = 0; i < aRelObject->mUniqueIndexCnt; i++ )
        {
            aRelObject->mUniqueIndexPhyHandle[i]
                = ellGetIndexHandle( & aRelObject->mUniqueIndexHandle[i] );
        }
    }
    
    /**
     * Non-Unique Index 정보 획득
     */

    STL_TRY( ellGetTableNonUniqueIndexDictHandle( aStmtInfo->mTransID,
                                                  aStmtInfo->mSQLStmt->mViewSCN,
                                                  QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                  aTableHandle,
                                                  & aRelObject->mNonUniqueIndexCnt,
                                                  & aRelObject->mNonUniqueIndexHandle,
                                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    if ( aRelObject->mNonUniqueIndexCnt > 0 )
    {
        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                    STL_SIZEOF(void *) * aRelObject->mNonUniqueIndexCnt,
                                    (void **) & aRelObject->mNonUniqueIndexPhyHandle,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        for ( i = 0; i < aRelObject->mNonUniqueIndexCnt; i++ )
        {
            aRelObject->mNonUniqueIndexPhyHandle[i]
                = ellGetIndexHandle( & aRelObject->mNonUniqueIndexHandle[i] );
        }
    }
    
    
    /******************************************************
     * Check Constraint 정보 획득 
     ******************************************************/
    
    /**
     * Check Not Null Constraint 정보 획득
     */

    STL_TRY( ellGetTableCheckNotNullDictHandle( aStmtInfo->mTransID,
                                                aStmtInfo->mSQLStmt->mViewSCN,
                                                QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                aTableHandle,
                                                & aRelObject->mCheckNotNullCnt,
                                                & aRelObject->mCheckNotNullHandle,
                                                ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Check Clause Constraint 정보 획득
     * @todo Check Clause Constraint 를 구현해야 함.
     */

    STL_TRY( ellGetTableCheckClauseDictHandle( aStmtInfo->mTransID,
                                               aStmtInfo->mSQLStmt->mViewSCN,
                                               QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                               aTableHandle,
                                               & aRelObject->mCheckClauseCnt,
                                               & aRelObject->mCheckClauseHandle,
                                               ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( aRelObject->mCheckClauseCnt == 0, RAMP_ERR_NOT_IMPLEMENTED );
                                               
    /**
     * Identity Column 정보 획득
     */

    sColumnCnt = ellGetTableColumnCnt( aTableHandle );
    sColumnHandle = ellGetTableColumnDictHandle( aTableHandle );
    
    aRelObject->mHasIdentity = STL_FALSE;
    ellInitDictHandle( & aRelObject->mIdentityColumnHandle );
    for ( i = 0; i < sColumnCnt; i++ )
    {
        if ( ellGetColumnIdentityHandle( & sColumnHandle[i] ) == NULL )
        {
            /* identity column 이 아님 */
        }
        else
        {
            aRelObject->mHasIdentity = STL_TRUE;
            stlMemcpy( & aRelObject->mIdentityColumnHandle,
                       & sColumnHandle[i],
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
                      "qlvGetRelObject4Insert()" );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/** @} qlvInsert */


