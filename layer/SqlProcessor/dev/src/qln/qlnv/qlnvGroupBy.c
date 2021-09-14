/*******************************************************************************
 * qlnvGroupBy.c
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
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
 * @file qlnvGroupBy.c
 * @brief SQL Processor Layer Validation - Group By Functions
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnv
 * @{
 */


/****************************************************
 * Functions
 ****************************************************/ 

/**
 * @brief Group By List를 생성한다.
 * @param[in]      aStmtInfo            Stmt Information
 * @param[in]      aValidationObjList   Validation Object List
 * @param[in]      aRefTableList        Reference Table List
 * @param[in]      aGroupBySource       Group By Clause Source
 * @param[in]      aHavingSource        Having Clause Source
 * @param[out]     aGroupByList         Group By List
 * @param[out]     aHasAggrDistinct     Having 절에 Distinct 포함된 Aggregation이 존재하는지 여부
 * @param[in]      aEnv                 Environment
 */
stlStatus qlvValidateGroupBy( qlvStmtInfo       * aStmtInfo,
                              ellObjectList     * aValidationObjList,
                              qlvRefTableList   * aRefTableList,
                              qlpGroupBy        * aGroupBySource,
                              qlpHaving         * aHavingSource,
                              qlvGroupBy        * aGroupByList,
                              stlBool           * aHasAggrDistinct,
                              qllEnv            * aEnv )
{
    qlvGroupBy          * sGroupBy          = NULL;
    qlpGroupingElem     * sGroupingElem     = NULL;
    qlvInitExpression   * sGroupingExpr     = NULL;
    qlvInitExpression   * sHavingExpr       = NULL;

    qlvInitNode         * sTableNode        = NULL;
    qlvRelationName     * sRelationName     = NULL;
    
    qlpListElement      * sGroupListElem    = NULL;
    qlpListElement      * sExprListElem     = NULL;
    stlUInt8              sColFlag          = 0;

    qlvInitValue        * sNullValue        = NULL;
    qlvRefTableList     * sRefTableList     = NULL;

    
    /**********************************************************
     * Parameter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValidationObjList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefTableList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aGroupByList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aHasAggrDistinct != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aGroupBySource != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aGroupBySource->mType == QLL_GROUP_BY_TYPE, QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * Group By Clause Validation
     **********************************************************/

    /**
     * @todo Grouping Element가 Simply인 경우만 고려하여 구현됨.
     *  <BR> OLAP 지원시 validation structure 구조 변경이 필요
     */

    STL_DASSERT( aRefTableList->mCount >= 1 );

    sGroupBy = aGroupByList;

    sTableNode = aRefTableList->mHead->mTableNode;

    switch( sTableNode->mType )
    {
        case QLV_NODE_TYPE_QUERY_SET :
        case QLV_NODE_TYPE_QUERY_SPEC :
        case QLV_NODE_TYPE_JOIN_TABLE :
            {
                sRelationName = NULL;
                break;
            }
        case QLV_NODE_TYPE_BASE_TABLE :
            {
                sRelationName = ((qlvInitBaseTableNode *) sTableNode)->mRelationName;
                break;
            }
        case QLV_NODE_TYPE_SUB_TABLE :
            {
                sRelationName = ((qlvInitSubTableNode *) sTableNode)->mRelationName;
                break;
            }
        case QLV_NODE_TYPE_FLAT_INSTANT :
        case QLV_NODE_TYPE_SORT_INSTANT :
        case QLV_NODE_TYPE_HASH_INSTANT :
        case QLV_NODE_TYPE_GROUP_HASH_INSTANT :
        case QLV_NODE_TYPE_GROUP_SORT_INSTANT :
        case QLV_NODE_TYPE_SORT_JOIN_INSTANT :
        case QLV_NODE_TYPE_HASH_JOIN_INSTANT :
        default :
            {
                STL_DASSERT( 0 );
                break;
            }
    }

    
    /**
     * @todo hash vs sort로 구분
     */
        
    STL_TRY( qlnoInitInstant( & sGroupBy->mInstant,
                              QLV_NODE_TYPE_GROUP_HASH_INSTANT,
                              sRelationName,
                              QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                              aEnv )
             == STL_SUCCESS );
    sGroupBy->mGroupByType = QLP_GROUPING_TYPE_ORDINARY;

    /* Default Grouping Key Flag 설정 */
    DTL_SET_INDEX_COLUMN_FLAG( sColFlag,
                               DTL_KEYCOLUMN_INDEX_ORDER_ASC,
                               DTL_KEYCOLUMN_INDEX_NULLABLE_TRUE,
                               DTL_KEYCOLUMN_INDEX_NULL_ORDER_LAST );
    
    
    /**
     * Validation Grouping Expression
     */
    
    QLP_LIST_FOR_EACH( aGroupBySource->mGroupingElemList, sGroupListElem )
    {
        sGroupingElem = (qlpGroupingElem *) QLP_LIST_GET_POINTER_VALUE( sGroupListElem );

        if( sGroupingElem == NULL )
        {
            continue;
        }
        else
        {
            /* Do Nothing */
        }
        
        switch( sGroupingElem->mGroupingType )
        {
            case QLP_GROUPING_TYPE_ORDINARY :
                {
                    /**
                     * Group By Item 구성
                     * @remark OLAP을 고려하지 않고,
                     *    <BR> Grouping Element의 구성이 simply인 것으로 제한하여
                     *    <BR> ORDINARY Element 분석시 Concatenation 까지 고려하여 코드 작성함.
                     */

                    sGroupBy->mGroupByType = sGroupingElem->mGroupingType;

                    QLP_LIST_FOR_EACH( sGroupingElem->mGroupingElem, sExprListElem )
                    {
                        /**
                         * Validate Expression
                         */

                        STL_TRY( qlvValidateValueExpr(
                                     aStmtInfo,
                                     QLV_EXPR_PHRASE_GROUPBY,
                                     aValidationObjList,
                                     aRefTableList,
                                     STL_FALSE,
                                     STL_FALSE,  /* Row Expr */
                                     0,
                                     (qllNode *) QLP_LIST_GET_POINTER_VALUE( sExprListElem ),
                                     & sGroupingExpr,
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

                        STL_TRY( qlvRewriteExpr( sGroupingExpr,
                                                 DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                                 aStmtInfo->mQueryExprList,
                                                 & sGroupingExpr,
                                                 QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                 aEnv )
                                 == STL_SUCCESS );

                        
                        /**
                         * Expression이 Constant Expression이면 INSTANT TABLE의 column으로 구성하지 않음
                         */

                        switch( sGroupingExpr->mIterationTime )
                        {
                            case DTL_ITERATION_TIME_FOR_EACH_AGGREGATION :
                            case DTL_ITERATION_TIME_FOR_EACH_EXPR :
                            case DTL_ITERATION_TIME_FOR_EACH_QUERY :
                                {
                                    /**
                                     * Add Key Column To Group Instant
                                     */
                        
                                    STL_TRY( qlnoAddKeyColToInstant( aStmtInfo->mSQLString,
                                                                     & sGroupBy->mInstant,
                                                                     sGroupingExpr,
                                                                     sColFlag,
                                                                     & sGroupingExpr,
                                                                     QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
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
                    
                    break;
                }
            case QLP_GROUPING_TYPE_GROUPING_SET :
            case QLP_GROUPING_TYPE_EMPTY :
            case QLP_GROUPING_TYPE_ROLL_UP :
            case QLP_GROUPING_TYPE_CUBE :
                {
                    STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                    break;
                }
            default :
                {
                    STL_DASSERT( 0 );
                    break;
                }

        }
    }


    if( sGroupBy->mInstant.mKeyColCnt == 0 )
    {
        /**
         * NULL Expression 생성
         */

        /* Code Null Value 생성 */
        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                    STL_SIZEOF( qlvInitValue ),
                                    (void **) & sNullValue,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sNullValue,
                   0x00,
                   STL_SIZEOF( qlvInitValue ) );

        /* Code Null Value 설정 */
        sNullValue->mValueType       = QLV_VALUE_TYPE_NULL;
        sNullValue->mInternalBindIdx = QLV_INTERNAL_BIND_VARIABLE_IDX_NA;
        sNullValue->mData.mString    = NULL;
    
        /* Code Expression 공간 할당 */
        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                    STL_SIZEOF( qlvInitExpression ),
                                    (void **) & sGroupingExpr,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sGroupingExpr,
                   0x00,
                   STL_SIZEOF( qlvInitExpression ) );

        /* Code Expression 설정 */
        QLV_VALIDATE_INCLUDE_EXPR_CNT( 1, aEnv );

        sGroupingExpr->mType           = QLV_EXPR_TYPE_VALUE;
        sGroupingExpr->mPosition       = 0;
        sGroupingExpr->mIncludeExprCnt = 1;
        sGroupingExpr->mExpr.mValue    = sNullValue;
        sGroupingExpr->mRelatedFuncId  = KNL_BUILTIN_FUNC_INVALID;
        sGroupingExpr->mIterationTime  = DTL_ITERATION_TIME_NONE;
        sGroupingExpr->mColumnName     = NULL;

        
        /**
         * Rewrite Expression
         */

        STL_TRY( qlvRewriteExpr( sGroupingExpr,
                                 DTL_ITERATION_TIME_NONE,
                                 aStmtInfo->mQueryExprList,
                                 & sGroupingExpr,
                                 QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                 aEnv )
                 == STL_SUCCESS );

        
        /**
         * Add Key Column To Group Instant
         */
                        
        STL_TRY( qlnoAddKeyColToInstant( aStmtInfo->mSQLString,
                                         & sGroupBy->mInstant,
                                         sGroupingExpr,
                                         sColFlag,
                                         & sGroupingExpr,
                                         QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                         aEnv )
                 == STL_SUCCESS );

        STL_DASSERT( sGroupBy->mInstant.mKeyColCnt == 1 );
    }
    else
    {
        /* Do Nothing */
    }
    
    
    /**********************************************************
     * Having Clause Validation
     **********************************************************/

    /**
     * validate Having Clause
     */

    if( aHavingSource == NULL )
    {
        sGroupBy->mHaving = NULL;
    }
    else
    {
        /**
         * validate expression
         * @remark Having clause 의 aggregation은 중첩될 수 없다.
         */

        STL_TRY( qlvCreateRefTableList( &QLL_VALIDATE_MEM( aEnv ),
                                        aRefTableList,
                                        & sRefTableList,
                                        aEnv )
                 == STL_SUCCESS );

        sGroupBy->mInstant.mTableNode = aRefTableList->mHead->mTableNode;

        STL_TRY( qlvSetNodeToRefTableItem( sRefTableList,
                                           (qlvInitNode *) & sGroupBy->mInstant,
                                           aEnv )
                 == STL_SUCCESS );
        
        STL_TRY( qlvValidateValueExpr(
                     aStmtInfo,
                     QLV_EXPR_PHRASE_HAVING,
                     aValidationObjList,
                     sRefTableList,
                     STL_FALSE,
                     STL_FALSE,  /* Row Expr */
                     1,
                     aHavingSource->mCondition,
                     & sHavingExpr,
                     NULL,
                     KNL_BUILTIN_FUNC_INVALID,
                     DTL_ITERATION_TIME_FOR_EACH_AGGREGATION,
                     DTL_ITERATION_TIME_FOR_EACH_AGGREGATION,
                     QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                     aEnv )
                 == STL_SUCCESS );


        /**
         * Rewrite Expression
         */

        STL_TRY( qlvRewriteExpr( sHavingExpr,
                                 DTL_ITERATION_TIME_FOR_EACH_AGGREGATION,
                                 aStmtInfo->mQueryExprList,
                                 & sGroupBy->mHaving,
                                 QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                 aEnv )
                 == STL_SUCCESS );


        /**
         * Having 절에 Distinct 포함된 Aggregation이 존재하는지 여부 확인
         */
        
        STL_TRY( qlnvHasDistinctAggr( aStmtInfo->mQueryExprList->mAggrExprList,
                                      aHasAggrDistinct,
                                      aEnv )
                 == STL_SUCCESS );


        /**
         * validate having clause
         */

        STL_TRY( qlvValidateValueExprByGroup( aStmtInfo,
                                              & sGroupBy->mInstant,
                                              sGroupBy->mHaving,
                                              STL_FALSE,
                                              0,
                                              & sGroupBy->mHaving,
                                              aEnv )
                 == STL_SUCCESS );
    }

    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sGroupingElem->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      "qlvValidateGroupBy()" );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 해당 Expression이 Group By 절에서의 구성이 유효한지 체크한다.
 * @param[in]      aStmtInfo                  Stmt Information
 * @param[in,out]  aInstantTable              Group Instant Table
 * @param[in]      aExpr                      Expression
 * @param[in]      aHasNestedAggr             Nested Aggregation 포함 여부
 * @param[in]      aAggrDepth                 Aggregation Depth
 * @param[out]     aResultExpr                Result Expression
 * @param[in]      aEnv                       Environment
 */
stlStatus qlvValidateValueExprByGroup( qlvStmtInfo          * aStmtInfo,
                                       qlvInitInstantNode   * aInstantTable,
                                       qlvInitExpression    * aExpr,
                                       stlBool                aHasNestedAggr,
                                       stlInt32               aAggrDepth,
                                       qlvInitExpression   ** aResultExpr,
                                       qllEnv               * aEnv )
{
    stlInt32             sLoop      = 0;
    qlvInitExpression  * sExpr      = aExpr;
    qlvInitExpression  * sNewExpr   = NULL;
    stlBool              sIsExist   = STL_FALSE;
    

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInstantTable != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aResultExpr != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * 동일한 Column Expression이 Group By 절에 있는지 검사 (사용가능 여부도 검사)
     * => 없으면 추가
     */

    STL_TRY_THROW( aAggrDepth <= 1,
                   RAMP_ERR_NESTED_GROUP_IS_NESTED_TOO_DEEPLY );
    
    switch( sExpr->mType )
    {
        case QLV_EXPR_TYPE_VALUE :
        case QLV_EXPR_TYPE_BIND_PARAM :
        case QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT :
        case QLV_EXPR_TYPE_OUTER_COLUMN :
        case QLV_EXPR_TYPE_REFERENCE :
            {
                *aResultExpr = sExpr;
                break;
            }
        case QLV_EXPR_TYPE_INNER_COLUMN :
            {
                if( (qlvInitNode*) aInstantTable == sExpr->mExpr.mInnerColumn->mRelationNode )
                {
                    if( aHasNestedAggr == STL_TRUE )
                    {
                        STL_DASSERT( aAggrDepth == 1 );
                        
                        /* add read column list */
                        STL_TRY( qlnoAddReadColToInstant( aInstantTable,
                                                          sExpr,
                                                          aResultExpr,
                                                          QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                          aEnv )
                                 == STL_SUCCESS );
                    }
                    else
                    {
                        STL_DASSERT( aAggrDepth == 0 );
                        
                        /* add read column list */
                        STL_TRY( qlnoAddReadColToInstant( aInstantTable,
                                                          sExpr,
                                                          aResultExpr,
                                                          QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                          aEnv )
                                 == STL_SUCCESS );
                    }
                }
                else
                {
                    if( aHasNestedAggr == STL_TRUE )
                    {
                        if( aAggrDepth == 0 )
                        {
                            STL_THROW( RAMP_ERR_NOT_A_GROUP_FUNCTION );
                        }
                        else
                        {
                            STL_DASSERT( aAggrDepth == 1 );
                            
                            /* check key column list */
                            STL_TRY( qlnoIsExistExprFromInstant( aInstantTable,
                                                                 sExpr,
                                                                 STL_TRUE,  /* key column list */
                                                                 & sIsExist,
                                                                 NULL,
                                                                 QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                                 aEnv )
                                     == STL_SUCCESS );
                            STL_TRY_THROW( sIsExist == STL_TRUE, RAMP_ERR_NOT_A_GROUP_FUNCTION );
                        
                            /* add read column list */
                            STL_TRY( qlnoAddReadColToInstant( aInstantTable,
                                                              sExpr,
                                                              aResultExpr,
                                                              QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                              aEnv )
                                     == STL_SUCCESS );
                        }
                    }
                    else
                    {
                        STL_DASSERT( aAggrDepth == 0 );
                        
                        /* check key column list */
                        STL_TRY( qlnoIsExistExprFromInstant( aInstantTable,
                                                             sExpr,
                                                             STL_TRUE,  /* key column list */
                                                             & sIsExist,
                                                             NULL,
                                                             QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                             aEnv )
                                 == STL_SUCCESS );
                        STL_TRY_THROW( sIsExist == STL_TRUE, RAMP_ERR_NOT_A_GROUP_FUNCTION );
                        
                        /* add read column list */
                        STL_TRY( qlnoAddReadColToInstant( aInstantTable,
                                                          sExpr,
                                                          aResultExpr,
                                                          QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                          aEnv )
                                 == STL_SUCCESS );
                    }
                }

                break;
            }
        case QLV_EXPR_TYPE_COLUMN :
        case QLV_EXPR_TYPE_ROWID_COLUMN :
            {
                if( aHasNestedAggr == STL_TRUE )
                {
                    if( aAggrDepth == 0 )
                    {
                        STL_THROW( RAMP_ERR_NOT_A_GROUP_FUNCTION );
                    }
                    else
                    {
                        STL_DASSERT( aAggrDepth == 1 );
                        
                        /* check key column list */
                        STL_TRY( qlnoIsExistExprFromInstant( aInstantTable,
                                                             sExpr,
                                                             STL_TRUE,  /* key column list */
                                                             & sIsExist,
                                                             NULL,
                                                             QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                             aEnv )
                                 == STL_SUCCESS );
                        
                        STL_TRY_THROW( sIsExist == STL_TRUE, RAMP_ERR_NOT_A_GROUP_FUNCTION );
                            
                        /* add read column list */
                        STL_TRY( qlnoAddReadColToInstant( aInstantTable,
                                                          sExpr,
                                                          aResultExpr,
                                                          QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                          aEnv )
                                 == STL_SUCCESS );
                    }
                }
                else
                {
                    STL_DASSERT( aAggrDepth == 0 );
                    
                    /* check key column list */
                    STL_TRY( qlnoIsExistExprFromInstant( aInstantTable,
                                                         sExpr,
                                                         STL_TRUE,  /* key column list */
                                                         & sIsExist,
                                                         NULL,
                                                         QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                         aEnv )
                             == STL_SUCCESS );
                    
                    STL_TRY_THROW( sIsExist == STL_TRUE, RAMP_ERR_NOT_A_GROUP_FUNCTION );
                            
                    /* add read column list */
                    STL_TRY( qlnoAddReadColToInstant( aInstantTable,
                                                      sExpr,
                                                      aResultExpr,
                                                      QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                      aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLV_EXPR_TYPE_AGGREGATION :
            {
                STL_TRY( qlnoIsExistExprFromInstant( aInstantTable,
                                                     sExpr,
                                                     STL_TRUE,  /* key column list */
                                                     & sIsExist,
                                                     NULL,
                                                     QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                     aEnv )
                         == STL_SUCCESS );
                    
                if( sIsExist == STL_TRUE )
                {
                    /* add read column list */
                    STL_TRY( qlnoAddReadColToInstant( aInstantTable,
                                                      sExpr,
                                                      aResultExpr,
                                                      QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                      aEnv )
                             == STL_SUCCESS );
                    break;
                }
                
                if( aHasNestedAggr == STL_TRUE )
                {
                    STL_TRY_THROW( aAggrDepth <= 1,
                                   RAMP_ERR_NESTED_GROUP_IS_NESTED_TOO_DEEPLY );

                    if( aAggrDepth == 0 )
                    {
                        STL_DASSERT( sExpr->mExpr.mAggregation->mHasNestedAggr == STL_TRUE );

                        aAggrDepth++;
                        for( sLoop = 0 ; sLoop < sExpr->mExpr.mAggregation->mArgCount ; sLoop++ )
                        {
                            STL_TRY( qlvValidateValueExprByGroup( aStmtInfo,
                                                                  aInstantTable,
                                                                  sExpr->mExpr.mAggregation->mArgs[ sLoop ],
                                                                  aHasNestedAggr,
                                                                  aAggrDepth,
                                                                  & sNewExpr,
                                                                  aEnv )
                                     == STL_SUCCESS );

                            sExpr->mExpr.mAggregation->mArgs[ sLoop ] = sNewExpr;
                        }
                        
                        *aResultExpr = sExpr;
                    }
                    else
                    {
                        STL_DASSERT( sExpr->mExpr.mAggregation->mHasNestedAggr == STL_FALSE );

                        STL_TRY( qlvCheckExprByGroup( aStmtInfo,
                                                      aInstantTable,
                                                      sExpr,
                                                      aHasNestedAggr,
                                                      aAggrDepth,
                                                      & sExpr,
                                                      aEnv )
                                 == STL_SUCCESS );

                        /* add read column list */
                        STL_TRY( qlnoAddReadColToInstant( aInstantTable,
                                                          sExpr,
                                                          aResultExpr,
                                                          QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                          aEnv )
                                 == STL_SUCCESS );
                    }
                }
                else
                {
                    STL_TRY_THROW( aAggrDepth == 0,
                                   RAMP_ERR_NESTED_GROUP_IS_NESTED_TOO_DEEPLY );

                    STL_DASSERT( sExpr->mExpr.mAggregation->mHasNestedAggr == STL_FALSE );

                    STL_TRY( qlvCheckExprByGroup( aStmtInfo,
                                                  aInstantTable,
                                                  sExpr,
                                                  aHasNestedAggr,
                                                  aAggrDepth,
                                                  & sExpr,
                                                  aEnv )
                             == STL_SUCCESS );

                    /* add read column list */
                    STL_TRY( qlnoAddReadColToInstant( aInstantTable,
                                                      sExpr,
                                                      aResultExpr,
                                                      QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                      aEnv )
                             == STL_SUCCESS );
                }
                
                break;
            }
        case QLV_EXPR_TYPE_FUNCTION :
            {
                if( aHasNestedAggr == STL_TRUE )
                {
                    if( aAggrDepth == 0 )
                    {
                        for( sLoop = 0 ; sLoop < sExpr->mExpr.mFunction->mArgCount ; sLoop++ )
                        {
                            STL_TRY( qlvValidateValueExprByGroup( aStmtInfo,
                                                                  aInstantTable,
                                                                  sExpr->mExpr.mFunction->mArgs[ sLoop ],
                                                                  aHasNestedAggr,
                                                                  aAggrDepth,
                                                                  & sNewExpr,
                                                                  aEnv )
                                     == STL_SUCCESS );

                            sExpr->mExpr.mFunction->mArgs[ sLoop ] = sNewExpr;
                        }

                        *aResultExpr = sExpr;
                    }
                    else
                    {
                        STL_DASSERT( aAggrDepth <= 2 );

                        for( sLoop = 0 ; sLoop < sExpr->mExpr.mFunction->mArgCount ; sLoop++ )
                        {
                            STL_TRY( qlvValidateValueExprByGroup( aStmtInfo,
                                                                  aInstantTable,
                                                                  sExpr->mExpr.mFunction->mArgs[ sLoop ],
                                                                  aHasNestedAggr,
                                                                  aAggrDepth,
                                                                  & sNewExpr,
                                                                  aEnv )
                                     == STL_SUCCESS );

                            sExpr->mExpr.mFunction->mArgs[ sLoop ] = sNewExpr;
                        }
                            
                        *aResultExpr = sExpr;
                    }
                }
                else
                {
                    STL_DASSERT( aAggrDepth <= 1 );

                    STL_TRY( qlnoIsExistExprFromInstant( aInstantTable,
                                                         sExpr,
                                                         STL_TRUE,  /* key column list */
                                                         & sIsExist,
                                                         NULL,
                                                         QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                         aEnv )
                             == STL_SUCCESS );

                    if( sIsExist )
                    {
                        /* add read column list */
                        STL_TRY( qlnoAddReadColToInstant( aInstantTable,
                                                          sExpr,
                                                          aResultExpr,
                                                          QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                          aEnv )
                                 == STL_SUCCESS );
                    }
                    else
                    {
                        for( sLoop = 0 ; sLoop < sExpr->mExpr.mFunction->mArgCount ; sLoop++ )
                        {
                            STL_TRY( qlvValidateValueExprByGroup( aStmtInfo,
                                                                  aInstantTable,
                                                                  sExpr->mExpr.mFunction->mArgs[ sLoop ],
                                                                  aHasNestedAggr,
                                                                  aAggrDepth,
                                                                  & sNewExpr,
                                                                  aEnv )
                                     == STL_SUCCESS );

                            sExpr->mExpr.mFunction->mArgs[ sLoop ] = sNewExpr;
                        }

                        *aResultExpr = sExpr;
                    }
                }
                
                break;
            }
        case QLV_EXPR_TYPE_CAST :
            {
                if( aHasNestedAggr == STL_TRUE )
                {
                    if( aAggrDepth == 0 )
                    {
                        for( sLoop = 0 ; sLoop < DTL_CAST_INPUT_ARG_CNT ; sLoop++ )
                        {
                            STL_TRY( qlvValidateValueExprByGroup( aStmtInfo,
                                                                  aInstantTable,
                                                                  sExpr->mExpr.mTypeCast->mArgs[ sLoop ],
                                                                  aHasNestedAggr,
                                                                  aAggrDepth,
                                                                  & sNewExpr,
                                                                  aEnv )
                                     == STL_SUCCESS );

                            sExpr->mExpr.mTypeCast->mArgs[ sLoop ] = sNewExpr;
                        }

                        *aResultExpr = sExpr;
                    }
                    else
                    {
                        STL_DASSERT( aAggrDepth <= 2 );

                        for( sLoop = 0 ; sLoop < DTL_CAST_INPUT_ARG_CNT ; sLoop++ )
                        {
                            STL_TRY( qlvValidateValueExprByGroup( aStmtInfo,
                                                                  aInstantTable,
                                                                  sExpr->mExpr.mTypeCast->mArgs[ sLoop ],
                                                                  aHasNestedAggr,
                                                                  aAggrDepth,
                                                                  & sNewExpr,
                                                                  aEnv )
                                     == STL_SUCCESS );

                            sExpr->mExpr.mTypeCast->mArgs[ sLoop ] = sNewExpr;
                        }

                        *aResultExpr = sExpr;
                    }
                }
                else
                {
                    STL_DASSERT( aAggrDepth <= 1 );

                    STL_TRY( qlnoIsExistExprFromInstant( aInstantTable,
                                                         sExpr,
                                                         STL_TRUE,  /* key column list */
                                                         & sIsExist,
                                                         NULL,
                                                         QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                         aEnv )
                             == STL_SUCCESS );

                    if( sIsExist )
                    {
                        /* add read column list */
                        STL_TRY( qlnoAddReadColToInstant( aInstantTable,
                                                          sExpr,
                                                          aResultExpr,
                                                          QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                          aEnv )
                                 == STL_SUCCESS );
                    }
                    else
                    {
                        for( sLoop = 0 ; sLoop < DTL_CAST_INPUT_ARG_CNT ; sLoop++ )
                        {
                            STL_TRY( qlvValidateValueExprByGroup( aStmtInfo,
                                                                  aInstantTable,
                                                                  sExpr->mExpr.mTypeCast->mArgs[ sLoop ],
                                                                  aHasNestedAggr,
                                                                  aAggrDepth,
                                                                  & sNewExpr,
                                                                  aEnv )
                                     == STL_SUCCESS );

                            sExpr->mExpr.mTypeCast->mArgs[ sLoop ] = sNewExpr;
                        }

                        *aResultExpr = sExpr;
                    }
                }
                break;
            }
        case QLV_EXPR_TYPE_CASE_EXPR :
            {
                if( aHasNestedAggr == STL_TRUE )
                {
                    if( aAggrDepth == 0 )
                    {
                        for( sLoop = 0 ; sLoop < sExpr->mExpr.mCaseExpr->mCount ; sLoop++ )
                        {
                            STL_TRY( qlvValidateValueExprByGroup( aStmtInfo,
                                                                  aInstantTable,
                                                                  sExpr->mExpr.mCaseExpr->mWhenArr[ sLoop ],
                                                                  aHasNestedAggr,
                                                                  aAggrDepth,
                                                                  & sNewExpr,
                                                                  aEnv )
                                     == STL_SUCCESS );
                            sExpr->mExpr.mCaseExpr->mWhenArr[ sLoop ] = sNewExpr;

                            STL_TRY( qlvValidateValueExprByGroup( aStmtInfo,
                                                                  aInstantTable,
                                                                  sExpr->mExpr.mCaseExpr->mThenArr[ sLoop ],
                                                                  aHasNestedAggr,
                                                                  aAggrDepth,
                                                                  & sNewExpr,
                                                                  aEnv )
                                     == STL_SUCCESS );
                            sExpr->mExpr.mCaseExpr->mThenArr[ sLoop ] = sNewExpr;
                        }

                        STL_TRY( qlvValidateValueExprByGroup( aStmtInfo,
                                                              aInstantTable,
                                                              sExpr->mExpr.mCaseExpr->mDefResult,
                                                              aHasNestedAggr,
                                                              aAggrDepth,
                                                              & sNewExpr,
                                                              aEnv )
                                 == STL_SUCCESS );
                        sExpr->mExpr.mCaseExpr->mDefResult = sNewExpr;

                        *aResultExpr = sExpr;
                    }
                    else
                    {
                        STL_DASSERT( aAggrDepth <= 2 );

                        for( sLoop = 0 ; sLoop < sExpr->mExpr.mCaseExpr->mCount ; sLoop++ )
                        {
                            STL_TRY( qlvValidateValueExprByGroup( aStmtInfo,
                                                                  aInstantTable,
                                                                  sExpr->mExpr.mCaseExpr->mWhenArr[ sLoop ],
                                                                  aHasNestedAggr,
                                                                  aAggrDepth,
                                                                  & sNewExpr,
                                                                  aEnv )
                                     == STL_SUCCESS );
                            sExpr->mExpr.mCaseExpr->mWhenArr[ sLoop ] = sNewExpr;

                            STL_TRY( qlvValidateValueExprByGroup( aStmtInfo,
                                                                  aInstantTable,
                                                                  sExpr->mExpr.mCaseExpr->mThenArr[ sLoop ],
                                                                  aHasNestedAggr,
                                                                  aAggrDepth,
                                                                  & sNewExpr,
                                                                  aEnv )
                                     == STL_SUCCESS );
                            sExpr->mExpr.mCaseExpr->mThenArr[ sLoop ] = sNewExpr;
                        }

                        STL_TRY( qlvValidateValueExprByGroup( aStmtInfo,
                                                              aInstantTable,
                                                              sExpr->mExpr.mCaseExpr->mDefResult,
                                                              aHasNestedAggr,
                                                              aAggrDepth,
                                                              & sNewExpr,
                                                              aEnv )
                                 == STL_SUCCESS );
                        sExpr->mExpr.mCaseExpr->mDefResult = sNewExpr;

                        *aResultExpr = sExpr;
                    }
                }
                else
                {
                    STL_DASSERT( aAggrDepth <= 2 );

                    STL_TRY( qlnoIsExistExprFromInstant( aInstantTable,
                                                         sExpr,
                                                         STL_TRUE,  /* key column list */
                                                         & sIsExist,
                                                         NULL,
                                                         QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                         aEnv )
                             == STL_SUCCESS );

                    if( sIsExist )
                    {
                        /* add read column list */
                        STL_TRY( qlnoAddReadColToInstant( aInstantTable,
                                                          sExpr,
                                                          aResultExpr,
                                                          QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                          aEnv )
                                 == STL_SUCCESS );
                    }
                    else
                    {
                        for( sLoop = 0 ; sLoop < sExpr->mExpr.mCaseExpr->mCount ; sLoop++ )
                        {
                            STL_TRY( qlvValidateValueExprByGroup( aStmtInfo,
                                                                  aInstantTable,
                                                                  sExpr->mExpr.mCaseExpr->mWhenArr[ sLoop ],
                                                                  aHasNestedAggr,
                                                                  aAggrDepth,
                                                                  & sNewExpr,
                                                                  aEnv )
                                     == STL_SUCCESS );
                            sExpr->mExpr.mCaseExpr->mWhenArr[ sLoop ] = sNewExpr;

                            STL_TRY( qlvValidateValueExprByGroup( aStmtInfo,
                                                                  aInstantTable,
                                                                  sExpr->mExpr.mCaseExpr->mThenArr[ sLoop ],
                                                                  aHasNestedAggr,
                                                                  aAggrDepth,
                                                                  & sNewExpr,
                                                                  aEnv )
                                     == STL_SUCCESS );
                            sExpr->mExpr.mCaseExpr->mThenArr[ sLoop ] = sNewExpr;
                        }

                        STL_TRY( qlvValidateValueExprByGroup( aStmtInfo,
                                                              aInstantTable,
                                                              sExpr->mExpr.mCaseExpr->mDefResult,
                                                              aHasNestedAggr,
                                                              aAggrDepth,
                                                              & sNewExpr,
                                                              aEnv )
                                 == STL_SUCCESS );
                        sExpr->mExpr.mCaseExpr->mDefResult = sNewExpr;

                        *aResultExpr = sExpr;
                    }
                }
                
                break;
            }
        case QLV_EXPR_TYPE_LIST_FUNCTION :
            {
                if( aHasNestedAggr == STL_TRUE )
                {
                    if( aAggrDepth == 0 )
                    {
                        for( sLoop = 0 ; sLoop < sExpr->mExpr.mListFunc->mArgCount ; sLoop++ )
                        {
                            STL_TRY( qlvValidateValueExprByGroup( aStmtInfo,
                                                                  aInstantTable,
                                                                  sExpr->mExpr.mListFunc->mArgs[ sLoop ],
                                                                  aHasNestedAggr,
                                                                  aAggrDepth,
                                                                  & sNewExpr,
                                                                  aEnv )
                                     == STL_SUCCESS );

                            sExpr->mExpr.mListFunc->mArgs[ sLoop ] = sNewExpr;
                        }

                        *aResultExpr = sExpr;
                    }
                    else
                    {
                        STL_DASSERT( aAggrDepth <= 2 );

                        for( sLoop = 0 ; sLoop < sExpr->mExpr.mListFunc->mArgCount ; sLoop++ )
                        {
                            STL_TRY( qlvValidateValueExprByGroup( aStmtInfo,
                                                                  aInstantTable,
                                                                  sExpr->mExpr.mListFunc->mArgs[ sLoop ],
                                                                  aHasNestedAggr,
                                                                  aAggrDepth,
                                                                  & sNewExpr,
                                                                  aEnv )
                                     == STL_SUCCESS );

                            sExpr->mExpr.mListFunc->mArgs[ sLoop ] = sNewExpr;
                        }

                        *aResultExpr = sExpr;
                    }
                }
                else
                {
                    STL_DASSERT( aAggrDepth <= 1 );

                    STL_TRY( qlnoIsExistExprFromInstant( aInstantTable,
                                                         sExpr,
                                                         STL_TRUE,  /* key column list */
                                                         & sIsExist,
                                                         NULL,
                                                         QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                         aEnv )
                             == STL_SUCCESS );

                    if( sIsExist )
                    {
                        /* add read column list */
                        STL_TRY( qlnoAddReadColToInstant( aInstantTable,
                                                          sExpr,
                                                          aResultExpr,
                                                          QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                          aEnv )
                                 == STL_SUCCESS );
                    }
                    else
                    {
                        for( sLoop = 0 ; sLoop < sExpr->mExpr.mListFunc->mArgCount ; sLoop++ )
                        {
                            STL_TRY( qlvValidateValueExprByGroup( aStmtInfo,
                                                                  aInstantTable,
                                                                  sExpr->mExpr.mListFunc->mArgs[ sLoop ],
                                                                  aHasNestedAggr,
                                                                  aAggrDepth,
                                                                  & sNewExpr,
                                                                  aEnv )
                                     == STL_SUCCESS );

                            sExpr->mExpr.mListFunc->mArgs[ sLoop ] = sNewExpr;
                        }

                        *aResultExpr = sExpr;
                    }
                }
                break;
            }
        case QLV_EXPR_TYPE_LIST_COLUMN :
            {
                if( aHasNestedAggr == STL_TRUE )
                {
                    if( aAggrDepth == 0 )
                    {
                        for( sLoop = 0 ; sLoop < sExpr->mExpr.mListCol->mArgCount ; sLoop++ )
                        {
                            STL_TRY( qlvValidateValueExprByGroup( aStmtInfo,
                                                                  aInstantTable,
                                                                  sExpr->mExpr.mListCol->mArgs[ sLoop ],
                                                                  aHasNestedAggr,
                                                                  aAggrDepth,
                                                                  & sNewExpr,
                                                                  aEnv )
                                     == STL_SUCCESS );

                            sExpr->mExpr.mListCol->mArgs[ sLoop ] = sNewExpr;
                        }

                        *aResultExpr = sExpr;
                    }
                    else
                    {
                        STL_DASSERT( aAggrDepth <= 2 );

                        for( sLoop = 0 ; sLoop < sExpr->mExpr.mListCol->mArgCount ; sLoop++ )
                        {
                            STL_TRY( qlvValidateValueExprByGroup( aStmtInfo,
                                                                  aInstantTable,
                                                                  sExpr->mExpr.mListCol->mArgs[ sLoop ],
                                                                  aHasNestedAggr,
                                                                  aAggrDepth,
                                                                  & sNewExpr,
                                                                  aEnv )
                                     == STL_SUCCESS );

                            sExpr->mExpr.mListCol->mArgs[ sLoop ] = sNewExpr;
                        }

                        *aResultExpr = sExpr;
                    }
                }
                else
                {
                    STL_DASSERT( aAggrDepth <= 1 );

                    for( sLoop = 0 ; sLoop < sExpr->mExpr.mListCol->mArgCount ; sLoop++ )
                    {
                        STL_TRY( qlvValidateValueExprByGroup( aStmtInfo,
                                                              aInstantTable,
                                                              sExpr->mExpr.mListCol->mArgs[ sLoop ],
                                                              aHasNestedAggr,
                                                              aAggrDepth,
                                                              & sNewExpr,
                                                              aEnv )
                                 == STL_SUCCESS );
                        
                        sExpr->mExpr.mListCol->mArgs[ sLoop ] = sNewExpr;
                    }
                    
                    *aResultExpr = sExpr;
                }
                break;
            }
        case QLV_EXPR_TYPE_ROW_EXPR :
            {
                if( aHasNestedAggr == STL_TRUE )
                {
                    if( aAggrDepth == 0 )
                    {
                        for( sLoop = 0 ; sLoop < sExpr->mExpr.mRowExpr->mArgCount ; sLoop++ )
                        {
                            STL_TRY( qlvValidateValueExprByGroup( aStmtInfo,
                                                                  aInstantTable,
                                                                  sExpr->mExpr.mRowExpr->mArgs[ sLoop ],
                                                                  aHasNestedAggr,
                                                                  aAggrDepth,
                                                                  & sNewExpr,
                                                                  aEnv )
                                     == STL_SUCCESS );

                            sExpr->mExpr.mRowExpr->mArgs[ sLoop ] = sNewExpr;
                        }

                        *aResultExpr = sExpr;
                    }
                    else
                    {
                        STL_DASSERT( aAggrDepth <= 2 );

                        for( sLoop = 0 ; sLoop < sExpr->mExpr.mRowExpr->mArgCount ; sLoop++ )
                        {
                            STL_TRY( qlvValidateValueExprByGroup( aStmtInfo,
                                                                  aInstantTable,
                                                                  sExpr->mExpr.mRowExpr->mArgs[ sLoop ],
                                                                  aHasNestedAggr,
                                                                  aAggrDepth,
                                                                  & sNewExpr,
                                                                  aEnv )
                                     == STL_SUCCESS );

                            sExpr->mExpr.mRowExpr->mArgs[ sLoop ] = sNewExpr;
                        }

                        *aResultExpr = sExpr;
                    }
                }
                else
                {
                    STL_DASSERT( aAggrDepth <= 1 );

                    for( sLoop = 0 ; sLoop < sExpr->mExpr.mRowExpr->mArgCount ; sLoop++ )
                    {
                        STL_TRY( qlvValidateValueExprByGroup( aStmtInfo,
                                                              aInstantTable,
                                                              sExpr->mExpr.mRowExpr->mArgs[ sLoop ],
                                                              aHasNestedAggr,
                                                              aAggrDepth,
                                                              & sNewExpr,
                                                              aEnv )
                                 == STL_SUCCESS );

                        sExpr->mExpr.mRowExpr->mArgs[ sLoop ] = sNewExpr;
                    }

                    *aResultExpr = sExpr;
                }
                break;
            }
        case QLV_EXPR_TYPE_SUB_QUERY :
            {
                *aResultExpr = sExpr;
                STL_TRY_THROW( aHasNestedAggr == STL_FALSE,
                               RAMP_ERR_SUB_QUERY_NOT_ALLOWED );
                break;
            }
        case QLV_EXPR_TYPE_PSEUDO_COLUMN :
            {
                STL_THROW( RAMP_ERR_PSEUDO_COLUMN_NOT_ALLOWED );
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
    
    STL_CATCH( RAMP_ERR_SUB_QUERY_NOT_ALLOWED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SUB_QUERY_NOT_ALLOWED,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sExpr->mPosition,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_NOT_A_GROUP_FUNCTION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_A_GROUP_FUNCTION,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sExpr->mPosition,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_NESTED_GROUP_IS_NESTED_TOO_DEEPLY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_GROUP_FUNCTION_IS_NESTED_TOO_DEEPLY,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sExpr->mPosition,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_PSEUDO_COLUMN_NOT_ALLOWED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SEQUENCE_NUMBER_NOT_ALLOWED,
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
 * @param[in]      aStmtInfo                  Stmt Information
 * @param[in]      aInstantTable              Group Instant Table
 * @param[in]      aExpr                      Expression
 * @param[in]      aHasNestedAggr             Nested Aggregation 포함 여부
 * @param[in]      aAggrDepth                 Aggregation Depth
 * @param[out]     aResultExpr                Result Expression
 * @param[in]      aEnv                       Environment
 */
stlStatus qlvCheckExprByGroup( qlvStmtInfo          * aStmtInfo,
                               qlvInitInstantNode   * aInstantTable,
                               qlvInitExpression    * aExpr,
                               stlBool                aHasNestedAggr,
                               stlInt32               aAggrDepth,
                               qlvInitExpression   ** aResultExpr,
                               qllEnv               * aEnv )
{
    stlInt32             sLoop      = 0;
    qlvInitExpression  * sExpr      = aExpr;
    stlBool              sIsExist   = STL_FALSE;
    

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInstantTable != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aResultExpr != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * 동일한 Column Expression이 Group By 절에 있는지 검사 (사용가능 여부도 검사)
     * => 없으면 추가
     */

    STL_DASSERT( aAggrDepth <= 2 );
    
    switch( sExpr->mType )
    {
        case QLV_EXPR_TYPE_VALUE :
        case QLV_EXPR_TYPE_BIND_PARAM :
        case QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT :
        case QLV_EXPR_TYPE_OUTER_COLUMN :
        case QLV_EXPR_TYPE_REFERENCE :
        case QLV_EXPR_TYPE_INNER_COLUMN :
        case QLV_EXPR_TYPE_COLUMN :
        case QLV_EXPR_TYPE_ROWID_COLUMN :
            {
                *aResultExpr = sExpr;
                break;
            }
        case QLV_EXPR_TYPE_AGGREGATION :
            {
                if( aHasNestedAggr == STL_TRUE )
                {
                    STL_DASSERT( aAggrDepth == 1 );
                    STL_DASSERT( sExpr->mExpr.mAggregation->mHasNestedAggr == STL_FALSE );
                }
                else
                {
                    STL_DASSERT( aAggrDepth == 0 );
                    STL_DASSERT( sExpr->mExpr.mAggregation->mHasNestedAggr == STL_FALSE );
                }
                
                aAggrDepth++;
                for( sLoop = 0 ; sLoop < sExpr->mExpr.mAggregation->mArgCount ; sLoop++ )
                {
                    STL_TRY( qlvCheckExprByGroup( aStmtInfo,
                                                  aInstantTable,
                                                  sExpr->mExpr.mAggregation->mArgs[ sLoop ],
                                                  aHasNestedAggr,
                                                  aAggrDepth,
                                                  & sExpr->mExpr.mAggregation->mArgs[ sLoop ],
                                                  aEnv )
                             == STL_SUCCESS );
                }
                
                *aResultExpr = sExpr;
                break;
            }
        case QLV_EXPR_TYPE_FUNCTION :
            {
                STL_TRY( qlnoIsExistExprFromInstant( aInstantTable,
                                                     sExpr,
                                                     STL_TRUE,  /* key column list */
                                                     & sIsExist,
                                                     & sExpr,
                                                     QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                     aEnv )
                         == STL_SUCCESS );

                if( sIsExist == STL_FALSE )
                {
                    for( sLoop = 0 ; sLoop < sExpr->mExpr.mFunction->mArgCount ; sLoop++ )
                    {
                        STL_TRY( qlvCheckExprByGroup( aStmtInfo,
                                                      aInstantTable,
                                                      sExpr->mExpr.mFunction->mArgs[ sLoop ],
                                                      aHasNestedAggr,
                                                      aAggrDepth,
                                                      & sExpr->mExpr.mFunction->mArgs[ sLoop ],
                                                      aEnv )
                                 == STL_SUCCESS );
                    }
                }
                else
                {
                    /* Do Nothing */
                }
                
                *aResultExpr = sExpr;
                break;
            }
        case QLV_EXPR_TYPE_CAST :
            {
                STL_TRY( qlnoIsExistExprFromInstant( aInstantTable,
                                                     sExpr,
                                                     STL_TRUE,  /* key column list */
                                                     & sIsExist,
                                                     & sExpr,
                                                     QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                     aEnv )
                         == STL_SUCCESS );

                if( sIsExist == STL_FALSE )
                {
                    for( sLoop = 0 ; sLoop < DTL_CAST_INPUT_ARG_CNT ; sLoop++ )
                    {
                        STL_TRY( qlvCheckExprByGroup( aStmtInfo,
                                                      aInstantTable,
                                                      sExpr->mExpr.mTypeCast->mArgs[ sLoop ],
                                                      aHasNestedAggr,
                                                      aAggrDepth,
                                                      & sExpr->mExpr.mTypeCast->mArgs[ sLoop ],
                                                      aEnv )
                                 == STL_SUCCESS );
                    }
                }
                else
                {
                    /* Do Nothing */
                }

                *aResultExpr = sExpr;
                break;
            }
        case QLV_EXPR_TYPE_CASE_EXPR :
            {
                STL_TRY( qlnoIsExistExprFromInstant( aInstantTable,
                                                     sExpr,
                                                     STL_TRUE,  /* key column list */
                                                     & sIsExist,
                                                     & sExpr,
                                                     QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                     aEnv )
                         == STL_SUCCESS );

                if( sIsExist == STL_FALSE )
                {
                    for( sLoop = 0 ; sLoop < sExpr->mExpr.mCaseExpr->mCount ; sLoop++ )
                    {
                        STL_TRY( qlvCheckExprByGroup( aStmtInfo,
                                                      aInstantTable,
                                                      sExpr->mExpr.mCaseExpr->mWhenArr[ sLoop ],
                                                      aHasNestedAggr,
                                                      aAggrDepth,
                                                      & sExpr->mExpr.mCaseExpr->mWhenArr[ sLoop ],
                                                      aEnv )
                                 == STL_SUCCESS );

                        STL_TRY( qlvCheckExprByGroup( aStmtInfo,
                                                      aInstantTable,
                                                      sExpr->mExpr.mCaseExpr->mThenArr[ sLoop ],
                                                      aHasNestedAggr,
                                                      aAggrDepth,
                                                      & sExpr->mExpr.mCaseExpr->mThenArr[ sLoop ],
                                                      aEnv )
                                 == STL_SUCCESS );
                    }

                    STL_TRY( qlvCheckExprByGroup( aStmtInfo,
                                                  aInstantTable,
                                                  sExpr->mExpr.mCaseExpr->mDefResult,
                                                  aHasNestedAggr,
                                                  aAggrDepth,
                                                  & sExpr->mExpr.mCaseExpr->mDefResult,
                                                  aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    /* Do Nothing */
                }
                
                *aResultExpr = sExpr;
                break;
            }
        case QLV_EXPR_TYPE_LIST_FUNCTION :
            {
                STL_TRY( qlnoIsExistExprFromInstant( aInstantTable,
                                                     sExpr,
                                                     STL_TRUE,  /* key column list */
                                                     & sIsExist,
                                                     & sExpr,
                                                     QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                     aEnv )
                         == STL_SUCCESS );

                if( sIsExist == STL_FALSE )
                {
                    for( sLoop = 0 ; sLoop < sExpr->mExpr.mListFunc->mArgCount ; sLoop++ )
                    {
                        STL_TRY( qlvCheckExprByGroup( aStmtInfo,
                                                      aInstantTable,
                                                      sExpr->mExpr.mListFunc->mArgs[ sLoop ],
                                                      aHasNestedAggr,
                                                      aAggrDepth,
                                                      & sExpr->mExpr.mListFunc->mArgs[ sLoop ],
                                                      aEnv )
                                 == STL_SUCCESS );
                    }
                }
                else
                {
                    /* Do Nothing */
                }
                
                *aResultExpr = sExpr;
                break;
            }
        case QLV_EXPR_TYPE_LIST_COLUMN :
            {
                for( sLoop = 0 ; sLoop < sExpr->mExpr.mListCol->mArgCount ; sLoop++ )
                {
                    STL_TRY( qlvCheckExprByGroup( aStmtInfo,
                                                  aInstantTable,
                                                  sExpr->mExpr.mListCol->mArgs[ sLoop ],
                                                  aHasNestedAggr,
                                                  aAggrDepth,
                                                  & sExpr->mExpr.mListCol->mArgs[ sLoop ],
                                                  aEnv )
                             == STL_SUCCESS );
                }

                *aResultExpr = sExpr;
                break;
            }
        case QLV_EXPR_TYPE_ROW_EXPR :
            {
                for( sLoop = 0 ; sLoop < sExpr->mExpr.mRowExpr->mArgCount ; sLoop++ )
                {
                    STL_TRY( qlvCheckExprByGroup( aStmtInfo,
                                                  aInstantTable,
                                                  sExpr->mExpr.mRowExpr->mArgs[ sLoop ],
                                                  aHasNestedAggr,
                                                  aAggrDepth,
                                                  & sExpr->mExpr.mRowExpr->mArgs[ sLoop ],
                                                  aEnv )
                             == STL_SUCCESS );
                }

                *aResultExpr = sExpr;
                break;
            }
        case QLV_EXPR_TYPE_SUB_QUERY :
            {
                *aResultExpr = sExpr;
                break;
            }
        case QLV_EXPR_TYPE_PSEUDO_COLUMN :
        case QLV_EXPR_TYPE_PSEUDO_ARGUMENT :
        default :
            {
                STL_DASSERT( 0 );
                break;
            }
    }


    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}



/** @} qlnv */

