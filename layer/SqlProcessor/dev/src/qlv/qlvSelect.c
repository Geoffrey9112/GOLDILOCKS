/*******************************************************************************
 * qlvSelect.c
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
 * @file qlvSelect.c
 * @brief SQL Processor Layer SELECT statement Validation
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlvSelect
 * @{
 */


/**
 * @brief SELECT 구문을 Validation 한다.
 * @param[in]      aTransID      Transaction ID
 * @param[in]      aDBCStmt      DBC Statement
 * @param[in,out]  aSQLStmt      SQL Statement
 * @param[in]      aSQLString    SQL String 
 * @param[in]      aParseTree    Parse Tree
 * @param[in]      aEnv          Environment
 */
stlStatus qlvValidateSelect( smlTransId      aTransID,
                             qllDBCStmt    * aDBCStmt,
                             qllStatement  * aSQLStmt,
                             stlChar       * aSQLString,
                             qllNode       * aParseTree,
                             qllEnv        * aEnv )
{
    qlpSelect            * sParseTree    = NULL;
    qlvInitSelect        * sInitPlan     = NULL;
    qlvInitStmtExprList  * sStmtExprList = NULL;
    qlvStmtInfo            sStmtInfo;
    qlvInitExprList        sQueryExprList;
    stlInt32               sQBIndex;
    
        
    /**********************************************************
     * Parameter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aSQLStmt->mStmtType == QLL_STMT_SELECT_TYPE ) || 
                        ( aSQLStmt->mStmtType == QLL_STMT_SELECT_FOR_UPDATE_TYPE ) ||
                        ( aSQLStmt->mStmtType == QLL_STMT_SELECT_INTO_TYPE ) ||
                        ( aSQLStmt->mStmtType == QLL_STMT_SELECT_INTO_FOR_UPDATE_TYPE ),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aParseTree->mType == QLL_STMT_SELECT_TYPE ) ||
                        ( aParseTree->mType == QLL_STMT_SELECT_FOR_UPDATE_TYPE ) ||
                        ( aParseTree->mType == QLL_STMT_SELECT_INTO_TYPE ) ||
                        ( aParseTree->mType == QLL_STMT_SELECT_INTO_FOR_UPDATE_TYPE ),
                        QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * 준비 작업
     **********************************************************/

    /**
     * select parse tree 포인터 매핑
     */

    sParseTree  = (qlpSelect *) aParseTree;


    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF( qlvInitSelect ),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    stlMemset( sInitPlan, 0x00, STL_SIZEOF( qlvInitSelect ) );

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

    sQueryExprList.mStmtExprList       = sStmtExprList;
    sQueryExprList.mPseudoColExprList  = NULL;
    sQueryExprList.mAggrExprList       = NULL;
    sQueryExprList.mNestedAggrExprList = NULL;
    

    /**
     * statement information 설정
     */

    sQBIndex = 0;

    sStmtInfo.mTransID       = aTransID;
    sStmtInfo.mHasHintError  = sParseTree->mHasHintErr;
    sStmtInfo.mQBIndex       = &sQBIndex;
    stlStrcpy( sStmtInfo.mQBPrefix, QLV_QUERY_BLOCK_NAME_PREFIX_SEL );
    sStmtInfo.mDBCStmt       = aDBCStmt;
    sStmtInfo.mSQLStmt       = aSQLStmt;
    sStmtInfo.mSQLString     = aSQLString;
    sStmtInfo.mQueryExprList = & sQueryExprList;
    
    
    /**********************************************************
     * Select Statement Validation
     **********************************************************/

    /**
     * Query Element Validation 
     */

    STL_TRY( qlvQueryNodeValidation( & sStmtInfo,
                                     STL_FALSE,
                                     sInitPlan->mInitPlan.mValidationObjList,
                                     NULL,
                                     sParseTree->mQueryNode,
                                     &sInitPlan->mQueryNode,
                                     aEnv )
             == STL_SUCCESS );

    ((qlvInitPlan *) sInitPlan)->mHasHintError = sStmtInfo.mHasHintError;
    

    /**
     * Into Clause 정보 초기화
     */

    sInitPlan->mIntoTargetCnt   = 0;
    sInitPlan->mIntoTargetArray = NULL;

    /**********************************************************
     * Result Set 의 Cursor Property 설정 
     **********************************************************/

    STL_TRY( qlvValidate4ResultSetDesc( aSQLStmt,
                                        sParseTree,
                                        sInitPlan,
                                        & sStmtInfo,
                                        aEnv )
             == STL_SUCCESS );
    
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

    if( ( aSQLStmt->mStmtType == QLL_STMT_SELECT_TYPE ) ||
        ( aSQLStmt->mStmtType == QLL_STMT_SELECT_FOR_UPDATE_TYPE ) )
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
                         
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief TARGET node validation
 * @param[in]      aStmtInfo             Stmt Information
 * @param[in]      aValidationObjList    Validation Object List
 * @param[in]      aRefTableList         Reference Table List
 * @param[in]      aAllowedAggrDepth     Allowed Aggregation Depth
 * @param[in]      aTargetSource         Target Source
 * @param[in,out]  aTargetCode           Target Code
 * @param[in]      aIterationTimeScope   Iteration Time Scope (허용 가능한 Iteration Time)
 * @param[in]      aIterationTime        상위 노드의 Iteration Time
 * @param[in]      aEnv                  Environment
 */
stlStatus qlvValidateTarget( qlvStmtInfo       * aStmtInfo,
                             ellObjectList     * aValidationObjList,
                             qlvRefTableList   * aRefTableList,
                             stlInt32            aAllowedAggrDepth,
                             qlpTarget         * aTargetSource,
                             qlvInitTarget     * aTargetCode,
                             dtlIterationTime    aIterationTimeScope,
                             dtlIterationTime    aIterationTime,
                             qllEnv            * aEnv )
{
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValidationObjList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefTableList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTargetSource != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTargetSource->mExpr->mType == QLL_VALUE_EXPR_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    
    /**
     * Valdate Value Expression
     */

    STL_TRY( qlvValidateValueExpr( aStmtInfo,
                                   QLV_EXPR_PHRASE_TARGET,
                                   aValidationObjList,
                                   aRefTableList,
                                   STL_FALSE,  /* Atomic */
                                   STL_FALSE,  /* Row Expr */
                                   aAllowedAggrDepth,
                                   aTargetSource->mExpr,
                                   & aTargetCode->mExpr,
                                   NULL,
                                   KNL_BUILTIN_FUNC_INVALID,
                                   aIterationTimeScope,
                                   aIterationTime,
                                   QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    /**
     * Rewrite Expression
     */

    STL_TRY( qlvRewriteExpr( aTargetCode->mExpr,
                             DTL_ITERATION_TIME_FOR_EACH_AGGREGATION,
                             aStmtInfo->mQueryExprList,
                             & aTargetCode->mExpr,
                             QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                             aEnv )
             == STL_SUCCESS );

    /**
     * Target Name 설정
     */

    STL_TRY( qlvSetTargetName( aStmtInfo,
                               aTargetSource,
                               aTargetCode,
                               aEnv )
             == STL_SUCCESS );

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Target Name 을 설정한다.
 * @param[in]     aStmtInfo      Stmt Information
 * @param[in]     aTargetSource  Target Parse Tree
 * @param[in,out] aTargetCode    Target Init Plan
 * @param[in]     aEnv           Environment
 * @remarks
 */
stlStatus qlvSetTargetName( qlvStmtInfo   * aStmtInfo,
                            qlpTarget     * aTargetSource,
                            qlvInitTarget * aTargetCode,
                            qllEnv        * aEnv )
{
    stlInt32          sNameLength    = 0;
    stlInt32          sLoop          = 0;
    stlBool           sIsQuoted      = STL_FALSE;
    stlBool           sIsSingleQuote = STL_FALSE;
    stlBool           sIsDoubleQuote = STL_FALSE;
    stlChar         * sFence         = NULL;
    stlChar         * sCurrChar      = NULL;
    stlInt32          sToUpper       = 'A' - 'a';

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTargetSource != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTargetSource->mName != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTargetCode != NULL, QLL_ERROR_STACK(aEnv) );
    
    /**
     * Target Name 설정
     */

    aTargetCode->mDisplayName = NULL;
    
    if( aTargetCode->mExpr != NULL )
    {
        if( ( aTargetCode->mExpr->mColumnName != NULL ) &&
            ( aTargetSource->mNodePos == aTargetSource->mNamePos ) )
        {
            /* alias 가 없는 column 또는 inner column 인 경우 */
            aTargetCode->mDisplayName = aTargetCode->mExpr->mColumnName;
            aTargetCode->mDisplayPos  = aTargetCode->mExpr->mPosition;
        }
    }
    
    if( aTargetCode->mDisplayName == NULL )
    {
        sNameLength = stlStrlen( aTargetSource->mName );

        if( sNameLength > STL_MAX_SQL_IDENTIFIER_LENGTH )
        {
            sNameLength = STL_MAX_SQL_IDENTIFIER_LENGTH;
        }
        else
        {
            /* Do Nothing */
        }
    
        sFence       = aTargetSource->mName + sNameLength - 1;

        /* truncate trailing spaces & marks */
        while( sFence > aTargetSource->mName )
        {
            if( (*sFence == ' ')  ||
                (*sFence == '\0') ||
                (*sFence == '\n') ||
                (*sFence == '\t') ||
                (*sFence == '\r') )
            {
                sFence--;
            }
            else
            {
                break;
            }
        }

        sNameLength = sFence - aTargetSource->mName + 1;

        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                    sNameLength + 1,
                                    (void **) & aTargetCode->mDisplayName,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );


        stlMemcpy( aTargetCode->mDisplayName, aTargetSource->mName, sNameLength );
        aTargetCode->mDisplayName[ sNameLength ] = '\0';
        aTargetCode->mDisplayPos = aTargetSource->mNodePos;
    }
    

    /**
     * non-quoted identifier 대문자로 변환
     */

    if ( aTargetSource->mNodePos != aTargetSource->mNamePos )
    {
        /**
         * Alias Name 이 있는 경우
         * - Parser 에서 대문자 처리 또는 Double Quoted 처리가 완료됨.
         * - Nothing To Do
         */

        aTargetCode->mDisplayPos = aTargetSource->mNamePos;
        aTargetCode->mAliasName = aTargetCode->mDisplayName;
    }
    else
    {
        aTargetCode->mAliasName  = NULL;
        
        /**
         * Alias Name 이 없는 경우
         */
        sCurrChar = aTargetCode->mDisplayName;
    
        for( sLoop = 0 ; sLoop <  sNameLength ; sLoop++ )
        {
            switch( *sCurrChar )
            {
                case '\'' :
                    {
                        if( sIsSingleQuote == STL_TRUE )
                        {
                            sIsQuoted      = STL_FALSE;
                            sIsSingleQuote = STL_FALSE;
                        }
                        else if( sIsDoubleQuote == STL_TRUE )
                        {
                            sIsSingleQuote = STL_FALSE;
                        }
                        else
                        {
                            sIsSingleQuote = STL_TRUE;
                            sIsQuoted      = STL_TRUE;
                        }
                        break;
                    }

                case '\"' :
                    {
                        if( sIsDoubleQuote == STL_TRUE )
                        {
                            sIsQuoted      = STL_FALSE;
                            sIsDoubleQuote = STL_FALSE;
                        }
                        else if( sIsSingleQuote == STL_TRUE )
                        {
                            sIsDoubleQuote = STL_FALSE;
                        }
                        else
                        {
                            sIsDoubleQuote = STL_TRUE;
                            sIsQuoted      = STL_TRUE;
                        }
                        break;
                    }
                
                default :
                    {
                        if( sIsQuoted == STL_FALSE )
                        {
                            if( ( *sCurrChar >= 'a' ) && ( *sCurrChar <= 'z' ) )
                            {
                                *sCurrChar += sToUpper;
                            }
                            else
                            {
                                /* Do Nohting */
                            }
                        }
                        else
                        {
                            /* Do Nohting */
                        }
                    }
            }
        
            sCurrChar++;
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Result Set 정보를 위한 Validation 을 수행한다.
 * @param[in] aSQLStmt       SQL Statement
 * @param[in] aSelectTree    SELECT parse tree
 * @param[in] aInitPlan      SELECT init plan
 * @param[in] aStmtInfo      Statement Information
 * @param[in] aEnv           Environment
 */

stlStatus qlvValidate4ResultSetDesc( qllStatement  * aSQLStmt,
                                     qlpSelect     * aSelectTree,
                                     qlvInitSelect * aInitPlan,
                                     qlvStmtInfo   * aStmtInfo,
                                     qllEnv        * aEnv )
{
    qlvInitQuerySpecNode * sQuerySpec = NULL;
    qlvRefTableList      * sScanTableList = NULL;
    ellCursorProperty    * sResultSetProperty = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSelectTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    
    /**
     * Updatable 정보 획득
     */

    if ( aSelectTree->mUpdatability == NULL )
    {
        aInitPlan->mUpdatableMode = QLP_UPDATABILITY_NA;
    }
    else
    {
        aInitPlan->mUpdatableMode = aSelectTree->mUpdatability->mUpdatableMode;
    }
    
    /**
     * Init Plan 의 Result Set Cursor Property 정보 설정
     */
    
    STL_TRY( qlvSetSelectResultCursorProperty( aStmtInfo,
                                               aInitPlan,
                                               aEnv )
             == STL_SUCCESS );

    sResultSetProperty = & aInitPlan->mInitPlan.mResultCursorProperty;
    
    /************************************************
     * Sensitivity
     ************************************************/

    if ( sResultSetProperty->mSensitivity == ELL_CURSOR_SENSITIVITY_SENSITIVE )
    {
        STL_DASSERT( aInitPlan->mQueryNode->mType == QLV_NODE_TYPE_QUERY_SPEC );
        
        sQuerySpec = (qlvInitQuerySpecNode *) aInitPlan->mQueryNode;
            
        /**
         * Scan Table List 초기화 
         */

        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                    STL_SIZEOF( qlvRefTableList ),
                                    (void **) & sScanTableList,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sScanTableList->mCount = 0;
        sScanTableList->mHead  = NULL;

        /**
         * FROM list 에서 sensitive table 의 list 를 구축
         */
    
        STL_TRY( qlvSetCursorItemListByFROM( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                             sQuerySpec->mTableNode,
                                             sScanTableList,
                                             aEnv )
                 == STL_SUCCESS );

        aInitPlan->mScanTableList = sScanTableList;
    }
    else
    {
        aInitPlan->mScanTableList = NULL;
    }
    
    /************************************************
     * Updatability 
     ************************************************/
    
    /**
     * Query 의 Result Set Property 속성이 FOR UPDATE 인 경우 Lock Item 정보 획득
     */

    if ( sResultSetProperty->mUpdatability == ELL_CURSOR_UPDATABILITY_FOR_UPDATE )
    {
        STL_DASSERT( aInitPlan->mQueryNode->mType == QLV_NODE_TYPE_QUERY_SPEC );

        sQuerySpec = (qlvInitQuerySpecNode *) aInitPlan->mQueryNode;
        
        /**
         * Version Conflict Resolution 을 위한 Serializability Action 을 설정한다.
         */
    
        if ( ( sQuerySpec->mWhereExpr == NULL ) &&
             ( sQuerySpec->mOrderBy == NULL ) &&
             ( sQuerySpec->mTableNode->mType == QLV_NODE_TYPE_BASE_TABLE ) )
        {
            /**
             * WHERE 절, JOIN, ORDER BY 절이 존재하지 않는 경우
             */

            aInitPlan->mSerialAction = QLV_SERIAL_NO_RETRY;
        }
        else
        {
            /**
             * WHERE 절 또는 JOIN 또는 ORDER BY 절이 존재하는 경우
             */

            aInitPlan->mSerialAction = QLV_SERIAL_RETRY_ONLY;
        }
        
        /**
         * FOR UPDATE 에 대한 Lock Information 을 구축한다.
         */
        
        STL_TRY( qlvSetLockInfo4ForUpdate(
                     aStmtInfo,
                     aSelectTree->mUpdatability,
                     aInitPlan,
                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /**
         * Read Only SELECT : Lock 관련 정보가 없음
         */

        aInitPlan->mLockWaitMode     = QLL_CURSOR_LOCK_WAIT_NA;
        aInitPlan->mLockTimeInterval = SML_LOCK_TIMEOUT_INVALID;
        aInitPlan->mLockTableList    = NULL;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Query 가 Updatable Query 인지 여부
 * @param[in]   aInitPlan     Init Plan
 * @return
 * stlBool - Updatable Query 여부
 * @remarks
 */
stlBool qlvIsUpdatableQuery( qlvInitSelect * aInitPlan )
{
    stlBool sUpdatable = STL_FALSE;
    
    qlvInitQuerySpecNode * sQuerySpec = NULL;
    
    /**
     * Paramter Validation
     */

    STL_DASSERT( aInitPlan != NULL );
    
    sUpdatable = STL_TRUE;
    
    while ( 1 )
    {
        /**
         * Set 연산자가 존재하지 않아야 한다.
         * (X) SELECT * FROM t1 UNION ALL SELECT * FROM t2;
         */

        if ( aInitPlan->mQueryNode->mType == QLV_NODE_TYPE_QUERY_SET )
        {
            sUpdatable = STL_FALSE;
            break;
        }

        sQuerySpec = (qlvInitQuerySpecNode *) aInitPlan->mQueryNode;
        
        /**
         * 최상위 query 에 DISTINCT 가 없어야 한다.
         * (X) SELECT DISTINCT * FROM t1;
         */

        if ( sQuerySpec->mIsDistinct == STL_TRUE )
        {
            sUpdatable = STL_FALSE;
            break;
        }
        
        /**
         * 최상위 query 에 GROUP BY, HAVING, aggregation function 이 존재하면 안된다.
         * (X) SELECT MAX(c1) FROM t1;
         */

        if ( sQuerySpec->mHasGroupOper == STL_TRUE )
        {
            /**
             * aggregation function 이 존재함
             */
            
            sUpdatable = STL_FALSE;
            break;
        }
        
        /**
         * FROM 절에 나열된 table 들에 적어도 하나의 updatable column 이 존재해야 한다.
         */

        sUpdatable = qlvHasUpdatableColumnInFROM( sQuerySpec->mTableNode );
        
        break;
    }

    /**
     * Output 설정
     */

    return sUpdatable;
}


/**
 * @brief FROM 절에 Updatable Column 이 존재하는 지 확인
 * @param[in]  aTableNode   Table Node
 * @return
 * stlBool - Updatable Column 이 하나라도 존재하는 지 확인 
 * @remarks
 */
stlBool qlvHasUpdatableColumnInFROM( qlvInitNode * aTableNode )
{
    stlBool                sHasUpdatable = STL_FALSE;
    
    qlvInitBaseTableNode * sBaseTable = NULL;
//    qlvInitSubTableNode  * sViewTable = NULL;
    qlvInitJoinTableNode * sJoinTable = NULL;
    
    /**
     * Parameter Validation
     */

    STL_DASSERT( aTableNode != NULL );

    /**
     * Table Tree 를 순회하며 하나의 Updatable Column 이라도 존재하는지 검사
     */
    
    switch ( aTableNode->mType )
    {
        case QLV_NODE_TYPE_BASE_TABLE:
            {
                sBaseTable = (qlvInitBaseTableNode *) aTableNode;

                if ( ellIsUpdatableTable( & sBaseTable->mTableHandle ) == STL_TRUE )
                {
                    /**
                     * Updatable Table 임 
                     */
                    
                    sHasUpdatable = STL_TRUE;
                }
                else
                {
                    sHasUpdatable = STL_FALSE;
                }
                break;
            }
        case QLV_NODE_TYPE_SUB_TABLE:
            {
//                sViewTable = (qlvInitSubTableNode *) aTableNode;

                /**
                 * @todo Updatable View 가 되면 이를 확장해야 함
                 */
                sHasUpdatable = STL_FALSE;
                break;
            }
        case QLV_NODE_TYPE_JOIN_TABLE:
            {
                sJoinTable = (qlvInitJoinTableNode *) aTableNode;

                switch ( sJoinTable->mJoinType )
                {
                    case QLV_JOIN_TYPE_NONE:
                    case QLV_JOIN_TYPE_CROSS:
                        {
                            /**
                             * Left 테이블 검사
                             */
                            
                            sHasUpdatable =
                                qlvHasUpdatableColumnInFROM( sJoinTable->mLeftTableNode );

                            if ( sHasUpdatable == STL_TRUE )
                            {
                                /**
                                 * 하나라도 존재하는 지 확인하면 됨
                                 */
                                STL_THROW( RAMP_HAS_UPDATABLE_COLUMN );
                            }

                            /**
                             * Right 테이블 검사
                             */
                            
                            sHasUpdatable =
                                qlvHasUpdatableColumnInFROM( sJoinTable->mRightTableNode );
                            
                            break;
                        }
                    case QLV_JOIN_TYPE_INNER:
                        {
                            if ( sJoinTable->mJoinSpec->mNamedColumnList != NULL )
                            {
                                /**
                                 * USING 구문이 사용된 INNER JOIN 은 Cross Join 이 아니다.
                                 */
                                sHasUpdatable = STL_FALSE;
                            }
                            else
                            {
                                /**
                                 * Left 테이블 검사
                                 */
                                
                                sHasUpdatable =
                                    qlvHasUpdatableColumnInFROM( sJoinTable->mLeftTableNode );
                                
                                if ( sHasUpdatable == STL_TRUE )
                                {
                                    /**
                                     * 하나라도 존재하는 지 확인하면 됨
                                     */
                                    STL_THROW( RAMP_HAS_UPDATABLE_COLUMN );
                                }
                                
                                /**
                                 * Right 테이블 검사
                                 */
                                
                                sHasUpdatable =
                                    qlvHasUpdatableColumnInFROM( sJoinTable->mRightTableNode );
                            }
                            
                            break;
                        }
                    case QLV_JOIN_TYPE_LEFT_SEMI:
                    case QLV_JOIN_TYPE_INVERTED_LEFT_SEMI:
                    case QLV_JOIN_TYPE_LEFT_ANTI_SEMI:
                    case QLV_JOIN_TYPE_LEFT_ANTI_SEMI_NA:
                        {
                            /**
                             * Left 테이블 검사
                             */
                            
                            sHasUpdatable =
                                qlvHasUpdatableColumnInFROM( sJoinTable->mLeftTableNode );

                            break;
                        }
                    case QLV_JOIN_TYPE_RIGHT_SEMI:
                    case QLV_JOIN_TYPE_INVERTED_RIGHT_SEMI:
                    case QLV_JOIN_TYPE_RIGHT_ANTI_SEMI:
                    case QLV_JOIN_TYPE_RIGHT_ANTI_SEMI_NA:
                        {
                            /**
                             * Right 테이블 검사
                             */
                            
                            sHasUpdatable =
                                qlvHasUpdatableColumnInFROM( sJoinTable->mRightTableNode );
                            
                            break;
                        }
                    case QLV_JOIN_TYPE_LEFT_OUTER:
                    case QLV_JOIN_TYPE_RIGHT_OUTER:
                    case QLV_JOIN_TYPE_FULL_OUTER:
                        {
                            /**
                             * Outer Join 의 모든 컬럼은 Updatable Column 이 아님
                             * - LEFT OUTER 의 left table 은 가능할 수도 있으나,
                             * - Lock Version Conflict 발생시
                             *   ON 절에 부합하는 새로운 결과를 추출할 수가 없다.
                             */
                            
                            sHasUpdatable = STL_FALSE;
                            break;
                        }
                    default:
                        STL_ASSERT(0);
                        break;
                }
                break;
            }
        default:
            {
                STL_ASSERT(0);
                break;
            }
    }

    /**
     * Output 설정
     */

    STL_RAMP( RAMP_HAS_UPDATABLE_COLUMN );
    
    return sHasUpdatable;
}



/**
 * @brief SELECT query 의 Result Set 의 Cursor Property 를 설정한다.
 * @param[in]  aStmtInfo   Stmt Information
 * @param[in]  aInitPlan   SELECT Init Plan
 * @param[in]  aEnv        Environment
 * @remarks
 */
stlStatus qlvSetSelectResultCursorProperty( qlvStmtInfo   * aStmtInfo,
                                            qlvInitSelect * aInitPlan,
                                            qllEnv        * aEnv )
{
    stlBool             sIsUpdatableQuery = STL_FALSE;
    qllStmtCursorProperty * sStmtProperty = NULL;
    ellCursorProperty     * sResultSetProperty = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sStmtProperty      = & aStmtInfo->mSQLStmt->mStmtCursorProperty;
    sResultSetProperty = & aInitPlan->mInitPlan.mResultCursorProperty;
    
    /*****************************************************
     * Updatable Query 여부 검사
     *****************************************************/

    /**
     * Updatable Query 여부를 판단할 필요가 있는
     * Cursor 속성이나 구문일 경우 Updatable Query 인지의 정보를 획득한다.
     */
    
    if ( (sStmtProperty->mProperty.mSensitivity == ELL_CURSOR_SENSITIVITY_SENSITIVE) ||
         (sStmtProperty->mProperty.mSensitivity == ELL_CURSOR_SENSITIVITY_ASENSITIVE) ||
         (sStmtProperty->mProperty.mUpdatability == ELL_CURSOR_UPDATABILITY_FOR_UPDATE) )
    {
        /**
         * SQL Statement 의 Cursor 속성이 SENSITIVE 이거나 FOR_UPDATE 인 경우
         */

        sIsUpdatableQuery = qlvIsUpdatableQuery( aInitPlan );
    }
    else
    {
        if ( aInitPlan->mUpdatableMode == QLP_UPDATABILITY_FOR_UPDATE )
        {
            /**
             * SQL 구문 자체가 SELECT FOR UPDATE 인 경우
             */
            
            sIsUpdatableQuery = qlvIsUpdatableQuery( aInitPlan );
        }
        else
        {
            sIsUpdatableQuery = STL_FALSE;
        }
    }

    /*****************************************************
     * Result Set 의 Standard Type 설정 
     *****************************************************/

    sResultSetProperty->mStandardType = sStmtProperty->mProperty.mStandardType;

    /*****************************************************
     * Result Set 의 Sensitivity 설정 
     *****************************************************/

    switch ( sResultSetProperty->mStandardType )
    {
        case ELL_CURSOR_STANDARD_ISO:
            {
                switch (sStmtProperty->mProperty.mSensitivity)
                {
                    case ELL_CURSOR_SENSITIVITY_INSENSITIVE:
                        sResultSetProperty->mSensitivity = ELL_CURSOR_SENSITIVITY_INSENSITIVE;
                        break;
                    case ELL_CURSOR_SENSITIVITY_SENSITIVE:
                        if ( sIsUpdatableQuery == STL_TRUE )
                        {
                            sResultSetProperty->mSensitivity = ELL_CURSOR_SENSITIVITY_SENSITIVE;
                        }
                        else
                        {
                            STL_TRY_THROW ( sStmtProperty->mIsDbcCursor == STL_TRUE, RAMP_ERR_NOT_SENSITIVE_QUERY );
                            sResultSetProperty->mSensitivity = ELL_CURSOR_SENSITIVITY_INSENSITIVE;
                        }
                        break;
                    case ELL_CURSOR_SENSITIVITY_ASENSITIVE:
                        if ( sIsUpdatableQuery == STL_TRUE )
                        {
                            sResultSetProperty->mSensitivity = ELL_CURSOR_SENSITIVITY_SENSITIVE;
                        }
                        else
                        {
                            sResultSetProperty->mSensitivity = ELL_CURSOR_SENSITIVITY_INSENSITIVE;
                        }
                        break;
                    default:
                        STL_ASSERT(0);
                        break;
                }
                break;
            }
        case ELL_CURSOR_STANDARD_ODBC_STATIC:
            {
                /**
                 * STATIC cursor 는 SENSITIVE 할 수 없다.
                 */
                sResultSetProperty->mSensitivity = ELL_CURSOR_SENSITIVITY_INSENSITIVE;
                break;
            }
        case ELL_CURSOR_STANDARD_ODBC_KEYSET:
            {
                /**
                 * KEYSET cursor
                 */
                switch (sStmtProperty->mProperty.mSensitivity)
                {
                    case ELL_CURSOR_SENSITIVITY_INSENSITIVE:
                        sResultSetProperty->mSensitivity = ELL_CURSOR_SENSITIVITY_INSENSITIVE;
                        break;
                    case ELL_CURSOR_SENSITIVITY_SENSITIVE:
                    case ELL_CURSOR_SENSITIVITY_ASENSITIVE:
                        if( sIsUpdatableQuery == STL_TRUE )
                        {
                            sResultSetProperty->mSensitivity = ELL_CURSOR_SENSITIVITY_SENSITIVE;
                        }
                        else
                        {
                            sResultSetProperty->mSensitivity = ELL_CURSOR_SENSITIVITY_INSENSITIVE;
                        }
                        break;
                    default:
                        STL_ASSERT(0);
                        break;
                }
                break;
            }
        case ELL_CURSOR_STANDARD_ODBC_DYNAMIC:
            {
                /**
                 * DYNAMIC cursor 는 반드시 SENSITIVIE 해야 한다.
                 */
                
                sResultSetProperty->mSensitivity = ELL_CURSOR_SENSITIVITY_SENSITIVE;
                STL_TRY_THROW( sIsUpdatableQuery == STL_TRUE, RAMP_ERR_NOT_SENSITIVE_QUERY );
                break;
            }
        default:
            {
                STL_ASSERT(0);
                break;
            }
    }
    
    /*****************************************************
     * Result Set 의 Scrollability 설정 
     *****************************************************/
    
    sResultSetProperty->mScrollability = sStmtProperty->mProperty.mScrollability;

    /*****************************************************
     * Result Set 의 Updatability 설정
     *****************************************************/

    /**
     * Result Set 의 Updatability 설정
     *
     * --------------------------------------------------------------------
     *  SELECT 구문              |   Driver READ_ONLY   |  Driver LOCK (UPDATABLE)
     * --------------------------------------------------------------------
     * updatable query          |   READ ONLY          | UPDATABLE
     * non-updatable query      |   READ ONLY          | READ ONLY
     * SELECT .. FOR READ ONLY  |   READ ONLY          | READ ONLY
     * SELECT .. FOR UPDATE     |   UPDATABLE          | UPDATABLE 
     * --------------------------------------------------------------------
     */

    if ( aInitPlan->mUpdatableMode == QLP_UPDATABILITY_NA )
    {
        /**
         * SELECT 문장에 FOR READ ONLY 나 FOR UPDATE 가 명시되지 않은 경우
         */
        
        if ( sStmtProperty->mProperty.mUpdatability == ELL_CURSOR_UPDATABILITY_FOR_READ_ONLY )
        {
            sResultSetProperty->mUpdatability = ELL_CURSOR_UPDATABILITY_FOR_READ_ONLY;
        }
        else
        {
            if ( sIsUpdatableQuery == STL_TRUE )
            {
                sResultSetProperty->mUpdatability = ELL_CURSOR_UPDATABILITY_FOR_UPDATE;
            }
            else
            {
                sResultSetProperty->mUpdatability = ELL_CURSOR_UPDATABILITY_FOR_READ_ONLY;
            }
        }
    }
    else
    {
        if ( aInitPlan->mUpdatableMode == QLP_UPDATABILITY_FOR_UPDATE )
        {
            /**
             * SELECT .. FOR UPDATE 인 경우
             */
            
            STL_TRY_THROW( sIsUpdatableQuery == STL_TRUE, RAMP_ERR_NOT_UPDATABLE_QUERY );
            
            sResultSetProperty->mUpdatability = ELL_CURSOR_UPDATABILITY_FOR_UPDATE;
        }
        else
        {
            /**
             * SELECT .. FOR READ ONLY 인 경우
             */
            
            sResultSetProperty->mUpdatability = ELL_CURSOR_UPDATABILITY_FOR_READ_ONLY;
        }
    }

    /*****************************************************
     * Result Set 의 Holdability 설정
     *****************************************************/

    switch ( sStmtProperty->mProperty.mHoldability )
    {
        case ELL_CURSOR_HOLDABILITY_UNSPECIFIED:
            {
                if ( sResultSetProperty->mUpdatability
                     == ELL_CURSOR_UPDATABILITY_FOR_UPDATE )
                {
                    sResultSetProperty->mHoldability
                        = ELL_CURSOR_HOLDABILITY_WITHOUT_HOLD;
                }
                else
                {
                    sResultSetProperty->mHoldability
                        = ELL_CURSOR_HOLDABILITY_WITH_HOLD;
                }
                break;
            }
        case ELL_CURSOR_HOLDABILITY_WITH_HOLD:
        case ELL_CURSOR_HOLDABILITY_WITHOUT_HOLD:
            {
                sResultSetProperty->mHoldability = sStmtProperty->mProperty.mHoldability;
                break;
            }
        default:
            {
                STL_ASSERT(0);
                break;
            }
    }
    
    if ( sResultSetProperty->mHoldability == ELL_CURSOR_HOLDABILITY_WITH_HOLD )
    {
        /**
         * WITH HOLD Cursor 는 Read Only Cursor 여야 함.
         */

        if ( sResultSetProperty->mUpdatability == ELL_CURSOR_UPDATABILITY_FOR_READ_ONLY )
        {
            /* no problem */
        }
        else
        {
            if ( sStmtProperty->mIsDbcCursor == STL_TRUE )
            {
                sResultSetProperty->mHoldability = ELL_CURSOR_HOLDABILITY_WITHOUT_HOLD;
            }
            else
            {
                STL_THROW( RAMP_ERR_INVALID_HOLDABILITY );
            }
        }
    }

    /*****************************************************
     * @todo Result Set 의 Returnability 설정
     *****************************************************/

    /**
     * @todo Returnability 설정
     */
    
    sResultSetProperty->mReturnability = sStmtProperty->mProperty.mReturnability;

    /*****************************************************
     * 미구현 사항 
     *****************************************************/

    switch ( sResultSetProperty->mStandardType )
    {
        case ELL_CURSOR_STANDARD_ISO:
        case ELL_CURSOR_STANDARD_ODBC_STATIC:
        case ELL_CURSOR_STANDARD_ODBC_KEYSET:
            break;
        case ELL_CURSOR_STANDARD_ODBC_DYNAMIC:
            STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
            break;
        default:
            STL_ASSERT(0);
            break;
    }

    switch ( sResultSetProperty->mReturnability )
    {
        case ELL_CURSOR_RETURNABILITY_WITH_RETURN:
            STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
            break;
        case ELL_CURSOR_RETURNABILITY_WITHOUT_RETURN:
            break;
        default:
            STL_ASSERT(0);
            break;
    }
    
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_SENSITIVE_QUERY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_QUERY_IS_NOT_UPDATABLE,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_NOT_UPDATABLE_QUERY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_QUERY_IS_NOT_UPDATABLE,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_INVALID_HOLDABILITY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_CURSOR_PROPERTY,
                      "holdable cursor should be read only cursor",
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "qlvSetSelectResultCursorProperty()" );
    }
    
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief FOR UPDATE 에 대한 Lock Information 을 구축한다.
 * @param[in]  aStmtInfo      Stmt Information
 * @param[in]  aForUpdateTree FOR UPDATE TREE (nullable)
 * @param[in]  aInitPlan      SELECT Init Plan
 * @param[in]  aEnv           Environment
 * @remarks
 */
stlStatus qlvSetLockInfo4ForUpdate( qlvStmtInfo        * aStmtInfo,
                                    qlpUpdatableClause * aForUpdateTree,
                                    qlvInitSelect      * aInitPlan,
                                    qllEnv             * aEnv )
{
    stlChar           * sEndPtr;

    qlvRefTableItem   * sLockTableItem = NULL;  
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    /* FOR UPDATE 가 없더라도 Result Set 이 update 일 수 있다. */
    /* STL_PARAM_VALIDATE( aForUpdateTree != NULL, QLL_ERROR_STACK(aEnv) ); */
    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Lock Wait 정보와 Lock Item 정보 구축
     */
    
    if ( aForUpdateTree == NULL )
    {
        /**
         * FOR UPDATE 는 기술되지 않았으나,
         * statement 속성이 updatable 이고 updatable query 로 결정된 경우
         */

        /**
         * Lock Wait 의 기본값으로 설정한다.
         * - FOR UPDATE WAIT 과 동일함 
         */
        
        aInitPlan->mLockWaitMode     = QLL_CURSOR_LOCK_WAIT_INFINITE;
        aInitPlan->mLockTimeInterval = SML_LOCK_TIMEOUT_INFINITE;

        /**
         * Lock Item 목록 결정 
         */
        
        STL_TRY( qlvSetLockItemList4ForUpdateOnly( aStmtInfo,
                                                   aInitPlan,
                                                   aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /**
         * Lock Wait 정보 설정 
         */

        if ( aForUpdateTree->mLockWait == NULL )
        {
            /**
             * Lock Wait 정보가 기술되지 않은 경우
             * - FOR UPDATE WAIT 과 동일함 
             */
            
            aInitPlan->mLockWaitMode     = QLL_CURSOR_LOCK_WAIT_INFINITE;
            aInitPlan->mLockTimeInterval = SML_LOCK_TIMEOUT_INFINITE;
        }
        else
        {
            /**
             * Lock Wait 정보가 기술된 경우
             */
            
            switch ( aForUpdateTree->mLockWait->mLockWaitMode )
            {
                case  QLP_FOR_UPDATE_LOCK_WAIT:
                    {
                        /**
                         * Execution 시점에 lock 을 획득
                         */
                        
                        if ( aForUpdateTree->mLockWait->mWaitSecond == NULL )
                        {
                            /**
                             * FOR UPDATE WAIT
                             */
                            
                            aInitPlan->mLockWaitMode     = QLL_CURSOR_LOCK_WAIT_INFINITE;
                            aInitPlan->mLockTimeInterval = SML_LOCK_TIMEOUT_INFINITE;
                        }
                        else
                        {
                            /**
                             * FOR UPDATE WAIT second
                             */

                            aInitPlan->mLockWaitMode     = QLL_CURSOR_LOCK_WAIT_INTERVAL;
                            
                            STL_TRY( stlStrToInt64(
                                         QLP_GET_PTR_VALUE( aForUpdateTree->mLockWait->mWaitSecond ),
                                         STL_NTS,
                                         &sEndPtr,
                                         10,
                                         &aInitPlan->mLockTimeInterval,
                                         KNL_ERROR_STACK(aEnv) )
                                     == STL_SUCCESS );
                            
                            /**
                             * WAIT time 의 제약 검사
                             */
                            
                            STL_TRY_THROW(
                                (aInitPlan->mLockTimeInterval >= QLV_MIN_WAIT_INTERVAL) &&
                                (aInitPlan->mLockTimeInterval <= QLV_MAX_WAIT_INTERVAL),
                                RAMP_ERR_INVALID_WAIT_INTERVAL );
                        }
                        break;
                    }
                case QLP_FOR_UPDATE_LOCK_NOWAIT:
                    {
                        /**
                         * FOR UPDATE NOWAIT
                         * - Execution 시점에 Lock Wait
                         */
                        
                        aInitPlan->mLockWaitMode     = QLL_CURSOR_LOCK_WAIT_NOWAIT;
                        aInitPlan->mLockTimeInterval = SML_LOCK_TIMEOUT_NOWAIT;

                        break;
                    }
                case QLP_FOR_UPDATE_LOCK_SKIP_LOCKED:
                    {
                        /**
                         * FOR UPDATE SKIP LOCKED
                         * - Fetch 시점에 Lock Wait
                         */
                        
                        aInitPlan->mLockWaitMode     = QLL_CURSOR_LOCK_WAIT_SKIP_LOCKED;
                        aInitPlan->mLockTimeInterval = SML_LOCK_TIMEOUT_NOWAIT;

                        /**
                         * @todo 향후 Queue 등이 지원되면 개발해야 함.
                         */
                        
                        STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                        
                        break;
                    }
                default:
                    {
                        STL_ASSERT(0);
                        break;
                    }
            }
        }

        /**
         * Lock Item 목록 결정 
         */
        
        if ( aForUpdateTree->mUpdateColumnList == NULL )
        {
            /**
             * FOR UPDATE 만 기술된 경우
             */
            
            STL_TRY( qlvSetLockItemList4ForUpdateOnly( aStmtInfo,
                                                       aInitPlan,
                                                       aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /**
             * FOR UDPATE OF columns 로 기술된 경우
             */

            STL_TRY( qlvSetLockItemList4ForUpdateColumns( aStmtInfo,
                                                          aForUpdateTree->mUpdateColumnList,
                                                          aInitPlan,
                                                          aEnv )
                     == STL_SUCCESS );
        }
    }

    /**
     * LOCK ON TABLE 권한 검사
     */

    sLockTableItem = aInitPlan->mLockTableList->mHead;

    while ( sLockTableItem != NULL )
    {
        STL_TRY( qlaCheckTablePriv( aStmtInfo->mTransID,
                                    aStmtInfo->mDBCStmt,
                                    aStmtInfo->mSQLStmt,
                                    ELL_TABLE_PRIV_ACTION_LOCK,
                                    & ((qlvInitBaseTableNode*)sLockTableItem->mTableNode)->mTableHandle,
                                    NULL, /* aColumnList */
                                    aEnv )
                 == STL_SUCCESS );
        
        sLockTableItem = sLockTableItem->mNext;
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_WAIT_INTERVAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_WAIT_INTERVAL,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aForUpdateTree->mLockWait->mWaitSecond->mNodePos,
                                           aEnv ),
                      ELL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      ELL_ERROR_STACK(aEnv),
                      "qlvSetLockInfo4ForUpdate()" );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief FOR UPDATE 만을 위한 Lock Item List 를 설정한다.
 * @param[in]  aStmtInfo      Stmt Information
 * @param[in]  aInitPlan      SELECT Init Plan 
 * @param[in]  aEnv           Environment
 * @remarks
 */
stlStatus qlvSetLockItemList4ForUpdateOnly( qlvStmtInfo        * aStmtInfo,
                                            qlvInitSelect      * aInitPlan,
                                            qllEnv             * aEnv )
{
    qlvInitQuerySpecNode * sQuerySpec = NULL;
    qlvRefTableList      * sLockTableList = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );

    sQuerySpec = (qlvInitQuerySpecNode *) aInitPlan->mQueryNode;
        
    /**
     * Lock Table List 초기화 
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                STL_SIZEOF( qlvRefTableList ),
                                (void **) & sLockTableList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sLockTableList->mCount = 0;
    sLockTableList->mHead  = NULL;

    /**
     * FROM list 에서 lockable table 의 list 를 구축
     */
    
    STL_TRY( qlvSetCursorItemListByFROM( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                         sQuerySpec->mTableNode,
                                         sLockTableList,
                                         aEnv )
             == STL_SUCCESS );

    /**
     * Init Plan 에 연결
     */

    STL_DASSERT( sLockTableList->mCount > 0 );
    aInitPlan->mLockTableList = sLockTableList;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief FROM list 로부터 Cursor Item 정보를 추가한다.
 * @param[in]     aRegionMem      Region Memory
 * @param[in]     aTableNode      Table Init Node
 * @param[in,out] aCursorItemList Lock Table List
 * @param[in]     aEnv            Environment
 * @remarks
 */
stlStatus qlvSetCursorItemListByFROM( knlRegionMem       * aRegionMem,
                                      qlvInitNode        * aTableNode,
                                      qlvRefTableList    * aCursorItemList,
                                      qllEnv             * aEnv )
{
    qlvRefTableItem * sCursorItem = NULL;
    qlvRefTableItem * sLastItem = NULL;
    
    
    qlvInitBaseTableNode * sBaseTable = NULL;
//    qlvInitSubTableNode  * sViewTable = NULL;
    qlvInitJoinTableNode * sJoinTable = NULL;
    
    /**
     * Parameter Validation
     */

    STL_DASSERT( aRegionMem != NULL );
    STL_DASSERT( aTableNode != NULL );
    STL_DASSERT( aCursorItemList != NULL );

    /**
     * Table Tree 를 순회하며 Cursor Item 을 추가 
     */
    
    switch ( aTableNode->mType )
    {
        case QLV_NODE_TYPE_BASE_TABLE:
            {
                sBaseTable = (qlvInitBaseTableNode *) aTableNode;

                if ( ellIsUpdatableTable( & sBaseTable->mTableHandle ) == STL_TRUE )
                {
                    /**
                     * Updatable Table 임 
                     */

                    STL_TRY( knlAllocRegionMem( aRegionMem,
                                                STL_SIZEOF( qlvRefTableItem ),
                                                (void **) & sCursorItem,
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    sCursorItem->mTableNode   = aTableNode;
                    sCursorItem->mTargetCount = 0;
                    sCursorItem->mTargets     = NULL;
                    sCursorItem->mNext        = NULL;

                    /**
                     * Cursor Item List 의 마지막에 추가한다.
                     */

                    if ( aCursorItemList->mHead == NULL )
                    {
                        aCursorItemList->mHead = sCursorItem;
                    }
                    else
                    {
                        sLastItem = aCursorItemList->mHead;
                        while ( sLastItem->mNext != NULL )
                        {
                            sLastItem = sLastItem->mNext;
                        }

                        sLastItem->mNext = sCursorItem;
                    }

                    aCursorItemList->mCount++;
                }
                else
                {
                    /**
                     * Updatable Table 이 아님
                     */
                }
                break;
            }
        case QLV_NODE_TYPE_SUB_TABLE:
            {
//                sViewTable = (qlvInitSubTableNode *) aTableNode;

                /**
                 * Updatable Table 이 아님 
                 * @todo Updatable View 가 되면 이를 확장해야 함
                 */
                break;
            }
        case QLV_NODE_TYPE_JOIN_TABLE:
            {
                sJoinTable = (qlvInitJoinTableNode *) aTableNode;

                switch ( sJoinTable->mJoinType )
                {
                    case QLV_JOIN_TYPE_NONE:
                    case QLV_JOIN_TYPE_CROSS:
                        {
                            /**
                             * Left 테이블 검사
                             */
                            
                            STL_TRY( qlvSetCursorItemListByFROM(
                                         aRegionMem,
                                         sJoinTable->mLeftTableNode,
                                         aCursorItemList,
                                         aEnv )
                                     == STL_SUCCESS );
                            
                            /**
                             * Right 테이블 검사
                             */
                            
                            STL_TRY( qlvSetCursorItemListByFROM(
                                         aRegionMem,
                                         sJoinTable->mRightTableNode,
                                         aCursorItemList,
                                         aEnv )
                                     == STL_SUCCESS );
                            break;
                        }
                    case QLV_JOIN_TYPE_INNER:
                        {
                            /**
                             * Left 테이블 검사
                             */
                                
                            STL_TRY( qlvSetCursorItemListByFROM(
                                         aRegionMem,
                                         sJoinTable->mLeftTableNode,
                                         aCursorItemList,
                                         aEnv )
                                     == STL_SUCCESS );
                                
                            /**
                             * Right 테이블 검사
                             */
                                
                            STL_TRY( qlvSetCursorItemListByFROM(
                                         aRegionMem,
                                         sJoinTable->mRightTableNode,
                                         aCursorItemList,
                                         aEnv )
                                     == STL_SUCCESS );
                                
                            break;
                        }
                    case QLV_JOIN_TYPE_LEFT_OUTER:
                    case QLV_JOIN_TYPE_RIGHT_OUTER:
                    case QLV_JOIN_TYPE_FULL_OUTER:
                        {
                            /**
                             * Outer Join 의 모든 컬럼은 Updatable Column 이 아님
                             * - LEFT OUTER 의 left table 은 가능할 수도 있으나,
                             * - Lock Version Conflict 발생시
                             *   ON 절에 부합하는 새로운 결과를 추출할 수가 없다.
                             */
                            
                            break;
                        }
                    case QLV_JOIN_TYPE_LEFT_SEMI:
                    case QLV_JOIN_TYPE_INVERTED_RIGHT_SEMI:
                    case QLV_JOIN_TYPE_LEFT_ANTI_SEMI:
                    case QLV_JOIN_TYPE_LEFT_ANTI_SEMI_NA:
                        {
                            /**
                             * Left 테이블 검사
                             */
                                
                            STL_TRY( qlvSetCursorItemListByFROM(
                                         aRegionMem,
                                         sJoinTable->mLeftTableNode,
                                         aCursorItemList,
                                         aEnv )
                                     == STL_SUCCESS );
                            break;
                        }                            
                    case QLV_JOIN_TYPE_RIGHT_SEMI:
                    case QLV_JOIN_TYPE_INVERTED_LEFT_SEMI:
                    case QLV_JOIN_TYPE_RIGHT_ANTI_SEMI:
                    case QLV_JOIN_TYPE_RIGHT_ANTI_SEMI_NA:
                        {
                            /**
                             * Right 테이블 검사
                             */
                                
                            STL_TRY( qlvSetCursorItemListByFROM(
                                         aRegionMem,
                                         sJoinTable->mRightTableNode,
                                         aCursorItemList,
                                         aEnv )
                                     == STL_SUCCESS );
                                
                            break;
                        }
                    default:
                        STL_ASSERT(0);
                        break;
                }
                break;
            }
        case QLV_NODE_TYPE_FLAT_INSTANT:
        case QLV_NODE_TYPE_SORT_INSTANT:
        case QLV_NODE_TYPE_HASH_INSTANT:
        case QLV_NODE_TYPE_SORT_JOIN_INSTANT:
        case QLV_NODE_TYPE_HASH_JOIN_INSTANT:
            {
                STL_DASSERT( ((qlvInitInstantNode *) aTableNode)->mTableNode != NULL );
                
                STL_TRY( qlvSetCursorItemListByFROM(
                             aRegionMem,
                             ((qlvInitInstantNode *) aTableNode)->mTableNode,
                             aCursorItemList,
                             aEnv )
                         == STL_SUCCESS );
                break;
            }
        default:
            {
                STL_ASSERT(0);
                break;
            }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
        

/**
 * @brief FOR UPDATE OF columns 를 위한 Lock Item List 를 설정한다.
 * @param[in]  aStmtInfo          Stmt Information
 * @param[in]  aUpdateColumnList  FOR UPDATE OF column list
 * @param[in]  aInitPlan          SELECT Init Plan 
 * @param[in]  aEnv               Environment
 * @remarks
 */
stlStatus qlvSetLockItemList4ForUpdateColumns( qlvStmtInfo   * aStmtInfo,
                                               qlpList       * aUpdateColumnList,
                                               qlvInitSelect * aInitPlan,
                                               qllEnv        * aEnv )
{
    qlvInitQuerySpecNode * sQuerySpecNode    = NULL;
    
    qlvRefTableList   * sRefTableList = NULL;
    
    qlpListElement    * sListElement = NULL;
    qlpColumnRef      * sColumnRef = NULL;
    qlvInitExpression * sColumnExpr = NULL;
    qlvInitNode       * sColumnRelationNode = NULL;

    qlvRefTableList   * sLockTableList = NULL;
    qlvRefTableItem   * sLockTableItem = NULL;
    qlvInitNode       * sLockTableNode = NULL;
    qlvRefTableItem   * sLastItem = NULL;

    stlBool             sIsExistTable = STL_FALSE;
    stlBool             sIsUpdatable = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aUpdateColumnList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan->mQueryNode->mType == QLV_NODE_TYPE_QUERY_SPEC,
                        QLL_ERROR_STACK(aEnv) );

    
    /********************************************************
     * FOR UPDATE OF 절을 Validation 하기 위한 FROM 절에 대한 임의 정보 구축
     ********************************************************/

    sQuerySpecNode = (qlvInitQuerySpecNode *) aInitPlan->mQueryNode;
    
    /**
     * Reference Table List 생성
     */

    STL_TRY( qlvCreateRefTableList( & QLL_VALIDATE_MEM( aEnv ),
                                    NULL,
                                    & sRefTableList,
                                    aEnv )
             == STL_SUCCESS );

    /**
     * 현재 Table Node 를 Reference Table List에 추가
     */

    STL_TRY( qlvSetNodeToRefTableItem( sRefTableList,
                                       sQuerySpecNode->mTableNode,
                                       aEnv )
             == STL_SUCCESS );

    /********************************************************
     * FOR UPDATE OF 절을 이용해 Lock Table List 구축 
     ********************************************************/

    /**
     * Lock Table List 초기화 
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                STL_SIZEOF( qlvRefTableList ),
                                (void **) & sLockTableList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sLockTableList->mCount = 0;
    sLockTableList->mHead  = NULL;

    
    QLP_LIST_FOR_EACH( aUpdateColumnList, sListElement )
    {
        sColumnRef = (qlpColumnRef *) QLP_LIST_GET_POINTER_VALUE( sListElement );

        /**
         * Column 정보 획득
         */
        
        sIsUpdatable = STL_TRUE;
        STL_TRY( qlvValidateColumnRef( aStmtInfo,
                                       QLV_EXPR_PHRASE_TARGET,
                                       aInitPlan->mInitPlan.mValidationObjList,
                                       sRefTableList,
                                       STL_FALSE,  /* aIsAtomic */
                                       STL_FALSE,  /* aAllowedAggrDepth */
                                       sColumnRef,
                                       & sColumnExpr,
                                       NULL,
                                       & sIsUpdatable,
                                       KNL_BUILTIN_FUNC_INVALID,
                                       DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                       DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                       QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                       aEnv )
                 == STL_SUCCESS );

        STL_TRY_THROW( sIsUpdatable == STL_TRUE, RAMP_ERR_NOT_UPDATABLE_COLUMN );


        /**
         * Column 의 Original Base Table Node 를 획득
         */
        
        sColumnRelationNode = sColumnExpr->mExpr.mColumn->mRelationNode;

        switch (sColumnRelationNode->mType)
        {
            case QLV_NODE_TYPE_BASE_TABLE:
                {
                    sLockTableNode = sColumnRelationNode;
                    break;
                }
            case QLV_NODE_TYPE_JOIN_TABLE:
                {
                    while( sColumnExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN )
                    {
                        sColumnExpr = sColumnExpr->mExpr.mInnerColumn->mOrgExpr;
                    }

                    STL_DASSERT( sColumnExpr->mType == QLV_EXPR_TYPE_COLUMN );
                    
                    sLockTableNode = sColumnExpr->mExpr.mColumn->mRelationNode;
                    break;
                }
            case QLV_NODE_TYPE_QUERY_SET:
            case QLV_NODE_TYPE_QUERY_SPEC:
            case QLV_NODE_TYPE_SUB_TABLE:
            default:
                {
                    STL_DASSERT(0);
                    break;
                }
        }

        STL_DASSERT( sLockTableNode != NULL );
        STL_DASSERT( sLockTableNode->mType == QLV_NODE_TYPE_BASE_TABLE );

        /**
         * 새로운 lockable table 일 경우 lock table list 에 추가함.
         */
        
        if ( sLockTableList->mHead == NULL )
        {
            STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                        STL_SIZEOF( qlvRefTableItem ),
                                        (void **) & sLockTableItem,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sLockTableItem->mTableNode   = sLockTableNode;
            sLockTableItem->mTargetCount = 0;
            sLockTableItem->mTargets     = NULL;
            sLockTableItem->mNext        = NULL;

            sLockTableList->mHead = sLockTableItem;
            sLockTableList->mCount++;
        }
        else
        {
            sLastItem = sLockTableList->mHead;

            sIsExistTable = STL_FALSE;
            while ( sLastItem != NULL )
            {
                if ( sLastItem->mTableNode == sLockTableNode )
                {
                    sIsExistTable = STL_TRUE;
                    break;
                }

                if ( sLastItem->mNext == NULL )
                {
                    break;
                }
                else
                {
                    sLastItem = sLastItem->mNext;
                }
            }

            if ( sIsExistTable == STL_TRUE )
            {
                /**
                 * 이미 존재하는 Lock Table 임
                 */
            }
            else
            {
                STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                            STL_SIZEOF( qlvRefTableItem ),
                                            (void **) & sLockTableItem,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                sLockTableItem->mTableNode   = sLockTableNode;
                sLockTableItem->mTargetCount = 0;
                sLockTableItem->mTargets     = NULL;
                sLockTableItem->mNext        = NULL;
                
                sLastItem->mNext = sLockTableItem;
                sLockTableList->mCount++;
            }
        }
    }

    
    /**
     * Init Plan 에 연결
     */

    STL_DASSERT( sLockTableList->mCount > 0 );
    aInitPlan->mLockTableList = sLockTableList;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_UPDATABLE_COLUMN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_COLUMN_IS_NOT_UPDATABLE,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sColumnRef->mNodePos,
                                           aEnv ),
                      ELL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/** @} qlvSelect */


