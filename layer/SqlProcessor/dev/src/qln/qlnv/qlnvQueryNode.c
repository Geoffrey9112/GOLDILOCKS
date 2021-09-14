/*******************************************************************************
 * qlnvQueryNode.c
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
 * @file qlnvQueryNode.c
 * @brief SQL Processor Layer Validation - Query Node Functions
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnv
 * @{
 */


/****************************************************
 * Inline Functions
 ****************************************************/ 

stlBool qlnvHasNestedAggr( qlvInitExpression * aExpr );


/**
 * @brief QUERY NODE 구문을 Validation 한다.
 * @param[in]       aStmtInfo           Stmt Information
 * @param[in]       aCalledFromSubQuery Subquery로부터 이 함수가 호출되었는지 여부
 * @param[in,out]   aValidationObjList  Validation Object List
 * @param[in]       aRefTableList       Reference Table List
 * @param[in]       aParseQueryNode     Query Node Parse Tree
 * @param[out]      aInitQueryNode      Query Node
 * @param[in]       aEnv                Environment
 */
inline stlStatus qlvQueryNodeValidation( qlvStmtInfo          * aStmtInfo,
                                         stlBool                aCalledFromSubQuery,
                                         ellObjectList        * aValidationObjList,
                                         qlvRefTableList      * aRefTableList,
                                         qllNode              * aParseQueryNode,
                                         qlvInitNode         ** aInitQueryNode,
                                         qllEnv               * aEnv )
{
    qllNode  * sQueryNode = NULL;

    /**********************************************************
     * Parameter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValidationObjList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseQueryNode != NULL, QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * Query Node Validation
     **********************************************************/

    sQueryNode = aParseQueryNode;

    while( sQueryNode->mType == QLL_SUB_TABLE_NODE_TYPE )
    {
        sQueryNode = ((qlpSubTableNode *) sQueryNode)->mQueryNode;
    }

    switch( sQueryNode->mType )
    {
        case QLL_QUERY_SET_NODE_TYPE:
            /**
             * Query Set Node Validation
             */

            STL_TRY( qlvValidateQuerySetNode( aStmtInfo,
                                              aCalledFromSubQuery,
                                              aValidationObjList,
                                              aRefTableList,
                                              (qlpQuerySetNode*)sQueryNode,
                                              aInitQueryNode,
                                              aEnv )
                     == STL_SUCCESS );

            break;
        case QLL_QUERY_SPEC_NODE_TYPE:
            /**
             * Query Spec Node Validation
             */
            STL_TRY( qlvValidateQuerySpecNode( aStmtInfo,
                                               aCalledFromSubQuery,
                                               aValidationObjList,
                                               aRefTableList,
                                               (qlpQuerySpecNode*)sQueryNode,
                                               aInitQueryNode,
                                               aEnv )
                     == STL_SUCCESS );
            break;
        default:
            STL_DASSERT( 0 );
            break;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief QUERY SPECIFICATION 구문을 Validation 한다.
 * @param[in]       aStmtInfo           Stmt Information
 * @param[in]       aCalledFromSubQuery Subquery로부터 이 함수가 호출되었는지 여부
 * @param[in,out]   aValidationObjList  Validation Object List
 * @param[in]       aRefTableList       Reference Table List
 * @param[in]       aParseQuerySpecNode Query Specification Parse Tree
 * @param[out]      aQueryNode          Query Node
 * @param[in]       aEnv                Environment
 *
 * @remark < Validation 순서 >
 *    <BR>
 *    <BR> Query 단위 Expression List 구성
 *    <BR> -> WITH clause
 *    <BR> -> FROM clause
 *    <BR> -> WHERE clause
 *    <BR> -> GROUP BY HAVING clause
 *    <BR> -> TARGET clause
 *    <BR> -> ORDER BY clause
 */
stlStatus qlvValidateQuerySpecNode( qlvStmtInfo         * aStmtInfo,
                                    stlBool               aCalledFromSubQuery,
                                    ellObjectList       * aValidationObjList,
                                    qlvRefTableList     * aRefTableList,
                                    qlpQuerySpecNode    * aParseQuerySpecNode,
                                    qlvInitNode        ** aQueryNode,
                                    qllEnv              * aEnv )
{
    stlInt32                  i                 = 0;

    qllNode                 * sTargetNode       = NULL;
    qlpListElement          * sListElem         = NULL;
    stlInt32                  sTargetCount      = 0;
    stlInt32                  sCount            = 0;

    qlvInitQuerySpecNode    * sQuerySpecNode    = NULL;
    qlvInitTarget           * sTargets          = NULL;
    qlvInitTarget           * sTempTargets      = NULL;
    qlvInitExpression       * sWhere            = NULL;
    qlvRefTableList         * sRefTableList     = NULL;
    qlvRefTableList         * sRefTableListGroupBy = NULL;

    stlInt32                  sExceptNameCount  = 0;
    stlChar                ** sExceptNameArr    = NULL;

    stlBool                   sHasOuterJoinOperator = STL_FALSE;
    stlBool                   sHasNestedAggr        = STL_FALSE;
    stlBool                   sIsGroupOperation     = STL_FALSE;

    qlvStmtInfo               sStmtInfo;
    qlvInitExprList         * sQueryExprList    = NULL;

    stlInt32                  sAllowedAggrDepth = 1;

    qlvRefExprItem          * sExprItem         = NULL;

    qlpGroupBy                sGroupBy;
    qlpList                   sList;

    qlvInitInstantNode      * sGroupingInst     = NULL;
    
    
    ellDictHandle sSchemaHandle;
    ellInitDictHandle( & sSchemaHandle );


    /**********************************************************
     * Parameter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValidationObjList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseQuerySpecNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aParseQuerySpecNode->mType == QLL_QUERY_SPEC_NODE_TYPE),
                        QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * 준비 작업
     **********************************************************/

    /**
     * Query Specification Node 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                STL_SIZEOF( qlvInitQuerySpecNode ),
                                (void **) & sQuerySpecNode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sQuerySpecNode, 0x00, STL_SIZEOF( qlvInitQuerySpecNode ) );
    sQuerySpecNode->mType = QLV_NODE_TYPE_QUERY_SPEC;


    /**
     * Query Block Name 기본값 및 Query Block Identifier 설정
     */

    stlSnprintf( sQuerySpecNode->mQBName,
                 QLV_QUERY_BLOCK_NAME_MAX_SIZE - 1,
                 "%s%d",
                 aStmtInfo->mQBPrefix,
                 *(aStmtInfo->mQBIndex) );

    sQuerySpecNode->mQBID = *(aStmtInfo->mQBIndex);

    (*(aStmtInfo->mQBIndex))++;


    /**
     * Query Expression List 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                STL_SIZEOF( qlvInitExprList ),
                                (void **) & sQueryExprList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sQueryExprList->mPseudoColExprList,
                                   QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sQueryExprList->mAggrExprList,
                                   QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sQueryExprList->mNestedAggrExprList,
                                   QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    sQueryExprList->mStmtExprList = aStmtInfo->mQueryExprList->mStmtExprList;

    sQuerySpecNode->mQueryExprList = sQueryExprList;
    sQuerySpecNode->mHasGroupOper  = STL_FALSE;
    
    
    /**
     * statement information 설정
     */

    sStmtInfo = *aStmtInfo;
    sStmtInfo.mQueryExprList = sQueryExprList;


    /**********************************************************
     * Expression 공통
     **********************************************************/

    /**
     * Reference Table List 생성
     */

    STL_TRY( qlvCreateRefTableList( &QLL_VALIDATE_MEM( aEnv ),
                                    aRefTableList,
                                    & sRefTableList,
                                    aEnv )
             == STL_SUCCESS );


    /***************************************************
     * With 절 Validation
     ***************************************************/

    /**
     * @todo With Clause
     */


    /**********************************************************
     * FROM 절 Validation
     **********************************************************/

    sQuerySpecNode->mTableCount = 0;
    STL_TRY( qlvTableNodeValidation( & sStmtInfo,
                                     aValidationObjList,
                                     sRefTableList,
                                     aParseQuerySpecNode->mFrom,
                                     &(sQuerySpecNode->mTableNode),
                                     &(sQuerySpecNode->mTableCount),
                                     aEnv )
             == STL_SUCCESS );


    /**
     * 현재 Table Node를 Reference Table List에 추가
     */

    STL_TRY( qlvSetNodeToRefTableItem( sRefTableList,
                                       sQuerySpecNode->mTableNode,
                                       aEnv )
             == STL_SUCCESS );


    /**********************************************************
     * WHERE 절 Validation
     **********************************************************/

    /**
     * WHERE절 validation
     */

    if( aParseQuerySpecNode->mWhere == NULL )
    {
        sWhere = NULL;
    }
    else
    {
        sHasOuterJoinOperator = STL_FALSE;
        STL_TRY( qlvValidateValueExpr( & sStmtInfo,
                                       QLV_EXPR_PHRASE_CONDITION,
                                       aValidationObjList,
                                       sRefTableList,
                                       STL_FALSE,
                                       STL_FALSE,  /* Row Expr */
                                       sAllowedAggrDepth,
                                       aParseQuerySpecNode->mWhere,
                                       & sWhere,
                                       &sHasOuterJoinOperator,
                                       KNL_BUILTIN_FUNC_INVALID,
                                       DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                       DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                       QLL_INIT_PLAN( sStmtInfo.mDBCStmt ),
                                       aEnv )
                 == STL_SUCCESS );

        /* Outer Join Operator가 존재하는 경우 Validation 및 Table Node 조정 */
        if( sHasOuterJoinOperator == STL_TRUE )
        {
            STL_TRY( qlvValidateOuterJoinOperator( &sStmtInfo,
                                                   sWhere,
                                                   sQuerySpecNode->mTableNode,
                                                   &sWhere,
                                                   &(sQuerySpecNode->mTableNode),
                                                   aEnv )
                     == STL_SUCCESS );
        }


        /**
         * Rewrite Expression
         */

        if( sWhere != NULL )
        {
            STL_TRY( qlvRewriteExpr( sWhere,
                                     DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                     sQueryExprList,
                                     & sWhere,
                                     QLL_INIT_PLAN( sStmtInfo.mDBCStmt ),
                                     aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /* Do Nothing */
        }
    }

    sQuerySpecNode->mWhereExpr = sWhere;


    /**********************************************************
     * Hint 분석
     **********************************************************/

    /**
     * Hint Info 생성
     */

    STL_TRY( qlvCreateHintInfo( &sQuerySpecNode->mHintInfo,
                                sQuerySpecNode->mTableCount,
                                QLL_INIT_PLAN( sStmtInfo.mDBCStmt ),
                                aEnv )
             == STL_SUCCESS );

    STL_TRY( qlvValidateHintInfo( &sStmtInfo,
                                  aCalledFromSubQuery,
                                  ( ( (aParseQuerySpecNode->mGroupBy == NULL) &&
                                      (aParseQuerySpecNode->mHaving == NULL) )
                                    ? STL_FALSE : STL_TRUE ),
                                  ( (aParseQuerySpecNode->mSetQuantifier == QLP_SET_QUANTIFIER_DISTINCT)
                                    ? STL_TRUE : STL_FALSE ),
                                  (qlvInitNode*)sQuerySpecNode->mTableNode,
                                  sQuerySpecNode->mTableCount,
                                  aParseQuerySpecNode->mHints,
                                  sQuerySpecNode->mHintInfo,
                                  aEnv )
             == STL_SUCCESS );

    if( sStmtInfo.mHasHintError == STL_TRUE )
    {
        aStmtInfo->mHasHintError = STL_TRUE;
    }
    else
    {
        /* Do Nothing */
    }


    /***************************************************
     * Group By 절 Validation
     ***************************************************/

    /**
     * qlvGroupByList 생성
     */

    if( aParseQuerySpecNode->mGroupBy == NULL ) 
    {
        sAllowedAggrDepth = 1;

        if( aParseQuerySpecNode->mHaving == NULL )
        {
            sQuerySpecNode->mGroupBy = NULL;
            sQuerySpecNode->mHasAggrDistinct = STL_FALSE;
            sGroupingInst = NULL;
            sRefTableListGroupBy = sRefTableList;
        }
        else
        {
            sQuerySpecNode->mHasGroupOper = STL_TRUE;
        
            STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( sStmtInfo.mDBCStmt ),
                                        STL_SIZEOF( qlvGroupBy ),
                                        (void **) &sQuerySpecNode->mGroupBy,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            stlMemset( sQuerySpecNode->mGroupBy, 0x00, STL_SIZEOF( qlvGroupBy ) );

            sGroupBy.mType             = QLL_GROUP_BY_TYPE;
            sGroupBy.mNodePos          = 0;
            sGroupBy.mGroupingElemList = & sList;

            sList.mType  = QLL_POINTER_LIST_TYPE;
            sList.mCount = 0;
            sList.mHead  = NULL;
            sList.mTail  = NULL;

            STL_TRY( qlvValidateGroupBy( & sStmtInfo,
                                         aValidationObjList,
                                         sRefTableList,
                                         & sGroupBy,
                                         (qlpHaving *) aParseQuerySpecNode->mHaving,
                                         sQuerySpecNode->mGroupBy,
                                         & sQuerySpecNode->mHasAggrDistinct,
                                         aEnv )
                     == STL_SUCCESS );

            sGroupingInst = & sQuerySpecNode->mGroupBy->mInstant;
            sGroupingInst->mTableNode = sQuerySpecNode->mTableNode;

            /**
             * Group By Reference Table List 생성
             */

            STL_TRY( qlvCreateRefTableList( &QLL_VALIDATE_MEM( aEnv ),
                                            aRefTableList,
                                            & sRefTableListGroupBy,
                                            aEnv )
                     == STL_SUCCESS );

            /**
             * Group By Node를 Reference Table List에 추가
             */
            
            STL_TRY( qlvSetNodeToRefTableItem( sRefTableListGroupBy,
                                               (qlvInitNode *) sGroupingInst,
                                               aEnv )
                     == STL_SUCCESS );
        }
    }
    else
    {
        /**
         * GROUP BY validation
         */

        sQuerySpecNode->mHasGroupOper = STL_TRUE;
        
        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( sStmtInfo.mDBCStmt ),
                                    STL_SIZEOF( qlvGroupBy ),
                                    (void **) &sQuerySpecNode->mGroupBy,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sQuerySpecNode->mGroupBy, 0x00, STL_SIZEOF( qlvGroupBy ) );

        STL_TRY( qlvValidateGroupBy( & sStmtInfo,
                                     aValidationObjList,
                                     sRefTableList,
                                     (qlpGroupBy *) aParseQuerySpecNode->mGroupBy,
                                     (qlpHaving *) aParseQuerySpecNode->mHaving,
                                     sQuerySpecNode->mGroupBy,
                                     & sQuerySpecNode->mHasAggrDistinct,
                                     aEnv )
                 == STL_SUCCESS );

        sAllowedAggrDepth = 2;
        sGroupingInst = & sQuerySpecNode->mGroupBy->mInstant;
        sGroupingInst->mTableNode = sQuerySpecNode->mTableNode;

        /**
         * Group By Reference Table List 생성
         */

        STL_TRY( qlvCreateRefTableList( &QLL_VALIDATE_MEM( aEnv ),
                                        aRefTableList,
                                        & sRefTableListGroupBy,
                                        aEnv )
                 == STL_SUCCESS );

        /**
         * Group By Node를 Reference Table List에 추가
         */
            
        STL_TRY( qlvSetNodeToRefTableItem( sRefTableListGroupBy,
                                           (qlvInitNode *) sGroupingInst,
                                           aEnv )
                 == STL_SUCCESS );
    }


    /**********************************************************
     * TARGET 절 & ORDER BY절 Validation (GROUP BY 관련성 평가 이전)
     **********************************************************/

    /**
     * 준비 작업
     * ----------------------------------------
     *  Join의 Using 절에 사용된 컬럼들의 이름을
     *  except named list로 만든다.
     */

    sExceptNameCount = 0;
    STL_TRY( qlvGetJoinNamedColumnCount( sQuerySpecNode->mTableNode,
                                         &sExceptNameCount,
                                         aEnv )
             == STL_SUCCESS );

    if( sExceptNameCount == 0 )
    {
        sExceptNameArr = NULL;
    }
    else
    {
        STL_TRY( knlAllocRegionMem( &QLL_VALIDATE_MEM( aEnv ),
                                    STL_SIZEOF( stlChar* ) * sExceptNameCount,
                                    (void **) &sExceptNameArr,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    

    /**
     * TARGET절 Validation : CODE영역의 Target 설정
     */

    sListElem   = QLP_LIST_GET_FIRST( aParseQuerySpecNode->mTargets );
    sTargetNode = (qllNode *) QLP_LIST_GET_POINTER_VALUE( sListElem );

    if( sTargetNode->mType == QLL_TARGET_ASTERISK_TYPE )
    {
        if( ((qlpTarget*)sTargetNode)->mRelName == NULL )
        {
            /**
             * 현재 Table Node의 모든 Table들의 모든 컬럼을 target으로 설정
             */

            STL_TRY( qlvGetAsteriskTargetCountMultiTables( & sStmtInfo,
                                                           sQuerySpecNode->mTableNode,
                                                           0,
                                                           sExceptNameArr,
                                                           &sTargetCount,
                                                           aEnv )
                     == STL_SUCCESS );


            /**
             * TARGET절 Validation : Init 영역의 Target 공간 할당
             */

            STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( sStmtInfo.mDBCStmt ),
                                        STL_SIZEOF( qlvInitTarget ) * sTargetCount,
                                        (void **) & sTargets,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            stlMemset( sTargets,
                       0x00,
                       STL_SIZEOF( qlvInitTarget ) * sTargetCount );


            /**
             * Target List 생성
             */

            sCount = 0;
            STL_TRY( qlvValidateAsteriskMultiTables( & sStmtInfo,
                                                     QLV_EXPR_PHRASE_TARGET,
                                                     sQuerySpecNode->mTableNode,
                                                     (qlpTarget*)sTargetNode,
                                                     0,
                                                     sExceptNameArr,
                                                     sTargets,
                                                     &sCount,
                                                     aEnv )
                     == STL_SUCCESS );

            STL_THROW( RAMP_SKIP_TARGET_VALIDATION );
        }
    }
    

    /**
     * TARGET절 Validation : INIT영역의 Target 공간 할당
     */

    STL_TRY( qlvGetTargetCountFromTargetList( & sStmtInfo,
                                              aParseQuerySpecNode->mTargets,
                                              sQuerySpecNode->mTableNode,
                                              0,
                                              sExceptNameArr,
                                              &sTargetCount,
                                              aEnv )
             == STL_SUCCESS );

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( sStmtInfo.mDBCStmt ),
                                STL_SIZEOF( qlvInitTarget ) * sTargetCount,
                                (void **) & sTargets,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sTargets,
               0x00,
               STL_SIZEOF( qlvInitTarget ) * sTargetCount );


    /**
     * Target validation
     */

    sTargetCount = 0;
    QLP_LIST_FOR_EACH( aParseQuerySpecNode->mTargets, sListElem )
    {
        sTargetNode = (qllNode *) QLP_LIST_GET_POINTER_VALUE( sListElem );

        if( sTargetNode->mType == QLL_TARGET_TYPE )
        {
            STL_TRY( qlvValidateTarget( & sStmtInfo,
                                        aValidationObjList,
                                        sRefTableListGroupBy,
                                        sAllowedAggrDepth,
                                        (qlpTarget*)sTargetNode,
                                        &sTargets[sTargetCount],
                                        DTL_ITERATION_TIME_FOR_EACH_AGGREGATION,
                                        DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                        aEnv )
                     == STL_SUCCESS );
            sTargetCount++;
        }
        else if( sTargetNode->mType == QLL_TARGET_ASTERISK_TYPE )
        {
            sCount = 0;

            STL_TRY( qlvValidateAsteriskMultiTables( & sStmtInfo,
                                                     QLV_EXPR_PHRASE_TARGET,
                                                     sQuerySpecNode->mTableNode,
                                                     (qlpTarget*)sTargetNode,
                                                     0,
                                                     sExceptNameArr,
                                                     &sTargets[sTargetCount],
                                                     &sCount,
                                                     aEnv )
                     == STL_SUCCESS );

            sTargetCount += sCount;
        }
        else
        {
            STL_DASSERT( 0 );
        }
    }

    STL_RAMP( RAMP_SKIP_TARGET_VALIDATION );
    

    /**
     * 현재 Target 정보를 Reference Table List에 추가
     */

    STL_TRY( qlvSetTargetToRefTableItem( sRefTableListGroupBy,
                                         sTargetCount,
                                         sTargets,
                                         aEnv )
             == STL_SUCCESS );


    /**********************************************************
     * Push : Target -> GROUP BY 
     **********************************************************/

    /**
     * Nested Aggregation 존재 여부 검사
     */

    /* Target Clause */
    sHasNestedAggr = STL_FALSE;
    for( i = 0; i < sTargetCount; i++ )
    {
        if( qlnvHasNestedAggr( sTargets[i].mExpr ) == STL_TRUE )
        {
            sHasNestedAggr = STL_TRUE;
            break;
        }
    }


    if( sHasNestedAggr == STL_FALSE )
    {
        /**
         * Order By Validation
         */
    
        if( aParseQuerySpecNode->mOrderBy != NULL )
        {
            /**
             * TARGET절 Validation : Init 영역의 Target 공간 할당
             */

            STL_TRY( knlAllocRegionMem( & QLL_VALIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlvInitTarget ) * sTargetCount,
                                        (void **) & sTempTargets,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            stlMemcpy( sTempTargets,
                       sTargets,
                       STL_SIZEOF( qlvInitTarget ) * sTargetCount );

            /* 임시로 Order By 구문을 Validation 한다. */
            STL_TRY( qlvValidateOrderBy( & sStmtInfo,
                                         aValidationObjList,
                                         sRefTableListGroupBy,
                                         sTargetCount,
                                         sTempTargets,
                                         sAllowedAggrDepth,
                                         aParseQuerySpecNode->mOrderBy,
                                         & sQuerySpecNode->mOrderBy,
                                         aEnv )
                     == STL_SUCCESS );

            /**
             * Nested Aggregation 존재 여부 검사
             */

            if( sQuerySpecNode->mOrderBy != NULL )
            {
                sExprItem = sQuerySpecNode->mOrderBy->mInstant.mKeyColList->mHead;
                while( sExprItem != NULL )
                {
                    if( qlnvHasNestedAggr( sExprItem->mExprPtr ) == STL_TRUE )
                    {
                        sHasNestedAggr = STL_TRUE;
                        break;
                    }
                    sExprItem = sExprItem->mNext;
                }
            }
        }
    }
    

    /**
     * Target절의 Aggregation을 Nested Aggregation으로 설정
     */

    if( sHasNestedAggr == STL_TRUE )
    {
        /* target */
        for( i = 0; i < sTargetCount; i++ )
        {
            STL_TRY( qlvSetNestedAggr( & sStmtInfo,
                                       sQuerySpecNode->mQueryExprList,
                                       sTargets[i].mExpr,
                                       aEnv )
                     == STL_SUCCESS );
        }
    }


    /**
     * Grouping Operation 존재 여부 체크
     */
    
    if( sQuerySpecNode->mQueryExprList->mAggrExprList->mCount > 0 )
    {
        sQuerySpecNode->mHasGroupOper = STL_TRUE;
        sIsGroupOperation = STL_TRUE;
    }
    else
    {
        /* Do Nothing */
    }


    /**
     * Group By Clause에 대한 Target Expression을 Validation
     */

    if( sQuerySpecNode->mGroupBy != NULL )
    {
        for( i = 0; i < sTargetCount; i++ )
        {
            STL_TRY( qlvValidateValueExprByGroup( aStmtInfo,
                                                  & sQuerySpecNode->mGroupBy->mInstant,
                                                  sTargets[i].mExpr,
                                                  sHasNestedAggr,
                                                  0,
                                                  &(sTargets[i].mExpr),
                                                  aEnv )
                     == STL_SUCCESS );
        }
    }
    else
    {
        if( sIsGroupOperation == STL_TRUE )
        {
            for( i = 0; i < sTargetCount; i++ )
            {
                STL_TRY( qlvValidateValueExprWithoutGroup( aStmtInfo,
                                                           sTargets[i].mExpr,
                                                           aEnv )
                         == STL_SUCCESS );
            }
        }
        else
        {
            /* Do Nothing */
        }
    }


    /**********************************************************
     * Push : Target -> DISTINCT
     **********************************************************/

    /**
     * Distinct 여부 셋팅
     */

    if( aParseQuerySpecNode->mSetQuantifier == QLP_SET_QUANTIFIER_ALL )
    {
        sQuerySpecNode->mIsDistinct = STL_FALSE;
        sQuerySpecNode->mDistinct = NULL;
    }
    else
    {
        STL_DASSERT( aParseQuerySpecNode->mSetQuantifier == QLP_SET_QUANTIFIER_DISTINCT );

        /**
         * Sequence를 사용하는지 체크
         */

        STL_TRY_THROW( sQueryExprList->mPseudoColExprList->mCount == 0,
                       RAMP_ERR_PSEUDO_COLUMN_NOT_ALLOWED );

        
        /**
         * Distinct를 위한 새로운 Group Instant 생성
         */

        if( sHasNestedAggr == STL_TRUE )
        {
            sQuerySpecNode->mIsDistinct = STL_FALSE;
            sQuerySpecNode->mDistinct = NULL;
        }
        else
        {
            if( ( sIsGroupOperation == STL_TRUE ) &&
                ( sQuerySpecNode->mGroupBy == NULL ) )
            {
                sQuerySpecNode->mIsDistinct = STL_FALSE;
                sQuerySpecNode->mDistinct = NULL;
            }
            else
            {
                sQuerySpecNode->mHasGroupOper = STL_TRUE;
                sQuerySpecNode->mIsDistinct   = STL_TRUE;

                STL_TRY( qlvValidateValueExprByDistinct( aStmtInfo,
                                                         & sQuerySpecNode->mDistinct,
                                                         sQuerySpecNode->mGroupBy,
                                                         sTargetCount,
                                                         sTargets,
                                                         aEnv )
                         == STL_SUCCESS );
        
                sGroupingInst = sQuerySpecNode->mDistinct;
            }
        }
    }

    
    /**********************************************************
     * Push : Target -> Order By
     **********************************************************/

    /**
     * Order By Validation
     */
    
    if( aParseQuerySpecNode->mOrderBy == NULL )
    {
        sQuerySpecNode->mOrderBy = NULL;
    }
    else
    {
        STL_TRY_THROW( sQueryExprList->mPseudoColExprList->mCount == 0,
                       RAMP_ERR_PSEUDO_COLUMN_NOT_ALLOWED );
        
        STL_TRY( qlvValidateOrderBy( & sStmtInfo,
                                     aValidationObjList,
                                     sRefTableListGroupBy,
                                     sTargetCount,
                                     sTargets,
                                     sAllowedAggrDepth,
                                     aParseQuerySpecNode->mOrderBy,
                                     & sQuerySpecNode->mOrderBy,
                                     aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Target절과 Order By 절의 Aggregation을 Nested Aggregation으로 설정
     */

    if( sHasNestedAggr == STL_TRUE )
    {
        /* order by */
        if( sQuerySpecNode->mOrderBy != NULL )
        {
            sExprItem = sQuerySpecNode->mOrderBy->mInstant.mKeyColList->mHead;
            while( sExprItem != NULL )
            {
                STL_TRY( qlvSetNestedAggr( & sStmtInfo,
                                           sQuerySpecNode->mQueryExprList,
                                           sExprItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr,
                                           aEnv )
                         == STL_SUCCESS );
                sExprItem = sExprItem->mNext;
            }
        
            sExprItem = sQuerySpecNode->mOrderBy->mInstant.mRecColList->mHead;
            while( sExprItem != NULL )
            {
                STL_TRY( qlvSetNestedAggr( & sStmtInfo,
                                           sQuerySpecNode->mQueryExprList,
                                           sExprItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr,
                                           aEnv )
                         == STL_SUCCESS );
                sExprItem = sExprItem->mNext;
            }
        }
        else
        {
            /* Do Nothing */
        }
    }
    else
    {
        /* Do Nothing */
    }


    /**
     * Aggregation에 Distinct 조건이 존재한다면 Child Node에 Aggregation을 Push 할 수 없다.
     */

    STL_TRY( qlnvHasDistinctAggr( sQuerySpecNode->mQueryExprList->mAggrExprList,
                                  & sQuerySpecNode->mHasAggrDistinct,
                                  aEnv )
             == STL_SUCCESS );
    
    STL_TRY( qlnvHasDistinctAggr( sQuerySpecNode->mQueryExprList->mNestedAggrExprList,
                                  & sQuerySpecNode->mHasNestedAggrDistinct,
                                  aEnv )
             == STL_SUCCESS );


    /**
     * Order By Clause에 대한 Target Expression을 Validation
     */

    if( sQuerySpecNode->mOrderBy != NULL )
    {
        for( i = 0; i < sTargetCount; i++ )
        {
            STL_TRY( qlnoAddReadColToInstant( & sQuerySpecNode->mOrderBy->mInstant,
                                              sTargets[ i ].mExpr,
                                              & sTargets[ i ].mExpr,
                                              QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                              aEnv )
                     == STL_SUCCESS );
        }
    }
    

    /**********************************************************
     * TARGET 절 & ORDER BY절 Validation (GROUP BY 관련성 평가)
     **********************************************************/
    
    /**
     * Group By Clause에 대한 Order By Expression을 Validation
     */
    
    if( sQuerySpecNode->mOrderBy != NULL )
    {
        if( sGroupingInst != NULL )
        {
            /* Sort Key 검사 */
            sExprItem = sQuerySpecNode->mOrderBy->mInstant.mKeyColList->mHead;
            while( sExprItem != NULL )
            {
                STL_TRY( qlvValidateValueExprByGroup(
                             aStmtInfo,
                             sGroupingInst,
                             sExprItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr,
                             sHasNestedAggr,
                             0,
                             & sExprItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr,
                             aEnv )
                         == STL_SUCCESS );
                sExprItem = sExprItem->mNext;
            }
        }
        else
        {
            /* Sort Key 검사 */
            if( sIsGroupOperation == STL_TRUE )
            {
                sExprItem = sQuerySpecNode->mOrderBy->mInstant.mKeyColList->mHead;
                while( sExprItem != NULL )
                {
                    STL_TRY( qlvValidateValueExprWithoutGroup( aStmtInfo,
                                                               sExprItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr,
                                                               aEnv )
                             == STL_SUCCESS );
                    sExprItem = sExprItem->mNext;
                }

                for( i = 0; i < sTargetCount; i++ )
                {
                    STL_TRY( qlvValidateValueExprWithoutGroup( aStmtInfo,
                                                               sTargets[i].mExpr,
                                                               aEnv )
                             == STL_SUCCESS );
                }
            }
            else
            {
                /* Do Nothing */
            }
        }
    }

    
    /**********************************************************
     * Distinct를 위한 Instant Node에 Record Column이 없어야 한다.
     **********************************************************/

    if( sQuerySpecNode->mDistinct != NULL )
    {
        STL_TRY_THROW( sQuerySpecNode->mDistinct->mRecColCnt == 0,
                       RAMP_ERR_NOT_A_GROUP_FUNCTION );
    }

    
    /**********************************************************
     * 모든 Instant Node에 대하여 Inner Column의 Idx 설정
     **********************************************************/

    /* Group By Instant Node */
    if( sQuerySpecNode->mGroupBy != NULL )
    {
        STL_TRY( qlnoSetIdxInstant( &(sQuerySpecNode->mGroupBy->mInstant),
                                    aEnv )
                 == STL_SUCCESS );
    }

    /* Distinct Instant Node */
    if( sQuerySpecNode->mDistinct != NULL )
    {
        STL_TRY( qlnoSetIdxInstant( sQuerySpecNode->mDistinct,
                                    aEnv )
                 == STL_SUCCESS );
    }

    /* Order By Instant Node */
    if( sQuerySpecNode->mOrderBy != NULL )
    {
        STL_TRY( qlnoSetIdxInstant( &(sQuerySpecNode->mOrderBy->mInstant),
                                    aEnv )
                 == STL_SUCCESS );
    }


    /**********************************************************
     * Target Expression 마무리
     **********************************************************/

    /**
     * Target Column Expresion을 INNER COLUMN으로 Wrapping
     */

    for( i = 0; i < sTargetCount; i++ )
    {
        STL_TRY( qlvGetExprDataTypeInfo( sStmtInfo.mSQLString,
                                         & QLL_VALIDATE_MEM( aEnv ),
                                         sTargets[i].mExpr,
                                         NULL,
                                         & sTargets[i].mDataTypeInfo,
                                         aEnv )
                 == STL_SUCCESS );
            
        STL_TRY( qlvAddInnerColumnExpr( &(sTargets[i].mExpr),
                                        (qlvInitNode*)sQuerySpecNode,
                                        i,
                                        QLL_INIT_PLAN( sStmtInfo.mDBCStmt ),
                                        aEnv )
                 == STL_SUCCESS );
    }
    

    sQuerySpecNode->mTargetCount = sTargetCount;
    sQuerySpecNode->mTargets     = sTargets;


    /**********************************************************
     * OFFSET .. LIMIT .. validation
     **********************************************************/

    /**
     * OFFSET : Result Skip
     */

    if( aParseQuerySpecNode->mResultSkip == NULL )
    {
        sQuerySpecNode->mResultSkip = NULL;
        sQuerySpecNode->mSkipCnt = 0;
    }
    else
    {
        STL_TRY( qlvValidateValueExpr( & sStmtInfo,
                                       QLV_EXPR_PHRASE_OFFSETLIMIT,
                                       aValidationObjList,
                                       sRefTableList,
                                       STL_FALSE,
                                       STL_FALSE,  /* Row Expr */
                                       0 /* Allowed Aggregation Depth */,
                                       aParseQuerySpecNode->mResultSkip,
                                       & sQuerySpecNode->mResultSkip,
                                       NULL,
                                       KNL_BUILTIN_FUNC_INVALID,
                                       DTL_ITERATION_TIME_FOR_EACH_QUERY,
                                       DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                       QLL_INIT_PLAN( sStmtInfo.mDBCStmt ),
                                       aEnv )
                 == STL_SUCCESS );

        STL_TRY_THROW ( qlvIsValidSkipLimit( sQuerySpecNode->mResultSkip, aEnv ) == STL_TRUE,
                        RAMP_ERR_INVALID_RESULT_SKIP );

        if( sQuerySpecNode->mResultSkip->mType == QLV_EXPR_TYPE_VALUE )
        {
            STL_TRY( dtlGetIntegerFromString(
                         sQuerySpecNode->mResultSkip->mExpr.mValue->mData.mString,
                         stlStrlen( sQuerySpecNode->mResultSkip->mExpr.mValue->mData.mString ),
                         & sQuerySpecNode->mSkipCnt,
                         QLL_ERROR_STACK( aEnv ) )
                     == STL_SUCCESS );

            STL_TRY_THROW( sQuerySpecNode->mSkipCnt >= 0, RAMP_ERR_INVALID_RESULT_SKIP );
        }
        else
        {
            /**
             * @todo Rewrite OFFSET Expression
             */

            STL_DASSERT( sQuerySpecNode->mResultSkip->mType == QLV_EXPR_TYPE_BIND_PARAM );
            sQuerySpecNode->mSkipCnt = 0;
        }
    }

    
    /**
     * LIMIT : Result Limit
     */

    if( aParseQuerySpecNode->mResultLimit == NULL )
    {
        sQuerySpecNode->mResultLimit = NULL;
        sQuerySpecNode->mFetchCnt = QLL_FETCH_COUNT_MAX;
    }
    else
    {
        STL_TRY( qlvValidateValueExpr( & sStmtInfo,
                                       QLV_EXPR_PHRASE_OFFSETLIMIT,
                                       aValidationObjList,
                                       sRefTableList,
                                       STL_FALSE,
                                       STL_FALSE,  /* Row Expr */
                                       0 /* Allowed Aggregation Depth */,
                                       aParseQuerySpecNode->mResultLimit,
                                       & sQuerySpecNode->mResultLimit,
                                       NULL,
                                       KNL_BUILTIN_FUNC_INVALID,
                                       DTL_ITERATION_TIME_FOR_EACH_QUERY,
                                       DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                       QLL_INIT_PLAN( sStmtInfo.mDBCStmt ),
                                       aEnv )
                 == STL_SUCCESS );

        STL_TRY_THROW ( qlvIsValidSkipLimit( sQuerySpecNode->mResultLimit, aEnv ) == STL_TRUE,
                        RAMP_ERR_INVALID_RESULT_LIMIT );

        if( sQuerySpecNode->mResultLimit->mType == QLV_EXPR_TYPE_VALUE )
        {
            STL_TRY( dtlGetIntegerFromString(
                         sQuerySpecNode->mResultLimit->mExpr.mValue->mData.mString,
                         stlStrlen( sQuerySpecNode->mResultLimit->mExpr.mValue->mData.mString ),
                         & sQuerySpecNode->mFetchCnt,
                         QLL_ERROR_STACK( aEnv ) )
                     == STL_SUCCESS );

            STL_TRY_THROW( sQuerySpecNode->mFetchCnt > 0, RAMP_ERR_INVALID_RESULT_LIMIT );
        }
        else
        {
            /**
             * @todo Rewrite LIMIT Expression
             */

            STL_DASSERT( sQuerySpecNode->mResultLimit->mType == QLV_EXPR_TYPE_BIND_PARAM );
            sQuerySpecNode->mFetchCnt = QLL_FETCH_COUNT_MAX;
        }
    }

    
    /**********************************************************
     * SELECT Privilege Validation
     **********************************************************/

    /**
     * Query Spec Privilege Check
     */

    STL_TRY( qlaCheckPrivQuerySpec( aStmtInfo->mTransID,
                                    aStmtInfo->mDBCStmt,
                                    aStmtInfo->mSQLStmt,
                                    sQuerySpecNode->mTableNode,
                                    aEnv )
             == STL_SUCCESS );

    
    /**********************************************************
     * 마무리
     **********************************************************/

    /**
     * Output 설정
     */

    *aQueryNode = (qlvInitNode*)sQuerySpecNode;


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_RESULT_SKIP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_RESULT_OFFSET_MUST_BE_ZERO_OR_POSITIVE,
                      qlgMakeErrPosString( sStmtInfo.mSQLString,
                                           aParseQuerySpecNode->mResultSkip->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_INVALID_RESULT_LIMIT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_RESULT_LIMIT_MUST_BE_POSITIVE,
                      qlgMakeErrPosString( sStmtInfo.mSQLString,
                                           aParseQuerySpecNode->mResultLimit->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_NOT_A_GROUP_FUNCTION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_A_GROUP_FUNCTION,
                      qlgMakeErrPosString( sStmtInfo.mSQLString,
                                           sQuerySpecNode->mDistinct->mRecColList->mHead->mExprPtr->mPosition,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_PSEUDO_COLUMN_NOT_ALLOWED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SEQUENCE_NUMBER_NOT_ALLOWED,
                      qlgMakeErrPosString( sStmtInfo.mSQLString,
                                           sQueryExprList->mPseudoColExprList->mHead->mExprPtr->mPosition,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}




/**
 * @brief 해당 Expression이 Group By 절에서의 구성이 유효한지 체크한다.
 * @param[in]      aStmtInfo            Stmt Information
 * @param[in]      aExpr                Expression
 * @param[in]      aEnv                 Environment
 */
stlStatus qlvValidateValueExprWithoutGroup( qlvStmtInfo          * aStmtInfo,
                                            qlvInitExpression    * aExpr,
                                            qllEnv               * aEnv )
{
    stlInt32              i     = 0;
    qlvInitExpression   * sExpr = NULL;


    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Value Expression Validation
     */

    sExpr = aExpr;

    STL_RAMP( BEGIN_VALIDATION );

    switch( sExpr->mType )
    {
        case QLV_EXPR_TYPE_VALUE :
        case QLV_EXPR_TYPE_BIND_PARAM :
        case QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT :
        case QLV_EXPR_TYPE_REFERENCE :
        case QLV_EXPR_TYPE_OUTER_COLUMN :
        case QLV_EXPR_TYPE_PSEUDO_COLUMN :
        case QLV_EXPR_TYPE_PSEUDO_ARGUMENT :
        case QLV_EXPR_TYPE_AGGREGATION :
        case QLV_EXPR_TYPE_SUB_QUERY :
            {
                /* Do Nothing */
                break;
            }
        case QLV_EXPR_TYPE_COLUMN :
            {
                STL_THROW( RAMP_ERR_NOT_A_GROUP_FUNCTION );
            }
        case QLV_EXPR_TYPE_FUNCTION :
            {
                for( i = 0; i < sExpr->mExpr.mFunction->mArgCount; i++ )
                {
                    STL_TRY( qlvValidateValueExprWithoutGroup(
                                 aStmtInfo,
                                 sExpr->mExpr.mFunction->mArgs[i],
                                 aEnv )
                             == STL_SUCCESS );
                }
                break;
            }
        case QLV_EXPR_TYPE_CAST :
            {
                for( i = 0; i < DTL_CAST_INPUT_ARG_CNT; i++ )
                {
                    STL_TRY( qlvValidateValueExprWithoutGroup(
                                 aStmtInfo,
                                 sExpr->mExpr.mTypeCast->mArgs[i],
                                 aEnv )
                             == STL_SUCCESS );
                }
                break;
            }
        case QLV_EXPR_TYPE_INNER_COLUMN :
            {
                sExpr = sExpr->mExpr.mInnerColumn->mOrgExpr;
                STL_THROW( BEGIN_VALIDATION );
                break;
            }
        case QLV_EXPR_TYPE_CASE_EXPR :
            {
                for( i = 0; i < sExpr->mExpr.mCaseExpr->mCount; i++ )
                {
                    STL_TRY( qlvValidateValueExprWithoutGroup(
                                 aStmtInfo,
                                 sExpr->mExpr.mCaseExpr->mWhenArr[i],
                                 aEnv )
                             == STL_SUCCESS );

                    STL_TRY( qlvValidateValueExprWithoutGroup(
                                 aStmtInfo,
                                 sExpr->mExpr.mCaseExpr->mThenArr[i],
                                 aEnv )
                             == STL_SUCCESS );
                }

                STL_TRY( qlvValidateValueExprWithoutGroup(
                             aStmtInfo,
                             sExpr->mExpr.mCaseExpr->mDefResult,
                             aEnv )
                         == STL_SUCCESS );
                
                break;
            }
        case QLV_EXPR_TYPE_LIST_FUNCTION :
            {
                for( i = 0; i < sExpr->mExpr.mListFunc->mArgCount; i++ )
                {
                    STL_TRY( qlvValidateValueExprWithoutGroup(
                                 aStmtInfo,
                                 sExpr->mExpr.mListFunc->mArgs[i],
                                 aEnv )
                             == STL_SUCCESS );
                }
                break;
            }
        case QLV_EXPR_TYPE_LIST_COLUMN :
            {
                for( i = 0; i < sExpr->mExpr.mListCol->mArgCount; i++ )
                {
                    STL_TRY( qlvValidateValueExprWithoutGroup(
                                 aStmtInfo,
                                 sExpr->mExpr.mListCol->mArgs[i],
                                 aEnv )
                             == STL_SUCCESS );
                }
                break;
            }
        case QLV_EXPR_TYPE_ROW_EXPR :
            {
                for( i = 0; i < sExpr->mExpr.mRowExpr->mArgCount; i++ )
                {
                    STL_TRY( qlvValidateValueExprWithoutGroup(
                                 aStmtInfo,
                                 sExpr->mExpr.mRowExpr->mArgs[i],
                                 aEnv )
                             == STL_SUCCESS );
                }
                break;
            }
        default :
            {
                STL_DASSERT( 0 );
                break;
            }
    }


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_A_GROUP_FUNCTION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_A_GROUP_FUNCTION,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sExpr->mPosition,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief 해당 Expression이 Group By 절에서의 구성이 유효한지 체크한다.
 * @param[in]      aStmtInfo            Stmt Information
 * @param[in]      aDistinct            Distinct를 위한 Instant Table
 * @param[in]      aGroupBy             Group By Init Node
 * @param[in]      aTargetCnt           Target Expression Count 
 * @param[in, out] aTargetExprList      Target Expression List (Array)
 * @param[in]      aEnv                 Environment
 */
stlStatus qlvValidateValueExprByDistinct( qlvStmtInfo          * aStmtInfo,
                                          qlvInitInstantNode  ** aDistinct,
                                          qlvGroupBy           * aGroupBy,
                                          stlInt32               aTargetCnt,
                                          qlvInitTarget        * aTargetExprList,
                                          qllEnv               * aEnv )
{
    qlvInitInstantNode  * sDistinct  = NULL;
    qlvInitExpression   * sExpr      = NULL;
    knlRegionMem        * sRegionMem = NULL;
    stlInt32              sLoop      = 0;
    stlBool               sNeedDist  = STL_FALSE;
    stlBool               sIsExist   = STL_TRUE;
    stlBool               sIsConst   = STL_TRUE;
    stlUInt8              sColFlag   = 0;    
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDistinct != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTargetCnt > 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTargetExprList != NULL, QLL_ERROR_STACK(aEnv) );
    

    /**
     * Distinct 대상 Expression이 Group By 절의 Key Column이 모두 사용되었는지 확인
     */

    sRegionMem = QLL_INIT_PLAN( aStmtInfo->mDBCStmt );

    for( sLoop = 0 ; sLoop < aTargetCnt ; sLoop++ )
    {   
        sExpr = aTargetExprList[ sLoop ].mExpr;
        
        switch( sExpr->mIterationTime )
        {
            case DTL_ITERATION_TIME_FOR_EACH_AGGREGATION :
            case DTL_ITERATION_TIME_FOR_EACH_EXPR :
            case DTL_ITERATION_TIME_FOR_EACH_QUERY :
                {
                    sIsConst = STL_FALSE;

                    if( aGroupBy != NULL )
                    {
                        sIsExist = STL_TRUE;
                        STL_TRY( qlnoIsExistExprFromInstant( & aGroupBy->mInstant,
                                                             sExpr,
                                                             STL_TRUE,  /* key column list */
                                                             & sIsExist,
                                                             NULL,
                                                             sRegionMem,
                                                             aEnv )
                                 == STL_SUCCESS );

                        if( sIsExist == STL_FALSE )
                        {
                            sNeedDist = STL_TRUE;
                            STL_THROW( RAMP_MAKE_DISTINCT );
                        }
                        else
                        {
                            break;
                        }
                    }
                    else
                    {
                        sNeedDist = STL_TRUE;
                        STL_THROW( RAMP_MAKE_DISTINCT );
                        break;
                    }
                }
            case DTL_ITERATION_TIME_FOR_EACH_STATEMENT :
            case DTL_ITERATION_TIME_NONE :
                {
                    sNeedDist = STL_TRUE;
                    break;
                }
            default :
                {
                    STL_DASSERT( 0 );
                    break;
                }
        }
    }


    /**
     * Instant Table 공간 할당
     */

    STL_RAMP( RAMP_MAKE_DISTINCT );

    STL_TRY_THROW( sNeedDist == STL_TRUE, RAMP_FINISH );

    if( ( sIsConst == STL_TRUE ) &&
        ( aGroupBy != NULL ) )
    {
        STL_DASSERT( aGroupBy->mInstant.mKeyColList->mCount > 0 );
        
        if( aGroupBy->mInstant.mKeyColList->mCount == 1 )
        {
            switch( aGroupBy->mInstant.mKeyColList->mHead->mExprPtr->mIterationTime )
            {
                case DTL_ITERATION_TIME_FOR_EACH_STATEMENT :
                case DTL_ITERATION_TIME_NONE :
                    STL_THROW( RAMP_FINISH );
                default :
                    break;
            }
        }
    }

    STL_TRY( knlAllocRegionMem( sRegionMem,
                                STL_SIZEOF( qlvInitInstantNode ),
                                (void **) & sDistinct,
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    STL_TRY( qlnoInitInstant( sDistinct,
                              QLV_NODE_TYPE_GROUP_HASH_INSTANT,
                              NULL,
                              sRegionMem,
                              aEnv )
             == STL_SUCCESS );

    
    /**
     * Target Expression 평가
     */

    DTL_SET_INDEX_COLUMN_FLAG( sColFlag,
                               DTL_KEYCOLUMN_INDEX_ORDER_ASC,
                               DTL_KEYCOLUMN_INDEX_NULLABLE_TRUE,
                               DTL_KEYCOLUMN_INDEX_NULL_ORDER_LAST );

    for( sLoop = 0 ; sLoop < aTargetCnt ; sLoop++ )
    {   
        /**
         * distinct 수행을 위한 Instant Table 정보 구축
         */
                    
        sExpr = aTargetExprList[ sLoop ].mExpr;

        if( ( sLoop == 0 ) && ( sIsConst == STL_TRUE ) )
        {
            /* check key column list */
            STL_TRY( qlnoAddKeyColToInstant( aStmtInfo->mSQLString,
                                             sDistinct,
                                             sExpr,
                                             sColFlag,
                                             & sExpr,
                                             sRegionMem,
                                             aEnv )
                     == STL_SUCCESS );
            break;
        }
        else
        {
            switch( sExpr->mIterationTime )
            {
                case DTL_ITERATION_TIME_FOR_EACH_AGGREGATION :
                case DTL_ITERATION_TIME_FOR_EACH_EXPR :
                case DTL_ITERATION_TIME_FOR_EACH_QUERY :
                    {
                        /* check key column list */
                        STL_TRY( qlnoAddKeyColToInstant( aStmtInfo->mSQLString,
                                                         sDistinct,
                                                         sExpr,
                                                         sColFlag,
                                                         & sExpr,
                                                         sRegionMem,
                                                         aEnv )
                                 == STL_SUCCESS );

                        /* add read column list */
                        STL_TRY( qlnoAddReadColToInstant( sDistinct,
                                                          sExpr,
                                                          & aTargetExprList[ sLoop ].mExpr,
                                                          sRegionMem,
                                                          aEnv )
                                 == STL_SUCCESS );
                        break;
                    }
                case DTL_ITERATION_TIME_FOR_EACH_STATEMENT :
                case DTL_ITERATION_TIME_NONE :
                    {
                        /* Do Nothing */
                        break;
                    }
                default :
                    {
                        STL_DASSERT( 0 );
                        break;
                    }
            }
        }
    }


    /**
     * OUTPUT 설정
     */

    STL_RAMP( RAMP_FINISH );
    
    if( sNeedDist == STL_TRUE )
    {
        *aDistinct = sDistinct;
    }
    else
    {
        *aDistinct = NULL;
    }

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


stlBool qlnvHasNestedAggr( qlvInitExpression * aExpr )
{
    qlvInitExpression * sExpr = aExpr;
    stlInt32            i;

    if( sExpr->mIterationTime != DTL_ITERATION_TIME_FOR_EACH_AGGREGATION )
    {
        return STL_FALSE;
    }
    else
    {
        /* Do Nothing */
    }

    STL_RAMP( RAMP_FIRST );
    
    switch( sExpr->mType )
    {
        case QLV_EXPR_TYPE_VALUE :
        case QLV_EXPR_TYPE_BIND_PARAM :
        case QLV_EXPR_TYPE_COLUMN :
        case QLV_EXPR_TYPE_PSEUDO_COLUMN :
        case QLV_EXPR_TYPE_PSEUDO_ARGUMENT :
        case QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT :
        case QLV_EXPR_TYPE_REFERENCE :
        case QLV_EXPR_TYPE_OUTER_COLUMN :
        case QLV_EXPR_TYPE_ROWID_COLUMN :
        case QLV_EXPR_TYPE_SUB_QUERY :
            {
                return STL_FALSE;
            }
        case QLV_EXPR_TYPE_INNER_COLUMN :
            {
                sExpr = sExpr->mExpr.mInnerColumn->mOrgExpr;
                STL_THROW( RAMP_FIRST );
            }
        case QLV_EXPR_TYPE_AGGREGATION :
            {
                return sExpr->mExpr.mAggregation->mHasNestedAggr;
            }
        case QLV_EXPR_TYPE_FUNCTION :
            {
                for( i = 0; i < sExpr->mExpr.mFunction->mArgCount; i++ )
                {
                    if( qlnvHasNestedAggr( sExpr->mExpr.mFunction->mArgs[i] ) == STL_TRUE )
                    {
                        return STL_TRUE;
                    }
                }
                break;
            }
        case QLV_EXPR_TYPE_CAST :
            {
                return qlnvHasNestedAggr( sExpr->mExpr.mTypeCast->mArgs[0] );
            }
        case QLV_EXPR_TYPE_CASE_EXPR :
            {
                for( i = 0; i < sExpr->mExpr.mCaseExpr->mCount; i++ )
                {
                    if( qlnvHasNestedAggr( sExpr->mExpr.mCaseExpr->mWhenArr[i] ) == STL_TRUE )
                    {
                        return STL_TRUE;
                    }
                    
                    if( qlnvHasNestedAggr( sExpr->mExpr.mCaseExpr->mThenArr[i] ) == STL_TRUE )
                    {
                        return STL_TRUE;
                    }
                }

                if( qlnvHasNestedAggr( sExpr->mExpr.mCaseExpr->mDefResult ) == STL_TRUE )
                {
                    return STL_TRUE;
                }
                
                break;
            }
        case QLV_EXPR_TYPE_LIST_FUNCTION :
            {
                for( i = 0; i < sExpr->mExpr.mListFunc->mArgCount; i++ )
                {
                    if( qlnvHasNestedAggr( sExpr->mExpr.mListFunc->mArgs[i] ) == STL_TRUE )
                    {
                        return STL_TRUE;
                    }
                }
                break;
            }
        case QLV_EXPR_TYPE_LIST_COLUMN :
            {
                for( i = 0; i < sExpr->mExpr.mListCol->mArgCount; i++ )
                {
                    if( qlnvHasNestedAggr( sExpr->mExpr.mListCol->mArgs[i] ) == STL_TRUE )
                    {
                        return STL_TRUE;
                    }
                }
                break;
            }
        case QLV_EXPR_TYPE_ROW_EXPR :
            {
                for( i = 0; i < sExpr->mExpr.mRowExpr->mArgCount; i++ )
                {
                    if( qlnvHasNestedAggr( sExpr->mExpr.mRowExpr->mArgs[i] ) == STL_TRUE )
                    {
                        return STL_TRUE;
                    }
                }
                break;
            }
        default :
            {
                STL_DASSERT( 0 );
                break;
            }
    }
    
    return STL_FALSE;
}


/**
 * @brief 해당 Expression에 포함된 최상위 Aggregation을 Nested Aggregation으로 설정한다.
 * @param[in]      aStmtInfo            Stmt Information
 * @param[in,out]  aInitExprList        Query Expression List
 * @param[in]      aExpr                Expression
 * @param[in]      aEnv                 Environment
 */
stlStatus qlvSetNestedAggr( qlvStmtInfo          * aStmtInfo,
                            qlvInitExprList      * aInitExprList,
                            qlvInitExpression    * aExpr,
                            qllEnv               * aEnv )
{
    stlInt32  sLoop = 0;
    
    switch( aExpr->mType )
    {
        case QLV_EXPR_TYPE_VALUE :
        case QLV_EXPR_TYPE_BIND_PARAM :
        case QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT :
        case QLV_EXPR_TYPE_REFERENCE :
        case QLV_EXPR_TYPE_OUTER_COLUMN :
        case QLV_EXPR_TYPE_INNER_COLUMN :
        case QLV_EXPR_TYPE_COLUMN :
        case QLV_EXPR_TYPE_PSEUDO_COLUMN :
        case QLV_EXPR_TYPE_SUB_QUERY :
            {
                break;
            }
        case QLV_EXPR_TYPE_AGGREGATION :
            {
                if( aExpr->mExpr.mAggregation->mHasNestedAggr == STL_TRUE )
                {
                    /* Do Nothing */
                }
                else
                {
                    STL_TRY( qlvRemoveExprToRefExprList( aInitExprList->mAggrExprList,
                                                         aExpr,
                                                         aEnv )
                             == STL_SUCCESS );
                            
                    aExpr->mExpr.mAggregation->mHasNestedAggr = STL_TRUE;
                    STL_TRY( qlvAddExprToRefExprList( aInitExprList->mNestedAggrExprList,
                                                      aExpr,
                                                      STL_FALSE,
                                                      QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                      aEnv )
                             == STL_SUCCESS );
                }
                break;
            }    
        case QLV_EXPR_TYPE_FUNCTION :
            {
                for( sLoop = 0 ; sLoop < aExpr->mExpr.mFunction->mArgCount ; sLoop++ )
                {
                    STL_TRY( qlvSetNestedAggr( aStmtInfo,
                                               aInitExprList,
                                               aExpr->mExpr.mFunction->mArgs[ sLoop ],
                                               aEnv )
                             == STL_SUCCESS );
                }
                break;
            }
        case QLV_EXPR_TYPE_CAST :
            {
                for( sLoop = 0 ; sLoop < DTL_CAST_INPUT_ARG_CNT ; sLoop++ )
                {
                    STL_TRY( qlvSetNestedAggr( aStmtInfo,
                                               aInitExprList,
                                               aExpr->mExpr.mTypeCast->mArgs[ sLoop ],
                                               aEnv )
                             == STL_SUCCESS );
                }
                break;
            }
        case QLV_EXPR_TYPE_CASE_EXPR :
            {
                for( sLoop = 0 ; sLoop < aExpr->mExpr.mCaseExpr->mCount ; sLoop++ )
                {
                    STL_TRY( qlvSetNestedAggr( aStmtInfo,
                                               aInitExprList,
                                               aExpr->mExpr.mCaseExpr->mWhenArr[ sLoop ],
                                               aEnv )
                             == STL_SUCCESS );

                    STL_TRY( qlvSetNestedAggr( aStmtInfo,
                                               aInitExprList,
                                               aExpr->mExpr.mCaseExpr->mThenArr[ sLoop ],
                                               aEnv )
                             == STL_SUCCESS );
                }
            
                STL_TRY( qlvSetNestedAggr( aStmtInfo,
                                           aInitExprList,
                                           aExpr->mExpr.mCaseExpr->mDefResult,
                                           aEnv )
                         == STL_SUCCESS );
                break;
            }
        case QLV_EXPR_TYPE_LIST_FUNCTION :
            {
                for( sLoop = 0 ; sLoop < aExpr->mExpr.mListFunc->mArgCount ; sLoop++ )
                {
                    STL_TRY( qlvSetNestedAggr( aStmtInfo,
                                               aInitExprList,
                                               aExpr->mExpr.mListFunc->mArgs[ sLoop ],
                                               aEnv )
                             == STL_SUCCESS );
                }
                break;
            }
        case QLV_EXPR_TYPE_LIST_COLUMN :
            {
                for( sLoop = 0 ; sLoop < aExpr->mExpr.mListCol->mArgCount ; sLoop++ )
                {
                    STL_TRY( qlvSetNestedAggr( aStmtInfo,
                                               aInitExprList,
                                               aExpr->mExpr.mListCol->mArgs[ sLoop ],
                                               aEnv )
                             == STL_SUCCESS );
                }
                break;
            }
        case QLV_EXPR_TYPE_ROW_EXPR :
            {
                for( sLoop = 0 ; sLoop < aExpr->mExpr.mRowExpr->mArgCount ; sLoop++ )
                {
                    STL_TRY( qlvSetNestedAggr( aStmtInfo,
                                               aInitExprList,
                                               aExpr->mExpr.mRowExpr->mArgs[ sLoop ],
                                               aEnv )
                             == STL_SUCCESS );
                }
                break;
            }
        case QLV_EXPR_TYPE_PSEUDO_ARGUMENT :
        default :
            {
                STL_DASSERT( 0 );
                break;
            }
    }

    
    return STL_SUCCESS;

    /* STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED ) */
    /* { */
    /*     stlPushError( STL_ERROR_LEVEL_ABORT, */
    /*                   QLL_ERRCODE_NOT_IMPLEMENTED, */
    /*                   qlgMakeErrPosString( aStmtInfo->mSQLString, */
    /*                                        aExpr->mPosition, */
    /*                                        aEnv ), */
    /*                   QLL_ERROR_STACK(aEnv), */
    /*                   "qlvSetNestedAggr()" ); */
    /* } */

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Aggregation Expression List에 distinct 조건이 포함된 Aggregation이 존재하는지 여부
 * @param[in]      aAggrExprList       Aggregation Expression List
 * @param[out]     aHasDistinct        distinct 조건이 포함된 Aggregation이 존재 여부
 * @param[in]      aEnv                Environment
 */
stlStatus qlnvHasDistinctAggr( qlvRefExprList  * aAggrExprList,
                               stlBool         * aHasDistinct,
                               qllEnv          * aEnv )
{
    qlvRefExprItem  * sExprItem = NULL;
    stlBool           sHasDistinct = STL_FALSE;

    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aAggrExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aHasDistinct != NULL, QLL_ERROR_STACK(aEnv) );
    
    sExprItem = aAggrExprList->mHead;
    
    while( sExprItem != NULL )
    {
        STL_DASSERT( sExprItem->mExprPtr->mType == QLV_EXPR_TYPE_AGGREGATION );
        
        if( sExprItem->mExprPtr->mExpr.mAggregation->mIsDistinct == STL_TRUE )
        {
            sHasDistinct = STL_TRUE;
            break;
        }
        else
        {
            sExprItem = sExprItem->mNext;
        }
    }

    *aHasDistinct = sHasDistinct;
    
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/** @} qlnv */
